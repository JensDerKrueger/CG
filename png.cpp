#include "png.h"
#include "Image.h"

#include <array>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <vector>
#include <algorithm>

namespace {

  constexpr std::array<uint8_t, 8> kPngSignature = {
    0x89, 0x50, 0x4E, 0x47, 0x0D, 0x0A, 0x1A, 0x0A
  };

  uint32_t toBigEndianU32(uint32_t v) {
    return ((v & 0x000000FFu) << 24) |
    ((v & 0x0000FF00u) << 8)  |
    ((v & 0x00FF0000u) >> 8)  |
    ((v & 0xFF000000u) >> 24);
  }

  void writeBytes(std::FILE* f, const void* data, size_t size) {
    if (size == 0) return;
    (void)std::fwrite(data, 1, size, f);
  }

  void writeU32(std::FILE* f, uint32_t vBigEndian) {
    writeBytes(f, &vBigEndian, sizeof(vBigEndian));
  }

  // ---------- CRC32 (PNG chunks) ----------

  uint32_t crc32Update(uint32_t crc, const uint8_t* data, size_t len) {
    static uint32_t table[256];
    static bool tableInit = false;

    if (!tableInit) {
      for (uint32_t i = 0; i < 256; ++i) {
        uint32_t c = i;
        for (int k = 0; k < 8; ++k) {
          c = (c & 1u) ? (0xEDB88320u ^ (c >> 1)) : (c >> 1);
        }
        table[i] = c;
      }
      tableInit = true;
    }

    uint32_t c = crc;
    for (size_t i = 0; i < len; ++i) {
      c = table[(c ^ data[i]) & 0xFFu] ^ (c >> 8);
    }
    return c;
  }

  void writeChunk(std::FILE* f, const char type[4],
                  const uint8_t* payload, uint32_t payloadSize) {
    const uint32_t lengthBE = toBigEndianU32(payloadSize);
    writeU32(f, lengthBE);

    writeBytes(f, type, 4);
    if (payloadSize > 0) writeBytes(f, payload, payloadSize);

    uint32_t c = 0xFFFFFFFFu;
    c = crc32Update(c, reinterpret_cast<const uint8_t*>(type), 4);
    if (payloadSize > 0) c = crc32Update(c, payload, payloadSize);
    c ^= 0xFFFFFFFFu;

    const uint32_t crcBE = toBigEndianU32(c);
    writeU32(f, crcBE);
  }

  // ---------- Adler32 (zlib trailer) ----------

  uint32_t adler32(const uint8_t* data, size_t len) {
    constexpr uint32_t modAdler = 65521u;
    uint32_t a = 1u;
    uint32_t b = 0u;
    for (size_t i = 0; i < len; ++i) {
      a += data[i];
      if (a >= modAdler) a -= modAdler;
      b += a;
      b %= modAdler;
    }
    return (b << 16) | a;
  }

  // ---------- PNG filters ----------

  uint8_t paethPredictor(uint8_t a, uint8_t b, uint8_t c) {
    const int p  = int(a) + int(b) - int(c);
    const int pa = std::abs(p - int(a));
    const int pb = std::abs(p - int(b));
    const int pc = std::abs(p - int(c));
    if (pa <= pb && pa <= pc) return a;
    if (pb <= pc) return b;
    return c;
  }

  int filterScore(const std::vector<uint8_t>& rowFiltered) {
    int s = 0;
    for (uint8_t v : rowFiltered) {
      const int8_t sv = static_cast<int8_t>(v);
      s += std::abs(int(sv));
    }
    return s;
  }

  void applyFilterNone(std::vector<uint8_t>& out,
                       const uint8_t* cur, const uint8_t* /*prev*/,
                       size_t rowBytes, size_t /*bpp*/) {
    out.resize(rowBytes);
    std::memcpy(out.data(), cur, rowBytes);
  }

  void applyFilterSub(std::vector<uint8_t>& out,
                      const uint8_t* cur, const uint8_t* /*prev*/,
                      size_t rowBytes, size_t bpp) {
    out.resize(rowBytes);
    for (size_t i = 0; i < rowBytes; ++i) {
      const uint8_t left = (i >= bpp) ? cur[i - bpp] : 0;
      out[i] = uint8_t(cur[i] - left);
    }
  }

  void applyFilterUp(std::vector<uint8_t>& out,
                     const uint8_t* cur, const uint8_t* prev,
                     size_t rowBytes, size_t /*bpp*/) {
    out.resize(rowBytes);
    for (size_t i = 0; i < rowBytes; ++i) {
      const uint8_t up = prev ? prev[i] : 0;
      out[i] = uint8_t(cur[i] - up);
    }
  }

  void applyFilterAverage(std::vector<uint8_t>& out,
                          const uint8_t* cur, const uint8_t* prev,
                          size_t rowBytes, size_t bpp) {
    out.resize(rowBytes);
    for (size_t i = 0; i < rowBytes; ++i) {
      const uint8_t left = (i >= bpp) ? cur[i - bpp] : 0;
      const uint8_t up   = prev ? prev[i] : 0;
      const uint8_t avg  = uint8_t((int(left) + int(up)) / 2);
      out[i] = uint8_t(cur[i] - avg);
    }
  }

  void applyFilterPaeth(std::vector<uint8_t>& out,
                        const uint8_t* cur, const uint8_t* prev,
                        size_t rowBytes, size_t bpp) {
    out.resize(rowBytes);
    for (size_t i = 0; i < rowBytes; ++i) {
      const uint8_t a = (i >= bpp) ? cur[i - bpp] : 0;
      const uint8_t b = prev ? prev[i] : 0;
      const uint8_t c = (prev && i >= bpp) ? prev[i - bpp] : 0;
      const uint8_t p = paethPredictor(a, b, c);
      out[i] = uint8_t(cur[i] - p);
    }
  }

  std::vector<uint8_t> buildFilteredScanlines(const Image& image) {
    const uint32_t w = image.width;
    const uint32_t h = image.height;
    const size_t bpp = size_t(image.componentCount);
    const size_t rowBytes = size_t(w) * bpp;

    std::vector<uint8_t> out;
    out.reserve(size_t(h) * (1 + rowBytes));

    std::vector<uint8_t> candNone, candSub, candUp, candAvg, candPaeth;

    for (uint32_t y = 0; y < h; ++y) {
      const uint8_t* cur  = image.data.data() + size_t(y) * rowBytes;
      const uint8_t* prev = (y > 0) ? (image.data.data() + size_t(y - 1) * rowBytes) : nullptr;

      applyFilterNone(candNone, cur, prev, rowBytes, bpp);
      applyFilterSub(candSub, cur, prev, rowBytes, bpp);
      applyFilterUp(candUp, cur, prev, rowBytes, bpp);
      applyFilterAverage(candAvg, cur, prev, rowBytes, bpp);
      applyFilterPaeth(candPaeth, cur, prev, rowBytes, bpp);

      const int s0 = filterScore(candNone);
      const int s1 = filterScore(candSub);
      const int s2 = filterScore(candUp);
      const int s3 = filterScore(candAvg);
      const int s4 = filterScore(candPaeth);

      uint8_t filterType = 0;
      const std::vector<uint8_t>* best = &candNone;
      int bestScore = s0;

      auto pick = [&](uint8_t t, const std::vector<uint8_t>& v, int sc) {
        if (sc < bestScore) { bestScore = sc; filterType = t; best = &v; }
      };

      pick(1, candSub,   s1);
      pick(2, candUp,    s2);
      pick(3, candAvg,   s3);
      pick(4, candPaeth, s4);

      out.push_back(filterType);
      out.insert(out.end(), best->begin(), best->end());
    }

    return out;
  }

  // ---------- DEFLATE fixed Huffman encoder (zlib wrapper) ----------

  struct BitWriter {
    std::vector<uint8_t> data;
    uint32_t bitBuf = 0;
    int bitCount = 0;

    void writeBits(uint32_t bits, int n) {
      // LSB-first
      bitBuf |= (bits << bitCount);
      bitCount += n;
      while (bitCount >= 8) {
        data.push_back(uint8_t(bitBuf & 0xFFu));
        bitBuf >>= 8;
        bitCount -= 8;
      }
    }

    void flushToByte() {
      if (bitCount > 0) {
        data.push_back(uint8_t(bitBuf & 0xFFu));
        bitBuf = 0;
        bitCount = 0;
      }
    }
  };

  uint32_t reverseBits(uint32_t x, int n) {
    uint32_t r = 0;
    for (int i = 0; i < n; ++i) {
      r = (r << 1) | (x & 1u);
      x >>= 1;
    }
    return r;
  }

  // Fixed Huffman codes per RFC1951
  void fixedLitLenCode(int sym, uint32_t& code, int& nbits) {
    if (sym <= 143) {
      nbits = 8;
      code = 0x30u + uint32_t(sym);
    } else if (sym <= 255) {
      nbits = 9;
      code = 0x190u + uint32_t(sym - 144);
    } else if (sym <= 279) {
      nbits = 7;
      code = uint32_t(sym - 256);
    } else {
      nbits = 8;
      code = 0xC0u + uint32_t(sym - 280);
    }
    code = reverseBits(code, nbits);
  }

  void fixedDistCode(int sym, uint32_t& code, int& nbits) {
    nbits = 5;
    code = reverseBits(uint32_t(sym), 5);
  }

  struct LenSym { int sym; int extraBits; int extraValue; };
  struct DistSym { int sym; int extraBits; int extraValue; };

  LenSym encodeLength(int length) {
    static const int baseLen[29] = {
      3,4,5,6,7,8,9,10,
      11,13,15,17,
      19,23,27,31,
      35,43,51,59,
      67,83,99,115,
      131,163,195,227,258
    };
    static const int extra[29] = {
      0,0,0,0,0,0,0,0,
      1,1,1,1,
      2,2,2,2,
      3,3,3,3,
      4,4,4,4,
      5,5,5,5,0
    };

    if (length == 258) return {285, 0, 0};

    for (int i = 0; i < 28; ++i) {
      const int start = baseLen[i];
      const int end = baseLen[i + 1] - 1;
      if (length >= start && length <= end) {
        const int e = extra[i];
        const int val = length - start;
        return {257 + i, e, val};
      }
    }
    return {285, 0, 0};
  }

  DistSym encodeDistance(int dist) {
    static const int baseDist[30] = {
      1,2,3,4,5,7,9,13,
      17,25,33,49,65,97,129,193,
      257,385,513,769,1025,1537,2049,3073,
      4097,6145,8193,12289,16385,24577
    };
    static const int extra[30] = {
      0,0,0,0,1,1,2,2,
      3,3,4,4,5,5,6,6,
      7,7,8,8,9,9,10,10,
      11,11,12,12,13,13
    };

    for (int i = 0; i < 30; ++i) {
      const int start = baseDist[i];
      const int end = (i == 29) ? 32768 : (baseDist[i + 1] - 1);
      if (dist >= start && dist <= end) {
        const int e = extra[i];
        const int val = dist - start;
        return {i, e, val};
      }
    }
    return {29, 13, dist - 24577};
  }

  struct Match { int length; int distance; };

  uint32_t hash3(const uint8_t* p) {
    return (uint32_t(p[0]) * 2654435761u) ^
    (uint32_t(p[1]) * 2246822519u) ^
    (uint32_t(p[2]) * 3266489917u);
  }

  Match findMatch(const uint8_t* data, int size, int pos,
                  int headPos,
                  const std::vector<int>& prev) {
    constexpr int kMaxWindow = 32768;
    constexpr int kMaxChain = 64;
    constexpr int kMaxLen = 258;
    constexpr int kMinLen = 3;

    Match best{0, 0};
    const int limit = std::max(0, pos - kMaxWindow);

    int cur = headPos;
    int chain = 0;

    while (cur >= limit && cur >= 0 && chain < kMaxChain) {
      const int dist = pos - cur;
      if (dist <= 0) break; // must be positive

      int l = 0;
      const int maxL = std::min(kMaxLen, size - pos);
      while (l < maxL && data[cur + l] == data[pos + l]) ++l;

      if (l >= kMinLen && l > best.length) {
        best.length = l;
        best.distance = dist;
        if (l == kMaxLen) break;
      }

      cur = prev[cur];
      ++chain;
    }

    return best;
  }

  std::vector<uint8_t> zlibDeflateFixed(const uint8_t* in, size_t inSize) {
    // zlib header: CMF/FLG for DEFLATE, 32K window; low compression flag
    std::vector<uint8_t> out;
    out.reserve(inSize / 2 + 256);

    out.push_back(0x78);
    out.push_back(0x01);

    BitWriter bw;

    // Single block: BFINAL=1, BTYPE=01 (fixed Huffman)
    bw.writeBits(1, 1);
    bw.writeBits(1, 2);

    const int size = int(inSize);

    constexpr int kHashSize = 1 << 15;
    std::vector<int> head(kHashSize, -1);
    std::vector<int> prev(size, -1);

    auto hashAt = [&](int pos) -> int {
      if (pos + 2 >= size) return -1;
      uint32_t h = hash3(in + pos);
      return int(h & (kHashSize - 1));
    };

    auto insertPos = [&](int pos) {
      int h = hashAt(pos);
      if (h < 0) return;
      prev[pos] = head[h];
      head[h] = pos;
    };

    int pos = 0;
    while (pos < size) {
      Match m{0, 0};

      int h = hashAt(pos);
      if (h >= 0) {
        const int headPos = head[h]; // only earlier positions (we insert after encoding)
        if (headPos >= 0) {
          m = findMatch(in, size, pos, headPos, prev);
        }
      }

      if (m.length >= 3) {
        LenSym ls = encodeLength(m.length);
        uint32_t code; int nbits;

        fixedLitLenCode(ls.sym, code, nbits);
        bw.writeBits(code, nbits);
        if (ls.extraBits > 0) bw.writeBits(uint32_t(ls.extraValue), ls.extraBits);

        DistSym ds = encodeDistance(m.distance);
        fixedDistCode(ds.sym, code, nbits);
        bw.writeBits(code, nbits);
        if (ds.extraBits > 0) bw.writeBits(uint32_t(ds.extraValue), ds.extraBits);

        // Insert the positions we consumed (so future matches can reference them)
        const int advance = m.length;
        for (int k = 0; k < advance; ++k) {
          if (pos + k < size) insertPos(pos + k);
        }

        pos += advance;
      } else {
        uint32_t code; int nbits;
        fixedLitLenCode(int(in[pos]), code, nbits);
        bw.writeBits(code, nbits);

        insertPos(pos);
        ++pos;
      }
    }

    // End-of-block (256)
    {
      uint32_t code; int nbits;
      fixedLitLenCode(256, code, nbits);
      bw.writeBits(code, nbits);
    }

    bw.flushToByte();
    out.insert(out.end(), bw.data.begin(), bw.data.end());

    // Adler32 trailer (big-endian)
    const uint32_t ad = adler32(in, inSize);
    out.push_back(uint8_t((ad >> 24) & 0xFF));
    out.push_back(uint8_t((ad >> 16) & 0xFF));
    out.push_back(uint8_t((ad >> 8) & 0xFF));
    out.push_back(uint8_t(ad & 0xFF));

    return out;
  }

} // namespace

namespace PNG {
  bool save(const std::string& filePath, const Image& image,
                bool writeSRGBChunk,
                uint8_t srgbRenderingIntent) {
    if (image.width == 0 || image.height == 0) return false;
    if (!(image.componentCount == 3 || image.componentCount == 4)) return false;

    const size_t expected = size_t(image.width) * size_t(image.height) * size_t(image.componentCount);
    if (image.data.size() < expected) return false;

    const uint8_t colorType = (image.componentCount == 4) ? 6 : 2; // RGBA or RGB

    std::FILE* f = std::fopen(filePath.c_str(), "wb");
    if (!f) return false;

    writeBytes(f, kPngSignature.data(), kPngSignature.size());

    // IHDR
    std::array<uint8_t, 13> ihdr{};
    const uint32_t wBE = toBigEndianU32(image.width);
    const uint32_t hBE = toBigEndianU32(image.height);

    std::memcpy(&ihdr[0], &wBE, 4);
    std::memcpy(&ihdr[4], &hBE, 4);
    ihdr[8]  = 8;
    ihdr[9]  = colorType;
    ihdr[10] = 0;
    ihdr[11] = 0;
    ihdr[12] = 0;

    writeChunk(f, "IHDR", ihdr.data(), uint32_t(ihdr.size()));

    if (writeSRGBChunk) {
      uint8_t srgbPayload[1] = { srgbRenderingIntent };
      writeChunk(f, "sRGB", srgbPayload, 1);
    }

    // Filtered scanlines
    std::vector<uint8_t> filtered = buildFilteredScanlines(image);

    // zlib + deflate (fixed Huffman)
    std::vector<uint8_t> z = zlibDeflateFixed(filtered.data(), filtered.size());

    if (z.size() > 0xFFFFFFFFu) {
      std::fclose(f);
      return false;
    }

    writeChunk(f, "IDAT", z.data(), uint32_t(z.size()));
    writeChunk(f, "IEND", nullptr, 0);

    std::fclose(f);
    return true;
  }}

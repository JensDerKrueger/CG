#include "Compression.h"

#include <array>
#include <cstring>
#include <limits>

namespace Compression {

  CompressionError::CompressionError(const char* message)
  : std::runtime_error(message) {}

  CompressionError::CompressionError(const std::string& message)
  : std::runtime_error(message) {}

  static constexpr uint8_t kRleMagic[4] = {'R','L','E','F'};

  static inline void writeU64LE(std::vector<uint8_t>& out, uint64_t v) {
    for (int i = 0; i < 8; ++i) out.push_back(static_cast<uint8_t>((v >> (8 * i)) & 0xFF));
  }

  static inline uint64_t readU64LE(const uint8_t* p) {
    uint64_t v = 0;
    for (int i = 0; i < 8; ++i) v |= (static_cast<uint64_t>(p[i]) << (8 * i));
    return v;
  }

  static inline uint16_t readU16LE(const uint8_t* p) {
    return static_cast<uint16_t>(p[0] | (static_cast<uint16_t>(p[1]) << 8));
  }

  static inline void writeU16LE(std::vector<uint8_t>& out, uint16_t v) {
    out.push_back(static_cast<uint8_t>(v & 0xFF));
    out.push_back(static_cast<uint8_t>((v >> 8) & 0xFF));
  }

  static inline uint32_t readU32LE(const uint8_t* p) {
    return static_cast<uint32_t>(p[0])
    | (static_cast<uint32_t>(p[1]) << 8)
    | (static_cast<uint32_t>(p[2]) << 16)
    | (static_cast<uint32_t>(p[3]) << 24);
  }

  std::vector<uint8_t> compressRLE(const std::vector<uint8_t>& input) {
    const size_t n = input.size();

    std::vector<uint8_t> out;
    out.reserve(4 + 8 + (n / 2) + 16);

    out.insert(out.end(), kRleMagic, kRleMagic + 4);
    writeU64LE(out, static_cast<uint64_t>(n));

    if (n == 0) return out;

    size_t i = 0;
    while (i < n) {
      const uint8_t v = input[i];
      size_t run = 1;
      while (i + run < n && input[i + run] == v && run < 255) ++run;

      out.push_back(static_cast<uint8_t>(run));
      out.push_back(v);

      i += run;
    }

    return out;
  }

  std::vector<uint8_t> decompressRLE(const std::vector<uint8_t>& input) {
    if (input.size() < 12) {
      throw CompressionError("Compression::decompressRLE: input too small");
    }
    if (!(input[0] == kRleMagic[0] && input[1] == kRleMagic[1] &&
          input[2] == kRleMagic[2] && input[3] == kRleMagic[3])) {
      throw CompressionError("Compression::decompressRLE: bad magic");
    }

    const uint64_t origSize64 = readU64LE(input.data() + 4);
    if (origSize64 > static_cast<uint64_t>(std::numeric_limits<size_t>::max())) {
      throw CompressionError("Compression::decompressRLE: original size too large");
    }
    const size_t origSize = static_cast<size_t>(origSize64);

    std::vector<uint8_t> out;
    out.reserve(origSize);

    const uint8_t* ip = input.data() + 12;
    const uint8_t* ipEnd = input.data() + input.size();

    while (ip < ipEnd) {
      if (ipEnd - ip < 2) {
        throw CompressionError("Compression::decompressRLE: truncated input");
      }
      const uint8_t count = *ip++;
      const uint8_t value = *ip++;

      if (count == 0) {
        throw CompressionError("Compression::decompressRLE: invalid run length");
      }

      if (out.size() + count > origSize) {
        throw CompressionError("Compression::decompressRLE: output overflow");
      }
      out.insert(out.end(), count, value);
    }

    if (out.size() != origSize) {
      throw CompressionError("Compression::decompressRLE: output size mismatch");
    }
    return out;
  }

  static constexpr uint8_t kMagic[4] = {'L','Z','4','F'};
  static constexpr int kMinMatch = 4;
  static constexpr int kHashBits = 16;
  static constexpr int kHashSize = 1 << kHashBits;
  static constexpr int kMaxOffset = 65535;

  static inline uint32_t hash4(uint32_t x) {
    return (x * 2654435761u) >> (32 - kHashBits);
  }

  static inline void emitLength(std::vector<uint8_t>& out, int len) {
    while (len >= 255) {
      out.push_back(255);
      len -= 255;
    }
    out.push_back(static_cast<uint8_t>(len));
  }

  std::vector<uint8_t> compressLZ4(const std::vector<uint8_t>& input) {
    const size_t n = input.size();

    std::vector<uint8_t> out;
    out.reserve(n + n / 255 + 32);

    out.insert(out.end(), kMagic, kMagic + 4);
    writeU64LE(out, static_cast<uint64_t>(n));

    if (n == 0) {
      return out;
    }

    std::array<int32_t, kHashSize> table;
    table.fill(-1);

    const uint8_t* src = input.data();
    const uint8_t* srcEnd = src + n;

    const uint8_t* anchor = src;
    const uint8_t* ip = src;

    const uint8_t* matchLimit = (n >= 4) ? (srcEnd - 4) : src;

    auto storePosition = [&](const uint8_t* p) {
      if (p <= matchLimit) {
        uint32_t v = readU32LE(p);
        table[hash4(v)] = static_cast<int32_t>(p - src);
      }
    };

    storePosition(ip);

    while (ip <= matchLimit) {
      uint32_t v = readU32LE(ip);
      uint32_t h = hash4(v);
      int32_t refPos = table[h];
      table[h] = static_cast<int32_t>(ip - src);

      const uint8_t* ref = (refPos >= 0) ? (src + refPos) : nullptr;

      bool matchFound = false;
      uint16_t offset = 0;

      if (ref && ref < ip) {
        size_t dist = static_cast<size_t>(ip - ref);
        if (dist <= static_cast<size_t>(kMaxOffset)) {
          if (std::memcmp(ref, ip, 4) == 0) {
            matchFound = true;
            offset = static_cast<uint16_t>(dist);
          }
        }
      }

      if (!matchFound) {
        ++ip;
        continue;
      }

      const uint8_t* matchStart = ip;
      const uint8_t* refStart = ip - offset;

      const uint8_t* matchEnd = ip + 4;
      const uint8_t* refIt = refStart + 4;

      while (matchEnd < srcEnd && *matchEnd == *refIt) {
        ++matchEnd;
        ++refIt;
      }

      const int literalLen = static_cast<int>(matchStart - anchor);
      const int matchLen = static_cast<int>(matchEnd - matchStart);

      size_t tokenIndex = out.size();
      out.push_back(0);

      uint8_t token = 0;

      if (literalLen < 15) token |= static_cast<uint8_t>(literalLen << 4);
      else token |= static_cast<uint8_t>(15 << 4);

      int ml = matchLen - kMinMatch;
      if (ml < 15) token |= static_cast<uint8_t>(ml);
      else token |= 15;

      if (literalLen >= 15) {
        emitLength(out, literalLen - 15);
      }

      if (literalLen > 0) {
        out.insert(out.end(), anchor, matchStart);
      }

      writeU16LE(out, offset);

      if (ml >= 15) {
        emitLength(out, ml - 15);
      }

      out[tokenIndex] = token;

      ip = matchEnd;
      anchor = ip;

      const uint8_t* update = ip - 3;
      if (update < src) update = src;
      while (update <= matchLimit && update < ip) {
        storePosition(update);
        ++update;
      }
    }

    if (anchor < srcEnd) {
      const int literalLen = static_cast<int>(srcEnd - anchor);
      size_t tokenIndex = out.size();
      out.push_back(0);

      uint8_t token = 0;
      if (literalLen < 15) token |= static_cast<uint8_t>(literalLen << 4);
      else token |= static_cast<uint8_t>(15 << 4);

      if (literalLen >= 15) {
        emitLength(out, literalLen - 15);
      }

      out.insert(out.end(), anchor, srcEnd);
      out[tokenIndex] = token;
    }

    return out;
  }

  std::vector<uint8_t> decompressLZ4(const std::vector<uint8_t>& input) {
    if (input.size() < 12) {
      throw CompressionError("Compression::decompressLZ4: input too small");
    }
    if (!(input[0] == kMagic[0] && input[1] == kMagic[1] &&
          input[2] == kMagic[2] && input[3] == kMagic[3])) {
      throw CompressionError("Compression::decompressLZ4: bad magic");
    }

    const uint64_t origSize64 = readU64LE(input.data() + 4);
    if (origSize64 > static_cast<uint64_t>(std::numeric_limits<size_t>::max())) {
      throw CompressionError("Compression::decompressLZ4: original size too large");
    }
    const size_t origSize = static_cast<size_t>(origSize64);

    std::vector<uint8_t> out;
    out.reserve(origSize);

    const uint8_t* ip = input.data() + 12;
    const uint8_t* ipEnd = input.data() + input.size();

    auto require = [&](size_t bytes) {
      if (static_cast<size_t>(ipEnd - ip) < bytes) {
        throw CompressionError("Compression::decompressLZ4: truncated input");
      }
    };

    while (ip < ipEnd) {
      require(1);
      uint8_t token = *ip++;

      int literalLen = token >> 4;
      if (literalLen == 15) {
        while (true) {
          require(1);
          uint8_t s = *ip++;
          literalLen += s;
          if (s != 255) break;
        }
      }

      require(static_cast<size_t>(literalLen));
      if (literalLen > 0) {
        out.insert(out.end(), ip, ip + literalLen);
        ip += literalLen;
      }

      if (ip >= ipEnd) {
        break;
      }

      require(2);
      uint16_t offset = readU16LE(ip);
      ip += 2;

      if (offset == 0 || offset > out.size()) {
        throw CompressionError("Compression::decompressLZ4: invalid offset");
      }

      int matchLen = (token & 0x0F);
      if (matchLen == 15) {
        while (true) {
          require(1);
          uint8_t s = *ip++;
          matchLen += s;
          if (s != 255) break;
        }
      }
      matchLen += kMinMatch;

      const size_t start = out.size() - offset;
      const size_t oldSize = out.size();
      out.resize(oldSize + static_cast<size_t>(matchLen));

      uint8_t* dst = out.data() + oldSize;
      const uint8_t* src = out.data() + start;

      for (int i = 0; i < matchLen; ++i) {
        dst[i] = src[i];
      }
    }

    if (out.size() != origSize) {
      throw CompressionError("Compression::decompressLZ4: output size mismatch");
    }

    return out;
  }

} // namespace Compression

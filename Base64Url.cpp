#include "Base64Url.h"

#include <stdexcept>

int Base64Url::value(char c) {
  if (c >= 'A' && c <= 'Z') return int(c - 'A');
  if (c >= 'a' && c <= 'z') return int(c - 'a') + 26;
  if (c >= '0' && c <= '9') return int(c - '0') + 52;
  if (c == '-') return 62;
  if (c == '_') return 63;
  return -1;
}

std::string Base64Url::encodeNoPad(const std::vector<uint8_t>& data) {
  static const char* kAlphabet =
  "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789-_";

  std::string out;
  out.reserve(((data.size() + 2) / 3) * 4);

  size_t i = 0;
  while (i + 3 <= data.size()) {
    uint32_t v = (uint32_t(data[i]) << 16) |
    (uint32_t(data[i + 1]) << 8) |
    (uint32_t(data[i + 2]) << 0);

    out.push_back(kAlphabet[(v >> 18) & 63]);
    out.push_back(kAlphabet[(v >> 12) & 63]);
    out.push_back(kAlphabet[(v >>  6) & 63]);
    out.push_back(kAlphabet[(v >>  0) & 63]);
    i += 3;
  }

  const size_t rem = data.size() - i;
  if (rem == 1) {
    uint32_t v = (uint32_t(data[i]) << 16);
    out.push_back(kAlphabet[(v >> 18) & 63]);
    out.push_back(kAlphabet[(v >> 12) & 63]);
  } else if (rem == 2) {
    uint32_t v = (uint32_t(data[i]) << 16) |
    (uint32_t(data[i + 1]) << 8);
    out.push_back(kAlphabet[(v >> 18) & 63]);
    out.push_back(kAlphabet[(v >> 12) & 63]);
    out.push_back(kAlphabet[(v >>  6) & 63]);
  }

  return out;
}

std::vector<uint8_t> Base64Url::decodeNoPad(const std::string& s) {
  std::vector<uint8_t> out;
  out.reserve((s.size() * 3) / 4 + 3);

  uint32_t buf = 0;
  int bits = 0;

  for (char c : s) {
    const int v = value(c);
    if (v < 0) {
      throw std::runtime_error("Base64Url::decodeNoPad: invalid character");
    }

    buf = (buf << 6) | uint32_t(v);
    bits += 6;

    if (bits >= 8) {
      bits -= 8;
      out.push_back(uint8_t((buf >> bits) & 0xFF));
    }
  }

  if (bits > 0) {
    const uint32_t mask = (1u << bits) - 1u;
    if ((buf & mask) != 0u) {
      throw std::runtime_error("Base64Url::decodeNoPad: invalid tail bits");
    }
  }

  return out;
}

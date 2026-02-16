#pragma once

#include <cstdint>
#include <stdexcept>
#include <vector>

namespace Compression {

  class CompressionError final : public std::runtime_error {
  public:
    explicit CompressionError(const char* message);
    explicit CompressionError(const std::string& message);
  };

  std::vector<uint8_t> compressLZ4(const std::vector<uint8_t>& input);
  std::vector<uint8_t> decompressLZ4(const std::vector<uint8_t>& input);

  std::vector<uint8_t> compressRLE(const std::vector<uint8_t>& input);
  std::vector<uint8_t> decompressRLE(const std::vector<uint8_t>& input);

} // namespace Compression

#pragma once

#include <cstdint>
#include <string>
#include <vector>

class Base64Url {
public:
  static std::string encodeNoPad(const std::vector<uint8_t>& data);
  static std::vector<uint8_t> decodeNoPad(const std::string& s);

private:
  static int value(char c);
};

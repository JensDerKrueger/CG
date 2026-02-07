#pragma once

#include "Vec2.h"
#include "Image.h"

namespace ImageLoader {
  class Exception : public std::exception {
    public:
    Exception(const std::string& whatStr) : whatStr(whatStr) {}
      virtual const char* what() const throw() {
        return whatStr.c_str();
      }
    private:
      std::string whatStr;
  };

  Image load(const std::string& filename, bool flipY=true);
}

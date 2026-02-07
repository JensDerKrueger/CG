#pragma once

#include <algorithm>
#include <cstdint>
#include <cmath>
#include <ostream>
#include <string>
#include <sstream>
#include <array>

#include "Vec2.h"
#include "Vec3.h"

template <typename T>
union Vec4t {
public:
  std::array<T, 4> e;
  struct { T x; T y; T z; T w;};
  struct { T r; T g; T b; T a;};
  Vec2t<T> xy;
  Vec3t<T> xyz;

  Vec4t(T v=T(0)) :
  e{v,v,v,v}
  {}

  Vec4t(T x, T y, T z, T w):
  e{x,y,z,w}
  {}

  Vec4t(const Vec4t& other) :
  e{other.x, other.y, other.z, other.w}
  {}

  Vec4t(const Vec3t<T>& other, T w):
  e{other.x, other.y, other.z, w}
  {}

  Vec4t(const Vec2t<T>& other, T z, T w) :
  e{other.x, other.y, z, w}
  {}

  template <typename U>
  explicit Vec4t(const Vec4t<U>& other) :
  e{other.x, other.y, other.z, other.w}
  {}

  template <typename U>
  explicit Vec4t(const Vec3t<U>& other, T w):
  e{other.x, other.y, other.z, w}
  {}

  template <typename U>
  explicit Vec4t(const Vec2t<U>& other, T z, T w) :
  e{other.x, other.y, z, w}
  {}

  std::string toString() const {
    std::stringstream s;
    s << "[" << e[0] << ", " << e[1] << ", " << e[2] << ", " << e[3] << "]";
    return s.str();
  }

  Vec4t operator+(const Vec4t& val) const {
    return {e[0]+val.e[0],
      e[1]+val.e[1],
      e[2]+val.e[2],
      e[3]+val.e[3]};
  }

  Vec4t operator-(const Vec4t& val) const {
    return {e[0]-val.e[0],
      e[1]-val.e[1],
      e[2]-val.e[2],
      e[3]-val.e[3]};
  }

  Vec4t operator*(const Vec4t& val) const {
    return {e[0]*val.e[0],
      e[1]*val.e[1],
      e[2]*val.e[2],
      e[3]*val.e[3]};
  }

  Vec4t operator/(const Vec4t& val) const {
    return {e[0]/val.e[0],
      e[1]/val.e[1],
      e[2]/val.e[2],
      e[3]/val.e[3]};
  }

  Vec4t operator*(const T& val) const {
    return {e[0]*val, e[1]*val, e[2]*val, e[3]*val};
  }

  Vec4t operator/(const T& val) const {
    return {e[0]/val, e[1]/val, e[2]/val, e[3]/val};
  }

  Vec4t operator+(const T& val) const {
    return {e[0]+val,e[1]+val,e[2]+val,e[3]+val};
  }

  Vec4t operator-(const T& val) const{
    return {e[0]-val,e[1]-val,e[2]-val,e[3]-val};
  }

  bool operator == ( const Vec4t& other ) const {
    return e[0] == other.e[0] &&
    e[1] == other.e[1] &&
    e[2] == other.e[2] &&
    e[3] == other.e[3];
  }

  bool operator != ( const Vec4t& other ) const {
    return e[0] != other.e[0] ||
    e[1] != other.e[1] ||
    e[2] != other.e[2] ||
    e[3] != other.e[3];
  }

  T length() const {
    return std::sqrt(sqlength());
  }

  T sqlength() const {
    return e[0]*e[0]+e[1]*e[1]+e[2]*e[2]+e[3]*e[3];
  }

  Vec3t<T> vec3() const {
    return {e[0], e[1], e[2]};
  }

  operator T*(void) {return e.data();}
  operator const T*(void) const  {return e.data();}

  static T dot(const Vec4t& a, const Vec4t& b) {
    return a.e[0]*b.e[0]+a.e[1]*b.e[1]+a.e[2]*b.e[2]+a.e[3]*b.e[3];
  }

  static Vec4t normalize(const Vec4t& a) {
    const T l = a.length();
    return (l != T(0)) ? a / l : Vec4t{T(0),T(0),T(0),T(0)};
  }

  static Vec4t<float> random() {
    return {staticRand.rand01(),staticRand.rand01(),staticRand.rand01(),staticRand.rand01()};
  }

  static Vec4t<float> clamp(const Vec4t& val, float minVal, float maxVal) {
    return { std::clamp(val.x, minVal, maxVal),
      std::clamp(val.y, minVal, maxVal),
      std::clamp(val.z, minVal, maxVal),
      std::clamp(val.w, minVal, maxVal)
    };
  }
};

template <typename T>
std::ostream & operator<<(std::ostream & os, const Vec4t<T> & v) {
  os << v.toString();
  return os;
}

template <typename T>
Vec4t<T> operator*(float scalar, const Vec4t<T>& vec) {
  return vec * scalar;
}

typedef Vec4t<float> Vec4;
typedef Vec4t<int32_t> Vec4i;
typedef Vec4t<uint32_t> Vec4ui;

/*
 Copyright (c) 2026 Computer Graphics and Visualization Group, University of
 Duisburg-Essen

 Permission is hereby granted, free of charge, to any person obtaining a copy of
 this software and associated documentation files (the "Software"), to deal in the
 Software without restriction, including without limitation the rights to use, copy,
 modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and
 to permit persons to whom the Software is furnished to do so, subject to the following
 conditions:

 The above copyright notice and this permission notice shall be included in all copies
 or substantial portions of the Software.

 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,
 INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A
 PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
 HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF
 CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR
 THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

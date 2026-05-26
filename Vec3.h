#pragma once

#include <algorithm>
#include <cstdint>
#include <ostream>
#include <sstream>
#include <string>
#include <array>
#include <cmath>
#include <optional>
#include <iomanip>


#include "Rand.h"
#include "Vec2.h"

template <typename T>
union Vec3t {
public:
  std::array<T, 3> e;
  struct { T x; T y; T z; };
  struct { T r; T g; T b; };
  Vec2t<T> xy;

  Vec3t(T v=T(0)) :
  e{v,v,v}
  {}

  Vec3t(T x, T y, T z) :
  e{x,y,z}
  {}

  Vec3t(const Vec3t& other) :
  e{other.x, other.y, other.z}
  {}

  Vec3t(const Vec2t<T>& other, T z) :
  e{other.x, other.y, z}
  {}

  template <typename U>
  explicit Vec3t(const Vec3t<U>& other) :
  e{T(other.x), T(other.y), T(other.z)}
  {}

  template <typename U>
  explicit Vec3t(const Vec2t<U>& other, T z) :
  e{T(other.x), T(other.y), z}
  {}

  static Vec3t fromEncoding(const std::string& str) {
    Vec3t m;

    std::string cleaned;
    cleaned.reserve(str.size());
    for (char c : str) {
      if (c == '[' || c == ']') { continue; }
      if (std::isspace(static_cast<unsigned char>(c))) { continue; }
      cleaned.push_back(c);
    }

    std::array<T, 3> values{};
    std::stringstream ss(cleaned);
    ss.imbue(std::locale::classic());
    std::string token;
    size_t i = 0;

    while (std::getline(ss, token, '_')) {
      if (token.empty()) { return Vec3t(); }

      std::stringstream ts(token);
      ts.imbue(std::locale::classic());
      T v{};
      ts >> v;
      if (ts.fail()) { return Vec3t(); }

      char extra{};
      if (ts >> extra) { return Vec3t(); }

      if (i >= values.size()) { return Vec3t(); }
      values[i++] = v;
    }

    if (i != values.size()) { return Vec3t(); }

    m.e = values;
    return m;
  }

  std::string toEncoding() const {
    std::ostringstream s;
    s.imbue(std::locale::classic());
    s << std::setprecision(std::numeric_limits<T>::max_digits10);
    for (size_t i = 0; i < e.size(); ++i) {
      if (i != 0) { s << "_"; }
      s << e[i];
    }
    return s.str();
  }


  Vec3t operator+(const Vec3t& val) const {
    return Vec3t{e[0]+val.e[0],
      e[1]+val.e[1],
      e[2]+val.e[2]};
  }

  Vec3t operator-(const Vec3t& val) const {
    return Vec3t{e[0]-val.e[0],
      e[1]-val.e[1],
      e[2]-val.e[2]};
  }

  Vec3t operator*(const Vec3t& val) const{
    return Vec3t{e[0]*val.e[0],
      e[1]*val.e[1],
      e[2]*val.e[2]};
  }

  Vec3t operator/(const Vec3t& val) const{
    return Vec3t{e[0]/val.e[0],
      e[1]/val.e[1],
      e[2]/val.e[2]};
  }

  Vec3t operator*(const T& val) const {
    return {e[0]*val,e[1]*val,e[2]*val};
  }

  Vec3t operator/(const T& val) const{
    return {e[0]/val,e[1]/val,e[2]/val};
  }

  Vec3t operator+(const T& val) const {
    return {e[0]+val,e[1]+val,e[2]+val};
  }

  Vec3t operator-(const T& val) const{
    return {e[0]-val,e[1]-val,e[2]-val};
  }

  bool operator == (const Vec3t& other) const {
    return e[0] == other.e[0] &&
    e[1] == other.e[1] &&
    e[2] == other.e[2];
  }

  bool operator != (const Vec3t& other) const {
    return e[0] != other.e[0] ||
    e[1] != other.e[1] ||
    e[2] != other.e[2];
  }

  T length() const {
    return std::sqrt(sqlength());
  }

  T sqlength() const {
    return e[0]*e[0]+e[1]*e[1]+e[2]*e[2];
  }

  operator T*(void) {return e.data();}
  operator const T*(void) const  {return e.data();}

  static T dot(const Vec3t& a, const Vec3t& b) {
    return a.e[0]*b.e[0]+a.e[1]*b.e[1]+a.e[2]*b.e[2];
  }

  static Vec3t cross(const Vec3t& a, const Vec3t& b) {
    return Vec3t{a.e[1] * b.e[2] - a.e[2] * b.e[1],
      a.e[2] * b.e[0] - a.e[0] * b.e[2],
      a.e[0] * b.e[1] - a.e[1] * b.e[0]};
  }

  static Vec3t normalize(const Vec3t& a) {
    const T l = a.length();
    return (l != T(0)) ? a / l : Vec3t{T(0),T(0),T(0)};
  }

  static Vec3t reflect(const Vec3t& a, const Vec3t& n) {
    return a-n*dot(a,n)*T(2);
  }

  static std::optional<Vec3t> refract(const Vec3t& a, const Vec3t& normal, const T IOR) {
    const T cosI = Vec3t::dot(a, normal);
    int sign = (cosI < 0) ? -1 : 1;

    // we assume that if we look from the back side we are exiting the material
    // back side means that the sign/cosI is positive because the incoming ray
    // is assumed to point
    // towards the surface and the normal away
    const T n = (sign == 1) ? IOR : T(1) / IOR;
    const T sinThetaSq = n * n * (T(1) - cosI * cosI);

    if (sinThetaSq > T(1)) {
      // Total internal reflection
      return {};
    } else {
      const Vec3t d = a * n;
      const T c = n * cosI -sign * std::sqrt(T(1) - sinThetaSq);
      const Vec3t b = normal * c;
      return d - b;
    }
  }

  static Vec3t minV(const Vec3t& a, const Vec3t& b) {
    return {std::min(a.x,b.x), std::min(a.y,b.y), std::min(a.z,b.z)};
  }

  static Vec3t maxV(const Vec3t& a, const Vec3t& b) {
    return {std::max(a.x,b.x), std::max(a.y,b.y), std::max(a.z,b.z)};
  }

  static Vec3t<float> random() {
    return {staticRand.rand01(),staticRand.rand01(),staticRand.rand01()};
  }

  static Vec3t<float> randomPointInSphere() {
    while (true) {
      Vec3t<float> p{staticRand.rand11(),staticRand.rand11(),staticRand.rand11()};
      if (p.sqlength() > 1) continue;
      return p;
    }
  }

  static Vec3t<float> randomPointInHemisphere() {
    while (true) {
      Vec3t<float> p{staticRand.rand01(),staticRand.rand01(),staticRand.rand01()};
      if (p.sqlength() > 1) continue;
      return p;
    }
  }

  static Vec3t<float> randomPointInDisc() {
    while (true) {
      Vec3t<float> p{staticRand.rand11(),staticRand.rand11(),0};
      if (p.sqlength() > 1) continue;
      return p;
    }
  }

  static Vec3t<float> randomUnitVector() {
    const float a = staticRand.rand0Pi();
    const float z = staticRand.rand11();
    const float r = std::sqrt(1.0f - z*z);
    return {r*cosf(a), r*sinf(a), z};
  }

  static Vec3t<float> clamp(const Vec3t& val, float minVal, float maxVal) {
    return { std::clamp(val.x, minVal, maxVal),
      std::clamp(val.y, minVal, maxVal),
      std::clamp(val.z, minVal, maxVal)
    };
  }
};

template <typename T>
std::ostream & operator<<(std::ostream & os, const Vec3t<T> & v) {
  os << "[" << v[0] << ", " << v[1] << ", " << v[2] << "]";
  return os;
}

template <typename T>
Vec3t<T> operator*(float scalar, const Vec3t<T>& vec) {
  return vec * scalar;
}

typedef Vec3t<float> Vec3;
typedef Vec3t<int32_t> Vec3i;
typedef Vec3t<uint32_t> Vec3ui;

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

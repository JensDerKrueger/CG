#pragma once

#include <algorithm>
#include <cstdint>
#include <ostream>
#include <string>
#include <array>
#include <sstream>
#include <cmath>

#include "Rand.h"

template <typename T>
union Vec2t {
public:
  std::array<T, 2> e;
  struct { T x; T y; };
  struct { T r; T g; };

  Vec2t(T v=T(0)) :
  e{v,v}
  {}

  Vec2t(T x, T y) :
  e{x,y}
  {}

  Vec2t(const Vec2t& other) :
  e{other.x, other.y}
  {}

  template <typename U>
  explicit Vec2t(const Vec2t<U>& other) :
  e{T(other.x), T(other.y)}
  {}

  std::string toString() const {
    std::stringstream s;
    s << "[" << e[0] << ", " << e[1] << "]";
    return s.str();
  }

  Vec2t operator+(const Vec2t& val) const{
    return {e[0]+val.e[0],e[1]+val.e[1]};
  }

  Vec2t operator-(const Vec2t& val) const {
    return {e[0]-val.e[0],e[1]-val.e[1]};
  }

  Vec2t operator*(const Vec2t& val) const {
    return {e[0]*val.e[0],e[1]*val.e[1]};
  }

  Vec2t operator/(const Vec2t& val) const {
    return {e[0]/val.e[0],e[1]/val.e[1]};
  }

  Vec2t operator*(const T& val) const {
    return {e[0]*val,e[1]*val};
  }

  Vec2t operator/(const T& val) const {
    return {e[0]/val, e[1]/val};
  }

  Vec2t operator+(const T& val) const {
    return {e[0]+val,e[1]+val};
  }

  Vec2t operator-(const T& val) const{
    return {e[0]-val,e[1]-val};
  }

  bool operator == ( const Vec2t& other ) const {
    return e == other.e;
  }

  bool operator != ( const Vec2t& other ) const {
    return e != other.e;
  }

  T length() const {
    return std::sqrt(sqlength());
  }

  T sqlength() const {
    return e[0]*e[0]+e[1]*e[1];
  }

  operator T*(void) {return e.data();}
  operator const T*(void) const  {return e.data();}

  static Vec2t random() {
    return Vec2t{T{staticRand.rand01()},T{staticRand.rand01()}};
  }

  static Vec2t normalize(const Vec2t& a) {
    const T l = a.length();
    return (l != T(0)) ? a / l : Vec2t{T(0),T(0)};
  }

  static Vec2t<float> clamp(const Vec2t& val, float minVal, float maxVal) {
    return { std::clamp(val.x, minVal, maxVal),
      std::clamp(val.y, minVal, maxVal)
    };
  }
};

template <typename T>
std::ostream & operator<<(std::ostream & os, const Vec2t<T> & v) {
  os << v.toString();
  return os;
}

template <typename T>
Vec2t<T> operator*(float scalar, const Vec2t<T>& vec) {
  return vec * scalar;
}

typedef Vec2t<float> Vec2;
typedef Vec2t<int32_t> Vec2i;
typedef Vec2t<uint32_t> Vec2ui;

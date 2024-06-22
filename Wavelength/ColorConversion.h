#pragma once

#include "Vec3.h"
#include "Vec4.h"
#include "Mat4.h"

namespace ColorConversion {
  template <typename T> Vec3t<T> rgbToHsv(const Vec3t<T>& other) {
    const T minComp = std::min(other.r, std::min(other.g,other.b));
    const T maxComp = std::max(other.r, std::max(other.g,other.b));
    const T delta = maxComp - minComp;

    T h = T(0);
    if (maxComp != minComp) {
      if (maxComp == other.r)
        h = fmod((T(60) * ((other.g - other.b) / delta) + T(360)), T(360));
      else if (maxComp == other.g)
        h = fmod((T(60) * ((other.b - other.r) / delta) + T(120)), T(360));
      else if (maxComp == other.b)
        h = fmod((T(60) * ((other.r - other.g) / delta) + T(240)), T(360));
    }

    const T s = (maxComp == T(0)) ? T(0) : (delta / maxComp);
    const T v = maxComp;

    return {h,s,v};
  }

  template <typename T> Vec3t<T> hsvToRgb(const Vec3t<T>& other) {
    const T h = T(int32_t(other.x) % 360) / T(60);
    const T s = std::max(T(0), std::min(T(1), other.y));
    const T v = std::max(T(0), std::min(T(1), other.z));

    if (s == 0) return {v,v,v}; // Achromatic (grey)

    const int32_t i = int32_t(floor(h));
    const T f = h - T(i);
    const T p = v * (T(1) - s);
    const T q = v * (T(1) - s * f);
    const T t = v * (T(1) - s * (T(1) - f));

    switch(i) {
      case 0: return {v,t,p};
      case 1: return {q,v,p};
      case 2: return {p,v,t};
      case 3: return {p,q,v};
      case 4: return {t,p,v};
      default: return {v,p,q};
    }
  }

  template <typename T> Vec3t<T> hslToHsv(const Vec3t<T>& other) {
    const T h = other.x;
    const T s = other.y;
    const T l = other.z;
    
    const T v = s*std::min(l,T(1)-l)+l;
    return {h,(v > T(0)) ?  T(2)-T(2)*l/v : T(0), v};
  }
  template <typename T> Vec3t<T> hsvToHsl(const Vec3t<T>& other) {
    const T h = other.x;
    const T s = other.y;
    const T v = other.z;
    
    T l = v-v*s/T(2);
    T m = std::min(l,T(1)-l);
    return {h,(m > T(0)) ? (v-l)/m : l, l};
  }

    template <typename T> Vec3t<T> rgbToCmy(const Vec3t<T>& other) {
      return {T(1)-other.x,T(1)-other.y,T(1)-other.z};
    }

  template <typename T> Vec4t<T> rgbToCmyk(const Vec3t<T>& other) {
    const Vec3t<T> cmy = rgbToCmy(other);
    const T minVal = std::min(cmy.x,std::min(cmy.y,cmy.z));
    return {cmy-minVal,minVal};
  }

    template <typename T> Vec3t<T> cmyToRgb(const Vec3t<T>& other){
      return {T(1)-other.x,T(1)-other.y,T(1)-other.z};
    }

  template <typename T> Vec3t<T> cmykToRgb(const Vec4& other) {
    return {T(1)-(other.x+other.w),T(1)-(other.y+other.w),T(1)-(other.z+other.w)};
  }
      
  template <typename T> Vec3t<T> rgbToYuv(const Vec3t<T>& other) {
    const Mat4t<T> c{ T(0.299),  T(0.587),  T(0.114),  T(0),
                      T(-0.147), T(-0.289), T(0.436),  T(0),
                      T(0.615),  T(-0.515), T(-0.100), T(0),
                      T(0),      T(0),      T(0),      T(1)
    };
    return (c * Vec4t<T>(other,1)).xyz;
  }

  template <typename T> Vec3t<T> yuvToRgb(const Vec3t<T>& other) {
    const Mat4t<T> c{ T(1), T(0),       T(1.140),  T(0),
                      T(1), T(-0.395),  T(-0.581), T(0),
                      T(1), T(2.032),   T(0),      T(0),
                      T(0), T(0),       T(0),      T(1)
    };
    return (c * Vec4t<T>(other,1)).xyz;
  }

  template <typename T> T srgbXYZ2RGBPostprocess(T c) {
    // clip if c is out of range
    c = c > 1 ? 1 : (c < 0 ? 0 : c);

    // apply the color component transfer function
    c = c <= 0.0031308 ? c * 12.92 : 1.055 * pow(c, 1. / 2.4) - 0.055;

    return c;
  }

  template <typename T> Vec3t<T> srgbXYZ2RGB(const Vec3t<T>& xyz) {
    const T x = xyz[0];
    const T y = xyz[1];
    const T z = xyz[2];

    const T rl =  3.2406255 * x + -1.537208  * y + -0.4986286 * z;
    const T gl = -0.9689307 * x +  1.8757561 * y +  0.0415175 * z;
    const T bl =  0.0557101 * x + -0.2040211 * y +  1.0569959 * z;

    return Vec3t<T>{srgbXYZ2RGBPostprocess(rl),
                    srgbXYZ2RGBPostprocess(gl),
                    srgbXYZ2RGBPostprocess(bl)};
  }

  template <typename T> Vec3t<T> cie1931WavelengthToXYZFit(T wavelength) {
    T x;
    {
      const T t1 = (wavelength - 442.0) * ((wavelength < 442.0) ? 0.0624 : 0.0374);
      const T t2 = (wavelength - 599.8) * ((wavelength < 599.8) ? 0.0264 : 0.0323);
      const T t3 = (wavelength - 501.1) * ((wavelength < 501.1) ? 0.0490 : 0.0382);

      x =   0.362 * exp(-0.5 * t1 * t1) + 1.056 * exp(-0.5 * t2 * t2) - 0.065 * exp(-0.5 * t3 * t3);
    }

    T y;
    {
      const T t1 = (wavelength - 568.8) * ((wavelength < 568.8) ? 0.0213 : 0.0247);
      const T t2 = (wavelength - 530.9) * ((wavelength < 530.9) ? 0.0613 : 0.0322);

      y =   0.821 * exp(-0.5 * t1 * t1) + 0.286 * exp(-0.5 * t2 * t2);
    }

    T z;
    {
      const T t1 = (wavelength - 437.0) * ((wavelength < 437.0) ? 0.0845 : 0.0278);
      const T t2 = (wavelength - 459.0) * ((wavelength < 459.0) ? 0.0385 : 0.0725);

      z =   1.217 * exp(-0.5 * t1 * t1) + 0.681 * exp(-0.5 * t2 * t2);
    }

    return Vec3t<T>{ x, y, z };
  }


  template <typename T> Vec3t<T> wavelengthToRGB(T wavelength){
    const Vec3t<T> xyz = cie1931WavelengthToXYZFit(wavelength);
    return srgbXYZ2RGB(xyz);
  }

}

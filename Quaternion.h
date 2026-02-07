#pragma once

#include "Vec3.h"
#include "Mat4.h"

class Quaternion {
public:
  Quaternion():
    x(0),
    y(0),
    z(0),
    w(0)
  {}

  Quaternion(const Vec3 v, float w):
    x(v.x),
    y(v.y),
    z(v.z),
    w(w)
  {}

  Quaternion(float x, float y, float z, float w):
    x(x),
    y(y),
    z(z),
    w(w)
  {}
  
  Quaternion(const Quaternion& other) :
    x(other.x),
    y(other.y),
    z(other.z),
    w(other.w)
  {}

  Mat4 computeRotation() const {
    float n, s;
    float xs, ys, zs;
    float wx, wy, wz;
    float xx, xy, xz;
    float yy, yz, zz;
    
    n = (x * x) + (y * y) + (z * z) + (w * w);
    s = (n > 0.0f) ? (2.0f / n) : 0.0f;
    
    xs = x * s;
    ys = y * s;
    zs = z * s;
    wx = w * xs;
    wy = w * ys;
    wz = w * zs;
    xx = x * xs;
    xy = x * ys;
    xz = x * zs;
    yy = y * ys;
    yz = y * zs;
    zz = z * zs;
    
    return Mat4(1.0f - (yy + zz), xy - wz,          xz + wy,          0,
                xy + wz,          1.0f - (xx + zz), yz - wx,          0,
                xz - wy,          yz + wx,          1.0f - (xx + yy), 0,
                0,                0,                0,                1);
  }
  
private:
  float x;
  float y;
  float z;
  float w;
};

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

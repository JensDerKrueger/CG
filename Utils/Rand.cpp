#include "Rand.h"

#ifndef M_PI
constexpr float M_PI = 3.14159265358979323846f;
#endif

Random::Random() :
rd{},
gen{rd()},
dis01{0.0f, 1.0f},
dis11{-1.0f, 1.0f},
disPi{0.0f, 2.0f * float(M_PI)},
dis005{0.0f, 0.5f},
dis051{0.5f, 1.0f}
{
  
}

Random::Random(uint32_t seed)  :
rd{},
gen{seed},
dis01{0.0f, 1.0f},
dis11{-1.0f, 1.0f},
disPi{0.0f, 2.0f * float(M_PI)},
dis005{0.0f, 0.5f},
dis051{0.5f, 1.0f}
{
}



float Random::rand01() {
    return dis01(gen);
}

float Random::rand005() {
    return dis005(gen);
}

float Random::rand051() {
    return dis051(gen);
}

float Random::rand11() {
    return dis11(gen);
}

float Random::rand0Pi() {
    return disPi(gen);
}

Random staticRand;

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

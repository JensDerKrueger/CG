#pragma once

#include "Vec3.h"
#include "Mat4.h"

#include "GLProgram.h"
#include "GLBuffer.h"
#include "GLArray.h"
#include "GLTexture2D.h"

const Vec3 RANDOM_COLOR{-1.0f,-1.0f,-1.0f};
const Vec3 RAINBOW_COLOR{-2.0f,-2.0f,-2.0f};

class AbstractParticleSystem {
public:
    AbstractParticleSystem(float pointSize, float refDepth=1.0f);
	virtual void update(float t) = 0;
		
	void setPointSize(float pointSize, float refDepth=1.0f) {
        this->pointSize = pointSize;
        this->refDepth = refDepth;
    }
	float getPointSize() const {return pointSize;}
    float getRefDepth() const {return refDepth;}
	virtual void setColor(const Vec3& color) = 0;
	
	void render(const Mat4& v, const Mat4& p);
	
	virtual std::vector<float> getData() const = 0;
	virtual size_t getParticleCount() const = 0;

    static Vec3 computeColor(const Vec3& c);
    
private:
	float pointSize;
  float refDepth;
	
	GLProgram prog;
	GLint mvpLocation;
  GLint ppLocation;
	GLint texLocation;	
	GLTexture2D sprite;	
	
	GLArray particleArray;
	GLBuffer vbPosColor;

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

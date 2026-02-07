#include "GLApp.h"
#include <filesystem>
#include <chrono>
#include <ctime>
#include <iomanip>
#include <sstream>
#include <string>


#ifndef __EMSCRIPTEN__
GLApp* GLApp::staticAppPtr = nullptr;
#endif

GLApp::GLApp(uint32_t w, uint32_t h, uint32_t s,
             const std::string& title,
             bool fpsCounter, bool sync,
             bool exactPixels,
             const std::vector<std::string>& args) :
#ifdef __EMSCRIPTEN__
  glEnv{w,h,s,title,fpsCounter,sync,exactPixels,3,0,true,},
#else
  glEnv{w,h,s,title,fpsCounter,sync,exactPixels,4,1,true},
#endif
  p{},
  mv{},
  simpleProg{GLProgram::createFromString(
     "uniform mat4 MVP;\n"
     "in vec3 vPos;\n"
     "in vec4 vColor;\n"
     "out vec4 color;\n"
     "void main() {\n"
     "    gl_Position = MVP * vec4(vPos, 1.0);\n"
     "    color = vColor;\n"
     "}\n",
     "in vec4 color;\n"
     "out vec4 FragColor;\n"
     "void main() {\n"
     "    FragColor = color;\n"
     "}\n","",false,true)},
  simplePointProg{GLProgram::createFromString(
     "uniform mat4 MVP;\n"
     "in vec3 vPos;\n"
     "in vec4 vColor;\n"
     "out vec4 color;\n"
     "void main() {\n"
     "    gl_Position = MVP * vec4(vPos, 1.0);\n"
     "    color = vColor;\n"
     "}\n",
     "in vec4 color;\n"
     "out vec4 FragColor;\n"
     "void main() {\n"
     "    FragColor = color;\n"
     "}\n","",false,true)},
  simpleSpriteProg{GLProgram::createFromString(
     "uniform mat4 MVP;\n"
     "in vec3 vPos;\n"
     "in vec4 vColor;\n"
     "out vec4 color;\n"
     "void main() {\n"
     "    gl_Position = MVP * vec4(vPos, 1.0);\n"
     "    color = vColor;\n"
     "}\n",
     "uniform sampler2D pointSprite;\n"
     "in vec4 color;\n"
     "out vec4 FragColor;\n"
     "void main() {\n"
     "    FragColor = color*texture(pointSprite, gl_PointCoord);\n"
     "}\n","",false,true)},
  simpleHLSpriteProg{GLProgram::createFromString(
     "uniform mat4 MVP;\n"
     "in vec3 vPos;\n"
     "in vec4 vColor;\n"
     "out vec4 color;\n"
     "void main() {\n"
     "    gl_Position = MVP * vec4(vPos, 1.0);\n"
     "    color = vColor;\n"
     "}\n",
     "uniform sampler2D pointSprite;\n"
     "uniform sampler2D pointSpriteHighlight;\n"
     "in vec4 color;\n"
     "out vec4 FragColor;\n"
     "void main() {\n"
     "    FragColor = color*texture(pointSprite, gl_PointCoord)+texture(pointSpriteHighlight, gl_PointCoord);\n"
     "}\n","",false,true)},
  simpleTexProg{GLProgram::createFromString(
     "uniform mat4 MVP;\n"
     "in vec3 vPos;\n"
     "in vec2 vTexCoords;\n"
     "out vec4 color;\n"
     "out vec2 texCoords;\n"
     "void main() {\n"
     "    gl_Position = MVP * vec4(vPos, 1.0);\n"
     "    texCoords = vTexCoords;\n"
     "}\n",
     "uniform sampler2D raster;\n"
     "in vec2 texCoords;\n"
     "out vec4 FragColor;\n"
     "void main() {\n"
     "    FragColor = texture(raster, texCoords);\n"
     "}\n","",false,true)},
  simpleLightProg{GLProgram::createFromString(
     "uniform mat4 MVP;\n"
     "uniform mat4 MV;\n"
     "uniform mat4 MVit;\n"
     "in vec3 vPos;\n"
     "in vec4 vColor;\n"
     "in vec3 vNormal;\n"
     "out vec4 color;\n"
     "out vec3 normal;\n"
     "out vec3 pos;\n"
     "void main() {\n"
     "    gl_Position = MVP * vec4(vPos, 1.0);\n"
     "    pos = (MV * vec4(vPos, 1.0)).xyz;\n"
     "    color = vColor;\n"
     "    normal = (MVit * vec4(vNormal, 0.0)).xyz;\n"
     "}\n",
     "in vec4 color;\n"
     "in vec3 pos;\n"
     "in vec3 normal;\n"
     "out vec4 FragColor;\n"
     "void main() {\n"
     "    vec3 nnormal = normalize(normal);"
     "    vec3 nlightDir = normalize(vec3(0.0,0.0,0.0)-pos);"
     "    FragColor = color*abs(dot(nlightDir,nnormal));\n"
     "}\n","",false,true)},
  simpleArray{},
  simpleVb{GL_ARRAY_BUFFER},
  raster{GL_LINEAR, GL_LINEAR,GL_CLAMP_TO_EDGE,GL_CLAMP_TO_EDGE},
  pointSprite{GL_LINEAR, GL_LINEAR,GL_CLAMP_TO_EDGE,GL_CLAMP_TO_EDGE},
  pointSpriteHighlight{GL_LINEAR, GL_LINEAR,GL_CLAMP_TO_EDGE,GL_CLAMP_TO_EDGE},
  resumeTime{0},
  animationActive{true}
{
  setInteractionCallbacks();
  resetPointTexture();
  
  // setup a minimal shader and buffer
  shaderUpdate();

#ifdef __EMSCRIPTEN__
  startTime = emscripten_performance_now()/1000.0;
#else
  startTime = glfwGetTime();
#endif
  const Dimensions dim = glEnv.getFramebufferSize();
  GL(glViewport(0, 0, GLsizei(dim.width), GLsizei(dim.height)));

  initScript(args);
}

GLApp::~GLApp() {
}

void GLApp::setInteractionCallbacks() {
  if (interaction) {
#ifdef __EMSCRIPTEN__
    glEnv.setMouseCallbacks(cursorPositionCallback,
                            mouseButtonUpCallback,
                            mouseButtonDownCallback,
                            scrollCallback,
                            touchStartCallback,
                            touchEndCallback,
                            touchMoveCallback,
                            this);
    glEnv.setKeyCallback(keyCallback, this);
    glEnv.setResizeCallback(sizeCallback, this);
#else
    staticAppPtr = this;
    glEnv.setMouseCallbacks(cursorPositionCallback,
                            mouseButtonCallback,
                            scrollCallback);
    glEnv.setKeyCallbacks(keyCallback, keyCharCallback);
    glEnv.setResizeCallback(sizeCallback);
#endif
  } else {
#ifdef __EMSCRIPTEN__
    glEnv.setMouseCallbacks(nullptr, nullptr, nullptr, nullptr,
                            nullptr, nullptr, nullptr, this);
    glEnv.setKeyCallback(nullptr, this);
#else
    glEnv.setMouseCallbacks(nullptr, nullptr, nullptr);
    glEnv.setKeyCallbacks(nullptr, nullptr);
#endif
  }
}

void GLApp::setPointTexture(const Image& shape) {
  setPointTexture(shape.data, shape.width, shape.height, shape.componentCount);
}

void GLApp::setPointHighlightTexture(const Image& shape) {
  pointSpriteHighlight.setData(shape.data, shape.width, shape.height, shape.componentCount);
}

void GLApp::setPointTexture(const std::vector<uint8_t>& shape, uint32_t x, 
                            uint32_t y, uint8_t components) {
  pointSprite.setData(shape, x, y, components);
}

void GLApp::resetPointTexture(uint32_t resolution) {
  std::vector<uint8_t> disk(resolution*resolution*4);
  const Vec2 center{0.0f,0.0f};
  for (size_t y = 0;y<resolution;++y) {
    for (size_t x = 0;x<resolution;++x) {
      const Vec2 normPos{2.0f*x/float(resolution)-1.0f, 2.0f*y/float(resolution)-1.0f};
      const uint8_t dist = uint8_t(std::max<int16_t>(0, int16_t((1.0f-(center - normPos).length()) * 255))); 
      disk[4*(x+y*resolution)+0] = 255;
      disk[4*(x+y*resolution)+1] = 255;
      disk[4*(x+y*resolution)+2] = 255;
      disk[4*(x+y*resolution)+3] = dist;
    }
  }
  setPointTexture(disk, resolution, resolution, 4);
}

void GLApp::resetPointHighlightTexture() {
  Image i{0,0};
  setPointHighlightTexture(i);
}

void GLApp::processScript() {
  if (scriptRunning) {
    CommandResultCode result = interpreter.runBatch();

    if (result == CommandResultCode::success) {
    } else if (result == CommandResultCode::triggerLoop) {
    } else if (result == CommandResultCode::waitingNoop) {
    } else if (result == CommandResultCode::finished) {
      scriptRunning = false;
    } else {
      std::cerr << "Scripting Error: " << result << " in line " << interpreter.getLastInstructionIndex()+1 << "\n";
      scriptRunning = false;
    }
  }
}
void GLApp::mainLoop() {
#ifdef __EMSCRIPTEN__
  processScript();
  if (animationActive) {
    animate(emscripten_performance_now()/1000.0-startTime);
  }
  glEnv.beginOfFrame();
  GL(glClearColor(background.x,background.y,background.z,background.w));
  GL(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));
  draw();
  glEnv.endOfFrame();
#else
  do {
    processScript();
    if (animationActive) {
      animate(glfwGetTime()-startTime);
    }
    glEnv.beginOfFrame();
    GL(glClearColor(background.x,background.y,background.z,background.w));
    GL(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));
    draw();
    glEnv.endOfFrame();
  } while (!glEnv.shouldClose());
#endif
}

void GLApp::run() {
  init();
  reset();
  const Dimensions winDim = glEnv.getWindowSize();
  const Dimensions fbDim  = glEnv.getFramebufferSize();

  resize(winDim, fbDim); // GLsizei(dim.width), GLsizei(dim.height));

#ifdef __EMSCRIPTEN__
  emscripten_set_main_loop_arg(mainLoopWrapper, this, 0, 1);
  glEnv.setSync(glEnv.getSync());
#else
  mainLoop();
#endif
}
 
void GLApp::resize(const Dimensions winDim, const Dimensions fbDim) {
  GL(glViewport(0, 0, GLsizei(fbDim.width), GLsizei(fbDim.height)));
}


void GLApp::triangulate(const Vec3& p0,
                        const Vec3& p1, const Vec4& c1,
                        const Vec3& p2, const Vec4& c2,
                        const Vec3& p3,
                        float lineThickness,
                        std::vector<float>& trisData) {

  const Dimensions dim{ glEnv.getFramebufferSize() };
  const Vec3 scale{Vec3{2.0f/float(dim.width), 2.0f/float(dim.height), 1.0} * lineThickness};

  const Vec3 pDir = Vec3::normalize(p1-p0);
  const Vec3 cDir = Vec3::normalize(p2-p1);
  const Vec3 nDir = Vec3::normalize(p3-p2);
  
  const Vec3 viewDir = Vec3::normalize((mvi * Vec4{0,0,1,0}).xyz);
  
  const Vec3 pPerp = Vec3::cross(pDir, viewDir);
  const Vec3 cPerp = Vec3::cross(cDir, viewDir);
  const Vec3 nPerp = Vec3::cross(nDir, viewDir);
  
  Vec3 pSep = pPerp + cPerp;
  Vec3 nSep = nPerp + cPerp;
  
  pSep = (pSep / std::max(1.0f,Vec3::dot(pSep, cPerp))) * scale;
  nSep = (nSep / std::max(1.0f,Vec3::dot(nSep, cPerp))) * scale;

  trisData.push_back(p1[0]+pSep[0]); trisData.push_back(p1[1]+pSep[1]);trisData.push_back(p1[2]+pSep[2]);
  trisData.push_back(c1[0]);trisData.push_back(c1[1]); trisData.push_back(c1[2]); trisData.push_back(c1[3]);

  trisData.push_back(p2[0]+nSep[0]); trisData.push_back(p2[1]+nSep[1]);trisData.push_back(p2[2]+nSep[2]);
  trisData.push_back(c2[0]);trisData.push_back(c2[1]); trisData.push_back(c2[2]); trisData.push_back(c2[3]);

  trisData.push_back(p1[0]-pSep[0]); trisData.push_back(p1[1]-pSep[1]);trisData.push_back(p1[2]-pSep[2]);
  trisData.push_back(c1[0]);trisData.push_back(c1[1]); trisData.push_back(c1[2]); trisData.push_back(c1[3]);

  trisData.push_back(p2[0]+nSep[0]); trisData.push_back(p2[1]+nSep[1]);trisData.push_back(p2[2]+nSep[2]);
  trisData.push_back(c2[0]);trisData.push_back(c2[1]); trisData.push_back(c2[2]); trisData.push_back(c2[3]);

  trisData.push_back(p2[0]-nSep[0]); trisData.push_back(p2[1]-nSep[1]);trisData.push_back(p2[2]-nSep[2]);
  trisData.push_back(c2[0]);trisData.push_back(c2[1]); trisData.push_back(c2[2]); trisData.push_back(c2[3]);

  trisData.push_back(p1[0]-pSep[0]); trisData.push_back(p1[1]-pSep[1]);trisData.push_back(p1[2]-pSep[2]);
  trisData.push_back(c1[0]);trisData.push_back(c1[1]); trisData.push_back(c1[2]); trisData.push_back(c1[3]);
}


void GLApp::drawLines(const std::vector<float>& data, LineDrawType t, float lineThickness) {
  shaderUpdate();
  
  simpleProg.enable();
  simpleArray.bind();

  if (lineThickness > 1.0f) {
    std::vector<float> trisData;
    
    switch (t) {
      case LineDrawType::LIST :
        for (size_t i = 0;i<data.size()/7;i+=2) {
                    
          const size_t i1 = i;
          const size_t i2 = i+1;

          const Vec3 p1{data[i1*7+0],data[i1*7+1],data[i1*7+2]};
          const Vec4 c1{data[i1*7+3],data[i1*7+4],data[i1*7+5],data[i1*7+6]};
          const Vec3 p2{data[i2*7+0],data[i2*7+1],data[i2*7+2]};
          const Vec4 c2{data[i2*7+3],data[i2*7+4],data[i2*7+5],data[i2*7+6]};

          Vec3 p0{p1};
          Vec3 p3{p2};
          
          if (i1 >= 2 && p1[0] == data[(i1-1)*7+0] && p1[1] == data[(i1-1)*7+1] && p1[2] == data[(i1-1)*7+2]) {
            const size_t i0 = i-2;
            p0 = Vec3{data[i0*7+0],data[i0*7+1],data[i0*7+2]};
          }

          if (i2+2 < data.size()/7 && p2[0] == data[(i2+1)*7+0] && p2[1] == data[(i2+1)*7+1] && p2[2] == data[(i2+1)*7+2]) {
            const size_t i3 = i+2;
            p3 = Vec3{data[i3*7+0],data[i3*7+1],data[i3*7+2]};
          }

          triangulate(p0, p1, c1, p2, c2, p3, lineThickness, trisData);
        }
        break;
      case LineDrawType::STRIP :
        for (size_t i = 0;i<(data.size()/7)-1;++i) {
          
          const size_t i0 = (i==0) ? 0 : i-1;
          const size_t i1 = i;
          const size_t i2 = i+1;
          const size_t i3 = (i==(data.size()/7)-2) ? i2 : i2+1;
          
          const Vec3 p0{data[i0*7+0],data[i0*7+1],data[i0*7+2]};
          const Vec3 p1{data[i1*7+0],data[i1*7+1],data[i1*7+2]};
          const Vec4 c1{data[i1*7+3],data[i1*7+4],data[i1*7+5],data[i1*7+6]};
          const Vec3 p2{data[i2*7+0],data[i2*7+1],data[i2*7+2]};
          const Vec4 c2{data[i2*7+3],data[i2*7+4],data[i2*7+5],data[i2*7+6]};
          const Vec3 p3{data[i3*7+0],data[i3*7+1],data[i3*7+2]};

          triangulate(p0, p1, c1, p2, c2, p3, lineThickness, trisData);
        }
        break;
      case LineDrawType::LOOP :
        for (size_t i = 0;i<data.size()/7;++i) {

          const size_t i0 = (i==0) ? 0 : i-1;
          const size_t i1 = i;
          const size_t i2 = (i+1)%data.size();
          const size_t i3 = (i==(data.size()/7)-1) ? i2 : (i2+1)%data.size();
          
          const Vec3 p0{data[i0*7+0],data[i0*7+1],data[i0*7+2]};
          const Vec3 p1{data[i1*7+0],data[i1*7+1],data[i1*7+2]};
          const Vec4 c1{data[i1*7+3],data[i1*7+4],data[i1*7+5],data[i1*7+6]};
          const Vec3 p2{data[i2*7+0],data[i2*7+1],data[i2*7+2]};
          const Vec4 c2{data[i2*7+3],data[i2*7+4],data[i2*7+5],data[i2*7+6]};
          const Vec3 p3{data[i3*7+0],data[i3*7+1],data[i3*7+2]};

          triangulate(p0, p1, c1, p2, c2, p3, lineThickness, trisData);
        }
        break;
    }
#ifndef __EMSCRIPTEN__
    GL(glPolygonMode( GL_FRONT_AND_BACK, GL_FILL ));
#endif
    simpleVb.setData(trisData,7,GL_DYNAMIC_DRAW);
    simpleArray.connectVertexAttrib(simpleVb, simpleProg, "vPos", 3);
    simpleArray.connectVertexAttrib(simpleVb, simpleProg, "vColor", 4, 3);

    GL(glDrawArrays(GL_TRIANGLES, 0, GLsizei(trisData.size()/7)));
  } else {
    simpleVb.setData(data,7,GL_DYNAMIC_DRAW);
    simpleArray.connectVertexAttrib(simpleVb, simpleProg, "vPos", 3);
    simpleArray.connectVertexAttrib(simpleVb, simpleProg, "vColor", 4, 3);
    switch (t) {
      case LineDrawType::LIST :
        GL(glDrawArrays(GL_LINES, 0, GLsizei(data.size()/7)));
        break;
      case LineDrawType::STRIP :
        GL(glDrawArrays(GL_LINE_STRIP, 0, GLsizei(data.size()/7)));
        break;
      case LineDrawType::LOOP :
        GL(glDrawArrays(GL_LINE_LOOP, 0, GLsizei(data.size()/7)));
        break;
    }
  }
}

void GLApp::drawPoints(const std::vector<float>& data, float pointSize, bool useTex) {
  shaderUpdate();
  
  if (useTex) {
    if (pointSpriteHighlight.getHeight() > 0) {
      simpleHLSpriteProg.enable();
#ifdef __EMSCRIPTEN__
      simpleHLSpriteProg.setUniform("pointSize", pointSize);
#endif
      simpleHLSpriteProg.setTexture("pointSprite", pointSprite, 0);
      simpleHLSpriteProg.setTexture("pointSpriteHighlight", pointSpriteHighlight, 1);
      simpleVb.setData(data,7,GL_DYNAMIC_DRAW);
      simpleArray.bind();
      simpleArray.connectVertexAttrib(simpleVb, simpleHLSpriteProg, "vPos", 3);
      simpleArray.connectVertexAttrib(simpleVb, simpleHLSpriteProg, "vColor", 4, 3);
    } else {
      simpleSpriteProg.enable();
#ifdef __EMSCRIPTEN__
      simpleSpriteProg.setUniform("pointSize", pointSize);
#endif
      simpleSpriteProg.setTexture("pointSprite", pointSprite, 0);
      simpleVb.setData(data,7,GL_DYNAMIC_DRAW);
      simpleArray.bind();
      simpleArray.connectVertexAttrib(simpleVb, simpleSpriteProg, "vPos", 3);
      simpleArray.connectVertexAttrib(simpleVb, simpleSpriteProg, "vColor", 4, 3);
    }
    
  } else {
    simplePointProg.enable();
#ifdef __EMSCRIPTEN__
    simpleSpriteProg.setUniform("pointSize", pointSize);
#else
    GL(glPointSize(pointSize));
#endif
    simpleVb.setData(data,7,GL_DYNAMIC_DRAW);
    simpleArray.bind();
    simpleArray.connectVertexAttrib(simpleVb, simplePointProg, "vPos", 3);
    simpleArray.connectVertexAttrib(simpleVb, simplePointProg, "vColor", 4, 3);
  }

  GL(glDrawArrays(GL_POINTS, 0, GLsizei(data.size()/7)));
}

void GLApp::redrawTriangles(bool wireframe) {
  shaderUpdate();

  if (lastLighting) {
    simpleLightProg.enable();
    simpleArray.bind();
    simpleArray.connectVertexAttrib(simpleVb, simpleLightProg, "vPos", 3);
    simpleArray.connectVertexAttrib(simpleVb, simpleLightProg, "vColor", 4, 3);
    simpleArray.connectVertexAttrib(simpleVb, simpleLightProg, "vNormal", 3, 7);
  } else {
    simpleProg.enable();
    simpleArray.bind();
    simpleArray.connectVertexAttrib(simpleVb, simpleProg, "vPos", 3);
    simpleArray.connectVertexAttrib(simpleVb, simpleProg, "vColor", 4, 3);
  }


  switch (lastTrisType) {
    case TrisDrawType::LIST :
      if (wireframe) {
        GL(glDrawArrays(GL_LINES, 0, lastTrisCount));
      } else {
        GL(glDrawArrays(GL_TRIANGLES, 0, lastTrisCount));
      }
      break;
    case TrisDrawType::STRIP :
      GL(glDrawArrays(GL_TRIANGLE_STRIP, 0, lastTrisCount));
      break;
    case TrisDrawType::FAN :
      GL(glDrawArrays(GL_TRIANGLE_FAN, 0, lastTrisCount));
      break;
  }
}

static std::vector<float> convertTriangleFanToLines(
                                                    const std::vector<float>& fanVertices,
                                                    std::size_t compCount // floats per vertex
) {
  std::vector<float> lineVertices;

  const std::size_t totalVertices = fanVertices.size() / compCount;
  if (totalVertices < 3) return lineVertices; // Not enough for a triangle

  const float* v0 = &fanVertices[0]; // shared center vertex

  for (std::size_t i = 1; i < totalVertices - 1; ++i) {
    const float* v1 = &fanVertices[i * compCount];
    const float* v2 = &fanVertices[(i + 1) * compCount];

    // Line v0 -> v1
    lineVertices.insert(lineVertices.end(), v0, v0 + compCount);
    lineVertices.insert(lineVertices.end(), v1, v1 + compCount);

    // Line v1 -> v2
    lineVertices.insert(lineVertices.end(), v1, v1 + compCount);
    lineVertices.insert(lineVertices.end(), v2, v2 + compCount);

    // Line v2 -> v0
    lineVertices.insert(lineVertices.end(), v2, v2 + compCount);
    lineVertices.insert(lineVertices.end(), v0, v0 + compCount);
  }

  return lineVertices;
}

static std::vector<float> convertTriangleStripToLines(
                                               const std::vector<float>& stripVertices,
                                               std::size_t compCount // floats per vertex
) {
  std::vector<float> lineVertices;

  const std::size_t totalVertices = stripVertices.size() / compCount;
  if (totalVertices < 3) return lineVertices; // Not enough to form a triangle

  for (std::size_t i = 2; i < totalVertices; ++i) {
    const float* v0;
    const float* v1;
    const float* v2;

    // Determine winding order based on parity
    if ((i % 2) == 0) {
      // Even triangle: v0, v1, v2 = i-2, i-1, i
      v0 = &stripVertices[(i - 2) * compCount];
      v1 = &stripVertices[(i - 1) * compCount];
      v2 = &stripVertices[i * compCount];
    } else {
      // Odd triangle: v0, v1, v2 = i-1, i-2, i
      v0 = &stripVertices[(i - 1) * compCount];
      v1 = &stripVertices[(i - 2) * compCount];
      v2 = &stripVertices[i * compCount];
    }

    // Line v0 -> v1
    lineVertices.insert(lineVertices.end(), v0, v0 + compCount);
    lineVertices.insert(lineVertices.end(), v1, v1 + compCount);

    // Line v1 -> v2
    lineVertices.insert(lineVertices.end(), v1, v1 + compCount);
    lineVertices.insert(lineVertices.end(), v2, v2 + compCount);

    // Line v2 -> v0
    lineVertices.insert(lineVertices.end(), v2, v2 + compCount);
    lineVertices.insert(lineVertices.end(), v0, v0 + compCount);
  }
  return lineVertices;
}

static std::vector<float> convertTrianglesToLines(
                                           const std::vector<float>& triangleVertices,
                                           std::size_t compCount // number of floats per vertex
) {
  std::vector<float> lineVertices;

  const std::size_t floatsPerTriangle = 3 * compCount;

  if (triangleVertices.size() % floatsPerTriangle != 0) {
    // Not a clean set of triangles
    return lineVertices;
  }

  for (std::size_t i = 0; i < triangleVertices.size(); i += floatsPerTriangle) {
    // Extract pointers to the three full vertices
    const float* v0 = &triangleVertices[i];
    const float* v1 = &triangleVertices[i + compCount];
    const float* v2 = &triangleVertices[i + 2 * compCount];

    // Line v0 -> v1
    lineVertices.insert(lineVertices.end(), v0, v0 + compCount);
    lineVertices.insert(lineVertices.end(), v1, v1 + compCount);

    // Line v1 -> v2
    lineVertices.insert(lineVertices.end(), v1, v1 + compCount);
    lineVertices.insert(lineVertices.end(), v2, v2 + compCount);

    // Line v2 -> v0
    lineVertices.insert(lineVertices.end(), v2, v2 + compCount);
    lineVertices.insert(lineVertices.end(), v0, v0 + compCount);
  }

  return lineVertices;
}

void GLApp::drawTriangles(const std::vector<float>& data, TrisDrawType t, bool wireframe, bool lighting) {
  shaderUpdate();

  size_t compCount = lighting ? 10 : 7;

  if (wireframe) {
    std::vector<float> lineVerts;
    switch (t) {
      case TrisDrawType::LIST:
        lineVerts = convertTrianglesToLines(data,compCount);
        break;
      case TrisDrawType::STRIP:
        lineVerts = convertTriangleStripToLines(data,compCount);
        break;
      case TrisDrawType::FAN:
        lineVerts = convertTriangleFanToLines(data,compCount);
        break;
    }

    simpleVb.setData(lineVerts,compCount,GL_DYNAMIC_DRAW);
    lastTrisCount = GLsizei(lineVerts.size()/compCount);
  } else {
    simpleVb.setData(data,compCount,GL_DYNAMIC_DRAW);
    lastTrisCount = GLsizei(data.size()/compCount);
  }
  lastLighting = lighting;
  lastTrisType = t;

  redrawTriangles(wireframe);
}

void GLApp::setDrawProjection(const Mat4& mat) {
  p = mat;
}

Mat4 GLApp::getDrawProjection() const {
  return p;
}

Mat4 GLApp::getDrawTransform() const {
  return mv;
}

void GLApp::setDrawTransform(const Mat4& mat) {
  mv = mat;
  mvi = Mat4::inverse(mv);
}

void GLApp::shaderUpdate() {
  simpleProg.enable();
  simpleProg.setUniform("MVP", p*mv);

  simplePointProg.enable();
  simplePointProg.setUniform("MVP", p*mv);

  simpleSpriteProg.enable();
  simpleSpriteProg.setUniform("MVP", p*mv);

  simpleHLSpriteProg.enable();
  simpleHLSpriteProg.setUniform("MVP", p*mv);

  simpleTexProg.enable();
  simpleTexProg.setUniform("MVP", p*mv);

  simpleLightProg.enable();
  simpleLightProg.setUniform("MVP", p*mv);
  simpleLightProg.setUniform("MV", mv);
  simpleLightProg.setUniform("MVit", mvi, true);
}

void GLApp::setImageFilter(GLint magFilter, GLint minFilter) {
  raster.setFilter(magFilter, minFilter);
}

void GLApp::drawImage(const GLTexture2D& image, const Vec2& bl, const Vec2& tr) {
  drawImage(image,
            {bl.x,bl.y,0.0f},
            {tr.x,bl.y,0.0f},
            {bl.x,tr.y,0.0f},
            {tr.x,tr.y,0.0f});
}

void GLApp::drawImage(const Image& image, const Vec2& bl, const Vec2& tr) {
    drawImage(image,
              {bl.x,bl.y,0.0f},
              {tr.x,bl.y,0.0f},
              {bl.x,tr.y,0.0f},
              {tr.x,tr.y,0.0f});
}


void GLApp::drawImage(const GLTexture2D& image, const Vec3& bl,
                      const Vec3& br, const Vec3& tl,
                      const Vec3& tr) {

  shaderUpdate();
  
  simpleTexProg.enable();
  
  std::vector<float> data = {
    tr[0], tr[1], tr[2], 1.0f, 1.0f,
    br[0], br[1], br[2], 1.0f, 0.0f,
    tl[0], tl[1], tl[2], 0.0f, 1.0f,
    tl[0], tl[1], tl[2], 0.0f, 1.0f,
    bl[0], bl[1], bl[2], 0.0f, 0.0f,
    br[0], br[1], br[2], 1.0f, 0.0f
  };
  
  simpleVb.setData(data,5,GL_DYNAMIC_DRAW);
  
  simpleArray.bind();
  simpleArray.connectVertexAttrib(simpleVb, simpleTexProg, "vPos", 3);
  simpleArray.connectVertexAttrib(simpleVb, simpleTexProg, "vTexCoords", 2, 3);
  simpleTexProg.setTexture("raster",image,0);

  GL(glDrawArrays(GL_TRIANGLES, 0, GLsizei(data.size()/5)));
}

void GLApp::drawImage(const Image& image, const Vec3& bl,
                      const Vec3& br, const Vec3& tl,
                      const Vec3& tr) {

  raster.setData(image.data, image.width, image.height, image.componentCount);
  drawImage(raster, bl, br, tl, tr);
}

void GLApp::drawRect(const Vec4& color, const Vec2& bl, const Vec2& tr) {
  drawRect(color,
            {bl.x,bl.y,0.0f},
            {tr.x,bl.y,0.0f},
            {bl.x,tr.y,0.0f},
            {tr.x,tr.y,0.0f});
}

void GLApp::drawRect(const Vec4& color, const Vec3& bl, const Vec3& br,
                     const Vec3& tl, const Vec3& tr) {
  drawImage(Image{color}, bl, br, tl, tr);
}

Mat4 GLApp::computeImageTransform(const Vec2ui& imageSize) const {
  const Dimensions s = glEnv.getWindowSize();
  const float ax = imageSize.x/float(s.width);
  const float ay = imageSize.y/float(s.height);
  const float m = std::max(ax,ay);
  return Mat4::scaling({ax/m, ay/m, 1.0f});
}

Mat4 GLApp::computeImageTransformFixedHeight(const Vec2ui& imageSize,
                                             float height,
                                             const Vec3& center) const {
  const Dimensions s = glEnv.getWindowSize();
  const float ax = imageSize.x/float(s.width);
  const float ay = imageSize.y/float(s.height);
  return Mat4::translation(center) * Mat4::scaling({height*ax/ay, height, 1.0f});
}

Mat4 GLApp::computeImageTransformFixedWidth(const Vec2ui& imageSize,
                                            float width,
                                            const Vec3& center) const {
  const Dimensions s = glEnv.getWindowSize();
  const float ax = imageSize.x/float(s.width);
  const float ay = imageSize.y/float(s.height);
  return Mat4::translation(center) * Mat4::scaling({width, width*ay/ax, 1.0f});
}

static std::string getScriptFilename(const std::vector<std::string>& args) {
  for (std::size_t i = 0; i + 1 < args.size(); ++i) {
    std::string key = args[i];
    std::transform(key.begin(), key.end(), key.begin(),
                   [](unsigned char c){ return std::tolower(c); });
    if (key == "--script") {
      return args[i + 1];
    }
  }
  return "";
}

void GLApp::initScript(const std::vector<std::string>& args) {
  const std::string scriptName = getScriptFilename(args);
  if (!scriptName.empty()) {
    scriptRunning = interpreter.loadFromFile(scriptName) == CommandResultCode::success;
    interpreter.registerCommand(
                                "reset",
                                [this]() {
                                  reset();
                                }
                                );
    interpreter.registerCommand(
                                "setinteraction",
                                [this](bool enabled) {
                                  interaction = enabled;
                                  setInteractionCallbacks();
                                }
                                );
    interpreter.registerCommand(
                                "setbackground",
                                [this](float r, float g, float b, float a) {
                                  setBackground(r,g,b,a);
                                }
                                );
    interpreter.registerCommand(
                                "resize",
                                [this](int width, int height) {
                                  glEnv.setSize(width, height);
                                }
                                );
    interpreter.registerCommand(
                                "screenshot",
                                [this]() -> CommandResultCode {
                                  std::filesystem::path base = logDir;

                                  // make sure both front and back buffer
                                  // are updated
                                  static int needsUpdate = 2;
                                  if (needsUpdate) {
                                    needsUpdate--;
                                    return CommandResultCode::waitingNoop;
                                  } else {
                                    needsUpdate = 2;
                                  }
                                  static uint32_t imageCounter = 0;
                                  std::stringstream ss;
                                  ss << "screenshot-" << std::setw(6) << std::setfill('0') << imageCounter++ << ".bmp";

                                  std::filesystem::path filePath = base / ss.str();

                                  return GLScreenshot::saveBmp(filePath.string())
                                  ? CommandResultCode::success
                                  : CommandResultCode::callbackError;
                                }
                                );
    interpreter.registerCommand(
                                "screenshot",
                                [this](std::string filename) -> CommandResultCode {
                                  std::filesystem::path base = logDir;
                                  std::filesystem::path filePath = base / filename;

                                  static bool needsUpdate = true;
                                  if (needsUpdate) {
                                    needsUpdate = false;
                                    return CommandResultCode::waitingNoop;
                                  } else {
                                    needsUpdate = true;
                                  }

                                  return GLScreenshot::saveBmp(filePath.string())
                                  ? CommandResultCode::success
                                  : CommandResultCode::callbackError;
                                }
                                );

    interpreter.registerCommand(
                                "setfpswindow",
                                [this](int window) -> CommandResultCode {
                                  if (window < 1) {
                                    return CommandResultCode::invalidArguments;
                                  }
                                  glEnv.setFPSAccumulationInterval(static_cast<uint64_t>(window));
                                  return CommandResultCode::success;
                                }
                                );
    interpreter.registerCommand(
                                "clearlog",
                                [this]() -> CommandResultCode {
                                  if (scriptLogFile.empty()) {
                                    return CommandResultCode::success;
                                  }
                                  std::filesystem::path base = logDir;
                                  std::filesystem::path filePath = base / scriptLogFile;
                                  std::filesystem::remove(filePath);
                                  return CommandResultCode::success;
                                }
                                );
    interpreter.registerCommand(
                                "logfile",
                                [this](std::string filename) {
                                  scriptLogFile = std::move(filename);
                                }
                                );
    interpreter.registerCommand(
                                "logtime",
                                [this]() -> CommandResultCode {
                                  auto now = std::chrono::system_clock::now();
                                  std::time_t t = std::chrono::system_clock::to_time_t(now);

                                  std::stringstream s;
#ifdef _WIN32
                                  std::tm tm_buf{};
                                  errno_t err = localtime_s(&tm_buf, &t);
                                  if (err != 0) {
                                    return CommandResultCode::callbackError;
                                  }
                                  s << std::put_time(&tm_buf, "%Y-%m-%d %H:%M:%S");
#else
                                  s << std::put_time(std::localtime(&t), "%Y-%m-%d %H:%M:%S");
#endif
                                  if (writeScriptLog(s.str())) {
                                    return CommandResultCode::success;
                                  }
                                  return CommandResultCode::callbackError;
                                }
                                );
    interpreter.registerCommand(
                                "logfps",
                                [this]() -> CommandResultCode {
                                  if (!glEnv.getFPSCounterStatus()) {
                                    if (writeScriptLog("fpsCounter disabled")) {
                                      return CommandResultCode::success;
                                    }
                                    return CommandResultCode::callbackError;
                                  }

                                  if (glEnv.getFps() == 0) {
                                    return CommandResultCode::waitingNoop;
                                  }

                                  std::stringstream s;
                                  s << static_cast<int>(std::ceil(glEnv.getFps())) << " fps";
                                  if (writeScriptLog(s.str())) {
                                    return CommandResultCode::success;
                                  }
                                  return CommandResultCode::callbackError;
                                }
                                );

    interpreter.registerCommand(
                                "logGLInfo",
                                [this](bool includeExtensions) -> CommandResultCode {
                                  const std::string result = glEnv.getOpenGlInfoString(includeExtensions);
                                  if (writeScriptLog(result)) {
                                    return CommandResultCode::success;
                                  }
                                  return CommandResultCode::callbackError;
                                }
                                );
    interpreter.registerCommand(
                                "log",
                                CommandInterpreter::Callback{
                                  [this](const std::vector<std::string>& args) {
                                    std::string result;
                                    for (size_t i = 0; i < args.size(); ++i) {
                                      result += args[i];
                                      if (i + 1 < args.size()) result += ' ';
                                    }
                                    return writeScriptLog(result) ? CommandResultCode::success
                                    : CommandResultCode::callbackError;
                                  }
                                }
                                );

    interpreter.registerCommand(
                                "setdir",
                                [this](std::string dir) -> CommandResultCode {
                                  std::error_code ec;
                                  std::filesystem::create_directories(dir, ec);
                                  if (ec) {
                                    return CommandResultCode::callbackError;
                                  }
                                  logDir = std::move(dir);
                                  return CommandResultCode::success;
                                }
                                );

    interpreter.registerCommand(
                                "quit",
                                [this]() {
                                  closeWindow();
                                }
                                );
    interpreter.setUnknownCommandHandler(
                                         [](const std::string &command,
                                                const std::vector<std::string> &args) {
                                                  std::cerr << "unknown command: " << command << "\n";
                                                  return CommandResultCode::unknownCommand;
                                                });
  } else {
    scriptRunning = false;
  }
}

bool GLApp::writeScriptLog(const std::string &text) const {
  if (scriptLogFile.empty()) return false;

  std::filesystem::path base = logDir;
  std::filesystem::path filePath = base / scriptLogFile;

  std::ofstream out(filePath, std::ios::app);
  if (!out) return false;

  out << text << std::endl;
  return true;
}

#ifdef _WIN32
#include <Windows.h>

std::vector<std::string> getArgsWindows() {
  int argc = 0;
  LPWSTR* argv = CommandLineToArgvW(GetCommandLineW(), &argc);
  std::vector<std::string> args;

  if (argv) {
    for (int i = 1; i < argc; ++i) {
      int len = WideCharToMultiByte(CP_UTF8, 0, argv[i], -1, nullptr, 0, nullptr, nullptr);
      std::string s(len - 1, '\0');
      WideCharToMultiByte(CP_UTF8, 0, argv[i], -1, s.data(), len, nullptr, nullptr);
      args.push_back(std::move(s));
    }
    LocalFree(argv);
  }
  return args;
}
#endif

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

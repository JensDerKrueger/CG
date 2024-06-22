#include <GLApp.h>
#include <FontRenderer.h>
#include "ColorConversion.h"
#include "ResourceFile.h"

#ifdef SPECTRO
#include "Spectrometer.h"
#endif
#include <cmath>

const uint32_t resolution = 1000;
const size_t valCount = 1000*7;
const uint32_t minWaveLength = 300;
const uint32_t maxWaveLength = 700;
float spectralPowerDistributionScale = 20.0f;

class MyGLApp : public GLApp {
public:

  bool bRenderSensitivityCurve = false;
  bool bRenderFinalColor = true;
  bool bRenderRainbow = true;
  bool bRenderExcitement = false;
  bool bRenderCursorWavelength = false;
  bool bPerceptuallyWeighted = false;

  Image rainbow{1000,1};
  std::vector<float> conesL;
  std::vector<float> conesM;
  std::vector<float> conesS;
  Image color{1,1};

  std::vector<float> spectralPowerDistributionStored;
  std::vector<float> spectralPowerDistribution;
  size_t lastIndex;
  bool leftMouseDown;

  FontRenderer fr{ResourceFile::getFilePath("helvetica_neue.bmp"), ResourceFile::getFilePath("helvetica_neue.pos")};
  std::shared_ptr<FontEngine> fe{nullptr};
  std::string text;

# ifdef SPECTRO
  Spectrometer spectrometer{};
  bool light{false};
  bool showSpectroReadings{false};
#endif

  MyGLApp() :
  GLApp{1000,500,1,"Metamerism Simulator"},
  conesL(valCount*7),
  conesM(valCount*7),
  conesS(valCount*7),
  spectralPowerDistribution(resolution)
  {}
  
  void gaussiansToCurves() {
    for (size_t i =0;i<valCount;++i) {
      const float wavelength = (float(i)/valCount)*float(minWaveLength)+float(maxWaveLength-minWaveLength);
      const Vec3 cs = coneSensitivities(wavelength);

      const float x = normalize(float(i)/valCount);
      float y = normalize(cs.x);
      conesL[i*7+0] = x; conesL[i*7+1] = y; conesL[i*7+2] = 1;
      conesL[i*7+3] = 0; conesL[i*7+4] = 0; conesL[i*7+5] = 1; conesL[i*7+6] = 1;

      y = normalize(cs.y);
      conesM[i*7+0] = x; conesM[i*7+1] = y; conesM[i*7+2] = 1;
      conesM[i*7+3] = 0; conesM[i*7+4] = 1; conesM[i*7+5] = 0; conesM[i*7+6] = 1;

      y = normalize(cs.z);
      conesS[i*7+0] = x; conesS[i*7+1] = y; conesS[i*7+2] = 1;
      conesS[i*7+3] = 1; conesS[i*7+4] = 0; conesS[i*7+5] = 0; conesS[i*7+6] = 1;
    }
  }
  
  virtual void init() override {
    fe = fr.generateFontEngine();
    for (uint32_t y = 0;y<rainbow.height;++y) {
      for (uint32_t x = 0;x<rainbow.width;++x) {
        const float wavelength = (float(x)/rainbow.width)*float(minWaveLength)+float(maxWaveLength-minWaveLength);
        const Vec3 rgb = ColorConversion::wavelengthToRGB(wavelength);
        rainbow.setNormalizedValue(x,y,0,rgb.r); rainbow.setNormalizedValue(x,y,1,rgb.g);
        rainbow.setNormalizedValue(x,y,2,rgb.b); rainbow.setValue(x,y,3,255);
      }
    }

    gaussiansToCurves();


    GL(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));
    GL(glBlendEquation(GL_FUNC_ADD));
    GL(glEnable(GL_BLEND));
  }

    virtual void mouseButton(int button, int state, int mods,
                           double xPosition, double yPosition) override {


    Dimensions s = glEnv.getWindowSize();
    if (xPosition < 0 || xPosition > s.width || yPosition < 0 || yPosition > s.height) return;
    if (button == GLFW_MOUSE_BUTTON_LEFT && state == GLFW_PRESS) {
      lastIndex = size_t(float(xPosition)/s.width*spectralPowerDistribution.size());
      leftMouseDown = true;
    }
    if (button == GLFW_MOUSE_BUTTON_LEFT && state == GLFW_RELEASE) {
      leftMouseDown = false;
    }
  }
    
  float gaussian(float x, float A, float mu, float sigma) {
    return A * expf(-powf(x - mu, 2.0f) / (2.0f * powf(sigma, 2.0f)));
  }
/*
   float sConeSensitivity(float wavelength) {
     if (wavelength < 400.0f || wavelength > 550.0f) return 0.0f;
     return gaussian(wavelength, 5.6179775281f * 0.178f, 471.92f, 21.71f);
   }

   float mConeSensitivity(float wavelength) {
     if (wavelength < 400.0f || wavelength > 700.0f) return 0.0f;
     return gaussian(wavelength, 5.6179775281f * 0.099f, 548.34f, 48.969f);
   }

   float lConeSensitivity(float wavelength) {
     if (wavelength < 400.0f || wavelength > 700.0f) return 0.0f;
     return gaussian(wavelength, 5.6179775281f * 0.089f, 568.60f, 58.64725f);
   }

  */
  
  float sConeSensitivity(float wavelength) {
    if (wavelength < 400.0f || wavelength > 550.0f) return 0.0f;
    if (bPerceptuallyWeighted)
      return gaussian(wavelength, 5.6179775281f * 0.178f, 471.92f, 21.71f);
    else
      return gaussian(wavelength, 1.6129032258f* 0.05f, 445, 21.71f);
  }

  float mConeSensitivity(float wavelength) {
    if (wavelength < 400.0f || wavelength > 700.0f) return 0.0f;
    if (bPerceptuallyWeighted)
      return gaussian(wavelength, 5.6179775281f * 0.099f, 548.34f, 48.969f);
    else
      return gaussian(wavelength, 1.6129032258f*0.3f, 540, 48.969f);
  }

  float lConeSensitivity(float wavelength) {
    if (wavelength < 400.0f || wavelength > 700.0f) return 0.0f;
    if (bPerceptuallyWeighted)
      return gaussian(wavelength, 5.6179775281f * 0.089f, 568.60f, 58.64725f);
    else
      return gaussian(wavelength, 1.6129032258f*0.62f, 560, 50);
  }

  Vec3 coneSensitivities(float wavelength) {
    return {
      sConeSensitivity(wavelength),
      mConeSensitivity(wavelength),
      lConeSensitivity(wavelength)
    };
  }

  virtual void mouseMove(double xPosition, double yPosition) override {
    Dimensions s = glEnv.getWindowSize();
    if (xPosition < 0 || xPosition > s.width || yPosition < 0 || yPosition > s.height) return;
    const float wavelength = (float(xPosition)/s.width)*float(minWaveLength)+float(maxWaveLength-minWaveLength);
    std::stringstream ss; ss << "Wavelength: " << wavelength << "nm"; text = ss.str();

    if (s.height-yPosition < 10) yPosition = s.height;

    if (leftMouseDown) {
      size_t index = size_t(float(xPosition)/s.width*spectralPowerDistribution.size());

      if (lastIndex < index) {
        for (size_t i = lastIndex; i <= index; ++i) {
          spectralPowerDistribution[i] = 1-(float(yPosition)/s.height);
        }
      } else {
        for (size_t i = index; i <= lastIndex; ++i) {
          spectralPowerDistribution[i] = 1-(float(yPosition)/s.height);
        }
      }
      lastIndex = index;
    }
  }

  virtual void keyboard(int key, int scancode, int action, int mods) override {
    if (action == GLFW_PRESS) {
      switch (key) {
# ifdef SPECTRO
        case GLFW_KEY_L:
          light = !light;
          spectrometer.light(light);
          break;
        case GLFW_KEY_S:
          showSpectroReadings = !showSpectroReadings;
      //    if (showSpectroReadings && !spectrometer.isConnected()) {
      //      spectrometer.reconnect();
      //    }
          break;
#endif
        case GLFW_KEY_ESCAPE :
          closeWindow();
          break;
        case GLFW_KEY_R :
          std::fill(spectralPowerDistribution.begin(), spectralPowerDistribution.end(), 0);
          spectralPowerDistributionStored.clear();
          break;
        case GLFW_KEY_3 :
          bRenderSensitivityCurve = !bRenderSensitivityCurve;
          break;
        case GLFW_KEY_2 :
          bRenderFinalColor = !bRenderFinalColor;
          break;
        case GLFW_KEY_1 :
          bRenderRainbow = !bRenderRainbow;
          break;
        case GLFW_KEY_4 :
          bRenderExcitement = !bRenderExcitement;
          break;
        case GLFW_KEY_5 :
          bRenderCursorWavelength = !bRenderCursorWavelength;
          break;
        case GLFW_KEY_6:
          bPerceptuallyWeighted = !bPerceptuallyWeighted;
          gaussiansToCurves();
          break;
        case GLFW_KEY_UP :
          spectralPowerDistributionScale += 1;
          break;
        case GLFW_KEY_DOWN :
          spectralPowerDistributionScale -= 1;
          break;
        case GLFW_KEY_SPACE :
          spectralPowerDistributionStored = spectralPowerDistribution;
          std::fill(spectralPowerDistribution.begin(), spectralPowerDistribution.end(), 0);
          break;
      }
    }
  }

  float clamp(const float f) {
    return std::max<float>(0,std::min<float>(1,f));
  }

  Vec3 clamp(const Vec3& v) {
    return {clamp(v.x),clamp(v.y),clamp(v.z)};
  }

  float normalize(float f) {
    return -1.0f + 2.0f*f;
  }


  void renderData(const std::vector<float>& dataLine,
                  const Vec4& lineColor={1.0f,1.0f,1.0f,1.0f},
                  const float xOffset=0.0f) {

    const Dimensions dim{ glEnv.getFramebufferSize() };

    Vec3 totalColor{0,0,0};
    Vec3 totalResponse{0,0,0};
    for (size_t i =0;i<dataLine.size();++i) {
      const float wavelength = (float(i)/dataLine.size())*float(minWaveLength)+float(maxWaveLength-minWaveLength);
      const Vec3 rgb = ColorConversion::wavelengthToRGB<float>(wavelength);
      totalColor = totalColor + rgb*dataLine[i]*spectralPowerDistributionScale;
      totalResponse = totalResponse + coneSensitivities(wavelength)*dataLine[i]*spectralPowerDistributionScale;
    }

    totalColor = clamp(totalColor/dataLine.size());
    totalResponse = clamp(totalResponse/dataLine.size());
    if (bRenderFinalColor) {
      color.setNormalizedValue(0,0,0,totalColor.r);
      color.setNormalizedValue(0,0,1,totalColor.g);
      color.setNormalizedValue(0,0,2,totalColor.b);
      color.setValue(0,0,3,255);
      drawImage(color,{0.7f+xOffset,0.7f},{0.8f+xOffset,0.9f});
      std::stringstream ss; ss << totalColor;
      fe->render(ss.str(), dim.aspect(), 0.02f, {0.75f+xOffset,0.65f}, Alignment::Center, {1,1,1,1});
    }
    std::vector<float> line(dataLine.size()*7);
    for (size_t i =0;i<dataLine.size();++i) {
      const float x = normalize(float(i)/dataLine.size());
      const float y = normalize(dataLine[i]);
      line[i*7+0] = x; line[i*7+1] = y; line[i*7+2] = 0;
      line[i*7+3] = lineColor.r; line[i*7+4] = lineColor.g; line[i*7+5] = lineColor.b;
      line[i*7+6] = lineColor.a;
    }
    drawLines(line, LineDrawType::STRIP,2);
    if (bRenderExcitement) {
      drawRect({0.0f,0.0f,1.0f,1.0f},{0.85f+xOffset,-0.1f},{0.8f+xOffset,0.6f*totalResponse.x-0.1f});
      drawRect({0.0f,1.0f,0.0f,1.0f},{0.75f+xOffset,-0.1f},{0.7f+xOffset,0.6f*totalResponse.y-0.1f});
      drawRect({1.0f,0.0f,0.0f,1.0f},{0.65f+xOffset,-0.1f},{0.6f+xOffset,0.6f*totalResponse.z-0.1f});

      fe->render("L", dim.aspect(), 0.03f, {0.625f+xOffset,-0.15f}, Alignment::Center, {1,1,1,1});
      fe->render("M", dim.aspect(), 0.03f, {0.725f+xOffset,-0.15f}, Alignment::Center, {1,1,1,1});
      fe->render("S", dim.aspect(), 0.03f, {0.825f+xOffset,-0.15f}, Alignment::Center, {1,1,1,1});
    }
  }

# ifdef SPECTRO
  virtual void animate(double animationTime) override {
    if (showSpectroReadings) {
      std::optional<Distribution> d = spectrometer.get();

      if (d) {
        std::fill(spectralPowerDistribution.begin(), spectralPowerDistribution.end(), 0);

        size_t index = 0;
        for (size_t i = 0;i<spectralPowerDistribution.size();++i) {
          const float wavelength = (float(i)/spectralPowerDistribution.size())*float(minWaveLength)+float(maxWaveLength-minWaveLength);

          if (index < wavelengths.size()-1 && abs(wavelength - wavelengths[index]) > abs(wavelength - wavelengths[index+1])) {
            index++;
          }
          spectralPowerDistribution[i] = d->at(index)/60000.0f;
        }
      }
    }
  }
#endif

  virtual void draw() override {
    GL(glClear(GL_COLOR_BUFFER_BIT));
    const Dimensions dim{ glEnv.getFramebufferSize() };

    if (bRenderRainbow) {
      drawImage(rainbow,{-1.0f,-1.0f},{1.0f,-0.9f});
    }

    if (bRenderSensitivityCurve) {
      drawLines(conesL, LineDrawType::STRIP,2);
      drawLines(conesM, LineDrawType::STRIP,2);
      drawLines(conesS, LineDrawType::STRIP,2);
    }

    renderData(spectralPowerDistribution, {1.0f,1.0f,1.0f,1.0f}, 0.0f);
    if (!spectralPowerDistributionStored.empty()) {
      renderData(spectralPowerDistributionStored, {1.0f,1.0f,0.0f,1.0f}, -1.5f);
    }

    if (bRenderCursorWavelength){
      fe->render(text, dim.aspect(), 0.04f, {0,0.95f}, Alignment::Center, {1,1,1,1});
    }

  }
};


#ifdef _WIN32
#include <Windows.h>
INT WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR lpCmdLine, INT nCmdShow) {
#else
int main(int argc, char** argv) {
#endif
  try {
    MyGLApp myApp;
    myApp.run();
  }
  catch (const GLException& e) {
    std::stringstream ss;
    ss << "Insufficient OpenGL Support " << e.what();
#ifndef _WIN32
    std::cerr << ss.str().c_str() << std::endl;
#else
    MessageBoxA(
      NULL,
      ss.str().c_str(),
      "OpenGL Error",
      MB_ICONERROR | MB_OK
    );
#endif
    return EXIT_FAILURE;
  }
  return EXIT_SUCCESS;
}

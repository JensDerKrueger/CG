#pragma once

#include "Image.h"
#include "Vec3.h"
#include "TextureCoordinates.h"

enum class FilterMode {
	NEAREST, BILINEAR
};

enum class BorderMode {
	CLAMP_TO_EDGE, CLAMP_TO_BORDER, REPEAT, MIRRORED_REPEAT
};

class Texture {
  uint32_t width;
  uint32_t height;
	FilterMode filterMode;
	BorderMode borderModeU;
	BorderMode borderModeV;
	Vec3 borderColor;
	std::shared_ptr<Image> data{};

public:

	Texture(uint32_t width, uint32_t height);
	Texture(uint32_t width, uint32_t height, FilterMode filterMode);
	Texture(uint32_t width, uint32_t height, FilterMode filterMode, BorderMode borderMode);
	Texture(const std::string& filename);
	Texture(const std::string& filename, FilterMode filterMode);
	Texture(const std::string& filename, FilterMode filterMode, BorderMode borderMode);

	static Texture genCheckerboardTexture(uint32_t width, uint32_t height);

	Vec3 sample(const TextureCoordinates& texCoords) const;
	
  uint32_t getWidth() const { return width; }
  uint32_t getHeight() const { return height; }
	void setBorderMode(BorderMode borderMode);
	void setBorderModeU(BorderMode borderMode);
	void setBorderModeV(BorderMode borderMode);
	void setBorderColor(const Vec3& borderColor);

private:
	Vec3 sample(int pixelCoordX, int pixelCoordY) const;
};


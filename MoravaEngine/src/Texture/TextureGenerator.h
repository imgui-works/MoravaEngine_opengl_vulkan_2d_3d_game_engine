#pragma once

#include "Texture/MoravaTexture.h"


/**
 * Based on Procedural Landmass Generation series by Sebastian Lague
 */
class TextureGenerator
{
public:
	static H2M::RefH2M<MoravaTexture> TextureFromColorMap(glm::vec4* colorMap, const char* fileLocation, int width, int height);
	static H2M::RefH2M<MoravaTexture> TextureFromHeightMap(float** noiseMap, const char* fileLocation, int width, int height);

};

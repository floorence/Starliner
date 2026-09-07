#include <cmath>
#include <stb/stb_image.h>
#include "texture/ImageTexture.h"
#include "util/Log.h"
#include "util/Utils.h"
#include <fmt/format.h>

ImageTexture::ImageTexture(const char* image, TextureType texType, GLenum pixelType, bool convertToSpecular)
	: AssetTexture((texType == TextureType::Diffuse) ? "material.diffuse" : "material.specular")
{
	type = texType;
	path = image;

	int widthImg, heightImg, numColCh;
	stbi_set_flip_vertically_on_load(true);

	unsigned char* bytes = stbi_load(image, &widthImg, &heightImg, &numColCh, STBI_rgb_alpha); // force 4 colour channels
	if (!bytes) {
		Log::err(TAG, fmt::format("Failed to load texture: {}", image));
		stbi_image_free(bytes);
		return;
	}

	int realNumColCh = 4;
	if (texType == TextureType::Specular) {
		Log::log(TAG, "specular texture");
		unsigned char* redChannel = new unsigned char[widthImg * heightImg];
		for (int i = 0; i < widthImg * heightImg; i++) {
			unsigned char r = bytes[i * realNumColCh + 0];
			unsigned char g = bytes[i * realNumColCh + 1];
			unsigned char b = bytes[i * realNumColCh + 2];

			float gray = r; // image is black and white if texType == specular and convertToSpecular == false

			if (convertToSpecular) {
				gray = Utils::getBrightness(r, g, b);

				gray = pow(gray / 255.0f, GAMMA_CORRECT) * 255.0f;
				gray = std::min(gray * BOOST_COEFFICIENT, 255.0f);
			}

			redChannel[i] = static_cast<unsigned char>(gray);
		}

		setTextureData(redChannel, GL_RED, pixelType, widthImg, heightImg);
		delete[] redChannel;
	} else { // texType == diffuse
		setTextureData(bytes, GL_RGBA, pixelType, widthImg, heightImg);
	}

	stbi_image_free(bytes);
}

std::string ImageTexture::getPath() {
	return path;
}
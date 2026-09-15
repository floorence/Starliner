#pragma once

#include "texture/Texture.h"

enum class TextureType {
	Diffuse, Specular
};

class AssetTexture: public Texture {
public:
	TextureType type = TextureType::Diffuse;

	AssetTexture(std::string uniform);

	virtual ~AssetTexture() = default;

    AssetTexture(AssetTexture&& other) noexcept;
    AssetTexture& operator=(AssetTexture&& other) noexcept;
protected:
	void setTextureData(unsigned char* bytes, GLenum format, GLenum pixelType, int width, int height);
};

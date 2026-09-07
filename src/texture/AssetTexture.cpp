#include "AssetTexture.h"

AssetTexture::AssetTexture(std::string uniform): Texture(uniform) {}

void AssetTexture::setTextureData(unsigned char* bytes, GLenum format, GLenum pixelType, int width, int height) {
	bind();

	// how the image is scaled
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	// how the image repeats
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	
	// make the alignment 1 otherwise opengl will assume 4 alignment which may cause issues with !=4 channel textures
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	// assigns the image to the OpenGL Texture object
	glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, pixelType, bytes);
	glGenerateMipmap(GL_TEXTURE_2D);
}

AssetTexture::AssetTexture(AssetTexture&& other) noexcept
    : Texture(std::move(other)), type(other.type) {}

AssetTexture& AssetTexture::operator=(AssetTexture&& other) noexcept {
	if (this != &other) {
		Texture::operator=(std::move(other));
		type = other.type;
	}
	return *this;
}


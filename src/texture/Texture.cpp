#include "Texture.h"

Texture::Texture(std::string uniform)
	: uniform(uniform)
{
	glGenTextures(1, &ID);
}

void Texture::bind() {
	glBindTexture(GL_TEXTURE_2D, ID);
}

void Texture::bind(GLuint unit) {
	glActiveTexture(GL_TEXTURE0 + unit);
    bind();
}

Texture::~Texture() {
	glDeleteTextures(1, &ID);
}

Texture::Texture(Texture&& other) noexcept
    : ID(other.ID)
{
    other.ID = 0;
}

Texture& Texture::operator=(Texture&& other) noexcept {
	if (this != &other) {
		glDeleteTextures(1, &ID);

		ID = other.ID;
		other.ID = 0;
	}
	return *this;
}
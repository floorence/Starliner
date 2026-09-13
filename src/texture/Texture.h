#pragma once

#include "glad/glad.h"
#include <string>

class Texture {
public:
    GLuint ID = 0;
    std::string uniform = "";

    Texture(std::string uniform);
    virtual void bind();
	void bind(GLuint unit); // bind texture to given unit

	virtual ~Texture();

	Texture(const Texture&) = delete;
    Texture& operator=(const Texture&) = delete;

    Texture(Texture&& other) noexcept;
    Texture& operator=(Texture&& other) noexcept;
};

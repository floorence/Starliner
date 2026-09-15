#pragma once

#include "texture/Texture.h"
#include <glad/glad.h>
#include <string>
#include <vector>

class CubeMapTexture: public Texture {
public:
    CubeMapTexture(std::string uniform, uint width, uint height);
    CubeMapTexture(std::string uniform, std::vector<std::string> faces);

    void bind() override;
};

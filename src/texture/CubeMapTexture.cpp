#include "CubeMapTexture.h"
#include "util/Log.h"
#include <fmt/format.h>
#include <stb/stb_image.h>

CubeMapTexture::CubeMapTexture(std::string uniform, uint width, uint height)
    : Texture(uniform)
{
    bind();
    for (uint i = 0; i < 6; i++) {
        glTexImage2D(
            GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_DEPTH_COMPONENT, 
            width, height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr
        );
    }
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);  
}

CubeMapTexture::CubeMapTexture(std::string uniform, std::vector<std::string> faces)
    : Texture(uniform)
{
    bind();
    int widthImg, heightImg, numColCh;
    for (uint i = 0; i < faces.size(); i++) {
        unsigned char *bytes = stbi_load(faces[i].c_str(), &widthImg, &heightImg, &numColCh, 0);
        if (bytes) {
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 
                         0, GL_RGB, widthImg, heightImg, 0, GL_RGB, GL_UNSIGNED_BYTE, bytes
            );
            stbi_image_free(bytes);
        } else {
            Log::err("CubeMapTexture", fmt::format("Failed to load texture: {}", faces[i]));
            stbi_image_free(bytes);
        }
    }
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
}

void CubeMapTexture::bind() {
    glBindTexture(GL_TEXTURE_CUBE_MAP, ID);
}
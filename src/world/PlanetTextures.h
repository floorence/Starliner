#pragma once

#include "texture/ImageTexture.h"
#include <memory>

namespace PlanetTextures {
    inline std::unique_ptr<ImageTexture> diffuses[5];
    inline std::unique_ptr<ImageTexture> speculars[5];

    inline void init() {
        // planets ordered in increasing size in real life
        diffuses[0] = std::make_unique<ImageTexture>("assets/planet/mercury.jpg", TextureType::Diffuse);
        diffuses[1] = std::make_unique<ImageTexture>("assets/planet/mars.jpg", TextureType::Diffuse);
        diffuses[2] = std::make_unique<ImageTexture>("assets/planet/venus.jpg", TextureType::Diffuse);
        diffuses[3] = std::make_unique<ImageTexture>("assets/planet/neptune.jpg", TextureType::Diffuse);
        diffuses[4] = std::make_unique<ImageTexture>("assets/planet/saturn.jpg", TextureType::Diffuse);

        speculars[0] = std::make_unique<ImageTexture>("assets/planet/mercury.jpg", TextureType::Specular, GL_UNSIGNED_BYTE, true);
        speculars[1] = std::make_unique<ImageTexture>("assets/planet/mars.jpg", TextureType::Specular, GL_UNSIGNED_BYTE, true);
        speculars[2] = std::make_unique<ImageTexture>("assets/planet/venus.jpg", TextureType::Specular, GL_UNSIGNED_BYTE, true);
        speculars[3] = std::make_unique<ImageTexture>("assets/planet/neptune.jpg", TextureType::Specular, GL_UNSIGNED_BYTE, true);
        speculars[4] = std::make_unique<ImageTexture>("assets/planet/saturn.jpg", TextureType::Specular, GL_UNSIGNED_BYTE, true);
    }
}
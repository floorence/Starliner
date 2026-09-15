#pragma once

#include "texture/ImageTexture.h"
#include <memory>

namespace PlanetTextures {
    inline std::unique_ptr<ImageTexture> diffuses[5];
    inline std::unique_ptr<ImageTexture> speculars[5];

    inline void init(bool useFastTextures) {
        // planets ordered in increasing size in real life
        const char* textures[5];

        if (useFastTextures) {
            textures[0] = "assets/planet/mercury_fast.jpg";
            textures[1] = "assets/planet/mars_fast.jpg";
            textures[2] = "assets/planet/venus_fast.jpg";
            textures[3] = "assets/planet/neptune_fast.jpg";
            textures[4] = "assets/planet/saturn_fast.jpg";
        } else {
            textures[0] = "assets/planet/mercury.jpg";
            textures[1] = "assets/planet/mars.jpg";
            textures[2] = "assets/planet/venus.jpg";
            textures[3] = "assets/planet/neptune.jpg";
            textures[4] = "assets/planet/saturn.jpg";
        }

        for (int i = 0; i < 5; i++) {
            diffuses[i] = std::make_unique<ImageTexture>(textures[i], TextureType::Diffuse);
            speculars[i] = std::make_unique<ImageTexture>(textures[i], TextureType::Specular, GL_UNSIGNED_BYTE, true);
        }
    }
}
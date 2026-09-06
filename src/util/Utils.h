#pragma once

#include "glm/glm.hpp"
#include <random>

namespace Utils {
	static constexpr float R_COEFFICIENT = 0.2126f;
	static constexpr float G_COEFFICIENT = 0.7152f;
	static constexpr float B_COEFFICIENT = 0.0722f;

    static std::random_device rd; 
    static std::mt19937 gen(rd()); 

    /** @param r, g, b value of colour channel from 0-255 */
    float getBrightness(float r, float g, float b);
    float getBrightness(glm::vec3 color);

    void checkOpenGlErrors();

    void unbindVao();
    void unbindVbo();
    void unbindEbo();
    void unbindFbo();
    void unbindFboAndClear();
    void unbindPbo();

    glm::vec3 clampVectorLength(glm::vec3 v, float maxLength);
    glm::vec3 setVectorLength(glm::vec3 v, float desiredLength);

	float approach(float from, float to, float amount); // approach from -> to by amount
};

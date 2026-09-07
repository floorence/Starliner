#pragma once

#include <cmath>
#include <glm/ext/vector_float3.hpp>

// contains basic light data with no drawing capabilities.
struct Light {
public:
    glm::vec3 position, color;
    float range, linear, quadratic;

    Light() = default;
    Light(float range, glm::vec3 position, glm::vec3 color): position(position), color(color), range(range) {
        calculateAttenuationCoefficients(range);
    }
private:
    // these are parameters for a best fit power regression model on the values that work well for specified ranges,
    // courtesy of https://wiki.ogre3d.org/tiki-index.php?page=-Point+Light+Attenuation
    // thank you desmos for fitting the model: https://www.desmos.com/calculator/9homov5tyc
    static constexpr float LINEAR_COEFFICIENT = 4.88011;
    static constexpr float LINEAR_POWER = -1.02046;
    static constexpr float QUADRATIC_COEFFICIENT = 87.39333;
    static constexpr float QUADRATIC_POWER = -2.03568;
    /**
     * @param range the distance away from the light that it can (visibly) reach.
     *              note that most of the light falls in the first 20% of range.
     */
    void calculateAttenuationCoefficients(float range) {
        linear = LINEAR_COEFFICIENT * pow(range, LINEAR_POWER);
        quadratic = QUADRATIC_COEFFICIENT * pow(range, QUADRATIC_POWER);
    }
};
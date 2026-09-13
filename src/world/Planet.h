#pragma once

#include "3d/shape/Sphere.h"
#include "mass/Mass.h"
#include "world/random/LocalGen.h"

class Planet: public Sphere {
public:
    float orbitalSpeed; // radians/s
    Planet(LocalGen& gen, glm::vec3 position);

    void calculateOrbitalSpeed(float starDist, float starMass);
private:
    void generateTexture();
    void generateColorTint(LocalGen& gen);
};
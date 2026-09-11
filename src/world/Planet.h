#pragma once

#include "3d/shape/Sphere.h"
#include "mass/Mass.h"
#include "world/random/LocalGen.h"

class Planet: public Mass, public Sphere {
public:
    Planet(LocalGen& gen, glm::vec3 position);
private:
    void generateTexture();
    void generateColorTint(LocalGen& gen);
};
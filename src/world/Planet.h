#pragma once

#include "3d/shape/Sphere.h"
#include "mass/Mass.h"

class Planet: public Mass, public Sphere {
public:
    Planet(glm::vec3 position, int radius);
};
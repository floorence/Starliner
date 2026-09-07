#pragma once

#include "3d/shape/Line.h"
#include "3d/shape/Sphere.h"
#include "mass/Mass.h"

class Star: public Mass, public Sphere {
public:
    Star(glm::vec3 position, int radius);

    glm::vec3 getNorth();
    void setNorth(glm::vec3 north);
    void draw(Camera& camera) override;
private:
    glm::vec3 north;

    Line northLine;
};
#pragma once

#include "3d/shape/Line.h"
#include "3d/shape/Sphere.h"
#include "mass/Mass.h"
#include "world/Region.h"
#include "world/random/LocalGen.h"

class Star: public Mass, public Sphere {
public:
    Star(LocalGen& gen, Region region);

    glm::vec3 getNorth();
    void draw(Camera& camera) override;
private:
    glm::vec3 north;
    Line northLine;

    glm::vec3 generatePosition(LocalGen& gen, Region region);
    void generateNorth(LocalGen& gen);
    void generateColor(LocalGen& gen);
};
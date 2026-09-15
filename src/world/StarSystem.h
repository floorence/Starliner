#pragma once

#include "3d/Drawable3D.h"
#include "lighting/Light.h"
#include "world/Planet.h"
#include "world/Region.h"
#include "world/Star.h"
#include "world/random/LocalGen.h"
#include <glm/ext/vector_float3.hpp>

class StarSystem: public Drawable3D {
public:
    Light starLightData;

    StarSystem(uint localSeed, Region region);

    void update(float deltaTime);

    void draw(Camera& camera) override;
    void drawToDepthMap(PointLightCamera& camera, Shader& depthShader) override;
private:
    Star star;
    std::vector<Planet> planets;

    StarSystem(LocalGen&& gen, Region region);

    void generatePlanets(LocalGen& gen);
};
#pragma once

#include "3d/Drawable3D.h"
#include "lighting/LightController.h"
#include "world/Region.h"
#include "world/StarSystem.h"
#include <glm/ext/vector_float3.hpp>
#include <memory>
#include <vector>

class World: public Drawable3D {
public:
    static const int REGION_SIZE = 64;

    World(uint seed, LightController* lc);

    void onPlayerPosition(glm::vec3 pos);
    void update(float deltaTime);
    std::string getDebugString();

    void draw(Camera& camera) override;
    void drawToDepthMap(PointLightCamera& camera, Shader& depthShader) override;
private:
    uint seed;
    LightController* lc;
    glm::vec3 playerPos = glm::vec3(0.0f);
    Region playerRegion;

    std::vector<std::unique_ptr<StarSystem>> starSystems; // unique_ptr since Region stores pointer to corresponding StarSystem
    std::vector<Region> loaded;
    std::vector<StarSystem*> cachedCloseStarSystems;

    // radius of regions around player's current region
    const int simulationRadius = 2;

    void loadPlayerRegions();
    void loadIfNotLoaded(Region region);

    bool potentiallyHasStarSystem(Region region);
    double getPriority(Region region);
    bool actuallyHasStarSystem(Region region);
};
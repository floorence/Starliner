#pragma once

#include "3d/Drawable3D.h"
#include "lighting/LightController.h"
#include "world/Region.h"
#include "world/StarSystem.h"
#include <glm/ext/vector_float3.hpp>
#include <memory>
#include <vector>

/**
 * * split the world into cubic regions with side length REGION_SIZE
 * * regions will be tracked by the coordinate of the corner that is in the negative x, y, and z direction relative to the rest of the region, divided by REGION_SIZE.
 * * a region can have 1 star system or have nothing in it
 * * star systems can't be in regions right next to each other or diagonal to each other
 * * in valid regions, a star system can spawn 50% of the time
 * * only regions that have been loaded by the player will be considered. a player loads a region if they enter a region that is
 *   2 regions away from it, including diagonals. the region that was entered will notify all regions that are 2 regions away
 *   and tell them to load if not already. this number should be configurable
 * * if player is at 0,0,0 they will be considered inside the region tracked by 0,0,0. same logic applies to other regions 
 * * also only star systems within the loading distance of the player would simulate gravity and stuff.
 * * once a region has decided that it'll have a star system, the StarSystem class does the work of generating its planets and stuff
 */
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
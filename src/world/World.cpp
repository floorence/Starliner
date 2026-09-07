#include "World.h"
#include "lighting/Light.h"
#include "util/Log.h"
#include <algorithm>
#include <memory>
#include "GenRandom.h"

World::World(uint seed, LightController* lc): seed(seed), lc(lc) {
    playerRegion = {0, 0, 0};
    loadPlayerRegions();
}

void World::onPlayerPosition(glm::vec3 pos) {
    playerPos = pos;
    Region newRegion = {
        static_cast<int>(std::floor(pos.x / REGION_SIZE)),
        static_cast<int>(std::floor(pos.y / REGION_SIZE)), 
        static_cast<int>(std::floor(pos.z / REGION_SIZE))
    };

    if (newRegion == playerRegion) return;
    Log::log("World", fmt::format("player moved regions: {}, {}, {} -> {}, {}, {}", 
       playerRegion.x, playerRegion.y, playerRegion.z,
       newRegion.x, newRegion.y, newRegion.z));

    // only attempt loading the ones that could have not been loaded, so only check the ones that wouldn't have been
    // checked when player entered the previous region
    int dx = newRegion.x - playerRegion.x;
    int dy = newRegion.y - playerRegion.y;
    int dz = newRegion.z - playerRegion.z;

    if (dx > 1 || dy > 1 || dz > 1) {
        // player must have teleported, ensure all regions around them are loaded
        loadPlayerRegions();
        return;
    }

    int loadX = newRegion.x + simulationRadius * dx;
    int loadY = newRegion.y + simulationRadius * dy;
    int loadZ = newRegion.z + simulationRadius * dz;
    for (int a = -simulationRadius; a <= simulationRadius; a++) {
        for (int b = -simulationRadius; b <= simulationRadius; b++) {
            // player could possibly have moved diagonals if they crossed the borders in less than frame time
            if (dx != 0) {
                loadIfNotLoaded({loadX, newRegion.y + a, newRegion.z + b});
            }
            if (dy != 0) {
                loadIfNotLoaded({newRegion.x + a, loadY, newRegion.z + b});
            }
            if (dz != 0) {
                loadIfNotLoaded({newRegion.x + a, newRegion.y + b, loadZ});
            }
        }
    }

    playerRegion = newRegion;
}

void World::update(float deltaTime) {
    int r = simulationRadius;
    cachedCloseStarSystems.clear();
    std::vector<Light> closeStarsLightData;

    float closestStarDist2 = INT_MAX;
    int closestStarIndex = -1;
    
    for (int x = playerRegion.x - r; x <= playerRegion.x + r; x++) {
        for (int y = playerRegion.y - r; y <= playerRegion.y + r; y++) {
            for (int z = playerRegion.z - r; z <= playerRegion.z + r; z++) {
                Region r = {x, y, z};
                auto it = std::lower_bound(loaded.begin(), loaded.end(), r);

                if (it != loaded.end() && *it == r) {
                    int i = it - loaded.begin();
                    if (loaded[i].starSystem != nullptr) {
                        loaded[i].starSystem->update(deltaTime);
                        cachedCloseStarSystems.push_back(loaded[i].starSystem);

                        Light light = loaded[i].starSystem->starLightData;
                        closeStarsLightData.push_back(light);

                        float starDist2 = glm::length2(light.position - playerPos);
                        if (starDist2 < closestStarDist2) {
                            closestStarDist2 = starDist2;
                            closestStarIndex = closeStarsLightData.size() - 1;
                        }
                    }
                } else {
                    Log::warn("World", fmt::format("tried to update a region {} {} {} that wasn't loaded!", r.x, r.y, r.z));
                }
            }
        }
    }

    if (closestStarIndex != -1) {
        lc->setLights(closeStarsLightData, closestStarIndex);
    }
}

std::string World::getDebugString() {
	return fmt::format("position: {:.3f}, {:.3f}, {:.3f}\nregion: {}, {}, {}",
        playerPos.x, playerPos.y, playerPos.z,
		playerRegion.x, playerRegion.y, playerRegion.z
	);
}

void World::draw(Camera& camera) {
    for (auto& starSystem: cachedCloseStarSystems) {
        starSystem->draw(camera);
    }
}

void World::drawToDepthMap(PointLightCamera& camera, Shader& depthShader) {
    for (auto& starSystem: cachedCloseStarSystems) {
        starSystem->drawToDepthMap(camera, depthShader);
    }
}

void World::loadPlayerRegions() {
    int r = simulationRadius;
    for (int x = playerRegion.x - r; x <= playerRegion.x + r; x++) {
        for (int y = playerRegion.y - r; y <= playerRegion.y + r; y++) {
            for (int z = playerRegion.z - r; z <= playerRegion.z + r; z++) {
                loadIfNotLoaded({x, y, z});
            }
        }
    }
}

void World::loadIfNotLoaded(Region region) {
    Log::log("World", fmt::format("loadIfNotLoaded ({}, {}, {}), checking...", region.x, region.y, region.z));
    bool hasLoaded = std::binary_search(loaded.begin(), loaded.end(), region);
    if (hasLoaded) return;
    Log::log("World", fmt::format("loading..."));
    
    if (actuallyHasStarSystem(region)) {
        auto starSystem = std::make_unique<StarSystem>(GenRandom::getRegionSeed(seed, region), region);
        region.starSystem = starSystem.get();
        Log::log("World", fmt::format("star system at region {}, {}, {}, star pos: {}, {}, {}", region.x, region.y, region.z,
            starSystem->starLightData.position.x, starSystem->starLightData.position.y, starSystem->starLightData.position.z));
        starSystems.push_back(std::move(starSystem));
    }

    auto it = std::lower_bound(loaded.begin(), loaded.end(), region);
    loaded.insert(it, region);
}

bool World::potentiallyHasStarSystem(Region region) {
    return GenRandom::hash01(seed, region, 0) < 0.5;
}

double World::getPriority(Region region) {
    return GenRandom::hash01(seed, region, 1);
}

bool World::actuallyHasStarSystem(Region region) {
    int potentially = potentiallyHasStarSystem(region);
    if (!potentially) return false;

    double myPriority = getPriority(region);

    // star systems can't be in regions right next to each other including diagonal regions 
    // to avoid an infinite loop, a region yields to its neighbours if the neighbour has a higher priority.
    for (int x = region.x - 1; x <= region.x + 1; x++) {
        for (int y = region.y - 1; y <= region.y + 1; y++) {
            for (int z = region.z - 1; z <= region.z + 1; z++) {
                if (region.x == x && region.y == y && region.z == z) continue; // skip myself
                
                Region neighbor = {x, y, z};
                if (!potentiallyHasStarSystem(neighbor)) continue;
                if (getPriority(neighbor) > myPriority) return false;
            }
        }
    }
    
    return true;
}

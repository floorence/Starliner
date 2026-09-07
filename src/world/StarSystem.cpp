#include "StarSystem.h"
#include "util/Constants.h"
#include "util/Log.h"
#include "util/Utils.h"
#include "world/GenRandom.h"
#include "world/World.h"
using gen = GenRandom;

StarSystem::StarSystem(uint localSeed, Region region)
    : localGen(localSeed),
      star(generateStarPosition(region), gen::randomFloat(localGen, World::REGION_SIZE / 16.0f, World::REGION_SIZE / 8.0f))
{
    // TODO: star mass, colour, luminosity
    star.setColor(glm::vec3(
        gen::randomFloat(localGen, 75.0f, 100.0f),
        gen::randomFloat(localGen, 75.0f, 100.0f),
        gen::randomFloat(localGen, 75.0f, 100.0f)
    ));
    Log::log("StarSystem", fmt::format("star colour: {}, {}, {}", star.getColor().x, star.getColor().y, star.getColor().z));
    star.setNorth(generateStarNorth());
    starLightData = Light(World::REGION_SIZE / 2.0, star.getPosition(), star.getColor());

    generatePlanets();
}

void StarSystem::update(float deltaTime) {
    // TODO update each planet
}

void StarSystem::draw(Camera& camera) {
    star.draw(camera);
    for (auto& planet: planets) {
        planet.draw(camera);
    }
}

void StarSystem::drawToDepthMap(PointLightCamera& camera, Shader& depthShader) {
    // star is the light source
    for (auto& planet: planets) {
        planet.drawToDepthMap(camera, depthShader);
    }
}

glm::vec3 StarSystem::generateStarPosition(Region region) {
    int startX = region.x * World::REGION_SIZE;
    int startY = region.y * World::REGION_SIZE;
    int startZ = region.z * World::REGION_SIZE;
    
    int offsetX = gen::randomInt(localGen, 0, World::REGION_SIZE - 1);
    int offsetY = gen::randomInt(localGen, 0, World::REGION_SIZE - 1);
    int offsetZ = gen::randomInt(localGen, 0, World::REGION_SIZE - 1);

    return {startX + offsetX, startY + offsetY, startZ + offsetZ};
}

glm::vec3 StarSystem::generateStarNorth() {
    float x = gen::randomFloat(localGen, 0, 100);
    float y = gen::randomFloat(localGen, 0, 100);
    float z = gen::randomFloat(localGen, 0, 100);

    return glm::normalize(glm::vec3(x, y, z));
}

void StarSystem::generatePlanets() {
    glm::vec3 basePlanetVec = glm::cross(star.getNorth(), Constants::FORWARD);

    int numPlanets = 3; // random(0, 5);
    float minDist = World::REGION_SIZE / 16.0f, maxDist = World::REGION_SIZE / 8.0f;
    float currDist = star.radius;

    Log::log("StarSystem", fmt::format("numPlanets: {}", numPlanets));
    for (int i = 0; i < numPlanets; i++) {
        float distLeft = World::REGION_SIZE / 2.0f - currDist;
        float allowableDist = distLeft - minDist * (numPlanets - i - 1);
        Log::log("StarSystem", fmt::format("currDist: {}, distLeft: {}, allowableDist: {}", currDist, distLeft, allowableDist));
        float distance = gen::randomFloat(localGen, minDist, std::min(maxDist, allowableDist));
        currDist += distance;

        float radius = gen::randomFloat(localGen,World::REGION_SIZE / 64.0f, World::REGION_SIZE / 32.0f);

        // planets usually orbit star in the same plane and in the same direction; rotate each planet by a random amount around star north
        float rotation = gen::randomFloat(localGen, 1.0f, 360.0f);
        glm::vec3 starToPlanet = glm::rotate(basePlanetVec, glm::radians(static_cast<float>(rotation)), star.getNorth());
        starToPlanet = Utils::setVectorLength(starToPlanet, currDist);

        Planet planet(star.Mass::position + starToPlanet, radius);
        // TODO
        planet.setColor(glm::vec3(
            gen::randomFloat(localGen, 0.0f, 10.0f),
            gen::randomFloat(localGen, 0.0f, 10.0f),
            gen::randomFloat(localGen, 0.0f, 10.0f)
        ));
        planets.push_back(std::move(planet));
    }
}

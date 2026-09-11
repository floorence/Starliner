#include "StarSystem.h"
#include "util/Constants.h"
#include "util/Log.h"
#include "util/Utils.h"
#include "world/World.h"

StarSystem::StarSystem(uint localSeed, Region region): StarSystem(LocalGen(localSeed), region) {};

StarSystem::StarSystem(LocalGen&& gen, Region region): star(gen, region) {
    starLightData = Light(World::REGION_SIZE / 2.0, star.getPosition(), star.getColor());
    generatePlanets(gen);
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

void StarSystem::generatePlanets(LocalGen& gen) {
    glm::vec3 basePlanetVec = glm::cross(star.getNorth(), Constants::FORWARD);

    int numPlanets = gen.randomInt(0, 5);
    float minDist = World::REGION_SIZE / 16.0f, maxDist = World::REGION_SIZE / 8.0f;
    float currDist = star.radius;

    Log::log("StarSystem", fmt::format("numPlanets: {}", numPlanets));
    for (int i = 0; i < numPlanets; i++) {
        float distLeft = World::REGION_SIZE / 2.0f - currDist;
        float allowableDist = distLeft - minDist * (numPlanets - i - 1);
        Log::log("StarSystem", fmt::format("currDist: {}, distLeft: {}, allowableDist: {}", currDist, distLeft, allowableDist));
        float distance = gen.randomFloat(minDist, std::min(maxDist, allowableDist));
        currDist += distance;

        // planets usually orbit star in the same plane and in the same direction; rotate each planet by a random amount around star north
        float rotation = gen.randomFloat(1.0f, 360.0f);
        glm::vec3 starToPlanet = glm::rotate(basePlanetVec, glm::radians(static_cast<float>(rotation)), star.getNorth());
        starToPlanet = Utils::setVectorLength(starToPlanet, currDist);

        Planet planet(gen, star.Mass::position + starToPlanet);
        // TODO
        planets.push_back(std::move(planet));
    }
}

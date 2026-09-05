#include "StarSystem.h"
#include "util/Constants.h"
#include "util/Log.h"
#include "util/Utils.h"
#include "world/World.h"

StarSystem::StarSystem(uint localSeed, Region region)
    : localGen(localSeed),
      star(generateStarPosition(region), random(World::REGION_SIZE / 16, World::REGION_SIZE / 8))
{
    // TODO: star mass, colour, luminosity
    star.setColor(glm::vec3(100.0f, 100.0f, 100.0f));
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
    
    int offsetX = random(0, World::REGION_SIZE - 1);
    int offsetY = random(0, World::REGION_SIZE - 1);
    int offsetZ = random(0, World::REGION_SIZE - 1);

    return {startX + offsetX, startY + offsetY, startZ + offsetZ};
}

glm::vec3 StarSystem::generateStarNorth() {
    int x = random(0, 100);
    int y = random(0, 100);
    int z = random(0, 100);

    return glm::normalize(glm::vec3(x, y, z));
}

void StarSystem::generatePlanets() {
    // planets usually orbit star in the same plane and in the same direction
    // make vector that's rotated a random amount up and down and a random amount around the star
    // int rotXDegrees = random(0, 90);
    // int rotYDegrees = random(0, 180);
    // glm::vec3 planeVec = glm::rotate(Constants::FORWARD, glm::radians((float) rotXDegrees), Constants::RIGHT);
    // planeVec = glm::rotate(planeVec, glm::radians((float) rotYDegrees), Constants::UP);

    glm::vec3 basePlanetVec = glm::cross(star.getNorth(), Constants::FORWARD);

    int numPlanets = 3; // random(0, 5);
    int minDist = World::REGION_SIZE / 16, maxDist = World::REGION_SIZE / 8;
    int currDist = star.radius;

    Log::log("StarSystem", fmt::format("numPlanets: {}", numPlanets));
    for (int i = 0; i < numPlanets; i++) {
        int distLeft = World::REGION_SIZE / 2 - currDist;
        int allowableDist = distLeft - minDist * (numPlanets - i - 1);
        Log::log("StarSystem", fmt::format("currDist: {}, distLeft: {}, allowableDist: {}", currDist, distLeft, allowableDist));
        int distance = random(minDist, std::min(maxDist, allowableDist));
        currDist += distance;

        int radius = random(World::REGION_SIZE / 64, World::REGION_SIZE / 32);
        // rotate each planet by another random amount around star
        int rotation = random(1, 360);
        glm::vec3 starToPlanet = glm::rotate(basePlanetVec, glm::radians(static_cast<float>(rotation)), star.getNorth());
        starToPlanet = Utils::setVectorLength(starToPlanet, currDist);

        Planet planet(star.Mass::position + starToPlanet, radius);
        planet.setColor(glm::vec3(5.76f, 8.71f, 1.33f)); // TODO
        planets.push_back(std::move(planet));
    }
}

int StarSystem::random(int min, int max) {
    std::uniform_int_distribution<int> dist(min, max);
    return dist(localGen);
}
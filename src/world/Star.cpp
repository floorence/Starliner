#include "Star.h"
#include "util/Utils.h"
#include "world/World.h"

Star::Star(LocalGen& gen, Region region):
    Sphere(
        generatePosition(gen, region), 
        gen.randomFloat(World::REGION_SIZE / 16.0f, World::REGION_SIZE / 8.0f), 
        32, 64, true
    ) 
{
    this->Mass::position = this->Object3D::position;
    northLine.setColor(glm::vec3(100.0f, 0.0f, 69.0f));

    generateNorth(gen);
    generateColor(gen);
}

glm::vec3 Star::getNorth() {
    return north;
}

glm::vec3 Star::generatePosition(LocalGen& gen, Region region) {
    int startX = region.x * World::REGION_SIZE;
    int startY = region.y * World::REGION_SIZE;
    int startZ = region.z * World::REGION_SIZE;
    
    int offsetX = gen.randomInt(0, World::REGION_SIZE - 1);
    int offsetY = gen.randomInt(0, World::REGION_SIZE - 1);
    int offsetZ = gen.randomInt(0, World::REGION_SIZE - 1);

    return {startX + offsetX, startY + offsetY, startZ + offsetZ};
}

void Star::generateNorth(LocalGen& gen) {
    float x = gen.randomFloat(0, 100);
    float y = gen.randomFloat(0, 100);
    float z = gen.randomFloat(0, 100);

    north = glm::normalize(glm::vec3(x, y, z));
    northLine.setCoordinates(
        this->Object3D::position,
        this->Object3D::position + Utils::setVectorLength(north, radius + 4)
    );
}

void Star::generateColor(LocalGen& gen) {
    setColor(glm::vec3(
        gen.randomFloat(75.0f, 100.0f),
        gen.randomFloat(75.0f, 100.0f),
        gen.randomFloat(75.0f, 100.0f)
    ));
}

void Star::draw(Camera& camera) {
    Shape3D::draw(camera);
    northLine.draw(camera);
}
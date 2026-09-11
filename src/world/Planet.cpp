#include "Planet.h"
#include "world/PlanetTextures.h"
#include "world/World.h"

Planet::Planet(LocalGen& gen, glm::vec3 position)
    : Sphere(position, gen.randomFloat(World::REGION_SIZE / 64.0f, World::REGION_SIZE / 32.0f))
{
    this->Mass::position = position;
    generateTexture();
    generateColorTint(gen);
}

void Planet::generateTexture() {
    int size = (radius - World::REGION_SIZE / 64.0f) * 5; // map radius to value [0, 4]
    setTextures(PlanetTextures::diffuses[size].get(), PlanetTextures::speculars[size].get());
}

void Planet::generateColorTint(LocalGen& gen) {
    tintColor = glm::vec4(
        gen.randomFloat(0.0f, 10.0f),
        gen.randomFloat(0.0f, 10.0f),
        gen.randomFloat(0.0f, 10.0f),
        0.5f
    );
}
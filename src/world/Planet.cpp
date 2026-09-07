#include "Planet.h"

Planet::Planet(glm::vec3 position, int radius)
    : Sphere(position, radius)
{
    this->Mass::position = position;
}
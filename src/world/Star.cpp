#include "Star.h"
#include "util/Utils.h"

Star::Star(glm::vec3 position, int radius)
    : Sphere(position, radius, 32, 64, true) 
{
    this->Mass::position = position;
    northLine.setColor(glm::vec3(100.0f, 0.0f, 69.0f));
}

glm::vec3 Star::getNorth() {
    return north;
}

void Star::setNorth(glm::vec3 north) {
    this->north = north;
    northLine.setCoordinates(
        this->Object3D::position,
        this->Object3D::position + Utils::setVectorLength(north, radius + 4)
    );
}

void Star::draw(Camera& camera) {
    Shape3D::draw(camera);
    northLine.draw(camera);
}
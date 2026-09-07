#pragma once

#include "3d/shape/Shape3D.h"

class Sphere: public Shape3D {
public:
    float radius;

    Sphere(glm::vec3 position, float radius, int stacks = 16, int sectors = 32, bool isLightSource = false);
    Sphere(AssetTexture* diffuse, AssetTexture* specular, glm::vec3 position, float radius, int stacks = 16, int sectors = 32, bool isLightSource = false);
private:
    int stacks, sectors;

    std::vector<Vertex> generateVertices() override;
    std::vector<GLuint> generateIndices() override;
};

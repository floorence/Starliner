#include "Sphere.h"
#include <glm/gtc/constants.hpp>

Sphere::Sphere(glm::vec3 position, float radius, int stacks, int sectors, bool isLightSource)
    : Shape3D(position, isLightSource),
      radius(radius), stacks(stacks), sectors(sectors)
{
    invalidateShape();
}

Sphere::Sphere(AssetTexture* diffuse, AssetTexture* specular, glm::vec3 position, float radius, int stacks, int sectors, bool isLightSource) 
    : Shape3D(diffuse, specular, position, isLightSource),
      radius(radius), stacks(stacks), sectors(sectors)
{
    invalidateShape();
}

// adapted from https://www.songho.ca/opengl/gl_sphere.html#example_sphere
//              Sphere::buildVerticesSmooth() (first half)
std::vector<Vertex> Sphere::generateVertices() {
    std::vector<Vertex> vertices;
    const float PI = glm::pi<float>();

    float sectorStep = 2 * PI / sectors;
    float stackStep = PI / stacks;

    for (int i = 0; i <= stacks; i++) {
        float stackAngle = PI / 2 - i * stackStep; // pi/2 to -pi/2 (instead of -pi/2 to pi/2 since tex coords start at the top)
        float xz = radius * cosf(stackAngle); // r * cos(u) -> horizontal distance from middle of sphere
        float y = radius * sinf(stackAngle); // r * sin(u) -> vertical distance from middle of sphere

        // add (sectors+1) vertices per stack
        // the first and last vertices have same position and normal, but different tex coords
        for (int j = 0; j <= sectors; j++) {
            float sectorAngle = j * sectorStep; // 0 to 2pi

            // vertex position
            float x = xz * cosf(sectorAngle); // r * cos(u) * cos(v)  
            float z = xz * sinf(sectorAngle); // r * cos(u) * sin(v)

            // vertex tex coord between [0, 1]
            float u = (float)j / sectors;
            float v = (float)i / stacks;

            Vertex vertex = Vertex {
                glm::vec3(x, y, z),
                glm::vec3(x, y, z), // normals same as position for smooth lighting
                glm::vec2(u, v)
            };

            vertices.push_back(vertex);
        }
    }
    return vertices;
}

// adapted from https://www.songho.ca/opengl/gl_sphere.html#example_sphere
//              Sphere::buildVerticesSmooth() (second half)
std::vector<GLuint> Sphere::generateIndices() {
    std::vector<GLuint> indices;
    //  k1--k1+1
    //  |  / |
    //  | /  |
    //  k2--k2+1
    for (int i = 0; i < stacks; i++) {
        unsigned int k1 = i * (sectors + 1); // beginning of current stack
        unsigned int k2 = k1 + sectors + 1; // beginning of next stack

        for (int j = 0; j < sectors; j++, k1++, k2++) {
            // 2 triangles per sector excluding 1st and last stacks
            // TODO: figure out why for sphere front facing is drawn clockwisely here... but for other shapes its counterclockwise
            if (i != 0) {
                indices.insert(indices.end(), {k1, k1 + 1, k2});
            }

            if (i != (stacks - 1)) {
                indices.insert(indices.end(), {k1 + 1, k2 + 1, k2});
            }
        }
    }
    return indices;
}

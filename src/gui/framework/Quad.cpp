#include "Quad.h"
#include "mesh/VBO.h"
#include "mesh/Mesh.h"

Quad::Quad(Texture* texture) {
    setTexture(texture);
}

Quad::Quad(float x, float y, float w, float h, bool corners): Rect(x, y, w, h, corners) {}

void Quad::setColor(glm::vec3 color) {
    mesh.setColor(color);
}

void Quad::setColor(glm::vec4 color) {
    mesh.setColor(color);
}

void Quad::setTexture(Texture* texture) {
    mesh.setTextures({texture});
}

void Quad::setTextures(std::vector<Texture*> textures) {
    mesh.setTextures(textures);
}

void Quad::onBoundsChanged() {
    float xv = x + w;
    float yv = y + h;

    std::vector<Vertex> vertices = {
        Vertex {glm::vec3(x, y, 0.0f), glm::vec3(0.0f), glm::vec2(0.0f, 1.0f)},
        Vertex {glm::vec3(xv, y, 0.0f), glm::vec3(0.0f), glm::vec2(1.0f, 1.0f)},
        Vertex {glm::vec3(x, yv, 0.0f), glm::vec3(0.0f), glm::vec2(0.0f, 0.0f)},
        Vertex {glm::vec3(xv, yv, 0.0f), glm::vec3(0.0f), glm::vec2(1.0f, 0.0f)}
    };
    std::vector<GLuint> indices = {0, 2, 3, 0, 3, 1};

    mesh.setShapeData(vertices, indices);
}

void Quad::draw(Shader& shader) {
    shader.setColorTint(tintColor);
    mesh.drawGui(shader);
}
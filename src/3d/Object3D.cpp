#include "Object3D.h"
#include "util/Log.h"

Object3D::Object3D(glm::vec3 position) {
    setPosition(position);
}

std::string getDebugString();

glm::vec3 Object3D::getPosition() {
    return position;
}

void Object3D::setPosition(glm::vec3 position) {
    this->position = position;
    invalidateModel();
}

void Object3D::rotate(float angle, glm::vec3 axis) {
    axis = glm::normalize(axis);
    rotation = glm::rotate(rotation, angle, axis);
    invalidateModel();
}

void Object3D::setRotation(float rotationX, float rotationY, float rotationZ) {
    rotation = rotate(defaultRotation, rotationX, rotationY, rotationZ);
    invalidateModel();
}

void Object3D::setRotation(float angle, glm::vec3 axis) {
    axis = glm::normalize(axis);
    rotation = glm::rotate(defaultRotation, angle, axis);
    invalidateModel();
}

void Object3D::setDefaultRotation(float rotationX, float rotationY, float rotationZ) {
    defaultRotation = rotate(glm::mat4(1.0f), rotationX, rotationY, rotationZ);
    rotation = defaultRotation;
    invalidateModel();
}

void Object3D::setScale(float scale) {
    setScale(glm::vec3(scale));
}

void Object3D::setScale(glm::vec3 scale) {
    this->scale = scale;
    invalidateModel();
}

void Object3D::preDraw(Camera& camera) {
    Drawable3D::preDraw(camera);
    shader->setModel(model);
    shader->setRotation(rotation); // shader still has to rotate normals
}

void Object3D::preDrawToDepthMap(Shader& depthShader) {
    Drawable3D::preDrawToDepthMap(depthShader);
    depthShader.setModel(model);
}

void Object3D::invalidateModel() {
    // right to left: scale -> rotate -> translate since matrix multiplication is weird like that
    model = glm::translate(glm::mat4(1.0f), position);
    model *= rotation;
    model = glm::scale(model, scale);
}

glm::mat4 Object3D::rotate(glm::mat4 mat, float rotationX, float rotationY, float rotationZ) {
    mat = glm::rotate(mat, rotationX, glm::vec3(1.0f, 0.0f, 0.0f));
    mat = glm::rotate(mat, rotationY, glm::vec3(0.0f, 1.0f, 0.0f));
    mat = glm::rotate(mat, rotationZ, glm::vec3(0.0f, 0.0f, 1.0f));
    return mat;
}
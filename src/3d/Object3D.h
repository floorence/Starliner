#pragma once

#include <glm/ext/matrix_float4x4.hpp>
#include "3d/Drawable3D.h"

class Object3D: public Drawable3D {
public:
    Object3D() = default;
    Object3D(glm::vec3 position);

    virtual ~Object3D() = default;

    // need to explicitly declare because of the explicitly declared destructor, which causes the compiler to not generate these
    Object3D(const Object3D&) = delete;
    Object3D& operator=(const Object3D&) = delete;

    Object3D(Object3D&&) noexcept = default;
    Object3D& operator=(Object3D&&) noexcept = default;

    glm::vec3 getPosition();
    void setPosition(glm::vec3 position);
    void rotate(float angle, glm::vec3 axis);
    /**
     * @param rotationX rotation around x axis in radians
     * @param rotationY rotation around y axis in radians
     * @param rotationZ rotation around z axis in radians
     */
    void setRotation(float rotationX, float rotationY, float rotationZ);
    /**
     * @param angle angle of rotation in degrees, counterclockwise around a positive axis
     * @param axis axis of rotation
     */
    void setRotation(float angle, glm::vec3 axis);
    /**
     * @brief set the rotation that should always be applied on top of setRotation() 
     * should call this before any setRotation calls, since this will reset current rotation to the default
     */
    void setDefaultRotation(float rotationX, float rotationY, float rotationZ);
    /**
     * @param scale scale to apply to x, y, and z axes
     */
    void setScale(float scale);
    /**
     * @param scale scale vector whose components represent scale in x, y, and z axes
     */
    void setScale(glm::vec3 scale);
protected:
    glm::mat4 model = glm::mat4(1.0f); // shader uses model to place vertices around correct location in the world
    glm::vec3 position = glm::vec3(0.0f);
    glm::mat4 rotation = glm::mat4(1.0f);
    glm::vec3 scale = glm::vec3(1.0f);

    glm::mat4 defaultRotation = glm::mat4(1.0f);

    virtual void preDraw() override;
    virtual void preDrawToDepthMap(Shader& depthShader) override;
    /**
     * resets model with current position, rotation, and scale.
     */
    virtual void invalidateModel();
    glm::mat4 rotate(glm::mat4 mat, float rotationX, float rotationY, float rotationZ);
};

#pragma once

#include "mesh/Mesh.h"
#include "3d/Object3D.h"
#include "texture/AssetTexture.h"

class Shape3D: public Object3D {
public:
    bool isLightSource = false;

    // if !isLightSource, will be the tint colour of the shape. else, will not be used
    // higher alpha value will lead to stronger tint
    glm::vec4 tintColor = glm::vec4(0.0f);  

    Shape3D() = default;
    Shape3D(glm::vec3 position, bool isLightSource);
    Shape3D(AssetTexture* diffuse, AssetTexture* specular, glm::vec3 position, bool isLightSource);
    virtual ~Shape3D() = default;

    // need to explicitly declare because of the explicitly declared destructor, which causes the compiler to not generate these
    Shape3D(const Shape3D&) = delete;
    Shape3D& operator=(const Shape3D&) = delete;

    Shape3D(Shape3D&&) noexcept = default;
    Shape3D& operator=(Shape3D&&) noexcept = default;

    glm::vec3 getColor();
    void setColor(glm::vec3 color);
    void setTextures(AssetTexture* diffuse, AssetTexture* specular);
    
    virtual void draw(Camera& camera) override;
    void drawToDepthMap(PointLightCamera& camera, Shader& depthShader) override;
protected:
    Mesh mesh;
 
    virtual void preDraw() override;
    /**
     * call this after updating subclass fields used by generateVertices/Indices
     */
    void invalidateShape();
private:

    virtual std::vector<Vertex> generateVertices() = 0;
    virtual std::vector<GLuint> generateIndices() = 0;
};

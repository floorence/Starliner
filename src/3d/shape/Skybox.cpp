#include "Skybox.h"
#include "3d/shape/RectangularPrism.h"

Skybox::Skybox(): 
    RectangularPrism(glm::vec3(0.0f), 1.0f, 1.0f, 1.0f),
    texture("skybox", {
    "assets/skybox/right.png", 
    "assets/skybox/left.png", 
    "assets/skybox/up.png", 
    "assets/skybox/down.png", 
    "assets/skybox/front.png", 
    "assets/skybox/back.png"
    }),
    skyboxShader("shader/skybox.vert", "shader/skybox.frag")
{
    shader = &skyboxShader;
    cullFacesBeforeDraw = false; // camera is inside the cube
}

void Skybox::drawToDepthMap(PointLightCamera&, Shader&) {
    // do not draw to depth map since otherwise the actual cube will cast shadows
}

void Skybox::preDraw(Camera& camera) {
    Object3D::preDraw(camera);
    glm::mat4 view = glm::mat4(glm::mat3(camera.view)); // remove translation from the view matrix
    shader->setView(view);
    shader->setProjection(camera.projection);
    glDepthMask(GL_FALSE);
}

void Skybox::draw(Camera& camera) {
    preDraw(camera);
    mesh.drawGui(*shader); // we already took care of the camera since we had to remove translation
    glDepthMask(GL_TRUE);
}
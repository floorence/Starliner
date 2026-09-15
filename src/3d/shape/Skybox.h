#pragma once

#include "3d/shape/RectangularPrism.h"
#include "shader/Shader.h"
#include "texture/CubeMapTexture.h"

class Skybox: public RectangularPrism {
public:
    Skybox();

    void draw(Camera& camera) override;
    void drawToDepthMap(PointLightCamera& camera, Shader& depthShader) override;
private:
    CubeMapTexture texture;
    Shader skyboxShader;

    void preDraw(Camera& camera) override;
};
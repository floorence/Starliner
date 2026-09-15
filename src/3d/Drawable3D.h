#pragma once

#include "camera/Camera.h"
#include "shader/Shader.h"
#include "util/Globals.h"

class Drawable3D {
public:
    bool cullFacesBeforeDraw = true;
    Shader* shader = Globals::DefaultShader;

    Drawable3D() = default;

    virtual ~Drawable3D() = default;

    virtual void draw(Camera& camera) = 0;
    virtual void drawToDepthMap(PointLightCamera& camera, Shader& depthShader) = 0;
protected:
    virtual void preDraw(Camera&) {
        if (cullFacesBeforeDraw) glEnable(GL_CULL_FACE); else glDisable(GL_CULL_FACE);
    }
    virtual void preDrawToDepthMap(Shader&) {
        if (cullFacesBeforeDraw) glEnable(GL_CULL_FACE); else glDisable(GL_CULL_FACE);
    }
};

#pragma once

#include "lighting/FBO.h"
#include "lighting/Light.h"
#include "settings/SettingsListener.h"
#include "gui/framework/Quad.h"
#include "3d/Drawable3D.h"

enum class ShadowQuality {
	Off, Low, High
};

class LightController: public SettingsListener {
public:
    LightController(int fbWidth, int fbHeight);

    // for drawable light sources, must call setLights, which will store the light data to upload to shaders,
    // and registerDrawable which will store the light to actually be drawn.

    /**
     * @param primary index of the primary light source in lights
     */
    void setLights(std::vector<Light> lights, int primary);
    void registerDrawable(Drawable3D* drawable);
    void registerDrawables(const std::vector<Drawable3D*>& drawables);

    void render(Camera& camera, float deltaTime);

    void onSettingsChanged(const Settings& settings) override;
    void onFrameBufferSizeChanged(int newWidth, int newHeight);
    std::string getDebugString();
private:
    int fbWidth, fbHeight; // framebuffer width and height
    std::vector<Light> lights;
    std::vector<Drawable3D*> drawables;

    const std::string TAG = "LightController";

    // stuff for shadows
    bool shadowsEnabled = true;
    int primaryLightSourceIndex = 0; // the light source that casts shadows and affects sky colour
    const uint DEPTH_MAP_SIZE = 2048;
    FBO depthMapFbo;
    CubeMapTexture depthMapTexture;
    Shader depthShader;
    PointLightCamera pointLightCam;

    // stuff for hdr and bloom
    FBO hdrBloomFbo;
    Texture hdrTexture;
    Texture bloomTexture;
    GLuint rboID;
    Shader hdrBloomShader;
    Quad hdrBloomResult;

    // auto exposure adjustment
    FBO exposureFbos[2];
    Texture luminanceTexture;
    Texture exposureTextures[2]; // one previous and one current
    Shader exposureShader;
    Quad exposureResult;
    bool exposureIndex = false;

    // gaussian blur
    FBO blurFbos[2];
    Texture blurTextures[2];
    Shader blurShader;
    Quad blurResult;
    int blurAmount = 10;

    // window resizing
    Texture* windowSizeTextures[5] = {&hdrTexture, &bloomTexture, &luminanceTexture, &blurTextures[0], &blurTextures[1]};
    FBO* windowSizeFbos[3] = {&hdrBloomFbo, &blurFbos[0], &blurFbos[1]};

    // debug vars
    float debugBrightness;
    float debugExposure;

    void prepareDepthMap();
    void prepareHdrAndBloom();
    void prepareAutoExposure();
    void prepareGaussianBlur();
    void prepareFPTexture(Texture& texture, bool mipmap); // FP = floating point

    void processLighting();
    void renderForShadows();
    void renderForHDRAndBloom(Camera& camera);
    void adjustBrightness(float deltaTime);
    void blurBrightAreas();
    void renderForReal();

    glm::vec3 getSkyColor(Camera& camera);
};

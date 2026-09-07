#include "LightController.h"
#include "camera/Camera.h"
#include "camera/PointLightCamera.h"
#include "texture/CubeMapTexture.h"
#include "util/Globals.h"
#include "util/Log.h"
#include "util/Utils.h"

LightController::LightController(int fbWidth, int fbHeight) 
    : fbWidth(fbWidth), fbHeight(fbHeight),
      depthMapTexture("depthMap", DEPTH_MAP_SIZE, DEPTH_MAP_SIZE),
      depthShader("shader/depth.vert", "shader/depth.geom", "shader/depth.frag"),
      pointLightCam(glm::vec3(0.0f), DEPTH_MAP_SIZE, DEPTH_MAP_SIZE),
      hdrTexture("hdrBuffer"),
      bloomTexture("bloomBlur"),
      hdrBloomShader("shader/gui.vert", "shader/hdr_bloom.frag"),
      luminanceTexture("logLuminance"),
      exposureTextures{{"previousExposure"}, {"previousExposure"}},
      exposureShader("shader/gui.vert", "shader/exposure.frag"),
      blurTextures{{"bloomBlur"}, {"bloomBlur"}},
      blurShader("shader/gui.vert", "shader/blur.frag")
{
    prepareDepthMap();
    prepareHdrAndBloom();
    prepareAutoExposure();
    prepareGaussianBlur();
    Utils::checkOpenGlErrors();
}

void LightController::setLights(std::vector<Light> lights, int primary) {
    this->lights = lights;
    primaryLightSourceIndex = primary;
}

void LightController::registerDrawable(Drawable3D* drawable) {
    drawables.push_back(drawable);
}

void LightController::registerDrawables(const std::vector<Drawable3D*>& drawables) {
    this->drawables.insert(this->drawables.end(), drawables.begin(), drawables.end());
}

void LightController::processLighting() {
    int numPointLights = 0;

    for (const auto& light: lights) {
        Globals::DefaultShader->registerLightSource(
            numPointLights,
            light.color,
            light.position,
            light.linear, light.quadratic
        );
        numPointLights++;
    }

    Globals::DefaultShader->setNumPointLights(numPointLights);
}

void LightController::render(Camera& camera, float deltaTime) {
    processLighting();
    if (shadowsEnabled) renderForShadows();
    renderForHDRAndBloom(camera);
    adjustBrightness(deltaTime);
    blurBrightAreas();
    renderForReal();
}

void LightController::renderForShadows() {
    depthMapFbo.bindAndClear();
    glViewport(0, 0, DEPTH_MAP_SIZE, DEPTH_MAP_SIZE);

    pointLightCam.position = lights[primaryLightSourceIndex].position;
    pointLightCam.setPerspective(90.0f, 0.1f, lights[primaryLightSourceIndex].range);
    pointLightCam.generateTransforms();
    for (const auto& drawable : drawables) {
        drawable->drawToDepthMap(pointLightCam, depthShader);
    }

    Utils::unbindFboAndClear();

    Globals::DefaultShader->setTexture(depthMapTexture, 5);
    Globals::DefaultShader->setFarPlane(pointLightCam.farPlane);

    glViewport(0, 0, fbWidth, fbHeight);
}

void LightController::renderForHDRAndBloom(Camera& camera) {
    hdrBloomFbo.bind();

    glm::vec3 skyColor = getSkyColor(camera);
    const float background[] = {skyColor.r, skyColor.g, skyColor.b, 1.0f};
    const float black[] = {0.0f, 0.0f, 0.0f, 0.0f};

    glClearBufferfv(GL_COLOR, 0, background); // hdr attachment
    glClearBufferfv(GL_COLOR, 1, black); // bloom attachment
    glClear(GL_DEPTH_BUFFER_BIT);

    for (const auto& drawable: drawables) {
        drawable->draw(camera);
    }

    Utils::unbindFboAndClear();
}

void LightController::adjustBrightness(float deltaTime) {
    luminanceTexture.bind();
    glGenerateMipmap(GL_TEXTURE_2D);

    exposureFbos[exposureIndex].bind();
    glViewport(0, 0, 1, 1);

    // draw with the other exposure index's texture to produce current exposure index's texture
    exposureShader.setDeltaTime(deltaTime);
    exposureTextures[!exposureIndex].uniform = "previousExposure";
    exposureResult.setTextures({&luminanceTexture, &exposureTextures[!exposureIndex]});
    exposureResult.draw(exposureShader);

    exposureIndex = !exposureIndex;
    Utils::unbindFbo();
    glViewport(0, 0, fbWidth, fbHeight);
}

void LightController::blurBrightAreas() {
    bool horizontal = true;
    for (int i = 0; i < blurAmount; i++) {
        blurFbos[horizontal].bind();
        blurShader.setBlurHorizontal(horizontal);

        Texture* texture = (i == 0) ? &bloomTexture : &blurTextures[!horizontal];
        texture->bind();
        glGenerateMipmap(GL_TEXTURE_2D);
        blurResult.setTexture(texture);
        blurResult.draw(blurShader);
        horizontal = !horizontal;
    }
    Utils::unbindFboAndClear();
}

void LightController::renderForReal() {
    exposureTextures[!exposureIndex].uniform = "exposureTex";
    // since blurAmount is always even we know blurTextures[0] was the last one drawn in blurBrightAreas
    hdrBloomResult.setTextures({&hdrTexture, &blurTextures[0], &exposureTextures[!exposureIndex]});
    hdrBloomResult.draw(hdrBloomShader);
}

void LightController::onFrameBufferSizeChanged(int newWidth, int newHeight) {
    fbWidth = newWidth;
    fbHeight = newHeight;
    // view port is updated in renderForShadows

    for (auto& texture: windowSizeTextures) {
        texture->bind();
        glTexImage2D(
            GL_TEXTURE_2D, 0, GL_RGBA16F, fbWidth, fbHeight, 0, GL_RGBA, GL_FLOAT, nullptr
        );
    }
    glBindRenderbuffer(GL_RENDERBUFFER, rboID);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, fbWidth, fbHeight);

    for (auto& fbo: windowSizeFbos) {
        fbo->bind();
        fbo->checkStatus();
    }
    Utils::unbindFbo();
}

void LightController::onSettingsChanged(const Settings& settings) {
    blurAmount = settings.graphics.bloomAmount.value * 10;
    hdrBloomShader.setBloomEnabled(blurAmount);
    int shadowQuality = settings.graphics.shadowQuality.value;
    shadowsEnabled = shadowQuality != static_cast<int>(ShadowQuality::Off);
    Globals::DefaultShader->setShadowQuality(shadowQuality);
}

std::string LightController::getDebugString() {
    return fmt::format("average brightness: {:.3f}\nexposure: {:.3f}", debugBrightness, debugExposure);
}
    
void LightController::prepareDepthMap() {
    depthMapFbo.bind();
    depthMapFbo.attachTextureCube(depthMapTexture.ID);
    depthMapFbo.checkStatus();
    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);

    Utils::unbindFbo();
}

void LightController::prepareHdrAndBloom() {
    // create floating point color buffer
    prepareFPTexture(hdrTexture, false);
    prepareFPTexture(bloomTexture, true);
    prepareFPTexture(luminanceTexture, true);
    // create depth buffer (renderbuffer) THIS IS NEEDED TO RESOLVE DEPTHS!!! (texture only does colours)
    glGenRenderbuffers(1, &rboID);
    glBindRenderbuffer(GL_RENDERBUFFER, rboID);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, fbWidth, fbHeight);
    // attach textures and buffer
    hdrBloomFbo.bind();
    hdrBloomFbo.attachTexture2D(hdrTexture.ID, 0);
    hdrBloomFbo.attachTexture2D(bloomTexture.ID, 1);
    hdrBloomFbo.attachTexture2D(luminanceTexture.ID, 2);
    hdrBloomFbo.attachRenderBuffer(rboID);
    // configure fbo for 3 colour attachments
    unsigned int attachments[3] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2 };
    glDrawBuffers(3, attachments); 
    hdrBloomFbo.checkStatus();
    Utils::unbindFbo();

    hdrBloomShader.setProjection(glm::mat4(1.0f));
    hdrBloomResult.disableDimensionsProcessing = true;
    hdrBloomResult.setCorners(-1.0f, 1.0f, 1.0f, -1.0f);
}

void LightController::prepareAutoExposure() {
    float initialExposure = 1.0f;
    for (uint i = 0; i < 2; i++) {
        exposureTextures[i].bind();
        glTexImage2D(
            GL_TEXTURE_2D, 0, GL_R16F, 1, 1, 0, GL_RED, GL_FLOAT, &initialExposure
        );
        exposureFbos[i].bind();
        exposureFbos[i].attachTexture2D(exposureTextures[i].ID);
        exposureFbos[i].checkStatus();
    }
    exposureShader.setProjection(glm::mat4(1.0f));
    exposureResult.disableDimensionsProcessing = true;
    exposureResult.setCorners(-1.0f, 1.0f, 1.0f, -1.0f);
}

void LightController::prepareGaussianBlur() {
    for (uint i = 0; i < 2; i++) {
        prepareFPTexture(blurTextures[i], true);
        blurFbos[i].bind();
        blurFbos[i].attachTexture2D(blurTextures[i].ID);
        blurFbos[i].checkStatus();
    }

    blurShader.setProjection(glm::mat4(1.0f));
    blurResult.disableDimensionsProcessing = true;
    blurResult.setCorners(-1.0f, 1.0f, 1.0f, -1.0f);
}

void LightController::prepareFPTexture(Texture& texture, bool mipmap) {
    texture.bind();
    glTexImage2D(
        GL_TEXTURE_2D, 0, GL_RGBA16F, fbWidth, fbHeight, 0, GL_RGBA, GL_FLOAT, nullptr
    );
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, mipmap ? GL_LINEAR_MIPMAP_LINEAR : GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
}

glm::vec3 LightController::getSkyColor(Camera& camera) {
    Light light = lights[primaryLightSourceIndex];
    float dist = glm::length(camera.position - light.position);
    glm::vec3 c = light.position;
    glm::vec3 mappedLightColor = c / std::max(std::max(c.r, c.g), c.b);
    return mappedLightColor / std::max((dist * dist), 2.0f);
}
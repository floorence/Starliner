#include "LightController.h"
#include "camera/PointLightCamera.h"
#include "texture/CubeMapTexture.h"
#include "util/Globals.h"
#include "util/Log.h"
#include "util/Utils.h"
#include <cmath>

LightController::LightController(int fbWidth, int fbHeight) 
    : fbWidth(fbWidth), fbHeight(fbHeight),
      depthMapTexture(DEPTH_MAP_SIZE, DEPTH_MAP_SIZE),
      depthShader("shader/depth.vert", "shader/depth.geom", "shader/depth.frag"),
      pointLightCam(glm::vec3(0.0f), DEPTH_MAP_SIZE, DEPTH_MAP_SIZE),
      hdrTexture("hdrBuffer"),
      bloomTexture("image"),
      hdrBloomShader("shader/gui.vert", "shader/hdr_bloom.frag"),
      blurTextures{{"image"}, {"image"}},
      blurShader("shader/gui.vert", "shader/blur.frag")
{
    prepareDepthMap();
    prepareHdrAndBloom();
    prepareAvgColorBuffer();
    prepareGaussianBlur();
}

uint LightController::registerLight(Light light) {
    lights.push_back(light);
    return lights.size() - 1;
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

    Globals::DefaultShader->setCubeMapTexture(depthMapTexture, "depthMap", 5);
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
    hdrTexture.bind();
    glGenerateMipmap(GL_TEXTURE_2D);

    int highestMipLevel = floor(log2(std::max(fbWidth, fbHeight)));

    int nextIndex = pboIndex;
    int currentIndex = (pboIndex + 1) % 2; 
    pboIndex = currentIndex;
    pbos[pboIndex].bind();
    
    // map the buffer pointer directly to CPU memory to read results of the previous frame
    float* src = (float*)glMapBuffer(GL_PIXEL_PACK_BUFFER, GL_READ_ONLY);
    if (src) {
        float r = src[0];
        float g = src[1];
        float b = src[2];
        
        //Log::log(TAG, fmt::format("average color r: {}, g: {}, b: {}, a: {}", r, g, b, a));
        float logAvgBrightness = Utils::getBrightness(r, g, b); // TODO
        float newExposure;
        if (logAvgBrightness <= 2.0) {
            newExposure = targetExposure;
        } else {
            newExposure = TARGET_BRIGHTNESS / log(logAvgBrightness);
        }
        targetExposure = newExposure;
        if (targetExposure < exposure) {
            exposure = std::max(exposure - adaptationSpeed * deltaTime, targetExposure);
        } else {
            exposure = std::min(exposure + adaptationSpeed * deltaTime, targetExposure);
        }
        //Log::log(TAG, fmt::format("exposure: {}", exposure));
        this->debugBrightness = logAvgBrightness;
        this->debugExposure = newExposure;

        hdrBloomShader.setExposure(exposure);

        glUnmapBuffer(GL_PIXEL_PACK_BUFFER);
    } else {
        Log::err(TAG, fmt::format("failed to read average color buffer!"));
        Utils::checkOpenGlErrors();
    }

    pbos[nextIndex].bind();

    // trigger the gpu transfer for the current frame
    // when a PBO is bound, the last argument is byte offset inside the PBO, rather than a pointer to cpu memory, it returns immediately.
    glGetTexImage(GL_TEXTURE_2D, highestMipLevel, GL_RGBA, GL_FLOAT, 0);

    Utils::unbindPbo();
}

void LightController::blurBrightAreas() {
    blurTextures[0].uniform = "image"; // todo: no reason to not make it bloomBlur in blur shader as well
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
    // since blurAmount is always even we know blurTextures[0] was the last one drawn in blurBrightAreas
    blurTextures[0].uniform = "bloomBlur";
    hdrBloomResult.setTextures({&hdrTexture, &blurTextures[0]});
    hdrBloomResult.draw(hdrBloomShader);
}

void LightController::onFrameBufferSizeChanged(int newWidth, int newHeight) {
    fbWidth = newWidth;
    fbHeight = newHeight;
    // view port is updated in renderForShadows

    for (int i = 0; i < 4; i++) {
        windowSizeTextures[i]->bind();
        glTexImage2D(
            GL_TEXTURE_2D, 0, GL_RGBA16F, fbWidth, fbHeight, 0, GL_RGBA, GL_FLOAT, NULL
        );
    }
    glBindRenderbuffer(GL_RENDERBUFFER, rboID);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, fbWidth, fbHeight);

    for (int i = 0; i < 3; i++) {
        windowSizeFbos[i]->bind();
        windowSizeFbos[i]->checkStatus();
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
    prepareFPTexture(hdrTexture);
    prepareFPTexture(bloomTexture);
    hdrTexture.bind();
    // create depth buffer (renderbuffer) THIS IS NEEDED TO RESOLVE DEPTHS!!! (texture only does colours)
    glGenRenderbuffers(1, &rboID);
    glBindRenderbuffer(GL_RENDERBUFFER, rboID);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, fbWidth, fbHeight);
    // attach textures and buffer
    hdrBloomFbo.bind();
    hdrBloomFbo.attachTexture2D(hdrTexture.ID, 0);
    hdrBloomFbo.attachTexture2D(bloomTexture.ID, 1);
    hdrBloomFbo.attachRenderBuffer(rboID);
    // configure fbo for 2 colour attachments
    unsigned int attachments[2] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1 };
    glDrawBuffers(2, attachments); 
    hdrBloomFbo.checkStatus();
    Utils::unbindFbo();

    hdrBloomShader.setProjection(glm::mat4(1.0f));
    hdrBloomResult.disableDimensionsProcessing = true;
    hdrBloomResult.setCorners(-1.0f, 1.0f, 1.0f, -1.0f);
}

void LightController::prepareAvgColorBuffer() {
    for (int i = 0; i < 2; i++) {
        pbos[i].bind();
        glBufferData(GL_PIXEL_PACK_BUFFER, sizeof(float) * 4, nullptr, GL_STREAM_READ); // 4 float = 1 pixel rgba float
    }
    Utils::unbindPbo();
}

void LightController::prepareGaussianBlur() {
    for (uint i = 0; i < 2; i++) {
        prepareFPTexture(blurTextures[i]);
        blurFbos[i].bind();
        blurFbos[i].attachTexture2D(blurTextures[i].ID);
    }

    blurShader.setProjection(glm::mat4(1.0f));
    blurResult.disableDimensionsProcessing = true;
    blurResult.setCorners(-1.0f, 1.0f, 1.0f, -1.0f);
}

void LightController::prepareFPTexture(Texture& texture) {
    texture.bind();
    glTexImage2D(
        GL_TEXTURE_2D, 0, GL_RGBA16F, fbWidth, fbHeight, 0, GL_RGBA, GL_FLOAT, NULL
    );
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
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
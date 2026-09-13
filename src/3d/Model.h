#pragma once

#include "mesh/Mesh.h"
#include "3d/Object3D.h"
#include "texture/AssetTexture.h"
#include "texture/ImageTexture.h"
#include "texture/Texture.h"
#include <assimp/material.h>
#include <memory>
#include <string>
#include <assimp/scene.h>

class Model: public Object3D {
public:
    Model(std::string path, glm::vec3 position);

    glm::vec3 getDimensions();

    void draw(Camera& camera) override;
    void drawToDepthMap(PointLightCamera& camera, Shader& depthShader) override;
private:
	// stores all the textures loaded so far since we need textures to be class scoped
    // and also optimization to make sure textures aren't loaded more than once.
    // unique_ptr instead of ImageTexture so that actual memory address stays the same which is needed since meshes have pointers to textures
    std::vector<std::unique_ptr<ImageTexture>> textures;
    std::vector<Mesh> meshes;
    std::string directory;

    glm::vec3 minCoords = glm::vec3(INT_MAX);
    glm::vec3 maxCoords = glm::vec3(INT_MIN);
    
    const std::string TAG = "Model";

    void loadModel(std::string path);
    void processNode(aiNode *node, const aiScene *scene);
    Mesh processMesh(aiMesh *mesh, const aiScene *scene);
    std::vector<Texture*> loadMaterialTextures(aiMaterial *mat, aiTextureType type);
    TextureType aiToTextureType(aiTextureType type);

    void preDraw(Camera& camera) override;
};

#include "Model.h"
#include "texture/ImageTexture.h"
#include "util/Log.h"

#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>

Model::Model(std::string path, glm::vec3 position)
    : Object3D(position)
{
    loadModel(path);
}

glm::vec3 Model::getDimensions() {
    return {
        (maxCoords.x - minCoords.x) * scale.x,
        (maxCoords.y - minCoords.y) * scale.y,
        (maxCoords.z - minCoords.z) * scale.z,
    };
}

// function adapted from https://learnopengl.com/code_viewer_gh.php?code=includes/learnopengl/model.h loadModel()
void Model::loadModel(std::string path) {
    Assimp::Importer import;
    const aiScene *scene = import.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs);	
	
    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
        Log::err(TAG, import.GetErrorString());
        return;
    }
    directory = path.substr(0, path.find_last_of('/'));

    processNode(scene->mRootNode, scene);
}

// function adapted from https://learnopengl.com/code_viewer_gh.php?code=includes/learnopengl/model.h processNode()
void Model::processNode(aiNode *node, const aiScene *scene) {
    // process all the node's meshes (if any)
    for (uint i = 0; i < node->mNumMeshes; i++) {
        aiMesh *mesh = scene->mMeshes[node->mMeshes[i]]; 
        meshes.push_back(processMesh(mesh, scene));			
    }
    // then do the same for each of its children
    for (uint i = 0; i < node->mNumChildren; i++) {
        processNode(node->mChildren[i], scene);
    }
}

// function adapted from https://learnopengl.com/code_viewer_gh.php?code=includes/learnopengl/model.h processMesh()
Mesh Model::processMesh(aiMesh *mesh, const aiScene *scene) {
    std::vector<Vertex> vertices;
    std::vector<GLuint> indices;
    std::vector<Texture*> textures;

    for (uint i = 0; i < mesh->mNumVertices; i++) {
        Vertex vertex;
        // positions
        vertex.position = {
            mesh->mVertices[i].x,
            mesh->mVertices[i].y,
            mesh->mVertices[i].z
        };
        // normals
        if (mesh->HasNormals()) {
            vertex.normal = {
                mesh->mNormals[i].x,
                mesh->mNormals[i].y,
                mesh->mNormals[i].z
            };
        }
        // texture coordinates
        // a vertex can contain up to 8 different texture coordinates. since that's weird, only take the first set (0)
        if (mesh->mTextureCoords[0]) {
            vertex.texUV = {
                mesh->mTextureCoords[0][i].x,
                mesh->mTextureCoords[0][i].y
            };
        }        
        // colours, idk if a vertex can contain more than one but that'd be weird so only take the first set (0)
        if (mesh->mColors[0]) {
            vertex.color = {
                mesh->mColors[0][i].r,
                mesh->mColors[0][i].g,
                mesh->mColors[0][i].b
            };
            // Log::log(TAG, fmt::format("vertex.color = {}, {}, {}", vertex.color.r, vertex.color.g, vertex.color.b));
        }
        // update min max coordinates
        minCoords.x = std::min(minCoords.x, vertex.position.x);
        minCoords.y = std::min(minCoords.y, vertex.position.y);
        minCoords.z = std::min(minCoords.z, vertex.position.z);
        maxCoords.x = std::max(maxCoords.x, vertex.position.x);
        maxCoords.y = std::max(maxCoords.y, vertex.position.y);
        maxCoords.z = std::max(maxCoords.z, vertex.position.z);

        vertices.push_back(vertex);
    }
    // indices from mesh faces
    for (uint i = 0; i < mesh->mNumFaces; i++) {
        aiFace face = mesh->mFaces[i];

        for (uint j = 0; j < face.mNumIndices; j++)
            indices.push_back(face.mIndices[j]);        
    }
    // process materials
    aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];    

    // diffuse maps
    std::vector<Texture*> diffuseMaps = loadMaterialTextures(material, aiTextureType_DIFFUSE);
    textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());
    // specular maps
    std::vector<Texture*> specularMaps = loadMaterialTextures(material, aiTextureType_SPECULAR);
    textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());

    if (textures.empty()) {
        // it must be using colours instead of textures
        glm::vec3 color = glm::vec3(1.0f);
        aiColor3D aiColor;
        if (material->Get(AI_MATKEY_COLOR_DIFFUSE, aiColor) == AI_SUCCESS) {
            color = {aiColor.r, aiColor.g, aiColor.b};
            color *= 10; // there are extremely bright light sources in the world so make models brighter to still be visible
            // Log::log(TAG, fmt::format("color: {}, {}, {}", color.r, color.g, color.b));
        } else {
            Log::warn(TAG, "processMesh() found a mesh without textures or material colours!");
        }
        return Mesh(vertices, indices, color);
    } else {
        return Mesh(vertices, indices, textures);
    }
}

// function adapted from https://learnopengl.com/code_viewer_gh.php?code=includes/learnopengl/model.h loadMaterialTextures()
std::vector<Texture*> Model::loadMaterialTextures(aiMaterial *mat, aiTextureType type) {
    std::vector<Texture*> textures;
    // if there's more than one texture per type, the last texture that is created will be the one that's used
    for (uint i = 0; i < mat->GetTextureCount(type); i++) {
        aiString str;
        mat->GetTexture(type, i, &str);
        std::string path = (directory + std::string(str.C_Str()));
        // check if texture was loaded before and if so, continue to next iteration
        bool skip = false;
        for (uint j = 0; j < this->textures.size(); j++) {
            if (std::strcmp(this->textures[j]->getPath().c_str(), str.C_Str()) == 0) {
                textures.push_back(this->textures[j].get());
                skip = true;
                break;
            }
        }

        if (!skip) {
            auto texture = std::make_unique<ImageTexture>(
                path.c_str(),
                aiToTextureType(type)
            );
            textures.push_back(texture.get());
            this->textures.push_back(std::move(texture));
        }
    }
    return textures;
}

TextureType Model::aiToTextureType(aiTextureType type) {
    switch (type) {
        case aiTextureType_DIFFUSE: return TextureType::Diffuse;
        case aiTextureType_SPECULAR: return TextureType::Specular;
        default:
            Log::err(TAG, fmt::format("unrecognized texture type! {}", (int) type));
            return TextureType::Diffuse;
    }
}

void Model::preDraw() {
    Object3D::preDraw();
    shader->setShininess(16); // TODO
}

void Model::draw(Camera& camera) {
    preDraw();
    for (auto& mesh: meshes) {
        mesh.draw(camera, *shader);
    }
}

void Model::drawToDepthMap(PointLightCamera& camera, Shader& depthShader) {
    preDrawToDepthMap(depthShader);
    for (auto& mesh: meshes) {
        mesh.drawToDepthMap(camera, depthShader);
    }
}
#pragma once

#include "gui/framework/Rect.h"
#include "mesh/Mesh.h"
#include "texture/Texture.h"
#include "shader/Shader.h"
#include "util/Globals.h"

// assumes y increases downward, since that's how the gui shader is set up. 
class Quad: public Rect {
public:
    glm::vec4 tintColor = glm::vec4(0.0f);

    Quad() = default;
    Quad(Texture* texture);
    Quad(float x, float y, float w, float h, bool corners = false);

    void draw(Shader& shader = *Globals::GuiShader);

    // Textures and colors, if none is called the Quad will draw with whatever was last uploaded
    // to the shader (undefined behaviour).

    void setColor(glm::vec3 color);
    void setColor(glm::vec4 color);
    void setTexture(Texture* texture);
    void setTextures(std::vector<Texture*> textures);
private:
    Mesh mesh;

    void onBoundsChanged() override;
};

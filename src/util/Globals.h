#pragma once

class FontTexture;
class Shader;

namespace Globals {
    inline FontTexture* Font;
    inline Shader* GuiShader;
    inline Shader* FontShader;
    inline Shader* DefaultShader;
    inline Shader* LightShader;
    inline Shader* FlatShader;

    inline bool drawDebugVectors = true;
}

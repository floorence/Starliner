#pragma once

#include "gui/element/Button.h"
#include "texture/ImageTexture.h"

class IconButton: public Button {
public:
    IconButton(std::string iconPath);

    void draw() override;
private:
    Quad icon;
    ImageTexture iconTex;
    static constexpr float ICON_TO_EDGE_MARGIN = 5.0f;

    void onBoundsChanged() override;
};
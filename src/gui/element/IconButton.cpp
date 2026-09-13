#include "IconButton.h"
#include "texture/ImageTexture.h"
#include "util/Log.h"

IconButton::IconButton(std::string iconPath)
    : iconTex(iconPath.c_str(), TextureType::Diffuse) 
{
    icon.setTexture(&iconTex);
}

void IconButton::onBoundsChanged() {
    // Log::log("IconButton", fmt::format("onBoundsChanged() x: {}, y: {}, w: {}, h: {}", x, y, w, h));
    Button::onBoundsChanged();
    float iconSize = w - ICON_TO_EDGE_MARGIN * 2;
    icon.setBounds(x + ICON_TO_EDGE_MARGIN, y + ICON_TO_EDGE_MARGIN, iconSize, iconSize);
}

void IconButton::onHover() {
    Button::onHover();
    icon.tintColor = glm::vec4(1.0f, 1.0f, 1.0f, 0.1f);
}

void IconButton::onHoverOff() {
    Button::onHoverOff();
    icon.tintColor = glm::vec4(0.0f);
}

void IconButton::draw() {
    Button::draw();
    icon.draw();
}

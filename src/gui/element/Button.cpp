#include "Button.h"

void Button::onBoundsChanged() {
    background.setBounds(x, y, w, h);
}

void Button::onHover() {
    // Log::log("Button", fmt::format("{} onHover()", text.getText()));
    background.tintColor = glm::vec4(1.0f, 1.0f, 1.0f, 0.1f);
}

void Button::onHoverOff() {
    // Log::log("Button", fmt::format("{} onHoverOff()", text.getText()));
    background.tintColor = glm::vec4(0.0f);
}

void Button::setBackgroundColor(glm::vec3 color) {
    background.setColor(color);
}

void Button::setBackgroundColor(glm::vec4 color) {
    background.setColor(color);
}

void Button::draw() {
    background.draw();
}

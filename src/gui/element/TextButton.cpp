#include "TextButton.h"
#include "texture/FontTexture.h"
#include "util/Log.h"

void TextButton::onBoundsChanged() {
    Button::onBoundsChanged();
    text.setFontSize(deriveFontSize(h));
    text.setBounds(x, y, w, h);
}

float TextButton::getUnboundWidth(float h) {
    // Log::log("Button", fmt::format("getUnboundWidth: text: {} fontSize: {}", text.getText(), deriveFontSize(h)));
    std::pair<float, float> textSize = Globals::Font->getSize(text.getText(), deriveFontSize(h));
    return TEXT_TO_EDGE_MARGIN * 2 + textSize.first;
}

float TextButton::getUnboundHeight(float w) {
    return w / 4;
}

int TextButton::deriveFontSize(float h) {
    if (text.getText().size() == 1) { // text is being used as icon, make it bigger
        return h * 3/4;
    } else {
        return h / 2;
    }
}

void TextButton::setText(std::string text) {
    this->text.setText(text);
    invalidateBounds();
}

void TextButton::draw() {
    Button::draw();
    text.draw();
}

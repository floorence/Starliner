#pragma once

#include "gui/element/Button.h"
#include "gui/framework/Text.h"

class TextButton: public Button {
public:
    TextButton() = default;

    void setText(std::string text);
    void draw() override;

    // public since other gui elements who use button might need to call these
    float getUnboundWidth(float h) override;
    float getUnboundHeight(float w) override;
private:
    Text text;
    static constexpr float TEXT_TO_EDGE_MARGIN = 10.0f;

    void onBoundsChanged() override;
    int deriveFontSize(float h);
};

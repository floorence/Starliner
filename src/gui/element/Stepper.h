#pragma once

#include "gui/element/TextButton.h"
#include "gui/framework/Clickable.h"
#include "gui/element/IntGuiElement.h"

class Stepper: public IntGuiElement {
public:
    Stepper();

    bool dispatchMouseEvent(float x, float y, MouseEvent event) override;
    int getData() override;
    void setData(int data) override;
    void setStepAmount(int amount);
    void setMinMax(int minCount, int maxCount);
    void setUnits(std::string units);
    void setColors(glm::vec3 buttonsColor, glm::vec3 textColor);
    void draw() override;
private:
    TextButton decButton;
    Text countText;
    TextButton incButton;
    std::string units = "";

    int count = 0, stepAmount = 1, min, max;
    const float TEXT_TO_BUTTONS_MARGIN = 10.0f;

    void onBoundsChanged() override;
    float getUnboundWidth(float h) override;
    float getUnboundHeight(float w) override;
};

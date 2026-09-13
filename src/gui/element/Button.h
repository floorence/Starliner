#pragma once

#include "gui/element/GuiElement.h"
#include "gui/framework/Quad.h"

class Button: public GuiElement {
public:
    Button() = default;
    virtual ~Button() = default;

    // need to explicitly declare because of the explicitly declared destructor, which causes the compiler to not generate these
    Button(Button&&) noexcept = default;
    Button& operator=(Button&&) noexcept = default;

    void setBackgroundColor(glm::vec3 color);
    void setBackgroundColor(glm::vec4 color);
    void onHover() override;
    void onHoverOff() override;

    void draw() override;
protected:
    Quad background;

    void onBoundsChanged() override;
};
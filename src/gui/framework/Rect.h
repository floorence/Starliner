#pragma once

#include <string>

enum class Alignment {
    TopLeft, TopRight, CenterVertical, CenterHorizontal, Center
    // todo when there's a use case: BotLeft, BotRight
};

class Rect {
public:
    // set to true if dimensions are expected to be negative for some reason. ONLY FOR LIGHTCONTROLLER QUADS!
    // LightController quads are drawn with special shaders that follow default opengl axes, but Rect and Quad 
    // assumes reversed y axis, leading to negative height
    bool disableDimensionsProcessing = false;
    // TODO: add way to change only one at a time

    Rect() = default;
    Rect(float x, float y, float w, float h, bool corners = false);
    virtual ~Rect() = default;

    /**
     * @param x, y top left of rect
     * @param w, h width and height of rect; passing negative for either value will be interpreted
     *             as an "unbound" value and an actual value will be set accordingly
     */
    void setBounds(float x, float y, float w, float h);
    /** @brief same as setBounds but x value is the right of rect. */
    void setBoundsEnd(float x, float y, float w, float h);
    /**
     * @param xu, yu top left of rect
     * @param xv, yv bottom right of rect
     */
    void setCorners(float xu, float yu, float xv, float yv);

    void centerHorizontally(float y, float w, float h, float start, float end);
    void centerVertically(float x, float w, float h, float start, float end);
    void center(float w, float h, float startX, float endX, float startY, float endY);
    /** re-apply saved bounds, recalculating any unbound values */
    void invalidateBounds();
protected:
    float x = 0, y = 0, w = 0, h = 0;
    float specX = 0, specY = 0, specW = 0, specH = 0; // keep track of what public functions were called with
    float startX = 0, endX = 0, startY = 0, endY = 0;
    Alignment alignment = Alignment::TopLeft;
    // set to true if subclass can handle unbound width and height at the same time
    bool enableUnboundWidthAndHeight = false;
private:
    std::string TAG = "Rect";

    virtual void onBoundsChanged() {};
    virtual float getUnboundWidth(float h) { return h; };
    virtual float getUnboundHeight(float w) { return w; };
    void setBoundsInternal(float x, float y, float w, float h);
    std::pair<float, float> processDimensions(float w, float h);
};

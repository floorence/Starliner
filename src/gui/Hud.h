#pragma once

#include "gui/element/IconButton.h"
#include "window/WindowListener.h"
#include "gui/SettingsMenu.h"

class Hud: public Clickable, public WindowListener {
public:
    Hud(int windowWidth, int windowHeight, SettingsMenu* settingsMenu);

    bool dispatchMouseEvent(float x, float y, MouseEvent event) override;
    void onWindowSizeChanged(int newWidth, int newHeight) override;

    void setPerformanceText(std::string text);
    void draw();
private:
	IconButton settingsButton;
    IconButton debugButton;
	Text performanceText;
};
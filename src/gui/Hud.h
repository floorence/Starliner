#pragma once

#include "gui/DebugMenu.h"
#include "gui/element/IconButton.h"
#include "window/WindowListener.h"
#include "gui/SettingsMenu.h"

class Hud: public Clickable, public WindowListener {
public:
	IconButton settingsButton;
    IconButton debugButton;

    Hud(int windowWidth, int windowHeight);

    bool dispatchMouseEvent(float x, float y, MouseEvent event) override;
    void onWindowSizeChanged(int newWidth, int newHeight) override;

    void setPerformanceText(std::string text);
    void draw();
private:
	Text performanceText;
};
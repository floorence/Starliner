#pragma once

#include "gui/element/TextButton.h"
#include "gui/framework/Clickable.h"
#include "gui/framework/Quad.h"
#include "gui/framework/Rect.h"
#include "mass/Player.h"
#include "window/WindowListener.h"
#include "world/StarSystem.h"

struct StarSystemEntry {
    bool populated;
    Text regionText;
    Text positionText;
    TextButton teleportButton;
};

class DebugMenu: public Rect, public Clickable, public WindowListener {
public:
    bool isOpen = false;

    DebugMenu(Player* player);
    void setStarSystems(std::vector<StarSystem*> starSystems);

    void draw();
    bool dispatchMouseEvent(float x, float y, MouseEvent event) override;
    void onWindowSizeChanged(int newWidth, int newHeight) override;
private:
    Text debugTitle = Text("Debug");
    Quad background;
    TextButton closeButton;

    std::vector<StarSystemEntry> entries;
    const float MAX_ENTRIES = 10;

    Player* player;

    void initialize();
    void onBoundsChanged() override;
};
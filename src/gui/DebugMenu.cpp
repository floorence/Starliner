#include "DebugMenu.h"
#include "gui/element/TextButton.h"
#include "util/Constants.h"
#include "world/World.h"

DebugMenu::DebugMenu(Player* player): player(player) {
    initialize();
}

void DebugMenu::initialize() {
    background.setColor(Constants::GREY_3);

    closeButton.setText("x");
    closeButton.setBackgroundColor(Constants::GREY_7);
    closeButton.setOnClick([this]() {
        isOpen = false;
    });

    // allocate MAX_ENTRIES star system entries; if there are more than MAX_ENTRIES close star systems, too bad!
    for (uint i = 0; i < MAX_ENTRIES; i++) {
        StarSystemEntry entry = {
            false,
            Text(),
            Text(),
            TextButton(),
        };
        entry.teleportButton.setBackgroundColor(Constants::GREY_5);
        entry.teleportButton.setText("Teleport!");
        entries.push_back(std::move(entry));
    }
}

void DebugMenu::setStarSystems(std::vector<StarSystem*> starSystems) {
    for (uint i = 0; i < MAX_ENTRIES; i++) {
        if (i < starSystems.size()) {
            entries[i].populated = true;

            glm::vec3 starPos = starSystems[i]->starLightData.position;
            Region region = {
                static_cast<int>(std::floor(starPos.x / World::REGION_SIZE)),
                static_cast<int>(std::floor(starPos.y / World::REGION_SIZE)), 
                static_cast<int>(std::floor(starPos.z / World::REGION_SIZE))
            };
            std::string regionStr = fmt::format("Region: {}, {}, {}", region.x, region.y, region.z);
            entries[i].regionText.setText(regionStr);
            
            std::string positionStr = fmt::format("Pos: {}, {}, {}", starPos.x, starPos.y, starPos.z);
            entries[i].positionText.setText(positionStr);
            entries[i].teleportButton.setOnClick([this, starPos]() {
                this->player->teleport(starPos);
            });
        } else {
            entries[i].populated = false;
        }
    }
}

void DebugMenu::onBoundsChanged() {
    float headerHeight = h / 10;
    float padding = headerHeight / 6;
    float endWithPadding = x + w - padding;
    float closeButtonSize = headerHeight * 2/3;
    float entryHeight = h / 16;
    float entry1Y = y + headerHeight + padding;
    float regionTextX = x + padding;
    float positionTextX = x + w * 1/3;

    background.setBounds(x, y, w, h);
    debugTitle.setFontSize(headerHeight * 2/3);
    debugTitle.center(-1, -1, x, x + w, y, y + headerHeight);

    closeButton.setBounds(endWithPadding - closeButtonSize, y + padding, closeButtonSize, closeButtonSize);

    for (uint i = 0; i < MAX_ENTRIES; i++) {
        float entryY = entry1Y + (entryHeight + padding) * i;
        entries[i].regionText.setFontSize(entryHeight * 2/3);
        entries[i].regionText.centerVertically(
            regionTextX, 
            -1, -1,
            entryY, 
            entryY + entryHeight
        );
        entries[i].positionText.setFontSize(entryHeight * 2/3);
        entries[i].positionText.centerVertically(
            positionTextX, 
            -1, -1,
            entryY, 
            entryY + entryHeight
        );
        entries[i].teleportButton.setBoundsEnd(
            endWithPadding,
            entryY, -1, entryHeight
        );
    }
}

bool DebugMenu::dispatchMouseEvent(float x, float y, MouseEvent event) {
    if (!isOpen) return false;

    for (uint i = 0; i < MAX_ENTRIES; i++) {
        if (!entries[i].populated) break;
        bool registeredClick = entries[i].teleportButton.dispatchMouseEvent(x, y, event);
        if (event != MouseEvent::Hover && registeredClick) return true;
    }
    return closeButton.dispatchMouseEvent(x, y, event);
}

void DebugMenu::onWindowSizeChanged(int newWidth, int newHeight) {
	setCorners(100, 100, newWidth - 100, newHeight - 100);
}

void DebugMenu::draw() {
    background.draw();
    debugTitle.draw();
    closeButton.draw();

    for (uint i = 0; i < MAX_ENTRIES; i++) {
        if (!entries[i].populated) break;
        entries[i].regionText.draw();
        entries[i].positionText.draw();
        entries[i].teleportButton.draw();
    }
}

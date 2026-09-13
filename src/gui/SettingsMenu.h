#pragma once

#include "window/WindowListener.h"
#include "settings/SettingsController.h"
#include "settings/SettingsListener.h"
#include "gui/element/TextButton.h"
#include "gui/element/IntGuiElement.h"
#include "gui/framework/Quad.h"
#include <memory>

struct SettingGui {
    std::unique_ptr<Text> description; // could store just Text. but unique_ptrs are nice and it would be consistent with guiElement.
    std::unique_ptr<IntGuiElement> guiElement; // pointer because IntGuiElement is abstract
};

class SettingsMenu: public Rect, public SettingsListener, public Clickable, public WindowListener {
public:
    bool isOpen = false;

    SettingsMenu(SettingsController* sc);

    void draw();
    bool dispatchMouseEvent(float x, float y, MouseEvent event) override;
    void onSettingsChanged(const Settings& settings) override;
    void onWindowSizeChanged(int newWidth, int newHeight) override;
private:
    Text settingsTitle = Text("Settings");
    Quad background;
    Quad headerBackground;
    Quad footerBackground;

    TextButton closeButton;
    TextButton applyButton;
    TextButton cancelButton;

    glm::vec3 textColor = glm::vec3(1.0f, 1.0f, 1.0f);

    std::vector<TextButton> categoryButtons;
    std::vector<std::vector<SettingGui>> settingGuiData;

    const std::string TAG = "SettingsMenu";

    SettingsController* sc;
    int currentTab = 0;

    void initialize(const Settings& settings);
    void initPersistentUI();
    void initGuiElementsFor(Setting& setting, int categoryNum);

    void onBoundsChanged() override;
    Settings readSettings();
};

#include "SettingsMenu.h"
#include "settings/Settings.h"
#include "gui/element/CycleButton.h"
#include "gui/element/IntGuiElement.h"
#include "gui/element/Stepper.h"
#include "gui/element/Toggle.h"
#include "util/Log.h"
#include <fmt/format.h>
#include <memory>

SettingsMenu::SettingsMenu(SettingsController* sc)
    : sc(sc) 
{
    initialize(Settings {});
}

void SettingsMenu::initialize(const Settings& settings) {
    initPersistentUI();

    for (uint i = 0; i < settings.children.size(); i++) {
        TextButton categoryButton;
        std::string categoryName = settings.children[i]->name;
        categoryButton.setText(categoryName);
        categoryButton.setBackgroundColor(Constants::GREY_4);
        categoryButton.setOnClick([this, categoryName, i]() {
            currentTab = i;
        });
        categoryButtons.push_back(std::move(categoryButton));
        settingGuiData.push_back(std::vector<SettingGui>());

        for (auto& setting: settings.children[i]->getChildren()) {
            initGuiElementsFor(*setting, i);
        }
    }
}

void SettingsMenu::initPersistentUI() {
    background.setColor(Constants::GREY_3);
    headerBackground.setColor(Constants::GREY_5);
    footerBackground.setColor(Constants::GREY_5);

    closeButton.setText("x");
    closeButton.setBackgroundColor(Constants::GREY_7);
    closeButton.setOnClick([this]() {
        isOpen = false;
    });

    applyButton.setText("Apply");
    applyButton.setBackgroundColor(Constants::GREY_7);
    applyButton.setOnClick([this]() {
        this->sc->save(readSettings());
    });

    cancelButton.setText("Cancel");
    cancelButton.setBackgroundColor(Constants::GREY_7);
    cancelButton.setOnClick([this]() {
        isOpen = false;
    });
}

void SettingsMenu::initGuiElementsFor(Setting& setting, int categoryNum) {
    Log::log(TAG, fmt::format("initGuiElementsFor({}, {})", setting.name, categoryNum));
    SettingGui settingGui;
    
    settingGui.description = std::make_unique<Text>(Text(setting.name));

    switch (setting.guiRepresentation) {
        case SettingGuiElement::Stepper: {
            auto stepper = std::make_unique<Stepper>();

            stepper->setColors(Constants::GREY_5, textColor);
            stepper->setMinMax(setting.minValue, setting.maxValue);            
            stepper->setStepAmount((setting.maxValue - setting.minValue > 50) ? 10 : 1); // TODO

            settingGui.guiElement = std::move(stepper);
            break;
        }
        case SettingGuiElement::Toggle: {
            auto toggle = std::make_unique<Toggle>();
            
            toggle->setColors(Constants::GREY_4, Constants::GREY_5);
            settingGui.guiElement = std::move(toggle);
            break;
        }
        case SettingGuiElement::CycleButton: {
            auto cycleButton = std::make_unique<CycleButton>();
            
            cycleButton->setBackgroundColor(Constants::GREY_5);
            cycleButton->setOptions(setting.options);
            settingGui.guiElement = std::move(cycleButton);
            break;
        }
    }

    settingGuiData[categoryNum].push_back(std::move(settingGui));
}

void SettingsMenu::onBoundsChanged() {
    float headerFooterHeight = h / 10;
    float tabHeight = h / 12;
    float settingHeight = h / 20;
    float sideBarWidth = w / 4;
    float buttonsHeight = headerFooterHeight * 2/3;
    float padding = headerFooterHeight / 6;

    float endWithPadding = x + w - padding;
    float bottomWithPadding = y + h - padding;
    float setting1Y = y + headerFooterHeight + padding;

    background.setBounds(x, y, w, h);
    headerBackground.setBounds(x, y, w, headerFooterHeight);
    footerBackground.setBounds(x, y + h - headerFooterHeight, w, headerFooterHeight);

    settingsTitle.setFontSize(headerFooterHeight * 2/3);
    settingsTitle.center(-1, -1, x, x + w, y, y + headerFooterHeight);

    closeButton.setBounds(endWithPadding - buttonsHeight, y + padding, buttonsHeight, buttonsHeight);
    applyButton.setBounds(endWithPadding - buttonsHeight * 2, bottomWithPadding - buttonsHeight, buttonsHeight * 2, buttonsHeight);
    cancelButton.setBounds(endWithPadding - buttonsHeight * 4 - padding, bottomWithPadding - buttonsHeight, buttonsHeight * 2, buttonsHeight);

    for (uint i = 0; i < categoryButtons.size(); i++) {
        categoryButtons[i].setBounds(x, y + headerFooterHeight + tabHeight * i, sideBarWidth, tabHeight);
    }

    for (uint i = 0; i < settingGuiData.size(); i++) {
        for (uint j = 0; j < settingGuiData[i].size(); j++) {
            float settingY = setting1Y + (settingHeight + padding) * j;

            settingGuiData[i][j].description->setFontSize(settingHeight * 2/3);
            settingGuiData[i][j].description->centerVertically(
                x + sideBarWidth + padding, 
                -1, -1,
                settingY, 
                settingY + settingHeight
            );

            settingGuiData[i][j].guiElement->setBoundsEnd(
                endWithPadding,
                settingY,
                -1,
                settingHeight
            );
        }
    }
}

bool SettingsMenu::dispatchMouseEvent(float x, float y, MouseEvent event) {
    if (!isOpen) return false;
    std::vector<Clickable*> clickables = {&closeButton, &cancelButton, &applyButton};
    for (auto& button: categoryButtons) {
        clickables.push_back(&button);
    }

    for (auto& settingGui: settingGuiData[currentTab]) {
        clickables.push_back(settingGui.guiElement.get());
    }

    for (auto& clickable: clickables) {
        bool registeredClick = clickable->dispatchMouseEvent(x, y, event);
        if (event != MouseEvent::Hover && registeredClick) return true;
    }
    return false;
}

void SettingsMenu::onSettingsChanged(const Settings& settings) {
    Log::log(TAG, "onSettingsChanged");

    for (uint i = 0; i < settings.children.size(); i++) {
        SettingsCategory* category = settings.children[i];

        for (uint j = 0; j < category->getChildren().size(); j++) {
            Setting* setting = category->getChildren()[j];
            settingGuiData[i][j].guiElement->setData(setting->value);
        }
    }
}

void SettingsMenu::onWindowSizeChanged(int newWidth, int newHeight) {
	setCorners(100, 100, newWidth - 100, newHeight - 100);
}

Settings SettingsMenu::readSettings() {
    Settings settings;

    for (uint i = 0; i < settings.children.size(); i++) {
        SettingsCategory* category = settings.children[i];

        for (uint j = 0; j < category->getChildren().size(); j++) {
            Setting* setting = category->getChildren()[j];
            setting->value = settingGuiData[i][j].guiElement->getData();
        }
    }

    return settings;
}

void SettingsMenu::draw() {
    background.draw();
    headerBackground.draw();
    footerBackground.draw();
    settingsTitle.draw();
    closeButton.draw();
    applyButton.draw();
    cancelButton.draw();

    for (auto& button: categoryButtons) {
        button.draw();
    }

    for (auto& settingGui: settingGuiData[currentTab]) {
        settingGui.description->draw();
        settingGui.guiElement->draw();
    }
}
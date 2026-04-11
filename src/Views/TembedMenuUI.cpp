#if defined(DEVICE_TEMBEDS3) || defined(DEVICE_TEMBEDS3CC1101)

#include "TembedMenuUI.h"
#include "States/GlobalState.h"
#include <Arduino.h>

TembedMenuUI::TembedMenuUI()
    : tft(nullptr), menuSprite(nullptr), isDirty(true),
      currentState(ViewState::List), showingGeneralCommands(false),
      selectedIndex(0), scrollOffset(0), carouselIndex(0), lastRenderMs(0)
{
    activeCommand = {"", "", false};
}

TembedMenuUI::~TembedMenuUI() {
    if (menuSprite) {
        menuSprite->deleteSprite();
        delete menuSprite;
    }
}

void TembedMenuUI::initialize(void* screen) {
    tft = static_cast<LGFX_Tembed*>(screen);

    // Use the right half of the screen
    scrW = tft->width() / 2;
    scrH = tft->height();
    originX = tft->width() / 2;
    originY = 0;

    menuSprite = new LGFX_Sprite(tft);
    menuSprite->setColorDepth(16);
    menuSprite->createSprite(scrW, scrH);

    loadItems();
}

void TembedMenuUI::loadItems() {
    auto currentMode = GlobalState::getInstance().getCurrentMode();
    if (showingGeneralCommands) {
        currentItems = MenuTree::getGeneralCommands();
    } else {
        currentItems = MenuTree::getProtocolCommands(currentMode);
        if (currentItems.empty()) {
            showingGeneralCommands = true;
            currentItems = MenuTree::getGeneralCommands();
        }
    }

    // Ensure index bounds
    if (selectedIndex >= (int)currentItems.size()) {
        selectedIndex = currentItems.size() - 1;
    }
    if (selectedIndex < -1) {
        selectedIndex = -1;
    }
}

void TembedMenuUI::scroll(int delta) {
    if (currentState == ViewState::List) {
        selectedIndex += delta;
        if (selectedIndex < -1) selectedIndex = -1;
        if (selectedIndex >= (int)currentItems.size()) {
            selectedIndex = (int)currentItems.size() - 1;
        }

        // Adjust scrollOffset to keep selectedIndex visible
        int visibleItems = (scrH - 30) / 20; // 30 header, 20 item height
        if (selectedIndex >= 0) {
            if (selectedIndex < scrollOffset) {
                scrollOffset = selectedIndex;
            } else if (selectedIndex >= scrollOffset + visibleItems) {
                scrollOffset = selectedIndex - visibleItems + 1;
            }
        }

    } else if (currentState == ViewState::Keyboard) {
        carouselIndex += delta;
        if (carouselIndex < 0) carouselIndex = (int)carouselItems.size() - 1;
        if (carouselIndex >= (int)carouselItems.size()) carouselIndex = 0;
    }
    isDirty = true;
}

std::string TembedMenuUI::select() {
    isDirty = true;

    if (currentState == ViewState::List) {
        if (selectedIndex == -1) {
            // Toggle between general commands and mode commands
            showingGeneralCommands = !showingGeneralCommands;
            selectedIndex = 0;
            scrollOffset = 0;
            loadItems();
            return "";
        }

        if (selectedIndex >= 0 && selectedIndex < (int)currentItems.size()) {
            MenuCommand cmd = currentItems[selectedIndex];
            if (cmd.requiresParameter) {
                activeCommand = cmd;
                currentState = ViewState::Keyboard;
                keyboardBuffer = "";
                carouselIndex = 0; // Default to 'A'
                return "";
            } else {
                return cmd.name + "\r";
            }
        }
    } else if (currentState == ViewState::Keyboard) {
        std::string selectedChar = carouselItems[carouselIndex];
        if (selectedChar == "<OK>") {
            currentState = ViewState::List;
            std::string cmdToExecute = activeCommand.name + " " + keyboardBuffer + "\r";
            keyboardBuffer = "";
            return cmdToExecute;
        } else if (selectedChar == "<DEL>") {
            if (!keyboardBuffer.empty()) {
                keyboardBuffer.pop_back();
            }
        } else if (selectedChar == "<ESC>") {
            currentState = ViewState::List;
            keyboardBuffer = "";
        } else {
            keyboardBuffer += selectedChar;
        }
    }
    return "";
}

void TembedMenuUI::reset() {
    showingGeneralCommands = false;
    selectedIndex = 0;
    scrollOffset = 0;
    currentState = ViewState::List;
    loadItems();
    isDirty = true;
}

void TembedMenuUI::setDirty() {
    isDirty = true;
}

void TembedMenuUI::render(bool force) {
    if (!isDirty && !force) return;

    // Throttle rendering
    uint32_t now = millis();
    if (now - lastRenderMs < 30) return; // ~30fps max
    lastRenderMs = now;
    isDirty = false;

    // Clear sprite
    menuSprite->fillScreen(TFT_BLACK);

    if (currentState == ViewState::List) {
        renderList();
    } else {
        renderKeyboard();
    }

    // Push to right half of screen
    menuSprite->pushSprite(originX, originY);
}

void TembedMenuUI::renderHeader() {
    menuSprite->fillRect(0, 0, scrW, 25, TFT_DARKGREY);
    menuSprite->setTextColor(TFT_WHITE, TFT_DARKGREY);
    menuSprite->setTextSize(1);

    // Back button focus indicator
    if (selectedIndex == -1 && currentState == ViewState::List) {
        menuSprite->fillRect(2, 2, 20, 21, TFT_GREEN);
        menuSprite->setTextColor(TFT_BLACK, TFT_GREEN);
    } else {
        menuSprite->setTextColor(TFT_WHITE, TFT_DARKGREY);
    }

    menuSprite->drawString("[<]", 4, 8);

    menuSprite->setTextColor(TFT_WHITE, TFT_DARKGREY);
    std::string pathText = showingGeneralCommands ? "General" : ModeEnumMapper::toString(GlobalState::getInstance().getCurrentMode());
    menuSprite->drawString(pathText.c_str(), 30, 8);
    menuSprite->drawFastHLine(0, 25, scrW, TFT_GREEN);
}

void TembedMenuUI::renderList() {
    renderHeader();

    int startY = 30;
    int itemHeight = 20;
    int visibleItems = (scrH - startY) / itemHeight;

    for (int i = 0; i < visibleItems; i++) {
        int index = scrollOffset + i;
        if (index >= (int)currentItems.size()) break;

        int y = startY + (i * itemHeight);
        bool isSelected = (index == selectedIndex);

        renderItem(index, y, scrW, itemHeight, isSelected);
    }
}

void TembedMenuUI::renderItem(int index, int y, int w, int h, bool isSelected) {
    MenuCommand cmd = currentItems[index];

    if (isSelected) {
        menuSprite->fillRect(0, y, w, h, TFT_GREEN);
        menuSprite->setTextColor(TFT_BLACK, TFT_GREEN);
    } else {
        menuSprite->fillRect(0, y, w, h, TFT_BLACK);
        menuSprite->setTextColor(TFT_WHITE, TFT_BLACK);
    }

    // Indent text slightly
    menuSprite->drawString(cmd.name.c_str(), 10, y + 4);

    if (cmd.requiresParameter) {
        menuSprite->drawString(">", w - 15, y + 4);
    }
}

void TembedMenuUI::renderKeyboard() {
    // Title
    menuSprite->fillRect(0, 0, scrW, 25, TFT_DARKGREY);
    menuSprite->setTextColor(TFT_WHITE, TFT_DARKGREY);
    std::string title = "INPUT: " + activeCommand.name;
    menuSprite->drawString(title.c_str(), 5, 8);
    menuSprite->drawFastHLine(0, 25, scrW, TFT_GREEN);

    // Buffer input text box
    int boxY = 40;
    menuSprite->drawRoundRect(5, boxY, scrW - 10, 30, 4, TFT_GREEN);
    menuSprite->setTextColor(TFT_GREEN, TFT_BLACK);
    menuSprite->drawString(keyboardBuffer.c_str(), 10, boxY + 8);

    // Carousel
    int carouselY = 90;
    menuSprite->setTextColor(TFT_WHITE, TFT_BLACK);
    menuSprite->drawString("Select Character:", 5, carouselY - 15);

    int numItems = carouselItems.size();

    // Draw 5 items horizontally
    for (int i = -2; i <= 2; i++) {
        int idx = carouselIndex + i;
        while (idx < 0) idx += numItems;
        while (idx >= numItems) idx -= numItems;

        int xPos = (scrW / 2) + (i * 30);

        if (i == 0) {
            menuSprite->setTextColor(TFT_GREEN, TFT_BLACK);
            menuSprite->drawString(("[ " + carouselItems[idx] + " ]").c_str(), xPos - 10, carouselY);
        } else {
            menuSprite->setTextColor(TFT_DARKGREY, TFT_BLACK);
            menuSprite->drawString(carouselItems[idx].c_str(), xPos, carouselY);
        }
    }
}

#endif

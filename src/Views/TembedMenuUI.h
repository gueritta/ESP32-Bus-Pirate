#pragma once

#if defined(DEVICE_TEMBEDS3) || defined(DEVICE_TEMBEDS3CC1101)

#include <string>
#include <vector>
#include "Data/MenuTree.h"
#include "Enums/ModeEnum.h"
#include <LovyanGFX.hpp>
#include "TembedDeviceView.h"

class TembedMenuUI {
public:
    TembedMenuUI();
    ~TembedMenuUI();

    void initialize(void* screen);

    // Call this repeatedly inside the input loop or whenever rotary position changes
    void render(bool force = false);

    // Rotary dial input
    void scroll(int delta);

    // Select button input
    // Returns a complete string when a command is ready to be dispatched
    std::string select();

    // Reset back to main mode selection
    void reset();

    // Signal state refresh from outside
    void setDirty();

private:
    LGFX_Tembed* tft;
    LGFX_Sprite* menuSprite;

    bool isDirty;
    int scrW, scrH;
    int originX, originY;

    // View States
    enum class ViewState {
        List,
        Keyboard
    };
    ViewState currentState;

    // List State
    bool showingGeneralCommands;
    std::vector<MenuCommand> currentItems;
    int selectedIndex; // -1 for the [<] back button
    int scrollOffset;

    // Keyboard State
    std::string keyboardBuffer;
    MenuCommand activeCommand;
    int carouselIndex;
    const std::vector<std::string> carouselItems = {
        "A", "B", "C", "D", "E", "F", "G", "H", "I", "J", "K", "L", "M",
        "N", "O", "P", "Q", "R", "S", "T", "U", "V", "W", "X", "Y", "Z",
        "a", "b", "c", "d", "e", "f", "g", "h", "i", "j", "k", "l", "m",
        "n", "o", "p", "q", "r", "s", "t", "u", "v", "w", "x", "y", "z",
        "0", "1", "2", "3", "4", "5", "6", "7", "8", "9",
        " ", ".", ",", "-", "_", ":", "/", "@",
        "<DEL>", "<OK>", "<ESC>"
    };

    void loadItems();
    void renderList();
    void renderKeyboard();
    void renderHeader();
    void renderItem(int index, int y, int w, int h, bool isSelected);

    uint32_t lastRenderMs;
};

#endif

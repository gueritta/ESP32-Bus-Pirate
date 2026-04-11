#if defined(DEVICE_TEMBEDS3) || defined(DEVICE_TEMBEDS3CC1101)

#include "TembedInput.h"
#include "Inputs/InputKeys.h"
#include <esp_sleep.h>
#include <Arduino.h>

TembedInput::TembedInput()
    : encoder(TEMBED_PIN_ENCODE_A, TEMBED_PIN_ENCODE_B, RotaryEncoder::LatchMode::TWO03),
      lastInput(KEY_NONE),
      lastPos(0),
      lastButton(false),
      pressStart(0), screen(nullptr), isSplitScreenActive(true)
{
    encoder.setPosition(0);
    pinMode(TEMBED_PIN_ENCODE_BTN, INPUT_PULLUP);
    pinMode(TEMBED_PIN_SIDE_BTN, INPUT_PULLUP);
}

void TembedInput::tick() {
    encoder.tick();

    int pos = encoder.getPosition();
    if (pos < lastPos) {
        lastInput = KEY_ARROW_LEFT;
        lastPos = pos;
    } else if (pos > lastPos) {
        lastInput = KEY_ARROW_RIGHT;
        lastPos = pos;
    } else if (!digitalRead(TEMBED_PIN_ENCODE_BTN) && !lastButton) {
        lastInput = KEY_OK;
        lastButton = true;
    } else if (digitalRead(TEMBED_PIN_ENCODE_BTN)) {
        lastButton = false;
    }


    // Removed old checkShutdownRequest logic here. Now handled in readChar / processMenuNavigation

}

char TembedInput::readChar() {
    tick();

    // Check for side button long press to shutdown
    if (!digitalRead(TEMBED_PIN_SIDE_BTN)) {
        if (pressStart == 0) {
            pressStart = millis();
        } else if (millis() - pressStart > 2000) {
            shutdownToDeepSleep();
        }
    } else {
        // Also check if central button is long pressed to toggle full screen
        if (!digitalRead(TEMBED_PIN_ENCODE_BTN)) {
            if (pressStart == 0) {
                pressStart = millis();
            } else if (millis() - pressStart > 1000 && lastButton) {
                // Toggle full screen mode
                isSplitScreenActive = !isSplitScreenActive;
                lastButton = false; // consume
                pressStart = 0;

                // We need to trigger redraw somehow.
                // Returning a dummy refresh command like 'system' or just a newline could work,
                // but we also need to inform TerminalView. For now, since TerminalView reads this from
                // the Dispatcher, we can't cleanly toggle TerminalView's splitScreen from here without
                // passing it around. Wait, let's just handle menu navigation.

            }
        } else {
            pressStart = 0;
        }
    }

    // Return buffered command characters if any
    if (!commandBuffer.empty()) {
        char c = commandBuffer[0];
        commandBuffer.erase(0, 1);
        return c;
    }

    processMenuNavigation();

    char c = lastInput;
    lastInput = KEY_NONE;

    // If not in standalone mode, just return the character (though Standalone is what matters)
    if (!isSplitScreenActive) {
        return c;
    }

    return KEY_NONE;
}

void TembedInput::processMenuNavigation() {
    if (lastInput == KEY_ARROW_LEFT) {
        menuUI.scroll(-1);
    } else if (lastInput == KEY_ARROW_RIGHT) {
        menuUI.scroll(1);
    } else if (lastInput == KEY_OK) {
        std::string cmd = menuUI.select();
        if (!cmd.empty()) {
            commandBuffer = cmd;
        }
    }

    // Redraw menu if we are split
    if (isSplitScreenActive && screen) {
        menuUI.render();
    }
}
char TembedInput::handler() {
    while (true) {
        char c = readChar();
        if (c != KEY_NONE) return c;
        delay(5);
    }
}

void TembedInput::waitPress(uint32_t timeoutMs) {
    uint32_t start = millis();
    while (true) {
        if (readChar() != KEY_NONE) return;
        if (timeoutMs > 0 && (millis() - start) >= timeoutMs) return;
        delay(5);
    }
}

void TembedInput::checkShutdownRequest() { return; /* deprecated */ }
void TembedInput::shutdownToDeepSleep() {
    delay(2000);
    esp_sleep_enable_ext0_wakeup((gpio_num_t)TEMBED_PIN_SIDE_BTN, 0);
    esp_deep_sleep_start();
}

#endif

// util.cpp - FreeRTOS-cooperative helpers

#include "util.h"
#include "input.h"
#include "terminal.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

namespace Util {

void coopDelay(unsigned long ms) {
    if (ms == 0) {
        taskYIELD();
        return;
    }
    vTaskDelay(pdMS_TO_TICKS(ms));
}

bool waitForSelectOrStop() {
    while (true) {
        if (Input::isButtonPressed(BUTTON_SELECT)) return true;
        if (Terminal::stopRequested()) return false;
        vTaskDelay(pdMS_TO_TICKS(20));
    }
}

uint8_t waitForAnyButtonOrStop() {
    while (true) {
        if (Input::isButtonPressed(BUTTON_SELECT)) return BUTTON_SELECT;
        if (Input::isButtonPressed(BUTTON_UP))     return BUTTON_UP;
        if (Input::isButtonPressed(BUTTON_DOWN))   return BUTTON_DOWN;
        if (Terminal::stopRequested()) return 0;
        vTaskDelay(pdMS_TO_TICKS(20));
    }
}

bool isLongPressSelect(unsigned long ms) {
    if (!Input::isButtonPressed(BUTTON_SELECT)) return false;
    unsigned long start = millis();
    while (Input::isButtonPressed(BUTTON_SELECT)) {
        if (millis() - start >= ms) return true;
        vTaskDelay(pdMS_TO_TICKS(10));
    }
    return false;
}

}  // namespace Util

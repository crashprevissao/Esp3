// util.h - FreeRTOS-cooperative helpers used across modules.
// These replace the V1 spin-and-yield idioms with vTaskDelay-based waits
// so other tasks (serial bridge on Core 0, web server, etc.) get CPU time.

#ifndef STARBEAM_UTIL_H
#define STARBEAM_UTIL_H

#include <Arduino.h>
#include "../config.h"

namespace Util {

// Cooperative delay. Replaces the .ino-local nonBlockingDelay().
void coopDelay(unsigned long ms);

// Block until SELECT is pressed or Terminal::stopRequested() goes true.
// Returns true if exited because of SELECT, false if exited because of stop.
bool waitForSelectOrStop();

// Block until any button is pressed or stop. Returns the pin pressed, or 0
// if stop. Useful for "press any key" prompts.
uint8_t waitForAnyButtonOrStop();

// Detect a long-press of SELECT (>= ms). Non-blocking-ish: returns true when
// the long-press condition is reached. Yields cooperatively while polling.
bool isLongPressSelect(unsigned long ms = LONG_PRESS_MS);

}  // namespace Util

#endif

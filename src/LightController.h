#pragma once
#include <cstdint>

namespace LightController {

void begin();

// Renders `base` color, corrected for the given hour-of-day/month-of-year
// (see core/LightLogic.h), onto the LED strip. Returns true if night mode
// is currently active (dim red — caller is expected to blink it).
bool render(uint8_t r, uint8_t g, uint8_t b, int hour, int month);

void allOff();
void pulse(uint8_t r, uint8_t g, uint8_t b, uint8_t phase);
void fadeTo(uint8_t r, uint8_t g, uint8_t b, int durationMs = 3000);

} // namespace LightController

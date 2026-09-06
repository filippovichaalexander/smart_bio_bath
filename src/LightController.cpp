#include "LightController.h"
#include "Config.h"
#include "core/LightLogic.h"
#include <Arduino.h>
#include <FastLED.h>

namespace LightController {

namespace {
CRGB leds[NUM_LEDS];
}

void begin() {
  FastLED.addLeds<WS2812, PIN_RGB, GRB>(leds, NUM_LEDS);
  FastLED.setBrightness(255);
  allOff();
}

bool render(uint8_t r, uint8_t g, uint8_t b, int hour, int month) {
  LightResult result = computeLight({r, g, b}, hour, month, NIGHT_START_HOUR, NIGHT_END_HOUR);
  fill_solid(leds, NUM_LEDS, CRGB(result.color.r, result.color.g, result.color.b));
  FastLED.show();
  return result.nightMode;
}

void allOff() {
  fill_solid(leds, NUM_LEDS, CRGB::Black);
  FastLED.show();
}

void pulse(uint8_t r, uint8_t g, uint8_t b, uint8_t phase) {
  fill_solid(leds, NUM_LEDS, CRGB(r * phase / 255, g * phase / 255, b * phase / 255));
  FastLED.show();
}

void fadeTo(uint8_t r, uint8_t g, uint8_t b, int durationMs) {
  const int steps = 30;
  for (int s = 0; s <= steps; s++) {
    float progress = (float)s / steps;
    for (int i = 0; i < NUM_LEDS; i++) {
      leds[i] = CRGB(
          leds[i].r + (r - leds[i].r) * progress,
          leds[i].g + (g - leds[i].g) * progress,
          leds[i].b + (b - leds[i].b) * progress);
    }
    FastLED.show();
    delay(durationMs / steps);
  }
}

} // namespace LightController

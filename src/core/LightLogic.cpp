#include "LightLogic.h"
#include <algorithm>

LightResult computeLight(RGB base, int hour, int month, int nightStartHour, int nightEndHour) {
  int r = base.r, g = base.g, b = base.b;
  int brightness = 255;
  bool nightMode = false;

  if (hour >= nightStartHour && hour < nightEndHour) {
    nightMode = true;
    brightness = 80;
    r = 255; g = 0; b = 0;
  } else {
    if (hour >= 6 && hour < 10) {
      brightness = 200; // morning: brighter
    } else if (hour >= 17 && hour < 22) {
      r = std::min(255, r + 30); // evening: warmer
      g = std::max(0, g - 20);
      b = std::max(0, b - 30);
      brightness = 180;
    } else {
      brightness = 255;
    }

    if (month == 12 || month <= 2) {
      r = std::min(255, r + 20); // winter: warm shift
      g = std::max(0, g - 10);
      b = std::max(0, b - 20);
    } else if (month >= 6 && month <= 8) {
      r = std::max(0, r - 10); // summer: cool shift
      g = std::min(255, g + 10);
      b = std::min(255, b + 20);
    }
  }

  RGB finalColor{
      static_cast<uint8_t>(r * brightness / 255),
      static_cast<uint8_t>(g * brightness / 255),
      static_cast<uint8_t>(b * brightness / 255),
  };
  return LightResult{finalColor, static_cast<uint8_t>(brightness), nightMode};
}

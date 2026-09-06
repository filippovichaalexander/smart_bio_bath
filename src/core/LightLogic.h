#pragma once
#include <cstdint>

struct RGB {
  uint8_t r, g, b;
};

struct LightResult {
  RGB color;          // final color, brightness already applied
  uint8_t brightness; // 0-255 brightness that was applied
  bool nightMode;      // true if the night-mode (dim red) branch was taken
};

// Recomputes the lamp output for `base` color at the given hour-of-day
// (0-23) and month-of-year (1-12), applying the night/morning/evening and
// winter/summer corrections from docs/ORIGINAL_SPEC.md ("LightController").
// Pure function — no hardware access, safe to unit-test on a host machine.
LightResult computeLight(RGB base, int hour, int month, int nightStartHour, int nightEndHour);

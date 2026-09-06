#pragma once
#include <cstdint>

// One gait "fingerprint": 7 parameters extracted from a single footstep's
// piezo signal. See docs/ORIGINAL_SPEC.md ("FFT-analyzer") for how these are
// derived from the raw waveform.
struct StepParams {
  float duration;      // step length, seconds
  float amplitude;     // peak amplitude, volts
  float slope;         // steepest rising edge, volts/second
  float microVib;      // micro-vibration count within the step
  float interval;      // time since the previous step, seconds
  float freq200_400;   // average spectral energy in the 200-400 Hz band
  float startRatio;    // signal/noise ratio at step onset
};

struct UserProfile {
  char name[20];
  StepParams params;
  uint8_t r, g, b;      // favorite light color
  uint32_t lastSeen;    // unix timestamp of last recognized visit
  bool active;
};

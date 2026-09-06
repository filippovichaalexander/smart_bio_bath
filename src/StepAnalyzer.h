#pragma once
#include "core/StepParams.h"

namespace StepAnalyzer {

// Captures FFT_SAMPLES samples from PIN_PIEZO (~256ms at 1kHz) and extracts
// the 7-parameter gait fingerprint: duration, peak amplitude, rising-edge
// slope, micro-vibration count, inter-step interval, 200-400Hz spectral
// energy, and onset signal/noise ratio. Blocking — call only once a step's
// rising edge has already been detected on the analog pin.
StepParams analyzeStep();

} // namespace StepAnalyzer

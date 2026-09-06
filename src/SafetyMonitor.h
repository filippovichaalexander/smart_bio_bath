#pragma once

namespace SafetyMonitor {

void begin();

// Call every loop iteration. Sounds the buzzer and returns true once the
// microphone has detected continuous water noise for longer than
// WATER_TIMEOUT_S; returns false (and silences the buzzer) as soon as the
// noise stops.
bool checkWaterLeak();

} // namespace SafetyMonitor

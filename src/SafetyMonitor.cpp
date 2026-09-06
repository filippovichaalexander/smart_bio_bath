#include "SafetyMonitor.h"
#include "Config.h"
#include <Arduino.h>

namespace SafetyMonitor {

namespace {
unsigned long waterStartTime = 0;
constexpr int MIC_NOISE_THRESHOLD = 500;
}

void begin() {
  pinMode(PIN_BUZZER, OUTPUT);
  pinMode(PIN_MIC, INPUT);
  digitalWrite(PIN_BUZZER, LOW);
}

bool checkWaterLeak() {
  int micValue = analogRead(PIN_MIC);
  if (micValue <= MIC_NOISE_THRESHOLD) {
    waterStartTime = 0;
    digitalWrite(PIN_BUZZER, LOW);
    return false;
  }

  if (waterStartTime == 0) waterStartTime = millis();
  bool alarm = (millis() - waterStartTime) > (unsigned long)WATER_TIMEOUT_S * 1000;
  digitalWrite(PIN_BUZZER, alarm ? HIGH : LOW);
  return alarm;
}

} // namespace SafetyMonitor

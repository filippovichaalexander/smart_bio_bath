#include "BioAdapter.h"
#include "Config.h"
#include <Arduino.h>

namespace BioAdapter {

void begin() {
  pinMode(PIN_BIO_STIM, OUTPUT);
  digitalWrite(PIN_BIO_STIM, LOW);
}

void punish() {
  Serial.println("BioAdapter: punish pulse (\"No\" button pressed)");
  digitalWrite(PIN_BIO_STIM, HIGH);
  delay(200);
  digitalWrite(PIN_BIO_STIM, LOW);
}

void reward() {
  Serial.println("BioAdapter: reward pulse (correct recognition)");
  digitalWrite(PIN_BIO_STIM, HIGH);
  delay(50);
  digitalWrite(PIN_BIO_STIM, LOW);
}

} // namespace BioAdapter

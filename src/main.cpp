/*
  ============================================================================
  SMART BIO-BATHROOM — ESP32 + Physarum polycephalum wetware
  ============================================================================
  Fully offline: no WiFi/BLE/cloud code path. The Physarum plasmodium
  wired to PIN_BIO_STIM is the adaptive "brain" that learns each person's
  reinforcement history; this firmware is only the interface between it and
  the piezo gait sensor, RGB light, voice module, and safety sensors.
  See docs/ORIGINAL_SPEC.md for the design conversation this was built from,
  docs/HARDWARE.md for wiring, and docs/WETWARE.md for the biological setup.
  ============================================================================
*/
#include <Arduino.h>
#include <RTClib.h>
#include <SD.h>
#include <SPI.h>

#include "Config.h"
#include "core/StepParams.h"
#include "StepAnalyzer.h"
#include "ProfileManager.h"
#include "LightController.h"
#include "VoiceEngine.h"
#include "BioAdapter.h"
#include "SafetyMonitor.h"

namespace {
RTC_DS3231 rtc;
SPIClass sdSPI(HSPI);

int currentUserID = -1;
unsigned long lastStepTime = 0;
unsigned long lastForgetSweep = 0;
unsigned long lastMoveTime = 0;
bool nightBlinkOn = false;

void runLearningFlow(const String &name) {
  VoiceEngine::say("take_5_steps");
  delay(3000);

  StepParams avg = {0, 0, 0, 0, 0, 0, 0};
  for (int s = 0; s < STEPS_TO_LEARN; s++) {
    StepParams p = StepAnalyzer::analyzeStep();
    avg.duration += p.duration;
    avg.amplitude += p.amplitude;
    avg.slope += p.slope;
    avg.microVib += p.microVib;
    avg.interval += p.interval;
    avg.freq200_400 += p.freq200_400;
    avg.startRatio += p.startRatio;
    delay(500);
  }
  avg.duration /= STEPS_TO_LEARN;
  avg.amplitude /= STEPS_TO_LEARN;
  avg.slope /= STEPS_TO_LEARN;
  avg.microVib /= STEPS_TO_LEARN;
  avg.interval /= STEPS_TO_LEARN;
  avg.freq200_400 /= STEPS_TO_LEARN;
  avg.startRatio /= STEPS_TO_LEARN;

  int id = ProfileManager::createProfile(name.c_str(), avg, 150, 200, 255, rtc.now().unixtime());
  if (id >= 0) {
    VoiceEngine::say("profile_saved");
  } else {
    VoiceEngine::say("no_room_for_profile");
  }
}

void handleVoiceCommand(const String &cmd) {
  if (cmd.startsWith("Запомни меня как ")) {
    runLearningFlow(cmd.substring(18));
  } else if (cmd.startsWith("Мой цвет")) {
    int id = ProfileManager::findUser(StepAnalyzer::analyzeStep());
    if (id < 0) return;
    UserProfile &p = ProfileManager::get(id);
    if (cmd.indexOf("син") > 0) { p.r = 0; p.g = 100; p.b = 255; }
    else if (cmd.indexOf("красн") > 0) { p.r = 255; p.g = 0; p.b = 0; }
    else if (cmd.indexOf("зелен") > 0) { p.r = 0; p.g = 255; p.b = 0; }
    else if (cmd.indexOf("желт") > 0) { p.r = 255; p.g = 200; p.b = 0; }
    else if (cmd.indexOf("фиолетов") > 0) { p.r = 150; p.g = 0; p.b = 255; }
    else return;
    VoiceEngine::say("color_changed");
  } else if (cmd == "Удали меня") {
    int id = ProfileManager::findUser(StepAnalyzer::analyzeStep());
    if (id >= 0) {
      ProfileManager::deleteProfile(id);
      VoiceEngine::say("profile_deleted");
    }
  }
}
} // namespace

void setup() {
  Serial.begin(115200);
  Serial.println("=== Smart Bio-Bathroom ===");

  if (!rtc.begin()) Serial.println("RTC not found!");
  if (rtc.lostPower()) rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));

  sdSPI.begin(PIN_SD_SCK, PIN_SD_MISO, PIN_SD_MOSI, PIN_SD_CS);
  if (!SD.begin(PIN_SD_CS, sdSPI)) Serial.println("SD card not found!");

  LightController::begin();
  VoiceEngine::begin();
  BioAdapter::begin();
  SafetyMonitor::begin();
  ProfileManager::begin();

  pinMode(PIN_BTN_NO, INPUT_PULLUP);

  // Default "Host" profile so the system is usable before anyone trains it.
  StepParams defaultParams = {0.3f, 2.5f, 6.0f, 12.0f, 0.5f, 320.0f, 0.9f};
  ProfileManager::createProfile("Host", defaultParams, 255, 255, 255, rtc.now().unixtime());

  VoiceEngine::say("welcome");
  Serial.println("System ready.");
}

void loop() {
  DateTime now = rtc.now();
  int piezoValue = analogRead(PIN_PIEZO);

  // 1. Gait recognition
  if (piezoValue > PIEZO_TRIGGER_THRESHOLD) {
    unsigned long t = millis();
    if (t - lastStepTime > STEP_DEBOUNCE_MS) {
      lastStepTime = t;

      StepParams p = StepAnalyzer::analyzeStep();
      int userID = ProfileManager::findUser(p);

      if (userID >= 0) {
        ProfileManager::touchLastSeen(userID, now.unixtime());
        currentUserID = userID;
        UserProfile &profile = ProfileManager::get(userID);
        nightBlinkOn = LightController::render(profile.r, profile.g, profile.b, now.hour(), now.month());
        Serial.printf("Recognized: %s\n", profile.name);
        BioAdapter::reward();
      } else {
        currentUserID = -1;
        nightBlinkOn = LightController::render(255, 150, 0, now.hour(), now.month());
        Serial.println("Unknown visitor");
        VoiceEngine::say("unknown_visitor");
      }
    }
  }

  // 2. "No" button -> negative reinforcement
  if (digitalRead(PIN_BTN_NO) == LOW) {
    BioAdapter::punish();
    delay(500); // debounce
  }

  // 3. Voice commands
  String cmd = VoiceEngine::readCommand();
  if (cmd.length() > 0) handleVoiceCommand(cmd);

  // 4. Water-leak detector
  if (SafetyMonitor::checkWaterLeak()) {
    VoiceEngine::say("leak_warning");
  }

  // 5. Night-mode blink
  if (nightBlinkOn && currentUserID >= 0) {
    static unsigned long lastBlink = 0;
    static bool blinkState = false;
    if (millis() - lastBlink > 500) {
      lastBlink = millis();
      blinkState = !blinkState;
      if (blinkState) {
        UserProfile &profile = ProfileManager::get(currentUserID);
        LightController::render(profile.r, profile.g, profile.b, now.hour(), now.month());
      } else {
        LightController::allOff();
      }
    }
  }

  // 6. Daily stale-profile sweep
  if (millis() - lastForgetSweep > 86400000UL) {
    ProfileManager::forgetStale(now.unixtime());
    lastForgetSweep = millis();
  }

  // 7. Idle pulse (breathing light after standing still for 30s)
  if (currentUserID >= 0) {
    if (piezoValue < PIEZO_TRIGGER_THRESHOLD) {
      if (millis() - lastMoveTime > 30000) {
        static uint8_t phase = 0;
        phase++;
        UserProfile &profile = ProfileManager::get(currentUserID);
        LightController::pulse(profile.r, profile.g, profile.b, phase);
      }
    } else {
      lastMoveTime = millis();
    }
  }

  delay(100);
}

#include "ProfileManager.h"
#include "Config.h"
#include "core/CosineMatch.h"
#include <Arduino.h>
#include <cstring>

namespace ProfileManager {

namespace {
UserProfile profiles[MAX_PROFILES];
}

void begin() {
  for (auto &p : profiles) p.active = false;
}

int findUser(const StepParams &sample) {
  return findClosestProfile(sample, profiles, MAX_PROFILES, MATCH_THRESHOLD);
}

int createProfile(const char *name, const StepParams &params, uint8_t r, uint8_t g, uint8_t b, uint32_t nowUnix) {
  for (int i = 0; i < MAX_PROFILES; i++) {
    if (!profiles[i].active) {
      strncpy(profiles[i].name, name, sizeof(profiles[i].name) - 1);
      profiles[i].name[sizeof(profiles[i].name) - 1] = '\0';
      profiles[i].params = params;
      profiles[i].r = r;
      profiles[i].g = g;
      profiles[i].b = b;
      profiles[i].lastSeen = nowUnix;
      profiles[i].active = true;
      return i;
    }
  }
  return -1;
}

void deleteProfile(int id) {
  if (id >= 0 && id < MAX_PROFILES) profiles[id].active = false;
}

void touchLastSeen(int id, uint32_t nowUnix) {
  if (id >= 0 && id < MAX_PROFILES) profiles[id].lastSeen = nowUnix;
}

void forgetStale(uint32_t nowUnix) {
  for (int i = 0; i < MAX_PROFILES; i++) {
    if (profiles[i].active && (nowUnix - profiles[i].lastSeen) > (uint32_t)FORGET_DAYS * 86400) {
      profiles[i].active = false;
      Serial.printf("Forgot stale profile: %s\n", profiles[i].name);
    }
  }
}

UserProfile &get(int id) {
  return profiles[id];
}

} // namespace ProfileManager

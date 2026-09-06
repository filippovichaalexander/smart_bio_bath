#pragma once
#include "core/StepParams.h"

namespace ProfileManager {

void begin();

// Finds the closest active profile matching `sample`, or -1 if none is
// within MATCH_THRESHOLD. Delegates to core/CosineMatch.
int findUser(const StepParams &sample);

// Creates a new profile timestamped `nowUnix`. Returns its slot index, or
// -1 if the MAX_PROFILES table is full.
int createProfile(const char *name, const StepParams &params, uint8_t r, uint8_t g, uint8_t b, uint32_t nowUnix);

void deleteProfile(int id);
void touchLastSeen(int id, uint32_t nowUnix);

// Deactivates any profile not seen for more than FORGET_DAYS.
void forgetStale(uint32_t nowUnix);

UserProfile &get(int id);

} // namespace ProfileManager

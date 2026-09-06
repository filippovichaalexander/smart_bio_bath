#include "CosineMatch.h"
#include <cmath>

float cosineDistance(const StepParams &a, const StepParams &b) {
  const float *pa = reinterpret_cast<const float *>(&a);
  const float *pb = reinterpret_cast<const float *>(&b);
  float dot = 0, magA = 0, magB = 0;
  for (int i = 0; i < 7; i++) {
    dot += pa[i] * pb[i];
    magA += pa[i] * pa[i];
    magB += pb[i] * pb[i];
  }
  if (magA == 0.0f || magB == 0.0f) {
    return 1.0f;
  }
  return 1.0f - (dot / (std::sqrt(magA) * std::sqrt(magB)));
}

int findClosestProfile(const StepParams &sample, const UserProfile *profiles, int count, float threshold) {
  int best = -1;
  float bestDist = threshold;
  for (int i = 0; i < count; i++) {
    if (!profiles[i].active) continue;
    float dist = cosineDistance(sample, profiles[i].params);
    if (dist < bestDist) {
      bestDist = dist;
      best = i;
    }
  }
  return best;
}

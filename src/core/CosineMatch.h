#pragma once
#include "StepParams.h"

// 1 - cosine similarity between two gait fingerprints (0 = identical
// direction, 1 = orthogonal, 2 = opposite). Degenerate (all-zero) vectors
// are treated as maximally dissimilar rather than dividing by zero.
float cosineDistance(const StepParams &a, const StepParams &b);

// Scans the first `count` entries of `profiles` and returns the index of the
// *closest* active match whose distance is below `threshold`, or -1 if none
// qualifies. (Picking the closest rather than the first match under
// threshold avoids misidentifying a user when two profiles are both within
// range.)
int findClosestProfile(const StepParams &sample, const UserProfile *profiles, int count, float threshold);

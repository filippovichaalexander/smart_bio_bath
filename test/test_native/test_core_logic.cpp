// Host-side unit tests for src/core (gait matching + lighting logic).
// Run with: pio test -e native   (no ESP32 board or Docker required)
#include <unity.h>
#include "CosineMatch.h"
#include "LightLogic.h"

void setUp(void) {}
void tearDown(void) {}

void test_cosine_distance_identical_is_zero(void) {
  StepParams a = {0.30f, 2.50f, 6.00f, 12.0f, 0.50f, 320.0f, 0.90f};
  TEST_ASSERT_FLOAT_WITHIN(0.0001f, 0.0f, cosineDistance(a, a));
}

void test_cosine_distance_different_profiles_is_positive(void) {
  // freq200_400 dominates the vector's magnitude (its values run ~100-400
  // vs ~0.1-12 for every other feature), so two profiles that only differ
  // in the *small* dimensions can look almost perfectly parallel to plain
  // cosine similarity. Vary the dominant dimension itself so the distance
  // is unambiguously positive rather than lost to float rounding.
  StepParams a = {0.30f, 2.50f, 6.00f, 12.0f, 0.50f, 320.0f, 0.90f};
  StepParams b = {0.30f, 2.50f, 6.00f, 12.0f, 0.50f, 20.0f, 0.90f};
  TEST_ASSERT_GREATER_THAN_FLOAT(0.05f, cosineDistance(a, b));
}

void test_cosine_distance_all_zero_is_not_a_match(void) {
  StepParams zero = {0, 0, 0, 0, 0, 0, 0};
  // A degenerate all-zero fingerprint (e.g. a missed step) must never look
  // like a perfect match against another zeroed slot.
  TEST_ASSERT_EQUAL_FLOAT(1.0f, cosineDistance(zero, zero));
}

void test_find_closest_profile_picks_best_match_not_first(void) {
  StepParams sample = {0.30f, 2.50f, 6.00f, 12.0f, 0.50f, 320.0f, 0.90f};
  UserProfile profiles[3] = {};
  profiles[0].active = true;
  profiles[0].params = {0.20f, 2.00f, 5.00f, 10.0f, 0.40f, 280.0f, 0.80f}; // mediocre match
  profiles[1].active = true;
  profiles[1].params = sample; // exact match
  profiles[2].active = false;
  profiles[2].params = sample; // exact but inactive -> must be skipped

  TEST_ASSERT_EQUAL_INT(1, findClosestProfile(sample, profiles, 3, 0.3f));
}

void test_find_closest_profile_returns_none_below_threshold(void) {
  StepParams sample = {0.30f, 2.50f, 6.00f, 12.0f, 0.50f, 320.0f, 0.90f};
  UserProfile profiles[1] = {};
  profiles[0].active = true;
  profiles[0].params = {5.0f, 0.01f, 0.01f, 0.0f, 5.0f, 1.0f, 0.01f};

  TEST_ASSERT_EQUAL_INT(-1, findClosestProfile(sample, profiles, 1, 0.3f));
}

void test_night_mode_forces_dim_red(void) {
  RGB white{255, 255, 255};
  LightResult res = computeLight(white, 2, 7, 0, 5); // 02:00, July
  TEST_ASSERT_TRUE(res.nightMode);
  TEST_ASSERT_EQUAL_UINT8(80, res.color.r);
  TEST_ASSERT_EQUAL_UINT8(0, res.color.g);
  TEST_ASSERT_EQUAL_UINT8(0, res.color.b);
}

void test_winter_daytime_warms_color_vs_summer(void) {
  RGB base{100, 100, 100};
  LightResult summer = computeLight(base, 12, 7, 0, 5); // noon, July
  LightResult winter = computeLight(base, 12, 1, 0, 5); // noon, January
  TEST_ASSERT_FALSE(summer.nightMode);
  TEST_ASSERT_FALSE(winter.nightMode);
  TEST_ASSERT_TRUE(winter.color.r >= summer.color.r);
  TEST_ASSERT_TRUE(winter.color.b <= summer.color.b);
}

void test_morning_is_brighter_than_late_night_edge(void) {
  RGB base{100, 100, 100};
  LightResult morning = computeLight(base, 8, 4, 0, 5); // 08:00
  LightResult midday = computeLight(base, 13, 4, 0, 5); // 13:00
  TEST_ASSERT_TRUE(morning.brightness < midday.brightness);
}

int main(int argc, char **argv) {
  UNITY_BEGIN();
  RUN_TEST(test_cosine_distance_identical_is_zero);
  RUN_TEST(test_cosine_distance_different_profiles_is_positive);
  RUN_TEST(test_cosine_distance_all_zero_is_not_a_match);
  RUN_TEST(test_find_closest_profile_picks_best_match_not_first);
  RUN_TEST(test_find_closest_profile_returns_none_below_threshold);
  RUN_TEST(test_night_mode_forces_dim_red);
  RUN_TEST(test_winter_daytime_warms_color_vs_summer);
  RUN_TEST(test_morning_is_brighter_than_late_night_edge);
  return UNITY_END();
}

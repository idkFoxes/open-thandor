#pragma once

#include <stdint.h>

inline constexpr uint32_t RANDOM_LCG_MULTIPLIER = 33U;
inline constexpr uint32_t RANDOM_LCG_INCREMENT = 101U;
inline constexpr uint32_t RANDOM_OUTPUT_FIRST_STEP_SCALE = 0x4000U;

/* Pure form of Thandor's recovered two-step random generator.  The live game
   supplies its primary/secondary seed; tools such as Simulation Lab can keep a
   private seed while executing the exact same state transition. */
inline uint32_t Random_NextFromSeed(uint32_t *seed) noexcept
{
  const uint32_t firstStepSeed = *seed * RANDOM_LCG_MULTIPLIER + RANDOM_LCG_INCREMENT;
  *seed = firstStepSeed * RANDOM_LCG_MULTIPLIER + RANDOM_LCG_INCREMENT;
  return (firstStepSeed * RANDOM_OUTPUT_FIRST_STEP_SCALE) ^ (*seed >> 2U);
}

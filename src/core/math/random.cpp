/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/src/core/math/random.cpp
 * Reverse engineering by idkFoxes 2026
 */

#include <thandor/core/math/random.h>
#include <thandor/thandor.h>
#include <thandor/platform/debug/hooks.h>

/* Module data. */

RandomGeneratorState g_RandomGeneratorState = {.next = THANDOR_SLOT(Random_NextPrimary), .primarySeed = 0x198F};

/* Implementation ownership: core/math/random. */

/* Primary random stream (the default g_RandomGeneratorState.next): steps the linear congruential seed
   twice and returns (first step << 14) ^ (second step >> 2), mixing both steps so the weak low bits of
   the LCG do not show up directly.
*/
uint32_t Random_NextPrimary()

{
  uint32_t firstStepSeed; /* unsigned: the steps wrap modulo 2^32 like the original's 32-bit arithmetic */

  firstStepSeed = g_RandomGeneratorState.primarySeed * RANDOM_LCG_MULTIPLIER + RANDOM_LCG_INCREMENT;
  g_RandomGeneratorState.primarySeed = firstStepSeed * RANDOM_LCG_MULTIPLIER + RANDOM_LCG_INCREMENT;
  return (firstStepSeed * RANDOM_OUTPUT_FIRST_STEP_SCALE) ^ (g_RandomGeneratorState.primarySeed >> 2);
}


/* Secondary random stream (selected by Random_SelectSecondaryStream for sessions): the same two LCG steps
   and output mix as Random_NextPrimary, but on the separate secondary seed, so the session stream can be
   kept in step across machines independently of the primary one.
*/
uint32_t Random_NextSecondary()

{
  uint32_t firstStepSeed; /* unsigned: the steps wrap modulo 2^32 like the original's 32-bit arithmetic */

  DebugHook_NoteOutsideStep("session random draw");
  firstStepSeed = g_RandomGeneratorState.secondarySeed * RANDOM_LCG_MULTIPLIER + RANDOM_LCG_INCREMENT;
  g_RandomGeneratorState.secondarySeed = firstStepSeed * RANDOM_LCG_MULTIPLIER + RANDOM_LCG_INCREMENT;
  return (firstStepSeed * RANDOM_OUTPUT_FIRST_STEP_SCALE) ^ (g_RandomGeneratorState.secondarySeed >> 2);
}


/* Sets the primary and the secondary seed to the same value without changing the active stream. A new
   session seeds both from one value, which network clients receive from the host, so that every machine
   draws the same numbers.
*/
void Random_SetBothSeeds(RandomSeed seed)

{
  g_RandomGeneratorState.primarySeed = seed;
  g_RandomGeneratorState.secondarySeed = seed;
}


/* Returns the current secondary seed (without stepping it); the host sends it in the player snapshot
   packet so that joining machines can continue the same stream.
*/
uint32_t __cdecl Random_GetSecondarySeed()

{
  return g_RandomGeneratorState.secondarySeed;
}

/* Makes Random_NextSecondary the active generator (g_RandomGeneratorState.next) without touching either seed;
   used together with Random_SetBothSeeds when a session starts.
*/
void Random_SelectSecondaryStream()

{
  g_RandomGeneratorState.next = Random_NextSecondary;
}


/* Makes Random_NextPrimary the active generator (g_RandomGeneratorState.next) again without touching
   either seed; the front end calls it when a session is left, undoing Random_SelectSecondaryStream.
*/
void __cdecl Random_SelectPrimaryStream()

{
  g_RandomGeneratorState.next = Random_NextPrimary;
  return;
}

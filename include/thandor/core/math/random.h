/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/include/thandor/core/math/random.h
 * Reverse engineering by idkFoxes 2026
 */

#ifndef THANDOR_CORE_MATH_RANDOM_H
#define THANDOR_CORE_MATH_RANDOM_H

#include <thandor/core/math/types.h>
#include <thandor/core/types.h>
#include <thandor/core/contracts.h>

/* Both random streams step their seed as seed = seed * 33 + 101 (mod 2^32), see Random_NextPrimary. */
#define RANDOM_LCG_MULTIPLIER 33
#define RANDOM_LCG_INCREMENT 101
/* The output is (first step * 2^14) ^ (second step >> 2): the first step's factor */
#define RANDOM_OUTPUT_FIRST_STEP_SCALE 0x4000

uint32_t Random_NextPrimary();

uint32_t Random_NextSecondary();

void Random_SetBothSeeds(RandomSeed seed);

uint32_t __cdecl Random_GetSecondarySeed();

void Random_SelectSecondaryStream();

void __cdecl Random_SelectPrimaryStream();

extern RandomGeneratorState g_RandomGeneratorState;

#endif /* THANDOR_CORE_MATH_RANDOM_H */

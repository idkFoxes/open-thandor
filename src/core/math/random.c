/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/src/core/math/random.c
 * Reverse engineering by idkFoxes 2026
 */

#include <thandor/core/math/random.h>

/* Implementation ownership: core/math/random. */

/* Address: 0x004030B0.
   Ownership: core/math/random.
   Purpose: Advances RandomGeneratorState.primarySeed twice with 32-bit wraparound using state=state*33+101. The
   first advanced value is shifted left 14 bits, the second becomes the stored seed and is shifted right 2 bits,
   and the two values are XORed for the return value.
*/
dword __thandor_eax_preserve_ecx_edx Random_NextPrimary(void)

{
  int firstGeneratorStepState;
  
  firstGeneratorStepState = g_RandomGeneratorState.primarySeed * 0x21 + 0x65;
  g_RandomGeneratorState.primarySeed = firstGeneratorStepState * 0x21 + 0x65;
  return firstGeneratorStepState * 0x4000 ^ g_RandomGeneratorState.primarySeed >> 2;
}


/* Address: 0x004030E0.
   Ownership: core/math/random.
   Purpose: Uses the same two-step state=state*33+101 recurrence and output mix as Random_NextPrimary, but reads
   and writes only RandomGeneratorState.secondarySeed.
*/
dword __thandor_eax_preserve_ecx_edx Random_NextSecondary(void)

{
  int firstGeneratorStepState;
  
  firstGeneratorStepState = g_RandomGeneratorState.secondarySeed * 0x21 + 0x65;
  g_RandomGeneratorState.secondarySeed = firstGeneratorStepState * 0x21 + 0x65;
  return firstGeneratorStepState * 0x4000 ^ g_RandomGeneratorState.secondarySeed >> 2;
}


/* Address: 0x00403110.
   Ownership: core/math/random.
   Purpose: It does not change the active RandomGeneratorState.next selector.
*/
void __thandor_void_preserve_eax_ecx_edx Random_SetBothSeeds(RandomSeed seed)

{
  g_RandomGeneratorState.primarySeed = seed;
  g_RandomGeneratorState.secondarySeed = seed;
  return;
}


/* Address: 0x00403130.
   Ownership: core/math/random.
   Purpose: Returns RandomGeneratorState.secondarySeed. It does not return primarySeed or invoke the active
   generator.
*/
dword __cdecl Random_GetSecondarySeed(void)

{
  return g_RandomGeneratorState.secondarySeed;
}

/* Address: 0x00403140.
   Ownership: core/math/random.
   Purpose: Sets RandomGeneratorState.next to Random_NextSecondary without changing either seed.
*/
void __thandor_void_preserve_eax_ecx_edx Random_SelectSecondaryStream(void)

{
  g_RandomGeneratorState.next = Random_NextSecondary;
  return;
}


/* Address: 0x00403150.
   Ownership: core/math/random.
   Purpose: Sets RandomGeneratorState.next to Random_NextPrimary without changing either seed.
*/
void __cdecl Random_SelectPrimaryStream(void)

{
  g_RandomGeneratorState.next = Random_NextPrimary;
  return;
}

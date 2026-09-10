#ifndef THANDOR_CORE_MATH_RANDOM_H
#define THANDOR_CORE_MATH_RANDOM_H

#include <thandor/generated/v523_types.h>
#include <thandor/core/contracts.h>

/* Submodule: core/math/random. */
/* Functions are grouped by semantic ownership; address comments are executable virtual addresses. */

/* 0x004030B0 */
dword __cdecl Random_NextPrimary(void);

/* 0x004030E0 */
dword __cdecl Random_NextSecondary(void);

/* 0x00403110 */
void Random_SetBothSeeds(RandomSeed seed);

/* 0x00403130 */
dword __cdecl Random_GetSecondarySeed(void);

/* 0x00403140 */
void __cdecl Random_SelectSecondaryStream(void);

/* 0x00403150 */
void __cdecl Random_SelectPrimaryStream(void);

#endif /* THANDOR_CORE_MATH_RANDOM_H */

/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/include/thandor/core/math/interpolation.h
 * Reverse engineering by idkFoxes 2026
 */

#ifndef THANDOR_CORE_MATH_INTERPOLATION_H
#define THANDOR_CORE_MATH_INTERPOLATION_H

#include <thandor/generated/types.h>
#include <thandor/core/contracts.h>

/* Submodule: core/math/interpolation. */

/* Bit 31 as an unsigned int: (uint32_t)value < INTERPOLATION_SIGN_BIT tests a signed value for >= 0 (the
   original's unsigned compare) */
#define INTERPOLATION_SIGN_BIT 0x80000000

/* Functions are grouped by semantic ownership. */

void InterpolationState_SetNegatedTargetAndRescaleProgress
          (GraphicsTransitionTickCount fadeOutTicks,GraphicsShadingRuntimeRecord *shadingRecord);

void InterpolationStateTable_Advance256ByTicks(GraphicsElapsedTickCount elapsedTicks);

#endif /* THANDOR_CORE_MATH_INTERPOLATION_H */

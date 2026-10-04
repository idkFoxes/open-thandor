/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/include/thandor/graphics/render/light_transitions.h
 * Reverse engineering by idkFoxes 2026
 */

#ifndef THANDOR_GRAPHICS_RENDER_LIGHT_TRANSITIONS_H
#define THANDOR_GRAPHICS_RENDER_LIGHT_TRANSITIONS_H

#include <thandor/graphics/render/types.h>
#include <thandor/core/contracts.h>

/* Bit 31 as an unsigned int: (uint32_t)value < INTERPOLATION_SIGN_BIT tests a signed value for >= 0 (the
   original's unsigned compare) */
#define INTERPOLATION_SIGN_BIT 0x80000000

void InterpolationState_SetNegatedTargetAndRescaleProgress
          (GraphicsTransitionTickCount fadeOutTicks,GraphicsShadingRuntimeRecord *shadingRecord);

void InterpolationStateTable_Advance256ByTicks(GraphicsElapsedTickCount elapsedTicks);

#endif /* THANDOR_GRAPHICS_RENDER_LIGHT_TRANSITIONS_H */

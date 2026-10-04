/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/include/thandor/ui/frontend/credits_mask.h
 * Reverse engineering by idkFoxes 2026
 */

#ifndef THANDOR_UI_FRONTEND_CREDITS_MASK_H
#define THANDOR_UI_FRONTEND_CREDITS_MASK_H

#include <thandor/graphics/render/types.h>
#include <thandor/ui/frontend/types.h>
#include <thandor/core/contracts.h>

/* SoftwareMaskBuffer: a revealed mask byte brightens by this much per tick, saturating at 0xFF */
#define SOFTWARE_MASK_BRIGHTEN_STEP 0x1f

void SoftwareMaskBuffer_AdvancePatternByPercentTick(SoftwareMaskRuntimeView *maskRuntime);

void SoftwareMaskBuffer_Clear(SoftwareMaskRuntimeView *maskControl);

void SoftwareMaskBuffer_AdvanceNonzeroPixelsSaturating31(SoftwareMaskRuntimeView *maskRuntime);

void SoftwareMaskBuffer_ApplyCircularRegionBit(UiBooleanState32 invertSelection,GraphicsScreenCoordinate centerY,
          GraphicsScreenCoordinate centerX,SoftwareMaskRadiusStep radiusStep,
          SoftwareMaskRuntimeView *maskRuntime);

void SoftwareMaskBuffer_ApplyDiagonalHalfPlaneBit
          (UiBooleanState32 invertSelection,SoftwareMaskThresholdStep thresholdStep,
          SoftwareMaskRuntimeView *maskRuntime);

void SoftwareMaskBuffer_SetAllPixelsBit(SoftwareMaskRuntimeView *maskControl);

void SoftwareMaskBuffer_ApplyHorizontalBandBit(UiBooleanState32 reverseRows,TerrainGridMaskIndex bandIndex,
          SoftwareMaskRuntimeView *maskRuntime);

#endif /* THANDOR_UI_FRONTEND_CREDITS_MASK_H */

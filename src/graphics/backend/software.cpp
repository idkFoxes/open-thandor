/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/src/graphics/backend/software.cpp
 * Reverse engineering by idkFoxes 2026
 */

#include <thandor/graphics/backend/software.h>
#include <thandor/thandor.h>
#include <thandor/platform/bootstrap/image.h>
#include "software_raster.h"

/* Module data. */

/* SoftwareFramebufferDestroyProc * hook slot, statically SoftwareFramebuffer_Destroy (graphics/backend/software_display_mode.cpp). */
[[maybe_unused]] static SoftwareFramebufferDestroyProc *g_SoftwareFramebufferDestroy = &SoftwareFramebuffer_Destroy;

/* int16_t[256][4] MMX word lanes per 8-bit fraction f: lane0 = 0x4040 - 0x40*f, lane1 = 0x40*f (sum 0x4040), lanes 2/3 zero; PMADDWD horizontal weights of SoftwareTexture_SampleIntensity (graphics/backend/software_texture_scale.cpp); built by SoftwareRenderer_BuildFactorTables */
int16_t g_SoftwareBilinearPackedInterpolationWeights256[256][4];

/* Bilinear weights per 8-bit fraction f, all four lanes equal: forward (f * 0x4040) >> 8, inverse
   ((256 - f) * 0x4040) >> 8 (built by SoftwareRenderer_BuildFactorTables). Entry 256 is an original quirk:
   WorldLightingRuntime_UpdateInterpolatedTerrainLighting reads index 256 when the lighting cycle phase is 0
   (cosine exactly 1.0); in the original that read the first entry of the table that followed each one
   (g_UiScalerFirstPixelWeights: 0x4000, g_SoftwareBlendAlphaFactors: 0), so those values are kept there. */
SoftwareBgraWordLanes g_SoftwareBilinearForwardFactors[257];

SoftwareBgraWordLanes g_SoftwareBilinearInverseFactors[257];

/* Alpha blend factors per alpha a, B/G/R lanes (fourth lane zero): alpha (a * 0x4040) >> 8, inverse
   ((256 - a) * 0x4040) >> 8 (built by SoftwareRenderer_BuildFactorTables). Read past their 256 rows through
   the original-address emulation of software_raster.h (g_SoftwareBlendOverreadRanges), so they keep their size. */
SoftwareRgbWordLanes g_SoftwareBlendAlphaFactors[256];

SoftwareRgbWordLanes g_SoftwareBlendInverseAlphaFactors[256];

GraphicsEndSceneProc *g_GraphicsEndScene = &SoftwareGraphicsDispatch_NoOp;

GraphicsDiagnosticCounter g_PrimitiveDrawCallCount = 0;

GraphicsDiagnosticCounter g_TextureBindStateChangeCount = 0;

GraphicsDiagnosticCounter g_TextureDeviceReloadCount = 0;

GraphicsSetViewportProc *g_GraphicsSetViewportAndClearDepth = &SoftwareRenderer_ClearViewport;

GraphicsDrawPrimitiveQueueProc *g_GraphicsDrawPrimitiveQueue = &SoftwareRenderer_DrawPrimitiveQueueBridge;

GraphicsBeginSceneProc *g_GraphicsBeginScene = &SoftwareGraphicsDispatch_SuccessNoOp;

/* Implementation ownership: graphics/backend/software. */

/* Not in the original (it carried the tables precomputed): builds the bilinear and alpha blend factor tables.
   Per 8-bit fraction or alpha f the forward factor is (f * 0x4040) >> 8 and the inverse ((256 - f) * 0x4040) >> 8
   (the pair sums to 0x4040 or 0x403F): g_SoftwareBilinearForwardFactors / g_SoftwareBilinearInverseFactors in all
   four lanes, g_SoftwareBlendAlphaFactors / g_SoftwareBlendInverseAlphaFactors in the B/G/R lanes (fourth lane
   zero), g_SoftwareBilinearPackedInterpolationWeights256 as {inverse, forward, 0, 0}. Entry 256 of the bilinear
   tables keeps the original quirk values (forward 0x4000, inverse 0, see there). This reproduces every entry of
   the original tables. Called once at startup. */
void SoftwareRenderer_BuildFactorTables()
{
  int fraction;
  uint16_t forward;
  uint16_t inverse;

  for (fraction = 0; fraction <= 256; fraction++) {
    forward = (uint16_t)((fraction * 0x4040) >> 8);
    inverse = (uint16_t)(((256 - fraction) * 0x4040) >> 8);
    if (fraction == 256) {
      forward = 0x4000; /* the original read the first entry of g_UiScalerFirstPixelWeights here */
    }
    g_SoftwareBilinearForwardFactors[fraction].blue = forward;
    g_SoftwareBilinearForwardFactors[fraction].green = forward;
    g_SoftwareBilinearForwardFactors[fraction].red = forward;
    g_SoftwareBilinearForwardFactors[fraction].alpha = forward;
    g_SoftwareBilinearInverseFactors[fraction].blue = inverse;
    g_SoftwareBilinearInverseFactors[fraction].green = inverse;
    g_SoftwareBilinearInverseFactors[fraction].red = inverse;
    g_SoftwareBilinearInverseFactors[fraction].alpha = inverse;
    if (fraction < 256) {
      g_SoftwareBlendAlphaFactors[fraction].blue = forward;
      g_SoftwareBlendAlphaFactors[fraction].green = forward;
      g_SoftwareBlendAlphaFactors[fraction].red = forward;
      g_SoftwareBlendAlphaFactors[fraction].zero = 0;
      g_SoftwareBlendInverseAlphaFactors[fraction].blue = inverse;
      g_SoftwareBlendInverseAlphaFactors[fraction].green = inverse;
      g_SoftwareBlendInverseAlphaFactors[fraction].red = inverse;
      g_SoftwareBlendInverseAlphaFactors[fraction].zero = 0;
      g_SoftwareBilinearPackedInterpolationWeights256[fraction][0] = (int16_t)inverse;
      g_SoftwareBilinearPackedInterpolationWeights256[fraction][1] = (int16_t)forward;
      g_SoftwareBilinearPackedInterpolationWeights256[fraction][2] = 0;
      g_SoftwareBilinearPackedInterpolationWeights256[fraction][3] = 0;
    }
  }
}







/* Software backend of g_GraphicsBeginScene: the software renderer needs no scene setup, so it only reports
   success.
*/
void SoftwareGraphicsDispatch_SuccessNoOp()

{
  return;
}

/* Software backend of g_GraphicsEndScene: nothing to finish.
*/
void SoftwareGraphicsDispatch_NoOp()

{
  return;
}





































/* Class vtables. */


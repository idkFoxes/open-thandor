/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/include/thandor/graphics/backend/software.h
 * Reverse engineering by idkFoxes 2026
 */

#ifndef THANDOR_GRAPHICS_BACKEND_SOFTWARE_H
#define THANDOR_GRAPHICS_BACKEND_SOFTWARE_H

#include <thandor/core/types.h>
#include <thandor/graphics/backend/types.h>
#include <thandor/core/contracts.h>

/* Submodule: graphics/backend/software. */

/* Functions are grouped by semantic ownership. */

/* Builds the bilinear and alpha blend factor tables (the original carried them precomputed); once at startup. */
void SoftwareRenderer_BuildFactorTables();

void SoftwareGraphicsDispatch_NoOp();

extern GraphicsSetViewportProc *g_GraphicsSetViewportAndClearDepth;
extern GraphicsDrawPrimitiveQueueProc *g_GraphicsDrawPrimitiveQueue;

extern SoftwareBgraWordLanes g_SoftwareBilinearForwardFactors[257];
extern SoftwareBgraWordLanes g_SoftwareBilinearInverseFactors[257];
extern SoftwareRgbWordLanes g_SoftwareBlendAlphaFactors[256];
extern SoftwareRgbWordLanes g_SoftwareBlendInverseAlphaFactors[256];
extern GraphicsEndSceneProc *g_GraphicsEndScene;
extern GraphicsDiagnosticCounter g_PrimitiveDrawCallCount;

extern int16_t g_SoftwareBilinearPackedInterpolationWeights256[256][4];

#endif /* THANDOR_GRAPHICS_BACKEND_SOFTWARE_H */

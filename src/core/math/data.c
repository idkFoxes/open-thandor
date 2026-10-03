/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/src/core/math/data.c
 */

/* Data of the original image that this module uses (moved here from the generated image data in
   step 4c); declared in <thandor/core/math/data.h>. */

#include <thandor/thandor.h>

#pragma warning(disable : 4152) /* function pointer fields initialized through (void *) */

__declspec(align(16)) RandomGeneratorState g_RandomGeneratorState = {.next = (void *)Random_NextPrimary, .primarySeed = 0x198F};

/* filled at startup
   by FixedMath_BuildSinCosTables */
__declspec(align(16)) int32_t g_FixedSineQ28[98304] = {0};

__declspec(align(16)) GraphicsFixedMatrix3x4 g_FixedTransformInputRotationScratch = {0};

__declspec(align(16)) GraphicsFixedMatrix3x4 g_FixedTransformComposedRotationScratch = {0};

__declspec(align(16)) GraphicsFixedVec3 g_ModelTransformInput = {0};

__declspec(align(4)) GraphicsFixedVec3 g_ModelTransformOutput = {0};

__declspec(align(16)) float *g_WorldMotionSplineMatrixWorkspaces[6] = {0};

__declspec(align(8)) float *g_WorldMotionSplineCoefficientTables[6] = {0};

__declspec(align(16)) float g_WorldMotionSplineCachedDerivatives[6] = {0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f};

__declspec(align(8)) int32_t g_WorldMotionSplineEquationCounts[6] = {0};

__declspec(align(16)) float g_Q12FloatScale4096 = 4096.0f;

/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/src/core/math/data.c
 */

/* Data of the original image that this module uses (moved here from the generated image data in
   step 4c); declared in <thandor/core/math/data.h>. Original addresses in the comments. */

#include <thandor/thandor.h>

#pragma warning(disable : 4152) /* function pointer fields initialized through (void *) */

/* 004030A0 g_RandomGeneratorState */
__declspec(align(16)) RandomGeneratorState g_RandomGeneratorState = {.next = (void *)Random_NextPrimary, .primarySeed = 0x198F};

/* 004246A0 g_FixedSineQ28 (sin before zero, sin from 004346A0, cos from 004446A0): filled at startup
   by FixedMath_BuildSinCosTables */
__declspec(align(16)) int32_t g_FixedSineQ28[98304] = {0};

/* 004BEAA0 g_FixedTransformInputRotationScratch */
__declspec(align(16)) GraphicsFixedMatrix3x4 g_FixedTransformInputRotationScratch = {0};

/* 004BEAD0 g_FixedTransformComposedRotationScratch */
__declspec(align(16)) GraphicsFixedMatrix3x4 g_FixedTransformComposedRotationScratch = {0};

/* 004BEB00 g_ModelTransformInput */
__declspec(align(16)) GraphicsFixedVec3 g_ModelTransformInput = {0};

/* 004BEB0C g_ModelTransformOutput */
__declspec(align(4)) GraphicsFixedVec3 g_ModelTransformOutput = {0};

/* 0053C9A0 g_WorldMotionSplineMatrixWorkspaces */
__declspec(align(16)) float *g_WorldMotionSplineMatrixWorkspaces[6] = {0};

/* 0053C9B8 g_WorldMotionSplineCoefficientTables */
__declspec(align(8)) float *g_WorldMotionSplineCoefficientTables[6] = {0};

/* 0053C9D0 g_WorldMotionSplineCachedDerivatives */
__declspec(align(16)) float g_WorldMotionSplineCachedDerivatives[6] = {0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f};

/* 0053C9E8 g_WorldMotionSplineEquationCounts */
__declspec(align(8)) int32_t g_WorldMotionSplineEquationCounts[6] = {0};

/* 0053CA00 g_Q12FloatScale4096 */
__declspec(align(16)) float g_Q12FloatScale4096 = 4096.0f;

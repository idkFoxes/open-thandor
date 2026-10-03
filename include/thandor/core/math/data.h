/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/include/thandor/core/math/data.h
 */

#ifndef THANDOR_CORE_MATH_DATA_H
#define THANDOR_CORE_MATH_DATA_H

#include <thandor/generated/types.h>
#include <thandor/generated/ui_templates.h>

extern RandomGeneratorState g_RandomGeneratorState;

/* one sine over 1.5 turns in Q28, from a quarter turn before angle 0: sin(-16384..-1), then
   sin(0..16383) (FIXED_SINE_TABLE_SIN), then cos(0..65535) (FIXED_SINE_TABLE_COS); signed and full-turn lookups run on from one part into the next */
extern int32_t g_FixedSineQ28[98304];

extern GraphicsFixedMatrix3x4 g_FixedTransformInputRotationScratch;

extern GraphicsFixedMatrix3x4 g_FixedTransformComposedRotationScratch;

extern GraphicsFixedVec3 g_ModelTransformInput;

extern GraphicsFixedVec3 g_ModelTransformOutput;

extern float *g_WorldMotionSplineMatrixWorkspaces[6];

extern float *g_WorldMotionSplineCoefficientTables[6];

extern float g_WorldMotionSplineCachedDerivatives[6];

extern int32_t g_WorldMotionSplineEquationCounts[6];

extern float g_Q12FloatScale4096;

#endif

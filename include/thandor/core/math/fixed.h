/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/include/thandor/core/math/fixed.h
 * Reverse engineering by idkFoxes 2026
 */

#ifndef THANDOR_CORE_MATH_FIXED_H
#define THANDOR_CORE_MATH_FIXED_H

#include <thandor/generated/types.h>
#include <thandor/core/contracts.h>

/* Submodule: core/math/fixed. */

#ifndef Q12_ONE
#define Q12_ONE 0x1000 /* 1.0 in Q12 fixed point (scales, world coordinates) */
#endif
#ifndef Q28_ONE
#define Q28_ONE 0x10000000 /* 1.0 in Q28 fixed point (g_FixedSinQ28/g_FixedCosQ28 values) */
#endif

/* Engine angles are 16-bit fractions of a full turn (0x10000 = 360 degrees), as used by the
   g_FixedSinQ28/g_FixedCosQ28 lookups and returned by FixedMath_Atan2Angle16. */
#define FIXED_ANGLE16_EIGHTH_TURN 0x2000
#define FIXED_ANGLE16_QUARTER_TURN 0x4000
#define FIXED_ANGLE16_HALF_TURN 0x8000
#define FIXED_ANGLE16_THREE_QUARTER_TURN 0xC000
#define FIXED_ANGLE16_FULL_TURN 0x10000
/* Wraps an angle to 16 bits (one full turn) before a g_FixedSinQ28/g_FixedCosQ28 lookup */
#define FIXED_ANGLE16_MASK 0xffff

/* The low 32 bits of a signed 64-bit product shifted right by `shift` (0 < shift < 32): the original's
   SHLD EDX,EAX,32-shift / SHRD EAX,EDX,shift, which drops the fixed-point factor of a product (28 for a Q28
   factor, 12 for Q12). Written as the two halves so the compiler emits the same SHLD form. */
#define FIXED_PRODUCT_SHR(product, shift) \
  ((int)((uint64_t)(product) >> 32) << (32 - (shift)) | (uint32_t)(product) >> (shift))
/* CosineDerivedLookupTables_Init: two 256x256 tables of shorts (the .sam codec's cosine transform) */
#define COSINE_DERIVED_TABLE_ORDER 256
#define COSINE_DERIVED_TABLE_ANGLE_STEP 0x40 /* pi/512 in angle16 units */
#define COSINE_DERIVED_INV_SQRT2_Q12 2896 /* 1/sqrt(2) in Q12: row 0 of the first table */
#define COSINE_DERIVED_INV_SQRT2_Q14 11585 /* 1/sqrt(2) in Q14: entry 0 of each row of the second table */

/* High 32 bits of the signed 64-bit product a * b (the EDX of a one-operand IMUL) */
#define FIXED_MUL_HIGH(a, b) ((int)((uint64_t)((int64_t)(a) * (int64_t)(b)) >> 32))
/* FixedMath_UInt64Sqrt of a 64-bit sum of squares, passed as its high and low halves */
#define FIXED_UINT64_SQRT(value) \
  FixedMath_UInt64Sqrt((UInt64Half32)((uint64_t)(value) >> 32),(UInt64Half32)(value))
/* Functions are grouped by semantic ownership; address comments are executable virtual addresses. */

/* 0x004BECB0 */
FixedAzimuthElevationRoll FixedTransform_ComposeEulerAnglesRegs
          (AngleTurn32 inputAngle0,AngleTurn32 inputAngle1,AngleTurn32 inputAngle2,
          AngleTurn32 basisAngle0,AngleTurn32 basisAngle1,AngleTurn32 basisAngle2);

/* 0x00484930 */
FixedLengthAzimuthElevation
FixedMath_VectorToAnglesAndLength3Regs
          (FixedMathVectorComponent32 x,FixedMathVectorComponent32 y,FixedMathVectorComponent32 z);

/* 0x00484A10 */
FixedLengthAzimuthElevation FixedMath_VectorToAnglesAndLengthVec3Regs(GraphicsFixedVec3 *vector);

/* 0x00484B70 */
FixedLengthAngle FixedMath_Vector2AngleAndLengthRegs
          (FixedMathVectorComponent32 component0,FixedMathVectorComponent32 component1);

/* 0x004BEB20 */
FixedVectorQ12
FixedTransform_ApplyEulerRotationToVectorRegs
          (Q12 inputZQ12,Q12 inputYQ12,Q12 inputXQ12,AngleTurn32 rotationAngle0,
          AngleTurn32 rotationAngle1,AngleTurn32 rotationAngle2);

/* 0x004BED10 */
void FixedVector_StepBackwardAlongOwnDirection
          (FixedVectorStepMultiplier32 stepMultiplier,FixedMathScale32 directionScale,
          FixedVectorStateAddress32 vectorState);

/* 0x00521FA0 */
FixedTriangleJointAngles FixedGeometry_SolveTriangleJointAnglesRegs(Q12 sideLength0Q12,Q12 sideLength1Q12,Q12 sideLength2Q12);

/* 0x004849D0 */
uint32_t FixedMath_Length3(FixedMathVectorComponent32 x,FixedMathVectorComponent32 y,
                 FixedMathVectorComponent32 z);

/* 0x00484E50 */
FixedVectorAngles FixedTransform_ExtractForwardAnglesRegs(GraphicsFixedMatrix3x4 *transform);

/* 0x004857A0 */
void FixedVec3_NormalizeQ28(GraphicsFixedVec3 *output,GraphicsFixedVec3 *input);

/* 0x004BEC20 */
FixedVectorQ12
FixedTransform_RotateDirectionScaledRegs
          (FixedMathScale32 directionScale,AngleTurn32 elevationAngle,AngleTurn32 azimuthAngle,
          AngleTurn32 rotationAngle0,AngleTurn32 rotationAngle1,AngleTurn32 rotationAngle2);

/* 0x00417620 */
void __cdecl CosineDerivedLookupTables_Init(void);

/* 0x004848C0 */
void FixedMath_WriteDirectionQ28(GraphicsFixedVec3 *output,AngleTurn32 elevationAngle,AngleTurn32 azimuthAngle);

/* 0x00484B00 */
FixedSinCosEdxEax8 FixedMath_SinCosScaled(AngleTurn32 angle,FixedMathScale32 scale);

/* 0x00484B40 */
FixedSinCosEdxEax8 FixedMath_SinCosQ28(AngleTurn32 angle);

/* 0x00484F10 */
void FixedTransform_ApplyDirection
          (GraphicsFixedVec3 *output,GraphicsFixedVec3 *direction,GraphicsFixedMatrix3x4 *transform);

/* 0x00485090 */
void FixedTransform_ApplyTransposeDirection
          (GraphicsFixedVec3 *output,GraphicsFixedMatrix3x4 *transform,GraphicsFixedVec3 *direction);

/* 0x00485520 */
void FixedTransform_InvertRigidQ28(GraphicsFixedMatrix3x4 *output,GraphicsFixedMatrix3x4 *input);

/* 0x004856B0 */
int32_t FixedVec3_DotQ12(GraphicsFixedVec3 *left,GraphicsFixedVec3 *right);

/* 0x004856F0 */
int32_t FixedVec3_DotQ28(GraphicsFixedVec3 *left,GraphicsFixedVec3 *right);

/* 0x00485730 */
void FixedVec3_CrossQ12(GraphicsFixedVec3 *output,GraphicsFixedVec3 *rightOperand,
                  GraphicsFixedVec3 *leftOperand);

/* 0x0052AD50 */
FixedPlanarPointEdxEax8 FixedTrig_ProjectPlanarPointRegs(Q12 distance,AngleTurn32 angle16,Q12 baseY,Q12 baseX);

/* 0x004BEC50 */
FixedVectorQ12 FixedTransform_RotateDirectionScaledCoreRegs
          (FixedMathScale32 directionScale,AngleTurn32 elevationAngle,AngleTurn32 azimuthAngle,
          AngleTurn32 rotationAngle0,AngleTurn32 rotationAngle1,AngleTurn32 rotationAngle2);

/* 0x00484A70 */
FixedElevationAzimuth FixedMath_VectorToAnglesVec3Regs(GraphicsFixedVec3 *vector);

/* 0x00484E00 */
FixedRollAzimuthElevation FixedTransform_ExtractEulerAnglesRegs(GraphicsFixedMatrix3x4 *transform);

/* 0x00484AC0 */
uint32_t FixedMath_LengthVec3(GraphicsFixedVec3 *vector);

/* 0x00484CF0 */
uint32_t FixedMath_Length2(FixedMathVectorComponent32 x,FixedMathVectorComponent32 y);

/* 0x00484770 */
FixedDirection
FixedMath_DirectionFromAnglesScaledRegs
          (AngleTurn32 elevationAngle,AngleTurn32 azimuthAngle,FixedMathScale32 scale);

/* 0x004847E0 */
FixedDirection
FixedMath_DirectionFromAnglesQ28Regs(AngleTurn32 elevationAngle,AngleTurn32 azimuthAngle);

/* 0x00484840 */
void FixedMath_WriteDirectionScaled(GraphicsFixedVec3 *output,AngleTurn32 elevationAngle,AngleTurn32 azimuthAngle,
          FixedMathScale32 scale);

/* 0x00485120 */
void FixedTransform_Compose(GraphicsFixedMatrix3x4 *output,GraphicsFixedMatrix3x4 *innerTransform,
          GraphicsFixedMatrix3x4 *outerTransform);

/* 0x00484990 */
FixedVectorAngles FixedMath_VectorToAngles3Regs
          (FixedMathVectorComponent32 z,FixedMathVectorComponent32 y,FixedMathVectorComponent32 x);

/* 0x00484E70 */
void FixedTransform_ApplyPoint(GraphicsFixedVec3 *output,GraphicsFixedVec3 *point,GraphicsFixedMatrix3x4 *transform);

/* 0x00484D20 */
void FixedTransform_BuildRotationBasis(GraphicsFixedMatrix3x4 *output,AngleTurn32 rollAngle,AngleTurn32 elevationAngle,
          AngleTurn32 azimuthAngle);

/* 0x00484BA0 */
uint32_t FixedMath_Atan2Angle16(FixedMathVectorComponent32 y,FixedMathVectorComponent32 x);

/* 0x00484700 */
uint32_t FixedMath_UInt64Sqrt(UInt64Half32 high,UInt64Half32 low);


/* 0x004846A0 */
uint32_t FixedMath_SqrtQ12Approx(uint32_t inputValue);

/* Not in the original: fills g_FixedSinBeforeZeroQ28, g_FixedSinQ28 and g_FixedCosQ28 (the original shipped them precomputed). */
void FixedMath_BuildSinCosTables(void);

#endif /* THANDOR_CORE_MATH_FIXED_H */

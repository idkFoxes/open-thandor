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
/* Functions are grouped by semantic ownership; address comments are executable virtual addresses. */

/* 0x004BECB0 */
FixedEulerAnglesEaxEbxEdx12 __thandor_eax_edx_cf_preserve_ecx
FixedTransform_ComposeEulerAnglesRegs
          (AngleTurn32 inputAngle0,AngleTurn32 inputAngle1,AngleTurn32 inputAngle2,
          AngleTurn32 basisAngle0,AngleTurn32 basisAngle1,AngleTurn32 basisAngle2);

/* 0x00484930 */
FixedLengthAnglesEaxEcxEdx12
FixedMath_VectorToAnglesAndLength3Regs
          (FixedMathVectorComponent32 x,FixedMathVectorComponent32 y,FixedMathVectorComponent32 z);

/* 0x00484A10 */
FixedLengthAnglesEaxEcxEdx12 FixedMath_VectorToAnglesAndLengthVec3Regs(GraphicsFixedVec3 *vector);

/* 0x00484B70 */
FixedLengthAngleEaxEdx8 __thandor_eax_edx_cf_preserve_ecx
FixedMath_Vector2AngleAndLengthRegs
          (FixedMathVectorComponent32 component0,FixedMathVectorComponent32 component1);

/* 0x004BEB20 */
FixedVectorEaxEcxEdx12
FixedTransform_ApplyEulerRotationToVectorRegs
          (Q12 inputZQ12,Q12 inputYQ12,Q12 inputXQ12,AngleTurn32 rotationAngle0,
          AngleTurn32 rotationAngle1,AngleTurn32 rotationAngle2);

/* 0x004BED10 */
void __thandor_void_preserve_eax_ecx_edx
FixedVector_StepBackwardAlongOwnDirection
          (FixedVectorStepMultiplier32 stepMultiplier,FixedMathScale32 directionScale,
          FixedVectorStateAddress32 vectorState);

/* 0x00521FA0 */
FixedTriangleJointAnglesEaxEdx8 __thandor_eax_edx_cf_preserve_ecx
FixedGeometry_SolveTriangleJointAnglesRegs(Q12 sideLength0Q12,Q12 sideLength1Q12,Q12 sideLength2Q12);

/* 0x004849D0 */
dword __thandor_eax_preserve_ecx_edx
FixedMath_Length3(FixedMathVectorComponent32 x,FixedMathVectorComponent32 y,
                 FixedMathVectorComponent32 z);

/* 0x00484E50 */
FixedMathVectorAnglesRegs8 __thandor_preserve_eax
FixedTransform_ExtractForwardAnglesRegs(GraphicsFixedMatrix3x4 *transform);

/* 0x004857A0 */
void __thandor_void_preserve_eax_ecx_edx
FixedVec3_NormalizeQ28(GraphicsFixedVec3 *output,GraphicsFixedVec3 *input);

/* 0x004BEC20 */
FixedVectorEaxEcxEdx12
FixedTransform_RotateDirectionScaledRegs
          (FixedMathScale32 directionScale,AngleTurn32 elevationAngle,AngleTurn32 azimuthAngle,
          AngleTurn32 rotationAngle0,AngleTurn32 rotationAngle1,AngleTurn32 rotationAngle2);

/* 0x00417620 */
void __cdecl CosineDerivedLookupTables_InitCf(void);

/* 0x004848C0 */
void __thandor_void_preserve_eax_ecx_edx
FixedMath_WriteDirectionQ28
          (GraphicsFixedVec3 *output,AngleTurn32 elevationAngle,AngleTurn32 azimuthAngle);

/* 0x00484B00 */
FixedSinCosEdxEax8 __thandor_eax_edx_cf_preserve_ecx
FixedMath_SinCosScaled(AngleTurn32 angle,FixedMathScale32 scale);

/* 0x00484B40 */
FixedSinCosEdxEax8 FixedMath_SinCosQ28(AngleTurn32 angle);

/* 0x00484F10 */
void __thandor_void_preserve_eax_ecx_edx
FixedTransform_ApplyDirection
          (GraphicsFixedVec3 *output,GraphicsFixedVec3 *direction,GraphicsFixedMatrix3x4 *transform);

/* 0x00485090 */
void __thandor_void_preserve_eax_ecx_edx
FixedTransform_ApplyTransposeDirection
          (GraphicsFixedVec3 *output,GraphicsFixedMatrix3x4 *transform,GraphicsFixedVec3 *direction);

/* 0x00485520 */
void __thandor_void_preserve_eax_ecx_edx
FixedTransform_InvertRigidQ28(GraphicsFixedMatrix3x4 *output,GraphicsFixedMatrix3x4 *input);

/* 0x004856B0 */
sdword __thandor_eax_preserve_ecx_edx
FixedVec3_DotQ12(GraphicsFixedVec3 *left,GraphicsFixedVec3 *right);

/* 0x004856F0 */
sdword __thandor_eax_preserve_ecx_edx
FixedVec3_DotQ28(GraphicsFixedVec3 *left,GraphicsFixedVec3 *right);

/* 0x00485730 */
void __thandor_void_preserve_eax_ecx_edx
FixedVec3_CrossQ12(GraphicsFixedVec3 *output,GraphicsFixedVec3 *rightOperand,
                  GraphicsFixedVec3 *leftOperand);

/* 0x0052AD50 */
FixedPlanarPointEdxEax8 FixedTrig_ProjectPlanarPointRegs(Q12 distance,AngleTurn32 angle16,Q12 baseY,Q12 baseX);

/* 0x004BEC50 */
FixedVectorXEaxYEbxZEdx12 __thandor_eax_edx_cf_preserve_ecx
FixedTransform_RotateDirectionScaledCoreRegs
          (FixedMathScale32 directionScale,AngleTurn32 elevationAngle,AngleTurn32 azimuthAngle,
          AngleTurn32 rotationAngle0,AngleTurn32 rotationAngle1,AngleTurn32 rotationAngle2);

/* 0x00484A70 */
FixedMathVectorAnglesRegs8 __thandor_preserve_eax
FixedMath_VectorToAnglesVec3Regs(GraphicsFixedVec3 *vector);

/* 0x00484E00 */
FixedEulerAnglesEaxEcxEdx12 FixedTransform_ExtractEulerAnglesRegs(GraphicsFixedMatrix3x4 *transform);

/* 0x00484AC0 */
dword __thandor_eax_preserve_ecx_edx FixedMath_LengthVec3(GraphicsFixedVec3 *vector);

/* 0x00484CF0 */
dword __thandor_eax_preserve_ecx_edx
FixedMath_Length2(FixedMathVectorComponent32 x,FixedMathVectorComponent32 y);

/* 0x00484770 */
FixedDirectionXyzRegs12
FixedMath_DirectionFromAnglesScaledRegs
          (AngleTurn32 elevationAngle,AngleTurn32 azimuthAngle,FixedMathScale32 scale);

/* 0x004847E0 */
FixedDirectionXyzRegs12
FixedMath_DirectionFromAnglesQ28Regs(AngleTurn32 elevationAngle,AngleTurn32 azimuthAngle);

/* 0x00484840 */
void __thandor_void_preserve_eax_ecx_edx
FixedMath_WriteDirectionScaled
          (GraphicsFixedVec3 *output,AngleTurn32 elevationAngle,AngleTurn32 azimuthAngle,
          FixedMathScale32 scale);

/* 0x00485120 */
void __thandor_void_preserve_eax_ecx_edx
FixedTransform_Compose
          (GraphicsFixedMatrix3x4 *output,GraphicsFixedMatrix3x4 *transformA,
          GraphicsFixedMatrix3x4 *transformB);

/* 0x00484990 */
FixedMathVectorAnglesRegs8 __thandor_preserve_eax
FixedMath_VectorToAngles3Regs
          (FixedMathVectorComponent32 x,FixedMathVectorComponent32 y,FixedMathVectorComponent32 z);

/* 0x00484E70 */
void __thandor_void_preserve_eax_ecx_edx
FixedTransform_ApplyPoint
          (GraphicsFixedVec3 *output,GraphicsFixedVec3 *point,GraphicsFixedMatrix3x4 *transform);

/* 0x00484D20 */
void __thandor_void_preserve_eax_ecx_edx
FixedTransform_BuildRotationBasis
          (GraphicsFixedMatrix3x4 *output,AngleTurn32 angle0,AngleTurn32 angle1,AngleTurn32 angle2);

/* 0x00484BA0 */
dword __thandor_eax_preserve_ecx_edx
FixedMath_Atan2Angle16(FixedMathVectorComponent32 y,FixedMathVectorComponent32 x);

/* 0x00484700 */
dword __thandor_eax_preserve_ecx_edx FixedMath_UInt64Sqrt(UInt64Half32 high,UInt64Half32 low);


/* 0x004846A0 */
dword __thandor_eax_preserve_ecx_edx FixedMath_SqrtQ12Approx(uint inputValue);

#endif /* THANDOR_CORE_MATH_FIXED_H */

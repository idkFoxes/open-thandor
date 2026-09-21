/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/src/core/math/fixed.c
 * Reverse engineering by idkFoxes 2026
 */

#include <thandor/core/math/fixed.h>

/* Implementation ownership: core/math/fixed. */

/* Address: 0x004BECB0.
   Ownership: core/math/fixed.
   Purpose: Builds two fixed-point rotation bases, composes them, extracts the resulting Euler angles, and returns
   the composed orientation through the engine register convention. Kept distinct from Q12 coordinates, Q4/Q5
   resource scales, attachment ordinals, and raw renderer flags. Typed parameters: p2 inputAngle0→AngleTurn32, p3
   inputAngle1→AngleTurn32, p4 inputAngle2→AngleTurn32, p5 basisAngle0→AngleTurn32, p6 basisAngle1→AngleTurn32, p7
   basisAngle2→AngleTurn32. Calling convention, storage, body bytes, control flow, and executable data remain
   unchanged.
   Local calls: FixedTransform_BuildRotationBasis, FixedTransform_Compose, FixedTransform_ExtractEulerAnglesRegs.
*/
FixedEulerAnglesEaxEbxEdx12 __thandor_eax_edx_cf_preserve_ecx
FixedTransform_ComposeEulerAnglesRegs
          (AngleTurn32 inputAngle0,AngleTurn32 inputAngle1,AngleTurn32 inputAngle2,
          AngleTurn32 basisAngle0,AngleTurn32 basisAngle1,AngleTurn32 basisAngle2)

{
  FixedEulerPairEdxEax8 composedEulerAnglePair;
  FixedEulerAnglesEaxEcxEdx12 FVar1;
  FixedEulerAnglesEaxEbxEdx12 FVar2;
  
  FixedTransform_BuildRotationBasis
            ((GraphicsFixedMatrix3x4 *)&g_ModelTransformScratchMatrix,basisAngle0,basisAngle1,
             basisAngle2);
  FixedTransform_BuildRotationBasis
            ((GraphicsFixedMatrix3x4 *)&g_FixedTransformInputRotationScratch,inputAngle0,inputAngle1
             ,inputAngle2);
  FixedTransform_Compose
            ((GraphicsFixedMatrix3x4 *)&g_FixedTransformComposedRotationScratch,
             (GraphicsFixedMatrix3x4 *)&g_FixedTransformInputRotationScratch,
             (GraphicsFixedMatrix3x4 *)&g_ModelTransformScratchMatrix);
  FVar1 = FixedTransform_ExtractEulerAnglesRegs
                    ((GraphicsFixedMatrix3x4 *)&g_FixedTransformComposedRotationScratch);
  FVar2.angle2 = FVar1.eaxAngle;
  FVar2.angle0 = (int)FVar1._4_8_;
  FVar2.angle1 = (int)((ulonglong)FVar1._4_8_ >> 0x20);
  return FVar2;
}


/* Address: 0x00484930.
   Ownership: core/math/fixed.
   Purpose: Calculates a 3D vector length and two wrapping 16-bit angles. EAX=length, EDX=elevation angle,
   ECX=azimuth angle. The declared 64-bit return models EDX:EAX; ECX remains an extra output. Typed parameters: p0
   x→FixedMathVectorComponent32_V342, p1 y→FixedMathVectorComponent32_V342, p2 z→FixedMathVectorComponent32_V342.
   Calling convention, exact VariableStorage serialization, function body bytes, control flow, globals, locals, and
   executable data remain unchanged.
   Local calls: FixedMath_UInt64Sqrt, FixedMath_Atan2Angle16.
*/
FixedLengthAnglesEaxEcxEdx12
FixedMath_VectorToAnglesAndLength3Regs
          (FixedMathVectorComponent32 x,FixedMathVectorComponent32 y,FixedMathVectorComponent32 z)

{
  ulonglong uVar1;
  dword elevationAngle16;
  dword dVar2;
  dword dVar3;
  dword vectorLengthQ12;
  FixedLengthAnglesEaxEcxEdx12 FVar4;
  longlong squaredLengthAccumulatorQ24;
  longlong totalSquaredLengthQ24;
  
  squaredLengthAccumulatorQ24 = (longlong)y * (longlong)y + (longlong)z * (longlong)z;
  elevationAngle16 =
       FixedMath_UInt64Sqrt
                 ((UInt64Half32)((ulonglong)squaredLengthAccumulatorQ24 >> 0x20),
                  (UInt64Half32)squaredLengthAccumulatorQ24);
  dVar2 = FixedMath_Atan2Angle16(x,elevationAngle16);
  dVar3 = FixedMath_Atan2Angle16(y,z);
  totalSquaredLengthQ24 = squaredLengthAccumulatorQ24 + (longlong)x * (longlong)x;
  vectorLengthQ12 =
       FixedMath_UInt64Sqrt
                 ((UInt64Half32)((ulonglong)totalSquaredLengthQ24 >> 0x20),
                  (UInt64Half32)totalSquaredLengthQ24);
  uVar1 = CONCAT44(dVar3,vectorLengthQ12) & 0xffffffffffff;
  FVar4.elevationAngle = dVar2;
  FVar4.lengthQ12 = (int)uVar1;
  FVar4.azimuthAngle = (int)(uVar1 >> 0x20);
  return FVar4;
}


/* Address: 0x00484A10.
   Ownership: core/math/fixed.
   Purpose: Pointer form of FixedMath_VectorToAnglesAndLength3Regs. EAX=length, EDX=elevation angle, ECX=azimuth
   angle.
   Local calls: FixedMath_UInt64Sqrt, FixedMath_Atan2Angle16.
*/
FixedLengthAnglesEaxEcxEdx12 FixedMath_VectorToAnglesAndLengthVec3Regs(GraphicsFixedVec3 *vector)

{
  ulonglong uVar1;
  dword elevationAngle16;
  dword dVar2;
  dword dVar3;
  dword vectorLengthQ12;
  FixedLengthAnglesEaxEcxEdx12 FVar4;
  int y;
  int x;
  longlong totalSquaredLengthQ24;
  longlong squaredLengthAccumulatorQ24;
  GraphicsWorldCoordinateQ12 inputZQ12;
  
  x = vector->x;
  inputZQ12 = vector->z;
  y = vector->y;
  squaredLengthAccumulatorQ24 = (longlong)y * (longlong)y + (longlong)x * (longlong)x;
  elevationAngle16 =
       FixedMath_UInt64Sqrt
                 ((UInt64Half32)((ulonglong)squaredLengthAccumulatorQ24 >> 0x20),
                  (UInt64Half32)squaredLengthAccumulatorQ24);
  dVar2 = FixedMath_Atan2Angle16(vector->z,elevationAngle16);
  dVar3 = FixedMath_Atan2Angle16(y,x);
  totalSquaredLengthQ24 = squaredLengthAccumulatorQ24 + (longlong)inputZQ12 * (longlong)inputZQ12;
  vectorLengthQ12 =
       FixedMath_UInt64Sqrt
                 ((UInt64Half32)((ulonglong)totalSquaredLengthQ24 >> 0x20),
                  (UInt64Half32)totalSquaredLengthQ24);
  uVar1 = CONCAT44(dVar3,vectorLengthQ12) & 0xffffffffffff;
  FVar4.elevationAngle = dVar2;
  FVar4.lengthQ12 = (int)uVar1;
  FVar4.azimuthAngle = (int)(uVar1 >> 0x20);
  return FVar4;
}


/* Address: 0x00484B70.
   Ownership: core/math/fixed.
   Purpose: Calculates a 2D vector length and wrapping 16-bit angle. EAX=length and EDX=angle. Typed parameters: p0
   component0→FixedMathVectorComponent32_V342, p1 component1→FixedMathVectorComponent32_V342. Calling convention,
   exact VariableStorage serialization, function body bytes, control flow, globals, locals, and executable data
   remain unchanged.
   Local calls: FixedMath_Atan2Angle16, FixedMath_Length2.
*/
FixedLengthAngleEaxEdx8 __thandor_eax_edx_cf_preserve_ecx
FixedMath_Vector2AngleAndLengthRegs
          (FixedMathVectorComponent32 component0,FixedMathVectorComponent32 component1)

{
  dword dVar1;
  dword vectorLengthQ12;
  
  dVar1 = FixedMath_Atan2Angle16(component0,component1);
  vectorLengthQ12 = FixedMath_Length2(component0,component1);
  return (FixedLengthAngleEaxEdx8)(CONCAT44(dVar1,vectorLengthQ12) & 0xffffffffffff);
}


/* Address: 0x004BEB20.
   Ownership: core/math/fixed.
   Purpose: Euler triple -> basis -> rotate vector; the billboard/view-facing rotation helper path. Kept distinct
   from Q12 coordinates, Q4/Q5 resource scales, attachment ordinals, and raw renderer flags. Typed parameters: p5
   rotationAngle0→AngleTurn32, p6 rotationAngle1→AngleTurn32, p7 rotationAngle2→AngleTurn32. Calling convention,
   storage, body bytes, control flow, and executable data remain unchanged. Typed parameters: p2 inputZQ12→Q12, p3
   inputYQ12→Q12, p4 inputXQ12→Q12.
   Local calls: FixedTransform_BuildRotationBasis, FixedTransform_ApplyPoint.
*/
FixedVectorEaxEcxEdx12
FixedTransform_ApplyEulerRotationToVectorRegs
          (Q12 inputZQ12,Q12 inputYQ12,Q12 inputXQ12,AngleTurn32 rotationAngle0,
          AngleTurn32 rotationAngle1,AngleTurn32 rotationAngle2)

{
  FixedVectorEaxEcxEdx12 FVar1;
  
  FixedTransform_BuildRotationBasis
            ((GraphicsFixedMatrix3x4 *)&g_ModelTransformScratchMatrix,rotationAngle0,rotationAngle1,
             rotationAngle2);
  g_ModelTransformInputX = inputXQ12;
  g_ModelTransformInputY = inputYQ12;
  g_ModelTransformInputZ = inputZQ12;
  FixedTransform_ApplyPoint
            ((GraphicsFixedVec3 *)&g_ModelTransformOutputX,
             (GraphicsFixedVec3 *)&g_ModelTransformInputX,
             (GraphicsFixedMatrix3x4 *)&g_ModelTransformScratchMatrix);
  FVar1.yQ12 = g_ModelTransformOutputY;
  FVar1.xQ12 = g_ModelTransformOutputX;
  FVar1.zQ12 = g_ModelTransformOutputZ;
  return FVar1;
}


/* Address: 0x004BED10.
   Ownership: core/math/fixed.
   Purpose: Typed parameters: p2 stepMultiplier→FixedVectorStepMultiplier32_V344. Calling convention, complete
   VariableStorage serialization, function bytes, control flow, globals, locals, and executable data remain
   unchanged. Typed parameters: p4 vectorState→FixedVectorStateAddress32_V345. Calling convention, complete
   VariableStorage serialization, function bytes, control flow, globals, locals, and executable data remain
   unchanged.
   Local calls: FixedMath_VectorToAnglesVec3Regs, FixedMath_DirectionFromAnglesScaledRegs.
*/
void __thandor_void_preserve_eax_ecx_edx
FixedVector_StepBackwardAlongOwnDirection
          (FixedVectorStepMultiplier32 stepMultiplier,FixedMathScale32 directionScale,
          FixedVectorStateAddress32 vectorState)

{
  FixedDirectionXZEdxEax8 stepDirectionXZQ12;
  FixedDirectionXyzRegs12 FVar1;
  FixedMathVectorAnglesRegs8 FVar2;
  
  FVar2 = FixedMath_VectorToAnglesVec3Regs((GraphicsFixedVec3 *)(vectorState + 0x18));
  FVar1 = FixedMath_DirectionFromAnglesScaledRegs(FVar2.ecx,FVar2.edx,directionScale);
  *(int *)(vectorState + 0x18) = *(int *)(vectorState + 0x18) - FVar1.eax * stepMultiplier;
  *(int *)(vectorState + 0x1c) = *(int *)(vectorState + 0x1c) - FVar1.ecx * stepMultiplier;
  *(int *)(vectorState + 0x20) = *(int *)(vectorState + 0x20) - FVar1.edx * stepMultiplier;
  return;
}


/* Address: 0x00521FA0.
   Ownership: core/math/fixed.
   Purpose: Solves the two angle results for a triangle from three fixed-point side lengths, using the verified
   square-root and atan2 paths with saturated fallback states. Storage remains one signed 32-bit word. Typed
   parameters: p0 sideLength0Q12→Q12, p1 sideLength1Q12→Q12, p2 sideLength2Q12→Q12. Calling convention, storage,
   body bytes, control flow, and executable data remain unchanged.
   Local calls: FixedMath_UInt64Sqrt, FixedMath_Atan2Angle16.
*/
FixedTriangleJointAnglesEaxEdx8 __thandor_eax_edx_cf_preserve_ecx
FixedGeometry_SolveTriangleJointAnglesRegs(Q12 sideLength0Q12,Q12 sideLength1Q12,Q12 sideLength2Q12)

{
  longlong lVar1;
  longlong lVar2;
  longlong lVar3;
  longlong lVar4;
  uint uVar5;
  int iVar6;
  uint uVar7;
  dword dVar8;
  dword dVar9;
  AngleTurn32 AVar10;
  AngleTurn32 AVar11;
  uint uVar12;
  FixedTriangleJointAnglesEaxEdx8 FVar13;
  FixedTriangleJointAnglesEaxEdx8 FVar14;
  longlong lVar15;
  
  iVar6 = (int)(((longlong)sideLength0Q12 * (longlong)sideLength0Q12 -
                (longlong)sideLength1Q12 * (longlong)sideLength1Q12) / (longlong)sideLength2Q12);
  lVar1 = (longlong)iVar6 * (longlong)iVar6;
  iVar6 = (int)lVar1;
  uVar5 = -iVar6;
  lVar2 = (longlong)sideLength2Q12 * (longlong)sideLength2Q12;
  uVar12 = uVar5 - (uint)lVar2;
  lVar3 = (longlong)sideLength1Q12 * (longlong)sideLength1Q12;
  uVar7 = (uint)lVar3;
  lVar4 = (longlong)sideLength0Q12 * (longlong)sideLength0Q12;
  lVar15 = (lVar2 - lVar3) + lVar4;
  lVar1 = lVar4 * 2 +
          CONCAT44((((-(uint)(iVar6 != 0) - (int)((ulonglong)lVar1 >> 0x20)) -
                    (int)((ulonglong)lVar2 >> 0x20)) - (uint)(uVar5 < (uint)lVar2)) +
                   (int)((ulonglong)lVar3 >> 0x20) * 2 + (uint)CARRY4(uVar7,uVar7) +
                   (uint)CARRY4(uVar12,uVar7 * 2),uVar12 + uVar7 * 2);
  if ((-1 < lVar1) && (0x10 < sideLength2Q12)) {
    dVar8 = FixedMath_UInt64Sqrt((UInt64Half32)((ulonglong)lVar1 >> 0x20),(UInt64Half32)lVar1);
    dVar9 = FixedMath_Atan2Angle16((int)dVar8 >> 1,(int)(lVar15 / (longlong)sideLength2Q12) >> 1);
    FVar13.jointAngle0 =
         FixedMath_Atan2Angle16
                   ((int)dVar8 >> 1,(int)(((lVar3 + lVar2) - lVar4) / (longlong)sideLength2Q12) >> 1
                   );
    FVar13.jointAngle1 = dVar9 + FVar13.jointAngle0;
    return FVar13;
  }
  if ((uint)sideLength0Q12 < (uint)sideLength2Q12) {
    AVar10 = 0;
    AVar11 = 0;
  }
  else {
    AVar10 = 0x8000;
    AVar11 = 0x8000;
  }
  FVar14.jointAngle1 = AVar11;
  FVar14.jointAngle0 = AVar10;
  return FVar14;
}


/* Address: 0x004849D0.
   Ownership: core/math/fixed.
   Purpose: Returns floor(sqrt(x*x + y*y + z*z)). Typed parameters: p0 x→FixedMathVectorComponent32_V342, p1
   y→FixedMathVectorComponent32_V342, p2 z→FixedMathVectorComponent32_V342. Calling convention, exact
   VariableStorage serialization, function body bytes, control flow, globals, locals, and executable data remain
   unchanged.
   Local calls: FixedMath_UInt64Sqrt.
*/
dword __thandor_eax_preserve_ecx_edx
FixedMath_Length3(FixedMathVectorComponent32 x,FixedMathVectorComponent32 y,
                 FixedMathVectorComponent32 z)

{
  dword vectorLengthQ12;
  longlong squaredLengthAccumulatorQ24;
  
  squaredLengthAccumulatorQ24 =
       (longlong)y * (longlong)y + (longlong)z * (longlong)z + (longlong)x * (longlong)x;
  vectorLengthQ12 =
       FixedMath_UInt64Sqrt
                 ((UInt64Half32)((ulonglong)squaredLengthAccumulatorQ24 >> 0x20),
                  (UInt64Half32)squaredLengthAccumulatorQ24);
  return vectorLengthQ12;
}


/* Address: 0x00484E50.
   Ownership: core/math/fixed.
   Purpose: Extracts the two direction angles from the transform's third basis column. EDX=elevation angle and
   ECX=azimuth angle.
   Local calls: FixedMath_VectorToAngles3Regs.
*/
FixedMathVectorAnglesRegs8 __thandor_preserve_eax
FixedTransform_ExtractForwardAnglesRegs(GraphicsFixedMatrix3x4 *transform)

{
  FixedMathVectorAnglesRegs8 FVar1;
  
  FVar1 = FixedMath_VectorToAngles3Regs
                    (transform->basisRow2[2],transform->basisRow1[2],transform->basisRow0[2]);
  return FVar1;
}


/* Address: 0x004857A0.
   Ownership: core/math/fixed.
   Purpose: Normalizes the input vector to Q28. Vectors with integer length below 2 produce {0,0,0}.
   Local calls: FixedMath_LengthVec3.
*/
void __thandor_void_preserve_eax_ecx_edx
FixedVec3_NormalizeQ28(GraphicsFixedVec3 *output,GraphicsFixedVec3 *input)

{
  dword inputLengthQ12;
  int reciprocalLengthScaleQ32;
  longlong normalizedComponentProduct;
  longlong finalNormalizedComponentProduct;
  longlong currentNormalizedComponentProduct;
  
  inputLengthQ12 = FixedMath_LengthVec3(input);
  if (inputLengthQ12 < 2) {
    output->x = 0;
    output->y = 0;
    output->z = 0;
  }
  else {
    reciprocalLengthScaleQ32 = (int)(0x100000000 / (ulonglong)inputLengthQ12);
    normalizedComponentProduct = (longlong)reciprocalLengthScaleQ32 * (longlong)input->x;
    output->x = (int)((ulonglong)normalizedComponentProduct >> 0x20) << 0x1c |
                (uint)normalizedComponentProduct >> 4;
    currentNormalizedComponentProduct = (longlong)reciprocalLengthScaleQ32 * (longlong)input->y;
    output->y = (int)((ulonglong)currentNormalizedComponentProduct >> 0x20) << 0x1c |
                (uint)currentNormalizedComponentProduct >> 4;
    finalNormalizedComponentProduct = (longlong)reciprocalLengthScaleQ32 * (longlong)input->z;
    output->z = (int)((ulonglong)finalNormalizedComponentProduct >> 0x20) << 0x1c |
                (uint)finalNormalizedComponentProduct >> 4;
  }
  return;
}


/* Address: 0x004BEC20.
   Ownership: core/math/fixed.
   Purpose: Thin register-preserving wrapper around the fixed-transform direction rotation core. Kept distinct from
   Q12 coordinates, Q4/Q5 resource scales, attachment ordinals, and raw renderer flags. Typed parameters: p3
   elevationAngle→AngleTurn32, p4 azimuthAngle→AngleTurn32, p5 rotationAngle0→AngleTurn32, p6
   rotationAngle1→AngleTurn32, p7 rotationAngle2→AngleTurn32. Calling convention, storage, body bytes, control
   flow, and executable data remain unchanged. Typed parameters: p2 directionScale→FixedMathScale32_V342.
   Local calls: FixedTransform_RotateDirectionScaledCoreRegs.
*/
FixedVectorEaxEcxEdx12
FixedTransform_RotateDirectionScaledRegs
          (FixedMathScale32 directionScale,AngleTurn32 elevationAngle,AngleTurn32 azimuthAngle,
          AngleTurn32 rotationAngle0,AngleTurn32 rotationAngle1,AngleTurn32 rotationAngle2)

{
  FixedVectorEaxEcxEdx12 FVar1;
  FixedVectorXEaxYEbxZEdx12 FVar2;
  undefined8 uVar3;
  
  FVar2 = FixedTransform_RotateDirectionScaledCoreRegs
                    (directionScale,elevationAngle,azimuthAngle,rotationAngle0,rotationAngle1,
                     rotationAngle2);
  FVar1.xQ12 = FVar2.xQ12;
  uVar3._4_4_ = FVar2.yQ12;
  register0x00000008 = FVar2.zQ12;
  return FVar1;
}


/* Address: 0x00417620.
   Ownership: core/math/fixed.
   Purpose: Allocates one 0x40000-byte image, seeds its fixed prefix, and generates two signed-16-bit lookup
   regions from g_FixedCosQ28. CF reports allocation failure.
*/
void __cdecl CosineDerivedLookupTables_InitCf(void)

{
  short *outputCursor;
  int entriesRemainingInRow;
  uint angleStep16;
  uint uVar1;
  int iVar2;
  uint angleIndex16;
  uint uVar3;
  ArenaAllocEaxCf5 AVar4;
  
  AVar4 = (*g_MemoryApi.alloc)(0x40000);
  outputCursor = (short *)AVar4.eax;
  if (!AVar4.carry) {
    g_CosineDerivedLookupAllocation = outputCursor;
    for (entriesRemainingInRow = 0x80; entriesRemainingInRow != 0;
        entriesRemainingInRow = entriesRemainingInRow + -1) {
      outputCursor[0] = 0xb50;
      outputCursor[1] = 0xb50;
      outputCursor = outputCursor + 2;
    }
    angleIndex16 = 0x40;
    angleStep16 = 0x40;
    iVar2 = 0x100;
    do {
      do {
        *outputCursor = (short)((uint)g_FixedCosQ28[angleIndex16] >> 0x10);
        outputCursor = outputCursor + 1;
        angleIndex16 = angleIndex16 + angleStep16 * 2 & 0xffff;
        iVar2 = iVar2 + -1;
      } while (iVar2 != 0);
      angleStep16 = angleStep16 + 0x40;
      iVar2 = 0x100;
      angleIndex16 = angleStep16 & 0xffff;
    } while (angleStep16 < 0x4000);
    uVar3 = 0;
    iVar2 = 0x100;
    uVar1 = 0x40;
    g_CosineDerivedLookupSecondTable = outputCursor;
    do {
      do {
        if (iVar2 == 0x100) {
          *outputCursor = 0x2d41;
        }
        else {
          *outputCursor = (short)(g_FixedCosQ28[uVar3] >> 0xe);
        }
        outputCursor = outputCursor + 1;
        uVar3 = uVar3 + uVar1 & 0xffff;
        iVar2 = iVar2 + -1;
      } while (iVar2 != 0);
      uVar1 = uVar1 + 0x80;
      iVar2 = 0x100;
      uVar3 = 0;
    } while (uVar1 < 0x8000);
  }
  return;
}


/* Address: 0x004848C0.
   Ownership: core/math/fixed.
   Purpose: Writes a Q28 unit direction vector from two wrapping 16-bit angles. Kept distinct from Q12 coordinates,
   Q4/Q5 resource scales, attachment ordinals, and raw renderer flags. Typed parameters: p1
   elevationAngle→AngleTurn32, p2 azimuthAngle→AngleTurn32. Calling convention, storage, body bytes, control flow,
   and executable data remain unchanged.
*/
void __thandor_void_preserve_eax_ecx_edx
FixedMath_WriteDirectionQ28
          (GraphicsFixedVec3 *output,AngleTurn32 elevationAngle,AngleTurn32 azimuthAngle)

{
  uint azimuthPlusElevationAngle16;
  uint azimuthMinusElevationAngle16;
  uint uVar1;
  sdword verticalSinQ28;
  int azimuthPlusElevationSinQ28;
  int azimuthMinusElevationSinQ28;
  
  azimuthMinusElevationAngle16 = elevationAngle & 0xffff;
  verticalSinQ28 = g_FixedSinQ28[azimuthMinusElevationAngle16];
  azimuthPlusElevationAngle16 = azimuthMinusElevationAngle16 + azimuthAngle & 0xffff;
  uVar1 = azimuthAngle - azimuthMinusElevationAngle16 & 0xffff;
  azimuthPlusElevationSinQ28 = g_FixedSinQ28[azimuthPlusElevationAngle16];
  azimuthMinusElevationSinQ28 = g_FixedSinQ28[uVar1];
  output->x = g_FixedCosQ28[azimuthPlusElevationAngle16] + g_FixedCosQ28[uVar1] >> 1;
  output->y = azimuthPlusElevationSinQ28 + azimuthMinusElevationSinQ28 >> 1;
  output->z = verticalSinQ28;
  return;
}


/* Address: 0x00484B00.
   Ownership: core/math/fixed.
   Purpose: Returns EAX=cos(angle)*scale and EDX=sin(angle)*scale, both using Q28 table multiplication. angle16 ->
   (sin,cos) scaled pair, reg-pair return; feeds TerrainDirectionRecord vectors and the rotation basis builder.
   Kept distinct from Q12 coordinates, Q4/Q5 resource scales, attachment ordinals, and raw renderer flags. Typed
   parameters: p0 angle→AngleTurn32. Calling convention, storage, body bytes, control flow, and executable data
   remain unchanged.
*/
FixedSinCosEdxEax8 __thandor_eax_edx_cf_preserve_ecx
FixedMath_SinCosScaled(AngleTurn32 angle,FixedMathScale32 scale)

{
  return CONCAT44((int)((ulonglong)((longlong)g_FixedSinQ28[angle & 0xffff] * (longlong)scale) >>
                       0x20) << 4 |
                  (uint)((longlong)g_FixedSinQ28[angle & 0xffff] * (longlong)scale) >> 0x1c,
                  (int)((ulonglong)((longlong)g_FixedCosQ28[angle & 0xffff] * (longlong)scale) >>
                       0x20) << 4 |
                  (uint)((longlong)g_FixedCosQ28[angle & 0xffff] * (longlong)scale) >> 0x1c);
}


/* Address: 0x00484B40.
   Ownership: core/math/fixed.
   Purpose: Returns EAX=cos(angle) and EDX=sin(angle) from the Q28 tables. Kept distinct from Q12 coordinates,
   Q4/Q5 resource scales, attachment ordinals, and raw renderer flags. Typed parameters: p0 angle→AngleTurn32.
   Calling convention, storage, body bytes, control flow, and executable data remain unchanged.
*/
FixedSinCosEdxEax8 FixedMath_SinCosQ28(AngleTurn32 angle)

{
  return CONCAT44(g_FixedSinQ28[angle & 0xffff],g_FixedCosQ28[angle & 0xffff]);
}

/* Address: 0x00484F10.
   Ownership: core/math/fixed.
   Purpose: Applies only the 3x3 Q28 basis and ignores transform->translation.
*/
void __thandor_void_preserve_eax_ecx_edx
FixedTransform_ApplyDirection
          (GraphicsFixedVec3 *output,GraphicsFixedVec3 *direction,GraphicsFixedMatrix3x4 *transform)

{
  int iVar1;
  longlong lVar2;
  int currentBasisRowComponent0Q28;
  longlong currentBasisDotProductQ40;
  longlong basisDotProductAccumulatorQ40;
  
  basisDotProductAccumulatorQ40 =
       (longlong)transform->basisRow0[1] * (longlong)direction->y +
       (longlong)transform->basisRow0[0] * (longlong)direction->x +
       (longlong)transform->basisRow0[2] * (longlong)direction->z;
  currentBasisRowComponent0Q28 = transform->basisRow1[0];
  output->x = (int)((ulonglong)basisDotProductAccumulatorQ40 >> 0x20) << 4 |
              (uint)basisDotProductAccumulatorQ40 >> 0x1c;
  currentBasisDotProductQ40 =
       (longlong)transform->basisRow1[1] * (longlong)direction->y +
       (longlong)currentBasisRowComponent0Q28 * (longlong)direction->x +
       (longlong)transform->basisRow1[2] * (longlong)direction->z;
  iVar1 = transform->basisRow2[0];
  output->y = (int)((ulonglong)currentBasisDotProductQ40 >> 0x20) << 4 |
              (uint)currentBasisDotProductQ40 >> 0x1c;
  lVar2 = (longlong)transform->basisRow2[1] * (longlong)direction->y +
          (longlong)iVar1 * (longlong)direction->x +
          (longlong)transform->basisRow2[2] * (longlong)direction->z;
  output->z = (int)((ulonglong)lVar2 >> 0x20) << 4 | (uint)lVar2 >> 0x1c;
  return;
}


/* Address: 0x00485090.
   Ownership: core/math/fixed.
   Purpose: Multiplies a direction by the transpose of the transform's 3x3 Q28 basis. Translation is ignored.
*/
void __thandor_void_preserve_eax_ecx_edx
FixedTransform_ApplyTransposeDirection
          (GraphicsFixedVec3 *output,GraphicsFixedMatrix3x4 *transform,GraphicsFixedVec3 *direction)

{
  int iVar1;
  longlong lVar2;
  longlong currentBasisDotProductQ40;
  int currentBasisColumnRow0ComponentQ28;
  longlong basisDotProductAccumulatorQ40;
  
  basisDotProductAccumulatorQ40 =
       (longlong)transform->basisRow1[0] * (longlong)direction->y +
       (longlong)transform->basisRow0[0] * (longlong)direction->x +
       (longlong)transform->basisRow2[0] * (longlong)direction->z;
  currentBasisColumnRow0ComponentQ28 = transform->basisRow0[1];
  output->x = (int)((ulonglong)basisDotProductAccumulatorQ40 >> 0x20) << 4 |
              (uint)basisDotProductAccumulatorQ40 >> 0x1c;
  currentBasisDotProductQ40 =
       (longlong)transform->basisRow1[1] * (longlong)direction->y +
       (longlong)currentBasisColumnRow0ComponentQ28 * (longlong)direction->x +
       (longlong)transform->basisRow2[1] * (longlong)direction->z;
  iVar1 = transform->basisRow0[2];
  output->y = (int)((ulonglong)currentBasisDotProductQ40 >> 0x20) << 4 |
              (uint)currentBasisDotProductQ40 >> 0x1c;
  lVar2 = (longlong)transform->basisRow1[2] * (longlong)direction->y +
          (longlong)iVar1 * (longlong)direction->x +
          (longlong)transform->basisRow2[2] * (longlong)direction->z;
  output->z = (int)((ulonglong)lVar2 >> 0x20) << 4 | (uint)lVar2 >> 0x1c;
  return;
}


/* Address: 0x00485520.
   Ownership: core/math/fixed.
   Purpose: It assumes the basis is a rotation matrix.
*/
void __thandor_void_preserve_eax_ecx_edx
FixedTransform_InvertRigidQ28(GraphicsFixedMatrix3x4 *output,GraphicsFixedMatrix3x4 *input)

{
  longlong lVar1;
  longlong lVar2;
  longlong lVar3;
  uint uVar4;
  int iVar5;
  uint uVar6;
  uint uVar7;
  int iVar8;
  uint uVar9;
  
  lVar1 = (longlong)input->basisRow1[1] * (longlong)input->basisRow2[2] -
          (longlong)input->basisRow1[2] * (longlong)input->basisRow2[1];
  iVar8 = input->basisRow1[2];
  output->basisRow0[0] = (int)((ulonglong)lVar1 >> 0x20) << 4 | (uint)lVar1 >> 0x1c;
  lVar1 = (longlong)iVar8 * (longlong)input->basisRow2[0] -
          (longlong)input->basisRow1[0] * (longlong)input->basisRow2[2];
  iVar8 = input->basisRow1[0];
  output->basisRow1[0] = (int)((ulonglong)lVar1 >> 0x20) << 4 | (uint)lVar1 >> 0x1c;
  lVar1 = (longlong)iVar8 * (longlong)input->basisRow2[1] -
          (longlong)input->basisRow1[1] * (longlong)input->basisRow2[0];
  iVar8 = input->basisRow0[2];
  output->basisRow2[0] = (int)((ulonglong)lVar1 >> 0x20) << 4 | (uint)lVar1 >> 0x1c;
  lVar1 = (longlong)iVar8 * (longlong)input->basisRow2[1] -
          (longlong)input->basisRow0[1] * (longlong)input->basisRow2[2];
  iVar8 = input->basisRow0[0];
  output->basisRow0[1] = (int)((ulonglong)lVar1 >> 0x20) << 4 | (uint)lVar1 >> 0x1c;
  lVar1 = (longlong)iVar8 * (longlong)input->basisRow2[2] -
          (longlong)input->basisRow0[2] * (longlong)input->basisRow2[0];
  iVar8 = input->basisRow0[1];
  output->basisRow1[1] = (int)((ulonglong)lVar1 >> 0x20) << 4 | (uint)lVar1 >> 0x1c;
  lVar1 = (longlong)iVar8 * (longlong)input->basisRow2[0] -
          (longlong)input->basisRow0[0] * (longlong)input->basisRow2[1];
  iVar8 = input->basisRow0[1];
  output->basisRow2[1] = (int)((ulonglong)lVar1 >> 0x20) << 4 | (uint)lVar1 >> 0x1c;
  lVar1 = (longlong)iVar8 * (longlong)input->basisRow1[2] -
          (longlong)input->basisRow0[2] * (longlong)input->basisRow1[1];
  iVar8 = input->basisRow0[2];
  output->basisRow0[2] = (int)((ulonglong)lVar1 >> 0x20) << 4 | (uint)lVar1 >> 0x1c;
  lVar1 = (longlong)iVar8 * (longlong)input->basisRow1[0] -
          (longlong)input->basisRow0[0] * (longlong)input->basisRow1[2];
  iVar8 = input->basisRow0[0];
  output->basisRow1[2] = (int)((ulonglong)lVar1 >> 0x20) << 4 | (uint)lVar1 >> 0x1c;
  lVar1 = (longlong)iVar8 * (longlong)input->basisRow1[1] -
          (longlong)input->basisRow0[1] * (longlong)input->basisRow1[0];
  output->basisRow2[2] = (int)((ulonglong)lVar1 >> 0x20) << 4 | (uint)lVar1 >> 0x1c;
  lVar1 = (longlong)(input->translation).x * (longlong)output->basisRow0[0];
  iVar5 = (int)lVar1;
  uVar4 = -iVar5;
  lVar2 = (longlong)(input->translation).y * (longlong)output->basisRow0[1];
  uVar6 = (uint)lVar2;
  uVar9 = uVar4 - uVar6;
  lVar3 = (longlong)(input->translation).z * (longlong)output->basisRow0[2];
  uVar7 = (uint)lVar3;
  iVar8 = (input->translation).x;
  (output->translation).x =
       (((((-(uint)(iVar5 != 0) - (int)((ulonglong)lVar1 >> 0x20)) - (int)((ulonglong)lVar2 >> 0x20)
          ) - (uint)(uVar4 < uVar6)) - (int)((ulonglong)lVar3 >> 0x20)) - (uint)(uVar9 < uVar7)) *
       0x10 | uVar9 - uVar7 >> 0x1c;
  lVar1 = (longlong)iVar8 * (longlong)output->basisRow1[0];
  iVar5 = (int)lVar1;
  uVar4 = -iVar5;
  lVar2 = (longlong)(input->translation).y * (longlong)output->basisRow1[1];
  uVar6 = (uint)lVar2;
  uVar9 = uVar4 - uVar6;
  lVar3 = (longlong)(input->translation).z * (longlong)output->basisRow1[2];
  uVar7 = (uint)lVar3;
  iVar8 = (input->translation).x;
  (output->translation).y =
       (((((-(uint)(iVar5 != 0) - (int)((ulonglong)lVar1 >> 0x20)) - (int)((ulonglong)lVar2 >> 0x20)
          ) - (uint)(uVar4 < uVar6)) - (int)((ulonglong)lVar3 >> 0x20)) - (uint)(uVar9 < uVar7)) *
       0x10 | uVar9 - uVar7 >> 0x1c;
  lVar1 = (longlong)iVar8 * (longlong)output->basisRow2[0];
  iVar8 = (int)lVar1;
  uVar4 = -iVar8;
  lVar2 = (longlong)(input->translation).y * (longlong)output->basisRow2[1];
  uVar6 = (uint)lVar2;
  uVar9 = uVar4 - uVar6;
  lVar3 = (longlong)(input->translation).z * (longlong)output->basisRow2[2];
  uVar7 = (uint)lVar3;
  (output->translation).z =
       (((((-(uint)(iVar8 != 0) - (int)((ulonglong)lVar1 >> 0x20)) - (int)((ulonglong)lVar2 >> 0x20)
          ) - (uint)(uVar4 < uVar6)) - (int)((ulonglong)lVar3 >> 0x20)) - (uint)(uVar9 < uVar7)) *
       0x10 | uVar9 - uVar7 >> 0x1c;
  return;
}


/* Address: 0x004856B0.
   Ownership: core/math/fixed.
   Purpose: Returns (left.x*right.x + left.y*right.y + left.z*right.z) shifted right by 12.
*/
sdword __thandor_eax_preserve_ecx_edx
FixedVec3_DotQ12(GraphicsFixedVec3 *left,GraphicsFixedVec3 *right)

{
  longlong dotProductAccumulatorQ24;
  
  dotProductAccumulatorQ24 =
       (longlong)right->y * (longlong)left->y + (longlong)right->x * (longlong)left->x +
       (longlong)right->z * (longlong)left->z;
  return (uint)dotProductAccumulatorQ24 >> 0xc |
         (int)((ulonglong)dotProductAccumulatorQ24 >> 0x20) << 0x14;
}


/* Address: 0x004856F0.
   Ownership: core/math/fixed.
   Purpose: Returns (left.x*right.x + left.y*right.y + left.z*right.z) shifted right by 28.
*/
sdword __thandor_eax_preserve_ecx_edx
FixedVec3_DotQ28(GraphicsFixedVec3 *left,GraphicsFixedVec3 *right)

{
  longlong dotProductAccumulatorQ56;
  
  dotProductAccumulatorQ56 =
       (longlong)right->y * (longlong)left->y + (longlong)right->x * (longlong)left->x +
       (longlong)right->z * (longlong)left->z;
  return (uint)dotProductAccumulatorQ56 >> 0x1c |
         (int)((ulonglong)dotProductAccumulatorQ56 >> 0x20) << 4;
}


/* Address: 0x00485730.
   Ownership: core/math/fixed.
   Purpose: Writes leftOperand cross rightOperand, shifted right by 12. The executable's stack order is output,
   rightOperand, leftOperand.
*/
void __thandor_void_preserve_eax_ecx_edx
FixedVec3_CrossQ12(GraphicsFixedVec3 *output,GraphicsFixedVec3 *rightOperand,
                  GraphicsFixedVec3 *leftOperand)

{
  int iVar1;
  longlong lVar2;
  int currentLeftComponentQ12;
  longlong currentCrossProductDifferenceQ24;
  longlong crossComponentProductDifferenceQ24;
  
  crossComponentProductDifferenceQ24 =
       (longlong)leftOperand->y * (longlong)rightOperand->z -
       (longlong)leftOperand->z * (longlong)rightOperand->y;
  currentLeftComponentQ12 = leftOperand->z;
  output->x = (int)((ulonglong)crossComponentProductDifferenceQ24 >> 0x20) << 0x14 |
              (uint)crossComponentProductDifferenceQ24 >> 0xc;
  currentCrossProductDifferenceQ24 =
       (longlong)currentLeftComponentQ12 * (longlong)rightOperand->x -
       (longlong)leftOperand->x * (longlong)rightOperand->z;
  iVar1 = leftOperand->x;
  output->y = (int)((ulonglong)currentCrossProductDifferenceQ24 >> 0x20) << 0x14 |
              (uint)currentCrossProductDifferenceQ24 >> 0xc;
  lVar2 = (longlong)iVar1 * (longlong)rightOperand->y -
          (longlong)leftOperand->y * (longlong)rightOperand->x;
  output->z = (int)((ulonglong)lVar2 >> 0x20) << 0x14 | (uint)lVar2 >> 0xc;
  return;
}


/* Address: 0x0052AD50.
   Ownership: core/math/fixed.
   Purpose: Projects a planar point from a signed distance, 16-bit angle, and base coordinates using the fixed
   sine/cosine Q28 tables. EAX returns baseX plus cosine displacement and EDX returns baseY plus sine displacement.
   Kept distinct from Q12 coordinates, Q4/Q5 resource scales, attachment ordinals, and raw renderer flags. Typed
   parameters: p1 angle16→AngleTurn32. Calling convention, storage, body bytes, control flow, and executable data
   remain unchanged.
*/
FixedPlanarPointEdxEax8
FixedTrig_ProjectPlanarPointRegs(Q12 distance,AngleTurn32 angle16,Q12 baseY,Q12 baseX)

{
  return CONCAT44(((int)((ulonglong)((longlong)g_FixedSinQ28[angle16 & 0xffff] * (longlong)distance)
                        >> 0x20) << 4 |
                  (uint)((longlong)g_FixedSinQ28[angle16 & 0xffff] * (longlong)distance) >> 0x1c) +
                  baseY,baseX + ((int)((ulonglong)
                                       ((longlong)g_FixedCosQ28[angle16 & 0xffff] *
                                       (longlong)distance) >> 0x20) << 4 |
                                (uint)((longlong)g_FixedCosQ28[angle16 & 0xffff] *
                                      (longlong)distance) >> 0x1c));
}

/* Address: 0x004BEC50.
   Ownership: core/math/fixed.
   Purpose: Builds a rotation basis, creates a scaled direction vector from two angles, rotates it through the
   basis, and returns the transformed vector through the engine register convention. Kept distinct from Q12
   coordinates, Q4/Q5 resource scales, attachment ordinals, and raw renderer flags. Typed parameters: p3
   elevationAngle→AngleTurn32, p4 azimuthAngle→AngleTurn32, p5 rotationAngle0→AngleTurn32, p6
   rotationAngle1→AngleTurn32, p7 rotationAngle2→AngleTurn32. Calling convention, storage, body bytes, control
   flow, and executable data remain unchanged. Typed parameters: p2 directionScale→FixedMathScale32_V342.
   Local calls: FixedTransform_BuildRotationBasis, FixedMath_WriteDirectionScaled, FixedTransform_ApplyPoint.
*/
FixedVectorXEaxYEbxZEdx12 __thandor_eax_edx_cf_preserve_ecx
FixedTransform_RotateDirectionScaledCoreRegs
          (FixedMathScale32 directionScale,AngleTurn32 elevationAngle,AngleTurn32 azimuthAngle,
          AngleTurn32 rotationAngle0,AngleTurn32 rotationAngle1,AngleTurn32 rotationAngle2)

{
  FixedVectorXEaxYEbxZEdx12 FVar1;
  
  FixedTransform_BuildRotationBasis
            ((GraphicsFixedMatrix3x4 *)&g_ModelTransformScratchMatrix,rotationAngle0,rotationAngle1,
             rotationAngle2);
  FixedMath_WriteDirectionScaled
            ((GraphicsFixedVec3 *)&g_ModelTransformInputX,elevationAngle,azimuthAngle,directionScale
            );
  FixedTransform_ApplyPoint
            ((GraphicsFixedVec3 *)&g_ModelTransformOutputX,
             (GraphicsFixedVec3 *)&g_ModelTransformInputX,
             (GraphicsFixedMatrix3x4 *)&g_ModelTransformScratchMatrix);
  FVar1.yQ12 = g_ModelTransformOutputY;
  FVar1.xQ12 = g_ModelTransformOutputX;
  FVar1.zQ12 = g_ModelTransformOutputZ;
  return FVar1;
}


/* Address: 0x00484A70.
   Ownership: core/math/fixed.
   Purpose: Pointer form of FixedMath_VectorToAngles3Regs. EDX=elevation angle and ECX=azimuth angle.
   Local calls: FixedMath_UInt64Sqrt, FixedMath_Atan2Angle16.
*/
FixedMathVectorAnglesRegs8 __thandor_preserve_eax
FixedMath_VectorToAnglesVec3Regs(GraphicsFixedVec3 *vector)

{
  dword dVar1;
  dword dVar2;
  int y;
  int x;
  longlong horizontalSquaredLengthAccumulatorQ24;
  
  x = vector->x;
  y = vector->y;
  horizontalSquaredLengthAccumulatorQ24 = (longlong)y * (longlong)y + (longlong)x * (longlong)x;
  dVar1 = FixedMath_UInt64Sqrt
                    ((UInt64Half32)((ulonglong)horizontalSquaredLengthAccumulatorQ24 >> 0x20),
                     (UInt64Half32)horizontalSquaredLengthAccumulatorQ24);
  dVar1 = FixedMath_Atan2Angle16(vector->z,dVar1);
  dVar2 = FixedMath_Atan2Angle16(y,x);
  return (FixedMathVectorAnglesRegs8)(CONCAT44(dVar2,dVar1) & 0xffffffffffff);
}


/* Address: 0x00484E00.
   Ownership: core/math/fixed.
   Purpose: Extracts three wrapping 16-bit orientation angles from the 3x3 basis. EAX, EDX, and ECX hold the three
   results. The declared 64-bit return models EDX:EAX; ECX remains an extra output.
   Local calls: FixedMath_VectorToAngles3Regs, FixedMath_Atan2Angle16.
*/
FixedEulerAnglesEaxEcxEdx12 FixedTransform_ExtractEulerAnglesRegs(GraphicsFixedMatrix3x4 *transform)

{
  dword extractedRotationAngle2;
  dword dVar1;
  FixedMathVectorAnglesRegs8 FVar2;
  FixedEulerAnglesEaxEcxEdx12 FVar3;
  
  FVar2 = FixedMath_VectorToAngles3Regs
                    (transform->basisRow2[2],transform->basisRow1[2],transform->basisRow0[2]);
  FVar3.edxAngle = FVar2.edx;
  FVar3.ecxAngle = FVar2.ecx;
  if ((longlong)FVar2 < 0) {
    dVar1 = FixedMath_Atan2Angle16
                      (transform->basisRow1[0] + transform->basisRow0[1],
                       transform->basisRow1[1] - transform->basisRow0[0]);
    extractedRotationAngle2 = dVar1 + FVar3.ecxAngle * 2 & 0xffff;
  }
  else {
    extractedRotationAngle2 =
         FixedMath_Atan2Angle16
                   (transform->basisRow1[0] - transform->basisRow0[1],
                    transform->basisRow1[1] + transform->basisRow0[0]);
  }
  FVar3.eaxAngle = extractedRotationAngle2;
  return FVar3;
}


/* Address: 0x00484AC0.
   Ownership: core/math/fixed.
   Purpose: Returns floor(sqrt(vector->x^2 + vector->y^2 + vector->z^2)).
   Local calls: FixedMath_UInt64Sqrt.
*/
dword __thandor_eax_preserve_ecx_edx FixedMath_LengthVec3(GraphicsFixedVec3 *vector)

{
  dword vectorLengthQ12;
  longlong squaredLengthAccumulatorQ24;
  
  squaredLengthAccumulatorQ24 =
       (longlong)vector->y * (longlong)vector->y + (longlong)vector->x * (longlong)vector->x +
       (longlong)vector->z * (longlong)vector->z;
  vectorLengthQ12 =
       FixedMath_UInt64Sqrt
                 ((UInt64Half32)((ulonglong)squaredLengthAccumulatorQ24 >> 0x20),
                  (UInt64Half32)squaredLengthAccumulatorQ24);
  return vectorLengthQ12;
}


/* Address: 0x00484CF0.
   Ownership: core/math/fixed.
   Purpose: Returns floor(sqrt(x*x + y*y)). Typed parameters: p0 x→FixedMathVectorComponent32_V342, p1
   y→FixedMathVectorComponent32_V342. Calling convention, exact VariableStorage serialization, function body bytes,
   control flow, globals, locals, and executable data remain unchanged.
   Local calls: FixedMath_UInt64Sqrt.
*/
dword __thandor_eax_preserve_ecx_edx
FixedMath_Length2(FixedMathVectorComponent32 x,FixedMathVectorComponent32 y)

{
  dword vectorLengthQ12;
  longlong squaredLengthAccumulatorQ24;
  
  squaredLengthAccumulatorQ24 = (longlong)x * (longlong)x + (longlong)y * (longlong)y;
  vectorLengthQ12 =
       FixedMath_UInt64Sqrt
                 ((UInt64Half32)((ulonglong)squaredLengthAccumulatorQ24 >> 0x20),
                  (UInt64Half32)squaredLengthAccumulatorQ24);
  return vectorLengthQ12;
}


/* Address: 0x00484770.
   Ownership: core/math/fixed.
   Purpose: Builds a scaled direction vector. Register outputs are EAX=x, ECX=y, EDX=z. The declared 64-bit C
   return models only EDX:EAX; ECX remains an extra output. Scaled direction vector from angle pair; projectile
   velocity seeding in the shot creator. Kept distinct from Q12 coordinates, Q4/Q5 resource scales, attachment
   ordinals, and raw renderer flags.
*/
FixedDirectionXyzRegs12
FixedMath_DirectionFromAnglesScaledRegs
          (AngleTurn32 elevationAngle,AngleTurn32 azimuthAngle,FixedMathScale32 scale)

{
  longlong lVar1;
  uint uVar2;
  uint elevationAngle16;
  uint uVar3;
  FixedDirectionXyzRegs12 FVar4;
  longlong scaledHorizontalComponentProduct;
  
  elevationAngle16 = elevationAngle & 0xffff;
  uVar2 = elevationAngle16 + azimuthAngle & 0xffff;
  uVar3 = azimuthAngle - elevationAngle16 & 0xffff;
  scaledHorizontalComponentProduct =
       (longlong)(g_FixedCosQ28[uVar2] + g_FixedCosQ28[uVar3]) * (longlong)scale;
  lVar1 = (longlong)(g_FixedSinQ28[uVar2] + g_FixedSinQ28[uVar3]) * (longlong)scale;
  FVar4.edx = (int)((ulonglong)((longlong)g_FixedSinQ28[elevationAngle16] * (longlong)scale) >> 0x20
                   ) << 4 |
              (uint)((longlong)g_FixedSinQ28[elevationAngle16] * (longlong)scale) >> 0x1c;
  FVar4.ecx = (int)((ulonglong)lVar1 >> 0x20) << 3 | (uint)lVar1 >> 0x1d;
  FVar4.eax = (int)((ulonglong)scaledHorizontalComponentProduct >> 0x20) << 3 |
              (uint)scaledHorizontalComponentProduct >> 0x1d;
  return FVar4;
}


/* Address: 0x004847E0.
   Ownership: core/math/fixed.
   Purpose: Builds a Q28 direction vector. Register outputs are EAX=x, ECX=y, EDX=z. The declared 64-bit C return
   models only EDX:EAX; ECX remains an extra output. (heading, pitch) angle16 pair -> Q28 direction vector;
   rotation-basis row builder. Kept distinct from Q12 coordinates, Q4/Q5 resource scales, attachment ordinals, and
   raw renderer flags.
*/
FixedDirectionXyzRegs12
FixedMath_DirectionFromAnglesQ28Regs(AngleTurn32 elevationAngle,AngleTurn32 azimuthAngle)

{
  uint uVar1;
  uint elevationAngle16;
  uint uVar2;
  FixedDirectionXyzRegs12 FVar3;
  
  elevationAngle16 = elevationAngle & 0xffff;
  uVar1 = elevationAngle16 + azimuthAngle & 0xffff;
  uVar2 = azimuthAngle - elevationAngle16 & 0xffff;
  FVar3.eax = g_FixedCosQ28[uVar1] + g_FixedCosQ28[uVar2] >> 1;
  FVar3.ecx = g_FixedSinQ28[uVar1] + g_FixedSinQ28[uVar2] >> 1;
  FVar3.edx = g_FixedSinQ28[elevationAngle16];
  return FVar3;
}


/* Address: 0x00484840.
   Ownership: core/math/fixed.
   Purpose: Writes {cos(elevation)*cos(azimuth), cos(elevation)*sin(azimuth), sin(elevation)} multiplied by scale.
   Kept distinct from Q12 coordinates, Q4/Q5 resource scales, attachment ordinals, and raw renderer flags. Typed
   parameters: p1 elevationAngle→AngleTurn32, p2 azimuthAngle→AngleTurn32. Calling convention, storage, body bytes,
   control flow, and executable data remain unchanged. Typed parameters: p3 scale→FixedMathScale32_V342.
*/
void __thandor_void_preserve_eax_ecx_edx
FixedMath_WriteDirectionScaled
          (GraphicsFixedVec3 *output,AngleTurn32 elevationAngle,AngleTurn32 azimuthAngle,
          FixedMathScale32 scale)

{
  uint azimuthPlusElevationAngle16;
  uint uVar1;
  int verticalSinQ28;
  int azimuthPlusElevationSinQ28;
  int azimuthMinusElevationSinQ28;
  longlong yComponentScaleProduct;
  longlong horizontalComponentScaleProduct;
  
  uVar1 = elevationAngle & 0xffff;
  verticalSinQ28 = g_FixedSinQ28[uVar1];
  azimuthPlusElevationAngle16 = uVar1 + azimuthAngle & 0xffff;
  uVar1 = azimuthAngle - uVar1 & 0xffff;
  azimuthPlusElevationSinQ28 = g_FixedSinQ28[azimuthPlusElevationAngle16];
  azimuthMinusElevationSinQ28 = g_FixedSinQ28[uVar1];
  horizontalComponentScaleProduct =
       (longlong)(g_FixedCosQ28[azimuthPlusElevationAngle16] + g_FixedCosQ28[uVar1]) *
       (longlong)scale;
  output->x = (int)((ulonglong)horizontalComponentScaleProduct >> 0x20) << 3 |
              (uint)horizontalComponentScaleProduct >> 0x1d;
  yComponentScaleProduct =
       (longlong)(azimuthPlusElevationSinQ28 + azimuthMinusElevationSinQ28) * (longlong)scale;
  output->y = (int)((ulonglong)yComponentScaleProduct >> 0x20) << 3 |
              (uint)yComponentScaleProduct >> 0x1d;
  output->z = (int)((ulonglong)((longlong)verticalSinQ28 * (longlong)scale) >> 0x20) << 4 |
              (uint)((longlong)verticalSinQ28 * (longlong)scale) >> 0x1c;
  return;
}


/* Address: 0x00485120.
   Ownership: core/math/fixed.
   Purpose: Composes two Q28 transforms. The executable calculates output = transformB * transformA, including
   translation. Fixed 3x4 transform concatenation (basis multiply + translation accumulate), Q12 rounding via >>
   12.
*/
void __thandor_void_preserve_eax_ecx_edx
FixedTransform_Compose
          (GraphicsFixedMatrix3x4 *output,GraphicsFixedMatrix3x4 *transformA,
          GraphicsFixedMatrix3x4 *transformB)

{
  int iVar1;
  longlong lVar2;
  int currentRightBasisRowComponent0Q28;
  longlong currentComposedComponentProductSumQ56;
  
  currentComposedComponentProductSumQ56 =
       (longlong)transformB->basisRow0[1] * (longlong)transformA->basisRow1[0] +
       (longlong)transformB->basisRow0[0] * (longlong)transformA->basisRow0[0] +
       (longlong)transformB->basisRow0[2] * (longlong)transformA->basisRow2[0];
  currentRightBasisRowComponent0Q28 = transformB->basisRow0[0];
  output->basisRow0[0] =
       (int)((ulonglong)currentComposedComponentProductSumQ56 >> 0x20) << 4 |
       (uint)currentComposedComponentProductSumQ56 >> 0x1c;
  lVar2 = (longlong)transformB->basisRow0[1] * (longlong)transformA->basisRow1[1] +
          (longlong)currentRightBasisRowComponent0Q28 * (longlong)transformA->basisRow0[1] +
          (longlong)transformB->basisRow0[2] * (longlong)transformA->basisRow2[1];
  iVar1 = transformB->basisRow0[0];
  output->basisRow0[1] = (int)((ulonglong)lVar2 >> 0x20) << 4 | (uint)lVar2 >> 0x1c;
  lVar2 = (longlong)transformB->basisRow0[1] * (longlong)transformA->basisRow1[2] +
          (longlong)iVar1 * (longlong)transformA->basisRow0[2] +
          (longlong)transformB->basisRow0[2] * (longlong)transformA->basisRow2[2];
  iVar1 = transformB->basisRow0[0];
  output->basisRow0[2] = (int)((ulonglong)lVar2 >> 0x20) << 4 | (uint)lVar2 >> 0x1c;
  lVar2 = (longlong)transformB->basisRow0[1] * (longlong)(transformA->translation).y +
          (longlong)iVar1 * (longlong)(transformA->translation).x +
          (longlong)transformB->basisRow0[2] * (longlong)(transformA->translation).z;
  iVar1 = transformB->basisRow1[0];
  (output->translation).x =
       ((int)((ulonglong)lVar2 >> 0x20) << 4 | (uint)lVar2 >> 0x1c) + (transformB->translation).x;
  lVar2 = (longlong)transformB->basisRow1[1] * (longlong)transformA->basisRow1[0] +
          (longlong)iVar1 * (longlong)transformA->basisRow0[0] +
          (longlong)transformB->basisRow1[2] * (longlong)transformA->basisRow2[0];
  iVar1 = transformB->basisRow1[0];
  output->basisRow1[0] = (int)((ulonglong)lVar2 >> 0x20) << 4 | (uint)lVar2 >> 0x1c;
  lVar2 = (longlong)transformB->basisRow1[1] * (longlong)transformA->basisRow1[1] +
          (longlong)iVar1 * (longlong)transformA->basisRow0[1] +
          (longlong)transformB->basisRow1[2] * (longlong)transformA->basisRow2[1];
  iVar1 = transformB->basisRow1[0];
  output->basisRow1[1] = (int)((ulonglong)lVar2 >> 0x20) << 4 | (uint)lVar2 >> 0x1c;
  lVar2 = (longlong)transformB->basisRow1[1] * (longlong)transformA->basisRow1[2] +
          (longlong)iVar1 * (longlong)transformA->basisRow0[2] +
          (longlong)transformB->basisRow1[2] * (longlong)transformA->basisRow2[2];
  iVar1 = transformB->basisRow1[0];
  output->basisRow1[2] = (int)((ulonglong)lVar2 >> 0x20) << 4 | (uint)lVar2 >> 0x1c;
  lVar2 = (longlong)transformB->basisRow1[1] * (longlong)(transformA->translation).y +
          (longlong)iVar1 * (longlong)(transformA->translation).x +
          (longlong)transformB->basisRow1[2] * (longlong)(transformA->translation).z;
  iVar1 = transformB->basisRow2[0];
  (output->translation).y =
       ((int)((ulonglong)lVar2 >> 0x20) << 4 | (uint)lVar2 >> 0x1c) + (transformB->translation).y;
  lVar2 = (longlong)transformB->basisRow2[1] * (longlong)transformA->basisRow1[0] +
          (longlong)iVar1 * (longlong)transformA->basisRow0[0] +
          (longlong)transformB->basisRow2[2] * (longlong)transformA->basisRow2[0];
  iVar1 = transformB->basisRow2[0];
  output->basisRow2[0] = (int)((ulonglong)lVar2 >> 0x20) << 4 | (uint)lVar2 >> 0x1c;
  lVar2 = (longlong)transformB->basisRow2[1] * (longlong)transformA->basisRow1[1] +
          (longlong)iVar1 * (longlong)transformA->basisRow0[1] +
          (longlong)transformB->basisRow2[2] * (longlong)transformA->basisRow2[1];
  iVar1 = transformB->basisRow2[0];
  output->basisRow2[1] = (int)((ulonglong)lVar2 >> 0x20) << 4 | (uint)lVar2 >> 0x1c;
  lVar2 = (longlong)transformB->basisRow2[1] * (longlong)transformA->basisRow1[2] +
          (longlong)iVar1 * (longlong)transformA->basisRow0[2] +
          (longlong)transformB->basisRow2[2] * (longlong)transformA->basisRow2[2];
  iVar1 = transformB->basisRow2[0];
  output->basisRow2[2] = (int)((ulonglong)lVar2 >> 0x20) << 4 | (uint)lVar2 >> 0x1c;
  lVar2 = (longlong)transformB->basisRow2[1] * (longlong)(transformA->translation).y +
          (longlong)iVar1 * (longlong)(transformA->translation).x +
          (longlong)transformB->basisRow2[2] * (longlong)(transformA->translation).z;
  (output->translation).z =
       ((int)((ulonglong)lVar2 >> 0x20) << 4 | (uint)lVar2 >> 0x1c) + (transformB->translation).z;
  return;
}


/* Address: 0x00484990.
   Ownership: core/math/fixed.
   Purpose: Calculates two wrapping 16-bit vector angles. EDX=elevation angle and ECX=azimuth angle; EAX is
   preserved rather than used as a C return. Vector -> (heading, pitch) angle16 pair, reg-pair return. Typed
   parameters: p0 x→FixedMathVectorComponent32_V342, p1 y→FixedMathVectorComponent32_V342, p2
   z→FixedMathVectorComponent32_V342. Calling convention, exact VariableStorage serialization, function body bytes,
   control flow, globals, locals, and executable data remain unchanged.
   Local calls: FixedMath_UInt64Sqrt, FixedMath_Atan2Angle16.
*/
FixedMathVectorAnglesRegs8 __thandor_preserve_eax
FixedMath_VectorToAngles3Regs
          (FixedMathVectorComponent32 x,FixedMathVectorComponent32 y,FixedMathVectorComponent32 z)

{
  dword horizontalMagnitudeQ12;
  dword dVar1;
  dword dVar2;
  longlong horizontalMagnitudeSquaredQ24;
  
  horizontalMagnitudeSquaredQ24 = (longlong)y * (longlong)y + (longlong)z * (longlong)z;
  horizontalMagnitudeQ12 =
       FixedMath_UInt64Sqrt
                 ((UInt64Half32)((ulonglong)horizontalMagnitudeSquaredQ24 >> 0x20),
                  (UInt64Half32)horizontalMagnitudeSquaredQ24);
  dVar1 = FixedMath_Atan2Angle16(x,horizontalMagnitudeQ12);
  dVar2 = FixedMath_Atan2Angle16(y,z);
  return (FixedMathVectorAnglesRegs8)(CONCAT44(dVar1,dVar2) & 0xffffffff0000ffff);
}


/* Address: 0x00484E70.
   Ownership: core/math/fixed.
   Purpose: Applies the 3x3 Q28 basis and then adds transform->translation. Point through 3x4 fixed transform
   (rotate + translate).
*/
void __thandor_void_preserve_eax_ecx_edx
FixedTransform_ApplyPoint
          (GraphicsFixedVec3 *output,GraphicsFixedVec3 *point,GraphicsFixedMatrix3x4 *transform)

{
  int iVar1;
  longlong lVar2;
  int currentBasisRowComponent0Q28;
  longlong currentBasisDotProductQ40;
  
  currentBasisDotProductQ40 =
       (longlong)transform->basisRow0[1] * (longlong)point->y +
       (longlong)transform->basisRow0[0] * (longlong)point->x +
       (longlong)transform->basisRow0[2] * (longlong)point->z;
  currentBasisRowComponent0Q28 = transform->basisRow1[0];
  output->x = ((int)((ulonglong)currentBasisDotProductQ40 >> 0x20) << 4 |
              (uint)currentBasisDotProductQ40 >> 0x1c) + (transform->translation).x;
  lVar2 = (longlong)transform->basisRow1[1] * (longlong)point->y +
          (longlong)currentBasisRowComponent0Q28 * (longlong)point->x +
          (longlong)transform->basisRow1[2] * (longlong)point->z;
  iVar1 = transform->basisRow2[0];
  output->y = ((int)((ulonglong)lVar2 >> 0x20) << 4 | (uint)lVar2 >> 0x1c) +
              (transform->translation).y;
  lVar2 = (longlong)transform->basisRow2[1] * (longlong)point->y +
          (longlong)iVar1 * (longlong)point->x +
          (longlong)transform->basisRow2[2] * (longlong)point->z;
  output->z = ((int)((ulonglong)lVar2 >> 0x20) << 4 | (uint)lVar2 >> 0x1c) +
              (transform->translation).z;
  return;
}


/* Address: 0x00484D20.
   Ownership: core/math/fixed.
   Purpose: Writes the nine contiguous Q28 basis coefficients. The caller initializes the following translation
   vector separately. Three euler angle16s -> 3x3 Q12 basis via FixedMath_SinCosScaled; the compose/apply
   primitives build on it. Kept distinct from Q12 coordinates, Q4/Q5 resource scales, attachment ordinals, and raw
   renderer flags. Typed parameters: p1 angle0→AngleTurn32, p2 angle1→AngleTurn32, p3 angle2→AngleTurn32.
   Local calls: FixedMath_DirectionFromAnglesQ28Regs.
*/
void __thandor_void_preserve_eax_ecx_edx
FixedTransform_BuildRotationBasis
          (GraphicsFixedMatrix3x4 *output,AngleTurn32 angle0,AngleTurn32 angle1,AngleTurn32 angle2)

{
  longlong lVar1;
  int currentSymmetricComponentQ28;
  int iVar2;
  uint uVar3;
  uint secondarySymmetricAngleIndex16;
  uint primarySymmetricAngleIndex16;
  FixedDirectionXZEdxEax8 directionSamplePairQ28;
  FixedDirectionXZEdxEax8 secondaryDirectionSamplePairQ28;
  FixedDirectionXyzRegs12 FVar4;
  longlong currentComponentTimesVerticalSinProduct;
  int verticalSinQ28;
  longlong componentTimesVerticalSinProduct;
  longlong secondaryComponentTimesVerticalSinProduct;
  
  FVar4 = FixedMath_DirectionFromAnglesQ28Regs(angle1,angle2);
  output->basisRow0[2] = FVar4.eax;
  output->basisRow1[2] = FVar4.ecx;
  output->basisRow2[2] = FVar4.edx;
  FVar4 = FixedMath_DirectionFromAnglesQ28Regs(angle1,angle0 - angle2);
  secondarySymmetricAngleIndex16 = (angle0 - angle2) + angle2;
  output->basisRow2[1] = FVar4.ecx;
  output->basisRow2[0] = -FVar4.eax;
  primarySymmetricAngleIndex16 = secondarySymmetricAngleIndex16 & 0xffff;
  uVar3 = secondarySymmetricAngleIndex16 + angle2 * -2 & 0xffff;
  currentSymmetricComponentQ28 =
       g_FixedCosQ28[primarySymmetricAngleIndex16] - g_FixedCosQ28[uVar3] >> 1;
  verticalSinQ28 = g_FixedSinQ28[angle1 & 0xffff];
  output->basisRow0[0] = currentSymmetricComponentQ28;
  iVar2 = g_FixedCosQ28[primarySymmetricAngleIndex16] + g_FixedCosQ28[uVar3] >> 1;
  output->basisRow1[1] =
       ((int)((ulonglong)((longlong)currentSymmetricComponentQ28 * (longlong)verticalSinQ28) >> 0x20
             ) << 4 |
       (uint)((longlong)currentSymmetricComponentQ28 * (longlong)verticalSinQ28) >> 0x1c) + iVar2;
  currentComponentTimesVerticalSinProduct = (longlong)iVar2 * (longlong)verticalSinQ28;
  output->basisRow0[0] =
       output->basisRow0[0] +
       ((int)((ulonglong)currentComponentTimesVerticalSinProduct >> 0x20) << 4 |
       (uint)currentComponentTimesVerticalSinProduct >> 0x1c);
  iVar2 = g_FixedSinQ28[primarySymmetricAngleIndex16] + g_FixedSinQ28[uVar3] >> 1;
  output->basisRow1[0] = iVar2;
  lVar1 = (longlong)iVar2 * (longlong)verticalSinQ28;
  iVar2 = g_FixedSinQ28[uVar3] - g_FixedSinQ28[primarySymmetricAngleIndex16] >> 1;
  secondaryComponentTimesVerticalSinProduct = (longlong)iVar2 * (longlong)verticalSinQ28;
  output->basisRow0[1] = iVar2 - ((int)((ulonglong)lVar1 >> 0x20) << 4 | (uint)lVar1 >> 0x1c);
  output->basisRow1[0] =
       output->basisRow1[0] -
       ((int)((ulonglong)secondaryComponentTimesVerticalSinProduct >> 0x20) << 4 |
       (uint)secondaryComponentTimesVerticalSinProduct >> 0x1c);
  return;
}


/* Address: 0x00484BA0.
   Ownership: core/math/fixed.
   Purpose: Approximates atan2(y, x) as a wrapping 16-bit engine angle. Fixed-point atan2 -> angle16; euler
   extraction callee. Typed parameters: p0 y→FixedMathVectorComponent32_V342, p1 x→FixedMathVectorComponent32_V342.
   Calling convention, exact VariableStorage serialization, function body bytes, control flow, globals, locals, and
   executable data remain unchanged.
*/
dword __thandor_eax_preserve_ecx_edx
FixedMath_Atan2Angle16(FixedMathVectorComponent32 y,FixedMathVectorComponent32 x)

{
  dword angle16Result;
  int iVar1;
  uint reducedAngleNumerator;
  int iVar2;
  AngleTurn16Stored32 octantBaseAngle16;
  
  octantBaseAngle16 = 0;
  iVar1 = x * 2;
  iVar2 = y * 2;
  if (iVar2 - x == 0 || iVar2 < x) {
    if (iVar1 < -y) {
      if (y < iVar1) {
        octantBaseAngle16 = -0x4000;
        iVar1 = -y;
        reducedAngleNumerator = x;
      }
      else {
        reducedAngleNumerator = x - y;
        octantBaseAngle16 = -0x6000;
        iVar1 = -(x + y);
      }
    }
    else {
      iVar1 = x;
      reducedAngleNumerator = y;
      if (iVar2 < -x) {
        reducedAngleNumerator = x + y;
        octantBaseAngle16 = -0x2000;
        iVar1 = x - y;
      }
    }
  }
  else if (iVar1 < -y) {
    if (iVar2 < -x) {
      octantBaseAngle16 = 0x8000;
      iVar1 = -x;
      reducedAngleNumerator = -y;
      if (0 < (int)reducedAngleNumerator) {
        octantBaseAngle16 = -0x8000;
      }
    }
    else {
      iVar1 = y - x;
      octantBaseAngle16 = 0x6000;
      reducedAngleNumerator = -(x + y);
    }
  }
  else if (y < iVar1) {
    iVar1 = x + y;
    octantBaseAngle16 = 0x2000;
    reducedAngleNumerator = y - x;
  }
  else {
    octantBaseAngle16 = 0x4000;
    reducedAngleNumerator = -x;
    iVar1 = y;
  }
  angle16Result = 0;
  if (iVar1 * 2 != 0) {
    iVar1 = (int)((longlong)((ulonglong)reducedAngleNumerator << 0x20) / (longlong)(iVar1 * 2));
    iVar2 = (int)((ulonglong)((longlong)iVar1 * (longlong)iVar1) >> 0x20);
    angle16Result =
         octantBaseAngle16 +
         (int)((ulonglong)
               ((longlong)iVar1 *
               (longlong)
               ((int)((ulonglong)
                      ((longlong)iVar2 *
                      (longlong)((int)((ulonglong)((longlong)iVar2 * 0x104c2) >> 0x20) + -0x6ca6))
                     >> 0x20) + 0x517d)) >> 0x20);
  }
  return angle16Result;
}


/* Address: 0x004846A0.
   Ownership: core/math/fixed.
   Purpose: Computes the recovered Q12 fixed-point square-root approximation.
*/
dword __thandor_eax_preserve_ecx_edx FixedMath_SqrtQ12Approx(uint inputValue)

{
  int iVar1;
  uint uVar2;
  
  iVar1 = 0x1f;
  if (inputValue != 0) {
    for (; inputValue >> iVar1 == 0; iVar1 = iVar1 + -1) {
    }
  }
  if (inputValue != 0) {
    uVar2 = 0x1cU - iVar1 & 0x1e;
    iVar1 = inputValue << (sbyte)uVar2;
    return (int)((ulonglong)
                 ((longlong)iVar1 *
                 (longlong)
                 ((int)((ulonglong)
                        ((longlong)iVar1 *
                        (longlong)
                        ((int)((ulonglong)((longlong)iVar1 * 0x25ed098) >> 0x20) + -0x1c71c71)) >>
                       0x20) + 0xb1c71c)) >> 0x20) + 0x66b75U >> (sbyte)(uVar2 >> 1);
  }
  return 0;
}

/* Address: 0x00484700.
   Ownership: core/math/fixed.
   Purpose: Returns floor(sqrt((high << 32) | low)) using three Newton iterations. 64-bit integer square root
   (vector length for the angle extraction path). Typed parameters: p0 high→UInt64Half32_V342, p1
   low→UInt64Half32_V342. Calling convention, exact VariableStorage serialization, function body bytes, control
   flow, globals, locals, and executable data remain unchanged.
*/
dword __thandor_eax_preserve_ecx_edx FixedMath_UInt64Sqrt(UInt64Half32 high,UInt64Half32 low)

{
  byte initialRootShift;
  uint rootEstimate;
  uint refinedRootEstimate;
  uint secondRootEstimate;
  int lowHighestSetBitIndex;
  int highestSetBitIndex;
  
  highestSetBitIndex = 0x1f;
  if (high != 0) {
    for (; high >> highestSetBitIndex == 0; highestSetBitIndex = highestSetBitIndex + -1) {
    }
  }
  if (high == 0) {
    lowHighestSetBitIndex = 0x1f;
    if (low != 0) {
      for (; low >> lowHighestSetBitIndex == 0; lowHighestSetBitIndex = lowHighestSetBitIndex + -1)
      {
      }
    }
    if (low == 0) {
      return 0;
    }
    initialRootShift = (byte)(lowHighestSetBitIndex + 1U >> 1);
  }
  else {
    initialRootShift = (byte)(highestSetBitIndex + 0x21U >> 1);
  }
  rootEstimate = 1 << (initialRootShift & 0x1f);
  refinedRootEstimate = rootEstimate + (int)(CONCAT44(high,low) / (ulonglong)rootEstimate) >> 1;
  secondRootEstimate =
       refinedRootEstimate + (int)(CONCAT44(high,low) / (ulonglong)refinedRootEstimate) >> 1;
  return (int)(CONCAT44(high,low) / (ulonglong)secondRootEstimate) + secondRootEstimate >> 1;
}


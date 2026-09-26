/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/src/core/math/fixed.c
 * Reverse engineering by idkFoxes 2026
 */

#include <math.h>
#include <thandor/core/math/fixed.h>
#include <thandor/thandor.h>

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
  FixedEulerAnglesEaxEcxEdx12 extractedAngles;
  FixedEulerAnglesEaxEbxEdx12 composedAngles;
  
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
  extractedAngles = FixedTransform_ExtractEulerAnglesRegs
                    ((GraphicsFixedMatrix3x4 *)&g_FixedTransformComposedRotationScratch);
  composedAngles.angle2 = extractedAngles.eaxAngle;
  composedAngles.angle0 = (int)THANDOR_PART(qword, extractedAngles, 4);
  composedAngles.angle1 = (int)((ulonglong)THANDOR_PART(qword, extractedAngles, 4) >> 0x20);
  return composedAngles;
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
  ulonglong lengthAzimuthPair;
  dword elevationAngle16;
  dword elevationAngleResult;
  dword azimuthAngle16;
  dword vectorLengthQ12;
  FixedLengthAnglesEaxEcxEdx12 lengthAnglesResult;
  longlong squaredLengthAccumulatorQ24;
  longlong totalSquaredLengthQ24;
  
  squaredLengthAccumulatorQ24 = (longlong)y * (longlong)y + (longlong)z * (longlong)z;
  elevationAngle16 =
       FixedMath_UInt64Sqrt
                 ((UInt64Half32)((ulonglong)squaredLengthAccumulatorQ24 >> 0x20),
                  (UInt64Half32)squaredLengthAccumulatorQ24);
  elevationAngleResult = FixedMath_Atan2Angle16(x,elevationAngle16);
  azimuthAngle16 = FixedMath_Atan2Angle16(y,z);
  totalSquaredLengthQ24 = squaredLengthAccumulatorQ24 + (longlong)x * (longlong)x;
  vectorLengthQ12 =
       FixedMath_UInt64Sqrt
                 ((UInt64Half32)((ulonglong)totalSquaredLengthQ24 >> 0x20),
                  (UInt64Half32)totalSquaredLengthQ24);
  lengthAzimuthPair = CONCAT44(azimuthAngle16,vectorLengthQ12) & 0xffffffffffff;
  lengthAnglesResult.elevationAngle = elevationAngleResult;
  lengthAnglesResult.lengthQ12 = (int)lengthAzimuthPair;
  lengthAnglesResult.azimuthAngle = (int)(lengthAzimuthPair >> 0x20);
  return lengthAnglesResult;
}


/* Address: 0x00484A10.
   Ownership: core/math/fixed.
   Purpose: Pointer form of FixedMath_VectorToAnglesAndLength3Regs. EAX=length, EDX=elevation angle, ECX=azimuth
   angle.
   Local calls: FixedMath_UInt64Sqrt, FixedMath_Atan2Angle16.
*/
FixedLengthAnglesEaxEcxEdx12 FixedMath_VectorToAnglesAndLengthVec3Regs(GraphicsFixedVec3 *vector)

{
  ulonglong lengthAzimuthPair;
  dword elevationAngle16;
  dword elevationAngleResult;
  dword azimuthAngle16;
  dword vectorLengthQ12;
  FixedLengthAnglesEaxEcxEdx12 lengthAnglesResult;
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
  elevationAngleResult = FixedMath_Atan2Angle16(vector->z,elevationAngle16);
  azimuthAngle16 = FixedMath_Atan2Angle16(y,x);
  totalSquaredLengthQ24 = squaredLengthAccumulatorQ24 + (longlong)inputZQ12 * (longlong)inputZQ12;
  vectorLengthQ12 =
       FixedMath_UInt64Sqrt
                 ((UInt64Half32)((ulonglong)totalSquaredLengthQ24 >> 0x20),
                  (UInt64Half32)totalSquaredLengthQ24);
  lengthAzimuthPair = CONCAT44(azimuthAngle16,vectorLengthQ12) & 0xffffffffffff;
  lengthAnglesResult.elevationAngle = elevationAngleResult;
  lengthAnglesResult.lengthQ12 = (int)lengthAzimuthPair;
  lengthAnglesResult.azimuthAngle = (int)(lengthAzimuthPair >> 0x20);
  return lengthAnglesResult;
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
  dword vectorAngle16;
  dword vectorLengthQ12;
  
  vectorAngle16 = FixedMath_Atan2Angle16(component0,component1);
  vectorLengthQ12 = FixedMath_Length2(component0,component1);
  return THANDOR_BITCAST(__int64, FixedLengthAngleEaxEdx8, (CONCAT44(vectorAngle16,vectorLengthQ12) & 0xffffffffffff));
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
  FixedVectorEaxEcxEdx12 rotatedVector;
  
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
  rotatedVector.yQ12 = g_ModelTransformOutputY;
  rotatedVector.xQ12 = g_ModelTransformOutputX;
  rotatedVector.zQ12 = g_ModelTransformOutputZ;
  return rotatedVector;
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
  FixedDirectionXyzRegs12 stepDirection;
  FixedMathVectorAnglesRegs8 vectorAngles;
  
  vectorAngles = FixedMath_VectorToAnglesVec3Regs((GraphicsFixedVec3 *)(vectorState + 0x18));
  stepDirection = FixedMath_DirectionFromAnglesScaledRegs(vectorAngles.ecx,vectorAngles.edx,directionScale);
  *(int *)(vectorState + 0x18) = *(int *)(vectorState + 0x18) - stepDirection.eax * stepMultiplier;
  *(int *)(vectorState + 0x1c) = *(int *)(vectorState + 0x1c) - stepDirection.ecx * stepMultiplier;
  *(int *)(vectorState + 0x20) = *(int *)(vectorState + 0x20) - stepDirection.edx * stepMultiplier;
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
  longlong projectionOrHeightSquared;
  longlong side2Squared;
  longlong side1Squared;
  longlong side0Squared;
  uint negatedSquaredLow;
  int projectionOrSquaredLow;
  uint side1SquaredLow;
  dword triangleHeight;
  dword firstAngle16;
  AngleTurn32 fallbackAngle0;
  AngleTurn32 fallbackAngle1;
  uint partialDifferenceLow;
  FixedTriangleJointAnglesEaxEdx8 solvedAngles;
  FixedTriangleJointAnglesEaxEdx8 fallbackAngles;
  longlong cosineNumerator0;
  
  projectionOrSquaredLow = (int)(((longlong)sideLength0Q12 * (longlong)sideLength0Q12 -
                (longlong)sideLength1Q12 * (longlong)sideLength1Q12) / (longlong)sideLength2Q12);
  projectionOrHeightSquared = (longlong)projectionOrSquaredLow * (longlong)projectionOrSquaredLow;
  projectionOrSquaredLow = (int)projectionOrHeightSquared;
  negatedSquaredLow = -projectionOrSquaredLow;
  side2Squared = (longlong)sideLength2Q12 * (longlong)sideLength2Q12;
  partialDifferenceLow = negatedSquaredLow - (uint)side2Squared;
  side1Squared = (longlong)sideLength1Q12 * (longlong)sideLength1Q12;
  side1SquaredLow = (uint)side1Squared;
  side0Squared = (longlong)sideLength0Q12 * (longlong)sideLength0Q12;
  cosineNumerator0 = (side2Squared - side1Squared) + side0Squared;
  projectionOrHeightSquared = side0Squared * 2 +
          CONCAT44((((-(uint)(projectionOrSquaredLow != 0) - (int)((ulonglong)projectionOrHeightSquared >> 0x20)) -
                    (int)((ulonglong)side2Squared >> 0x20)) - (uint)(negatedSquaredLow < (uint)side2Squared)) +
                   (int)((ulonglong)side1Squared >> 0x20) * 2 + (uint)CARRY4(side1SquaredLow,side1SquaredLow) +
                   (uint)CARRY4(partialDifferenceLow,side1SquaredLow * 2),partialDifferenceLow + side1SquaredLow * 2);
  if ((-1 < projectionOrHeightSquared) && (0x10 < sideLength2Q12)) {
    triangleHeight = FixedMath_UInt64Sqrt((UInt64Half32)((ulonglong)projectionOrHeightSquared >> 0x20),(UInt64Half32)projectionOrHeightSquared);
    firstAngle16 = FixedMath_Atan2Angle16((int)triangleHeight >> 1,(int)(cosineNumerator0 / (longlong)sideLength2Q12) >> 1);
    solvedAngles.jointAngle0 =
         FixedMath_Atan2Angle16
                   ((int)triangleHeight >> 1,(int)(((side1Squared + side2Squared) - side0Squared) / (longlong)sideLength2Q12) >> 1
                   );
    solvedAngles.jointAngle1 = firstAngle16 + solvedAngles.jointAngle0;
    return solvedAngles;
  }
  if ((uint)sideLength0Q12 < (uint)sideLength2Q12) {
    fallbackAngle0 = 0;
    fallbackAngle1 = 0;
  }
  else {
    fallbackAngle0 = 0x8000;
    fallbackAngle1 = 0x8000;
  }
  fallbackAngles.jointAngle1 = fallbackAngle1;
  fallbackAngles.jointAngle0 = fallbackAngle0;
  return fallbackAngles;
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
  FixedMathVectorAnglesRegs8 forwardAngles;
  
  forwardAngles = FixedMath_VectorToAngles3Regs
                    (transform->basisRow2[2],transform->basisRow1[2],transform->basisRow0[2]);
  return forwardAngles;
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
  FixedVectorEaxEcxEdx12 rotatedVector;
  FixedVectorXEaxYEbxZEdx12 coreRotatedVector;
  
  coreRotatedVector = FixedTransform_RotateDirectionScaledCoreRegs
                    (directionScale,elevationAngle,azimuthAngle,rotationAngle0,rotationAngle1,
                     rotationAngle2);
  /* Ghidra split the ECX/EDX halves of the return into uVar3._4_4_ and register0x00000008. */
  rotatedVector.xQ12 = coreRotatedVector.xQ12;
  rotatedVector.yQ12 = coreRotatedVector.yQ12;
  rotatedVector.zQ12 = coreRotatedVector.zQ12;
  return rotatedVector;
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
  uint secondAngleStep16;
  int entriesRemaining;
  uint angleIndex16;
  uint secondAngleIndex16;
  ArenaAllocEaxCf5 allocResult;
  
  allocResult = (*g_MemoryApi.alloc)(0x40000);
  outputCursor = (short *)allocResult.eax;
  if (!allocResult.carry) {
    g_CosineDerivedLookupAllocation = outputCursor;
    for (entriesRemainingInRow = 0x80; entriesRemainingInRow != 0;
        entriesRemainingInRow = entriesRemainingInRow + -1) {
      outputCursor[0] = 0xb50;
      outputCursor[1] = 0xb50;
      outputCursor = outputCursor + 2;
    }
    angleIndex16 = 0x40;
    angleStep16 = 0x40;
    entriesRemaining = 0x100;
    do {
      do {
        *outputCursor = (short)((uint)g_FixedCosQ28[angleIndex16] >> 0x10);
        outputCursor = outputCursor + 1;
        angleIndex16 = angleIndex16 + angleStep16 * 2 & 0xffff;
        entriesRemaining = entriesRemaining + -1;
      } while (entriesRemaining != 0);
      angleStep16 = angleStep16 + 0x40;
      entriesRemaining = 0x100;
      angleIndex16 = angleStep16 & 0xffff;
    } while (angleStep16 < 0x4000);
    secondAngleIndex16 = 0;
    entriesRemaining = 0x100;
    secondAngleStep16 = 0x40;
    g_CosineDerivedLookupSecondTable = outputCursor;
    do {
      do {
        if (entriesRemaining == 0x100) {
          *outputCursor = 0x2d41;
        }
        else {
          *outputCursor = (short)(g_FixedCosQ28[secondAngleIndex16] >> 0xe);
        }
        outputCursor = outputCursor + 1;
        secondAngleIndex16 = secondAngleIndex16 + secondAngleStep16 & 0xffff;
        entriesRemaining = entriesRemaining + -1;
      } while (entriesRemaining != 0);
      secondAngleStep16 = secondAngleStep16 + 0x80;
      entriesRemaining = 0x100;
      secondAngleIndex16 = 0;
    } while (secondAngleStep16 < 0x8000);
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
  uint azimuthMinusElevationIndex16;
  sdword verticalSinQ28;
  int azimuthPlusElevationSinQ28;
  int azimuthMinusElevationSinQ28;
  
  azimuthMinusElevationAngle16 = elevationAngle & 0xffff;
  verticalSinQ28 = g_FixedSinQ28[azimuthMinusElevationAngle16];
  azimuthPlusElevationAngle16 = azimuthMinusElevationAngle16 + azimuthAngle & 0xffff;
  azimuthMinusElevationIndex16 = azimuthAngle - azimuthMinusElevationAngle16 & 0xffff;
  azimuthPlusElevationSinQ28 = g_FixedSinQ28[azimuthPlusElevationAngle16];
  azimuthMinusElevationSinQ28 = g_FixedSinQ28[azimuthMinusElevationIndex16];
  output->x = g_FixedCosQ28[azimuthPlusElevationAngle16] + g_FixedCosQ28[azimuthMinusElevationIndex16] >> 1;
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
  int basisRow2Component0Q28;
  longlong finalBasisDotProductQ40;
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
  basisRow2Component0Q28 = transform->basisRow2[0];
  output->y = (int)((ulonglong)currentBasisDotProductQ40 >> 0x20) << 4 |
              (uint)currentBasisDotProductQ40 >> 0x1c;
  finalBasisDotProductQ40 = (longlong)transform->basisRow2[1] * (longlong)direction->y +
          (longlong)basisRow2Component0Q28 * (longlong)direction->x +
          (longlong)transform->basisRow2[2] * (longlong)direction->z;
  output->z = (int)((ulonglong)finalBasisDotProductQ40 >> 0x20) << 4 | (uint)finalBasisDotProductQ40 >> 0x1c;
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
  int basisRow0Component2Q28;
  longlong finalBasisDotProductQ40;
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
  basisRow0Component2Q28 = transform->basisRow0[2];
  output->y = (int)((ulonglong)currentBasisDotProductQ40 >> 0x20) << 4 |
              (uint)currentBasisDotProductQ40 >> 0x1c;
  finalBasisDotProductQ40 = (longlong)transform->basisRow1[2] * (longlong)direction->y +
          (longlong)basisRow0Component2Q28 * (longlong)direction->x +
          (longlong)transform->basisRow2[2] * (longlong)direction->z;
  output->z = (int)((ulonglong)finalBasisDotProductQ40 >> 0x20) << 4 | (uint)finalBasisDotProductQ40 >> 0x1c;
  return;
}


/* Address: 0x00485520.
   Ownership: core/math/fixed.
   Purpose: It assumes the basis is a rotation matrix.
*/
void __thandor_void_preserve_eax_ecx_edx
FixedTransform_InvertRigidQ28(GraphicsFixedMatrix3x4 *output,GraphicsFixedMatrix3x4 *input)

{
  longlong cofactorOrTranslationProduct;
  longlong translationYProduct;
  longlong translationZProduct;
  uint negatedProductXLow;
  int productXLow;
  uint productYLow;
  uint productZLow;
  int componentOrProductLow;
  uint partialDifferenceLow;
  
  cofactorOrTranslationProduct = (longlong)input->basisRow1[1] * (longlong)input->basisRow2[2] -
          (longlong)input->basisRow1[2] * (longlong)input->basisRow2[1];
  componentOrProductLow = input->basisRow1[2];
  output->basisRow0[0] = (int)((ulonglong)cofactorOrTranslationProduct >> 0x20) << 4 | (uint)cofactorOrTranslationProduct >> 0x1c;
  cofactorOrTranslationProduct = (longlong)componentOrProductLow * (longlong)input->basisRow2[0] -
          (longlong)input->basisRow1[0] * (longlong)input->basisRow2[2];
  componentOrProductLow = input->basisRow1[0];
  output->basisRow1[0] = (int)((ulonglong)cofactorOrTranslationProduct >> 0x20) << 4 | (uint)cofactorOrTranslationProduct >> 0x1c;
  cofactorOrTranslationProduct = (longlong)componentOrProductLow * (longlong)input->basisRow2[1] -
          (longlong)input->basisRow1[1] * (longlong)input->basisRow2[0];
  componentOrProductLow = input->basisRow0[2];
  output->basisRow2[0] = (int)((ulonglong)cofactorOrTranslationProduct >> 0x20) << 4 | (uint)cofactorOrTranslationProduct >> 0x1c;
  cofactorOrTranslationProduct = (longlong)componentOrProductLow * (longlong)input->basisRow2[1] -
          (longlong)input->basisRow0[1] * (longlong)input->basisRow2[2];
  componentOrProductLow = input->basisRow0[0];
  output->basisRow0[1] = (int)((ulonglong)cofactorOrTranslationProduct >> 0x20) << 4 | (uint)cofactorOrTranslationProduct >> 0x1c;
  cofactorOrTranslationProduct = (longlong)componentOrProductLow * (longlong)input->basisRow2[2] -
          (longlong)input->basisRow0[2] * (longlong)input->basisRow2[0];
  componentOrProductLow = input->basisRow0[1];
  output->basisRow1[1] = (int)((ulonglong)cofactorOrTranslationProduct >> 0x20) << 4 | (uint)cofactorOrTranslationProduct >> 0x1c;
  cofactorOrTranslationProduct = (longlong)componentOrProductLow * (longlong)input->basisRow2[0] -
          (longlong)input->basisRow0[0] * (longlong)input->basisRow2[1];
  componentOrProductLow = input->basisRow0[1];
  output->basisRow2[1] = (int)((ulonglong)cofactorOrTranslationProduct >> 0x20) << 4 | (uint)cofactorOrTranslationProduct >> 0x1c;
  cofactorOrTranslationProduct = (longlong)componentOrProductLow * (longlong)input->basisRow1[2] -
          (longlong)input->basisRow0[2] * (longlong)input->basisRow1[1];
  componentOrProductLow = input->basisRow0[2];
  output->basisRow0[2] = (int)((ulonglong)cofactorOrTranslationProduct >> 0x20) << 4 | (uint)cofactorOrTranslationProduct >> 0x1c;
  cofactorOrTranslationProduct = (longlong)componentOrProductLow * (longlong)input->basisRow1[0] -
          (longlong)input->basisRow0[0] * (longlong)input->basisRow1[2];
  componentOrProductLow = input->basisRow0[0];
  output->basisRow1[2] = (int)((ulonglong)cofactorOrTranslationProduct >> 0x20) << 4 | (uint)cofactorOrTranslationProduct >> 0x1c;
  cofactorOrTranslationProduct = (longlong)componentOrProductLow * (longlong)input->basisRow1[1] -
          (longlong)input->basisRow0[1] * (longlong)input->basisRow1[0];
  output->basisRow2[2] = (int)((ulonglong)cofactorOrTranslationProduct >> 0x20) << 4 | (uint)cofactorOrTranslationProduct >> 0x1c;
  cofactorOrTranslationProduct = (longlong)(input->translation).x * (longlong)output->basisRow0[0];
  productXLow = (int)cofactorOrTranslationProduct;
  negatedProductXLow = -productXLow;
  translationYProduct = (longlong)(input->translation).y * (longlong)output->basisRow0[1];
  productYLow = (uint)translationYProduct;
  partialDifferenceLow = negatedProductXLow - productYLow;
  translationZProduct = (longlong)(input->translation).z * (longlong)output->basisRow0[2];
  productZLow = (uint)translationZProduct;
  componentOrProductLow = (input->translation).x;
  (output->translation).x =
       (((((-(uint)(productXLow != 0) - (int)((ulonglong)cofactorOrTranslationProduct >> 0x20)) - (int)((ulonglong)translationYProduct >> 0x20)
          ) - (uint)(negatedProductXLow < productYLow)) - (int)((ulonglong)translationZProduct >> 0x20)) - (uint)(partialDifferenceLow < productZLow)) *
       0x10 | partialDifferenceLow - productZLow >> 0x1c;
  cofactorOrTranslationProduct = (longlong)componentOrProductLow * (longlong)output->basisRow1[0];
  productXLow = (int)cofactorOrTranslationProduct;
  negatedProductXLow = -productXLow;
  translationYProduct = (longlong)(input->translation).y * (longlong)output->basisRow1[1];
  productYLow = (uint)translationYProduct;
  partialDifferenceLow = negatedProductXLow - productYLow;
  translationZProduct = (longlong)(input->translation).z * (longlong)output->basisRow1[2];
  productZLow = (uint)translationZProduct;
  componentOrProductLow = (input->translation).x;
  (output->translation).y =
       (((((-(uint)(productXLow != 0) - (int)((ulonglong)cofactorOrTranslationProduct >> 0x20)) - (int)((ulonglong)translationYProduct >> 0x20)
          ) - (uint)(negatedProductXLow < productYLow)) - (int)((ulonglong)translationZProduct >> 0x20)) - (uint)(partialDifferenceLow < productZLow)) *
       0x10 | partialDifferenceLow - productZLow >> 0x1c;
  cofactorOrTranslationProduct = (longlong)componentOrProductLow * (longlong)output->basisRow2[0];
  componentOrProductLow = (int)cofactorOrTranslationProduct;
  negatedProductXLow = -componentOrProductLow;
  translationYProduct = (longlong)(input->translation).y * (longlong)output->basisRow2[1];
  productYLow = (uint)translationYProduct;
  partialDifferenceLow = negatedProductXLow - productYLow;
  translationZProduct = (longlong)(input->translation).z * (longlong)output->basisRow2[2];
  productZLow = (uint)translationZProduct;
  (output->translation).z =
       (((((-(uint)(componentOrProductLow != 0) - (int)((ulonglong)cofactorOrTranslationProduct >> 0x20)) - (int)((ulonglong)translationYProduct >> 0x20)
          ) - (uint)(negatedProductXLow < productYLow)) - (int)((ulonglong)translationZProduct >> 0x20)) - (uint)(partialDifferenceLow < productZLow)) *
       0x10 | partialDifferenceLow - productZLow >> 0x1c;
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
  int leftXQ12;
  longlong finalCrossProductDifferenceQ24;
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
  leftXQ12 = leftOperand->x;
  output->y = (int)((ulonglong)currentCrossProductDifferenceQ24 >> 0x20) << 0x14 |
              (uint)currentCrossProductDifferenceQ24 >> 0xc;
  finalCrossProductDifferenceQ24 = (longlong)leftXQ12 * (longlong)rightOperand->y -
          (longlong)leftOperand->y * (longlong)rightOperand->x;
  output->z = (int)((ulonglong)finalCrossProductDifferenceQ24 >> 0x20) << 0x14 | (uint)finalCrossProductDifferenceQ24 >> 0xc;
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
  FixedVectorXEaxYEbxZEdx12 rotatedVector;
  
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
  rotatedVector.yQ12 = g_ModelTransformOutputY;
  rotatedVector.xQ12 = g_ModelTransformOutputX;
  rotatedVector.zQ12 = g_ModelTransformOutputZ;
  return rotatedVector;
}


/* Address: 0x00484A70.
   Ownership: core/math/fixed.
   Purpose: Pointer form of FixedMath_VectorToAngles3Regs. EDX=elevation angle and ECX=azimuth angle.
   Local calls: FixedMath_UInt64Sqrt, FixedMath_Atan2Angle16.
*/
FixedMathVectorAnglesRegs8 __thandor_preserve_eax
FixedMath_VectorToAnglesVec3Regs(GraphicsFixedVec3 *vector)

{
  dword magnitudeOrElevationAngle;
  dword azimuthAngle16;
  int y;
  int x;
  longlong horizontalSquaredLengthAccumulatorQ24;
  
  x = vector->x;
  y = vector->y;
  horizontalSquaredLengthAccumulatorQ24 = (longlong)y * (longlong)y + (longlong)x * (longlong)x;
  magnitudeOrElevationAngle = FixedMath_UInt64Sqrt
                    ((UInt64Half32)((ulonglong)horizontalSquaredLengthAccumulatorQ24 >> 0x20),
                     (UInt64Half32)horizontalSquaredLengthAccumulatorQ24);
  magnitudeOrElevationAngle = FixedMath_Atan2Angle16(vector->z,magnitudeOrElevationAngle);
  azimuthAngle16 = FixedMath_Atan2Angle16(y,x);
  return THANDOR_BITCAST(__int64, FixedMathVectorAnglesRegs8, (CONCAT44(azimuthAngle16,magnitudeOrElevationAngle) & 0xffffffffffff));
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
  dword gimbalAngle16;
  FixedMathVectorAnglesRegs8 forwardAngles;
  FixedEulerAnglesEaxEcxEdx12 eulerAngles;
  
  forwardAngles = FixedMath_VectorToAngles3Regs
                    (transform->basisRow2[2],transform->basisRow1[2],transform->basisRow0[2]);
  eulerAngles.edxAngle = forwardAngles.edx;
  eulerAngles.ecxAngle = forwardAngles.ecx;
  if (THANDOR_BITCAST(FixedMathVectorAnglesRegs8, longlong, forwardAngles) < 0) {
    gimbalAngle16 = FixedMath_Atan2Angle16
                      (transform->basisRow1[0] + transform->basisRow0[1],
                       transform->basisRow1[1] - transform->basisRow0[0]);
    extractedRotationAngle2 = gimbalAngle16 + eulerAngles.ecxAngle * 2 & 0xffff;
  }
  else {
    extractedRotationAngle2 =
         FixedMath_Atan2Angle16
                   (transform->basisRow1[0] - transform->basisRow0[1],
                    transform->basisRow1[1] + transform->basisRow0[0]);
  }
  eulerAngles.eaxAngle = extractedRotationAngle2;
  return eulerAngles;
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
  longlong scaledYComponentProduct;
  uint sumAngle16;
  uint elevationAngle16;
  uint differenceAngle16;
  FixedDirectionXyzRegs12 scaledDirection;
  longlong scaledHorizontalComponentProduct;
  
  elevationAngle16 = elevationAngle & 0xffff;
  sumAngle16 = elevationAngle16 + azimuthAngle & 0xffff;
  differenceAngle16 = azimuthAngle - elevationAngle16 & 0xffff;
  scaledHorizontalComponentProduct =
       (longlong)(g_FixedCosQ28[sumAngle16] + g_FixedCosQ28[differenceAngle16]) * (longlong)scale;
  scaledYComponentProduct = (longlong)(g_FixedSinQ28[sumAngle16] + g_FixedSinQ28[differenceAngle16]) * (longlong)scale;
  scaledDirection.edx = (int)((ulonglong)((longlong)g_FixedSinQ28[elevationAngle16] * (longlong)scale) >> 0x20
                   ) << 4 |
              (uint)((longlong)g_FixedSinQ28[elevationAngle16] * (longlong)scale) >> 0x1c;
  scaledDirection.ecx = (int)((ulonglong)scaledYComponentProduct >> 0x20) << 3 | (uint)scaledYComponentProduct >> 0x1d;
  scaledDirection.eax = (int)((ulonglong)scaledHorizontalComponentProduct >> 0x20) << 3 |
              (uint)scaledHorizontalComponentProduct >> 0x1d;
  return scaledDirection;
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
  uint sumAngle16;
  uint elevationAngle16;
  uint differenceAngle16;
  FixedDirectionXyzRegs12 directionQ28;
  
  elevationAngle16 = elevationAngle & 0xffff;
  sumAngle16 = elevationAngle16 + azimuthAngle & 0xffff;
  differenceAngle16 = azimuthAngle - elevationAngle16 & 0xffff;
  directionQ28.eax = g_FixedCosQ28[sumAngle16] + g_FixedCosQ28[differenceAngle16] >> 1;
  directionQ28.ecx = g_FixedSinQ28[sumAngle16] + g_FixedSinQ28[differenceAngle16] >> 1;
  directionQ28.edx = g_FixedSinQ28[elevationAngle16];
  return directionQ28;
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
  uint elevationOrDifferenceAngle16;
  int verticalSinQ28;
  int azimuthPlusElevationSinQ28;
  int azimuthMinusElevationSinQ28;
  longlong yComponentScaleProduct;
  longlong horizontalComponentScaleProduct;
  
  elevationOrDifferenceAngle16 = elevationAngle & 0xffff;
  verticalSinQ28 = g_FixedSinQ28[elevationOrDifferenceAngle16];
  azimuthPlusElevationAngle16 = elevationOrDifferenceAngle16 + azimuthAngle & 0xffff;
  elevationOrDifferenceAngle16 = azimuthAngle - elevationOrDifferenceAngle16 & 0xffff;
  azimuthPlusElevationSinQ28 = g_FixedSinQ28[azimuthPlusElevationAngle16];
  azimuthMinusElevationSinQ28 = g_FixedSinQ28[elevationOrDifferenceAngle16];
  horizontalComponentScaleProduct =
       (longlong)(g_FixedCosQ28[azimuthPlusElevationAngle16] + g_FixedCosQ28[elevationOrDifferenceAngle16]) *
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
  int rightBasisComponent0Q28;
  longlong composedProductSumQ56;
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
  composedProductSumQ56 = (longlong)transformB->basisRow0[1] * (longlong)transformA->basisRow1[1] +
          (longlong)currentRightBasisRowComponent0Q28 * (longlong)transformA->basisRow0[1] +
          (longlong)transformB->basisRow0[2] * (longlong)transformA->basisRow2[1];
  rightBasisComponent0Q28 = transformB->basisRow0[0];
  output->basisRow0[1] = (int)((ulonglong)composedProductSumQ56 >> 0x20) << 4 | (uint)composedProductSumQ56 >> 0x1c;
  composedProductSumQ56 = (longlong)transformB->basisRow0[1] * (longlong)transformA->basisRow1[2] +
          (longlong)rightBasisComponent0Q28 * (longlong)transformA->basisRow0[2] +
          (longlong)transformB->basisRow0[2] * (longlong)transformA->basisRow2[2];
  rightBasisComponent0Q28 = transformB->basisRow0[0];
  output->basisRow0[2] = (int)((ulonglong)composedProductSumQ56 >> 0x20) << 4 | (uint)composedProductSumQ56 >> 0x1c;
  composedProductSumQ56 = (longlong)transformB->basisRow0[1] * (longlong)(transformA->translation).y +
          (longlong)rightBasisComponent0Q28 * (longlong)(transformA->translation).x +
          (longlong)transformB->basisRow0[2] * (longlong)(transformA->translation).z;
  rightBasisComponent0Q28 = transformB->basisRow1[0];
  (output->translation).x =
       ((int)((ulonglong)composedProductSumQ56 >> 0x20) << 4 | (uint)composedProductSumQ56 >> 0x1c) + (transformB->translation).x;
  composedProductSumQ56 = (longlong)transformB->basisRow1[1] * (longlong)transformA->basisRow1[0] +
          (longlong)rightBasisComponent0Q28 * (longlong)transformA->basisRow0[0] +
          (longlong)transformB->basisRow1[2] * (longlong)transformA->basisRow2[0];
  rightBasisComponent0Q28 = transformB->basisRow1[0];
  output->basisRow1[0] = (int)((ulonglong)composedProductSumQ56 >> 0x20) << 4 | (uint)composedProductSumQ56 >> 0x1c;
  composedProductSumQ56 = (longlong)transformB->basisRow1[1] * (longlong)transformA->basisRow1[1] +
          (longlong)rightBasisComponent0Q28 * (longlong)transformA->basisRow0[1] +
          (longlong)transformB->basisRow1[2] * (longlong)transformA->basisRow2[1];
  rightBasisComponent0Q28 = transformB->basisRow1[0];
  output->basisRow1[1] = (int)((ulonglong)composedProductSumQ56 >> 0x20) << 4 | (uint)composedProductSumQ56 >> 0x1c;
  composedProductSumQ56 = (longlong)transformB->basisRow1[1] * (longlong)transformA->basisRow1[2] +
          (longlong)rightBasisComponent0Q28 * (longlong)transformA->basisRow0[2] +
          (longlong)transformB->basisRow1[2] * (longlong)transformA->basisRow2[2];
  rightBasisComponent0Q28 = transformB->basisRow1[0];
  output->basisRow1[2] = (int)((ulonglong)composedProductSumQ56 >> 0x20) << 4 | (uint)composedProductSumQ56 >> 0x1c;
  composedProductSumQ56 = (longlong)transformB->basisRow1[1] * (longlong)(transformA->translation).y +
          (longlong)rightBasisComponent0Q28 * (longlong)(transformA->translation).x +
          (longlong)transformB->basisRow1[2] * (longlong)(transformA->translation).z;
  rightBasisComponent0Q28 = transformB->basisRow2[0];
  (output->translation).y =
       ((int)((ulonglong)composedProductSumQ56 >> 0x20) << 4 | (uint)composedProductSumQ56 >> 0x1c) + (transformB->translation).y;
  composedProductSumQ56 = (longlong)transformB->basisRow2[1] * (longlong)transformA->basisRow1[0] +
          (longlong)rightBasisComponent0Q28 * (longlong)transformA->basisRow0[0] +
          (longlong)transformB->basisRow2[2] * (longlong)transformA->basisRow2[0];
  rightBasisComponent0Q28 = transformB->basisRow2[0];
  output->basisRow2[0] = (int)((ulonglong)composedProductSumQ56 >> 0x20) << 4 | (uint)composedProductSumQ56 >> 0x1c;
  composedProductSumQ56 = (longlong)transformB->basisRow2[1] * (longlong)transformA->basisRow1[1] +
          (longlong)rightBasisComponent0Q28 * (longlong)transformA->basisRow0[1] +
          (longlong)transformB->basisRow2[2] * (longlong)transformA->basisRow2[1];
  rightBasisComponent0Q28 = transformB->basisRow2[0];
  output->basisRow2[1] = (int)((ulonglong)composedProductSumQ56 >> 0x20) << 4 | (uint)composedProductSumQ56 >> 0x1c;
  composedProductSumQ56 = (longlong)transformB->basisRow2[1] * (longlong)transformA->basisRow1[2] +
          (longlong)rightBasisComponent0Q28 * (longlong)transformA->basisRow0[2] +
          (longlong)transformB->basisRow2[2] * (longlong)transformA->basisRow2[2];
  rightBasisComponent0Q28 = transformB->basisRow2[0];
  output->basisRow2[2] = (int)((ulonglong)composedProductSumQ56 >> 0x20) << 4 | (uint)composedProductSumQ56 >> 0x1c;
  composedProductSumQ56 = (longlong)transformB->basisRow2[1] * (longlong)(transformA->translation).y +
          (longlong)rightBasisComponent0Q28 * (longlong)(transformA->translation).x +
          (longlong)transformB->basisRow2[2] * (longlong)(transformA->translation).z;
  (output->translation).z =
       ((int)((ulonglong)composedProductSumQ56 >> 0x20) << 4 | (uint)composedProductSumQ56 >> 0x1c) + (transformB->translation).z;
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
  dword elevationAngle16;
  dword azimuthAngle16;
  longlong horizontalMagnitudeSquaredQ24;
  
  horizontalMagnitudeSquaredQ24 = (longlong)y * (longlong)y + (longlong)z * (longlong)z;
  horizontalMagnitudeQ12 =
       FixedMath_UInt64Sqrt
                 ((UInt64Half32)((ulonglong)horizontalMagnitudeSquaredQ24 >> 0x20),
                  (UInt64Half32)horizontalMagnitudeSquaredQ24);
  elevationAngle16 = FixedMath_Atan2Angle16(x,horizontalMagnitudeQ12);
  azimuthAngle16 = FixedMath_Atan2Angle16(y,z);
  return THANDOR_BITCAST(unsigned __int64, FixedMathVectorAnglesRegs8, (CONCAT44(elevationAngle16,azimuthAngle16) & 0xffffffff0000ffff));
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
  int basisRow2Component0Q28;
  longlong basisDotProductQ40;
  int currentBasisRowComponent0Q28;
  longlong currentBasisDotProductQ40;
  
  currentBasisDotProductQ40 =
       (longlong)transform->basisRow0[1] * (longlong)point->y +
       (longlong)transform->basisRow0[0] * (longlong)point->x +
       (longlong)transform->basisRow0[2] * (longlong)point->z;
  currentBasisRowComponent0Q28 = transform->basisRow1[0];
  output->x = ((int)((ulonglong)currentBasisDotProductQ40 >> 0x20) << 4 |
              (uint)currentBasisDotProductQ40 >> 0x1c) + (transform->translation).x;
  basisDotProductQ40 = (longlong)transform->basisRow1[1] * (longlong)point->y +
          (longlong)currentBasisRowComponent0Q28 * (longlong)point->x +
          (longlong)transform->basisRow1[2] * (longlong)point->z;
  basisRow2Component0Q28 = transform->basisRow2[0];
  output->y = ((int)((ulonglong)basisDotProductQ40 >> 0x20) << 4 | (uint)basisDotProductQ40 >> 0x1c) +
              (transform->translation).y;
  basisDotProductQ40 = (longlong)transform->basisRow2[1] * (longlong)point->y +
          (longlong)basisRow2Component0Q28 * (longlong)point->x +
          (longlong)transform->basisRow2[2] * (longlong)point->z;
  output->z = ((int)((ulonglong)basisDotProductQ40 >> 0x20) << 4 | (uint)basisDotProductQ40 >> 0x1c) +
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
  longlong rowComponentTimesVerticalSinProduct;
  int currentSymmetricComponentQ28;
  int symmetricComponentQ28;
  uint differenceAngleIndex16;
  uint secondarySymmetricAngleIndex16;
  uint primarySymmetricAngleIndex16;
  FixedDirectionXZEdxEax8 directionSamplePairQ28;
  FixedDirectionXZEdxEax8 secondaryDirectionSamplePairQ28;
  FixedDirectionXyzRegs12 directionRow;
  longlong currentComponentTimesVerticalSinProduct;
  int verticalSinQ28;
  longlong componentTimesVerticalSinProduct;
  longlong secondaryComponentTimesVerticalSinProduct;
  
  directionRow = FixedMath_DirectionFromAnglesQ28Regs(angle1,angle2);
  output->basisRow0[2] = directionRow.eax;
  output->basisRow1[2] = directionRow.ecx;
  output->basisRow2[2] = directionRow.edx;
  directionRow = FixedMath_DirectionFromAnglesQ28Regs(angle1,angle0 - angle2);
  secondarySymmetricAngleIndex16 = (angle0 - angle2) + angle2;
  output->basisRow2[1] = directionRow.ecx;
  output->basisRow2[0] = -directionRow.eax;
  primarySymmetricAngleIndex16 = secondarySymmetricAngleIndex16 & 0xffff;
  differenceAngleIndex16 = secondarySymmetricAngleIndex16 + angle2 * -2 & 0xffff;
  currentSymmetricComponentQ28 =
       g_FixedCosQ28[primarySymmetricAngleIndex16] - g_FixedCosQ28[differenceAngleIndex16] >> 1;
  verticalSinQ28 = g_FixedSinQ28[angle1 & 0xffff];
  output->basisRow0[0] = currentSymmetricComponentQ28;
  symmetricComponentQ28 = g_FixedCosQ28[primarySymmetricAngleIndex16] + g_FixedCosQ28[differenceAngleIndex16] >> 1;
  output->basisRow1[1] =
       ((int)((ulonglong)((longlong)currentSymmetricComponentQ28 * (longlong)verticalSinQ28) >> 0x20
             ) << 4 |
       (uint)((longlong)currentSymmetricComponentQ28 * (longlong)verticalSinQ28) >> 0x1c) + symmetricComponentQ28;
  currentComponentTimesVerticalSinProduct = (longlong)symmetricComponentQ28 * (longlong)verticalSinQ28;
  output->basisRow0[0] =
       output->basisRow0[0] +
       ((int)((ulonglong)currentComponentTimesVerticalSinProduct >> 0x20) << 4 |
       (uint)currentComponentTimesVerticalSinProduct >> 0x1c);
  symmetricComponentQ28 = g_FixedSinQ28[primarySymmetricAngleIndex16] + g_FixedSinQ28[differenceAngleIndex16] >> 1;
  output->basisRow1[0] = symmetricComponentQ28;
  rowComponentTimesVerticalSinProduct = (longlong)symmetricComponentQ28 * (longlong)verticalSinQ28;
  symmetricComponentQ28 = g_FixedSinQ28[differenceAngleIndex16] - g_FixedSinQ28[primarySymmetricAngleIndex16] >> 1;
  secondaryComponentTimesVerticalSinProduct = (longlong)symmetricComponentQ28 * (longlong)verticalSinQ28;
  output->basisRow0[1] = symmetricComponentQ28 - ((int)((ulonglong)rowComponentTimesVerticalSinProduct >> 0x20) << 4 | (uint)rowComponentTimesVerticalSinProduct >> 0x1c);
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
  int denominatorOrRatio;
  uint reducedAngleNumerator;
  int doubledYOrRatioSquared;
  AngleTurn16Stored32 octantBaseAngle16;
  
  octantBaseAngle16 = 0;
  denominatorOrRatio = x * 2;
  doubledYOrRatioSquared = y * 2;
  if (doubledYOrRatioSquared - x == 0 || doubledYOrRatioSquared < x) {
    if (denominatorOrRatio < -y) {
      if (y < denominatorOrRatio) {
        octantBaseAngle16 = -0x4000;
        denominatorOrRatio = -y;
        reducedAngleNumerator = x;
      }
      else {
        reducedAngleNumerator = x - y;
        octantBaseAngle16 = -0x6000;
        denominatorOrRatio = -(x + y);
      }
    }
    else {
      denominatorOrRatio = x;
      reducedAngleNumerator = y;
      if (doubledYOrRatioSquared < -x) {
        reducedAngleNumerator = x + y;
        octantBaseAngle16 = -0x2000;
        denominatorOrRatio = x - y;
      }
    }
  }
  else if (denominatorOrRatio < -y) {
    if (doubledYOrRatioSquared < -x) {
      octantBaseAngle16 = 0x8000;
      denominatorOrRatio = -x;
      reducedAngleNumerator = -y;
      if (0 < (int)reducedAngleNumerator) {
        octantBaseAngle16 = -0x8000;
      }
    }
    else {
      denominatorOrRatio = y - x;
      octantBaseAngle16 = 0x6000;
      reducedAngleNumerator = -(x + y);
    }
  }
  else if (y < denominatorOrRatio) {
    denominatorOrRatio = x + y;
    octantBaseAngle16 = 0x2000;
    reducedAngleNumerator = y - x;
  }
  else {
    octantBaseAngle16 = 0x4000;
    reducedAngleNumerator = -x;
    denominatorOrRatio = y;
  }
  angle16Result = 0;
  if (denominatorOrRatio * 2 != 0) {
    denominatorOrRatio = (int)((longlong)((ulonglong)reducedAngleNumerator << 0x20) / (longlong)(denominatorOrRatio * 2));
    doubledYOrRatioSquared = (int)((ulonglong)((longlong)denominatorOrRatio * (longlong)denominatorOrRatio) >> 0x20);
    angle16Result =
         octantBaseAngle16 +
         (int)((ulonglong)
               ((longlong)denominatorOrRatio *
               (longlong)
               ((int)((ulonglong)
                      ((longlong)doubledYOrRatioSquared *
                      (longlong)((int)((ulonglong)((longlong)doubledYOrRatioSquared * 0x104c2) >> 0x20) + -0x6ca6))
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
  int highestBitOrNormalized;
  uint normalizeShift;
  
  highestBitOrNormalized = 0x1f;
  if (inputValue != 0) {
    for (; inputValue >> highestBitOrNormalized == 0; highestBitOrNormalized = highestBitOrNormalized + -1) {
    }
  }
  if (inputValue != 0) {
    normalizeShift = 0x1cU - highestBitOrNormalized & 0x1e;
    highestBitOrNormalized = inputValue << (sbyte)normalizeShift;
    return (int)((ulonglong)
                 ((longlong)highestBitOrNormalized *
                 (longlong)
                 ((int)((ulonglong)
                        ((longlong)highestBitOrNormalized *
                        (longlong)
                        ((int)((ulonglong)((longlong)highestBitOrNormalized * 0x25ed098) >> 0x20) + -0x1c71c71)) >>
                       0x20) + 0xb1c71c)) >> 0x20) + 0x66b75U >> (sbyte)(normalizeShift >> 1);
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


/* Not in the original: the original executable carries these tables precomputed (0x004346A0,
   81920 dwords). g_FixedSinQ28 holds the first quarter turn and g_FixedCosQ28 directly follows
   it, so together they are one sine over 1.25 turns: g_FixedCosQ28[i] = sin(i + quarter turn),
   and sine lookups up to a full turn run on into the cosine table.
   Entry i is sin(i * 2pi / 65536) in Q28, rounded half up, computed with pi = 3.141592654; this
   reproduces every entry of the original. Called once at startup. */
static sdword FixedMath_SineTableEntry(int index)
{
  return (sdword)floor(sin(index * (3.141592654 / 32768.0)) * 268435456.0 + 0.5);
}

void FixedMath_BuildSinCosTables(void)
{
  int index;

  for (index = 0; index < 16384; index++) {
    g_FixedSinQ28[index] = FixedMath_SineTableEntry(index);
  }
  for (index = 0; index < 65536; index++) {
    g_FixedCosQ28[index] = FixedMath_SineTableEntry(index + 16384);
  }
}

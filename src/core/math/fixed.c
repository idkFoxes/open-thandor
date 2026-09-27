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
  composedAngles.angle0 = (int)THANDOR_PART(uint64_t, extractedAngles, 4);
  composedAngles.angle1 = (int)((uint64_t)THANDOR_PART(uint64_t, extractedAngles, 4) >> 0x20);
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
  uint32_t elevationAngle16;
  uint32_t elevationAngleResult;
  uint32_t azimuthAngle16;
  uint32_t vectorLengthQ12;
  FixedLengthAnglesEaxEcxEdx12 lengthAnglesResult;
  int64_t squaredLengthAccumulatorQ24;
  int64_t totalSquaredLengthQ24;
  
  squaredLengthAccumulatorQ24 = (int64_t)y * (int64_t)y + (int64_t)z * (int64_t)z;
  elevationAngle16 =
       FixedMath_UInt64Sqrt
                 ((UInt64Half32)((uint64_t)squaredLengthAccumulatorQ24 >> 0x20),
                  (UInt64Half32)squaredLengthAccumulatorQ24);
  elevationAngleResult = FixedMath_Atan2Angle16(x,elevationAngle16);
  azimuthAngle16 = FixedMath_Atan2Angle16(y,z);
  totalSquaredLengthQ24 = squaredLengthAccumulatorQ24 + (int64_t)x * (int64_t)x;
  vectorLengthQ12 =
       FixedMath_UInt64Sqrt
                 ((UInt64Half32)((uint64_t)totalSquaredLengthQ24 >> 0x20),
                  (UInt64Half32)totalSquaredLengthQ24);
  lengthAnglesResult.elevationAngle = elevationAngleResult;
  lengthAnglesResult.lengthQ12 = vectorLengthQ12;
  lengthAnglesResult.azimuthAngle = azimuthAngle16 & 0xffff;
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
  uint32_t elevationAngle16;
  uint32_t elevationAngleResult;
  uint32_t azimuthAngle16;
  uint32_t vectorLengthQ12;
  FixedLengthAnglesEaxEcxEdx12 lengthAnglesResult;
  int y;
  int x;
  int64_t totalSquaredLengthQ24;
  int64_t squaredLengthAccumulatorQ24;
  GraphicsWorldCoordinateQ12 inputZQ12;
  
  x = vector->x;
  inputZQ12 = vector->z;
  y = vector->y;
  squaredLengthAccumulatorQ24 = (int64_t)y * (int64_t)y + (int64_t)x * (int64_t)x;
  elevationAngle16 =
       FixedMath_UInt64Sqrt
                 ((UInt64Half32)((uint64_t)squaredLengthAccumulatorQ24 >> 0x20),
                  (UInt64Half32)squaredLengthAccumulatorQ24);
  elevationAngleResult = FixedMath_Atan2Angle16(vector->z,elevationAngle16);
  azimuthAngle16 = FixedMath_Atan2Angle16(y,x);
  totalSquaredLengthQ24 = squaredLengthAccumulatorQ24 + (int64_t)inputZQ12 * (int64_t)inputZQ12;
  vectorLengthQ12 =
       FixedMath_UInt64Sqrt
                 ((UInt64Half32)((uint64_t)totalSquaredLengthQ24 >> 0x20),
                  (UInt64Half32)totalSquaredLengthQ24);
  lengthAnglesResult.elevationAngle = elevationAngleResult;
  lengthAnglesResult.lengthQ12 = vectorLengthQ12;
  lengthAnglesResult.azimuthAngle = azimuthAngle16 & 0xffff;
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
  uint32_t vectorAngle16;
  uint32_t vectorLengthQ12;
  FixedLengthAngleEaxEdx8 lengthAngle;

  vectorAngle16 = FixedMath_Atan2Angle16(component0,component1);
  vectorLengthQ12 = FixedMath_Length2(component0,component1);
  lengthAngle.length = vectorLengthQ12;
  lengthAngle.angle = vectorAngle16 & 0xffff;
  return lengthAngle;
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
  FixedDirection stepDirection;
  FixedElevationAzimuth vectorAngles;
  
  vectorAngles = FixedMath_VectorToAnglesVec3Regs((GraphicsFixedVec3 *)(vectorState + 0x18));
  stepDirection = FixedMath_DirectionFromAnglesScaledRegs(vectorAngles.elevationAngle,vectorAngles.azimuthAngle,directionScale);
  *(int *)(vectorState + 0x18) = *(int *)(vectorState + 0x18) - stepDirection.x * stepMultiplier;
  *(int *)(vectorState + 0x1c) = *(int *)(vectorState + 0x1c) - stepDirection.y * stepMultiplier;
  *(int *)(vectorState + 0x20) = *(int *)(vectorState + 0x20) - stepDirection.z * stepMultiplier;
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
  int64_t projectionOrHeightSquared;
  int64_t side2Squared;
  int64_t side1Squared;
  int64_t side0Squared;
  int projectionOrSquaredLow;
  uint32_t triangleHeight;
  uint32_t firstAngle16;
  AngleTurn32 fallbackAngle0;
  AngleTurn32 fallbackAngle1;
  FixedTriangleJointAnglesEaxEdx8 solvedAngles;
  FixedTriangleJointAnglesEaxEdx8 fallbackAngles;
  int64_t cosineNumerator0;
  
  projectionOrSquaredLow = (int)(((int64_t)sideLength0Q12 * (int64_t)sideLength0Q12 -
                (int64_t)sideLength1Q12 * (int64_t)sideLength1Q12) / (int64_t)sideLength2Q12);
  projectionOrHeightSquared = (int64_t)projectionOrSquaredLow * (int64_t)projectionOrSquaredLow;
  side2Squared = (int64_t)sideLength2Q12 * (int64_t)sideLength2Q12;
  side1Squared = (int64_t)sideLength1Q12 * (int64_t)sideLength1Q12;
  side0Squared = (int64_t)sideLength0Q12 * (int64_t)sideLength0Q12;
  cosineNumerator0 = (side2Squared - side1Squared) + side0Squared;
  /* 64-bit (EBX:ECX) -p^2 - s2^2 + 2*s1^2 + 2*s0^2, in the original's order */
  projectionOrHeightSquared =
       (((0 - projectionOrHeightSquared) - side2Squared) + side1Squared * 2) + side0Squared * 2;
  if ((-1 < projectionOrHeightSquared) && (0x10 < sideLength2Q12)) {
    triangleHeight = FixedMath_UInt64Sqrt((UInt64Half32)((uint64_t)projectionOrHeightSquared >> 0x20),(UInt64Half32)projectionOrHeightSquared);
    firstAngle16 = FixedMath_Atan2Angle16((int)triangleHeight >> 1,(int)(cosineNumerator0 / (int64_t)sideLength2Q12) >> 1);
    solvedAngles.jointAngle0 =
         FixedMath_Atan2Angle16
                   ((int)triangleHeight >> 1,(int)(((side1Squared + side2Squared) - side0Squared) / (int64_t)sideLength2Q12) >> 1
                   );
    solvedAngles.jointAngle1 = firstAngle16 + solvedAngles.jointAngle0;
    return solvedAngles;
  }
  if ((uint32_t)sideLength0Q12 < (uint32_t)sideLength2Q12) {
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
uint32_t __thandor_eax_preserve_ecx_edx
FixedMath_Length3(FixedMathVectorComponent32 x,FixedMathVectorComponent32 y,
                 FixedMathVectorComponent32 z)

{
  uint32_t vectorLengthQ12;
  int64_t squaredLengthAccumulatorQ24;
  
  squaredLengthAccumulatorQ24 =
       (int64_t)y * (int64_t)y + (int64_t)z * (int64_t)z + (int64_t)x * (int64_t)x;
  vectorLengthQ12 =
       FixedMath_UInt64Sqrt
                 ((UInt64Half32)((uint64_t)squaredLengthAccumulatorQ24 >> 0x20),
                  (UInt64Half32)squaredLengthAccumulatorQ24);
  return vectorLengthQ12;
}


/* Address: 0x00484E50.
   Ownership: core/math/fixed.
   Purpose: Extracts the two direction angles from the transform's third basis column. EDX=elevation angle and
   ECX=azimuth angle.
   Local calls: FixedMath_VectorToAngles3Regs.
*/
FixedVectorAngles __thandor_preserve_eax
FixedTransform_ExtractForwardAnglesRegs(GraphicsFixedMatrix3x4 *transform)

{
  FixedVectorAngles forwardAngles;
  
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
  uint32_t inputLengthQ12;
  int reciprocalLengthScaleQ32;
  int64_t normalizedComponentProduct;
  int64_t finalNormalizedComponentProduct;
  int64_t currentNormalizedComponentProduct;
  
  inputLengthQ12 = FixedMath_LengthVec3(input);
  if (inputLengthQ12 < 2) {
    output->x = 0;
    output->y = 0;
    output->z = 0;
  }
  else {
    reciprocalLengthScaleQ32 = (int)(0x100000000 / (uint64_t)inputLengthQ12);
    normalizedComponentProduct = (int64_t)reciprocalLengthScaleQ32 * (int64_t)input->x;
    output->x = (int)((uint64_t)normalizedComponentProduct >> 0x20) << 0x1c |
                (uint32_t)normalizedComponentProduct >> 4;
    currentNormalizedComponentProduct = (int64_t)reciprocalLengthScaleQ32 * (int64_t)input->y;
    output->y = (int)((uint64_t)currentNormalizedComponentProduct >> 0x20) << 0x1c |
                (uint32_t)currentNormalizedComponentProduct >> 4;
    finalNormalizedComponentProduct = (int64_t)reciprocalLengthScaleQ32 * (int64_t)input->z;
    output->z = (int)((uint64_t)finalNormalizedComponentProduct >> 0x20) << 0x1c |
                (uint32_t)finalNormalizedComponentProduct >> 4;
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
   Builds the two 256x256 cosine matrices of the .sam sound codec in one 0x40000-byte allocation (called by
   DirectSound_Init). The first (g_CosineDerivedLookupAllocation, Q12) has row u, entry k =
   cos((2k+1) * u * pi / 512), row 0 being 1/sqrt(2); the second (g_CosineDerivedLookupSecondTable, Q14) is
   its transpose, row m, entry k = cos(k * (2m+1) * pi / 512), entry 0 being 1/sqrt(2). Angles are 16-bit
   (65536 = full turn), so 0x40 is pi/512. On allocation failure the pointers stay unset.
*/
void __cdecl CosineDerivedLookupTables_Init(void)

{
  short *outputCursor;
  int entriesRemainingInRow;
  uint32_t angleStep16;
  uint32_t secondAngleStep16;
  int entriesRemaining;
  uint32_t angleIndex16;
  uint32_t secondAngleIndex16;
  ArenaAllocResult allocResult;

  allocResult = g_MemoryApi.alloc(0x40000);
  outputCursor = (short *)allocResult.payloadOrError;
  if (!allocResult.failed) {
    g_CosineDerivedLookupAllocation = outputCursor;
    /* row 0: 256 entries of 1/sqrt(2) in Q12, written as 128 pairs (REP STOSD in the original) */
    for (entriesRemainingInRow = 128; entriesRemainingInRow != 0; entriesRemainingInRow--) {
      outputCursor[0] = 2896;
      outputCursor[1] = 2896;
      outputCursor = outputCursor + 2;
    }
    /* rows 1..255: angleStep16 = u * 0x40, entries at the odd multiples (2k+1) * angleStep16 */
    angleIndex16 = 0x40;
    angleStep16 = 0x40;
    entriesRemaining = 256;
    do {
      do {
        *outputCursor = (short)((uint32_t)g_FixedCosQ28[angleIndex16] >> 16); /* Q28 -> Q12 */
        outputCursor++;
        angleIndex16 = angleIndex16 + angleStep16 * 2 & 0xffff;
        entriesRemaining--;
      } while (entriesRemaining != 0);
      angleStep16 = angleStep16 + 0x40;
      entriesRemaining = 256;
      angleIndex16 = angleStep16 & 0xffff;
    } while (angleStep16 < 0x4000);
    /* rows m = 0..255: secondAngleStep16 = (2m+1) * 0x40, entries at k * secondAngleStep16 */
    secondAngleIndex16 = 0;
    entriesRemaining = 256;
    secondAngleStep16 = 0x40;
    g_CosineDerivedLookupSecondTable = outputCursor;
    do {
      do {
        if (entriesRemaining == 256) {
          *outputCursor = 11585; /* entry 0: 1/sqrt(2) in Q14 */
        }
        else {
          *outputCursor = (short)(g_FixedCosQ28[secondAngleIndex16] >> 14); /* Q28 -> Q14 */
        }
        outputCursor++;
        secondAngleIndex16 = secondAngleIndex16 + secondAngleStep16 & 0xffff;
        entriesRemaining--;
      } while (entriesRemaining != 0);
      secondAngleStep16 = secondAngleStep16 + 0x80;
      entriesRemaining = 256;
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
  uint32_t azimuthPlusElevationAngle16;
  uint32_t azimuthMinusElevationAngle16;
  uint32_t azimuthMinusElevationIndex16;
  int32_t verticalSinQ28;
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
  uint32_t sinScaled;
  uint32_t cosScaled;

  /* SHRD by 28 of the 64-bit products: bits 28..59 */
  sinScaled = (uint32_t)((int64_t)g_FixedSinQ28[angle & 0xffff] * (int64_t)scale >> 0x1c);
  cosScaled = (uint32_t)((int64_t)g_FixedCosQ28[angle & 0xffff] * (int64_t)scale >> 0x1c);
  return (uint64_t)sinScaled << 0x20 | (uint64_t)cosScaled; /* EDX = sin, EAX = cos */
}


/* Address: 0x00484B40.
   Ownership: core/math/fixed.
   Purpose: Returns EAX=cos(angle) and EDX=sin(angle) from the Q28 tables. Kept distinct from Q12 coordinates,
   Q4/Q5 resource scales, attachment ordinals, and raw renderer flags. Typed parameters: p0 angle→AngleTurn32.
   Calling convention, storage, body bytes, control flow, and executable data remain unchanged.
*/
FixedSinCosEdxEax8 FixedMath_SinCosQ28(AngleTurn32 angle)

{
  return (uint64_t)(uint32_t)g_FixedSinQ28[angle & 0xffff] << 0x20 | (uint64_t)(uint32_t)g_FixedCosQ28[angle & 0xffff];
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
  int64_t finalBasisDotProductQ40;
  int currentBasisRowComponent0Q28;
  int64_t currentBasisDotProductQ40;
  int64_t basisDotProductAccumulatorQ40;
  
  basisDotProductAccumulatorQ40 =
       (int64_t)transform->basisRow0[1] * (int64_t)direction->y +
       (int64_t)transform->basisRow0[0] * (int64_t)direction->x +
       (int64_t)transform->basisRow0[2] * (int64_t)direction->z;
  currentBasisRowComponent0Q28 = transform->basisRow1[0];
  output->x = (int)((uint64_t)basisDotProductAccumulatorQ40 >> 0x20) << 4 |
              (uint32_t)basisDotProductAccumulatorQ40 >> 0x1c;
  currentBasisDotProductQ40 =
       (int64_t)transform->basisRow1[1] * (int64_t)direction->y +
       (int64_t)currentBasisRowComponent0Q28 * (int64_t)direction->x +
       (int64_t)transform->basisRow1[2] * (int64_t)direction->z;
  basisRow2Component0Q28 = transform->basisRow2[0];
  output->y = (int)((uint64_t)currentBasisDotProductQ40 >> 0x20) << 4 |
              (uint32_t)currentBasisDotProductQ40 >> 0x1c;
  finalBasisDotProductQ40 = (int64_t)transform->basisRow2[1] * (int64_t)direction->y +
          (int64_t)basisRow2Component0Q28 * (int64_t)direction->x +
          (int64_t)transform->basisRow2[2] * (int64_t)direction->z;
  output->z = (int)((uint64_t)finalBasisDotProductQ40 >> 0x20) << 4 | (uint32_t)finalBasisDotProductQ40 >> 0x1c;
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
  int64_t finalBasisDotProductQ40;
  int64_t currentBasisDotProductQ40;
  int currentBasisColumnRow0ComponentQ28;
  int64_t basisDotProductAccumulatorQ40;
  
  basisDotProductAccumulatorQ40 =
       (int64_t)transform->basisRow1[0] * (int64_t)direction->y +
       (int64_t)transform->basisRow0[0] * (int64_t)direction->x +
       (int64_t)transform->basisRow2[0] * (int64_t)direction->z;
  currentBasisColumnRow0ComponentQ28 = transform->basisRow0[1];
  output->x = (int)((uint64_t)basisDotProductAccumulatorQ40 >> 0x20) << 4 |
              (uint32_t)basisDotProductAccumulatorQ40 >> 0x1c;
  currentBasisDotProductQ40 =
       (int64_t)transform->basisRow1[1] * (int64_t)direction->y +
       (int64_t)currentBasisColumnRow0ComponentQ28 * (int64_t)direction->x +
       (int64_t)transform->basisRow2[1] * (int64_t)direction->z;
  basisRow0Component2Q28 = transform->basisRow0[2];
  output->y = (int)((uint64_t)currentBasisDotProductQ40 >> 0x20) << 4 |
              (uint32_t)currentBasisDotProductQ40 >> 0x1c;
  finalBasisDotProductQ40 = (int64_t)transform->basisRow1[2] * (int64_t)direction->y +
          (int64_t)basisRow0Component2Q28 * (int64_t)direction->x +
          (int64_t)transform->basisRow2[2] * (int64_t)direction->z;
  output->z = (int)((uint64_t)finalBasisDotProductQ40 >> 0x20) << 4 | (uint32_t)finalBasisDotProductQ40 >> 0x1c;
  return;
}


/* Address: 0x00485520.
   Ownership: core/math/fixed.
   Purpose: It assumes the basis is a rotation matrix.
*/
void __thandor_void_preserve_eax_ecx_edx
FixedTransform_InvertRigidQ28(GraphicsFixedMatrix3x4 *output,GraphicsFixedMatrix3x4 *input)

{
  int64_t cofactorOrTranslationProduct;
  int64_t translationYProduct;
  int64_t translationZProduct;
  uint32_t negatedProductXLow;
  int productXLow;
  uint32_t productYLow;
  uint32_t productZLow;
  int componentOrProductLow;
  uint32_t partialDifferenceLow;
  
  cofactorOrTranslationProduct = (int64_t)input->basisRow1[1] * (int64_t)input->basisRow2[2] -
          (int64_t)input->basisRow1[2] * (int64_t)input->basisRow2[1];
  componentOrProductLow = input->basisRow1[2];
  output->basisRow0[0] = (int)((uint64_t)cofactorOrTranslationProduct >> 0x20) << 4 | (uint32_t)cofactorOrTranslationProduct >> 0x1c;
  cofactorOrTranslationProduct = (int64_t)componentOrProductLow * (int64_t)input->basisRow2[0] -
          (int64_t)input->basisRow1[0] * (int64_t)input->basisRow2[2];
  componentOrProductLow = input->basisRow1[0];
  output->basisRow1[0] = (int)((uint64_t)cofactorOrTranslationProduct >> 0x20) << 4 | (uint32_t)cofactorOrTranslationProduct >> 0x1c;
  cofactorOrTranslationProduct = (int64_t)componentOrProductLow * (int64_t)input->basisRow2[1] -
          (int64_t)input->basisRow1[1] * (int64_t)input->basisRow2[0];
  componentOrProductLow = input->basisRow0[2];
  output->basisRow2[0] = (int)((uint64_t)cofactorOrTranslationProduct >> 0x20) << 4 | (uint32_t)cofactorOrTranslationProduct >> 0x1c;
  cofactorOrTranslationProduct = (int64_t)componentOrProductLow * (int64_t)input->basisRow2[1] -
          (int64_t)input->basisRow0[1] * (int64_t)input->basisRow2[2];
  componentOrProductLow = input->basisRow0[0];
  output->basisRow0[1] = (int)((uint64_t)cofactorOrTranslationProduct >> 0x20) << 4 | (uint32_t)cofactorOrTranslationProduct >> 0x1c;
  cofactorOrTranslationProduct = (int64_t)componentOrProductLow * (int64_t)input->basisRow2[2] -
          (int64_t)input->basisRow0[2] * (int64_t)input->basisRow2[0];
  componentOrProductLow = input->basisRow0[1];
  output->basisRow1[1] = (int)((uint64_t)cofactorOrTranslationProduct >> 0x20) << 4 | (uint32_t)cofactorOrTranslationProduct >> 0x1c;
  cofactorOrTranslationProduct = (int64_t)componentOrProductLow * (int64_t)input->basisRow2[0] -
          (int64_t)input->basisRow0[0] * (int64_t)input->basisRow2[1];
  componentOrProductLow = input->basisRow0[1];
  output->basisRow2[1] = (int)((uint64_t)cofactorOrTranslationProduct >> 0x20) << 4 | (uint32_t)cofactorOrTranslationProduct >> 0x1c;
  cofactorOrTranslationProduct = (int64_t)componentOrProductLow * (int64_t)input->basisRow1[2] -
          (int64_t)input->basisRow0[2] * (int64_t)input->basisRow1[1];
  componentOrProductLow = input->basisRow0[2];
  output->basisRow0[2] = (int)((uint64_t)cofactorOrTranslationProduct >> 0x20) << 4 | (uint32_t)cofactorOrTranslationProduct >> 0x1c;
  cofactorOrTranslationProduct = (int64_t)componentOrProductLow * (int64_t)input->basisRow1[0] -
          (int64_t)input->basisRow0[0] * (int64_t)input->basisRow1[2];
  componentOrProductLow = input->basisRow0[0];
  output->basisRow1[2] = (int)((uint64_t)cofactorOrTranslationProduct >> 0x20) << 4 | (uint32_t)cofactorOrTranslationProduct >> 0x1c;
  cofactorOrTranslationProduct = (int64_t)componentOrProductLow * (int64_t)input->basisRow1[1] -
          (int64_t)input->basisRow0[1] * (int64_t)input->basisRow1[0];
  output->basisRow2[2] = (int)((uint64_t)cofactorOrTranslationProduct >> 0x20) << 4 | (uint32_t)cofactorOrTranslationProduct >> 0x1c;
  cofactorOrTranslationProduct = (int64_t)(input->translation).x * (int64_t)output->basisRow0[0];
  productXLow = (int)cofactorOrTranslationProduct;
  negatedProductXLow = -productXLow;
  translationYProduct = (int64_t)(input->translation).y * (int64_t)output->basisRow0[1];
  productYLow = (uint32_t)translationYProduct;
  partialDifferenceLow = negatedProductXLow - productYLow;
  translationZProduct = (int64_t)(input->translation).z * (int64_t)output->basisRow0[2];
  productZLow = (uint32_t)translationZProduct;
  componentOrProductLow = (input->translation).x;
  (output->translation).x =
       (((((-(uint32_t)(productXLow != 0) - (int)((uint64_t)cofactorOrTranslationProduct >> 0x20)) - (int)((uint64_t)translationYProduct >> 0x20)
          ) - (uint32_t)(negatedProductXLow < productYLow)) - (int)((uint64_t)translationZProduct >> 0x20)) - (uint32_t)(partialDifferenceLow < productZLow)) *
       0x10 | partialDifferenceLow - productZLow >> 0x1c;
  cofactorOrTranslationProduct = (int64_t)componentOrProductLow * (int64_t)output->basisRow1[0];
  productXLow = (int)cofactorOrTranslationProduct;
  negatedProductXLow = -productXLow;
  translationYProduct = (int64_t)(input->translation).y * (int64_t)output->basisRow1[1];
  productYLow = (uint32_t)translationYProduct;
  partialDifferenceLow = negatedProductXLow - productYLow;
  translationZProduct = (int64_t)(input->translation).z * (int64_t)output->basisRow1[2];
  productZLow = (uint32_t)translationZProduct;
  componentOrProductLow = (input->translation).x;
  (output->translation).y =
       (((((-(uint32_t)(productXLow != 0) - (int)((uint64_t)cofactorOrTranslationProduct >> 0x20)) - (int)((uint64_t)translationYProduct >> 0x20)
          ) - (uint32_t)(negatedProductXLow < productYLow)) - (int)((uint64_t)translationZProduct >> 0x20)) - (uint32_t)(partialDifferenceLow < productZLow)) *
       0x10 | partialDifferenceLow - productZLow >> 0x1c;
  cofactorOrTranslationProduct = (int64_t)componentOrProductLow * (int64_t)output->basisRow2[0];
  componentOrProductLow = (int)cofactorOrTranslationProduct;
  negatedProductXLow = -componentOrProductLow;
  translationYProduct = (int64_t)(input->translation).y * (int64_t)output->basisRow2[1];
  productYLow = (uint32_t)translationYProduct;
  partialDifferenceLow = negatedProductXLow - productYLow;
  translationZProduct = (int64_t)(input->translation).z * (int64_t)output->basisRow2[2];
  productZLow = (uint32_t)translationZProduct;
  (output->translation).z =
       (((((-(uint32_t)(componentOrProductLow != 0) - (int)((uint64_t)cofactorOrTranslationProduct >> 0x20)) - (int)((uint64_t)translationYProduct >> 0x20)
          ) - (uint32_t)(negatedProductXLow < productYLow)) - (int)((uint64_t)translationZProduct >> 0x20)) - (uint32_t)(partialDifferenceLow < productZLow)) *
       0x10 | partialDifferenceLow - productZLow >> 0x1c;
  return;
}


/* Address: 0x004856B0.
   Ownership: core/math/fixed.
   Purpose: Returns (left.x*right.x + left.y*right.y + left.z*right.z) shifted right by 12.
*/
int32_t __thandor_eax_preserve_ecx_edx
FixedVec3_DotQ12(GraphicsFixedVec3 *left,GraphicsFixedVec3 *right)

{
  int64_t dotProductAccumulatorQ24;
  
  dotProductAccumulatorQ24 =
       (int64_t)right->y * (int64_t)left->y + (int64_t)right->x * (int64_t)left->x +
       (int64_t)right->z * (int64_t)left->z;
  return (uint32_t)dotProductAccumulatorQ24 >> 0xc |
         (int)((uint64_t)dotProductAccumulatorQ24 >> 0x20) << 0x14;
}


/* Address: 0x004856F0.
   Ownership: core/math/fixed.
   Purpose: Returns (left.x*right.x + left.y*right.y + left.z*right.z) shifted right by 28.
*/
int32_t __thandor_eax_preserve_ecx_edx
FixedVec3_DotQ28(GraphicsFixedVec3 *left,GraphicsFixedVec3 *right)

{
  int64_t dotProductAccumulatorQ56;
  
  dotProductAccumulatorQ56 =
       (int64_t)right->y * (int64_t)left->y + (int64_t)right->x * (int64_t)left->x +
       (int64_t)right->z * (int64_t)left->z;
  return (uint32_t)dotProductAccumulatorQ56 >> 0x1c |
         (int)((uint64_t)dotProductAccumulatorQ56 >> 0x20) << 4;
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
  int64_t finalCrossProductDifferenceQ24;
  int currentLeftComponentQ12;
  int64_t currentCrossProductDifferenceQ24;
  int64_t crossComponentProductDifferenceQ24;
  
  crossComponentProductDifferenceQ24 =
       (int64_t)leftOperand->y * (int64_t)rightOperand->z -
       (int64_t)leftOperand->z * (int64_t)rightOperand->y;
  currentLeftComponentQ12 = leftOperand->z;
  output->x = (int)((uint64_t)crossComponentProductDifferenceQ24 >> 0x20) << 0x14 |
              (uint32_t)crossComponentProductDifferenceQ24 >> 0xc;
  currentCrossProductDifferenceQ24 =
       (int64_t)currentLeftComponentQ12 * (int64_t)rightOperand->x -
       (int64_t)leftOperand->x * (int64_t)rightOperand->z;
  leftXQ12 = leftOperand->x;
  output->y = (int)((uint64_t)currentCrossProductDifferenceQ24 >> 0x20) << 0x14 |
              (uint32_t)currentCrossProductDifferenceQ24 >> 0xc;
  finalCrossProductDifferenceQ24 = (int64_t)leftXQ12 * (int64_t)rightOperand->y -
          (int64_t)leftOperand->y * (int64_t)rightOperand->x;
  output->z = (int)((uint64_t)finalCrossProductDifferenceQ24 >> 0x20) << 0x14 | (uint32_t)finalCrossProductDifferenceQ24 >> 0xc;
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
  uint32_t pointX;
  uint32_t pointY;

  /* SHLD EDX,EAX,4 of the 64-bit products: bits 28..59 */
  pointX = baseX + (uint32_t)((int64_t)g_FixedCosQ28[angle16 & 0xffff] * (int64_t)distance >> 0x1c);
  pointY = (uint32_t)((int64_t)g_FixedSinQ28[angle16 & 0xffff] * (int64_t)distance >> 0x1c) + baseY;
  return (uint64_t)pointY << 0x20 | (uint64_t)pointX; /* EDX = y, EAX = x */
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
FixedElevationAzimuth __thandor_preserve_eax
FixedMath_VectorToAnglesVec3Regs(GraphicsFixedVec3 *vector)

{
  uint32_t magnitudeOrElevationAngle;
  uint32_t azimuthAngle16;
  int y;
  int x;
  int64_t horizontalSquaredLengthAccumulatorQ24;
  FixedElevationAzimuth vectorAngles;
  
  x = vector->x;
  y = vector->y;
  horizontalSquaredLengthAccumulatorQ24 = (int64_t)y * (int64_t)y + (int64_t)x * (int64_t)x;
  magnitudeOrElevationAngle = FixedMath_UInt64Sqrt
                    ((UInt64Half32)((uint64_t)horizontalSquaredLengthAccumulatorQ24 >> 0x20),
                     (UInt64Half32)horizontalSquaredLengthAccumulatorQ24);
  magnitudeOrElevationAngle = FixedMath_Atan2Angle16(vector->z,magnitudeOrElevationAngle);
  azimuthAngle16 = FixedMath_Atan2Angle16(y,x);
  /* The original returns EDX = elevation and ECX = azimuth & 0xffff (as FixedMath_VectorToAngles3Regs).
     The port returns them elevation first (FixedElevationAzimuth), which its consumers
     (FixedVector_StepBackwardAlongOwnDirection, ArmyArticulatedRuntime_UpdateSuspensionHierarchy) expect. */
  vectorAngles.elevationAngle = magnitudeOrElevationAngle;
  vectorAngles.azimuthAngle = azimuthAngle16 & 0xffff;
  return vectorAngles;
}


/* Address: 0x00484E00.
   Ownership: core/math/fixed.
   Purpose: Extracts three wrapping 16-bit orientation angles from the 3x3 basis. EAX, EDX, and ECX hold the three
   results. The declared 64-bit return models EDX:EAX; ECX remains an extra output.
   Local calls: FixedMath_VectorToAngles3Regs, FixedMath_Atan2Angle16.
*/
FixedEulerAnglesEaxEcxEdx12 FixedTransform_ExtractEulerAnglesRegs(GraphicsFixedMatrix3x4 *transform)

{
  uint32_t extractedRotationAngle2;
  uint32_t gimbalAngle16;
  FixedVectorAngles forwardAngles;
  FixedEulerAnglesEaxEcxEdx12 eulerAngles;
  
  forwardAngles = FixedMath_VectorToAngles3Regs
                    (transform->basisRow2[2],transform->basisRow1[2],transform->basisRow0[2]);
  eulerAngles.edxAngle = forwardAngles.elevationAngle;
  eulerAngles.ecxAngle = forwardAngles.azimuthAngle;
  if ((int)forwardAngles.elevationAngle < 0) { /* TEST EDX,EDX: elevation sign */
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
uint32_t __thandor_eax_preserve_ecx_edx FixedMath_LengthVec3(GraphicsFixedVec3 *vector)

{
  uint32_t vectorLengthQ12;
  int64_t squaredLengthAccumulatorQ24;
  
  squaredLengthAccumulatorQ24 =
       (int64_t)vector->y * (int64_t)vector->y + (int64_t)vector->x * (int64_t)vector->x +
       (int64_t)vector->z * (int64_t)vector->z;
  vectorLengthQ12 =
       FixedMath_UInt64Sqrt
                 ((UInt64Half32)((uint64_t)squaredLengthAccumulatorQ24 >> 0x20),
                  (UInt64Half32)squaredLengthAccumulatorQ24);
  return vectorLengthQ12;
}


/* Address: 0x00484CF0.
   Length of the 2D vector (x, y): floor(sqrt(x*x + y*y)), with the squares summed in 64 bits so Q12
   components cannot overflow. The result has the components' fixed-point scale.
*/
uint32_t __thandor_eax_preserve_ecx_edx
FixedMath_Length2(FixedMathVectorComponent32 x,FixedMathVectorComponent32 y)

{
  uint32_t vectorLengthQ12;
  int64_t squaredLengthAccumulatorQ24;

  squaredLengthAccumulatorQ24 = (int64_t)x * (int64_t)x + (int64_t)y * (int64_t)y;
  vectorLengthQ12 =
       FixedMath_UInt64Sqrt
                 ((UInt64Half32)((uint64_t)squaredLengthAccumulatorQ24 >> 32),
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
FixedDirection
FixedMath_DirectionFromAnglesScaledRegs
          (AngleTurn32 elevationAngle,AngleTurn32 azimuthAngle,FixedMathScale32 scale)

{
  int64_t scaledYComponentProduct;
  uint32_t sumAngle16;
  uint32_t elevationAngle16;
  uint32_t differenceAngle16;
  FixedDirection scaledDirection;
  int64_t scaledHorizontalComponentProduct;
  
  elevationAngle16 = elevationAngle & 0xffff;
  sumAngle16 = elevationAngle16 + azimuthAngle & 0xffff;
  differenceAngle16 = azimuthAngle - elevationAngle16 & 0xffff;
  scaledHorizontalComponentProduct =
       (int64_t)(g_FixedCosQ28[sumAngle16] + g_FixedCosQ28[differenceAngle16]) * (int64_t)scale;
  scaledYComponentProduct = (int64_t)(g_FixedSinQ28[sumAngle16] + g_FixedSinQ28[differenceAngle16]) * (int64_t)scale;
  scaledDirection.z = (int)((uint64_t)((int64_t)g_FixedSinQ28[elevationAngle16] * (int64_t)scale) >> 0x20
                   ) << 4 |
              (uint32_t)((int64_t)g_FixedSinQ28[elevationAngle16] * (int64_t)scale) >> 0x1c;
  scaledDirection.y = (int)((uint64_t)scaledYComponentProduct >> 0x20) << 3 | (uint32_t)scaledYComponentProduct >> 0x1d;
  scaledDirection.x = (int)((uint64_t)scaledHorizontalComponentProduct >> 0x20) << 3 |
              (uint32_t)scaledHorizontalComponentProduct >> 0x1d;
  return scaledDirection;
}


/* Address: 0x004847E0.
   Ownership: core/math/fixed.
   Purpose: Builds a Q28 direction vector. Register outputs are EAX=x, ECX=y, EDX=z. The declared 64-bit C return
   models only EDX:EAX; ECX remains an extra output. (heading, pitch) angle16 pair -> Q28 direction vector;
   rotation-basis row builder. Kept distinct from Q12 coordinates, Q4/Q5 resource scales, attachment ordinals, and
   raw renderer flags.
*/
FixedDirection
FixedMath_DirectionFromAnglesQ28Regs(AngleTurn32 elevationAngle,AngleTurn32 azimuthAngle)

{
  uint32_t sumAngle16;
  uint32_t elevationAngle16;
  uint32_t differenceAngle16;
  FixedDirection directionQ28;
  
  elevationAngle16 = elevationAngle & 0xffff;
  sumAngle16 = elevationAngle16 + azimuthAngle & 0xffff;
  differenceAngle16 = azimuthAngle - elevationAngle16 & 0xffff;
  directionQ28.x = g_FixedCosQ28[sumAngle16] + g_FixedCosQ28[differenceAngle16] >> 1;
  directionQ28.y = g_FixedSinQ28[sumAngle16] + g_FixedSinQ28[differenceAngle16] >> 1;
  directionQ28.z = g_FixedSinQ28[elevationAngle16];
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
  uint32_t azimuthPlusElevationAngle16;
  uint32_t elevationOrDifferenceAngle16;
  int verticalSinQ28;
  int azimuthPlusElevationSinQ28;
  int azimuthMinusElevationSinQ28;
  int64_t yComponentScaleProduct;
  int64_t horizontalComponentScaleProduct;
  
  elevationOrDifferenceAngle16 = elevationAngle & 0xffff;
  verticalSinQ28 = g_FixedSinQ28[elevationOrDifferenceAngle16];
  azimuthPlusElevationAngle16 = elevationOrDifferenceAngle16 + azimuthAngle & 0xffff;
  elevationOrDifferenceAngle16 = azimuthAngle - elevationOrDifferenceAngle16 & 0xffff;
  azimuthPlusElevationSinQ28 = g_FixedSinQ28[azimuthPlusElevationAngle16];
  azimuthMinusElevationSinQ28 = g_FixedSinQ28[elevationOrDifferenceAngle16];
  horizontalComponentScaleProduct =
       (int64_t)(g_FixedCosQ28[azimuthPlusElevationAngle16] + g_FixedCosQ28[elevationOrDifferenceAngle16]) *
       (int64_t)scale;
  output->x = (int)((uint64_t)horizontalComponentScaleProduct >> 0x20) << 3 |
              (uint32_t)horizontalComponentScaleProduct >> 0x1d;
  yComponentScaleProduct =
       (int64_t)(azimuthPlusElevationSinQ28 + azimuthMinusElevationSinQ28) * (int64_t)scale;
  output->y = (int)((uint64_t)yComponentScaleProduct >> 0x20) << 3 |
              (uint32_t)yComponentScaleProduct >> 0x1d;
  output->z = (int)((uint64_t)((int64_t)verticalSinQ28 * (int64_t)scale) >> 0x20) << 4 |
              (uint32_t)((int64_t)verticalSinQ28 * (int64_t)scale) >> 0x1c;
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
  int64_t composedProductSumQ56;
  int currentRightBasisRowComponent0Q28;
  int64_t currentComposedComponentProductSumQ56;
  
  currentComposedComponentProductSumQ56 =
       (int64_t)transformB->basisRow0[1] * (int64_t)transformA->basisRow1[0] +
       (int64_t)transformB->basisRow0[0] * (int64_t)transformA->basisRow0[0] +
       (int64_t)transformB->basisRow0[2] * (int64_t)transformA->basisRow2[0];
  currentRightBasisRowComponent0Q28 = transformB->basisRow0[0];
  output->basisRow0[0] =
       (int)((uint64_t)currentComposedComponentProductSumQ56 >> 0x20) << 4 |
       (uint32_t)currentComposedComponentProductSumQ56 >> 0x1c;
  composedProductSumQ56 = (int64_t)transformB->basisRow0[1] * (int64_t)transformA->basisRow1[1] +
          (int64_t)currentRightBasisRowComponent0Q28 * (int64_t)transformA->basisRow0[1] +
          (int64_t)transformB->basisRow0[2] * (int64_t)transformA->basisRow2[1];
  rightBasisComponent0Q28 = transformB->basisRow0[0];
  output->basisRow0[1] = (int)((uint64_t)composedProductSumQ56 >> 0x20) << 4 | (uint32_t)composedProductSumQ56 >> 0x1c;
  composedProductSumQ56 = (int64_t)transformB->basisRow0[1] * (int64_t)transformA->basisRow1[2] +
          (int64_t)rightBasisComponent0Q28 * (int64_t)transformA->basisRow0[2] +
          (int64_t)transformB->basisRow0[2] * (int64_t)transformA->basisRow2[2];
  rightBasisComponent0Q28 = transformB->basisRow0[0];
  output->basisRow0[2] = (int)((uint64_t)composedProductSumQ56 >> 0x20) << 4 | (uint32_t)composedProductSumQ56 >> 0x1c;
  composedProductSumQ56 = (int64_t)transformB->basisRow0[1] * (int64_t)(transformA->translation).y +
          (int64_t)rightBasisComponent0Q28 * (int64_t)(transformA->translation).x +
          (int64_t)transformB->basisRow0[2] * (int64_t)(transformA->translation).z;
  rightBasisComponent0Q28 = transformB->basisRow1[0];
  (output->translation).x =
       ((int)((uint64_t)composedProductSumQ56 >> 0x20) << 4 | (uint32_t)composedProductSumQ56 >> 0x1c) + (transformB->translation).x;
  composedProductSumQ56 = (int64_t)transformB->basisRow1[1] * (int64_t)transformA->basisRow1[0] +
          (int64_t)rightBasisComponent0Q28 * (int64_t)transformA->basisRow0[0] +
          (int64_t)transformB->basisRow1[2] * (int64_t)transformA->basisRow2[0];
  rightBasisComponent0Q28 = transformB->basisRow1[0];
  output->basisRow1[0] = (int)((uint64_t)composedProductSumQ56 >> 0x20) << 4 | (uint32_t)composedProductSumQ56 >> 0x1c;
  composedProductSumQ56 = (int64_t)transformB->basisRow1[1] * (int64_t)transformA->basisRow1[1] +
          (int64_t)rightBasisComponent0Q28 * (int64_t)transformA->basisRow0[1] +
          (int64_t)transformB->basisRow1[2] * (int64_t)transformA->basisRow2[1];
  rightBasisComponent0Q28 = transformB->basisRow1[0];
  output->basisRow1[1] = (int)((uint64_t)composedProductSumQ56 >> 0x20) << 4 | (uint32_t)composedProductSumQ56 >> 0x1c;
  composedProductSumQ56 = (int64_t)transformB->basisRow1[1] * (int64_t)transformA->basisRow1[2] +
          (int64_t)rightBasisComponent0Q28 * (int64_t)transformA->basisRow0[2] +
          (int64_t)transformB->basisRow1[2] * (int64_t)transformA->basisRow2[2];
  rightBasisComponent0Q28 = transformB->basisRow1[0];
  output->basisRow1[2] = (int)((uint64_t)composedProductSumQ56 >> 0x20) << 4 | (uint32_t)composedProductSumQ56 >> 0x1c;
  composedProductSumQ56 = (int64_t)transformB->basisRow1[1] * (int64_t)(transformA->translation).y +
          (int64_t)rightBasisComponent0Q28 * (int64_t)(transformA->translation).x +
          (int64_t)transformB->basisRow1[2] * (int64_t)(transformA->translation).z;
  rightBasisComponent0Q28 = transformB->basisRow2[0];
  (output->translation).y =
       ((int)((uint64_t)composedProductSumQ56 >> 0x20) << 4 | (uint32_t)composedProductSumQ56 >> 0x1c) + (transformB->translation).y;
  composedProductSumQ56 = (int64_t)transformB->basisRow2[1] * (int64_t)transformA->basisRow1[0] +
          (int64_t)rightBasisComponent0Q28 * (int64_t)transformA->basisRow0[0] +
          (int64_t)transformB->basisRow2[2] * (int64_t)transformA->basisRow2[0];
  rightBasisComponent0Q28 = transformB->basisRow2[0];
  output->basisRow2[0] = (int)((uint64_t)composedProductSumQ56 >> 0x20) << 4 | (uint32_t)composedProductSumQ56 >> 0x1c;
  composedProductSumQ56 = (int64_t)transformB->basisRow2[1] * (int64_t)transformA->basisRow1[1] +
          (int64_t)rightBasisComponent0Q28 * (int64_t)transformA->basisRow0[1] +
          (int64_t)transformB->basisRow2[2] * (int64_t)transformA->basisRow2[1];
  rightBasisComponent0Q28 = transformB->basisRow2[0];
  output->basisRow2[1] = (int)((uint64_t)composedProductSumQ56 >> 0x20) << 4 | (uint32_t)composedProductSumQ56 >> 0x1c;
  composedProductSumQ56 = (int64_t)transformB->basisRow2[1] * (int64_t)transformA->basisRow1[2] +
          (int64_t)rightBasisComponent0Q28 * (int64_t)transformA->basisRow0[2] +
          (int64_t)transformB->basisRow2[2] * (int64_t)transformA->basisRow2[2];
  rightBasisComponent0Q28 = transformB->basisRow2[0];
  output->basisRow2[2] = (int)((uint64_t)composedProductSumQ56 >> 0x20) << 4 | (uint32_t)composedProductSumQ56 >> 0x1c;
  composedProductSumQ56 = (int64_t)transformB->basisRow2[1] * (int64_t)(transformA->translation).y +
          (int64_t)rightBasisComponent0Q28 * (int64_t)(transformA->translation).x +
          (int64_t)transformB->basisRow2[2] * (int64_t)(transformA->translation).z;
  (output->translation).z =
       ((int)((uint64_t)composedProductSumQ56 >> 0x20) << 4 | (uint32_t)composedProductSumQ56 >> 0x1c) + (transformB->translation).z;
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
FixedVectorAngles __thandor_preserve_eax
FixedMath_VectorToAngles3Regs
          (FixedMathVectorComponent32 x,FixedMathVectorComponent32 y,FixedMathVectorComponent32 z)

{
  uint32_t horizontalMagnitudeQ12;
  uint32_t elevationAngle16;
  uint32_t azimuthAngle16;
  int64_t horizontalMagnitudeSquaredQ24;
  FixedVectorAngles vectorAngles;
  
  horizontalMagnitudeSquaredQ24 = (int64_t)y * (int64_t)y + (int64_t)z * (int64_t)z;
  horizontalMagnitudeQ12 =
       FixedMath_UInt64Sqrt
                 ((UInt64Half32)((uint64_t)horizontalMagnitudeSquaredQ24 >> 0x20),
                  (UInt64Half32)horizontalMagnitudeSquaredQ24);
  elevationAngle16 = FixedMath_Atan2Angle16(x,horizontalMagnitudeQ12);
  azimuthAngle16 = FixedMath_Atan2Angle16(y,z);
  vectorAngles.azimuthAngle = azimuthAngle16 & 0xffff;
  vectorAngles.elevationAngle = elevationAngle16;
  return vectorAngles;
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
  int64_t basisDotProductQ40;
  int currentBasisRowComponent0Q28;
  int64_t currentBasisDotProductQ40;
  
  currentBasisDotProductQ40 =
       (int64_t)transform->basisRow0[1] * (int64_t)point->y +
       (int64_t)transform->basisRow0[0] * (int64_t)point->x +
       (int64_t)transform->basisRow0[2] * (int64_t)point->z;
  currentBasisRowComponent0Q28 = transform->basisRow1[0];
  output->x = ((int)((uint64_t)currentBasisDotProductQ40 >> 0x20) << 4 |
              (uint32_t)currentBasisDotProductQ40 >> 0x1c) + (transform->translation).x;
  basisDotProductQ40 = (int64_t)transform->basisRow1[1] * (int64_t)point->y +
          (int64_t)currentBasisRowComponent0Q28 * (int64_t)point->x +
          (int64_t)transform->basisRow1[2] * (int64_t)point->z;
  basisRow2Component0Q28 = transform->basisRow2[0];
  output->y = ((int)((uint64_t)basisDotProductQ40 >> 0x20) << 4 | (uint32_t)basisDotProductQ40 >> 0x1c) +
              (transform->translation).y;
  basisDotProductQ40 = (int64_t)transform->basisRow2[1] * (int64_t)point->y +
          (int64_t)basisRow2Component0Q28 * (int64_t)point->x +
          (int64_t)transform->basisRow2[2] * (int64_t)point->z;
  output->z = ((int)((uint64_t)basisDotProductQ40 >> 0x20) << 4 | (uint32_t)basisDotProductQ40 >> 0x1c) +
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
  int64_t rowComponentTimesVerticalSinProduct;
  int currentSymmetricComponentQ28;
  int symmetricComponentQ28;
  uint32_t differenceAngleIndex16;
  uint32_t secondarySymmetricAngleIndex16;
  uint32_t primarySymmetricAngleIndex16;
  FixedDirectionXZEdxEax8 directionSamplePairQ28;
  FixedDirectionXZEdxEax8 secondaryDirectionSamplePairQ28;
  FixedDirection directionRow;
  int64_t currentComponentTimesVerticalSinProduct;
  int verticalSinQ28;
  int64_t componentTimesVerticalSinProduct;
  int64_t secondaryComponentTimesVerticalSinProduct;
  
  directionRow = FixedMath_DirectionFromAnglesQ28Regs(angle1,angle2);
  output->basisRow0[2] = directionRow.x;
  output->basisRow1[2] = directionRow.y;
  output->basisRow2[2] = directionRow.z;
  directionRow = FixedMath_DirectionFromAnglesQ28Regs(angle1,angle0 - angle2);
  secondarySymmetricAngleIndex16 = (angle0 - angle2) + angle2;
  output->basisRow2[1] = directionRow.y;
  output->basisRow2[0] = -directionRow.x;
  primarySymmetricAngleIndex16 = secondarySymmetricAngleIndex16 & 0xffff;
  differenceAngleIndex16 = secondarySymmetricAngleIndex16 + angle2 * -2 & 0xffff;
  currentSymmetricComponentQ28 =
       g_FixedCosQ28[primarySymmetricAngleIndex16] - g_FixedCosQ28[differenceAngleIndex16] >> 1;
  verticalSinQ28 = g_FixedSinQ28[angle1 & 0xffff];
  output->basisRow0[0] = currentSymmetricComponentQ28;
  symmetricComponentQ28 = g_FixedCosQ28[primarySymmetricAngleIndex16] + g_FixedCosQ28[differenceAngleIndex16] >> 1;
  output->basisRow1[1] =
       ((int)((uint64_t)((int64_t)currentSymmetricComponentQ28 * (int64_t)verticalSinQ28) >> 0x20
             ) << 4 |
       (uint32_t)((int64_t)currentSymmetricComponentQ28 * (int64_t)verticalSinQ28) >> 0x1c) + symmetricComponentQ28;
  currentComponentTimesVerticalSinProduct = (int64_t)symmetricComponentQ28 * (int64_t)verticalSinQ28;
  output->basisRow0[0] =
       output->basisRow0[0] +
       ((int)((uint64_t)currentComponentTimesVerticalSinProduct >> 0x20) << 4 |
       (uint32_t)currentComponentTimesVerticalSinProduct >> 0x1c);
  symmetricComponentQ28 = g_FixedSinQ28[primarySymmetricAngleIndex16] + g_FixedSinQ28[differenceAngleIndex16] >> 1;
  output->basisRow1[0] = symmetricComponentQ28;
  rowComponentTimesVerticalSinProduct = (int64_t)symmetricComponentQ28 * (int64_t)verticalSinQ28;
  symmetricComponentQ28 = g_FixedSinQ28[differenceAngleIndex16] - g_FixedSinQ28[primarySymmetricAngleIndex16] >> 1;
  secondaryComponentTimesVerticalSinProduct = (int64_t)symmetricComponentQ28 * (int64_t)verticalSinQ28;
  output->basisRow0[1] = symmetricComponentQ28 - ((int)((uint64_t)rowComponentTimesVerticalSinProduct >> 0x20) << 4 | (uint32_t)rowComponentTimesVerticalSinProduct >> 0x1c);
  output->basisRow1[0] =
       output->basisRow1[0] -
       ((int)((uint64_t)secondaryComponentTimesVerticalSinProduct >> 0x20) << 4 |
       (uint32_t)secondaryComponentTimesVerticalSinProduct >> 0x1c);
  return;
}


/* Address: 0x00484BA0.
   Ownership: core/math/fixed.
   Purpose: Approximates atan2(y, x) as a wrapping 16-bit engine angle. Fixed-point atan2 -> angle16; euler
   extraction callee. Typed parameters: p0 y→FixedMathVectorComponent32_V342, p1 x→FixedMathVectorComponent32_V342.
   Calling convention, exact VariableStorage serialization, function body bytes, control flow, globals, locals, and
   executable data remain unchanged.
*/
uint32_t __thandor_eax_preserve_ecx_edx
FixedMath_Atan2Angle16(FixedMathVectorComponent32 y,FixedMathVectorComponent32 x)

{
  uint32_t angle16Result;
  int denominatorOrRatio;
  uint32_t reducedAngleNumerator;
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
    denominatorOrRatio = (int)((int64_t)((uint64_t)reducedAngleNumerator << 0x20) / (int64_t)(denominatorOrRatio * 2));
    doubledYOrRatioSquared = (int)((uint64_t)((int64_t)denominatorOrRatio * (int64_t)denominatorOrRatio) >> 0x20);
    angle16Result =
         octantBaseAngle16 +
         (int)((uint64_t)
               ((int64_t)denominatorOrRatio *
               (int64_t)
               ((int)((uint64_t)
                      ((int64_t)doubledYOrRatioSquared *
                      (int64_t)((int)((uint64_t)((int64_t)doubledYOrRatioSquared * 0x104c2) >> 0x20) + -0x6ca6))
                     >> 0x20) + 0x517d)) >> 0x20);
  }
  return angle16Result;
}


/* Address: 0x004846A0.
   Ownership: core/math/fixed.
   Purpose: Computes the recovered Q12 fixed-point square-root approximation.
*/
uint32_t __thandor_eax_preserve_ecx_edx FixedMath_SqrtQ12Approx(uint32_t inputValue)

{
  int highestBitOrNormalized;
  uint32_t normalizeShift;
  
  highestBitOrNormalized = 0x1f;
  if (inputValue != 0) {
    for (; inputValue >> highestBitOrNormalized == 0; highestBitOrNormalized = highestBitOrNormalized + -1) {
    }
  }
  if (inputValue != 0) {
    normalizeShift = 0x1cU - highestBitOrNormalized & 0x1e;
    highestBitOrNormalized = inputValue << (int8_t)normalizeShift;
    return (int)((uint64_t)
                 ((int64_t)highestBitOrNormalized *
                 (int64_t)
                 ((int)((uint64_t)
                        ((int64_t)highestBitOrNormalized *
                        (int64_t)
                        ((int)((uint64_t)((int64_t)highestBitOrNormalized * 0x25ed098) >> 0x20) + -0x1c71c71)) >>
                       0x20) + 0xb1c71c)) >> 0x20) + 0x66b75U >> (int8_t)(normalizeShift >> 1);
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
uint32_t __thandor_eax_preserve_ecx_edx FixedMath_UInt64Sqrt(UInt64Half32 high,UInt64Half32 low)

{
  uint8_t initialRootShift;
  uint32_t rootEstimate;
  uint32_t refinedRootEstimate;
  uint32_t secondRootEstimate;
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
    initialRootShift = (uint8_t)(lowHighestSetBitIndex + 1U >> 1);
  }
  else {
    initialRootShift = (uint8_t)(highestSetBitIndex + 0x21U >> 1);
  }
  rootEstimate = 1 << (initialRootShift & 0x1f);
  /* unsigned DIV of EDX:EAX = high:low */
  refinedRootEstimate =
       rootEstimate + (int)(((uint64_t)high << 0x20 | (uint64_t)low) / (uint64_t)rootEstimate) >> 1;
  secondRootEstimate =
       refinedRootEstimate + (int)(((uint64_t)high << 0x20 | (uint64_t)low) / (uint64_t)refinedRootEstimate) >> 1;
  return (int)(((uint64_t)high << 0x20 | (uint64_t)low) / (uint64_t)secondRootEstimate) + secondRootEstimate >> 1;
}


/* Not in the original: the original executable carries these tables precomputed (0x004246A0,
   98304 dwords). They are one sine over 1.5 turns, from a quarter turn before angle 0:
   g_FixedSinBeforeZeroQ28 (angles -16384..-1), g_FixedSinQ28 (0..16383) and g_FixedCosQ28
   (cos(i) = sin(i + quarter turn)); lookups with signed or full-turn angles run on from one
   table into the next.
   Entry i is sin(i * 2pi / 65536) in Q28, rounded half up, computed with pi = 3.141592654; this
   reproduces every entry of the original. Called once at startup. */
static int32_t FixedMath_SineTableEntry(int index)
{
  return (int32_t)floor(sin(index * (3.141592654 / 32768.0)) * 268435456.0 + 0.5);
}

void FixedMath_BuildSinCosTables(void)
{
  int index;

  for (index = 0; index < 16384; index++) {
    g_FixedSinBeforeZeroQ28[index] = FixedMath_SineTableEntry(index - 16384);
    g_FixedSinQ28[index] = FixedMath_SineTableEntry(index);
  }
  for (index = 0; index < 65536; index++) {
    g_FixedCosQ28[index] = FixedMath_SineTableEntry(index + 16384);
  }
}

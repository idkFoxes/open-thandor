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
   Composes two orientations given as angle triples: builds the rotation basis of each (basis angles into
   g_ModelTransformScratchMatrix, input angles into the input scratch), multiplies them and extracts the angles
   of the product again. Used by the army movement code to add a local rotation to a heading.
   Returns EAX = azimuth, EBX = elevation, EDX = roll of the composed rotation.
*/
FixedEulerAnglesEaxEbxEdx12 FixedTransform_ComposeEulerAnglesRegs
          (AngleTurn32 inputAngle0,AngleTurn32 inputAngle1,AngleTurn32 inputAngle2,
          AngleTurn32 basisAngle0,AngleTurn32 basisAngle1,AngleTurn32 basisAngle2)

{
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
  /* extracted EAX = roll, ECX = azimuth, EDX = elevation, rotated into EDX, EAX, EBX */
  composedAngles.angle2 = extractedAngles.eaxAngle;
  composedAngles.angle0 = (int)THANDOR_PART(uint64_t, extractedAngles, 4);
  composedAngles.angle1 = (int)((uint64_t)THANDOR_PART(uint64_t, extractedAngles, 4) >> 32);
  return composedAngles;
}


/* Address: 0x00484930.
   Converts the vector (x, y, z) into its length and two 16-bit angles: the elevation of x over the (y, z) plane
   and the azimuth within that plane. Returned in registers: EAX = length, EDX = elevation, ECX = azimuth (low
   16 bits); squares are summed in 64 bits so Q12 components cannot overflow.
*/
FixedLengthAnglesEaxEcxEdx12
FixedMath_VectorToAnglesAndLength3Regs
          (FixedMathVectorComponent32 x,FixedMathVectorComponent32 y,FixedMathVectorComponent32 z)

{
  uint32_t planeLength;
  uint32_t elevationAngle;
  uint32_t azimuthAngle;
  uint32_t vectorLengthQ12;
  FixedLengthAnglesEaxEcxEdx12 lengthAnglesResult;
  int64_t planeSquaredLengthQ24;
  int64_t totalSquaredLengthQ24;

  planeSquaredLengthQ24 = (int64_t)y * (int64_t)y + (int64_t)z * (int64_t)z;
  planeLength =
       FixedMath_UInt64Sqrt
                 ((UInt64Half32)((uint64_t)planeSquaredLengthQ24 >> 32),
                  (UInt64Half32)planeSquaredLengthQ24);
  elevationAngle = FixedMath_Atan2Angle16(x,planeLength);
  azimuthAngle = FixedMath_Atan2Angle16(y,z);
  totalSquaredLengthQ24 = planeSquaredLengthQ24 + (int64_t)x * (int64_t)x;
  vectorLengthQ12 =
       FixedMath_UInt64Sqrt
                 ((UInt64Half32)((uint64_t)totalSquaredLengthQ24 >> 32),
                  (UInt64Half32)totalSquaredLengthQ24);
  lengthAnglesResult.elevationAngle = elevationAngle;
  lengthAnglesResult.lengthQ12 = vectorLengthQ12;
  lengthAnglesResult.azimuthAngle = azimuthAngle & 0xffff;
  return lengthAnglesResult;
}


/* Address: 0x00484A10.
   Converts a vector into its length and two 16-bit angles: the elevation of z over the (x, y) plane and the
   azimuth atan2(y, x) within it (the component roles differ from FixedMath_VectorToAnglesAndLength3Regs).
   Returns EAX = length, EDX = elevation, ECX = azimuth (low 16 bits); squares are summed in 64 bits.
   Used by the shot maintenance code for ballistic angles.
*/
FixedLengthAnglesEaxEcxEdx12 FixedMath_VectorToAnglesAndLengthVec3Regs(GraphicsFixedVec3 *vector)

{
  uint32_t horizontalLengthQ12;
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
  horizontalLengthQ12 =
       FixedMath_UInt64Sqrt
                 ((UInt64Half32)((uint64_t)squaredLengthAccumulatorQ24 >> 32),
                  (UInt64Half32)squaredLengthAccumulatorQ24);
  elevationAngleResult = FixedMath_Atan2Angle16(vector->z,horizontalLengthQ12);
  azimuthAngle16 = FixedMath_Atan2Angle16(y,x);
  totalSquaredLengthQ24 = squaredLengthAccumulatorQ24 + (int64_t)inputZQ12 * (int64_t)inputZQ12;
  vectorLengthQ12 =
       FixedMath_UInt64Sqrt
                 ((UInt64Half32)((uint64_t)totalSquaredLengthQ24 >> 32),
                  (UInt64Half32)totalSquaredLengthQ24);
  lengthAnglesResult.elevationAngle = elevationAngleResult;
  lengthAnglesResult.lengthQ12 = vectorLengthQ12;
  lengthAnglesResult.azimuthAngle = azimuthAngle16 & 0xffff;
  return lengthAnglesResult;
}


/* Address: 0x00484B70.
   Angle and length of a 2D vector: EDX = atan2(component0, component1) as a 16-bit angle (65536 = full
   turn), EAX = floor(sqrt(component0^2 + component1^2)). Used by the army movement and combat code and the
   shot catalog for planar headings and distances.
*/
FixedLengthAngleEaxEdx8 FixedMath_Vector2AngleAndLengthRegs
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
   Rotates the Q12 vector (x, y, z) by the rotation basis built from three angles and returns it in
   EAX = x, ECX = y, EDX = z. The parameters are in the original's stack order (z first). Works through the
   shared model-transform scratch globals; used by the model hierarchy for view-relative vectors.
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
   Moves the local translation of the model node at vectorState along its own direction:
   vector -= direction(vector) * directionScale * stepMultiplier. The army movement code uses it for weapon
   attachment nodes with the weapon's backward-step scale and -elapsedTicks, which moves the offset outward
   along its direction.
*/
void FixedVector_StepBackwardAlongOwnDirection
          (FixedVectorStepMultiplier32 stepMultiplier,FixedMathScale32 directionScale,
          FixedVectorStateAddress32 vectorState)

{
  FixedDirection stepDirection;
  FixedElevationAzimuth vectorAngles;

  vectorAngles = FixedMath_VectorToAnglesVec3Regs((GraphicsFixedVec3 *)&((ModelRuntimeNode *)vectorState)->modelPayload.localTranslationXQ12);
  stepDirection = FixedMath_DirectionFromAnglesScaledRegs(vectorAngles.elevationAngle,vectorAngles.azimuthAngle,directionScale);
  ((ModelRuntimeNode *)vectorState)->modelPayload.localTranslationXQ12 = ((ModelRuntimeNode *)vectorState)->modelPayload.localTranslationXQ12 - stepDirection.x * stepMultiplier;
  ((ModelRuntimeNode *)vectorState)->modelPayload.localTranslationYQ12 = ((ModelRuntimeNode *)vectorState)->modelPayload.localTranslationYQ12 - stepDirection.y * stepMultiplier;
  ((ModelRuntimeNode *)vectorState)->modelPayload.localTranslationZQ12 = ((ModelRuntimeNode *)vectorState)->modelPayload.localTranslationZQ12 - stepDirection.z * stepMultiplier;
}


/* Address: 0x00521FA0.
   Two-bone joint solver for the leg suspension: for a triangle with sides s0, s1 and base s2 it returns
   EAX = the angle between s2 and s1 and EDX = that angle plus the one between s2 and s0 (the bend at the
   joint of s0 and s1). The height over the base comes from 64-bit sums of squares; when the triangle cannot
   close or the base is at most 0x10, both angles are 0 when s0 < s2 (unsigned) and 0x8000 (half turn) otherwise.
*/
FixedTriangleJointAnglesEaxEdx8 FixedGeometry_SolveTriangleJointAnglesRegs(Q12 sideLength0Q12,Q12 sideLength1Q12,Q12 sideLength2Q12)

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
  /* 64-bit (EBX:ECX) -p^2 - s2^2 + 2*s1^2 + 2*s0^2, in the original's order; with p = (s0^2 - s1^2) / s2
     this is (2 * height)^2, so the square root is halved below like the two base projections */
  projectionOrHeightSquared =
       (((0 - projectionOrHeightSquared) - side2Squared) + side1Squared * 2) + side0Squared * 2;
  if ((-1 < projectionOrHeightSquared) && (0x10 < sideLength2Q12)) {
    triangleHeight = FixedMath_UInt64Sqrt((UInt64Half32)((uint64_t)projectionOrHeightSquared >> 32),(UInt64Half32)projectionOrHeightSquared);
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
   Returns the length floor(sqrt(x*x + y*y + z*z)) of a 3D vector; the squares are summed in 64 bits so Q12
   world coordinates cannot overflow, and the result has the same fixed-point scale as the components.
*/
uint32_t FixedMath_Length3(FixedMathVectorComponent32 x,FixedMathVectorComponent32 y,
                 FixedMathVectorComponent32 z)

{
  int64_t squaredLengthQ24;

  squaredLengthQ24 =
       (int64_t)y * (int64_t)y + (int64_t)z * (int64_t)z + (int64_t)x * (int64_t)x;
  return FixedMath_UInt64Sqrt((UInt64Half32)((uint64_t)squaredLengthQ24 >> 32),
                              (UInt64Half32)squaredLengthQ24);
}


/* Address: 0x00484E50.
   Returns the direction angles of the transform's third basis column (row0[2], row1[2], row2[2]):
   EDX = elevation of row2[2] over the other two, ECX = azimuth atan2(row1[2], row0[2]). Same as the first
   step of FixedTransform_ExtractEulerAnglesRegs. No caller, function-pointer table or data reference to
   0x00484E50 was found in the port or the image data.
*/
FixedVectorAngles FixedTransform_ExtractForwardAnglesRegs(GraphicsFixedMatrix3x4 *transform)

{
  FixedVectorAngles forwardAngles;
  
  forwardAngles = FixedMath_VectorToAngles3Regs
                    (transform->basisRow2[2],transform->basisRow1[2],transform->basisRow0[2]);
  return forwardAngles;
}


/* Address: 0x004857A0.
   Writes input / |input| as a Q28 unit vector (output may alias input). Each component is multiplied by
   2^32 / length (unsigned 64/32 DIV) and shifted right by 4. Vectors shorter than 2 give {0, 0, 0}.
   Used to normalize the frustum plane normals and model light directions.
*/
void FixedVec3_NormalizeQ28(GraphicsFixedVec3 *output,GraphicsFixedVec3 *input)

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
    /* SHLD EDX,EAX,28: bits 4..35 of each product */
    normalizedComponentProduct = (int64_t)reciprocalLengthScaleQ32 * (int64_t)input->x;
    output->x = (int)((uint64_t)normalizedComponentProduct >> 32) << 28 |
                (uint32_t)normalizedComponentProduct >> 4;
    currentNormalizedComponentProduct = (int64_t)reciprocalLengthScaleQ32 * (int64_t)input->y;
    output->y = (int)((uint64_t)currentNormalizedComponentProduct >> 32) << 28 |
                (uint32_t)currentNormalizedComponentProduct >> 4;
    finalNormalizedComponentProduct = (int64_t)reciprocalLengthScaleQ32 * (int64_t)input->z;
    output->z = (int)((uint64_t)finalNormalizedComponentProduct >> 32) << 28 |
                (uint32_t)finalNormalizedComponentProduct >> 4;
  }
}


/* Address: 0x004BEC20.
   Wrapper around FixedTransform_RotateDirectionScaledCoreRegs that returns the rotated direction in
   EAX = x, ECX = y, EDX = z instead of the core's EAX/EBX/EDX (MOV ECX,EBX), keeping EBX. Used by the model
   hierarchy.
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
   Writes the Q28 unit direction for an elevation and an azimuth angle (16-bit turns, 65536 = full circle):
   x = cos(az)cos(el), y = sin(az)cos(el), z = sin(el). The products are formed with the sum-to-product
   identities, e.g. cos(az)cos(el) = (cos(az+el) + cos(az-el)) / 2, so only table lookups are needed.
*/
void FixedMath_WriteDirectionQ28(GraphicsFixedVec3 *output,AngleTurn32 elevationAngle,AngleTurn32 azimuthAngle)

{
  uint32_t azimuthPlusElevationAngle16;
  uint32_t elevationAngle16;
  uint32_t azimuthMinusElevationAngle16;
  int32_t elevationSinQ28;
  int azimuthPlusElevationSinQ28;
  int azimuthMinusElevationSinQ28;

  elevationAngle16 = elevationAngle & 0xffff;
  elevationSinQ28 = g_FixedSinQ28[elevationAngle16];
  azimuthPlusElevationAngle16 = elevationAngle16 + azimuthAngle & 0xffff;
  azimuthMinusElevationAngle16 = azimuthAngle - elevationAngle16 & 0xffff;
  azimuthPlusElevationSinQ28 = g_FixedSinQ28[azimuthPlusElevationAngle16];
  azimuthMinusElevationSinQ28 = g_FixedSinQ28[azimuthMinusElevationAngle16];
  output->x = g_FixedCosQ28[azimuthPlusElevationAngle16] + g_FixedCosQ28[azimuthMinusElevationAngle16] >> 1;
  output->y = azimuthPlusElevationSinQ28 + azimuthMinusElevationSinQ28 >> 1;
  output->z = elevationSinQ28;
}


/* Address: 0x00484B00.
   Returns cos(angle) * scale in EAX and sin(angle) * scale in EDX for a 16-bit angle (65536 = full turn),
   using the Q28 tables, so the results keep the scale's fixed-point format. Used for terrain direction
   records and by the rotation basis builder.
*/
FixedSinCosEdxEax8 FixedMath_SinCosScaled(AngleTurn32 angle,FixedMathScale32 scale)

{
  uint32_t sinScaled;
  uint32_t cosScaled;

  /* SHLD by 4 of the 64-bit products: bits 28..59, i.e. the Q28 factor is divided out */
  sinScaled = (uint32_t)((int64_t)g_FixedSinQ28[angle & 0xffff] * (int64_t)scale >> 28);
  cosScaled = (uint32_t)((int64_t)g_FixedCosQ28[angle & 0xffff] * (int64_t)scale >> 28);
  return (uint64_t)sinScaled << 32 | (uint64_t)cosScaled; /* EDX = sin, EAX = cos */
}


/* Address: 0x00484B40.
   Table lookup of a 16-bit angle (65536 = full turn): returns EAX = cos(angle) and EDX = sin(angle) in Q28.
   Used by the graphics projection setup (g_ProjectionAngleFactors).
*/
FixedSinCosEdxEax8 FixedMath_SinCosQ28(AngleTurn32 angle)

{
  return (uint64_t)(uint32_t)g_FixedSinQ28[angle & 0xffff] << 32 | (uint64_t)(uint32_t)g_FixedCosQ28[angle & 0xffff];
}

/* Address: 0x00484F10.
   Rotates a direction by the transform's 3x3 Q28 basis (output = basis * direction, each dot product summed
   in 64 bits and shifted right by 28); the translation is ignored, so the direction keeps its scale.
   Used by the model lighting to rotate surface normals.
*/
void FixedTransform_ApplyDirection
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
  output->x = (int)((uint64_t)basisDotProductAccumulatorQ40 >> 32) << 4 |
              (uint32_t)basisDotProductAccumulatorQ40 >> 28;
  currentBasisDotProductQ40 =
       (int64_t)transform->basisRow1[1] * (int64_t)direction->y +
       (int64_t)currentBasisRowComponent0Q28 * (int64_t)direction->x +
       (int64_t)transform->basisRow1[2] * (int64_t)direction->z;
  basisRow2Component0Q28 = transform->basisRow2[0];
  output->y = (int)((uint64_t)currentBasisDotProductQ40 >> 32) << 4 |
              (uint32_t)currentBasisDotProductQ40 >> 28;
  finalBasisDotProductQ40 = (int64_t)transform->basisRow2[1] * (int64_t)direction->y +
          (int64_t)basisRow2Component0Q28 * (int64_t)direction->x +
          (int64_t)transform->basisRow2[2] * (int64_t)direction->z;
  output->z = (int)((uint64_t)finalBasisDotProductQ40 >> 32) << 4 | (uint32_t)finalBasisDotProductQ40 >> 28;
}


/* Address: 0x00485090.
   Multiplies a direction by the transpose of the transform's 3x3 Q28 basis (for a rotation this is the inverse
   rotation, i.e. world to local), summing in 64 bits and shifting right by 28; the translation is ignored.
   Used by ModelRender_PrepareViewDirections to bring the view and auxiliary directions into model space.
*/
void FixedTransform_ApplyTransposeDirection
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
  output->x = (int)((uint64_t)basisDotProductAccumulatorQ40 >> 32) << 4 |
              (uint32_t)basisDotProductAccumulatorQ40 >> 28;
  currentBasisDotProductQ40 =
       (int64_t)transform->basisRow1[1] * (int64_t)direction->y +
       (int64_t)currentBasisColumnRow0ComponentQ28 * (int64_t)direction->x +
       (int64_t)transform->basisRow2[1] * (int64_t)direction->z;
  basisRow0Component2Q28 = transform->basisRow0[2];
  output->y = (int)((uint64_t)currentBasisDotProductQ40 >> 32) << 4 |
              (uint32_t)currentBasisDotProductQ40 >> 28;
  finalBasisDotProductQ40 = (int64_t)transform->basisRow1[2] * (int64_t)direction->y +
          (int64_t)basisRow0Component2Q28 * (int64_t)direction->x +
          (int64_t)transform->basisRow2[2] * (int64_t)direction->z;
  output->z = (int)((uint64_t)finalBasisDotProductQ40 >> 32) << 4 | (uint32_t)finalBasisDotProductQ40 >> 28;
}


/* Address: 0x00485520.
   Inverts a rigid Q28 transform: the output basis is the adjugate of the input basis (each cofactor a 64-bit
   difference of products shifted right by 28), which is the inverse because a rotation has determinant 1,
   and the output translation is -(outputBasis * inputTranslation). Used for the view transform and the
   leg suspension.
*/
void FixedTransform_InvertRigidQ28(GraphicsFixedMatrix3x4 *output,GraphicsFixedMatrix3x4 *input)

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
  output->basisRow0[0] = (int)((uint64_t)cofactorOrTranslationProduct >> 32) << 4 | (uint32_t)cofactorOrTranslationProduct >> 28;
  cofactorOrTranslationProduct = (int64_t)componentOrProductLow * (int64_t)input->basisRow2[0] -
          (int64_t)input->basisRow1[0] * (int64_t)input->basisRow2[2];
  componentOrProductLow = input->basisRow1[0];
  output->basisRow1[0] = (int)((uint64_t)cofactorOrTranslationProduct >> 32) << 4 | (uint32_t)cofactorOrTranslationProduct >> 28;
  cofactorOrTranslationProduct = (int64_t)componentOrProductLow * (int64_t)input->basisRow2[1] -
          (int64_t)input->basisRow1[1] * (int64_t)input->basisRow2[0];
  componentOrProductLow = input->basisRow0[2];
  output->basisRow2[0] = (int)((uint64_t)cofactorOrTranslationProduct >> 32) << 4 | (uint32_t)cofactorOrTranslationProduct >> 28;
  cofactorOrTranslationProduct = (int64_t)componentOrProductLow * (int64_t)input->basisRow2[1] -
          (int64_t)input->basisRow0[1] * (int64_t)input->basisRow2[2];
  componentOrProductLow = input->basisRow0[0];
  output->basisRow0[1] = (int)((uint64_t)cofactorOrTranslationProduct >> 32) << 4 | (uint32_t)cofactorOrTranslationProduct >> 28;
  cofactorOrTranslationProduct = (int64_t)componentOrProductLow * (int64_t)input->basisRow2[2] -
          (int64_t)input->basisRow0[2] * (int64_t)input->basisRow2[0];
  componentOrProductLow = input->basisRow0[1];
  output->basisRow1[1] = (int)((uint64_t)cofactorOrTranslationProduct >> 32) << 4 | (uint32_t)cofactorOrTranslationProduct >> 28;
  cofactorOrTranslationProduct = (int64_t)componentOrProductLow * (int64_t)input->basisRow2[0] -
          (int64_t)input->basisRow0[0] * (int64_t)input->basisRow2[1];
  componentOrProductLow = input->basisRow0[1];
  output->basisRow2[1] = (int)((uint64_t)cofactorOrTranslationProduct >> 32) << 4 | (uint32_t)cofactorOrTranslationProduct >> 28;
  cofactorOrTranslationProduct = (int64_t)componentOrProductLow * (int64_t)input->basisRow1[2] -
          (int64_t)input->basisRow0[2] * (int64_t)input->basisRow1[1];
  componentOrProductLow = input->basisRow0[2];
  output->basisRow0[2] = (int)((uint64_t)cofactorOrTranslationProduct >> 32) << 4 | (uint32_t)cofactorOrTranslationProduct >> 28;
  cofactorOrTranslationProduct = (int64_t)componentOrProductLow * (int64_t)input->basisRow1[0] -
          (int64_t)input->basisRow0[0] * (int64_t)input->basisRow1[2];
  componentOrProductLow = input->basisRow0[0];
  output->basisRow1[2] = (int)((uint64_t)cofactorOrTranslationProduct >> 32) << 4 | (uint32_t)cofactorOrTranslationProduct >> 28;
  cofactorOrTranslationProduct = (int64_t)componentOrProductLow * (int64_t)input->basisRow1[1] -
          (int64_t)input->basisRow0[1] * (int64_t)input->basisRow1[0];
  output->basisRow2[2] = (int)((uint64_t)cofactorOrTranslationProduct >> 32) << 4 | (uint32_t)cofactorOrTranslationProduct >> 28;
  /* Each translation component is the 64-bit 0 - tx*r0 - ty*r1 - tz*r2 (XOR/SUB/SBB chain in the original),
     written out below as low halves with explicit borrows, then shifted right by 28 (high * 0x10 | low >> 28). */
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
}


/* Address: 0x004856B0.
   Dot product of two Q12 vectors, summed in 64 bits and shifted right by 12 (SHRD), so the result is Q12.
   Used by the model renderer for back-face and light-facing tests.
*/
int32_t FixedVec3_DotQ12(GraphicsFixedVec3 *left,GraphicsFixedVec3 *right)

{
  int64_t dotProductAccumulatorQ24;

  dotProductAccumulatorQ24 =
       (int64_t)right->y * (int64_t)left->y + (int64_t)right->x * (int64_t)left->x +
       (int64_t)right->z * (int64_t)left->z;
  return (uint32_t)dotProductAccumulatorQ24 >> 12 |
         (int)((uint64_t)dotProductAccumulatorQ24 >> 32) << 20;
}


/* Address: 0x004856F0.
   Dot product summed in 64 bits and shifted right by 28: with one Q28 unit vector (a frustum plane normal or
   a direction) the result keeps the other vector's scale. Used for frustum culling and effect motion.
*/
int32_t FixedVec3_DotQ28(GraphicsFixedVec3 *left,GraphicsFixedVec3 *right)

{
  int64_t dotProductAccumulatorQ56;

  dotProductAccumulatorQ56 =
       (int64_t)right->y * (int64_t)left->y + (int64_t)right->x * (int64_t)left->x +
       (int64_t)right->z * (int64_t)left->z;
  return (uint32_t)dotProductAccumulatorQ56 >> 28 |
         (int)((uint64_t)dotProductAccumulatorQ56 >> 32) << 4;
}


/* Address: 0x00485730.
   Writes leftOperand x rightOperand (cross product of two Q12 vectors, 64-bit differences shifted right by 12).
   The parameters are in the original's stack order: output, rightOperand, leftOperand. Used to build the
   frustum plane normals from the corner rays.
*/
void FixedVec3_CrossQ12(GraphicsFixedVec3 *output,GraphicsFixedVec3 *rightOperand,
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
  output->x = (int)((uint64_t)crossComponentProductDifferenceQ24 >> 32) << 20 |
              (uint32_t)crossComponentProductDifferenceQ24 >> 12;
  currentCrossProductDifferenceQ24 =
       (int64_t)currentLeftComponentQ12 * (int64_t)rightOperand->x -
       (int64_t)leftOperand->x * (int64_t)rightOperand->z;
  leftXQ12 = leftOperand->x;
  output->y = (int)((uint64_t)currentCrossProductDifferenceQ24 >> 32) << 20 |
              (uint32_t)currentCrossProductDifferenceQ24 >> 12;
  finalCrossProductDifferenceQ24 = (int64_t)leftXQ12 * (int64_t)rightOperand->y -
          (int64_t)leftOperand->y * (int64_t)rightOperand->x;
  output->z = (int)((uint64_t)finalCrossProductDifferenceQ24 >> 32) << 20 | (uint32_t)finalCrossProductDifferenceQ24 >> 12;
}


/* Address: 0x0052AD50.
   Moves a planar point by distance in the direction of a 16-bit angle: returns EAX = baseX + cos(angle) *
   distance and EDX = baseY + sin(angle) * distance (Q28 table products shifted right by 28). Used by the
   army movement code to step a unit along its heading.
*/
FixedPlanarPointEdxEax8
FixedTrig_ProjectPlanarPointRegs(Q12 distance,AngleTurn32 angle16,Q12 baseY,Q12 baseX)

{
  uint32_t pointX;
  uint32_t pointY;

  /* SHLD EDX,EAX,4 of the 64-bit products: bits 28..59 */
  pointX = baseX + (uint32_t)((int64_t)g_FixedCosQ28[angle16 & 0xffff] * (int64_t)distance >> 28);
  pointY = (uint32_t)((int64_t)g_FixedSinQ28[angle16 & 0xffff] * (int64_t)distance >> 28) + baseY;
  return (uint64_t)pointY << 32 | (uint64_t)pointX; /* EDX = y, EAX = x */
}

/* Address: 0x004BEC50.
   Builds the direction of (elevationAngle, azimuthAngle) with length directionScale, rotates it by the
   rotation basis of the three rotation angles and returns it in EAX = x, EBX = y, EDX = z. Works through the
   shared model-transform scratch globals; called only by FixedTransform_RotateDirectionScaledRegs.
*/
FixedVectorXEaxYEbxZEdx12 FixedTransform_RotateDirectionScaledCoreRegs
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
   Direction angles of a vector without its length: elevation = atan2(z, |(x, y)|) and azimuth = atan2(y, x)
   as 16-bit angles (the horizontal length is summed in 64 bits). Used by the army suspension, the graphics
   direction setup and FixedVector_StepBackwardAlongOwnDirection.
*/
FixedElevationAzimuth FixedMath_VectorToAnglesVec3Regs(GraphicsFixedVec3 *vector)

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
                    ((UInt64Half32)((uint64_t)horizontalSquaredLengthAccumulatorQ24 >> 32),
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
   Inverse of FixedTransform_BuildRotationBasis: recovers the angles from a rotation basis, elevation
   (EDX) and azimuth (ECX) from the third column and the roll (EAX) from the upper 2x2 block. Used to turn
   a composed suspension rotation back into a model node's local angles.
*/
FixedEulerAnglesEaxEcxEdx12 FixedTransform_ExtractEulerAnglesRegs(GraphicsFixedMatrix3x4 *transform)

{
  uint32_t rollAngle16;
  uint32_t rollMinusTwoAzimuth16;
  FixedVectorAngles forwardAngles;
  FixedEulerAnglesEaxEcxEdx12 eulerAngles;

  forwardAngles = FixedMath_VectorToAngles3Regs
                    (transform->basisRow2[2],transform->basisRow1[2],transform->basisRow0[2]);
  eulerAngles.edxAngle = forwardAngles.elevationAngle;
  eulerAngles.ecxAngle = forwardAngles.azimuthAngle;
  if ((int)forwardAngles.elevationAngle < 0) { /* TEST EDX,EDX: elevation sign */
    /* for a negative elevation the 2x2 block yields roll - 2 * azimuth */
    rollMinusTwoAzimuth16 = FixedMath_Atan2Angle16
                      (transform->basisRow1[0] + transform->basisRow0[1],
                       transform->basisRow1[1] - transform->basisRow0[0]);
    rollAngle16 = rollMinusTwoAzimuth16 + eulerAngles.ecxAngle * 2 & 0xffff;
  }
  else {
    rollAngle16 =
         FixedMath_Atan2Angle16
                   (transform->basisRow1[0] - transform->basisRow0[1],
                    transform->basisRow1[1] + transform->basisRow0[0]);
  }
  eulerAngles.eaxAngle = rollAngle16;
  return eulerAngles;
}


/* Address: 0x00484AC0.
   Length of a 3D vector: floor(sqrt(x*x + y*y + z*z)), with the squares summed in 64 bits so Q12 components
   cannot overflow. The result has the components' fixed-point scale.
*/
uint32_t FixedMath_LengthVec3(GraphicsFixedVec3 *vector)

{
  uint32_t vectorLengthQ12;
  int64_t squaredLengthQ24;

  squaredLengthQ24 =
       (int64_t)vector->y * (int64_t)vector->y + (int64_t)vector->x * (int64_t)vector->x +
       (int64_t)vector->z * (int64_t)vector->z;
  vectorLengthQ12 =
       FixedMath_UInt64Sqrt
                 ((UInt64Half32)((uint64_t)squaredLengthQ24 >> 32),(UInt64Half32)squaredLengthQ24);
  return vectorLengthQ12;
}


/* Address: 0x00484CF0.
   Length of the 2D vector (x, y): floor(sqrt(x*x + y*y)), with the squares summed in 64 bits so Q12
   components cannot overflow. The result has the components' fixed-point scale.
*/
uint32_t FixedMath_Length2(FixedMathVectorComponent32 x,FixedMathVectorComponent32 y)

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
   Returns the direction of an elevation and an azimuth angle (16-bit turns) multiplied by scale, in EAX (x),
   ECX (y) and EDX (z): x = cos(az)cos(el) * scale, y = sin(az)cos(el) * scale, z = sin(el) * scale. The shot
   creator uses it to seed projectile velocities. As in FixedMath_WriteDirectionQ28 the products come from
   sum-to-product identities; the halving is folded into the shift (29 instead of 28).
*/
FixedDirection
FixedMath_DirectionFromAnglesScaledRegs
          (AngleTurn32 elevationAngle,AngleTurn32 azimuthAngle,FixedMathScale32 scale)

{
  int64_t scaledYProduct;
  uint32_t azimuthPlusElevationAngle16;
  uint32_t elevationAngle16;
  uint32_t azimuthMinusElevationAngle16;
  FixedDirection scaledDirection;
  int64_t scaledXProduct;

  elevationAngle16 = elevationAngle & 0xffff;
  azimuthPlusElevationAngle16 = elevationAngle16 + azimuthAngle & 0xffff;
  azimuthMinusElevationAngle16 = azimuthAngle - elevationAngle16 & 0xffff;
  scaledXProduct =
       (int64_t)(g_FixedCosQ28[azimuthPlusElevationAngle16] + g_FixedCosQ28[azimuthMinusElevationAngle16]) *
       (int64_t)scale;
  scaledYProduct =
       (int64_t)(g_FixedSinQ28[azimuthPlusElevationAngle16] + g_FixedSinQ28[azimuthMinusElevationAngle16]) *
       (int64_t)scale;
  scaledDirection.z = (int)((uint64_t)((int64_t)g_FixedSinQ28[elevationAngle16] * (int64_t)scale) >> 32
                   ) << 4 |
              (uint32_t)((int64_t)g_FixedSinQ28[elevationAngle16] * (int64_t)scale) >> 28;
  scaledDirection.y = (int)((uint64_t)scaledYProduct >> 32) << 3 | (uint32_t)scaledYProduct >> 29;
  scaledDirection.x = (int)((uint64_t)scaledXProduct >> 32) << 3 | (uint32_t)scaledXProduct >> 29;
  return scaledDirection;
}


/* Address: 0x004847E0.
   Returns the Q28 unit direction of an elevation and an azimuth angle (16-bit turns) in EAX (x), ECX (y) and
   EDX (z): x = cos(az)cos(el), y = sin(az)cos(el), z = sin(el), formed like FixedMath_WriteDirectionQ28.
   The rotation-basis builder uses it for its rows.
*/
FixedDirection
FixedMath_DirectionFromAnglesQ28Regs(AngleTurn32 elevationAngle,AngleTurn32 azimuthAngle)

{
  uint32_t azimuthPlusElevationAngle16;
  uint32_t elevationAngle16;
  uint32_t azimuthMinusElevationAngle16;
  FixedDirection directionQ28;

  elevationAngle16 = elevationAngle & 0xffff;
  azimuthPlusElevationAngle16 = elevationAngle16 + azimuthAngle & 0xffff;
  azimuthMinusElevationAngle16 = azimuthAngle - elevationAngle16 & 0xffff;
  directionQ28.x = g_FixedCosQ28[azimuthPlusElevationAngle16] + g_FixedCosQ28[azimuthMinusElevationAngle16] >> 1;
  directionQ28.y = g_FixedSinQ28[azimuthPlusElevationAngle16] + g_FixedSinQ28[azimuthMinusElevationAngle16] >> 1;
  directionQ28.z = g_FixedSinQ28[elevationAngle16];
  return directionQ28;
}


/* Address: 0x00484840.
   Scaled form of FixedMath_WriteDirectionQ28: writes {cos(el)cos(az), cos(el)sin(az), sin(el)} * scale, so the
   output has the scale's fixed-point format. x and y use the sum-to-product sums (twice the value), hence the
   shift by 29 instead of 28. Used for the frustum corner rays and by FixedTransform_RotateDirectionScaledCoreRegs.
*/
void FixedMath_WriteDirectionScaled(GraphicsFixedVec3 *output,AngleTurn32 elevationAngle,AngleTurn32 azimuthAngle,
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
  output->x = (int)((uint64_t)horizontalComponentScaleProduct >> 32) << 3 |
              (uint32_t)horizontalComponentScaleProduct >> 29;
  yComponentScaleProduct =
       (int64_t)(azimuthPlusElevationSinQ28 + azimuthMinusElevationSinQ28) * (int64_t)scale;
  output->y = (int)((uint64_t)yComponentScaleProduct >> 32) << 3 |
              (uint32_t)yComponentScaleProduct >> 29;
  output->z = (int)((uint64_t)((int64_t)verticalSinQ28 * (int64_t)scale) >> 32) << 4 |
              (uint32_t)((int64_t)verticalSinQ28 * (int64_t)scale) >> 28;
}


/* Address: 0x00485120.
   Concatenates two rigid transforms: output = outerTransform * innerTransform, i.e. a point is moved by
   innerTransform first and then by outerTransform. Each Q28 basis product is summed in 64 bits and
   shifted back by 28; the inner translation is rotated by the outer basis and the outer translation added.
*/
void FixedTransform_Compose(GraphicsFixedMatrix3x4 *output,GraphicsFixedMatrix3x4 *innerTransform,
          GraphicsFixedMatrix3x4 *outerTransform)

{
  int outerRowX;
  int64_t rowDotProduct;
  int firstOuterRowX;
  int64_t firstRowDotProduct;

  /* outerRowX is the outer row's first coefficient, loaded one output ahead as in the original */
  firstRowDotProduct =
       (int64_t)outerTransform->basisRow0[1] * (int64_t)innerTransform->basisRow1[0] +
       (int64_t)outerTransform->basisRow0[0] * (int64_t)innerTransform->basisRow0[0] +
       (int64_t)outerTransform->basisRow0[2] * (int64_t)innerTransform->basisRow2[0];
  firstOuterRowX = outerTransform->basisRow0[0];
  output->basisRow0[0] =
       (int)((uint64_t)firstRowDotProduct >> 32) << 4 |
       (uint32_t)firstRowDotProduct >> 28;
  rowDotProduct = (int64_t)outerTransform->basisRow0[1] * (int64_t)innerTransform->basisRow1[1] +
          (int64_t)firstOuterRowX * (int64_t)innerTransform->basisRow0[1] +
          (int64_t)outerTransform->basisRow0[2] * (int64_t)innerTransform->basisRow2[1];
  outerRowX = outerTransform->basisRow0[0];
  output->basisRow0[1] = (int)((uint64_t)rowDotProduct >> 32) << 4 | (uint32_t)rowDotProduct >> 28;
  rowDotProduct = (int64_t)outerTransform->basisRow0[1] * (int64_t)innerTransform->basisRow1[2] +
          (int64_t)outerRowX * (int64_t)innerTransform->basisRow0[2] +
          (int64_t)outerTransform->basisRow0[2] * (int64_t)innerTransform->basisRow2[2];
  outerRowX = outerTransform->basisRow0[0];
  output->basisRow0[2] = (int)((uint64_t)rowDotProduct >> 32) << 4 | (uint32_t)rowDotProduct >> 28;
  rowDotProduct = (int64_t)outerTransform->basisRow0[1] * (int64_t)(innerTransform->translation).y +
          (int64_t)outerRowX * (int64_t)(innerTransform->translation).x +
          (int64_t)outerTransform->basisRow0[2] * (int64_t)(innerTransform->translation).z;
  outerRowX = outerTransform->basisRow1[0];
  (output->translation).x =
       ((int)((uint64_t)rowDotProduct >> 32) << 4 | (uint32_t)rowDotProduct >> 28) + (outerTransform->translation).x;
  rowDotProduct = (int64_t)outerTransform->basisRow1[1] * (int64_t)innerTransform->basisRow1[0] +
          (int64_t)outerRowX * (int64_t)innerTransform->basisRow0[0] +
          (int64_t)outerTransform->basisRow1[2] * (int64_t)innerTransform->basisRow2[0];
  outerRowX = outerTransform->basisRow1[0];
  output->basisRow1[0] = (int)((uint64_t)rowDotProduct >> 32) << 4 | (uint32_t)rowDotProduct >> 28;
  rowDotProduct = (int64_t)outerTransform->basisRow1[1] * (int64_t)innerTransform->basisRow1[1] +
          (int64_t)outerRowX * (int64_t)innerTransform->basisRow0[1] +
          (int64_t)outerTransform->basisRow1[2] * (int64_t)innerTransform->basisRow2[1];
  outerRowX = outerTransform->basisRow1[0];
  output->basisRow1[1] = (int)((uint64_t)rowDotProduct >> 32) << 4 | (uint32_t)rowDotProduct >> 28;
  rowDotProduct = (int64_t)outerTransform->basisRow1[1] * (int64_t)innerTransform->basisRow1[2] +
          (int64_t)outerRowX * (int64_t)innerTransform->basisRow0[2] +
          (int64_t)outerTransform->basisRow1[2] * (int64_t)innerTransform->basisRow2[2];
  outerRowX = outerTransform->basisRow1[0];
  output->basisRow1[2] = (int)((uint64_t)rowDotProduct >> 32) << 4 | (uint32_t)rowDotProduct >> 28;
  rowDotProduct = (int64_t)outerTransform->basisRow1[1] * (int64_t)(innerTransform->translation).y +
          (int64_t)outerRowX * (int64_t)(innerTransform->translation).x +
          (int64_t)outerTransform->basisRow1[2] * (int64_t)(innerTransform->translation).z;
  outerRowX = outerTransform->basisRow2[0];
  (output->translation).y =
       ((int)((uint64_t)rowDotProduct >> 32) << 4 | (uint32_t)rowDotProduct >> 28) + (outerTransform->translation).y;
  rowDotProduct = (int64_t)outerTransform->basisRow2[1] * (int64_t)innerTransform->basisRow1[0] +
          (int64_t)outerRowX * (int64_t)innerTransform->basisRow0[0] +
          (int64_t)outerTransform->basisRow2[2] * (int64_t)innerTransform->basisRow2[0];
  outerRowX = outerTransform->basisRow2[0];
  output->basisRow2[0] = (int)((uint64_t)rowDotProduct >> 32) << 4 | (uint32_t)rowDotProduct >> 28;
  rowDotProduct = (int64_t)outerTransform->basisRow2[1] * (int64_t)innerTransform->basisRow1[1] +
          (int64_t)outerRowX * (int64_t)innerTransform->basisRow0[1] +
          (int64_t)outerTransform->basisRow2[2] * (int64_t)innerTransform->basisRow2[1];
  outerRowX = outerTransform->basisRow2[0];
  output->basisRow2[1] = (int)((uint64_t)rowDotProduct >> 32) << 4 | (uint32_t)rowDotProduct >> 28;
  rowDotProduct = (int64_t)outerTransform->basisRow2[1] * (int64_t)innerTransform->basisRow1[2] +
          (int64_t)outerRowX * (int64_t)innerTransform->basisRow0[2] +
          (int64_t)outerTransform->basisRow2[2] * (int64_t)innerTransform->basisRow2[2];
  outerRowX = outerTransform->basisRow2[0];
  output->basisRow2[2] = (int)((uint64_t)rowDotProduct >> 32) << 4 | (uint32_t)rowDotProduct >> 28;
  rowDotProduct = (int64_t)outerTransform->basisRow2[1] * (int64_t)(innerTransform->translation).y +
          (int64_t)outerRowX * (int64_t)(innerTransform->translation).x +
          (int64_t)outerTransform->basisRow2[2] * (int64_t)(innerTransform->translation).z;
  (output->translation).z =
       ((int)((uint64_t)rowDotProduct >> 32) << 4 | (uint32_t)rowDotProduct >> 28) + (outerTransform->translation).z;
}


/* Address: 0x00484990.
   Converts a vector, passed in the order z, y, x, into its direction angles (16-bit turns): EDX = elevation
   atan2(z, sqrt(x*x + y*y)) and ECX = azimuth atan2(y, x); EAX is preserved. It is the inverse of
   FixedMath_WriteDirectionQ28 and is used for aiming and view angles.
*/
FixedVectorAngles FixedMath_VectorToAngles3Regs
          (FixedMathVectorComponent32 z,FixedMathVectorComponent32 y,FixedMathVectorComponent32 x)

{
  uint32_t horizontalMagnitudeQ12;
  uint32_t elevationAngle16;
  uint32_t azimuthAngle16;
  int64_t horizontalMagnitudeSquaredQ24;
  FixedVectorAngles vectorAngles;

  horizontalMagnitudeSquaredQ24 = (int64_t)y * (int64_t)y + (int64_t)x * (int64_t)x;
  horizontalMagnitudeQ12 =
       FixedMath_UInt64Sqrt
                 ((UInt64Half32)((uint64_t)horizontalMagnitudeSquaredQ24 >> 32),
                  (UInt64Half32)horizontalMagnitudeSquaredQ24);
  elevationAngle16 = FixedMath_Atan2Angle16(z,horizontalMagnitudeQ12);
  azimuthAngle16 = FixedMath_Atan2Angle16(y,x);
  vectorAngles.azimuthAngle = azimuthAngle16 & 0xffff;
  vectorAngles.elevationAngle = elevationAngle16;
  return vectorAngles;
}


/* Address: 0x00484E70.
   Moves a point through a rigid transform: output = basis * point + translation, where each row is a Q28
   dot product summed in 64 bits and shifted back by 28 (SHLD 4), so the point keeps its own scale.
*/
void FixedTransform_ApplyPoint(GraphicsFixedVec3 *output,GraphicsFixedVec3 *point,GraphicsFixedMatrix3x4 *transform)

{
  int row2FirstCoefficientQ28;
  int64_t rowDotProduct;
  int row1FirstCoefficientQ28;
  int64_t firstRowDotProduct;

  /* each row's first coefficient is loaded one output ahead, as in the original */
  firstRowDotProduct =
       (int64_t)transform->basisRow0[1] * (int64_t)point->y +
       (int64_t)transform->basisRow0[0] * (int64_t)point->x +
       (int64_t)transform->basisRow0[2] * (int64_t)point->z;
  row1FirstCoefficientQ28 = transform->basisRow1[0];
  output->x = ((int)((uint64_t)firstRowDotProduct >> 32) << 4 |
              (uint32_t)firstRowDotProduct >> 28) + (transform->translation).x;
  rowDotProduct = (int64_t)transform->basisRow1[1] * (int64_t)point->y +
          (int64_t)row1FirstCoefficientQ28 * (int64_t)point->x +
          (int64_t)transform->basisRow1[2] * (int64_t)point->z;
  row2FirstCoefficientQ28 = transform->basisRow2[0];
  output->y = ((int)((uint64_t)rowDotProduct >> 32) << 4 | (uint32_t)rowDotProduct >> 28) +
              (transform->translation).y;
  rowDotProduct = (int64_t)transform->basisRow2[1] * (int64_t)point->y +
          (int64_t)row2FirstCoefficientQ28 * (int64_t)point->x +
          (int64_t)transform->basisRow2[2] * (int64_t)point->z;
  output->z = ((int)((uint64_t)rowDotProduct >> 32) << 4 | (uint32_t)rowDotProduct >> 28) +
              (transform->translation).z;
}


/* Address: 0x00484D20.
   Builds the Q28 rotation basis of an orientation given as roll, elevation and azimuth angle16s: the
   third column is the forward direction (elevation, azimuth), the rest follows from the roll about it.
   Only the nine basis coefficients are written; callers set the translation themselves.
*/
void FixedTransform_BuildRotationBasis(GraphicsFixedMatrix3x4 *output,AngleTurn32 rollAngle,AngleTurn32 elevationAngle,
          AngleTurn32 azimuthAngle)

{
  int64_t firstTermTimesSinElevation;
  int firstHalfTrigTermQ28;
  int halfTrigTermQ28;
  uint32_t rollMinusTwoAzimuth16;
  uint32_t rollAngleUnmasked;
  uint32_t rollAngle16;
  FixedDirection directionRow;
  int64_t termTimesSinElevation;
  int sinElevationQ28;
  int64_t secondTermTimesSinElevation;

  directionRow = FixedMath_DirectionFromAnglesQ28Regs(elevationAngle,azimuthAngle);
  output->basisRow0[2] = directionRow.x;
  output->basisRow1[2] = directionRow.y;
  output->basisRow2[2] = directionRow.z;
  directionRow = FixedMath_DirectionFromAnglesQ28Regs(elevationAngle,rollAngle - azimuthAngle);
  rollAngleUnmasked = (rollAngle - azimuthAngle) + azimuthAngle;
  output->basisRow2[1] = directionRow.y;
  output->basisRow2[0] = -directionRow.x;
  rollAngle16 = rollAngleUnmasked & 0xffff;
  rollMinusTwoAzimuth16 = rollAngleUnmasked + azimuthAngle * -2 & 0xffff;
  /* half sums/differences of cos/sin(roll) and cos/sin(roll - 2 * azimuth), i.e. products of the
     roll and azimuth sines and cosines */
  firstHalfTrigTermQ28 =
       g_FixedCosQ28[rollAngle16] - g_FixedCosQ28[rollMinusTwoAzimuth16] >> 1;
  sinElevationQ28 = g_FixedSinQ28[elevationAngle & 0xffff];
  output->basisRow0[0] = firstHalfTrigTermQ28;
  halfTrigTermQ28 = g_FixedCosQ28[rollAngle16] + g_FixedCosQ28[rollMinusTwoAzimuth16] >> 1;
  output->basisRow1[1] =
       ((int)((uint64_t)((int64_t)firstHalfTrigTermQ28 * (int64_t)sinElevationQ28) >> 32
             ) << 4 |
       (uint32_t)((int64_t)firstHalfTrigTermQ28 * (int64_t)sinElevationQ28) >> 28) + halfTrigTermQ28;
  termTimesSinElevation = (int64_t)halfTrigTermQ28 * (int64_t)sinElevationQ28;
  output->basisRow0[0] =
       output->basisRow0[0] +
       ((int)((uint64_t)termTimesSinElevation >> 32) << 4 |
       (uint32_t)termTimesSinElevation >> 28);
  halfTrigTermQ28 = g_FixedSinQ28[rollAngle16] + g_FixedSinQ28[rollMinusTwoAzimuth16] >> 1;
  output->basisRow1[0] = halfTrigTermQ28;
  firstTermTimesSinElevation = (int64_t)halfTrigTermQ28 * (int64_t)sinElevationQ28;
  halfTrigTermQ28 = g_FixedSinQ28[rollMinusTwoAzimuth16] - g_FixedSinQ28[rollAngle16] >> 1;
  secondTermTimesSinElevation = (int64_t)halfTrigTermQ28 * (int64_t)sinElevationQ28;
  output->basisRow0[1] = halfTrigTermQ28 - ((int)((uint64_t)firstTermTimesSinElevation >> 32) << 4 | (uint32_t)firstTermTimesSinElevation >> 28);
  output->basisRow1[0] =
       output->basisRow1[0] -
       ((int)((uint64_t)secondTermTimesSinElevation >> 32) << 4 |
       (uint32_t)secondTermTimesSinElevation >> 28);
}


/* Address: 0x00484BA0.
   atan2(y, x) as an engine angle (1/65536 turns, not masked to 16 bits). The plane is split into
   eighth-turn sectors, rotating (x, y) so the remaining angle is within +-1/16 turn, which an odd
   polynomial in the ratio numerator/denominator approximates. (0, 0) yields 0.
*/
uint32_t FixedMath_Atan2Angle16(FixedMathVectorComponent32 y,FixedMathVectorComponent32 x)

{
  uint32_t angle16Result;
  int denominatorOrRatio; /* the sector's denominator, later the ratio in Q31 */
  uint32_t reducedAngleNumerator;
  int doubledYOrRatioSquared; /* y * 2 for the sector tests, later ratio^2 in Q30 */
  AngleTurn16Stored32 octantBaseAngle16;

  octantBaseAngle16 = 0;
  denominatorOrRatio = x * 2;
  doubledYOrRatioSquared = y * 2;
  if (doubledYOrRatioSquared - x == 0 || doubledYOrRatioSquared < x) {
    if (denominatorOrRatio < -y) {
      if (y < denominatorOrRatio) {
        octantBaseAngle16 = -FIXED_ANGLE16_QUARTER_TURN;
        denominatorOrRatio = -y;
        reducedAngleNumerator = x;
      }
      else {
        reducedAngleNumerator = x - y;
        octantBaseAngle16 = -3 * FIXED_ANGLE16_EIGHTH_TURN;
        denominatorOrRatio = -(x + y);
      }
    }
    else {
      denominatorOrRatio = x;
      reducedAngleNumerator = y;
      if (doubledYOrRatioSquared < -x) {
        reducedAngleNumerator = x + y;
        octantBaseAngle16 = -FIXED_ANGLE16_EIGHTH_TURN;
        denominatorOrRatio = x - y;
      }
    }
  }
  else if (denominatorOrRatio < -y) {
    if (doubledYOrRatioSquared < -x) {
      octantBaseAngle16 = FIXED_ANGLE16_HALF_TURN;
      denominatorOrRatio = -x;
      reducedAngleNumerator = -y;
      if (0 < (int)reducedAngleNumerator) {
        octantBaseAngle16 = -FIXED_ANGLE16_HALF_TURN;
      }
    }
    else {
      denominatorOrRatio = y - x;
      octantBaseAngle16 = 3 * FIXED_ANGLE16_EIGHTH_TURN;
      reducedAngleNumerator = -(x + y);
    }
  }
  else if (y < denominatorOrRatio) {
    denominatorOrRatio = x + y;
    octantBaseAngle16 = FIXED_ANGLE16_EIGHTH_TURN;
    reducedAngleNumerator = y - x;
  }
  else {
    octantBaseAngle16 = FIXED_ANGLE16_QUARTER_TURN;
    reducedAngleNumerator = -x;
    denominatorOrRatio = y;
  }
  angle16Result = 0;
  if (denominatorOrRatio * 2 != 0) {
    denominatorOrRatio = (int)((int64_t)((uint64_t)reducedAngleNumerator << 32) / (int64_t)(denominatorOrRatio * 2));
    doubledYOrRatioSquared = (int)((uint64_t)((int64_t)denominatorOrRatio * (int64_t)denominatorOrRatio) >> 32);
    /* atan(t) in angle units ~ t * (c1 + t^2 * (c3 + t^2 * c5)); 0x517D = 20861 = 2 * 65536 / (2 * pi) */
    angle16Result =
         octantBaseAngle16 +
         (int)((uint64_t)
               ((int64_t)denominatorOrRatio *
               (int64_t)
               ((int)((uint64_t)
                      ((int64_t)doubledYOrRatioSquared *
                      (int64_t)((int)((uint64_t)((int64_t)doubledYOrRatioSquared * 0x104c2) >> 32) + -0x6ca6))
                     >> 32) + 0x517d)) >> 32);
  }
  return angle16Result;
}


/* Address: 0x004846A0.
   Approximate square root of a Q12 value, returned in Q12 (about 0.6% low): the input is shifted left by an
   even amount so its top bit lands on bit 27 or 28 (BSR), a cubic polynomial in the normalized value is
   evaluated with the high halves of IMULs, and the result is shifted right by half the normalizing shift.
   Inputs of 2^29 or more give wrong results (the shift wraps). No caller, function-pointer table or data
   reference to 0x004846A0 was found in the port or the image data.
*/
uint32_t FixedMath_SqrtQ12Approx(uint32_t inputValue)

{
  int highestBitOrNormalized;
  uint32_t normalizeShift;

  highestBitOrNormalized = 31;
  if (inputValue != 0) {
    for (; inputValue >> highestBitOrNormalized == 0; highestBitOrNormalized--) {
    }
  }
  if (inputValue != 0) {
    normalizeShift = 28U - highestBitOrNormalized & 0x1e;
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
   Integer square root of the 64-bit value high:low, used for vector lengths from 64-bit sums of squares.
   The start value is the power of two just above the root (from the highest set bit, BSR); three Newton
   steps x = (x + value / x) / 2 follow, the divisions being 64/32-bit DIVs.
*/
uint32_t FixedMath_UInt64Sqrt(UInt64Half32 high,UInt64Half32 low)

{
  uint8_t initialRootShift;
  uint32_t rootEstimate;
  uint32_t refinedRootEstimate;
  uint32_t secondRootEstimate;
  int lowHighestSetBitIndex;
  int highestSetBitIndex;

  highestSetBitIndex = 31;
  if (high != 0) {
    for (; high >> highestSetBitIndex == 0; highestSetBitIndex--) {
    }
  }
  if (high == 0) {
    lowHighestSetBitIndex = 31;
    if (low != 0) {
      for (; low >> lowHighestSetBitIndex == 0; lowHighestSetBitIndex--) {
      }
    }
    if (low == 0) {
      return 0;
    }
    initialRootShift = (uint8_t)(lowHighestSetBitIndex + 1U >> 1);
  }
  else {
    initialRootShift = (uint8_t)(highestSetBitIndex + 33U >> 1);
  }
  rootEstimate = 1 << (initialRootShift & 0x1f);
  /* unsigned DIV of EDX:EAX = high:low */
  refinedRootEstimate =
       rootEstimate + (int)(((uint64_t)high << 32 | (uint64_t)low) / (uint64_t)rootEstimate) >> 1;
  secondRootEstimate =
       refinedRootEstimate + (int)(((uint64_t)high << 32 | (uint64_t)low) / (uint64_t)refinedRootEstimate) >> 1;
  return (int)(((uint64_t)high << 32 | (uint64_t)low) / (uint64_t)secondRootEstimate) + secondRootEstimate >> 1;
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
  /* 32768 = half turn in angle16 units, 268435456 = 2^28 = 1.0 in Q28 */
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

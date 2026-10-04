/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/src/core/math/fixed_trig.cpp
 * Reverse engineering by idkFoxes 2026
 */

#include <math.h>
#include <thandor/core/math/fixed_trig.h>
#include <thandor/thandor.h>

/* Module data. */

/* one sine over 1.5 turns in Q28 (layout in <thandor/core/math/fixed_trig.h>, index macros
   FIXED_SINE_TABLE_* in <thandor/core/math/fixed_point.h>); filled at startup by FixedMath_BuildSinCosTables */
int32_t g_FixedSineQ28[98304] = {0};

/* Implementation ownership: core/math/fixed_trig. */

/* Converts the vector (x, y, z) into its length and two 16-bit angles: the elevation of x over the (y, z) plane
   and the azimuth within that plane (masked to 16 bits); squares are summed in 64 bits so Q12 components
   cannot overflow.
*/
FixedLengthAzimuthElevation
FixedMath_VectorToAnglesAndLength(FixedMathVectorComponent32 x,FixedMathVectorComponent32 y,FixedMathVectorComponent32 z)

{
  uint32_t planeLength;
  uint32_t elevationAngle;
  uint32_t azimuthAngle;
  uint32_t vectorLengthQ12;
  FixedLengthAzimuthElevation lengthAnglesResult;
  int64_t planeSquaredLengthQ24;
  int64_t totalSquaredLengthQ24;

  planeSquaredLengthQ24 = (int64_t)y * (int64_t)y + (int64_t)z * (int64_t)z;
  planeLength = FIXED_UINT64_SQRT(planeSquaredLengthQ24);
  elevationAngle = FixedMath_Atan2Angle16(x,planeLength);
  azimuthAngle = FixedMath_Atan2Angle16(y,z);
  totalSquaredLengthQ24 = planeSquaredLengthQ24 + (int64_t)x * (int64_t)x;
  vectorLengthQ12 = FIXED_UINT64_SQRT(totalSquaredLengthQ24);
  lengthAnglesResult.elevationAngle = elevationAngle;
  lengthAnglesResult.lengthQ12 = vectorLengthQ12;
  lengthAnglesResult.azimuthAngle = azimuthAngle & FIXED_ANGLE16_MASK;
  return lengthAnglesResult;
}

/* Converts a vector into its length and two 16-bit angles: the elevation of z over the (x, y) plane and the
   azimuth atan2(y, x) within it (the component roles differ from FixedMath_VectorToAnglesAndLength).
   The azimuth is masked to 16 bits; squares are summed in 64 bits.
   Used by the shot maintenance code for ballistic angles.
*/
FixedLengthAzimuthElevation FixedMath_VectorToAnglesAndLengthVec3(GraphicsFixedVec3 *vector)

{
  uint32_t horizontalLengthQ12;
  uint32_t elevationAngleResult;
  uint32_t azimuthAngle16;
  uint32_t vectorLengthQ12;
  FixedLengthAzimuthElevation lengthAnglesResult;
  int y;
  int x;
  int64_t totalSquaredLengthQ24;
  int64_t squaredLengthAccumulatorQ24;
  GraphicsWorldCoordinateQ12 inputZQ12;

  x = vector->x;
  inputZQ12 = vector->z;
  y = vector->y;
  squaredLengthAccumulatorQ24 = (int64_t)y * (int64_t)y + (int64_t)x * (int64_t)x;
  horizontalLengthQ12 = FIXED_UINT64_SQRT(squaredLengthAccumulatorQ24);
  elevationAngleResult = FixedMath_Atan2Angle16(vector->z,horizontalLengthQ12);
  azimuthAngle16 = FixedMath_Atan2Angle16(y,x);
  totalSquaredLengthQ24 = squaredLengthAccumulatorQ24 + (int64_t)inputZQ12 * (int64_t)inputZQ12;
  vectorLengthQ12 = FIXED_UINT64_SQRT(totalSquaredLengthQ24);
  lengthAnglesResult.elevationAngle = elevationAngleResult;
  lengthAnglesResult.lengthQ12 = vectorLengthQ12;
  lengthAnglesResult.azimuthAngle = azimuthAngle16 & FIXED_ANGLE16_MASK;
  return lengthAnglesResult;
}

/* Angle and length of a 2D vector: angle = atan2(component0, component1) as a 16-bit angle (65536 = full
   turn), length = floor(sqrt(component0^2 + component1^2)). Used by the army movement and combat code and the
   shot catalog for planar headings and distances.
*/
FixedLengthAngle FixedMath_Vector2AngleAndLength
          (FixedMathVectorComponent32 component0,FixedMathVectorComponent32 component1)

{
  uint32_t vectorAngle16;
  uint32_t vectorLengthQ12;
  FixedLengthAngle lengthAngle;

  vectorAngle16 = FixedMath_Atan2Angle16(component0,component1);
  vectorLengthQ12 = FixedMath_Length2(component0,component1);
  lengthAngle.length = vectorLengthQ12;
  lengthAngle.angle = vectorAngle16 & FIXED_ANGLE16_MASK;
  return lengthAngle;
}

/* Writes the Q28 unit direction for an elevation and an azimuth angle (16-bit turns, 65536 = full circle):
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

  elevationAngle16 = elevationAngle & FIXED_ANGLE16_MASK;
  elevationSinQ28 = g_FixedSineQ28[FIXED_SINE_TABLE_SIN + elevationAngle16];
  azimuthPlusElevationAngle16 = (elevationAngle16 + azimuthAngle) & FIXED_ANGLE16_MASK;
  azimuthMinusElevationAngle16 = (azimuthAngle - elevationAngle16) & FIXED_ANGLE16_MASK;
  azimuthPlusElevationSinQ28 = g_FixedSineQ28[FIXED_SINE_TABLE_SIN + azimuthPlusElevationAngle16];
  azimuthMinusElevationSinQ28 = g_FixedSineQ28[FIXED_SINE_TABLE_SIN + azimuthMinusElevationAngle16];
  output->x = (g_FixedSineQ28[FIXED_SINE_TABLE_COS + azimuthPlusElevationAngle16] +
               g_FixedSineQ28[FIXED_SINE_TABLE_COS + azimuthMinusElevationAngle16]) >> 1;
  output->y = (azimuthPlusElevationSinQ28 + azimuthMinusElevationSinQ28) >> 1;
  output->z = elevationSinQ28;
}

/* Returns cos(angle) * scale and sin(angle) * scale for a 16-bit angle (65536 = full turn),
   using the Q28 tables, so the results keep the scale's fixed-point format. Used for terrain direction
   records and by the rotation basis builder.
*/
FixedSinCos FixedMath_SinCosScaled(AngleTurn32 angle,FixedMathScale32 scale)

{
  FixedSinCos result;

  /* bits 28..59 of the 64-bit products, i.e. the Q28 factor is divided out */
  result.cosValue =
       (int32_t)((int64_t)g_FixedSineQ28[FIXED_SINE_TABLE_COS + (angle & FIXED_ANGLE16_MASK)] * (int64_t)scale >> 28);
  result.sinValue =
       (int32_t)((int64_t)g_FixedSineQ28[FIXED_SINE_TABLE_SIN + (angle & FIXED_ANGLE16_MASK)] * (int64_t)scale >> 28);
  return result;
}

/* Moves a planar point by distance in the direction of a 16-bit angle: returns x = baseX + cos(angle) *
   distance and y = baseY + sin(angle) * distance (Q28 table products shifted right by 28). Used by the
   army movement code to step a unit along its heading.
*/
FixedPlanarPointQ12
FixedTrig_ProjectPlanarPoint(Q12 baseX,Q12 baseY,Q12 distance,AngleTurn32 angle16)

{
  FixedPlanarPointQ12 point;

  /* bits 28..59 of the 64-bit products */
  point.xQ12 = (Q12)(baseX + (uint32_t)((int64_t)g_FixedSineQ28[FIXED_SINE_TABLE_COS + (angle16 & FIXED_ANGLE16_MASK)] *
                                        (int64_t)distance >> 28));
  point.yQ12 = (Q12)((uint32_t)((int64_t)g_FixedSineQ28[FIXED_SINE_TABLE_SIN + (angle16 & FIXED_ANGLE16_MASK)] *
                                (int64_t)distance >> 28) + baseY);
  return point;
}

/* Direction angles of a vector without its length: elevation = atan2(z, |(x, y)|) and azimuth = atan2(y, x)
   as 16-bit angles (the horizontal length is summed in 64 bits). Used by the army suspension, the graphics
   direction setup and FixedVector_StepBackwardAlongOwnDirection.
*/
FixedElevationAzimuth FixedMath_VectorToAnglesVec3(GraphicsFixedVec3 *vector)

{
  uint32_t horizontalLengthQ12;
  uint32_t elevationAngle16;
  uint32_t azimuthAngle16;
  int y;
  int x;
  int64_t horizontalSquaredLengthAccumulatorQ24;
  FixedElevationAzimuth vectorAngles;
  
  x = vector->x;
  y = vector->y;
  horizontalSquaredLengthAccumulatorQ24 = (int64_t)y * (int64_t)y + (int64_t)x * (int64_t)x;
  horizontalLengthQ12 = FIXED_UINT64_SQRT(horizontalSquaredLengthAccumulatorQ24);
  elevationAngle16 = FixedMath_Atan2Angle16(vector->z,horizontalLengthQ12);
  azimuthAngle16 = FixedMath_Atan2Angle16(y,x);
  vectorAngles.elevationAngle = elevationAngle16;
  vectorAngles.azimuthAngle = azimuthAngle16 & FIXED_ANGLE16_MASK;
  return vectorAngles;
}

/* Returns the direction of an elevation and an azimuth angle (16-bit turns) multiplied by scale:
   x = cos(az)cos(el) * scale, y = sin(az)cos(el) * scale, z = sin(el) * scale. The shot creator uses it to
   seed projectile velocities. As in FixedMath_WriteDirectionQ28 the products come from sum-to-product
   identities; the halving is folded into the shift (29 instead of 28).
*/
FixedDirection FixedMath_DirectionFromAnglesScaled(AngleTurn32 elevationAngle,AngleTurn32 azimuthAngle,
          FixedMathScale32 scale)

{
  int64_t scaledYProduct;
  uint32_t azimuthPlusElevationAngle16;
  uint32_t elevationAngle16;
  uint32_t azimuthMinusElevationAngle16;
  FixedDirection scaledDirection;
  int64_t scaledXProduct;

  elevationAngle16 = elevationAngle & FIXED_ANGLE16_MASK;
  azimuthPlusElevationAngle16 = (elevationAngle16 + azimuthAngle) & FIXED_ANGLE16_MASK;
  azimuthMinusElevationAngle16 = (azimuthAngle - elevationAngle16) & FIXED_ANGLE16_MASK;
  scaledXProduct =
       (int64_t)(g_FixedSineQ28[FIXED_SINE_TABLE_COS + azimuthPlusElevationAngle16] +
                 g_FixedSineQ28[FIXED_SINE_TABLE_COS + azimuthMinusElevationAngle16]) *
       (int64_t)scale;
  scaledYProduct =
       (int64_t)(g_FixedSineQ28[FIXED_SINE_TABLE_SIN + azimuthPlusElevationAngle16] +
                 g_FixedSineQ28[FIXED_SINE_TABLE_SIN + azimuthMinusElevationAngle16]) *
       (int64_t)scale;
  scaledDirection.z =
       FIXED_PRODUCT_SHR((int64_t)g_FixedSineQ28[FIXED_SINE_TABLE_SIN + elevationAngle16] * (int64_t)scale,28);
  scaledDirection.y = FIXED_PRODUCT_SHR(scaledYProduct,29);
  scaledDirection.x = FIXED_PRODUCT_SHR(scaledXProduct,29);
  return scaledDirection;
}

/* Returns the Q28 unit direction of an elevation and an azimuth angle (16-bit turns): x = cos(az)cos(el),
   y = sin(az)cos(el), z = sin(el), formed like FixedMath_WriteDirectionQ28. The rotation-basis builder uses
   it for its rows.
*/
FixedDirection FixedMath_DirectionFromAnglesQ28(AngleTurn32 elevationAngle,AngleTurn32 azimuthAngle)

{
  uint32_t azimuthPlusElevationAngle16;
  uint32_t elevationAngle16;
  uint32_t azimuthMinusElevationAngle16;
  FixedDirection directionQ28;

  elevationAngle16 = elevationAngle & FIXED_ANGLE16_MASK;
  azimuthPlusElevationAngle16 = (elevationAngle16 + azimuthAngle) & FIXED_ANGLE16_MASK;
  azimuthMinusElevationAngle16 = (azimuthAngle - elevationAngle16) & FIXED_ANGLE16_MASK;
  directionQ28.x = (g_FixedSineQ28[FIXED_SINE_TABLE_COS + azimuthPlusElevationAngle16] +
                    g_FixedSineQ28[FIXED_SINE_TABLE_COS + azimuthMinusElevationAngle16]) >> 1;
  directionQ28.y = (g_FixedSineQ28[FIXED_SINE_TABLE_SIN + azimuthPlusElevationAngle16] +
                    g_FixedSineQ28[FIXED_SINE_TABLE_SIN + azimuthMinusElevationAngle16]) >> 1;
  directionQ28.z = g_FixedSineQ28[FIXED_SINE_TABLE_SIN + elevationAngle16];
  return directionQ28;
}

/* Scaled form of FixedMath_WriteDirectionQ28: writes {cos(el)cos(az), cos(el)sin(az), sin(el)} * scale, so the
   output has the scale's fixed-point format. x and y use the sum-to-product sums (twice the value), hence the
   shift by 29 instead of 28. Used for the frustum corner rays and by FixedTransform_RotateScaledDirectionCore.
*/
void FixedMath_WriteDirectionScaled(GraphicsFixedVec3 *output,AngleTurn32 elevationAngle,AngleTurn32 azimuthAngle,
          FixedMathScale32 scale)

{
  uint32_t azimuthPlusElevationAngle16;
  uint32_t elevationAngle16;
  uint32_t azimuthMinusElevationAngle16;
  int verticalSinQ28;
  int azimuthPlusElevationSinQ28;
  int azimuthMinusElevationSinQ28;
  int64_t yComponentScaleProduct;
  int64_t horizontalComponentScaleProduct;

  elevationAngle16 = elevationAngle & FIXED_ANGLE16_MASK;
  verticalSinQ28 = g_FixedSineQ28[FIXED_SINE_TABLE_SIN + elevationAngle16];
  azimuthPlusElevationAngle16 = (elevationAngle16 + azimuthAngle) & FIXED_ANGLE16_MASK;
  azimuthMinusElevationAngle16 = (azimuthAngle - elevationAngle16) & FIXED_ANGLE16_MASK;
  azimuthPlusElevationSinQ28 = g_FixedSineQ28[FIXED_SINE_TABLE_SIN + azimuthPlusElevationAngle16];
  azimuthMinusElevationSinQ28 = g_FixedSineQ28[FIXED_SINE_TABLE_SIN + azimuthMinusElevationAngle16];
  horizontalComponentScaleProduct =
       (int64_t)(g_FixedSineQ28[FIXED_SINE_TABLE_COS + azimuthPlusElevationAngle16] +
                 g_FixedSineQ28[FIXED_SINE_TABLE_COS + azimuthMinusElevationAngle16]) *
       (int64_t)scale;
  output->x = FIXED_PRODUCT_SHR(horizontalComponentScaleProduct,29);
  yComponentScaleProduct = (int64_t)(azimuthPlusElevationSinQ28 + azimuthMinusElevationSinQ28) * (int64_t)scale;
  output->y = FIXED_PRODUCT_SHR(yComponentScaleProduct,29);
  output->z = FIXED_PRODUCT_SHR((int64_t)verticalSinQ28 * (int64_t)scale,28);
}

/* Converts a vector, passed in the order z, y, x, into its direction angles (16-bit turns): elevation
   atan2(z, sqrt(x*x + y*y)) and azimuth atan2(y, x). It is the inverse of FixedMath_WriteDirectionQ28 and is
   used for aiming and view angles.
*/
FixedVectorAngles FixedMath_VectorToAngles
          (FixedMathVectorComponent32 z,FixedMathVectorComponent32 y,FixedMathVectorComponent32 x)

{
  uint32_t horizontalMagnitudeQ12;
  uint32_t elevationAngle16;
  uint32_t azimuthAngle16;
  int64_t horizontalMagnitudeSquaredQ24;
  FixedVectorAngles vectorAngles;

  horizontalMagnitudeSquaredQ24 = (int64_t)y * (int64_t)y + (int64_t)x * (int64_t)x;
  horizontalMagnitudeQ12 = FIXED_UINT64_SQRT(horizontalMagnitudeSquaredQ24);
  elevationAngle16 = FixedMath_Atan2Angle16(z,horizontalMagnitudeQ12);
  azimuthAngle16 = FixedMath_Atan2Angle16(y,x);
  vectorAngles.azimuthAngle = azimuthAngle16 & FIXED_ANGLE16_MASK;
  vectorAngles.elevationAngle = elevationAngle16;
  return vectorAngles;
}

/* atan2(y, x) as an engine angle (1/65536 turns, not masked to 16 bits). The plane is split into
   eighth-turn sectors, rotating (x, y) so the remaining angle is within +-1/16 turn, which an odd
   polynomial in the ratio numerator/denominator approximates. (0, 0) yields 0.
*/
uint32_t FixedMath_Atan2Angle16(FixedMathVectorComponent32 y,FixedMathVectorComponent32 x)

{
  int doubledX;
  int doubledY;
  int denominator;
  uint32_t reducedAngleNumerator;
  int ratioQ31;
  int ratioSquaredQ30;
  AngleTurn16Stored32 octantBaseAngle16;

  octantBaseAngle16 = 0;
  doubledX = x * 2;
  doubledY = y * 2;
  if (doubledY <= x) {
    if (doubledX < -y) {
      if (y < doubledX) {
        octantBaseAngle16 = -FIXED_ANGLE16_QUARTER_TURN;
        denominator = -y;
        reducedAngleNumerator = x;
      }
      else {
        reducedAngleNumerator = x - y;
        octantBaseAngle16 = -3 * FIXED_ANGLE16_EIGHTH_TURN;
        denominator = -(x + y);
      }
    }
    else {
      denominator = x;
      reducedAngleNumerator = y;
      if (doubledY < -x) {
        reducedAngleNumerator = x + y;
        octantBaseAngle16 = -FIXED_ANGLE16_EIGHTH_TURN;
        denominator = x - y;
      }
    }
  }
  else if (doubledX < -y) {
    if (doubledY < -x) {
      octantBaseAngle16 = FIXED_ANGLE16_HALF_TURN;
      denominator = -x;
      reducedAngleNumerator = -y;
      if (0 < (int)reducedAngleNumerator) {
        octantBaseAngle16 = -FIXED_ANGLE16_HALF_TURN;
      }
    }
    else {
      denominator = y - x;
      octantBaseAngle16 = 3 * FIXED_ANGLE16_EIGHTH_TURN;
      reducedAngleNumerator = -(x + y);
    }
  }
  else if (y < doubledX) {
    denominator = x + y;
    octantBaseAngle16 = FIXED_ANGLE16_EIGHTH_TURN;
    reducedAngleNumerator = y - x;
  }
  else {
    octantBaseAngle16 = FIXED_ANGLE16_QUARTER_TURN;
    reducedAngleNumerator = -x;
    denominator = y;
  }
  if (denominator * 2 == 0) {
    return 0;
  }
  ratioQ31 = (int)((int64_t)((uint64_t)reducedAngleNumerator << 32) / (int64_t)(denominator * 2));
  ratioSquaredQ30 = FIXED_MUL_HIGH(ratioQ31,ratioQ31);
  /* atan(t) in angle units (FIXED_ATAN_ANGLE16_C*) */
  return octantBaseAngle16 +
         FIXED_MUL_HIGH(ratioQ31,
                        FIXED_MUL_HIGH(ratioSquaredQ30,
                                       FIXED_MUL_HIGH(ratioSquaredQ30,FIXED_ATAN_ANGLE16_C5) - FIXED_ATAN_ANGLE16_C3) +
                        FIXED_ATAN_ANGLE16_C1);
}

/* Not in the original: the original executable carries these tables precomputed (98304
   dwords). They are one sine over 1.5 turns, from a quarter turn before angle 0:
   g_FixedSineQ28 holds angles -16384..-1, then 0..16383 (from FIXED_SINE_TABLE_SIN), then the
   cosine 0..65535 (from FIXED_SINE_TABLE_COS, cos(i) = sin(i + quarter turn)); lookups with signed
   or full-turn angles run on from one part into the next.
   Entry i is sin(i * 2pi / 65536) in Q28, rounded half up, computed with pi = 3.141592654; this
   reproduces every entry of the original. Called once at startup. */
static int32_t FixedMath_SineTableEntry(int index)
{
  /* 32768 = half turn in angle16 units, 268435456 = 2^28 = 1.0 in Q28 */
  return (int32_t)floor(sin(index * (3.141592654 / 32768.0)) * 268435456.0 + 0.5);
}

void FixedMath_BuildSinCosTables()
{
  int index;

  for (index = 0; index < 98304; index++) {
    g_FixedSineQ28[index] = FixedMath_SineTableEntry(index - FIXED_SINE_TABLE_SIN);
  }
}

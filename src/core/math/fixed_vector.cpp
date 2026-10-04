/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/src/core/math/fixed_vector.cpp
 * Reverse engineering by idkFoxes 2026
 */

#include <math.h>
#include <thandor/core/math/fixed_vector.h>
#include <thandor/thandor.h>

/* Implementation ownership: core/math/fixed_vector. */

/* Returns the length floor(sqrt(x*x + y*y + z*z)) of a 3D vector; the squares are summed in 64 bits so Q12
   world coordinates cannot overflow, and the result has the same fixed-point scale as the components.
*/
uint32_t FixedMath_Length3(FixedMathVectorComponent32 x,FixedMathVectorComponent32 y,
                 FixedMathVectorComponent32 z)

{
  int64_t squaredLengthQ24;

  squaredLengthQ24 = (int64_t)y * (int64_t)y + (int64_t)z * (int64_t)z + (int64_t)x * (int64_t)x;
  return FIXED_UINT64_SQRT(squaredLengthQ24);
}

/* Writes input / |input| as a Q28 unit vector (output may alias input). Each component is multiplied by
   2^32 / length (unsigned 64-by-32-bit division) and shifted right by 4. Vectors shorter than 2 give {0, 0, 0}.
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
    reciprocalLengthScaleQ32 = (int)(Q32_ONE / (uint64_t)inputLengthQ12);
    /* bits 4..35 of each 64-bit product */
    normalizedComponentProduct = (int64_t)reciprocalLengthScaleQ32 * (int64_t)input->x;
    output->x = FIXED_PRODUCT_SHR(normalizedComponentProduct,4);
    currentNormalizedComponentProduct = (int64_t)reciprocalLengthScaleQ32 * (int64_t)input->y;
    output->y = FIXED_PRODUCT_SHR(currentNormalizedComponentProduct,4);
    finalNormalizedComponentProduct = (int64_t)reciprocalLengthScaleQ32 * (int64_t)input->z;
    output->z = FIXED_PRODUCT_SHR(finalNormalizedComponentProduct,4);
  }
}

/* Dot product of two Q12 vectors, summed in 64 bits and shifted right by 12, so the result is Q12.
   Used by the model renderer for back-face and light-facing tests.
*/
int32_t FixedVec3_DotQ12(GraphicsFixedVec3 *left,GraphicsFixedVec3 *right)

{
  int64_t dotProductAccumulatorQ24;

  dotProductAccumulatorQ24 =
       (int64_t)right->y * (int64_t)left->y + (int64_t)right->x * (int64_t)left->x +
       (int64_t)right->z * (int64_t)left->z;
  return FIXED_PRODUCT_SHR(dotProductAccumulatorQ24,12);
}

/* Dot product summed in 64 bits and shifted right by 28: with one Q28 unit vector (a frustum plane normal or
   a direction) the result keeps the other vector's scale. Used for frustum culling and effect motion.
*/
int32_t FixedVec3_DotQ28(GraphicsFixedVec3 *left,GraphicsFixedVec3 *right)

{
  int64_t dotProductAccumulatorQ56;

  dotProductAccumulatorQ56 =
       (int64_t)right->y * (int64_t)left->y + (int64_t)right->x * (int64_t)left->x +
       (int64_t)right->z * (int64_t)left->z;
  return FIXED_PRODUCT_SHR(dotProductAccumulatorQ56,28);
}

/* Writes leftOperand x rightOperand (cross product of two Q12 vectors, 64-bit differences shifted right by 12).
   The parameters are in the original's order: output, rightOperand, leftOperand. Used to build the
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
  output->x = FIXED_PRODUCT_SHR(crossComponentProductDifferenceQ24,12);
  currentCrossProductDifferenceQ24 =
       (int64_t)currentLeftComponentQ12 * (int64_t)rightOperand->x -
       (int64_t)leftOperand->x * (int64_t)rightOperand->z;
  leftXQ12 = leftOperand->x;
  output->y = FIXED_PRODUCT_SHR(currentCrossProductDifferenceQ24,12);
  finalCrossProductDifferenceQ24 = (int64_t)leftXQ12 * (int64_t)rightOperand->y -
          (int64_t)leftOperand->y * (int64_t)rightOperand->x;
  output->z = FIXED_PRODUCT_SHR(finalCrossProductDifferenceQ24,12);
}

/* Length of a 3D vector: floor(sqrt(x*x + y*y + z*z)), with the squares summed in 64 bits so Q12 components
   cannot overflow. The result has the components' fixed-point scale.
*/
uint32_t FixedMath_LengthVec3(GraphicsFixedVec3 *vector)

{
  uint32_t vectorLengthQ12;
  int64_t squaredLengthQ24;

  squaredLengthQ24 =
       (int64_t)vector->y * (int64_t)vector->y + (int64_t)vector->x * (int64_t)vector->x +
       (int64_t)vector->z * (int64_t)vector->z;
  vectorLengthQ12 = FIXED_UINT64_SQRT(squaredLengthQ24);
  return vectorLengthQ12;
}

/* Length of the 2D vector (x, y): floor(sqrt(x*x + y*y)), with the squares summed in 64 bits so Q12
   components cannot overflow. The result has the components' fixed-point scale.
*/
uint32_t FixedMath_Length2(FixedMathVectorComponent32 x,FixedMathVectorComponent32 y)

{
  uint32_t vectorLengthQ12;
  int64_t squaredLengthAccumulatorQ24;

  squaredLengthAccumulatorQ24 = (int64_t)x * (int64_t)x + (int64_t)y * (int64_t)y;
  vectorLengthQ12 = FIXED_UINT64_SQRT(squaredLengthAccumulatorQ24);
  return vectorLengthQ12;
}

/* Approximate square root of a Q12 value, returned in Q12 (about 0.6% low): the input is shifted left by an
   even amount so its top bit lands on bit 27 or 28, a cubic polynomial in the normalized value is
   evaluated with the high 32 bits of signed 64-bit products, and the result is shifted right by half the
   normalizing shift. Inputs of 2^29 or more give wrong results (the shift wraps). No caller, function-pointer
   table or data reference to it was found in the port or the image data.
*/
uint32_t FixedMath_SqrtQ12Approx(uint32_t inputValue)

{
  int highestSetBitIndex;
  int normalizedValue;
  uint32_t normalizeShift;

  if (inputValue == 0) {
    return 0;
  }
  highestSetBitIndex = 31;
  while (inputValue >> highestSetBitIndex == 0) {
    highestSetBitIndex--;
  }
  normalizeShift = (28U - highestSetBitIndex) & FIXED_SQRT_EVEN_SHIFT_MASK; /* even, so the root can shift back by half */
  normalizedValue = inputValue << (int8_t)normalizeShift;
  return (FIXED_MUL_HIGH(normalizedValue,
                         FIXED_MUL_HIGH(normalizedValue,
                                        FIXED_MUL_HIGH(normalizedValue,FIXED_SQRT_POLY_C3) - FIXED_SQRT_POLY_C2) +
                         FIXED_SQRT_POLY_C1) + FIXED_SQRT_POLY_C0) >> (int8_t)(normalizeShift >> 1);
}

/* Integer square root of the 64-bit value high:low, used for vector lengths from 64-bit sums of squares.
   The start value is the power of two just above the root (from the highest set bit); three Newton
   steps x = (x + value / x) / 2 follow, the divisions being unsigned 64-by-32-bit divisions.
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
  rootEstimate = 1 << (initialRootShift & 31);
  /* unsigned division of the 64-bit value high:low */
  refinedRootEstimate = (rootEstimate + (int)(((uint64_t)high << 32 | (uint64_t)low) / (uint64_t)rootEstimate)) >> 1;
  secondRootEstimate =
       (refinedRootEstimate + (int)(((uint64_t)high << 32 | (uint64_t)low) / (uint64_t)refinedRootEstimate)) >> 1;
  return ((int)(((uint64_t)high << 32 | (uint64_t)low) / (uint64_t)secondRootEstimate) + secondRootEstimate) >> 1;
}

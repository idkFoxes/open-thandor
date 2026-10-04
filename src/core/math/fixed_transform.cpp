/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/src/core/math/fixed_transform.cpp
 * Reverse engineering by idkFoxes 2026
 */

#include <math.h>
#include <thandor/core/math/fixed_transform.h>
#include <thandor/thandor.h>

/* Module data. */

static GraphicsFixedMatrix3x4 g_FixedTransformInputRotationScratch = {};

static GraphicsFixedMatrix3x4 g_FixedTransformComposedRotationScratch = {};

static GraphicsFixedVec3 g_ModelTransformInput = {};

GraphicsFixedVec3 g_ModelTransformOutput = {};

/* Composes two orientations given as angle triples: builds the rotation basis of each (basis angles into
   g_ModelTransformScratchMatrix, input angles into the input scratch), multiplies them and extracts the angles
   of the product again. Used by the army movement code to add a local rotation to a heading.
   Returns the azimuth, elevation and roll of the composed rotation.
*/
FixedAzimuthElevationRoll FixedTransform_ComposeEulerAngles
          (AngleTurn32 inputAngle0,AngleTurn32 inputAngle1,AngleTurn32 inputAngle2,
          AngleTurn32 basisAngle0,AngleTurn32 basisAngle1,AngleTurn32 basisAngle2)

{
  FixedRollAzimuthElevation extractedAngles;
  FixedAzimuthElevationRoll composedAngles;

  FixedTransform_BuildRotationBasis(&g_ModelTransformScratchMatrix,basisAngle0,basisAngle1,basisAngle2);
  FixedTransform_BuildRotationBasis(&g_FixedTransformInputRotationScratch,inputAngle0,inputAngle1,inputAngle2);
  FixedTransform_Compose(&g_FixedTransformComposedRotationScratch,&g_FixedTransformInputRotationScratch,
                         &g_ModelTransformScratchMatrix);
  extractedAngles = FixedTransform_ExtractEulerAngles(&g_FixedTransformComposedRotationScratch);
  composedAngles.rollAngle = extractedAngles.rollAngle;
  composedAngles.azimuthAngle = extractedAngles.azimuthAngle;
  composedAngles.elevationAngle = extractedAngles.elevationAngle;
  return composedAngles;
}

/* Rotates the Q12 vector (x, y, z) by the rotation basis built from three angles and returns the rotated
   vector (the original pushed the components z first). Works through the shared model-transform scratch
   globals; used by the model hierarchy for view-relative vectors.
*/
FixedVectorQ12
FixedTransform_RotateVectorByEulerAngles
          (Q12 inputXQ12,Q12 inputYQ12,Q12 inputZQ12,AngleTurn32 rotationAngle0,
          AngleTurn32 rotationAngle1,AngleTurn32 rotationAngle2)

{
  FixedVectorQ12 rotatedVector;
  
  FixedTransform_BuildRotationBasis(&g_ModelTransformScratchMatrix,rotationAngle0,rotationAngle1,rotationAngle2);
  g_ModelTransformInput.x = inputXQ12;
  g_ModelTransformInput.y = inputYQ12;
  g_ModelTransformInput.z = inputZQ12;
  FixedTransform_ApplyPoint
            (&g_ModelTransformOutput,
             &g_ModelTransformInput,
             &g_ModelTransformScratchMatrix);
  rotatedVector.yQ12 = g_ModelTransformOutput.y;
  rotatedVector.xQ12 = g_ModelTransformOutput.x;
  rotatedVector.zQ12 = g_ModelTransformOutput.z;
  return rotatedVector;
}

/* Wrapper around FixedTransform_RotateScaledDirectionCore; the original only repacked the result, so it
   returns the same rotated direction. Used by the model hierarchy.
*/
FixedVectorQ12
FixedTransform_RotateScaledDirection
          (FixedMathScale32 directionScale,AngleTurn32 elevationAngle,AngleTurn32 azimuthAngle,
          AngleTurn32 rotationAngle0,AngleTurn32 rotationAngle1,AngleTurn32 rotationAngle2)

{
  return FixedTransform_RotateScaledDirectionCore
                    (directionScale,elevationAngle,azimuthAngle,rotationAngle0,rotationAngle1,
                     rotationAngle2);
}

/* Rotates a direction by the transform's 3x3 Q28 basis (output = basis * direction, each dot product summed
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
  output->x = FIXED_PRODUCT_SHR(basisDotProductAccumulatorQ40,28);
  currentBasisDotProductQ40 =
       (int64_t)transform->basisRow1[1] * (int64_t)direction->y +
       (int64_t)currentBasisRowComponent0Q28 * (int64_t)direction->x +
       (int64_t)transform->basisRow1[2] * (int64_t)direction->z;
  basisRow2Component0Q28 = transform->basisRow2[0];
  output->y = FIXED_PRODUCT_SHR(currentBasisDotProductQ40,28);
  finalBasisDotProductQ40 = (int64_t)transform->basisRow2[1] * (int64_t)direction->y +
          (int64_t)basisRow2Component0Q28 * (int64_t)direction->x +
          (int64_t)transform->basisRow2[2] * (int64_t)direction->z;
  output->z = FIXED_PRODUCT_SHR(finalBasisDotProductQ40,28);
}

/* Multiplies a direction by the transpose of the transform's 3x3 Q28 basis (for a rotation this is the inverse
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
  output->x = FIXED_PRODUCT_SHR(basisDotProductAccumulatorQ40,28);
  currentBasisDotProductQ40 =
       (int64_t)transform->basisRow1[1] * (int64_t)direction->y +
       (int64_t)currentBasisColumnRow0ComponentQ28 * (int64_t)direction->x +
       (int64_t)transform->basisRow2[1] * (int64_t)direction->z;
  basisRow0Component2Q28 = transform->basisRow0[2];
  output->y = FIXED_PRODUCT_SHR(currentBasisDotProductQ40,28);
  finalBasisDotProductQ40 = (int64_t)transform->basisRow1[2] * (int64_t)direction->y +
          (int64_t)basisRow0Component2Q28 * (int64_t)direction->x +
          (int64_t)transform->basisRow2[2] * (int64_t)direction->z;
  output->z = FIXED_PRODUCT_SHR(finalBasisDotProductQ40,28);
}

/* Returns (0 - xProduct - yProduct - zProduct) >> 28 (64-bit, arithmetic) as the original computes it: the low halves are subtracted with explicit borrows into the high half, and the result is
   high * 16 | low >> 28. */
static uint32_t FixedTransform_NegatedProductSumShr28(int64_t xProduct,int64_t yProduct,int64_t zProduct)

{
  int productXLow;
  uint32_t negatedProductXLow;
  uint32_t productYLow;
  uint32_t productZLow;
  uint32_t partialDifferenceLow;

  productXLow = (int)xProduct;
  negatedProductXLow = -productXLow;
  productYLow = (uint32_t)yProduct;
  partialDifferenceLow = negatedProductXLow - productYLow;
  productZLow = (uint32_t)zProduct;
  return (-(uint32_t)(productXLow != 0) - (int)((uint64_t)xProduct >> 32) -
          (int)((uint64_t)yProduct >> 32) - (uint32_t)(negatedProductXLow < productYLow) -
          (int)((uint64_t)zProduct >> 32) - (uint32_t)(partialDifferenceLow < productZLow)) * 16 |
         (partialDifferenceLow - productZLow) >> 28;
}

/* Inverts a rigid Q28 transform: the output basis is the adjugate of the input basis (each cofactor a 64-bit
   difference of products shifted right by 28), which is the inverse because a rotation has determinant 1,
   and the output translation is -(outputBasis * inputTranslation). Used for the view transform and the
   leg suspension.
*/
void FixedTransform_InvertRigidQ28(GraphicsFixedMatrix3x4 *output,GraphicsFixedMatrix3x4 *input)

{
  int64_t cofactor;
  int64_t translationXProduct;
  int64_t translationYProduct;
  int64_t translationZProduct;
  int nextFactor; /* input component loaded ahead of the store, as in the original (output may alias input) */
  int inputTranslationX;

  cofactor = (int64_t)input->basisRow1[1] * (int64_t)input->basisRow2[2] -
          (int64_t)input->basisRow1[2] * (int64_t)input->basisRow2[1];
  nextFactor = input->basisRow1[2];
  output->basisRow0[0] = FIXED_PRODUCT_SHR(cofactor,28);
  cofactor = (int64_t)nextFactor * (int64_t)input->basisRow2[0] -
          (int64_t)input->basisRow1[0] * (int64_t)input->basisRow2[2];
  nextFactor = input->basisRow1[0];
  output->basisRow1[0] = FIXED_PRODUCT_SHR(cofactor,28);
  cofactor = (int64_t)nextFactor * (int64_t)input->basisRow2[1] -
          (int64_t)input->basisRow1[1] * (int64_t)input->basisRow2[0];
  nextFactor = input->basisRow0[2];
  output->basisRow2[0] = FIXED_PRODUCT_SHR(cofactor,28);
  cofactor = (int64_t)nextFactor * (int64_t)input->basisRow2[1] -
          (int64_t)input->basisRow0[1] * (int64_t)input->basisRow2[2];
  nextFactor = input->basisRow0[0];
  output->basisRow0[1] = FIXED_PRODUCT_SHR(cofactor,28);
  cofactor = (int64_t)nextFactor * (int64_t)input->basisRow2[2] -
          (int64_t)input->basisRow0[2] * (int64_t)input->basisRow2[0];
  nextFactor = input->basisRow0[1];
  output->basisRow1[1] = FIXED_PRODUCT_SHR(cofactor,28);
  cofactor = (int64_t)nextFactor * (int64_t)input->basisRow2[0] -
          (int64_t)input->basisRow0[0] * (int64_t)input->basisRow2[1];
  nextFactor = input->basisRow0[1];
  output->basisRow2[1] = FIXED_PRODUCT_SHR(cofactor,28);
  cofactor = (int64_t)nextFactor * (int64_t)input->basisRow1[2] -
          (int64_t)input->basisRow0[2] * (int64_t)input->basisRow1[1];
  nextFactor = input->basisRow0[2];
  output->basisRow0[2] = FIXED_PRODUCT_SHR(cofactor,28);
  cofactor = (int64_t)nextFactor * (int64_t)input->basisRow1[0] -
          (int64_t)input->basisRow0[0] * (int64_t)input->basisRow1[2];
  nextFactor = input->basisRow0[0];
  output->basisRow1[2] = FIXED_PRODUCT_SHR(cofactor,28);
  cofactor = (int64_t)nextFactor * (int64_t)input->basisRow1[1] -
          (int64_t)input->basisRow0[1] * (int64_t)input->basisRow1[0];
  output->basisRow2[2] = FIXED_PRODUCT_SHR(cofactor,28);
  /* Each translation component is -(tx*r0 + ty*r1 + tz*r2) >> 28. As in the original, the input translation x
     for the y and z rows is the value loaded just before the previous row's store (output may alias input);
     the y and z inputs are read in each row. */
  translationXProduct = (int64_t)(input->translation).x * (int64_t)output->basisRow0[0];
  translationYProduct = (int64_t)(input->translation).y * (int64_t)output->basisRow0[1];
  translationZProduct = (int64_t)(input->translation).z * (int64_t)output->basisRow0[2];
  inputTranslationX = (input->translation).x;
  (output->translation).x =
       FixedTransform_NegatedProductSumShr28(translationXProduct,translationYProduct,translationZProduct);
  translationXProduct = (int64_t)inputTranslationX * (int64_t)output->basisRow1[0];
  translationYProduct = (int64_t)(input->translation).y * (int64_t)output->basisRow1[1];
  translationZProduct = (int64_t)(input->translation).z * (int64_t)output->basisRow1[2];
  inputTranslationX = (input->translation).x;
  (output->translation).y =
       FixedTransform_NegatedProductSumShr28(translationXProduct,translationYProduct,translationZProduct);
  translationXProduct = (int64_t)inputTranslationX * (int64_t)output->basisRow2[0];
  translationYProduct = (int64_t)(input->translation).y * (int64_t)output->basisRow2[1];
  translationZProduct = (int64_t)(input->translation).z * (int64_t)output->basisRow2[2];
  (output->translation).z =
       FixedTransform_NegatedProductSumShr28(translationXProduct,translationYProduct,translationZProduct);
}

/* Builds the direction of (elevationAngle, azimuthAngle) with length directionScale, rotates it by the
   rotation basis of the three rotation angles and returns the rotated vector. Works through the shared
   model-transform scratch globals; called only by FixedTransform_RotateScaledDirection.
*/
FixedVectorQ12 FixedTransform_RotateScaledDirectionCore
          (FixedMathScale32 directionScale,AngleTurn32 elevationAngle,AngleTurn32 azimuthAngle,
          AngleTurn32 rotationAngle0,AngleTurn32 rotationAngle1,AngleTurn32 rotationAngle2)

{
  FixedVectorQ12 rotatedVector;
  
  FixedTransform_BuildRotationBasis(&g_ModelTransformScratchMatrix,rotationAngle0,rotationAngle1,rotationAngle2);
  FixedMath_WriteDirectionScaled
            (&g_ModelTransformInput,elevationAngle,azimuthAngle,directionScale
            );
  FixedTransform_ApplyPoint
            (&g_ModelTransformOutput,
             &g_ModelTransformInput,
             &g_ModelTransformScratchMatrix);
  rotatedVector.yQ12 = g_ModelTransformOutput.y;
  rotatedVector.xQ12 = g_ModelTransformOutput.x;
  rotatedVector.zQ12 = g_ModelTransformOutput.z;
  return rotatedVector;
}

/* Inverse of FixedTransform_BuildRotationBasis: recovers the angles from a rotation basis, elevation
   and azimuth from the third column and the roll from the upper 2x2 block. Used to turn a composed
   suspension rotation back into a model node's local angles.
*/
FixedRollAzimuthElevation FixedTransform_ExtractEulerAngles(GraphicsFixedMatrix3x4 *transform)

{
  uint32_t rollAngle16;
  uint32_t rollMinusTwoAzimuth16;
  FixedVectorAngles forwardAngles;
  FixedRollAzimuthElevation eulerAngles;

  forwardAngles = FixedMath_VectorToAngles
                    (transform->basisRow2[2],transform->basisRow1[2],transform->basisRow0[2]);
  eulerAngles.elevationAngle = forwardAngles.elevationAngle;
  eulerAngles.azimuthAngle = forwardAngles.azimuthAngle;
  if ((int)forwardAngles.elevationAngle < 0) {
    /* for a negative elevation the 2x2 block yields roll - 2 * azimuth */
    rollMinusTwoAzimuth16 = FixedMath_Atan2Angle16
                      (transform->basisRow1[0] + transform->basisRow0[1],
                       transform->basisRow1[1] - transform->basisRow0[0]);
    rollAngle16 = (rollMinusTwoAzimuth16 + eulerAngles.azimuthAngle * 2) & FIXED_ANGLE16_MASK;
  }
  else {
    rollAngle16 =
         FixedMath_Atan2Angle16
                   (transform->basisRow1[0] - transform->basisRow0[1],
                    transform->basisRow1[1] + transform->basisRow0[0]);
  }
  eulerAngles.rollAngle = rollAngle16;
  return eulerAngles;
}

/* Concatenates two rigid transforms: output = outerTransform * innerTransform, i.e. a point is moved by
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
  output->basisRow0[0] = FIXED_PRODUCT_SHR(firstRowDotProduct,28);
  rowDotProduct = (int64_t)outerTransform->basisRow0[1] * (int64_t)innerTransform->basisRow1[1] +
          (int64_t)firstOuterRowX * (int64_t)innerTransform->basisRow0[1] +
          (int64_t)outerTransform->basisRow0[2] * (int64_t)innerTransform->basisRow2[1];
  outerRowX = outerTransform->basisRow0[0];
  output->basisRow0[1] = FIXED_PRODUCT_SHR(rowDotProduct,28);
  rowDotProduct = (int64_t)outerTransform->basisRow0[1] * (int64_t)innerTransform->basisRow1[2] +
          (int64_t)outerRowX * (int64_t)innerTransform->basisRow0[2] +
          (int64_t)outerTransform->basisRow0[2] * (int64_t)innerTransform->basisRow2[2];
  outerRowX = outerTransform->basisRow0[0];
  output->basisRow0[2] = FIXED_PRODUCT_SHR(rowDotProduct,28);
  rowDotProduct = (int64_t)outerTransform->basisRow0[1] * (int64_t)(innerTransform->translation).y +
          (int64_t)outerRowX * (int64_t)(innerTransform->translation).x +
          (int64_t)outerTransform->basisRow0[2] * (int64_t)(innerTransform->translation).z;
  outerRowX = outerTransform->basisRow1[0];
  (output->translation).x = FIXED_PRODUCT_SHR(rowDotProduct,28) + (outerTransform->translation).x;
  rowDotProduct = (int64_t)outerTransform->basisRow1[1] * (int64_t)innerTransform->basisRow1[0] +
          (int64_t)outerRowX * (int64_t)innerTransform->basisRow0[0] +
          (int64_t)outerTransform->basisRow1[2] * (int64_t)innerTransform->basisRow2[0];
  outerRowX = outerTransform->basisRow1[0];
  output->basisRow1[0] = FIXED_PRODUCT_SHR(rowDotProduct,28);
  rowDotProduct = (int64_t)outerTransform->basisRow1[1] * (int64_t)innerTransform->basisRow1[1] +
          (int64_t)outerRowX * (int64_t)innerTransform->basisRow0[1] +
          (int64_t)outerTransform->basisRow1[2] * (int64_t)innerTransform->basisRow2[1];
  outerRowX = outerTransform->basisRow1[0];
  output->basisRow1[1] = FIXED_PRODUCT_SHR(rowDotProduct,28);
  rowDotProduct = (int64_t)outerTransform->basisRow1[1] * (int64_t)innerTransform->basisRow1[2] +
          (int64_t)outerRowX * (int64_t)innerTransform->basisRow0[2] +
          (int64_t)outerTransform->basisRow1[2] * (int64_t)innerTransform->basisRow2[2];
  outerRowX = outerTransform->basisRow1[0];
  output->basisRow1[2] = FIXED_PRODUCT_SHR(rowDotProduct,28);
  rowDotProduct = (int64_t)outerTransform->basisRow1[1] * (int64_t)(innerTransform->translation).y +
          (int64_t)outerRowX * (int64_t)(innerTransform->translation).x +
          (int64_t)outerTransform->basisRow1[2] * (int64_t)(innerTransform->translation).z;
  outerRowX = outerTransform->basisRow2[0];
  (output->translation).y = FIXED_PRODUCT_SHR(rowDotProduct,28) + (outerTransform->translation).y;
  rowDotProduct = (int64_t)outerTransform->basisRow2[1] * (int64_t)innerTransform->basisRow1[0] +
          (int64_t)outerRowX * (int64_t)innerTransform->basisRow0[0] +
          (int64_t)outerTransform->basisRow2[2] * (int64_t)innerTransform->basisRow2[0];
  outerRowX = outerTransform->basisRow2[0];
  output->basisRow2[0] = FIXED_PRODUCT_SHR(rowDotProduct,28);
  rowDotProduct = (int64_t)outerTransform->basisRow2[1] * (int64_t)innerTransform->basisRow1[1] +
          (int64_t)outerRowX * (int64_t)innerTransform->basisRow0[1] +
          (int64_t)outerTransform->basisRow2[2] * (int64_t)innerTransform->basisRow2[1];
  outerRowX = outerTransform->basisRow2[0];
  output->basisRow2[1] = FIXED_PRODUCT_SHR(rowDotProduct,28);
  rowDotProduct = (int64_t)outerTransform->basisRow2[1] * (int64_t)innerTransform->basisRow1[2] +
          (int64_t)outerRowX * (int64_t)innerTransform->basisRow0[2] +
          (int64_t)outerTransform->basisRow2[2] * (int64_t)innerTransform->basisRow2[2];
  outerRowX = outerTransform->basisRow2[0];
  output->basisRow2[2] = FIXED_PRODUCT_SHR(rowDotProduct,28);
  rowDotProduct = (int64_t)outerTransform->basisRow2[1] * (int64_t)(innerTransform->translation).y +
          (int64_t)outerRowX * (int64_t)(innerTransform->translation).x +
          (int64_t)outerTransform->basisRow2[2] * (int64_t)(innerTransform->translation).z;
  (output->translation).z = FIXED_PRODUCT_SHR(rowDotProduct,28) + (outerTransform->translation).z;
}

/* Moves a point through a rigid transform: output = basis * point + translation, where each row is a Q28
   dot product summed in 64 bits and shifted back by 28, so the point keeps its own scale.
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
  output->x = FIXED_PRODUCT_SHR(firstRowDotProduct,28) + (transform->translation).x;
  rowDotProduct = (int64_t)transform->basisRow1[1] * (int64_t)point->y +
          (int64_t)row1FirstCoefficientQ28 * (int64_t)point->x +
          (int64_t)transform->basisRow1[2] * (int64_t)point->z;
  row2FirstCoefficientQ28 = transform->basisRow2[0];
  output->y = FIXED_PRODUCT_SHR(rowDotProduct,28) +
              (transform->translation).y;
  rowDotProduct = (int64_t)transform->basisRow2[1] * (int64_t)point->y +
          (int64_t)row2FirstCoefficientQ28 * (int64_t)point->x +
          (int64_t)transform->basisRow2[2] * (int64_t)point->z;
  output->z = FIXED_PRODUCT_SHR(rowDotProduct,28) +
              (transform->translation).z;
}

/* Builds the Q28 rotation basis of an orientation given as roll, elevation and azimuth angle16s: the
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

  directionRow = FixedMath_DirectionFromAnglesQ28(elevationAngle,azimuthAngle);
  output->basisRow0[2] = directionRow.x;
  output->basisRow1[2] = directionRow.y;
  output->basisRow2[2] = directionRow.z;
  directionRow = FixedMath_DirectionFromAnglesQ28(elevationAngle,rollAngle - azimuthAngle);
  rollAngleUnmasked = (rollAngle - azimuthAngle) + azimuthAngle;
  output->basisRow2[1] = directionRow.y;
  output->basisRow2[0] = -directionRow.x;
  rollAngle16 = rollAngleUnmasked & FIXED_ANGLE16_MASK;
  rollMinusTwoAzimuth16 = (rollAngleUnmasked + azimuthAngle * -2) & FIXED_ANGLE16_MASK;
  /* half sums/differences of cos/sin(roll) and cos/sin(roll - 2 * azimuth), i.e. products of the
     roll and azimuth sines and cosines */
  firstHalfTrigTermQ28 = (g_FixedSineQ28[FIXED_SINE_TABLE_COS + rollAngle16] -
                          g_FixedSineQ28[FIXED_SINE_TABLE_COS + rollMinusTwoAzimuth16]) >> 1;
  sinElevationQ28 = g_FixedSineQ28[FIXED_SINE_TABLE_SIN + (elevationAngle & FIXED_ANGLE16_MASK)];
  output->basisRow0[0] = firstHalfTrigTermQ28;
  halfTrigTermQ28 = (g_FixedSineQ28[FIXED_SINE_TABLE_COS + rollAngle16] +
                     g_FixedSineQ28[FIXED_SINE_TABLE_COS + rollMinusTwoAzimuth16]) >> 1;
  output->basisRow1[1] =
       FIXED_PRODUCT_SHR((int64_t)firstHalfTrigTermQ28 * (int64_t)sinElevationQ28,28) + halfTrigTermQ28;
  termTimesSinElevation = (int64_t)halfTrigTermQ28 * (int64_t)sinElevationQ28;
  output->basisRow0[0] =
       output->basisRow0[0] +
       FIXED_PRODUCT_SHR(termTimesSinElevation,28);
  halfTrigTermQ28 = (g_FixedSineQ28[FIXED_SINE_TABLE_SIN + rollAngle16] +
                     g_FixedSineQ28[FIXED_SINE_TABLE_SIN + rollMinusTwoAzimuth16]) >> 1;
  output->basisRow1[0] = halfTrigTermQ28;
  firstTermTimesSinElevation = (int64_t)halfTrigTermQ28 * (int64_t)sinElevationQ28;
  halfTrigTermQ28 = (g_FixedSineQ28[FIXED_SINE_TABLE_SIN + rollMinusTwoAzimuth16] -
                     g_FixedSineQ28[FIXED_SINE_TABLE_SIN + rollAngle16]) >> 1;
  secondTermTimesSinElevation = (int64_t)halfTrigTermQ28 * (int64_t)sinElevationQ28;
  output->basisRow0[1] = halfTrigTermQ28 - FIXED_PRODUCT_SHR(firstTermTimesSinElevation,28);
  output->basisRow1[0] =
       output->basisRow1[0] -
       FIXED_PRODUCT_SHR(secondTermTimesSinElevation,28);
}

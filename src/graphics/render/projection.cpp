/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/src/graphics/render/projection.cpp
 * Reverse engineering by idkFoxes 2026
 */

/* View projection: the view and projection parameters, the projection viewport and clip rectangle, the
   frustum planes, the auxiliary orientation and the point projection used by the renderers. */

#include <thandor/graphics/render/projection.h>
#include <thandor/thandor.h>
#include <thandor/platform/bootstrap/image.h>
#include <thandor/platform/debug/hooks.h>

/* Module data. */

THANDOR_ALIGN(4) GraphicsFixedVec3 g_ViewOriginFixed = {0};

int32_t g_ProjectionScaleFixed = 0;

GraphicsFixedMatrix3x4 g_ViewProjectionMatrixFixed = {0};

GraphicsFixedMatrix3x4 g_AuxiliaryRotationMatrixFixed = {0};

static GraphicsViewAngle16 g_ViewAngle0 = 0;

static GraphicsViewAngle16 g_ViewAngle1 = 0;

static uint32_t g_ProjectionShift = 0;

static GraphicsWideFixed g_ProjectionNumerator = {0};

static GraphicsFixedVec2 g_ProjectionCenterFixed = {0};

static GraphicsFixedMatrix3x4 g_ViewRotationMatrixFixed = {0};

static GraphicsFixedMatrix3x4 g_CameraTransformMatrixFixed = {0};

static GraphicsFixedVec3 g_FrustumCornerRayFixed_0[4] = {0};

GraphicsFixedVec3 g_AuxiliaryForwardDirectionFixed = {0};

GraphicsFixedVec3 g_FrustumPlaneNormalFixed_0[4] = {0};

GraphicsSceneBounds8 g_SceneBoundsFixed = {0};

GraphicsPrimitiveQueue *g_ActivePrimitiveQueue = nullptr;

GraphicsFixedRect g_ProjectionClipRect = {0};

/* Perspective-projects one view-space Q12 point to screen coordinates (returned as an x/y pair): the perspective
   scale is the 64-bit projection numerator divided by z, x and y are scaled by it and offset by the projection
   centre. Points with z not above the numerator's high dword (behind or too close to the eye, where the
   32-bit division would overflow) project to (0,0).
*/
GraphicsProjectedPointPair Graphics_ProjectViewPoint(GraphicsFixedVec3 *viewPoint)

{
  int perspectiveScaleQ12;
  GraphicsProjectedPointPair projectedPoint;
  GraphicsProjectedPointPair offscreenPoint;
  int64_t projectedXProduct;
  int64_t projectedYProduct;
  
  if (g_ProjectionNumerator.high < viewPoint->z) {
    perspectiveScaleQ12 =
         (int)((int64_t)((uint64_t)(uint32_t)g_ProjectionNumerator.high << 32 | g_ProjectionNumerator.low) /
               (int64_t)viewPoint->z);
    projectedXProduct = (int64_t)viewPoint->x * (int64_t)perspectiveScaleQ12;
    projectedYProduct = (int64_t)viewPoint->y * (int64_t)perspectiveScaleQ12;
    /* bits 12..43 of the 64-bit product, i.e. the Q12 product shifted back by 12 */
    projectedPoint.projectedY =
         (FIXED_PRODUCT_SHR(projectedYProduct, 12)) +
         g_ProjectionCenterFixed.component1;
    projectedPoint.projectedX =
         g_ProjectionCenterFixed.component0 +
         (FIXED_PRODUCT_SHR(projectedXProduct, 12));
    return projectedPoint;
  }
  offscreenPoint.projectedX = 0;
  offscreenPoint.projectedY = 0;
  return offscreenPoint;
}

/* Sets the screen rectangle projected geometry is clipped against, converted from pixels to Q12 (20.12 fixed
   point). First step of a scene setup, before the view parameters and the viewport (called by
   FrontendModelPointerContext_RenderWorldViewQueuesClipped and GraphicsOffscreen_RenderModelListToTextureSource).
*/
void Graphics_SetProjectionClipRect
          (GraphicsScreenCoordinate maxY,GraphicsScreenCoordinate maxX,GraphicsScreenCoordinate minY
          ,GraphicsScreenCoordinate minX)

{
  g_ProjectionClipRect.minX = minX << Q12_SHIFT;
  g_ProjectionClipRect.minY = minY << Q12_SHIFT;
  g_ProjectionClipRect.maxX = maxX << Q12_SHIFT;
  g_ProjectionClipRect.maxY = maxY << Q12_SHIFT;
}

/* Sets up the camera for the next scene: stores the eye position (Q12 world coordinates), projection scale and
   view angles, builds the view rotation, the camera matrix (identity rotation, translation to the eye) and
   their composition g_ViewProjectionMatrixFixed. (The original also stored the sin/cos pairs of the view azimuth
   plus and minus the half view angle atan2(1 << (12 - projectionShift), projectionScale); nothing read them.)
   The azimuth/elevation names follow FixedMath_DirectionFromAnglesScaled, which
   Graphics_RebuildFrustumPlanes feeds with the same two angles.
*/
void Graphics_SetViewProjectionParameters
          (GraphicsProjectionShift projectionShift,GraphicsViewAngle16 viewElevationAngle,
          GraphicsViewAngle16 viewAzimuthAngle,GraphicsProjectionScale projectionScale,
          GraphicsWorldCoordinateQ12 originZ,GraphicsWorldCoordinateQ12 originY,
          GraphicsWorldCoordinateQ12 originX)

{
  g_ViewOriginFixed.x = originX;
  g_ViewOriginFixed.y = originY;
  g_ViewOriginFixed.z = originZ;
  g_ProjectionScaleFixed = projectionScale;
  g_ViewAngle0 = viewAzimuthAngle;
  g_ViewAngle1 = viewElevationAngle;
  FixedTransform_BuildRotationBasis
            (&g_ViewRotationMatrixFixed,FIXED_ANGLE16_QUARTER_TURN - viewAzimuthAngle & FIXED_ANGLE16_MASK,viewElevationAngle,
             FIXED_ANGLE16_THREE_QUARTER_TURN);
  g_ViewRotationMatrixFixed.translation.x = 0;
  g_ViewRotationMatrixFixed.translation.y = 0;
  g_ViewRotationMatrixFixed.translation.z = 0;
  g_CameraTransformMatrixFixed.translation.x = -originX;
  g_CameraTransformMatrixFixed.translation.y = -originY;
  g_CameraTransformMatrixFixed.translation.z = -originZ;
  g_CameraTransformMatrixFixed.basisRow0[0] = Q28_ONE;
  g_CameraTransformMatrixFixed.basisRow0[1] = 0;
  g_CameraTransformMatrixFixed.basisRow0[2] = 0;
  g_CameraTransformMatrixFixed.basisRow1[0] = 0;
  g_CameraTransformMatrixFixed.basisRow1[1] = Q28_ONE;
  g_CameraTransformMatrixFixed.basisRow1[2] = 0;
  g_CameraTransformMatrixFixed.basisRow2[0] = 0;
  g_CameraTransformMatrixFixed.basisRow2[1] = 0;
  g_CameraTransformMatrixFixed.basisRow2[2] = Q28_ONE;
  FixedTransform_Compose
            (&g_ViewProjectionMatrixFixed,&g_CameraTransformMatrixFixed,&g_ViewRotationMatrixFixed);
  g_ProjectionShift = projectionShift;
}

/* Maps the view onto a screen rectangle in pixels: the projection centre
   is the rectangle's midpoint in Q12, and the perspective numerator that Graphics_ProjectViewPoint divides by z
   is width * projection scale, shifted by g_ProjectionShift - 1 and widened to a signed 64-bit value << 12.
   Must follow Graphics_SetViewProjectionParameters, whose scale and shift it reads.
*/
void Graphics_SetProjectionViewport(GraphicsScreenCoordinate bottom,GraphicsScreenCoordinate right,
          GraphicsScreenCoordinate top,GraphicsScreenCoordinate left)

{
  int projectionShiftDelta;
  int64_t projectionScaleProduct;
  uint32_t shiftedScaleProduct;
  uint8_t rightShiftAmount;

  /* (a + b) * 0x800 = the midpoint (a + b) / 2 in Q12 */
  g_ProjectionCenterFixed.component0 = (left + right) * (Q12_ONE / 2);
  g_ProjectionCenterFixed.component1 = (top + bottom) * (Q12_ONE / 2);
  projectionShiftDelta = g_ProjectionShift - 1;
  projectionScaleProduct = (int64_t)(right - left) * (int64_t)(int)g_ProjectionScaleFixed;
  shiftedScaleProduct = (uint32_t)projectionScaleProduct;
  if (projectionShiftDelta != 0) {
    if (projectionShiftDelta < 0) {
      rightShiftAmount = -(uint8_t)projectionShiftDelta & SHIFT_COUNT_MASK;
      shiftedScaleProduct =
           shiftedScaleProduct >> rightShiftAmount |
           (int)((uint64_t)projectionScaleProduct >> 32) << (32 - rightShiftAmount);
    }
    else {
      shiftedScaleProduct = shiftedScaleProduct << ((uint8_t)projectionShiftDelta & SHIFT_COUNT_MASK);
    }
  }
  /* 64-bit numerator = sign-extended product << 12 */
  g_ProjectionNumerator.low = shiftedScaleProduct << Q12_SHIFT;
  g_ProjectionNumerator.high = (int)shiftedScaleProduct >> (32 - Q12_SHIFT);
}

/* Sets the scene's second direction (elevation/azimuth): stores its unit direction
   g_AuxiliaryForwardDirectionFixed and a rotation built like the view rotation. The model renderer transforms the
   direction into each model's space and passes it to ModelRender_ComputeVertexIntensity* as the light direction;
   the rotation is used by the generated-texture shading code.
*/
void Graphics_SetAuxiliaryOrientation(AngleTurn32 elevationAngle,AngleTurn32 azimuthAngle)

{
  FixedMath_WriteDirectionQ28(&g_AuxiliaryForwardDirectionFixed,elevationAngle,azimuthAngle);
  FixedTransform_BuildRotationBasis
            (&g_AuxiliaryRotationMatrixFixed,FIXED_ANGLE16_QUARTER_TURN - azimuthAngle & FIXED_ANGLE16_MASK,elevationAngle,
             FIXED_ANGLE16_THREE_QUARTER_TURN);
  g_AuxiliaryRotationMatrixFixed.translation.x = 0;
  g_AuxiliaryRotationMatrixFixed.translation.y = 0;
  g_AuxiliaryRotationMatrixFixed.translation.z = 0;
}

/* Stores the eight per-scene values in g_SceneBoundsFixed. bound4..bound7 are packed ARGB
   colours: the model renderer passes bound5/bound4 and bound7/bound6 as the scene colour pairs of
   ModelRender_ComputeVertexIntensityDefaultPath and ...ScaledPath. No reader of bound0..bound3 is known.
*/
void Graphics_SetSceneBoundsAndColors(GraphicsSceneExtentFixed bound7,GraphicsSceneExtentFixed bound6,
          GraphicsSceneExtentFixed bound5,GraphicsSceneExtentFixed bound4,
          GraphicsSceneExtentFixed bound3,GraphicsSceneExtentFixed bound2,
          GraphicsSceneExtentFixed bound1,GraphicsSceneExtentFixed bound0)

{
  g_SceneBoundsFixed.bound0 = bound0;
  g_SceneBoundsFixed.bound1 = bound1;
  g_SceneBoundsFixed.bound2 = bound2;
  g_SceneBoundsFixed.bound3 = bound3;
  g_SceneBoundsFixed.bound4 = bound4;
  g_SceneBoundsFixed.bound5 = bound5;
  g_SceneBoundsFixed.bound6 = bound6;
  g_SceneBoundsFixed.bound7 = bound7;
}

/* Selects the primitive queue the model renderer appends its triangles to (g_ActivePrimitiveQueue); the scene
   setup calls it with the queue freshly reset by GraphicsPrimitiveQueue_ResetGlobal.
*/
void Graphics_SetActivePrimitiveQueue(GraphicsPrimitiveQueue *queue)

{
  g_ActivePrimitiveQueue = queue;
}

/* Rebuilds the four side planes of the view frustum from the current view angles, projection scale and shift
   (call after Graphics_SetViewProjectionParameters). Two edge rays are forward + / - a sideways vector of length
   1 << (12 - shift), two are forward + / - an up/down vector of that length; the plane normals are cross
   products of neighbouring rays, normalised to Q28 in g_FrustumPlaneNormalFixed_0[0..3].
*/
void Graphics_RebuildFrustumPlanes()

{
  uint32_t forwardX;
  uint32_t forwardY;
  uint32_t forwardZ;
  uint32_t sideAzimuthAngle16;
  uint32_t edgeAzimuthAngle16;
  int scale;
  uint32_t upElevationAngle16;
  FixedDirection viewDirection;
  uint32_t viewElevationAngle16;
  uint32_t viewAzimuthAngle16;
  
  viewElevationAngle16 = g_ViewAngle1;
  viewAzimuthAngle16 = g_ViewAngle0;
  scale = 1 << (12U - (char)g_ProjectionShift & SHIFT_COUNT_MASK);
  viewDirection = FixedMath_DirectionFromAnglesScaled(g_ViewAngle1,g_ViewAngle0,g_ProjectionScaleFixed);
  forwardZ = viewDirection.z;
  forwardY = viewDirection.y;
  forwardX = viewDirection.x;
  /* rays 0 and 1: horizontal vectors of length scale, a quarter turn to either side */
  sideAzimuthAngle16 = viewAzimuthAngle16 + FIXED_ANGLE16_QUARTER_TURN & FIXED_ANGLE16_MASK;
  FixedMath_WriteDirectionScaled(g_FrustumCornerRayFixed_0,0,sideAzimuthAngle16,scale);
  edgeAzimuthAngle16 = sideAzimuthAngle16 - FIXED_ANGLE16_HALF_TURN & FIXED_ANGLE16_MASK;
  FixedMath_WriteDirectionScaled(g_FrustumCornerRayFixed_0 + 1,0,edgeAzimuthAngle16,scale);
  g_FrustumCornerRayFixed_0[0].x = g_FrustumCornerRayFixed_0[0].x + forwardX;
  g_FrustumCornerRayFixed_0[0].y = g_FrustumCornerRayFixed_0[0].y + forwardY;
  g_FrustumCornerRayFixed_0[0].z = g_FrustumCornerRayFixed_0[0].z + forwardZ;
  g_FrustumCornerRayFixed_0[1].x = g_FrustumCornerRayFixed_0[1].x + forwardX;
  g_FrustumCornerRayFixed_0[1].y = g_FrustumCornerRayFixed_0[1].y + forwardY;
  g_FrustumCornerRayFixed_0[1].z = g_FrustumCornerRayFixed_0[1].z + forwardZ;
  /* rays 2 and 3: up and down (elevation + / - a quarter turn) at the view azimuth again */
  edgeAzimuthAngle16 = edgeAzimuthAngle16 + FIXED_ANGLE16_QUARTER_TURN & FIXED_ANGLE16_MASK;
  upElevationAngle16 = viewElevationAngle16 + FIXED_ANGLE16_QUARTER_TURN & FIXED_ANGLE16_MASK;
  FixedMath_WriteDirectionScaled(g_FrustumCornerRayFixed_0 + 2,upElevationAngle16,edgeAzimuthAngle16,scale);
  FixedMath_WriteDirectionScaled
            (g_FrustumCornerRayFixed_0 + 3,upElevationAngle16 - FIXED_ANGLE16_HALF_TURN & FIXED_ANGLE16_MASK,edgeAzimuthAngle16,
             scale);
  FixedVec3_CrossQ12(g_FrustumPlaneNormalFixed_0,g_FrustumCornerRayFixed_0,
                     g_FrustumCornerRayFixed_0 + 2);
  FixedVec3_CrossQ12(g_FrustumPlaneNormalFixed_0 + 1,g_FrustumCornerRayFixed_0 + 2,
                     g_FrustumCornerRayFixed_0 + 1);
  /* ray 1 back to the pure sideways vector, rays 2 and 3 tilted forward */
  g_FrustumCornerRayFixed_0[1].x = g_FrustumCornerRayFixed_0[1].x - forwardX;
  g_FrustumCornerRayFixed_0[1].y = g_FrustumCornerRayFixed_0[1].y - forwardY;
  g_FrustumCornerRayFixed_0[1].z = g_FrustumCornerRayFixed_0[1].z - forwardZ;
  g_FrustumCornerRayFixed_0[2].x = g_FrustumCornerRayFixed_0[2].x + forwardX;
  g_FrustumCornerRayFixed_0[2].y = g_FrustumCornerRayFixed_0[2].y + forwardY;
  g_FrustumCornerRayFixed_0[2].z = g_FrustumCornerRayFixed_0[2].z + forwardZ;
  g_FrustumCornerRayFixed_0[3].x = g_FrustumCornerRayFixed_0[3].x + forwardX;
  g_FrustumCornerRayFixed_0[3].y = g_FrustumCornerRayFixed_0[3].y + forwardY;
  g_FrustumCornerRayFixed_0[3].z = g_FrustumCornerRayFixed_0[3].z + forwardZ;
  FixedVec3_CrossQ12(g_FrustumPlaneNormalFixed_0 + 2,g_FrustumCornerRayFixed_0 + 2,
                     g_FrustumCornerRayFixed_0 + 1);
  FixedVec3_CrossQ12(g_FrustumPlaneNormalFixed_0 + 3,g_FrustumCornerRayFixed_0 + 1,
                     g_FrustumCornerRayFixed_0 + 3);
  FixedVec3_NormalizeQ28(g_FrustumPlaneNormalFixed_0,g_FrustumPlaneNormalFixed_0);
  FixedVec3_NormalizeQ28(g_FrustumPlaneNormalFixed_0 + 1,g_FrustumPlaneNormalFixed_0 + 1);
  FixedVec3_NormalizeQ28(g_FrustumPlaneNormalFixed_0 + 2,g_FrustumPlaneNormalFixed_0 + 2);
  FixedVec3_NormalizeQ28(g_FrustumPlaneNormalFixed_0 + 3,g_FrustumPlaneNormalFixed_0 + 3);
}

/* Point-in-triangle test for the mouse pointer against a projected triangle, used by
   ModelRuntimeNode_HitTestProjectedBoundsAndChildren on the faces of a model's projected bounding box.
   Returns true when the pointer is strictly inside. Each edge test is a 64-bit cross product; the
   winding is normalized first (vertex1/vertex2 swapped when the triangle's own cross product is not
   negative) so that "inside" means all three pointer cross products are negative.
*/
Bool8 GraphicsProjectedPoint_IsInsideTriangle(int pointerY,int pointerX,GraphicsProjectedPoint2i *vertex0,
          GraphicsProjectedPoint2i *vertex1,GraphicsProjectedPoint2i *vertex2)

{
  int64_t crossPartB;
  int64_t crossPartA;
  GraphicsProjectedPoint2i *orderedVertex2;
  
  /* each test is the sign of the wrapping 64-bit sum crossPartA + crossPartB */
  crossPartA = (int64_t)(vertex0->y - vertex1->y) * (int64_t)vertex2->x +
          (int64_t)(vertex2->y - vertex0->y) * (int64_t)vertex1->x;
  crossPartB = (int64_t)(vertex1->y - vertex2->y) * (int64_t)vertex0->x;
  orderedVertex2 = vertex2;
  if (0 <= (int64_t)((uint64_t)crossPartB + (uint64_t)crossPartA)) {
    orderedVertex2 = vertex1;
    vertex1 = vertex2;
  }
  crossPartA = (int64_t)(pointerY - vertex1->y) * (int64_t)orderedVertex2->x +
          (int64_t)(orderedVertex2->y - pointerY) * (int64_t)vertex1->x;
  crossPartB = (int64_t)(vertex1->y - orderedVertex2->y) * (int64_t)pointerX;
  if ((int64_t)((uint64_t)crossPartB + (uint64_t)crossPartA) < 0) {
    crossPartA = (int64_t)(vertex0->y - pointerY) * (int64_t)orderedVertex2->x +
            (int64_t)(orderedVertex2->y - vertex0->y) * (int64_t)pointerX;
    crossPartB = (int64_t)(pointerY - orderedVertex2->y) * (int64_t)vertex0->x;
    if ((int64_t)((uint64_t)crossPartB + (uint64_t)crossPartA) < 0) {
      crossPartA = (int64_t)(vertex0->y - vertex1->y) * (int64_t)pointerX +
              (int64_t)(pointerY - vertex0->y) * (int64_t)vertex1->x;
      crossPartB = (int64_t)(vertex1->y - pointerY) * (int64_t)vertex0->x;
      if ((int64_t)((uint64_t)crossPartB + (uint64_t)crossPartA) < 0) {
        return true;
      }
    }
  }
  return false;
}

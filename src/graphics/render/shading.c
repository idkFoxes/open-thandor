/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/src/graphics/render/shading.c
 * Reverse engineering by idkFoxes 2026
 */

#include <thandor/graphics/render/shading.h>
#include <thandor/thandor.h>

/* Implementation ownership: graphics/render/shading. */

/* Not in the original: C stand-in for MOVD mm,packed; PUNPCKLBW mm,mm; PSRLW mm,shift. Every byte b of
   packed becomes the word lane ((b << 8) | b) >> shift (byte k -> word lane k), which turns a packed
   colour into four fixed-point factors for PMULHW. */
static __inline uint64_t Shading_DuplicateBytesToWordLanes(uint32_t packed,int shift)
{
  uint64_t lanes;
  uint32_t laneByte;
  int lane;

  lanes = 0;
  for (lane = 0; lane < 4; lane++) {
    laneByte = (packed >> (lane * 8)) & 0xff;
    lanes = lanes | ((uint64_t)((((laneByte << 8) | laneByte) >> shift) & 0xffff) << (lane * 16));
  }
  return lanes;
}

/* Not in the original: C stand-in for PACKUSWB mm,mm; MOVD dword,mm. Saturates the four signed word
   lanes to unsigned bytes (<= 0 -> 0, > 0xff -> 0xff) and packs them into one dword (word lane k ->
   byte k), i.e. turns scaled colour lanes back into a packed ARGB colour. */
static __inline uint32_t Shading_PackWordLanesUnsignedSaturate(uint64_t lanes)
{
  uint32_t packed;
  short laneValue;
  int lane;

  packed = 0;
  for (lane = 0; lane < 4; lane++) {
    laneValue = (short)(lanes >> (lane * 16));
    if (0xff < laneValue) {
      packed = packed | (0xffu << (lane * 8));
    }
    else if (0 < laneValue) {
      packed = packed | ((uint32_t)laneValue << (lane * 8));
    }
  }
  return packed;
}

/* Address: 0x004CDD40.
   Casts the shadow of one model hierarchy onto the terrain (world view render pass, context flag 0x20000,
   called per candidate model from the frontend world render in src/ui/frontend/runtime.c). While the
   generated shadow textures still have a free tile, a model inside the view frustum gets twelve sample
   points (corners and edge midpoints of its light-space bounds plus four inner points); each point is moved
   along the light direction (renderContext angles) onto the terrain, and the 14 reserved projected point blocks
   become a textured patch whose vertex alpha fades with the ray distance to the caster. The model's
   silhouette is then rasterized into the current texture tile (blurred for flag-0 mesh records, sharp for
   the rest) and the tile cursor advances. Any point without a terrain hit abandons the model.
*/
void GraphicsShadingGeneratedTexture_ProcessRenderableHierarchy
          (ModelRuntimeNode *modelNode,GeneratedTextureRenderContextView *renderContext)

{
  GraphicsProjectedPointPair pointPair1;
  GraphicsProjectedPointPair pointPair2;
  GraphicsProjectedPointPair pointPair3;
  GraphicsWorldCoordinateQ12 sampleWorldZ;
  int32_t planeDotOrTextureOffset;
  int radiusScaleOrCoordinate;
  uint32_t directionXOrTileCoordinate;
  FixedMathScale32 surfaceDistanceQ12;
  Q12 remainingRayLength;
  GraphicsProjectedPointPair *projectedBlocks;
  uint32_t tintMaskOrIntensityB;
  uint32_t directionYOrGridStep;
  int halfDirectionYOrCoordinate;
  uint32_t directionZ;
  int halfDirectionZOrCoordinate;
  uint32_t intensityC;
  uint32_t heightDeltaOrIntensityA;
  bool lacksGeometry;
  uint64_t shadedA;
  uint64_t shadedB;
  uint64_t shadedC;
  uint64_t tintLanesOrShadedC;
  GraphicsProjectedPointPair pointPair0;
  HeightSampleResult surfaceHeight;
  ProjectedBlockReserveResult reservedBlocks;
  TerrainRaycastResult terrainRay;
  FixedDirection direction;
  
  if (g_GraphicsShadingGeneratedTextureCompletedTraversalCount == 0) {
    g_GeneratedTextureScratchRuntime.currentModelOriginQ12.x =
         (modelNode->worldTransform).translation.x;
    g_GeneratedTextureScratchRuntime.currentModelOriginQ12.y =
         (modelNode->worldTransform).translation.y;
    g_GeneratedTextureScratchRuntime.currentModelOriginQ12.z =
         (modelNode->worldTransform).translation.z;
    radiusScaleOrCoordinate = modelNode->subtreeBoundingRadiusQ12;
    g_ModelCullViewRelativeX =
         g_GeneratedTextureScratchRuntime.currentModelOriginQ12.x - g_ViewOriginFixed.x;
    g_ModelCullViewRelativeY =
         g_GeneratedTextureScratchRuntime.currentModelOriginQ12.y - g_ViewOriginFixed.y;
    g_ModelCullViewRelativeZ =
         g_GeneratedTextureScratchRuntime.currentModelOriginQ12.z - g_ViewOriginFixed.z;
    lacksGeometry = GraphicsShadingGeneratedTexture_ProbeHierarchyForGeometry(modelNode);
    if (!lacksGeometry) {
      /* frustum test with 2.25 times the subtree radius, since the shadow reaches beyond the model */
      heightDeltaOrIntensityA = (uint32_t)(radiusScaleOrCoordinate * 9) >> 2;
      planeDotOrTextureOffset = FixedVec3_DotQ28(g_FrustumPlaneNormalFixed_0,
                                (GraphicsFixedVec3 *)&g_ModelCullViewRelativeX);
      if ((((planeDotOrTextureOffset <= (int)heightDeltaOrIntensityA) &&
           (planeDotOrTextureOffset = FixedVec3_DotQ28(g_FrustumPlaneNormalFixed_0 + 1,
                                      (GraphicsFixedVec3 *)&g_ModelCullViewRelativeX),
           planeDotOrTextureOffset <= (int)heightDeltaOrIntensityA)) &&
          (planeDotOrTextureOffset = FixedVec3_DotQ28(g_FrustumPlaneNormalFixed_0 + 2,
                                     (GraphicsFixedVec3 *)&g_ModelCullViewRelativeX),
          planeDotOrTextureOffset <= (int)heightDeltaOrIntensityA)) &&
         (planeDotOrTextureOffset = FixedVec3_DotQ28(g_FrustumPlaneNormalFixed_0 + 3,
                                    (GraphicsFixedVec3 *)&g_ModelCullViewRelativeX),
         planeDotOrTextureOffset <= (int)heightDeltaOrIntensityA)) {
        FixedTransform_ApplyPoint
                  ((GraphicsFixedVec3 *)&g_ModelCullViewRelativeX,
                   &g_GeneratedTextureScratchRuntime.currentModelOriginQ12,
                   &g_ViewProjectionMatrixFixed);
        if (((int)g_ProjectionScaleFixed < (int)g_ModelCullViewRelativeZ) &&
           (((modelNode->modelPayload).modelResource)->boundingRadiusQ12 <
            (int)(g_ModelCullViewRelativeZ - g_ProjectionScaleFixed))) {
          g_GeneratedTextureScratchRuntime.projectedMinX = 0x7fffffff;
          g_GeneratedTextureScratchRuntime.projectedMinY = 0x7fffffff;
          g_GeneratedTextureScratchRuntime.projectedMaxX = -0x7fffffff;
          g_GeneratedTextureScratchRuntime.projectedMaxY = -0x7fffffff;
          GraphicsShadingGeneratedTexture_TraverseHierarchyAndAccumulateProjectedBounds(modelNode);
          direction = FixedMath_DirectionFromAnglesScaledRegs
                             (0,renderContext->viewAngleB8 + FIXED_ANGLE16_QUARTER_TURN & 0xffff,
                              g_GeneratedTextureScratchRuntime.projectedMinX +
                              g_GeneratedTextureScratchRuntime.projectedMaxX >> 1);
          g_GeneratedTextureScratchRuntime.currentModelOriginQ12.x =
               g_GeneratedTextureScratchRuntime.currentModelOriginQ12.x - direction.x;
          g_GeneratedTextureScratchRuntime.currentModelOriginQ12.y =
               g_GeneratedTextureScratchRuntime.currentModelOriginQ12.y - direction.y;
          g_GeneratedTextureScratchRuntime.currentModelOriginQ12.z =
               g_GeneratedTextureScratchRuntime.currentModelOriginQ12.z - direction.z;
          direction = FixedMath_DirectionFromAnglesScaledRegs
                             (renderContext->viewAngleBC + FIXED_ANGLE16_QUARTER_TURN,renderContext->viewAngleB8,
                              g_GeneratedTextureScratchRuntime.projectedMinY +
                              g_GeneratedTextureScratchRuntime.projectedMaxY >> 1);
          g_GeneratedTextureScratchRuntime.currentModelOriginQ12.x =
               g_GeneratedTextureScratchRuntime.currentModelOriginQ12.x - direction.x;
          g_GeneratedTextureScratchRuntime.currentModelOriginQ12.y =
               g_GeneratedTextureScratchRuntime.currentModelOriginQ12.y - direction.y;
          g_GeneratedTextureScratchRuntime.currentModelOriginQ12.z =
               g_GeneratedTextureScratchRuntime.currentModelOriginQ12.z - direction.z;
          g_GeneratedTextureScratchRuntime.samples[0].worldPoint.x =
               g_GeneratedTextureScratchRuntime.currentModelOriginQ12.x;
          g_GeneratedTextureScratchRuntime.samples[0].worldPoint.y =
               g_GeneratedTextureScratchRuntime.currentModelOriginQ12.y;
          g_GeneratedTextureScratchRuntime.samples[0].worldPoint.z =
               g_GeneratedTextureScratchRuntime.currentModelOriginQ12.z;
          g_GeneratedTextureScratchRuntime.samples[1].worldPoint.x =
               g_GeneratedTextureScratchRuntime.currentModelOriginQ12.x;
          g_GeneratedTextureScratchRuntime.samples[1].worldPoint.y =
               g_GeneratedTextureScratchRuntime.currentModelOriginQ12.y;
          g_GeneratedTextureScratchRuntime.samples[1].worldPoint.z =
               g_GeneratedTextureScratchRuntime.currentModelOriginQ12.z;
          g_GeneratedTextureScratchRuntime.samples[2].worldPoint.x =
               g_GeneratedTextureScratchRuntime.currentModelOriginQ12.x;
          g_GeneratedTextureScratchRuntime.samples[2].worldPoint.y =
               g_GeneratedTextureScratchRuntime.currentModelOriginQ12.y;
          g_GeneratedTextureScratchRuntime.samples[2].worldPoint.z =
               g_GeneratedTextureScratchRuntime.currentModelOriginQ12.z;
          g_GeneratedTextureScratchRuntime.samples[3].worldPoint.x =
               g_GeneratedTextureScratchRuntime.currentModelOriginQ12.x;
          g_GeneratedTextureScratchRuntime.samples[3].worldPoint.y =
               g_GeneratedTextureScratchRuntime.currentModelOriginQ12.y;
          g_GeneratedTextureScratchRuntime.samples[3].worldPoint.z =
               g_GeneratedTextureScratchRuntime.currentModelOriginQ12.z;
          g_GeneratedTextureScratchRuntime.samples[4].worldPoint.x =
               g_GeneratedTextureScratchRuntime.currentModelOriginQ12.x;
          g_GeneratedTextureScratchRuntime.samples[4].worldPoint.y =
               g_GeneratedTextureScratchRuntime.currentModelOriginQ12.y;
          g_GeneratedTextureScratchRuntime.samples[4].worldPoint.z =
               g_GeneratedTextureScratchRuntime.currentModelOriginQ12.z;
          g_GeneratedTextureScratchRuntime.samples[5].worldPoint.x =
               g_GeneratedTextureScratchRuntime.currentModelOriginQ12.x;
          g_GeneratedTextureScratchRuntime.samples[5].worldPoint.y =
               g_GeneratedTextureScratchRuntime.currentModelOriginQ12.y;
          g_GeneratedTextureScratchRuntime.samples[5].worldPoint.z =
               g_GeneratedTextureScratchRuntime.currentModelOriginQ12.z;
          g_GeneratedTextureScratchRuntime.samples[6].worldPoint.x =
               g_GeneratedTextureScratchRuntime.currentModelOriginQ12.x;
          g_GeneratedTextureScratchRuntime.samples[6].worldPoint.y =
               g_GeneratedTextureScratchRuntime.currentModelOriginQ12.y;
          g_GeneratedTextureScratchRuntime.samples[6].worldPoint.z =
               g_GeneratedTextureScratchRuntime.currentModelOriginQ12.z;
          g_GeneratedTextureScratchRuntime.samples[7].worldPoint.x =
               g_GeneratedTextureScratchRuntime.currentModelOriginQ12.x;
          g_GeneratedTextureScratchRuntime.samples[7].worldPoint.y =
               g_GeneratedTextureScratchRuntime.currentModelOriginQ12.y;
          g_GeneratedTextureScratchRuntime.samples[7].worldPoint.z =
               g_GeneratedTextureScratchRuntime.currentModelOriginQ12.z;
          g_GeneratedTextureScratchRuntime.samples[8].worldPoint.x =
               g_GeneratedTextureScratchRuntime.currentModelOriginQ12.x;
          g_GeneratedTextureScratchRuntime.samples[8].worldPoint.y =
               g_GeneratedTextureScratchRuntime.currentModelOriginQ12.y;
          g_GeneratedTextureScratchRuntime.samples[8].worldPoint.z =
               g_GeneratedTextureScratchRuntime.currentModelOriginQ12.z;
          g_GeneratedTextureScratchRuntime.samples[9].worldPoint.x =
               g_GeneratedTextureScratchRuntime.currentModelOriginQ12.x;
          g_GeneratedTextureScratchRuntime.samples[9].worldPoint.y =
               g_GeneratedTextureScratchRuntime.currentModelOriginQ12.y;
          g_GeneratedTextureScratchRuntime.samples[9].worldPoint.z =
               g_GeneratedTextureScratchRuntime.currentModelOriginQ12.z;
          g_GeneratedTextureScratchRuntime.samples[10].worldPoint.x =
               g_GeneratedTextureScratchRuntime.currentModelOriginQ12.x;
          g_GeneratedTextureScratchRuntime.samples[10].worldPoint.y =
               g_GeneratedTextureScratchRuntime.currentModelOriginQ12.y;
          g_GeneratedTextureScratchRuntime.samples[10].worldPoint.z =
               g_GeneratedTextureScratchRuntime.currentModelOriginQ12.z;
          g_GeneratedTextureScratchRuntime.samples[11].worldPoint.x =
               g_GeneratedTextureScratchRuntime.currentModelOriginQ12.x;
          g_GeneratedTextureScratchRuntime.samples[11].worldPoint.y =
               g_GeneratedTextureScratchRuntime.currentModelOriginQ12.y;
          g_GeneratedTextureScratchRuntime.samples[11].worldPoint.z =
               g_GeneratedTextureScratchRuntime.currentModelOriginQ12.z;
          /* EDX:EAX = sign-extended (halfSize - border) << 24, then unsigned DIV (as in the original). */
          radiusScaleOrCoordinate = (int)((uint64_t)((int64_t)(int)(g_GraphicsShadingGridHalfSize -
                                                   g_GeneratedTextureScratchRuntime.downsampleBorderOffset) *
                                         0x1000000) /
                        (uint64_t)
                        (uint32_t)(g_GeneratedTextureScratchRuntime.projectedMaxX -
                              g_GeneratedTextureScratchRuntime.projectedMinX));
          g_GeneratedTextureScratchRuntime.generatedTextureBasisTransform.basisRow0[0] =
               (int)((uint64_t)
                     ((int64_t)g_AuxiliaryRotationMatrixFixed.basisRow0[0] * (int64_t)radiusScaleOrCoordinate) >>
                    0x20) << 4 |
               (uint32_t)((int64_t)g_AuxiliaryRotationMatrixFixed.basisRow0[0] * (int64_t)radiusScaleOrCoordinate) >>
               0x1c;
          g_GeneratedTextureScratchRuntime.generatedTextureBasisTransform.basisRow0[1] =
               (int)((uint64_t)
                     ((int64_t)g_AuxiliaryRotationMatrixFixed.basisRow0[1] * (int64_t)radiusScaleOrCoordinate) >>
                    0x20) << 4 |
               (uint32_t)((int64_t)g_AuxiliaryRotationMatrixFixed.basisRow0[1] * (int64_t)radiusScaleOrCoordinate) >>
               0x1c;
          g_GeneratedTextureScratchRuntime.generatedTextureBasisTransform.basisRow0[2] =
               (int)((uint64_t)
                     ((int64_t)g_AuxiliaryRotationMatrixFixed.basisRow0[2] * (int64_t)radiusScaleOrCoordinate) >>
                    0x20) << 4 |
               (uint32_t)((int64_t)g_AuxiliaryRotationMatrixFixed.basisRow0[2] * (int64_t)radiusScaleOrCoordinate) >>
               0x1c;
          g_GeneratedTextureScratchRuntime.generatedTextureBasisTransform.basisRow2[0] = 0;
          g_GeneratedTextureScratchRuntime.generatedTextureBasisTransform.basisRow2[1] = 0;
          g_GeneratedTextureScratchRuntime.generatedTextureBasisTransform.basisRow2[2] = 0;
          radiusScaleOrCoordinate = (int)((uint64_t)((int64_t)(int)(g_GraphicsShadingGridHalfSize -
                                                   g_GeneratedTextureScratchRuntime.downsampleBorderOffset) *
                                         0x1000000) /
                        (uint64_t)
                        (uint32_t)(g_GeneratedTextureScratchRuntime.projectedMaxY -
                              g_GeneratedTextureScratchRuntime.projectedMinY));
          g_GeneratedTextureScratchRuntime.generatedTextureBasisTransform.basisRow1[0] =
               (int)((uint64_t)
                     ((int64_t)g_AuxiliaryRotationMatrixFixed.basisRow1[0] * (int64_t)radiusScaleOrCoordinate) >>
                    0x20) << 4 |
               (uint32_t)((int64_t)g_AuxiliaryRotationMatrixFixed.basisRow1[0] * (int64_t)radiusScaleOrCoordinate) >>
               0x1c;
          g_GeneratedTextureScratchRuntime.generatedTextureBasisTransform.basisRow1[1] =
               (int)((uint64_t)
                     ((int64_t)g_AuxiliaryRotationMatrixFixed.basisRow1[1] * (int64_t)radiusScaleOrCoordinate) >>
                    0x20) << 4 |
               (uint32_t)((int64_t)g_AuxiliaryRotationMatrixFixed.basisRow1[1] * (int64_t)radiusScaleOrCoordinate) >>
               0x1c;
          g_GeneratedTextureScratchRuntime.generatedTextureBasisTransform.basisRow1[2] =
               (int)((uint64_t)
                     ((int64_t)g_AuxiliaryRotationMatrixFixed.basisRow1[2] * (int64_t)radiusScaleOrCoordinate) >>
                    0x20) << 4 |
               (uint32_t)((int64_t)g_AuxiliaryRotationMatrixFixed.basisRow1[2] * (int64_t)radiusScaleOrCoordinate) >>
               0x1c;
          g_GeneratedTextureScratchRuntime.generatedTextureBasisTransform.translation.x = 0;
          g_GeneratedTextureScratchRuntime.generatedTextureBasisTransform.translation.y = 0;
          g_GeneratedTextureScratchRuntime.generatedTextureBasisTransform.translation.z = 0;
          direction = FixedMath_DirectionFromAnglesScaledRegs
                             (0,renderContext->viewAngleB8 + FIXED_ANGLE16_QUARTER_TURN & 0xffff,
                              g_GeneratedTextureScratchRuntime.projectedMaxX -
                              g_GeneratedTextureScratchRuntime.projectedMinX >> 1);
          directionZ = direction.z;
          directionYOrGridStep = direction.y;
          directionXOrTileCoordinate = direction.x;
          g_GeneratedTextureScratchRuntime.samples[0].worldPoint.x =
               g_GeneratedTextureScratchRuntime.samples[0].worldPoint.x + directionXOrTileCoordinate;
          g_GeneratedTextureScratchRuntime.samples[0].worldPoint.y =
               g_GeneratedTextureScratchRuntime.samples[0].worldPoint.y + directionYOrGridStep;
          g_GeneratedTextureScratchRuntime.samples[0].worldPoint.z =
               g_GeneratedTextureScratchRuntime.samples[0].worldPoint.z + directionZ;
          g_GeneratedTextureScratchRuntime.samples[1].worldPoint.x =
               g_GeneratedTextureScratchRuntime.samples[1].worldPoint.x + directionXOrTileCoordinate;
          g_GeneratedTextureScratchRuntime.samples[1].worldPoint.y =
               g_GeneratedTextureScratchRuntime.samples[1].worldPoint.y + directionYOrGridStep;
          g_GeneratedTextureScratchRuntime.samples[1].worldPoint.z =
               g_GeneratedTextureScratchRuntime.samples[1].worldPoint.z + directionZ;
          g_GeneratedTextureScratchRuntime.samples[2].worldPoint.x =
               g_GeneratedTextureScratchRuntime.samples[2].worldPoint.x - directionXOrTileCoordinate;
          g_GeneratedTextureScratchRuntime.samples[2].worldPoint.y =
               g_GeneratedTextureScratchRuntime.samples[2].worldPoint.y - directionYOrGridStep;
          g_GeneratedTextureScratchRuntime.samples[2].worldPoint.z =
               g_GeneratedTextureScratchRuntime.samples[2].worldPoint.z - directionZ;
          g_GeneratedTextureScratchRuntime.samples[3].worldPoint.x =
               g_GeneratedTextureScratchRuntime.samples[3].worldPoint.x - directionXOrTileCoordinate;
          g_GeneratedTextureScratchRuntime.samples[3].worldPoint.y =
               g_GeneratedTextureScratchRuntime.samples[3].worldPoint.y - directionYOrGridStep;
          g_GeneratedTextureScratchRuntime.samples[3].worldPoint.z =
               g_GeneratedTextureScratchRuntime.samples[3].worldPoint.z - directionZ;
          g_GeneratedTextureScratchRuntime.samples[4].worldPoint.x =
               g_GeneratedTextureScratchRuntime.samples[4].worldPoint.x + directionXOrTileCoordinate;
          g_GeneratedTextureScratchRuntime.samples[4].worldPoint.y =
               g_GeneratedTextureScratchRuntime.samples[4].worldPoint.y + directionYOrGridStep;
          g_GeneratedTextureScratchRuntime.samples[4].worldPoint.z =
               g_GeneratedTextureScratchRuntime.samples[4].worldPoint.z + directionZ;
          g_GeneratedTextureScratchRuntime.samples[5].worldPoint.x =
               g_GeneratedTextureScratchRuntime.samples[5].worldPoint.x - directionXOrTileCoordinate;
          g_GeneratedTextureScratchRuntime.samples[5].worldPoint.y =
               g_GeneratedTextureScratchRuntime.samples[5].worldPoint.y - directionYOrGridStep;
          g_GeneratedTextureScratchRuntime.samples[5].worldPoint.z =
               g_GeneratedTextureScratchRuntime.samples[5].worldPoint.z - directionZ;
          radiusScaleOrCoordinate = (int)directionXOrTileCoordinate >> 1;
          halfDirectionYOrCoordinate = (int)directionYOrGridStep >> 1;
          halfDirectionZOrCoordinate = (int)directionZ >> 1;
          g_GeneratedTextureScratchRuntime.samples[8].worldPoint.x =
               g_GeneratedTextureScratchRuntime.samples[8].worldPoint.x + radiusScaleOrCoordinate;
          g_GeneratedTextureScratchRuntime.samples[8].worldPoint.y =
               g_GeneratedTextureScratchRuntime.samples[8].worldPoint.y + halfDirectionYOrCoordinate;
          g_GeneratedTextureScratchRuntime.samples[8].worldPoint.z =
               g_GeneratedTextureScratchRuntime.samples[8].worldPoint.z + halfDirectionZOrCoordinate;
          g_GeneratedTextureScratchRuntime.samples[9].worldPoint.x =
               g_GeneratedTextureScratchRuntime.samples[9].worldPoint.x + radiusScaleOrCoordinate;
          g_GeneratedTextureScratchRuntime.samples[9].worldPoint.y =
               g_GeneratedTextureScratchRuntime.samples[9].worldPoint.y + halfDirectionYOrCoordinate;
          g_GeneratedTextureScratchRuntime.samples[9].worldPoint.z =
               g_GeneratedTextureScratchRuntime.samples[9].worldPoint.z + halfDirectionZOrCoordinate;
          g_GeneratedTextureScratchRuntime.samples[10].worldPoint.x =
               g_GeneratedTextureScratchRuntime.samples[10].worldPoint.x - radiusScaleOrCoordinate;
          g_GeneratedTextureScratchRuntime.samples[10].worldPoint.y =
               g_GeneratedTextureScratchRuntime.samples[10].worldPoint.y - halfDirectionYOrCoordinate;
          g_GeneratedTextureScratchRuntime.samples[10].worldPoint.z =
               g_GeneratedTextureScratchRuntime.samples[10].worldPoint.z - halfDirectionZOrCoordinate;
          g_GeneratedTextureScratchRuntime.samples[11].worldPoint.x =
               g_GeneratedTextureScratchRuntime.samples[11].worldPoint.x - radiusScaleOrCoordinate;
          g_GeneratedTextureScratchRuntime.samples[11].worldPoint.y =
               g_GeneratedTextureScratchRuntime.samples[11].worldPoint.y - halfDirectionYOrCoordinate;
          g_GeneratedTextureScratchRuntime.samples[11].worldPoint.z =
               g_GeneratedTextureScratchRuntime.samples[11].worldPoint.z - halfDirectionZOrCoordinate;
          direction = FixedMath_DirectionFromAnglesScaledRegs
                             (renderContext->viewAngleBC + FIXED_ANGLE16_QUARTER_TURN,renderContext->viewAngleB8,
                              g_GeneratedTextureScratchRuntime.projectedMaxY -
                              g_GeneratedTextureScratchRuntime.projectedMinY >> 1);
          directionZ = direction.z;
          directionYOrGridStep = direction.y;
          directionXOrTileCoordinate = direction.x;
          g_GeneratedTextureScratchRuntime.samples[0].worldPoint.x =
               g_GeneratedTextureScratchRuntime.samples[0].worldPoint.x + directionXOrTileCoordinate;
          g_GeneratedTextureScratchRuntime.samples[0].worldPoint.y =
               g_GeneratedTextureScratchRuntime.samples[0].worldPoint.y + directionYOrGridStep;
          g_GeneratedTextureScratchRuntime.samples[0].worldPoint.z =
               g_GeneratedTextureScratchRuntime.samples[0].worldPoint.z + directionZ;
          sampleWorldZ = g_GeneratedTextureScratchRuntime.samples[0].worldPoint.z;
          g_GeneratedTextureScratchRuntime.samples[1].worldPoint.x =
               g_GeneratedTextureScratchRuntime.samples[1].worldPoint.x - directionXOrTileCoordinate;
          g_GeneratedTextureScratchRuntime.samples[1].worldPoint.y =
               g_GeneratedTextureScratchRuntime.samples[1].worldPoint.y - directionYOrGridStep;
          g_GeneratedTextureScratchRuntime.samples[1].worldPoint.z =
               g_GeneratedTextureScratchRuntime.samples[1].worldPoint.z - directionZ;
          g_GeneratedTextureScratchRuntime.samples[2].worldPoint.x =
               g_GeneratedTextureScratchRuntime.samples[2].worldPoint.x + directionXOrTileCoordinate;
          g_GeneratedTextureScratchRuntime.samples[2].worldPoint.y =
               g_GeneratedTextureScratchRuntime.samples[2].worldPoint.y + directionYOrGridStep;
          g_GeneratedTextureScratchRuntime.samples[2].worldPoint.z =
               g_GeneratedTextureScratchRuntime.samples[2].worldPoint.z + directionZ;
          g_GeneratedTextureScratchRuntime.samples[3].worldPoint.x =
               g_GeneratedTextureScratchRuntime.samples[3].worldPoint.x - directionXOrTileCoordinate;
          g_GeneratedTextureScratchRuntime.samples[3].worldPoint.y =
               g_GeneratedTextureScratchRuntime.samples[3].worldPoint.y - directionYOrGridStep;
          g_GeneratedTextureScratchRuntime.samples[3].worldPoint.z =
               g_GeneratedTextureScratchRuntime.samples[3].worldPoint.z - directionZ;
          g_GeneratedTextureScratchRuntime.samples[6].worldPoint.x =
               g_GeneratedTextureScratchRuntime.samples[6].worldPoint.x + directionXOrTileCoordinate;
          g_GeneratedTextureScratchRuntime.samples[6].worldPoint.y =
               g_GeneratedTextureScratchRuntime.samples[6].worldPoint.y + directionYOrGridStep;
          g_GeneratedTextureScratchRuntime.samples[6].worldPoint.z =
               g_GeneratedTextureScratchRuntime.samples[6].worldPoint.z + directionZ;
          g_GeneratedTextureScratchRuntime.samples[7].worldPoint.x =
               g_GeneratedTextureScratchRuntime.samples[7].worldPoint.x - directionXOrTileCoordinate;
          g_GeneratedTextureScratchRuntime.samples[7].worldPoint.y =
               g_GeneratedTextureScratchRuntime.samples[7].worldPoint.y - directionYOrGridStep;
          g_GeneratedTextureScratchRuntime.samples[7].worldPoint.z =
               g_GeneratedTextureScratchRuntime.samples[7].worldPoint.z - directionZ;
          radiusScaleOrCoordinate = (int)directionXOrTileCoordinate >> 1;
          halfDirectionYOrCoordinate = (int)directionYOrGridStep >> 1;
          halfDirectionZOrCoordinate = (int)directionZ >> 1;
          g_GeneratedTextureScratchRuntime.samples[8].worldPoint.x =
               g_GeneratedTextureScratchRuntime.samples[8].worldPoint.x + radiusScaleOrCoordinate;
          g_GeneratedTextureScratchRuntime.samples[8].worldPoint.y =
               g_GeneratedTextureScratchRuntime.samples[8].worldPoint.y + halfDirectionYOrCoordinate;
          g_GeneratedTextureScratchRuntime.samples[8].worldPoint.z =
               g_GeneratedTextureScratchRuntime.samples[8].worldPoint.z + halfDirectionZOrCoordinate;
          g_GeneratedTextureScratchRuntime.samples[9].worldPoint.x =
               g_GeneratedTextureScratchRuntime.samples[9].worldPoint.x - radiusScaleOrCoordinate;
          g_GeneratedTextureScratchRuntime.samples[9].worldPoint.y =
               g_GeneratedTextureScratchRuntime.samples[9].worldPoint.y - halfDirectionYOrCoordinate;
          g_GeneratedTextureScratchRuntime.samples[9].worldPoint.z =
               g_GeneratedTextureScratchRuntime.samples[9].worldPoint.z - halfDirectionZOrCoordinate;
          g_GeneratedTextureScratchRuntime.samples[10].worldPoint.x =
               g_GeneratedTextureScratchRuntime.samples[10].worldPoint.x + radiusScaleOrCoordinate;
          g_GeneratedTextureScratchRuntime.samples[10].worldPoint.y =
               g_GeneratedTextureScratchRuntime.samples[10].worldPoint.y + halfDirectionYOrCoordinate;
          g_GeneratedTextureScratchRuntime.samples[10].worldPoint.z =
               g_GeneratedTextureScratchRuntime.samples[10].worldPoint.z + halfDirectionZOrCoordinate;
          g_GeneratedTextureScratchRuntime.samples[11].worldPoint.x =
               g_GeneratedTextureScratchRuntime.samples[11].worldPoint.x - radiusScaleOrCoordinate;
          g_GeneratedTextureScratchRuntime.samples[11].worldPoint.y =
               g_GeneratedTextureScratchRuntime.samples[11].worldPoint.y - halfDirectionYOrCoordinate;
          g_GeneratedTextureScratchRuntime.samples[11].worldPoint.z =
               g_GeneratedTextureScratchRuntime.samples[11].worldPoint.z - halfDirectionZOrCoordinate;
          g_GeneratedTextureScratchRuntime.samples[1].textureCoordinateOffsetQ20 =
               g_GraphicsShadingGridStepQ20Current - 0x1000;
          g_GeneratedTextureScratchRuntime.samples[4].textureCoordinateOffsetQ20 =
               (int)g_GraphicsShadingGridStepQ20Current >> 1;
          g_GeneratedTextureScratchRuntime.samples[0].textureCoordinateOffsetQ20 = 0;
          g_GeneratedTextureScratchRuntime.samples[2].textureCoordinateOffsetQ20 = 0;
          g_GeneratedTextureScratchRuntime.samples[6].textureCoordinateOffsetQ20 = 0;
          g_GeneratedTextureScratchRuntime.samples[8].textureCoordinateOffsetQ20 =
               g_GeneratedTextureScratchRuntime.samples[4].textureCoordinateOffsetQ20 -
               ((int)g_GraphicsShadingGridStepQ20Current >> 2);
          g_GeneratedTextureScratchRuntime.samples[9].textureCoordinateOffsetQ20 =
               ((int)g_GraphicsShadingGridStepQ20Current >> 2) +
               g_GeneratedTextureScratchRuntime.samples[4].textureCoordinateOffsetQ20;
          g_GeneratedTextureScratchRuntime.samples[3].textureCoordinateOffsetQ20 =
               g_GeneratedTextureScratchRuntime.samples[1].textureCoordinateOffsetQ20;
          g_GeneratedTextureScratchRuntime.samples[5].textureCoordinateOffsetQ20 =
               g_GeneratedTextureScratchRuntime.samples[4].textureCoordinateOffsetQ20;
          g_GeneratedTextureScratchRuntime.samples[7].textureCoordinateOffsetQ20 =
               g_GeneratedTextureScratchRuntime.samples[1].textureCoordinateOffsetQ20;
          g_GeneratedTextureScratchRuntime.samples[10].textureCoordinateOffsetQ20 =
               g_GeneratedTextureScratchRuntime.samples[8].textureCoordinateOffsetQ20;
          g_GeneratedTextureScratchRuntime.samples[11].textureCoordinateOffsetQ20 =
               g_GeneratedTextureScratchRuntime.samples[9].textureCoordinateOffsetQ20;
          surfaceHeight = FieldGrid_InterpolateTopSurfaceHeight
                             (g_GeneratedTextureScratchRuntime.samples[0].worldPoint.y,
                              g_GeneratedTextureScratchRuntime.samples[0].worldPoint.x,
                              renderContext->fieldGrid);
          planeDotOrTextureOffset = g_GeneratedTextureScratchRuntime.samples[0].textureCoordinateOffsetQ20;
          heightDeltaOrIntensityA = surfaceHeight.heightQ12 - sampleWorldZ;
          if (surfaceHeight.heightQ12 < sampleWorldZ) {
            terrainRay = FieldGrid_RaycastTerrainTrianglesAlongDirection
                               (renderContext->viewAngleBC,renderContext->viewAngleB8,
                                modelNode->subtreeBoundingRadiusQ12 * 2,
                                g_GeneratedTextureScratchRuntime.samples[0].worldPoint.z,
                                g_GeneratedTextureScratchRuntime.samples[0].worldPoint.y,
                                g_GeneratedTextureScratchRuntime.samples[0].worldPoint.x,
                                renderContext->fieldGrid);
            if (terrainRay.hit) {
              g_GeneratedTextureScratchRuntime.samples[0].terrainRayDistanceQ12 =
                   terrainRay.distanceQ12;
              direction = FixedMath_DirectionFromAnglesScaledRegs
                                 (renderContext->viewAngleBC,renderContext->viewAngleB8,
                                  terrainRay.distanceQ12);
              g_GeneratedTextureScratchRuntime.samples[0].worldPoint.x =
                   g_GeneratedTextureScratchRuntime.samples[0].worldPoint.x + direction.x;
              g_GeneratedTextureScratchRuntime.samples[0].worldPoint.y =
                   g_GeneratedTextureScratchRuntime.samples[0].worldPoint.y + direction.y;
              g_GeneratedTextureScratchRuntime.samples[0].worldPoint.z =
                   g_GeneratedTextureScratchRuntime.samples[0].worldPoint.z + direction.z;
            }
            else {
              radiusScaleOrCoordinate = modelNode->subtreeBoundingRadiusQ12 * 2;
              if (g_GraphicsShadingGridStepQ20Current -
                  g_GeneratedTextureScratchRuntime.samples[0].textureCoordinateOffsetQ20 == 0) {
                return;
              }
              remainingRayLength = (Q12)(((int64_t)
                              (int)(g_GraphicsShadingGridStepQ20Current -
                                   g_GeneratedTextureScratchRuntime.samples[0].
                                   textureCoordinateOffsetQ20) * (int64_t)radiusScaleOrCoordinate) /
                            (int64_t)(int)g_GraphicsShadingGridStepQ20Current);
              g_GeneratedTextureScratchRuntime.samples[0].terrainRayDistanceQ12 = radiusScaleOrCoordinate;
              direction = FixedMath_DirectionFromAnglesScaledRegs
                                 (renderContext->viewAngleBC,renderContext->viewAngleB8,radiusScaleOrCoordinate);
              g_GeneratedTextureScratchRuntime.samples[0].worldPoint.x =
                   g_GeneratedTextureScratchRuntime.samples[0].worldPoint.x + direction.x;
              g_GeneratedTextureScratchRuntime.samples[0].worldPoint.y =
                   g_GeneratedTextureScratchRuntime.samples[0].worldPoint.y + direction.y;
              g_GeneratedTextureScratchRuntime.samples[0].worldPoint.z =
                   g_GeneratedTextureScratchRuntime.samples[0].worldPoint.z + direction.z;
              heightDeltaOrIntensityA = renderContext->viewAngleB8 ^ FIXED_ANGLE16_HALF_TURN;
              terrainRay = FieldGrid_RaycastTerrainSurfaceDistance
                                 (-renderContext->viewAngleBC - FIXED_ANGLE16_QUARTER_TURN,heightDeltaOrIntensityA,remainingRayLength,
                                  g_GeneratedTextureScratchRuntime.samples[0].worldPoint.z,
                                  g_GeneratedTextureScratchRuntime.samples[0].worldPoint.y,
                                  g_GeneratedTextureScratchRuntime.samples[0].worldPoint.x,
                                  renderContext->fieldGrid);
              surfaceDistanceQ12 = terrainRay.distanceQ12;
              if (!terrainRay.hit) {
                return;
              }
              /* Original quirk, kept (all 12 such sites, e.g. 0x004CE3F4-0x004CE3F8): after the hit the original
                 pushes EAX (distance), ECX (azimuth, preserved by the raycast) and EDX as the elevation. EDX was
                 -viewAngleBC - 0x4000 before the call, but the raycast returns the hit cell's material byte in
                 EDX (0x00504C80), so the material byte becomes the elevation angle. */
              direction = FixedMath_DirectionFromAnglesScaledRegs
                                 (terrainRay.materialOrCellIndex,heightDeltaOrIntensityA,surfaceDistanceQ12); /* quirk: EDX */
              g_GeneratedTextureScratchRuntime.samples[0].worldPoint.x =
                   g_GeneratedTextureScratchRuntime.samples[0].worldPoint.x + direction.x;
              g_GeneratedTextureScratchRuntime.samples[0].worldPoint.y =
                   g_GeneratedTextureScratchRuntime.samples[0].worldPoint.y + direction.y;
              g_GeneratedTextureScratchRuntime.samples[0].worldPoint.z =
                   g_GeneratedTextureScratchRuntime.samples[0].worldPoint.z + direction.z;
              g_GeneratedTextureScratchRuntime.samples[0].textureCoordinateOffsetQ20 =
                   g_GeneratedTextureScratchRuntime.samples[0].textureCoordinateOffsetQ20 +
                   ((int)(((int64_t)surfaceDistanceQ12 * (int64_t)(int)g_GraphicsShadingGridStepQ20Current) /
                         (int64_t)modelNode->subtreeBoundingRadiusQ12) >> 1);
              if ((int)g_GraphicsShadingGridStepQ20Current <
                  g_GeneratedTextureScratchRuntime.samples[0].textureCoordinateOffsetQ20) {
                return;
              }
            }
          }
          else {
            if ((uint32_t)modelNode->subtreeBoundingRadiusQ12 <= heightDeltaOrIntensityA) {
              heightDeltaOrIntensityA = modelNode->subtreeBoundingRadiusQ12;
            }
            g_GeneratedTextureScratchRuntime.samples[0].terrainRayDistanceQ12 = 0;
            radiusScaleOrCoordinate = (int)(((int64_t)(int)g_GraphicsShadingGridStepQ20Current *
                           (int64_t)
                           (int)((int)((uint64_t)
                                       ((int64_t)(int)heightDeltaOrIntensityA *
                                       (int64_t)g_FixedCosQ28[renderContext->viewAngleBC]) >> 0x20)
                                 << 3 | (uint32_t)((int64_t)(int)heightDeltaOrIntensityA *
                                              (int64_t)g_FixedCosQ28[renderContext->viewAngleBC])
                                        >> 0x1d)) / (int64_t)modelNode->subtreeBoundingRadiusQ12);
            g_GeneratedTextureScratchRuntime.samples[0].textureCoordinateOffsetQ20 =
                 g_GeneratedTextureScratchRuntime.samples[0].textureCoordinateOffsetQ20 - radiusScaleOrCoordinate;
            if (planeDotOrTextureOffset < radiusScaleOrCoordinate) {
              g_GeneratedTextureScratchRuntime.samples[0].textureCoordinateOffsetQ20 = 0;
              direction = FixedMath_DirectionFromAnglesScaledRegs
                                 (renderContext->viewAngleBC,renderContext->viewAngleB8,
                                  modelNode->subtreeBoundingRadiusQ12 * 2);
              g_GeneratedTextureScratchRuntime.samples[0].worldPoint.x =
                   g_GeneratedTextureScratchRuntime.samples[0].worldPoint.x - direction.x;
              g_GeneratedTextureScratchRuntime.samples[0].worldPoint.y =
                   g_GeneratedTextureScratchRuntime.samples[0].worldPoint.y - direction.y;
              g_GeneratedTextureScratchRuntime.samples[0].worldPoint.z =
                   g_GeneratedTextureScratchRuntime.samples[0].worldPoint.z - direction.z;
              terrainRay = FieldGrid_RaycastTerrainTrianglesAlongDirection
                                 (renderContext->viewAngleBC,renderContext->viewAngleB8,
                                  modelNode->subtreeBoundingRadiusQ12 * 2,
                                  g_GeneratedTextureScratchRuntime.samples[0].worldPoint.z,
                                  g_GeneratedTextureScratchRuntime.samples[0].worldPoint.y,
                                  g_GeneratedTextureScratchRuntime.samples[0].worldPoint.x,
                                  renderContext->fieldGrid);
              if (!terrainRay.hit) {
                return;
              }
              direction = FixedMath_DirectionFromAnglesScaledRegs
                                 (renderContext->viewAngleBC,renderContext->viewAngleB8,
                                  terrainRay.distanceQ12);
              g_GeneratedTextureScratchRuntime.samples[0].worldPoint.x =
                   g_GeneratedTextureScratchRuntime.samples[0].worldPoint.x + direction.x;
              g_GeneratedTextureScratchRuntime.samples[0].worldPoint.y =
                   g_GeneratedTextureScratchRuntime.samples[0].worldPoint.y + direction.y;
              g_GeneratedTextureScratchRuntime.samples[0].worldPoint.z =
                   g_GeneratedTextureScratchRuntime.samples[0].worldPoint.z + direction.z;
            }
            else {
              g_GeneratedTextureScratchRuntime.samples[0].worldPoint.z =
                   g_GeneratedTextureScratchRuntime.samples[0].worldPoint.z + heightDeltaOrIntensityA;
            }
          }
          sampleWorldZ = g_GeneratedTextureScratchRuntime.samples[2].worldPoint.z;
          surfaceHeight = FieldGrid_InterpolateTopSurfaceHeight
                             (g_GeneratedTextureScratchRuntime.samples[2].worldPoint.y,
                              g_GeneratedTextureScratchRuntime.samples[2].worldPoint.x,
                              renderContext->fieldGrid);
          planeDotOrTextureOffset = g_GeneratedTextureScratchRuntime.samples[2].textureCoordinateOffsetQ20;
          heightDeltaOrIntensityA = surfaceHeight.heightQ12 - sampleWorldZ;
          if (surfaceHeight.heightQ12 < sampleWorldZ) {
            terrainRay = FieldGrid_RaycastTerrainTrianglesAlongDirection
                               (renderContext->viewAngleBC,renderContext->viewAngleB8,
                                modelNode->subtreeBoundingRadiusQ12 * 2,
                                g_GeneratedTextureScratchRuntime.samples[2].worldPoint.z,
                                g_GeneratedTextureScratchRuntime.samples[2].worldPoint.y,
                                g_GeneratedTextureScratchRuntime.samples[2].worldPoint.x,
                                renderContext->fieldGrid);
            if (terrainRay.hit) {
              g_GeneratedTextureScratchRuntime.samples[2].terrainRayDistanceQ12 =
                   terrainRay.distanceQ12;
              direction = FixedMath_DirectionFromAnglesScaledRegs
                                 (renderContext->viewAngleBC,renderContext->viewAngleB8,
                                  terrainRay.distanceQ12);
              g_GeneratedTextureScratchRuntime.samples[2].worldPoint.x =
                   g_GeneratedTextureScratchRuntime.samples[2].worldPoint.x + direction.x;
              g_GeneratedTextureScratchRuntime.samples[2].worldPoint.y =
                   g_GeneratedTextureScratchRuntime.samples[2].worldPoint.y + direction.y;
              g_GeneratedTextureScratchRuntime.samples[2].worldPoint.z =
                   g_GeneratedTextureScratchRuntime.samples[2].worldPoint.z + direction.z;
            }
            else {
              radiusScaleOrCoordinate = modelNode->subtreeBoundingRadiusQ12 * 2;
              if (g_GraphicsShadingGridStepQ20Current -
                  g_GeneratedTextureScratchRuntime.samples[2].textureCoordinateOffsetQ20 == 0) {
                return;
              }
              remainingRayLength = (Q12)(((int64_t)
                              (int)(g_GraphicsShadingGridStepQ20Current -
                                   g_GeneratedTextureScratchRuntime.samples[2].
                                   textureCoordinateOffsetQ20) * (int64_t)radiusScaleOrCoordinate) /
                            (int64_t)(int)g_GraphicsShadingGridStepQ20Current);
              g_GeneratedTextureScratchRuntime.samples[2].terrainRayDistanceQ12 = radiusScaleOrCoordinate;
              direction = FixedMath_DirectionFromAnglesScaledRegs
                                 (renderContext->viewAngleBC,renderContext->viewAngleB8,radiusScaleOrCoordinate);
              g_GeneratedTextureScratchRuntime.samples[2].worldPoint.x =
                   g_GeneratedTextureScratchRuntime.samples[2].worldPoint.x + direction.x;
              g_GeneratedTextureScratchRuntime.samples[2].worldPoint.y =
                   g_GeneratedTextureScratchRuntime.samples[2].worldPoint.y + direction.y;
              g_GeneratedTextureScratchRuntime.samples[2].worldPoint.z =
                   g_GeneratedTextureScratchRuntime.samples[2].worldPoint.z + direction.z;
              heightDeltaOrIntensityA = renderContext->viewAngleB8 ^ FIXED_ANGLE16_HALF_TURN;
              terrainRay = FieldGrid_RaycastTerrainSurfaceDistance
                                 (-renderContext->viewAngleBC - FIXED_ANGLE16_QUARTER_TURN,heightDeltaOrIntensityA,remainingRayLength,
                                  g_GeneratedTextureScratchRuntime.samples[2].worldPoint.z,
                                  g_GeneratedTextureScratchRuntime.samples[2].worldPoint.y,
                                  g_GeneratedTextureScratchRuntime.samples[2].worldPoint.x,
                                  renderContext->fieldGrid);
              surfaceDistanceQ12 = terrainRay.distanceQ12;
              if (!terrainRay.hit) {
                return;
              }
              direction = FixedMath_DirectionFromAnglesScaledRegs
                                 (terrainRay.materialOrCellIndex,heightDeltaOrIntensityA,surfaceDistanceQ12); /* quirk: EDX */
              g_GeneratedTextureScratchRuntime.samples[2].worldPoint.x =
                   g_GeneratedTextureScratchRuntime.samples[2].worldPoint.x + direction.x;
              g_GeneratedTextureScratchRuntime.samples[2].worldPoint.y =
                   g_GeneratedTextureScratchRuntime.samples[2].worldPoint.y + direction.y;
              g_GeneratedTextureScratchRuntime.samples[2].worldPoint.z =
                   g_GeneratedTextureScratchRuntime.samples[2].worldPoint.z + direction.z;
              g_GeneratedTextureScratchRuntime.samples[2].textureCoordinateOffsetQ20 =
                   g_GeneratedTextureScratchRuntime.samples[2].textureCoordinateOffsetQ20 +
                   ((int)(((int64_t)surfaceDistanceQ12 * (int64_t)(int)g_GraphicsShadingGridStepQ20Current) /
                         (int64_t)modelNode->subtreeBoundingRadiusQ12) >> 1);
              if ((int)g_GraphicsShadingGridStepQ20Current <
                  g_GeneratedTextureScratchRuntime.samples[2].textureCoordinateOffsetQ20) {
                return;
              }
            }
          }
          else {
            if ((uint32_t)modelNode->subtreeBoundingRadiusQ12 <= heightDeltaOrIntensityA) {
              heightDeltaOrIntensityA = modelNode->subtreeBoundingRadiusQ12;
            }
            g_GeneratedTextureScratchRuntime.samples[2].terrainRayDistanceQ12 = 0;
            radiusScaleOrCoordinate = (int)(((int64_t)(int)g_GraphicsShadingGridStepQ20Current *
                           (int64_t)
                           (int)((int)((uint64_t)
                                       ((int64_t)(int)heightDeltaOrIntensityA *
                                       (int64_t)g_FixedCosQ28[renderContext->viewAngleBC]) >> 0x20)
                                 << 3 | (uint32_t)((int64_t)(int)heightDeltaOrIntensityA *
                                              (int64_t)g_FixedCosQ28[renderContext->viewAngleBC])
                                        >> 0x1d)) / (int64_t)modelNode->subtreeBoundingRadiusQ12);
            g_GeneratedTextureScratchRuntime.samples[2].textureCoordinateOffsetQ20 =
                 g_GeneratedTextureScratchRuntime.samples[2].textureCoordinateOffsetQ20 - radiusScaleOrCoordinate;
            if (planeDotOrTextureOffset < radiusScaleOrCoordinate) {
              g_GeneratedTextureScratchRuntime.samples[2].textureCoordinateOffsetQ20 = 0;
              direction = FixedMath_DirectionFromAnglesScaledRegs
                                 (renderContext->viewAngleBC,renderContext->viewAngleB8,
                                  modelNode->subtreeBoundingRadiusQ12 * 2);
              g_GeneratedTextureScratchRuntime.samples[2].worldPoint.x =
                   g_GeneratedTextureScratchRuntime.samples[2].worldPoint.x - direction.x;
              g_GeneratedTextureScratchRuntime.samples[2].worldPoint.y =
                   g_GeneratedTextureScratchRuntime.samples[2].worldPoint.y - direction.y;
              g_GeneratedTextureScratchRuntime.samples[2].worldPoint.z =
                   g_GeneratedTextureScratchRuntime.samples[2].worldPoint.z - direction.z;
              terrainRay = FieldGrid_RaycastTerrainTrianglesAlongDirection
                                 (renderContext->viewAngleBC,renderContext->viewAngleB8,
                                  modelNode->subtreeBoundingRadiusQ12 * 2,
                                  g_GeneratedTextureScratchRuntime.samples[2].worldPoint.z,
                                  g_GeneratedTextureScratchRuntime.samples[2].worldPoint.y,
                                  g_GeneratedTextureScratchRuntime.samples[2].worldPoint.x,
                                  renderContext->fieldGrid);
              if (!terrainRay.hit) {
                return;
              }
              direction = FixedMath_DirectionFromAnglesScaledRegs
                                 (renderContext->viewAngleBC,renderContext->viewAngleB8,
                                  terrainRay.distanceQ12);
              g_GeneratedTextureScratchRuntime.samples[2].worldPoint.x =
                   g_GeneratedTextureScratchRuntime.samples[2].worldPoint.x + direction.x;
              g_GeneratedTextureScratchRuntime.samples[2].worldPoint.y =
                   g_GeneratedTextureScratchRuntime.samples[2].worldPoint.y + direction.y;
              g_GeneratedTextureScratchRuntime.samples[2].worldPoint.z =
                   g_GeneratedTextureScratchRuntime.samples[2].worldPoint.z + direction.z;
            }
            else {
              g_GeneratedTextureScratchRuntime.samples[2].worldPoint.z =
                   g_GeneratedTextureScratchRuntime.samples[2].worldPoint.z + heightDeltaOrIntensityA;
            }
          }
          sampleWorldZ = g_GeneratedTextureScratchRuntime.samples[1].worldPoint.z;
          surfaceHeight = FieldGrid_InterpolateTopSurfaceHeight
                             (g_GeneratedTextureScratchRuntime.samples[1].worldPoint.y,
                              g_GeneratedTextureScratchRuntime.samples[1].worldPoint.x,
                              renderContext->fieldGrid);
          planeDotOrTextureOffset = g_GeneratedTextureScratchRuntime.samples[1].textureCoordinateOffsetQ20;
          heightDeltaOrIntensityA = surfaceHeight.heightQ12 - sampleWorldZ;
          if (surfaceHeight.heightQ12 < sampleWorldZ) {
            terrainRay = FieldGrid_RaycastTerrainTrianglesAlongDirection
                               (renderContext->viewAngleBC,renderContext->viewAngleB8,
                                modelNode->subtreeBoundingRadiusQ12 * 2,
                                g_GeneratedTextureScratchRuntime.samples[1].worldPoint.z,
                                g_GeneratedTextureScratchRuntime.samples[1].worldPoint.y,
                                g_GeneratedTextureScratchRuntime.samples[1].worldPoint.x,
                                renderContext->fieldGrid);
            if (terrainRay.hit) {
              g_GeneratedTextureScratchRuntime.samples[1].terrainRayDistanceQ12 =
                   terrainRay.distanceQ12;
              direction = FixedMath_DirectionFromAnglesScaledRegs
                                 (renderContext->viewAngleBC,renderContext->viewAngleB8,
                                  terrainRay.distanceQ12);
              g_GeneratedTextureScratchRuntime.samples[1].worldPoint.x =
                   g_GeneratedTextureScratchRuntime.samples[1].worldPoint.x + direction.x;
              g_GeneratedTextureScratchRuntime.samples[1].worldPoint.y =
                   g_GeneratedTextureScratchRuntime.samples[1].worldPoint.y + direction.y;
              g_GeneratedTextureScratchRuntime.samples[1].worldPoint.z =
                   g_GeneratedTextureScratchRuntime.samples[1].worldPoint.z + direction.z;
            }
            else {
              radiusScaleOrCoordinate = modelNode->subtreeBoundingRadiusQ12 * 2;
              if (g_GraphicsShadingGridStepQ20Current -
                  g_GeneratedTextureScratchRuntime.samples[1].textureCoordinateOffsetQ20 == 0) {
                return;
              }
              remainingRayLength = (Q12)(((int64_t)
                              (int)(g_GraphicsShadingGridStepQ20Current -
                                   g_GeneratedTextureScratchRuntime.samples[1].
                                   textureCoordinateOffsetQ20) * (int64_t)radiusScaleOrCoordinate) /
                            (int64_t)(int)g_GraphicsShadingGridStepQ20Current);
              g_GeneratedTextureScratchRuntime.samples[1].terrainRayDistanceQ12 = radiusScaleOrCoordinate;
              direction = FixedMath_DirectionFromAnglesScaledRegs
                                 (renderContext->viewAngleBC,renderContext->viewAngleB8,radiusScaleOrCoordinate);
              g_GeneratedTextureScratchRuntime.samples[1].worldPoint.x =
                   g_GeneratedTextureScratchRuntime.samples[1].worldPoint.x + direction.x;
              g_GeneratedTextureScratchRuntime.samples[1].worldPoint.y =
                   g_GeneratedTextureScratchRuntime.samples[1].worldPoint.y + direction.y;
              g_GeneratedTextureScratchRuntime.samples[1].worldPoint.z =
                   g_GeneratedTextureScratchRuntime.samples[1].worldPoint.z + direction.z;
              heightDeltaOrIntensityA = renderContext->viewAngleB8 ^ FIXED_ANGLE16_HALF_TURN;
              terrainRay = FieldGrid_RaycastTerrainSurfaceDistance
                                 (-renderContext->viewAngleBC - FIXED_ANGLE16_QUARTER_TURN,heightDeltaOrIntensityA,remainingRayLength,
                                  g_GeneratedTextureScratchRuntime.samples[1].worldPoint.z,
                                  g_GeneratedTextureScratchRuntime.samples[1].worldPoint.y,
                                  g_GeneratedTextureScratchRuntime.samples[1].worldPoint.x,
                                  renderContext->fieldGrid);
              surfaceDistanceQ12 = terrainRay.distanceQ12;
              if (!terrainRay.hit) {
                return;
              }
              direction = FixedMath_DirectionFromAnglesScaledRegs
                                 (terrainRay.materialOrCellIndex,heightDeltaOrIntensityA,surfaceDistanceQ12); /* quirk: EDX */
              g_GeneratedTextureScratchRuntime.samples[1].worldPoint.x =
                   g_GeneratedTextureScratchRuntime.samples[1].worldPoint.x + direction.x;
              g_GeneratedTextureScratchRuntime.samples[1].worldPoint.y =
                   g_GeneratedTextureScratchRuntime.samples[1].worldPoint.y + direction.y;
              g_GeneratedTextureScratchRuntime.samples[1].worldPoint.z =
                   g_GeneratedTextureScratchRuntime.samples[1].worldPoint.z + direction.z;
              g_GeneratedTextureScratchRuntime.samples[1].textureCoordinateOffsetQ20 =
                   g_GeneratedTextureScratchRuntime.samples[1].textureCoordinateOffsetQ20 +
                   ((int)(((int64_t)surfaceDistanceQ12 * (int64_t)(int)g_GraphicsShadingGridStepQ20Current) /
                         (int64_t)modelNode->subtreeBoundingRadiusQ12) >> 1);
              if ((int)g_GraphicsShadingGridStepQ20Current <
                  g_GeneratedTextureScratchRuntime.samples[1].textureCoordinateOffsetQ20) {
                return;
              }
            }
          }
          else {
            if ((uint32_t)modelNode->subtreeBoundingRadiusQ12 <= heightDeltaOrIntensityA) {
              heightDeltaOrIntensityA = modelNode->subtreeBoundingRadiusQ12;
            }
            g_GeneratedTextureScratchRuntime.samples[1].terrainRayDistanceQ12 = 0;
            radiusScaleOrCoordinate = (int)(((int64_t)(int)g_GraphicsShadingGridStepQ20Current *
                           (int64_t)
                           (int)((int)((uint64_t)
                                       ((int64_t)(int)heightDeltaOrIntensityA *
                                       (int64_t)g_FixedCosQ28[renderContext->viewAngleBC]) >> 0x20)
                                 << 3 | (uint32_t)((int64_t)(int)heightDeltaOrIntensityA *
                                              (int64_t)g_FixedCosQ28[renderContext->viewAngleBC])
                                        >> 0x1d)) / (int64_t)modelNode->subtreeBoundingRadiusQ12);
            g_GeneratedTextureScratchRuntime.samples[1].textureCoordinateOffsetQ20 =
                 g_GeneratedTextureScratchRuntime.samples[1].textureCoordinateOffsetQ20 - radiusScaleOrCoordinate;
            if (planeDotOrTextureOffset < radiusScaleOrCoordinate) {
              g_GeneratedTextureScratchRuntime.samples[1].textureCoordinateOffsetQ20 = 0;
              direction = FixedMath_DirectionFromAnglesScaledRegs
                                 (renderContext->viewAngleBC,renderContext->viewAngleB8,
                                  modelNode->subtreeBoundingRadiusQ12 * 2);
              g_GeneratedTextureScratchRuntime.samples[1].worldPoint.x =
                   g_GeneratedTextureScratchRuntime.samples[1].worldPoint.x - direction.x;
              g_GeneratedTextureScratchRuntime.samples[1].worldPoint.y =
                   g_GeneratedTextureScratchRuntime.samples[1].worldPoint.y - direction.y;
              g_GeneratedTextureScratchRuntime.samples[1].worldPoint.z =
                   g_GeneratedTextureScratchRuntime.samples[1].worldPoint.z - direction.z;
              terrainRay = FieldGrid_RaycastTerrainTrianglesAlongDirection
                                 (renderContext->viewAngleBC,renderContext->viewAngleB8,
                                  modelNode->subtreeBoundingRadiusQ12 * 2,
                                  g_GeneratedTextureScratchRuntime.samples[1].worldPoint.z,
                                  g_GeneratedTextureScratchRuntime.samples[1].worldPoint.y,
                                  g_GeneratedTextureScratchRuntime.samples[1].worldPoint.x,
                                  renderContext->fieldGrid);
              if (!terrainRay.hit) {
                return;
              }
              direction = FixedMath_DirectionFromAnglesScaledRegs
                                 (renderContext->viewAngleBC,renderContext->viewAngleB8,
                                  terrainRay.distanceQ12);
              g_GeneratedTextureScratchRuntime.samples[1].worldPoint.x =
                   g_GeneratedTextureScratchRuntime.samples[1].worldPoint.x + direction.x;
              g_GeneratedTextureScratchRuntime.samples[1].worldPoint.y =
                   g_GeneratedTextureScratchRuntime.samples[1].worldPoint.y + direction.y;
              g_GeneratedTextureScratchRuntime.samples[1].worldPoint.z =
                   g_GeneratedTextureScratchRuntime.samples[1].worldPoint.z + direction.z;
            }
            else {
              g_GeneratedTextureScratchRuntime.samples[1].worldPoint.z =
                   g_GeneratedTextureScratchRuntime.samples[1].worldPoint.z + heightDeltaOrIntensityA;
            }
          }
          sampleWorldZ = g_GeneratedTextureScratchRuntime.samples[3].worldPoint.z;
          surfaceHeight = FieldGrid_InterpolateTopSurfaceHeight
                             (g_GeneratedTextureScratchRuntime.samples[3].worldPoint.y,
                              g_GeneratedTextureScratchRuntime.samples[3].worldPoint.x,
                              renderContext->fieldGrid);
          planeDotOrTextureOffset = g_GeneratedTextureScratchRuntime.samples[3].textureCoordinateOffsetQ20;
          heightDeltaOrIntensityA = surfaceHeight.heightQ12 - sampleWorldZ;
          if (surfaceHeight.heightQ12 < sampleWorldZ) {
            terrainRay = FieldGrid_RaycastTerrainTrianglesAlongDirection
                               (renderContext->viewAngleBC,renderContext->viewAngleB8,
                                modelNode->subtreeBoundingRadiusQ12 * 2,
                                g_GeneratedTextureScratchRuntime.samples[3].worldPoint.z,
                                g_GeneratedTextureScratchRuntime.samples[3].worldPoint.y,
                                g_GeneratedTextureScratchRuntime.samples[3].worldPoint.x,
                                renderContext->fieldGrid);
            if (terrainRay.hit) {
              g_GeneratedTextureScratchRuntime.samples[3].terrainRayDistanceQ12 =
                   terrainRay.distanceQ12;
              direction = FixedMath_DirectionFromAnglesScaledRegs
                                 (renderContext->viewAngleBC,renderContext->viewAngleB8,
                                  terrainRay.distanceQ12);
              g_GeneratedTextureScratchRuntime.samples[3].worldPoint.x =
                   g_GeneratedTextureScratchRuntime.samples[3].worldPoint.x + direction.x;
              g_GeneratedTextureScratchRuntime.samples[3].worldPoint.y =
                   g_GeneratedTextureScratchRuntime.samples[3].worldPoint.y + direction.y;
              g_GeneratedTextureScratchRuntime.samples[3].worldPoint.z =
                   g_GeneratedTextureScratchRuntime.samples[3].worldPoint.z + direction.z;
            }
            else {
              radiusScaleOrCoordinate = modelNode->subtreeBoundingRadiusQ12 * 2;
              if (g_GraphicsShadingGridStepQ20Current -
                  g_GeneratedTextureScratchRuntime.samples[3].textureCoordinateOffsetQ20 == 0) {
                return;
              }
              remainingRayLength = (Q12)(((int64_t)
                              (int)(g_GraphicsShadingGridStepQ20Current -
                                   g_GeneratedTextureScratchRuntime.samples[3].
                                   textureCoordinateOffsetQ20) * (int64_t)radiusScaleOrCoordinate) /
                            (int64_t)(int)g_GraphicsShadingGridStepQ20Current);
              g_GeneratedTextureScratchRuntime.samples[3].terrainRayDistanceQ12 = radiusScaleOrCoordinate;
              direction = FixedMath_DirectionFromAnglesScaledRegs
                                 (renderContext->viewAngleBC,renderContext->viewAngleB8,radiusScaleOrCoordinate);
              g_GeneratedTextureScratchRuntime.samples[3].worldPoint.x =
                   g_GeneratedTextureScratchRuntime.samples[3].worldPoint.x + direction.x;
              g_GeneratedTextureScratchRuntime.samples[3].worldPoint.y =
                   g_GeneratedTextureScratchRuntime.samples[3].worldPoint.y + direction.y;
              g_GeneratedTextureScratchRuntime.samples[3].worldPoint.z =
                   g_GeneratedTextureScratchRuntime.samples[3].worldPoint.z + direction.z;
              heightDeltaOrIntensityA = renderContext->viewAngleB8 ^ FIXED_ANGLE16_HALF_TURN;
              terrainRay = FieldGrid_RaycastTerrainSurfaceDistance
                                 (-renderContext->viewAngleBC - FIXED_ANGLE16_QUARTER_TURN,heightDeltaOrIntensityA,remainingRayLength,
                                  g_GeneratedTextureScratchRuntime.samples[3].worldPoint.z,
                                  g_GeneratedTextureScratchRuntime.samples[3].worldPoint.y,
                                  g_GeneratedTextureScratchRuntime.samples[3].worldPoint.x,
                                  renderContext->fieldGrid);
              surfaceDistanceQ12 = terrainRay.distanceQ12;
              if (!terrainRay.hit) {
                return;
              }
              direction = FixedMath_DirectionFromAnglesScaledRegs
                                 (terrainRay.materialOrCellIndex,heightDeltaOrIntensityA,surfaceDistanceQ12); /* quirk: EDX */
              g_GeneratedTextureScratchRuntime.samples[3].worldPoint.x =
                   g_GeneratedTextureScratchRuntime.samples[3].worldPoint.x + direction.x;
              g_GeneratedTextureScratchRuntime.samples[3].worldPoint.y =
                   g_GeneratedTextureScratchRuntime.samples[3].worldPoint.y + direction.y;
              g_GeneratedTextureScratchRuntime.samples[3].worldPoint.z =
                   g_GeneratedTextureScratchRuntime.samples[3].worldPoint.z + direction.z;
              g_GeneratedTextureScratchRuntime.samples[3].textureCoordinateOffsetQ20 =
                   g_GeneratedTextureScratchRuntime.samples[3].textureCoordinateOffsetQ20 +
                   ((int)(((int64_t)surfaceDistanceQ12 * (int64_t)(int)g_GraphicsShadingGridStepQ20Current) /
                         (int64_t)modelNode->subtreeBoundingRadiusQ12) >> 1);
              if ((int)g_GraphicsShadingGridStepQ20Current <
                  g_GeneratedTextureScratchRuntime.samples[3].textureCoordinateOffsetQ20) {
                return;
              }
            }
          }
          else {
            if ((uint32_t)modelNode->subtreeBoundingRadiusQ12 <= heightDeltaOrIntensityA) {
              heightDeltaOrIntensityA = modelNode->subtreeBoundingRadiusQ12;
            }
            g_GeneratedTextureScratchRuntime.samples[3].terrainRayDistanceQ12 = 0;
            radiusScaleOrCoordinate = (int)(((int64_t)(int)g_GraphicsShadingGridStepQ20Current *
                           (int64_t)
                           (int)((int)((uint64_t)
                                       ((int64_t)(int)heightDeltaOrIntensityA *
                                       (int64_t)g_FixedCosQ28[renderContext->viewAngleBC]) >> 0x20)
                                 << 3 | (uint32_t)((int64_t)(int)heightDeltaOrIntensityA *
                                              (int64_t)g_FixedCosQ28[renderContext->viewAngleBC])
                                        >> 0x1d)) / (int64_t)modelNode->subtreeBoundingRadiusQ12);
            g_GeneratedTextureScratchRuntime.samples[3].textureCoordinateOffsetQ20 =
                 g_GeneratedTextureScratchRuntime.samples[3].textureCoordinateOffsetQ20 - radiusScaleOrCoordinate;
            if (planeDotOrTextureOffset < radiusScaleOrCoordinate) {
              g_GeneratedTextureScratchRuntime.samples[3].textureCoordinateOffsetQ20 = 0;
              direction = FixedMath_DirectionFromAnglesScaledRegs
                                 (renderContext->viewAngleBC,renderContext->viewAngleB8,
                                  modelNode->subtreeBoundingRadiusQ12 * 2);
              g_GeneratedTextureScratchRuntime.samples[3].worldPoint.x =
                   g_GeneratedTextureScratchRuntime.samples[3].worldPoint.x - direction.x;
              g_GeneratedTextureScratchRuntime.samples[3].worldPoint.y =
                   g_GeneratedTextureScratchRuntime.samples[3].worldPoint.y - direction.y;
              g_GeneratedTextureScratchRuntime.samples[3].worldPoint.z =
                   g_GeneratedTextureScratchRuntime.samples[3].worldPoint.z - direction.z;
              terrainRay = FieldGrid_RaycastTerrainTrianglesAlongDirection
                                 (renderContext->viewAngleBC,renderContext->viewAngleB8,
                                  modelNode->subtreeBoundingRadiusQ12 * 2,
                                  g_GeneratedTextureScratchRuntime.samples[3].worldPoint.z,
                                  g_GeneratedTextureScratchRuntime.samples[3].worldPoint.y,
                                  g_GeneratedTextureScratchRuntime.samples[3].worldPoint.x,
                                  renderContext->fieldGrid);
              if (!terrainRay.hit) {
                return;
              }
              direction = FixedMath_DirectionFromAnglesScaledRegs
                                 (renderContext->viewAngleBC,renderContext->viewAngleB8,
                                  terrainRay.distanceQ12);
              g_GeneratedTextureScratchRuntime.samples[3].worldPoint.x =
                   g_GeneratedTextureScratchRuntime.samples[3].worldPoint.x + direction.x;
              g_GeneratedTextureScratchRuntime.samples[3].worldPoint.y =
                   g_GeneratedTextureScratchRuntime.samples[3].worldPoint.y + direction.y;
              g_GeneratedTextureScratchRuntime.samples[3].worldPoint.z =
                   g_GeneratedTextureScratchRuntime.samples[3].worldPoint.z + direction.z;
            }
            else {
              g_GeneratedTextureScratchRuntime.samples[3].worldPoint.z =
                   g_GeneratedTextureScratchRuntime.samples[3].worldPoint.z + heightDeltaOrIntensityA;
            }
          }
          sampleWorldZ = g_GeneratedTextureScratchRuntime.samples[4].worldPoint.z;
          surfaceHeight = FieldGrid_InterpolateTopSurfaceHeight
                             (g_GeneratedTextureScratchRuntime.samples[4].worldPoint.y,
                              g_GeneratedTextureScratchRuntime.samples[4].worldPoint.x,
                              renderContext->fieldGrid);
          planeDotOrTextureOffset = g_GeneratedTextureScratchRuntime.samples[4].textureCoordinateOffsetQ20;
          heightDeltaOrIntensityA = surfaceHeight.heightQ12 - sampleWorldZ;
          if (surfaceHeight.heightQ12 < sampleWorldZ) {
            terrainRay = FieldGrid_RaycastTerrainTrianglesAlongDirection
                               (renderContext->viewAngleBC,renderContext->viewAngleB8,
                                modelNode->subtreeBoundingRadiusQ12 * 2,
                                g_GeneratedTextureScratchRuntime.samples[4].worldPoint.z,
                                g_GeneratedTextureScratchRuntime.samples[4].worldPoint.y,
                                g_GeneratedTextureScratchRuntime.samples[4].worldPoint.x,
                                renderContext->fieldGrid);
            if (terrainRay.hit) {
              g_GeneratedTextureScratchRuntime.samples[4].terrainRayDistanceQ12 =
                   terrainRay.distanceQ12;
              direction = FixedMath_DirectionFromAnglesScaledRegs
                                 (renderContext->viewAngleBC,renderContext->viewAngleB8,
                                  terrainRay.distanceQ12);
              g_GeneratedTextureScratchRuntime.samples[4].worldPoint.x =
                   g_GeneratedTextureScratchRuntime.samples[4].worldPoint.x + direction.x;
              g_GeneratedTextureScratchRuntime.samples[4].worldPoint.y =
                   g_GeneratedTextureScratchRuntime.samples[4].worldPoint.y + direction.y;
              g_GeneratedTextureScratchRuntime.samples[4].worldPoint.z =
                   g_GeneratedTextureScratchRuntime.samples[4].worldPoint.z + direction.z;
            }
            else {
              radiusScaleOrCoordinate = modelNode->subtreeBoundingRadiusQ12 * 2;
              if (g_GraphicsShadingGridStepQ20Current -
                  g_GeneratedTextureScratchRuntime.samples[4].textureCoordinateOffsetQ20 == 0) {
                return;
              }
              remainingRayLength = (Q12)(((int64_t)
                              (int)(g_GraphicsShadingGridStepQ20Current -
                                   g_GeneratedTextureScratchRuntime.samples[4].
                                   textureCoordinateOffsetQ20) * (int64_t)radiusScaleOrCoordinate) /
                            (int64_t)(int)g_GraphicsShadingGridStepQ20Current);
              g_GeneratedTextureScratchRuntime.samples[4].terrainRayDistanceQ12 = radiusScaleOrCoordinate;
              direction = FixedMath_DirectionFromAnglesScaledRegs
                                 (renderContext->viewAngleBC,renderContext->viewAngleB8,radiusScaleOrCoordinate);
              g_GeneratedTextureScratchRuntime.samples[4].worldPoint.x =
                   g_GeneratedTextureScratchRuntime.samples[4].worldPoint.x + direction.x;
              g_GeneratedTextureScratchRuntime.samples[4].worldPoint.y =
                   g_GeneratedTextureScratchRuntime.samples[4].worldPoint.y + direction.y;
              g_GeneratedTextureScratchRuntime.samples[4].worldPoint.z =
                   g_GeneratedTextureScratchRuntime.samples[4].worldPoint.z + direction.z;
              heightDeltaOrIntensityA = renderContext->viewAngleB8 ^ FIXED_ANGLE16_HALF_TURN;
              terrainRay = FieldGrid_RaycastTerrainSurfaceDistance
                                 (-renderContext->viewAngleBC - FIXED_ANGLE16_QUARTER_TURN,heightDeltaOrIntensityA,remainingRayLength,
                                  g_GeneratedTextureScratchRuntime.samples[4].worldPoint.z,
                                  g_GeneratedTextureScratchRuntime.samples[4].worldPoint.y,
                                  g_GeneratedTextureScratchRuntime.samples[4].worldPoint.x,
                                  renderContext->fieldGrid);
              surfaceDistanceQ12 = terrainRay.distanceQ12;
              if (!terrainRay.hit) {
                return;
              }
              direction = FixedMath_DirectionFromAnglesScaledRegs
                                 (terrainRay.materialOrCellIndex,heightDeltaOrIntensityA,surfaceDistanceQ12); /* quirk: EDX */
              g_GeneratedTextureScratchRuntime.samples[4].worldPoint.x =
                   g_GeneratedTextureScratchRuntime.samples[4].worldPoint.x + direction.x;
              g_GeneratedTextureScratchRuntime.samples[4].worldPoint.y =
                   g_GeneratedTextureScratchRuntime.samples[4].worldPoint.y + direction.y;
              g_GeneratedTextureScratchRuntime.samples[4].worldPoint.z =
                   g_GeneratedTextureScratchRuntime.samples[4].worldPoint.z + direction.z;
              g_GeneratedTextureScratchRuntime.samples[4].textureCoordinateOffsetQ20 =
                   g_GeneratedTextureScratchRuntime.samples[4].textureCoordinateOffsetQ20 +
                   ((int)(((int64_t)surfaceDistanceQ12 * (int64_t)(int)g_GraphicsShadingGridStepQ20Current) /
                         (int64_t)modelNode->subtreeBoundingRadiusQ12) >> 1);
              if ((int)g_GraphicsShadingGridStepQ20Current <
                  g_GeneratedTextureScratchRuntime.samples[4].textureCoordinateOffsetQ20) {
                return;
              }
            }
          }
          else {
            if ((uint32_t)modelNode->subtreeBoundingRadiusQ12 <= heightDeltaOrIntensityA) {
              heightDeltaOrIntensityA = modelNode->subtreeBoundingRadiusQ12;
            }
            g_GeneratedTextureScratchRuntime.samples[4].terrainRayDistanceQ12 = 0;
            radiusScaleOrCoordinate = (int)(((int64_t)(int)g_GraphicsShadingGridStepQ20Current *
                           (int64_t)
                           (int)((int)((uint64_t)
                                       ((int64_t)(int)heightDeltaOrIntensityA *
                                       (int64_t)g_FixedCosQ28[renderContext->viewAngleBC]) >> 0x20)
                                 << 3 | (uint32_t)((int64_t)(int)heightDeltaOrIntensityA *
                                              (int64_t)g_FixedCosQ28[renderContext->viewAngleBC])
                                        >> 0x1d)) / (int64_t)modelNode->subtreeBoundingRadiusQ12);
            g_GeneratedTextureScratchRuntime.samples[4].textureCoordinateOffsetQ20 =
                 g_GeneratedTextureScratchRuntime.samples[4].textureCoordinateOffsetQ20 - radiusScaleOrCoordinate;
            if (planeDotOrTextureOffset < radiusScaleOrCoordinate) {
              g_GeneratedTextureScratchRuntime.samples[4].textureCoordinateOffsetQ20 = 0;
              direction = FixedMath_DirectionFromAnglesScaledRegs
                                 (renderContext->viewAngleBC,renderContext->viewAngleB8,
                                  modelNode->subtreeBoundingRadiusQ12 * 2);
              g_GeneratedTextureScratchRuntime.samples[4].worldPoint.x =
                   g_GeneratedTextureScratchRuntime.samples[4].worldPoint.x - direction.x;
              g_GeneratedTextureScratchRuntime.samples[4].worldPoint.y =
                   g_GeneratedTextureScratchRuntime.samples[4].worldPoint.y - direction.y;
              g_GeneratedTextureScratchRuntime.samples[4].worldPoint.z =
                   g_GeneratedTextureScratchRuntime.samples[4].worldPoint.z - direction.z;
              terrainRay = FieldGrid_RaycastTerrainTrianglesAlongDirection
                                 (renderContext->viewAngleBC,renderContext->viewAngleB8,
                                  modelNode->subtreeBoundingRadiusQ12 * 2,
                                  g_GeneratedTextureScratchRuntime.samples[4].worldPoint.z,
                                  g_GeneratedTextureScratchRuntime.samples[4].worldPoint.y,
                                  g_GeneratedTextureScratchRuntime.samples[4].worldPoint.x,
                                  renderContext->fieldGrid);
              if (!terrainRay.hit) {
                return;
              }
              direction = FixedMath_DirectionFromAnglesScaledRegs
                                 (renderContext->viewAngleBC,renderContext->viewAngleB8,
                                  terrainRay.distanceQ12);
              g_GeneratedTextureScratchRuntime.samples[4].worldPoint.x =
                   g_GeneratedTextureScratchRuntime.samples[4].worldPoint.x + direction.x;
              g_GeneratedTextureScratchRuntime.samples[4].worldPoint.y =
                   g_GeneratedTextureScratchRuntime.samples[4].worldPoint.y + direction.y;
              g_GeneratedTextureScratchRuntime.samples[4].worldPoint.z =
                   g_GeneratedTextureScratchRuntime.samples[4].worldPoint.z + direction.z;
            }
            else {
              g_GeneratedTextureScratchRuntime.samples[4].worldPoint.z =
                   g_GeneratedTextureScratchRuntime.samples[4].worldPoint.z + heightDeltaOrIntensityA;
            }
          }
          sampleWorldZ = g_GeneratedTextureScratchRuntime.samples[5].worldPoint.z;
          surfaceHeight = FieldGrid_InterpolateTopSurfaceHeight
                             (g_GeneratedTextureScratchRuntime.samples[5].worldPoint.y,
                              g_GeneratedTextureScratchRuntime.samples[5].worldPoint.x,
                              renderContext->fieldGrid);
          planeDotOrTextureOffset = g_GeneratedTextureScratchRuntime.samples[5].textureCoordinateOffsetQ20;
          heightDeltaOrIntensityA = surfaceHeight.heightQ12 - sampleWorldZ;
          if (surfaceHeight.heightQ12 < sampleWorldZ) {
            terrainRay = FieldGrid_RaycastTerrainTrianglesAlongDirection
                               (renderContext->viewAngleBC,renderContext->viewAngleB8,
                                modelNode->subtreeBoundingRadiusQ12 * 2,
                                g_GeneratedTextureScratchRuntime.samples[5].worldPoint.z,
                                g_GeneratedTextureScratchRuntime.samples[5].worldPoint.y,
                                g_GeneratedTextureScratchRuntime.samples[5].worldPoint.x,
                                renderContext->fieldGrid);
            if (terrainRay.hit) {
              g_GeneratedTextureScratchRuntime.samples[5].terrainRayDistanceQ12 =
                   terrainRay.distanceQ12;
              direction = FixedMath_DirectionFromAnglesScaledRegs
                                 (renderContext->viewAngleBC,renderContext->viewAngleB8,
                                  terrainRay.distanceQ12);
              g_GeneratedTextureScratchRuntime.samples[5].worldPoint.x =
                   g_GeneratedTextureScratchRuntime.samples[5].worldPoint.x + direction.x;
              g_GeneratedTextureScratchRuntime.samples[5].worldPoint.y =
                   g_GeneratedTextureScratchRuntime.samples[5].worldPoint.y + direction.y;
              g_GeneratedTextureScratchRuntime.samples[5].worldPoint.z =
                   g_GeneratedTextureScratchRuntime.samples[5].worldPoint.z + direction.z;
            }
            else {
              radiusScaleOrCoordinate = modelNode->subtreeBoundingRadiusQ12 * 2;
              if (g_GraphicsShadingGridStepQ20Current -
                  g_GeneratedTextureScratchRuntime.samples[5].textureCoordinateOffsetQ20 == 0) {
                return;
              }
              remainingRayLength = (Q12)(((int64_t)
                              (int)(g_GraphicsShadingGridStepQ20Current -
                                   g_GeneratedTextureScratchRuntime.samples[5].
                                   textureCoordinateOffsetQ20) * (int64_t)radiusScaleOrCoordinate) /
                            (int64_t)(int)g_GraphicsShadingGridStepQ20Current);
              g_GeneratedTextureScratchRuntime.samples[5].terrainRayDistanceQ12 = radiusScaleOrCoordinate;
              direction = FixedMath_DirectionFromAnglesScaledRegs
                                 (renderContext->viewAngleBC,renderContext->viewAngleB8,radiusScaleOrCoordinate);
              g_GeneratedTextureScratchRuntime.samples[5].worldPoint.x =
                   g_GeneratedTextureScratchRuntime.samples[5].worldPoint.x + direction.x;
              g_GeneratedTextureScratchRuntime.samples[5].worldPoint.y =
                   g_GeneratedTextureScratchRuntime.samples[5].worldPoint.y + direction.y;
              g_GeneratedTextureScratchRuntime.samples[5].worldPoint.z =
                   g_GeneratedTextureScratchRuntime.samples[5].worldPoint.z + direction.z;
              heightDeltaOrIntensityA = renderContext->viewAngleB8 ^ FIXED_ANGLE16_HALF_TURN;
              terrainRay = FieldGrid_RaycastTerrainSurfaceDistance
                                 (-renderContext->viewAngleBC - FIXED_ANGLE16_QUARTER_TURN,heightDeltaOrIntensityA,remainingRayLength,
                                  g_GeneratedTextureScratchRuntime.samples[5].worldPoint.z,
                                  g_GeneratedTextureScratchRuntime.samples[5].worldPoint.y,
                                  g_GeneratedTextureScratchRuntime.samples[5].worldPoint.x,
                                  renderContext->fieldGrid);
              surfaceDistanceQ12 = terrainRay.distanceQ12;
              if (!terrainRay.hit) {
                return;
              }
              direction = FixedMath_DirectionFromAnglesScaledRegs
                                 (terrainRay.materialOrCellIndex,heightDeltaOrIntensityA,surfaceDistanceQ12); /* quirk: EDX */
              g_GeneratedTextureScratchRuntime.samples[5].worldPoint.x =
                   g_GeneratedTextureScratchRuntime.samples[5].worldPoint.x + direction.x;
              g_GeneratedTextureScratchRuntime.samples[5].worldPoint.y =
                   g_GeneratedTextureScratchRuntime.samples[5].worldPoint.y + direction.y;
              g_GeneratedTextureScratchRuntime.samples[5].worldPoint.z =
                   g_GeneratedTextureScratchRuntime.samples[5].worldPoint.z + direction.z;
              g_GeneratedTextureScratchRuntime.samples[5].textureCoordinateOffsetQ20 =
                   g_GeneratedTextureScratchRuntime.samples[5].textureCoordinateOffsetQ20 +
                   ((int)(((int64_t)surfaceDistanceQ12 * (int64_t)(int)g_GraphicsShadingGridStepQ20Current) /
                         (int64_t)modelNode->subtreeBoundingRadiusQ12) >> 1);
              if ((int)g_GraphicsShadingGridStepQ20Current <
                  g_GeneratedTextureScratchRuntime.samples[5].textureCoordinateOffsetQ20) {
                return;
              }
            }
          }
          else {
            if ((uint32_t)modelNode->subtreeBoundingRadiusQ12 <= heightDeltaOrIntensityA) {
              heightDeltaOrIntensityA = modelNode->subtreeBoundingRadiusQ12;
            }
            g_GeneratedTextureScratchRuntime.samples[5].terrainRayDistanceQ12 = 0;
            radiusScaleOrCoordinate = (int)(((int64_t)(int)g_GraphicsShadingGridStepQ20Current *
                           (int64_t)
                           (int)((int)((uint64_t)
                                       ((int64_t)(int)heightDeltaOrIntensityA *
                                       (int64_t)g_FixedCosQ28[renderContext->viewAngleBC]) >> 0x20)
                                 << 3 | (uint32_t)((int64_t)(int)heightDeltaOrIntensityA *
                                              (int64_t)g_FixedCosQ28[renderContext->viewAngleBC])
                                        >> 0x1d)) / (int64_t)modelNode->subtreeBoundingRadiusQ12);
            g_GeneratedTextureScratchRuntime.samples[5].textureCoordinateOffsetQ20 =
                 g_GeneratedTextureScratchRuntime.samples[5].textureCoordinateOffsetQ20 - radiusScaleOrCoordinate;
            if (planeDotOrTextureOffset < radiusScaleOrCoordinate) {
              g_GeneratedTextureScratchRuntime.samples[5].textureCoordinateOffsetQ20 = 0;
              direction = FixedMath_DirectionFromAnglesScaledRegs
                                 (renderContext->viewAngleBC,renderContext->viewAngleB8,
                                  modelNode->subtreeBoundingRadiusQ12 * 2);
              g_GeneratedTextureScratchRuntime.samples[5].worldPoint.x =
                   g_GeneratedTextureScratchRuntime.samples[5].worldPoint.x - direction.x;
              g_GeneratedTextureScratchRuntime.samples[5].worldPoint.y =
                   g_GeneratedTextureScratchRuntime.samples[5].worldPoint.y - direction.y;
              g_GeneratedTextureScratchRuntime.samples[5].worldPoint.z =
                   g_GeneratedTextureScratchRuntime.samples[5].worldPoint.z - direction.z;
              terrainRay = FieldGrid_RaycastTerrainTrianglesAlongDirection
                                 (renderContext->viewAngleBC,renderContext->viewAngleB8,
                                  modelNode->subtreeBoundingRadiusQ12 * 2,
                                  g_GeneratedTextureScratchRuntime.samples[5].worldPoint.z,
                                  g_GeneratedTextureScratchRuntime.samples[5].worldPoint.y,
                                  g_GeneratedTextureScratchRuntime.samples[5].worldPoint.x,
                                  renderContext->fieldGrid);
              if (!terrainRay.hit) {
                return;
              }
              direction = FixedMath_DirectionFromAnglesScaledRegs
                                 (renderContext->viewAngleBC,renderContext->viewAngleB8,
                                  terrainRay.distanceQ12);
              g_GeneratedTextureScratchRuntime.samples[5].worldPoint.x =
                   g_GeneratedTextureScratchRuntime.samples[5].worldPoint.x + direction.x;
              g_GeneratedTextureScratchRuntime.samples[5].worldPoint.y =
                   g_GeneratedTextureScratchRuntime.samples[5].worldPoint.y + direction.y;
              g_GeneratedTextureScratchRuntime.samples[5].worldPoint.z =
                   g_GeneratedTextureScratchRuntime.samples[5].worldPoint.z + direction.z;
            }
            else {
              g_GeneratedTextureScratchRuntime.samples[5].worldPoint.z =
                   g_GeneratedTextureScratchRuntime.samples[5].worldPoint.z + heightDeltaOrIntensityA;
            }
          }
          sampleWorldZ = g_GeneratedTextureScratchRuntime.samples[6].worldPoint.z;
          surfaceHeight = FieldGrid_InterpolateTopSurfaceHeight
                             (g_GeneratedTextureScratchRuntime.samples[6].worldPoint.y,
                              g_GeneratedTextureScratchRuntime.samples[6].worldPoint.x,
                              renderContext->fieldGrid);
          planeDotOrTextureOffset = g_GeneratedTextureScratchRuntime.samples[6].textureCoordinateOffsetQ20;
          heightDeltaOrIntensityA = surfaceHeight.heightQ12 - sampleWorldZ;
          if (surfaceHeight.heightQ12 < sampleWorldZ) {
            terrainRay = FieldGrid_RaycastTerrainTrianglesAlongDirection
                               (renderContext->viewAngleBC,renderContext->viewAngleB8,
                                modelNode->subtreeBoundingRadiusQ12 * 2,
                                g_GeneratedTextureScratchRuntime.samples[6].worldPoint.z,
                                g_GeneratedTextureScratchRuntime.samples[6].worldPoint.y,
                                g_GeneratedTextureScratchRuntime.samples[6].worldPoint.x,
                                renderContext->fieldGrid);
            if (terrainRay.hit) {
              g_GeneratedTextureScratchRuntime.samples[6].terrainRayDistanceQ12 =
                   terrainRay.distanceQ12;
              direction = FixedMath_DirectionFromAnglesScaledRegs
                                 (renderContext->viewAngleBC,renderContext->viewAngleB8,
                                  terrainRay.distanceQ12);
              g_GeneratedTextureScratchRuntime.samples[6].worldPoint.x =
                   g_GeneratedTextureScratchRuntime.samples[6].worldPoint.x + direction.x;
              g_GeneratedTextureScratchRuntime.samples[6].worldPoint.y =
                   g_GeneratedTextureScratchRuntime.samples[6].worldPoint.y + direction.y;
              g_GeneratedTextureScratchRuntime.samples[6].worldPoint.z =
                   g_GeneratedTextureScratchRuntime.samples[6].worldPoint.z + direction.z;
            }
            else {
              radiusScaleOrCoordinate = modelNode->subtreeBoundingRadiusQ12 * 2;
              if (g_GraphicsShadingGridStepQ20Current -
                  g_GeneratedTextureScratchRuntime.samples[6].textureCoordinateOffsetQ20 == 0) {
                return;
              }
              remainingRayLength = (Q12)(((int64_t)
                              (int)(g_GraphicsShadingGridStepQ20Current -
                                   g_GeneratedTextureScratchRuntime.samples[6].
                                   textureCoordinateOffsetQ20) * (int64_t)radiusScaleOrCoordinate) /
                            (int64_t)(int)g_GraphicsShadingGridStepQ20Current);
              g_GeneratedTextureScratchRuntime.samples[6].terrainRayDistanceQ12 = radiusScaleOrCoordinate;
              direction = FixedMath_DirectionFromAnglesScaledRegs
                                 (renderContext->viewAngleBC,renderContext->viewAngleB8,radiusScaleOrCoordinate);
              g_GeneratedTextureScratchRuntime.samples[6].worldPoint.x =
                   g_GeneratedTextureScratchRuntime.samples[6].worldPoint.x + direction.x;
              g_GeneratedTextureScratchRuntime.samples[6].worldPoint.y =
                   g_GeneratedTextureScratchRuntime.samples[6].worldPoint.y + direction.y;
              g_GeneratedTextureScratchRuntime.samples[6].worldPoint.z =
                   g_GeneratedTextureScratchRuntime.samples[6].worldPoint.z + direction.z;
              heightDeltaOrIntensityA = renderContext->viewAngleB8 ^ FIXED_ANGLE16_HALF_TURN;
              terrainRay = FieldGrid_RaycastTerrainSurfaceDistance
                                 (-renderContext->viewAngleBC - FIXED_ANGLE16_QUARTER_TURN,heightDeltaOrIntensityA,remainingRayLength,
                                  g_GeneratedTextureScratchRuntime.samples[6].worldPoint.z,
                                  g_GeneratedTextureScratchRuntime.samples[6].worldPoint.y,
                                  g_GeneratedTextureScratchRuntime.samples[6].worldPoint.x,
                                  renderContext->fieldGrid);
              surfaceDistanceQ12 = terrainRay.distanceQ12;
              if (!terrainRay.hit) {
                return;
              }
              direction = FixedMath_DirectionFromAnglesScaledRegs
                                 (terrainRay.materialOrCellIndex,heightDeltaOrIntensityA,surfaceDistanceQ12); /* quirk: EDX */
              g_GeneratedTextureScratchRuntime.samples[6].worldPoint.x =
                   g_GeneratedTextureScratchRuntime.samples[6].worldPoint.x + direction.x;
              g_GeneratedTextureScratchRuntime.samples[6].worldPoint.y =
                   g_GeneratedTextureScratchRuntime.samples[6].worldPoint.y + direction.y;
              g_GeneratedTextureScratchRuntime.samples[6].worldPoint.z =
                   g_GeneratedTextureScratchRuntime.samples[6].worldPoint.z + direction.z;
              g_GeneratedTextureScratchRuntime.samples[6].textureCoordinateOffsetQ20 =
                   g_GeneratedTextureScratchRuntime.samples[6].textureCoordinateOffsetQ20 +
                   ((int)(((int64_t)surfaceDistanceQ12 * (int64_t)(int)g_GraphicsShadingGridStepQ20Current) /
                         (int64_t)modelNode->subtreeBoundingRadiusQ12) >> 1);
              if ((int)g_GraphicsShadingGridStepQ20Current <
                  g_GeneratedTextureScratchRuntime.samples[6].textureCoordinateOffsetQ20) {
                return;
              }
            }
          }
          else {
            if ((uint32_t)modelNode->subtreeBoundingRadiusQ12 <= heightDeltaOrIntensityA) {
              heightDeltaOrIntensityA = modelNode->subtreeBoundingRadiusQ12;
            }
            g_GeneratedTextureScratchRuntime.samples[6].terrainRayDistanceQ12 = 0;
            radiusScaleOrCoordinate = (int)(((int64_t)(int)g_GraphicsShadingGridStepQ20Current *
                           (int64_t)
                           (int)((int)((uint64_t)
                                       ((int64_t)(int)heightDeltaOrIntensityA *
                                       (int64_t)g_FixedCosQ28[renderContext->viewAngleBC]) >> 0x20)
                                 << 3 | (uint32_t)((int64_t)(int)heightDeltaOrIntensityA *
                                              (int64_t)g_FixedCosQ28[renderContext->viewAngleBC])
                                        >> 0x1d)) / (int64_t)modelNode->subtreeBoundingRadiusQ12);
            g_GeneratedTextureScratchRuntime.samples[6].textureCoordinateOffsetQ20 =
                 g_GeneratedTextureScratchRuntime.samples[6].textureCoordinateOffsetQ20 - radiusScaleOrCoordinate;
            if (planeDotOrTextureOffset < radiusScaleOrCoordinate) {
              g_GeneratedTextureScratchRuntime.samples[6].textureCoordinateOffsetQ20 = 0;
              direction = FixedMath_DirectionFromAnglesScaledRegs
                                 (renderContext->viewAngleBC,renderContext->viewAngleB8,
                                  modelNode->subtreeBoundingRadiusQ12 * 2);
              g_GeneratedTextureScratchRuntime.samples[6].worldPoint.x =
                   g_GeneratedTextureScratchRuntime.samples[6].worldPoint.x - direction.x;
              g_GeneratedTextureScratchRuntime.samples[6].worldPoint.y =
                   g_GeneratedTextureScratchRuntime.samples[6].worldPoint.y - direction.y;
              g_GeneratedTextureScratchRuntime.samples[6].worldPoint.z =
                   g_GeneratedTextureScratchRuntime.samples[6].worldPoint.z - direction.z;
              terrainRay = FieldGrid_RaycastTerrainTrianglesAlongDirection
                                 (renderContext->viewAngleBC,renderContext->viewAngleB8,
                                  modelNode->subtreeBoundingRadiusQ12 * 2,
                                  g_GeneratedTextureScratchRuntime.samples[6].worldPoint.z,
                                  g_GeneratedTextureScratchRuntime.samples[6].worldPoint.y,
                                  g_GeneratedTextureScratchRuntime.samples[6].worldPoint.x,
                                  renderContext->fieldGrid);
              if (!terrainRay.hit) {
                return;
              }
              direction = FixedMath_DirectionFromAnglesScaledRegs
                                 (renderContext->viewAngleBC,renderContext->viewAngleB8,
                                  terrainRay.distanceQ12);
              g_GeneratedTextureScratchRuntime.samples[6].worldPoint.x =
                   g_GeneratedTextureScratchRuntime.samples[6].worldPoint.x + direction.x;
              g_GeneratedTextureScratchRuntime.samples[6].worldPoint.y =
                   g_GeneratedTextureScratchRuntime.samples[6].worldPoint.y + direction.y;
              g_GeneratedTextureScratchRuntime.samples[6].worldPoint.z =
                   g_GeneratedTextureScratchRuntime.samples[6].worldPoint.z + direction.z;
            }
            else {
              g_GeneratedTextureScratchRuntime.samples[6].worldPoint.z =
                   g_GeneratedTextureScratchRuntime.samples[6].worldPoint.z + heightDeltaOrIntensityA;
            }
          }
          sampleWorldZ = g_GeneratedTextureScratchRuntime.samples[7].worldPoint.z;
          surfaceHeight = FieldGrid_InterpolateTopSurfaceHeight
                             (g_GeneratedTextureScratchRuntime.samples[7].worldPoint.y,
                              g_GeneratedTextureScratchRuntime.samples[7].worldPoint.x,
                              renderContext->fieldGrid);
          planeDotOrTextureOffset = g_GeneratedTextureScratchRuntime.samples[7].textureCoordinateOffsetQ20;
          heightDeltaOrIntensityA = surfaceHeight.heightQ12 - sampleWorldZ;
          if (surfaceHeight.heightQ12 < sampleWorldZ) {
            terrainRay = FieldGrid_RaycastTerrainTrianglesAlongDirection
                               (renderContext->viewAngleBC,renderContext->viewAngleB8,
                                modelNode->subtreeBoundingRadiusQ12 * 2,
                                g_GeneratedTextureScratchRuntime.samples[7].worldPoint.z,
                                g_GeneratedTextureScratchRuntime.samples[7].worldPoint.y,
                                g_GeneratedTextureScratchRuntime.samples[7].worldPoint.x,
                                renderContext->fieldGrid);
            if (terrainRay.hit) {
              g_GeneratedTextureScratchRuntime.samples[7].terrainRayDistanceQ12 =
                   terrainRay.distanceQ12;
              direction = FixedMath_DirectionFromAnglesScaledRegs
                                 (renderContext->viewAngleBC,renderContext->viewAngleB8,
                                  terrainRay.distanceQ12);
              g_GeneratedTextureScratchRuntime.samples[7].worldPoint.x =
                   g_GeneratedTextureScratchRuntime.samples[7].worldPoint.x + direction.x;
              g_GeneratedTextureScratchRuntime.samples[7].worldPoint.y =
                   g_GeneratedTextureScratchRuntime.samples[7].worldPoint.y + direction.y;
              g_GeneratedTextureScratchRuntime.samples[7].worldPoint.z =
                   g_GeneratedTextureScratchRuntime.samples[7].worldPoint.z + direction.z;
            }
            else {
              radiusScaleOrCoordinate = modelNode->subtreeBoundingRadiusQ12 * 2;
              if (g_GraphicsShadingGridStepQ20Current -
                  g_GeneratedTextureScratchRuntime.samples[7].textureCoordinateOffsetQ20 == 0) {
                return;
              }
              remainingRayLength = (Q12)(((int64_t)
                              (int)(g_GraphicsShadingGridStepQ20Current -
                                   g_GeneratedTextureScratchRuntime.samples[7].
                                   textureCoordinateOffsetQ20) * (int64_t)radiusScaleOrCoordinate) /
                            (int64_t)(int)g_GraphicsShadingGridStepQ20Current);
              g_GeneratedTextureScratchRuntime.samples[7].terrainRayDistanceQ12 = radiusScaleOrCoordinate;
              direction = FixedMath_DirectionFromAnglesScaledRegs
                                 (renderContext->viewAngleBC,renderContext->viewAngleB8,radiusScaleOrCoordinate);
              g_GeneratedTextureScratchRuntime.samples[7].worldPoint.x =
                   g_GeneratedTextureScratchRuntime.samples[7].worldPoint.x + direction.x;
              g_GeneratedTextureScratchRuntime.samples[7].worldPoint.y =
                   g_GeneratedTextureScratchRuntime.samples[7].worldPoint.y + direction.y;
              g_GeneratedTextureScratchRuntime.samples[7].worldPoint.z =
                   g_GeneratedTextureScratchRuntime.samples[7].worldPoint.z + direction.z;
              heightDeltaOrIntensityA = renderContext->viewAngleB8 ^ FIXED_ANGLE16_HALF_TURN;
              terrainRay = FieldGrid_RaycastTerrainSurfaceDistance
                                 (-renderContext->viewAngleBC - FIXED_ANGLE16_QUARTER_TURN,heightDeltaOrIntensityA,remainingRayLength,
                                  g_GeneratedTextureScratchRuntime.samples[7].worldPoint.z,
                                  g_GeneratedTextureScratchRuntime.samples[7].worldPoint.y,
                                  g_GeneratedTextureScratchRuntime.samples[7].worldPoint.x,
                                  renderContext->fieldGrid);
              surfaceDistanceQ12 = terrainRay.distanceQ12;
              if (!terrainRay.hit) {
                return;
              }
              direction = FixedMath_DirectionFromAnglesScaledRegs
                                 (terrainRay.materialOrCellIndex,heightDeltaOrIntensityA,surfaceDistanceQ12); /* quirk: EDX */
              g_GeneratedTextureScratchRuntime.samples[7].worldPoint.x =
                   g_GeneratedTextureScratchRuntime.samples[7].worldPoint.x + direction.x;
              g_GeneratedTextureScratchRuntime.samples[7].worldPoint.y =
                   g_GeneratedTextureScratchRuntime.samples[7].worldPoint.y + direction.y;
              g_GeneratedTextureScratchRuntime.samples[7].worldPoint.z =
                   g_GeneratedTextureScratchRuntime.samples[7].worldPoint.z + direction.z;
              g_GeneratedTextureScratchRuntime.samples[7].textureCoordinateOffsetQ20 =
                   g_GeneratedTextureScratchRuntime.samples[7].textureCoordinateOffsetQ20 +
                   ((int)(((int64_t)surfaceDistanceQ12 * (int64_t)(int)g_GraphicsShadingGridStepQ20Current) /
                         (int64_t)modelNode->subtreeBoundingRadiusQ12) >> 1);
              if ((int)g_GraphicsShadingGridStepQ20Current <
                  g_GeneratedTextureScratchRuntime.samples[7].textureCoordinateOffsetQ20) {
                return;
              }
            }
          }
          else {
            if ((uint32_t)modelNode->subtreeBoundingRadiusQ12 <= heightDeltaOrIntensityA) {
              heightDeltaOrIntensityA = modelNode->subtreeBoundingRadiusQ12;
            }
            g_GeneratedTextureScratchRuntime.samples[7].terrainRayDistanceQ12 = 0;
            radiusScaleOrCoordinate = (int)(((int64_t)(int)g_GraphicsShadingGridStepQ20Current *
                           (int64_t)
                           (int)((int)((uint64_t)
                                       ((int64_t)(int)heightDeltaOrIntensityA *
                                       (int64_t)g_FixedCosQ28[renderContext->viewAngleBC]) >> 0x20)
                                 << 3 | (uint32_t)((int64_t)(int)heightDeltaOrIntensityA *
                                              (int64_t)g_FixedCosQ28[renderContext->viewAngleBC])
                                        >> 0x1d)) / (int64_t)modelNode->subtreeBoundingRadiusQ12);
            g_GeneratedTextureScratchRuntime.samples[7].textureCoordinateOffsetQ20 =
                 g_GeneratedTextureScratchRuntime.samples[7].textureCoordinateOffsetQ20 - radiusScaleOrCoordinate;
            if (planeDotOrTextureOffset < radiusScaleOrCoordinate) {
              g_GeneratedTextureScratchRuntime.samples[7].textureCoordinateOffsetQ20 = 0;
              direction = FixedMath_DirectionFromAnglesScaledRegs
                                 (renderContext->viewAngleBC,renderContext->viewAngleB8,
                                  modelNode->subtreeBoundingRadiusQ12 * 2);
              g_GeneratedTextureScratchRuntime.samples[7].worldPoint.x =
                   g_GeneratedTextureScratchRuntime.samples[7].worldPoint.x - direction.x;
              g_GeneratedTextureScratchRuntime.samples[7].worldPoint.y =
                   g_GeneratedTextureScratchRuntime.samples[7].worldPoint.y - direction.y;
              g_GeneratedTextureScratchRuntime.samples[7].worldPoint.z =
                   g_GeneratedTextureScratchRuntime.samples[7].worldPoint.z - direction.z;
              terrainRay = FieldGrid_RaycastTerrainTrianglesAlongDirection
                                 (renderContext->viewAngleBC,renderContext->viewAngleB8,
                                  modelNode->subtreeBoundingRadiusQ12 * 2,
                                  g_GeneratedTextureScratchRuntime.samples[7].worldPoint.z,
                                  g_GeneratedTextureScratchRuntime.samples[7].worldPoint.y,
                                  g_GeneratedTextureScratchRuntime.samples[7].worldPoint.x,
                                  renderContext->fieldGrid);
              if (!terrainRay.hit) {
                return;
              }
              direction = FixedMath_DirectionFromAnglesScaledRegs
                                 (renderContext->viewAngleBC,renderContext->viewAngleB8,
                                  terrainRay.distanceQ12);
              g_GeneratedTextureScratchRuntime.samples[7].worldPoint.x =
                   g_GeneratedTextureScratchRuntime.samples[7].worldPoint.x + direction.x;
              g_GeneratedTextureScratchRuntime.samples[7].worldPoint.y =
                   g_GeneratedTextureScratchRuntime.samples[7].worldPoint.y + direction.y;
              g_GeneratedTextureScratchRuntime.samples[7].worldPoint.z =
                   g_GeneratedTextureScratchRuntime.samples[7].worldPoint.z + direction.z;
            }
            else {
              g_GeneratedTextureScratchRuntime.samples[7].worldPoint.z =
                   g_GeneratedTextureScratchRuntime.samples[7].worldPoint.z + heightDeltaOrIntensityA;
            }
          }
          sampleWorldZ = g_GeneratedTextureScratchRuntime.samples[8].worldPoint.z;
          surfaceHeight = FieldGrid_InterpolateTopSurfaceHeight
                             (g_GeneratedTextureScratchRuntime.samples[8].worldPoint.y,
                              g_GeneratedTextureScratchRuntime.samples[8].worldPoint.x,
                              renderContext->fieldGrid);
          planeDotOrTextureOffset = g_GeneratedTextureScratchRuntime.samples[8].textureCoordinateOffsetQ20;
          heightDeltaOrIntensityA = surfaceHeight.heightQ12 - sampleWorldZ;
          if (surfaceHeight.heightQ12 < sampleWorldZ) {
            terrainRay = FieldGrid_RaycastTerrainTrianglesAlongDirection
                               (renderContext->viewAngleBC,renderContext->viewAngleB8,
                                modelNode->subtreeBoundingRadiusQ12 * 2,
                                g_GeneratedTextureScratchRuntime.samples[8].worldPoint.z,
                                g_GeneratedTextureScratchRuntime.samples[8].worldPoint.y,
                                g_GeneratedTextureScratchRuntime.samples[8].worldPoint.x,
                                renderContext->fieldGrid);
            if (terrainRay.hit) {
              g_GeneratedTextureScratchRuntime.samples[8].terrainRayDistanceQ12 =
                   terrainRay.distanceQ12;
              direction = FixedMath_DirectionFromAnglesScaledRegs
                                 (renderContext->viewAngleBC,renderContext->viewAngleB8,
                                  terrainRay.distanceQ12);
              g_GeneratedTextureScratchRuntime.samples[8].worldPoint.x =
                   g_GeneratedTextureScratchRuntime.samples[8].worldPoint.x + direction.x;
              g_GeneratedTextureScratchRuntime.samples[8].worldPoint.y =
                   g_GeneratedTextureScratchRuntime.samples[8].worldPoint.y + direction.y;
              g_GeneratedTextureScratchRuntime.samples[8].worldPoint.z =
                   g_GeneratedTextureScratchRuntime.samples[8].worldPoint.z + direction.z;
            }
            else {
              radiusScaleOrCoordinate = modelNode->subtreeBoundingRadiusQ12 * 2;
              if (g_GraphicsShadingGridStepQ20Current -
                  g_GeneratedTextureScratchRuntime.samples[8].textureCoordinateOffsetQ20 == 0) {
                return;
              }
              remainingRayLength = (Q12)(((int64_t)
                              (int)(g_GraphicsShadingGridStepQ20Current -
                                   g_GeneratedTextureScratchRuntime.samples[8].
                                   textureCoordinateOffsetQ20) * (int64_t)radiusScaleOrCoordinate) /
                            (int64_t)(int)g_GraphicsShadingGridStepQ20Current);
              g_GeneratedTextureScratchRuntime.samples[8].terrainRayDistanceQ12 = radiusScaleOrCoordinate;
              direction = FixedMath_DirectionFromAnglesScaledRegs
                                 (renderContext->viewAngleBC,renderContext->viewAngleB8,radiusScaleOrCoordinate);
              g_GeneratedTextureScratchRuntime.samples[8].worldPoint.x =
                   g_GeneratedTextureScratchRuntime.samples[8].worldPoint.x + direction.x;
              g_GeneratedTextureScratchRuntime.samples[8].worldPoint.y =
                   g_GeneratedTextureScratchRuntime.samples[8].worldPoint.y + direction.y;
              g_GeneratedTextureScratchRuntime.samples[8].worldPoint.z =
                   g_GeneratedTextureScratchRuntime.samples[8].worldPoint.z + direction.z;
              heightDeltaOrIntensityA = renderContext->viewAngleB8 ^ FIXED_ANGLE16_HALF_TURN;
              terrainRay = FieldGrid_RaycastTerrainSurfaceDistance
                                 (-renderContext->viewAngleBC - FIXED_ANGLE16_QUARTER_TURN,heightDeltaOrIntensityA,remainingRayLength,
                                  g_GeneratedTextureScratchRuntime.samples[8].worldPoint.z,
                                  g_GeneratedTextureScratchRuntime.samples[8].worldPoint.y,
                                  g_GeneratedTextureScratchRuntime.samples[8].worldPoint.x,
                                  renderContext->fieldGrid);
              surfaceDistanceQ12 = terrainRay.distanceQ12;
              if (!terrainRay.hit) {
                return;
              }
              direction = FixedMath_DirectionFromAnglesScaledRegs
                                 (terrainRay.materialOrCellIndex,heightDeltaOrIntensityA,surfaceDistanceQ12); /* quirk: EDX */
              g_GeneratedTextureScratchRuntime.samples[8].worldPoint.x =
                   g_GeneratedTextureScratchRuntime.samples[8].worldPoint.x + direction.x;
              g_GeneratedTextureScratchRuntime.samples[8].worldPoint.y =
                   g_GeneratedTextureScratchRuntime.samples[8].worldPoint.y + direction.y;
              g_GeneratedTextureScratchRuntime.samples[8].worldPoint.z =
                   g_GeneratedTextureScratchRuntime.samples[8].worldPoint.z + direction.z;
              g_GeneratedTextureScratchRuntime.samples[8].textureCoordinateOffsetQ20 =
                   g_GeneratedTextureScratchRuntime.samples[8].textureCoordinateOffsetQ20 +
                   ((int)(((int64_t)surfaceDistanceQ12 * (int64_t)(int)g_GraphicsShadingGridStepQ20Current) /
                         (int64_t)modelNode->subtreeBoundingRadiusQ12) >> 1);
              if ((int)g_GraphicsShadingGridStepQ20Current <
                  g_GeneratedTextureScratchRuntime.samples[8].textureCoordinateOffsetQ20) {
                return;
              }
            }
          }
          else {
            if ((uint32_t)modelNode->subtreeBoundingRadiusQ12 <= heightDeltaOrIntensityA) {
              heightDeltaOrIntensityA = modelNode->subtreeBoundingRadiusQ12;
            }
            g_GeneratedTextureScratchRuntime.samples[8].terrainRayDistanceQ12 = 0;
            radiusScaleOrCoordinate = (int)(((int64_t)(int)g_GraphicsShadingGridStepQ20Current *
                           (int64_t)
                           (int)((int)((uint64_t)
                                       ((int64_t)(int)heightDeltaOrIntensityA *
                                       (int64_t)g_FixedCosQ28[renderContext->viewAngleBC]) >> 0x20)
                                 << 3 | (uint32_t)((int64_t)(int)heightDeltaOrIntensityA *
                                              (int64_t)g_FixedCosQ28[renderContext->viewAngleBC])
                                        >> 0x1d)) / (int64_t)modelNode->subtreeBoundingRadiusQ12);
            g_GeneratedTextureScratchRuntime.samples[8].textureCoordinateOffsetQ20 =
                 g_GeneratedTextureScratchRuntime.samples[8].textureCoordinateOffsetQ20 - radiusScaleOrCoordinate;
            if (planeDotOrTextureOffset < radiusScaleOrCoordinate) {
              g_GeneratedTextureScratchRuntime.samples[8].textureCoordinateOffsetQ20 = 0;
              direction = FixedMath_DirectionFromAnglesScaledRegs
                                 (renderContext->viewAngleBC,renderContext->viewAngleB8,
                                  modelNode->subtreeBoundingRadiusQ12 * 2);
              g_GeneratedTextureScratchRuntime.samples[8].worldPoint.x =
                   g_GeneratedTextureScratchRuntime.samples[8].worldPoint.x - direction.x;
              g_GeneratedTextureScratchRuntime.samples[8].worldPoint.y =
                   g_GeneratedTextureScratchRuntime.samples[8].worldPoint.y - direction.y;
              g_GeneratedTextureScratchRuntime.samples[8].worldPoint.z =
                   g_GeneratedTextureScratchRuntime.samples[8].worldPoint.z - direction.z;
              terrainRay = FieldGrid_RaycastTerrainTrianglesAlongDirection
                                 (renderContext->viewAngleBC,renderContext->viewAngleB8,
                                  modelNode->subtreeBoundingRadiusQ12 * 2,
                                  g_GeneratedTextureScratchRuntime.samples[8].worldPoint.z,
                                  g_GeneratedTextureScratchRuntime.samples[8].worldPoint.y,
                                  g_GeneratedTextureScratchRuntime.samples[8].worldPoint.x,
                                  renderContext->fieldGrid);
              if (!terrainRay.hit) {
                return;
              }
              direction = FixedMath_DirectionFromAnglesScaledRegs
                                 (renderContext->viewAngleBC,renderContext->viewAngleB8,
                                  terrainRay.distanceQ12);
              g_GeneratedTextureScratchRuntime.samples[8].worldPoint.x =
                   g_GeneratedTextureScratchRuntime.samples[8].worldPoint.x + direction.x;
              g_GeneratedTextureScratchRuntime.samples[8].worldPoint.y =
                   g_GeneratedTextureScratchRuntime.samples[8].worldPoint.y + direction.y;
              g_GeneratedTextureScratchRuntime.samples[8].worldPoint.z =
                   g_GeneratedTextureScratchRuntime.samples[8].worldPoint.z + direction.z;
            }
            else {
              g_GeneratedTextureScratchRuntime.samples[8].worldPoint.z =
                   g_GeneratedTextureScratchRuntime.samples[8].worldPoint.z + heightDeltaOrIntensityA;
            }
          }
          sampleWorldZ = g_GeneratedTextureScratchRuntime.samples[10].worldPoint.z;
          surfaceHeight = FieldGrid_InterpolateTopSurfaceHeight
                             (g_GeneratedTextureScratchRuntime.samples[10].worldPoint.y,
                              g_GeneratedTextureScratchRuntime.samples[10].worldPoint.x,
                              renderContext->fieldGrid);
          planeDotOrTextureOffset = g_GeneratedTextureScratchRuntime.samples[10].textureCoordinateOffsetQ20;
          heightDeltaOrIntensityA = surfaceHeight.heightQ12 - sampleWorldZ;
          if (surfaceHeight.heightQ12 < sampleWorldZ) {
            terrainRay = FieldGrid_RaycastTerrainTrianglesAlongDirection
                               (renderContext->viewAngleBC,renderContext->viewAngleB8,
                                modelNode->subtreeBoundingRadiusQ12 * 2,
                                g_GeneratedTextureScratchRuntime.samples[10].worldPoint.z,
                                g_GeneratedTextureScratchRuntime.samples[10].worldPoint.y,
                                g_GeneratedTextureScratchRuntime.samples[10].worldPoint.x,
                                renderContext->fieldGrid);
            if (terrainRay.hit) {
              g_GeneratedTextureScratchRuntime.samples[10].terrainRayDistanceQ12 =
                   terrainRay.distanceQ12;
              direction = FixedMath_DirectionFromAnglesScaledRegs
                                 (renderContext->viewAngleBC,renderContext->viewAngleB8,
                                  terrainRay.distanceQ12);
              g_GeneratedTextureScratchRuntime.samples[10].worldPoint.x =
                   g_GeneratedTextureScratchRuntime.samples[10].worldPoint.x + direction.x;
              g_GeneratedTextureScratchRuntime.samples[10].worldPoint.y =
                   g_GeneratedTextureScratchRuntime.samples[10].worldPoint.y + direction.y;
              g_GeneratedTextureScratchRuntime.samples[10].worldPoint.z =
                   g_GeneratedTextureScratchRuntime.samples[10].worldPoint.z + direction.z;
            }
            else {
              radiusScaleOrCoordinate = modelNode->subtreeBoundingRadiusQ12 * 2;
              if (g_GraphicsShadingGridStepQ20Current -
                  g_GeneratedTextureScratchRuntime.samples[10].textureCoordinateOffsetQ20 == 0) {
                return;
              }
              remainingRayLength = (Q12)(((int64_t)
                              (int)(g_GraphicsShadingGridStepQ20Current -
                                   g_GeneratedTextureScratchRuntime.samples[10].
                                   textureCoordinateOffsetQ20) * (int64_t)radiusScaleOrCoordinate) /
                            (int64_t)(int)g_GraphicsShadingGridStepQ20Current);
              g_GeneratedTextureScratchRuntime.samples[10].terrainRayDistanceQ12 = radiusScaleOrCoordinate;
              direction = FixedMath_DirectionFromAnglesScaledRegs
                                 (renderContext->viewAngleBC,renderContext->viewAngleB8,radiusScaleOrCoordinate);
              g_GeneratedTextureScratchRuntime.samples[10].worldPoint.x =
                   g_GeneratedTextureScratchRuntime.samples[10].worldPoint.x + direction.x;
              g_GeneratedTextureScratchRuntime.samples[10].worldPoint.y =
                   g_GeneratedTextureScratchRuntime.samples[10].worldPoint.y + direction.y;
              g_GeneratedTextureScratchRuntime.samples[10].worldPoint.z =
                   g_GeneratedTextureScratchRuntime.samples[10].worldPoint.z + direction.z;
              heightDeltaOrIntensityA = renderContext->viewAngleB8 ^ FIXED_ANGLE16_HALF_TURN;
              terrainRay = FieldGrid_RaycastTerrainSurfaceDistance
                                 (-renderContext->viewAngleBC - FIXED_ANGLE16_QUARTER_TURN,heightDeltaOrIntensityA,remainingRayLength,
                                  g_GeneratedTextureScratchRuntime.samples[10].worldPoint.z,
                                  g_GeneratedTextureScratchRuntime.samples[10].worldPoint.y,
                                  g_GeneratedTextureScratchRuntime.samples[10].worldPoint.x,
                                  renderContext->fieldGrid);
              surfaceDistanceQ12 = terrainRay.distanceQ12;
              if (!terrainRay.hit) {
                return;
              }
              direction = FixedMath_DirectionFromAnglesScaledRegs
                                 (terrainRay.materialOrCellIndex,heightDeltaOrIntensityA,surfaceDistanceQ12); /* quirk: EDX */
              g_GeneratedTextureScratchRuntime.samples[10].worldPoint.x =
                   g_GeneratedTextureScratchRuntime.samples[10].worldPoint.x + direction.x;
              g_GeneratedTextureScratchRuntime.samples[10].worldPoint.y =
                   g_GeneratedTextureScratchRuntime.samples[10].worldPoint.y + direction.y;
              g_GeneratedTextureScratchRuntime.samples[10].worldPoint.z =
                   g_GeneratedTextureScratchRuntime.samples[10].worldPoint.z + direction.z;
              g_GeneratedTextureScratchRuntime.samples[10].textureCoordinateOffsetQ20 =
                   g_GeneratedTextureScratchRuntime.samples[10].textureCoordinateOffsetQ20 +
                   ((int)(((int64_t)surfaceDistanceQ12 * (int64_t)(int)g_GraphicsShadingGridStepQ20Current) /
                         (int64_t)modelNode->subtreeBoundingRadiusQ12) >> 1);
              if ((int)g_GraphicsShadingGridStepQ20Current <
                  g_GeneratedTextureScratchRuntime.samples[10].textureCoordinateOffsetQ20) {
                return;
              }
            }
          }
          else {
            if ((uint32_t)modelNode->subtreeBoundingRadiusQ12 <= heightDeltaOrIntensityA) {
              heightDeltaOrIntensityA = modelNode->subtreeBoundingRadiusQ12;
            }
            g_GeneratedTextureScratchRuntime.samples[10].terrainRayDistanceQ12 = 0;
            radiusScaleOrCoordinate = (int)(((int64_t)(int)g_GraphicsShadingGridStepQ20Current *
                           (int64_t)
                           (int)((int)((uint64_t)
                                       ((int64_t)(int)heightDeltaOrIntensityA *
                                       (int64_t)g_FixedCosQ28[renderContext->viewAngleBC]) >> 0x20)
                                 << 3 | (uint32_t)((int64_t)(int)heightDeltaOrIntensityA *
                                              (int64_t)g_FixedCosQ28[renderContext->viewAngleBC])
                                        >> 0x1d)) / (int64_t)modelNode->subtreeBoundingRadiusQ12);
            g_GeneratedTextureScratchRuntime.samples[10].textureCoordinateOffsetQ20 =
                 g_GeneratedTextureScratchRuntime.samples[10].textureCoordinateOffsetQ20 - radiusScaleOrCoordinate;
            if (planeDotOrTextureOffset < radiusScaleOrCoordinate) {
              g_GeneratedTextureScratchRuntime.samples[10].textureCoordinateOffsetQ20 = 0;
              direction = FixedMath_DirectionFromAnglesScaledRegs
                                 (renderContext->viewAngleBC,renderContext->viewAngleB8,
                                  modelNode->subtreeBoundingRadiusQ12 * 2);
              g_GeneratedTextureScratchRuntime.samples[10].worldPoint.x =
                   g_GeneratedTextureScratchRuntime.samples[10].worldPoint.x - direction.x;
              g_GeneratedTextureScratchRuntime.samples[10].worldPoint.y =
                   g_GeneratedTextureScratchRuntime.samples[10].worldPoint.y - direction.y;
              g_GeneratedTextureScratchRuntime.samples[10].worldPoint.z =
                   g_GeneratedTextureScratchRuntime.samples[10].worldPoint.z - direction.z;
              terrainRay = FieldGrid_RaycastTerrainTrianglesAlongDirection
                                 (renderContext->viewAngleBC,renderContext->viewAngleB8,
                                  modelNode->subtreeBoundingRadiusQ12 * 2,
                                  g_GeneratedTextureScratchRuntime.samples[10].worldPoint.z,
                                  g_GeneratedTextureScratchRuntime.samples[10].worldPoint.y,
                                  g_GeneratedTextureScratchRuntime.samples[10].worldPoint.x,
                                  renderContext->fieldGrid);
              if (!terrainRay.hit) {
                return;
              }
              direction = FixedMath_DirectionFromAnglesScaledRegs
                                 (renderContext->viewAngleBC,renderContext->viewAngleB8,
                                  terrainRay.distanceQ12);
              g_GeneratedTextureScratchRuntime.samples[10].worldPoint.x =
                   g_GeneratedTextureScratchRuntime.samples[10].worldPoint.x + direction.x;
              g_GeneratedTextureScratchRuntime.samples[10].worldPoint.y =
                   g_GeneratedTextureScratchRuntime.samples[10].worldPoint.y + direction.y;
              g_GeneratedTextureScratchRuntime.samples[10].worldPoint.z =
                   g_GeneratedTextureScratchRuntime.samples[10].worldPoint.z + direction.z;
            }
            else {
              g_GeneratedTextureScratchRuntime.samples[10].worldPoint.z =
                   g_GeneratedTextureScratchRuntime.samples[10].worldPoint.z + heightDeltaOrIntensityA;
            }
          }
          sampleWorldZ = g_GeneratedTextureScratchRuntime.samples[9].worldPoint.z;
          surfaceHeight = FieldGrid_InterpolateTopSurfaceHeight
                             (g_GeneratedTextureScratchRuntime.samples[9].worldPoint.y,
                              g_GeneratedTextureScratchRuntime.samples[9].worldPoint.x,
                              renderContext->fieldGrid);
          planeDotOrTextureOffset = g_GeneratedTextureScratchRuntime.samples[9].textureCoordinateOffsetQ20;
          heightDeltaOrIntensityA = surfaceHeight.heightQ12 - sampleWorldZ;
          if (surfaceHeight.heightQ12 < sampleWorldZ) {
            terrainRay = FieldGrid_RaycastTerrainTrianglesAlongDirection
                               (renderContext->viewAngleBC,renderContext->viewAngleB8,
                                modelNode->subtreeBoundingRadiusQ12 * 2,
                                g_GeneratedTextureScratchRuntime.samples[9].worldPoint.z,
                                g_GeneratedTextureScratchRuntime.samples[9].worldPoint.y,
                                g_GeneratedTextureScratchRuntime.samples[9].worldPoint.x,
                                renderContext->fieldGrid);
            if (terrainRay.hit) {
              g_GeneratedTextureScratchRuntime.samples[9].terrainRayDistanceQ12 =
                   terrainRay.distanceQ12;
              direction = FixedMath_DirectionFromAnglesScaledRegs
                                 (renderContext->viewAngleBC,renderContext->viewAngleB8,
                                  terrainRay.distanceQ12);
              g_GeneratedTextureScratchRuntime.samples[9].worldPoint.x =
                   g_GeneratedTextureScratchRuntime.samples[9].worldPoint.x + direction.x;
              g_GeneratedTextureScratchRuntime.samples[9].worldPoint.y =
                   g_GeneratedTextureScratchRuntime.samples[9].worldPoint.y + direction.y;
              g_GeneratedTextureScratchRuntime.samples[9].worldPoint.z =
                   g_GeneratedTextureScratchRuntime.samples[9].worldPoint.z + direction.z;
            }
            else {
              radiusScaleOrCoordinate = modelNode->subtreeBoundingRadiusQ12 * 2;
              if (g_GraphicsShadingGridStepQ20Current -
                  g_GeneratedTextureScratchRuntime.samples[9].textureCoordinateOffsetQ20 == 0) {
                return;
              }
              remainingRayLength = (Q12)(((int64_t)
                              (int)(g_GraphicsShadingGridStepQ20Current -
                                   g_GeneratedTextureScratchRuntime.samples[9].
                                   textureCoordinateOffsetQ20) * (int64_t)radiusScaleOrCoordinate) /
                            (int64_t)(int)g_GraphicsShadingGridStepQ20Current);
              g_GeneratedTextureScratchRuntime.samples[9].terrainRayDistanceQ12 = radiusScaleOrCoordinate;
              direction = FixedMath_DirectionFromAnglesScaledRegs
                                 (renderContext->viewAngleBC,renderContext->viewAngleB8,radiusScaleOrCoordinate);
              g_GeneratedTextureScratchRuntime.samples[9].worldPoint.x =
                   g_GeneratedTextureScratchRuntime.samples[9].worldPoint.x + direction.x;
              g_GeneratedTextureScratchRuntime.samples[9].worldPoint.y =
                   g_GeneratedTextureScratchRuntime.samples[9].worldPoint.y + direction.y;
              g_GeneratedTextureScratchRuntime.samples[9].worldPoint.z =
                   g_GeneratedTextureScratchRuntime.samples[9].worldPoint.z + direction.z;
              heightDeltaOrIntensityA = renderContext->viewAngleB8 ^ FIXED_ANGLE16_HALF_TURN;
              terrainRay = FieldGrid_RaycastTerrainSurfaceDistance
                                 (-renderContext->viewAngleBC - FIXED_ANGLE16_QUARTER_TURN,heightDeltaOrIntensityA,remainingRayLength,
                                  g_GeneratedTextureScratchRuntime.samples[9].worldPoint.z,
                                  g_GeneratedTextureScratchRuntime.samples[9].worldPoint.y,
                                  g_GeneratedTextureScratchRuntime.samples[9].worldPoint.x,
                                  renderContext->fieldGrid);
              surfaceDistanceQ12 = terrainRay.distanceQ12;
              if (!terrainRay.hit) {
                return;
              }
              direction = FixedMath_DirectionFromAnglesScaledRegs
                                 (terrainRay.materialOrCellIndex,heightDeltaOrIntensityA,surfaceDistanceQ12); /* quirk: EDX */
              g_GeneratedTextureScratchRuntime.samples[9].worldPoint.x =
                   g_GeneratedTextureScratchRuntime.samples[9].worldPoint.x + direction.x;
              g_GeneratedTextureScratchRuntime.samples[9].worldPoint.y =
                   g_GeneratedTextureScratchRuntime.samples[9].worldPoint.y + direction.y;
              g_GeneratedTextureScratchRuntime.samples[9].worldPoint.z =
                   g_GeneratedTextureScratchRuntime.samples[9].worldPoint.z + direction.z;
              g_GeneratedTextureScratchRuntime.samples[9].textureCoordinateOffsetQ20 =
                   g_GeneratedTextureScratchRuntime.samples[9].textureCoordinateOffsetQ20 +
                   ((int)(((int64_t)surfaceDistanceQ12 * (int64_t)(int)g_GraphicsShadingGridStepQ20Current) /
                         (int64_t)modelNode->subtreeBoundingRadiusQ12) >> 1);
              if ((int)g_GraphicsShadingGridStepQ20Current <
                  g_GeneratedTextureScratchRuntime.samples[9].textureCoordinateOffsetQ20) {
                return;
              }
            }
          }
          else {
            if ((uint32_t)modelNode->subtreeBoundingRadiusQ12 <= heightDeltaOrIntensityA) {
              heightDeltaOrIntensityA = modelNode->subtreeBoundingRadiusQ12;
            }
            g_GeneratedTextureScratchRuntime.samples[9].terrainRayDistanceQ12 = 0;
            radiusScaleOrCoordinate = (int)(((int64_t)(int)g_GraphicsShadingGridStepQ20Current *
                           (int64_t)
                           (int)((int)((uint64_t)
                                       ((int64_t)(int)heightDeltaOrIntensityA *
                                       (int64_t)g_FixedCosQ28[renderContext->viewAngleBC]) >> 0x20)
                                 << 3 | (uint32_t)((int64_t)(int)heightDeltaOrIntensityA *
                                              (int64_t)g_FixedCosQ28[renderContext->viewAngleBC])
                                        >> 0x1d)) / (int64_t)modelNode->subtreeBoundingRadiusQ12);
            g_GeneratedTextureScratchRuntime.samples[9].textureCoordinateOffsetQ20 =
                 g_GeneratedTextureScratchRuntime.samples[9].textureCoordinateOffsetQ20 - radiusScaleOrCoordinate;
            if (planeDotOrTextureOffset < radiusScaleOrCoordinate) {
              g_GeneratedTextureScratchRuntime.samples[9].textureCoordinateOffsetQ20 = 0;
              direction = FixedMath_DirectionFromAnglesScaledRegs
                                 (renderContext->viewAngleBC,renderContext->viewAngleB8,
                                  modelNode->subtreeBoundingRadiusQ12 * 2);
              g_GeneratedTextureScratchRuntime.samples[9].worldPoint.x =
                   g_GeneratedTextureScratchRuntime.samples[9].worldPoint.x - direction.x;
              g_GeneratedTextureScratchRuntime.samples[9].worldPoint.y =
                   g_GeneratedTextureScratchRuntime.samples[9].worldPoint.y - direction.y;
              g_GeneratedTextureScratchRuntime.samples[9].worldPoint.z =
                   g_GeneratedTextureScratchRuntime.samples[9].worldPoint.z - direction.z;
              terrainRay = FieldGrid_RaycastTerrainTrianglesAlongDirection
                                 (renderContext->viewAngleBC,renderContext->viewAngleB8,
                                  modelNode->subtreeBoundingRadiusQ12 * 2,
                                  g_GeneratedTextureScratchRuntime.samples[9].worldPoint.z,
                                  g_GeneratedTextureScratchRuntime.samples[9].worldPoint.y,
                                  g_GeneratedTextureScratchRuntime.samples[9].worldPoint.x,
                                  renderContext->fieldGrid);
              if (!terrainRay.hit) {
                return;
              }
              direction = FixedMath_DirectionFromAnglesScaledRegs
                                 (renderContext->viewAngleBC,renderContext->viewAngleB8,
                                  terrainRay.distanceQ12);
              g_GeneratedTextureScratchRuntime.samples[9].worldPoint.x =
                   g_GeneratedTextureScratchRuntime.samples[9].worldPoint.x + direction.x;
              g_GeneratedTextureScratchRuntime.samples[9].worldPoint.y =
                   g_GeneratedTextureScratchRuntime.samples[9].worldPoint.y + direction.y;
              g_GeneratedTextureScratchRuntime.samples[9].worldPoint.z =
                   g_GeneratedTextureScratchRuntime.samples[9].worldPoint.z + direction.z;
            }
            else {
              g_GeneratedTextureScratchRuntime.samples[9].worldPoint.z =
                   g_GeneratedTextureScratchRuntime.samples[9].worldPoint.z + heightDeltaOrIntensityA;
            }
          }
          sampleWorldZ = g_GeneratedTextureScratchRuntime.samples[11].worldPoint.z;
          surfaceHeight = FieldGrid_InterpolateTopSurfaceHeight
                             (g_GeneratedTextureScratchRuntime.samples[11].worldPoint.y,
                              g_GeneratedTextureScratchRuntime.samples[11].worldPoint.x,
                              renderContext->fieldGrid);
          planeDotOrTextureOffset = g_GeneratedTextureScratchRuntime.samples[11].textureCoordinateOffsetQ20;
          heightDeltaOrIntensityA = surfaceHeight.heightQ12 - sampleWorldZ;
          if (surfaceHeight.heightQ12 < sampleWorldZ) {
            terrainRay = FieldGrid_RaycastTerrainTrianglesAlongDirection
                               (renderContext->viewAngleBC,renderContext->viewAngleB8,
                                modelNode->subtreeBoundingRadiusQ12 * 2,
                                g_GeneratedTextureScratchRuntime.samples[11].worldPoint.z,
                                g_GeneratedTextureScratchRuntime.samples[11].worldPoint.y,
                                g_GeneratedTextureScratchRuntime.samples[11].worldPoint.x,
                                renderContext->fieldGrid);
            if (terrainRay.hit) {
              g_GeneratedTextureScratchRuntime.samples[11].terrainRayDistanceQ12 =
                   terrainRay.distanceQ12;
              direction = FixedMath_DirectionFromAnglesScaledRegs
                                 (renderContext->viewAngleBC,renderContext->viewAngleB8,
                                  terrainRay.distanceQ12);
              g_GeneratedTextureScratchRuntime.samples[11].worldPoint.x =
                   g_GeneratedTextureScratchRuntime.samples[11].worldPoint.x + direction.x;
              g_GeneratedTextureScratchRuntime.samples[11].worldPoint.y =
                   g_GeneratedTextureScratchRuntime.samples[11].worldPoint.y + direction.y;
              g_GeneratedTextureScratchRuntime.samples[11].worldPoint.z =
                   g_GeneratedTextureScratchRuntime.samples[11].worldPoint.z + direction.z;
            }
            else {
              radiusScaleOrCoordinate = modelNode->subtreeBoundingRadiusQ12 * 2;
              if (g_GraphicsShadingGridStepQ20Current -
                  g_GeneratedTextureScratchRuntime.samples[11].textureCoordinateOffsetQ20 == 0) {
                return;
              }
              remainingRayLength = (Q12)(((int64_t)
                              (int)(g_GraphicsShadingGridStepQ20Current -
                                   g_GeneratedTextureScratchRuntime.samples[11].
                                   textureCoordinateOffsetQ20) * (int64_t)radiusScaleOrCoordinate) /
                            (int64_t)(int)g_GraphicsShadingGridStepQ20Current);
              g_GeneratedTextureScratchRuntime.samples[11].terrainRayDistanceQ12 = radiusScaleOrCoordinate;
              direction = FixedMath_DirectionFromAnglesScaledRegs
                                 (renderContext->viewAngleBC,renderContext->viewAngleB8,radiusScaleOrCoordinate);
              g_GeneratedTextureScratchRuntime.samples[11].worldPoint.x =
                   g_GeneratedTextureScratchRuntime.samples[11].worldPoint.x + direction.x;
              g_GeneratedTextureScratchRuntime.samples[11].worldPoint.y =
                   g_GeneratedTextureScratchRuntime.samples[11].worldPoint.y + direction.y;
              g_GeneratedTextureScratchRuntime.samples[11].worldPoint.z =
                   g_GeneratedTextureScratchRuntime.samples[11].worldPoint.z + direction.z;
              heightDeltaOrIntensityA = renderContext->viewAngleB8 ^ FIXED_ANGLE16_HALF_TURN;
              terrainRay = FieldGrid_RaycastTerrainSurfaceDistance
                                 (-renderContext->viewAngleBC - FIXED_ANGLE16_QUARTER_TURN,heightDeltaOrIntensityA,remainingRayLength,
                                  g_GeneratedTextureScratchRuntime.samples[11].worldPoint.z,
                                  g_GeneratedTextureScratchRuntime.samples[11].worldPoint.y,
                                  g_GeneratedTextureScratchRuntime.samples[11].worldPoint.x,
                                  renderContext->fieldGrid);
              surfaceDistanceQ12 = terrainRay.distanceQ12;
              if (!terrainRay.hit) {
                return;
              }
              direction = FixedMath_DirectionFromAnglesScaledRegs
                                 (terrainRay.materialOrCellIndex,heightDeltaOrIntensityA,surfaceDistanceQ12); /* quirk: EDX */
              g_GeneratedTextureScratchRuntime.samples[11].worldPoint.x =
                   g_GeneratedTextureScratchRuntime.samples[11].worldPoint.x + direction.x;
              g_GeneratedTextureScratchRuntime.samples[11].worldPoint.y =
                   g_GeneratedTextureScratchRuntime.samples[11].worldPoint.y + direction.y;
              g_GeneratedTextureScratchRuntime.samples[11].worldPoint.z =
                   g_GeneratedTextureScratchRuntime.samples[11].worldPoint.z + direction.z;
              g_GeneratedTextureScratchRuntime.samples[11].textureCoordinateOffsetQ20 =
                   g_GeneratedTextureScratchRuntime.samples[11].textureCoordinateOffsetQ20 +
                   ((int)(((int64_t)surfaceDistanceQ12 * (int64_t)(int)g_GraphicsShadingGridStepQ20Current) /
                         (int64_t)modelNode->subtreeBoundingRadiusQ12) >> 1);
              if ((int)g_GraphicsShadingGridStepQ20Current <
                  g_GeneratedTextureScratchRuntime.samples[11].textureCoordinateOffsetQ20) {
                return;
              }
            }
          }
          else {
            if ((uint32_t)modelNode->subtreeBoundingRadiusQ12 <= heightDeltaOrIntensityA) {
              heightDeltaOrIntensityA = modelNode->subtreeBoundingRadiusQ12;
            }
            g_GeneratedTextureScratchRuntime.samples[11].terrainRayDistanceQ12 = 0;
            radiusScaleOrCoordinate = (int)(((int64_t)(int)g_GraphicsShadingGridStepQ20Current *
                           (int64_t)
                           (int)((int)((uint64_t)
                                       ((int64_t)(int)heightDeltaOrIntensityA *
                                       (int64_t)g_FixedCosQ28[renderContext->viewAngleBC]) >> 0x20)
                                 << 3 | (uint32_t)((int64_t)(int)heightDeltaOrIntensityA *
                                              (int64_t)g_FixedCosQ28[renderContext->viewAngleBC])
                                        >> 0x1d)) / (int64_t)modelNode->subtreeBoundingRadiusQ12);
            g_GeneratedTextureScratchRuntime.samples[11].textureCoordinateOffsetQ20 =
                 g_GeneratedTextureScratchRuntime.samples[11].textureCoordinateOffsetQ20 - radiusScaleOrCoordinate;
            if (planeDotOrTextureOffset < radiusScaleOrCoordinate) {
              g_GeneratedTextureScratchRuntime.samples[11].textureCoordinateOffsetQ20 = 0;
              direction = FixedMath_DirectionFromAnglesScaledRegs
                                 (renderContext->viewAngleBC,renderContext->viewAngleB8,
                                  modelNode->subtreeBoundingRadiusQ12 * 2);
              g_GeneratedTextureScratchRuntime.samples[11].worldPoint.x =
                   g_GeneratedTextureScratchRuntime.samples[11].worldPoint.x - direction.x;
              g_GeneratedTextureScratchRuntime.samples[11].worldPoint.y =
                   g_GeneratedTextureScratchRuntime.samples[11].worldPoint.y - direction.y;
              g_GeneratedTextureScratchRuntime.samples[11].worldPoint.z =
                   g_GeneratedTextureScratchRuntime.samples[11].worldPoint.z - direction.z;
              terrainRay = FieldGrid_RaycastTerrainTrianglesAlongDirection
                                 (renderContext->viewAngleBC,renderContext->viewAngleB8,
                                  modelNode->subtreeBoundingRadiusQ12 * 2,
                                  g_GeneratedTextureScratchRuntime.samples[11].worldPoint.z,
                                  g_GeneratedTextureScratchRuntime.samples[11].worldPoint.y,
                                  g_GeneratedTextureScratchRuntime.samples[11].worldPoint.x,
                                  renderContext->fieldGrid);
              if (!terrainRay.hit) {
                return;
              }
              direction = FixedMath_DirectionFromAnglesScaledRegs
                                 (renderContext->viewAngleBC,renderContext->viewAngleB8,
                                  terrainRay.distanceQ12);
              g_GeneratedTextureScratchRuntime.samples[11].worldPoint.x =
                   g_GeneratedTextureScratchRuntime.samples[11].worldPoint.x + direction.x;
              g_GeneratedTextureScratchRuntime.samples[11].worldPoint.y =
                   g_GeneratedTextureScratchRuntime.samples[11].worldPoint.y + direction.y;
              g_GeneratedTextureScratchRuntime.samples[11].worldPoint.z =
                   g_GeneratedTextureScratchRuntime.samples[11].worldPoint.z + direction.z;
            }
            else {
              g_GeneratedTextureScratchRuntime.samples[11].worldPoint.z =
                   g_GeneratedTextureScratchRuntime.samples[11].worldPoint.z + heightDeltaOrIntensityA;
            }
          }
          reservedBlocks = GraphicsShadingGeneratedTexture_ReserveFourteenProjectedPointBlocks
                             (renderContext);
          projectedBlocks = reservedBlocks.firstBlock;
          if (!reservedBlocks.poolFull) {
            FixedTransform_ApplyPoint
                      ((GraphicsFixedVec3 *)(projectedBlocks + 0x65),
                       &g_GeneratedTextureScratchRuntime.samples[0].worldPoint,
                       &g_ViewProjectionMatrixFixed);
            FixedTransform_ApplyPoint
                      ((GraphicsFixedVec3 *)(projectedBlocks + 0xb1),
                       &g_GeneratedTextureScratchRuntime.samples[1].worldPoint,
                       &g_ViewProjectionMatrixFixed);
            FixedTransform_ApplyPoint
                      ((GraphicsFixedVec3 *)(projectedBlocks + 0x71),
                       &g_GeneratedTextureScratchRuntime.samples[2].worldPoint,
                       &g_ViewProjectionMatrixFixed);
            FixedTransform_ApplyPoint
                      ((GraphicsFixedVec3 *)(projectedBlocks + 0xa9),
                       &g_GeneratedTextureScratchRuntime.samples[3].worldPoint,
                       &g_ViewProjectionMatrixFixed);
            FixedTransform_ApplyPoint
                      ((GraphicsFixedVec3 *)(projectedBlocks + 0x55),
                       &g_GeneratedTextureScratchRuntime.samples[4].worldPoint,
                       &g_ViewProjectionMatrixFixed);
            FixedTransform_ApplyPoint
                      ((GraphicsFixedVec3 *)(projectedBlocks + 0x39),
                       &g_GeneratedTextureScratchRuntime.samples[5].worldPoint,
                       &g_ViewProjectionMatrixFixed);
            FixedTransform_ApplyPoint
                      ((GraphicsFixedVec3 *)(projectedBlocks + 0x29),
                       &g_GeneratedTextureScratchRuntime.samples[6].worldPoint,
                       &g_ViewProjectionMatrixFixed);
            FixedTransform_ApplyPoint
                      ((GraphicsFixedVec3 *)(projectedBlocks + 0x45),
                       &g_GeneratedTextureScratchRuntime.samples[7].worldPoint,
                       &g_ViewProjectionMatrixFixed);
            FixedTransform_ApplyPoint
                      ((GraphicsFixedVec3 *)(projectedBlocks + 1),
                       &g_GeneratedTextureScratchRuntime.samples[8].worldPoint,
                       &g_ViewProjectionMatrixFixed);
            FixedTransform_ApplyPoint
                      ((GraphicsFixedVec3 *)(projectedBlocks + 9),
                       &g_GeneratedTextureScratchRuntime.samples[9].worldPoint,
                       &g_ViewProjectionMatrixFixed);
            FixedTransform_ApplyPoint
                      ((GraphicsFixedVec3 *)(projectedBlocks + 5),
                       &g_GeneratedTextureScratchRuntime.samples[10].worldPoint,
                       &g_ViewProjectionMatrixFixed);
            FixedTransform_ApplyPoint
                      ((GraphicsFixedVec3 *)(projectedBlocks + 0x11),
                       &g_GeneratedTextureScratchRuntime.samples[11].worldPoint,
                       &g_ViewProjectionMatrixFixed);
            if ((((projectedBlocks[0x66].projectedX < (int)g_ProjectionScaleFixed) ||
                 (projectedBlocks[0xb2].projectedX < (int)g_ProjectionScaleFixed)) ||
                ((projectedBlocks[0x72].projectedX < (int)g_ProjectionScaleFixed ||
                 (((projectedBlocks[0xaa].projectedX < (int)g_ProjectionScaleFixed ||
                   (projectedBlocks[0x56].projectedX < (int)g_ProjectionScaleFixed)) ||
                  (projectedBlocks[0x3a].projectedX < (int)g_ProjectionScaleFixed)))))) ||
               (((projectedBlocks[0x2a].projectedX < (int)g_ProjectionScaleFixed ||
                 (projectedBlocks[0x46].projectedX < (int)g_ProjectionScaleFixed)) ||
                ((projectedBlocks[2].projectedX < (int)g_ProjectionScaleFixed ||
                 (((projectedBlocks[10].projectedX < (int)g_ProjectionScaleFixed ||
                   (projectedBlocks[6].projectedX < (int)g_ProjectionScaleFixed)) ||
                  (projectedBlocks[0x12].projectedX < (int)g_ProjectionScaleFixed)))))))) {
              GraphicsShadingGeneratedTexture_RollbackFourteenProjectedPointBlocks(renderContext);
            }
            else {
              pointPair0 = Graphics_ProjectViewPoint((GraphicsFixedVec3 *)(projectedBlocks + 0x65));
              projectedBlocks[100] = pointPair0;
              pointPair0 = Graphics_ProjectViewPoint((GraphicsFixedVec3 *)(projectedBlocks + 0xb1));
              projectedBlocks[0xb0] = pointPair0;
              pointPair0 = Graphics_ProjectViewPoint((GraphicsFixedVec3 *)(projectedBlocks + 0x71));
              projectedBlocks[0x70] = pointPair0;
              pointPair0 = Graphics_ProjectViewPoint((GraphicsFixedVec3 *)(projectedBlocks + 0xa9));
              projectedBlocks[0xa8] = pointPair0;
              pointPair0 = Graphics_ProjectViewPoint((GraphicsFixedVec3 *)(projectedBlocks + 0x55));
              projectedBlocks[0x54] = pointPair0;
              pointPair0 = Graphics_ProjectViewPoint((GraphicsFixedVec3 *)(projectedBlocks + 0x39));
              projectedBlocks[0x38] = pointPair0;
              pointPair0 = Graphics_ProjectViewPoint((GraphicsFixedVec3 *)(projectedBlocks + 0x29));
              projectedBlocks[0x28] = pointPair0;
              pointPair0 = Graphics_ProjectViewPoint((GraphicsFixedVec3 *)(projectedBlocks + 0x45));
              projectedBlocks[0x44] = pointPair0;
              pointPair0 = Graphics_ProjectViewPoint((GraphicsFixedVec3 *)(projectedBlocks + 1));
              *projectedBlocks = pointPair0;
              pointPair0 = Graphics_ProjectViewPoint((GraphicsFixedVec3 *)(projectedBlocks + 9));
              projectedBlocks[8] = pointPair0;
              pointPair0 = Graphics_ProjectViewPoint((GraphicsFixedVec3 *)(projectedBlocks + 5));
              projectedBlocks[4] = pointPair0;
              pointPair0 = Graphics_ProjectViewPoint((GraphicsFixedVec3 *)(projectedBlocks + 0x11));
              projectedBlocks[0x10] = pointPair0;
              directionYOrGridStep = g_GraphicsShadingGridStepQ20;
              directionXOrTileCoordinate = g_GraphicsShadingGeneratedTextureTileXQ20;
              halfDirectionYOrCoordinate = (g_GraphicsShadingGridStepQ20 - 0x1000) +
                       g_GraphicsShadingGeneratedTextureTileXQ20;
              radiusScaleOrCoordinate = ((int)g_GraphicsShadingGridStepQ20 >> 1) +
                       g_GraphicsShadingGeneratedTextureTileXQ20;
              projectedBlocks[0x66].projectedY = g_GraphicsShadingGeneratedTextureTileXQ20;
              projectedBlocks[0x72].projectedY = halfDirectionYOrCoordinate;
              projectedBlocks[0xb2].projectedY = directionXOrTileCoordinate;
              projectedBlocks[0xaa].projectedY = halfDirectionYOrCoordinate;
              projectedBlocks[0x56].projectedY = directionXOrTileCoordinate;
              projectedBlocks[0x3a].projectedY = halfDirectionYOrCoordinate;
              projectedBlocks[0x2a].projectedY = radiusScaleOrCoordinate;
              projectedBlocks[0x46].projectedY = radiusScaleOrCoordinate;
              halfDirectionYOrCoordinate = ((int)directionYOrGridStep >> 2) + radiusScaleOrCoordinate;
              radiusScaleOrCoordinate = radiusScaleOrCoordinate - ((int)directionYOrGridStep >> 2);
              projectedBlocks[2].projectedY = radiusScaleOrCoordinate;
              projectedBlocks[6].projectedY = halfDirectionYOrCoordinate;
              projectedBlocks[10].projectedY = radiusScaleOrCoordinate;
              projectedBlocks[0x12].projectedY = halfDirectionYOrCoordinate;
              directionXOrTileCoordinate = g_GraphicsShadingGeneratedTextureTileYQ20;
              halfDirectionZOrCoordinate = g_GeneratedTextureScratchRuntime.samples[1].textureCoordinateOffsetQ20 +
                       g_GraphicsShadingGeneratedTextureTileYQ20;
              radiusScaleOrCoordinate = g_GeneratedTextureScratchRuntime.samples[2].textureCoordinateOffsetQ20 +
                       g_GraphicsShadingGeneratedTextureTileYQ20;
              halfDirectionYOrCoordinate = g_GeneratedTextureScratchRuntime.samples[3].textureCoordinateOffsetQ20 +
                       g_GraphicsShadingGeneratedTextureTileYQ20;
              projectedBlocks[0x67].projectedX =
                   g_GeneratedTextureScratchRuntime.samples[0].textureCoordinateOffsetQ20 +
                   g_GraphicsShadingGeneratedTextureTileYQ20;
              projectedBlocks[0xb3].projectedX = halfDirectionZOrCoordinate;
              projectedBlocks[0x73].projectedX = radiusScaleOrCoordinate;
              projectedBlocks[0xab].projectedX = halfDirectionYOrCoordinate;
              halfDirectionZOrCoordinate = g_GeneratedTextureScratchRuntime.samples[5].textureCoordinateOffsetQ20 +
                       directionXOrTileCoordinate;
              radiusScaleOrCoordinate = g_GeneratedTextureScratchRuntime.samples[6].textureCoordinateOffsetQ20 +
                       directionXOrTileCoordinate;
              halfDirectionYOrCoordinate = g_GeneratedTextureScratchRuntime.samples[7].textureCoordinateOffsetQ20 +
                       directionXOrTileCoordinate;
              projectedBlocks[0x57].projectedX =
                   g_GeneratedTextureScratchRuntime.samples[4].textureCoordinateOffsetQ20 + directionXOrTileCoordinate;
              projectedBlocks[0x3b].projectedX = halfDirectionZOrCoordinate;
              projectedBlocks[0x2b].projectedX = radiusScaleOrCoordinate;
              projectedBlocks[0x47].projectedX = halfDirectionYOrCoordinate;
              halfDirectionZOrCoordinate = g_GeneratedTextureScratchRuntime.samples[9].textureCoordinateOffsetQ20 +
                       directionXOrTileCoordinate;
              radiusScaleOrCoordinate = g_GeneratedTextureScratchRuntime.samples[10].textureCoordinateOffsetQ20 +
                       directionXOrTileCoordinate;
              halfDirectionYOrCoordinate = g_GeneratedTextureScratchRuntime.samples[11].textureCoordinateOffsetQ20 +
                       directionXOrTileCoordinate;
              projectedBlocks[3].projectedX =
                   g_GeneratedTextureScratchRuntime.samples[8].textureCoordinateOffsetQ20 + directionXOrTileCoordinate;
              projectedBlocks[0xb].projectedX = halfDirectionZOrCoordinate;
              projectedBlocks[7].projectedX = radiusScaleOrCoordinate;
              projectedBlocks[0x13].projectedX = halfDirectionYOrCoordinate;
              projectedBlocks[0x66].projectedX = projectedBlocks[0x66].projectedX - 0x1000;
              projectedBlocks[0xb2].projectedX = projectedBlocks[0xb2].projectedX - 0x1000;
              projectedBlocks[0x72].projectedX = projectedBlocks[0x72].projectedX - 0x1000;
              projectedBlocks[0xaa].projectedX = projectedBlocks[0xaa].projectedX - 0x1000;
              projectedBlocks[0x56].projectedX = projectedBlocks[0x56].projectedX - 0x1000;
              projectedBlocks[0x3a].projectedX = projectedBlocks[0x3a].projectedX - 0x1000;
              projectedBlocks[0x2a].projectedX = projectedBlocks[0x2a].projectedX - 0x1000;
              projectedBlocks[0x46].projectedX = projectedBlocks[0x46].projectedX - 0x1000;
              projectedBlocks[2].projectedX = projectedBlocks[2].projectedX - 0x1000;
              projectedBlocks[10].projectedX = projectedBlocks[10].projectedX - 0x1000;
              projectedBlocks[6].projectedX = projectedBlocks[6].projectedX - 0x1000;
              projectedBlocks[0x12].projectedX = projectedBlocks[0x12].projectedX - 0x1000;
              /* Lanes 0..2 = 0x0fff, lane 3 = the halved tint alpha duplicated to a word, >> 4. */
              tintMaskOrIntensityB = modelNode->tintArgb >> 1 | 0xffffff;
              tintLanesOrShadedC = Shading_DuplicateBytesToWordLanes(tintMaskOrIntensityB,4);
              heightDeltaOrIntensityA = GRAPHICS_SHADING_SHADOW_FADE_DISTANCE_Q12 - g_GeneratedTextureScratchRuntime.samples[0].terrainRayDistanceQ12;
              if ((int)heightDeltaOrIntensityA < 0) {
                heightDeltaOrIntensityA = 0;
              }
              tintMaskOrIntensityB = GRAPHICS_SHADING_SHADOW_FADE_DISTANCE_Q12 - g_GeneratedTextureScratchRuntime.samples[1].terrainRayDistanceQ12;
              if ((int)tintMaskOrIntensityB < 0) {
                tintMaskOrIntensityB = 0;
              }
              intensityC = GRAPHICS_SHADING_SHADOW_FADE_DISTANCE_Q12 - g_GeneratedTextureScratchRuntime.samples[2].terrainRayDistanceQ12;
              if ((int)intensityC < 0) {
                intensityC = 0;
              }
              heightDeltaOrIntensityA = heightDeltaOrIntensityA >> 6;
              tintMaskOrIntensityB = tintMaskOrIntensityB >> 6;
              intensityC = intensityC >> 6;
              if (0xff < heightDeltaOrIntensityA) {
                heightDeltaOrIntensityA = 0xff;
              }
              if (0xff < tintMaskOrIntensityB) {
                tintMaskOrIntensityB = 0xff;
              }
              if (0xff < intensityC) {
                intensityC = 0xff;
              }
              shadedA = pmulhw(*(uint64_t *)(heightDeltaOrIntensityA * 8 + THANDOR_ADDR(g_ShadingIntensityScaleMmx,0)),tintLanesOrShadedC);
              shadedB = pmulhw(*(uint64_t *)(tintMaskOrIntensityB * 8 + THANDOR_ADDR(g_ShadingIntensityScaleMmx,0)),tintLanesOrShadedC);
              shadedC = pmulhw(*(uint64_t *)(intensityC * 8 + THANDOR_ADDR(g_ShadingIntensityScaleMmx,0)),tintLanesOrShadedC);
              projectedBlocks[0x67].projectedY = Shading_PackWordLanesUnsignedSaturate(shadedA);
              projectedBlocks[0xb3].projectedY = Shading_PackWordLanesUnsignedSaturate(shadedB);
              projectedBlocks[0x73].projectedY = Shading_PackWordLanesUnsignedSaturate(shadedC);
              heightDeltaOrIntensityA = GRAPHICS_SHADING_SHADOW_FADE_DISTANCE_Q12 - g_GeneratedTextureScratchRuntime.samples[3].terrainRayDistanceQ12;
              if ((int)heightDeltaOrIntensityA < 0) {
                heightDeltaOrIntensityA = 0;
              }
              tintMaskOrIntensityB = GRAPHICS_SHADING_SHADOW_FADE_DISTANCE_Q12 - g_GeneratedTextureScratchRuntime.samples[4].terrainRayDistanceQ12;
              if ((int)tintMaskOrIntensityB < 0) {
                tintMaskOrIntensityB = 0;
              }
              intensityC = GRAPHICS_SHADING_SHADOW_FADE_DISTANCE_Q12 - g_GeneratedTextureScratchRuntime.samples[5].terrainRayDistanceQ12;
              if ((int)intensityC < 0) {
                intensityC = 0;
              }
              heightDeltaOrIntensityA = heightDeltaOrIntensityA >> 6;
              tintMaskOrIntensityB = tintMaskOrIntensityB >> 6;
              intensityC = intensityC >> 6;
              if (0xff < heightDeltaOrIntensityA) {
                heightDeltaOrIntensityA = 0xff;
              }
              if (0xff < tintMaskOrIntensityB) {
                tintMaskOrIntensityB = 0xff;
              }
              if (0xff < intensityC) {
                intensityC = 0xff;
              }
              shadedA = pmulhw(*(uint64_t *)(heightDeltaOrIntensityA * 8 + THANDOR_ADDR(g_ShadingIntensityScaleMmx,0)),tintLanesOrShadedC);
              shadedB = pmulhw(*(uint64_t *)(tintMaskOrIntensityB * 8 + THANDOR_ADDR(g_ShadingIntensityScaleMmx,0)),tintLanesOrShadedC);
              shadedC = pmulhw(*(uint64_t *)(intensityC * 8 + THANDOR_ADDR(g_ShadingIntensityScaleMmx,0)),tintLanesOrShadedC);
              projectedBlocks[0xab].projectedY = Shading_PackWordLanesUnsignedSaturate(shadedA);
              projectedBlocks[0x57].projectedY = Shading_PackWordLanesUnsignedSaturate(shadedB);
              projectedBlocks[0x3b].projectedY = Shading_PackWordLanesUnsignedSaturate(shadedC);
              heightDeltaOrIntensityA = GRAPHICS_SHADING_SHADOW_FADE_DISTANCE_Q12 - g_GeneratedTextureScratchRuntime.samples[6].terrainRayDistanceQ12;
              if ((int)heightDeltaOrIntensityA < 0) {
                heightDeltaOrIntensityA = 0;
              }
              tintMaskOrIntensityB = GRAPHICS_SHADING_SHADOW_FADE_DISTANCE_Q12 - g_GeneratedTextureScratchRuntime.samples[7].terrainRayDistanceQ12;
              if ((int)tintMaskOrIntensityB < 0) {
                tintMaskOrIntensityB = 0;
              }
              intensityC = GRAPHICS_SHADING_SHADOW_FADE_DISTANCE_Q12 - g_GeneratedTextureScratchRuntime.samples[8].terrainRayDistanceQ12;
              if ((int)intensityC < 0) {
                intensityC = 0;
              }
              heightDeltaOrIntensityA = heightDeltaOrIntensityA >> 6;
              tintMaskOrIntensityB = tintMaskOrIntensityB >> 6;
              intensityC = intensityC >> 6;
              if (0xff < heightDeltaOrIntensityA) {
                heightDeltaOrIntensityA = 0xff;
              }
              if (0xff < tintMaskOrIntensityB) {
                tintMaskOrIntensityB = 0xff;
              }
              if (0xff < intensityC) {
                intensityC = 0xff;
              }
              shadedA = pmulhw(*(uint64_t *)(heightDeltaOrIntensityA * 8 + THANDOR_ADDR(g_ShadingIntensityScaleMmx,0)),tintLanesOrShadedC);
              shadedB = pmulhw(*(uint64_t *)(tintMaskOrIntensityB * 8 + THANDOR_ADDR(g_ShadingIntensityScaleMmx,0)),tintLanesOrShadedC);
              shadedC = pmulhw(*(uint64_t *)(intensityC * 8 + THANDOR_ADDR(g_ShadingIntensityScaleMmx,0)),tintLanesOrShadedC);
              projectedBlocks[0x2b].projectedY = Shading_PackWordLanesUnsignedSaturate(shadedA);
              projectedBlocks[0x47].projectedY = Shading_PackWordLanesUnsignedSaturate(shadedB);
              projectedBlocks[3].projectedY = Shading_PackWordLanesUnsignedSaturate(shadedC);
              heightDeltaOrIntensityA = GRAPHICS_SHADING_SHADOW_FADE_DISTANCE_Q12 - g_GeneratedTextureScratchRuntime.samples[9].terrainRayDistanceQ12;
              if ((int)heightDeltaOrIntensityA < 0) {
                heightDeltaOrIntensityA = 0;
              }
              tintMaskOrIntensityB = GRAPHICS_SHADING_SHADOW_FADE_DISTANCE_Q12 - g_GeneratedTextureScratchRuntime.samples[10].terrainRayDistanceQ12;
              if ((int)tintMaskOrIntensityB < 0) {
                tintMaskOrIntensityB = 0;
              }
              intensityC = GRAPHICS_SHADING_SHADOW_FADE_DISTANCE_Q12 - g_GeneratedTextureScratchRuntime.samples[11].terrainRayDistanceQ12;
              if ((int)intensityC < 0) {
                intensityC = 0;
              }
              heightDeltaOrIntensityA = heightDeltaOrIntensityA >> 6;
              tintMaskOrIntensityB = tintMaskOrIntensityB >> 6;
              intensityC = intensityC >> 6;
              if (0xff < heightDeltaOrIntensityA) {
                heightDeltaOrIntensityA = 0xff;
              }
              if (0xff < tintMaskOrIntensityB) {
                tintMaskOrIntensityB = 0xff;
              }
              if (0xff < intensityC) {
                intensityC = 0xff;
              }
              shadedA = pmulhw(*(uint64_t *)(heightDeltaOrIntensityA * 8 + THANDOR_ADDR(g_ShadingIntensityScaleMmx,0)),tintLanesOrShadedC);
              shadedB = pmulhw(*(uint64_t *)(tintMaskOrIntensityB * 8 + THANDOR_ADDR(g_ShadingIntensityScaleMmx,0)),tintLanesOrShadedC);
              tintLanesOrShadedC = pmulhw(*(uint64_t *)(intensityC * 8 + THANDOR_ADDR(g_ShadingIntensityScaleMmx,0)),tintLanesOrShadedC);
              projectedBlocks[0xb].projectedY = Shading_PackWordLanesUnsignedSaturate(shadedA);
              projectedBlocks[7].projectedY = Shading_PackWordLanesUnsignedSaturate(shadedB);
              projectedBlocks[0x13].projectedY = Shading_PackWordLanesUnsignedSaturate(tintLanesOrShadedC);
              projectedBlocks[0xc].projectedY =
                   (GraphicsPrimitiveBackendCoordinate)
                   (g_GraphicsShadingTextureSet->entries +
                   g_GraphicsShadingGeneratedTextureSubresourceIndex);
              pointPair0 = *projectedBlocks;
              pointPair1 = projectedBlocks[1];
              pointPair2 = projectedBlocks[2];
              pointPair3 = projectedBlocks[3];
              projectedBlocks[0x20] = pointPair0;
              projectedBlocks[0x21] = pointPair1;
              projectedBlocks[0x22] = pointPair2;
              projectedBlocks[0x23] = pointPair3;
              projectedBlocks[0x50] = pointPair0;
              projectedBlocks[0x51] = pointPair1;
              projectedBlocks[0x52] = pointPair2;
              projectedBlocks[0x53] = pointPair3;
              projectedBlocks[0x60] = pointPair0;
              projectedBlocks[0x61] = pointPair1;
              projectedBlocks[0x62] = pointPair2;
              projectedBlocks[99] = pointPair3;
              projectedBlocks[0xd0] = pointPair0;
              projectedBlocks[0xd1] = pointPair1;
              projectedBlocks[0xd2] = pointPair2;
              projectedBlocks[0xd3] = pointPair3;
              pointPair0 = projectedBlocks[4];
              pointPair1 = projectedBlocks[5];
              pointPair2 = projectedBlocks[6];
              pointPair3 = projectedBlocks[7];
              projectedBlocks[0x14] = pointPair0;
              projectedBlocks[0x15] = pointPair1;
              projectedBlocks[0x16] = pointPair2;
              projectedBlocks[0x17] = pointPair3;
              projectedBlocks[0x24] = pointPair0;
              projectedBlocks[0x25] = pointPair1;
              projectedBlocks[0x26] = pointPair2;
              projectedBlocks[0x27] = pointPair3;
              projectedBlocks[0x34] = pointPair0;
              projectedBlocks[0x35] = pointPair1;
              projectedBlocks[0x36] = pointPair2;
              projectedBlocks[0x37] = pointPair3;
              projectedBlocks[0x74] = pointPair0;
              projectedBlocks[0x75] = pointPair1;
              projectedBlocks[0x76] = pointPair2;
              projectedBlocks[0x77] = pointPair3;
              projectedBlocks[0x84] = pointPair0;
              projectedBlocks[0x85] = pointPair1;
              projectedBlocks[0x86] = pointPair2;
              projectedBlocks[0x87] = pointPair3;
              pointPair0 = projectedBlocks[8];
              pointPair1 = projectedBlocks[9];
              pointPair2 = projectedBlocks[10];
              pointPair3 = projectedBlocks[0xb];
              projectedBlocks[0x18] = pointPair0;
              projectedBlocks[0x19] = pointPair1;
              projectedBlocks[0x1a] = pointPair2;
              projectedBlocks[0x1b] = pointPair3;
              projectedBlocks[0x48] = pointPair0;
              projectedBlocks[0x49] = pointPair1;
              projectedBlocks[0x4a] = pointPair2;
              projectedBlocks[0x4b] = pointPair3;
              projectedBlocks[0x58] = pointPair0;
              projectedBlocks[0x59] = pointPair1;
              projectedBlocks[0x5a] = pointPair2;
              projectedBlocks[0x5b] = pointPair3;
              projectedBlocks[0xb8] = pointPair0;
              projectedBlocks[0xb9] = pointPair1;
              projectedBlocks[0xba] = pointPair2;
              projectedBlocks[0xbb] = pointPair3;
              projectedBlocks[200] = pointPair0;
              projectedBlocks[0xc9] = pointPair1;
              projectedBlocks[0xca] = pointPair2;
              projectedBlocks[0xcb] = pointPair3;
              pointPair0 = projectedBlocks[0x10];
              pointPair1 = projectedBlocks[0x11];
              pointPair2 = projectedBlocks[0x12];
              pointPair3 = projectedBlocks[0x13];
              projectedBlocks[0x30] = pointPair0;
              projectedBlocks[0x31] = pointPair1;
              projectedBlocks[0x32] = pointPair2;
              projectedBlocks[0x33] = pointPair3;
              projectedBlocks[0x40] = pointPair0;
              projectedBlocks[0x41] = pointPair1;
              projectedBlocks[0x42] = pointPair2;
              projectedBlocks[0x43] = pointPair3;
              projectedBlocks[0x90] = pointPair0;
              projectedBlocks[0x91] = pointPair1;
              projectedBlocks[0x92] = pointPair2;
              projectedBlocks[0x93] = pointPair3;
              projectedBlocks[0xa0] = pointPair0;
              projectedBlocks[0xa1] = pointPair1;
              projectedBlocks[0xa2] = pointPair2;
              projectedBlocks[0xa3] = pointPair3;
              projectedBlocks[0x68] = projectedBlocks[0x28];
              projectedBlocks[0x69] = projectedBlocks[0x29];
              projectedBlocks[0x6a] = projectedBlocks[0x2a];
              projectedBlocks[0x6b] = projectedBlocks[0x2b];
              projectedBlocks[0x78] = projectedBlocks[0x28];
              projectedBlocks[0x79] = projectedBlocks[0x29];
              projectedBlocks[0x7a] = projectedBlocks[0x2a];
              projectedBlocks[0x7b] = projectedBlocks[0x2b];
              projectedBlocks[0x88] = projectedBlocks[0x38];
              projectedBlocks[0x89] = projectedBlocks[0x39];
              projectedBlocks[0x8a] = projectedBlocks[0x3a];
              projectedBlocks[0x8b] = projectedBlocks[0x3b];
              projectedBlocks[0x98] = projectedBlocks[0x38];
              projectedBlocks[0x99] = projectedBlocks[0x39];
              projectedBlocks[0x9a] = projectedBlocks[0x3a];
              projectedBlocks[0x9b] = projectedBlocks[0x3b];
              projectedBlocks[0xa4] = projectedBlocks[0x44];
              projectedBlocks[0xa5] = projectedBlocks[0x45];
              projectedBlocks[0xa6] = projectedBlocks[0x46];
              projectedBlocks[0xa7] = projectedBlocks[0x47];
              projectedBlocks[0xb4] = projectedBlocks[0x44];
              projectedBlocks[0xb5] = projectedBlocks[0x45];
              projectedBlocks[0xb6] = projectedBlocks[0x46];
              projectedBlocks[0xb7] = projectedBlocks[0x47];
              projectedBlocks[0xc4] = projectedBlocks[0x54];
              projectedBlocks[0xc5] = projectedBlocks[0x55];
              projectedBlocks[0xc6] = projectedBlocks[0x56];
              projectedBlocks[199] = projectedBlocks[0x57];
              projectedBlocks[0xd4] = projectedBlocks[0x54];
              projectedBlocks[0xd5] = projectedBlocks[0x55];
              projectedBlocks[0xd6] = projectedBlocks[0x56];
              projectedBlocks[0xd7] = projectedBlocks[0x57];
              projectedBlocks[0xd8] = projectedBlocks[100];
              projectedBlocks[0xd9] = projectedBlocks[0x65];
              projectedBlocks[0xda] = projectedBlocks[0x66];
              projectedBlocks[0xdb] = projectedBlocks[0x67];
              projectedBlocks[0x80] = projectedBlocks[0x70];
              projectedBlocks[0x81] = projectedBlocks[0x71];
              projectedBlocks[0x82] = projectedBlocks[0x72];
              projectedBlocks[0x83] = projectedBlocks[0x73];
              projectedBlocks[0x94] = projectedBlocks[0xa8];
              projectedBlocks[0x95] = projectedBlocks[0xa9];
              projectedBlocks[0x96] = projectedBlocks[0xaa];
              projectedBlocks[0x97] = projectedBlocks[0xab];
              projectedBlocks[0xc0] = projectedBlocks[0xb0];
              projectedBlocks[0xc1] = projectedBlocks[0xb1];
              projectedBlocks[0xc2] = projectedBlocks[0xb2];
              projectedBlocks[0xc3] = projectedBlocks[0xb3];
              pointPair0 = projectedBlocks[0xc];
              pointPair1 = projectedBlocks[0xd];
              pointPair2 = projectedBlocks[0xe];
              pointPair3 = projectedBlocks[0xf];
              projectedBlocks[0x1c] = pointPair0;
              projectedBlocks[0x1d] = pointPair1;
              projectedBlocks[0x1e] = pointPair2;
              projectedBlocks[0x1f] = pointPair3;
              projectedBlocks[0x2c] = pointPair0;
              projectedBlocks[0x2d] = pointPair1;
              projectedBlocks[0x2e] = pointPair2;
              projectedBlocks[0x2f] = pointPair3;
              projectedBlocks[0x3c] = pointPair0;
              projectedBlocks[0x3d] = pointPair1;
              projectedBlocks[0x3e] = pointPair2;
              projectedBlocks[0x3f] = pointPair3;
              projectedBlocks[0x4c] = pointPair0;
              projectedBlocks[0x4d] = pointPair1;
              projectedBlocks[0x4e] = pointPair2;
              projectedBlocks[0x4f] = pointPair3;
              projectedBlocks[0x5c] = pointPair0;
              projectedBlocks[0x5d] = pointPair1;
              projectedBlocks[0x5e] = pointPair2;
              projectedBlocks[0x5f] = pointPair3;
              projectedBlocks[0x6c] = pointPair0;
              projectedBlocks[0x6d] = pointPair1;
              projectedBlocks[0x6e] = pointPair2;
              projectedBlocks[0x6f] = pointPair3;
              projectedBlocks[0x7c] = pointPair0;
              projectedBlocks[0x7d] = pointPair1;
              projectedBlocks[0x7e] = pointPair2;
              projectedBlocks[0x7f] = pointPair3;
              projectedBlocks[0x8c] = pointPair0;
              projectedBlocks[0x8d] = pointPair1;
              projectedBlocks[0x8e] = pointPair2;
              projectedBlocks[0x8f] = pointPair3;
              projectedBlocks[0x9c] = pointPair0;
              projectedBlocks[0x9d] = pointPair1;
              projectedBlocks[0x9e] = pointPair2;
              projectedBlocks[0x9f] = pointPair3;
              projectedBlocks[0xac] = pointPair0;
              projectedBlocks[0xad] = pointPair1;
              projectedBlocks[0xae] = pointPair2;
              projectedBlocks[0xaf] = pointPair3;
              projectedBlocks[0xbc] = pointPair0;
              projectedBlocks[0xbd] = pointPair1;
              projectedBlocks[0xbe] = pointPair2;
              projectedBlocks[0xbf] = pointPair3;
              projectedBlocks[0xcc] = pointPair0;
              projectedBlocks[0xcd] = pointPair1;
              projectedBlocks[0xce] = pointPair2;
              projectedBlocks[0xcf] = pointPair3;
              projectedBlocks[0xdc] = pointPair0;
              projectedBlocks[0xdd] = pointPair1;
              projectedBlocks[0xde] = pointPair2;
              projectedBlocks[0xdf] = pointPair3;
              /* The original tests EBX as left by the traversal (0x004D09CD); the decompile tested a
                 stale local instead. */
              if (GraphicsShadingGeneratedTexture_TraverseHierarchyFlag0Set(modelNode) != 0) {
                GraphicsShadingGeneratedTexture_FilterGridScratchMmx();
              }
              GraphicsShadingGeneratedTexture_TraverseHierarchyFlag0Clear(modelNode);
              GraphicsShadingGeneratedTexture_AdvanceTileCursor();
            }
          }
        }
      }
    }
  }
  return;
}


/* Address: 0x004BCF70.
   Builds the 64 KiB intensity clamp table used by the model tint fade
   (ModelNodeRuntime_UpdateStateTintRecursive): entry (previous << 8) | target holds target limited to previous
   +/- GRAPHICS_INTENSITY_CLAMP_MAX_STEP. The table is aligned to 64 KiB so the original can index it with a
   16-bit register pair. CF set with the arena error when the allocation fails.
*/
StatusResult GraphicsIntensityClampTable_Initialize(void)

{
  int rowsRemaining;
  char targetByte;
  uint32_t targetIntensity;
  int previousIntensity;
  char *tableCursor;
  ArenaAllocResult allocResult;
  
  allocResult = g_MemoryApi.alloc(GRAPHICS_INTENSITY_CLAMP_ALLOCATION_BYTES);
  if (!allocResult.failed) {
    targetIntensity = 0;
    /* round up to the next 64 KiB boundary */
    tableCursor = (char *)(allocResult.payloadOrError + 0xffff & 0xffff0000);
    rowsRemaining = 256;
    previousIntensity = 0;
    g_GraphicsIntensityClampTableBase = (uint32_t)tableCursor;
    do {
      do {
        targetByte = (char)targetIntensity;
        if (previousIntensity < (int)targetIntensity) {
          if (previousIntensity + GRAPHICS_INTENSITY_CLAMP_MAX_STEP < (int)targetIntensity) {
            *tableCursor = (char)(previousIntensity + GRAPHICS_INTENSITY_CLAMP_MAX_STEP);
          }
          else {
            *tableCursor = targetByte;
          }
        }
        else if ((int)targetIntensity < previousIntensity - GRAPHICS_INTENSITY_CLAMP_MAX_STEP) {
          *tableCursor = (char)(previousIntensity - GRAPHICS_INTENSITY_CLAMP_MAX_STEP);
        }
        else {
          *tableCursor = targetByte;
        }
        tableCursor = tableCursor + 1;
        /* 8-bit wrap ends the row after target 255 (INC DL; JNZ in the original) */
        targetIntensity = (uint32_t)(uint8_t)(targetByte + 1U);
      } while ((uint8_t)(targetByte + 1U) != 0);
      previousIntensity++;
      rowsRemaining--;
    } while (rowsRemaining != 0);
    return StatusValue_Ok(0);
  }
  return StatusValue_Fail(allocResult.payloadOrError);
}


/* Address: 0x004CCA90.
   Adds the light of every active compact light record (view space, see
   GraphicsShadingRuntime_RebuildCompactLightingRecords) whose sphere contains worldPointQ12 to the packed
   MMX light accumulator (MM1 in the original), with unsigned saturation. The strength comes from
   g_PackedLightingLookupTable indexed by (radius^2 - distance^2) / radius^2, so it falls off towards the
   sphere edge. Used by the terrain vertex shading in src/world/terrain/projection.c.
   Original register convention: ECX and EDX preserved; the packed value lives in MMX register MM1.
*/
MmxPackedValue64 GraphicsShadingRuntime_AccumulateCompactLightingAtPointMmxRegs
          (GraphicsFixedVec3 *worldPointQ12,MmxPackedValue64 packedLightAccumulatorMmx)

{
  PackedRgb24 packedColor;
  int64_t axisDeltaSquared;
  int remainingHigh;
  uint32_t remainingLowOrSquareLow;
  int axisDelta;
  uint32_t squareLow;
  GraphicsShadingRecordCount recordsRemaining;
  uint32_t remainingLowOrRadiusScale;
  GraphicsShadingRuntimeRecord *shadingRecord;
  uint64_t scaledLight;
  
  shadingRecord = g_GraphicsShadingCompactRecords;
  for (recordsRemaining = g_GraphicsShadingCompactRecordCount; recordsRemaining != 0; recordsRemaining--) {
    if (shadingRecord->targetRadiusQ12 != 0) {
      /* 64-bit radius^2 - dx^2 - dy^2 - dz^2, giving up as soon as it turns negative */
      squareLow = (uint32_t)shadingRecord->squaredRadiusQ24;
      remainingHigh = worldPointQ12->x - shadingRecord->worldXQ12;
      axisDeltaSquared = (int64_t)remainingHigh * (int64_t)remainingHigh;
      remainingLowOrSquareLow = (uint32_t)axisDeltaSquared;
      remainingLowOrRadiusScale = squareLow - remainingLowOrSquareLow;
      remainingHigh = (*(int *)((int)&shadingRecord->squaredRadiusQ24 + 4) -
              (int)((uint64_t)axisDeltaSquared >> 0x20)) - (uint32_t)(squareLow < remainingLowOrSquareLow);
      if (-1 < remainingHigh) {
        axisDelta = worldPointQ12->y - shadingRecord->worldYQ12;
        axisDeltaSquared = (int64_t)axisDelta * (int64_t)axisDelta;
        squareLow = (uint32_t)axisDeltaSquared;
        remainingLowOrSquareLow = remainingLowOrRadiusScale - squareLow;
        remainingHigh = (remainingHigh - (int)((uint64_t)axisDeltaSquared >> 0x20)) - (uint32_t)(remainingLowOrRadiusScale < squareLow);
        if (-1 < remainingHigh) {
          axisDelta = worldPointQ12->z - shadingRecord->worldZQ12;
          axisDeltaSquared = (int64_t)axisDelta * (int64_t)axisDelta;
          squareLow = (uint32_t)axisDeltaSquared;
          remainingHigh = (remainingHigh - (int)((uint64_t)axisDeltaSquared >> 0x20)) - (uint32_t)(remainingLowOrSquareLow < squareLow);
          if (-1 < remainingHigh) {
            packedColor = shadingRecord->packedColorRgbActive;
            remainingLowOrRadiusScale = *(int *)((int)&shadingRecord->squaredRadiusQ24 + 4) << 0x14 |
                    (uint32_t)shadingRecord->squaredRadiusQ24 >> 0xc;
            if (remainingLowOrRadiusScale != 0) {
              scaledLight = pmulhw(Shading_DuplicateBytesToWordLanes(packedColor,2),
                              g_PackedLightingLookupTable[(remainingHigh * 0x8000000 | remainingLowOrSquareLow - squareLow >> 5) / remainingLowOrRadiusScale]);
              packedLightAccumulatorMmx = paddusw(packedLightAccumulatorMmx,scaledLight);
            }
          }
        }
      }
    }
    shadingRecord++;
  }
  return packedLightAccumulatorMmx;
}


/* Address: 0x004CCB40.
   Claims the first free runtime light record (colour 0) for a point light at the given world position and
   returns it. With transitionDurationTicks 0 the light starts at full radius, otherwise its squared radius
   starts at 0 and grows over that many ticks. CF set with NULL when packedColorRgb is 0 or all
   GRAPHICS_SHADING_RUNTIME_RECORD_COUNT records are taken.
*/
ShadingRecordResult GraphicsShadingRuntime_AllocateRecordRegs
          (GraphicsTransitionTickCount transitionDurationTicks,GraphicsRadiusQ12 radiusQ12,
          PackedRgb24 packedColorRgb,GraphicsWorldCoordinateQ12 worldZQ12,
          GraphicsWorldCoordinateQ12 worldYQ12,GraphicsWorldCoordinateQ12 worldXQ12)

{
  int recordsRemaining;
  GraphicsShadingRuntimeRecord *recordCursor;
  ShadingRecordResult failureResult;
  ShadingRecordResult successResult;
  
  if (packedColorRgb != 0) {
    recordCursor = g_GraphicsShadingRuntimeRecords;
    recordsRemaining = GRAPHICS_SHADING_RUNTIME_RECORD_COUNT;
    do {
      if (recordCursor->packedColorRgbActive == 0) {
        recordCursor->targetRadiusQ12 = radiusQ12;
        recordCursor->packedColorRgbActive = packedColorRgb & 0xffffff;
        if (transitionDurationTicks == 0) {
          recordCursor->squaredRadiusQ24 = (int64_t)radiusQ12 * (int64_t)radiusQ12;
          recordCursor->radiusTransitionDurationTicks = 0;
          recordCursor->radiusTransitionElapsedTicks = 0;
        }
        else {
          recordCursor->squaredRadiusQ24 = 0;
          recordCursor->radiusTransitionDurationTicks = transitionDurationTicks;
          recordCursor->radiusTransitionElapsedTicks = 0;
        }
        recordCursor->worldXQ12 = worldXQ12;
        recordCursor->worldYQ12 = worldYQ12;
        recordCursor->worldZQ12 = worldZQ12;
        successResult.failed = false;
        successResult.record = recordCursor;
        return successResult;
      }
      recordCursor = recordCursor + 1;
      recordsRemaining--;
    } while (recordsRemaining != 0);
  }
  failureResult.record = NULL;
  failureResult.failed = true;
  return failureResult;
}


/* Address: 0x004CCC60.
   Zeroes the 256 runtime light records (0x40 bytes each, 0x4000 bytes in total) so that no light source is
   active; GraphicsShadingRuntime_RebuildCompactLightingRecords only picks up records with a colour set.
*/
void GraphicsShadingRuntime_ClearRecordTable(void)

{
  int recordDwordsRemaining;
  GraphicsShadingRuntimeRecord *recordDwordCursor;

  recordDwordCursor = g_GraphicsShadingRuntimeRecords;
  for (recordDwordsRemaining = 0x1000; recordDwordsRemaining != 0;
      recordDwordsRemaining--) {
    recordDwordCursor->worldXQ12 = 0;
    recordDwordCursor = (GraphicsShadingRuntimeRecord *)&recordDwordCursor->worldYQ12;
  }
  return;
}


/* Address: 0x004CCD00.
   Once per rendered world frame (frontend world render in src/ui/frontend/runtime.c): copies every active
   runtime light record (colour set) into the compact table with its position transformed into view space,
   and publishes the count, so the per-vertex and per-model light queries only walk the live lights.
*/
void GraphicsShadingRuntime_RebuildCompactLightingRecords(void)

{
  GraphicsRadiusQ12 targetRadius;
  GraphicsShadingRecordCount compactCount;
  int recordsRemaining;
  GraphicsShadingRuntimeRecord *sourceRecord;
  GraphicsShadingRuntimeRecord *compactRecord;
  
  sourceRecord = g_GraphicsShadingRuntimeRecords;
  compactRecord = g_GraphicsShadingCompactRecords;
  recordsRemaining = GRAPHICS_SHADING_RUNTIME_RECORD_COUNT;
  compactCount = 0;
  do {
    if (sourceRecord->packedColorRgbActive != 0) {
      FixedTransform_ApplyPoint
                ((GraphicsFixedVec3 *)compactRecord,(GraphicsFixedVec3 *)sourceRecord,
                 &g_ViewProjectionMatrixFixed);
      targetRadius = sourceRecord->targetRadiusQ12;
      compactRecord->packedColorRgbActive = sourceRecord->packedColorRgbActive;
      compactRecord->targetRadiusQ12 = targetRadius;
      compactCount++;
      compactRecord->squaredRadiusQ24 = sourceRecord->squaredRadiusQ24;
      compactRecord++;
    }
    sourceRecord++;
    recordsRemaining--;
  } while (recordsRemaining != 0);
  g_GraphicsShadingCompactRecordCount = compactCount;
  return;
}


/* Address: 0x004CCD70.
   Copies every compact light record whose sphere overlaps the query sphere (distance^2 <= (queryRadius +
   lightRadius)^2, compared in 64 bits) into g_GraphicsShadingNearbyRecords and publishes
   g_GraphicsShadingNearbyRecordCount, so model vertex lighting (src/graphics/render/model.c) only tests
   the lights near the model. Called per model node by the hierarchy renderers in src/world/model/runtime.c.
*/
void GraphicsShadingRuntime_CollectNearbyRecords(GraphicsRadiusQ12 queryRadiusQ12,GraphicsWorldCoordinateQ12 worldZQ12,
          GraphicsWorldCoordinateQ12 worldYQ12,GraphicsWorldCoordinateQ12 worldXQ12)

{
  int deltaXRadiusOrCounter;
  int deltaYQ12;
  int deltaZQ12;
  uint32_t sourceRecordsRemaining;
  GraphicsShadingRuntimeRecord *sourceRecordCursor;
  GraphicsShadingRuntimeRecord *destinationRecordCursor;
  int64_t combinedRadiusSquaredQ24;
  int64_t distanceSquaredQ24;
  
  sourceRecordCursor = g_GraphicsShadingCompactRecords;
  destinationRecordCursor = g_GraphicsShadingNearbyRecords;
  g_GraphicsShadingNearbyRecordCount = 0;
  for (sourceRecordsRemaining = g_GraphicsShadingCompactRecordCount; sourceRecordsRemaining != 0;
      sourceRecordsRemaining--) {
    deltaXRadiusOrCounter = worldXQ12 - sourceRecordCursor->worldXQ12;
    deltaYQ12 = worldYQ12 - sourceRecordCursor->worldYQ12;
    deltaZQ12 = worldZQ12 - sourceRecordCursor->worldZQ12;
    distanceSquaredQ24 =
         (int64_t)deltaYQ12 * (int64_t)deltaYQ12 + (int64_t)deltaXRadiusOrCounter * (int64_t)deltaXRadiusOrCounter +
         (int64_t)deltaZQ12 * (int64_t)deltaZQ12;
    deltaXRadiusOrCounter = queryRadiusQ12 + sourceRecordCursor->targetRadiusQ12;
    combinedRadiusSquaredQ24 = (int64_t)deltaXRadiusOrCounter * (int64_t)deltaXRadiusOrCounter;
    if ((int)(((int)((uint64_t)combinedRadiusSquaredQ24 >> 0x20) -
              (int)((uint64_t)distanceSquaredQ24 >> 0x20)) -
             (uint32_t)((uint32_t)combinedRadiusSquaredQ24 < (uint32_t)distanceSquaredQ24)) < 0) {
      sourceRecordCursor++;
    }
    else {
      g_GraphicsShadingNearbyRecordCount++;
      /* copy the whole 0x40-byte record dword by dword (REP MOVSD); the source cursor advances with it */
      for (deltaXRadiusOrCounter = 0x10; deltaXRadiusOrCounter != 0; deltaXRadiusOrCounter--) {
        destinationRecordCursor->worldXQ12 = sourceRecordCursor->worldXQ12;
        sourceRecordCursor = (GraphicsShadingRuntimeRecord *)&sourceRecordCursor->worldYQ12;
        destinationRecordCursor =
             (GraphicsShadingRuntimeRecord *)&destinationRecordCursor->worldYQ12;
      }
    }
  }
  return;
}


/* Address: 0x004CCFF0.
   Sets up the generated shading textures: a zeroed square scratch grid of (2 * gridHalfSize)^2 bytes and
   an in-memory gfx asset with one palette (white with an alpha ramp) and subresourceCount 8-bit images of
   textureDimension^2 pixels, from which a texture set is created. Also derives the grid step and origin used to
   map world positions into the textures. Returns the allocator/texture error with CF set on failure.
*/
StatusResult GraphicsShadingRuntime_InitializeGeneratedTexture
          (GraphicsAssetSubresourceCount subresourceCount,GraphicsPixelDimension gridHalfSize,
          GraphicsPixelDimension textureDimension)

{
  GraphicsGeneratedTextureAssetOrEntryView200 *allocationCursor;
  AssetMagic paletteEntry;
  uint32_t allocationSize;
  uint32_t dwordsRemaining;
  int counterOrGridOrigin;
  AssetRelativeOffset pixelDataOffset;
  GraphicsGeneratedTextureAssetOrEntryView200 *entryCursor;
  ArenaAllocResult allocResult;
  TextureSetResult textureSetResult;
  StatusResult failureStatus;
  
  allocationSize = gridHalfSize * 2 * gridHalfSize * 2;
  allocResult = g_MemoryApi.alloc(allocationSize);
  allocationCursor = (GraphicsGeneratedTextureAssetOrEntryView200 *)allocResult.payloadOrError;
  if (!allocResult.failed) {
    allocationSize = allocationSize >> 2;
    g_GraphicsShadingGridScratchInterior = (pointer)((int)allocationCursor + allocationSize + (gridHalfSize >> 1));
    g_GraphicsShadingGridScratch = allocationCursor;
    for (; allocationSize != 0; allocationSize--) {
      (allocationCursor->asset).common.magic = 0;
      allocationCursor = (GraphicsGeneratedTextureAssetOrEntryView200 *)
               &(allocationCursor->asset).common.allocationSizeBytes;
    }
    /* gfx layout: header and palette up to 0xA00, then one 0x20-byte source entry per image, then the pixels */
    allocationSize = (textureDimension * textureDimension + 0x20) * subresourceCount + 0xa00;
    allocResult = g_MemoryApi.alloc(allocationSize);
    allocationCursor = (GraphicsGeneratedTextureAssetOrEntryView200 *)allocResult.payloadOrError;
    if (!allocResult.failed) {
      entryCursor = allocationCursor;
      g_GraphicsShadingGeneratedAsset = (GraphicsTextureSourceAsset *)allocationCursor;
      for (dwordsRemaining = allocationSize >> 2; dwordsRemaining != 0; dwordsRemaining--) {
        (entryCursor->asset).common.magic = 0;
        entryCursor = (GraphicsGeneratedTextureAssetOrEntryView200 *)
                 &(entryCursor->asset).common.allocationSizeBytes;
      }
      (allocationCursor->asset).common.magic = ASSET_MAGIC_GFX;
      (allocationCursor->asset).common.allocationSizeBytes = allocationSize;
      (allocationCursor->asset).tableDescriptor.subresourceCount = subresourceCount;
      (allocationCursor->asset).tableDescriptor.paletteBankCount = 1;
      (allocationCursor->asset).tableDescriptor.subresourceTableOffset = 0xa00;
      /* palette at +0x200: 256 ARGB entries 8 bytes apart (up to 0xA00), white with alpha = index */
      paletteEntry = 0xffffff;
      entryCursor = allocationCursor + 1;
      counterOrGridOrigin = 0x100;
      do {
        (entryCursor->asset).common.magic = paletteEntry;
        paletteEntry = paletteEntry + 0x1000000;
        entryCursor = (GraphicsGeneratedTextureAssetOrEntryView200 *)
                 &(entryCursor->asset).common.formatVersion;
        counterOrGridOrigin--;
      } while (counterOrGridOrigin != 0);
      g_GraphicsShadingSubresourceCount = subresourceCount;
      entryCursor = allocationCursor + 5; /* source entry table at 0xA00 */
      pixelDataOffset = subresourceCount * 0x20 + 0xa00;
      do {
        (entryCursor->asset).common.magic = textureDimension;
        (entryCursor->asset).common.allocationSizeBytes = textureDimension;
        (entryCursor->asset).common.formatVersion = 0;
        (entryCursor->sourceEntry).dataOffset = pixelDataOffset;
        (entryCursor->asset).common.buildMetadata.timestamps.dateValue0 = 0;
        (entryCursor->asset).common.buildMetadata.timestamps.timeValue0 = 0;
        (entryCursor->asset).common.buildMetadata.timestamps.dateValue1 = textureDimension;
        (entryCursor->asset).common.buildMetadata.timestamps.timeValue1 = textureDimension;
        pixelDataOffset = pixelDataOffset + textureDimension * textureDimension;
        entryCursor = (GraphicsGeneratedTextureAssetOrEntryView200 *)
                 &(entryCursor->asset).common.buildMetadata.timestamps.dateValue2;
        subresourceCount--;
      } while (subresourceCount != 0);
      g_GraphicsShadingTextureDimension = textureDimension;
      g_GraphicsShadingGridHalfSize = gridHalfSize;
      /* grid cells per texture pixel in Q20, and the grid origin (gridHalfSize / 2 - 1 cells) in Q12 */
      g_GraphicsShadingGridStepQ20 =
           (uint32_t)(((uint64_t)gridHalfSize * 0x100000) / (uint64_t)textureDimension);
      counterOrGridOrigin = ((int)gridHalfSize >> 1) - 1;
      g_GraphicsShadingPositiveGridOriginQ12 = counterOrGridOrigin * 0x1000;
      g_GraphicsShadingNegativeGridOriginQ12 = counterOrGridOrigin * -0x1000;
      g_GraphicsShadingGridStepQ20Current = g_GraphicsShadingGridStepQ20;
      textureSetResult = g_GraphicsCreateTextureSet(g_GraphicsShadingGeneratedAsset);
      allocationCursor = (GraphicsGeneratedTextureAssetOrEntryView200 *)textureSetResult.textureSet;
      if (!textureSetResult.failed) {
        g_GraphicsShadingTextureSet = (GraphicsTextureSet *)&allocationCursor->asset;
        return THANDOR_BITCAST(uint64_t, StatusResult, ((THANDOR_BITCAST(TextureSetResult, uint64_t, textureSetResult) & 0xFFFFFFFFFFull) & 0xffffffff));
      }
      g_MemoryApi.free(g_GraphicsShadingGeneratedAsset);
    }
  }
  failureStatus.failed = true;
  failureStatus.valueOrError = (uint32_t)allocationCursor;
  return failureStatus;
}


/* Address: 0x004CD1B0.
   Counterpart of GraphicsShadingRuntime_InitializeGeneratedTexture: destroys the texture set, frees the generated
   gfx asset and the scratch grid, and clears the three pointers.
*/
void GraphicsShadingRuntime_Shutdown(void)

{
  g_GraphicsDestroyTextureSet(g_GraphicsShadingTextureSet);
  g_GraphicsShadingTextureSet = NULL;
  g_MemoryApi.free(g_GraphicsShadingGeneratedAsset);
  g_GraphicsShadingGeneratedAsset = NULL;
  g_MemoryApi.free(g_GraphicsShadingGridScratch);
  g_GraphicsShadingGridScratch = NULL;
  return;
}


/* Address: 0x004CD200.
   Starts a shadow pass (frontend world render in src/ui/frontend/runtime.c, before the per-model
   GraphicsShadingGeneratedTexture_ProcessRenderableHierarchy calls): puts the tile cursor on the first
   tile of subresource 0 (the pixel cursor at the tile centre), clears the tile/subresource counters and the
   "all tiles used" count, and zeroes the 8-bit pixels of every generated shadow texture.
*/
void GraphicsShadingGeneratedTexture_ResetPassScratchAndClearAlphaPlanes(void)

{
  AssetRelativeOffset tableOffset;
  uint32_t dwordsRemaining;
  uint8_t *alphaCursor;
  
  g_GeneratedTextureScratchRuntime.downsampleBorderOffset = g_TextureDownsampleShift << 2;
  /* asset + first source entry's dataOffset (assetRelativeAddressAnchor28 lies at asset + 0x28), moved half
     a tile down and right */
  g_GraphicsShadingGeneratedTexturePixelCursor =
       (g_GraphicsShadingGeneratedAsset->common).buildMetadata.assetRelativeAddressAnchor28 +
       ((g_GraphicsShadingTextureDimension + 1) * g_GraphicsShadingGridHalfSize >> 1) +
       *(int *)((g_GraphicsShadingGeneratedAsset->common).buildMetadata.assetRelativeAddressAnchor28
               + ((g_GraphicsShadingGeneratedAsset->tableDescriptor).subresourceTableOffset - 0x1c))
       - 0x28;
  g_GraphicsShadingGeneratedTextureTileX = 0;
  g_GraphicsShadingGeneratedTextureTileY = 0;
  g_GraphicsShadingGeneratedTextureSubresourceIndex = 0;
  g_GraphicsShadingGeneratedTextureTileXQ20 = 0;
  g_GraphicsShadingGeneratedTextureTileYQ20 = 0;
  g_GraphicsShadingGeneratedTextureCompletedTraversalCount = 0;
  tableOffset = (g_GraphicsShadingGeneratedAsset->tableDescriptor).subresourceTableOffset;
  /* pixels follow the 0x20-byte source entries; size = count * width * height of the first entry */
  alphaCursor = (g_GraphicsShadingGeneratedAsset->common).buildMetadata.assetRelativeAddressAnchor28 +
           g_GraphicsShadingSubresourceCount * 0x20 + tableOffset - 0x28;
  for (dwordsRemaining = g_GraphicsShadingSubresourceCount *
               *(int *)((g_GraphicsShadingGeneratedAsset->common).buildMetadata.
                        assetRelativeAddressAnchor28 + (tableOffset - 0x10)) *
               *(int *)((g_GraphicsShadingGeneratedAsset->common).buildMetadata.
                        assetRelativeAddressAnchor28 + (tableOffset - 0xc)) >> 2; dwordsRemaining != 0;
      dwordsRemaining--) {
    alphaCursor[0] = 0;
    alphaCursor[1] = 0;
    alphaCursor[2] = 0;
    alphaCursor[3] = 0;
    alphaCursor = alphaCursor + 4;
  }
  return;
}


/* Address: 0x004CD360.
   Ends a shadow pass (frontend world render in src/ui/frontend/runtime.c): uploads the alpha of every
   generated shadow texture the pass filled, i.e. all subresources before the current one plus the current
   one when it has at least one used tile (and not every tile ran out).
*/
void GraphicsShadingGeneratedTexture_RefreshTouchedAlphaSubresources(void)

{
  uint32_t subresourceIndex;
  GraphicsSubresourceIndex subresourcesRemaining;

  subresourceIndex = 0;
  for (subresourcesRemaining = g_GraphicsShadingGeneratedTextureSubresourceIndex; subresourcesRemaining != 0; subresourcesRemaining--) {
    g_GraphicsRefreshTextureAlpha(subresourceIndex,g_GraphicsShadingTextureSet);
    subresourceIndex++;
  }
  if ((g_GraphicsShadingGeneratedTextureCompletedTraversalCount == 0) &&
     ((g_GraphicsShadingGeneratedTextureTileX != 0 || (g_GraphicsShadingGeneratedTextureTileY != 0))
     )) {
    g_GraphicsRefreshTextureAlpha(subresourceIndex,g_GraphicsShadingTextureSet);
  }
  return;
}


/* Address: 0x004D1170.
   Single-block variant of GraphicsShadingGeneratedTexture_ReserveFourteenProjectedPointBlocks: takes the next
   0x80-byte block of the render context's projected point pool and records its address in the pool's block
   table. The original reports a full pool with CF set (CF clear on success); this C version returns nothing.
   No caller in the C sources.
*/
void GraphicsShadingGeneratedTexture_ReserveOneProjectedPointBlock
               (GeneratedTextureRenderContextView *renderContext)

{
  uint32_t *blockPool;
  uint32_t usedBlockCount;

  /* pool: [0] capacity, [1] used blocks, [2] block data base, from +0x20 a table of 0x10-byte entries whose
     second dword holds the block address */
  blockPool = renderContext->projectedPointBlockPool;
  usedBlockCount = blockPool[1];
  if (usedBlockCount + 1 < *blockPool) {
    blockPool[1] = usedBlockCount + 1;
    blockPool[usedBlockCount * 4 + 9] = usedBlockCount * 0x80 + blockPool[2];
    return;
  }
  return;
}


/* Address: 0x004CD880.
   Shadow silhouette pass for a mesh record whose flag bit 0 (+0x10) is clear, called per mesh record by
   GraphicsShadingGeneratedTexture_TraverseHierarchyFlag0Clear after the blur: projects every vertex into the
   current shadow tile (quantized to whole texels) and fills every triangle with 0xFF. A mesh record holds the
   vertex count (+8), triangle count (+0xC) and flags (+0x10), then 0x40-byte vertices (position at +0,
   projected XY stored at +0x20) followed by 0x40-byte triangles (vertex pointers at +0, +0xC, +0x18).
*/
void GraphicsShadingGeneratedTexture_RasterizeRecordBatchFlag0Clear(ModelMeshGroupAddress32 meshRecord)

{
  int vertexCount;
  int triangleCount;
  GraphicsFixedVec3 *recordCursor;

  vertexCount = *(int *)(meshRecord + 8);
  triangleCount = *(int *)(meshRecord + 0xc);
  if (((*(uint32_t *)(meshRecord + 0x10) & 1) == 0) &&
     (recordCursor = (GraphicsFixedVec3 *)(meshRecord + 0x20), vertexCount != 0)) {
    do {
      GraphicsShadingGeneratedTexture_TransformPointXYQuantized
                ((GraphicsFixedVec2 *)&recordCursor[2].z,recordCursor,
                 &g_GeneratedTextureScratchRuntime.modelToGeneratedTextureTransform);
      recordCursor = (GraphicsFixedVec3 *)&recordCursor[5].y; /* next 0x40-byte vertex */
      vertexCount--;
    } while (vertexCount != 0);
    for (; triangleCount != 0; triangleCount--) {
      GraphicsShadingGeneratedTexture_RasterizeTriangleMask
                ((GraphicsFixedVec2 *)(*(int *)((int)recordCursor + 0x18) + 0x20),
                 (GraphicsFixedVec2 *)(*(int *)((int)recordCursor + 0xc) + 0x20),
                 (GraphicsFixedVec2 *)(recordCursor->x + 0x20));
      recordCursor = (GraphicsFixedVec3 *)((int)recordCursor + 0x40);
    }
  }
  return;
}


/* Address: 0x004CD930.
   Second silhouette pass of GraphicsShadingGeneratedTexture_ProcessRenderableHierarchy (after the blur): for
   the node and, recursively, all its children builds the node-to-shadow-tile transform (world transform taken
   relative to the shadow origin, composed with the generated texture basis) and rasterizes the node's mesh
   records whose flag bit 0 is clear.
*/
void GraphicsShadingGeneratedTexture_TraverseHierarchyFlag0Clear(ModelRuntimeNode *modelNode)

{
  GraphicsFixedVec3 *nodeTranslation;
  GraphicsWorldCoordinateQ12 *translationComponent;
  ModelResourceHitTestAndRenderView210 *resourceView;
  GraphicsWorldCoordinateQ12 originX;
  GraphicsWorldCoordinateQ12 originY;
  GraphicsWorldCoordinateQ12 originZ;
  int offsetCountOrChildIndex;
  uint32_t childrenRemaining;
  uint8_t *meshRecord;

  originZ = g_GeneratedTextureScratchRuntime.currentModelOriginQ12.z;
  originY = g_GeneratedTextureScratchRuntime.currentModelOriginQ12.y;
  originX = g_GeneratedTextureScratchRuntime.currentModelOriginQ12.x;
  /* the node translation is made relative to the shadow origin only for the compose, then restored */
  nodeTranslation = &(modelNode->worldTransform).translation;
  nodeTranslation->x = nodeTranslation->x - g_GeneratedTextureScratchRuntime.currentModelOriginQ12.x;
  translationComponent = &(modelNode->worldTransform).translation.y;
  *translationComponent = *translationComponent - originY;
  translationComponent = &(modelNode->worldTransform).translation.z;
  *translationComponent = *translationComponent - originZ;
  GraphicsShadingGeneratedTexture_ComposeTransform
            (&g_GeneratedTextureScratchRuntime.modelToGeneratedTextureTransform,
             &modelNode->worldTransform,
             &g_GeneratedTextureScratchRuntime.generatedTextureBasisTransform);
  nodeTranslation = &(modelNode->worldTransform).translation;
  nodeTranslation->x = nodeTranslation->x + originX;
  translationComponent = &(modelNode->worldTransform).translation.y;
  *translationComponent = *translationComponent + originY;
  translationComponent = &(modelNode->worldTransform).translation.z;
  *translationComponent = *translationComponent + originZ;
  resourceView = (modelNode->modelPayload).modelResource;
  /* resource +0xEC: offset of the mesh group (record count at +4, first mesh record at +0x20; each record
     starts with its own size) */
  offsetCountOrChildIndex = *(int *)resourceView->reservedEC_1FF;
  if (offsetCountOrChildIndex != 0) {
    meshRecord = resourceView->reserved00_AF + offsetCountOrChildIndex + 0x20;
    for (offsetCountOrChildIndex = *(int *)(resourceView->reserved00_AF + offsetCountOrChildIndex + 4); offsetCountOrChildIndex != 0; offsetCountOrChildIndex--) {
      GraphicsShadingGeneratedTexture_RasterizeRecordBatchFlag0Clear
                ((ModelMeshGroupAddress32)meshRecord);
      meshRecord = meshRecord + *(int *)meshRecord;
    }
  }
  offsetCountOrChildIndex = 0;
  for (childrenRemaining = modelNode->childCount; childrenRemaining != 0; childrenRemaining--) {
    if (modelNode->childNodes[offsetCountOrChildIndex] != NULL) {
      GraphicsShadingGeneratedTexture_TraverseHierarchyFlag0Clear(modelNode->childNodes[offsetCountOrChildIndex]);
    }
    offsetCountOrChildIndex++;
  }
  return;
}


/* Address: 0x004CD9F0.
   Counterpart of GraphicsShadingGeneratedTexture_RasterizeRecordBatchFlag0Clear for mesh records whose flag
   bit 0 is set (the parts that get the soft, filtered shadow); called per mesh record by
   GraphicsShadingGeneratedTexture_TraverseHierarchyFlag0Set. Returns EBX: 0 when nothing was rasterized, 1
   when triangles were, otherwise the last transformed record address (the original leaves EBX there when
   the batch has vertices but no triangles).
*/
uint32_t
GraphicsShadingGeneratedTexture_RasterizeRecordBatchFlag0Set(ModelMeshGroupAddress32 meshRecord)

{
  int vertexCount;
  int triangleCount;
  GraphicsFixedVec3 *recordCursor;
  uint32_t result;

  result = 0;
  vertexCount = *(int *)(meshRecord + 8);
  triangleCount = *(int *)(meshRecord + 0xc);
  if (((*(uint32_t *)(meshRecord + 0x10) & 1) != 0) &&
     (recordCursor = (GraphicsFixedVec3 *)(meshRecord + 0x20), vertexCount != 0)) {
    do {
      result = (uint32_t)&recordCursor[2].z;
      GraphicsShadingGeneratedTexture_TransformPointXYQuantized
                ((GraphicsFixedVec2 *)&recordCursor[2].z,recordCursor,
                 &g_GeneratedTextureScratchRuntime.modelToGeneratedTextureTransform);
      recordCursor = (GraphicsFixedVec3 *)&recordCursor[5].y; /* next 0x40-byte vertex */
      vertexCount--;
    } while (vertexCount != 0);
    if (triangleCount != 0) {
      for (; triangleCount != 0; triangleCount--) {
        GraphicsShadingGeneratedTexture_RasterizeTriangleMask
                  ((GraphicsFixedVec2 *)(*(int *)((int)recordCursor + 0x18) + 0x20),
                   (GraphicsFixedVec2 *)(*(int *)((int)recordCursor + 0xc) + 0x20),
                   (GraphicsFixedVec2 *)(recordCursor->x + 0x20));
        recordCursor = (GraphicsFixedVec3 *)((int)recordCursor + 0x40);
      }
      result = 1;
    }
  }
  return result;
}


/* Address: 0x004CDAB0.
   First silhouette pass of GraphicsShadingGeneratedTexture_ProcessRenderableHierarchy: like
   GraphicsShadingGeneratedTexture_TraverseHierarchyFlag0Clear, but rasterizes the mesh records whose flag bit 0
   is set. Returns EBX, which the caller tests before filtering the generated texture: the last mesh record's
   result plus the mesh-group table offset (EAX at 0x004CDB36), plus the children's results. Nonzero
   whenever the hierarchy has a mesh group.
*/
uint32_t
GraphicsShadingGeneratedTexture_TraverseHierarchyFlag0Set(ModelRuntimeNode *modelNode)

{
  uint32_t result;
  int meshGroupTableOffset;
  GraphicsFixedVec3 *nodeTranslation;
  GraphicsWorldCoordinateQ12 *translationComponent;
  ModelResourceHitTestAndRenderView210 *resourceView;
  GraphicsWorldCoordinateQ12 originX;
  GraphicsWorldCoordinateQ12 originY;
  GraphicsWorldCoordinateQ12 originZ;
  int offsetCountOrChildIndex;
  uint32_t childrenRemaining;
  uint8_t *meshRecord;

  originZ = g_GeneratedTextureScratchRuntime.currentModelOriginQ12.z;
  originY = g_GeneratedTextureScratchRuntime.currentModelOriginQ12.y;
  originX = g_GeneratedTextureScratchRuntime.currentModelOriginQ12.x;
  /* the node translation is made relative to the shadow origin only for the compose, then restored */
  nodeTranslation = &(modelNode->worldTransform).translation;
  nodeTranslation->x = nodeTranslation->x - g_GeneratedTextureScratchRuntime.currentModelOriginQ12.x;
  translationComponent = &(modelNode->worldTransform).translation.y;
  *translationComponent = *translationComponent - originY;
  translationComponent = &(modelNode->worldTransform).translation.z;
  *translationComponent = *translationComponent - originZ;
  GraphicsShadingGeneratedTexture_ComposeTransform
            (&g_GeneratedTextureScratchRuntime.modelToGeneratedTextureTransform,
             &modelNode->worldTransform,
             &g_GeneratedTextureScratchRuntime.generatedTextureBasisTransform);
  nodeTranslation = &(modelNode->worldTransform).translation;
  nodeTranslation->x = nodeTranslation->x + originX;
  translationComponent = &(modelNode->worldTransform).translation.y;
  *translationComponent = *translationComponent + originY;
  translationComponent = &(modelNode->worldTransform).translation.z;
  *translationComponent = *translationComponent + originZ;
  result = 0;
  resourceView = (modelNode->modelPayload).modelResource;
  offsetCountOrChildIndex = *(int *)resourceView->reservedEC_1FF;
  meshGroupTableOffset = offsetCountOrChildIndex;
  if (offsetCountOrChildIndex != 0) {
    meshRecord = resourceView->reserved00_AF + offsetCountOrChildIndex + 0x20;
    for (offsetCountOrChildIndex = *(int *)(resourceView->reserved00_AF + offsetCountOrChildIndex + 4); offsetCountOrChildIndex != 0; offsetCountOrChildIndex--) {
      result = GraphicsShadingGeneratedTexture_RasterizeRecordBatchFlag0Set
                         ((ModelMeshGroupAddress32)meshRecord) + meshGroupTableOffset;
      meshRecord = meshRecord + *(int *)meshRecord;
    }
  }
  offsetCountOrChildIndex = 0;
  for (childrenRemaining = modelNode->childCount; childrenRemaining != 0; childrenRemaining--) {
    if (modelNode->childNodes[offsetCountOrChildIndex] != NULL) {
      result = result +
               GraphicsShadingGeneratedTexture_TraverseHierarchyFlag0Set(modelNode->childNodes[offsetCountOrChildIndex]);
    }
    offsetCountOrChildIndex++;
  }
  return result;
}


/* Address: 0x004CDB80.
   Projects every vertex of one mesh record with the current node transform (light-space rotation, see
   GraphicsShadingGeneratedTexture_TraverseHierarchyAndAccumulateProjectedBounds, which calls it per mesh
   record) and widens the projected min/max X/Y of g_GeneratedTextureScratchRuntime, from which
   GraphicsShadingGeneratedTexture_ProcessRenderableHierarchy sizes the shadow tile.
*/
void GraphicsShadingGeneratedTexture_AccumulateProjectedBoundsFromRecords(ModelMeshGroupAddress32 meshRecord)

{
  int vertexX;
  int vertexY;
  int verticesRemaining;
  GraphicsFixedVec3 *vertexCursor;

  vertexCursor = (GraphicsFixedVec3 *)(meshRecord + 0x20);
  for (verticesRemaining = *(int *)(meshRecord + 8); verticesRemaining != 0; verticesRemaining--) {
    GraphicsShadingGeneratedTexture_TransformPointXY
              ((GraphicsFixedVec2 *)&vertexCursor[2].z,vertexCursor,
               &g_GeneratedTextureScratchRuntime.modelToGeneratedTextureTransform);
    vertexX = vertexCursor[2].z;
    vertexY = vertexCursor[3].x;
    vertexCursor = (GraphicsFixedVec3 *)&vertexCursor[5].y; /* next 0x40-byte vertex */
    if (vertexX < g_GeneratedTextureScratchRuntime.projectedMinX) {
      g_GeneratedTextureScratchRuntime.projectedMinX = vertexX;
    }
    if (vertexY < g_GeneratedTextureScratchRuntime.projectedMinY) {
      g_GeneratedTextureScratchRuntime.projectedMinY = vertexY;
    }
    if (g_GeneratedTextureScratchRuntime.projectedMaxX < vertexX) {
      g_GeneratedTextureScratchRuntime.projectedMaxX = vertexX;
    }
    if (g_GeneratedTextureScratchRuntime.projectedMaxY < vertexY) {
      g_GeneratedTextureScratchRuntime.projectedMaxY = vertexY;
    }
  }
  return;
}


/* Address: 0x004CDC20.
   First step of GraphicsShadingGeneratedTexture_ProcessRenderableHierarchy: for the node and, recursively,
   all its children composes the node transform (relative to the shadow origin) with
   g_AuxiliaryRotationMatrixFixed (the light-space rotation) and accumulates the projected bounds of all mesh
   records, i.e. the extent of the model's shadow before it is scaled into a texture tile.
*/
void GraphicsShadingGeneratedTexture_TraverseHierarchyAndAccumulateProjectedBounds(ModelRuntimeNode *modelNode)

{
  GraphicsFixedVec3 *nodeTranslation;
  GraphicsWorldCoordinateQ12 *translationComponent;
  ModelResourceHitTestAndRenderView210 *resourceView;
  GraphicsWorldCoordinateQ12 originX;
  GraphicsWorldCoordinateQ12 originY;
  GraphicsWorldCoordinateQ12 originZ;
  int offsetCountOrChildIndex;
  uint32_t childrenRemaining;
  uint8_t *meshRecord;

  originZ = g_GeneratedTextureScratchRuntime.currentModelOriginQ12.z;
  originY = g_GeneratedTextureScratchRuntime.currentModelOriginQ12.y;
  originX = g_GeneratedTextureScratchRuntime.currentModelOriginQ12.x;
  /* the node translation is made relative to the shadow origin only for the compose, then restored */
  nodeTranslation = &(modelNode->worldTransform).translation;
  nodeTranslation->x = nodeTranslation->x - g_GeneratedTextureScratchRuntime.currentModelOriginQ12.x;
  translationComponent = &(modelNode->worldTransform).translation.y;
  *translationComponent = *translationComponent - originY;
  translationComponent = &(modelNode->worldTransform).translation.z;
  *translationComponent = *translationComponent - originZ;
  FixedTransform_Compose
            (&g_GeneratedTextureScratchRuntime.modelToGeneratedTextureTransform,
             &modelNode->worldTransform,&g_AuxiliaryRotationMatrixFixed);
  nodeTranslation = &(modelNode->worldTransform).translation;
  nodeTranslation->x = nodeTranslation->x + originX;
  translationComponent = &(modelNode->worldTransform).translation.y;
  *translationComponent = *translationComponent + originY;
  translationComponent = &(modelNode->worldTransform).translation.z;
  *translationComponent = *translationComponent + originZ;
  resourceView = (modelNode->modelPayload).modelResource;
  offsetCountOrChildIndex = *(int *)resourceView->reservedEC_1FF;
  if (offsetCountOrChildIndex != 0) {
    meshRecord = resourceView->reserved00_AF + offsetCountOrChildIndex + 0x20;
    for (offsetCountOrChildIndex = *(int *)(resourceView->reserved00_AF + offsetCountOrChildIndex + 4); offsetCountOrChildIndex != 0; offsetCountOrChildIndex--) {
      GraphicsShadingGeneratedTexture_AccumulateProjectedBoundsFromRecords
                ((ModelMeshGroupAddress32)meshRecord);
      meshRecord = meshRecord + *(int *)meshRecord;
    }
  }
  offsetCountOrChildIndex = 0;
  for (childrenRemaining = modelNode->childCount; childrenRemaining != 0; childrenRemaining--) {
    if (modelNode->childNodes[offsetCountOrChildIndex] != NULL) {
      GraphicsShadingGeneratedTexture_TraverseHierarchyAndAccumulateProjectedBounds
                (modelNode->childNodes[offsetCountOrChildIndex]);
    }
    offsetCountOrChildIndex++;
  }
  return;
}


/* Address: 0x00485020.
   Transforms point by the first two rows of transform (Q28 basis, 64-bit dot products shifted right by 28)
   plus translation and stores only X and Y; the shadow bounds pass
   (GraphicsShadingGeneratedTexture_AccumulateProjectedBoundsFromRecords) needs no depth.
*/
void GraphicsShadingGeneratedTexture_TransformPointXY
          (GraphicsFixedVec2 *outputXY,GraphicsFixedVec3 *point,GraphicsFixedMatrix3x4 *transform)

{
  int row1Column0;
  int64_t dotProduct;
  
  dotProduct = (int64_t)transform->basisRow0[1] * (int64_t)point->y +
          (int64_t)transform->basisRow0[0] * (int64_t)point->x +
          (int64_t)transform->basisRow0[2] * (int64_t)point->z;
  row1Column0 = transform->basisRow1[0];
  outputXY->component0 =
       ((int)((uint64_t)dotProduct >> 0x20) << 4 | (uint32_t)dotProduct >> 0x1c) + (transform->translation).x;
  dotProduct = (int64_t)transform->basisRow1[1] * (int64_t)point->y +
          (int64_t)row1Column0 * (int64_t)point->x +
          (int64_t)transform->basisRow1[2] * (int64_t)point->z;
  outputXY->component1 =
       ((int)((uint64_t)dotProduct >> 0x20) << 4 | (uint32_t)dotProduct >> 0x1c) + (transform->translation).y;
  return;
}


/* Address: 0x004CD2B0.
   Moves the shadow tile cursor to the next gridHalfSize x gridHalfSize tile after a model's shadow was
   drawn (end of GraphicsShadingGeneratedTexture_ProcessRenderableHierarchy): left to right, then down a
   tile row, then on to the next generated texture. When the last texture is full the completed count
   becomes nonzero and further models get no shadow until the next pass.
*/
void GraphicsShadingGeneratedTexture_AdvanceTileCursor(void)

{
  g_GraphicsShadingGeneratedTextureTileX =
       g_GraphicsShadingGeneratedTextureTileX + g_GraphicsShadingGridHalfSize;
  g_GraphicsShadingGeneratedTexturePixelCursor =
       g_GraphicsShadingGeneratedTexturePixelCursor + g_GraphicsShadingGridHalfSize;
  g_GraphicsShadingGeneratedTextureTileXQ20 =
       g_GraphicsShadingGeneratedTextureTileXQ20 + g_GraphicsShadingGridStepQ20;
  if (g_GraphicsShadingTextureDimension <= g_GraphicsShadingGeneratedTextureTileX) {
    g_GraphicsShadingGeneratedTextureTileX = 0;
    g_GraphicsShadingGeneratedTextureTileXQ20 = 0;
    g_GraphicsShadingGeneratedTextureTileY =
         g_GraphicsShadingGeneratedTextureTileY + g_GraphicsShadingGridHalfSize;
    g_GraphicsShadingGeneratedTextureTileYQ20 =
         g_GraphicsShadingGeneratedTextureTileYQ20 + g_GraphicsShadingGridStepQ20;
    g_GraphicsShadingGeneratedTexturePixelCursor =
         g_GraphicsShadingGeneratedTexturePixelCursor +
         (g_GraphicsShadingGridHalfSize * g_GraphicsShadingTextureDimension -
         g_GraphicsShadingTextureDimension);
    if (g_GraphicsShadingTextureDimension <= g_GraphicsShadingGeneratedTextureTileY) {
      g_GraphicsShadingGeneratedTextureSubresourceIndex++;
      g_GraphicsShadingGeneratedTextureTileY = 0;
      g_GraphicsShadingGeneratedTextureTileYQ20 = 0;
      if (g_GraphicsShadingSubresourceCount <= g_GraphicsShadingGeneratedTextureSubresourceIndex) {
        g_GraphicsShadingGeneratedTextureCompletedTraversalCount++;
      }
    }
  }
  return;
}


/* Address: 0x004CD3D0.
   Softens the shadow in the current tile (GraphicsShadingGeneratedTexture_ProcessRenderableHierarchy calls it
   after the flag-0 silhouette pass drew something): copies the tile's texels, reduced to 0..7 (>> 5), into
   the zero-bordered scratch grid, then writes back to every texel the byte-saturated weighted sum of its
   neighbourhood (centre x4, taps gridHalfSize / 16 texels apart), 32 texels per step with MMX.
*/
void GraphicsShadingGeneratedTexture_FilterGridScratchMmx(void)

{
  int backtrackOffset;
  uint64_t threeBitMask;
  int scratchBlockStride;
  uint32_t rowBytesRemaining;
  uint32_t rowsRemaining;
  uint64_t *scratchWriteCursor;
  uint64_t *neighborCursor;
  int64_t *scratchCursor;
  uint64_t *textureWriteCursor;
  uint64_t *textureReadCursor;
  uint32_t neighborStep;
  bool rowContinues;
  uint64_t weightedSum0;
  uint64_t quad1OrResult0;
  uint64_t weightedSum1;
  uint64_t quad2OrResult1;
  uint64_t weightedSum2;
  uint64_t quad3OrResult2;
  uint64_t weightedSum3;
  uint64_t result3;
  uint64_t crossSum0;
  uint64_t crossSum1;
  uint64_t crossSum2;
  uint64_t crossSum3;
  
  threeBitMask = g_GraphicsShadingMmxPacked3BitPerByteMask;
  neighborStep = g_GraphicsShadingGridHalfSize >> 4;
  /* top-left texel of the tile (the pixel cursor points at its centre) */
  textureWriteCursor = (uint64_t *)
           (g_GraphicsShadingGeneratedTexturePixelCursor +
           (-(g_GraphicsShadingGridHalfSize >> 1) -
           g_GraphicsShadingTextureDimension * (g_GraphicsShadingGridHalfSize >> 1)));
  rowBytesRemaining = g_GraphicsShadingGridHalfSize;
  rowsRemaining = g_GraphicsShadingGridHalfSize;
  scratchWriteCursor = (uint64_t *)g_GraphicsShadingGridScratchInterior;
  textureReadCursor = textureWriteCursor;
  /* pass 1: tile -> scratch, each texel reduced to 3 bits */
  do {
    do {
      quad1OrResult0 = textureReadCursor[1];
      quad2OrResult1 = textureReadCursor[2];
      quad3OrResult2 = textureReadCursor[3];
      *scratchWriteCursor = *textureReadCursor >> 5 & threeBitMask;
      scratchWriteCursor[1] = quad1OrResult0 >> 5 & threeBitMask;
      scratchWriteCursor[2] = quad2OrResult1 >> 5 & threeBitMask;
      scratchWriteCursor[3] = quad3OrResult2 >> 5 & threeBitMask;
      textureReadCursor = textureReadCursor + 4;
      scratchWriteCursor = scratchWriteCursor + 4;
      rowContinues = 0x1f < rowBytesRemaining;
      rowBytesRemaining = rowBytesRemaining - 0x20;
    } while (rowContinues && rowBytesRemaining != 0);
    scratchWriteCursor = (uint64_t *)((int)scratchWriteCursor + g_GraphicsShadingGridHalfSize);
    textureReadCursor = (uint64_t *)
              ((int)textureReadCursor + (g_GraphicsShadingTextureDimension - g_GraphicsShadingGridHalfSize));
    rowsRemaining--;
    rowBytesRemaining = g_GraphicsShadingGridHalfSize;
  } while (rowsRemaining != 0);
  /* pass 2: weighted neighbourhood sums from the scratch grid (rows 2 * gridHalfSize bytes apart) back into
     the tile; scratchBlockStride is one vertical tap */
  scratchBlockStride = g_GraphicsShadingGridHalfSize * 2 * neighborStep;
  rowsRemaining = g_GraphicsShadingGridHalfSize;
  scratchCursor = (int64_t *)g_GraphicsShadingGridScratchInterior;
  do {
    do {
      weightedSum0 = paddusb(*scratchCursor << 2,*(uint64_t *)((int)scratchCursor + neighborStep * 2));
      weightedSum1 = paddusb(scratchCursor[1] << 2,*(uint64_t *)((int)scratchCursor + neighborStep * 2 + 8));
      weightedSum2 = paddusb(scratchCursor[2] << 2,*(uint64_t *)((int)scratchCursor + neighborStep * 2 + 0x10));
      weightedSum3 = paddusb(scratchCursor[3] << 2,*(uint64_t *)((int)scratchCursor + neighborStep * 2 + 0x18));
      neighborCursor = (uint64_t *)((int)scratchCursor - neighborStep);
      crossSum0 = paddusb(*(uint64_t *)((int)scratchCursor + neighborStep),*neighborCursor);
      crossSum1 = paddusb(*(uint64_t *)((int)scratchCursor + neighborStep + 8),neighborCursor[1]);
      crossSum2 = paddusb(*(uint64_t *)((int)scratchCursor + neighborStep + 0x10),neighborCursor[2]);
      crossSum3 = paddusb(*(uint64_t *)((int)scratchCursor + neighborStep + 0x18),neighborCursor[3]);
      neighborCursor = (uint64_t *)((int)neighborCursor - neighborStep);
      weightedSum0 = paddusb(weightedSum0,*neighborCursor);
      weightedSum1 = paddusb(weightedSum1,neighborCursor[1]);
      weightedSum2 = paddusb(weightedSum2,neighborCursor[2]);
      weightedSum3 = paddusb(weightedSum3,neighborCursor[3]);
      neighborCursor = (uint64_t *)((int)neighborCursor + (neighborStep * 2 - scratchBlockStride));
      crossSum0 = paddusb(crossSum0,*neighborCursor);
      crossSum1 = paddusb(crossSum1,neighborCursor[1]);
      crossSum2 = paddusb(crossSum2,neighborCursor[2]);
      crossSum3 = paddusb(crossSum3,neighborCursor[3]);
      weightedSum0 = paddusb(weightedSum0,*(uint64_t *)((int)neighborCursor + neighborStep));
      weightedSum1 = paddusb(weightedSum1,*(uint64_t *)((int)neighborCursor + neighborStep + 8));
      weightedSum2 = paddusb(weightedSum2,*(uint64_t *)((int)neighborCursor + neighborStep + 0x10));
      weightedSum3 = paddusb(weightedSum3,*(uint64_t *)((int)neighborCursor + neighborStep + 0x18));
      weightedSum0 = paddusb(weightedSum0,*(uint64_t *)((int)neighborCursor + neighborStep * 2));
      weightedSum1 = paddusb(weightedSum1,*(uint64_t *)((int)neighborCursor + neighborStep * 2 + 8));
      weightedSum2 = paddusb(weightedSum2,*(uint64_t *)((int)neighborCursor + neighborStep * 2 + 0x10));
      weightedSum3 = paddusb(weightedSum3,*(uint64_t *)((int)neighborCursor + neighborStep * 2 + 0x18));
      neighborCursor = (uint64_t *)((int)neighborCursor - neighborStep);
      weightedSum0 = paddusb(weightedSum0,*neighborCursor);
      weightedSum1 = paddusb(weightedSum1,neighborCursor[1]);
      weightedSum2 = paddusb(weightedSum2,neighborCursor[2]);
      weightedSum3 = paddusb(weightedSum3,neighborCursor[3]);
      neighborCursor = (uint64_t *)((int)neighborCursor - neighborStep);
      weightedSum0 = paddusb(weightedSum0,*neighborCursor);
      weightedSum1 = paddusb(weightedSum1,neighborCursor[1]);
      weightedSum2 = paddusb(weightedSum2,neighborCursor[2]);
      weightedSum3 = paddusb(weightedSum3,neighborCursor[3]);
      neighborCursor = (uint64_t *)((int)neighborCursor + scratchBlockStride * 2 + neighborStep * 2);
      crossSum0 = paddusb(crossSum0,*neighborCursor);
      crossSum1 = paddusb(crossSum1,neighborCursor[1]);
      crossSum2 = paddusb(crossSum2,neighborCursor[2]);
      crossSum3 = paddusb(crossSum3,neighborCursor[3]);
      weightedSum0 = paddusb(weightedSum0,*(uint64_t *)((int)neighborCursor + neighborStep));
      weightedSum1 = paddusb(weightedSum1,*(uint64_t *)((int)neighborCursor + neighborStep + 8));
      weightedSum2 = paddusb(weightedSum2,*(uint64_t *)((int)neighborCursor + neighborStep + 0x10));
      weightedSum3 = paddusb(weightedSum3,*(uint64_t *)((int)neighborCursor + neighborStep + 0x18));
      weightedSum0 = paddusb(weightedSum0,*(uint64_t *)((int)neighborCursor + neighborStep * 2));
      weightedSum1 = paddusb(weightedSum1,*(uint64_t *)((int)neighborCursor + neighborStep * 2 + 8));
      weightedSum2 = paddusb(weightedSum2,*(uint64_t *)((int)neighborCursor + neighborStep * 2 + 0x10));
      weightedSum3 = paddusb(weightedSum3,*(uint64_t *)((int)neighborCursor + neighborStep * 2 + 0x18));
      backtrackOffset = -scratchBlockStride - neighborStep;
      weightedSum0 = paddusb(weightedSum0,*(uint64_t *)((int)neighborCursor + scratchBlockStride + backtrackOffset));
      weightedSum1 = paddusb(weightedSum1,*(uint64_t *)((int)neighborCursor + scratchBlockStride + 8 + backtrackOffset));
      weightedSum2 = paddusb(weightedSum2,*(uint64_t *)((int)neighborCursor + scratchBlockStride + 0x10 + backtrackOffset));
      weightedSum3 = paddusb(weightedSum3,*(uint64_t *)((int)neighborCursor + scratchBlockStride + 0x18 + backtrackOffset));
      backtrackOffset = (backtrackOffset - neighborStep) - scratchBlockStride;
      weightedSum0 = paddusb(weightedSum0,*(uint64_t *)((int)neighborCursor + scratchBlockStride * 2 + backtrackOffset));
      weightedSum1 = paddusb(weightedSum1,*(uint64_t *)((int)neighborCursor + scratchBlockStride * 2 + backtrackOffset + 8));
      weightedSum2 = paddusb(weightedSum2,*(uint64_t *)((int)neighborCursor + scratchBlockStride * 2 + backtrackOffset + 0x10));
      weightedSum3 = paddusb(weightedSum3,*(uint64_t *)((int)neighborCursor + scratchBlockStride * 2 + backtrackOffset + 0x18));
      weightedSum0 = paddusb(weightedSum0,crossSum0);
      weightedSum1 = paddusb(weightedSum1,crossSum1);
      weightedSum2 = paddusb(weightedSum2,crossSum2);
      weightedSum3 = paddusb(weightedSum3,crossSum3);
      weightedSum0 = paddusb(weightedSum0,crossSum0);
      weightedSum1 = paddusb(weightedSum1,crossSum1);
      weightedSum2 = paddusb(weightedSum2,crossSum2);
      weightedSum3 = paddusb(weightedSum3,crossSum3);
      neighborCursor = (uint64_t *)((int)neighborCursor + neighborStep * 2 + (backtrackOffset - scratchBlockStride));
      weightedSum0 = paddusb(weightedSum0,crossSum0);
      weightedSum1 = paddusb(weightedSum1,crossSum1);
      weightedSum2 = paddusb(weightedSum2,crossSum2);
      weightedSum3 = paddusb(weightedSum3,crossSum3);
      weightedSum0 = paddusb(weightedSum0,*neighborCursor);
      weightedSum1 = paddusb(weightedSum1,neighborCursor[1]);
      weightedSum2 = paddusb(weightedSum2,neighborCursor[2]);
      weightedSum3 = paddusb(weightedSum3,neighborCursor[3]);
      weightedSum0 = paddusb(weightedSum0,*(uint64_t *)((int)neighborCursor + neighborStep));
      weightedSum1 = paddusb(weightedSum1,*(uint64_t *)((int)neighborCursor + neighborStep + 8));
      weightedSum2 = paddusb(weightedSum2,*(uint64_t *)((int)neighborCursor + neighborStep + 0x10));
      weightedSum3 = paddusb(weightedSum3,*(uint64_t *)((int)neighborCursor + neighborStep + 0x18));
      weightedSum0 = paddusb(weightedSum0,*(uint64_t *)((int)neighborCursor + scratchBlockStride * 4));
      weightedSum1 = paddusb(weightedSum1,*(uint64_t *)((int)neighborCursor + scratchBlockStride * 4 + 8));
      weightedSum2 = paddusb(weightedSum2,*(uint64_t *)((int)neighborCursor + scratchBlockStride * 4 + 0x10));
      weightedSum3 = paddusb(weightedSum3,*(uint64_t *)((int)neighborCursor + scratchBlockStride * 4 + 0x18));
      neighborCursor = (uint64_t *)((int)neighborCursor - neighborStep);
      weightedSum0 = paddusb(weightedSum0,*neighborCursor);
      weightedSum1 = paddusb(weightedSum1,neighborCursor[1]);
      weightedSum2 = paddusb(weightedSum2,neighborCursor[2]);
      weightedSum3 = paddusb(weightedSum3,neighborCursor[3]);
      weightedSum0 = paddusb(weightedSum0,*(uint64_t *)((int)neighborCursor + scratchBlockStride * 4));
      weightedSum1 = paddusb(weightedSum1,*(uint64_t *)((int)neighborCursor + scratchBlockStride * 4 + 8));
      weightedSum2 = paddusb(weightedSum2,*(uint64_t *)((int)neighborCursor + scratchBlockStride * 4 + 0x10));
      weightedSum3 = paddusb(weightedSum3,*(uint64_t *)((int)neighborCursor + scratchBlockStride * 4 + 0x18));
      quad1OrResult0 = paddusb(weightedSum0,*(uint64_t *)((int)neighborCursor + scratchBlockStride * 4 + neighborStep * 2));
      quad2OrResult1 = paddusb(weightedSum1,*(uint64_t *)((int)neighborCursor + scratchBlockStride * 4 + neighborStep * 2 + 8));
      quad3OrResult2 = paddusb(weightedSum2,*(uint64_t *)((int)neighborCursor + scratchBlockStride * 4 + neighborStep * 2 + 0x10));
      result3 = paddusb(weightedSum3,*(uint64_t *)((int)neighborCursor + scratchBlockStride * 4 + neighborStep * 2 + 0x18));
      *textureWriteCursor = quad1OrResult0;
      textureWriteCursor[1] = quad2OrResult1;
      textureWriteCursor[2] = quad3OrResult2;
      textureWriteCursor[3] = result3;
      scratchCursor = (int64_t *)((int)neighborCursor + scratchBlockStride * 2 + neighborStep + 0x20);
      textureWriteCursor = textureWriteCursor + 4;
      rowContinues = 0x1f < rowBytesRemaining;
      rowBytesRemaining = rowBytesRemaining - 0x20;
    } while (rowContinues && rowBytesRemaining != 0);
    scratchCursor = (int64_t *)((int)scratchCursor + g_GraphicsShadingGridHalfSize);
    textureWriteCursor = (uint64_t *)
             ((int)textureWriteCursor + (g_GraphicsShadingTextureDimension - g_GraphicsShadingGridHalfSize));
    rowsRemaining--;
    rowBytesRemaining = g_GraphicsShadingGridHalfSize;
  } while (rowsRemaining != 0);
  return;
}


/* Address: 0x004CDCE0.
   Returns true (CF set in the original) when neither the node's model resource nor any descendant has a mesh
   group (resource +0xEC), so GraphicsShadingGeneratedTexture_ProcessRenderableHierarchy skips hierarchies
   that cannot cast a shadow. Children are probed from the last to the first; the first hit ends the search.
*/
bool GraphicsShadingGeneratedTexture_ProbeHierarchyForGeometry(ModelRuntimeNode *modelNode)

{
  uint32_t decrementedCount;
  uint32_t childrenRemaining;
  int childIndex;
  bool childLacksGeometry;
  
  if (*(int *)((modelNode->modelPayload).modelResource)->reservedEC_1FF == 0) {
    childrenRemaining = modelNode->childCount;
    do {
      decrementedCount = childrenRemaining - 1;
      if ((int)decrementedCount < 0) {
        return true;
      }
      childIndex = childrenRemaining - 1;
      childrenRemaining = decrementedCount;
    } while ((modelNode->childNodes[childIndex] == NULL) ||
            (childLacksGeometry = GraphicsShadingGeneratedTexture_ProbeHierarchyForGeometry
                               (modelNode->childNodes[childIndex]), childLacksGeometry));
  }
  return false;
}


/* Address: 0x004D1060.
   Reserves the 14 consecutive 0x80-byte primitive blocks of one shadow patch from the render context's
   projected point pool (GraphicsShadingGeneratedTexture_ProcessRenderableHierarchy): records each block's
   address in the pool's block table and presets the first block's 0x20-byte header at +0x60 (0, flags
   0x11000); the three 0x20-byte vertices of each triangle come first. Pool full: poolFull (CF) set.
*/
ProjectedBlockReserveResult GraphicsShadingGeneratedTexture_ReserveFourteenProjectedPointBlocks
          (GeneratedTextureRenderContextView *renderContext)

{
  uint32_t *blockPool;
  uint32_t usedBlockCount;
  ProjectedBlockReserveResult reservedResult;
  ProjectedBlockReserveResult newCountOrFailure;
  
  blockPool = renderContext->projectedPointBlockPool;
  usedBlockCount = blockPool[1];
  newCountOrFailure.firstBlock = (void *)(usedBlockCount + 14);
  if (newCountOrFailure.firstBlock < (void *)*blockPool) {
    blockPool[1] = (uint32_t)newCountOrFailure.firstBlock;
    reservedResult.firstBlock = (void *)(usedBlockCount * 0x80 + blockPool[2]);
    blockPool[usedBlockCount * 4 + 9] = (uint32_t)reservedResult.firstBlock;
    blockPool[usedBlockCount * 4 + 0xd] = (uint32_t)((uint8_t *)reservedResult.firstBlock + 0x80);
    blockPool[usedBlockCount * 4 + 0x11] = (uint32_t)((uint8_t *)reservedResult.firstBlock + 0x100);
    blockPool[usedBlockCount * 4 + 0x15] = (uint32_t)((uint8_t *)reservedResult.firstBlock + 0x180);
    blockPool[usedBlockCount * 4 + 0x19] = (uint32_t)((uint8_t *)reservedResult.firstBlock + 0x200);
    blockPool[usedBlockCount * 4 + 0x1d] = (uint32_t)((uint8_t *)reservedResult.firstBlock + 0x280);
    blockPool[usedBlockCount * 4 + 0x21] = (uint32_t)((uint8_t *)reservedResult.firstBlock + 0x300);
    blockPool[usedBlockCount * 4 + 0x25] = (uint32_t)((uint8_t *)reservedResult.firstBlock + 0x380);
    blockPool[usedBlockCount * 4 + 0x29] = (uint32_t)((uint8_t *)reservedResult.firstBlock + 0x400);
    blockPool[usedBlockCount * 4 + 0x2d] = (uint32_t)((uint8_t *)reservedResult.firstBlock + 0x480);
    blockPool[usedBlockCount * 4 + 0x31] = (uint32_t)((uint8_t *)reservedResult.firstBlock + 0x500);
    blockPool[usedBlockCount * 4 + 0x35] = (uint32_t)((uint8_t *)reservedResult.firstBlock + 0x580);
    blockPool[usedBlockCount * 4 + 0x39] = (uint32_t)((uint8_t *)reservedResult.firstBlock + 0x600);
    blockPool[usedBlockCount * 4 + 0x3d] = (uint32_t)((uint8_t *)reservedResult.firstBlock + 0x680);
    ((GraphicsProjectedPointPair *)reservedResult.firstBlock)[0xc].projectedX = 0;
    ((GraphicsProjectedPointPair *)reservedResult.firstBlock)[0xd].projectedX = 0x11000;
    reservedResult.poolFull = false;
    return reservedResult;
  }
  newCountOrFailure.poolFull = true;
  return newCountOrFailure;
}


/* Address: 0x004D1150.
   Gives back the 14 blocks of GraphicsShadingGeneratedTexture_ReserveFourteenProjectedPointBlocks when
   GraphicsShadingGeneratedTexture_ProcessRenderableHierarchy finds a shadow point with a view depth below
   g_ProjectionScaleFixed (not projectable).
*/
void GraphicsShadingGeneratedTexture_RollbackFourteenProjectedPointBlocks
               (GeneratedTextureRenderContextView *renderContext)

{
  renderContext->projectedPointBlockPool[1] = renderContext->projectedPointBlockPool[1] - 14;
  return;
}


/* Address: 0x00484FA0.
   Like GraphicsShadingGeneratedTexture_TransformPointXY, but for the composed node-to-tile transform: the
   dot products are shifted right by 12 only and the results are rounded down to whole texels (multiples of
   0x1000), ready for GraphicsShadingGeneratedTexture_RasterizeTriangleMask. Used by the silhouette passes.
*/
void GraphicsShadingGeneratedTexture_TransformPointXYQuantized
          (GraphicsFixedVec2 *outputXY,GraphicsFixedVec3 *point,GraphicsFixedMatrix3x4 *transform)

{
  int row1Column0;
  int64_t dotProduct;
  
  dotProduct = (int64_t)transform->basisRow0[1] * (int64_t)point->y +
          (int64_t)transform->basisRow0[0] * (int64_t)point->x +
          (int64_t)transform->basisRow0[2] * (int64_t)point->z;
  row1Column0 = transform->basisRow1[0];
  outputXY->component0 =
       ((int)((uint64_t)dotProduct >> 0x20) << 0x14 | (uint32_t)dotProduct >> 0xc) + (transform->translation).x &
       0xfffff000;
  dotProduct = (int64_t)transform->basisRow1[1] * (int64_t)point->y +
          (int64_t)row1Column0 * (int64_t)point->x +
          (int64_t)transform->basisRow1[2] * (int64_t)point->z;
  outputXY->component1 =
       ((int)((uint64_t)dotProduct >> 0x20) << 0x14 | (uint32_t)dotProduct >> 0xc) + (transform->translation).y &
       0xfffff000;
  return;
}


/* Address: 0x00485320.
   outTransform = lhsTransform * rhsTransform for the shadow silhouette passes (lhs = generated texture basis,
   rhs = node world transform). Unlike FixedTransform_Compose, the translation products are shifted right by
   12 instead of 28, keeping the extra precision that GraphicsShadingGeneratedTexture_TransformPointXYQuantized
   (also >> 12) expects. The original reloads lhs row elements between the dot products, as kept here.
*/
void GraphicsShadingGeneratedTexture_ComposeTransform
          (GraphicsFixedMatrix3x4 *outTransform,GraphicsFixedMatrix3x4 *rhsTransform,
          GraphicsFixedMatrix3x4 *lhsTransform)

{
  int lhsElement;
  int64_t dotProduct;
  
  dotProduct = (int64_t)lhsTransform->basisRow0[1] * (int64_t)rhsTransform->basisRow1[0] +
          (int64_t)lhsTransform->basisRow0[0] * (int64_t)rhsTransform->basisRow0[0] +
          (int64_t)lhsTransform->basisRow0[2] * (int64_t)rhsTransform->basisRow2[0];
  lhsElement = lhsTransform->basisRow0[0];
  outTransform->basisRow0[0] = (int)((uint64_t)dotProduct >> 0x20) << 4 | (uint32_t)dotProduct >> 0x1c;
  dotProduct = (int64_t)lhsTransform->basisRow0[1] * (int64_t)rhsTransform->basisRow1[1] +
          (int64_t)lhsElement * (int64_t)rhsTransform->basisRow0[1] +
          (int64_t)lhsTransform->basisRow0[2] * (int64_t)rhsTransform->basisRow2[1];
  lhsElement = lhsTransform->basisRow0[0];
  outTransform->basisRow0[1] = (int)((uint64_t)dotProduct >> 0x20) << 4 | (uint32_t)dotProduct >> 0x1c;
  dotProduct = (int64_t)lhsTransform->basisRow0[1] * (int64_t)rhsTransform->basisRow1[2] +
          (int64_t)lhsElement * (int64_t)rhsTransform->basisRow0[2] +
          (int64_t)lhsTransform->basisRow0[2] * (int64_t)rhsTransform->basisRow2[2];
  lhsElement = lhsTransform->basisRow0[0];
  outTransform->basisRow0[2] = (int)((uint64_t)dotProduct >> 0x20) << 4 | (uint32_t)dotProduct >> 0x1c;
  dotProduct = (int64_t)lhsTransform->basisRow0[1] * (int64_t)(rhsTransform->translation).y +
          (int64_t)lhsElement * (int64_t)(rhsTransform->translation).x +
          (int64_t)lhsTransform->basisRow0[2] * (int64_t)(rhsTransform->translation).z;
  lhsElement = lhsTransform->basisRow1[0];
  (outTransform->translation).x =
       ((int)((uint64_t)dotProduct >> 0x20) << 0x14 | (uint32_t)dotProduct >> 0xc) +
       (lhsTransform->translation).x;
  dotProduct = (int64_t)lhsTransform->basisRow1[1] * (int64_t)rhsTransform->basisRow1[0] +
          (int64_t)lhsElement * (int64_t)rhsTransform->basisRow0[0] +
          (int64_t)lhsTransform->basisRow1[2] * (int64_t)rhsTransform->basisRow2[0];
  lhsElement = lhsTransform->basisRow1[0];
  outTransform->basisRow1[0] = (int)((uint64_t)dotProduct >> 0x20) << 4 | (uint32_t)dotProduct >> 0x1c;
  dotProduct = (int64_t)lhsTransform->basisRow1[1] * (int64_t)rhsTransform->basisRow1[1] +
          (int64_t)lhsElement * (int64_t)rhsTransform->basisRow0[1] +
          (int64_t)lhsTransform->basisRow1[2] * (int64_t)rhsTransform->basisRow2[1];
  lhsElement = lhsTransform->basisRow1[0];
  outTransform->basisRow1[1] = (int)((uint64_t)dotProduct >> 0x20) << 4 | (uint32_t)dotProduct >> 0x1c;
  dotProduct = (int64_t)lhsTransform->basisRow1[1] * (int64_t)rhsTransform->basisRow1[2] +
          (int64_t)lhsElement * (int64_t)rhsTransform->basisRow0[2] +
          (int64_t)lhsTransform->basisRow1[2] * (int64_t)rhsTransform->basisRow2[2];
  lhsElement = lhsTransform->basisRow1[0];
  outTransform->basisRow1[2] = (int)((uint64_t)dotProduct >> 0x20) << 4 | (uint32_t)dotProduct >> 0x1c;
  dotProduct = (int64_t)lhsTransform->basisRow1[1] * (int64_t)(rhsTransform->translation).y +
          (int64_t)lhsElement * (int64_t)(rhsTransform->translation).x +
          (int64_t)lhsTransform->basisRow1[2] * (int64_t)(rhsTransform->translation).z;
  lhsElement = lhsTransform->basisRow2[0];
  (outTransform->translation).y =
       ((int)((uint64_t)dotProduct >> 0x20) << 0x14 | (uint32_t)dotProduct >> 0xc) +
       (lhsTransform->translation).y;
  dotProduct = (int64_t)lhsTransform->basisRow2[1] * (int64_t)rhsTransform->basisRow1[0] +
          (int64_t)lhsElement * (int64_t)rhsTransform->basisRow0[0] +
          (int64_t)lhsTransform->basisRow2[2] * (int64_t)rhsTransform->basisRow2[0];
  lhsElement = lhsTransform->basisRow2[0];
  outTransform->basisRow2[0] = (int)((uint64_t)dotProduct >> 0x20) << 4 | (uint32_t)dotProduct >> 0x1c;
  dotProduct = (int64_t)lhsTransform->basisRow2[1] * (int64_t)rhsTransform->basisRow1[1] +
          (int64_t)lhsElement * (int64_t)rhsTransform->basisRow0[1] +
          (int64_t)lhsTransform->basisRow2[2] * (int64_t)rhsTransform->basisRow2[1];
  lhsElement = lhsTransform->basisRow2[0];
  outTransform->basisRow2[1] = (int)((uint64_t)dotProduct >> 0x20) << 4 | (uint32_t)dotProduct >> 0x1c;
  dotProduct = (int64_t)lhsTransform->basisRow2[1] * (int64_t)rhsTransform->basisRow1[2] +
          (int64_t)lhsElement * (int64_t)rhsTransform->basisRow0[2] +
          (int64_t)lhsTransform->basisRow2[2] * (int64_t)rhsTransform->basisRow2[2];
  lhsElement = lhsTransform->basisRow2[0];
  outTransform->basisRow2[2] = (int)((uint64_t)dotProduct >> 0x20) << 4 | (uint32_t)dotProduct >> 0x1c;
  dotProduct = (int64_t)lhsTransform->basisRow2[1] * (int64_t)(rhsTransform->translation).y +
          (int64_t)lhsElement * (int64_t)(rhsTransform->translation).x +
          (int64_t)lhsTransform->basisRow2[2] * (int64_t)(rhsTransform->translation).z;
  (outTransform->translation).z =
       ((int)((uint64_t)dotProduct >> 0x20) << 0x14 | (uint32_t)dotProduct >> 0xc) +
       (lhsTransform->translation).z;
  return;
}


/* Address: 0x004CD690.
   Fills one projected triangle (tile-relative Q12 texel coordinates, from
   GraphicsShadingGeneratedTexture_TransformPointXYQuantized) with 0xFF in the current shadow tile: sorts the
   vertices by Y, clamps Y and the vertex X values to the tile (+/- the grid origin), then draws horizontal
   spans between the long top-to-bottom edge and the two short edges. Called per triangle by the silhouette
   passes (GraphicsShadingGeneratedTexture_RasterizeRecordBatchFlag0Set/Flag0Clear).
*/
void GraphicsShadingGeneratedTexture_RasterizeTriangleMask
          (GraphicsFixedVec2 *vertexA,GraphicsFixedVec2 *vertexB,GraphicsFixedVec2 *vertexC)

{
  int upperEdgeStep;
  int upperRowsOrSpanCount;
  int yOrShortEdgeDelta;
  int spanDelta;
  int spanRemaining;
  int yOrLongEdgeStep;
  int yOrRowsRemaining;
  int32_t bottomXClamped;
  int xDeltaOrSpan;
  GraphicsFixedVec2 *otherVertex;
  GraphicsFixedVec2 *bottomVertex;
  GraphicsFixedVec2 *middleVertex;
  uint8_t *rowPixels;
  uint8_t *spanPixel;
  int32_t upperEdgeX;
  int topYOrLongEdgeX;
  int32_t lowerEdgeX;
  
  /* sort by Y: vertexC ends up as the top vertex, then middleVertex, then bottomVertex */
  topYOrLongEdgeX = vertexC->component1;
  yOrRowsRemaining = vertexB->component1;
  yOrLongEdgeStep = vertexA->component1;
  yOrShortEdgeDelta = topYOrLongEdgeX;
  middleVertex = vertexB;
  if (yOrRowsRemaining < topYOrLongEdgeX) {
    yOrShortEdgeDelta = yOrRowsRemaining;
    yOrRowsRemaining = topYOrLongEdgeX;
    middleVertex = vertexC;
    vertexC = vertexB;
  }
  topYOrLongEdgeX = yOrShortEdgeDelta;
  otherVertex = vertexA;
  if (yOrLongEdgeStep < yOrShortEdgeDelta) {
    topYOrLongEdgeX = yOrLongEdgeStep;
    yOrLongEdgeStep = yOrShortEdgeDelta;
    otherVertex = vertexC;
    vertexC = vertexA;
  }
  yOrShortEdgeDelta = yOrRowsRemaining;
  bottomVertex = otherVertex;
  if (yOrLongEdgeStep < yOrRowsRemaining) {
    yOrShortEdgeDelta = yOrLongEdgeStep;
    yOrLongEdgeStep = yOrRowsRemaining;
    bottomVertex = middleVertex;
    middleVertex = otherVertex;
  }
  if (topYOrLongEdgeX < g_GraphicsShadingNegativeGridOriginQ12) {
    if (yOrShortEdgeDelta < g_GraphicsShadingNegativeGridOriginQ12) {
      yOrShortEdgeDelta = g_GraphicsShadingNegativeGridOriginQ12;
    }
    topYOrLongEdgeX = g_GraphicsShadingNegativeGridOriginQ12;
    if (yOrLongEdgeStep < g_GraphicsShadingNegativeGridOriginQ12) {
      return;
    }
  }
  if (g_GraphicsShadingPositiveGridOriginQ12 < yOrLongEdgeStep) {
    if (g_GraphicsShadingPositiveGridOriginQ12 < yOrShortEdgeDelta) {
      yOrShortEdgeDelta = g_GraphicsShadingPositiveGridOriginQ12;
    }
    yOrLongEdgeStep = g_GraphicsShadingPositiveGridOriginQ12;
    if (g_GraphicsShadingPositiveGridOriginQ12 < topYOrLongEdgeX) {
      return;
    }
  }
  yOrRowsRemaining = yOrLongEdgeStep - topYOrLongEdgeX >> 0xc;
  if (yOrRowsRemaining != 0) {
    upperRowsOrSpanCount = yOrShortEdgeDelta - topYOrLongEdgeX >> 0xc;
    yOrLongEdgeStep = vertexC->component0;
    yOrShortEdgeDelta = middleVertex->component0;
    xDeltaOrSpan = bottomVertex->component0;
    upperEdgeX = g_GraphicsShadingNegativeGridOriginQ12;
    if ((g_GraphicsShadingNegativeGridOriginQ12 <= yOrLongEdgeStep) &&
       (upperEdgeX = yOrLongEdgeStep, g_GraphicsShadingPositiveGridOriginQ12 < yOrLongEdgeStep)) {
      upperEdgeX = g_GraphicsShadingPositiveGridOriginQ12;
    }
    lowerEdgeX = g_GraphicsShadingNegativeGridOriginQ12;
    if ((g_GraphicsShadingNegativeGridOriginQ12 <= yOrShortEdgeDelta) &&
       (lowerEdgeX = yOrShortEdgeDelta, g_GraphicsShadingPositiveGridOriginQ12 < yOrShortEdgeDelta)) {
      lowerEdgeX = g_GraphicsShadingPositiveGridOriginQ12;
    }
    bottomXClamped = g_GraphicsShadingNegativeGridOriginQ12;
    if ((g_GraphicsShadingNegativeGridOriginQ12 <= xDeltaOrSpan) &&
       (bottomXClamped = xDeltaOrSpan, g_GraphicsShadingPositiveGridOriginQ12 < xDeltaOrSpan)) {
      bottomXClamped = g_GraphicsShadingPositiveGridOriginQ12;
    }
    yOrShortEdgeDelta = lowerEdgeX - upperEdgeX;
    xDeltaOrSpan = bottomXClamped - upperEdgeX;
    rowPixels = g_GraphicsShadingGeneratedTexturePixelCursor +
              (topYOrLongEdgeX >> 0xc) * g_GraphicsShadingTextureDimension;
    yOrLongEdgeStep = xDeltaOrSpan / yOrRowsRemaining;
    topYOrLongEdgeX = upperEdgeX;
    if (upperRowsOrSpanCount != 0) {
      upperEdgeStep = yOrShortEdgeDelta / upperRowsOrSpanCount;
      do {
        spanPixel = rowPixels + (upperEdgeX >> 0xc);
        spanDelta = (topYOrLongEdgeX >> 0xc) - (upperEdgeX >> 0xc);
        if (spanDelta != 0) {
          spanRemaining = spanDelta;
          if (spanDelta < 0) {
            spanRemaining = -spanDelta;
            spanPixel = spanPixel + spanDelta;
          }
          for (; spanRemaining != 0; spanRemaining--) {
            *spanPixel = 0xff;
            spanPixel++;
          }
        }
        rowPixels = rowPixels + g_GraphicsShadingTextureDimension;
        upperEdgeX = upperEdgeX + upperEdgeStep;
        topYOrLongEdgeX = topYOrLongEdgeX + yOrLongEdgeStep;
        /* the original keeps the total row count in MM2 and decrements it with PSUBD by this {1, 0} constant */
        yOrRowsRemaining = yOrRowsRemaining - (int)g_GraphicsShadingRasterizeMmxPackedDwordOneZero;
        upperRowsOrSpanCount--;
      } while (upperRowsOrSpanCount != 0);
    }
    if (yOrRowsRemaining != 0) {
      yOrShortEdgeDelta = (xDeltaOrSpan - yOrShortEdgeDelta) / yOrRowsRemaining;
      do {
        spanPixel = rowPixels + (lowerEdgeX >> 0xc);
        xDeltaOrSpan = (topYOrLongEdgeX >> 0xc) - (lowerEdgeX >> 0xc);
        if (xDeltaOrSpan != 0) {
          upperRowsOrSpanCount = xDeltaOrSpan;
          if (xDeltaOrSpan < 0) {
            upperRowsOrSpanCount = -xDeltaOrSpan;
            spanPixel = spanPixel + xDeltaOrSpan;
          }
          for (; upperRowsOrSpanCount != 0; upperRowsOrSpanCount--) {
            *spanPixel = 0xff;
            spanPixel++;
          }
        }
        rowPixels = rowPixels + g_GraphicsShadingTextureDimension;
        lowerEdgeX = lowerEdgeX + yOrShortEdgeDelta;
        topYOrLongEdgeX = topYOrLongEdgeX + yOrLongEdgeStep;
        yOrRowsRemaining--;
      } while (yOrRowsRemaining != 0);
    }
  }
  return;
}


/* Not in the original: the original executable carries g_PackedLightingLookupTable precomputed
   (0x0041DE80, 512 entries). Each entry holds one light level as four Q12 words for PMULHW: the
   same factor in the three colour lanes and 0x1000 (1.0) in the alpha lane. Levels 0..255 map to
   (level * 0x101) >> 4, i.e. 0..0x0FFF; levels 256..511 saturate at 0x1000. Called once at startup. */
void GraphicsLighting_BuildPackedLookupTable(void)
{
  int level;

  for (level = 0; level < 512; level++) {
    uint64_t factor = level < 256 ? (uint64_t)((level * 0x101) >> 4) : 0x1000;
    g_PackedLightingLookupTable[level] = factor | factor << 16 | factor << 32 | (uint64_t)0x1000 << 48;
  }
}

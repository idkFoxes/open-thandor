/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/src/graphics/render/shadow_texture.cpp
 * Reverse engineering by idkFoxes 2026
 */

#include <thandor/graphics/render/shadow_texture.h>
#include <thandor/thandor.h>
#include <thandor/core/color_lanes.h>

/* the original offsets of the render context view and the 0x80-byte primitive blocks */
static_assert(offsetof(GeneratedTextureRenderContextView, fieldGrid) == 0x54 &&
              offsetof(GeneratedTextureRenderContextView, lightAzimuthAngle) == 0xB8 &&
              offsetof(GeneratedTextureRenderContextView, projectedPointBlockPool) == 0xC8,
              "GeneratedTextureRenderContextView layout");
static_assert(offsetof(GraphicsPrimitivePacket, textureEntry) == 0x64 && offsetof(GraphicsPrimitiveQueue, primaryNodes) == 0x20,
              "primitive block layout");
static_assert(sizeof(GraphicsPrimitivePacket) == GRAPHICS_PROJECTED_BLOCK_BYTES, "a primitive block is one packet");

/* Module data. */

uint32_t g_GraphicsShadingTextureDimension = 0;

uint32_t g_GraphicsShadingGridHalfSize = 0;

uint8_t *g_GraphicsShadingGeneratedTexturePixelCursor = nullptr;

uint32_t g_GraphicsShadingGeneratedTextureTileX = 0;

uint32_t g_GraphicsShadingGeneratedTextureTileY = 0;

GraphicsSubresourceIndex g_GraphicsShadingGeneratedTextureSubresourceIndex = 0;

uint32_t g_GraphicsShadingSubresourceCount = 0;

uint32_t g_GraphicsShadingGeneratedTextureTileXQ20 = 0;

uint32_t g_GraphicsShadingGeneratedTextureTileYQ20 = 0;

uint32_t g_GraphicsShadingGridStepQ20 = 0;

int32_t g_GraphicsShadingGridStepQ20Current = 0;

GraphicsTextureSourceAsset *g_GraphicsShadingGeneratedAsset = nullptr;

void *g_GraphicsShadingGridScratch = nullptr;

void *g_GraphicsShadingGridScratchInterior = nullptr;

GraphicsTextureSet *g_GraphicsShadingTextureSet = nullptr;

uint32_t g_GraphicsShadingGeneratedTextureCompletedTraversalCount = 0;

int32_t g_GraphicsShadingPositiveGridOriginQ12 = 0;

int32_t g_GraphicsShadingNegativeGridOriginQ12 = 0;

GeneratedTextureScratchRuntime g_GeneratedTextureScratchRuntime = {.reserved14 = 0x90909090, .reserved1A4 = 0x90909090};

static const uint64_t g_GraphicsShadingRasterizeMmxPackedDwordOneZero = 0x1ull;

static const uint64_t g_GraphicsShadingMmxPacked3BitPerByteMask = 0x707070707070707ull;

uint32_t g_TextureDownsampleShift = 0;

/* Implementation ownership: graphics/render/shadow_texture. */

/* Not in the original: adds a direction vector to a point. */
static void Shading_AddDirectionToPoint(GraphicsFixedVec3 *point,FixedDirection direction)
{
  point->x = point->x + direction.x;
  point->y = point->y + direction.y;
  point->z = point->z + direction.z;
}

/* Not in the original: subtracts a direction vector from a point. */
static void Shading_SubtractDirectionFromPoint(GraphicsFixedVec3 *point,FixedDirection direction)
{
  point->x = point->x - direction.x;
  point->y = point->y - direction.y;
  point->z = point->z - direction.z;
}

/* Not in the original as a separate function: scale factor that maps a projected extent of the model onto
   the usable part of the shadow grid: (halfSize - border) << 24, sign-extended to 64 bits and divided
   unsigned by the extent (as in the original). */
static int Shading_GeneratedTextureScale(uint32_t projectedExtent)
{
  return (int)((uint64_t)((int64_t)(int)(g_GraphicsShadingGridHalfSize -
                                         g_GeneratedTextureScratchRuntime.downsampleBorderOffset) *
                          (1 << 24)) /
               (uint64_t)projectedExtent);
}

/* Not in the original as a separate function: first phase of
   GraphicsShadingGeneratedTexture_ProcessRenderableHierarchy. Measures the model's projected bounds in light
   space, centres the shadow origin on them, sets up the generated texture basis and places the twelve sample
   points (corners and edge midpoints of the bounds plus four inner points) with their texture coordinate
   offsets. */
static void GraphicsShadingGeneratedTexture_PlaceSamplePoints
          (ModelRuntimeNode *modelNode,GeneratedTextureRenderContextView *renderContext)
{
  struct GeneratedTextureSampleWorkRecord *samples;
  FixedDirection direction;
  FixedDirection halfDirection;
  int textureScale;
  int sampleIndex;

  samples = g_GeneratedTextureScratchRuntime.samples;
  g_GeneratedTextureScratchRuntime.projectedMinX = INT32_MAX;
  g_GeneratedTextureScratchRuntime.projectedMinY = INT32_MAX;
  g_GeneratedTextureScratchRuntime.projectedMaxX = -INT32_MAX;
  g_GeneratedTextureScratchRuntime.projectedMaxY = -INT32_MAX;
  GraphicsShadingGeneratedTexture_TraverseHierarchyAndAccumulateProjectedBounds(modelNode);
  /* move the origin to the centre of the bounds along both light-space axes */
  direction = FixedMath_DirectionFromAnglesScaled
                     (0,(renderContext->lightAzimuthAngle + FIXED_ANGLE16_QUARTER_TURN) & FIXED_ANGLE16_MASK,
                      (g_GeneratedTextureScratchRuntime.projectedMinX +
                       g_GeneratedTextureScratchRuntime.projectedMaxX) >> 1);
  Shading_SubtractDirectionFromPoint(&g_GeneratedTextureScratchRuntime.currentModelOriginQ12,direction);
  direction = FixedMath_DirectionFromAnglesScaled
                     (renderContext->lightElevationAngle + FIXED_ANGLE16_QUARTER_TURN,
                      renderContext->lightAzimuthAngle,
                      (g_GeneratedTextureScratchRuntime.projectedMinY +
                       g_GeneratedTextureScratchRuntime.projectedMaxY) >> 1);
  Shading_SubtractDirectionFromPoint(&g_GeneratedTextureScratchRuntime.currentModelOriginQ12,direction);
  for (sampleIndex = 0; sampleIndex < 12; sampleIndex++) {
    samples[sampleIndex].worldPoint = g_GeneratedTextureScratchRuntime.currentModelOriginQ12;
  }
  textureScale = Shading_GeneratedTextureScale
                           ((uint32_t)(g_GeneratedTextureScratchRuntime.projectedMaxX -
                                       g_GeneratedTextureScratchRuntime.projectedMinX));
  g_GeneratedTextureScratchRuntime.generatedTextureBasisTransform.basisRow0[0] =
       FIXED_MUL_SHR(g_AuxiliaryRotationMatrixFixed.basisRow0[0],textureScale,Q28_SHIFT);
  g_GeneratedTextureScratchRuntime.generatedTextureBasisTransform.basisRow0[1] =
       FIXED_MUL_SHR(g_AuxiliaryRotationMatrixFixed.basisRow0[1],textureScale,Q28_SHIFT);
  g_GeneratedTextureScratchRuntime.generatedTextureBasisTransform.basisRow0[2] =
       FIXED_MUL_SHR(g_AuxiliaryRotationMatrixFixed.basisRow0[2],textureScale,Q28_SHIFT);
  g_GeneratedTextureScratchRuntime.generatedTextureBasisTransform.basisRow2[0] = 0;
  g_GeneratedTextureScratchRuntime.generatedTextureBasisTransform.basisRow2[1] = 0;
  g_GeneratedTextureScratchRuntime.generatedTextureBasisTransform.basisRow2[2] = 0;
  textureScale = Shading_GeneratedTextureScale
                           ((uint32_t)(g_GeneratedTextureScratchRuntime.projectedMaxY -
                                       g_GeneratedTextureScratchRuntime.projectedMinY));
  g_GeneratedTextureScratchRuntime.generatedTextureBasisTransform.basisRow1[0] =
       FIXED_MUL_SHR(g_AuxiliaryRotationMatrixFixed.basisRow1[0],textureScale,Q28_SHIFT);
  g_GeneratedTextureScratchRuntime.generatedTextureBasisTransform.basisRow1[1] =
       FIXED_MUL_SHR(g_AuxiliaryRotationMatrixFixed.basisRow1[1],textureScale,Q28_SHIFT);
  g_GeneratedTextureScratchRuntime.generatedTextureBasisTransform.basisRow1[2] =
       FIXED_MUL_SHR(g_AuxiliaryRotationMatrixFixed.basisRow1[2],textureScale,Q28_SHIFT);
  g_GeneratedTextureScratchRuntime.generatedTextureBasisTransform.translation.x = 0;
  g_GeneratedTextureScratchRuntime.generatedTextureBasisTransform.translation.y = 0;
  g_GeneratedTextureScratchRuntime.generatedTextureBasisTransform.translation.z = 0;
  /* spread the points over the bounds: half the width along the first light-space axis ... */
  direction = FixedMath_DirectionFromAnglesScaled
                     (0,(renderContext->lightAzimuthAngle + FIXED_ANGLE16_QUARTER_TURN) & FIXED_ANGLE16_MASK,
                      (g_GeneratedTextureScratchRuntime.projectedMaxX -
                       g_GeneratedTextureScratchRuntime.projectedMinX) >> 1);
  Shading_AddDirectionToPoint(&samples[0].worldPoint,direction);
  Shading_AddDirectionToPoint(&samples[1].worldPoint,direction);
  Shading_SubtractDirectionFromPoint(&samples[2].worldPoint,direction);
  Shading_SubtractDirectionFromPoint(&samples[3].worldPoint,direction);
  Shading_AddDirectionToPoint(&samples[4].worldPoint,direction);
  Shading_SubtractDirectionFromPoint(&samples[5].worldPoint,direction);
  halfDirection.x = (uint32_t)((int)direction.x >> 1);
  halfDirection.y = (uint32_t)((int)direction.y >> 1);
  halfDirection.z = (uint32_t)((int)direction.z >> 1);
  Shading_AddDirectionToPoint(&samples[8].worldPoint,halfDirection);
  Shading_AddDirectionToPoint(&samples[9].worldPoint,halfDirection);
  Shading_SubtractDirectionFromPoint(&samples[10].worldPoint,halfDirection);
  Shading_SubtractDirectionFromPoint(&samples[11].worldPoint,halfDirection);
  /* ... and half the height along the second one */
  direction = FixedMath_DirectionFromAnglesScaled
                     (renderContext->lightElevationAngle + FIXED_ANGLE16_QUARTER_TURN,
                      renderContext->lightAzimuthAngle,
                      (g_GeneratedTextureScratchRuntime.projectedMaxY -
                       g_GeneratedTextureScratchRuntime.projectedMinY) >> 1);
  Shading_AddDirectionToPoint(&samples[0].worldPoint,direction);
  Shading_SubtractDirectionFromPoint(&samples[1].worldPoint,direction);
  Shading_AddDirectionToPoint(&samples[2].worldPoint,direction);
  Shading_SubtractDirectionFromPoint(&samples[3].worldPoint,direction);
  Shading_AddDirectionToPoint(&samples[6].worldPoint,direction);
  Shading_SubtractDirectionFromPoint(&samples[7].worldPoint,direction);
  halfDirection.x = (uint32_t)((int)direction.x >> 1);
  halfDirection.y = (uint32_t)((int)direction.y >> 1);
  halfDirection.z = (uint32_t)((int)direction.z >> 1);
  Shading_AddDirectionToPoint(&samples[8].worldPoint,halfDirection);
  Shading_SubtractDirectionFromPoint(&samples[9].worldPoint,halfDirection);
  Shading_AddDirectionToPoint(&samples[10].worldPoint,halfDirection);
  Shading_SubtractDirectionFromPoint(&samples[11].worldPoint,halfDirection);
  samples[1].textureCoordinateOffsetQ20 = g_GraphicsShadingGridStepQ20Current - GRAPHICS_SHADING_SAMPLE_INSET_Q20;
  samples[4].textureCoordinateOffsetQ20 = (int)g_GraphicsShadingGridStepQ20Current >> 1;
  samples[0].textureCoordinateOffsetQ20 = 0;
  samples[2].textureCoordinateOffsetQ20 = 0;
  samples[6].textureCoordinateOffsetQ20 = 0;
  samples[8].textureCoordinateOffsetQ20 =
       samples[4].textureCoordinateOffsetQ20 - ((int)g_GraphicsShadingGridStepQ20Current >> 2);
  samples[9].textureCoordinateOffsetQ20 =
       ((int)g_GraphicsShadingGridStepQ20Current >> 2) + samples[4].textureCoordinateOffsetQ20;
  samples[3].textureCoordinateOffsetQ20 = samples[1].textureCoordinateOffsetQ20;
  samples[5].textureCoordinateOffsetQ20 = samples[4].textureCoordinateOffsetQ20;
  samples[7].textureCoordinateOffsetQ20 = samples[1].textureCoordinateOffsetQ20;
  samples[10].textureCoordinateOffsetQ20 = samples[8].textureCoordinateOffsetQ20;
  samples[11].textureCoordinateOffsetQ20 = samples[9].textureCoordinateOffsetQ20;
}

/* Not in the original as a separate function (the original repeats it inline for each of the twelve sample
   points): moves one sample point along the light direction onto the terrain.
   A point above the terrain is cast along the light onto the terrain triangles (up to twice the subtree
   radius) and records that ray distance. When the cast misses, the point is moved the full length and a
   second, reversed ray searches the terrain surface for the rest of the length; that hit also pushes the
   point's texture coordinate offset outwards.
   A point below the terrain is lifted onto it (by at most the subtree radius) with ray distance 0, which
   pulls its texture coordinate offset inwards; when the offset would pass its start, the offset becomes 0
   and the point is cast again from twice the radius back along the light.
   Returns false when the point finds no terrain or its offset leaves the grid step (the caller abandons the
   model). */
static Bool8 GraphicsShadingGeneratedTexture_DropSampleOntoTerrain
          (struct GeneratedTextureSampleWorkRecord *sample,ModelRuntimeNode *modelNode,
           GeneratedTextureRenderContextView *renderContext)
{
  GraphicsWorldCoordinateQ12 sampleWorldZ;
  Q12 surfaceHeightQ12;
  int32_t startTextureOffset;
  Bool8 terrainHit;
  Q12 terrainHitDistanceQ12;
  int missRayLength;
  Q12 remainingRayLength;
  AngleTurn32 oppositeAzimuth;
  Bool8 terrainSurfaceHit;
  Q12 surfaceDistanceQ12;
  uint32_t terrainHitMaterial;
  uint32_t heightDelta;
  int textureShift;

  sampleWorldZ = sample->worldPoint.z;
  FieldGrid_InterpolateTopSurfaceHeight
            (sample->worldPoint.y,sample->worldPoint.x,renderContext->fieldGrid,&surfaceHeightQ12);
  startTextureOffset = sample->textureCoordinateOffsetQ20;
  if (sampleWorldZ <= surfaceHeightQ12) {
    /* below the terrain: lift the point */
    heightDelta = (uint32_t)(surfaceHeightQ12 - sampleWorldZ);
    if ((uint32_t)modelNode->subtreeBoundingRadiusQ12 <= heightDelta) {
      heightDelta = modelNode->subtreeBoundingRadiusQ12;
    }
    sample->terrainRayDistanceQ12 = 0;
    textureShift = (int)(((int64_t)(int)g_GraphicsShadingGridStepQ20Current *
                          (int64_t)(int)(FIXED_MUL_SHR((int)heightDelta,
                                                       g_FixedSineQ28[FIXED_SINE_TABLE_COS +
                                                                      renderContext->lightElevationAngle],
                                                       Q28_SHIFT + 1))) /
                         (int64_t)modelNode->subtreeBoundingRadiusQ12);
    sample->textureCoordinateOffsetQ20 = sample->textureCoordinateOffsetQ20 - textureShift;
    if (startTextureOffset < textureShift) {
      sample->textureCoordinateOffsetQ20 = 0;
      Shading_SubtractDirectionFromPoint
                (&sample->worldPoint,
                 FixedMath_DirectionFromAnglesScaled
                           (renderContext->lightElevationAngle,renderContext->lightAzimuthAngle,
                            modelNode->subtreeBoundingRadiusQ12 * 2));
      terrainHit = FieldGrid_RaycastTerrainTrianglesAlongDirection
                         (renderContext->lightElevationAngle,renderContext->lightAzimuthAngle,
                          modelNode->subtreeBoundingRadiusQ12 * 2,sample->worldPoint.z,sample->worldPoint.y,
                          sample->worldPoint.x,renderContext->fieldGrid,&terrainHitDistanceQ12);
      if (!terrainHit) {
        return false;
      }
      Shading_AddDirectionToPoint
                (&sample->worldPoint,
                 FixedMath_DirectionFromAnglesScaled
                           (renderContext->lightElevationAngle,renderContext->lightAzimuthAngle,
                            terrainHitDistanceQ12));
    }
    else {
      sample->worldPoint.z = sample->worldPoint.z + heightDelta;
    }
    return true;
  }
  /* above the terrain: cast it along the light */
  terrainHit = FieldGrid_RaycastTerrainTrianglesAlongDirection
                     (renderContext->lightElevationAngle,renderContext->lightAzimuthAngle,
                      modelNode->subtreeBoundingRadiusQ12 * 2,sample->worldPoint.z,sample->worldPoint.y,
                      sample->worldPoint.x,renderContext->fieldGrid,&terrainHitDistanceQ12);
  if (terrainHit) {
    sample->terrainRayDistanceQ12 = terrainHitDistanceQ12;
    Shading_AddDirectionToPoint
              (&sample->worldPoint,
               FixedMath_DirectionFromAnglesScaled
                         (renderContext->lightElevationAngle,renderContext->lightAzimuthAngle,
                          terrainHitDistanceQ12));
    return true;
  }
  missRayLength = modelNode->subtreeBoundingRadiusQ12 * 2;
  if (g_GraphicsShadingGridStepQ20Current - sample->textureCoordinateOffsetQ20 == 0) {
    return false;
  }
  remainingRayLength = (Q12)(((int64_t)(int)(g_GraphicsShadingGridStepQ20Current -
                                             sample->textureCoordinateOffsetQ20) *
                              (int64_t)missRayLength) /
                             (int64_t)(int)g_GraphicsShadingGridStepQ20Current);
  sample->terrainRayDistanceQ12 = missRayLength;
  Shading_AddDirectionToPoint
            (&sample->worldPoint,
             FixedMath_DirectionFromAnglesScaled
                       (renderContext->lightElevationAngle,renderContext->lightAzimuthAngle,missRayLength));
  oppositeAzimuth = renderContext->lightAzimuthAngle ^ FIXED_ANGLE16_HALF_TURN;
  terrainSurfaceHit = FieldGrid_RaycastTerrainSurfaceDistance
                            (-renderContext->lightElevationAngle - FIXED_ANGLE16_QUARTER_TURN,oppositeAzimuth,
                             remainingRayLength,sample->worldPoint.z,sample->worldPoint.y,sample->worldPoint.x,
                             renderContext->fieldGrid,&surfaceDistanceQ12,&terrainHitMaterial);
  if (!terrainSurfaceHit) {
    return false;
  }
  /* Original quirk, kept (all 12 such sites): after the hit the original passes the hit distance, the
     azimuth and, as the elevation, a value it expected to still be -lightElevationAngle - 0x4000. But
     FieldGrid_RaycastTerrainSurfaceDistance overwrites that value with the hit cell's material byte
     (terrainHitMaterial), so the material byte becomes the elevation angle. */
  Shading_AddDirectionToPoint
            (&sample->worldPoint,
             FixedMath_DirectionFromAnglesScaled /* quirk: material byte as elevation */
                       (terrainHitMaterial,oppositeAzimuth,surfaceDistanceQ12));
  sample->textureCoordinateOffsetQ20 =
       sample->textureCoordinateOffsetQ20 +
       ((int)(((int64_t)surfaceDistanceQ12 * (int64_t)(int)g_GraphicsShadingGridStepQ20Current) /
              (int64_t)modelNode->subtreeBoundingRadiusQ12) >> 1);
  if ((int)g_GraphicsShadingGridStepQ20Current < sample->textureCoordinateOffsetQ20) {
    return false;
  }
  return true;
}

/* Not in the original as a table: first point pair of each sample point's vertex in the 14 reserved shadow
   patch blocks (indexed by sample). A vertex is four point pairs: [0] the projected screen point, [1] view x
   and y, [2] view z and texture u, [3] texture v and the vertex colour. The patch's other vertices are copies
   of these twelve (GraphicsShadingGeneratedTexture_ShareShadowPatchVertices). */
static const int s_ShadowSampleVertexPair[12] = {
  GRAPHICS_PROJECTED_PAIR(6,4),GRAPHICS_PROJECTED_PAIR(11,0),GRAPHICS_PROJECTED_PAIR(7,0),
  GRAPHICS_PROJECTED_PAIR(10,8),GRAPHICS_PROJECTED_PAIR(5,4),GRAPHICS_PROJECTED_PAIR(3,8),
  GRAPHICS_PROJECTED_PAIR(2,8),GRAPHICS_PROJECTED_PAIR(4,4),GRAPHICS_PROJECTED_PAIR(0,0),
  GRAPHICS_PROJECTED_PAIR(0,8),GRAPHICS_PROJECTED_PAIR(0,4),GRAPHICS_PROJECTED_PAIR(1,0)
};

/* Not in the original as a separate function: transforms the twelve sample points into view space (pairs
   [1] and [2] of their vertices) and reports whether any of them lies in front of the near plane. */
static Bool8 GraphicsShadingGeneratedTexture_TransformSamplesToView(GraphicsProjectedPointPair *projectedBlocks)
{
  GraphicsProjectedPointPair *vertex;
  int sampleIndex;

  for (sampleIndex = 0; sampleIndex < 12; sampleIndex++) {
    FixedTransform_ApplyPoint
              ((GraphicsFixedVec3 *)(projectedBlocks + s_ShadowSampleVertexPair[sampleIndex] + 1),
               &g_GeneratedTextureScratchRuntime.samples[sampleIndex].worldPoint,&g_ViewProjectionMatrixFixed);
  }
  for (sampleIndex = 0; sampleIndex < 12; sampleIndex++) {
    vertex = projectedBlocks + s_ShadowSampleVertexPair[sampleIndex];
    if (vertex[2].projectedX < (int)g_ProjectionScaleFixed) {
      return true;
    }
  }
  return false;
}

/* Not in the original as a separate function: colour of one shadow vertex. The model tint fades linearly
   with the sample's ray distance to the caster and is gone at GRAPHICS_SHADING_SHADOW_FADE_DISTANCE_Q12. */
static uint32_t Shading_ShadowVertexColour(Q12 terrainRayDistanceQ12,uint64_t tintLanes)
{
  uint32_t intensity;

  intensity = GRAPHICS_SHADING_SHADOW_FADE_DISTANCE_Q12 - terrainRayDistanceQ12;
  if ((int)intensity < 0) {
    intensity = 0;
  }
  intensity = intensity >> 6;
  if (GRAPHICS_SHADING_INTENSITY_MAX < intensity) {
    intensity = GRAPHICS_SHADING_INTENSITY_MAX;
  }
  return ColorLanes_PackWordsUnsignedSaturate
                   (pmulhw(*(uint64_t *)&g_ShadingIntensityScaleMmx[intensity],tintLanes));
}

/* Not in the original as a separate function: completes the twelve sample vertices: projected screen
   point, texture coordinates in the current tile, a depth bias of one world unit towards the viewer and the
   fading vertex colour. */
static void GraphicsShadingGeneratedTexture_FillShadowPatchVertices
          (GraphicsProjectedPointPair *projectedBlocks,ModelRuntimeNode *modelNode)
{
  GraphicsProjectedPointPair *vertex;
  uint32_t gridStep;
  uint32_t tileU;
  int edgeU;
  int middleU;
  int quarterStep;
  int textureU[12];
  uint64_t tintLanes;
  int sampleIndex;

  for (sampleIndex = 0; sampleIndex < 12; sampleIndex++) {
    vertex = projectedBlocks + s_ShadowSampleVertexPair[sampleIndex];
    vertex[0] = Graphics_ProjectViewPoint((GraphicsFixedVec3 *)(vertex + 1));
  }
  gridStep = g_GraphicsShadingGridStepQ20;
  tileU = g_GraphicsShadingGeneratedTextureTileXQ20;
  edgeU = (gridStep - GRAPHICS_SHADING_SAMPLE_INSET_Q20) + tileU;
  middleU = ((int)gridStep >> 1) + tileU;
  quarterStep = (int)gridStep >> 2;
  textureU[0] = tileU;
  textureU[1] = tileU;
  textureU[2] = edgeU;
  textureU[3] = edgeU;
  textureU[4] = tileU;
  textureU[5] = edgeU;
  textureU[6] = middleU;
  textureU[7] = middleU;
  textureU[8] = middleU - quarterStep;
  textureU[9] = middleU - quarterStep;
  textureU[10] = quarterStep + middleU;
  textureU[11] = quarterStep + middleU;
  /* Lanes 0..2 = 0x0fff, lane 3 = the halved tint alpha duplicated to a word, >> 4. */
  tintLanes = ColorLanes_UnpackBytesShiftRight(modelNode->tintArgb >> 1 | ARGB8888_RGB_MASK,4);
  for (sampleIndex = 0; sampleIndex < 12; sampleIndex++) {
    vertex = projectedBlocks + s_ShadowSampleVertexPair[sampleIndex];
    vertex[2].projectedY = textureU[sampleIndex];
    vertex[3].projectedX = g_GeneratedTextureScratchRuntime.samples[sampleIndex].textureCoordinateOffsetQ20 +
                           g_GraphicsShadingGeneratedTextureTileYQ20;
    vertex[2].projectedX = vertex[2].projectedX - Q12_ONE;
    vertex[3].projectedY = Shading_ShadowVertexColour
                                     (g_GeneratedTextureScratchRuntime.samples[sampleIndex].terrainRayDistanceQ12,
                                      tintLanes);
  }
}

/* Not in the original: copies one vertex (four point pairs) of the shadow patch. */
static void Shading_CopyPatchVertex(GraphicsProjectedPointPair *projectedBlocks,int sourcePair,int destinationPair)
{
  int pair;

  for (pair = 0; pair < 4; pair++) {
    projectedBlocks[destinationPair + pair] = projectedBlocks[sourcePair + pair];
  }
}

/* Not in the original as a separate function: fills the remaining vertices of the 14 patch triangles with
   copies of the sample vertices and gives every block the header of block 0. */
static void GraphicsShadingGeneratedTexture_ShareShadowPatchVertices(GraphicsProjectedPointPair *projectedBlocks)
{
  int block;

  Shading_CopyPatchVertex(projectedBlocks,GRAPHICS_PROJECTED_PAIR(0,0),GRAPHICS_PROJECTED_PAIR(2,0));
  Shading_CopyPatchVertex(projectedBlocks,GRAPHICS_PROJECTED_PAIR(0,0),GRAPHICS_PROJECTED_PAIR(5,0));
  Shading_CopyPatchVertex(projectedBlocks,GRAPHICS_PROJECTED_PAIR(0,0),GRAPHICS_PROJECTED_PAIR(6,0));
  Shading_CopyPatchVertex(projectedBlocks,GRAPHICS_PROJECTED_PAIR(0,0),GRAPHICS_PROJECTED_PAIR(13,0));
  Shading_CopyPatchVertex(projectedBlocks,GRAPHICS_PROJECTED_PAIR(0,4),GRAPHICS_PROJECTED_PAIR(1,4));
  Shading_CopyPatchVertex(projectedBlocks,GRAPHICS_PROJECTED_PAIR(0,4),GRAPHICS_PROJECTED_PAIR(2,4));
  Shading_CopyPatchVertex(projectedBlocks,GRAPHICS_PROJECTED_PAIR(0,4),GRAPHICS_PROJECTED_PAIR(3,4));
  Shading_CopyPatchVertex(projectedBlocks,GRAPHICS_PROJECTED_PAIR(0,4),GRAPHICS_PROJECTED_PAIR(7,4));
  Shading_CopyPatchVertex(projectedBlocks,GRAPHICS_PROJECTED_PAIR(0,4),GRAPHICS_PROJECTED_PAIR(8,4));
  Shading_CopyPatchVertex(projectedBlocks,GRAPHICS_PROJECTED_PAIR(0,8),GRAPHICS_PROJECTED_PAIR(1,8));
  Shading_CopyPatchVertex(projectedBlocks,GRAPHICS_PROJECTED_PAIR(0,8),GRAPHICS_PROJECTED_PAIR(4,8));
  Shading_CopyPatchVertex(projectedBlocks,GRAPHICS_PROJECTED_PAIR(0,8),GRAPHICS_PROJECTED_PAIR(5,8));
  Shading_CopyPatchVertex(projectedBlocks,GRAPHICS_PROJECTED_PAIR(0,8),GRAPHICS_PROJECTED_PAIR(11,8));
  Shading_CopyPatchVertex(projectedBlocks,GRAPHICS_PROJECTED_PAIR(0,8),GRAPHICS_PROJECTED_PAIR(12,8));
  Shading_CopyPatchVertex(projectedBlocks,GRAPHICS_PROJECTED_PAIR(1,0),GRAPHICS_PROJECTED_PAIR(3,0));
  Shading_CopyPatchVertex(projectedBlocks,GRAPHICS_PROJECTED_PAIR(1,0),GRAPHICS_PROJECTED_PAIR(4,0));
  Shading_CopyPatchVertex(projectedBlocks,GRAPHICS_PROJECTED_PAIR(1,0),GRAPHICS_PROJECTED_PAIR(9,0));
  Shading_CopyPatchVertex(projectedBlocks,GRAPHICS_PROJECTED_PAIR(1,0),GRAPHICS_PROJECTED_PAIR(10,0));
  Shading_CopyPatchVertex(projectedBlocks,GRAPHICS_PROJECTED_PAIR(2,8),GRAPHICS_PROJECTED_PAIR(6,8));
  Shading_CopyPatchVertex(projectedBlocks,GRAPHICS_PROJECTED_PAIR(2,8),GRAPHICS_PROJECTED_PAIR(7,8));
  Shading_CopyPatchVertex(projectedBlocks,GRAPHICS_PROJECTED_PAIR(3,8),GRAPHICS_PROJECTED_PAIR(8,8));
  Shading_CopyPatchVertex(projectedBlocks,GRAPHICS_PROJECTED_PAIR(3,8),GRAPHICS_PROJECTED_PAIR(9,8));
  Shading_CopyPatchVertex(projectedBlocks,GRAPHICS_PROJECTED_PAIR(4,4),GRAPHICS_PROJECTED_PAIR(10,4));
  Shading_CopyPatchVertex(projectedBlocks,GRAPHICS_PROJECTED_PAIR(4,4),GRAPHICS_PROJECTED_PAIR(11,4));
  Shading_CopyPatchVertex(projectedBlocks,GRAPHICS_PROJECTED_PAIR(5,4),GRAPHICS_PROJECTED_PAIR(12,4));
  Shading_CopyPatchVertex(projectedBlocks,GRAPHICS_PROJECTED_PAIR(5,4),GRAPHICS_PROJECTED_PAIR(13,4));
  Shading_CopyPatchVertex(projectedBlocks,GRAPHICS_PROJECTED_PAIR(6,4),GRAPHICS_PROJECTED_PAIR(13,8));
  Shading_CopyPatchVertex(projectedBlocks,GRAPHICS_PROJECTED_PAIR(7,0),GRAPHICS_PROJECTED_PAIR(8,0));
  Shading_CopyPatchVertex(projectedBlocks,GRAPHICS_PROJECTED_PAIR(10,8),GRAPHICS_PROJECTED_PAIR(9,4));
  Shading_CopyPatchVertex(projectedBlocks,GRAPHICS_PROJECTED_PAIR(11,0),GRAPHICS_PROJECTED_PAIR(12,0));
  /* block header (pairs 12..15, four pairs like a vertex) */
  for (block = 1; block < GRAPHICS_SHADOW_PATCH_BLOCK_COUNT; block++) {
    Shading_CopyPatchVertex(projectedBlocks,GRAPHICS_PROJECTED_PAIR(0,12),GRAPHICS_PROJECTED_PAIR(block,12));
  }
}

/* Casts the shadow of one model hierarchy onto the terrain (world view render pass, context flag 0x20000,
   called per candidate model from the frontend world render in src/ui/frontend/menu_room.cpp). While the
   generated shadow textures still have a free tile, a model inside the view frustum gets twelve sample
   points (corners and edge midpoints of its light-space bounds plus four inner points); each point is moved
   along the light direction (renderContext angles) onto the terrain, and the 14 reserved projected point blocks
   become a textured patch whose vertex alpha fades with the ray distance to the caster. The model's
   silhouette is then rasterized into the current texture tile (blurred for MODEL_MESH_SOFT_SHADOW meshes, sharp
   for the rest) and the tile cursor advances. Any point without a terrain hit abandons the model.
*/
void GraphicsShadingGeneratedTexture_ProcessRenderableHierarchy
          (ModelRuntimeNode *modelNode,GeneratedTextureRenderContextView *renderContext)

{
  /* the original handles the sample points in this order */
  static const int sampleDropOrder[12] = {0,2,1,3,4,5,6,7,8,10,9,11};
  int subtreeRadius;
  int frustumLimit;
  int plane;
  int orderIndex;
  GraphicsProjectedPointPair *projectedBlocks;

  if (g_GraphicsShadingGeneratedTextureCompletedTraversalCount != 0) {
    return;
  }
  g_GeneratedTextureScratchRuntime.currentModelOriginQ12.x = (modelNode->worldTransform).translation.x;
  g_GeneratedTextureScratchRuntime.currentModelOriginQ12.y = (modelNode->worldTransform).translation.y;
  g_GeneratedTextureScratchRuntime.currentModelOriginQ12.z = (modelNode->worldTransform).translation.z;
  subtreeRadius = modelNode->subtreeBoundingRadiusQ12;
  g_ModelCullViewRelative.x = g_GeneratedTextureScratchRuntime.currentModelOriginQ12.x - g_ViewOriginFixed.x;
  g_ModelCullViewRelative.y = g_GeneratedTextureScratchRuntime.currentModelOriginQ12.y - g_ViewOriginFixed.y;
  g_ModelCullViewRelative.z = g_GeneratedTextureScratchRuntime.currentModelOriginQ12.z - g_ViewOriginFixed.z;
  if (GraphicsShadingGeneratedTexture_ProbeHierarchyForGeometry(modelNode)) {
    return;
  }
  /* frustum test with 2.25 times the subtree radius, since the shadow reaches beyond the model */
  frustumLimit = (int)((uint32_t)(subtreeRadius * 9) >> 2);
  for (plane = 0; plane < 4; plane++) {
    if (frustumLimit < FixedVec3_DotQ28(g_FrustumPlaneNormalFixed_0 + plane,
                                        &g_ModelCullViewRelative)) {
      return;
    }
  }
  FixedTransform_ApplyPoint
            (&g_ModelCullViewRelative,&g_GeneratedTextureScratchRuntime.currentModelOriginQ12,
             &g_ViewProjectionMatrixFixed);
  if (g_ModelCullViewRelative.z <= g_ProjectionScaleFixed ||
      g_ModelCullViewRelative.z - g_ProjectionScaleFixed <=
      ((modelNode->modelPayload).modelResource)->boundingRadiusQ12) {
    return;
  }
  GraphicsShadingGeneratedTexture_PlaceSamplePoints(modelNode,renderContext);
  for (orderIndex = 0; orderIndex < 12; orderIndex++) {
    if (!GraphicsShadingGeneratedTexture_DropSampleOntoTerrain
                   (&g_GeneratedTextureScratchRuntime.samples[sampleDropOrder[orderIndex]],modelNode,
                    renderContext)) {
      return;
    }
  }
  projectedBlocks = GraphicsShadingGeneratedTexture_ReserveFourteenProjectedPointBlocks(renderContext);
  if (projectedBlocks == nullptr) {
    return;
  }
  if (GraphicsShadingGeneratedTexture_TransformSamplesToView(projectedBlocks)) {
    GraphicsShadingGeneratedTexture_RollbackFourteenProjectedPointBlocks(renderContext);
    return;
  }
  GraphicsShadingGeneratedTexture_FillShadowPatchVertices(projectedBlocks,modelNode);
  /* the first block's packet header (pair 12 of block 0): the generated texture's entry */
  ((GraphicsPrimitivePacket *)projectedBlocks)->textureEntry =
       g_GraphicsShadingTextureSet->entries + g_GraphicsShadingGeneratedTextureSubresourceIndex;
  GraphicsShadingGeneratedTexture_ShareShadowPatchVertices(projectedBlocks);
  /* The original tests the result the soft shadow traversal leaves (its return value here). */
  if (GraphicsShadingGeneratedTexture_RasterizeSoftShadowHierarchy(modelNode) != 0) {
    GraphicsShadingGeneratedTexture_FilterGridScratchMmx();
  }
  GraphicsShadingGeneratedTexture_RasterizeHardShadowHierarchy(modelNode);
  GraphicsShadingGeneratedTexture_AdvanceTileCursor();
}








/* Sets up the generated shading textures: a zeroed square scratch grid of (2 * gridHalfSize)^2 bytes and
   an in-memory gfx asset with one palette (white with an alpha ramp) and subresourceCount 8-bit images of
   textureDimension^2 pixels, from which a texture set is created. Also derives the grid step and origin used to
   map world positions into the textures. Returns 0 on success, otherwise the (non-zero) allocator or
   texture-set error code.
*/
uint32_t GraphicsShadingRuntime_InitializeGeneratedTexture
          (GraphicsAssetSubresourceCount subresourceCount,GraphicsPixelDimension gridHalfSize,
          GraphicsPixelDimension textureDimension)

{
  void *gridScratch;
  GraphicsTextureSourceAsset *asset;
  uint32_t *dwordCursor;
  uint32_t gridByteCount;
  uint32_t gridDwordCount;
  uint32_t assetByteCount;
  uint32_t dwordsRemaining;
  PackedArgb32 paletteArgb;
  GraphicsTexturePaletteEntry *paletteEntry;
  int paletteEntriesRemaining;
  GraphicsTextureSourceEntry *sourceEntry;
  AssetRelativeOffset pixelDataOffset;
  int gridOriginCells;
  uint32_t allocError;
  GraphicsTextureSet *createdTextureSet;
  uint32_t textureSetError;

  gridByteCount = gridHalfSize * 2 * gridHalfSize * 2;
  allocError = g_MemoryApi.alloc(gridByteCount,&gridScratch);
  if (allocError != 0) {
    return allocError;
  }
  gridDwordCount = gridByteCount >> 2;
  /* interior: gridDwordCount = gridHalfSize^2 bytes = gridHalfSize / 2 rows of 2 * gridHalfSize bytes, then
     gridHalfSize / 2 columns in */
  g_GraphicsShadingGridScratchInterior = (void *)((uint8_t *)gridScratch + gridDwordCount + (gridHalfSize >> 1));
  g_GraphicsShadingGridScratch = gridScratch;
  dwordCursor = (uint32_t *)gridScratch;
  for (dwordsRemaining = gridDwordCount; dwordsRemaining != 0; dwordsRemaining--) {
    *dwordCursor = 0;
    dwordCursor++;
  }
  /* gfx layout: header and palette up to 0xA00, then one 0x20-byte source entry per image, then the pixels */
  assetByteCount = (textureDimension * textureDimension + GFX_SUBRESOURCE_RECORD_SIZE) * subresourceCount +
                   GFX_ASSET_HEADER_SIZE + GFX_PALETTE_BANK_SIZE;
  allocError = g_MemoryApi.alloc(assetByteCount,(void **)&asset);
  if (allocError != 0) {
    /* the original leaked the scratch grid here; freed and cleared so a later shutdown does not see it */
    g_MemoryApi.free(gridScratch);
    g_GraphicsShadingGridScratch = nullptr;
    g_GraphicsShadingGridScratchInterior = nullptr;
    return allocError;
  }
  g_GraphicsShadingGeneratedAsset = asset;
  dwordCursor = (uint32_t *)asset;
  for (dwordsRemaining = assetByteCount >> 2; dwordsRemaining != 0; dwordsRemaining--) {
    *dwordCursor = 0;
    dwordCursor++;
  }
  asset->common.magic = ASSET_MAGIC_GFX;
  asset->common.allocationSizeBytes = assetByteCount;
  asset->tableDescriptor.subresourceCount = subresourceCount;
  asset->tableDescriptor.paletteBankCount = 1;
  asset->tableDescriptor.subresourceTableOffset = GFX_ASSET_HEADER_SIZE + GFX_PALETTE_BANK_SIZE;
  /* palette at +0x200: 256 ARGB entries 8 bytes apart (up to 0xA00), white with alpha = index */
  paletteArgb = ARGB8888_RGB_MASK;
  paletteEntry = (GraphicsTexturePaletteEntry *)((uint8_t *)asset + GFX_ASSET_HEADER_SIZE);
  for (paletteEntriesRemaining = GRAPHICS_PALETTE_BANK_ENTRIES; paletteEntriesRemaining != 0;
       paletteEntriesRemaining--) {
    paletteEntry->argb8888 = paletteArgb;
    paletteArgb = paletteArgb + ARGB8888_ALPHA_ONE;
    paletteEntry++;
  }
  g_GraphicsShadingSubresourceCount = subresourceCount;
  /* source entry table at 0xA00 */
  sourceEntry = (GraphicsTextureSourceEntry *)((uint8_t *)asset + GFX_ASSET_HEADER_SIZE + GFX_PALETTE_BANK_SIZE);
  pixelDataOffset = subresourceCount * GFX_SUBRESOURCE_RECORD_SIZE + GFX_ASSET_HEADER_SIZE + GFX_PALETTE_BANK_SIZE;
  /* Original quirk: the count is tested at the end, so subresourceCount 0 would wrap around */
  do {
    sourceEntry->logicalWidth = textureDimension;
    sourceEntry->logicalHeight = textureDimension;
    sourceEntry->paletteIndex = 0;
    sourceEntry->dataOffset = pixelDataOffset;
    sourceEntry->originX = 0;
    sourceEntry->originY = 0;
    sourceEntry->pixelWidth = textureDimension;
    sourceEntry->pixelHeight = textureDimension;
    pixelDataOffset = pixelDataOffset + textureDimension * textureDimension;
    sourceEntry++;
    subresourceCount--;
  } while (subresourceCount != 0);
  g_GraphicsShadingTextureDimension = textureDimension;
  g_GraphicsShadingGridHalfSize = gridHalfSize;
  /* grid cells per texture pixel in Q20, and the grid origin (gridHalfSize / 2 - 1 cells) in Q12 */
  g_GraphicsShadingGridStepQ20 =
       (uint32_t)(((uint64_t)gridHalfSize * (1 << Q20_SHIFT)) / (uint64_t)textureDimension);
  gridOriginCells = ((int)gridHalfSize >> 1) - 1;
  g_GraphicsShadingPositiveGridOriginQ12 = gridOriginCells * Q12_ONE;
  g_GraphicsShadingNegativeGridOriginQ12 = gridOriginCells * -Q12_ONE;
  g_GraphicsShadingGridStepQ20Current = g_GraphicsShadingGridStepQ20;
  createdTextureSet = g_GraphicsCreateTextureSet(g_GraphicsShadingGeneratedAsset,&textureSetError);
  if (createdTextureSet == nullptr) {
    /* The original kept the freed asset in g_GraphicsShadingGeneratedAsset (GraphicsShadingRuntime_Shutdown then
       freed it a second time) and the scratch grid allocated; both are freed and cleared here. */
    g_MemoryApi.free(g_GraphicsShadingGeneratedAsset);
    g_GraphicsShadingGeneratedAsset = nullptr;
    g_MemoryApi.free(gridScratch);
    g_GraphicsShadingGridScratch = nullptr;
    g_GraphicsShadingGridScratchInterior = nullptr;
    return textureSetError;
  }
  g_GraphicsShadingTextureSet = createdTextureSet;
  return 0;
}


/* Counterpart of GraphicsShadingRuntime_InitializeGeneratedTexture: destroys the texture set, frees the generated
   gfx asset and the scratch grid, and clears the three pointers.
*/
void GraphicsShadingRuntime_Shutdown()

{
  g_GraphicsDestroyTextureSet(g_GraphicsShadingTextureSet);
  g_GraphicsShadingTextureSet = nullptr;
  g_MemoryApi.free(g_GraphicsShadingGeneratedAsset);
  g_GraphicsShadingGeneratedAsset = nullptr;
  g_MemoryApi.free(g_GraphicsShadingGridScratch);
  g_GraphicsShadingGridScratch = nullptr;
  return;
}


/* Starts a shadow pass (frontend world render in src/ui/frontend/menu_room.cpp, before the per-model
   GraphicsShadingGeneratedTexture_ProcessRenderableHierarchy calls): puts the tile cursor on the first
   tile of subresource 0 (the pixel cursor at the tile centre), clears the tile/subresource counters and the
   "all tiles used" count, and zeroes the 8-bit pixels of every generated shadow texture.
*/
void GraphicsShadingGeneratedTexture_ResetPassScratchAndClearAlphaPlanes()

{
  AssetRelativeOffset tableOffset;
  uint32_t dwordsRemaining;
  uint8_t *alphaCursor;
  
  g_GeneratedTextureScratchRuntime.downsampleBorderOffset = g_TextureDownsampleShift << 2;
  /* asset + first source entry's dataOffset, moved half a tile down and right */
  g_GraphicsShadingGeneratedTexturePixelCursor =
       (uint8_t *)g_GraphicsShadingGeneratedAsset +
       ((g_GraphicsShadingTextureDimension + 1) * g_GraphicsShadingGridHalfSize >> 1) +
       (int)((GraphicsTextureSourceEntry *)
             ((uint8_t *)g_GraphicsShadingGeneratedAsset +
              (g_GraphicsShadingGeneratedAsset->tableDescriptor).subresourceTableOffset))->dataOffset;
  g_GraphicsShadingGeneratedTextureTileX = 0;
  g_GraphicsShadingGeneratedTextureTileY = 0;
  g_GraphicsShadingGeneratedTextureSubresourceIndex = 0;
  g_GraphicsShadingGeneratedTextureTileXQ20 = 0;
  g_GraphicsShadingGeneratedTextureTileYQ20 = 0;
  g_GraphicsShadingGeneratedTextureCompletedTraversalCount = 0;
  tableOffset = (g_GraphicsShadingGeneratedAsset->tableDescriptor).subresourceTableOffset;
  /* pixels follow the 0x20-byte source entries; size = count * width * height of the first entry */
  alphaCursor = (uint8_t *)g_GraphicsShadingGeneratedAsset +
                g_GraphicsShadingSubresourceCount * GFX_SUBRESOURCE_RECORD_SIZE + tableOffset;
  for (dwordsRemaining = g_GraphicsShadingSubresourceCount *
               ((GraphicsTextureSourceEntry *)((uint8_t *)g_GraphicsShadingGeneratedAsset + tableOffset))->pixelWidth *
               ((GraphicsTextureSourceEntry *)((uint8_t *)g_GraphicsShadingGeneratedAsset +
                                               tableOffset))->pixelHeight >> 2; dwordsRemaining != 0;
      dwordsRemaining--) {
    alphaCursor[0] = 0;
    alphaCursor[1] = 0;
    alphaCursor[2] = 0;
    alphaCursor[3] = 0;
    alphaCursor = alphaCursor + 4;
  }
  return;
}


/* Ends a shadow pass (frontend world render in src/ui/frontend/menu_room.cpp): uploads the alpha of every
   generated shadow texture the pass filled, i.e. all subresources before the current one plus the current
   one when it has at least one used tile (and not every tile ran out).
*/
void GraphicsShadingGeneratedTexture_RefreshTouchedAlphaSubresources()

{
  uint32_t subresourceIndex;
  GraphicsSubresourceIndex subresourcesRemaining;

  subresourceIndex = 0;
  for (subresourcesRemaining = g_GraphicsShadingGeneratedTextureSubresourceIndex; subresourcesRemaining != 0;
       subresourcesRemaining--) {
    g_GraphicsRefreshTextureAlpha(subresourceIndex,g_GraphicsShadingTextureSet);
    subresourceIndex++;
  }
  if (g_GraphicsShadingGeneratedTextureCompletedTraversalCount == 0 &&
      (g_GraphicsShadingGeneratedTextureTileX != 0 || g_GraphicsShadingGeneratedTextureTileY != 0)) {
    g_GraphicsRefreshTextureAlpha(subresourceIndex,g_GraphicsShadingTextureSet);
  }
  return;
}


/* Shadow silhouette pass for a mesh record without MODEL_MESH_SOFT_SHADOW (ModelMeshHeader.flags), called per mesh record by
   GraphicsShadingGeneratedTexture_RasterizeHardShadowHierarchy after the blur: projects every vertex into the
   current shadow tile (quantized to whole texels) and fills every triangle with 0xFF. A mesh record is a
   ModelMeshHeader, then 0x40-byte vertices (position at +0, projected XY stored at +0x20) followed by
   0x40-byte triangles (GraphicsTriangleInput vertex pointers).
*/
void GraphicsShadingGeneratedTexture_RasterizeHardShadowMesh(ModelMeshGroupAddress32 meshRecord)

{
  int vertexCount;
  int triangleCount;
  uint8_t *recordCursor;
  GraphicsTriangleInput *triangle;

  vertexCount = ((ModelMeshHeader *)meshRecord)->vertexCount;
  triangleCount = ((ModelMeshHeader *)meshRecord)->triangleCount;
  if ((((ModelMeshHeader *)meshRecord)->flags & MODEL_MESH_SOFT_SHADOW) != 0 || vertexCount == 0) {
    return;
  }
  recordCursor = (uint8_t *)meshRecord + sizeof(ModelMeshHeader);
  for (; vertexCount != 0; vertexCount--) {
    GraphicsShadingGeneratedTexture_TransformPointXYQuantized
              ((GraphicsFixedVec2 *)(recordCursor + MODEL_MESH_VERTEX_SHADOW_XY_OFFSET),
               (GraphicsFixedVec3 *)recordCursor,
               &g_GeneratedTextureScratchRuntime.modelToGeneratedTextureTransform);
    recordCursor = recordCursor + MODEL_MESH_RECORD_SIZE;
  }
  for (; triangleCount != 0; triangleCount--) {
    /* 5f-format: GraphicsTriangleInput.vertex0/vertex1/vertex2 (MDL mesh triangle record) */
    triangle = (GraphicsTriangleInput *)recordCursor;
    GraphicsShadingGeneratedTexture_RasterizeTriangleMask
              ((GraphicsFixedVec2 *)((uint8_t *)triangle->vertex2 + MODEL_MESH_VERTEX_SHADOW_XY_OFFSET),
               (GraphicsFixedVec2 *)((uint8_t *)triangle->vertex1 + MODEL_MESH_VERTEX_SHADOW_XY_OFFSET),
               (GraphicsFixedVec2 *)((uint8_t *)triangle->vertex0 + MODEL_MESH_VERTEX_SHADOW_XY_OFFSET));
    recordCursor = recordCursor + MODEL_MESH_RECORD_SIZE;
  }
  return;
}


/* Second silhouette pass of GraphicsShadingGeneratedTexture_ProcessRenderableHierarchy (after the blur): for
   the node and, recursively, all its children builds the node-to-shadow-tile transform (world transform taken
   relative to the shadow origin, composed with the generated texture basis) and rasterizes the node's mesh
   records without MODEL_MESH_SOFT_SHADOW.
*/
void GraphicsShadingGeneratedTexture_RasterizeHardShadowHierarchy(ModelRuntimeNode *modelNode)

{
  GraphicsFixedVec3 *nodeTranslation;
  ModelResource *resourceView;
  GraphicsWorldCoordinateQ12 originX;
  GraphicsWorldCoordinateQ12 originY;
  GraphicsWorldCoordinateQ12 originZ;
  int meshGroupOffset;
  int meshesRemaining;
  int childIndex;
  uint32_t childrenRemaining;
  uint8_t *meshRecord;

  originZ = g_GeneratedTextureScratchRuntime.currentModelOriginQ12.z;
  originY = g_GeneratedTextureScratchRuntime.currentModelOriginQ12.y;
  originX = g_GeneratedTextureScratchRuntime.currentModelOriginQ12.x;
  /* the node translation is made relative to the shadow origin only for the compose, then restored */
  nodeTranslation = &(modelNode->worldTransform).translation;
  nodeTranslation->x = nodeTranslation->x - originX;
  nodeTranslation->y = nodeTranslation->y - originY;
  nodeTranslation->z = nodeTranslation->z - originZ;
  GraphicsShadingGeneratedTexture_ComposeTransform
            (&g_GeneratedTextureScratchRuntime.modelToGeneratedTextureTransform,
             &modelNode->worldTransform,
             &g_GeneratedTextureScratchRuntime.generatedTextureBasisTransform);
  nodeTranslation->x = nodeTranslation->x + originX;
  nodeTranslation->y = nodeTranslation->y + originY;
  nodeTranslation->z = nodeTranslation->z + originZ;
  resourceView = (modelNode->modelPayload).modelResource;
  /* the shadow mesh group: a ModelMeshGroupHeader, then the mesh records from +0x20, each starting with its
     own size */
  meshGroupOffset = resourceView->shadowMeshGroupOffset;
  if (meshGroupOffset != 0) {
    meshRecord = (uint8_t *)resourceView + meshGroupOffset + sizeof(ModelMeshGroupHeader);
    for (meshesRemaining = ((ModelMeshGroupHeader *)((uint8_t *)resourceView + meshGroupOffset))->meshCount;
         meshesRemaining != 0; meshesRemaining--) {
      GraphicsShadingGeneratedTexture_RasterizeHardShadowMesh
                ((ModelMeshGroupAddress32)meshRecord);
      meshRecord = meshRecord + ((ModelMeshHeader *)meshRecord)->byteSize;
    }
  }
  childIndex = 0;
  for (childrenRemaining = modelNode->childCount; childrenRemaining != 0; childrenRemaining--) {
    if (modelNode->childNodes[childIndex] != nullptr) {
      GraphicsShadingGeneratedTexture_RasterizeHardShadowHierarchy(modelNode->childNodes[childIndex]);
    }
    childIndex++;
  }
  return;
}


/* Counterpart of GraphicsShadingGeneratedTexture_RasterizeHardShadowMesh for mesh records with
   MODEL_MESH_SOFT_SHADOW (the parts that get the soft, filtered shadow); called per mesh record by
   GraphicsShadingGeneratedTexture_RasterizeSoftShadowHierarchy. Returns 0 when nothing was rasterized, 1
   when triangles were, otherwise the address of the last transformed vertex's shadow XY (what the original
   leaves as its result when the batch has vertices but no triangles).
*/
uintptr_t
GraphicsShadingGeneratedTexture_RasterizeSoftShadowMesh(ModelMeshGroupAddress32 meshRecord)

{
  int vertexCount;
  int triangleCount;
  GraphicsFixedVec3 *recordCursor;
  uintptr_t result;

  result = 0;
  vertexCount = ((ModelMeshHeader *)meshRecord)->vertexCount;
  triangleCount = ((ModelMeshHeader *)meshRecord)->triangleCount;
  if (((((ModelMeshHeader *)meshRecord)->flags & MODEL_MESH_SOFT_SHADOW) != 0) &&
     (recordCursor = (GraphicsFixedVec3 *)(meshRecord + sizeof(ModelMeshHeader)), vertexCount != 0)) {
    do {
      result = (uintptr_t)&recordCursor[2].z;
      GraphicsShadingGeneratedTexture_TransformPointXYQuantized
                ((GraphicsFixedVec2 *)&recordCursor[2].z,recordCursor,
                 &g_GeneratedTextureScratchRuntime.modelToGeneratedTextureTransform);
      recordCursor = (GraphicsFixedVec3 *)((uint8_t *)recordCursor + MODEL_MESH_RECORD_SIZE);
      vertexCount--;
    } while (vertexCount != 0);
    if (triangleCount != 0) {
      for (; triangleCount != 0; triangleCount--) {
        /* 5f-format: GraphicsTriangleInput.vertex0/vertex1/vertex2 (MDL mesh triangle record, 32-bit vertex
           addresses; vertex0 read as recordCursor->x) */
        GraphicsShadingGeneratedTexture_RasterizeTriangleMask
                  (Thandor_U32ToPointer<GraphicsFixedVec2>((int)((GraphicsTriangleInput *)recordCursor)->vertex2 + MODEL_MESH_VERTEX_SHADOW_XY_OFFSET),
                   Thandor_U32ToPointer<GraphicsFixedVec2>((int)((GraphicsTriangleInput *)recordCursor)->vertex1 + MODEL_MESH_VERTEX_SHADOW_XY_OFFSET),
                   Thandor_U32ToPointer<GraphicsFixedVec2>(recordCursor->x + MODEL_MESH_VERTEX_SHADOW_XY_OFFSET));
        recordCursor = (GraphicsFixedVec3 *)((uint8_t *)recordCursor + MODEL_MESH_RECORD_SIZE);
      }
      result = 1;
    }
  }
  return result;
}


/* First silhouette pass of GraphicsShadingGeneratedTexture_ProcessRenderableHierarchy: like
   GraphicsShadingGeneratedTexture_RasterizeHardShadowHierarchy, but rasterizes the mesh records with
   MODEL_MESH_SOFT_SHADOW. Returns a value the caller tests before filtering the generated texture: the last
   mesh record's result plus the shadow mesh-group offset, plus the children's results. Nonzero whenever the
   hierarchy has a mesh group.
*/
uintptr_t
GraphicsShadingGeneratedTexture_RasterizeSoftShadowHierarchy(ModelRuntimeNode *modelNode)

{
  uintptr_t result;
  GraphicsFixedVec3 *nodeTranslation;
  ModelResource *resourceView;
  GraphicsWorldCoordinateQ12 originX;
  GraphicsWorldCoordinateQ12 originY;
  GraphicsWorldCoordinateQ12 originZ;
  int meshGroupOffset;
  int meshesRemaining;
  int childIndex;
  uint32_t childrenRemaining;
  uint8_t *meshRecord;

  originZ = g_GeneratedTextureScratchRuntime.currentModelOriginQ12.z;
  originY = g_GeneratedTextureScratchRuntime.currentModelOriginQ12.y;
  originX = g_GeneratedTextureScratchRuntime.currentModelOriginQ12.x;
  /* the node translation is made relative to the shadow origin only for the compose, then restored */
  nodeTranslation = &(modelNode->worldTransform).translation;
  nodeTranslation->x = nodeTranslation->x - originX;
  nodeTranslation->y = nodeTranslation->y - originY;
  nodeTranslation->z = nodeTranslation->z - originZ;
  GraphicsShadingGeneratedTexture_ComposeTransform
            (&g_GeneratedTextureScratchRuntime.modelToGeneratedTextureTransform,
             &modelNode->worldTransform,
             &g_GeneratedTextureScratchRuntime.generatedTextureBasisTransform);
  nodeTranslation->x = nodeTranslation->x + originX;
  nodeTranslation->y = nodeTranslation->y + originY;
  nodeTranslation->z = nodeTranslation->z + originZ;
  result = 0;
  resourceView = (modelNode->modelPayload).modelResource;
  /* the shadow mesh group: a ModelMeshGroupHeader, then the mesh records from +0x20, each starting with its
     own size */
  meshGroupOffset = resourceView->shadowMeshGroupOffset;
  if (meshGroupOffset != 0) {
    meshRecord = (uint8_t *)resourceView + meshGroupOffset + sizeof(ModelMeshGroupHeader);
    for (meshesRemaining = ((ModelMeshGroupHeader *)((uint8_t *)resourceView + meshGroupOffset))->meshCount;
         meshesRemaining != 0; meshesRemaining--) {
      /* Original quirk: each mesh overwrites the result (it does not accumulate) and the mesh-group offset is
         added to it; the caller only tests the sum for nonzero. */
      result = GraphicsShadingGeneratedTexture_RasterizeSoftShadowMesh((ModelMeshGroupAddress32)meshRecord) +
               meshGroupOffset;
      meshRecord = meshRecord + ((ModelMeshHeader *)meshRecord)->byteSize;
    }
  }
  childIndex = 0;
  for (childrenRemaining = modelNode->childCount; childrenRemaining != 0; childrenRemaining--) {
    if (modelNode->childNodes[childIndex] != nullptr) {
      result = result + GraphicsShadingGeneratedTexture_RasterizeSoftShadowHierarchy(modelNode->childNodes[childIndex]);
    }
    childIndex++;
  }
  return result;
}


/* Projects every vertex of one mesh record with the current node transform (light-space rotation, see
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

  vertexCursor = (GraphicsFixedVec3 *)(meshRecord + sizeof(ModelMeshHeader));
  for (verticesRemaining = ((ModelMeshHeader *)meshRecord)->vertexCount; verticesRemaining != 0; verticesRemaining--) {
    GraphicsShadingGeneratedTexture_TransformPointXY
              ((GraphicsFixedVec2 *)&vertexCursor[2].z,vertexCursor,
               &g_GeneratedTextureScratchRuntime.modelToGeneratedTextureTransform);
    vertexX = vertexCursor[2].z;
    vertexY = vertexCursor[3].x;
    vertexCursor = (GraphicsFixedVec3 *)((uint8_t *)vertexCursor + MODEL_MESH_RECORD_SIZE);
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


/* First step of GraphicsShadingGeneratedTexture_ProcessRenderableHierarchy: for the node and, recursively,
   all its children composes the node transform (relative to the shadow origin) with
   g_AuxiliaryRotationMatrixFixed (the light-space rotation) and accumulates the projected bounds of all mesh
   records, i.e. the extent of the model's shadow before it is scaled into a texture tile.
*/
void GraphicsShadingGeneratedTexture_TraverseHierarchyAndAccumulateProjectedBounds(ModelRuntimeNode *modelNode)

{
  GraphicsFixedVec3 *nodeTranslation;
  ModelResource *resourceView;
  GraphicsWorldCoordinateQ12 originX;
  GraphicsWorldCoordinateQ12 originY;
  GraphicsWorldCoordinateQ12 originZ;
  int meshGroupOffset;
  int meshesRemaining;
  int childIndex;
  uint32_t childrenRemaining;
  uint8_t *meshRecord;

  originZ = g_GeneratedTextureScratchRuntime.currentModelOriginQ12.z;
  originY = g_GeneratedTextureScratchRuntime.currentModelOriginQ12.y;
  originX = g_GeneratedTextureScratchRuntime.currentModelOriginQ12.x;
  /* the node translation is made relative to the shadow origin only for the compose, then restored */
  nodeTranslation = &(modelNode->worldTransform).translation;
  nodeTranslation->x = nodeTranslation->x - originX;
  nodeTranslation->y = nodeTranslation->y - originY;
  nodeTranslation->z = nodeTranslation->z - originZ;
  FixedTransform_Compose
            (&g_GeneratedTextureScratchRuntime.modelToGeneratedTextureTransform,
             &modelNode->worldTransform,&g_AuxiliaryRotationMatrixFixed);
  nodeTranslation->x = nodeTranslation->x + originX;
  nodeTranslation->y = nodeTranslation->y + originY;
  nodeTranslation->z = nodeTranslation->z + originZ;
  resourceView = (modelNode->modelPayload).modelResource;
  meshGroupOffset = resourceView->shadowMeshGroupOffset;
  if (meshGroupOffset != 0) {
    meshRecord = (uint8_t *)resourceView + meshGroupOffset + sizeof(ModelMeshGroupHeader);
    for (meshesRemaining = ((ModelMeshGroupHeader *)((uint8_t *)resourceView + meshGroupOffset))->meshCount;
         meshesRemaining != 0; meshesRemaining--) {
      GraphicsShadingGeneratedTexture_AccumulateProjectedBoundsFromRecords
                ((ModelMeshGroupAddress32)meshRecord);
      meshRecord = meshRecord + ((ModelMeshHeader *)meshRecord)->byteSize;
    }
  }
  childIndex = 0;
  for (childrenRemaining = modelNode->childCount; childrenRemaining != 0; childrenRemaining--) {
    if (modelNode->childNodes[childIndex] != nullptr) {
      GraphicsShadingGeneratedTexture_TraverseHierarchyAndAccumulateProjectedBounds
                (modelNode->childNodes[childIndex]);
    }
    childIndex++;
  }
  return;
}


/* Transforms point by the first two rows of transform (Q28 basis, 64-bit dot products shifted right by 28)
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
       (FIXED_PRODUCT_SHR(dotProduct, Q28_SHIFT)) + (transform->translation).x;
  dotProduct = (int64_t)transform->basisRow1[1] * (int64_t)point->y +
          (int64_t)row1Column0 * (int64_t)point->x +
          (int64_t)transform->basisRow1[2] * (int64_t)point->z;
  outputXY->component1 =
       (FIXED_PRODUCT_SHR(dotProduct, Q28_SHIFT)) + (transform->translation).y;
  return;
}


/* Moves the shadow tile cursor to the next gridHalfSize x gridHalfSize tile after a model's shadow was
   drawn (end of GraphicsShadingGeneratedTexture_ProcessRenderableHierarchy): left to right, then down a
   tile row, then on to the next generated texture. When the last texture is full the completed count
   becomes nonzero and further models get no shadow until the next pass.
*/
void GraphicsShadingGeneratedTexture_AdvanceTileCursor()

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


/* The 8 bytes at byteOffset from a scratch grid position. */
static uint64_t ShadingFilter_LoadQuad(const uint8_t *scratchPosition,int byteOffset)
{
  return *(const uint64_t *)(scratchPosition + byteOffset);
}

/* Filter value of the 8 scratch texels at center (each 0..7), byte-saturated: centre x4, the four direct taps
   (left/right tapStep texels, up/down tapRowStride bytes) x3 through one cross sum added three times, and the
   outer taps x1. The additions keep the original order. */
static uint64_t ShadingFilter_WeightedNeighbourhoodSum(const uint8_t *center,int tapStep,int tapRowStride)
{
  uint64_t sum;
  uint64_t crossSum;

  sum = paddusb(ShadingFilter_LoadQuad(center,0) << 2,ShadingFilter_LoadQuad(center,tapStep * 2));
  crossSum = paddusb(ShadingFilter_LoadQuad(center,tapStep),ShadingFilter_LoadQuad(center,-tapStep));
  sum = paddusb(sum,ShadingFilter_LoadQuad(center,-tapStep * 2));
  /* row above */
  crossSum = paddusb(crossSum,ShadingFilter_LoadQuad(center,-tapRowStride));
  sum = paddusb(sum,ShadingFilter_LoadQuad(center,-tapRowStride + tapStep));
  sum = paddusb(sum,ShadingFilter_LoadQuad(center,-tapRowStride + tapStep * 2));
  sum = paddusb(sum,ShadingFilter_LoadQuad(center,-tapRowStride - tapStep));
  sum = paddusb(sum,ShadingFilter_LoadQuad(center,-tapRowStride - tapStep * 2));
  /* row below */
  crossSum = paddusb(crossSum,ShadingFilter_LoadQuad(center,tapRowStride));
  sum = paddusb(sum,ShadingFilter_LoadQuad(center,tapRowStride + tapStep));
  sum = paddusb(sum,ShadingFilter_LoadQuad(center,tapRowStride + tapStep * 2));
  sum = paddusb(sum,ShadingFilter_LoadQuad(center,tapRowStride - tapStep));
  sum = paddusb(sum,ShadingFilter_LoadQuad(center,tapRowStride - tapStep * 2));
  sum = paddusb(sum,crossSum);
  sum = paddusb(sum,crossSum);
  sum = paddusb(sum,crossSum);
  /* two rows above and below */
  sum = paddusb(sum,ShadingFilter_LoadQuad(center,-tapRowStride * 2));
  sum = paddusb(sum,ShadingFilter_LoadQuad(center,-tapRowStride * 2 + tapStep));
  sum = paddusb(sum,ShadingFilter_LoadQuad(center,tapRowStride * 2));
  sum = paddusb(sum,ShadingFilter_LoadQuad(center,-tapRowStride * 2 - tapStep));
  sum = paddusb(sum,ShadingFilter_LoadQuad(center,tapRowStride * 2 - tapStep));
  return paddusb(sum,ShadingFilter_LoadQuad(center,tapRowStride * 2 + tapStep));
}


/* Softens the shadow in the current tile (GraphicsShadingGeneratedTexture_ProcessRenderableHierarchy calls it
   after the soft-shadow silhouette pass drew something): copies the tile's texels, reduced to 0..7 (>> 5), into
   the zero-bordered scratch grid, then writes back to every texel the byte-saturated weighted sum of its
   neighbourhood (centre x4, taps gridHalfSize / 16 texels apart), 32 texels per step with MMX.
*/
void GraphicsShadingGeneratedTexture_FilterGridScratchMmx()

{
  uint64_t threeBitMask;
  int tapStep;
  int tapRowStride;
  uint32_t blocksPerRow;
  uint32_t blocksRemaining;
  uint32_t rowsRemaining;
  int quadIndex;
  uint8_t *tileTopLeft;
  uint8_t *textureCursor;
  uint8_t *scratchCursor;

  threeBitMask = g_GraphicsShadingMmxPacked3BitPerByteMask;
  /* horizontal tap distance in texels; tapRowStride is the same distance vertically (scratch rows are
     2 * gridHalfSize bytes apart) */
  tapStep = (int)(g_GraphicsShadingGridHalfSize >> 4);
  /* 32 texels per step; a row always takes at least one step, and a partial last step is done whole */
  if (g_GraphicsShadingGridHalfSize == 0) {
    blocksPerRow = 1;
  }
  else {
    blocksPerRow = (g_GraphicsShadingGridHalfSize - 1) / 32 + 1;
  }
  /* top-left texel of the tile (the pixel cursor points at its centre) */
  tileTopLeft = g_GraphicsShadingGeneratedTexturePixelCursor +
                (int32_t)(-(g_GraphicsShadingGridHalfSize >> 1) -
                          g_GraphicsShadingTextureDimension * (g_GraphicsShadingGridHalfSize >> 1));
  /* pass 1: tile -> scratch, each texel reduced to 3 bits */
  textureCursor = tileTopLeft;
  scratchCursor = (uint8_t *)g_GraphicsShadingGridScratchInterior;
  /* Original quirk: both passes test the row count at the end, so gridHalfSize 0 would wrap around */
  rowsRemaining = g_GraphicsShadingGridHalfSize;
  do {
    for (blocksRemaining = blocksPerRow; blocksRemaining != 0; blocksRemaining--) {
      for (quadIndex = 0; quadIndex < 4; quadIndex++) {
        ((uint64_t *)scratchCursor)[quadIndex] = ((uint64_t *)textureCursor)[quadIndex] >> 5 & threeBitMask;
      }
      textureCursor = textureCursor + 32;
      scratchCursor = scratchCursor + 32;
    }
    scratchCursor = scratchCursor + g_GraphicsShadingGridHalfSize;
    textureCursor = textureCursor + (g_GraphicsShadingTextureDimension - g_GraphicsShadingGridHalfSize);
    rowsRemaining--;
  } while (rowsRemaining != 0);
  /* pass 2: weighted neighbourhood sums from the scratch grid back into the tile */
  tapRowStride = (int)(g_GraphicsShadingGridHalfSize * 2 * (uint32_t)tapStep);
  textureCursor = tileTopLeft;
  scratchCursor = (uint8_t *)g_GraphicsShadingGridScratchInterior;
  rowsRemaining = g_GraphicsShadingGridHalfSize;
  do {
    for (blocksRemaining = blocksPerRow; blocksRemaining != 0; blocksRemaining--) {
      for (quadIndex = 0; quadIndex < 4; quadIndex++) {
        ((uint64_t *)textureCursor)[quadIndex] =
             ShadingFilter_WeightedNeighbourhoodSum(scratchCursor + quadIndex * 8,tapStep,tapRowStride);
      }
      textureCursor = textureCursor + 32;
      scratchCursor = scratchCursor + 32;
    }
    scratchCursor = scratchCursor + g_GraphicsShadingGridHalfSize;
    textureCursor = textureCursor + (g_GraphicsShadingTextureDimension - g_GraphicsShadingGridHalfSize);
    rowsRemaining--;
  } while (rowsRemaining != 0);
  return;
}


/* Returns true when neither the node's model resource nor any descendant has a mesh
   group (ModelResource.shadowMeshGroupOffset), so GraphicsShadingGeneratedTexture_ProcessRenderableHierarchy skips hierarchies
   that cannot cast a shadow. Children are probed from the last to the first; the first hit ends the search.
*/
Bool8 GraphicsShadingGeneratedTexture_ProbeHierarchyForGeometry(ModelRuntimeNode *modelNode)

{
  int childIndex;

  if (((modelNode->modelPayload).modelResource)->shadowMeshGroupOffset != 0) {
    return false;
  }
  /* the count is taken as signed, so a count of 0x80000001 or more probes no child */
  for (childIndex = (int)(modelNode->childCount - 1); childIndex >= 0; childIndex--) {
    if (modelNode->childNodes[childIndex] != nullptr &&
        !GraphicsShadingGeneratedTexture_ProbeHierarchyForGeometry(modelNode->childNodes[childIndex])) {
      return false;
    }
  }
  return true;
}


/* Reserves the 14 consecutive 0x80-byte primitive blocks of one shadow patch from the render context's
   projected point pool (GraphicsShadingGeneratedTexture_ProcessRenderableHierarchy): records each block's
   address in the pool's block table and presets the first block's 0x20-byte header at +0x60 (0, flags
   0x11000 = textured, translucent); the three 0x20-byte vertices of each triangle come first. Returns the
   first block, or NULL when the pool is full (a reserved block is never NULL: it lies in the pool storage).
*/
GraphicsProjectedPointPair *GraphicsShadingGeneratedTexture_ReserveFourteenProjectedPointBlocks
          (GeneratedTextureRenderContextView *renderContext)

{
  GraphicsPrimitiveQueue *blockPool;
  uint32_t usedBlockCount;
  uint32_t newBlockCount;
  uint32_t blockIndex;
  GraphicsProjectedPointPair *firstBlock;
  GraphicsPrimitivePacket *firstPacket;

  /* the pool is the active primitive queue: count, capacity, packetPool and the primary nodes' packets */
  blockPool = renderContext->projectedPointBlockPool;
  usedBlockCount = blockPool->count;
  newBlockCount = usedBlockCount + 14;
  if (newBlockCount < blockPool->capacity) {
    blockPool->count = newBlockCount;
    firstBlock = (GraphicsProjectedPointPair *)
                 ((uint8_t *)blockPool->packetPool + usedBlockCount * GRAPHICS_PROJECTED_BLOCK_BYTES);
    for (blockIndex = 0; blockIndex < 14; blockIndex++) {
      blockPool->primaryNodes[usedBlockCount + blockIndex].packet =
           (GraphicsPrimitivePacket *)((uint8_t *)firstBlock + blockIndex * GRAPHICS_PROJECTED_BLOCK_BYTES);
    }
    /* the first block's packet header: modulation colour 0, flags textured + translucent */
    firstPacket = (GraphicsPrimitivePacket *)firstBlock;
    firstPacket->modulationColor = 0;
    firstPacket->renderFlags = GRAPHICS_PRIMITIVE_FLAG_TEXTURED | GRAPHICS_PRIMITIVE_BLEND_TRANSLUCENT;
    return firstBlock;
  }
  return nullptr;
}


/* Gives back the 14 blocks of GraphicsShadingGeneratedTexture_ReserveFourteenProjectedPointBlocks when
   GraphicsShadingGeneratedTexture_ProcessRenderableHierarchy finds a shadow point with a view depth below
   g_ProjectionScaleFixed (not projectable).
*/
void GraphicsShadingGeneratedTexture_RollbackFourteenProjectedPointBlocks
               (GeneratedTextureRenderContextView *renderContext)

{
  renderContext->projectedPointBlockPool->count = renderContext->projectedPointBlockPool->count - 14;
  return;
}


/* Like GraphicsShadingGeneratedTexture_TransformPointXY, but for the composed node-to-tile transform: the
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
       (FIXED_PRODUCT_SHR(dotProduct, Q12_SHIFT)) + (transform->translation).x &
       ~(uint32_t)Q12_FRACTION_MASK;
  dotProduct = (int64_t)transform->basisRow1[1] * (int64_t)point->y +
          (int64_t)row1Column0 * (int64_t)point->x +
          (int64_t)transform->basisRow1[2] * (int64_t)point->z;
  outputXY->component1 =
       (FIXED_PRODUCT_SHR(dotProduct, Q12_SHIFT)) + (transform->translation).y &
       ~(uint32_t)Q12_FRACTION_MASK;
  return;
}


/* outTransform = lhsTransform * rhsTransform for the shadow silhouette passes (lhs = generated texture basis,
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
  outTransform->basisRow0[0] = FIXED_PRODUCT_SHR(dotProduct, Q28_SHIFT);
  dotProduct = (int64_t)lhsTransform->basisRow0[1] * (int64_t)rhsTransform->basisRow1[1] +
          (int64_t)lhsElement * (int64_t)rhsTransform->basisRow0[1] +
          (int64_t)lhsTransform->basisRow0[2] * (int64_t)rhsTransform->basisRow2[1];
  lhsElement = lhsTransform->basisRow0[0];
  outTransform->basisRow0[1] = FIXED_PRODUCT_SHR(dotProduct, Q28_SHIFT);
  dotProduct = (int64_t)lhsTransform->basisRow0[1] * (int64_t)rhsTransform->basisRow1[2] +
          (int64_t)lhsElement * (int64_t)rhsTransform->basisRow0[2] +
          (int64_t)lhsTransform->basisRow0[2] * (int64_t)rhsTransform->basisRow2[2];
  lhsElement = lhsTransform->basisRow0[0];
  outTransform->basisRow0[2] = FIXED_PRODUCT_SHR(dotProduct, Q28_SHIFT);
  dotProduct = (int64_t)lhsTransform->basisRow0[1] * (int64_t)(rhsTransform->translation).y +
          (int64_t)lhsElement * (int64_t)(rhsTransform->translation).x +
          (int64_t)lhsTransform->basisRow0[2] * (int64_t)(rhsTransform->translation).z;
  lhsElement = lhsTransform->basisRow1[0];
  (outTransform->translation).x =
       (FIXED_PRODUCT_SHR(dotProduct, Q12_SHIFT)) +
       (lhsTransform->translation).x;
  dotProduct = (int64_t)lhsTransform->basisRow1[1] * (int64_t)rhsTransform->basisRow1[0] +
          (int64_t)lhsElement * (int64_t)rhsTransform->basisRow0[0] +
          (int64_t)lhsTransform->basisRow1[2] * (int64_t)rhsTransform->basisRow2[0];
  lhsElement = lhsTransform->basisRow1[0];
  outTransform->basisRow1[0] = FIXED_PRODUCT_SHR(dotProduct, Q28_SHIFT);
  dotProduct = (int64_t)lhsTransform->basisRow1[1] * (int64_t)rhsTransform->basisRow1[1] +
          (int64_t)lhsElement * (int64_t)rhsTransform->basisRow0[1] +
          (int64_t)lhsTransform->basisRow1[2] * (int64_t)rhsTransform->basisRow2[1];
  lhsElement = lhsTransform->basisRow1[0];
  outTransform->basisRow1[1] = FIXED_PRODUCT_SHR(dotProduct, Q28_SHIFT);
  dotProduct = (int64_t)lhsTransform->basisRow1[1] * (int64_t)rhsTransform->basisRow1[2] +
          (int64_t)lhsElement * (int64_t)rhsTransform->basisRow0[2] +
          (int64_t)lhsTransform->basisRow1[2] * (int64_t)rhsTransform->basisRow2[2];
  lhsElement = lhsTransform->basisRow1[0];
  outTransform->basisRow1[2] = FIXED_PRODUCT_SHR(dotProduct, Q28_SHIFT);
  dotProduct = (int64_t)lhsTransform->basisRow1[1] * (int64_t)(rhsTransform->translation).y +
          (int64_t)lhsElement * (int64_t)(rhsTransform->translation).x +
          (int64_t)lhsTransform->basisRow1[2] * (int64_t)(rhsTransform->translation).z;
  lhsElement = lhsTransform->basisRow2[0];
  (outTransform->translation).y =
       (FIXED_PRODUCT_SHR(dotProduct, Q12_SHIFT)) +
       (lhsTransform->translation).y;
  dotProduct = (int64_t)lhsTransform->basisRow2[1] * (int64_t)rhsTransform->basisRow1[0] +
          (int64_t)lhsElement * (int64_t)rhsTransform->basisRow0[0] +
          (int64_t)lhsTransform->basisRow2[2] * (int64_t)rhsTransform->basisRow2[0];
  lhsElement = lhsTransform->basisRow2[0];
  outTransform->basisRow2[0] = FIXED_PRODUCT_SHR(dotProduct, Q28_SHIFT);
  dotProduct = (int64_t)lhsTransform->basisRow2[1] * (int64_t)rhsTransform->basisRow1[1] +
          (int64_t)lhsElement * (int64_t)rhsTransform->basisRow0[1] +
          (int64_t)lhsTransform->basisRow2[2] * (int64_t)rhsTransform->basisRow2[1];
  lhsElement = lhsTransform->basisRow2[0];
  outTransform->basisRow2[1] = FIXED_PRODUCT_SHR(dotProduct, Q28_SHIFT);
  dotProduct = (int64_t)lhsTransform->basisRow2[1] * (int64_t)rhsTransform->basisRow1[2] +
          (int64_t)lhsElement * (int64_t)rhsTransform->basisRow0[2] +
          (int64_t)lhsTransform->basisRow2[2] * (int64_t)rhsTransform->basisRow2[2];
  lhsElement = lhsTransform->basisRow2[0];
  outTransform->basisRow2[2] = FIXED_PRODUCT_SHR(dotProduct, Q28_SHIFT);
  dotProduct = (int64_t)lhsTransform->basisRow2[1] * (int64_t)(rhsTransform->translation).y +
          (int64_t)lhsElement * (int64_t)(rhsTransform->translation).x +
          (int64_t)lhsTransform->basisRow2[2] * (int64_t)(rhsTransform->translation).z;
  (outTransform->translation).z =
       (FIXED_PRODUCT_SHR(dotProduct, Q12_SHIFT)) +
       (lhsTransform->translation).z;
  return;
}


/* x (Q12 texels) clamped to the current shadow tile, +/- the grid origin. */
static int32_t ShadingRaster_ClampToTile(int32_t x)
{
  if (x < g_GraphicsShadingNegativeGridOriginQ12) {
    return g_GraphicsShadingNegativeGridOriginQ12;
  }
  if (g_GraphicsShadingPositiveGridOriginQ12 < x) {
    return g_GraphicsShadingPositiveGridOriginQ12;
  }
  return x;
}

/* Sets the texels of one row between edgeX and otherEdgeX (whole texels; the lower end included, the higher
   end excluded) to 0xFF. */
static void ShadingRaster_FillSpan(uint8_t *rowPixels,int32_t edgeX,int otherEdgeX)
{
  uint8_t *spanPixel;
  int spanDelta;
  int spanRemaining;

  spanPixel = rowPixels + (edgeX >> Q12_SHIFT);
  spanDelta = (otherEdgeX >> Q12_SHIFT) - (edgeX >> Q12_SHIFT);
  if (spanDelta == 0) {
    return;
  }
  spanRemaining = spanDelta;
  if (spanDelta < 0) {
    spanRemaining = -spanDelta;
    spanPixel = spanPixel + spanDelta;
  }
  for (; spanRemaining != 0; spanRemaining--) {
    *spanPixel = ARGB8888_CHANNEL_MAX;
    spanPixel++;
  }
}

/* Fills one projected triangle (tile-relative Q12 texel coordinates, from
   GraphicsShadingGeneratedTexture_TransformPointXYQuantized) with 0xFF in the current shadow tile: sorts the
   vertices by Y, clamps Y and the vertex X values to the tile (+/- the grid origin), then draws horizontal
   spans between the long top-to-bottom edge and the two short edges. Called per triangle by the silhouette
   passes (GraphicsShadingGeneratedTexture_RasterizeSoftShadowMesh/HardShadowMesh).
*/
void GraphicsShadingGeneratedTexture_RasterizeTriangleMask
          (GraphicsFixedVec2 *vertexA,GraphicsFixedVec2 *vertexB,GraphicsFixedVec2 *vertexC)

{
  GraphicsFixedVec2 *topVertex;
  GraphicsFixedVec2 *middleVertex;
  GraphicsFixedVec2 *bottomVertex;
  GraphicsFixedVec2 *swapVertex;
  int topY;
  int middleY;
  int bottomY;
  int swapY;
  int rowsRemaining;
  int upperRowsRemaining;
  int32_t topXClamped;
  int32_t middleXClamped;
  int32_t bottomXClamped;
  int upperDeltaX;
  int longDeltaX;
  int longEdgeStep;
  int upperEdgeStep;
  int lowerEdgeStep;
  int longEdgeX;
  int32_t upperEdgeX;
  int32_t lowerEdgeX;
  uint8_t *rowPixels;

  /* sort by Y (ties keep the earlier order): topVertex, middleVertex, bottomVertex */
  topVertex = vertexC;
  topY = vertexC->component1;
  middleVertex = vertexB;
  middleY = vertexB->component1;
  bottomVertex = vertexA;
  bottomY = vertexA->component1;
  if (middleY < topY) {
    swapY = topY;
    topY = middleY;
    middleY = swapY;
    topVertex = vertexB;
    middleVertex = vertexC;
  }
  if (bottomY < topY) {
    swapY = topY;
    topY = bottomY;
    bottomY = swapY;
    bottomVertex = topVertex;
    topVertex = vertexA;
  }
  if (bottomY < middleY) {
    swapY = middleY;
    middleY = bottomY;
    bottomY = swapY;
    swapVertex = middleVertex;
    middleVertex = bottomVertex;
    bottomVertex = swapVertex;
  }
  /* clamp Y to the tile; triangles completely above or below it are dropped */
  if (topY < g_GraphicsShadingNegativeGridOriginQ12) {
    if (middleY < g_GraphicsShadingNegativeGridOriginQ12) {
      middleY = g_GraphicsShadingNegativeGridOriginQ12;
    }
    topY = g_GraphicsShadingNegativeGridOriginQ12;
    if (bottomY < g_GraphicsShadingNegativeGridOriginQ12) {
      return;
    }
  }
  if (g_GraphicsShadingPositiveGridOriginQ12 < bottomY) {
    if (g_GraphicsShadingPositiveGridOriginQ12 < middleY) {
      middleY = g_GraphicsShadingPositiveGridOriginQ12;
    }
    bottomY = g_GraphicsShadingPositiveGridOriginQ12;
    if (g_GraphicsShadingPositiveGridOriginQ12 < topY) {
      return;
    }
  }
  rowsRemaining = (bottomY - topY) >> Q12_SHIFT;
  if (rowsRemaining == 0) {
    return;
  }
  upperRowsRemaining = (middleY - topY) >> Q12_SHIFT;
  topXClamped = ShadingRaster_ClampToTile(topVertex->component0);
  middleXClamped = ShadingRaster_ClampToTile(middleVertex->component0);
  bottomXClamped = ShadingRaster_ClampToTile(bottomVertex->component0);
  upperDeltaX = middleXClamped - topXClamped;
  longDeltaX = bottomXClamped - topXClamped;
  rowPixels = g_GraphicsShadingGeneratedTexturePixelCursor + (int32_t)((topY >> Q12_SHIFT) * g_GraphicsShadingTextureDimension);
  /* the long edge runs from the top to the bottom vertex over all rows */
  longEdgeStep = longDeltaX / rowsRemaining;
  longEdgeX = topXClamped;
  upperEdgeX = topXClamped;
  lowerEdgeX = middleXClamped;
  if (upperRowsRemaining != 0) {
    upperEdgeStep = upperDeltaX / upperRowsRemaining;
    for (; upperRowsRemaining != 0; upperRowsRemaining--) {
      ShadingRaster_FillSpan(rowPixels,upperEdgeX,longEdgeX);
      rowPixels = rowPixels + g_GraphicsShadingTextureDimension;
      upperEdgeX = upperEdgeX + upperEdgeStep;
      longEdgeX = longEdgeX + longEdgeStep;
      /* the original keeps the total row count in an MMX lane and decrements it with PSUBD by this {1, 0}
         constant */
      rowsRemaining = rowsRemaining - (int)g_GraphicsShadingRasterizeMmxPackedDwordOneZero;
    }
  }
  if (rowsRemaining != 0) {
    lowerEdgeStep = (longDeltaX - upperDeltaX) / rowsRemaining;
    for (; rowsRemaining != 0; rowsRemaining--) {
      ShadingRaster_FillSpan(rowPixels,lowerEdgeX,longEdgeX);
      rowPixels = rowPixels + g_GraphicsShadingTextureDimension;
      lowerEdgeX = lowerEdgeX + lowerEdgeStep;
      longEdgeX = longEdgeX + longEdgeStep;
    }
  }
  return;
}




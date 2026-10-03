/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/include/thandor/graphics/render/shading.h
 * Reverse engineering by idkFoxes 2026
 */

#ifndef THANDOR_GRAPHICS_RENDER_SHADING_H
#define THANDOR_GRAPHICS_RENDER_SHADING_H

#include <thandor/generated/types.h>
#include <thandor/core/contracts.h>

/* Submodule: graphics/render/shading. */

/* Number of runtime light records in g_GraphicsShadingRuntimeRecords (0x40 bytes each) */
#define GRAPHICS_SHADING_RUNTIME_RECORD_COUNT 256
/* Intensity clamp table (GraphicsIntensityClampTable_Initialize): entry (previous << 8) | target is target
   limited to previous +/- this step, so model tints fade by at most 21 per update */
#define GRAPHICS_INTENSITY_CLAMP_MAX_STEP 21
/* 64 KiB table plus 64 KiB slack so it can be aligned to a 64 KiB boundary */
#define GRAPHICS_INTENSITY_CLAMP_ALLOCATION_BYTES 0x20000
#define GRAPHICS_INTENSITY_CLAMP_TABLE_ALIGNMENT 0x10000
/* Generated shadow texture (GraphicsShadingGeneratedTexture_ProcessRenderableHierarchy): a shadow vertex fades
   out linearly with its ray distance to the caster and vanishes at 5.0 world units (Q12) */
#define GRAPHICS_SHADING_SHADOW_FADE_DISTANCE_Q12 0x5000
/* 1/256 grid cell in Q20: the far texture coordinate sample of a g_GraphicsShadingGridStepQ20 step is pulled
   back by this so it stays inside the step (samples at 0, step / 2 and step - this) */
#define GRAPHICS_SHADING_SAMPLE_INSET_Q20 0x1000
/* Projected point pool of the generated shadow texture: 0x80-byte primitive blocks of 16 point pairs, three
   0x20-byte vertices (4 pairs each) and a 0x20-byte header at +0x60 (pairs 12..15). A shadow patch reserves 14
   consecutive blocks (GraphicsShadingGeneratedTexture_ReserveFourteenProjectedPointBlocks). */
#define GRAPHICS_PROJECTED_BLOCK_BYTES 0x80
#define GRAPHICS_PROJECTED_BLOCK_PAIRS 16
#define GRAPHICS_SHADOW_PATCH_BLOCK_COUNT 14
/* Index of point pair `pair` of block `block` in a GraphicsProjectedPointPair array of consecutive blocks */
#define GRAPHICS_PROJECTED_PAIR(block, pair) ((block) * GRAPHICS_PROJECTED_BLOCK_PAIRS + (pair))
/* Highest light level of g_ShadingIntensityScaleMmx (256 entries) */
#define GRAPHICS_SHADING_INTENSITY_MAX 255
/* Functions are grouped by semantic ownership. */

void GraphicsShadingGeneratedTexture_ProcessRenderableHierarchy
          (ModelRuntimeNode *modelNode,GeneratedTextureRenderContextView *renderContext);

uint32_t GraphicsIntensityClampTable_Initialize(void);

MmxPackedValue64 GraphicsShadingRuntime_AccumulateCompactLightingAtPoint
          (GraphicsFixedVec3 *worldPointQ12,MmxPackedValue64 packedLightAccumulator);

GraphicsShadingRuntimeRecord * GraphicsShadingRuntime_AllocateRecord
          (GraphicsTransitionTickCount transitionDurationTicks,GraphicsRadiusQ12 radiusQ12,
          PackedRgb24 packedColorRgb,GraphicsWorldCoordinateQ12 worldZQ12,
          GraphicsWorldCoordinateQ12 worldYQ12,GraphicsWorldCoordinateQ12 worldXQ12);

void GraphicsShadingRuntime_ClearRecordTable(void);

void GraphicsShadingRuntime_RebuildCompactLightingRecords(void);

void GraphicsShadingRuntime_CollectNearbyRecords(GraphicsRadiusQ12 queryRadiusQ12,GraphicsWorldCoordinateQ12 worldZQ12,
          GraphicsWorldCoordinateQ12 worldYQ12,GraphicsWorldCoordinateQ12 worldXQ12);

uint32_t GraphicsShadingRuntime_InitializeGeneratedTexture
          (GraphicsAssetSubresourceCount subresourceCount,GraphicsPixelDimension gridHalfSize,
          GraphicsPixelDimension textureDimension);

void GraphicsShadingRuntime_Shutdown(void);

void GraphicsShadingGeneratedTexture_ResetPassScratchAndClearAlphaPlanes(void);

void GraphicsShadingGeneratedTexture_RefreshTouchedAlphaSubresources(void);

void GraphicsShadingGeneratedTexture_RasterizeHardShadowMesh(ModelMeshGroupAddress32 meshRecord);

void GraphicsShadingGeneratedTexture_RasterizeHardShadowHierarchy(ModelRuntimeNode *modelNode);

uint32_t
GraphicsShadingGeneratedTexture_RasterizeSoftShadowMesh(ModelMeshGroupAddress32 meshRecord);

uint32_t
GraphicsShadingGeneratedTexture_RasterizeSoftShadowHierarchy(ModelRuntimeNode *modelNode);

void GraphicsShadingGeneratedTexture_AccumulateProjectedBoundsFromRecords(ModelMeshGroupAddress32 meshRecord);

void GraphicsShadingGeneratedTexture_TraverseHierarchyAndAccumulateProjectedBounds(ModelRuntimeNode *modelNode);

void GraphicsShadingGeneratedTexture_TransformPointXY
          (GraphicsFixedVec2 *outputXY,GraphicsFixedVec3 *point,GraphicsFixedMatrix3x4 *transform);

void GraphicsShadingGeneratedTexture_AdvanceTileCursor(void);

void GraphicsShadingGeneratedTexture_FilterGridScratchMmx(void);

bool GraphicsShadingGeneratedTexture_ProbeHierarchyForGeometry(ModelRuntimeNode *modelNode);

GraphicsProjectedPointPair *GraphicsShadingGeneratedTexture_ReserveFourteenProjectedPointBlocks
          (GeneratedTextureRenderContextView *renderContext);

void GraphicsShadingGeneratedTexture_RollbackFourteenProjectedPointBlocks
               (GeneratedTextureRenderContextView *renderContext);

void GraphicsShadingGeneratedTexture_TransformPointXYQuantized
          (GraphicsFixedVec2 *outputXY,GraphicsFixedVec3 *point,GraphicsFixedMatrix3x4 *transform);

void GraphicsShadingGeneratedTexture_ComposeTransform
          (GraphicsFixedMatrix3x4 *outTransform,GraphicsFixedMatrix3x4 *rhsTransform,
          GraphicsFixedMatrix3x4 *lhsTransform);

void GraphicsShadingGeneratedTexture_RasterizeTriangleMask
          (GraphicsFixedVec2 *vertexA,GraphicsFixedVec2 *vertexB,GraphicsFixedVec2 *vertexC);

/* Not in the original: fills g_PackedLightingLookupTable (the original shipped it precomputed). */
void GraphicsLighting_BuildPackedLookupTable(void);

#endif /* THANDOR_GRAPHICS_RENDER_SHADING_H */

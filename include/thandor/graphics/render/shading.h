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
/* Generated shadow texture (GraphicsShadingGeneratedTexture_ProcessRenderableHierarchy): a shadow vertex fades
   out linearly with its ray distance to the caster and vanishes at 5.0 world units (Q12) */
#define GRAPHICS_SHADING_SHADOW_FADE_DISTANCE_Q12 0x5000
/* Functions are grouped by semantic ownership; address comments are executable virtual addresses. */

/* 0x004CDD40 */
void GraphicsShadingGeneratedTexture_ProcessRenderableHierarchy
          (ModelRuntimeNode *modelNode,GeneratedTextureRenderContextView *renderContext);

/* 0x004BCF70 */
StatusResult GraphicsIntensityClampTable_Initialize(void);

/* 0x004CCA90 */
MmxPackedValue64 GraphicsShadingRuntime_AccumulateCompactLightingAtPointMmxRegs
          (GraphicsFixedVec3 *worldPointQ12,MmxPackedValue64 packedLightAccumulatorMmx);

/* 0x004CCB40 */
ShadingRecordResult GraphicsShadingRuntime_AllocateRecordRegs
          (GraphicsTransitionTickCount transitionDurationTicks,GraphicsRadiusQ12 radiusQ12,
          PackedRgb24 packedColorRgb,GraphicsWorldCoordinateQ12 worldZQ12,
          GraphicsWorldCoordinateQ12 worldYQ12,GraphicsWorldCoordinateQ12 worldXQ12);

/* 0x004CCC60 */
void GraphicsShadingRuntime_ClearRecordTable(void);

/* 0x004CCD00 */
void GraphicsShadingRuntime_RebuildCompactLightingRecords(void);

/* 0x004CCD70 */
void GraphicsShadingRuntime_CollectNearbyRecords(GraphicsRadiusQ12 queryRadiusQ12,GraphicsWorldCoordinateQ12 worldZQ12,
          GraphicsWorldCoordinateQ12 worldYQ12,GraphicsWorldCoordinateQ12 worldXQ12);

/* 0x004CCFF0 */
StatusResult GraphicsShadingRuntime_InitializeGeneratedTexture
          (GraphicsAssetSubresourceCount subresourceCount,GraphicsPixelDimension gridHalfSize,
          GraphicsPixelDimension textureDimension);

/* 0x004CD1B0 */
void GraphicsShadingRuntime_Shutdown(void);

/* 0x004CD200 */
void GraphicsShadingGeneratedTexture_ResetPassScratchAndClearAlphaPlanes(void);

/* 0x004CD360 */
void GraphicsShadingGeneratedTexture_RefreshTouchedAlphaSubresources(void);

/* 0x004D1170 */
void GraphicsShadingGeneratedTexture_ReserveOneProjectedPointBlock
               (GeneratedTextureRenderContextView *renderContext);

/* 0x004CD880 */
void GraphicsShadingGeneratedTexture_RasterizeHardShadowMesh(ModelMeshGroupAddress32 meshRecord);

/* 0x004CD930 */
void GraphicsShadingGeneratedTexture_RasterizeHardShadowHierarchy(ModelRuntimeNode *modelNode);

/* 0x004CD9F0 */
uint32_t
GraphicsShadingGeneratedTexture_RasterizeSoftShadowMesh(ModelMeshGroupAddress32 meshRecord);

/* 0x004CDAB0 */
uint32_t
GraphicsShadingGeneratedTexture_RasterizeSoftShadowHierarchy(ModelRuntimeNode *modelNode);

/* 0x004CDB80 */
void GraphicsShadingGeneratedTexture_AccumulateProjectedBoundsFromRecords(ModelMeshGroupAddress32 meshRecord);

/* 0x004CDC20 */
void GraphicsShadingGeneratedTexture_TraverseHierarchyAndAccumulateProjectedBounds(ModelRuntimeNode *modelNode);

/* 0x00485020 */
void GraphicsShadingGeneratedTexture_TransformPointXY
          (GraphicsFixedVec2 *outputXY,GraphicsFixedVec3 *point,GraphicsFixedMatrix3x4 *transform);

/* 0x004CD2B0 */
void GraphicsShadingGeneratedTexture_AdvanceTileCursor(void);

/* 0x004CD3D0 */
void GraphicsShadingGeneratedTexture_FilterGridScratchMmx(void);

/* 0x004CDCE0 */
bool GraphicsShadingGeneratedTexture_ProbeHierarchyForGeometry(ModelRuntimeNode *modelNode);

/* 0x004D1060 */
ProjectedBlockReserveResult GraphicsShadingGeneratedTexture_ReserveFourteenProjectedPointBlocks
          (GeneratedTextureRenderContextView *renderContext);

/* 0x004D1150 */
void GraphicsShadingGeneratedTexture_RollbackFourteenProjectedPointBlocks
               (GeneratedTextureRenderContextView *renderContext);

/* 0x00484FA0 */
void GraphicsShadingGeneratedTexture_TransformPointXYQuantized
          (GraphicsFixedVec2 *outputXY,GraphicsFixedVec3 *point,GraphicsFixedMatrix3x4 *transform);

/* 0x00485320 */
void GraphicsShadingGeneratedTexture_ComposeTransform
          (GraphicsFixedMatrix3x4 *outTransform,GraphicsFixedMatrix3x4 *rhsTransform,
          GraphicsFixedMatrix3x4 *lhsTransform);

/* 0x004CD690 */
void GraphicsShadingGeneratedTexture_RasterizeTriangleMask
          (GraphicsFixedVec2 *vertexA,GraphicsFixedVec2 *vertexB,GraphicsFixedVec2 *vertexC);

/* Not in the original: fills g_PackedLightingLookupTable (the original shipped it precomputed). */
void GraphicsLighting_BuildPackedLookupTable(void);

#endif /* THANDOR_GRAPHICS_RENDER_SHADING_H */

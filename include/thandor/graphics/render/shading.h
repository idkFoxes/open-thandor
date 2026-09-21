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
/* Functions are grouped by semantic ownership; address comments are executable virtual addresses. */

/* 0x004CDD40 */
void __thandor_void_preserve_eax_ecx_edx
GraphicsShadingGeneratedTexture_ProcessRenderableHierarchy
          (ModelRuntimeNode *modelNode,GeneratedTextureRenderContextView *renderContext);

/* 0x004BCF70 */
bool __thandor_void_preserve_ecx_edx GraphicsIntensityClampTable_InitializeCf(void);

/* 0x004CCA90 */
MmxPackedValue64 __thandor_void_preserve_ecx_edx_mm1
GraphicsShadingRuntime_AccumulateCompactLightingAtPointMmxRegs
          (GraphicsFixedVec3 *worldPointQ12,MmxPackedValue64 packedLightAccumulatorMmx);

/* 0x004CCB40 */
GraphicsShadingRuntimeRecordEaxCf5 __thandor_eax_cf_preserve_ecx_edx
GraphicsShadingRuntime_AllocateRecordRegs
          (GraphicsTransitionTickCount transitionDurationTicks,GraphicsRadiusQ12 radiusQ12,
          PackedRgb24 packedColorRgb,GraphicsWorldCoordinateQ12 worldZQ12,
          GraphicsWorldCoordinateQ12 worldYQ12,GraphicsWorldCoordinateQ12 worldXQ12);

/* 0x004CCC60 */
void __thandor_void_preserve_eax_ecx GraphicsShadingRuntime_ClearRecordTable(void);

/* 0x004CCD00 */
void __thandor_void_preserve_eax_ecx_edx GraphicsShadingRuntime_RebuildCompactLightingRecords(void);

/* 0x004CCD70 */
void __thandor_void_preserve_eax_ecx_edx
GraphicsShadingRuntime_CollectNearbyRecords
          (GraphicsRadiusQ12 queryRadiusQ12,GraphicsWorldCoordinateQ12 worldZQ12,
          GraphicsWorldCoordinateQ12 worldYQ12,GraphicsWorldCoordinateQ12 worldXQ12);

/* 0x004CCFF0 */
StatusValueEaxCf5 __thandor_eax_cf_preserve_ecx_edx
GraphicsShadingRuntime_InitializeGeneratedTextureCf
          (GraphicsAssetSubresourceCount subresourceCount,GraphicsPixelDimension gridHalfSize,
          GraphicsPixelDimension textureDimension);

/* 0x004CD1B0 */
void __thandor_void_preserve_eax GraphicsShadingRuntime_Shutdown(void);

/* 0x004CD200 */
void __thandor_void_preserve_eax_ecx_edx
GraphicsShadingGeneratedTexture_ResetPassScratchAndClearAlphaPlanes(void);

/* 0x004CD360 */
void __thandor_void_preserve_eax_ecx_edx
GraphicsShadingGeneratedTexture_RefreshTouchedAlphaSubresources(void);

/* 0x004D1170 */
void GraphicsShadingGeneratedTexture_ReserveOneProjectedPointBlockCf
               (GeneratedTextureRenderContextView *renderContext);

/* 0x004CD880 */
void __thandor_void_preserve_eax_ecx_edx
GraphicsShadingGeneratedTexture_RasterizeRecordBatchFlag0Clear(ModelMeshGroupAddress32 meshGroup);

/* 0x004CD930 */
void __thandor_void_preserve_eax_ecx_edx
GraphicsShadingGeneratedTexture_TraverseHierarchyFlag0Clear(ModelRuntimeNode *modelNode);

/* 0x004CD9F0 */
void __thandor_void_preserve_eax_ecx_edx
GraphicsShadingGeneratedTexture_RasterizeRecordBatchFlag0Set(ModelMeshGroupAddress32 meshGroup);

/* 0x004CDAB0 */
void __thandor_void_preserve_eax_ecx_edx
GraphicsShadingGeneratedTexture_TraverseHierarchyFlag0Set(ModelRuntimeNode *modelNode);

/* 0x004CDB80 */
void __thandor_void_preserve_eax_ecx_edx
GraphicsShadingGeneratedTexture_AccumulateProjectedBoundsFromRecords
          (ModelMeshGroupAddress32 meshGroup);

/* 0x004CDC20 */
void __thandor_void_preserve_eax_ecx_edx
GraphicsShadingGeneratedTexture_TraverseHierarchyAndAccumulateProjectedBounds
          (ModelRuntimeNode *modelNode);

/* 0x00485020 */
void __thandor_void_preserve_eax_ecx_edx
GraphicsShadingGeneratedTexture_TransformPointXY
          (GraphicsFixedVec2 *outputXY,GraphicsFixedVec3 *point,GraphicsFixedMatrix3x4 *transform);

/* 0x004CD2B0 */
void __thandor_void_preserve_eax_ecx GraphicsShadingGeneratedTexture_AdvanceTileCursor(void);

/* 0x004CD3D0 */
void __thandor_void_preserve_eax_ecx_edx GraphicsShadingGeneratedTexture_FilterGridScratchMmx(void);

/* 0x004CDCE0 */
bool __thandor_void_preserve_eax_ecx
GraphicsShadingGeneratedTexture_ProbeHierarchyForGeometry(ModelRuntimeNode *modelNode);

/* 0x004D1060 */
GraphicsProjectedBlockEaxCf5 __thandor_eax_cf_preserve_ecx_edx
GraphicsShadingGeneratedTexture_ReserveFourteenProjectedPointBlocks
          (GeneratedTextureRenderContextView *renderContext);

/* 0x004D1150 */
void GraphicsShadingGeneratedTexture_RollbackFourteenProjectedPointBlocks
               (GeneratedTextureRenderContextView *renderContext);

/* 0x00484FA0 */
void __thandor_void_preserve_eax_ecx_edx
GraphicsShadingGeneratedTexture_TransformPointXYQuantized
          (GraphicsFixedVec2 *outputXY,GraphicsFixedVec3 *point,GraphicsFixedMatrix3x4 *transform);

/* 0x00485320 */
void __thandor_void_preserve_eax_ecx_edx
GraphicsShadingGeneratedTexture_ComposeTransform
          (GraphicsFixedMatrix3x4 *outTransform,GraphicsFixedMatrix3x4 *rhsTransform,
          GraphicsFixedMatrix3x4 *lhsTransform);

/* 0x004CD690 */
void __thandor_void_preserve_eax_ecx_edx
GraphicsShadingGeneratedTexture_RasterizeTriangleMask
          (GraphicsFixedVec2 *vertexA,GraphicsFixedVec2 *vertexB,GraphicsFixedVec2 *vertexC);

#endif /* THANDOR_GRAPHICS_RENDER_SHADING_H */

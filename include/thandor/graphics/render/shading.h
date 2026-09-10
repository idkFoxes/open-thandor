#ifndef THANDOR_GRAPHICS_RENDER_SHADING_H
#define THANDOR_GRAPHICS_RENDER_SHADING_H

#include <thandor/generated/v523_types.h>
#include <thandor/core/contracts.h>

/* Submodule: graphics/render/shading. */
/* Functions are grouped by semantic ownership; address comments are executable virtual addresses. */

/* 0x004CDD40 */
void GraphicsShadingGeneratedTexture_ProcessRenderableHierarchy(int param_1,int param_2);

/* 0x004BCF70 */
void GraphicsIntensityClampTable_InitializeCf(void);

/* 0x004CCA90 */
void __fastcall GraphicsShadingRuntime_AccumulateCompactLightingAtPointMmxRegs(undefined4 param_1,int *param_2);

/* 0x004CCB40 */
longlong GraphicsShadingRuntime_AllocateRecordRegs (GraphicsTransitionTickCount transitionDurationTicks,GraphicsRadiusQ12 radiusQ12, PackedRgb24 packedColorRgb,GraphicsWorldCoordinateQ12 worldZQ12, GraphicsWorldCoordinateQ12 worldYQ12,GraphicsWorldCoordinateQ12 worldXQ12);

/* 0x004CCC60 */
void __cdecl GraphicsShadingRuntime_ClearRecordTable(void);

/* 0x004CCD00 */
void __cdecl GraphicsShadingRuntime_RebuildCompactLightingRecords(void);

/* 0x004CCD70 */
void GraphicsShadingRuntime_CollectNearbyRecords (GraphicsRadiusQ12 queryRadiusQ12,GraphicsWorldCoordinateQ12 worldZQ12, GraphicsWorldCoordinateQ12 worldYQ12,GraphicsWorldCoordinateQ12 worldXQ12);

/* 0x004CCFF0 */
undefined8 GraphicsShadingRuntime_InitializeGeneratedTextureCf (GraphicsAssetSubresourceCount subresourceCount,GraphicsPixelDimension gridHalfSize, GraphicsPixelDimension textureDimension);

/* 0x004CD1B0 */
undefined4 __cdecl GraphicsShadingRuntime_Shutdown(void);

/* 0x004CD200 */
void __cdecl GraphicsShadingGeneratedTexture_ResetPassScratchAndClearAlphaPlanes(void);

/* 0x004CD360 */
void GraphicsShadingGeneratedTexture_RefreshTouchedAlphaSubresources(void);

/* 0x004D1170 */
void GraphicsShadingGeneratedTexture_ReserveOneProjectedPointBlockCf(int param_1);

/* 0x004CD880 */
void GraphicsShadingGeneratedTexture_RasterizeRecordBatchFlag0Clear(int param_1);

/* 0x004CD930 */
void GraphicsShadingGeneratedTexture_TraverseHierarchyFlag0Clear(int param_1);

/* 0x004CD9F0 */
void GraphicsShadingGeneratedTexture_RasterizeRecordBatchFlag0Set(int param_1);

/* 0x004CDAB0 */
void GraphicsShadingGeneratedTexture_TraverseHierarchyFlag0Set(int param_1);

/* 0x004CDB80 */
void GraphicsShadingGeneratedTexture_AccumulateProjectedBoundsFromRecords(int param_1);

/* 0x004CDC20 */
void GraphicsShadingGeneratedTexture_TraverseHierarchyAndAccumulateProjectedBounds(int param_1);

/* 0x00485020 */
void GraphicsShadingGeneratedTexture_TransformPointXY(int *param_1,int *param_2,int *param_3);

/* 0x004CD2B0 */
void GraphicsShadingGeneratedTexture_AdvanceTileCursor(void);

/* 0x004CD3D0 */
void __cdecl GraphicsShadingGeneratedTexture_FilterGridScratchMmx(void);

/* 0x004CDCE0 */
undefined4 GraphicsShadingGeneratedTexture_ProbeHierarchyForGeometry(int param_1);

/* 0x004D1060 */
void GraphicsShadingGeneratedTexture_ReserveFourteenProjectedPointBlocks(int param_1);

/* 0x004D1150 */
void GraphicsShadingGeneratedTexture_RollbackFourteenProjectedPointBlocks(int param_1);

/* 0x00484FA0 */
void GraphicsShadingGeneratedTexture_TransformPointXYQuantized (uint *param_1,int *param_2,int *param_3);

/* 0x00485320 */
void GraphicsShadingGeneratedTexture_ComposeTransform(uint *param_1,int *param_2,int *param_3);

/* 0x004CD690 */
void GraphicsShadingGeneratedTexture_RasterizeTriangleMask(int *param_1,int *param_2,int *param_3);

#endif /* THANDOR_GRAPHICS_RENDER_SHADING_H */

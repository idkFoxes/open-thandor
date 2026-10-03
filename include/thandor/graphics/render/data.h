/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/include/thandor/graphics/render/data.h
 */

#ifndef THANDOR_GRAPHICS_RENDER_DATA_H
#define THANDOR_GRAPHICS_RENDER_DATA_H

#include <thandor/generated/types.h>
#include <thandor/generated/ui_templates.h>

extern SoftwareBgraWordLanes g_ShadingIntensityScaleMmx[256];

extern uint64_t g_GraphicsShadingRasterizeMmxPackedDwordOneZero;

extern uint64_t g_GraphicsShadingMmxPacked3BitPerByteMask;

extern uint64_t g_VertexColorAlphaPreserveMaskMMX;

extern uint64_t g_VertexColorRgbHalveMaskMMX;

extern GraphicsTextureSetRefreshProc *g_GraphicsRefreshTextureAlpha;

extern GraphicsPrimitiveQueueRadixSortProc *PTR_GraphicsPrimitiveQueue_RadixSortForRendering_00485844;

extern int32_t g_ProjectionScaleFixed;

extern GraphicsPrimitiveQueue *g_ActivePrimitiveQueue;

extern GraphicsFixedRect g_ProjectionClipRect;

extern GraphicsFixedMatrix3x4 g_ViewProjectionMatrixFixed;

extern GraphicsFixedMatrix3x4 g_AuxiliaryRotationMatrixFixed;

extern uint32_t g_PrimitiveRadixBucketWords[256];

extern uint32_t g_GraphicsIntensityClampTableBase;

extern GraphicsFixedMatrix3x4 g_ModelViewCompositeTransform;

extern GraphicsFixedVec3 g_ModelViewDirectionLocal;

extern GraphicsFixedVec3 g_ModelViewDirectionWorld;

extern GraphicsFixedVec3 g_ModelAuxiliaryForwardDirectionLocal;

extern GraphicsShadingRuntimeRecord g_GraphicsShadingCompactRecords[256];

extern GraphicsShadingRecordCount g_GraphicsShadingCompactRecordCount;

extern GraphicsShadingRuntimeRecord g_GraphicsShadingNearbyRecords[256];

extern GraphicsShadingRecordCount g_GraphicsShadingNearbyRecordCount; /* GraphicsShadingRecordCount (4 bytes, 0 in the image): number of valid g_GraphicsShadingNearbyRecords, set by GraphicsShadingRuntime_CollectNearbyRecords, read by the model vertex lighting. Followed by 8 bytes of 0x90 filler and g_ModelLightingMmxMultiplierRows. */

extern SoftwareBgraWordLanes g_ModelLightingMmxMultiplierRows[819]; /* PMULHW multipliers (alpha lane 0x4000) of the model vertex lighting; rows 0..135 = distance attenuation rows -136..-1 (B/G/R 0x007F at -1 rising by 0x80 to 0x3F7F, then 0x3FFF), rows 136..681 = distance attenuation from MODEL_DISTANCE_ATTENUATION_ROW0 (ModelRender_ComputeVertexIntensityDefaultPath, signed dot >> 21), rows 682..818 = scaled-lighting multipliers from MODEL_LIGHTING_SCALE_ROW0 (ModelRender_ComputeVertexIntensityScaledPath, signed (dot / lightingScaleQ12) >> 9; B/G/R 0x1FFF falling by 0x80 to 0x007F, then 0). See graphics/render/model.h. */

extern GraphicsFixedVec3 g_ModelLightingVertexToLightVectorScratch;

extern GraphicsFixedVec3 g_ModelLightingTransformedSurfaceNormalScratch;

extern uint32_t g_GraphicsShadingTextureDimension;

extern uint32_t g_GraphicsShadingGridHalfSize;

extern uint8_t *g_GraphicsShadingGeneratedTexturePixelCursor;

extern uint32_t g_GraphicsShadingGeneratedTextureTileX;

extern uint32_t g_GraphicsShadingGeneratedTextureTileY;

extern GraphicsSubresourceIndex g_GraphicsShadingGeneratedTextureSubresourceIndex;

extern uint32_t g_GraphicsShadingSubresourceCount;

extern uint32_t g_GraphicsShadingGeneratedTextureTileXQ20;

extern uint32_t g_GraphicsShadingGeneratedTextureTileYQ20;

extern uint32_t g_GraphicsShadingGridStepQ20;

extern int32_t g_GraphicsShadingGridStepQ20Current;

extern GraphicsTextureSourceAsset *g_GraphicsShadingGeneratedAsset;

extern void *g_GraphicsShadingGridScratch;

extern pointer g_GraphicsShadingGridScratchInterior;

extern GraphicsTextureSet *g_GraphicsShadingTextureSet;

extern uint32_t g_GraphicsShadingGeneratedTextureCompletedTraversalCount;

extern int32_t g_GraphicsShadingPositiveGridOriginQ12;

extern int32_t g_GraphicsShadingNegativeGridOriginQ12;

extern GeneratedTextureScratchRuntime g_GeneratedTextureScratchRuntime;

extern GraphicsPrimitiveQueue *g_PrimitiveQueueStorage;

extern uint32_t g_PrimitiveQueuePoolCapacity;

extern int32_t *g_SoftwareDepthBuffer;

extern GraphicsPaletteAsset *g_TerrainSecondaryPalette;

extern GraphicsFixedVec3 g_GraphicsTransformInputScratchVec3;

extern GraphicsFixedVec3 g_GraphicsTransformOutputScratchVec3;

#endif

/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/include/thandor/graphics/render/data.h
 */

#ifndef THANDOR_GRAPHICS_RENDER_DATA_H
#define THANDOR_GRAPHICS_RENDER_DATA_H

#include <thandor/generated/types.h>
#include <thandor/generated/ui_templates.h>

extern SoftwareBgraWordLanes g_ShadingIntensityScaleMmx[256]; /* 0041EE80 g_ShadingIntensityScaleMmx */

extern uint64_t g_GraphicsShadingRasterizeMmxPackedDwordOneZero; /* 0041F680 g_GraphicsShadingRasterizeMmxPackedDwordOneZero */

extern uint64_t g_GraphicsShadingMmxPacked3BitPerByteMask; /* 0041F6C8 g_GraphicsShadingMmxPacked3BitPerByteMask */

extern uint64_t g_VertexColorAlphaPreserveMaskMMX; /* 0041F700 g_VertexColorAlphaPreserveMaskMMX */

extern uint64_t g_VertexColorRgbHalveMaskMMX; /* 0041F708 g_VertexColorRgbHalveMaskMMX */

extern GraphicsTextureSetRefreshProc *g_GraphicsRefreshTextureAlpha; /* 00485840 g_GraphicsRefreshTextureAlpha */

extern GraphicsPrimitiveQueueRadixSortProc *PTR_GraphicsPrimitiveQueue_RadixSortForRendering_00485844; /* 00485844 PTR_GraphicsPrimitiveQueue_RadixSortForRendering_00485844 */

extern int32_t g_ProjectionScaleFixed; /* 00485868 g_ProjectionScaleFixed */

extern GraphicsPrimitiveQueue *g_ActivePrimitiveQueue; /* 0048588C g_ActivePrimitiveQueue */

extern GraphicsFixedRect g_ProjectionClipRect; /* 00485890 g_ProjectionClipRect */

extern GraphicsFixedMatrix3x4 g_ViewProjectionMatrixFixed; /* 004858A0 g_ViewProjectionMatrixFixed */

extern GraphicsFixedMatrix3x4 g_AuxiliaryRotationMatrixFixed; /* 00485954 g_AuxiliaryRotationMatrixFixed */

extern uint32_t g_PrimitiveRadixBucketWords[256]; /* 00485A40 g_PrimitiveRadixBucketWords */

extern uint32_t g_GraphicsIntensityClampTableBase; /* 004BCF68 g_GraphicsIntensityClampTableBase */

extern GraphicsFixedMatrix3x4 g_ModelViewCompositeTransform; /* 004BD450 g_ModelViewCompositeTransform */

extern GraphicsFixedVec3 g_ModelViewDirectionLocal; /* 004BD480 g_ModelViewDirectionLocal */

extern GraphicsFixedVec3 g_ModelViewDirectionWorld; /* 004BD48C g_ModelViewDirectionWorld */

extern GraphicsFixedVec3 g_ModelAuxiliaryForwardDirectionLocal; /* 004BD498 g_ModelAuxiliaryForwardDirectionLocal */

extern GraphicsShadingRuntimeRecord g_GraphicsShadingCompactRecords[256]; /* 004C2D50 g_GraphicsShadingCompactRecords */

extern GraphicsShadingRecordCount g_GraphicsShadingCompactRecordCount; /* 004C6D50 g_GraphicsShadingCompactRecordCount */

extern GraphicsShadingRuntimeRecord g_GraphicsShadingNearbyRecords[256]; /* 004C6D54 g_GraphicsShadingNearbyRecords */

extern GraphicsShadingRecordCount g_GraphicsShadingNearbyRecordCount; /* 004CAD54 g_GraphicsShadingNearbyRecordCount: GraphicsShadingRecordCount (4 bytes, 0 in the image): number of valid g_GraphicsShadingNearbyRecords, set by GraphicsShadingRuntime_CollectNearbyRecords, read by the model vertex lighting. Followed by 8 bytes of 0x90 filler and g_ModelLightingMmxMultiplierRows. */

extern SoftwareBgraWordLanes g_ModelLightingMmxMultiplierRows[819]; /* 004CAD60 g_ModelLightingMmxMultiplierRows: PMULHW multipliers (alpha lane 0x4000) of the model vertex lighting; rows 0..135 = distance attenuation rows -136..-1 (B/G/R 0x007F at -1 rising by 0x80 to 0x3F7F, then 0x3FFF), rows 136..681 = distance attenuation from MODEL_DISTANCE_ATTENUATION_ROW0 (0x004CB1A0; ModelRender_ComputeVertexIntensityDefaultPath, signed dot >> 21), rows 682..818 = scaled-lighting multipliers from MODEL_LIGHTING_SCALE_ROW0 (0x004CC2B0; ModelRender_ComputeVertexIntensityScaledPath, signed (dot / lightingScaleQ12) >> 9; B/G/R 0x1FFF falling by 0x80 to 0x007F, then 0). See graphics/render/model.h. */

extern GraphicsFixedVec3 g_ModelLightingVertexToLightVectorScratch; /* 004CC6F8 g_ModelLightingVertexToLightVectorScratch */

extern GraphicsFixedVec3 g_ModelLightingTransformedSurfaceNormalScratch; /* 004CC704 g_ModelLightingTransformedSurfaceNormalScratch */

extern uint32_t g_GraphicsShadingTextureDimension; /* 004CCE00 g_GraphicsShadingTextureDimension */

extern uint32_t g_GraphicsShadingGridHalfSize; /* 004CCE04 g_GraphicsShadingGridHalfSize */

extern uint8_t *g_GraphicsShadingGeneratedTexturePixelCursor; /* 004CCE08 g_GraphicsShadingGeneratedTexturePixelCursor */

extern uint32_t g_GraphicsShadingGeneratedTextureTileX; /* 004CCE0C g_GraphicsShadingGeneratedTextureTileX */

extern uint32_t g_GraphicsShadingGeneratedTextureTileY; /* 004CCE10 g_GraphicsShadingGeneratedTextureTileY */

extern GraphicsSubresourceIndex g_GraphicsShadingGeneratedTextureSubresourceIndex; /* 004CCE14 g_GraphicsShadingGeneratedTextureSubresourceIndex */

extern uint32_t g_GraphicsShadingSubresourceCount; /* 004CCE18 g_GraphicsShadingSubresourceCount */

extern uint32_t g_GraphicsShadingGeneratedTextureTileXQ20; /* 004CCE1C g_GraphicsShadingGeneratedTextureTileXQ20 */

extern uint32_t g_GraphicsShadingGeneratedTextureTileYQ20; /* 004CCE20 g_GraphicsShadingGeneratedTextureTileYQ20 */

extern uint32_t g_GraphicsShadingGridStepQ20; /* 004CCE24 g_GraphicsShadingGridStepQ20 */

extern int32_t g_GraphicsShadingGridStepQ20Current; /* 004CCE28 g_GraphicsShadingGridStepQ20Current */

extern GraphicsTextureSourceAsset *g_GraphicsShadingGeneratedAsset; /* 004CCE2C g_GraphicsShadingGeneratedAsset */

extern void *g_GraphicsShadingGridScratch; /* 004CCE30 g_GraphicsShadingGridScratch */

extern pointer g_GraphicsShadingGridScratchInterior; /* 004CCE34 g_GraphicsShadingGridScratchInterior */

extern GraphicsTextureSet *g_GraphicsShadingTextureSet; /* 004CCE38 g_GraphicsShadingTextureSet */

extern uint32_t g_GraphicsShadingGeneratedTextureCompletedTraversalCount; /* 004CCE3C g_GraphicsShadingGeneratedTextureCompletedTraversalCount */

extern int32_t g_GraphicsShadingPositiveGridOriginQ12; /* 004CCE40 g_GraphicsShadingPositiveGridOriginQ12 */

extern int32_t g_GraphicsShadingNegativeGridOriginQ12; /* 004CCE44 g_GraphicsShadingNegativeGridOriginQ12 */

extern GeneratedTextureScratchRuntime g_GeneratedTextureScratchRuntime; /* 004CCE48 g_GeneratedTextureScratchRuntime */

extern GraphicsPrimitiveQueue *g_PrimitiveQueueStorage; /* 004D0A00 g_PrimitiveQueueStorage */

extern uint32_t g_PrimitiveQueuePoolCapacity; /* 004D0A04 g_PrimitiveQueuePoolCapacity */

extern int32_t *g_SoftwareDepthBuffer; /* 004D1238 g_SoftwareDepthBuffer */

extern GraphicsPaletteAsset *g_TerrainSecondaryPalette; /* 00503A80 g_TerrainSecondaryPalette */

extern GraphicsFixedVec3 g_GraphicsTransformInputScratchVec3; /* 0050A380 g_GraphicsTransformInputScratchVec3 */

extern GraphicsFixedVec3 g_GraphicsTransformOutputScratchVec3; /* 0050A3B0 g_GraphicsTransformOutputScratchVec3 */

#endif

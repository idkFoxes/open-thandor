/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/include/thandor/world/terrain/data.h
 */

#ifndef THANDOR_WORLD_TERRAIN_DATA_H
#define THANDOR_WORLD_TERRAIN_DATA_H

#include <thandor/generated/types.h>
#include <thandor/generated/ui_templates.h>

extern uint64_t g_PackedLightingLookupTable[512]; /* 0041DE80 g_PackedLightingLookupTable */

extern uint64_t g_TerrainOccupancyMmxSignBiasBytes; /* 0041F690 g_TerrainOccupancyMmxSignBiasBytes */

extern uint64_t g_TerrainOccupancyMmxClearBits1And2Mask; /* 0041F698 g_TerrainOccupancyMmxClearBits1And2Mask */

extern uint64_t g_TerrainOccupancyMmxAllBitsMask; /* 0041F6A0 g_TerrainOccupancyMmxAllBitsMask */

extern uint64_t g_TerrainOccupancyMmxPackedScale0280; /* 0041F6A8 g_TerrainOccupancyMmxPackedScale0280 */

extern uint64_t g_TerrainOccupancyMmxPersistentWeights; /* 0041F6B0 g_TerrainOccupancyMmxPersistentWeights */

extern uint64_t g_TerrainOccupancyMmxCurrentWeights; /* 0041F6B8 g_TerrainOccupancyMmxCurrentWeights */

extern uint64_t g_FieldGridOccupancyMmxHighBitMask; /* 0041F6C0 g_FieldGridOccupancyMmxHighBitMask */

extern GraphicsTextureSetLoadPackageProc *g_GraphicsTextureSetLoadPackage; /* 0048582C g_GraphicsTextureSetLoadPackage */

extern GraphicsTextureSetReleasePackageProc *g_GraphicsTextureSetReleasePackage; /* 00485830 g_GraphicsTextureSetReleasePackage */

extern GraphicsPaletteAssetLoadPackageProc *g_GraphicsPaletteAssetLoadPackage; /* 004A8F54 g_GraphicsPaletteAssetLoadPackage */

extern FieldGridInterpolationCallbackTable5 g_FieldGridInterpolationCallbacks5; /* 004FEA30 g_FieldGridInterpolationCallbacks5 */

extern TerrainProjectedRowSpan g_TerrainProjectedRowSpans[260]; /* 004FFC80 g_TerrainProjectedRowSpans */

extern PackedArgb32 g_TerrainDirectionalLightColorLut[513]; /* 00501190 g_TerrainDirectionalLightColorLut: shaded ramp (256) + lit half (257, from 00501590), indexed from the middle entry */

extern uint32_t g_TerrainDirectionalLightSecondaryColor; /* 00501994 g_TerrainDirectionalLightSecondaryColor */

extern GraphicsFixedVec3 g_TerrainLightDirection; /* 00501998 g_TerrainLightDirection: Q28 unit vector */

extern uint8_t *g_TerrainByteClampLookup; /* 005019A4 g_TerrainByteClampLookup */

extern TerrainDirectionRecord g_TerrainDirectionRecordTable256[256]; /* 005019A8 g_TerrainDirectionRecordTable256 */

extern GraphicsTextureSet *g_TerrainPrimaryTextureSet; /* 00503A74 g_TerrainPrimaryTextureSet */

extern void *g_TerrainSoilPacketTablePayload; /* 00503A78 g_TerrainSoilPacketTablePayload */

extern void *g_TerrainSurfacePacketTablePayload; /* 00503A7C g_TerrainSurfacePacketTablePayload */

extern GraphicsPaletteAsset *g_TerrainPrimaryPalette; /* 00503A84 g_TerrainPrimaryPalette */

extern TerrainMaterialSuffixEntry g_TerrainMaterialTextureSuffixLettersUtf16AtoZ[26]; /* 00503A90 g_TerrainMaterialTextureSuffixLettersUtf16AtoZ */

extern int32_t g_TerrainHeightBandMaximumDelta; /* 00503AF8 g_TerrainHeightBandMaximumDelta */

extern int32_t g_TerrainHeightBandMinimumDelta; /* 00503AFC g_TerrainHeightBandMinimumDelta */

extern int32_t g_TerrainAuxHeightMinimum; /* 00503B00 g_TerrainAuxHeightMinimum: int32_t minimum (triangle1NormalAngles >> 16) for the auxiliary height/placement scans in world/terrain/height.c (0x3000) */

extern int32_t g_TerrainHeightDeltaScaleByStepQ12[256]; /* 00505FA0 g_TerrainHeightDeltaScaleByStepQ12 */

extern uint32_t g_TerrainScanRowStrideBytes; /* 005063A0 g_TerrainScanRowStrideBytes */

extern uint32_t g_TerrainScanStepLimit; /* 005063A4 g_TerrainScanStepLimit */

extern TerrainScanSelectorUnion g_TerrainScanSharedSelectorValue; /* 005063A8 g_TerrainScanSharedSelectorValue */

extern uint32_t g_TerrainScanReferenceHeight; /* 005063AC g_TerrainScanReferenceHeight */

extern TerrainClassPlacementAndOverlayCallbackTable10 g_TerrainClassPlacementAndOverlayCallbacks10; /* 0051FAF0 g_TerrainClassPlacementAndOverlayCallbacks10 */

extern TerrainCompositeTextureRuntime *g_TerrainCompositeTexture; /* 0053D360 g_TerrainCompositeTexture */

extern InGameRuntimeRoot *g_InGameRuntimeRoot; /* 0056327C g_InGameRuntimeRoot */

extern GraphicsTextureSourceAsset *g_InGamePanelTextureSource; /* 00563280 g_InGamePanelTextureSource */

extern uint32_t g_TerrainMaterialEditFieldGrid; /* 0056D82C g_TerrainMaterialEditFieldGrid */

extern uint32_t g_TerrainMaterialEditDeltaBuffer; /* 0056D830 g_TerrainMaterialEditDeltaBuffer */

extern uint32_t g_TerrainMaterialEditReferenceMaterialByte; /* 0056D834 g_TerrainMaterialEditReferenceMaterialByte */

extern uint32_t g_TerrainMaterialEditReplacementMaterialByte; /* 0056D838 g_TerrainMaterialEditReplacementMaterialByte: followed by 4 bytes 0x90 fill (dropped) */

#endif

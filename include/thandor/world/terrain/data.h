/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/include/thandor/world/terrain/data.h
 */

#ifndef THANDOR_WORLD_TERRAIN_DATA_H
#define THANDOR_WORLD_TERRAIN_DATA_H

#include <thandor/generated/types.h>
#include <thandor/generated/ui_templates.h>

extern uint64_t g_PackedLightingLookupTable[512];

extern uint64_t g_TerrainOccupancyMmxSignBiasBytes;

extern uint64_t g_TerrainOccupancyMmxClearBits1And2Mask;

extern uint64_t g_TerrainOccupancyMmxAllBitsMask;

extern uint64_t g_TerrainOccupancyMmxPackedScale0280;

extern uint64_t g_TerrainOccupancyMmxPersistentWeights;

extern uint64_t g_TerrainOccupancyMmxCurrentWeights;

extern uint64_t g_FieldGridOccupancyMmxHighBitMask;

extern GraphicsTextureSetLoadPackageProc *g_GraphicsTextureSetLoadPackage;

extern GraphicsTextureSetReleasePackageProc *g_GraphicsTextureSetReleasePackage;

extern GraphicsPaletteAssetLoadPackageProc *g_GraphicsPaletteAssetLoadPackage;

extern FieldGridInterpolationCallbackTable5 g_FieldGridInterpolationCallbacks5;

extern TerrainProjectedRowSpan g_TerrainProjectedRowSpans[260];

extern PackedArgb32 g_TerrainDirectionalLightColorLut[513]; /* shaded ramp (256) + lit half (257, from 00501590), indexed from the middle entry */

extern uint32_t g_TerrainDirectionalLightSecondaryColor;

extern GraphicsFixedVec3 g_TerrainLightDirection; /* Q28 unit vector */

extern uint8_t *g_TerrainByteClampLookup;

extern TerrainDirectionRecord g_TerrainDirectionRecordTable256[256];

extern GraphicsTextureSet *g_TerrainPrimaryTextureSet;

extern void *g_TerrainSoilPacketTablePayload;

extern void *g_TerrainSurfacePacketTablePayload;

extern GraphicsPaletteAsset *g_TerrainPrimaryPalette;

extern TerrainMaterialSuffixEntry g_TerrainMaterialTextureSuffixLettersUtf16AtoZ[26];

extern int32_t g_TerrainHeightBandMaximumDelta;

extern int32_t g_TerrainHeightBandMinimumDelta;

extern int32_t g_TerrainAuxHeightMinimum; /* int32_t minimum (triangle1NormalAngles >> 16) for the auxiliary height/placement scans in world/terrain/height.c (0x3000) */

extern int32_t g_TerrainHeightDeltaScaleByStepQ12[256];

extern uint32_t g_TerrainScanRowStrideBytes;

extern uint32_t g_TerrainScanStepLimit;

extern TerrainScanSelectorUnion g_TerrainScanSharedSelectorValue;

extern uint32_t g_TerrainScanReferenceHeight;

extern TerrainClassPlacementAndOverlayCallbackTable10 g_TerrainClassPlacementAndOverlayCallbacks10;

extern TerrainCompositeTextureRuntime *g_TerrainCompositeTexture;

extern InGameRuntimeRoot *g_InGameRuntimeRoot;

extern GraphicsTextureSourceAsset *g_InGamePanelTextureSource;

extern uint32_t g_TerrainMaterialEditFieldGrid;

extern uint32_t g_TerrainMaterialEditDeltaBuffer;

extern uint32_t g_TerrainMaterialEditReferenceMaterialByte;

extern uint32_t g_TerrainMaterialEditReplacementMaterialByte; /* followed by 4 bytes 0x90 fill (dropped) */

#endif

/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/include/thandor/world/terrain/visuals.h
 * Reverse engineering by idkFoxes 2026
 */

#ifndef THANDOR_WORLD_TERRAIN_VISUALS_H
#define THANDOR_WORLD_TERRAIN_VISUALS_H

#include <thandor/generated/types.h>
#include <thandor/core/contracts.h>

/* Submodule: world/terrain/visuals. */

/* g_TerrainMaterialTextureSets: one texture set per terrain material, loaded from the secondary path with
   the suffix letter a..z (TerrainVisualResources_LoadPrimary). */
#define TERRAIN_MATERIAL_TEXTURE_SET_COUNT 26
/* Entries of g_TerrainLightingColorRampArgb256 and g_TerrainDirectionalLightColorLut
   (TerrainLighting_BuildColorRampAndSetBaseColor). */
#define TERRAIN_LIGHTING_RAMP_ENTRY_COUNT 256
#define TERRAIN_DIRECTIONAL_LIGHT_LUT_ENTRY_COUNT 257
/* g_TerrainByteClampLookup (TerrainByteClampLookup_Initialize): 256 rows of 256 bytes, row = a cell's
   occupancy byte, column = its runtime byte +0x68. Each row moves the runtime byte by one fade step towards the
   row's target level (the levels FieldGrid_ClassifyCellFlagsToRuntimeByte writes directly): rows 0x00..0x7F
   by bit 0 (clear -> NONE, set -> FULL), 0x80 and 0x82 -> PERSISTENT, 0x81 and 0x83..0xFF -> FULL. */
#define TERRAIN_BYTE_CLAMP_LOOKUP_BYTES 0x10000
#define TERRAIN_RUNTIME_BYTE_FADE_STEP 0x15
#define TERRAIN_RUNTIME_BYTE_LEVEL_NONE 0x00
#define TERRAIN_RUNTIME_BYTE_LEVEL_PERSISTENT 0x87 /* only the persistent occupancy bit 7 */
#define TERRAIN_RUNTIME_BYTE_LEVEL_FULL 0xff

/* Functions are grouped by semantic ownership; address comments are executable virtual addresses. */

/* 0x0053D370 */
StatusResult __thandor_eax_cf_preserve_ecx_edx TerrainCompositeTexture_Create(void);

/* 0x00503B10 */
StatusResult __thandor_eax_cf_preserve_ecx_edx TerrainByteClampLookup_Initialize(void);

/* 0x00503F30 */
StatusResult __thandor_void_preserve_ecx_edx
TerrainVisualResources_LoadPrimary
          (uint16_t *primaryResourcePath,uint16_t *secondaryResourcePath,FieldGridAsset *field);

/* 0x005041C0 */
StatusResult __thandor_eax_cf_preserve_ecx_edx
TerrainVisualResources_LoadAndClearCellOverlayFlags
          (uint16_t *primaryResourcePath,uint16_t *secondaryResourcePath,FieldGridAsset *field);

/* 0x00504470 */
void __thandor_void_preserve_eax_ecx TerrainVisualResources_Shutdown(void);

/* 0x00505780 */
void __thandor_void_preserve_eax_ecx_edx
TerrainLighting_BuildColorRampAndSetBaseColor
          (PackedArgb32 secondaryColorArgb,PackedArgb32 baseColorArgb,PackedArgb32 rampStepColorArgb
          );

/* 0x0053D4D0 */
void __thandor_preserve_eax TerrainCompositeTexture_Destroy(void);

/* 0x00561EA0 */
void __thandor_preserve_eax_edx
TerrainLighting_AdjustDirectionAndRecomputeField
          (uint32_t playerRuntimeId,uint32_t reservedZero,uint32_t deltaElevationAngle,
          uint32_t deltaAzimuthAngle);

/* 0x0053D560 */
void __thandor_void_preserve_eax_ecx_edx TerrainCompositeTexture_FillPlane1(void);

/* 0x0053D680 */
void __thandor_void_preserve_eax_ecx_edx TerrainCompositeTexture_FillPlane2(void);

/* 0x0053D840 */
void __thandor_void_preserve_eax_ecx_edx TerrainCompositeTexture_RebuildPlane0(void);

#endif /* THANDOR_WORLD_TERRAIN_VISUALS_H */

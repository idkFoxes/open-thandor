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
/* TerrainCompositeTexture_Create: the minimap texture is a 0x200-byte gfx header, three 0x20-byte source entries
   and then three ARGB planes of gridWidth * gridHeight * 4 bytes each. */
#define TERRAIN_COMPOSITE_TEXTURE_HEADER_BYTES 0x200 /* subresource table offset */
#define TERRAIN_COMPOSITE_TEXTURE_PIXELS_OFFSET 0x260 /* dataOffset of plane 0 */
/* ".dat" extension code for WidePath_SetExtensionCode (see WIDE_PATH_EXTENSION_* in core/text/path.h) */
#define WIDE_PATH_EXTENSION_DAT 0x746164
/* <primary>.dat / <secondary>.dat packet tables: a 0x20-byte header (first dword: phase seed bit width, read by
   FieldGrid_InitializeRuntimeCellsAndBoundaryFlags), then the 0x20-byte packets. g_TerrainSurfacePacketTablePayload / g_TerrainSoilPacketTablePayload
   point past the header; release subtracts it again. */
#define TERRAIN_PACKET_TABLE_HEADER_BYTES 0x20
/* (argb & mask) >> 1 halves all four 8-bit channels at once (each channel's low bit cleared first); two
   halved pixels added give their average (TerrainCompositeTexture_RebuildPlane0) */
#define TERRAIN_ARGB_HALVE_MASK 0xfefefefe
/* Random animation of the 256 terrain direction records (TerrainVisualResources_Load*): scale (sin/cos
   amplitude) MIN + (random & MASK), rotation rate +-(MIN + (random & MASK)) 16-bit angle units per step. */
#define TERRAIN_DIRECTION_SCALE_MIN 0x80
#define TERRAIN_DIRECTION_SCALE_RANDOM_MASK 0x1f
#define TERRAIN_DIRECTION_RATE_MIN_ANGLE16 0x200
#define TERRAIN_DIRECTION_RATE_RANDOM_MASK 0x7f
/* TerrainLighting_AdjustDirectionAndRecomputeField: the light elevation stays at least this far (1/16 turn)
   below the horizon; the other limit is -FIXED_ANGLE16_QUARTER_TURN (straight down). */
#define TERRAIN_LIGHT_ELEVATION_MIN_TILT_ANGLE16 0x1000
/* Minimap planes (TerrainCompositeTexture_FillPlane1/2): g_PackedLightingLookupTable levels. Dry cells use
   HEIGHT_LIGHT_FIRST + (terrainHeight >> 7) clamped to HEIGHT_LEVELS steps; water uses
   WATER_LIGHT_FIRST + WATER_DEPTH_LEVELS - (waterSurfaceDelta >> 5), clamped to WATER_DEPTH_LEVELS steps. */
#define TERRAIN_MINIMAP_HEIGHT_LIGHT_FIRST 0x70
#define TERRAIN_MINIMAP_HEIGHT_LEVELS 0x60
#define TERRAIN_MINIMAP_HEIGHT_LIGHT_LAST \
          (TERRAIN_MINIMAP_HEIGHT_LIGHT_FIRST + TERRAIN_MINIMAP_HEIGHT_LEVELS - 1) /* 0xCF */
#define TERRAIN_MINIMAP_WATER_LIGHT_FIRST 0x80 /* deepest water */
#define TERRAIN_MINIMAP_WATER_DEPTH_LEVELS 0x40
#define TERRAIN_MINIMAP_WATER_LIGHT_LAST \
          (TERRAIN_MINIMAP_WATER_LIGHT_FIRST + TERRAIN_MINIMAP_WATER_DEPTH_LEVELS - 1) /* 0xBF, shallowest */
/* Entries of the in-game panel palette bank (panel subresource 36) used for the minimap */
#define TERRAIN_MINIMAP_PANEL_COLOR_SOIL 0x40
#define TERRAIN_MINIMAP_PANEL_COLOR_XENITE 0x41
#define TERRAIN_MINIMAP_PANEL_COLOR_TRITIUM 0x42
/* faction dots, + colorIndex (0 = unselected); RebuildPlane0 adds the panel paletteIndex * 4, not * bank size */
#define TERRAIN_MINIMAP_PANEL_COLOR_FACTION_FIRST 0x20

/* Functions are grouped by semantic ownership; address comments are executable virtual addresses. */

/* 0x0053D370 */
bool TerrainCompositeTexture_Create(uint32_t *outError);

/* 0x00503B10 */
bool TerrainByteClampLookup_Initialize(uint32_t *outError);

/* 0x00503F30 */
bool TerrainVisualResources_LoadPrimary
          (uint16_t *primaryResourcePath,uint16_t *secondaryResourcePath,FieldGridAsset *field,
          uint32_t *outError);

/* 0x005041C0 */
bool TerrainVisualResources_LoadAndClearCellOverlayFlags
          (uint16_t *primaryResourcePath,uint16_t *secondaryResourcePath,FieldGridAsset *field,
          uint32_t *outError);

/* 0x00504470 */
void TerrainVisualResources_Shutdown(void);

/* 0x00505780 */
void TerrainLighting_BuildColorRampAndSetBaseColor
          (PackedArgb32 secondaryColorArgb,PackedArgb32 baseColorArgb,PackedArgb32 rampStepColorArgb
          );

/* 0x0053D4D0 */
void TerrainCompositeTexture_Destroy(void);

/* 0x00561EA0 */
void TerrainLighting_AdjustDirectionAndRecomputeField
          (uint32_t playerRuntimeId,uint32_t reservedZero,uint32_t deltaElevationAngle,
          uint32_t deltaAzimuthAngle);

/* 0x0053D560 */
void TerrainCompositeTexture_FillPlane1(void);

/* 0x0053D680 */
void TerrainCompositeTexture_FillPlane2(void);

/* 0x0053D840 */
void TerrainCompositeTexture_RebuildPlane0(void);

#endif /* THANDOR_WORLD_TERRAIN_VISUALS_H */

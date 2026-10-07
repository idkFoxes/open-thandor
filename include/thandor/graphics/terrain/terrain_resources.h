/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/include/thandor/graphics/terrain/terrain_resources.h
 * Reverse engineering by idkFoxes 2026
 */

#ifndef THANDOR_GRAPHICS_TERRAIN_TERRAIN_RESOURCES_H
#define THANDOR_GRAPHICS_TERRAIN_TERRAIN_RESOURCES_H

#include <thandor/core/types.h>
#include <thandor/graphics/resources/types.h>
#include <thandor/world/terrain/types.h>
#include <thandor/core/contracts.h>

/* g_TerrainMaterialTextureSets: one texture set per terrain material, loaded from the secondary path with
   the suffix letter a..z (TerrainVisualResources_LoadPrimary). */
inline constexpr int TERRAIN_MATERIAL_TEXTURE_SET_COUNT = 26;
/* ".dat" extension code for WidePath_SetExtensionCode (see WIDE_PATH_EXTENSION_* in core/text/path.h) */
inline constexpr int WIDE_PATH_EXTENSION_DAT = 0x746164;
/* <primary>.dat / <secondary>.dat packet tables: a 0x20-byte header (first dword: phase seed bit width, read by
   FieldGrid_InitializeRuntimeCellsAndBoundaryFlags), then the 0x20-byte packets. g_TerrainSurfacePacketTablePayload / g_TerrainSoilPacketTablePayload
   point past the header; release subtracts it again. */
inline constexpr int TERRAIN_PACKET_TABLE_HEADER_BYTES = 0x20;
/* Random animation of the 256 terrain direction records (TerrainVisualResources_Load*): scale (sin/cos
   amplitude) MIN + (random & MASK), rotation rate +-(MIN + (random & MASK)) 16-bit angle units per step. */
inline constexpr int TERRAIN_DIRECTION_SCALE_MIN = 0x80;
inline constexpr int TERRAIN_DIRECTION_SCALE_RANDOM_MASK = 0x1f;
inline constexpr int TERRAIN_DIRECTION_RATE_MIN_ANGLE16 = 0x200;
inline constexpr int TERRAIN_DIRECTION_RATE_RANDOM_MASK = 0x7f;

Bool8 TerrainVisualResources_LoadPrimary
          (uint16_t *primaryResourcePath,uint16_t *secondaryResourcePath,FieldGridAsset *field,
          uint32_t *outError);

Bool8 TerrainVisualResources_LoadAndClearCellOverlayFlags
          (uint16_t *primaryResourcePath,uint16_t *secondaryResourcePath,FieldGridAsset *field,
          uint32_t *outError);

void TerrainVisualResources_Shutdown();

extern GraphicsTextureSet *g_TerrainPrimaryTextureSet;
extern void *g_TerrainSoilPacketTablePayload;
extern void *g_TerrainSurfacePacketTablePayload;
extern GraphicsPaletteAsset *g_TerrainPrimaryPalette;

extern GraphicsTextureSet *g_TerrainMaterialTextureSets[38]; /* one texture set per terrain material (26 used, TERRAIN_MATERIAL_COUNT); the remaining 12 entries are NULL. Original quirk: UiCommandMatrix_SelectIndex fills twelve swatches from a page base that can reach 15, so it reads entry 26 (always NULL, an empty swatch). */

extern GraphicsPaletteAsset *g_TerrainSecondaryPalette;

#endif /* THANDOR_GRAPHICS_TERRAIN_TERRAIN_RESOURCES_H */

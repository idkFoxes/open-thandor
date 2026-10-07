/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/include/thandor/ui/ingame/minimap_texture.h
 * Reverse engineering by idkFoxes 2026
 */

#ifndef THANDOR_UI_INGAME_MINIMAP_TEXTURE_H
#define THANDOR_UI_INGAME_MINIMAP_TEXTURE_H

#include <thandor/core/types.h>
#include <thandor/core/contracts.h>

/* TerrainCompositeTexture_Create: the minimap texture is a 0x200-byte gfx header, three 0x20-byte source entries
   and then three ARGB planes of gridWidth * gridHeight * 4 bytes each. */
inline constexpr int32_t TERRAIN_COMPOSITE_TEXTURE_HEADER_BYTES = 0x200; /* subresource table offset */
inline constexpr int32_t TERRAIN_COMPOSITE_TEXTURE_PIXELS_OFFSET = 0x260; /* dataOffset of plane 0 */
/* (argb & mask) >> 1 halves all four 8-bit channels at once (each channel's low bit cleared first); two
   halved pixels added give their average (TerrainCompositeTexture_RebuildPlane0) */
inline constexpr uint32_t TERRAIN_ARGB_HALVE_MASK = 0xfefefefe;
/* Minimap planes (TerrainCompositeTexture_FillPlane1/2): g_PackedLightingLookupTable levels. Dry cells use
   HEIGHT_LIGHT_FIRST + (terrainHeight >> 7) clamped to HEIGHT_LEVELS steps; water uses
   WATER_LIGHT_FIRST + WATER_DEPTH_LEVELS - (waterSurfaceDelta >> 5), clamped to WATER_DEPTH_LEVELS steps. */
inline constexpr int32_t TERRAIN_MINIMAP_HEIGHT_LIGHT_FIRST = 0x70;
inline constexpr int32_t TERRAIN_MINIMAP_HEIGHT_LEVELS = 0x60;
#define TERRAIN_MINIMAP_HEIGHT_LIGHT_LAST \
          (TERRAIN_MINIMAP_HEIGHT_LIGHT_FIRST + TERRAIN_MINIMAP_HEIGHT_LEVELS - 1) /* 0xCF */
inline constexpr int32_t TERRAIN_MINIMAP_WATER_LIGHT_FIRST = 0x80; /* deepest water */
inline constexpr int32_t TERRAIN_MINIMAP_WATER_DEPTH_LEVELS = 0x40;
#define TERRAIN_MINIMAP_WATER_LIGHT_LAST \
          (TERRAIN_MINIMAP_WATER_LIGHT_FIRST + TERRAIN_MINIMAP_WATER_DEPTH_LEVELS - 1) /* 0xBF, shallowest */
/* Entries of the in-game panel palette bank (panel subresource 36) used for the minimap */
inline constexpr int32_t TERRAIN_MINIMAP_PANEL_COLOR_SOIL = 0x40;
inline constexpr int32_t TERRAIN_MINIMAP_PANEL_COLOR_XENITE = 0x41;
inline constexpr int32_t TERRAIN_MINIMAP_PANEL_COLOR_TRITIUM = 0x42;
/* faction dots, + colorIndex (0 = unselected); RebuildPlane0 adds the panel paletteIndex * 4, not * bank size */
inline constexpr int32_t TERRAIN_MINIMAP_PANEL_COLOR_FACTION_FIRST = 0x20;

bool TerrainCompositeTexture_Create(uint32_t *outError);

void TerrainCompositeTexture_Destroy();

void TerrainCompositeTexture_FillPlane1();

void TerrainCompositeTexture_FillPlane2();

void TerrainCompositeTexture_RebuildPlane0();

#endif /* THANDOR_UI_INGAME_MINIMAP_TEXTURE_H */

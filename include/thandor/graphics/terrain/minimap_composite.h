/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/include/thandor/graphics/terrain/minimap_composite.h
 * Reverse engineering by idkFoxes 2026
 */

#ifndef THANDOR_GRAPHICS_TERRAIN_MINIMAP_COMPOSITE_H
#define THANDOR_GRAPHICS_TERRAIN_MINIMAP_COMPOSITE_H

#include <thandor/generated/types.h>
#include <thandor/core/contracts.h>

/* TerrainCompositeTexture_Create: the minimap texture is a 0x200-byte gfx header, three 0x20-byte source entries
   and then three ARGB planes of gridWidth * gridHeight * 4 bytes each. */
#define TERRAIN_COMPOSITE_TEXTURE_HEADER_BYTES 0x200 /* subresource table offset */
#define TERRAIN_COMPOSITE_TEXTURE_PIXELS_OFFSET 0x260 /* dataOffset of plane 0 */
/* (argb & mask) >> 1 halves all four 8-bit channels at once (each channel's low bit cleared first); two
   halved pixels added give their average (TerrainCompositeTexture_RebuildPlane0) */
#define TERRAIN_ARGB_HALVE_MASK 0xfefefefe
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

Bool8 TerrainCompositeTexture_Create(uint32_t *outError);

void TerrainCompositeTexture_Destroy(void);

void TerrainCompositeTexture_FillPlane1(void);

void TerrainCompositeTexture_FillPlane2(void);

void TerrainCompositeTexture_RebuildPlane0(void);

#endif /* THANDOR_GRAPHICS_TERRAIN_MINIMAP_COMPOSITE_H */

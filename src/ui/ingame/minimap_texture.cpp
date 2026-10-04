/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/src/ui/ingame/minimap_texture.cpp
 * Reverse engineering by idkFoxes 2026
 */

/* The minimap composite texture of the field grid: a gfx asset with three ARGB planes (terrain/water
   colours, panel colours, faction presence) built from the cells. */

#include <thandor/ui/ingame/minimap_texture.h>
#include <thandor/thandor.h>
#include <thandor/core/color_lanes.h>
#include <thandor/platform/bootstrap/image.h>

/* Module data. */

static TerrainCompositeTextureRuntime *g_TerrainCompositeTexture = nullptr;

/* Adds each byte of pixel to the matching 16-bit lane of words and halves the sum: per channel the average of
   the new colour and the pixel already in the plane (used to blend water over the ground colour). */
static inline uint64_t TerrainColor_AverageWordsWithPixelBytes(uint64_t words,uint32_t pixel)

{
  ThandorMmx lanes;
  int lane;

  lanes.q = words;
  for (lane = 0; lane < 4; lane++) {
    lanes.uw[lane] = (uint16_t)(lanes.uw[lane] + (pixel >> (lane * 8) & 0xff)) >> 1;
  }
  return lanes.q;
}

/* Builds the terrain composite texture for the current field grid: an in-memory gfx asset with three direct-colour
   ARGB images of one pixel per field cell, published in g_TerrainCompositeTexture and the in-game root. Fills
   planes 1 and 2 and then derives plane 0 from them. Returns true on success; on failure returns false and
   stores the allocator error in *outError (untouched on success).
*/
Bool8 TerrainCompositeTexture_Create(uint32_t *outError)

{
  FieldGridAsset *terrainFieldGrid;
  AssetDimension fieldWidth;
  AssetDimension fieldHeight;
  InGameRuntimeRoot *inGameRoot;
  AssetRelativeOffset plane2DataOffset;
  uint32_t totalImageBytes;
  int planeSizeBytes;
  uint32_t allocError;
  TerrainCompositeTextureRuntime *compositeTexture;
  
  inGameRoot = g_InGameRuntimeRoot;
  terrainFieldGrid = (g_InGameRuntimeRoot->worldRuntime).fieldGrid;
  fieldWidth = terrainFieldGrid->gridWidth;
  fieldHeight = terrainFieldGrid->gridHeight;
  /* layout: 0x200-byte gfx header, three 0x20-byte source entries (pixels from 0x260), three planes of
     width * height * 4 bytes */
  allocError = g_MemoryApi.alloc(fieldWidth * (3 * 4) * fieldHeight + TERRAIN_COMPOSITE_TEXTURE_PIXELS_OFFSET,
                                 (void **)&compositeTexture);
  if (allocError != 0) {
    *outError = allocError;
    return false;
  }
  g_TerrainCompositeTexture = compositeTexture;
  inGameRoot->minimapTextureSource = compositeTexture;
  /* paletteIndex -1: direct ARGB colour, no palette */
  compositeTexture->sourceEntries[0].pixelWidth = fieldWidth;
  compositeTexture->sourceEntries[0].pixelHeight = fieldHeight;
  compositeTexture->sourceEntries[0].originX = 0;
  compositeTexture->sourceEntries[0].originY = 0;
  planeSizeBytes = fieldWidth * 4 * fieldHeight;
  compositeTexture->sourceEntries[0].paletteIndex = -1;
  compositeTexture->sourceEntries[0].logicalWidth = fieldWidth;
  compositeTexture->sourceEntries[0].logicalHeight = fieldHeight;
  compositeTexture->sourceEntries[0].dataOffset = TERRAIN_COMPOSITE_TEXTURE_PIXELS_OFFSET;
  compositeTexture->sourceEntries[1].pixelWidth = fieldWidth;
  compositeTexture->sourceEntries[1].pixelHeight = fieldHeight;
  compositeTexture->sourceEntries[1].originX = 0;
  compositeTexture->sourceEntries[1].originY = 0;
  compositeTexture->sourceEntries[1].paletteIndex = -1;
  compositeTexture->sourceEntries[1].logicalWidth = fieldWidth;
  compositeTexture->sourceEntries[1].logicalHeight = fieldHeight;
  compositeTexture->sourceEntries[1].dataOffset = planeSizeBytes + (uint32_t)TERRAIN_COMPOSITE_TEXTURE_PIXELS_OFFSET;
  compositeTexture->sourceEntries[2].pixelWidth = fieldWidth;
  compositeTexture->sourceEntries[2].pixelHeight = fieldHeight;
  compositeTexture->sourceEntries[2].originX = 0;
  compositeTexture->sourceEntries[2].originY = 0;
  plane2DataOffset = planeSizeBytes + (uint32_t)TERRAIN_COMPOSITE_TEXTURE_PIXELS_OFFSET + planeSizeBytes;
  compositeTexture->sourceEntries[2].paletteIndex = -1;
  compositeTexture->sourceEntries[2].logicalWidth = fieldWidth;
  compositeTexture->sourceEntries[2].logicalHeight = fieldHeight;
  compositeTexture->sourceEntries[2].dataOffset = plane2DataOffset;
  (compositeTexture->textureSource).common.magic = ASSET_MAGIC_GFX;
  totalImageBytes = plane2DataOffset + planeSizeBytes;
  (compositeTexture->textureSource).tableDescriptor.subresourceCount = 3;
  (compositeTexture->textureSource).tableDescriptor.paletteBankCount = 0;
  (compositeTexture->textureSource).tableDescriptor.subresourceTableOffset = TERRAIN_COMPOSITE_TEXTURE_HEADER_BYTES;
  (compositeTexture->textureSource).unusedHeaderDwordBC = 0;
  (compositeTexture->textureSource).common.allocationSizeBytes = totalImageBytes;
  TerrainCompositeTexture_FillPlane1();
  TerrainCompositeTexture_FillPlane2();
  TerrainCompositeTexture_RebuildPlane0();
  return true;
}

/* Frees the terrain composite texture built by TerrainCompositeTexture_Create (through its allocation base).
   g_TerrainCompositeTexture and the in-game root keep the stale pointer.
*/
void TerrainCompositeTexture_Destroy()

{
  GraphicsTextureSourceAsset *allocationBase;

  allocationBase = g_GraphicsTextureSourceResolveAllocationBase
                     (&g_TerrainCompositeTexture->textureSource);
  g_MemoryApi.free(allocationBase);
}

/* Renders plane 1 of the terrain composite texture (the minimap image, one ARGB pixel per field cell):
   dry cells get their material's panel colour shaded by terrain height, flooded cells the water colour
   (palette entry 0) shaded by water depth, both through g_PackedLightingLookupTable.
*/
void TerrainCompositeTexture_FillPlane1()

{
  AssetDimension textureWidth;
  int panelSubresourceIndex;
  uint32_t materialColorArgb;
  PackedArgb32 waterColorArgb;
  GraphicsTextureSourceAsset *panelTextureSource;
  int lightingLevelIndex;
  AssetDimension columnsRemaining;
  uint8_t *planePixelCursor;
  FieldGridCell *fieldCell;
  uint64_t litGroundWords;
  uint64_t litWaterWords;
  AssetDimension rowsRemaining;
  
  panelTextureSource = g_InGamePanelTextureSource;
  textureWidth = g_TerrainCompositeTexture->sourceEntries[0].pixelWidth;
  rowsRemaining = g_TerrainCompositeTexture->sourceEntries[0].pixelHeight;
  planePixelCursor = (uint8_t *)g_TerrainCompositeTexture + g_TerrainCompositeTexture->sourceEntries[1].dataOffset;
  panelSubresourceIndex = ((GraphicsTextureSourceEntry *)((uint8_t *)g_InGamePanelTextureSource + (g_InGamePanelTextureSource->tableDescriptor).subresourceTableOffset))[36].paletteIndex;
  fieldCell = ((g_InGameRuntimeRoot->worldRuntime).fieldGrid)->cells;
  columnsRemaining = textureWidth;
  do {
    do {
      if (fieldCell->waterSurfaceDelta < 1) {
        lightingLevelIndex = fieldCell->terrainHeight >> 7; /* height levels 0x70..0xCF */
        materialColorArgb = ((GraphicsPaletteTextureSourceAsset *)panelTextureSource)->paletteEntries[panelSubresourceIndex * GRAPHICS_PALETTE_BANK_ENTRIES + (fieldCell->flagsAndMaterial & FIELD_CELL_MATERIAL_ID_MASK)].argb8888;
        if (lightingLevelIndex < 0) {
          lightingLevelIndex = TERRAIN_MINIMAP_HEIGHT_LIGHT_FIRST;
        }
        else if (lightingLevelIndex < TERRAIN_MINIMAP_HEIGHT_LEVELS) {
          lightingLevelIndex = lightingLevelIndex + TERRAIN_MINIMAP_HEIGHT_LIGHT_FIRST;
        }
        else {
          lightingLevelIndex = TERRAIN_MINIMAP_HEIGHT_LIGHT_LAST;
        }
        /* channels widened to words and shifted right by 3, scaled by the lighting level, packed with saturation */
        litGroundWords =
             pmulhw(ColorLanes_UnpackBytesShiftRight(materialColorArgb,3),
                    g_PackedLightingLookupTable[lightingLevelIndex]);
        *(uint32_t *)planePixelCursor = ColorLanes_PackWordsUnsignedSaturate(litGroundWords);
      }
      else {
        lightingLevelIndex = -fieldCell->waterSurfaceDelta >> 5; /* depth levels 0xBF down to 0x80 */
        waterColorArgb = g_TerrainPrimaryPalette->paletteEntries[0].argb8888;
        if (lightingLevelIndex < 0) {
          if (lightingLevelIndex < -(TERRAIN_MINIMAP_WATER_DEPTH_LEVELS - 1)) {
            lightingLevelIndex = TERRAIN_MINIMAP_WATER_LIGHT_FIRST;
          }
          else {
            lightingLevelIndex = lightingLevelIndex + (TERRAIN_MINIMAP_WATER_LIGHT_FIRST + TERRAIN_MINIMAP_WATER_DEPTH_LEVELS);
          }
        }
        else {
          lightingLevelIndex = TERRAIN_MINIMAP_WATER_LIGHT_LAST;
        }
        litWaterWords =
             pmulhw(ColorLanes_UnpackBytesShiftRight(waterColorArgb,3),
                    g_PackedLightingLookupTable[lightingLevelIndex]);
        *(uint32_t *)planePixelCursor = ColorLanes_PackWordsUnsignedSaturate(litWaterWords);
      }
      fieldCell++;
      planePixelCursor = planePixelCursor + 4;
      columnsRemaining--;
    } while (columnsRemaining != 0);
    rowsRemaining--;
    columnsRemaining = textureWidth;
  } while (rowsRemaining != 0);
}

/* Renders plane 2 of the terrain composite texture, the resource view of the minimap: Xenite, Tritium and
   plain soil cells get their panel colours shaded by terrain height, and water is blended 50/50 over
   flooded cells.
*/
void TerrainCompositeTexture_FillPlane2()

{
  AssetDimension textureWidth;
  int panelSubresourceIndex;
  AssetFormatVersion xeniteColorArgb;
  AssetPackedDate tritiumColorArgb;
  AssetMagic soilColorArgb;
  PackedArgb32 waterColorArgb;
  uint32_t existingPixelArgb;
  GraphicsTextureSourceAsset *panelTextureSource;
  int lightingLevelIndex;
  AssetDimension columnsRemaining;
  uint8_t *planePixelCursor;
  FieldGridCell *fieldCell;
  uint64_t litXeniteWords;
  uint64_t litTritiumWords;
  uint64_t litSoilWords;
  uint64_t litWaterWords;
  AssetDimension rowsRemaining;
  
  panelTextureSource = g_InGamePanelTextureSource;
  textureWidth = g_TerrainCompositeTexture->sourceEntries[0].pixelWidth;
  rowsRemaining = g_TerrainCompositeTexture->sourceEntries[0].pixelHeight;
  planePixelCursor = (uint8_t *)g_TerrainCompositeTexture + g_TerrainCompositeTexture->sourceEntries[2].dataOffset;
  panelSubresourceIndex = ((GraphicsTextureSourceEntry *)((uint8_t *)g_InGamePanelTextureSource + (g_InGamePanelTextureSource->tableDescriptor).subresourceTableOffset))[36].paletteIndex;
  fieldCell = ((g_InGameRuntimeRoot->worldRuntime).fieldGrid)->cells;
  columnsRemaining = textureWidth;
  do {
    do {
      if ((fieldCell->flagsAndMaterial & FIELD_CELL_XENITE_SUPPORT) == 0) {
        if ((fieldCell->flagsAndMaterial & FIELD_CELL_TRITIUM_SUPPORT) == 0) {
          lightingLevelIndex = fieldCell->terrainHeight >> 7; /* height levels 0x70..0xCF */
          soilColorArgb = ((GraphicsPaletteTextureSourceAsset *)panelTextureSource)->paletteEntries[panelSubresourceIndex * GRAPHICS_PALETTE_BANK_ENTRIES + TERRAIN_MINIMAP_PANEL_COLOR_SOIL].argb8888;
          if (lightingLevelIndex < 0) {
            lightingLevelIndex = TERRAIN_MINIMAP_HEIGHT_LIGHT_FIRST;
          }
          else if (lightingLevelIndex < TERRAIN_MINIMAP_HEIGHT_LEVELS) {
            lightingLevelIndex = lightingLevelIndex + TERRAIN_MINIMAP_HEIGHT_LIGHT_FIRST;
          }
          else {
            lightingLevelIndex = TERRAIN_MINIMAP_HEIGHT_LIGHT_LAST;
          }
          litSoilWords =
               pmulhw(ColorLanes_UnpackBytesShiftRight(soilColorArgb,3),
                      g_PackedLightingLookupTable[lightingLevelIndex]);
          *(uint32_t *)planePixelCursor = ColorLanes_PackWordsUnsignedSaturate(litSoilWords);
        }
        else {
          lightingLevelIndex = fieldCell->terrainHeight >> 7; /* height levels 0x70..0xCF */
          tritiumColorArgb = ((GraphicsPaletteTextureSourceAsset *)panelTextureSource)->paletteEntries[panelSubresourceIndex * GRAPHICS_PALETTE_BANK_ENTRIES + TERRAIN_MINIMAP_PANEL_COLOR_TRITIUM].argb8888;
          if (lightingLevelIndex < 0) {
            lightingLevelIndex = TERRAIN_MINIMAP_HEIGHT_LIGHT_FIRST;
          }
          else if (lightingLevelIndex < TERRAIN_MINIMAP_HEIGHT_LEVELS) {
            lightingLevelIndex = lightingLevelIndex + TERRAIN_MINIMAP_HEIGHT_LIGHT_FIRST;
          }
          else {
            lightingLevelIndex = TERRAIN_MINIMAP_HEIGHT_LIGHT_LAST;
          }
          litTritiumWords =
               pmulhw(ColorLanes_UnpackBytesShiftRight(tritiumColorArgb,3),
                      g_PackedLightingLookupTable[lightingLevelIndex]);
          *(uint32_t *)planePixelCursor = ColorLanes_PackWordsUnsignedSaturate(litTritiumWords);
        }
      }
      else {
        lightingLevelIndex = fieldCell->terrainHeight >> 7; /* height levels 0x70..0xCF */
        xeniteColorArgb = ((GraphicsPaletteTextureSourceAsset *)panelTextureSource)->paletteEntries[panelSubresourceIndex * GRAPHICS_PALETTE_BANK_ENTRIES + TERRAIN_MINIMAP_PANEL_COLOR_XENITE].argb8888;
        if (lightingLevelIndex < 0) {
          lightingLevelIndex = TERRAIN_MINIMAP_HEIGHT_LIGHT_FIRST;
        }
        else if (lightingLevelIndex < TERRAIN_MINIMAP_HEIGHT_LEVELS) {
          lightingLevelIndex = lightingLevelIndex + TERRAIN_MINIMAP_HEIGHT_LIGHT_FIRST;
        }
        else {
          lightingLevelIndex = TERRAIN_MINIMAP_HEIGHT_LIGHT_LAST;
        }
        litXeniteWords =
             pmulhw(ColorLanes_UnpackBytesShiftRight(xeniteColorArgb,3),
                    g_PackedLightingLookupTable[lightingLevelIndex]);
        *(uint32_t *)planePixelCursor = ColorLanes_PackWordsUnsignedSaturate(litXeniteWords);
      }
      if (0 < fieldCell->waterSurfaceDelta) {
        lightingLevelIndex = -fieldCell->waterSurfaceDelta >> 5; /* depth levels 0xBF down to 0x80 */
        waterColorArgb = g_TerrainPrimaryPalette->paletteEntries[0].argb8888;
        if (lightingLevelIndex < 0) {
          if (lightingLevelIndex < -(TERRAIN_MINIMAP_WATER_DEPTH_LEVELS - 1)) {
            lightingLevelIndex = TERRAIN_MINIMAP_WATER_LIGHT_FIRST;
          }
          else {
            lightingLevelIndex = lightingLevelIndex + (TERRAIN_MINIMAP_WATER_LIGHT_FIRST + TERRAIN_MINIMAP_WATER_DEPTH_LEVELS);
          }
        }
        else {
          lightingLevelIndex = TERRAIN_MINIMAP_WATER_LIGHT_LAST;
        }
        existingPixelArgb = *(uint32_t *)planePixelCursor;
        litWaterWords =
             pmulhw(ColorLanes_UnpackBytesShiftRight(waterColorArgb,3),
                    g_PackedLightingLookupTable[lightingLevelIndex]);
        /* averaged with the pixel already in the plane, packed with saturation */
        *(uint32_t *)planePixelCursor =
             ColorLanes_PackWordsUnsignedSaturate
                       (TerrainColor_AverageWordsWithPixelBytes(litWaterWords,existingPixelArgb));
      }
      fieldCell++;
      planePixelCursor = planePixelCursor + 4;
      columnsRemaining--;
    } while (columnsRemaining != 0);
    rowsRemaining--;
    columnsRemaining = textureWidth;
  } while (rowsRemaining != 0);
}

/* Builds the displayed minimap (plane 0): copies plane 1 (terrain) or, with bit 1 of
   minimapResourceButtonStateFlags, plane 2 (resources), hides cells the active faction has never seen
   (almost black) and darkens those it does not see now, then draws a pixel for each model runtime with an
   alpha tint whose faction has a non-zero colorIndex, in the panel colour of variant
   colorIndex, or variant 0 (white) for units in the local selection, so the selection stands out (blended 50/50 for
   a tint alpha below 0xFF).
*/
void TerrainCompositeTexture_RebuildPlane0()

{
  uint8_t visibilityFlags;
  AssetDimension textureWidth;
  AssetDimension textureHeight;
  WorldOwnerListNode *ownerNode;
  GameEntityRuntime *ownerEntity;
  InGameRuntimeRoot *inGameRoot;
  GraphicsTextureSourceAsset *panelTextureSource;
  uint32_t pixelArgb;
  int cellCount;
  int cellsRemaining;
  int copyRemaining;
  FactionRuntimeIndex activeFactionIndex;
  int gridColumn;
  int gridRow;
  int64_t roundedRowQ12;
  Bool8 rowRoundingOverflows;
  uint32_t colorVariant;
  AssetRelativeOffset assetOffset;
  uint8_t *pixelCursor;
  FieldGridCell *fieldCell;
  uint8_t *plane0Pixels;
  uint8_t *plane0WriteCursor;
  Bool8 notSelected;
  FieldGridCoordinates gridCoordinates;

  inGameRoot = g_InGameRuntimeRoot;
  if ((g_InGameRuntimeRoot->minimapResourceButtonStateFlags & UI_SELECTABLE_SELECTED_OR_CHECKED) == 0) {
    assetOffset = g_TerrainCompositeTexture->sourceEntries[1].dataOffset;
  }
  else {
    assetOffset = g_TerrainCompositeTexture->sourceEntries[2].dataOffset;
  }
  textureWidth = g_TerrainCompositeTexture->sourceEntries[0].pixelWidth;
  textureHeight = g_TerrainCompositeTexture->sourceEntries[0].pixelHeight;
  plane0Pixels = (uint8_t *)g_TerrainCompositeTexture + g_TerrainCompositeTexture->sourceEntries[0].dataOffset;
  cellCount = textureWidth * textureHeight;
  pixelCursor = (uint8_t *)g_TerrainCompositeTexture + assetOffset;
  plane0WriteCursor = plane0Pixels;
  for (copyRemaining = cellCount; copyRemaining != 0; copyRemaining--) {
    *(uint32_t *)plane0WriteCursor = *(uint32_t *)pixelCursor;
    pixelCursor = pixelCursor + 4;
    plane0WriteCursor = plane0WriteCursor + 4;
  }
  activeFactionIndex = (inGameRoot->worldRuntime).activeFactionRuntimeIndex;
  fieldCell = ((inGameRoot->worldRuntime).fieldGrid)->cells;
  pixelCursor = plane0Pixels;
  cellsRemaining = cellCount;
  do {
    visibilityFlags = ((uint8_t *)&fieldCell->occupancyMask)[activeFactionIndex];
    pixelArgb = (uint32_t)visibilityFlags;
    if ((visibilityFlags & FIELD_CELL_OCCUPANCY_CURRENT_PRESENCE_BITS) == 0) {
      if ((visibilityFlags & FIELD_CELL_OCCUPANCY_PRESENCE_BITS) != 0) {
        /* seen before: halve every channel */
        pixelArgb = (*(uint32_t *)pixelCursor & TERRAIN_ARGB_HALVE_MASK) >> 1;
      }
      *(uint32_t *)pixelCursor = pixelArgb;
    }
    fieldCell++;
    pixelCursor = pixelCursor + 4;
    cellsRemaining--;
  } while (cellsRemaining != 0);
  for (ownerNode = (inGameRoot->worldRuntime).ownerListHead; ownerNode != nullptr;
      ownerNode = ownerNode->nextNode) {
    if ((ownerNode->ownerClassId == WORLD_OWNER_RUNTIME_MODEL) && (0xffffff < ownerNode->modelTintArgb)) {
      gridCoordinates = FieldGrid_WorldToGridQ12(ownerNode->worldYQ12,ownerNode->worldXQ12);
      panelTextureSource = g_InGamePanelTextureSource;
      /* round to the nearest cell */
      gridColumn = (gridCoordinates.columnQ12 + FIELD_GRID_CELL_Q12 / 2) >> Q12_SHIFT;
      gridRow = (gridCoordinates.rowQ12 + FIELD_GRID_CELL_Q12 / 2) >> Q12_SHIFT;
      /* exact (non-wrapping) row + half a cell. In the original the column's sign test also depends on whether
         the row rounding addition overflows, so it is "column >= 0" unless that addition overflows. */
      roundedRowQ12 = (int64_t)gridCoordinates.rowQ12 + FIELD_GRID_CELL_Q12 / 2;
      rowRoundingOverflows = INT32_MAX < roundedRowQ12;
      if (((gridColumn < 0) == rowRoundingOverflows) && (roundedRowQ12 >= 0) &&
          (gridColumn < (int)textureWidth) && (gridRow < (int)textureHeight)) {
        ownerEntity = (GameEntityRuntime *)((ModelRuntimeSlot *)ownerNode->runtimePayload)->ownerArmyRuntimeOrSavedOffset.armyRuntime;
        colorVariant = g_GameFactionRuntimeImage.records[(ownerEntity->common).ownership.ownerIndex].
                 colorIndex;
        assetOffset = (g_InGamePanelTextureSource->tableDescriptor).subresourceTableOffset;
        if (colorVariant != 0) {
          /* SelectionInfo_IsEntryAbsent is true when the entry is NOT in the selection */
          notSelected = SelectionInfo_IsEntryAbsent(ownerEntity);
          if (!notSelected) {
            colorVariant = 0;
          }
          /* Original quirk: the bank is scaled by 4 entries (paletteIndex * 4), not by a whole
             bank like in FillPlane1/FillPlane2, so only bank 0 gives the right colour. Harmless:
             subresource 36 of panel0/1/2.gfx uses bank 0. */
          pixelArgb = ((GraphicsPaletteTextureSourceAsset *)g_InGamePanelTextureSource)->paletteEntries[(int32_t)(TERRAIN_MINIMAP_PANEL_COLOR_FACTION_FIRST +((GraphicsTextureSourceEntry *)((uint8_t *)panelTextureSource + assetOffset))[36].paletteIndex * 4 + colorVariant)].argb8888;
          if (ownerNode->modelTintArgb < ARGB8888_ALPHA_MASK) {
            pixelArgb = ((pixelArgb & TERRAIN_ARGB_HALVE_MASK) +
                         (*(uint32_t *)(plane0Pixels + (int32_t)((gridRow * textureWidth + gridColumn) * 4)) &
                          TERRAIN_ARGB_HALVE_MASK)) >> 1;
          }
          *(uint32_t *)(plane0Pixels + (int32_t)((gridRow * textureWidth + gridColumn) * 4)) = pixelArgb;
        }
      }
    }
  }
}

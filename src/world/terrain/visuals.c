/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/src/world/terrain/visuals.c
 * Reverse engineering by idkFoxes 2026
 */

#include <thandor/world/terrain/visuals.h>
#include <thandor/thandor.h>
#include <thandor/platform/bootstrap/image.h>

/* Implementation ownership: world/terrain/visuals. */

/* PUNPCKLBW mm,mm then PSRLW mm,shift: the four bytes b of value as the words ((b << 8) | b) >> shift.
   Spreads an ARGB colour into four 16-bit channels for the PMULHW shading in the composite texture fill. */
static __inline uint64_t TerrainColor_UnpackBytesShiftRight(uint32_t value,int shift)

{
  ThandorMmx lanes;
  int lane;

  for (lane = 0; lane < 4; lane++) {
    lanes.uw[lane] = (uint16_t)(((value >> (lane * 8) & 0xff) * 0x101) >> shift);
  }
  return lanes.q;
}

/* PACKUSWB mm,mm (low dword): the four signed words saturated to unsigned bytes, i.e. four shaded 16-bit
   channels packed back into one ARGB pixel. */
static __inline uint32_t TerrainColor_PackWordsUnsignedSaturate(uint64_t words)

{
  ThandorMmx lanes;
  uint32_t packed;
  int lane;

  lanes.q = words;
  packed = 0;
  for (lane = 0; lane < 4; lane++) {
    packed = packed |
             (uint32_t)(lanes.sw[lane] < 0 ? 0 : (0xff < lanes.sw[lane] ? 0xff : lanes.sw[lane])) << (lane * 8);
  }
  return packed;
}

/* PUNPCKLBW/PSRLW 8 of pixel (its bytes as words), PADDW to words, then PSRLW 1: per channel the average of the
   new colour and the pixel already in the plane (used to blend water over the ground colour). */
static __inline uint64_t TerrainColor_AverageWordsWithPixelBytes(uint64_t words,uint32_t pixel)

{
  ThandorMmx lanes;
  int lane;

  lanes.q = words;
  for (lane = 0; lane < 4; lane++) {
    lanes.uw[lane] = (uint16_t)(lanes.uw[lane] + (pixel >> (lane * 8) & 0xff)) >> 1;
  }
  return lanes.q;
}

/* Address: 0x0053D370.
   Builds the terrain composite texture for the current field grid: an in-memory gfx asset with three direct-colour
   ARGB images of one pixel per field cell, published in g_TerrainCompositeTexture and the in-game root. Fills
   planes 1 and 2 and then derives plane 0 from them. Returns the total image size, or the allocator error with
   CF set.
*/
StatusResult TerrainCompositeTexture_Create(void)

{
  FieldGridAsset *terrainFieldGrid;
  AssetDimension fieldWidth;
  AssetDimension fieldHeight;
  InGameRuntimeRoot *inGameRoot;
  AssetRelativeOffset plane2DataOffset;
  uint32_t totalImageBytes;
  int planeSizeBytes;
  ArenaAllocResult allocResult;
  StatusResult resultStatus;
  TerrainCompositeTextureRuntime *compositeTexture;
  
  inGameRoot = g_InGameRuntimeRoot;
  terrainFieldGrid = (g_InGameRuntimeRoot->worldRuntime).fieldGrid;
  fieldWidth = terrainFieldGrid->gridWidth;
  fieldHeight = terrainFieldGrid->gridHeight;
  /* layout: 0x200-byte gfx header, three 0x20-byte source entries (pixels from 0x260), three planes of
     width * height * 4 bytes */
  allocResult = g_MemoryApi.alloc(fieldWidth * (3 * 4) * fieldHeight + TERRAIN_COMPOSITE_TEXTURE_PIXELS_OFFSET);
  compositeTexture = (TerrainCompositeTextureRuntime *)allocResult.payloadOrError;
  if (!allocResult.failed) {
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
    allocResult.failed = false;
    allocResult.payloadOrError = totalImageBytes;
  }
  resultStatus.valueOrError = allocResult.payloadOrError;
  resultStatus.failed = allocResult.failed;
  return resultStatus;
}


/* Address: 0x00503B10.
   Builds g_TerrainByteClampLookup, the 64-KiB table FieldGrid_ApplyByteClampLookupToCells uses every few ticks to
   fade each cell's runtime byte (+0x68) one step (TERRAIN_RUNTIME_BYTE_FADE_STEP) towards the level its occupancy
   byte asks for (row targets: see TERRAIN_BYTE_CLAMP_LOOKUP_BYTES). The table is 64-KiB aligned so the original
   can index it by loading the two bytes into AH/AL. CF set with the allocator error on failure.
*/
StatusResult TerrainByteClampLookup_Initialize(void)

{
  void *lookupAllocationBase;
  int lookupRowsRemaining;
  int finalRowsRemaining;
  uint8_t nextInputByte;
  uint32_t clampInputValue;
  uint32_t lookupInputValue;
  uint8_t *lookupWriteCursor;
  ArenaAllocResult allocResult;

  /* twice the size, so a 64-KiB aligned table fits inside */
  allocResult = g_MemoryApi.alloc(TERRAIN_BYTE_CLAMP_LOOKUP_BYTES * 2);
  lookupAllocationBase = (void *)allocResult.payloadOrError;
  if (allocResult.failed) {
    return StatusValue_Fail(allocResult.payloadOrError);
  }
  clampInputValue = 0;
  lookupWriteCursor = (uint8_t *)((int)lookupAllocationBase + 0xffffU & 0xffff0000);
  lookupRowsRemaining = 0x40;
  g_TerrainByteClampLookup = lookupWriteCursor;
  /* rows 0x00..0x7F in pairs: even rows fade to NONE, odd rows to FULL */
  do {
    do {
      if (clampInputValue == 0) {
        *lookupWriteCursor = TERRAIN_RUNTIME_BYTE_LEVEL_NONE;
      }
      else if ((int)(clampInputValue - TERRAIN_RUNTIME_BYTE_FADE_STEP) < 1) {
        *lookupWriteCursor = TERRAIN_RUNTIME_BYTE_LEVEL_NONE;
      }
      else {
        *lookupWriteCursor = (uint8_t)(clampInputValue - TERRAIN_RUNTIME_BYTE_FADE_STEP);
      }
      lookupWriteCursor++;
      nextInputByte = (char)clampInputValue + 1;
      clampInputValue = (uint32_t)nextInputByte;
    } while (nextInputByte != 0);
    lookupInputValue = 0;
    do {
      /* a byte is never above 0xFF: only the rising branch is reached */
      if (lookupInputValue < 256) {
        if (lookupInputValue + TERRAIN_RUNTIME_BYTE_FADE_STEP < TERRAIN_RUNTIME_BYTE_LEVEL_FULL) {
          *lookupWriteCursor = (uint8_t)(lookupInputValue + TERRAIN_RUNTIME_BYTE_FADE_STEP);
        }
        else {
          *lookupWriteCursor = TERRAIN_RUNTIME_BYTE_LEVEL_FULL;
        }
      }
      else if ((int)(lookupInputValue - TERRAIN_RUNTIME_BYTE_FADE_STEP) < 256) {
        *lookupWriteCursor = TERRAIN_RUNTIME_BYTE_LEVEL_FULL;
      }
      else {
        *lookupWriteCursor = (uint8_t)(lookupInputValue - TERRAIN_RUNTIME_BYTE_FADE_STEP);
      }
      lookupWriteCursor++;
      nextInputByte = (char)lookupInputValue + 1;
      lookupInputValue = (uint32_t)nextInputByte;
    } while (nextInputByte != 0);
    lookupRowsRemaining--;
    clampInputValue = 0;
  } while (lookupRowsRemaining != 0);
  /* row 0x80: fade to PERSISTENT from either side */
  lookupInputValue = 0;
  do {
    if (lookupInputValue < TERRAIN_RUNTIME_BYTE_LEVEL_PERSISTENT + 1) {
      if (lookupInputValue + TERRAIN_RUNTIME_BYTE_FADE_STEP < TERRAIN_RUNTIME_BYTE_LEVEL_PERSISTENT) {
        *lookupWriteCursor = (uint8_t)(lookupInputValue + TERRAIN_RUNTIME_BYTE_FADE_STEP);
      }
      else {
        *lookupWriteCursor = TERRAIN_RUNTIME_BYTE_LEVEL_PERSISTENT;
      }
    }
    else if ((int)(lookupInputValue - TERRAIN_RUNTIME_BYTE_FADE_STEP) < TERRAIN_RUNTIME_BYTE_LEVEL_PERSISTENT + 1) {
      *lookupWriteCursor = TERRAIN_RUNTIME_BYTE_LEVEL_PERSISTENT;
    }
    else {
      *lookupWriteCursor = (uint8_t)(lookupInputValue - TERRAIN_RUNTIME_BYTE_FADE_STEP);
    }
    lookupWriteCursor++;
    nextInputByte = (char)lookupInputValue + 1;
    lookupInputValue = (uint32_t)nextInputByte;
  } while (nextInputByte != 0);
  /* row 0x81: FULL */
  lookupInputValue = 0;
  do {
    if (lookupInputValue < 256) {
      if (lookupInputValue + TERRAIN_RUNTIME_BYTE_FADE_STEP < TERRAIN_RUNTIME_BYTE_LEVEL_FULL) {
        *lookupWriteCursor = (uint8_t)(lookupInputValue + TERRAIN_RUNTIME_BYTE_FADE_STEP);
      }
      else {
        *lookupWriteCursor = TERRAIN_RUNTIME_BYTE_LEVEL_FULL;
      }
    }
    else if ((int)(lookupInputValue - TERRAIN_RUNTIME_BYTE_FADE_STEP) < 256) {
      *lookupWriteCursor = TERRAIN_RUNTIME_BYTE_LEVEL_FULL;
    }
    else {
      *lookupWriteCursor = (uint8_t)(lookupInputValue - TERRAIN_RUNTIME_BYTE_FADE_STEP);
    }
    lookupWriteCursor++;
    nextInputByte = (char)lookupInputValue + 1;
    lookupInputValue = (uint32_t)nextInputByte;
  } while (nextInputByte != 0);
  /* row 0x82: PERSISTENT */
  lookupInputValue = 0;
  do {
    if (lookupInputValue < TERRAIN_RUNTIME_BYTE_LEVEL_PERSISTENT + 1) {
      if (lookupInputValue + TERRAIN_RUNTIME_BYTE_FADE_STEP < TERRAIN_RUNTIME_BYTE_LEVEL_PERSISTENT) {
        *lookupWriteCursor = (uint8_t)(lookupInputValue + TERRAIN_RUNTIME_BYTE_FADE_STEP);
      }
      else {
        *lookupWriteCursor = TERRAIN_RUNTIME_BYTE_LEVEL_PERSISTENT;
      }
    }
    else if ((int)(lookupInputValue - TERRAIN_RUNTIME_BYTE_FADE_STEP) < TERRAIN_RUNTIME_BYTE_LEVEL_PERSISTENT + 1) {
      *lookupWriteCursor = TERRAIN_RUNTIME_BYTE_LEVEL_PERSISTENT;
    }
    else {
      *lookupWriteCursor = (uint8_t)(lookupInputValue - TERRAIN_RUNTIME_BYTE_FADE_STEP);
    }
    lookupWriteCursor++;
    nextInputByte = (char)lookupInputValue + 1;
    lookupInputValue = (uint32_t)nextInputByte;
  } while (nextInputByte != 0);
  /* rows 0x83..0xFF (125): FULL */
  finalRowsRemaining = 0x7d;
  lookupInputValue = 0;
  do {
    if (lookupInputValue < 256) {
      if (lookupInputValue + TERRAIN_RUNTIME_BYTE_FADE_STEP < TERRAIN_RUNTIME_BYTE_LEVEL_FULL) {
        *lookupWriteCursor = (uint8_t)(lookupInputValue + TERRAIN_RUNTIME_BYTE_FADE_STEP);
      }
      else {
        *lookupWriteCursor = TERRAIN_RUNTIME_BYTE_LEVEL_FULL;
      }
    }
    else if ((int)(lookupInputValue - TERRAIN_RUNTIME_BYTE_FADE_STEP) < 256) {
      *lookupWriteCursor = TERRAIN_RUNTIME_BYTE_LEVEL_FULL;
    }
    else {
      *lookupWriteCursor = (uint8_t)(lookupInputValue - TERRAIN_RUNTIME_BYTE_FADE_STEP);
    }
    lookupWriteCursor++;
    nextInputByte = (char)lookupInputValue + 1;
    lookupInputValue = (uint32_t)nextInputByte;
  } while ((nextInputByte != 0) || (finalRowsRemaining--, finalRowsRemaining != 0));
  return StatusValue_Ok(0);
}


/* Address: 0x00503F30.
   Loads the terrain graphics of a field (fld asset, else FATAL_ERROR_FIELD_ASSET_INVALID): the 26 material
   texture sets <secondary>a..z.gfx (those flagged in field->fieldFlags are required, the others optional),
   <primary>.dat/.gfx/.pal and <secondary>.pal/.dat, then initialises the field's runtime cells and the
   animated direction table. Advances the loading movie between steps; CF set with the error on failure.
*/
StatusResult TerrainVisualResources_LoadPrimary
          (uint16_t *primaryResourcePath,uint16_t *secondaryResourcePath,FieldGridAsset *field)

{
  GraphicsPaletteAsset *loadedResourceOrError;
  GraphicsTextureSet *materialTextureSet;
  uint32_t randomValue;
  int loopCounter;
  int materialSlotsRemaining;
  uint16_t pathCharOrRotationRate;
  uint32_t materialFlagBits;
  TerrainMaterialSuffixEntry *suffixLetterCursor;
  uint16_t *pathScanCursor;
  uint16_t *pathScanNext;
  GraphicsTextureSet **materialTextureSetSlot;
  TerrainDirectionRecord *directionRecord;
  TextureSetResult textureSetLoad;
  PackageLoadResult packageLoad;
  PaletteAssetResult paletteLoad;
  StatusResult randomOrSuccessStatus;
  StatusResult failureStatus;
  TerrainMaterialSuffixEntry *pathSuffixEntry;
  
  loopCounter = 256; /* the path is scanned for at most 256 characters */
  pathScanCursor = secondaryResourcePath;
  do {
    pathScanNext = pathScanCursor;
    if (loopCounter == 0) break;
    loopCounter--;
    pathScanNext = pathScanCursor + 1;
    pathCharOrRotationRate = *pathScanCursor;
    pathScanCursor = pathScanNext;
  } while (pathCharOrRotationRate != 0);
  pathSuffixEntry = (TerrainMaterialSuffixEntry *)(pathScanNext - 1);
  loadedResourceOrError = (GraphicsPaletteAsset *)FATAL_ERROR_FIELD_ASSET_INVALID;
  if (((field->common).magic == ASSET_MAGIC_FLD) &&
     ((field->common).converterVersion == PCK_CONVERTER_FLD_SHT_00060006)) {
    materialFlagBits = field->fieldFlags;
    loopCounter = 0;
    do {
      if ((materialFlagBits & 1) != 0) {
        loopCounter++;
      }
      materialFlagBits = materialFlagBits >> 1;
    } while (materialFlagBits != 0);
    materialSlotsRemaining = TERRAIN_MATERIAL_TEXTURE_SET_COUNT;
    materialFlagBits = field->fieldFlags;
    g_MoviePlaybackScheduleSpan = loopCounter * 2 + 10;
    suffixLetterCursor = g_TerrainMaterialTextureSuffixLettersUtf16AtoZ;
    materialTextureSetSlot = g_TerrainMaterialTextureSets;
    do {
      if ((materialFlagBits & 1) == 0) {
        *pathSuffixEntry = *suffixLetterCursor;
        WidePath_SetExtensionCode(ASSET_MAGIC_GFX,secondaryResourcePath);
        textureSetLoad = g_GraphicsTextureSetLoadPackage(secondaryResourcePath);
        materialTextureSet = textureSetLoad.textureSet;
        if (textureSetLoad.failed) {
          materialTextureSet = NULL; /* optional material: missing is fine */
        }
        *materialTextureSetSlot = materialTextureSet;
      }
      else {
        *pathSuffixEntry = *suffixLetterCursor;
        WidePath_SetExtensionCode(ASSET_MAGIC_GFX,secondaryResourcePath);
        MoviePlayback_AdvanceScheduledFrameAndTick();
        textureSetLoad = g_GraphicsTextureSetLoadPackage(secondaryResourcePath);
        loadedResourceOrError = (GraphicsPaletteAsset *)textureSetLoad.textureSet;
        if (textureSetLoad.failed) {
          failureStatus.failed = true;
          failureStatus.valueOrError = (uint32_t)loadedResourceOrError;
          return failureStatus;
        }
        MoviePlayback_AdvanceScheduledFrameAndTick();
        *materialTextureSetSlot = (GraphicsTextureSet *)loadedResourceOrError;
      }
      suffixLetterCursor++;
      materialTextureSetSlot++;
      materialFlagBits = materialFlagBits >> 1;
      materialSlotsRemaining--;
    } while (materialSlotsRemaining != 0);
    WidePath_SetExtensionCode(WIDE_PATH_EXTENSION_DAT,primaryResourcePath); /* ".dat" */
    packageLoad = Package_LoadEntry(primaryResourcePath);
    loadedResourceOrError = packageLoad.bufferOrError;
    if (!packageLoad.failed) {
      MoviePlayback_AdvanceScheduledFrameAndTick();
      g_TerrainSurfacePacketTablePayload =
           (uint8_t *)loadedResourceOrError + TERRAIN_PACKET_TABLE_HEADER_BYTES;
      WidePath_SetExtensionCode(ASSET_MAGIC_GFX,primaryResourcePath);
      textureSetLoad = g_GraphicsTextureSetLoadPackage(primaryResourcePath);
      loadedResourceOrError = (GraphicsPaletteAsset *)textureSetLoad.textureSet;
      if (!textureSetLoad.failed) {
        MoviePlayback_AdvanceScheduledFrameAndTick();
        g_TerrainPrimaryTextureSet = (GraphicsTextureSet *)loadedResourceOrError;
        WidePath_SetExtensionCode(ASSET_MAGIC_PAL,primaryResourcePath);
        paletteLoad = g_GraphicsPaletteAssetLoadPackage(primaryResourcePath);
        loadedResourceOrError = paletteLoad.paletteAsset;
        if (!paletteLoad.failed) {
          MoviePlayback_AdvanceScheduledFrameAndTick();
          g_TerrainPrimaryPalette = loadedResourceOrError;
          pathSuffixEntry->lowercaseLetterUtf16 = 0;
          pathSuffixEntry->terminator = 0;
          WidePath_SetExtensionCode(ASSET_MAGIC_PAL,secondaryResourcePath);
          paletteLoad = g_GraphicsPaletteAssetLoadPackage(secondaryResourcePath);
          loadedResourceOrError = paletteLoad.paletteAsset;
          if (!paletteLoad.failed) {
            MoviePlayback_AdvanceScheduledFrameAndTick();
            g_TerrainSecondaryPalette = loadedResourceOrError;
            pathSuffixEntry->lowercaseLetterUtf16 = 0;
            pathSuffixEntry->terminator = 0;
            WidePath_SetExtensionCode(WIDE_PATH_EXTENSION_DAT,secondaryResourcePath); /* ".dat" */
            packageLoad = Package_LoadEntry(secondaryResourcePath);
            loadedResourceOrError = packageLoad.bufferOrError;
            if (!packageLoad.failed) {
              MoviePlayback_AdvanceScheduledFrameAndTick();
              g_TerrainSoilPacketTablePayload =
                   (uint8_t *)loadedResourceOrError + TERRAIN_PACKET_TABLE_HEADER_BYTES;
              FieldGrid_InitializeRuntimeCellsAndBoundaryFlags(field);
              MoviePlayback_AdvanceScheduledFrameAndTick();
              /* random animation for the 256 direction records: scales 0x80..0x9F, rates +-(0x200..0x27F)
                 per step, random start angles */
              loopCounter = 256;
              directionRecord = g_TerrainDirectionRecordTable256;
              do {
                randomValue = Random_NextPrimary();
                directionRecord->scaleA = (randomValue & 0x1f) + 0x80;
                pathCharOrRotationRate = ((uint16_t)(randomValue >> 16) & 0x7f) + 0x200;
                randomValue = Random_NextPrimary();
                if ((int)randomValue < 0) {
                  pathCharOrRotationRate = -pathCharOrRotationRate;
                }
                directionRecord->rateA = pathCharOrRotationRate;
                ((short *)&directionRecord->packedAngles)[0] = (short)randomValue; /* angle A */
                randomValue = Random_NextPrimary();
                directionRecord->scaleB = (randomValue & 0x1f) + 0x80;
                pathCharOrRotationRate = ((uint16_t)(randomValue >> 16) & 0x7f) + 0x200;
                randomOrSuccessStatus.valueOrError = Random_NextPrimary();
                if ((int)randomOrSuccessStatus.valueOrError < 0) {
                  pathCharOrRotationRate = -pathCharOrRotationRate;
                }
                directionRecord->rateB = pathCharOrRotationRate;
                ((short *)&directionRecord->packedAngles)[1] = (short)randomOrSuccessStatus.valueOrError; /* angle B */
                directionRecord->angleAComponent0ScaledQ28 = 0;
                directionRecord->angleAComponent1ScaledQ28 = 0;
                directionRecord->angleBComponent0ScaledQ28 = 0;
                directionRecord++;
                loopCounter--;
              } while (loopCounter != 0);
              MoviePlayback_AdvanceScheduledFrameAndTick();
              TerrainDirectionTable_AdvanceAndRebuildVectors();
              randomOrSuccessStatus.failed = false;
              return randomOrSuccessStatus;
            }
          }
        }
      }
    }
  }
  failureStatus.failed = true;
  failureStatus.valueOrError = (uint32_t)loadedResourceOrError;
  return failureStatus;
}


/* Address: 0x005041C0.
   Variant of TerrainVisualResources_LoadPrimary for a field whose runtime cells already exist (loading a
   savegame): the same resources are loaded, but the cells only get their lookup pointers rebuilt, and
   flagsAndMaterial bit 28 (meaning unresolved) is cleared in every cell.
*/
StatusResult TerrainVisualResources_LoadAndClearCellOverlayFlags
          (uint16_t *primaryResourcePath,uint16_t *secondaryResourcePath,FieldGridAsset *field)

{
  GraphicsPaletteAsset *loadedResourceOrError;
  GraphicsTextureSet *materialTextureSet;
  uint32_t randomValue;
  int loopCounter;
  int materialSlotsRemaining;
  uint16_t pathCharOrRotationRate;
  uint32_t materialFlagBits;
  TerrainMaterialSuffixEntry *suffixLetterCursor;
  FieldGridCell *fieldCell;
  uint16_t *pathScanCursor;
  uint16_t *pathScanNext;
  GraphicsTextureSet **materialTextureSetSlot;
  TerrainDirectionRecord *directionRecord;
  TextureSetResult textureSetLoad;
  PackageLoadResult packageLoad;
  PaletteAssetResult paletteLoad;
  StatusResult randomOrSuccessStatus;
  StatusResult failureStatus;
  TerrainMaterialSuffixEntry *pathSuffixEntry;
  
  loopCounter = 256; /* the path is scanned for at most 256 characters */
  pathScanCursor = secondaryResourcePath;
  do {
    pathScanNext = pathScanCursor;
    if (loopCounter == 0) break;
    loopCounter--;
    pathScanNext = pathScanCursor + 1;
    pathCharOrRotationRate = *pathScanCursor;
    pathScanCursor = pathScanNext;
  } while (pathCharOrRotationRate != 0);
  pathSuffixEntry = (TerrainMaterialSuffixEntry *)(pathScanNext - 1);
  loadedResourceOrError = (GraphicsPaletteAsset *)FATAL_ERROR_FIELD_ASSET_INVALID;
  if (((field->common).magic == ASSET_MAGIC_FLD) &&
     ((field->common).converterVersion == PCK_CONVERTER_FLD_SHT_00060006)) {
    materialFlagBits = field->fieldFlags;
    loopCounter = 0;
    do {
      if ((materialFlagBits & 1) != 0) {
        loopCounter++;
      }
      materialFlagBits = materialFlagBits >> 1;
    } while (materialFlagBits != 0);
    materialSlotsRemaining = TERRAIN_MATERIAL_TEXTURE_SET_COUNT;
    materialFlagBits = field->fieldFlags;
    g_MoviePlaybackScheduleSpan = loopCounter * 2 + 10;
    suffixLetterCursor = g_TerrainMaterialTextureSuffixLettersUtf16AtoZ;
    materialTextureSetSlot = g_TerrainMaterialTextureSets;
    do {
      if ((materialFlagBits & 1) == 0) {
        *pathSuffixEntry = *suffixLetterCursor;
        WidePath_SetExtensionCode(ASSET_MAGIC_GFX,secondaryResourcePath);
        textureSetLoad = g_GraphicsTextureSetLoadPackage(secondaryResourcePath);
        materialTextureSet = textureSetLoad.textureSet;
        if (textureSetLoad.failed) {
          materialTextureSet = NULL; /* optional material: missing is fine */
        }
        *materialTextureSetSlot = materialTextureSet;
      }
      else {
        *pathSuffixEntry = *suffixLetterCursor;
        WidePath_SetExtensionCode(ASSET_MAGIC_GFX,secondaryResourcePath);
        MoviePlayback_AdvanceScheduledFrameAndTick();
        textureSetLoad = g_GraphicsTextureSetLoadPackage(secondaryResourcePath);
        loadedResourceOrError = (GraphicsPaletteAsset *)textureSetLoad.textureSet;
        if (textureSetLoad.failed) {
          failureStatus.failed = true;
          failureStatus.valueOrError = (uint32_t)loadedResourceOrError;
          return failureStatus;
        }
        MoviePlayback_AdvanceScheduledFrameAndTick();
        *materialTextureSetSlot = (GraphicsTextureSet *)loadedResourceOrError;
      }
      suffixLetterCursor++;
      materialTextureSetSlot++;
      materialFlagBits = materialFlagBits >> 1;
      materialSlotsRemaining--;
    } while (materialSlotsRemaining != 0);
    WidePath_SetExtensionCode(WIDE_PATH_EXTENSION_DAT,primaryResourcePath); /* ".dat" */
    packageLoad = Package_LoadEntry(primaryResourcePath);
    loadedResourceOrError = packageLoad.bufferOrError;
    if (!packageLoad.failed) {
      MoviePlayback_AdvanceScheduledFrameAndTick();
      g_TerrainSurfacePacketTablePayload =
           (uint8_t *)loadedResourceOrError + TERRAIN_PACKET_TABLE_HEADER_BYTES;
      WidePath_SetExtensionCode(ASSET_MAGIC_GFX,primaryResourcePath);
      textureSetLoad = g_GraphicsTextureSetLoadPackage(primaryResourcePath);
      loadedResourceOrError = (GraphicsPaletteAsset *)textureSetLoad.textureSet;
      if (!textureSetLoad.failed) {
        MoviePlayback_AdvanceScheduledFrameAndTick();
        g_TerrainPrimaryTextureSet = (GraphicsTextureSet *)loadedResourceOrError;
        WidePath_SetExtensionCode(ASSET_MAGIC_PAL,primaryResourcePath);
        paletteLoad = g_GraphicsPaletteAssetLoadPackage(primaryResourcePath);
        loadedResourceOrError = paletteLoad.paletteAsset;
        if (!paletteLoad.failed) {
          MoviePlayback_AdvanceScheduledFrameAndTick();
          g_TerrainPrimaryPalette = loadedResourceOrError;
          pathSuffixEntry->lowercaseLetterUtf16 = 0;
          pathSuffixEntry->terminator = 0;
          WidePath_SetExtensionCode(ASSET_MAGIC_PAL,secondaryResourcePath);
          paletteLoad = g_GraphicsPaletteAssetLoadPackage(secondaryResourcePath);
          loadedResourceOrError = paletteLoad.paletteAsset;
          if (!paletteLoad.failed) {
            MoviePlayback_AdvanceScheduledFrameAndTick();
            g_TerrainSecondaryPalette = loadedResourceOrError;
            pathSuffixEntry->lowercaseLetterUtf16 = 0;
            pathSuffixEntry->terminator = 0;
            WidePath_SetExtensionCode(WIDE_PATH_EXTENSION_DAT,secondaryResourcePath); /* ".dat" */
            packageLoad = Package_LoadEntry(secondaryResourcePath);
            loadedResourceOrError = packageLoad.bufferOrError;
            if (!packageLoad.failed) {
              MoviePlayback_AdvanceScheduledFrameAndTick();
              g_TerrainSoilPacketTablePayload =
                   (uint8_t *)loadedResourceOrError + TERRAIN_PACKET_TABLE_HEADER_BYTES;
              FieldGrid_RebuildCellLookupPointers(field);
              MoviePlayback_AdvanceScheduledFrameAndTick();
              /* random animation for the 256 direction records: scales 0x80..0x9F, rates +-(0x200..0x27F)
                 per step, random start angles */
              loopCounter = 256;
              directionRecord = g_TerrainDirectionRecordTable256;
              do {
                randomValue = Random_NextPrimary();
                directionRecord->scaleA = (randomValue & 0x1f) + 0x80;
                pathCharOrRotationRate = ((uint16_t)(randomValue >> 16) & 0x7f) + 0x200;
                randomValue = Random_NextPrimary();
                if ((int)randomValue < 0) {
                  pathCharOrRotationRate = -pathCharOrRotationRate;
                }
                directionRecord->rateA = pathCharOrRotationRate;
                ((short *)&directionRecord->packedAngles)[0] = (short)randomValue; /* angle A */
                randomValue = Random_NextPrimary();
                directionRecord->scaleB = (randomValue & 0x1f) + 0x80;
                pathCharOrRotationRate = ((uint16_t)(randomValue >> 16) & 0x7f) + 0x200;
                randomOrSuccessStatus.valueOrError = Random_NextPrimary();
                if ((int)randomOrSuccessStatus.valueOrError < 0) {
                  pathCharOrRotationRate = -pathCharOrRotationRate;
                }
                directionRecord->rateB = pathCharOrRotationRate;
                ((short *)&directionRecord->packedAngles)[1] = (short)randomOrSuccessStatus.valueOrError; /* angle B */
                directionRecord->angleAComponent0ScaledQ28 = 0;
                directionRecord->angleAComponent1ScaledQ28 = 0;
                directionRecord->angleBComponent0ScaledQ28 = 0;
                directionRecord++;
                loopCounter--;
              } while (loopCounter != 0);
              MoviePlayback_AdvanceScheduledFrameAndTick();
              loopCounter = field->gridWidth * field->gridHeight;
              fieldCell = field->cells;
              do {
                fieldCell->flagsAndMaterial =
                     fieldCell->flagsAndMaterial &
                     ~FIELD_CELL_TERRAIN_VISUAL_CLEARABLE_UNRESOLVED_BIT28;
                fieldCell++;
                loopCounter--;
              } while (loopCounter != 0);
              TerrainDirectionTable_AdvanceAndRebuildVectors();
              randomOrSuccessStatus.failed = false;
              return randomOrSuccessStatus;
            }
          }
        }
      }
    }
  }
  failureStatus.failed = true;
  failureStatus.valueOrError = (uint32_t)loadedResourceOrError;
  return failureStatus;
}


/* Address: 0x00504470.
   Releases everything TerrainVisualResources_Load* loaded: the 26 material texture sets, the primary
   texture set, both palettes and both .dat tables (their globals point 0x20 bytes into the loaded
   resource, past its header, so that offset is undone before Resource_Release).
*/
void TerrainVisualResources_Shutdown(void)

{
  int materialTextureSetsRemaining;
  GraphicsTextureSet **materialTextureSetCursor;
  void *surfacePacketTablePayload;

  materialTextureSetCursor = g_TerrainMaterialTextureSets;
  materialTextureSetsRemaining = TERRAIN_MATERIAL_TEXTURE_SET_COUNT;
  do {
    if (*materialTextureSetCursor != NULL) {
      g_GraphicsTextureSetReleasePackage(*materialTextureSetCursor);
      *materialTextureSetCursor = NULL;
    }
    materialTextureSetCursor++;
    materialTextureSetsRemaining--;
  } while (materialTextureSetsRemaining != 0);
  g_GraphicsTextureSetReleasePackage(g_TerrainPrimaryTextureSet);
  g_GraphicsPaletteAssetLifecycleCallbacks3.releasePackage(g_TerrainSecondaryPalette);
  g_GraphicsPaletteAssetLifecycleCallbacks3.releasePackage(g_TerrainPrimaryPalette);
  surfacePacketTablePayload = g_TerrainSurfacePacketTablePayload;
  if (g_TerrainSoilPacketTablePayload != NULL) {
    Resource_Release((void *)((int)g_TerrainSoilPacketTablePayload - TERRAIN_PACKET_TABLE_HEADER_BYTES));
  }
  if (surfacePacketTablePayload != NULL) {
    Resource_Release((void *)((int)surfacePacketTablePayload - TERRAIN_PACKET_TABLE_HEADER_BYTES));
  }
  g_TerrainPrimaryTextureSet = NULL;
  g_TerrainSecondaryPalette = NULL;
  g_TerrainPrimaryPalette = NULL;
  g_TerrainSoilPacketTablePayload = NULL;
  g_TerrainSurfacePacketTablePayload = NULL;
}


/* Address: 0x00505780.
   Sets up the terrain lighting colours: g_TerrainLightingColorRampArgb256[i] = base + ramp * (256 - i) / 256
   per colour channel (saturated at 0xFF, alpha taken from base), the directional-light LUT is filled with
   the base colour and the secondary colour is stored in g_TerrainDirectionalLightSecondaryColor.
*/
void TerrainLighting_BuildColorRampAndSetBaseColor
          (PackedArgb32 secondaryColorArgb,PackedArgb32 baseColorArgb,PackedArgb32 rampStepColorArgb
          )

{
  uint32_t channelValue;
  int rampStepsRemaining;
  uint32_t *rampEntryCursor;
  PackedArgb32 *lightLutCursor;
  
  rampEntryCursor = g_TerrainLightingColorRampArgb256;
  rampStepsRemaining = TERRAIN_LIGHTING_RAMP_ENTRY_COUNT;
  do {
    channelValue = ((rampStepColorArgb & ARGB8888_BLUE_MASK) * rampStepsRemaining >> 8) + (baseColorArgb & ARGB8888_BLUE_MASK);
    if (ARGB8888_BLUE_MASK < channelValue) {
      channelValue = ARGB8888_BLUE_MASK;
    }
    *rampEntryCursor = channelValue;
    rampEntryCursor++;
    rampStepsRemaining--;
  } while (rampStepsRemaining != 0);
  rampEntryCursor = g_TerrainLightingColorRampArgb256;
  rampStepsRemaining = TERRAIN_LIGHTING_RAMP_ENTRY_COUNT;
  do {
    channelValue = ((rampStepColorArgb & ARGB8888_GREEN_MASK) * rampStepsRemaining >> 8) + (baseColorArgb & ARGB8888_GREEN_MASK);
    if (0xffff < channelValue) {
      channelValue = ARGB8888_GREEN_MASK;
    }
    *rampEntryCursor = *rampEntryCursor | channelValue & ARGB8888_GREEN_MASK;
    rampEntryCursor++;
    rampStepsRemaining--;
  } while (rampStepsRemaining != 0);
  rampEntryCursor = g_TerrainLightingColorRampArgb256;
  rampStepsRemaining = TERRAIN_LIGHTING_RAMP_ENTRY_COUNT;
  do {
    channelValue = ((rampStepColorArgb & ARGB8888_RED_MASK) * rampStepsRemaining >> 8) + (baseColorArgb & ARGB8888_RED_MASK);
    if (0xffffff < channelValue) {
      channelValue = ARGB8888_RED_MASK;
    }
    *rampEntryCursor = *rampEntryCursor | channelValue & ARGB8888_RED_MASK;
    rampEntryCursor++;
    rampStepsRemaining--;
  } while (rampStepsRemaining != 0);
  rampEntryCursor = g_TerrainLightingColorRampArgb256;
  rampStepsRemaining = TERRAIN_LIGHTING_RAMP_ENTRY_COUNT;
  do {
    *rampEntryCursor = *rampEntryCursor | baseColorArgb & ARGB8888_ALPHA_MASK;
    rampEntryCursor++;
    rampStepsRemaining--;
  } while (rampStepsRemaining != 0);
  g_TerrainDirectionalLightSecondaryColor = secondaryColorArgb;
  lightLutCursor = g_TerrainDirectionalLightColorLut;
  for (rampStepsRemaining = TERRAIN_DIRECTIONAL_LIGHT_LUT_ENTRY_COUNT; rampStepsRemaining != 0;
       rampStepsRemaining--) {
    *lightLutCursor = baseColorArgb;
    lightLutCursor++;
  }
}


/* Address: 0x0053D4D0.
   Frees the terrain composite texture built by TerrainCompositeTexture_Create (through its allocation base).
   g_TerrainCompositeTexture and the in-game root keep the stale pointer.
*/
void TerrainCompositeTexture_Destroy(void)

{
  GraphicsTextureSourceAsset *allocationBase;

  allocationBase = g_GraphicsTextureSourceResolveAllocationBase
                     (&g_TerrainCompositeTexture->textureSource);
  g_MemoryApi.free(allocationBase);
  return;
}


/* Address: 0x00561EA0.
   In-game command 0x2D70 (INGAME_COMMAND_EDITOR_TURN_LIGHT; issued by Ctrl editor hotkeys in
   ui/ingame/runtime.c with steps of +-0x400): turns the terrain light and relights the field region. The
   elevation (the root's lightElevationAngle) is kept between -0x4000 (straight down) and -0x1000, the
   azimuth (lightAzimuthAngle) wraps around.
*/
void TerrainLighting_AdjustDirectionAndRecomputeField
          (uint32_t playerRuntimeId,uint32_t reservedZero,uint32_t deltaElevationAngle,
          uint32_t deltaAzimuthAngle)

{
  Q12 lightElevationAngle;

  lightElevationAngle = deltaElevationAngle + g_InGameRuntimeRoot->lightElevationAngle;
  if (-0x1000 < lightElevationAngle) {
    lightElevationAngle = -0x1000;
  }
  if (lightElevationAngle < -FIXED_ANGLE16_QUARTER_TURN) {
    lightElevationAngle = -FIXED_ANGLE16_QUARTER_TURN;
  }
  WorldRuntime_RecomputeFieldRegionNormalsAndLighting
            ((g_InGameRuntimeRoot->worldRuntime).fieldRegion.auxiliaryElevationAngle,
             (g_InGameRuntimeRoot->worldRuntime).fieldRegion.auxiliaryAzimuthAngle,lightElevationAngle,
             deltaAzimuthAngle + g_InGameRuntimeRoot->lightAzimuthAngle & FIXED_ANGLE16_MASK,
             &g_InGameRuntimeRoot->worldRuntime);
  return;
}


/* Address: 0x0053D560.
   Renders plane 1 of the terrain composite texture (the minimap image, one ARGB pixel per field cell):
   dry cells get their material's panel colour shaded by terrain height, flooded cells the water colour
   (palette entry 0) shaded by water depth, both through g_PackedLightingLookupTable.
*/
void TerrainCompositeTexture_FillPlane1(void)

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
  uint64_t mm0PackedValue0;
  uint64_t mm0PackedValue1;
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
        materialColorArgb = ((GraphicsPaletteTextureSourceAsset *)panelTextureSource)->paletteEntries[panelSubresourceIndex * 0x100 + (fieldCell->flagsAndMaterial & FIELD_CELL_MATERIAL_ID_MASK)].argb8888;
        if (lightingLevelIndex < 0) {
          lightingLevelIndex = 0x70;
        }
        else if (lightingLevelIndex < 0x60) {
          lightingLevelIndex = lightingLevelIndex + 0x70;
        }
        else {
          lightingLevelIndex = 0xcf;
        }
        /* PUNPCKLBW/PSRLW 3, PMULHW by the lighting level, PACKUSWB */
        mm0PackedValue0 =
             pmulhw(TerrainColor_UnpackBytesShiftRight(materialColorArgb,3),
                    g_PackedLightingLookupTable[lightingLevelIndex]);
        *(uint32_t *)planePixelCursor = TerrainColor_PackWordsUnsignedSaturate(mm0PackedValue0);
      }
      else {
        lightingLevelIndex = -fieldCell->waterSurfaceDelta >> 5; /* depth levels 0xBF down to 0x80 */
        waterColorArgb = g_TerrainPrimaryPalette->paletteEntries[0].argb8888;
        if (lightingLevelIndex < 0) {
          if (lightingLevelIndex < -0x3f) {
            lightingLevelIndex = 0x80;
          }
          else {
            lightingLevelIndex = lightingLevelIndex + 0xc0;
          }
        }
        else {
          lightingLevelIndex = 0xbf;
        }
        mm0PackedValue1 =
             pmulhw(TerrainColor_UnpackBytesShiftRight(waterColorArgb,3),
                    g_PackedLightingLookupTable[lightingLevelIndex]);
        *(uint32_t *)planePixelCursor = TerrainColor_PackWordsUnsignedSaturate(mm0PackedValue1);
      }
      fieldCell++;
      planePixelCursor = planePixelCursor + 4;
      columnsRemaining--;
    } while (columnsRemaining != 0);
    rowsRemaining--;
    columnsRemaining = textureWidth;
  } while (rowsRemaining != 0);
}


/* Address: 0x0053D680.
   Renders plane 2 of the terrain composite texture, the resource view of the minimap: Xenite, Tritium and
   plain soil cells get their panel colours shaded by terrain height, and water is blended 50/50 over
   flooded cells.
*/
void TerrainCompositeTexture_FillPlane2(void)

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
  uint64_t mm0PackedValue0;
  uint64_t mm0PackedValue2;
  uint64_t mm0PackedValue3;
  uint64_t mm0PackedValue1;
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
          soilColorArgb = ((GraphicsPaletteTextureSourceAsset *)panelTextureSource)->paletteEntries[panelSubresourceIndex * 0x100 + 0x40].argb8888;
          if (lightingLevelIndex < 0) {
            lightingLevelIndex = 0x70;
          }
          else if (lightingLevelIndex < 0x60) {
            lightingLevelIndex = lightingLevelIndex + 0x70;
          }
          else {
            lightingLevelIndex = 0xcf;
          }
          mm0PackedValue3 =
               pmulhw(TerrainColor_UnpackBytesShiftRight(soilColorArgb,3),
                      g_PackedLightingLookupTable[lightingLevelIndex]);
          *(uint32_t *)planePixelCursor = TerrainColor_PackWordsUnsignedSaturate(mm0PackedValue3);
        }
        else {
          lightingLevelIndex = fieldCell->terrainHeight >> 7; /* height levels 0x70..0xCF */
          tritiumColorArgb = ((GraphicsPaletteTextureSourceAsset *)panelTextureSource)->paletteEntries[panelSubresourceIndex * 0x100 + 0x42].argb8888;
          if (lightingLevelIndex < 0) {
            lightingLevelIndex = 0x70;
          }
          else if (lightingLevelIndex < 0x60) {
            lightingLevelIndex = lightingLevelIndex + 0x70;
          }
          else {
            lightingLevelIndex = 0xcf;
          }
          mm0PackedValue2 =
               pmulhw(TerrainColor_UnpackBytesShiftRight(tritiumColorArgb,3),
                      g_PackedLightingLookupTable[lightingLevelIndex]);
          *(uint32_t *)planePixelCursor = TerrainColor_PackWordsUnsignedSaturate(mm0PackedValue2);
        }
      }
      else {
        lightingLevelIndex = fieldCell->terrainHeight >> 7; /* height levels 0x70..0xCF */
        xeniteColorArgb = ((GraphicsPaletteTextureSourceAsset *)panelTextureSource)->paletteEntries[panelSubresourceIndex * 0x100 + 0x41].argb8888;
        if (lightingLevelIndex < 0) {
          lightingLevelIndex = 0x70;
        }
        else if (lightingLevelIndex < 0x60) {
          lightingLevelIndex = lightingLevelIndex + 0x70;
        }
        else {
          lightingLevelIndex = 0xcf;
        }
        mm0PackedValue0 =
             pmulhw(TerrainColor_UnpackBytesShiftRight(xeniteColorArgb,3),
                    g_PackedLightingLookupTable[lightingLevelIndex]);
        *(uint32_t *)planePixelCursor = TerrainColor_PackWordsUnsignedSaturate(mm0PackedValue0);
      }
      if (0 < fieldCell->waterSurfaceDelta) {
        lightingLevelIndex = -fieldCell->waterSurfaceDelta >> 5; /* depth levels 0xBF down to 0x80 */
        waterColorArgb = g_TerrainPrimaryPalette->paletteEntries[0].argb8888;
        if (lightingLevelIndex < 0) {
          if (lightingLevelIndex < -0x3f) {
            lightingLevelIndex = 0x80;
          }
          else {
            lightingLevelIndex = lightingLevelIndex + 0xc0;
          }
        }
        else {
          lightingLevelIndex = 0xbf;
        }
        existingPixelArgb = *(uint32_t *)planePixelCursor;
        mm0PackedValue1 =
             pmulhw(TerrainColor_UnpackBytesShiftRight(waterColorArgb,3),
                    g_PackedLightingLookupTable[lightingLevelIndex]);
        /* PADDW with the existing pixel's bytes (PUNPCKLBW/PSRLW 8), PSRLW 1, PACKUSWB */
        *(uint32_t *)planePixelCursor =
             TerrainColor_PackWordsUnsignedSaturate
                       (TerrainColor_AverageWordsWithPixelBytes(mm0PackedValue1,existingPixelArgb));
      }
      fieldCell++;
      planePixelCursor = planePixelCursor + 4;
      columnsRemaining--;
    } while (columnsRemaining != 0);
    rowsRemaining--;
    columnsRemaining = textureWidth;
  } while (rowsRemaining != 0);
}


/* Address: 0x0053D840.
   Builds the displayed minimap (plane 0): copies plane 1 (terrain) or, with bit 1 of
   minimapResourceButtonStateFlags, plane 2 (resources), hides cells the active faction has never seen
   (almost black) and darkens those it does not see now, then draws a pixel for each model runtime with an
   alpha tint whose faction has a non-zero colorIndex, in the panel colour of variant
   colorIndex when selected, variant 0 otherwise (blended 50/50 for a tint alpha below 0xFF).
*/
void TerrainCompositeTexture_RebuildPlane0(void)

{
  uint8_t visibilityFlags;
  AssetDimension textureWidth;
  AssetDimension textureHeight;
  WorldOwnerListNode *ownerNode;
  GameEntityRuntime *ownerEntity;
  InGameRuntimeRoot *inGameRoot;
  GraphicsTextureSourceAsset *panelTextureSource;
  uint32_t pixelArgb;
  int cellsRemainingOrRowQ12;
  int counterOrGridColumn;
  int gridRow;
  uint32_t colorVariant;
  AssetRelativeOffset assetOffset;
  uint8_t *pixelCursor;
  FieldGridCell *fieldCell;
  uint8_t *plane0Pixels;
  uint8_t *plane0WriteCursor;
  bool isSelected;
  FieldGridCoordinates gridCoordinates;
  
  inGameRoot = g_InGameRuntimeRoot;
  if ((g_InGameRuntimeRoot->minimapResourceButtonStateFlags & 2) == 0) {
    assetOffset = g_TerrainCompositeTexture->sourceEntries[1].dataOffset;
  }
  else {
    assetOffset = g_TerrainCompositeTexture->sourceEntries[2].dataOffset;
  }
  textureWidth = g_TerrainCompositeTexture->sourceEntries[0].pixelWidth;
  textureHeight = g_TerrainCompositeTexture->sourceEntries[0].pixelHeight;
  plane0Pixels = (uint8_t *)g_TerrainCompositeTexture + g_TerrainCompositeTexture->sourceEntries[0].dataOffset;
  cellsRemainingOrRowQ12 = textureWidth * textureHeight;
  pixelCursor = (uint8_t *)g_TerrainCompositeTexture + assetOffset;
  plane0WriteCursor = plane0Pixels;
  for (counterOrGridColumn = cellsRemainingOrRowQ12; counterOrGridColumn != 0; counterOrGridColumn--) {
    *(uint32_t *)plane0WriteCursor = *(uint32_t *)pixelCursor;
    pixelCursor = pixelCursor + 4;
    plane0WriteCursor = plane0WriteCursor + 4;
  }
  counterOrGridColumn = (inGameRoot->worldRuntime).activeFactionRuntimeIndex;
  fieldCell = ((inGameRoot->worldRuntime).fieldGrid)->cells;
  pixelCursor = plane0Pixels;
  do {
    visibilityFlags = ((uint8_t *)&fieldCell->occupancyMask)[counterOrGridColumn];
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
    cellsRemainingOrRowQ12--;
  } while (cellsRemainingOrRowQ12 != 0);
  for (ownerNode = (inGameRoot->worldRuntime).ownerListHead; ownerNode != NULL;
      ownerNode = ownerNode->nextNode) {
    if ((ownerNode->ownerClassId == WORLD_OWNER_RUNTIME_MODEL) && (0xffffff < ownerNode->modelTintArgb)) {
      gridCoordinates = FieldGrid_WorldToGridQ12(ownerNode->worldYQ12,ownerNode->worldXQ12);
      panelTextureSource = g_InGamePanelTextureSource;
      cellsRemainingOrRowQ12 = gridCoordinates.rowQ12;
      counterOrGridColumn = gridCoordinates.columnQ12 + FIELD_GRID_CELL_Q12 / 2 >> Q12_SHIFT; /* round to the nearest cell */
      if ((SCARRY4(cellsRemainingOrRowQ12,FIELD_GRID_CELL_Q12 / 2) == counterOrGridColumn < 0) &&
         (((gridRow = cellsRemainingOrRowQ12 + FIELD_GRID_CELL_Q12 / 2 >> Q12_SHIFT, SCARRY4(cellsRemainingOrRowQ12,FIELD_GRID_CELL_Q12 / 2) == gridRow < 0 &&
           (counterOrGridColumn < (int)textureWidth)) && (gridRow < (int)textureHeight)))) {
        ownerEntity = (GameEntityRuntime *)((ModelRuntimeSlot *)ownerNode->runtimePayload)->ownerArmyRuntimeOrSavedOffset.armyRuntime;
        colorVariant = g_GameFactionRuntimeImage.records[(ownerEntity->common).ownership.ownerIndex].
                 colorIndex;
        assetOffset = (g_InGamePanelTextureSource->tableDescriptor).subresourceTableOffset;
        if (colorVariant != 0) {
          isSelected = SelectionInfo_FindEntry(ownerEntity);
          if (!isSelected) {
            colorVariant = 0;
          }
          pixelArgb = ((GraphicsPaletteTextureSourceAsset *)g_InGamePanelTextureSource)->paletteEntries[0x20 + ((GraphicsTextureSourceEntry *)((uint8_t *)panelTextureSource + assetOffset))[36].paletteIndex * 4 + colorVariant].argb8888;
          if (ownerNode->modelTintArgb < ARGB8888_ALPHA_MASK) {
            pixelArgb = (pixelArgb & TERRAIN_ARGB_HALVE_MASK) +
                    (*(uint32_t *)(plane0Pixels + (gridRow * textureWidth + counterOrGridColumn) * 4) & TERRAIN_ARGB_HALVE_MASK) >> 1;
          }
          *(uint32_t *)(plane0Pixels + (gridRow * textureWidth + counterOrGridColumn) * 4) = pixelArgb;
        }
      }
    }
  }
}


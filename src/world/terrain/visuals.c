/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/src/world/terrain/visuals.c
 * Reverse engineering by idkFoxes 2026
 */

#include <thandor/world/terrain/visuals.h>
#include <thandor/thandor.h>
#include <thandor/platform/bootstrap/image.h>

/* Module data. */

/* one texture set per terrain material (26 used, TERRAIN_MATERIAL_COUNT); the remaining 12 entries are NULL. Original quirk: UiCommandMatrix_SelectIndex fills twelve swatches from a page base that can reach 15, so it reads entry 26 (always NULL, an empty swatch). */
__declspec(align(4)) GraphicsTextureSet *g_TerrainMaterialTextureSets[38] = {0};

/* path suffix letters "a".."z" (with terminator) of the 26 terrain material texture sets */
static const TerrainMaterialSuffixEntry g_TerrainMaterialTextureSuffixLettersUtf16AtoZ[26] = {
    {'a'}, {'b'}, {'c'}, {'d'}, {'e'}, {'f'}, {'g'}, {'h'}, {'i'}, {'j'}, {'k'}, {'l'}, {'m'},
    {'n'}, {'o'}, {'p'}, {'q'}, {'r'}, {'s'}, {'t'}, {'u'}, {'v'}, {'w'}, {'x'}, {'y'}, {'z'}};

static TerrainCompositeTextureRuntime *g_TerrainCompositeTexture = 0;

/* entries 0..255 the shaded colour ramp (originally
   g_TerrainLightingColorRampArgb256), entries 256..512 the lit half; indexed by the signed dot
   product -256..256 from entry 256 */
PackedArgb32 g_TerrainDirectionalLightColorLut[513] = {0};

uint32_t g_TerrainDirectionalLightSecondaryColor = 0;

uint8_t *g_TerrainByteClampLookup = 0;

GraphicsTextureSet *g_TerrainPrimaryTextureSet = 0;

void *g_TerrainSoilPacketTablePayload = 0;

void *g_TerrainSurfacePacketTablePayload = 0;

GraphicsPaletteAsset *g_TerrainPrimaryPalette = 0;

/* Implementation ownership: world/terrain/visuals. */

/* PUNPCKLBW mm,mm then PSRLW mm,shift: the four bytes b of value as the words ((b << 8) | b) >> shift.
   Spreads an ARGB colour into four 16-bit channels for the PMULHW shading in the composite texture fill. */
static __inline uint64_t TerrainColor_UnpackBytesShiftRight(uint32_t value,int shift)

{
  ThandorMmx lanes;
  int lane;

  for (lane = 0; lane < 4; lane++) {
    lanes.uw[lane] = (uint16_t)(((value >> (lane * 8) & 0xff) * COLOR_CHANNEL_TO_WORD_LANE) >> shift);
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

/* Builds the terrain composite texture for the current field grid: an in-memory gfx asset with three direct-colour
   ARGB images of one pixel per field cell, published in g_TerrainCompositeTexture and the in-game root. Fills
   planes 1 and 2 and then derives plane 0 from them. Returns true on success; on failure returns false and
   stores the allocator error in *outError (untouched on success).
*/
bool TerrainCompositeTexture_Create(uint32_t *outError)

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


/* Fills one 256-byte row of g_TerrainByteClampLookup: for every runtime byte 0..0xFF the byte moved one
   TERRAIN_RUNTIME_BYTE_FADE_STEP towards targetLevel, stopping at targetLevel from either side. Returns the
   position after the row. */
static uint8_t *TerrainByteClampLookup_FillRow(uint8_t *rowCursor,int targetLevel)

{
  int inputValue;

  for (inputValue = 0; inputValue < 256; inputValue++) {
    if (inputValue <= targetLevel) {
      if (inputValue + TERRAIN_RUNTIME_BYTE_FADE_STEP < targetLevel) {
        *rowCursor = (uint8_t)(inputValue + TERRAIN_RUNTIME_BYTE_FADE_STEP);
      }
      else {
        *rowCursor = (uint8_t)targetLevel;
      }
    }
    else if (inputValue - TERRAIN_RUNTIME_BYTE_FADE_STEP <= targetLevel) {
      *rowCursor = (uint8_t)targetLevel;
    }
    else {
      *rowCursor = (uint8_t)(inputValue - TERRAIN_RUNTIME_BYTE_FADE_STEP);
    }
    rowCursor++;
  }
  return rowCursor;
}

/* Builds g_TerrainByteClampLookup, the 64-KiB table FieldGrid_ApplyByteClampLookupToCells uses every few ticks to
   fade each cell's runtime byte (visibilityLightingIndex) one step (TERRAIN_RUNTIME_BYTE_FADE_STEP) towards the
   level its occupancy byte asks for (row targets: see TERRAIN_BYTE_CLAMP_LOOKUP_BYTES). The table is 64-KiB
   aligned so the original can index it with the two bytes as the low 16 address bits. Returns true on success; false when the allocation fails,
   with the allocator error in *outError.
*/
bool TerrainByteClampLookup_Initialize(uint32_t *outError)

{
  void *lookupAllocationBase;
  int rowPair;
  int row;
  uint8_t *lookupWriteCursor;
  uint32_t allocError;

  /* twice the size, so a 64-KiB aligned table fits inside */
  allocError = g_MemoryApi.alloc(TERRAIN_BYTE_CLAMP_LOOKUP_BYTES * 2,&lookupAllocationBase);
  if (allocError != 0) {
    *outError = allocError;
    return false;
  }
  lookupWriteCursor = (uint8_t *)(((uintptr_t)lookupAllocationBase + 0xffff) & ~(uintptr_t)0xffff);
  g_TerrainByteClampLookup = lookupWriteCursor;
  /* rows 0x00..0x7F in pairs: even rows fade to NONE, odd rows to FULL */
  for (rowPair = 0; rowPair < 64; rowPair++) {
    lookupWriteCursor = TerrainByteClampLookup_FillRow(lookupWriteCursor,TERRAIN_RUNTIME_BYTE_LEVEL_NONE);
    lookupWriteCursor = TerrainByteClampLookup_FillRow(lookupWriteCursor,TERRAIN_RUNTIME_BYTE_LEVEL_FULL);
  }
  /* row 0x80: PERSISTENT, row 0x81: FULL, row 0x82: PERSISTENT */
  lookupWriteCursor = TerrainByteClampLookup_FillRow(lookupWriteCursor,TERRAIN_RUNTIME_BYTE_LEVEL_PERSISTENT);
  lookupWriteCursor = TerrainByteClampLookup_FillRow(lookupWriteCursor,TERRAIN_RUNTIME_BYTE_LEVEL_FULL);
  lookupWriteCursor = TerrainByteClampLookup_FillRow(lookupWriteCursor,TERRAIN_RUNTIME_BYTE_LEVEL_PERSISTENT);
  /* rows 0x83..0xFF: FULL */
  for (row = 0x83; row < 0x100; row++) {
    lookupWriteCursor = TerrainByteClampLookup_FillRow(lookupWriteCursor,TERRAIN_RUNTIME_BYTE_LEVEL_FULL);
  }
  return true;
}


/* The suffix slot of a secondary resource path: its terminator, or its 256th character when there is none
   within the first 256 (the path is scanned for at most 256 characters). The material letter and the
   terminator are written there. */
static TerrainMaterialSuffixEntry *TerrainVisualResources_FindPathSuffixEntry(uint16_t *resourcePath)

{
  int pathLength;

  for (pathLength = 0; (pathLength < 255) && (resourcePath[pathLength] != 0); pathLength++) {
  }
  return (TerrainMaterialSuffixEntry *)(resourcePath + pathLength);
}

/* Loads the 26 material texture sets <secondary>a..z.gfx into g_TerrainMaterialTextureSets: those whose bit is
   set in fieldFlags are required (advancing the loading movie before and after each), the others optional
   (NULL when missing). Also sets the loading movie span from the number of required sets. Returns true on
   success; false with the load error in *outError when a required set fails. */
static bool TerrainVisualResources_LoadMaterialTextureSets
          (uint16_t *secondaryResourcePath,TerrainMaterialSuffixEntry *pathSuffixEntry,FieldGridFlags fieldFlags,
          uint32_t *outError)

{
  GraphicsTextureSet *materialTextureSet;
  int requiredSetCount;
  int materialIndex;
  uint32_t materialFlagBits;
  uint32_t loadErrorCode;

  requiredSetCount = 0;
  for (materialFlagBits = fieldFlags; materialFlagBits != 0; materialFlagBits = materialFlagBits >> 1) {
    if ((materialFlagBits & 1) != 0) {
      requiredSetCount++;
    }
  }
  g_MoviePlaybackScheduleSpan = requiredSetCount * 2 + 10;
  materialFlagBits = fieldFlags;
  for (materialIndex = 0; materialIndex < TERRAIN_MATERIAL_TEXTURE_SET_COUNT; materialIndex++) {
    *pathSuffixEntry = g_TerrainMaterialTextureSuffixLettersUtf16AtoZ[materialIndex];
    WidePath_SetExtensionCode(ASSET_MAGIC_GFX,secondaryResourcePath);
    if ((materialFlagBits & 1) == 0) {
      /* optional material: missing is fine (NULL) */
      g_TerrainMaterialTextureSets[materialIndex] = g_GraphicsTextureSetLoadPackage(secondaryResourcePath,NULL);
    }
    else {
      MoviePlayback_AdvanceScheduledFrameAndTick();
      materialTextureSet = g_GraphicsTextureSetLoadPackage(secondaryResourcePath,&loadErrorCode);
      if (materialTextureSet == NULL) {
        *outError = loadErrorCode;
        return false;
      }
      MoviePlayback_AdvanceScheduledFrameAndTick();
      g_TerrainMaterialTextureSets[materialIndex] = materialTextureSet;
    }
    materialFlagBits = materialFlagBits >> 1;
  }
  return true;
}

/* Loads <primary>.dat, <primary>.gfx, <primary>.pal, <secondary>.pal and <secondary>.dat (the secondary path
   without its material letter) into the terrain globals, advancing the loading movie after each. Returns true
   on success; false with the load error in *outError at the first failure. */
static bool TerrainVisualResources_LoadTablesAndPalettes
          (uint16_t *primaryResourcePath,uint16_t *secondaryResourcePath,
          TerrainMaterialSuffixEntry *pathSuffixEntry,uint32_t *outError)

{
  void *packetTable;
  GraphicsTextureSet *primaryTextureSet;
  GraphicsPaletteAsset *palette;
  uint32_t loadErrorCode;

  WidePath_SetExtensionCode(WIDE_PATH_EXTENSION_DAT,primaryResourcePath); /* ".dat" */
  packetTable = Package_LoadEntry(primaryResourcePath,&loadErrorCode);
  if (packetTable == NULL) {
    *outError = loadErrorCode;
    return false;
  }
  MoviePlayback_AdvanceScheduledFrameAndTick();
  g_TerrainSurfacePacketTablePayload = (uint8_t *)packetTable + TERRAIN_PACKET_TABLE_HEADER_BYTES;
  WidePath_SetExtensionCode(ASSET_MAGIC_GFX,primaryResourcePath);
  primaryTextureSet = g_GraphicsTextureSetLoadPackage(primaryResourcePath,&loadErrorCode);
  if (primaryTextureSet == NULL) {
    *outError = loadErrorCode;
    return false;
  }
  MoviePlayback_AdvanceScheduledFrameAndTick();
  g_TerrainPrimaryTextureSet = primaryTextureSet;
  WidePath_SetExtensionCode(ASSET_MAGIC_PAL,primaryResourcePath);
  palette = g_GraphicsPaletteAssetLoadPackage(primaryResourcePath,&loadErrorCode);
  if (palette == NULL) {
    *outError = loadErrorCode;
    return false;
  }
  MoviePlayback_AdvanceScheduledFrameAndTick();
  g_TerrainPrimaryPalette = palette;
  pathSuffixEntry->lowercaseLetterUtf16 = 0;
  pathSuffixEntry->terminator = 0;
  WidePath_SetExtensionCode(ASSET_MAGIC_PAL,secondaryResourcePath);
  palette = g_GraphicsPaletteAssetLoadPackage(secondaryResourcePath,&loadErrorCode);
  if (palette == NULL) {
    *outError = loadErrorCode;
    return false;
  }
  MoviePlayback_AdvanceScheduledFrameAndTick();
  g_TerrainSecondaryPalette = palette;
  pathSuffixEntry->lowercaseLetterUtf16 = 0;
  pathSuffixEntry->terminator = 0;
  WidePath_SetExtensionCode(WIDE_PATH_EXTENSION_DAT,secondaryResourcePath); /* ".dat" */
  packetTable = Package_LoadEntry(secondaryResourcePath,&loadErrorCode);
  if (packetTable == NULL) {
    *outError = loadErrorCode;
    return false;
  }
  MoviePlayback_AdvanceScheduledFrameAndTick();
  g_TerrainSoilPacketTablePayload = (uint8_t *)packetTable + TERRAIN_PACKET_TABLE_HEADER_BYTES;
  return true;
}

/* Random animation for the 256 direction records: scales 0x80..0x9F, rates +-(0x200..0x27F) per step, random
   start angles; the angle components are cleared. */
static void TerrainDirectionTable_RandomizeRecords(void)

{
  TerrainDirectionRecord *directionRecord;
  uint32_t randomValue;
  uint16_t rotationRate;
  int recordsRemaining;

  directionRecord = g_TerrainDirectionRecordTable256;
  for (recordsRemaining = 256; recordsRemaining != 0; recordsRemaining--) {
    randomValue = Random_NextPrimary();
    directionRecord->scaleA = (randomValue & TERRAIN_DIRECTION_SCALE_RANDOM_MASK) + TERRAIN_DIRECTION_SCALE_MIN;
    rotationRate = ((uint16_t)(randomValue >> 16) & TERRAIN_DIRECTION_RATE_RANDOM_MASK) +
                   TERRAIN_DIRECTION_RATE_MIN_ANGLE16;
    randomValue = Random_NextPrimary();
    if ((int)randomValue < 0) {
      rotationRate = -rotationRate;
    }
    directionRecord->rateA = rotationRate;
    ((short *)&directionRecord->packedAngles)[0] = (short)randomValue; /* angle A */
    randomValue = Random_NextPrimary();
    directionRecord->scaleB = (randomValue & TERRAIN_DIRECTION_SCALE_RANDOM_MASK) + TERRAIN_DIRECTION_SCALE_MIN;
    rotationRate = ((uint16_t)(randomValue >> 16) & TERRAIN_DIRECTION_RATE_RANDOM_MASK) +
                   TERRAIN_DIRECTION_RATE_MIN_ANGLE16;
    randomValue = Random_NextPrimary();
    if ((int)randomValue < 0) {
      rotationRate = -rotationRate;
    }
    directionRecord->rateB = rotationRate;
    ((short *)&directionRecord->packedAngles)[1] = (short)randomValue; /* angle B */
    directionRecord->angleAComponent0ScaledQ28 = 0;
    directionRecord->angleAComponent1ScaledQ28 = 0;
    directionRecord->angleBComponent0ScaledQ28 = 0;
    directionRecord++;
  }
}

/* Loads the terrain graphics of a field (fld asset, else FATAL_ERROR_FIELD_ASSET_INVALID): the 26 material
   texture sets <secondary>a..z.gfx (those flagged in field->fieldFlags are required, the others optional),
   <primary>.dat/.gfx/.pal and <secondary>.pal/.dat, then initialises the field's runtime cells and the
   animated direction table. Advances the loading movie between steps. Returns true on success; on failure
   returns false and stores the error (field check or failed resource load) in *outError (untouched on success).
*/
bool TerrainVisualResources_LoadPrimary
          (uint16_t *primaryResourcePath,uint16_t *secondaryResourcePath,FieldGridAsset *field,
          uint32_t *outError)

{
  TerrainMaterialSuffixEntry *pathSuffixEntry;

  pathSuffixEntry = TerrainVisualResources_FindPathSuffixEntry(secondaryResourcePath);
  if (((field->common).magic != ASSET_MAGIC_FLD) ||
      ((field->common).converterVersion != PCK_CONVERTER_FLD_SHT_00060006)) {
    *outError = (uint32_t)FATAL_ERROR_FIELD_ASSET_INVALID;
    return false;
  }
  if (!TerrainVisualResources_LoadMaterialTextureSets(secondaryResourcePath,pathSuffixEntry,field->fieldFlags,
                                                      outError)) {
    return false;
  }
  if (!TerrainVisualResources_LoadTablesAndPalettes(primaryResourcePath,secondaryResourcePath,pathSuffixEntry,
                                                    outError)) {
    return false;
  }
  FieldGrid_InitializeRuntimeCellsAndBoundaryFlags(field);
  MoviePlayback_AdvanceScheduledFrameAndTick();
  TerrainDirectionTable_RandomizeRecords();
  MoviePlayback_AdvanceScheduledFrameAndTick();
  TerrainDirectionTable_AdvanceAndRebuildVectors();
  return true;
}


/* Variant of TerrainVisualResources_LoadPrimary for a field whose runtime cells already exist (loading a
   savegame): the same resources are loaded, but the cells only get their lookup pointers rebuilt, and
   flagsAndMaterial bit 28 (meaning unresolved) is cleared in every cell. Returns true on success; on failure
   returns false and stores the error in *outError (untouched on success).
*/
bool TerrainVisualResources_LoadAndClearCellOverlayFlags
          (uint16_t *primaryResourcePath,uint16_t *secondaryResourcePath,FieldGridAsset *field,
          uint32_t *outError)

{
  TerrainMaterialSuffixEntry *pathSuffixEntry;
  FieldGridCell *fieldCell;
  int cellsRemaining;

  pathSuffixEntry = TerrainVisualResources_FindPathSuffixEntry(secondaryResourcePath);
  if (((field->common).magic != ASSET_MAGIC_FLD) ||
      ((field->common).converterVersion != PCK_CONVERTER_FLD_SHT_00060006)) {
    *outError = (uint32_t)FATAL_ERROR_FIELD_ASSET_INVALID;
    return false;
  }
  if (!TerrainVisualResources_LoadMaterialTextureSets(secondaryResourcePath,pathSuffixEntry,field->fieldFlags,
                                                      outError)) {
    return false;
  }
  if (!TerrainVisualResources_LoadTablesAndPalettes(primaryResourcePath,secondaryResourcePath,pathSuffixEntry,
                                                    outError)) {
    return false;
  }
  FieldGrid_RebuildCellLookupPointers(field);
  MoviePlayback_AdvanceScheduledFrameAndTick();
  TerrainDirectionTable_RandomizeRecords();
  MoviePlayback_AdvanceScheduledFrameAndTick();
  /* runs at least once (as in the original), so a field without cells would run away */
  cellsRemaining = field->gridWidth * field->gridHeight;
  fieldCell = field->cells;
  do {
    fieldCell->flagsAndMaterial = fieldCell->flagsAndMaterial & ~FIELD_CELL_TERRAIN_VISUAL_CLEARABLE_UNRESOLVED_BIT28;
    fieldCell++;
    cellsRemaining--;
  } while (cellsRemaining != 0);
  TerrainDirectionTable_AdvanceAndRebuildVectors();
  return true;
}


/* Releases everything TerrainVisualResources_Load* loaded: the 26 material texture sets, the primary
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


/* Sets up the terrain lighting colours: the shaded half of g_TerrainDirectionalLightColorLut gets
   [i] = base + ramp * (256 - i) / 256 per colour channel (saturated at 0xFF, alpha taken from base), the lit
   half (from TERRAIN_DIRECTIONAL_LIGHT_LUT_ZERO_INDEX) is filled with the base colour and the secondary colour
   is stored in g_TerrainDirectionalLightSecondaryColor.
*/
void TerrainLighting_BuildColorRampAndSetBaseColor
          (PackedArgb32 secondaryColorArgb,PackedArgb32 baseColorArgb,PackedArgb32 rampStepColorArgb
          )

{
  uint32_t channelValue;
  int rampStepsRemaining;
  uint32_t *rampEntryCursor;
  PackedArgb32 *lightLutCursor;
  
  rampEntryCursor = g_TerrainDirectionalLightColorLut;
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
  rampEntryCursor = g_TerrainDirectionalLightColorLut;
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
  rampEntryCursor = g_TerrainDirectionalLightColorLut;
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
  rampEntryCursor = g_TerrainDirectionalLightColorLut;
  rampStepsRemaining = TERRAIN_LIGHTING_RAMP_ENTRY_COUNT;
  do {
    *rampEntryCursor = *rampEntryCursor | baseColorArgb & ARGB8888_ALPHA_MASK;
    rampEntryCursor++;
    rampStepsRemaining--;
  } while (rampStepsRemaining != 0);
  g_TerrainDirectionalLightSecondaryColor = secondaryColorArgb;
  lightLutCursor = &g_TerrainDirectionalLightColorLut[TERRAIN_DIRECTIONAL_LIGHT_LUT_ZERO_INDEX];
  for (rampStepsRemaining = TERRAIN_DIRECTIONAL_LIGHT_LUT_LIT_ENTRY_COUNT; rampStepsRemaining != 0;
       rampStepsRemaining--) {
    *lightLutCursor = baseColorArgb;
    lightLutCursor++;
  }
}


/* Frees the terrain composite texture built by TerrainCompositeTexture_Create (through its allocation base).
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


/* In-game command 0x2D70 (INGAME_COMMAND_EDITOR_TURN_LIGHT; issued by Ctrl editor hotkeys in
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
  if (-TERRAIN_LIGHT_ELEVATION_MIN_TILT_ANGLE16 < lightElevationAngle) {
    lightElevationAngle = -TERRAIN_LIGHT_ELEVATION_MIN_TILT_ANGLE16;
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


/* Renders plane 1 of the terrain composite texture (the minimap image, one ARGB pixel per field cell):
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


/* Renders plane 2 of the terrain composite texture, the resource view of the minimap: Xenite, Tritium and
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
          mm0PackedValue3 =
               pmulhw(TerrainColor_UnpackBytesShiftRight(soilColorArgb,3),
                      g_PackedLightingLookupTable[lightingLevelIndex]);
          *(uint32_t *)planePixelCursor = TerrainColor_PackWordsUnsignedSaturate(mm0PackedValue3);
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
          mm0PackedValue2 =
               pmulhw(TerrainColor_UnpackBytesShiftRight(tritiumColorArgb,3),
                      g_PackedLightingLookupTable[lightingLevelIndex]);
          *(uint32_t *)planePixelCursor = TerrainColor_PackWordsUnsignedSaturate(mm0PackedValue2);
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
        mm0PackedValue0 =
             pmulhw(TerrainColor_UnpackBytesShiftRight(xeniteColorArgb,3),
                    g_PackedLightingLookupTable[lightingLevelIndex]);
        *(uint32_t *)planePixelCursor = TerrainColor_PackWordsUnsignedSaturate(mm0PackedValue0);
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


/* Builds the displayed minimap (plane 0): copies plane 1 (terrain) or, with bit 1 of
   minimapResourceButtonStateFlags, plane 2 (resources), hides cells the active faction has never seen
   (almost black) and darkens those it does not see now, then draws a pixel for each model runtime with an
   alpha tint whose faction has a non-zero colorIndex, in the panel colour of variant
   colorIndex, or variant 0 (white) for units in the local selection, so the selection stands out (blended 50/50 for
   a tint alpha below 0xFF).
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
  int cellCount;
  int cellsRemaining;
  int copyRemaining;
  FactionRuntimeIndex activeFactionIndex;
  int gridColumn;
  int gridRow;
  int64_t roundedRowQ12;
  bool rowRoundingOverflows;
  uint32_t colorVariant;
  AssetRelativeOffset assetOffset;
  uint8_t *pixelCursor;
  FieldGridCell *fieldCell;
  uint8_t *plane0Pixels;
  uint8_t *plane0WriteCursor;
  bool notSelected;
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
  for (ownerNode = (inGameRoot->worldRuntime).ownerListHead; ownerNode != NULL;
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
          pixelArgb = ((GraphicsPaletteTextureSourceAsset *)g_InGamePanelTextureSource)->paletteEntries[TERRAIN_MINIMAP_PANEL_COLOR_FACTION_FIRST +((GraphicsTextureSourceEntry *)((uint8_t *)panelTextureSource + assetOffset))[36].paletteIndex * 4 + colorVariant].argb8888;
          if (ownerNode->modelTintArgb < ARGB8888_ALPHA_MASK) {
            pixelArgb = ((pixelArgb & TERRAIN_ARGB_HALVE_MASK) +
                         (*(uint32_t *)(plane0Pixels + (gridRow * textureWidth + gridColumn) * 4) &
                          TERRAIN_ARGB_HALVE_MASK)) >> 1;
          }
          *(uint32_t *)(plane0Pixels + (gridRow * textureWidth + gridColumn) * 4) = pixelArgb;
        }
      }
    }
  }
}


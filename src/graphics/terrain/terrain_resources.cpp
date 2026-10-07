/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/src/graphics/terrain/terrain_resources.cpp
 * Reverse engineering by idkFoxes 2026
 */

#include <thandor/graphics/terrain/terrain_resources.h>
#include <thandor/thandor.h>
#include <thandor/platform/bootstrap/image.h>

/* Module data. */

/* one texture set per terrain material (26 used, TERRAIN_MATERIAL_COUNT); the remaining 12 entries are NULL. Original quirk: UiCommandMatrix_SelectIndex fills twelve swatches from a page base that can reach 15, so it reads entry 26 (always NULL, an empty swatch). */
THANDOR_ALIGN(4) GraphicsTextureSet *g_TerrainMaterialTextureSets[38] = {};

/* path suffix letters "a".."z" (with terminator) of the 26 terrain material texture sets */
static const TerrainMaterialSuffixEntry g_TerrainMaterialTextureSuffixLettersUtf16AtoZ[26] = {
    {'a'}, {'b'}, {'c'}, {'d'}, {'e'}, {'f'}, {'g'}, {'h'}, {'i'}, {'j'}, {'k'}, {'l'}, {'m'},
    {'n'}, {'o'}, {'p'}, {'q'}, {'r'}, {'s'}, {'t'}, {'u'}, {'v'}, {'w'}, {'x'}, {'y'}, {'z'}};

GraphicsTextureSet *g_TerrainPrimaryTextureSet = nullptr;

void *g_TerrainSoilPacketTablePayload = nullptr;

void *g_TerrainSurfacePacketTablePayload = nullptr;

GraphicsPaletteAsset *g_TerrainPrimaryPalette = nullptr;

GraphicsPaletteAsset *g_TerrainSecondaryPalette = nullptr;



/* The suffix slot of a secondary resource path: its terminator, or its 256th character when there is none
   within the first 256 (the path is scanned for at most 256 characters). The material letter and the
   terminator are written there. */
static TerrainMaterialSuffixEntry *TerrainVisualResources_FindPathSuffixEntry(uint16_t *resourcePath)

{
  int pathLength;

  for (pathLength = 0; (pathLength < 255) && (resourcePath[pathLength] != 0); pathLength++) {
  }
  return reinterpret_cast<TerrainMaterialSuffixEntry *>(resourcePath + pathLength); /* the suffix record is written over the path's end */
}

/* Loads the 26 material texture sets <secondary>a..z.gfx into g_TerrainMaterialTextureSets: those whose bit is
   set in fieldFlags are required (advancing the loading movie before and after each), the others optional
   (NULL when missing). Also sets the loading movie span from the number of required sets. Returns true on
   success; false with the load error in *outError when a required set fails. */
static Bool8 TerrainVisualResources_LoadMaterialTextureSets
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
      g_TerrainMaterialTextureSets[materialIndex] = g_GraphicsTextureSetLoadPackage(secondaryResourcePath,nullptr);
    }
    else {
      MoviePlayback_AdvanceScheduledFrameAndTick();
      materialTextureSet = g_GraphicsTextureSetLoadPackage(secondaryResourcePath,&loadErrorCode);
      if (materialTextureSet == nullptr) {
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
static Bool8 TerrainVisualResources_LoadTablesAndPalettes
          (uint16_t *primaryResourcePath,uint16_t *secondaryResourcePath,
          TerrainMaterialSuffixEntry *pathSuffixEntry,uint32_t *outError)

{
  void *packetTable;
  GraphicsTextureSet *primaryTextureSet;
  GraphicsPaletteAsset *palette;
  uint32_t loadErrorCode;

  WidePath_SetExtensionCode(WIDE_PATH_EXTENSION_DAT,primaryResourcePath); /* ".dat" */
  packetTable = Package_LoadEntry(primaryResourcePath,&loadErrorCode);
  if (packetTable == nullptr) {
    *outError = loadErrorCode;
    return false;
  }
  MoviePlayback_AdvanceScheduledFrameAndTick();
  g_TerrainSurfacePacketTablePayload = static_cast<uint8_t *>(packetTable) + TERRAIN_PACKET_TABLE_HEADER_BYTES;
  WidePath_SetExtensionCode(ASSET_MAGIC_GFX,primaryResourcePath);
  primaryTextureSet = g_GraphicsTextureSetLoadPackage(primaryResourcePath,&loadErrorCode);
  if (primaryTextureSet == nullptr) {
    *outError = loadErrorCode;
    return false;
  }
  MoviePlayback_AdvanceScheduledFrameAndTick();
  g_TerrainPrimaryTextureSet = primaryTextureSet;
  WidePath_SetExtensionCode(ASSET_MAGIC_PAL,primaryResourcePath);
  palette = g_GraphicsPaletteAssetLoadPackage(primaryResourcePath,&loadErrorCode);
  if (palette == nullptr) {
    *outError = loadErrorCode;
    return false;
  }
  MoviePlayback_AdvanceScheduledFrameAndTick();
  g_TerrainPrimaryPalette = palette;
  pathSuffixEntry->lowercaseLetterUtf16 = 0;
  pathSuffixEntry->terminator = 0;
  WidePath_SetExtensionCode(ASSET_MAGIC_PAL,secondaryResourcePath);
  palette = g_GraphicsPaletteAssetLoadPackage(secondaryResourcePath,&loadErrorCode);
  if (palette == nullptr) {
    *outError = loadErrorCode;
    return false;
  }
  MoviePlayback_AdvanceScheduledFrameAndTick();
  g_TerrainSecondaryPalette = palette;
  pathSuffixEntry->lowercaseLetterUtf16 = 0;
  pathSuffixEntry->terminator = 0;
  WidePath_SetExtensionCode(WIDE_PATH_EXTENSION_DAT,secondaryResourcePath); /* ".dat" */
  packetTable = Package_LoadEntry(secondaryResourcePath,&loadErrorCode);
  if (packetTable == nullptr) {
    *outError = loadErrorCode;
    return false;
  }
  MoviePlayback_AdvanceScheduledFrameAndTick();
  g_TerrainSoilPacketTablePayload = static_cast<uint8_t *>(packetTable) + TERRAIN_PACKET_TABLE_HEADER_BYTES;
  return true;
}

/* Random animation for the 256 direction records: scales 0x80..0x9F, rates +-(0x200..0x27F) per step, random
   start angles; the angle components are cleared. */
static void TerrainDirectionTable_RandomizeRecords()

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
    /* packedAngles holds the two 16-bit angles in its low and high word */
    reinterpret_cast<short *>(&directionRecord->packedAngles)[0] = (short)randomValue; /* angle A */
    randomValue = Random_NextPrimary();
    directionRecord->scaleB = (randomValue & TERRAIN_DIRECTION_SCALE_RANDOM_MASK) + TERRAIN_DIRECTION_SCALE_MIN;
    rotationRate = ((uint16_t)(randomValue >> 16) & TERRAIN_DIRECTION_RATE_RANDOM_MASK) +
                   TERRAIN_DIRECTION_RATE_MIN_ANGLE16;
    randomValue = Random_NextPrimary();
    if ((int)randomValue < 0) {
      rotationRate = -rotationRate;
    }
    directionRecord->rateB = rotationRate;
    reinterpret_cast<short *>(&directionRecord->packedAngles)[1] = (short)randomValue; /* angle B */
    directionRecord->angleAComponent0ScaledQ28 = 0;
    directionRecord->angleAComponent1ScaledQ28 = 0;
    directionRecord->angleBComponent0ScaledQ28 = 0;
    directionRecord++;
  }
}

/* Loads the terrain graphics of a field (fld asset with valid dimensions, FieldGrid_ValidateLoadedImage, else
   FATAL_ERROR_FIELD_ASSET_INVALID): the 26 material texture sets <secondary>a..z.gfx (those flagged in field->fieldFlags are required, the others optional),
   <primary>.dat/.gfx/.pal and <secondary>.pal/.dat, then initialises the field's runtime cells and the
   animated direction table. Advances the loading movie between steps. Returns true on success; on failure
   returns false and stores the error (field check or failed resource load) in *outError (untouched on success).
*/
Bool8 TerrainVisualResources_LoadPrimary
          (uint16_t *primaryResourcePath,uint16_t *secondaryResourcePath,FieldGridAsset *field,
          uint32_t *outError)

{
  TerrainMaterialSuffixEntry *pathSuffixEntry;

  pathSuffixEntry = TerrainVisualResources_FindPathSuffixEntry(secondaryResourcePath);
  /* The original checks only magic and converter; the dimensions bounded here as well (level data) */
  if (((field->common).magic != ASSET_MAGIC_FLD) ||
      ((field->common).converterVersion != PCK_CONVERTER_FLD_SHT_00060006) ||
      !FieldGrid_ValidateLoadedImage(field,(field->common).allocationSizeBytes)) {
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
Bool8 TerrainVisualResources_LoadAndClearCellOverlayFlags
          (uint16_t *primaryResourcePath,uint16_t *secondaryResourcePath,FieldGridAsset *field,
          uint32_t *outError)

{
  TerrainMaterialSuffixEntry *pathSuffixEntry;
  FieldGridCell *fieldCell;
  int cellsRemaining;

  pathSuffixEntry = TerrainVisualResources_FindPathSuffixEntry(secondaryResourcePath);
  /* The original checks only magic and converter; the dimensions bounded here as well (level data) */
  if (((field->common).magic != ASSET_MAGIC_FLD) ||
      ((field->common).converterVersion != PCK_CONVERTER_FLD_SHT_00060006) ||
      !FieldGrid_ValidateLoadedImage(field,(field->common).allocationSizeBytes)) {
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
  /* Original quirk: a do-while, a count of 0 runs it 2^32 times (D8: kept for step 11) */
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
void TerrainVisualResources_Shutdown()

{
  int materialTextureSetsRemaining;
  GraphicsTextureSet **materialTextureSetCursor;
  void *surfacePacketTablePayload;

  materialTextureSetCursor = g_TerrainMaterialTextureSets;
  materialTextureSetsRemaining = TERRAIN_MATERIAL_TEXTURE_SET_COUNT;
  for (; materialTextureSetsRemaining != 0; materialTextureSetsRemaining--) {
    if (*materialTextureSetCursor != nullptr) {
      g_GraphicsTextureSetReleasePackage(*materialTextureSetCursor);
      *materialTextureSetCursor = nullptr;
    }
    materialTextureSetCursor++;
  }
  g_GraphicsTextureSetReleasePackage(g_TerrainPrimaryTextureSet);
  g_GraphicsPaletteAssetLifecycleCallbacks3.releasePackage(g_TerrainSecondaryPalette);
  g_GraphicsPaletteAssetLifecycleCallbacks3.releasePackage(g_TerrainPrimaryPalette);
  surfacePacketTablePayload = g_TerrainSurfacePacketTablePayload;
  if (g_TerrainSoilPacketTablePayload != nullptr) {
    Resource_Release(reinterpret_cast<void *>((uintptr_t)g_TerrainSoilPacketTablePayload - TERRAIN_PACKET_TABLE_HEADER_BYTES));
  }
  if (surfacePacketTablePayload != nullptr) {
    Resource_Release(reinterpret_cast<void *>((uintptr_t)surfacePacketTablePayload - TERRAIN_PACKET_TABLE_HEADER_BYTES));
  }
  g_TerrainPrimaryTextureSet = nullptr;
  g_TerrainSecondaryPalette = nullptr;
  g_TerrainPrimaryPalette = nullptr;
  g_TerrainSoilPacketTablePayload = nullptr;
  g_TerrainSurfacePacketTablePayload = nullptr;
}

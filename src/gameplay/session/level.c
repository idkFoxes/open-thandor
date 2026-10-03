/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/src/gameplay/session/level.c
 * Reverse engineering by idkFoxes 2026
 */

#include <thandor/gameplay/session/level.h>
#include <thandor/thandor.h>

/* Module data. */

/* L"army.hex" */
__declspec(align(4)) uint16_t g_ArmyHexPathUtf16[9] = {'a', 'r', 'm', 'y', '.', 'h', 'e', 'x', 0};

/* L"effect.hex" */
uint16_t g_EffectHexPathUtf16[11] = {'e', 'f', 'f', 'e', 'c', 't', '.', 'h', 'e', 'x', 0};

/* L"shot.hex" */
uint16_t g_ShotHexPathUtf16[9] = {'s', 'h', 'o', 't', '.', 'h', 'e', 'x', 0};

/* L"modul.hex" */
uint16_t g_ModulHexPathUtf16[10] = {'m', 'o', 'd', 'u', 'l', '.', 'h', 'e', 'x', 0};

/* L"field.hex" */
uint16_t g_FieldHexPathUtf16[10] = {'f', 'i', 'e', 'l', 'd', '.', 'h', 'e', 'x', 0};

/* L"light.hex" */
uint16_t g_LightHexPathUtf16[10] = {'l', 'i', 'g', 'h', 't', '.', 'h', 'e', 'x', 0};

/* L"widget.hex" */
uint16_t g_WidgetHexPathUtf16[11] = {'w', 'i', 'd', 'g', 'e', 't', '.', 'h', 'e', 'x', 0};

/* L"level.hex" */
uint16_t g_LevelHexPathUtf16[10] = {'l', 'e', 'v', 'e', 'l', '.', 'h', 'e', 'x', 0};

uint16_t g_LevelEndingMovieSourcePath[256] = {0};

uint32_t g_InGameLevelTitleTextResourceIndex = 0;

uint32_t g_InGameLevelCampaignAssociationIndex = 0;

DirectSoundVoiceSet *g_InGameLevelEffectVoiceSets[4] = {0, 0, 0, 0};

DirectSoundVoiceSet *g_InGameLevelMusicVoiceSets[4] = {0, 0, 0, 0};

InGameLevelRuntimeGlobalBlock20 g_InGameLevelRuntimeGlobalBlock = {.playerSlotByteOffsets = {0, 32, 64, 96, 128, 160, 192}};

uint32_t g_MoviePlaybackBaseFrameGroup = 0;

uint32_t g_MoviePlaybackScheduleCounter = 0;

uint32_t g_MoviePlaybackScheduleSpan = 0;

uint32_t g_SoundPackageHandle = 0;

/* L"sound\\level00.sam" */
static uint16_t g_SoundLevel00SamPathUtf16[18] =
    {'s', 'o', 'u', 'n', 'd', '\\', 'l', 'e', 'v', 'e', 'l', '0', '0', '.', 's', 'a', 'm', 0};

/* L"sound\\music00.sam" */
static uint16_t g_SessionMusic00SamPathUtf16[18] =
    {'s', 'o', 'u', 'n', 'd', '\\', 'm', 'u', 's', 'i', 'c', '0', '0', '.', 's', 'a', 'm', 0};

static void **g_InGameLoadedResourcePointers = 0;

static InGameLoadedResourcePointerCount g_InGameLoadedResourcePointerCount = 0;

static uint16_t g_InGameLevelSoundLeafOrCombinedPathScratchUtf16[256] = {0};

static uint16_t g_InGameLevelSoundParentDirectoryScratchUtf16[256] = {0};

/* Implementation ownership: gameplay/session/level. */

/* Prepares the movies of a level before it is loaded: stores the level's loading movie (the path at
   header pathOffsets.endingMovieBasePathOffset, with its extension set to "flm") in *outMoviePath and writes
   the matching end movie number into "flm\ende0000.flm" from the 5th and 6th characters of that path
   ('w' 0xFC -> 2, "ei" -> 3, "la" -> 4, anything else 0). It keeps a copy of the level path in
   g_LevelEndingMovieSourcePath, which the loaders report on errors. Returns true on success; false with FATAL_ERROR_LEVEL_ASSET_INVALID in *outError when asset is not a LEV
   asset of converter version 0x70001.
*/
Bool8 LevelAsset_PrepareEndingMoviePath
          (uint16_t *currentLevelPath,LevelAssetHeader *asset,uint16_t **outMoviePath,uint32_t *outError)

{
  int movieNameChars4And5;
  int remainingDwordCount;
  int32_t movieNumber;
  uint16_t *sourcePathCursor;
  uint8_t *path;
  
  if (((asset->common).magic == ASSET_MAGIC_LEV) &&
     ((asset->common).converterVersion == PCK_CONVERTER_LEV_00070001)) {
    path = (uint8_t *)asset + (asset->pathOffsets).endingMovieBasePathOffset;
    remainingDwordCount = 128; /* 256 UTF-16 code units of the level path */
    /* UTF-16 characters 4 and 5 of the movie path, read as one dword */
    movieNameChars4And5 = *(int *)(path + 8);
    movieNumber = 0;
    if (movieNameChars4And5 == LEVEL_ENDING_MOVIE_NAME_W_UUML) {
      movieNumber = 2;
    }
    else if (movieNameChars4And5 == LEVEL_ENDING_MOVIE_NAME_EI) {
      movieNumber = 3;
    }
    else if (movieNameChars4And5 == LEVEL_ENDING_MOVIE_NAME_LA) {
      movieNumber = 4;
    }
    WidePath_SetExtensionCode(ASSET_MAGIC_FLM,(uint16_t *)path);
    g_WideNumberFormatUtf16
              (WIDE_FORMAT_PAD_WITH_ZERO,0,4,1,movieNumber,
               (uint16_t *)(g_SessionEndMoviePathUtf16 + 8)); /* the "0000" */
    sourcePathCursor = g_LevelEndingMovieSourcePath;
    for (; remainingDwordCount != 0; remainingDwordCount--) {
      *(uint32_t *)sourcePathCursor = *(uint32_t *)currentLevelPath;
      currentLevelPath = currentLevelPath + 2;
      sourcePathCursor = sourcePathCursor + 2;
    }
    *outMoviePath = (uint16_t *)path;
    return true;
  }
  Package_SetLastErrorPath(currentLevelPath);
  *outError = FATAL_ERROR_LEVEL_ASSET_INVALID;
  return false;
}


/* LEV level file ('lev', converter version 0x70001) as read by the two level loaders below. Offsets are
   byte offsets from the start of the file image; every "path" field holds the offset of a UTF-16 path string.
     +0x000  common asset prefix (magic, allocation size, format and converter version, build metadata)
     +0x0B0  field-grid (FLD) path; the frontend replaces it with the loaded FieldGridAsset pointer before the
             loaders run, so they pass the field unchanged
     +0x0B4  ground, +0x0B8 surface, +0x0BC sky, +0x0C0 army, +0x0C4 shot, +0x0C8 effect texture base paths
     +0x0CC  loading movie path (LevelAsset_PrepareEndingMoviePath)
     +0x0D0  sound directory: every 'sam' file in it whose name ends in a number n becomes spatial sound slot n
     +0x0D4  technology file ('tec')
     +0x0D8  number of initial army placements
     +0x0DC  used twice: byte size of the level prefix copied into g_InGameLevelRuntimeGlobalBlock.conditionStorage
             (it includes this header, the player slots, the tail and the condition schedule) and file offset of
             the placement table, which directly follows that prefix
     +0x0E0  ARM, +0x0E8 MDL, +0x0F0 EFF, +0x0F8 SHT file lists: record count, then the table offset; each record
             is 0x40 bytes (a 32-character UTF-16 path whose extension the loader sets)
     +0x170  title text resource id; +0x190 campaign association index
     +0x200  seven 0x20-byte player slots for factions 1..7: start camera X, Y, Z (Q12), camera distance, heading
             (low 16 bits) and pitch (high 16 bits), start Xenite and Tritium (Q4), and a value that is added to
             the faction number to form the faction's class/mode
     +0x2E0  tail: field-region origin and size (packed 16:16), terrain base, ramp and lighting colours;
             +0x310 assignable and +0x314 active faction count, +0x318 relation UI flags, +0x31C / +0x320 faction
             groups whose members start in relation state 8 / 4 (four 8-bit masks each), +0x350 four music and
             +0x360 four effect sample numbers (sound\music%02d.sam, sound\level%02d.sam; 0 = none)
   Initial army placement (0x20 bytes): +0x00 army asset id, +0x04 owner faction (spawned only while that faction
   is active), +0x08 / +0x0C position (passed as the worldYQ12 / worldXQ12 parameters of
   ArmyRuntime_CreateInstanceFromAsset), +0x10 rotation angle.
   Both loaders return true on success, or false with a FATAL_ERROR_* code or the code of the failing step in
   *outError; the progress of
   the loading movie is driven through g_MoviePlaybackBaseFrameGroup / ScheduleSpan and
   MoviePlayback_AdvanceScheduledFrameAndTick, one frame group per loading stage (0..6).
*/

/* Stores error in *outError and returns false: the common failure exit of the new-level loader below. */
static Bool8 NewLevel_Fail(uint32_t *outError,uint32_t error)

{
  *outError = error;
  return false;
}


/* Allocates g_InGameLevelRuntimeGlobalBlock.conditionStorage and copies the level prefix (header
   resourceTables.runtimePrefixByteSizeAndInitialArmyPlacementOffset bytes) into it dword by dword. */
static Bool8 NewLevel_CopyRuntimePrefix(LevelAssetRuntimePrefix *levelImage,uint32_t *outError)

{
  uint32_t prefixByteSize;
  uint32_t remainingDwordCount;
  uint32_t allocError;
  void *conditionStorage;
  uint32_t *copySourceCursor;
  uint32_t *copyTargetCursor;

  prefixByteSize = (levelImage->header).resourceTables.runtimePrefixByteSizeAndInitialArmyPlacementOffset;
  allocError = g_MemoryApi.alloc(prefixByteSize,&conditionStorage);
  if (allocError != 0) {
    return NewLevel_Fail(outError,allocError);
  }
  copySourceCursor = (uint32_t *)levelImage;
  copyTargetCursor = (uint32_t *)conditionStorage;
  g_InGameLevelRuntimeGlobalBlock.conditionStorage = (InGameLevelConditionStorage *)conditionStorage;
  for (remainingDwordCount = prefixByteSize >> 2; remainingDwordCount != 0; remainingDwordCount--) {
    *copyTargetCursor = *copySourceCursor;
    copySourceCursor++;
    copyTargetCursor++;
  }
  return true;
}


/* Loads the level's technology file (pathOffsets.technologyPathOffset) into g_TechnologyAsset and checks that it
   is a TEC asset of converter version 0x20000. */
static Bool8 NewLevel_LoadTechnology(LevelAssetRuntimePrefix *levelImage,uint32_t *outError)

{
  uint16_t *technologyPath;
  TechnologyAsset *loadedTechnologyAsset;
  uint32_t loadErrorCode;

  technologyPath = (uint16_t *)((uint8_t *)levelImage + (levelImage->header).pathOffsets.technologyPathOffset);
  WidePath_SetExtensionCode(ASSET_MAGIC_TEC,technologyPath);
  loadedTechnologyAsset = (TechnologyAsset *)Package_LoadEntry(technologyPath,&loadErrorCode);
  if (loadedTechnologyAsset == NULL) {
    return NewLevel_Fail(outError,loadErrorCode);
  }
  g_TechnologyAsset = loadedTechnologyAsset;
  if (((loadedTechnologyAsset->header).common.magic != ASSET_MAGIC_TEC) ||
      ((loadedTechnologyAsset->header).common.converterVersion != PCK_CONVERTER_TEC_00020000)) {
    return NewLevel_Fail(outError,FATAL_ERROR_TECHNOLOGY_ASSET_INVALID);
  }
  return true;
}


/* Player slots (playerSlots): camera bookmarks 1-7, start resources and class/mode of factions 1-7, plus the
   active faction count of the tail. */
static void NewLevel_ApplyPlayerSlots(LevelAssetRuntimePrefix *levelImage)

{
  g_LevelCameraBookmark1PositionXQ12 = levelImage->playerSlots[0].startCameraXQ12;
  g_LevelCameraBookmark1PositionYQ12 = levelImage->playerSlots[0].startCameraYQ12;
  g_LevelCameraBookmark1PositionZQ12 = levelImage->playerSlots[0].startCameraZQ12;
  g_LevelCameraBookmark1PositionMagnitudeQ12 = levelImage->playerSlots[0].startCameraMagnitudeQ12;
  g_LevelCameraBookmark1PackedHeadingLow16PitchHigh16 = levelImage->playerSlots[0].packedHeadingLow16PitchHigh16;
  g_LevelCameraBookmark2PositionXQ12 = levelImage->playerSlots[1].startCameraXQ12;
  g_LevelCameraBookmark2PositionYQ12 = levelImage->playerSlots[1].startCameraYQ12;
  g_LevelCameraBookmark2PositionZQ12 = levelImage->playerSlots[1].startCameraZQ12;
  g_LevelCameraBookmark2PositionMagnitudeQ12 = levelImage->playerSlots[1].startCameraMagnitudeQ12;
  g_LevelCameraBookmark2PackedHeadingLow16PitchHigh16 = levelImage->playerSlots[1].packedHeadingLow16PitchHigh16;
  g_LevelCameraBookmark3PositionXQ12 = levelImage->playerSlots[2].startCameraXQ12;
  g_LevelCameraBookmark3PositionYQ12 = levelImage->playerSlots[2].startCameraYQ12;
  g_LevelCameraBookmark3PositionZQ12 = levelImage->playerSlots[2].startCameraZQ12;
  g_LevelCameraBookmark3PositionMagnitudeQ12 = levelImage->playerSlots[2].startCameraMagnitudeQ12;
  g_LevelCameraBookmark3PackedHeadingLow16PitchHigh16 = levelImage->playerSlots[2].packedHeadingLow16PitchHigh16;
  g_LevelCameraBookmark4PositionXQ12 = levelImage->playerSlots[3].startCameraXQ12;
  g_LevelCameraBookmark4PositionYQ12 = levelImage->playerSlots[3].startCameraYQ12;
  g_LevelCameraBookmark4PositionZQ12 = levelImage->playerSlots[3].startCameraZQ12;
  g_LevelCameraBookmark4PositionMagnitudeQ12 = levelImage->playerSlots[3].startCameraMagnitudeQ12;
  g_LevelCameraBookmark4PackedHeadingLow16PitchHigh16 = levelImage->playerSlots[3].packedHeadingLow16PitchHigh16;
  g_LevelCameraBookmark5PositionXQ12 = levelImage->playerSlots[4].startCameraXQ12;
  g_LevelCameraBookmark5PositionYQ12 = levelImage->playerSlots[4].startCameraYQ12;
  g_LevelCameraBookmark5PositionZQ12 = levelImage->playerSlots[4].startCameraZQ12;
  g_LevelCameraBookmark5PositionMagnitudeQ12 = levelImage->playerSlots[4].startCameraMagnitudeQ12;
  g_LevelCameraBookmark5PackedHeadingLow16PitchHigh16 = levelImage->playerSlots[4].packedHeadingLow16PitchHigh16;
  g_LevelCameraBookmark6PositionXQ12 = levelImage->playerSlots[5].startCameraXQ12;
  g_LevelCameraBookmark6PositionYQ12 = levelImage->playerSlots[5].startCameraYQ12;
  g_LevelCameraBookmark6PositionZQ12 = levelImage->playerSlots[5].startCameraZQ12;
  g_LevelCameraBookmark6PositionMagnitudeQ12 = levelImage->playerSlots[5].startCameraMagnitudeQ12;
  g_LevelCameraBookmark6PackedHeadingLow16PitchHigh16 = levelImage->playerSlots[5].packedHeadingLow16PitchHigh16;
  g_LevelCameraBookmark7PositionXQ12 = levelImage->playerSlots[6].startCameraXQ12;
  g_LevelCameraBookmark7PositionYQ12 = levelImage->playerSlots[6].startCameraYQ12;
  g_LevelCameraBookmark7PositionZQ12 = levelImage->playerSlots[6].startCameraZQ12;
  g_LevelCameraBookmark7PositionMagnitudeQ12 = levelImage->playerSlots[6].startCameraMagnitudeQ12;
  g_LevelCameraBookmark7PackedHeadingLow16PitchHigh16 = levelImage->playerSlots[6].packedHeadingLow16PitchHigh16;
  g_GameFactionRuntimeImage.records[1].xeniteCurrentQ4 = levelImage->playerSlots[0].startXeniteQ4;
  g_GameFactionRuntimeImage.records[1].tritiumCurrentQ4 = levelImage->playerSlots[0].startTritiumQ4;
  g_GameFactionRuntimeImage.records[2].xeniteCurrentQ4 = levelImage->playerSlots[1].startXeniteQ4;
  g_GameFactionRuntimeImage.records[2].tritiumCurrentQ4 = levelImage->playerSlots[1].startTritiumQ4;
  g_GameFactionRuntimeImage.records[3].xeniteCurrentQ4 = levelImage->playerSlots[2].startXeniteQ4;
  g_GameFactionRuntimeImage.records[3].tritiumCurrentQ4 = levelImage->playerSlots[2].startTritiumQ4;
  g_GameFactionRuntimeImage.records[4].xeniteCurrentQ4 = levelImage->playerSlots[3].startXeniteQ4;
  g_GameFactionRuntimeImage.records[4].tritiumCurrentQ4 = levelImage->playerSlots[3].startTritiumQ4;
  g_GameFactionRuntimeImage.records[5].xeniteCurrentQ4 = levelImage->playerSlots[4].startXeniteQ4;
  g_GameFactionRuntimeImage.records[5].tritiumCurrentQ4 = levelImage->playerSlots[4].startTritiumQ4;
  g_GameFactionRuntimeImage.records[6].xeniteCurrentQ4 = levelImage->playerSlots[5].startXeniteQ4;
  g_GameFactionRuntimeImage.records[6].tritiumCurrentQ4 = levelImage->playerSlots[5].startTritiumQ4;
  g_GameFactionRuntimeImage.records[7].xeniteCurrentQ4 = levelImage->playerSlots[6].startXeniteQ4;
  g_GameFactionRuntimeImage.records[7].tritiumCurrentQ4 = levelImage->playerSlots[6].startTritiumQ4;
  g_GameFactionRuntimeImage.records[1].colorIndex = levelImage->playerSlots[0].aiClassOrMode + 1;
  g_GameFactionRuntimeImage.records[2].colorIndex = levelImage->playerSlots[1].aiClassOrMode + 2;
  g_GameFactionRuntimeImage.records[3].colorIndex = levelImage->playerSlots[2].aiClassOrMode + 3;
  g_GameFactionRuntimeImage.tail.activeFactionCount = (levelImage->worldSettings).activeFactionCount;
  g_GameFactionRuntimeImage.records[4].colorIndex = levelImage->playerSlots[3].aiClassOrMode + 4;
  g_GameFactionRuntimeImage.records[5].colorIndex = levelImage->playerSlots[4].aiClassOrMode + 5;
  g_GameFactionRuntimeImage.records[6].colorIndex = levelImage->playerSlots[5].aiClassOrMode + 6;
  g_GameFactionRuntimeImage.records[7].colorIndex = levelImage->playerSlots[6].aiClassOrMode + 7;
}


/* Prepares one loaded file of a LEV file list; false with the step's error code in *outError. */
typedef Bool8 (*NewLevelPrepareAssetFn)(void *asset,uint32_t *outError);

static Bool8 NewLevel_PrepareEffectAsset(void *asset,uint32_t *outError)

{
  return EffectAsset_PrepareEntries((EffectAssetHeader *)asset,outError);
}

static Bool8 NewLevel_PrepareShotAsset(void *asset,uint32_t *outError)

{
  *outError = ShotAsset_PrepareEntries((ShotAssetHeader *)asset);
  return *outError == 0;
}

static Bool8 NewLevel_PrepareModelAsset(void *asset,uint32_t *outError)

{
  return ModelAsset_PrepareRecords((ModelAssetHeader *)asset,outError);
}

static Bool8 NewLevel_PrepareArmyAsset(void *asset,uint32_t *outError)

{
  *outError = ArmyAsset_PrepareRecords((ArmyAssetHeader *)asset);
  return *outError == 0;
}


/* Loads one LEV file list (EFF, SHT, MDL or ARM; 0x40-byte path records at pathTableOffset): sets each path's
   extension, loads the file, appends it at *loadedResourceCursor to g_InGameLoadedResourcePointers and prepares
   it. One loading-movie step per file. */
static Bool8 NewLevel_LoadAssetList
          (LevelAssetRuntimePrefix *levelImage,LevelAssetRelativeByteOffset pathTableOffset,
           LevelAssetRecordCount remainingRecordCount,PackedFileExtensionCode32 extensionCode,
           NewLevelPrepareAssetFn prepareAsset,void ***loadedResourceCursor,uint32_t *outError)

{
  uint16_t *assetPathCursor;
  void *loadedAsset;
  uint32_t loadErrorCode;
  uint32_t prepareError;

  assetPathCursor = (uint16_t *)((uint8_t *)levelImage + pathTableOffset);
  for (; remainingRecordCount != 0; remainingRecordCount--) {
    WidePath_SetExtensionCode(extensionCode,assetPathCursor);
    if (INGAME_LOADED_RESOURCE_CAPACITY - 1 < g_InGameLoadedResourcePointerCount) {
      return NewLevel_Fail(outError,FATAL_ERROR_LEVEL_TOO_MANY_RESOURCES);
    }
    loadedAsset = Package_LoadEntry(assetPathCursor,&loadErrorCode);
    if (loadedAsset == NULL) {
      return NewLevel_Fail(outError,loadErrorCode);
    }
    **loadedResourceCursor = loadedAsset;
    g_InGameLoadedResourcePointerCount++;
    (*loadedResourceCursor)++;
    if (!prepareAsset(loadedAsset,&prepareError)) {
      return NewLevel_Fail(outError,prepareError);
    }
    MoviePlayback_AdvanceScheduledFrameAndTick();
    assetPathCursor = assetPathCursor + LEVEL_ASSET_PATH_RECORD_UNITS; /* next path record */
  }
  return true;
}


/* Loading stages 1 to 5: terrain textures (surface, ground) and the field grid, the model pool, the army,
   shot and effect texture sets (pathOffsets.armyTextureBasePathOffset, shotTextureBasePathOffset,
   effectTextureBasePathOffset), then the terrain lighting of the tail. */
static Bool8 NewLevel_InitTerrainAndGraphics
          (LevelAssetRuntimePrefix *levelImage,WorldRuntimeContext *worldRuntime,uint32_t *outError)

{
  uint16_t *armyTextureBasePath;
  uint16_t *effectTextureBasePath;
  uint32_t stepError;

  g_MoviePlaybackBaseFrameGroup = 1;
  g_MoviePlaybackScheduleCounter = 0;
  g_MoviePlaybackScheduleSpan = LEVEL_LOAD_MOVIE_SPAN_HOLD;
  if (!TerrainVisualResources_LoadPrimary
         ((uint16_t *)((uint8_t *)levelImage + (levelImage->header).pathOffsets.surfaceTextureBasePathOffset),
          (uint16_t *)((uint8_t *)levelImage + (levelImage->header).pathOffsets.groundTextureBasePathOffset),
          (FieldGridAsset *)(levelImage->header).pathOffsets.levelPathOffset,&stepError)) {
    return NewLevel_Fail(outError,stepError);
  }
  stepError = ShotDefinitions_ValidateTerrainMaterialReferences();
  if (stepError != 0) {
    return NewLevel_Fail(outError,stepError);
  }
  g_MoviePlaybackBaseFrameGroup = 2;
  g_MoviePlaybackScheduleCounter = 0;
  g_MoviePlaybackScheduleSpan = LEVEL_LOAD_MOVIE_SPAN_HOLD;
  stepError = ModelRuntimePool_Init();
  if (stepError != 0) {
    return NewLevel_Fail(outError,stepError);
  }
  armyTextureBasePath =
       (uint16_t *)((uint8_t *)levelImage + (levelImage->header).pathOffsets.armyTextureBasePathOffset);
  effectTextureBasePath =
       (uint16_t *)((uint8_t *)levelImage + (levelImage->header).pathOffsets.effectTextureBasePathOffset);
  if (!ArmyRuntime_InitializePoolAndGraphics(worldRuntime,armyTextureBasePath,&stepError)) {
    return NewLevel_Fail(outError,stepError);
  }
  g_MoviePlaybackBaseFrameGroup = 3;
  g_MoviePlaybackScheduleCounter = 0;
  g_MoviePlaybackScheduleSpan = 4;
  if (!ShotRuntime_InitGraphicsResources
         ((uint16_t *)((uint8_t *)levelImage + (levelImage->header).pathOffsets.shotTextureBasePathOffset),
          &stepError)) {
    return NewLevel_Fail(outError,stepError);
  }
  g_MoviePlaybackBaseFrameGroup = 4;
  g_MoviePlaybackScheduleCounter = 0;
  g_MoviePlaybackScheduleSpan = 4;
  if (!EffectRuntime_InitGraphicsResources(effectTextureBasePath,&stepError)) {
    return NewLevel_Fail(outError,stepError);
  }
  g_MoviePlaybackBaseFrameGroup = 5;
  g_MoviePlaybackScheduleCounter = 0;
  g_MoviePlaybackScheduleSpan = 6;
  WorldRuntime_SetTerrainLightingConfiguration
            ((levelImage->worldSettings).terrainLightingColor13CArgb,
             (levelImage->worldSettings).terrainLightingColor138Argb,
             (levelImage->worldSettings).terrainLightingColor134Argb,
             (levelImage->worldSettings).terrainLightingColor130Argb,
             (levelImage->worldSettings).terrainSecondaryColorArgb,
             (levelImage->worldSettings).terrainLightingColor128Argb,
             (levelImage->worldSettings).terrainBaseColorArgb,
             (levelImage->worldSettings).terrainRampStepColorArgb,worldRuntime);
  return true;
}


/* Attaches the field grid, sets the start camera from the local faction's player slot and lights the field
   region given by the tail. */
static void NewLevel_PlaceStartCameraAndLightFieldRegion
          (LevelAssetRuntimePrefix *levelImage,WorldRuntimeContext *worldRuntime)

{
  int localFactionIndex;
  LevelPlayerSlotByteOffset32 playerSlotByteOffset;
  struct LevelPlayerSlotRecord *startSlot;
  uint32_t packedHeadingLow16PitchHigh16;
  uint32_t packedRegionOriginYHigh16XLow16;
  uint32_t packedRegionHeightHigh16WidthLow16;

  localFactionIndex = worldRuntime->activeFactionRuntimeIndex;
  MoviePlayback_AdvanceScheduledFrameAndTick();
  playerSlotByteOffset = g_InGameLevelRuntimeGlobalBlock.playerSlotByteOffsets[localFactionIndex - 1];
  WorldRuntime_AttachFieldGridAsset
            ((FieldGridAsset *)(levelImage->header).pathOffsets.levelPathOffset,worldRuntime);
  MoviePlayback_AdvanceScheduledFrameAndTick();
  startSlot = (struct LevelPlayerSlotRecord *)((uint8_t *)&levelImage->playerSlots[0] + playerSlotByteOffset);
  packedHeadingLow16PitchHigh16 = startSlot->packedHeadingLow16PitchHigh16;
  WorldRuntime_SetCameraPositionKeepingTarget
            (startSlot->startCameraZQ12,startSlot->startCameraYQ12,startSlot->startCameraXQ12,worldRuntime);
  WorldRuntime_SetCameraAnglesAndMagnitudeClamped
            (2,(int)packedHeadingLow16PitchHigh16 >> 16,packedHeadingLow16PitchHigh16 & 0xffff,
             startSlot->startCameraMagnitudeQ12,worldRuntime);
  packedRegionOriginYHigh16XLow16 = (levelImage->worldSettings).packedFieldRegionOriginYHigh16XLow16;
  WorldRuntime_RecomputeMotionEndpointAgainstFieldSurface(worldRuntime);
  WorldRuntime_CommitCameraTargetDistance(worldRuntime);
  packedRegionHeightHigh16WidthLow16 = (levelImage->worldSettings).packedFieldRegionHeightHigh16WidthLow16;
  WorldRuntime_RecomputeFieldRegionNormalsAndLighting
            ((int)packedRegionHeightHigh16WidthLow16 >> 16,packedRegionHeightHigh16WidthLow16 & 0xffff,
             (int)packedRegionOriginYHigh16XLow16 >> 16,packedRegionOriginYHigh16XLow16 & 0xffff,worldRuntime);
}


/* Spawns the initial armies (LevelInitialArmyPlacementRecord20 records at LEV +[0xDC]) of the active factions. */
static Bool8 NewLevel_SpawnInitialArmies
          (LevelAssetRuntimePrefix *levelImage,WorldRuntimeContext *worldRuntime,uint32_t *outError)

{
  LevelAssetRecordCount remainingRecordCount;
  LevelInitialArmyPlacementRecord20 *placementCursor;
  ArmyRuntimeSlot *createdArmy;
  uint32_t armyCreateError;

  remainingRecordCount = (levelImage->header).initialArmyPlacementRecordCount;
  placementCursor = (LevelInitialArmyPlacementRecord20 *)
                    ((uint8_t *)levelImage +
                     (levelImage->header).resourceTables.runtimePrefixByteSizeAndInitialArmyPlacementOffset);
  MoviePlayback_AdvanceScheduledFrameAndTick();
  for (; remainingRecordCount != 0; remainingRecordCount--) {
    if (g_GameFactionRuntimeImage.tail.factionLifecycleStates[placementCursor->factionIndex] ==
        FACTION_RUNTIME_LIFECYCLE_ACTIVE) {
      createdArmy = ArmyRuntime_CreateInstanceFromAsset
                         (ARMY_CREATE_COUNT_FOR_ACTIVE_FACTION | ARMY_CREATE_UNLOCK_TECHNOLOGY,
                          placementCursor->orientationAngle,placementCursor->worldXQ12,
                          placementCursor->worldYQ12,placementCursor->factionIndex,
                          placementCursor->armyAssetId,worldRuntime,&armyCreateError);
      if (createdArmy == NULL) {
        return NewLevel_Fail(outError,armyCreateError);
      }
    }
    placementCursor++;
  }
  return true;
}


/* Spatial sound slots (loading stage 6): cleared, then filled from the level's sound directory
   (pathOffsets.soundBasePathOffset), listed from the sound package or, failing that, from disk. A 'sam' file
   whose name ends in the number n becomes slot n; one loading-movie step per loaded sound. */
static Bool8 NewLevel_LoadSpatialSounds
          (LevelAssetRuntimePrefix *levelImage,WorldRuntimeContext *worldRuntime,uint32_t *outError)

{
  uint32_t *soundSlotCursor;
  WorldWorkspaceElementCount remainingSoundSlotCount;
  uint16_t *soundDirectoryPath;
  void *directoryListing;
  PckOutputCapacityBytes listingCapacityBytes;
  Bool8 soundsInPackage; /* the sounds are listed from g_SoundPackageHandle, not a directory */
  uint32_t listedSoundCount;
  uint32_t soundDirectoryRecordSizeBytes;
  uint32_t allocError;
  uint32_t shrinkError;
  uint16_t *listedSoundPath;
  uint32_t soundIndex;
  Bool8 sampleLoaded;
  void *loadedSample;
  uint32_t loadErrorCode;
  SpatialSoundSlot *soundSlot;

  soundSlotCursor = worldRuntime->dwordArray;
  for (remainingSoundSlotCount = worldRuntime->dwordArrayCount; remainingSoundSlotCount != 0;
      remainingSoundSlotCount--) {
    *soundSlotCursor = 0;
    soundSlotCursor++;
  }
  soundDirectoryPath =
       (uint16_t *)((uint8_t *)levelImage + (levelImage->header).pathOffsets.soundBasePathOffset);
  WidePath_SetExtensionCode(ASSET_MAGIC_SAM,soundDirectoryPath);
  WidePath_SplitParentAndLeaf
            (g_InGameLevelSoundLeafOrCombinedPathScratchUtf16,
             g_InGameLevelSoundParentDirectoryScratchUtf16,soundDirectoryPath);
  allocError = g_MemoryApi.allocLargestFreeBlock(&directoryListing,&listingCapacityBytes);
  if (allocError != 0) {
    return NewLevel_Fail(outError,allocError);
  }
  soundsInPackage = Package_FindEntry(listingCapacityBytes,(PckEntryHeader *)directoryListing,soundDirectoryPath,
                                      g_SoundPackageHandle,&listedSoundCount);
  soundDirectoryRecordSizeBytes = PCK_ENTRY_HEADER_BYTES;
  if (!soundsInPackage) {
    listedSoundCount = g_FileSystemEnumerateDirectoryOrVolumeEntries
                         (FILESYSTEM_ENUMERATE_FILES,UINT32_MAX,listingCapacityBytes,(uint8_t *)directoryListing,
                          (uint8_t *)soundDirectoryPath);
    soundDirectoryRecordSizeBytes = FILESYSTEM_ENUMERATION_RECORD_BYTES;
  }
  g_MoviePlaybackBaseFrameGroup = 6;
  g_MoviePlaybackScheduleCounter = 0;
  g_MoviePlaybackScheduleSpan = listedSoundCount;
  shrinkError = g_MemoryApi.shrinkInPlace(soundDirectoryRecordSizeBytes * listedSoundCount,directoryListing);
  if (shrinkError != 0) {
    g_MemoryApi.free(directoryListing);
    return NewLevel_Fail(outError,shrinkError);
  }
  if (worldRuntime->dwordArrayCount < listedSoundCount) {
    listedSoundCount = worldRuntime->dwordArrayCount;
  }
  listedSoundPath = (uint16_t *)directoryListing;
  for (; listedSoundCount != 0; listedSoundCount--) {
    soundSlotCursor = worldRuntime->dwordArray;
    soundIndex = WidePath_ParseTrailingNumberBeforeExtension(listedSoundPath);
    if (soundIndex < worldRuntime->dwordArrayCount) {
      if (soundsInPackage) {
        sampleLoaded = Resource_Load(listedSoundPath,&loadedSample,NULL,&loadErrorCode);
      }
      else {
        WidePath_CombineDirectoryAndLeaf
                  (g_InGameLevelSoundLeafOrCombinedPathScratchUtf16,listedSoundPath,
                   g_InGameLevelSoundParentDirectoryScratchUtf16);
        sampleLoaded = Resource_Load(g_InGameLevelSoundLeafOrCombinedPathScratchUtf16,
                                     &loadedSample,NULL,&loadErrorCode);
      }
      if (!sampleLoaded) {
        g_MemoryApi.free(directoryListing);
        return NewLevel_Fail(outError,loadErrorCode);
      }
      soundSlot = SpatialSoundSlot_CreateFromSampleAsset((SoundSampleAsset *)loadedSample);
      if (soundSlot != NULL) {
        soundSlotCursor[soundIndex] = (uint32_t)soundSlot;
      }
      Resource_Release(loadedSample);
      MoviePlayback_AdvanceScheduledFrameAndTick();
    }
    /* advance by one directory record */
    listedSoundPath = (uint16_t *)((uint8_t *)listedSoundPath + soundDirectoryRecordSizeBytes);
  }
  g_MemoryApi.free(directoryListing);
  return true;
}


/* Loads one level sample (sound\level%02d.sam or sound\music%02d.sam: sampleNumber is written into the
   template path at character 11) into a voice set; 0 means none. A failed load or voice set leaves
   *outVoiceSet unchanged. */
static void NewLevel_LoadLevelSample(uint32_t sampleNumber,uint16_t *pathTemplate,DirectSoundVoiceSet **outVoiceSet)

{
  void *loadedSampleBuffer;
  DirectSoundVoiceSet *createdVoiceSet;

  if (sampleNumber == 0) {
    return;
  }
  g_WideNumberFormatUtf16(WIDE_FORMAT_PAD_WITH_ZERO,0,2,1,sampleNumber,pathTemplate + 11);
  if (Resource_Load(pathTemplate,&loadedSampleBuffer,NULL,NULL)) {
    if (g_SoundCreateSampleVoiceSet((SoundSampleAsset *)loadedSampleBuffer,&createdVoiceSet) == 0) {
      *outVoiceSet = createdVoiceSet;
    }
    Resource_Release((SoundSampleAsset *)loadedSampleBuffer);
  }
}


/* Resets the in-game effect and music voice state and loads the four level effect and four music samples of
   the tail. */
static void NewLevel_LoadLevelSamples(void)

{
  struct LevelWorldSettings *worldSettings;

  g_InGameLevelEffectVoiceSets[0] = NULL;
  g_InGameLevelEffectVoiceSets[1] = NULL;
  g_InGameLevelEffectVoiceSets[2] = NULL;
  g_InGameLevelEffectVoiceSets[3] = NULL;
  g_InGameActiveEffectVoice = 0;
  g_InGameEffectsEnabled = 1;
  g_InGameActiveMusicVoice = 0;
  g_InGameMusicNextTrackCountdown = 1;
  g_InGameLevelMusicVoiceSets[0] = NULL;
  g_InGameLevelMusicVoiceSets[1] = NULL;
  g_InGameLevelMusicVoiceSets[2] = NULL;
  g_InGameLevelMusicVoiceSets[3] = NULL;
  worldSettings = &(g_InGameLevelRuntimeGlobalBlock.conditionStorage->levelImage).worldSettings;
  NewLevel_LoadLevelSample(worldSettings->effectSampleNumbers[0],g_SoundLevel00SamPathUtf16,
                           &g_InGameLevelEffectVoiceSets[0]);
  NewLevel_LoadLevelSample(worldSettings->effectSampleNumbers[1],g_SoundLevel00SamPathUtf16,
                           &g_InGameLevelEffectVoiceSets[1]);
  NewLevel_LoadLevelSample(worldSettings->effectSampleNumbers[2],g_SoundLevel00SamPathUtf16,
                           &g_InGameLevelEffectVoiceSets[2]);
  NewLevel_LoadLevelSample(worldSettings->effectSampleNumbers[3],g_SoundLevel00SamPathUtf16,
                           &g_InGameLevelEffectVoiceSets[3]);
  NewLevel_LoadLevelSample(worldSettings->musicSampleNumbers[0],g_SessionMusic00SamPathUtf16,
                           &g_InGameLevelMusicVoiceSets[0]);
  NewLevel_LoadLevelSample(worldSettings->musicSampleNumbers[1],g_SessionMusic00SamPathUtf16,
                           &g_InGameLevelMusicVoiceSets[1]);
  NewLevel_LoadLevelSample(worldSettings->musicSampleNumbers[2],g_SessionMusic00SamPathUtf16,
                           &g_InGameLevelMusicVoiceSets[2]);
  NewLevel_LoadLevelSample(worldSettings->musicSampleNumbers[3],g_SessionMusic00SamPathUtf16,
                           &g_InGameLevelMusicVoiceSets[3]);
}


/* Default build list for factions that start with class-18 models but no structure: the last registered army
   assets (army flags bit 0) whose model class is 0x0B, 0x0E without / with the model's classParameterC0 value,
   and 0x10. Nothing is assigned unless both a class-0x0B and a class-0x0E asset without classParameterC0 value
   exist. */
static void NewLevel_AssignDefaultBuildLists(WorldRuntimeContext *worldRuntime)

{
  ArmyAssetRecordPrefix *class0BArmyDefinition;
  ArmyAssetRecordPrefix *class0ENoExtraArmyDefinition;
  ArmyAssetRecordPrefix *class0EArmyDefinition;
  ArmyAssetRecordPrefix *class10ArmyDefinition;
  ArmyAssetRecordPrefix *registryArmyDefinition;
  ModelDefinitionRecordPrefix *rootModelDefinition;
  ModelRuntimeClassId rootClassId;
  int registrySlot;
  uint32_t factionIndex;
  uint32_t factionModelFlags; /* bit 0: has a model with a group-A class command, bit 1: has a class-18 model */
  WorldOwnerListNode *ownerListNode;
  ModelRuntimeSlot *modelSlot;
  int modelClassId;

  class0BArmyDefinition = NULL;
  class0ENoExtraArmyDefinition = NULL;
  class0EArmyDefinition = NULL; /* this and class10ArmyDefinition are uninitialized in the original */
  class10ArmyDefinition = NULL;
  for (registrySlot = 0; registrySlot < ARMY_ASSET_REGISTRY_SLOT_COUNT; registrySlot++) {
    registryArmyDefinition = g_ArmyAssetRecordRegistry[registrySlot];
    if ((registryArmyDefinition == NULL) || ((((ArmyAssetRecord *)registryArmyDefinition)->flags & 1) == 0)) {
      continue;
    }
    rootModelDefinition = ModelDefinitionRegistry_FindById
                       (((ArmyModelTreeNode *)registryArmyDefinition->rootNodeOffsetOrPointer)->
                        linkedDefinitionIds[0]);
    if (rootModelDefinition == NULL) {
      /* Original quirk: a failed lookup is not checked; its error code is read as the definition */
      rootModelDefinition = (ModelDefinitionRecordPrefix *)FATAL_ERROR_MODEL_DEFINITION_MISSING;
    }
    rootClassId = ((ModelDefinition *)rootModelDefinition)->runtimeClassId;
    if (rootClassId == MODEL_RUNTIME_CLASS_11) {
      class0BArmyDefinition = registryArmyDefinition;
    }
    else if (rootClassId == MODEL_RUNTIME_CLASS_16) {
      class10ArmyDefinition = registryArmyDefinition;
    }
    else if (rootClassId == MODEL_RUNTIME_CLASS_14) {
      if (((ModelDefinition *)rootModelDefinition)->classParameterC0 == 0) {
        class0ENoExtraArmyDefinition = registryArmyDefinition;
      }
      else {
        class0EArmyDefinition = registryArmyDefinition;
      }
    }
  }
  if ((class0BArmyDefinition == NULL) || (class0ENoExtraArmyDefinition == NULL) ||
      (worldRuntime->ownerListHead == NULL)) {
    return;
  }
  /* factions 1..activeFactionCount; faction 1 is checked even when the count is 0 */
  factionIndex = 1;
  do {
    factionModelFlags = 0;
    for (ownerListNode = worldRuntime->ownerListHead; ownerListNode != NULL;
        ownerListNode = ownerListNode->nextNode) {
      if (ownerListNode->ownerClassId != WORLD_OWNER_RUNTIME_MODEL) {
        continue;
      }
      modelSlot = (ModelRuntimeSlot *)ownerListNode->runtimePayload;
      if (factionIndex == modelSlot->ownerArmyRuntimeOrSavedOffset.armyRuntime->factionIndex) {
        modelClassId = modelSlot->definitionOrSavedId.runtimeDefinition->runtimeClassId;
        if (modelClassId == MODEL_RUNTIME_CLASS_18) {
          factionModelFlags = factionModelFlags | 2;
        }
        else if (g_ArmyRuntimeOrderHandlerMatrix11Columns24Classes.classCommand[modelClassId] ==
                 ArmyRuntime_ClassCommandHandlerGroupA) {
          factionModelFlags = factionModelFlags | 1;
        }
      }
    }
    if (factionModelFlags == 2) {
      g_GameFactionRuntimeImage.records[factionIndex].primaryArmyAssetPointersOrIds[0] =
           (uint32_t)class0BArmyDefinition;
      g_GameFactionRuntimeImage.records[factionIndex].primaryArmyAssetPointersOrIds[1] =
           (uint32_t)class0ENoExtraArmyDefinition;
      g_GameFactionRuntimeImage.records[factionIndex].primaryArmyAssetPointersOrIds[2] =
           (uint32_t)class0EArmyDefinition;
      g_GameFactionRuntimeImage.records[factionIndex].primaryArmyAssetPointersOrIds[3] =
           (uint32_t)class10ArmyDefinition;
      g_GameFactionRuntimeImage.records[factionIndex].primaryArmyAssetCount = 4;
    }
    factionIndex++;
  } while (factionIndex <= g_GameFactionRuntimeImage.tail.activeFactionCount);
}


/* Gives every faction pair inside one 8-bit group of groupMasks (four groups, low byte first) the relation
   state relationState in both directions.
   Original quirk: when the first group is empty, the later groups are skipped too. */
static void NewLevel_ApplyGroupRelations(uint32_t groupMasks,FactionRelationStateNibble relationState)

{
  uint32_t sourceFactionIndex;
  uint32_t targetFactionIndex;

  if ((groupMasks & 0xff) == 0) {
    return;
  }
  for (; groupMasks != 0; groupMasks = groupMasks >> 8) {
    for (sourceFactionIndex = 0; sourceFactionIndex < 8; sourceFactionIndex++) {
      if ((groupMasks & (1u << sourceFactionIndex)) == 0) {
        continue;
      }
      for (targetFactionIndex = sourceFactionIndex + 1; targetFactionIndex < 8; targetFactionIndex++) {
        if ((groupMasks & (1u << targetFactionIndex)) != 0) {
          GameFactionRuntime_ApplyPairwiseRelationTransition
                    (32,32,relationState,relationState,targetFactionIndex,sourceFactionIndex);
        }
      }
    }
  }
}


/* Initial relations: every pair inside one 8-bit faction group of relationState4FactionGroupMasks gets state 4/4,
   of relationState8FactionGroupMasks state 8/8; also copies the relation UI flags. */
static void NewLevel_ApplyInitialRelations(void)

{
  struct LevelWorldSettings *worldSettings;
  uint32_t relationState4FactionGroupMasks;
  uint32_t relationState8FactionGroupMasks;

  worldSettings = &(g_InGameLevelRuntimeGlobalBlock.conditionStorage->levelImage).worldSettings;
  relationState4FactionGroupMasks = worldSettings->relationState4FactionGroupMasks;
  relationState8FactionGroupMasks = worldSettings->relationState8FactionGroupMasks;
  g_GameFactionRuntimeImage.tail.relationUiFlags = worldSettings->relationUiFlags;
  NewLevel_ApplyGroupRelations(relationState4FactionGroupMasks,FACTION_RELATION_STATE_FRIENDLY);
  NewLevel_ApplyGroupRelations(relationState8FactionGroupMasks,FACTION_RELATION_STATE_ALLIED);
}


/* Loads a new level (after GameData_ResetDefaults): copies the level prefix, loads the technology file and all
   listed EFF/SHT/MDL/ARM files, initialises terrain, graphics pools, camera bookmarks, start resources and
   factions, spawns the initial armies, loads the level sounds, gives factions that start without a structure a
   default build list and applies the initial faction relations.
*/

Bool8 InGameLevelRuntime_LoadResourcesAfterDefaultReset
          (LevelAssetRuntimePrefix *levelImage,WorldRuntimeContext *worldRuntime,uint32_t *outError)

{
  void **loadedResourceCursor;
  uint32_t allocError;
  uint32_t stepError;

  allocError = g_MemoryApi.alloc(INGAME_LOADED_RESOURCE_CAPACITY * 4,(void **)&loadedResourceCursor);
  if (allocError != 0) {
    return NewLevel_Fail(outError,allocError);
  }
  g_InGameLoadedResourcePointerCount = 0;
  g_InGameLoadedResourcePointers = loadedResourceCursor;
  Package_SetLastErrorPath(g_LevelEndingMovieSourcePath);
  if (((levelImage->header).common.magic != ASSET_MAGIC_LEV) ||
      ((levelImage->header).common.converterVersion != PCK_CONVERTER_LEV_00070001)) {
    return NewLevel_Fail(outError,FATAL_ERROR_LEVEL_ASSET_INVALID);
  }
  if (!NewLevel_CopyRuntimePrefix(levelImage,outError)) {
    return false;
  }
  g_InGameLevelTitleTextResourceIndex = (levelImage->header).titleTextResourceIndex;
  g_InGameLevelCampaignAssociationIndex = (levelImage->header).campaignAssociationIndex;
  if (!NewLevel_LoadTechnology(levelImage,outError)) {
    return false;
  }
  NewLevel_ApplyPlayerSlots(levelImage);
  /* loading stage 0: one movie step per EFF, SHT, MDL and ARM file */
  g_MoviePlaybackScheduleSpan =
       (levelImage->header).resourceTables.effectAssetPathCount +
       (levelImage->header).resourceTables.shotAssetPathCount +
       (levelImage->header).resourceTables.modelAssetPathCount +
       (levelImage->header).resourceTables.armyAssetPathCount;
  g_MoviePlaybackBaseFrameGroup = 0;
  g_MoviePlaybackScheduleCounter = 0;
  if (!NewLevel_LoadAssetList(levelImage,(levelImage->header).resourceTables.effectAssetPathTableOffset,
                              (levelImage->header).resourceTables.effectAssetPathCount,ASSET_MAGIC_EFF,
                              NewLevel_PrepareEffectAsset,&loadedResourceCursor,outError) ||
      !NewLevel_LoadAssetList(levelImage,(levelImage->header).resourceTables.shotAssetPathTableOffset,
                              (levelImage->header).resourceTables.shotAssetPathCount,ASSET_MAGIC_SHT,
                              NewLevel_PrepareShotAsset,&loadedResourceCursor,outError)) {
    return false;
  }
  stepError = EffectDefinitions_ResolveCrossReferences();
  if (stepError != 0) {
    return NewLevel_Fail(outError,stepError);
  }
  if (!NewLevel_LoadAssetList(levelImage,(levelImage->header).resourceTables.modelAssetPathTableOffset,
                              (levelImage->header).resourceTables.modelAssetPathCount,ASSET_MAGIC_MDL,
                              NewLevel_PrepareModelAsset,&loadedResourceCursor,outError) ||
      !NewLevel_LoadAssetList(levelImage,(levelImage->header).resourceTables.armyAssetPathTableOffset,
                              (levelImage->header).resourceTables.armyAssetPathCount,ASSET_MAGIC_ARM,
                              NewLevel_PrepareArmyAsset,&loadedResourceCursor,outError)) {
    return false;
  }
  if (g_InGameLoadedResourcePointerCount >= INGAME_LOADED_RESOURCE_CAPACITY) {
    return NewLevel_Fail(outError,FATAL_ERROR_LEVEL_TOO_MANY_RESOURCES);
  }
  if (!NewLevel_InitTerrainAndGraphics(levelImage,worldRuntime,outError)) {
    return false;
  }
  NewLevel_PlaceStartCameraAndLightFieldRegion(levelImage,worldRuntime);
  if (!NewLevel_SpawnInitialArmies(levelImage,worldRuntime,outError)) {
    return false;
  }
  MoviePlayback_AdvanceScheduledFrameAndTick();
  /* signature differs: the callbacks' context is WorldRuntimeContext *, the slot's void * */
  WorldRuntime_ForEachOwnerListNode
            (worldRuntime,
             (WorldRuntimeNodeTraversalCallback *)ArmyRuntimeNode_AccumulateTerrainOcclusionAndOccupancyCallback,
             worldRuntime);
  WorldRuntime_ForEachOwnerListNode
            (worldRuntime,
             (WorldRuntimeNodeTraversalCallback *)ArmyRuntimeNode_RebuildTerrainOccupancyAndVisualStateCallback,
             worldRuntime);
  FieldGrid_ClassifyCellFlagsToRuntimeByte(worldRuntime->activeFactionRuntimeIndex,worldRuntime->fieldGrid);
  MoviePlayback_AdvanceScheduledFrameAndTick();
  if (!NewLevel_LoadSpatialSounds(levelImage,worldRuntime,outError)) {
    return false;
  }
  NewLevel_LoadLevelSamples();
  NewLevel_AssignDefaultBuildLists(worldRuntime);
  NewLevel_ApplyInitialRelations();
  return true;
}


/* Loading stages 1 to 5 of a saved game: as NewLevel_InitTerrainAndGraphics, but the terrain loader also clears
   the cell overlay flags, and the army references of the saved faction image are rebased before the terrain
   lighting is set. */
static Bool8 SavedLevel_InitTerrainAndGraphics
          (LevelAssetRuntimePrefix *levelImage,WorldRuntimeContext *worldRuntime,uint32_t *outError)

{
  uint16_t *armyTextureBasePath;
  uint16_t *effectTextureBasePath;
  uint32_t stepError;

  g_MoviePlaybackBaseFrameGroup = 1;
  g_MoviePlaybackScheduleCounter = 0;
  g_MoviePlaybackScheduleSpan = LEVEL_LOAD_MOVIE_SPAN_HOLD;
  if (!TerrainVisualResources_LoadAndClearCellOverlayFlags
         ((uint16_t *)((uint8_t *)levelImage + (levelImage->header).pathOffsets.surfaceTextureBasePathOffset),
          (uint16_t *)((uint8_t *)levelImage + (levelImage->header).pathOffsets.groundTextureBasePathOffset),
          (FieldGridAsset *)(levelImage->header).pathOffsets.levelPathOffset,&stepError)) {
    return NewLevel_Fail(outError,stepError);
  }
  stepError = ShotDefinitions_ValidateTerrainMaterialReferences();
  if (stepError != 0) {
    return NewLevel_Fail(outError,stepError);
  }
  g_MoviePlaybackBaseFrameGroup = 2;
  g_MoviePlaybackScheduleCounter = 0;
  g_MoviePlaybackScheduleSpan = LEVEL_LOAD_MOVIE_SPAN_HOLD;
  stepError = ModelRuntimePool_Init();
  if (stepError != 0) {
    return NewLevel_Fail(outError,stepError);
  }
  armyTextureBasePath =
       (uint16_t *)((uint8_t *)levelImage + (levelImage->header).pathOffsets.armyTextureBasePathOffset);
  effectTextureBasePath =
       (uint16_t *)((uint8_t *)levelImage + (levelImage->header).pathOffsets.effectTextureBasePathOffset);
  if (!ArmyRuntime_InitializePoolAndGraphics(worldRuntime,armyTextureBasePath,&stepError)) {
    return NewLevel_Fail(outError,stepError);
  }
  g_MoviePlaybackBaseFrameGroup = 3;
  g_MoviePlaybackScheduleCounter = 0;
  g_MoviePlaybackScheduleSpan = 4;
  if (!ShotRuntime_InitGraphicsResources
         ((uint16_t *)((uint8_t *)levelImage + (levelImage->header).pathOffsets.shotTextureBasePathOffset),
          &stepError)) {
    return NewLevel_Fail(outError,stepError);
  }
  g_MoviePlaybackBaseFrameGroup = 4;
  g_MoviePlaybackScheduleCounter = 0;
  g_MoviePlaybackScheduleSpan = 4;
  if (!EffectRuntime_InitGraphicsResources(effectTextureBasePath,&stepError)) {
    return NewLevel_Fail(outError,stepError);
  }
  g_MoviePlaybackBaseFrameGroup = 5;
  g_MoviePlaybackScheduleCounter = 0;
  g_MoviePlaybackScheduleSpan = 6;
  GameFactionRuntime_RebaseLoadedArmyReferences();
  WorldRuntime_SetTerrainLightingConfiguration
            ((levelImage->worldSettings).terrainLightingColor13CArgb,
             (levelImage->worldSettings).terrainLightingColor138Argb,
             (levelImage->worldSettings).terrainLightingColor134Argb,
             (levelImage->worldSettings).terrainLightingColor130Argb,
             (levelImage->worldSettings).terrainSecondaryColorArgb,
             (levelImage->worldSettings).terrainLightingColor128Argb,
             (levelImage->worldSettings).terrainBaseColorArgb,
             (levelImage->worldSettings).terrainRampStepColorArgb,worldRuntime);
  return true;
}


/* As NewLevel_PlaceStartCameraAndLightFieldRegion, with one more loading-movie step before the field region is
   lit. */
static void SavedLevel_PlaceStartCameraAndLightFieldRegion
          (LevelAssetRuntimePrefix *levelImage,WorldRuntimeContext *worldRuntime)

{
  int localFactionIndex;
  LevelPlayerSlotByteOffset32 playerSlotByteOffset;
  struct LevelPlayerSlotRecord *startSlot;
  uint32_t packedHeadingLow16PitchHigh16;
  uint32_t packedRegionOriginYHigh16XLow16;
  uint32_t packedRegionHeightHigh16WidthLow16;

  localFactionIndex = worldRuntime->activeFactionRuntimeIndex;
  MoviePlayback_AdvanceScheduledFrameAndTick();
  playerSlotByteOffset = g_InGameLevelRuntimeGlobalBlock.playerSlotByteOffsets[localFactionIndex - 1];
  WorldRuntime_AttachFieldGridAsset
            ((FieldGridAsset *)(levelImage->header).pathOffsets.levelPathOffset,worldRuntime);
  MoviePlayback_AdvanceScheduledFrameAndTick();
  startSlot = (struct LevelPlayerSlotRecord *)((uint8_t *)&levelImage->playerSlots[0] + playerSlotByteOffset);
  packedHeadingLow16PitchHigh16 = startSlot->packedHeadingLow16PitchHigh16;
  WorldRuntime_SetCameraPositionKeepingTarget
            (startSlot->startCameraZQ12,startSlot->startCameraYQ12,startSlot->startCameraXQ12,worldRuntime);
  WorldRuntime_SetCameraAnglesAndMagnitudeClamped
            (2,(int)packedHeadingLow16PitchHigh16 >> 16,packedHeadingLow16PitchHigh16 & 0xffff,
             startSlot->startCameraMagnitudeQ12,worldRuntime);
  packedRegionOriginYHigh16XLow16 = (levelImage->worldSettings).packedFieldRegionOriginYHigh16XLow16;
  WorldRuntime_RecomputeMotionEndpointAgainstFieldSurface(worldRuntime);
  WorldRuntime_CommitCameraTargetDistance(worldRuntime);
  packedRegionHeightHigh16WidthLow16 = (levelImage->worldSettings).packedFieldRegionHeightHigh16WidthLow16;
  MoviePlayback_AdvanceScheduledFrameAndTick();
  WorldRuntime_RecomputeFieldRegionNormalsAndLighting
            ((int)packedRegionHeightHigh16WidthLow16 >> 16,packedRegionHeightHigh16WidthLow16 & 0xffff,
             (int)packedRegionOriginYHigh16XLow16 >> 16,packedRegionOriginYHigh16XLow16 & 0xffff,worldRuntime);
}


/* Loads one saved runtime pool (a .hex entry of the save package) into its buffer; false with the load error in
   *outError, which stays unchanged on success. */
static Bool8 SavedLevel_LoadRuntimePool
          (PckLoadCapacityFlags bufferCapacity,uint8_t *destination,uint16_t *path,uint32_t *outError)

{
  uint32_t loadResult; /* Package_LoadEntryIntoBuffer: byte count on success, error code on failure */

  if (!Package_LoadEntryIntoBuffer(bufferCapacity,destination,path,&loadResult)) {
    return NewLevel_Fail(outError,loadResult);
  }
  return true;
}


/* Loads the saved runtime pools (widget.hex, army.hex, modul.hex, effect.hex, shot.hex, light.hex) over the
   freshly initialised ones and rebases their pointers. */
static Bool8 SavedLevel_LoadRuntimePools(WorldRuntimeContext *worldRuntime,uint32_t *outError)

{
  if (!SavedLevel_LoadRuntimePool(worldRuntime->objectCount * sizeof(WorldObjectRecord),
                                  (uint8_t *)worldRuntime->objectArray,(uint16_t *)g_WidgetHexPathUtf16,
                                  outError) ||
      !SavedLevel_LoadRuntimePool(ARMY_RUNTIME_SLOT_COUNT * sizeof(ArmyRuntimeSlot),(uint8_t *)g_ArmyRuntimeSlots,
                                  (uint16_t *)g_ArmyHexPathUtf16,outError) ||
      !SavedLevel_LoadRuntimePool(MODEL_RUNTIME_POOL_BYTES,(uint8_t *)g_ModelRuntimeSlots,
                                  (uint16_t *)g_ModulHexPathUtf16,outError) ||
      !SavedLevel_LoadRuntimePool(EFFECT_RUNTIME_POOL_BYTES,(uint8_t *)g_EffectRuntimeSlots,
                                  (uint16_t *)g_EffectHexPathUtf16,outError) ||
      !SavedLevel_LoadRuntimePool(SHOT_RUNTIME_POOL_BYTES,(uint8_t *)g_ShotRuntimeSlots,
                                  (uint16_t *)g_ShotHexPathUtf16,outError) ||
      !SavedLevel_LoadRuntimePool(sizeof(g_GraphicsShadingRuntimeRecords),
                                  (uint8_t *)g_GraphicsShadingRuntimeRecords,(uint16_t *)g_LightHexPathUtf16,
                                  outError)) {
    return false;
  }
  ArmyRuntimePool_RebaseAfterLoad();
  ModelRuntimePool_RebaseAfterLoad();
  ShotRuntime_RebaseSlotsAfterLoad();
  EffectRuntime_RebaseSlotsAfterLoad();
  ResourceRegistrationRuntime_RebaseLoadedRecords((ResourceRegistrationRuntimeImage *)worldRuntime);
  RuntimeHexSegment_ToggleLightImageFlag();
  return true;
}


/* Spatial sound slots of a saved game (loading stage 6): as NewLevel_LoadSpatialSounds, with one more
   loading-movie step after the sound directory's extension is set. */
static Bool8 SavedLevel_LoadSpatialSounds
          (LevelAssetRuntimePrefix *levelImage,WorldRuntimeContext *worldRuntime,uint32_t *outError)

{
  uint32_t *soundSlotCursor;
  WorldWorkspaceElementCount remainingSoundSlotCount;
  uint16_t *soundDirectoryPath;
  void *directoryListing;
  PckOutputCapacityBytes listingCapacityBytes;
  Bool8 soundsInPackage; /* the sounds are listed from g_SoundPackageHandle, not a directory */
  uint32_t listedSoundCount;
  uint32_t soundDirectoryRecordSizeBytes;
  uint32_t allocError;
  uint32_t shrinkError;
  uint16_t *listedSoundPath;
  uint32_t soundIndex;
  Bool8 sampleLoaded;
  void *loadedSample;
  uint32_t loadErrorCode;
  SpatialSoundSlot *soundSlot;

  soundSlotCursor = worldRuntime->dwordArray;
  for (remainingSoundSlotCount = worldRuntime->dwordArrayCount; remainingSoundSlotCount != 0;
      remainingSoundSlotCount--) {
    *soundSlotCursor = 0;
    soundSlotCursor++;
  }
  soundDirectoryPath =
       (uint16_t *)((uint8_t *)levelImage + (levelImage->header).pathOffsets.soundBasePathOffset);
  WidePath_SetExtensionCode(ASSET_MAGIC_SAM,soundDirectoryPath);
  MoviePlayback_AdvanceScheduledFrameAndTick();
  WidePath_SplitParentAndLeaf
            (g_InGameLevelSoundLeafOrCombinedPathScratchUtf16,
             g_InGameLevelSoundParentDirectoryScratchUtf16,soundDirectoryPath);
  allocError = g_MemoryApi.allocLargestFreeBlock(&directoryListing,&listingCapacityBytes);
  if (allocError != 0) {
    return NewLevel_Fail(outError,allocError);
  }
  soundsInPackage = Package_FindEntry(listingCapacityBytes,(PckEntryHeader *)directoryListing,soundDirectoryPath,
                                      g_SoundPackageHandle,&listedSoundCount);
  soundDirectoryRecordSizeBytes = PCK_ENTRY_HEADER_BYTES;
  if (!soundsInPackage) {
    listedSoundCount = g_FileSystemEnumerateDirectoryOrVolumeEntries
                         (FILESYSTEM_ENUMERATE_FILES,UINT32_MAX,listingCapacityBytes,(uint8_t *)directoryListing,
                          (uint8_t *)soundDirectoryPath);
    soundDirectoryRecordSizeBytes = FILESYSTEM_ENUMERATION_RECORD_BYTES;
  }
  g_MoviePlaybackBaseFrameGroup = 6;
  g_MoviePlaybackScheduleCounter = 0;
  g_MoviePlaybackScheduleSpan = listedSoundCount;
  shrinkError = g_MemoryApi.shrinkInPlace(soundDirectoryRecordSizeBytes * listedSoundCount,directoryListing);
  if (shrinkError != 0) {
    g_MemoryApi.free(directoryListing);
    return NewLevel_Fail(outError,shrinkError);
  }
  if (worldRuntime->dwordArrayCount < listedSoundCount) {
    listedSoundCount = worldRuntime->dwordArrayCount;
  }
  listedSoundPath = (uint16_t *)directoryListing;
  for (; listedSoundCount != 0; listedSoundCount--) {
    soundSlotCursor = worldRuntime->dwordArray;
    soundIndex = WidePath_ParseTrailingNumberBeforeExtension(listedSoundPath);
    if (soundIndex < worldRuntime->dwordArrayCount) {
      if (soundsInPackage) {
        sampleLoaded = Resource_Load(listedSoundPath,&loadedSample,NULL,&loadErrorCode);
      }
      else {
        WidePath_CombineDirectoryAndLeaf
                  (g_InGameLevelSoundLeafOrCombinedPathScratchUtf16,listedSoundPath,
                   g_InGameLevelSoundParentDirectoryScratchUtf16);
        sampleLoaded = Resource_Load(g_InGameLevelSoundLeafOrCombinedPathScratchUtf16,
                                     &loadedSample,NULL,&loadErrorCode);
      }
      if (!sampleLoaded) {
        g_MemoryApi.free(directoryListing);
        return NewLevel_Fail(outError,loadErrorCode);
      }
      soundSlot = SpatialSoundSlot_CreateFromSampleAsset((SoundSampleAsset *)loadedSample);
      if (soundSlot != NULL) {
        soundSlotCursor[soundIndex] = (uint32_t)soundSlot;
      }
      Resource_Release(loadedSample);
      MoviePlayback_AdvanceScheduledFrameAndTick();
    }
    /* advance by one directory record */
    listedSoundPath = (uint16_t *)((uint8_t *)listedSoundPath + soundDirectoryRecordSizeBytes);
  }
  g_MemoryApi.free(directoryListing);
  return true;
}


/* Loads the level of a saved game (after GameData_LoadExternalTables): the same LEV steps as
   InGameLevelRuntime_LoadResourcesAfterDefaultReset (see the layout above it) up to the camera, but instead of
   spawning the initial armies it restores the saved runtime pools from widget.hex, army.hex, modul.hex,
   effect.hex, shot.hex and light.hex and rebases their pointers. The player-slot copies, default build lists
   and initial relations are skipped; the saved faction image already holds them.
*/

Bool8 InGameLevelRuntime_LoadResourcesAfterExternalTables
          (FrontendLoadedLevelAsset *levelImage,WorldRuntimeContext *worldRuntime,uint32_t *outError)

{
  LevelAssetRuntimePrefix *levelPrefix;
  void **loadedResourceCursor;
  uint32_t allocError;
  uint32_t stepError;

  /* the same LEV image, viewed through the type the shared NewLevel_ steps take (identical layout) */
  levelPrefix = (LevelAssetRuntimePrefix *)levelImage;
  allocError = g_MemoryApi.alloc(INGAME_LOADED_RESOURCE_CAPACITY * 4,(void **)&loadedResourceCursor);
  if (allocError != 0) {
    return NewLevel_Fail(outError,allocError);
  }
  g_InGameLoadedResourcePointerCount = 0;
  g_InGameLoadedResourcePointers = loadedResourceCursor;
  Package_SetLastErrorPath(g_LevelEndingMovieSourcePath);
  if (((levelImage->header).common.magic != ASSET_MAGIC_LEV) ||
      ((levelImage->header).common.converterVersion != PCK_CONVERTER_LEV_00070001)) {
    return NewLevel_Fail(outError,FATAL_ERROR_LEVEL_ASSET_INVALID);
  }
  if (!NewLevel_CopyRuntimePrefix(levelPrefix,outError)) {
    return false;
  }
  g_InGameLevelTitleTextResourceIndex = (levelImage->header).titleTextResourceIndex;
  g_InGameLevelCampaignAssociationIndex = (levelImage->header).campaignAssociationIndex;
  if (!NewLevel_LoadTechnology(levelPrefix,outError)) {
    return false;
  }
  /* loading stage 0: one movie step per EFF, SHT, MDL and ARM file */
  g_MoviePlaybackScheduleSpan =
       (levelImage->header).resourceTables.effectAssetPathCount +
       (levelImage->header).resourceTables.shotAssetPathCount +
       (levelImage->header).resourceTables.modelAssetPathCount +
       (levelImage->header).resourceTables.armyAssetPathCount;
  g_MoviePlaybackBaseFrameGroup = 0;
  g_MoviePlaybackScheduleCounter = 0;
  if (!NewLevel_LoadAssetList(levelPrefix,(levelImage->header).resourceTables.effectAssetPathTableOffset,
                              (levelImage->header).resourceTables.effectAssetPathCount,ASSET_MAGIC_EFF,
                              NewLevel_PrepareEffectAsset,&loadedResourceCursor,outError) ||
      !NewLevel_LoadAssetList(levelPrefix,(levelImage->header).resourceTables.shotAssetPathTableOffset,
                              (levelImage->header).resourceTables.shotAssetPathCount,ASSET_MAGIC_SHT,
                              NewLevel_PrepareShotAsset,&loadedResourceCursor,outError)) {
    return false;
  }
  stepError = EffectDefinitions_ResolveCrossReferences();
  if (stepError != 0) {
    return NewLevel_Fail(outError,stepError);
  }
  if (!NewLevel_LoadAssetList(levelPrefix,(levelImage->header).resourceTables.modelAssetPathTableOffset,
                              (levelImage->header).resourceTables.modelAssetPathCount,ASSET_MAGIC_MDL,
                              NewLevel_PrepareModelAsset,&loadedResourceCursor,outError) ||
      !NewLevel_LoadAssetList(levelPrefix,(levelImage->header).resourceTables.armyAssetPathTableOffset,
                              (levelImage->header).resourceTables.armyAssetPathCount,ASSET_MAGIC_ARM,
                              NewLevel_PrepareArmyAsset,&loadedResourceCursor,outError)) {
    return false;
  }
  if (g_InGameLoadedResourcePointerCount >= INGAME_LOADED_RESOURCE_CAPACITY) {
    return NewLevel_Fail(outError,FATAL_ERROR_LEVEL_TOO_MANY_RESOURCES);
  }
  if (!SavedLevel_InitTerrainAndGraphics(levelPrefix,worldRuntime,outError)) {
    return false;
  }
  SavedLevel_PlaceStartCameraAndLightFieldRegion(levelPrefix,worldRuntime);
  if (!SavedLevel_LoadRuntimePools(worldRuntime,outError)) {
    return false;
  }
  MoviePlayback_AdvanceScheduledFrameAndTick();
  if (!SavedLevel_LoadSpatialSounds(levelPrefix,worldRuntime,outError)) {
    return false;
  }
  /* Original quirk: a failed level sample is ignored; the original even left its error code as this function's
     (nonzero, so successful) result unless a later sample overwrote it. */
  NewLevel_LoadLevelSamples();
  return true;
}


/* Releases everything a level loader set up: the effect, shot, model, army and terrain graphics, the spatial
   sound slots, the level's effect and music voices, every loaded EFF/SHT/MDL/ARM file, the copied level prefix
   and the technology file.
*/

void InGameLevelRuntime_ShutdownLoadedAssetResources(WorldRuntimeContext *worldRuntime)

{
  void **loadedResourceCursor;
  InGameLoadedResourcePointerCount remainingResourceCount;
  uint32_t remainingSlotCount;
  uint32_t *soundSlotCursor;
  
  EffectRuntime_ShutdownGraphicsResources();
  ShotRuntime_ShutdownGraphicsResources();
  ModelRuntimePool_ShutdownAndReleaseDefinitions();
  ArmyRuntime_ShutdownPoolAndGraphics();
  TerrainVisualResources_Shutdown();
  remainingSlotCount = worldRuntime->dwordArrayCount;
  soundSlotCursor = worldRuntime->dwordArray;
  /* the original loops only when both the slot count and the slot array are non-zero */
  if (remainingSlotCount != 0 && soundSlotCursor != NULL) {
    do {
      SpatialSoundSlot_ReleaseSample((SpatialSoundSlot *)*soundSlotCursor);
      soundSlotCursor++;
      remainingSlotCount--;
    } while (remainingSlotCount != 0);
  }
  g_SoundReleaseSampleVoiceSet(g_InGameLevelEffectVoiceSets[0]);
  g_SoundReleaseSampleVoiceSet(g_InGameLevelEffectVoiceSets[1]);
  g_SoundReleaseSampleVoiceSet(g_InGameLevelEffectVoiceSets[2]);
  g_SoundReleaseSampleVoiceSet(g_InGameLevelEffectVoiceSets[3]);
  g_SoundReleaseSampleVoiceSet(g_InGameLevelMusicVoiceSets[0]);
  g_SoundReleaseSampleVoiceSet(g_InGameLevelMusicVoiceSets[1]);
  g_SoundReleaseSampleVoiceSet(g_InGameLevelMusicVoiceSets[2]);
  g_SoundReleaseSampleVoiceSet(g_InGameLevelMusicVoiceSets[3]);
  loadedResourceCursor = g_InGameLoadedResourcePointers;
  remainingResourceCount = g_InGameLoadedResourcePointerCount;
  if (g_InGameLoadedResourcePointers != NULL) {
    for (; remainingResourceCount != 0; remainingResourceCount--) {
      Resource_Release(*loadedResourceCursor);
      loadedResourceCursor++;
    }
  }
  g_MemoryApi.free(g_InGameLoadedResourcePointers);
  g_InGameLoadedResourcePointers = NULL;
  g_InGameLoadedResourcePointerCount = 0;
  g_MemoryApi.free(g_InGameLevelRuntimeGlobalBlock.conditionStorage);
  g_InGameLevelRuntimeGlobalBlock.conditionStorage = NULL;
  Resource_Release(g_TechnologyAsset);
  g_TechnologyAsset = NULL;
  return;
}


/* Editor save of the current level: reloads the level asset (g_LevelEndingMovieSourcePath) into the package
   scratch buffer, replaces its placement table with one 0x20-byte record per live world model, stores the
   field region and the seven camera bookmarks and writes the image back to the same path. Returns true on
   success; on failure returns false with the load or write error in *outError. Called by
   InGameUiCommand_SaveFieldAndLevelAssetImages (ui/ingame/runtime.c); the field grid itself is written separately.
*/

Bool8 InGameLevelRuntime_SaveLevelAssetImageFromWorldState(InGameLevelSaveWorldView *saveWorldView,uint32_t *outError)

{
  uint32_t placementTableOffset;
  ArmyRuntimeSlot *armyRuntime;
  FactionRuntimeIndex activeFactionIndex;
  WorldOwnerListNode *ownerListNode;
  uint8_t *levelImageBytes;
  LevelAssetRuntimePrefix *levelImage;
  uint32_t loadError;
  uint32_t writeError;
  LevelInitialArmyPlacementRecord20 *placementRecordCursor;
  Bool8 imageLoaded;

  imageLoaded = Package_LoadEntryIntoBuffer(PACKAGE_SCRATCH_BUFFER_BYTES,g_PackageScratchBuffer,
                                            g_LevelEndingMovieSourcePath,&loadError);
  levelImageBytes = g_PackageScratchBuffer;
  levelImage = (LevelAssetRuntimePrefix *)levelImageBytes;
  if (!imageLoaded) {
    *outError = loadError;
    return false;
  }
  /* the placement table is the last part of the image: the file is cut there and regrown per record */
  placementTableOffset = levelImage->header.resourceTables.runtimePrefixByteSizeAndInitialArmyPlacementOffset;
  activeFactionIndex = (saveWorldView->worldRuntime).activeFactionRuntimeIndex;
  levelImage->header.common.allocationSizeBytes = placementTableOffset;
  levelImage->header.initialArmyPlacementRecordCount = 0;
  /* the loader adds 7 to this value for faction 7's class/mode, yet the editor stores the index of the
     faction it plays here */
  levelImage->playerSlots[6].aiClassOrMode = activeFactionIndex;
  placementRecordCursor = (LevelInitialArmyPlacementRecord20 *)(levelImageBytes + placementTableOffset);
  for (ownerListNode = (saveWorldView->worldRuntime).ownerListHead;
      ownerListNode != NULL; ownerListNode = ownerListNode->nextNode) {
    if (ownerListNode->ownerClassId != WORLD_OWNER_RUNTIME_MODEL) {
      continue;
    }
    levelImage->header.initialArmyPlacementRecordCount++;
    levelImage->header.common.allocationSizeBytes += sizeof(LevelInitialArmyPlacementRecord20);
    armyRuntime = ((ModelRuntimeSlot *)ownerListNode->runtimePayload)->ownerArmyRuntimeOrSavedOffset.armyRuntime;
    /* worldYQ12 receives the node's world X and worldXQ12 its world Y, the reverse of the placement record's field
       names (which follow the parameters the loader passes them to, ArmyRuntime_CreateInstanceFromAsset) */
    placementRecordCursor->worldYQ12 = ownerListNode->worldXQ12;
    placementRecordCursor->factionIndex = armyRuntime->factionIndex;
    placementRecordCursor->armyAssetId = armyRuntime->armyAssetId;
    placementRecordCursor->worldXQ12 = ownerListNode->worldYQ12;
    placementRecordCursor->orientationAngle = ownerListNode->modelLocalRotationAngle2;
    memset(placementRecordCursor->zeroPadding,0,sizeof(placementRecordCursor->zeroPadding));
    placementRecordCursor++;
  }
  levelImage->worldSettings.packedFieldRegionOriginYHigh16XLow16 =
       saveWorldView->lightAzimuthAngle & 0xffffU |
       saveWorldView->lightElevationAngle << 16;
  levelImage->worldSettings.packedFieldRegionHeightHigh16WidthLow16 =
       (saveWorldView->worldRuntime).fieldRegion.auxiliaryAzimuthAngle & 0xffff |
       (saveWorldView->worldRuntime).fieldRegion.auxiliaryElevationAngle << 16;
  /* camera bookmarks 1..7 become the start cameras of player slots 0..6 */
  levelImage->playerSlots[0].startCameraXQ12 = g_LevelCameraBookmark1PositionXQ12;
  levelImage->playerSlots[0].startCameraYQ12 = g_LevelCameraBookmark1PositionYQ12;
  levelImage->playerSlots[0].startCameraZQ12 = g_LevelCameraBookmark1PositionZQ12;
  levelImage->playerSlots[0].startCameraMagnitudeQ12 = g_LevelCameraBookmark1PositionMagnitudeQ12;
  levelImage->playerSlots[0].packedHeadingLow16PitchHigh16 = g_LevelCameraBookmark1PackedHeadingLow16PitchHigh16;
  levelImage->playerSlots[1].startCameraXQ12 = g_LevelCameraBookmark2PositionXQ12;
  levelImage->playerSlots[1].startCameraYQ12 = g_LevelCameraBookmark2PositionYQ12;
  levelImage->playerSlots[1].startCameraZQ12 = g_LevelCameraBookmark2PositionZQ12;
  levelImage->playerSlots[1].startCameraMagnitudeQ12 = g_LevelCameraBookmark2PositionMagnitudeQ12;
  levelImage->playerSlots[1].packedHeadingLow16PitchHigh16 = g_LevelCameraBookmark2PackedHeadingLow16PitchHigh16;
  levelImage->playerSlots[2].startCameraXQ12 = g_LevelCameraBookmark3PositionXQ12;
  levelImage->playerSlots[2].startCameraYQ12 = g_LevelCameraBookmark3PositionYQ12;
  levelImage->playerSlots[2].startCameraZQ12 = g_LevelCameraBookmark3PositionZQ12;
  levelImage->playerSlots[2].startCameraMagnitudeQ12 = g_LevelCameraBookmark3PositionMagnitudeQ12;
  levelImage->playerSlots[2].packedHeadingLow16PitchHigh16 = g_LevelCameraBookmark3PackedHeadingLow16PitchHigh16;
  levelImage->playerSlots[3].startCameraXQ12 = g_LevelCameraBookmark4PositionXQ12;
  levelImage->playerSlots[3].startCameraYQ12 = g_LevelCameraBookmark4PositionYQ12;
  levelImage->playerSlots[3].startCameraZQ12 = g_LevelCameraBookmark4PositionZQ12;
  levelImage->playerSlots[3].startCameraMagnitudeQ12 = g_LevelCameraBookmark4PositionMagnitudeQ12;
  levelImage->playerSlots[3].packedHeadingLow16PitchHigh16 = g_LevelCameraBookmark4PackedHeadingLow16PitchHigh16;
  levelImage->playerSlots[4].startCameraXQ12 = g_LevelCameraBookmark5PositionXQ12;
  levelImage->playerSlots[4].startCameraYQ12 = g_LevelCameraBookmark5PositionYQ12;
  levelImage->playerSlots[4].startCameraZQ12 = g_LevelCameraBookmark5PositionZQ12;
  levelImage->playerSlots[4].startCameraMagnitudeQ12 = g_LevelCameraBookmark5PositionMagnitudeQ12;
  levelImage->playerSlots[4].packedHeadingLow16PitchHigh16 = g_LevelCameraBookmark5PackedHeadingLow16PitchHigh16;
  levelImage->playerSlots[5].startCameraXQ12 = g_LevelCameraBookmark6PositionXQ12;
  levelImage->playerSlots[5].startCameraYQ12 = g_LevelCameraBookmark6PositionYQ12;
  levelImage->playerSlots[5].startCameraZQ12 = g_LevelCameraBookmark6PositionZQ12;
  levelImage->playerSlots[5].startCameraMagnitudeQ12 = g_LevelCameraBookmark6PositionMagnitudeQ12;
  levelImage->playerSlots[5].packedHeadingLow16PitchHigh16 = g_LevelCameraBookmark6PackedHeadingLow16PitchHigh16;
  levelImage->playerSlots[6].startCameraXQ12 = g_LevelCameraBookmark7PositionXQ12;
  levelImage->playerSlots[6].startCameraYQ12 = g_LevelCameraBookmark7PositionYQ12;
  levelImage->playerSlots[6].startCameraZQ12 = g_LevelCameraBookmark7PositionZQ12;
  levelImage->playerSlots[6].startCameraMagnitudeQ12 = g_LevelCameraBookmark7PositionMagnitudeQ12;
  levelImage->playerSlots[6].packedHeadingLow16PitchHigh16 = g_LevelCameraBookmark7PackedHeadingLow16PitchHigh16;
  writeError = FileSystem_WriteBufferToPath
                 (levelImage->header.common.allocationSizeBytes,levelImageBytes,g_LevelEndingMovieSourcePath);
  if (writeError != 0) {
    *outError = writeError;
    return false;
  }
  return true;
}


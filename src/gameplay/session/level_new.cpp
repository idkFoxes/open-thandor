/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/src/gameplay/session/level_new.cpp
 * Reverse engineering by idkFoxes 2026
 */

#include <thandor/gameplay/session/level_new.h>
#include <thandor/thandor.h>

/* Module data. */

uint32_t g_InGameLevelTitleTextResourceIndex = 0;

uint32_t g_InGameLevelCampaignAssociationIndex = 0;

DirectSoundVoiceSet *g_InGameLevelEffectVoiceSets[4] = {nullptr, nullptr, nullptr, nullptr};

DirectSoundVoiceSet *g_InGameLevelMusicVoiceSets[4] = {nullptr, nullptr, nullptr, nullptr};

InGameLevelRuntimeGlobalBlock20 g_InGameLevelRuntimeGlobalBlock = {.playerSlotByteOffsets = {0, 32, 64, 96, 128, 160, 192}};

uint32_t g_MoviePlaybackBaseFrameGroup = 0;

uint32_t g_MoviePlaybackScheduleCounter = 0;

uint32_t g_MoviePlaybackScheduleSpan = 0;

EngineFileHandle g_SoundPackageHandle = 0;

/* L"sound\\level00.sam" */
static uint16_t g_SoundLevel00SamPathUtf16[18] =
    {'s', 'o', 'u', 'n', 'd', '\\', 'l', 'e', 'v', 'e', 'l', '0', '0', '.', 's', 'a', 'm', 0};

/* L"sound\\music00.sam" */
static uint16_t g_SessionMusic00SamPathUtf16[18] =
    {'s', 'o', 'u', 'n', 'd', '\\', 'm', 'u', 's', 'i', 'c', '0', '0', '.', 's', 'a', 'm', 0};

Ptr32<void> *g_InGameLoadedResourcePointers = nullptr;

InGameLoadedResourcePointerCount g_InGameLoadedResourcePointerCount = 0;

uint16_t g_InGameLevelSoundLeafOrCombinedPathScratchUtf16[256] = {0};

uint16_t g_InGameLevelSoundParentDirectoryScratchUtf16[256] = {0};

/* Implementation ownership: gameplay/session/level_new. */

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
Bool8 NewLevel_Fail(uint32_t *outError,uint32_t error)

{
  *outError = error;
  return false;
}

/* Allocates g_InGameLevelRuntimeGlobalBlock.conditionStorage and copies the level prefix (header
   resourceTables.runtimePrefixByteSizeAndInitialArmyPlacementOffset bytes) into it dword by dword. */
Bool8 NewLevel_CopyRuntimePrefix(LevelAssetRuntimePrefix *levelImage,uint32_t *outError)

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
Bool8 NewLevel_LoadTechnology(LevelAssetRuntimePrefix *levelImage,uint32_t *outError)

{
  uint16_t *technologyPath;
  TechnologyAsset *loadedTechnologyAsset;
  uint32_t loadErrorCode;

  technologyPath = (uint16_t *)((uint8_t *)levelImage + (levelImage->header).pathOffsets.technologyPathOffset);
  WidePath_SetExtensionCode(ASSET_MAGIC_TEC,technologyPath);
  loadedTechnologyAsset = (TechnologyAsset *)Package_LoadEntry(technologyPath,&loadErrorCode);
  if (loadedTechnologyAsset == nullptr) {
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

Bool8 NewLevel_PrepareEffectAsset(void *asset,uint32_t *outError)

{
  return EffectAsset_PrepareEntries((EffectAssetHeader *)asset,outError);
}

Bool8 NewLevel_PrepareShotAsset(void *asset,uint32_t *outError)

{
  *outError = ShotAsset_PrepareEntries((ShotAssetHeader *)asset);
  return *outError == 0;
}

Bool8 NewLevel_PrepareModelAsset(void *asset,uint32_t *outError)

{
  return ModelAsset_PrepareRecords((ModelAssetHeader *)asset,outError);
}

Bool8 NewLevel_PrepareArmyAsset(void *asset,uint32_t *outError)

{
  *outError = ArmyAsset_PrepareRecords((ArmyAssetHeader *)asset);
  return *outError == 0;
}

/* Loads one LEV file list (EFF, SHT, MDL or ARM; 0x40-byte path records at pathTableOffset): sets each path's
   extension, loads the file, appends it at *loadedResourceCursor to g_InGameLoadedResourcePointers and prepares
   it. One loading-movie step per file. */
Bool8 NewLevel_LoadAssetList
          (LevelAssetRuntimePrefix *levelImage,LevelAssetRelativeByteOffset pathTableOffset,
           LevelAssetRecordCount remainingRecordCount,PackedFileExtensionCode32 extensionCode,
           NewLevelPrepareAssetFn prepareAsset,Ptr32<void> **loadedResourceCursor,uint32_t *outError)

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
    if (loadedAsset == nullptr) {
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
          Thandor_U32ToPointer<FieldGridAsset>((levelImage->header).pathOffsets.levelPathOffset),&stepError)) { /* 5f-format: LevelAsset +0x0B0 levelPathOffset (FieldGridAsset *) */
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
            (Thandor_U32ToPointer<FieldGridAsset>((levelImage->header).pathOffsets.levelPathOffset),worldRuntime); /* 5f-format: LevelAsset +0x0B0 levelPathOffset (FieldGridAsset *) */
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
      if (createdArmy == nullptr) {
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
  uintptr_t *soundSlotCursor;
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
        sampleLoaded = Resource_Load(listedSoundPath,&loadedSample,nullptr,&loadErrorCode);
      }
      else {
        WidePath_CombineDirectoryAndLeaf
                  (g_InGameLevelSoundLeafOrCombinedPathScratchUtf16,listedSoundPath,
                   g_InGameLevelSoundParentDirectoryScratchUtf16);
        sampleLoaded = Resource_Load(g_InGameLevelSoundLeafOrCombinedPathScratchUtf16,
                                     &loadedSample,nullptr,&loadErrorCode);
      }
      if (!sampleLoaded) {
        g_MemoryApi.free(directoryListing);
        return NewLevel_Fail(outError,loadErrorCode);
      }
      soundSlot = SpatialSoundSlot_CreateFromSampleAsset((SoundSampleAsset *)loadedSample);
      if (soundSlot != nullptr) {
        soundSlotCursor[soundIndex] = (uintptr_t)soundSlot;
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
  if (Resource_Load(pathTemplate,&loadedSampleBuffer,nullptr,nullptr)) {
    if (g_SoundCreateSampleVoiceSet((SoundSampleAsset *)loadedSampleBuffer,&createdVoiceSet) == 0) {
      *outVoiceSet = createdVoiceSet;
    }
    Resource_Release((SoundSampleAsset *)loadedSampleBuffer);
  }
}

/* Resets the in-game effect and music voice state and loads the four level effect and four music samples of
   the tail. */
void NewLevel_LoadLevelSamples(void)

{
  struct LevelWorldSettings *worldSettings;

  g_InGameLevelEffectVoiceSets[0] = nullptr;
  g_InGameLevelEffectVoiceSets[1] = nullptr;
  g_InGameLevelEffectVoiceSets[2] = nullptr;
  g_InGameLevelEffectVoiceSets[3] = nullptr;
  g_InGameActiveEffectVoice = nullptr;
  g_InGameEffectsEnabled = 1;
  g_InGameActiveMusicVoice = nullptr;
  g_InGameMusicNextTrackCountdown = 1;
  g_InGameLevelMusicVoiceSets[0] = nullptr;
  g_InGameLevelMusicVoiceSets[1] = nullptr;
  g_InGameLevelMusicVoiceSets[2] = nullptr;
  g_InGameLevelMusicVoiceSets[3] = nullptr;
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

  class0BArmyDefinition = nullptr;
  class0ENoExtraArmyDefinition = nullptr;
  class0EArmyDefinition = nullptr; /* this and class10ArmyDefinition are uninitialized in the original */
  class10ArmyDefinition = nullptr;
  for (registrySlot = 0; registrySlot < ARMY_ASSET_REGISTRY_SLOT_COUNT; registrySlot++) {
    registryArmyDefinition = g_ArmyAssetRecordRegistry[registrySlot];
    if ((registryArmyDefinition == nullptr) || ((((ArmyAssetRecord *)registryArmyDefinition)->flags & 1) == 0)) {
      continue;
    }
    rootModelDefinition = ModelDefinitionRegistry_FindById
                       (Thandor_U32ToPointer<ArmyModelTreeNode>(registryArmyDefinition->rootNodeOffsetOrPointer)-> /* 5f-format: ArmyAssetRecord.rootNodeOffsetOrPointer */
                        linkedDefinitionIds[0]);
    if (rootModelDefinition == nullptr) {
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
  if ((class0BArmyDefinition == nullptr) || (class0ENoExtraArmyDefinition == nullptr) ||
      (worldRuntime->ownerListHead == nullptr)) {
    return;
  }
  /* factions 1..activeFactionCount; faction 1 is checked even when the count is 0 */
  factionIndex = 1;
  do {
    factionModelFlags = 0;
    for (ownerListNode = worldRuntime->ownerListHead; ownerListNode != nullptr;
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
           Thandor_PointerToU32(class0BArmyDefinition); /* 5f-format: GameFactionRuntimeRecord.primaryArmyAssetPointersOrIds */
      g_GameFactionRuntimeImage.records[factionIndex].primaryArmyAssetPointersOrIds[1] =
           Thandor_PointerToU32(class0ENoExtraArmyDefinition); /* 5f-format: GameFactionRuntimeRecord.primaryArmyAssetPointersOrIds */
      g_GameFactionRuntimeImage.records[factionIndex].primaryArmyAssetPointersOrIds[2] =
           Thandor_PointerToU32(class0EArmyDefinition); /* 5f-format: GameFactionRuntimeRecord.primaryArmyAssetPointersOrIds */
      g_GameFactionRuntimeImage.records[factionIndex].primaryArmyAssetPointersOrIds[3] =
           Thandor_PointerToU32(class10ArmyDefinition); /* 5f-format: GameFactionRuntimeRecord.primaryArmyAssetPointersOrIds */
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
  Ptr32<void> *loadedResourceCursor;
  uint32_t allocError;
  uint32_t stepError;

  allocError = g_MemoryApi.alloc(INGAME_LOADED_RESOURCE_CAPACITY * sizeof(Ptr32<void>),(void **)&loadedResourceCursor);
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

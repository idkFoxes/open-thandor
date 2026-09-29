/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/src/gameplay/session/level.c
 * Reverse engineering by idkFoxes 2026
 */

#include <thandor/gameplay/session/level.h>
#include <thandor/thandor.h>

/* Implementation ownership: gameplay/session/level. */

/* Address: 0x00531080.
   Prepares the movies of a level before it is loaded: returns the level's loading movie (the path at LEV +0xCC
   with its extension set to "flm") and writes the matching end movie number into "flm\ende0000.flm" from the
   5th and 6th characters of that path ('w' 0xFC -> 2, "ei" -> 3, "la" -> 4, anything else 0). It keeps a copy of
   the level path in g_LevelEndingMovieSourcePath, which the loaders report on errors.
*/
EndingMoviePathResult LevelAsset_PrepareEndingMoviePath(uint16_t *currentLevelPath,LevelAssetHeader *asset)

{
  int movieNameChars4And5;
  int remainingDwordCount;
  int32_t movieNumber;
  uint16_t *sourcePathCursor;
  EndingMoviePathResult successResult;
  EndingMoviePathResult failureResult;
  uint8_t *path;
  
  if (((asset->common).magic == ASSET_MAGIC_LEV) &&
     ((asset->common).converterVersion == PCK_CONVERTER_LEV_00070001)) {
    path = (asset->common).buildMetadata.reserved28_2F +
           ((asset->pathOffsets).endingMovieBasePathOffset - 0x28);
    remainingDwordCount = 0x80; /* 256 UTF-16 code units of the level path */
    /* UTF-16 characters 4 and 5 of the movie path, read as one dword */
    movieNameChars4And5 = *(int *)(path + 8);
    movieNumber = 0;
    if (movieNameChars4And5 == 0xfc0077) {
      movieNumber = 2;
    }
    else if (movieNameChars4And5 == 0x690065) {
      movieNumber = 3;
    }
    else if (movieNameChars4And5 == 0x61006c) {
      movieNumber = 4;
    }
    WidePath_SetExtensionCode(0x6d6c66,(uint16_t *)path); /* "flm" */
    g_WideNumberFormatUtf16
              (WIDE_FORMAT_PAD_WITH_ZERO,0,4,1,movieNumber,
               (uint16_t *)(u_flm_ende0000_flm_0050df06 + 8)); /* the "0000" */
    sourcePathCursor = g_LevelEndingMovieSourcePath;
    for (; remainingDwordCount != 0; remainingDwordCount--) {
      *(uint32_t *)sourcePathCursor = *(uint32_t *)currentLevelPath;
      currentLevelPath = currentLevelPath + 2;
      sourcePathCursor = sourcePathCursor + 2;
    }
    successResult.failed = false;
    successResult.moviePath = (uint16_t *)path;
    return successResult;
  }
  Package_SetLastErrorPath(currentLevelPath);
  failureResult.failed = true;
  failureResult.moviePath = (uint16_t *)FATAL_ERROR_LEVEL_ASSET_INVALID;
  return failureResult;
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
   Both loaders report errors with CF set and a FATAL_ERROR_* code or the code of the failing step; the progress of
   the loading movie is driven through g_MoviePlaybackBaseFrameGroup / ScheduleSpan and
   MoviePlayback_AdvanceScheduledFrameAndTick, one frame group per loading stage (0..6).
*/

/* Address: 0x005311D0.
   Loads a new level (after GameData_ResetDefaults): copies the level prefix, loads the technology file and all
   listed EFF/SHT/MDL/ARM files, initialises terrain, graphics pools, camera bookmarks, start resources and
   factions, spawns the initial armies, loads the level sounds, gives factions that start without a structure a
   default build list and applies the initial faction relations.
*/

LevelDefaultLoadResult InGameLevelRuntime_LoadResourcesAfterDefaultReset
          (LevelAssetRuntimePrefix *levelImage,WorldRuntimeContext *worldRuntime)

{
  LevelPlayerSlotByteOffset32 playerSlotByteOffset;
  InGameLevelConditionStorage *levelConditionStorage;
  void **loadedResourcePointerArray;
  void *resultOrPointer;
  TechnologyAsset *loadedTechnologyAsset;
  uint32_t soundDirectoryRecordSizeBytes;
  void *shrinkResultOrError;
  uint32_t countOrPackedValue;
  WorldWorkspaceElementCount remainingSoundSlotCount;
  PckOutputCapacityBytes outputCapacityBytes;
  uint32_t modelFlagsOrSoundIndex;
  uint32_t sourceBitOrIndex;
  uint32_t targetIndexOrBit;
  LevelAssetRecordCount remainingRecordCount;
  uint16_t *armyTextureBasePath;
  uint16_t *effectTextureBasePath;
  int counterOrClassId;
  uint32_t flagsOrRelationMask;
  ArmyAssetRecordPrefix **armyRegistryCursor;
  uint32_t relationFactionIndex;
  uint32_t currentSourceIndex;
  uint32_t targetBitOrSavedIndex;
  uint32_t *copySourceCursor;
  uint8_t *assetPath;
  uint16_t *assetPathCursor;
  LevelInitialArmyPlacementRecord20 *placementCursor;
  uint32_t *soundSlotCursor;
  WorldOwnerListNode *ownerListNode;
  uint32_t regionOriginOrRelationMask;
  ArenaAllocResult allocResult;
  PackageLoadResult loadEntryResult;
  StatusResult statusResult;
  ArmyRuntimeInitResult armyInitResult;
  ArmyRuntimeCreateResult armyCreateResult;
  ArenaShrinkResult shrinkResult;
  SpatialSoundSlotResult soundSlotResult;
  SampleVoiceSetResult voiceSetResult;
  ModelDefinitionResult modelLookupResult;
  LevelDefaultLoadResult successResult;
  LevelDefaultLoadResult failureResult;
  ArenaLargestAllocResult largestBlockResult;
  PackageFindResult findEntryResult;
  DirectoryEnumerationResult enumerationResult;
  ResourceLoadResult resourceLoadResult;
  WorldRuntimeContext *soundLoopWorldRuntimeCopy;
  ArmyAssetRecordPrefix *class10ArmyDefinition;
  ArmyAssetRecordPrefix *class0EArmyDefinition;
  ArmyAssetRecordPrefix *class0ENoExtraArmyDefinition;
  ArmyAssetRecordPrefix *class0BArmyDefinition;
  uint16_t *soundDirectoryPathCursor;
  ArmyAssetRecordPrefix *registryArmyDefinition;
  ArmyAssetRecordPrefix *nextClass0EArmyDefinition;
  ArmyAssetRecordPrefix *nextClass10ArmyDefinition;
  ArmyAssetRecordPrefix *nextClass0BArmyDefinition;
  WorldRuntimeContext *soundLoopWorldRuntime;
  
  allocResult = g_MemoryApi.alloc(INGAME_LOADED_RESOURCE_CAPACITY * 4);
  loadedResourcePointerArray = (void **)allocResult.payloadOrError;
  resultOrPointer = loadedResourcePointerArray;
  if (!allocResult.failed) {
    g_InGameLoadedResourcePointerCount = 0;
    resultOrPointer = (void *)FATAL_ERROR_LEVEL_ASSET_INVALID;
    g_InGameLoadedResourcePointers = loadedResourcePointerArray;
    Package_SetLastErrorPath(g_LevelEndingMovieSourcePath);
    if (((levelImage->header).common.magic == ASSET_MAGIC_LEV) &&
       ((levelImage->header).common.converterVersion == PCK_CONVERTER_LEV_00070001)) {
      countOrPackedValue = (levelImage->header).resourceTables.runtimePrefixByteSizeAndInitialArmyPlacementOffset
      ;
      allocResult = g_MemoryApi.alloc(countOrPackedValue);
      resultOrPointer = (void *)allocResult.payloadOrError;
      if (!allocResult.failed) {
        /* copy the level prefix (LEV +0xDC bytes) dword by dword into the condition storage */
        copySourceCursor = (uint32_t *)levelImage;
        g_InGameLevelRuntimeGlobalBlock.conditionStorage = resultOrPointer;
        for (countOrPackedValue = countOrPackedValue >> 2; countOrPackedValue != 0; countOrPackedValue--) {
          *(uint32_t *)resultOrPointer = *copySourceCursor;
          copySourceCursor++;
          resultOrPointer = (uint32_t *)resultOrPointer + 1;
        }
        g_InGameLevelTitleTextResourceIndex = (levelImage->header).titleTextResourceIndex;
        g_InGameLevelCampaignAssociationIndex = (levelImage->header).campaignAssociationIndex;
        assetPath = (levelImage->header).common.buildMetadata.reserved28_2F +
                  ((levelImage->header).pathOffsets.technologyPathOffset - 0x28);
        WidePath_SetExtensionCode(0x636574,(uint16_t *)assetPath); /* "tec" */
        loadEntryResult = Package_LoadEntry((uint16_t *)assetPath);
        loadedTechnologyAsset = loadEntryResult.bufferOrError;
        resultOrPointer = loadedTechnologyAsset;
        if (((!loadEntryResult.failed) &&
            (resultOrPointer = (void *)FATAL_ERROR_TECHNOLOGY_ASSET_INVALID,
            g_TechnologyAsset = loadedTechnologyAsset,
            (loadedTechnologyAsset->header).common.magic == ASSET_MAGIC_TEC)) &&
           ((loadedTechnologyAsset->header).common.converterVersion == PCK_CONVERTER_TEC_00020000))
        {
          /* player slots (LEV +0x200): camera bookmarks 1-7, start resources and class/mode of factions 1-7 */
          g_LevelCameraBookmark1PositionXQ12 = levelImage->playerSlots[0].startCameraXQ12;
          g_LevelCameraBookmark1PositionYQ12 = levelImage->playerSlots[0].startCameraYQ12;
          g_LevelCameraBookmark1PositionZQ12 = levelImage->playerSlots[0].startCameraZQ12;
          g_LevelCameraBookmark1PositionMagnitudeQ12 =
               levelImage->playerSlots[0].startCameraMagnitudeQ12;
          g_LevelCameraBookmark1PackedHeadingLow16PitchHigh16 =
               levelImage->playerSlots[0].packedHeadingLow16PitchHigh16;
          g_LevelCameraBookmark2PositionXQ12 = levelImage->playerSlots[1].startCameraXQ12;
          g_LevelCameraBookmark2PositionYQ12 = levelImage->playerSlots[1].startCameraYQ12;
          g_LevelCameraBookmark2PositionZQ12 = levelImage->playerSlots[1].startCameraZQ12;
          g_LevelCameraBookmark2PositionMagnitudeQ12 =
               levelImage->playerSlots[1].startCameraMagnitudeQ12;
          g_LevelCameraBookmark2PackedHeadingLow16PitchHigh16 =
               levelImage->playerSlots[1].packedHeadingLow16PitchHigh16;
          g_LevelCameraBookmark3PositionXQ12 = levelImage->playerSlots[2].startCameraXQ12;
          g_LevelCameraBookmark3PositionYQ12 = levelImage->playerSlots[2].startCameraYQ12;
          g_LevelCameraBookmark3PositionZQ12 = levelImage->playerSlots[2].startCameraZQ12;
          g_LevelCameraBookmark3PositionMagnitudeQ12 =
               levelImage->playerSlots[2].startCameraMagnitudeQ12;
          g_LevelCameraBookmark3PackedHeadingLow16PitchHigh16 =
               levelImage->playerSlots[2].packedHeadingLow16PitchHigh16;
          g_LevelCameraBookmark4PositionXQ12 = levelImage->playerSlots[3].startCameraXQ12;
          g_LevelCameraBookmark4PositionYQ12 = levelImage->playerSlots[3].startCameraYQ12;
          g_LevelCameraBookmark4PositionZQ12 = levelImage->playerSlots[3].startCameraZQ12;
          g_LevelCameraBookmark4PositionMagnitudeQ12 =
               levelImage->playerSlots[3].startCameraMagnitudeQ12;
          g_LevelCameraBookmark4PackedHeadingLow16PitchHigh16 =
               levelImage->playerSlots[3].packedHeadingLow16PitchHigh16;
          g_LevelCameraBookmark5PositionXQ12 = levelImage->playerSlots[4].startCameraXQ12;
          g_LevelCameraBookmark5PositionYQ12 = levelImage->playerSlots[4].startCameraYQ12;
          g_LevelCameraBookmark5PositionZQ12 = levelImage->playerSlots[4].startCameraZQ12;
          g_LevelCameraBookmark5PositionMagnitudeQ12 =
               levelImage->playerSlots[4].startCameraMagnitudeQ12;
          g_LevelCameraBookmark5PackedHeadingLow16PitchHigh16 =
               levelImage->playerSlots[4].packedHeadingLow16PitchHigh16;
          g_LevelCameraBookmark6PositionXQ12 = levelImage->playerSlots[5].startCameraXQ12;
          g_LevelCameraBookmark6PositionYQ12 = levelImage->playerSlots[5].startCameraYQ12;
          g_LevelCameraBookmark6PositionZQ12 = levelImage->playerSlots[5].startCameraZQ12;
          g_LevelCameraBookmark6PositionMagnitudeQ12 =
               levelImage->playerSlots[5].startCameraMagnitudeQ12;
          g_LevelCameraBookmark6PackedHeadingLow16PitchHigh16 =
               levelImage->playerSlots[5].packedHeadingLow16PitchHigh16;
          g_LevelCameraBookmark7PositionXQ12 = levelImage->playerSlots[6].startCameraXQ12;
          g_LevelCameraBookmark7PositionYQ12 = levelImage->playerSlots[6].startCameraYQ12;
          g_LevelCameraBookmark7PositionZQ12 = levelImage->playerSlots[6].startCameraZQ12;
          g_LevelCameraBookmark7PositionMagnitudeQ12 =
               levelImage->playerSlots[6].startCameraMagnitudeQ12;
          g_LevelCameraBookmark7PackedHeadingLow16PitchHigh16 =
               levelImage->playerSlots[6].packedHeadingLow16PitchHigh16;
          g_GameFactionRuntimeImage.records[1].xeniteCurrentQ4 =
               levelImage->playerSlots[0].startXeniteQ4;
          g_GameFactionRuntimeImage.records[1].tritiumCurrentQ4 =
               levelImage->playerSlots[0].startTritiumQ4;
          g_GameFactionRuntimeImage.records[2].xeniteCurrentQ4 =
               levelImage->playerSlots[1].startXeniteQ4;
          g_GameFactionRuntimeImage.records[2].tritiumCurrentQ4 =
               levelImage->playerSlots[1].startTritiumQ4;
          g_GameFactionRuntimeImage.records[3].xeniteCurrentQ4 =
               levelImage->playerSlots[2].startXeniteQ4;
          g_GameFactionRuntimeImage.records[3].tritiumCurrentQ4 =
               levelImage->playerSlots[2].startTritiumQ4;
          g_GameFactionRuntimeImage.records[4].xeniteCurrentQ4 =
               levelImage->playerSlots[3].startXeniteQ4;
          g_GameFactionRuntimeImage.records[4].tritiumCurrentQ4 =
               levelImage->playerSlots[3].startTritiumQ4;
          g_GameFactionRuntimeImage.records[5].xeniteCurrentQ4 =
               levelImage->playerSlots[4].startXeniteQ4;
          g_GameFactionRuntimeImage.records[5].tritiumCurrentQ4 =
               levelImage->playerSlots[4].startTritiumQ4;
          g_GameFactionRuntimeImage.records[6].xeniteCurrentQ4 =
               levelImage->playerSlots[5].startXeniteQ4;
          g_GameFactionRuntimeImage.records[6].tritiumCurrentQ4 =
               levelImage->playerSlots[5].startTritiumQ4;
          g_GameFactionRuntimeImage.records[7].xeniteCurrentQ4 =
               levelImage->playerSlots[6].startXeniteQ4;
          g_GameFactionRuntimeImage.records[7].tritiumCurrentQ4 =
               levelImage->playerSlots[6].startTritiumQ4;
          g_GameFactionRuntimeImage.records[1].factionClassOrMode =
               levelImage->playerSlots[0].aiClassOrMode + 1;
          g_GameFactionRuntimeImage.records[2].factionClassOrMode =
               levelImage->playerSlots[1].aiClassOrMode + 2;
          g_GameFactionRuntimeImage.records[3].factionClassOrMode =
               levelImage->playerSlots[2].aiClassOrMode + 3;
          g_GameFactionRuntimeImage.tail.activeFactionCount =
               (levelImage->worldSettings).activeFactionCount;
          g_GameFactionRuntimeImage.records[4].factionClassOrMode =
               levelImage->playerSlots[3].aiClassOrMode + 4;
          g_GameFactionRuntimeImage.records[5].factionClassOrMode =
               levelImage->playerSlots[4].aiClassOrMode + 5;
          g_GameFactionRuntimeImage.records[6].factionClassOrMode =
               levelImage->playerSlots[5].aiClassOrMode + 6;
          g_GameFactionRuntimeImage.records[7].factionClassOrMode =
               levelImage->playerSlots[6].aiClassOrMode + 7;
          /* loading stage 0: one movie step per EFF, SHT, MDL and ARM file */
          g_MoviePlaybackScheduleSpan =
               (levelImage->header).resourceTables.effectAssetPathCount +
               (levelImage->header).resourceTables.shotAssetPathCount +
               (levelImage->header).resourceTables.modelAssetPathCount +
               (levelImage->header).resourceTables.armyAssetPathCount;
          g_MoviePlaybackBaseFrameGroup = 0;
          g_MoviePlaybackScheduleCounter = 0;
          assetPathCursor = (uint16_t *)((levelImage->header).common.buildMetadata.reserved28_2F
                            + ((levelImage->header).resourceTables.effectAssetPathTableOffset - 0x28
                              ));
          for (remainingRecordCount = (levelImage->header).resourceTables.effectAssetPathCount;
                 remainingRecordCount != 0;
              remainingRecordCount--) {
            WidePath_SetExtensionCode(0x666665,assetPathCursor); /* "eff" */
            resultOrPointer = (void *)FATAL_ERROR_LEVEL_TOO_MANY_RESOURCES;
            if (INGAME_LOADED_RESOURCE_CAPACITY - 1 < g_InGameLoadedResourcePointerCount) goto load_failed;
            loadEntryResult = Package_LoadEntry(assetPathCursor);
            resultOrPointer = loadEntryResult.bufferOrError;
            if (loadEntryResult.failed) goto load_failed;
            *loadedResourcePointerArray = resultOrPointer;
            g_InGameLoadedResourcePointerCount++;
            loadedResourcePointerArray++;
            statusResult = EffectAsset_PrepareEntries(resultOrPointer);
            resultOrPointer = (void *)statusResult.valueOrError;
            if (statusResult.failed) goto load_failed;
            MoviePlayback_AdvanceScheduledFrameAndTick();
            assetPathCursor = assetPathCursor + 0x20; /* next 0x40-byte path record */
          }
          assetPathCursor = (uint16_t *)((levelImage->header).common.buildMetadata.reserved28_2F
                            + ((levelImage->header).resourceTables.shotAssetPathTableOffset - 0x28))
          ;
          for (remainingRecordCount = (levelImage->header).resourceTables.shotAssetPathCount;
                 remainingRecordCount != 0;
              remainingRecordCount--) {
            WidePath_SetExtensionCode(0x746873,assetPathCursor); /* "sht" */
            resultOrPointer = (void *)FATAL_ERROR_LEVEL_TOO_MANY_RESOURCES;
            if (INGAME_LOADED_RESOURCE_CAPACITY - 1 < g_InGameLoadedResourcePointerCount) goto load_failed;
            loadEntryResult = Package_LoadEntry(assetPathCursor);
            resultOrPointer = loadEntryResult.bufferOrError;
            if (loadEntryResult.failed) goto load_failed;
            *loadedResourcePointerArray = resultOrPointer;
            g_InGameLoadedResourcePointerCount++;
            loadedResourcePointerArray++;
            statusResult = ShotAsset_PrepareEntries(resultOrPointer);
            resultOrPointer = (void *)statusResult.valueOrError;
            if (statusResult.failed) goto load_failed;
            MoviePlayback_AdvanceScheduledFrameAndTick();
            assetPathCursor = assetPathCursor + 0x20; /* next 0x40-byte path record */
          }
          statusResult = EffectDefinitions_ResolveCrossReferences();
          resultOrPointer = (void *)statusResult.valueOrError;
          if (!statusResult.failed) {
            assetPathCursor = (uint16_t *)((levelImage->header).common.buildMetadata.
                               reserved28_2F +
                              ((levelImage->header).resourceTables.modelAssetPathTableOffset - 0x28)
                              );
            for (remainingRecordCount = (levelImage->header).resourceTables.modelAssetPathCount;
                 remainingRecordCount != 0;
                remainingRecordCount--) {
              WidePath_SetExtensionCode(0x6c646d,assetPathCursor); /* "mdl" */
              resultOrPointer = (void *)FATAL_ERROR_LEVEL_TOO_MANY_RESOURCES;
              if (INGAME_LOADED_RESOURCE_CAPACITY - 1 < g_InGameLoadedResourcePointerCount) goto load_failed;
              loadEntryResult = Package_LoadEntry(assetPathCursor);
              resultOrPointer = loadEntryResult.bufferOrError;
              if (loadEntryResult.failed) goto load_failed;
              *loadedResourcePointerArray = resultOrPointer;
              g_InGameLoadedResourcePointerCount++;
              loadedResourcePointerArray++;
              statusResult = ModelAsset_PrepareRecords(resultOrPointer);
              resultOrPointer = (void *)statusResult.valueOrError;
              if (statusResult.failed) goto load_failed;
              MoviePlayback_AdvanceScheduledFrameAndTick();
              assetPathCursor = assetPathCursor + 0x20; /* next 0x40-byte path record */
            }
            assetPathCursor = (uint16_t *)((levelImage->header).common.buildMetadata.
                               reserved28_2F +
                              ((levelImage->header).resourceTables.armyAssetPathTableOffset - 0x28))
            ;
            for (remainingRecordCount = (levelImage->header).resourceTables.armyAssetPathCount;
                 remainingRecordCount != 0;
                remainingRecordCount--) {
              WidePath_SetExtensionCode(0x6d7261,assetPathCursor); /* "arm" */
              resultOrPointer = (void *)FATAL_ERROR_LEVEL_TOO_MANY_RESOURCES;
              if (INGAME_LOADED_RESOURCE_CAPACITY - 1 < g_InGameLoadedResourcePointerCount) goto load_failed;
              loadEntryResult = Package_LoadEntry(assetPathCursor);
              resultOrPointer = loadEntryResult.bufferOrError;
              if (loadEntryResult.failed) goto load_failed;
              *loadedResourcePointerArray = resultOrPointer;
              g_InGameLoadedResourcePointerCount++;
              loadedResourcePointerArray++;
              statusResult = ArmyAsset_PrepareRecords(resultOrPointer);
              resultOrPointer = (void *)statusResult.valueOrError;
              if (statusResult.failed) goto load_failed;
              MoviePlayback_AdvanceScheduledFrameAndTick();
              assetPathCursor = assetPathCursor + 0x20; /* next 0x40-byte path record */
            }
            resultOrPointer = (void *)FATAL_ERROR_LEVEL_TOO_MANY_RESOURCES;
            if (g_InGameLoadedResourcePointerCount < INGAME_LOADED_RESOURCE_CAPACITY) {
              g_MoviePlaybackBaseFrameGroup = 1;
              g_MoviePlaybackScheduleCounter = 0;
              g_MoviePlaybackScheduleSpan = 0x10000;
              /* stage 1: terrain textures (surface, ground) and the field grid */
              statusResult = TerrainVisualResources_LoadPrimary
                                 ((uint16_t *)((levelImage->header).common.buildMetadata.
                                           reserved28_2F +
                                          ((levelImage->header).pathOffsets.
                                           surfaceTextureBasePathOffset - 0x28)),
                                  (uint16_t *)((levelImage->header).common.buildMetadata.
                                           reserved28_2F +
                                          ((levelImage->header).pathOffsets.
                                           groundTextureBasePathOffset - 0x28)),
                                  (FieldGridAsset *)(levelImage->header).pathOffsets.levelPathOffset
                                 );
              resultOrPointer = (void *)statusResult.valueOrError;
              if (!statusResult.failed) {
                statusResult = ShotDefinitions_ValidateTerrainMaterialReferences();
                resultOrPointer = (void *)statusResult.valueOrError;
                if (!statusResult.failed) {
                  g_MoviePlaybackBaseFrameGroup = 2;
                  g_MoviePlaybackScheduleCounter = 0;
                  g_MoviePlaybackScheduleSpan = 0x10000;
                  statusResult = ModelRuntimePool_Init();
                  resultOrPointer = (void *)statusResult.valueOrError;
                  if (!statusResult.failed) {
                    /* The decompiler lost these two locals; the original reads them from the level header
                       (+0xC0 army and +0xC8 effect texture base paths). */
                    armyTextureBasePath =
                         (uint16_t *)((levelImage->header).common.buildMetadata.reserved28_2F +
                                      ((levelImage->header).pathOffsets.armyTextureBasePathOffset - 0x28));
                    effectTextureBasePath =
                         (uint16_t *)((levelImage->header).common.buildMetadata.reserved28_2F +
                                      ((levelImage->header).pathOffsets.effectTextureBasePathOffset - 0x28));
                    armyInitResult = ArmyRuntime_InitializePoolAndGraphics(worldRuntime,armyTextureBasePath);
                    resultOrPointer = (void *)armyInitResult.errorOrValue;
                    if (!armyInitResult.failed) {
                      g_MoviePlaybackBaseFrameGroup = 3;
                      g_MoviePlaybackScheduleCounter = 0;
                      g_MoviePlaybackScheduleSpan = 4;
                      statusResult = ShotRuntime_InitGraphicsResources
                                         ((uint16_t *)((levelImage->header).common.buildMetadata.
                                                   reserved28_2F +
                                                  ((levelImage->header).pathOffsets.
                                                   shotTextureBasePathOffset - 0x28)));
                      resultOrPointer = (void *)statusResult.valueOrError;
                      if (!statusResult.failed) {
                        g_MoviePlaybackBaseFrameGroup = 4;
                        g_MoviePlaybackScheduleCounter = 0;
                        g_MoviePlaybackScheduleSpan = 4;
                        statusResult = EffectRuntime_InitGraphicsResources(effectTextureBasePath);
                        resultOrPointer = (void *)statusResult.valueOrError;
                        if (!statusResult.failed) {
                          g_MoviePlaybackBaseFrameGroup = 5;
                          g_MoviePlaybackScheduleCounter = 0;
                          g_MoviePlaybackScheduleSpan = 6;
                          WorldRuntime_SetTerrainLightingConfiguration
                                    ((levelImage->worldSettings).terrainLightingColor13CArgb,
                                     (levelImage->worldSettings).terrainLightingColor138Argb,
                                     (levelImage->worldSettings).terrainLightingColor134Argb,
                                     (levelImage->worldSettings).terrainLightingColor130Argb,
                                     (levelImage->worldSettings).terrainRampColor12CArgb,
                                     (levelImage->worldSettings).terrainLightingColor128Argb,
                                     (levelImage->worldSettings).terrainRampColor124Argb,
                                     (levelImage->worldSettings).terrainBaseColorArgb,worldRuntime)
                          ;
                          /* start camera from the local faction's player slot, then the lit field region
                             from the tail */
                          counterOrClassId = worldRuntime->activeFactionRuntimeIndex;
                          MoviePlayback_AdvanceScheduledFrameAndTick();
                          playerSlotByteOffset =
                               g_InGameLevelRuntimeGlobalBlock.playerSlotByteOffsets[counterOrClassId - 1];
                          WorldRuntime_AttachFieldGridAsset
                                    ((FieldGridAsset *)
                                     (levelImage->header).pathOffsets.levelPathOffset,worldRuntime);
                          MoviePlayback_AdvanceScheduledFrameAndTick();
                          countOrPackedValue = *(uint32_t *)((uint8_t *)&levelImage->playerSlots[0].
                                                  packedHeadingLow16PitchHigh16 + playerSlotByteOffset);
                          WorldRuntime_SetCameraPositionKeepingTarget
                                    (*(Q12 *)((uint8_t *)&levelImage->playerSlots[0].startCameraZQ12 +
                                             playerSlotByteOffset),
                                     *(Q12 *)((uint8_t *)&levelImage->playerSlots[0].startCameraYQ12 +
                                             playerSlotByteOffset),
                                     *(Q12 *)((uint8_t *)&levelImage->playerSlots[0].startCameraXQ12 +
                                             playerSlotByteOffset),worldRuntime);
                          WorldRuntime_SetCameraAnglesAndMagnitudeClamped
                                    (2,(int)countOrPackedValue >> 0x10,countOrPackedValue & 0xffff,
                                     *(UQ12 *)((uint8_t *)&levelImage->playerSlots[0].
                                                     startCameraMagnitudeQ12 + playerSlotByteOffset),worldRuntime);
                          countOrPackedValue = (levelImage->worldSettings).packedFieldRegionOriginYHigh16XLow16;
                          WorldRuntime_RecomputeMotionEndpointAgainstFieldSurface(worldRuntime);
                          regionOriginOrRelationMask = countOrPackedValue;
                          WorldRuntime_CommitCameraTargetDistance(worldRuntime);
                          flagsOrRelationMask = (levelImage->worldSettings).
                                   packedFieldRegionHeightHigh16WidthLow16;
                          WorldRuntime_RecomputeFieldRegionNormalsAndLighting
                                    ((int)flagsOrRelationMask >> 0x10,flagsOrRelationMask & 0xffff,
                                     (int)regionOriginOrRelationMask >> 0x10,
                                     countOrPackedValue & 0xffff,worldRuntime);
                          /* initial army placements (0x20-byte records at LEV +[0xDC]) */
                          remainingRecordCount = (levelImage->header).initialArmyPlacementRecordCount;
                          placementCursor = (LevelInitialArmyPlacementRecord20 *)
                                            ((levelImage->header).common.buildMetadata.reserved28_2F +
                                             ((levelImage->header).resourceTables.
                                              runtimePrefixByteSizeAndInitialArmyPlacementOffset - 0x28));
                          MoviePlayback_AdvanceScheduledFrameAndTick();
                          for (; remainingRecordCount != 0; remainingRecordCount--) {
                            if (g_GameFactionRuntimeImage.tail.factionLifecycleStates[placementCursor->factionIndex] ==
                                FACTION_RUNTIME_LIFECYCLE_ACTIVE) {
                              armyCreateResult = ArmyRuntime_CreateInstanceFromAsset
                                                 (ARMY_CREATE_COUNT_FOR_ACTIVE_FACTION | ARMY_CREATE_UNLOCK_TECHNOLOGY,
                                                  placementCursor->orientationAngle,placementCursor->worldXQ12,
                                                  placementCursor->worldYQ12,placementCursor->factionIndex,
                                                  placementCursor->armyAssetId,worldRuntime);
                              resultOrPointer = (void *)armyCreateResult.armyRuntimeOrError;
                              if (armyCreateResult.failed) goto load_failed;
                            }
                            placementCursor++;
                          }
                          MoviePlayback_AdvanceScheduledFrameAndTick();
                          WorldRuntime_ForEachOwnerListNode
                                    (worldRuntime,
                                     ArmyRuntimeNode_AccumulateTerrainOcclusionAndOccupancyCallback,
                                     worldRuntime);
                          WorldRuntime_ForEachOwnerListNode
                                    (worldRuntime,
                                     ArmyRuntimeNode_RebuildTerrainOccupancyAndVisualStateCallback,
                                     worldRuntime);
                          FieldGrid_ClassifyCellFlagsToRuntimeByte
                                    (worldRuntime->activeFactionRuntimeIndex,worldRuntime->fieldGrid
                                    );
                          MoviePlayback_AdvanceScheduledFrameAndTick();
                          /* spatial sound slots: cleared, then filled from the level's sound directory, listed
                             from the sound package or, failing that, from disk */
                          soundSlotCursor = worldRuntime->dwordArray;
                          for (remainingSoundSlotCount = worldRuntime->dwordArrayCount; remainingSoundSlotCount != 0;
                              remainingSoundSlotCount--) {
                            *soundSlotCursor = 0;
                            soundSlotCursor++;
                          }
                          assetPath = (levelImage->header).common.buildMetadata.
                                    reserved28_2F +
                                    ((levelImage->header).pathOffsets.soundBasePathOffset - 0x28);
                          WidePath_SetExtensionCode(0x6d6173,(uint16_t *)assetPath); /* "sam" */
                          WidePath_SplitParentAndLeaf
                                    ((uint16_t *)&g_InGameLevelSoundLeafOrCombinedPathScratchUtf16,
                                     (uint16_t *)&g_InGameLevelSoundParentDirectoryScratchUtf16,
                                     (uint16_t *)assetPath);
                          largestBlockResult = g_MemoryApi.allocLargestFreeBlock();
                          outputCapacityBytes = largestBlockResult.blockSizeOrSentinel;
                          resultOrPointer = (void *)largestBlockResult.allocationOrError;
                          if (!largestBlockResult.failed) {
                            findEntryResult = Package_FindEntry(outputCapacityBytes,resultOrPointer,
                                                       (uint16_t *)assetPath,g_SoundPackageHandle);
                            countOrPackedValue = findEntryResult.matchCount;
                            soundDirectoryRecordSizeBytes = findEntryResult.recordSizeOrError;
                            if (findEntryResult.failed) {
                              enumerationResult = g_FileSystemEnumerateDirectoryOrVolumeEntries
                                                 (FILESYSTEM_ENUMERATE_FILES,0xffffffff,
                                                  outputCapacityBytes,resultOrPointer,assetPath);
                              countOrPackedValue = enumerationResult.entryCount;
                              soundDirectoryRecordSizeBytes = enumerationResult.recordSizeBytes;
                            }
                            shrinkResultOrError = (void *)soundDirectoryRecordSizeBytes;
                            if (1 /* Ghidra stack-probe artifact: &stack0x.. < 0xfffffffc always holds */) {
                              g_MoviePlaybackBaseFrameGroup = 6;
                              g_MoviePlaybackScheduleCounter = 0;
                              g_MoviePlaybackScheduleSpan = countOrPackedValue;
                              shrinkResult = g_MemoryApi.shrinkInPlace
                                                 (soundDirectoryRecordSizeBytes * countOrPackedValue,
                                                  resultOrPointer);
                              shrinkResultOrError = (void *)shrinkResult.scratchOrError;
                              if (!shrinkResult.failed) {
                                soundLoopWorldRuntime = worldRuntime;
                                soundDirectoryPathCursor = resultOrPointer;
                                levelConditionStorage = g_InGameLevelRuntimeGlobalBlock.conditionStorage;
                                if (worldRuntime->dwordArrayCount < countOrPackedValue) {
                                  countOrPackedValue = worldRuntime->dwordArrayCount;
                                }
                                do {
                                  g_InGameLevelRuntimeGlobalBlock.conditionStorage = levelConditionStorage;
                                  if (countOrPackedValue == 0) {
                                    g_MemoryApi.free(resultOrPointer);
                                    g_InGameLevelEffectVoiceSet0 = (DirectSoundVoiceSet *)0x0;
                                    g_InGameLevelEffectVoiceSet1 = (DirectSoundVoiceSet *)0x0;
                                    g_InGameLevelEffectVoiceSet2 = (DirectSoundVoiceSet *)0x0;
                                    g_InGameLevelEffectVoiceSet3 = (DirectSoundVoiceSet *)0x0;
                                    g_InGameActiveEffectVoice = 0;
                                    g_InGameEffectsEnabled = 1;
                                    g_InGameActiveMusicVoice = 0;
                                    g_InGameMusicEnabled = 1;
                                    g_InGameLevelMusicVoiceSet0 = (DirectSoundVoiceSet *)0x0;
                                    g_InGameLevelMusicVoiceSet1 = (DirectSoundVoiceSet *)0x0;
                                    g_InGameLevelMusicVoiceSet2 = (DirectSoundVoiceSet *)0x0;
                                    g_InGameLevelMusicVoiceSet3 = (DirectSoundVoiceSet *)0x0;
                                    /* the four level effect and four music samples of the tail */
                                    if ((levelConditionStorage->levelImage).worldSettings.effectSampleNumbers[0]
                                        != 0) {
                                      g_WideNumberFormatUtf16
                                                (WIDE_FORMAT_PAD_WITH_ZERO,0,2,1,
                                                 (levelConditionStorage->levelImage).worldSettings.
                                                 effectSampleNumbers[0],
                                                 (uint16_t *)(u_sound_level00_sam_0050df6c + 0xb));
                                      resourceLoadResult = Resource_Load((uint16_t *)u_sound_level00_sam_0050df6c);
                                      if (!resourceLoadResult.failed) {
                                        voiceSetResult = g_SoundCreateSampleVoiceSet
                                                           ((SoundSampleAsset *)resourceLoadResult.bufferOrError);
                                        if (!voiceSetResult.failed) {
                                          g_InGameLevelEffectVoiceSet0 = voiceSetResult.voiceSet;
                                        }
                                        Resource_Release((SoundSampleAsset *)resourceLoadResult.bufferOrError);
                                      }
                                    }
                                    if ((levelConditionStorage->levelImage).worldSettings.effectSampleNumbers[1]
                                        != 0) {
                                      g_WideNumberFormatUtf16
                                                (WIDE_FORMAT_PAD_WITH_ZERO,0,2,1,
                                                 (levelConditionStorage->levelImage).worldSettings.
                                                 effectSampleNumbers[1],
                                                 (uint16_t *)(u_sound_level00_sam_0050df6c + 0xb));
                                      resourceLoadResult = Resource_Load((uint16_t *)u_sound_level00_sam_0050df6c);
                                      if (!resourceLoadResult.failed) {
                                        voiceSetResult = g_SoundCreateSampleVoiceSet
                                                           ((SoundSampleAsset *)resourceLoadResult.bufferOrError);
                                        if (!voiceSetResult.failed) {
                                          g_InGameLevelEffectVoiceSet1 = voiceSetResult.voiceSet;
                                        }
                                        Resource_Release((SoundSampleAsset *)resourceLoadResult.bufferOrError);
                                      }
                                    }
                                    if ((levelConditionStorage->levelImage).worldSettings.effectSampleNumbers[2]
                                        != 0) {
                                      g_WideNumberFormatUtf16
                                                (WIDE_FORMAT_PAD_WITH_ZERO,0,2,1,
                                                 (levelConditionStorage->levelImage).worldSettings.
                                                 effectSampleNumbers[2],
                                                 (uint16_t *)(u_sound_level00_sam_0050df6c + 0xb));
                                      resourceLoadResult = Resource_Load((uint16_t *)u_sound_level00_sam_0050df6c);
                                      if (!resourceLoadResult.failed) {
                                        voiceSetResult = g_SoundCreateSampleVoiceSet
                                                           ((SoundSampleAsset *)resourceLoadResult.bufferOrError);
                                        if (!voiceSetResult.failed) {
                                          g_InGameLevelEffectVoiceSet2 = voiceSetResult.voiceSet;
                                        }
                                        Resource_Release((SoundSampleAsset *)resourceLoadResult.bufferOrError);
                                      }
                                    }
                                    if ((levelConditionStorage->levelImage).worldSettings.effectSampleNumbers[3]
                                        != 0) {
                                      g_WideNumberFormatUtf16
                                                (WIDE_FORMAT_PAD_WITH_ZERO,0,2,1,
                                                 (levelConditionStorage->levelImage).worldSettings.
                                                 effectSampleNumbers[3],
                                                 (uint16_t *)(u_sound_level00_sam_0050df6c + 0xb));
                                      resourceLoadResult = Resource_Load((uint16_t *)u_sound_level00_sam_0050df6c);
                                      if (!resourceLoadResult.failed) {
                                        voiceSetResult = g_SoundCreateSampleVoiceSet
                                                           ((SoundSampleAsset *)resourceLoadResult.bufferOrError);
                                        if (!voiceSetResult.failed) {
                                          g_InGameLevelEffectVoiceSet3 = voiceSetResult.voiceSet;
                                        }
                                        Resource_Release((SoundSampleAsset *)resourceLoadResult.bufferOrError);
                                      }
                                    }
                                    if ((levelConditionStorage->levelImage).worldSettings.musicSampleNumbers[0] !=
                                        0) {
                                      g_WideNumberFormatUtf16
                                                (WIDE_FORMAT_PAD_WITH_ZERO,0,2,1,
                                                 (levelConditionStorage->levelImage).worldSettings.
                                                 musicSampleNumbers[0],
                                                 (uint16_t *)(u_sound_music00_sam_0050df90 + 0xb));
                                      resourceLoadResult = Resource_Load((uint16_t *)u_sound_music00_sam_0050df90);
                                      if (!resourceLoadResult.failed) {
                                        voiceSetResult = g_SoundCreateSampleVoiceSet
                                                           ((SoundSampleAsset *)resourceLoadResult.bufferOrError);
                                        if (!voiceSetResult.failed) {
                                          g_InGameLevelMusicVoiceSet0 = voiceSetResult.voiceSet;
                                        }
                                        Resource_Release((SoundSampleAsset *)resourceLoadResult.bufferOrError);
                                      }
                                    }
                                    if ((levelConditionStorage->levelImage).worldSettings.musicSampleNumbers[1] !=
                                        0) {
                                      g_WideNumberFormatUtf16
                                                (WIDE_FORMAT_PAD_WITH_ZERO,0,2,1,
                                                 (levelConditionStorage->levelImage).worldSettings.
                                                 musicSampleNumbers[1],
                                                 (uint16_t *)(u_sound_music00_sam_0050df90 + 0xb));
                                      resourceLoadResult = Resource_Load((uint16_t *)u_sound_music00_sam_0050df90);
                                      if (!resourceLoadResult.failed) {
                                        voiceSetResult = g_SoundCreateSampleVoiceSet
                                                           ((SoundSampleAsset *)resourceLoadResult.bufferOrError);
                                        if (!voiceSetResult.failed) {
                                          g_InGameLevelMusicVoiceSet1 = voiceSetResult.voiceSet;
                                        }
                                        Resource_Release((SoundSampleAsset *)resourceLoadResult.bufferOrError);
                                      }
                                    }
                                    if ((levelConditionStorage->levelImage).worldSettings.musicSampleNumbers[2] !=
                                        0) {
                                      g_WideNumberFormatUtf16
                                                (WIDE_FORMAT_PAD_WITH_ZERO,0,2,1,
                                                 (levelConditionStorage->levelImage).worldSettings.
                                                 musicSampleNumbers[2],
                                                 (uint16_t *)(u_sound_music00_sam_0050df90 + 0xb));
                                      resourceLoadResult = Resource_Load((uint16_t *)u_sound_music00_sam_0050df90);
                                      if (!resourceLoadResult.failed) {
                                        voiceSetResult = g_SoundCreateSampleVoiceSet
                                                           ((SoundSampleAsset *)resourceLoadResult.bufferOrError);
                                        if (!voiceSetResult.failed) {
                                          g_InGameLevelMusicVoiceSet2 = voiceSetResult.voiceSet;
                                        }
                                        Resource_Release((SoundSampleAsset *)resourceLoadResult.bufferOrError);
                                      }
                                    }
                                    if ((levelConditionStorage->levelImage).worldSettings.musicSampleNumbers[3] !=
                                        0) {
                                      g_WideNumberFormatUtf16
                                                (WIDE_FORMAT_PAD_WITH_ZERO,0,2,1,
                                                 (levelConditionStorage->levelImage).worldSettings.
                                                 musicSampleNumbers[3],
                                                 (uint16_t *)(u_sound_music00_sam_0050df90 + 0xb));
                                      resourceLoadResult = Resource_Load((uint16_t *)u_sound_music00_sam_0050df90);
                                      if (!resourceLoadResult.failed) {
                                        voiceSetResult = g_SoundCreateSampleVoiceSet
                                                           ((SoundSampleAsset *)resourceLoadResult.bufferOrError);
                                        if (!voiceSetResult.failed) {
                                          g_InGameLevelMusicVoiceSet3 = voiceSetResult.voiceSet;
                                        }
                                        Resource_Release((SoundSampleAsset *)resourceLoadResult.bufferOrError);
                                      }
                                    }
                                    /* default build list for factions that start with class-18 models but no
                                       structure: the last registered army assets (army flag +0x14 bit 0) whose
                                       model class is 0x0B, 0x0E without / with the model's +0xC0 value, and 0x10 */
                                    armyRegistryCursor = g_ArmyAssetRecordRegistry;
                                    counterOrClassId = ARMY_ASSET_REGISTRY_SLOT_COUNT;
                                    class0BArmyDefinition = NULL;
                                    class0ENoExtraArmyDefinition = NULL;
                                    class0EArmyDefinition = NULL; /* this and class10ArmyDefinition are uninitialized in the original */
                                    class10ArmyDefinition = NULL;
                                    do {
                                      registryArmyDefinition = *armyRegistryCursor;
                                      nextClass10ArmyDefinition = class10ArmyDefinition;
                                      nextClass0EArmyDefinition = class0EArmyDefinition;
                                      nextClass0BArmyDefinition = class0BArmyDefinition;
                                      if ((registryArmyDefinition != NULL) &&
                                         ((((ArmyAssetRecord *)registryArmyDefinition)->flags &
                                          1) != 0)) {
                                        modelLookupResult = ModelDefinitionRegistry_FindByIdWithError
                                                           (((ArmyModelTreeNode *)registryArmyDefinition->
                                                             rootNodeOffsetOrPointer)->linkedDefinitionIds[0]);
                                        modelFlagsOrSoundIndex =
                                             ((ModelDefinition *)modelLookupResult.modelDefinition)->
                                                                 runtimeClassId;
                                        nextClass0BArmyDefinition = registryArmyDefinition;
                                        if (((modelFlagsOrSoundIndex != 0xb) &&
                                            ((nextClass10ArmyDefinition = registryArmyDefinition,
                                             nextClass0BArmyDefinition = class0BArmyDefinition,
                                                  modelFlagsOrSoundIndex != 0x10 &&
                                             (nextClass10ArmyDefinition = class10ArmyDefinition,
                                              modelFlagsOrSoundIndex == 0xe)))) &&
                                           (nextClass0EArmyDefinition = registryArmyDefinition,
                                           ((ModelDefinition *)modelLookupResult.modelDefinition)->
                                           classParameterC0 == 0)) {
                                          nextClass0EArmyDefinition = class0EArmyDefinition;
                                          class0ENoExtraArmyDefinition = registryArmyDefinition;
                                        }
                                      }
                                      class0BArmyDefinition = nextClass0BArmyDefinition;
                                      class0EArmyDefinition = nextClass0EArmyDefinition;
                                      class10ArmyDefinition = nextClass10ArmyDefinition;
                                      armyRegistryCursor++;
                                      counterOrClassId--;
                                    } while (counterOrClassId != 0);
                                    if ((class0BArmyDefinition != NULL) &&
                                       (class0ENoExtraArmyDefinition != NULL)) {
                                      countOrPackedValue = 1;
                                      ownerListNode = worldRuntime->ownerListHead;
                                      flagsOrRelationMask = 0;
                                      if (ownerListNode != NULL) {
                                        do {
                                          do {
                                            if ((ownerListNode->ownerClassId ==
                                                 WORLD_OWNER_RUNTIME_MODEL) &&
                                               (countOrPackedValue ==
                                                ((ModelRuntimeSlot *)ownerListNode->runtimePayload)->
                                                ownerArmyRuntimeOrSavedOffset.armyRuntime->factionIndex)) {
                                              counterOrClassId = ((ModelRuntimeSlot *)ownerListNode->runtimePayload)->
                                                                 definitionOrSavedId.runtimeDefinition->runtimeClassId;
                                              if (counterOrClassId == 0x12) {
                                                flagsOrRelationMask = flagsOrRelationMask | 2;
                                              }
                                              else if (
                                                  g_ArmyRuntimeOrderHandlerMatrix11Columns24Classes.
                                                  classCommand[counterOrClassId] ==
                                                  ArmyRuntime_ClassCommandHandlerGroupA) {
                                                flagsOrRelationMask = flagsOrRelationMask | 1;
                                              }
                                            }
                                            ownerListNode = ownerListNode->nextNode;
                                          } while (ownerListNode != NULL);
                                          if (flagsOrRelationMask == 2) {
                                            g_GameFactionRuntimeImage.records[countOrPackedValue].
                                            primaryArmyAssetPointersOrIds[0] = (uint32_t)class0BArmyDefinition;
                                            g_GameFactionRuntimeImage.records[countOrPackedValue].
                                            primaryArmyAssetPointersOrIds[1] = (uint32_t)class0ENoExtraArmyDefinition;
                                            g_GameFactionRuntimeImage.records[countOrPackedValue].
                                            primaryArmyAssetPointersOrIds[2] = (uint32_t)class0EArmyDefinition;
                                            g_GameFactionRuntimeImage.records[countOrPackedValue].
                                            primaryArmyAssetPointersOrIds[3] = (uint32_t)class10ArmyDefinition;
                                            g_GameFactionRuntimeImage.records[countOrPackedValue].
                                            primaryArmyAssetCount = 4;
                                          }
                                          ownerListNode = worldRuntime->ownerListHead;
                                          countOrPackedValue++;
                                          flagsOrRelationMask = 0;
                                        } while (countOrPackedValue <= g_GameFactionRuntimeImage.tail.
                                                          activeFactionCount);
                                      }
                                    }
                                    /* initial relations: every pair inside one 8-bit faction group of tail +0x40
                                       gets state 4/4, of tail +0x3C state 8/8 */
                                    countOrPackedValue = ((g_InGameLevelRuntimeGlobalBlock.conditionStorage)->
                                            levelImage).worldSettings.relationUiFlags;
                                    flagsOrRelationMask = ((g_InGameLevelRuntimeGlobalBlock.conditionStorage)->
                                             levelImage).worldSettings.
                                             relationState4FactionGroupMasks;
                                    regionOriginOrRelationMask = ((g_InGameLevelRuntimeGlobalBlock.conditionStorage)->
                                             levelImage).worldSettings.
                                             relationState8FactionGroupMasks;
                                    g_GameFactionRuntimeImage.tail.relationUiFlags = countOrPackedValue;
                                    sourceBitOrIndex = flagsOrRelationMask & 0xff;
                                    while (sourceBitOrIndex != 0) {
                                      sourceBitOrIndex = 1;
                                      relationFactionIndex = 0;
                                      do {
                                        currentSourceIndex = relationFactionIndex;
                                        targetBitOrSavedIndex = sourceBitOrIndex;
                                        targetIndexOrBit = relationFactionIndex;
                                        if ((flagsOrRelationMask & sourceBitOrIndex) != 0) {
                                          while (targetIndexOrBit = targetIndexOrBit + 1, countOrPackedValue =
                                                 relationFactionIndex,
                                                 targetIndexOrBit < 8) {
                                            targetBitOrSavedIndex = targetBitOrSavedIndex * 2;
                                            if ((flagsOrRelationMask & targetBitOrSavedIndex) != 0) {
                                              GameFactionRuntime_ApplyPairwiseRelationTransition
                                                        (0x20,0x20,FACTION_RELATION_STATE_FRIENDLY,
                                                         FACTION_RELATION_STATE_FRIENDLY,targetIndexOrBit,
                                                         relationFactionIndex);
                                            }
                                          }
                                        }
                                        relationFactionIndex = currentSourceIndex + 1;
                                        sourceBitOrIndex = sourceBitOrIndex * 2;
                                      } while (relationFactionIndex != 8);
                                      flagsOrRelationMask = flagsOrRelationMask >> 8;
                                      sourceBitOrIndex = flagsOrRelationMask;
                                    }
                                    flagsOrRelationMask = regionOriginOrRelationMask & 0xff;
                                    while (flagsOrRelationMask != 0) {
                                      flagsOrRelationMask = 1;
                                      sourceBitOrIndex = 0;
                                      do {
                                        targetBitOrSavedIndex = sourceBitOrIndex;
                                        targetIndexOrBit = flagsOrRelationMask;
                                        relationFactionIndex = sourceBitOrIndex;
                                        if ((regionOriginOrRelationMask & flagsOrRelationMask) != 0) {
                                          while (relationFactionIndex = relationFactionIndex + 1, countOrPackedValue =
                                                 sourceBitOrIndex,
                                                 relationFactionIndex < 8) {
                                            targetIndexOrBit = targetIndexOrBit * 2;
                                            if ((regionOriginOrRelationMask & targetIndexOrBit) != 0) {
                                              GameFactionRuntime_ApplyPairwiseRelationTransition
                                                        (0x20,0x20,FACTION_RELATION_STATE_ALLIED,
                                                         FACTION_RELATION_STATE_ALLIED,relationFactionIndex,
                                                         sourceBitOrIndex);
                                            }
                                          }
                                        }
                                        sourceBitOrIndex = targetBitOrSavedIndex + 1;
                                        flagsOrRelationMask = flagsOrRelationMask * 2;
                                      } while (sourceBitOrIndex != 8);
                                      regionOriginOrRelationMask = regionOriginOrRelationMask >> 8;
                                      flagsOrRelationMask = regionOriginOrRelationMask;
                                    }
                                    successResult.failed = false;
                                    successResult.errorOrValue = countOrPackedValue;
                                    return successResult;
                                  }
                                  soundSlotCursor = soundLoopWorldRuntime->dwordArray;
                                  soundLoopWorldRuntimeCopy = soundLoopWorldRuntime;
                                  modelFlagsOrSoundIndex = WidePath_ParseTrailingNumberBeforeExtensionRegs
                                                    (soundDirectoryPathCursor);
                                  if (modelFlagsOrSoundIndex < soundLoopWorldRuntime->dwordArrayCount) {
                                    if (!findEntryResult.failed) {
                                      resourceLoadResult = Resource_Load(soundDirectoryPathCursor);
                                      shrinkResultOrError = (void *)resourceLoadResult.bufferOrError;
                                      if (resourceLoadResult.failed) break;
                                    }
                                    else {
                                      WidePath_CombineDirectoryAndLeaf
                                                ((uint16_t *)&
                                                  g_InGameLevelSoundLeafOrCombinedPathScratchUtf16,
                                                 soundDirectoryPathCursor,
                                                 (uint16_t *)&
                                                  g_InGameLevelSoundParentDirectoryScratchUtf16);
                                      resourceLoadResult = Resource_Load((uint16_t *)&
                                                  g_InGameLevelSoundLeafOrCombinedPathScratchUtf16);
                                      shrinkResultOrError = (void *)resourceLoadResult.bufferOrError;
                                      if (resourceLoadResult.failed) break;
                                    }
                                    soundSlotResult = SpatialSoundSlot_CreateFromSampleAsset
                                                       (shrinkResultOrError);
                                    if (!soundSlotResult.failed) {
                                      soundSlotCursor[modelFlagsOrSoundIndex] = (uint32_t)soundSlotResult.soundSlot;
                                    }
                                    Resource_Release(shrinkResultOrError);
                                    MoviePlayback_AdvanceScheduledFrameAndTick();
                                  }
                                  countOrPackedValue--;
                                  soundLoopWorldRuntime = soundLoopWorldRuntimeCopy;
                                  /* advance by one directory record (soundDirectoryRecordSizeBytes) */
                                  soundDirectoryPathCursor =
                                       (uint16_t *)((int)soundDirectoryPathCursor + (int)soundDirectoryRecordSizeBytes);
                                  levelConditionStorage = g_InGameLevelRuntimeGlobalBlock.conditionStorage;
                                } while( true );
                              }
                            }
                            g_MemoryApi.free(resultOrPointer);
                            resultOrPointer = shrinkResultOrError;
                          }
                        }
                      }
                    }
                  }
                }
              }
            }
          }
        }
      }
    }
  }
load_failed:
  failureResult.failed = true;
  failureResult.errorOrValue = (uint32_t)resultOrPointer;
  return failureResult;
}


/* Address: 0x00532020.
   Loads the level of a saved game (after GameData_LoadExternalTables): the same LEV steps as
   InGameLevelRuntime_LoadResourcesAfterDefaultReset (see the layout above it) up to the camera, but instead of
   spawning the initial armies it restores the saved runtime pools from widget.hex, army.hex, modul.hex,
   effect.hex, shot.hex and light.hex and rebases their pointers. The player-slot copies, default build lists
   and initial relations are skipped; the saved faction image already holds them.
*/

LevelLoadResult InGameLevelRuntime_LoadResourcesAfterExternalTables
          (FrontendLoadedLevelAsset *levelImage,WorldRuntimeContext *worldRuntime)

{
  LevelPlayerSlotByteOffset32 playerSlotByteOffset;
  InGameLevelConditionStorage *levelConditionStorage;
  void **loadedResourcePointerArray;
  void *resultOrPointer;
  TechnologyAsset *loadedTechnologyAsset;
  uint32_t soundDirectoryRecordSizeBytes;
  void *shrinkResultOrError;
  SoundSampleAsset *loadedSampleAsset;
  SoundSampleAsset *sampleOrVoiceSet;
  uint32_t countOrPackedValue;
  WorldWorkspaceElementCount remainingSoundSlotCount;
  PckOutputCapacityBytes outputCapacityBytes;
  uint32_t soundSlotIndex;
  LevelAssetRecordCount remainingPathCount;
  uint16_t *armyTextureBasePath;
  uint16_t *effectTextureBasePath;
  uint32_t packedRegionValue;
  int factionIndexOrOriginY;
  FieldGridDimensionCells gridHeight;
  uint32_t *copySourceCursor;
  uint8_t *assetPath;
  uint16_t *pathTableCursor;
  uint32_t *soundSlotCursor;
  ArenaAllocResult allocResult;
  PackageLoadResult loadEntryResult;
  StatusResult statusResult;
  ArmyRuntimeInitResult armyInitResult;
  ArenaShrinkResult shrinkResult;
  SpatialSoundSlotResult soundSlotResult;
  ArenaFreeResult freeResult;
  SampleVoiceSetResult voiceSetResult;
  LevelLoadResult successResult;
  LevelLoadResult failureResult;
  ArenaLargestAllocResult largestBlockResult;
  PackageFindResult findEntryResult;
  DirectoryEnumerationResult enumerationResult;
  ResourceLoadResult resourceLoadResult;
  uint16_t *soundDirectoryPathCursor;
  WorldRuntimeContext *soundLoopWorldRuntime;
  
  allocResult = g_MemoryApi.alloc(INGAME_LOADED_RESOURCE_CAPACITY * 4);
  loadedResourcePointerArray = (void **)allocResult.payloadOrError;
  resultOrPointer = loadedResourcePointerArray;
  if (!allocResult.failed) {
    g_InGameLoadedResourcePointerCount = 0;
    resultOrPointer = (void *)FATAL_ERROR_LEVEL_ASSET_INVALID;
    g_InGameLoadedResourcePointers = loadedResourcePointerArray;
    Package_SetLastErrorPath(g_LevelEndingMovieSourcePath);
    if (((levelImage->header).common.magic == ASSET_MAGIC_LEV) &&
       ((levelImage->header).common.converterVersion == PCK_CONVERTER_LEV_00070001)) {
      countOrPackedValue = (levelImage->header).resourceTables.runtimePrefixByteSizeAndInitialArmyPlacementOffset
      ;
      allocResult = g_MemoryApi.alloc(countOrPackedValue);
      resultOrPointer = (void *)allocResult.payloadOrError;
      if (!allocResult.failed) {
        /* copy the level prefix (LEV +0xDC bytes) dword by dword into the condition storage */
        copySourceCursor = (uint32_t *)levelImage;
        g_InGameLevelRuntimeGlobalBlock.conditionStorage = resultOrPointer;
        for (countOrPackedValue = countOrPackedValue >> 2; countOrPackedValue != 0; countOrPackedValue--) {
          *(uint32_t *)resultOrPointer = *copySourceCursor;
          copySourceCursor++;
          resultOrPointer = (uint32_t *)resultOrPointer + 1;
        }
        g_InGameLevelTitleTextResourceIndex = (levelImage->header).titleTextResourceIndex;
        g_InGameLevelCampaignAssociationIndex = (levelImage->header).campaignAssociationIndex;
        assetPath = (levelImage->header).common.buildMetadata.reserved28_2F +
                  ((levelImage->header).pathState.technologyPathOffset - 0x28);
        WidePath_SetExtensionCode(0x636574,(uint16_t *)assetPath); /* "tec" */
        loadEntryResult = Package_LoadEntry((uint16_t *)assetPath);
        loadedTechnologyAsset = loadEntryResult.bufferOrError;
        resultOrPointer = loadedTechnologyAsset;
        if (((!loadEntryResult.failed) &&
            (resultOrPointer = (void *)FATAL_ERROR_TECHNOLOGY_ASSET_INVALID,
            g_TechnologyAsset = loadedTechnologyAsset,
            (loadedTechnologyAsset->header).common.magic == ASSET_MAGIC_TEC)) &&
           ((loadedTechnologyAsset->header).common.converterVersion == PCK_CONVERTER_TEC_00020000))
        {
          /* loading stage 0: one movie step per EFF, SHT, MDL and ARM file */
          g_MoviePlaybackScheduleSpan =
               (levelImage->header).resourceTables.effectAssetPathCount +
               (levelImage->header).resourceTables.shotAssetPathCount +
               (levelImage->header).resourceTables.modelAssetPathCount +
               (levelImage->header).resourceTables.armyAssetPathCount;
          g_MoviePlaybackBaseFrameGroup = 0;
          g_MoviePlaybackScheduleCounter = 0;
          pathTableCursor = (uint16_t *)((levelImage->header).common.buildMetadata.reserved28_2F
                            + ((levelImage->header).resourceTables.effectAssetPathTableOffset - 0x28
                              ));
          for (remainingPathCount = (levelImage->header).resourceTables.effectAssetPathCount; remainingPathCount != 0;
              remainingPathCount--) {
            WidePath_SetExtensionCode(0x666665,pathTableCursor); /* "eff" */
            resultOrPointer = (void *)FATAL_ERROR_LEVEL_TOO_MANY_RESOURCES;
            if (INGAME_LOADED_RESOURCE_CAPACITY - 1 < g_InGameLoadedResourcePointerCount) goto load_failed;
            loadEntryResult = Package_LoadEntry(pathTableCursor);
            resultOrPointer = loadEntryResult.bufferOrError;
            if (loadEntryResult.failed) goto load_failed;
            *loadedResourcePointerArray = resultOrPointer;
            g_InGameLoadedResourcePointerCount++;
            loadedResourcePointerArray++;
            statusResult = EffectAsset_PrepareEntries(resultOrPointer);
            resultOrPointer = (void *)statusResult.valueOrError;
            if (statusResult.failed) goto load_failed;
            MoviePlayback_AdvanceScheduledFrameAndTick();
            pathTableCursor = pathTableCursor + 0x20; /* next 0x40-byte path record */
          }
          pathTableCursor = (uint16_t *)((levelImage->header).common.buildMetadata.reserved28_2F
                            + ((levelImage->header).resourceTables.shotAssetPathTableOffset - 0x28))
          ;
          for (remainingPathCount = (levelImage->header).resourceTables.shotAssetPathCount; remainingPathCount != 0;
              remainingPathCount--) {
            WidePath_SetExtensionCode(0x746873,pathTableCursor); /* "sht" */
            resultOrPointer = (void *)FATAL_ERROR_LEVEL_TOO_MANY_RESOURCES;
            if (INGAME_LOADED_RESOURCE_CAPACITY - 1 < g_InGameLoadedResourcePointerCount) goto load_failed;
            loadEntryResult = Package_LoadEntry(pathTableCursor);
            resultOrPointer = loadEntryResult.bufferOrError;
            if (loadEntryResult.failed) goto load_failed;
            *loadedResourcePointerArray = resultOrPointer;
            g_InGameLoadedResourcePointerCount++;
            loadedResourcePointerArray++;
            statusResult = ShotAsset_PrepareEntries(resultOrPointer);
            resultOrPointer = (void *)statusResult.valueOrError;
            if (statusResult.failed) goto load_failed;
            MoviePlayback_AdvanceScheduledFrameAndTick();
            pathTableCursor = pathTableCursor + 0x20; /* next 0x40-byte path record */
          }
          statusResult = EffectDefinitions_ResolveCrossReferences();
          resultOrPointer = (void *)statusResult.valueOrError;
          if (!statusResult.failed) {
            pathTableCursor = (uint16_t *)((levelImage->header).common.buildMetadata.
                               reserved28_2F +
                              ((levelImage->header).resourceTables.modelAssetPathTableOffset - 0x28)
                              );
            for (remainingPathCount = (levelImage->header).resourceTables.modelAssetPathCount; remainingPathCount != 0;
                remainingPathCount--) {
              WidePath_SetExtensionCode(0x6c646d,pathTableCursor); /* "mdl" */
              resultOrPointer = (void *)FATAL_ERROR_LEVEL_TOO_MANY_RESOURCES;
              if (INGAME_LOADED_RESOURCE_CAPACITY - 1 < g_InGameLoadedResourcePointerCount) goto load_failed;
              loadEntryResult = Package_LoadEntry(pathTableCursor);
              resultOrPointer = loadEntryResult.bufferOrError;
              if (loadEntryResult.failed) goto load_failed;
              *loadedResourcePointerArray = resultOrPointer;
              g_InGameLoadedResourcePointerCount++;
              loadedResourcePointerArray++;
              statusResult = ModelAsset_PrepareRecords(resultOrPointer);
              resultOrPointer = (void *)statusResult.valueOrError;
              if (statusResult.failed) goto load_failed;
              MoviePlayback_AdvanceScheduledFrameAndTick();
              pathTableCursor = pathTableCursor + 0x20; /* next 0x40-byte path record */
            }
            pathTableCursor = (uint16_t *)((levelImage->header).common.buildMetadata.
                               reserved28_2F +
                              ((levelImage->header).resourceTables.armyAssetPathTableOffset - 0x28))
            ;
            for (remainingPathCount = (levelImage->header).resourceTables.armyAssetPathCount; remainingPathCount != 0;
                remainingPathCount--) {
              WidePath_SetExtensionCode(0x6d7261,pathTableCursor); /* "arm" */
              resultOrPointer = (void *)FATAL_ERROR_LEVEL_TOO_MANY_RESOURCES;
              if (INGAME_LOADED_RESOURCE_CAPACITY - 1 < g_InGameLoadedResourcePointerCount) goto load_failed;
              loadEntryResult = Package_LoadEntry(pathTableCursor);
              resultOrPointer = loadEntryResult.bufferOrError;
              if (loadEntryResult.failed) goto load_failed;
              *loadedResourcePointerArray = resultOrPointer;
              g_InGameLoadedResourcePointerCount++;
              loadedResourcePointerArray++;
              statusResult = ArmyAsset_PrepareRecords(resultOrPointer);
              resultOrPointer = (void *)statusResult.valueOrError;
              if (statusResult.failed) goto load_failed;
              MoviePlayback_AdvanceScheduledFrameAndTick();
              pathTableCursor = pathTableCursor + 0x20; /* next 0x40-byte path record */
            }
            resultOrPointer = (void *)FATAL_ERROR_LEVEL_TOO_MANY_RESOURCES;
            if (g_InGameLoadedResourcePointerCount < INGAME_LOADED_RESOURCE_CAPACITY) {
              g_MoviePlaybackBaseFrameGroup = 1;
              g_MoviePlaybackScheduleCounter = 0;
              g_MoviePlaybackScheduleSpan = 0x10000;
              /* stage 1: terrain textures (surface, ground) and the field grid */
              statusResult = TerrainVisualResources_LoadAndClearCellOverlayFlags
                                 ((uint16_t *)((levelImage->header).common.buildMetadata.
                                           reserved28_2F +
                                          ((levelImage->header).pathState.
                                           surfaceTextureBasePathOffset - 0x28)),
                                  (uint16_t *)((levelImage->header).common.buildMetadata.
                                           reserved28_2F +
                                          ((levelImage->header).pathState.
                                           groundTextureBasePathOffset - 0x28)),
                                  (FieldGridAsset *)
                                  (levelImage->header).pathState.levelPathOffsetOrLoadedFieldGrid);
              resultOrPointer = (void *)statusResult.valueOrError;
              if (!statusResult.failed) {
                statusResult = ShotDefinitions_ValidateTerrainMaterialReferences();
                resultOrPointer = (void *)statusResult.valueOrError;
                if (!statusResult.failed) {
                  g_MoviePlaybackBaseFrameGroup = 2;
                  g_MoviePlaybackScheduleCounter = 0;
                  g_MoviePlaybackScheduleSpan = 0x10000;
                  statusResult = ModelRuntimePool_Init();
                  resultOrPointer = (void *)statusResult.valueOrError;
                  if (!statusResult.failed) {
                    /* The decompiler lost these two locals; the original reads them from the level header
                       (+0xC0 army and +0xC8 effect texture base paths). */
                    armyTextureBasePath =
                         (uint16_t *)((levelImage->header).common.buildMetadata.reserved28_2F +
                                      ((levelImage->header).pathState.armyTextureBasePathOffset - 0x28));
                    effectTextureBasePath =
                         (uint16_t *)((levelImage->header).common.buildMetadata.reserved28_2F +
                                      ((levelImage->header).pathState.effectTextureBasePathOffset - 0x28));
                    armyInitResult = ArmyRuntime_InitializePoolAndGraphics(worldRuntime,armyTextureBasePath);
                    resultOrPointer = (void *)armyInitResult.errorOrValue;
                    if (!armyInitResult.failed) {
                      g_MoviePlaybackBaseFrameGroup = 3;
                      g_MoviePlaybackScheduleCounter = 0;
                      g_MoviePlaybackScheduleSpan = 4;
                      statusResult = ShotRuntime_InitGraphicsResources
                                         ((uint16_t *)((levelImage->header).common.buildMetadata.
                                                   reserved28_2F +
                                                  ((levelImage->header).pathState.
                                                   shotTextureBasePathOffset - 0x28)));
                      resultOrPointer = (void *)statusResult.valueOrError;
                      if (!statusResult.failed) {
                        g_MoviePlaybackBaseFrameGroup = 4;
                        g_MoviePlaybackScheduleCounter = 0;
                        g_MoviePlaybackScheduleSpan = 4;
                        statusResult = EffectRuntime_InitGraphicsResources(effectTextureBasePath);
                        resultOrPointer = (void *)statusResult.valueOrError;
                        if (!statusResult.failed) {
                          g_MoviePlaybackBaseFrameGroup = 5;
                          g_MoviePlaybackScheduleCounter = 0;
                          g_MoviePlaybackScheduleSpan = 6;
                          GameFactionRuntime_RebaseLoadedArmyReferences();
                          WorldRuntime_SetTerrainLightingConfiguration
                                    ((levelImage->worldSettings).terrainLightingColor13CArgb,
                                     (levelImage->worldSettings).terrainLightingColor138Argb,
                                     (levelImage->worldSettings).terrainLightingColor134Argb,
                                     (levelImage->worldSettings).terrainLightingColor130Argb,
                                     (levelImage->worldSettings).terrainRampColor12CArgb,
                                     (levelImage->worldSettings).terrainLightingColor128Argb,
                                     (levelImage->worldSettings).terrainRampColor124Argb,
                                     (levelImage->worldSettings).terrainBaseColorArgb,worldRuntime)
                          ;
                          /* start camera from the local faction's player slot, then the lit field region
                             from the tail */
                          factionIndexOrOriginY = worldRuntime->activeFactionRuntimeIndex;
                          MoviePlayback_AdvanceScheduledFrameAndTick();
                          playerSlotByteOffset =
                               g_InGameLevelRuntimeGlobalBlock.playerSlotByteOffsets[factionIndexOrOriginY - 1];
                          WorldRuntime_AttachFieldGridAsset
                                    ((FieldGridAsset *)
                                     (levelImage->header).pathState.levelPathOffsetOrLoadedFieldGrid
                                     ,worldRuntime);
                          MoviePlayback_AdvanceScheduledFrameAndTick();
                          countOrPackedValue = *(uint32_t *)((uint8_t *)&levelImage->playerSlots[0].
                                                  packedHeadingLow16PitchHigh16 + playerSlotByteOffset);
                          WorldRuntime_SetCameraPositionKeepingTarget
                                    (*(Q12 *)((uint8_t *)&levelImage->playerSlots[0].startCameraZQ12 +
                                             playerSlotByteOffset),
                                     *(Q12 *)((uint8_t *)&levelImage->playerSlots[0].startCameraYQ12 +
                                             playerSlotByteOffset),
                                     *(Q12 *)((uint8_t *)&levelImage->playerSlots[0].startCameraXQ12 +
                                             playerSlotByteOffset),worldRuntime);
                          WorldRuntime_SetCameraAnglesAndMagnitudeClamped
                                    (2,(int)countOrPackedValue >> 0x10,countOrPackedValue & 0xffff,
                                     *(UQ12 *)((uint8_t *)&levelImage->playerSlots[0].
                                                     startCameraMagnitudeQ12 + playerSlotByteOffset),worldRuntime);
                          countOrPackedValue = (levelImage->worldSettings).packedFieldRegionOriginYHigh16XLow16;
                          WorldRuntime_RecomputeMotionEndpointAgainstFieldSurface(worldRuntime);
                          packedRegionValue = countOrPackedValue;
                          WorldRuntime_CommitCameraTargetDistance(worldRuntime);
                          countOrPackedValue = countOrPackedValue & 0xffff;
                          factionIndexOrOriginY = (int)packedRegionValue >> 0x10;
                          packedRegionValue = (levelImage->worldSettings).
                                  packedFieldRegionHeightHigh16WidthLow16;
                          soundLoopWorldRuntime = worldRuntime;
                          MoviePlayback_AdvanceScheduledFrameAndTick();
                          WorldRuntime_RecomputeFieldRegionNormalsAndLighting
                                    ((int)packedRegionValue >> 0x10,packedRegionValue & 0xffff,factionIndexOrOriginY,
                                     countOrPackedValue,soundLoopWorldRuntime);
                          /* the saved runtime pools, loaded over the freshly initialised ones and rebased */
                          statusResult = Package_LoadEntryIntoBuffer
                                             (worldRuntime->objectCount * 0x100,
                                              (uint8_t *)worldRuntime->objectArray,
                                              (uint16_t *)u_widget_hex_0050e02a);
                          resultOrPointer = (void *)statusResult.valueOrError;
                          if (!statusResult.failed) {
                            statusResult = Package_LoadEntryIntoBuffer
                                               (0x48000,(uint8_t *)g_ArmyRuntimeSlots,
                                                (uint16_t *)u_army_hex_0050dfb4);
                            resultOrPointer = (void *)statusResult.valueOrError;
                            if (!statusResult.failed) {
                              statusResult = Package_LoadEntryIntoBuffer
                                                 (0x400000,(uint8_t *)g_ModelRuntimeSlots,
                                                  (uint16_t *)u_modul_hex_0050dfee);
                              resultOrPointer = (void *)statusResult.valueOrError;
                              if (!statusResult.failed) {
                                statusResult = Package_LoadEntryIntoBuffer
                                                   (0x40000,(uint8_t *)g_EffectRuntimeSlots,
                                                    (uint16_t *)u_effect_hex_0050dfc6);
                                resultOrPointer = (void *)statusResult.valueOrError;
                                if (!statusResult.failed) {
                                  statusResult = Package_LoadEntryIntoBuffer
                                                     (0x40000,(uint8_t *)g_ShotRuntimeSlots,
                                                      (uint16_t *)u_shot_hex_0050dfdc);
                                  resultOrPointer = (void *)statusResult.valueOrError;
                                  if (!statusResult.failed) {
                                    statusResult = Package_LoadEntryIntoBuffer
                                                       (0x4000,(uint8_t *)
                                                  g_GraphicsShadingRuntimeRecords,
                                                  (uint16_t *)u_light_hex_0050e016);
                                    resultOrPointer = (void *)statusResult.valueOrError;
                                    if (!statusResult.failed) {
                                      ArmyRuntimePool_RebaseAfterLoad();
                                      ModelRuntimePool_RebaseAfterLoad();
                                      ShotRuntime_RebaseSlotsAfterLoad();
                                      EffectRuntime_RebaseSlotsAfterLoad();
                                      ResourceRegistrationRuntime_RebaseLoadedRecords
                                                ((ResourceRegistrationRuntimeImage *)worldRuntime);
                                      RuntimeHexSegment_ToggleLightImageFlag();
                                      MoviePlayback_AdvanceScheduledFrameAndTick();
                                      /* spatial sound slots: cleared, then filled from the level's sound
                                         directory, listed from the sound package or, failing that, from disk */
                                      soundSlotCursor = worldRuntime->dwordArray;
                                      for (remainingSoundSlotCount =
                                           worldRuntime->dwordArrayCount; remainingSoundSlotCount != 0;
                                          remainingSoundSlotCount--) {
                                        *soundSlotCursor = 0;
                                        soundSlotCursor++;
                                      }
                                      assetPath = (levelImage->header).common.buildMetadata.
                                                reserved28_2F +
                                                ((levelImage->header).pathState.soundBasePathOffset
                                                - 0x28);
                                      WidePath_SetExtensionCode(0x6d6173,(uint16_t *)assetPath); /* "sam" */
                                      MoviePlayback_AdvanceScheduledFrameAndTick();
                                      WidePath_SplitParentAndLeaf
                                                ((uint16_t *)&
                                                  g_InGameLevelSoundLeafOrCombinedPathScratchUtf16,
                                                 (uint16_t *)&
                                                  g_InGameLevelSoundParentDirectoryScratchUtf16,
                                                 (uint16_t *)assetPath);
                                      largestBlockResult = g_MemoryApi.allocLargestFreeBlock();
                                      outputCapacityBytes = largestBlockResult.blockSizeOrSentinel;
                                      resultOrPointer = (void *)largestBlockResult.allocationOrError;
                                      if (!largestBlockResult.failed) {
                                        findEntryResult = Package_FindEntry(outputCapacityBytes,
                                                                   resultOrPointer,(uint16_t *)assetPath,
                                                                   g_SoundPackageHandle);
                                        countOrPackedValue = findEntryResult.matchCount;
                                        soundDirectoryRecordSizeBytes = findEntryResult.recordSizeOrError;
                                        if (findEntryResult.failed) {
                                          enumerationResult = (*g_FileSystemEnumerateDirectoryOrVolumeEntries
                                                   )(FILESYSTEM_ENUMERATE_FILES,0xffffffff,
                                                     outputCapacityBytes,resultOrPointer,assetPath);
                                          countOrPackedValue = enumerationResult.entryCount;
                                          soundDirectoryRecordSizeBytes = enumerationResult.recordSizeBytes;
                                        }
                                        shrinkResultOrError = (void *)soundDirectoryRecordSizeBytes;
                                        if (1 /* Ghidra stack-probe artifact: &stack0x.. < 0xfffffffc always holds */) {
                                          g_MoviePlaybackBaseFrameGroup = 6;
                                          g_MoviePlaybackScheduleCounter = 0;
                                          g_MoviePlaybackScheduleSpan = countOrPackedValue;
                                          shrinkResult = g_MemoryApi.shrinkInPlace
                                                             (soundDirectoryRecordSizeBytes * countOrPackedValue,
                                                              resultOrPointer);
                                          shrinkResultOrError = (void *)shrinkResult.scratchOrError;
                                          if (!shrinkResult.failed) {
                                            soundDirectoryPathCursor = resultOrPointer;
                                            levelConditionStorage = g_InGameLevelRuntimeGlobalBlock.
                                                     conditionStorage;
                                            if (worldRuntime->dwordArrayCount < countOrPackedValue) {
                                              countOrPackedValue = worldRuntime->dwordArrayCount;
                                            }
                                            do {
                                              g_InGameLevelRuntimeGlobalBlock.conditionStorage =
                                                   levelConditionStorage;
                                              if (countOrPackedValue == 0) {
                                                freeResult = g_MemoryApi.free(resultOrPointer);
                                                g_InGameLevelEffectVoiceSet0 = (void *)0x0;
                                                g_InGameLevelEffectVoiceSet1 = (void *)0x0;
                                                g_InGameLevelEffectVoiceSet2 = (void *)0x0;
                                                g_InGameLevelEffectVoiceSet3 = (void *)0x0;
                                                g_InGameActiveEffectVoice = 0;
                                                g_InGameEffectsEnabled = 1;
                                                g_InGameActiveMusicVoice = 0;
                                                g_InGameMusicEnabled = 1;
                                                g_InGameLevelMusicVoiceSet0 = (void *)0x0;
                                                g_InGameLevelMusicVoiceSet1 = (void *)0x0;
                                                g_InGameLevelMusicVoiceSet2 = (void *)0x0;
                                                g_InGameLevelMusicVoiceSet3 = (void *)0x0;
                                                sampleOrVoiceSet = (SoundSampleAsset *)freeResult.valueOrError;
                                                /* the four level effect and four music samples of the tail */
                                                if ((levelConditionStorage->levelImage).worldSettings.
                                                    effectSampleNumbers[0] != 0) {
                                                  g_WideNumberFormatUtf16
                                                            (WIDE_FORMAT_PAD_WITH_ZERO,0,2,1,
                                                             (levelConditionStorage->levelImage).worldSettings.
                                                             effectSampleNumbers[0],
                                                             (uint16_t *)(u_sound_level00_sam_0050df6c +
                                                                     0xb));
                                                  resourceLoadResult = Resource_Load((uint16_t *)
                                                  u_sound_level00_sam_0050df6c);
                                                  loadedSampleAsset =
                                                       (SoundSampleAsset *)resourceLoadResult.bufferOrError;
                                                  sampleOrVoiceSet = loadedSampleAsset;
                                                  if (!resourceLoadResult.failed) {
                                                    voiceSetResult = g_SoundCreateSampleVoiceSet(loadedSampleAsset);
                                                    sampleOrVoiceSet = (SoundSampleAsset *)voiceSetResult.voiceSet;
                                                    if (!voiceSetResult.failed) {
                                                      g_InGameLevelEffectVoiceSet0 = sampleOrVoiceSet;
                                                    }
                                                    Resource_Release(loadedSampleAsset);
                                                  }
                                                }
                                                if ((levelConditionStorage->levelImage).worldSettings.
                                                    effectSampleNumbers[1] != 0) {
                                                  g_WideNumberFormatUtf16
                                                            (WIDE_FORMAT_PAD_WITH_ZERO,0,2,1,
                                                             (levelConditionStorage->levelImage).worldSettings.
                                                             effectSampleNumbers[1],
                                                             (uint16_t *)(u_sound_level00_sam_0050df6c +
                                                                     0xb));
                                                  resourceLoadResult = Resource_Load((uint16_t *)
                                                  u_sound_level00_sam_0050df6c);
                                                  loadedSampleAsset =
                                                       (SoundSampleAsset *)resourceLoadResult.bufferOrError;
                                                  sampleOrVoiceSet = loadedSampleAsset;
                                                  if (!resourceLoadResult.failed) {
                                                    voiceSetResult = g_SoundCreateSampleVoiceSet(loadedSampleAsset);
                                                    sampleOrVoiceSet = (SoundSampleAsset *)voiceSetResult.voiceSet;
                                                    if (!voiceSetResult.failed) {
                                                      g_InGameLevelEffectVoiceSet1 = sampleOrVoiceSet;
                                                    }
                                                    Resource_Release(loadedSampleAsset);
                                                  }
                                                }
                                                if ((levelConditionStorage->levelImage).worldSettings.
                                                    effectSampleNumbers[2] != 0) {
                                                  g_WideNumberFormatUtf16
                                                            (WIDE_FORMAT_PAD_WITH_ZERO,0,2,1,
                                                             (levelConditionStorage->levelImage).worldSettings.
                                                             effectSampleNumbers[2],
                                                             (uint16_t *)(u_sound_level00_sam_0050df6c +
                                                                     0xb));
                                                  resourceLoadResult = Resource_Load((uint16_t *)
                                                  u_sound_level00_sam_0050df6c);
                                                  loadedSampleAsset =
                                                       (SoundSampleAsset *)resourceLoadResult.bufferOrError;
                                                  sampleOrVoiceSet = loadedSampleAsset;
                                                  if (!resourceLoadResult.failed) {
                                                    voiceSetResult = g_SoundCreateSampleVoiceSet(loadedSampleAsset);
                                                    sampleOrVoiceSet = (SoundSampleAsset *)voiceSetResult.voiceSet;
                                                    if (!voiceSetResult.failed) {
                                                      g_InGameLevelEffectVoiceSet2 = sampleOrVoiceSet;
                                                    }
                                                    Resource_Release(loadedSampleAsset);
                                                  }
                                                }
                                                if ((levelConditionStorage->levelImage).worldSettings.
                                                    effectSampleNumbers[3] != 0) {
                                                  g_WideNumberFormatUtf16
                                                            (WIDE_FORMAT_PAD_WITH_ZERO,0,2,1,
                                                             (levelConditionStorage->levelImage).worldSettings.
                                                             effectSampleNumbers[3],
                                                             (uint16_t *)(u_sound_level00_sam_0050df6c +
                                                                     0xb));
                                                  resourceLoadResult = Resource_Load((uint16_t *)
                                                  u_sound_level00_sam_0050df6c);
                                                  loadedSampleAsset =
                                                       (SoundSampleAsset *)resourceLoadResult.bufferOrError;
                                                  sampleOrVoiceSet = loadedSampleAsset;
                                                  if (!resourceLoadResult.failed) {
                                                    voiceSetResult = g_SoundCreateSampleVoiceSet(loadedSampleAsset);
                                                    sampleOrVoiceSet = (SoundSampleAsset *)voiceSetResult.voiceSet;
                                                    if (!voiceSetResult.failed) {
                                                      g_InGameLevelEffectVoiceSet3 = sampleOrVoiceSet;
                                                    }
                                                    Resource_Release(loadedSampleAsset);
                                                  }
                                                }
                                                if ((levelConditionStorage->levelImage).worldSettings.
                                                    musicSampleNumbers[0] != 0) {
                                                  g_WideNumberFormatUtf16
                                                            (WIDE_FORMAT_PAD_WITH_ZERO,0,2,1,
                                                             (levelConditionStorage->levelImage).worldSettings.
                                                             musicSampleNumbers[0],
                                                             (uint16_t *)(u_sound_music00_sam_0050df90 +
                                                                     0xb));
                                                  resourceLoadResult = Resource_Load((uint16_t *)
                                                  u_sound_music00_sam_0050df90);
                                                  loadedSampleAsset =
                                                       (SoundSampleAsset *)resourceLoadResult.bufferOrError;
                                                  sampleOrVoiceSet = loadedSampleAsset;
                                                  if (!resourceLoadResult.failed) {
                                                    voiceSetResult = g_SoundCreateSampleVoiceSet(loadedSampleAsset);
                                                    sampleOrVoiceSet = (SoundSampleAsset *)voiceSetResult.voiceSet;
                                                    if (!voiceSetResult.failed) {
                                                      g_InGameLevelMusicVoiceSet0 = sampleOrVoiceSet;
                                                    }
                                                    Resource_Release(loadedSampleAsset);
                                                  }
                                                }
                                                if ((levelConditionStorage->levelImage).worldSettings.
                                                    musicSampleNumbers[1] != 0) {
                                                  g_WideNumberFormatUtf16
                                                            (WIDE_FORMAT_PAD_WITH_ZERO,0,2,1,
                                                             (levelConditionStorage->levelImage).worldSettings.
                                                             musicSampleNumbers[1],
                                                             (uint16_t *)(u_sound_music00_sam_0050df90 +
                                                                     0xb));
                                                  resourceLoadResult = Resource_Load((uint16_t *)
                                                  u_sound_music00_sam_0050df90);
                                                  loadedSampleAsset =
                                                       (SoundSampleAsset *)resourceLoadResult.bufferOrError;
                                                  sampleOrVoiceSet = loadedSampleAsset;
                                                  if (!resourceLoadResult.failed) {
                                                    voiceSetResult = g_SoundCreateSampleVoiceSet(loadedSampleAsset);
                                                    sampleOrVoiceSet = (SoundSampleAsset *)voiceSetResult.voiceSet;
                                                    if (!voiceSetResult.failed) {
                                                      g_InGameLevelMusicVoiceSet1 = sampleOrVoiceSet;
                                                    }
                                                    Resource_Release(loadedSampleAsset);
                                                  }
                                                }
                                                if ((levelConditionStorage->levelImage).worldSettings.
                                                    musicSampleNumbers[2] != 0) {
                                                  g_WideNumberFormatUtf16
                                                            (WIDE_FORMAT_PAD_WITH_ZERO,0,2,1,
                                                             (levelConditionStorage->levelImage).worldSettings.
                                                             musicSampleNumbers[2],
                                                             (uint16_t *)(u_sound_music00_sam_0050df90 +
                                                                     0xb));
                                                  resourceLoadResult = Resource_Load((uint16_t *)
                                                  u_sound_music00_sam_0050df90);
                                                  loadedSampleAsset =
                                                       (SoundSampleAsset *)resourceLoadResult.bufferOrError;
                                                  sampleOrVoiceSet = loadedSampleAsset;
                                                  if (!resourceLoadResult.failed) {
                                                    voiceSetResult = g_SoundCreateSampleVoiceSet(loadedSampleAsset);
                                                    sampleOrVoiceSet = (SoundSampleAsset *)voiceSetResult.voiceSet;
                                                    if (!voiceSetResult.failed) {
                                                      g_InGameLevelMusicVoiceSet2 = sampleOrVoiceSet;
                                                    }
                                                    Resource_Release(loadedSampleAsset);
                                                  }
                                                }
                                                if ((levelConditionStorage->levelImage).worldSettings.
                                                    musicSampleNumbers[3] != 0) {
                                                  g_WideNumberFormatUtf16
                                                            (WIDE_FORMAT_PAD_WITH_ZERO,0,2,1,
                                                             (levelConditionStorage->levelImage).worldSettings.
                                                             musicSampleNumbers[3],
                                                             (uint16_t *)(u_sound_music00_sam_0050df90 +
                                                                     0xb));
                                                  resourceLoadResult = Resource_Load((uint16_t *)
                                                  u_sound_music00_sam_0050df90);
                                                  loadedSampleAsset =
                                                       (SoundSampleAsset *)resourceLoadResult.bufferOrError;
                                                  sampleOrVoiceSet = loadedSampleAsset;
                                                  if (!resourceLoadResult.failed) {
                                                    voiceSetResult = g_SoundCreateSampleVoiceSet(loadedSampleAsset);
                                                    sampleOrVoiceSet = (SoundSampleAsset *)voiceSetResult.voiceSet;
                                                    if (!voiceSetResult.failed) {
                                                      g_InGameLevelMusicVoiceSet3 = sampleOrVoiceSet;
                                                    }
                                                    Resource_Release(loadedSampleAsset);
                                                  }
                                                }
                                                successResult.failed = false;
                                                successResult.errorOrValue = (uint32_t)sampleOrVoiceSet;
                                                return successResult;
                                              }
                                              soundSlotCursor = worldRuntime->dwordArray;
                                              soundLoopWorldRuntime = worldRuntime;
                                              soundSlotIndex = 
                                                  WidePath_ParseTrailingNumberBeforeExtensionRegs
                                                            (soundDirectoryPathCursor);
                                              if (soundSlotIndex < worldRuntime->dwordArrayCount) {
                                                if (!findEntryResult.failed) {
                                                  resourceLoadResult = Resource_Load(soundDirectoryPathCursor);
                                                  shrinkResultOrError = (void *)resourceLoadResult.bufferOrError;
                                                  if (resourceLoadResult.failed) break;
                                                }
                                                else {
                                                  WidePath_CombineDirectoryAndLeaf
                                                            ((uint16_t *)&
                                                  g_InGameLevelSoundLeafOrCombinedPathScratchUtf16,
                                                  soundDirectoryPathCursor,
                                                  (uint16_t *)&
                                                  g_InGameLevelSoundParentDirectoryScratchUtf16);
                                                  resourceLoadResult = Resource_Load((uint16_t *)&
                                                  g_InGameLevelSoundLeafOrCombinedPathScratchUtf16);
                                                  shrinkResultOrError = (void *)resourceLoadResult.bufferOrError;
                                                  if (resourceLoadResult.failed) break;
                                                }
                                                soundSlotResult = SpatialSoundSlot_CreateFromSampleAsset
                                                                   (shrinkResultOrError);
                                                if (!soundSlotResult.failed) {
                                                  soundSlotCursor[soundSlotIndex] = (uint32_t)soundSlotResult.soundSlot;
                                                }
                                                Resource_Release(shrinkResultOrError);
                                                MoviePlayback_AdvanceScheduledFrameAndTick();
                                              }
                                              countOrPackedValue--;
                                              worldRuntime = soundLoopWorldRuntime;
                                              /* advance by one directory record (soundDirectoryRecordSizeBytes) */
                                              soundDirectoryPathCursor =
                                                   (uint16_t *)((int)soundDirectoryPathCursor +
                                                                (int)soundDirectoryRecordSizeBytes);
                                              levelConditionStorage = g_InGameLevelRuntimeGlobalBlock.
                                                       conditionStorage;
                                            } while( true );
                                          }
                                        }
                                        g_MemoryApi.free(resultOrPointer);
                                        resultOrPointer = shrinkResultOrError;
                                      }
                                    }
                                  }
                                }
                              }
                            }
                          }
                        }
                      }
                    }
                  }
                }
              }
            }
          }
        }
      }
    }
  }
load_failed:
  failureResult.failed = true;
  failureResult.errorOrValue = (uint32_t)resultOrPointer;
  return failureResult;
}


/* Address: 0x005329C0.
   Releases everything a level loader set up: the effect, shot, model, army and terrain graphics, the spatial
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
  g_SoundReleaseSampleVoiceSet(g_InGameLevelEffectVoiceSet0);
  g_SoundReleaseSampleVoiceSet(g_InGameLevelEffectVoiceSet1);
  g_SoundReleaseSampleVoiceSet(g_InGameLevelEffectVoiceSet2);
  g_SoundReleaseSampleVoiceSet(g_InGameLevelEffectVoiceSet3);
  g_SoundReleaseSampleVoiceSet(g_InGameLevelMusicVoiceSet0);
  g_SoundReleaseSampleVoiceSet(g_InGameLevelMusicVoiceSet1);
  g_SoundReleaseSampleVoiceSet(g_InGameLevelMusicVoiceSet2);
  g_SoundReleaseSampleVoiceSet(g_InGameLevelMusicVoiceSet3);
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


/* Address: 0x00532CA0.
   Editor save of the current level: reloads the level asset (g_LevelEndingMovieSourcePath) into the package
   scratch buffer, replaces its placement table with one 0x20-byte record per live world model, stores the
   field region and the seven camera bookmarks and writes the image back to the same path. CF reports failure
   (load or write error in EAX). Called by InGameUiCommand_SaveFieldAndLevelAssetImages (ui/ingame/runtime.c);
   the field grid itself is written separately.
*/

StatusResult InGameLevelRuntime_SaveLevelAssetImageFromWorldState(InGameLevelSaveWorldView *saveWorldView)

{
  int placementOffsetOrModelRuntime;
  FactionRuntimeIndex activeFactionIndex;
  WorldOwnerListNode *ownerListNode;
  AngleTurn32 modelRotationAngle;
  uint8_t *levelImageBytes;
  LevelAssetRuntimePrefix *levelImage;
  uint32_t bookmarkZ;
  uint32_t bookmarkMagnitude;
  uint32_t bookmarkPackedHeadingPitch;
  uint32_t statusOrFieldValue;
  LevelInitialArmyPlacementRecord20 *placementRecordCursor;
  StatusResult statusResult;

  statusResult = Package_LoadEntryIntoBuffer(PACKAGE_SCRATCH_BUFFER_BYTES,g_PackageScratchBuffer,
                                             g_LevelEndingMovieSourcePath);
  levelImageBytes = g_PackageScratchBuffer;
  levelImage = (LevelAssetRuntimePrefix *)levelImageBytes;
  statusOrFieldValue = statusResult.valueOrError;
  if (!statusResult.failed) {
    /* the placement table is the last part of the image: the file is cut there and regrown per record */
    placementOffsetOrModelRuntime =
         (int)levelImage->header.resourceTables.runtimePrefixByteSizeAndInitialArmyPlacementOffset;
    activeFactionIndex = (saveWorldView->worldRuntime).activeFactionRuntimeIndex;
    levelImage->header.common.allocationSizeBytes = placementOffsetOrModelRuntime;
    /* header.initialArmyPlacementRecordCount = 0, byte by byte */
    ((uint8_t *)&levelImage->header.initialArmyPlacementRecordCount)[0] = 0;
    ((uint8_t *)&levelImage->header.initialArmyPlacementRecordCount)[1] = 0;
    ((uint8_t *)&levelImage->header.initialArmyPlacementRecordCount)[2] = 0;
    ((uint8_t *)&levelImage->header.initialArmyPlacementRecordCount)[3] = 0;
    /* the loader adds 7 to this value for faction 7's class/mode, yet the editor stores the index of the
       faction it plays here */
    levelImage->playerSlots[6].aiClassOrMode = activeFactionIndex;
    placementRecordCursor = (LevelInitialArmyPlacementRecord20 *)(levelImageBytes + placementOffsetOrModelRuntime);
    for (ownerListNode = (saveWorldView->worldRuntime).ownerListHead;
        ownerListNode != NULL; ownerListNode = ownerListNode->nextNode) {
      if (ownerListNode->ownerClassId == WORLD_OWNER_RUNTIME_MODEL) {
        levelImage->header.initialArmyPlacementRecordCount++;
        levelImage->header.common.allocationSizeBytes = levelImage->header.common.allocationSizeBytes + 0x20;
        placementOffsetOrModelRuntime =
             (int)((ModelRuntimeSlot *)ownerListNode->runtimePayload)->ownerArmyRuntimeOrSavedOffset.armyRuntime;
        /* +0x08 receives the node's world X and +0x0C its world Y, the reverse of the placement record's field
           names (which follow the parameters the loader passes them to, ArmyRuntime_CreateInstanceFromAsset) */
        placementRecordCursor->worldYQ12 = ownerListNode->worldXQ12;
        statusOrFieldValue = ((ArmyRuntimeSlot *)placementOffsetOrModelRuntime)->armyAssetId;
        placementRecordCursor->factionIndex = ((ArmyRuntimeSlot *)placementOffsetOrModelRuntime)->factionIndex;
        placementRecordCursor->armyAssetId = statusOrFieldValue;
        modelRotationAngle = ownerListNode->modelLocalRotationAngle2;
        placementRecordCursor->worldXQ12 = ownerListNode->worldYQ12;
        placementRecordCursor->orientationAngle = modelRotationAngle;
        placementRecordCursor->reserved14_1F[0] = 0;
        placementRecordCursor->reserved14_1F[1] = 0;
        placementRecordCursor->reserved14_1F[2] = 0;
        placementRecordCursor->reserved14_1F[3] = 0;
        placementRecordCursor->reserved14_1F[4] = 0;
        placementRecordCursor->reserved14_1F[5] = 0;
        placementRecordCursor->reserved14_1F[6] = 0;
        placementRecordCursor->reserved14_1F[7] = 0;
        placementRecordCursor->reserved14_1F[8] = 0;
        placementRecordCursor->reserved14_1F[9] = 0;
        placementRecordCursor->reserved14_1F[10] = 0;
        placementRecordCursor->reserved14_1F[11] = 0;
        placementRecordCursor++;
      }
    }
    levelImage->worldSettings.packedFieldRegionOriginYHigh16XLow16 =
         saveWorldView->lightAzimuthAngle & 0xffffU |
         saveWorldView->lightElevationAngle << 16;
    levelImage->worldSettings.packedFieldRegionHeightHigh16WidthLow16 =
         (saveWorldView->worldRuntime).fieldRegion.auxiliaryAzimuthAngle & 0xffff |
         (saveWorldView->worldRuntime).fieldRegion.auxiliaryElevationAngle << 16;
    bookmarkPackedHeadingPitch = g_LevelCameraBookmark1PackedHeadingLow16PitchHigh16;
    bookmarkMagnitude = g_LevelCameraBookmark1PositionMagnitudeQ12;
    bookmarkZ = g_LevelCameraBookmark1PositionZQ12;
    statusOrFieldValue = g_LevelCameraBookmark1PositionYQ12;
    levelImage->playerSlots[0].startCameraXQ12 = g_LevelCameraBookmark1PositionXQ12;
    levelImage->playerSlots[0].startCameraYQ12 = statusOrFieldValue;
    levelImage->playerSlots[0].startCameraZQ12 = bookmarkZ;
    levelImage->playerSlots[0].startCameraMagnitudeQ12 = bookmarkMagnitude;
    levelImage->playerSlots[0].packedHeadingLow16PitchHigh16 = bookmarkPackedHeadingPitch;
    bookmarkPackedHeadingPitch = g_LevelCameraBookmark2PackedHeadingLow16PitchHigh16;
    bookmarkMagnitude = g_LevelCameraBookmark2PositionMagnitudeQ12;
    bookmarkZ = g_LevelCameraBookmark2PositionZQ12;
    statusOrFieldValue = g_LevelCameraBookmark2PositionYQ12;
    levelImage->playerSlots[1].startCameraXQ12 = g_LevelCameraBookmark2PositionXQ12;
    levelImage->playerSlots[1].startCameraYQ12 = statusOrFieldValue;
    levelImage->playerSlots[1].startCameraZQ12 = bookmarkZ;
    levelImage->playerSlots[1].startCameraMagnitudeQ12 = bookmarkMagnitude;
    levelImage->playerSlots[1].packedHeadingLow16PitchHigh16 = bookmarkPackedHeadingPitch;
    bookmarkPackedHeadingPitch = g_LevelCameraBookmark3PackedHeadingLow16PitchHigh16;
    bookmarkMagnitude = g_LevelCameraBookmark3PositionMagnitudeQ12;
    bookmarkZ = g_LevelCameraBookmark3PositionZQ12;
    statusOrFieldValue = g_LevelCameraBookmark3PositionYQ12;
    levelImage->playerSlots[2].startCameraXQ12 = g_LevelCameraBookmark3PositionXQ12;
    levelImage->playerSlots[2].startCameraYQ12 = statusOrFieldValue;
    levelImage->playerSlots[2].startCameraZQ12 = bookmarkZ;
    levelImage->playerSlots[2].startCameraMagnitudeQ12 = bookmarkMagnitude;
    levelImage->playerSlots[2].packedHeadingLow16PitchHigh16 = bookmarkPackedHeadingPitch;
    bookmarkPackedHeadingPitch = g_LevelCameraBookmark4PackedHeadingLow16PitchHigh16;
    bookmarkMagnitude = g_LevelCameraBookmark4PositionMagnitudeQ12;
    bookmarkZ = g_LevelCameraBookmark4PositionZQ12;
    statusOrFieldValue = g_LevelCameraBookmark4PositionYQ12;
    levelImage->playerSlots[3].startCameraXQ12 = g_LevelCameraBookmark4PositionXQ12;
    levelImage->playerSlots[3].startCameraYQ12 = statusOrFieldValue;
    levelImage->playerSlots[3].startCameraZQ12 = bookmarkZ;
    levelImage->playerSlots[3].startCameraMagnitudeQ12 = bookmarkMagnitude;
    levelImage->playerSlots[3].packedHeadingLow16PitchHigh16 = bookmarkPackedHeadingPitch;
    bookmarkPackedHeadingPitch = g_LevelCameraBookmark5PackedHeadingLow16PitchHigh16;
    bookmarkMagnitude = g_LevelCameraBookmark5PositionMagnitudeQ12;
    bookmarkZ = g_LevelCameraBookmark5PositionZQ12;
    statusOrFieldValue = g_LevelCameraBookmark5PositionYQ12;
    levelImage->playerSlots[4].startCameraXQ12 = g_LevelCameraBookmark5PositionXQ12;
    levelImage->playerSlots[4].startCameraYQ12 = statusOrFieldValue;
    levelImage->playerSlots[4].startCameraZQ12 = bookmarkZ;
    levelImage->playerSlots[4].startCameraMagnitudeQ12 = bookmarkMagnitude;
    levelImage->playerSlots[4].packedHeadingLow16PitchHigh16 = bookmarkPackedHeadingPitch;
    bookmarkPackedHeadingPitch = g_LevelCameraBookmark6PackedHeadingLow16PitchHigh16;
    bookmarkMagnitude = g_LevelCameraBookmark6PositionMagnitudeQ12;
    bookmarkZ = g_LevelCameraBookmark6PositionZQ12;
    statusOrFieldValue = g_LevelCameraBookmark6PositionYQ12;
    levelImage->playerSlots[5].startCameraXQ12 = g_LevelCameraBookmark6PositionXQ12;
    levelImage->playerSlots[5].startCameraYQ12 = statusOrFieldValue;
    levelImage->playerSlots[5].startCameraZQ12 = bookmarkZ;
    levelImage->playerSlots[5].startCameraMagnitudeQ12 = bookmarkMagnitude;
    levelImage->playerSlots[5].packedHeadingLow16PitchHigh16 = bookmarkPackedHeadingPitch;
    bookmarkPackedHeadingPitch = g_LevelCameraBookmark7PackedHeadingLow16PitchHigh16;
    bookmarkMagnitude = g_LevelCameraBookmark7PositionMagnitudeQ12;
    bookmarkZ = g_LevelCameraBookmark7PositionZQ12;
    statusOrFieldValue = g_LevelCameraBookmark7PositionYQ12;
    levelImage->playerSlots[6].startCameraXQ12 = g_LevelCameraBookmark7PositionXQ12;
    levelImage->playerSlots[6].startCameraYQ12 = statusOrFieldValue;
    levelImage->playerSlots[6].startCameraZQ12 = bookmarkZ;
    levelImage->playerSlots[6].startCameraMagnitudeQ12 = bookmarkMagnitude;
    levelImage->playerSlots[6].packedHeadingLow16PitchHigh16 = bookmarkPackedHeadingPitch;
    statusResult = FileSystem_WriteBufferToPath
                      (levelImage->header.common.allocationSizeBytes,levelImageBytes,g_LevelEndingMovieSourcePath);
    statusOrFieldValue = statusResult.valueOrError;
    if (!statusResult.failed) {
      return THANDOR_BITCAST(uint64_t, StatusResult,
                             ((THANDOR_BITCAST(StatusResult, uint64_t, statusResult) & 0xFFFFFFFFFFull) & 0xffffffff));
    }
  }
  statusResult.failed = true;
  statusResult.valueOrError = statusOrFieldValue;
  return statusResult;
}


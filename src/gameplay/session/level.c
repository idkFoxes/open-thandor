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
   Prepares the movies of a level before it is loaded: stores the level's loading movie (the path at LEV +0xCC
   with its extension set to "flm") in *outMoviePath and writes the matching end movie number into
   "flm\ende0000.flm" from the 5th and 6th characters of that path ('w' 0xFC -> 2, "ei" -> 3, "la" -> 4, anything
   else 0). It keeps a copy of the level path in g_LevelEndingMovieSourcePath, which the loaders report on errors.
   Returns true on success; false with FATAL_ERROR_LEVEL_ASSET_INVALID in *outError when asset is not a LEV
   asset of converter version 0x70001.
*/
bool LevelAsset_PrepareEndingMoviePath
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
               (uint16_t *)(u_flm_ende0000_flm_0050df06 + 8)); /* the "0000" */
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

/* Address: 0x005311D0.
   Loads a new level (after GameData_ResetDefaults): copies the level prefix, loads the technology file and all
   listed EFF/SHT/MDL/ARM files, initialises terrain, graphics pools, camera bookmarks, start resources and
   factions, spawns the initial armies, loads the level sounds, gives factions that start without a structure a
   default build list and applies the initial faction relations.
*/

bool InGameLevelRuntime_LoadResourcesAfterDefaultReset
          (LevelAssetRuntimePrefix *levelImage,WorldRuntimeContext *worldRuntime,uint32_t *outError)

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
  uint32_t terrainLoadError;
  uint32_t assetError;
  bool armyRuntimeInitialized;
  uint32_t armyInitError;
  bool shotsInitialized;
  uint32_t shotInitError;
  bool effectsInitialized;
  uint32_t effectInitError;
  ArmyRuntimeSlot *createdArmy;
  uint32_t armyCreateError;
  ArenaShrinkResult shrinkResult;
  SpatialSoundSlot *soundSlot;
  SampleVoiceSetResult voiceSetResult;
  ModelDefinitionRecordPrefix *rootModelDefinition;
  ArenaLargestAllocResult largestBlockResult;
  bool soundsInPackage; /* the sounds are listed from g_SoundPackageHandle, not a directory */
  DirectoryEnumerationResult enumerationResult;
  void *loadedSampleBuffer;
  uint32_t loadErrorCode;
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
        assetPath = (uint8_t *)levelImage + (levelImage->header).pathOffsets.technologyPathOffset;
        WidePath_SetExtensionCode(ASSET_MAGIC_TEC,(uint16_t *)assetPath);
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
          g_GameFactionRuntimeImage.records[1].colorIndex =
               levelImage->playerSlots[0].aiClassOrMode + 1;
          g_GameFactionRuntimeImage.records[2].colorIndex =
               levelImage->playerSlots[1].aiClassOrMode + 2;
          g_GameFactionRuntimeImage.records[3].colorIndex =
               levelImage->playerSlots[2].aiClassOrMode + 3;
          g_GameFactionRuntimeImage.tail.activeFactionCount =
               (levelImage->worldSettings).activeFactionCount;
          g_GameFactionRuntimeImage.records[4].colorIndex =
               levelImage->playerSlots[3].aiClassOrMode + 4;
          g_GameFactionRuntimeImage.records[5].colorIndex =
               levelImage->playerSlots[4].aiClassOrMode + 5;
          g_GameFactionRuntimeImage.records[6].colorIndex =
               levelImage->playerSlots[5].aiClassOrMode + 6;
          g_GameFactionRuntimeImage.records[7].colorIndex =
               levelImage->playerSlots[6].aiClassOrMode + 7;
          /* loading stage 0: one movie step per EFF, SHT, MDL and ARM file */
          g_MoviePlaybackScheduleSpan =
               (levelImage->header).resourceTables.effectAssetPathCount +
               (levelImage->header).resourceTables.shotAssetPathCount +
               (levelImage->header).resourceTables.modelAssetPathCount +
               (levelImage->header).resourceTables.armyAssetPathCount;
          g_MoviePlaybackBaseFrameGroup = 0;
          g_MoviePlaybackScheduleCounter = 0;
          assetPathCursor = (uint16_t *)((uint8_t *)levelImage +
                                         (levelImage->header).resourceTables.effectAssetPathTableOffset);
          for (remainingRecordCount = (levelImage->header).resourceTables.effectAssetPathCount;
                 remainingRecordCount != 0;
              remainingRecordCount--) {
            WidePath_SetExtensionCode(ASSET_MAGIC_EFF,assetPathCursor);
            resultOrPointer = (void *)FATAL_ERROR_LEVEL_TOO_MANY_RESOURCES;
            if (INGAME_LOADED_RESOURCE_CAPACITY - 1 < g_InGameLoadedResourcePointerCount) goto load_failed;
            loadEntryResult = Package_LoadEntry(assetPathCursor);
            resultOrPointer = loadEntryResult.bufferOrError;
            if (loadEntryResult.failed) goto load_failed;
            *loadedResourcePointerArray = resultOrPointer;
            g_InGameLoadedResourcePointerCount++;
            loadedResourcePointerArray++;
            if (!EffectAsset_PrepareEntries(resultOrPointer,&assetError)) {
              resultOrPointer = (void *)assetError;
              goto load_failed;
            }
            MoviePlayback_AdvanceScheduledFrameAndTick();
            assetPathCursor =assetPathCursor + LEVEL_ASSET_PATH_RECORD_UNITS; /* next path record */
          }
          assetPathCursor = (uint16_t *)((uint8_t *)levelImage +
                                         (levelImage->header).resourceTables.shotAssetPathTableOffset);
          for (remainingRecordCount = (levelImage->header).resourceTables.shotAssetPathCount;
                 remainingRecordCount != 0;
              remainingRecordCount--) {
            WidePath_SetExtensionCode(ASSET_MAGIC_SHT,assetPathCursor);
            resultOrPointer = (void *)FATAL_ERROR_LEVEL_TOO_MANY_RESOURCES;
            if (INGAME_LOADED_RESOURCE_CAPACITY - 1 < g_InGameLoadedResourcePointerCount) goto load_failed;
            loadEntryResult = Package_LoadEntry(assetPathCursor);
            resultOrPointer = loadEntryResult.bufferOrError;
            if (loadEntryResult.failed) goto load_failed;
            *loadedResourcePointerArray = resultOrPointer;
            g_InGameLoadedResourcePointerCount++;
            loadedResourcePointerArray++;
            assetError = ShotAsset_PrepareEntries(resultOrPointer);
            resultOrPointer = (void *)assetError;
            if (assetError != 0) goto load_failed;
            MoviePlayback_AdvanceScheduledFrameAndTick();
            assetPathCursor = assetPathCursor + LEVEL_ASSET_PATH_RECORD_UNITS; /* next path record */
          }
          assetError = EffectDefinitions_ResolveCrossReferences();
          resultOrPointer = (void *)assetError;
          if (assetError == 0) {
            assetPathCursor =(uint16_t *)((uint8_t *)levelImage +
                                           (levelImage->header).resourceTables.modelAssetPathTableOffset);
            for (remainingRecordCount = (levelImage->header).resourceTables.modelAssetPathCount;
                 remainingRecordCount != 0;
                remainingRecordCount--) {
              WidePath_SetExtensionCode(ASSET_MAGIC_MDL,assetPathCursor);
              resultOrPointer = (void *)FATAL_ERROR_LEVEL_TOO_MANY_RESOURCES;
              if (INGAME_LOADED_RESOURCE_CAPACITY - 1 < g_InGameLoadedResourcePointerCount) goto load_failed;
              loadEntryResult = Package_LoadEntry(assetPathCursor);
              resultOrPointer = loadEntryResult.bufferOrError;
              if (loadEntryResult.failed) goto load_failed;
              *loadedResourcePointerArray = resultOrPointer;
              g_InGameLoadedResourcePointerCount++;
              loadedResourcePointerArray++;
              if (!ModelAsset_PrepareRecords(resultOrPointer,&assetError)) {
                resultOrPointer = (void *)assetError;
                goto load_failed;
              }
              MoviePlayback_AdvanceScheduledFrameAndTick();
              assetPathCursor = assetPathCursor + LEVEL_ASSET_PATH_RECORD_UNITS; /* next path record */
            }
            assetPathCursor = (uint16_t *)((uint8_t *)levelImage +
                                           (levelImage->header).resourceTables.armyAssetPathTableOffset);
            for (remainingRecordCount = (levelImage->header).resourceTables.armyAssetPathCount;
                 remainingRecordCount != 0;
                remainingRecordCount--) {
              WidePath_SetExtensionCode(ASSET_MAGIC_ARM,assetPathCursor);
              resultOrPointer = (void *)FATAL_ERROR_LEVEL_TOO_MANY_RESOURCES;
              if (INGAME_LOADED_RESOURCE_CAPACITY - 1 < g_InGameLoadedResourcePointerCount) goto load_failed;
              loadEntryResult = Package_LoadEntry(assetPathCursor);
              resultOrPointer = loadEntryResult.bufferOrError;
              if (loadEntryResult.failed) goto load_failed;
              *loadedResourcePointerArray = resultOrPointer;
              g_InGameLoadedResourcePointerCount++;
              loadedResourcePointerArray++;
              assetError = ArmyAsset_PrepareRecords(resultOrPointer);
              resultOrPointer = (void *)assetError;
              if (assetError != 0) goto load_failed;
              MoviePlayback_AdvanceScheduledFrameAndTick();
              assetPathCursor =assetPathCursor + LEVEL_ASSET_PATH_RECORD_UNITS; /* next path record */
            }
            resultOrPointer = (void *)FATAL_ERROR_LEVEL_TOO_MANY_RESOURCES;
            if (g_InGameLoadedResourcePointerCount < INGAME_LOADED_RESOURCE_CAPACITY) {
              g_MoviePlaybackBaseFrameGroup = 1;
              g_MoviePlaybackScheduleCounter = 0;
              g_MoviePlaybackScheduleSpan = LEVEL_LOAD_MOVIE_SPAN_HOLD;
              /* stage 1: terrain textures (surface, ground) and the field grid */
              if (!TerrainVisualResources_LoadPrimary
                     ((uint16_t *)((uint8_t *)levelImage +
                                   (levelImage->header).pathOffsets.surfaceTextureBasePathOffset),
                      (uint16_t *)((uint8_t *)levelImage +
                                   (levelImage->header).pathOffsets.groundTextureBasePathOffset),
                      (FieldGridAsset *)(levelImage->header).pathOffsets.levelPathOffset,&terrainLoadError)) {
                resultOrPointer = (void *)terrainLoadError;
              }
              else {
                assetError = ShotDefinitions_ValidateTerrainMaterialReferences();
                resultOrPointer = (void *)assetError;
                if (assetError == 0) {
                  g_MoviePlaybackBaseFrameGroup = 2;
                  g_MoviePlaybackScheduleCounter = 0;
                  g_MoviePlaybackScheduleSpan = LEVEL_LOAD_MOVIE_SPAN_HOLD;
                  assetError = ModelRuntimePool_Init();
                  resultOrPointer = (void *)assetError;
                  if (assetError == 0) {
                    /* The decompiler lost these two locals; the original reads them from the level header
                       (+0xC0 army and +0xC8 effect texture base paths). */
                    armyTextureBasePath =
                         (uint16_t *)((uint8_t *)levelImage +
                                      (levelImage->header).pathOffsets.armyTextureBasePathOffset);
                    effectTextureBasePath =
                         (uint16_t *)((uint8_t *)levelImage +
                                      (levelImage->header).pathOffsets.effectTextureBasePathOffset);
                    armyRuntimeInitialized =
                         ArmyRuntime_InitializePoolAndGraphics(worldRuntime,armyTextureBasePath,&armyInitError);
                    resultOrPointer = (void *)armyInitError;
                    if (armyRuntimeInitialized) {
                      g_MoviePlaybackBaseFrameGroup = 3;
                      g_MoviePlaybackScheduleCounter = 0;
                      g_MoviePlaybackScheduleSpan = 4;
                      shotsInitialized = ShotRuntime_InitGraphicsResources
                                         ((uint16_t *)((uint8_t *)levelImage +
                                                       (levelImage->header).pathOffsets.shotTextureBasePathOffset),
                                          &shotInitError);
                      resultOrPointer = (void *)shotInitError;
                      if (shotsInitialized) {
                        g_MoviePlaybackBaseFrameGroup = 4;
                        g_MoviePlaybackScheduleCounter = 0;
                        g_MoviePlaybackScheduleSpan = 4;
                        effectsInitialized =
                             EffectRuntime_InitGraphicsResources(effectTextureBasePath,&effectInitError);
                        resultOrPointer = (void *)effectInitError;
                        if (effectsInitialized) {
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
                                     (levelImage->worldSettings).terrainRampStepColorArgb,worldRuntime)
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
                                    (2,(int)countOrPackedValue >> 16,countOrPackedValue & 0xffff,
                                     *(UQ12 *)((uint8_t *)&levelImage->playerSlots[0].
                                                     startCameraMagnitudeQ12 + playerSlotByteOffset),worldRuntime);
                          countOrPackedValue = (levelImage->worldSettings).packedFieldRegionOriginYHigh16XLow16;
                          WorldRuntime_RecomputeMotionEndpointAgainstFieldSurface(worldRuntime);
                          regionOriginOrRelationMask = countOrPackedValue;
                          WorldRuntime_CommitCameraTargetDistance(worldRuntime);
                          flagsOrRelationMask = (levelImage->worldSettings).
                                   packedFieldRegionHeightHigh16WidthLow16;
                          WorldRuntime_RecomputeFieldRegionNormalsAndLighting
                                    ((int)flagsOrRelationMask >> 16,flagsOrRelationMask & 0xffff,
                                     (int)regionOriginOrRelationMask >> 16,
                                     countOrPackedValue & 0xffff,worldRuntime);
                          /* initial army placements (LevelInitialArmyPlacementRecord20 records at LEV +[0xDC]) */
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
                                resultOrPointer = (void *)armyCreateError;
                                goto load_failed;
                              }
                              resultOrPointer = createdArmy;
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
                          assetPath = (uint8_t *)levelImage + (levelImage->header).pathOffsets.soundBasePathOffset;
                          WidePath_SetExtensionCode(ASSET_MAGIC_SAM,(uint16_t *)assetPath);
                          WidePath_SplitParentAndLeaf
                                    ((uint16_t *)&g_InGameLevelSoundLeafOrCombinedPathScratchUtf16,
                                     (uint16_t *)&g_InGameLevelSoundParentDirectoryScratchUtf16,
                                     (uint16_t *)assetPath);
                          largestBlockResult = g_MemoryApi.allocLargestFreeBlock();
                          outputCapacityBytes = largestBlockResult.blockSizeOrSentinel;
                          resultOrPointer = (void *)largestBlockResult.allocationOrError;
                          if (!largestBlockResult.failed) {
                            soundsInPackage = Package_FindEntry(outputCapacityBytes,resultOrPointer,
                                                       (uint16_t *)assetPath,g_SoundPackageHandle,
                                                       &countOrPackedValue);
                            soundDirectoryRecordSizeBytes = PCK_ENTRY_HEADER_BYTES;
                            if (!soundsInPackage) {
                              enumerationResult = g_FileSystemEnumerateDirectoryOrVolumeEntries
                                                 (FILESYSTEM_ENUMERATE_FILES,UINT32_MAX,
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
                                    g_InGameLevelEffectVoiceSet0 = NULL;
                                    g_InGameLevelEffectVoiceSet1 = NULL;
                                    g_InGameLevelEffectVoiceSet2 = NULL;
                                    g_InGameLevelEffectVoiceSet3 = NULL;
                                    g_InGameActiveEffectVoice = 0;
                                    g_InGameEffectsEnabled = 1;
                                    g_InGameActiveMusicVoice = 0;
                                    g_InGameMusicNextTrackCountdown = 1;
                                    g_InGameLevelMusicVoiceSet0 = NULL;
                                    g_InGameLevelMusicVoiceSet1 = NULL;
                                    g_InGameLevelMusicVoiceSet2 = NULL;
                                    g_InGameLevelMusicVoiceSet3 = NULL;
                                    /* the four level effect and four music samples of the tail */
                                    if ((levelConditionStorage->levelImage).worldSettings.effectSampleNumbers[0]
                                        != 0) {
                                      g_WideNumberFormatUtf16
                                                (WIDE_FORMAT_PAD_WITH_ZERO,0,2,1,
                                                 (levelConditionStorage->levelImage).worldSettings.
                                                 effectSampleNumbers[0],
                                                 (uint16_t *)(u_sound_level00_sam_0050df6c + 11));
                                      if (Resource_Load((uint16_t *)u_sound_level00_sam_0050df6c,&loadedSampleBuffer,NULL,NULL)) {
                                        voiceSetResult = g_SoundCreateSampleVoiceSet
                                                           ((SoundSampleAsset *)loadedSampleBuffer);
                                        if (!voiceSetResult.failed) {
                                          g_InGameLevelEffectVoiceSet0 = voiceSetResult.voiceSet;
                                        }
                                        Resource_Release((SoundSampleAsset *)loadedSampleBuffer);
                                      }
                                    }
                                    if ((levelConditionStorage->levelImage).worldSettings.effectSampleNumbers[1]
                                        != 0) {
                                      g_WideNumberFormatUtf16
                                                (WIDE_FORMAT_PAD_WITH_ZERO,0,2,1,
                                                 (levelConditionStorage->levelImage).worldSettings.
                                                 effectSampleNumbers[1],
                                                 (uint16_t *)(u_sound_level00_sam_0050df6c + 11));
                                      if (Resource_Load((uint16_t *)u_sound_level00_sam_0050df6c,&loadedSampleBuffer,NULL,NULL)) {
                                        voiceSetResult = g_SoundCreateSampleVoiceSet
                                                           ((SoundSampleAsset *)loadedSampleBuffer);
                                        if (!voiceSetResult.failed) {
                                          g_InGameLevelEffectVoiceSet1 = voiceSetResult.voiceSet;
                                        }
                                        Resource_Release((SoundSampleAsset *)loadedSampleBuffer);
                                      }
                                    }
                                    if ((levelConditionStorage->levelImage).worldSettings.effectSampleNumbers[2]
                                        != 0) {
                                      g_WideNumberFormatUtf16
                                                (WIDE_FORMAT_PAD_WITH_ZERO,0,2,1,
                                                 (levelConditionStorage->levelImage).worldSettings.
                                                 effectSampleNumbers[2],
                                                 (uint16_t *)(u_sound_level00_sam_0050df6c + 11));
                                      if (Resource_Load((uint16_t *)u_sound_level00_sam_0050df6c,&loadedSampleBuffer,NULL,NULL)) {
                                        voiceSetResult = g_SoundCreateSampleVoiceSet
                                                           ((SoundSampleAsset *)loadedSampleBuffer);
                                        if (!voiceSetResult.failed) {
                                          g_InGameLevelEffectVoiceSet2 = voiceSetResult.voiceSet;
                                        }
                                        Resource_Release((SoundSampleAsset *)loadedSampleBuffer);
                                      }
                                    }
                                    if ((levelConditionStorage->levelImage).worldSettings.effectSampleNumbers[3]
                                        != 0) {
                                      g_WideNumberFormatUtf16
                                                (WIDE_FORMAT_PAD_WITH_ZERO,0,2,1,
                                                 (levelConditionStorage->levelImage).worldSettings.
                                                 effectSampleNumbers[3],
                                                 (uint16_t *)(u_sound_level00_sam_0050df6c + 11));
                                      if (Resource_Load((uint16_t *)u_sound_level00_sam_0050df6c,&loadedSampleBuffer,NULL,NULL)) {
                                        voiceSetResult = g_SoundCreateSampleVoiceSet
                                                           ((SoundSampleAsset *)loadedSampleBuffer);
                                        if (!voiceSetResult.failed) {
                                          g_InGameLevelEffectVoiceSet3 = voiceSetResult.voiceSet;
                                        }
                                        Resource_Release((SoundSampleAsset *)loadedSampleBuffer);
                                      }
                                    }
                                    if ((levelConditionStorage->levelImage).worldSettings.musicSampleNumbers[0] !=
                                        0) {
                                      g_WideNumberFormatUtf16
                                                (WIDE_FORMAT_PAD_WITH_ZERO,0,2,1,
                                                 (levelConditionStorage->levelImage).worldSettings.
                                                 musicSampleNumbers[0],
                                                 (uint16_t *)(u_sound_music00_sam_0050df90 + 11));
                                      if (Resource_Load((uint16_t *)u_sound_music00_sam_0050df90,&loadedSampleBuffer,NULL,NULL)) {
                                        voiceSetResult = g_SoundCreateSampleVoiceSet
                                                           ((SoundSampleAsset *)loadedSampleBuffer);
                                        if (!voiceSetResult.failed) {
                                          g_InGameLevelMusicVoiceSet0 = voiceSetResult.voiceSet;
                                        }
                                        Resource_Release((SoundSampleAsset *)loadedSampleBuffer);
                                      }
                                    }
                                    if ((levelConditionStorage->levelImage).worldSettings.musicSampleNumbers[1] !=
                                        0) {
                                      g_WideNumberFormatUtf16
                                                (WIDE_FORMAT_PAD_WITH_ZERO,0,2,1,
                                                 (levelConditionStorage->levelImage).worldSettings.
                                                 musicSampleNumbers[1],
                                                 (uint16_t *)(u_sound_music00_sam_0050df90 + 11));
                                      if (Resource_Load((uint16_t *)u_sound_music00_sam_0050df90,&loadedSampleBuffer,NULL,NULL)) {
                                        voiceSetResult = g_SoundCreateSampleVoiceSet
                                                           ((SoundSampleAsset *)loadedSampleBuffer);
                                        if (!voiceSetResult.failed) {
                                          g_InGameLevelMusicVoiceSet1 = voiceSetResult.voiceSet;
                                        }
                                        Resource_Release((SoundSampleAsset *)loadedSampleBuffer);
                                      }
                                    }
                                    if ((levelConditionStorage->levelImage).worldSettings.musicSampleNumbers[2] !=
                                        0) {
                                      g_WideNumberFormatUtf16
                                                (WIDE_FORMAT_PAD_WITH_ZERO,0,2,1,
                                                 (levelConditionStorage->levelImage).worldSettings.
                                                 musicSampleNumbers[2],
                                                 (uint16_t *)(u_sound_music00_sam_0050df90 + 11));
                                      if (Resource_Load((uint16_t *)u_sound_music00_sam_0050df90,&loadedSampleBuffer,NULL,NULL)) {
                                        voiceSetResult = g_SoundCreateSampleVoiceSet
                                                           ((SoundSampleAsset *)loadedSampleBuffer);
                                        if (!voiceSetResult.failed) {
                                          g_InGameLevelMusicVoiceSet2 = voiceSetResult.voiceSet;
                                        }
                                        Resource_Release((SoundSampleAsset *)loadedSampleBuffer);
                                      }
                                    }
                                    if ((levelConditionStorage->levelImage).worldSettings.musicSampleNumbers[3] !=
                                        0) {
                                      g_WideNumberFormatUtf16
                                                (WIDE_FORMAT_PAD_WITH_ZERO,0,2,1,
                                                 (levelConditionStorage->levelImage).worldSettings.
                                                 musicSampleNumbers[3],
                                                 (uint16_t *)(u_sound_music00_sam_0050df90 + 11));
                                      if (Resource_Load((uint16_t *)u_sound_music00_sam_0050df90,&loadedSampleBuffer,NULL,NULL)) {
                                        voiceSetResult = g_SoundCreateSampleVoiceSet
                                                           ((SoundSampleAsset *)loadedSampleBuffer);
                                        if (!voiceSetResult.failed) {
                                          g_InGameLevelMusicVoiceSet3 = voiceSetResult.voiceSet;
                                        }
                                        Resource_Release((SoundSampleAsset *)loadedSampleBuffer);
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
                                        rootModelDefinition = ModelDefinitionRegistry_FindById
                                                           (((ArmyModelTreeNode *)registryArmyDefinition->
                                                             rootNodeOffsetOrPointer)->linkedDefinitionIds[0]);
                                        if (rootModelDefinition == NULL) {
                                          /* Original quirk: a failed lookup is not checked; its error code is
                                             read as the definition */
                                          rootModelDefinition =
                                               (ModelDefinitionRecordPrefix *)FATAL_ERROR_MODEL_DEFINITION_MISSING;
                                        }
                                        modelFlagsOrSoundIndex =
                                             ((ModelDefinition *)rootModelDefinition)->
                                                                 runtimeClassId;
                                        nextClass0BArmyDefinition = registryArmyDefinition;
                                        if (((modelFlagsOrSoundIndex != MODEL_RUNTIME_CLASS_11) &&
                                            ((nextClass10ArmyDefinition = registryArmyDefinition,
                                             nextClass0BArmyDefinition = class0BArmyDefinition,
                                                  modelFlagsOrSoundIndex != MODEL_RUNTIME_CLASS_16 &&
                                             (nextClass10ArmyDefinition = class10ArmyDefinition,
                                              modelFlagsOrSoundIndex == MODEL_RUNTIME_CLASS_14)))) &&
                                           (nextClass0EArmyDefinition = registryArmyDefinition,
                                           ((ModelDefinition *)rootModelDefinition)->
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
                                              if (counterOrClassId == MODEL_RUNTIME_CLASS_18) {
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
                                                        (32,32,FACTION_RELATION_STATE_FRIENDLY,
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
                                                        (32,32,FACTION_RELATION_STATE_ALLIED,
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
                                    return true;
                                  }
                                  soundSlotCursor = soundLoopWorldRuntime->dwordArray;
                                  soundLoopWorldRuntimeCopy = soundLoopWorldRuntime;
                                  modelFlagsOrSoundIndex = WidePath_ParseTrailingNumberBeforeExtensionRegs
                                                    (soundDirectoryPathCursor);
                                  if (modelFlagsOrSoundIndex < soundLoopWorldRuntime->dwordArrayCount) {
                                    if (soundsInPackage) {
                                      if (!Resource_Load(soundDirectoryPathCursor,&shrinkResultOrError,NULL,&loadErrorCode)) {
                                        shrinkResultOrError = (void *)loadErrorCode; /* passed on as this function's error */
                                        break;
                                      }
                                    }
                                    else {
                                      WidePath_CombineDirectoryAndLeaf
                                                ((uint16_t *)&
                                                  g_InGameLevelSoundLeafOrCombinedPathScratchUtf16,
                                                 soundDirectoryPathCursor,
                                                 (uint16_t *)&
                                                  g_InGameLevelSoundParentDirectoryScratchUtf16);
                                      if (!Resource_Load((uint16_t *)&
                                                  g_InGameLevelSoundLeafOrCombinedPathScratchUtf16,&shrinkResultOrError,NULL,&loadErrorCode)) {
                                        shrinkResultOrError = (void *)loadErrorCode; /* passed on as this function's error */
                                        break;
                                      }
                                    }
                                    soundSlot = SpatialSoundSlot_CreateFromSampleAsset(shrinkResultOrError);
                                    if (soundSlot != NULL) {
                                      soundSlotCursor[modelFlagsOrSoundIndex] = (uint32_t)soundSlot;
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
  *outError = (uint32_t)resultOrPointer;
  return false;
}


/* Address: 0x00532020.
   Loads the level of a saved game (after GameData_LoadExternalTables): the same LEV steps as
   InGameLevelRuntime_LoadResourcesAfterDefaultReset (see the layout above it) up to the camera, but instead of
   spawning the initial armies it restores the saved runtime pools from widget.hex, army.hex, modul.hex,
   effect.hex, shot.hex and light.hex and rebases their pointers. The player-slot copies, default build lists
   and initial relations are skipped; the saved faction image already holds them.
*/

bool InGameLevelRuntime_LoadResourcesAfterExternalTables
          (FrontendLoadedLevelAsset *levelImage,WorldRuntimeContext *worldRuntime,uint32_t *outError)

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
  uint32_t terrainLoadError;
  uint32_t assetError;
  bool armyRuntimeInitialized;
  uint32_t armyInitError;
  bool shotsInitialized;
  uint32_t shotInitError;
  bool effectsInitialized;
  uint32_t effectInitError;
  ArenaShrinkResult shrinkResult;
  SpatialSoundSlot *soundSlot;
  ArenaFreeResult freeResult;
  SampleVoiceSetResult voiceSetResult;
  ArenaLargestAllocResult largestBlockResult;
  bool soundsInPackage; /* the sounds are listed from g_SoundPackageHandle, not a directory */
  DirectoryEnumerationResult enumerationResult;
  bool sampleLoaded;
  uint32_t loadErrorCode;
  uint16_t *soundDirectoryPathCursor;
  WorldRuntimeContext *soundLoopWorldRuntime;
  bool poolLoaded;
  uint32_t poolByteCountOrError; /* the saved pool's byte count, or the load error code */

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
        assetPath = (uint8_t *)levelImage + (levelImage->header).pathState.technologyPathOffset;
        WidePath_SetExtensionCode(ASSET_MAGIC_TEC,(uint16_t *)assetPath);
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
          pathTableCursor = (uint16_t *)((uint8_t *)levelImage +
                                         (levelImage->header).resourceTables.effectAssetPathTableOffset);
          for (remainingPathCount = (levelImage->header).resourceTables.effectAssetPathCount; remainingPathCount != 0;
              remainingPathCount--) {
            WidePath_SetExtensionCode(ASSET_MAGIC_EFF,pathTableCursor);
            resultOrPointer = (void *)FATAL_ERROR_LEVEL_TOO_MANY_RESOURCES;
            if (INGAME_LOADED_RESOURCE_CAPACITY - 1 < g_InGameLoadedResourcePointerCount) goto load_failed;
            loadEntryResult = Package_LoadEntry(pathTableCursor);
            resultOrPointer = loadEntryResult.bufferOrError;
            if (loadEntryResult.failed) goto load_failed;
            *loadedResourcePointerArray = resultOrPointer;
            g_InGameLoadedResourcePointerCount++;
            loadedResourcePointerArray++;
            if (!EffectAsset_PrepareEntries(resultOrPointer,&assetError)) {
              resultOrPointer = (void *)assetError;
              goto load_failed;
            }
            MoviePlayback_AdvanceScheduledFrameAndTick();
            pathTableCursor =pathTableCursor + LEVEL_ASSET_PATH_RECORD_UNITS; /* next path record */
          }
          pathTableCursor = (uint16_t *)((uint8_t *)levelImage +
                                         (levelImage->header).resourceTables.shotAssetPathTableOffset);
          for (remainingPathCount = (levelImage->header).resourceTables.shotAssetPathCount; remainingPathCount != 0;
              remainingPathCount--) {
            WidePath_SetExtensionCode(ASSET_MAGIC_SHT,pathTableCursor);
            resultOrPointer = (void *)FATAL_ERROR_LEVEL_TOO_MANY_RESOURCES;
            if (INGAME_LOADED_RESOURCE_CAPACITY - 1 < g_InGameLoadedResourcePointerCount) goto load_failed;
            loadEntryResult = Package_LoadEntry(pathTableCursor);
            resultOrPointer = loadEntryResult.bufferOrError;
            if (loadEntryResult.failed) goto load_failed;
            *loadedResourcePointerArray = resultOrPointer;
            g_InGameLoadedResourcePointerCount++;
            loadedResourcePointerArray++;
            assetError = ShotAsset_PrepareEntries(resultOrPointer);
            resultOrPointer = (void *)assetError;
            if (assetError != 0) goto load_failed;
            MoviePlayback_AdvanceScheduledFrameAndTick();
            pathTableCursor = pathTableCursor + LEVEL_ASSET_PATH_RECORD_UNITS; /* next path record */
          }
          assetError = EffectDefinitions_ResolveCrossReferences();
          resultOrPointer = (void *)assetError;
          if (assetError == 0) {
            pathTableCursor =(uint16_t *)((uint8_t *)levelImage +
                                           (levelImage->header).resourceTables.modelAssetPathTableOffset);
            for (remainingPathCount = (levelImage->header).resourceTables.modelAssetPathCount; remainingPathCount != 0;
                remainingPathCount--) {
              WidePath_SetExtensionCode(ASSET_MAGIC_MDL,pathTableCursor);
              resultOrPointer = (void *)FATAL_ERROR_LEVEL_TOO_MANY_RESOURCES;
              if (INGAME_LOADED_RESOURCE_CAPACITY - 1 < g_InGameLoadedResourcePointerCount) goto load_failed;
              loadEntryResult = Package_LoadEntry(pathTableCursor);
              resultOrPointer = loadEntryResult.bufferOrError;
              if (loadEntryResult.failed) goto load_failed;
              *loadedResourcePointerArray = resultOrPointer;
              g_InGameLoadedResourcePointerCount++;
              loadedResourcePointerArray++;
              if (!ModelAsset_PrepareRecords(resultOrPointer,&assetError)) {
                resultOrPointer = (void *)assetError;
                goto load_failed;
              }
              MoviePlayback_AdvanceScheduledFrameAndTick();
              pathTableCursor = pathTableCursor + LEVEL_ASSET_PATH_RECORD_UNITS; /* next path record */
            }
            pathTableCursor = (uint16_t *)((uint8_t *)levelImage +
                                           (levelImage->header).resourceTables.armyAssetPathTableOffset);
            for (remainingPathCount = (levelImage->header).resourceTables.armyAssetPathCount; remainingPathCount != 0;
                remainingPathCount--) {
              WidePath_SetExtensionCode(ASSET_MAGIC_ARM,pathTableCursor);
              resultOrPointer = (void *)FATAL_ERROR_LEVEL_TOO_MANY_RESOURCES;
              if (INGAME_LOADED_RESOURCE_CAPACITY - 1 < g_InGameLoadedResourcePointerCount) goto load_failed;
              loadEntryResult = Package_LoadEntry(pathTableCursor);
              resultOrPointer = loadEntryResult.bufferOrError;
              if (loadEntryResult.failed) goto load_failed;
              *loadedResourcePointerArray = resultOrPointer;
              g_InGameLoadedResourcePointerCount++;
              loadedResourcePointerArray++;
              assetError = ArmyAsset_PrepareRecords(resultOrPointer);
              resultOrPointer = (void *)assetError;
              if (assetError != 0) goto load_failed;
              MoviePlayback_AdvanceScheduledFrameAndTick();
              pathTableCursor =pathTableCursor + LEVEL_ASSET_PATH_RECORD_UNITS; /* next path record */
            }
            resultOrPointer = (void *)FATAL_ERROR_LEVEL_TOO_MANY_RESOURCES;
            if (g_InGameLoadedResourcePointerCount < INGAME_LOADED_RESOURCE_CAPACITY) {
              g_MoviePlaybackBaseFrameGroup = 1;
              g_MoviePlaybackScheduleCounter = 0;
              g_MoviePlaybackScheduleSpan = LEVEL_LOAD_MOVIE_SPAN_HOLD;
              /* stage 1: terrain textures (surface, ground) and the field grid */
              if (!TerrainVisualResources_LoadAndClearCellOverlayFlags
                     ((uint16_t *)((uint8_t *)levelImage +
                                   (levelImage->header).pathState.surfaceTextureBasePathOffset),
                      (uint16_t *)((uint8_t *)levelImage +
                                   (levelImage->header).pathState.groundTextureBasePathOffset),
                      (FieldGridAsset *)(levelImage->header).pathState.levelPathOffsetOrLoadedFieldGrid,
                      &terrainLoadError)) {
                resultOrPointer = (void *)terrainLoadError;
              }
              else {
                assetError = ShotDefinitions_ValidateTerrainMaterialReferences();
                resultOrPointer = (void *)assetError;
                if (assetError == 0) {
                  g_MoviePlaybackBaseFrameGroup = 2;
                  g_MoviePlaybackScheduleCounter = 0;
                  g_MoviePlaybackScheduleSpan = LEVEL_LOAD_MOVIE_SPAN_HOLD;
                  assetError = ModelRuntimePool_Init();
                  resultOrPointer = (void *)assetError;
                  if (assetError == 0) {
                    /* The decompiler lost these two locals; the original reads them from the level header
                       (+0xC0 army and +0xC8 effect texture base paths). */
                    armyTextureBasePath =
                         (uint16_t *)((uint8_t *)levelImage + (levelImage->header).pathState.armyTextureBasePathOffset);
                    effectTextureBasePath =
                         (uint16_t *)((uint8_t *)levelImage +
                                      (levelImage->header).pathState.effectTextureBasePathOffset);
                    armyRuntimeInitialized =
                         ArmyRuntime_InitializePoolAndGraphics(worldRuntime,armyTextureBasePath,&armyInitError);
                    resultOrPointer = (void *)armyInitError;
                    if (armyRuntimeInitialized) {
                      g_MoviePlaybackBaseFrameGroup = 3;
                      g_MoviePlaybackScheduleCounter = 0;
                      g_MoviePlaybackScheduleSpan = 4;
                      shotsInitialized = ShotRuntime_InitGraphicsResources
                                         ((uint16_t *)((uint8_t *)levelImage +
                                                       (levelImage->header).pathState.shotTextureBasePathOffset),
                                          &shotInitError);
                      resultOrPointer = (void *)shotInitError;
                      if (shotsInitialized) {
                        g_MoviePlaybackBaseFrameGroup = 4;
                        g_MoviePlaybackScheduleCounter = 0;
                        g_MoviePlaybackScheduleSpan = 4;
                        effectsInitialized =
                             EffectRuntime_InitGraphicsResources(effectTextureBasePath,&effectInitError);
                        resultOrPointer = (void *)effectInitError;
                        if (effectsInitialized) {
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
                                     (levelImage->worldSettings).terrainRampStepColorArgb,worldRuntime)
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
                                    (2,(int)countOrPackedValue >> 16,countOrPackedValue & 0xffff,
                                     *(UQ12 *)((uint8_t *)&levelImage->playerSlots[0].
                                                     startCameraMagnitudeQ12 + playerSlotByteOffset),worldRuntime);
                          countOrPackedValue = (levelImage->worldSettings).packedFieldRegionOriginYHigh16XLow16;
                          WorldRuntime_RecomputeMotionEndpointAgainstFieldSurface(worldRuntime);
                          packedRegionValue = countOrPackedValue;
                          WorldRuntime_CommitCameraTargetDistance(worldRuntime);
                          countOrPackedValue = countOrPackedValue & 0xffff;
                          factionIndexOrOriginY = (int)packedRegionValue >> 16;
                          packedRegionValue = (levelImage->worldSettings).
                                  packedFieldRegionHeightHigh16WidthLow16;
                          soundLoopWorldRuntime = worldRuntime;
                          MoviePlayback_AdvanceScheduledFrameAndTick();
                          WorldRuntime_RecomputeFieldRegionNormalsAndLighting
                                    ((int)packedRegionValue >> 16,packedRegionValue & 0xffff,factionIndexOrOriginY,
                                     countOrPackedValue,soundLoopWorldRuntime);
                          /* the saved runtime pools, loaded over the freshly initialised ones and rebased */
                          poolLoaded = Package_LoadEntryIntoBuffer
                                             (worldRuntime->objectCount * sizeof(WorldObjectRecord),
                                              (uint8_t *)worldRuntime->objectArray,
                                              (uint16_t *)u_widget_hex_0050e02a,&poolByteCountOrError);
                          resultOrPointer = (void *)poolByteCountOrError;
                          if (poolLoaded) {
                            poolLoaded = Package_LoadEntryIntoBuffer
                                               (ARMY_RUNTIME_SLOT_COUNT * sizeof(ArmyRuntimeSlot),(uint8_t *)g_ArmyRuntimeSlots,
                                                (uint16_t *)u_army_hex_0050dfb4,&poolByteCountOrError);
                            resultOrPointer = (void *)poolByteCountOrError;
                            if (poolLoaded) {
                              poolLoaded = Package_LoadEntryIntoBuffer
                                                 (MODEL_RUNTIME_POOL_BYTES,(uint8_t *)g_ModelRuntimeSlots,
                                                  (uint16_t *)u_modul_hex_0050dfee,&poolByteCountOrError);
                              resultOrPointer = (void *)poolByteCountOrError;
                              if (poolLoaded) {
                                poolLoaded = Package_LoadEntryIntoBuffer
                                                   (EFFECT_RUNTIME_POOL_BYTES,(uint8_t *)g_EffectRuntimeSlots,
                                                    (uint16_t *)u_effect_hex_0050dfc6,&poolByteCountOrError);
                                resultOrPointer = (void *)poolByteCountOrError;
                                if (poolLoaded) {
                                  poolLoaded = Package_LoadEntryIntoBuffer
                                                     (SHOT_RUNTIME_POOL_BYTES,(uint8_t *)g_ShotRuntimeSlots,
                                                      (uint16_t *)u_shot_hex_0050dfdc,&poolByteCountOrError);
                                  resultOrPointer = (void *)poolByteCountOrError;
                                  if (poolLoaded) {
                                    poolLoaded = Package_LoadEntryIntoBuffer
                                                       (sizeof(g_GraphicsShadingRuntimeRecords),(uint8_t *)
                                                  g_GraphicsShadingRuntimeRecords,
                                                  (uint16_t *)u_light_hex_0050e016,&poolByteCountOrError);
                                    resultOrPointer = (void *)poolByteCountOrError;
                                    if (poolLoaded) {
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
                                      assetPath = (uint8_t *)levelImage +
                                                  (levelImage->header).pathState.soundBasePathOffset;
                                      WidePath_SetExtensionCode(ASSET_MAGIC_SAM,(uint16_t *)assetPath);
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
                                        soundsInPackage = Package_FindEntry(outputCapacityBytes,
                                                                   resultOrPointer,(uint16_t *)assetPath,
                                                                   g_SoundPackageHandle,&countOrPackedValue);
                                        soundDirectoryRecordSizeBytes = PCK_ENTRY_HEADER_BYTES;
                                        if (!soundsInPackage) {
                                          enumerationResult = (*g_FileSystemEnumerateDirectoryOrVolumeEntries
                                                   )(FILESYSTEM_ENUMERATE_FILES,UINT32_MAX,
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
                                                g_InGameLevelEffectVoiceSet0 = NULL;
                                                g_InGameLevelEffectVoiceSet1 = NULL;
                                                g_InGameLevelEffectVoiceSet2 = NULL;
                                                g_InGameLevelEffectVoiceSet3 = NULL;
                                                g_InGameActiveEffectVoice = 0;
                                                g_InGameEffectsEnabled = 1;
                                                g_InGameActiveMusicVoice = 0;
                                                g_InGameMusicNextTrackCountdown = 1;
                                                g_InGameLevelMusicVoiceSet0 = NULL;
                                                g_InGameLevelMusicVoiceSet1 = NULL;
                                                g_InGameLevelMusicVoiceSet2 = NULL;
                                                g_InGameLevelMusicVoiceSet3 = NULL;
                                                sampleOrVoiceSet = (SoundSampleAsset *)freeResult.valueOrError;
                                                /* the four level effect and four music samples of the tail */
                                                if ((levelConditionStorage->levelImage).worldSettings.
                                                    effectSampleNumbers[0] != 0) {
                                                  g_WideNumberFormatUtf16
                                                            (WIDE_FORMAT_PAD_WITH_ZERO,0,2,1,
                                                             (levelConditionStorage->levelImage).worldSettings.
                                                             effectSampleNumbers[0],
                                                             (uint16_t *)(u_sound_level00_sam_0050df6c +
                                                                     11));
                                                  sampleLoaded = Resource_Load((uint16_t *)u_sound_level00_sam_0050df6c,(void **)&loadedSampleAsset,NULL,
                                                                                &loadErrorCode);
                                                  /* Original quirk: a failed load leaves its error code as this function's success value
                                                     (unless a later sample overwrites it) */
                                                  sampleOrVoiceSet = sampleLoaded ? loadedSampleAsset : (SoundSampleAsset *)loadErrorCode;
                                                  if (sampleLoaded) {
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
                                                                     11));
                                                  sampleLoaded = Resource_Load((uint16_t *)u_sound_level00_sam_0050df6c,(void **)&loadedSampleAsset,NULL,
                                                                                &loadErrorCode);
                                                  /* Original quirk: a failed load leaves its error code as this function's success value
                                                     (unless a later sample overwrites it) */
                                                  sampleOrVoiceSet = sampleLoaded ? loadedSampleAsset : (SoundSampleAsset *)loadErrorCode;
                                                  if (sampleLoaded) {
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
                                                                     11));
                                                  sampleLoaded = Resource_Load((uint16_t *)u_sound_level00_sam_0050df6c,(void **)&loadedSampleAsset,NULL,
                                                                                &loadErrorCode);
                                                  /* Original quirk: a failed load leaves its error code as this function's success value
                                                     (unless a later sample overwrites it) */
                                                  sampleOrVoiceSet = sampleLoaded ? loadedSampleAsset : (SoundSampleAsset *)loadErrorCode;
                                                  if (sampleLoaded) {
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
                                                                     11));
                                                  sampleLoaded = Resource_Load((uint16_t *)u_sound_level00_sam_0050df6c,(void **)&loadedSampleAsset,NULL,
                                                                                &loadErrorCode);
                                                  /* Original quirk: a failed load leaves its error code as this function's success value
                                                     (unless a later sample overwrites it) */
                                                  sampleOrVoiceSet = sampleLoaded ? loadedSampleAsset : (SoundSampleAsset *)loadErrorCode;
                                                  if (sampleLoaded) {
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
                                                                     11));
                                                  sampleLoaded = Resource_Load((uint16_t *)u_sound_music00_sam_0050df90,(void **)&loadedSampleAsset,NULL,
                                                                                &loadErrorCode);
                                                  /* Original quirk: a failed load leaves its error code as this function's success value
                                                     (unless a later sample overwrites it) */
                                                  sampleOrVoiceSet = sampleLoaded ? loadedSampleAsset : (SoundSampleAsset *)loadErrorCode;
                                                  if (sampleLoaded) {
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
                                                                     11));
                                                  sampleLoaded = Resource_Load((uint16_t *)u_sound_music00_sam_0050df90,(void **)&loadedSampleAsset,NULL,
                                                                                &loadErrorCode);
                                                  /* Original quirk: a failed load leaves its error code as this function's success value
                                                     (unless a later sample overwrites it) */
                                                  sampleOrVoiceSet = sampleLoaded ? loadedSampleAsset : (SoundSampleAsset *)loadErrorCode;
                                                  if (sampleLoaded) {
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
                                                                     11));
                                                  sampleLoaded = Resource_Load((uint16_t *)u_sound_music00_sam_0050df90,(void **)&loadedSampleAsset,NULL,
                                                                                &loadErrorCode);
                                                  /* Original quirk: a failed load leaves its error code as this function's success value
                                                     (unless a later sample overwrites it) */
                                                  sampleOrVoiceSet = sampleLoaded ? loadedSampleAsset : (SoundSampleAsset *)loadErrorCode;
                                                  if (sampleLoaded) {
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
                                                                     11));
                                                  sampleLoaded = Resource_Load((uint16_t *)u_sound_music00_sam_0050df90,(void **)&loadedSampleAsset,NULL,
                                                                                &loadErrorCode);
                                                  /* Original quirk: a failed load leaves its error code as this function's success value
                                                     (unless a later sample overwrites it) */
                                                  sampleOrVoiceSet = sampleLoaded ? loadedSampleAsset : (SoundSampleAsset *)loadErrorCode;
                                                  if (sampleLoaded) {
                                                    voiceSetResult = g_SoundCreateSampleVoiceSet(loadedSampleAsset);
                                                    sampleOrVoiceSet = (SoundSampleAsset *)voiceSetResult.voiceSet;
                                                    if (!voiceSetResult.failed) {
                                                      g_InGameLevelMusicVoiceSet3 = sampleOrVoiceSet;
                                                    }
                                                    Resource_Release(loadedSampleAsset);
                                                  }
                                                }
                                                return true;
                                              }
                                              soundSlotCursor = worldRuntime->dwordArray;
                                              soundLoopWorldRuntime = worldRuntime;
                                              soundSlotIndex = 
                                                  WidePath_ParseTrailingNumberBeforeExtensionRegs
                                                            (soundDirectoryPathCursor);
                                              if (soundSlotIndex < worldRuntime->dwordArrayCount) {
                                                if (soundsInPackage) {
                                                  if (!Resource_Load(soundDirectoryPathCursor,&shrinkResultOrError,NULL,&loadErrorCode)) {
                                                    shrinkResultOrError = (void *)loadErrorCode; /* passed on as this function's error */
                                                    break;
                                                  }
                                                }
                                                else {
                                                  WidePath_CombineDirectoryAndLeaf
                                                            ((uint16_t *)&
                                                  g_InGameLevelSoundLeafOrCombinedPathScratchUtf16,
                                                  soundDirectoryPathCursor,
                                                  (uint16_t *)&
                                                  g_InGameLevelSoundParentDirectoryScratchUtf16);
                                                  if (!Resource_Load((uint16_t *)&
                                                  g_InGameLevelSoundLeafOrCombinedPathScratchUtf16,&shrinkResultOrError,NULL,&loadErrorCode)) {
                                                    shrinkResultOrError = (void *)loadErrorCode; /* passed on as this function's error */
                                                    break;
                                                  }
                                                }
                                                soundSlot = SpatialSoundSlot_CreateFromSampleAsset
                                                                   (shrinkResultOrError);
                                                if (soundSlot != NULL) {
                                                  soundSlotCursor[soundSlotIndex] = (uint32_t)soundSlot;
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
  *outError = (uint32_t)resultOrPointer;
  return false;
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
   field region and the seven camera bookmarks and writes the image back to the same path. Returns true on
   success; on failure returns false with the load or write error in *outError. Called by
   InGameUiCommand_SaveFieldAndLevelAssetImages (ui/ingame/runtime.c); the field grid itself is written separately.
*/

bool InGameLevelRuntime_SaveLevelAssetImageFromWorldState(InGameLevelSaveWorldView *saveWorldView,uint32_t *outError)

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
  bool imageLoaded;

  imageLoaded = Package_LoadEntryIntoBuffer(PACKAGE_SCRATCH_BUFFER_BYTES,g_PackageScratchBuffer,
                                            g_LevelEndingMovieSourcePath,&statusOrFieldValue);
  levelImageBytes = g_PackageScratchBuffer;
  levelImage = (LevelAssetRuntimePrefix *)levelImageBytes;
  if (imageLoaded) {
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
        levelImage->header.common.allocationSizeBytes = levelImage->header.common.allocationSizeBytes + sizeof(LevelInitialArmyPlacementRecord20);
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
        placementRecordCursor->zeroPadding[0] = 0;
        placementRecordCursor->zeroPadding[1] = 0;
        placementRecordCursor->zeroPadding[2] = 0;
        placementRecordCursor->zeroPadding[3] = 0;
        placementRecordCursor->zeroPadding[4] = 0;
        placementRecordCursor->zeroPadding[5] = 0;
        placementRecordCursor->zeroPadding[6] = 0;
        placementRecordCursor->zeroPadding[7] = 0;
        placementRecordCursor->zeroPadding[8] = 0;
        placementRecordCursor->zeroPadding[9] = 0;
        placementRecordCursor->zeroPadding[10] = 0;
        placementRecordCursor->zeroPadding[11] = 0;
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
    statusOrFieldValue = FileSystem_WriteBufferToPath
                      (levelImage->header.common.allocationSizeBytes,levelImageBytes,g_LevelEndingMovieSourcePath);
    if (statusOrFieldValue == 0) {
      return true;
    }
  }
  *outError = statusOrFieldValue;
  return false;
}


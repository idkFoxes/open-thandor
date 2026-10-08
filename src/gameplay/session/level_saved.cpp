/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/src/gameplay/session/level_saved.cpp
 * Reverse engineering by idkFoxes 2026
 */

#include <thandor/gameplay/session/level_saved.h>
#include <thandor/thandor.h>
#include <thandor/assets/record_bytes.h>

/* Loading stages 1 to 5 of a saved game: as NewLevel_InitTerrainAndGraphics, but the terrain loader also clears
   the cell overlay flags, and the army references of the saved faction image are rebased before the terrain
   lighting is set. */
static bool SavedLevel_InitTerrainAndGraphics
          (LevelAssetRuntimePrefix *levelImage,WorldRuntimeContext *worldRuntime,uint32_t *outError)

{
  uint16_t *armyTextureBasePath;
  uint16_t *effectTextureBasePath;
  uint32_t stepError;

  g_MoviePlaybackBaseFrameGroup = 1;
  g_MoviePlaybackScheduleCounter = 0;
  g_MoviePlaybackScheduleSpan = LEVEL_LOAD_MOVIE_SPAN_HOLD;
  if (!TerrainVisualResources_LoadAndClearCellOverlayFlags
         (Asset_RecordAt<uint16_t>(levelImage,(levelImage->header).pathOffsets.surfaceTextureBasePathOffset),
          Asset_RecordAt<uint16_t>(levelImage,(levelImage->header).pathOffsets.groundTextureBasePathOffset),
          Thandor_U32ToPointer<FieldGridAsset>((levelImage->header).pathOffsets.levelPathOffset),&stepError)) { /* 32-bit format field: LevelAsset +0x0B0 levelPathOffset, a FieldGridAsset pointer */
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
       Asset_RecordAt<uint16_t>(levelImage,(levelImage->header).pathOffsets.armyTextureBasePathOffset);
  effectTextureBasePath =
       Asset_RecordAt<uint16_t>(levelImage,(levelImage->header).pathOffsets.effectTextureBasePathOffset);
  if (!ArmyRuntime_InitializePoolAndGraphics(worldRuntime,armyTextureBasePath,&stepError)) {
    return NewLevel_Fail(outError,stepError);
  }
  g_MoviePlaybackBaseFrameGroup = 3;
  g_MoviePlaybackScheduleCounter = 0;
  g_MoviePlaybackScheduleSpan = 4;
  if (!ShotRuntime_InitGraphicsResources
         (Asset_RecordAt<uint16_t>(levelImage,(levelImage->header).pathOffsets.shotTextureBasePathOffset),
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
            (Thandor_U32ToPointer<FieldGridAsset>((levelImage->header).pathOffsets.levelPathOffset),worldRuntime); /* 32-bit format field: LevelAsset +0x0B0 levelPathOffset, a FieldGridAsset pointer */
  MoviePlayback_AdvanceScheduledFrameAndTick();
  startSlot = Asset_RecordAt<LevelPlayerSlotRecord>(&levelImage->playerSlots[0],playerSlotByteOffset);
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

/* Spatial sound slots of a saved game (loading stage 6): as NewLevel_LoadSpatialSounds, with one more
   loading-movie step after the sound directory's extension is set. */
static bool SavedLevel_LoadSpatialSounds
          (LevelAssetRuntimePrefix *levelImage,WorldRuntimeContext *worldRuntime,uint32_t *outError)

{
  uintptr_t *soundSlotCursor;
  WorldWorkspaceElementCount remainingSoundSlotCount;
  uint16_t *soundDirectoryPath;
  void *directoryListing;
  PckOutputCapacityBytes listingCapacityBytes;
  bool soundsInPackage; /* the sounds are listed from g_SoundPackageHandle, not a directory */
  uint32_t listedSoundCount;
  uint32_t soundDirectoryRecordSizeBytes;
  uint32_t allocError;
  uint32_t shrinkError;
  uint16_t *listedSoundPath;
  uint32_t soundIndex;
  bool sampleLoaded;
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
       Asset_RecordAt<uint16_t>(levelImage,(levelImage->header).pathOffsets.soundBasePathOffset);
  WidePath_SetExtensionCode(ASSET_MAGIC_SAM,soundDirectoryPath);
  MoviePlayback_AdvanceScheduledFrameAndTick();
  WidePath_SplitParentAndLeaf
            (g_InGameLevelSoundLeafOrCombinedPathScratchUtf16,
             g_InGameLevelSoundParentDirectoryScratchUtf16,soundDirectoryPath);
  allocError = g_MemoryApi.allocLargestFreeBlock(&directoryListing,&listingCapacityBytes);
  if (allocError != 0) {
    return NewLevel_Fail(outError,allocError);
  }
  soundsInPackage = Package_FindEntry(listingCapacityBytes,static_cast<PckEntryHeader *>(directoryListing),
                                      soundDirectoryPath,g_SoundPackageHandle,&listedSoundCount);
  soundDirectoryRecordSizeBytes = PCK_ENTRY_HEADER_BYTES;
  if (!soundsInPackage) {
    listedSoundCount = g_FileSystemEnumerateDirectoryOrVolumeEntries
                         (FileSystemEnumerationMode::FILESYSTEM_ENUMERATE_FILES,UINT32_MAX,listingCapacityBytes,
                          static_cast<uint8_t *>(directoryListing),
                          reinterpret_cast<uint8_t *>(soundDirectoryPath)); /* the UTF-16 path as bytes */
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
  listedSoundPath = static_cast<uint16_t *>(directoryListing);
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
      soundSlot = SpatialSoundSlot_CreateFromSampleAsset(static_cast<SoundSampleAsset *>(loadedSample));
      if (soundSlot != nullptr) {
        soundSlotCursor[soundIndex] = reinterpret_cast<uintptr_t>(soundSlot); /* kept as an integer slot */
      }
      Resource_Release(loadedSample);
      MoviePlayback_AdvanceScheduledFrameAndTick();
    }
    /* advance by one directory record */
    listedSoundPath = Asset_RecordAt<uint16_t>(listedSoundPath,soundDirectoryRecordSizeBytes);
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

bool InGameLevelRuntime_LoadResourcesAfterExternalTables
          (FrontendLoadedLevelAsset *levelImage,WorldRuntimeContext *worldRuntime,uint32_t *outError)

{
  LevelAssetRuntimePrefix *levelPrefix;
  Ptr32<void> *loadedResourceCursor;
  uint32_t allocError;
  uint32_t stepError;

  /* the same LEV image, viewed through the type the shared NewLevel_ steps take (identical layout) */
  levelPrefix = reinterpret_cast<LevelAssetRuntimePrefix *>(levelImage);
  allocError = g_MemoryApi.alloc(INGAME_LOADED_RESOURCE_CAPACITY * sizeof(Ptr32<void>),
                                 reinterpret_cast<void **>(&loadedResourceCursor));
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
  /* level.hex of a save holds only the runtime prefix (the save writes the condition storage with the header's
     prefix byte size), so the paths must lie inside it */
  if (!NewLevel_ValidateImage(levelPrefix,
                              (levelImage->header).resourceTables.runtimePrefixByteSizeAndInitialArmyPlacementOffset,
                              worldRuntime->activeFactionRuntimeIndex,outError)) {
    return false;
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

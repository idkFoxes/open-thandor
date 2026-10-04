/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/src/gameplay/session/level.cpp
 * Reverse engineering by idkFoxes 2026
 */

#include <thandor/gameplay/session/level.h>
#include <thandor/thandor.h>

/* Module data. */

/* L"field.hex" */
uint16_t g_FieldHexPathUtf16[10] = {'f', 'i', 'e', 'l', 'd', '.', 'h', 'e', 'x', 0};

/* L"level.hex" */
uint16_t g_LevelHexPathUtf16[10] = {'l', 'e', 'v', 'e', 'l', '.', 'h', 'e', 'x', 0};

uint16_t g_LevelEndingMovieSourcePath[256] = {0};

/* LevelPackage_ValidateAndMount's one-entry Package_FindEntry output buffer (PCK_ENTRY_HEADER_BYTES) */
static PckEntryHeader g_LevelPackageFoundEntry = {0};

static uint16_t g_LevelLevPatternUtf16[12] = {'l', 'e', 'v', 'e', 'l', '\\', '*', '.', 'l', 'e', 'v', 0}; /* L"level\\*.lev" */

static uint16_t g_LevelStrPatternUtf16[12] = {'l', 'e', 'v', 'e', 'l', '\\', '*', '.', 's', 't', 'r', 0}; /* L"level\\*.str" */

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

/* Releases everything a level loader set up: the effect, shot, model, army and terrain graphics, the spatial
   sound slots, the level's effect and music voices, every loaded EFF/SHT/MDL/ARM file, the copied level prefix
   and the technology file.
*/

void InGameLevelRuntime_ShutdownLoadedAssetResources(WorldRuntimeContext *worldRuntime)

{
  Ptr32<void> *loadedResourceCursor;
  InGameLoadedResourcePointerCount remainingResourceCount;
  uint32_t remainingSlotCount;
  uintptr_t *soundSlotCursor;
  
  EffectRuntime_ShutdownGraphicsResources();
  ShotRuntime_ShutdownGraphicsResources();
  ModelRuntimePool_ShutdownAndReleaseDefinitions();
  ArmyRuntime_ShutdownPoolAndGraphics();
  TerrainVisualResources_Shutdown();
  remainingSlotCount = worldRuntime->dwordArrayCount;
  soundSlotCursor = worldRuntime->dwordArray;
  /* the original loops only when both the slot count and the slot array are non-zero */
  if (remainingSlotCount != 0 && soundSlotCursor != nullptr) {
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
  if (g_InGameLoadedResourcePointers != nullptr) {
    for (; remainingResourceCount != 0; remainingResourceCount--) {
      Resource_Release(*loadedResourceCursor);
      loadedResourceCursor++;
    }
  }
  g_MemoryApi.free(g_InGameLoadedResourcePointers);
  g_InGameLoadedResourcePointers = nullptr;
  g_InGameLoadedResourcePointerCount = 0;
  g_MemoryApi.free(g_InGameLevelRuntimeGlobalBlock.conditionStorage);
  g_InGameLevelRuntimeGlobalBlock.conditionStorage = nullptr;
  Resource_Release(g_TechnologyAsset);
  g_TechnologyAsset = nullptr;
}

/* Editor save of the current level: reloads the level asset (g_LevelEndingMovieSourcePath) into the package
   scratch buffer, replaces its placement table with one 0x20-byte record per live world model, stores the
   field region and the seven camera bookmarks and writes the image back to the same path. Returns true on
   success; on failure returns false with the load or write error in *outError. Called by
   InGameUiCommand_SaveFieldAndLevelAssetImages (ui/ingame/editor_tools.cpp); the field grid itself is written separately.
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
      ownerListNode != nullptr; ownerListNode = ownerListNode->nextNode) {
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

/* Mounts the level package levelPathUtf16 and checks that it holds a valid level: its level\*.lev must be a
   'lev' asset of converter version 0x70001, and the level\*.str text page must load as the level's text
   aliases (keyed by the level's title text id). Returns false when the package stays mounted; on any failure
   it is unmounted again and true is returned (a failed mount returns without unmounting).
*/
Bool8 LevelPackage_ValidateAndMount(uint16_t *levelPathUtf16)

{
  uint32_t levelTitleTextId;
  EngineFileHandle fileHandle;
  int *levelAsset;
  uint32_t matchCount;

  /* on failure fileHandle holds the error code but is not used */
  if (!Package_Mount(levelPathUtf16,&fileHandle)) {
    return true; /* nothing mounted, nothing to unmount */
  }
  if (Package_FindEntry(PCK_ENTRY_HEADER_BYTES,&g_LevelPackageFoundEntry,
                        (uint16_t *)g_LevelLevPatternUtf16,fileHandle,&matchCount) &&
      matchCount != 0) {
    levelAsset = (int *)Package_LoadEntry(g_LevelPackageFoundEntry.path,nullptr);
    if (levelAsset != nullptr) {
      /* dword 0: asset magic, dword 3: converter version */
      if (*levelAsset == ASSET_MAGIC_LEV && levelAsset[3] == PCK_CONVERTER_LEV_00070001) {
        levelTitleTextId = levelAsset[92]; /* LEV +0x170 */
        Resource_Release(levelAsset);
        if (Package_FindEntry(PCK_ENTRY_HEADER_BYTES,&g_LevelPackageFoundEntry,
                              (uint16_t *)g_LevelStrPatternUtf16,fileHandle,&matchCount) &&
            matchCount != 0 &&
            !TextResourcePage_LoadCompatibilityAliases(levelTitleTextId,g_LevelPackageFoundEntry.path)) {
          return false; /* valid level: the package stays mounted */
        }
      }
      else {
        Resource_Release(levelAsset);
      }
    }
  }
  Package_Unmount(fileHandle);
  return true;
}

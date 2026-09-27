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
   Ownership: gameplay/session/level.
   Purpose: EAX returns the prepared movie path; CF reports failure.
   Cross-module calls: WidePath_SetExtensionCode [core/text/path], Package_SetLastErrorPath
   [assets/package/runtime].
*/
EndingMoviePathEaxCf5 __thandor_eax_cf_preserve_ecx_edx
LevelAsset_PrepareEndingMoviePathCf(uint16_t *currentLevelPath,LevelAssetHeader *asset)

{
  int producerNameLeadChars;
  int remainingDwordCount;
  int32_t movieNumber;
  uint16_t *sourcePathCursor;
  EndingMoviePathEaxCf5 successResult;
  EndingMoviePathEaxCf5 failureResult;
  uint8_t *path;
  
  if (((asset->common).magic == ASSET_MAGIC_LEV) &&
     ((asset->common).converterVersion == PCK_CONVERTER_LEV_00070001)) {
    path = (asset->common).buildMetadata.assetRelativeAddressAnchor28 +
           ((asset->pathOffsets).endingMovieBasePathOffset - 0x28);
    remainingDwordCount = 0x80;
    producerNameLeadChars = *(int *)((AssetProducerSourceNames *)(path + 8))->producerName;
    movieNumber = 0;
    if (producerNameLeadChars == 0xfc0077) {
      movieNumber = 2;
    }
    else if (producerNameLeadChars == 0x690065) {
      movieNumber = 3;
    }
    else if (producerNameLeadChars == 0x61006c) {
      movieNumber = 4;
    }
    WidePath_SetExtensionCode(0x6d6c66,(uint16_t *)path);
    (*g_WideNumberFormatUtf16)
              (WIDE_FORMAT_PAD_WITH_ZERO,0,4,1,movieNumber,(uint16_t *)(u_flm_ende0000_flm_0050df06 + 8));
    sourcePathCursor = g_LevelEndingMovieSourcePath;
    for (; remainingDwordCount != 0; remainingDwordCount = remainingDwordCount + -1) {
      *(uint32_t *)sourcePathCursor = *(uint32_t *)currentLevelPath;
      currentLevelPath = currentLevelPath + 2;
      sourcePathCursor = sourcePathCursor + 2;
    }
    successResult.carry = false;
    successResult.moviePath = (uint16_t *)path;
    return successResult;
  }
  Package_SetLastErrorPath(currentLevelPath);
  failureResult.carry = true;
  failureResult.moviePath = (uint16_t *)0x39;
  return failureResult;
}


/* Address: 0x005311D0.
   Ownership: gameplay/session/level.
   Purpose: Validates and copies the level asset, loads its technology, effect, shot, model, army, terrain,
   graphics, sound, and runtime resources, initializes world and faction state, and reports failures through carry
   and the package error path. This entry is used after GameData_ResetDefaults. THE LEV loader: ARM/MDL/EFF/SHT
   path directories (@0xE0..), tech.tec bind, player-slot page read via the levelAsset[1] alias lens (= file offset
   0x200; L5 — start cameras / start credits / AI class), then the ENTITY WALK (L139624-139648): base from header
   @0xDC, stride +0x20, +0x00 type -> armyAssetId, +0x04 owner (spawn gated on factionLifecycleStates[owner] == 1),
   +0x08 -> worldY / +0x0C -> worldX (the millimap XZ swap),...
   Cross-module calls: Package_SetLastErrorPath [assets/package/runtime], WidePath_SetExtensionCode
   [core/text/path], Package_LoadEntry [assets/package/runtime], EffectAsset_PrepareEntries
   [assets/effect/catalog], MoviePlayback_AdvanceScheduledFrameAndTick [movie/runtime/playback],
   ShotAsset_PrepareEntries [assets/shot/catalog].
*/

InGameLevelDefaultLoadEaxCf5 __thandor_eax_cf_preserve_ecx_edx
InGameLevelRuntime_LoadResourcesAfterDefaultResetCf
          (LevelAssetRuntimeImagePrefix370 *levelImage,WorldRuntimeContext *worldRuntime)

{
  LevelPlayerSlotByteOffset32 playerSlotByteOffset;
  InGameLevelConditionStorageView800 *levelConditionStorage;
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
  uint16_t *graphicsBasePath;
  uint16_t *mutableBasePath;
  int counterOrClassId;
  uint32_t flagsOrRelationMask;
  ArmyAssetRecordPrefix **armyRegistryCursor;
  uint32_t relationFactionIndex;
  uint32_t currentSourceIndex;
  uint32_t targetBitOrSavedIndex;
  LevelAssetRuntimeImagePrefix370 *copySourceCursor;
  uint8_t *assetPath;
  uint16_t *pathOrPlacementCursor;
  uint32_t *soundSlotCursor;
  WorldOwnerListNode100 *ownerListNode;
  uint32_t regionOriginOrRelationMask;
  ArenaAllocEaxCf5 allocResult;
  PackageLoadEntryEaxCf5 loadEntryResult;
  StatusValueEaxCf5 statusResult;
  ArmyRuntimeInitEaxCf5 armyInitResult;
  ArmyRuntimeCreateEaxCf5 armyCreateResult;
  ArenaShrinkEaxCf5 shrinkResult;
  SpatialSoundSlotEaxCf5 soundSlotResult;
  SoundCreateSampleVoiceSetEaxCf5 voiceSetResult;
  ModelDefinitionLookupEaxCf5 modelLookupResult;
  InGameLevelDefaultLoadEaxCf5 successResult;
  InGameLevelDefaultLoadEaxCf5 failureResult;
  ArenaLargestAllocationEaxEcxCf9 largestBlockResult;
  PackageFindEntryEaxEcxCf9 findEntryResult;
  FileSystemEnumerationEaxEcxCf9 enumerationResult;
  ResourceLoadEaxEcxCf9 resourceLoadResult;
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
  
  allocResult = (*g_MemoryApi.alloc)(0x800);
  loadedResourcePointerArray = (void **)allocResult.eax;
  resultOrPointer = loadedResourcePointerArray;
  if (!allocResult.carry) {
    g_InGameLoadedResourcePointerCount = 0;
    resultOrPointer = (void *)0x39;
    g_InGameLoadedResourcePointers = loadedResourcePointerArray;
    Package_SetLastErrorPath(g_LevelEndingMovieSourcePath);
    if (((levelImage->header).common.magic == ASSET_MAGIC_LEV) &&
       ((levelImage->header).common.converterVersion == PCK_CONVERTER_LEV_00070001)) {
      countOrPackedValue = (levelImage->header).resourceTables.runtimePrefixByteSizeAndInitialArmyPlacementOffset
      ;
      allocResult = (*g_MemoryApi.alloc)(countOrPackedValue);
      resultOrPointer = (void *)allocResult.eax;
      if (!allocResult.carry) {
        copySourceCursor = levelImage;
        g_InGameLevelRuntimeGlobalBlock.conditionStorage = resultOrPointer;
        for (countOrPackedValue = countOrPackedValue >> 2; countOrPackedValue != 0; countOrPackedValue = countOrPackedValue - 1) {
          (((InGameLevelConditionStorageView800 *)resultOrPointer)->levelImage).header.common.magic
               = (copySourceCursor->header).common.magic;
          copySourceCursor = (LevelAssetRuntimeImagePrefix370 *)&(copySourceCursor->header).common.allocationSizeBytes
          ;
          resultOrPointer =
               &(((InGameLevelConditionStorageView800 *)resultOrPointer)->levelImage).header.common.
                allocationSizeBytes;
        }
        g_InGameLevelTitleTextResourceIndex = (levelImage->header).titleTextResourceIndex;
        g_InGameLevelCampaignAssociationIndex = (levelImage->header).campaignAssociationIndex;
        assetPath = (levelImage->header).common.buildMetadata.assetRelativeAddressAnchor28 +
                  ((levelImage->header).pathOffsets.technologyPathOffset - 0x28);
        WidePath_SetExtensionCode(0x636574,(uint16_t *)assetPath);
        loadEntryResult = Package_LoadEntry((uint16_t *)assetPath);
        loadedTechnologyAsset = loadEntryResult.bufferOrError;
        resultOrPointer = loadedTechnologyAsset;
        if (((!loadEntryResult.carry) &&
            (resultOrPointer = &k_LowAddressLiteral0000004F,
            g_TechnologyAsset = loadedTechnologyAsset,
            (loadedTechnologyAsset->header).common.magic == ASSET_MAGIC_TEC)) &&
           ((loadedTechnologyAsset->header).common.converterVersion == PCK_CONVERTER_TEC_00020000))
        {
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
               (levelImage->runtimeTail2E0).activeFactionCount;
          g_GameFactionRuntimeImage.records[4].factionClassOrMode =
               levelImage->playerSlots[3].aiClassOrMode + 4;
          g_GameFactionRuntimeImage.records[5].factionClassOrMode =
               levelImage->playerSlots[4].aiClassOrMode + 5;
          g_GameFactionRuntimeImage.records[6].factionClassOrMode =
               levelImage->playerSlots[5].aiClassOrMode + 6;
          g_GameFactionRuntimeImage.records[7].factionClassOrMode =
               levelImage->playerSlots[6].aiClassOrMode + 7;
          g_MoviePlaybackScheduleSpan =
               (levelImage->header).resourceTables.effectAssetPathCount +
               (levelImage->header).resourceTables.shotAssetPathCount +
               (levelImage->header).resourceTables.modelAssetPathCount +
               (levelImage->header).resourceTables.armyAssetPathCount;
          g_MoviePlaybackBaseFrameGroup = 0;
          g_MoviePlaybackScheduleCounter = 0;
          pathOrPlacementCursor = (uint16_t *)((levelImage->header).common.buildMetadata.assetRelativeAddressAnchor28
                            + ((levelImage->header).resourceTables.effectAssetPathTableOffset - 0x28
                              ));
          for (remainingRecordCount = (levelImage->header).resourceTables.effectAssetPathCount; remainingRecordCount != 0;
              remainingRecordCount = remainingRecordCount - 1) {
            WidePath_SetExtensionCode(0x666665,pathOrPlacementCursor);
            resultOrPointer = (InGameLevelConditionStorageView800 *)0x3a;
            if (0x1ff < g_InGameLoadedResourcePointerCount)
            goto 
            InGameLevelRuntime_LoadResourcesAfterDefaultResetCf_ReturnCurrentResourceLoadFailureStatus
            ;
            loadEntryResult = Package_LoadEntry(pathOrPlacementCursor);
            resultOrPointer = loadEntryResult.bufferOrError;
            if (loadEntryResult.carry)
            goto 
            InGameLevelRuntime_LoadResourcesAfterDefaultResetCf_ReturnCurrentResourceLoadFailureStatus
            ;
            *loadedResourcePointerArray = resultOrPointer;
            g_InGameLoadedResourcePointerCount = g_InGameLoadedResourcePointerCount + 1;
            loadedResourcePointerArray = loadedResourcePointerArray + 1;
            statusResult = EffectAsset_PrepareEntries(resultOrPointer);
            resultOrPointer = (void *)statusResult.valueOrError;
            if (statusResult.carry)
            goto 
            InGameLevelRuntime_LoadResourcesAfterDefaultResetCf_ReturnCurrentResourceLoadFailureStatus
            ;
            MoviePlayback_AdvanceScheduledFrameAndTick();
            pathOrPlacementCursor = pathOrPlacementCursor + 0x20;
          }
          pathOrPlacementCursor = (uint16_t *)((levelImage->header).common.buildMetadata.assetRelativeAddressAnchor28
                            + ((levelImage->header).resourceTables.shotAssetPathTableOffset - 0x28))
          ;
          for (remainingRecordCount = (levelImage->header).resourceTables.shotAssetPathCount; remainingRecordCount != 0;
              remainingRecordCount = remainingRecordCount - 1) {
            WidePath_SetExtensionCode(0x746873,pathOrPlacementCursor);
            resultOrPointer = (InGameLevelConditionStorageView800 *)0x3a;
            if (0x1ff < g_InGameLoadedResourcePointerCount)
            goto 
            InGameLevelRuntime_LoadResourcesAfterDefaultResetCf_ReturnCurrentResourceLoadFailureStatus
            ;
            loadEntryResult = Package_LoadEntry(pathOrPlacementCursor);
            resultOrPointer = loadEntryResult.bufferOrError;
            if (loadEntryResult.carry)
            goto 
            InGameLevelRuntime_LoadResourcesAfterDefaultResetCf_ReturnCurrentResourceLoadFailureStatus
            ;
            *loadedResourcePointerArray = resultOrPointer;
            g_InGameLoadedResourcePointerCount = g_InGameLoadedResourcePointerCount + 1;
            loadedResourcePointerArray = loadedResourcePointerArray + 1;
            statusResult = ShotAsset_PrepareEntries(resultOrPointer);
            resultOrPointer = (void *)statusResult.valueOrError;
            if (statusResult.carry)
            goto 
            InGameLevelRuntime_LoadResourcesAfterDefaultResetCf_ReturnCurrentResourceLoadFailureStatus
            ;
            MoviePlayback_AdvanceScheduledFrameAndTick();
            pathOrPlacementCursor = pathOrPlacementCursor + 0x20;
          }
          statusResult = EffectDefinitions_ResolveCrossReferences();
          resultOrPointer = (void *)statusResult.valueOrError;
          if (!statusResult.carry) {
            pathOrPlacementCursor = (uint16_t *)((levelImage->header).common.buildMetadata.
                               assetRelativeAddressAnchor28 +
                              ((levelImage->header).resourceTables.modelAssetPathTableOffset - 0x28)
                              );
            for (remainingRecordCount = (levelImage->header).resourceTables.modelAssetPathCount; remainingRecordCount != 0;
                remainingRecordCount = remainingRecordCount - 1) {
              WidePath_SetExtensionCode(0x6c646d,pathOrPlacementCursor);
              resultOrPointer = (InGameLevelConditionStorageView800 *)0x3a;
              if (0x1ff < g_InGameLoadedResourcePointerCount)
              goto 
              InGameLevelRuntime_LoadResourcesAfterDefaultResetCf_ReturnCurrentResourceLoadFailureStatus
              ;
              loadEntryResult = Package_LoadEntry(pathOrPlacementCursor);
              resultOrPointer = loadEntryResult.bufferOrError;
              if (loadEntryResult.carry)
              goto 
              InGameLevelRuntime_LoadResourcesAfterDefaultResetCf_ReturnCurrentResourceLoadFailureStatus
              ;
              *loadedResourcePointerArray = resultOrPointer;
              g_InGameLoadedResourcePointerCount = g_InGameLoadedResourcePointerCount + 1;
              loadedResourcePointerArray = loadedResourcePointerArray + 1;
              statusResult = ModelAsset_PrepareRecords(resultOrPointer);
              resultOrPointer = (void *)statusResult.valueOrError;
              if (statusResult.carry)
              goto 
              InGameLevelRuntime_LoadResourcesAfterDefaultResetCf_ReturnCurrentResourceLoadFailureStatus
              ;
              MoviePlayback_AdvanceScheduledFrameAndTick();
              pathOrPlacementCursor = pathOrPlacementCursor + 0x20;
            }
            pathOrPlacementCursor = (uint16_t *)((levelImage->header).common.buildMetadata.
                               assetRelativeAddressAnchor28 +
                              ((levelImage->header).resourceTables.armyAssetPathTableOffset - 0x28))
            ;
            for (remainingRecordCount = (levelImage->header).resourceTables.armyAssetPathCount; remainingRecordCount != 0;
                remainingRecordCount = remainingRecordCount - 1) {
              WidePath_SetExtensionCode(0x6d7261,pathOrPlacementCursor);
              resultOrPointer = (InGameLevelConditionStorageView800 *)0x3a;
              if (0x1ff < g_InGameLoadedResourcePointerCount)
              goto 
              InGameLevelRuntime_LoadResourcesAfterDefaultResetCf_ReturnCurrentResourceLoadFailureStatus
              ;
              loadEntryResult = Package_LoadEntry(pathOrPlacementCursor);
              resultOrPointer = loadEntryResult.bufferOrError;
              if (loadEntryResult.carry)
              goto 
              InGameLevelRuntime_LoadResourcesAfterDefaultResetCf_ReturnCurrentResourceLoadFailureStatus
              ;
              *loadedResourcePointerArray = resultOrPointer;
              g_InGameLoadedResourcePointerCount = g_InGameLoadedResourcePointerCount + 1;
              loadedResourcePointerArray = loadedResourcePointerArray + 1;
              statusResult = ArmyAsset_PrepareRecords(resultOrPointer);
              resultOrPointer = (void *)statusResult.valueOrError;
              if (statusResult.carry)
              goto 
              InGameLevelRuntime_LoadResourcesAfterDefaultResetCf_ReturnCurrentResourceLoadFailureStatus
              ;
              MoviePlayback_AdvanceScheduledFrameAndTick();
              pathOrPlacementCursor = pathOrPlacementCursor + 0x20;
            }
            resultOrPointer = (InGameLevelConditionStorageView800 *)0x3a;
            if (g_InGameLoadedResourcePointerCount < 0x200) {
              g_MoviePlaybackBaseFrameGroup = 1;
              g_MoviePlaybackScheduleCounter = 0;
              g_MoviePlaybackScheduleSpan = 0x10000;
              statusResult = TerrainVisualResources_LoadPrimary
                                 ((uint16_t *)((levelImage->header).common.buildMetadata.
                                           assetRelativeAddressAnchor28 +
                                          ((levelImage->header).pathOffsets.
                                           surfaceTextureBasePathOffset - 0x28)),
                                  (uint16_t *)((levelImage->header).common.buildMetadata.
                                           assetRelativeAddressAnchor28 +
                                          ((levelImage->header).pathOffsets.
                                           groundTextureBasePathOffset - 0x28)),
                                  (FieldGridAsset *)(levelImage->header).pathOffsets.levelPathOffset
                                 );
              resultOrPointer = (void *)statusResult.valueOrError;
              if (!statusResult.carry) {
                statusResult = ShotDefinitions_ValidateTerrainMaterialReferences();
                resultOrPointer = (void *)statusResult.valueOrError;
                if (!statusResult.carry) {
                  g_MoviePlaybackBaseFrameGroup = 2;
                  g_MoviePlaybackScheduleCounter = 0;
                  g_MoviePlaybackScheduleSpan = 0x10000;
                  statusResult = ModelRuntimePool_Init();
                  resultOrPointer = (void *)statusResult.valueOrError;
                  if (!statusResult.carry) {
                    /* The decompiler lost these two locals; the original reads them from the level header
                       (+0xC0 army and +0xC8 effect texture base paths). */
                    graphicsBasePath = (uint16_t *)((levelImage->header).common.buildMetadata.assetRelativeAddressAnchor28 +
                                      ((levelImage->header).pathOffsets.armyTextureBasePathOffset - 0x28));
                    mutableBasePath = (uint16_t *)((levelImage->header).common.buildMetadata.assetRelativeAddressAnchor28 +
                                      ((levelImage->header).pathOffsets.effectTextureBasePathOffset - 0x28));
                    armyInitResult = ArmyRuntime_InitializePoolAndGraphicsCf(worldRuntime,graphicsBasePath);
                    resultOrPointer = (void *)armyInitResult.errorOrValue;
                    if (!armyInitResult.carry) {
                      g_MoviePlaybackBaseFrameGroup = 3;
                      g_MoviePlaybackScheduleCounter = 0;
                      g_MoviePlaybackScheduleSpan = 4;
                      statusResult = ShotRuntime_InitGraphicsResources
                                         ((uint16_t *)((levelImage->header).common.buildMetadata.
                                                   assetRelativeAddressAnchor28 +
                                                  ((levelImage->header).pathOffsets.
                                                   shotTextureBasePathOffset - 0x28)));
                      resultOrPointer = (void *)statusResult.valueOrError;
                      if (!statusResult.carry) {
                        g_MoviePlaybackBaseFrameGroup = 4;
                        g_MoviePlaybackScheduleCounter = 0;
                        g_MoviePlaybackScheduleSpan = 4;
                        statusResult = EffectRuntime_InitGraphicsResources(mutableBasePath);
                        resultOrPointer = (void *)statusResult.valueOrError;
                        if (!statusResult.carry) {
                          g_MoviePlaybackBaseFrameGroup = 5;
                          g_MoviePlaybackScheduleCounter = 0;
                          g_MoviePlaybackScheduleSpan = 6;
                          WorldRuntime_SetTerrainLightingConfiguration
                                    ((levelImage->runtimeTail2E0).terrainLightingColor13CArgb,
                                     (levelImage->runtimeTail2E0).terrainLightingColor138Argb,
                                     (levelImage->runtimeTail2E0).terrainLightingColor134Argb,
                                     (levelImage->runtimeTail2E0).terrainLightingColor130Argb,
                                     (levelImage->runtimeTail2E0).terrainRampColor12CArgb,
                                     (levelImage->runtimeTail2E0).terrainLightingColor128Argb,
                                     (levelImage->runtimeTail2E0).terrainRampColor124Argb,
                                     (levelImage->runtimeTail2E0).terrainBaseColorArgb,worldRuntime)
                          ;
                          counterOrClassId = worldRuntime->activeFactionRuntimeIndex;
                          MoviePlayback_AdvanceScheduledFrameAndTick();
                          playerSlotByteOffset = g_InGameLevelRuntimeGlobalBlock.playerSlotByteOffsets[counterOrClassId + -1];
                          WorldRuntime_AttachFieldGridAsset
                                    ((FieldGridAsset *)
                                     (levelImage->header).pathOffsets.levelPathOffset,worldRuntime);
                          MoviePlayback_AdvanceScheduledFrameAndTick();
                          countOrPackedValue = *(uint32_t *)((int)&levelImage->playerSlots[0].
                                                  packedHeadingLow16PitchHigh16 + playerSlotByteOffset);
                          WorldRuntime_SetPosition60AndDistanceFromPosition80
                                    (*(Q12 *)((int)&levelImage->playerSlots[0].startCameraZQ12 +
                                             playerSlotByteOffset),
                                     *(Q12 *)((int)&levelImage->playerSlots[0].startCameraYQ12 +
                                             playerSlotByteOffset),
                                     *(Q12 *)((int)&levelImage->playerSlots[0].startCameraXQ12 +
                                             playerSlotByteOffset),worldRuntime);
                          WorldRuntime_SetMotionParameters6CThrough78Clamped
                                    (2,(int)countOrPackedValue >> 0x10,countOrPackedValue & 0xffff,
                                     *(UQ12 *)((int)&levelImage->playerSlots[0].
                                                     startCameraMagnitudeQ12 + playerSlotByteOffset),worldRuntime);
                          countOrPackedValue = (levelImage->runtimeTail2E0).packedFieldRegionOriginYHigh16XLow16;
                          WorldRuntime_RecomputeMotionEndpointAgainstFieldSurface(worldRuntime);
                          regionOriginOrRelationMask = countOrPackedValue;
                          WorldRuntime_CommitScalar7CFrom8C(worldRuntime);
                          flagsOrRelationMask = (levelImage->runtimeTail2E0).
                                   packedFieldRegionHeightHigh16WidthLow16;
                          WorldRuntime_RecomputeFieldRegionNormalsAndLighting
                                    ((int)flagsOrRelationMask >> 0x10,flagsOrRelationMask & 0xffff,(int)regionOriginOrRelationMask >> 0x10,
                                     countOrPackedValue & 0xffff,worldRuntime);
                          remainingRecordCount = (levelImage->header).initialArmyPlacementRecordCount;
                          pathOrPlacementCursor = (uint16_t *)((levelImage->header).common.buildMetadata.
                                             assetRelativeAddressAnchor28 +
                                            ((levelImage->header).resourceTables.
                                             runtimePrefixByteSizeAndInitialArmyPlacementOffset -
                                            0x28));
                          MoviePlayback_AdvanceScheduledFrameAndTick();
                          for (; remainingRecordCount != 0; remainingRecordCount = remainingRecordCount - 1) {
                            if (g_GameFactionRuntimeImage.tail.factionLifecycleStates
                                [*(PckArmyAssetIdCatalog *)(pathOrPlacementCursor + 2)] ==
                                FACTION_RUNTIME_LIFECYCLE_ACTIVE) {
                              armyCreateResult = ArmyRuntime_CreateInstanceFromAssetCf
                                                 (6,*(PckArmyAssetIdCatalog *)(pathOrPlacementCursor + 8),
                                                  *(PckArmyAssetIdCatalog *)(pathOrPlacementCursor + 6),
                                                  *(Q12 *)((AssetProducerSourceNames *)(pathOrPlacementCursor + 4)
                                                          )->producerName,
                                                  *(PckArmyAssetIdCatalog *)(pathOrPlacementCursor + 2),
                                                  *(PckArmyAssetIdCatalog *)pathOrPlacementCursor,worldRuntime);
                              resultOrPointer = (void *)armyCreateResult.eax;
                              if (armyCreateResult.carry)
                              goto 
                              InGameLevelRuntime_LoadResourcesAfterDefaultResetCf_ReturnCurrentResourceLoadFailureStatus
                              ;
                            }
                            pathOrPlacementCursor = pathOrPlacementCursor + 0x10;
                          }
                          MoviePlayback_AdvanceScheduledFrameAndTick();
                          WorldRuntime_ForEachNodeInOwnerListD8
                                    (worldRuntime,
                                     ArmyRuntimeNode_AccumulateTerrainOcclusionAndOccupancyCallback,
                                     worldRuntime);
                          WorldRuntime_ForEachNodeInOwnerListD8
                                    (worldRuntime,
                                     ArmyRuntimeNode_RebuildTerrainOccupancyAndVisualStateCallback,
                                     worldRuntime);
                          FieldGrid_ClassifyCellFlagsToRuntimeByte
                                    (worldRuntime->activeFactionRuntimeIndex,worldRuntime->fieldGrid
                                    );
                          MoviePlayback_AdvanceScheduledFrameAndTick();
                          soundSlotCursor = worldRuntime->dwordArray;
                          for (remainingSoundSlotCount = worldRuntime->dwordArrayCount; remainingSoundSlotCount != 0; remainingSoundSlotCount = remainingSoundSlotCount - 1)
                          {
                            *soundSlotCursor = 0;
                            soundSlotCursor = soundSlotCursor + 1;
                          }
                          assetPath = (levelImage->header).common.buildMetadata.
                                    assetRelativeAddressAnchor28 +
                                    ((levelImage->header).pathOffsets.soundBasePathOffset - 0x28);
                          WidePath_SetExtensionCode(0x6d6173,(uint16_t *)assetPath);
                          WidePath_SplitParentAndLeaf
                                    ((uint16_t *)&g_InGameLevelSoundLeafOrCombinedPathScratchUtf16,
                                     (uint16_t *)&g_InGameLevelSoundParentDirectoryScratchUtf16,
                                     (uint16_t *)assetPath);
                          largestBlockResult = (*g_MemoryApi.allocLargestFreeBlock)();
                          outputCapacityBytes = largestBlockResult.blockSizeOrSentinel;
                          resultOrPointer = (void *)largestBlockResult.allocationOrError;
                          if (!largestBlockResult.carry) {
                            findEntryResult = Package_FindEntry(outputCapacityBytes,resultOrPointer,
                                                       (uint16_t *)assetPath,g_SoundPackageHandle);
                            countOrPackedValue = findEntryResult.matchCount;
                            soundDirectoryRecordSizeBytes = findEntryResult.recordSizeOrError;
                            if (findEntryResult.carry) {
                              enumerationResult = (*g_FileSystemEnumerateDirectoryOrVolumeEntriesCf)
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
                              shrinkResult = (*g_MemoryApi.shrinkInPlace)
                                                 (soundDirectoryRecordSizeBytes * countOrPackedValue,
                                                  resultOrPointer);
                              shrinkResultOrError = (void *)shrinkResult.scratchOrError;
                              if (!shrinkResult.carry) {
                                soundLoopWorldRuntime = worldRuntime;
                                soundDirectoryPathCursor = resultOrPointer;
                                levelConditionStorage = g_InGameLevelRuntimeGlobalBlock.conditionStorage;
                                if (worldRuntime->dwordArrayCount < countOrPackedValue) {
                                  countOrPackedValue = worldRuntime->dwordArrayCount;
                                }
                                do {
                                  g_InGameLevelRuntimeGlobalBlock.conditionStorage = levelConditionStorage;
                                  if (countOrPackedValue == 0) {
                                    (*g_MemoryApi.free)(resultOrPointer);
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
                                    if ((levelConditionStorage->levelImage).runtimeTail2E0.effectSampleNumbers[0]
                                        != 0) {
                                      (*g_WideNumberFormatUtf16)
                                                (WIDE_FORMAT_PAD_WITH_ZERO,0,2,1,
                                                 (levelConditionStorage->levelImage).runtimeTail2E0.
                                                 effectSampleNumbers[0],
                                                 (uint16_t *)(u_sound_level00_sam_0050df6c + 0xb));
                                      resourceLoadResult = Resource_Load((uint16_t *)u_sound_level00_sam_0050df6c);
                                      if (!resourceLoadResult.carry) {
                                        voiceSetResult = (*g_SoundCreateSampleVoiceSet)
                                                           ((SoundSampleAsset *)resourceLoadResult.eax);
                                        if (!voiceSetResult.carry) {
                                          g_InGameLevelEffectVoiceSet0 = voiceSetResult.eax;
                                        }
                                        Resource_Release((SoundSampleAsset *)resourceLoadResult.eax);
                                      }
                                    }
                                    if ((levelConditionStorage->levelImage).runtimeTail2E0.effectSampleNumbers[1]
                                        != 0) {
                                      (*g_WideNumberFormatUtf16)
                                                (WIDE_FORMAT_PAD_WITH_ZERO,0,2,1,
                                                 (levelConditionStorage->levelImage).runtimeTail2E0.
                                                 effectSampleNumbers[1],
                                                 (uint16_t *)(u_sound_level00_sam_0050df6c + 0xb));
                                      resourceLoadResult = Resource_Load((uint16_t *)u_sound_level00_sam_0050df6c);
                                      if (!resourceLoadResult.carry) {
                                        voiceSetResult = (*g_SoundCreateSampleVoiceSet)
                                                           ((SoundSampleAsset *)resourceLoadResult.eax);
                                        if (!voiceSetResult.carry) {
                                          g_InGameLevelEffectVoiceSet1 = voiceSetResult.eax;
                                        }
                                        Resource_Release((SoundSampleAsset *)resourceLoadResult.eax);
                                      }
                                    }
                                    if ((levelConditionStorage->levelImage).runtimeTail2E0.effectSampleNumbers[2]
                                        != 0) {
                                      (*g_WideNumberFormatUtf16)
                                                (WIDE_FORMAT_PAD_WITH_ZERO,0,2,1,
                                                 (levelConditionStorage->levelImage).runtimeTail2E0.
                                                 effectSampleNumbers[2],
                                                 (uint16_t *)(u_sound_level00_sam_0050df6c + 0xb));
                                      resourceLoadResult = Resource_Load((uint16_t *)u_sound_level00_sam_0050df6c);
                                      if (!resourceLoadResult.carry) {
                                        voiceSetResult = (*g_SoundCreateSampleVoiceSet)
                                                           ((SoundSampleAsset *)resourceLoadResult.eax);
                                        if (!voiceSetResult.carry) {
                                          g_InGameLevelEffectVoiceSet2 = voiceSetResult.eax;
                                        }
                                        Resource_Release((SoundSampleAsset *)resourceLoadResult.eax);
                                      }
                                    }
                                    if ((levelConditionStorage->levelImage).runtimeTail2E0.effectSampleNumbers[3]
                                        != 0) {
                                      (*g_WideNumberFormatUtf16)
                                                (WIDE_FORMAT_PAD_WITH_ZERO,0,2,1,
                                                 (levelConditionStorage->levelImage).runtimeTail2E0.
                                                 effectSampleNumbers[3],
                                                 (uint16_t *)(u_sound_level00_sam_0050df6c + 0xb));
                                      resourceLoadResult = Resource_Load((uint16_t *)u_sound_level00_sam_0050df6c);
                                      if (!resourceLoadResult.carry) {
                                        voiceSetResult = (*g_SoundCreateSampleVoiceSet)
                                                           ((SoundSampleAsset *)resourceLoadResult.eax);
                                        if (!voiceSetResult.carry) {
                                          g_InGameLevelEffectVoiceSet3 = voiceSetResult.eax;
                                        }
                                        Resource_Release((SoundSampleAsset *)resourceLoadResult.eax);
                                      }
                                    }
                                    if ((levelConditionStorage->levelImage).runtimeTail2E0.musicSampleNumbers[0] !=
                                        0) {
                                      (*g_WideNumberFormatUtf16)
                                                (WIDE_FORMAT_PAD_WITH_ZERO,0,2,1,
                                                 (levelConditionStorage->levelImage).runtimeTail2E0.
                                                 musicSampleNumbers[0],
                                                 (uint16_t *)(u_sound_music00_sam_0050df90 + 0xb));
                                      resourceLoadResult = Resource_Load((uint16_t *)u_sound_music00_sam_0050df90);
                                      if (!resourceLoadResult.carry) {
                                        voiceSetResult = (*g_SoundCreateSampleVoiceSet)
                                                           ((SoundSampleAsset *)resourceLoadResult.eax);
                                        if (!voiceSetResult.carry) {
                                          g_InGameLevelMusicVoiceSet0 = voiceSetResult.eax;
                                        }
                                        Resource_Release((SoundSampleAsset *)resourceLoadResult.eax);
                                      }
                                    }
                                    if ((levelConditionStorage->levelImage).runtimeTail2E0.musicSampleNumbers[1] !=
                                        0) {
                                      (*g_WideNumberFormatUtf16)
                                                (WIDE_FORMAT_PAD_WITH_ZERO,0,2,1,
                                                 (levelConditionStorage->levelImage).runtimeTail2E0.
                                                 musicSampleNumbers[1],
                                                 (uint16_t *)(u_sound_music00_sam_0050df90 + 0xb));
                                      resourceLoadResult = Resource_Load((uint16_t *)u_sound_music00_sam_0050df90);
                                      if (!resourceLoadResult.carry) {
                                        voiceSetResult = (*g_SoundCreateSampleVoiceSet)
                                                           ((SoundSampleAsset *)resourceLoadResult.eax);
                                        if (!voiceSetResult.carry) {
                                          g_InGameLevelMusicVoiceSet1 = voiceSetResult.eax;
                                        }
                                        Resource_Release((SoundSampleAsset *)resourceLoadResult.eax);
                                      }
                                    }
                                    if ((levelConditionStorage->levelImage).runtimeTail2E0.musicSampleNumbers[2] !=
                                        0) {
                                      (*g_WideNumberFormatUtf16)
                                                (WIDE_FORMAT_PAD_WITH_ZERO,0,2,1,
                                                 (levelConditionStorage->levelImage).runtimeTail2E0.
                                                 musicSampleNumbers[2],
                                                 (uint16_t *)(u_sound_music00_sam_0050df90 + 0xb));
                                      resourceLoadResult = Resource_Load((uint16_t *)u_sound_music00_sam_0050df90);
                                      if (!resourceLoadResult.carry) {
                                        voiceSetResult = (*g_SoundCreateSampleVoiceSet)
                                                           ((SoundSampleAsset *)resourceLoadResult.eax);
                                        if (!voiceSetResult.carry) {
                                          g_InGameLevelMusicVoiceSet2 = voiceSetResult.eax;
                                        }
                                        Resource_Release((SoundSampleAsset *)resourceLoadResult.eax);
                                      }
                                    }
                                    if ((levelConditionStorage->levelImage).runtimeTail2E0.musicSampleNumbers[3] !=
                                        0) {
                                      (*g_WideNumberFormatUtf16)
                                                (WIDE_FORMAT_PAD_WITH_ZERO,0,2,1,
                                                 (levelConditionStorage->levelImage).runtimeTail2E0.
                                                 musicSampleNumbers[3],
                                                 (uint16_t *)(u_sound_music00_sam_0050df90 + 0xb));
                                      resourceLoadResult = Resource_Load((uint16_t *)u_sound_music00_sam_0050df90);
                                      if (!resourceLoadResult.carry) {
                                        voiceSetResult = (*g_SoundCreateSampleVoiceSet)
                                                           ((SoundSampleAsset *)resourceLoadResult.eax);
                                        if (!voiceSetResult.carry) {
                                          g_InGameLevelMusicVoiceSet3 = voiceSetResult.eax;
                                        }
                                        Resource_Release((SoundSampleAsset *)resourceLoadResult.eax);
                                      }
                                    }
                                    armyRegistryCursor = g_ArmyAssetRecordRegistry;
                                    counterOrClassId = 0x300;
                                    class0BArmyDefinition = (ArmyAssetRecordPrefix *)0x0;
                                    class0ENoExtraArmyDefinition = (ArmyAssetRecordPrefix *)0x0;
                                    class0EArmyDefinition = (ArmyAssetRecordPrefix *)0x0; /* uninitialized in the original */
                                    class10ArmyDefinition = (ArmyAssetRecordPrefix *)0x0;
                                    do {
                                      registryArmyDefinition = *armyRegistryCursor;
                                      nextClass10ArmyDefinition = class10ArmyDefinition;
                                      nextClass0EArmyDefinition = class0EArmyDefinition;
                                      nextClass0BArmyDefinition = class0BArmyDefinition;
                                      if ((registryArmyDefinition != (ArmyAssetRecordPrefix *)0x0) &&
                                         ((registryArmyDefinition[1].selectionDetailTemplateVariantIndex &
                                          1) != 0)) {
                                        modelLookupResult = ModelDefinitionRegistry_FindByIdWithErrorCf
                                                           (*(PckModelDefinitionIdCatalog *)
                                                             (registryArmyDefinition->
                                                              rootNodeOffsetOrPointer + 0x20));
                                        modelFlagsOrSoundIndex = modelLookupResult.modelDefinition[6].flags;
                                        nextClass0BArmyDefinition = registryArmyDefinition;
                                        if (((modelFlagsOrSoundIndex != 0xb) &&
                                            ((nextClass10ArmyDefinition = registryArmyDefinition,
                                             nextClass0BArmyDefinition = class0BArmyDefinition, modelFlagsOrSoundIndex != 0x10 &&
                                             (nextClass10ArmyDefinition = class10ArmyDefinition, modelFlagsOrSoundIndex == 0xe)))) &&
                                           (nextClass0EArmyDefinition = registryArmyDefinition,
                                           modelLookupResult.modelDefinition[0x10].byteSize == 0)) {
                                          nextClass0EArmyDefinition = class0EArmyDefinition;
                                          class0ENoExtraArmyDefinition = registryArmyDefinition;
                                        }
                                      }
                                      class0BArmyDefinition = nextClass0BArmyDefinition;
                                      class0EArmyDefinition = nextClass0EArmyDefinition;
                                      class10ArmyDefinition = nextClass10ArmyDefinition;
                                      armyRegistryCursor = armyRegistryCursor + 1;
                                      counterOrClassId = counterOrClassId + -1;
                                    } while (counterOrClassId != 0);
                                    if ((class0BArmyDefinition != (ArmyAssetRecordPrefix *)0x0) &&
                                       (class0ENoExtraArmyDefinition != (ArmyAssetRecordPrefix *)0x0)) {
                                      countOrPackedValue = 1;
                                      ownerListNode = worldRuntime->ownerListHead;
                                      flagsOrRelationMask = 0;
                                      if (ownerListNode != (WorldOwnerListNode100 *)0x0) {
                                        do {
                                          do {
                                            if ((ownerListNode->ownerClassId ==
                                                 WORLD_OWNER_RUNTIME_MODEL) &&
                                               (countOrPackedValue == *(uint32_t *)(*(int *)((int)ownerListNode->
                                                                                 runtimePayload + 8)
                                                                  + 0xc))) {
                                              counterOrClassId = *(int *)(*(int *)ownerListNode->runtimePayload +
                                                              0x4c);
                                              if (counterOrClassId == 0x12) {
                                                flagsOrRelationMask = flagsOrRelationMask | 2;
                                              }
                                              else if (
                                                  g_ArmyRuntimeOrderHandlerMatrix11Columns24Classes.
                                                  classCommand[counterOrClassId] ==
                                                  ArmyRuntime_ClassCommandHandlerGroupACf) {
                                                flagsOrRelationMask = flagsOrRelationMask | 1;
                                              }
                                            }
                                            ownerListNode = ownerListNode->nextNode;
                                          } while (ownerListNode != (WorldOwnerListNode100 *)0x0);
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
                                          countOrPackedValue = countOrPackedValue + 1;
                                          flagsOrRelationMask = 0;
                                        } while (countOrPackedValue <= g_GameFactionRuntimeImage.tail.
                                                          activeFactionCount);
                                      }
                                    }
                                    countOrPackedValue = ((g_InGameLevelRuntimeGlobalBlock.conditionStorage)->
                                            levelImage).runtimeTail2E0.relationUiFlags;
                                    flagsOrRelationMask = ((g_InGameLevelRuntimeGlobalBlock.conditionStorage)->
                                             levelImage).runtimeTail2E0.
                                             relationState4FactionGroupMasks;
                                    regionOriginOrRelationMask = ((g_InGameLevelRuntimeGlobalBlock.conditionStorage)->
                                             levelImage).runtimeTail2E0.
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
                                          while (targetIndexOrBit = targetIndexOrBit + 1, countOrPackedValue = relationFactionIndex, targetIndexOrBit < 8) {
                                            targetBitOrSavedIndex = targetBitOrSavedIndex * 2;
                                            if ((flagsOrRelationMask & targetBitOrSavedIndex) != 0) {
                                              GameFactionRuntime_ApplyPairwiseRelationTransition
                                                        (0x20,0x20,4,4,targetIndexOrBit,relationFactionIndex);
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
                                          while (relationFactionIndex = relationFactionIndex + 1, countOrPackedValue = sourceBitOrIndex, relationFactionIndex < 8) {
                                            targetIndexOrBit = targetIndexOrBit * 2;
                                            if ((regionOriginOrRelationMask & targetIndexOrBit) != 0) {
                                              GameFactionRuntime_ApplyPairwiseRelationTransition
                                                        (0x20,0x20,8,8,relationFactionIndex,sourceBitOrIndex);
                                            }
                                          }
                                        }
                                        sourceBitOrIndex = targetBitOrSavedIndex + 1;
                                        flagsOrRelationMask = flagsOrRelationMask * 2;
                                      } while (sourceBitOrIndex != 8);
                                      regionOriginOrRelationMask = regionOriginOrRelationMask >> 8;
                                      flagsOrRelationMask = regionOriginOrRelationMask;
                                    }
                                    successResult.carry = false;
                                    successResult.errorOrValue = countOrPackedValue;
                                    return successResult;
                                  }
                                  soundSlotCursor = soundLoopWorldRuntime->dwordArray;
                                  soundLoopWorldRuntimeCopy = soundLoopWorldRuntime;
                                  modelFlagsOrSoundIndex = WidePath_ParseTrailingNumberBeforeExtensionRegs
                                                    (soundDirectoryPathCursor);
                                  if (modelFlagsOrSoundIndex < soundLoopWorldRuntime->dwordArrayCount) {
                                    if (!findEntryResult.carry) {
                                      resourceLoadResult = Resource_Load(soundDirectoryPathCursor);
                                      shrinkResultOrError = (void *)resourceLoadResult.eax;
                                      if (resourceLoadResult.carry) break;
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
                                      shrinkResultOrError = (void *)resourceLoadResult.eax;
                                      if (resourceLoadResult.carry) break;
                                    }
                                    soundSlotResult = SpatialSoundSlot_CreateFromSampleAsset
                                                       (shrinkResultOrError);
                                    if (!soundSlotResult.carry) {
                                      soundSlotCursor[modelFlagsOrSoundIndex] = (uint32_t)soundSlotResult.soundSlot;
                                    }
                                    Resource_Release(shrinkResultOrError);
                                    MoviePlayback_AdvanceScheduledFrameAndTick();
                                  }
                                  countOrPackedValue = countOrPackedValue - 1;
                                  soundLoopWorldRuntime = soundLoopWorldRuntimeCopy;
                                  soundDirectoryPathCursor =
                                       (uint16_t *)((int)soundDirectoryPathCursor +
                                               (int)&(((InGameLevelConditionStorageView800 *)
                                                      soundDirectoryRecordSizeBytes)->levelImage).
                                                     header);
                                  levelConditionStorage = g_InGameLevelRuntimeGlobalBlock.conditionStorage;
                                } while( true );
                              }
                            }
                            (*g_MemoryApi.free)(resultOrPointer);
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
InGameLevelRuntime_LoadResourcesAfterDefaultResetCf_ReturnCurrentResourceLoadFailureStatus:
  failureResult.carry = true;
  failureResult.errorOrValue = (uint32_t)resultOrPointer;
  return failureResult;
}


/* Address: 0x00532020.
   Ownership: gameplay/session/level.
   Purpose: Performs the level-asset resource initialization path used after GameData_LoadExternalTables, including
   technology, effect, shot, model, army, terrain, graphics, sound, world, and faction runtime setup with carry-
   based failure reporting.
   Cross-module calls: Package_SetLastErrorPath [assets/package/runtime], WidePath_SetExtensionCode
   [core/text/path], Package_LoadEntry [assets/package/runtime], EffectAsset_PrepareEntries
   [assets/effect/catalog], MoviePlayback_AdvanceScheduledFrameAndTick [movie/runtime/playback],
   ShotAsset_PrepareEntries [assets/shot/catalog].
*/

InGameLevelLoadEaxCf5 __thandor_eax_cf_preserve_ecx_edx
InGameLevelRuntime_LoadResourcesAfterExternalTablesCf
          (FrontendLoadedLevelRuntimeImage370 *levelImage,WorldRuntimeContext *worldRuntime)

{
  LevelPlayerSlotByteOffset32 playerSlotByteOffset;
  InGameLevelConditionStorageView800 *levelConditionStorage;
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
  uint16_t *graphicsBasePath;
  uint16_t *mutableBasePath;
  uint32_t packedRegionValue;
  int factionIndexOrOriginY;
  FieldGridDimensionCells gridHeight;
  FrontendLoadedLevelRuntimeImage370 *copySourceCursor;
  uint8_t *assetPath;
  uint16_t *pathTableCursor;
  uint32_t *soundSlotCursor;
  ArenaAllocEaxCf5 allocResult;
  PackageLoadEntryEaxCf5 loadEntryResult;
  StatusValueEaxCf5 statusResult;
  ArmyRuntimeInitEaxCf5 armyInitResult;
  ArenaShrinkEaxCf5 shrinkResult;
  SpatialSoundSlotEaxCf5 soundSlotResult;
  ArenaFreeEaxCf5 freeResult;
  SoundCreateSampleVoiceSetEaxCf5 voiceSetResult;
  InGameLevelLoadEaxCf5 successResult;
  InGameLevelLoadEaxCf5 failureResult;
  ArenaLargestAllocationEaxEcxCf9 largestBlockResult;
  PackageFindEntryEaxEcxCf9 findEntryResult;
  FileSystemEnumerationEaxEcxCf9 enumerationResult;
  ResourceLoadEaxEcxCf9 resourceLoadResult;
  uint16_t *soundDirectoryPathCursor;
  WorldRuntimeContext *soundLoopWorldRuntime;
  
  allocResult = (*g_MemoryApi.alloc)(0x800);
  loadedResourcePointerArray = (void **)allocResult.eax;
  resultOrPointer = loadedResourcePointerArray;
  if (!allocResult.carry) {
    g_InGameLoadedResourcePointerCount = 0;
    resultOrPointer = (void *)0x39;
    g_InGameLoadedResourcePointers = loadedResourcePointerArray;
    Package_SetLastErrorPath(g_LevelEndingMovieSourcePath);
    if (((levelImage->header).common.magic == ASSET_MAGIC_LEV) &&
       ((levelImage->header).common.converterVersion == PCK_CONVERTER_LEV_00070001)) {
      countOrPackedValue = (levelImage->header).resourceTables.runtimePrefixByteSizeAndInitialArmyPlacementOffset
      ;
      allocResult = (*g_MemoryApi.alloc)(countOrPackedValue);
      resultOrPointer = (void *)allocResult.eax;
      if (!allocResult.carry) {
        copySourceCursor = levelImage;
        g_InGameLevelRuntimeGlobalBlock.conditionStorage = resultOrPointer;
        for (countOrPackedValue = countOrPackedValue >> 2; countOrPackedValue != 0; countOrPackedValue = countOrPackedValue - 1) {
          (((InGameLevelConditionStorageView800 *)resultOrPointer)->levelImage).header.common.magic
               = (copySourceCursor->header).common.magic;
          copySourceCursor = (FrontendLoadedLevelRuntimeImage370 *)
                    &(copySourceCursor->header).common.allocationSizeBytes;
          resultOrPointer =
               &(((InGameLevelConditionStorageView800 *)resultOrPointer)->levelImage).header.common.
                allocationSizeBytes;
        }
        g_InGameLevelTitleTextResourceIndex = (levelImage->header).titleTextResourceIndex;
        g_InGameLevelCampaignAssociationIndex = (levelImage->header).campaignAssociationIndex;
        assetPath = (levelImage->header).common.buildMetadata.assetRelativeAddressAnchor28 +
                  ((levelImage->header).pathState.technologyPathOffset - 0x28);
        WidePath_SetExtensionCode(0x636574,(uint16_t *)assetPath);
        loadEntryResult = Package_LoadEntry((uint16_t *)assetPath);
        loadedTechnologyAsset = loadEntryResult.bufferOrError;
        resultOrPointer = loadedTechnologyAsset;
        if (((!loadEntryResult.carry) &&
            (resultOrPointer = &k_LowAddressLiteral0000004F,
            g_TechnologyAsset = loadedTechnologyAsset,
            (loadedTechnologyAsset->header).common.magic == ASSET_MAGIC_TEC)) &&
           ((loadedTechnologyAsset->header).common.converterVersion == PCK_CONVERTER_TEC_00020000))
        {
          g_MoviePlaybackScheduleSpan =
               (levelImage->header).resourceTables.effectAssetPathCount +
               (levelImage->header).resourceTables.shotAssetPathCount +
               (levelImage->header).resourceTables.modelAssetPathCount +
               (levelImage->header).resourceTables.armyAssetPathCount;
          g_MoviePlaybackBaseFrameGroup = 0;
          g_MoviePlaybackScheduleCounter = 0;
          pathTableCursor = (uint16_t *)((levelImage->header).common.buildMetadata.assetRelativeAddressAnchor28
                            + ((levelImage->header).resourceTables.effectAssetPathTableOffset - 0x28
                              ));
          for (remainingPathCount = (levelImage->header).resourceTables.effectAssetPathCount; remainingPathCount != 0;
              remainingPathCount = remainingPathCount - 1) {
            WidePath_SetExtensionCode(0x666665,pathTableCursor);
            resultOrPointer = (InGameLevelConditionStorageView800 *)0x3a;
            if (0x1ff < g_InGameLoadedResourcePointerCount)
            goto 
            InGameLevelRuntime_LoadResourcesAfterExternalTablesCf_ReturnCurrentResourceLoadFailureStatus
            ;
            loadEntryResult = Package_LoadEntry(pathTableCursor);
            resultOrPointer = loadEntryResult.bufferOrError;
            if (loadEntryResult.carry)
            goto 
            InGameLevelRuntime_LoadResourcesAfterExternalTablesCf_ReturnCurrentResourceLoadFailureStatus
            ;
            *loadedResourcePointerArray = resultOrPointer;
            g_InGameLoadedResourcePointerCount = g_InGameLoadedResourcePointerCount + 1;
            loadedResourcePointerArray = loadedResourcePointerArray + 1;
            statusResult = EffectAsset_PrepareEntries(resultOrPointer);
            resultOrPointer = (void *)statusResult.valueOrError;
            if (statusResult.carry)
            goto 
            InGameLevelRuntime_LoadResourcesAfterExternalTablesCf_ReturnCurrentResourceLoadFailureStatus
            ;
            MoviePlayback_AdvanceScheduledFrameAndTick();
            pathTableCursor = pathTableCursor + 0x20;
          }
          pathTableCursor = (uint16_t *)((levelImage->header).common.buildMetadata.assetRelativeAddressAnchor28
                            + ((levelImage->header).resourceTables.shotAssetPathTableOffset - 0x28))
          ;
          for (remainingPathCount = (levelImage->header).resourceTables.shotAssetPathCount; remainingPathCount != 0;
              remainingPathCount = remainingPathCount - 1) {
            WidePath_SetExtensionCode(0x746873,pathTableCursor);
            resultOrPointer = (InGameLevelConditionStorageView800 *)0x3a;
            if (0x1ff < g_InGameLoadedResourcePointerCount)
            goto 
            InGameLevelRuntime_LoadResourcesAfterExternalTablesCf_ReturnCurrentResourceLoadFailureStatus
            ;
            loadEntryResult = Package_LoadEntry(pathTableCursor);
            resultOrPointer = loadEntryResult.bufferOrError;
            if (loadEntryResult.carry)
            goto 
            InGameLevelRuntime_LoadResourcesAfterExternalTablesCf_ReturnCurrentResourceLoadFailureStatus
            ;
            *loadedResourcePointerArray = resultOrPointer;
            g_InGameLoadedResourcePointerCount = g_InGameLoadedResourcePointerCount + 1;
            loadedResourcePointerArray = loadedResourcePointerArray + 1;
            statusResult = ShotAsset_PrepareEntries(resultOrPointer);
            resultOrPointer = (void *)statusResult.valueOrError;
            if (statusResult.carry)
            goto 
            InGameLevelRuntime_LoadResourcesAfterExternalTablesCf_ReturnCurrentResourceLoadFailureStatus
            ;
            MoviePlayback_AdvanceScheduledFrameAndTick();
            pathTableCursor = pathTableCursor + 0x20;
          }
          statusResult = EffectDefinitions_ResolveCrossReferences();
          resultOrPointer = (void *)statusResult.valueOrError;
          if (!statusResult.carry) {
            pathTableCursor = (uint16_t *)((levelImage->header).common.buildMetadata.
                               assetRelativeAddressAnchor28 +
                              ((levelImage->header).resourceTables.modelAssetPathTableOffset - 0x28)
                              );
            for (remainingPathCount = (levelImage->header).resourceTables.modelAssetPathCount; remainingPathCount != 0;
                remainingPathCount = remainingPathCount - 1) {
              WidePath_SetExtensionCode(0x6c646d,pathTableCursor);
              resultOrPointer = (InGameLevelConditionStorageView800 *)0x3a;
              if (0x1ff < g_InGameLoadedResourcePointerCount)
              goto 
              InGameLevelRuntime_LoadResourcesAfterExternalTablesCf_ReturnCurrentResourceLoadFailureStatus
              ;
              loadEntryResult = Package_LoadEntry(pathTableCursor);
              resultOrPointer = loadEntryResult.bufferOrError;
              if (loadEntryResult.carry)
              goto 
              InGameLevelRuntime_LoadResourcesAfterExternalTablesCf_ReturnCurrentResourceLoadFailureStatus
              ;
              *loadedResourcePointerArray = resultOrPointer;
              g_InGameLoadedResourcePointerCount = g_InGameLoadedResourcePointerCount + 1;
              loadedResourcePointerArray = loadedResourcePointerArray + 1;
              statusResult = ModelAsset_PrepareRecords(resultOrPointer);
              resultOrPointer = (void *)statusResult.valueOrError;
              if (statusResult.carry)
              goto 
              InGameLevelRuntime_LoadResourcesAfterExternalTablesCf_ReturnCurrentResourceLoadFailureStatus
              ;
              MoviePlayback_AdvanceScheduledFrameAndTick();
              pathTableCursor = pathTableCursor + 0x20;
            }
            pathTableCursor = (uint16_t *)((levelImage->header).common.buildMetadata.
                               assetRelativeAddressAnchor28 +
                              ((levelImage->header).resourceTables.armyAssetPathTableOffset - 0x28))
            ;
            for (remainingPathCount = (levelImage->header).resourceTables.armyAssetPathCount; remainingPathCount != 0;
                remainingPathCount = remainingPathCount - 1) {
              WidePath_SetExtensionCode(0x6d7261,pathTableCursor);
              resultOrPointer = (InGameLevelConditionStorageView800 *)0x3a;
              if (0x1ff < g_InGameLoadedResourcePointerCount)
              goto 
              InGameLevelRuntime_LoadResourcesAfterExternalTablesCf_ReturnCurrentResourceLoadFailureStatus
              ;
              loadEntryResult = Package_LoadEntry(pathTableCursor);
              resultOrPointer = loadEntryResult.bufferOrError;
              if (loadEntryResult.carry)
              goto 
              InGameLevelRuntime_LoadResourcesAfterExternalTablesCf_ReturnCurrentResourceLoadFailureStatus
              ;
              *loadedResourcePointerArray = resultOrPointer;
              g_InGameLoadedResourcePointerCount = g_InGameLoadedResourcePointerCount + 1;
              loadedResourcePointerArray = loadedResourcePointerArray + 1;
              statusResult = ArmyAsset_PrepareRecords(resultOrPointer);
              resultOrPointer = (void *)statusResult.valueOrError;
              if (statusResult.carry)
              goto 
              InGameLevelRuntime_LoadResourcesAfterExternalTablesCf_ReturnCurrentResourceLoadFailureStatus
              ;
              MoviePlayback_AdvanceScheduledFrameAndTick();
              pathTableCursor = pathTableCursor + 0x20;
            }
            resultOrPointer = (InGameLevelConditionStorageView800 *)0x3a;
            if (g_InGameLoadedResourcePointerCount < 0x200) {
              g_MoviePlaybackBaseFrameGroup = 1;
              g_MoviePlaybackScheduleCounter = 0;
              g_MoviePlaybackScheduleSpan = 0x10000;
              statusResult = TerrainVisualResources_LoadAndClearCellOverlayFlags
                                 ((uint16_t *)((levelImage->header).common.buildMetadata.
                                           assetRelativeAddressAnchor28 +
                                          ((levelImage->header).pathState.
                                           surfaceTextureBasePathOffset - 0x28)),
                                  (uint16_t *)((levelImage->header).common.buildMetadata.
                                           assetRelativeAddressAnchor28 +
                                          ((levelImage->header).pathState.
                                           groundTextureBasePathOffset - 0x28)),
                                  (FieldGridAsset *)
                                  (levelImage->header).pathState.levelPathOffsetOrLoadedFieldGrid);
              resultOrPointer = (void *)statusResult.valueOrError;
              if (!statusResult.carry) {
                statusResult = ShotDefinitions_ValidateTerrainMaterialReferences();
                resultOrPointer = (void *)statusResult.valueOrError;
                if (!statusResult.carry) {
                  g_MoviePlaybackBaseFrameGroup = 2;
                  g_MoviePlaybackScheduleCounter = 0;
                  g_MoviePlaybackScheduleSpan = 0x10000;
                  statusResult = ModelRuntimePool_Init();
                  resultOrPointer = (void *)statusResult.valueOrError;
                  if (!statusResult.carry) {
                    /* The decompiler lost these two locals; the original reads them from the level header
                       (+0xC0 army and +0xC8 effect texture base paths). */
                    graphicsBasePath = (uint16_t *)((levelImage->header).common.buildMetadata.assetRelativeAddressAnchor28 +
                                      ((levelImage->header).pathState.armyTextureBasePathOffset - 0x28));
                    mutableBasePath = (uint16_t *)((levelImage->header).common.buildMetadata.assetRelativeAddressAnchor28 +
                                      ((levelImage->header).pathState.effectTextureBasePathOffset - 0x28));
                    armyInitResult = ArmyRuntime_InitializePoolAndGraphicsCf(worldRuntime,graphicsBasePath);
                    resultOrPointer = (void *)armyInitResult.errorOrValue;
                    if (!armyInitResult.carry) {
                      g_MoviePlaybackBaseFrameGroup = 3;
                      g_MoviePlaybackScheduleCounter = 0;
                      g_MoviePlaybackScheduleSpan = 4;
                      statusResult = ShotRuntime_InitGraphicsResources
                                         ((uint16_t *)((levelImage->header).common.buildMetadata.
                                                   assetRelativeAddressAnchor28 +
                                                  ((levelImage->header).pathState.
                                                   shotTextureBasePathOffset - 0x28)));
                      resultOrPointer = (void *)statusResult.valueOrError;
                      if (!statusResult.carry) {
                        g_MoviePlaybackBaseFrameGroup = 4;
                        g_MoviePlaybackScheduleCounter = 0;
                        g_MoviePlaybackScheduleSpan = 4;
                        statusResult = EffectRuntime_InitGraphicsResources(mutableBasePath);
                        resultOrPointer = (void *)statusResult.valueOrError;
                        if (!statusResult.carry) {
                          g_MoviePlaybackBaseFrameGroup = 5;
                          g_MoviePlaybackScheduleCounter = 0;
                          g_MoviePlaybackScheduleSpan = 6;
                          GameFactionRuntime_RebaseLoadedArmyReferences();
                          WorldRuntime_SetTerrainLightingConfiguration
                                    ((levelImage->runtimeTail2E0).terrainLightingColor13CArgb,
                                     (levelImage->runtimeTail2E0).terrainLightingColor138Argb,
                                     (levelImage->runtimeTail2E0).terrainLightingColor134Argb,
                                     (levelImage->runtimeTail2E0).terrainLightingColor130Argb,
                                     (levelImage->runtimeTail2E0).terrainRampColor12CArgb,
                                     (levelImage->runtimeTail2E0).terrainLightingColor128Argb,
                                     (levelImage->runtimeTail2E0).terrainRampColor124Argb,
                                     (levelImage->runtimeTail2E0).terrainBaseColorArgb,worldRuntime)
                          ;
                          factionIndexOrOriginY = worldRuntime->activeFactionRuntimeIndex;
                          MoviePlayback_AdvanceScheduledFrameAndTick();
                          playerSlotByteOffset = g_InGameLevelRuntimeGlobalBlock.playerSlotByteOffsets[factionIndexOrOriginY + -1]
                          ;
                          WorldRuntime_AttachFieldGridAsset
                                    ((FieldGridAsset *)
                                     (levelImage->header).pathState.levelPathOffsetOrLoadedFieldGrid
                                     ,worldRuntime);
                          MoviePlayback_AdvanceScheduledFrameAndTick();
                          countOrPackedValue = *(uint32_t *)((int)&levelImage->playerSlots[0].
                                                  packedHeadingLow16PitchHigh16 + playerSlotByteOffset);
                          WorldRuntime_SetPosition60AndDistanceFromPosition80
                                    (*(Q12 *)((int)&levelImage->playerSlots[0].startCameraZQ12 +
                                             playerSlotByteOffset),
                                     *(Q12 *)((int)&levelImage->playerSlots[0].startCameraYQ12 +
                                             playerSlotByteOffset),
                                     *(Q12 *)((int)&levelImage->playerSlots[0].startCameraXQ12 +
                                             playerSlotByteOffset),worldRuntime);
                          WorldRuntime_SetMotionParameters6CThrough78Clamped
                                    (2,(int)countOrPackedValue >> 0x10,countOrPackedValue & 0xffff,
                                     *(UQ12 *)((int)&levelImage->playerSlots[0].
                                                     startCameraMagnitudeQ12 + playerSlotByteOffset),worldRuntime);
                          countOrPackedValue = (levelImage->runtimeTail2E0).packedFieldRegionOriginYHigh16XLow16;
                          WorldRuntime_RecomputeMotionEndpointAgainstFieldSurface(worldRuntime);
                          packedRegionValue = countOrPackedValue;
                          WorldRuntime_CommitScalar7CFrom8C(worldRuntime);
                          countOrPackedValue = countOrPackedValue & 0xffff;
                          factionIndexOrOriginY = (int)packedRegionValue >> 0x10;
                          packedRegionValue = (levelImage->runtimeTail2E0).
                                  packedFieldRegionHeightHigh16WidthLow16;
                          soundLoopWorldRuntime = worldRuntime;
                          MoviePlayback_AdvanceScheduledFrameAndTick();
                          WorldRuntime_RecomputeFieldRegionNormalsAndLighting
                                    ((int)packedRegionValue >> 0x10,packedRegionValue & 0xffff,factionIndexOrOriginY,countOrPackedValue,soundLoopWorldRuntime);
                          statusResult = Package_LoadEntryIntoBuffer
                                             (worldRuntime->objectCount * 0x100,
                                              (uint8_t *)worldRuntime->objectArray,
                                              (uint16_t *)u_widget_hex_0050e02a);
                          resultOrPointer = (void *)statusResult.valueOrError;
                          if (!statusResult.carry) {
                            statusResult = Package_LoadEntryIntoBuffer
                                               (0x48000,(uint8_t *)g_ArmyRuntimeSlots,
                                                (uint16_t *)u_army_hex_0050dfb4);
                            resultOrPointer = (void *)statusResult.valueOrError;
                            if (!statusResult.carry) {
                              statusResult = Package_LoadEntryIntoBuffer
                                                 (0x400000,(uint8_t *)g_ModelRuntimeSlots,
                                                  (uint16_t *)u_modul_hex_0050dfee);
                              resultOrPointer = (void *)statusResult.valueOrError;
                              if (!statusResult.carry) {
                                statusResult = Package_LoadEntryIntoBuffer
                                                   (0x40000,(uint8_t *)g_EffectRuntimeSlots,
                                                    (uint16_t *)u_effect_hex_0050dfc6);
                                resultOrPointer = (void *)statusResult.valueOrError;
                                if (!statusResult.carry) {
                                  statusResult = Package_LoadEntryIntoBuffer
                                                     (0x40000,(uint8_t *)g_ShotRuntimeSlots,
                                                      (uint16_t *)u_shot_hex_0050dfdc);
                                  resultOrPointer = (void *)statusResult.valueOrError;
                                  if (!statusResult.carry) {
                                    statusResult = Package_LoadEntryIntoBuffer
                                                       (0x4000,(uint8_t *)
                                                  g_GraphicsShadingRuntimeRecords,
                                                  (uint16_t *)u_light_hex_0050e016);
                                    resultOrPointer = (void *)statusResult.valueOrError;
                                    if (!statusResult.carry) {
                                      ArmyRuntimePool_RebaseAfterLoad();
                                      ModelRuntimePool_RebaseAfterLoad();
                                      ShotRuntime_RebaseSlotsAfterLoad();
                                      EffectRuntime_RebaseSlotsAfterLoad();
                                      ResourceRegistrationRuntime_RebaseLoadedRecords
                                                ((ResourceRegistrationRuntimeImage *)worldRuntime);
                                      RuntimeHexSegment_ToggleLightImageFlag();
                                      MoviePlayback_AdvanceScheduledFrameAndTick();
                                      soundSlotCursor = worldRuntime->dwordArray;
                                      for (remainingSoundSlotCount = worldRuntime->dwordArrayCount; remainingSoundSlotCount != 0;
                                          remainingSoundSlotCount = remainingSoundSlotCount - 1) {
                                        *soundSlotCursor = 0;
                                        soundSlotCursor = soundSlotCursor + 1;
                                      }
                                      assetPath = (levelImage->header).common.buildMetadata.
                                                assetRelativeAddressAnchor28 +
                                                ((levelImage->header).pathState.soundBasePathOffset
                                                - 0x28);
                                      WidePath_SetExtensionCode(0x6d6173,(uint16_t *)assetPath);
                                      MoviePlayback_AdvanceScheduledFrameAndTick();
                                      WidePath_SplitParentAndLeaf
                                                ((uint16_t *)&
                                                  g_InGameLevelSoundLeafOrCombinedPathScratchUtf16,
                                                 (uint16_t *)&
                                                  g_InGameLevelSoundParentDirectoryScratchUtf16,
                                                 (uint16_t *)assetPath);
                                      largestBlockResult = (*g_MemoryApi.allocLargestFreeBlock)();
                                      outputCapacityBytes = largestBlockResult.blockSizeOrSentinel;
                                      resultOrPointer = (void *)largestBlockResult.allocationOrError;
                                      if (!largestBlockResult.carry) {
                                        findEntryResult = Package_FindEntry(outputCapacityBytes,
                                                                   resultOrPointer,(uint16_t *)assetPath,
                                                                   g_SoundPackageHandle);
                                        countOrPackedValue = findEntryResult.matchCount;
                                        soundDirectoryRecordSizeBytes = findEntryResult.recordSizeOrError;
                                        if (findEntryResult.carry) {
                                          enumerationResult = (*g_FileSystemEnumerateDirectoryOrVolumeEntriesCf
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
                                          shrinkResult = (*g_MemoryApi.shrinkInPlace)
                                                             (soundDirectoryRecordSizeBytes * countOrPackedValue,
                                                              resultOrPointer);
                                          shrinkResultOrError = (void *)shrinkResult.scratchOrError;
                                          if (!shrinkResult.carry) {
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
                                                freeResult = (*g_MemoryApi.free)(resultOrPointer);
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
                                                sampleOrVoiceSet = (SoundSampleAsset *)freeResult.eax;
                                                if ((levelConditionStorage->levelImage).runtimeTail2E0.
                                                    effectSampleNumbers[0] != 0) {
                                                  (*g_WideNumberFormatUtf16)
                                                            (WIDE_FORMAT_PAD_WITH_ZERO,0,2,1,
                                                             (levelConditionStorage->levelImage).runtimeTail2E0.
                                                             effectSampleNumbers[0],
                                                             (uint16_t *)(u_sound_level00_sam_0050df6c +
                                                                     0xb));
                                                  resourceLoadResult = Resource_Load((uint16_t *)
                                                  u_sound_level00_sam_0050df6c);
                                                  loadedSampleAsset = (SoundSampleAsset *)resourceLoadResult.eax;
                                                  sampleOrVoiceSet = loadedSampleAsset;
                                                  if (!resourceLoadResult.carry) {
                                                    voiceSetResult = (*g_SoundCreateSampleVoiceSet)(loadedSampleAsset);
                                                    sampleOrVoiceSet = (SoundSampleAsset *)voiceSetResult.eax;
                                                    if (!voiceSetResult.carry) {
                                                      g_InGameLevelEffectVoiceSet0 = sampleOrVoiceSet;
                                                    }
                                                    Resource_Release(loadedSampleAsset);
                                                  }
                                                }
                                                if ((levelConditionStorage->levelImage).runtimeTail2E0.
                                                    effectSampleNumbers[1] != 0) {
                                                  (*g_WideNumberFormatUtf16)
                                                            (WIDE_FORMAT_PAD_WITH_ZERO,0,2,1,
                                                             (levelConditionStorage->levelImage).runtimeTail2E0.
                                                             effectSampleNumbers[1],
                                                             (uint16_t *)(u_sound_level00_sam_0050df6c +
                                                                     0xb));
                                                  resourceLoadResult = Resource_Load((uint16_t *)
                                                  u_sound_level00_sam_0050df6c);
                                                  loadedSampleAsset = (SoundSampleAsset *)resourceLoadResult.eax;
                                                  sampleOrVoiceSet = loadedSampleAsset;
                                                  if (!resourceLoadResult.carry) {
                                                    voiceSetResult = (*g_SoundCreateSampleVoiceSet)(loadedSampleAsset);
                                                    sampleOrVoiceSet = (SoundSampleAsset *)voiceSetResult.eax;
                                                    if (!voiceSetResult.carry) {
                                                      g_InGameLevelEffectVoiceSet1 = sampleOrVoiceSet;
                                                    }
                                                    Resource_Release(loadedSampleAsset);
                                                  }
                                                }
                                                if ((levelConditionStorage->levelImage).runtimeTail2E0.
                                                    effectSampleNumbers[2] != 0) {
                                                  (*g_WideNumberFormatUtf16)
                                                            (WIDE_FORMAT_PAD_WITH_ZERO,0,2,1,
                                                             (levelConditionStorage->levelImage).runtimeTail2E0.
                                                             effectSampleNumbers[2],
                                                             (uint16_t *)(u_sound_level00_sam_0050df6c +
                                                                     0xb));
                                                  resourceLoadResult = Resource_Load((uint16_t *)
                                                  u_sound_level00_sam_0050df6c);
                                                  loadedSampleAsset = (SoundSampleAsset *)resourceLoadResult.eax;
                                                  sampleOrVoiceSet = loadedSampleAsset;
                                                  if (!resourceLoadResult.carry) {
                                                    voiceSetResult = (*g_SoundCreateSampleVoiceSet)(loadedSampleAsset);
                                                    sampleOrVoiceSet = (SoundSampleAsset *)voiceSetResult.eax;
                                                    if (!voiceSetResult.carry) {
                                                      g_InGameLevelEffectVoiceSet2 = sampleOrVoiceSet;
                                                    }
                                                    Resource_Release(loadedSampleAsset);
                                                  }
                                                }
                                                if ((levelConditionStorage->levelImage).runtimeTail2E0.
                                                    effectSampleNumbers[3] != 0) {
                                                  (*g_WideNumberFormatUtf16)
                                                            (WIDE_FORMAT_PAD_WITH_ZERO,0,2,1,
                                                             (levelConditionStorage->levelImage).runtimeTail2E0.
                                                             effectSampleNumbers[3],
                                                             (uint16_t *)(u_sound_level00_sam_0050df6c +
                                                                     0xb));
                                                  resourceLoadResult = Resource_Load((uint16_t *)
                                                  u_sound_level00_sam_0050df6c);
                                                  loadedSampleAsset = (SoundSampleAsset *)resourceLoadResult.eax;
                                                  sampleOrVoiceSet = loadedSampleAsset;
                                                  if (!resourceLoadResult.carry) {
                                                    voiceSetResult = (*g_SoundCreateSampleVoiceSet)(loadedSampleAsset);
                                                    sampleOrVoiceSet = (SoundSampleAsset *)voiceSetResult.eax;
                                                    if (!voiceSetResult.carry) {
                                                      g_InGameLevelEffectVoiceSet3 = sampleOrVoiceSet;
                                                    }
                                                    Resource_Release(loadedSampleAsset);
                                                  }
                                                }
                                                if ((levelConditionStorage->levelImage).runtimeTail2E0.
                                                    musicSampleNumbers[0] != 0) {
                                                  (*g_WideNumberFormatUtf16)
                                                            (WIDE_FORMAT_PAD_WITH_ZERO,0,2,1,
                                                             (levelConditionStorage->levelImage).runtimeTail2E0.
                                                             musicSampleNumbers[0],
                                                             (uint16_t *)(u_sound_music00_sam_0050df90 +
                                                                     0xb));
                                                  resourceLoadResult = Resource_Load((uint16_t *)
                                                  u_sound_music00_sam_0050df90);
                                                  loadedSampleAsset = (SoundSampleAsset *)resourceLoadResult.eax;
                                                  sampleOrVoiceSet = loadedSampleAsset;
                                                  if (!resourceLoadResult.carry) {
                                                    voiceSetResult = (*g_SoundCreateSampleVoiceSet)(loadedSampleAsset);
                                                    sampleOrVoiceSet = (SoundSampleAsset *)voiceSetResult.eax;
                                                    if (!voiceSetResult.carry) {
                                                      g_InGameLevelMusicVoiceSet0 = sampleOrVoiceSet;
                                                    }
                                                    Resource_Release(loadedSampleAsset);
                                                  }
                                                }
                                                if ((levelConditionStorage->levelImage).runtimeTail2E0.
                                                    musicSampleNumbers[1] != 0) {
                                                  (*g_WideNumberFormatUtf16)
                                                            (WIDE_FORMAT_PAD_WITH_ZERO,0,2,1,
                                                             (levelConditionStorage->levelImage).runtimeTail2E0.
                                                             musicSampleNumbers[1],
                                                             (uint16_t *)(u_sound_music00_sam_0050df90 +
                                                                     0xb));
                                                  resourceLoadResult = Resource_Load((uint16_t *)
                                                  u_sound_music00_sam_0050df90);
                                                  loadedSampleAsset = (SoundSampleAsset *)resourceLoadResult.eax;
                                                  sampleOrVoiceSet = loadedSampleAsset;
                                                  if (!resourceLoadResult.carry) {
                                                    voiceSetResult = (*g_SoundCreateSampleVoiceSet)(loadedSampleAsset);
                                                    sampleOrVoiceSet = (SoundSampleAsset *)voiceSetResult.eax;
                                                    if (!voiceSetResult.carry) {
                                                      g_InGameLevelMusicVoiceSet1 = sampleOrVoiceSet;
                                                    }
                                                    Resource_Release(loadedSampleAsset);
                                                  }
                                                }
                                                if ((levelConditionStorage->levelImage).runtimeTail2E0.
                                                    musicSampleNumbers[2] != 0) {
                                                  (*g_WideNumberFormatUtf16)
                                                            (WIDE_FORMAT_PAD_WITH_ZERO,0,2,1,
                                                             (levelConditionStorage->levelImage).runtimeTail2E0.
                                                             musicSampleNumbers[2],
                                                             (uint16_t *)(u_sound_music00_sam_0050df90 +
                                                                     0xb));
                                                  resourceLoadResult = Resource_Load((uint16_t *)
                                                  u_sound_music00_sam_0050df90);
                                                  loadedSampleAsset = (SoundSampleAsset *)resourceLoadResult.eax;
                                                  sampleOrVoiceSet = loadedSampleAsset;
                                                  if (!resourceLoadResult.carry) {
                                                    voiceSetResult = (*g_SoundCreateSampleVoiceSet)(loadedSampleAsset);
                                                    sampleOrVoiceSet = (SoundSampleAsset *)voiceSetResult.eax;
                                                    if (!voiceSetResult.carry) {
                                                      g_InGameLevelMusicVoiceSet2 = sampleOrVoiceSet;
                                                    }
                                                    Resource_Release(loadedSampleAsset);
                                                  }
                                                }
                                                if ((levelConditionStorage->levelImage).runtimeTail2E0.
                                                    musicSampleNumbers[3] != 0) {
                                                  (*g_WideNumberFormatUtf16)
                                                            (WIDE_FORMAT_PAD_WITH_ZERO,0,2,1,
                                                             (levelConditionStorage->levelImage).runtimeTail2E0.
                                                             musicSampleNumbers[3],
                                                             (uint16_t *)(u_sound_music00_sam_0050df90 +
                                                                     0xb));
                                                  resourceLoadResult = Resource_Load((uint16_t *)
                                                  u_sound_music00_sam_0050df90);
                                                  loadedSampleAsset = (SoundSampleAsset *)resourceLoadResult.eax;
                                                  sampleOrVoiceSet = loadedSampleAsset;
                                                  if (!resourceLoadResult.carry) {
                                                    voiceSetResult = (*g_SoundCreateSampleVoiceSet)(loadedSampleAsset);
                                                    sampleOrVoiceSet = (SoundSampleAsset *)voiceSetResult.eax;
                                                    if (!voiceSetResult.carry) {
                                                      g_InGameLevelMusicVoiceSet3 = sampleOrVoiceSet;
                                                    }
                                                    Resource_Release(loadedSampleAsset);
                                                  }
                                                }
                                                successResult.carry = false;
                                                successResult.errorOrValue = (uint32_t)sampleOrVoiceSet;
                                                return successResult;
                                              }
                                              soundSlotCursor = worldRuntime->dwordArray;
                                              soundLoopWorldRuntime = worldRuntime;
                                              soundSlotIndex = 
                                                  WidePath_ParseTrailingNumberBeforeExtensionRegs
                                                            (soundDirectoryPathCursor);
                                              if (soundSlotIndex < worldRuntime->dwordArrayCount) {
                                                if (!findEntryResult.carry) {
                                                  resourceLoadResult = Resource_Load(soundDirectoryPathCursor);
                                                  shrinkResultOrError = (void *)resourceLoadResult.eax;
                                                  if (resourceLoadResult.carry) break;
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
                                                  shrinkResultOrError = (void *)resourceLoadResult.eax;
                                                  if (resourceLoadResult.carry) break;
                                                }
                                                soundSlotResult = SpatialSoundSlot_CreateFromSampleAsset
                                                                   (shrinkResultOrError);
                                                if (!soundSlotResult.carry) {
                                                  soundSlotCursor[soundSlotIndex] = (uint32_t)soundSlotResult.soundSlot;
                                                }
                                                Resource_Release(shrinkResultOrError);
                                                MoviePlayback_AdvanceScheduledFrameAndTick();
                                              }
                                              countOrPackedValue = countOrPackedValue - 1;
                                              worldRuntime = soundLoopWorldRuntime;
                                              soundDirectoryPathCursor =
                                                   (uint16_t *)((int)soundDirectoryPathCursor +
                                                           (int)&(((
                                                  InGameLevelConditionStorageView800 *)
                                                  soundDirectoryRecordSizeBytes)->levelImage).header
                                                  );
                                              levelConditionStorage = g_InGameLevelRuntimeGlobalBlock.
                                                       conditionStorage;
                                            } while( true );
                                          }
                                        }
                                        (*g_MemoryApi.free)(resultOrPointer);
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
InGameLevelRuntime_LoadResourcesAfterExternalTablesCf_ReturnCurrentResourceLoadFailureStatus:
  failureResult.carry = true;
  failureResult.errorOrValue = (uint32_t)resultOrPointer;
  return failureResult;
}


/* Address: 0x005329C0.
   Ownership: gameplay/session/level.
   Purpose: Shuts down effect, shot, model, army, and terrain resources; releases spatial sounds, shared sample
   voices, tracked package resources, the copied level image, and the technology asset; then clears the owning
   globals.
   Cross-module calls: EffectRuntime_ShutdownGraphicsResources [world/effects/runtime],
   ShotRuntime_ShutdownGraphicsResources [world/shots/runtime], ModelRuntimePool_ShutdownAndReleaseDefinitions
   [world/model/runtime], ArmyRuntime_ShutdownPoolAndGraphics [gameplay/army/runtime],
   TerrainVisualResources_Shutdown [world/terrain/visuals], SpatialSoundSlot_ReleaseSample [audio/spatial/runtime].
*/

void __thandor_void_preserve_eax_ecx
InGameLevelRuntime_ShutdownLoadedAssetResources(WorldRuntimeContext *worldRuntime)

{
  void **loadedResourceCursor;
  InGameLoadedResourcePointerCount remainingResourceCount;
  uint32_t *slotLoopGuard;
  uint32_t *remainingSlotCount;
  uint32_t *soundSlotCursor;
  void **resourcePointerCursor;
  
  EffectRuntime_ShutdownGraphicsResources();
  ShotRuntime_ShutdownGraphicsResources();
  ModelRuntimePool_ShutdownAndReleaseDefinitions();
  ArmyRuntime_ShutdownPoolAndGraphics();
  TerrainVisualResources_Shutdown();
  remainingSlotCount = (uint32_t *)worldRuntime->dwordArrayCount;
  soundSlotCursor = worldRuntime->dwordArray;
  slotLoopGuard = soundSlotCursor;
  if (remainingSlotCount != (uint32_t *)0x0) {
    while (slotLoopGuard != (uint32_t *)0x0) {
      SpatialSoundSlot_ReleaseSample((SpatialSoundSlot *)*soundSlotCursor);
      soundSlotCursor = soundSlotCursor + 1;
      remainingSlotCount = (uint32_t *)((int)remainingSlotCount + -1);
      slotLoopGuard = remainingSlotCount;
    }
  }
  (*g_SoundReleaseSampleVoiceSet)(g_InGameLevelEffectVoiceSet0);
  (*g_SoundReleaseSampleVoiceSet)(g_InGameLevelEffectVoiceSet1);
  (*g_SoundReleaseSampleVoiceSet)(g_InGameLevelEffectVoiceSet2);
  (*g_SoundReleaseSampleVoiceSet)(g_InGameLevelEffectVoiceSet3);
  (*g_SoundReleaseSampleVoiceSet)(g_InGameLevelMusicVoiceSet0);
  (*g_SoundReleaseSampleVoiceSet)(g_InGameLevelMusicVoiceSet1);
  (*g_SoundReleaseSampleVoiceSet)(g_InGameLevelMusicVoiceSet2);
  (*g_SoundReleaseSampleVoiceSet)(g_InGameLevelMusicVoiceSet3);
  loadedResourceCursor = g_InGameLoadedResourcePointers;
  remainingResourceCount = g_InGameLoadedResourcePointerCount;
  if (g_InGameLoadedResourcePointers != (void **)0x0) {
    for (; remainingResourceCount != 0; remainingResourceCount = remainingResourceCount - 1) {
      Resource_Release(*loadedResourceCursor);
      loadedResourceCursor = loadedResourceCursor + 1;
    }
  }
  (*g_MemoryApi.free)(g_InGameLoadedResourcePointers);
  g_InGameLoadedResourcePointers = (void **)0x0;
  g_InGameLoadedResourcePointerCount = 0;
  (*g_MemoryApi.free)(g_InGameLevelRuntimeGlobalBlock.conditionStorage);
  g_InGameLevelRuntimeGlobalBlock.conditionStorage = (InGameLevelConditionStorageView800 *)0x0;
  Resource_Release(g_TechnologyAsset);
  g_TechnologyAsset = (TechnologyAsset *)0x0;
  return;
}


/* Address: 0x00532CA0.
   Ownership: gameplay/session/level.
   Purpose: Copies the base LEV image, serializes live 0x20-byte placements, writes count/size and seven camera
   bookmark records, then writes the level asset. RET 4 and CF report status. No executable bytes changed. It
   serializes placements and the seven camera bookmarks from live world state. It is not the FLD writer.
   Cross-module calls: Package_LoadEntryIntoBuffer [assets/package/runtime], FileSystem_WriteBufferToPathCf
   [platform/filesystem/win32].
*/

StatusValueEaxCf5 __thandor_eax_cf_preserve_ecx_edx
InGameLevelRuntime_SaveLevelAssetImageFromWorldStateCf(InGameLevelSaveWorldView *saveWorldView)

{
  int offsetOrModelRuntime;
  FactionRuntimeIndex activeFactionIndex;
  WorldOwnerListNode100 *ownerListNode;
  AngleTurn32 modelRotationAngle;
  uint8_t *source;
  uint32_t bookmarkZ;
  uint32_t bookmarkMagnitude;
  uint32_t bookmarkPackedHeadingPitch;
  uint32_t statusOrFieldValue;
  LevelPlacedModelRecord20 *objectRecordCursor;
  StatusValueEaxCf5 statusResult;
  
  statusResult = Package_LoadEntryIntoBuffer(0x800000,g_PackageScratchBuffer,g_LevelEndingMovieSourcePath);
  source = g_PackageScratchBuffer;
  statusOrFieldValue = statusResult.valueOrError;
  if (!statusResult.carry) {
    offsetOrModelRuntime = *(int *)(g_PackageScratchBuffer + 0xdc);
    activeFactionIndex = (saveWorldView->worldRuntime).activeFactionRuntimeIndex;
    *(int *)(g_PackageScratchBuffer + 4) = offsetOrModelRuntime;
    source[0xd8] = 0;
    source[0xd9] = 0;
    source[0xda] = 0;
    source[0xdb] = 0;
    *(FactionRuntimeIndex *)(source + 0x2dc) = activeFactionIndex;
    objectRecordCursor = (LevelPlacedModelRecord20 *)(source + offsetOrModelRuntime);
    for (ownerListNode = (saveWorldView->worldRuntime).ownerListHead;
        ownerListNode != (WorldOwnerListNode100 *)0x0; ownerListNode = ownerListNode->nextNode) {
      if (ownerListNode->ownerClassId == WORLD_OWNER_RUNTIME_MODEL) {
        *(int *)(source + 0xd8) = *(int *)(source + 0xd8) + 1;
        *(int *)(source + 4) = *(int *)(source + 4) + 0x20;
        offsetOrModelRuntime = *(int *)((int)ownerListNode->runtimePayload + 8);
        objectRecordCursor->meshGroupMask = ownerListNode->worldXQ12;
        statusOrFieldValue = *(uint32_t *)(offsetOrModelRuntime + 0xa0);
        objectRecordCursor->modelRuntimeField0C = *(uint32_t *)(offsetOrModelRuntime + 0xc);
        objectRecordCursor->modelRuntimeFieldA0 = statusOrFieldValue;
        modelRotationAngle = ownerListNode->modelLocalRotationAngle2;
        objectRecordCursor->nodePayloadField0C = ownerListNode->worldYQ12;
        objectRecordCursor->worldRotationAngle2 = modelRotationAngle;
        objectRecordCursor->reserved14_1F[0] = 0;
        objectRecordCursor->reserved14_1F[1] = 0;
        objectRecordCursor->reserved14_1F[2] = 0;
        objectRecordCursor->reserved14_1F[3] = 0;
        objectRecordCursor->reserved14_1F[4] = 0;
        objectRecordCursor->reserved14_1F[5] = 0;
        objectRecordCursor->reserved14_1F[6] = 0;
        objectRecordCursor->reserved14_1F[7] = 0;
        objectRecordCursor->reserved14_1F[8] = 0;
        objectRecordCursor->reserved14_1F[9] = 0;
        objectRecordCursor->reserved14_1F[10] = 0;
        objectRecordCursor->reserved14_1F[0xb] = 0;
        objectRecordCursor = objectRecordCursor + 1;
      }
    }
    *(uint32_t *)(source + 0x2e0) =
         saveWorldView->fieldRegionOriginWorldXQ12 & 0xffffU |
         saveWorldView->fieldRegionOriginWorldYQ12 << 0x10;
    *(WorldFieldDimension *)(source + 0x2f0) =
         (saveWorldView->worldRuntime).fieldRegion.regionWidth & 0xffff |
         (saveWorldView->worldRuntime).fieldRegion.regionHeight << 0x10;
    bookmarkPackedHeadingPitch = g_LevelCameraBookmark1PackedHeadingLow16PitchHigh16;
    bookmarkMagnitude = g_LevelCameraBookmark1PositionMagnitudeQ12;
    bookmarkZ = g_LevelCameraBookmark1PositionZQ12;
    statusOrFieldValue = g_LevelCameraBookmark1PositionYQ12;
    *(uint32_t *)(source + 0x200) = g_LevelCameraBookmark1PositionXQ12;
    *(uint32_t *)(source + 0x204) = statusOrFieldValue;
    *(uint32_t *)(source + 0x208) = bookmarkZ;
    *(uint32_t *)(source + 0x20c) = bookmarkMagnitude;
    *(uint32_t *)(source + 0x210) = bookmarkPackedHeadingPitch;
    bookmarkPackedHeadingPitch = g_LevelCameraBookmark2PackedHeadingLow16PitchHigh16;
    bookmarkMagnitude = g_LevelCameraBookmark2PositionMagnitudeQ12;
    bookmarkZ = g_LevelCameraBookmark2PositionZQ12;
    statusOrFieldValue = g_LevelCameraBookmark2PositionYQ12;
    *(uint32_t *)(source + 0x220) = g_LevelCameraBookmark2PositionXQ12;
    *(uint32_t *)(source + 0x224) = statusOrFieldValue;
    *(uint32_t *)(source + 0x228) = bookmarkZ;
    *(uint32_t *)(source + 0x22c) = bookmarkMagnitude;
    *(uint32_t *)(source + 0x230) = bookmarkPackedHeadingPitch;
    bookmarkPackedHeadingPitch = g_LevelCameraBookmark3PackedHeadingLow16PitchHigh16;
    bookmarkMagnitude = g_LevelCameraBookmark3PositionMagnitudeQ12;
    bookmarkZ = g_LevelCameraBookmark3PositionZQ12;
    statusOrFieldValue = g_LevelCameraBookmark3PositionYQ12;
    *(uint32_t *)(source + 0x240) = g_LevelCameraBookmark3PositionXQ12;
    *(uint32_t *)(source + 0x244) = statusOrFieldValue;
    *(uint32_t *)(source + 0x248) = bookmarkZ;
    *(uint32_t *)(source + 0x24c) = bookmarkMagnitude;
    *(uint32_t *)(source + 0x250) = bookmarkPackedHeadingPitch;
    bookmarkPackedHeadingPitch = g_LevelCameraBookmark4PackedHeadingLow16PitchHigh16;
    bookmarkMagnitude = g_LevelCameraBookmark4PositionMagnitudeQ12;
    bookmarkZ = g_LevelCameraBookmark4PositionZQ12;
    statusOrFieldValue = g_LevelCameraBookmark4PositionYQ12;
    *(uint32_t *)(source + 0x260) = g_LevelCameraBookmark4PositionXQ12;
    *(uint32_t *)(source + 0x264) = statusOrFieldValue;
    *(uint32_t *)(source + 0x268) = bookmarkZ;
    *(uint32_t *)(source + 0x26c) = bookmarkMagnitude;
    *(uint32_t *)(source + 0x270) = bookmarkPackedHeadingPitch;
    bookmarkPackedHeadingPitch = g_LevelCameraBookmark5PackedHeadingLow16PitchHigh16;
    bookmarkMagnitude = g_LevelCameraBookmark5PositionMagnitudeQ12;
    bookmarkZ = g_LevelCameraBookmark5PositionZQ12;
    statusOrFieldValue = g_LevelCameraBookmark5PositionYQ12;
    *(uint32_t *)(source + 0x280) = g_LevelCameraBookmark5PositionXQ12;
    *(uint32_t *)(source + 0x284) = statusOrFieldValue;
    *(uint32_t *)(source + 0x288) = bookmarkZ;
    *(uint32_t *)(source + 0x28c) = bookmarkMagnitude;
    *(uint32_t *)(source + 0x290) = bookmarkPackedHeadingPitch;
    bookmarkPackedHeadingPitch = g_LevelCameraBookmark6PackedHeadingLow16PitchHigh16;
    bookmarkMagnitude = g_LevelCameraBookmark6PositionMagnitudeQ12;
    bookmarkZ = g_LevelCameraBookmark6PositionZQ12;
    statusOrFieldValue = g_LevelCameraBookmark6PositionYQ12;
    *(uint32_t *)(source + 0x2a0) = g_LevelCameraBookmark6PositionXQ12;
    *(uint32_t *)(source + 0x2a4) = statusOrFieldValue;
    *(uint32_t *)(source + 0x2a8) = bookmarkZ;
    *(uint32_t *)(source + 0x2ac) = bookmarkMagnitude;
    *(uint32_t *)(source + 0x2b0) = bookmarkPackedHeadingPitch;
    bookmarkPackedHeadingPitch = g_LevelCameraBookmark7PackedHeadingLow16PitchHigh16;
    bookmarkMagnitude = g_LevelCameraBookmark7PositionMagnitudeQ12;
    bookmarkZ = g_LevelCameraBookmark7PositionZQ12;
    statusOrFieldValue = g_LevelCameraBookmark7PositionYQ12;
    *(uint32_t *)(source + 0x2c0) = g_LevelCameraBookmark7PositionXQ12;
    *(uint32_t *)(source + 0x2c4) = statusOrFieldValue;
    *(uint32_t *)(source + 0x2c8) = bookmarkZ;
    *(uint32_t *)(source + 0x2cc) = bookmarkMagnitude;
    *(uint32_t *)(source + 0x2d0) = bookmarkPackedHeadingPitch;
    statusResult = FileSystem_WriteBufferToPathCf
                      (*(FileIoByteCount *)(source + 4),source,g_LevelEndingMovieSourcePath);
    statusOrFieldValue = statusResult.valueOrError;
    if (!statusResult.carry) {
      return THANDOR_BITCAST(uint64_t, StatusValueEaxCf5, ((THANDOR_BITCAST(StatusValueEaxCf5, uint64_t, statusResult) & 0xFFFFFFFFFFull) & 0xffffffff));
    }
  }
  statusResult.carry = true;
  statusResult.valueOrError = statusOrFieldValue;
  return statusResult;
}


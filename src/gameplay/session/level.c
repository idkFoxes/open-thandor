/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/src/gameplay/session/level.c
 * Reverse engineering by idkFoxes 2026
 */

#include <thandor/gameplay/session/level.h>

/* Implementation ownership: gameplay/session/level. */

/* Address: 0x00531080.
   Ownership: gameplay/session/level.
   Purpose: EAX returns the prepared movie path; CF reports failure.
   Cross-module calls: WidePath_SetExtensionCode [core/text/path], Package_SetLastErrorPath
   [assets/package/runtime].
*/
EndingMoviePathEaxCf5 __thandor_eax_cf_preserve_ecx_edx
LevelAsset_PrepareEndingMoviePathCf(word *currentLevelPath,LevelAssetHeader *asset)

{
  int iVar1;
  int iVar2;
  sdword arg4;
  word *pwVar3;
  EndingMoviePathEaxCf5 EVar4;
  EndingMoviePathEaxCf5 EVar5;
  byte *path;
  
  if (((asset->common).magic == ASSET_MAGIC_LEV) &&
     ((asset->common).converterVersion == PCK_CONVERTER_LEV_00070001)) {
    path = (asset->common).buildMetadata.assetRelativeAddressAnchor28 +
           ((asset->pathOffsets).endingMovieBasePathOffset - 0x28);
    iVar2 = 0x80;
    iVar1 = *(int *)((AssetProducerSourceNames *)(path + 8))->producerName;
    arg4 = 0;
    if (iVar1 == 0xfc0077) {
      arg4 = 2;
    }
    else if (iVar1 == 0x690065) {
      arg4 = 3;
    }
    else if (iVar1 == 0x61006c) {
      arg4 = 4;
    }
    WidePath_SetExtensionCode(0x6d6c66,(word *)path);
    (*g_WideNumberFormatUtf16)
              (WIDE_FORMAT_PAD_WITH_ZERO,0,4,1,arg4,(word *)(u_flm_ende0000_flm_0050df06 + 8));
    pwVar3 = g_LevelEndingMovieSourcePath;
    for (; iVar2 != 0; iVar2 = iVar2 + -1) {
      *(undefined4 *)pwVar3 = *(undefined4 *)currentLevelPath;
      currentLevelPath = currentLevelPath + 2;
      pwVar3 = pwVar3 + 2;
    }
    EVar4.carry = false;
    EVar4.moviePath = (word *)path;
    return EVar4;
  }
  Package_SetLastErrorPath(currentLevelPath);
  EVar5.carry = true;
  EVar5.moviePath = (word *)0x39;
  return EVar5;
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
  LevelPlayerSlotByteOffset32 LVar1;
  InGameLevelConditionStorageView800 *pIVar2;
  void **loadedResourcePointerArray;
  void *resultOrPointer;
  TechnologyAsset *loadedTechnologyAsset;
  dword soundDirectoryRecordSizeBytes;
  void *shrinkResultOrError;
  uint uVar3;
  WorldWorkspaceElementCount WVar4;
  PckOutputCapacityBytes outputCapacityBytes;
  dword dVar5;
  uint uVar6;
  uint uVar7;
  LevelAssetRecordCount LVar8;
  word *graphicsBasePath;
  word *mutableBasePath;
  int iVar9;
  uint uVar10;
  ArmyAssetRecordPrefix **ppAVar11;
  uint uVar12;
  uint uVar13;
  uint uVar14;
  LevelAssetRuntimeImagePrefix370 *pLVar15;
  byte *pbVar16;
  word *pwVar17;
  dword *pdVar18;
  WorldOwnerListNode100 *worldNode1;
  uint uVar19;
  ArenaAllocEaxCf5 AVar20;
  PackageLoadEntryEaxCf5 PVar21;
  StatusValueEaxCf5 SVar22;
  ArmyRuntimeInitEaxCf5 AVar23;
  ArmyRuntimeCreateEaxCf5 AVar24;
  ArenaShrinkEaxCf5 AVar25;
  SpatialSoundSlotEaxCf5 SVar26;
  SoundCreateSampleVoiceSetEaxCf5 SVar27;
  ModelDefinitionLookupEaxCf5 MVar28;
  InGameLevelDefaultLoadEaxCf5 IVar29;
  InGameLevelDefaultLoadEaxCf5 IVar30;
  ArenaLargestAllocationEaxEcxCf9 AVar31;
  PackageFindEntryEaxEcxCf9 PVar32;
  FileSystemEnumerationEaxEcxCf9 FVar33;
  ResourceLoadEaxEcxCf9 RVar34;
  WorldRuntimeContext *pWVar35;
  ArmyAssetRecordPrefix *local_2c;
  ArmyAssetRecordPrefix *local_28;
  ArmyAssetRecordPrefix *local_24;
  ArmyAssetRecordPrefix *local_20;
  word *soundDirectoryPathCursor;
  ArmyAssetRecordPrefix *armyDefinition1;
  ArmyAssetRecordPrefix *armyDefinition2;
  ArmyAssetRecordPrefix *armyDefinition3;
  ArmyAssetRecordPrefix *armyDefinition4;
  WorldRuntimeContext *worldContext1;
  
  AVar20 = (*g_MemoryApi.alloc)(0x800);
  loadedResourcePointerArray = (void **)AVar20.eax;
  resultOrPointer = loadedResourcePointerArray;
  if (!AVar20.carry) {
    g_InGameLoadedResourcePointerCount = 0;
    resultOrPointer = (void *)0x39;
    g_InGameLoadedResourcePointers = loadedResourcePointerArray;
    Package_SetLastErrorPath(g_LevelEndingMovieSourcePath);
    if (((levelImage->header).common.magic == ASSET_MAGIC_LEV) &&
       ((levelImage->header).common.converterVersion == PCK_CONVERTER_LEV_00070001)) {
      uVar3 = (levelImage->header).resourceTables.runtimePrefixByteSizeAndInitialArmyPlacementOffset
      ;
      AVar20 = (*g_MemoryApi.alloc)(uVar3);
      resultOrPointer = (void *)AVar20.eax;
      if (!AVar20.carry) {
        pLVar15 = levelImage;
        g_InGameLevelRuntimeGlobalBlock.conditionStorage = resultOrPointer;
        for (uVar3 = uVar3 >> 2; uVar3 != 0; uVar3 = uVar3 - 1) {
          (((InGameLevelConditionStorageView800 *)resultOrPointer)->levelImage).header.common.magic
               = (pLVar15->header).common.magic;
          pLVar15 = (LevelAssetRuntimeImagePrefix370 *)&(pLVar15->header).common.allocationSizeBytes
          ;
          resultOrPointer =
               &(((InGameLevelConditionStorageView800 *)resultOrPointer)->levelImage).header.common.
                allocationSizeBytes;
        }
        g_InGameLevelTitleTextResourceIndex = (levelImage->header).titleTextResourceIndex;
        g_InGameLevelCampaignAssociationIndex = (levelImage->header).campaignAssociationIndex;
        pbVar16 = (levelImage->header).common.buildMetadata.assetRelativeAddressAnchor28 +
                  ((levelImage->header).pathOffsets.technologyPathOffset - 0x28);
        WidePath_SetExtensionCode(0x636574,(word *)pbVar16);
        PVar21 = Package_LoadEntry((word *)pbVar16);
        loadedTechnologyAsset = PVar21.bufferOrError;
        resultOrPointer = loadedTechnologyAsset;
        if (((!PVar21.carry) &&
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
          pwVar17 = (word *)((levelImage->header).common.buildMetadata.assetRelativeAddressAnchor28
                            + ((levelImage->header).resourceTables.effectAssetPathTableOffset - 0x28
                              ));
          for (LVar8 = (levelImage->header).resourceTables.effectAssetPathCount; LVar8 != 0;
              LVar8 = LVar8 - 1) {
            WidePath_SetExtensionCode(0x666665,pwVar17);
            resultOrPointer = (InGameLevelConditionStorageView800 *)0x3a;
            if (0x1ff < g_InGameLoadedResourcePointerCount)
            goto 
            InGameLevelRuntime_LoadResourcesAfterDefaultResetCf_ReturnCurrentResourceLoadFailureStatus
            ;
            PVar21 = Package_LoadEntry(pwVar17);
            resultOrPointer = PVar21.bufferOrError;
            if (PVar21.carry)
            goto 
            InGameLevelRuntime_LoadResourcesAfterDefaultResetCf_ReturnCurrentResourceLoadFailureStatus
            ;
            *loadedResourcePointerArray = resultOrPointer;
            g_InGameLoadedResourcePointerCount = g_InGameLoadedResourcePointerCount + 1;
            loadedResourcePointerArray = loadedResourcePointerArray + 1;
            SVar22 = EffectAsset_PrepareEntries(resultOrPointer);
            resultOrPointer = (void *)SVar22.valueOrError;
            if (SVar22.carry)
            goto 
            InGameLevelRuntime_LoadResourcesAfterDefaultResetCf_ReturnCurrentResourceLoadFailureStatus
            ;
            MoviePlayback_AdvanceScheduledFrameAndTick();
            pwVar17 = pwVar17 + 0x20;
          }
          pwVar17 = (word *)((levelImage->header).common.buildMetadata.assetRelativeAddressAnchor28
                            + ((levelImage->header).resourceTables.shotAssetPathTableOffset - 0x28))
          ;
          for (LVar8 = (levelImage->header).resourceTables.shotAssetPathCount; LVar8 != 0;
              LVar8 = LVar8 - 1) {
            WidePath_SetExtensionCode(0x746873,pwVar17);
            resultOrPointer = (InGameLevelConditionStorageView800 *)0x3a;
            if (0x1ff < g_InGameLoadedResourcePointerCount)
            goto 
            InGameLevelRuntime_LoadResourcesAfterDefaultResetCf_ReturnCurrentResourceLoadFailureStatus
            ;
            PVar21 = Package_LoadEntry(pwVar17);
            resultOrPointer = PVar21.bufferOrError;
            if (PVar21.carry)
            goto 
            InGameLevelRuntime_LoadResourcesAfterDefaultResetCf_ReturnCurrentResourceLoadFailureStatus
            ;
            *loadedResourcePointerArray = resultOrPointer;
            g_InGameLoadedResourcePointerCount = g_InGameLoadedResourcePointerCount + 1;
            loadedResourcePointerArray = loadedResourcePointerArray + 1;
            SVar22 = ShotAsset_PrepareEntries(resultOrPointer);
            resultOrPointer = (void *)SVar22.valueOrError;
            if (SVar22.carry)
            goto 
            InGameLevelRuntime_LoadResourcesAfterDefaultResetCf_ReturnCurrentResourceLoadFailureStatus
            ;
            MoviePlayback_AdvanceScheduledFrameAndTick();
            pwVar17 = pwVar17 + 0x20;
          }
          SVar22 = EffectDefinitions_ResolveCrossReferences();
          resultOrPointer = (void *)SVar22.valueOrError;
          if (!SVar22.carry) {
            pwVar17 = (word *)((levelImage->header).common.buildMetadata.
                               assetRelativeAddressAnchor28 +
                              ((levelImage->header).resourceTables.modelAssetPathTableOffset - 0x28)
                              );
            for (LVar8 = (levelImage->header).resourceTables.modelAssetPathCount; LVar8 != 0;
                LVar8 = LVar8 - 1) {
              WidePath_SetExtensionCode(0x6c646d,pwVar17);
              resultOrPointer = (InGameLevelConditionStorageView800 *)0x3a;
              if (0x1ff < g_InGameLoadedResourcePointerCount)
              goto 
              InGameLevelRuntime_LoadResourcesAfterDefaultResetCf_ReturnCurrentResourceLoadFailureStatus
              ;
              PVar21 = Package_LoadEntry(pwVar17);
              resultOrPointer = PVar21.bufferOrError;
              if (PVar21.carry)
              goto 
              InGameLevelRuntime_LoadResourcesAfterDefaultResetCf_ReturnCurrentResourceLoadFailureStatus
              ;
              *loadedResourcePointerArray = resultOrPointer;
              g_InGameLoadedResourcePointerCount = g_InGameLoadedResourcePointerCount + 1;
              loadedResourcePointerArray = loadedResourcePointerArray + 1;
              SVar22 = ModelAsset_PrepareRecords(resultOrPointer);
              resultOrPointer = (void *)SVar22.valueOrError;
              if (SVar22.carry)
              goto 
              InGameLevelRuntime_LoadResourcesAfterDefaultResetCf_ReturnCurrentResourceLoadFailureStatus
              ;
              MoviePlayback_AdvanceScheduledFrameAndTick();
              pwVar17 = pwVar17 + 0x20;
            }
            pwVar17 = (word *)((levelImage->header).common.buildMetadata.
                               assetRelativeAddressAnchor28 +
                              ((levelImage->header).resourceTables.armyAssetPathTableOffset - 0x28))
            ;
            for (LVar8 = (levelImage->header).resourceTables.armyAssetPathCount; LVar8 != 0;
                LVar8 = LVar8 - 1) {
              WidePath_SetExtensionCode(0x6d7261,pwVar17);
              resultOrPointer = (InGameLevelConditionStorageView800 *)0x3a;
              if (0x1ff < g_InGameLoadedResourcePointerCount)
              goto 
              InGameLevelRuntime_LoadResourcesAfterDefaultResetCf_ReturnCurrentResourceLoadFailureStatus
              ;
              PVar21 = Package_LoadEntry(pwVar17);
              resultOrPointer = PVar21.bufferOrError;
              if (PVar21.carry)
              goto 
              InGameLevelRuntime_LoadResourcesAfterDefaultResetCf_ReturnCurrentResourceLoadFailureStatus
              ;
              *loadedResourcePointerArray = resultOrPointer;
              g_InGameLoadedResourcePointerCount = g_InGameLoadedResourcePointerCount + 1;
              loadedResourcePointerArray = loadedResourcePointerArray + 1;
              SVar22 = ArmyAsset_PrepareRecords(resultOrPointer);
              resultOrPointer = (void *)SVar22.valueOrError;
              if (SVar22.carry)
              goto 
              InGameLevelRuntime_LoadResourcesAfterDefaultResetCf_ReturnCurrentResourceLoadFailureStatus
              ;
              MoviePlayback_AdvanceScheduledFrameAndTick();
              pwVar17 = pwVar17 + 0x20;
            }
            resultOrPointer = (InGameLevelConditionStorageView800 *)0x3a;
            if (g_InGameLoadedResourcePointerCount < 0x200) {
              g_MoviePlaybackBaseFrameGroup = 1;
              g_MoviePlaybackScheduleCounter = 0;
              g_MoviePlaybackScheduleSpan = 0x10000;
              SVar22 = TerrainVisualResources_LoadPrimary
                                 ((word *)((levelImage->header).common.buildMetadata.
                                           assetRelativeAddressAnchor28 +
                                          ((levelImage->header).pathOffsets.
                                           surfaceTextureBasePathOffset - 0x28)),
                                  (word *)((levelImage->header).common.buildMetadata.
                                           assetRelativeAddressAnchor28 +
                                          ((levelImage->header).pathOffsets.
                                           groundTextureBasePathOffset - 0x28)),
                                  (FieldGridAsset *)(levelImage->header).pathOffsets.levelPathOffset
                                 );
              resultOrPointer = (void *)SVar22.valueOrError;
              if (!SVar22.carry) {
                SVar22 = ShotDefinitions_ValidateTerrainMaterialReferences();
                resultOrPointer = (void *)SVar22.valueOrError;
                if (!SVar22.carry) {
                  g_MoviePlaybackBaseFrameGroup = 2;
                  g_MoviePlaybackScheduleCounter = 0;
                  g_MoviePlaybackScheduleSpan = 0x10000;
                  SVar22 = ModelRuntimePool_Init();
                  resultOrPointer = (void *)SVar22.valueOrError;
                  if (!SVar22.carry) {
                    AVar23 = ArmyRuntime_InitializePoolAndGraphicsCf(worldRuntime,graphicsBasePath);
                    resultOrPointer = (void *)AVar23.errorOrValue;
                    if (!AVar23.carry) {
                      g_MoviePlaybackBaseFrameGroup = 3;
                      g_MoviePlaybackScheduleCounter = 0;
                      g_MoviePlaybackScheduleSpan = 4;
                      SVar22 = ShotRuntime_InitGraphicsResources
                                         ((word *)((levelImage->header).common.buildMetadata.
                                                   assetRelativeAddressAnchor28 +
                                                  ((levelImage->header).pathOffsets.
                                                   shotTextureBasePathOffset - 0x28)));
                      resultOrPointer = (void *)SVar22.valueOrError;
                      if (!SVar22.carry) {
                        g_MoviePlaybackBaseFrameGroup = 4;
                        g_MoviePlaybackScheduleCounter = 0;
                        g_MoviePlaybackScheduleSpan = 4;
                        SVar22 = EffectRuntime_InitGraphicsResources(mutableBasePath);
                        resultOrPointer = (void *)SVar22.valueOrError;
                        if (!SVar22.carry) {
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
                          iVar9 = worldRuntime->activeFactionRuntimeIndex;
                          MoviePlayback_AdvanceScheduledFrameAndTick();
                          LVar1 = g_InGameLevelRuntimeGlobalBlock.playerSlotByteOffsets[iVar9 + -1];
                          WorldRuntime_AttachFieldGridAsset
                                    ((FieldGridAsset *)
                                     (levelImage->header).pathOffsets.levelPathOffset,worldRuntime);
                          MoviePlayback_AdvanceScheduledFrameAndTick();
                          uVar3 = *(uint *)((int)&levelImage->playerSlots[0].
                                                  packedHeadingLow16PitchHigh16 + LVar1);
                          WorldRuntime_SetPosition60AndDistanceFromPosition80
                                    (*(Q12 *)((int)&levelImage->playerSlots[0].startCameraZQ12 +
                                             LVar1),
                                     *(Q12 *)((int)&levelImage->playerSlots[0].startCameraYQ12 +
                                             LVar1),
                                     *(Q12 *)((int)&levelImage->playerSlots[0].startCameraXQ12 +
                                             LVar1),worldRuntime);
                          WorldRuntime_SetMotionParameters6CThrough78Clamped
                                    (2,(int)uVar3 >> 0x10,uVar3 & 0xffff,
                                     *(UQ12 *)((int)&levelImage->playerSlots[0].
                                                     startCameraMagnitudeQ12 + LVar1),worldRuntime);
                          uVar3 = (levelImage->runtimeTail2E0).packedFieldRegionOriginYHigh16XLow16;
                          WorldRuntime_RecomputeMotionEndpointAgainstFieldSurface(worldRuntime);
                          uVar19 = uVar3;
                          WorldRuntime_CommitScalar7CFrom8C(worldRuntime);
                          uVar10 = (levelImage->runtimeTail2E0).
                                   packedFieldRegionHeightHigh16WidthLow16;
                          WorldRuntime_RecomputeFieldRegionNormalsAndLighting
                                    ((int)uVar10 >> 0x10,uVar10 & 0xffff,(int)uVar19 >> 0x10,
                                     uVar3 & 0xffff,worldRuntime);
                          LVar8 = (levelImage->header).initialArmyPlacementRecordCount;
                          pwVar17 = (word *)((levelImage->header).common.buildMetadata.
                                             assetRelativeAddressAnchor28 +
                                            ((levelImage->header).resourceTables.
                                             runtimePrefixByteSizeAndInitialArmyPlacementOffset -
                                            0x28));
                          MoviePlayback_AdvanceScheduledFrameAndTick();
                          for (; LVar8 != 0; LVar8 = LVar8 - 1) {
                            if (g_GameFactionRuntimeImage.tail.factionLifecycleStates
                                [*(PckArmyAssetIdCatalog *)(pwVar17 + 2)] ==
                                FACTION_RUNTIME_LIFECYCLE_ACTIVE) {
                              AVar24 = ArmyRuntime_CreateInstanceFromAssetCf
                                                 (6,*(PckArmyAssetIdCatalog *)(pwVar17 + 8),
                                                  *(PckArmyAssetIdCatalog *)(pwVar17 + 6),
                                                  *(Q12 *)((AssetProducerSourceNames *)(pwVar17 + 4)
                                                          )->producerName,
                                                  *(PckArmyAssetIdCatalog *)(pwVar17 + 2),
                                                  *(PckArmyAssetIdCatalog *)pwVar17,worldRuntime);
                              resultOrPointer = (void *)AVar24.eax;
                              if (AVar24.carry)
                              goto 
                              InGameLevelRuntime_LoadResourcesAfterDefaultResetCf_ReturnCurrentResourceLoadFailureStatus
                              ;
                            }
                            pwVar17 = pwVar17 + 0x10;
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
                          pdVar18 = worldRuntime->dwordArray;
                          for (WVar4 = worldRuntime->dwordArrayCount; WVar4 != 0; WVar4 = WVar4 - 1)
                          {
                            *pdVar18 = 0;
                            pdVar18 = pdVar18 + 1;
                          }
                          pbVar16 = (levelImage->header).common.buildMetadata.
                                    assetRelativeAddressAnchor28 +
                                    ((levelImage->header).pathOffsets.soundBasePathOffset - 0x28);
                          WidePath_SetExtensionCode(0x6d6173,(word *)pbVar16);
                          WidePath_SplitParentAndLeaf
                                    ((word *)&g_InGameLevelSoundLeafOrCombinedPathScratchUtf16,
                                     (word *)&g_InGameLevelSoundParentDirectoryScratchUtf16,
                                     (word *)pbVar16);
                          AVar31 = (*g_MemoryApi.allocLargestFreeBlock)();
                          outputCapacityBytes = AVar31.blockSizeOrSentinel;
                          resultOrPointer = (void *)AVar31.allocationOrError;
                          if (!AVar31.carry) {
                            PVar32 = Package_FindEntry(outputCapacityBytes,resultOrPointer,
                                                       (word *)pbVar16,g_SoundPackageHandle);
                            uVar3 = PVar32.matchCount;
                            soundDirectoryRecordSizeBytes = PVar32.recordSizeOrError;
                            if (PVar32.carry) {
                              FVar33 = (*g_FileSystemEnumerateDirectoryOrVolumeEntriesCf)
                                                 (FILESYSTEM_ENUMERATE_FILES,0xffffffff,
                                                  outputCapacityBytes,resultOrPointer,pbVar16);
                              uVar3 = FVar33.entryCount;
                              soundDirectoryRecordSizeBytes = FVar33.recordSizeBytes;
                            }
                            shrinkResultOrError = (void *)soundDirectoryRecordSizeBytes;
                            if (&stack0xffffffd0 < (undefined1 *)0xfffffffc) {
                              g_MoviePlaybackBaseFrameGroup = 6;
                              g_MoviePlaybackScheduleCounter = 0;
                              g_MoviePlaybackScheduleSpan = uVar3;
                              AVar25 = (*g_MemoryApi.shrinkInPlace)
                                                 (soundDirectoryRecordSizeBytes * uVar3,
                                                  resultOrPointer);
                              shrinkResultOrError = (void *)AVar25.scratchOrError;
                              if (!AVar25.carry) {
                                worldContext1 = worldRuntime;
                                soundDirectoryPathCursor = resultOrPointer;
                                pIVar2 = g_InGameLevelRuntimeGlobalBlock.conditionStorage;
                                if (worldRuntime->dwordArrayCount < uVar3) {
                                  uVar3 = worldRuntime->dwordArrayCount;
                                }
                                do {
                                  g_InGameLevelRuntimeGlobalBlock.conditionStorage = pIVar2;
                                  if (uVar3 == 0) {
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
                                    if ((pIVar2->levelImage).runtimeTail2E0.effectSampleNumbers[0]
                                        != 0) {
                                      (*g_WideNumberFormatUtf16)
                                                (WIDE_FORMAT_PAD_WITH_ZERO,0,2,1,
                                                 (pIVar2->levelImage).runtimeTail2E0.
                                                 effectSampleNumbers[0],
                                                 (word *)(u_sound_level00_sam_0050df6c + 0xb));
                                      RVar34 = Resource_Load((word *)u_sound_level00_sam_0050df6c);
                                      if (!RVar34.carry) {
                                        SVar27 = (*g_SoundCreateSampleVoiceSet)
                                                           ((SoundSampleAsset *)RVar34.eax);
                                        if (!SVar27.carry) {
                                          g_InGameLevelEffectVoiceSet0 = SVar27.eax;
                                        }
                                        Resource_Release((SoundSampleAsset *)RVar34.eax);
                                      }
                                    }
                                    if ((pIVar2->levelImage).runtimeTail2E0.effectSampleNumbers[1]
                                        != 0) {
                                      (*g_WideNumberFormatUtf16)
                                                (WIDE_FORMAT_PAD_WITH_ZERO,0,2,1,
                                                 (pIVar2->levelImage).runtimeTail2E0.
                                                 effectSampleNumbers[1],
                                                 (word *)(u_sound_level00_sam_0050df6c + 0xb));
                                      RVar34 = Resource_Load((word *)u_sound_level00_sam_0050df6c);
                                      if (!RVar34.carry) {
                                        SVar27 = (*g_SoundCreateSampleVoiceSet)
                                                           ((SoundSampleAsset *)RVar34.eax);
                                        if (!SVar27.carry) {
                                          g_InGameLevelEffectVoiceSet1 = SVar27.eax;
                                        }
                                        Resource_Release((SoundSampleAsset *)RVar34.eax);
                                      }
                                    }
                                    if ((pIVar2->levelImage).runtimeTail2E0.effectSampleNumbers[2]
                                        != 0) {
                                      (*g_WideNumberFormatUtf16)
                                                (WIDE_FORMAT_PAD_WITH_ZERO,0,2,1,
                                                 (pIVar2->levelImage).runtimeTail2E0.
                                                 effectSampleNumbers[2],
                                                 (word *)(u_sound_level00_sam_0050df6c + 0xb));
                                      RVar34 = Resource_Load((word *)u_sound_level00_sam_0050df6c);
                                      if (!RVar34.carry) {
                                        SVar27 = (*g_SoundCreateSampleVoiceSet)
                                                           ((SoundSampleAsset *)RVar34.eax);
                                        if (!SVar27.carry) {
                                          g_InGameLevelEffectVoiceSet2 = SVar27.eax;
                                        }
                                        Resource_Release((SoundSampleAsset *)RVar34.eax);
                                      }
                                    }
                                    if ((pIVar2->levelImage).runtimeTail2E0.effectSampleNumbers[3]
                                        != 0) {
                                      (*g_WideNumberFormatUtf16)
                                                (WIDE_FORMAT_PAD_WITH_ZERO,0,2,1,
                                                 (pIVar2->levelImage).runtimeTail2E0.
                                                 effectSampleNumbers[3],
                                                 (word *)(u_sound_level00_sam_0050df6c + 0xb));
                                      RVar34 = Resource_Load((word *)u_sound_level00_sam_0050df6c);
                                      if (!RVar34.carry) {
                                        SVar27 = (*g_SoundCreateSampleVoiceSet)
                                                           ((SoundSampleAsset *)RVar34.eax);
                                        if (!SVar27.carry) {
                                          g_InGameLevelEffectVoiceSet3 = SVar27.eax;
                                        }
                                        Resource_Release((SoundSampleAsset *)RVar34.eax);
                                      }
                                    }
                                    if ((pIVar2->levelImage).runtimeTail2E0.musicSampleNumbers[0] !=
                                        0) {
                                      (*g_WideNumberFormatUtf16)
                                                (WIDE_FORMAT_PAD_WITH_ZERO,0,2,1,
                                                 (pIVar2->levelImage).runtimeTail2E0.
                                                 musicSampleNumbers[0],
                                                 (word *)(u_sound_music00_sam_0050df90 + 0xb));
                                      RVar34 = Resource_Load((word *)u_sound_music00_sam_0050df90);
                                      if (!RVar34.carry) {
                                        SVar27 = (*g_SoundCreateSampleVoiceSet)
                                                           ((SoundSampleAsset *)RVar34.eax);
                                        if (!SVar27.carry) {
                                          g_InGameLevelMusicVoiceSet0 = SVar27.eax;
                                        }
                                        Resource_Release((SoundSampleAsset *)RVar34.eax);
                                      }
                                    }
                                    if ((pIVar2->levelImage).runtimeTail2E0.musicSampleNumbers[1] !=
                                        0) {
                                      (*g_WideNumberFormatUtf16)
                                                (WIDE_FORMAT_PAD_WITH_ZERO,0,2,1,
                                                 (pIVar2->levelImage).runtimeTail2E0.
                                                 musicSampleNumbers[1],
                                                 (word *)(u_sound_music00_sam_0050df90 + 0xb));
                                      RVar34 = Resource_Load((word *)u_sound_music00_sam_0050df90);
                                      if (!RVar34.carry) {
                                        SVar27 = (*g_SoundCreateSampleVoiceSet)
                                                           ((SoundSampleAsset *)RVar34.eax);
                                        if (!SVar27.carry) {
                                          g_InGameLevelMusicVoiceSet1 = SVar27.eax;
                                        }
                                        Resource_Release((SoundSampleAsset *)RVar34.eax);
                                      }
                                    }
                                    if ((pIVar2->levelImage).runtimeTail2E0.musicSampleNumbers[2] !=
                                        0) {
                                      (*g_WideNumberFormatUtf16)
                                                (WIDE_FORMAT_PAD_WITH_ZERO,0,2,1,
                                                 (pIVar2->levelImage).runtimeTail2E0.
                                                 musicSampleNumbers[2],
                                                 (word *)(u_sound_music00_sam_0050df90 + 0xb));
                                      RVar34 = Resource_Load((word *)u_sound_music00_sam_0050df90);
                                      if (!RVar34.carry) {
                                        SVar27 = (*g_SoundCreateSampleVoiceSet)
                                                           ((SoundSampleAsset *)RVar34.eax);
                                        if (!SVar27.carry) {
                                          g_InGameLevelMusicVoiceSet2 = SVar27.eax;
                                        }
                                        Resource_Release((SoundSampleAsset *)RVar34.eax);
                                      }
                                    }
                                    if ((pIVar2->levelImage).runtimeTail2E0.musicSampleNumbers[3] !=
                                        0) {
                                      (*g_WideNumberFormatUtf16)
                                                (WIDE_FORMAT_PAD_WITH_ZERO,0,2,1,
                                                 (pIVar2->levelImage).runtimeTail2E0.
                                                 musicSampleNumbers[3],
                                                 (word *)(u_sound_music00_sam_0050df90 + 0xb));
                                      RVar34 = Resource_Load((word *)u_sound_music00_sam_0050df90);
                                      if (!RVar34.carry) {
                                        SVar27 = (*g_SoundCreateSampleVoiceSet)
                                                           ((SoundSampleAsset *)RVar34.eax);
                                        if (!SVar27.carry) {
                                          g_InGameLevelMusicVoiceSet3 = SVar27.eax;
                                        }
                                        Resource_Release((SoundSampleAsset *)RVar34.eax);
                                      }
                                    }
                                    ppAVar11 = g_ArmyAssetRecordRegistry;
                                    iVar9 = 0x300;
                                    local_20 = (ArmyAssetRecordPrefix *)0x0;
                                    local_24 = (ArmyAssetRecordPrefix *)0x0;
                                    do {
                                      armyDefinition1 = *ppAVar11;
                                      armyDefinition3 = local_2c;
                                      armyDefinition2 = local_28;
                                      armyDefinition4 = local_20;
                                      if ((armyDefinition1 != (ArmyAssetRecordPrefix *)0x0) &&
                                         ((armyDefinition1[1].selectionDetailTemplateVariantIndex &
                                          1) != 0)) {
                                        MVar28 = ModelDefinitionRegistry_FindByIdWithErrorCf
                                                           (*(PckModelDefinitionIdCatalog *)
                                                             (armyDefinition1->
                                                              rootNodeOffsetOrPointer + 0x20));
                                        dVar5 = MVar28.modelDefinition[6].flags;
                                        armyDefinition4 = armyDefinition1;
                                        if (((dVar5 != 0xb) &&
                                            ((armyDefinition3 = armyDefinition1,
                                             armyDefinition4 = local_20, dVar5 != 0x10 &&
                                             (armyDefinition3 = local_2c, dVar5 == 0xe)))) &&
                                           (armyDefinition2 = armyDefinition1,
                                           MVar28.modelDefinition[0x10].byteSize == 0)) {
                                          armyDefinition2 = local_28;
                                          local_24 = armyDefinition1;
                                        }
                                      }
                                      local_20 = armyDefinition4;
                                      local_28 = armyDefinition2;
                                      local_2c = armyDefinition3;
                                      ppAVar11 = ppAVar11 + 1;
                                      iVar9 = iVar9 + -1;
                                    } while (iVar9 != 0);
                                    if ((local_20 != (ArmyAssetRecordPrefix *)0x0) &&
                                       (local_24 != (ArmyAssetRecordPrefix *)0x0)) {
                                      uVar3 = 1;
                                      worldNode1 = worldRuntime->ownerListHead;
                                      uVar10 = 0;
                                      if (worldNode1 != (WorldOwnerListNode100 *)0x0) {
                                        do {
                                          do {
                                            if ((worldNode1->ownerClassId ==
                                                 WORLD_OWNER_RUNTIME_MODEL) &&
                                               (uVar3 == *(uint *)(*(int *)((int)worldNode1->
                                                                                 runtimePayload + 8)
                                                                  + 0xc))) {
                                              iVar9 = *(int *)(*(int *)worldNode1->runtimePayload +
                                                              0x4c);
                                              if (iVar9 == 0x12) {
                                                uVar10 = uVar10 | 2;
                                              }
                                              else if (
                                                  g_ArmyRuntimeOrderHandlerMatrix11Columns24Classes.
                                                  classCommand[iVar9] ==
                                                  ArmyRuntime_ClassCommandHandlerGroupACf) {
                                                uVar10 = uVar10 | 1;
                                              }
                                            }
                                            worldNode1 = worldNode1->nextNode;
                                          } while (worldNode1 != (WorldOwnerListNode100 *)0x0);
                                          if (uVar10 == 2) {
                                            g_GameFactionRuntimeImage.records[uVar3].
                                            primaryArmyAssetPointersOrIds[0] = (dword)local_20;
                                            g_GameFactionRuntimeImage.records[uVar3].
                                            primaryArmyAssetPointersOrIds[1] = (dword)local_24;
                                            g_GameFactionRuntimeImage.records[uVar3].
                                            primaryArmyAssetPointersOrIds[2] = (dword)local_28;
                                            g_GameFactionRuntimeImage.records[uVar3].
                                            primaryArmyAssetPointersOrIds[3] = (dword)local_2c;
                                            g_GameFactionRuntimeImage.records[uVar3].
                                            primaryArmyAssetCount = 4;
                                          }
                                          worldNode1 = worldRuntime->ownerListHead;
                                          uVar3 = uVar3 + 1;
                                          uVar10 = 0;
                                        } while (uVar3 <= g_GameFactionRuntimeImage.tail.
                                                          activeFactionCount);
                                      }
                                    }
                                    uVar3 = ((g_InGameLevelRuntimeGlobalBlock.conditionStorage)->
                                            levelImage).runtimeTail2E0.relationUiFlags;
                                    uVar10 = ((g_InGameLevelRuntimeGlobalBlock.conditionStorage)->
                                             levelImage).runtimeTail2E0.
                                             relationState4FactionGroupMasks;
                                    uVar19 = ((g_InGameLevelRuntimeGlobalBlock.conditionStorage)->
                                             levelImage).runtimeTail2E0.
                                             relationState8FactionGroupMasks;
                                    g_GameFactionRuntimeImage.tail.relationUiFlags = uVar3;
                                    uVar6 = uVar10 & 0xff;
                                    while (uVar6 != 0) {
                                      uVar6 = 1;
                                      uVar12 = 0;
                                      do {
                                        uVar13 = uVar12;
                                        uVar14 = uVar6;
                                        uVar7 = uVar12;
                                        if ((uVar10 & uVar6) != 0) {
                                          while (uVar7 = uVar7 + 1, uVar3 = uVar12, uVar7 < 8) {
                                            uVar14 = uVar14 * 2;
                                            if ((uVar10 & uVar14) != 0) {
                                              GameFactionRuntime_ApplyPairwiseRelationTransition
                                                        (0x20,0x20,4,4,uVar7,uVar12);
                                            }
                                          }
                                        }
                                        uVar12 = uVar13 + 1;
                                        uVar6 = uVar6 * 2;
                                      } while (uVar12 != 8);
                                      uVar10 = uVar10 >> 8;
                                      uVar6 = uVar10;
                                    }
                                    uVar10 = uVar19 & 0xff;
                                    while (uVar10 != 0) {
                                      uVar10 = 1;
                                      uVar6 = 0;
                                      do {
                                        uVar14 = uVar6;
                                        uVar7 = uVar10;
                                        uVar12 = uVar6;
                                        if ((uVar19 & uVar10) != 0) {
                                          while (uVar12 = uVar12 + 1, uVar3 = uVar6, uVar12 < 8) {
                                            uVar7 = uVar7 * 2;
                                            if ((uVar19 & uVar7) != 0) {
                                              GameFactionRuntime_ApplyPairwiseRelationTransition
                                                        (0x20,0x20,8,8,uVar12,uVar6);
                                            }
                                          }
                                        }
                                        uVar6 = uVar14 + 1;
                                        uVar10 = uVar10 * 2;
                                      } while (uVar6 != 8);
                                      uVar19 = uVar19 >> 8;
                                      uVar10 = uVar19;
                                    }
                                    IVar29.carry = false;
                                    IVar29.errorOrValue = uVar3;
                                    return IVar29;
                                  }
                                  pdVar18 = worldContext1->dwordArray;
                                  pWVar35 = worldContext1;
                                  dVar5 = WidePath_ParseTrailingNumberBeforeExtensionRegs
                                                    (soundDirectoryPathCursor);
                                  if (dVar5 < worldContext1->dwordArrayCount) {
                                    if (!PVar32.carry) {
                                      RVar34 = Resource_Load(soundDirectoryPathCursor);
                                      shrinkResultOrError = (void *)RVar34.eax;
                                      if (RVar34.carry) break;
                                    }
                                    else {
                                      WidePath_CombineDirectoryAndLeaf
                                                ((word *)&
                                                  g_InGameLevelSoundLeafOrCombinedPathScratchUtf16,
                                                 soundDirectoryPathCursor,
                                                 (word *)&
                                                  g_InGameLevelSoundParentDirectoryScratchUtf16);
                                      RVar34 = Resource_Load((word *)&
                                                  g_InGameLevelSoundLeafOrCombinedPathScratchUtf16);
                                      shrinkResultOrError = (void *)RVar34.eax;
                                      if (RVar34.carry) break;
                                    }
                                    SVar26 = SpatialSoundSlot_CreateFromSampleAsset
                                                       (shrinkResultOrError);
                                    if (!SVar26.carry) {
                                      pdVar18[dVar5] = (dword)SVar26.soundSlot;
                                    }
                                    Resource_Release(shrinkResultOrError);
                                    MoviePlayback_AdvanceScheduledFrameAndTick();
                                  }
                                  uVar3 = uVar3 - 1;
                                  worldContext1 = pWVar35;
                                  soundDirectoryPathCursor =
                                       (word *)((int)soundDirectoryPathCursor +
                                               (int)&(((InGameLevelConditionStorageView800 *)
                                                      soundDirectoryRecordSizeBytes)->levelImage).
                                                     header);
                                  pIVar2 = g_InGameLevelRuntimeGlobalBlock.conditionStorage;
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
  IVar30.carry = true;
  IVar30.errorOrValue = (dword)resultOrPointer;
  return IVar30;
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
  LevelPlayerSlotByteOffset32 LVar1;
  InGameLevelConditionStorageView800 *pIVar2;
  void **loadedResourcePointerArray;
  void *resultOrPointer;
  TechnologyAsset *loadedTechnologyAsset;
  dword soundDirectoryRecordSizeBytes;
  void *shrinkResultOrError;
  SoundSampleAsset *pSVar3;
  SoundSampleAsset *pSVar4;
  uint uVar5;
  WorldWorkspaceElementCount WVar6;
  PckOutputCapacityBytes outputCapacityBytes;
  dword dVar7;
  LevelAssetRecordCount LVar8;
  word *graphicsBasePath;
  word *mutableBasePath;
  uint uVar9;
  int iVar10;
  FieldGridDimensionCells gridHeight;
  FrontendLoadedLevelRuntimeImage370 *pFVar11;
  byte *pbVar12;
  word *pwVar13;
  dword *pdVar14;
  ArenaAllocEaxCf5 AVar15;
  PackageLoadEntryEaxCf5 PVar16;
  StatusValueEaxCf5 SVar17;
  ArmyRuntimeInitEaxCf5 AVar18;
  ArenaShrinkEaxCf5 AVar19;
  SpatialSoundSlotEaxCf5 SVar20;
  ArenaFreeEaxCf5 AVar21;
  SoundCreateSampleVoiceSetEaxCf5 SVar22;
  InGameLevelLoadEaxCf5 IVar23;
  InGameLevelLoadEaxCf5 IVar24;
  ArenaLargestAllocationEaxEcxCf9 AVar25;
  PackageFindEntryEaxEcxCf9 PVar26;
  FileSystemEnumerationEaxEcxCf9 FVar27;
  ResourceLoadEaxEcxCf9 RVar28;
  word *soundDirectoryPathCursor;
  WorldRuntimeContext *worldContext1;
  
  AVar15 = (*g_MemoryApi.alloc)(0x800);
  loadedResourcePointerArray = (void **)AVar15.eax;
  resultOrPointer = loadedResourcePointerArray;
  if (!AVar15.carry) {
    g_InGameLoadedResourcePointerCount = 0;
    resultOrPointer = (void *)0x39;
    g_InGameLoadedResourcePointers = loadedResourcePointerArray;
    Package_SetLastErrorPath(g_LevelEndingMovieSourcePath);
    if (((levelImage->header).common.magic == ASSET_MAGIC_LEV) &&
       ((levelImage->header).common.converterVersion == PCK_CONVERTER_LEV_00070001)) {
      uVar5 = (levelImage->header).resourceTables.runtimePrefixByteSizeAndInitialArmyPlacementOffset
      ;
      AVar15 = (*g_MemoryApi.alloc)(uVar5);
      resultOrPointer = (void *)AVar15.eax;
      if (!AVar15.carry) {
        pFVar11 = levelImage;
        g_InGameLevelRuntimeGlobalBlock.conditionStorage = resultOrPointer;
        for (uVar5 = uVar5 >> 2; uVar5 != 0; uVar5 = uVar5 - 1) {
          (((InGameLevelConditionStorageView800 *)resultOrPointer)->levelImage).header.common.magic
               = (pFVar11->header).common.magic;
          pFVar11 = (FrontendLoadedLevelRuntimeImage370 *)
                    &(pFVar11->header).common.allocationSizeBytes;
          resultOrPointer =
               &(((InGameLevelConditionStorageView800 *)resultOrPointer)->levelImage).header.common.
                allocationSizeBytes;
        }
        g_InGameLevelTitleTextResourceIndex = (levelImage->header).titleTextResourceIndex;
        g_InGameLevelCampaignAssociationIndex = (levelImage->header).campaignAssociationIndex;
        pbVar12 = (levelImage->header).common.buildMetadata.assetRelativeAddressAnchor28 +
                  ((levelImage->header).pathState.technologyPathOffset - 0x28);
        WidePath_SetExtensionCode(0x636574,(word *)pbVar12);
        PVar16 = Package_LoadEntry((word *)pbVar12);
        loadedTechnologyAsset = PVar16.bufferOrError;
        resultOrPointer = loadedTechnologyAsset;
        if (((!PVar16.carry) &&
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
          pwVar13 = (word *)((levelImage->header).common.buildMetadata.assetRelativeAddressAnchor28
                            + ((levelImage->header).resourceTables.effectAssetPathTableOffset - 0x28
                              ));
          for (LVar8 = (levelImage->header).resourceTables.effectAssetPathCount; LVar8 != 0;
              LVar8 = LVar8 - 1) {
            WidePath_SetExtensionCode(0x666665,pwVar13);
            resultOrPointer = (InGameLevelConditionStorageView800 *)0x3a;
            if (0x1ff < g_InGameLoadedResourcePointerCount)
            goto 
            InGameLevelRuntime_LoadResourcesAfterExternalTablesCf_ReturnCurrentResourceLoadFailureStatus
            ;
            PVar16 = Package_LoadEntry(pwVar13);
            resultOrPointer = PVar16.bufferOrError;
            if (PVar16.carry)
            goto 
            InGameLevelRuntime_LoadResourcesAfterExternalTablesCf_ReturnCurrentResourceLoadFailureStatus
            ;
            *loadedResourcePointerArray = resultOrPointer;
            g_InGameLoadedResourcePointerCount = g_InGameLoadedResourcePointerCount + 1;
            loadedResourcePointerArray = loadedResourcePointerArray + 1;
            SVar17 = EffectAsset_PrepareEntries(resultOrPointer);
            resultOrPointer = (void *)SVar17.valueOrError;
            if (SVar17.carry)
            goto 
            InGameLevelRuntime_LoadResourcesAfterExternalTablesCf_ReturnCurrentResourceLoadFailureStatus
            ;
            MoviePlayback_AdvanceScheduledFrameAndTick();
            pwVar13 = pwVar13 + 0x20;
          }
          pwVar13 = (word *)((levelImage->header).common.buildMetadata.assetRelativeAddressAnchor28
                            + ((levelImage->header).resourceTables.shotAssetPathTableOffset - 0x28))
          ;
          for (LVar8 = (levelImage->header).resourceTables.shotAssetPathCount; LVar8 != 0;
              LVar8 = LVar8 - 1) {
            WidePath_SetExtensionCode(0x746873,pwVar13);
            resultOrPointer = (InGameLevelConditionStorageView800 *)0x3a;
            if (0x1ff < g_InGameLoadedResourcePointerCount)
            goto 
            InGameLevelRuntime_LoadResourcesAfterExternalTablesCf_ReturnCurrentResourceLoadFailureStatus
            ;
            PVar16 = Package_LoadEntry(pwVar13);
            resultOrPointer = PVar16.bufferOrError;
            if (PVar16.carry)
            goto 
            InGameLevelRuntime_LoadResourcesAfterExternalTablesCf_ReturnCurrentResourceLoadFailureStatus
            ;
            *loadedResourcePointerArray = resultOrPointer;
            g_InGameLoadedResourcePointerCount = g_InGameLoadedResourcePointerCount + 1;
            loadedResourcePointerArray = loadedResourcePointerArray + 1;
            SVar17 = ShotAsset_PrepareEntries(resultOrPointer);
            resultOrPointer = (void *)SVar17.valueOrError;
            if (SVar17.carry)
            goto 
            InGameLevelRuntime_LoadResourcesAfterExternalTablesCf_ReturnCurrentResourceLoadFailureStatus
            ;
            MoviePlayback_AdvanceScheduledFrameAndTick();
            pwVar13 = pwVar13 + 0x20;
          }
          SVar17 = EffectDefinitions_ResolveCrossReferences();
          resultOrPointer = (void *)SVar17.valueOrError;
          if (!SVar17.carry) {
            pwVar13 = (word *)((levelImage->header).common.buildMetadata.
                               assetRelativeAddressAnchor28 +
                              ((levelImage->header).resourceTables.modelAssetPathTableOffset - 0x28)
                              );
            for (LVar8 = (levelImage->header).resourceTables.modelAssetPathCount; LVar8 != 0;
                LVar8 = LVar8 - 1) {
              WidePath_SetExtensionCode(0x6c646d,pwVar13);
              resultOrPointer = (InGameLevelConditionStorageView800 *)0x3a;
              if (0x1ff < g_InGameLoadedResourcePointerCount)
              goto 
              InGameLevelRuntime_LoadResourcesAfterExternalTablesCf_ReturnCurrentResourceLoadFailureStatus
              ;
              PVar16 = Package_LoadEntry(pwVar13);
              resultOrPointer = PVar16.bufferOrError;
              if (PVar16.carry)
              goto 
              InGameLevelRuntime_LoadResourcesAfterExternalTablesCf_ReturnCurrentResourceLoadFailureStatus
              ;
              *loadedResourcePointerArray = resultOrPointer;
              g_InGameLoadedResourcePointerCount = g_InGameLoadedResourcePointerCount + 1;
              loadedResourcePointerArray = loadedResourcePointerArray + 1;
              SVar17 = ModelAsset_PrepareRecords(resultOrPointer);
              resultOrPointer = (void *)SVar17.valueOrError;
              if (SVar17.carry)
              goto 
              InGameLevelRuntime_LoadResourcesAfterExternalTablesCf_ReturnCurrentResourceLoadFailureStatus
              ;
              MoviePlayback_AdvanceScheduledFrameAndTick();
              pwVar13 = pwVar13 + 0x20;
            }
            pwVar13 = (word *)((levelImage->header).common.buildMetadata.
                               assetRelativeAddressAnchor28 +
                              ((levelImage->header).resourceTables.armyAssetPathTableOffset - 0x28))
            ;
            for (LVar8 = (levelImage->header).resourceTables.armyAssetPathCount; LVar8 != 0;
                LVar8 = LVar8 - 1) {
              WidePath_SetExtensionCode(0x6d7261,pwVar13);
              resultOrPointer = (InGameLevelConditionStorageView800 *)0x3a;
              if (0x1ff < g_InGameLoadedResourcePointerCount)
              goto 
              InGameLevelRuntime_LoadResourcesAfterExternalTablesCf_ReturnCurrentResourceLoadFailureStatus
              ;
              PVar16 = Package_LoadEntry(pwVar13);
              resultOrPointer = PVar16.bufferOrError;
              if (PVar16.carry)
              goto 
              InGameLevelRuntime_LoadResourcesAfterExternalTablesCf_ReturnCurrentResourceLoadFailureStatus
              ;
              *loadedResourcePointerArray = resultOrPointer;
              g_InGameLoadedResourcePointerCount = g_InGameLoadedResourcePointerCount + 1;
              loadedResourcePointerArray = loadedResourcePointerArray + 1;
              SVar17 = ArmyAsset_PrepareRecords(resultOrPointer);
              resultOrPointer = (void *)SVar17.valueOrError;
              if (SVar17.carry)
              goto 
              InGameLevelRuntime_LoadResourcesAfterExternalTablesCf_ReturnCurrentResourceLoadFailureStatus
              ;
              MoviePlayback_AdvanceScheduledFrameAndTick();
              pwVar13 = pwVar13 + 0x20;
            }
            resultOrPointer = (InGameLevelConditionStorageView800 *)0x3a;
            if (g_InGameLoadedResourcePointerCount < 0x200) {
              g_MoviePlaybackBaseFrameGroup = 1;
              g_MoviePlaybackScheduleCounter = 0;
              g_MoviePlaybackScheduleSpan = 0x10000;
              SVar17 = TerrainVisualResources_LoadAndClearCellOverlayFlags
                                 ((word *)((levelImage->header).common.buildMetadata.
                                           assetRelativeAddressAnchor28 +
                                          ((levelImage->header).pathState.
                                           surfaceTextureBasePathOffset - 0x28)),
                                  (word *)((levelImage->header).common.buildMetadata.
                                           assetRelativeAddressAnchor28 +
                                          ((levelImage->header).pathState.
                                           groundTextureBasePathOffset - 0x28)),
                                  (FieldGridAsset *)
                                  (levelImage->header).pathState.levelPathOffsetOrLoadedFieldGrid);
              resultOrPointer = (void *)SVar17.valueOrError;
              if (!SVar17.carry) {
                SVar17 = ShotDefinitions_ValidateTerrainMaterialReferences();
                resultOrPointer = (void *)SVar17.valueOrError;
                if (!SVar17.carry) {
                  g_MoviePlaybackBaseFrameGroup = 2;
                  g_MoviePlaybackScheduleCounter = 0;
                  g_MoviePlaybackScheduleSpan = 0x10000;
                  SVar17 = ModelRuntimePool_Init();
                  resultOrPointer = (void *)SVar17.valueOrError;
                  if (!SVar17.carry) {
                    AVar18 = ArmyRuntime_InitializePoolAndGraphicsCf(worldRuntime,graphicsBasePath);
                    resultOrPointer = (void *)AVar18.errorOrValue;
                    if (!AVar18.carry) {
                      g_MoviePlaybackBaseFrameGroup = 3;
                      g_MoviePlaybackScheduleCounter = 0;
                      g_MoviePlaybackScheduleSpan = 4;
                      SVar17 = ShotRuntime_InitGraphicsResources
                                         ((word *)((levelImage->header).common.buildMetadata.
                                                   assetRelativeAddressAnchor28 +
                                                  ((levelImage->header).pathState.
                                                   shotTextureBasePathOffset - 0x28)));
                      resultOrPointer = (void *)SVar17.valueOrError;
                      if (!SVar17.carry) {
                        g_MoviePlaybackBaseFrameGroup = 4;
                        g_MoviePlaybackScheduleCounter = 0;
                        g_MoviePlaybackScheduleSpan = 4;
                        SVar17 = EffectRuntime_InitGraphicsResources(mutableBasePath);
                        resultOrPointer = (void *)SVar17.valueOrError;
                        if (!SVar17.carry) {
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
                          iVar10 = worldRuntime->activeFactionRuntimeIndex;
                          MoviePlayback_AdvanceScheduledFrameAndTick();
                          LVar1 = g_InGameLevelRuntimeGlobalBlock.playerSlotByteOffsets[iVar10 + -1]
                          ;
                          WorldRuntime_AttachFieldGridAsset
                                    ((FieldGridAsset *)
                                     (levelImage->header).pathState.levelPathOffsetOrLoadedFieldGrid
                                     ,worldRuntime);
                          MoviePlayback_AdvanceScheduledFrameAndTick();
                          uVar5 = *(uint *)((int)&levelImage->playerSlots[0].
                                                  packedHeadingLow16PitchHigh16 + LVar1);
                          WorldRuntime_SetPosition60AndDistanceFromPosition80
                                    (*(Q12 *)((int)&levelImage->playerSlots[0].startCameraZQ12 +
                                             LVar1),
                                     *(Q12 *)((int)&levelImage->playerSlots[0].startCameraYQ12 +
                                             LVar1),
                                     *(Q12 *)((int)&levelImage->playerSlots[0].startCameraXQ12 +
                                             LVar1),worldRuntime);
                          WorldRuntime_SetMotionParameters6CThrough78Clamped
                                    (2,(int)uVar5 >> 0x10,uVar5 & 0xffff,
                                     *(UQ12 *)((int)&levelImage->playerSlots[0].
                                                     startCameraMagnitudeQ12 + LVar1),worldRuntime);
                          uVar5 = (levelImage->runtimeTail2E0).packedFieldRegionOriginYHigh16XLow16;
                          WorldRuntime_RecomputeMotionEndpointAgainstFieldSurface(worldRuntime);
                          uVar9 = uVar5;
                          WorldRuntime_CommitScalar7CFrom8C(worldRuntime);
                          uVar5 = uVar5 & 0xffff;
                          iVar10 = (int)uVar9 >> 0x10;
                          uVar9 = (levelImage->runtimeTail2E0).
                                  packedFieldRegionHeightHigh16WidthLow16;
                          worldContext1 = worldRuntime;
                          MoviePlayback_AdvanceScheduledFrameAndTick();
                          WorldRuntime_RecomputeFieldRegionNormalsAndLighting
                                    ((int)uVar9 >> 0x10,uVar9 & 0xffff,iVar10,uVar5,worldContext1);
                          SVar17 = Package_LoadEntryIntoBuffer
                                             (worldRuntime->objectCount * 0x100,
                                              (byte *)worldRuntime->objectArray,
                                              (word *)u_widget_hex_0050e02a);
                          resultOrPointer = (void *)SVar17.valueOrError;
                          if (!SVar17.carry) {
                            SVar17 = Package_LoadEntryIntoBuffer
                                               (0x48000,(byte *)g_ArmyRuntimeSlots,
                                                (word *)u_army_hex_0050dfb4);
                            resultOrPointer = (void *)SVar17.valueOrError;
                            if (!SVar17.carry) {
                              SVar17 = Package_LoadEntryIntoBuffer
                                                 (0x400000,(byte *)g_ModelRuntimeSlots,
                                                  (word *)u_modul_hex_0050dfee);
                              resultOrPointer = (void *)SVar17.valueOrError;
                              if (!SVar17.carry) {
                                SVar17 = Package_LoadEntryIntoBuffer
                                                   (0x40000,(byte *)g_EffectRuntimeSlots,
                                                    (word *)u_effect_hex_0050dfc6);
                                resultOrPointer = (void *)SVar17.valueOrError;
                                if (!SVar17.carry) {
                                  SVar17 = Package_LoadEntryIntoBuffer
                                                     (0x40000,(byte *)g_ShotRuntimeSlots,
                                                      (word *)u_shot_hex_0050dfdc);
                                  resultOrPointer = (void *)SVar17.valueOrError;
                                  if (!SVar17.carry) {
                                    SVar17 = Package_LoadEntryIntoBuffer
                                                       (0x4000,(byte *)
                                                  g_GraphicsShadingRuntimeRecords,
                                                  (word *)u_light_hex_0050e016);
                                    resultOrPointer = (void *)SVar17.valueOrError;
                                    if (!SVar17.carry) {
                                      ArmyRuntimePool_RebaseAfterLoad();
                                      ModelRuntimePool_RebaseAfterLoad();
                                      ShotRuntime_RebaseSlotsAfterLoad();
                                      EffectRuntime_RebaseSlotsAfterLoad();
                                      ResourceRegistrationRuntime_RebaseLoadedRecords
                                                ((ResourceRegistrationRuntimeImage *)worldRuntime);
                                      RuntimeHexSegment_ToggleLightImageFlag();
                                      MoviePlayback_AdvanceScheduledFrameAndTick();
                                      pdVar14 = worldRuntime->dwordArray;
                                      for (WVar6 = worldRuntime->dwordArrayCount; WVar6 != 0;
                                          WVar6 = WVar6 - 1) {
                                        *pdVar14 = 0;
                                        pdVar14 = pdVar14 + 1;
                                      }
                                      pbVar12 = (levelImage->header).common.buildMetadata.
                                                assetRelativeAddressAnchor28 +
                                                ((levelImage->header).pathState.soundBasePathOffset
                                                - 0x28);
                                      WidePath_SetExtensionCode(0x6d6173,(word *)pbVar12);
                                      MoviePlayback_AdvanceScheduledFrameAndTick();
                                      WidePath_SplitParentAndLeaf
                                                ((word *)&
                                                  g_InGameLevelSoundLeafOrCombinedPathScratchUtf16,
                                                 (word *)&
                                                  g_InGameLevelSoundParentDirectoryScratchUtf16,
                                                 (word *)pbVar12);
                                      AVar25 = (*g_MemoryApi.allocLargestFreeBlock)();
                                      outputCapacityBytes = AVar25.blockSizeOrSentinel;
                                      resultOrPointer = (void *)AVar25.allocationOrError;
                                      if (!AVar25.carry) {
                                        PVar26 = Package_FindEntry(outputCapacityBytes,
                                                                   resultOrPointer,(word *)pbVar12,
                                                                   g_SoundPackageHandle);
                                        uVar5 = PVar26.matchCount;
                                        soundDirectoryRecordSizeBytes = PVar26.recordSizeOrError;
                                        if (PVar26.carry) {
                                          FVar27 = (*g_FileSystemEnumerateDirectoryOrVolumeEntriesCf
                                                   )(FILESYSTEM_ENUMERATE_FILES,0xffffffff,
                                                     outputCapacityBytes,resultOrPointer,pbVar12);
                                          uVar5 = FVar27.entryCount;
                                          soundDirectoryRecordSizeBytes = FVar27.recordSizeBytes;
                                        }
                                        shrinkResultOrError = (void *)soundDirectoryRecordSizeBytes;
                                        if (&stack0xffffffd8 < (undefined1 *)0xfffffffc) {
                                          g_MoviePlaybackBaseFrameGroup = 6;
                                          g_MoviePlaybackScheduleCounter = 0;
                                          g_MoviePlaybackScheduleSpan = uVar5;
                                          AVar19 = (*g_MemoryApi.shrinkInPlace)
                                                             (soundDirectoryRecordSizeBytes * uVar5,
                                                              resultOrPointer);
                                          shrinkResultOrError = (void *)AVar19.scratchOrError;
                                          if (!AVar19.carry) {
                                            soundDirectoryPathCursor = resultOrPointer;
                                            pIVar2 = g_InGameLevelRuntimeGlobalBlock.
                                                     conditionStorage;
                                            if (worldRuntime->dwordArrayCount < uVar5) {
                                              uVar5 = worldRuntime->dwordArrayCount;
                                            }
                                            do {
                                              g_InGameLevelRuntimeGlobalBlock.conditionStorage =
                                                   pIVar2;
                                              if (uVar5 == 0) {
                                                AVar21 = (*g_MemoryApi.free)(resultOrPointer);
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
                                                pSVar4 = (SoundSampleAsset *)AVar21.eax;
                                                if ((pIVar2->levelImage).runtimeTail2E0.
                                                    effectSampleNumbers[0] != 0) {
                                                  (*g_WideNumberFormatUtf16)
                                                            (WIDE_FORMAT_PAD_WITH_ZERO,0,2,1,
                                                             (pIVar2->levelImage).runtimeTail2E0.
                                                             effectSampleNumbers[0],
                                                             (word *)(u_sound_level00_sam_0050df6c +
                                                                     0xb));
                                                  RVar28 = Resource_Load((word *)
                                                  u_sound_level00_sam_0050df6c);
                                                  pSVar3 = (SoundSampleAsset *)RVar28.eax;
                                                  pSVar4 = pSVar3;
                                                  if (!RVar28.carry) {
                                                    SVar22 = (*g_SoundCreateSampleVoiceSet)(pSVar3);
                                                    pSVar4 = (SoundSampleAsset *)SVar22.eax;
                                                    if (!SVar22.carry) {
                                                      g_InGameLevelEffectVoiceSet0 = pSVar4;
                                                    }
                                                    Resource_Release(pSVar3);
                                                  }
                                                }
                                                if ((pIVar2->levelImage).runtimeTail2E0.
                                                    effectSampleNumbers[1] != 0) {
                                                  (*g_WideNumberFormatUtf16)
                                                            (WIDE_FORMAT_PAD_WITH_ZERO,0,2,1,
                                                             (pIVar2->levelImage).runtimeTail2E0.
                                                             effectSampleNumbers[1],
                                                             (word *)(u_sound_level00_sam_0050df6c +
                                                                     0xb));
                                                  RVar28 = Resource_Load((word *)
                                                  u_sound_level00_sam_0050df6c);
                                                  pSVar3 = (SoundSampleAsset *)RVar28.eax;
                                                  pSVar4 = pSVar3;
                                                  if (!RVar28.carry) {
                                                    SVar22 = (*g_SoundCreateSampleVoiceSet)(pSVar3);
                                                    pSVar4 = (SoundSampleAsset *)SVar22.eax;
                                                    if (!SVar22.carry) {
                                                      g_InGameLevelEffectVoiceSet1 = pSVar4;
                                                    }
                                                    Resource_Release(pSVar3);
                                                  }
                                                }
                                                if ((pIVar2->levelImage).runtimeTail2E0.
                                                    effectSampleNumbers[2] != 0) {
                                                  (*g_WideNumberFormatUtf16)
                                                            (WIDE_FORMAT_PAD_WITH_ZERO,0,2,1,
                                                             (pIVar2->levelImage).runtimeTail2E0.
                                                             effectSampleNumbers[2],
                                                             (word *)(u_sound_level00_sam_0050df6c +
                                                                     0xb));
                                                  RVar28 = Resource_Load((word *)
                                                  u_sound_level00_sam_0050df6c);
                                                  pSVar3 = (SoundSampleAsset *)RVar28.eax;
                                                  pSVar4 = pSVar3;
                                                  if (!RVar28.carry) {
                                                    SVar22 = (*g_SoundCreateSampleVoiceSet)(pSVar3);
                                                    pSVar4 = (SoundSampleAsset *)SVar22.eax;
                                                    if (!SVar22.carry) {
                                                      g_InGameLevelEffectVoiceSet2 = pSVar4;
                                                    }
                                                    Resource_Release(pSVar3);
                                                  }
                                                }
                                                if ((pIVar2->levelImage).runtimeTail2E0.
                                                    effectSampleNumbers[3] != 0) {
                                                  (*g_WideNumberFormatUtf16)
                                                            (WIDE_FORMAT_PAD_WITH_ZERO,0,2,1,
                                                             (pIVar2->levelImage).runtimeTail2E0.
                                                             effectSampleNumbers[3],
                                                             (word *)(u_sound_level00_sam_0050df6c +
                                                                     0xb));
                                                  RVar28 = Resource_Load((word *)
                                                  u_sound_level00_sam_0050df6c);
                                                  pSVar3 = (SoundSampleAsset *)RVar28.eax;
                                                  pSVar4 = pSVar3;
                                                  if (!RVar28.carry) {
                                                    SVar22 = (*g_SoundCreateSampleVoiceSet)(pSVar3);
                                                    pSVar4 = (SoundSampleAsset *)SVar22.eax;
                                                    if (!SVar22.carry) {
                                                      g_InGameLevelEffectVoiceSet3 = pSVar4;
                                                    }
                                                    Resource_Release(pSVar3);
                                                  }
                                                }
                                                if ((pIVar2->levelImage).runtimeTail2E0.
                                                    musicSampleNumbers[0] != 0) {
                                                  (*g_WideNumberFormatUtf16)
                                                            (WIDE_FORMAT_PAD_WITH_ZERO,0,2,1,
                                                             (pIVar2->levelImage).runtimeTail2E0.
                                                             musicSampleNumbers[0],
                                                             (word *)(u_sound_music00_sam_0050df90 +
                                                                     0xb));
                                                  RVar28 = Resource_Load((word *)
                                                  u_sound_music00_sam_0050df90);
                                                  pSVar3 = (SoundSampleAsset *)RVar28.eax;
                                                  pSVar4 = pSVar3;
                                                  if (!RVar28.carry) {
                                                    SVar22 = (*g_SoundCreateSampleVoiceSet)(pSVar3);
                                                    pSVar4 = (SoundSampleAsset *)SVar22.eax;
                                                    if (!SVar22.carry) {
                                                      g_InGameLevelMusicVoiceSet0 = pSVar4;
                                                    }
                                                    Resource_Release(pSVar3);
                                                  }
                                                }
                                                if ((pIVar2->levelImage).runtimeTail2E0.
                                                    musicSampleNumbers[1] != 0) {
                                                  (*g_WideNumberFormatUtf16)
                                                            (WIDE_FORMAT_PAD_WITH_ZERO,0,2,1,
                                                             (pIVar2->levelImage).runtimeTail2E0.
                                                             musicSampleNumbers[1],
                                                             (word *)(u_sound_music00_sam_0050df90 +
                                                                     0xb));
                                                  RVar28 = Resource_Load((word *)
                                                  u_sound_music00_sam_0050df90);
                                                  pSVar3 = (SoundSampleAsset *)RVar28.eax;
                                                  pSVar4 = pSVar3;
                                                  if (!RVar28.carry) {
                                                    SVar22 = (*g_SoundCreateSampleVoiceSet)(pSVar3);
                                                    pSVar4 = (SoundSampleAsset *)SVar22.eax;
                                                    if (!SVar22.carry) {
                                                      g_InGameLevelMusicVoiceSet1 = pSVar4;
                                                    }
                                                    Resource_Release(pSVar3);
                                                  }
                                                }
                                                if ((pIVar2->levelImage).runtimeTail2E0.
                                                    musicSampleNumbers[2] != 0) {
                                                  (*g_WideNumberFormatUtf16)
                                                            (WIDE_FORMAT_PAD_WITH_ZERO,0,2,1,
                                                             (pIVar2->levelImage).runtimeTail2E0.
                                                             musicSampleNumbers[2],
                                                             (word *)(u_sound_music00_sam_0050df90 +
                                                                     0xb));
                                                  RVar28 = Resource_Load((word *)
                                                  u_sound_music00_sam_0050df90);
                                                  pSVar3 = (SoundSampleAsset *)RVar28.eax;
                                                  pSVar4 = pSVar3;
                                                  if (!RVar28.carry) {
                                                    SVar22 = (*g_SoundCreateSampleVoiceSet)(pSVar3);
                                                    pSVar4 = (SoundSampleAsset *)SVar22.eax;
                                                    if (!SVar22.carry) {
                                                      g_InGameLevelMusicVoiceSet2 = pSVar4;
                                                    }
                                                    Resource_Release(pSVar3);
                                                  }
                                                }
                                                if ((pIVar2->levelImage).runtimeTail2E0.
                                                    musicSampleNumbers[3] != 0) {
                                                  (*g_WideNumberFormatUtf16)
                                                            (WIDE_FORMAT_PAD_WITH_ZERO,0,2,1,
                                                             (pIVar2->levelImage).runtimeTail2E0.
                                                             musicSampleNumbers[3],
                                                             (word *)(u_sound_music00_sam_0050df90 +
                                                                     0xb));
                                                  RVar28 = Resource_Load((word *)
                                                  u_sound_music00_sam_0050df90);
                                                  pSVar3 = (SoundSampleAsset *)RVar28.eax;
                                                  pSVar4 = pSVar3;
                                                  if (!RVar28.carry) {
                                                    SVar22 = (*g_SoundCreateSampleVoiceSet)(pSVar3);
                                                    pSVar4 = (SoundSampleAsset *)SVar22.eax;
                                                    if (!SVar22.carry) {
                                                      g_InGameLevelMusicVoiceSet3 = pSVar4;
                                                    }
                                                    Resource_Release(pSVar3);
                                                  }
                                                }
                                                IVar23.carry = false;
                                                IVar23.errorOrValue = (dword)pSVar4;
                                                return IVar23;
                                              }
                                              pdVar14 = worldRuntime->dwordArray;
                                              worldContext1 = worldRuntime;
                                              dVar7 = 
                                                  WidePath_ParseTrailingNumberBeforeExtensionRegs
                                                            (soundDirectoryPathCursor);
                                              if (dVar7 < worldRuntime->dwordArrayCount) {
                                                if (!PVar26.carry) {
                                                  RVar28 = Resource_Load(soundDirectoryPathCursor);
                                                  shrinkResultOrError = (void *)RVar28.eax;
                                                  if (RVar28.carry) break;
                                                }
                                                else {
                                                  WidePath_CombineDirectoryAndLeaf
                                                            ((word *)&
                                                  g_InGameLevelSoundLeafOrCombinedPathScratchUtf16,
                                                  soundDirectoryPathCursor,
                                                  (word *)&
                                                  g_InGameLevelSoundParentDirectoryScratchUtf16);
                                                  RVar28 = Resource_Load((word *)&
                                                  g_InGameLevelSoundLeafOrCombinedPathScratchUtf16);
                                                  shrinkResultOrError = (void *)RVar28.eax;
                                                  if (RVar28.carry) break;
                                                }
                                                SVar20 = SpatialSoundSlot_CreateFromSampleAsset
                                                                   (shrinkResultOrError);
                                                if (!SVar20.carry) {
                                                  pdVar14[dVar7] = (dword)SVar20.soundSlot;
                                                }
                                                Resource_Release(shrinkResultOrError);
                                                MoviePlayback_AdvanceScheduledFrameAndTick();
                                              }
                                              uVar5 = uVar5 - 1;
                                              worldRuntime = worldContext1;
                                              soundDirectoryPathCursor =
                                                   (word *)((int)soundDirectoryPathCursor +
                                                           (int)&(((
                                                  InGameLevelConditionStorageView800 *)
                                                  soundDirectoryRecordSizeBytes)->levelImage).header
                                                  );
                                              pIVar2 = g_InGameLevelRuntimeGlobalBlock.
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
  IVar24.carry = true;
  IVar24.errorOrValue = (dword)resultOrPointer;
  return IVar24;
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
  void **ppvVar1;
  InGameLoadedResourcePointerCount IVar2;
  dword *pdVar3;
  dword *pdVar4;
  dword *pdVar5;
  void **resourcePointerCursor;
  
  EffectRuntime_ShutdownGraphicsResources();
  ShotRuntime_ShutdownGraphicsResources();
  ModelRuntimePool_ShutdownAndReleaseDefinitions();
  ArmyRuntime_ShutdownPoolAndGraphics();
  TerrainVisualResources_Shutdown();
  pdVar4 = (dword *)worldRuntime->dwordArrayCount;
  pdVar5 = worldRuntime->dwordArray;
  pdVar3 = pdVar5;
  if (pdVar4 != (dword *)0x0) {
    while (pdVar3 != (dword *)0x0) {
      SpatialSoundSlot_ReleaseSample((SpatialSoundSlot *)*pdVar5);
      pdVar5 = pdVar5 + 1;
      pdVar4 = (dword *)((int)pdVar4 + -1);
      pdVar3 = pdVar4;
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
  ppvVar1 = g_InGameLoadedResourcePointers;
  IVar2 = g_InGameLoadedResourcePointerCount;
  if (g_InGameLoadedResourcePointers != (void **)0x0) {
    for (; IVar2 != 0; IVar2 = IVar2 - 1) {
      Resource_Release(*ppvVar1);
      ppvVar1 = ppvVar1 + 1;
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
  int iVar1;
  FactionRuntimeIndex FVar2;
  WorldOwnerListNode100 *pWVar3;
  AngleTurn32 AVar4;
  byte *source;
  dword dVar5;
  dword dVar6;
  dword dVar7;
  dword dVar8;
  LevelPlacedModelRecord20 *objectRecordCursor;
  StatusValueEaxCf5 SVar9;
  
  SVar9 = Package_LoadEntryIntoBuffer(0x800000,g_PackageScratchBuffer,g_LevelEndingMovieSourcePath);
  source = g_PackageScratchBuffer;
  dVar8 = SVar9.valueOrError;
  if (!SVar9.carry) {
    iVar1 = *(int *)(g_PackageScratchBuffer + 0xdc);
    FVar2 = (saveWorldView->worldRuntime).activeFactionRuntimeIndex;
    *(int *)(g_PackageScratchBuffer + 4) = iVar1;
    source[0xd8] = 0;
    source[0xd9] = 0;
    source[0xda] = 0;
    source[0xdb] = 0;
    *(FactionRuntimeIndex *)(source + 0x2dc) = FVar2;
    objectRecordCursor = (LevelPlacedModelRecord20 *)(source + iVar1);
    for (pWVar3 = (saveWorldView->worldRuntime).ownerListHead;
        pWVar3 != (WorldOwnerListNode100 *)0x0; pWVar3 = pWVar3->nextNode) {
      if (pWVar3->ownerClassId == WORLD_OWNER_RUNTIME_MODEL) {
        *(int *)(source + 0xd8) = *(int *)(source + 0xd8) + 1;
        *(int *)(source + 4) = *(int *)(source + 4) + 0x20;
        iVar1 = *(int *)((int)pWVar3->runtimePayload + 8);
        objectRecordCursor->meshGroupMask = pWVar3->worldXQ12;
        dVar8 = *(dword *)(iVar1 + 0xa0);
        objectRecordCursor->modelRuntimeField0C = *(dword *)(iVar1 + 0xc);
        objectRecordCursor->modelRuntimeFieldA0 = dVar8;
        AVar4 = pWVar3->modelLocalRotationAngle2;
        objectRecordCursor->nodePayloadField0C = pWVar3->worldYQ12;
        objectRecordCursor->worldRotationAngle2 = AVar4;
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
    *(uint *)(source + 0x2e0) =
         saveWorldView->fieldRegionOriginWorldXQ12 & 0xffffU |
         saveWorldView->fieldRegionOriginWorldYQ12 << 0x10;
    *(WorldFieldDimension *)(source + 0x2f0) =
         (saveWorldView->worldRuntime).fieldRegion.regionWidth & 0xffff |
         (saveWorldView->worldRuntime).fieldRegion.regionHeight << 0x10;
    dVar7 = g_LevelCameraBookmark1PackedHeadingLow16PitchHigh16;
    dVar6 = g_LevelCameraBookmark1PositionMagnitudeQ12;
    dVar5 = g_LevelCameraBookmark1PositionZQ12;
    dVar8 = g_LevelCameraBookmark1PositionYQ12;
    *(dword *)(source + 0x200) = g_LevelCameraBookmark1PositionXQ12;
    *(dword *)(source + 0x204) = dVar8;
    *(dword *)(source + 0x208) = dVar5;
    *(dword *)(source + 0x20c) = dVar6;
    *(dword *)(source + 0x210) = dVar7;
    dVar7 = g_LevelCameraBookmark2PackedHeadingLow16PitchHigh16;
    dVar6 = g_LevelCameraBookmark2PositionMagnitudeQ12;
    dVar5 = g_LevelCameraBookmark2PositionZQ12;
    dVar8 = g_LevelCameraBookmark2PositionYQ12;
    *(dword *)(source + 0x220) = g_LevelCameraBookmark2PositionXQ12;
    *(dword *)(source + 0x224) = dVar8;
    *(dword *)(source + 0x228) = dVar5;
    *(dword *)(source + 0x22c) = dVar6;
    *(dword *)(source + 0x230) = dVar7;
    dVar7 = g_LevelCameraBookmark3PackedHeadingLow16PitchHigh16;
    dVar6 = g_LevelCameraBookmark3PositionMagnitudeQ12;
    dVar5 = g_LevelCameraBookmark3PositionZQ12;
    dVar8 = g_LevelCameraBookmark3PositionYQ12;
    *(dword *)(source + 0x240) = g_LevelCameraBookmark3PositionXQ12;
    *(dword *)(source + 0x244) = dVar8;
    *(dword *)(source + 0x248) = dVar5;
    *(dword *)(source + 0x24c) = dVar6;
    *(dword *)(source + 0x250) = dVar7;
    dVar7 = g_LevelCameraBookmark4PackedHeadingLow16PitchHigh16;
    dVar6 = g_LevelCameraBookmark4PositionMagnitudeQ12;
    dVar5 = g_LevelCameraBookmark4PositionZQ12;
    dVar8 = g_LevelCameraBookmark4PositionYQ12;
    *(dword *)(source + 0x260) = g_LevelCameraBookmark4PositionXQ12;
    *(dword *)(source + 0x264) = dVar8;
    *(dword *)(source + 0x268) = dVar5;
    *(dword *)(source + 0x26c) = dVar6;
    *(dword *)(source + 0x270) = dVar7;
    dVar7 = g_LevelCameraBookmark5PackedHeadingLow16PitchHigh16;
    dVar6 = g_LevelCameraBookmark5PositionMagnitudeQ12;
    dVar5 = g_LevelCameraBookmark5PositionZQ12;
    dVar8 = g_LevelCameraBookmark5PositionYQ12;
    *(dword *)(source + 0x280) = g_LevelCameraBookmark5PositionXQ12;
    *(dword *)(source + 0x284) = dVar8;
    *(dword *)(source + 0x288) = dVar5;
    *(dword *)(source + 0x28c) = dVar6;
    *(dword *)(source + 0x290) = dVar7;
    dVar7 = g_LevelCameraBookmark6PackedHeadingLow16PitchHigh16;
    dVar6 = g_LevelCameraBookmark6PositionMagnitudeQ12;
    dVar5 = g_LevelCameraBookmark6PositionZQ12;
    dVar8 = g_LevelCameraBookmark6PositionYQ12;
    *(dword *)(source + 0x2a0) = g_LevelCameraBookmark6PositionXQ12;
    *(dword *)(source + 0x2a4) = dVar8;
    *(dword *)(source + 0x2a8) = dVar5;
    *(dword *)(source + 0x2ac) = dVar6;
    *(dword *)(source + 0x2b0) = dVar7;
    dVar7 = g_LevelCameraBookmark7PackedHeadingLow16PitchHigh16;
    dVar6 = g_LevelCameraBookmark7PositionMagnitudeQ12;
    dVar5 = g_LevelCameraBookmark7PositionZQ12;
    dVar8 = g_LevelCameraBookmark7PositionYQ12;
    *(dword *)(source + 0x2c0) = g_LevelCameraBookmark7PositionXQ12;
    *(dword *)(source + 0x2c4) = dVar8;
    *(dword *)(source + 0x2c8) = dVar5;
    *(dword *)(source + 0x2cc) = dVar6;
    *(dword *)(source + 0x2d0) = dVar7;
    SVar9 = FileSystem_WriteBufferToPathCf
                      (*(FileIoByteCount *)(source + 4),source,g_LevelEndingMovieSourcePath);
    dVar8 = SVar9.valueOrError;
    if (!SVar9.carry) {
      return (StatusValueEaxCf5)((uint5)SVar9 & 0xffffffff);
    }
  }
  SVar9.carry = true;
  SVar9.valueOrError = dVar8;
  return SVar9;
}


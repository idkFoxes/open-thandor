#include <thandor/gameplay/session/level.h>

/* Implementation ownership: gameplay/session/level. */

/* Address: 0x00531080.
   Ownership: gameplay/session/level.
   Purpose: EAX returns the prepared movie path; CF reports failure.
   Cross-module calls: WidePath_SetExtensionCode [core/text/path], Package_SetLastErrorPath
   [assets/package/runtime].
*/
undefined8 LevelAsset_PrepareEndingMoviePathCf(word *currentLevelPath,LevelAssetHeader *asset)

{
  undefined4 extraout_EAX;
  int extraout_ECX;
  int iVar1;
  undefined4 in_EDX;
  sdword arg4;
  word *pwVar2;
  undefined4 uVar3;
  
  if (((asset->common).magic == ASSET_MAGIC_LEV) &&
     ((asset->common).converterVersion == PCK_CONVERTER_LEV_00070001)) {
    WidePath_SetExtensionCode
              (0x6d6c66,(word *)((asset->common).buildMetadata.assetRelativeAddressAnchor28 +
                                ((asset->pathOffsets).endingMovieBasePathOffset - 0x28)));
    uVar3 = 8;
    (*g_WideNumberFormatUtf16)
              (WIDE_FORMAT_PAD_WITH_ZERO,0,4,1,arg4,(word *)(u_flm_ende0000_flm_0050df06 + 8));
    pwVar2 = g_LevelEndingMovieSourcePath;
    for (iVar1 = extraout_ECX; iVar1 != 0; iVar1 = iVar1 + -1) {
      *(undefined4 *)pwVar2 = *(undefined4 *)currentLevelPath;
      currentLevelPath = currentLevelPath + 2;
      pwVar2 = pwVar2 + 2;
    }
    return CONCAT44(in_EDX,uVar3);
  }
  Package_SetLastErrorPath(currentLevelPath);
  return CONCAT44(in_EDX,extraout_EAX);
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
undefined8 __fastcall
InGameLevelRuntime_LoadResourcesAfterDefaultResetCf
          (undefined4 param_1,undefined4 param_2,LevelAssetRuntimeImagePrefix370 *levelImage,
          WorldRuntimeContext *worldRuntime)

{
  AssetRelativeOffset *pAVar1;
  FactionRuntimeLifecycleObservedState FVar2;
  PckConverterVersion PVar3;
  InGameConditionRuntime *pIVar4;
  uint uVar5;
  TechnologyAsset *pTVar6;
  TechnologyAsset *extraout_EAX;
  TechnologyAsset *outputEntries;
  TechnologyAsset *pTVar7;
  TechnologyAsset *extraout_EAX_00;
  uint extraout_EAX_01;
  dword dVar8;
  SpatialSoundSlot *pSVar9;
  DirectSoundVoiceSet *pDVar10;
  ModelDefinitionRecordPrefix *modelDefinition1;
  uint extraout_ECX;
  uint uVar11;
  int extraout_ECX_00;
  int extraout_ECX_01;
  int iVar12;
  WorldWorkspaceElementCount WVar13;
  PckOutputCapacityBytes outputCapacityBytes;
  int extraout_ECX_02;
  int extraout_ECX_03;
  uint extraout_ECX_04;
  uint extraout_ECX_05;
  undefined4 extraout_ECX_06;
  undefined4 extraout_ECX_07;
  undefined4 extraout_ECX_08;
  undefined4 extraout_ECX_09;
  undefined4 extraout_ECX_10;
  undefined4 extraout_ECX_11;
  undefined4 extraout_ECX_12;
  undefined4 extraout_ECX_13;
  undefined4 extraout_ECX_14;
  byte abVar14 [4];
  uint uVar15;
  uint uVar16;
  uint extraout_ECX_15;
  uint uVar17;
  uint extraout_ECX_16;
  word *graphicsBasePath;
  int extraout_EDX;
  int extraout_EDX_00;
  byte *outputRecords;
  byte *extraout_EDX_01;
  byte *memory;
  undefined4 extraout_EDX_02;
  word *pwVar18;
  undefined4 extraout_EDX_03;
  undefined4 extraout_EDX_04;
  undefined4 extraout_EDX_05;
  undefined4 extraout_EDX_06;
  undefined4 extraout_EDX_07;
  undefined4 extraout_EDX_08;
  undefined4 extraout_EDX_09;
  undefined4 extraout_EDX_10;
  int extraout_EDX_11;
  ArmyAssetRecordPrefix **ppAVar19;
  uint uVar20;
  LevelAssetRuntimeImagePrefix370 *pLVar21;
  byte *pbVar22;
  dword *pdVar23;
  WorldRuntimeNode *worldNode1;
  uint uVar24;
  bool bVar25;
  undefined1 uVar26;
  bool bVar27;
  undefined8 uVar28;
  WorldRuntimeContext *pWVar29;
  PckOutputCapacityBytes outputCapacityBytes_00;
  ArmyAssetRecordPrefix *local_2c;
  ArmyAssetRecordPrefix *local_28;
  ArmyAssetRecordPrefix *local_24;
  ArmyAssetRecordPrefix *local_20;
  ArmyAssetRecordPrefix *armyDefinition1;
  ArmyAssetRecordPrefix *armyDefinition2;
  ArmyAssetRecordPrefix *armyDefinition3;
  ArmyAssetRecordPrefix *armyDefinition4;
  WorldRuntimeContext *worldContext1;
  
  bVar25 = &stack0xffffffe8 < (undefined1 *)0x14;
  pTVar6 = (*g_MemoryApi.alloc)(0x800);
  outputEntries = pTVar6;
  if (!bVar25) {
    g_InGameLoadedResourcePointerCount = 0;
    g_InGameLoadedResourcePointers = pTVar6;
    Package_SetLastErrorPath(g_LevelEndingMovieSourcePath);
    outputEntries = extraout_EAX;
    if ((((levelImage->header).common.magic == ASSET_MAGIC_LEV) &&
        (PVar3 = (levelImage->header).common.converterVersion,
        bVar25 = PVar3 < PCK_CONVERTER_LEV_00070001, outputEntries = extraout_EAX,
        PVar3 == PCK_CONVERTER_LEV_00070001)) &&
       (outputEntries =
             (*g_MemoryApi.alloc)
                       ((levelImage->header).resourceTables.loadedResourcePointerArrayBytes),
       !bVar25)) {
      pLVar21 = levelImage;
      g_InGameConditionRuntime = (InGameConditionRuntime *)outputEntries;
      for (uVar11 = extraout_ECX >> 2; uVar11 != 0; uVar11 = uVar11 - 1) {
        (outputEntries->header).common.magic = (pLVar21->header).common.magic;
        pLVar21 = (LevelAssetRuntimeImagePrefix370 *)&(pLVar21->header).common.allocationSizeBytes;
        outputEntries = (TechnologyAsset *)&(outputEntries->header).common.allocationSizeBytes;
      }
      g_InGameLevelTitleTextResourceIndex = (levelImage->header).titleTextResourceIndex;
      g_InGameLevelCampaignAssociationIndex = (levelImage->header).campaignAssociationIndex;
      uVar11 = (levelImage->header).pathOffsets.technologyPathOffset;
      uVar26 = CARRY4(uVar11,(uint)levelImage);
      pbVar22 = (levelImage->header).common.buildMetadata.assetRelativeAddressAnchor28 +
                (uVar11 - 0x28);
      WidePath_SetExtensionCode(0x636574,(word *)pbVar22);
      pTVar7 = Package_LoadEntry((word *)pbVar22);
      outputEntries = pTVar7;
      if (((!(bool)uVar26) &&
          (outputEntries = (TechnologyAsset *)&k_LowAddressLiteral0000004F,
          g_TechnologyAsset = pTVar7, (pTVar7->header).common.magic == ASSET_MAGIC_TEC)) &&
         ((pTVar7->header).common.converterVersion == PCK_CONVERTER_TEC_00020000)) {
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
        g_GameFactionRuntimeImage.tail.reserved00_03 =
             *(byte (*) [4])(levelImage->opaqueRuntimeTail2E0_36F + 0x34);
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
        pwVar18 = (word *)((levelImage->header).common.buildMetadata.assetRelativeAddressAnchor28 +
                          ((levelImage->header).resourceTables.effectAssetPathTableOffset - 0x28));
        if ((levelImage->header).resourceTables.effectAssetPathCount != 0) {
          do {
            WidePath_SetExtensionCode(0x666665,pwVar18);
            outputEntries = (TechnologyAsset *)0x3a;
            bVar25 = g_InGameLoadedResourcePointerCount < 0x200;
            if ((!bVar25) || (outputEntries = Package_LoadEntry(pwVar18), bVar25))
            goto 
            InGameLevelRuntime_LoadResourcesAfterDefaultResetCf_ReturnCurrentResourceLoadFailureStatus
            ;
            (pTVar6->header).common.magic = (AssetMagic)outputEntries;
            g_InGameLoadedResourcePointerCount = g_InGameLoadedResourcePointerCount + 1;
            bVar25 = (TechnologyAsset *)0xfffffffb < pTVar6;
            pTVar6 = (TechnologyAsset *)&(pTVar6->header).common.allocationSizeBytes;
            outputEntries =
                 (TechnologyAsset *)EffectAsset_PrepareEntries((EffectAssetHeader *)outputEntries);
            if (bVar25)
            goto 
            InGameLevelRuntime_LoadResourcesAfterDefaultResetCf_ReturnCurrentResourceLoadFailureStatus
            ;
            uVar28 = MoviePlayback_AdvanceScheduledFrameAndTick();
            pwVar18 = pwVar18 + 0x20;
          } while ((int)((ulonglong)uVar28 >> 0x20) != 1);
        }
        pwVar18 = (word *)((levelImage->header).common.buildMetadata.assetRelativeAddressAnchor28 +
                          ((levelImage->header).resourceTables.shotAssetPathTableOffset - 0x28));
        bVar25 = false;
        if ((levelImage->header).resourceTables.shotAssetPathCount != 0) {
          do {
            WidePath_SetExtensionCode(0x746873,pwVar18);
            outputEntries = (TechnologyAsset *)0x3a;
            bVar25 = g_InGameLoadedResourcePointerCount < 0x200;
            if ((!bVar25) || (outputEntries = Package_LoadEntry(pwVar18), bVar25))
            goto 
            InGameLevelRuntime_LoadResourcesAfterDefaultResetCf_ReturnCurrentResourceLoadFailureStatus
            ;
            (pTVar6->header).common.magic = (AssetMagic)outputEntries;
            g_InGameLoadedResourcePointerCount = g_InGameLoadedResourcePointerCount + 1;
            bVar25 = (TechnologyAsset *)0xfffffffb < pTVar6;
            pTVar6 = (TechnologyAsset *)&(pTVar6->header).common.allocationSizeBytes;
            outputEntries =
                 (TechnologyAsset *)ShotAsset_PrepareEntries((ShotAssetHeader *)outputEntries);
            if (bVar25)
            goto 
            InGameLevelRuntime_LoadResourcesAfterDefaultResetCf_ReturnCurrentResourceLoadFailureStatus
            ;
            uVar28 = MoviePlayback_AdvanceScheduledFrameAndTick();
            bVar25 = (byte *)0xffffffbf < pwVar18;
            pwVar18 = pwVar18 + 0x20;
          } while ((int)((ulonglong)uVar28 >> 0x20) != 1);
        }
        outputEntries = (TechnologyAsset *)EffectDefinitions_ResolveCrossReferences();
        if (!bVar25) {
          pwVar18 = (word *)((levelImage->header).common.buildMetadata.assetRelativeAddressAnchor28
                            + ((levelImage->header).resourceTables.modelAssetPathTableOffset - 0x28)
                            );
          if ((levelImage->header).resourceTables.modelAssetPathCount != 0) {
            do {
              WidePath_SetExtensionCode(0x6c646d,pwVar18);
              outputEntries = (TechnologyAsset *)0x3a;
              bVar25 = g_InGameLoadedResourcePointerCount < 0x200;
              if ((!bVar25) || (outputEntries = Package_LoadEntry(pwVar18), bVar25))
              goto 
              InGameLevelRuntime_LoadResourcesAfterDefaultResetCf_ReturnCurrentResourceLoadFailureStatus
              ;
              (pTVar6->header).common.magic = (AssetMagic)outputEntries;
              g_InGameLoadedResourcePointerCount = g_InGameLoadedResourcePointerCount + 1;
              bVar25 = (TechnologyAsset *)0xfffffffb < pTVar6;
              pTVar6 = (TechnologyAsset *)&(pTVar6->header).common.allocationSizeBytes;
              outputEntries =
                   (TechnologyAsset *)ModelAsset_PrepareRecords((ModelAssetHeader *)outputEntries);
              if (bVar25)
              goto 
              InGameLevelRuntime_LoadResourcesAfterDefaultResetCf_ReturnCurrentResourceLoadFailureStatus
              ;
              uVar28 = MoviePlayback_AdvanceScheduledFrameAndTick();
              pwVar18 = pwVar18 + 0x20;
            } while ((int)((ulonglong)uVar28 >> 0x20) != 1);
          }
          pwVar18 = (word *)((levelImage->header).common.buildMetadata.assetRelativeAddressAnchor28
                            + ((levelImage->header).resourceTables.armyAssetPathTableOffset - 0x28))
          ;
          if ((levelImage->header).resourceTables.armyAssetPathCount != 0) {
            do {
              WidePath_SetExtensionCode(0x6d7261,pwVar18);
              outputEntries = (TechnologyAsset *)0x3a;
              bVar25 = g_InGameLoadedResourcePointerCount < 0x200;
              if ((!bVar25) || (outputEntries = Package_LoadEntry(pwVar18), bVar25))
              goto 
              InGameLevelRuntime_LoadResourcesAfterDefaultResetCf_ReturnCurrentResourceLoadFailureStatus
              ;
              (pTVar6->header).common.magic = (AssetMagic)outputEntries;
              g_InGameLoadedResourcePointerCount = g_InGameLoadedResourcePointerCount + 1;
              bVar25 = (TechnologyAsset *)0xfffffffb < pTVar6;
              pTVar6 = (TechnologyAsset *)&(pTVar6->header).common.allocationSizeBytes;
              outputEntries =
                   (TechnologyAsset *)ArmyAsset_PrepareRecords((ArmyAssetHeader *)outputEntries);
              if (bVar25)
              goto 
              InGameLevelRuntime_LoadResourcesAfterDefaultResetCf_ReturnCurrentResourceLoadFailureStatus
              ;
              uVar28 = MoviePlayback_AdvanceScheduledFrameAndTick();
              pwVar18 = pwVar18 + 0x20;
            } while ((int)((ulonglong)uVar28 >> 0x20) != 1);
          }
          outputEntries = (TechnologyAsset *)0x3a;
          if (g_InGameLoadedResourcePointerCount < 0x200) {
            uVar11 = (levelImage->header).pathOffsets.surfaceTextureBasePathOffset;
            bVar25 = CARRY4(uVar11,(uint)levelImage);
            g_MoviePlaybackBaseFrameGroup = 1;
            g_MoviePlaybackScheduleCounter = 0;
            g_MoviePlaybackScheduleSpan = 0x10000;
            uVar28 = TerrainVisualResources_LoadPrimary
                               ((word *)((levelImage->header).common.buildMetadata.
                                         assetRelativeAddressAnchor28 + (uVar11 - 0x28)),
                                (word *)((levelImage->header).common.buildMetadata.
                                         assetRelativeAddressAnchor28 +
                                        ((levelImage->header).pathOffsets.
                                         groundTextureBasePathOffset - 0x28)),
                                (FieldGridAsset *)(levelImage->header).pathOffsets.levelPathOffset);
            outputEntries = (TechnologyAsset *)uVar28;
            if ((!bVar25) &&
               (outputEntries =
                     (TechnologyAsset *)ShotDefinitions_ValidateTerrainMaterialReferences(), !bVar25
               )) {
              bVar25 = CARRY4((levelImage->header).pathOffsets.armyTextureBasePathOffset,
                              (uint)levelImage);
              g_MoviePlaybackBaseFrameGroup = 2;
              g_MoviePlaybackScheduleCounter = 0;
              g_MoviePlaybackScheduleSpan = 0x10000;
              ModelRuntimePool_Init();
              outputEntries = extraout_EAX_00;
              if ((!bVar25) &&
                 (outputEntries =
                       (TechnologyAsset *)
                       ArmyRuntime_InitializePoolAndGraphicsCf(worldRuntime,graphicsBasePath),
                 !bVar25)) {
                bVar25 = CARRY4((levelImage->header).pathOffsets.effectTextureBasePathOffset,
                                (uint)levelImage);
                g_MoviePlaybackBaseFrameGroup = 3;
                g_MoviePlaybackScheduleCounter = 0;
                g_MoviePlaybackScheduleSpan = 4;
                uVar28 = ShotRuntime_InitGraphicsResources
                                   ((word *)((levelImage->header).common.buildMetadata.
                                             assetRelativeAddressAnchor28 +
                                            ((levelImage->header).pathOffsets.
                                             shotTextureBasePathOffset - 0x28)));
                outputEntries = (TechnologyAsset *)uVar28;
                if (!bVar25) {
                  g_MoviePlaybackBaseFrameGroup = 4;
                  g_MoviePlaybackScheduleCounter = 0;
                  g_MoviePlaybackScheduleSpan = 4;
                  outputEntries =
                       (TechnologyAsset *)
                       EffectRuntime_InitGraphicsResources((word *)((ulonglong)uVar28 >> 0x20));
                  if (!bVar25) {
                    g_MoviePlaybackBaseFrameGroup = 5;
                    g_MoviePlaybackScheduleCounter = 0;
                    g_MoviePlaybackScheduleSpan = 6;
                    WorldRuntime_SetTerrainLightingConfiguration
                              (*(PackedArgb32 *)(levelImage->opaqueRuntimeTail2E0_36F + 0x2c),
                               *(PackedArgb32 *)(levelImage->opaqueRuntimeTail2E0_36F + 0x28),
                               *(PackedArgb32 *)(levelImage->opaqueRuntimeTail2E0_36F + 0x24),
                               *(PackedArgb32 *)(levelImage->opaqueRuntimeTail2E0_36F + 0x20),
                               *(PackedArgb32 *)(levelImage->opaqueRuntimeTail2E0_36F + 0x18),
                               *(PackedArgb32 *)(levelImage->opaqueRuntimeTail2E0_36F + 0x14),
                               *(PackedArgb32 *)(levelImage->opaqueRuntimeTail2E0_36F + 8),
                               *(PackedArgb32 *)(levelImage->opaqueRuntimeTail2E0_36F + 4),
                               worldRuntime);
                    uVar28 = MoviePlayback_AdvanceScheduledFrameAndTick();
                    pIVar4 = (&g_InGameConditionRuntime)[(int)((ulonglong)uVar28 >> 0x20)];
                    WorldRuntime_AttachFieldGridAsset
                              ((FieldGridAsset *)(levelImage->header).pathOffsets.levelPathOffset,
                               worldRuntime);
                    MoviePlayback_AdvanceScheduledFrameAndTick();
                    WorldRuntime_SetPosition60AndDistanceFromPosition80
                              (*(Q12 *)((int)&levelImage->playerSlots[0].startCameraZQ12 +
                                       (int)pIVar4->reserved00_4F),
                               *(Q12 *)((int)&levelImage->playerSlots[0].startCameraYQ12 +
                                       (int)pIVar4->reserved00_4F),
                               *(Q12 *)(pIVar4->reserved00_4F +
                                       (int)&levelImage->playerSlots[0].startCameraXQ12),
                               worldRuntime);
                    WorldRuntime_SetMotionParameters6CThrough78Clamped
                              (2,extraout_EDX >> 0x10,extraout_EAX_01 & 0xffff,
                               *(UQ12 *)((int)&levelImage->playerSlots[0].startCameraMagnitudeQ12 +
                                        (int)pIVar4->reserved00_4F),worldRuntime);
                    WorldRuntime_RecomputeMotionEndpointAgainstFieldSurface(worldRuntime);
                    uVar11 = WorldRuntime_CommitScalar7CFrom8C(worldRuntime);
                    WorldRuntime_RecomputeFieldRegionNormalsAndLighting
                              ((int)*(uint *)(levelImage->opaqueRuntimeTail2E0_36F + 0x10) >> 0x10,
                               *(uint *)(levelImage->opaqueRuntimeTail2E0_36F + 0x10) & 0xffff,
                               extraout_EDX_00 >> 0x10,uVar11 & 0xffff,worldRuntime);
                    pwVar18 = (word *)((levelImage->header).common.buildMetadata.
                                       assetRelativeAddressAnchor28 +
                                      ((levelImage->header).resourceTables.
                                       loadedResourcePointerArrayBytes - 0x28));
                    MoviePlayback_AdvanceScheduledFrameAndTick();
                    for (iVar12 = extraout_ECX_00; iVar12 != 0; iVar12 = iVar12 + -1) {
                      FVar2 = g_GameFactionRuntimeImage.tail.factionLifecycleStates
                              [*(PckArmyAssetIdCatalog *)(pwVar18 + 2)];
                      bVar25 = FVar2 == 0;
                      if ((FVar2 == FACTION_RUNTIME_LIFECYCLE_ACTIVE) &&
                         (outputEntries =
                               (TechnologyAsset *)
                               ArmyRuntime_CreateInstanceFromAssetCf
                                         (6,*(PckArmyAssetIdCatalog *)(pwVar18 + 8),
                                          *(PckArmyAssetIdCatalog *)(pwVar18 + 6),
                                          *(Q12 *)((AssetProducerSourceNames *)(pwVar18 + 4))->
                                                  producerName,
                                          *(PckArmyAssetIdCatalog *)(pwVar18 + 2),
                                          *(PckArmyAssetIdCatalog *)pwVar18,worldRuntime),
                         iVar12 = extraout_ECX_01, bVar25))
                      goto 
                      InGameLevelRuntime_LoadResourcesAfterDefaultResetCf_ReturnCurrentResourceLoadFailureStatus
                      ;
                      pwVar18 = pwVar18 + 0x10;
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
                              (worldRuntime->activeFactionRuntimeIndex,worldRuntime->fieldGrid);
                    MoviePlayback_AdvanceScheduledFrameAndTick();
                    pdVar23 = worldRuntime->dwordArray;
                    for (WVar13 = worldRuntime->dwordArrayCount; WVar13 != 0; WVar13 = WVar13 - 1) {
                      *pdVar23 = 0;
                      pdVar23 = pdVar23 + 1;
                    }
                    pAVar1 = &(levelImage->header).pathOffsets.soundBasePathOffset;
                    uVar26 = CARRY4((uint)levelImage,*pAVar1);
                    pbVar22 = (levelImage->header).common.buildMetadata.assetRelativeAddressAnchor28
                              + (*pAVar1 - 0x28);
                    WidePath_SetExtensionCode(0x6d6173,(word *)pbVar22);
                    WidePath_SplitParentAndLeaf
                              ((word *)&g_InGameLevelSoundLeafOrCombinedPathScratchUtf16,
                               (word *)&g_InGameLevelSoundParentDirectoryScratchUtf16,
                               (word *)pbVar22);
                    outputEntries = (*g_MemoryApi.allocLargestFreeBlock)();
                    if (!(bool)uVar26) {
                      bVar25 = false;
                      outputCapacityBytes_00 = outputCapacityBytes;
                      pTVar6 = (TechnologyAsset *)
                               Package_FindEntry(outputCapacityBytes,(PckEntryHeader *)outputEntries
                                                 ,(word *)pbVar22,g_SoundPackageHandle);
                      iVar12 = extraout_ECX_02;
                      memory = outputRecords;
                      if (bVar25) {
                        pTVar6 = (TechnologyAsset *)
                                 (*g_FileSystemEnumerateDirectoryOrVolumeEntriesCf)
                                           (FILESYSTEM_ENUMERATE_FILES,0xffffffff,
                                            outputCapacityBytes_00,outputRecords,pbVar22);
                        iVar12 = extraout_ECX_03;
                        memory = extraout_EDX_01;
                      }
                      pTVar7 = pTVar6;
                      if (&stack0xffffffd0 < (undefined1 *)0xfffffffc) {
                        g_MoviePlaybackBaseFrameGroup = 6;
                        g_MoviePlaybackScheduleCounter = 0;
                        dVar8 = (dword)((longlong)(int)pTVar6 * (longlong)iVar12);
                        bVar27 = (longlong)(int)dVar8 != (longlong)(int)pTVar6 * (longlong)iVar12;
                        g_MoviePlaybackScheduleSpan = iVar12;
                        uVar28 = (*g_MemoryApi.shrinkInPlace)(dVar8,memory);
                        pwVar18 = (word *)((ulonglong)uVar28 >> 0x20);
                        pTVar7 = (TechnologyAsset *)uVar28;
                        if (!bVar27) {
                          worldContext1 = worldRuntime;
                          uVar11 = extraout_ECX_04;
                          pIVar4 = g_InGameConditionRuntime;
                          if (worldRuntime->dwordArrayCount < extraout_ECX_04) {
                            uVar11 = worldRuntime->dwordArrayCount;
                          }
                          do {
                            g_InGameConditionRuntime = pIVar4;
                            if (uVar11 == 0) {
                              (*g_MemoryApi.free)(outputEntries);
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
                              uVar26 = 0;
                              if (*(int *)(pIVar4[3].reservedB0_D7 + 0x1c) != 0) {
                                (*g_WideNumberFormatUtf16)
                                          (WIDE_FORMAT_PAD_WITH_ZERO,0,2,1,
                                           *(sdword *)(pIVar4[3].reservedB0_D7 + 0x1c),
                                           (word *)(u_sound_level00_sam_0050df6c + 0xb));
                                uVar28 = Resource_Load(extraout_ECX_07,extraout_EDX_03,
                                                       (word *)u_sound_level00_sam_0050df6c);
                                if (!(bool)uVar26) {
                                  pDVar10 = (*g_SoundCreateSampleVoiceSet)
                                                      ((SoundSampleAsset *)uVar28);
                                  if (!(bool)uVar26) {
                                    g_InGameLevelEffectVoiceSet0 = pDVar10;
                                  }
                                  Resource_Release((SoundSampleAsset *)uVar28);
                                }
                              }
                              uVar26 = 0;
                              if (*(int *)(pIVar4[3].reservedB0_D7 + 0x20) != 0) {
                                (*g_WideNumberFormatUtf16)
                                          (WIDE_FORMAT_PAD_WITH_ZERO,0,2,1,
                                           *(sdword *)(pIVar4[3].reservedB0_D7 + 0x20),
                                           (word *)(u_sound_level00_sam_0050df6c + 0xb));
                                uVar28 = Resource_Load(extraout_ECX_08,extraout_EDX_04,
                                                       (word *)u_sound_level00_sam_0050df6c);
                                if (!(bool)uVar26) {
                                  pDVar10 = (*g_SoundCreateSampleVoiceSet)
                                                      ((SoundSampleAsset *)uVar28);
                                  if (!(bool)uVar26) {
                                    g_InGameLevelEffectVoiceSet1 = pDVar10;
                                  }
                                  Resource_Release((SoundSampleAsset *)uVar28);
                                }
                              }
                              uVar26 = 0;
                              if (*(int *)(pIVar4[3].reservedB0_D7 + 0x24) != 0) {
                                (*g_WideNumberFormatUtf16)
                                          (WIDE_FORMAT_PAD_WITH_ZERO,0,2,1,
                                           *(sdword *)(pIVar4[3].reservedB0_D7 + 0x24),
                                           (word *)(u_sound_level00_sam_0050df6c + 0xb));
                                uVar28 = Resource_Load(extraout_ECX_09,extraout_EDX_05,
                                                       (word *)u_sound_level00_sam_0050df6c);
                                if (!(bool)uVar26) {
                                  pDVar10 = (*g_SoundCreateSampleVoiceSet)
                                                      ((SoundSampleAsset *)uVar28);
                                  if (!(bool)uVar26) {
                                    g_InGameLevelEffectVoiceSet2 = pDVar10;
                                  }
                                  Resource_Release((SoundSampleAsset *)uVar28);
                                }
                              }
                              uVar26 = 0;
                              if (pIVar4[3].tailRecordD8 != (InGameConditionRecord *)0x0) {
                                (*g_WideNumberFormatUtf16)
                                          (WIDE_FORMAT_PAD_WITH_ZERO,0,2,1,
                                           (sdword)pIVar4[3].tailRecordD8,
                                           (word *)(u_sound_level00_sam_0050df6c + 0xb));
                                uVar28 = Resource_Load(extraout_ECX_10,extraout_EDX_06,
                                                       (word *)u_sound_level00_sam_0050df6c);
                                if (!(bool)uVar26) {
                                  pDVar10 = (*g_SoundCreateSampleVoiceSet)
                                                      ((SoundSampleAsset *)uVar28);
                                  if (!(bool)uVar26) {
                                    g_InGameLevelEffectVoiceSet3 = pDVar10;
                                  }
                                  Resource_Release((SoundSampleAsset *)uVar28);
                                }
                              }
                              uVar26 = 0;
                              if (*(int *)(pIVar4[3].reservedB0_D7 + 0xc) != 0) {
                                (*g_WideNumberFormatUtf16)
                                          (WIDE_FORMAT_PAD_WITH_ZERO,0,2,1,
                                           *(sdword *)(pIVar4[3].reservedB0_D7 + 0xc),
                                           (word *)(u_sound_music00_sam_0050df90 + 0xb));
                                uVar28 = Resource_Load(extraout_ECX_11,extraout_EDX_07,
                                                       (word *)u_sound_music00_sam_0050df90);
                                if (!(bool)uVar26) {
                                  pDVar10 = (*g_SoundCreateSampleVoiceSet)
                                                      ((SoundSampleAsset *)uVar28);
                                  if (!(bool)uVar26) {
                                    g_InGameLevelMusicVoiceSet0 = pDVar10;
                                  }
                                  Resource_Release((SoundSampleAsset *)uVar28);
                                }
                              }
                              uVar26 = 0;
                              if (*(int *)(pIVar4[3].reservedB0_D7 + 0x10) != 0) {
                                (*g_WideNumberFormatUtf16)
                                          (WIDE_FORMAT_PAD_WITH_ZERO,0,2,1,
                                           *(sdword *)(pIVar4[3].reservedB0_D7 + 0x10),
                                           (word *)(u_sound_music00_sam_0050df90 + 0xb));
                                uVar28 = Resource_Load(extraout_ECX_12,extraout_EDX_08,
                                                       (word *)u_sound_music00_sam_0050df90);
                                if (!(bool)uVar26) {
                                  pDVar10 = (*g_SoundCreateSampleVoiceSet)
                                                      ((SoundSampleAsset *)uVar28);
                                  if (!(bool)uVar26) {
                                    g_InGameLevelMusicVoiceSet1 = pDVar10;
                                  }
                                  Resource_Release((SoundSampleAsset *)uVar28);
                                }
                              }
                              uVar26 = 0;
                              if (*(int *)(pIVar4[3].reservedB0_D7 + 0x14) != 0) {
                                (*g_WideNumberFormatUtf16)
                                          (WIDE_FORMAT_PAD_WITH_ZERO,0,2,1,
                                           *(sdword *)(pIVar4[3].reservedB0_D7 + 0x14),
                                           (word *)(u_sound_music00_sam_0050df90 + 0xb));
                                uVar28 = Resource_Load(extraout_ECX_13,extraout_EDX_09,
                                                       (word *)u_sound_music00_sam_0050df90);
                                if (!(bool)uVar26) {
                                  pDVar10 = (*g_SoundCreateSampleVoiceSet)
                                                      ((SoundSampleAsset *)uVar28);
                                  if (!(bool)uVar26) {
                                    g_InGameLevelMusicVoiceSet2 = pDVar10;
                                  }
                                  Resource_Release((SoundSampleAsset *)uVar28);
                                }
                              }
                              uVar26 = 0;
                              if (*(int *)(pIVar4[3].reservedB0_D7 + 0x18) != 0) {
                                (*g_WideNumberFormatUtf16)
                                          (WIDE_FORMAT_PAD_WITH_ZERO,0,2,1,
                                           *(sdword *)(pIVar4[3].reservedB0_D7 + 0x18),
                                           (word *)(u_sound_music00_sam_0050df90 + 0xb));
                                uVar28 = Resource_Load(extraout_ECX_14,extraout_EDX_10,
                                                       (word *)u_sound_music00_sam_0050df90);
                                if (!(bool)uVar26) {
                                  pDVar10 = (*g_SoundCreateSampleVoiceSet)
                                                      ((SoundSampleAsset *)uVar28);
                                  if (!(bool)uVar26) {
                                    g_InGameLevelMusicVoiceSet3 = pDVar10;
                                  }
                                  Resource_Release((SoundSampleAsset *)uVar28);
                                }
                              }
                              ppAVar19 = g_ArmyAssetRecordRegistry;
                              iVar12 = 0x300;
                              local_20 = (ArmyAssetRecordPrefix *)0x0;
                              local_24 = (ArmyAssetRecordPrefix *)0x0;
                              do {
                                armyDefinition1 = *ppAVar19;
                                armyDefinition3 = local_2c;
                                armyDefinition2 = local_28;
                                armyDefinition4 = local_20;
                                if ((armyDefinition1 != (ArmyAssetRecordPrefix *)0x0) &&
                                   ((armyDefinition1[1].selectionDetailTemplateVariantIndex & 1) !=
                                    0)) {
                                  modelDefinition1 =
                                       ModelDefinitionRegistry_FindByIdWithErrorCf
                                                 (*(PckModelDefinitionIdCatalog *)
                                                   (armyDefinition1->rootNodeOffsetOrPointer + 0x20)
                                                 );
                                  dVar8 = modelDefinition1[6].flags;
                                  iVar12 = extraout_EDX_11;
                                  armyDefinition4 = armyDefinition1;
                                  if ((dVar8 != 0xb) &&
                                     (((armyDefinition3 = armyDefinition1,
                                       armyDefinition4 = local_20, dVar8 != 0x10 &&
                                       (armyDefinition3 = local_2c, dVar8 == 0xe)) &&
                                      (armyDefinition2 = armyDefinition1,
                                      modelDefinition1[0x10].byteSize == 0)))) {
                                    armyDefinition2 = local_28;
                                    local_24 = armyDefinition1;
                                  }
                                }
                                local_20 = armyDefinition4;
                                local_28 = armyDefinition2;
                                local_2c = armyDefinition3;
                                ppAVar19 = ppAVar19 + 1;
                                iVar12 = iVar12 + -1;
                              } while (iVar12 != 0);
                              if ((local_20 != (ArmyAssetRecordPrefix *)0x0) &&
                                 (local_24 != (ArmyAssetRecordPrefix *)0x0)) {
                                abVar14[0] = 1;
                                abVar14[1] = 0;
                                abVar14[2] = 0;
                                abVar14[3] = 0;
                                worldNode1 = worldRuntime->ownerListHead;
                                uVar11 = 0;
                                if (worldNode1 != (WorldRuntimeNode *)0x0) {
                                  do {
                                    do {
                                      if ((worldNode1[2].common.nextNode == (WorldRuntimeNode *)0x0)
                                         && (abVar14 ==
                                             *(byte (*) [4])
                                              (*(int *)((int)worldNode1->runtimePayload + 8) + 0xc))
                                         ) {
                                        iVar12 = *(int *)(*(int *)worldNode1->runtimePayload + 0x4c)
                                        ;
                                        if (iVar12 == 0x12) {
                                          uVar11 = uVar11 | 2;
                                        }
                                        else if (g_ArmyRuntimeOrderHandlerMatrix11Columns24Classes.
                                                 classCommand[iVar12] ==
                                                 ArmyRuntime_ClassCommandHandlerGroupACf) {
                                          uVar11 = uVar11 | 1;
                                        }
                                      }
                                      worldNode1 = (worldNode1->common).nextNode;
                                    } while (worldNode1 != (WorldRuntimeNode *)0x0);
                                    if (uVar11 == 2) {
                                      g_GameFactionRuntimeImage.records[(int)abVar14].
                                      primaryArmyAssetPointersOrIds[0] = (dword)local_20;
                                      g_GameFactionRuntimeImage.records[(int)abVar14].
                                      primaryArmyAssetPointersOrIds[1] = (dword)local_24;
                                      g_GameFactionRuntimeImage.records[(int)abVar14].
                                      primaryArmyAssetPointersOrIds[2] = (dword)local_28;
                                      g_GameFactionRuntimeImage.records[(int)abVar14].
                                      primaryArmyAssetPointersOrIds[3] = (dword)local_2c;
                                      g_GameFactionRuntimeImage.records[(int)abVar14].
                                      primaryArmyAssetCount = 4;
                                    }
                                    worldNode1 = worldRuntime->ownerListHead;
                                    abVar14 = (byte  [4])((int)abVar14 + 1);
                                    uVar11 = 0;
                                  } while ((uint)abVar14 <=
                                           (uint)g_GameFactionRuntimeImage.tail.reserved00_03);
                                }
                              }
                              uVar11 = *(uint *)(g_InGameConditionRuntime[3].reserved5C_AB + 0x28);
                              uVar15 = *(uint *)(g_InGameConditionRuntime[3].reserved5C_AB + 0x30);
                              uVar24 = *(uint *)(g_InGameConditionRuntime[3].reserved5C_AB + 0x2c);
                              g_GameFactionRuntimeImage.tail.relationUiFlags = uVar11;
                              uVar20 = uVar15 & 0xff;
                              while (uVar20 != 0) {
                                uVar28 = CONCAT44(uVar15,uVar11);
                                uVar15 = 1;
                                uVar20 = 0;
                                do {
                                  uVar11 = (uint)((ulonglong)uVar28 >> 0x20);
                                  if ((uVar11 & uVar15) != 0) {
                                    uVar28 = CONCAT44(uVar11,uVar20);
                                    uVar16 = uVar15;
                                    uVar11 = uVar20;
                                    while (uVar11 = uVar11 + 1, uVar11 < 8) {
                                      uVar16 = uVar16 * 2;
                                      if (((uint)((ulonglong)uVar28 >> 0x20) & uVar16) != 0) {
                                        uVar28 = GameFactionRuntime_ApplyPairwiseRelationTransition
                                                           (0x20,0x20,4,4,uVar11,
                                                            (FactionRuntimeIndex)uVar28);
                                        uVar16 = extraout_ECX_15;
                                      }
                                    }
                                  }
                                  uVar11 = (uint)uVar28;
                                  uVar20 = uVar20 + 1;
                                  uVar15 = uVar15 * 2;
                                } while (uVar20 != 8);
                                uVar15 = (uint)((ulonglong)uVar28 >> 0x28);
                                uVar20 = uVar15;
                              }
                              uVar15 = uVar24 & 0xff;
                              while (uVar15 != 0) {
                                uVar15 = 1;
                                uVar20 = 0;
                                do {
                                  uVar17 = uVar15;
                                  uVar5 = uVar20;
                                  uVar16 = uVar20;
                                  if ((uVar24 & uVar15) != 0) {
                                    while (uVar11 = uVar5, uVar16 = uVar16 + 1, uVar16 < 8) {
                                      uVar17 = uVar17 * 2;
                                      uVar5 = uVar11;
                                      if ((uVar24 & uVar17) != 0) {
                                        uVar28 = GameFactionRuntime_ApplyPairwiseRelationTransition
                                                           (0x20,0x20,8,8,uVar16,uVar11);
                                        uVar17 = extraout_ECX_16;
                                        uVar5 = (uint)uVar28;
                                      }
                                    }
                                  }
                                  uVar20 = uVar20 + 1;
                                  uVar15 = uVar15 * 2;
                                } while (uVar20 != 8);
                                uVar24 = uVar24 >> 8;
                                uVar15 = uVar24;
                              }
                              g_GameFactionRuntimeImage.tail.reserved00_03[0] =
                                   g_GameFactionRuntimeImage.tail.reserved00_03[0];
                              g_GameFactionRuntimeImage.tail.reserved00_03[1] =
                                   g_GameFactionRuntimeImage.tail.reserved00_03[1];
                              g_GameFactionRuntimeImage.tail.reserved00_03[2] =
                                   g_GameFactionRuntimeImage.tail.reserved00_03[2];
                              g_GameFactionRuntimeImage.tail.reserved00_03[3] =
                                   g_GameFactionRuntimeImage.tail.reserved00_03[3];
                              return CONCAT44(param_2,uVar11);
                            }
                            pdVar23 = worldContext1->dwordArray;
                            pWVar29 = worldContext1;
                            uVar28 = WidePath_ParseTrailingNumberBeforeExtensionRegs(pwVar18);
                            pwVar18 = (word *)((ulonglong)uVar28 >> 0x20);
                            if (extraout_ECX_05 < worldContext1->dwordArrayCount) {
                              uVar26 = false;
                              if (!bVar25) {
                                uVar28 = Resource_Load(extraout_ECX_05,pwVar18,pwVar18);
                                pTVar7 = (TechnologyAsset *)uVar28;
                                if ((bool)uVar26) break;
                              }
                              else {
                                WidePath_CombineDirectoryAndLeaf
                                          ((word *)&g_InGameLevelSoundLeafOrCombinedPathScratchUtf16
                                           ,pwVar18,(word *)&
                                                  g_InGameLevelSoundParentDirectoryScratchUtf16);
                                uVar28 = Resource_Load(extraout_ECX_06,extraout_EDX_02,
                                                       (word *)&
                                                  g_InGameLevelSoundLeafOrCombinedPathScratchUtf16);
                                pTVar7 = (TechnologyAsset *)uVar28;
                                if ((bool)uVar26) break;
                              }
                              bVar27 = false;
                              pSVar9 = SpatialSoundSlot_CreateFromSampleAsset
                                                 ((SoundSampleAsset *)pTVar7);
                              if (!bVar27) {
                                pdVar23[extraout_ECX_05] = (dword)pSVar9;
                              }
                              Resource_Release(pTVar7);
                              uVar28 = MoviePlayback_AdvanceScheduledFrameAndTick();
                              pwVar18 = (word *)((ulonglong)uVar28 >> 0x20);
                            }
                            pwVar18 = (word *)((int)pwVar18 + (int)pTVar6);
                            uVar11 = uVar11 - 1;
                            worldContext1 = pWVar29;
                            pIVar4 = g_InGameConditionRuntime;
                          } while( true );
                        }
                      }
                      (*g_MemoryApi.free)(outputEntries);
                      outputEntries = pTVar7;
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
  return CONCAT44(param_2,outputEntries);
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
undefined8 __fastcall
InGameLevelRuntime_LoadResourcesAfterExternalTablesCf
          (undefined4 param_1,undefined4 param_2,LevelAssetRuntimeImagePrefix370 *levelImage,
          WorldRuntimeContext *worldRuntime)

{
  AssetRelativeOffset *pAVar1;
  PckConverterVersion PVar2;
  InGameConditionRuntime *pIVar3;
  longlong lVar4;
  TechnologyAsset *pTVar5;
  TechnologyAsset *extraout_EAX;
  TechnologyAsset *outputEntries;
  TechnologyAsset *pTVar6;
  TechnologyAsset *extraout_EAX_00;
  uint extraout_EAX_01;
  dword newSize;
  SpatialSoundSlot *pSVar7;
  SoundSampleAsset *arg0;
  DirectSoundVoiceSet *pDVar8;
  uint extraout_ECX;
  uint uVar9;
  WorldWorkspaceElementCount WVar10;
  PckOutputCapacityBytes outputCapacityBytes;
  int extraout_ECX_00;
  int extraout_ECX_01;
  uint extraout_ECX_02;
  uint extraout_ECX_03;
  undefined4 extraout_ECX_04;
  undefined4 extraout_ECX_05;
  undefined4 extraout_ECX_06;
  undefined4 extraout_ECX_07;
  undefined4 extraout_ECX_08;
  undefined4 extraout_ECX_09;
  undefined4 extraout_ECX_10;
  undefined4 extraout_ECX_11;
  undefined4 extraout_ECX_12;
  word *graphicsBasePath;
  int extraout_EDX;
  int extraout_EDX_00;
  int iVar11;
  FieldGridDimensionCells gridHeight;
  PckLoadCapacityFlags bufferCapacityAndLoadFlags;
  byte *outputRecords;
  byte *extraout_EDX_01;
  byte *memory;
  undefined4 extraout_EDX_02;
  word *pwVar12;
  undefined4 extraout_EDX_03;
  undefined4 extraout_EDX_04;
  undefined4 extraout_EDX_05;
  undefined4 extraout_EDX_06;
  undefined4 extraout_EDX_07;
  undefined4 extraout_EDX_08;
  undefined4 extraout_EDX_09;
  undefined4 extraout_EDX_10;
  LevelAssetRuntimeImagePrefix370 *pLVar13;
  byte *pbVar14;
  dword *pdVar15;
  bool bVar16;
  undefined1 uVar17;
  bool bVar18;
  undefined8 uVar19;
  PckOutputCapacityBytes outputCapacityBytes_00;
  WorldRuntimeContext *worldContext1;
  
  bVar16 = &stack0xffffffe8 < (undefined1 *)0xc;
  pTVar5 = (*g_MemoryApi.alloc)(0x800);
  outputEntries = pTVar5;
  if (!bVar16) {
    g_InGameLoadedResourcePointerCount = 0;
    g_InGameLoadedResourcePointers = pTVar5;
    Package_SetLastErrorPath(g_LevelEndingMovieSourcePath);
    outputEntries = extraout_EAX;
    if ((((levelImage->header).common.magic == ASSET_MAGIC_LEV) &&
        (PVar2 = (levelImage->header).common.converterVersion,
        bVar16 = PVar2 < PCK_CONVERTER_LEV_00070001, outputEntries = extraout_EAX,
        PVar2 == PCK_CONVERTER_LEV_00070001)) &&
       (outputEntries =
             (*g_MemoryApi.alloc)
                       ((levelImage->header).resourceTables.loadedResourcePointerArrayBytes),
       !bVar16)) {
      pLVar13 = levelImage;
      g_InGameConditionRuntime = (InGameConditionRuntime *)outputEntries;
      for (uVar9 = extraout_ECX >> 2; uVar9 != 0; uVar9 = uVar9 - 1) {
        (outputEntries->header).common.magic = (pLVar13->header).common.magic;
        pLVar13 = (LevelAssetRuntimeImagePrefix370 *)&(pLVar13->header).common.allocationSizeBytes;
        outputEntries = (TechnologyAsset *)&(outputEntries->header).common.allocationSizeBytes;
      }
      g_InGameLevelTitleTextResourceIndex = (levelImage->header).titleTextResourceIndex;
      g_InGameLevelCampaignAssociationIndex = (levelImage->header).campaignAssociationIndex;
      uVar9 = (levelImage->header).pathOffsets.technologyPathOffset;
      uVar17 = CARRY4(uVar9,(uint)levelImage);
      pbVar14 = (levelImage->header).common.buildMetadata.assetRelativeAddressAnchor28 +
                (uVar9 - 0x28);
      WidePath_SetExtensionCode(0x636574,(word *)pbVar14);
      pTVar6 = Package_LoadEntry((word *)pbVar14);
      outputEntries = pTVar6;
      if (((!(bool)uVar17) &&
          (outputEntries = (TechnologyAsset *)&k_LowAddressLiteral0000004F,
          g_TechnologyAsset = pTVar6, (pTVar6->header).common.magic == ASSET_MAGIC_TEC)) &&
         ((pTVar6->header).common.converterVersion == PCK_CONVERTER_TEC_00020000)) {
        g_MoviePlaybackScheduleSpan =
             (levelImage->header).resourceTables.effectAssetPathCount +
             (levelImage->header).resourceTables.shotAssetPathCount +
             (levelImage->header).resourceTables.modelAssetPathCount +
             (levelImage->header).resourceTables.armyAssetPathCount;
        g_MoviePlaybackBaseFrameGroup = 0;
        g_MoviePlaybackScheduleCounter = 0;
        pwVar12 = (word *)((levelImage->header).common.buildMetadata.assetRelativeAddressAnchor28 +
                          ((levelImage->header).resourceTables.effectAssetPathTableOffset - 0x28));
        if ((levelImage->header).resourceTables.effectAssetPathCount != 0) {
          do {
            WidePath_SetExtensionCode(0x666665,pwVar12);
            outputEntries = (TechnologyAsset *)0x3a;
            bVar16 = g_InGameLoadedResourcePointerCount < 0x200;
            if ((!bVar16) || (outputEntries = Package_LoadEntry(pwVar12), bVar16))
            goto 
            InGameLevelRuntime_LoadResourcesAfterExternalTablesCf_ReturnCurrentResourceLoadFailureStatus
            ;
            (pTVar5->header).common.magic = (AssetMagic)outputEntries;
            g_InGameLoadedResourcePointerCount = g_InGameLoadedResourcePointerCount + 1;
            bVar16 = (TechnologyAsset *)0xfffffffb < pTVar5;
            pTVar5 = (TechnologyAsset *)&(pTVar5->header).common.allocationSizeBytes;
            outputEntries =
                 (TechnologyAsset *)EffectAsset_PrepareEntries((EffectAssetHeader *)outputEntries);
            if (bVar16)
            goto 
            InGameLevelRuntime_LoadResourcesAfterExternalTablesCf_ReturnCurrentResourceLoadFailureStatus
            ;
            uVar19 = MoviePlayback_AdvanceScheduledFrameAndTick();
            pwVar12 = pwVar12 + 0x20;
          } while ((int)((ulonglong)uVar19 >> 0x20) != 1);
        }
        pwVar12 = (word *)((levelImage->header).common.buildMetadata.assetRelativeAddressAnchor28 +
                          ((levelImage->header).resourceTables.shotAssetPathTableOffset - 0x28));
        bVar16 = false;
        if ((levelImage->header).resourceTables.shotAssetPathCount != 0) {
          do {
            WidePath_SetExtensionCode(0x746873,pwVar12);
            outputEntries = (TechnologyAsset *)0x3a;
            bVar16 = g_InGameLoadedResourcePointerCount < 0x200;
            if ((!bVar16) || (outputEntries = Package_LoadEntry(pwVar12), bVar16))
            goto 
            InGameLevelRuntime_LoadResourcesAfterExternalTablesCf_ReturnCurrentResourceLoadFailureStatus
            ;
            (pTVar5->header).common.magic = (AssetMagic)outputEntries;
            g_InGameLoadedResourcePointerCount = g_InGameLoadedResourcePointerCount + 1;
            bVar16 = (TechnologyAsset *)0xfffffffb < pTVar5;
            pTVar5 = (TechnologyAsset *)&(pTVar5->header).common.allocationSizeBytes;
            outputEntries =
                 (TechnologyAsset *)ShotAsset_PrepareEntries((ShotAssetHeader *)outputEntries);
            if (bVar16)
            goto 
            InGameLevelRuntime_LoadResourcesAfterExternalTablesCf_ReturnCurrentResourceLoadFailureStatus
            ;
            uVar19 = MoviePlayback_AdvanceScheduledFrameAndTick();
            bVar16 = (byte *)0xffffffbf < pwVar12;
            pwVar12 = pwVar12 + 0x20;
          } while ((int)((ulonglong)uVar19 >> 0x20) != 1);
        }
        outputEntries = (TechnologyAsset *)EffectDefinitions_ResolveCrossReferences();
        if (!bVar16) {
          pwVar12 = (word *)((levelImage->header).common.buildMetadata.assetRelativeAddressAnchor28
                            + ((levelImage->header).resourceTables.modelAssetPathTableOffset - 0x28)
                            );
          if ((levelImage->header).resourceTables.modelAssetPathCount != 0) {
            do {
              WidePath_SetExtensionCode(0x6c646d,pwVar12);
              outputEntries = (TechnologyAsset *)0x3a;
              bVar16 = g_InGameLoadedResourcePointerCount < 0x200;
              if ((!bVar16) || (outputEntries = Package_LoadEntry(pwVar12), bVar16))
              goto 
              InGameLevelRuntime_LoadResourcesAfterExternalTablesCf_ReturnCurrentResourceLoadFailureStatus
              ;
              (pTVar5->header).common.magic = (AssetMagic)outputEntries;
              g_InGameLoadedResourcePointerCount = g_InGameLoadedResourcePointerCount + 1;
              bVar16 = (TechnologyAsset *)0xfffffffb < pTVar5;
              pTVar5 = (TechnologyAsset *)&(pTVar5->header).common.allocationSizeBytes;
              outputEntries =
                   (TechnologyAsset *)ModelAsset_PrepareRecords((ModelAssetHeader *)outputEntries);
              if (bVar16)
              goto 
              InGameLevelRuntime_LoadResourcesAfterExternalTablesCf_ReturnCurrentResourceLoadFailureStatus
              ;
              uVar19 = MoviePlayback_AdvanceScheduledFrameAndTick();
              pwVar12 = pwVar12 + 0x20;
            } while ((int)((ulonglong)uVar19 >> 0x20) != 1);
          }
          pwVar12 = (word *)((levelImage->header).common.buildMetadata.assetRelativeAddressAnchor28
                            + ((levelImage->header).resourceTables.armyAssetPathTableOffset - 0x28))
          ;
          if ((levelImage->header).resourceTables.armyAssetPathCount != 0) {
            do {
              WidePath_SetExtensionCode(0x6d7261,pwVar12);
              outputEntries = (TechnologyAsset *)0x3a;
              bVar16 = g_InGameLoadedResourcePointerCount < 0x200;
              if ((!bVar16) || (outputEntries = Package_LoadEntry(pwVar12), bVar16))
              goto 
              InGameLevelRuntime_LoadResourcesAfterExternalTablesCf_ReturnCurrentResourceLoadFailureStatus
              ;
              (pTVar5->header).common.magic = (AssetMagic)outputEntries;
              g_InGameLoadedResourcePointerCount = g_InGameLoadedResourcePointerCount + 1;
              bVar16 = (TechnologyAsset *)0xfffffffb < pTVar5;
              pTVar5 = (TechnologyAsset *)&(pTVar5->header).common.allocationSizeBytes;
              outputEntries =
                   (TechnologyAsset *)ArmyAsset_PrepareRecords((ArmyAssetHeader *)outputEntries);
              if (bVar16)
              goto 
              InGameLevelRuntime_LoadResourcesAfterExternalTablesCf_ReturnCurrentResourceLoadFailureStatus
              ;
              uVar19 = MoviePlayback_AdvanceScheduledFrameAndTick();
              pwVar12 = pwVar12 + 0x20;
            } while ((int)((ulonglong)uVar19 >> 0x20) != 1);
          }
          outputEntries = (TechnologyAsset *)0x3a;
          if (g_InGameLoadedResourcePointerCount < 0x200) {
            uVar9 = (levelImage->header).pathOffsets.surfaceTextureBasePathOffset;
            bVar16 = CARRY4(uVar9,(uint)levelImage);
            g_MoviePlaybackBaseFrameGroup = 1;
            g_MoviePlaybackScheduleCounter = 0;
            g_MoviePlaybackScheduleSpan = 0x10000;
            uVar19 = TerrainVisualResources_LoadAndClearCellOverlayFlags
                               ((word *)((levelImage->header).common.buildMetadata.
                                         assetRelativeAddressAnchor28 + (uVar9 - 0x28)),
                                (word *)((levelImage->header).common.buildMetadata.
                                         assetRelativeAddressAnchor28 +
                                        ((levelImage->header).pathOffsets.
                                         groundTextureBasePathOffset - 0x28)),
                                (FieldGridAsset *)(levelImage->header).pathOffsets.levelPathOffset);
            outputEntries = (TechnologyAsset *)uVar19;
            if ((!bVar16) &&
               (outputEntries =
                     (TechnologyAsset *)ShotDefinitions_ValidateTerrainMaterialReferences(), !bVar16
               )) {
              bVar16 = CARRY4((levelImage->header).pathOffsets.armyTextureBasePathOffset,
                              (uint)levelImage);
              g_MoviePlaybackBaseFrameGroup = 2;
              g_MoviePlaybackScheduleCounter = 0;
              g_MoviePlaybackScheduleSpan = 0x10000;
              ModelRuntimePool_Init();
              outputEntries = extraout_EAX_00;
              if ((!bVar16) &&
                 (outputEntries =
                       (TechnologyAsset *)
                       ArmyRuntime_InitializePoolAndGraphicsCf(worldRuntime,graphicsBasePath),
                 !bVar16)) {
                bVar16 = CARRY4((levelImage->header).pathOffsets.effectTextureBasePathOffset,
                                (uint)levelImage);
                g_MoviePlaybackBaseFrameGroup = 3;
                g_MoviePlaybackScheduleCounter = 0;
                g_MoviePlaybackScheduleSpan = 4;
                uVar19 = ShotRuntime_InitGraphicsResources
                                   ((word *)((levelImage->header).common.buildMetadata.
                                             assetRelativeAddressAnchor28 +
                                            ((levelImage->header).pathOffsets.
                                             shotTextureBasePathOffset - 0x28)));
                outputEntries = (TechnologyAsset *)uVar19;
                if (!bVar16) {
                  g_MoviePlaybackBaseFrameGroup = 4;
                  g_MoviePlaybackScheduleCounter = 0;
                  g_MoviePlaybackScheduleSpan = 4;
                  outputEntries =
                       (TechnologyAsset *)
                       EffectRuntime_InitGraphicsResources((word *)((ulonglong)uVar19 >> 0x20));
                  if (!bVar16) {
                    g_MoviePlaybackBaseFrameGroup = 5;
                    g_MoviePlaybackScheduleCounter = 0;
                    g_MoviePlaybackScheduleSpan = 6;
                    GameFactionRuntime_RebaseLoadedArmyReferences();
                    WorldRuntime_SetTerrainLightingConfiguration
                              (*(PackedArgb32 *)(levelImage->opaqueRuntimeTail2E0_36F + 0x2c),
                               *(PackedArgb32 *)(levelImage->opaqueRuntimeTail2E0_36F + 0x28),
                               *(PackedArgb32 *)(levelImage->opaqueRuntimeTail2E0_36F + 0x24),
                               *(PackedArgb32 *)(levelImage->opaqueRuntimeTail2E0_36F + 0x20),
                               *(PackedArgb32 *)(levelImage->opaqueRuntimeTail2E0_36F + 0x18),
                               *(PackedArgb32 *)(levelImage->opaqueRuntimeTail2E0_36F + 0x14),
                               *(PackedArgb32 *)(levelImage->opaqueRuntimeTail2E0_36F + 8),
                               *(PackedArgb32 *)(levelImage->opaqueRuntimeTail2E0_36F + 4),
                               worldRuntime);
                    uVar19 = MoviePlayback_AdvanceScheduledFrameAndTick();
                    pIVar3 = (&g_InGameConditionRuntime)[(int)((ulonglong)uVar19 >> 0x20)];
                    WorldRuntime_AttachFieldGridAsset
                              ((FieldGridAsset *)(levelImage->header).pathOffsets.levelPathOffset,
                               worldRuntime);
                    MoviePlayback_AdvanceScheduledFrameAndTick();
                    WorldRuntime_SetPosition60AndDistanceFromPosition80
                              (*(Q12 *)((int)&levelImage->playerSlots[0].startCameraZQ12 +
                                       (int)pIVar3->reserved00_4F),
                               *(Q12 *)((int)&levelImage->playerSlots[0].startCameraYQ12 +
                                       (int)pIVar3->reserved00_4F),
                               *(Q12 *)(pIVar3->reserved00_4F +
                                       (int)&levelImage->playerSlots[0].startCameraXQ12),
                               worldRuntime);
                    WorldRuntime_SetMotionParameters6CThrough78Clamped
                              (2,extraout_EDX >> 0x10,extraout_EAX_01 & 0xffff,
                               *(UQ12 *)((int)&levelImage->playerSlots[0].startCameraMagnitudeQ12 +
                                        (int)pIVar3->reserved00_4F),worldRuntime);
                    WorldRuntime_RecomputeMotionEndpointAgainstFieldSurface(worldRuntime);
                    uVar9 = WorldRuntime_CommitScalar7CFrom8C(worldRuntime);
                    uVar9 = uVar9 & 0xffff;
                    iVar11 = extraout_EDX_00 >> 0x10;
                    worldContext1 = worldRuntime;
                    uVar19 = MoviePlayback_AdvanceScheduledFrameAndTick();
                    gridHeight = (FieldGridDimensionCells)((ulonglong)uVar19 >> 0x20);
                    WorldRuntime_RecomputeFieldRegionNormalsAndLighting
                              (gridHeight,(FieldGridDimensionCells)uVar19,iVar11,uVar9,worldContext1
                              );
                    lVar4 = (longlong)(int)worldRuntime->objectCount * 0x100;
                    bufferCapacityAndLoadFlags = (PckLoadCapacityFlags)lVar4;
                    bVar16 = (int)bufferCapacityAndLoadFlags != lVar4;
                    outputEntries =
                         (TechnologyAsset *)
                         Package_LoadEntryIntoBuffer
                                   (bufferCapacityAndLoadFlags,(byte *)worldRuntime->objectArray,
                                    (word *)u_widget_hex_0050e02a);
                    if (((((!bVar16) &&
                          (outputEntries =
                                (TechnologyAsset *)
                                Package_LoadEntryIntoBuffer
                                          (0x48000,(byte *)g_ArmyRuntimeSlots,
                                           (word *)u_army_hex_0050dfb4), !bVar16)) &&
                         (outputEntries =
                               (TechnologyAsset *)
                               Package_LoadEntryIntoBuffer
                                         (0x400000,(byte *)g_ModelRuntimeSlots,
                                          (word *)u_modul_hex_0050dfee), !bVar16)) &&
                        ((outputEntries =
                               (TechnologyAsset *)
                               Package_LoadEntryIntoBuffer
                                         (0x40000,(byte *)g_EffectRuntimeSlots,
                                          (word *)u_effect_hex_0050dfc6), !bVar16 &&
                         (outputEntries =
                               (TechnologyAsset *)
                               Package_LoadEntryIntoBuffer
                                         (0x40000,(byte *)g_ShotRuntimeSlots,
                                          (word *)u_shot_hex_0050dfdc), !bVar16)))) &&
                       (outputEntries =
                             (TechnologyAsset *)
                             Package_LoadEntryIntoBuffer
                                       (0x4000,(byte *)g_GraphicsShadingRuntimeRecords,
                                        (word *)u_light_hex_0050e016), !bVar16)) {
                      ArmyRuntimePool_RebaseAfterLoad();
                      ModelRuntimePool_RebaseAfterLoad();
                      ShotRuntime_RebaseSlotsAfterLoad();
                      EffectRuntime_RebaseSlotsAfterLoad();
                      InGameConditionRuntime_RebaseLoadedRecords
                                ((InGameConditionRuntime *)worldRuntime);
                      RuntimeHexSegment_ToggleLightImageFlag();
                      MoviePlayback_AdvanceScheduledFrameAndTick();
                      pdVar15 = worldRuntime->dwordArray;
                      for (WVar10 = worldRuntime->dwordArrayCount; WVar10 != 0; WVar10 = WVar10 - 1)
                      {
                        *pdVar15 = 0;
                        pdVar15 = pdVar15 + 1;
                      }
                      pAVar1 = &(levelImage->header).pathOffsets.soundBasePathOffset;
                      uVar17 = CARRY4((uint)levelImage,*pAVar1);
                      pbVar14 = (levelImage->header).common.buildMetadata.
                                assetRelativeAddressAnchor28 + (*pAVar1 - 0x28);
                      WidePath_SetExtensionCode(0x6d6173,(word *)pbVar14);
                      MoviePlayback_AdvanceScheduledFrameAndTick();
                      WidePath_SplitParentAndLeaf
                                ((word *)&g_InGameLevelSoundLeafOrCombinedPathScratchUtf16,
                                 (word *)&g_InGameLevelSoundParentDirectoryScratchUtf16,
                                 (word *)pbVar14);
                      outputEntries = (*g_MemoryApi.allocLargestFreeBlock)();
                      if (!(bool)uVar17) {
                        bVar16 = false;
                        outputCapacityBytes_00 = outputCapacityBytes;
                        pTVar5 = (TechnologyAsset *)
                                 Package_FindEntry(outputCapacityBytes,
                                                   (PckEntryHeader *)outputEntries,(word *)pbVar14,
                                                   g_SoundPackageHandle);
                        iVar11 = extraout_ECX_00;
                        memory = outputRecords;
                        if (bVar16) {
                          pTVar5 = (TechnologyAsset *)
                                   (*g_FileSystemEnumerateDirectoryOrVolumeEntriesCf)
                                             (FILESYSTEM_ENUMERATE_FILES,0xffffffff,
                                              outputCapacityBytes_00,outputRecords,pbVar14);
                          iVar11 = extraout_ECX_01;
                          memory = extraout_EDX_01;
                        }
                        pTVar6 = pTVar5;
                        if (&stack0xffffffd8 < (undefined1 *)0xfffffffc) {
                          g_MoviePlaybackBaseFrameGroup = 6;
                          g_MoviePlaybackScheduleCounter = 0;
                          newSize = (dword)((longlong)(int)pTVar5 * (longlong)iVar11);
                          bVar18 = (longlong)(int)newSize !=
                                   (longlong)(int)pTVar5 * (longlong)iVar11;
                          g_MoviePlaybackScheduleSpan = iVar11;
                          uVar19 = (*g_MemoryApi.shrinkInPlace)(newSize,memory);
                          pwVar12 = (word *)((ulonglong)uVar19 >> 0x20);
                          pTVar6 = (TechnologyAsset *)uVar19;
                          if (!bVar18) {
                            uVar9 = extraout_ECX_02;
                            pIVar3 = g_InGameConditionRuntime;
                            if (worldRuntime->dwordArrayCount < extraout_ECX_02) {
                              uVar9 = worldRuntime->dwordArrayCount;
                            }
                            do {
                              g_InGameConditionRuntime = pIVar3;
                              if (uVar9 == 0) {
                                arg0 = (SoundSampleAsset *)(*g_MemoryApi.free)(outputEntries);
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
                                uVar17 = 0;
                                if (*(int *)(pIVar3[3].reservedB0_D7 + 0x1c) != 0) {
                                  (*g_WideNumberFormatUtf16)
                                            (WIDE_FORMAT_PAD_WITH_ZERO,0,2,1,
                                             *(sdword *)(pIVar3[3].reservedB0_D7 + 0x1c),
                                             (word *)(u_sound_level00_sam_0050df6c + 0xb));
                                  uVar19 = Resource_Load(extraout_ECX_05,extraout_EDX_03,
                                                         (word *)u_sound_level00_sam_0050df6c);
                                  arg0 = (SoundSampleAsset *)uVar19;
                                  if (!(bool)uVar17) {
                                    pDVar8 = (*g_SoundCreateSampleVoiceSet)(arg0);
                                    if (!(bool)uVar17) {
                                      g_InGameLevelEffectVoiceSet0 = pDVar8;
                                    }
                                    arg0 = (SoundSampleAsset *)Resource_Release(arg0);
                                  }
                                }
                                uVar17 = 0;
                                if (*(int *)(pIVar3[3].reservedB0_D7 + 0x20) != 0) {
                                  (*g_WideNumberFormatUtf16)
                                            (WIDE_FORMAT_PAD_WITH_ZERO,0,2,1,
                                             *(sdword *)(pIVar3[3].reservedB0_D7 + 0x20),
                                             (word *)(u_sound_level00_sam_0050df6c + 0xb));
                                  uVar19 = Resource_Load(extraout_ECX_06,extraout_EDX_04,
                                                         (word *)u_sound_level00_sam_0050df6c);
                                  arg0 = (SoundSampleAsset *)uVar19;
                                  if (!(bool)uVar17) {
                                    pDVar8 = (*g_SoundCreateSampleVoiceSet)(arg0);
                                    if (!(bool)uVar17) {
                                      g_InGameLevelEffectVoiceSet1 = pDVar8;
                                    }
                                    arg0 = (SoundSampleAsset *)Resource_Release(arg0);
                                  }
                                }
                                uVar17 = 0;
                                if (*(int *)(pIVar3[3].reservedB0_D7 + 0x24) != 0) {
                                  (*g_WideNumberFormatUtf16)
                                            (WIDE_FORMAT_PAD_WITH_ZERO,0,2,1,
                                             *(sdword *)(pIVar3[3].reservedB0_D7 + 0x24),
                                             (word *)(u_sound_level00_sam_0050df6c + 0xb));
                                  uVar19 = Resource_Load(extraout_ECX_07,extraout_EDX_05,
                                                         (word *)u_sound_level00_sam_0050df6c);
                                  arg0 = (SoundSampleAsset *)uVar19;
                                  if (!(bool)uVar17) {
                                    pDVar8 = (*g_SoundCreateSampleVoiceSet)(arg0);
                                    if (!(bool)uVar17) {
                                      g_InGameLevelEffectVoiceSet2 = pDVar8;
                                    }
                                    arg0 = (SoundSampleAsset *)Resource_Release(arg0);
                                  }
                                }
                                uVar17 = 0;
                                if (pIVar3[3].tailRecordD8 != (InGameConditionRecord *)0x0) {
                                  (*g_WideNumberFormatUtf16)
                                            (WIDE_FORMAT_PAD_WITH_ZERO,0,2,1,
                                             (sdword)pIVar3[3].tailRecordD8,
                                             (word *)(u_sound_level00_sam_0050df6c + 0xb));
                                  uVar19 = Resource_Load(extraout_ECX_08,extraout_EDX_06,
                                                         (word *)u_sound_level00_sam_0050df6c);
                                  arg0 = (SoundSampleAsset *)uVar19;
                                  if (!(bool)uVar17) {
                                    pDVar8 = (*g_SoundCreateSampleVoiceSet)(arg0);
                                    if (!(bool)uVar17) {
                                      g_InGameLevelEffectVoiceSet3 = pDVar8;
                                    }
                                    arg0 = (SoundSampleAsset *)Resource_Release(arg0);
                                  }
                                }
                                uVar17 = 0;
                                if (*(int *)(pIVar3[3].reservedB0_D7 + 0xc) != 0) {
                                  (*g_WideNumberFormatUtf16)
                                            (WIDE_FORMAT_PAD_WITH_ZERO,0,2,1,
                                             *(sdword *)(pIVar3[3].reservedB0_D7 + 0xc),
                                             (word *)(u_sound_music00_sam_0050df90 + 0xb));
                                  uVar19 = Resource_Load(extraout_ECX_09,extraout_EDX_07,
                                                         (word *)u_sound_music00_sam_0050df90);
                                  arg0 = (SoundSampleAsset *)uVar19;
                                  if (!(bool)uVar17) {
                                    pDVar8 = (*g_SoundCreateSampleVoiceSet)(arg0);
                                    if (!(bool)uVar17) {
                                      g_InGameLevelMusicVoiceSet0 = pDVar8;
                                    }
                                    arg0 = (SoundSampleAsset *)Resource_Release(arg0);
                                  }
                                }
                                uVar17 = 0;
                                if (*(int *)(pIVar3[3].reservedB0_D7 + 0x10) != 0) {
                                  (*g_WideNumberFormatUtf16)
                                            (WIDE_FORMAT_PAD_WITH_ZERO,0,2,1,
                                             *(sdword *)(pIVar3[3].reservedB0_D7 + 0x10),
                                             (word *)(u_sound_music00_sam_0050df90 + 0xb));
                                  uVar19 = Resource_Load(extraout_ECX_10,extraout_EDX_08,
                                                         (word *)u_sound_music00_sam_0050df90);
                                  arg0 = (SoundSampleAsset *)uVar19;
                                  if (!(bool)uVar17) {
                                    pDVar8 = (*g_SoundCreateSampleVoiceSet)(arg0);
                                    if (!(bool)uVar17) {
                                      g_InGameLevelMusicVoiceSet1 = pDVar8;
                                    }
                                    arg0 = (SoundSampleAsset *)Resource_Release(arg0);
                                  }
                                }
                                uVar17 = 0;
                                if (*(int *)(pIVar3[3].reservedB0_D7 + 0x14) != 0) {
                                  (*g_WideNumberFormatUtf16)
                                            (WIDE_FORMAT_PAD_WITH_ZERO,0,2,1,
                                             *(sdword *)(pIVar3[3].reservedB0_D7 + 0x14),
                                             (word *)(u_sound_music00_sam_0050df90 + 0xb));
                                  uVar19 = Resource_Load(extraout_ECX_11,extraout_EDX_09,
                                                         (word *)u_sound_music00_sam_0050df90);
                                  arg0 = (SoundSampleAsset *)uVar19;
                                  if (!(bool)uVar17) {
                                    pDVar8 = (*g_SoundCreateSampleVoiceSet)(arg0);
                                    if (!(bool)uVar17) {
                                      g_InGameLevelMusicVoiceSet2 = pDVar8;
                                    }
                                    arg0 = (SoundSampleAsset *)Resource_Release(arg0);
                                  }
                                }
                                uVar17 = 0;
                                if (*(int *)(pIVar3[3].reservedB0_D7 + 0x18) != 0) {
                                  (*g_WideNumberFormatUtf16)
                                            (WIDE_FORMAT_PAD_WITH_ZERO,0,2,1,
                                             *(sdword *)(pIVar3[3].reservedB0_D7 + 0x18),
                                             (word *)(u_sound_music00_sam_0050df90 + 0xb));
                                  uVar19 = Resource_Load(extraout_ECX_12,extraout_EDX_10,
                                                         (word *)u_sound_music00_sam_0050df90);
                                  arg0 = (SoundSampleAsset *)uVar19;
                                  if (!(bool)uVar17) {
                                    pDVar8 = (*g_SoundCreateSampleVoiceSet)(arg0);
                                    if (!(bool)uVar17) {
                                      g_InGameLevelMusicVoiceSet3 = pDVar8;
                                    }
                                    arg0 = (SoundSampleAsset *)Resource_Release(arg0);
                                  }
                                }
                                return CONCAT44(param_2,arg0);
                              }
                              pdVar15 = worldRuntime->dwordArray;
                              worldContext1 = worldRuntime;
                              uVar19 = WidePath_ParseTrailingNumberBeforeExtensionRegs(pwVar12);
                              pwVar12 = (word *)((ulonglong)uVar19 >> 0x20);
                              if (extraout_ECX_03 < worldRuntime->dwordArrayCount) {
                                uVar17 = false;
                                if (!bVar16) {
                                  uVar19 = Resource_Load(extraout_ECX_03,pwVar12,pwVar12);
                                  pTVar6 = (TechnologyAsset *)uVar19;
                                  if ((bool)uVar17) break;
                                }
                                else {
                                  WidePath_CombineDirectoryAndLeaf
                                            ((word *)&
                                                  g_InGameLevelSoundLeafOrCombinedPathScratchUtf16,
                                             pwVar12,(word *)&
                                                  g_InGameLevelSoundParentDirectoryScratchUtf16);
                                  uVar19 = Resource_Load(extraout_ECX_04,extraout_EDX_02,
                                                         (word *)&
                                                  g_InGameLevelSoundLeafOrCombinedPathScratchUtf16);
                                  pTVar6 = (TechnologyAsset *)uVar19;
                                  if ((bool)uVar17) break;
                                }
                                bVar18 = false;
                                pSVar7 = SpatialSoundSlot_CreateFromSampleAsset
                                                   ((SoundSampleAsset *)pTVar6);
                                if (!bVar18) {
                                  pdVar15[extraout_ECX_03] = (dword)pSVar7;
                                }
                                Resource_Release(pTVar6);
                                uVar19 = MoviePlayback_AdvanceScheduledFrameAndTick();
                                pwVar12 = (word *)((ulonglong)uVar19 >> 0x20);
                              }
                              pwVar12 = (word *)((int)pwVar12 + (int)pTVar5);
                              uVar9 = uVar9 - 1;
                              worldRuntime = worldContext1;
                              pIVar3 = g_InGameConditionRuntime;
                            } while( true );
                          }
                        }
                        (*g_MemoryApi.free)(outputEntries);
                        outputEntries = pTVar6;
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
  return CONCAT44(param_2,outputEntries);
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
void __thiscall
InGameLevelRuntime_ShutdownLoadedAssetResources(void *this,LevelAssetHeader *levelAsset)

{
  int extraout_ECX;
  int extraout_ECX_00;
  undefined4 *puVar1;
  void **resourcePointerCursor;
  
  EffectRuntime_ShutdownGraphicsResources();
  ShotRuntime_ShutdownGraphicsResources();
  ModelRuntimePool_ShutdownAndReleaseDefinitions();
  ArmyRuntime_ShutdownPoolAndGraphics();
  TerrainVisualResources_Shutdown();
  puVar1 = (undefined4 *)(levelAsset->pathOffsets).armyTextureBasePathOffset;
  if (((levelAsset->pathOffsets).shotTextureBasePathOffset != 0) && (puVar1 != (undefined4 *)0x0)) {
    do {
      SpatialSoundSlot_ReleaseSample((SpatialSoundSlot *)*puVar1);
      puVar1 = puVar1 + 1;
    } while (extraout_ECX != 1);
  }
  (*g_SoundReleaseSampleVoiceSet)(g_InGameLevelEffectVoiceSet0);
  (*g_SoundReleaseSampleVoiceSet)(g_InGameLevelEffectVoiceSet1);
  (*g_SoundReleaseSampleVoiceSet)(g_InGameLevelEffectVoiceSet2);
  (*g_SoundReleaseSampleVoiceSet)(g_InGameLevelEffectVoiceSet3);
  (*g_SoundReleaseSampleVoiceSet)(g_InGameLevelMusicVoiceSet0);
  (*g_SoundReleaseSampleVoiceSet)(g_InGameLevelMusicVoiceSet1);
  (*g_SoundReleaseSampleVoiceSet)(g_InGameLevelMusicVoiceSet2);
  (*g_SoundReleaseSampleVoiceSet)(g_InGameLevelMusicVoiceSet3);
  if ((g_InGameLoadedResourcePointers != (void **)0x0) &&
     (resourcePointerCursor = g_InGameLoadedResourcePointers,
     g_InGameLoadedResourcePointerCount != 0)) {
    do {
      Resource_Release(*resourcePointerCursor);
      resourcePointerCursor = resourcePointerCursor + 1;
    } while (extraout_ECX_00 != 1);
  }
  (*g_MemoryApi.free)(g_InGameLoadedResourcePointers);
  g_InGameLoadedResourcePointers = (void **)0x0;
  g_InGameLoadedResourcePointerCount = 0;
  (*g_MemoryApi.free)(g_InGameConditionRuntime);
  g_InGameConditionRuntime = (InGameConditionRuntime *)0x0;
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
void InGameLevelRuntime_SaveLevelAssetImageFromWorldStateCf(int param_1)

{
  int iVar1;
  undefined4 uVar2;
  int iVar3;
  byte *source;
  dword dVar4;
  dword dVar5;
  dword dVar6;
  dword dVar7;
  byte *pbVar8;
  undefined1 in_CF;
  bool bVar9;
  
  Package_LoadEntryIntoBuffer(0x800000,g_PackageScratchBuffer,g_LevelEndingMovieSourcePath);
  source = g_PackageScratchBuffer;
  if (!(bool)in_CF) {
    iVar1 = *(int *)(g_PackageScratchBuffer + 0xdc);
    uVar2 = *(undefined4 *)(param_1 + 0x50);
    *(int *)(g_PackageScratchBuffer + 4) = iVar1;
    source[0xd8] = 0;
    source[0xd9] = 0;
    source[0xda] = 0;
    source[0xdb] = 0;
    *(undefined4 *)(source + 0x2dc) = uVar2;
    pbVar8 = source + iVar1;
    for (iVar1 = *(int *)(param_1 + 0xd8); iVar1 != 0; iVar1 = *(int *)(iVar1 + 4)) {
      if (*(int *)(iVar1 + 0xa4) == 0) {
        *(int *)(source + 0xd8) = *(int *)(source + 0xd8) + 1;
        *(int *)(source + 4) = *(int *)(source + 4) + 0x20;
        iVar3 = *(int *)(*(int *)(iVar1 + 0x48) + 8);
        *(undefined4 *)(pbVar8 + 8) = *(undefined4 *)(iVar1 + 0x94);
        uVar2 = *(undefined4 *)(iVar3 + 0xa0);
        *(undefined4 *)(pbVar8 + 4) = *(undefined4 *)(iVar3 + 0xc);
        *(undefined4 *)pbVar8 = uVar2;
        uVar2 = *(undefined4 *)(iVar1 + 0x14);
        *(undefined4 *)(pbVar8 + 0xc) = *(undefined4 *)(iVar1 + 0x98);
        *(undefined4 *)(pbVar8 + 0x10) = uVar2;
        pbVar8[0x14] = 0;
        pbVar8[0x15] = 0;
        pbVar8[0x16] = 0;
        pbVar8[0x17] = 0;
        pbVar8[0x18] = 0;
        pbVar8[0x19] = 0;
        pbVar8[0x1a] = 0;
        pbVar8[0x1b] = 0;
        pbVar8[0x1c] = 0;
        pbVar8[0x1d] = 0;
        pbVar8[0x1e] = 0;
        pbVar8[0x1f] = 0;
        pbVar8 = pbVar8 + 0x20;
      }
    }
    *(uint *)(source + 0x2e0) =
         *(uint *)(param_1 + 0x178) & 0xffff | *(int *)(param_1 + 0x17c) << 0x10;
    bVar9 = false;
    *(uint *)(source + 0x2f0) =
         *(uint *)(param_1 + 0xb8) & 0xffff | *(int *)(param_1 + 0xbc) << 0x10;
    dVar7 = g_LevelCameraBookmark1PackedHeadingLow16PitchHigh16;
    dVar6 = g_LevelCameraBookmark1PositionMagnitudeQ12;
    dVar5 = g_LevelCameraBookmark1PositionZQ12;
    dVar4 = g_LevelCameraBookmark1PositionYQ12;
    *(dword *)(source + 0x200) = g_LevelCameraBookmark1PositionXQ12;
    *(dword *)(source + 0x204) = dVar4;
    *(dword *)(source + 0x208) = dVar5;
    *(dword *)(source + 0x20c) = dVar6;
    *(dword *)(source + 0x210) = dVar7;
    dVar7 = g_LevelCameraBookmark2PackedHeadingLow16PitchHigh16;
    dVar6 = g_LevelCameraBookmark2PositionMagnitudeQ12;
    dVar5 = g_LevelCameraBookmark2PositionZQ12;
    dVar4 = g_LevelCameraBookmark2PositionYQ12;
    *(dword *)(source + 0x220) = g_LevelCameraBookmark2PositionXQ12;
    *(dword *)(source + 0x224) = dVar4;
    *(dword *)(source + 0x228) = dVar5;
    *(dword *)(source + 0x22c) = dVar6;
    *(dword *)(source + 0x230) = dVar7;
    dVar7 = g_LevelCameraBookmark3PackedHeadingLow16PitchHigh16;
    dVar6 = g_LevelCameraBookmark3PositionMagnitudeQ12;
    dVar5 = g_LevelCameraBookmark3PositionZQ12;
    dVar4 = g_LevelCameraBookmark3PositionYQ12;
    *(dword *)(source + 0x240) = g_LevelCameraBookmark3PositionXQ12;
    *(dword *)(source + 0x244) = dVar4;
    *(dword *)(source + 0x248) = dVar5;
    *(dword *)(source + 0x24c) = dVar6;
    *(dword *)(source + 0x250) = dVar7;
    dVar7 = g_LevelCameraBookmark4PackedHeadingLow16PitchHigh16;
    dVar6 = g_LevelCameraBookmark4PositionMagnitudeQ12;
    dVar5 = g_LevelCameraBookmark4PositionZQ12;
    dVar4 = g_LevelCameraBookmark4PositionYQ12;
    *(dword *)(source + 0x260) = g_LevelCameraBookmark4PositionXQ12;
    *(dword *)(source + 0x264) = dVar4;
    *(dword *)(source + 0x268) = dVar5;
    *(dword *)(source + 0x26c) = dVar6;
    *(dword *)(source + 0x270) = dVar7;
    dVar7 = g_LevelCameraBookmark5PackedHeadingLow16PitchHigh16;
    dVar6 = g_LevelCameraBookmark5PositionMagnitudeQ12;
    dVar5 = g_LevelCameraBookmark5PositionZQ12;
    dVar4 = g_LevelCameraBookmark5PositionYQ12;
    *(dword *)(source + 0x280) = g_LevelCameraBookmark5PositionXQ12;
    *(dword *)(source + 0x284) = dVar4;
    *(dword *)(source + 0x288) = dVar5;
    *(dword *)(source + 0x28c) = dVar6;
    *(dword *)(source + 0x290) = dVar7;
    dVar7 = g_LevelCameraBookmark6PackedHeadingLow16PitchHigh16;
    dVar6 = g_LevelCameraBookmark6PositionMagnitudeQ12;
    dVar5 = g_LevelCameraBookmark6PositionZQ12;
    dVar4 = g_LevelCameraBookmark6PositionYQ12;
    *(dword *)(source + 0x2a0) = g_LevelCameraBookmark6PositionXQ12;
    *(dword *)(source + 0x2a4) = dVar4;
    *(dword *)(source + 0x2a8) = dVar5;
    *(dword *)(source + 0x2ac) = dVar6;
    *(dword *)(source + 0x2b0) = dVar7;
    dVar7 = g_LevelCameraBookmark7PackedHeadingLow16PitchHigh16;
    dVar6 = g_LevelCameraBookmark7PositionMagnitudeQ12;
    dVar5 = g_LevelCameraBookmark7PositionZQ12;
    dVar4 = g_LevelCameraBookmark7PositionYQ12;
    *(dword *)(source + 0x2c0) = g_LevelCameraBookmark7PositionXQ12;
    *(dword *)(source + 0x2c4) = dVar4;
    *(dword *)(source + 0x2c8) = dVar5;
    *(dword *)(source + 0x2cc) = dVar6;
    *(dword *)(source + 0x2d0) = dVar7;
    FileSystem_WriteBufferToPathCf
              (*(FileIoByteCount *)(source + 4),source,g_LevelEndingMovieSourcePath);
    if (!bVar9) {
      return;
    }
  }
  return;
}

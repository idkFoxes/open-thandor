#include <thandor/ui/frontend/runtime.h>

/* Implementation ownership: ui/frontend/runtime. */

/* Address: 0x00546BD0.
   Ownership: ui/frontend/runtime.
   Purpose: Handles frontend main loop.
   Local calls: Frontend_Init, FrontendRuntime_ShutdownAndReleaseResourcesRegs.
   Cross-module calls: FrontendRomActionTable_ExecuteRecord [assets/rom/runtime], FrontendRomTransition_RequestStop
   [assets/rom/runtime], FrontendRomTransition_ProcessPendingRecord [assets/rom/runtime], UiRootStack_InvalidateAll
   [ui/controls/layout], UiFrame_ProcessAndPresent [ui/controls/layout], UiFrame_FlushInputAndResetPendingTicks
   [ui/controls/layout].
*/
undefined8 __fastcall Frontend_MainLoop(undefined4 param_1,undefined4 param_2,RomRecordId param_3)

{
  FrontendRoleStateFlags *pFVar1;
  int iVar2;
  AssetAllocationSizeBytes AVar3;
  FrontendSnapshotTransferFlags FVar4;
  SessionNetworkRoleFlags SVar5;
  ScenarioCatalogHeader *source;
  dword sourceSizeBytes;
  word *pwVar6;
  FieldGridAsset *sourceGrid;
  RomRecordId RVar7;
  PckDecodedByteCount *pPVar8;
  undefined4 extraout_EAX;
  SessionNetworkRoleFlags extraout_ECX;
  SessionNetworkRoleFlags extraout_ECX_00;
  SessionNetworkRoleFlags extraout_ECX_01;
  SessionNetworkRoleFlags extraout_ECX_02;
  SessionNetworkRoleFlags extraout_ECX_03;
  undefined4 extraout_ECX_04;
  undefined4 uVar9;
  SessionNetworkRoleFlags extraout_ECX_05;
  FrontendPlayerRuntimeBlockCount FVar10;
  int iVar11;
  int iVar12;
  int extraout_ECX_06;
  int extraout_ECX_07;
  undefined4 extraout_ECX_08;
  int extraout_ECX_09;
  uint extraout_ECX_10;
  uint uVar13;
  undefined4 extraout_ECX_11;
  SessionNetworkRoleFlags extraout_ECX_12;
  SessionNetworkRoleFlags extraout_ECX_13;
  undefined4 extraout_ECX_14;
  SessionNetworkRoleFlags extraout_ECX_15;
  SessionNetworkRoleFlags extraout_ECX_16;
  SessionNetworkRoleFlags extraout_ECX_17;
  SessionNetworkRoleFlags extraout_ECX_18;
  SessionNetworkRoleFlags extraout_ECX_19;
  SessionNetworkRoleFlags extraout_ECX_20;
  SessionNetworkRoleFlags extraout_ECX_21;
  SessionNetworkRoleFlags extraout_ECX_22;
  SessionNetworkRoleFlags extraout_ECX_23;
  SessionNetworkRoleFlags extraout_ECX_24;
  SessionNetworkRoleFlags SVar14;
  SessionNetworkRoleFlags extraout_ECX_25;
  SessionNetworkRoleFlags extraout_ECX_26;
  SessionNetworkRoleFlags extraout_ECX_27;
  int extraout_EDX;
  int extraout_EDX_00;
  int extraout_EDX_01;
  undefined4 extraout_EDX_02;
  int extraout_EDX_03;
  undefined4 extraout_EDX_04;
  PckOutputCapacityBytes destinationCapacityBytes;
  undefined4 unaff_EBX;
  FrontendPlayerRuntimeRecord *pFVar15;
  void *pvVar16;
  void *pvVar17;
  byte *pbVar18;
  LevelAssetRuntimeImagePrefix370 *source_00;
  FrontendPlayerRuntimeRecord *pFVar19;
  FrontendSnapshotTransferFlags *pFVar20;
  FrontendPlayerRuntimeRecord *unaff_EDI;
  undefined4 *puVar21;
  undefined1 in_CF;
  undefined1 uVar22;
  bool bVar23;
  undefined8 uVar24;
  FieldGridAsset *pFVar25;
  undefined4 uVar26;
  
  g_FrontendNetworkState = 0;
  Frontend_Init(param_1,param_2,param_3);
  if (!(bool)in_CF) {
    (*g_CommandLineFindOption)(5,s_SPIELER__SPIEL__NETZWERK__HOST_00545e72 + 0x1a);
    if ((bool)in_CF) {
      (*g_CommandLineFindOption)(8,s_NAME__CLIENT__KARTE___00545e91 + 6);
      if ((bool)in_CF) {
        (*g_CommandLineFindOption)(7,s_NAME__CLIENT__KARTE___00545e91 + 0xe);
        SVar14 = extraout_ECX_01;
        if (!(bool)in_CF) {
          FrontendRomActionTable_ExecuteRecord(0,0,1,0);
          FrontendRomTransition_RequestStop();
          SVar14 = extraout_ECX_02;
        }
      }
      else {
        FrontendRomActionTable_ExecuteRecord(0,0,1,3);
        FrontendRomTransition_RequestStop();
        SVar14 = extraout_ECX_00;
      }
    }
    else {
      FrontendRomActionTable_ExecuteRecord(0,0,1,3);
      FrontendRomTransition_RequestStop();
      SVar14 = extraout_ECX;
    }
FrontendMainLoop_ProcessFrameAndPendingPageAction:
    do {
      while( true ) {
        while( true ) {
          while( true ) {
            if (g_UiRootNode != (UiRootNode *)0xffffffff) {
              FrontendRomTransition_ProcessPendingRecord(SVar14);
            }
            UiRootStack_InvalidateAll();
            UiFrame_ProcessAndPresent();
            g_FrontendPendingPageActionDepth = 0;
            if (g_FrontendPendingPageAction != SESSION_NETWORK_ROLE_LOCAL) break;
            SVar14 = extraout_ECX_03;
            if (g_UiRootNode == (UiRootNode *)0xffffffff) {
              uVar24 = FrontendRuntime_ShutdownAndReleaseResourcesRegs();
              return CONCAT44(param_2,(int)uVar24);
            }
          }
          UiFrame_FlushInputAndResetPendingTicks();
          g_FrontendPendingPageActionDepth = g_FrontendPendingPageActionDepth + 1;
          if (g_FrontendPendingPageAction != SESSION_NETWORK_ROLE_HOST) break;
          FrontendNetworkSetupPage_InitializeBackendMode(g_FrontendRootNode);
          g_FrontendPendingPageAction = SESSION_NETWORK_ROLE_LOCAL;
          SVar14 = extraout_ECX_15;
        }
        if (g_FrontendPendingPageAction != SESSION_NETWORK_ROLE_NETWORKED_MASK) break;
        FrontendGameplaySettingsPage_InitializeFromPersistentSettings
                  ((UiRootNode *)&g_FrontendRootNode->commonState);
        g_FrontendPendingPageAction = SESSION_NETWORK_ROLE_LOCAL;
        SVar14 = extraout_ECX_16;
      }
      if (g_FrontendPendingPageAction == 5) {
        pFVar15 = g_FrontendPlayerRuntimeBlocks;
        SVar14 = g_FrontendPlayerRuntimeBlockCount;
        SVar5 = g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_NETWORKED_MASK;
        while (FVar10 = g_FrontendPlayerRuntimeBlockCount, pFVar19 = g_FrontendPlayerRuntimeBlocks,
              SVar5 != SESSION_NETWORK_ROLE_LOCAL) {
          if ((pFVar15->snapshotTransferFlags & FRONTEND_SNAPSHOT_HOST_PUBLICATION_READY) == 0) {
            bVar23 = false;
            if (((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_CLIENT) !=
                 SESSION_NETWORK_ROLE_LOCAL) &&
               (pPVar8 = UiTransferMailbox_GetReceivedBufferCf(), SVar14 = extraout_ECX_18, !bVar23)
               ) {
              PckCodec_DecodeHuffmanRle
                        (*pPVar8,g_PackageScratchBuffer,
                         extraout_ECX_18 + ~SESSION_NETWORK_ROLE_NETWORKED_MASK,(byte *)(pPVar8 + 1)
                        );
              FVar10 = g_FrontendPlayerRuntimeBlockCount;
              pFVar20 = (FrontendSnapshotTransferFlags *)g_PackageScratchBuffer;
              unaff_EDI = g_FrontendPlayerRuntimeBlocks;
              do {
                FVar4 = *pFVar20;
                unaff_EDI->snapshotTransferFlags = unaff_EDI->snapshotTransferFlags | FVar4;
                pFVar20 = pFVar20 + 1;
                if ((FVar4 & FRONTEND_SNAPSHOT_PAYLOAD_COMPLETE) != 0) {
                  pbVar18 = unaff_EDI->snapshotPayloadB0_13AF;
                  for (iVar12 = 0x4c0; iVar12 != 0; iVar12 = iVar12 + -1) {
                    *(FrontendSnapshotTransferFlags *)pbVar18 = *pFVar20;
                    pFVar20 = pFVar20 + 1;
                    pbVar18 = pbVar18 + 4;
                  }
                }
                unaff_EDI = unaff_EDI + 1;
                FVar10 = FVar10 - 1;
              } while (FVar10 != 0);
              FrontendCommandQueue_EnqueueLocalPlayerCommand(0x1710,0,0,0);
              UiTransferMailbox_ClearReceivedState();
              SVar14 = extraout_ECX_19;
            }
            goto FrontendMainLoop_ProcessFrameAndPendingPageAction;
          }
          pFVar15 = pFVar15 + 1;
          SVar14 = SVar14 - SESSION_NETWORK_ROLE_CLIENT;
          SVar5 = SVar14;
        }
        do {
          pFVar1 = &(pFVar19->factionAssignment).roleStateFlags;
          *pFVar1 = *pFVar1 & 1;
          pFVar15 = g_FrontendPlayerRuntimeBlocks;
          if (*pFVar1 == 0) {
            FrontendScenarioTransfer_ProcessReceivedAsset();
            SVar14 = extraout_ECX_20;
            if (((pFVar15->factionAssignment).roleStateFlags & 1) == 0) {
              pFVar1 = &(pFVar15->factionAssignment).roleStateFlags;
              *pFVar1 = *pFVar1 | 1;
              ScenarioCatalog_Rebuild();
              sourceSizeBytes = g_ScenarioCatalogUsedBytes;
              source = g_ScenarioCatalog;
              if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_CLIENT) ==
                  SESSION_NETWORK_ROLE_LOCAL) {
                SVar14 = extraout_ECX_21;
                if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_HOST) !=
                    SESSION_NETWORK_ROLE_LOCAL) {
                  pbVar18 = (byte *)((int)&g_ScenarioCatalog->campaignRecordsOffset +
                                    g_ScenarioCatalogUsedBytes);
                  destinationCapacityBytes = 0x2fffc - g_ScenarioCatalogUsedBytes;
                  *(dword *)(pbVar18 + -4) = g_ScenarioCatalogUsedBytes;
                  uVar9 = extraout_EAX;
                  uVar26 = extraout_EDX_04;
                  PckCodec_EncodeHuffmanRle
                            (destinationCapacityBytes,pbVar18,sourceSizeBytes,(byte *)source);
                  iVar12 = (*g_FatalErrorPrimaryDispatchCf)(uVar9,uVar26);
                  UiTransferMailbox_SetOutgoingBuffer(iVar12 + 4,pbVar18 + -4);
                  SVar14 = extraout_ECX_22;
                }
              }
              else {
                UiTransferMailbox_MarkUnavailable();
                g_FrontendScenarioTransferState = 1;
                SVar14 = extraout_ECX_23;
              }
            }
            goto FrontendMainLoop_ProcessFrameAndPendingPageAction;
          }
          pFVar19 = pFVar19 + 1;
          FVar10 = FVar10 - 1;
        } while (FVar10 != 0);
        if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_HOST) != SESSION_NETWORK_ROLE_LOCAL) {
          UiTransferMailbox_SetOutgoingBuffer(0,(void *)0x0);
        }
        FrontendScenarioSelectionPage_InitializeAndApplyMapOption(g_FrontendRootNode->raw);
        g_FrontendPendingPageAction = SESSION_NETWORK_ROLE_LOCAL;
        SVar14 = extraout_ECX_17;
        goto FrontendMainLoop_ProcessFrameAndPendingPageAction;
      }
      if (g_FrontendPendingPageAction == 7) {
        FrontendScenarioTransfer_ProcessReceivedAsset();
        SVar14 = g_FrontendPlayerRuntimeBlockCount;
        pFVar15 = g_FrontendPlayerRuntimeBlocks;
        do {
          if (((pFVar15->factionAssignment).roleStateFlags & 2) == 0)
          goto FrontendMainLoop_ProcessFrameAndPendingPageAction;
          pFVar15 = pFVar15 + 1;
          SVar14 = SVar14 - SESSION_NETWORK_ROLE_CLIENT;
        } while (SVar14 != SESSION_NETWORK_ROLE_LOCAL);
        if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_HOST) != SESSION_NETWORK_ROLE_LOCAL) {
          (*g_MemoryApi.free)(g_UiTransferMailbox.outgoingAllocation);
          UiTransferMailbox_SetOutgoingBuffer(0,(void *)0x0);
        }
        FrontendTaskAssignmentPage_Initialize((UiRootNode *)&g_FrontendRootNode->commonState);
        g_FrontendPendingPageAction = SESSION_NETWORK_ROLE_LOCAL;
        SVar14 = extraout_ECX_24;
      }
      else if (g_FrontendPendingPageAction == 8) {
        FrontendScenarioTransfer_ProcessReceivedAsset();
        SVar14 = g_FrontendPlayerRuntimeBlockCount;
        pFVar15 = g_FrontendPlayerRuntimeBlocks;
        do {
          if (((pFVar15->factionAssignment).roleStateFlags & 0xc) == 0)
          goto FrontendMainLoop_ProcessFrameAndPendingPageAction;
          pFVar15 = pFVar15 + 1;
          SVar14 = SVar14 - SESSION_NETWORK_ROLE_CLIENT;
        } while (SVar14 != SESSION_NETWORK_ROLE_LOCAL);
        if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_HOST) != SESSION_NETWORK_ROLE_LOCAL) {
          (*g_MemoryApi.free)(g_UiTransferMailbox.outgoingAllocation);
          UiTransferMailbox_SetOutgoingBuffer(0,(void *)0x0);
        }
        FrontendMissionBriefingPage_Initialize((UiRootNode *)&g_FrontendRootNode->commonState);
        g_FrontendPendingPageAction = SESSION_NETWORK_ROLE_LOCAL;
        SVar14 = extraout_ECX_25;
      }
      else if (g_FrontendPendingPageAction == 9) {
        CreditsScreen_Open(g_FrontendRootNode);
        g_FrontendPendingPageAction = SESSION_NETWORK_ROLE_LOCAL;
        SVar14 = extraout_ECX_26;
      }
      else {
        if (g_FrontendPendingPageAction != 4) {
          uVar24 = FrontendRuntime_ShutdownAndReleaseResourcesRegs();
          iVar12 = (int)((ulonglong)uVar24 >> 0x20);
          if (g_FrontendPendingPageAction == SESSION_NETWORK_ROLE_CLIENT) {
            PersistentSettings_Flush();
            InGameRuntime_RunSessionUntilExit
                      (extraout_ECX_14,extraout_EDX_02,g_FrontendLoadedLevelAsset,0,
                       (word *)&g_FrontendScenarioPathScratchUtf16);
            (*g_FatalErrorPrimaryDispatchCf)();
            PersistentSettings_Flush();
            UiFrame_FlushInputAndResetPendingTicks();
            g_FrontendScenarioInitializationCount = 0;
            FVar10 = g_FrontendPlayerRuntimeBlockCount;
            pFVar15 = g_FrontendPlayerRuntimeBlocks;
            do {
              (pFVar15->factionAssignment).roleStateFlags = 0;
              pFVar15 = pFVar15 + 1;
              FVar10 = FVar10 - 1;
              iVar12 = extraout_EDX_03;
              pvVar17 = g_FrontendLoadedCampaignAsset;
            } while (FVar10 != 0);
joined_r0x0054707a:
            iVar11 = 0;
            g_FrontendLoadedCampaignAsset = pvVar17;
            if (pvVar17 != (void *)0x0) {
              iVar11 = *(int *)((int)pvVar17 + 0xb8);
              pvVar16 = pvVar17;
              do {
                if (*(int *)((int)pvVar17 + 0xc4) == *(int *)((int)pvVar16 + 0x300)) {
                  iVar2 = *(int *)((int)pvVar16 + g_EndMovieSelectionIndex * 4 + 0x200);
                  iVar12 = g_EndMovieSelectionIndex;
                  if (-1 < iVar2) {
                    iVar12 = *(int *)((int)pvVar17 + 0xb8);
                    *(int *)((int)pvVar17 + 0xc4) = iVar2;
                    goto FrontendCampaign_FindSelectedLevelRecord;
                  }
                  break;
                }
                pvVar16 = (void *)((int)pvVar16 + 0x180);
                iVar11 = iVar11 + -1;
              } while (iVar11 != 0);
            }
            goto FrontendScenario_UseResolvedPathOrFallbackPage;
          }
          if (g_FrontendPendingPageAction == 6) {
            InGameRuntime_RunSessionUntilExit
                      (extraout_ECX_04,iVar12,g_FrontendLoadedLevelAsset,1,
                       (word *)&g_FrontendScenarioPathScratchUtf16);
            (*g_FatalErrorPrimaryDispatchCf)();
            PersistentSettings_Flush();
            UiFrame_FlushInputAndResetPendingTicks();
            g_FrontendScenarioInitializationCount = 0;
            FVar10 = g_FrontendPlayerRuntimeBlockCount;
            pFVar15 = g_FrontendPlayerRuntimeBlocks;
            do {
              (pFVar15->factionAssignment).roleStateFlags = 0;
              pFVar15 = pFVar15 + 1;
              FVar10 = FVar10 - 1;
              iVar12 = extraout_EDX;
              pvVar17 = g_FrontendLoadedCampaignAsset;
            } while (FVar10 != 0);
            goto joined_r0x0054707a;
          }
          uVar9 = 0;
          RVar7 = param_3;
          goto FrontendMainLoop_InitializeRequestedPage;
        }
        FrontendSession_ShowPage9WithCompactLayout(g_FrontendRootNode);
        g_FrontendPendingPageAction = SESSION_NETWORK_ROLE_LOCAL;
        SVar14 = extraout_ECX_27;
      }
    } while( true );
  }
FrontendMainLoop_ShutdownAndReturn:
  uVar24 = FrontendRuntime_ShutdownAndReleaseResourcesRegs();
  return CONCAT44(param_2,(int)uVar24);
  while( true ) {
    pvVar17 = (void *)((int)pvVar17 + 0x180);
    iVar12 = iVar12 + -1;
    if (iVar12 == 0) break;
FrontendCampaign_FindSelectedLevelRecord:
    if (iVar2 == *(int *)((int)pvVar17 + 0x300)) {
      uVar22 = (void *)0xfffffcf3 < pvVar17;
      WidePath_CombineDirectoryAndLeaf
                ((word *)&g_FrontendScenarioPathScratchUtf16,(word *)((int)pvVar17 + 0x30c),
                 (word *)u_level_0050daac);
      WidePath_SetExtensionCode(0x76656c,(word *)&g_FrontendScenarioPathScratchUtf16);
      iVar11 = extraout_ECX_07;
      iVar12 = extraout_EDX_01;
      goto FrontendScenario_InitializeSelectedLevel;
    }
  }
  Resource_Release(g_FrontendLoadedCampaignAsset);
  g_FrontendLoadedCampaignAsset = (void *)0x0;
  g_FrontendScenarioInitializationCount = 0;
  iVar11 = extraout_ECX_06;
  iVar12 = extraout_EDX_00;
FrontendScenario_UseResolvedPathOrFallbackPage:
  uVar22 = 0;
  if (g_FrontendScenarioPathScratchUtf16 != 0) {
FrontendScenario_InitializeSelectedLevel:
    uVar24 = Frontend_Init(iVar11,iVar12,10);
    uVar9 = (undefined4)uVar24;
    if ((bool)uVar22) goto FrontendMainLoop_ShutdownAndReturn;
    g_FrontendScenarioInitializationCount = g_FrontendScenarioInitializationCount + 1;
    g_FrontendPendingPageAction = 8;
    pFVar1 = &(g_FrontendPlayerRuntimeBlocks->factionAssignment).roleStateFlags;
    *pFVar1 = *pFVar1 | 4;
    if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_CLIENT) == SESSION_NETWORK_ROLE_LOCAL) {
      if ((g_FrontendLoadedLevelAsset != (LevelAssetRuntimeImagePrefix370 *)0x0) &&
         (0xffff < (g_FrontendLoadedLevelAsset->header).pathOffsets.levelPathOffset)) {
        Resource_Release((void *)(g_FrontendLoadedLevelAsset->header).pathOffsets.levelPathOffset);
      }
      Resource_Release(g_FrontendLoadedLevelAsset);
      g_FrontendLoadedLevelAsset = (LevelAssetRuntimeImagePrefix370 *)0x0;
      Package_LoadEntry((word *)&g_FrontendScenarioPathScratchUtf16);
      g_FrontendLoadedLevelAsset =
           (LevelAssetRuntimeImagePrefix370 *)
           (*g_FatalErrorPrimaryDispatchCf)(unaff_EDI,unaff_EBX,uVar9);
      pwVar6 = (word *)WidePath_SetExtensionCode
                                 (0x646c66,(word *)((g_FrontendLoadedLevelAsset->header).common.
                                                    buildMetadata.assetRelativeAddressAnchor28 +
                                                   ((g_FrontendLoadedLevelAsset->header).pathOffsets
                                                    .levelPathOffset - 0x28)));
      pwVar6 = (word *)WidePath_CombineDirectoryAndLeaf
                                 ((word *)&g_LevelResourcePathScratchUtf16,pwVar6,
                                  (word *)&g_ExecutableDirectoryUtf16);
      sourceGrid = Package_LoadEntry(pwVar6);
      source_00 = g_FrontendLoadedLevelAsset;
      pbVar18 = g_PackageScratchBuffer;
      uVar9 = extraout_ECX_08;
      if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_HOST) != SESSION_NETWORK_ROLE_LOCAL) {
        AVar3 = (sourceGrid->common).allocationSizeBytes;
        *(AssetAllocationSizeBytes *)g_PackageScratchBuffer =
             (g_FrontendLoadedLevelAsset->header).common.allocationSizeBytes;
        *(AssetAllocationSizeBytes *)(pbVar18 + 4) = AVar3;
        pFVar25 = sourceGrid;
        PckCodec_EncodeHuffmanRle
                  (0x7fffe8,pbVar18 + 0x10,(source_00->header).common.allocationSizeBytes,
                   (byte *)source_00);
        uVar24 = (*g_FatalErrorPrimaryDispatchCf)();
        iVar12 = (int)uVar24;
        *(int *)(pbVar18 + 8) = iVar12;
        PckCodec_EncodeFieldGrid
                  (extraout_ECX_09 - iVar12,(byte *)((int)((ulonglong)uVar24 >> 0x20) + iVar12),
                   (sourceGrid->common).allocationSizeBytes,sourceGrid);
        uVar24 = (*g_FatalErrorPrimaryDispatchCf)();
        *(int *)(pbVar18 + 0xc) = (int)uVar24;
        (*g_MemoryApi.alloc)(((int)((ulonglong)uVar24 >> 0x20) + (int)uVar24) - (int)pbVar18);
        uVar24 = (*g_FatalErrorPrimaryDispatchCf)();
        puVar21 = (undefined4 *)uVar24;
        for (uVar13 = extraout_ECX_10 >> 2; uVar13 != 0; uVar13 = uVar13 - 1) {
          *puVar21 = *(undefined4 *)pbVar18;
          pbVar18 = pbVar18 + 4;
          puVar21 = puVar21 + 1;
        }
        sourceGrid = pFVar25;
        UiTransferMailbox_SetOutgoingBuffer
                  ((UiTransferPayloadByteCount)((ulonglong)uVar24 >> 0x20),(undefined4 *)uVar24);
        uVar9 = extraout_ECX_11;
      }
      (source_00->header).pathOffsets.levelPathOffset = (AssetRelativeOffset)sourceGrid;
      FrontendPlayerRuntime_InitializeFactionAssignments(uVar9,sourceGrid);
      SVar14 = extraout_ECX_12;
    }
    else {
      UiTransferMailbox_MarkUnavailable();
      g_FrontendScenarioTransferState = 5;
      SVar14 = extraout_ECX_13;
    }
    goto FrontendMainLoop_ProcessFrameAndPendingPageAction;
  }
  RVar7 = 0xc;
  uVar9 = 5;
FrontendMainLoop_InitializeRequestedPage:
  bVar23 = false;
  Frontend_Init(uVar9,iVar12,RVar7);
  SVar14 = extraout_ECX_05;
  g_FrontendPendingPageAction = extraout_ECX_05;
  if (!bVar23) goto FrontendMainLoop_ProcessFrameAndPendingPageAction;
  goto FrontendMainLoop_ShutdownAndReturn;
}

/* Address: 0x0050C380.
   Ownership: ui/frontend/runtime.
   Purpose: Handles frontend model pointer context select best model hit target and resolve action.
   Local calls: FrontendModelPointerContext_FindBestEligibleModelHitTarget.
*/
GraphicsCursorFrameIndex
FrontendModelPointerContext_SelectBestModelHitTargetAndResolveAction
          (int pointerY,int pointerX,FrontendModelPointerContextRuntimeState118 *context)

{
  dword dVar1;
  GraphicsCursorFrameIndex GVar2;
  qword qVar3;
  
  qVar3 = FrontendModelPointerContext_FindBestEligibleModelHitTarget(pointerY,pointerX,context);
  context->selectedHitMetric = (int)qVar3;
  context->selectedModelNode = (ModelRuntimeNode *)(qVar3 >> 0x20);
  if ((context->contextFlags & FRONTEND_MODEL_POINTER_CONTEXT_ROUTE_TO_SECONDARY_CALLBACK) != 0) {
    if (context->resolvedActionCallback108 != (FrontendModelPointerResolvedActionCallbackProc *)0x0)
    {
      dVar1 = (*context->resolvedActionCallback108)
                        (context->callbackArgumentF0,context->callbackArgumentEC,
                         context->callbackArgumentE8,context->selectedHitMetric,
                         context->selectedModelNode,context);
      return dVar1;
    }
    return 0;
  }
  if ((context->contextFlags & FRONTEND_MODEL_POINTER_CONTEXT_ROUTE_TO_BUILTIN_ACTION_RESOLUTION) ==
      0) {
    if (context->resolvedActionCallback104 != (FrontendModelPointerResolvedActionCallbackProc *)0x0)
    {
      dVar1 = (*context->resolvedActionCallback104)
                        (context->callbackArgumentF0,context->callbackArgumentEC,
                         context->callbackArgumentE8,context->selectedHitMetric,
                         context->selectedModelNode,context);
      return dVar1;
    }
    return 0;
  }
  if ((context->contextFlags & FRONTEND_MODEL_POINTER_CONTEXT_SUPPRESS_BUILTIN_ACTION_RESOLUTION) !=
      0) {
    return 0;
  }
  if ((context->contextFlags & FRONTEND_MODEL_POINTER_CONTEXT_OBSERVED_ACTION_BRANCH_00000100) != 0)
  {
    if ((g_CursorButtonState & 1) != 0) {
      return 0x11;
    }
    return 0x10;
  }
  if ((context->contextFlags & FRONTEND_MODEL_POINTER_CONTEXT_OBSERVED_ACTION_BRANCH_00008000) != 0)
  {
    if ((g_KeyboardStateMask & 0xc) != 0) {
      if ((g_CursorButtonState & 1) != 0) {
        return 0x11;
      }
      return 0x12;
    }
    if ((g_CursorButtonState & 1) != 0) {
      return 0x14;
    }
    return 0x13;
  }
  if ((context->contextFlags & FRONTEND_MODEL_POINTER_CONTEXT_OBSERVED_ACTION_BRANCH_00000200) == 0)
  {
    return 0;
  }
  if ((g_KeyboardStateMask & 0xc) != 0) {
    return 0xf;
  }
  if ((g_KeyboardStateMask & 0x30) == 0) {
    if ((g_KeyboardStateMask & 3) != 0) {
      return 0x25;
    }
    if ((context->contextFlags & FRONTEND_MODEL_POINTER_CONTEXT_OBSERVED_BUTTON_BRANCH_04000000) ==
        0) {
      if ((g_CursorButtonState & 1) == 0) {
        return 1;
      }
      if ((context->contextFlags & FRONTEND_MODEL_POINTER_CONTEXT_OBSERVED_CODE_OVERRIDE_40000000)
          != 0) {
        return 0xe;
      }
      if ((context->contextFlags & FRONTEND_MODEL_POINTER_CONTEXT_OBSERVED_CODE_OVERRIDE_80000000)
          == 0) {
        return 0x25;
      }
    }
    else {
      if ((g_CursorButtonState & 1) != 0) {
        if ((context->contextFlags & FRONTEND_MODEL_POINTER_CONTEXT_OBSERVED_CODE_OVERRIDE_40000000)
            != 0) {
          return 0xf;
        }
        if ((context->contextFlags & FRONTEND_MODEL_POINTER_CONTEXT_OBSERVED_CODE_OVERRIDE_80000000)
            == 0) {
          return 0;
        }
        goto FrontendModelPointerAction_ReturnObservedCode11;
      }
      if ((context->contextFlags & FRONTEND_MODEL_POINTER_CONTEXT_OBSERVED_CODE_OVERRIDE_40000000)
          != 0) {
        return 0xe;
      }
      if ((context->contextFlags & FRONTEND_MODEL_POINTER_CONTEXT_OBSERVED_CODE_OVERRIDE_80000000)
          == 0) {
        return 0x25;
      }
    }
    GVar2 = 0x10;
  }
  else {
FrontendModelPointerAction_ReturnObservedCode11:
    GVar2 = 0x11;
  }
  return GVar2;
}

/* Address: 0x0050C5A0.
   Ownership: ui/frontend/runtime.
   Purpose: Exact packed function-table or callback-registration provenance plus immutable body topology prove this
   callable entry.
   Local calls: FrontendModelPointerContext_FindBestEligibleModelHitTarget.
*/
void FrontendModelPointerContext_NonRightPress
               (UiPointerWheelDelta wheelDelta,UiPixelCoordinate pointerY,UiPixelCoordinate pointerX
               ,FrontendModelPointerContextRuntimeState118 *callbackContext)

{
  qword qVar1;
  
  callbackContext[1].base.nodeFlags = pointerX;
  callbackContext[1].contextFlags = pointerY;
  qVar1 = FrontendModelPointerContext_FindBestEligibleModelHitTarget
                    (pointerY,pointerX,callbackContext);
  callbackContext->selectedModelNode = (ModelRuntimeNode *)(qVar1 >> 0x20);
  callbackContext->selectedHitMetric = (int)qVar1;
  callbackContext->contextFlags =
       callbackContext->contextFlags | FRONTEND_MODEL_POINTER_CONTEXT_ROUTE_TO_SECONDARY_CALLBACK;
  if (callbackContext->resolvedActionCallback10C !=
      (FrontendModelPointerResolvedActionCallbackProc *)0x0) {
    (*callbackContext->resolvedActionCallback10C)
              (callbackContext->callbackArgumentF0,callbackContext->callbackArgumentEC,
               callbackContext->callbackArgumentE8,callbackContext->selectedHitMetric,
               callbackContext->selectedModelNode,callbackContext);
  }
  return;
}

/* Address: 0x0050C610.
   Ownership: ui/frontend/runtime.
   Purpose: Exact packed function-table or callback-registration provenance plus immutable body topology prove this
   callable entry.
   Local calls: FrontendModelPointerContext_FindBestEligibleModelHitTarget.
*/
void FrontendModelPointerContext_NonRightRelease
               (UiPointerWheelDelta wheelDelta,UiPixelCoordinate pointerY,UiPixelCoordinate pointerX
               ,FrontendModelPointerContextRuntimeState118 *callbackContext)

{
  qword qVar1;
  
  qVar1 = FrontendModelPointerContext_FindBestEligibleModelHitTarget
                    (pointerY,pointerX,callbackContext);
  callbackContext->selectedModelNode = (ModelRuntimeNode *)(qVar1 >> 0x20);
  callbackContext->selectedHitMetric = (int)qVar1;
  callbackContext->contextFlags =
       callbackContext->contextFlags & ~FRONTEND_MODEL_POINTER_CONTEXT_ROUTE_TO_SECONDARY_CALLBACK;
  if (callbackContext->resolvedActionCallback114 !=
      (FrontendModelPointerResolvedActionCallbackProc *)0x0) {
    (*callbackContext->resolvedActionCallback114)
              (callbackContext->callbackArgumentF0,callbackContext->callbackArgumentEC,
               callbackContext->callbackArgumentE8,callbackContext->selectedHitMetric,
               callbackContext->selectedModelNode,callbackContext);
  }
  return;
}

/* Address: 0x0050C670.
   Ownership: ui/frontend/runtime.
   Purpose: Exact packed function-table or callback-registration provenance plus immutable body topology prove this
   callable entry.
   Local calls: FrontendModelPointerContext_FindBestEligibleModelHitTarget.
*/
void FrontendModelPointerContext_NonRightDrag
               (UiPointerWheelDelta wheelDelta,UiPixelCoordinate pointerY,UiPixelCoordinate pointerX
               ,FrontendModelPointerContextRuntimeState118 *callbackContext)

{
  qword qVar1;
  
  *(UiPixelCoordinate *)callbackContext[1].reserved50_5F = pointerX;
  *(UiPixelCoordinate *)(callbackContext[1].reserved50_5F + 4) = pointerY;
  qVar1 = FrontendModelPointerContext_FindBestEligibleModelHitTarget
                    (pointerY,pointerX,callbackContext);
  callbackContext->selectedModelNode = (ModelRuntimeNode *)(qVar1 >> 0x20);
  callbackContext->selectedHitMetric = (int)qVar1;
  if (callbackContext->resolvedActionCallback110 !=
      (FrontendModelPointerResolvedActionCallbackProc *)0x0) {
    (*callbackContext->resolvedActionCallback110)
              (callbackContext->callbackArgumentF0,callbackContext->callbackArgumentEC,
               callbackContext->callbackArgumentE8,callbackContext->selectedHitMetric,
               callbackContext->selectedModelNode,callbackContext);
  }
  return;
}

/* Address: 0x00549B40.
   Ownership: ui/frontend/runtime.
   Purpose: Recovered action-table target FRONTEND_PAGE20[68] (0x2044).
   Local calls: FrontendUiAction2044_IndexedSelectionHelper.
   Cross-module calls: FrontendCommandQueue_EnqueueLocalPlayerCommand [network/protocol/commands].
*/
void FrontendUiAction2044_Handler(UiNodeBase *factionControl)

{
  CommandPayloadDword04 payloadDword04;
  
  payloadDword04 = 0;
  do {
    if ((int)factionControl - g_FrontendRootNode ==
        g_FrontendTaskAssignmentControlOffsets.factionControls.offsets[payloadDword04 + 1]) {
      if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_NETWORKED_MASK) ==
          SESSION_NETWORK_ROLE_LOCAL) {
        FrontendUiAction2044_IndexedSelectionHelper(g_LocalPlayerRuntimeId,0,0,payloadDword04);
      }
      else {
        FrontendCommandQueue_EnqueueLocalPlayerCommand(0x650,0,0,payloadDword04);
      }
      return;
    }
    payloadDword04 = payloadDword04 + 1;
  } while (payloadDword04 < 7);
  return;
}

/* Address: 0x00549BC0.
   Ownership: ui/frontend/runtime.
   Purpose: Recovered action-table target FRONTEND_PAGE20[69] (0x2045).
   Local calls: FrontendUiAction2045_IndexedSelectionHelper.
   Cross-module calls: FrontendCommandQueue_EnqueueLocalPlayerCommand [network/protocol/commands].
*/
void FrontendUiAction2045_Handler(UiNodeBase *playerControl)

{
  CommandPayloadDword04 payloadDword04;
  
  payloadDword04 = 0;
  do {
    if ((int)playerControl - g_FrontendRootNode ==
        g_FrontendTaskAssignmentControlOffsets.playerControls.offsets[payloadDword04 + 1]) {
      if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_NETWORKED_MASK) ==
          SESSION_NETWORK_ROLE_LOCAL) {
        FrontendUiAction2045_IndexedSelectionHelper(g_LocalPlayerRuntimeId,0,0,payloadDword04);
      }
      else {
        FrontendCommandQueue_EnqueueLocalPlayerCommand(0x6f0,0,0,payloadDword04);
      }
      return;
    }
    payloadDword04 = payloadDword04 + 1;
  } while (payloadDword04 < 7);
  return;
}

/* Address: 0x00549C40.
   Ownership: ui/frontend/runtime.
   Purpose: Recovered action-table target FRONTEND_PAGE20[70] (0x2046).
   Local calls: FrontendUiAction2046_IndexedSelectionHelper.
   Cross-module calls: FrontendCommandQueue_EnqueueLocalPlayerCommand [network/protocol/commands].
*/
void FrontendUiAction2046_Handler(UiNodeBase *selectionRowControl)

{
  CommandPayloadDword04 payloadDword04;
  
  payloadDword04 = 0;
  do {
    if ((int)selectionRowControl - g_FrontendRootNode ==
        g_FrontendTaskAssignmentControlOffsets.selectionRows.offsets[payloadDword04 + 1]) {
      if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_NETWORKED_MASK) ==
          SESSION_NETWORK_ROLE_LOCAL) {
        FrontendUiAction2046_IndexedSelectionHelper(g_LocalPlayerRuntimeId,0,0,payloadDword04);
      }
      else {
        FrontendCommandQueue_EnqueueLocalPlayerCommand(0x750,0,0,payloadDword04);
      }
      return;
    }
    payloadDword04 = payloadDword04 + 1;
  } while (payloadDword04 < 7);
  return;
}

/* Address: 0x0050BB80.
   Ownership: ui/frontend/runtime.
   Purpose: UNREFERENCED_FRAMED_RUNTIME_INITIALIZER_PROVISIONAL.
   Cross-module calls: UiContainer_RelocateChildren [ui/controls/layout].
*/
void FrontendModelPointerContext_Relocate
               (UiSerializedRelocationDelta relocationDelta,UiNodeBase *control)

{
  control[1].topAnchorQ31 = 0;
  control[1].rightAnchorQ31 = 0;
  control[1].bottomAnchorQ31 = 0;
  if (control[1].layoutHeight == 0) {
    control[1].layoutHeight = -0x4000;
  }
  if (control[1].nodeFlags == 0) {
    control[1].nodeFlags = 0x4000;
  }
  if (control[2].nextSibling == (UiNodeBase *)0x0) {
    control[2].nextSibling = (UiNodeBase *)0x400;
  }
  if (control[2].firstChild == (UiNodeBase *)0x0) {
    control[2].firstChild = (UiNodeBase *)0x7f000;
  }
  control[1].vtable = (UiNodeVtable *)0x0;
  control[2].top = 0;
  control[2].layoutWidth = 0;
  control[3].right = 0;
  UiContainer_RelocateChildren(relocationDelta,control);
  return;
}

/* Address: 0x0050BC30.
   Ownership: ui/frontend/runtime.
   Purpose: Exact packed function-table or callback-registration provenance plus immutable body topology prove this
   callable entry.
   Cross-module calls: WorldRuntime_ClearFieldGridDirtyFlag [world/runtime/core], UiContainer_LayoutChildren
   [ui/controls/layout].
*/
void FrontendModelPointerContext_Layout(WorldRuntimeContext *callbackContext)

{
  WorldRuntime_ClearFieldGridDirtyFlag(callbackContext);
  UiContainer_LayoutChildren((UiNodeBase *)callbackContext);
  return;
}

/* Address: 0x0050BC60.
   Ownership: ui/frontend/runtime.
   Purpose: Exact packed function-table or callback-registration provenance plus immutable body topology prove this
   callable entry.
   Cross-module calls: Graphics_SetProjectionClipRect [graphics/core/runtime], Graphics_SetViewProjectionParameters
   [graphics/core/runtime], SpatialSound_RebuildListenerTransformFromPose [audio/spatial/runtime],
   Graphics_SetProjectionViewport [graphics/core/runtime], Graphics_SetAuxiliaryOrientation
   [graphics/core/runtime], Graphics_SetSceneBounds [graphics/core/runtime].
*/
void FrontendModelPointerContext_DrawClipped
               (UiPixelCoordinate clipTop,UiPixelCoordinate clipLeft,UiPixelCoordinate clipBottom,
               UiPixelCoordinate clipRight,UiNodeBase *control)

{
  sdword *psVar1;
  UiPixelCoordinate UVar2;
  UiPixelCoordinate UVar3;
  GraphicsPrimitiveQueue *pGVar4;
  UiAnchorFractionQ31 extraout_EAX;
  dword dVar5;
  UiAnchorFractionQ31 extraout_EAX_00;
  UiAnchorFractionQ31 extraout_EAX_01;
  UiAnchorFractionQ31 extraout_EAX_02;
  UiPixelCoordinate extraout_ECX;
  sdword extraout_ECX_00;
  sdword extraout_ECX_01;
  sdword extraout_ECX_02;
  sdword extraout_ECX_03;
  sdword extraout_ECX_04;
  sdword extraout_ECX_05;
  sdword extraout_ECX_06;
  sdword extraout_ECX_07;
  sdword extraout_ECX_08;
  sdword sVar6;
  uint uVar7;
  code *pcVar8;
  code *extraout_EDX;
  code *extraout_EDX_00;
  UiPixelCoordinate extraout_EDX_01;
  sdword sVar9;
  sdword extraout_EDX_02;
  sdword extraout_EDX_03;
  sdword extraout_EDX_04;
  sdword extraout_EDX_05;
  sdword sVar10;
  int iVar11;
  undefined1 uVar12;
  bool bVar13;
  undefined8 uVar14;
  
  if (((uint)control[1].nextSibling & 0x2000) != 0) {
    return;
  }
  if (clipRight < control->left) {
    clipRight = control->left;
  }
  if (control->right < clipLeft) {
    clipLeft = control->right;
  }
  if (clipBottom < control->top) {
    clipBottom = control->top;
  }
  if (control->bottom < clipTop) {
    clipTop = control->bottom;
  }
  (*g_GraphicsSetViewportAndClearDepth)(clipTop,clipLeft,clipBottom,clipRight);
  (*g_SpinLockAcquire)((RuntimeSpinLockValue *)control[2].rightAnchorQ31);
  UVar3 = g_CursorOverrideY;
  UVar2 = g_CursorOverrideX;
  control[2].nodeFlags = 0;
  control[3].nextSibling = (UiNodeBase *)0x7fffffff;
  control[3].firstChild = (UiNodeBase *)0x7fffffff;
  control[3].parent = (UiNodeBase *)0x7fffffff;
  control[3].vtable = (UiNodeVtable *)0x7fffffff;
  control[3].left = UVar2 << 0xc;
  control[3].top = UVar3 << 0xc;
  control[1].left = 0;
  Graphics_SetProjectionClipRect(clipTop,clipLeft,clipBottom,clipRight);
  Graphics_SetViewProjectionParameters
            (control[1].bottomOffset,control[1].rightOffset,control[1].topOffset,
             control[1].leftOffset,control[1].bottom,control[1].right,control[1].top);
  uVar12 = 0;
  if (((uint)control[1].nextSibling & 0x10000) != 0) {
    uVar7 = (int)control[1].leftAnchorQ31 >> 2;
    uVar12 = CARRY4(uVar7,control[1].bottomAnchorQ31);
    SpatialSound_RebuildListenerTransformFromPose
              (control[1].rightOffset,control[1].topOffset,uVar7 + control[1].bottomAnchorQ31,
               control[1].rightAnchorQ31,control[1].topAnchorQ31);
  }
  Graphics_SetProjectionViewport(control->bottom,control->right,control->top,control->left);
  Graphics_SetAuxiliaryOrientation(control[2].topOffset,control[2].leftOffset);
  Graphics_SetSceneBounds
            ((GraphicsSceneExtentFixed)control[4].vtable,(GraphicsSceneExtentFixed)control[4].parent
             ,(GraphicsSceneExtentFixed)control[4].firstChild,
             (GraphicsSceneExtentFixed)control[4].nextSibling,control[3].nodeFlags,
             control[3].layoutHeight,control[3].layoutWidth,control[3].bottomAnchorQ31);
  Graphics_RebuildFrustumPlanes();
  (*g_GraphicsBeginScene)();
  (*g_SpinLockReleaseAndInvoke)
            ((SpinLockReleaseCallbackProc *)control[2].bottomAnchorQ31,
             (RuntimeSpinLockValue *)control[2].rightAnchorQ31);
  (*g_SpinLockAcquire)((RuntimeSpinLockValue *)control[2].rightAnchorQ31);
  GraphicsShadingRuntime_RebuildCompactLightingRecords();
  (*g_SpinLockReleaseAndInvoke)
            ((SpinLockReleaseCallbackProc *)control[2].bottomAnchorQ31,
             (RuntimeSpinLockValue *)control[2].rightAnchorQ31);
  (*g_SpinLockAcquire)((RuntimeSpinLockValue *)control[2].rightAnchorQ31);
  pGVar4 = GraphicsPrimitiveQueue_ResetGlobal();
  sVar10 = clipTop;
  iVar11 = clipBottom;
  if (!(bool)uVar12) {
    Graphics_SetActivePrimitiveQueue(pGVar4);
    control[2].leftAnchorQ31 = extraout_EAX;
    if (control[4].bottomOffset != 0) {
      (*(code *)control[4].bottomOffset)(0,control);
    }
    iVar11 = control[2].layoutWidth;
    pcVar8 = ModelRuntime_CullAndRenderHierarchyRecursive;
    if (((uint)control[1].nextSibling & 0x1000) != 0) {
      pcVar8 = ModelRuntime_RenderHierarchyRecursiveAlternatePath;
    }
    for (; iVar11 != 0; iVar11 = *(int *)(iVar11 + 4)) {
      if ((((*(uint *)(iVar11 + 0x4c) & 0x40) == 0) && ((*(uint *)(iVar11 + 0x4c) & 0x200) != 0)) &&
         (*(uint *)(iVar11 + 0x4c) = *(uint *)(iVar11 + 0x4c) & 0xfffffffd,
         (*(uint *)(iVar11 + 0x58) & 0xff000000) != 0)) {
        (*pcVar8)(iVar11);
        pcVar8 = extraout_EDX;
      }
    }
    if (control[4].bottomOffset != 0) {
      (*(code *)control[4].bottomOffset)(1,control);
    }
    (*(code *)PTR_GraphicsPrimitiveQueue_RadixSortForRendering_00485844)
              (control->nodeFlags & UI_NODE_SUPPRESSED,control[2].leftAnchorQ31);
    (*g_GraphicsDrawPrimitiveQueue)
              (clipTop,clipLeft,clipBottom,clipRight,
               (GraphicsPrimitiveQueue *)control[2].leftAnchorQ31);
    dVar5 = GraphicsPrimitiveQueue_GetCount((GraphicsPrimitiveQueue *)control[2].leftAnchorQ31);
    control[1].left = control[1].left + dVar5;
    (*g_SpinLockReleaseAndInvoke)
              ((SpinLockReleaseCallbackProc *)control[2].bottomAnchorQ31,
               (RuntimeSpinLockValue *)control[2].rightAnchorQ31);
    (*g_SpinLockAcquire)((RuntimeSpinLockValue *)control[2].rightAnchorQ31);
    iVar11 = 0;
    if ((((uint)control[1].nextSibling & 0x4000) != 0) &&
       (bVar13 = false, control[1].parent != (UiNodeBase *)0x0)) {
      pGVar4 = GraphicsPrimitiveQueue_ResetGlobal();
      if (bVar13) goto LAB_0050c196;
      Graphics_SetActivePrimitiveQueue(pGVar4);
      control[2].leftAnchorQ31 = extraout_EAX_00;
      TerrainProjectedGrid_TransformShadeAndQueue
                ((int)control[1].parent,(GraphicsPrimitiveQueue *)control);
      (*(code *)PTR_GraphicsPrimitiveQueue_RadixSortForRendering_00485844)
                (control->nodeFlags & UI_NODE_SUPPRESSED,control[2].leftAnchorQ31);
      (*g_GraphicsDrawPrimitiveQueue)
                (clipTop,clipLeft,clipBottom,clipRight,
                 (GraphicsPrimitiveQueue *)control[2].leftAnchorQ31);
      dVar5 = GraphicsPrimitiveQueue_GetCount((GraphicsPrimitiveQueue *)control[2].leftAnchorQ31);
      control[1].left = control[1].left + dVar5;
    }
    (*g_SpinLockReleaseAndInvoke)
              ((SpinLockReleaseCallbackProc *)control[2].bottomAnchorQ31,
               (RuntimeSpinLockValue *)control[2].rightAnchorQ31);
    (*g_SpinLockAcquire)((RuntimeSpinLockValue *)control[2].rightAnchorQ31);
    uVar12 = false;
    if (((uint)control[1].nextSibling & 0x20000) != 0) {
      iVar11 = control[2].layoutWidth;
      pGVar4 = GraphicsPrimitiveQueue_ResetGlobal();
      if ((bool)uVar12) goto LAB_0050c196;
      Graphics_SetActivePrimitiveQueue(pGVar4);
      control[2].leftAnchorQ31 = extraout_EAX_01;
      if (iVar11 != 0) {
        GraphicsShadingGeneratedTexture_ResetPassScratchAndClearAlphaPlanes();
        do {
          if ((((*(uint *)(iVar11 + 0x4c) & 0x40) == 0) && ((*(uint *)(iVar11 + 0x4c) & 0x100) != 0)
              ) && ((*(uint *)(iVar11 + 0x58) & 0xff000000) != 0)) {
            GraphicsShadingGeneratedTexture_ProcessRenderableHierarchy(iVar11,(int)control);
          }
          iVar11 = *(int *)(iVar11 + 4);
        } while (iVar11 != 0);
        GraphicsShadingGeneratedTexture_RefreshTouchedAlphaSubresources();
        iVar11 = 0;
      }
      (*(code *)PTR_GraphicsPrimitiveQueue_RadixSortForRendering_00485844)
                (control->nodeFlags & UI_NODE_SUPPRESSED,control[2].leftAnchorQ31);
      (*g_GraphicsDrawPrimitiveQueue)
                (clipTop,clipLeft,clipBottom,clipRight,
                 (GraphicsPrimitiveQueue *)control[2].leftAnchorQ31);
      dVar5 = GraphicsPrimitiveQueue_GetCount((GraphicsPrimitiveQueue *)control[2].leftAnchorQ31);
      psVar1 = &control[1].left;
      uVar12 = CARRY4(*psVar1,dVar5);
      *psVar1 = *psVar1 + dVar5;
    }
    (*g_SpinLockReleaseAndInvoke)
              ((SpinLockReleaseCallbackProc *)control[2].bottomAnchorQ31,
               (RuntimeSpinLockValue *)control[2].rightAnchorQ31);
    (*g_SpinLockAcquire)((RuntimeSpinLockValue *)control[2].rightAnchorQ31);
    pGVar4 = GraphicsPrimitiveQueue_ResetGlobal();
    if (!(bool)uVar12) {
      Graphics_SetActivePrimitiveQueue(pGVar4);
      control[2].leftAnchorQ31 = extraout_EAX_02;
      if (control[4].bottomOffset != 0) {
        (*(code *)control[4].bottomOffset)(0,control);
      }
      iVar11 = control[2].layoutWidth;
      pcVar8 = ModelRuntime_CullAndRenderHierarchyRecursive;
      if (((uint)control[1].nextSibling & 0x1000) != 0) {
        pcVar8 = ModelRuntime_RenderHierarchyRecursiveAlternatePath;
      }
      for (; iVar11 != 0; iVar11 = *(int *)(iVar11 + 4)) {
        if (((*(uint *)(iVar11 + 0x4c) & 0x240) == 0) &&
           (*(uint *)(iVar11 + 0x4c) = *(uint *)(iVar11 + 0x4c) & 0xfffffffd,
           (*(uint *)(iVar11 + 0x58) & 0xff000000) != 0)) {
          (*pcVar8)(iVar11);
          pcVar8 = extraout_EDX_00;
        }
      }
      if (control[4].bottomOffset != 0) {
        (*(code *)control[4].bottomOffset)(1,control);
      }
      (*(code *)PTR_GraphicsPrimitiveQueue_RadixSortForRendering_00485844)
                (control->nodeFlags & UI_NODE_SUPPRESSED,control[2].leftAnchorQ31);
      (*g_GraphicsDrawPrimitiveQueue)
                (clipTop,clipLeft,clipBottom,clipRight,
                 (GraphicsPrimitiveQueue *)control[2].leftAnchorQ31);
      dVar5 = GraphicsPrimitiveQueue_GetCount((GraphicsPrimitiveQueue *)control[2].leftAnchorQ31);
      control[1].left = control[1].left + dVar5;
      (*g_SpinLockReleaseAndInvoke)
                ((SpinLockReleaseCallbackProc *)control[2].bottomAnchorQ31,
                 (RuntimeSpinLockValue *)control[2].rightAnchorQ31);
      (*g_SpinLockAcquire)((RuntimeSpinLockValue *)control[2].rightAnchorQ31);
      sVar10 = clipRight;
      iVar11 = clipBottom;
      if ((((clipRight == control->left) && (clipLeft == control->right)) &&
          (clipBottom == control->top)) && (clipTop == control->bottom)) {
        control[1].nextSibling = (UiNodeBase *)((uint)control[1].nextSibling | 0x800);
      }
    }
  }
LAB_0050c196:
  (*g_GraphicsEndScene)();
  g_RenderedFrameCountSinceDebugRefresh = g_RenderedFrameCountSinceDebugRefresh + 1;
  if ((control->nodeFlags & UI_NODE_SUPPRESSED) == 0) {
    g_SelectionPanelBlitOpaque = g_GraphicsTextureSourceBlitSourceAlpha;
    g_SelectionPanelBlitClipped = g_GraphicsTextureSourceBlitTiledSourceAlpha;
  }
  else {
    g_SelectionPanelBlitOpaque = g_GraphicsTextureSourceBlitHalfSourceRgb;
    g_SelectionPanelBlitClipped = g_GraphicsTextureSourceBlitTiledHalfSourceRgb;
  }
  if ((g_UiCommandRuntimeFlags & 0x8000) == 0) {
    sVar6 = extraout_ECX;
    sVar9 = extraout_EDX_01;
    if (((uint)control[1].nextSibling & 0x400) != 0) {
      uVar14 = SelectionOverlay_RenderSelectedArmyMetrics
                         (extraout_EDX_01,extraout_ECX,iVar11,sVar10);
      sVar9 = (sdword)((ulonglong)uVar14 >> 0x20);
      bVar13 = false;
      sVar6 = extraout_ECX_00;
      if ((control[3].right != 0) &&
         (SelectionInfo_FindEntryCf((GameEntityRuntime *)control[3].right), sVar6 = extraout_ECX_01,
         sVar9 = extraout_EDX_02, bVar13)) {
        SelectionOverlay_RenderArmyMetricsForEntity
                  (extraout_ECX_01,extraout_EDX_02,extraout_EDX_02,extraout_ECX_01,iVar11,sVar10,
                   control[3].right);
        sVar6 = extraout_ECX_02;
        sVar9 = extraout_EDX_03;
      }
    }
    if (((uint)control[1].nextSibling & 0x80) != 0) {
      SelectionOverlay_DrawBoundsFrame
                (sVar9,sVar6,iVar11,sVar10,control[4].bottomAnchorQ31,control[4].rightAnchorQ31,
                 control[4].topAnchorQ31,control[4].leftAnchorQ31);
      sVar6 = extraout_ECX_03;
      sVar9 = extraout_EDX_04;
    }
    if (((uint)control[1].nextSibling & 0x200000) != 0) {
      uVar14 = SelectionOverlay_DrawMarkerADForFieldGridTerrainPoints
                         (sVar9,sVar6,iVar11,sVar10,control[4].layoutHeight,
                          (int *)control[4].layoutWidth,(FieldGridAsset *)control[1].parent);
      sVar9 = (sdword)((ulonglong)uVar14 >> 0x20);
      sVar6 = extraout_ECX_04;
    }
    if ((((uint)control[1].nextSibling & 0x100000) != 0) &&
       (control[3].vtable != (UiNodeVtable *)0x7fffffff)) {
      SelectionOverlay_DrawMarkerACForWorldSurfacePoint
                (sVar6,sVar9,sVar9,sVar6,iVar11,sVar10,
                 (uint)((g_UiCommandModeGColorVariantLimit & 0xff000000) != 0),
                 (Q12)control[3].parent,(Q12)control[3].firstChild,
                 (FieldGridAsset *)control[1].parent);
      sVar6 = extraout_ECX_05;
      sVar9 = extraout_EDX_05;
    }
    if (((uint)control[1].nextSibling & 0x800000) != 0) {
      uVar14 = SelectionOverlay_DrawMarkerAEForVisibleProjectedGridVertices
                         (sVar9,sVar6,iVar11,sVar10,(int)control[1].parent);
      sVar9 = (sdword)((ulonglong)uVar14 >> 0x20);
      sVar6 = extraout_ECX_06;
    }
    if (((uint)control[1].nextSibling & 0x1000000) != 0) {
      uVar14 = SelectionOverlay_DrawMarkerAFB0ForProjectedVertexStateFlags
                         (sVar9,sVar6,iVar11,sVar10,(int)control[1].parent);
      sVar9 = (sdword)((ulonglong)uVar14 >> 0x20);
      sVar6 = extraout_ECX_07;
    }
    if (((uint)control[1].nextSibling & 0x2000000) != 0) {
      uVar14 = SelectionOverlay_DrawMarkerB1B2ForProjectedVertexMask1800
                         (sVar9,sVar6,iVar11,sVar10,(byte)control[2].bottom,(int)control[1].parent);
      sVar9 = (sdword)((ulonglong)uVar14 >> 0x20);
      sVar6 = extraout_ECX_08;
    }
    if (((((uint)control[1].nextSibling & 0x4000) != 0) && (control[1].parent != (UiNodeBase *)0x0))
       && ((g_UiCommandRuntimeFlags & 0x40) != 0)) {
      SelectionOverlay_DrawMarkerAFForProjectedVertexFlag8000
                (sVar9,sVar6,iVar11,sVar10,(int)control[1].parent);
    }
  }
  g_SelectionPanelBlitOpaque = g_GraphicsTextureSourceBlitSourceAlpha;
  g_SelectionPanelBlitClipped = g_GraphicsTextureSourceBlitTiledSourceAlpha;
  (*g_SpinLockReleaseAndInvoke)
            ((SpinLockReleaseCallbackProc *)control[2].bottomAnchorQ31,
             (RuntimeSpinLockValue *)control[2].rightAnchorQ31);
  if ((control[2].nodeFlags != 0) && ((int)control[3].vtable < (int)control[3].nextSibling)) {
    control[2].nodeFlags = 0;
  }
  UiContainer_DrawIntersectingChildren(clipTop,clipLeft,clipBottom,clipRight,control);
  return;
}

/* Address: 0x0050C6E0.
   Ownership: ui/frontend/runtime.
   Purpose: Exact packed function-table or callback-registration provenance plus immutable body topology prove this
   callable entry.
*/
void FrontendModelPointerContext_RightPress
               (UiPointerWheelDelta wheelDelta,UiPixelCoordinate pointerY,UiPixelCoordinate pointerX
               ,FrontendModelPointerContextRuntimeState118 *callbackContext)

{
  *(UiPixelCoordinate *)(callbackContext->reserved6C_D7 + 0x34) = pointerX;
  *(UiPixelCoordinate *)(callbackContext->reserved6C_D7 + 0x38) = pointerY;
  *(UiPointerWheelDelta *)(callbackContext->reserved6C_D7 + 0x3c) = wheelDelta;
  callbackContext->contextFlags =
       callbackContext->contextFlags |
       FRONTEND_MODEL_POINTER_CONTEXT_ROUTE_TO_BUILTIN_ACTION_RESOLUTION;
  callbackContext[1].base.firstChild = (UiNodeBase *)0x0;
  g_CursorUseOverridePosition = g_CursorUseOverridePosition + 1;
  return;
}

/* Address: 0x0050C730.
   Ownership: ui/frontend/runtime.
   Purpose: Exact packed function-table or callback-registration provenance plus immutable body topology prove this
   callable entry.
*/
void FrontendModelPointerContext_RightRelease
               (UiPointerWheelDelta wheelDelta,UiPixelCoordinate pointerY,UiPixelCoordinate pointerX
               ,FrontendModelPointerContextRuntimeState118 *callbackContext)

{
  UiNodeBase *pUVar1;
  UiNodeBase *pUVar2;
  
  g_CursorUseOverridePosition = 0;
  pUVar1 = callbackContext[1].base.firstChild;
  pUVar2 = callbackContext[1].base.nextSibling;
  callbackContext->contextFlags = callbackContext->contextFlags & 0xffffffb0;
  if ((pUVar1 < (UiNodeBase *)0x7) && (pUVar2 != (UiNodeBase *)0x0)) {
    (*(code *)pUVar2)(callbackContext);
  }
  return;
}

/* Address: 0x0050CC80.
   Ownership: ui/frontend/runtime.
   Purpose: Exact packed function-table or callback-registration provenance plus immutable body topology prove this
   callable entry.
   Cross-module calls: WorldMotion_AdjustHeadingAndRecomputePosition [world/motion/runtime],
   WorldMotion_AdjustPitchClampAndRecomputePosition [world/motion/runtime],
   WorldMotion_AdjustDistanceClampAndRecomputePosition [world/motion/runtime],
   WorldMotion_TranslateCurrentAndTargetByInputElevationAndHeadingQuarterTurn [world/motion/runtime],
   WorldMotion_TranslateCurrentAndTargetByNegatedPitchReverseHeading [world/motion/runtime],
   WorldMotion_AdjustHeadingAndClearFieldGridDirty [world/motion/runtime].
*/
void FrontendModelPointerContext_RightDrag
               (UiPointerWheelDelta wheelDelta,UiPixelCoordinate pointerY,UiPixelCoordinate pointerX
               ,WorldRuntimeContext *callbackContext)

{
  uint uVar1;
  code *extraout_EAX;
  dword screenDeltaY;
  AngleTurn32 elevationAngle;
  AngleTurn32 extraout_EDX;
  AngleTurn32 extraout_EDX_00;
  int pitchDeltaInput;
  int screenDelta;
  int pitchDeltaInput_00;
  
  screenDeltaY = pointerX - *(int *)callbackContext->reservedA0_AB;
  elevationAngle = pointerY - *(int *)(callbackContext->reservedA0_AB + 4);
  if ((callbackContext->runtimeFlags & 0x10) != 0) {
    return;
  }
  if ((callbackContext->runtimeFlags & 0x100) != 0) {
    if ((g_CursorButtonState & 1) == 0) {
      callbackContext->runtimeFlags = callbackContext->runtimeFlags & 0xfffffffa;
      callbackContext->runtimeFlags = callbackContext->runtimeFlags | 10;
      WorldMotion_AdjustHeadingAndRecomputePosition(screenDeltaY,callbackContext);
      WorldMotion_AdjustPitchClampAndRecomputePosition(pitchDeltaInput,callbackContext);
    }
    else {
      callbackContext->runtimeFlags = callbackContext->runtimeFlags & 0xfffffff4;
      callbackContext->runtimeFlags = callbackContext->runtimeFlags | 4;
      WorldMotion_AdjustDistanceClampAndRecomputePosition(elevationAngle,callbackContext);
    }
    goto LAB_0050ce98;
  }
  if ((callbackContext->runtimeFlags & 0x8000) != 0) {
    if ((g_CursorButtonState & 1) == 0) {
      if ((g_KeyboardStateMask & 0xc) == 0) {
        callbackContext->runtimeFlags = callbackContext->runtimeFlags & 0xfffffff1;
        callbackContext->runtimeFlags = callbackContext->runtimeFlags | 1;
        WorldMotion_TranslateCurrentAndTargetByInputElevationAndHeadingQuarterTurn
                  (screenDeltaY,elevationAngle,screenDeltaY,callbackContext);
        WorldMotion_TranslateCurrentAndTargetByNegatedPitchReverseHeading
                  (screenDelta,callbackContext);
      }
      else {
        callbackContext->runtimeFlags = callbackContext->runtimeFlags & 0xfffffffa;
        callbackContext->runtimeFlags = callbackContext->runtimeFlags | 10;
        WorldMotion_AdjustHeadingAndClearFieldGridDirty(screenDeltaY,callbackContext);
        WorldMotion_AdjustPitchClampAndClearFieldGridDirty(pitchDeltaInput_00,callbackContext);
      }
    }
    else if ((g_KeyboardStateMask & 0xc) == 0) {
      callbackContext->runtimeFlags = callbackContext->runtimeFlags & 0xfffffff1;
      callbackContext->runtimeFlags = callbackContext->runtimeFlags | 1;
      WorldMotion_TranslateCurrentAndTargetByPitchQuarterTurn(elevationAngle,callbackContext);
    }
    else {
      callbackContext->runtimeFlags = callbackContext->runtimeFlags & 0xfffffff4;
      callbackContext->runtimeFlags = callbackContext->runtimeFlags | 4;
      WorldMotion_AdjustPositionMagnitudeClamp(elevationAngle,callbackContext);
    }
    goto LAB_0050ce98;
  }
  if ((callbackContext->runtimeFlags & 0x200) == 0) {
    return;
  }
  if ((g_KeyboardStateMask & 0xc) == 0) {
    if ((g_KeyboardStateMask & 0x30) == 0) {
      if ((g_KeyboardStateMask & 3) != 0) {
        callbackContext->runtimeFlags = callbackContext->runtimeFlags & 0xfffffff2;
        callbackContext->runtimeFlags = callbackContext->runtimeFlags | 2;
        WorldMotion_AdjustHeadingAndRecomputePosition(screenDeltaY,callbackContext);
        goto LAB_0050ce98;
      }
      if ((callbackContext->runtimeFlags & 0x4000000) == 0) {
        if ((g_CursorButtonState & 1) == 0) {
          callbackContext->runtimeFlags = callbackContext->runtimeFlags & 0xfffffff1;
          callbackContext->runtimeFlags = callbackContext->runtimeFlags | 1;
          WorldRuntime_TranslateCameraByScreenDelta(elevationAngle,screenDeltaY,callbackContext);
          WorldRuntime_RecomputeMotionEndpointAgainstFieldSurface(callbackContext);
          goto LAB_0050ce98;
        }
        callbackContext->runtimeFlags = callbackContext->runtimeFlags & 0xfffffff2;
        callbackContext->runtimeFlags = callbackContext->runtimeFlags | 2;
        WorldMotion_AdjustHeadingAndRecomputePosition(screenDeltaY,callbackContext);
        elevationAngle = extraout_EDX;
        if ((callbackContext->runtimeFlags & 0x40000000) == 0) {
          uVar1 = callbackContext->runtimeFlags;
joined_r0x0050cd8b:
          if ((uVar1 & 0x80000000) == 0) goto LAB_0050ce98;
          goto LAB_0050cdd9;
        }
      }
      else if ((g_CursorButtonState & 1) == 0) {
        callbackContext->runtimeFlags = callbackContext->runtimeFlags & 0xfffffff2;
        callbackContext->runtimeFlags = callbackContext->runtimeFlags | 2;
        WorldMotion_AdjustHeadingAndRecomputePosition(screenDeltaY,callbackContext);
        elevationAngle = extraout_EDX_00;
        if ((callbackContext->runtimeFlags & 0x40000000) == 0) {
          uVar1 = callbackContext->runtimeFlags;
          goto joined_r0x0050cd8b;
        }
      }
      else {
        callbackContext->runtimeFlags = callbackContext->runtimeFlags & 0xfffffff0;
        if ((callbackContext->runtimeFlags & 0x40000000) != 0) goto LAB_0050cdd9;
        if ((callbackContext->runtimeFlags & 0x80000000) == 0) goto LAB_0050ce98;
      }
    }
    else {
      callbackContext->runtimeFlags = callbackContext->runtimeFlags & 0xfffffff4;
    }
    callbackContext->runtimeFlags = callbackContext->runtimeFlags | 4;
    WorldMotion_AdjustDistanceClampAndRecomputePosition(elevationAngle,callbackContext);
  }
  else {
    callbackContext->runtimeFlags = callbackContext->runtimeFlags & 0xfffffff8;
LAB_0050cdd9:
    callbackContext->runtimeFlags = callbackContext->runtimeFlags | 8;
    WorldMotion_AdjustPitchClampAndRecomputePosition(elevationAngle,callbackContext);
  }
LAB_0050ce98:
  (*g_PointerSetPosition)
            (*(sdword *)(callbackContext->reservedA0_AB + 4),
             *(sdword *)callbackContext->reservedA0_AB);
  WorldRuntime_CaptureMotionStateToSnapshot(callbackContext);
  if (extraout_EAX != (code *)0x0) {
    (*extraout_EAX)(callbackContext);
  }
  return;
}

/* Address: 0x0050CED0.
   Ownership: ui/frontend/runtime.
   Purpose: Exact packed function-table or callback-registration provenance plus immutable body topology prove this
   callable entry.
   Cross-module calls: WorldMotion_AdjustDistanceClampAndRecomputePosition [world/motion/runtime],
   WorldRuntime_CaptureMotionStateToSnapshot [world/runtime/core], WorldMotion_AdjustPitchClampAndRecomputePosition
   [world/motion/runtime].
*/
void FrontendModelPointerContext_PointerWheel
               (UiPointerWheelDelta wheelDelta,UiPixelCoordinate pointerY,UiPixelCoordinate pointerX
               ,WorldRuntimeContext *callbackContext)

{
  int iVar1;
  
  if (((callbackContext->runtimeFlags & 0x10) == 0) &&
     ((callbackContext->runtimeFlags & 0x8300) != 0)) {
    if ((g_KeyboardStateMask & 0xc) == 0) {
      iVar1 = wheelDelta * DAT_0050baf6;
      callbackContext->runtimeFlags = callbackContext->runtimeFlags & 0xfffffff4;
      callbackContext->runtimeFlags = callbackContext->runtimeFlags | 4;
      WorldMotion_AdjustDistanceClampAndRecomputePosition(iVar1,callbackContext);
      WorldRuntime_CaptureMotionStateToSnapshot(callbackContext);
    }
    else {
      iVar1 = wheelDelta * DAT_0050baf6;
      callbackContext->runtimeFlags = callbackContext->runtimeFlags & 0xfffffff8;
      callbackContext->runtimeFlags = callbackContext->runtimeFlags | 8;
      WorldMotion_AdjustPitchClampAndRecomputePosition(iVar1,callbackContext);
      WorldRuntime_CaptureMotionStateToSnapshot(callbackContext);
    }
  }
  return;
}

/* Address: 0x0050CF50.
   Ownership: ui/frontend/runtime.
   Purpose: Exact packed function-table or callback-registration provenance plus immutable body topology prove this
   callable entry.
   Cross-module calls: UiNode_DefaultKeyboardEventMoveFocusNextCf [ui/controls/input].
*/
void FrontendModelPointerContext_KeyboardEventCf
               (UiKeyboardStateMask keyboardStateMask,UiKeyboardEventCode keyCode,
               UiNodeBase *control)

{
  UiKeyboardEventCode extraout_ECX;
  UiKeyboardStateMask extraout_EDX;
  bool bVar1;
  
  bVar1 = false;
  if (((code *)control[3].bottom != (code *)0x0) &&
     ((*(code *)control[3].bottom)(keyboardStateMask,keyCode,control), keyCode = extraout_ECX,
     keyboardStateMask = extraout_EDX, !bVar1)) {
    return;
  }
  UiNode_DefaultKeyboardEventMoveFocusNextCf(keyboardStateMask,keyCode,control);
  return;
}

/* Address: 0x0050CF90.
   Ownership: ui/frontend/runtime.
   Purpose: Exact packed function-table or callback-registration provenance plus immutable body topology prove this
   callable entry.
   Cross-module calls: FixedMath_DirectionFromAnglesScaledRegs [core/math/fixed],
   WorldRuntime_ClearFieldGridDirtyFlag [world/runtime/core].
*/
void FrontendModelPointerContext_Tick(WorldRuntimeContext *callbackContext)

{
  dword *pdVar1;
  UQ12 UVar2;
  int iVar3;
  UQ12 UVar4;
  int extraout_ECX;
  FixedDirectionXZEdxEax8 FVar5;
  
  if ((callbackContext->runtimeFlags & 0x40) != 0) {
    pdVar1 = &(callbackContext->selection).reservedCallbackState40;
    *pdVar1 = *pdVar1 + 1;
  }
  if ((callbackContext->runtimeFlags & 0x48110) == 0) {
    UVar4 = (callbackContext->motion).committedDistanceQ12;
    if ((int)UVar4 < (int)callbackContext->reserved98) {
      UVar4 = callbackContext->reserved98;
    }
    if ((int)callbackContext->surfaceSelectionFlags < (int)UVar4) {
      UVar4 = callbackContext->surfaceSelectionFlags;
    }
    UVar2 = (callbackContext->motion).targetDistanceQ12;
    iVar3 = DAT_0050bade;
    if ((int)(UVar4 * 0xf) >> 4 <= (int)UVar2) {
      if ((int)UVar2 <= (int)(UVar4 * 0x11) >> 4) {
        return;
      }
      iVar3 = -DAT_0050bade;
    }
    (callbackContext->motion).targetDistanceQ12 = UVar2 + iVar3;
    FVar5 = FixedMath_DirectionFromAnglesScaledRegs
                      (-(callbackContext->motion).pitchAngle,
                       (callbackContext->motion).headingAngle ^ 0x8000,UVar2 + iVar3);
    (callbackContext->motion).positionXQ12 =
         (int)FVar5 + (callbackContext->motion).targetPositionXQ12;
    (callbackContext->motion).positionYQ12 =
         extraout_ECX + (callbackContext->motion).targetPositionYQ12;
    (callbackContext->motion).positionZQ12 =
         (int)(FVar5 >> 0x20) + (callbackContext->motion).targetPositionZQ12;
    WorldRuntime_ClearFieldGridDirtyFlag(callbackContext);
  }
  return;
}

/* Address: 0x00514E40.
   Ownership: ui/frontend/runtime.
   Purpose: Copies the selected faction's Q4 resource and progress values into the frontend runtime cache and
   formats the primary amount into the verified UTF-16 display buffer.
*/
void FrontendRuntime_UpdateCurrentFactionMetricCache(void)

{
  XeniteAmountQ4 XVar1;
  TritiumAmountQ4 TVar2;
  int arg4;
  FactionProgressAmountQ4 progressCurrentQ4;
  FactionProgressAmountQ4 progressLimitQ4;
  FactionArmyContributionValue activeArmyScaleValue;
  InGameRuntimeRootImageC3E4 *runtimeRoot;
  int activeFactionIndex;
  
  runtimeRoot = g_InGameRuntimeRoot;
  activeFactionIndex = (g_InGameRuntimeRoot->worldRuntime0A30).activeFactionRuntimeIndex;
  XVar1 = g_GameFactionRuntimeImage.records[activeFactionIndex].xeniteStorageLimitQ4;
  arg4 = (int)g_GameFactionRuntimeImage.records[activeFactionIndex].xeniteCurrentQ4 >> 4;
  g_InGameRuntimeRoot->primaryResourceDisplayCurrent49B4 = arg4;
  runtimeRoot->primaryResourceDisplayLimit49B8 = (int)XVar1 >> 4;
  (*g_WideNumberFormatUtf16)
            (WIDE_FORMAT_WRITE_TERMINATOR,0,10,1,arg4,
             (word *)&g_FrontendCurrentFactionPrimaryResourceTextUtf16);
  TVar2 = g_GameFactionRuntimeImage.records[activeFactionIndex].tritiumStorageLimitQ4;
  progressCurrentQ4 = g_GameFactionRuntimeImage.records[activeFactionIndex].baselineEnergySupplyQ4;
  runtimeRoot->secondaryResourceDisplayCurrent4A4C =
       (int)g_GameFactionRuntimeImage.records[activeFactionIndex].tritiumCurrentQ4 >> 4;
  runtimeRoot->secondaryResourceDisplayLimit4A50 = (int)TVar2 >> 4;
  progressLimitQ4 = g_GameFactionRuntimeImage.records[activeFactionIndex].energyGenerationCapacityQ4
  ;
  activeArmyScaleValue =
       g_GameFactionRuntimeImage.records[activeFactionIndex].tritiumExtractionRateQ4PerTick;
  runtimeRoot->transientContributionDisplay4AE4 =
       (int)(g_GameFactionRuntimeImage.records[activeFactionIndex].suppliedEnergyDemandQ4 +
            g_GameFactionRuntimeImage.records[activeFactionIndex].unpoweredEnergyDemandQ4) >> 4;
  runtimeRoot->progressLimitDisplay4AE8 = (int)progressLimitQ4 >> 4;
  runtimeRoot->combinedProgressOrArmyScaleDisplay4B28 =
       ((int)progressCurrentQ4 >> 4) + activeArmyScaleValue;
  return;
}

/* Address: 0x00547620.
   Ownership: ui/frontend/runtime.
   Purpose: Periodic frontend timer callback. Decrements the shared countdown only when it is nonzero and otherwise
   leaves it unchanged.
*/
void __cdecl FrontendRuntime_TimerCountdownTick(void)

{
  if (g_FrontendTimerCountdownTicks != 0) {
    g_FrontendTimerCountdownTicks = g_FrontendTimerCountdownTicks + -1;
  }
  return;
}

/* Address: 0x00547FB0.
   Ownership: ui/frontend/runtime.
   Purpose: Frontend periodic timer callback. It increments the active tick counter only while the verified
   frontend-active flag is nonzero.
*/
void __cdecl FrontendRuntime_IncrementActiveTickCounter(void)

{
  if (g_FrontendRomTransitionPendingCount != 0) {
    g_FrontendRomTransitionElapsedTicks = g_FrontendRomTransitionElapsedTicks + 1;
  }
  return;
}

/* Address: 0x00548030.
   Ownership: ui/frontend/runtime.
   Purpose: Frontend command dispatcher selected by command code and modifier flags. Typed parameters: p0
   modifierFlags→UiKeyboardStateMask_V297, p1 commandCode→UiActionId_V338. Nearby but non-identical semantic
   domains were explicitly deferred. Calling convention, parameter storage, body bytes, control flow, globals,
   locals, and executable data remain unchanged.
*/
void FrontendRuntime_DispatchCommandByCodeAndModifierFlagsCf
               (UiKeyboardStateMask modifierFlags,UiActionId commandCode,void *frontendRuntime)

{
  UiCommandDispatchRecord *dispatchRecordCursor;
  uint recordModifierFlags;
  UiCommandDispatchRecord *currentDispatchRecord;
  
  dispatchRecordCursor = g_FrontendCommandDispatchRecords_00_Code00030071_Modifier30;

  FrontendRuntime_DispatchCommandByCodeAndModifierFlagsCf_ScanNextDispatchRecordForCodeAndModifierMatch
  :
  while( true ) {
    do {
      currentDispatchRecord = dispatchRecordCursor;
      recordModifierFlags = currentDispatchRecord->modifierClassFlags;
      if (currentDispatchRecord->commandCode == 0) {
        return;
      }
      dispatchRecordCursor = currentDispatchRecord + 1;
    } while (currentDispatchRecord->commandCode != commandCode);
    if (recordModifierFlags != 0) break;
    if ((modifierFlags & 0x3f) == 0) {
      (*(code *)currentDispatchRecord->continuationEntryAddress)();
      return;
    }
  }
  if ((recordModifierFlags & 3) == 0) {
    if ((modifierFlags & 3) != 0)
    goto 
    FrontendRuntime_DispatchCommandByCodeAndModifierFlagsCf_ScanNextDispatchRecordForCodeAndModifierMatch
    ;
  }
  else if ((modifierFlags & 3) == 0)
  goto 
  FrontendRuntime_DispatchCommandByCodeAndModifierFlagsCf_ScanNextDispatchRecordForCodeAndModifierMatch
  ;
  if ((recordModifierFlags & 0x30) == 0) {
    if (((modifierFlags & 0xc) != 0) && ((modifierFlags & 0x30) == 0)) {
      (*(code *)currentDispatchRecord->continuationEntryAddress)();
      return;
    }
  }
  else if ((recordModifierFlags & 0xc) == 0) {
    if (((modifierFlags & 0xc) == 0) && ((modifierFlags & 0x30) != 0)) {
      (*(code *)currentDispatchRecord->continuationEntryAddress)();
      return;
    }
  }
  else if (((modifierFlags & 0xc) != 0) && ((modifierFlags & 0x30) != 0)) {
    (*(code *)currentDispatchRecord->continuationEntryAddress)();
    return;
  }
  goto 
  FrontendRuntime_DispatchCommandByCodeAndModifierFlagsCf_ScanNextDispatchRecordForCodeAndModifierMatch
  ;
}

/* Address: 0x00548700.
   Ownership: ui/frontend/runtime.
   Purpose: EAX is preserved. Typed parameters: p0 stateCode→FrontendStatusCode_V306. Nearby but non-identical
   semantic domains were explicitly deferred. Calling convention, parameter storage, body bytes, control flow,
   globals, locals, and executable data remain unchanged.
   Cross-module calls: FrontendRomActionTable_ExecuteRecord [assets/rom/runtime].
*/
undefined4 FrontendState_DispatchCode(FrontendStatusCode stateCode)

{
  undefined4 in_EAX;
  
  FrontendRomActionTable_ExecuteRecord(0,0,0,stateCode);
  return in_EAX;
}

/* Address: 0x00548910.
   Ownership: ui/frontend/runtime.
   Purpose: Frontend pointer callback that updates pointer context and scene-view state and returns the resolved
   action value. [VERSIONLESS_CANONICAL_DATATYPE_CLOSURE] Retired detached enum dictionary
   FrontendPointerContextFlags after transferring its complete value vocabulary to code annotation. It is not a
   safe whole-value storage type. Values: 16=FRONTEND_POINTER_CONTEXT_SUPPRESS_BUILTIN_ACTION_RESOLUTION,
   32=FRONTEND_POINTER_CONTEXT_ROUTE_TO_SECONDARY_CALLBACK,
   64=FRONTEND_POINTER_CONTEXT_ROUTE_TO_BUILTIN_ACTION_RESOLUTION,
   256=FRONTEND_POINTER_CONTEXT_OBSERVED_ACTION_BRANCH_00000100,
   512=FRONTEND_POINTER_CONTEXT_OBSERVED_ACTION_BRANCH_00000200,
   4096=FRONTEND_POINTER_CONTEXT_COMPARE_HITS_BY_METRIC_ONLY,
   32768=FRONTEND_POINTER_CONTEXT_OBSERVED_ACTION_BRANCH_00008000,
   4194304=FRONTEND_POINTER_CONTEXT_ALLOW_CANDIDATE_WITHOUT_NODE_FLAG_20,
   67108864=FRONTEND_POINTER_CONTEXT_OBSERVED_BUTTON_BRANCH_04000000,
   1073741824=FRONTEND_POINTER_CONTEXT_OBSERVED_CODE_OVERRIDE_40000000,
   2147483648=FRONTEND_POINTER_CONTEXT_OBSERVED_CODE_OVERRIDE_80000000
   Cross-module calls: UiPageStack_ActivePageNotInListCf [ui/controls/lists], RomRegistry_FindRecordBySlotValue
   [assets/rom/runtime], RomRecordTable_FindRecordById [assets/rom/runtime],
   WorldMotionSpline_BuildSixChannelCurves [core/math/interpolation], TextResource_Resolve [assets/text/resources],
   RichTextCommandStream_MeasureRegs [assets/text/richtext].
*/
dword FrontendRuntime_UpdatePointerContextAndSceneViewCf
                (dword pointerValue0,dword pointerValue1,dword pointerValue2,dword pointerValue3,
                void *pointedRecord,void *frontendRuntime)

{
  int extraout_EAX;
  RomAssetRecordPrefix *pRVar1;
  int *piVar2;
  dword dVar3;
  word *commandStream;
  RomRecordId recordId;
  word *extraout_ECX;
  int extraout_EDX;
  int iVar4;
  int iVar5;
  int iVar6;
  RichTextExtentRegs RVar7;
  qword qVar8;
  
  if (((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_CLIENT) != SESSION_NETWORK_ROLE_LOCAL) ||
     (UiPageStack_ActivePageNotInListCf((UiPageStackControl *)((int)frontendRuntime + 0x1a0)),
     extraout_EAX != 0)) {
    dVar3 = 0;
    iVar5 = 0;
    goto FrontendPointerContext_RefreshHintText;
  }
  pRVar1 = RomRegistry_FindRecordBySlotValue((RomRegistrySlotValue)pointedRecord);
  recordId = 0xf0000000;
  if (pRVar1 != (RomAssetRecordPrefix *)0x0) {
    recordId = pRVar1->recordId;
  }
  piVar2 = RomRecordTable_FindRecordById(recordId,g_FrontendActiveRomRecordTable);
  dVar3 = 0;
  iVar5 = extraout_EDX;
  if (((piVar2 == (int *)0x0) ||
      (((((dVar3 = 0, piVar2[8] == 3 || (piVar2[8] == 4)) || (piVar2[8] == 9)) || (piVar2[8] < 0))
       && ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_NETWORKED_MASK) !=
           SESSION_NETWORK_ROLE_LOCAL)))) ||
     ((piVar2[8] == 2 && (g_NetworkBackendInstanceCount == 0))))
  goto FrontendPointerContext_RefreshHintText;
  if ((*piVar2 == g_FrontendRomTransitionKeyframe1Channel0Q12) &&
     ((piVar2[1] == g_FrontendRomTransitionKeyframe1Channel1Q12 &&
      (piVar2[2] == g_FrontendRomTransitionKeyframe1Channel2Q12)))) {
    iVar5 = piVar2[3];
    iVar6 = piVar2[4];
    iVar4 = piVar2[5];
    if (((iVar5 != g_FrontendRomTransitionKeyframe1Channel3Q12) ||
        (iVar6 != g_FrontendRomTransitionKeyframe1Channel4Q12)) ||
       (iVar4 != g_FrontendRomTransitionKeyframe1Channel5Q12))
    goto FrontendRomTransition_RebuildSpline;
  }
  else {
    iVar5 = piVar2[3];
    iVar6 = piVar2[4];
    iVar4 = piVar2[5];
    g_FrontendRomTransitionKeyframe1Channel0Q12 = *piVar2;
    g_FrontendRomTransitionKeyframe1Channel1Q12 = piVar2[1];
    g_FrontendRomTransitionKeyframe1Channel2Q12 = piVar2[2];
FrontendRomTransition_RebuildSpline:
    g_FrontendRomTransitionKeyframe0Channel0Q12 = *(undefined4 *)((int)frontendRuntime + 0x60);
    g_FrontendRomTransitionKeyframe0Channel1Q12 = *(undefined4 *)((int)frontendRuntime + 100);
    g_FrontendRomTransitionKeyframe0Channel2Q12 = *(undefined4 *)((int)frontendRuntime + 0x68);
    g_FrontendRomTransitionKeyframe0Channel3Q12 = *(undefined4 *)((int)frontendRuntime + 0x6c);
    g_FrontendRomTransitionKeyframe0Channel4Q12 = *(undefined4 *)((int)frontendRuntime + 0x70);
    g_FrontendRomTransitionKeyframe0Channel5Q12 = *(undefined4 *)((int)frontendRuntime + 0x74);
    g_FrontendRomTransitionKeyframe0TimeQ12 = 0;
    g_FrontendRomTransitionKeyframe1TimeQ12 = 0xc0;
    g_FrontendRomTransitionElapsedTicks = 0;
    g_FrontendRomTransitionPendingCount = 0xffffffff;
    g_FrontendRomTransitionSplineKeyframeCount = 2;
    g_FrontendRomTransitionSplineKeyframes = &g_FrontendRomTransitionKeyframe0Channel0Q12;
    g_FrontendRomTransitionKeyframe1Channel3Q12 = iVar5;
    g_FrontendRomTransitionKeyframe1Channel4Q12 = iVar6;
    g_FrontendRomTransitionKeyframe1Channel5Q12 = iVar4;
    WorldMotionSpline_BuildSixChannelCurves
              (2,(WorldMotionSplineKeyframe *)&g_FrontendRomTransitionKeyframe0Channel0Q12);
  }
  iVar5 = piVar2[6];
  dVar3 = 7;
FrontendPointerContext_RefreshHintText:
  if (iVar5 == 0) {
    if (g_FrontendPendingPageActionDepth == 0) {
      *(undefined4 *)((int)frontendRuntime + 0x43e0) = 0;
      *(undefined4 *)((int)frontendRuntime + 0x43e4) = 0;
      return dVar3;
    }
    iVar5 = 1;
  }
  commandStream = TextResource_Resolve(iVar5 + 0x2000);
  if (commandStream != extraout_ECX) {
    *(word **)((int)frontendRuntime + 0x43e4) = commandStream;
    RVar7 = RichTextCommandStream_MeasureRegs(g_UiTextStyleNormal,commandStream);
    iVar5 = (int)(RVar7.widthPixels + 1) >> 1;
    iVar6 = (int)(RVar7.heightPixels + 1) >> 1;
    *(int *)((int)frontendRuntime + 0x43b0) = iVar5;
    *(int *)((int)frontendRuntime + 0x43b4) = iVar6;
    *(int *)((int)frontendRuntime + 0x43a8) = -iVar5;
    *(int *)((int)frontendRuntime + 0x43ac) = -iVar6;
    qVar8 = (*g_GraphicsTextureSourceGetLogicalSize)(0x72,g_UiWindowTextureSource);
    iVar4 = (int)(qVar8 >> 0x20);
    iVar6 = (int)qVar8 + 3;
    *(undefined4 *)((int)frontendRuntime + 0x43e0) = 1;
    *(int *)((int)frontendRuntime + 0x43b0) = *(int *)((int)frontendRuntime + 0x43b0) + iVar6;
    *(int *)((int)frontendRuntime + 0x43b4) = *(int *)((int)frontendRuntime + 0x43b4) + iVar4;
    iVar5 = *(int *)(*(int *)((int)frontendRuntime + 0x4390) + 0xc);
    *(int *)((int)frontendRuntime + 0x43a8) = *(int *)((int)frontendRuntime + 0x43a8) - iVar6;
    *(int *)((int)frontendRuntime + 0x43ac) = *(int *)((int)frontendRuntime + 0x43ac) - iVar4;
    (**(code **)(iVar5 + 0xc))(*(int *)((int)frontendRuntime + 0x4390));
  }
  return dVar3;
}

/* Address: 0x00548BE0.
   Ownership: ui/frontend/runtime.
   Purpose: Six-argument frontend runtime callback installed at object slot +0x5C. It performs no operation and
   returns with ret 0x18.
*/
void FrontendRuntimeCallback5C_NoOp
               (dword argument1,dword argument2,dword argument3,dword argument4,dword argument5,
               dword argument6)

{
  return;
}

/* Address: 0x00548BF0.
   Ownership: ui/frontend/runtime.
   Purpose: Six-argument frontend runtime callback installed at object slot +0x60. It performs no operation and
   returns with ret 0x18.
*/
void FrontendRuntimeCallback60_NoOp
               (dword argument1,dword argument2,dword argument3,dword argument4,dword argument5,
               dword argument6)

{
  return;
}

/* Address: 0x00548C00.
   Ownership: ui/frontend/runtime.
   Purpose: Six-argument frontend runtime callback installed at object slot +0x64. When frontend mode bit 0 is
   clear, it resolves the fifth argument through the runtime record tables, maps the record identifier, and
   dispatches the nonnegative result either through message 0x1350 or the local frontend service. Register results
   remain preserved. Typed parameters: p4 argument5→FrontendCallbackArgument5_V344. Calling convention, complete
   VariableStorage serialization, function bytes, control flow, globals, locals, and executable data remain
   unchanged.
   Cross-module calls: RomRegistry_FindRecordBySlotValue [assets/rom/runtime], RomRecordTable_FindIndexById
   [assets/rom/runtime], FrontendRomActionTable_ExecuteRecord [assets/rom/runtime],
   FrontendCommandQueue_EnqueueLocalPlayerCommand [network/protocol/commands].
*/
undefined4
FrontendRuntimeCallback64_DispatchRecord1350
          (dword argument1,dword argument2,dword argument3,dword argument4,
          FrontendCallbackArgument5 argument5,dword argument6)

{
  undefined4 in_EAX;
  RomAssetRecordPrefix *pRVar1;
  CommandPayloadDword04 recordIndex;
  undefined8 uVar2;
  
  if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_CLIENT) == SESSION_NETWORK_ROLE_LOCAL) {
    pRVar1 = RomRegistry_FindRecordBySlotValue(argument5);
    if (pRVar1 != (RomAssetRecordPrefix *)0x0) {
      uVar2 = RomRecordTable_FindIndexById(pRVar1->recordId,g_FrontendActiveRomRecordTable);
      recordIndex = (CommandPayloadDword04)uVar2;
      if (-1 < (int)recordIndex) {
        if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_NETWORKED_MASK) ==
            SESSION_NETWORK_ROLE_LOCAL) {
          FrontendRomActionTable_ExecuteRecord(g_LocalPlayerRuntimeId,0,0,recordIndex);
        }
        else {
          FrontendCommandQueue_EnqueueLocalPlayerCommand(0x1350,0,0,recordIndex);
        }
      }
    }
  }
  return in_EAX;
}

/* Address: 0x00548C70.
   Ownership: ui/frontend/runtime.
   Purpose: One-argument frontend runtime callback installed at object slot +0x68. When frontend mode bit 0 is
   clear, it dispatches a refresh through message 0x1340 or the local frontend service according to the remaining
   mode bits.
   Cross-module calls: ScenarioCatalog_RequestRomTransitionStopCallback [assets/scenario/catalog],
   FrontendCommandQueue_EnqueueLocalPlayerCommand [network/protocol/commands].
*/
void FrontendRuntimeCallback68_DispatchRefresh1340(dword callbackArgument)

{
  if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_CLIENT) == SESSION_NETWORK_ROLE_LOCAL) {
    if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_NETWORKED_MASK) ==
        SESSION_NETWORK_ROLE_LOCAL) {
      ScenarioCatalog_RequestRomTransitionStopCallback(g_LocalPlayerRuntimeId,0,0,0);
    }
    else {
      FrontendCommandQueue_EnqueueLocalPlayerCommand(0x1340,0,0,0);
    }
  }
  return;
}

/* Address: 0x00548CB0.
   Ownership: ui/frontend/runtime.
   Purpose: Takes a UTF-16 text pointer in EAX, inserts it into the shared recent-text history, and rebuilds the
   frontend five-entry pointer list at activeFrontendState+0x350. EAX is preserved.
   Cross-module calls: RecentTextHistory_Insert [ui/support/runtime], RecentTextHistory_SortAndBuildPointerList
   [ui/support/runtime].
*/
word * __fastcall FrontendRecentTextHistory_InsertAndRebuild5(void)

{
  word *in_EAX;
  RecentTextHistoryPointerList *output;
  
  output = (RecentTextHistoryPointerList *)(g_FrontendRootNode + 0x350);
  RecentTextHistory_Insert(in_EAX);
  RecentTextHistory_SortAndBuildPointerList(5,output);
  return in_EAX;
}

/* Address: 0x00549100.
   Ownership: ui/frontend/runtime.
   Purpose: One-argument frontend action callback. Local mode invokes
   FrontendSession_ApplyGameSpeedAndReturnToMainPage with zero state; network modes dispatch message offset 0x2C0.
   Existing register results are preserved. Queued UI action handler for FRONTEND_PAGE20[67] (0x2043). Return
   datatype is preserved for non-queue direct callers.
   Cross-module calls: FrontendSession_ApplyGameSpeedAndReturnToMainPage [ui/frontend/session],
   FrontendCommandQueue_EnqueueLocalPlayerCommand [network/protocol/commands].
*/
undefined4 FrontendCallback_ApplyGameSpeedOrDispatch02C0(dword callbackArgument)

{
  undefined4 extraout_EAX;
  undefined4 callbackDispatchResult;
  
  if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_NETWORKED_MASK) ==
      SESSION_NETWORK_ROLE_LOCAL) {
    callbackDispatchResult =
         FrontendSession_ApplyGameSpeedAndReturnToMainPage(g_LocalPlayerRuntimeId,0,0,0);
  }
  else {
    FrontendCommandQueue_EnqueueLocalPlayerCommand(0x2c0,0,0,0);
    callbackDispatchResult = extraout_EAX;
  }
  return callbackDispatchResult;
}

/* Address: 0x00549140.
   Ownership: ui/frontend/runtime.
   Purpose: One-argument frontend action callback. Local mode releases the selected resource and returns to the
   main page; network modes dispatch message offset 0x320. Existing register results are preserved. Queued UI
   action handler for FRONTEND_PAGE20[79] (0x204F). Return datatype is preserved for non-queue direct callers.
   Cross-module calls: FrontendSession_ReleaseSelectedResourceAndReturnToMainPage [ui/frontend/session],
   FrontendCommandQueue_EnqueueLocalPlayerCommand [network/protocol/commands].
*/
undefined4 FrontendCallback_ReleaseSelectedResourceOrDispatch0320(dword callbackArgument)

{
  undefined4 extraout_EAX;
  undefined4 callbackDispatchResult;
  
  if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_NETWORKED_MASK) ==
      SESSION_NETWORK_ROLE_LOCAL) {
    callbackDispatchResult =
         FrontendSession_ReleaseSelectedResourceAndReturnToMainPage(g_LocalPlayerRuntimeId,0,0,0);
  }
  else {
    FrontendCommandQueue_EnqueueLocalPlayerCommand(800,0,0,0);
    callbackDispatchResult = extraout_EAX;
  }
  return callbackDispatchResult;
}

/* Address: 0x00549180.
   Ownership: ui/frontend/runtime.
   Purpose: One-argument no-op callback referenced by the large frontend callback table. The exact original action
   label remains unresolved. Queued UI action handler for FRONTEND_PAGE20[80] (0x2050). Return datatype is
   preserved for non-queue direct callers.
*/
undefined1 FrontendCallback_NoOpArg1(void *source)

{
  undefined1 in_AL;
  
  return in_AL;
}

/* Address: 0x00549AB0.
   Ownership: ui/frontend/runtime.
   Purpose: One-argument frontend action callback. Local mode returns to the main page with state zero; network
   modes dispatch message offset 0xDC0. Existing register results are preserved. Queued UI action handler for
   FRONTEND_PAGE20[64] (0x2040). Return datatype is preserved for non-queue direct callers.
   Cross-module calls: FrontendSession_ReturnToMainPage [ui/frontend/session],
   FrontendCommandQueue_EnqueueLocalPlayerCommand [network/protocol/commands].
*/
undefined4 FrontendCallback_ReturnToMainPageOrDispatch0DC0(dword callbackArgument)

{
  undefined4 extraout_EAX;
  undefined4 callbackDispatchResult;
  
  if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_NETWORKED_MASK) ==
      SESSION_NETWORK_ROLE_LOCAL) {
    callbackDispatchResult = FrontendSession_ReturnToMainPage(g_LocalPlayerRuntimeId,0,0,0);
  }
  else {
    FrontendCommandQueue_EnqueueLocalPlayerCommand(0xdc0,0,0,0);
    callbackDispatchResult = extraout_EAX;
  }
  return callbackDispatchResult;
}

/* Address: 0x0054A5A0.
   Ownership: ui/frontend/runtime.
   Purpose: One-argument frontend action callback. It selects state zero or four from the frontend mode path, then
   either returns locally to the main page or dispatches message offset 0xDC0. Existing register results are
   preserved. Queued UI action handler for FRONTEND_PAGE20[52] (0x2034). Return datatype is preserved for non-queue
   direct callers.
   Cross-module calls: FrontendSession_ReturnToMainPage [ui/frontend/session],
   FrontendCommandQueue_EnqueueLocalPlayerCommand [network/protocol/commands].
*/
undefined4 FrontendCallback_ReturnToMainPageOrDispatchState4(dword callbackArgument)

{
  undefined4 extraout_EAX;
  undefined4 uVar1;
  undefined4 extraout_EAX_00;
  
  if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_NETWORKED_MASK) ==
      SESSION_NETWORK_ROLE_LOCAL) {
    if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_NETWORKED_MASK) ==
        SESSION_NETWORK_ROLE_LOCAL) {
      uVar1 = FrontendSession_ReturnToMainPage(g_LocalPlayerRuntimeId,0,0,0);
    }
    else {
      FrontendCommandQueue_EnqueueLocalPlayerCommand(0xdc0,0,0,0);
      uVar1 = extraout_EAX;
    }
  }
  else if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_NETWORKED_MASK) ==
           SESSION_NETWORK_ROLE_LOCAL) {
    uVar1 = FrontendSession_ReturnToMainPage(g_LocalPlayerRuntimeId,0,0,4);
  }
  else {
    FrontendCommandQueue_EnqueueLocalPlayerCommand(0xdc0,0,0,4);
    uVar1 = extraout_EAX_00;
  }
  return uVar1;
}

/* Address: 0x0054A7D0.
   Ownership: ui/frontend/runtime.
   Purpose: Second one-argument callback slot with the same verified local-main-page versus network-0xDC0 behavior.
   Existing register results are preserved. Queued UI action handler for FRONTEND_PAGE20[51] (0x2033). Return
   datatype is preserved for non-queue direct callers.
   Cross-module calls: FrontendSession_ReturnToMainPage [ui/frontend/session],
   FrontendCommandQueue_EnqueueLocalPlayerCommand [network/protocol/commands].
*/
undefined4 FrontendCallback_ReturnToMainPageOrDispatch0DC0_Secondary(dword callbackArgument)

{
  undefined4 extraout_EAX;
  undefined4 callbackDispatchResult;
  
  if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_NETWORKED_MASK) ==
      SESSION_NETWORK_ROLE_LOCAL) {
    callbackDispatchResult = FrontendSession_ReturnToMainPage(g_LocalPlayerRuntimeId,0,0,0);
  }
  else {
    FrontendCommandQueue_EnqueueLocalPlayerCommand(0xdc0,0,0,0);
    callbackDispatchResult = extraout_EAX;
  }
  return callbackDispatchResult;
}

/* Address: 0x0054AAD0.
   Ownership: ui/frontend/runtime.
   Purpose: Recovered action-table target FRONTEND_PAGE20[16] (0x2010).
   Cross-module calls: FrontendSession_ReturnToMainPage [ui/frontend/session],
   FrontendCommandQueue_EnqueueLocalPlayerCommand [network/protocol/commands], UiPageStack_SetActiveIndex
   [ui/controls/layout].
*/
void FrontendUiAction2010_Handler(UiNodeBase *sourceNode)

{
  UiNodeFlags *pUVar1;
  int parentNodeAddress;
  FrontendRootPageState26C4 *frontendRootPage;
  
  parentNodeAddress = (int)sourceNode->parent;
  frontendRootPage = (FrontendRootPageState26C4 *)sourceNode;
  while ((UiNodeBase *)parentNodeAddress != (UiNodeBase *)0xffffffff) {
    frontendRootPage = (FrontendRootPageState26C4 *)(frontendRootPage->rootNode).parent;
    parentNodeAddress = (int)(frontendRootPage->rootNode).parent;
  }
  if (sourceNode == &frontendRootPage->returnToMainActionControl) {
    if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_NETWORKED_MASK) ==
        SESSION_NETWORK_ROLE_LOCAL) {
      FrontendSession_ReturnToMainPage(g_LocalPlayerRuntimeId,0,0,0);
    }
    else {
      FrontendCommandQueue_EnqueueLocalPlayerCommand(0xdc0,0,0,0);
    }
    return;
  }
  if ((int)g_FramebufferWidth < 0x281) {
    pUVar1 = &(frontendRootPage->compactLayoutControl).nodeFlags;
    *pUVar1 = *pUVar1 | 0x2000;
  }
  UiPageStack_SetActiveIndex(5,&frontendRootPage->primaryPageStack);
  return;
}

/* Address: 0x0054AB70.
   Ownership: ui/frontend/runtime.
   Purpose: Recovered action-table target FRONTEND_PAGE20[17] (0x2011).
   Cross-module calls: UiPageStack_SetActiveIndex [ui/controls/layout], TextResource_Resolve
   [assets/text/resources], PersistentSettings_ReadDword [core/settings/persistent],
   FrontendDisplaySettingsPage_UpdateModeActionAvailability [ui/frontend/settings].
*/
void FrontendUiAction2011_Handler(FrontendDisplaySettingsPageOptionState1010 *source)

{
  byte *pbVar1;
  GraphicsAdapterRecord *pGVar2;
  word *pwVar3;
  uint uVar4;
  undefined4 uVar5;
  undefined4 uVar6;
  GraphicsDisplayModeCount GVar7;
  GraphicsDisplayMode *pGVar8;
  
  UiPageStack_SetActiveIndex
            (6,(UiPageStackControl *)(source[-3].resolutionRows.rows[9].reserved0008_0067 + 0x48));
  if ((int)g_FramebufferWidth < 0x281) {
    pbVar1 = source[-3].resolutionRows.rows[6].reserved0008_0067 + 0x2c;
    *(uint *)pbVar1 = *(uint *)pbVar1 | 0x2000;
  }
  g_FrontendUiDisplayModeAndTaskAssignmentScratch.displayModeScratch.candidateValues10[0] =
       0xffffffff;
  g_FrontendUiDisplayModeAndTaskAssignmentScratch.displayModeScratch.candidateValues10[1] =
       0xffffffff;
  g_FrontendUiDisplayModeAndTaskAssignmentScratch.displayModeScratch.candidateValues10[2] =
       0xffffffff;
  g_FrontendUiDisplayModeAndTaskAssignmentScratch.displayModeScratch.candidateValues10[3] =
       0xffffffff;
  GVar7 = g_GraphicsDisplayModeCount;
  pGVar8 = g_GraphicsDisplayModes;
  do {
    uVar4 = pGVar8->bitsPerPixel;
    if ((((uVar4 != g_FrontendUiDisplayModeAndTaskAssignmentScratch.displayModeScratch.
                    candidateValues10[0]) &&
         (uVar4 != g_FrontendUiDisplayModeAndTaskAssignmentScratch.displayModeScratch.
                   candidateValues10[1])) &&
        (uVar4 != g_FrontendUiDisplayModeAndTaskAssignmentScratch.displayModeScratch.
                  candidateValues10[2])) &&
       (uVar4 != g_FrontendUiDisplayModeAndTaskAssignmentScratch.displayModeScratch.
                 candidateValues10[3])) {
      uVar6 = uVar4;
      if (uVar4 < g_FrontendUiDisplayModeAndTaskAssignmentScratch.displayModeScratch.
                  candidateValues10[0]) {
        LOCK();
        UNLOCK();
        uVar6 = g_FrontendUiDisplayModeAndTaskAssignmentScratch.displayModeScratch.candidateValues10
                [0];
        g_FrontendUiDisplayModeAndTaskAssignmentScratch.displayModeScratch.candidateValues10[0] =
             uVar4;
      }
      uVar5 = uVar6;
      if ((uint)uVar6 <
          g_FrontendUiDisplayModeAndTaskAssignmentScratch.displayModeScratch.candidateValues10[1]) {
        LOCK();
        UNLOCK();
        uVar5 = g_FrontendUiDisplayModeAndTaskAssignmentScratch.displayModeScratch.candidateValues10
                [1];
        g_FrontendUiDisplayModeAndTaskAssignmentScratch.displayModeScratch.candidateValues10[1] =
             uVar6;
      }
      uVar6 = uVar5;
      if ((uint)uVar5 <
          g_FrontendUiDisplayModeAndTaskAssignmentScratch.displayModeScratch.candidateValues10[2]) {
        LOCK();
        UNLOCK();
        uVar6 = g_FrontendUiDisplayModeAndTaskAssignmentScratch.displayModeScratch.candidateValues10
                [2];
        g_FrontendUiDisplayModeAndTaskAssignmentScratch.displayModeScratch.candidateValues10[2] =
             uVar5;
      }
      if ((uint)uVar6 <
          g_FrontendUiDisplayModeAndTaskAssignmentScratch.displayModeScratch.candidateValues10[3]) {
        LOCK();
        UNLOCK();
        g_FrontendUiDisplayModeAndTaskAssignmentScratch.displayModeScratch.candidateValues10[3] =
             uVar6;
      }
    }
    pGVar8 = pGVar8 + 1;
    GVar7 = GVar7 - 1;
  } while (GVar7 != 0);
  (source->colorDepthRows).rows[0].bitsPerPixel =
       g_FrontendUiDisplayModeAndTaskAssignmentScratch.displayModeScratch.candidateValues10[0];
  (source->colorDepthRows).rows[1].bitsPerPixel =
       g_FrontendUiDisplayModeAndTaskAssignmentScratch.displayModeScratch.candidateValues10[1];
  (source->colorDepthRows).rows[2].bitsPerPixel =
       g_FrontendUiDisplayModeAndTaskAssignmentScratch.displayModeScratch.candidateValues10[2];
  (source->colorDepthRows).rows[3].bitsPerPixel =
       g_FrontendUiDisplayModeAndTaskAssignmentScratch.displayModeScratch.candidateValues10[3];
  pGVar2 = g_GraphicsAdapters;
  (source->adapterRows).rows[0].adapterDescriptionUtf16 = g_GraphicsAdapters->driverDescriptionUtf16
  ;
  if ((pGVar2->deviceGuid).Data1 == 0) {
    pwVar3 = TextResource_Resolve(0x212d);
  }
  else {
    pwVar3 = pGVar2->deviceNameUtf16;
  }
  (source->adapterRows).rows[0].deviceNameUtf16 = pwVar3;
  pGVar2 = g_GraphicsAdapters;
  if (1 < g_GraphicsAdapterCount) {
    (source->adapterRows).rows[1].adapterDescriptionUtf16 =
         g_GraphicsAdapters[1].driverDescriptionUtf16;
    if (pGVar2[1].deviceGuid.Data1 == 0) {
      pwVar3 = TextResource_Resolve(0x212d);
    }
    else {
      pwVar3 = pGVar2[1].deviceNameUtf16;
    }
    (source->adapterRows).rows[1].deviceNameUtf16 = pwVar3;
  }
  pGVar2 = g_GraphicsAdapters;
  if (2 < g_GraphicsAdapterCount) {
    (source->adapterRows).rows[2].adapterDescriptionUtf16 =
         g_GraphicsAdapters[2].driverDescriptionUtf16;
    if (pGVar2[2].deviceGuid.Data1 == 0) {
      pwVar3 = TextResource_Resolve(0x212d);
    }
    else {
      pwVar3 = pGVar2[2].deviceNameUtf16;
    }
    (source->adapterRows).rows[2].deviceNameUtf16 = pwVar3;
  }
  pGVar2 = g_GraphicsAdapters;
  if (3 < g_GraphicsAdapterCount) {
    (source->adapterRows).rows[3].adapterDescriptionUtf16 =
         g_GraphicsAdapters[3].driverDescriptionUtf16;
    if (pGVar2[3].deviceGuid.Data1 == 0) {
      pwVar3 = TextResource_Resolve(0x212d);
    }
    else {
      pwVar3 = pGVar2[3].deviceNameUtf16;
    }
    (source->adapterRows).rows[3].deviceNameUtf16 = pwVar3;
  }
  pGVar2 = g_GraphicsAdapters;
  if (4 < g_GraphicsAdapterCount) {
    (source->adapterRows).rows[4].adapterDescriptionUtf16 =
         g_GraphicsAdapters[4].driverDescriptionUtf16;
    if (pGVar2[4].deviceGuid.Data1 == 0) {
      pwVar3 = TextResource_Resolve(0x212d);
    }
    else {
      pwVar3 = pGVar2[4].deviceNameUtf16;
    }
    (source->adapterRows).rows[4].deviceNameUtf16 = pwVar3;
  }
  g_FrontendUiDisplayModeAndTaskAssignmentScratch.displayModeScratch.candidateValues10[0] =
       0xffffffff;
  g_FrontendUiDisplayModeAndTaskAssignmentScratch.displayModeScratch.candidateValues10[1] =
       0xffffffff;
  g_FrontendUiDisplayModeAndTaskAssignmentScratch.displayModeScratch.candidateValues10[2] =
       0xffffffff;
  g_FrontendUiDisplayModeAndTaskAssignmentScratch.displayModeScratch.candidateValues10[3] =
       0xffffffff;
  g_FrontendUiDisplayModeAndTaskAssignmentScratch.displayModeScratch.candidateValues10[4] =
       0xffffffff;
  g_FrontendUiDisplayModeAndTaskAssignmentScratch.displayModeScratch.candidateValues10[5] =
       0xffffffff;
  g_FrontendUiDisplayModeAndTaskAssignmentScratch.displayModeScratch.candidateValues10[6] =
       0xffffffff;
  g_FrontendUiDisplayModeAndTaskAssignmentScratch.displayModeScratch.candidateValues10[7] =
       0xffffffff;
  g_FrontendUiDisplayModeAndTaskAssignmentScratch.displayModeScratch.candidateValues10[8] =
       0xffffffff;
  g_FrontendUiDisplayModeAndTaskAssignmentScratch.displayModeScratch.candidateValues10[9] =
       0xffffffff;
  GVar7 = g_GraphicsDisplayModeCount;
  pGVar8 = g_GraphicsDisplayModes;
  do {
    uVar4 = pGVar8->width * 0x10000 + pGVar8->height;
    if ((((uVar4 != g_FrontendUiDisplayModeAndTaskAssignmentScratch.displayModeScratch.
                    candidateValues10[0]) &&
         (uVar4 != g_FrontendUiDisplayModeAndTaskAssignmentScratch.displayModeScratch.
                   candidateValues10[1])) &&
        ((uVar4 != g_FrontendUiDisplayModeAndTaskAssignmentScratch.displayModeScratch.
                   candidateValues10[2] &&
         ((uVar4 != g_FrontendUiDisplayModeAndTaskAssignmentScratch.displayModeScratch.
                    candidateValues10[3] &&
          (uVar4 != g_FrontendUiDisplayModeAndTaskAssignmentScratch.displayModeScratch.
                    candidateValues10[4])))))) &&
       ((uVar4 != g_FrontendUiDisplayModeAndTaskAssignmentScratch.displayModeScratch.
                  candidateValues10[5] &&
        ((((uVar4 != g_FrontendUiDisplayModeAndTaskAssignmentScratch.displayModeScratch.
                     candidateValues10[6] &&
           (uVar4 != g_FrontendUiDisplayModeAndTaskAssignmentScratch.displayModeScratch.
                     candidateValues10[7])) &&
          (uVar4 != g_FrontendUiDisplayModeAndTaskAssignmentScratch.displayModeScratch.
                    candidateValues10[8])) &&
         (uVar4 != g_FrontendUiDisplayModeAndTaskAssignmentScratch.displayModeScratch.
                   candidateValues10[9])))))) {
      uVar6 = uVar4;
      if (uVar4 < g_FrontendUiDisplayModeAndTaskAssignmentScratch.displayModeScratch.
                  candidateValues10[0]) {
        LOCK();
        UNLOCK();
        uVar6 = g_FrontendUiDisplayModeAndTaskAssignmentScratch.displayModeScratch.candidateValues10
                [0];
        g_FrontendUiDisplayModeAndTaskAssignmentScratch.displayModeScratch.candidateValues10[0] =
             uVar4;
      }
      uVar5 = uVar6;
      if ((uint)uVar6 <
          g_FrontendUiDisplayModeAndTaskAssignmentScratch.displayModeScratch.candidateValues10[1]) {
        LOCK();
        UNLOCK();
        uVar5 = g_FrontendUiDisplayModeAndTaskAssignmentScratch.displayModeScratch.candidateValues10
                [1];
        g_FrontendUiDisplayModeAndTaskAssignmentScratch.displayModeScratch.candidateValues10[1] =
             uVar6;
      }
      uVar6 = uVar5;
      if ((uint)uVar5 <
          g_FrontendUiDisplayModeAndTaskAssignmentScratch.displayModeScratch.candidateValues10[2]) {
        LOCK();
        UNLOCK();
        uVar6 = g_FrontendUiDisplayModeAndTaskAssignmentScratch.displayModeScratch.candidateValues10
                [2];
        g_FrontendUiDisplayModeAndTaskAssignmentScratch.displayModeScratch.candidateValues10[2] =
             uVar5;
      }
      uVar5 = uVar6;
      if ((uint)uVar6 <
          g_FrontendUiDisplayModeAndTaskAssignmentScratch.displayModeScratch.candidateValues10[3]) {
        LOCK();
        UNLOCK();
        uVar5 = g_FrontendUiDisplayModeAndTaskAssignmentScratch.displayModeScratch.candidateValues10
                [3];
        g_FrontendUiDisplayModeAndTaskAssignmentScratch.displayModeScratch.candidateValues10[3] =
             uVar6;
      }
      uVar6 = uVar5;
      if ((uint)uVar5 <
          g_FrontendUiDisplayModeAndTaskAssignmentScratch.displayModeScratch.candidateValues10[4]) {
        LOCK();
        UNLOCK();
        uVar6 = g_FrontendUiDisplayModeAndTaskAssignmentScratch.displayModeScratch.candidateValues10
                [4];
        g_FrontendUiDisplayModeAndTaskAssignmentScratch.displayModeScratch.candidateValues10[4] =
             uVar5;
      }
      uVar5 = uVar6;
      if ((uint)uVar6 <
          g_FrontendUiDisplayModeAndTaskAssignmentScratch.displayModeScratch.candidateValues10[5]) {
        LOCK();
        UNLOCK();
        uVar5 = g_FrontendUiDisplayModeAndTaskAssignmentScratch.displayModeScratch.candidateValues10
                [5];
        g_FrontendUiDisplayModeAndTaskAssignmentScratch.displayModeScratch.candidateValues10[5] =
             uVar6;
      }
      uVar6 = uVar5;
      if ((uint)uVar5 <
          g_FrontendUiDisplayModeAndTaskAssignmentScratch.displayModeScratch.candidateValues10[6]) {
        LOCK();
        UNLOCK();
        uVar6 = g_FrontendUiDisplayModeAndTaskAssignmentScratch.displayModeScratch.candidateValues10
                [6];
        g_FrontendUiDisplayModeAndTaskAssignmentScratch.displayModeScratch.candidateValues10[6] =
             uVar5;
      }
      uVar5 = uVar6;
      if ((uint)uVar6 <
          g_FrontendUiDisplayModeAndTaskAssignmentScratch.displayModeScratch.candidateValues10[7]) {
        LOCK();
        UNLOCK();
        uVar5 = g_FrontendUiDisplayModeAndTaskAssignmentScratch.displayModeScratch.candidateValues10
                [7];
        g_FrontendUiDisplayModeAndTaskAssignmentScratch.displayModeScratch.candidateValues10[7] =
             uVar6;
      }
      uVar6 = uVar5;
      if ((uint)uVar5 <
          g_FrontendUiDisplayModeAndTaskAssignmentScratch.displayModeScratch.candidateValues10[8]) {
        LOCK();
        UNLOCK();
        uVar6 = g_FrontendUiDisplayModeAndTaskAssignmentScratch.displayModeScratch.candidateValues10
                [8];
        g_FrontendUiDisplayModeAndTaskAssignmentScratch.displayModeScratch.candidateValues10[8] =
             uVar5;
      }
      if ((uint)uVar6 <
          g_FrontendUiDisplayModeAndTaskAssignmentScratch.displayModeScratch.candidateValues10[9]) {
        LOCK();
        UNLOCK();
        g_FrontendUiDisplayModeAndTaskAssignmentScratch.displayModeScratch.candidateValues10[9] =
             uVar6;
      }
    }
    pGVar8 = pGVar8 + 1;
    GVar7 = GVar7 - 1;
  } while (GVar7 != 0);
  uVar4 = g_FrontendUiDisplayModeAndTaskAssignmentScratch.displayModeScratch.candidateValues10[0] &
          0xffff;
  (source->resolutionRows).rows[0].width =
       g_FrontendUiDisplayModeAndTaskAssignmentScratch.displayModeScratch.candidateValues10[0] >>
       0x10;
  (source->resolutionRows).rows[0].height = uVar4;
  uVar4 = g_FrontendUiDisplayModeAndTaskAssignmentScratch.displayModeScratch.candidateValues10[1] &
          0xffff;
  (source->resolutionRows).rows[1].width =
       g_FrontendUiDisplayModeAndTaskAssignmentScratch.displayModeScratch.candidateValues10[1] >>
       0x10;
  (source->resolutionRows).rows[1].height = uVar4;
  uVar4 = g_FrontendUiDisplayModeAndTaskAssignmentScratch.displayModeScratch.candidateValues10[2] &
          0xffff;
  (source->resolutionRows).rows[2].width =
       g_FrontendUiDisplayModeAndTaskAssignmentScratch.displayModeScratch.candidateValues10[2] >>
       0x10;
  (source->resolutionRows).rows[2].height = uVar4;
  uVar4 = g_FrontendUiDisplayModeAndTaskAssignmentScratch.displayModeScratch.candidateValues10[3] &
          0xffff;
  (source->resolutionRows).rows[3].width =
       g_FrontendUiDisplayModeAndTaskAssignmentScratch.displayModeScratch.candidateValues10[3] >>
       0x10;
  (source->resolutionRows).rows[3].height = uVar4;
  uVar4 = g_FrontendUiDisplayModeAndTaskAssignmentScratch.displayModeScratch.candidateValues10[4] &
          0xffff;
  (source->resolutionRows).rows[4].width =
       g_FrontendUiDisplayModeAndTaskAssignmentScratch.displayModeScratch.candidateValues10[4] >>
       0x10;
  (source->resolutionRows).rows[4].height = uVar4;
  uVar4 = g_FrontendUiDisplayModeAndTaskAssignmentScratch.displayModeScratch.candidateValues10[5] &
          0xffff;
  (source->resolutionRows).rows[5].width =
       g_FrontendUiDisplayModeAndTaskAssignmentScratch.displayModeScratch.candidateValues10[5] >>
       0x10;
  (source->resolutionRows).rows[5].height = uVar4;
  uVar4 = g_FrontendUiDisplayModeAndTaskAssignmentScratch.displayModeScratch.candidateValues10[6] &
          0xffff;
  (source->resolutionRows).rows[6].width =
       g_FrontendUiDisplayModeAndTaskAssignmentScratch.displayModeScratch.candidateValues10[6] >>
       0x10;
  (source->resolutionRows).rows[6].height = uVar4;
  uVar4 = g_FrontendUiDisplayModeAndTaskAssignmentScratch.displayModeScratch.candidateValues10[7] &
          0xffff;
  (source->resolutionRows).rows[7].width =
       g_FrontendUiDisplayModeAndTaskAssignmentScratch.displayModeScratch.candidateValues10[7] >>
       0x10;
  (source->resolutionRows).rows[7].height = uVar4;
  uVar4 = g_FrontendUiDisplayModeAndTaskAssignmentScratch.displayModeScratch.candidateValues10[8] &
          0xffff;
  (source->resolutionRows).rows[8].width =
       g_FrontendUiDisplayModeAndTaskAssignmentScratch.displayModeScratch.candidateValues10[8] >>
       0x10;
  (source->resolutionRows).rows[8].height = uVar4;
  uVar4 = g_FrontendUiDisplayModeAndTaskAssignmentScratch.displayModeScratch.candidateValues10[9] &
          0xffff;
  (source->resolutionRows).rows[9].width =
       g_FrontendUiDisplayModeAndTaskAssignmentScratch.displayModeScratch.candidateValues10[9] >>
       0x10;
  (source->resolutionRows).rows[9].height = uVar4;
  g_FrontendUiDisplayModeAndTaskAssignmentScratch.displayEnumeration.persistentSelection.
  adapterIndex = PersistentSettings_ReadDword(1,0);
  g_FrontendUiDisplayModeAndTaskAssignmentScratch.displayEnumeration.persistentSelection.width =
       PersistentSettings_ReadDword(0x280,4);
  g_FrontendUiDisplayModeAndTaskAssignmentScratch.displayEnumeration.persistentSelection.height =
       PersistentSettings_ReadDword(0x1e0,8);
  g_FrontendUiDisplayModeAndTaskAssignmentScratch.displayEnumeration.persistentSelection.
  bitsPerPixel = PersistentSettings_ReadDword(0x10,0xc);
  FrontendDisplaySettingsPage_UpdateModeActionAvailability((UiNodeBase *)source);
  return;
}

/* Address: 0x0054BA30.
   Ownership: ui/frontend/runtime.
   Purpose: Recovered action-table target
   FRONTEND_PAGE20[44],FRONTEND_PAGE20[45],FRONTEND_PAGE20[46],FRONTEND_PAGE20[47],FRONTEND_PAGE20[48]
   (0x202C,0x202D,0x202E,0x202F,0x2030).
   Cross-module calls: FrontendDisplaySettingsPage_UpdateModeActionAvailability [ui/frontend/settings].
*/
void FrontendUiAction202CTo2030_SharedHandler(UiNodeBase *sourceNode)

{
  int controlOffsetFromParent;
  
  g_FrontendUiDisplayModeAndTaskAssignmentScratch.displayEnumeration.persistentSelection.
  adapterIndex = 0;
  controlOffsetFromParent = (int)sourceNode - (int)sourceNode->parent;
  if ((((controlOffsetFromParent != 0x54) &&
       (g_FrontendUiDisplayModeAndTaskAssignmentScratch.displayEnumeration.persistentSelection.
        adapterIndex = 1, controlOffsetFromParent != 0xbc)) &&
      (g_FrontendUiDisplayModeAndTaskAssignmentScratch.displayEnumeration.persistentSelection.
       adapterIndex = 2, controlOffsetFromParent != 0x124)) &&
     (g_FrontendUiDisplayModeAndTaskAssignmentScratch.displayEnumeration.persistentSelection.
      adapterIndex = 3, controlOffsetFromParent != 0x18c)) {
    g_FrontendUiDisplayModeAndTaskAssignmentScratch.displayEnumeration.persistentSelection.
    adapterIndex = 4;
  }
  FrontendDisplaySettingsPage_UpdateModeActionAvailability(sourceNode->parent);
  return;
}

/* Address: 0x0054D3F0.
   Ownership: ui/frontend/runtime.
   Purpose: Recovered action-table target FRONTEND_PAGE20[12] (0x200C).
   Cross-module calls: UiNodeList_SuppressActionId [ui/controls/lists], UiNodeList_UnsuppressActionId
   [ui/controls/lists].
*/
void FrontendUiAction200C_Handler(UiPointerListControl *sessionListControl)

{
  UiPointerListControl *firstNode;
  UiNodeBase *parentCursor;
  
  parentCursor = (sessionListControl->base).parent;
  firstNode = sessionListControl;
  while (parentCursor != (UiNodeBase *)0xffffffff) {
    firstNode = (UiPointerListControl *)(firstNode->base).parent;
    parentCursor = (firstNode->base).parent;
  }
  if (sessionListControl->selectedRowSlot == sessionListControl->rowSlots) {
    UiNodeList_SuppressActionId(0x200b,&firstNode->base);
    return;
  }
  UiNodeList_UnsuppressActionId(0x200b,&firstNode->base);
  return;
}

/* Address: 0x0054D460.
   Ownership: ui/frontend/runtime.
   Purpose: Binary entry is anchored by g_UiActionPage20InitializedHandlers[14]@00545938. Queued UI action handler
   for FRONTEND_PAGE20[14] (0x200E). Return datatype is preserved for non-queue direct callers. Typed parameters:
   p0 source→UiNodeBase *. Calling convention, complete VariableStorage serialization, function bytes, control
   flow, globals, locals, and executable data remain unchanged.
   Cross-module calls: RecentTextHistory_RemoveOldest [ui/support/runtime],
   RecentTextHistory_SortAndBuildPointerList [ui/support/runtime].
*/
undefined4 FrontendRecentText_TrimAndSortTopFive(UiNodeBase *source)

{
  undefined4 in_EAX;
  int extraout_ECX;
  uint currentEntryCount;
  
  currentEntryCount = (uint)source[1].vtable;
  while ((UiNodeVtable *)0x4 < currentEntryCount) {
    RecentTextHistory_RemoveOldest();
    currentEntryCount = extraout_ECX - 1;
  }
  RecentTextHistory_RemoveOldest();
  RecentTextHistory_SortAndBuildPointerList(5,(RecentTextHistoryPointerList *)&source[1].vtable);
  return in_EAX;
}

/* Address: 0x0054D4A0.
   Ownership: ui/frontend/runtime.
   Purpose: Recovered action-table target FRONTEND_PAGE20[15] (0x200F).
   Cross-module calls: UiPointerList_GetSelectedIndexVariantACf [ui/controls/lists], UiNodeList_SuppressActionId
   [ui/controls/lists], UiPointerList_InitializeColumnLayout [ui/controls/lists],
   UiTransfer_SendPacketType10000Value2931Cf [network/protocol/transfer], FrontendSession_ReturnToMainPage
   [ui/frontend/session], FrontendCommandQueue_EnqueueLocalPlayerCommand [network/protocol/commands].
*/
void FrontendUiAction200F_Handler(FrontendNetworkSetupPageBackendListPtr backendList)

{
  UiListRowIndex UVar1;
  UiPointerListControl *control;
  dword returnValue;
  dword extraout_ECX;
  int iVar2;
  dword extraout_ECX_00;
  dword dVar3;
  undefined4 extraout_EDX;
  undefined4 extraout_EDX_00;
  undefined4 uVar4;
  dword *endpointSourceDwordCursor;
  dword *endpointDestinationDwordCursor;
  undefined1 uVar5;
  
  UVar1 = UiPointerList_GetSelectedIndexVariantACf(backendList);
  uVar5 = UVar1 < g_NetworkBackendInstanceCount;
  if (!(bool)uVar5) {
    return;
  }
  (*g_NetworkBackendSlot3)();
  (*g_NetworkBackendSlot1)();
  dVar3 = returnValue;
  (*g_NetworkBackendSlot0)(returnValue);
  (*g_FatalErrorRuntimeDispatchCf)(dVar3);
  dVar3 = extraout_ECX;
  uVar4 = extraout_EDX;
  if (!(bool)uVar5) {
    (*g_NetworkBackendSlot2)(0x3a1);
    (*g_FatalErrorRuntimeDispatchCf)();
    if (!(bool)uVar5) {
      endpointSourceDwordCursor = (dword *)&g_NetworkLocalEndpointDescriptor16;
      endpointDestinationDwordCursor = (dword *)&g_FrontendNetworkEndpointScratch;
      for (iVar2 = 4; iVar2 != 0; iVar2 = iVar2 + -1) {
        *endpointDestinationDwordCursor = *endpointSourceDwordCursor;
        endpointSourceDwordCursor = endpointSourceDwordCursor + 1;
        endpointDestinationDwordCursor = endpointDestinationDwordCursor + 1;
      }
      (*g_NetworkBackendSlot7)
                (&g_FrontendNetworkEndpointTextUtf16,
                 (WinSockAddress *)&g_FrontendNetworkEndpointScratch);
      UiNodeList_SuppressActionId(0x2002,&ADJ(backendList).rootNode);
      UiPointerList_InitializeColumnLayout(0,g_FrontendSessionListRows,control);
      UiTransfer_SendPacketType10000Value2931Cf();
      return;
    }
    (*g_NetworkBackendSlot1)();
    dVar3 = extraout_ECX_00;
    uVar4 = extraout_EDX_00;
  }
  (*g_NetworkBackendSlot0)(dVar3);
  if (!(bool)uVar5) {
    (*g_NetworkBackendSlot2)(0x3a1);
    if (!(bool)uVar5) {
      return;
    }
    (*g_NetworkBackendSlot1)(uVar4);
  }
  if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_NETWORKED_MASK) ==
      SESSION_NETWORK_ROLE_LOCAL) {
    FrontendSession_ReturnToMainPage(g_LocalPlayerRuntimeId,0,0,0);
  }
  else {
    FrontendCommandQueue_EnqueueLocalPlayerCommand(0xdc0,0,0,0);
  }
  Random_SelectPrimaryStream();
  return;
}

/* Address: 0x00565A30.
   Ownership: ui/frontend/runtime.
   Purpose: Existing post-movie results and UI flow are left untouched.
   Cross-module calls: Movie_Close [movie/runtime/playback], Movie_Open [movie/runtime/playback],
   UiPageStack_SetActiveIndex [ui/controls/layout], Movie_AdvanceFrame [movie/runtime/playback],
   UiNode_InvalidateRoot [ui/core/runtime], UiFrame_ProcessAndPresent [ui/controls/layout].
*/
void Frontend_PlaySelectedEndMovie(void)

{
  UiRootCallbacks *pUVar1;
  ulonglong uVar2;
  InGameRuntimeRootImageC3E4 *pIVar3;
  sdword arg4;
  MovieRuntime *pMVar4;
  int extraout_EAX;
  word *pwVar5;
  int iVar6;
  dword arg0;
  int iVar7;
  int extraout_ECX;
  TextResourceId resourceId;
  FrontendPlayerRuntimeBlockCount FVar8;
  undefined4 extraout_ECX_00;
  UiPageStackControl *stack;
  WorldRuntimeContext *worldRuntime;
  WorldRuntimeContext *extraout_EDX;
  word *stream;
  undefined4 extraout_EDX_00;
  uint uVar9;
  int factionIndex;
  byte *pbVar10;
  FactionRuntimeLifecycleObservedState *pFVar11;
  FrontendPlayerRuntimeRecord *pFVar12;
  byte *pbVar13;
  undefined1 uVar14;
  bool bVar15;
  
  pIVar3 = g_InGameRuntimeRoot;
  (*g_GraphicsCursorSetFrame)(0);
  g_CursorVisibilityToken = g_CursorVisibilityToken + -1;
  if ((pIVar3 != (InGameRuntimeRootImageC3E4 *)0x0) &&
     (pUVar1 = (pIVar3->rootUi0000).callbacks, g_EndMoviePath != (word *)0x0)) {
    pUVar1->keyboardFallbackCf = EndMovieUiRuntime_DispatchCommandByFlagsCf;
    pUVar1->frameUpdate = EndMovieUiRuntime_HandleModeTransitionCf;
    uVar14 = 0;
    if (g_FrontendLoadedCampaignAsset != 0) {
      iVar6 = *(int *)(g_FrontendLoadedCampaignAsset + 0xb8);
      uVar9 = g_FrontendLoadedCampaignAsset + 0x200;
      do {
        if (*(int *)(g_FrontendLoadedCampaignAsset + 0xc4) == *(int *)(uVar9 + 0x100)) {
          uVar14 = 0;
          if (g_EndMovieVariantIndex == 0) {
            arg4 = *(sdword *)(uVar9 + 0x40 + g_EndMovieSelectionIndex * 4);
          }
          else {
            arg4 = *(sdword *)(uVar9 + 0x20 + g_EndMovieSelectionIndex * 4);
          }
          (*g_WideNumberFormatUtf16)
                    (WIDE_FORMAT_PAD_WITH_ZERO,0,4,1,arg4,(word *)(u_flm_ende0000_flm_0050df4a + 8))
          ;
          g_EndMoviePath = (word *)u_flm_ende0000_flm_0050df4a;
          break;
        }
        uVar14 = 0xfffffe7f < uVar9;
        uVar9 = uVar9 + 0x180;
        iVar6 = iVar6 + -1;
      } while (iVar6 != 0);
    }
    Movie_Close();
    (*g_GraphicsFramebufferBeginAccess)();
    if (!(bool)uVar14) {
      (*g_GraphicsFramebufferFillRectArgb)
                (g_FramebufferHeight,g_FramebufferWidth,0,0,g_FramebufferHeight,g_FramebufferWidth,0
                 ,0,0xff000000,g_FramebufferAccess);
      (*g_GraphicsFramebufferEndAccess)();
      (*g_GraphicsFramebufferPresent)(g_FramebufferAccess);
    }
    (*g_GraphicsFramebufferBeginAccess)();
    if (!(bool)uVar14) {
      (*g_GraphicsFramebufferFillRectArgb)
                (g_FramebufferHeight,g_FramebufferWidth,0,0,g_FramebufferHeight,g_FramebufferWidth,0
                 ,0,0xff000000,g_FramebufferAccess);
      (*g_GraphicsFramebufferEndAccess)();
      (*g_GraphicsFramebufferPresent)(g_FramebufferAccess);
    }
    Movie_Open(1,g_EndMoviePath);
    pIVar3 = g_InGameRuntimeRoot;
    if (!(bool)uVar14) {
      g_EndMoviePendingTicks = 0;
      (*g_TimerRegisterPeriodic)(arg0,FrontendSession_PeriodicTick);
      UiPageStack_SetActiveIndex(1,stack);
      pMVar4 = Movie_AdvanceFrame();
      if (!(bool)uVar14) {
        pIVar3->activeEndMovieRuntime022C = pMVar4;
        pIVar3->endMoviePlaybackState0230 = 0;
        g_EndMoviePendingTicks = 0;
        do {
          bVar15 = false;
          if (g_EndMoviePendingTicks != 0) {
            g_EndMoviePendingTicks = g_EndMoviePendingTicks - 1;
            Movie_AdvanceFrame();
            if (bVar15) {
              g_UiCommandRuntimeFlags = g_UiCommandRuntimeFlags & 0xfffff7ff;
            }
          }
          UiNode_InvalidateRoot((UiNodeBase *)pIVar3);
          UiFrame_ProcessAndPresent();
        } while ((g_UiCommandRuntimeFlags & 0x800) != 0);
      }
      g_CursorVisibilityToken = g_CursorVisibilityToken + 1;
      UiPageStack_SetActiveIndex(1,&pIVar3->endMoviePageStack02F8);
      iVar7 = 7;
      pFVar11 = g_GameFactionRuntimeImage.tail.factionLifecycleStates;
      iVar6 = 0;
      factionIndex = 1;
      worldRuntime = &pIVar3->worldRuntime0A30;
      do {
        pFVar11 = pFVar11 + 1;
        if (*pFVar11 != 0) {
          GameFactionRuntime_RecomputeProgressAndScoreMetrics(factionIndex,worldRuntime);
          iVar6 = extraout_EAX;
          iVar7 = extraout_ECX;
          worldRuntime = extraout_EDX;
        }
        factionIndex = factionIndex + 1;
        iVar7 = iVar7 + -1;
      } while (iVar7 != 0);
      if (iVar6 != 0) {
        *(int *)(pIVar3->opaque034C_08D3 + 0x114) = iVar6;
        *(int *)(pIVar3->opaque034C_08D3 + 400) = iVar6;
        *(int *)(pIVar3->opaque034C_08D3 + 0x20c) = iVar6;
        uVar2 = (ulonglong)(g_GameFactionRuntimeImage.tail.periodicClockTick + 0x12bf) / 0x12c0;
        (*g_LocaleFormatTimeFieldsUtf16)
                  ((dword)(uVar2 / 0x3c),(dword)(uVar2 % 0x3c),
                   (word *)&g_EndGameElapsedTimeScratchUtf16);
        pwVar5 = TextResource_Resolve(0x21c0);
        RichTextCommandStream_PatchPayloadBySelector(1,&g_EndGameElapsedTimeScratchUtf16,pwVar5);
        pwVar5 = TextResource_Resolve(resourceId);
        RichTextCommandStream_PatchPayloadBySelector(0,pwVar5,stream);
        UiNodeList_UnsuppressActionId(0x101b,(UiNodeBase *)pIVar3);
        if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_NETWORKED_MASK) ==
            SESSION_NETWORK_ROLE_LOCAL) {
          UiNodeList_SuppressActionId(0x1025,(UiNodeBase *)pIVar3);
        }
        FVar8 = g_FrontendPlayerRuntimeBlockCount;
        pFVar12 = g_FrontendPlayerRuntimeBlocks;
        if (((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_HOST) != SESSION_NETWORK_ROLE_LOCAL)
           && (1 < g_FrontendPlayerRuntimeBlockCount)) {
          UiNodeList_SuppressActionId(0x101b,(UiNodeBase *)pIVar3);
          FVar8 = g_FrontendPlayerRuntimeBlockCount;
          pFVar12 = g_FrontendPlayerRuntimeBlocks;
        }
        do {
          (pFVar12->factionAssignment).readyOrWaitState = 0;
          FVar8 = FVar8 - 1;
          pFVar12 = pFVar12 + 1;
        } while (FVar8 != 0);
        if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_NETWORKED_MASK) ==
            SESSION_NETWORK_ROLE_LOCAL) {
          uVar9 = *(uint *)(pIVar3->opaque034C_08D3 + 0x110);
          *(int *)(pIVar3->opaque034C_08D3 + 0x110) = *(int *)(pIVar3->opaque034C_08D3 + 0x110) + -1
          ;
          iVar6 = uVar9 - 3;
          if (2 < uVar9 && iVar6 != 0) {
            pbVar10 = pIVar3->opaque034C_08D3 + 300;
            pbVar13 = pIVar3->opaque034C_08D3 + 0x128;
            for (; iVar6 != 0; iVar6 = iVar6 + -1) {
              *(undefined4 *)pbVar13 = *(undefined4 *)pbVar10;
              pbVar10 = pbVar10 + 4;
              pbVar13 = pbVar13 + 4;
            }
          }
        }
        do {
          UiRootStack_InvalidateAll();
          UiFrame_ProcessAndPresent();
          if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_HOST) != SESSION_NETWORK_ROLE_LOCAL)
          {
            FrontendPlayerRuntime_MarkReadyByIdAndUpdateAction101B
                      (extraout_ECX_00,extraout_EDX_00,0xffffffff);
          }
        } while ((g_UiCommandRuntimeFlags & 0x1000) == 0);
      }
      (*g_TimerUnregisterPeriodic)(FrontendSession_PeriodicTick);
      Movie_Close();
      goto Frontend_PlaySelectedEndMovie_RestoreEndGameResultsCallbacksAndReturn;
    }
  }
  g_CursorVisibilityToken = g_CursorVisibilityToken + 1;
Frontend_PlaySelectedEndMovie_RestoreEndGameResultsCallbacksAndReturn:
  pUVar1 = (g_InGameRuntimeRoot->rootUi0000).callbacks;
  pUVar1->keyboardFallbackCf = EndGameResultsUiRuntime_DispatchCommandByFlagsCf;
  pUVar1->frameUpdate = EndGameResultsUiRuntime_UpdateAndHandleInputCf;
  return;
}

/* Address: 0x00546700.
   Ownership: ui/frontend/runtime.
   Purpose: Handles frontend init.
   Local calls: FrontendMenu_BindSharedResources, Frontend_StateTick.
   Cross-module calls: PersistentSettings_ReadDword [core/settings/persistent], UiRuntime_SetSynchronizationHooks
   [ui/core/runtime], TextResource_Resolve [assets/text/resources], RichTextCommandStream_PatchPayloadBySelector
   [assets/text/richtext], Resource_Load [assets/resource/runtime], Resource_Release [assets/resource/runtime].
*/
undefined8 __fastcall Frontend_Init(undefined4 param_1,undefined4 param_2,RomRecordId param_3)

{
  WorldRuntimeContext *worldRuntime;
  SessionNetworkRoleFlags SVar1;
  dword dVar2;
  GraphicsPaletteAsset *pGVar3;
  word *pwVar4;
  SoundSampleAsset *arg0;
  GraphicsPaletteAsset *root;
  DirectSoundVoiceSet *arg2;
  IDirectSoundBuffer *pIVar5;
  AssetMagic AVar6;
  SessionNetworkRoleFlags SVar7;
  dword arg0_00;
  undefined4 extraout_ECX;
  undefined4 uVar8;
  undefined4 extraout_ECX_00;
  int iVar9;
  uint extraout_ECX_01;
  uint uVar10;
  undefined4 extraout_ECX_02;
  dword dVar11;
  undefined4 extraout_ECX_03;
  undefined4 uVar12;
  undefined4 extraout_EDX;
  undefined4 extraout_EDX_00;
  void *allocation;
  dword *pdVar13;
  undefined4 extraout_EDX_01;
  AssetMagic *pAVar14;
  undefined4 *puVar15;
  FrontendPlayerRuntimeRecord *pFVar16;
  undefined4 *puVar17;
  byte *pbVar18;
  undefined1 uVar19;
  bool bVar20;
  undefined8 uVar21;
  
  dVar2 = PersistentSettings_ReadDword(0,0x30);
  g_TextureDownsampleShift = dVar2 >> 1;
  uVar19 = false;
  pFVar16 = g_FrontendPlayerRuntimeBlocks;
  SVar7 = g_FrontendPlayerRuntimeBlockCount;
  SVar1 = g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_NETWORKED_MASK;
  while (SVar1 != SESSION_NETWORK_ROLE_LOCAL) {
    (pFVar16->factionAssignment).readyOrWaitState = 0;
    pFVar16->commandSyncPending = FRONTEND_COMMAND_SYNC_PENDING;
    uVar19 = (FrontendPlayerRuntimeRecord *)0xffffec4f < pFVar16;
    pFVar16 = pFVar16 + 1;
    SVar7 = SVar7 - SESSION_NETWORK_ROLE_CLIENT;
    SVar1 = SVar7;
  }
  (*g_GraphicsFramebufferBeginAccess)();
  if (!(bool)uVar19) {
    (*g_GraphicsFramebufferFillRectArgb)
              (g_FramebufferHeight,g_FramebufferWidth,0,0,g_FramebufferHeight,g_FramebufferWidth,0,0
               ,0xff000000,g_FramebufferAccess);
    (*g_GraphicsFramebufferEndAccess)();
  }
  (*g_GraphicsFramebufferPresent)(g_FramebufferAccess);
  (*g_GraphicsCursorSetFrame)(6);
  g_CursorVisibilityToken = g_CursorVisibilityToken + 1;
  g_FrontendRomTransitionPendingCount = 0;
  g_FrontendPendingPageAction = 0;
  g_FrontendRuntimeFlags = 0x10;
  g_FrontendTimerCountdownTicks = 4;
  g_FrontendStateTickSpinLock = 0;
  (*g_TimerRegisterPeriodic)(0x50,FrontendRuntime_TimerCountdownTick);
  UiRuntime_SetSynchronizationHooks(Frontend_StateTick,&g_FrontendStateTickSpinLock);
  uVar21 = (*(code *)g_GraphicsTextureSetLoadPackageCf)(u_gfx_texturen_zentrale_gfx_00545acc);
  pGVar3 = (GraphicsPaletteAsset *)uVar21;
  if ((!(bool)uVar19) &&
     (g_FrontendCentralTextureSet = (RomAssetHeader *)(GraphicsPaletteAsset *)uVar21,
     pGVar3 = (*g_GraphicsPaletteAssetLoadPackage)
                        (arg0_00,(dword)((ulonglong)uVar21 >> 0x20),
                         (word *)u_gfx_texturen_zentrale_pal_00545b00), !(bool)uVar19)) {
    g_FrontendCentralPaletteAsset = (RomAssetHeader *)pGVar3;
    pwVar4 = TextResource_Resolve(0x2104);
    uVar21 = RichTextCommandStream_PatchPayloadBySelector
                       (0,&g_FrontendNetworkEndpointTextUtf16,pwVar4);
    uVar12 = (undefined4)((ulonglong)uVar21 >> 0x20);
    u_sound_menue01_sam_00545b54[0xb] = L'0';
    u_sound_menue01_sam_00545b54[0xc] = L'1';
    puVar17 = &g_FrontendMenuSoundVoiceSetLoadBaseEntry1;
    uVar8 = extraout_ECX;
    do {
      do {
        uVar21 = Resource_Load(uVar8,uVar12,(word *)u_sound_menue01_sam_00545b54);
        arg0 = (SoundSampleAsset *)uVar21;
        bVar20 = true;
        if ((bool)uVar19) goto Frontend_Init_ContinueWithCentralRomAndRuntimeInitialization;
        pGVar3 = (GraphicsPaletteAsset *)(*g_SoundCreateSampleVoiceSet)(arg0);
        if ((bool)uVar19) {
          LOCK();
          UNLOCK();
          Resource_Release(arg0);
          goto Frontend_Init_ReturnInitializationFailure;
        }
        *puVar17 = pGVar3;
        Resource_Release(arg0);
        u_sound_menue01_sam_00545b54[0xc] = u_sound_menue01_sam_00545b54[0xc] + L'\x01';
        puVar17 = puVar17 + 1;
        uVar19 = (ushort)u_sound_menue01_sam_00545b54[0xc] < 0x39;
        uVar8 = extraout_ECX_00;
        uVar12 = extraout_EDX;
      } while ((ushort)u_sound_menue01_sam_00545b54[0xc] < 0x3a);
      u_sound_menue01_sam_00545b54[0xb] = u_sound_menue01_sam_00545b54[0xb] + L'\x01';
      u_sound_menue01_sam_00545b54[0xc] = L'0';
      uVar19 = (ushort)u_sound_menue01_sam_00545b54[0xb] < 0x39;
      bVar20 = (bool)uVar19;
    } while ((ushort)u_sound_menue01_sam_00545b54[0xb] < 0x3a);
Frontend_Init_ContinueWithCentralRomAndRuntimeInitialization:
    pGVar3 = Package_LoadEntry((word *)(u_Tengine_zentrale_rom_00545aa2 + 1));
    if (((!bVar20) &&
        (g_FrontendCentralRomAsset = (RomAssetHeader *)pGVar3,
        pGVar3 = (GraphicsPaletteAsset *)RomAsset_PrepareRecords((RomAssetHeader *)pGVar3), !bVar20)
        ) && (pGVar3 = (*g_MemoryApi.alloc)(0x10000), !bVar20)) {
      bVar20 = false;
      g_FrontendWorldObjectRecords = (WorldObjectRecord *)pGVar3;
      for (iVar9 = 0x4000; iVar9 != 0; iVar9 = iVar9 + -1) {
        (((GeneratedAssetRecordCountHeader *)&pGVar3->magic)->common).magic = 0;
        pGVar3 = (GraphicsPaletteAsset *)&pGVar3->allocationSizeBytes;
      }
      root = (*g_MemoryApi.alloc)(0x5954);
      pGVar3 = root;
      if (!bVar20) {
        worldRuntime = (WorldRuntimeContext *)(root[1].reservedB4_1FF + 0xac);
        pAVar14 = &g_FrontendRootInitializationTemplate;
        g_FrontendRootNode = (RomAssetHeader *)root;
        for (uVar10 = extraout_ECX_01 >> 2; uVar10 != 0; uVar10 = uVar10 - 1) {
          (((GeneratedAssetRecordCountHeader *)&pGVar3->magic)->common).magic = *pAVar14;
          pAVar14 = pAVar14 + 1;
          pGVar3 = (GraphicsPaletteAsset *)&pGVar3->allocationSizeBytes;
        }
        FrontendMenu_BindSharedResources(root);
        UiRootStack_Push(&g_UiRootCallbacks_0053DA70,(UiRootNode *)root);
        dVar2 = PersistentSettings_ReadDword(3,0x20);
        uVar19 = false;
        pIVar5 = g_FrontendMusicActiveBuffer;
        if ((dVar2 & 2) != 0) {
          uVar21 = Resource_Load(extraout_ECX_02,extraout_EDX_00,
                                 (word *)u_sound_music00_sam_00545c4e);
          pIVar5 = g_FrontendMusicActiveBuffer;
          if (!(bool)uVar19) {
            arg2 = (*g_SoundCreateSampleVoiceSet)((SoundSampleAsset *)uVar21);
            if ((bool)uVar19) {
              Resource_Release(allocation);
              pIVar5 = g_FrontendMusicActiveBuffer;
            }
            else {
              g_FrontendMusicVoiceSet = arg2;
              Resource_Release(allocation);
              dVar2 = PersistentSettings_ReadDword(0x8000,0x2c);
              pIVar5 = (*g_SoundPlayLooping)(dVar2,dVar2,arg2);
              if ((bool)uVar19) {
                (*g_SoundReleaseSampleVoiceSet)(arg2);
                g_FrontendMusicVoiceSet = (DirectSoundVoiceSet *)0x0;
                pIVar5 = g_FrontendMusicActiveBuffer;
              }
            }
          }
        }
        g_FrontendMusicActiveBuffer = pIVar5;
        dVar2 = g_NetworkBackendInstanceCount;
        pdVar13 = g_FrontendTaskAssignmentControlOffsets.primaryAndPadding.offsets;
        if (g_NetworkBackendInstanceCount != 0) {
          pwVar4 = g_NetworkBackendInstanceTable->displayNameUtf16;
          dVar11 = g_NetworkBackendInstanceCount;
          do {
            pdVar13 = pdVar13 + 1;
            *pdVar13 = (dword)pwVar4;
            pwVar4 = pwVar4 + 0x80;
            dVar11 = dVar11 - 1;
          } while (dVar11 != 0);
          UiPointerList_InitializeMeasuredTextRows
                    (dVar2,(void **)(g_FrontendTaskAssignmentControlOffsets.primaryAndPadding.
                                     offsets + 1),
                     (UiPointerListControl *)(root[0x24].reservedB4_1FF + 0x9c));
        }
        *(code **)((int)root[2].reserved08_AF + 0x50) =
             FrontendRuntime_DispatchCommandByCodeAndModifierFlagsCf;
        *(code **)((int)root[2].reserved08_AF + 0x54) =
             FrontendRuntime_UpdatePointerContextAndSceneViewCf;
        *(code **)((int)root[2].reserved08_AF + 0x58) =
             FrontendRuntime_UpdatePointerContextAndSceneViewCf;
        *(code **)((int)root[2].reserved08_AF + 0x5c) = FrontendRuntimeCallback5C_NoOp;
        *(code **)((int)root[2].reserved08_AF + 0x60) = FrontendRuntimeCallback60_NoOp;
        *(code **)((int)root[2].reserved08_AF + 100) = FrontendRuntimeCallback64_DispatchRecord1350;
        *(AssetPackedDate *)root[2].reserved08_AF = 0;
        *(code **)((int)root[2].reserved08_AF + 0x68) =
             FrontendRuntimeCallback68_DispatchRefresh1340;
        *(dword **)((int)root[2].reserved08_AF + 0x20) = &g_FrontendStateTickSpinLock;
        *(code **)((int)root[2].reserved08_AF + 0x24) = Frontend_StateTick;
        RecentTextHistory_SortAndBuildPointerList
                  (5,(RecentTextHistoryPointerList *)(root[1].reservedB4_1FF + 0x94));
        uVar19 = 0;
        WorldRuntime_SetTerrainLightingConfiguration(0,0,0xffffffff,0,0,0,0,0,worldRuntime);
        WorldRuntime_AttachObjectArray(0x100,g_FrontendWorldObjectRecords,worldRuntime);
        pGVar3 = (GraphicsPaletteAsset *)
                 RomRuntime_BuildAllRegistryNodeTrees(extraout_ECX_03,extraout_EDX_01,worldRuntime);
        if ((!(bool)uVar19) &&
           (pGVar3 = (GraphicsPaletteAsset *)
                     FrontendRomTransition_ActivateRecordByIdCf(param_3,worldRuntime), !(bool)uVar19
           )) {
          puVar17 = PersistentSettings_GetRegionOrFallback
                              (0x28,&g_FrontendLocalPlayerNameUtf16,0x60);
          puVar15 = puVar17;
          pbVar18 = root[0x26].reservedB4_1FF + 0xd0;
          for (iVar9 = 10; iVar9 != 0; iVar9 = iVar9 + -1) {
            *(undefined4 *)pbVar18 = *puVar15;
            puVar15 = puVar15 + 1;
            pbVar18 = pbVar18 + 4;
          }
          puVar15 = &g_FrontendLocalPlayerNameUtf16;
          for (iVar9 = 10; iVar9 != 0; iVar9 = iVar9 + -1) {
            *puVar15 = *puVar17;
            puVar17 = puVar17 + 1;
            puVar15 = puVar15 + 1;
          }
          puVar17 = PersistentSettings_GetRegionOrFallback
                              (0x28,&g_FrontendLocalPlayerNameUtf16,0x88);
          pbVar18 = root[0x27].reservedB4_1FF + 0xd4;
          for (iVar9 = 10; iVar9 != 0; iVar9 = iVar9 + -1) {
            *(undefined4 *)pbVar18 = *puVar17;
            puVar17 = puVar17 + 1;
            pbVar18 = pbVar18 + 4;
          }
          AVar6 = PersistentSettings_ReadDword(4,0x3c);
          root[0x28].magic = AVar6;
          UiFrame_FlushInputAndResetPendingTicks();
          (*g_SpinLockAcquire)(&g_FrontendStateTickSpinLock);
          WorldMotionSpline_ClearCachedDerivatives();
          (*g_TimerRegisterPeriodic)(0x100,FrontendRuntime_IncrementActiveTickCounter);
          if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_NETWORKED_MASK) ==
              SESSION_NETWORK_ROLE_LOCAL) {
            FrontendPlayerRuntime_RecordReadyAndUpdateWaitState(g_LocalPlayerRuntimeId,0,0,0);
          }
          else {
            FrontendCommandQueue_EnqueueLocalPlayerCommand(0xd0,0,0,0);
          }
          (*g_SpinLockRelease)(&g_FrontendStateTickSpinLock);
          do {
            UiNode_InvalidateRoot((UiNodeBase *)root);
            UiFrame_Update(0);
            UiFrame_Draw();
            (*g_GraphicsFramebufferPresent)(g_FramebufferAccess);
            Frontend_StateTick();
          } while ((g_FrontendRuntimeFlags & 0x10) != 0);
          (*g_GraphicsCursorSetFrame)(0);
          UiFrame_FlushInputAndResetPendingTicks();
          return CONCAT44(param_2,root);
        }
      }
    }
  }
Frontend_Init_ReturnInitializationFailure:
  return CONCAT44(param_2,pGVar3);
}

/* Address: 0x00547630.
   Ownership: ui/frontend/runtime.
   Purpose: Table 00547660: 00547680, 005476B0, 00547700, 00547750, 005477A0, 005477F0.
   Local calls: FrontendDebugOverlay_RefreshCountersAndWorldCoordinates.
   Cross-module calls: UiTransfer_SendPacketType10000Value2931Cf [network/protocol/transfer],
   UiRuntimeRecordRing_DiscardOldestCf [ui/core/runtime], FrontendTransfer_HandleSessionListAndJoinAckPackets
   [network/protocol/transfer], FrontendTransfer_PublishHostSessionAndDispatchQueuedCommands
   [network/protocol/transfer], FrontendTransfer_HandleLobbyDiscoveryAndPlayerPackets [network/protocol/transfer],
   FrontendTransfer_SendPacket10006 [network/protocol/transfer].
*/
undefined8 Frontend_StateTick(void)

{
  FrontendRootRuntimeAddress32 frontendRuntime;
  undefined4 extraout_ECX;
  undefined4 extraout_ECX_00;
  undefined4 extraout_ECX_01;
  undefined4 extraout_ECX_02;
  uint extraout_ECX_03;
  undefined4 extraout_ECX_04;
  uint extraout_ECX_05;
  uint uVar1;
  UiTransferEndpointDescriptor *pUVar2;
  undefined4 extraout_EDX;
  undefined4 uVar3;
  undefined4 unaff_EBX;
  undefined4 unaff_EDI;
  undefined1 in_CF;
  undefined1 uVar4;
  undefined8 uVar5;
  
  (*g_SpinLockTryAcquire)(&g_FrontendStateTickSpinLock);
  uVar1 = g_FrontendNetworkTickCounter;
  frontendRuntime = g_FrontendRootNode;
  if ((bool)in_CF) goto Frontend_StateTick_ReturnAfterReleaseOrLockBusy;
                    
  switch(g_FrontendNetworkState) {
  case 0:
    if (g_FrontendTimerCountdownTicks == 0) {
      g_FrontendNetworkTickCounter = g_FrontendNetworkTickCounter + 1;
      g_FrontendTimerCountdownTicks = 4;
      uVar3 = g_FrontendNetworkState;
Frontend_StateTick_RefreshDebugOverlayAfterNetworkStateWork:
      if ((g_FrontendRuntimeFlags & 0x10) == 0) {
        FrontendDebugOverlay_RefreshCountersAndWorldCoordinates(uVar1 & 7,uVar3);
      }
    }
    break;
  case 1:
    if (g_FrontendTimerCountdownTicks == 0) {
      g_FrontendNetworkTickCounter = g_FrontendNetworkTickCounter + 1;
      g_FrontendTimerCountdownTicks = 4;
      uVar4 = 0;
      if ((uVar1 & 0xf) != 0) {
        UiTransfer_SendPacketType10000Value2931Cf();
      }
      while( true ) {
        uVar5 = UiRuntimeRecordRing_DiscardOldestCf();
        pUVar2 = (UiTransferEndpointDescriptor *)((ulonglong)uVar5 >> 0x20);
        if ((bool)uVar4) break;
        FrontendTransfer_HandleSessionListAndJoinAckPackets
                  (pUVar2,(FrontendTransferPacketUnion *)uVar5,frontendRuntime);
      }
      FrontendDebugOverlay_RefreshCountersAndWorldCoordinates(extraout_ECX,pUVar2);
    }
    break;
  case 2:
    uVar4 = 0;
    if (g_FrontendTimerCountdownTicks == 0) {
      g_FrontendNetworkTickCounter = g_FrontendNetworkTickCounter + 1;
      g_FrontendTimerCountdownTicks = 4;
      FrontendTransfer_PublishHostSessionAndDispatchQueuedCommands(g_FrontendRootNode);
      while( true ) {
        uVar5 = UiRuntimeRecordRing_DiscardOldestCf();
        pUVar2 = (UiTransferEndpointDescriptor *)((ulonglong)uVar5 >> 0x20);
        if ((bool)uVar4) break;
        FrontendTransfer_HandleLobbyDiscoveryAndPlayerPackets
                  (pUVar2,(FrontendTransferPacketUnion *)uVar5,frontendRuntime);
      }
      FrontendDebugOverlay_RefreshCountersAndWorldCoordinates(extraout_ECX_00,pUVar2);
    }
    break;
  case 3:
    if (g_FrontendTimerCountdownTicks == 0) {
      g_FrontendNetworkTickCounter = g_FrontendNetworkTickCounter + 1;
      g_FrontendTimerCountdownTicks = 4;
      uVar4 = 0;
      if ((uVar1 & 0xf) != 0) {
        FrontendTransfer_SendPacket10006(uVar1,g_FrontendNetworkState);
      }
      while( true ) {
        uVar5 = UiRuntimeRecordRing_DiscardOldestCf();
        pUVar2 = (UiTransferEndpointDescriptor *)((ulonglong)uVar5 >> 0x20);
        if ((bool)uVar4) break;
        FrontendTransfer_HandleHostSessionAndCommandBatchPackets
                  (pUVar2,(FrontendTransferPacketUnion *)uVar5,frontendRuntime);
      }
      FrontendDebugOverlay_RefreshCountersAndWorldCoordinates(extraout_ECX_01,pUVar2);
    }
    break;
  case 4:
    uVar4 = 0;
    if (g_FrontendTimerCountdownTicks == 0) {
      g_FrontendNetworkTickCounter = g_FrontendNetworkTickCounter + 1;
      g_FrontendTimerCountdownTicks = 4;
      while( true ) {
        uVar5 = UiRuntimeRecordRing_DiscardOldestCf();
        uVar3 = (undefined4)((ulonglong)uVar5 >> 0x20);
        if ((bool)uVar4) break;
        FrontendNetwork_HandleHandshakeAndPlayerStatePackets
                  (extraout_ECX_02,uVar3,(UiTransferEndpointDescriptor *)uVar3,
                   (FrontendTransferPacketUnion *)uVar5);
      }
      uVar5 = FrontendNetwork_HostTickCommandAndSnapshotTransfer(extraout_ECX_02,uVar3);
      uVar1 = extraout_ECX_03;
      uVar3 = (int)((ulonglong)uVar5 >> 0x20);
      if (!(bool)uVar4) goto Frontend_StateTick_RefreshDebugOverlayAfterNetworkStateWork;
      g_FrontendTimerCountdownTicks = 1;
    }
    break;
  case 5:
    UiRuntimeRecordRing_ContainsIdCf(g_FrontendSessionToken);
    if ((bool)in_CF) {
      g_FrontendNetworkTickCounter = g_FrontendNetworkTickCounter + 1;
      uVar4 = 1;
      do {
        uVar5 = UiRuntimeRecordRing_DiscardOldestCf();
        uVar3 = (undefined4)((ulonglong)uVar5 >> 0x20);
        if ((bool)uVar4) break;
        FrontendTransfer_HandleGameplayCommandAndRosterPacketsCf
                  (extraout_ECX_04,uVar3,(UiTransferEndpointDescriptor *)uVar3,
                   (FrontendTransferPacketUnion *)uVar5);
      } while (!(bool)uVar4);
      FrontendTransfer_ConsumeProcessedFlagFrontendCf();
      uVar1 = extraout_ECX_05;
      uVar3 = extraout_EDX;
      if (!(bool)uVar4) goto Frontend_StateTick_RefreshDebugOverlayAfterNetworkStateWork;
    }
  }
  (*g_SpinLockRelease)(&g_FrontendStateTickSpinLock);
Frontend_StateTick_ReturnAfterReleaseOrLockBusy:
  return CONCAT44(unaff_EDI,unaff_EBX);
}

/* Address: 0x00543B70.
   Ownership: ui/frontend/runtime.
   Purpose: Loads gfx\panel\menue.gfx, stores it as the shared frontend-menu texture source, assigns it to verified
   menu controls, and binds the preloaded button sound voice sets across the frontend UI.
*/
void FrontendMenu_BindSharedResources(void *frontendUiState)

{
  undefined *puVar1;
  undefined *puVar2;
  GraphicsTextureSourceAsset *pGVar3;
  dword in_ECX;
  int iVar4;
  dword in_EDX;
  undefined1 in_CF;
  
  pGVar3 = (*g_GraphicsTextureSourceLoadPackageAsset)
                     (in_ECX,in_EDX,(word *)u_gfx_panel_menue_gfx_00545b78);
  if (!(bool)in_CF) {
    g_FrontendMenuTextureSource = pGVar3;
    *(GraphicsTextureSourceAsset **)((int)frontendUiState + 0x485c) = pGVar3;
    *(GraphicsTextureSourceAsset **)((int)frontendUiState + 0x4f30) = pGVar3;
    *(GraphicsTextureSourceAsset **)((int)frontendUiState + 0x53d8) = pGVar3;
    *(GraphicsTextureSourceAsset **)((int)frontendUiState + 0x5720) = pGVar3;
    *(GraphicsTextureSourceAsset **)((int)frontendUiState + 0x2670) = pGVar3;
    *(GraphicsTextureSourceAsset **)((int)frontendUiState + 0x2d0c) = pGVar3;
    *(GraphicsTextureSourceAsset **)((int)frontendUiState + 0x3738) = pGVar3;
    *(GraphicsTextureSourceAsset **)((int)frontendUiState + 0x3e64) = pGVar3;
    *(GraphicsTextureSourceAsset **)((int)frontendUiState + 0x24f8) = pGVar3;
    *(GraphicsTextureSourceAsset **)((int)frontendUiState + 0x1c8c) = pGVar3;
    *(GraphicsTextureSourceAsset **)((int)frontendUiState + 0xae4) = pGVar3;
    *(GraphicsTextureSourceAsset **)((int)frontendUiState + 0x5e0) = pGVar3;
    puVar1 = g_UiButtonSoundVoiceSets7[3];
    *(undefined **)((int)frontendUiState + 0x644) = g_UiButtonSoundVoiceSets7[3];
    *(undefined **)((int)frontendUiState + 0x764) = puVar1;
    *(undefined **)((int)frontendUiState + 0x6a4) = puVar1;
    *(undefined **)((int)frontendUiState + 0x704) = puVar1;
    *(undefined **)((int)frontendUiState + 0xb48) = puVar1;
    *(undefined **)((int)frontendUiState + 0xba8) = puVar1;
    *(undefined **)((int)frontendUiState + 0xc68) = puVar1;
    *(undefined **)((int)frontendUiState + 0x1cf0) = puVar1;
    *(undefined **)((int)frontendUiState + 0x1d50) = puVar1;
    *(undefined **)((int)frontendUiState + 0x1db0) = puVar1;
    *(undefined **)((int)frontendUiState + 0x1e10) = puVar1;
    *(undefined **)((int)frontendUiState + 0x1e70) = puVar1;
    *(undefined **)((int)frontendUiState + 0x255c) = puVar1;
    *(undefined **)((int)frontendUiState + 0x25bc) = puVar1;
    *(undefined **)((int)frontendUiState + 0x26d4) = puVar1;
    *(undefined **)((int)frontendUiState + 0x2790) = puVar1;
    *(undefined **)((int)frontendUiState + 0x27f0) = puVar1;
    *(undefined **)((int)frontendUiState + 0x2850) = puVar1;
    *(undefined **)((int)frontendUiState + 0x2d70) = puVar1;
    *(undefined **)((int)frontendUiState + 0x2dd0) = puVar1;
    *(undefined **)((int)frontendUiState + 0x379c) = puVar1;
    *(undefined **)((int)frontendUiState + 0x3ec8) = puVar1;
    *(undefined **)((int)frontendUiState + 0x491c) = puVar1;
    *(undefined **)((int)frontendUiState + 0x497c) = puVar1;
    *(undefined **)((int)frontendUiState + 0x49dc) = puVar1;
    *(undefined **)((int)frontendUiState + 0x4ff0) = puVar1;
    *(undefined **)((int)frontendUiState + 0x5050) = puVar1;
    *(undefined **)((int)frontendUiState + 0x5498) = puVar1;
    *(undefined **)((int)frontendUiState + 0x54f8) = puVar1;
    *(undefined **)((int)frontendUiState + 0x5558) = puVar1;
    *(undefined **)((int)frontendUiState + 0x57e0) = puVar1;
    puVar1 = g_UiButtonSoundVoiceSets7[4];
    *(undefined **)((int)frontendUiState + 0x2ae0) = g_UiButtonSoundVoiceSets7[4];
    *(undefined **)((int)frontendUiState + 0x2b40) = puVar1;
    *(undefined **)((int)frontendUiState + 0x2bf4) = puVar1;
    *(undefined **)((int)frontendUiState + 0x2c54) = puVar1;
    *(undefined **)((int)frontendUiState + 0x2cb4) = puVar1;
    *(undefined **)((int)frontendUiState + 12000) = puVar1;
    *(undefined **)((int)frontendUiState + 0x2f48) = puVar1;
    *(undefined **)((int)frontendUiState + 0x2fb0) = puVar1;
    *(undefined **)((int)frontendUiState + 0x3018) = puVar1;
    *(undefined **)((int)frontendUiState + 0x3080) = puVar1;
    *(undefined **)((int)frontendUiState + 0x313c) = puVar1;
    *(undefined **)((int)frontendUiState + 0x31a4) = puVar1;
    *(undefined **)((int)frontendUiState + 0x320c) = puVar1;
    *(undefined **)((int)frontendUiState + 0x3274) = puVar1;
    *(undefined **)((int)frontendUiState + 0x32dc) = puVar1;
    *(undefined **)((int)frontendUiState + 0x3344) = puVar1;
    *(undefined **)((int)frontendUiState + 0x33ac) = puVar1;
    *(undefined **)((int)frontendUiState + 0x3414) = puVar1;
    *(undefined **)((int)frontendUiState + 0x347c) = puVar1;
    *(undefined **)((int)frontendUiState + 0x34e4) = puVar1;
    *(undefined **)((int)frontendUiState + 0x35a0) = puVar1;
    *(undefined **)((int)frontendUiState + 0x3608) = puVar1;
    *(undefined **)((int)frontendUiState + 0x3670) = puVar1;
    *(undefined **)((int)frontendUiState + 0x36d8) = puVar1;
    *(undefined **)((int)frontendUiState + 0x3858) = puVar1;
    *(undefined **)((int)frontendUiState + 0x390c) = puVar1;
    *(undefined **)((int)frontendUiState + 0x3974) = puVar1;
    *(undefined **)((int)frontendUiState + 0x39dc) = puVar1;
    *(undefined **)((int)frontendUiState + 0x3a44) = puVar1;
    *(undefined **)((int)frontendUiState + 0x3aac) = puVar1;
    *(undefined **)((int)frontendUiState + 0x3b14) = puVar1;
    *(undefined **)((int)frontendUiState + 0x3d4c) = puVar1;
    *(undefined **)((int)frontendUiState + 0x3dac) = puVar1;
    *(undefined **)((int)frontendUiState + 0x3e0c) = puVar1;
    *(undefined **)((int)frontendUiState + 0x3f84) = puVar1;
    *(undefined **)((int)frontendUiState + 0x3fe4) = puVar1;
    *(undefined **)((int)frontendUiState + 0x4044) = puVar1;
    *(undefined **)((int)frontendUiState + 0x28b0) = puVar1;
    iVar4 = 7;
    do {
      *(undefined **)
       (g_FrontendTaskAssignmentControlOffsets.factionControls.offsets[iVar4] + 0x5c +
       (int)frontendUiState) = puVar1;
      *(undefined **)
       (g_FrontendTaskAssignmentControlOffsets.playerControls.offsets[iVar4] + 0x5c +
       (int)frontendUiState) = puVar1;
      *(undefined **)
       (g_FrontendTaskAssignmentControlOffsets.selectionRows.offsets[iVar4] + 0x5c +
       (int)frontendUiState) = puVar1;
      puVar2 = g_UiButtonSoundVoiceSets7[5];
      iVar4 = iVar4 + -1;
    } while (iVar4 != 0);
    *(undefined **)((int)frontendUiState + 0x3c98) = g_UiButtonSoundVoiceSets7[5];
    *(undefined **)((int)frontendUiState + 0x41c0) = puVar2;
    *(undefined **)((int)frontendUiState + 0x433c) = puVar2;
    *(undefined **)((int)frontendUiState + 0x44b8) = puVar2;
    *(undefined **)((int)frontendUiState + 0x4634) = puVar2;
    *(undefined **)((int)frontendUiState + 0x514c) = puVar2;
    *(undefined **)((int)frontendUiState + 0x5210) = puVar2;
    *(undefined **)((int)frontendUiState + 0xa8c) = puVar2;
    puVar1 = g_UiButtonSoundVoiceSets7[6];
    *(undefined **)((int)frontendUiState + 0x2024) = g_UiButtonSoundVoiceSets7[6];
    *(undefined **)((int)frontendUiState + 0x21ec) = puVar1;
    *(undefined **)((int)frontendUiState + 0x23cc) = puVar1;
    *(undefined **)((int)frontendUiState + 0x4ad4) = puVar1;
    *(undefined **)((int)frontendUiState + 0x4bd0) = puVar1;
    *(undefined **)((int)frontendUiState + 0x5654) = puVar1;
    *(undefined **)((int)frontendUiState + 0x4dc4) = puVar1;
    *(undefined **)((int)frontendUiState + 0x4eb0) = puVar1;
    *(undefined **)((int)frontendUiState + 0x50bc) = puVar1;
  }
  return;
}

/* Address: 0x005445A0.
   Ownership: ui/frontend/runtime.
   Purpose: Typed parameters: p3 selectionIndex→FrontendFactionAssignmentIndex_V306. Nearby but non-identical
   semantic domains were explicitly deferred. Calling convention, parameter storage, body bytes, control flow,
   globals, locals, and executable data remain unchanged. Typed parameters: p0
   argument1→FrontendIndexedSelectionArgument_V344. Calling convention, complete VariableStorage serialization,
   function bytes, control flow, globals, locals, and executable data remain unchanged.
*/
void FrontendUiAction2044_IndexedSelectionHelper
               (FrontendIndexedSelectionArgument argument1,dword argument2,dword argument3,
               FrontendFactionAssignmentIndex selectionIndex)

{
  uint nextSelectionTextId;
  dword playerRecordsRemaining;
  FrontendPlayerRuntimeRecord *playerRecordCursor;
  int selectionControlAddress;
  int factionAssetRecordAddress;
  int selectionTextCycleLength;
  int *selectionCycleCounterField;
  
  selectionTextCycleLength = 7;
  playerRecordsRemaining = g_FrontendPlayerRuntimeBlockCount;
  playerRecordCursor = g_FrontendPlayerRuntimeBlocks;
  do {
    if (argument1 == playerRecordCursor->playerRuntimeId) {
      if ((playerRecordCursor->runtimeState64 & 1) != 0) {
        selectionTextCycleLength = 8;
      }
      selectionControlAddress =
           g_FrontendRootNode +
           g_FrontendTaskAssignmentControlOffsets.factionControls.offsets[selectionIndex + 1];
      factionAssetRecordAddress =
           *(int *)(selectionIndex * 4 + 0x531064) + g_FrontendLoadedLevelAsset;
      nextSelectionTextId = *(int *)(selectionControlAddress + 0x54) + 1;
      selectionCycleCounterField = (int *)(factionAssetRecordAddress + 0x21c);
      *selectionCycleCounterField = *selectionCycleCounterField + 1;
      if (selectionTextCycleLength + 0x2174U <= nextSelectionTextId) {
        nextSelectionTextId = 0x2174;
        *(int *)(factionAssetRecordAddress + 0x21c) =
             *(int *)(factionAssetRecordAddress + 0x21c) - selectionTextCycleLength;
      }
      *(uint *)(selectionControlAddress + 0x54) = nextSelectionTextId;
      return;
    }
    playerRecordCursor = playerRecordCursor + 1;
    playerRecordsRemaining = playerRecordsRemaining - 1;
  } while (playerRecordsRemaining != 0);
  return;
}

/* Address: 0x00544640.
   Ownership: ui/frontend/runtime.
   Purpose: Typed parameters: p3 selectionIndex→FrontendFactionAssignmentIndex_V306. Nearby but non-identical
   semantic domains were explicitly deferred. Calling convention, parameter storage, body bytes, control flow,
   globals, locals, and executable data remain unchanged.
   Cross-module calls: FrontendTaskAssignmentPage_RefreshFactionAndPlayerControls [ui/frontend/settings].
*/
void FrontendUiAction2045_IndexedSelectionHelper
               (dword argument1,dword argument2,dword argument3,
               FrontendFactionAssignmentIndex selectionIndex)

{
  FactionRuntimeLifecycleObservedState *pFVar1;
  FrontendPlayerRuntimeBlockCount FVar2;
  FrontendPlayerRuntimeRecord *pFVar3;
  
  FVar2 = g_FrontendPlayerRuntimeBlockCount;
  pFVar3 = g_FrontendPlayerRuntimeBlocks;
  do {
    if (selectionIndex + 1 == (pFVar3->factionAssignment).factionAssignmentIndex) {
      return;
    }
    pFVar3 = pFVar3 + 1;
    FVar2 = FVar2 - 1;
  } while (FVar2 != 0);
  pFVar1 = g_GameFactionRuntimeImage.tail.factionLifecycleStates + selectionIndex + 1;
  *pFVar1 = *pFVar1 ^ FACTION_RUNTIME_LIFECYCLE_ACTIVE;
  FrontendTaskAssignmentPage_RefreshFactionAndPlayerControls(selectionIndex,0,g_FrontendRootNode);
  return;
}

/* Address: 0x005446A0.
   Ownership: ui/frontend/runtime.
   Purpose: Typed parameters: p3 selectionIndex→FrontendFactionAssignmentIndex_V306. Nearby but non-identical
   semantic domains were explicitly deferred. Calling convention, parameter storage, body bytes, control flow,
   globals, locals, and executable data remain unchanged. Typed parameters: p0
   argument1→FrontendIndexedSelectionArgument_V344. Calling convention, complete VariableStorage serialization,
   function bytes, control flow, globals, locals, and executable data remain unchanged.
   Cross-module calls: UiSelectableGroup_SelectExclusive [ui/controls/lists],
   FrontendTaskAssignmentPage_RefreshFactionAndPlayerControls [ui/frontend/settings].
*/
void FrontendUiAction2046_IndexedSelectionHelper
               (FrontendIndexedSelectionArgument argument1,dword argument2,dword argument3,
               FrontendFactionAssignmentIndex selectionIndex)

{
  SessionNetworkRoleFlags SVar1;
  SessionNetworkRoleFlags SVar2;
  dword selectionIndex_00;
  SessionNetworkRoleFlags generationCursor;
  SessionNetworkRoleFlags extraout_ECX;
  UiNodeBase *selectedControl;
  FrontendPlayerRuntimeRecord *pFVar3;
  FrontendPlayerRuntimeRecord *pFVar4;
  
  selectedControl =
       (UiNodeBase *)
       ((int)&(g_FrontendRootNode->base).nextSibling +
       g_FrontendTaskAssignmentControlOffsets.selectionRows.offsets[selectionIndex + 1]);
  generationCursor = 7;
  if (((uint)selectedControl[1].nextSibling & 0x400) == 0) {
    if (argument1 == g_LocalPlayerRuntimeId) {
      do {
        generationCursor = generationCursor - SESSION_NETWORK_ROLE_CLIENT;
      } while (generationCursor != SESSION_NETWORK_ROLE_LOCAL);
      UiSelectableGroup_SelectExclusive(7,selectedControl);
      generationCursor = extraout_ECX;
    }
    selectionIndex_00 = g_FrontendFactionAssignmentReadyStateGeneration;
    SVar1 = g_FrontendPlayerRuntimeBlockCount;
    SVar2 = g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_NETWORKED_MASK;
    for (pFVar3 = g_FrontendPlayerRuntimeBlocks;
        (pFVar4 = g_FrontendPlayerRuntimeBlocks, SVar2 != SESSION_NETWORK_ROLE_LOCAL &&
        (generationCursor = SVar1, pFVar4 = pFVar3, argument1 != pFVar3->playerRuntimeId));
        pFVar3 = pFVar3 + 1) {
      generationCursor = SVar1 - SESSION_NETWORK_ROLE_CLIENT;
      SVar1 = generationCursor;
      SVar2 = generationCursor;
    }
    (pFVar4->factionAssignment).factionAssignmentIndex = selectionIndex + 1;
    (pFVar4->factionAssignment).readyOrWaitState = selectionIndex_00;
    g_FrontendFactionAssignmentReadyStateGeneration =
         g_FrontendFactionAssignmentReadyStateGeneration + 1;
    FrontendTaskAssignmentPage_RefreshFactionAndPlayerControls
              (generationCursor,selectionIndex_00,g_FrontendRootNode);
  }
  return;
}

/* Address: 0x00546190.
   Ownership: ui/frontend/runtime.
   Purpose: Refreshes debug-overlay UTF-16 fields for render counters, world vectors, cursor coordinates, and free
   arena bytes. The four render counters are sampled every twenty calls and then cleared.
   Cross-module calls: WideNumber_FormatUtf16 [core/text/string], WorldRuntime_GetVector0Regs [world/runtime/core],
   WorldRuntime_GetVector1Regs [world/runtime/core].
*/
void __fastcall
FrontendDebugOverlay_RefreshCountersAndWorldCoordinates(undefined4 param_1,undefined4 param_2)

{
  dword value;
  WideNumberSignedValue32 value_00;
  WideNumberSignedValue32 value_01;
  WideNumberDenominator32 extraout_EDX;
  WideNumberDenominator32 denominator;
  WideNumberDenominator32 denominator_00;
  WideNumberDenominator32 denominator_01;
  WideNumberSignedValue32 value_02;
  WideNumberSignedValue32 value_03;
  WorldRuntimeContext *world;
  undefined8 uVar1;
  
  g_DebugOverlayCounterRefreshCountdown = g_DebugOverlayCounterRefreshCountdown - 1;
  if (g_DebugOverlayCounterRefreshCountdown == 0) {
    g_DebugOverlayCounterRefreshCountdown = 0x14;
    WideNumber_FormatUtf16
              (WIDE_FORMAT_WRITE_TERMINATOR,0,10,1,g_RenderedFrameCountSinceDebugRefresh,
               g_FrontendDebugOverlayTextSlot00Utf16);
    denominator = extraout_EDX;
    if (extraout_EDX == 0) {
      denominator = 1;
    }
    WideNumber_FormatUtf16
              (WIDE_FORMAT_WRITE_TERMINATOR|WIDE_FORMAT_FIXED_FRACTION_WIDTH,2,10,denominator,
               g_PrimitiveDrawCallCount,g_FrontendDebugOverlayTextSlot01Utf16);
    WideNumber_FormatUtf16
              (WIDE_FORMAT_WRITE_TERMINATOR|WIDE_FORMAT_FIXED_FRACTION_WIDTH,2,10,denominator_00,
               g_TextureBindStateChangeCount,g_FrontendDebugOverlayTextSlot02Utf16);
    WideNumber_FormatUtf16
              (WIDE_FORMAT_WRITE_TERMINATOR|WIDE_FORMAT_FIXED_FRACTION_WIDTH,2,10,denominator_01,
               g_TextureDeviceReloadCount,g_FrontendDebugOverlayTextSlot03Utf16);
    g_RenderedFrameCountSinceDebugRefresh = 0;
    g_PrimitiveDrawCallCount = 0;
    g_TextureBindStateChangeCount = 0;
    g_TextureDeviceReloadCount = 0;
  }
  world = (WorldRuntimeContext *)(g_FrontendRootNode + 0x368);
  uVar1 = WorldRuntime_GetVector0Regs(world);
  WideNumber_FormatUtf16
            (WIDE_FORMAT_GROUP_THOUSANDS|WIDE_FORMAT_WRITE_TERMINATOR|WIDE_FORMAT_SIGNED_VALUE,0,10,
             1,(WideNumberSignedValue32)uVar1,g_FrontendDebugOverlayTextSlot04Utf16);
  WideNumber_FormatUtf16
            (WIDE_FORMAT_GROUP_THOUSANDS|WIDE_FORMAT_WRITE_TERMINATOR|WIDE_FORMAT_SIGNED_VALUE,0,10,
             1,value_00,g_FrontendDebugOverlayTextSlot05Utf16);
  WideNumber_FormatUtf16
            (WIDE_FORMAT_GROUP_THOUSANDS|WIDE_FORMAT_WRITE_TERMINATOR|WIDE_FORMAT_SIGNED_VALUE,0,10,
             1,value_02,g_FrontendDebugOverlayTextSlot06Utf16);
  uVar1 = WorldRuntime_GetVector1Regs(world);
  WideNumber_FormatUtf16
            (WIDE_FORMAT_GROUP_THOUSANDS|WIDE_FORMAT_WRITE_TERMINATOR|WIDE_FORMAT_SIGNED_VALUE,0,10,
             1,(WideNumberSignedValue32)uVar1,g_FrontendDebugOverlayTextSlot07Utf16);
  WideNumber_FormatUtf16
            (WIDE_FORMAT_GROUP_THOUSANDS|WIDE_FORMAT_WRITE_TERMINATOR|WIDE_FORMAT_SIGNED_VALUE,0,10,
             1,value_01,g_FrontendDebugOverlayTextSlot08Utf16);
  WideNumber_FormatUtf16
            (WIDE_FORMAT_GROUP_THOUSANDS|WIDE_FORMAT_WRITE_TERMINATOR|WIDE_FORMAT_SIGNED_VALUE,0,10,
             1,value_03,g_FrontendDebugOverlayTextSlot09Utf16);
  WideNumber_FormatUtf16
            (WIDE_FORMAT_WRITE_TERMINATOR|WIDE_FORMAT_SIGNED_VALUE,0,10,1,g_CursorOverrideX,
             g_FrontendDebugOverlayTextSlot10Utf16);
  WideNumber_FormatUtf16
            (WIDE_FORMAT_WRITE_TERMINATOR|WIDE_FORMAT_SIGNED_VALUE,0,10,1,g_CursorOverrideY,
             g_FrontendDebugOverlayTextSlot11Utf16);
  value = (*g_MemoryApi.queryFreeBytes)();
  WideNumber_FormatUtf16
            (WIDE_FORMAT_WRITE_TERMINATOR|WIDE_FORMAT_HEXADECIMAL,0,10,1,value,
             g_FrontendDebugOverlayTextSlot12Utf16);
  g_FrontendDebugOverlayTextSlot13Utf16[0] = 0;
  return;
}

/* Address: 0x005474E0.
   Ownership: ui/frontend/runtime.
   Purpose: Handles frontend runtime shutdown and release resources register result.
   Cross-module calls: UiRuntime_SetSynchronizationHooks [ui/core/runtime],
   FrontendTeardown_SaveRootStateSnapshot80 [ui/frontend/network], UiRootStack_PopCf [ui/controls/layout],
   FrontendRomRegistry_ClearAndReleaseNestedResources [assets/rom/runtime], Resource_Release
   [assets/resource/runtime], GraphicsShadingRuntime_ClearRecordTable [graphics/render/shading].
*/
undefined8 __cdecl FrontendRuntime_ShutdownAndReleaseResourcesRegs(void)

{
  UiRootNode *root;
  int voiceSetsRemaining;
  int extraout_ECX;
  undefined4 unaff_EBX;
  undefined4 unaff_EDI;
  undefined4 *voiceSetCursor;
  
  UiRuntime_SetSynchronizationHooks
            ((UiRuntimePostUnlockCallbackProc *)0x0,(RuntimeSpinLockValue *)0x0);
  (*g_TimerUnregisterPeriodic)(FrontendRuntime_TimerCountdownTick);
  (*g_TimerUnregisterPeriodic)(FrontendRuntime_IncrementActiveTickCounter);
  root = g_FrontendRootNode;
  g_CursorVisibilityToken = g_CursorVisibilityToken + -1;
  if (g_FrontendRootNode != (UiRootNode *)0x0) {
    FrontendTeardown_SaveRootStateSnapshot80(g_FrontendRootNode);
    UiRootStack_PopCf(root);
    (*g_MemoryApi.free)(root);
    g_FrontendRootNode = (UiRootNode *)0x0;
  }
  FrontendRomRegistry_ClearAndReleaseNestedResources();
  (*g_MemoryApi.free)(g_FrontendWorldObjectRecords);
  g_FrontendWorldObjectRecords = (WorldObjectRecord *)0x0;
  Resource_Release(g_FrontendCentralRomAsset);
  g_FrontendCentralRomAsset = (void *)0x0;
  GraphicsShadingRuntime_ClearRecordTable();
  (*(code *)g_GraphicsTextureSetReleasePackageCf)(g_FrontendCentralTextureSet);
  (*g_GraphicsPaletteAssetLifecycleCallbacks3.releasePackage)(g_FrontendCentralPaletteAsset);
  (*g_GraphicsTextureSourceLifecycleCallbacks3.releasePackage)(g_FrontendMenuTextureSource);
  g_FrontendCentralTextureSet = 0;
  g_FrontendCentralPaletteAsset = (GraphicsPaletteAsset *)0x0;
  g_FrontendMenuTextureSource = (GraphicsTextureSourceAsset *)0x0;
  voiceSetCursor = &g_FrontendMenuSoundVoiceSetTable100;
  voiceSetsRemaining = 100;
  do {
    if ((DirectSoundVoiceSet *)*voiceSetCursor != (DirectSoundVoiceSet *)0x0) {
      (*g_SoundReleaseSampleVoiceSet)((DirectSoundVoiceSet *)*voiceSetCursor);
      voiceSetsRemaining = extraout_ECX;
    }
    *voiceSetCursor = 0;
    voiceSetCursor = voiceSetCursor + 1;
    voiceSetsRemaining = voiceSetsRemaining + -1;
  } while (voiceSetsRemaining != 0);
  (*g_SoundStopVoice)(g_FrontendMusicActiveBuffer);
  (*g_SoundReleaseSampleVoiceSet)(g_FrontendMusicVoiceSet);
  g_FrontendMusicActiveBuffer = (IDirectSoundBuffer *)0x0;
  g_FrontendMusicVoiceSet = (DirectSoundVoiceSet *)0x0;
  SpriteAssetRegistry_Reset();
  UiFrame_FlushInputAndResetPendingTicks();
  return CONCAT44(unaff_EDI,unaff_EBX);
}

/* Address: 0x0050AD90.
   Ownership: ui/frontend/runtime.
   Purpose: EDX returns selected ModelRuntimeNode and EAX its hit metric.
   Cross-module calls: ModelRuntimeNode_HitTestProjectedBoundsAndChildrenCf [world/model/hierarchy].
*/
qword FrontendModelPointerContext_FindBestEligibleModelHitTarget
                (int pointerY,int pointerX,FrontendModelPointerContextRuntimeState118 *context)

{
  ModelRuntimeNode *modelNode;
  dword dVar1;
  dword dVar2;
  dword extraout_ECX;
  ModelRuntimeNode *pMVar3;
  ModelRuntimeNode *extraout_EDX;
  bool bVar4;
  
  pMVar3 = (ModelRuntimeNode *)0x0;
  modelNode = context->candidateModelListHead;
  dVar2 = 0x7fffffff;
  do {
    if (modelNode == (ModelRuntimeNode *)0x0) {
      return CONCAT44(pMVar3,dVar2);
    }
    if ((((modelNode->runtimeFlags & 2) != 0) && (modelNode->ownerClassId == MODEL_RUNTIME_CLASS_00)
        ) && (((context->contextFlags &
               FRONTEND_MODEL_POINTER_CONTEXT_ALLOW_MODEL_WITHOUT_RUNTIME_FLAG_20) != 0 ||
              ((modelNode->runtimeFlags & 0x20) != 0)))) {
      bVar4 = false;
      dVar1 = ModelRuntimeNode_HitTestProjectedBoundsAndChildrenCf
                        (pointerY,pointerX,modelNode,context);
      dVar2 = extraout_ECX;
      pMVar3 = extraout_EDX;
      if (!bVar4) {
        if ((context->contextFlags & FRONTEND_MODEL_POINTER_CONTEXT_COMPARE_HITS_BY_METRIC_ONLY) ==
            0) {
          if (extraout_EDX != (ModelRuntimeNode *)0x0) {
            dVar2 = extraout_ECX;
            pMVar3 = extraout_EDX;
            if ((int)(&g_RuntimeModelClassPriorityByModelClassId.modelClass00Priority)
                     [*(int *)((int)((modelNode->runtimePayload).armyRuntime)->definitionOrAsset +
                              0x4c)] <
                (int)(&g_RuntimeModelClassPriorityByModelClassId.modelClass00Priority)
                     [*(int *)((int)((extraout_EDX->runtimePayload).armyRuntime)->definitionOrAsset
                              + 0x4c)]) goto FrontendModelHitSelection_AdvanceCandidate;
            if ((int)(&g_RuntimeModelClassPriorityByModelClassId.modelClass00Priority)
                     [*(int *)((int)((modelNode->runtimePayload).armyRuntime)->definitionOrAsset +
                              0x4c)] <=
                (int)(&g_RuntimeModelClassPriorityByModelClassId.modelClass00Priority)
                     [*(int *)((int)((extraout_EDX->runtimePayload).armyRuntime)->definitionOrAsset
                              + 0x4c)]) goto FrontendModelHitSelection_CompareHitMetric;
          }
        }
        else {
FrontendModelHitSelection_CompareHitMetric:
          dVar2 = extraout_ECX;
          pMVar3 = extraout_EDX;
          if ((int)extraout_ECX <= (int)dVar1) goto FrontendModelHitSelection_AdvanceCandidate;
        }
        dVar2 = dVar1;
        pMVar3 = modelNode;
      }
    }
FrontendModelHitSelection_AdvanceCandidate:
    modelNode = (ModelRuntimeNode *)(modelNode->common).nextNode;
  } while( true );
}

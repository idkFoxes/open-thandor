/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/src/ui/frontend/runtime.c
 * Reverse engineering by idkFoxes 2026
 */

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
FrontendMainLoopEaxCf5 __thandor_eax_cf_preserve_ecx_edx
Frontend_MainLoop(RomRecordId frontendEntryRecordId)

{
  FrontendRoleStateFlags *pFVar1;
  AssetAllocationSizeBytes AVar2;
  FrontendSnapshotTransferFlags FVar3;
  SessionNetworkRoleFlags SVar4;
  ScenarioCatalogHeader *source;
  dword dVar5;
  uint extraout_EAX;
  FieldGridAsset *sourceGrid;
  RomRecordId initialRomRecordId;
  int iVar6;
  FrontendPlayerRuntimeBlockCount FVar7;
  int iVar8;
  uint uVar9;
  SessionNetworkRoleFlags SVar10;
  byte *pbVar11;
  PckOutputCapacityBytes destinationCapacityBytes;
  FrontendPlayerRuntimeRecord *pFVar12;
  void *pvVar13;
  void *pvVar14;
  byte *transferSourceBytes;
  FrontendLoadedLevelRuntimeImage370 *source_00;
  FrontendPlayerRuntimeRecord *pFVar15;
  FrontendSnapshotTransferFlags *pFVar16;
  dword *transferDwordCursor;
  FrontendInitEaxCf5 FVar17;
  FrontendMainLoopEaxCf5 FVar18;
  FrontendMainLoopEaxCf5 FVar19;
  InGameRuntimeRunEaxCf5 IVar20;
  PackageLoadEntryEaxCf5 PVar21;
  FatalErrorEaxCf5 FVar22;
  PckCodecEaxCf5 PVar23;
  ArenaAllocEaxCf5 AVar24;
  UiTransferMailboxReceivedEaxEcxCf9 UVar25;
  CommandLineFindOptionEbxCf5 CVar26;
  FieldGridAsset *pFVar27;
  
  g_FrontendNetworkState = 0;
  FVar17 = Frontend_Init(frontendEntryRecordId);
  dVar5 = FVar17.frontendRootOrError;
  if (!FVar17.carry) {
    CVar26 = (*g_CommandLineFindOption)(5,s_SPIELER__SPIEL__NETZWERK__HOST_00545e72 + 0x1a);
    if (CVar26.carry) {
      CVar26 = (*g_CommandLineFindOption)(8,s_NAME__CLIENT__KARTE___00545e91 + 6);
      if (CVar26.carry) {
        CVar26 = (*g_CommandLineFindOption)(7,s_NAME__CLIENT__KARTE___00545e91 + 0xe);
        if (!CVar26.carry) {
          FrontendRomActionTable_ExecuteRecord(0,0,1,0);
          FrontendRomTransition_RequestStop();
        }
      }
      else {
        FrontendRomActionTable_ExecuteRecord(0,0,1,3);
        FrontendRomTransition_RequestStop();
      }
    }
    else {
      FrontendRomActionTable_ExecuteRecord(0,0,1,3);
      FrontendRomTransition_RequestStop();
    }
FrontendMainLoop_ProcessFrameAndPendingPageAction:
    do {
      while( true ) {
        while( true ) {
          while( true ) {
            if (g_UiRootNode != (UiRootNode *)0xffffffff) {
              FrontendRomTransition_ProcessPendingRecord();
            }
            UiRootStack_InvalidateAll();
            UiFrame_ProcessAndPresent();
            g_FrontendPendingPageActionDepth = 0;
            if (g_FrontendPendingPageAction != 0) break;
            if (g_UiRootNode == (UiRootNode *)0xffffffff) {
              FrontendRuntime_ShutdownAndReleaseResourcesRegs();
              FVar18.carry = false;
              FVar18.errorOrValue = extraout_EAX;
              return FVar18;
            }
          }
          UiFrame_FlushInputAndResetPendingTicks();
          g_FrontendPendingPageActionDepth = g_FrontendPendingPageActionDepth + 1;
          if (g_FrontendPendingPageAction != 2) break;
          FrontendNetworkSetupPage_InitializeBackendMode(g_FrontendRootNode);
          g_FrontendPendingPageAction = 0;
        }
        if (g_FrontendPendingPageAction != 3) break;
        FrontendGameplaySettingsPage_InitializeFromPersistentSettings
                  ((UiRootNode *)&g_FrontendRootNode->commonState);
        g_FrontendPendingPageAction = 0;
      }
      if (g_FrontendPendingPageAction == 5) {
        pFVar12 = g_FrontendPlayerRuntimeBlocks;
        SVar10 = g_FrontendPlayerRuntimeBlockCount;
        SVar4 = g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_NETWORKED_MASK;
        while (FVar7 = g_FrontendPlayerRuntimeBlockCount, pFVar15 = g_FrontendPlayerRuntimeBlocks,
              SVar4 != SESSION_NETWORK_ROLE_LOCAL) {
          if ((pFVar12->snapshotTransferFlags & FRONTEND_SNAPSHOT_HOST_PUBLICATION_READY) == 0) {
            if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_CLIENT) !=
                SESSION_NETWORK_ROLE_LOCAL) {
              UVar25 = UiTransferMailbox_GetReceivedBufferCf();
              if (!UVar25.carry) {
                PckCodec_DecodeHuffmanRle
                          (*(PckDecodedByteCount *)UVar25.eax,g_PackageScratchBuffer,UVar25.ecx - 4,
                           (byte *)((PckDecodedByteCount *)UVar25.eax + 1));
                FVar7 = g_FrontendPlayerRuntimeBlockCount;
                pFVar16 = (FrontendSnapshotTransferFlags *)g_PackageScratchBuffer;
                pFVar12 = g_FrontendPlayerRuntimeBlocks;
                do {
                  FVar3 = *pFVar16;
                  pFVar12->snapshotTransferFlags = pFVar12->snapshotTransferFlags | FVar3;
                  pFVar16 = pFVar16 + 1;
                  if ((FVar3 & FRONTEND_SNAPSHOT_PAYLOAD_COMPLETE) != 0) {
                    pbVar11 = pFVar12->snapshotPayloadB0_13AF;
                    for (iVar6 = 0x4c0; iVar6 != 0; iVar6 = iVar6 + -1) {
                      *(FrontendSnapshotTransferFlags *)pbVar11 = *pFVar16;
                      pFVar16 = pFVar16 + 1;
                      pbVar11 = pbVar11 + 4;
                    }
                  }
                  pFVar12 = pFVar12 + 1;
                  FVar7 = FVar7 - 1;
                } while (FVar7 != 0);
                FrontendCommandQueue_EnqueueLocalPlayerCommand(0x1710,0,0,0);
                UiTransferMailbox_ClearReceivedState();
              }
            }
            goto FrontendMainLoop_ProcessFrameAndPendingPageAction;
          }
          pFVar12 = pFVar12 + 1;
          SVar10 = SVar10 - SESSION_NETWORK_ROLE_CLIENT;
          SVar4 = SVar10;
        }
        do {
          pFVar1 = &(pFVar15->factionAssignment).roleStateFlags;
          *pFVar1 = *pFVar1 & 1;
          pFVar12 = g_FrontendPlayerRuntimeBlocks;
          if (*pFVar1 == 0) {
            FrontendScenarioTransfer_ProcessReceivedAsset();
            if (((pFVar12->factionAssignment).roleStateFlags & 1) == 0) {
              pFVar1 = &(pFVar12->factionAssignment).roleStateFlags;
              *pFVar1 = *pFVar1 | 1;
              ScenarioCatalog_Rebuild();
              dVar5 = g_ScenarioCatalogUsedBytes;
              source = g_ScenarioCatalog;
              if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_CLIENT) ==
                  SESSION_NETWORK_ROLE_LOCAL) {
                if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_HOST) !=
                    SESSION_NETWORK_ROLE_LOCAL) {
                  pbVar11 = (byte *)((int)&g_ScenarioCatalog->campaignRecordsOffset +
                                    g_ScenarioCatalogUsedBytes);
                  destinationCapacityBytes = 0x2fffc - g_ScenarioCatalogUsedBytes;
                  *(dword *)(pbVar11 + -4) = g_ScenarioCatalogUsedBytes;
                  PVar23 = PckCodec_EncodeHuffmanRle
                                     (destinationCapacityBytes,pbVar11,dVar5,(byte *)source);
                  FVar22 = (*g_FatalErrorPrimaryDispatchCf)(PVar23.eax,PVar23.carry);
                  UiTransferMailbox_SetOutgoingBuffer(FVar22.eax + 4,pbVar11 + -4);
                }
              }
              else {
                UiTransferMailbox_MarkUnavailable();
                g_FrontendScenarioTransferState = 1;
              }
            }
            goto FrontendMainLoop_ProcessFrameAndPendingPageAction;
          }
          pFVar15 = pFVar15 + 1;
          FVar7 = FVar7 - 1;
        } while (FVar7 != 0);
        if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_HOST) != SESSION_NETWORK_ROLE_LOCAL) {
          UiTransferMailbox_SetOutgoingBuffer(0,(void *)0x0);
        }
        FrontendScenarioSelectionPage_InitializeAndApplyMapOption
                  ((FrontendScenarioSelectionPageView26C4 *)&g_FrontendRootNode->commonState);
        g_FrontendPendingPageAction = 0;
        goto FrontendMainLoop_ProcessFrameAndPendingPageAction;
      }
      if (g_FrontendPendingPageAction == 7) {
        FrontendScenarioTransfer_ProcessReceivedAsset();
        FVar7 = g_FrontendPlayerRuntimeBlockCount;
        pFVar12 = g_FrontendPlayerRuntimeBlocks;
        do {
          if (((pFVar12->factionAssignment).roleStateFlags & 2) == 0)
          goto FrontendMainLoop_ProcessFrameAndPendingPageAction;
          pFVar12 = pFVar12 + 1;
          FVar7 = FVar7 - 1;
        } while (FVar7 != 0);
        if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_HOST) != SESSION_NETWORK_ROLE_LOCAL) {
          (*g_MemoryApi.free)(g_UiTransferMailbox.outgoingAllocation);
          UiTransferMailbox_SetOutgoingBuffer(0,(void *)0x0);
        }
        FrontendTaskAssignmentPage_Initialize
                  ((FrontendTaskAssignmentPageInitView26C4 *)&g_FrontendRootNode->commonState);
        g_FrontendPendingPageAction = 0;
      }
      else if (g_FrontendPendingPageAction == 8) {
        FrontendScenarioTransfer_ProcessReceivedAsset();
        FVar7 = g_FrontendPlayerRuntimeBlockCount;
        pFVar12 = g_FrontendPlayerRuntimeBlocks;
        do {
          if (((pFVar12->factionAssignment).roleStateFlags & 0xc) == 0)
          goto FrontendMainLoop_ProcessFrameAndPendingPageAction;
          pFVar12 = pFVar12 + 1;
          FVar7 = FVar7 - 1;
        } while (FVar7 != 0);
        if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_HOST) != SESSION_NETWORK_ROLE_LOCAL) {
          (*g_MemoryApi.free)(g_UiTransferMailbox.outgoingAllocation);
          UiTransferMailbox_SetOutgoingBuffer(0,(void *)0x0);
        }
        FrontendMissionBriefingPage_Initialize((UiRootNode *)&g_FrontendRootNode->commonState);
        g_FrontendPendingPageAction = 0;
      }
      else if (g_FrontendPendingPageAction == 9) {
        CreditsScreen_Open((FrontendCreditsUiStateView *)&g_FrontendRootNode->commonState);
        g_FrontendPendingPageAction = 0;
      }
      else {
        if (g_FrontendPendingPageAction != 4) {
          FrontendRuntime_ShutdownAndReleaseResourcesRegs();
          if (g_FrontendPendingPageAction == 1) {
            PersistentSettings_Flush();
            IVar20 = InGameRuntime_RunSessionUntilExit
                               ((LevelAssetRuntimeImagePrefix370 *)g_FrontendLoadedLevelAsset,0,
                                (word *)&g_FrontendScenarioPathScratchUtf16);
            (*g_FatalErrorPrimaryDispatchCf)(IVar20.exitCodeOrError,IVar20.carry);
            PersistentSettings_Flush();
            UiFrame_FlushInputAndResetPendingTicks();
            g_FrontendScenarioInitializationCount = 0;
            FVar7 = g_FrontendPlayerRuntimeBlockCount;
            pFVar12 = g_FrontendPlayerRuntimeBlocks;
            do {
              (pFVar12->factionAssignment).roleStateFlags = 0;
              pFVar12 = pFVar12 + 1;
              FVar7 = FVar7 - 1;
              pvVar14 = g_FrontendLoadedCampaignAsset;
            } while (FVar7 != 0);
joined_r0x0054707a:
            g_FrontendLoadedCampaignAsset = pvVar14;
            if (pvVar14 != (void *)0x0) {
              iVar6 = *(int *)((int)pvVar14 + 0xb8);
              pvVar13 = pvVar14;
              do {
                if (*(int *)((int)pvVar14 + 0xc4) == *(int *)((int)pvVar13 + 0x300)) {
                  iVar6 = *(int *)((int)pvVar13 + (int)g_EndMovieSelectionIndex * 4 + 0x200);
                  if (-1 < iVar6) {
                    iVar8 = *(int *)((int)pvVar14 + 0xb8);
                    *(int *)((int)pvVar14 + 0xc4) = iVar6;
                    goto FrontendCampaign_FindSelectedLevelRecord;
                  }
                  break;
                }
                pvVar13 = (void *)((int)pvVar13 + 0x180);
                iVar6 = iVar6 + -1;
              } while (iVar6 != 0);
            }
            goto FrontendScenario_UseResolvedPathOrFallbackPage;
          }
          if (g_FrontendPendingPageAction == 6) {
            IVar20 = InGameRuntime_RunSessionUntilExit
                               ((LevelAssetRuntimeImagePrefix370 *)g_FrontendLoadedLevelAsset,1,
                                (word *)&g_FrontendScenarioPathScratchUtf16);
            (*g_FatalErrorPrimaryDispatchCf)(IVar20.exitCodeOrError,IVar20.carry);
            PersistentSettings_Flush();
            UiFrame_FlushInputAndResetPendingTicks();
            g_FrontendScenarioInitializationCount = 0;
            FVar7 = g_FrontendPlayerRuntimeBlockCount;
            pFVar12 = g_FrontendPlayerRuntimeBlocks;
            do {
              (pFVar12->factionAssignment).roleStateFlags = 0;
              pFVar12 = pFVar12 + 1;
              FVar7 = FVar7 - 1;
              pvVar14 = g_FrontendLoadedCampaignAsset;
            } while (FVar7 != 0);
            goto joined_r0x0054707a;
          }
          iVar6 = 0;
          initialRomRecordId = frontendEntryRecordId;
          goto FrontendMainLoop_InitializeRequestedPage;
        }
        FrontendSession_ShowPage9WithCompactLayout(g_FrontendRootNode);
        g_FrontendPendingPageAction = 0;
      }
    } while( true );
  }
FrontendMainLoop_ShutdownAndReturn:
  FrontendRuntime_ShutdownAndReleaseResourcesRegs();
  FVar19.carry = true;
  FVar19.errorOrValue = dVar5;
  return FVar19;
  while( true ) {
    pvVar14 = (void *)((int)pvVar14 + 0x180);
    iVar8 = iVar8 + -1;
    if (iVar8 == 0) break;
FrontendCampaign_FindSelectedLevelRecord:
    if (iVar6 == *(int *)((int)pvVar14 + 0x300)) {
      WidePath_CombineDirectoryAndLeaf
                ((word *)&g_FrontendScenarioPathScratchUtf16,(word *)((int)pvVar14 + 0x30c),
                 (word *)u_level_0050daac);
      WidePath_SetExtensionCode(0x76656c,(word *)&g_FrontendScenarioPathScratchUtf16);
      goto FrontendScenario_InitializeSelectedLevel;
    }
  }
  Resource_Release(g_FrontendLoadedCampaignAsset);
  g_FrontendLoadedCampaignAsset = (void *)0x0;
  g_FrontendScenarioInitializationCount = 0;
FrontendScenario_UseResolvedPathOrFallbackPage:
  if (g_FrontendScenarioPathScratchUtf16 != 0) {
FrontendScenario_InitializeSelectedLevel:
    FVar17 = Frontend_Init(10);
    dVar5 = FVar17.frontendRootOrError;
    if (FVar17.carry) goto FrontendMainLoop_ShutdownAndReturn;
    g_FrontendScenarioInitializationCount = g_FrontendScenarioInitializationCount + 1;
    g_FrontendPendingPageAction = 8;
    pFVar1 = &(g_FrontendPlayerRuntimeBlocks->factionAssignment).roleStateFlags;
    *pFVar1 = *pFVar1 | 4;
    if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_CLIENT) == SESSION_NETWORK_ROLE_LOCAL) {
      if ((g_FrontendLoadedLevelAsset != (FrontendLoadedLevelRuntimeImage370 *)0x0) &&
         (0xffff < (g_FrontendLoadedLevelAsset->header).pathState.levelPathOffsetOrLoadedFieldGrid))
      {
        Resource_Release((void *)(g_FrontendLoadedLevelAsset->header).pathState.
                                 levelPathOffsetOrLoadedFieldGrid);
      }
      Resource_Release(g_FrontendLoadedLevelAsset);
      g_FrontendLoadedLevelAsset = (FrontendLoadedLevelRuntimeImage370 *)0x0;
      PVar21 = Package_LoadEntry((word *)&g_FrontendScenarioPathScratchUtf16);
      FVar22 = (*g_FatalErrorPrimaryDispatchCf)((dword)PVar21.bufferOrError,PVar21.carry);
      g_FrontendLoadedLevelAsset = (FrontendLoadedLevelRuntimeImage370 *)FVar22.eax;
      pbVar11 = (g_FrontendLoadedLevelAsset->header).common.buildMetadata.
                assetRelativeAddressAnchor28 +
                ((g_FrontendLoadedLevelAsset->header).pathState.levelPathOffsetOrLoadedFieldGrid -
                0x28);
      WidePath_SetExtensionCode(0x646c66,(word *)pbVar11);
      WidePath_CombineDirectoryAndLeaf
                ((word *)&g_LevelResourcePathScratchUtf16,(word *)pbVar11,
                 (word *)&g_ExecutableDirectoryUtf16);
      PVar21 = Package_LoadEntry((word *)pbVar11);
      source_00 = g_FrontendLoadedLevelAsset;
      transferSourceBytes = g_PackageScratchBuffer;
      sourceGrid = PVar21.bufferOrError;
      if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_HOST) != SESSION_NETWORK_ROLE_LOCAL) {
        AVar2 = (sourceGrid->common).allocationSizeBytes;
        *(AssetAllocationSizeBytes *)g_PackageScratchBuffer =
             (g_FrontendLoadedLevelAsset->header).common.allocationSizeBytes;
        *(AssetAllocationSizeBytes *)(transferSourceBytes + 4) = AVar2;
        pbVar11 = transferSourceBytes + 0x10;
        pFVar27 = sourceGrid;
        PVar23 = PckCodec_EncodeHuffmanRle
                           (0x7fffe8,pbVar11,(source_00->header).common.allocationSizeBytes,
                            (byte *)source_00);
        FVar22 = (*g_FatalErrorPrimaryDispatchCf)(PVar23.eax,PVar23.carry);
        dVar5 = FVar22.eax;
        *(dword *)(transferSourceBytes + 8) = dVar5;
        PVar23 = PckCodec_EncodeFieldGrid
                           (0x7fffe8 - dVar5,pbVar11 + dVar5,
                            (sourceGrid->common).allocationSizeBytes,sourceGrid);
        FVar22 = (*g_FatalErrorPrimaryDispatchCf)(PVar23.eax,PVar23.carry);
        *(dword *)(transferSourceBytes + 0xc) = FVar22.eax;
        pbVar11 = pbVar11 + dVar5 + (FVar22.eax - (int)transferSourceBytes);
        AVar24 = (*g_MemoryApi.alloc)((dword)pbVar11);
        FVar22 = (*g_FatalErrorPrimaryDispatchCf)(AVar24.eax,AVar24.carry);
        transferDwordCursor = (dword *)FVar22.eax;
        for (uVar9 = (uint)pbVar11 >> 2; uVar9 != 0; uVar9 = uVar9 - 1) {
          *transferDwordCursor = *(dword *)transferSourceBytes;
          transferSourceBytes = transferSourceBytes + 4;
          transferDwordCursor = transferDwordCursor + 1;
        }
        sourceGrid = pFVar27;
        UiTransferMailbox_SetOutgoingBuffer((UiTransferPayloadByteCount)pbVar11,(dword *)FVar22.eax)
        ;
      }
      (source_00->header).pathState.levelPathOffsetOrLoadedFieldGrid = (dword)sourceGrid;
      FrontendPlayerRuntime_InitializeFactionAssignments();
    }
    else {
      UiTransferMailbox_MarkUnavailable();
      g_FrontendScenarioTransferState = 5;
    }
    goto FrontendMainLoop_ProcessFrameAndPendingPageAction;
  }
  initialRomRecordId = 0xc;
  iVar6 = 5;
FrontendMainLoop_InitializeRequestedPage:
  FVar17 = Frontend_Init(initialRomRecordId);
  dVar5 = FVar17.frontendRootOrError;
  g_FrontendPendingPageAction = iVar6;
  if (!FVar17.carry) goto FrontendMainLoop_ProcessFrameAndPendingPageAction;
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
void __thandor_preserve_eax_edx
FrontendModelPointerContext_NonRightPress
          (UiPointerWheelDelta wheelDelta,UiPixelCoordinate pointerY,UiPixelCoordinate pointerX,
          FrontendModelPointerContextRuntimeState17C *callbackContext)

{
  qword qVar1;
  
  callbackContext->scratchCoordinate160 = pointerX;
  callbackContext->scratchCoordinate164 = pointerY;
  qVar1 = FrontendModelPointerContext_FindBestEligibleModelHitTarget
                    (pointerY,pointerX,(FrontendModelPointerContextRuntimeState118 *)callbackContext
                    );
  callbackContext->selectedModelNode = (ModelRuntimeNode *)(qVar1 >> 0x20);
  callbackContext->selectedHitMetric = (int)qVar1;
  callbackContext->contextFlags =
       callbackContext->contextFlags | FRONTEND_MODEL_POINTER_CONTEXT_ROUTE_TO_SECONDARY_CALLBACK;
  if (callbackContext->resolvedActionCallback10C !=
      (FrontendModelPointerResolvedActionCallbackProc *)0x0) {
    (*callbackContext->resolvedActionCallback10C)
              (callbackContext->callbackArgumentF0,callbackContext->callbackArgumentEC,
               callbackContext->callbackArgumentE8,callbackContext->selectedHitMetric,
               callbackContext->selectedModelNode,
               (FrontendModelPointerContextRuntimeState118 *)callbackContext);
  }
  return;
}


/* Address: 0x0050C610.
   Ownership: ui/frontend/runtime.
   Purpose: Exact packed function-table or callback-registration provenance plus immutable body topology prove this
   callable entry.
   Local calls: FrontendModelPointerContext_FindBestEligibleModelHitTarget.
*/
void __thandor_preserve_eax_edx
FrontendModelPointerContext_NonRightRelease
          (UiPointerWheelDelta wheelDelta,UiPixelCoordinate pointerY,UiPixelCoordinate pointerX,
          FrontendModelPointerContextRuntimeState118 *callbackContext)

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
void __thandor_preserve_eax_edx
FrontendModelPointerContext_NonRightDrag
          (UiPointerWheelDelta wheelDelta,UiPixelCoordinate pointerY,UiPixelCoordinate pointerX,
          FrontendModelPointerContextRuntimeState17C *callbackContext)

{
  qword qVar1;
  
  callbackContext->scratchCoordinate168 = pointerX;
  callbackContext->scratchCoordinate16C = pointerY;
  qVar1 = FrontendModelPointerContext_FindBestEligibleModelHitTarget
                    (pointerY,pointerX,(FrontendModelPointerContextRuntimeState118 *)callbackContext
                    );
  callbackContext->selectedModelNode = (ModelRuntimeNode *)(qVar1 >> 0x20);
  callbackContext->selectedHitMetric = (int)qVar1;
  if (callbackContext->resolvedActionCallback110 !=
      (FrontendModelPointerResolvedActionCallbackProc *)0x0) {
    (*callbackContext->resolvedActionCallback110)
              (callbackContext->callbackArgumentF0,callbackContext->callbackArgumentEC,
               callbackContext->callbackArgumentE8,callbackContext->selectedHitMetric,
               callbackContext->selectedModelNode,
               (FrontendModelPointerContextRuntimeState118 *)callbackContext);
  }
  return;
}


/* Address: 0x00549B40.
   Ownership: ui/frontend/runtime.
   Purpose: Recovered action-table target FRONTEND_PAGE20[68] (0x2044).
   Local calls: FrontendUiAction2044_IndexedSelectionHelper.
   Cross-module calls: FrontendCommandQueue_EnqueueLocalPlayerCommand [network/protocol/commands].
*/
void __thandor_void_preserve_eax_ecx FrontendUiAction2044_Handler(UiNodeBase *factionControl)

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
void __thandor_void_preserve_eax_ecx FrontendUiAction2045_Handler(UiNodeBase *playerControl)

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
void __thandor_void_preserve_eax_ecx FrontendUiAction2046_Handler(UiNodeBase *selectionRowControl)

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
               (UiSerializedRelocationDelta relocationDelta,
               FrontendModelPointerContextRuntimeState17C *control)

{
  control->targetPositionXQ12 = 0;
  control->targetPositionYQ12 = 0;
  control->targetPositionZQ12 = 0;
  if (control->minimumPitchAngle == 0) {
    control->minimumPitchAngle = 0xffffc000;
  }
  if (control->maximumPitchAngle == 0) {
    control->maximumPitchAngle = 0x4000;
  }
  if (control->minimumDistanceQ12 == 0) {
    control->minimumDistanceQ12 = 0x400;
  }
  if (control->maximumDistanceOrSurfaceLimitQ12 == 0) {
    control->maximumDistanceOrSurfaceLimitQ12 = 0x7f000;
  }
  control->contextValue58 = 0;
  control->objectCountOrFrontendStateAC = 0;
  control->candidateModelListHead = (ModelRuntimeNode *)0x0;
  control->selectedOverlayEntity = (GameEntityRuntime *)0x0;
  UiContainer_RelocateChildren(relocationDelta,&control->base);
  return;
}


/* Address: 0x0050BC30.
   Ownership: ui/frontend/runtime.
   Purpose: Exact packed function-table or callback-registration provenance plus immutable body topology prove this
   callable entry.
   Cross-module calls: WorldRuntime_ClearFieldGridDirtyFlag [world/runtime/core], UiContainer_LayoutChildren
   [ui/controls/layout].
*/
void __thandor_void_preserve_eax_ecx_edx
FrontendModelPointerContext_Layout(WorldRuntimeContext *callbackContext)

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
void __thandor_void_preserve_eax_ecx_edx
FrontendModelPointerContext_RenderWorldViewQueuesClipped
          (UiPixelCoordinate clipTop,UiPixelCoordinate clipLeft,UiPixelCoordinate clipBottom,
          UiPixelCoordinate clipRight,FrontendModelPointerContextRuntimeState17C *control)

{
  UiPixelCoordinate UVar1;
  UiPixelCoordinate UVar2;
  dword dVar3;
  GraphicsWorldCoordinateQ12 originY;
  code *pcVar4;
  code *clipRight_00;
  ModelRuntimeNode *pMVar5;
  bool bVar6;
  GraphicsPrimitiveQueueEaxCf5 GVar7;
  
  if ((control->contextFlags & 0x2000) != 0) {
    return;
  }
  if (clipRight < (control->base).left) {
    clipRight = (control->base).left;
  }
  if ((control->base).right < clipLeft) {
    clipLeft = (control->base).right;
  }
  if (clipBottom < (control->base).top) {
    clipBottom = (control->base).top;
  }
  if ((control->base).bottom < clipTop) {
    clipTop = (control->base).bottom;
  }
  (*g_GraphicsSetViewportAndClearDepth)(clipTop,clipLeft,clipBottom,clipRight);
  (*g_SpinLockAcquire)(control->renderSpinLock);
  UVar2 = g_CursorOverrideY;
  UVar1 = g_CursorOverrideX;
  control->selectedModelNode = (ModelRuntimeNode *)0x0;
  control->selectedHitMetric = 0x7fffffff;
  control->callbackArgumentE8 = 0x7fffffff;
  control->callbackArgumentEC = 0x7fffffff;
  control->callbackArgumentF0 = 0x7fffffff;
  pcVar4 = (code *)(UVar2 << 0xc);
  control->cursorWorldXQ12 = UVar1 << 0xc;
  control->cursorWorldYQ12 = (Q12)pcVar4;
  control->renderedPrimitiveCount = 0;
  Graphics_SetProjectionClipRect(clipTop,clipLeft,clipBottom,clipRight);
  Graphics_SetViewProjectionParameters
            (control->projectionShift,control->viewAngle1,control->viewAngle0,
             control->projectionScale,control->hitReferenceWorldZQ12,control->hitReferenceWorldYQ12,
             control->hitReferenceWorldXQ12);
  originY = clipLeft;
  if ((control->contextFlags & 0x10000) != 0) {
    originY = control->targetPositionYQ12;
    pcVar4 = (code *)(((int)control->committedDistanceOrSoundZOffset >> 2) +
                     control->targetPositionZQ12);
    SpatialSound_RebuildListenerTransformFromPose
              (control->viewAngle1,control->viewAngle0,(GraphicsWorldCoordinateQ12)pcVar4,originY,
               control->targetPositionXQ12);
  }
  Graphics_SetProjectionViewport
            ((control->base).bottom,(control->base).right,(control->base).top,(control->base).left);
  Graphics_SetAuxiliaryOrientation
            (control->auxiliaryOrientationAngle1,control->auxiliaryOrientationAngle0);
  Graphics_SetSceneBounds
            (control->sceneBound7,control->sceneBound6,control->sceneBound5,control->sceneBound4,
             control->sceneBound3,control->sceneBound2,control->sceneBound1,control->sceneBound0);
  Graphics_RebuildFrustumPlanes();
  (*g_GraphicsBeginScene)();
  (*g_SpinLockReleaseAndInvoke)(control->renderSpinLockReleaseCallback,control->renderSpinLock);
  (*g_SpinLockAcquire)(control->renderSpinLock);
  GraphicsShadingRuntime_RebuildCompactLightingRecords();
  (*g_SpinLockReleaseAndInvoke)(control->renderSpinLockReleaseCallback,control->renderSpinLock);
  (*g_SpinLockAcquire)(control->renderSpinLock);
  GVar7 = GraphicsPrimitiveQueue_ResetGlobal();
  clipRight_00 = (code *)clipTop;
  pMVar5 = (ModelRuntimeNode *)clipBottom;
  if (!GVar7.carry) {
    Graphics_SetActivePrimitiveQueue(GVar7.queue);
    control->activePrimitiveQueue = GVar7.queue;
    if (control->renderPhaseCallback15C != (InGameWorldOverlayPhaseCallbackProc *)0x0) {
      (*control->renderPhaseCallback15C)(GRAPHICS_STATE_DISABLED,(WorldRuntimeContext *)control);
    }
    pMVar5 = control->candidateModelListHead;
    pcVar4 = ModelRuntime_CullAndRenderHierarchyRecursive;
    if ((control->contextFlags & FRONTEND_MODEL_POINTER_CONTEXT_COMPARE_HITS_BY_METRIC_ONLY) != 0) {
      pcVar4 = ModelRuntime_RenderHierarchyRecursiveAlternatePath;
    }
    for (; pMVar5 != (ModelRuntimeNode *)0x0; pMVar5 = (ModelRuntimeNode *)(pMVar5->common).nextNode
        ) {
      if ((((pMVar5->runtimeFlags & 0x40) == 0) && ((pMVar5->runtimeFlags & 0x200) != 0)) &&
         (pMVar5->runtimeFlags = pMVar5->runtimeFlags & 0xfffffffd,
         (pMVar5->tintArgb & 0xff000000) != 0)) {
        (*pcVar4)(pMVar5);
      }
    }
    if (control->renderPhaseCallback15C != (InGameWorldOverlayPhaseCallbackProc *)0x0) {
      (*control->renderPhaseCallback15C)(GRAPHICS_STATE_ENABLED,(WorldRuntimeContext *)control);
    }
    (*PTR_GraphicsPrimitiveQueue_RadixSortForRendering_00485844)
              ((control->base).nodeFlags & 8,control->activePrimitiveQueue);
    (*g_GraphicsDrawPrimitiveQueue)
              (clipTop,clipLeft,clipBottom,clipRight,control->activePrimitiveQueue);
    dVar3 = GraphicsPrimitiveQueue_GetCount(control->activePrimitiveQueue);
    control->renderedPrimitiveCount = control->renderedPrimitiveCount + dVar3;
    (*g_SpinLockReleaseAndInvoke)(control->renderSpinLockReleaseCallback,control->renderSpinLock);
    (*g_SpinLockAcquire)(control->renderSpinLock);
    pMVar5 = (ModelRuntimeNode *)0x0;
    if (((control->contextFlags & 0x4000) != 0) && (control->fieldGrid != (FieldGridAsset *)0x0)) {
      GVar7 = GraphicsPrimitiveQueue_ResetGlobal();
      if (GVar7.carry) goto LAB_0050c196;
      Graphics_SetActivePrimitiveQueue(GVar7.queue);
      control->activePrimitiveQueue = GVar7.queue;
      TerrainProjectedGrid_TransformShadeAndQueue(control->fieldGrid,control);
      (*PTR_GraphicsPrimitiveQueue_RadixSortForRendering_00485844)
                ((control->base).nodeFlags & 8,control->activePrimitiveQueue);
      (*g_GraphicsDrawPrimitiveQueue)
                (clipTop,clipLeft,clipBottom,clipRight,control->activePrimitiveQueue);
      dVar3 = GraphicsPrimitiveQueue_GetCount(control->activePrimitiveQueue);
      control->renderedPrimitiveCount = control->renderedPrimitiveCount + dVar3;
    }
    (*g_SpinLockReleaseAndInvoke)(control->renderSpinLockReleaseCallback,control->renderSpinLock);
    (*g_SpinLockAcquire)(control->renderSpinLock);
    if ((control->contextFlags & 0x20000) != 0) {
      pMVar5 = control->candidateModelListHead;
      GVar7 = GraphicsPrimitiveQueue_ResetGlobal();
      if (GVar7.carry) goto LAB_0050c196;
      Graphics_SetActivePrimitiveQueue(GVar7.queue);
      control->activePrimitiveQueue = GVar7.queue;
      if (pMVar5 != (ModelRuntimeNode *)0x0) {
        GraphicsShadingGeneratedTexture_ResetPassScratchAndClearAlphaPlanes();
        do {
          if ((((pMVar5->runtimeFlags & 0x40) == 0) && ((pMVar5->runtimeFlags & 0x100) != 0)) &&
             ((pMVar5->tintArgb & 0xff000000) != 0)) {
            GraphicsShadingGeneratedTexture_ProcessRenderableHierarchy
                      (pMVar5,(GeneratedTextureRenderContextView *)control);
          }
          pMVar5 = (ModelRuntimeNode *)(pMVar5->common).nextNode;
        } while (pMVar5 != (ModelRuntimeNode *)0x0);
        GraphicsShadingGeneratedTexture_RefreshTouchedAlphaSubresources();
        pMVar5 = (ModelRuntimeNode *)0x0;
      }
      (*PTR_GraphicsPrimitiveQueue_RadixSortForRendering_00485844)
                ((control->base).nodeFlags & 8,control->activePrimitiveQueue);
      (*g_GraphicsDrawPrimitiveQueue)
                (clipTop,clipLeft,clipBottom,clipRight,control->activePrimitiveQueue);
      dVar3 = GraphicsPrimitiveQueue_GetCount(control->activePrimitiveQueue);
      control->renderedPrimitiveCount = control->renderedPrimitiveCount + dVar3;
    }
    (*g_SpinLockReleaseAndInvoke)(control->renderSpinLockReleaseCallback,control->renderSpinLock);
    (*g_SpinLockAcquire)(control->renderSpinLock);
    GVar7 = GraphicsPrimitiveQueue_ResetGlobal();
    if (!GVar7.carry) {
      Graphics_SetActivePrimitiveQueue(GVar7.queue);
      control->activePrimitiveQueue = GVar7.queue;
      if (control->renderPhaseCallback15C != (InGameWorldOverlayPhaseCallbackProc *)0x0) {
        (*control->renderPhaseCallback15C)(GRAPHICS_STATE_DISABLED,(WorldRuntimeContext *)control);
      }
      pMVar5 = control->candidateModelListHead;
      pcVar4 = ModelRuntime_CullAndRenderHierarchyRecursive;
      if ((control->contextFlags & FRONTEND_MODEL_POINTER_CONTEXT_COMPARE_HITS_BY_METRIC_ONLY) != 0)
      {
        pcVar4 = ModelRuntime_RenderHierarchyRecursiveAlternatePath;
      }
      for (; pMVar5 != (ModelRuntimeNode *)0x0;
          pMVar5 = (ModelRuntimeNode *)(pMVar5->common).nextNode) {
        if (((pMVar5->runtimeFlags & 0x240) == 0) &&
           (pMVar5->runtimeFlags = pMVar5->runtimeFlags & 0xfffffffd,
           (pMVar5->tintArgb & 0xff000000) != 0)) {
          (*pcVar4)(pMVar5);
        }
      }
      if (control->renderPhaseCallback15C != (InGameWorldOverlayPhaseCallbackProc *)0x0) {
        (*control->renderPhaseCallback15C)(GRAPHICS_STATE_ENABLED,(WorldRuntimeContext *)control);
      }
      (*PTR_GraphicsPrimitiveQueue_RadixSortForRendering_00485844)
                ((control->base).nodeFlags & 8,control->activePrimitiveQueue);
      (*g_GraphicsDrawPrimitiveQueue)
                (clipTop,clipLeft,clipBottom,clipRight,control->activePrimitiveQueue);
      dVar3 = GraphicsPrimitiveQueue_GetCount(control->activePrimitiveQueue);
      control->renderedPrimitiveCount = control->renderedPrimitiveCount + dVar3;
      (*g_SpinLockReleaseAndInvoke)(control->renderSpinLockReleaseCallback,control->renderSpinLock);
      (*g_SpinLockAcquire)(control->renderSpinLock);
      originY = clipLeft;
      pcVar4 = (code *)clipTop;
      clipRight_00 = (code *)clipRight;
      pMVar5 = (ModelRuntimeNode *)clipBottom;
      if (((((code *)clipRight == (code *)(control->base).left) &&
           (clipLeft == (control->base).right)) &&
          ((ModelRuntimeNode *)clipBottom == (ModelRuntimeNode *)(control->base).top)) &&
         ((code *)clipTop == (code *)(control->base).bottom)) {
        control->contextFlags = control->contextFlags | 0x800;
      }
    }
  }
LAB_0050c196:
  (*g_GraphicsEndScene)();
  g_RenderedFrameCountSinceDebugRefresh = g_RenderedFrameCountSinceDebugRefresh + 1;
  if (((control->base).nodeFlags & UI_NODE_SUPPRESSED) == 0) {
    g_SelectionPanelBlitOpaque = g_GraphicsTextureSourceBlitSourceAlpha;
    g_SelectionPanelBlitClipped = g_GraphicsTextureSourceBlitTiledSourceAlpha;
  }
  else {
    g_SelectionPanelBlitOpaque = g_GraphicsTextureSourceBlitHalfSourceRgb;
    g_SelectionPanelBlitClipped = g_GraphicsTextureSourceBlitTiledHalfSourceRgb;
  }
  if ((g_UiCommandRuntimeFlags & 0x8000) == 0) {
    if ((((control->contextFlags & 0x400) != 0) &&
        (SelectionOverlay_RenderSelectedArmyMetrics
                   ((UiPixelCoordinate)pcVar4,originY,(UiPixelCoordinate)pMVar5,
                    (UiPixelCoordinate)clipRight_00),
        control->selectedOverlayEntity != (GameEntityRuntime *)0x0)) &&
       (bVar6 = SelectionInfo_FindEntryCf(control->selectedOverlayEntity), bVar6)) {
      SelectionOverlay_RenderArmyMetricsForEntity
                ((UiPixelCoordinate)pcVar4,originY,(UiPixelCoordinate)pMVar5,
                 (UiPixelCoordinate)clipRight_00,control->selectedOverlayEntity);
    }
    if ((control->contextFlags & 0x80) != 0) {
      SelectionOverlay_DrawBoundsFrame
                ((UiPixelCoordinate)pcVar4,originY,(UiPixelCoordinate)pMVar5,
                 (UiPixelCoordinate)clipRight_00,control->scratchCoordinate16C,
                 control->scratchCoordinate168,control->scratchCoordinate164,
                 control->scratchCoordinate160);
    }
    if ((control->contextFlags & 0x200000) != 0) {
      SelectionOverlay_DrawMarkerADForFieldGridTerrainPoints
                ((UiPixelCoordinate)pcVar4,originY,(UiPixelCoordinate)pMVar5,
                 (UiPixelCoordinate)clipRight_00,control->terrainMarkerPointCount174,
                 control->terrainMarkerCoordinatePairs170,control->fieldGrid);
    }
    if (((control->contextFlags & 0x100000) != 0) && (control->callbackArgumentF0 != 0x7fffffff)) {
      SelectionOverlay_DrawMarkerACForWorldSurfacePoint
                ((UiPixelCoordinate)pcVar4,originY,(UiPixelCoordinate)pMVar5,
                 (UiPixelCoordinate)clipRight_00,
                 (uint)((g_UiCommandModeGColorVariantLimit & 0xff000000) != 0),
                 control->callbackArgumentEC,control->callbackArgumentE8,control->fieldGrid);
    }
    if ((control->contextFlags & 0x800000) != 0) {
      SelectionOverlay_DrawMarkerAEForVisibleProjectedGridVertices
                ((UiPixelCoordinate)pcVar4,originY,(UiPixelCoordinate)pMVar5,
                 (UiPixelCoordinate)clipRight_00,control->fieldGrid);
    }
    if ((control->contextFlags & 0x1000000) != 0) {
      SelectionOverlay_DrawMarkerAFB0ForProjectedVertexStateFlags
                ((UiPixelCoordinate)pcVar4,originY,(UiPixelCoordinate)pMVar5,
                 (UiPixelCoordinate)clipRight_00,control->fieldGrid);
    }
    if ((control->contextFlags & 0x2000000) != 0) {
      SelectionOverlay_DrawMarkerB1B2ForProjectedVertexMask1800
                ((UiPixelCoordinate)pcVar4,originY,(UiPixelCoordinate)pMVar5,
                 (UiPixelCoordinate)clipRight_00,(byte)control->overlayMarkerStateB4,
                 control->fieldGrid);
    }
    if ((((control->contextFlags & 0x4000) != 0) && (control->fieldGrid != (FieldGridAsset *)0x0))
       && ((g_UiCommandRuntimeFlags & 0x40) != 0)) {
      SelectionOverlay_DrawMarkerAFForProjectedVertexFlag8000
                ((UiPixelCoordinate)pcVar4,originY,(UiPixelCoordinate)pMVar5,
                 (UiPixelCoordinate)clipRight_00,control->fieldGrid);
    }
  }
  g_SelectionPanelBlitOpaque = g_GraphicsTextureSourceBlitSourceAlpha;
  g_SelectionPanelBlitClipped = g_GraphicsTextureSourceBlitTiledSourceAlpha;
  (*g_SpinLockReleaseAndInvoke)(control->renderSpinLockReleaseCallback,control->renderSpinLock);
  if ((control->selectedModelNode != (ModelRuntimeNode *)0x0) &&
     ((int)control->callbackArgumentF0 < control->selectedHitMetric)) {
    control->selectedModelNode = (ModelRuntimeNode *)0x0;
  }
  UiContainer_DrawIntersectingChildren(clipTop,clipLeft,clipBottom,clipRight,&control->base);
  return;
}


/* Address: 0x0050C6E0.
   Ownership: ui/frontend/runtime.
   Purpose: Exact packed function-table or callback-registration provenance plus immutable body topology prove this
   callable entry.
*/
void __thandor_void_preserve_eax_ecx_edx
FrontendModelPointerContext_RightPress
          (UiPointerWheelDelta wheelDelta,UiPixelCoordinate pointerY,UiPixelCoordinate pointerX,
          FrontendModelPointerContextRuntimeState17C *callbackContext)

{
  callbackContext->capturedPointerX = pointerX;
  callbackContext->capturedPointerY = pointerY;
  callbackContext->capturedWheelDelta = wheelDelta;
  callbackContext->contextFlags =
       callbackContext->contextFlags |
       FRONTEND_MODEL_POINTER_CONTEXT_ROUTE_TO_BUILTIN_ACTION_RESOLUTION;
  callbackContext->rightButtonState11C = 0;
  g_CursorUseOverridePosition = g_CursorUseOverridePosition + 1;
  return;
}


/* Address: 0x0050C730.
   Ownership: ui/frontend/runtime.
   Purpose: Exact packed function-table or callback-registration provenance plus immutable body topology prove this
   callable entry.
*/
void __thandor_preserve_eax_edx
FrontendModelPointerContext_RightRelease
          (UiPointerWheelDelta wheelDelta,UiPixelCoordinate pointerY,UiPixelCoordinate pointerX,
          FrontendModelPointerContextRuntimeState17C *callbackContext)

{
  g_CursorUseOverridePosition = 0;
  callbackContext->contextFlags = callbackContext->contextFlags & 0xffffffb0;
  if ((callbackContext->rightButtonState11C < 7) &&
     ((code *)callbackContext->rightReleaseCallback118 != (code *)0x0)) {
    (*(code *)callbackContext->rightReleaseCallback118)(callbackContext);
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
void __thandor_void_preserve_eax_ecx_edx
FrontendModelPointerContext_DispatchWorldCameraPointerInput
          (UiPointerWheelDelta wheelDelta,UiPixelCoordinate pointerY,UiPixelCoordinate pointerX,
          WorldRuntimeContext *callbackContext)

{
  uint uVar1;
  InGameWorldTransientStateClearCallbackProc *pIVar2;
  dword screenDeltaY;
  AngleTurn32 elevationAngle;
  int pitchDeltaInput;
  
  screenDeltaY = pointerX - callbackContext->pointerCaptureX;
  elevationAngle = pointerY - callbackContext->pointerCaptureY;
  if ((callbackContext->runtimeFlags & 0x10) != 0) {
    return;
  }
  if ((callbackContext->runtimeFlags & 0x100) != 0) {
    if ((g_CursorButtonState & 1) == 0) {
      callbackContext->runtimeFlags = callbackContext->runtimeFlags & 0xfffffffa;
      callbackContext->runtimeFlags = callbackContext->runtimeFlags | 10;
      WorldMotion_AdjustHeadingAndRecomputePosition(screenDeltaY,callbackContext);
      WorldMotion_AdjustPitchClampAndRecomputePosition(elevationAngle,callbackContext);
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
                  (elevationAngle,screenDeltaY,callbackContext);
        WorldMotion_TranslateCurrentAndTargetByNegatedPitchReverseHeading
                  (elevationAngle,callbackContext);
      }
      else {
        callbackContext->runtimeFlags = callbackContext->runtimeFlags & 0xfffffffa;
        callbackContext->runtimeFlags = callbackContext->runtimeFlags | 10;
        WorldMotion_AdjustHeadingAndClearFieldGridDirty(screenDeltaY,callbackContext);
        WorldMotion_AdjustPitchClampAndClearFieldGridDirty(pitchDeltaInput,callbackContext);
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
  (*g_PointerSetPosition)(callbackContext->pointerCaptureY,callbackContext->pointerCaptureX);
  pIVar2 = (callbackContext->fieldRegion).clearTransientStateCallback;
  WorldRuntime_CaptureMotionStateToSnapshot(callbackContext);
  if (pIVar2 != (InGameWorldTransientStateClearCallbackProc *)0x0) {
    (*pIVar2)(callbackContext);
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
void __thandor_void_preserve_eax_ecx_edx
FrontendModelPointerContext_PointerWheel
          (UiPointerWheelDelta wheelDelta,UiPixelCoordinate pointerY,UiPixelCoordinate pointerX,
          WorldRuntimeContext *callbackContext)

{
  int iVar1;
  
  if (((callbackContext->runtimeFlags & 0x10) == 0) &&
     ((callbackContext->runtimeFlags & 0x8300) != 0)) {
    if ((g_KeyboardStateMask & 0xc) == 0) {
      iVar1 = wheelDelta * g_WorldMotionPointerWheelInputScale;
      callbackContext->runtimeFlags = callbackContext->runtimeFlags & 0xfffffff4;
      callbackContext->runtimeFlags = callbackContext->runtimeFlags | 4;
      WorldMotion_AdjustDistanceClampAndRecomputePosition(iVar1,callbackContext);
      WorldRuntime_CaptureMotionStateToSnapshot(callbackContext);
    }
    else {
      iVar1 = wheelDelta * g_WorldMotionPointerWheelInputScale;
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
bool __thandor_cf_preserve_eax_ecx_edx
FrontendModelPointerContext_KeyboardEventCf
          (UiKeyboardStateMask keyboardStateMask,UiKeyboardEventCode keyCode,
          FrontendModelPointerContextRuntimeState118 *control)

{
  bool bVar1;
  
  if ((control->keyboardFallbackCf != (UiRootKeyboardFallbackCf *)0x0) &&
     (bVar1 = (*control->keyboardFallbackCf)(keyboardStateMask,keyCode,(UiRootNode *)control),
     !bVar1)) {
    return bVar1;
  }
  bVar1 = UiNode_DefaultKeyboardEventMoveFocusNextCf(keyboardStateMask,keyCode,&control->base);
  return bVar1;
}


/* Address: 0x0050CF90.
   Ownership: ui/frontend/runtime.
   Purpose: Exact packed function-table or callback-registration provenance plus immutable body topology prove this
   callable entry.
   Cross-module calls: FixedMath_DirectionFromAnglesScaledRegs [core/math/fixed],
   WorldRuntime_ClearFieldGridDirtyFlag [world/runtime/core].
*/
void __thandor_void_preserve_eax_ecx_edx
FrontendModelPointerContext_Tick(WorldRuntimeContext *callbackContext)

{
  dword *pdVar1;
  UQ12 UVar2;
  UQ12 UVar3;
  UQ12 UVar4;
  FixedDirectionXyzRegs12 FVar5;
  
  if ((callbackContext->runtimeFlags & 0x40) != 0) {
    pdVar1 = &(callbackContext->selection).reservedCallbackState40;
    *pdVar1 = *pdVar1 + 1;
  }
  if ((callbackContext->runtimeFlags & 0x48110) == 0) {
    UVar4 = (callbackContext->motion).committedDistanceQ12;
    if ((int)UVar4 < (int)callbackContext->minimumCameraDistanceQ12) {
      UVar4 = callbackContext->minimumCameraDistanceQ12;
    }
    if ((int)callbackContext->maximumCameraDistanceQ12 < (int)UVar4) {
      UVar4 = callbackContext->maximumCameraDistanceQ12;
    }
    UVar2 = (callbackContext->motion).targetDistanceQ12;
    UVar3 = g_WorldMotionTargetDistanceConvergenceStepQ12;
    if ((int)(UVar4 * 0xf) >> 4 <= (int)UVar2) {
      if ((int)UVar2 <= (int)(UVar4 * 0x11) >> 4) {
        return;
      }
      UVar3 = -g_WorldMotionTargetDistanceConvergenceStepQ12;
    }
    (callbackContext->motion).targetDistanceQ12 = UVar2 + UVar3;
    FVar5 = FixedMath_DirectionFromAnglesScaledRegs
                      (-(callbackContext->motion).pitchAngle,
                       (callbackContext->motion).headingAngle ^ 0x8000,UVar2 + UVar3);
    (callbackContext->motion).positionXQ12 =
         FVar5.eax + (callbackContext->motion).targetPositionXQ12;
    (callbackContext->motion).positionYQ12 =
         FVar5.ecx + (callbackContext->motion).targetPositionYQ12;
    (callbackContext->motion).positionZQ12 =
         FVar5.edx + (callbackContext->motion).targetPositionZQ12;
    WorldRuntime_ClearFieldGridDirtyFlag(callbackContext);
  }
  return;
}


/* Address: 0x00514E40.
   Ownership: ui/frontend/runtime.
   Purpose: Copies the selected faction's Q4 resource and progress values into the frontend runtime cache and
   formats the primary amount into the verified UTF-16 display buffer.
*/
void __thandor_void_preserve_eax_ecx_edx FrontendRuntime_UpdateCurrentFactionMetricCache(void)

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
bool __thandor_cf_preserve_eax_ecx_edx
FrontendRuntime_DispatchCommandByCodeAndModifierFlagsCf
          (UiKeyboardStateMask modifierFlags,UiActionId commandCode,void *frontendRuntime)

{
  bool bVar1;
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
        return true;
      }
      dispatchRecordCursor = currentDispatchRecord + 1;
    } while (currentDispatchRecord->commandCode != commandCode);
    if (recordModifierFlags != 0) break;
    bVar1 = false;
    if ((modifierFlags & 0x3f) == 0) {
      (*(code *)currentDispatchRecord->continuationEntryAddress)();
      return bVar1;
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
    if (((modifierFlags & 0xc) != 0) && (bVar1 = false, (modifierFlags & 0x30) == 0)) {
      (*(code *)currentDispatchRecord->continuationEntryAddress)();
      return bVar1;
    }
  }
  else if ((recordModifierFlags & 0xc) == 0) {
    if (((modifierFlags & 0xc) == 0) && (bVar1 = false, (modifierFlags & 0x30) != 0)) {
      (*(code *)currentDispatchRecord->continuationEntryAddress)();
      return bVar1;
    }
  }
  else if (((modifierFlags & 0xc) != 0) && (bVar1 = false, (modifierFlags & 0x30) != 0)) {
    (*(code *)currentDispatchRecord->continuationEntryAddress)();
    return bVar1;
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
void __thandor_void_preserve_eax_ecx_edx FrontendState_DispatchCode(FrontendStatusCode stateCode)

{
  FrontendRomActionTable_ExecuteRecord(0,0,0,stateCode);
  return;
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
dword __thandor_eax_preserve_ecx_edx
FrontendRuntime_UpdatePointerContextAndSceneViewCf
          (dword pointerValue0,dword pointerValue1,dword pointerValue2,dword pointerValue3,
          void *pointedRecord,FrontendPointerSceneRuntimeView43E8 *frontendRuntime)

{
  sdword *psVar1;
  word *pwVar2;
  UiNodeBase *control;
  UiNodeVtable *pUVar3;
  RomAssetRecordPrefix *pRVar4;
  int *piVar5;
  dword dVar6;
  word *commandStream;
  RomRecordId recordId;
  int iVar7;
  int iVar8;
  int iVar9;
  RichTextExtentRegs RVar10;
  StatusValueEaxCf5 SVar11;
  TextResourceResolveEaxCf5 TVar12;
  GraphicsTextureSizeEaxEdxCf9 GVar13;
  
  if (((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_CLIENT) != SESSION_NETWORK_ROLE_LOCAL) ||
     (SVar11 = UiPageStack_ActivePageNotInListCf(&frontendRuntime->activePageStack1A0),
     SVar11.valueOrError != 0)) {
    dVar6 = 0;
    iVar7 = 0;
    goto FrontendPointerContext_RefreshHintText;
  }
  pRVar4 = RomRegistry_FindRecordBySlotValue((RomRegistrySlotValue)pointedRecord);
  recordId = 0xf0000000;
  if (pRVar4 != (RomAssetRecordPrefix *)0x0) {
    recordId = pRVar4->recordId;
  }
  iVar7 = 0;
  piVar5 = RomRecordTable_FindRecordById(recordId,g_FrontendActiveRomRecordTable);
  dVar6 = 0;
  if (((piVar5 == (int *)0x0) ||
      (((((dVar6 = 0, piVar5[8] == 3 || (piVar5[8] == 4)) || (piVar5[8] == 9)) || (piVar5[8] < 0))
       && ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_NETWORKED_MASK) !=
           SESSION_NETWORK_ROLE_LOCAL)))) ||
     ((piVar5[8] == 2 && (g_NetworkBackendInstanceCount == 0))))
  goto FrontendPointerContext_RefreshHintText;
  if ((*piVar5 == g_FrontendRomTransitionKeyframe1Channel0Q12) &&
     ((piVar5[1] == g_FrontendRomTransitionKeyframe1Channel1Q12 &&
      (piVar5[2] == g_FrontendRomTransitionKeyframe1Channel2Q12)))) {
    iVar7 = piVar5[3];
    iVar9 = piVar5[4];
    iVar8 = piVar5[5];
    if (((iVar7 != g_FrontendRomTransitionKeyframe1Channel3Q12) ||
        (iVar9 != g_FrontendRomTransitionKeyframe1Channel4Q12)) ||
       (iVar8 != g_FrontendRomTransitionKeyframe1Channel5Q12))
    goto FrontendRomTransition_RebuildSpline;
  }
  else {
    iVar7 = piVar5[3];
    iVar9 = piVar5[4];
    iVar8 = piVar5[5];
    g_FrontendRomTransitionKeyframe1Channel0Q12 = *piVar5;
    g_FrontendRomTransitionKeyframe1Channel1Q12 = piVar5[1];
    g_FrontendRomTransitionKeyframe1Channel2Q12 = piVar5[2];
FrontendRomTransition_RebuildSpline:
    g_FrontendRomTransitionKeyframe0Channel0Q12 = frontendRuntime->hitReferenceWorldXQ12;
    g_FrontendRomTransitionKeyframe0Channel1Q12 = frontendRuntime->hitReferenceWorldYQ12;
    g_FrontendRomTransitionKeyframe0Channel2Q12 = frontendRuntime->hitReferenceWorldZQ12;
    g_FrontendRomTransitionKeyframe0Channel3Q12 = frontendRuntime->projectionScale;
    g_FrontendRomTransitionKeyframe0Channel4Q12 = frontendRuntime->viewAngle0;
    g_FrontendRomTransitionKeyframe0Channel5Q12 = frontendRuntime->viewAngle1;
    g_FrontendRomTransitionKeyframe0TimeQ12 = 0;
    g_FrontendRomTransitionKeyframe1TimeQ12 = 0xc0;
    g_FrontendRomTransitionElapsedTicks = 0;
    g_FrontendRomTransitionPendingCount = 0xffffffff;
    g_FrontendRomTransitionSplineKeyframeCount = 2;
    g_FrontendRomTransitionSplineKeyframes = &g_FrontendRomTransitionKeyframe0Channel0Q12;
    g_FrontendRomTransitionKeyframe1Channel3Q12 = iVar7;
    g_FrontendRomTransitionKeyframe1Channel4Q12 = iVar9;
    g_FrontendRomTransitionKeyframe1Channel5Q12 = iVar8;
    WorldMotionSpline_BuildSixChannelCurves
              (2,(WorldMotionSplineKeyframe *)&g_FrontendRomTransitionKeyframe0Channel0Q12);
  }
  iVar7 = piVar5[6];
  dVar6 = 7;
FrontendPointerContext_RefreshHintText:
  if (iVar7 == 0) {
    if (g_FrontendPendingPageActionDepth == 0) {
      (frontendRuntime->hintControl4390).hintActive50 = 0;
      (frontendRuntime->hintControl4390).commandStream54 = (word *)0x0;
      return dVar6;
    }
    iVar7 = 1;
  }
  pwVar2 = (frontendRuntime->hintControl4390).commandStream54;
  TVar12 = TextResource_Resolve(iVar7 + 0x2000);
  commandStream = TVar12.eax;
  if (commandStream != pwVar2) {
    (frontendRuntime->hintControl4390).commandStream54 = commandStream;
    RVar10 = RichTextCommandStream_MeasureRegs(g_UiTextStyleNormal,commandStream);
    iVar7 = (int)(RVar10.widthPixels + 1) >> 1;
    iVar9 = (int)(RVar10.heightPixels + 1) >> 1;
    (frontendRuntime->hintControl4390).base.leftOffset = iVar7;
    (frontendRuntime->hintControl4390).base.topOffset = iVar9;
    (frontendRuntime->hintControl4390).base.right = -iVar7;
    (frontendRuntime->hintControl4390).base.bottom = -iVar9;
    GVar13 = (*g_GraphicsTextureSourceGetLogicalSize)(0x72,g_UiWindowTextureSource);
    iVar7 = GVar13.logicalWidthPixels + 3;
    control = (frontendRuntime->hintControl4390).base.nextSibling;
    (frontendRuntime->hintControl4390).hintActive50 = 1;
    psVar1 = &(frontendRuntime->hintControl4390).base.leftOffset;
    *psVar1 = *psVar1 + iVar7;
    psVar1 = &(frontendRuntime->hintControl4390).base.topOffset;
    *psVar1 = *psVar1 + GVar13.logicalHeightPixels;
    pUVar3 = control->vtable;
    psVar1 = &(frontendRuntime->hintControl4390).base.right;
    *psVar1 = *psVar1 - iVar7;
    psVar1 = &(frontendRuntime->hintControl4390).base.bottom;
    *psVar1 = *psVar1 - GVar13.logicalHeightPixels;
    (*pUVar3->layout)(control);
  }
  return dVar6;
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
void __thandor_void_preserve_eax_ecx_edx
FrontendRuntimeCallback64_DispatchRecord1350
          (dword argument1,dword argument2,dword argument3,dword argument4,
          FrontendCallbackArgument5 argument5,dword argument6)

{
  RomAssetRecordPrefix *pRVar1;
  RomRecordTableIndex recordIndex;
  
  if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_CLIENT) == SESSION_NETWORK_ROLE_LOCAL) {
    pRVar1 = RomRegistry_FindRecordBySlotValue(argument5);
    if (pRVar1 != (RomAssetRecordPrefix *)0x0) {
      recordIndex = RomRecordTable_FindIndexById(pRVar1->recordId,g_FrontendActiveRomRecordTable);
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
  return;
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
void __thandor_void_preserve_eax_ecx_edx FrontendRecentTextHistory_InsertAndRebuild5(word *text)

{
  RecentTextHistoryPointerList *output;
  
  output = (RecentTextHistoryPointerList *)(g_FrontendRootNode + 0x350);
  RecentTextHistory_Insert(text);
  RecentTextHistory_SortAndBuildPointerList(5,output);
  return;
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
void FrontendCallback_ApplyGameSpeedOrDispatch02C0(dword callbackArgument)

{
  if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_NETWORKED_MASK) ==
      SESSION_NETWORK_ROLE_LOCAL) {
    FrontendSession_ApplyGameSpeedAndReturnToMainPage(g_LocalPlayerRuntimeId,0,0,0);
  }
  else {
    FrontendCommandQueue_EnqueueLocalPlayerCommand(0x2c0,0,0,0);
  }
  return;
}


/* Address: 0x00549140.
   Ownership: ui/frontend/runtime.
   Purpose: One-argument frontend action callback. Local mode releases the selected resource and returns to the
   main page; network modes dispatch message offset 0x320. Existing register results are preserved. Queued UI
   action handler for FRONTEND_PAGE20[79] (0x204F). Return datatype is preserved for non-queue direct callers.
   Cross-module calls: FrontendSession_ReleaseSelectedResourceAndReturnToMainPage [ui/frontend/session],
   FrontendCommandQueue_EnqueueLocalPlayerCommand [network/protocol/commands].
*/
void FrontendCallback_ReleaseSelectedResourceOrDispatch0320(dword callbackArgument)

{
  if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_NETWORKED_MASK) ==
      SESSION_NETWORK_ROLE_LOCAL) {
    FrontendSession_ReleaseSelectedResourceAndReturnToMainPage(g_LocalPlayerRuntimeId,0,0,0);
  }
  else {
    FrontendCommandQueue_EnqueueLocalPlayerCommand(800,0,0,0);
  }
  return;
}


/* Address: 0x00549180.
   Ownership: ui/frontend/runtime.
   Purpose: One-argument no-op callback referenced by the large frontend callback table. The exact original action
   label remains unresolved. Queued UI action handler for FRONTEND_PAGE20[80] (0x2050). Return datatype is
   preserved for non-queue direct callers.
*/
void FrontendCallback_NoOpArg1(void *source)

{
  return;
}


/* Address: 0x00549AB0.
   Ownership: ui/frontend/runtime.
   Purpose: One-argument frontend action callback. Local mode returns to the main page with state zero; network
   modes dispatch message offset 0xDC0. Existing register results are preserved. Queued UI action handler for
   FRONTEND_PAGE20[64] (0x2040). Return datatype is preserved for non-queue direct callers.
   Cross-module calls: FrontendSession_ReturnToMainPage [ui/frontend/session],
   FrontendCommandQueue_EnqueueLocalPlayerCommand [network/protocol/commands].
*/
void __thandor_preserve_eax FrontendCallback_ReturnToMainPageOrDispatch0DC0(dword callbackArgument)

{
  if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_NETWORKED_MASK) ==
      SESSION_NETWORK_ROLE_LOCAL) {
    FrontendSession_ReturnToMainPage(g_LocalPlayerRuntimeId,0,0,0);
  }
  else {
    FrontendCommandQueue_EnqueueLocalPlayerCommand(0xdc0,0,0,0);
  }
  return;
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
void FrontendCallback_ReturnToMainPageOrDispatchState4(dword callbackArgument)

{
  if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_NETWORKED_MASK) ==
      SESSION_NETWORK_ROLE_LOCAL) {
    if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_NETWORKED_MASK) ==
        SESSION_NETWORK_ROLE_LOCAL) {
      FrontendSession_ReturnToMainPage(g_LocalPlayerRuntimeId,0,0,0);
    }
    else {
      FrontendCommandQueue_EnqueueLocalPlayerCommand(0xdc0,0,0,0);
    }
  }
  else if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_NETWORKED_MASK) ==
           SESSION_NETWORK_ROLE_LOCAL) {
    FrontendSession_ReturnToMainPage(g_LocalPlayerRuntimeId,0,0,4);
  }
  else {
    FrontendCommandQueue_EnqueueLocalPlayerCommand(0xdc0,0,0,4);
  }
  return;
}


/* Address: 0x0054A7D0.
   Ownership: ui/frontend/runtime.
   Purpose: Second one-argument callback slot with the same verified local-main-page versus network-0xDC0 behavior.
   Existing register results are preserved. Queued UI action handler for FRONTEND_PAGE20[51] (0x2033). Return
   datatype is preserved for non-queue direct callers.
   Cross-module calls: FrontendSession_ReturnToMainPage [ui/frontend/session],
   FrontendCommandQueue_EnqueueLocalPlayerCommand [network/protocol/commands].
*/
void __thandor_preserve_eax
FrontendCallback_ReturnToMainPageOrDispatch0DC0_Secondary(dword callbackArgument)

{
  if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_NETWORKED_MASK) ==
      SESSION_NETWORK_ROLE_LOCAL) {
    FrontendSession_ReturnToMainPage(g_LocalPlayerRuntimeId,0,0,0);
  }
  else {
    FrontendCommandQueue_EnqueueLocalPlayerCommand(0xdc0,0,0,0);
  }
  return;
}


/* Address: 0x0054AAD0.
   Ownership: ui/frontend/runtime.
   Purpose: Recovered action-table target FRONTEND_PAGE20[16] (0x2010).
   Cross-module calls: FrontendSession_ReturnToMainPage [ui/frontend/session],
   FrontendCommandQueue_EnqueueLocalPlayerCommand [network/protocol/commands], UiPageStack_SetActiveIndex
   [ui/controls/layout].
*/
void __thandor_preserve_eax_edx FrontendUiAction2010_Handler(UiNodeBase *sourceNode)

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
void __thandor_void_preserve_eax_ecx_edx
FrontendUiAction2011_Handler(FrontendDisplaySettingsPageOptionState1010 *source)

{
  byte *pbVar1;
  GraphicsAdapterRecord *pGVar2;
  word *pwVar3;
  uint uVar4;
  undefined4 uVar5;
  undefined4 uVar6;
  GraphicsDisplayModeCount GVar7;
  GraphicsDisplayMode *pGVar8;
  TextResourceResolveEaxCf5 TVar9;
  
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
    TVar9 = TextResource_Resolve(0x212d);
    pwVar3 = TVar9.eax;
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
      TVar9 = TextResource_Resolve(0x212d);
      pwVar3 = TVar9.eax;
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
      TVar9 = TextResource_Resolve(0x212d);
      pwVar3 = TVar9.eax;
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
      TVar9 = TextResource_Resolve(0x212d);
      pwVar3 = TVar9.eax;
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
      TVar9 = TextResource_Resolve(0x212d);
      pwVar3 = TVar9.eax;
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
void __thandor_void_preserve_eax_ecx
FrontendUiAction202CTo2030_SharedHandler(UiNodeBase *sourceNode)

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
void __thandor_void_preserve_eax_ecx_edx
FrontendUiAction200C_Handler(UiPointerListControl *sessionListControl)

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
void __thandor_void_preserve_eax_ecx FrontendRecentText_TrimAndSortTopFive(UiNodeBase *source)

{
  uint currentEntryCount;
  
  for (currentEntryCount = (uint)source[1].vtable; (UiNodeVtable *)0x4 < currentEntryCount;
      currentEntryCount = (int)&((UiNodeVtable *)(currentEntryCount + -0x48))->pointerWheel + 3) {
    RecentTextHistory_RemoveOldest();
  }
  RecentTextHistory_RemoveOldest();
  RecentTextHistory_SortAndBuildPointerList(5,(RecentTextHistoryPointerList *)&source[1].vtable);
  return;
}


/* Address: 0x0054D4A0.
   Ownership: ui/frontend/runtime.
   Purpose: Recovered action-table target FRONTEND_PAGE20[15] (0x200F).
   Cross-module calls: UiPointerList_GetSelectedIndexVariantACf [ui/controls/lists], UiNodeList_SuppressActionId
   [ui/controls/lists], UiPointerList_InitializeColumnLayout [ui/controls/lists],
   UiTransfer_SendPacketType10000Value2931Cf [network/protocol/transfer], FrontendSession_ReturnToMainPage
   [ui/frontend/session], FrontendCommandQueue_EnqueueLocalPlayerCommand [network/protocol/commands].
*/
void __thandor_void_preserve_eax_ecx_edx
FrontendUiAction200F_Handler(FrontendNetworkSetupPageBackendListPtr backendList)

{
  UiListRowIndex returnValue;
  int iVar1;
  dword *endpointSourceDwordCursor;
  dword *endpointDestinationDwordCursor;
  NetworkBackendSetSessionEaxCf5 NVar2;
  FatalErrorEaxCf5 FVar3;
  NetworkBackendOpenBindEaxCf5 NVar4;
  NetworkSessionContext *pNVar5;
  UiListRowIndex UVar6;
  
  returnValue = UiPointerList_GetSelectedIndexVariantACf(backendList);
  pNVar5 = g_NetworkBackendSessionContext;
  if (g_NetworkBackendInstanceCount <= returnValue) {
    return;
  }
  (*g_NetworkBackendSlot3)();
  (*g_NetworkBackendSlot1)();
  UVar6 = returnValue;
  NVar2 = (*g_NetworkBackendSlot0)(returnValue);
  FVar3 = (*g_FatalErrorRuntimeDispatchCf)(NVar2.eax,NVar2.carry);
  if (!FVar3.carry) {
    NVar4 = (*g_NetworkBackendSlot2)(0x3a1);
    FVar3 = (*g_FatalErrorRuntimeDispatchCf)(NVar4.eax,NVar4.carry);
    if (!FVar3.carry) {
      endpointSourceDwordCursor = (dword *)&g_NetworkLocalEndpointDescriptor16;
      endpointDestinationDwordCursor = (dword *)&g_FrontendNetworkEndpointScratch;
      for (iVar1 = 4; iVar1 != 0; iVar1 = iVar1 + -1) {
        *endpointDestinationDwordCursor = *endpointSourceDwordCursor;
        endpointSourceDwordCursor = endpointSourceDwordCursor + 1;
        endpointDestinationDwordCursor = endpointDestinationDwordCursor + 1;
      }
      (*g_NetworkBackendSlot7)
                (&g_FrontendNetworkEndpointTextUtf16,
                 (WinSockAddress *)&g_FrontendNetworkEndpointScratch);
      UiNodeList_SuppressActionId(0x2002,&ADJ(backendList).rootNode);
      UiPointerList_InitializeColumnLayout
                (0,g_FrontendSessionListRows,&ADJ(backendList)->sessionList);
      UiTransfer_SendPacketType10000Value2931Cf();
      return;
    }
    (*g_NetworkBackendSlot1)(UVar6);
  }
  NVar2 = (*g_NetworkBackendSlot0)(returnValue);
  if (!NVar2.carry) {
    NVar4 = (*g_NetworkBackendSlot2)(0x3a1);
    if (!NVar4.carry) {
      return;
    }
    (*g_NetworkBackendSlot1)(pNVar5);
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
void __thandor_void_preserve_eax_ecx_edx Frontend_PlaySelectedEndMovie(void)

{
  UiRootCallbacks *pUVar1;
  uint uVar2;
  ulonglong uVar3;
  InGameRuntimeRootImageC3E4 *pIVar4;
  sdword arg4;
  int iVar5;
  dword arg0;
  TextResourceId resourceId;
  FrontendPlayerRuntimeBlockCount FVar6;
  UiPageStackControl *stack;
  WorldRuntimeContext *worldRuntime;
  int iVar7;
  int factionIndex;
  byte *pbVar8;
  FactionRuntimeLifecycleObservedState *pFVar9;
  FrontendPlayerRuntimeRecord *pFVar10;
  byte *pbVar11;
  bool bVar12;
  MovieOpenEaxCf5 MVar13;
  MovieAdvanceFrameEaxCf5 MVar14;
  TextResourceResolveEaxCf5 TVar15;
  TextResourceResolveEaxCf5 TVar16;
  
  pIVar4 = g_InGameRuntimeRoot;
  (*g_GraphicsCursorSetFrame)(0);
  g_CursorVisibilityToken = g_CursorVisibilityToken + -1;
  if ((pIVar4 != (InGameRuntimeRootImageC3E4 *)0x0) &&
     (pUVar1 = (pIVar4->rootUi0000).callbacks, g_EndMoviePath != (word *)0x0)) {
    pUVar1->keyboardFallbackCf = EndMovieUiRuntime_DispatchCommandByFlagsCf;
    pUVar1->frameUpdate = EndMovieUiRuntime_HandleModeTransitionCf;
    if (g_FrontendLoadedCampaignAsset != 0) {
      iVar5 = *(int *)(g_FrontendLoadedCampaignAsset + 0xb8);
      iVar7 = g_FrontendLoadedCampaignAsset + 0x200;
      do {
        if (*(int *)(g_FrontendLoadedCampaignAsset + 0xc4) == *(int *)(iVar7 + 0x100)) {
          if (g_EndMovieVariantIndex == 0) {
            arg4 = *(sdword *)(iVar7 + 0x40 + g_EndMovieSelectionIndex * 4);
          }
          else {
            arg4 = *(sdword *)(iVar7 + 0x20 + g_EndMovieSelectionIndex * 4);
          }
          (*g_WideNumberFormatUtf16)
                    (WIDE_FORMAT_PAD_WITH_ZERO,0,4,1,arg4,(word *)(u_flm_ende0000_flm_0050df4a + 8))
          ;
          g_EndMoviePath = (word *)u_flm_ende0000_flm_0050df4a;
          break;
        }
        iVar7 = iVar7 + 0x180;
        iVar5 = iVar5 + -1;
      } while (iVar5 != 0);
    }
    Movie_Close();
    bVar12 = (*g_GraphicsFramebufferBeginAccess)();
    if (!bVar12) {
      (*g_GraphicsFramebufferFillRectArgb)
                (g_FramebufferHeight,g_FramebufferWidth,0,0,g_FramebufferHeight,g_FramebufferWidth,0
                 ,0,0xff000000,g_FramebufferAccess);
      (*g_GraphicsFramebufferEndAccess)();
      (*g_GraphicsFramebufferPresent)(g_FramebufferAccess);
    }
    bVar12 = (*g_GraphicsFramebufferBeginAccess)();
    if (!bVar12) {
      (*g_GraphicsFramebufferFillRectArgb)
                (g_FramebufferHeight,g_FramebufferWidth,0,0,g_FramebufferHeight,g_FramebufferWidth,0
                 ,0,0xff000000,g_FramebufferAccess);
      (*g_GraphicsFramebufferEndAccess)();
      (*g_GraphicsFramebufferPresent)(g_FramebufferAccess);
    }
    MVar13 = Movie_Open(1,g_EndMoviePath);
    pIVar4 = g_InGameRuntimeRoot;
    if (!MVar13.carry) {
      g_EndMoviePendingTicks = 0;
      (*g_TimerRegisterPeriodic)(arg0,FrontendSession_PeriodicTick);
      UiPageStack_SetActiveIndex(1,stack);
      MVar14 = Movie_AdvanceFrame();
      if (!MVar14.carry) {
        pIVar4->activeEndMovieRuntime022C = (MovieRuntime *)MVar14.eax;
        pIVar4->endMoviePlaybackState0230 = 0;
        g_EndMoviePendingTicks = 0;
        do {
          if (g_EndMoviePendingTicks != 0) {
            g_EndMoviePendingTicks = g_EndMoviePendingTicks - 1;
            MVar14 = Movie_AdvanceFrame();
            if (MVar14.carry) {
              g_UiCommandRuntimeFlags = g_UiCommandRuntimeFlags & 0xfffff7ff;
            }
          }
          UiNode_InvalidateRoot((UiNodeBase *)pIVar4);
          UiFrame_ProcessAndPresent();
        } while ((g_UiCommandRuntimeFlags & 0x800) != 0);
      }
      g_CursorVisibilityToken = g_CursorVisibilityToken + 1;
      UiPageStack_SetActiveIndex(1,&pIVar4->endMoviePageStack02F8);
      iVar7 = 7;
      pFVar9 = g_GameFactionRuntimeImage.tail.factionLifecycleStates;
      iVar5 = 0;
      factionIndex = 1;
      do {
        pFVar9 = pFVar9 + 1;
        if (*pFVar9 != 0) {
          iVar5 = iVar5 + 1;
          GameFactionRuntime_RecomputeProgressAndScoreMetrics
                    (factionIndex,&pIVar4->worldRuntime0A30);
        }
        factionIndex = factionIndex + 1;
        iVar7 = iVar7 + -1;
      } while (iVar7 != 0);
      if (iVar5 != 0) {
        *(int *)(pIVar4->opaque034C_08D3 + 0x114) = iVar5;
        *(int *)(pIVar4->opaque034C_08D3 + 400) = iVar5;
        *(int *)(pIVar4->opaque034C_08D3 + 0x20c) = iVar5;
        uVar3 = (ulonglong)(g_GameFactionRuntimeImage.tail.periodicClockTick + 0x12bf) / 0x12c0;
        (*g_LocaleFormatTimeFieldsUtf16)
                  ((dword)(uVar3 / 0x3c),(dword)(uVar3 % 0x3c),
                   (word *)&g_EndGameElapsedTimeScratchUtf16);
        TVar15 = TextResource_Resolve(0x21c0);
        resourceId = g_InGameLevelTitleTextResourceIndex + 0x2230;
        RichTextCommandStream_PatchPayloadBySelector(1,&g_EndGameElapsedTimeScratchUtf16,TVar15.eax)
        ;
        TVar16 = TextResource_Resolve(resourceId);
        RichTextCommandStream_PatchPayloadBySelector(0,TVar16.eax,TVar15.eax);
        UiNodeList_UnsuppressActionId(0x101b,(UiNodeBase *)pIVar4);
        if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_NETWORKED_MASK) ==
            SESSION_NETWORK_ROLE_LOCAL) {
          UiNodeList_SuppressActionId(0x1025,(UiNodeBase *)pIVar4);
        }
        FVar6 = g_FrontendPlayerRuntimeBlockCount;
        pFVar10 = g_FrontendPlayerRuntimeBlocks;
        if (((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_HOST) != SESSION_NETWORK_ROLE_LOCAL)
           && (1 < g_FrontendPlayerRuntimeBlockCount)) {
          UiNodeList_SuppressActionId(0x101b,(UiNodeBase *)pIVar4);
          FVar6 = g_FrontendPlayerRuntimeBlockCount;
          pFVar10 = g_FrontendPlayerRuntimeBlocks;
        }
        do {
          (pFVar10->factionAssignment).readyOrWaitState = 0;
          FVar6 = FVar6 - 1;
          pFVar10 = pFVar10 + 1;
        } while (FVar6 != 0);
        if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_NETWORKED_MASK) ==
            SESSION_NETWORK_ROLE_LOCAL) {
          uVar2 = *(uint *)(pIVar4->opaque034C_08D3 + 0x110);
          *(int *)(pIVar4->opaque034C_08D3 + 0x110) = *(int *)(pIVar4->opaque034C_08D3 + 0x110) + -1
          ;
          iVar5 = uVar2 - 3;
          if (2 < uVar2 && iVar5 != 0) {
            pbVar8 = pIVar4->opaque034C_08D3 + 300;
            pbVar11 = pIVar4->opaque034C_08D3 + 0x128;
            for (; iVar5 != 0; iVar5 = iVar5 + -1) {
              *(undefined4 *)pbVar11 = *(undefined4 *)pbVar8;
              pbVar8 = pbVar8 + 4;
              pbVar11 = pbVar11 + 4;
            }
          }
        }
        do {
          UiRootStack_InvalidateAll();
          UiFrame_ProcessAndPresent();
          if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_HOST) != SESSION_NETWORK_ROLE_LOCAL)
          {
            FrontendPlayerRuntime_MarkReadyByIdAndUpdateAction101B(0xffffffff);
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

FrontendInitEaxCf5 __thandor_eax_cf_preserve_ecx_edx Frontend_Init(RomRecordId initialRomRecordId)

{
  SessionNetworkRoleFlags SVar1;
  IDirectSoundBuffer *pIVar2;
  dword dVar3;
  SoundSampleAsset *pSVar4;
  FrontendRootResourceSlots5954 *pFVar5;
  FrontendRootResourceSlots5954 *frontendUiState;
  DirectSoundVoiceSet *arg2;
  FrontendRootResourceSlots5954 *extraout_EAX;
  dword *pdVar6;
  dword *settingsCopySourceDwordsB;
  SessionNetworkRoleFlags SVar7;
  int iVar8;
  dword dVar9;
  dword *frontendInitTemplateDwords;
  word *pwVar10;
  dword *settingsCopySourceDwordsA;
  FrontendPlayerRuntimeRecord *pFVar11;
  dword *menuSoundVoiceSetSlotDwords;
  dword *settingsCopyDestDwordsA;
  dword *pdVar12;
  dword *settingsCopyDestDwordsB;
  bool bVar13;
  GraphicsTextureSetEaxCf5 GVar14;
  GraphicsPaletteAssetEaxCf5 GVar15;
  TextResourceResolveEaxCf5 TVar16;
  SoundCreateSampleVoiceSetEaxCf5 SVar17;
  PackageLoadEntryEaxCf5 PVar18;
  StatusValueEaxCf5 SVar19;
  ArenaAllocEaxCf5 AVar20;
  SoundPlayVoiceEaxCf5 SVar21;
  FrontendInitEaxCf5 FVar22;
  FrontendInitEaxCf5 FVar23;
  ResourceLoadEaxEcxCf9 RVar24;
  WorldRuntimeContext *worldRuntime;
  
  dVar3 = PersistentSettings_ReadDword(0,0x30);
  g_TextureDownsampleShift = dVar3 >> 1;
  pFVar11 = g_FrontendPlayerRuntimeBlocks;
  SVar7 = g_FrontendPlayerRuntimeBlockCount;
  SVar1 = g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_NETWORKED_MASK;
  while (SVar1 != SESSION_NETWORK_ROLE_LOCAL) {
    (pFVar11->factionAssignment).readyOrWaitState = 0;
    pFVar11->commandSyncPending = FRONTEND_COMMAND_SYNC_PENDING;
    pFVar11 = pFVar11 + 1;
    SVar7 = SVar7 - SESSION_NETWORK_ROLE_CLIENT;
    SVar1 = SVar7;
  }
  bVar13 = (*g_GraphicsFramebufferBeginAccess)();
  if (!bVar13) {
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
  GVar14 = (*g_GraphicsTextureSetLoadPackageCf)((word *)u_gfx_texturen_zentrale_gfx_00545acc);
  pFVar5 = (FrontendRootResourceSlots5954 *)GVar14.textureSet;
  if (!GVar14.carry) {
    g_FrontendCentralTextureSet = (FrontendRootResourceSlots5954 *)GVar14.textureSet;
    GVar15 = (*g_GraphicsPaletteAssetLoadPackage)((word *)u_gfx_texturen_zentrale_pal_00545b00);
    pFVar5 = (FrontendRootResourceSlots5954 *)GVar15.paletteAsset;
    if (!GVar15.carry) {
      g_FrontendCentralPaletteAsset = (FrontendRootResourceSlots5954 *)GVar15.paletteAsset;
      TVar16 = TextResource_Resolve(0x2104);
      RichTextCommandStream_PatchPayloadBySelector(0,&g_FrontendNetworkEndpointTextUtf16,TVar16.eax)
      ;
      u_sound_menue01_sam_00545b54[0xb] = L'0';
      u_sound_menue01_sam_00545b54[0xc] = L'1';
      menuSoundVoiceSetSlotDwords = &g_FrontendMenuSoundVoiceSetLoadBaseEntry1;
      do {
        do {
          RVar24 = Resource_Load((word *)u_sound_menue01_sam_00545b54);
          pSVar4 = (SoundSampleAsset *)RVar24.eax;
          if (RVar24.carry) goto Frontend_Init_ContinueWithCentralRomAndRuntimeInitialization;
          SVar17 = (*g_SoundCreateSampleVoiceSet)(pSVar4);
          pFVar5 = (FrontendRootResourceSlots5954 *)SVar17.eax;
          if (SVar17.carry) {
            LOCK();
            UNLOCK();
            Resource_Release(pSVar4);
            goto Frontend_Init_ReturnInitializationFailure;
          }
          *menuSoundVoiceSetSlotDwords = (dword)pFVar5;
          Resource_Release(pSVar4);
          u_sound_menue01_sam_00545b54[0xc] = u_sound_menue01_sam_00545b54[0xc] + L'\x01';
          menuSoundVoiceSetSlotDwords = menuSoundVoiceSetSlotDwords + 1;
        } while ((ushort)u_sound_menue01_sam_00545b54[0xc] < 0x3a);
        u_sound_menue01_sam_00545b54[0xb] = u_sound_menue01_sam_00545b54[0xb] + L'\x01';
        u_sound_menue01_sam_00545b54[0xc] = L'0';
      } while ((ushort)u_sound_menue01_sam_00545b54[0xb] < 0x3a);
Frontend_Init_ContinueWithCentralRomAndRuntimeInitialization:
      PVar18 = Package_LoadEntry((word *)u_engine_zentrale_rom_00545aa4);
      pFVar5 = PVar18.bufferOrError;
      if (!PVar18.carry) {
        g_FrontendCentralRomAsset = pFVar5;
        SVar19 = RomAsset_PrepareRecords((RomAssetHeader *)pFVar5);
        pFVar5 = (FrontendRootResourceSlots5954 *)SVar19.valueOrError;
        if (!SVar19.carry) {
          AVar20 = (*g_MemoryApi.alloc)(0x10000);
          pFVar5 = (FrontendRootResourceSlots5954 *)AVar20.eax;
          if (!AVar20.carry) {
            g_FrontendWorldObjectRecords = (WorldObjectRecord *)pFVar5;
            for (iVar8 = 0x4000; iVar8 != 0; iVar8 = iVar8 + -1) {
              pFVar5->opaqueGap0000_05DF[0] = 0;
              pFVar5->opaqueGap0000_05DF[1] = 0;
              pFVar5->opaqueGap0000_05DF[2] = 0;
              pFVar5->opaqueGap0000_05DF[3] = 0;
              pFVar5 = (FrontendRootResourceSlots5954 *)(pFVar5->opaqueGap0000_05DF + 4);
            }
            AVar20 = (*g_MemoryApi.alloc)(0x5954);
            frontendUiState = (FrontendRootResourceSlots5954 *)AVar20.eax;
            pFVar5 = frontendUiState;
            if (!AVar20.carry) {
              worldRuntime = (WorldRuntimeContext *)(frontendUiState->opaqueGap0000_05DF + 0x368);
              frontendInitTemplateDwords = &g_FrontendRootInitializationTemplate;
              g_FrontendRootNode = frontendUiState;
              for (iVar8 = 0x1655; iVar8 != 0; iVar8 = iVar8 + -1) {
                *(dword *)pFVar5->opaqueGap0000_05DF = *frontendInitTemplateDwords;
                frontendInitTemplateDwords = frontendInitTemplateDwords + 1;
                pFVar5 = (FrontendRootResourceSlots5954 *)(pFVar5->opaqueGap0000_05DF + 4);
              }
              FrontendMenu_BindSharedResources(frontendUiState);
              UiRootStack_Push(&g_UiRootCallbacks_0053DA70,(UiRootNode *)frontendUiState);
              dVar3 = PersistentSettings_ReadDword(3,0x20);
              pIVar2 = g_FrontendMusicActiveBuffer;
              if ((dVar3 & 2) != 0) {
                RVar24 = Resource_Load((word *)u_sound_music00_sam_00545c4e);
                pSVar4 = (SoundSampleAsset *)RVar24.eax;
                pIVar2 = g_FrontendMusicActiveBuffer;
                if (!RVar24.carry) {
                  SVar17 = (*g_SoundCreateSampleVoiceSet)(pSVar4);
                  arg2 = SVar17.eax;
                  if (SVar17.carry) {
                    Resource_Release(pSVar4);
                    pIVar2 = g_FrontendMusicActiveBuffer;
                  }
                  else {
                    g_FrontendMusicVoiceSet = arg2;
                    Resource_Release(pSVar4);
                    dVar3 = PersistentSettings_ReadDword(0x8000,0x2c);
                    SVar21 = (*g_SoundPlayLooping)(dVar3,dVar3,arg2);
                    pIVar2 = SVar21.eax;
                    if (SVar21.carry) {
                      (*g_SoundReleaseSampleVoiceSet)(arg2);
                      g_FrontendMusicVoiceSet = (DirectSoundVoiceSet *)0x0;
                      pIVar2 = g_FrontendMusicActiveBuffer;
                    }
                  }
                }
              }
              g_FrontendMusicActiveBuffer = pIVar2;
              dVar3 = g_NetworkBackendInstanceCount;
              pdVar6 = g_FrontendTaskAssignmentControlOffsets.primaryAndPadding.offsets;
              if (g_NetworkBackendInstanceCount != 0) {
                pwVar10 = g_NetworkBackendInstanceTable->displayNameUtf16;
                dVar9 = g_NetworkBackendInstanceCount;
                do {
                  pdVar6 = pdVar6 + 1;
                  *pdVar6 = (dword)pwVar10;
                  pwVar10 = pwVar10 + 0x80;
                  dVar9 = dVar9 - 1;
                } while (dVar9 != 0);
                UiPointerList_InitializeMeasuredTextRows
                          (dVar3,(void **)(g_FrontendTaskAssignmentControlOffsets.primaryAndPadding.
                                           offsets + 1),
                           (UiPointerListControl *)(frontendUiState->opaqueGap49E0_4AD3 + 0x90));
              }
              *(code **)(frontendUiState->opaqueGap0000_05DF + 0x468) =
                   FrontendRuntime_DispatchCommandByCodeAndModifierFlagsCf;
              *(code **)(frontendUiState->opaqueGap0000_05DF + 0x46c) =
                   FrontendRuntime_UpdatePointerContextAndSceneViewCf;
              *(code **)(frontendUiState->opaqueGap0000_05DF + 0x470) =
                   FrontendRuntime_UpdatePointerContextAndSceneViewCf;
              *(code **)(frontendUiState->opaqueGap0000_05DF + 0x474) =
                   FrontendRuntimeCallback5C_NoOp;
              *(code **)(frontendUiState->opaqueGap0000_05DF + 0x478) =
                   FrontendRuntimeCallback60_NoOp;
              *(code **)(frontendUiState->opaqueGap0000_05DF + 0x47c) =
                   FrontendRuntimeCallback64_DispatchRecord1350;
              frontendUiState->opaqueGap0000_05DF[0x418] = 0;
              frontendUiState->opaqueGap0000_05DF[0x419] = 0;
              frontendUiState->opaqueGap0000_05DF[0x41a] = 0;
              frontendUiState->opaqueGap0000_05DF[0x41b] = 0;
              *(code **)(frontendUiState->opaqueGap0000_05DF + 0x480) =
                   FrontendRuntimeCallback68_DispatchRefresh1340;
              *(dword **)(frontendUiState->opaqueGap0000_05DF + 0x438) =
                   &g_FrontendStateTickSpinLock;
              *(code **)(frontendUiState->opaqueGap0000_05DF + 0x43c) = Frontend_StateTick;
              RecentTextHistory_SortAndBuildPointerList
                        (5,(RecentTextHistoryPointerList *)
                           (frontendUiState->opaqueGap0000_05DF + 0x350));
              WorldRuntime_SetTerrainLightingConfiguration(0,0,0xffffffff,0,0,0,0,0,worldRuntime);
              WorldRuntime_AttachObjectArray(0x100,g_FrontendWorldObjectRecords,worldRuntime);
              bVar13 = RomRuntime_BuildAllRegistryNodeTrees(worldRuntime);
              pFVar5 = extraout_EAX;
              if (!bVar13) {
                SVar19 = FrontendRomTransition_ActivateRecordByIdCf(initialRomRecordId,worldRuntime)
                ;
                pFVar5 = (FrontendRootResourceSlots5954 *)SVar19.valueOrError;
                if (!SVar19.carry) {
                  pdVar6 = PersistentSettings_GetRegionOrFallback
                                     (0x28,&g_FrontendLocalPlayerNameUtf16,0x60);
                  settingsCopySourceDwordsA = pdVar6;
                  settingsCopyDestDwordsA = (dword *)frontendUiState->opaqueGap4EB4_4F2F;
                  for (iVar8 = 10; iVar8 != 0; iVar8 = iVar8 + -1) {
                    *settingsCopyDestDwordsA = *settingsCopySourceDwordsA;
                    settingsCopySourceDwordsA = settingsCopySourceDwordsA + 1;
                    settingsCopyDestDwordsA = settingsCopyDestDwordsA + 1;
                  }
                  pdVar12 = &g_FrontendLocalPlayerNameUtf16;
                  for (iVar8 = 10; iVar8 != 0; iVar8 = iVar8 + -1) {
                    *pdVar12 = *pdVar6;
                    pdVar6 = pdVar6 + 1;
                    pdVar12 = pdVar12 + 1;
                  }
                  settingsCopySourceDwordsB =
                       PersistentSettings_GetRegionOrFallback
                                 (0x28,&g_FrontendLocalPlayerNameUtf16,0x88);
                  settingsCopyDestDwordsB = (dword *)frontendUiState->opaqueGap50C0_514B;
                  for (iVar8 = 10; iVar8 != 0; iVar8 = iVar8 + -1) {
                    *settingsCopyDestDwordsB = *settingsCopySourceDwordsB;
                    settingsCopySourceDwordsB = settingsCopySourceDwordsB + 1;
                    settingsCopyDestDwordsB = settingsCopyDestDwordsB + 1;
                  }
                  dVar3 = PersistentSettings_ReadDword(4,0x3c);
                  *(dword *)(frontendUiState->opaqueGap50C0_514B + 0x80) = dVar3;
                  UiFrame_FlushInputAndResetPendingTicks();
                  (*g_SpinLockAcquire)(&g_FrontendStateTickSpinLock);
                  WorldMotionSpline_ClearCachedDerivatives();
                  (*g_TimerRegisterPeriodic)(0x100,FrontendRuntime_IncrementActiveTickCounter);
                  if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_NETWORKED_MASK) ==
                      SESSION_NETWORK_ROLE_LOCAL) {
                    FrontendPlayerRuntime_RecordReadyAndUpdateWaitState
                              (g_LocalPlayerRuntimeId,0,0,0);
                  }
                  else {
                    FrontendCommandQueue_EnqueueLocalPlayerCommand(0xd0,0,0,0);
                  }
                  (*g_SpinLockRelease)(&g_FrontendStateTickSpinLock);
                  do {
                    UiNode_InvalidateRoot((UiNodeBase *)frontendUiState);
                    UiFrame_Update(0);
                    UiFrame_Draw();
                    (*g_GraphicsFramebufferPresent)(g_FramebufferAccess);
                    Frontend_StateTick();
                  } while ((g_FrontendRuntimeFlags & 0x10) != 0);
                  (*g_GraphicsCursorSetFrame)(0);
                  UiFrame_FlushInputAndResetPendingTicks();
                  FVar22.carry = false;
                  FVar22.frontendRootOrError = (dword)frontendUiState;
                  return FVar22;
                }
              }
            }
          }
        }
      }
    }
  }
Frontend_Init_ReturnInitializationFailure:
  FVar23.carry = true;
  FVar23.frontendRootOrError = (dword)pFVar5;
  return FVar23;
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
void __thandor_void_preserve_eax_ecx_edx Frontend_StateTick(void)

{
  dword unusedDispatchArg;
  uint uVar1;
  bool bVar2;
  UiRuntimeRecordRingDiscardEaxEdxCf9 UVar3;
  
  bVar2 = (*g_SpinLockTryAcquire)(&g_FrontendStateTickSpinLock);
  uVar1 = g_FrontendNetworkTickCounter;
  unusedDispatchArg = g_FrontendRootNode;
  if (bVar2) {
    return;
  }
                    // WARNING: Switch is manually overridden
  switch(g_FrontendNetworkState) {
  case 0:
    if (g_FrontendTimerCountdownTicks != 0) goto LAB_0054784a;
    g_FrontendNetworkTickCounter = g_FrontendNetworkTickCounter + 1;
    g_FrontendTimerCountdownTicks = 4;
    break;
  case 1:
    if (g_FrontendTimerCountdownTicks == 0) {
      g_FrontendNetworkTickCounter = g_FrontendNetworkTickCounter + 1;
      g_FrontendTimerCountdownTicks = 4;
      if ((uVar1 & 0xf) != 0) {
        UiTransfer_SendPacketType10000Value2931Cf();
      }
      while( true ) {
        UVar3 = UiRuntimeRecordRing_DiscardOldestCf();
        if (UVar3.carryEmpty) break;
        FrontendTransfer_HandleSessionListAndJoinAckPackets
                  ((UiTransferEndpointDescriptor *)UVar3.edxEndpointOrReadIndex,
                   (FrontendTransferPacketUnion *)UVar3.eaxPayloadOrReadIndex,unusedDispatchArg);
      }
      FrontendDebugOverlay_RefreshCountersAndWorldCoordinates();
    }
    goto LAB_0054784a;
  case 2:
    if (g_FrontendTimerCountdownTicks == 0) {
      g_FrontendNetworkTickCounter = g_FrontendNetworkTickCounter + 1;
      g_FrontendTimerCountdownTicks = 4;
      FrontendTransfer_PublishHostSessionAndDispatchQueuedCommands(g_FrontendRootNode);
      while( true ) {
        UVar3 = UiRuntimeRecordRing_DiscardOldestCf();
        if (UVar3.carryEmpty) break;
        FrontendTransfer_HandleLobbyDiscoveryAndPlayerPackets
                  ((UiTransferEndpointDescriptor *)UVar3.edxEndpointOrReadIndex,
                   (FrontendTransferPacketUnion *)UVar3.eaxPayloadOrReadIndex,unusedDispatchArg);
      }
      FrontendDebugOverlay_RefreshCountersAndWorldCoordinates();
    }
    goto LAB_0054784a;
  case 3:
    if (g_FrontendTimerCountdownTicks == 0) {
      g_FrontendNetworkTickCounter = g_FrontendNetworkTickCounter + 1;
      g_FrontendTimerCountdownTicks = 4;
      if ((uVar1 & 0xf) != 0) {
        FrontendTransfer_SendPacket10006();
      }
      while( true ) {
        UVar3 = UiRuntimeRecordRing_DiscardOldestCf();
        if (UVar3.carryEmpty) break;
        FrontendTransfer_HandleHostSessionAndCommandBatchPackets
                  ((UiTransferEndpointDescriptor *)UVar3.edxEndpointOrReadIndex,
                   (FrontendTransferPacketUnion *)UVar3.eaxPayloadOrReadIndex,unusedDispatchArg);
      }
      FrontendDebugOverlay_RefreshCountersAndWorldCoordinates();
    }
    goto LAB_0054784a;
  case 4:
    if (g_FrontendTimerCountdownTicks != 0) goto LAB_0054784a;
    g_FrontendNetworkTickCounter = g_FrontendNetworkTickCounter + 1;
    g_FrontendTimerCountdownTicks = 4;
    while( true ) {
      UVar3 = UiRuntimeRecordRing_DiscardOldestCf();
      if (UVar3.carryEmpty) break;
      FrontendNetwork_HandleHandshakeAndPlayerStatePackets
                ((UiTransferEndpointDescriptor *)UVar3.edxEndpointOrReadIndex,
                 (FrontendTransferPacketUnion *)UVar3.eaxPayloadOrReadIndex,unusedDispatchArg);
    }
    bVar2 = FrontendNetwork_HostTickCommandAndSnapshotTransfer(unusedDispatchArg);
    if (bVar2) {
      g_FrontendTimerCountdownTicks = 1;
      goto LAB_0054784a;
    }
    break;
  case 5:
    bVar2 = UiRuntimeRecordRing_ContainsIdCf(g_FrontendSessionToken);
    if (!bVar2) goto LAB_0054784a;
    g_FrontendNetworkTickCounter = g_FrontendNetworkTickCounter + 1;
    do {
      UVar3 = UiRuntimeRecordRing_DiscardOldestCf();
      if (UVar3.carryEmpty) break;
      bVar2 = FrontendTransfer_HandleGameplayCommandAndRosterPacketsCf
                        ((UiTransferEndpointDescriptor *)UVar3.edxEndpointOrReadIndex,
                         (FrontendTransferPacketUnion *)UVar3.eaxPayloadOrReadIndex,
                         unusedDispatchArg);
    } while (!bVar2);
    bVar2 = FrontendTransfer_ConsumeProcessedFlagFrontendCf();
    if (bVar2) goto LAB_0054784a;
  }
  if ((g_FrontendRuntimeFlags & 0x10) == 0) {
    FrontendDebugOverlay_RefreshCountersAndWorldCoordinates();
  }
LAB_0054784a:
  (*g_SpinLockRelease)(&g_FrontendStateTickSpinLock);
  return;
}


/* Address: 0x00543B70.
   Ownership: ui/frontend/runtime.
   Purpose: Loads gfx\panel\menue.gfx, stores it as the shared frontend-menu texture source, assigns it to verified
   menu controls, and binds the preloaded button sound voice sets across the frontend UI.
*/

void __thandor_void_preserve_ecx_edx
FrontendMenu_BindSharedResources(FrontendRootResourceSlots5954 *frontendUiState)

{
  DirectSoundVoiceSet *pDVar1;
  DirectSoundVoiceSet *pDVar2;
  GraphicsTextureSourceAsset *pGVar3;
  int iVar4;
  GraphicsTextureSourceLoadEaxCf5 GVar5;
  
  GVar5 = (*g_GraphicsTextureSourceLoadPackageAsset)((word *)u_gfx_panel_menue_gfx_00545b78);
  pGVar3 = GVar5.eax;
  if (!GVar5.carry) {
    g_FrontendMenuTextureSource = pGVar3;
    frontendUiState->menuTextureSource_485C = pGVar3;
    frontendUiState->menuTextureSource_4F30 = pGVar3;
    frontendUiState->menuTextureSource_53D8 = pGVar3;
    frontendUiState->menuTextureSource_5720 = pGVar3;
    frontendUiState->menuTextureSource_2670 = pGVar3;
    frontendUiState->menuTextureSource_2D0C = pGVar3;
    frontendUiState->menuTextureSource_3738 = pGVar3;
    frontendUiState->menuTextureSource_3E64 = pGVar3;
    frontendUiState->menuTextureSource_24F8 = pGVar3;
    frontendUiState->menuTextureSource_1C8C = pGVar3;
    frontendUiState->menuTextureSource_0AE4 = pGVar3;
    frontendUiState->menuTextureSource_05E0 = pGVar3;
    pDVar1 = g_UiButtonSoundVoiceSets7[3];
    frontendUiState->buttonVoiceSet3_0644 = g_UiButtonSoundVoiceSets7[3];
    frontendUiState->buttonVoiceSet3_0764 = pDVar1;
    frontendUiState->buttonVoiceSet3_06A4 = pDVar1;
    frontendUiState->buttonVoiceSet3_0704 = pDVar1;
    frontendUiState->buttonVoiceSet3_0B48 = pDVar1;
    frontendUiState->buttonVoiceSet3_0BA8 = pDVar1;
    frontendUiState->buttonVoiceSet3_0C68 = pDVar1;
    frontendUiState->buttonVoiceSet3_1CF0 = pDVar1;
    frontendUiState->buttonVoiceSet3_1D50 = pDVar1;
    frontendUiState->buttonVoiceSet3_1DB0 = pDVar1;
    frontendUiState->buttonVoiceSet3_1E10 = pDVar1;
    frontendUiState->buttonVoiceSet3_1E70 = pDVar1;
    frontendUiState->buttonVoiceSet3_255C = pDVar1;
    frontendUiState->buttonVoiceSet3_25BC = pDVar1;
    frontendUiState->buttonVoiceSet3_26D4 = pDVar1;
    frontendUiState->buttonVoiceSet3_2790 = pDVar1;
    frontendUiState->buttonVoiceSet3_27F0 = pDVar1;
    frontendUiState->buttonVoiceSet3_2850 = pDVar1;
    frontendUiState->buttonVoiceSet3_2D70 = pDVar1;
    frontendUiState->buttonVoiceSet3_2DD0 = pDVar1;
    frontendUiState->buttonVoiceSet3_379C = pDVar1;
    frontendUiState->buttonVoiceSet3_3EC8 = pDVar1;
    frontendUiState->buttonVoiceSet3_491C = pDVar1;
    frontendUiState->buttonVoiceSet3_497C = pDVar1;
    frontendUiState->buttonVoiceSet3_49DC = pDVar1;
    frontendUiState->buttonVoiceSet3_4FF0 = pDVar1;
    frontendUiState->buttonVoiceSet3_5050 = pDVar1;
    frontendUiState->buttonVoiceSet3_5498 = pDVar1;
    frontendUiState->buttonVoiceSet3_54F8 = pDVar1;
    frontendUiState->buttonVoiceSet3_5558 = pDVar1;
    frontendUiState->buttonVoiceSet3_57E0 = pDVar1;
    pDVar1 = g_UiButtonSoundVoiceSets7[4];
    frontendUiState->buttonVoiceSet4_2AE0 = g_UiButtonSoundVoiceSets7[4];
    frontendUiState->buttonVoiceSet4_2B40 = pDVar1;
    frontendUiState->buttonVoiceSet4_2BF4 = pDVar1;
    frontendUiState->buttonVoiceSet4_2C54 = pDVar1;
    frontendUiState->buttonVoiceSet4_2CB4 = pDVar1;
    frontendUiState->buttonVoiceSet4_2EE0 = pDVar1;
    frontendUiState->buttonVoiceSet4_2F48 = pDVar1;
    frontendUiState->buttonVoiceSet4_2FB0 = pDVar1;
    frontendUiState->buttonVoiceSet4_3018 = pDVar1;
    frontendUiState->buttonVoiceSet4_3080 = pDVar1;
    frontendUiState->buttonVoiceSet4_313C = pDVar1;
    frontendUiState->buttonVoiceSet4_31A4 = pDVar1;
    frontendUiState->buttonVoiceSet4_320C = pDVar1;
    frontendUiState->buttonVoiceSet4_3274 = pDVar1;
    frontendUiState->buttonVoiceSet4_32DC = pDVar1;
    frontendUiState->buttonVoiceSet4_3344 = pDVar1;
    frontendUiState->buttonVoiceSet4_33AC = pDVar1;
    frontendUiState->buttonVoiceSet4_3414 = pDVar1;
    frontendUiState->buttonVoiceSet4_347C = pDVar1;
    frontendUiState->buttonVoiceSet4_34E4 = pDVar1;
    frontendUiState->buttonVoiceSet4_35A0 = pDVar1;
    frontendUiState->buttonVoiceSet4_3608 = pDVar1;
    frontendUiState->buttonVoiceSet4_3670 = pDVar1;
    frontendUiState->buttonVoiceSet4_36D8 = pDVar1;
    frontendUiState->buttonVoiceSet4_3858 = pDVar1;
    frontendUiState->buttonVoiceSet4_390C = pDVar1;
    frontendUiState->buttonVoiceSet4_3974 = pDVar1;
    frontendUiState->buttonVoiceSet4_39DC = pDVar1;
    frontendUiState->buttonVoiceSet4_3A44 = pDVar1;
    frontendUiState->buttonVoiceSet4_3AAC = pDVar1;
    frontendUiState->buttonVoiceSet4_3B14 = pDVar1;
    frontendUiState->buttonVoiceSet4_3D4C = pDVar1;
    frontendUiState->buttonVoiceSet4_3DAC = pDVar1;
    frontendUiState->buttonVoiceSet4_3E0C = pDVar1;
    frontendUiState->buttonVoiceSet4_3F84 = pDVar1;
    frontendUiState->buttonVoiceSet4_3FE4 = pDVar1;
    frontendUiState->buttonVoiceSet4_4044 = pDVar1;
    frontendUiState->buttonVoiceSet4_28B0 = pDVar1;
    iVar4 = 7;
    do {
      *(DirectSoundVoiceSet **)
       (frontendUiState->opaqueGap0000_05DF +
       g_FrontendTaskAssignmentControlOffsets.factionControls.offsets[iVar4] + 0x5c) = pDVar1;
      *(DirectSoundVoiceSet **)
       (frontendUiState->opaqueGap0000_05DF +
       g_FrontendTaskAssignmentControlOffsets.playerControls.offsets[iVar4] + 0x5c) = pDVar1;
      *(DirectSoundVoiceSet **)
       (frontendUiState->opaqueGap0000_05DF +
       g_FrontendTaskAssignmentControlOffsets.selectionRows.offsets[iVar4] + 0x5c) = pDVar1;
      pDVar2 = g_UiButtonSoundVoiceSets7[5];
      iVar4 = iVar4 + -1;
    } while (iVar4 != 0);
    frontendUiState->buttonVoiceSet5_3C98 = g_UiButtonSoundVoiceSets7[5];
    frontendUiState->buttonVoiceSet5_41C0 = pDVar2;
    frontendUiState->buttonVoiceSet5_433C = pDVar2;
    frontendUiState->buttonVoiceSet5_44B8 = pDVar2;
    frontendUiState->buttonVoiceSet5_4634 = pDVar2;
    frontendUiState->buttonVoiceSet5_514C = pDVar2;
    frontendUiState->buttonVoiceSet5_5210 = pDVar2;
    frontendUiState->buttonVoiceSet5_0A8C = pDVar2;
    pDVar1 = g_UiButtonSoundVoiceSets7[6];
    frontendUiState->buttonVoiceSet6_2024 = g_UiButtonSoundVoiceSets7[6];
    frontendUiState->buttonVoiceSet6_21EC = pDVar1;
    frontendUiState->buttonVoiceSet6_23CC = pDVar1;
    frontendUiState->buttonVoiceSet6_4AD4 = pDVar1;
    frontendUiState->buttonVoiceSet6_4BD0 = pDVar1;
    frontendUiState->buttonVoiceSet6_5654 = pDVar1;
    frontendUiState->buttonVoiceSet6_4DC4 = pDVar1;
    frontendUiState->buttonVoiceSet6_4EB0 = pDVar1;
    frontendUiState->buttonVoiceSet6_50BC = pDVar1;
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

void __thandor_void_preserve_eax_ecx_edx
FrontendUiAction2044_IndexedSelectionHelper
          (FrontendIndexedSelectionArgument argument1,dword argument2,dword argument3,
          FrontendFactionAssignmentIndex selectionIndex)

{
  int *piVar1;
  LevelPlayerSlotByteOffset32 LVar2;
  FrontendLoadedLevelRuntimeImage370 *pFVar3;
  uint nextSelectionTextId;
  dword playerRecordsRemaining;
  FrontendPlayerRuntimeRecord *playerRecordCursor;
  int selectionControlAddress;
  int factionAssetRecordAddress;
  int selectionTextCycleLength;
  int *selectionCycleCounterField;
  
  pFVar3 = g_FrontendLoadedLevelAsset;
  selectionTextCycleLength = 7;
  playerRecordsRemaining = g_FrontendPlayerRuntimeBlockCount;
  playerRecordCursor = g_FrontendPlayerRuntimeBlocks;
  do {
    if (argument1 == playerRecordCursor->playerRuntimeId) {
      if ((playerRecordCursor->runtimeState64 & 1) != 0) {
        selectionTextCycleLength = 8;
      }
      LVar2 = g_InGameLevelRuntimeGlobalBlock.playerSlotByteOffsets[selectionIndex];
      selectionControlAddress =
           g_FrontendRootNode +
           g_FrontendTaskAssignmentControlOffsets.factionControls.offsets[selectionIndex + 1];
      nextSelectionTextId = *(int *)(selectionControlAddress + 0x54) + 1;
      selectionCycleCounterField =
           (int *)((int)&g_FrontendLoadedLevelAsset->playerSlots[0].aiClassOrMode + LVar2);
      *selectionCycleCounterField = *selectionCycleCounterField + 1;
      if (selectionTextCycleLength + 0x2174U <= nextSelectionTextId) {
        nextSelectionTextId = 0x2174;
        piVar1 = (int *)((int)&pFVar3->playerSlots[0].aiClassOrMode + LVar2);
        *piVar1 = *piVar1 - selectionTextCycleLength;
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
void __thandor_void_preserve_eax_ecx_edx
FrontendUiAction2045_IndexedSelectionHelper
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
void __thandor_void_preserve_eax_ecx_edx
FrontendUiAction2046_IndexedSelectionHelper
          (FrontendIndexedSelectionArgument argument1,dword argument2,dword argument3,
          FrontendFactionAssignmentIndex selectionIndex)

{
  SessionNetworkRoleFlags SVar1;
  SessionNetworkRoleFlags SVar2;
  dword selectionIndex_00;
  SessionNetworkRoleFlags generationCursor;
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
      generationCursor = SESSION_NETWORK_ROLE_LOCAL;
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
void __thandor_void_preserve_eax_ecx_edx
FrontendDebugOverlay_RefreshCountersAndWorldCoordinates(void)

{
  dword value;
  WideNumberDenominator32 denominator;
  WorldRuntimeContext *world;
  WorldVector0EaxEcxEdx12 WVar1;
  WorldVector1EaxEcxEdx12 WVar2;
  
  denominator = g_RenderedFrameCountSinceDebugRefresh;
  g_DebugOverlayCounterRefreshCountdown = g_DebugOverlayCounterRefreshCountdown - 1;
  if (g_DebugOverlayCounterRefreshCountdown == 0) {
    g_DebugOverlayCounterRefreshCountdown = 0x14;
    WideNumber_FormatUtf16
              (WIDE_FORMAT_WRITE_TERMINATOR,0,10,1,g_RenderedFrameCountSinceDebugRefresh,
               g_FrontendDebugOverlayTextSlot00Utf16);
    if (denominator == 0) {
      denominator = 1;
    }
    WideNumber_FormatUtf16
              (WIDE_FORMAT_WRITE_TERMINATOR|WIDE_FORMAT_FIXED_FRACTION_WIDTH,2,10,denominator,
               g_PrimitiveDrawCallCount,g_FrontendDebugOverlayTextSlot01Utf16);
    WideNumber_FormatUtf16
              (WIDE_FORMAT_WRITE_TERMINATOR|WIDE_FORMAT_FIXED_FRACTION_WIDTH,2,10,denominator,
               g_TextureBindStateChangeCount,g_FrontendDebugOverlayTextSlot02Utf16);
    WideNumber_FormatUtf16
              (WIDE_FORMAT_WRITE_TERMINATOR|WIDE_FORMAT_FIXED_FRACTION_WIDTH,2,10,denominator,
               g_TextureDeviceReloadCount,g_FrontendDebugOverlayTextSlot03Utf16);
    g_RenderedFrameCountSinceDebugRefresh = 0;
    g_PrimitiveDrawCallCount = 0;
    g_TextureBindStateChangeCount = 0;
    g_TextureDeviceReloadCount = 0;
  }
  world = (WorldRuntimeContext *)(g_FrontendRootNode + 0x368);
  WVar1 = WorldRuntime_GetVector0Regs(world);
  WideNumber_FormatUtf16
            (WIDE_FORMAT_GROUP_THOUSANDS|WIDE_FORMAT_WRITE_TERMINATOR|WIDE_FORMAT_SIGNED_VALUE,0,10,
             1,WVar1.xQ12,g_FrontendDebugOverlayTextSlot04Utf16);
  WideNumber_FormatUtf16
            (WIDE_FORMAT_GROUP_THOUSANDS|WIDE_FORMAT_WRITE_TERMINATOR|WIDE_FORMAT_SIGNED_VALUE,0,10,
             1,WVar1.yQ12,g_FrontendDebugOverlayTextSlot05Utf16);
  WideNumber_FormatUtf16
            (WIDE_FORMAT_GROUP_THOUSANDS|WIDE_FORMAT_WRITE_TERMINATOR|WIDE_FORMAT_SIGNED_VALUE,0,10,
             1,WVar1.zQ12,g_FrontendDebugOverlayTextSlot06Utf16);
  WVar2 = WorldRuntime_GetVector1Regs(world);
  WideNumber_FormatUtf16
            (WIDE_FORMAT_GROUP_THOUSANDS|WIDE_FORMAT_WRITE_TERMINATOR|WIDE_FORMAT_SIGNED_VALUE,0,10,
             1,WVar2.magnitudeQ12,g_FrontendDebugOverlayTextSlot07Utf16);
  WideNumber_FormatUtf16
            (WIDE_FORMAT_GROUP_THOUSANDS|WIDE_FORMAT_WRITE_TERMINATOR|WIDE_FORMAT_SIGNED_VALUE,0,10,
             1,WVar2.headingAngle,g_FrontendDebugOverlayTextSlot08Utf16);
  WideNumber_FormatUtf16
            (WIDE_FORMAT_GROUP_THOUSANDS|WIDE_FORMAT_WRITE_TERMINATOR|WIDE_FORMAT_SIGNED_VALUE,0,10,
             1,WVar2.pitchAngle,g_FrontendDebugOverlayTextSlot09Utf16);
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
void __thandor_void_preserve_eax_ecx_edx FrontendRuntime_ShutdownAndReleaseResourcesRegs(void)

{
  UiRootNode *root;
  int voiceSetsRemaining;
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
  (*g_GraphicsTextureSetReleasePackageCf)(g_FrontendCentralTextureSet);
  (*g_GraphicsPaletteAssetLifecycleCallbacks3.releasePackage)(g_FrontendCentralPaletteAsset);
  (*g_GraphicsTextureSourceLifecycleCallbacks3.releasePackage)(g_FrontendMenuTextureSource);
  g_FrontendCentralTextureSet = (GraphicsTextureSet *)0x0;
  g_FrontendCentralPaletteAsset = (GraphicsPaletteAsset *)0x0;
  g_FrontendMenuTextureSource = (GraphicsTextureSourceAsset *)0x0;
  voiceSetCursor = &g_FrontendMenuSoundVoiceSetTable100;
  voiceSetsRemaining = 100;
  do {
    if ((DirectSoundVoiceSet *)*voiceSetCursor != (DirectSoundVoiceSet *)0x0) {
      (*g_SoundReleaseSampleVoiceSet)((DirectSoundVoiceSet *)*voiceSetCursor);
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
  return;
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
  ModelRuntimeNode *pMVar2;
  StatusValueEaxCf5 SVar3;
  
  pMVar2 = (ModelRuntimeNode *)0x0;
  modelNode = context->candidateModelListHead;
  dVar1 = 0x7fffffff;
  do {
    if (modelNode == (ModelRuntimeNode *)0x0) {
      return CONCAT44(pMVar2,dVar1);
    }
    if ((((modelNode->runtimeFlags & 2) != 0) && (modelNode->ownerClassId == MODEL_RUNTIME_CLASS_00)
        ) && (((context->contextFlags &
               FRONTEND_MODEL_POINTER_CONTEXT_ALLOW_MODEL_WITHOUT_RUNTIME_FLAG_20) != 0 ||
              ((modelNode->runtimeFlags & 0x20) != 0)))) {
      SVar3 = ModelRuntimeNode_HitTestProjectedBoundsAndChildrenCf
                        (pointerY,pointerX,modelNode,context);
      if (!SVar3.carry) {
        if ((context->contextFlags & FRONTEND_MODEL_POINTER_CONTEXT_COMPARE_HITS_BY_METRIC_ONLY) ==
            0) {
          if (pMVar2 != (ModelRuntimeNode *)0x0) {
            if ((int)(&g_RuntimeModelClassPriorityByModelClassId.modelClass00Priority)
                     [*(int *)((((modelNode->runtimePayload).modelRuntime)->definitionOrSavedId).
                               savedIdOrOffset + 0x4c)] <
                (int)(&g_RuntimeModelClassPriorityByModelClassId.modelClass00Priority)
                     [*(int *)((((pMVar2->runtimePayload).modelRuntime)->definitionOrSavedId).
                               savedIdOrOffset + 0x4c)])
            goto FrontendModelHitSelection_AdvanceCandidate;
            if ((int)(&g_RuntimeModelClassPriorityByModelClassId.modelClass00Priority)
                     [*(int *)((((modelNode->runtimePayload).modelRuntime)->definitionOrSavedId).
                               savedIdOrOffset + 0x4c)] <=
                (int)(&g_RuntimeModelClassPriorityByModelClassId.modelClass00Priority)
                     [*(int *)((((pMVar2->runtimePayload).modelRuntime)->definitionOrSavedId).
                               savedIdOrOffset + 0x4c)])
            goto FrontendModelHitSelection_CompareHitMetric;
          }
        }
        else {
FrontendModelHitSelection_CompareHitMetric:
          if ((int)dVar1 <= (int)SVar3.valueOrError)
          goto FrontendModelHitSelection_AdvanceCandidate;
        }
        dVar1 = SVar3.valueOrError;
        pMVar2 = modelNode;
      }
    }
FrontendModelHitSelection_AdvanceCandidate:
    modelNode = (ModelRuntimeNode *)(modelNode->common).nextNode;
  } while( true );
}


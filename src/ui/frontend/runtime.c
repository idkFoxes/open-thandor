/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/src/ui/frontend/runtime.c
 * Reverse engineering by idkFoxes 2026
 */

#include <thandor/ui/frontend/runtime.h>
#include <thandor/thandor.h>
#include <thandor/platform/bootstrap/image.h>

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
FrontendMainLoopResult __thandor_eax_cf_preserve_ecx_edx
Frontend_MainLoop(RomRecordId frontendEntryRecordId)

{
  FrontendRoleStateFlags *roleStateFlagsPtr;
  AssetAllocationSizeBytes fieldGridAllocationSize;
  FrontendSnapshotTransferFlags receivedTransferFlags;
  SessionNetworkRoleFlags pendingBlockCountOrRoleMask;
  ScenarioCatalogHeader *source;
  uint32_t statusOrByteCount;
  FieldGridAsset *sourceGrid;
  RomRecordId initialRomRecordId;
  int countOrSelectedId;
  FrontendPlayerRuntimeBlockCount remainingPlayerBlocks;
  int remainingLevelRecords;
  uint32_t remainingDwords;
  SessionNetworkRoleFlags remainingBlockCount;
  uint8_t *encodeCursorOrSize;
  PckOutputCapacityBytes destinationCapacityBytes;
  FrontendPlayerRuntimeRecord *playerBlock;
  void *levelRecordCursor;
  void *campaignRecordCursor;
  uint8_t *transferSourceBytes;
  FrontendLoadedLevelRuntimeImage370 *loadedLevelAsset;
  FrontendPlayerRuntimeRecord *roleScanBlock;
  FrontendSnapshotTransferFlags *receivedFlagsCursor;
  uint32_t *transferDwordCursor;
  FrontendInitResult initResult;
  FrontendMainLoopResult exitResult;
  FrontendMainLoopResult failureResult;
  SessionRunResult sessionRunResult;
  PackageLoadResult packageLoadResult;
  FatalErrorCheckResult checkedResult;
  PckCodecResult encodeResult;
  ArenaAllocResult allocResult;
  MailboxReceiveResult receivedBuffer;
  CommandLineOptionResult commandLineOption;
  FieldGridAsset *savedFieldGrid;
  
  g_FrontendNetworkState = 0;
  initResult = Frontend_Init(frontendEntryRecordId);
  statusOrByteCount = initResult.frontendRootOrError;
  if (!initResult.failed) {
    commandLineOption = (*g_CommandLineFindOption)(5,s_SPIELER__SPIEL__NETZWERK__HOST_00545e72 + 0x1a);
    if (commandLineOption.notFound) {
      commandLineOption = (*g_CommandLineFindOption)(8,s_NAME__CLIENT__KARTE___00545e91 + 6);
      if (commandLineOption.notFound) {
        commandLineOption = (*g_CommandLineFindOption)(7,s_NAME__CLIENT__KARTE___00545e91 + 0xe);
        if (!commandLineOption.notFound) {
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
              exitResult.failed = false;
              /* The asm returns with CLC and EAX left over from UiFrame_ProcessAndPresent (the shutdown helper
                 preserves EAX); the only caller hands it to the fatal-error dispatcher, which ignores it with CF
                 clear. */
              exitResult.errorOrValue = 0;
              return exitResult;
            }
          }
          UiFrame_FlushInputAndResetPendingTicks();
          g_FrontendPendingPageActionDepth = g_FrontendPendingPageActionDepth + 1;
          if (g_FrontendPendingPageAction != 2) break;
          FrontendNetworkSetupPage_InitializeBackendMode((FrontendUiImage *)g_FrontendRootNode);
          g_FrontendPendingPageAction = 0;
        }
        if (g_FrontendPendingPageAction != 3) break;
        FrontendGameplaySettingsPage_InitializeFromPersistentSettings
                  ((UiRootNode *)&((union FrontendNetworkSettingsControlView250 *)(uintptr_t)g_FrontendRootNode)->commonState);
        g_FrontendPendingPageAction = 0;
      }
      if (g_FrontendPendingPageAction == 5) {
        playerBlock = g_FrontendPlayerRuntimeBlocks;
        remainingBlockCount = g_FrontendPlayerRuntimeBlockCount;
        pendingBlockCountOrRoleMask = g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_NETWORKED_MASK;
        while (remainingPlayerBlocks = g_FrontendPlayerRuntimeBlockCount, roleScanBlock = g_FrontendPlayerRuntimeBlocks,
              pendingBlockCountOrRoleMask != SESSION_NETWORK_ROLE_LOCAL) {
          if ((playerBlock->snapshotTransferFlags & FRONTEND_SNAPSHOT_HOST_PUBLICATION_READY) == 0) {
            if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_CLIENT) !=
                SESSION_NETWORK_ROLE_LOCAL) {
              receivedBuffer = UiTransferMailbox_GetReceivedBufferCf();
              if (!receivedBuffer.unavailable) {
                PckCodec_DecodeHuffmanRle
                          (*(PckDecodedByteCount *)receivedBuffer.buffer,g_PackageScratchBuffer,receivedBuffer.byteCount - 4,
                           (uint8_t *)((PckDecodedByteCount *)receivedBuffer.buffer + 1));
                remainingPlayerBlocks = g_FrontendPlayerRuntimeBlockCount;
                receivedFlagsCursor = (FrontendSnapshotTransferFlags *)g_PackageScratchBuffer;
                playerBlock = g_FrontendPlayerRuntimeBlocks;
                do {
                  receivedTransferFlags = *receivedFlagsCursor;
                  playerBlock->snapshotTransferFlags = playerBlock->snapshotTransferFlags | receivedTransferFlags;
                  receivedFlagsCursor = receivedFlagsCursor + 1;
                  if ((receivedTransferFlags & FRONTEND_SNAPSHOT_PAYLOAD_COMPLETE) != 0) {
                    encodeCursorOrSize = playerBlock->snapshotPayloadB0_13AF;
                    for (countOrSelectedId = 0x4c0; countOrSelectedId != 0; countOrSelectedId = countOrSelectedId + -1) {
                      *(FrontendSnapshotTransferFlags *)encodeCursorOrSize = *receivedFlagsCursor;
                      receivedFlagsCursor = receivedFlagsCursor + 1;
                      encodeCursorOrSize = encodeCursorOrSize + 4;
                    }
                  }
                  playerBlock = playerBlock + 1;
                  remainingPlayerBlocks = remainingPlayerBlocks - 1;
                } while (remainingPlayerBlocks != 0);
                FrontendCommandQueue_EnqueueLocalPlayerCommand(0x1710,0,0,0);
                UiTransferMailbox_ClearReceivedState();
              }
            }
            goto FrontendMainLoop_ProcessFrameAndPendingPageAction;
          }
          playerBlock = playerBlock + 1;
          remainingBlockCount = remainingBlockCount - SESSION_NETWORK_ROLE_CLIENT;
          pendingBlockCountOrRoleMask = remainingBlockCount;
        }
        do {
          roleStateFlagsPtr = &(roleScanBlock->factionAssignment).roleStateFlags;
          *roleStateFlagsPtr = *roleStateFlagsPtr & 1;
          playerBlock = g_FrontendPlayerRuntimeBlocks;
          if (*roleStateFlagsPtr == 0) {
            FrontendScenarioTransfer_ProcessReceivedAsset();
            if (((playerBlock->factionAssignment).roleStateFlags & 1) == 0) {
              roleStateFlagsPtr = &(playerBlock->factionAssignment).roleStateFlags;
              *roleStateFlagsPtr = *roleStateFlagsPtr | 1;
              ScenarioCatalog_Rebuild();
              statusOrByteCount = g_ScenarioCatalogUsedBytes;
              source = g_ScenarioCatalog;
              if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_CLIENT) ==
                  SESSION_NETWORK_ROLE_LOCAL) {
                if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_HOST) !=
                    SESSION_NETWORK_ROLE_LOCAL) {
                  encodeCursorOrSize = (uint8_t *)((int)&g_ScenarioCatalog->campaignRecordsOffset +
                                    g_ScenarioCatalogUsedBytes);
                  destinationCapacityBytes = 0x2fffc - g_ScenarioCatalogUsedBytes;
                  *(uint32_t *)(encodeCursorOrSize + -4) = g_ScenarioCatalogUsedBytes;
                  encodeResult = PckCodec_EncodeHuffmanRle
                                     (destinationCapacityBytes,encodeCursorOrSize,statusOrByteCount,(uint8_t *)source);
                  checkedResult = (*g_FatalErrorPrimaryDispatchCf)(encodeResult.byteCountOrError,encodeResult.failed);
                  UiTransferMailbox_SetOutgoingBuffer(checkedResult.valueOrError + 4,encodeCursorOrSize + -4);
                }
              }
              else {
                UiTransferMailbox_MarkUnavailable();
                g_FrontendScenarioTransferState = 1;
              }
            }
            goto FrontendMainLoop_ProcessFrameAndPendingPageAction;
          }
          roleScanBlock = roleScanBlock + 1;
          remainingPlayerBlocks = remainingPlayerBlocks - 1;
        } while (remainingPlayerBlocks != 0);
        if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_HOST) != SESSION_NETWORK_ROLE_LOCAL) {
          UiTransferMailbox_SetOutgoingBuffer(0,(void *)0x0);
        }
        FrontendScenarioSelectionPage_InitializeAndApplyMapOption
                  ((FrontendScenarioSelectionPageView26C4 *)&((union FrontendNetworkSettingsControlView250 *)(uintptr_t)g_FrontendRootNode)->commonState);
        g_FrontendPendingPageAction = 0;
        goto FrontendMainLoop_ProcessFrameAndPendingPageAction;
      }
      if (g_FrontendPendingPageAction == 7) {
        FrontendScenarioTransfer_ProcessReceivedAsset();
        remainingPlayerBlocks = g_FrontendPlayerRuntimeBlockCount;
        playerBlock = g_FrontendPlayerRuntimeBlocks;
        do {
          if (((playerBlock->factionAssignment).roleStateFlags & 2) == 0)
          goto FrontendMainLoop_ProcessFrameAndPendingPageAction;
          playerBlock = playerBlock + 1;
          remainingPlayerBlocks = remainingPlayerBlocks - 1;
        } while (remainingPlayerBlocks != 0);
        if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_HOST) != SESSION_NETWORK_ROLE_LOCAL) {
          (*g_MemoryApi.free)(g_UiTransferMailbox.outgoingAllocation);
          UiTransferMailbox_SetOutgoingBuffer(0,(void *)0x0);
        }
        FrontendTaskAssignmentPage_Initialize
                  ((FrontendTaskAssignmentPageInitView26C4 *)&((union FrontendNetworkSettingsControlView250 *)(uintptr_t)g_FrontendRootNode)->commonState);
        g_FrontendPendingPageAction = 0;
      }
      else if (g_FrontendPendingPageAction == 8) {
        FrontendScenarioTransfer_ProcessReceivedAsset();
        remainingPlayerBlocks = g_FrontendPlayerRuntimeBlockCount;
        playerBlock = g_FrontendPlayerRuntimeBlocks;
        do {
          if (((playerBlock->factionAssignment).roleStateFlags & 0xc) == 0)
          goto FrontendMainLoop_ProcessFrameAndPendingPageAction;
          playerBlock = playerBlock + 1;
          remainingPlayerBlocks = remainingPlayerBlocks - 1;
        } while (remainingPlayerBlocks != 0);
        if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_HOST) != SESSION_NETWORK_ROLE_LOCAL) {
          (*g_MemoryApi.free)(g_UiTransferMailbox.outgoingAllocation);
          UiTransferMailbox_SetOutgoingBuffer(0,(void *)0x0);
        }
        FrontendMissionBriefingPage_Initialize((UiRootNode *)&((union FrontendNetworkSettingsControlView250 *)(uintptr_t)g_FrontendRootNode)->commonState);
        g_FrontendPendingPageAction = 0;
      }
      else if (g_FrontendPendingPageAction == 9) {
        CreditsScreen_Open((FrontendCreditsUiStateView *)&((union FrontendNetworkSettingsControlView250 *)(uintptr_t)g_FrontendRootNode)->commonState);
        g_FrontendPendingPageAction = 0;
      }
      else {
        if (g_FrontendPendingPageAction != 4) {
          FrontendRuntime_ShutdownAndReleaseResourcesRegs();
          if (g_FrontendPendingPageAction == 1) {
            PersistentSettings_Flush();
            sessionRunResult = InGameRuntime_RunSessionUntilExit
                               ((LevelAssetRuntimeImagePrefix370 *)g_FrontendLoadedLevelAsset,0,
                                (uint16_t *)&g_FrontendScenarioPathScratchUtf16);
            (*g_FatalErrorPrimaryDispatchCf)(sessionRunResult.exitCodeOrError,sessionRunResult.failed);
            PersistentSettings_Flush();
            UiFrame_FlushInputAndResetPendingTicks();
            g_FrontendScenarioInitializationCount = 0;
            remainingPlayerBlocks = g_FrontendPlayerRuntimeBlockCount;
            playerBlock = g_FrontendPlayerRuntimeBlocks;
            do {
              (playerBlock->factionAssignment).roleStateFlags = 0;
              playerBlock = playerBlock + 1;
              remainingPlayerBlocks = remainingPlayerBlocks - 1;
              campaignRecordCursor = g_FrontendLoadedCampaignAsset;
            } while (remainingPlayerBlocks != 0);
FrontendMainLoop_AdvanceCampaignAfterSession:
            g_FrontendLoadedCampaignAsset = campaignRecordCursor;
            if (campaignRecordCursor != (void *)0x0) {
              countOrSelectedId = *(int *)((int)campaignRecordCursor + 0xb8);
              levelRecordCursor = campaignRecordCursor;
              do {
                if (*(int *)((int)campaignRecordCursor + 0xc4) == *(int *)((int)levelRecordCursor + 0x300)) {
                  /* Current level found: follow the successor chosen by the end movie selection. */
                  countOrSelectedId = *(int *)((int)levelRecordCursor + (int)g_EndMovieSelectionIndex * 4 + 0x200);
                  if (-1 < countOrSelectedId) {
                    remainingLevelRecords = *(int *)((int)campaignRecordCursor + 0xb8);
                    *(int *)((int)campaignRecordCursor + 0xc4) = countOrSelectedId;
                    while( true ) {
                      if (countOrSelectedId == *(int *)((int)campaignRecordCursor + 0x300)) {
                        WidePath_CombineDirectoryAndLeaf
                                  ((uint16_t *)&g_FrontendScenarioPathScratchUtf16,
                                   (uint16_t *)((int)campaignRecordCursor + 0x30c),(uint16_t *)u_level_0050daac);
                        WidePath_SetExtensionCode(0x76656c,(uint16_t *)&g_FrontendScenarioPathScratchUtf16);
                        goto FrontendScenario_InitializeSelectedLevel;
                      }
                      campaignRecordCursor = (void *)((int)campaignRecordCursor + 0x180);
                      remainingLevelRecords = remainingLevelRecords + -1;
                      if (remainingLevelRecords == 0) break;
                    }
                    /* Successor level missing: the campaign is finished. */
                    Resource_Release(g_FrontendLoadedCampaignAsset);
                    g_FrontendLoadedCampaignAsset = (void *)0x0;
                    g_FrontendScenarioInitializationCount = 0;
                  }
                  break;
                }
                levelRecordCursor = (void *)((int)levelRecordCursor + 0x180);
                countOrSelectedId = countOrSelectedId + -1;
              } while (countOrSelectedId != 0);
            }
            goto FrontendScenario_UseResolvedPathOrFallbackPage;
          }
          if (g_FrontendPendingPageAction == 6) {
            sessionRunResult = InGameRuntime_RunSessionUntilExit
                               ((LevelAssetRuntimeImagePrefix370 *)g_FrontendLoadedLevelAsset,1,
                                (uint16_t *)&g_FrontendScenarioPathScratchUtf16);
            (*g_FatalErrorPrimaryDispatchCf)(sessionRunResult.exitCodeOrError,sessionRunResult.failed);
            PersistentSettings_Flush();
            UiFrame_FlushInputAndResetPendingTicks();
            g_FrontendScenarioInitializationCount = 0;
            remainingPlayerBlocks = g_FrontendPlayerRuntimeBlockCount;
            playerBlock = g_FrontendPlayerRuntimeBlocks;
            do {
              (playerBlock->factionAssignment).roleStateFlags = 0;
              playerBlock = playerBlock + 1;
              remainingPlayerBlocks = remainingPlayerBlocks - 1;
              campaignRecordCursor = g_FrontendLoadedCampaignAsset;
            } while (remainingPlayerBlocks != 0);
            goto FrontendMainLoop_AdvanceCampaignAfterSession;
          }
          countOrSelectedId = 0;
          initialRomRecordId = frontendEntryRecordId;
          goto FrontendMainLoop_InitializeRequestedPage;
        }
        FrontendSession_ShowPage9WithCompactLayout((FrontendUiImage *)g_FrontendRootNode);
        g_FrontendPendingPageAction = 0;
      }
    } while( true );
  }
FrontendMainLoop_ShutdownAndReturn:
  FrontendRuntime_ShutdownAndReleaseResourcesRegs();
  failureResult.failed = true;
  failureResult.errorOrValue = statusOrByteCount;
  return failureResult;
FrontendScenario_UseResolvedPathOrFallbackPage:
  if (g_FrontendScenarioPathScratchUtf16 == 0) {
    initialRomRecordId = 0xc;
    countOrSelectedId = 5;
    goto FrontendMainLoop_InitializeRequestedPage;
  }
FrontendScenario_InitializeSelectedLevel:
  {
    initResult = Frontend_Init(10);
    statusOrByteCount = initResult.frontendRootOrError;
    if (initResult.failed) goto FrontendMainLoop_ShutdownAndReturn;
    g_FrontendScenarioInitializationCount = g_FrontendScenarioInitializationCount + 1;
    g_FrontendPendingPageAction = 8;
    roleStateFlagsPtr = &(g_FrontendPlayerRuntimeBlocks->factionAssignment).roleStateFlags;
    *roleStateFlagsPtr = *roleStateFlagsPtr | 4;
    if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_CLIENT) == SESSION_NETWORK_ROLE_LOCAL) {
      if ((g_FrontendLoadedLevelAsset != (FrontendLoadedLevelRuntimeImage370 *)0x0) &&
         (0xffff < (g_FrontendLoadedLevelAsset->header).pathState.levelPathOffsetOrLoadedFieldGrid))
      {
        Resource_Release((void *)(g_FrontendLoadedLevelAsset->header).pathState.
                                 levelPathOffsetOrLoadedFieldGrid);
      }
      Resource_Release(g_FrontendLoadedLevelAsset);
      g_FrontendLoadedLevelAsset = (FrontendLoadedLevelRuntimeImage370 *)0x0;
      packageLoadResult = Package_LoadEntry((uint16_t *)&g_FrontendScenarioPathScratchUtf16);
      checkedResult = (*g_FatalErrorPrimaryDispatchCf)((uint32_t)packageLoadResult.bufferOrError,packageLoadResult.failed);
      g_FrontendLoadedLevelAsset = (FrontendLoadedLevelRuntimeImage370 *)checkedResult.valueOrError;
      encodeCursorOrSize = (g_FrontendLoadedLevelAsset->header).common.buildMetadata.
                assetRelativeAddressAnchor28 +
                ((g_FrontendLoadedLevelAsset->header).pathState.levelPathOffsetOrLoadedFieldGrid -
                0x28);
      WidePath_SetExtensionCode(0x646c66,(uint16_t *)encodeCursorOrSize);
      WidePath_CombineDirectoryAndLeaf
                ((uint16_t *)&g_LevelResourcePathScratchUtf16,(uint16_t *)encodeCursorOrSize,
                 (uint16_t *)&g_ExecutableDirectoryUtf16);
      packageLoadResult = Package_LoadEntry((uint16_t *)encodeCursorOrSize);
      loadedLevelAsset = g_FrontendLoadedLevelAsset;
      transferSourceBytes = g_PackageScratchBuffer;
      sourceGrid = packageLoadResult.bufferOrError;
      if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_HOST) != SESSION_NETWORK_ROLE_LOCAL) {
        fieldGridAllocationSize = (sourceGrid->common).allocationSizeBytes;
        *(AssetAllocationSizeBytes *)g_PackageScratchBuffer =
             (g_FrontendLoadedLevelAsset->header).common.allocationSizeBytes;
        *(AssetAllocationSizeBytes *)(transferSourceBytes + 4) = fieldGridAllocationSize;
        encodeCursorOrSize = transferSourceBytes + 0x10;
        savedFieldGrid = sourceGrid;
        encodeResult = PckCodec_EncodeHuffmanRle
                           (0x7fffe8,encodeCursorOrSize,(loadedLevelAsset->header).common.allocationSizeBytes,
                            (uint8_t *)loadedLevelAsset);
        checkedResult = (*g_FatalErrorPrimaryDispatchCf)(encodeResult.byteCountOrError,encodeResult.failed);
        statusOrByteCount = checkedResult.valueOrError;
        *(uint32_t *)(transferSourceBytes + 8) = statusOrByteCount;
        encodeResult = PckCodec_EncodeFieldGrid
                           (0x7fffe8 - statusOrByteCount,encodeCursorOrSize + statusOrByteCount,
                            (sourceGrid->common).allocationSizeBytes,sourceGrid);
        checkedResult = (*g_FatalErrorPrimaryDispatchCf)(encodeResult.byteCountOrError,encodeResult.failed);
        *(uint32_t *)(transferSourceBytes + 0xc) = checkedResult.valueOrError;
        encodeCursorOrSize = encodeCursorOrSize + statusOrByteCount + (checkedResult.valueOrError - (int)transferSourceBytes);
        allocResult = (*g_MemoryApi.alloc)((uint32_t)encodeCursorOrSize);
        checkedResult = (*g_FatalErrorPrimaryDispatchCf)(allocResult.payloadOrError,allocResult.failed);
        transferDwordCursor = (uint32_t *)checkedResult.valueOrError;
        for (remainingDwords = (uint32_t)encodeCursorOrSize >> 2; remainingDwords != 0; remainingDwords = remainingDwords - 1) {
          *transferDwordCursor = *(uint32_t *)transferSourceBytes;
          transferSourceBytes = transferSourceBytes + 4;
          transferDwordCursor = transferDwordCursor + 1;
        }
        sourceGrid = savedFieldGrid;
        UiTransferMailbox_SetOutgoingBuffer((UiTransferPayloadByteCount)encodeCursorOrSize,(uint32_t *)checkedResult.valueOrError)
        ;
      }
      (loadedLevelAsset->header).pathState.levelPathOffsetOrLoadedFieldGrid = (uint32_t)sourceGrid;
      FrontendPlayerRuntime_InitializeFactionAssignments();
    }
    else {
      UiTransferMailbox_MarkUnavailable();
      g_FrontendScenarioTransferState = 5;
    }
    goto FrontendMainLoop_ProcessFrameAndPendingPageAction;
  }
FrontendMainLoop_InitializeRequestedPage:
  initResult = Frontend_Init(initialRomRecordId);
  statusOrByteCount = initResult.frontendRootOrError;
  g_FrontendPendingPageAction = countOrSelectedId;
  if (!initResult.failed) goto FrontendMainLoop_ProcessFrameAndPendingPageAction;
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
  uint32_t callbackResult;
  GraphicsCursorFrameIndex cursorFrameIndex;
  uint64_t bestHit;
  
  bestHit = FrontendModelPointerContext_FindBestEligibleModelHitTarget(pointerY,pointerX,context);
  context->selectedHitMetric = (int)bestHit;
  context->selectedModelNode = (ModelRuntimeNode *)(bestHit >> 0x20);
  if ((context->contextFlags & FRONTEND_MODEL_POINTER_CONTEXT_ROUTE_TO_SECONDARY_CALLBACK) != 0) {
    if (context->resolvedActionCallback108 != (FrontendModelPointerResolvedActionCallbackProc *)0x0)
    {
      callbackResult = (*context->resolvedActionCallback108)
                        (context->callbackArgumentF0,context->callbackArgumentEC,
                         context->callbackArgumentE8,context->selectedHitMetric,
                         context->selectedModelNode,context);
      return callbackResult;
    }
    return 0;
  }
  if ((context->contextFlags & FRONTEND_MODEL_POINTER_CONTEXT_ROUTE_TO_BUILTIN_ACTION_RESOLUTION) ==
      0) {
    if (context->resolvedActionCallback104 != (FrontendModelPointerResolvedActionCallbackProc *)0x0)
    {
      callbackResult = (*context->resolvedActionCallback104)
                        (context->callbackArgumentF0,context->callbackArgumentEC,
                         context->callbackArgumentE8,context->selectedHitMetric,
                         context->selectedModelNode,context);
      return callbackResult;
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
        return 0x11;
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
    cursorFrameIndex = 0x10;
  }
  else {
    cursorFrameIndex = 0x11;
  }
  return cursorFrameIndex;
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
  uint64_t bestHit;
  
  callbackContext->scratchCoordinate160 = pointerX;
  callbackContext->scratchCoordinate164 = pointerY;
  bestHit = FrontendModelPointerContext_FindBestEligibleModelHitTarget
                    (pointerY,pointerX,(FrontendModelPointerContextRuntimeState118 *)callbackContext
                    );
  callbackContext->selectedModelNode = (ModelRuntimeNode *)(bestHit >> 0x20);
  callbackContext->selectedHitMetric = (int)bestHit;
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
  uint64_t bestHit;
  
  bestHit = FrontendModelPointerContext_FindBestEligibleModelHitTarget
                    (pointerY,pointerX,callbackContext);
  callbackContext->selectedModelNode = (ModelRuntimeNode *)(bestHit >> 0x20);
  callbackContext->selectedHitMetric = (int)bestHit;
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
  uint64_t bestHit;
  
  callbackContext->scratchCoordinate168 = pointerX;
  callbackContext->scratchCoordinate16C = pointerY;
  bestHit = FrontendModelPointerContext_FindBestEligibleModelHitTarget
                    (pointerY,pointerX,(FrontendModelPointerContextRuntimeState118 *)callbackContext
                    );
  callbackContext->selectedModelNode = (ModelRuntimeNode *)(bestHit >> 0x20);
  callbackContext->selectedHitMetric = (int)bestHit;
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
  UiPixelCoordinate cursorOverrideX;
  UiPixelCoordinate cursorOverrideY;
  uint32_t queuedPrimitiveCount;
  GraphicsWorldCoordinateQ12 originY;
  void (*renderHierarchyProc)(ModelRuntimeNode *);
  ModelRuntimeNode *modelNode;
  /* The selection overlays after the scene take their rectangle from EDX/ECX/EDI/ESI (top/left/bottom/right).
     On the complete path these are reloaded with the clipped rectangle; when a primitive-queue reset fails the
     original jumps straight to the end and the overlays receive whatever those registers held at that point
     (cursor Y in Q12, the listener Z, the render procedure address, a model-list pointer, ...). The overlay*
     variables below model exactly those register contents. */
  UiPixelCoordinate overlayClipTop;
  UiPixelCoordinate overlayClipBottom;
  UiPixelCoordinate overlayClipRight;
  bool entryFound;
  PrimitiveQueueResult queueResult;

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
  cursorOverrideY = g_CursorOverrideY;
  cursorOverrideX = g_CursorOverrideX;
  control->selectedModelNode = (ModelRuntimeNode *)0x0;
  control->selectedHitMetric = 0x7fffffff;
  control->callbackArgumentE8 = 0x7fffffff;
  control->callbackArgumentEC = 0x7fffffff;
  control->callbackArgumentF0 = 0x7fffffff;
  overlayClipTop = cursorOverrideY << 0xc;
  control->cursorWorldXQ12 = cursorOverrideX << 0xc;
  control->cursorWorldYQ12 = overlayClipTop;
  control->renderedPrimitiveCount = 0;
  Graphics_SetProjectionClipRect(clipTop,clipLeft,clipBottom,clipRight);
  Graphics_SetViewProjectionParameters
            (control->projectionShift,control->viewAngle1,control->viewAngle0,
             control->projectionScale,control->hitReferenceWorldZQ12,control->hitReferenceWorldYQ12,
             control->hitReferenceWorldXQ12);
  originY = clipLeft;
  if ((control->contextFlags & 0x10000) != 0) {
    originY = control->targetPositionYQ12;
    overlayClipTop = ((int)control->committedDistanceOrSoundZOffset >> 2) + control->targetPositionZQ12;
    SpatialSound_RebuildListenerTransformFromPose
              (control->viewAngle1,control->viewAngle0,overlayClipTop,originY,control->targetPositionXQ12);
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
  queueResult = GraphicsPrimitiveQueue_ResetGlobal();
  overlayClipRight = clipTop;
  overlayClipBottom = clipBottom;
  if (!queueResult.failed) {
    Graphics_SetActivePrimitiveQueue(queueResult.queue);
    control->activePrimitiveQueue = queueResult.queue;
    if (control->renderPhaseCallback15C != (InGameWorldOverlayPhaseCallbackProc *)0x0) {
      (*control->renderPhaseCallback15C)(GRAPHICS_STATE_DISABLED,(WorldRuntimeContext *)control);
    }
    renderHierarchyProc = ModelRuntime_CullAndRenderHierarchyRecursive;
    if ((control->contextFlags & FRONTEND_MODEL_POINTER_CONTEXT_COMPARE_HITS_BY_METRIC_ONLY) != 0) {
      renderHierarchyProc = ModelRuntime_RenderHierarchyRecursiveAlternatePath;
    }
    overlayClipTop = (UiPixelCoordinate)(uintptr_t)renderHierarchyProc;
    for (modelNode = control->candidateModelListHead; modelNode != (ModelRuntimeNode *)0x0;
        modelNode = (ModelRuntimeNode *)(modelNode->common).nextNode) {
      if ((((modelNode->runtimeFlags & 0x40) == 0) && ((modelNode->runtimeFlags & 0x200) != 0)) &&
         (modelNode->runtimeFlags = modelNode->runtimeFlags & 0xfffffffd,
         (modelNode->tintArgb & 0xff000000) != 0)) {
        (*renderHierarchyProc)(modelNode);
      }
    }
    if (control->renderPhaseCallback15C != (InGameWorldOverlayPhaseCallbackProc *)0x0) {
      (*control->renderPhaseCallback15C)(GRAPHICS_STATE_ENABLED,(WorldRuntimeContext *)control);
    }
    (*PTR_GraphicsPrimitiveQueue_RadixSortForRendering_00485844)
              ((control->base).nodeFlags & 8,control->activePrimitiveQueue);
    (*g_GraphicsDrawPrimitiveQueue)
              (clipTop,clipLeft,clipBottom,clipRight,control->activePrimitiveQueue);
    queuedPrimitiveCount = GraphicsPrimitiveQueue_GetCount(control->activePrimitiveQueue);
    control->renderedPrimitiveCount = control->renderedPrimitiveCount + queuedPrimitiveCount;
    (*g_SpinLockReleaseAndInvoke)(control->renderSpinLockReleaseCallback,control->renderSpinLock);
    (*g_SpinLockAcquire)(control->renderSpinLock);
    overlayClipBottom = 0; /* EDI: the model loop ran to its NULL terminator */
    if (((control->contextFlags & 0x4000) != 0) && (control->fieldGrid != (FieldGridAsset *)0x0)) {
      queueResult = GraphicsPrimitiveQueue_ResetGlobal();
      if (queueResult.failed) goto EndSceneAndDrawOverlays;
      Graphics_SetActivePrimitiveQueue(queueResult.queue);
      control->activePrimitiveQueue = queueResult.queue;
      TerrainProjectedGrid_TransformShadeAndQueue(control->fieldGrid,control);
      (*PTR_GraphicsPrimitiveQueue_RadixSortForRendering_00485844)
                ((control->base).nodeFlags & 8,control->activePrimitiveQueue);
      (*g_GraphicsDrawPrimitiveQueue)
                (clipTop,clipLeft,clipBottom,clipRight,control->activePrimitiveQueue);
      queuedPrimitiveCount = GraphicsPrimitiveQueue_GetCount(control->activePrimitiveQueue);
      control->renderedPrimitiveCount = control->renderedPrimitiveCount + queuedPrimitiveCount;
    }
    (*g_SpinLockReleaseAndInvoke)(control->renderSpinLockReleaseCallback,control->renderSpinLock);
    (*g_SpinLockAcquire)(control->renderSpinLock);
    if ((control->contextFlags & 0x20000) != 0) {
      modelNode = control->candidateModelListHead;
      overlayClipBottom = (UiPixelCoordinate)(uintptr_t)modelNode; /* EDI */
      queueResult = GraphicsPrimitiveQueue_ResetGlobal();
      if (queueResult.failed) goto EndSceneAndDrawOverlays;
      Graphics_SetActivePrimitiveQueue(queueResult.queue);
      control->activePrimitiveQueue = queueResult.queue;
      if (modelNode != (ModelRuntimeNode *)0x0) {
        GraphicsShadingGeneratedTexture_ResetPassScratchAndClearAlphaPlanes();
        do {
          if ((((modelNode->runtimeFlags & 0x40) == 0) && ((modelNode->runtimeFlags & 0x100) != 0)) &&
             ((modelNode->tintArgb & 0xff000000) != 0)) {
            GraphicsShadingGeneratedTexture_ProcessRenderableHierarchy
                      (modelNode,(GeneratedTextureRenderContextView *)control);
          }
          modelNode = (ModelRuntimeNode *)(modelNode->common).nextNode;
        } while (modelNode != (ModelRuntimeNode *)0x0);
        GraphicsShadingGeneratedTexture_RefreshTouchedAlphaSubresources();
        overlayClipBottom = 0;
      }
      (*PTR_GraphicsPrimitiveQueue_RadixSortForRendering_00485844)
                ((control->base).nodeFlags & 8,control->activePrimitiveQueue);
      (*g_GraphicsDrawPrimitiveQueue)
                (clipTop,clipLeft,clipBottom,clipRight,control->activePrimitiveQueue);
      queuedPrimitiveCount = GraphicsPrimitiveQueue_GetCount(control->activePrimitiveQueue);
      control->renderedPrimitiveCount = control->renderedPrimitiveCount + queuedPrimitiveCount;
    }
    (*g_SpinLockReleaseAndInvoke)(control->renderSpinLockReleaseCallback,control->renderSpinLock);
    (*g_SpinLockAcquire)(control->renderSpinLock);
    queueResult = GraphicsPrimitiveQueue_ResetGlobal();
    if (!queueResult.failed) {
      Graphics_SetActivePrimitiveQueue(queueResult.queue);
      control->activePrimitiveQueue = queueResult.queue;
      if (control->renderPhaseCallback15C != (InGameWorldOverlayPhaseCallbackProc *)0x0) {
        (*control->renderPhaseCallback15C)(GRAPHICS_STATE_DISABLED,(WorldRuntimeContext *)control);
      }
      renderHierarchyProc = ModelRuntime_CullAndRenderHierarchyRecursive;
      if ((control->contextFlags & FRONTEND_MODEL_POINTER_CONTEXT_COMPARE_HITS_BY_METRIC_ONLY) != 0)
      {
        renderHierarchyProc = ModelRuntime_RenderHierarchyRecursiveAlternatePath;
      }
      for (modelNode = control->candidateModelListHead; modelNode != (ModelRuntimeNode *)0x0;
          modelNode = (ModelRuntimeNode *)(modelNode->common).nextNode) {
        if (((modelNode->runtimeFlags & 0x240) == 0) &&
           (modelNode->runtimeFlags = modelNode->runtimeFlags & 0xfffffffd,
           (modelNode->tintArgb & 0xff000000) != 0)) {
          (*renderHierarchyProc)(modelNode);
        }
      }
      if (control->renderPhaseCallback15C != (InGameWorldOverlayPhaseCallbackProc *)0x0) {
        (*control->renderPhaseCallback15C)(GRAPHICS_STATE_ENABLED,(WorldRuntimeContext *)control);
      }
      (*PTR_GraphicsPrimitiveQueue_RadixSortForRendering_00485844)
                ((control->base).nodeFlags & 8,control->activePrimitiveQueue);
      (*g_GraphicsDrawPrimitiveQueue)
                (clipTop,clipLeft,clipBottom,clipRight,control->activePrimitiveQueue);
      queuedPrimitiveCount = GraphicsPrimitiveQueue_GetCount(control->activePrimitiveQueue);
      control->renderedPrimitiveCount = control->renderedPrimitiveCount + queuedPrimitiveCount;
      (*g_SpinLockReleaseAndInvoke)(control->renderSpinLockReleaseCallback,control->renderSpinLock);
      (*g_SpinLockAcquire)(control->renderSpinLock);
      originY = clipLeft;
      overlayClipTop = clipTop;
      overlayClipRight = clipRight;
      overlayClipBottom = clipBottom;
      if ((((clipRight == (control->base).left) && (clipLeft == (control->base).right)) &&
          (clipBottom == (control->base).top)) && (clipTop == (control->base).bottom)) {
        control->contextFlags = control->contextFlags | 0x800;
      }
    }
  }
EndSceneAndDrawOverlays:
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
                   (overlayClipTop,originY,overlayClipBottom,
                    overlayClipRight),
        control->selectedOverlayEntity != (GameEntityRuntime *)0x0)) &&
       (entryFound = SelectionInfo_FindEntryCf(control->selectedOverlayEntity), entryFound)) {
      SelectionOverlay_RenderArmyMetricsForEntity
                (overlayClipTop,originY,overlayClipBottom,
                 overlayClipRight,control->selectedOverlayEntity);
    }
    if ((control->contextFlags & 0x80) != 0) {
      SelectionOverlay_DrawBoundsFrame
                (overlayClipTop,originY,overlayClipBottom,
                 overlayClipRight,control->scratchCoordinate16C,
                 control->scratchCoordinate168,control->scratchCoordinate164,
                 control->scratchCoordinate160);
    }
    if ((control->contextFlags & 0x200000) != 0) {
      SelectionOverlay_DrawMarkerADForFieldGridTerrainPoints
                (overlayClipTop,originY,overlayClipBottom,
                 overlayClipRight,control->terrainMarkerPointCount174,
                 control->terrainMarkerCoordinatePairs170,control->fieldGrid);
    }
    if (((control->contextFlags & 0x100000) != 0) && (control->callbackArgumentF0 != 0x7fffffff)) {
      SelectionOverlay_DrawMarkerACForWorldSurfacePoint
                (overlayClipTop,originY,overlayClipBottom,
                 overlayClipRight,
                 (uint32_t)((g_UiCommandModeGColorVariantLimit & 0xff000000) != 0),
                 control->callbackArgumentEC,control->callbackArgumentE8,control->fieldGrid);
    }
    if ((control->contextFlags & 0x800000) != 0) {
      SelectionOverlay_DrawMarkerAEForVisibleProjectedGridVertices
                (overlayClipTop,originY,overlayClipBottom,
                 overlayClipRight,control->fieldGrid);
    }
    if ((control->contextFlags & 0x1000000) != 0) {
      SelectionOverlay_DrawMarkerAFB0ForProjectedVertexStateFlags
                (overlayClipTop,originY,overlayClipBottom,
                 overlayClipRight,control->fieldGrid);
    }
    if ((control->contextFlags & 0x2000000) != 0) {
      SelectionOverlay_DrawMarkerB1B2ForProjectedVertexMask1800
                (overlayClipTop,originY,overlayClipBottom,
                 overlayClipRight,(uint8_t)control->overlayMarkerStateB4,
                 control->fieldGrid);
    }
    if ((((control->contextFlags & 0x4000) != 0) && (control->fieldGrid != (FieldGridAsset *)0x0))
       && ((g_UiCommandRuntimeFlags & 0x40) != 0)) {
      SelectionOverlay_DrawMarkerAFForProjectedVertexFlag8000
                (overlayClipTop,originY,overlayClipBottom,
                 overlayClipRight,control->fieldGrid);
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
     (callbackContext->rightReleaseCallback118 != NULL)) {
    (*callbackContext->rightReleaseCallback118)(callbackContext);
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
  InGameWorldTransientStateClearCallbackProc *clearTransientCallback;
  uint32_t screenDeltaY;
  AngleTurn32 elevationAngle;
  
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
  }
  else if ((callbackContext->runtimeFlags & 0x8000) != 0) {
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
        /* EDX: the pointer Y delta, as in the other pitch branches (the decompile lost it). */
        WorldMotion_AdjustPitchClampAndClearFieldGridDirty(elevationAngle,callbackContext);
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
  }
  else {
    if ((callbackContext->runtimeFlags & 0x200) == 0) {
      return;
    }
    if ((g_KeyboardStateMask & 0xc) != 0) {
      callbackContext->runtimeFlags = callbackContext->runtimeFlags & 0xfffffff8;
      callbackContext->runtimeFlags = callbackContext->runtimeFlags | 8;
      WorldMotion_AdjustPitchClampAndRecomputePosition(elevationAngle,callbackContext);
    }
    else if ((g_KeyboardStateMask & 0x30) != 0) {
      callbackContext->runtimeFlags = callbackContext->runtimeFlags & 0xfffffff4;
      callbackContext->runtimeFlags = callbackContext->runtimeFlags | 4;
      WorldMotion_AdjustDistanceClampAndRecomputePosition(elevationAngle,callbackContext);
    }
    else if ((g_KeyboardStateMask & 3) != 0) {
      callbackContext->runtimeFlags = callbackContext->runtimeFlags & 0xfffffff2;
      callbackContext->runtimeFlags = callbackContext->runtimeFlags | 2;
      WorldMotion_AdjustHeadingAndRecomputePosition(screenDeltaY,callbackContext);
    }
    else if (((callbackContext->runtimeFlags & 0x4000000) != 0) && ((g_CursorButtonState & 1) != 0)) {
      /* Flag 0x4000000 with the button held: 0x40000000 selects pitch, 0x80000000 distance. */
      callbackContext->runtimeFlags = callbackContext->runtimeFlags & 0xfffffff0;
      if ((callbackContext->runtimeFlags & 0x40000000) != 0) {
        callbackContext->runtimeFlags = callbackContext->runtimeFlags | 8;
        WorldMotion_AdjustPitchClampAndRecomputePosition(elevationAngle,callbackContext);
      }
      else if ((callbackContext->runtimeFlags & 0x80000000) != 0) {
        callbackContext->runtimeFlags = callbackContext->runtimeFlags | 4;
        WorldMotion_AdjustDistanceClampAndRecomputePosition(elevationAngle,callbackContext);
      }
    }
    else if (((callbackContext->runtimeFlags & 0x4000000) == 0) && ((g_CursorButtonState & 1) == 0)) {
      callbackContext->runtimeFlags = callbackContext->runtimeFlags & 0xfffffff1;
      callbackContext->runtimeFlags = callbackContext->runtimeFlags | 1;
      WorldRuntime_TranslateCameraByScreenDelta(elevationAngle,screenDeltaY,callbackContext);
      WorldRuntime_RecomputeMotionEndpointAgainstFieldSurface(callbackContext);
    }
    else {
      /* Heading drag (button state opposite to flag 0x4000000); afterwards 0x40000000 adds a distance step and
         0x80000000 a pitch step (flags re-read after the heading call). */
      callbackContext->runtimeFlags = callbackContext->runtimeFlags & 0xfffffff2;
      callbackContext->runtimeFlags = callbackContext->runtimeFlags | 2;
      WorldMotion_AdjustHeadingAndRecomputePosition(screenDeltaY,callbackContext);
      if ((callbackContext->runtimeFlags & 0x40000000) != 0) {
        callbackContext->runtimeFlags = callbackContext->runtimeFlags | 4;
        WorldMotion_AdjustDistanceClampAndRecomputePosition(elevationAngle,callbackContext);
      }
      else if ((callbackContext->runtimeFlags & 0x80000000) != 0) {
        callbackContext->runtimeFlags = callbackContext->runtimeFlags | 8;
        WorldMotion_AdjustPitchClampAndRecomputePosition(elevationAngle,callbackContext);
      }
    }
  }
  (*g_PointerSetPosition)(callbackContext->pointerCaptureY,callbackContext->pointerCaptureX);
  clearTransientCallback = (callbackContext->fieldRegion).clearTransientStateCallback;
  WorldRuntime_CaptureMotionStateToSnapshot(callbackContext);
  if (clearTransientCallback != (InGameWorldTransientStateClearCallbackProc *)0x0) {
    (*clearTransientCallback)(callbackContext);
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
  int scaledWheelDelta;
  
  if (((callbackContext->runtimeFlags & 0x10) == 0) &&
     ((callbackContext->runtimeFlags & 0x8300) != 0)) {
    if ((g_KeyboardStateMask & 0xc) == 0) {
      scaledWheelDelta = wheelDelta * g_WorldMotionPointerWheelInputScale;
      callbackContext->runtimeFlags = callbackContext->runtimeFlags & 0xfffffff4;
      callbackContext->runtimeFlags = callbackContext->runtimeFlags | 4;
      WorldMotion_AdjustDistanceClampAndRecomputePosition(scaledWheelDelta,callbackContext);
      WorldRuntime_CaptureMotionStateToSnapshot(callbackContext);
    }
    else {
      scaledWheelDelta = wheelDelta * g_WorldMotionPointerWheelInputScale;
      callbackContext->runtimeFlags = callbackContext->runtimeFlags & 0xfffffff8;
      callbackContext->runtimeFlags = callbackContext->runtimeFlags | 8;
      WorldMotion_AdjustPitchClampAndRecomputePosition(scaledWheelDelta,callbackContext);
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
  bool keyboardEventCarry;
  
  if ((control->keyboardFallbackCf != (UiRootKeyboardFallbackCf *)0x0) &&
     (keyboardEventCarry = (*control->keyboardFallbackCf)(keyboardStateMask,keyCode,(UiRootNode *)control),
     !keyboardEventCarry)) {
    return keyboardEventCarry;
  }
  keyboardEventCarry = UiNode_DefaultKeyboardEventMoveFocusNextCf(keyboardStateMask,keyCode,&control->base);
  return keyboardEventCarry;
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
  uint32_t *callbackStateCounter;
  UQ12 targetDistance;
  UQ12 convergenceStep;
  UQ12 clampedCommittedDistance;
  FixedDirectionXyzRegs12 cameraOffset;
  
  if ((callbackContext->runtimeFlags & 0x40) != 0) {
    callbackStateCounter = &(callbackContext->selection).reservedCallbackState40;
    *callbackStateCounter = *callbackStateCounter + 1;
  }
  if ((callbackContext->runtimeFlags & 0x48110) == 0) {
    clampedCommittedDistance = (callbackContext->motion).committedDistanceQ12;
    if ((int)clampedCommittedDistance < (int)callbackContext->minimumCameraDistanceQ12) {
      clampedCommittedDistance = callbackContext->minimumCameraDistanceQ12;
    }
    if ((int)callbackContext->maximumCameraDistanceQ12 < (int)clampedCommittedDistance) {
      clampedCommittedDistance = callbackContext->maximumCameraDistanceQ12;
    }
    targetDistance = (callbackContext->motion).targetDistanceQ12;
    convergenceStep = g_WorldMotionTargetDistanceConvergenceStepQ12;
    if ((int)(clampedCommittedDistance * 0xf) >> 4 <= (int)targetDistance) {
      if ((int)targetDistance <= (int)(clampedCommittedDistance * 0x11) >> 4) {
        return;
      }
      convergenceStep = -g_WorldMotionTargetDistanceConvergenceStepQ12;
    }
    (callbackContext->motion).targetDistanceQ12 = targetDistance + convergenceStep;
    cameraOffset = FixedMath_DirectionFromAnglesScaledRegs
                      (-(callbackContext->motion).pitchAngle,
                       (callbackContext->motion).headingAngle ^ 0x8000,targetDistance + convergenceStep);
    (callbackContext->motion).positionXQ12 =
         cameraOffset.eax + (callbackContext->motion).targetPositionXQ12;
    (callbackContext->motion).positionYQ12 =
         cameraOffset.ecx + (callbackContext->motion).targetPositionYQ12;
    (callbackContext->motion).positionZQ12 =
         cameraOffset.edx + (callbackContext->motion).targetPositionZQ12;
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
  XeniteAmountQ4 xeniteStorageLimit;
  TritiumAmountQ4 tritiumStorageLimit;
  int xeniteCurrentDisplay;
  FactionProgressAmountQ4 progressCurrentQ4;
  FactionProgressAmountQ4 progressLimitQ4;
  FactionArmyContributionValue activeArmyScaleValue;
  InGameRuntimeRootImageC3E4 *runtimeRoot;
  int activeFactionIndex;
  
  runtimeRoot = g_InGameRuntimeRoot;
  activeFactionIndex = (g_InGameRuntimeRoot->worldRuntime0A30).activeFactionRuntimeIndex;
  xeniteStorageLimit = g_GameFactionRuntimeImage.records[activeFactionIndex].xeniteStorageLimitQ4;
  xeniteCurrentDisplay = (int)g_GameFactionRuntimeImage.records[activeFactionIndex].xeniteCurrentQ4 >> 4;
  g_InGameRuntimeRoot->primaryResourceDisplayCurrent49B4 = xeniteCurrentDisplay;
  runtimeRoot->primaryResourceDisplayLimit49B8 = (int)xeniteStorageLimit >> 4;
  (*g_WideNumberFormatUtf16)
            (WIDE_FORMAT_WRITE_TERMINATOR,0,10,1,xeniteCurrentDisplay,
             (uint16_t *)&g_FrontendCurrentFactionPrimaryResourceTextUtf16);
  tritiumStorageLimit = g_GameFactionRuntimeImage.records[activeFactionIndex].tritiumStorageLimitQ4;
  progressCurrentQ4 = g_GameFactionRuntimeImage.records[activeFactionIndex].baselineEnergySupplyQ4;
  runtimeRoot->secondaryResourceDisplayCurrent4A4C =
       (int)g_GameFactionRuntimeImage.records[activeFactionIndex].tritiumCurrentQ4 >> 4;
  runtimeRoot->secondaryResourceDisplayLimit4A50 = (int)tritiumStorageLimit >> 4;
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
  /* Rewritten from the assembly (0x00548030-0x0054813A and the continuations 0x00548140 /
     0x00548190-0x005483BA, which Ghidra does not assign to any function). The decompiled version
     jumped into the original machine code. EBX is g_FrontendRootNode. CF set = not handled. */
  UiCommandDispatchRecord *record = g_FrontendCommandDispatchRecords_00_Code00030071_Modifier30;
  uint8_t *root = (uint8_t *)g_FrontendRootNode;
  uint32_t target = 0;

  (void)frontendRuntime;
  for (;; record++) {
    uint32_t flags = record->modifierClassFlags;
    if (record->commandCode == 0) {
      return true;
    }
    if (record->commandCode != commandCode) {
      continue;
    }
    if (flags == 0) {
      if ((modifierFlags & 0x3f) != 0) continue;
    }
    else {
      if ((flags & 3) != 0) {
        if ((modifierFlags & 3) == 0) continue;
      }
      else if ((modifierFlags & 3) != 0) {
        continue;
      }
      if ((flags & 0x30) == 0) {
        if (((modifierFlags & 0xc) == 0) || ((modifierFlags & 0x30) != 0)) continue;
      }
      else if ((flags & 0xc) == 0) {
        if (((modifierFlags & 0xc) != 0) || ((modifierFlags & 0x30) == 0)) continue;
      }
      else {
        if (((modifierFlags & 0xc) == 0) || ((modifierFlags & 0x30) == 0)) continue;
      }
    }
    target = (uint32_t)record->continuationEntryAddress;
    break;
  }
  switch (target) {
  case 0x548140:
    if (UiPageStack_ActivePageNotInListCf((UiPageStackControl *)FRONTEND_UI(root,frontendPageStack)).valueOrError == 0xb) {
      if ((g_SessionNetworkRoleFlags & 3) != 0) {
        FrontendCommandQueue_EnqueueLocalPlayerCommand(0x3b0,0,0,1);
      }
      else {
        FrontendPlayerRuntime_XorStateMaskByPlayerId(g_LocalPlayerRuntimeId,0,0,1);
      }
    }
    break;
  case 0x548190: {
    FrontendPlayerRuntimeRecord *player;
    TextResolveResult text;
    if ((g_SessionNetworkRoleFlags & 3) == 0) {
      if ((g_FrontendLoadedCampaignAsset == 0) && (g_FrontendScenarioInitializationCount == 0)) {
        UiActionQueue_Enqueue(0,root);
        break;
      }
      Resource_Release((void *)(uintptr_t)g_FrontendLoadedCampaignAsset);
      g_FrontendLoadedCampaignAsset = 0;
      g_FrontendScenarioInitializationCount = 0;
      g_FrontendNetworkState = 0;
      (*g_NetworkBackendSlot3)();
      (*g_NetworkBackendSlot1)();
      UiPageStack_SetActiveIndex(0,(UiPageStackControl *)FRONTEND_UI(root,frontendPageStack));
      FRONTEND_UI_FIELD(root,menuRoomModelView,0x4C,uint32_t) =
           FRONTEND_UI_FIELD(root,menuRoomModelView,0x4C,uint32_t) & 0xffffdfff;
      g_FrontendPendingPageAction = 0;
      g_FrontendRomTransitionContextValue = 0;
      FrontendRomTransition_ActivateRecordByIdCf
                (1,(WorldRuntimeContext *)FRONTEND_UI(root,menuRoomModelView));
      break;
    }
    /* Leaving a network session: host (bit 0, 0x00548320) or client (bit 1, 0x00548250). */
    {
      int wasHost = (g_SessionNetworkRoleFlags & 1) != 0;
      g_SessionNetworkRoleFlags = g_SessionNetworkRoleFlags & 0xfffffffc;
      g_FrontendNetworkState = 0;
      g_FrontendScenarioInitializationCount = 0;
      (*g_NetworkBackendSlot3)();
      (*g_NetworkBackendSlot1)();
      UiPageStack_SetActiveIndex(0,(UiPageStackControl *)FRONTEND_UI(root,frontendPageStack));
      FRONTEND_UI_FIELD(root,menuRoomModelView,0x4C,uint32_t) =
           FRONTEND_UI_FIELD(root,menuRoomModelView,0x4C,uint32_t) & 0xffffdfff;
      g_FrontendPendingPageAction = 0;
      g_FrontendRomTransitionContextValue = 0;
      player = (FrontendPlayerRuntimeRecord *)g_FrontendPlayerRuntimeBlocks;
      FrontendRomTransition_ActivateRecordByIdCf
                (1,(WorldRuntimeContext *)FRONTEND_UI(root,menuRoomModelView));
      text = TextResource_Resolve(wasHost ? 0xff02 : 0xff04);
      RichTextCommandStream_PatchPayloadBySelector(0,(uint8_t *)player + 0x18,text.text);
      FrontendRecentTextHistory_InsertAndRebuild5(text.text);
      if (!wasHost) {
        g_FrontendPlayerRuntimeBlockCount = 1;
        g_LocalPlayerRuntimeId = 0;
        *(uint32_t *)((uint8_t *)player + 0x18) = 0;
        *(uint32_t *)((uint8_t *)player + 0x14) = 0;
        *(uint32_t *)((uint8_t *)player + 0x60) = 0;
        *(uint32_t *)((uint8_t *)player + 0x64) = 0;
        *(uint32_t *)((uint8_t *)player + 0x68) = 0;
      }
    }
    break;
  }
  default:
    Thandor_Log("Frontend dispatch: unhandled continuation %08x",target);
    break;
  }
  return false;
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
uint32_t __thandor_eax_preserve_ecx_edx
FrontendRuntime_UpdatePointerContextAndSceneViewCf
          (uint32_t pointerValue0,uint32_t pointerValue1,uint32_t pointerValue2,uint32_t pointerValue3,
          void *pointedRecord,FrontendPointerSceneRuntimeView43E8 *frontendRuntime)

{
  int32_t *hintEdgeField;
  uint16_t *previousCommandStream;
  UiNodeBase *control;
  UiNodeVtable *controlVtable;
  RomAssetRecordPrefix *pointedRomRecord;
  int *transitionRecord;
  uint32_t resultCode;
  uint16_t *commandStream;
  RomRecordId recordId;
  int channel3OrHintValue;
  int keyframeChannel5;
  int channel4OrHalfHeight;
  RichTextExtentRegs textExtent;
  StatusResult pageStackStatus;
  TextResolveResult hintTextResult;
  TextureSizeResult windowTextureSize;
  
  resultCode = 0;
  channel3OrHintValue = 0;
  if (((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_CLIENT) == SESSION_NETWORK_ROLE_LOCAL) &&
     (pageStackStatus = UiPageStack_ActivePageNotInListCf(&frontendRuntime->activePageStack1A0),
     pageStackStatus.valueOrError == 0)) {
    pointedRomRecord = RomRegistry_FindRecordBySlotValue((RomRegistrySlotValue)pointedRecord);
    recordId = 0xf0000000;
    if (pointedRomRecord != (RomAssetRecordPrefix *)0x0) {
      recordId = pointedRomRecord->recordId;
    }
    transitionRecord = RomRecordTable_FindRecordById(recordId,g_FrontendActiveRomRecordTable);
    /* Skip records without a transition, network-only pages (3/4/9/negative) in a networked session and the
       network page (2) when no backend exists. */
    if ((transitionRecord != (int *)0x0) &&
       ((((transitionRecord[8] != 3 && (transitionRecord[8] != 4)) && (transitionRecord[8] != 9)) &&
         (transitionRecord[8] >= 0)) ||
        ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_NETWORKED_MASK) == SESSION_NETWORK_ROLE_LOCAL)) &&
       ((transitionRecord[8] != 2) || (g_NetworkBackendInstanceCount != 0))) {
      channel3OrHintValue = transitionRecord[3];
      channel4OrHalfHeight = transitionRecord[4];
      keyframeChannel5 = transitionRecord[5];
      /* Rebuild the camera spline only when the target keyframe changed. (When channels 0-2 already match
         the original skips storing them; storing the equal values here is equivalent.) */
      if ((((*transitionRecord != g_FrontendRomTransitionKeyframe1Channel0Q12) ||
           (transitionRecord[1] != g_FrontendRomTransitionKeyframe1Channel1Q12)) ||
          (transitionRecord[2] != g_FrontendRomTransitionKeyframe1Channel2Q12)) ||
         (((channel3OrHintValue != g_FrontendRomTransitionKeyframe1Channel3Q12) ||
          (channel4OrHalfHeight != g_FrontendRomTransitionKeyframe1Channel4Q12)) ||
          (keyframeChannel5 != g_FrontendRomTransitionKeyframe1Channel5Q12))) {
        g_FrontendRomTransitionKeyframe1Channel0Q12 = *transitionRecord;
        g_FrontendRomTransitionKeyframe1Channel1Q12 = transitionRecord[1];
        g_FrontendRomTransitionKeyframe1Channel2Q12 = transitionRecord[2];
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
        g_FrontendRomTransitionKeyframe1Channel3Q12 = channel3OrHintValue;
        g_FrontendRomTransitionKeyframe1Channel4Q12 = channel4OrHalfHeight;
        g_FrontendRomTransitionKeyframe1Channel5Q12 = keyframeChannel5;
        WorldMotionSpline_BuildSixChannelCurves
                  (2,(WorldMotionSplineKeyframe *)&g_FrontendRomTransitionKeyframe0Channel0Q12);
      }
      channel3OrHintValue = transitionRecord[6];
      resultCode = 7;
    }
  }
  if (channel3OrHintValue == 0) {
    if (g_FrontendPendingPageActionDepth == 0) {
      (frontendRuntime->hintControl4390).hintActive50 = 0;
      (frontendRuntime->hintControl4390).commandStream54 = (uint16_t *)0x0;
      return resultCode;
    }
    channel3OrHintValue = 1;
  }
  previousCommandStream = (frontendRuntime->hintControl4390).commandStream54;
  hintTextResult = TextResource_Resolve(channel3OrHintValue + 0x2000);
  commandStream = hintTextResult.text;
  if (commandStream != previousCommandStream) {
    (frontendRuntime->hintControl4390).commandStream54 = commandStream;
    textExtent = RichTextCommandStream_MeasureRegs(g_UiTextStyleNormal,commandStream);
    channel3OrHintValue = (int)(textExtent.widthPixels + 1) >> 1;
    channel4OrHalfHeight = (int)(textExtent.heightPixels + 1) >> 1;
    (frontendRuntime->hintControl4390).base.leftOffset = channel3OrHintValue;
    (frontendRuntime->hintControl4390).base.topOffset = channel4OrHalfHeight;
    (frontendRuntime->hintControl4390).base.right = -channel3OrHintValue;
    (frontendRuntime->hintControl4390).base.bottom = -channel4OrHalfHeight;
    windowTextureSize = (*g_GraphicsTextureSourceGetLogicalSize)(0x72,g_UiWindowTextureSource);
    channel3OrHintValue = windowTextureSize.logicalWidthPixels + 3;
    control = (frontendRuntime->hintControl4390).base.nextSibling;
    (frontendRuntime->hintControl4390).hintActive50 = 1;
    hintEdgeField = &(frontendRuntime->hintControl4390).base.leftOffset;
    *hintEdgeField = *hintEdgeField + channel3OrHintValue;
    hintEdgeField = &(frontendRuntime->hintControl4390).base.topOffset;
    *hintEdgeField = *hintEdgeField + windowTextureSize.logicalHeightPixels;
    controlVtable = control->vtable;
    hintEdgeField = &(frontendRuntime->hintControl4390).base.right;
    *hintEdgeField = *hintEdgeField - channel3OrHintValue;
    hintEdgeField = &(frontendRuntime->hintControl4390).base.bottom;
    *hintEdgeField = *hintEdgeField - windowTextureSize.logicalHeightPixels;
    (*controlVtable->layout)(control);
  }
  return resultCode;
}


/* Address: 0x00548BE0.
   Ownership: ui/frontend/runtime.
   Purpose: Six-argument frontend runtime callback installed at object slot +0x5C. It performs no operation and
   returns with ret 0x18.
*/
void FrontendRuntimeCallback5C_NoOp
               (uint32_t argument1,uint32_t argument2,uint32_t argument3,uint32_t argument4,uint32_t argument5,
               uint32_t argument6)

{
  return;
}

/* Address: 0x00548BF0.
   Ownership: ui/frontend/runtime.
   Purpose: Six-argument frontend runtime callback installed at object slot +0x60. It performs no operation and
   returns with ret 0x18.
*/
void FrontendRuntimeCallback60_NoOp
               (uint32_t argument1,uint32_t argument2,uint32_t argument3,uint32_t argument4,uint32_t argument5,
               uint32_t argument6)

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
          (uint32_t argument1,uint32_t argument2,uint32_t argument3,uint32_t argument4,
          FrontendCallbackArgument5 argument5,uint32_t argument6)

{
  RomAssetRecordPrefix *slotRecord;
  RomRecordTableIndex recordIndex;
  
  if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_CLIENT) == SESSION_NETWORK_ROLE_LOCAL) {
    slotRecord = RomRegistry_FindRecordBySlotValue(argument5);
    if (slotRecord != (RomAssetRecordPrefix *)0x0) {
      recordIndex = RomRecordTable_FindIndexById(slotRecord->recordId,g_FrontendActiveRomRecordTable);
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
void FrontendRuntimeCallback68_DispatchRefresh1340(uint32_t callbackArgument)

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
void __thandor_void_preserve_eax_ecx_edx FrontendRecentTextHistory_InsertAndRebuild5(uint16_t *text)

{
  RecentTextHistoryPointerList *output;
  
  /* lineCount + textLines of the chat history box form the pointer list. */
  output = (RecentTextHistoryPointerList *)
           &((UiConditionalActionControl *)FRONTEND_UI(g_FrontendRootNode,chatMessageHistory))->lineCount;
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
void FrontendCallback_ApplyGameSpeedOrDispatch02C0(uint32_t callbackArgument)

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
void FrontendCallback_ReleaseSelectedResourceOrDispatch0320(uint32_t callbackArgument)

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
void __thandor_preserve_eax FrontendCallback_ReturnToMainPageOrDispatch0DC0(uint32_t callbackArgument)

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
void FrontendCallback_ReturnToMainPageOrDispatchState4(uint32_t callbackArgument)

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
FrontendCallback_ReturnToMainPageOrDispatch0DC0_Secondary(uint32_t callbackArgument)

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
  UiNodeFlags *compactLayoutFlags;
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
    compactLayoutFlags = &(frontendRootPage->compactLayoutControl).nodeFlags;
    *compactLayoutFlags = *compactLayoutFlags | 0x2000;
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
  uint8_t *compactLayoutFlagBytes;
  GraphicsAdapterRecord *adapters;
  uint16_t *deviceNameText;
  uint32_t modeValue;
  uint32_t displacedValueA;
  uint32_t displacedValueB;
  GraphicsDisplayModeCount remainingModes;
  GraphicsDisplayMode *displayMode;
  TextResolveResult fallbackNameResult;
  
  UiPageStack_SetActiveIndex
            (6,(UiPageStackControl *)(source[-3].resolutionRows.rows[9].reserved0008_0067 + 0x48));
  if ((int)g_FramebufferWidth < 0x281) {
    compactLayoutFlagBytes = source[-3].resolutionRows.rows[6].reserved0008_0067 + 0x2c;
    *(uint32_t *)compactLayoutFlagBytes = *(uint32_t *)compactLayoutFlagBytes | 0x2000;
  }
  g_FrontendUiDisplayModeAndTaskAssignmentScratch.displayModeScratch.candidateValues10[0] =
       0xffffffff;
  g_FrontendUiDisplayModeAndTaskAssignmentScratch.displayModeScratch.candidateValues10[1] =
       0xffffffff;
  g_FrontendUiDisplayModeAndTaskAssignmentScratch.displayModeScratch.candidateValues10[2] =
       0xffffffff;
  g_FrontendUiDisplayModeAndTaskAssignmentScratch.displayModeScratch.candidateValues10[3] =
       0xffffffff;
  remainingModes = g_GraphicsDisplayModeCount;
  displayMode = g_GraphicsDisplayModes;
  do {
    modeValue = displayMode->bitsPerPixel;
    if ((((modeValue != g_FrontendUiDisplayModeAndTaskAssignmentScratch.displayModeScratch.
                    candidateValues10[0]) &&
         (modeValue != g_FrontendUiDisplayModeAndTaskAssignmentScratch.displayModeScratch.
                   candidateValues10[1])) &&
        (modeValue != g_FrontendUiDisplayModeAndTaskAssignmentScratch.displayModeScratch.
                  candidateValues10[2])) &&
       (modeValue != g_FrontendUiDisplayModeAndTaskAssignmentScratch.displayModeScratch.
                 candidateValues10[3])) {
      displacedValueB = modeValue;
      if (modeValue < g_FrontendUiDisplayModeAndTaskAssignmentScratch.displayModeScratch.
                  candidateValues10[0]) {
        LOCK();
        UNLOCK();
        displacedValueB = g_FrontendUiDisplayModeAndTaskAssignmentScratch.displayModeScratch.candidateValues10
                [0];
        g_FrontendUiDisplayModeAndTaskAssignmentScratch.displayModeScratch.candidateValues10[0] =
             modeValue;
      }
      displacedValueA = displacedValueB;
      if ((uint32_t)displacedValueB <
          g_FrontendUiDisplayModeAndTaskAssignmentScratch.displayModeScratch.candidateValues10[1]) {
        LOCK();
        UNLOCK();
        displacedValueA = g_FrontendUiDisplayModeAndTaskAssignmentScratch.displayModeScratch.candidateValues10
                [1];
        g_FrontendUiDisplayModeAndTaskAssignmentScratch.displayModeScratch.candidateValues10[1] =
             displacedValueB;
      }
      displacedValueB = displacedValueA;
      if ((uint32_t)displacedValueA <
          g_FrontendUiDisplayModeAndTaskAssignmentScratch.displayModeScratch.candidateValues10[2]) {
        LOCK();
        UNLOCK();
        displacedValueB = g_FrontendUiDisplayModeAndTaskAssignmentScratch.displayModeScratch.candidateValues10
                [2];
        g_FrontendUiDisplayModeAndTaskAssignmentScratch.displayModeScratch.candidateValues10[2] =
             displacedValueA;
      }
      if ((uint32_t)displacedValueB <
          g_FrontendUiDisplayModeAndTaskAssignmentScratch.displayModeScratch.candidateValues10[3]) {
        LOCK();
        UNLOCK();
        g_FrontendUiDisplayModeAndTaskAssignmentScratch.displayModeScratch.candidateValues10[3] =
             displacedValueB;
      }
    }
    displayMode = displayMode + 1;
    remainingModes = remainingModes - 1;
  } while (remainingModes != 0);
  (source->colorDepthRows).rows[0].bitsPerPixel =
       g_FrontendUiDisplayModeAndTaskAssignmentScratch.displayModeScratch.candidateValues10[0];
  (source->colorDepthRows).rows[1].bitsPerPixel =
       g_FrontendUiDisplayModeAndTaskAssignmentScratch.displayModeScratch.candidateValues10[1];
  (source->colorDepthRows).rows[2].bitsPerPixel =
       g_FrontendUiDisplayModeAndTaskAssignmentScratch.displayModeScratch.candidateValues10[2];
  (source->colorDepthRows).rows[3].bitsPerPixel =
       g_FrontendUiDisplayModeAndTaskAssignmentScratch.displayModeScratch.candidateValues10[3];
  adapters = g_GraphicsAdapters;
  (source->adapterRows).rows[0].adapterDescriptionUtf16 = g_GraphicsAdapters->driverDescriptionUtf16
  ;
  if ((adapters->deviceGuid).Data1 == 0) {
    fallbackNameResult = TextResource_Resolve(0x212d);
    deviceNameText = fallbackNameResult.text;
  }
  else {
    deviceNameText = adapters->deviceNameUtf16;
  }
  (source->adapterRows).rows[0].deviceNameUtf16 = deviceNameText;
  adapters = g_GraphicsAdapters;
  if (1 < g_GraphicsAdapterCount) {
    (source->adapterRows).rows[1].adapterDescriptionUtf16 =
         g_GraphicsAdapters[1].driverDescriptionUtf16;
    if (adapters[1].deviceGuid.Data1 == 0) {
      fallbackNameResult = TextResource_Resolve(0x212d);
      deviceNameText = fallbackNameResult.text;
    }
    else {
      deviceNameText = adapters[1].deviceNameUtf16;
    }
    (source->adapterRows).rows[1].deviceNameUtf16 = deviceNameText;
  }
  adapters = g_GraphicsAdapters;
  if (2 < g_GraphicsAdapterCount) {
    (source->adapterRows).rows[2].adapterDescriptionUtf16 =
         g_GraphicsAdapters[2].driverDescriptionUtf16;
    if (adapters[2].deviceGuid.Data1 == 0) {
      fallbackNameResult = TextResource_Resolve(0x212d);
      deviceNameText = fallbackNameResult.text;
    }
    else {
      deviceNameText = adapters[2].deviceNameUtf16;
    }
    (source->adapterRows).rows[2].deviceNameUtf16 = deviceNameText;
  }
  adapters = g_GraphicsAdapters;
  if (3 < g_GraphicsAdapterCount) {
    (source->adapterRows).rows[3].adapterDescriptionUtf16 =
         g_GraphicsAdapters[3].driverDescriptionUtf16;
    if (adapters[3].deviceGuid.Data1 == 0) {
      fallbackNameResult = TextResource_Resolve(0x212d);
      deviceNameText = fallbackNameResult.text;
    }
    else {
      deviceNameText = adapters[3].deviceNameUtf16;
    }
    (source->adapterRows).rows[3].deviceNameUtf16 = deviceNameText;
  }
  adapters = g_GraphicsAdapters;
  if (4 < g_GraphicsAdapterCount) {
    (source->adapterRows).rows[4].adapterDescriptionUtf16 =
         g_GraphicsAdapters[4].driverDescriptionUtf16;
    if (adapters[4].deviceGuid.Data1 == 0) {
      fallbackNameResult = TextResource_Resolve(0x212d);
      deviceNameText = fallbackNameResult.text;
    }
    else {
      deviceNameText = adapters[4].deviceNameUtf16;
    }
    (source->adapterRows).rows[4].deviceNameUtf16 = deviceNameText;
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
  remainingModes = g_GraphicsDisplayModeCount;
  displayMode = g_GraphicsDisplayModes;
  do {
    modeValue = displayMode->width * 0x10000 + displayMode->height;
    if ((((modeValue != g_FrontendUiDisplayModeAndTaskAssignmentScratch.displayModeScratch.
                    candidateValues10[0]) &&
         (modeValue != g_FrontendUiDisplayModeAndTaskAssignmentScratch.displayModeScratch.
                   candidateValues10[1])) &&
        ((modeValue != g_FrontendUiDisplayModeAndTaskAssignmentScratch.displayModeScratch.
                   candidateValues10[2] &&
         ((modeValue != g_FrontendUiDisplayModeAndTaskAssignmentScratch.displayModeScratch.
                    candidateValues10[3] &&
          (modeValue != g_FrontendUiDisplayModeAndTaskAssignmentScratch.displayModeScratch.
                    candidateValues10[4])))))) &&
       ((modeValue != g_FrontendUiDisplayModeAndTaskAssignmentScratch.displayModeScratch.
                  candidateValues10[5] &&
        ((((modeValue != g_FrontendUiDisplayModeAndTaskAssignmentScratch.displayModeScratch.
                     candidateValues10[6] &&
           (modeValue != g_FrontendUiDisplayModeAndTaskAssignmentScratch.displayModeScratch.
                     candidateValues10[7])) &&
          (modeValue != g_FrontendUiDisplayModeAndTaskAssignmentScratch.displayModeScratch.
                    candidateValues10[8])) &&
         (modeValue != g_FrontendUiDisplayModeAndTaskAssignmentScratch.displayModeScratch.
                   candidateValues10[9])))))) {
      displacedValueB = modeValue;
      if (modeValue < g_FrontendUiDisplayModeAndTaskAssignmentScratch.displayModeScratch.
                  candidateValues10[0]) {
        LOCK();
        UNLOCK();
        displacedValueB = g_FrontendUiDisplayModeAndTaskAssignmentScratch.displayModeScratch.candidateValues10
                [0];
        g_FrontendUiDisplayModeAndTaskAssignmentScratch.displayModeScratch.candidateValues10[0] =
             modeValue;
      }
      displacedValueA = displacedValueB;
      if ((uint32_t)displacedValueB <
          g_FrontendUiDisplayModeAndTaskAssignmentScratch.displayModeScratch.candidateValues10[1]) {
        LOCK();
        UNLOCK();
        displacedValueA = g_FrontendUiDisplayModeAndTaskAssignmentScratch.displayModeScratch.candidateValues10
                [1];
        g_FrontendUiDisplayModeAndTaskAssignmentScratch.displayModeScratch.candidateValues10[1] =
             displacedValueB;
      }
      displacedValueB = displacedValueA;
      if ((uint32_t)displacedValueA <
          g_FrontendUiDisplayModeAndTaskAssignmentScratch.displayModeScratch.candidateValues10[2]) {
        LOCK();
        UNLOCK();
        displacedValueB = g_FrontendUiDisplayModeAndTaskAssignmentScratch.displayModeScratch.candidateValues10
                [2];
        g_FrontendUiDisplayModeAndTaskAssignmentScratch.displayModeScratch.candidateValues10[2] =
             displacedValueA;
      }
      displacedValueA = displacedValueB;
      if ((uint32_t)displacedValueB <
          g_FrontendUiDisplayModeAndTaskAssignmentScratch.displayModeScratch.candidateValues10[3]) {
        LOCK();
        UNLOCK();
        displacedValueA = g_FrontendUiDisplayModeAndTaskAssignmentScratch.displayModeScratch.candidateValues10
                [3];
        g_FrontendUiDisplayModeAndTaskAssignmentScratch.displayModeScratch.candidateValues10[3] =
             displacedValueB;
      }
      displacedValueB = displacedValueA;
      if ((uint32_t)displacedValueA <
          g_FrontendUiDisplayModeAndTaskAssignmentScratch.displayModeScratch.candidateValues10[4]) {
        LOCK();
        UNLOCK();
        displacedValueB = g_FrontendUiDisplayModeAndTaskAssignmentScratch.displayModeScratch.candidateValues10
                [4];
        g_FrontendUiDisplayModeAndTaskAssignmentScratch.displayModeScratch.candidateValues10[4] =
             displacedValueA;
      }
      displacedValueA = displacedValueB;
      if ((uint32_t)displacedValueB <
          g_FrontendUiDisplayModeAndTaskAssignmentScratch.displayModeScratch.candidateValues10[5]) {
        LOCK();
        UNLOCK();
        displacedValueA = g_FrontendUiDisplayModeAndTaskAssignmentScratch.displayModeScratch.candidateValues10
                [5];
        g_FrontendUiDisplayModeAndTaskAssignmentScratch.displayModeScratch.candidateValues10[5] =
             displacedValueB;
      }
      displacedValueB = displacedValueA;
      if ((uint32_t)displacedValueA <
          g_FrontendUiDisplayModeAndTaskAssignmentScratch.displayModeScratch.candidateValues10[6]) {
        LOCK();
        UNLOCK();
        displacedValueB = g_FrontendUiDisplayModeAndTaskAssignmentScratch.displayModeScratch.candidateValues10
                [6];
        g_FrontendUiDisplayModeAndTaskAssignmentScratch.displayModeScratch.candidateValues10[6] =
             displacedValueA;
      }
      displacedValueA = displacedValueB;
      if ((uint32_t)displacedValueB <
          g_FrontendUiDisplayModeAndTaskAssignmentScratch.displayModeScratch.candidateValues10[7]) {
        LOCK();
        UNLOCK();
        displacedValueA = g_FrontendUiDisplayModeAndTaskAssignmentScratch.displayModeScratch.candidateValues10
                [7];
        g_FrontendUiDisplayModeAndTaskAssignmentScratch.displayModeScratch.candidateValues10[7] =
             displacedValueB;
      }
      displacedValueB = displacedValueA;
      if ((uint32_t)displacedValueA <
          g_FrontendUiDisplayModeAndTaskAssignmentScratch.displayModeScratch.candidateValues10[8]) {
        LOCK();
        UNLOCK();
        displacedValueB = g_FrontendUiDisplayModeAndTaskAssignmentScratch.displayModeScratch.candidateValues10
                [8];
        g_FrontendUiDisplayModeAndTaskAssignmentScratch.displayModeScratch.candidateValues10[8] =
             displacedValueA;
      }
      if ((uint32_t)displacedValueB <
          g_FrontendUiDisplayModeAndTaskAssignmentScratch.displayModeScratch.candidateValues10[9]) {
        LOCK();
        UNLOCK();
        g_FrontendUiDisplayModeAndTaskAssignmentScratch.displayModeScratch.candidateValues10[9] =
             displacedValueB;
      }
    }
    displayMode = displayMode + 1;
    remainingModes = remainingModes - 1;
  } while (remainingModes != 0);
  modeValue = g_FrontendUiDisplayModeAndTaskAssignmentScratch.displayModeScratch.candidateValues10[0] &
          0xffff;
  (source->resolutionRows).rows[0].width =
       g_FrontendUiDisplayModeAndTaskAssignmentScratch.displayModeScratch.candidateValues10[0] >>
       0x10;
  (source->resolutionRows).rows[0].height = modeValue;
  modeValue = g_FrontendUiDisplayModeAndTaskAssignmentScratch.displayModeScratch.candidateValues10[1] &
          0xffff;
  (source->resolutionRows).rows[1].width =
       g_FrontendUiDisplayModeAndTaskAssignmentScratch.displayModeScratch.candidateValues10[1] >>
       0x10;
  (source->resolutionRows).rows[1].height = modeValue;
  modeValue = g_FrontendUiDisplayModeAndTaskAssignmentScratch.displayModeScratch.candidateValues10[2] &
          0xffff;
  (source->resolutionRows).rows[2].width =
       g_FrontendUiDisplayModeAndTaskAssignmentScratch.displayModeScratch.candidateValues10[2] >>
       0x10;
  (source->resolutionRows).rows[2].height = modeValue;
  modeValue = g_FrontendUiDisplayModeAndTaskAssignmentScratch.displayModeScratch.candidateValues10[3] &
          0xffff;
  (source->resolutionRows).rows[3].width =
       g_FrontendUiDisplayModeAndTaskAssignmentScratch.displayModeScratch.candidateValues10[3] >>
       0x10;
  (source->resolutionRows).rows[3].height = modeValue;
  modeValue = g_FrontendUiDisplayModeAndTaskAssignmentScratch.displayModeScratch.candidateValues10[4] &
          0xffff;
  (source->resolutionRows).rows[4].width =
       g_FrontendUiDisplayModeAndTaskAssignmentScratch.displayModeScratch.candidateValues10[4] >>
       0x10;
  (source->resolutionRows).rows[4].height = modeValue;
  modeValue = g_FrontendUiDisplayModeAndTaskAssignmentScratch.displayModeScratch.candidateValues10[5] &
          0xffff;
  (source->resolutionRows).rows[5].width =
       g_FrontendUiDisplayModeAndTaskAssignmentScratch.displayModeScratch.candidateValues10[5] >>
       0x10;
  (source->resolutionRows).rows[5].height = modeValue;
  modeValue = g_FrontendUiDisplayModeAndTaskAssignmentScratch.displayModeScratch.candidateValues10[6] &
          0xffff;
  (source->resolutionRows).rows[6].width =
       g_FrontendUiDisplayModeAndTaskAssignmentScratch.displayModeScratch.candidateValues10[6] >>
       0x10;
  (source->resolutionRows).rows[6].height = modeValue;
  modeValue = g_FrontendUiDisplayModeAndTaskAssignmentScratch.displayModeScratch.candidateValues10[7] &
          0xffff;
  (source->resolutionRows).rows[7].width =
       g_FrontendUiDisplayModeAndTaskAssignmentScratch.displayModeScratch.candidateValues10[7] >>
       0x10;
  (source->resolutionRows).rows[7].height = modeValue;
  modeValue = g_FrontendUiDisplayModeAndTaskAssignmentScratch.displayModeScratch.candidateValues10[8] &
          0xffff;
  (source->resolutionRows).rows[8].width =
       g_FrontendUiDisplayModeAndTaskAssignmentScratch.displayModeScratch.candidateValues10[8] >>
       0x10;
  (source->resolutionRows).rows[8].height = modeValue;
  modeValue = g_FrontendUiDisplayModeAndTaskAssignmentScratch.displayModeScratch.candidateValues10[9] &
          0xffff;
  (source->resolutionRows).rows[9].width =
       g_FrontendUiDisplayModeAndTaskAssignmentScratch.displayModeScratch.candidateValues10[9] >>
       0x10;
  (source->resolutionRows).rows[9].height = modeValue;
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
  uint32_t currentEntryCount;
  
  for (currentEntryCount = ((UiConditionalActionControl *)source)->lineCount; 4 < currentEntryCount;
      currentEntryCount = currentEntryCount - 1) {
    RecentTextHistory_RemoveOldest();
  }
  RecentTextHistory_RemoveOldest();
  RecentTextHistory_SortAndBuildPointerList
            (5,(RecentTextHistoryPointerList *)&((UiConditionalActionControl *)source)->lineCount);
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
  UiListRowIndex selectedBackendIndex;
  int remainingDwords;
  uint32_t *endpointSourceDwordCursor;
  uint32_t *endpointDestinationDwordCursor;
  NetworkSetSessionResult setSessionResult;
  FatalErrorCheckResult fatalCheckResult;
  NetworkOpenBindResult openBindResult;
  NetworkSessionContext *staleSessionContext;
  UiListRowIndex staleBackendIndex;
  
  selectedBackendIndex = UiPointerList_GetSelectedIndexVariantACf(backendList);
  staleSessionContext = g_NetworkBackendSessionContext;
  if (g_NetworkBackendInstanceCount <= selectedBackendIndex) {
    return;
  }
  (*g_NetworkBackendSlot3)();
  (*g_NetworkBackendSlot1)();
  staleBackendIndex = selectedBackendIndex;
  setSessionResult = (*g_NetworkBackendSlot0)(selectedBackendIndex);
  fatalCheckResult = (*g_FatalErrorRuntimeDispatchCf)(setSessionResult.valueOrError,setSessionResult.failed);
  if (!fatalCheckResult.failed) {
    openBindResult = (*g_NetworkBackendSlot2)(0x3a1);
    fatalCheckResult = (*g_FatalErrorRuntimeDispatchCf)(openBindResult.valueOrError,openBindResult.failed);
    if (!fatalCheckResult.failed) {
      endpointSourceDwordCursor = (uint32_t *)&g_NetworkLocalEndpointDescriptor16;
      endpointDestinationDwordCursor = (uint32_t *)&g_FrontendNetworkEndpointScratch;
      for (remainingDwords = 4; remainingDwords != 0; remainingDwords = remainingDwords + -1) {
        *endpointDestinationDwordCursor = *endpointSourceDwordCursor;
        endpointSourceDwordCursor = endpointSourceDwordCursor + 1;
        endpointDestinationDwordCursor = endpointDestinationDwordCursor + 1;
      }
      (*g_NetworkBackendSlot7)
                (&g_FrontendNetworkEndpointTextUtf16,
                 (WinSockAddress *)&g_FrontendNetworkEndpointScratch);
      UiNodeList_SuppressActionId(0x2002,&THANDOR_CONTAINER_OF(backendList, FrontendNetworkSetupPageState4BCC, backendList)->rootNode);
      UiPointerList_InitializeColumnLayout
                (0,g_FrontendSessionListRows,&THANDOR_CONTAINER_OF(backendList, FrontendNetworkSetupPageState4BCC, backendList)->sessionList);
      UiTransfer_SendPacketType10000Value2931Cf();
      return;
    }
    (*g_NetworkBackendSlot1)(); /* cleanup takes no arguments; Ghidra passed stale staleBackendIndex */
  }
  setSessionResult = (*g_NetworkBackendSlot0)(selectedBackendIndex);
  if (!setSessionResult.failed) {
    openBindResult = (*g_NetworkBackendSlot2)(0x3a1);
    if (!openBindResult.failed) {
      return;
    }
    (*g_NetworkBackendSlot1)(); /* cleanup takes no arguments; Ghidra passed stale staleSessionContext */
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
  UiRootCallbacks *rootCallbacks;
  uint32_t previousResultCount;
  uint64_t elapsedTimeUnits;
  InGameRuntimeRootImageC3E4 *runtimeRoot;
  int32_t endMovieNumber;
  int countOrActiveFactions;
  uint32_t playbackRateHz;
  TextResourceId resourceId;
  FrontendPlayerRuntimeBlockCount remainingPlayerBlocks;
  UiPageStackControl *stack;
  WorldRuntimeContext *worldRuntime;
  int recordCursorOrRemaining;
  int factionIndex;
  uint8_t *copySource;
  FactionRuntimeLifecycleObservedState *factionLifecycleState;
  FrontendPlayerRuntimeRecord *playerBlock;
  uint8_t *copyDestination;
  bool framebufferAccessFailed;
  MovieOpenResult movieOpenResult;
  MovieFrameResult frameAdvanceResult;
  TextResolveResult resultsTextResult;
  TextResolveResult levelTitleResult;
  
  runtimeRoot = g_InGameRuntimeRoot;
  (*g_GraphicsCursorSetFrame)(0);
  g_CursorVisibilityToken = g_CursorVisibilityToken + -1;
  if ((runtimeRoot != (InGameRuntimeRootImageC3E4 *)0x0) &&
     (rootCallbacks = (runtimeRoot->rootUi0000).callbacks, g_EndMoviePath != (uint16_t *)0x0)) {
    rootCallbacks->keyboardFallbackCf = EndMovieUiRuntime_DispatchCommandByFlagsCf;
    rootCallbacks->frameUpdate = EndMovieUiRuntime_HandleModeTransitionCf;
    if (g_FrontendLoadedCampaignAsset != 0) {
      countOrActiveFactions = *(int *)(g_FrontendLoadedCampaignAsset + 0xb8);
      recordCursorOrRemaining = g_FrontendLoadedCampaignAsset + 0x200;
      do {
        if (*(int *)(g_FrontendLoadedCampaignAsset + 0xc4) == *(int *)(recordCursorOrRemaining + 0x100)) {
          if (g_EndMovieVariantIndex == 0) {
            endMovieNumber = *(int32_t *)(recordCursorOrRemaining + 0x40 + g_EndMovieSelectionIndex * 4);
          }
          else {
            endMovieNumber = *(int32_t *)(recordCursorOrRemaining + 0x20 + g_EndMovieSelectionIndex * 4);
          }
          (*g_WideNumberFormatUtf16)
                    (WIDE_FORMAT_PAD_WITH_ZERO,0,4,1,endMovieNumber,(uint16_t *)(u_flm_ende0000_flm_0050df4a + 8))
          ;
          g_EndMoviePath = (uint16_t *)u_flm_ende0000_flm_0050df4a;
          break;
        }
        recordCursorOrRemaining = recordCursorOrRemaining + 0x180;
        countOrActiveFactions = countOrActiveFactions + -1;
      } while (countOrActiveFactions != 0);
    }
    Movie_Close();
    framebufferAccessFailed = (*g_GraphicsFramebufferBeginAccess)();
    if (!framebufferAccessFailed) {
      (*g_GraphicsFramebufferFillRectArgb)
                (g_FramebufferHeight,g_FramebufferWidth,0,0,g_FramebufferHeight,g_FramebufferWidth,0
                 ,0,0xff000000,g_FramebufferAccess);
      (*g_GraphicsFramebufferEndAccess)();
      (*g_GraphicsFramebufferPresent)(g_FramebufferAccess);
    }
    framebufferAccessFailed = (*g_GraphicsFramebufferBeginAccess)();
    if (!framebufferAccessFailed) {
      (*g_GraphicsFramebufferFillRectArgb)
                (g_FramebufferHeight,g_FramebufferWidth,0,0,g_FramebufferHeight,g_FramebufferWidth,0
                 ,0,0xff000000,g_FramebufferAccess);
      (*g_GraphicsFramebufferEndAccess)();
      (*g_GraphicsFramebufferPresent)(g_FramebufferAccess);
    }
    movieOpenResult = Movie_Open(1,g_EndMoviePath);
    runtimeRoot = g_InGameRuntimeRoot;
    if (!movieOpenResult.failed) {
      g_EndMoviePendingTicks = 0;
      playbackRateHz = movieOpenResult.playbackRateHz; /* ECX left by Movie_Open */
      (*g_TimerRegisterPeriodic)(playbackRateHz,FrontendSession_PeriodicTick);
      /* EDX = g_InGameRuntimeRoot + 0x17C in the original; the decompiler lost it. */
      stack = (UiPageStackControl *)INGAME_UI(runtimeRoot,primaryPageStack);
      UiPageStack_SetActiveIndex(1,stack);
      frameAdvanceResult = Movie_AdvanceFrame();
      if (!frameAdvanceResult.ended) {
        runtimeRoot->activeEndMovieRuntime022C = (MovieRuntime *)frameAdvanceResult.movieOrError;
        runtimeRoot->endMoviePlaybackState0230 = 0;
        g_EndMoviePendingTicks = 0;
        do {
          if (g_EndMoviePendingTicks != 0) {
            g_EndMoviePendingTicks = g_EndMoviePendingTicks - 1;
            frameAdvanceResult = Movie_AdvanceFrame();
            if (frameAdvanceResult.ended) {
              g_UiCommandRuntimeFlags = g_UiCommandRuntimeFlags & 0xfffff7ff;
            }
          }
          UiNode_InvalidateRoot((UiNodeBase *)runtimeRoot);
          UiFrame_ProcessAndPresent();
        } while ((g_UiCommandRuntimeFlags & 0x800) != 0);
      }
      g_CursorVisibilityToken = g_CursorVisibilityToken + 1;
      UiPageStack_SetActiveIndex(1,&runtimeRoot->endMoviePageStack02F8);
      recordCursorOrRemaining = 7;
      factionLifecycleState = g_GameFactionRuntimeImage.tail.factionLifecycleStates;
      countOrActiveFactions = 0;
      factionIndex = 1;
      do {
        factionLifecycleState = factionLifecycleState + 1;
        if (*factionLifecycleState != 0) {
          countOrActiveFactions = countOrActiveFactions + 1;
          GameFactionRuntime_RecomputeProgressAndScoreMetrics
                    (factionIndex,&runtimeRoot->worldRuntime0A30);
        }
        factionIndex = factionIndex + 1;
        recordCursorOrRemaining = recordCursorOrRemaining + -1;
      } while (recordCursorOrRemaining != 0);
      if (countOrActiveFactions != 0) {
        *(int *)(runtimeRoot->opaque034C_08D3 + 0x114) = countOrActiveFactions;
        *(int *)(runtimeRoot->opaque034C_08D3 + 400) = countOrActiveFactions;
        *(int *)(runtimeRoot->opaque034C_08D3 + 0x20c) = countOrActiveFactions;
        elapsedTimeUnits = (uint64_t)(g_GameFactionRuntimeImage.tail.periodicClockTick + 0x12bf) / 0x12c0;
        (*g_LocaleFormatTimeFieldsUtf16)
                  ((uint32_t)(elapsedTimeUnits / 0x3c),(uint32_t)(elapsedTimeUnits % 0x3c),
                   (uint16_t *)&g_EndGameElapsedTimeScratchUtf16);
        resultsTextResult = TextResource_Resolve(0x21c0);
        resourceId = g_InGameLevelTitleTextResourceIndex + 0x2230;
        RichTextCommandStream_PatchPayloadBySelector(1,&g_EndGameElapsedTimeScratchUtf16,resultsTextResult.text)
        ;
        levelTitleResult = TextResource_Resolve(resourceId);
        RichTextCommandStream_PatchPayloadBySelector(0,levelTitleResult.text,resultsTextResult.text);
        UiNodeList_UnsuppressActionId(0x101b,(UiNodeBase *)runtimeRoot);
        if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_NETWORKED_MASK) ==
            SESSION_NETWORK_ROLE_LOCAL) {
          UiNodeList_SuppressActionId(0x1025,(UiNodeBase *)runtimeRoot);
        }
        remainingPlayerBlocks = g_FrontendPlayerRuntimeBlockCount;
        playerBlock = g_FrontendPlayerRuntimeBlocks;
        if (((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_HOST) != SESSION_NETWORK_ROLE_LOCAL)
           && (1 < g_FrontendPlayerRuntimeBlockCount)) {
          UiNodeList_SuppressActionId(0x101b,(UiNodeBase *)runtimeRoot);
          remainingPlayerBlocks = g_FrontendPlayerRuntimeBlockCount;
          playerBlock = g_FrontendPlayerRuntimeBlocks;
        }
        do {
          (playerBlock->factionAssignment).readyOrWaitState = 0;
          remainingPlayerBlocks = remainingPlayerBlocks - 1;
          playerBlock = playerBlock + 1;
        } while (remainingPlayerBlocks != 0);
        if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_NETWORKED_MASK) ==
            SESSION_NETWORK_ROLE_LOCAL) {
          previousResultCount = *(uint32_t *)(runtimeRoot->opaque034C_08D3 + 0x110);
          *(int *)(runtimeRoot->opaque034C_08D3 + 0x110) = *(int *)(runtimeRoot->opaque034C_08D3 + 0x110) + -1
          ;
          countOrActiveFactions = previousResultCount - 3;
          if (2 < previousResultCount && countOrActiveFactions != 0) {
            copySource = runtimeRoot->opaque034C_08D3 + 300;
            copyDestination = runtimeRoot->opaque034C_08D3 + 0x128;
            for (; countOrActiveFactions != 0; countOrActiveFactions = countOrActiveFactions + -1) {
              *(uint32_t *)copyDestination = *(uint32_t *)copySource;
              copySource = copySource + 4;
              copyDestination = copyDestination + 4;
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
    }
    else {
      g_CursorVisibilityToken = g_CursorVisibilityToken + 1;
    }
  }
  else {
    g_CursorVisibilityToken = g_CursorVisibilityToken + 1;
  }
  rootCallbacks = (g_InGameRuntimeRoot->rootUi0000).callbacks;
  rootCallbacks->keyboardFallbackCf = InGameHotkeys_DispatchCommandByFlagsCf;
  rootCallbacks->frameUpdate = EndGameResultsUiRuntime_UpdateAndHandleInputCf;
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

FrontendInitResult __thandor_eax_cf_preserve_ecx_edx Frontend_Init(RomRecordId initialRomRecordId)

{
  SessionNetworkRoleFlags pendingBlockCountOrRoleMask;
  IDirectSoundBuffer *musicBuffer;
  uint32_t settingValue;
  SoundSampleAsset *loadedSample;
  FrontendRootResourceSlots5954 *fillCursorOrResult;
  FrontendRootResourceSlots5954 *frontendUiState;
  DirectSoundVoiceSet *musicVoiceSet;
  uint32_t *nameSlotOrSourceDwords;
  uint32_t *settingsCopySourceDwordsB;
  SessionNetworkRoleFlags remainingBlockCount;
  int remainingDwords;
  uint32_t remainingBackends;
  uint32_t *frontendInitTemplateDwords;
  uint16_t *backendDisplayName;
  uint32_t *settingsCopySourceDwordsA;
  FrontendPlayerRuntimeRecord *playerBlock;
  uint32_t *menuSoundVoiceSetSlotDwords;
  uint32_t *settingsCopyDestDwordsA;
  uint32_t *playerNameDestDwords;
  uint32_t *settingsCopyDestDwordsB;
  bool callFailed;
  TextureSetResult textureSetResult;
  PaletteAssetResult paletteResult;
  TextResolveResult endpointTextResult;
  SampleVoiceSetResult voiceSetResult;
  PackageLoadResult romLoadResult;
  StatusResult statusResult;
  ArenaAllocResult allocResult;
  SoundPlayResult playResult;
  FrontendInitResult successResult;
  FrontendInitResult failureResult;
  ResourceLoadResult sampleLoadResult;
  WorldRuntimeContext *worldRuntime;
  FrontendModelPointerContextRuntimeState17C *pointerContext;
  typedef uint32_t FrontendModelPointerResolvedActionProc
          (uint32_t,uint32_t,uint32_t,int,struct ModelRuntimeNode *,struct FrontendModelPointerContextRuntimeState118 *);

  settingValue = PersistentSettings_ReadDword(0,0x30);
  g_TextureDownsampleShift = settingValue >> 1;
  playerBlock = g_FrontendPlayerRuntimeBlocks;
  remainingBlockCount = g_FrontendPlayerRuntimeBlockCount;
  pendingBlockCountOrRoleMask = g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_NETWORKED_MASK;
  while (pendingBlockCountOrRoleMask != SESSION_NETWORK_ROLE_LOCAL) {
    (playerBlock->factionAssignment).readyOrWaitState = 0;
    playerBlock->commandSyncPending = FRONTEND_COMMAND_SYNC_PENDING;
    playerBlock = playerBlock + 1;
    remainingBlockCount = remainingBlockCount - SESSION_NETWORK_ROLE_CLIENT;
    pendingBlockCountOrRoleMask = remainingBlockCount;
  }
  callFailed = (*g_GraphicsFramebufferBeginAccess)();
  if (!callFailed) {
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
  textureSetResult = (*g_GraphicsTextureSetLoadPackageCf)((uint16_t *)u_gfx_texturen_zentrale_gfx_00545acc);
  fillCursorOrResult = (FrontendRootResourceSlots5954 *)textureSetResult.textureSet;
  if (!textureSetResult.failed) {
    g_FrontendCentralTextureSet = (FrontendRootResourceSlots5954 *)textureSetResult.textureSet;
    paletteResult = (*g_GraphicsPaletteAssetLoadPackage)((uint16_t *)u_gfx_texturen_zentrale_pal_00545b00);
    fillCursorOrResult = (FrontendRootResourceSlots5954 *)paletteResult.paletteAsset;
    if (!paletteResult.failed) {
      g_FrontendCentralPaletteAsset = (FrontendRootResourceSlots5954 *)paletteResult.paletteAsset;
      endpointTextResult = TextResource_Resolve(0x2104);
      RichTextCommandStream_PatchPayloadBySelector(0,&g_FrontendNetworkEndpointTextUtf16,endpointTextResult.text)
      ;
      u_sound_menue01_sam_00545b54[0xb] = L'0';
      u_sound_menue01_sam_00545b54[0xc] = L'1';
      menuSoundVoiceSetSlotDwords = &g_FrontendMenuSoundVoiceSetLoadBaseEntry1;
      do {
        do {
          sampleLoadResult = Resource_Load((uint16_t *)u_sound_menue01_sam_00545b54);
          loadedSample = (SoundSampleAsset *)sampleLoadResult.bufferOrError;
          if (sampleLoadResult.failed) goto Frontend_Init_ContinueWithCentralRomAndRuntimeInitialization;
          voiceSetResult = (*g_SoundCreateSampleVoiceSet)(loadedSample);
          fillCursorOrResult = (FrontendRootResourceSlots5954 *)voiceSetResult.voiceSet;
          if (voiceSetResult.failed) {
            LOCK();
            UNLOCK();
            Resource_Release(loadedSample);
            goto Frontend_Init_ReturnInitializationFailure;
          }
          *menuSoundVoiceSetSlotDwords = (uint32_t)fillCursorOrResult;
          Resource_Release(loadedSample);
          u_sound_menue01_sam_00545b54[0xc] = u_sound_menue01_sam_00545b54[0xc] + L'\x01';
          menuSoundVoiceSetSlotDwords = menuSoundVoiceSetSlotDwords + 1;
        } while ((uint16_t)u_sound_menue01_sam_00545b54[0xc] < 0x3a);
        u_sound_menue01_sam_00545b54[0xb] = u_sound_menue01_sam_00545b54[0xb] + L'\x01';
        u_sound_menue01_sam_00545b54[0xc] = L'0';
      } while ((uint16_t)u_sound_menue01_sam_00545b54[0xb] < 0x3a);
Frontend_Init_ContinueWithCentralRomAndRuntimeInitialization:
      romLoadResult = Package_LoadEntry((uint16_t *)u_engine_zentrale_rom_00545aa4);
      fillCursorOrResult = romLoadResult.bufferOrError;
      if (!romLoadResult.failed) {
        g_FrontendCentralRomAsset = fillCursorOrResult;
        statusResult = RomAsset_PrepareRecords((RomAssetHeader *)fillCursorOrResult);
        fillCursorOrResult = (FrontendRootResourceSlots5954 *)statusResult.valueOrError;
        if (!statusResult.failed) {
          allocResult = (*g_MemoryApi.alloc)(0x10000);
          fillCursorOrResult = (FrontendRootResourceSlots5954 *)allocResult.payloadOrError;
          if (!allocResult.failed) {
            g_FrontendWorldObjectRecords = (WorldObjectRecord *)fillCursorOrResult;
            for (remainingDwords = 0x4000; remainingDwords != 0; remainingDwords = remainingDwords + -1) {
              fillCursorOrResult->opaqueGap0000_05DF[0] = 0;
              fillCursorOrResult->opaqueGap0000_05DF[1] = 0;
              fillCursorOrResult->opaqueGap0000_05DF[2] = 0;
              fillCursorOrResult->opaqueGap0000_05DF[3] = 0;
              fillCursorOrResult = (FrontendRootResourceSlots5954 *)(fillCursorOrResult->opaqueGap0000_05DF + 4);
            }
            allocResult = (*g_MemoryApi.alloc)(0x5954);
            frontendUiState = (FrontendRootResourceSlots5954 *)allocResult.payloadOrError;
            fillCursorOrResult = frontendUiState;
            if (!allocResult.failed) {
              worldRuntime = (WorldRuntimeContext *)(frontendUiState->opaqueGap0000_05DF + 0x368);
              frontendInitTemplateDwords = (uint32_t *)&g_FrontendRootInitializationTemplate;
              g_FrontendRootNode = frontendUiState;
              for (remainingDwords = 0x1655; remainingDwords != 0; remainingDwords = remainingDwords + -1) {
                *(uint32_t *)fillCursorOrResult->opaqueGap0000_05DF = *frontendInitTemplateDwords;
                frontendInitTemplateDwords = frontendInitTemplateDwords + 1;
                fillCursorOrResult = (FrontendRootResourceSlots5954 *)(fillCursorOrResult->opaqueGap0000_05DF + 4);
              }
              FrontendMenu_BindSharedResources(frontendUiState);
              UiRootStack_Push(&g_UiRootCallbacks_0053DA70,(UiRootNode *)frontendUiState);
              settingValue = PersistentSettings_ReadDword(3,0x20);
              musicBuffer = g_FrontendMusicActiveBuffer;
              if ((settingValue & 2) != 0) {
                sampleLoadResult = Resource_Load((uint16_t *)u_sound_music00_sam_00545c4e);
                loadedSample = (SoundSampleAsset *)sampleLoadResult.bufferOrError;
                musicBuffer = g_FrontendMusicActiveBuffer;
                if (!sampleLoadResult.failed) {
                  voiceSetResult = (*g_SoundCreateSampleVoiceSet)(loadedSample);
                  musicVoiceSet = voiceSetResult.voiceSet;
                  if (voiceSetResult.failed) {
                    Resource_Release(loadedSample);
                    musicBuffer = g_FrontendMusicActiveBuffer;
                  }
                  else {
                    g_FrontendMusicVoiceSet = musicVoiceSet;
                    Resource_Release(loadedSample);
                    settingValue = PersistentSettings_ReadDword(0x8000,0x2c);
                    playResult = (*g_SoundPlayLooping)(settingValue,settingValue,musicVoiceSet);
                    musicBuffer = playResult.soundBuffer;
                    if (playResult.failed) {
                      (*g_SoundReleaseSampleVoiceSet)(musicVoiceSet);
                      g_FrontendMusicVoiceSet = (DirectSoundVoiceSet *)0x0;
                      musicBuffer = g_FrontendMusicActiveBuffer;
                    }
                  }
                }
              }
              g_FrontendMusicActiveBuffer = musicBuffer;
              settingValue = g_NetworkBackendInstanceCount;
              nameSlotOrSourceDwords = g_FrontendTaskAssignmentControlOffsets.primaryAndPadding.offsets;
              if (g_NetworkBackendInstanceCount != 0) {
                backendDisplayName = g_NetworkBackendInstanceTable->displayNameUtf16;
                remainingBackends = g_NetworkBackendInstanceCount;
                do {
                  nameSlotOrSourceDwords = nameSlotOrSourceDwords + 1;
                  *nameSlotOrSourceDwords = (uint32_t)backendDisplayName;
                  backendDisplayName = backendDisplayName + 0x80;
                  remainingBackends = remainingBackends - 1;
                } while (remainingBackends != 0);
                UiPointerList_InitializeMeasuredTextRows
                          (settingValue,(void **)(g_FrontendTaskAssignmentControlOffsets.primaryAndPadding.
                                           offsets + 1),
                           (UiPointerListControl *)(frontendUiState->opaqueGap49E0_4AD3 + 0x90));
              }
              /* The 3D pointer-context control lives at root+0x368 (EBX = EDI+0x368 in the asm); the installed
                 handlers do not all match the generic callback field types, hence the casts. */
              pointerContext = (FrontendModelPointerContextRuntimeState17C *)worldRuntime;
              pointerContext->keyboardFallbackCf =
                   (bool (*)(UiKeyboardStateMask,UiActionId,struct UiRootNode *))
                   FrontendRuntime_DispatchCommandByCodeAndModifierFlagsCf;
              pointerContext->resolvedActionCallback104 =
                   (FrontendModelPointerResolvedActionProc *)FrontendRuntime_UpdatePointerContextAndSceneViewCf;
              pointerContext->resolvedActionCallback108 =
                   (FrontendModelPointerResolvedActionProc *)FrontendRuntime_UpdatePointerContextAndSceneViewCf;
              pointerContext->resolvedActionCallback10C =
                   (FrontendModelPointerResolvedActionProc *)FrontendRuntimeCallback5C_NoOp;
              pointerContext->resolvedActionCallback110 =
                   (FrontendModelPointerResolvedActionProc *)FrontendRuntimeCallback60_NoOp;
              pointerContext->resolvedActionCallback114 =
                   (FrontendModelPointerResolvedActionProc *)FrontendRuntimeCallback64_DispatchRecord1350;
              pointerContext->transientClearCallbackOrFrontendStateB0 = 0;
              pointerContext->rightReleaseCallback118 =
                   (void (*)(FrontendModelPointerContextRuntimeState17C *))FrontendRuntimeCallback68_DispatchRefresh1340;
              pointerContext->renderSpinLock = (RuntimeSpinLockValue *)&g_FrontendStateTickSpinLock;
              pointerContext->renderSpinLockReleaseCallback = Frontend_StateTick;
              RecentTextHistory_SortAndBuildPointerList
                        (5,(RecentTextHistoryPointerList *)
                           (frontendUiState->opaqueGap0000_05DF + 0x350));
              WorldRuntime_SetTerrainLightingConfiguration(0,0,0xffffffff,0,0,0,0,0,worldRuntime);
              WorldRuntime_AttachObjectArray(0x100,g_FrontendWorldObjectRecords,worldRuntime);
              callFailed = RomRuntime_BuildAllRegistryNodeTrees(worldRuntime);
              /* EAX when RomRuntime_BuildAllRegistryNodeTrees fails: its only failure source is
                 WorldObjectArray_AllocateFreeRecordCf's 0x14 (object array full), passed up unchanged through
                 RomRuntime_BuildNodeTreeRecursive. */
              fillCursorOrResult = (FrontendRootResourceSlots5954 *)0x14;
              if (!callFailed) {
                statusResult = FrontendRomTransition_ActivateRecordByIdCf(initialRomRecordId,worldRuntime)
                ;
                fillCursorOrResult = (FrontendRootResourceSlots5954 *)statusResult.valueOrError;
                if (!statusResult.failed) {
                  nameSlotOrSourceDwords = PersistentSettings_GetRegionOrFallback
                                     (0x28,g_FrontendLocalPlayerNameUtf16,0x60);
                  settingsCopySourceDwordsA = nameSlotOrSourceDwords;
                  settingsCopyDestDwordsA = (uint32_t *)frontendUiState->opaqueGap4EB4_4F2F;
                  for (remainingDwords = 10; remainingDwords != 0; remainingDwords = remainingDwords + -1) {
                    *settingsCopyDestDwordsA = *settingsCopySourceDwordsA;
                    settingsCopySourceDwordsA = settingsCopySourceDwordsA + 1;
                    settingsCopyDestDwordsA = settingsCopyDestDwordsA + 1;
                  }
                  playerNameDestDwords = (void *)g_FrontendLocalPlayerNameUtf16;
                  for (remainingDwords = 10; remainingDwords != 0; remainingDwords = remainingDwords + -1) {
                    *playerNameDestDwords = *nameSlotOrSourceDwords;
                    nameSlotOrSourceDwords = nameSlotOrSourceDwords + 1;
                    playerNameDestDwords = playerNameDestDwords + 1;
                  }
                  settingsCopySourceDwordsB =
                       PersistentSettings_GetRegionOrFallback
                                 (0x28,g_FrontendLocalPlayerNameUtf16,0x88);
                  settingsCopyDestDwordsB = (uint32_t *)frontendUiState->opaqueGap50C0_514B;
                  for (remainingDwords = 10; remainingDwords != 0; remainingDwords = remainingDwords + -1) {
                    *settingsCopyDestDwordsB = *settingsCopySourceDwordsB;
                    settingsCopySourceDwordsB = settingsCopySourceDwordsB + 1;
                    settingsCopyDestDwordsB = settingsCopyDestDwordsB + 1;
                  }
                  settingValue = PersistentSettings_ReadDword(4,0x3c);
                  *(uint32_t *)(frontendUiState->opaqueGap50C0_514B + 0x80) = settingValue;
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
                  successResult.failed = false;
                  successResult.frontendRootOrError = (uint32_t)frontendUiState;
                  return successResult;
                }
              }
            }
          }
        }
      }
    }
  }
Frontend_Init_ReturnInitializationFailure:
  failureResult.failed = true;
  failureResult.frontendRootOrError = (uint32_t)fillCursorOrResult;
  return failureResult;
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
  uint32_t unusedDispatchArg;
  uint32_t previousTickCounter;
  bool callResult;
  RecordRingDiscardResult discardedRecord;
  
  callResult = (*g_SpinLockTryAcquire)(&g_FrontendStateTickSpinLock);
  previousTickCounter = g_FrontendNetworkTickCounter;
  unusedDispatchArg = g_FrontendRootNode;
  if (callResult) {
    return;
  }
                    // WARNING: Switch is manually overridden
  switch(g_FrontendNetworkState) {
  case 0:
    if (g_FrontendTimerCountdownTicks != 0) goto FrontendStateTick_ReleaseLock;
    g_FrontendNetworkTickCounter = g_FrontendNetworkTickCounter + 1;
    g_FrontendTimerCountdownTicks = 4;
    break;
  case 1:
    if (g_FrontendTimerCountdownTicks == 0) {
      g_FrontendNetworkTickCounter = g_FrontendNetworkTickCounter + 1;
      g_FrontendTimerCountdownTicks = 4;
      if ((previousTickCounter & 0xf) != 0) {
        UiTransfer_SendPacketType10000Value2931Cf();
      }
      while( true ) {
        discardedRecord = UiRuntimeRecordRing_DiscardOldestCf();
        if (discardedRecord.empty) break;
        FrontendTransfer_HandleSessionListAndJoinAckPackets
                  ((UiTransferEndpointDescriptor *)discardedRecord.endpointOrReadIndex,
                   (FrontendTransferPacketUnion *)discardedRecord.payloadOrReadIndex,unusedDispatchArg);
      }
      FrontendDebugOverlay_RefreshCountersAndWorldCoordinates();
    }
    goto FrontendStateTick_ReleaseLock;
  case 2:
    if (g_FrontendTimerCountdownTicks == 0) {
      g_FrontendNetworkTickCounter = g_FrontendNetworkTickCounter + 1;
      g_FrontendTimerCountdownTicks = 4;
      FrontendTransfer_PublishHostSessionAndDispatchQueuedCommands(g_FrontendRootNode);
      while( true ) {
        discardedRecord = UiRuntimeRecordRing_DiscardOldestCf();
        if (discardedRecord.empty) break;
        FrontendTransfer_HandleLobbyDiscoveryAndPlayerPackets
                  ((UiTransferEndpointDescriptor *)discardedRecord.endpointOrReadIndex,
                   (FrontendTransferPacketUnion *)discardedRecord.payloadOrReadIndex,unusedDispatchArg);
      }
      FrontendDebugOverlay_RefreshCountersAndWorldCoordinates();
    }
    goto FrontendStateTick_ReleaseLock;
  case 3:
    if (g_FrontendTimerCountdownTicks == 0) {
      g_FrontendNetworkTickCounter = g_FrontendNetworkTickCounter + 1;
      g_FrontendTimerCountdownTicks = 4;
      if ((previousTickCounter & 0xf) != 0) {
        FrontendTransfer_SendPacket10006();
      }
      while( true ) {
        discardedRecord = UiRuntimeRecordRing_DiscardOldestCf();
        if (discardedRecord.empty) break;
        FrontendTransfer_HandleHostSessionAndCommandBatchPackets
                  ((UiTransferEndpointDescriptor *)discardedRecord.endpointOrReadIndex,
                   (FrontendTransferPacketUnion *)discardedRecord.payloadOrReadIndex,unusedDispatchArg);
      }
      FrontendDebugOverlay_RefreshCountersAndWorldCoordinates();
    }
    goto FrontendStateTick_ReleaseLock;
  case 4:
    if (g_FrontendTimerCountdownTicks != 0) goto FrontendStateTick_ReleaseLock;
    g_FrontendNetworkTickCounter = g_FrontendNetworkTickCounter + 1;
    g_FrontendTimerCountdownTicks = 4;
    while( true ) {
      discardedRecord = UiRuntimeRecordRing_DiscardOldestCf();
      if (discardedRecord.empty) break;
      FrontendNetwork_HandleHandshakeAndPlayerStatePackets
                ((UiTransferEndpointDescriptor *)discardedRecord.endpointOrReadIndex,
                 (FrontendTransferPacketUnion *)discardedRecord.payloadOrReadIndex,unusedDispatchArg);
    }
    callResult = FrontendNetwork_HostTickCommandAndSnapshotTransfer(unusedDispatchArg);
    if (callResult) {
      g_FrontendTimerCountdownTicks = 1;
      goto FrontendStateTick_ReleaseLock;
    }
    break;
  case 5:
    callResult = UiRuntimeRecordRing_ContainsIdCf(g_FrontendSessionToken);
    if (!callResult) goto FrontendStateTick_ReleaseLock;
    g_FrontendNetworkTickCounter = g_FrontendNetworkTickCounter + 1;
    do {
      discardedRecord = UiRuntimeRecordRing_DiscardOldestCf();
      if (discardedRecord.empty) break;
      callResult = FrontendTransfer_HandleGameplayCommandAndRosterPacketsCf
                        ((UiTransferEndpointDescriptor *)discardedRecord.endpointOrReadIndex,
                         (FrontendTransferPacketUnion *)discardedRecord.payloadOrReadIndex,
                         unusedDispatchArg);
    } while (!callResult);
    callResult = FrontendTransfer_ConsumeProcessedFlagFrontendCf();
    if (callResult) goto FrontendStateTick_ReleaseLock;
  }
  if ((g_FrontendRuntimeFlags & 0x10) == 0) {
    FrontendDebugOverlay_RefreshCountersAndWorldCoordinates();
  }
FrontendStateTick_ReleaseLock:
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
  DirectSoundVoiceSet *buttonVoiceSet;
  DirectSoundVoiceSet *buttonVoiceSet5;
  GraphicsTextureSourceAsset *menuTexture;
  int controlIndex;
  TextureSourceLoadResult textureLoadResult;
  
  textureLoadResult = (*g_GraphicsTextureSourceLoadPackageAsset)((uint16_t *)u_gfx_panel_menue_gfx_00545b78);
  menuTexture = textureLoadResult.textureSource;
  if (!textureLoadResult.failed) {
    g_FrontendMenuTextureSource = menuTexture;
    frontendUiState->menuTextureSource_485C = menuTexture;
    frontendUiState->menuTextureSource_4F30 = menuTexture;
    frontendUiState->menuTextureSource_53D8 = menuTexture;
    frontendUiState->menuTextureSource_5720 = menuTexture;
    frontendUiState->menuTextureSource_2670 = menuTexture;
    frontendUiState->menuTextureSource_2D0C = menuTexture;
    frontendUiState->menuTextureSource_3738 = menuTexture;
    frontendUiState->menuTextureSource_3E64 = menuTexture;
    frontendUiState->menuTextureSource_24F8 = menuTexture;
    frontendUiState->menuTextureSource_1C8C = menuTexture;
    frontendUiState->menuTextureSource_0AE4 = menuTexture;
    frontendUiState->menuTextureSource_05E0 = menuTexture;
    buttonVoiceSet = g_UiButtonSoundVoiceSets7[3];
    frontendUiState->buttonVoiceSet3_0644 = g_UiButtonSoundVoiceSets7[3];
    frontendUiState->buttonVoiceSet3_0764 = buttonVoiceSet;
    frontendUiState->buttonVoiceSet3_06A4 = buttonVoiceSet;
    frontendUiState->buttonVoiceSet3_0704 = buttonVoiceSet;
    frontendUiState->buttonVoiceSet3_0B48 = buttonVoiceSet;
    frontendUiState->buttonVoiceSet3_0BA8 = buttonVoiceSet;
    frontendUiState->buttonVoiceSet3_0C68 = buttonVoiceSet;
    frontendUiState->buttonVoiceSet3_1CF0 = buttonVoiceSet;
    frontendUiState->buttonVoiceSet3_1D50 = buttonVoiceSet;
    frontendUiState->buttonVoiceSet3_1DB0 = buttonVoiceSet;
    frontendUiState->buttonVoiceSet3_1E10 = buttonVoiceSet;
    frontendUiState->buttonVoiceSet3_1E70 = buttonVoiceSet;
    frontendUiState->buttonVoiceSet3_255C = buttonVoiceSet;
    frontendUiState->buttonVoiceSet3_25BC = buttonVoiceSet;
    frontendUiState->buttonVoiceSet3_26D4 = buttonVoiceSet;
    frontendUiState->buttonVoiceSet3_2790 = buttonVoiceSet;
    frontendUiState->buttonVoiceSet3_27F0 = buttonVoiceSet;
    frontendUiState->buttonVoiceSet3_2850 = buttonVoiceSet;
    frontendUiState->buttonVoiceSet3_2D70 = buttonVoiceSet;
    frontendUiState->buttonVoiceSet3_2DD0 = buttonVoiceSet;
    frontendUiState->buttonVoiceSet3_379C = buttonVoiceSet;
    frontendUiState->buttonVoiceSet3_3EC8 = buttonVoiceSet;
    frontendUiState->buttonVoiceSet3_491C = buttonVoiceSet;
    frontendUiState->buttonVoiceSet3_497C = buttonVoiceSet;
    frontendUiState->buttonVoiceSet3_49DC = buttonVoiceSet;
    frontendUiState->buttonVoiceSet3_4FF0 = buttonVoiceSet;
    frontendUiState->buttonVoiceSet3_5050 = buttonVoiceSet;
    frontendUiState->buttonVoiceSet3_5498 = buttonVoiceSet;
    frontendUiState->buttonVoiceSet3_54F8 = buttonVoiceSet;
    frontendUiState->buttonVoiceSet3_5558 = buttonVoiceSet;
    frontendUiState->buttonVoiceSet3_57E0 = buttonVoiceSet;
    buttonVoiceSet = g_UiButtonSoundVoiceSets7[4];
    frontendUiState->buttonVoiceSet4_2AE0 = g_UiButtonSoundVoiceSets7[4];
    frontendUiState->buttonVoiceSet4_2B40 = buttonVoiceSet;
    frontendUiState->buttonVoiceSet4_2BF4 = buttonVoiceSet;
    frontendUiState->buttonVoiceSet4_2C54 = buttonVoiceSet;
    frontendUiState->buttonVoiceSet4_2CB4 = buttonVoiceSet;
    frontendUiState->buttonVoiceSet4_2EE0 = buttonVoiceSet;
    frontendUiState->buttonVoiceSet4_2F48 = buttonVoiceSet;
    frontendUiState->buttonVoiceSet4_2FB0 = buttonVoiceSet;
    frontendUiState->buttonVoiceSet4_3018 = buttonVoiceSet;
    frontendUiState->buttonVoiceSet4_3080 = buttonVoiceSet;
    frontendUiState->buttonVoiceSet4_313C = buttonVoiceSet;
    frontendUiState->buttonVoiceSet4_31A4 = buttonVoiceSet;
    frontendUiState->buttonVoiceSet4_320C = buttonVoiceSet;
    frontendUiState->buttonVoiceSet4_3274 = buttonVoiceSet;
    frontendUiState->buttonVoiceSet4_32DC = buttonVoiceSet;
    frontendUiState->buttonVoiceSet4_3344 = buttonVoiceSet;
    frontendUiState->buttonVoiceSet4_33AC = buttonVoiceSet;
    frontendUiState->buttonVoiceSet4_3414 = buttonVoiceSet;
    frontendUiState->buttonVoiceSet4_347C = buttonVoiceSet;
    frontendUiState->buttonVoiceSet4_34E4 = buttonVoiceSet;
    frontendUiState->buttonVoiceSet4_35A0 = buttonVoiceSet;
    frontendUiState->buttonVoiceSet4_3608 = buttonVoiceSet;
    frontendUiState->buttonVoiceSet4_3670 = buttonVoiceSet;
    frontendUiState->buttonVoiceSet4_36D8 = buttonVoiceSet;
    frontendUiState->buttonVoiceSet4_3858 = buttonVoiceSet;
    frontendUiState->buttonVoiceSet4_390C = buttonVoiceSet;
    frontendUiState->buttonVoiceSet4_3974 = buttonVoiceSet;
    frontendUiState->buttonVoiceSet4_39DC = buttonVoiceSet;
    frontendUiState->buttonVoiceSet4_3A44 = buttonVoiceSet;
    frontendUiState->buttonVoiceSet4_3AAC = buttonVoiceSet;
    frontendUiState->buttonVoiceSet4_3B14 = buttonVoiceSet;
    frontendUiState->buttonVoiceSet4_3D4C = buttonVoiceSet;
    frontendUiState->buttonVoiceSet4_3DAC = buttonVoiceSet;
    frontendUiState->buttonVoiceSet4_3E0C = buttonVoiceSet;
    frontendUiState->buttonVoiceSet4_3F84 = buttonVoiceSet;
    frontendUiState->buttonVoiceSet4_3FE4 = buttonVoiceSet;
    frontendUiState->buttonVoiceSet4_4044 = buttonVoiceSet;
    frontendUiState->buttonVoiceSet4_28B0 = buttonVoiceSet;
    controlIndex = 7;
    do {
      *(DirectSoundVoiceSet **)
       (frontendUiState->opaqueGap0000_05DF +
       g_FrontendTaskAssignmentControlOffsets.factionControls.offsets[controlIndex] + 0x5c) = buttonVoiceSet;
      *(DirectSoundVoiceSet **)
       (frontendUiState->opaqueGap0000_05DF +
       g_FrontendTaskAssignmentControlOffsets.playerControls.offsets[controlIndex] + 0x5c) = buttonVoiceSet;
      *(DirectSoundVoiceSet **)
       (frontendUiState->opaqueGap0000_05DF +
       g_FrontendTaskAssignmentControlOffsets.selectionRows.offsets[controlIndex] + 0x5c) = buttonVoiceSet;
      buttonVoiceSet5 = g_UiButtonSoundVoiceSets7[5];
      controlIndex = controlIndex + -1;
    } while (controlIndex != 0);
    frontendUiState->buttonVoiceSet5_3C98 = g_UiButtonSoundVoiceSets7[5];
    frontendUiState->buttonVoiceSet5_41C0 = buttonVoiceSet5;
    frontendUiState->buttonVoiceSet5_433C = buttonVoiceSet5;
    frontendUiState->buttonVoiceSet5_44B8 = buttonVoiceSet5;
    frontendUiState->buttonVoiceSet5_4634 = buttonVoiceSet5;
    frontendUiState->buttonVoiceSet5_514C = buttonVoiceSet5;
    frontendUiState->buttonVoiceSet5_5210 = buttonVoiceSet5;
    frontendUiState->buttonVoiceSet5_0A8C = buttonVoiceSet5;
    buttonVoiceSet = g_UiButtonSoundVoiceSets7[6];
    frontendUiState->buttonVoiceSet6_2024 = g_UiButtonSoundVoiceSets7[6];
    frontendUiState->buttonVoiceSet6_21EC = buttonVoiceSet;
    frontendUiState->buttonVoiceSet6_23CC = buttonVoiceSet;
    frontendUiState->buttonVoiceSet6_4AD4 = buttonVoiceSet;
    frontendUiState->buttonVoiceSet6_4BD0 = buttonVoiceSet;
    frontendUiState->buttonVoiceSet6_5654 = buttonVoiceSet;
    frontendUiState->buttonVoiceSet6_4DC4 = buttonVoiceSet;
    frontendUiState->buttonVoiceSet6_4EB0 = buttonVoiceSet;
    frontendUiState->buttonVoiceSet6_50BC = buttonVoiceSet;
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
          (FrontendIndexedSelectionArgument argument1,uint32_t argument2,uint32_t argument3,
          FrontendFactionAssignmentIndex selectionIndex)

{
  int *levelCycleCounterField;
  LevelPlayerSlotByteOffset32 playerSlotOffset;
  FrontendLoadedLevelRuntimeImage370 *loadedLevelAsset;
  uint32_t nextSelectionTextId;
  uint32_t playerRecordsRemaining;
  FrontendPlayerRuntimeRecord *playerRecordCursor;
  int selectionControlAddress;
  int factionAssetRecordAddress;
  int selectionTextCycleLength;
  int *selectionCycleCounterField;
  
  loadedLevelAsset = g_FrontendLoadedLevelAsset;
  selectionTextCycleLength = 7;
  playerRecordsRemaining = g_FrontendPlayerRuntimeBlockCount;
  playerRecordCursor = g_FrontendPlayerRuntimeBlocks;
  do {
    if (argument1 == playerRecordCursor->playerRuntimeId) {
      if ((playerRecordCursor->runtimeState64 & 1) != 0) {
        selectionTextCycleLength = 8;
      }
      playerSlotOffset = g_InGameLevelRuntimeGlobalBlock.playerSlotByteOffsets[selectionIndex];
      selectionControlAddress =
           g_FrontendRootNode +
           g_FrontendTaskAssignmentControlOffsets.factionControls.offsets[selectionIndex + 1];
      nextSelectionTextId = *(int *)(selectionControlAddress + 0x54) + 1;
      selectionCycleCounterField =
           (int *)((int)&g_FrontendLoadedLevelAsset->playerSlots[0].aiClassOrMode + playerSlotOffset);
      *selectionCycleCounterField = *selectionCycleCounterField + 1;
      if (selectionTextCycleLength + 0x2174U <= nextSelectionTextId) {
        nextSelectionTextId = 0x2174;
        levelCycleCounterField = (int *)((int)&loadedLevelAsset->playerSlots[0].aiClassOrMode + playerSlotOffset);
        *levelCycleCounterField = *levelCycleCounterField - selectionTextCycleLength;
      }
      *(uint32_t *)(selectionControlAddress + 0x54) = nextSelectionTextId;
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
          (uint32_t argument1,uint32_t argument2,uint32_t argument3,
          FrontendFactionAssignmentIndex selectionIndex)

{
  FactionRuntimeLifecycleObservedState *lifecycleState;
  FrontendPlayerRuntimeBlockCount remainingPlayerBlocks;
  FrontendPlayerRuntimeRecord *playerBlock;
  
  remainingPlayerBlocks = g_FrontendPlayerRuntimeBlockCount;
  playerBlock = g_FrontendPlayerRuntimeBlocks;
  do {
    if (selectionIndex + 1 == (playerBlock->factionAssignment).factionAssignmentIndex) {
      return;
    }
    playerBlock = playerBlock + 1;
    remainingPlayerBlocks = remainingPlayerBlocks - 1;
  } while (remainingPlayerBlocks != 0);
  lifecycleState = g_GameFactionRuntimeImage.tail.factionLifecycleStates + selectionIndex + 1;
  *lifecycleState = *lifecycleState ^ FACTION_RUNTIME_LIFECYCLE_ACTIVE;
  FrontendTaskAssignmentPage_RefreshFactionAndPlayerControls(g_FrontendRootNode);
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
          (FrontendIndexedSelectionArgument argument1,uint32_t argument2,uint32_t argument3,
          FrontendFactionAssignmentIndex selectionIndex)

{
  SessionNetworkRoleFlags remainingBlockCount;
  SessionNetworkRoleFlags pendingBlockCountOrRoleMask;
  uint32_t readyStateGeneration;
  SessionNetworkRoleFlags generationCursor;
  UiNodeBase *selectedControl;
  FrontendPlayerRuntimeRecord *playerBlockCursor;
  FrontendPlayerRuntimeRecord *matchedPlayerBlock;
  
  selectedControl =
       THANDOR_UI_AT(g_FrontendRootNode,
                     g_FrontendTaskAssignmentControlOffsets.selectionRows.offsets[selectionIndex + 1]);
  generationCursor = 7;
  if ((((UiSelectableControl *)selectedControl)->stateFlags & 0x400) == 0) {
    if (argument1 == g_LocalPlayerRuntimeId) {
      do {
        generationCursor = generationCursor - SESSION_NETWORK_ROLE_CLIENT;
      } while (generationCursor != SESSION_NETWORK_ROLE_LOCAL);
      UiSelectableGroup_SelectExclusive(7,selectedControl,
          THANDOR_UI_AT(g_FrontendRootNode,
                        g_FrontendTaskAssignmentControlOffsets.selectionRows.offsets[1]),
          THANDOR_UI_AT(g_FrontendRootNode,
                        g_FrontendTaskAssignmentControlOffsets.selectionRows.offsets[2]),
          THANDOR_UI_AT(g_FrontendRootNode,
                        g_FrontendTaskAssignmentControlOffsets.selectionRows.offsets[3]),
          THANDOR_UI_AT(g_FrontendRootNode,
                        g_FrontendTaskAssignmentControlOffsets.selectionRows.offsets[4]),
          THANDOR_UI_AT(g_FrontendRootNode,
                        g_FrontendTaskAssignmentControlOffsets.selectionRows.offsets[5]),
          THANDOR_UI_AT(g_FrontendRootNode,
                        g_FrontendTaskAssignmentControlOffsets.selectionRows.offsets[6]),
          THANDOR_UI_AT(g_FrontendRootNode,
                        g_FrontendTaskAssignmentControlOffsets.selectionRows.offsets[7]));
      generationCursor = SESSION_NETWORK_ROLE_LOCAL;
    }
    readyStateGeneration = g_FrontendFactionAssignmentReadyStateGeneration;
    remainingBlockCount = g_FrontendPlayerRuntimeBlockCount;
    pendingBlockCountOrRoleMask = g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_NETWORKED_MASK;
    for (playerBlockCursor = g_FrontendPlayerRuntimeBlocks;
        (matchedPlayerBlock = g_FrontendPlayerRuntimeBlocks, pendingBlockCountOrRoleMask != SESSION_NETWORK_ROLE_LOCAL &&
        (generationCursor = remainingBlockCount, matchedPlayerBlock = playerBlockCursor, argument1 != playerBlockCursor->playerRuntimeId));
        playerBlockCursor = playerBlockCursor + 1) {
      generationCursor = remainingBlockCount - SESSION_NETWORK_ROLE_CLIENT;
      remainingBlockCount = generationCursor;
      pendingBlockCountOrRoleMask = generationCursor;
    }
    (matchedPlayerBlock->factionAssignment).factionAssignmentIndex = selectionIndex + 1;
    (matchedPlayerBlock->factionAssignment).readyOrWaitState = readyStateGeneration;
    g_FrontendFactionAssignmentReadyStateGeneration =
         g_FrontendFactionAssignmentReadyStateGeneration + 1;
    FrontendTaskAssignmentPage_RefreshFactionAndPlayerControls(g_FrontendRootNode);
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
  uint32_t freeArenaBytes;
  WideNumberDenominator32 denominator;
  WorldRuntimeContext *world;
  WorldVector0EaxEcxEdx12 worldVector0;
  WorldVector1EaxEcxEdx12 worldVector1;
  
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
  world = (WorldRuntimeContext *)FRONTEND_UI(g_FrontendRootNode,menuRoomModelView);
  worldVector0 = WorldRuntime_GetVector0Regs(world);
  WideNumber_FormatUtf16
            (WIDE_FORMAT_GROUP_THOUSANDS|WIDE_FORMAT_WRITE_TERMINATOR|WIDE_FORMAT_SIGNED_VALUE,0,10,
             1,worldVector0.xQ12,g_FrontendDebugOverlayTextSlot04Utf16);
  WideNumber_FormatUtf16
            (WIDE_FORMAT_GROUP_THOUSANDS|WIDE_FORMAT_WRITE_TERMINATOR|WIDE_FORMAT_SIGNED_VALUE,0,10,
             1,worldVector0.yQ12,g_FrontendDebugOverlayTextSlot05Utf16);
  WideNumber_FormatUtf16
            (WIDE_FORMAT_GROUP_THOUSANDS|WIDE_FORMAT_WRITE_TERMINATOR|WIDE_FORMAT_SIGNED_VALUE,0,10,
             1,worldVector0.zQ12,g_FrontendDebugOverlayTextSlot06Utf16);
  worldVector1 = WorldRuntime_GetVector1Regs(world);
  WideNumber_FormatUtf16
            (WIDE_FORMAT_GROUP_THOUSANDS|WIDE_FORMAT_WRITE_TERMINATOR|WIDE_FORMAT_SIGNED_VALUE,0,10,
             1,worldVector1.magnitudeQ12,g_FrontendDebugOverlayTextSlot07Utf16);
  WideNumber_FormatUtf16
            (WIDE_FORMAT_GROUP_THOUSANDS|WIDE_FORMAT_WRITE_TERMINATOR|WIDE_FORMAT_SIGNED_VALUE,0,10,
             1,worldVector1.headingAngle,g_FrontendDebugOverlayTextSlot08Utf16);
  WideNumber_FormatUtf16
            (WIDE_FORMAT_GROUP_THOUSANDS|WIDE_FORMAT_WRITE_TERMINATOR|WIDE_FORMAT_SIGNED_VALUE,0,10,
             1,worldVector1.pitchAngle,g_FrontendDebugOverlayTextSlot09Utf16);
  WideNumber_FormatUtf16
            (WIDE_FORMAT_WRITE_TERMINATOR|WIDE_FORMAT_SIGNED_VALUE,0,10,1,g_CursorOverrideX,
             g_FrontendDebugOverlayTextSlot10Utf16);
  WideNumber_FormatUtf16
            (WIDE_FORMAT_WRITE_TERMINATOR|WIDE_FORMAT_SIGNED_VALUE,0,10,1,g_CursorOverrideY,
             g_FrontendDebugOverlayTextSlot11Utf16);
  freeArenaBytes = (*g_MemoryApi.queryFreeBytes)();
  WideNumber_FormatUtf16
            (WIDE_FORMAT_WRITE_TERMINATOR|WIDE_FORMAT_HEXADECIMAL,0,10,1,freeArenaBytes,
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
  uint32_t *voiceSetCursor;

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
uint64_t FrontendModelPointerContext_FindBestEligibleModelHitTarget
                (int pointerY,int pointerX,FrontendModelPointerContextRuntimeState118 *context)

{
  ModelRuntimeNode *modelNode;
  uint32_t bestHitMetric;
  ModelRuntimeNode *bestModelNode;
  StatusResult hitTestResult;
  int candidatePriority;
  int bestPriority;

  bestModelNode = (ModelRuntimeNode *)0x0;
  bestHitMetric = 0x7fffffff;
  for (modelNode = context->candidateModelListHead; modelNode != (ModelRuntimeNode *)0x0;
      modelNode = (ModelRuntimeNode *)(modelNode->common).nextNode) {
    if ((((modelNode->runtimeFlags & 2) != 0) && (modelNode->ownerClassId == WORLD_OWNER_RUNTIME_MODEL)
        ) && (((context->contextFlags &
               FRONTEND_MODEL_POINTER_CONTEXT_ALLOW_MODEL_WITHOUT_RUNTIME_FLAG_20) != 0 ||
              ((modelNode->runtimeFlags & 0x20) != 0)))) {
      hitTestResult = ModelRuntimeNode_HitTestProjectedBoundsAndChildrenCf
                        (pointerY,pointerX,modelNode,context);
      if (hitTestResult.failed) continue;
      if ((context->contextFlags & FRONTEND_MODEL_POINTER_CONTEXT_COMPARE_HITS_BY_METRIC_ONLY) != 0) {
        if ((int)bestHitMetric <= (int)hitTestResult.valueOrError) continue;
      }
      else if (bestModelNode != (ModelRuntimeNode *)0x0) {
        /* Higher model-class priority wins; equal priority falls back to the smaller hit metric. */
        candidatePriority =
             (int)(&g_RuntimeModelClassPriorityByModelClassId.modelClass00Priority)
                  [*(int *)((((modelNode->runtimePayload).modelRuntime)->definitionOrSavedId).
                            savedIdOrOffset + 0x4c)];
        bestPriority =
             (int)(&g_RuntimeModelClassPriorityByModelClassId.modelClass00Priority)
                  [*(int *)((((bestModelNode->runtimePayload).modelRuntime)->definitionOrSavedId).
                            savedIdOrOffset + 0x4c)];
        if (candidatePriority < bestPriority) continue;
        if ((candidatePriority == bestPriority) && ((int)bestHitMetric <= (int)hitTestResult.valueOrError))
        continue;
      }
      bestHitMetric = hitTestResult.valueOrError;
      bestModelNode = modelNode;
    }
  }
  /* EDX:EAX = best node : its hit metric. */
  return ((uint64_t)(uintptr_t)bestModelNode << 32) | (uint64_t)bestHitMetric;
}


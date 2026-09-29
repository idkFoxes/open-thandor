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
   The frontend (main menu) state machine, run from Game_Run until the player quits. It builds the menu at
   frontendEntryRecordId, jumps straight into the host/client/map flow when -HOST, -CLIENT= or -KARTE= is on the
   command line, then presents one UI frame per iteration and performs g_FrontendPendingPageAction
   (FRONTEND_PAGE_ACTION_*): open a menu page, wait for the network peers (scenario catalogue, task assignment,
   level transfer), or tear the frontend down to run a session. After a session a campaign continues with the
   successor level chosen by the end movie (menu rebuilt at FRONTEND_ROM_RECORD_MISSION_BRIEFING), otherwise
   the menu is rebuilt at the scenario selection or the entry record. Returns CF clear when the UI root stack
   empties (quit), CF set with the error when Frontend_Init fails.
*/
FrontendMainLoopResult Frontend_MainLoop(RomRecordId frontendEntryRecordId)

{
  FrontendRoleStateFlags *roleStateFlagsPtr;
  AssetAllocationSizeBytes fieldGridAllocationSize;
  FrontendSnapshotTransferFlags receivedTransferFlags;
  SessionNetworkRoleFlags pendingBlockCountOrRoleMask;
  ScenarioCatalogHeader *scenarioCatalog;
  uint32_t errorOrByteCount;
  FieldGridAsset *sourceGrid;
  RomRecordId nextRomRecordId;
  int countOrLevelIdOrNextAction;
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
  errorOrByteCount = initResult.frontendRootOrError;
  if (!initResult.failed) {
    /* -HOST and -CLIENT= activate entry 3 of the entry menu's action table, -KARTE= (map) entry 0, without the
       click sound, and let the started camera transition end at once. */
    commandLineOption = g_CommandLineFindOption(5,s_SPIELER__SPIEL__NETZWERK__HOST_00545e72 + 0x1a); /* "HOST" */
    if (commandLineOption.notFound) {
      commandLineOption = g_CommandLineFindOption(8,s_NAME__CLIENT__KARTE___00545e91 + 6); /* "CLIENT=" */
      if (commandLineOption.notFound) {
        commandLineOption = g_CommandLineFindOption(7,s_NAME__CLIENT__KARTE___00545e91 + 0xe); /* "KARTE=" */
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
nextFrame:
    do {
      for (;;) {
        for (;;) {
          for (;;) {
            if (g_UiRootNode != UI_ROOT_STACK_END) {
              FrontendRomTransition_ProcessPendingRecord();
            }
            UiRootStack_InvalidateAll();
            UiFrame_ProcessAndPresent();
            g_FrontendPendingPageActionDepth = 0;
            if (g_FrontendPendingPageAction != FRONTEND_PAGE_ACTION_NONE) break;
            if (g_UiRootNode == UI_ROOT_STACK_END) {
              /* the last UI root was popped: the player quit the game */
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
          g_FrontendPendingPageActionDepth++;
          if (g_FrontendPendingPageAction != FRONTEND_PAGE_ACTION_NETWORK_SETUP_PAGE) break;
          FrontendNetworkSetupPage_InitializeBackendMode((FrontendUiImage *)g_FrontendRootNode);
          g_FrontendPendingPageAction = FRONTEND_PAGE_ACTION_NONE;
        }
        if (g_FrontendPendingPageAction != FRONTEND_PAGE_ACTION_GAMEPLAY_SETTINGS_PAGE) break;
        FrontendGameplaySettingsPage_InitializeFromPersistentSettings
                  ((UiRootNode *)g_FrontendRootNode);
        g_FrontendPendingPageAction = FRONTEND_PAGE_ACTION_NONE;
      }
      if (g_FrontendPendingPageAction == FRONTEND_PAGE_ACTION_SCENARIO_SELECTION_PAGE) {
        /* In a network session first wait until every player's snapshot is published; a client takes the
           snapshot table the host sends meanwhile. Then wait for the scenario catalogue exchange (the host
           sends its catalogue, a client receives it) before the page opens. */
        playerBlock = g_FrontendPlayerRuntimeBlocks;
        remainingBlockCount = g_FrontendPlayerRuntimeBlockCount;
        pendingBlockCountOrRoleMask = g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_NETWORKED_MASK;
        while (remainingPlayerBlocks = g_FrontendPlayerRuntimeBlockCount, roleScanBlock = g_FrontendPlayerRuntimeBlocks,
              pendingBlockCountOrRoleMask != SESSION_NETWORK_ROLE_LOCAL) {
          if ((playerBlock->snapshotTransferFlags & FRONTEND_SNAPSHOT_HOST_PUBLICATION_READY) == 0) {
            if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_CLIENT) !=
                SESSION_NETWORK_ROLE_LOCAL) {
              receivedBuffer = UiTransferMailbox_GetReceivedBuffer();
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
                  receivedFlagsCursor++;
                  if ((receivedTransferFlags & FRONTEND_SNAPSHOT_PAYLOAD_COMPLETE) != 0) {
                    encodeCursorOrSize = playerBlock->snapshotPayload;
                    for (countOrLevelIdOrNextAction = FRONTEND_SNAPSHOT_PAYLOAD_BYTES / sizeof(uint32_t);
                         countOrLevelIdOrNextAction != 0; countOrLevelIdOrNextAction--) {
                      *(FrontendSnapshotTransferFlags *)encodeCursorOrSize = *receivedFlagsCursor;
                      receivedFlagsCursor++;
                      encodeCursorOrSize = encodeCursorOrSize + 4;
                    }
                  }
                  playerBlock++;
                  remainingPlayerBlocks--;
                } while (remainingPlayerBlocks != 0);
                FrontendCommandQueue_EnqueueLocalPlayerCommand(FRONTEND_COMMAND_SNAPSHOTS_RECEIVED,0,0,0);
                UiTransferMailbox_ClearReceivedState();
              }
            }
            goto nextFrame;
          }
          playerBlock++;
          remainingBlockCount--;
          pendingBlockCountOrRoleMask = remainingBlockCount;
        }
        do {
          /* the AND really clears every other progress bit of the player (AND [ESI+0x60],1 in the asm) */
          roleStateFlagsPtr = &roleScanBlock->factionAssignment.roleStateFlags;
          *roleStateFlagsPtr = *roleStateFlagsPtr & FRONTEND_PLAYER_STATE_SCENARIO_CATALOG;
          playerBlock = g_FrontendPlayerRuntimeBlocks;
          if (*roleStateFlagsPtr == 0) {
            FrontendScenarioTransfer_ProcessReceivedAsset();
            if ((playerBlock->factionAssignment.roleStateFlags & FRONTEND_PLAYER_STATE_SCENARIO_CATALOG) == 0) {
              roleStateFlagsPtr = &playerBlock->factionAssignment.roleStateFlags;
              *roleStateFlagsPtr = *roleStateFlagsPtr | FRONTEND_PLAYER_STATE_SCENARIO_CATALOG;
              ScenarioCatalog_Rebuild();
              errorOrByteCount = g_ScenarioCatalogUsedBytes;
              scenarioCatalog = g_ScenarioCatalog;
              if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_CLIENT) ==
                  SESSION_NETWORK_ROLE_LOCAL) {
                if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_HOST) !=
                    SESSION_NETWORK_ROLE_LOCAL) {
                  /* Host: compress the catalogue into its own buffer right behind the used bytes, prefixed with
                     the uncompressed size, and offer it to the clients. */
                  encodeCursorOrSize = (uint8_t *)g_ScenarioCatalog + g_ScenarioCatalogUsedBytes + sizeof(uint32_t);
                  destinationCapacityBytes = 0x2fffc - g_ScenarioCatalogUsedBytes;
                  *(uint32_t *)(encodeCursorOrSize - 4) = g_ScenarioCatalogUsedBytes;
                  encodeResult = PckCodec_EncodeHuffmanRle
                                     (destinationCapacityBytes,encodeCursorOrSize,errorOrByteCount,(uint8_t *)scenarioCatalog);
                  checkedResult = FatalError_ExitIfFailed(encodeResult.byteCountOrError,encodeResult.failed);
                  UiTransferMailbox_SetOutgoingBuffer(checkedResult.valueOrError + 4,encodeCursorOrSize - 4);
                }
              }
              else {
                UiTransferMailbox_MarkUnavailable();
                g_FrontendScenarioTransferState = 1;
              }
            }
            goto nextFrame;
          }
          roleScanBlock++;
          remainingPlayerBlocks--;
        } while (remainingPlayerBlocks != 0);
        if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_HOST) != SESSION_NETWORK_ROLE_LOCAL) {
          UiTransferMailbox_SetOutgoingBuffer(0,NULL);
        }
        FrontendScenarioSelectionPage_InitializeAndApplyMapOption
                  ((FrontendScenarioSelectionPageView26C4 *)g_FrontendRootNode);
        g_FrontendPendingPageAction = FRONTEND_PAGE_ACTION_NONE;
        goto nextFrame;
      }
      if (g_FrontendPendingPageAction == FRONTEND_PAGE_ACTION_TASK_ASSIGNMENT_PAGE) {
        FrontendScenarioTransfer_ProcessReceivedAsset();
        remainingPlayerBlocks = g_FrontendPlayerRuntimeBlockCount;
        playerBlock = g_FrontendPlayerRuntimeBlocks;
        do {
          if ((playerBlock->factionAssignment.roleStateFlags & FRONTEND_PLAYER_STATE_TASK_ASSIGNMENT) == 0)
          goto nextFrame;
          playerBlock++;
          remainingPlayerBlocks--;
        } while (remainingPlayerBlocks != 0);
        if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_HOST) != SESSION_NETWORK_ROLE_LOCAL) {
          g_MemoryApi.free(g_UiTransferMailbox.outgoingAllocation);
          UiTransferMailbox_SetOutgoingBuffer(0,NULL);
        }
        FrontendTaskAssignmentPage_Initialize
                  ((FrontendTaskAssignmentPageInitView *)g_FrontendRootNode);
        g_FrontendPendingPageAction = FRONTEND_PAGE_ACTION_NONE;
      }
      else if (g_FrontendPendingPageAction == FRONTEND_PAGE_ACTION_MISSION_BRIEFING_PAGE) {
        FrontendScenarioTransfer_ProcessReceivedAsset();
        remainingPlayerBlocks = g_FrontendPlayerRuntimeBlockCount;
        playerBlock = g_FrontendPlayerRuntimeBlocks;
        do {
          if ((playerBlock->factionAssignment.roleStateFlags & FRONTEND_PLAYER_STATE_LEVEL_READY_MASK) == 0)
          goto nextFrame;
          playerBlock++;
          remainingPlayerBlocks--;
        } while (remainingPlayerBlocks != 0);
        if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_HOST) != SESSION_NETWORK_ROLE_LOCAL) {
          g_MemoryApi.free(g_UiTransferMailbox.outgoingAllocation);
          UiTransferMailbox_SetOutgoingBuffer(0,NULL);
        }
        FrontendMissionBriefingPage_Initialize((UiRootNode *)g_FrontendRootNode);
        g_FrontendPendingPageAction = FRONTEND_PAGE_ACTION_NONE;
      }
      else if (g_FrontendPendingPageAction == FRONTEND_PAGE_ACTION_CREDITS) {
        CreditsScreen_Open((FrontendCreditsUiStateView *)g_FrontendRootNode);
        g_FrontendPendingPageAction = FRONTEND_PAGE_ACTION_NONE;
      }
      else {
        if (g_FrontendPendingPageAction != FRONTEND_PAGE_ACTION_QUIT_CONFIRM_PAGE) {
          /* every remaining action leaves the menu: tear the frontend down first */
          FrontendRuntime_ShutdownAndReleaseResourcesRegs();
          if (g_FrontendPendingPageAction == FRONTEND_PAGE_ACTION_START_SESSION) {
            PersistentSettings_Flush();
            sessionRunResult = InGameRuntime_RunSessionUntilExit
                               ((LevelAssetRuntimeImagePrefix370 *)g_FrontendLoadedLevelAsset,0,
                                (uint16_t *)&g_FrontendScenarioPathScratchUtf16);
            FatalError_ExitIfFailed(sessionRunResult.exitCodeOrError,sessionRunResult.failed);
            PersistentSettings_Flush();
            UiFrame_FlushInputAndResetPendingTicks();
            g_FrontendScenarioInitializationCount = 0;
            remainingPlayerBlocks = g_FrontendPlayerRuntimeBlockCount;
            playerBlock = g_FrontendPlayerRuntimeBlocks;
            do {
              playerBlock->factionAssignment.roleStateFlags = 0;
              playerBlock++;
              remainingPlayerBlocks--;
              campaignRecordCursor = g_FrontendLoadedCampaignAsset;
            } while (remainingPlayerBlocks != 0);
advanceCampaign:
            /* Campaign asset (CampaignAsset): the record cursors start at the asset base and advance by one
               CampaignLevelRecord, so level record i is ((CampaignAsset *)cursor)->levels[0]. */
            g_FrontendLoadedCampaignAsset = campaignRecordCursor;
            if (campaignRecordCursor != NULL) {
              countOrLevelIdOrNextAction = ((CampaignAsset *)campaignRecordCursor)->levelRecordCount;
              levelRecordCursor = campaignRecordCursor;
              do {
                if (((CampaignAsset *)campaignRecordCursor)->currentLevelId == ((CampaignAsset *)levelRecordCursor)->levels[0].levelId) {
                  /* Current level found: follow the successor chosen by the end movie selection. */
                  countOrLevelIdOrNextAction = ((CampaignAsset *)levelRecordCursor)->levels[0].successorLevelIds[(int)g_EndMovieSelectionIndex];
                  if (-1 < countOrLevelIdOrNextAction) {
                    remainingLevelRecords = ((CampaignAsset *)campaignRecordCursor)->levelRecordCount;
                    ((CampaignAsset *)campaignRecordCursor)->currentLevelId = countOrLevelIdOrNextAction;
                    for (;;) {
                      if (countOrLevelIdOrNextAction == ((CampaignAsset *)campaignRecordCursor)->levels[0].levelId) {
                        WidePath_CombineDirectoryAndLeaf
                                  ((uint16_t *)&g_FrontendScenarioPathScratchUtf16,
                                   ((CampaignAsset *)campaignRecordCursor)->levels[0].levelFileName,(uint16_t *)u_level_0050daac);
                        WidePath_SetExtensionCode(0x76656c,(uint16_t *)&g_FrontendScenarioPathScratchUtf16); /* "lev" */
                        goto loadSelectedLevel;
                      }
                      campaignRecordCursor = (void *)((CampaignLevelRecord *)campaignRecordCursor + 1);
                      remainingLevelRecords--;
                      if (remainingLevelRecords == 0) break;
                    }
                    /* Successor level missing: the campaign is finished. */
                    Resource_Release(g_FrontendLoadedCampaignAsset);
                    g_FrontendLoadedCampaignAsset = NULL;
                    g_FrontendScenarioInitializationCount = 0;
                  }
                  break;
                }
                levelRecordCursor = (void *)((CampaignLevelRecord *)levelRecordCursor + 1);
                countOrLevelIdOrNextAction--;
              } while (countOrLevelIdOrNextAction != 0);
            }
            goto noNextLevel;
          }
          if (g_FrontendPendingPageAction == FRONTEND_PAGE_ACTION_RESUME_SAVED_SESSION) {
            /* unlike FRONTEND_PAGE_ACTION_START_SESSION the settings are not flushed before the session */
            sessionRunResult = InGameRuntime_RunSessionUntilExit
                               ((LevelAssetRuntimeImagePrefix370 *)g_FrontendLoadedLevelAsset,1,
                                (uint16_t *)&g_FrontendScenarioPathScratchUtf16);
            FatalError_ExitIfFailed(sessionRunResult.exitCodeOrError,sessionRunResult.failed);
            PersistentSettings_Flush();
            UiFrame_FlushInputAndResetPendingTicks();
            g_FrontendScenarioInitializationCount = 0;
            remainingPlayerBlocks = g_FrontendPlayerRuntimeBlockCount;
            playerBlock = g_FrontendPlayerRuntimeBlocks;
            do {
              playerBlock->factionAssignment.roleStateFlags = 0;
              playerBlock++;
              remainingPlayerBlocks--;
              campaignRecordCursor = g_FrontendLoadedCampaignAsset;
            } while (remainingPlayerBlocks != 0);
            goto advanceCampaign;
          }
          /* any other action: rebuild the menu at the entry record */
          countOrLevelIdOrNextAction = FRONTEND_PAGE_ACTION_NONE;
          nextRomRecordId = frontendEntryRecordId;
          goto rebuildMenu;
        }
        FrontendSession_ShowQuitConfirmPage((FrontendUiImage *)g_FrontendRootNode);
        g_FrontendPendingPageAction = FRONTEND_PAGE_ACTION_NONE;
      }
    } while (true);
  }
initFailed:
  FrontendRuntime_ShutdownAndReleaseResourcesRegs();
  failureResult.failed = true;
  failureResult.errorOrValue = errorOrByteCount;
  return failureResult;
noNextLevel:
  /* No level to continue with (no campaign, or it ended): back to the scenario selection. */
  if (g_FrontendScenarioPathScratchUtf16 == 0) {
    nextRomRecordId = FRONTEND_ROM_RECORD_SCENARIO_SELECTION;
    countOrLevelIdOrNextAction = FRONTEND_PAGE_ACTION_SCENARIO_SELECTION_PAGE;
    goto rebuildMenu;
  }
loadSelectedLevel:
  {
    /* Rebuild the menu in the briefing room and load the level (host and local game) or wait for it from
       the host (client); FRONTEND_PAGE_ACTION_MISSION_BRIEFING_PAGE opens once every player has it. */
    initResult = Frontend_Init(FRONTEND_ROM_RECORD_MISSION_BRIEFING);
    errorOrByteCount = initResult.frontendRootOrError;
    if (initResult.failed) goto initFailed;
    g_FrontendScenarioInitializationCount++;
    g_FrontendPendingPageAction = FRONTEND_PAGE_ACTION_MISSION_BRIEFING_PAGE;
    roleStateFlagsPtr = &g_FrontendPlayerRuntimeBlocks->factionAssignment.roleStateFlags;
    *roleStateFlagsPtr = *roleStateFlagsPtr | FRONTEND_PLAYER_STATE_LEVEL_LOADED;
    if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_CLIENT) == SESSION_NETWORK_ROLE_LOCAL) {
      /* levelPathOffsetOrLoadedFieldGrid holds the field grid pointer once loaded, an offset (<= 0xFFFF)
         into the level before */
      if ((g_FrontendLoadedLevelAsset != NULL) &&
         (0xffff < g_FrontendLoadedLevelAsset->header.pathState.levelPathOffsetOrLoadedFieldGrid))
      {
        Resource_Release((void *)(g_FrontendLoadedLevelAsset->header).pathState.
                                 levelPathOffsetOrLoadedFieldGrid);
      }
      Resource_Release(g_FrontendLoadedLevelAsset);
      g_FrontendLoadedLevelAsset = NULL;
      packageLoadResult = Package_LoadEntry((uint16_t *)&g_FrontendScenarioPathScratchUtf16);
      checkedResult = FatalError_ExitIfFailed((uint32_t)packageLoadResult.bufferOrError,packageLoadResult.failed);
      g_FrontendLoadedLevelAsset = (FrontendLoadedLevelRuntimeImage370 *)checkedResult.valueOrError;
      encodeCursorOrSize = g_FrontendLoadedLevelAsset->header.common.buildMetadata.
                assetRelativeAddressAnchor28 +
                (g_FrontendLoadedLevelAsset->header.pathState.levelPathOffsetOrLoadedFieldGrid -
                0x28);
      /* the field grid file: the level's path with the extension "fld", under the executable directory */
      WidePath_SetExtensionCode(0x646c66,(uint16_t *)encodeCursorOrSize);
      WidePath_CombineDirectoryAndLeaf
                ((uint16_t *)&g_LevelResourcePathScratchUtf16,(uint16_t *)encodeCursorOrSize,
                 (uint16_t *)&g_ExecutableDirectoryUtf16);
      packageLoadResult = Package_LoadEntry((uint16_t *)encodeCursorOrSize);
      loadedLevelAsset = g_FrontendLoadedLevelAsset;
      transferSourceBytes = g_PackageScratchBuffer;
      sourceGrid = packageLoadResult.bufferOrError;
      if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_HOST) != SESSION_NETWORK_ROLE_LOCAL) {
        /* Host: build the level transfer in the package scratch buffer (level size, grid size, packed level
           size, packed grid size, then both packed images), copy it to its own allocation and offer it to
           the clients. */
        fieldGridAllocationSize = sourceGrid->common.allocationSizeBytes;
        ((ScenarioLevelBundleHeader *)g_PackageScratchBuffer)->levelDecodedBytes =
             g_FrontendLoadedLevelAsset->header.common.allocationSizeBytes;
        ((ScenarioLevelBundleHeader *)transferSourceBytes)->fieldGridDecodedBytes = fieldGridAllocationSize;
        encodeCursorOrSize = (uint8_t *)((ScenarioLevelBundleHeader *)transferSourceBytes + 1);
        savedFieldGrid = sourceGrid;
        encodeResult = PckCodec_EncodeHuffmanRle
                           (0x7fffe8,encodeCursorOrSize,loadedLevelAsset->header.common.allocationSizeBytes,
                            (uint8_t *)loadedLevelAsset);
        checkedResult = FatalError_ExitIfFailed(encodeResult.byteCountOrError,encodeResult.failed);
        errorOrByteCount = checkedResult.valueOrError;
        ((ScenarioLevelBundleHeader *)transferSourceBytes)->levelEncodedBytes = errorOrByteCount;
        encodeResult = PckCodec_EncodeFieldGrid
                           (0x7fffe8 - errorOrByteCount,encodeCursorOrSize + errorOrByteCount,
                            sourceGrid->common.allocationSizeBytes,sourceGrid);
        checkedResult = FatalError_ExitIfFailed(encodeResult.byteCountOrError,encodeResult.failed);
        ((ScenarioLevelBundleHeader *)transferSourceBytes)->fieldGridEncodedBytes = checkedResult.valueOrError;
        /* the cursor becomes the total transfer size in bytes */
        encodeCursorOrSize = encodeCursorOrSize + errorOrByteCount + (checkedResult.valueOrError - (int)transferSourceBytes);
        allocResult = g_MemoryApi.alloc((uint32_t)encodeCursorOrSize);
        checkedResult = FatalError_ExitIfFailed(allocResult.payloadOrError,allocResult.failed);
        transferDwordCursor = (uint32_t *)checkedResult.valueOrError;
        for (remainingDwords = (uint32_t)encodeCursorOrSize >> 2; remainingDwords != 0; remainingDwords--) {
          *transferDwordCursor = *(uint32_t *)transferSourceBytes;
          transferSourceBytes = transferSourceBytes + 4;
          transferDwordCursor = transferDwordCursor + 1;
        }
        sourceGrid = savedFieldGrid;
        UiTransferMailbox_SetOutgoingBuffer((UiTransferPayloadByteCount)encodeCursorOrSize,(uint32_t *)checkedResult.valueOrError)
        ;
      }
      loadedLevelAsset->header.pathState.levelPathOffsetOrLoadedFieldGrid = (uint32_t)sourceGrid;
      FrontendPlayerRuntime_InitializeFactionAssignments();
    }
    else {
      UiTransferMailbox_MarkUnavailable();
      g_FrontendScenarioTransferState = 5;
    }
    goto nextFrame;
  }
rebuildMenu:
  initResult = Frontend_Init(nextRomRecordId);
  errorOrByteCount = initResult.frontendRootOrError;
  g_FrontendPendingPageAction = countOrLevelIdOrNextAction;
  if (!initResult.failed) goto nextFrame;
  goto initFailed;
}


/* Address: 0x0050C380.
   Pointer-move handler of the model pointer context (pointerMove of g_FrontendModelPointerContextVtable): stores
   the best model hit under the pointer, then returns the cursor frame. While a non-right button is held
   (ROUTE_TO_SECONDARY_CALLBACK) resolvedActionCallback108 decides it, with no button resolvedActionCallback104;
   while the right button drags the camera (ROUTE_TO_BUILTIN_ACTION_RESOLUTION) the frame shows the camera
   motion FrontendModelPointerContext_DispatchWorldCameraPointerInput will perform for the camera scheme bits,
   the left button and the modifier keys (1 move, 0x0F pitch, 0x10 heading and pitch, 0x11 distance, 0x25
   heading, 0x0E heading and distance, 0x12..0x14 the variants of scheme 0x8000).
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
  context->selectedModelNode = (ModelRuntimeNode *)(bestHit >> 32);
  if ((context->contextFlags & FRONTEND_MODEL_POINTER_CONTEXT_ROUTE_TO_SECONDARY_CALLBACK) != 0) {
    if (context->resolvedActionCallback108 != NULL)
    {
      callbackResult = context->resolvedActionCallback108
                        (context->callbackArgumentF0,context->callbackArgumentEC,
                         context->callbackArgumentE8,context->selectedHitMetric,
                         context->selectedModelNode,context);
      return callbackResult;
    }
    return 0;
  }
  if ((context->contextFlags & FRONTEND_MODEL_POINTER_CONTEXT_ROUTE_TO_BUILTIN_ACTION_RESOLUTION) ==
      0) {
    if (context->resolvedActionCallback104 != NULL)
    {
      callbackResult = context->resolvedActionCallback104
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
  if ((context->contextFlags & FRONTEND_MODEL_POINTER_CONTEXT_CAMERA_ORBIT) != 0)
  {
    if ((g_CursorButtonState & LEFT) != 0) {
      return 0x11;
    }
    return 0x10;
  }
  if ((context->contextFlags & FRONTEND_MODEL_POINTER_CONTEXT_CAMERA_PAN) != 0)
  {
    if ((g_KeyboardStateMask & KEYBOARD_STATE_CTRL) != 0) {
      if ((g_CursorButtonState & LEFT) != 0) {
        return 0x11;
      }
      return 0x12;
    }
    if ((g_CursorButtonState & LEFT) != 0) {
      return 0x14;
    }
    return 0x13;
  }
  if ((context->contextFlags & FRONTEND_MODEL_POINTER_CONTEXT_CAMERA_FREE) == 0)
  {
    return 0;
  }
  if ((g_KeyboardStateMask & KEYBOARD_STATE_CTRL) != 0) {
    return 0xf;
  }
  if ((g_KeyboardStateMask & KEYBOARD_STATE_ALT) == 0) {
    if ((g_KeyboardStateMask & KEYBOARD_STATE_SHIFT) != 0) {
      return 0x25;
    }
    if ((context->contextFlags & FRONTEND_MODEL_POINTER_CONTEXT_HIDE_PANEL) ==
        0) {
      if ((g_CursorButtonState & LEFT) == 0) {
        return 1;
      }
      if ((context->contextFlags & FRONTEND_MODEL_POINTER_CONTEXT_LINK_ROTATION_ZOOM)
          != 0) {
        return 0xe;
      }
      if ((context->contextFlags & FRONTEND_MODEL_POINTER_CONTEXT_LINK_ROTATION_TILT)
          == 0) {
        return 0x25;
      }
    }
    else {
      if ((g_CursorButtonState & LEFT) != 0) {
        if ((context->contextFlags & FRONTEND_MODEL_POINTER_CONTEXT_LINK_ROTATION_ZOOM)
            != 0) {
          return 0xf;
        }
        if ((context->contextFlags & FRONTEND_MODEL_POINTER_CONTEXT_LINK_ROTATION_TILT)
            == 0) {
          return 0;
        }
        return 0x11;
      }
      if ((context->contextFlags & FRONTEND_MODEL_POINTER_CONTEXT_LINK_ROTATION_ZOOM)
          != 0) {
        return 0xe;
      }
      if ((context->contextFlags & FRONTEND_MODEL_POINTER_CONTEXT_LINK_ROTATION_TILT)
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
   Press of a non-right button on the model pointer context (nonRightPress of
   g_FrontendModelPointerContextVtable): remembers the press point (corner of the drag frame), stores the best
   model hit, routes the following pointer moves to resolvedActionCallback108 and reports the press to
   resolvedActionCallback10C.
*/
void FrontendModelPointerContext_NonRightPress
          (UiPointerWheelDelta wheelDelta,UiPixelCoordinate pointerY,UiPixelCoordinate pointerX,
          FrontendModelPointerContextRuntimeState17C *callbackContext)

{
  uint64_t bestHit;
  
  callbackContext->scratchCoordinate160 = pointerX;
  callbackContext->scratchCoordinate164 = pointerY;
  bestHit = FrontendModelPointerContext_FindBestEligibleModelHitTarget
                    (pointerY,pointerX,(FrontendModelPointerContextRuntimeState118 *)callbackContext
                    );
  callbackContext->selectedModelNode = (ModelRuntimeNode *)(bestHit >> 32);
  callbackContext->selectedHitMetric = (int)bestHit;
  callbackContext->contextFlags =
       callbackContext->contextFlags | FRONTEND_MODEL_POINTER_CONTEXT_ROUTE_TO_SECONDARY_CALLBACK;
  if (callbackContext->resolvedActionCallback10C !=
      NULL) {
    callbackContext->resolvedActionCallback10C
              (callbackContext->callbackArgumentF0,callbackContext->callbackArgumentEC,
               callbackContext->callbackArgumentE8,callbackContext->selectedHitMetric,
               callbackContext->selectedModelNode,
               (FrontendModelPointerContextRuntimeState118 *)callbackContext);
  }
  return;
}


/* Address: 0x0050C610.
   Release of a non-right button on the model pointer context (nonRightRelease of
   g_FrontendModelPointerContextVtable): stores the best model hit, routes pointer moves back to the hover
   callback and reports the release to resolvedActionCallback114.
*/
void FrontendModelPointerContext_NonRightRelease
          (UiPointerWheelDelta wheelDelta,UiPixelCoordinate pointerY,UiPixelCoordinate pointerX,
          FrontendModelPointerContextRuntimeState118 *callbackContext)

{
  uint64_t bestHit;
  
  bestHit = FrontendModelPointerContext_FindBestEligibleModelHitTarget
                    (pointerY,pointerX,callbackContext);
  callbackContext->selectedModelNode = (ModelRuntimeNode *)(bestHit >> 32);
  callbackContext->selectedHitMetric = (int)bestHit;
  callbackContext->contextFlags =
       callbackContext->contextFlags & ~FRONTEND_MODEL_POINTER_CONTEXT_ROUTE_TO_SECONDARY_CALLBACK;
  if (callbackContext->resolvedActionCallback114 !=
      NULL) {
    callbackContext->resolvedActionCallback114
              (callbackContext->callbackArgumentF0,callbackContext->callbackArgumentEC,
               callbackContext->callbackArgumentE8,callbackContext->selectedHitMetric,
               callbackContext->selectedModelNode,callbackContext);
  }
  return;
}


/* Address: 0x0050C670.
   Drag with a non-right button on the model pointer context (nonRightDrag of
   g_FrontendModelPointerContextVtable): remembers the current point (the other corner of the drag frame),
   stores the best model hit and reports the drag to resolvedActionCallback110.
*/
void FrontendModelPointerContext_NonRightDrag
          (UiPointerWheelDelta wheelDelta,UiPixelCoordinate pointerY,UiPixelCoordinate pointerX,
          FrontendModelPointerContextRuntimeState17C *callbackContext)

{
  uint64_t bestHit;
  
  callbackContext->scratchCoordinate168 = pointerX;
  callbackContext->scratchCoordinate16C = pointerY;
  bestHit = FrontendModelPointerContext_FindBestEligibleModelHitTarget
                    (pointerY,pointerX,(FrontendModelPointerContextRuntimeState118 *)callbackContext
                    );
  callbackContext->selectedModelNode = (ModelRuntimeNode *)(bestHit >> 32);
  callbackContext->selectedHitMetric = (int)bestHit;
  if (callbackContext->resolvedActionCallback110 !=
      NULL) {
    callbackContext->resolvedActionCallback110
              (callbackContext->callbackArgumentF0,callbackContext->callbackArgumentEC,
               callbackContext->callbackArgumentE8,callbackContext->selectedHitMetric,
               callbackContext->selectedModelNode,
               (FrontendModelPointerContextRuntimeState118 *)callbackContext);
  }
  return;
}


/* Address: 0x00549B40.
   Handler of action 0x2044 (slot 68 of g_FrontendUiActionHandlersPage20.handlers00_54), the colour buttons of
   the faction setup page: finds the row of the pressed button in the factionControls offset table and cycles
   that faction's colour (FrontendFactionSetup_CycleFactionColour directly in a local game,
   FRONTEND_COMMAND_CYCLE_FACTION_COLOUR in a network game).
*/
void FrontendUiAction2044_Handler(UiNodeBase *factionControl)

{
  CommandPayloadDword04 rowIndex;
  
  rowIndex = 0;
  do {
    if ((int)factionControl - g_FrontendRootNode ==
        g_FrontendTaskAssignmentControlOffsets.factionControls.offsets[rowIndex + 1]) {
      if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_NETWORKED_MASK) ==
          SESSION_NETWORK_ROLE_LOCAL) {
        FrontendFactionSetup_CycleFactionColour(g_LocalPlayerRuntimeId,0,0,rowIndex);
      }
      else {
        FrontendCommandQueue_EnqueueLocalPlayerCommand(FRONTEND_COMMAND_CYCLE_FACTION_COLOUR,0,0,rowIndex);
      }
      return;
    }
    rowIndex++;
  } while (rowIndex < 7);
  return;
}


/* Address: 0x00549BC0.
   Handler of action 0x2045 (slot 69 of g_FrontendUiActionHandlersPage20.handlers00_54), the mode buttons of
   the faction setup page: finds the row of the pressed button in the playerControls offset table and toggles
   whether that faction takes part (FrontendFactionSetup_ToggleFactionActive directly in a local game,
   FRONTEND_COMMAND_TOGGLE_FACTION_ACTIVE in a network game).
*/
void FrontendUiAction2045_Handler(UiNodeBase *playerControl)

{
  CommandPayloadDword04 rowIndex;
  
  rowIndex = 0;
  do {
    if ((int)playerControl - g_FrontendRootNode ==
        g_FrontendTaskAssignmentControlOffsets.playerControls.offsets[rowIndex + 1]) {
      if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_NETWORKED_MASK) ==
          SESSION_NETWORK_ROLE_LOCAL) {
        FrontendFactionSetup_ToggleFactionActive(g_LocalPlayerRuntimeId,0,0,rowIndex);
      }
      else {
        FrontendCommandQueue_EnqueueLocalPlayerCommand(FRONTEND_COMMAND_TOGGLE_FACTION_ACTIVE,0,0,rowIndex);
      }
      return;
    }
    rowIndex++;
  } while (rowIndex < 7);
  return;
}


/* Address: 0x00549C40.
   Handler of action 0x2046 (slot 70 of g_FrontendUiActionHandlersPage20.handlers00_54), the "play" checkboxes
   of the faction setup page: finds the row of the pressed checkbox in the selectionRows offset table and makes
   that faction the local player's (FrontendFactionSetup_ChooseFaction directly in a local game,
   FRONTEND_COMMAND_CHOOSE_FACTION in a network game).
*/
void FrontendUiAction2046_Handler(UiNodeBase *selectionRowControl)

{
  CommandPayloadDword04 rowIndex;
  
  rowIndex = 0;
  do {
    if ((int)selectionRowControl - g_FrontendRootNode ==
        g_FrontendTaskAssignmentControlOffsets.selectionRows.offsets[rowIndex + 1]) {
      if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_NETWORKED_MASK) ==
          SESSION_NETWORK_ROLE_LOCAL) {
        FrontendFactionSetup_ChooseFaction(g_LocalPlayerRuntimeId,0,0,rowIndex);
      }
      else {
        FrontendCommandQueue_EnqueueLocalPlayerCommand(FRONTEND_COMMAND_CHOOSE_FACTION,0,0,rowIndex);
      }
      return;
    }
    rowIndex++;
  } while (rowIndex < 7);
  return;
}


/* Address: 0x0050BB80.
   Relocate method of the model pointer context (relocate of g_FrontendModelPointerContextVtable), run when the
   control is built from its template: puts the camera target at the origin, gives camera limits the template
   left at 0 their defaults, starts with no candidate models and no overlay entity, then relocates the children.
*/
void FrontendModelPointerContext_Relocate
               (UiSerializedRelocationDelta relocationDelta,
               FrontendModelPointerContextRuntimeState17C *control)

{
  control->targetPositionXQ12 = 0;
  control->targetPositionYQ12 = 0;
  control->targetPositionZQ12 = 0;
  /* angles in 1/0x10000 of a turn: pitch -90..+90 degrees */
  if (control->minimumPitchAngle == 0) {
    control->minimumPitchAngle = 0xffffc000;
  }
  if (control->maximumPitchAngle == 0) {
    control->maximumPitchAngle = 0x4000;
  }
  /* camera distance 0.25..127 (Q12) */
  if (control->minimumDistanceQ12 == 0) {
    control->minimumDistanceQ12 = 0x400;
  }
  if (control->maximumDistanceOrSurfaceLimitQ12 == 0) {
    control->maximumDistanceOrSurfaceLimitQ12 = 0x7f000;
  }
  control->contextValue58 = 0;
  control->objectCountOrFrontendStateAC = 0;
  control->candidateModelListHead = NULL;
  control->selectedOverlayEntity = NULL;
  UiContainer_RelocateChildren(relocationDelta,&control->base);
  return;
}


/* Address: 0x0050BC30.
   Layout method of the model pointer context (layout of g_FrontendModelPointerContextVtable): a new size
   invalidates the reusable terrain projection (WorldRuntime_ClearFieldGridDirtyFlag clears
   TERRAIN_RENDER_REUSE_PROJECTION), then the children are laid out.
*/
void FrontendModelPointerContext_Layout(WorldRuntimeContext *callbackContext)

{
  WorldRuntime_ClearFieldGridDirtyFlag(callbackContext);
  UiContainer_LayoutChildren((UiNodeBase *)callbackContext);
  return;
}


/* Address: 0x0050BC60.
   Draw method of the model pointer context (drawClipped of g_FrontendModelPointerContextVtable), the 3D view of
   the menu room and of the in-game world: clamps the clip rectangle to the control, sets up camera, projection
   and (optionally) the sound listener, then renders the candidate models in up to four primitive-queue passes
   (models with flag 0x200, the terrain, the shading pass of models with flag 0x100, the remaining models),
   releasing and re-acquiring the render spin lock between passes. Afterwards the enabled selection
   overlays are drawn and the child controls on top. Nothing is drawn while FRONTEND_MENU_ROOM_RENDER_SUPPRESSED
   is set (a dialog page covers the room).
*/
void FrontendModelPointerContext_RenderWorldViewQueuesClipped
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

  if ((control->contextFlags & FRONTEND_MENU_ROOM_RENDER_SUPPRESSED) != 0) {
    return;
  }
  if (clipRight < control->base.left) {
    clipRight = control->base.left;
  }
  if (control->base.right < clipLeft) {
    clipLeft = control->base.right;
  }
  if (clipBottom < control->base.top) {
    clipBottom = control->base.top;
  }
  if (control->base.bottom < clipTop) {
    clipTop = control->base.bottom;
  }
  g_GraphicsSetViewportAndClearDepth(clipTop,clipLeft,clipBottom,clipRight);
  g_SpinLockAcquire(control->renderSpinLock);
  cursorOverrideY = g_CursorOverrideY;
  cursorOverrideX = g_CursorOverrideX;
  control->selectedModelNode = NULL;
  control->selectedHitMetric = WORLD_POINTER_NO_HIT;
  control->callbackArgumentE8 = WORLD_POINTER_NO_HIT;
  control->callbackArgumentEC = WORLD_POINTER_NO_HIT;
  control->callbackArgumentF0 = WORLD_POINTER_NO_HIT;
  overlayClipTop = cursorOverrideY << 12;
  control->cursorWorldXQ12 = cursorOverrideX << 12;
  control->cursorWorldYQ12 = overlayClipTop;
  control->renderedPrimitiveCount = 0;
  Graphics_SetProjectionClipRect(clipTop,clipLeft,clipBottom,clipRight);
  Graphics_SetViewProjectionParameters
            (control->projectionShift,control->viewAngle1,control->viewAngle0,
             control->projectionScale,control->hitReferenceWorldZQ12,control->hitReferenceWorldYQ12,
             control->hitReferenceWorldXQ12);
  originY = clipLeft;
  if ((control->contextFlags & WORLD_RUNTIME_FLAG_SOUND_LISTENER) != 0) {
    originY = control->targetPositionYQ12;
    overlayClipTop = ((int)control->committedDistanceOrSoundZOffset >> 2) + control->targetPositionZQ12;
    SpatialSound_RebuildListenerTransformFromPose
              (control->viewAngle1,control->viewAngle0,overlayClipTop,originY,control->targetPositionXQ12);
  }
  Graphics_SetProjectionViewport
            (control->base.bottom,control->base.right,control->base.top,control->base.left);
  Graphics_SetAuxiliaryOrientation
            (control->auxiliaryOrientationAngle1,control->auxiliaryOrientationAngle0);
  Graphics_SetSceneBounds
            (control->sceneBound7,control->sceneBound6,control->sceneBound5,control->sceneBound4,
             control->sceneBound3,control->sceneBound2,control->sceneBound1,control->sceneBound0);
  Graphics_RebuildFrustumPlanes();
  g_GraphicsBeginScene();
  g_SpinLockReleaseAndInvoke(control->renderSpinLockReleaseCallback,control->renderSpinLock);
  g_SpinLockAcquire(control->renderSpinLock);
  GraphicsShadingRuntime_RebuildCompactLightingRecords();
  g_SpinLockReleaseAndInvoke(control->renderSpinLockReleaseCallback,control->renderSpinLock);
  g_SpinLockAcquire(control->renderSpinLock);
  queueResult = GraphicsPrimitiveQueue_ResetGlobal();
  overlayClipRight = clipTop;
  overlayClipBottom = clipBottom;
  if (!queueResult.failed) {
    Graphics_SetActivePrimitiveQueue(queueResult.queue);
    control->activePrimitiveQueue = queueResult.queue;
    if (control->renderPhaseCallback15C != NULL) {
      control->renderPhaseCallback15C(GRAPHICS_STATE_DISABLED,(WorldRuntimeContext *)control);
    }
    renderHierarchyProc = ModelRuntime_CullAndRenderHierarchyRecursive;
    if ((control->contextFlags & FRONTEND_MODEL_POINTER_CONTEXT_COMPARE_HITS_BY_METRIC_ONLY) != 0) {
      renderHierarchyProc = ModelRuntime_RenderHierarchyRecursiveAlternatePath;
    }
    overlayClipTop = (UiPixelCoordinate)(uintptr_t)renderHierarchyProc;
    for (modelNode = control->candidateModelListHead; modelNode != NULL;
        modelNode = (ModelRuntimeNode *)(modelNode->common).nextNode) {
      if ((((modelNode->runtimeFlags & 0x40) == 0) && ((modelNode->runtimeFlags & 0x200) != 0)) &&
         (modelNode->runtimeFlags = modelNode->runtimeFlags & 0xfffffffd,
         (modelNode->tintArgb & 0xff000000) != 0)) {
        renderHierarchyProc(modelNode);
      }
    }
    if (control->renderPhaseCallback15C != NULL) {
      control->renderPhaseCallback15C(GRAPHICS_STATE_ENABLED,(WorldRuntimeContext *)control);
    }
    PTR_GraphicsPrimitiveQueue_RadixSortForRendering_00485844
              (control->base.nodeFlags & 8,control->activePrimitiveQueue);
    g_GraphicsDrawPrimitiveQueue
              (clipTop,clipLeft,clipBottom,clipRight,control->activePrimitiveQueue);
    queuedPrimitiveCount = GraphicsPrimitiveQueue_GetCount(control->activePrimitiveQueue);
    control->renderedPrimitiveCount = control->renderedPrimitiveCount + queuedPrimitiveCount;
    g_SpinLockReleaseAndInvoke(control->renderSpinLockReleaseCallback,control->renderSpinLock);
    g_SpinLockAcquire(control->renderSpinLock);
    overlayClipBottom = 0; /* EDI: the model loop ran to its NULL terminator */
    if (((control->contextFlags & WORLD_RUNTIME_FLAG_DRAW_TERRAIN) != 0) && (control->fieldGrid != NULL)) {
      queueResult = GraphicsPrimitiveQueue_ResetGlobal();
      if (queueResult.failed) goto endScene;
      Graphics_SetActivePrimitiveQueue(queueResult.queue);
      control->activePrimitiveQueue = queueResult.queue;
      TerrainProjectedGrid_TransformShadeAndQueue(control->fieldGrid,control);
      PTR_GraphicsPrimitiveQueue_RadixSortForRendering_00485844
                (control->base.nodeFlags & 8,control->activePrimitiveQueue);
      g_GraphicsDrawPrimitiveQueue
                (clipTop,clipLeft,clipBottom,clipRight,control->activePrimitiveQueue);
      queuedPrimitiveCount = GraphicsPrimitiveQueue_GetCount(control->activePrimitiveQueue);
      control->renderedPrimitiveCount = control->renderedPrimitiveCount + queuedPrimitiveCount;
    }
    g_SpinLockReleaseAndInvoke(control->renderSpinLockReleaseCallback,control->renderSpinLock);
    g_SpinLockAcquire(control->renderSpinLock);
    if ((control->contextFlags & WORLD_RUNTIME_FLAG_SHADING_ENABLED) != 0) {
      modelNode = control->candidateModelListHead;
      overlayClipBottom = (UiPixelCoordinate)(uintptr_t)modelNode; /* EDI */
      queueResult = GraphicsPrimitiveQueue_ResetGlobal();
      if (queueResult.failed) goto endScene;
      Graphics_SetActivePrimitiveQueue(queueResult.queue);
      control->activePrimitiveQueue = queueResult.queue;
      if (modelNode != NULL) {
        GraphicsShadingGeneratedTexture_ResetPassScratchAndClearAlphaPlanes();
        do {
          if ((((modelNode->runtimeFlags & 0x40) == 0) && ((modelNode->runtimeFlags & 0x100) != 0)) &&
             ((modelNode->tintArgb & 0xff000000) != 0)) {
            GraphicsShadingGeneratedTexture_ProcessRenderableHierarchy
                      (modelNode,(GeneratedTextureRenderContextView *)control);
          }
          modelNode = (ModelRuntimeNode *)(modelNode->common).nextNode;
        } while (modelNode != NULL);
        GraphicsShadingGeneratedTexture_RefreshTouchedAlphaSubresources();
        overlayClipBottom = 0;
      }
      PTR_GraphicsPrimitiveQueue_RadixSortForRendering_00485844
                (control->base.nodeFlags & 8,control->activePrimitiveQueue);
      g_GraphicsDrawPrimitiveQueue
                (clipTop,clipLeft,clipBottom,clipRight,control->activePrimitiveQueue);
      queuedPrimitiveCount = GraphicsPrimitiveQueue_GetCount(control->activePrimitiveQueue);
      control->renderedPrimitiveCount = control->renderedPrimitiveCount + queuedPrimitiveCount;
    }
    g_SpinLockReleaseAndInvoke(control->renderSpinLockReleaseCallback,control->renderSpinLock);
    g_SpinLockAcquire(control->renderSpinLock);
    queueResult = GraphicsPrimitiveQueue_ResetGlobal();
    if (!queueResult.failed) {
      Graphics_SetActivePrimitiveQueue(queueResult.queue);
      control->activePrimitiveQueue = queueResult.queue;
      if (control->renderPhaseCallback15C != NULL) {
        control->renderPhaseCallback15C(GRAPHICS_STATE_DISABLED,(WorldRuntimeContext *)control);
      }
      renderHierarchyProc = ModelRuntime_CullAndRenderHierarchyRecursive;
      if ((control->contextFlags & FRONTEND_MODEL_POINTER_CONTEXT_COMPARE_HITS_BY_METRIC_ONLY) != 0)
      {
        renderHierarchyProc = ModelRuntime_RenderHierarchyRecursiveAlternatePath;
      }
      for (modelNode = control->candidateModelListHead; modelNode != NULL;
          modelNode = (ModelRuntimeNode *)(modelNode->common).nextNode) {
        if (((modelNode->runtimeFlags & 0x240) == 0) &&
           (modelNode->runtimeFlags = modelNode->runtimeFlags & 0xfffffffd,
           (modelNode->tintArgb & 0xff000000) != 0)) {
          renderHierarchyProc(modelNode);
        }
      }
      if (control->renderPhaseCallback15C != NULL) {
        control->renderPhaseCallback15C(GRAPHICS_STATE_ENABLED,(WorldRuntimeContext *)control);
      }
      PTR_GraphicsPrimitiveQueue_RadixSortForRendering_00485844
                (control->base.nodeFlags & 8,control->activePrimitiveQueue);
      g_GraphicsDrawPrimitiveQueue
                (clipTop,clipLeft,clipBottom,clipRight,control->activePrimitiveQueue);
      queuedPrimitiveCount = GraphicsPrimitiveQueue_GetCount(control->activePrimitiveQueue);
      control->renderedPrimitiveCount = control->renderedPrimitiveCount + queuedPrimitiveCount;
      g_SpinLockReleaseAndInvoke(control->renderSpinLockReleaseCallback,control->renderSpinLock);
      g_SpinLockAcquire(control->renderSpinLock);
      originY = clipLeft;
      overlayClipTop = clipTop;
      overlayClipRight = clipRight;
      overlayClipBottom = clipBottom;
      if ((((clipRight == control->base.left) && (clipLeft == control->base.right)) &&
          (clipBottom == control->base.top)) && (clipTop == control->base.bottom)) {
        /* the whole view was drawn: the next terrain pass may reuse this projection */
        control->contextFlags = control->contextFlags | TERRAIN_RENDER_REUSE_PROJECTION;
      }
    }
  }
endScene:
  g_GraphicsEndScene();
  g_RenderedFrameCountSinceDebugRefresh++;
  if ((control->base.nodeFlags & UI_NODE_SUPPRESSED) == 0) {
    g_SelectionPanelBlitOpaque = g_GraphicsTextureSourceBlitSourceAlpha;
    g_SelectionPanelBlitClipped = g_GraphicsTextureSourceBlitTiledSourceAlpha;
  }
  else {
    g_SelectionPanelBlitOpaque = g_GraphicsTextureSourceBlitHalfSourceRgb;
    g_SelectionPanelBlitClipped = g_GraphicsTextureSourceBlitTiledHalfSourceRgb;
  }
  if ((g_UiCommandRuntimeFlags & 0x8000) == 0) {
    if ((((control->contextFlags & WORLD_RUNTIME_FLAG_DRAW_ARMY_METRICS) != 0) &&
        (SelectionOverlay_RenderSelectedArmyMetrics
                   (overlayClipTop,originY,overlayClipBottom,
                    overlayClipRight),
        control->selectedOverlayEntity != NULL)) &&
       (entryFound = SelectionInfo_FindEntry(control->selectedOverlayEntity), entryFound)) {
      SelectionOverlay_RenderArmyMetricsForEntity
                (overlayClipTop,originY,overlayClipBottom,
                 overlayClipRight,control->selectedOverlayEntity);
    }
    if ((control->contextFlags & WORLD_RUNTIME_FLAG_DRAG_SELECTING) != 0) {
      SelectionOverlay_DrawBoundsFrame
                (overlayClipTop,originY,overlayClipBottom,
                 overlayClipRight,control->scratchCoordinate16C,
                 control->scratchCoordinate168,control->scratchCoordinate164,
                 control->scratchCoordinate160);
    }
    if ((control->contextFlags & WORLD_RUNTIME_FLAG_DRAW_TERRAIN_POINT_MARKERS) != 0) {
      SelectionOverlay_DrawMarkerADForFieldGridTerrainPoints
                (overlayClipTop,originY,overlayClipBottom,
                 overlayClipRight,control->terrainMarkerPointCount174,
                 control->terrainMarkerCoordinatePairs170,control->fieldGrid);
    }
    if (((control->contextFlags & WORLD_RUNTIME_FLAG_DRAW_SURFACE_POINT_MARKER) != 0) && (control->callbackArgumentF0 != WORLD_POINTER_NO_HIT)) {
      SelectionOverlay_DrawMarkerACForWorldSurfacePoint
                (overlayClipTop,originY,overlayClipBottom,
                 overlayClipRight,
                 (uint32_t)((g_UiCommandModeGColorVariantLimit & 0xff000000) != 0),
                 control->callbackArgumentEC,control->callbackArgumentE8,control->fieldGrid);
    }
    if ((control->contextFlags & WORLD_RUNTIME_FLAG_DRAW_GRID_VERTEX_MARKERS) != 0) {
      SelectionOverlay_DrawMarkerAEForVisibleProjectedGridVertices
                (overlayClipTop,originY,overlayClipBottom,
                 overlayClipRight,control->fieldGrid);
    }
    if ((control->contextFlags & WORLD_RUNTIME_FLAG_SECONDARY_SURFACE_ONLY) != 0) {
      SelectionOverlay_DrawMarkerAFB0ForProjectedVertexStateFlags
                (overlayClipTop,originY,overlayClipBottom,
                 overlayClipRight,control->fieldGrid);
    }
    if ((control->contextFlags & WORLD_RUNTIME_FLAG_DRAW_REGION_MARKERS) != 0) {
      SelectionOverlay_DrawMarkerB1B2ForProjectedVertexMask1800
                (overlayClipTop,originY,overlayClipBottom,
                 overlayClipRight,(uint8_t)control->overlayMarkerStateB4,
                 control->fieldGrid);
    }
    if ((((control->contextFlags & WORLD_RUNTIME_FLAG_DRAW_TERRAIN) != 0) && (control->fieldGrid != NULL))
       && ((g_UiCommandRuntimeFlags & 0x40) != 0)) {
      SelectionOverlay_DrawMarkerAFForProjectedVertexFlag8000
                (overlayClipTop,originY,overlayClipBottom,
                 overlayClipRight,control->fieldGrid);
    }
  }
  g_SelectionPanelBlitOpaque = g_GraphicsTextureSourceBlitSourceAlpha;
  g_SelectionPanelBlitClipped = g_GraphicsTextureSourceBlitTiledSourceAlpha;
  g_SpinLockReleaseAndInvoke(control->renderSpinLockReleaseCallback,control->renderSpinLock);
  if ((control->selectedModelNode != NULL) &&
     ((int)control->callbackArgumentF0 < control->selectedHitMetric)) {
    control->selectedModelNode = NULL;
  }
  UiContainer_DrawIntersectingChildren(clipTop,clipLeft,clipBottom,clipRight,&control->base);
  return;
}


/* Address: 0x0050C6E0.
   Right-button press on the model pointer context (rightPress of g_FrontendModelPointerContextVtable): starts a
   camera drag. Remembers the press point (the pointer is put back there after every drag step), routes pointer
   moves to the camera cursor resolution, restarts the held-tick counter (rightButtonState11C, counted by
   FrontendModelPointerContext_Tick) and pins the drawn cursor.
*/
void FrontendModelPointerContext_RightPress
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
  g_CursorUseOverridePosition++;
  return;
}


/* Address: 0x0050C730.
   Right-button release on the model pointer context (rightRelease of g_FrontendModelPointerContextVtable): ends
   the camera drag and unpins the cursor. A release within 7 ticks of the press counts as a click and is
   reported to rightReleaseCallback118 (the menu room stops its camera flight with it).
*/
void FrontendModelPointerContext_RightRelease
          (UiPointerWheelDelta wheelDelta,UiPixelCoordinate pointerY,UiPixelCoordinate pointerX,
          FrontendModelPointerContextRuntimeState17C *callbackContext)

{
  g_CursorUseOverridePosition = 0;
  /* clears ROUTE_TO_BUILTIN_ACTION_RESOLUTION and the camera motion bits 0..3 */
  callbackContext->contextFlags = callbackContext->contextFlags & 0xffffffb0;
  if ((callbackContext->rightButtonState11C < 7) &&
     (callbackContext->rightReleaseCallback118 != NULL)) {
    callbackContext->rightReleaseCallback118(callbackContext);
  }
  return;
}


/* Address: 0x0050CC80.
   Right-button drag on the model pointer context (rightDrag of g_FrontendModelPointerContextVtable): moves the
   camera by the pointer's offset from the press point, then puts the pointer back there, snapshots the camera
   state and calls the view's clearTransientStateCallback. The motion depends on the camera scheme bit of the
   view (0x100, 0x8000 or 0x200), the left button and the modifier keys: move, heading, pitch, distance or a
   combination; 0x10 blocks camera input. FrontendModelPointerContext_SelectBestModelHitTargetAndResolveAction
   shows the matching cursor.
*/
void FrontendModelPointerContext_DispatchWorldCameraPointerInput
          (UiPointerWheelDelta wheelDelta,UiPixelCoordinate pointerY,UiPixelCoordinate pointerX,
          WorldRuntimeContext *callbackContext)

{
  InGameWorldTransientStateClearCallbackProc *clearTransientCallback;
  uint32_t pointerDeltaX;
  AngleTurn32 pointerDeltaY;

  /* The low four runtimeFlags bits record the motion in progress: 1 move, 2 heading, 4 distance, 8 pitch.
     The pointer's X offset drives the heading, its Y offset pitch, distance and moves. */
  pointerDeltaX = pointerX - callbackContext->pointerCaptureX;
  pointerDeltaY = pointerY - callbackContext->pointerCaptureY;
  if ((callbackContext->runtimeFlags & 0x10) != 0) {
    return;
  }
  if ((callbackContext->runtimeFlags & 0x100) != 0) {
    if ((g_CursorButtonState & LEFT) == 0) {
      callbackContext->runtimeFlags = callbackContext->runtimeFlags & 0xfffffffa;
      callbackContext->runtimeFlags = callbackContext->runtimeFlags | 10;
      WorldMotion_AdjustHeadingAndRecomputePosition(pointerDeltaX,callbackContext);
      WorldMotion_AdjustPitchClampAndRecomputePosition(pointerDeltaY,callbackContext);
    }
    else {
      callbackContext->runtimeFlags = callbackContext->runtimeFlags & 0xfffffff4;
      callbackContext->runtimeFlags = callbackContext->runtimeFlags | 4;
      WorldMotion_AdjustDistanceClampAndRecomputePosition(pointerDeltaY,callbackContext);
    }
  }
  else if ((callbackContext->runtimeFlags & 0x8000) != 0) {
    if ((g_CursorButtonState & LEFT) == 0) {
      if ((g_KeyboardStateMask & KEYBOARD_STATE_CTRL) == 0) {
        callbackContext->runtimeFlags = callbackContext->runtimeFlags & 0xfffffff1;
        callbackContext->runtimeFlags = callbackContext->runtimeFlags | 1;
        WorldMotion_TranslateCurrentAndTargetByInputElevationAndHeadingQuarterTurn
                  (pointerDeltaY,pointerDeltaX,callbackContext);
        WorldMotion_TranslateCurrentAndTargetByNegatedPitchReverseHeading
                  (pointerDeltaY,callbackContext);
      }
      else {
        callbackContext->runtimeFlags = callbackContext->runtimeFlags & 0xfffffffa;
        callbackContext->runtimeFlags = callbackContext->runtimeFlags | 10;
        WorldMotion_AdjustHeadingAndClearFieldGridDirty(pointerDeltaX,callbackContext);
        /* EDX: the pointer Y delta, as in the other pitch branches (the decompile lost it). */
        WorldMotion_AdjustPitchClampAndClearFieldGridDirty(pointerDeltaY,callbackContext);
      }
    }
    else if ((g_KeyboardStateMask & KEYBOARD_STATE_CTRL) == 0) {
      callbackContext->runtimeFlags = callbackContext->runtimeFlags & 0xfffffff1;
      callbackContext->runtimeFlags = callbackContext->runtimeFlags | 1;
      WorldMotion_TranslateCurrentAndTargetByPitchQuarterTurn(pointerDeltaY,callbackContext);
    }
    else {
      callbackContext->runtimeFlags = callbackContext->runtimeFlags & 0xfffffff4;
      callbackContext->runtimeFlags = callbackContext->runtimeFlags | 4;
      WorldMotion_AdjustPositionMagnitudeClamp(pointerDeltaY,callbackContext);
    }
  }
  else {
    if ((callbackContext->runtimeFlags & 0x200) == 0) {
      return;
    }
    if ((g_KeyboardStateMask & KEYBOARD_STATE_CTRL) != 0) {
      callbackContext->runtimeFlags = callbackContext->runtimeFlags & 0xfffffff8;
      callbackContext->runtimeFlags = callbackContext->runtimeFlags | 8;
      WorldMotion_AdjustPitchClampAndRecomputePosition(pointerDeltaY,callbackContext);
    }
    else if ((g_KeyboardStateMask & KEYBOARD_STATE_ALT) != 0) {
      callbackContext->runtimeFlags = callbackContext->runtimeFlags & 0xfffffff4;
      callbackContext->runtimeFlags = callbackContext->runtimeFlags | 4;
      WorldMotion_AdjustDistanceClampAndRecomputePosition(pointerDeltaY,callbackContext);
    }
    else if ((g_KeyboardStateMask & KEYBOARD_STATE_SHIFT) != 0) {
      callbackContext->runtimeFlags = callbackContext->runtimeFlags & 0xfffffff2;
      callbackContext->runtimeFlags = callbackContext->runtimeFlags | 2;
      WorldMotion_AdjustHeadingAndRecomputePosition(pointerDeltaX,callbackContext);
    }
    else if (((callbackContext->runtimeFlags & 0x4000000) != 0) && ((g_CursorButtonState & LEFT) != 0)) {
      /* Flag 0x4000000 with the button held: 0x40000000 selects pitch, 0x80000000 distance. */
      callbackContext->runtimeFlags = callbackContext->runtimeFlags & 0xfffffff0;
      if ((callbackContext->runtimeFlags & 0x40000000) != 0) {
        callbackContext->runtimeFlags = callbackContext->runtimeFlags | 8;
        WorldMotion_AdjustPitchClampAndRecomputePosition(pointerDeltaY,callbackContext);
      }
      else if ((callbackContext->runtimeFlags & 0x80000000) != 0) {
        callbackContext->runtimeFlags = callbackContext->runtimeFlags | 4;
        WorldMotion_AdjustDistanceClampAndRecomputePosition(pointerDeltaY,callbackContext);
      }
    }
    else if (((callbackContext->runtimeFlags & 0x4000000) == 0) && ((g_CursorButtonState & LEFT) == 0)) {
      callbackContext->runtimeFlags = callbackContext->runtimeFlags & 0xfffffff1;
      callbackContext->runtimeFlags = callbackContext->runtimeFlags | 1;
      WorldRuntime_TranslateCameraByScreenDelta(pointerDeltaY,pointerDeltaX,callbackContext);
      WorldRuntime_RecomputeMotionEndpointAgainstFieldSurface(callbackContext);
    }
    else {
      /* Heading drag (button state opposite to flag 0x4000000); afterwards 0x40000000 adds a distance step and
         0x80000000 a pitch step (flags re-read after the heading call). */
      callbackContext->runtimeFlags = callbackContext->runtimeFlags & 0xfffffff2;
      callbackContext->runtimeFlags = callbackContext->runtimeFlags | 2;
      WorldMotion_AdjustHeadingAndRecomputePosition(pointerDeltaX,callbackContext);
      if ((callbackContext->runtimeFlags & 0x40000000) != 0) {
        callbackContext->runtimeFlags = callbackContext->runtimeFlags | 4;
        WorldMotion_AdjustDistanceClampAndRecomputePosition(pointerDeltaY,callbackContext);
      }
      else if ((callbackContext->runtimeFlags & 0x80000000) != 0) {
        callbackContext->runtimeFlags = callbackContext->runtimeFlags | 8;
        WorldMotion_AdjustPitchClampAndRecomputePosition(pointerDeltaY,callbackContext);
      }
    }
  }
  g_PointerSetPosition(callbackContext->pointerCaptureY,callbackContext->pointerCaptureX);
  clearTransientCallback = callbackContext->fieldRegion.clearTransientStateCallback;
  WorldRuntime_CaptureMotionStateToSnapshot(callbackContext);
  if (clearTransientCallback != NULL) {
    clearTransientCallback(callbackContext);
  }
  return;
}


/* Address: 0x0050CED0.
   Wheel handler of the model pointer context (pointerWheel of g_FrontendModelPointerContextVtable): unless
   camera input is blocked (0x10) or the view has no camera scheme (0x100/0x200/0x8000), the scaled wheel
   delta changes the camera distance, with Ctrl the pitch, and the camera state is snapshotted.
*/
void FrontendModelPointerContext_PointerWheel
          (UiPointerWheelDelta wheelDelta,UiPixelCoordinate pointerY,UiPixelCoordinate pointerX,
          WorldRuntimeContext *callbackContext)

{
  int scaledWheelDelta;
  
  if (((callbackContext->runtimeFlags & 0x10) == 0) &&
     ((callbackContext->runtimeFlags & 0x8300) != 0)) {
    if ((g_KeyboardStateMask & KEYBOARD_STATE_CTRL) == 0) {
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
   Keyboard handler of the model pointer context (keyboardEvent of g_FrontendModelPointerContextVtable): offers
   the key to the view's keyboardFallback first; when there is none or it returns CF set, the default handling
   (UiNode_DefaultKeyboardEventMoveFocusNext) decides. Returns CF.
*/
bool FrontendModelPointerContext_KeyboardEvent(UiKeyboardStateMask keyboardStateMask,UiKeyboardEventCode keyCode,
          FrontendModelPointerContextRuntimeState118 *control)

{
  bool keyboardEventCarry;
  
  if ((control->keyboardFallback != NULL) &&
     (keyboardEventCarry = control->keyboardFallback(keyboardStateMask,keyCode,(UiRootNode *)control),
     !keyboardEventCarry)) {
    return keyboardEventCarry;
  }
  keyboardEventCarry = UiNode_DefaultKeyboardEventMoveFocusNext(keyboardStateMask,keyCode,&control->base);
  return keyboardEventCarry;
}


/* Address: 0x0050CF90.
   Tick method of the model pointer context (tick of g_FrontendModelPointerContextVtable): counts the ticks the
   right button is held (rightButtonState11C, read by FrontendModelPointerContext_RightRelease) and, in a view
   without camera scheme 0x100/0x8000, camera input block (0x10) and WORLD_RUNTIME_FLAG_UNLIMITED_CAMERA, eases
   the camera distance by one convergence step per tick until it is within 15/16..17/16 of the clamped committed
   distance, placing the camera behind the target.
*/
void FrontendModelPointerContext_Tick(WorldRuntimeContext *callbackContext)

{
  uint32_t *callbackStateCounter;
  UQ12 targetDistance;
  UQ12 convergenceStep;
  UQ12 clampedCommittedDistance;
  FixedDirection cameraOffset;
  
  /* 0x40: right button held (ROUTE_TO_BUILTIN_ACTION_RESOLUTION); +0x11C is rightButtonState11C of the
     pointer-context view of this record */
  if ((callbackContext->runtimeFlags & 0x40) != 0) {
    callbackStateCounter = &callbackContext->selection.reservedCallbackState40;
    *callbackStateCounter = *callbackStateCounter + 1;
  }
  if ((callbackContext->runtimeFlags & 0x48110) == 0) {
    clampedCommittedDistance = callbackContext->motion.committedDistanceQ12;
    if ((int)clampedCommittedDistance < (int)callbackContext->minimumCameraDistanceQ12) {
      clampedCommittedDistance = callbackContext->minimumCameraDistanceQ12;
    }
    if ((int)callbackContext->maximumCameraDistanceQ12 < (int)clampedCommittedDistance) {
      clampedCommittedDistance = callbackContext->maximumCameraDistanceQ12;
    }
    targetDistance = callbackContext->motion.targetDistanceQ12;
    convergenceStep = g_WorldMotionTargetDistanceConvergenceStepQ12;
    if ((int)(clampedCommittedDistance * 0xf) >> 4 <= (int)targetDistance) {
      if ((int)targetDistance <= (int)(clampedCommittedDistance * 0x11) >> 4) {
        return;
      }
      convergenceStep = -g_WorldMotionTargetDistanceConvergenceStepQ12;
    }
    callbackContext->motion.targetDistanceQ12 = targetDistance + convergenceStep;
    /* heading ^ 0x8000 turns half round: the camera sits behind the target */
    cameraOffset = FixedMath_DirectionFromAnglesScaledRegs
                      (-callbackContext->motion.pitchAngle,
                       callbackContext->motion.headingAngle ^ 0x8000,targetDistance + convergenceStep);
    callbackContext->motion.positionXQ12 =
         cameraOffset.x + callbackContext->motion.targetPositionXQ12;
    callbackContext->motion.positionYQ12 =
         cameraOffset.y + callbackContext->motion.targetPositionYQ12;
    callbackContext->motion.positionZQ12 =
         cameraOffset.z + callbackContext->motion.targetPositionZQ12;
    WorldRuntime_ClearFieldGridDirtyFlag(callbackContext);
  }
  return;
}


/* Address: 0x00514E40.
   Refreshes the HUD resource numbers of the active faction in the in-game root: Xenite and Tritium
   (current / storage limit), Energy demand / generation capacity, and baseline Energy supply plus the Tritium
   extraction rate. Q4 amounts are shown as whole units (>> 4); the Xenite amount is also formatted as text.
*/
void FrontendRuntime_UpdateCurrentFactionMetricCache(void)

{
  XeniteAmountQ4 xeniteStorageLimit;
  TritiumAmountQ4 tritiumStorageLimit;
  int xeniteCurrentDisplay;
  FactionProgressAmountQ4 baselineEnergySupplyQ4;
  FactionProgressAmountQ4 energyGenerationCapacityQ4;
  FactionArmyContributionValue tritiumExtractionRate;
  InGameRuntimeRootImageC3E4 *runtimeRoot;
  int activeFactionIndex;
  
  runtimeRoot = g_InGameRuntimeRoot;
  activeFactionIndex = g_InGameRuntimeRoot->worldRuntime0A30.activeFactionRuntimeIndex;
  xeniteStorageLimit = g_GameFactionRuntimeImage.records[activeFactionIndex].xeniteStorageLimitQ4;
  xeniteCurrentDisplay = (int)g_GameFactionRuntimeImage.records[activeFactionIndex].xeniteCurrentQ4 >> 4;
  g_InGameRuntimeRoot->primaryResourceDisplayCurrent49B4 = xeniteCurrentDisplay;
  runtimeRoot->primaryResourceDisplayLimit49B8 = (int)xeniteStorageLimit >> 4;
  /* decimal, no fraction digits */
  g_WideNumberFormatUtf16
            (WIDE_FORMAT_WRITE_TERMINATOR,0,10,1,xeniteCurrentDisplay,
             (uint16_t *)&g_FrontendCurrentFactionPrimaryResourceTextUtf16);
  tritiumStorageLimit = g_GameFactionRuntimeImage.records[activeFactionIndex].tritiumStorageLimitQ4;
  baselineEnergySupplyQ4 = g_GameFactionRuntimeImage.records[activeFactionIndex].baselineEnergySupplyQ4;
  runtimeRoot->secondaryResourceDisplayCurrent4A4C =
       (int)g_GameFactionRuntimeImage.records[activeFactionIndex].tritiumCurrentQ4 >> 4;
  runtimeRoot->secondaryResourceDisplayLimit4A50 = (int)tritiumStorageLimit >> 4;
  energyGenerationCapacityQ4 = g_GameFactionRuntimeImage.records[activeFactionIndex].energyGenerationCapacityQ4;
  tritiumExtractionRate =
       g_GameFactionRuntimeImage.records[activeFactionIndex].tritiumExtractionRateQ4PerTick;
  runtimeRoot->transientContributionDisplay4AE4 =
       (int)(g_GameFactionRuntimeImage.records[activeFactionIndex].suppliedEnergyDemandQ4 +
            g_GameFactionRuntimeImage.records[activeFactionIndex].unpoweredEnergyDemandQ4) >> 4;
  runtimeRoot->progressLimitDisplay4AE8 = (int)energyGenerationCapacityQ4 >> 4;
  /* the extraction rate is added unshifted, as in the original */
  runtimeRoot->combinedProgressOrArmyScaleDisplay4B28 =
       ((int)baselineEnergySupplyQ4 >> 4) + tritiumExtractionRate;
  return;
}


/* Address: 0x00547620.
   Periodic timer callback of the frontend (80 Hz): counts g_FrontendTimerCountdownTicks down to zero.
   Frontend_StateTick uses the countdown to pace its network polling.
*/
void __cdecl FrontendRuntime_TimerCountdownTick(void)

{
  if (g_FrontendTimerCountdownTicks != 0) {
    g_FrontendTimerCountdownTicks--;
  }
  return;
}

/* Address: 0x00547FB0.
   Periodic timer callback of the frontend (256 Hz): advances the clock of the menu camera flight while a ROM
   transition is pending; the flight's spline is evaluated at g_FrontendRomTransitionElapsedTicks.
*/
void __cdecl FrontendRomTransition_AdvanceElapsedTicks(void)

{
  if (g_FrontendRomTransitionTargetRecordId != 0) {
    g_FrontendRomTransitionElapsedTicks++;
  }
  return;
}

/* Address: 0x00548030.
   Keyboard fallback of the menu room's pointer context: looks the key up in the frontend hotkey table
   (commandCode + required modifier class, see KEYBOARD_STATE_*). Alt+Q and Alt+key 0x20004 leave the
   current menu: back to the main page, a network session is closed first; Ctrl+key 0x20001 on the faction
   setup page toggles bit 0 of the local player's colourCycleFlags (an eighth entry in the faction cycle,
   FrontendFactionSetup_CycleFactionColour). Returns true (CF set) when the key is not in the table.
*/
bool FrontendRuntime_DispatchCommandByCodeAndModifierFlags
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
    uint32_t flags = record->modifierClassFlags; /* the modifier classes the entry requires */
    if (record->commandCode == 0) {
      return true;
    }
    if (record->commandCode != commandCode) {
      continue;
    }
    if (flags == 0) {
      if ((modifierFlags & KEYBOARD_STATE_ANY_MODIFIER) != 0) continue;
    }
    else {
      if ((flags & KEYBOARD_STATE_SHIFT) != 0) {
        if ((modifierFlags & KEYBOARD_STATE_SHIFT) == 0) continue;
      }
      else if ((modifierFlags & KEYBOARD_STATE_SHIFT) != 0) {
        continue;
      }
      if ((flags & KEYBOARD_STATE_ALT) == 0) {
        if (((modifierFlags & KEYBOARD_STATE_CTRL) == 0) || ((modifierFlags & KEYBOARD_STATE_ALT) != 0)) continue;
      }
      else if ((flags & KEYBOARD_STATE_CTRL) == 0) {
        if (((modifierFlags & KEYBOARD_STATE_CTRL) != 0) || ((modifierFlags & KEYBOARD_STATE_ALT) == 0)) continue;
      }
      else {
        if (((modifierFlags & KEYBOARD_STATE_CTRL) == 0) || ((modifierFlags & KEYBOARD_STATE_ALT) == 0)) continue;
      }
    }
    target = (uint32_t)record->continuationEntryAddress;
    break;
  }
  switch (target) {
  case 0x548140:
    if (UiPageStack_ActivePageNotInList((UiPageStackControl *)FRONTEND_UI(root,frontendPageStack)).pageIndex ==
        FRONTEND_PAGE_FACTION_SETUP) {
      if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_NETWORKED_MASK) != 0) {
        FrontendCommandQueue_EnqueueLocalPlayerCommand(FRONTEND_COMMAND_XOR_PLAYER_STATE,0,0,1);
      }
      else {
        FrontendPlayerRuntime_XorStateMaskByPlayerId(g_LocalPlayerRuntimeId,0,0,1);
      }
    }
    break;
  case 0x548190: {
    FrontendPlayerRuntimeRecord *player;
    TextResolveResult text;
    if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_NETWORKED_MASK) == 0) {
      /* local game with no campaign or scenario loaded: only queue UI action 0 */
      if ((g_FrontendLoadedCampaignAsset == 0) && (g_FrontendScenarioInitializationCount == 0)) {
        UiActionQueue_Enqueue(0,root);
        break;
      }
      Resource_Release((void *)(uintptr_t)g_FrontendLoadedCampaignAsset);
      g_FrontendLoadedCampaignAsset = 0;
      g_FrontendScenarioInitializationCount = 0;
      g_FrontendNetworkState = FRONTEND_NETWORK_STATE_IDLE;
      g_NetworkBackendSlot3();
      g_NetworkBackendSlot1();
      UiPageStack_SetActiveIndex(FRONTEND_PAGE_MAIN,(UiPageStackControl *)FRONTEND_UI(root,frontendPageStack));
      ((FrontendModelPointerContextRuntimeState17C *)FRONTEND_UI(root,menuRoomModelView))->contextFlags &= ~FRONTEND_MENU_ROOM_RENDER_SUPPRESSED;
      g_FrontendPendingPageAction = FRONTEND_PAGE_ACTION_NONE;
      g_FrontendRomTransitionPageAction = 0;
      FrontendRomTransition_ActivateRecordById
                (FRONTEND_ROM_RECORD_MAIN_MENU,(WorldRuntimeContext *)FRONTEND_UI(root,menuRoomModelView));
      break;
    }
    /* Leaving a network session. The transcription names bit 0 the host (0x00548320) and bit 1 the client
       (0x00548250), but SESSION_NETWORK_ROLE_CLIENT is bit 0; the variable follows the enum. */
    {
      int wasClient = (g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_CLIENT) != 0;
      g_SessionNetworkRoleFlags = g_SessionNetworkRoleFlags & ~SESSION_NETWORK_ROLE_NETWORKED_MASK;
      g_FrontendNetworkState = FRONTEND_NETWORK_STATE_IDLE;
      g_FrontendScenarioInitializationCount = 0;
      g_NetworkBackendSlot3();
      g_NetworkBackendSlot1();
      UiPageStack_SetActiveIndex(FRONTEND_PAGE_MAIN,(UiPageStackControl *)FRONTEND_UI(root,frontendPageStack));
      ((FrontendModelPointerContextRuntimeState17C *)FRONTEND_UI(root,menuRoomModelView))->contextFlags &= ~FRONTEND_MENU_ROOM_RENDER_SUPPRESSED;
      g_FrontendPendingPageAction = FRONTEND_PAGE_ACTION_NONE;
      g_FrontendRomTransitionPageAction = 0;
      player = g_FrontendPlayerRuntimeBlocks;
      FrontendRomTransition_ActivateRecordById
                (FRONTEND_ROM_RECORD_MAIN_MENU,(WorldRuntimeContext *)FRONTEND_UI(root,menuRoomModelView));
      /* chat history notice with the first player's name */
      text = TextResource_Resolve(wasClient ? TEXT_ID_NETWORK_SESSION_LEFT : TEXT_ID_NETWORK_SESSION_CLOSED);
      RichTextCommandStream_PatchPayloadBySelector(0,&player->playerName,text.text);
      FrontendRecentTextHistory_InsertAndRebuild5(text.text);
      if (!wasClient) {
        /* the player record becomes a fresh local one (same fields as the client path of
           FrontendNetworkSetupPage_InitializeBackendMode) */
        g_FrontendPlayerRuntimeBlockCount = 1;
        g_LocalPlayerRuntimeId = 0;
        *(uint32_t *)&player->playerName = 0; /* first two code units */
        player->playerRuntimeId = 0;
        player->factionAssignment.roleStateFlags = 0;
        player->colourCycleFlags = 0;
        player->snapshotTransferFlags = 0;
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
   Runs one record of the frontend ROM action table locally, with the activation sound (the direct-call form of
   the FRONTEND_COMMAND_EXECUTE_ROM_ACTION command handler).
*/
void FrontendState_DispatchCode(FrontendStatusCode romRecordIndex)

{
  FrontendRomActionTable_ExecuteRecord(0,0,false,romRecordIndex);
  return;
}


/* Address: 0x00548910.
   Hover handler of the menu room's pointer context (resolvedActionCallback104/108). On the main page (not on a
   network client) an object of the room that has a usable ROM action record starts a camera flight towards
   the record's keyframe and makes the pointer cursor frame 7; the record's hint text (text id 0x2000 + hint)
   is shown in the hint box, hint 1 while a page action is still being processed, none otherwise.
   Actions 3, 4, 9 and negative ones are not offered in a network session, the network page (2) not without
   a network backend (the same rule as FrontendRomActionTable_ExecuteRecord). Returns the cursor frame index.
*/
uint32_t FrontendRuntime_UpdatePointerContextAndSceneView
          (uint32_t callbackArgument1,uint32_t callbackArgument2,uint32_t callbackArgument3,uint32_t hitMetric,
          void *pointedModelNode,FrontendPointerSceneRuntimeView *frontendRuntime)

{
  int32_t *hintEdgeField;
  uint16_t *previousCommandStream;
  UiNodeBase *control;
  UiNodeVtable *controlVtable;
  RomAssetRecordPrefix *pointedRomRecord;
  int *transitionRecord; /* keyframe channels 0..5, [6] hint text, [8] FRONTEND_PAGE_ACTION_* */
  uint32_t resultCode;
  uint16_t *commandStream;
  RomRecordId recordId;
  int channel3OrHintValue;
  int keyframeChannel5;
  int channel4OrHalfHeight;
  RichTextExtentRegs textExtent;
  PageStackSearchResult pageStackStatus;
  TextResolveResult hintTextResult;
  TextureSizeResult windowTextureSize;

  resultCode = 0;
  channel3OrHintValue = 0;
  if (((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_CLIENT) == SESSION_NETWORK_ROLE_LOCAL) &&
     (pageStackStatus = UiPageStack_ActivePageNotInList(&frontendRuntime->activePageStack),
     pageStackStatus.pageIndex == 0)) {
    pointedRomRecord = RomRegistry_FindRecordBySlotValue((RomRegistrySlotValue)pointedModelNode);
    recordId = 0xf0000000; /* matches no record */
    if (pointedRomRecord != NULL) {
      recordId = pointedRomRecord->recordId;
    }
    transitionRecord = RomRecordTable_FindRecordById(recordId,g_FrontendActiveRomRecord);
    /* Skip records without a transition, network-only pages (3/4/9/negative) in a networked session and the
       network page (2) when no backend exists. */
    if ((transitionRecord != NULL) &&
       ((((transitionRecord[8] != FRONTEND_PAGE_ACTION_GAMEPLAY_SETTINGS_PAGE &&
          (transitionRecord[8] != FRONTEND_PAGE_ACTION_QUIT_CONFIRM_PAGE)) &&
         (transitionRecord[8] != FRONTEND_PAGE_ACTION_CREDITS)) &&
         (transitionRecord[8] >= 0)) ||
        ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_NETWORKED_MASK) == SESSION_NETWORK_ROLE_LOCAL)) &&
       ((transitionRecord[8] != FRONTEND_PAGE_ACTION_NETWORK_SETUP_PAGE) || (g_NetworkBackendInstanceCount != 0))) {
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
        /* fly from the current camera (keyframe 0) to the record's camera (keyframe 1) in 0xC0 ticks of
           FrontendRomTransition_AdvanceElapsedTicks; pending -1 = no record to activate at the end */
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
        g_FrontendRomTransitionTargetRecordId = 0xffffffff;
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
      frontendRuntime->hintBox.hintActive = 0;
      frontendRuntime->hintBox.commandStream = NULL;
      return resultCode;
    }
    channel3OrHintValue = 1;
  }
  /* only when the text changed: size the hint box around it plus the window frame (texture frame 0x72) */
  previousCommandStream = frontendRuntime->hintBox.commandStream;
  hintTextResult = TextResource_Resolve(channel3OrHintValue + TEXT_ID_MENU_HINT_BASE);
  commandStream = hintTextResult.text;
  if (commandStream != previousCommandStream) {
    frontendRuntime->hintBox.commandStream = commandStream;
    textExtent = RichTextCommandStream_MeasureRegs(g_UiTextStyleNormal,commandStream);
    channel3OrHintValue = (int)(textExtent.widthPixels + 1) >> 1;
    channel4OrHalfHeight = (int)(textExtent.heightPixels + 1) >> 1;
    frontendRuntime->hintBox.base.leftOffset = channel3OrHintValue;
    frontendRuntime->hintBox.base.topOffset = channel4OrHalfHeight;
    frontendRuntime->hintBox.base.right = -channel3OrHintValue;
    frontendRuntime->hintBox.base.bottom = -channel4OrHalfHeight;
    windowTextureSize = g_GraphicsTextureSourceGetLogicalSize(0x72,g_UiWindowTextureSource);
    channel3OrHintValue = windowTextureSize.logicalWidthPixels + 3;
    control = frontendRuntime->hintBox.base.nextSibling;
    frontendRuntime->hintBox.hintActive = 1;
    hintEdgeField = &frontendRuntime->hintBox.base.leftOffset;
    *hintEdgeField = *hintEdgeField + channel3OrHintValue;
    hintEdgeField = &frontendRuntime->hintBox.base.topOffset;
    *hintEdgeField = *hintEdgeField + windowTextureSize.logicalHeightPixels;
    controlVtable = control->vtable;
    hintEdgeField = &frontendRuntime->hintBox.base.right;
    *hintEdgeField = *hintEdgeField - channel3OrHintValue;
    hintEdgeField = &frontendRuntime->hintBox.base.bottom;
    *hintEdgeField = *hintEdgeField - windowTextureSize.logicalHeightPixels;
    controlVtable->layout(control);
  }
  return resultCode;
}


/* Address: 0x00548BE0.
   Button-press handler of the menu room's pointer context (resolvedActionCallback10C): the frontend does nothing
   on press, it acts on release (FrontendMenuRoom_ExecuteClickedRomAction).
*/
void FrontendMenuRoom_PressNoOp
               (uint32_t callbackArgument1,uint32_t callbackArgument2,uint32_t callbackArgument3,
               uint32_t hitMetric,uint32_t pointedModelNode,uint32_t pointerContext)

{
  return;
}

/* Address: 0x00548BF0.
   Drag handler of the menu room's pointer context (resolvedActionCallback110); dragging does nothing in the
   frontend.
*/
void FrontendMenuRoom_DragNoOp
               (uint32_t callbackArgument1,uint32_t callbackArgument2,uint32_t callbackArgument3,
               uint32_t hitMetric,uint32_t pointedModelNode,uint32_t pointerContext)

{
  return;
}

/* Address: 0x00548C00.
   Button-release handler of the menu room's pointer context (resolvedActionCallback114): clicking an object of
   the menu room runs the ROM action record that belongs to it (FrontendRomActionTable_ExecuteRecord). In a
   network game the host sends it as a frontend command so every player follows; clients ignore clicks.
*/
void FrontendMenuRoom_ExecuteClickedRomAction
          (uint32_t callbackArgument1,uint32_t callbackArgument2,uint32_t callbackArgument3,uint32_t hitMetric,
          FrontendCallbackArgument5 pointedModelNode,uint32_t pointerContext)

{
  RomAssetRecordPrefix *slotRecord;
  RomRecordTableIndex recordIndex;
  
  if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_CLIENT) == SESSION_NETWORK_ROLE_LOCAL) {
    slotRecord = RomRegistry_FindRecordBySlotValue(pointedModelNode);
    if (slotRecord != NULL) {
      recordIndex = RomRecordTable_FindIndexById(slotRecord->recordId,g_FrontendActiveRomRecord);
      if (-1 < (int)recordIndex) {
        if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_NETWORKED_MASK) ==
            SESSION_NETWORK_ROLE_LOCAL) {
          FrontendRomActionTable_ExecuteRecord(g_LocalPlayerRuntimeId,0,0,recordIndex);
        }
        else {
          FrontendCommandQueue_EnqueueLocalPlayerCommand(FRONTEND_COMMAND_EXECUTE_ROM_ACTION,0,0,recordIndex);
        }
      }
    }
  }
  return;
}


/* Address: 0x00548C70.
   Right-button release handler of the menu room's pointer context (rightReleaseCallback118): stops the running
   camera flight (ScenarioCatalog_RequestRomTransitionStopCallback), in a network game as a frontend command
   sent by the host; clients ignore it.
*/
void FrontendMenuRoom_StopCameraFlight(uint32_t pointerContext)

{
  if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_CLIENT) == SESSION_NETWORK_ROLE_LOCAL) {
    if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_NETWORKED_MASK) ==
        SESSION_NETWORK_ROLE_LOCAL) {
      ScenarioCatalog_RequestRomTransitionStopCallback(g_LocalPlayerRuntimeId,0,0,0);
    }
    else {
      FrontendCommandQueue_EnqueueLocalPlayerCommand(FRONTEND_COMMAND_STOP_ROM_TRANSITION,0,0,0);
    }
  }
  return;
}

/* Address: 0x00548CB0.
   Adds a chat line to the shared recent-text history and rebuilds the frontend chat history box from its five
   newest entries.
*/
void FrontendRecentTextHistory_InsertAndRebuild5(uint16_t *text)

{
  RecentTextHistoryPointerList *output;
  
  /* lineCount + textLines of the chat history box form the pointer list. */
  output = (RecentTextHistoryPointerList *)
           &((UiConditionalActionControl *)FRONTEND_UI(g_FrontendRootNode,chatMessageHistory))->lineCount;
  RecentTextHistory_Insert(text);
  RecentTextHistory_SortAndBuildPointerList(5,output); /* the box shows five lines */
  return;
}


/* Address: 0x00549100.
   Handler of action 0x2043 (slot 67 of g_FrontendUiActionHandlersPage20.handlers00_54), the mission briefing's
   "Back" button: applies the game speed and returns to the main page with ROM action record 0
   (FRONTEND_COMMAND_APPLY_GAME_SPEED in a network game).
*/
void FrontendCallback_ApplyGameSpeedOrDispatch02C0(uint32_t callbackArgument)

{
  if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_NETWORKED_MASK) ==
      SESSION_NETWORK_ROLE_LOCAL) {
    FrontendSession_ApplyGameSpeedAndReturnToMainPage(g_LocalPlayerRuntimeId,0,0,0);
  }
  else {
    FrontendCommandQueue_EnqueueLocalPlayerCommand(FRONTEND_COMMAND_APPLY_GAME_SPEED,0,0,0);
  }
  return;
}


/* Address: 0x00549140.
   Handler of action 0x204F (slot 79 of g_FrontendUiActionHandlersPage20.handlers00_54), the "Exit" button of
   the in-game variant of the mission briefing: releases the loaded campaign and returns to the main page
   (FRONTEND_COMMAND_RELEASE_CAMPAIGN in a network game).
*/
void FrontendCallback_ReleaseSelectedResourceOrDispatch0320(uint32_t callbackArgument)

{
  if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_NETWORKED_MASK) ==
      SESSION_NETWORK_ROLE_LOCAL) {
    FrontendSession_ReleaseSelectedResourceAndReturnToMainPage(g_LocalPlayerRuntimeId,0,0,0);
  }
  else {
    FrontendCommandQueue_EnqueueLocalPlayerCommand(FRONTEND_COMMAND_RELEASE_CAMPAIGN,0,0,0);
  }
  return;
}


/* Address: 0x00549180.
   Handler of action 0x2050 (slot 80 of g_FrontendUiActionHandlersPage20.handlers00_54), the "Save" button of
   the in-game variant of the mission briefing: does nothing.
*/
void FrontendCallback_NoOpArg1(void *source)

{
  return;
}


/* Address: 0x00549AB0.
   Handler of action 0x2040 (slot 64 of g_FrontendUiActionHandlersPage20.handlers00_54), the faction setup
   page's "Back" button: returns to the main page with ROM action record 0 (FRONTEND_COMMAND_RETURN_TO_MAIN_PAGE
   in a network game).
*/
void FrontendCallback_ReturnToMainPageOrDispatch0DC0(uint32_t callbackArgument)

{
  if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_NETWORKED_MASK) ==
      SESSION_NETWORK_ROLE_LOCAL) {
    FrontendSession_ReturnToMainPage(g_LocalPlayerRuntimeId,0,0,0);
  }
  else {
    FrontendCommandQueue_EnqueueLocalPlayerCommand(FRONTEND_COMMAND_RETURN_TO_MAIN_PAGE,0,0,0);
  }
  return;
}


/* Address: 0x0054A5A0.
   Handler of action 0x2034 (slot 52 of g_FrontendUiActionHandlersPage20.handlers00_54), the "Choose game"
   page's "Cancel" button: returns to the main page with ROM action record 0 in a local game and with record 4
   (as FRONTEND_COMMAND_RETURN_TO_MAIN_PAGE) in a network game.
*/
void FrontendCallback_ReturnToMainPageOrDispatchState4(uint32_t callbackArgument)

{
  /* the inner tests repeat the outer one, so only the local-direct and network-queued paths are reachable */
  if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_NETWORKED_MASK) ==
      SESSION_NETWORK_ROLE_LOCAL) {
    if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_NETWORKED_MASK) ==
        SESSION_NETWORK_ROLE_LOCAL) {
      FrontendSession_ReturnToMainPage(g_LocalPlayerRuntimeId,0,0,0);
    }
    else {
      FrontendCommandQueue_EnqueueLocalPlayerCommand(FRONTEND_COMMAND_RETURN_TO_MAIN_PAGE,0,0,0);
    }
  }
  else if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_NETWORKED_MASK) ==
           SESSION_NETWORK_ROLE_LOCAL) {
    FrontendSession_ReturnToMainPage(g_LocalPlayerRuntimeId,0,0,4);
  }
  else {
    FrontendCommandQueue_EnqueueLocalPlayerCommand(FRONTEND_COMMAND_RETURN_TO_MAIN_PAGE,0,0,4);
  }
  return;
}


/* Address: 0x0054A7D0.
   Handler of action 0x2033 (slot 51 of g_FrontendUiActionHandlersPage20.handlers00_54), the quit dialog's "no"
   button: returns to the main page with ROM action record 0 (FRONTEND_COMMAND_RETURN_TO_MAIN_PAGE in a
   network game).
*/
void FrontendCallback_ReturnToMainPageOrDispatch0DC0_Secondary(uint32_t callbackArgument)

{
  if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_NETWORKED_MASK) ==
      SESSION_NETWORK_ROLE_LOCAL) {
    FrontendSession_ReturnToMainPage(g_LocalPlayerRuntimeId,0,0,0);
  }
  else {
    FrontendCommandQueue_EnqueueLocalPlayerCommand(FRONTEND_COMMAND_RETURN_TO_MAIN_PAGE,0,0,0);
  }
  return;
}


/* Address: 0x0054AAD0.
   Handler of action 0x2010 (slot 16 of g_FrontendUiActionHandlersPage20.handlers00_54), shared by the options
   page's "Ok" button and the display settings page's "Back" button: "Ok" returns to the main page (ROM action
   record 0, FRONTEND_COMMAND_RETURN_TO_MAIN_PAGE in a network game), "Back" reopens the options page.
*/
void FrontendUiAction2010_Handler(UiNodeBase *sourceNode)

{
  FrontendModelPointerContextFlags *compactLayoutFlags;
  int parentNodeAddress;
  FrontendRootPageState *frontendRootPage;
  
  parentNodeAddress = (int)sourceNode->parent;
  frontendRootPage = (FrontendRootPageState *)sourceNode;
  while ((UiNodeBase *)parentNodeAddress != UI_NODE_NONE) {
    frontendRootPage = (FrontendRootPageState *)(frontendRootPage->rootNode).parent;
    parentNodeAddress = (int)(frontendRootPage->rootNode).parent;
  }
  if (sourceNode == &frontendRootPage->returnToMainActionControl) {
    if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_NETWORKED_MASK) ==
        SESSION_NETWORK_ROLE_LOCAL) {
      FrontendSession_ReturnToMainPage(g_LocalPlayerRuntimeId,0,0,0);
    }
    else {
      FrontendCommandQueue_EnqueueLocalPlayerCommand(FRONTEND_COMMAND_RETURN_TO_MAIN_PAGE,0,0,0);
    }
    return;
  }
  if ((int)g_FramebufferWidth < FRONTEND_COMPACT_LAYOUT_MAX_WIDTH + 1) {
    compactLayoutFlags =
         &((FrontendModelPointerContextRuntimeState17C *)FRONTEND_UI(frontendRootPage,menuRoomModelView))->contextFlags;
    *compactLayoutFlags = *compactLayoutFlags | FRONTEND_MENU_ROOM_RENDER_SUPPRESSED;
  }
  UiPageStack_SetActiveIndex(FRONTEND_PAGE_OPTIONS,&frontendRootPage->primaryPageStack);
  return;
}


/* Address: 0x0054AB70.
   Handler of action 0x2011 (slot 17 of g_FrontendUiActionHandlersPage20.handlers00_54), the options page's
   "Graphics" button: opens the display settings page and fills its choices: the four smallest distinct colour
   depths and the ten smallest distinct resolutions (width << 16 | height) of g_GraphicsDisplayModes, each
   collected by an insertion into a sorted list with 0xFFFFFFFF as the empty mark, and the name and device of
   up to five adapters. The saved adapter, resolution and colour depth become the current selection.
*/
void FrontendUiAction2011_Handler(FrontendDisplaySettingsPageOptionState *source)

{
  FrontendModelPointerContextFlags *menuRoomContextFlags;
  GraphicsAdapterRecord *adapters;
  uint16_t *deviceNameText;
  uint32_t modeValue;
  uint32_t displacedValueA;
  uint32_t displacedValueB;
  GraphicsDisplayModeCount remainingModes;
  GraphicsDisplayMode *displayMode;
  TextResolveResult fallbackNameResult;
  
  /* source is the frontend template's graphicsSettingsButton */
  UiPageStack_SetActiveIndex
            (FRONTEND_PAGE_DISPLAY_SETTINGS,
             (UiPageStackControl *)FRONTEND_UI((uint8_t *)source - offsetof(FrontendUiImage,graphicsSettingsButton),
                                               frontendPageStack));
  if ((int)g_FramebufferWidth < FRONTEND_COMPACT_LAYOUT_MAX_WIDTH + 1) {
    menuRoomContextFlags =
         &((FrontendModelPointerContextRuntimeState17C *)
           FRONTEND_UI((uint8_t *)source - offsetof(FrontendUiImage,graphicsSettingsButton),menuRoomModelView))->
         contextFlags;
    *menuRoomContextFlags = *menuRoomContextFlags | FRONTEND_MENU_ROOM_RENDER_SUPPRESSED;
  }
  g_FrontendUiDisplayModeAndTaskAssignmentScratch.displayModeScratch.candidateValues[0] =
       0xffffffff;
  g_FrontendUiDisplayModeAndTaskAssignmentScratch.displayModeScratch.candidateValues[1] =
       0xffffffff;
  g_FrontendUiDisplayModeAndTaskAssignmentScratch.displayModeScratch.candidateValues[2] =
       0xffffffff;
  g_FrontendUiDisplayModeAndTaskAssignmentScratch.displayModeScratch.candidateValues[3] =
       0xffffffff;
  /* LOCK/UNLOCK mark the XCHG swaps of the original insertion (no other thread uses this scratch) */
  remainingModes = g_GraphicsDisplayModeCount;
  displayMode = g_GraphicsDisplayModes;
  do {
    modeValue = displayMode->bitsPerPixel;
    if ((((modeValue != g_FrontendUiDisplayModeAndTaskAssignmentScratch.displayModeScratch.
                    candidateValues[0]) &&
         (modeValue != g_FrontendUiDisplayModeAndTaskAssignmentScratch.displayModeScratch.
                   candidateValues[1])) &&
        (modeValue != g_FrontendUiDisplayModeAndTaskAssignmentScratch.displayModeScratch.
                  candidateValues[2])) &&
       (modeValue != g_FrontendUiDisplayModeAndTaskAssignmentScratch.displayModeScratch.
                 candidateValues[3])) {
      displacedValueB = modeValue;
      if (modeValue < g_FrontendUiDisplayModeAndTaskAssignmentScratch.displayModeScratch.
                  candidateValues[0]) {
        LOCK();
        UNLOCK();
        displacedValueB = g_FrontendUiDisplayModeAndTaskAssignmentScratch.displayModeScratch.candidateValues
                [0];
        g_FrontendUiDisplayModeAndTaskAssignmentScratch.displayModeScratch.candidateValues[0] =
             modeValue;
      }
      displacedValueA = displacedValueB;
      if ((uint32_t)displacedValueB <
          g_FrontendUiDisplayModeAndTaskAssignmentScratch.displayModeScratch.candidateValues[1]) {
        LOCK();
        UNLOCK();
        displacedValueA = g_FrontendUiDisplayModeAndTaskAssignmentScratch.displayModeScratch.candidateValues
                [1];
        g_FrontendUiDisplayModeAndTaskAssignmentScratch.displayModeScratch.candidateValues[1] =
             displacedValueB;
      }
      displacedValueB = displacedValueA;
      if ((uint32_t)displacedValueA <
          g_FrontendUiDisplayModeAndTaskAssignmentScratch.displayModeScratch.candidateValues[2]) {
        LOCK();
        UNLOCK();
        displacedValueB = g_FrontendUiDisplayModeAndTaskAssignmentScratch.displayModeScratch.candidateValues
                [2];
        g_FrontendUiDisplayModeAndTaskAssignmentScratch.displayModeScratch.candidateValues[2] =
             displacedValueA;
      }
      if ((uint32_t)displacedValueB <
          g_FrontendUiDisplayModeAndTaskAssignmentScratch.displayModeScratch.candidateValues[3]) {
        LOCK();
        UNLOCK();
        g_FrontendUiDisplayModeAndTaskAssignmentScratch.displayModeScratch.candidateValues[3] =
             displacedValueB;
      }
    }
    displayMode++;
    remainingModes--;
  } while (remainingModes != 0);
  source->colorDepthRows.rows[0].bitsPerPixel =
       g_FrontendUiDisplayModeAndTaskAssignmentScratch.displayModeScratch.candidateValues[0];
  source->colorDepthRows.rows[1].bitsPerPixel =
       g_FrontendUiDisplayModeAndTaskAssignmentScratch.displayModeScratch.candidateValues[1];
  source->colorDepthRows.rows[2].bitsPerPixel =
       g_FrontendUiDisplayModeAndTaskAssignmentScratch.displayModeScratch.candidateValues[2];
  source->colorDepthRows.rows[3].bitsPerPixel =
       g_FrontendUiDisplayModeAndTaskAssignmentScratch.displayModeScratch.candidateValues[3];
  adapters = g_GraphicsAdapters;
  source->adapterRows.rows[0].adapterDescriptionUtf16 = g_GraphicsAdapters->driverDescriptionUtf16
  ;
  if (adapters->deviceGuid.Data1 == GRAPHICS_DEVICE_GUID_SOFTWARE) {
    fallbackNameResult = TextResource_Resolve(TEXT_ID_DISPLAY_SOFTWARE_DEVICE_NAME);
    deviceNameText = fallbackNameResult.text;
  }
  else {
    deviceNameText = adapters->deviceNameUtf16;
  }
  source->adapterRows.rows[0].deviceNameUtf16 = deviceNameText;
  adapters = g_GraphicsAdapters;
  if (1 < g_GraphicsAdapterCount) {
    source->adapterRows.rows[1].adapterDescriptionUtf16 =
         g_GraphicsAdapters[1].driverDescriptionUtf16;
    if (adapters[1].deviceGuid.Data1 == GRAPHICS_DEVICE_GUID_SOFTWARE) {
      fallbackNameResult = TextResource_Resolve(TEXT_ID_DISPLAY_SOFTWARE_DEVICE_NAME);
      deviceNameText = fallbackNameResult.text;
    }
    else {
      deviceNameText = adapters[1].deviceNameUtf16;
    }
    source->adapterRows.rows[1].deviceNameUtf16 = deviceNameText;
  }
  adapters = g_GraphicsAdapters;
  if (2 < g_GraphicsAdapterCount) {
    source->adapterRows.rows[2].adapterDescriptionUtf16 =
         g_GraphicsAdapters[2].driverDescriptionUtf16;
    if (adapters[2].deviceGuid.Data1 == GRAPHICS_DEVICE_GUID_SOFTWARE) {
      fallbackNameResult = TextResource_Resolve(TEXT_ID_DISPLAY_SOFTWARE_DEVICE_NAME);
      deviceNameText = fallbackNameResult.text;
    }
    else {
      deviceNameText = adapters[2].deviceNameUtf16;
    }
    source->adapterRows.rows[2].deviceNameUtf16 = deviceNameText;
  }
  adapters = g_GraphicsAdapters;
  if (3 < g_GraphicsAdapterCount) {
    source->adapterRows.rows[3].adapterDescriptionUtf16 =
         g_GraphicsAdapters[3].driverDescriptionUtf16;
    if (adapters[3].deviceGuid.Data1 == GRAPHICS_DEVICE_GUID_SOFTWARE) {
      fallbackNameResult = TextResource_Resolve(TEXT_ID_DISPLAY_SOFTWARE_DEVICE_NAME);
      deviceNameText = fallbackNameResult.text;
    }
    else {
      deviceNameText = adapters[3].deviceNameUtf16;
    }
    source->adapterRows.rows[3].deviceNameUtf16 = deviceNameText;
  }
  adapters = g_GraphicsAdapters;
  if (4 < g_GraphicsAdapterCount) {
    source->adapterRows.rows[4].adapterDescriptionUtf16 =
         g_GraphicsAdapters[4].driverDescriptionUtf16;
    if (adapters[4].deviceGuid.Data1 == GRAPHICS_DEVICE_GUID_SOFTWARE) {
      fallbackNameResult = TextResource_Resolve(TEXT_ID_DISPLAY_SOFTWARE_DEVICE_NAME);
      deviceNameText = fallbackNameResult.text;
    }
    else {
      deviceNameText = adapters[4].deviceNameUtf16;
    }
    source->adapterRows.rows[4].deviceNameUtf16 = deviceNameText;
  }
  g_FrontendUiDisplayModeAndTaskAssignmentScratch.displayModeScratch.candidateValues[0] =
       0xffffffff;
  g_FrontendUiDisplayModeAndTaskAssignmentScratch.displayModeScratch.candidateValues[1] =
       0xffffffff;
  g_FrontendUiDisplayModeAndTaskAssignmentScratch.displayModeScratch.candidateValues[2] =
       0xffffffff;
  g_FrontendUiDisplayModeAndTaskAssignmentScratch.displayModeScratch.candidateValues[3] =
       0xffffffff;
  g_FrontendUiDisplayModeAndTaskAssignmentScratch.displayModeScratch.candidateValues[4] =
       0xffffffff;
  g_FrontendUiDisplayModeAndTaskAssignmentScratch.displayModeScratch.candidateValues[5] =
       0xffffffff;
  g_FrontendUiDisplayModeAndTaskAssignmentScratch.displayModeScratch.candidateValues[6] =
       0xffffffff;
  g_FrontendUiDisplayModeAndTaskAssignmentScratch.displayModeScratch.candidateValues[7] =
       0xffffffff;
  g_FrontendUiDisplayModeAndTaskAssignmentScratch.displayModeScratch.candidateValues[8] =
       0xffffffff;
  g_FrontendUiDisplayModeAndTaskAssignmentScratch.displayModeScratch.candidateValues[9] =
       0xffffffff;
  remainingModes = g_GraphicsDisplayModeCount;
  displayMode = g_GraphicsDisplayModes;
  do {
    modeValue = displayMode->width * 0x10000 + displayMode->height;
    if ((((modeValue != g_FrontendUiDisplayModeAndTaskAssignmentScratch.displayModeScratch.
                    candidateValues[0]) &&
         (modeValue != g_FrontendUiDisplayModeAndTaskAssignmentScratch.displayModeScratch.
                   candidateValues[1])) &&
        ((modeValue != g_FrontendUiDisplayModeAndTaskAssignmentScratch.displayModeScratch.
                   candidateValues[2] &&
         ((modeValue != g_FrontendUiDisplayModeAndTaskAssignmentScratch.displayModeScratch.
                    candidateValues[3] &&
          (modeValue != g_FrontendUiDisplayModeAndTaskAssignmentScratch.displayModeScratch.
                    candidateValues[4])))))) &&
       ((modeValue != g_FrontendUiDisplayModeAndTaskAssignmentScratch.displayModeScratch.
                  candidateValues[5] &&
        ((((modeValue != g_FrontendUiDisplayModeAndTaskAssignmentScratch.displayModeScratch.
                     candidateValues[6] &&
           (modeValue != g_FrontendUiDisplayModeAndTaskAssignmentScratch.displayModeScratch.
                     candidateValues[7])) &&
          (modeValue != g_FrontendUiDisplayModeAndTaskAssignmentScratch.displayModeScratch.
                    candidateValues[8])) &&
         (modeValue != g_FrontendUiDisplayModeAndTaskAssignmentScratch.displayModeScratch.
                   candidateValues[9])))))) {
      displacedValueB = modeValue;
      if (modeValue < g_FrontendUiDisplayModeAndTaskAssignmentScratch.displayModeScratch.
                  candidateValues[0]) {
        LOCK();
        UNLOCK();
        displacedValueB = g_FrontendUiDisplayModeAndTaskAssignmentScratch.displayModeScratch.candidateValues
                [0];
        g_FrontendUiDisplayModeAndTaskAssignmentScratch.displayModeScratch.candidateValues[0] =
             modeValue;
      }
      displacedValueA = displacedValueB;
      if ((uint32_t)displacedValueB <
          g_FrontendUiDisplayModeAndTaskAssignmentScratch.displayModeScratch.candidateValues[1]) {
        LOCK();
        UNLOCK();
        displacedValueA = g_FrontendUiDisplayModeAndTaskAssignmentScratch.displayModeScratch.candidateValues
                [1];
        g_FrontendUiDisplayModeAndTaskAssignmentScratch.displayModeScratch.candidateValues[1] =
             displacedValueB;
      }
      displacedValueB = displacedValueA;
      if ((uint32_t)displacedValueA <
          g_FrontendUiDisplayModeAndTaskAssignmentScratch.displayModeScratch.candidateValues[2]) {
        LOCK();
        UNLOCK();
        displacedValueB = g_FrontendUiDisplayModeAndTaskAssignmentScratch.displayModeScratch.candidateValues
                [2];
        g_FrontendUiDisplayModeAndTaskAssignmentScratch.displayModeScratch.candidateValues[2] =
             displacedValueA;
      }
      displacedValueA = displacedValueB;
      if ((uint32_t)displacedValueB <
          g_FrontendUiDisplayModeAndTaskAssignmentScratch.displayModeScratch.candidateValues[3]) {
        LOCK();
        UNLOCK();
        displacedValueA = g_FrontendUiDisplayModeAndTaskAssignmentScratch.displayModeScratch.candidateValues
                [3];
        g_FrontendUiDisplayModeAndTaskAssignmentScratch.displayModeScratch.candidateValues[3] =
             displacedValueB;
      }
      displacedValueB = displacedValueA;
      if ((uint32_t)displacedValueA <
          g_FrontendUiDisplayModeAndTaskAssignmentScratch.displayModeScratch.candidateValues[4]) {
        LOCK();
        UNLOCK();
        displacedValueB = g_FrontendUiDisplayModeAndTaskAssignmentScratch.displayModeScratch.candidateValues
                [4];
        g_FrontendUiDisplayModeAndTaskAssignmentScratch.displayModeScratch.candidateValues[4] =
             displacedValueA;
      }
      displacedValueA = displacedValueB;
      if ((uint32_t)displacedValueB <
          g_FrontendUiDisplayModeAndTaskAssignmentScratch.displayModeScratch.candidateValues[5]) {
        LOCK();
        UNLOCK();
        displacedValueA = g_FrontendUiDisplayModeAndTaskAssignmentScratch.displayModeScratch.candidateValues
                [5];
        g_FrontendUiDisplayModeAndTaskAssignmentScratch.displayModeScratch.candidateValues[5] =
             displacedValueB;
      }
      displacedValueB = displacedValueA;
      if ((uint32_t)displacedValueA <
          g_FrontendUiDisplayModeAndTaskAssignmentScratch.displayModeScratch.candidateValues[6]) {
        LOCK();
        UNLOCK();
        displacedValueB = g_FrontendUiDisplayModeAndTaskAssignmentScratch.displayModeScratch.candidateValues
                [6];
        g_FrontendUiDisplayModeAndTaskAssignmentScratch.displayModeScratch.candidateValues[6] =
             displacedValueA;
      }
      displacedValueA = displacedValueB;
      if ((uint32_t)displacedValueB <
          g_FrontendUiDisplayModeAndTaskAssignmentScratch.displayModeScratch.candidateValues[7]) {
        LOCK();
        UNLOCK();
        displacedValueA = g_FrontendUiDisplayModeAndTaskAssignmentScratch.displayModeScratch.candidateValues
                [7];
        g_FrontendUiDisplayModeAndTaskAssignmentScratch.displayModeScratch.candidateValues[7] =
             displacedValueB;
      }
      displacedValueB = displacedValueA;
      if ((uint32_t)displacedValueA <
          g_FrontendUiDisplayModeAndTaskAssignmentScratch.displayModeScratch.candidateValues[8]) {
        LOCK();
        UNLOCK();
        displacedValueB = g_FrontendUiDisplayModeAndTaskAssignmentScratch.displayModeScratch.candidateValues
                [8];
        g_FrontendUiDisplayModeAndTaskAssignmentScratch.displayModeScratch.candidateValues[8] =
             displacedValueA;
      }
      if ((uint32_t)displacedValueB <
          g_FrontendUiDisplayModeAndTaskAssignmentScratch.displayModeScratch.candidateValues[9]) {
        LOCK();
        UNLOCK();
        g_FrontendUiDisplayModeAndTaskAssignmentScratch.displayModeScratch.candidateValues[9] =
             displacedValueB;
      }
    }
    displayMode++;
    remainingModes--;
  } while (remainingModes != 0);
  modeValue = g_FrontendUiDisplayModeAndTaskAssignmentScratch.displayModeScratch.candidateValues[0] &
          0xffff;
  source->resolutionRows.rows[0].width =
       g_FrontendUiDisplayModeAndTaskAssignmentScratch.displayModeScratch.candidateValues[0] >> 16;
  source->resolutionRows.rows[0].height = modeValue;
  modeValue = g_FrontendUiDisplayModeAndTaskAssignmentScratch.displayModeScratch.candidateValues[1] &
          0xffff;
  source->resolutionRows.rows[1].width =
       g_FrontendUiDisplayModeAndTaskAssignmentScratch.displayModeScratch.candidateValues[1] >> 16;
  source->resolutionRows.rows[1].height = modeValue;
  modeValue = g_FrontendUiDisplayModeAndTaskAssignmentScratch.displayModeScratch.candidateValues[2] &
          0xffff;
  source->resolutionRows.rows[2].width =
       g_FrontendUiDisplayModeAndTaskAssignmentScratch.displayModeScratch.candidateValues[2] >> 16;
  source->resolutionRows.rows[2].height = modeValue;
  modeValue = g_FrontendUiDisplayModeAndTaskAssignmentScratch.displayModeScratch.candidateValues[3] &
          0xffff;
  source->resolutionRows.rows[3].width =
       g_FrontendUiDisplayModeAndTaskAssignmentScratch.displayModeScratch.candidateValues[3] >> 16;
  source->resolutionRows.rows[3].height = modeValue;
  modeValue = g_FrontendUiDisplayModeAndTaskAssignmentScratch.displayModeScratch.candidateValues[4] &
          0xffff;
  source->resolutionRows.rows[4].width =
       g_FrontendUiDisplayModeAndTaskAssignmentScratch.displayModeScratch.candidateValues[4] >> 16;
  source->resolutionRows.rows[4].height = modeValue;
  modeValue = g_FrontendUiDisplayModeAndTaskAssignmentScratch.displayModeScratch.candidateValues[5] &
          0xffff;
  source->resolutionRows.rows[5].width =
       g_FrontendUiDisplayModeAndTaskAssignmentScratch.displayModeScratch.candidateValues[5] >> 16;
  source->resolutionRows.rows[5].height = modeValue;
  modeValue = g_FrontendUiDisplayModeAndTaskAssignmentScratch.displayModeScratch.candidateValues[6] &
          0xffff;
  source->resolutionRows.rows[6].width =
       g_FrontendUiDisplayModeAndTaskAssignmentScratch.displayModeScratch.candidateValues[6] >> 16;
  source->resolutionRows.rows[6].height = modeValue;
  modeValue = g_FrontendUiDisplayModeAndTaskAssignmentScratch.displayModeScratch.candidateValues[7] &
          0xffff;
  source->resolutionRows.rows[7].width =
       g_FrontendUiDisplayModeAndTaskAssignmentScratch.displayModeScratch.candidateValues[7] >> 16;
  source->resolutionRows.rows[7].height = modeValue;
  modeValue = g_FrontendUiDisplayModeAndTaskAssignmentScratch.displayModeScratch.candidateValues[8] &
          0xffff;
  source->resolutionRows.rows[8].width =
       g_FrontendUiDisplayModeAndTaskAssignmentScratch.displayModeScratch.candidateValues[8] >> 16;
  source->resolutionRows.rows[8].height = modeValue;
  modeValue = g_FrontendUiDisplayModeAndTaskAssignmentScratch.displayModeScratch.candidateValues[9] &
          0xffff;
  source->resolutionRows.rows[9].width =
       g_FrontendUiDisplayModeAndTaskAssignmentScratch.displayModeScratch.candidateValues[9] >> 16;
  source->resolutionRows.rows[9].height = modeValue;
  g_FrontendUiDisplayModeAndTaskAssignmentScratch.displayEnumeration.persistentSelection.
  adapterIndex = PersistentSettings_Read(1,PERSISTENT_SETTING_ADAPTER_INDEX);
  g_FrontendUiDisplayModeAndTaskAssignmentScratch.displayEnumeration.persistentSelection.width =
       PersistentSettings_Read(640,PERSISTENT_SETTING_DISPLAY_WIDTH);
  g_FrontendUiDisplayModeAndTaskAssignmentScratch.displayEnumeration.persistentSelection.height =
       PersistentSettings_Read(480,PERSISTENT_SETTING_DISPLAY_HEIGHT);
  g_FrontendUiDisplayModeAndTaskAssignmentScratch.displayEnumeration.persistentSelection.
  bitsPerPixel = PersistentSettings_Read(16,PERSISTENT_SETTING_BITS_PER_PIXEL);
  FrontendDisplaySettingsPage_UpdateModeActionAvailability((UiNodeBase *)source);
  return;
}


/* Address: 0x0054BA30.
   Handler of actions 0x202C..0x2030 (slots 44..48 of g_FrontendUiActionHandlersPage20.handlers00_54), the five
   adapter choices of the display settings page: selects the adapter whose button was pressed (identified by
   its offset in the parent container, 0x68 bytes apart) and refreshes which modes can be chosen.
*/
void FrontendUiAction202CTo2030_SharedHandler(UiNodeBase *sourceNode)

{
  int controlOffsetFromParent;

  /* the index is stored before each comparison, as in the original */
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
   Handler of action 0x200C (slot 12 of g_FrontendUiActionHandlersPage20.handlers00_54), a selection change in
   the host lobby's player list: the Kick button (FRONTEND_ACTION_KICK_PLAYER) is hidden while the first row,
   the host itself, is selected and shown for any other player.
*/
void FrontendUiAction200C_Handler(UiPointerListControl *playerListControl)

{
  UiPointerListControl *frontendRoot;
  UiNodeBase *parentCursor;
  
  parentCursor = playerListControl->base.parent;
  frontendRoot = playerListControl;
  while (parentCursor != UI_NODE_NONE) {
    frontendRoot = (UiPointerListControl *)(frontendRoot->base).parent;
    parentCursor = frontendRoot->base.parent;
  }
  if (playerListControl->selectedRowSlot == playerListControl->rowSlots) {
    UiNodeList_SuppressActionId(FRONTEND_ACTION_KICK_PLAYER,&frontendRoot->base);
    return;
  }
  UiNodeList_UnsuppressActionId(FRONTEND_ACTION_KICK_PLAYER,&frontendRoot->base);
  return;
}


/* Address: 0x0054D460.
   Handler of action 0x200E (slot 14 of g_FrontendUiActionHandlersPage20.handlers00_54), a click on the chat
   strip at the top left: drops the oldest chat lines until four are left, then one more (a click removes the
   oldest line shown), and rebuilds the strip's pointer list of at most five lines.
*/
void FrontendRecentText_TrimAndSortTopFive(UiNodeBase *source)

{
  uint32_t currentEntryCount;
  
  for (currentEntryCount = ((UiConditionalActionControl *)source)->lineCount; 4 < currentEntryCount;
      currentEntryCount--) {
    RecentTextHistory_RemoveOldest();
  }
  RecentTextHistory_RemoveOldest();
  RecentTextHistory_SortAndBuildPointerList
            (5,(RecentTextHistoryPointerList *)&((UiConditionalActionControl *)source)->lineCount);
  return;
}


/* Address: 0x0054D4A0.
   Handler of action 0x200F (slot 15 of g_FrontendUiActionHandlersPage20.handlers00_54), a choice in the network
   game page's protocol list: closes the current backend and opens the chosen one on NETWORK_GAME_UDP_PORT. On
   success the local endpoint is copied to g_FrontendNetworkEndpointScratch and formatted into
   g_FrontendNetworkEndpointTextUtf16, the session list is emptied, Join hidden and a discovery probe sent. A failure is reported and the backend opened once more without a report; if that
   fails too, the menu returns to the main page and the random generator to the primary stream.
*/
void FrontendUiAction200F_Handler(FrontendNetworkSetupPageBackendListPtr backendList)

{
  UiListRowIndex selectedBackendIndex;
  int remainingDwords;
  uint32_t *endpointSourceDwordCursor;
  uint32_t *endpointDestinationDwordCursor;
  NetworkSetSessionResult setSessionResult;
  FatalErrorCheckResult fatalCheckResult;
  NetworkOpenBindResult openBindResult;
  
  selectedBackendIndex = UiPointerList_GetSelectedIndex(backendList);
  if (g_NetworkBackendInstanceCount <= selectedBackendIndex) {
    return;
  }
  g_NetworkBackendSlot3(); /* close */
  g_NetworkBackendSlot1(); /* cleanup */
  setSessionResult = g_NetworkBackendSlot0(selectedBackendIndex);
  fatalCheckResult = FatalError_ReportIfFailed(setSessionResult.valueOrError,setSessionResult.failed);
  if (!fatalCheckResult.failed) {
    openBindResult = g_NetworkBackendSlot2(NETWORK_GAME_UDP_PORT);
    fatalCheckResult = FatalError_ReportIfFailed(openBindResult.valueOrError,openBindResult.failed);
    if (!fatalCheckResult.failed) {
      endpointSourceDwordCursor = (uint32_t *)&g_NetworkLocalEndpointDescriptor16;
      endpointDestinationDwordCursor = (uint32_t *)&g_FrontendNetworkEndpointScratch;
      for (remainingDwords = sizeof(UiTransferEndpointDescriptor) / sizeof(uint32_t); remainingDwords != 0;
           remainingDwords--) {
        *endpointDestinationDwordCursor = *endpointSourceDwordCursor;
        endpointSourceDwordCursor++;
        endpointDestinationDwordCursor++;
      }
      g_NetworkBackendSlot7
                (&g_FrontendNetworkEndpointTextUtf16,
                 (WinSockAddress *)&g_FrontendNetworkEndpointScratch);
      UiNodeList_SuppressActionId(FRONTEND_ACTION_JOIN_GAME,&THANDOR_CONTAINER_OF(backendList, FrontendNetworkSetupPageState, backendList)->rootNode);
      UiPointerList_InitializeColumnLayout
                (0,g_FrontendSessionListRows,&THANDOR_CONTAINER_OF(backendList, FrontendNetworkSetupPageState, backendList)->sessionList);
      UiTransfer_SendDiscoveryProbe();
      return;
    }
    g_NetworkBackendSlot1(); /* cleanup */
  }
  setSessionResult = g_NetworkBackendSlot0(selectedBackendIndex);
  if (!setSessionResult.failed) {
    openBindResult = g_NetworkBackendSlot2(NETWORK_GAME_UDP_PORT);
    if (!openBindResult.failed) {
      return;
    }
    g_NetworkBackendSlot1(); /* cleanup */
  }
  if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_NETWORKED_MASK) ==
      SESSION_NETWORK_ROLE_LOCAL) {
    FrontendSession_ReturnToMainPage(g_LocalPlayerRuntimeId,0,0,0);
  }
  else {
    FrontendCommandQueue_EnqueueLocalPlayerCommand(FRONTEND_COMMAND_RETURN_TO_MAIN_PAGE,0,0,0);
  }
  Random_SelectPrimaryStream();
  return;
}


/* Address: 0x00565A30.
   End of a mission: plays the end movie chosen by the current scenario's record in the loaded campaign
   (flm\endeNNNN.flm for the outcome g_EndMovieSelectionIndex and variant g_EndMovieVariantIndex) on the
   in-game root, then shows the results page (per-faction scores, elapsed time, level title) until a results
   button sets UI_COMMAND_RUNTIME_FLAG_RESULTS_CLOSED. Without an in-game root or end movie path, or when the
   movie cannot be opened, it only installs the results-screen callbacks.
*/
void Frontend_PlaySelectedEndMovie(void)

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
  g_GraphicsCursorSetFrame(0);
  g_CursorVisibilityToken--;
  if ((runtimeRoot != NULL) &&
     (rootCallbacks = runtimeRoot->rootUi0000.callbacks, g_EndMoviePath != NULL)) {
    rootCallbacks->keyboardFallback = EndMovieUiRuntime_DispatchCommandByFlags;
    rootCallbacks->frameUpdate = EndMovieUiRuntime_HandleModeTransition;
    /* Campaign level records (CampaignLevelRecord) as in OldUnitRuntime_RebuildScenarioReplayTables: the end
       movie number per outcome for a nonzero / zero variant index. */
    if (g_FrontendLoadedCampaignAsset != 0) {
      countOrActiveFactions = ((CampaignAsset *)g_FrontendLoadedCampaignAsset)->levelRecordCount;
      recordCursorOrRemaining = (int)((CampaignAsset *)g_FrontendLoadedCampaignAsset)->levels;
      do {
        if (((CampaignAsset *)g_FrontendLoadedCampaignAsset)->currentLevelId == ((CampaignLevelRecord *)recordCursorOrRemaining)->levelId) {
          if (g_EndMovieVariantIndex == 0) {
            endMovieNumber = *(int32_t *)(recordCursorOrRemaining + offsetof(CampaignLevelRecord,endMovieNumbers) +
                                         g_EndMovieSelectionIndex * 4);
          }
          else {
            endMovieNumber = *(int32_t *)(recordCursorOrRemaining + offsetof(CampaignLevelRecord,endMovieNumbersVariant) +
                                         g_EndMovieSelectionIndex * 4);
          }
          /* four zero-padded digits over the "0000" of flm\ende0000.flm */
          g_WideNumberFormatUtf16
                    (WIDE_FORMAT_PAD_WITH_ZERO,0,4,1,endMovieNumber,(uint16_t *)(u_flm_ende0000_flm_0050df4a + 8))
          ;
          g_EndMoviePath = (uint16_t *)u_flm_ende0000_flm_0050df4a;
          break;
        }
        recordCursorOrRemaining = (int)((CampaignLevelRecord *)recordCursorOrRemaining + 1);
        countOrActiveFactions--;
      } while (countOrActiveFactions != 0);
    }
    Movie_Close();
    /* clear both buffers to black */
    framebufferAccessFailed = g_GraphicsFramebufferBeginAccess();
    if (!framebufferAccessFailed) {
      g_GraphicsFramebufferFillRectArgb
                (g_FramebufferHeight,g_FramebufferWidth,0,0,g_FramebufferHeight,g_FramebufferWidth,0
                 ,0,0xff000000,g_FramebufferAccess);
      g_GraphicsFramebufferEndAccess();
      g_GraphicsFramebufferPresent(g_FramebufferAccess);
    }
    framebufferAccessFailed = g_GraphicsFramebufferBeginAccess();
    if (!framebufferAccessFailed) {
      g_GraphicsFramebufferFillRectArgb
                (g_FramebufferHeight,g_FramebufferWidth,0,0,g_FramebufferHeight,g_FramebufferWidth,0
                 ,0,0xff000000,g_FramebufferAccess);
      g_GraphicsFramebufferEndAccess();
      g_GraphicsFramebufferPresent(g_FramebufferAccess);
    }
    movieOpenResult = Movie_Open(1,g_EndMoviePath);
    runtimeRoot = g_InGameRuntimeRoot;
    if (!movieOpenResult.failed) {
      g_EndMoviePendingTicks = 0;
      playbackRateHz = movieOpenResult.playbackRateHz; /* ECX left by Movie_Open */
      g_TimerRegisterPeriodic(playbackRateHz,FrontendSession_PeriodicTick);
      /* EDX = g_InGameRuntimeRoot + 0x17C in the original; the decompiler lost it. */
      stack = (UiPageStackControl *)INGAME_UI(runtimeRoot,primaryPageStack);
      UiPageStack_SetActiveIndex(1,stack);
      frameAdvanceResult = Movie_AdvanceFrame();
      if (!frameAdvanceResult.ended) {
        runtimeRoot->activeEndMovieRuntime022C = (MovieRuntime *)frameAdvanceResult.movieOrError;
        runtimeRoot->endMoviePlaybackState0230 = 0;
        g_EndMoviePendingTicks = 0;
        /* one movie frame per timer tick until the movie ends (or the end-movie flag is cleared elsewhere) */
        do {
          if (g_EndMoviePendingTicks != 0) {
            g_EndMoviePendingTicks--;
            frameAdvanceResult = Movie_AdvanceFrame();
            if (frameAdvanceResult.ended) {
              g_UiCommandRuntimeFlags = g_UiCommandRuntimeFlags & ~UI_COMMAND_RUNTIME_FLAG_END_MOVIE_PENDING;
            }
          }
          UiNode_InvalidateRoot((UiNodeBase *)runtimeRoot);
          UiFrame_ProcessAndPresent();
        } while ((g_UiCommandRuntimeFlags & UI_COMMAND_RUNTIME_FLAG_END_MOVIE_PENDING) != 0);
      }
      g_CursorVisibilityToken++;
      UiPageStack_SetActiveIndex(1,&runtimeRoot->endMoviePageStack02F8);
      recordCursorOrRemaining = 7;
      factionLifecycleState = g_GameFactionRuntimeImage.tail.factionLifecycleStates;
      countOrActiveFactions = 0;
      factionIndex = 1;
      /* scores of every faction 1..7 that took part (lifecycle state not 0) */
      do {
        factionLifecycleState++;
        if (*factionLifecycleState != 0) {
          countOrActiveFactions++;
          GameFactionRuntime_RecomputeProgressAndScoreMetrics
                    (factionIndex,&runtimeRoot->worldRuntime0A30);
        }
        factionIndex++;
        recordCursorOrRemaining--;
      } while (recordCursorOrRemaining != 0);
      if (countOrActiveFactions != 0) {
        /* row counts of the three results lists */
        ((FrontendResultsColumnSequenceTemplate8_84 *)INGAME_UI(runtimeRoot,resultsChart1))->rowCount =
             countOrActiveFactions;
        ((FrontendResultsColumnSequenceTemplate8_84 *)INGAME_UI(runtimeRoot,resultsChart2))->rowCount =
             countOrActiveFactions;
        ((FrontendResultsColumnSequenceTemplate8_84 *)INGAME_UI(runtimeRoot,resultsChart3))->rowCount =
             countOrActiveFactions;
        /* elapsed minutes of the 80 Hz clock, rounded up, shown as hours and minutes */
        elapsedTimeUnits = (uint64_t)(g_GameFactionRuntimeImage.tail.periodicClockTick + 4799) / 4800;
        g_LocaleFormatTimeFieldsUtf16
                  ((uint32_t)(elapsedTimeUnits / 60),(uint32_t)(elapsedTimeUnits % 60),
                   (uint16_t *)&g_EndGameElapsedTimeScratchUtf16);
        resultsTextResult = TextResource_Resolve(TEXT_ID_RESULTS_TITLE_TEMPLATE);
        resourceId = g_InGameLevelTitleTextResourceIndex + TEXT_ID_LEVEL_TITLE_BASE;
        RichTextCommandStream_PatchPayloadBySelector(1,&g_EndGameElapsedTimeScratchUtf16,resultsTextResult.text)
        ;
        levelTitleResult = TextResource_Resolve(resourceId);
        RichTextCommandStream_PatchPayloadBySelector(0,levelTitleResult.text,resultsTextResult.text);
        /* the continue button; 0x1025 is the second results button, local games hide it */
        UiNodeList_UnsuppressActionId(INGAME_ACTION_RESULTS_CONTINUE,(UiNodeBase *)runtimeRoot);
        if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_NETWORKED_MASK) ==
            SESSION_NETWORK_ROLE_LOCAL) {
          UiNodeList_SuppressActionId(0x1025,(UiNodeBase *)runtimeRoot);
        }
        remainingPlayerBlocks = g_FrontendPlayerRuntimeBlockCount;
        playerBlock = g_FrontendPlayerRuntimeBlocks;
        /* a host with other players waits for them instead of offering continue */
        if (((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_HOST) != SESSION_NETWORK_ROLE_LOCAL)
           && (1 < g_FrontendPlayerRuntimeBlockCount)) {
          UiNodeList_SuppressActionId(INGAME_ACTION_RESULTS_CONTINUE,(UiNodeBase *)runtimeRoot);
          remainingPlayerBlocks = g_FrontendPlayerRuntimeBlockCount;
          playerBlock = g_FrontendPlayerRuntimeBlocks;
        }
        do {
          playerBlock->factionAssignment.readyOrWaitState = 0;
          remainingPlayerBlocks--;
          playerBlock++;
        } while (remainingPlayerBlocks != 0);
        if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_NETWORKED_MASK) ==
            SESSION_NETWORK_ROLE_LOCAL) {
          /* local game: remove column 2 (the third) of resultsChart1; the shift copies the count - 3 later ones */
          previousResultCount =
               ((FrontendResultsColumnSequenceTemplate8_84 *)INGAME_UI(runtimeRoot,resultsChart1))->columnTypeCount;
          ((FrontendResultsColumnSequenceTemplate8_84 *)INGAME_UI(runtimeRoot,resultsChart1))->columnTypeCount =
               ((FrontendResultsColumnSequenceTemplate8_84 *)INGAME_UI(runtimeRoot,resultsChart1))->columnTypeCount - 1;
          countOrActiveFactions = previousResultCount - 3;
          if (2 < previousResultCount && countOrActiveFactions != 0) {
            copySource =
                 (uint8_t *)&((FrontendResultsColumnSequenceTemplate8_84 *)INGAME_UI(runtimeRoot,resultsChart1))->
                 columnTypes[3];
            copyDestination =
                 (uint8_t *)&((FrontendResultsColumnSequenceTemplate8_84 *)INGAME_UI(runtimeRoot,resultsChart1))->
                 columnTypes[2];
            for (; countOrActiveFactions != 0; countOrActiveFactions--) {
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
            FrontendPlayerRuntime_MarkResultsReadyAndUpdateContinueButton(0xffffffff);
          }
        } while ((g_UiCommandRuntimeFlags & UI_COMMAND_RUNTIME_FLAG_RESULTS_CLOSED) == 0);
      }
      g_TimerUnregisterPeriodic(FrontendSession_PeriodicTick);
      Movie_Close();
    }
    else {
      g_CursorVisibilityToken++;
    }
  }
  else {
    g_CursorVisibilityToken++;
  }
  rootCallbacks = g_InGameRuntimeRoot->rootUi0000.callbacks;
  rootCallbacks->keyboardFallback = InGameHotkeys_DispatchCommandByFlags;
  rootCallbacks->frameUpdate = EndGameResultsUiRuntime_UpdateAndHandleInput;
  return;
}


/* Address: 0x00546700.
   Builds the frontend (menu) at the ROM record initialRomRecordId: clears the screen, loads the central
   texture set, palette, menu sounds (sound\menueNN.sam until the first missing one), engine\zentrale.rom and
   the menu music, creates the 0x5954-byte frontend root from its template and pushes it on the UI root stack,
   installs the 3D menu-room callbacks and activates the record's camera transition. It then reports this
   player ready and draws frames until every player is ready (network sessions wait here for the peers).
   Returns the frontend root, or CF set with the failing call's error; FrontendRuntime_ShutdownAndReleaseResourcesRegs
   undoes it.
*/
FrontendInitResult Frontend_Init(RomRecordId initialRomRecordId)

{
  SessionNetworkRoleFlags pendingBlockCountOrRoleMask;
  IDirectSoundBuffer *musicBuffer;
  uint32_t settingValue;
  SoundSampleAsset *loadedSample;
  FrontendRootResourceSlots *fillCursorOrResult;
  FrontendRootResourceSlots *frontendUiState;
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

  settingValue = PersistentSettings_Read(0,PERSISTENT_SETTING_TEXTURE_QUALITY);
  g_TextureDownsampleShift = settingValue >> 1;
  /* network session (the mask test is the first loop condition): every player starts not ready */
  playerBlock = g_FrontendPlayerRuntimeBlocks;
  remainingBlockCount = g_FrontendPlayerRuntimeBlockCount;
  pendingBlockCountOrRoleMask = g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_NETWORKED_MASK;
  while (pendingBlockCountOrRoleMask != SESSION_NETWORK_ROLE_LOCAL) {
    playerBlock->factionAssignment.readyOrWaitState = 0;
    playerBlock->commandSyncPending = FRONTEND_COMMAND_SYNC_PENDING;
    playerBlock++;
    remainingBlockCount--;
    pendingBlockCountOrRoleMask = remainingBlockCount;
  }
  callFailed = g_GraphicsFramebufferBeginAccess();
  if (!callFailed) {
    g_GraphicsFramebufferFillRectArgb
              (g_FramebufferHeight,g_FramebufferWidth,0,0,g_FramebufferHeight,g_FramebufferWidth,0,0
               ,0xff000000,g_FramebufferAccess); /* opaque black */
    g_GraphicsFramebufferEndAccess();
  }
  g_GraphicsFramebufferPresent(g_FramebufferAccess);
  g_GraphicsCursorSetFrame(GRAPHICS_CURSOR_FRAME_BUSY);
  g_CursorVisibilityToken++;
  g_FrontendRomTransitionTargetRecordId = 0;
  g_FrontendPendingPageAction = FRONTEND_PAGE_ACTION_NONE;
  g_FrontendRuntimeFlags = FRONTEND_RUNTIME_FLAG_WAITING_FOR_PLAYERS;
  g_FrontendTimerCountdownTicks = FRONTEND_TIMER_TICKS_PER_NETWORK_TICK;
  g_FrontendStateTickSpinLock = 0;
  g_TimerRegisterPeriodic(0x50,FrontendRuntime_TimerCountdownTick);
  UiRuntime_SetSynchronizationHooks(Frontend_StateTick,&g_FrontendStateTickSpinLock);
  textureSetResult = g_GraphicsTextureSetLoadPackage((uint16_t *)u_gfx_texturen_zentrale_gfx_00545acc);
  fillCursorOrResult = (FrontendRootResourceSlots *)textureSetResult.textureSet;
  if (!textureSetResult.failed) {
    g_FrontendCentralTextureSet = (FrontendRootResourceSlots *)textureSetResult.textureSet;
    paletteResult = g_GraphicsPaletteAssetLoadPackage((uint16_t *)u_gfx_texturen_zentrale_pal_00545b00);
    fillCursorOrResult = (FrontendRootResourceSlots *)paletteResult.paletteAsset;
    if (!paletteResult.failed) {
      g_FrontendCentralPaletteAsset = (FrontendRootResourceSlots *)paletteResult.paletteAsset;
      endpointTextResult = TextResource_Resolve(TEXT_ID_NETWORK_ADDRESS_TEMPLATE);
      RichTextCommandStream_PatchPayloadBySelector(0,&g_FrontendNetworkEndpointTextUtf16,endpointTextResult.text)
      ;
      /* Menu sounds: count the two digits of "sound\menue01.sam" from 01 up to 99 into the voice-set table
         slots 1..99 and stop at the first file that does not exist (0x3A is the character after '9'). */
      u_sound_menue01_sam_00545b54[0xb] = L'0';
      u_sound_menue01_sam_00545b54[0xc] = L'1';
      menuSoundVoiceSetSlotDwords = &g_FrontendMenuSoundVoiceSetLoadBaseEntry1;
      do {
        do {
          sampleLoadResult = Resource_Load((uint16_t *)u_sound_menue01_sam_00545b54);
          loadedSample = (SoundSampleAsset *)sampleLoadResult.bufferOrError;
          if (sampleLoadResult.failed) goto loadCentralRom;
          voiceSetResult = g_SoundCreateSampleVoiceSet(loadedSample);
          fillCursorOrResult = (FrontendRootResourceSlots *)voiceSetResult.voiceSet;
          if (voiceSetResult.failed) {
            /* the asm swaps the stacked sample with the error (XCHG [ESP],EAX) to release it */
            Resource_Release(loadedSample);
            goto fail;
          }
          *menuSoundVoiceSetSlotDwords = (uint32_t)fillCursorOrResult;
          Resource_Release(loadedSample);
          u_sound_menue01_sam_00545b54[0xc] = u_sound_menue01_sam_00545b54[0xc] + 1;
          menuSoundVoiceSetSlotDwords++;
        } while ((uint16_t)u_sound_menue01_sam_00545b54[0xc] < 0x3a);
        u_sound_menue01_sam_00545b54[0xb] = u_sound_menue01_sam_00545b54[0xb] + 1;
        u_sound_menue01_sam_00545b54[0xc] = L'0';
      } while ((uint16_t)u_sound_menue01_sam_00545b54[0xb] < 0x3a);
loadCentralRom:
      romLoadResult = Package_LoadEntry((uint16_t *)u_engine_zentrale_rom_00545aa4);
      fillCursorOrResult = romLoadResult.bufferOrError;
      if (!romLoadResult.failed) {
        g_FrontendCentralRomAsset = fillCursorOrResult;
        statusResult = RomAsset_PrepareRecords((RomAssetHeader *)fillCursorOrResult);
        fillCursorOrResult = (FrontendRootResourceSlots *)statusResult.valueOrError;
        if (!statusResult.failed) {
          allocResult = g_MemoryApi.alloc(0x10000);
          fillCursorOrResult = (FrontendRootResourceSlots *)allocResult.payloadOrError;
          if (!allocResult.failed) {
            /* 0x100 world object records of 0x100 bytes for the 3D menu room, zeroed */
            g_FrontendWorldObjectRecords = (WorldObjectRecord *)fillCursorOrResult;
            for (remainingDwords = 0x4000; remainingDwords != 0; remainingDwords--) {
              fillCursorOrResult->opaqueGap0000_05DF[0] = 0;
              fillCursorOrResult->opaqueGap0000_05DF[1] = 0;
              fillCursorOrResult->opaqueGap0000_05DF[2] = 0;
              fillCursorOrResult->opaqueGap0000_05DF[3] = 0;
              fillCursorOrResult = (FrontendRootResourceSlots *)(fillCursorOrResult->opaqueGap0000_05DF + 4);
            }
            allocResult = g_MemoryApi.alloc(0x5954);
            frontendUiState = (FrontendRootResourceSlots *)allocResult.payloadOrError;
            fillCursorOrResult = frontendUiState;
            if (!allocResult.failed) {
              worldRuntime = (WorldRuntimeContext *)FRONTEND_UI(frontendUiState,menuRoomModelView);
              frontendInitTemplateDwords = (uint32_t *)&g_FrontendRootInitializationTemplate;
              g_FrontendRootNode = frontendUiState;
              for (remainingDwords = 0x1655; remainingDwords != 0; remainingDwords--) { /* 0x5954 bytes */
                *(uint32_t *)fillCursorOrResult->opaqueGap0000_05DF = *frontendInitTemplateDwords;
                frontendInitTemplateDwords = frontendInitTemplateDwords + 1;
                fillCursorOrResult = (FrontendRootResourceSlots *)(fillCursorOrResult->opaqueGap0000_05DF + 4);
              }
              FrontendMenu_BindSharedResources(frontendUiState);
              UiRootStack_Push(&g_UiRootCallbacks_0053DA70,(UiRootNode *)frontendUiState);
              settingValue = PersistentSettings_Read(PERSISTENT_SOUND_OPTION_DEFAULT,PERSISTENT_SETTING_SOUND_OPTION_FLAGS);
              musicBuffer = g_FrontendMusicActiveBuffer;
              if ((settingValue & PERSISTENT_SOUND_OPTION_MUSIC) != 0) {
                sampleLoadResult = Resource_Load((uint16_t *)u_sound_music00_sam_00545c4e);
                loadedSample = (SoundSampleAsset *)sampleLoadResult.bufferOrError;
                musicBuffer = g_FrontendMusicActiveBuffer;
                if (!sampleLoadResult.failed) {
                  voiceSetResult = g_SoundCreateSampleVoiceSet(loadedSample);
                  musicVoiceSet = voiceSetResult.voiceSet;
                  if (voiceSetResult.failed) {
                    Resource_Release(loadedSample);
                    musicBuffer = g_FrontendMusicActiveBuffer;
                  }
                  else {
                    g_FrontendMusicVoiceSet = musicVoiceSet;
                    Resource_Release(loadedSample);
                    settingValue = PersistentSettings_Read(0x8000,PERSISTENT_SETTING_MUSIC_GAIN);
                    playResult = g_SoundPlayLooping(settingValue,settingValue,musicVoiceSet);
                    musicBuffer = playResult.soundBuffer;
                    if (playResult.failed) {
                      g_SoundReleaseSampleVoiceSet(musicVoiceSet);
                      g_FrontendMusicVoiceSet = NULL;
                      musicBuffer = g_FrontendMusicActiveBuffer;
                    }
                  }
                }
              }
              g_FrontendMusicActiveBuffer = musicBuffer;
              /* Fill the network-backend list with the backends' display names (0x100 bytes apart). The row
                 pointer table is the storage from g_FrontendTaskAssignmentControlOffsets...offsets[1] on
                 (0x005434EC); the global's name does not describe this use. */
              settingValue = g_NetworkBackendInstanceCount;
              nameSlotOrSourceDwords = g_FrontendTaskAssignmentControlOffsets.primaryAndPadding.offsets;
              if (g_NetworkBackendInstanceCount != 0) {
                backendDisplayName = g_NetworkBackendInstanceTable->displayNameUtf16;
                remainingBackends = g_NetworkBackendInstanceCount;
                do {
                  nameSlotOrSourceDwords = nameSlotOrSourceDwords + 1;
                  *nameSlotOrSourceDwords = (uint32_t)backendDisplayName;
                  backendDisplayName = backendDisplayName + 0x80;
                  remainingBackends--;
                } while (remainingBackends != 0);
                UiPointerList_InitializeMeasuredTextRows
                          (settingValue,(void **)(g_FrontendTaskAssignmentControlOffsets.primaryAndPadding.
                                           offsets + 1),
                           (UiPointerListControl *)FRONTEND_UI(frontendUiState,networkProtocolList));
              }
              /* The 3D pointer-context control lives at root+0x368 (EBX = EDI+0x368 in the asm); the installed
                 handlers do not all match the generic callback field types, hence the casts. */
              pointerContext = (FrontendModelPointerContextRuntimeState17C *)worldRuntime;
              pointerContext->keyboardFallback =
                   (bool (*)(UiKeyboardStateMask,UiActionId,struct UiRootNode *))
                   FrontendRuntime_DispatchCommandByCodeAndModifierFlags;
              pointerContext->resolvedActionCallback104 =
                   (FrontendModelPointerResolvedActionProc *)FrontendRuntime_UpdatePointerContextAndSceneView;
              pointerContext->resolvedActionCallback108 =
                   (FrontendModelPointerResolvedActionProc *)FrontendRuntime_UpdatePointerContextAndSceneView;
              pointerContext->resolvedActionCallback10C =
                   (FrontendModelPointerResolvedActionProc *)FrontendMenuRoom_PressNoOp;
              pointerContext->resolvedActionCallback110 =
                   (FrontendModelPointerResolvedActionProc *)FrontendMenuRoom_DragNoOp;
              pointerContext->resolvedActionCallback114 =
                   (FrontendModelPointerResolvedActionProc *)FrontendMenuRoom_ExecuteClickedRomAction;
              pointerContext->transientClearCallbackOrFrontendStateB0 = 0;
              pointerContext->rightReleaseCallback118 =
                   (void (*)(FrontendModelPointerContextRuntimeState17C *))FrontendMenuRoom_StopCameraFlight;
              pointerContext->renderSpinLock = (RuntimeSpinLockValue *)&g_FrontendStateTickSpinLock;
              pointerContext->renderSpinLockReleaseCallback = Frontend_StateTick;
              RecentTextHistory_SortAndBuildPointerList
                        (5,(RecentTextHistoryPointerList *)
                           &((UiConditionalActionControl *)FRONTEND_UI(frontendUiState,chatMessageHistory))->lineCount);
              WorldRuntime_SetTerrainLightingConfiguration(0,0,0xffffffff,0,0,0,0,0,worldRuntime);
              WorldRuntime_AttachObjectArray(0x100,g_FrontendWorldObjectRecords,worldRuntime);
              callFailed = RomRuntime_BuildAllRegistryNodeTrees(worldRuntime);
              /* EAX when RomRuntime_BuildAllRegistryNodeTrees fails: its only failure source is
                 WorldObjectArray_AllocateFreeRecord's 0x14 (object array full), passed up unchanged through
                 RomRuntime_BuildNodeTreeRecursive. */
              fillCursorOrResult = (FrontendRootResourceSlots *)0x14;
              if (!callFailed) {
                statusResult = FrontendRomTransition_ActivateRecordById(initialRomRecordId,worldRuntime)
                ;
                fillCursorOrResult = (FrontendRootResourceSlots *)statusResult.valueOrError;
                if (!statusResult.failed) {
                  /* Saved player name into the name field and g_FrontendLocalPlayerNameUtf16 (which is also
                     the fallback), saved game name into the game-name field (10 dwords = 0x28 bytes each). */
                  nameSlotOrSourceDwords = PersistentSettings_GetRegionOrFallback
                                     (PERSISTENT_SETTINGS_NAME_BYTES,g_FrontendLocalPlayerNameUtf16,
                                      PERSISTENT_SETTING_PLAYER_NAME);
                  settingsCopySourceDwordsA = nameSlotOrSourceDwords;
                  settingsCopyDestDwordsA = (uint32_t *)((UiRequiredTextEditControl *)FRONTEND_UI(frontendUiState,playerNameEdit))->textBuffer;
                  for (remainingDwords = PERSISTENT_SETTINGS_NAME_BYTES / sizeof(uint32_t); remainingDwords != 0;
                       remainingDwords--) {
                    *settingsCopyDestDwordsA = *settingsCopySourceDwordsA;
                    settingsCopySourceDwordsA++;
                    settingsCopyDestDwordsA++;
                  }
                  playerNameDestDwords = (void *)g_FrontendLocalPlayerNameUtf16;
                  for (remainingDwords = PERSISTENT_SETTINGS_NAME_BYTES / sizeof(uint32_t); remainingDwords != 0;
                       remainingDwords--) {
                    *playerNameDestDwords = *nameSlotOrSourceDwords;
                    nameSlotOrSourceDwords++;
                    playerNameDestDwords++;
                  }
                  settingsCopySourceDwordsB =
                       PersistentSettings_GetRegionOrFallback
                                 (PERSISTENT_SETTINGS_NAME_BYTES,g_FrontendLocalPlayerNameUtf16,
                                  PERSISTENT_SETTING_GAME_NAME);
                  settingsCopyDestDwordsB = (uint32_t *)((UiRequiredTextEditControl *)FRONTEND_UI(frontendUiState,gameNameEdit))->textBuffer;
                  for (remainingDwords = PERSISTENT_SETTINGS_NAME_BYTES / sizeof(uint32_t); remainingDwords != 0;
                       remainingDwords--) {
                    *settingsCopyDestDwordsB = *settingsCopySourceDwordsB;
                    settingsCopySourceDwordsB++;
                    settingsCopyDestDwordsB++;
                  }
                  settingValue = PersistentSettings_Read(4,PERSISTENT_SETTING_NETWORK_PLAYER_COUNT);
                  ((UiRangeSliderControl *)FRONTEND_UI(frontendUiState,maxPlayersSlider))->value = settingValue;
                  UiFrame_FlushInputAndResetPendingTicks();
                  g_SpinLockAcquire(&g_FrontendStateTickSpinLock);
                  WorldMotionSpline_ClearCachedDerivatives();
                  g_TimerRegisterPeriodic(0x100,FrontendRomTransition_AdvanceElapsedTicks);
                  if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_NETWORKED_MASK) ==
                      SESSION_NETWORK_ROLE_LOCAL) {
                    FrontendPlayerRuntime_RecordReadyAndUpdateWaitState
                              (g_LocalPlayerRuntimeId,0,0,0);
                  }
                  else {
                    /* the same "ready" report, sent through the network command queue */
                    FrontendCommandQueue_EnqueueLocalPlayerCommand(FRONTEND_COMMAND_PLAYER_READY,0,0,0);
                  }
                  g_SpinLockRelease(&g_FrontendStateTickSpinLock);
                  do {
                    UiNode_InvalidateRoot((UiNodeBase *)frontendUiState);
                    UiFrame_Update(0);
                    UiFrame_Draw();
                    g_GraphicsFramebufferPresent(g_FramebufferAccess);
                    Frontend_StateTick();
                  } while ((g_FrontendRuntimeFlags & FRONTEND_RUNTIME_FLAG_WAITING_FOR_PLAYERS) != 0);
                  g_GraphicsCursorSetFrame(GRAPHICS_CURSOR_FRAME_ARROW);
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
fail:
  failureResult.failed = true;
  failureResult.frontendRootOrError = (uint32_t)fillCursorOrResult;
  return failureResult;
}


/* Address: 0x00547630.
   Network work of the frontend, run under the frontend tick spin lock (skipped while the lock is busy); it is
   also installed as the menu room's render-lock release callback. According to g_FrontendNetworkState
   it sends the periodic packets of the state and hands every received packet to the state's handler, at most
   once per FRONTEND_TIMER_TICKS_PER_NETWORK_TICK timer ticks (the session start states faster).
*/
void Frontend_StateTick(void)

{
  uint32_t frontendRoot; /* passed to the packet handlers */
  uint32_t previousTickCounter;
  bool callResult;
  RecordRingDiscardResult discardedRecord;

  callResult = g_SpinLockTryAcquire(&g_FrontendStateTickSpinLock);
  previousTickCounter = g_FrontendNetworkTickCounter;
  frontendRoot = g_FrontendRootNode;
  if (callResult) {
    return;
  }
  switch(g_FrontendNetworkState) {
  case FRONTEND_NETWORK_STATE_IDLE:
    if (g_FrontendTimerCountdownTicks != 0) goto unlock;
    g_FrontendNetworkTickCounter++;
    g_FrontendTimerCountdownTicks = FRONTEND_TIMER_TICKS_PER_NETWORK_TICK;
    break;
  case FRONTEND_NETWORK_STATE_BROWSING:
    if (g_FrontendTimerCountdownTicks == 0) {
      g_FrontendNetworkTickCounter++;
      g_FrontendTimerCountdownTicks = FRONTEND_TIMER_TICKS_PER_NETWORK_TICK;
      /* the packet goes out on 15 of every 16 ticks */
      if ((previousTickCounter & 0xf) != 0) {
        UiTransfer_SendDiscoveryProbe();
      }
      while (true) {
        discardedRecord = UiRuntimeRecordRing_DiscardOldest();
        if (discardedRecord.empty) break;
        FrontendTransfer_HandleSessionListAndJoinAckPackets
                  ((UiTransferEndpointDescriptor *)discardedRecord.endpointOrReadIndex,
                   (FrontendTransferPacketUnion *)discardedRecord.payloadOrReadIndex,frontendRoot);
      }
      FrontendDebugOverlay_RefreshCountersAndWorldCoordinates();
    }
    goto unlock;
  case FRONTEND_NETWORK_STATE_HOSTING:
    if (g_FrontendTimerCountdownTicks == 0) {
      g_FrontendNetworkTickCounter++;
      g_FrontendTimerCountdownTicks = FRONTEND_TIMER_TICKS_PER_NETWORK_TICK;
      FrontendTransfer_PublishHostSessionAndDispatchQueuedCommands(g_FrontendRootNode);
      while (true) {
        discardedRecord = UiRuntimeRecordRing_DiscardOldest();
        if (discardedRecord.empty) break;
        FrontendTransfer_HandleLobbyDiscoveryAndPlayerPackets
                  ((UiTransferEndpointDescriptor *)discardedRecord.endpointOrReadIndex,
                   (FrontendTransferPacketUnion *)discardedRecord.payloadOrReadIndex,frontendRoot);
      }
      FrontendDebugOverlay_RefreshCountersAndWorldCoordinates();
    }
    goto unlock;
  case FRONTEND_NETWORK_STATE_JOINED:
    if (g_FrontendTimerCountdownTicks == 0) {
      g_FrontendNetworkTickCounter++;
      g_FrontendTimerCountdownTicks = FRONTEND_TIMER_TICKS_PER_NETWORK_TICK;
      if ((previousTickCounter & 0xf) != 0) {
        FrontendTransfer_SendCapabilityHeartbeat();
      }
      while (true) {
        discardedRecord = UiRuntimeRecordRing_DiscardOldest();
        if (discardedRecord.empty) break;
        FrontendTransfer_HandleHostSessionAndCommandBatchPackets
                  ((UiTransferEndpointDescriptor *)discardedRecord.endpointOrReadIndex,
                   (FrontendTransferPacketUnion *)discardedRecord.payloadOrReadIndex,frontendRoot);
      }
      FrontendDebugOverlay_RefreshCountersAndWorldCoordinates();
    }
    goto unlock;
  case FRONTEND_NETWORK_STATE_HOST_STARTING:
    if (g_FrontendTimerCountdownTicks != 0) goto unlock;
    g_FrontendNetworkTickCounter++;
    g_FrontendTimerCountdownTicks = FRONTEND_TIMER_TICKS_PER_NETWORK_TICK;
    while (true) {
      discardedRecord = UiRuntimeRecordRing_DiscardOldest();
      if (discardedRecord.empty) break;
      FrontendNetwork_HandleHandshakeAndPlayerStatePackets
                ((UiTransferEndpointDescriptor *)discardedRecord.endpointOrReadIndex,
                 (FrontendTransferPacketUnion *)discardedRecord.payloadOrReadIndex,frontendRoot);
    }
    callResult = FrontendNetwork_HostTickCommandAndSnapshotTransfer(frontendRoot);
    if (callResult) {
      /* transfer still running: next tick at once */
      g_FrontendTimerCountdownTicks = 1;
      goto unlock;
    }
    break;
  case FRONTEND_NETWORK_STATE_CLIENT_STARTING:
    /* no pacing: works whenever a packet of this session has arrived */
    callResult = UiRuntimeRecordRing_ContainsId(g_FrontendSessionToken);
    if (!callResult) goto unlock;
    g_FrontendNetworkTickCounter++;
    do {
      discardedRecord = UiRuntimeRecordRing_DiscardOldest();
      if (discardedRecord.empty) break;
      callResult = FrontendTransfer_HandleGameplayCommandAndRosterPackets
                        ((UiTransferEndpointDescriptor *)discardedRecord.endpointOrReadIndex,
                         (FrontendTransferPacketUnion *)discardedRecord.payloadOrReadIndex,
                         frontendRoot);
    } while (!callResult);
    callResult = FrontendTransfer_ConsumeProcessedFlagFrontend();
    if (callResult) goto unlock;
  }
  if ((g_FrontendRuntimeFlags & FRONTEND_RUNTIME_FLAG_WAITING_FOR_PLAYERS) == 0) {
    FrontendDebugOverlay_RefreshCountersAndWorldCoordinates();
  }
unlock:
  g_SpinLockRelease(&g_FrontendStateTickSpinLock);
  return;
}


/* Address: 0x00543B70.
   Called by Frontend_Init for the freshly copied frontend UI: loads gfx\panel\menue.gfx as the texture of the
   menu panels (also kept in g_FrontendMenuTextureSource) and gives the buttons their click sounds, button
   sound voice sets 3 to 6 by control kind. Nothing is bound when the texture cannot be loaded.
*/
void FrontendMenu_BindSharedResources(FrontendRootResourceSlots *frontendUiState)

{
  DirectSoundVoiceSet *buttonVoiceSet;
  DirectSoundVoiceSet *buttonVoiceSet5;
  GraphicsTextureSourceAsset *menuTexture;
  int controlIndex;
  TextureSourceLoadResult textureLoadResult;
  
  textureLoadResult = g_GraphicsTextureSourceLoadPackageAsset((uint16_t *)u_gfx_panel_menue_gfx_00545b78);
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
    /* the seven faction, player and selection-row controls of the faction setup page (entries 1..7) */
    do {
      ((UiFramedTextButtonControl *)THANDOR_UI_AT(frontendUiState,g_FrontendTaskAssignmentControlOffsets.factionControls.offsets[controlIndex]))->activationSoundId =
           (uint32_t)buttonVoiceSet;
      ((UiFramedTextButtonControl *)THANDOR_UI_AT(frontendUiState,g_FrontendTaskAssignmentControlOffsets.playerControls.offsets[controlIndex]))->activationSoundId =
           (uint32_t)buttonVoiceSet;
      ((UiTextButtonControl *)THANDOR_UI_AT(frontendUiState,g_FrontendTaskAssignmentControlOffsets.selectionRows.offsets[controlIndex]))->activationSoundId =
           (uint32_t)buttonVoiceSet;
      buttonVoiceSet5 = g_UiButtonSoundVoiceSets7[5];
      controlIndex--;
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
   Handler of frontend command FRONTEND_COMMAND_CYCLE_FACTION_COLOUR (0x650), called directly by
   FrontendUiAction2044_Handler in a local game: advances the colour of faction row rowIndex + 1 (0-based index)
   by one, wrapping after 7 colours (8 when the requesting player has colourCycleFlags bit 0); the row's caption
   (faction colour name) and the level player slot's colour index (the field typed aiClassOrMode) move together.
   Nothing happens for an unknown player id.
*/
void FrontendFactionSetup_CycleFactionColour
          (FrontendIndexedSelectionArgument playerRuntimeId,uint32_t unusedArgument1,uint32_t unusedArgument2,
          FrontendFactionAssignmentIndex rowIndex)

{
  int *levelCycleCounterField;
  LevelPlayerSlotByteOffset32 playerSlotOffset;
  FrontendLoadedLevelRuntimeImage370 *loadedLevelAsset;
  uint32_t nextSelectionTextId;
  uint32_t playerRecordsRemaining;
  FrontendPlayerRuntimeRecord *playerRecordCursor;
  int selectionControlAddress;
  int selectionTextCycleLength;
  int *selectionCycleCounterField;
  
  loadedLevelAsset = g_FrontendLoadedLevelAsset;
  selectionTextCycleLength = 7;
  playerRecordsRemaining = g_FrontendPlayerRuntimeBlockCount;
  playerRecordCursor = g_FrontendPlayerRuntimeBlocks;
  do {
    if (playerRuntimeId == playerRecordCursor->playerRuntimeId) {
      if ((playerRecordCursor->colourCycleFlags & 1) != 0) {
        selectionTextCycleLength = 8;
      }
      playerSlotOffset = g_InGameLevelRuntimeGlobalBlock.playerSlotByteOffsets[rowIndex];
      selectionControlAddress =
           g_FrontendRootNode +
           g_FrontendTaskAssignmentControlOffsets.factionControls.offsets[rowIndex + 1];
      nextSelectionTextId = ((UiFramedTextButtonControl *)selectionControlAddress)->textResourceId + 1;
      selectionCycleCounterField =
           (int *)&((LevelPlayerSlotRecord *)((uint8_t *)g_FrontendLoadedLevelAsset->playerSlots + playerSlotOffset))->aiClassOrMode;
      *selectionCycleCounterField = *selectionCycleCounterField + 1;
      if (selectionTextCycleLength + (TEXT_ID_FACTION_NAME_BASE + 1U) <= nextSelectionTextId) {
        nextSelectionTextId = TEXT_ID_FACTION_NAME_BASE + 1;
        levelCycleCounterField = (int *)&((LevelPlayerSlotRecord *)((uint8_t *)loadedLevelAsset->playerSlots + playerSlotOffset))->aiClassOrMode;
        *levelCycleCounterField = *levelCycleCounterField - selectionTextCycleLength;
      }
      ((UiFramedTextButtonControl *)selectionControlAddress)->textResourceId = nextSelectionTextId;
      return;
    }
    playerRecordCursor++;
    playerRecordsRemaining--;
  } while (playerRecordsRemaining != 0);
  return;
}


/* Address: 0x00544640.
   Handler of frontend command FRONTEND_COMMAND_TOGGLE_FACTION_ACTIVE (0x6F0), called directly by
   FrontendUiAction2045_Handler in a local game: unless a player has chosen faction rowIndex + 1, toggles
   whether that faction takes part (FACTION_RUNTIME_LIFECYCLE_ACTIVE: computer or nobody) and refreshes the
   faction setup page.
*/
void FrontendFactionSetup_ToggleFactionActive
          (uint32_t playerRuntimeId,uint32_t unusedArgument1,uint32_t unusedArgument2,
          FrontendFactionAssignmentIndex rowIndex)

{
  FactionRuntimeLifecycleObservedState *lifecycleState;
  FrontendPlayerRuntimeBlockCount remainingPlayerBlocks;
  FrontendPlayerRuntimeRecord *playerBlock;
  
  remainingPlayerBlocks = g_FrontendPlayerRuntimeBlockCount;
  playerBlock = g_FrontendPlayerRuntimeBlocks;
  do {
    if (rowIndex + 1 == playerBlock->factionAssignment.factionAssignmentIndex) {
      return;
    }
    playerBlock++;
    remainingPlayerBlocks--;
  } while (remainingPlayerBlocks != 0);
  lifecycleState = g_GameFactionRuntimeImage.tail.factionLifecycleStates + rowIndex + 1;
  *lifecycleState = *lifecycleState ^ FACTION_RUNTIME_LIFECYCLE_ACTIVE;
  FrontendTaskAssignmentPage_RefreshFactionAndPlayerControls(g_FrontendRootNode);
  return;
}


/* Address: 0x005446A0.
   Handler of frontend command FRONTEND_COMMAND_CHOOSE_FACTION (0x750), called directly by
   FrontendUiAction2046_Handler in a local game: unless the row is inactive (FRONTEND_CONTROL_INACTIVE), the
   player chooses faction rowIndex + 1. For the local player the row's checkbox becomes the only one checked.
   The player's record (the first one in a local game) gets the faction and the next ready-state generation,
   which orders the choices, then the faction setup page is refreshed.
*/
void FrontendFactionSetup_ChooseFaction
          (FrontendIndexedSelectionArgument playerRuntimeId,uint32_t unusedArgument1,uint32_t unusedArgument2,
          FrontendFactionAssignmentIndex rowIndex)

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
                     g_FrontendTaskAssignmentControlOffsets.selectionRows.offsets[rowIndex + 1]);
  generationCursor = 7;
  if ((((UiSelectableControl *)selectedControl)->stateFlags & FRONTEND_CONTROL_INACTIVE) == 0) {
    if (playerRuntimeId == g_LocalPlayerRuntimeId) {
      /* an empty count-down from 7, kept from the original */
      do {
        generationCursor--;
      } while (generationCursor != 0);
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
      generationCursor = 0;
    }
    readyStateGeneration = g_FrontendFactionAssignmentReadyStateGeneration;
    remainingBlockCount = g_FrontendPlayerRuntimeBlockCount;
    /* network game: search the player's record; when the count runs out first, the first record is used */
    pendingBlockCountOrRoleMask = g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_NETWORKED_MASK;
    for (playerBlockCursor = g_FrontendPlayerRuntimeBlocks;
        (matchedPlayerBlock = g_FrontendPlayerRuntimeBlocks, pendingBlockCountOrRoleMask != SESSION_NETWORK_ROLE_LOCAL &&
        (generationCursor = remainingBlockCount, matchedPlayerBlock = playerBlockCursor, playerRuntimeId != playerBlockCursor->playerRuntimeId));
        playerBlockCursor++) {
      generationCursor = remainingBlockCount - 1;
      remainingBlockCount = generationCursor;
      pendingBlockCountOrRoleMask = generationCursor;
    }
    matchedPlayerBlock->factionAssignment.factionAssignmentIndex = rowIndex + 1;
    matchedPlayerBlock->factionAssignment.readyOrWaitState = readyStateGeneration;
    g_FrontendFactionAssignmentReadyStateGeneration++;
    FrontendTaskAssignmentPage_RefreshFactionAndPlayerControls(g_FrontendRootNode);
  }
  return;
}


/* Address: 0x00546190.
   Fills the frontend debug overlay texts: every 20th call the frames rendered since the last refresh and the
   draw calls, texture binds and texture reloads per frame (then all four counters restart), and on every call
   the menu camera's position and orientation, the cursor override position and the free arena bytes.
*/
void FrontendDebugOverlay_RefreshCountersAndWorldCoordinates(void)

{
  uint32_t freeArenaBytes;
  WideNumberDenominator32 denominator;
  WorldRuntimeContext *world;
  WorldVector0EaxEcxEdx12 worldVector0;
  WorldVector1EaxEcxEdx12 worldVector1;
  
  denominator = g_RenderedFrameCountSinceDebugRefresh;
  g_DebugOverlayCounterRefreshCountdown--;
  if (g_DebugOverlayCounterRefreshCountdown == 0) {
    g_DebugOverlayCounterRefreshCountdown = 20;
    WideNumber_FormatUtf16
              (WIDE_FORMAT_WRITE_TERMINATOR,0,10,1,g_RenderedFrameCountSinceDebugRefresh,
               g_FrontendDebugOverlayTextSlot00Utf16);
    /* per-frame averages with two decimals */
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
  freeArenaBytes = g_MemoryApi.queryFreeBytes();
  WideNumber_FormatUtf16
            (WIDE_FORMAT_WRITE_TERMINATOR|WIDE_FORMAT_HEXADECIMAL,0,10,1,freeArenaBytes,
             g_FrontendDebugOverlayTextSlot12Utf16); /* hexadecimal despite radix 10 */
  g_FrontendDebugOverlayTextSlot13Utf16[0] = 0;
  return;
}


/* Address: 0x005474E0.
   Tears down what Frontend_Init built, before a session starts, before the menu is rebuilt and when the game
   quits: removes the frame hooks and frontend timers, saves the root's state snapshot and pops/frees the
   frontend root, releases the ROM registry, world objects, central ROM, textures, palette, menu sounds and
   music, and flushes pending input. Preserves EAX, ECX and EDX.
*/
void FrontendRuntime_ShutdownAndReleaseResourcesRegs(void)

{
  UiRootNode *root;
  int voiceSetsRemaining;
  uint32_t *voiceSetCursor;

  UiRuntime_SetSynchronizationHooks(NULL,NULL);
  g_TimerUnregisterPeriodic(FrontendRuntime_TimerCountdownTick);
  g_TimerUnregisterPeriodic(FrontendRomTransition_AdvanceElapsedTicks);
  root = g_FrontendRootNode;
  g_CursorVisibilityToken--;
  if (g_FrontendRootNode != NULL) {
    FrontendTeardown_SaveStatusTextAndHostAddress(g_FrontendRootNode);
    UiRootStack_Pop(root);
    g_MemoryApi.free(root);
    g_FrontendRootNode = NULL;
  }
  FrontendRomRegistry_ClearAndReleaseNestedResources();
  g_MemoryApi.free(g_FrontendWorldObjectRecords);
  g_FrontendWorldObjectRecords = NULL;
  Resource_Release(g_FrontendCentralRomAsset);
  g_FrontendCentralRomAsset = NULL;
  GraphicsShadingRuntime_ClearRecordTable();
  g_GraphicsTextureSetReleasePackage(g_FrontendCentralTextureSet);
  g_GraphicsPaletteAssetLifecycleCallbacks3.releasePackage(g_FrontendCentralPaletteAsset);
  g_GraphicsTextureSourceLifecycleCallbacks3.releasePackage(g_FrontendMenuTextureSource);
  g_FrontendCentralTextureSet = NULL;
  g_FrontendCentralPaletteAsset = NULL;
  g_FrontendMenuTextureSource = NULL;
  /* all 100 menu sound slots (Frontend_Init fills 1..99) */
  voiceSetCursor = &g_FrontendMenuSoundVoiceSetTable100;
  voiceSetsRemaining = 100;
  do {
    if ((DirectSoundVoiceSet *)*voiceSetCursor != NULL) {
      g_SoundReleaseSampleVoiceSet((DirectSoundVoiceSet *)*voiceSetCursor);
    }
    *voiceSetCursor = 0;
    voiceSetCursor++;
    voiceSetsRemaining--;
  } while (voiceSetsRemaining != 0);
  g_SoundStopVoice(g_FrontendMusicActiveBuffer);
  g_SoundReleaseSampleVoiceSet(g_FrontendMusicVoiceSet);
  g_FrontendMusicActiveBuffer = NULL;
  g_FrontendMusicVoiceSet = NULL;
  SpriteAssetRegistry_Reset();
  UiFrame_FlushInputAndResetPendingTicks();
  return;
}


/* Address: 0x0050AD90.
   Finds the model under the pointer for the model pointer context's press, release, drag and move handlers:
   hit-tests every candidate model node with flag 2 that is a runtime model (and has flag 0x20 unless the
   context allows models without it). The winner is the nearest hit, or, unless the context compares by metric
   only, the hit whose model class has the highest priority, the nearer one on equal priority. Returns the
   node in EDX and its hit metric in EAX (NULL and WORLD_POINTER_NO_HIT without a hit).
*/
uint64_t FrontendModelPointerContext_FindBestEligibleModelHitTarget
                (int pointerY,int pointerX,FrontendModelPointerContextRuntimeState118 *context)

{
  ModelRuntimeNode *modelNode;
  uint32_t bestHitMetric;
  ModelRuntimeNode *bestModelNode;
  ModelHitTestResult hitTestResult;
  int candidatePriority;
  int bestPriority;

  bestModelNode = NULL;
  bestHitMetric = WORLD_POINTER_NO_HIT;
  for (modelNode = context->candidateModelListHead; modelNode != NULL;
      modelNode = (ModelRuntimeNode *)(modelNode->common).nextNode) {
    if ((modelNode->runtimeFlags & 2) != 0 && modelNode->ownerClassId == WORLD_OWNER_RUNTIME_MODEL &&
        ((context->contextFlags & FRONTEND_MODEL_POINTER_CONTEXT_ALLOW_MODEL_WITHOUT_RUNTIME_FLAG_20) != 0 ||
         (modelNode->runtimeFlags & 0x20) != 0)) {
      hitTestResult = ModelRuntimeNode_HitTestProjectedBoundsAndChildren
                        (pointerY,pointerX,modelNode,context);
      if (hitTestResult.missed) continue;
      if ((context->contextFlags & FRONTEND_MODEL_POINTER_CONTEXT_COMPARE_HITS_BY_METRIC_ONLY) != 0) {
        if ((int)bestHitMetric <= (int)hitTestResult.distanceQ12) continue;
      }
      else if (bestModelNode != NULL) {
        /* Higher model-class priority wins; equal priority falls back to the smaller hit metric. */
        candidatePriority =
             (int)(&g_RuntimeModelClassPriorityByModelClassId.modelClass00Priority)
                  [modelNode->runtimePayload.modelRuntime->definitionOrSavedId.runtimeDefinition->runtimeClassId4C];
        bestPriority =
             (int)(&g_RuntimeModelClassPriorityByModelClassId.modelClass00Priority)
                  [bestModelNode->runtimePayload.modelRuntime->definitionOrSavedId.runtimeDefinition->runtimeClassId4C];
        if (candidatePriority < bestPriority) continue;
        if ((candidatePriority == bestPriority) && ((int)bestHitMetric <= (int)hitTestResult.distanceQ12))
        continue;
      }
      bestHitMetric = hitTestResult.distanceQ12;
      bestModelNode = modelNode;
    }
  }
  /* EDX:EAX = best node : its hit metric. */
  return ((uint64_t)(uintptr_t)bestModelNode << 32) | (uint64_t)bestHitMetric;
}


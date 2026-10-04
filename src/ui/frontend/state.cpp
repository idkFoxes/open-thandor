/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/src/ui/frontend/state.cpp
 * Reverse engineering by idkFoxes 2026
 */

#include <thandor/ui/frontend/state.h>
#include <thandor/thandor.h>
#include <thandor/platform/bootstrap/image.h>

/* Module data. */

uint32_t g_FrontendNetworkTickCounter = 0;

uint32_t g_FrontendTimerCountdownTicks = 0;

/* 3 command records and the terminator record
   (commandCode 0) that ends the dispatcher's scan */
static UiCommandDispatchRecord g_FrontendCommandDispatchRecords_00_Code00030071_Modifier30[4] = {
    /* 0 */ {.commandCode = 0x30071, .modifierClassFlags = 0x30, .continuationEntryAddress = 0x548190},
    /* 1 */ {.commandCode = 0x20004, .modifierClassFlags = 0x30, .continuationEntryAddress = 0x548190},
    /* 2 */ {.commandCode = 0x20001, .modifierClassFlags = 0xC, .continuationEntryAddress = 0x548140},
    /* 3 */ {.commandCode = 0x0, .modifierClassFlags = 0x90909090, .continuationEntryAddress = 0x90909090}}; /* commandCode 0, the rest is the original's NOP fill */

/* Implementation ownership: ui/frontend/state. */

/* Periodic timer callback of the frontend (80 Hz): counts g_FrontendTimerCountdownTicks down to zero.
   Frontend_StateTick uses the countdown to pace its network polling.
*/
void __cdecl FrontendRuntime_TimerCountdownTick(void)

{
  if (g_FrontendTimerCountdownTicks != 0) {
    g_FrontendTimerCountdownTicks--;
  }
  return;
}

/* Periodic timer callback of the frontend (256 Hz): advances the clock of the menu camera flight while a ROM
   transition is pending; the flight's spline is evaluated at g_FrontendRomTransitionElapsedTicks.
*/
void __cdecl FrontendRomTransition_AdvanceElapsedTicks(void)

{
  if (g_FrontendRomTransitionTargetRecordId != 0) {
    g_FrontendRomTransitionElapsedTicks++;
  }
  return;
}

/* Keyboard fallback of the menu room's pointer context: looks the key up in the frontend hotkey table
   (commandCode + required modifier class, see KEYBOARD_STATE_*). Alt+Q and Alt+key 0x20004 leave the
   current menu: back to the main page, a network session is closed first; Ctrl+key 0x20001 on the faction
   setup page toggles bit 0 of the local player's colourCycleFlags (an eighth entry in the faction cycle,
   FrontendFactionSetup_CycleFactionColour). Returns true when the key is not in the table.
*/
Bool8 FrontendRuntime_DispatchCommandByCodeAndModifierFlags
          (UiKeyboardStateMask modifierFlags,UiActionId commandCode,void *frontendRuntime)

{
  /* Each dispatch record names its handler by continuationEntryAddress, which only serves as the case label
     of the switch below. root is g_FrontendRootNode. Returns true = not handled. */
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
    if (UiPageStack_ActivePageIndex((UiPageStackControl *)FRONTEND_UI(root,frontendPageStack)) ==
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
    uint16_t *text;
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
      ((FrontendModelPointerContext *)FRONTEND_UI(root,menuRoomModelView))->contextFlags &= ~FRONTEND_MENU_ROOM_RENDER_SUPPRESSED;
      g_FrontendPendingPageAction = FRONTEND_PAGE_ACTION_NONE;
      g_FrontendRomTransitionPageAction = 0;
      FrontendRomTransition_ActivateRecordById
                (FRONTEND_ROM_RECORD_MAIN_MENU,(WorldRuntimeContext *)FRONTEND_UI(root,menuRoomModelView));
      break;
    }
    /* Leaving a network session. An earlier transcription named bit 0 the host and bit 1 the client, but
       SESSION_NETWORK_ROLE_CLIENT is bit 0; the variable follows the enum. */
    {
      int wasClient = (g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_CLIENT) != 0;
      g_SessionNetworkRoleFlags = g_SessionNetworkRoleFlags & ~SESSION_NETWORK_ROLE_NETWORKED_MASK;
      g_FrontendNetworkState = FRONTEND_NETWORK_STATE_IDLE;
      g_FrontendScenarioInitializationCount = 0;
      g_NetworkBackendSlot3();
      g_NetworkBackendSlot1();
      UiPageStack_SetActiveIndex(FRONTEND_PAGE_MAIN,(UiPageStackControl *)FRONTEND_UI(root,frontendPageStack));
      ((FrontendModelPointerContext *)FRONTEND_UI(root,menuRoomModelView))->contextFlags &= ~FRONTEND_MENU_ROOM_RENDER_SUPPRESSED;
      g_FrontendPendingPageAction = FRONTEND_PAGE_ACTION_NONE;
      g_FrontendRomTransitionPageAction = 0;
      player = g_FrontendPlayerRuntimeBlocks;
      FrontendRomTransition_ActivateRecordById
                (FRONTEND_ROM_RECORD_MAIN_MENU,(WorldRuntimeContext *)FRONTEND_UI(root,menuRoomModelView));
      /* chat history notice with the first player's name */
      text = TextResource_Resolve(wasClient ? TEXT_ID_NETWORK_SESSION_LEFT : TEXT_ID_NETWORK_SESSION_CLOSED);
      RichTextCommandStream_PatchPayloadBySelector(0,&player->playerName,text);
      FrontendRecentTextHistory_InsertAndRebuild5(text);
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

/* Runs one record of the frontend ROM action table locally, with the activation sound (the direct-call form of
   the FRONTEND_COMMAND_EXECUTE_ROM_ACTION command handler).
*/
void FrontendState_DispatchCode(FrontendStatusCode romRecordIndex)

{
  FrontendRomActionTable_ExecuteRecord(0,0,false,romRecordIndex);
  return;
}

/* Network work of the frontend, run under the frontend tick spin lock (skipped while the lock is busy); it is
   also installed as the menu room's render-lock release callback. According to g_FrontendNetworkState
   it sends the periodic packets of the state and hands every received packet to the state's handler, at most
   once per FRONTEND_TIMER_TICKS_PER_NETWORK_TICK timer ticks (the session start states faster).
*/
void Frontend_StateTick(void)

{
  uintptr_t frontendRoot; /* passed to the packet handlers */
  uint32_t previousTickCounter;
  Bool8 callResult;
  void *packet;
  void *packetEndpoint;

  callResult = g_SpinLockTryAcquire((RuntimeSpinLockValue *)&g_FrontendStateTickSpinLock);
  previousTickCounter = g_FrontendNetworkTickCounter;
  frontendRoot = g_FrontendRootNode;
  if (callResult) {
    return;
  }
  switch(g_FrontendNetworkState) {
  case FRONTEND_NETWORK_STATE_IDLE:
    if (g_FrontendTimerCountdownTicks != 0) {
      g_SpinLockRelease((RuntimeSpinLockValue *)&g_FrontendStateTickSpinLock);
      return;
    }
    g_FrontendNetworkTickCounter++;
    g_FrontendTimerCountdownTicks = FRONTEND_TIMER_TICKS_PER_NETWORK_TICK;
    break;
  case FRONTEND_NETWORK_STATE_BROWSING:
    if (g_FrontendTimerCountdownTicks == 0) {
      g_FrontendNetworkTickCounter++;
      g_FrontendTimerCountdownTicks = FRONTEND_TIMER_TICKS_PER_NETWORK_TICK;
      /* the packet goes out on 15 of every 16 ticks */
      if ((previousTickCounter & 15) != 0) {
        UiTransfer_SendDiscoveryProbe();
      }
      while (UiRuntimeRecordRing_TakeOldest(&packet,&packetEndpoint)) {
        FrontendTransfer_HandleSessionListAndJoinAckPackets
                  ((UiTransferEndpointDescriptor *)packetEndpoint,(FrontendTransferPacketUnion *)packet,
                   frontendRoot);
      }
      FrontendDebugOverlay_RefreshCountersAndWorldCoordinates();
    }
    g_SpinLockRelease((RuntimeSpinLockValue *)&g_FrontendStateTickSpinLock);
    return;
  case FRONTEND_NETWORK_STATE_HOSTING:
    if (g_FrontendTimerCountdownTicks == 0) {
      g_FrontendNetworkTickCounter++;
      g_FrontendTimerCountdownTicks = FRONTEND_TIMER_TICKS_PER_NETWORK_TICK;
      FrontendTransfer_PublishHostSessionAndDispatchQueuedCommands(g_FrontendRootNode);
      while (UiRuntimeRecordRing_TakeOldest(&packet,&packetEndpoint)) {
        FrontendTransfer_HandleLobbyDiscoveryAndPlayerPackets
                  ((UiTransferEndpointDescriptor *)packetEndpoint,(FrontendTransferPacketUnion *)packet,
                   frontendRoot);
      }
      FrontendDebugOverlay_RefreshCountersAndWorldCoordinates();
    }
    g_SpinLockRelease((RuntimeSpinLockValue *)&g_FrontendStateTickSpinLock);
    return;
  case FRONTEND_NETWORK_STATE_JOINED:
    if (g_FrontendTimerCountdownTicks == 0) {
      g_FrontendNetworkTickCounter++;
      g_FrontendTimerCountdownTicks = FRONTEND_TIMER_TICKS_PER_NETWORK_TICK;
      if ((previousTickCounter & 15) != 0) {
        FrontendTransfer_SendCapabilityHeartbeat();
      }
      while (UiRuntimeRecordRing_TakeOldest(&packet,&packetEndpoint)) {
        FrontendTransfer_HandleHostSessionAndCommandBatchPackets
                  ((UiTransferEndpointDescriptor *)packetEndpoint,(FrontendTransferPacketUnion *)packet,
                   frontendRoot);
      }
      FrontendDebugOverlay_RefreshCountersAndWorldCoordinates();
    }
    g_SpinLockRelease((RuntimeSpinLockValue *)&g_FrontendStateTickSpinLock);
    return;
  case FRONTEND_NETWORK_STATE_HOST_STARTING:
    if (g_FrontendTimerCountdownTicks != 0) {
      g_SpinLockRelease((RuntimeSpinLockValue *)&g_FrontendStateTickSpinLock);
      return;
    }
    g_FrontendNetworkTickCounter++;
    g_FrontendTimerCountdownTicks = FRONTEND_TIMER_TICKS_PER_NETWORK_TICK;
    while (UiRuntimeRecordRing_TakeOldest(&packet,&packetEndpoint)) {
      FrontendNetwork_HandleHandshakeAndPlayerStatePackets
                ((UiTransferEndpointDescriptor *)packetEndpoint,(FrontendTransferPacketUnion *)packet,
                 frontendRoot);
    }
    callResult = FrontendNetwork_HostTickCommandAndSnapshotTransfer(frontendRoot);
    if (callResult) {
      /* transfer still running: next tick at once */
      g_FrontendTimerCountdownTicks = 1;
      g_SpinLockRelease((RuntimeSpinLockValue *)&g_FrontendStateTickSpinLock);
      return;
    }
    break;
  case FRONTEND_NETWORK_STATE_CLIENT_STARTING:
    /* no pacing: works whenever a packet of this session has arrived */
    callResult = UiRuntimeRecordRing_ContainsId(g_FrontendSessionToken);
    if (!callResult) {
      g_SpinLockRelease((RuntimeSpinLockValue *)&g_FrontendStateTickSpinLock);
      return;
    }
    g_FrontendNetworkTickCounter++;
    do {
      if (!UiRuntimeRecordRing_TakeOldest(&packet,&packetEndpoint)) break;
      callResult = FrontendTransfer_HandleGameplayCommandAndRosterPackets
                        ((UiTransferEndpointDescriptor *)packetEndpoint,(FrontendTransferPacketUnion *)packet,
                         frontendRoot);
    } while (!callResult);
    callResult = FrontendTransfer_ConsumeProcessedFlagForMenuTick();
    if (callResult) {
      g_SpinLockRelease((RuntimeSpinLockValue *)&g_FrontendStateTickSpinLock);
      return;
    }
  }
  if ((g_FrontendRuntimeFlags & FRONTEND_RUNTIME_FLAG_WAITING_FOR_PLAYERS) == 0) {
    FrontendDebugOverlay_RefreshCountersAndWorldCoordinates();
  }
  g_SpinLockRelease((RuntimeSpinLockValue *)&g_FrontendStateTickSpinLock);
  return;
}

/* Title marker of one level on the game selection page: highlighted when one of the other players (records
   1..) has the level's bit clear in scenarioAvailabilityMask0..2[maskWordIndex], else normal. */
static uint16_t FrontendScenarioList_LevelAvailabilityMarker(uint32_t maskWordIndex,uint32_t levelMaskBit)
{
  FrontendPlayerRuntimeRecord *player;
  FrontendPlayerRuntimeBlockCount playersRemaining;

  player = g_FrontendPlayerRuntimeBlocks;
  playersRemaining = g_FrontendPlayerRuntimeBlockCount;
  while (--playersRemaining != 0) {
    player++;
    if (((&player->scenarioAvailabilityMask0)[maskWordIndex] & levelMaskBit) == 0) {
      return FRONTEND_TEXT_STYLE_HIGHLIGHTED;
    }
  }
  return FRONTEND_TEXT_STYLE_NORMAL;
}

/* Frame update of the frontend root: the frameUpdate callback of g_FrontendUiRootCallbacks, which Frontend_Init
   pushes on the UI root stack. Sorts the chat history, runs the timeout tick of the current network state,
   plays the briefing movie in a loop and the movie view's mask pattern, shows the chat line only in network
   games, updates the 3D menu room and the cursor from the hovered control, and on the game selection page
   (single games tab) marks each level title that some other player does not have.
*/

void FrontendRoot_TickNetworkPagesMovieCursorAndScenarioState(UiRootNode *rootCallbackContext)

{
  FrontendNetworkListsRuntimeView *frontendRoot;
  uint32_t networkState;
  UiNodeBase *hoveredNode;
  ScenarioCatalogDisplayRecord *levelRecord;
  GraphicsCursorFrameIndex cursorFrame;
  uint32_t levelMaskBit;
  ScenarioCatalogRecordCount levelsRemaining;
  uint32_t maskWordIndex;
  uint32_t activePageIndex;
  uint16_t *markerText;
  uint32_t selectedTabIndex;
  uint16_t availabilityMarker;
  
  networkState = g_FrontendNetworkState;
  frontendRoot = (FrontendNetworkListsRuntimeView *)g_FrontendRootNode;
  RecentTextHistory_SortAndBuildPointerList
            (5,(RecentTextHistoryPointerList *)&((UiConditionalActionControl *)FRONTEND_UI(g_FrontendRootNode,chatMessageHistory))->lineCount);
  switch(networkState) {
  case FRONTEND_NETWORK_STATE_BROWSING:
    FrontendSessionList_DecrementExpiryAndCompactRows(frontendRoot);
    break;
  case FRONTEND_NETWORK_STATE_HOSTING:
    FrontendPlayerRuntime_DecrementExpiryAndCompactBlocks(frontendRoot);
    break;
  case FRONTEND_NETWORK_STATE_JOINED:
    FrontendTransfer_TickRequestTimeoutAndResetPage(frontendRoot);
    break;
  case FRONTEND_NETWORK_STATE_HOST_STARTING:
    FrontendPlayerRuntime_DecrementTimeoutsAndRemoveExpiredPeers();
    break;
  case FRONTEND_NETWORK_STATE_CLIENT_STARTING:
    FrontendNetwork_TickDisconnectTimeoutAndResetSession();
  }
  if ((g_FrontendRuntimeFlags & FRONTEND_RUNTIME_FLAG_WAITING_FOR_PLAYERS) == 0) {
    /* the briefing image's movie (set by FrontendMissionBriefingPage_Initialize) plays in a loop */
    if ((((UiImageActionControl *)FRONTEND_UI(frontendRoot,briefingImage))->textureSource != NULL) &&
       !Movie_AdvanceFrame(NULL,NULL)) {
      Movie_Rewind();
    }
    if (((UiSoftwareTexturePreviewControl *)FRONTEND_UI(frontendRoot,moviePlaybackView))->textureSource != NULL) {
      SoftwareMaskBuffer_AdvancePatternByPercentTick
                ((SoftwareMaskRuntimeView *)FRONTEND_UI(frontendRoot,moviePlaybackView));
    }
  }
  /* bottom bar: empty page in a local game, the chat input line in a network game */
  if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_NETWORKED_MASK) ==
      SESSION_NETWORK_ROLE_LOCAL) {
    UiPageStack_SetActiveIndex(0,(UiPageStackControl *)FRONTEND_UI(frontendRoot,chatInputSlot));
  }
  else {
    UiPageStack_SetActiveIndex(1,(UiPageStackControl *)FRONTEND_UI(frontendRoot,chatInputSlot));
  }
  (*g_FrontendModelPointerContextVtable.pointerMove)
            (g_CursorOverrideY,g_CursorOverrideX,FRONTEND_UI(frontendRoot,menuRoomModelView));
  hoveredNode = (*((UiNodeBase *)frontendRoot)->vtable->hitTest)
                    (g_CursorOverrideY,g_CursorOverrideX,(UiNodeBase *)frontendRoot);
  if (hoveredNode == UI_NODE_NONE) {
    g_GraphicsCursorSetFrame(0);
  }
  else {
    cursorFrame = hoveredNode->vtable->pointerMove(g_CursorOverrideY,g_CursorOverrideX,hoveredNode);
    g_GraphicsCursorSetFrame(cursorFrame);
  }
  if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_CLIENT) == SESSION_NETWORK_ROLE_LOCAL) {
    g_FrontendPlayerRuntimeBlocks->capabilityFlags = FRONTEND_CAPABILITY_CD;
  }
  activePageIndex = UiPageStack_ActivePageIndex
                     ((UiPageStackControl *)FRONTEND_UI(frontendRoot,frontendPageStack));
  if (activePageIndex == FRONTEND_PAGE_STACK_CHOOSE_GAME) {
    selectedTabIndex = UiSelectableGroup_SelectedIndex(3,
      FRONTEND_UI(g_FrontendRootNode,loadGameTabButton),
      FRONTEND_UI(g_FrontendRootNode,singleGameTabButton),
      FRONTEND_UI(g_FrontendRootNode,campaignsTabButton));
    /* none selected gives 3, never the single-games tab */
    if ((selectedTabIndex == SCENARIO_SELECTION_TAB_SINGLE_GAMES) && (g_ScenarioCatalog != NULL)) {
      levelsRemaining = g_ScenarioCatalog->levelRecordCount;
      levelRecord = (ScenarioCatalogDisplayRecord *)
                    ((uint8_t *)g_ScenarioCatalog + g_ScenarioCatalog->levelRecordsOffset);
      /* level n has bit n of the players' scenarioAvailabilityMask0..2; its title starts with the rich-text
         code 0x8001 (highlighted) when one of the other players (records 1..) lacks it, else 0x8000.
         The three mask words cover at most 96 levels; further levels are left unmarked. */
      levelMaskBit = 1;
      maskWordIndex = 0;
      while (levelsRemaining != 0) {
        availabilityMarker = FrontendScenarioList_LevelAvailabilityMarker(maskWordIndex,levelMaskBit);
        markerText = TextResource_Resolve(levelRecord->scenarioTextResourceId + TEXT_ID_LEVEL_TITLE_BASE);
        *markerText = availabilityMarker;
        levelRecord++;
        levelMaskBit = levelMaskBit * 2;
        if (levelMaskBit == 0) {
          maskWordIndex = maskWordIndex + 1;
          levelMaskBit = 1;
          if (2 < maskWordIndex) {
            break;
          }
        }
        levelsRemaining = levelsRemaining - 1;
      }
    }
  }
}

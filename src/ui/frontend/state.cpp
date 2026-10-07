/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/src/ui/frontend/state.cpp
 * Reverse engineering by idkFoxes 2026
 */

#include <thandor/core/bytes.h>
#include <thandor/ui/frontend/state.h>
#include <thandor/thandor.h>
#include <thandor/platform/bootstrap/image.h>
#include <thandor/ui/core/key_dispatch.h>

/* Module data. */

uint32_t g_FrontendNetworkTickCounter = 0;

std::atomic<uint32_t> g_FrontendTimerCountdownTicks{0};

/* The modifier classes of the frontend hotkey table. The original matcher sends a Shift-only class (0x03) into
   its Ctrl branch (Shift+Ctrl required); UiKeyModifierRule::ExactWithShift wants Shift alone there. No record
   uses a Shift-only class, so both agree on this table (OPEN_THANDOR_SELFTEST=keymatch, case R2'). */
static constexpr uint32_t FRONTEND_HOTKEY_CLASS_ALT = 0x30;
static constexpr uint32_t FRONTEND_HOTKEY_CLASS_CTRL = 0xC;
static constexpr bool FrontendHotkey_ClassIsNotShiftOnly(uint32_t classFlags)
{
  return ((classFlags & KEYBOARD_STATE_SHIFT) == 0) || ((classFlags & (KEYBOARD_STATE_CTRL | KEYBOARD_STATE_ALT)) != 0);
}
static_assert(FrontendHotkey_ClassIsNotShiftOnly(FRONTEND_HOTKEY_CLASS_ALT) &&
              FrontendHotkey_ClassIsNotShiftOnly(FRONTEND_HOTKEY_CLASS_CTRL),
              "a Shift-only class would differ between the original frontend matcher and ExactWithShift");

/* 3 command records and the terminator record
   (commandCode 0) that ends the dispatcher's scan */
static UiCommandDispatchRecord g_FrontendCommandDispatchRecords_00_Code00030071_Modifier30[4] = {
    /* 0 */ {.commandCode = 0x30071, .modifierClassFlags = FRONTEND_HOTKEY_CLASS_ALT, .continuationEntryAddress = 0x548190},
    /* 1 */ {.commandCode = 0x20004, .modifierClassFlags = FRONTEND_HOTKEY_CLASS_ALT, .continuationEntryAddress = 0x548190},
    /* 2 */ {.commandCode = 0x20001, .modifierClassFlags = FRONTEND_HOTKEY_CLASS_CTRL, .continuationEntryAddress = 0x548140},
    /* 3 */ {.commandCode = 0x0, .modifierClassFlags = 0x90909090, .continuationEntryAddress = 0x90909090}}; /* commandCode 0, the rest is the original's NOP fill */

/* Periodic timer callback of the frontend (80 Hz): counts g_FrontendTimerCountdownTicks down to zero.
   Frontend_StateTick uses the countdown to pace its network polling.
*/
void FrontendRuntime_TimerCountdownTick()

{
  uint32_t remainingTicks;

  /* decrement only while nonzero; the compare-exchange keeps a concurrent reload by the main thread intact */
  remainingTicks = g_FrontendTimerCountdownTicks.load();
  while ((remainingTicks != 0) &&
         !g_FrontendTimerCountdownTicks.compare_exchange_weak(remainingTicks,remainingTicks - 1)) {
  }
}

/* Periodic timer callback of the frontend (256 Hz): advances the clock of the menu camera flight while a ROM
   transition is pending; the flight's spline is evaluated at g_FrontendRomTransitionElapsedTicks.
*/
void FrontendRomTransition_AdvanceElapsedTicks()

{
  if (g_FrontendRomTransitionTargetRecordId != 0) {
    g_FrontendRomTransitionElapsedTicks.fetch_add(1);
  }
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
  FrontendUiImage *root = FrontendUi_Image(g_FrontendRootNode);
  uint32_t target;

  (void)frontendRuntime;
  record = UiCommandDispatch_Find(record,commandCode,modifierFlags,UiKeyModifierRule::ExactWithShift);
  if (record == nullptr) {
    return true;
  }
  target = (uint32_t)record->continuationEntryAddress;
  switch (target) {
  case 0x548140:
    if (UiPageStack_ActivePageIndex(UiLayoutContainerControl_AsPageStack(&FrontendUi_Image(root)->frontendPageStack)) ==
        FRONTEND_PAGE_FACTION_SETUP) {
      FrontendCommand_Issue<FrontendPlayerRuntime_XorStateMaskByPlayerId>(0,0,1);
    }
    break;
  case 0x548190: {
    FrontendPlayerRuntimeRecord *player;
    uint16_t *text;
    if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_NETWORKED_MASK) == 0) {
      /* local game with no campaign or scenario loaded: only queue UI action 0 */
      if ((g_FrontendLoadedCampaignAsset == nullptr) && (g_FrontendScenarioInitializationCount == 0)) {
        UiActionQueue_Enqueue(0,root);
        break;
      }
      Resource_Release(g_FrontendLoadedCampaignAsset);
      g_FrontendLoadedCampaignAsset = nullptr;
      g_FrontendScenarioInitializationCount = 0;
      g_FrontendNetworkState = FRONTEND_NETWORK_STATE_IDLE;
      g_NetworkBackendSlot3();
      g_NetworkBackendSlot1();
      UiPageStack_SetActiveIndex(FRONTEND_PAGE_MAIN,UiLayoutContainerControl_AsPageStack(&FrontendUi_Image(root)->frontendPageStack));
      FrontendUi_Image(root)->menuRoomModelView.contextFlags &= ~FRONTEND_MENU_ROOM_RENDER_SUPPRESSED;
      g_FrontendPendingPageAction = FRONTEND_PAGE_ACTION_NONE;
      g_FrontendRomTransitionPageAction = 0;
      FrontendRomTransition_ActivateRecordById
                (FRONTEND_ROM_RECORD_MAIN_MENU,FrontendModelPointerContext_AsWorldRuntime(&FrontendUi_Image(root)->menuRoomModelView));
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
      UiPageStack_SetActiveIndex(FRONTEND_PAGE_MAIN,UiLayoutContainerControl_AsPageStack(&FrontendUi_Image(root)->frontendPageStack));
      FrontendUi_Image(root)->menuRoomModelView.contextFlags &= ~FRONTEND_MENU_ROOM_RENDER_SUPPRESSED;
      g_FrontendPendingPageAction = FRONTEND_PAGE_ACTION_NONE;
      g_FrontendRomTransitionPageAction = 0;
      player = g_FrontendPlayerRuntimeBlocks;
      FrontendRomTransition_ActivateRecordById
                (FRONTEND_ROM_RECORD_MAIN_MENU,FrontendModelPointerContext_AsWorldRuntime(&FrontendUi_Image(root)->menuRoomModelView));
      /* chat history notice with the first player's name */
      text = TextResource_Resolve(wasClient ? TEXT_ID_NETWORK_SESSION_LEFT : TEXT_ID_NETWORK_SESSION_CLOSED);
      RichTextCommandStream_PatchPayloadBySelector(0,&player->playerName,text);
      FrontendRecentTextHistory_InsertAndRebuild5(text);
      if (!wasClient) {
        /* the player record becomes a fresh local one (same fields as the client path of
           FrontendNetworkSetupPage_InitializeBackendMode) */
        g_FrontendPlayerRuntimeBlockCount = 1;
        g_LocalPlayerRuntimeId = 0;
        /* first two code units, cleared as one dword */
        *reinterpret_cast<uint32_t *>(player->playerName.textUtf16) = 0;
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
}

/* Network work of the frontend, run under the frontend tick spin lock (skipped while the lock is busy); it is
   also installed as the menu room's render-lock release callback. According to g_FrontendNetworkState
   it sends the periodic packets of the state and hands every received packet to the state's handler, at most
   once per FRONTEND_TIMER_TICKS_PER_NETWORK_TICK timer ticks (the session start states faster).
*/
void Frontend_StateTick()

{
  uintptr_t frontendRoot; /* passed to the packet handlers */
  uint32_t previousTickCounter;
  Bool8 callResult;
  void *packet;
  void *packetEndpoint;

  callResult = g_SpinLockTryAcquire(&g_FrontendStateTickSpinLock);
  previousTickCounter = g_FrontendNetworkTickCounter;
  frontendRoot = g_FrontendRootNode;
  if (callResult) {
    return;
  }
  switch(g_FrontendNetworkState) {
  case FRONTEND_NETWORK_STATE_IDLE:
    if (g_FrontendTimerCountdownTicks != 0) {
      g_SpinLockRelease(&g_FrontendStateTickSpinLock);
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
                  (static_cast<UiTransferEndpointDescriptor *>(packetEndpoint),static_cast<FrontendTransferPacketUnion *>(packet),
                   frontendRoot);
      }
      FrontendDebugOverlay_RefreshCountersAndWorldCoordinates();
    }
    g_SpinLockRelease(&g_FrontendStateTickSpinLock);
    return;
  case FRONTEND_NETWORK_STATE_HOSTING:
    if (g_FrontendTimerCountdownTicks == 0) {
      g_FrontendNetworkTickCounter++;
      g_FrontendTimerCountdownTicks = FRONTEND_TIMER_TICKS_PER_NETWORK_TICK;
      FrontendTransfer_PublishHostSessionAndDispatchQueuedCommands(g_FrontendRootNode);
      while (UiRuntimeRecordRing_TakeOldest(&packet,&packetEndpoint)) {
        FrontendTransfer_HandleLobbyDiscoveryAndPlayerPackets
                  (static_cast<UiTransferEndpointDescriptor *>(packetEndpoint),static_cast<FrontendTransferPacketUnion *>(packet),
                   frontendRoot);
      }
      FrontendDebugOverlay_RefreshCountersAndWorldCoordinates();
    }
    g_SpinLockRelease(&g_FrontendStateTickSpinLock);
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
                  (static_cast<UiTransferEndpointDescriptor *>(packetEndpoint),static_cast<FrontendTransferPacketUnion *>(packet),
                   frontendRoot);
      }
      FrontendDebugOverlay_RefreshCountersAndWorldCoordinates();
    }
    g_SpinLockRelease(&g_FrontendStateTickSpinLock);
    return;
  case FRONTEND_NETWORK_STATE_HOST_STARTING:
    if (g_FrontendTimerCountdownTicks != 0) {
      g_SpinLockRelease(&g_FrontendStateTickSpinLock);
      return;
    }
    g_FrontendNetworkTickCounter++;
    g_FrontendTimerCountdownTicks = FRONTEND_TIMER_TICKS_PER_NETWORK_TICK;
    while (UiRuntimeRecordRing_TakeOldest(&packet,&packetEndpoint)) {
      FrontendNetwork_HandleHandshakeAndPlayerStatePackets
                (static_cast<UiTransferEndpointDescriptor *>(packetEndpoint),static_cast<FrontendTransferPacketUnion *>(packet),
                 frontendRoot);
    }
    callResult = FrontendNetwork_HostTickCommandAndSnapshotTransfer(frontendRoot);
    if (callResult) {
      /* transfer still running: next tick at once */
      g_FrontendTimerCountdownTicks = 1;
      g_SpinLockRelease(&g_FrontendStateTickSpinLock);
      return;
    }
    break;
  case FRONTEND_NETWORK_STATE_CLIENT_STARTING:
    /* no pacing: works whenever a packet of this session has arrived */
    callResult = UiRuntimeRecordRing_ContainsId(g_FrontendSessionToken);
    if (!callResult) {
      g_SpinLockRelease(&g_FrontendStateTickSpinLock);
      return;
    }
    g_FrontendNetworkTickCounter++;
    do {
      if (!UiRuntimeRecordRing_TakeOldest(&packet,&packetEndpoint)) break;
      callResult = FrontendTransfer_HandleGameplayCommandAndRosterPackets
                        (static_cast<UiTransferEndpointDescriptor *>(packetEndpoint),static_cast<FrontendTransferPacketUnion *>(packet),
                         frontendRoot);
    } while (!callResult);
    callResult = FrontendTransfer_ConsumeProcessedFlagForMenuTick();
    if (callResult) {
      g_SpinLockRelease(&g_FrontendStateTickSpinLock);
      return;
    }
  }
  if ((g_FrontendRuntimeFlags & FRONTEND_RUNTIME_FLAG_WAITING_FOR_PLAYERS) == 0) {
    FrontendDebugOverlay_RefreshCountersAndWorldCoordinates();
  }
  g_SpinLockRelease(&g_FrontendStateTickSpinLock);
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
  FrontendUiImage *frontendRoot;
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
  frontendRoot = FrontendUi_Image(g_FrontendRootNode);
  /* the text box's lineCount and textLines slots are the count and entries of a pointer list */
  RecentTextHistory_SortAndBuildPointerList
            (5,reinterpret_cast<RecentTextHistoryPointerList *>
                 (&FrontendUi_Image(g_FrontendRootNode)->chatMessageHistory.lineCount));
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
    /* briefingImage: an image action control, its template node is shorter than the class */
    if ((reinterpret_cast<UiImageActionControl *>(&FrontendUi_Image(frontendRoot)->briefingImage)->textureSource != nullptr) &&
       !Movie_AdvanceFrame(nullptr,nullptr)) {
      Movie_Rewind();
    }
    if (FrontendUi_Image(frontendRoot)->moviePlaybackView.textureSource != nullptr) {
      SoftwareMaskBuffer_AdvancePatternByPercentTick /* the mask view of the texture preview */
                (reinterpret_cast<SoftwareMaskRuntimeView *>(&FrontendUi_Image(frontendRoot)->moviePlaybackView));
    }
  }
  /* bottom bar: empty page in a local game, the chat input line in a network game */
  if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_NETWORKED_MASK) ==
      SESSION_NETWORK_ROLE_LOCAL) {
    UiPageStack_SetActiveIndex(0,UiLayoutContainerControl_AsPageStack(&FrontendUi_Image(frontendRoot)->chatInputSlot));
  }
  else {
    UiPageStack_SetActiveIndex(1,UiLayoutContainerControl_AsPageStack(&FrontendUi_Image(frontendRoot)->chatInputSlot));
  }
  (*g_FrontendModelPointerContextVtable.pointerMove)
            (g_CursorOverrideY,g_CursorOverrideX,&FrontendUi_Image(frontendRoot)->menuRoomModelView.base);
  hoveredNode = (*frontendRoot->frontendRoot.root.base.vtable->hitTest)
                    (g_CursorOverrideY,g_CursorOverrideX,&frontendRoot->frontendRoot.root.base);
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
                     (UiLayoutContainerControl_AsPageStack(&FrontendUi_Image(frontendRoot)->frontendPageStack));
  if (activePageIndex == FRONTEND_PAGE_STACK_CHOOSE_GAME) {
    selectedTabIndex = UiSelectableGroup_SelectedIndex(3,
      &FrontendUi_Image(g_FrontendRootNode)->loadGameTabButton.selectable.base,
      &FrontendUi_Image(g_FrontendRootNode)->singleGameTabButton.selectable.base,
      &FrontendUi_Image(g_FrontendRootNode)->campaignsTabButton.selectable.base);
    /* none selected gives 3, never the single-games tab */
    if ((selectedTabIndex == SCENARIO_SELECTION_TAB_SINGLE_GAMES) && (g_ScenarioCatalog != nullptr)) {
      levelsRemaining = g_ScenarioCatalog->levelRecordCount;
      levelRecord = Thandor_At<ScenarioCatalogDisplayRecord>(g_ScenarioCatalog,g_ScenarioCatalog->levelRecordsOffset);
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

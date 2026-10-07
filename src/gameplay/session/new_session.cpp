/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/src/gameplay/session/new_session.cpp
 * Reverse engineering by idkFoxes 2026
 */

#include <thandor/gameplay/session/new_session.h>
#include <thandor/thandor.h>
#include <thandor/platform/bootstrap/image.h>
#include <thandor/platform/debug/hooks.h>

/* Module data. */

uint32_t g_EndMovieVariantIndex = 0;

uint16_t *g_EndMoviePath = nullptr;

uint32_t g_HostCommandBatchSyncSentThisInterval = 0;

uint32_t g_EndGameResultsCurrentMusicTrackId = 0;

WorldObjectRecord *g_InGameWorldObjectRecords = nullptr;

uintptr_t g_InGameWorldRuntimeDwordArray256[256] = {}; /* SpatialSoundSlot pointers (WorldRuntimeContext.dwordArray) */

/* Failure exit of InGameRuntime_InitializeNewSession: closes the level movie (also when it was not opened yet),
   stores the error in *outError and returns false. */
static Bool8 InGameNewSession_Fail(uint32_t error,uint32_t *outError)

{
  Movie_Close();
  *outError = error;
  return false;
}

/* Resets the session state for a new game: session counters and end-movie state, clears the player-removal packet
   area and all selection blocks, sets up every player's frontend record and selection block (not ready, command
   sync pending, fresh timeout, faction and name), and starts the periodic step timer and the UI synchronization
   hooks.
*/
static void InGameNewSession_ResetSessionState()

{
  uint32_t *clearCursor;
  int remainingCount;
  FrontendPlayerRuntimeBlockCount remainingPlayers;
  FrontendPlayerRuntimeRecord *frontendPlayer;
  SelectionPlayerRuntimeBlock *selectionBlock;
  PlayerRuntimeId playerId;
  uint32_t factionIndex;
  uint32_t *nameSource;
  uint32_t *nameDestination;

  InGameSession_ResetTickState();
  g_TextureDownsampleShift = PersistentSettings_Read(0,PERSISTENT_SETTING_TEXTURE_QUALITY);
  g_EndMovieSelectionIndex = UINT32_MAX;
  g_EndMovieVariantIndex = 0;
  g_EndMoviePath = nullptr;
  /* clear the client packet buffers and all selection blocks (0x10230 dwords). The original clears the six packet
     buffers with one 0x280-byte fill over their contiguous memory range; they are separate variables here, so
     each is cleared on its own, in the original memory order. */
  memset(&g_FrontendClientPlayerRemovalPacket10007,0,sizeof(g_FrontendClientPlayerRemovalPacket10007));
  memset(g_FrontendClientPlayerCommandRecords,0,sizeof(g_FrontendClientPlayerCommandRecords));
  memset(g_FrontendClientCommandBatchPacketBuffer,0,sizeof(g_FrontendClientCommandBatchPacketBuffer));
  memset(&g_FrontendPacket10021Buffer,0,sizeof(g_FrontendPacket10021Buffer));
  memset(&g_FrontendPacket10022Buffer,0,sizeof(g_FrontendPacket10022Buffer));
  memset(&g_FrontendPacket10023Buffer,0,sizeof(g_FrontendPacket10023Buffer));
  clearCursor = reinterpret_cast<uint32_t *>(g_SelectionPlayerBlocks); /* dword clear */
  for (remainingCount = SELECTION_PLAYER_BLOCK_COUNT * sizeof(SelectionPlayerRuntimeBlock) / sizeof(uint32_t);
       remainingCount != 0; remainingCount--) {
    *clearCursor = 0;
    clearCursor++;
  }
  /* per player: not ready, command sync pending, fresh timeout; link its selection block and copy the name.
     Original quirk: the player count is tested only after the first player.
     The original runs the loop g_FrontendPlayerRuntimeBlockCount times; bounded here to the 8 selection blocks
     (and frontend player records) because a count of 0 or above 8 would run past both tables. */
  remainingPlayers = g_FrontendPlayerRuntimeBlockCount;
  if (remainingPlayers == 0 || remainingPlayers > SELECTION_PLAYER_BLOCK_COUNT) {
    Thandor_Log("new session: player count %u out of range, bounded to the %d selection blocks",
                (unsigned)remainingPlayers,SELECTION_PLAYER_BLOCK_COUNT);
    remainingPlayers = remainingPlayers == 0 ? 1 : SELECTION_PLAYER_BLOCK_COUNT;
  }
  selectionBlock = g_SelectionPlayerBlocks;
  frontendPlayer = g_FrontendPlayerRuntimeBlocks;
  for (; remainingPlayers != 0; remainingPlayers--) {
    playerId = frontendPlayer->playerRuntimeId;
    (frontendPlayer->factionAssignment).readyOrWaitState = 0;
    frontendPlayer->commandSyncPending = FRONTEND_COMMAND_SYNC_PENDING;
    frontendPlayer->heartbeatExpiryTicks = 1024;
    factionIndex = (frontendPlayer->factionAssignment).factionAssignmentIndex;
    g_SelectionPlayerRuntimeBlockPointers[playerId] = selectionBlock;
    selectionBlock->factionIndex = factionIndex;
    selectionBlock->simulationStepTicks = 1;
    /* Original quirk: 20 dwords (80 bytes) are copied although both name fields hold 20 UTF-16 characters
       (40 bytes), so the copy also covers the 40 bytes behind each of them (the start of the next selection block).
       The original copies 80 bytes for the last selection block as well; bounded here to the 40-byte name field
       because its excess would land behind the g_SelectionPlayerBlocks allocation. */
    nameSource = reinterpret_cast<uint32_t *>(frontendPlayer->playerName.textUtf16); /* dword copy */
    nameDestination = reinterpret_cast<uint32_t *>(selectionBlock->playerNameUtf16);
    remainingCount = 20;
    if (selectionBlock == g_SelectionPlayerBlocks + (SELECTION_PLAYER_BLOCK_COUNT - 1)) {
      remainingCount = (int)(sizeof(selectionBlock->playerNameUtf16) / (sizeof(uint32_t)));
    }
    for (; remainingCount != 0; remainingCount--) {
      *nameDestination = *nameSource;
      nameSource++;
      nameDestination++;
    }
    selectionBlock++;
    frontendPlayer++;
  }
  g_SessionTransferTimeoutTicks = 1024;
  InGameSession_InstallStepTimerAndHooks();
}

/* Whether the session name keeps titleChar: the characters Windows forbids in file names and '.' are dropped. */
static Bool8 InGameNewSession_IsSessionNameCharacter(uint16_t titleChar)

{
  switch (titleChar) {
  case '*':
  case '<':
  case '>':
  case '"':
  case '/':
  case '\\':
  case '.':
  case '?':
  case ':':
  case '|':
    return false;
  default:
    return true;
  }
}

/* Sets the text of the in-game template's save-name edit (saveNameEdit, 32 code units) to the level title without
   the characters dropped by InGameNewSession_IsSessionNameCharacter. The first title character is skipped and at
   most the next 31 are looked at. */
static void InGameNewSession_BuildSessionName(UiTextResourceId titleTextIndex)

{
  uint16_t *sessionNameCursor;
  uint16_t *titleSource;
  uint16_t titleChar;
  int remainingCount;

  sessionNameCursor = reinterpret_cast<UiRequiredTextEditControl *>
                        (&g_InGameRuntimeDefaultImageTemplate.saveNameEdit)->textBuffer;
  for (remainingCount = 32; remainingCount != 0; remainingCount--) {
    *sessionNameCursor = 0;
    sessionNameCursor++;
  }
  titleSource = TextResource_Resolve(titleTextIndex + TEXT_ID_LEVEL_TITLE_BASE);
  sessionNameCursor = reinterpret_cast<UiRequiredTextEditControl *>
                        (&g_InGameRuntimeDefaultImageTemplate.saveNameEdit)->textBuffer;
  for (remainingCount = 31; remainingCount != 0; remainingCount--) {
    titleSource++;
    titleChar = *titleSource;
    if (titleChar == 0) {
      break;
    }
    if (InGameNewSession_IsSessionNameCharacter(titleChar)) {
      *sessionNameCursor = titleChar;
      sessionNameCursor++;
    }
  }
}

/* Opens the level movie that plays while loading and shows its first frames, attaches the world arrays with the
   local player's faction, resets the game data defaults and loads the level resources, clears the notification
   queue and creates the terrain texture. Returns true on success; on failure returns false with the error in
   *outError.
*/
static Bool8 InGameNewSession_LoadWorld(LevelAssetRuntimePrefix *levelAsset,uint16_t *levelMoviePath,
                                       InGameRuntimeRoot *inGameRoot,uint32_t *outError)

{
  WorldRuntimeContext *world;
  PlayerRuntimeId localPlayerId;
  uint32_t localFactionIndex;
  uint32_t resetDefaultsError;
  uint32_t stepError;

  world = &inGameRoot->worldRuntime;
  if (!InGameSession_OpenLoadingMovieAndAttachObjects(levelMoviePath,&levelAsset->header,inGameRoot,outError)) {
    return false;
  }
  localPlayerId = g_LocalPlayerRuntimeId;
  localFactionIndex = g_SelectionPlayerRuntimeBlockPointers[g_LocalPlayerRuntimeId]->factionIndex;
  levelAsset->playerSlots[6].aiClassOrMode = localFactionIndex;
  world->activeFactionRuntimeIndex = localFactionIndex;
  world->selection.activePlayerRuntimeId = localPlayerId;
  WorldRuntime_AttachAndClearDwordArray
            (INGAME_WORLD_DWORD_ARRAY_COUNT,g_InGameWorldRuntimeDwordArray256,world);
  resetDefaultsError = GameData_ResetDefaults();
  if (resetDefaultsError != 0) {
    *outError = resetDefaultsError;
    return false;
  }
  if (!InGameLevelRuntime_LoadResourcesAfterDefaultReset(levelAsset,world,&stepError)) {
    *outError = stepError;
    return false;
  }
  return InGameSession_ClearNotificationsAndCreateTerrainTexture(inGameRoot,outError);
}

/* Takes the step spin lock and finishes the world while the step is held off: sets up the shading texture,
   mirrors the shading and mouse/panel options into the world runtime flags and the panel layout, carries campaign
   units over (or resets the pending unit tables), allocates the grid scratch and rebuilds the derived terrain,
   influence, technology, build/army/command grid and lighting data. Returns true on success with the lock still
   held; on failure returns false with the error in *outError.
   Original quirk: the spin lock is not released on failure.
*/
static Bool8 InGameNewSession_FinishWorldUnderTickLock(InGameRuntimeRoot *inGameRoot,uint32_t *outError)

{
  WorldRuntimeContext *world;
  PersistentMapMouseOptionFlags mapMouseOptionFlags;

  world = &inGameRoot->worldRuntime;
  g_SpinLockAcquire(&g_InGameStateTickSpinLock);
  InGameSession_InitShadingAndMirrorViewOptions(world);
  mapMouseOptionFlags = PersistentSettings_ReadMapMouseOptions();
  if (Any(mapMouseOptionFlags & PERSISTENT_MAP_OPTION_SIDE_PANEL_HIDDEN)) {
    UiPageStack_SetActiveIndex(1,&inGameRoot->sidePanelPageStack);
    UiPageStack_SetActiveIndex(0,&inGameRoot->resourceBarModePageStack);
    UiPageStack_SetActiveIndex(0,&inGameRoot->gamePanelsModePageStack);
    inGameRoot->worldViewAreaRightOffset = 0;
    UiContainer_LayoutChildren(&inGameRoot->rootUi.base);
  }
  /* a campaign carries units over from the previous level */
  if (g_FrontendLoadedCampaignAsset == nullptr) {
    OldUnitRuntime_ResetPendingTables();
  }
  else {
    DebugHook_CampaignCarryOver(0);
    OldUnitRuntime_MergeMasksAndReplayRecords();
    DebugHook_CampaignCarryOver(1);
  }
  if (!InGameSession_AllocateGridScratchAndRebuildDerived(world,outError)) {
    return false;
  }
  if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_NETWORKED_MASK) == SESSION_NETWORK_ROLE_LOCAL) {
    inGameRoot->worldViewWrappedTextNodeFlags = inGameRoot->worldViewWrappedTextNodeFlags | UI_NODE_SUPPRESSED;
  }
  else {
    inGameRoot->worldViewWrappedTextNodeFlags = inGameRoot->worldViewWrappedTextNodeFlags & ~UI_NODE_SUPPRESSED;
  }
  InGameSession_RebuildUiGrids(inGameRoot);
  WorldLightingRuntime_UpdateInterpolatedTerrainLighting();
  return true;
}

/* Starts the session notification timer and queues the level's five intro notification movies (consecutive ids
   from the first one; none when it is 0). */
static void InGameNewSession_QueueIntroNotifications()

{
  InGameNotificationMovieId firstMovieId;
  uint32_t introIndex;

  /* the first of the five level intro notification movies, from the level image */
  firstMovieId = g_InGameLevelRuntimeGlobalBlock.conditionStorage->levelImage.worldSettings.introNotificationMovieId;
  g_TimerRegisterPeriodic(10,InGameRuntime_ProcessQueuedSessionNotificationTimer);
  if (firstMovieId == 0) {
    return;
  }
  for (introIndex = 0; introIndex < 5; introIndex++) {
    InGameNotificationQueue_InsertPriorityRecord
              (NOTIFICATION_PAYLOAD_NONE,0,0,0,0,0,1,firstMovieId + introIndex);
  }
}

/* Starts a new game on a level: resets the session counters and the per-player blocks, installs the step timer
   and InGameRuntime_UpdateSimulationAndNetworkTick as the UI synchronization hook, builds the in-game UI root from
   its template, opens the level movie that plays while loading and loads the level (world, terrain, shading,
   technologies, units). It then reports itself ready to the other players and keeps drawing the player-status
   screen while stepping until every player is ready, and finally queues the level's five intro notifications.
   Returns true on success; on failure returns false and stores the error of the failing step in *outError.
*/
Bool8 InGameRuntime_InitializeNewSession(LevelAssetRuntimePrefix *levelAsset,uint16_t *levelMoviePath,
                                        uint32_t *outError)

{
  InGameRuntimeRoot *inGameRoot;
  uint32_t stepError;

  InGameNewSession_ResetSessionState();
  InGameNewSession_BuildSessionName((levelAsset->header).titleTextResourceIndex);
  /* the local player's block starts with its selection slots (SelectionPointerArray32): the same 32 entity
     pointers */
  if (!InGameSession_CreateRoot
         (reinterpret_cast<SelectionInfoEntitySlots *>(g_SelectionPlayerRuntimeBlockPointers[g_LocalPlayerRuntimeId]),
          &inGameRoot,&stepError)) {
    return InGameNewSession_Fail(stepError,outError);
  }
  if (!InGameNewSession_LoadWorld(levelAsset,levelMoviePath,inGameRoot,&stepError)) {
    return InGameNewSession_Fail(stepError,outError);
  }
  if (!InGameNewSession_FinishWorldUnderTickLock(inGameRoot,&stepError)) {
    return InGameNewSession_Fail(stepError,outError);
  }
  /* the spin lock taken by InGameNewSession_FinishWorldUnderTickLock is released here */
  InGameSession_ReportReadyAndWaitForPlayers(inGameRoot);
  g_GraphicsCursorSetFrame(GRAPHICS_CURSOR_FRAME_ARROW);
  InGameNewSession_QueueIntroNotifications();
  return true;
}

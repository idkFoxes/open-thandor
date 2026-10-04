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

uint16_t *g_EndMoviePath = 0;

uint32_t g_HostCommandBatchSyncSentThisInterval = 0;

uint32_t g_EndGameResultsCurrentMusicTrackId = 0;

WorldObjectRecord *g_InGameWorldObjectRecords = 0;

uintptr_t g_InGameWorldRuntimeDwordArray256[256] = {0}; /* SpatialSoundSlot pointers (WorldRuntimeContext.dwordArray) */

static uint8_t g_InGameSessionStartedNetworked = 0;

/* Implementation ownership: gameplay/session/new_session. */

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
static void InGameNewSession_ResetSessionState(void)

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

  g_UiCommandRuntimeFlags = UI_COMMAND_RUNTIME_FLAG_WAITING_FOR_PLAYERS | UI_COMMAND_RUNTIME_FLAG_PAUSED;
  g_SessionNetworkTickCounter = 1;
  g_HostCommandBatchSyncSentThisInterval = 0;
  g_GameFactionRuntimeImage.tail.simulationTick = 1;
  g_GameFactionRuntimeImage.tail.presentationTick = 0;
  g_InGameSessionNotificationTimeoutTicks = 0;
  g_InGameReadyStateToggleFlags = 0;
  g_EndGameResultsCurrentMusicTrackId = 0;
  g_InGameSelectedTechnologyId = TEC_000_BASIC_TECHNOLOGY;
  g_InGameSessionStartedNetworked =
       (g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_NETWORKED_MASK) !=
       SESSION_NETWORK_ROLE_LOCAL;
  g_TextureDownsampleShift = PersistentSettings_Read(0,PERSISTENT_SETTING_TEXTURE_QUALITY);
  g_EndMovieSelectionIndex = UINT32_MAX;
  g_EndMovieVariantIndex = 0;
  g_EndMoviePath = NULL;
  /* clear the client packet buffers and all selection blocks (0x10230 dwords). The original clears the six packet
     buffers with one 0x280-byte fill over their contiguous memory range; they are separate variables here, so
     each is cleared on its own, in the original memory order. */
  memset(&g_FrontendClientPlayerRemovalPacket10007,0,sizeof(g_FrontendClientPlayerRemovalPacket10007));
  memset(g_FrontendClientPlayerCommandRecords,0,sizeof(g_FrontendClientPlayerCommandRecords));
  memset(g_FrontendClientCommandBatchPacketBuffer,0,sizeof(g_FrontendClientCommandBatchPacketBuffer));
  memset(&g_FrontendPacket10021Buffer,0,sizeof(g_FrontendPacket10021Buffer));
  memset(&g_FrontendPacket10022Buffer,0,sizeof(g_FrontendPacket10022Buffer));
  memset(&g_FrontendPacket10023Buffer,0,sizeof(g_FrontendPacket10023Buffer));
  clearCursor = (uint32_t *)g_SelectionPlayerBlocks;
  for (remainingCount = SELECTION_PLAYER_BLOCK_COUNT * sizeof(SelectionPlayerRuntimeBlock) / sizeof(uint32_t);
       remainingCount != 0; remainingCount--) {
    *clearCursor = 0;
    clearCursor++;
  }
  /* per player: not ready, command sync pending, fresh timeout; link its selection block and copy the name.
     Original quirk: the player count is tested only after the first player. */
  remainingPlayers = g_FrontendPlayerRuntimeBlockCount;
  selectionBlock = g_SelectionPlayerBlocks;
  frontendPlayer = g_FrontendPlayerRuntimeBlocks;
  do {
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
    nameSource = (uint32_t *)frontendPlayer->playerName.textUtf16;
    nameDestination = (uint32_t *)selectionBlock->playerNameUtf16;
    remainingCount = 20;
    if (selectionBlock == g_SelectionPlayerBlocks + (SELECTION_PLAYER_BLOCK_COUNT - 1)) {
      remainingCount = (int)(sizeof(selectionBlock->playerNameUtf16) / (sizeof(uint32_t)));
    }
    for (; remainingCount != 0; remainingCount--) {
      *nameDestination = *nameSource;
      nameSource++;
      nameDestination++;
    }
    remainingPlayers--;
    selectionBlock++;
    frontendPlayer++;
  } while (remainingPlayers != 0);
  g_SessionTransferTimeoutTicks = 1024;
  g_InGameNetworkTickCountdown = INGAME_TIMER_TICKS_PER_SIMULATION_STEP;
  g_InGameStateTickSpinLock = 0;
  g_TimerRegisterPeriodic(INGAME_PERIODIC_TIMER_HZ,InGameRuntime_PeriodicCountdownAndClockTick);
  UiRuntime_SetSynchronizationHooks
            (InGameRuntime_UpdateSimulationAndNetworkTick,(RuntimeSpinLockValue *)&g_InGameStateTickSpinLock);
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

  sessionNameCursor = ((UiRequiredTextEditControl *)&g_InGameRuntimeDefaultImageTemplate.saveNameEdit)->textBuffer;
  for (remainingCount = 32; remainingCount != 0; remainingCount--) {
    *sessionNameCursor = 0;
    sessionNameCursor++;
  }
  titleSource = TextResource_Resolve(titleTextIndex + TEXT_ID_LEVEL_TITLE_BASE);
  sessionNameCursor = ((UiRequiredTextEditControl *)&g_InGameRuntimeDefaultImageTemplate.saveNameEdit)->textBuffer;
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

/* Allocates the zeroed world object pool, the selection info panel resources of the local player and the in-game
   root (a copy of g_InGameRuntimeDefaultImageTemplate with its control tree, world callbacks and camera limits) and
   pushes the root onto the UI root stack. Returns true with the root in *outRoot; on failure returns false with
   the error in *outError.
*/
static Bool8 InGameNewSession_CreateRoot(InGameRuntimeRoot **outRoot,uint32_t *outError)

{
  void *objectPool;
  InGameRuntimeRoot *inGameRoot;
  SelectionPlayerRuntimeBlock *localPlayerBlock;
  uint32_t *clearCursor;
  uint32_t *copyCursor;
  uint32_t *templateCursor;
  int remainingCount;
  uint32_t allocationError;
  uint32_t stepError;

  /* 4 MB pool for the world objects (INGAME_WORLD_OBJECT_RECORD_COUNT records), zeroed */
  allocationError = g_MemoryApi.alloc(INGAME_WORLD_OBJECT_RECORD_COUNT * sizeof(WorldObjectRecord),&objectPool);
  if (allocationError != 0) {
    *outError = allocationError;
    return false;
  }
  g_RuntimeObjectRebaseBaseMinusOne = (uint8_t *)objectPool - 1;
  g_InGameWorldObjectRecords = (WorldObjectRecord *)objectPool;
  clearCursor = (uint32_t *)objectPool;
  for (remainingCount = INGAME_WORLD_OBJECT_RECORD_COUNT * sizeof(WorldObjectRecord) / 4; remainingCount != 0;
       remainingCount--) {
    *clearCursor = 0;
    clearCursor++;
  }
  if (!SelectionInfoPanel_InitResources
                     ((SelectionInfoEntitySlots *)
                      g_SelectionPlayerRuntimeBlockPointers[g_LocalPlayerRuntimeId],&stepError)) {
    *outError = stepError;
    return false;
  }
  allocationError = g_MemoryApi.alloc(sizeof(InGameRuntimeRoot),(void **)&inGameRoot);
  if (allocationError != 0) {
    *outError = allocationError;
    return false;
  }
  templateCursor = (uint32_t *)&g_InGameRuntimeDefaultImageTemplate;
  g_InGameRuntimeRoot = inGameRoot;
  /* copy the in-game root template (sizeof(InGameRuntimeRoot) / 4 dwords) */
  copyCursor = (uint32_t *)inGameRoot;
  for (remainingCount = sizeof(InGameRuntimeRoot) / 4; remainingCount != 0; remainingCount--) {
    *copyCursor = *templateCursor;
    templateCursor++;
    copyCursor++;
  }
  if (!InGameUiRuntime_InitializeControlTreeResources((UiRootNode *)inGameRoot,&stepError)) {
    *outError = stepError;
    return false;
  }
  /* world input and command callbacks, camera limits, and the step hook for the world runtime */
  /* signature differs: the overlay callback takes GraphicsBooleanState (int), the slot uint32_t */
  inGameRoot->worldOverlayCallback =
       (void (*)(uint32_t, WorldRuntimeContext *))InGameWorldOverlay_RebuildOrReleaseTransientMarkers;
  (inGameRoot->worldRuntime).selection.dispatchCommandCallback =
       InGameUiRuntime_DispatchCommandByCodeAndModifierFlags;
  (inGameRoot->worldRuntime).selection.resolveContextActionPrimaryCallback =
       InGameWorldInput_ResolveContextActionAndCursor;
  (inGameRoot->worldRuntime).selection.resolveContextActionSecondaryCallback =
       InGameWorldInput_ResolveContextActionAndCursor;
  (inGameRoot->worldRuntime).selection.beginPointerCaptureCallback =
       InGameWorldInput_BeginPointerCapture;
  (inGameRoot->worldRuntime).selection.updateDragSelectionCallback =
       InGameWorldInput_UpdateDragSelectionAndCamera;
  (inGameRoot->worldRuntime).selection.commitPointerActionCallback =
       InGameWorldInput_CommitPointerAction;
  /* signature differs: the callback takes void *, the slot WorldRuntimeContext * */
  (inGameRoot->worldRuntime).fieldRegion.clearTransientStateCallback =
       (void (*)(WorldRuntimeContext *))InGameUiRuntime_ResetNotificationButtonCursor;
  (inGameRoot->worldRuntime).selection.dispatchWorldContextActionCallback =
       InGameUiRuntime_DispatchWorldContextActionCallback;
  (inGameRoot->worldRuntime).minimumCameraDistanceQ12 = 8 * Q12_ONE;
  (inGameRoot->worldRuntime).maximumCameraDistanceQ12 = 19 * Q12_ONE;
  (inGameRoot->worldRuntime).motion.minimumPitchAngle = INGAME_CAMERA_MINIMUM_PITCH_ANGLE16;
  (inGameRoot->worldRuntime).motion.maximumPitchAngle = INGAME_CAMERA_MAXIMUM_PITCH_ANGLE16;
  (inGameRoot->worldRuntime).tickSpinLock = &g_InGameStateTickSpinLock;
  (inGameRoot->worldRuntime).simulationAndNetworkTickCallback =
       InGameRuntime_UpdateSimulationAndNetworkTick;
  localPlayerBlock = g_SelectionPlayerRuntimeBlockPointers[g_LocalPlayerRuntimeId];
  inGameRoot->localPlayerMarkedCellCount = 0;
  inGameRoot->localPlayerMarkedCells = localPlayerBlock->markedCells;
  UiRootStack_Push(&g_InGameUiRootCallbacks,(UiRootNode *)inGameRoot);
  *outRoot = inGameRoot;
  return true;
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
  uint16_t *loadingMoviePath;
  MovieRuntime *firstFrameMovie;
  uint32_t movieEndCode;
  PlayerRuntimeId localPlayerId;
  uint32_t localFactionIndex;
  uint32_t resetDefaultsError;
  uint32_t *clearCursor;
  int remainingCount;
  uint32_t stepError;

  world = &inGameRoot->worldRuntime;
  if (!LevelAsset_PrepareEndingMoviePath(levelMoviePath,&levelAsset->header,&loadingMoviePath,&stepError)) {
    *outError = stepError;
    return false;
  }
  if (!Movie_Open(MOVIE_OPEN_PACKAGE_ONLY,loadingMoviePath,NULL,&stepError)) {
    *outError = stepError;
    return false;
  }
  if (!Movie_AdvanceFrame(&firstFrameMovie,&movieEndCode)) {
    *outError = movieEndCode;
    return false;
  }
  inGameRoot->levelMovieRuntime = firstFrameMovie;
  g_MoviePlaybackBaseFrameGroup = 0;
  g_MoviePlaybackScheduleCounter = 0;
  g_MoviePlaybackScheduleSpan = 0;
  g_MoviePlaybackCurrentFrame = 0;
  MoviePlayback_AdvanceToFrameAndPresent(0);
  MoviePlayback_AdvanceToFrameAndPresent(1);
  RecentTextHistory_SortAndBuildPointerList(8,&inGameRoot->recentTextHistory);
  WorldRuntime_AttachObjectArray(INGAME_WORLD_OBJECT_RECORD_COUNT,g_InGameWorldObjectRecords,world);
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
  /* clear the four notification queue records (0x80 bytes) */
  clearCursor = (uint32_t *)inGameRoot->notificationQueue;
  for (remainingCount = 32; remainingCount != 0; remainingCount--) {
    *clearCursor = 0;
    clearCursor++;
  }
  if (!TerrainCompositeTexture_Create(&stepError)) {
    *outError = stepError;
    return false;
  }
  return true;
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
  uint32_t textureDimension;
  uint32_t gridHalfSize;
  uint32_t subresourceCount;
  uint32_t linkOptionFlags;
  uint32_t mapMouseOptionFlags;
  uint32_t subsystemFailureError;
  uint32_t gridScratchError;

  world = &inGameRoot->worldRuntime;
  g_SpinLockAcquire((RuntimeSpinLockValue *)&g_InGameStateTickSpinLock);
  g_InGameSimulationStepTicks = 1;
  textureDimension =
       PersistentSettings_Read(PERSISTENT_DEFAULT_SHADING_TEXTURE_DIMENSION,PERSISTENT_SETTING_SHADING_TEXTURE_DIMENSION);
  gridHalfSize = PersistentSettings_Read(PERSISTENT_DEFAULT_SHADING_GRID_HALF_SIZE,PERSISTENT_SETTING_SHADING_GRID_HALF_SIZE);
  subresourceCount =
       PersistentSettings_Read(PERSISTENT_DEFAULT_SHADING_SUBRESOURCE_COUNT,PERSISTENT_SETTING_SHADING_SUBRESOURCE_COUNT);
  GraphicsShadingRuntime_InitializeGeneratedTexture(subresourceCount,gridHalfSize,textureDimension);
  /* mirror the shading and mouse/panel options into the world runtime flags */
  InGameSession_SetWorldRuntimeFlag
            (world,WORLD_RUNTIME_FLAG_SHADING_ENABLED,PersistentSettings_Read(1,PERSISTENT_SETTING_SHADING_ENABLED) != 0);
  linkOptionFlags = PersistentSettings_Read(0,PERSISTENT_SETTING_MOUSE_LINK_PANEL_OPTION_FLAGS);
  InGameSession_SetWorldRuntimeFlag
            (world,WORLD_RUNTIME_FLAG_LINK_ROTATION_ZOOM,(linkOptionFlags & PERSISTENT_LINK_OPTION_ROTATION_ZOOM) != 0);
  InGameSession_SetWorldRuntimeFlag
            (world,WORLD_RUNTIME_FLAG_LINK_ROTATION_TILT,(linkOptionFlags & PERSISTENT_LINK_OPTION_ROTATION_TILT) != 0);
  InGameSession_SetWorldRuntimeFlag
            (world,WORLD_RUNTIME_FLAG_HIDE_PANEL,(linkOptionFlags & PERSISTENT_LINK_OPTION_HIDE_PANEL) != 0);
  mapMouseOptionFlags = PersistentSettings_Read(0,PERSISTENT_SETTING_MAP_MOUSE_OPTION_FLAGS);
  /* Original quirk: the (unreachable) failure of the optional subsystem below reports the map/mouse option flags
     as its error code, or the address of the resource bar page stack when bit 2 of them is set (left-over
     intermediate values). */
  subsystemFailureError = mapMouseOptionFlags;
  if ((mapMouseOptionFlags & 4) != 0) {
    UiPageStack_SetActiveIndex(1,&inGameRoot->sidePanelPageStack);
    subsystemFailureError = (uint32_t)(uintptr_t)&inGameRoot->resourceBarModePageStack; /* low 32 bits on x64 */
    UiPageStack_SetActiveIndex(0,&inGameRoot->resourceBarModePageStack);
    UiPageStack_SetActiveIndex(0,&inGameRoot->gamePanelsModePageStack);
    inGameRoot->worldViewAreaRightOffset = 0;
    UiContainer_LayoutChildren((UiNodeBase *)inGameRoot);
  }
  if (InGameRuntime_InitializeOptionalSubsystemAlwaysSuccess((uintptr_t)world->fieldGrid) != 0) {
    *outError = subsystemFailureError;
    return false;
  }
  /* a campaign carries units over from the previous level */
  if (g_FrontendLoadedCampaignAsset == 0) {
    OldUnitRuntime_ResetPendingTables();
  }
  else {
    DebugHook_CampaignCarryOver(0);
    OldUnitRuntime_MergeMasksAndReplayRecords();
    DebugHook_CampaignCarryOver(1);
  }
  if (!GridScratch_AllocateForFieldGrid(world->fieldGrid,&gridScratchError)) {
    *outError = gridScratchError;
    return false;
  }
  GridScratch_RebuildTerrainAndRuntimeClassificationMasks(world);
  GridInfluence_ClearDistanceBandsAndRefreshEntities(world->ownerListHead);
  TechnologyRuntime_RebuildDerivedLimitsAndCategoryMasks();
  if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_NETWORKED_MASK) == SESSION_NETWORK_ROLE_LOCAL) {
    inGameRoot->worldViewWrappedTextNodeFlags = inGameRoot->worldViewWrappedTextNodeFlags | 8;
  }
  else {
    inGameRoot->worldViewWrappedTextNodeFlags = inGameRoot->worldViewWrappedTextNodeFlags & ~8u;
  }
  InGameBuildCatalog_RebuildGrid((UiNodeBase *)inGameRoot);
  InGameSpecialBuildCatalog_RebuildGrid((UiNodeBase *)inGameRoot);
  InGameArmyStock_RebuildGrid((UiNodeBase *)inGameRoot);
  InGameOtherPlayerCommand_RebuildTargetEntries((UiNodeBase *)inGameRoot);
  WorldLightingRuntime_UpdateInterpolatedTerrainLighting();
  return true;
}

/* Reports this player as loaded, releases the step spin lock taken by InGameNewSession_FinishWorldUnderTickLock and
   shows the player-status screen while keeping the lockstep running until every player is ready; then closes the
   level movie and switches to the game page.
*/
static void InGameNewSession_ReportReadyAndWaitForPlayers(InGameRuntimeRoot *inGameRoot)

{
  if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_NETWORKED_MASK) == SESSION_NETWORK_ROLE_LOCAL) {
    FrontendPlayerRuntime_IncrementReadyCountAndResolveConsensus(g_LocalPlayerRuntimeId,0,0,0);
  }
  else {
    InGameCommandQueue_AppendLocalPlayerCommand(INGAME_COMMAND_PLAYER_READY,0,0,0);
  }
  g_SpinLockRelease((RuntimeSpinLockValue *)&g_InGameStateTickSpinLock);
  UiFrame_FlushInputAndResetPendingTicks();
  g_GraphicsCursorSetFrame(GRAPHICS_CURSOR_FRAME_BUSY);
  g_CursorVisibilityToken++;
  InGamePanel_RebuildPlayerStatusRows(inGameRoot);
  do {
    UiNode_InvalidateRoot(&inGameRoot->playerStatusNode);
    InGamePanel_RebuildPlayerStatusRows(inGameRoot);
    UiFrame_Update(0);
    UiFrame_Draw();
    g_GraphicsFramebufferPresent(g_FramebufferAccess);
    InGameRuntime_UpdateSimulationAndNetworkTick();
  } while ((g_UiCommandRuntimeFlags & UI_COMMAND_RUNTIME_FLAG_WAITING_FOR_PLAYERS) != 0);
  inGameRoot->levelMovieRuntime = NULL;
  inGameRoot->playerStatusLineCount = 0;
  UiPageStack_SetActiveIndex(2,&inGameRoot->primaryPageStack);
  Movie_Close();
  g_GraphicsCursorSetFrame(GRAPHICS_CURSOR_FRAME_ARROW);
}

/* Starts the session notification timer and queues the level's five intro notification movies (consecutive ids
   from the first one; none when it is 0). */
static void InGameNewSession_QueueIntroNotifications(void)

{
  InGameNotificationMovieId firstMovieId;
  uint32_t introIndex;

  /* Lost load: the original reads the first of five level intro notification movies from the level image. */
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
  if (!InGameNewSession_CreateRoot(&inGameRoot,&stepError)) {
    return InGameNewSession_Fail(stepError,outError);
  }
  if (!InGameNewSession_LoadWorld(levelAsset,levelMoviePath,inGameRoot,&stepError)) {
    return InGameNewSession_Fail(stepError,outError);
  }
  if (!InGameNewSession_FinishWorldUnderTickLock(inGameRoot,&stepError)) {
    return InGameNewSession_Fail(stepError,outError);
  }
  InGameNewSession_ReportReadyAndWaitForPlayers(inGameRoot);
  InGameNewSession_QueueIntroNotifications();
  return true;
}

/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/src/gameplay/session/startup.cpp
 * Reverse engineering by idkFoxes 2026
 */

#include <thandor/gameplay/session/startup.h>
#include <thandor/thandor.h>
#include <thandor/platform/bootstrap/image.h>
#include <thandor/platform/debug/hooks.h>

/* Module data. */

SelectionPlayerRuntimeBlock *g_SelectionPlayerBlocks = 0;

uint32_t g_EndMovieVariantIndex = 0;

uint16_t *g_EndMoviePath = 0;

uint32_t g_HostCommandBatchSyncSentThisInterval = 0;

static void *g_InGameFactionScratchBufferSetA8[8] = {0};

static void *g_InGameFactionScratchBufferSetB8[8] = {0};

int32_t g_InGamePendingSimulationTicks = 0;

uint32_t g_EndGameResultsCurrentMusicTrackId = 0;

static WorldObjectRecord *g_InGameWorldObjectRecords = 0;

static uintptr_t g_InGameWorldRuntimeDwordArray256[256] = {0}; /* SpatialSoundSlot pointers (WorldRuntimeContext.dwordArray) */

static uint8_t g_InGameSessionStartedNetworked = 0;

/* Implementation ownership: gameplay/session/startup. */

/* Failure exit of InGameRuntime_RunSessionUntilExit: releases what the session set up and reports the error. */
static Bool8 InGameRuntime_FailSession(uint32_t sessionError,uint32_t *outError)

{
  InGameRuntime_ShutdownAndReleaseResources();
  *outError = sessionError;
  return false;
}

/* Runs one in-game session from the frontend: starts a new level or loads a saved game (bit 0 of
   loadExistingSessionFlag), then renders frames until the session is closed, the end movie is due or the local
   player left, tears the session down along the matching path and returns true. A failed start or an emptied UI
   root stack returns false with the error code in *outError, which the caller hands to the fatal-error dispatcher.
*/
Bool8 InGameRuntime_RunSessionUntilExit(LevelAssetRuntimePrefix *levelAsset,
          FrontendBooleanState32 loadExistingSessionFlag,uint16_t *levelPathUtf16,uint32_t *outError)

{
  uint32_t startupError;
  Bool8 started;

  /* Original quirk: a local game leaves the simulation on the primary random stream, which the frontend and the
     in-game UI also advance per drawn frame, so its outcome depends on the frame rate (a network game seeds both
     streams and simulates on the secondary one). The state hash test aid does the same as a network game here,
     before the first simulation steps, which already run during the initialisation. */
  DebugHook_SessionInitializing();
  if ((loadExistingSessionFlag & 1U) == 0) {
    started = InGameRuntime_InitializeNewSession(levelAsset,levelPathUtf16,&startupError);
  }
  else {
    started = InGameRuntime_InitializeLoadedSession(levelPathUtf16,&startupError);
  }
  if (!started) {
    return InGameRuntime_FailSession(startupError,outError);
  }
  DebugHook_SessionStarted();
  do {
    DebugHook_SessionFrameBegin();
    /* two pending simulation ticks are consumed per rendered frame, clamped at zero */
    g_InGamePendingSimulationTicks = g_InGamePendingSimulationTicks - 2;
    if ((int)g_InGamePendingSimulationTicks < 0) {
      g_InGamePendingSimulationTicks = 0;
    }
    UiRootStack_InvalidateAll();
    UiFrame_ProcessAndPresent();
    DebugHook_SessionFrameEnd();
    if ((g_UiCommandRuntimeFlags & UI_COMMAND_RUNTIME_FLAG_SESSION_CLOSED) != 0) {
      g_SoundStopAllVoices();
      g_TimerUnregisterPeriodic(InGameRuntime_ProcessQueuedSessionNotificationTimer);
      GridScratch_ReleaseBuffers();
      g_EndMovieSelectionIndex = 0;
      OldUnitRuntime_RebuildScenarioReplayTables();
      UiRuntime_SetSynchronizationHooks(NULL,NULL);
      UiRootStack_PopUntilWindowTextureBoundary();
      InGameRuntime_ShutdownAndReleaseResources();
      return true;
    }
    if ((g_UiCommandRuntimeFlags & UI_COMMAND_RUNTIME_FLAG_END_MOVIE_PENDING) != 0) {
      g_SoundStopAllVoices();
      g_TimerUnregisterPeriodic(InGameRuntime_ProcessQueuedSessionNotificationTimer);
      GridScratch_ReleaseBuffers();
      /* FrontendSession_PeriodicTick keeps running under the in-game tick lock while the end movie plays */
      UiRuntime_SetSynchronizationHooks(FrontendSession_PeriodicTick,(RuntimeSpinLockValue *)&g_InGameStateTickSpinLock);
      Frontend_PlaySelectedEndMovie();
      OldUnitRuntime_RebuildScenarioReplayTables();
      UiRuntime_SetSynchronizationHooks(NULL,NULL);
      UiRootStack_PopUntilWindowTextureBoundary();
      InGameRuntime_ShutdownAndReleaseResources();
      g_FrontendScenarioPathScratchUtf16[0] = 0;
      return true;
    }
    if ((g_UiCommandRuntimeFlags & UI_COMMAND_RUNTIME_FLAG_LOCAL_PLAYER_LEFT) != 0) {
      g_SoundStopAllVoices();
      g_TimerUnregisterPeriodic(InGameRuntime_ProcessQueuedSessionNotificationTimer);
      GridScratch_ReleaseBuffers();
      UiRuntime_SetSynchronizationHooks(NULL,NULL);
      InGameRuntime_ShutdownAndReleaseResources();
      g_FrontendScenarioPathScratchUtf16[0] = 0;
      return true;
    }
  } while (g_UiRootNode != UI_ROOT_STACK_END);
  return InGameRuntime_FailSession(FATAL_ERROR_GENERAL_FAILURE,outError);
}

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
       (40 bytes), so the copy also covers the 40 bytes behind each of them. */
    nameSource = (uint32_t *)frontendPlayer->playerName.textUtf16;
    nameDestination = (uint32_t *)selectionBlock->playerNameUtf16;
    for (remainingCount = 20; remainingCount != 0; remainingCount--) {
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

/* Sets or clears flag in the world runtime flags. */
static void InGameSession_SetWorldRuntimeFlag(WorldRuntimeContext *world,WorldRuntimeFlags flag,Bool8 enabled)

{
  if (enabled) {
    world->runtimeFlags = world->runtimeFlags | flag;
  }
  else {
    world->runtimeFlags = world->runtimeFlags & ~flag;
  }
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

/* Failure exit of InGameRuntime_InitializeLoadedSession: closes the level movie, releases the level entry and
   unmounts the save package (levelAsset is NULL and saveHandle 0 when they were not loaded yet), stores the error
   in *outError and returns false.
*/
static Bool8 InGameLoadedSession_Fail(FrontendLoadedLevelAsset *levelAsset,EngineFileHandle saveHandle,uint32_t error,
                                     uint32_t *outError)

{
  Movie_Close();
  Resource_Release(levelAsset);
  Package_Unmount(saveHandle);
  *outError = error;
  return false;
}

/* Sets the text of the in-game template's save-name edit (saveNameEdit, 32 code units) to the session name of a
   mounted save package: the UTF-16 string at offset 256 of the package header (scanned for at most 36
   characters), without its four-character file extension and cut to 31 characters. Without a terminator the name
   stays empty.
*/
static void InGameLoadedSession_ReadSessionName(EngineFileHandle saveHandle)

{
  uint8_t *headerBuffer;
  uint16_t *nameStart;
  uint16_t *scanEnd;
  uint16_t *sessionNameCursor;
  uint16_t *sourceCursor;
  uint32_t copyCount;
  int remainingCount;
  Bool8 terminatorFound;

  headerBuffer = g_PackageScratchBuffer;
  sessionNameCursor = ((UiRequiredTextEditControl *)&g_InGameRuntimeDefaultImageTemplate.saveNameEdit)->textBuffer;
  for (remainingCount = 32; remainingCount != 0; remainingCount--) {
    *sessionNameCursor = 0;
    sessionNameCursor++;
  }
  g_FileSystemSeek(FILESYSTEM_SEEK_BEGIN,0,THANDOR_PTR(saveHandle));
  g_FileSystemReadExact(PCK_ENTRY_HEADER_BYTES,headerBuffer,THANDOR_PTR(saveHandle));
  nameStart = (uint16_t *)(headerBuffer + 256);
  terminatorFound = false;
  scanEnd = nameStart;
  for (remainingCount = 36; remainingCount != 0 && !terminatorFound; remainingCount--) {
    terminatorFound = *scanEnd == 0;
    scanEnd++;
  }
  if (!terminatorFound) {
    return;
  }
  /* scanEnd is just past the terminator: the four characters before the terminator (the file extension) are cut
     off */
  scanEnd[-3] = 0;
  scanEnd[-2] = 0;
  scanEnd[-5] = 0;
  scanEnd[-4] = 0;
  copyCount = (uint32_t)(scanEnd - nameStart);
  if (31 < copyCount) {
    copyCount = 31;
  }
  sessionNameCursor = ((UiRequiredTextEditControl *)&g_InGameRuntimeDefaultImageTemplate.saveNameEdit)->textBuffer;
  sourceCursor = nameStart;
  for (; copyCount != 0; copyCount--) {
    *sessionNameCursor = *sourceCursor;
    sourceCursor++;
    sessionNameCursor++;
  }
}

/* Resets the session state for a loaded game: clears all selection blocks and sets up only block 0 (local player 0
   with the saved faction), resets the end-movie, tick and ready state, and starts the periodic step timer and the
   UI synchronisation hooks.
*/
static void InGameLoadedSession_ResetSessionState(uint32_t savedFactionIndex)

{
  uint32_t *clearCursor;
  int remainingCount;

  clearCursor = (uint32_t *)g_SelectionPlayerBlocks;
  for (remainingCount = SELECTION_PLAYER_BLOCK_COUNT * sizeof(SelectionPlayerRuntimeBlock) / sizeof(uint32_t);
       remainingCount != 0; remainingCount--) {
    *clearCursor = 0;
    clearCursor++;
  }
  g_EndMovieSelectionIndex = UINT32_MAX;
  g_EndMovieVariantIndex = 0;
  g_EndMoviePath = NULL;
  g_LocalPlayerRuntimeId = 0;
  g_SelectionPlayerRuntimeBlockPointers[0] = g_SelectionPlayerBlocks;
  g_SelectionPlayerBlocks->factionIndex = savedFactionIndex;
  g_SelectionPlayerBlocks->simulationStepTicks = 1;
  g_UiCommandRuntimeFlags = UI_COMMAND_RUNTIME_FLAG_WAITING_FOR_PLAYERS | UI_COMMAND_RUNTIME_FLAG_PAUSED;
  g_SessionNetworkTickCounter = 1;
  g_HostCommandBatchSyncSentThisInterval = 0;
  g_GameFactionRuntimeImage.tail.simulationTick = 1;
  g_GameFactionRuntimeImage.tail.presentationTick = 0;
  g_InGameSessionNotificationTimeoutTicks = 0;
  g_InGameReadyStateToggleFlags = 0;
  g_InGameNetworkTickCountdown = INGAME_TIMER_TICKS_PER_SIMULATION_STEP;
  g_InGameStateTickSpinLock = 0;
  g_EndGameResultsCurrentMusicTrackId = 0;
  g_InGameSelectedTechnologyId = TEC_000_BASIC_TECHNOLOGY;
  g_TimerRegisterPeriodic(INGAME_PERIODIC_TIMER_HZ,InGameRuntime_PeriodicCountdownAndClockTick);
  UiRuntime_SetSynchronizationHooks
            (InGameRuntime_UpdateSimulationAndNetworkTick,(RuntimeSpinLockValue *)&g_InGameStateTickSpinLock);
}

/* Allocates the zeroed world object pool, the selection info panel resources and the in-game root (a copy of
   g_InGameRuntimeDefaultImageTemplate with its control tree and callbacks), pushes the root onto the UI root stack
   and builds the level's scenario path. Returns true with the root in *outRoot; on failure returns false with the
   error in *outError.
*/
static Bool8 InGameLoadedSession_CreateRoot(FrontendLoadedLevelAsset *levelImage,InGameRuntimeRoot **outRoot,
                                           uint32_t *outError)

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

  /* 4 MB pool for the world objects (0x4000 records of 0x100 bytes), zeroed */
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
  if (!SelectionInfoPanel_InitResources((SelectionInfoEntitySlots *)g_SelectionPlayerBlocks,&stepError)) {
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
  /* copy the in-game root template (0x30F9 dwords = 0xC3E4 bytes) */
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
  WidePath_CombineDirectoryAndLeaf
            (g_FrontendScenarioPathScratchUtf16,
             (levelImage->header).levelFileNameUtf16,(uint16_t *)g_ScenarioLevelDirectoryUtf16);
  WidePath_SetExtensionCode(WIDE_PATH_EXTENSION_LEV,g_FrontendScenarioPathScratchUtf16);
  *outRoot = inGameRoot;
  return true;
}

/* Opens the level movie and shows its first frames, attaches the world arrays, loads the saved external tables and
   field grid with the level resources, clears the notification queue and creates the terrain texture. Returns
   true on success; on failure returns false with the error in *outError.
*/
static Bool8 InGameLoadedSession_LoadWorld(uint16_t *savePackagePath,FrontendLoadedLevelAsset *levelImage,
                                          InGameRuntimeRoot *inGameRoot,uint32_t *outError)

{
  WorldRuntimeContext *world;
  uint16_t *loadingMoviePath;
  MovieRuntime *firstFrameMovie;
  uint32_t movieEndCode;
  uint32_t localFactionIndex;
  void *fieldGrid;
  uint32_t packageLoadErrorCode;
  uint32_t *clearCursor;
  int remainingCount;
  uint32_t stepError;

  world = &inGameRoot->worldRuntime;
  if (!LevelAsset_PrepareEndingMoviePath
         (savePackagePath,(LevelAssetHeader *)levelImage,&loadingMoviePath,&stepError)) {
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
  localFactionIndex = g_SelectionPlayerBlocks->factionIndex;
  world->activeFactionRuntimeIndex = (FactionRuntimeIndex)localFactionIndex;
  world->selection.activePlayerRuntimeId = 0;
  WorldRuntime_AttachAndClearDwordArray
            (INGAME_WORLD_DWORD_ARRAY_COUNT,g_InGameWorldRuntimeDwordArray256,world);
  if (GameData_LoadExternalTables()) {
    /* Original quirk: this failure reports the local player's faction index as its error code (a left-over
       intermediate value). */
    *outError = localFactionIndex;
    return false;
  }
  fieldGrid = Package_LoadEntry((uint16_t *)g_FieldHexPathUtf16,&packageLoadErrorCode);
  if (fieldGrid == NULL) {
    *outError = packageLoadErrorCode;
    return false;
  }
  (levelImage->header).pathState.levelPathOffsetOrLoadedFieldGrid = (uint32_t)fieldGrid; /* 5f-format: LevelAssetHeader.pathState.levelPathOffsetOrLoadedFieldGrid (+0xB0) */
  if (!InGameLevelRuntime_LoadResourcesAfterExternalTables(levelImage,world,&stepError)) {
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

/* Takes the step spin lock and finishes the world while the step is held off: rebuilds the build/army/command
   grids, sets up the shading texture, mirrors the shading and mouse/panel options into the world runtime flags,
   allocates the grid scratch and rebuilds the derived terrain, influence, technology and lighting data. Returns
   true on success with the lock still held; on failure returns false with the error in *outError.
   Original quirk: the spin lock is not released on failure.
*/
static Bool8 InGameLoadedSession_FinishWorldUnderTickLock(InGameRuntimeRoot *inGameRoot,uint32_t *outError)

{
  WorldRuntimeContext *world;
  uint32_t textureDimension;
  uint32_t gridHalfSize;
  uint32_t subresourceCount;
  uint32_t linkOptionFlags;
  uint32_t gridScratchError;

  world = &inGameRoot->worldRuntime;
  g_SpinLockAcquire((RuntimeSpinLockValue *)&g_InGameStateTickSpinLock);
  InGameBuildCatalog_RebuildGrid((UiNodeBase *)inGameRoot);
  InGameSpecialBuildCatalog_RebuildGrid((UiNodeBase *)inGameRoot);
  InGameArmyStock_RebuildGrid((UiNodeBase *)inGameRoot);
  InGameOtherPlayerCommand_RebuildTargetEntries((UiNodeBase *)inGameRoot);
  g_InGameSimulationStepTicks = 1;
  textureDimension =
       PersistentSettings_Read(PERSISTENT_DEFAULT_SHADING_TEXTURE_DIMENSION,PERSISTENT_SETTING_SHADING_TEXTURE_DIMENSION);
  gridHalfSize =
       PersistentSettings_Read(PERSISTENT_DEFAULT_SHADING_GRID_HALF_SIZE,PERSISTENT_SETTING_SHADING_GRID_HALF_SIZE);
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
  if (InGameRuntime_InitializeOptionalSubsystemAlwaysSuccess((uintptr_t)world->fieldGrid) != 0) {
    /* Original quirk: this (unreachable) failure reports the mouse/panel option flags as its error code. */
    *outError = linkOptionFlags;
    return false;
  }
  if (!GridScratch_AllocateForFieldGrid(world->fieldGrid,&gridScratchError)) {
    *outError = gridScratchError;
    return false;
  }
  GridScratch_RebuildTerrainAndRuntimeClassificationMasks(world);
  GridInfluence_ClearDistanceBandsAndRefreshEntities(world->ownerListHead);
  TechnologyRuntime_RebuildDerivedLimitsAndCategoryMasks();
  WorldLightingRuntime_UpdateInterpolatedTerrainLighting();
  return true;
}

/* Continues a saved game: mounts the save package, takes the session name from its header, loads the campaign
   and level entries, and then follows the same steps as InGameRuntime_InitializeNewSession, except that the local
   player is always player 0 of a single block, the world comes from the saved external tables and field grid
   (InGameLevelRuntime_LoadResourcesAfterExternalTables) instead of a fresh level, and no intro notifications are
   queued. The package and the level entry are released again at the end. Returns true on success; on failure
   returns false and stores the error of the failing step in *outError.
*/
Bool8 InGameRuntime_InitializeLoadedSession(uint16_t *savePackagePath,uint32_t *outError)

{
  uintptr_t mountResult; /* the save package's handle, or the mount error code */
  EngineFileHandle saveHandle;
  void *campaignAsset;
  FrontendLoadedLevelAsset *levelImage;
  uint32_t packageLoadErrorCode;
  InGameRuntimeRoot *inGameRoot;
  uint32_t stepError;

  g_TextureDownsampleShift = PersistentSettings_Read(0,PERSISTENT_SETTING_TEXTURE_QUALITY);
  if (!Package_Mount(savePackagePath,&mountResult)) {
    return InGameLoadedSession_Fail(NULL,0,(uint32_t)mountResult,outError);
  }
  saveHandle = mountResult;
  InGameLoadedSession_ReadSessionName(saveHandle);
  campaignAsset = Package_LoadEntry((uint16_t *)g_CampagneHexPathUtf16,NULL);
  if (campaignAsset != NULL) {
    g_FrontendLoadedCampaignAsset = (uintptr_t)campaignAsset;
  }
  levelImage = (FrontendLoadedLevelAsset *)Package_LoadEntry((uint16_t *)g_LevelHexPathUtf16,&packageLoadErrorCode);
  if (levelImage == NULL) {
    return InGameLoadedSession_Fail(NULL,saveHandle,packageLoadErrorCode,outError);
  }
  InGameLoadedSession_ResetSessionState(levelImage->playerSlots[6].aiClassOrMode);
  if (!InGameLoadedSession_CreateRoot(levelImage,&inGameRoot,&stepError) ||
      !InGameLoadedSession_LoadWorld(savePackagePath,levelImage,inGameRoot,&stepError) ||
      !InGameLoadedSession_FinishWorldUnderTickLock(inGameRoot,&stepError)) {
    return InGameLoadedSession_Fail(levelImage,saveHandle,stepError,outError);
  }
  if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_NETWORKED_MASK) == SESSION_NETWORK_ROLE_LOCAL) {
    inGameRoot->worldViewWrappedTextNodeFlags = inGameRoot->worldViewWrappedTextNodeFlags | 8;
  }
  else {
    inGameRoot->worldViewWrappedTextNodeFlags = inGameRoot->worldViewWrappedTextNodeFlags & ~8u;
  }
  /* report this player as loaded */
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
  /* show the player-status screen and keep the lockstep running until all are ready */
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
  Resource_Release(levelImage);
  Package_Unmount(saveHandle);
  g_GraphicsCursorSetFrame(GRAPHICS_CURSOR_FRAME_ARROW);
  g_TimerRegisterPeriodic(10,InGameRuntime_ProcessQueuedSessionNotificationTimer);
  return true;
}

/* Ends an in-game session (counterpart of InGameRuntime_InitializeNewSession/InitializeLoadedSession): stops the
   step timer, shows a black screen with the busy cursor, then releases the world (every entity's bindings, the
   level assets), the in-game UI root, the faction scratch buffers, the object pool, the level movie, the terrain
   texture and the four panel texture packages, and resets the sprite registry and pending input so the frontend
   starts clean.
*/
void InGameRuntime_ShutdownAndReleaseResources(void)

{
  WorldRuntimeContext *world;
  InGameRuntimeRoot *inGameRoot;
  Bool8 beginAccessFailed;
  
  g_TimerUnregisterPeriodic(InGameRuntime_PeriodicCountdownAndClockTick);
  inGameRoot = g_InGameRuntimeRoot;
  g_GraphicsCursorSetFrame(GRAPHICS_CURSOR_FRAME_BUSY);
  beginAccessFailed = g_GraphicsFramebufferBeginAccess();
  if (!beginAccessFailed) {
    /* opaque black over the whole framebuffer */
    g_GraphicsFramebufferFillRectArgb
              (g_FramebufferHeight,g_FramebufferWidth,0,0,g_FramebufferHeight,g_FramebufferWidth,0,0
               ,0xff000000,g_FramebufferAccess);
    g_GraphicsFramebufferEndAccess();
  }
  g_GraphicsFramebufferPresent(g_FramebufferAccess);
  GraphicsShadingRuntime_Shutdown();
  if (inGameRoot != NULL) {
    InGameRuntime_SaveWorldViewInfoTextChoice(&inGameRoot->rootUi);
    world = &inGameRoot->worldRuntime;
    /* signature differs: the callback's context is WorldRuntimeContext *, the slot's void * */
    WorldRuntime_ForEachOwnerListNode
              (world,(WorldRuntimeNodeTraversalCallback *)WorldRuntimeNode_ReleaseShutdownBindingsCallback,world);
    InGameLevelRuntime_ShutdownLoadedAssetResources(world);
    if ((inGameRoot->rootUi).previousRoot != NULL) {
      UiRootStack_Pop(&inGameRoot->rootUi);
    }
    g_MemoryApi.free(inGameRoot);
    g_InGameRuntimeRoot = NULL;
  }
  InGameRuntime_ReleaseFactionScratchBuffers();
  g_MemoryApi.free(g_InGameWorldObjectRecords);
  g_InGameWorldObjectRecords = NULL;
  Movie_Close();
  TerrainCompositeTexture_Destroy();
  g_GraphicsTextureSourceLifecycleCallbacks3.releasePackage((GraphicsTextureSourceAsset *)g_InGameDiagramTextureSource);
  g_GraphicsTextureSourceLifecycleCallbacks3.releasePackage(g_InGamePanelTextureSource);
  g_GraphicsTextureSourceLifecycleCallbacks3.releasePackage((GraphicsTextureSourceAsset *)g_InGameTechnologyTextureSource);
  g_GraphicsTextureSourceLifecycleCallbacks3.releasePackage((GraphicsTextureSourceAsset *)g_InGameWindowTextureSource);
  g_InGameDiagramTextureSource = NULL;
  g_InGamePanelTextureSource = NULL;
  g_InGameTechnologyTextureSource = NULL;
  g_InGameWindowTextureSource = NULL;
  GraphicsShadingRuntime_ClearRecordTable();
  SelectionInfoPanel_ShutdownResources();
  SpriteAssetRegistry_Reset();
  UiFrame_FlushInputAndResetPendingTicks();
  g_CursorVisibilityToken--;
  return;
}

/* Frees the two scratch buffers of each of the eight factions (sets A and B) at session shutdown and clears the
   pointers.
*/
void InGameRuntime_ReleaseFactionScratchBuffers(void)

{
  int remainingFactions;
  void **scratchBufferSetBCursor;
  void **scratchBufferSetACursor;

  remainingFactions = 8;
  scratchBufferSetACursor = g_InGameFactionScratchBufferSetA8;
  scratchBufferSetBCursor = g_InGameFactionScratchBufferSetB8;
  do {
    g_MemoryApi.free(*scratchBufferSetACursor);
    g_MemoryApi.free(*scratchBufferSetBCursor);
    *scratchBufferSetACursor = NULL;
    *scratchBufferSetBCursor = NULL;
    scratchBufferSetACursor++;
    scratchBufferSetBCursor++;
    remainingFactions--;
  } while (remainingFactions != 0);
  return;
}

/* Optional initialisation step of new and loaded sessions; it always succeeds (returns 0), so the callers' failure
   branches never run.
*/
uint8_t InGameRuntime_InitializeOptionalSubsystemAlwaysSuccess(uintptr_t unusedArgument)

{
  return 0;
}

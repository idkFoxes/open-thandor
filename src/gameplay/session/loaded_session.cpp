/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/src/gameplay/session/loaded_session.cpp
 * Reverse engineering by idkFoxes 2026
 */

#include <thandor/gameplay/session/loaded_session.h>
#include <thandor/thandor.h>
#include <thandor/platform/bootstrap/image.h>
#include <thandor/platform/debug/hooks.h>

/* Module data. */

SelectionPlayerRuntimeBlock *g_SelectionPlayerBlocks = nullptr;

/* Implementation ownership: gameplay/session/loaded_session. */

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
  g_EndMoviePath = nullptr;
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
  if (!Movie_Open(MOVIE_OPEN_PACKAGE_ONLY,loadingMoviePath,nullptr,&stepError)) {
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
  if (fieldGrid == nullptr) {
    *outError = packageLoadErrorCode;
    return false;
  }
  (levelImage->header).pathState.levelPathOffsetOrLoadedFieldGrid = Thandor_PointerToU32(fieldGrid); /* 5f-format: LevelAssetHeader.pathState.levelPathOffsetOrLoadedFieldGrid (+0xB0) */
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
    return InGameLoadedSession_Fail(nullptr,0,(uint32_t)mountResult,outError);
  }
  saveHandle = mountResult;
  InGameLoadedSession_ReadSessionName(saveHandle);
  campaignAsset = Package_LoadEntry((uint16_t *)g_CampagneHexPathUtf16,nullptr);
  if (campaignAsset != nullptr) {
    g_FrontendLoadedCampaignAsset = (uintptr_t)campaignAsset;
  }
  levelImage = (FrontendLoadedLevelAsset *)Package_LoadEntry((uint16_t *)g_LevelHexPathUtf16,&packageLoadErrorCode);
  if (levelImage == nullptr) {
    return InGameLoadedSession_Fail(nullptr,saveHandle,packageLoadErrorCode,outError);
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
  inGameRoot->levelMovieRuntime = nullptr;
  inGameRoot->playerStatusLineCount = 0;
  UiPageStack_SetActiveIndex(2,&inGameRoot->primaryPageStack);
  Movie_Close();
  Resource_Release(levelImage);
  Package_Unmount(saveHandle);
  g_GraphicsCursorSetFrame(GRAPHICS_CURSOR_FRAME_ARROW);
  g_TimerRegisterPeriodic(10,InGameRuntime_ProcessQueuedSessionNotificationTimer);
  return true;
}

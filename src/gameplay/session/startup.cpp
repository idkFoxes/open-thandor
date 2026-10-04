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

static void *g_InGameFactionScratchBufferSetA8[8] = {};

static void *g_InGameFactionScratchBufferSetB8[8] = {};

int32_t g_InGamePendingSimulationTicks = 0;

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
      UiRuntime_SetSynchronizationHooks(nullptr,nullptr);
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
      UiRuntime_SetSynchronizationHooks(nullptr,nullptr);
      UiRootStack_PopUntilWindowTextureBoundary();
      InGameRuntime_ShutdownAndReleaseResources();
      g_FrontendScenarioPathScratchUtf16[0] = 0;
      return true;
    }
    if ((g_UiCommandRuntimeFlags & UI_COMMAND_RUNTIME_FLAG_LOCAL_PLAYER_LEFT) != 0) {
      g_SoundStopAllVoices();
      g_TimerUnregisterPeriodic(InGameRuntime_ProcessQueuedSessionNotificationTimer);
      GridScratch_ReleaseBuffers();
      UiRuntime_SetSynchronizationHooks(nullptr,nullptr);
      InGameRuntime_ShutdownAndReleaseResources();
      g_FrontendScenarioPathScratchUtf16[0] = 0;
      return true;
    }
  } while (g_UiRootNode != UI_ROOT_STACK_END);
  return InGameRuntime_FailSession(FATAL_ERROR_GENERAL_FAILURE,outError);
}

/* Sets or clears flag in the world runtime flags. */
void InGameSession_SetWorldRuntimeFlag(WorldRuntimeContext *world,WorldRuntimeFlags flag,Bool8 enabled)

{
  if (enabled) {
    world->runtimeFlags = world->runtimeFlags | flag;
  }
  else {
    world->runtimeFlags = world->runtimeFlags & ~flag;
  }
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
  if (inGameRoot != nullptr) {
    InGameRuntime_SaveWorldViewInfoTextChoice(&inGameRoot->rootUi);
    world = &inGameRoot->worldRuntime;
    /* signature differs: the callback's context is WorldRuntimeContext *, the slot's void * */
    WorldRuntime_ForEachOwnerListNode
              (world,(WorldRuntimeNodeTraversalCallback *)WorldRuntimeNode_ReleaseShutdownBindingsCallback,world);
    InGameLevelRuntime_ShutdownLoadedAssetResources(world);
    if ((inGameRoot->rootUi).previousRoot != nullptr) {
      UiRootStack_Pop(&inGameRoot->rootUi);
    }
    g_MemoryApi.free(inGameRoot);
    g_InGameRuntimeRoot = nullptr;
  }
  InGameRuntime_ReleaseFactionScratchBuffers();
  g_MemoryApi.free(g_InGameWorldObjectRecords);
  g_InGameWorldObjectRecords = nullptr;
  Movie_Close();
  TerrainCompositeTexture_Destroy();
  g_GraphicsTextureSourceLifecycleCallbacks3.releasePackage((GraphicsTextureSourceAsset *)g_InGameDiagramTextureSource);
  g_GraphicsTextureSourceLifecycleCallbacks3.releasePackage(g_InGamePanelTextureSource);
  g_GraphicsTextureSourceLifecycleCallbacks3.releasePackage((GraphicsTextureSourceAsset *)g_InGameTechnologyTextureSource);
  g_GraphicsTextureSourceLifecycleCallbacks3.releasePackage((GraphicsTextureSourceAsset *)g_InGameWindowTextureSource);
  g_InGameDiagramTextureSource = nullptr;
  g_InGamePanelTextureSource = nullptr;
  g_InGameTechnologyTextureSource = nullptr;
  g_InGameWindowTextureSource = nullptr;
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
    *scratchBufferSetACursor = nullptr;
    *scratchBufferSetBCursor = nullptr;
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

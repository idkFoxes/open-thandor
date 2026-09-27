/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/src/gameplay/session/runtime.c
 * Reverse engineering by idkFoxes 2026
 */

#include <thandor/gameplay/session/runtime.h>
#include <thandor/thandor.h>
#include <thandor/platform/bootstrap/image.h>

/* Implementation ownership: gameplay/session/runtime. */

/* Address: 0x00564F70.
   Ownership: gameplay/session/runtime.
   Purpose: Chooses new-session or loaded-session initialization, processes frames until an exit flag is raised,
   performs the matching movie or frontend transition, and releases the in-game runtime resources. Typed
   parameters: p3 loadExistingSessionFlag→FrontendBooleanState32_V342. Calling convention, complete VariableStorage
   serialization, function bytes, control flow, globals, locals, and executable data remain unchanged.
   Local calls: InGameRuntime_InitializeNewSession, InGameRuntime_InitializeLoadedSession,
   InGameRuntime_ShutdownAndReleaseResources.
   Cross-module calls: UiRootStack_InvalidateAll [ui/controls/layout], UiFrame_ProcessAndPresent
   [ui/controls/layout], GridScratch_ReleaseBuffers [world/pathing/grid],
   OldUnitRuntime_RebuildScenarioReplayTables [gameplay/faction/runtime], UiRuntime_SetSynchronizationHooks
   [ui/core/runtime], UiRootStack_PopUntilWindowTextureBoundaryCf [ui/controls/text].
*/
SessionRunResult __thandor_eax_cf_preserve_ecx_edx
InGameRuntime_RunSessionUntilExit
          (LevelAssetRuntimeImagePrefix370 *levelAsset,
          FrontendBooleanState32 loadExistingSessionFlag,uint16_t *levelPathUtf16)

{
  uint32_t startupErrorOrExitCode;
  NewSessionInitResult newSessionInit;
  LoadedSessionInitResult loadedSessionInit;
  SessionRunResult abortResult;
  SessionRunResult quitResult;
  SessionRunResult endMovieResult;
  SessionRunResult failureResult;
  
  if ((loadExistingSessionFlag & 1U) == 0) {
    newSessionInit = InGameRuntime_InitializeNewSession(levelAsset,levelPathUtf16);
    startupErrorOrExitCode = newSessionInit.runtimeRootOrError;
    if (newSessionInit.failed)
    goto InGameRuntime_RunSessionUntilExit_ShutdownAndReturnStartupOrUiRootFailureWithCarrySet;
  }
  else {
    loadedSessionInit = InGameRuntime_InitializeLoadedSession(levelPathUtf16);
    startupErrorOrExitCode = loadedSessionInit.runtimeRootOrError;
    if (loadedSessionInit.failed)
    goto InGameRuntime_RunSessionUntilExit_ShutdownAndReturnStartupOrUiRootFailureWithCarrySet;
  }
  do {
    g_TestAidInGameFrames = g_TestAidInGameFrames + 1;
    g_InGamePendingSimulationTicks = g_InGamePendingSimulationTicks + -2;
    if ((int)g_InGamePendingSimulationTicks < 0) {
      g_InGamePendingSimulationTicks = 0;
    }
    UiRootStack_InvalidateAll();
    UiFrame_ProcessAndPresent();
    if ((g_UiCommandRuntimeFlags & 0x10000) != 0) {
      (*g_SoundStopAllVoices)();
      (*g_TimerUnregisterPeriodic)(InGameRuntime_ProcessQueuedSessionNotificationTimer);
      GridScratch_ReleaseBuffers();
      g_EndMovieSelectionIndex = 0;
      OldUnitRuntime_RebuildScenarioReplayTables();
      UiRuntime_SetSynchronizationHooks
                ((UiRuntimePostUnlockCallbackProc *)0x0,(RuntimeSpinLockValue *)0x0);
      UiRootStack_PopUntilWindowTextureBoundaryCf();
      InGameRuntime_ShutdownAndReleaseResources();
      quitResult.exitCodeOrError = 0xc;
      quitResult.failed = false;
      return quitResult;
    }
    if ((g_UiCommandRuntimeFlags & 0x800) != 0) {
      (*g_SoundStopAllVoices)();
      (*g_TimerUnregisterPeriodic)(InGameRuntime_ProcessQueuedSessionNotificationTimer);
      GridScratch_ReleaseBuffers();
      UiRuntime_SetSynchronizationHooks(FrontendSession_PeriodicTick,&g_InGameStateTickSpinLock);
      Frontend_PlaySelectedEndMovie();
      OldUnitRuntime_RebuildScenarioReplayTables();
      UiRuntime_SetSynchronizationHooks
                ((UiRuntimePostUnlockCallbackProc *)0x0,(RuntimeSpinLockValue *)0x0);
      UiRootStack_PopUntilWindowTextureBoundaryCf();
      InGameRuntime_ShutdownAndReleaseResources();
      g_FrontendScenarioPathScratchUtf16 = 0;
      endMovieResult.exitCodeOrError = 0xc;
      endMovieResult.failed = false;
      return endMovieResult;
    }
    if ((g_UiCommandRuntimeFlags & 0x20000) != 0) {
      (*g_SoundStopAllVoices)();
      (*g_TimerUnregisterPeriodic)(InGameRuntime_ProcessQueuedSessionNotificationTimer);
      GridScratch_ReleaseBuffers();
      UiRuntime_SetSynchronizationHooks
                ((UiRuntimePostUnlockCallbackProc *)0x0,(RuntimeSpinLockValue *)0x0);
      InGameRuntime_ShutdownAndReleaseResources();
      g_FrontendScenarioPathScratchUtf16 = 0;
      abortResult.exitCodeOrError = 0xc;
      abortResult.failed = false;
      return abortResult;
    }
  } while (g_UiRootNode != (UiRootNode *)0xffffffff);
  startupErrorOrExitCode = 0x14;
InGameRuntime_RunSessionUntilExit_ShutdownAndReturnStartupOrUiRootFailureWithCarrySet:
  InGameRuntime_ShutdownAndReleaseResources();
  failureResult.failed = true;
  failureResult.exitCodeOrError = startupErrorOrExitCode;
  return failureResult;
}


/* Address: 0x00566290.
   Ownership: gameplay/session/runtime.
   Purpose: End-game results update callback that advances presentation state and handles current input.
   Local calls: InGameRuntime_UpdateCursorGridAndViewScaleCache.
   Cross-module calls: FrontendClientSession_DecrementTimeoutsAndCompactPlayers [ui/frontend/session],
   FrontendHostSession_TickShutdownOrReadyConsensus [ui/frontend/session], FieldGrid_SetAllCellOverlayColors
   [world/terrain/grid], WorldRuntime_EmitModelDefinitionOverlayForMatchingEntries [world/runtime/core],
   RecentTextHistory_SortAndBuildPointerList [ui/support/runtime], InGameHud_UpdateStatusCountersAndSessionPrompts
   [ui/ingame/runtime].
*/

void __thandor_void_preserve_eax_ecx_edx
EndGameResultsUiRuntime_UpdateAndHandleInputCf(EndGameResultsRuntimeView44C4 *endGameResultsRuntime)

{
  Q12 *motionCoordinate;
  WorldMotionState *cameraMotion;
  FieldGridAsset *cameraFieldGrid;
  ArmyRuntimeSlot *armyRuntime;
  LevelMusicSampleNumber selectedMusicTrackId;
  int64_t projectedProduct;
  InGameLevelConditionStorageView800 *levelConditionStorage;
  UiNodeBase *hoveredNode;
  uint32_t candidateFrameOrScore;
  int columnDeltaOrCount;
  AngleTurn32 clampedPitchAngle;
  UQ12 clampedTargetDistance;
  int gridRowOrDeltaY;
  uint32_t bestTrackOrSecondsLeft;
  InGamePresentationTick currentPresentationTick;
  int targetRowQ12;
  WorldRuntimeContext *worldRuntime;
  int gridColumn;
  int outOfBoundsAxisCount;
  uint32_t trackIndex;
  uint32_t nextTrackIndex;
  InGameConditionScheduleImageView480 *scheduledCondition;
  bool voicePlaying;
  FieldGridCoordinatesEaxEdx8 targetGridPosition;
  StatusResult pageStackStatus;
  SoundPlayResult playVoiceResult;
  uint32_t cursorFrameOrScratch;
  
  if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_CLIENT) == SESSION_NETWORK_ROLE_LOCAL) {
    if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_HOST) != SESSION_NETWORK_ROLE_LOCAL) {
      FrontendClientSession_DecrementTimeoutsAndCompactPlayers();
    }
  }
  else {
    FrontendHostSession_TickShutdownOrReadyConsensus();
  }
  if ((g_UiCommandRuntimeFlags & 0x10) == 0) {
    if ((g_UiCommandRuntimeFlags & 0x2000) == 0) {
      if ((g_UiCommandRuntimeFlags & 0x20) != 0) {
        g_UiCommandRuntimeFlags = g_UiCommandRuntimeFlags | 0x2000;
        FieldGrid_SetAllCellOverlayColors
                  (0xff808080,(endGameResultsRuntime->worldRuntime0A30).fieldGrid);
        WorldRuntime_EmitModelDefinitionOverlayForMatchingEntries
                  ((void *)g_InGamePendingPlacementArmyAsset,
                   &endGameResultsRuntime->worldRuntime0A30);
      }
    }
    else if ((g_UiCommandRuntimeFlags & 0x20) == 0) {
      g_UiCommandRuntimeFlags = g_UiCommandRuntimeFlags & 0xffffdfff;
      FieldGrid_SetAllCellOverlayColors
                (0xffffffff,(endGameResultsRuntime->worldRuntime0A30).fieldGrid);
    }
    else if ((g_GameFactionRuntimeImage.tail.simulationTick & 7) == 0) {
      FieldGrid_SetAllCellOverlayColors
                (0xff808080,(endGameResultsRuntime->worldRuntime0A30).fieldGrid);
      WorldRuntime_EmitModelDefinitionOverlayForMatchingEntries
                ((void *)g_InGamePendingPlacementArmyAsset,&endGameResultsRuntime->worldRuntime0A30)
      ;
    }
    cursorFrameOrScratch = 0;
    hoveredNode = (*((endGameResultsRuntime->rootUi0000).base.vtable)->hitTest)
                       (g_CursorOverrideY,g_CursorOverrideX,(UiNodeBase *)endGameResultsRuntime);
    if (hoveredNode != (UiNodeBase *)0xffffffff) {
      cursorFrameOrScratch = (*hoveredNode->vtable->pointerMove)(g_CursorOverrideY,g_CursorOverrideX,hoveredNode);
    }
    g_GameFactionRuntimeImage.tail.presentationTick =
         g_GameFactionRuntimeImage.tail.presentationTick + 1;
    RecentTextHistory_SortAndBuildPointerList(8,&endGameResultsRuntime->recentTextHistory09B8);
    currentPresentationTick = g_GameFactionRuntimeImage.tail.presentationTick;
    worldRuntime = &endGameResultsRuntime->worldRuntime0A30;
    InGameHud_UpdateStatusCountersAndSessionPrompts();
    pageStackStatus = UiPageStack_ActivePageNotInListCf(&endGameResultsRuntime->endGameResultsPageStack09DC);
    if ((((((endGameResultsRuntime->worldRuntime0A30).runtimeFlags & 0x90) == 0) &&
         (pageStackStatus.valueOrError == 0)) &&
        (((endGameResultsRuntime->worldRuntime0A30).interaction.interactionFlags48 & 8) == 0)) &&
       (((g_CursorButtonState & 4) == 0 &&
        (candidateFrameOrScore = WorldRuntime_ApplyEdgeScrollAndGetCursorFrame(worldRuntime), candidateFrameOrScore != 0)))) {
      cursorFrameOrScratch = candidateFrameOrScore;
    }
    (*g_GraphicsCursorSetFrame)(cursorFrameOrScratch);
    cameraFieldGrid = (endGameResultsRuntime->worldRuntime0A30).fieldGrid;
    outOfBoundsAxisCount = 0;
    targetGridPosition = FieldGrid_WorldToGridQ12
                       ((endGameResultsRuntime->worldRuntime0A30).motion.targetPositionYQ12,
                        (endGameResultsRuntime->worldRuntime0A30).motion.targetPositionXQ12);
    targetRowQ12 = targetGridPosition.rowQ12;
    columnDeltaOrCount = targetGridPosition.columnQ12;
    gridColumn = (columnDeltaOrCount >> 0xc) + -8;
    gridRowOrDeltaY = (targetRowQ12 >> 0xc) + -8;
    if (gridColumn < -0x10) {
      columnDeltaOrCount = -0x8000;
      outOfBoundsAxisCount = 1;
    }
    else if ((int)cameraFieldGrid->gridWidth < gridColumn) {
      outOfBoundsAxisCount = 1;
      columnDeltaOrCount = (cameraFieldGrid->gridWidth + 8) * 0x1000;
    }
    if (gridRowOrDeltaY < -0x10) {
      targetRowQ12 = -0x8000;
      outOfBoundsAxisCount = outOfBoundsAxisCount + 1;
    }
    else if ((int)cameraFieldGrid->gridHeight < gridRowOrDeltaY) {
      outOfBoundsAxisCount = outOfBoundsAxisCount + 1;
      targetRowQ12 = (cameraFieldGrid->gridHeight + 8) * 0x1000;
    }
    if (outOfBoundsAxisCount != 0) {
      projectedProduct = (int64_t)(targetRowQ12 + columnDeltaOrCount * 2) * 0x901;
      columnDeltaOrCount = ((int)((uint64_t)projectedProduct >> 0x20) << 0x13 | (uint32_t)projectedProduct >> 0xd) -
              (endGameResultsRuntime->worldRuntime0A30).motion.targetPositionXQ12;
      gridRowOrDeltaY = ((int)((uint64_t)((int64_t)targetRowQ12 * -1999) >> 0x20) << 0x14 |
               (uint32_t)((int64_t)targetRowQ12 * -1999) >> 0xc) -
               (endGameResultsRuntime->worldRuntime0A30).motion.targetPositionYQ12;
      motionCoordinate = &(endGameResultsRuntime->worldRuntime0A30).motion.targetPositionXQ12;
      *motionCoordinate = *motionCoordinate + columnDeltaOrCount;
      motionCoordinate = &(endGameResultsRuntime->worldRuntime0A30).motion.targetPositionYQ12;
      *motionCoordinate = *motionCoordinate + gridRowOrDeltaY;
      cameraMotion = &(endGameResultsRuntime->worldRuntime0A30).motion;
      cameraMotion->positionXQ12 = cameraMotion->positionXQ12 + columnDeltaOrCount;
      motionCoordinate = &(endGameResultsRuntime->worldRuntime0A30).motion.positionYQ12;
      *motionCoordinate = *motionCoordinate + gridRowOrDeltaY;
      WorldRuntime_ClearFieldGridDirtyFlag(worldRuntime);
    }
    if ((g_UiCommandRuntimeFlags & 4) == 0) {
      TerrainDirectionTable_AdvanceAndRebuildVectors();
      cursorFrameOrScratch = PersistentSettings_ReadDword(3,0x20);
      if ((cursorFrameOrScratch & 1) != 0) {
        if ((currentPresentationTick & 7) == 0) {
          SpatialSoundPool_ClearDesiredGains();
          for (armyRuntime = (ArmyRuntimeSlot *)
                             (endGameResultsRuntime->worldRuntime0A30).ownerListHead;
              armyRuntime != (ArmyRuntimeSlot *)0x0;
              armyRuntime = (ArmyRuntimeSlot *)armyRuntime->modelNodeRuntime) {
            (*(&g_RuntimeMaintenanceCallbackPhases.audioRefresh.army)[armyRuntime->runtimeStateA4])
                      (worldRuntime,armyRuntime);
          }
          SpatialSoundPool_ApplyDesiredGains();
          InGameSelectionDetailPanel_Rebuild();
        }
        if (g_InGameEffectsEnabled == 0) {
          voicePlaying = (*g_SoundIsVoicePlaying)(g_InGameActiveEffectVoice);
          if (voicePlaying) {
            g_InGameActiveEffectVoice = (IDirectSoundBuffer *)0x0;
            cursorFrameOrScratch = Random_NextPrimary();
            g_InGameEffectsEnabled = (cursorFrameOrScratch & 0x3f) + 1;
          }
        }
        else {
          g_InGameEffectsEnabled = g_InGameEffectsEnabled - 1;
          if (g_InGameEffectsEnabled == 0) {
            cursorFrameOrScratch = PersistentSettings_ReadDword(0x8000,0x24);
            candidateFrameOrScore = Random_NextPrimary();
            playVoiceResult = (*g_SoundPlayOneShot)
                               (cursorFrameOrScratch,cursorFrameOrScratch,
                                (DirectSoundVoiceSet *)(&g_InGameLevelEffectVoiceSet0)[candidateFrameOrScore & 3]);
            if (!playVoiceResult.failed) {
              g_InGameActiveEffectVoice = playVoiceResult.soundBuffer;
            }
          }
        }
      }
      cursorFrameOrScratch = PersistentSettings_ReadDword(3,0x20);
      levelConditionStorage = g_InGameLevelRuntimeGlobalBlock.conditionStorage;
      if ((cursorFrameOrScratch & 2) != 0) {
        if (g_InGameMusicEnabled == 0) {
          voicePlaying = (*g_SoundIsVoicePlaying)(g_InGameActiveMusicVoice);
          if (voicePlaying) {
            g_InGameActiveMusicVoice = (IDirectSoundBuffer *)0x0;
            cursorFrameOrScratch = Random_NextPrimary();
            g_InGameMusicEnabled = (cursorFrameOrScratch & 0x3f) + 1;
          }
        }
        else {
          g_InGameMusicEnabled = g_InGameMusicEnabled - 1;
          if (g_InGameMusicEnabled == 0) {
            cursorFrameOrScratch = 0;
            bestTrackOrSecondsLeft = 0;
            trackIndex = 0;
            do {
              candidateFrameOrScore = InGameMusic_ComputeTrackSuitabilityScore
                                ((levelConditionStorage->levelImage).runtimeTail2E0.musicSampleNumbers[trackIndex],
                                 worldRuntime);
              nextTrackIndex = trackIndex + 1;
              if ((int)cursorFrameOrScratch < (int)candidateFrameOrScore) {
                bestTrackOrSecondsLeft = trackIndex;
                cursorFrameOrScratch = candidateFrameOrScore;
              }
              trackIndex = nextTrackIndex;
            } while (nextTrackIndex < 4);
            if (cursorFrameOrScratch != 0) {
              selectedMusicTrackId = (levelConditionStorage->levelImage).runtimeTail2E0.musicSampleNumbers[bestTrackOrSecondsLeft];
              cursorFrameOrScratch = PersistentSettings_ReadDword(0x8000,0x2c);
              g_EndGameResultsCurrentMusicTrackId = selectedMusicTrackId;
              playVoiceResult = (*g_SoundPlayOneShot)
                                 (cursorFrameOrScratch,cursorFrameOrScratch,
                                  (DirectSoundVoiceSet *)(&g_InGameLevelMusicVoiceSet0)[bestTrackOrSecondsLeft]);
              if (!playVoiceResult.failed) {
                g_InGameActiveMusicVoice = playVoiceResult.soundBuffer;
              }
            }
          }
        }
      }
      if ((g_UiCommandRuntimeFlags & 1) != 0)
      goto EndGameResultsUiRuntime_UpdateAndHandleInput_UpdateCursorGridAndReturn;
      pageStackStatus = UiPageStack_ActivePageNotInListCf(&endGameResultsRuntime->gameWindowPageStack0BD0);
      if (pageStackStatus.valueOrError == 2) {
        InGameTechnologyPanel_Rebuild(&endGameResultsRuntime->rootUi0000);
      }
      InterpolationStateTable_Advance256ByTicks(g_InGameSimulationStepTicks);
      if (g_KeyboardSpecialKeyDown[0x14] != 0) {
        cursorFrameOrScratch = PersistentSettings_ReadDword(0x20,0x48);
        WorldRuntime_TranslateCameraByScreenDelta(0,-cursorFrameOrScratch,worldRuntime);
        WorldRuntime_RecomputeMotionEndpointAgainstFieldSurface(worldRuntime);
      }
      if (g_KeyboardSpecialKeyDown[0x16] != 0) {
        cursorFrameOrScratch = PersistentSettings_ReadDword(0x20,0x48);
        WorldRuntime_TranslateCameraByScreenDelta(0,cursorFrameOrScratch,worldRuntime);
        WorldRuntime_RecomputeMotionEndpointAgainstFieldSurface(worldRuntime);
      }
      if (g_KeyboardSpecialKeyDown[0x11] != 0) {
        cursorFrameOrScratch = PersistentSettings_ReadDword(0x20,0x48);
        WorldRuntime_TranslateCameraByScreenDelta(-cursorFrameOrScratch,0,worldRuntime);
        WorldRuntime_RecomputeMotionEndpointAgainstFieldSurface(worldRuntime);
      }
      if (g_KeyboardSpecialKeyDown[0x19] != 0) {
        cursorFrameOrScratch = PersistentSettings_ReadDword(0x20,0x48);
        WorldRuntime_TranslateCameraByScreenDelta(cursorFrameOrScratch,0,worldRuntime);
        WorldRuntime_RecomputeMotionEndpointAgainstFieldSurface(worldRuntime);
      }
      if (g_KeyboardSpecialKeyDown[0x12] != 0) {
        clampedPitchAngle = (endGameResultsRuntime->worldRuntime0A30).motion.pitchAngle - 0x400;
        if ((int)clampedPitchAngle < (int)(endGameResultsRuntime->worldRuntime0A30).motion.minimumPitchAngle) {
          clampedPitchAngle = (endGameResultsRuntime->worldRuntime0A30).motion.minimumPitchAngle;
        }
        WorldRuntime_SetPosition80AndRebuildPosition60FromAngles
                  (clampedPitchAngle,(endGameResultsRuntime->worldRuntime0A30).motion.headingAngle,
                   (endGameResultsRuntime->worldRuntime0A30).motion.targetDistanceQ12,
                   (endGameResultsRuntime->worldRuntime0A30).motion.targetPositionZQ12,
                   (endGameResultsRuntime->worldRuntime0A30).motion.targetPositionYQ12,
                   (endGameResultsRuntime->worldRuntime0A30).motion.targetPositionXQ12,worldRuntime)
        ;
      }
      if (g_KeyboardSpecialKeyDown[0x1a] != 0) {
        clampedPitchAngle = (endGameResultsRuntime->worldRuntime0A30).motion.pitchAngle + 0x400;
        if ((int)(endGameResultsRuntime->worldRuntime0A30).motion.maximumPitchAngle < (int)clampedPitchAngle) {
          clampedPitchAngle = (endGameResultsRuntime->worldRuntime0A30).motion.maximumPitchAngle;
        }
        WorldRuntime_SetPosition80AndRebuildPosition60FromAngles
                  (clampedPitchAngle,(endGameResultsRuntime->worldRuntime0A30).motion.headingAngle,
                   (endGameResultsRuntime->worldRuntime0A30).motion.targetDistanceQ12,
                   (endGameResultsRuntime->worldRuntime0A30).motion.targetPositionZQ12,
                   (endGameResultsRuntime->worldRuntime0A30).motion.targetPositionYQ12,
                   (endGameResultsRuntime->worldRuntime0A30).motion.targetPositionXQ12,worldRuntime)
        ;
      }
      if (g_KeyboardSpecialKeyDown[7] != 0) {
        WorldRuntime_SetPosition80AndRebuildPosition60FromAngles
                  ((endGameResultsRuntime->worldRuntime0A30).motion.pitchAngle,
                   (endGameResultsRuntime->worldRuntime0A30).motion.headingAngle - 0x400 & 0xffff,
                   (endGameResultsRuntime->worldRuntime0A30).motion.targetDistanceQ12,
                   (endGameResultsRuntime->worldRuntime0A30).motion.targetPositionZQ12,
                   (endGameResultsRuntime->worldRuntime0A30).motion.targetPositionYQ12,
                   (endGameResultsRuntime->worldRuntime0A30).motion.targetPositionXQ12,worldRuntime)
        ;
      }
      if (g_KeyboardSpecialKeyDown[6] != 0) {
        WorldRuntime_SetPosition80AndRebuildPosition60FromAngles
                  ((endGameResultsRuntime->worldRuntime0A30).motion.pitchAngle,
                   (endGameResultsRuntime->worldRuntime0A30).motion.headingAngle + 0x400 & 0xffff,
                   (endGameResultsRuntime->worldRuntime0A30).motion.targetDistanceQ12,
                   (endGameResultsRuntime->worldRuntime0A30).motion.targetPositionZQ12,
                   (endGameResultsRuntime->worldRuntime0A30).motion.targetPositionYQ12,
                   (endGameResultsRuntime->worldRuntime0A30).motion.targetPositionXQ12,worldRuntime)
        ;
      }
      if (g_KeyboardSpecialKeyDown[0x10] != 0) {
        clampedTargetDistance = (endGameResultsRuntime->worldRuntime0A30).motion.targetDistanceQ12 - 0x800;
        if ((int)clampedTargetDistance < (int)(endGameResultsRuntime->worldRuntime0A30).minimumCameraDistanceQ12) {
          clampedTargetDistance = (endGameResultsRuntime->worldRuntime0A30).minimumCameraDistanceQ12;
        }
        WorldRuntime_SetPosition80AndRebuildPosition60FromAngles
                  ((endGameResultsRuntime->worldRuntime0A30).motion.pitchAngle,
                   (endGameResultsRuntime->worldRuntime0A30).motion.headingAngle,clampedTargetDistance,
                   (endGameResultsRuntime->worldRuntime0A30).motion.targetPositionZQ12,
                   (endGameResultsRuntime->worldRuntime0A30).motion.targetPositionYQ12,
                   (endGameResultsRuntime->worldRuntime0A30).motion.targetPositionXQ12,worldRuntime)
        ;
      }
      if (g_KeyboardSpecialKeyDown[0x18] != 0) {
        clampedTargetDistance = (endGameResultsRuntime->worldRuntime0A30).motion.targetDistanceQ12 + 0x800;
        if ((int)(endGameResultsRuntime->worldRuntime0A30).maximumCameraDistanceQ12 < (int)clampedTargetDistance) {
          clampedTargetDistance = (endGameResultsRuntime->worldRuntime0A30).maximumCameraDistanceQ12;
        }
        WorldRuntime_SetPosition80AndRebuildPosition60FromAngles
                  ((endGameResultsRuntime->worldRuntime0A30).motion.pitchAngle,
                   (endGameResultsRuntime->worldRuntime0A30).motion.headingAngle,clampedTargetDistance,
                   (endGameResultsRuntime->worldRuntime0A30).motion.targetPositionZQ12,
                   (endGameResultsRuntime->worldRuntime0A30).motion.targetPositionYQ12,
                   (endGameResultsRuntime->worldRuntime0A30).motion.targetPositionXQ12,worldRuntime)
        ;
      }
      levelConditionStorage = g_InGameLevelRuntimeGlobalBlock.conditionStorage;
      endGameResultsRuntime->sessionTimerNodeFlags44C0 =
           endGameResultsRuntime->sessionTimerNodeFlags44C0 & ~UI_NODE_SUPPRESSED;
      currentPresentationTick = g_GameFactionRuntimeImage.tail.presentationTick;
      columnDeltaOrCount = 0x40;
      scheduledCondition = &levelConditionStorage->schedule;
      do {
        if (((scheduledCondition->conditions[0].statusAndKind.kind & 0xfe) ==
             INGAME_SCHEDULED_CONDITION_COUNTDOWN_ELAPSED) &&
           (bestTrackOrSecondsLeft = scheduledCondition->conditions[0].payload.operands[1], bestTrackOrSecondsLeft != 0)) {
          cursorFrameOrScratch = (*g_WideNumberFormatUtf16)
                             (WIDE_FORMAT_PAD_WITH_SPACE,0,2,1,bestTrackOrSecondsLeft / 0x3c,(uint16_t *)THANDOR_ADDR(g_InGameCountdownTextUtf16,0));
          *(uint16_t *)(cursorFrameOrScratch + THANDOR_ADDR(g_InGameCountdownTextUtf16,0)) = 0x3a;
          (*g_WideNumberFormatUtf16)
                    (WIDE_FORMAT_WRITE_TERMINATOR|WIDE_FORMAT_PAD_WITH_ZERO,0,2,1,bestTrackOrSecondsLeft % 0x3c,
                     (uint16_t *)(cursorFrameOrScratch + THANDOR_ADDR(g_InGameCountdownTextUtf16,0x2)));
          currentPresentationTick = g_GameFactionRuntimeImage.tail.presentationTick;
          goto 
          EndGameResultsUiRuntime_UpdateAndHandleInput_RefreshTerrainCompositeOnPresentationCadence;
        }
        scheduledCondition = (InGameConditionScheduleImageView480 *)(scheduledCondition->conditions + 1);
        columnDeltaOrCount = columnDeltaOrCount + -1;
      } while (columnDeltaOrCount != 0);
      endGameResultsRuntime->sessionTimerNodeFlags44C0 =
           endGameResultsRuntime->sessionTimerNodeFlags44C0 | UI_NODE_SUPPRESSED;
    }
EndGameResultsUiRuntime_UpdateAndHandleInput_RefreshTerrainCompositeOnPresentationCadence:
    if ((currentPresentationTick & 0x1f) == 0) {
      TerrainCompositeTexture_FillPlane1();
    }
    if ((currentPresentationTick & 3) == 0) {
      TerrainCompositeTexture_RebuildPlane0();
    }
  }
EndGameResultsUiRuntime_UpdateAndHandleInput_UpdateCursorGridAndReturn:
  InGameRuntime_UpdateCursorGridAndViewScaleCache();
  return;
}


/* Address: 0x0050EA90.
   Ownership: gameplay/session/runtime.
   Purpose: Rebases exact 0x100-byte loaded condition records. RET 4 proves one stack argument and removes two
   false register parameters.
   Cross-module calls: SpriteAssetRegistry_FindById [assets/sprite/catalog].
*/
void __thandor_void_preserve_eax_ecx_edx
ResourceRegistrationRuntime_RebaseLoadedRecords(ResourceRegistrationRuntimeImage *runtimeImage)

{
  void *tailNestedPointer;
  FrontendPlayerRuntimeRecord *playerRuntimeBlocks;
  uint8_t *primaryPointer;
  uint8_t *auxiliaryPointer;
  GraphicsTextureSet *selectedTextureSet;
  SpriteAssetHeader *resolvedSprite;
  uint8_t *secondaryPointer;
  uint32_t nestedRemaining;
  GraphicsPaletteAsset *selectedPalette;
  ResourceRegistrationRecord100 *registrationRecord;
  uint32_t remainingRecords;
  uint8_t *nestedBasePointer;
  ResourceRegistrationRecord100 *nestedCursor;
  ArmyRuntimeSlot *payloadSlot;
  
  registrationRecord = runtimeImage->records58;
  remainingRecords = runtimeImage->recordCountAC;
  do {
    if ((registrationRecord->flags & RUNTIME_REGISTRATION_RECORD_ALLOCATED) != 0) {
      primaryPointer = (uint8_t *)(registrationRecord->primaryPointerOrSavedOffset).savedIdOrOffset;
      secondaryPointer = (registrationRecord->secondaryPointerOrSavedOffset).runtimePointer;
      nestedBasePointer = (registrationRecord->nestedBasePointerOrSavedOffset).runtimePointer;
      if (primaryPointer != (uint8_t *)0x0) {
        primaryPointer = primaryPointer + (int)g_RuntimeObjectRebaseBaseMinusOne;
      }
      if (secondaryPointer != (uint8_t *)0x0) {
        secondaryPointer = secondaryPointer + (int)g_RuntimeObjectRebaseBaseMinusOne;
      }
      if (nestedBasePointer != (uint8_t *)0x0) {
        nestedBasePointer = nestedBasePointer + (int)g_RuntimeObjectRebaseBaseMinusOne;
      }
      (registrationRecord->primaryPointerOrSavedOffset).savedIdOrOffset = (uint32_t)primaryPointer;
      (registrationRecord->secondaryPointerOrSavedOffset).runtimePointer = secondaryPointer;
      (registrationRecord->nestedBasePointerOrSavedOffset).runtimePointer = nestedBasePointer;
      (registrationRecord->ownerRuntimeOrSavedOffset).runtimePointer = runtimeImage;
      auxiliaryPointer = (uint8_t *)(registrationRecord->auxiliaryPointerOrSavedOffset).savedIdOrOffset;
      nestedRemaining = registrationRecord->nestedCountC8;
      if (auxiliaryPointer != (uint8_t *)0x0) {
        /* 1-based offset from the shading records; 0 is null */
        auxiliaryPointer = (uint8_t *)(THANDOR_ADDR(g_GraphicsShadingRuntimeRecords,-1) + (int)auxiliaryPointer);
      }
      (registrationRecord->auxiliaryPointerOrSavedOffset).savedIdOrOffset = (uint32_t)auxiliaryPointer;
      nestedCursor = registrationRecord;
      selectedPalette = g_ShotPalette;
      for (; g_ShotPalette = selectedPalette, nestedRemaining != 0; nestedRemaining = nestedRemaining - 1) {
        if (nestedCursor->nestedPointerOrOffsetArray13[0].runtimePointer != (void *)0x0) {
          nestedCursor->nestedPointerOrOffsetArray13[0].runtimePointer =
               (uint8_t *)((int)nestedCursor->nestedPointerOrOffsetArray13[0].runtimePointer +
                       (int)g_RuntimeObjectRebaseBaseMinusOne);
        }
        nestedCursor = (ResourceRegistrationRecord100 *)&nestedCursor->secondaryPointerOrSavedOffset;
        selectedPalette = g_ShotPalette;
      }
      payloadSlot = (registrationRecord->runtimePayload).armyRuntime;
                    // WARNING: Switch is manually overridden
      switch(registrationRecord->domainIndex) {
      case RESOURCE_DOMAIN_ARMY_RUNTIME:
        payloadSlot = (ArmyRuntimeSlot *)
                     ((int)&payloadSlot->modelRuntimeOrSavedOffset + g_ModelRuntimeRebaseDelta);
        selectedPalette = g_ArmyGraphicsBindings[(int)registrationRecord->textureSet].paletteAsset;
        registrationRecord->textureSet = g_ArmyGraphicsBindings[(int)registrationRecord->textureSet].textureSet;
        registrationRecord->paletteAsset = selectedPalette;
        break;
      case RESOURCE_DOMAIN_SHOT_RUNTIME:
        payloadSlot = (ArmyRuntimeSlot *)
                     (g_ShotRuntimeRebaseBaseMinusOne + (int)&payloadSlot->modelRuntimeOrSavedOffset)
        ;
        registrationRecord->textureSet = g_ShotTextureSet;
        registrationRecord->paletteAsset = selectedPalette;
        break;
      case RESOURCE_DOMAIN_EFFECT_RUNTIME:
        payloadSlot = (ArmyRuntimeSlot *)
                     (g_EffectRuntimeRebaseBaseMinusOne +
                     (int)&payloadSlot->modelRuntimeOrSavedOffset);
        selectedTextureSet = g_EffectTextureSet;
        selectedPalette = g_EffectPalette;
        if ((*(uint32_t *)(((payloadSlot->modelRuntimeOrSavedOffset).modelRuntime)->reserved10_37 + 0x20)
            & 2) != 0) {
          selectedTextureSet = g_ArmyGraphicsBindings[0].textureSet;
          selectedPalette = g_ArmyGraphicsBindings[0].paletteAsset;
        }
        registrationRecord->textureSet = selectedTextureSet;
        registrationRecord->paletteAsset = selectedPalette;
      }
      (registrationRecord->runtimePayload).armyRuntime = payloadSlot;
      resolvedSprite = SpriteAssetRegistry_FindById((SpriteAssetId)registrationRecord->spriteAsset);
      registrationRecord->spriteAsset = resolvedSprite;
    }
    playerRuntimeBlocks = g_FrontendPlayerRuntimeBlocks;
    registrationRecord = registrationRecord + 1;
    remainingRecords = remainingRecords - 1;
  } while (remainingRecords != 0);
  tailNestedPointer = runtimeImage->records58[runtimeImage->recordCountAC - 1].nestedPointerOrOffsetArray13
           [0xc].runtimePointer;
  registrationRecord = (ResourceRegistrationRecord100 *)0x0;
  if (tailNestedPointer != (void *)0x0) {
    registrationRecord = (ResourceRegistrationRecord100 *)(g_RuntimeObjectRebaseBaseMinusOne + (int)tailNestedPointer);
  }
  runtimeImage->tailRecordD8 = registrationRecord;
  (playerRuntimeBlocks->factionAssignment).factionAssignmentIndex = runtimeImage->levelRuntimeRecordIndex50;
  return;
}


/* Address: 0x00565E10.
   Ownership: gameplay/session/runtime.
   Purpose: In-game periodic timer callback. It decrements the startup/countdown field when nonzero and advances
   the shared runtime clock while the pause byte is clear.
*/
void __cdecl InGameRuntime_PeriodicCountdownAndClockTick(void)

{
  if (g_InGameNetworkTickCountdown != 0) {
    g_InGameNetworkTickCountdown = g_InGameNetworkTickCountdown + -1;
  }
  if (g_InGameResourceRegistrationBusyCount == '\0') {
    g_GameFactionRuntimeImage.tail.periodicClockTick =
         g_GameFactionRuntimeImage.tail.periodicClockTick + 1;
  }
  return;
}

/* Address: 0x00567060.
   Ownership: gameplay/session/runtime.
   Purpose: In-game hotkeys (the in-game root's keyboard fallback, also installed by the frontend for
   the running game): chat, windows, save, quit, panels and the cheat keys, selected by command code and
   modifier flags. Typed parameters: p0
   modifierFlags→UiKeyboardStateMask_V297, p1 commandCode→UiActionId_V338. Nearby but non-identical semantic
   domains were explicitly deferred. Calling convention, parameter storage, body bytes, control flow, globals,
   locals, and executable data remain unchanged.
*/
bool __thandor_cf_preserve_eax_ecx_edx
InGameHotkeys_DispatchCommandByFlagsCf
          (UiKeyboardStateMask modifierFlags,UiActionId commandCode,
          EndGameResultsRuntimeView44C4 *endGameResultsRuntime)

{
  /* Rewritten from the assembly (0x00567060-0x005678B9). The record table holds continuation
     addresses inside this function; the decompiled version jumped into the original machine code,
     which then called the recovered C functions with the wrong calling convention (crash on ESC
     after loading). Each continuation is translated below; EBX is the runtime root. */
  uint8_t *rt = (uint8_t *)endGameResultsRuntime;
  UiCommandDispatchRecord *record = g_EndGameResultsCommandDispatchRecords_00_Code00030071_Modifier30;
  uint32_t target = 0;
  bool localSession = (g_SessionNetworkRoleFlags & 3) == 0;

  for (;; record++) {
    uint32_t flags = record->modifierClassFlags;
    if (record->commandCode == 0) {
      return false;
    }
    if (record->commandCode != commandCode) {
      continue;
    }
    if (flags == 0) {
      if ((modifierFlags & 0x3c) != 0) continue;
    }
    else if ((flags & 0x30) == 0) {
      if (((modifierFlags & 0xc) == 0) || ((modifierFlags & 0x30) != 0)) continue;
    }
    else if ((flags & 0xc) == 0) {
      if (((modifierFlags & 0xc) != 0) || ((modifierFlags & 0x30) == 0)) continue;
    }
    else {
      if (((modifierFlags & 0xc) == 0) || ((modifierFlags & 0x30) == 0)) continue;
    }
    target = (uint32_t)record->continuationEntryAddress;
    break;
  }
  switch (target) {
  case 0x5671e0: /* cheat: toggle runtime flag 0x100000 */
    if ((g_UiCommandRuntimeFlags & 0x40000) != 0) {
      g_UiCommandRuntimeFlags = g_UiCommandRuntimeFlags ^ 0x100000;
    }
    break;
  case 0x567200: /* cheat: add xenite */
    if ((g_UiCommandRuntimeFlags & 0x40000) != 0) {
      *(uint32_t *)((uint8_t *)&g_GameFactionRuntimeImage + ((WorldRuntimeContext *)INGAME_UI(rt,worldView))->activeFactionRuntimeIndex * 0x740) += 0x3e80;
    }
    break;
  case 0x567230: /* cheat: add energy */
    if ((g_UiCommandRuntimeFlags & 0x40000) != 0) {
      *(uint32_t *)((uint8_t *)&g_GameFactionRuntimeImage + ((WorldRuntimeContext *)INGAME_UI(rt,worldView))->activeFactionRuntimeIndex * 0x740 + 0x20) += 0x640;
      *(uint32_t *)((uint8_t *)&g_GameFactionRuntimeImage + ((WorldRuntimeContext *)INGAME_UI(rt,worldView))->activeFactionRuntimeIndex * 0x740 + 0x24) += 0x640;
    }
    break;
  case 0x567270:
    UiPageStack_SetActiveIndex(1,(UiPageStackControl *)INGAME_UI(rt,chatInputPageStack));
    ((UiTextEditControl *)INGAME_UI(rt,chatInputTextEdit))->cursorIndex = 0;
    ((UiTextEditControl *)INGAME_UI(rt,chatInputTextEdit))->selectionStart = 0;
    ((UiTextEditControl *)INGAME_UI(rt,chatInputTextEdit))->selectionEnd = 0;
    if (!localSession) {
      SelectableGroupNodeResult visible;
      int i;
      for (i = 0; i < 0x18; i++) {
        INGAME_UI_FIELD(rt,chatInputTextEdit,0x6c + i * 4,uint32_t) = 0;
      }
      visible = UiSelectableGroup_NoneVisibleSelectedCf(3,INGAME_UI(rt,messageRecipientAllTab),INGAME_UI(rt,messageRecipientGroupsTab),
                                                    INGAME_UI(rt,messageRecipientPlayersTab));
      (*(void (**)(void *))(uintptr_t)(THANDOR_ADDR(g_InGameUiActionHandlersPage10,0) + (*(uint32_t *)((uint8_t *)visible.node + 0x50) & 0xff) * 4))
                (visible.node);
    }
    UiKeyboardFocus_Set(INGAME_UI(rt,chatInputTextEdit));
    break;
  case 0x567340:
  case 0x5673a0:
  case 0x567410:
  case 0x567460: {
    UiSelectableControl *toggle;
    if ((target == 0x5673a0) && !localSession) {
      break;
    }
    toggle = (UiSelectableControl *)(target == 0x567460 ? INGAME_UI(rt,missionObjectivesButton) :
                                                     INGAME_UI(rt,inGameMenuButton));
    UiSelectableControl_SetSelected(1,toggle);
    if (((*(uint32_t *)((uint8_t *)toggle + 0x4c) & 0x200) != 0) &&
        (*(uint32_t *)((uint8_t *)toggle + 0x70) != 0)) {
      (*g_SoundPlayOneShot)(g_UiSoundGainQ15,g_UiSoundGainQ15,
                            *(DirectSoundVoiceSet **)((uint8_t *)toggle + 0x70));
    }
    if (target == 0x567460) {
      InGameUiAction101F_Handler((UiNodeBase *)toggle);
      break;
    }
    InGameSettingsPage_ToggleAndSynchronizeControls(toggle);
    if (target == 0x567340) {
      InGameCommandPanel_OpenPage4AndRefreshAvailability((InGameCommandPanelSourceAddress32)INGAME_UI(rt,gameMenuQuitButton));
    }
    else if (target == 0x5673a0) {
      InGameSaveGamePage_RebuildCatalog(INGAME_UI(rt,gameMenuSaveButton));
    }
    break;
  }
  case 0x5674b0: {
    UiPageStackControl *stack;
    uint32_t index;
    SelectableGroupNodeResult visible;
    int i;
    if (localSession) {
      break;
    }
    stack = (UiPageStackControl *)INGAME_UI(rt,gameWindowPageStack);
    index = (UiPageStack_ActivePageNotInListCf(stack).valueOrError == 1) ? 0 : 1;
    UiPageStack_SetActiveIndex(index,stack);
    INGAME_UI(rt,worldView)->nodeFlags =
         INGAME_UI(rt,worldView)->nodeFlags & ~UI_NODE_SUPPRESSED;
    if (index != 1) {
      break;
    }
    INGAME_UI(rt,worldView)->nodeFlags =
         INGAME_UI(rt,worldView)->nodeFlags | UI_NODE_SUPPRESSED;
    UiKeyboardFocus_ReleaseNode(INGAME_UI(rt,worldView));
    ((UiTextEditControl *)INGAME_UI(rt,messageTextEdit))->cursorIndex = 0;
    ((UiTextEditControl *)INGAME_UI(rt,messageTextEdit))->selectionStart = 0;
    ((UiTextEditControl *)INGAME_UI(rt,messageTextEdit))->selectionEnd = 0;
    for (i = 0; i < 0x18; i++) {
      INGAME_UI_FIELD(rt,messageTextEdit,0x6c + i * 4,uint32_t) = 0;
    }
    visible = UiSelectableGroup_NoneVisibleSelectedCf(3,INGAME_UI(rt,messageRecipientAllTab),INGAME_UI(rt,messageRecipientGroupsTab),
                                                    INGAME_UI(rt,messageRecipientPlayersTab));
    (*(void (**)(void *))(uintptr_t)(THANDOR_ADDR(g_InGameUiActionHandlersPage10,0) + (*(uint32_t *)((uint8_t *)visible.node + 0x50) & 0xff) * 4))
              (visible.node);
    (*g_KeyboardFlushEvents)();
    break;
  }
  case 0x5675e0: /* pause */
    if (localSession) {
      InGameCommandMode_TogglePlayerFlagBit0AndReconcileGlobal(g_LocalPlayerRuntimeId,0,0,0);
    }
    else {
      InGameCommandQueue_AppendLocalPlayerCommand(0x370,0,0,0);
    }
    break;
  case 0x567620: /* faster */
  case 0x567660: /* slower */ {
    int step = (target == 0x567620) ? 1 : -1;
    if (localSession) {
      InGameSimulationSpeed_AdjustPlayerAndRecomputeMinimumTicks(g_LocalPlayerRuntimeId,0,0,step);
    }
    else {
      InGameCommandQueue_AppendLocalPlayerCommand(0x3f0,0,0,step);
    }
    break;
  }
  case 0x5676a0: {
    uint32_t settings = PersistentSettings_ReadDword(0,0x40);
    UiPageStackControl *stack = (UiPageStackControl *)INGAME_UI(rt,sidePanelStack);
    if (UiPageStack_ActivePageNotInListCf(stack).valueOrError != 0) {
      UiPageStack_SetActiveIndex(0,stack);
      UiPageStack_SetActiveIndex(0,(UiPageStackControl *)INGAME_UI(rt,resourceBarModeStack));
      UiPageStack_SetActiveIndex(0,(UiPageStackControl *)INGAME_UI(rt,gamePanelsModeStack));
      INGAME_UI(rt,worldViewArea)->rightOffset = INGAME_UI(rt,sidePanelFrameLeftEdge)->leftOffset;
      settings = settings & 0xfffffffb;
    }
    else {
      UiPageStack_SetActiveIndex(1,stack);
      UiPageStack_SetActiveIndex(0,(UiPageStackControl *)INGAME_UI(rt,resourceBarModeStack));
      UiPageStack_SetActiveIndex(0,(UiPageStackControl *)INGAME_UI(rt,gamePanelsModeStack));
      INGAME_UI(rt,worldViewArea)->rightOffset = 0;
      settings = settings | 4;
    }
    UiContainer_LayoutChildren((UiNodeBase *)rt);
    PersistentSettings_WriteDword(settings,0x40);
    break;
  }
  case 0x5677e0: { /* screenshot */
    FramebufferCaptureResult capture =
         (*g_GraphicsFramebufferCaptureRegion)(g_FramebufferHeight,g_FramebufferWidth,0,0);
    PcxEncodeResult pcx;
    uint16_t *digitHigh = (uint16_t *)(uintptr_t)THANDOR_ADDR(g_ScreenshotFileNameUtf16,0xc);
    uint16_t *digitLow = (uint16_t *)(uintptr_t)THANDOR_ADDR(g_ScreenshotFileNameUtf16,0xe);
    if (capture.failed) {
      break;
    }
    pcx = (*g_PcxFunctionExport3)(g_PcxFunctionModule,capture.capture);
    if (pcx.failed) {
      (*g_MemoryApi.free)(capture.capture);
      break;
    }
    FileSystem_WriteBufferToPathCf(pcx.encodedByteCount,pcx.encodedBytesOrError,
                                   (uint16_t *)(uintptr_t)THANDOR_ADDR(g_ScreenshotFileNameUtf16,0));
    (*g_MemoryApi.free)(pcx.encodedBytesOrError);
    (*g_MemoryApi.free)(capture.capture);
    *digitLow = *digitLow + 1;
    if (*digitLow > 0x39) {
      *digitHigh = *digitHigh + 1;
      *digitLow = *digitLow - 10;
      if (*digitHigh > 0x39) {
        *digitHigh = *digitHigh - 10;
      }
    }
    break;
  }
  case 0x567870:
    if ((g_SessionNetworkRoleFlags & 2) != 0) {
      break;
    }
    if (localSession) {
      InGameCommand150_HandlePlayerDepartureAndOwnership(g_LocalPlayerRuntimeId,0,0,0);
    }
    else {
      InGameCommandQueue_AppendLocalPlayerCommand(0x150,0,0,0);
    }
    break;
  default:
    Thandor_Log("EndGameResults dispatch: unhandled continuation %08x",target);
    break;
  }
  return false;
}


/* Address: 0x00569920.
   Ownership: gameplay/session/runtime.
   Purpose: Ten-millisecond gameplay timer that advances queued session notification and status records.
   Cross-module calls: Movie_Open [movie/runtime/playback], Movie_SetAudioGainQ15 [movie/runtime/playback],
   Movie_AdvanceFrame [movie/runtime/playback], Movie_Close [movie/runtime/playback].
*/
void __thandor_void_preserve_eax_ecx_edx InGameRuntime_ProcessQueuedSessionNotificationTimer(void)

{
  uint32_t notificationMovieNumber;
  GraphicsTextureSourceAsset *panelTextureSource;
  int remainingWords;
  InGameNotificationPayload18 *sourcePayload;
  InGameNotificationQueueRecord20 *sourceRecord;
  InGameNotificationPayload18 *destinationPayload;
  InGameNotificationQueueRecord20 *destinationRecord;
  MovieFrameResult frameResult;
  MovieOpenResult openResult;
  InGameRuntimeRootImageC3E4 *inGameRoot;
  
  panelTextureSource = g_InGamePanelTextureSource;
  inGameRoot = g_InGameRuntimeRoot;
  if (((g_InGameSessionNotificationTimeoutTicks != 0) &&
      (g_InGameSessionNotificationTimeoutTicks = g_InGameSessionNotificationTimeoutTicks - 1,
      g_InGameSessionNotificationTimeoutTicks == 0)) &&
     (g_InGameRuntimeRoot->sessionNotificationInteractionState9B4C == PAYLOAD_ACTIVE)) {
    g_InGameRuntimeRoot->sessionNotificationInteractionState9B4C = NOTIFICATION_INTERACTION_NONE;
  }
  if (panelTextureSource == (GraphicsTextureSourceAsset *)inGameRoot->observedSessionNotificationValue9B50) {
    if (inGameRoot->notificationQueue9E60[0].priority04 != 0) {
      notificationMovieNumber = inGameRoot->notificationQueue9E60[0].notificationMovieId00;
      (*g_WideNumberFormatUtf16)
                (WIDE_FORMAT_PAD_WITH_ZERO,0,3,1,notificationMovieNumber,(uint16_t *)(u_flm_movie000_flm_0056314e + 9));
      openResult = Movie_Open(0x80000000,(uint16_t *)u_flm_movie000_flm_0056314e);
      if (!openResult.failed) {
        if ((99 < notificationMovieNumber) && ((notificationMovieNumber < 300 || ((699 < notificationMovieNumber && (notificationMovieNumber < 900)))))) {
          Movie_SetAudioGainQ15(g_MovieAlternateAudioGainQ15);
        }
        frameResult = Movie_AdvanceFrame();
        if (!frameResult.ended) {
          inGameRoot->observedSessionNotificationValue9B50 = frameResult.movieOrError;
          inGameRoot->notificationPlaybackCompletionCode9B54 = 0;
          sourcePayload = &inGameRoot->notificationQueue9E60[0].payload08;
          destinationPayload = &inGameRoot->activeNotificationPayload9E40;
          for (remainingWords = 6; remainingWords != 0; remainingWords = remainingWords + -1) {
            destinationPayload->primaryWorldCoordinateQ12_00 = sourcePayload->primaryWorldCoordinateQ12_00;
            sourcePayload = (InGameNotificationPayload18 *)&sourcePayload->secondaryWorldCoordinateQ12_04;
            destinationPayload = (InGameNotificationPayload18 *)&destinationPayload->secondaryWorldCoordinateQ12_04;
          }
          if (inGameRoot->sessionNotificationInteractionState9B4C == PAYLOAD_ACTIVE) {
            inGameRoot->sessionNotificationInteractionState9B4C = NOTIFICATION_INTERACTION_NONE;
          }
          g_InGameSessionNotificationTimeoutTicks = 0;
          if ((inGameRoot->activeNotificationPayload9E40).payloadKind14 != NOTIFICATION_PAYLOAD_NONE) {
            inGameRoot->sessionNotificationInteractionState9B4C = PAYLOAD_ACTIVE;
          }
        }
      }
      sourceRecord = inGameRoot->notificationQueue9E60 + 1;
      destinationRecord = inGameRoot->notificationQueue9E60;
      for (remainingWords = 0x18; remainingWords != 0; remainingWords = remainingWords + -1) {
        destinationRecord->notificationMovieId00 = sourceRecord->notificationMovieId00;
        sourceRecord = (InGameNotificationQueueRecord20 *)&sourceRecord->priority04;
        destinationRecord = (InGameNotificationQueueRecord20 *)&destinationRecord->priority04;
      }
      for (remainingWords = 8; remainingWords != 0; remainingWords = remainingWords + -1) {
        destinationRecord->notificationMovieId00 = 0;
        destinationRecord = (InGameNotificationQueueRecord20 *)&destinationRecord->priority04;
      }
    }
  }
  else {
    frameResult = Movie_AdvanceFrame();
    if (frameResult.ended) {
      Movie_Close();
      g_InGameSessionNotificationTimeoutTicks = 0x280;
      inGameRoot->observedSessionNotificationValue9B50 = (uint32_t)panelTextureSource;
      inGameRoot->notificationPlaybackCompletionCode9B54 = 0x25;
    }
  }
  return;
}


/* Address: 0x005641D0.
   Ownership: gameplay/session/runtime.
   Purpose: Initializes a new in-game session from a level asset, builds player and UI runtime state, opens the
   level movie, prepares world, terrain, shading, technology, army, model, shot, effect, and audio resources, and
   reports failure through CF.
   Local calls: InGameRuntime_InitializeOptionalSubsystemAlwaysSuccessCf,
   InGameRuntime_UpdateSimulationAndNetworkTick.
   Cross-module calls: PersistentSettings_ReadDword [core/settings/persistent], UiRuntime_SetSynchronizationHooks
   [ui/core/runtime], TextResource_Resolve [assets/text/resources], SelectionInfoPanel_InitResources
   [gameplay/selection/runtime], InGameUiRuntime_InitializeControlTreeResourcesCf [ui/ingame/runtime],
   UiRootStack_Push [ui/controls/layout].
*/
NewSessionInitResult __thandor_eax_cf_preserve_ecx_edx
InGameRuntime_InitializeNewSession(LevelAssetRuntimeImagePrefix370 *levelAsset,uint16_t *levelMoviePath)

{
  WorldRuntimeFlags *worldRuntimeFlags;
  WorldRuntimeContext *world;
  uint16_t titleChar;
  UiTextResourceId titleTextIndex;
  PlayerRuntimeId localPlayerId;
  uint16_t *titleSource;
  InGameRuntimeRootImageC3E4 *inGameRoot;
  uint32_t settingOrFactionToken;
  uint32_t gridHalfSize;
  uint32_t subresourceCount;
  int countOrPlayerId;
  FrontendPlayerRuntimeBlockCount remainingPlayers;
  InGameNotificationMovieId notificationMovieId;
  InGameNotificationMovieId unusedNotificationMovieId;
  SelectionPlayerRuntimeBlock *selectionBlock;
  FrontendPlayerNameUtf16_28 *playerNameSource;
  uint32_t *templateCursor;
  FrontendPlayerRemovalPacket10007 *packetCursor;
  SelectionPlayerRuntimeBlock *selectionBlockCursor;
  FrontendPlayerRuntimeRecord *frontendPlayer;
  uint8_t *playerNameDestination;
  uint16_t *sessionNameClearCursor;
  uint16_t *sessionNameCursor;
  InGameRuntimeRootImageC3E4 *rootCursorOrError;
  InGameNotificationQueueRecord20 *queueRecord;
  bool subsystemFailed;
  TextResolveResult resolvedTitle;
  ArenaAllocResult allocation;
  StatusResult statusResult;
  EndingMoviePathResult endingMoviePath;
  MovieOpenResult movieOpen;
  MovieFrameResult firstFrame;
  LevelDefaultLoadResult levelLoad;
  GridScratchAllocResult gridScratch;
  NewSessionInitResult failureResult;
  
  g_UiCommandRuntimeFlags = 0x11;
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
  g_TextureDownsampleShift = PersistentSettings_ReadDword(0,0x30);
  g_EndMovieSelectionIndex = 0xffffffff;
  g_EndMovieVariantIndex = 0;
  g_EndMoviePath = (uint16_t *)0x0;
  packetCursor = &g_FrontendClientPlayerRemovalPacket10007;
  for (countOrPlayerId = 0xa0; countOrPlayerId != 0; countOrPlayerId = countOrPlayerId + -1) {
    (packetCursor->header).packedTypeAndUnitCount = 0;
    packetCursor = (FrontendPlayerRemovalPacket10007 *)&(packetCursor->header).sequenceToken;
  }
  selectionBlockCursor = g_SelectionPlayerBlocks;
  for (countOrPlayerId = 0x10230; remainingPlayers = g_FrontendPlayerRuntimeBlockCount, selectionBlock = g_SelectionPlayerBlocks,
      frontendPlayer = g_FrontendPlayerRuntimeBlocks, countOrPlayerId != 0; countOrPlayerId = countOrPlayerId + -1) {
    (selectionBlockCursor->selection).entries[0] = (GameEntityRuntime *)0x0;
    selectionBlockCursor = (SelectionPlayerRuntimeBlock *)((selectionBlockCursor->selection).entries + 1);
  }
  do {
    countOrPlayerId = frontendPlayer->playerRuntimeId;
    (frontendPlayer->factionAssignment).readyOrWaitState = 0;
    frontendPlayer->commandSyncPending = FRONTEND_COMMAND_SYNC_PENDING;
    frontendPlayer->heartbeatExpiryTicks = 0x400;
    settingOrFactionToken = (frontendPlayer->factionAssignment).factionAssignmentIndex;
    g_SelectionPlayerRuntimeBlockPointers[countOrPlayerId] = selectionBlock;
    selectionBlock->primaryEntityOrFactionToken8080 = settingOrFactionToken;
    selectionBlock->simulationStepTicks = 1;
    playerNameSource = &frontendPlayer->playerName;
    playerNameDestination = selectionBlock->reserved80B0_8117 + 0x40;
    for (countOrPlayerId = 0x14; countOrPlayerId != 0; countOrPlayerId = countOrPlayerId + -1) {
      *(uint32_t *)playerNameDestination = *(uint32_t *)playerNameSource->textUtf16;
      playerNameSource = (FrontendPlayerNameUtf16_28 *)(playerNameSource->textUtf16 + 2);
      playerNameDestination = playerNameDestination + 4;
    }
    remainingPlayers = remainingPlayers - 1;
    selectionBlock = selectionBlock + 1;
    frontendPlayer = frontendPlayer + 1;
  } while (remainingPlayers != 0);
  g_SessionTransferTimeoutTicks = 0x400;
  g_InGameNetworkTickCountdown = 4;
  g_InGameStateTickSpinLock = 0;
  (*g_TimerRegisterPeriodic)(0x50,InGameRuntime_PeriodicCountdownAndClockTick);
  UiRuntime_SetSynchronizationHooks
            (InGameRuntime_UpdateSimulationAndNetworkTick,&g_InGameStateTickSpinLock);
  titleTextIndex = (levelAsset->header).titleTextResourceIndex;
  sessionNameClearCursor = &g_InGameSessionNameScratchUtf16;
  for (countOrPlayerId = 0x20; countOrPlayerId != 0; countOrPlayerId = countOrPlayerId + -1) {
    *sessionNameClearCursor = 0;
    sessionNameClearCursor = sessionNameClearCursor + 1;
  }
  resolvedTitle = TextResource_Resolve(titleTextIndex + 0x2230);
  titleSource = resolvedTitle.text;
  sessionNameCursor = &g_InGameSessionNameScratchUtf16;
  countOrPlayerId = 0x1f;
  do {
    titleSource = titleSource + 1;
    titleChar = *titleSource;
    if (titleChar == 0) break;
    if ((((((titleChar != 0x2a) && (titleChar != 0x3c)) && (titleChar != 0x3e)) &&
         ((titleChar != 0x22 && (titleChar != 0x2f)))) &&
        ((titleChar != 0x5c && ((titleChar != 0x2e && (titleChar != 0x3f)))))) &&
       ((titleChar != 0x3a && (titleChar != 0x7c)))) {
      *sessionNameCursor = titleChar;
      sessionNameCursor = sessionNameCursor + 1;
    }
    countOrPlayerId = countOrPlayerId + -1;
  } while (countOrPlayerId != 0);
  allocation = (*g_MemoryApi.alloc)(0x400000);
  rootCursorOrError = (InGameRuntimeRootImageC3E4 *)allocation.payloadOrError;
  if (!allocation.failed) {
    g_RuntimeObjectRebaseBaseMinusOne = rootCursorOrError[-1].opaqueA06C_C3E3 + 0x2377;
    g_InGameWorldObjectRecords = (WorldObjectRecord *)rootCursorOrError;
    for (countOrPlayerId = 0x100000; countOrPlayerId != 0; countOrPlayerId = countOrPlayerId + -1) {
      (rootCursorOrError->rootUi0000).base.nextSibling = (UiNodeBase *)0x0;
      rootCursorOrError = (InGameRuntimeRootImageC3E4 *)&(rootCursorOrError->rootUi0000).base.firstChild;
    }
    statusResult = SelectionInfoPanel_InitResources
                       ((SelectionInfoEntitySlots *)
                        g_SelectionPlayerRuntimeBlockPointers[g_LocalPlayerRuntimeId]);
    rootCursorOrError = (InGameRuntimeRootImageC3E4 *)statusResult.valueOrError;
    if (!statusResult.failed) {
      allocation = (*g_MemoryApi.alloc)(0xc3e4);
      inGameRoot = (InGameRuntimeRootImageC3E4 *)allocation.payloadOrError;
      rootCursorOrError = inGameRoot;
      if (!allocation.failed) {
        templateCursor = (uint32_t *)&g_InGameRuntimeDefaultImageTemplate;
        g_InGameRuntimeRoot = inGameRoot;
        for (countOrPlayerId = 0x30f9; countOrPlayerId != 0; countOrPlayerId = countOrPlayerId + -1) {
          (rootCursorOrError->rootUi0000).base.nextSibling = (UiNodeBase *)*templateCursor;
          templateCursor = templateCursor + 1;
          rootCursorOrError = (InGameRuntimeRootImageC3E4 *)&(rootCursorOrError->rootUi0000).base.firstChild;
        }
        world = &inGameRoot->worldRuntime0A30;
        statusResult = InGameUiRuntime_InitializeControlTreeResourcesCf((UiRootNode *)inGameRoot);
        rootCursorOrError = (InGameRuntimeRootImageC3E4 *)statusResult.valueOrError;
        if (!statusResult.failed) {
          inGameRoot->worldOverlayCallback0B8C =
               InGameWorldOverlay_RebuildOrReleaseTransientMarkersCf;
          (inGameRoot->worldRuntime0A30).selection.dispatchCommandCallback =
               InGameUiRuntime_DispatchCommandByCodeAndModifierFlagsCf;
          (inGameRoot->worldRuntime0A30).selection.resolveContextActionPrimaryCallback =
               InGameWorldInput_ResolveContextActionAndCursorCf;
          (inGameRoot->worldRuntime0A30).selection.resolveContextActionSecondaryCallback =
               InGameWorldInput_ResolveContextActionAndCursorCf;
          (inGameRoot->worldRuntime0A30).selection.beginPointerCaptureCallback =
               InGameWorldInput_BeginPointerCaptureCf;
          (inGameRoot->worldRuntime0A30).selection.updateDragSelectionCallback =
               InGameWorldInput_UpdateDragSelectionAndCameraCf;
          (inGameRoot->worldRuntime0A30).selection.commitPointerActionCallback =
               InGameWorldInput_CommitPointerActionCf;
          (inGameRoot->worldRuntime0A30).fieldRegion.clearTransientStateCallback =
               InGameUiRuntime_ClearTransientState1BCallback;
          (inGameRoot->worldRuntime0A30).selection.dispatchWorldContextActionCallback =
               InGameUiRuntime_DispatchWorldContextActionCallback;
          (inGameRoot->worldRuntime0A30).minimumCameraDistanceQ12 = 0x8000;
          (inGameRoot->worldRuntime0A30).maximumCameraDistanceQ12 = 0x13000;
          (inGameRoot->worldRuntime0A30).motion.minimumPitchAngle = 0xffffc400;
          (inGameRoot->worldRuntime0A30).motion.maximumPitchAngle = 0xffffe800;
          (inGameRoot->worldRuntime0A30).tickSpinLock = &g_InGameStateTickSpinLock;
          (inGameRoot->worldRuntime0A30).simulationAndNetworkTickCallback =
               InGameRuntime_UpdateSimulationAndNetworkTick;
          selectionBlockCursor = g_SelectionPlayerRuntimeBlockPointers[g_LocalPlayerRuntimeId];
          inGameRoot->localPlayerPairCount0BA4 = 0;
          inGameRoot->localPlayerPairRecords0BA0 = selectionBlockCursor->pairRecords80_807F;
          UiRootStack_Push(&g_UiRootCallbacks_0054FBC0,(UiRootNode *)inGameRoot);
          endingMoviePath = LevelAsset_PrepareEndingMoviePathCf(levelMoviePath,&levelAsset->header);
          rootCursorOrError = (InGameRuntimeRootImageC3E4 *)endingMoviePath.moviePath;
          if (!endingMoviePath.failed) {
            movieOpen = Movie_Open(0x80000000,(uint16_t *)rootCursorOrError);
            rootCursorOrError = (InGameRuntimeRootImageC3E4 *)movieOpen.frameCountOrError;
            if (!movieOpen.failed) {
              firstFrame = Movie_AdvanceFrame();
              rootCursorOrError = (InGameRuntimeRootImageC3E4 *)firstFrame.movieOrError;
              if (!firstFrame.ended) {
                inGameRoot->levelMovieRuntime08D4 = (MovieRuntime *)rootCursorOrError;
                g_MoviePlaybackBaseFrameGroup = 0;
                g_MoviePlaybackScheduleCounter = 0;
                g_MoviePlaybackScheduleSpan = 0;
                g_MoviePlaybackCurrentFrame = 0;
                MoviePlayback_AdvanceToFrameAndPresent(0);
                MoviePlayback_AdvanceToFrameAndPresent(1);
                RecentTextHistory_SortAndBuildPointerList(8,&inGameRoot->recentTextHistory09B8);
                WorldRuntime_AttachObjectArray(0x4000,g_InGameWorldObjectRecords,world);
                localPlayerId = g_LocalPlayerRuntimeId;
                settingOrFactionToken = g_SelectionPlayerRuntimeBlockPointers[g_LocalPlayerRuntimeId]->
                        primaryEntityOrFactionToken8080;
                levelAsset->playerSlots[6].aiClassOrMode = settingOrFactionToken;
                (inGameRoot->worldRuntime0A30).activeFactionRuntimeIndex = settingOrFactionToken;
                (inGameRoot->worldRuntime0A30).selection.activePlayerRuntimeId = localPlayerId;
                WorldRuntime_AttachAndClearDwordArray
                          (0x100,(uint32_t *)&g_InGameWorldRuntimeDwordArray256,world);
                statusResult = GameData_ResetDefaults();
                rootCursorOrError = (InGameRuntimeRootImageC3E4 *)statusResult.valueOrError;
                if (!statusResult.failed) {
                  levelLoad = InGameLevelRuntime_LoadResourcesAfterDefaultResetCf(levelAsset,world);
                  rootCursorOrError = (InGameRuntimeRootImageC3E4 *)levelLoad.errorOrValue;
                  if (!levelLoad.failed) {
                    queueRecord = inGameRoot->notificationQueue9E60;
                    for (countOrPlayerId = 0x20; countOrPlayerId != 0; countOrPlayerId = countOrPlayerId + -1) {
                      queueRecord->notificationMovieId00 = 0;
                      queueRecord = (InGameNotificationQueueRecord20 *)&queueRecord->priority04;
                    }
                    statusResult = TerrainCompositeTexture_Create();
                    rootCursorOrError = (InGameRuntimeRootImageC3E4 *)statusResult.valueOrError;
                    if (!statusResult.failed) {
                      (*g_SpinLockAcquire)(&g_InGameStateTickSpinLock);
                      g_InGameSimulationStepTicks = 1;
                      settingOrFactionToken = PersistentSettings_ReadDword(0x40,0x14);
                      gridHalfSize = PersistentSettings_ReadDword(0x20,0x10);
                      subresourceCount = PersistentSettings_ReadDword(0x10,0x18);
                      GraphicsShadingRuntime_InitializeGeneratedTextureCf
                                (subresourceCount,gridHalfSize,settingOrFactionToken);
                      settingOrFactionToken = PersistentSettings_ReadDword(1,0x1c);
                      if (settingOrFactionToken == 0) {
                        worldRuntimeFlags = &(inGameRoot->worldRuntime0A30).runtimeFlags;
                        *worldRuntimeFlags = *worldRuntimeFlags & 0xfffdffff;
                      }
                      else {
                        worldRuntimeFlags = &(inGameRoot->worldRuntime0A30).runtimeFlags;
                        *worldRuntimeFlags = *worldRuntimeFlags | 0x20000;
                      }
                      settingOrFactionToken = PersistentSettings_ReadDword(0,0x5c);
                      if ((settingOrFactionToken & 1) == 0) {
                        worldRuntimeFlags = &(inGameRoot->worldRuntime0A30).runtimeFlags;
                        *worldRuntimeFlags = *worldRuntimeFlags & 0xbfffffff;
                      }
                      else {
                        worldRuntimeFlags = &(inGameRoot->worldRuntime0A30).runtimeFlags;
                        *worldRuntimeFlags = *worldRuntimeFlags | 0x40000000;
                      }
                      if ((settingOrFactionToken & 2) == 0) {
                        worldRuntimeFlags = &(inGameRoot->worldRuntime0A30).runtimeFlags;
                        *worldRuntimeFlags = *worldRuntimeFlags & 0x7fffffff;
                      }
                      else {
                        worldRuntimeFlags = &(inGameRoot->worldRuntime0A30).runtimeFlags;
                        *worldRuntimeFlags = *worldRuntimeFlags | 0x80000000;
                      }
                      if ((settingOrFactionToken & 4) == 0) {
                        worldRuntimeFlags = &(inGameRoot->worldRuntime0A30).runtimeFlags;
                        *worldRuntimeFlags = *worldRuntimeFlags & 0xfbffffff;
                      }
                      else {
                        worldRuntimeFlags = &(inGameRoot->worldRuntime0A30).runtimeFlags;
                        *worldRuntimeFlags = *worldRuntimeFlags | 0x4000000;
                      }
                      rootCursorOrError = (InGameRuntimeRootImageC3E4 *)
                                     PersistentSettings_ReadDword(0,0x40);
                      if (((uint32_t)rootCursorOrError & 4) != 0) {
                        UiPageStack_SetActiveIndex(1,&inGameRoot->optionalUiPageStack40AC);
                        rootCursorOrError = (InGameRuntimeRootImageC3E4 *)
                                       &inGameRoot->optionalUiPageStack4530;
                        UiPageStack_SetActiveIndex(0,(UiPageStackControl *)rootCursorOrError);
                        UiPageStack_SetActiveIndex(0,&inGameRoot->optionalUiPageStack4644);
                        inGameRoot->optionalUiLayoutState0A04 = 0;
                        UiContainer_LayoutChildren((UiNodeBase *)inGameRoot);
                      }
                      subsystemFailed = (bool)InGameRuntime_InitializeOptionalSubsystemAlwaysSuccessCf
                                               ((uint32_t)(inGameRoot->worldRuntime0A30).fieldGrid);
                      if (!subsystemFailed) {
                        if (g_FrontendLoadedCampaignAsset == 0) {
                          OldUnitRuntime_ResetPendingTables();
                        }
                        else {
                          OldUnitRuntime_MergeMasksAndReplayRecords();
                        }
                        gridScratch = GridScratch_AllocateForFieldGridCf
                                           ((inGameRoot->worldRuntime0A30).fieldGrid);
                        rootCursorOrError = (InGameRuntimeRootImageC3E4 *)gridScratch.valueOrError;
                        if (!gridScratch.failed) {
                          GridScratch_RebuildTerrainAndRuntimeClassificationMasks(world);
                          GridInfluence_ClearDistanceBandsAndRefreshEntities
                                    ((inGameRoot->worldRuntime0A30).ownerListHead);
                          TechnologyRuntime_RebuildDerivedLimitsAndCategoryMasks();
                          if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_NETWORKED_MASK) ==
                              SESSION_NETWORK_ROLE_LOCAL) {
                            inGameRoot->localNetworkUiStateFlags24E0 =
                                 inGameRoot->localNetworkUiStateFlags24E0 | 8;
                          }
                          else {
                            inGameRoot->localNetworkUiStateFlags24E0 =
                                 inGameRoot->localNetworkUiStateFlags24E0 & 0xfffffff7;
                          }
                          UiCatalogGroup48_RebuildGrid((UiNodeBase *)inGameRoot);
                          UiCatalogGroup42_RebuildGrid((UiNodeBase *)inGameRoot);
                          UiCommandSpriteVariantA_RebuildGrid((UiNodeBase *)inGameRoot);
                          InGameOtherPlayerCommand_RebuildTargetEntries((UiNodeBase *)inGameRoot);
                          WorldLightingRuntime_UpdateInterpolatedTerrainLighting();
                          if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_NETWORKED_MASK) ==
                              SESSION_NETWORK_ROLE_LOCAL) {
                            FrontendPlayerRuntime_IncrementReadyCountAndResolveConsensus
                                      (g_LocalPlayerRuntimeId,0,0,0);
                          }
                          else {
                            InGameCommandQueue_AppendLocalPlayerCommand(0x550,0,0,0);
                          }
                          (*g_SpinLockRelease)(&g_InGameStateTickSpinLock);
                          UiFrame_FlushInputAndResetPendingTicks();
                          (*g_GraphicsCursorSetFrame)(6);
                          g_CursorVisibilityToken = g_CursorVisibilityToken + 1;
                          InGamePanel_RebuildPlayerStatusRows(inGameRoot);
                          do {
                            UiNode_InvalidateRoot(&inGameRoot->playerStatusNode08E4);
                            InGamePanel_RebuildPlayerStatusRows(inGameRoot);
                            UiFrame_Update(0);
                            UiFrame_Draw();
                            (*g_GraphicsFramebufferPresent)(g_FramebufferAccess);
                            InGameRuntime_UpdateSimulationAndNetworkTick();
                          } while ((g_UiCommandRuntimeFlags & 0x10) != 0);
                          inGameRoot->levelMovieRuntime08D4 = (MovieRuntime *)0x0;
                          inGameRoot->playerStatusLayoutMetric093C = 0;
                          UiPageStack_SetActiveIndex(2,&inGameRoot->primaryPageStack017C);
                          Movie_Close();
                          (*g_GraphicsCursorSetFrame)(0);
                          /* Lost load: the original reads the first of five level intro
                             notification movies from conditionStorage+0x324. */
                          notificationMovieId =
                               *(InGameNotificationMovieId *)
                                ((uint8_t *)g_InGameLevelRuntimeGlobalBlock.conditionStorage + 0x324);
                          (*g_TimerRegisterPeriodic)
                                    (10,InGameRuntime_ProcessQueuedSessionNotificationTimer);
                          if (notificationMovieId != 0) {
                            InGameNotificationQueue_InsertPriorityRecord
                                      (NOTIFICATION_PAYLOAD_NONE,0,0,0,0,0,1,notificationMovieId);
                            InGameNotificationQueue_InsertPriorityRecord
                                      (NOTIFICATION_PAYLOAD_NONE,0,0,0,0,0,1,notificationMovieId + 1);
                            InGameNotificationQueue_InsertPriorityRecord
                                      (NOTIFICATION_PAYLOAD_NONE,0,0,0,0,0,1,notificationMovieId + 2);
                            InGameNotificationQueue_InsertPriorityRecord
                                      (NOTIFICATION_PAYLOAD_NONE,0,0,0,0,0,1,notificationMovieId + 3);
                            InGameNotificationQueue_InsertPriorityRecord
                                      (NOTIFICATION_PAYLOAD_NONE,0,0,0,0,0,1,notificationMovieId + 4);
                          }
                          return THANDOR_BITCAST(uint64_t, NewSessionInitResult, ((THANDOR_BITCAST(ArenaAllocResult, uint64_t, allocation) & 0xFFFFFFFFFFull) & 0xffffffff));
                        }
                      }
                    }
                  }
                }
              }
            }
          }
        }
      }
    }
  }
  Movie_Close();
  failureResult.failed = true;
  failureResult.runtimeRootOrError = (uint32_t)rootCursorOrError;
  return failureResult;
}


/* Address: 0x00564920.
   Ownership: gameplay/session/runtime.
   Purpose: Initializes an in-game session from persisted save state, reconstructs player and world runtime data,
   restores assets and subsystem state, installs callbacks, and reports failure through CF.
   Local calls: InGameRuntime_InitializeOptionalSubsystemAlwaysSuccessCf,
   InGameRuntime_UpdateSimulationAndNetworkTick.
   Cross-module calls: PersistentSettings_ReadDword [core/settings/persistent], Package_Mount
   [assets/package/runtime], Package_LoadEntry [assets/package/runtime], UiRuntime_SetSynchronizationHooks
   [ui/core/runtime], SelectionInfoPanel_InitResources [gameplay/selection/runtime],
   InGameUiRuntime_InitializeControlTreeResourcesCf [ui/ingame/runtime].
*/
LoadedSessionInitResult __thandor_eax_cf_preserve_ecx_edx
InGameRuntime_InitializeLoadedSession(uint16_t *savePackagePath)

{
  WorldRuntimeFlags *worldRuntimeFlags;
  WorldRuntimeContext *world;
  SelectionPlayerRuntimeBlock *entitySlots;
  InGameRuntimeRootImageC3E4 *mountResult;
  FrontendLoadedLevelRuntimeImage370 *levelImage;
  InGameRuntimeRootImageC3E4 *inGameRoot;
  uint32_t settingOrFactionToken;
  uint32_t gridHalfSize;
  uint32_t subresourceCount;
  InGameRuntimeRootImageC3E4 *rootCursorOrError;
  int remainingCount;
  uint8_t *saveNameSource;
  uint32_t *templateCursor;
  uint16_t *sessionNameClearCursor;
  uint8_t *scanCursor;
  uint8_t *scanEnd;
  uint32_t copyCount;
  short *sessionNameCursor;
  SelectionPlayerRuntimeBlock *selectionBlockCursor;
  InGameNotificationQueueRecord20 *queueRecord;
  bool terminatorOrFailure;
  StatusResult statusResult;
  PackageLoadResult packageEntry;
  ArenaAllocResult allocation;
  EndingMoviePathResult endingMoviePath;
  MovieOpenResult movieOpen;
  MovieFrameResult firstFrame;
  LevelLoadResult levelLoad;
  GridScratchAllocResult gridScratch;
  LoadedSessionInitResult failureResult;
  InGameRuntimeRootImageC3E4 *mountedPackage;
  FrontendLoadedLevelRuntimeImage370 *loadedLevelAsset;
  
  g_TextureDownsampleShift = PersistentSettings_ReadDword(0,0x30);
  mountedPackage = (InGameRuntimeRootImageC3E4 *)0x0;
  loadedLevelAsset = (FrontendLoadedLevelRuntimeImage370 *)0x0;
  statusResult = Package_Mount(savePackagePath);
  saveNameSource = g_PackageScratchBuffer;
  mountResult = (InGameRuntimeRootImageC3E4 *)statusResult.valueOrError;
  rootCursorOrError = mountResult;
  if (!statusResult.failed) {
    sessionNameClearCursor = &g_InGameSessionNameScratchUtf16;
    for (remainingCount = 0x20; remainingCount != 0; remainingCount = remainingCount + -1) {
      *sessionNameClearCursor = 0;
      sessionNameClearCursor = sessionNameClearCursor + 1;
    }
    (*g_FileSystemSeekCf)(FILESYSTEM_SEEK_BEGIN,0,mountResult);
    (*g_FileSystemReadExactCf)(0x200,saveNameSource,mountResult);
    saveNameSource = saveNameSource + 0x100;
    terminatorOrFailure = true;
    remainingCount = 0x24;
    scanCursor = saveNameSource;
    do {
      scanEnd = scanCursor;
      if (remainingCount == 0) break;
      remainingCount = remainingCount + -1;
      scanEnd = scanCursor + 2;
      terminatorOrFailure = *(short *)scanCursor == 0;
      scanCursor = scanEnd;
    } while (!terminatorOrFailure);
    if (terminatorOrFailure) {
      scanEnd[-0xffffffff00000006] = 0;
      scanEnd[-0xffffffff00000005] = 0;
      scanEnd[-0xffffffff00000004] = 0;
      scanEnd[-0xffffffff00000003] = 0;
      scanEnd[-0xffffffff0000000a] = 0;
      scanEnd[-0xffffffff00000009] = 0;
      scanEnd[-0xffffffff00000008] = 0;
      scanEnd[-0xffffffff00000007] = 0;
      copyCount = (uint32_t)((int)scanEnd - (int)saveNameSource) >> 1;
      sessionNameCursor = &g_InGameSessionNameScratchUtf16;
      if (0x1f < copyCount) {
        copyCount = 0x1f;
      }
      for (; copyCount != 0; copyCount = copyCount - 1) {
        *sessionNameCursor = *(short *)saveNameSource;
        saveNameSource = saveNameSource + 2;
        sessionNameCursor = sessionNameCursor + 1;
      }
    }
    packageEntry = Package_LoadEntry((uint16_t *)u_campagne_hex_0050e068);
    if (!packageEntry.failed) {
      g_FrontendLoadedCampaignAsset = packageEntry.bufferOrError;
    }
    packageEntry = Package_LoadEntry((uint16_t *)u_level_hex_0050e040);
    levelImage = packageEntry.bufferOrError;
    rootCursorOrError = (InGameRuntimeRootImageC3E4 *)levelImage;
    mountedPackage = mountResult;
    if (!packageEntry.failed) {
      settingOrFactionToken = levelImage->playerSlots[6].aiClassOrMode;
      selectionBlockCursor = g_SelectionPlayerBlocks;
      for (remainingCount = 0x10230; entitySlots = g_SelectionPlayerBlocks, remainingCount != 0; remainingCount = remainingCount + -1) {
        (selectionBlockCursor->selection).entries[0] = (GameEntityRuntime *)0x0;
        selectionBlockCursor = (SelectionPlayerRuntimeBlock *)((selectionBlockCursor->selection).entries + 1);
      }
      g_EndMovieSelectionIndex = 0xffffffff;
      g_EndMovieVariantIndex = 0;
      g_EndMoviePath = (uint16_t *)0x0;
      g_LocalPlayerRuntimeId = 0;
      g_SelectionPlayerRuntimeBlockPointers[0] = g_SelectionPlayerBlocks;
      g_SelectionPlayerBlocks->primaryEntityOrFactionToken8080 = settingOrFactionToken;
      entitySlots->simulationStepTicks = 1;
      g_UiCommandRuntimeFlags = 0x11;
      g_SessionNetworkTickCounter = 1;
      g_HostCommandBatchSyncSentThisInterval = 0;
      g_GameFactionRuntimeImage.tail.simulationTick = 1;
      g_GameFactionRuntimeImage.tail.presentationTick = 0;
      g_InGameSessionNotificationTimeoutTicks = 0;
      g_InGameReadyStateToggleFlags = 0;
      g_InGameNetworkTickCountdown = 4;
      g_InGameStateTickSpinLock = 0;
      g_EndGameResultsCurrentMusicTrackId = 0;
      g_InGameSelectedTechnologyId = TEC_000_BASIC_TECHNOLOGY;
      (*g_TimerRegisterPeriodic)(0x50,InGameRuntime_PeriodicCountdownAndClockTick);
      UiRuntime_SetSynchronizationHooks
                (InGameRuntime_UpdateSimulationAndNetworkTick,&g_InGameStateTickSpinLock);
      allocation = (*g_MemoryApi.alloc)(0x400000);
      rootCursorOrError = (InGameRuntimeRootImageC3E4 *)allocation.payloadOrError;
      loadedLevelAsset = levelImage;
      if (!allocation.failed) {
        g_RuntimeObjectRebaseBaseMinusOne = rootCursorOrError[-1].opaqueA06C_C3E3 + 0x2377;
        g_InGameWorldObjectRecords = (WorldObjectRecord *)rootCursorOrError;
        for (remainingCount = 0x100000; remainingCount != 0; remainingCount = remainingCount + -1) {
          (rootCursorOrError->rootUi0000).base.nextSibling = (UiNodeBase *)0x0;
          rootCursorOrError = (InGameRuntimeRootImageC3E4 *)&(rootCursorOrError->rootUi0000).base.firstChild;
        }
        statusResult = SelectionInfoPanel_InitResources((SelectionInfoEntitySlots *)entitySlots);
        rootCursorOrError = (InGameRuntimeRootImageC3E4 *)statusResult.valueOrError;
        if (!statusResult.failed) {
          allocation = (*g_MemoryApi.alloc)(0xc3e4);
          inGameRoot = (InGameRuntimeRootImageC3E4 *)allocation.payloadOrError;
          rootCursorOrError = inGameRoot;
          if (!allocation.failed) {
            templateCursor = (uint32_t *)&g_InGameRuntimeDefaultImageTemplate;
            g_InGameRuntimeRoot = inGameRoot;
            for (remainingCount = 0x30f9; remainingCount != 0; remainingCount = remainingCount + -1) {
              (rootCursorOrError->rootUi0000).base.nextSibling = (UiNodeBase *)*templateCursor;
              templateCursor = templateCursor + 1;
              rootCursorOrError = (InGameRuntimeRootImageC3E4 *)&(rootCursorOrError->rootUi0000).base.firstChild;
            }
            world = &inGameRoot->worldRuntime0A30;
            statusResult = InGameUiRuntime_InitializeControlTreeResourcesCf((UiRootNode *)inGameRoot);
            rootCursorOrError = (InGameRuntimeRootImageC3E4 *)statusResult.valueOrError;
            if (!statusResult.failed) {
              inGameRoot->worldOverlayCallback0B8C =
                   InGameWorldOverlay_RebuildOrReleaseTransientMarkersCf;
              (inGameRoot->worldRuntime0A30).selection.dispatchCommandCallback =
                   InGameUiRuntime_DispatchCommandByCodeAndModifierFlagsCf;
              (inGameRoot->worldRuntime0A30).selection.resolveContextActionPrimaryCallback =
                   InGameWorldInput_ResolveContextActionAndCursorCf;
              (inGameRoot->worldRuntime0A30).selection.resolveContextActionSecondaryCallback =
                   InGameWorldInput_ResolveContextActionAndCursorCf;
              (inGameRoot->worldRuntime0A30).selection.beginPointerCaptureCallback =
                   InGameWorldInput_BeginPointerCaptureCf;
              (inGameRoot->worldRuntime0A30).selection.updateDragSelectionCallback =
                   InGameWorldInput_UpdateDragSelectionAndCameraCf;
              (inGameRoot->worldRuntime0A30).selection.commitPointerActionCallback =
                   InGameWorldInput_CommitPointerActionCf;
              (inGameRoot->worldRuntime0A30).fieldRegion.clearTransientStateCallback =
                   InGameUiRuntime_ClearTransientState1BCallback;
              (inGameRoot->worldRuntime0A30).selection.dispatchWorldContextActionCallback =
                   InGameUiRuntime_DispatchWorldContextActionCallback;
              (inGameRoot->worldRuntime0A30).minimumCameraDistanceQ12 = 0x8000;
              (inGameRoot->worldRuntime0A30).maximumCameraDistanceQ12 = 0x13000;
              (inGameRoot->worldRuntime0A30).motion.minimumPitchAngle = 0xffffc400;
              (inGameRoot->worldRuntime0A30).motion.maximumPitchAngle = 0xffffe800;
              (inGameRoot->worldRuntime0A30).tickSpinLock = &g_InGameStateTickSpinLock;
              (inGameRoot->worldRuntime0A30).simulationAndNetworkTickCallback =
                   InGameRuntime_UpdateSimulationAndNetworkTick;
              selectionBlockCursor = g_SelectionPlayerRuntimeBlockPointers[g_LocalPlayerRuntimeId];
              inGameRoot->localPlayerPairCount0BA4 = 0;
              inGameRoot->localPlayerPairRecords0BA0 = selectionBlockCursor->pairRecords80_807F;
              UiRootStack_Push(&g_UiRootCallbacks_0054FBC0,(UiRootNode *)inGameRoot);
              WidePath_CombineDirectoryAndLeaf
                        (&g_FrontendScenarioPathScratchUtf16,
                         (uint16_t *)(levelImage->header).opaque100_16F,(uint16_t *)u_level_0050daac);
              WidePath_SetExtensionCode(0x76656c,&g_FrontendScenarioPathScratchUtf16);
              endingMoviePath = LevelAsset_PrepareEndingMoviePathCf
                                 (savePackagePath,(LevelAssetHeader *)levelImage);
              rootCursorOrError = (InGameRuntimeRootImageC3E4 *)endingMoviePath.moviePath;
              if (!endingMoviePath.failed) {
                movieOpen = Movie_Open(0x80000000,(uint16_t *)rootCursorOrError);
                rootCursorOrError = (InGameRuntimeRootImageC3E4 *)movieOpen.frameCountOrError;
                if (!movieOpen.failed) {
                  firstFrame = Movie_AdvanceFrame();
                  rootCursorOrError = (InGameRuntimeRootImageC3E4 *)firstFrame.movieOrError;
                  if (!firstFrame.ended) {
                    inGameRoot->levelMovieRuntime08D4 = (MovieRuntime *)rootCursorOrError;
                    g_MoviePlaybackBaseFrameGroup = 0;
                    g_MoviePlaybackScheduleCounter = 0;
                    g_MoviePlaybackScheduleSpan = 0;
                    g_MoviePlaybackCurrentFrame = 0;
                    MoviePlayback_AdvanceToFrameAndPresent(0);
                    MoviePlayback_AdvanceToFrameAndPresent(1);
                    RecentTextHistory_SortAndBuildPointerList(8,&inGameRoot->recentTextHistory09B8);
                    selectionBlockCursor = g_SelectionPlayerBlocks;
                    WorldRuntime_AttachObjectArray(0x4000,g_InGameWorldObjectRecords,world);
                    rootCursorOrError = (InGameRuntimeRootImageC3E4 *)selectionBlockCursor->primaryEntityOrFactionToken8080;
                    (inGameRoot->worldRuntime0A30).activeFactionRuntimeIndex =
                         (FactionRuntimeIndex)rootCursorOrError;
                    (inGameRoot->worldRuntime0A30).selection.activePlayerRuntimeId = 0;
                    WorldRuntime_AttachAndClearDwordArray
                              (0x100,(uint32_t *)&g_InGameWorldRuntimeDwordArray256,world);
                    terminatorOrFailure = GameData_LoadExternalTables();
                    if (!terminatorOrFailure) {
                      packageEntry = Package_LoadEntry((uint16_t *)u_field_hex_0050e002);
                      rootCursorOrError = packageEntry.bufferOrError;
                      if (!packageEntry.failed) {
                        (levelImage->header).pathState.levelPathOffsetOrLoadedFieldGrid =
                             (uint32_t)rootCursorOrError;
                        levelLoad = InGameLevelRuntime_LoadResourcesAfterExternalTablesCf
                                           (levelImage,world);
                        rootCursorOrError = (InGameRuntimeRootImageC3E4 *)levelLoad.errorOrValue;
                        if (!levelLoad.failed) {
                          queueRecord = inGameRoot->notificationQueue9E60;
                          for (remainingCount = 0x20; remainingCount != 0; remainingCount = remainingCount + -1) {
                            queueRecord->notificationMovieId00 = 0;
                            queueRecord = (InGameNotificationQueueRecord20 *)&queueRecord->priority04;
                          }
                          statusResult = TerrainCompositeTexture_Create();
                          rootCursorOrError = (InGameRuntimeRootImageC3E4 *)statusResult.valueOrError;
                          if (!statusResult.failed) {
                            (*g_SpinLockAcquire)(&g_InGameStateTickSpinLock);
                            UiCatalogGroup48_RebuildGrid((UiNodeBase *)inGameRoot);
                            UiCatalogGroup42_RebuildGrid((UiNodeBase *)inGameRoot);
                            UiCommandSpriteVariantA_RebuildGrid((UiNodeBase *)inGameRoot);
                            InGameOtherPlayerCommand_RebuildTargetEntries((UiNodeBase *)inGameRoot);
                            g_InGameSimulationStepTicks = 1;
                            settingOrFactionToken = PersistentSettings_ReadDword(0x40,0x14);
                            gridHalfSize = PersistentSettings_ReadDword(0x20,0x10);
                            subresourceCount = PersistentSettings_ReadDword(0x10,0x18);
                            GraphicsShadingRuntime_InitializeGeneratedTextureCf
                                      (subresourceCount,gridHalfSize,settingOrFactionToken);
                            settingOrFactionToken = PersistentSettings_ReadDword(1,0x1c);
                            if (settingOrFactionToken == 0) {
                              worldRuntimeFlags = &(inGameRoot->worldRuntime0A30).runtimeFlags;
                              *worldRuntimeFlags = *worldRuntimeFlags & 0xfffdffff;
                            }
                            else {
                              worldRuntimeFlags = &(inGameRoot->worldRuntime0A30).runtimeFlags;
                              *worldRuntimeFlags = *worldRuntimeFlags | 0x20000;
                            }
                            rootCursorOrError = (InGameRuntimeRootImageC3E4 *)
                                     PersistentSettings_ReadDword(0,0x5c);
                            if (((uint32_t)rootCursorOrError & 1) == 0) {
                              worldRuntimeFlags = &(inGameRoot->worldRuntime0A30).runtimeFlags;
                              *worldRuntimeFlags = *worldRuntimeFlags & 0xbfffffff;
                            }
                            else {
                              worldRuntimeFlags = &(inGameRoot->worldRuntime0A30).runtimeFlags;
                              *worldRuntimeFlags = *worldRuntimeFlags | 0x40000000;
                            }
                            if (((uint32_t)rootCursorOrError & 2) == 0) {
                              worldRuntimeFlags = &(inGameRoot->worldRuntime0A30).runtimeFlags;
                              *worldRuntimeFlags = *worldRuntimeFlags & 0x7fffffff;
                            }
                            else {
                              worldRuntimeFlags = &(inGameRoot->worldRuntime0A30).runtimeFlags;
                              *worldRuntimeFlags = *worldRuntimeFlags | 0x80000000;
                            }
                            if (((uint32_t)rootCursorOrError & 4) == 0) {
                              worldRuntimeFlags = &(inGameRoot->worldRuntime0A30).runtimeFlags;
                              *worldRuntimeFlags = *worldRuntimeFlags & 0xfbffffff;
                            }
                            else {
                              worldRuntimeFlags = &(inGameRoot->worldRuntime0A30).runtimeFlags;
                              *worldRuntimeFlags = *worldRuntimeFlags | 0x4000000;
                            }
                            terminatorOrFailure = (bool)InGameRuntime_InitializeOptionalSubsystemAlwaysSuccessCf
                                                     ((uint32_t)(inGameRoot->worldRuntime0A30).
                                                             fieldGrid);
                            if (!terminatorOrFailure) {
                              gridScratch = GridScratch_AllocateForFieldGridCf
                                                 ((inGameRoot->worldRuntime0A30).fieldGrid);
                              rootCursorOrError = (InGameRuntimeRootImageC3E4 *)gridScratch.valueOrError;
                              if (!gridScratch.failed) {
                                GridScratch_RebuildTerrainAndRuntimeClassificationMasks(world);
                                GridInfluence_ClearDistanceBandsAndRefreshEntities
                                          ((inGameRoot->worldRuntime0A30).ownerListHead);
                                TechnologyRuntime_RebuildDerivedLimitsAndCategoryMasks();
                                WorldLightingRuntime_UpdateInterpolatedTerrainLighting();
                                if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_NETWORKED_MASK
                                    ) == SESSION_NETWORK_ROLE_LOCAL) {
                                  inGameRoot->localNetworkUiStateFlags24E0 =
                                       inGameRoot->localNetworkUiStateFlags24E0 | 8;
                                }
                                else {
                                  inGameRoot->localNetworkUiStateFlags24E0 =
                                       inGameRoot->localNetworkUiStateFlags24E0 & 0xfffffff7;
                                }
                                if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_NETWORKED_MASK
                                    ) == SESSION_NETWORK_ROLE_LOCAL) {
                                  FrontendPlayerRuntime_IncrementReadyCountAndResolveConsensus
                                            (g_LocalPlayerRuntimeId,0,0,0);
                                }
                                else {
                                  InGameCommandQueue_AppendLocalPlayerCommand(0x550,0,0,0);
                                }
                                (*g_SpinLockRelease)(&g_InGameStateTickSpinLock);
                                UiFrame_FlushInputAndResetPendingTicks();
                                (*g_GraphicsCursorSetFrame)(6);
                                g_CursorVisibilityToken = g_CursorVisibilityToken + 1;
                                InGamePanel_RebuildPlayerStatusRows(inGameRoot);
                                do {
                                  UiNode_InvalidateRoot(&inGameRoot->playerStatusNode08E4);
                                  InGamePanel_RebuildPlayerStatusRows(inGameRoot);
                                  UiFrame_Update(0);
                                  UiFrame_Draw();
                                  (*g_GraphicsFramebufferPresent)(g_FramebufferAccess);
                                  InGameRuntime_UpdateSimulationAndNetworkTick();
                                } while ((g_UiCommandRuntimeFlags & 0x10) != 0);
                                inGameRoot->levelMovieRuntime08D4 = (MovieRuntime *)0x0;
                                inGameRoot->playerStatusLayoutMetric093C = 0;
                                UiPageStack_SetActiveIndex(2,&inGameRoot->primaryPageStack017C);
                                Movie_Close();
                                Resource_Release(levelImage);
                                Package_Unmount((EngineFileHandle)mountResult);
                                (*g_GraphicsCursorSetFrame)(0);
                                (*g_TimerRegisterPeriodic)
                                          (10,InGameRuntime_ProcessQueuedSessionNotificationTimer);
                                return THANDOR_BITCAST(uint64_t, LoadedSessionInitResult, ((THANDOR_BITCAST(ArenaAllocResult, uint64_t, allocation) & 0xFFFFFFFFFFull) & 0xffffffff));
                              }
                            }
                          }
                        }
                      }
                    }
                  }
                }
              }
            }
          }
        }
      }
    }
  }
  Movie_Close();
  Resource_Release(loadedLevelAsset);
  Package_Unmount((EngineFileHandle)mountedPackage);
  failureResult.failed = true;
  failureResult.runtimeRootOrError = (uint32_t)rootCursorOrError;
  return failureResult;
}


/* Address: 0x005651D0.
   Ownership: gameplay/session/runtime.
   Purpose: Unregisters the in-game timer, clears and presents the framebuffer, tears down the active root and
   world state, closes movie and terrain resources, releases four texture assets, resets registries and input, and
   decrements the cursor-visibility token.
   Local calls: InGameRuntime_PublishRootWorldStatePointer, InGameRuntime_ReleaseFactionScratchBuffers.
   Cross-module calls: GraphicsShadingRuntime_Shutdown [graphics/render/shading],
   WorldRuntime_ForEachNodeInOwnerListD8 [world/runtime/core], InGameLevelRuntime_ShutdownLoadedAssetResources
   [gameplay/session/level], UiRootStack_PopCf [ui/controls/layout], Movie_Close [movie/runtime/playback],
   TerrainCompositeTexture_Destroy [world/terrain/visuals].
*/
void __thandor_void_preserve_eax_ecx_edx InGameRuntime_ShutdownAndReleaseResources(void)

{
  WorldRuntimeContext *world;
  InGameRuntimeRootImageC3E4 *inGameRoot;
  bool beginAccessFailed;
  
  (*g_TimerUnregisterPeriodic)(InGameRuntime_PeriodicCountdownAndClockTick);
  inGameRoot = g_InGameRuntimeRoot;
  (*g_GraphicsCursorSetFrame)(6);
  beginAccessFailed = (*g_GraphicsFramebufferBeginAccess)();
  if (!beginAccessFailed) {
    (*g_GraphicsFramebufferFillRectArgb)
              (g_FramebufferHeight,g_FramebufferWidth,0,0,g_FramebufferHeight,g_FramebufferWidth,0,0
               ,0xff000000,g_FramebufferAccess);
    (*g_GraphicsFramebufferEndAccess)();
  }
  (*g_GraphicsFramebufferPresent)(g_FramebufferAccess);
  GraphicsShadingRuntime_Shutdown();
  if (inGameRoot != (InGameRuntimeRootImageC3E4 *)0x0) {
    InGameRuntime_PublishRootWorldStatePointer(&inGameRoot->rootUi0000);
    world = &inGameRoot->worldRuntime0A30;
    WorldRuntime_ForEachNodeInOwnerListD8
              (world,WorldRuntimeNode_ReleaseShutdownBindingsCallback,world);
    InGameLevelRuntime_ShutdownLoadedAssetResources(world);
    if ((inGameRoot->rootUi0000).previousRoot != (UiRootNode *)0x0) {
      UiRootStack_PopCf(&inGameRoot->rootUi0000);
    }
    (*g_MemoryApi.free)(inGameRoot);
    g_InGameRuntimeRoot = (InGameRuntimeRootImageC3E4 *)0x0;
  }
  InGameRuntime_ReleaseFactionScratchBuffers();
  (*g_MemoryApi.free)(g_InGameWorldObjectRecords);
  g_InGameWorldObjectRecords = (WorldObjectRecord *)0x0;
  Movie_Close();
  TerrainCompositeTexture_Destroy();
  (*g_GraphicsTextureSourceLifecycleCallbacks3.releasePackage)(g_InGameDiagramTextureSource);
  (*g_GraphicsTextureSourceLifecycleCallbacks3.releasePackage)(g_InGamePanelTextureSource);
  (*g_GraphicsTextureSourceLifecycleCallbacks3.releasePackage)(g_InGameTechnologyTextureSource);
  (*g_GraphicsTextureSourceLifecycleCallbacks3.releasePackage)(g_InGameWindowTextureSource);
  g_InGameDiagramTextureSource = (GraphicsTextureSourceAsset *)0x0;
  g_InGamePanelTextureSource = (GraphicsTextureSourceAsset *)0x0;
  g_InGameTechnologyTextureSource = (GraphicsTextureSourceAsset *)0x0;
  g_InGameWindowTextureSource = (GraphicsTextureSourceAsset *)0x0;
  GraphicsShadingRuntime_ClearRecordTable();
  SelectionInfoPanel_ShutdownResources();
  SpriteAssetRegistry_Reset();
  UiFrame_FlushInputAndResetPendingTicks();
  g_CursorVisibilityToken = g_CursorVisibilityToken + -1;
  return;
}


/* Address: 0x0050E0D0.
   Ownership: gameplay/session/runtime.
   Purpose: Releases and clears the paired per-faction scratch-buffer arrays used by the in-game runtime.
*/
void __thandor_void_preserve_eax_ecx InGameRuntime_ReleaseFactionScratchBuffers(void)

{
  int remainingFactions;
  void **scratchBufferSetBCursor;
  void **scratchBufferSetACursor;

  remainingFactions = 8;
  scratchBufferSetACursor = g_InGameFactionScratchBufferSetA8;
  scratchBufferSetBCursor = g_InGameFactionScratchBufferSetB8;
  do {
    (*g_MemoryApi.free)(*scratchBufferSetACursor);
    (*g_MemoryApi.free)(*scratchBufferSetBCursor);
    *scratchBufferSetACursor = (void *)0x0;
    *scratchBufferSetBCursor = (void *)0x0;
    scratchBufferSetACursor = scratchBufferSetACursor + 1;
    scratchBufferSetBCursor = scratchBufferSetBCursor + 1;
    remainingFactions = remainingFactions + -1;
  } while (remainingFactions != 0);
  return;
}


/* Address: 0x0050E120.
   Ownership: gameplay/session/runtime.
   Purpose: Advances faction tail states, reevaluates the sixty-four scheduled condition records through the
   verified fourteen-way jump table, processes completion records and end-state effects, and performs the
   associated field-runtime cleanup paths. InGameConditionRuntimeStorageView800_V287 is a function-specific storage
   view, not a replacement for the existing InGameConditionRuntime record-array view.
   [VERSIONLESS_CANONICAL_DATATYPE_CLOSURE] Retired detached enum dictionary InGameConditionExpressionOpcode after
   transferring its complete value vocabulary to code annotation. It is not a safe whole-value storage type.
   Values: 252=INGAME_CONDITION_EXPRESSION_END, 253=INGAME_CONDITION_EXPRESSION_NOT,
   254=INGAME_CONDITION_EXPRESSION_AND, 255=INGAME_CONDITION_EXPRESSION_OR [VERSIONLESS_CANONICAL_DATATYPE_CLOSURE]
   Retired detached enum dictionary InGameEndConditionTriggerStateFlags after transferring its complete value
   vocabulary to code annotation.
   Cross-module calls: ModelRuntimeHierarchy_ApplyFlags418UnlessBit8Recursive [world/model/hierarchy].
*/

void __thandor_void_preserve_eax_ecx_edx InGameConditionRuntime_UpdateScheduledRecords(void)

{
  ResourceExtractionDescriptor32 *cellExtractionFlags;
  uint32_t secondFactionIndex;
  int modelRecord;
  int *modelRuntime;
  InGameLevelConditionStorageView800 *levelConditionStorage;
  uint32_t operandValue;
  uint32_t countOrFactionIndex;
  uint8_t tokenOrShift;
  int remainingCount;
  InGameScheduledConditionKind kindOrExpressionValue;
  uint32_t cellsLeftOrFaction;
  uint8_t *byteCursor;
  uint32_t cellCount;
  FactionRuntimeLifecycleObservedState *otherLifecycleState;
  WorldRuntimeContext *contextArg;
  FactionRuntimeLifecycleObservedState *lifecycleState;
  WorldOwnerListNode100 *worldNode;
  InGameConditionScheduleImageView480 *scheduledCondition;
  InGameEndConditionTriggerRecord8 *endTrigger;
  FieldGridAsset *conditionFieldGrid;
  InGameRuntimeRootImageC3E4 *triggerRoot;
  InGameRuntimeRootImageC3E4 *relationRoot;
  
  lifecycleState = g_GameFactionRuntimeImage.tail.factionLifecycleStates;
  remainingCount = 7;
  do {
    lifecycleState = lifecycleState + 1;
    if (*lifecycleState == FACTION_RUNTIME_LIFECYCLE_ENDING_PENDING) {
      *lifecycleState = *lifecycleState + FACTION_RUNTIME_LIFECYCLE_ACTIVE;
    }
    levelConditionStorage = g_InGameLevelRuntimeGlobalBlock.conditionStorage;
    remainingCount = remainingCount + -1;
  } while (remainingCount != 0);
  remainingCount = 0x40;
  scheduledCondition = &(g_InGameLevelRuntimeGlobalBlock.conditionStorage)->schedule;
  do {
    kindOrExpressionValue = scheduledCondition->conditions[0].statusAndKind.kind;
    scheduledCondition->conditions[0].statusAndKind.kind =
         scheduledCondition->conditions[0].statusAndKind.kind & 0xfffffffe;
                    // WARNING: Switch is manually overridden
    switch(kindOrExpressionValue & 0xfe) {
    case INGAME_SCHEDULED_CONDITION_NO_ACTIVE_ENTITY_WITH_DEFINITION:
      for (worldNode = (g_InGameRuntimeRoot->worldRuntime0A30).ownerListHead;
          worldNode != (WorldOwnerListNode100 *)0x0; worldNode = worldNode->nextNode) {
        if ((worldNode->ownerClassId == WORLD_OWNER_RUNTIME_MODEL) &&
           (scheduledCondition->conditions[0].payload.operands[0] ==
            *(uint32_t *)(*(int *)((int)worldNode->runtimePayload + 8) + 0xc)))
        goto InGameScheduledCondition_AdvanceToNextRecord;
      }
      goto InGameConditionRuntime_UpdateScheduledRecords_MarkCurrentConditionSatisfied;
    case INGAME_SCHEDULED_CONDITION_NO_ACTIVE_ENTITY_WITH_DEFINITION_AND_CLASS_COMMAND_GROUP_A:
      for (worldNode = (g_InGameRuntimeRoot->worldRuntime0A30).ownerListHead;
          worldNode != (WorldOwnerListNode100 *)0x0; worldNode = worldNode->nextNode) {
        if (((worldNode->ownerClassId == WORLD_OWNER_RUNTIME_MODEL) &&
            (g_ArmyRuntimeOrderHandlerMatrix11Columns24Classes.classCommand
             [*(int *)(*(int *)worldNode->runtimePayload + 0x4c)] ==
             ArmyRuntime_ClassCommandHandlerGroupACf)) &&
           (*(uint32_t *)(*(int *)((int)worldNode->runtimePayload + 8) + 0xc) ==
            scheduledCondition->conditions[0].payload.operands[0]))
        goto InGameScheduledCondition_AdvanceToNextRecord;
      }
      goto InGameConditionRuntime_UpdateScheduledRecords_MarkCurrentConditionSatisfied;
    case INGAME_SCHEDULED_CONDITION_NO_ACTIVE_ENTITY_WITH_DEFINITION_AND_RUNTIME_ID:
      for (worldNode = (g_InGameRuntimeRoot->worldRuntime0A30).ownerListHead;
          worldNode != (WorldOwnerListNode100 *)0x0; worldNode = worldNode->nextNode) {
        if (((worldNode->ownerClassId == WORLD_OWNER_RUNTIME_MODEL) &&
            (modelRecord = *(int *)((int)worldNode->runtimePayload + 8),
            scheduledCondition->conditions[0].payload.operands[0] == *(uint32_t *)(modelRecord + 0xc))) &&
           (*(uint32_t *)(modelRecord + 0xa0) == scheduledCondition->conditions[0].payload.operands[2]))
        goto InGameScheduledCondition_AdvanceToNextRecord;
      }
      goto InGameConditionRuntime_UpdateScheduledRecords_MarkCurrentConditionSatisfied;
    case INGAME_SCHEDULED_CONDITION_FACTION_INACTIVE_OR_RELATION_AT_LEAST_8:
      operandValue = scheduledCondition->conditions[0].payload.operands[1];
      secondFactionIndex = scheduledCondition->conditions[0].payload.operands[0];
      if (((g_GameFactionRuntimeImage.tail.factionLifecycleStates[operandValue] !=
            FACTION_RUNTIME_LIFECYCLE_ACTIVE) ||
          (g_GameFactionRuntimeImage.tail.factionLifecycleStates[secondFactionIndex] !=
           FACTION_RUNTIME_LIFECYCLE_ACTIVE)) ||
         (7 < (g_GameFactionRuntimeImage.records[operandValue].packedRelationStates >>
               ((char)secondFactionIndex * '\x04' & 0x1fU) & 0xf)))
      goto InGameConditionRuntime_UpdateScheduledRecords_MarkCurrentConditionSatisfied;
      break;
    case INGAME_SCHEDULED_CONDITION_PRIMARY_RESOURCE_CURRENT_AT_LEAST:
      if ((int)scheduledCondition->conditions[0].payload.operands[1] <=
          (int)g_GameFactionRuntimeImage.records[scheduledCondition->conditions[0].payload.operands[0]].
               xeniteCurrentQ4) {
        scheduledCondition->conditions[0].statusAndKind.kind = scheduledCondition->conditions[0].statusAndKind.kind | 1;
      }
      break;
    case INGAME_SCHEDULED_CONDITION_SECONDARY_RESOURCE_CURRENT_AT_LEAST:
      if ((int)scheduledCondition->conditions[0].payload.operands[1] <=
          (int)g_GameFactionRuntimeImage.records[scheduledCondition->conditions[0].payload.operands[0]].
               tritiumCurrentQ4) {
        scheduledCondition->conditions[0].statusAndKind.kind = scheduledCondition->conditions[0].statusAndKind.kind | 1;
      }
      break;
    case INGAME_SCHEDULED_CONDITION_ACTIVE_ARMY_SCALE_VALUE_AT_LEAST:
      if ((int)scheduledCondition->conditions[0].payload.operands[1] <=
          (int)g_GameFactionRuntimeImage.records[scheduledCondition->conditions[0].payload.operands[0]].
               tritiumExtractionRateQ4PerTick) {
        scheduledCondition->conditions[0].statusAndKind.kind = scheduledCondition->conditions[0].statusAndKind.kind | 1;
      }
      break;
    case INGAME_SCHEDULED_CONDITION_MATCHING_DEFINITION_AND_RUNTIME_ID_ACTIVE_ENTITY_COUNT_AT_LEAST:
      operandValue = scheduledCondition->conditions[0].payload.operands[1];
      for (worldNode = (g_InGameRuntimeRoot->worldRuntime0A30).ownerListHead;
          worldNode != (WorldOwnerListNode100 *)0x0; worldNode = worldNode->nextNode) {
        if (((worldNode->ownerClassId == WORLD_OWNER_RUNTIME_MODEL) &&
            (*(uint32_t *)(*(int *)((int)worldNode->runtimePayload + 8) + 0xc) ==
             scheduledCondition->conditions[0].payload.operands[0])) &&
           ((*(uint32_t *)(*(int *)((int)worldNode->runtimePayload + 8) + 0xa0) ==
             scheduledCondition->conditions[0].payload.operands[2] && (operandValue = operandValue - 1, operandValue == 0))))
        goto InGameConditionRuntime_UpdateScheduledRecords_MarkCurrentConditionSatisfied;
      }
      break;
    case INGAME_SCHEDULED_CONDITION_FACTION_TERRAIN_OCCUPANCY_MASK_F9_PERCENT_AT_LEAST:
      conditionFieldGrid = (g_InGameRuntimeRoot->worldRuntime0A30).fieldGrid;
      cellCount = conditionFieldGrid->gridWidth * conditionFieldGrid->gridHeight;
      byteCursor = conditionFieldGrid->cells[0].runtime0C_3F +
                (scheduledCondition->conditions[0].payload.operands[0] - 0xc);
      countOrFactionIndex = 0;
      cellsLeftOrFaction = cellCount;
      do {
        cellExtractionFlags = (ResourceExtractionDescriptor32 *)(byteCursor + 0x70);
        byteCursor = byteCursor + 0x80;
        countOrFactionIndex = countOrFactionIndex + ((*cellExtractionFlags & 0xf9) != 0);
        cellsLeftOrFaction = cellsLeftOrFaction - 1;
      } while (cellsLeftOrFaction != 0);
      if ((int)scheduledCondition->conditions[0].payload.operands[1] <=
          (int)(((uint64_t)countOrFactionIndex * 100) / (uint64_t)cellCount)) {
        scheduledCondition->conditions[0].statusAndKind.kind = scheduledCondition->conditions[0].statusAndKind.kind | 1;
      }
      break;
    case INGAME_SCHEDULED_CONDITION_COUNTDOWN_ELAPSED:
      operandValue = scheduledCondition->conditions[0].payload.operands[1] - g_InGameSimulationStepTicks;
      scheduledCondition->conditions[0].payload.operands[1] = operandValue;
      if ((int)operandValue < 1) {
        scheduledCondition->conditions[0].statusAndKind.kind = scheduledCondition->conditions[0].statusAndKind.kind | 1;
        scheduledCondition->conditions[0].payload.operands[1] = 0;
      }
      break;
    case INGAME_SCHEDULED_CONDITION_PRIMARY_RESOURCE_LIMIT_AT_MOST_0FA0:
      if ((int)g_GameFactionRuntimeImage.records[scheduledCondition->conditions[0].payload.operands[0]].
               xeniteStorageLimitQ4 < 0xfa1) {
        scheduledCondition->conditions[0].statusAndKind.kind = scheduledCondition->conditions[0].statusAndKind.kind | 1;
      }
      break;
    case INGAME_SCHEDULED_CONDITION_NO_ACTIVE_ENTITY_WITH_CLASS_ID_OUTSIDE_CLASS_COMMAND_GROUP_A:
      for (worldNode = (g_InGameRuntimeRoot->worldRuntime0A30).ownerListHead;
          worldNode != (WorldOwnerListNode100 *)0x0; worldNode = worldNode->nextNode) {
        if (((worldNode->ownerClassId == WORLD_OWNER_RUNTIME_MODEL) &&
            (operandValue = *(uint32_t *)(*(int *)worldNode->runtimePayload + 0x4c),
            g_ArmyRuntimeOrderHandlerMatrix11Columns24Classes.classCommand[operandValue] !=
            ArmyRuntime_ClassCommandHandlerGroupACf)) &&
           (operandValue == scheduledCondition->conditions[0].payload.operands[0]))
        goto InGameScheduledCondition_AdvanceToNextRecord;
      }
InGameConditionRuntime_UpdateScheduledRecords_MarkCurrentConditionSatisfied:
      scheduledCondition->conditions[0].statusAndKind.kind = scheduledCondition->conditions[0].statusAndKind.kind | 1;
      break;
    case INGAME_SCHEDULED_CONDITION_BOOLEAN_POSTFIX_EXPRESSION:
      byteCursor = (uint8_t *)((int)&scheduledCondition->conditions[0].statusAndKind.kind + 1);
      kindOrExpressionValue = INGAME_SCHEDULED_CONDITION_NONE_OR_UNUSED;
      while( true ) {
        while( true ) {
          while( true ) {
            while( true ) {
              tokenOrShift = *byteCursor;
              byteCursor = byteCursor + 1;
              if (tokenOrShift != 0xff) break;
              kindOrExpressionValue = kindOrExpressionValue >> 1 | kindOrExpressionValue & 1;
            }
            if (tokenOrShift != 0xfe) break;
            kindOrExpressionValue = kindOrExpressionValue >> 1 & (kindOrExpressionValue | 0xfffffffe);
          }
          if (tokenOrShift != 0xfd) break;
          kindOrExpressionValue = kindOrExpressionValue ^ 1;
        }
        if (tokenOrShift == 0xfc) break;
        kindOrExpressionValue = ((levelConditionStorage->schedule).conditions[tokenOrShift].statusAndKind.kind & 1) + kindOrExpressionValue * 2;
      }
      scheduledCondition->conditions[0].statusAndKind.kind =
           scheduledCondition->conditions[0].statusAndKind.kind | kindOrExpressionValue & 1;
    }
InGameScheduledCondition_AdvanceToNextRecord:
    scheduledCondition = (InGameConditionScheduleImageView480 *)(scheduledCondition->conditions + 1);
    remainingCount = remainingCount + -1;
    if (remainingCount == 0) {
      endTrigger = (InGameEndConditionTriggerRecord8 *)(levelConditionStorage->schedule).triggers;
      remainingCount = 0x10;
      do {
        if ((endTrigger->stateFlags == INGAME_END_CONDITION_TRIGGER_ACTIVE) &&
           (((levelConditionStorage->schedule).conditions[endTrigger->conditionIndex].statusAndKind.kind & 1) !=
            INGAME_SCHEDULED_CONDITION_NONE_OR_UNUSED)) {
          endTrigger->stateFlags = endTrigger->stateFlags | INGAME_END_CONDITION_TRIGGER_PROCESSED;
          triggerRoot = g_InGameRuntimeRoot;
          cellsLeftOrFaction = (uint32_t)endTrigger->factionRuntimeIndex;
          if (g_GameFactionRuntimeImage.tail.factionLifecycleStates[cellsLeftOrFaction] ==
              FACTION_RUNTIME_LIFECYCLE_ACTIVE) {
            contextArg = &g_InGameRuntimeRoot->worldRuntime0A30;
            g_GameFactionRuntimeImage.tail.factionLifecycleStates[cellsLeftOrFaction] =
                 FACTION_RUNTIME_LIFECYCLE_ENDING_PENDING;
            if (endTrigger->skipArmyDisableWhenOne != 1) {
              worldNode = (triggerRoot->worldRuntime0A30).ownerListHead;
              if (worldNode != (WorldOwnerListNode100 *)0x0) {
                do {
                  if ((worldNode->ownerClassId == WORLD_OWNER_RUNTIME_MODEL) &&
                     (modelRuntime = *(int **)((int)worldNode->runtimePayload + 8),
                     cellsLeftOrFaction == modelRuntime[3])) {
                    ModelRuntimeHierarchy_ApplyFlags418UnlessBit8Recursive(contextArg,modelRuntime);
                  }
                  worldNode = worldNode->nextNode;
                } while (worldNode != (WorldOwnerListNode100 *)0x0);
                g_GameFactionRuntimeImage.records[cellsLeftOrFaction].secondaryArmyAssetCount = 0;
                g_GameFactionRuntimeImage.records[cellsLeftOrFaction].primaryArmyAssetCount = 0;
              }
              relationRoot = g_InGameRuntimeRoot;
              if (cellsLeftOrFaction == (triggerRoot->worldRuntime0A30).activeFactionRuntimeIndex) {
                g_UiCommandRuntimeFlags = g_UiCommandRuntimeFlags | 0x108;
              }
              lifecycleState = g_GameFactionRuntimeImage.tail.factionLifecycleStates;
              cellsLeftOrFaction = 1;
              tokenOrShift = 4;
              do {
                lifecycleState = lifecycleState + 1;
                if (*lifecycleState == FACTION_RUNTIME_LIFECYCLE_ACTIVE) {
                  countOrFactionIndex = cellsLeftOrFaction + 1;
                  otherLifecycleState = lifecycleState;
                  do {
                    otherLifecycleState = otherLifecycleState + 1;
                    if ((*otherLifecycleState == FACTION_RUNTIME_LIFECYCLE_ACTIVE) &&
                       ((g_GameFactionRuntimeImage.records[countOrFactionIndex].packedRelationStates >>
                         (tokenOrShift & 0x1f) & 0xf) < 8)) {
                      if ((uint32_t)endTrigger->factionRuntimeIndex ==
                          (g_InGameRuntimeRoot->worldRuntime0A30).activeFactionRuntimeIndex) {
                        g_InGameRuntimeRoot->observedRelationTransitionFlags4D54 =
                             g_InGameRuntimeRoot->observedRelationTransitionFlags4D54 | 8;
                        *(uint32_t *)(relationRoot->opaque4D58_9A6B + 0x10a4) =
                             *(uint32_t *)(relationRoot->opaque4D58_9A6B + 0x10a4) | 8;
                        *(uint32_t *)(relationRoot->opaque4D58_9A6B + 0x2970) =
                             *(uint32_t *)(relationRoot->opaque4D58_9A6B + 0x2970) | 8;
                        *(uint32_t *)(relationRoot->opaque4D58_9A6B + 0x3f3c) =
                             *(uint32_t *)(relationRoot->opaque4D58_9A6B + 0x3f3c) | 8;
                      }
                      return;
                    }
                    countOrFactionIndex = countOrFactionIndex + 1;
                  } while (countOrFactionIndex < 8);
                }
                cellsLeftOrFaction = cellsLeftOrFaction + 1;
                tokenOrShift = tokenOrShift + 4;
              } while (cellsLeftOrFaction < 7);
              contextArg = &g_InGameRuntimeRoot->worldRuntime0A30;
              cellsLeftOrFaction = (uint32_t)endTrigger->factionRuntimeIndex;
            }
            countOrFactionIndex = contextArg->activeFactionRuntimeIndex;
            g_EndMovieVariantIndex = (uint32_t)endTrigger->movieVariantSelector;
            if (((countOrFactionIndex != cellsLeftOrFaction) &&
                (g_EndMovieVariantIndex = 0,
                g_GameFactionRuntimeImage.tail.factionLifecycleStates[countOrFactionIndex] <
                FACTION_RUNTIME_LIFECYCLE_ENDED_OR_TRANSITIONED)) &&
               (g_GameFactionRuntimeImage.tail.factionLifecycleStates[countOrFactionIndex] != 0)) {
              g_EndMovieVariantIndex = endTrigger->movieVariantSelector ^ 1;
              if (3 < (g_GameFactionRuntimeImage.records[countOrFactionIndex].packedRelationStates >>
                       ((char)cellsLeftOrFaction * '\x04' & 0x1fU) & 0xf)) {
                g_EndMovieVariantIndex = (uint32_t)endTrigger->movieVariantSelector;
              }
            }
            g_EndMovieSelectionIndex = (uint32_t)endTrigger->endMovieSelectionIndex;
            g_EndMoviePath = (uint16_t *)u_flm_ende0000_flm_0050df06;
            if (g_EndMovieVariantIndex == 0) {
              g_EndMoviePath = (uint16_t *)u_flm_ende0001_flm_0050df28;
            }
            g_UiCommandRuntimeFlags = g_UiCommandRuntimeFlags | 0x800;
            return;
          }
        }
        endTrigger = endTrigger + 1;
        remainingCount = remainingCount + -1;
        if (remainingCount == 0) {
          return;
        }
      } while( true );
    }
  } while( true );
}


/* Address: 0x00513160.
   Ownership: gameplay/session/runtime.
   Purpose: Clears per-frame faction accumulators, decays auxiliary counters, removes transient field-cell flags,
   computes connected terrain-region contributions, sorts active runtime-capacity records, updates faction capacity
   and notifications, and refreshes the persistent stat table. [RESOURCE_FUEL_ENERGY_CAPACITY_SEPARATION_CLOSURE]
   Faction economy owner. FLD 0x0800 connected regions feed Xenite stock/rate/total; FLD 0x1000 regions feed
   Tritium stock/rate/total. Energy supply = baselineEnergySupplyQ4 + Tritium stock * 0x10, capped by
   energyGenerationCapacityQ4. [VERSIONLESS_CANONICAL_DATATYPE_CLOSURE] Economy scalar domains are versionless
   canonical types: XeniteAmountQ4, TritiumAmountQ4, EnergyAmountQ4, EnergyDemandQ4 and
   ResourceExtractionRateQ4PerTick.
   Cross-module calls: TerrainRegionCollection_CollectConnectedCellsRecursive [world/terrain/editing],
   InGameNotificationQueue_InsertPriorityRecord [ui/ingame/runtime],
   GameFactionRuntime_RecomputeProgressAndScoreMetrics [gameplay/faction/runtime].
*/

void __fastcall InGameRuntime_UpdateFactionResourceExtractionAndEnergyAllocationState(void)

{
  uint8_t *totalAccumulator;
  FieldGridDimension fieldGridWidth;
  int *attachedRuntime;
  InGameLevelConditionStorageView800 *levelConditionStorage;
  InGameRuntimeRootImageC3E4 *inGameRoot;
  uint32_t valueOrFactionIndex;
  int tickContribution;
  int counterOrValue;
  int cellCountOrValue;
  int rebasedModel;
  int cellsRemaining;
  uint32_t entryCountOrValue;
  FactionArmyAssetCount remainingArmyAssets;
  uint32_t remainingEntries;
  FieldCellPackedFlagsAndMaterial requiredOccupancyMask;
  uint32_t *entryCursor;
  uint32_t supplyOrSwapValue;
  uint32_t remainingEnergy;
  TerrainRegionCollectionCount remainingRegionEntries;
  WorldRuntimeContext *worldRuntime;
  GameFactionRuntimeRecord *factionRecord;
  int resourceOffsetOrValue;
  GameFactionRuntimeImage *factionImageCursor;
  GameFactionRuntimeRecord *reverseFactionRecord;
  int *runtimeOrStatCursor;
  uint32_t *pairPressureRow;
  FieldGridCell *firstCell;
  FieldGridCell *clearCursor;
  FieldGridCell *cell;
  uint32_t factionOrDemand;
  uint32_t *sortBaseOrFlags;
  InGameNotificationMovieId notificationMovieId;
  bool queueCapacityNotification;
  FieldGridAsset *factionFieldGrid;
  WorldOwnerListNode100 *worldNode;
  
  inGameRoot = g_InGameRuntimeRoot;
  factionRecord = g_GameFactionRuntimeImage.records;
  pairPressureRow = g_GameDataAuxState.pairPressureMatrix8x8;
  counterOrValue = 8;
  do {
    factionRecord->suppliedEnergyDemandQ4 = 0;
    factionRecord->unpoweredEnergyDemandQ4 = 0;
    factionRecord->xeniteExtractionRateQ4PerTick = 0;
    factionRecord->tritiumExtractionRateQ4PerTick = 0;
    *pairPressureRow = *pairPressureRow * 7 >> 3;
    pairPressureRow[1] = pairPressureRow[1] * 7 >> 3;
    pairPressureRow[2] = pairPressureRow[2] * 7 >> 3;
    pairPressureRow[3] = pairPressureRow[3] * 7 >> 3;
    pairPressureRow[4] = pairPressureRow[4] * 7 >> 3;
    pairPressureRow[5] = pairPressureRow[5] * 7 >> 3;
    pairPressureRow[6] = pairPressureRow[6] * 7 >> 3;
    pairPressureRow[7] = pairPressureRow[7] * 7 >> 3;
    if (factionRecord->anchorCooldown1 != 0) {
      factionRecord->anchorCooldown1 = factionRecord->anchorCooldown1 - 1;
    }
    if (factionRecord->anchorCooldown2 != 0) {
      factionRecord->anchorCooldown2 = factionRecord->anchorCooldown2 - 1;
    }
    if (factionRecord->primaryAnchorCooldown != 0) {
      factionRecord->primaryAnchorCooldown = factionRecord->primaryAnchorCooldown - 1;
    }
    if (factionRecord->anchorCooldown0 != 0) {
      factionRecord->anchorCooldown0 = factionRecord->anchorCooldown0 - 1;
    }
    factionRecord->relationTransitionTick = factionRecord->relationTransitionTick + 1;
    pairPressureRow = pairPressureRow + 8;
    factionRecord = factionRecord + 1;
    counterOrValue = counterOrValue + -1;
  } while (counterOrValue != 0);
  factionFieldGrid = (inGameRoot->worldRuntime0A30).fieldGrid;
  fieldGridWidth = factionFieldGrid->gridWidth;
  cellCountOrValue = fieldGridWidth * factionFieldGrid->gridHeight;
  firstCell = factionFieldGrid->cells;
  requiredOccupancyMask = FIELD_CELL_XENITE_SUPPORT;
  resourceOffsetOrValue = 0;
  counterOrValue = cellCountOrValue;
  clearCursor = firstCell;
  do {
    do {
      clearCursor->flagsAndMaterial =
           clearCursor->flagsAndMaterial & ~FIELD_CELL_CONNECTED_REGION_VISITED;
      counterOrValue = counterOrValue + -1;
      cellsRemaining = cellCountOrValue;
      cell = firstCell;
      clearCursor = clearCursor + 1;
    } while (counterOrValue != 0);
    do {
      if (((cell->flagsAndMaterial & 0x88016000) == 0) &&
         ((cell->flagsAndMaterial & requiredOccupancyMask) != 0)) {
        g_TerrainRegionCollectionStoredCount = 0;
        g_TerrainRegionCollectionVisitedCount = 0;
        TerrainRegionCollection_CollectConnectedCellsRecursive
                  (requiredOccupancyMask,fieldGridWidth << 7,cell);
        if (g_TerrainRegionCollectionStoredCount != 0) {
          counterOrValue = (int)g_TerrainRegionCollectionVisitedCount /
                  (int)g_TerrainRegionCollectionStoredCount;
          entryCursor = g_TerrainRegionCollectionEntries;
          remainingRegionEntries = g_TerrainRegionCollectionStoredCount;
          do {
            factionOrDemand = *entryCursor >> 0xd & 0x7ff;
            valueOrFactionIndex = counterOrValue * 2 * (*entryCursor >> 0x18) *
                    g_GameFactionRuntimeImage.records[factionOrDemand].terrainContributionScaleQ8 >> 0xf;
            entryCountOrValue = entryCursor[1];
            runtimeOrStatCursor = (int *)(resourceOffsetOrValue + THANDOR_ADDR(g_GameFactionRuntimeImage,0x8) + factionOrDemand * 0x740);
            *runtimeOrStatCursor = *runtimeOrStatCursor + valueOrFactionIndex;
            tickContribution = valueOrFactionIndex * g_InGameSimulationStepTicks;
            totalAccumulator = g_GameFactionRuntimeImage.records[factionOrDemand].reserved78_87 + resourceOffsetOrValue + -0x78;
            *(int *)totalAccumulator = *(int *)totalAccumulator + tickContribution;
            runtimeOrStatCursor = (int *)(resourceOffsetOrValue + THANDOR_ADDR(g_GameFactionRuntimeImage,0xc) + factionOrDemand * 0x740);
            *runtimeOrStatCursor = *runtimeOrStatCursor + tickContribution;
            if ((entryCountOrValue != 0) &&
               (rebasedModel = entryCountOrValue + g_ModelRuntimeRebaseDelta, *(int *)(rebasedModel + 4) != 0)) {
              *(int *)(rebasedModel + 0x60) = tickContribution;
            }
            entryCursor = entryCursor + 2;
            remainingRegionEntries = remainingRegionEntries - 1;
          } while (remainingRegionEntries != 0);
        }
      }
      cellsRemaining = cellsRemaining + -1;
      cell = cell + 1;
    } while (cellsRemaining != 0);
    requiredOccupancyMask = requiredOccupancyMask * 2;
    resourceOffsetOrValue = resourceOffsetOrValue + 0x10;
    counterOrValue = cellCountOrValue;
    clearCursor = firstCell;
  } while (requiredOccupancyMask == FIELD_CELL_TRITIUM_SUPPORT);
  factionImageCursor = &g_GameFactionRuntimeImage;
  counterOrValue = 8;
  do {
    entryCountOrValue = factionImageCursor->records[0].xeniteStorageLimitQ4;
    valueOrFactionIndex = factionImageCursor->records[0].tritiumStorageLimitQ4;
    if (entryCountOrValue < factionImageCursor->records[0].xeniteCurrentQ4) {
      factionImageCursor->records[0].xeniteCurrentQ4 = entryCountOrValue;
    }
    if (valueOrFactionIndex < factionImageCursor->records[0].tritiumCurrentQ4) {
      factionImageCursor->records[0].tritiumCurrentQ4 = valueOrFactionIndex;
    }
    factionImageCursor = (GameFactionRuntimeImage *)(factionImageCursor->records + 1);
    counterOrValue = counterOrValue + -1;
  } while (counterOrValue != 0);
  entryCountOrValue = 0;
  entryCursor = g_TerrainRegionCollectionEntries;
  for (worldNode = (g_InGameRuntimeRoot->worldRuntime0A30).ownerListHead;
      worldNode != (WorldOwnerListNode100 *)0x0; worldNode = worldNode->nextNode) {
    if ((worldNode->ownerClassId == WORLD_OWNER_RUNTIME_MODEL) &&
       (runtimeOrStatCursor = worldNode->runtimePayload, (runtimeOrStatCursor[0x3b] & 0x10U) == 0)) {
      if (runtimeOrStatCursor[0x3d] != 0) {
        if (0xff < entryCountOrValue) continue;
        valueOrFactionIndex = *(uint32_t *)(runtimeOrStatCursor[2] + 0xc);
        counterOrValue = *(int *)(*runtimeOrStatCursor + 0x4c);
        *entryCursor = (uint32_t)runtimeOrStatCursor;
        entryCursor[1] = valueOrFactionIndex;
        valueOrFactionIndex = runtimeOrStatCursor[0x3d];
        entryCursor[3] = *(uint32_t *)(&g_FactionEnergyAllocationPriorityByModelClass + counterOrValue * 4);
        entryCursor[2] = valueOrFactionIndex;
        entryCountOrValue = entryCountOrValue + 1;
        entryCursor = entryCursor + 4;
      }
      counterOrValue = runtimeOrStatCursor[3];
      if ((entryCountOrValue < 0x100) && ((*(uint32_t *)(*runtimeOrStatCursor + 0x68) & 0x80) != 0)) {
        for (; counterOrValue != 0; counterOrValue = counterOrValue + -1) {
          attachedRuntime = (int *)runtimeOrStatCursor[0x50];
          if (((attachedRuntime != (int *)0x0) && (attachedRuntime[0x3d] != 0)) && (entryCountOrValue < 0x100)) {
            valueOrFactionIndex = *(uint32_t *)(attachedRuntime[2] + 0xc);
            cellCountOrValue = *(int *)(*attachedRuntime + 0x4c);
            *entryCursor = (uint32_t)attachedRuntime;
            entryCursor[1] = valueOrFactionIndex;
            valueOrFactionIndex = attachedRuntime[0x3d];
            entryCursor[3] = *(uint32_t *)(&g_FactionEnergyAllocationPriorityByModelClass + cellCountOrValue * 4);
            entryCursor[2] = valueOrFactionIndex;
            entryCountOrValue = entryCountOrValue + 1;
            entryCursor = entryCursor + 4;
          }
          runtimeOrStatCursor = runtimeOrStatCursor + 8;
        }
      }
    }
  }
  if (entryCountOrValue != 0) {
    if (1 < entryCountOrValue) {
      valueOrFactionIndex = ((uint32_t *)(uintptr_t)g_TerrainRegionCollectionEntries)[3];
      entryCursor = (uint32_t *)(uintptr_t)g_TerrainRegionCollectionEntries + 4; /* 16-byte records: +1 entry */
      counterOrValue = entryCountOrValue - 1;
      factionOrDemand = entryCountOrValue;
      sortBaseOrFlags = g_TerrainRegionCollectionEntries;
      while( true ) {
        do {
          if (valueOrFactionIndex < entryCursor[3]) {
            LOCK();
            supplyOrSwapValue = entryCursor[3];
            entryCursor[3] = valueOrFactionIndex;
            UNLOCK();
            sortBaseOrFlags[3] = supplyOrSwapValue;
            LOCK();
            valueOrFactionIndex = *entryCursor;
            *entryCursor = *sortBaseOrFlags;
            UNLOCK();
            *sortBaseOrFlags = valueOrFactionIndex;
            LOCK();
            valueOrFactionIndex = entryCursor[2];
            entryCursor[2] = sortBaseOrFlags[2];
            UNLOCK();
            sortBaseOrFlags[2] = valueOrFactionIndex;
            LOCK();
            valueOrFactionIndex = entryCursor[1];
            entryCursor[1] = sortBaseOrFlags[1];
            UNLOCK();
            sortBaseOrFlags[1] = valueOrFactionIndex;
            valueOrFactionIndex = supplyOrSwapValue;
          }
          entryCursor = entryCursor + 4;
          counterOrValue = counterOrValue + -1;
        } while (counterOrValue != 0);
        if (factionOrDemand - 1 < 2) break;
        valueOrFactionIndex = sortBaseOrFlags[7];
        entryCursor = sortBaseOrFlags + 8;
        counterOrValue = factionOrDemand - 2;
        factionOrDemand = factionOrDemand - 1;
        sortBaseOrFlags = sortBaseOrFlags + 4;
      }
    }
    valueOrFactionIndex = 7;
    reverseFactionRecord = g_GameFactionRuntimeImage.records + 7;
    do {
      entryCursor = g_TerrainRegionCollectionEntries;
      counterOrValue = 0;
      factionOrDemand = 0;
      for (remainingArmyAssets = reverseFactionRecord->primaryArmyAssetCount; remainingArmyAssets != 0; remainingArmyAssets = remainingArmyAssets - 1) {
        if (*(int *)(reverseFactionRecord->primaryArmyAssetPointersOrIds[counterOrValue] + 0x74) == 0) {
          factionOrDemand = factionOrDemand + 0x10;
        }
        else {
          factionOrDemand = factionOrDemand + 0x50;
        }
        counterOrValue = counterOrValue + 1;
      }
      supplyOrSwapValue = reverseFactionRecord->tritiumCurrentQ4 * 0x10 +
               reverseFactionRecord->baselineEnergySupplyQ4;
      if ((int)reverseFactionRecord->energyGenerationCapacityQ4 < (int)supplyOrSwapValue) {
        supplyOrSwapValue = reverseFactionRecord->energyGenerationCapacityQ4;
      }
      reverseFactionRecord->suppliedEnergyDemandQ4 =
           reverseFactionRecord->suppliedEnergyDemandQ4 + factionOrDemand;
      remainingEnergy = supplyOrSwapValue - factionOrDemand;
      remainingEntries = entryCountOrValue;
      if (supplyOrSwapValue < factionOrDemand) {
        remainingEnergy = 0;
      }
      do {
        factionOrDemand = entryCursor[2];
        if (valueOrFactionIndex == entryCursor[1]) {
          supplyOrSwapValue = *entryCursor;
          if (remainingEnergy < factionOrDemand) {
            sortBaseOrFlags = (uint32_t *)(supplyOrSwapValue + 0xec);
            *sortBaseOrFlags = *sortBaseOrFlags | 1;
            reverseFactionRecord->unpoweredEnergyDemandQ4 =
                 reverseFactionRecord->unpoweredEnergyDemandQ4 + factionOrDemand;
          }
          else {
            remainingEnergy = remainingEnergy - factionOrDemand;
            reverseFactionRecord->suppliedEnergyDemandQ4 =
                 reverseFactionRecord->suppliedEnergyDemandQ4 + factionOrDemand;
            sortBaseOrFlags = (uint32_t *)(supplyOrSwapValue + 0xec);
            *sortBaseOrFlags = *sortBaseOrFlags & 0xfffffffe;
          }
        }
        levelConditionStorage = g_InGameLevelRuntimeGlobalBlock.conditionStorage;
        entryCursor = entryCursor + 4;
        remainingEntries = remainingEntries - 1;
      } while (remainingEntries != 0);
      factionOrDemand = reverseFactionRecord->suppliedEnergyDemandQ4;
      supplyOrSwapValue = factionOrDemand - reverseFactionRecord->baselineEnergySupplyQ4;
      if (factionOrDemand < reverseFactionRecord->baselineEnergySupplyQ4) {
        supplyOrSwapValue = 0;
      }
      if (reverseFactionRecord->unpoweredEnergyDemandQ4 == 0) {
        reverseFactionRecord->anchorCooldown1 = 0;
      }
      else if ((g_InGameRuntimeRoot->worldRuntime0A30).activeFactionRuntimeIndex == valueOrFactionIndex) {
        queueCapacityNotification = false;
        if (reverseFactionRecord->energyGenerationCapacityQ4 <
            factionOrDemand + reverseFactionRecord->unpoweredEnergyDemandQ4) {
          if (reverseFactionRecord->anchorCooldown1 == 0) {
            notificationMovieId = 400;
            reverseFactionRecord->anchorCooldown1 = 0x96;
            queueCapacityNotification = true;
          }
        }
        else if (reverseFactionRecord->anchorCooldown2 == 0) {
          notificationMovieId = 0x191;
          reverseFactionRecord->anchorCooldown2 = 0x96;
          queueCapacityNotification = true;
        }
        if (queueCapacityNotification) {
          /* Level header text starting with UTF-16 "t00_tu": a fixed notification, and both cooldowns never
             expire. */
          if (((*(int *)(levelConditionStorage->levelImage).header.opaque100_16F == 0x300074) &&
              (*(int *)((levelConditionStorage->levelImage).header.opaque100_16F + 4) == 0x5f0030)) &&
             (*(int *)((levelConditionStorage->levelImage).header.opaque100_16F + 8) == 0x750074)) {
            notificationMovieId = 0x192;
            reverseFactionRecord->anchorCooldown1 = 0x7fffffff;
            reverseFactionRecord->anchorCooldown2 = 0x7fffffff;
          }
          InGameNotificationQueue_InsertPriorityRecord(NOTIFICATION_PAYLOAD_NONE,0,0,0,0,0,3,notificationMovieId);
        }
      }
      reverseFactionRecord->tritiumCurrentQ4 =
           reverseFactionRecord->tritiumCurrentQ4 - (supplyOrSwapValue >> 4) * g_InGameSimulationStepTicks;
      reverseFactionRecord = reverseFactionRecord + -1;
      valueOrFactionIndex = valueOrFactionIndex - 1;
    } while (valueOrFactionIndex != 0);
  }
  if ((g_GameFactionRuntimeImage.tail.simulationTick & 0x78) == 0) {
    counterOrValue = THANDOR_ADDR(g_GameFactionRuntimeImage,0x740);
    worldRuntime = &g_InGameRuntimeRoot->worldRuntime0A30;
    if (g_GameFactionRuntimeImage.tail.simulationTick >> 7 < 0x1000) {
      entryCountOrValue = 1;
      runtimeOrStatCursor = (int *)((g_GameFactionRuntimeImage.tail.simulationTick >> 7) * 0x38 +
                       (int)g_GameStatTableImage);
      do {
        GameFactionRuntime_RecomputeProgressAndScoreMetrics(entryCountOrValue,worldRuntime);
        cellCountOrValue = *(int *)(counterOrValue + 0x88);
        resourceOffsetOrValue = *(int *)(counterOrValue + 0x8c);
        if (cellCountOrValue < 0) {
          cellCountOrValue = 0;
        }
        if (resourceOffsetOrValue < 0) {
          resourceOffsetOrValue = 0;
        }
        *runtimeOrStatCursor = cellCountOrValue;
        runtimeOrStatCursor[1] = resourceOffsetOrValue;
        entryCountOrValue = entryCountOrValue + 1;
        counterOrValue = counterOrValue + 0x740;
        runtimeOrStatCursor = runtimeOrStatCursor + 2;
      } while (entryCountOrValue < 8);
    }
  }
  return;
}


/* Address: 0x0053D4F0.
   Ownership: gameplay/session/runtime.
   Purpose: Converts the global in-game cursor world position to grid coordinates and refreshes the cached view-
   scale and settings-derived fields.
   Cross-module calls: FieldGrid_WorldToGridQ12 [world/terrain/grid], PersistentSettings_ReadDword
   [core/settings/persistent].
*/
void __thandor_void_preserve_eax_ecx_edx InGameRuntime_UpdateCursorGridAndViewScaleCache(void)

{
  UQ12 committedDistance;
  uint32_t viewSettings;
  FieldGridCoordinatesEaxEdx8 cursorGridPosition;
  InGameRuntimeRootImageC3E4 *inGameRoot;
  
  inGameRoot = g_InGameRuntimeRoot;
  cursorGridPosition = FieldGrid_WorldToGridQ12
                    ((g_InGameRuntimeRoot->worldRuntime0A30).motion.targetPositionYQ12,
                     (g_InGameRuntimeRoot->worldRuntime0A30).motion.targetPositionXQ12);
  inGameRoot->fieldGridPosition9A6C = THANDOR_BITCAST(FieldGridCoordinatesEaxEdx8, FixedPlanarPointEdxEax8, cursorGridPosition);
  viewSettings = PersistentSettings_ReadDword(0,0x40);
  committedDistance = (inGameRoot->worldRuntime0A30).motion.committedDistanceQ12;
  if ((viewSettings & 2) == 0) {
    *(AngleTurn32 *)(inGameRoot->opaque9A74_9B4B + 4) =
         (inGameRoot->worldRuntime0A30).motion.headingAngle;
  }
  if ((viewSettings & 1) == 0) {
    *(int *)inGameRoot->opaque9A74_9B4B =
         (int)((uint64_t)((int64_t)(int)committedDistance * 0x6000000) >> 0x20);
  }
  return;
}


/* Address: 0x005651A0.
   Ownership: gameplay/session/runtime.
   Purpose: Before shutdown traversal, saves the text resource id currently shown by the in-game copy's
   worldViewCyclingInfoText (root offset 0x23D8, cycled 0x112..0x117) back into the in-game template and into the
   frontend template's bottomBarStatusText, so the choice survives the next template copy.
*/
void __thandor_void_preserve_eax_ecx
InGameRuntime_PublishRootWorldStatePointer(UiRootNode *inGameRoot)

{
  g_InGameTemplateWorldViewInfoTextResourceId =
       INGAME_UI_FIELD(inGameRoot,worldViewCyclingInfoText,0x54,int32_t);
  g_FrontendTemplateStatusTextResourceId = g_InGameTemplateWorldViewInfoTextResourceId;
  return;
}


/* Address: 0x00565E30.
   Ownership: gameplay/session/runtime.
   Purpose: Runs under the gameplay spin lock, advances command and synchronization state, processes host or client
   transfer records, updates simulation entities and periodic world systems, and dispatches the active tick phase.
   Table 00566040: 00566060, 00566080, 00566090, 005660A0, 005660F0, 00566100, 00566110, 00566120. The function has
   no semantic return value; the prior EDX:EAX result was preserved-register noise. It contains, but is broader
   than, switch(simulationTick & 7). Do not rename the whole function to a narrow wheel-only name.
   Local calls: InGameConditionRuntime_UpdateScheduledRecords,
   InGameRuntime_UpdateFactionResourceExtractionAndEnergyAllocationState.
   Cross-module calls: UiRuntimeRecordRing_DiscardOldestCf [ui/core/runtime],
   FrontendTransfer_BroadcastPendingCommandBatchAndSyncState [network/protocol/transfer],
   FrontendTransfer_HandleSyncRequest10021AndReply10023 [network/protocol/transfer],
   FrontendTransfer_DispatchStagedCommandRecords [network/protocol/transfer], UiRuntimeRecordRing_ContainsIdCf
   [ui/core/runtime], FrontendNetwork_HandleCommandBatchAndPlayerTimeoutCf [network/backend/runtime].
*/
void __thandor_void_preserve_eax_ecx_edx InGameRuntime_UpdateSimulationAndNetworkTick(void)

{
  uint32_t modelDefinition;
  uint32_t tickPhase;
  WorldRuntimeContext *worldRuntime;
  WorldOwnerListNode100 *worldNode;
  ModelRuntimeNode *modelNode;
  bool boolResult;
  RecordRingDiscardResult discardedRecord;
  InGameRuntimeRootImageC3E4 *inGameRoot;
  
  boolResult = (*g_SpinLockTryAcquire)(&g_InGameStateTickSpinLock);
  inGameRoot = g_InGameRuntimeRoot;
  if (boolResult) {
    return;
  }
  if ((g_UiCommandRuntimeFlags & 0x10) == 0) {
    if (2 < (int)g_InGamePendingSimulationTicks) goto InGameRuntime_ReleaseSimulationTickLockAndReturn;
    g_InGamePendingSimulationTicks = g_InGamePendingSimulationTicks + 1;
  }
  if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_CLIENT) == SESSION_NETWORK_ROLE_LOCAL) {
    if (g_InGameNetworkTickCountdown != 0) goto InGameRuntime_ReleaseSimulationTickLockAndReturn;
    g_InGameNetworkTickCountdown = 4;
    if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_HOST) != SESSION_NETWORK_ROLE_LOCAL) {
      if (g_SessionNetworkTickCounter % g_SessionNetworkTickInterval == 0) {
        while (g_HostCommandBatchSyncSentThisInterval == 0) {
          discardedRecord = UiRuntimeRecordRing_DiscardOldestCf();
          if (discardedRecord.empty) {
            boolResult = FrontendTransfer_BroadcastPendingCommandBatchAndSyncState(1);
            if (boolResult) goto InGameRuntime_ReleaseSimulationTickLockAndReturn;
            break;
          }
          FrontendTransfer_HandleSyncRequest10021AndReply10023
                    ((NetworkSessionContext *)discardedRecord.endpointOrReadIndex,
                     (FrontendTransferPacketUnion *)discardedRecord.payloadOrReadIndex);
        }
        FrontendTransfer_DispatchStagedCommandRecords();
        g_HostCommandBatchSyncSentThisInterval = 0;
      }
      else {
        while( true ) {
          discardedRecord = UiRuntimeRecordRing_DiscardOldestCf();
          if (discardedRecord.empty) break;
          FrontendTransfer_HandleSyncRequest10021AndReply10023
                    ((NetworkSessionContext *)discardedRecord.endpointOrReadIndex,
                     (FrontendTransferPacketUnion *)discardedRecord.payloadOrReadIndex);
        }
        if ((g_HostCommandBatchSyncSentThisInterval == 0) &&
           (boolResult = FrontendTransfer_BroadcastPendingCommandBatchAndSyncState(0), !boolResult)) {
          g_HostCommandBatchSyncSentThisInterval = 1;
        }
      }
    }
  }
  else {
    if (g_SessionNetworkTickCounter % g_SessionNetworkTickInterval == 0) {
      boolResult = UiRuntimeRecordRing_ContainsIdCf(g_FrontendSessionToken);
      if (!boolResult) goto InGameRuntime_ReleaseSimulationTickLockAndReturn;
      do {
        discardedRecord = UiRuntimeRecordRing_DiscardOldestCf();
        if (discardedRecord.empty) break;
        boolResult = FrontendNetwork_HandleCommandBatchAndPlayerTimeoutCf
                          ((NetworkSessionContext *)discardedRecord.endpointOrReadIndex,
                           (FrontendTransferPacketUnion *)discardedRecord.payloadOrReadIndex);
      } while (!boolResult);
      boolResult = FrontendTransfer_ConsumeProcessedFlagCf();
      if (boolResult) goto InGameRuntime_ReleaseSimulationTickLockAndReturn;
    }
    else if (g_InGameNetworkTickCountdown != 0)
    goto InGameRuntime_ReleaseSimulationTickLockAndReturn;
    g_InGameNetworkTickCountdown = 4;
  }
  g_SessionNetworkTickCounter = g_SessionNetworkTickCounter + 1;
  if ((g_UiCommandRuntimeFlags & 4) == 0) {
    if ((g_UiCommandRuntimeFlags & 0x11) == 0) {
      g_GameFactionRuntimeImage.tail.simulationTick =
           g_GameFactionRuntimeImage.tail.simulationTick + 1;
      worldRuntime = &inGameRoot->worldRuntime0A30;
      tickPhase = g_GameFactionRuntimeImage.tail.simulationTick & 7;
      for (worldNode = (inGameRoot->worldRuntime0A30).ownerListHead;
          worldNode != (WorldOwnerListNode100 *)0x0; worldNode = worldNode->nextNode) {
        (*(&g_RuntimeMaintenanceCallbackPhases.primaryUpdate.army)[worldNode->ownerClassId])
                  (worldRuntime,worldNode);
      }
      if (g_GameFactionRuntimeImage.tail.simulationTick % 0x14 == 0) {
        InGameConditionRuntime_UpdateScheduledRecords();
      }
                    // WARNING: Switch is manually overridden
      switch(tickPhase) {
      case 0:
        InGameRuntime_UpdateFactionResourceExtractionAndEnergyAllocationState();
        FrontendRuntime_UpdateCurrentFactionMetricCache();
        GameFactionRuntime_SynchronizeTechnologiesForRelationStates8To10();
        WorldLightingRuntime_UpdateInterpolatedTerrainLighting();
        break;
      case 1:
        TerrainGrid_RelaxNeighborHeightsForwardWithSignGate
                  ((inGameRoot->worldRuntime0A30).fieldGrid);
        break;
      case 2:
        AiFactionRuntime_RebuildPlanningCapacityState();
        break;
      case 3:
        modelNode = (ModelRuntimeNode *)(inGameRoot->worldRuntime0A30).ownerListHead;
        FieldGrid_ApplyByteClampLookupToCells
                  ((inGameRoot->worldRuntime0A30).activeFactionRuntimeIndex,
                   (inGameRoot->worldRuntime0A30).fieldGrid);
        if (modelNode != (ModelRuntimeNode *)0x0) {
          do {
            (*(&g_RuntimeMaintenanceCallbackPhases.terrainStateRefresh.army)
              [modelNode->ownerClassId])(worldRuntime,modelNode);
            modelNode = (ModelRuntimeNode *)(modelNode->common).nextNode;
          } while (modelNode != (ModelRuntimeNode *)0x0);
          FieldGrid_ApplyByteClampLookupToCells
                    ((inGameRoot->worldRuntime0A30).activeFactionRuntimeIndex,
                     (inGameRoot->worldRuntime0A30).fieldGrid);
        }
        break;
      case 4:
        GridInfluence_ClearDistanceBandsAndRefreshEntities
                  ((inGameRoot->worldRuntime0A30).ownerListHead);
        break;
      case 5:
        TerrainGrid_RelaxNeighborHeightsReverseWithSignGate
                  ((inGameRoot->worldRuntime0A30).fieldGrid);
        break;
      case 6:
        AiFactionRuntime_RebuildPlanningCapacityState();
        break;
      case 7:
        worldNode = (inGameRoot->worldRuntime0A30).ownerListHead;
        FieldGrid_ClearOccupancyMaskBits0To6AllCells((inGameRoot->worldRuntime0A30).fieldGrid);
        if ((g_UiCommandRuntimeFlags & 8) != 0) {
          FieldGrid_SetOccupancyMaskByteBit0AllCells
                    ((inGameRoot->worldRuntime0A30).activeFactionRuntimeIndex,
                     (inGameRoot->worldRuntime0A30).fieldGrid);
        }
        if (worldNode != (WorldOwnerListNode100 *)0x0) {
          do {
            (*(&g_RuntimeMaintenanceCallbackPhases.occupancyRebuild.army)[worldNode->ownerClassId])
                      (worldRuntime,worldNode);
            worldNode = worldNode->nextNode;
          } while (worldNode != (WorldOwnerListNode100 *)0x0);
          modelNode = (ModelRuntimeNode *)(inGameRoot->worldRuntime0A30).ownerListHead;
          do {
            (*(&g_RuntimeMaintenanceCallbackPhases.terrainStateRefresh.army)
              [modelNode->ownerClassId])(worldRuntime,modelNode);
            modelNode = (ModelRuntimeNode *)(modelNode->common).nextNode;
          } while (modelNode != (ModelRuntimeNode *)0x0);
          FieldGrid_ApplyByteClampLookupToCells
                    ((inGameRoot->worldRuntime0A30).activeFactionRuntimeIndex,
                     (inGameRoot->worldRuntime0A30).fieldGrid);
          GridScratch_PropagateFieldOccupancyMaskNeighborhood
                    ((inGameRoot->worldRuntime0A30).fieldGrid);
        }
      }
    }
  }
  else {
    modelNode = (ModelRuntimeNode *)(inGameRoot->worldRuntime0A30).ownerListHead;
    g_GameFactionRuntimeImage.tail.simulationTick =
         g_GameFactionRuntimeImage.tail.simulationTick + 1;
    if ((g_GameFactionRuntimeImage.tail.simulationTick & 1) == 0) {
      for (; modelNode != (ModelRuntimeNode *)0x0;
          modelNode = (ModelRuntimeNode *)(modelNode->common).nextNode) {
        if (modelNode->ownerClassId == WORLD_OWNER_RUNTIME_MODEL) {
          modelDefinition = (((modelNode->runtimePayload).modelRuntime)->definitionOrSavedId).savedIdOrOffset
          ;
          (*g_ArmyPlacementContactKindDispatchTable.callbacks[*(int *)(modelDefinition + 0x278)])
                    (*(Q12 *)(modelDefinition + 0x54),(modelNode->worldTransform).translation.y,
                     (modelNode->worldTransform).translation.x,modelNode,
                     &inGameRoot->worldRuntime0A30);
          ModelNodeRuntime_RebuildTransformsFromRoot(modelNode);
        }
      }
      if ((g_GameFactionRuntimeImage.tail.simulationTick & 0xc) == 0) {
        if ((g_GameFactionRuntimeImage.tail.simulationTick & 2) == 0) {
          GridInfluence_ClearDistanceBandsAndRefreshEntities
                    ((inGameRoot->worldRuntime0A30).ownerListHead);
        }
        else {
          GridScratch_RebuildTerrainAndRuntimeClassificationMasks(&inGameRoot->worldRuntime0A30);
        }
      }
    }
  }
InGameRuntime_ReleaseSimulationTickLockAndReturn:
  (*g_SpinLockRelease)(&g_InGameStateTickSpinLock);
  return;
}


/* Address: 0x0050E0B0.
   Ownership: gameplay/session/runtime.
   Purpose: Initialization callback used by both new-session and loaded-session setup. Both binary call sites pass
   one argument and branch on carry immediately after return. The reachable implementation clears carry and returns
   at 0050E0C1-0050E0C3. The detached bytes at 0050E0C4-0050E0CF form an unreachable carry-set epilogue with no
   direct branch, call, or absolute-pointer reference in the archived executable, so they are verified separately
   and are not included in the function range.
*/
uint8_t __thandor_cf_preserve_eax_ecx_edx
InGameRuntime_InitializeOptionalSubsystemAlwaysSuccessCf(uint32_t unusedArgument)

{
  return 0;
}


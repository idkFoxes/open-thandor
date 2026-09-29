/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/src/gameplay/session/runtime.c
 * Reverse engineering by idkFoxes 2026
 */

#include <thandor/gameplay/session/runtime.h>
#include <thandor/thandor.h>
#include <thandor/platform/bootstrap/image.h>

#ifdef THANDOR_TEST_AIDS
#include <stdlib.h>

/* Port-only test aid for campaign carry-over tests: OPEN_THANDOR_AUTOWIN=<seconds> ends a campaign level as won
   after that many seconds of the session (with OPEN_THANDOR_AUTOWIN_LEVELS=<k> only in the first k sessions). It
   picks the active end trigger of the level script whose end selection moves the campaign forward (a later
   level, else successor 0 = the campaign end; see the scoring below), moves the owner-list position of every
   mobile unit of the local faction that stands outside the local faction's exit zone of the campaign level
   record to the zone centre, and then sets the end movie state exactly as
   InGameConditionRuntime_UpdateScheduledRecords does for that trigger. From there the original code runs:
   the end movie, OldUnitRuntime_RebuildScenarioReplayTables (collects the units in the exit zone), the next
   level and OldUnitRuntime_MergeMasksAndReplayRecords. Called once per frame by InGameRuntime_RunSessionUntilExit. */
static void InGameRuntime_TestAidAutoWin(void)
{
  static unsigned sessionSeen;
  static unsigned sessionStart;
  static int done;
  const char *value = getenv("OPEN_THANDOR_AUTOWIN");
  CampaignAsset *campaign = (CampaignAsset *)g_FrontendLoadedCampaignAsset;
  CampaignLevelRecord *level = NULL;
  InGameEndConditionTriggerRecord8 *chosen = NULL;
  int chosenScore = 0;
  InGameEndConditionTriggerRecord8 *trigger;
  WorldOwnerListNode *ownerNode;
  uint32_t localFaction;
  int index;
  int moved = 0;
  if (value == NULL || g_InGameRuntimeRoot == NULL) {
    return;
  }
  /* OPEN_THANDOR_AUTOWIN_LEVELS=<k>: only the first k sessions of the process end automatically */
  if (getenv("OPEN_THANDOR_AUTOWIN_LEVELS") != NULL &&
      g_TestAidSessionCount > (unsigned)atoi(getenv("OPEN_THANDOR_AUTOWIN_LEVELS"))) {
    return;
  }
  if (sessionSeen != g_TestAidSessionCount) {
    sessionSeen = g_TestAidSessionCount;
    sessionStart = Thandor_TickCount();
    done = 0;
  }
  if (done || Thandor_TickCount() - sessionStart < (unsigned)atoi(value) * 1000u) {
    return;
  }
  done = 1;
  if (campaign == NULL) {
    Thandor_Log("test aid: auto-win: not a campaign level");
    return;
  }
  for (index = 0; index < campaign->levelRecordCount; index++) {
    if (campaign->levels[index].levelId == campaign->currentLevelId) {
      level = &campaign->levels[index];
    }
  }
  if (level == NULL) {
    Thandor_Log("test aid: auto-win: level %d has no campaign record", campaign->currentLevelId);
    return;
  }
  localFaction = (g_InGameRuntimeRoot->worldRuntime).activeFactionRuntimeIndex;
  /* the trigger that moves the campaign forward: a successor with a higher level id (score 3), else successor 0,
     the campaign end (2), else another known level (1); a trigger ending another faction than the local one wins
     a tie. A successor equal to the current level (replay) or unknown is never chosen, except as the last resort
     in a level without any other trigger. */
  trigger = (InGameEndConditionTriggerRecord8 *)(g_InGameLevelRuntimeGlobalBlock.conditionStorage)->schedule.triggers;
  for (index = 0; index < INGAME_END_CONDITION_TRIGGER_COUNT; index++, trigger++) {
    uint32_t selection = trigger->endMovieSelectionIndex;
    int successor;
    int score = 0;
    int other;
    if (trigger->stateFlags != INGAME_END_CONDITION_TRIGGER_ACTIVE || selection >= 8) {
      continue;
    }
    successor = level->successorLevelIds[selection];
    Thandor_Log("test aid: auto-win: level %d trigger %d: ends faction %u, end selection %u -> level %d", level->levelId,
                index, (unsigned)trigger->factionRuntimeIndex, (unsigned)selection, successor);
    for (other = 0; other < campaign->levelRecordCount; other++) {
      if (campaign->levels[other].levelId == successor && successor != level->levelId) {
        score = successor > level->levelId ? 3 : 1;
      }
    }
    if (successor == 0) {
      score = 2;
    }
    score = score * 2 + (trigger->factionRuntimeIndex != localFaction);
    if (chosen == NULL || score > chosenScore) {
      chosen = trigger;
      chosenScore = score;
    }
  }
  if (chosen == NULL) {
    Thandor_Log("test aid: auto-win: level %d has no active end trigger", level->levelId);
    return;
  }
  if (level->successorLevelIds[chosen->endMovieSelectionIndex] == 0 || chosenScore < 2) {
    Thandor_Log("test aid: auto-win: level %d has no successor level, firing the campaign end", level->levelId);
  }
  for (ownerNode = (g_InGameRuntimeRoot->worldRuntime).ownerListHead; ownerNode != NULL;
       ownerNode = ownerNode->nextNode) {
    ModelRuntimeSlot *model;
    if (ownerNode->ownerClassId != WORLD_OWNER_RUNTIME_MODEL) {
      continue;
    }
    model = (ModelRuntimeSlot *)ownerNode->runtimePayload;
    /* units already inside the zone stay where they are (many levels use centre 0,0 with a radius covering the
       whole map, i.e. everything is carried over at its position); radius 0 means no carry-over */
    if (level->exitZoneRadius[localFaction] <= 0 ||
        model->ownerArmyRuntimeOrSavedOffset.armyRuntime->factionIndex != localFaction ||
        model->definitionOrSavedId.runtimeDefinition->accelerationPerTick == 0 ||
        (int)FixedMath_Length2(level->exitZoneCenterY[localFaction] - ownerNode->worldYQ12,
                               level->exitZoneCenterX[localFaction] - ownerNode->worldXQ12) <=
            level->exitZoneRadius[localFaction]) {
      continue;
    }
    ownerNode->worldXQ12 = level->exitZoneCenterX[localFaction];
    ownerNode->worldYQ12 = level->exitZoneCenterY[localFaction];
    moved++;
  }
  /* end movie state as the level script sets it for this trigger (the ended faction is another one than the
     local one when a win trigger was found) */
  g_EndMovieVariantIndex = chosen->movieVariantSelector;
  if (chosen->factionRuntimeIndex != localFaction) {
    g_EndMovieVariantIndex = chosen->movieVariantSelector ^ 1;
  }
  g_EndMovieSelectionIndex = chosen->endMovieSelectionIndex;
  g_EndMoviePath = (uint16_t *)u_flm_ende0000_flm_0050df06;
  if (g_EndMovieVariantIndex == 0) {
    g_EndMoviePath = (uint16_t *)u_flm_ende0001_flm_0050df28;
  }
  g_UiCommandRuntimeFlags = g_UiCommandRuntimeFlags | UI_COMMAND_RUNTIME_FLAG_END_MOVIE_PENDING;
  Thandor_Log("test aid: auto-win: level %d, end selection %u -> level %d, exit zone radius %d, "
              "%d mobile units of faction %u outside it moved into it", level->levelId, (unsigned)chosen->endMovieSelectionIndex,
              level->successorLevelIds[chosen->endMovieSelectionIndex], level->exitZoneRadius[localFaction],
              moved, localFaction);
  Thandor_Log("test aid: auto-win: exit zone centre (%d,%d), destination (%d,%d)",
              level->exitZoneCenterX[localFaction], level->exitZoneCenterY[localFaction],
              level->exitZoneDestinationX[localFaction], level->exitZoneDestinationY[localFaction]);
}
#endif

/* Implementation ownership: gameplay/session/runtime. */

/* Address: 0x00564F70.
   Runs one in-game session from the frontend: starts a new level or loads a saved game (bit 0 of
   loadExistingSessionFlag), then renders frames until the session is closed, the end movie is due or the local
   player left, tears the session down along the matching path and returns to the frontend. A failed start or an
   emptied UI root stack returns an error code with CF set, which the caller hands to the fatal-error dispatcher.
*/
SessionRunResult InGameRuntime_RunSessionUntilExit(LevelAssetRuntimePrefix *levelAsset,
          FrontendBooleanState32 loadExistingSessionFlag,uint16_t *levelPathUtf16)

{
  uint32_t startupErrorOrExitCode;
  NewSessionInitResult newSessionInit;
  LoadedSessionInitResult loadedSessionInit;
  SessionRunResult localPlayerLeftResult;
  SessionRunResult sessionClosedResult;
  SessionRunResult endMovieResult;
  SessionRunResult failureResult;
  
  if ((loadExistingSessionFlag & 1U) == 0) {
    newSessionInit = InGameRuntime_InitializeNewSession(levelAsset,levelPathUtf16);
    startupErrorOrExitCode = newSessionInit.runtimeRootOrError;
    if (newSessionInit.failed) goto shutdown_and_fail;
  }
  else {
    loadedSessionInit = InGameRuntime_InitializeLoadedSession(levelPathUtf16);
    startupErrorOrExitCode = loadedSessionInit.runtimeRootOrError;
    if (loadedSessionInit.failed) goto shutdown_and_fail;
  }
#ifdef THANDOR_TEST_AIDS
  g_TestAidSessionCount++;
#endif
  do {
    g_TestAidInGameFrames++; /* project test aid, not part of the original code */
    /* two pending simulation ticks are consumed per rendered frame, clamped at zero */
    g_InGamePendingSimulationTicks = g_InGamePendingSimulationTicks - 2;
    if ((int)g_InGamePendingSimulationTicks < 0) {
      g_InGamePendingSimulationTicks = 0;
    }
    UiRootStack_InvalidateAll();
    UiFrame_ProcessAndPresent();
#ifdef THANDOR_TEST_AIDS
    InGameRuntime_TestAidAutoWin();
#endif
    /* The success paths return 0x0C in EAX; callers ignore it because CF is clear. */
    if ((g_UiCommandRuntimeFlags & UI_COMMAND_RUNTIME_FLAG_SESSION_CLOSED) != 0) {
      g_SoundStopAllVoices();
      g_TimerUnregisterPeriodic(InGameRuntime_ProcessQueuedSessionNotificationTimer);
      GridScratch_ReleaseBuffers();
      g_EndMovieSelectionIndex = 0;
      OldUnitRuntime_RebuildScenarioReplayTables();
      UiRuntime_SetSynchronizationHooks(NULL,NULL);
      UiRootStack_PopUntilWindowTextureBoundary();
      InGameRuntime_ShutdownAndReleaseResources();
      sessionClosedResult.exitCodeOrError = 12;
      sessionClosedResult.failed = false;
      return sessionClosedResult;
    }
    if ((g_UiCommandRuntimeFlags & UI_COMMAND_RUNTIME_FLAG_END_MOVIE_PENDING) != 0) {
      g_SoundStopAllVoices();
      g_TimerUnregisterPeriodic(InGameRuntime_ProcessQueuedSessionNotificationTimer);
      GridScratch_ReleaseBuffers();
      /* FrontendSession_PeriodicTick keeps running under the in-game tick lock while the end movie plays */
      UiRuntime_SetSynchronizationHooks(FrontendSession_PeriodicTick,&g_InGameStateTickSpinLock);
      Frontend_PlaySelectedEndMovie();
      OldUnitRuntime_RebuildScenarioReplayTables();
      UiRuntime_SetSynchronizationHooks(NULL,NULL);
      UiRootStack_PopUntilWindowTextureBoundary();
      InGameRuntime_ShutdownAndReleaseResources();
      g_FrontendScenarioPathScratchUtf16 = 0;
      endMovieResult.exitCodeOrError = 12;
      endMovieResult.failed = false;
      return endMovieResult;
    }
    if ((g_UiCommandRuntimeFlags & UI_COMMAND_RUNTIME_FLAG_LOCAL_PLAYER_LEFT) != 0) {
      g_SoundStopAllVoices();
      g_TimerUnregisterPeriodic(InGameRuntime_ProcessQueuedSessionNotificationTimer);
      GridScratch_ReleaseBuffers();
      UiRuntime_SetSynchronizationHooks(NULL,NULL);
      InGameRuntime_ShutdownAndReleaseResources();
      g_FrontendScenarioPathScratchUtf16 = 0;
      localPlayerLeftResult.exitCodeOrError = 12;
      localPlayerLeftResult.failed = false;
      return localPlayerLeftResult;
    }
  } while (g_UiRootNode != UI_ROOT_STACK_END);
  startupErrorOrExitCode = FATAL_ERROR_GENERAL_FAILURE;
shutdown_and_fail:
  InGameRuntime_ShutdownAndReleaseResources();
  failureResult.failed = true;
  failureResult.exitCodeOrError = startupErrorOrExitCode;
  return failureResult;
}


/* Address: 0x00566290.
   Frame update of the in-game UI root for the whole session: network session upkeep, the
   placement overlay, cursor frame and edge scrolling, keeping the camera target near the field, and, unless the
   interaction subsystem is active, ambient effect sounds, music selection, the camera keys, the countdown text and
   the terrain texture refresh. Nothing but the network upkeep runs while waiting for players.
*/

void InGameUiRoot_UpdateFrame(InGameRuntimeRootFrameView *inGameRoot)

{
  Q12 *motionCoordinate;
  WorldMotionState *cameraMotion;
  FieldGridAsset *cameraFieldGrid;
  WorldOwnerListNode *ownerNode;
  LevelMusicSampleNumber selectedMusicTrackId;
  int64_t projectedProduct;
  InGameLevelConditionStorage *levelConditionStorage;
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
  InGameConditionSchedule *scheduledCondition;
  bool voicePlaying;
  FieldGridCoordinates targetGridPosition;
  PageStackSearchResult pageStackStatus;
  SoundPlayResult playVoiceResult;
  uint32_t cursorFrameOrScratch;
  
  if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_CLIENT) == SESSION_NETWORK_ROLE_LOCAL) {
    if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_HOST) != SESSION_NETWORK_ROLE_LOCAL) {
      FrontendHostSession_TickPeerTimeoutsAndDropPlayers();
    }
  }
  else {
    FrontendClientSession_TickHostTimeout();
  }
  if ((g_UiCommandRuntimeFlags & UI_COMMAND_RUNTIME_FLAG_WAITING_FOR_PLAYERS) == 0) {
    /* placement overlay: grey the field and mark where the pending army asset fits; refreshed every 8th
       simulation tick, removed once the placement ends */
    if ((g_UiCommandRuntimeFlags & UI_COMMAND_RUNTIME_FLAG_PLACEMENT_OVERLAY_SHOWN) == 0) {
      if ((g_UiCommandRuntimeFlags & UI_COMMAND_RUNTIME_FLAG_PLACEMENT_PENDING) != 0) {
        g_UiCommandRuntimeFlags = g_UiCommandRuntimeFlags | UI_COMMAND_RUNTIME_FLAG_PLACEMENT_OVERLAY_SHOWN;
        FieldGrid_SetAllCellOverlayColors
                  (0xff808080,(inGameRoot->worldRuntime).fieldGrid);
        WorldRuntime_EmitModelDefinitionOverlayForMatchingEntries
                  ((void *)g_InGamePendingPlacementArmyAsset,
                   &inGameRoot->worldRuntime);
      }
    }
    else if ((g_UiCommandRuntimeFlags & UI_COMMAND_RUNTIME_FLAG_PLACEMENT_PENDING) == 0) {
      g_UiCommandRuntimeFlags = g_UiCommandRuntimeFlags & ~UI_COMMAND_RUNTIME_FLAG_PLACEMENT_OVERLAY_SHOWN;
      FieldGrid_SetAllCellOverlayColors
                (0xffffffff,(inGameRoot->worldRuntime).fieldGrid);
    }
    else if ((g_GameFactionRuntimeImage.tail.simulationTick & 7) == 0) {
      FieldGrid_SetAllCellOverlayColors
                (0xff808080,(inGameRoot->worldRuntime).fieldGrid);
      WorldRuntime_EmitModelDefinitionOverlayForMatchingEntries
                ((void *)g_InGamePendingPlacementArmyAsset,&inGameRoot->worldRuntime)
      ;
    }
    cursorFrameOrScratch = 0;
    hoveredNode = (*((inGameRoot->rootUi).base.vtable)->hitTest)
                       (g_CursorOverrideY,g_CursorOverrideX,(UiNodeBase *)inGameRoot);
    if (hoveredNode != UI_NODE_NONE) { /* hit test found a node */
      cursorFrameOrScratch = hoveredNode->vtable->pointerMove(g_CursorOverrideY,g_CursorOverrideX,hoveredNode);
    }
    g_GameFactionRuntimeImage.tail.presentationTick++;
    RecentTextHistory_SortAndBuildPointerList(8,&inGameRoot->recentTextHistory);
    currentPresentationTick = g_GameFactionRuntimeImage.tail.presentationTick;
    worldRuntime = &inGameRoot->worldRuntime;
    InGameHud_UpdateStatusCountersAndSessionPrompts();
    pageStackStatus = UiPageStack_ActivePageNotInList(&inGameRoot->worldViewAreaPageStack);
    if ((((((inGameRoot->worldRuntime).runtimeFlags & (WORLD_RUNTIME_FLAG_DRAG_SELECTING | WORLD_RUNTIME_FLAG_NOTIFICATION_GOTO)) == 0) &&
         (pageStackStatus.pageIndex == 0)) &&
        (((inGameRoot->worldRuntime).interaction.nodeFlags & 8) == 0)) &&
       (((g_CursorButtonState & 4) == 0 &&
        (candidateFrameOrScore = WorldRuntime_ApplyEdgeScrollAndGetCursorFrame(worldRuntime),
         candidateFrameOrScore != 0)))) {
      cursorFrameOrScratch = candidateFrameOrScore;
    }
    g_GraphicsCursorSetFrame(cursorFrameOrScratch);
    /* keep the camera target within 16 cells of the field: clamp the grid position, convert it back to world
       coordinates and move target and camera by the difference */
    cameraFieldGrid = (inGameRoot->worldRuntime).fieldGrid;
    outOfBoundsAxisCount = 0;
    targetGridPosition = FieldGrid_WorldToGridQ12
                       ((inGameRoot->worldRuntime).motion.targetPositionYQ12,
                        (inGameRoot->worldRuntime).motion.targetPositionXQ12);
    targetRowQ12 = targetGridPosition.rowQ12;
    columnDeltaOrCount = targetGridPosition.columnQ12;
    gridColumn = (columnDeltaOrCount >> Q12_SHIFT) - 8;
    gridRowOrDeltaY = (targetRowQ12 >> Q12_SHIFT) - 8;
    if (gridColumn < -16) {
      columnDeltaOrCount = -8 * Q12_ONE;
      outOfBoundsAxisCount = 1;
    }
    else if ((int)cameraFieldGrid->gridWidth < gridColumn) {
      outOfBoundsAxisCount = 1;
      columnDeltaOrCount = (cameraFieldGrid->gridWidth + 8) * Q12_ONE;
    }
    if (gridRowOrDeltaY < -16) {
      targetRowQ12 = -8 * Q12_ONE;
      outOfBoundsAxisCount++;
    }
    else if ((int)cameraFieldGrid->gridHeight < gridRowOrDeltaY) {
      outOfBoundsAxisCount++;
      targetRowQ12 = (cameraFieldGrid->gridHeight + 8) * Q12_ONE;
    }
    if (outOfBoundsAxisCount != 0) {
      projectedProduct = (int64_t)(targetRowQ12 + columnDeltaOrCount * 2) * FIELD_GRID_WORLD_COLUMN_STEP_X;
      columnDeltaOrCount = FIXED_PRODUCT_SHR(projectedProduct,13) -
              (inGameRoot->worldRuntime).motion.targetPositionXQ12;
      gridRowOrDeltaY = FIXED_PRODUCT_SHR(((int64_t)targetRowQ12 * -1999),Q12_SHIFT) -
               (inGameRoot->worldRuntime).motion.targetPositionYQ12;
      motionCoordinate = &(inGameRoot->worldRuntime).motion.targetPositionXQ12;
      *motionCoordinate = *motionCoordinate + columnDeltaOrCount;
      motionCoordinate = &(inGameRoot->worldRuntime).motion.targetPositionYQ12;
      *motionCoordinate = *motionCoordinate + gridRowOrDeltaY;
      cameraMotion = &(inGameRoot->worldRuntime).motion;
      cameraMotion->positionXQ12 = cameraMotion->positionXQ12 + columnDeltaOrCount;
      motionCoordinate = &(inGameRoot->worldRuntime).motion.positionYQ12;
      *motionCoordinate = *motionCoordinate + gridRowOrDeltaY;
      WorldRuntime_ClearFieldGridDirtyFlag(worldRuntime);
    }
    if ((g_UiCommandRuntimeFlags & UI_COMMAND_RUNTIME_FLAG_INTERACTION_SUBSYSTEM_ACTIVE) == 0) {
      TerrainDirectionTable_AdvanceAndRebuildVectors();
      cursorFrameOrScratch =
           PersistentSettings_Read(PERSISTENT_SOUND_OPTION_DEFAULT,PERSISTENT_SETTING_SOUND_OPTION_FLAGS);
      if ((cursorFrameOrScratch & PERSISTENT_SOUND_OPTION_EFFECTS) != 0) {
        /* every 8th frame: recompute the spatial sound gains of all world objects */
        if ((currentPresentationTick & 7) == 0) {
          SpatialSoundPool_ClearDesiredGains();
          for (ownerNode = (inGameRoot->worldRuntime).ownerListHead; ownerNode != NULL;
              ownerNode = ownerNode->nextNode) {
            /* the callback of the node's owner class (model, shot or effect) */
            (*(&g_RuntimeMaintenanceCallbackPhases.audioRefresh.army)[ownerNode->ownerClassId])
                      (worldRuntime,ownerNode);
          }
          SpatialSoundPool_ApplyDesiredGains();
          InGameSelectionDetailPanel_Rebuild();
        }
        /* g_InGameEffectsEnabled counts frames until the next ambient effect sound: at 0 it waits for the current
           one to end and starts a new delay of 1..64 frames; when it counts down to 0 one of the four level
           effects plays */
        if (g_InGameEffectsEnabled == 0) {
          voicePlaying = g_SoundIsVoicePlaying(g_InGameActiveEffectVoice);
          if (voicePlaying) {
            g_InGameActiveEffectVoice = NULL;
            cursorFrameOrScratch = Random_NextPrimary();
            g_InGameEffectsEnabled = (cursorFrameOrScratch & 0x3f) + 1;
          }
        }
        else {
          g_InGameEffectsEnabled--;
          if (g_InGameEffectsEnabled == 0) {
            cursorFrameOrScratch = PersistentSettings_Read(PERSISTENT_DEFAULT_GAIN_Q15,PERSISTENT_SETTING_EFFECTS_GAIN);
            candidateFrameOrScore = Random_NextPrimary();
            playVoiceResult = g_SoundPlayOneShot
                               (cursorFrameOrScratch,cursorFrameOrScratch,
                                (DirectSoundVoiceSet *)(&g_InGameLevelEffectVoiceSet0)[candidateFrameOrScore & 3]);
            if (!playVoiceResult.failed) {
              g_InGameActiveEffectVoice = playVoiceResult.soundBuffer;
            }
          }
        }
      }
      cursorFrameOrScratch =
           PersistentSettings_Read(PERSISTENT_SOUND_OPTION_DEFAULT,PERSISTENT_SETTING_SOUND_OPTION_FLAGS);
      levelConditionStorage = g_InGameLevelRuntimeGlobalBlock.conditionStorage;
      /* music: same delay scheme, then the best-suited of the four tracks */
      if ((cursorFrameOrScratch & PERSISTENT_SOUND_OPTION_MUSIC) != 0) {
        if (g_InGameMusicNextTrackCountdown == 0) {
          voicePlaying = g_SoundIsVoicePlaying(g_InGameActiveMusicVoice);
          if (voicePlaying) {
            g_InGameActiveMusicVoice = NULL;
            cursorFrameOrScratch = Random_NextPrimary();
            g_InGameMusicNextTrackCountdown = (cursorFrameOrScratch & 0x3f) + 1;
          }
        }
        else {
          g_InGameMusicNextTrackCountdown--;
          if (g_InGameMusicNextTrackCountdown == 0) {
            cursorFrameOrScratch = 0;
            bestTrackOrSecondsLeft = 0;
            trackIndex = 0;
            do {
              candidateFrameOrScore = InGameMusic_ComputeTrackSuitabilityScore
                                ((levelConditionStorage->levelImage).worldSettings.musicSampleNumbers[trackIndex],
                                 worldRuntime);
              nextTrackIndex = trackIndex + 1;
              if ((int)cursorFrameOrScratch < (int)candidateFrameOrScore) {
                bestTrackOrSecondsLeft = trackIndex;
                cursorFrameOrScratch = candidateFrameOrScore;
              }
              trackIndex = nextTrackIndex;
            } while (nextTrackIndex < 4);
            if (cursorFrameOrScratch != 0) {
              selectedMusicTrackId =
                   (levelConditionStorage->levelImage).worldSettings.musicSampleNumbers[bestTrackOrSecondsLeft];
              cursorFrameOrScratch = PersistentSettings_Read(PERSISTENT_DEFAULT_GAIN_Q15,PERSISTENT_SETTING_MUSIC_GAIN);
              g_EndGameResultsCurrentMusicTrackId = selectedMusicTrackId;
              playVoiceResult = g_SoundPlayOneShot
                                 (cursorFrameOrScratch,cursorFrameOrScratch,
                                  (DirectSoundVoiceSet *)(&g_InGameLevelMusicVoiceSet0)[bestTrackOrSecondsLeft]);
              if (!playVoiceResult.failed) {
                g_InGameActiveMusicVoice = playVoiceResult.soundBuffer;
              }
            }
          }
        }
      }
      if ((g_UiCommandRuntimeFlags & UI_COMMAND_RUNTIME_FLAG_PAUSED) != 0) goto update_cursor_grid;
      pageStackStatus = UiPageStack_ActivePageNotInList(&inGameRoot->gameWindowPageStack);
      if (pageStackStatus.pageIndex == 2) {
        InGameTechnologyPanel_Rebuild(&inGameRoot->rootUi);
      }
      InterpolationStateTable_Advance256ByTicks(g_InGameSimulationStepTicks);
      /* camera keys: arrows scroll by the configured step, Page Up/Down tilt, Insert/Delete rotate,
         Home/End zoom (0x400 = 1/64 turn, 0x800 = 0.5 in Q12) */
      if (g_KeyboardSpecialKeyDown[KEYBOARD_SPECIAL_KEY_LEFT] != 0) {
        cursorFrameOrScratch = PersistentSettings_Read(PERSISTENT_DEFAULT_CAMERA_SCROLL_STEP,PERSISTENT_SETTING_CAMERA_SCROLL_STEP);
        WorldRuntime_TranslateCameraByScreenDelta(0,-cursorFrameOrScratch,worldRuntime);
        WorldRuntime_RecomputeMotionEndpointAgainstFieldSurface(worldRuntime);
      }
      if (g_KeyboardSpecialKeyDown[KEYBOARD_SPECIAL_KEY_RIGHT] != 0) {
        cursorFrameOrScratch = PersistentSettings_Read(PERSISTENT_DEFAULT_CAMERA_SCROLL_STEP,PERSISTENT_SETTING_CAMERA_SCROLL_STEP);
        WorldRuntime_TranslateCameraByScreenDelta(0,cursorFrameOrScratch,worldRuntime);
        WorldRuntime_RecomputeMotionEndpointAgainstFieldSurface(worldRuntime);
      }
      if (g_KeyboardSpecialKeyDown[KEYBOARD_SPECIAL_KEY_UP] != 0) {
        cursorFrameOrScratch = PersistentSettings_Read(PERSISTENT_DEFAULT_CAMERA_SCROLL_STEP,PERSISTENT_SETTING_CAMERA_SCROLL_STEP);
        WorldRuntime_TranslateCameraByScreenDelta(-cursorFrameOrScratch,0,worldRuntime);
        WorldRuntime_RecomputeMotionEndpointAgainstFieldSurface(worldRuntime);
      }
      if (g_KeyboardSpecialKeyDown[KEYBOARD_SPECIAL_KEY_DOWN] != 0) {
        cursorFrameOrScratch = PersistentSettings_Read(PERSISTENT_DEFAULT_CAMERA_SCROLL_STEP,PERSISTENT_SETTING_CAMERA_SCROLL_STEP);
        WorldRuntime_TranslateCameraByScreenDelta(cursorFrameOrScratch,0,worldRuntime);
        WorldRuntime_RecomputeMotionEndpointAgainstFieldSurface(worldRuntime);
      }
      if (g_KeyboardSpecialKeyDown[KEYBOARD_SPECIAL_KEY_PAGE_UP] != 0) {
        clampedPitchAngle = (inGameRoot->worldRuntime).motion.pitchAngle - INGAME_CAMERA_KEY_ANGLE_STEP;
        if ((int)clampedPitchAngle < (int)(inGameRoot->worldRuntime).motion.minimumPitchAngle) {
          clampedPitchAngle = (inGameRoot->worldRuntime).motion.minimumPitchAngle;
        }
        WorldRuntime_PointCameraAtTarget
                  (clampedPitchAngle,(inGameRoot->worldRuntime).motion.headingAngle,
                   (inGameRoot->worldRuntime).motion.targetDistanceQ12,
                   (inGameRoot->worldRuntime).motion.targetPositionZQ12,
                   (inGameRoot->worldRuntime).motion.targetPositionYQ12,
                   (inGameRoot->worldRuntime).motion.targetPositionXQ12,worldRuntime)
        ;
      }
      if (g_KeyboardSpecialKeyDown[KEYBOARD_SPECIAL_KEY_PAGE_DOWN] != 0) {
        clampedPitchAngle = (inGameRoot->worldRuntime).motion.pitchAngle + INGAME_CAMERA_KEY_ANGLE_STEP;
        if ((int)(inGameRoot->worldRuntime).motion.maximumPitchAngle < (int)clampedPitchAngle) {
          clampedPitchAngle = (inGameRoot->worldRuntime).motion.maximumPitchAngle;
        }
        WorldRuntime_PointCameraAtTarget
                  (clampedPitchAngle,(inGameRoot->worldRuntime).motion.headingAngle,
                   (inGameRoot->worldRuntime).motion.targetDistanceQ12,
                   (inGameRoot->worldRuntime).motion.targetPositionZQ12,
                   (inGameRoot->worldRuntime).motion.targetPositionYQ12,
                   (inGameRoot->worldRuntime).motion.targetPositionXQ12,worldRuntime)
        ;
      }
      if (g_KeyboardSpecialKeyDown[KEYBOARD_SPECIAL_KEY_INSERT] != 0) {
        WorldRuntime_PointCameraAtTarget
                  ((inGameRoot->worldRuntime).motion.pitchAngle,
                   (inGameRoot->worldRuntime).motion.headingAngle - INGAME_CAMERA_KEY_ANGLE_STEP & FIXED_ANGLE16_MASK,
                   (inGameRoot->worldRuntime).motion.targetDistanceQ12,
                   (inGameRoot->worldRuntime).motion.targetPositionZQ12,
                   (inGameRoot->worldRuntime).motion.targetPositionYQ12,
                   (inGameRoot->worldRuntime).motion.targetPositionXQ12,worldRuntime)
        ;
      }
      if (g_KeyboardSpecialKeyDown[KEYBOARD_SPECIAL_KEY_DELETE] != 0) {
        WorldRuntime_PointCameraAtTarget
                  ((inGameRoot->worldRuntime).motion.pitchAngle,
                   (inGameRoot->worldRuntime).motion.headingAngle + INGAME_CAMERA_KEY_ANGLE_STEP & FIXED_ANGLE16_MASK,
                   (inGameRoot->worldRuntime).motion.targetDistanceQ12,
                   (inGameRoot->worldRuntime).motion.targetPositionZQ12,
                   (inGameRoot->worldRuntime).motion.targetPositionYQ12,
                   (inGameRoot->worldRuntime).motion.targetPositionXQ12,worldRuntime)
        ;
      }
      if (g_KeyboardSpecialKeyDown[KEYBOARD_SPECIAL_KEY_HOME] != 0) {
        clampedTargetDistance = (inGameRoot->worldRuntime).motion.targetDistanceQ12 - INGAME_CAMERA_KEY_DISTANCE_STEP_Q12;
        if ((int)clampedTargetDistance < (int)(inGameRoot->worldRuntime).minimumCameraDistanceQ12) {
          clampedTargetDistance = (inGameRoot->worldRuntime).minimumCameraDistanceQ12;
        }
        WorldRuntime_PointCameraAtTarget
                  ((inGameRoot->worldRuntime).motion.pitchAngle,
                   (inGameRoot->worldRuntime).motion.headingAngle,clampedTargetDistance,
                   (inGameRoot->worldRuntime).motion.targetPositionZQ12,
                   (inGameRoot->worldRuntime).motion.targetPositionYQ12,
                   (inGameRoot->worldRuntime).motion.targetPositionXQ12,worldRuntime)
        ;
      }
      if (g_KeyboardSpecialKeyDown[KEYBOARD_SPECIAL_KEY_END] != 0) {
        clampedTargetDistance = (inGameRoot->worldRuntime).motion.targetDistanceQ12 + INGAME_CAMERA_KEY_DISTANCE_STEP_Q12;
        if ((int)(inGameRoot->worldRuntime).maximumCameraDistanceQ12 < (int)clampedTargetDistance) {
          clampedTargetDistance = (inGameRoot->worldRuntime).maximumCameraDistanceQ12;
        }
        WorldRuntime_PointCameraAtTarget
                  ((inGameRoot->worldRuntime).motion.pitchAngle,
                   (inGameRoot->worldRuntime).motion.headingAngle,clampedTargetDistance,
                   (inGameRoot->worldRuntime).motion.targetPositionZQ12,
                   (inGameRoot->worldRuntime).motion.targetPositionYQ12,
                   (inGameRoot->worldRuntime).motion.targetPositionXQ12,worldRuntime)
        ;
      }
      levelConditionStorage = g_InGameLevelRuntimeGlobalBlock.conditionStorage;
      inGameRoot->countdownPanelNodeFlags =
           inGameRoot->countdownPanelNodeFlags & ~UI_NODE_SUPPRESSED;
      currentPresentationTick = g_GameFactionRuntimeImage.tail.presentationTick;
      /* countdown text: the first of the 64 scheduled conditions that is a running countdown shows its
         remaining seconds as minutes:seconds; without one the timer node stays hidden */
      columnDeltaOrCount = INGAME_SCHEDULED_CONDITION_COUNT;
      scheduledCondition = &levelConditionStorage->schedule;
      do {
        if (((scheduledCondition->conditions[0].statusAndKind.kind & INGAME_SCHEDULED_CONDITION_KIND_MASK) ==
             INGAME_SCHEDULED_CONDITION_COUNTDOWN_ELAPSED) &&
           (bestTrackOrSecondsLeft = scheduledCondition->conditions[0].payload.operands[1],
            bestTrackOrSecondsLeft != 0)) {
          cursorFrameOrScratch = g_WideNumberFormatUtf16
                             (WIDE_FORMAT_PAD_WITH_SPACE,0,2,1,bestTrackOrSecondsLeft / 60,
                              (uint16_t *)THANDOR_ADDR(g_InGameCountdownTextUtf16,0));
          *(uint16_t *)(cursorFrameOrScratch + THANDOR_ADDR(g_InGameCountdownTextUtf16,0)) = ':';
          g_WideNumberFormatUtf16
                    (WIDE_FORMAT_WRITE_TERMINATOR|WIDE_FORMAT_PAD_WITH_ZERO,0,2,1,bestTrackOrSecondsLeft % 60,
                     (uint16_t *)(cursorFrameOrScratch + THANDOR_ADDR(g_InGameCountdownTextUtf16,2)));
          currentPresentationTick = g_GameFactionRuntimeImage.tail.presentationTick;
          goto refresh_terrain_composite;
        }
        scheduledCondition = (InGameConditionSchedule *)(scheduledCondition->conditions + 1);
        columnDeltaOrCount--;
      } while (columnDeltaOrCount != 0);
      inGameRoot->countdownPanelNodeFlags =
           inGameRoot->countdownPanelNodeFlags | UI_NODE_SUPPRESSED;
    }
refresh_terrain_composite:
    if ((currentPresentationTick & 31) == 0) {
      TerrainCompositeTexture_FillPlane1();
    }
    if ((currentPresentationTick & 3) == 0) {
      TerrainCompositeTexture_RebuildPlane0();
    }
  }
update_cursor_grid:
  InGameRuntime_UpdateCursorGridAndViewScaleCache();
  return;
}


/* Address: 0x0050EA90.
   Turns the saved form of the resource registration records (widget.hex) back into pointers, after a savegame
   load and after writing a savegame: the 1-based offsets become runtime-object, shading-record, army/shot/effect
   slot pointers, texture set and palette are re-selected per domain, and the sprite id is resolved again. Also
   restores the tail record pointer and the local player's faction assignment.
*/
void ResourceRegistrationRuntime_RebaseLoadedRecords(ResourceRegistrationRuntimeImage *runtimeImage)

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
  ResourceRegistrationRecord *registrationRecord;
  uint32_t remainingRecords;
  uint8_t *nestedBasePointer;
  ResourceRegistrationRecord *nestedCursor;
  ArmyRuntimeSlot *payloadSlot;
  
  registrationRecord = runtimeImage->records;
  remainingRecords = runtimeImage->recordCount;
  do {
    if ((registrationRecord->flags & RUNTIME_REGISTRATION_RECORD_ALLOCATED) != 0) {
      primaryPointer = (uint8_t *)(registrationRecord->primaryPointerOrSavedOffset).savedIdOrOffset;
      secondaryPointer = (registrationRecord->secondaryPointerOrSavedOffset).runtimePointer;
      nestedBasePointer = (registrationRecord->nestedBasePointerOrSavedOffset).runtimePointer;
      /* 1-based offsets from the runtime-object base; 0 stays NULL */
      if (primaryPointer != NULL) {
        primaryPointer = primaryPointer + (int)g_RuntimeObjectRebaseBaseMinusOne;
      }
      if (secondaryPointer != NULL) {
        secondaryPointer = secondaryPointer + (int)g_RuntimeObjectRebaseBaseMinusOne;
      }
      if (nestedBasePointer != NULL) {
        nestedBasePointer = nestedBasePointer + (int)g_RuntimeObjectRebaseBaseMinusOne;
      }
      (registrationRecord->primaryPointerOrSavedOffset).savedIdOrOffset = (uint32_t)primaryPointer;
      (registrationRecord->secondaryPointerOrSavedOffset).runtimePointer = secondaryPointer;
      (registrationRecord->nestedBasePointerOrSavedOffset).runtimePointer = nestedBasePointer;
      (registrationRecord->ownerRuntimeOrSavedOffset).runtimePointer = runtimeImage;
      auxiliaryPointer = (uint8_t *)(registrationRecord->auxiliaryPointerOrSavedOffset).savedIdOrOffset;
      nestedRemaining = registrationRecord->nestedCount;
      if (auxiliaryPointer != NULL) {
        /* 1-based offset from the shading records; 0 is null */
        auxiliaryPointer = (uint8_t *)(THANDOR_ADDR(g_GraphicsShadingRuntimeRecords,-1) + (int)auxiliaryPointer);
      }
      (registrationRecord->auxiliaryPointerOrSavedOffset).savedIdOrOffset = (uint32_t)auxiliaryPointer;
      nestedCursor = registrationRecord;
      selectedPalette = g_ShotPalette;
      for (; g_ShotPalette = selectedPalette, nestedRemaining != 0; nestedRemaining = nestedRemaining - 1) {
        if (nestedCursor->nestedPointersOrSavedOffsets[0].runtimePointer != NULL) {
          nestedCursor->nestedPointersOrSavedOffsets[0].runtimePointer =
               (uint8_t *)((int)nestedCursor->nestedPointersOrSavedOffsets[0].runtimePointer +
                       (int)g_RuntimeObjectRebaseBaseMinusOne);
        }
        nestedCursor = (ResourceRegistrationRecord *)&nestedCursor->secondaryPointerOrSavedOffset;
        selectedPalette = g_ShotPalette;
      }
      payloadSlot = (registrationRecord->runtimePayload).armyRuntime;
      switch(registrationRecord->domainIndex) {
      case RESOURCE_DOMAIN_ARMY_RUNTIME:
        /* textureSet holds the army graphics binding index until here */
        payloadSlot = (ArmyRuntimeSlot *)
                     ((int)payloadSlot + g_ModelRuntimeRebaseDelta);
        selectedPalette = g_ArmyGraphicsBindings[(int)registrationRecord->textureSet].paletteAsset;
        registrationRecord->textureSet = g_ArmyGraphicsBindings[(int)registrationRecord->textureSet].textureSet;
        registrationRecord->paletteAsset = selectedPalette;
        break;
      case RESOURCE_DOMAIN_SHOT_RUNTIME:
        payloadSlot = (ArmyRuntimeSlot *)
                     (g_ShotRuntimeRebaseBaseMinusOne + (int)payloadSlot);
        registrationRecord->textureSet = g_ShotTextureSet;
        registrationRecord->paletteAsset = selectedPalette;
        break;
      case RESOURCE_DOMAIN_EFFECT_RUNTIME:
        payloadSlot = (ArmyRuntimeSlot *)
                     (g_EffectRuntimeRebaseBaseMinusOne + (int)payloadSlot);
        selectedTextureSet = g_EffectTextureSet;
        selectedPalette = g_EffectPalette;
        /* effects flagged 2 in their model runtime use the army graphics of binding 0 */
        if ((((payloadSlot->modelRuntimeOrSavedOffset).modelRuntime)->effectModelFlags
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
    registrationRecord++;
    remainingRecords = remainingRecords - 1;
  } while (remainingRecords != 0);
  /* the last nested slot of the last record is the saved tail record */
  tailNestedPointer = runtimeImage->records[runtimeImage->recordCount - 1].nestedPointersOrSavedOffsets
           [12].runtimePointer;
  registrationRecord = NULL;
  if (tailNestedPointer != NULL) {
    registrationRecord = (ResourceRegistrationRecord *)(g_RuntimeObjectRebaseBaseMinusOne + (int)tailNestedPointer);
  }
  runtimeImage->tailRecord = registrationRecord;
  (playerRuntimeBlocks->factionAssignment).factionAssignmentIndex = runtimeImage->factionAssignmentIndex;
  return;
}


/* Address: 0x00565E10.
   Periodic timer callback of the in-game session: counts the network tick countdown down to zero and advances the
   periodic clock while no resource registration is in progress.
*/
void __cdecl InGameRuntime_PeriodicCountdownAndClockTick(void)

{
  if (g_InGameNetworkTickCountdown != 0) {
    g_InGameNetworkTickCountdown--;
  }
  if (g_InGameResourceRegistrationBusyCount == 0) {
    g_GameFactionRuntimeImage.tail.periodicClockTick++;
  }
  return;
}

/* Address: 0x00567060.
   Keyboard fallback of the in-game UI root: looks the key up in the hotkey table (key code plus required Ctrl/Alt
   combination) and runs its action: chat, message window, menus, save, pause, game speed, side panel,
   screenshot, leaving the game and the three cheat keys (only while cheats are enabled).
*/
bool InGameHotkeys_DispatchCommandByFlags(UiKeyboardStateMask modifierFlags,UiActionId commandCode,
          InGameRuntimeRootFrameView *inGameRoot)

{
  /* Rewritten from the assembly (0x00567060-0x005678B9). The record table holds continuation
     addresses inside this function; the decompiled version jumped into the original machine code,
     which then called the recovered C functions with the wrong calling convention (crash on ESC
     after loading). Each continuation is translated below; EBX is the runtime root. */
  uint8_t *rt = (uint8_t *)inGameRoot;
  UiCommandDispatchRecord *record = g_EndGameResultsCommandDispatchRecords_00_Code00030071_Modifier30;
  uint32_t target = 0;
  bool localSession = (g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_NETWORKED_MASK) == 0;

  /* A record without modifier class matches only without Ctrl and Alt; otherwise exactly the named
     combination (Ctrl, Alt, or both) must be held. */

  for (;; record++) {
    uint32_t flags = record->modifierClassFlags;
    if (record->commandCode == 0) {
      return false;
    }
    if (record->commandCode != commandCode) {
      continue;
    }
    if (flags == 0) {
      if ((modifierFlags & (KEYBOARD_STATE_CTRL | KEYBOARD_STATE_ALT)) != 0) continue;
    }
    else if ((flags & KEYBOARD_STATE_ALT) == 0) {
      if (((modifierFlags & KEYBOARD_STATE_CTRL) == 0) || ((modifierFlags & KEYBOARD_STATE_ALT) != 0)) continue;
    }
    else if ((flags & KEYBOARD_STATE_CTRL) == 0) {
      if (((modifierFlags & KEYBOARD_STATE_CTRL) != 0) || ((modifierFlags & KEYBOARD_STATE_ALT) == 0)) continue;
    }
    else {
      if (((modifierFlags & KEYBOARD_STATE_CTRL) == 0) || ((modifierFlags & KEYBOARD_STATE_ALT) == 0)) continue;
    }
    target = (uint32_t)record->continuationEntryAddress;
    break;
  }
  switch (target) {
  case 0x5671e0: /* Ctrl+Alt+Z, cheat: toggle fast build and research */
    if ((g_UiCommandRuntimeFlags & UI_COMMAND_RUNTIME_FLAG_CHEATS_ENABLED) != 0) {
      g_UiCommandRuntimeFlags = g_UiCommandRuntimeFlags ^ UI_COMMAND_RUNTIME_FLAG_CHEAT_FAST_BUILD;
    }
    break;
  case 0x567200: /* Ctrl+Alt+X, cheat: +1000 Xenite (xeniteCurrentQ4 += 1000 << Q4_SHIFT) */
    if ((g_UiCommandRuntimeFlags & UI_COMMAND_RUNTIME_FLAG_CHEATS_ENABLED) != 0) {
      g_GameFactionRuntimeImage.records[((WorldRuntimeContext *)INGAME_UI(rt,
           worldView))->activeFactionRuntimeIndex].xeniteCurrentQ4 += 1000 << Q4_SHIFT;
    }
    break;
  case 0x567230: /* Ctrl+Alt+E, cheat: +100 energy supply and capacity (record +0x20 and +0x24, Q4) */
    if ((g_UiCommandRuntimeFlags & UI_COMMAND_RUNTIME_FLAG_CHEATS_ENABLED) != 0) {
      g_GameFactionRuntimeImage.records[((WorldRuntimeContext *)INGAME_UI(rt,
           worldView))->activeFactionRuntimeIndex].baselineEnergySupplyQ4 += 100 << Q4_SHIFT;
      g_GameFactionRuntimeImage.records[((WorldRuntimeContext *)INGAME_UI(rt,
           worldView))->activeFactionRuntimeIndex].energyGenerationCapacityQ4 += 100 << Q4_SHIFT;
    }
    break;
  case 0x567270: /* Enter: open the chat line */
    UiPageStack_SetActiveIndex(1,(UiPageStackControl *)INGAME_UI(rt,chatInputPageStack));
    ((UiTextEditControl *)INGAME_UI(rt,chatInputTextEdit))->cursorIndex = 0;
    ((UiTextEditControl *)INGAME_UI(rt,chatInputTextEdit))->selectionStart = 0;
    ((UiTextEditControl *)INGAME_UI(rt,chatInputTextEdit))->selectionEnd = 0;
    if (!localSession) {
      SelectableGroupNodeResult visible;
      int i;
      for (i = 0; i < 24; i++) {
        ((uint32_t *)((InGameCommandTextEditControlCC *)INGAME_UI(rt,chatInputTextEdit))->textBuffer)[i] = 0;
      }
      visible = UiSelectableGroup_NoneVisibleSelected(3,INGAME_UI(rt,messageRecipientAllTab),
                                                      INGAME_UI(rt,messageRecipientGroupsTab),
                                                    INGAME_UI(rt,messageRecipientPlayersTab));
      (*(void (**)(void *))(uintptr_t)(THANDOR_ADDR(g_InGameUiActionHandlersPage10,0) +
                                       (((UiSelectableControl *)visible.node)->actionId & 0xff) * 4))
                (visible.node);
    }
    UiKeyboardFocus_Set(INGAME_UI(rt,chatInputTextEdit));
    break;
  case 0x567340: /* Alt+F4: game menu on the quit page */
  case 0x5673a0: /* F2: game menu on the save page (local games only) */
  case 0x567410: /* Esc: game menu */
  case 0x567460: { /* F1: mission objectives */
    UiSelectableControl *toggle;
    if ((target == 0x5673a0) && !localSession) {
      break;
    }
    toggle = (UiSelectableControl *)(target == 0x567460 ? INGAME_UI(rt,missionObjectivesButton) :
                                                     INGAME_UI(rt,inGameMenuButton));
    UiSelectableControl_SetSelected(1,toggle);
    if (((toggle->stateFlags & 0x200) != 0) &&
        (((UiSpriteButtonControl *)toggle)->activationSound != NULL)) {
      g_SoundPlayOneShot(g_UiSoundGainQ15,g_UiSoundGainQ15,
                            ((UiSpriteButtonControl *)toggle)->activationSound);
    }
    if (target == 0x567460) {
      InGameMissionHelpPage_Toggle((UiNodeBase *)toggle);
      break;
    }
    InGameSettingsPage_ToggleAndSynchronizeControls(toggle);
    if (target == 0x567340) {
      InGameQuitMenu_OpenAndRefreshButtons((InGameCommandPanelSourceAddress32)INGAME_UI(rt,
           gameMenuQuitButton));
    }
    else if (target == 0x5673a0) {
      InGameSaveGamePage_RebuildCatalog(INGAME_UI(rt,gameMenuSaveButton));
    }
    break;
  }
  case 0x5674b0: { /* C: toggle the message window (network games only) */
    UiPageStackControl *stack;
    uint32_t index;
    SelectableGroupNodeResult visible;
    int i;
    if (localSession) {
      break;
    }
    stack = (UiPageStackControl *)INGAME_UI(rt,gameWindowPageStack);
    index = (UiPageStack_ActivePageNotInList(stack).pageIndex == 1) ? 0 : 1;
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
    for (i = 0; i < 24; i++) {
      ((uint32_t *)((InGameCommandTextEditControlCC *)INGAME_UI(rt,messageTextEdit))->textBuffer)[i] = 0;
    }
    visible = UiSelectableGroup_NoneVisibleSelected(3,INGAME_UI(rt,messageRecipientAllTab),
                                                    INGAME_UI(rt,messageRecipientGroupsTab),
                                                    INGAME_UI(rt,messageRecipientPlayersTab));
    (*(void (**)(void *))(uintptr_t)(THANDOR_ADDR(g_InGameUiActionHandlersPage10,0) +
                                     (((UiSelectableControl *)visible.node)->actionId & 0xff) * 4))
              (visible.node);
    g_KeyboardFlushEvents();
    break;
  }
  case 0x5675e0: /* P: pause */
    if (localSession) {
      InGameCommand_TogglePauseRequest(g_LocalPlayerRuntimeId,0,0,0);
    }
    else {
      InGameCommandQueue_AppendLocalPlayerCommand(INGAME_COMMAND_TOGGLE_PAUSE,0,0,0);
    }
    break;
  case 0x567620: /* G: faster */
  case 0x567660: /* Alt+G: slower */ {
    int step = (target == 0x567620) ? 1 : -1;
    if (localSession) {
      InGameSimulationSpeed_AdjustPlayerAndRecomputeMinimumTicks(g_LocalPlayerRuntimeId,0,0,step);
    }
    else {
      InGameCommandQueue_AppendLocalPlayerCommand(INGAME_COMMAND_ADJUST_GAME_SPEED,0,0,step);
    }
    break;
  }
  case 0x5676a0: { /* Tab: hide or show the side panel; bit 2 of the map/mouse settings remembers it */
    uint32_t settings = PersistentSettings_Read(0,PERSISTENT_SETTING_MAP_MOUSE_OPTION_FLAGS);
    UiPageStackControl *stack = (UiPageStackControl *)INGAME_UI(rt,sidePanelStack);
    if (UiPageStack_ActivePageNotInList(stack).pageIndex != 0) {
      UiPageStack_SetActiveIndex(0,stack);
      UiPageStack_SetActiveIndex(0,(UiPageStackControl *)INGAME_UI(rt,resourceBarModeStack));
      UiPageStack_SetActiveIndex(0,(UiPageStackControl *)INGAME_UI(rt,gamePanelsModeStack));
      INGAME_UI(rt,worldViewArea)->rightOffset = INGAME_UI(rt,sidePanelFrameLeftEdge)->leftOffset;
      settings = settings & ~PERSISTENT_MAP_OPTION_SIDE_PANEL_HIDDEN;
    }
    else {
      UiPageStack_SetActiveIndex(1,stack);
      UiPageStack_SetActiveIndex(0,(UiPageStackControl *)INGAME_UI(rt,resourceBarModeStack));
      UiPageStack_SetActiveIndex(0,(UiPageStackControl *)INGAME_UI(rt,gamePanelsModeStack));
      INGAME_UI(rt,worldViewArea)->rightOffset = 0;
      settings = settings | PERSISTENT_MAP_OPTION_SIDE_PANEL_HIDDEN;
    }
    UiContainer_LayoutChildren((UiNodeBase *)rt);
    PersistentSettings_Write(settings,PERSISTENT_SETTING_MAP_MOUSE_OPTION_FLAGS);
    break;
  }
  case 0x5677e0: { /* Alt+P: screenshot to the next numbered PCX file */
    FramebufferCaptureResult capture =
         g_GraphicsFramebufferCaptureRegion(g_FramebufferHeight,g_FramebufferWidth,0,0);
    PcxEncodeResult pcx;
    uint16_t *digitHigh = (uint16_t *)(uintptr_t)THANDOR_ADDR(g_ScreenshotFileNameUtf16,12);
    uint16_t *digitLow = (uint16_t *)(uintptr_t)THANDOR_ADDR(g_ScreenshotFileNameUtf16,14);
    if (capture.failed) {
      break;
    }
    pcx = g_PcxFunctionExport3(g_PcxFunctionModule,capture.capture);
    if (pcx.failed) {
      g_MemoryApi.free(capture.capture);
      break;
    }
    FileSystem_WriteBufferToPath(pcx.encodedByteCount,pcx.encodedBytesOrError,
                                   (uint16_t *)(uintptr_t)THANDOR_ADDR(g_ScreenshotFileNameUtf16,0));
    g_MemoryApi.free(pcx.encodedBytesOrError);
    g_MemoryApi.free(capture.capture);
    /* advance the two-digit number in the file name */
    *digitLow = *digitLow + 1;
    if (*digitLow > '9') {
      *digitHigh = *digitHigh + 1;
      *digitLow = *digitLow - 10;
      if (*digitHigh > '9') {
        *digitHigh = *digitHigh - 10;
      }
    }
    break;
  }
  case 0x567870: /* Alt+Q: leave the game (not as host) */
    if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_HOST) != 0) {
      break;
    }
    if (localSession) {
      InGameCommand_HandlePlayerDeparture(g_LocalPlayerRuntimeId,0,0,0);
    }
    else {
      InGameCommandQueue_AppendLocalPlayerCommand(INGAME_COMMAND_PLAYER_DEPARTURE,0,0,0);
    }
    break;
  default:
    Thandor_Log("EndGameResults dispatch: unhandled continuation %08x",target);
    break;
  }
  return false;
}


/* Address: 0x00569920.
   Periodic timer that plays the queued in-game notification movies: while one plays it advances a frame and, at
   the end, closes it and keeps the notification's map target clickable for 0x280 more ticks; otherwise it starts
   the movie of the queue head ("flm\movie%03d.flm"), makes its payload the active notification and pops the
   four-entry queue.
*/
void InGameRuntime_ProcessQueuedSessionNotificationTimer(void)

{
  uint32_t notificationMovieNumber;
  GraphicsTextureSourceAsset *panelTextureSource;
  int remainingWords;
  uint32_t *sourceWord;
  uint32_t *destinationWord;
  MovieFrameResult frameResult;
  MovieOpenResult openResult;
  InGameRuntimeRoot *inGameRoot;
  
  panelTextureSource = g_InGamePanelTextureSource;
  inGameRoot = g_InGameRuntimeRoot;
  if (((g_InGameSessionNotificationTimeoutTicks != 0) &&
      (g_InGameSessionNotificationTimeoutTicks = g_InGameSessionNotificationTimeoutTicks - 1,
      g_InGameSessionNotificationTimeoutTicks == 0)) &&
     (g_InGameRuntimeRoot->notificationButtonCursorFrame == PAYLOAD_ACTIVE)) {
    g_InGameRuntimeRoot->notificationButtonCursorFrame = NOTIFICATION_INTERACTION_NONE;
  }
  /* notificationButtonTextureSource holds the playing movie, or the panel texture source when none plays */
  if (panelTextureSource == (GraphicsTextureSourceAsset *)inGameRoot->notificationButtonTextureSource) {
    if (inGameRoot->notificationQueue[0].priority != 0) {
      notificationMovieNumber = inGameRoot->notificationQueue[0].movieId;
      g_WideNumberFormatUtf16
                (WIDE_FORMAT_PAD_WITH_ZERO,0,3,1,notificationMovieNumber,(uint16_t *)(u_flm_movie000_flm_0056314e + 9));
      openResult = Movie_Open(MOVIE_OPEN_PACKAGE_ONLY,(uint16_t *)u_flm_movie000_flm_0056314e);
      if (!openResult.failed) {
        /* movies 100-299 and 700-899 play at the alternate movie gain */
        if ((99 < notificationMovieNumber) &&
            ((notificationMovieNumber < 300 || ((699 < notificationMovieNumber && (notificationMovieNumber < 900)))))) {
          Movie_SetAudioGainQ15(g_MovieAlternateAudioGainQ15);
        }
        frameResult = Movie_AdvanceFrame();
        if (!frameResult.ended) {
          inGameRoot->notificationButtonTextureSource = frameResult.movieOrError;
          inGameRoot->notificationButtonSubresource = 0;
          /* copy the 0x18-byte payload of the queue head into the active notification */
          sourceWord = (uint32_t *)&inGameRoot->notificationQueue[0].payload;
          destinationWord = (uint32_t *)&inGameRoot->activeNotificationPayload;
          for (remainingWords = 6; remainingWords != 0; remainingWords--) {
            *destinationWord = *sourceWord;
            sourceWord++;
            destinationWord++;
          }
          if (inGameRoot->notificationButtonCursorFrame == PAYLOAD_ACTIVE) {
            inGameRoot->notificationButtonCursorFrame = NOTIFICATION_INTERACTION_NONE;
          }
          g_InGameSessionNotificationTimeoutTicks = 0;
          if ((inGameRoot->activeNotificationPayload).payloadKind != NOTIFICATION_PAYLOAD_NONE) {
            inGameRoot->notificationButtonCursorFrame = PAYLOAD_ACTIVE;
          }
        }
      }
      /* pop the queue head: move entries 1-3 (0x18 dwords) forward and clear the last 0x20-byte entry */
      sourceWord = (uint32_t *)(inGameRoot->notificationQueue + 1);
      destinationWord = (uint32_t *)inGameRoot->notificationQueue;
      for (remainingWords = 24; remainingWords != 0; remainingWords--) {
        *destinationWord = *sourceWord;
        sourceWord++;
        destinationWord++;
      }
      for (remainingWords = 8; remainingWords != 0; remainingWords--) {
        *destinationWord = 0;
        destinationWord++;
      }
    }
  }
  else {
    frameResult = Movie_AdvanceFrame();
    if (frameResult.ended) {
      Movie_Close();
      g_InGameSessionNotificationTimeoutTicks = 640;
      inGameRoot->notificationButtonTextureSource = (uint32_t)panelTextureSource;
      inGameRoot->notificationButtonSubresource = 37;
    }
  }
  return;
}


/* Address: 0x005641D0.
   Starts a new game on a level: resets the session counters and the per-player blocks, installs the step timer
   and InGameRuntime_UpdateSimulationAndNetworkTick as the UI synchronization hook, builds the in-game UI root from
   its template, opens the level movie that plays while loading and loads the level (world, terrain, shading,
   technologies, units). It then reports itself ready to the other players and keeps drawing the player-status
   screen while stepping until every player is ready, and finally queues the level's five intro notifications.
   CF set (failed) returns the error of the failing step.
*/
NewSessionInitResult InGameRuntime_InitializeNewSession(LevelAssetRuntimePrefix *levelAsset,
                                                        uint16_t *levelMoviePath)

{
  WorldRuntimeFlags *worldRuntimeFlags;
  WorldRuntimeContext *world;
  uint16_t titleChar;
  UiTextResourceId titleTextIndex;
  PlayerRuntimeId localPlayerId;
  uint16_t *titleSource;
  InGameRuntimeRoot *inGameRoot;
  uint32_t settingOrFactionToken;
  uint32_t gridHalfSize;
  uint32_t subresourceCount;
  int countOrPlayerId;
  FrontendPlayerRuntimeBlockCount remainingPlayers;
  InGameNotificationMovieId notificationMovieId;
  InGameNotificationMovieId unusedNotificationMovieId;
  SelectionPlayerRuntimeBlock *selectionBlock;
  FrontendPlayerNameUtf16 *playerNameSource;
  uint32_t *templateCursor;
  FrontendPlayerRemovalPacket10007 *packetCursor;
  SelectionPlayerRuntimeBlock *selectionBlockCursor;
  FrontendPlayerRuntimeRecord *frontendPlayer;
  uint8_t *playerNameDestination;
  uint16_t *sessionNameClearCursor;
  uint16_t *sessionNameCursor;
  InGameRuntimeRoot *rootCursorOrError;
  InGameNotificationQueueRecord *queueRecord;
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
  /* clear the 0x280-byte player-removal packet and all selection blocks (0x10230 dwords) */
  packetCursor = &g_FrontendClientPlayerRemovalPacket10007;
  for (countOrPlayerId = 160; countOrPlayerId != 0; countOrPlayerId--) {
    (packetCursor->header).packedTypeAndUnitCount = 0;
    packetCursor = (FrontendPlayerRemovalPacket10007 *)&(packetCursor->header).sequenceToken;
  }
  selectionBlockCursor = g_SelectionPlayerBlocks;
  for (countOrPlayerId = 0x10230; remainingPlayers = g_FrontendPlayerRuntimeBlockCount,
       selectionBlock = g_SelectionPlayerBlocks,
      frontendPlayer = g_FrontendPlayerRuntimeBlocks, countOrPlayerId != 0; countOrPlayerId--) {
    (selectionBlockCursor->selection).entries[0] = NULL;
    selectionBlockCursor = (SelectionPlayerRuntimeBlock *)((selectionBlockCursor->selection).entries + 1);
  }
  /* per player: not ready, command sync pending, fresh timeout; link its selection block and copy the name */
  do {
    countOrPlayerId = frontendPlayer->playerRuntimeId;
    (frontendPlayer->factionAssignment).readyOrWaitState = 0;
    frontendPlayer->commandSyncPending = FRONTEND_COMMAND_SYNC_PENDING;
    frontendPlayer->heartbeatExpiryTicks = 1024;
    settingOrFactionToken = (frontendPlayer->factionAssignment).factionAssignmentIndex;
    g_SelectionPlayerRuntimeBlockPointers[countOrPlayerId] = selectionBlock;
    selectionBlock->factionIndex = settingOrFactionToken;
    selectionBlock->simulationStepTicks = 1;
    playerNameSource = &frontendPlayer->playerName;
    playerNameDestination = (uint8_t *)selectionBlock->playerNameUtf16;
    for (countOrPlayerId = 20; countOrPlayerId != 0; countOrPlayerId--) {
      *(uint32_t *)playerNameDestination = *(uint32_t *)playerNameSource->textUtf16;
      playerNameSource = (FrontendPlayerNameUtf16 *)(playerNameSource->textUtf16 + 2);
      playerNameDestination = playerNameDestination + 4;
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
            (InGameRuntime_UpdateSimulationAndNetworkTick,&g_InGameStateTickSpinLock);
  /* session name = the level title without the characters Windows forbids in file names (at most 31);
     the first title character is skipped */
  titleTextIndex = (levelAsset->header).titleTextResourceIndex;
  sessionNameClearCursor = &g_InGameSessionNameScratchUtf16;
  for (countOrPlayerId = 32; countOrPlayerId != 0; countOrPlayerId--) {
    *sessionNameClearCursor = 0;
    sessionNameClearCursor++;
  }
  resolvedTitle = TextResource_Resolve(titleTextIndex + TEXT_ID_LEVEL_TITLE_BASE);
  titleSource = resolvedTitle.text;
  sessionNameCursor = &g_InGameSessionNameScratchUtf16;
  countOrPlayerId = 31;
  do {
    titleSource++;
    titleChar = *titleSource;
    if (titleChar == 0) break;
    if ((((((titleChar != '*') && (titleChar != '<')) && (titleChar != '>')) &&
         ((titleChar != '"' && (titleChar != '/')))) &&
        ((titleChar != '\\' && ((titleChar != '.' && (titleChar != '?')))))) &&
       ((titleChar != ':' && (titleChar != '|')))) {
      *sessionNameCursor = titleChar;
      sessionNameCursor++;
    }
    countOrPlayerId--;
  } while (countOrPlayerId != 0);
  /* 4 MB pool for the world objects (INGAME_WORLD_OBJECT_RECORD_COUNT records), zeroed */
  allocation = g_MemoryApi.alloc(INGAME_WORLD_OBJECT_RECORD_COUNT * sizeof(WorldObjectRecord));
  rootCursorOrError = (InGameRuntimeRoot *)allocation.payloadOrError;
  if (!allocation.failed) {
    g_RuntimeObjectRebaseBaseMinusOne = rootCursorOrError[-1].reservedA06C_C3E3 + 0x2377;
    g_InGameWorldObjectRecords = (WorldObjectRecord *)rootCursorOrError;
    for (countOrPlayerId = INGAME_WORLD_OBJECT_RECORD_COUNT * sizeof(WorldObjectRecord) / 4; countOrPlayerId != 0; countOrPlayerId--) {
      (rootCursorOrError->rootUi).base.nextSibling = NULL;
      rootCursorOrError = (InGameRuntimeRoot *)&(rootCursorOrError->rootUi).base.firstChild;
    }
    statusResult = SelectionInfoPanel_InitResources
                       ((SelectionInfoEntitySlots *)
                        g_SelectionPlayerRuntimeBlockPointers[g_LocalPlayerRuntimeId]);
    rootCursorOrError = (InGameRuntimeRoot *)statusResult.valueOrError;
    if (!statusResult.failed) {
      allocation = g_MemoryApi.alloc(sizeof(InGameRuntimeRoot));
      inGameRoot = (InGameRuntimeRoot *)allocation.payloadOrError;
      rootCursorOrError = inGameRoot;
      if (!allocation.failed) {
        templateCursor = (uint32_t *)&g_InGameRuntimeDefaultImageTemplate;
        g_InGameRuntimeRoot = inGameRoot;
        /* copy the in-game root template (sizeof(InGameRuntimeRoot) / 4 dwords) */
        for (countOrPlayerId = sizeof(InGameRuntimeRoot) / 4; countOrPlayerId != 0; countOrPlayerId--) {
          (rootCursorOrError->rootUi).base.nextSibling = (UiNodeBase *)*templateCursor;
          templateCursor++;
          rootCursorOrError = (InGameRuntimeRoot *)&(rootCursorOrError->rootUi).base.firstChild;
        }
        world = &inGameRoot->worldRuntime;
        statusResult = InGameUiRuntime_InitializeControlTreeResources((UiRootNode *)inGameRoot);
        rootCursorOrError = (InGameRuntimeRoot *)statusResult.valueOrError;
        if (!statusResult.failed) {
          /* world input and command callbacks, camera limits, and the step hook for the world runtime */
          inGameRoot->worldOverlayCallback =
               InGameWorldOverlay_RebuildOrReleaseTransientMarkers;
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
          (inGameRoot->worldRuntime).fieldRegion.clearTransientStateCallback =
               InGameUiRuntime_ResetNotificationButtonCursor;
          (inGameRoot->worldRuntime).selection.dispatchWorldContextActionCallback =
               InGameUiRuntime_DispatchWorldContextActionCallback;
          (inGameRoot->worldRuntime).minimumCameraDistanceQ12 = 8 * Q12_ONE;
          (inGameRoot->worldRuntime).maximumCameraDistanceQ12 = 19 * Q12_ONE;
          (inGameRoot->worldRuntime).motion.minimumPitchAngle = 0xffffc400;
          (inGameRoot->worldRuntime).motion.maximumPitchAngle = 0xffffe800;
          (inGameRoot->worldRuntime).tickSpinLock = &g_InGameStateTickSpinLock;
          (inGameRoot->worldRuntime).simulationAndNetworkTickCallback =
               InGameRuntime_UpdateSimulationAndNetworkTick;
          selectionBlockCursor = g_SelectionPlayerRuntimeBlockPointers[g_LocalPlayerRuntimeId];
          inGameRoot->localPlayerMarkedCellCount = 0;
          inGameRoot->localPlayerMarkedCells = selectionBlockCursor->markedCells;
          UiRootStack_Push(&g_UiRootCallbacks_0054FBC0,(UiRootNode *)inGameRoot);
          endingMoviePath = LevelAsset_PrepareEndingMoviePath(levelMoviePath,&levelAsset->header);
          rootCursorOrError = (InGameRuntimeRoot *)endingMoviePath.moviePath;
          if (!endingMoviePath.failed) {
            movieOpen = Movie_Open(MOVIE_OPEN_PACKAGE_ONLY,(uint16_t *)rootCursorOrError);
            rootCursorOrError = (InGameRuntimeRoot *)movieOpen.frameCountOrError;
            if (!movieOpen.failed) {
              firstFrame = Movie_AdvanceFrame();
              rootCursorOrError = (InGameRuntimeRoot *)firstFrame.movieOrError;
              if (!firstFrame.ended) {
                inGameRoot->levelMovieRuntime = (MovieRuntime *)rootCursorOrError;
                g_MoviePlaybackBaseFrameGroup = 0;
                g_MoviePlaybackScheduleCounter = 0;
                g_MoviePlaybackScheduleSpan = 0;
                g_MoviePlaybackCurrentFrame = 0;
                MoviePlayback_AdvanceToFrameAndPresent(0);
                MoviePlayback_AdvanceToFrameAndPresent(1);
                RecentTextHistory_SortAndBuildPointerList(8,&inGameRoot->recentTextHistory);
                WorldRuntime_AttachObjectArray(INGAME_WORLD_OBJECT_RECORD_COUNT,g_InGameWorldObjectRecords,world);
                localPlayerId = g_LocalPlayerRuntimeId;
                settingOrFactionToken = g_SelectionPlayerRuntimeBlockPointers[g_LocalPlayerRuntimeId]->
                        factionIndex;
                levelAsset->playerSlots[6].aiClassOrMode = settingOrFactionToken;
                (inGameRoot->worldRuntime).activeFactionRuntimeIndex = settingOrFactionToken;
                (inGameRoot->worldRuntime).selection.activePlayerRuntimeId = localPlayerId;
                WorldRuntime_AttachAndClearDwordArray
                          (INGAME_WORLD_DWORD_ARRAY_COUNT,(uint32_t *)&g_InGameWorldRuntimeDwordArray256,world);
                statusResult = GameData_ResetDefaults();
                rootCursorOrError = (InGameRuntimeRoot *)statusResult.valueOrError;
                if (!statusResult.failed) {
                  levelLoad = InGameLevelRuntime_LoadResourcesAfterDefaultReset(levelAsset,world);
                  rootCursorOrError = (InGameRuntimeRoot *)levelLoad.errorOrValue;
                  if (!levelLoad.failed) {
                    queueRecord = inGameRoot->notificationQueue;
                    for (countOrPlayerId = 32; countOrPlayerId != 0; countOrPlayerId--) {
                      queueRecord->movieId = 0;
                      queueRecord = (InGameNotificationQueueRecord *)&queueRecord->priority;
                    }
                    statusResult = TerrainCompositeTexture_Create();
                    rootCursorOrError = (InGameRuntimeRoot *)statusResult.valueOrError;
                    if (!statusResult.failed) {
                      /* hold the step off while the world is finished */
                      g_SpinLockAcquire(&g_InGameStateTickSpinLock);
                      g_InGameSimulationStepTicks = 1;
                      settingOrFactionToken =
                           PersistentSettings_Read(PERSISTENT_DEFAULT_SHADING_TEXTURE_DIMENSION,PERSISTENT_SETTING_SHADING_TEXTURE_DIMENSION);
                      gridHalfSize = PersistentSettings_Read(PERSISTENT_DEFAULT_SHADING_GRID_HALF_SIZE,PERSISTENT_SETTING_SHADING_GRID_HALF_SIZE);
                      subresourceCount =
                           PersistentSettings_Read(PERSISTENT_DEFAULT_SHADING_SUBRESOURCE_COUNT,PERSISTENT_SETTING_SHADING_SUBRESOURCE_COUNT);
                      GraphicsShadingRuntime_InitializeGeneratedTexture
                                (subresourceCount,gridHalfSize,settingOrFactionToken);
                      /* mirror the shading and mouse/panel options into the world runtime flags */
                      settingOrFactionToken = PersistentSettings_Read(1,PERSISTENT_SETTING_SHADING_ENABLED);
                      if (settingOrFactionToken == 0) {
                        worldRuntimeFlags = &(inGameRoot->worldRuntime).runtimeFlags;
                        *worldRuntimeFlags = *worldRuntimeFlags & ~WORLD_RUNTIME_FLAG_SHADING_ENABLED;
                      }
                      else {
                        worldRuntimeFlags = &(inGameRoot->worldRuntime).runtimeFlags;
                        *worldRuntimeFlags = *worldRuntimeFlags | WORLD_RUNTIME_FLAG_SHADING_ENABLED;
                      }
                      settingOrFactionToken =
                           PersistentSettings_Read(0,PERSISTENT_SETTING_MOUSE_LINK_PANEL_OPTION_FLAGS);
                      if ((settingOrFactionToken & PERSISTENT_LINK_OPTION_ROTATION_ZOOM) == 0) {
                        worldRuntimeFlags = &(inGameRoot->worldRuntime).runtimeFlags;
                        *worldRuntimeFlags = *worldRuntimeFlags & ~WORLD_RUNTIME_FLAG_LINK_ROTATION_ZOOM;
                      }
                      else {
                        worldRuntimeFlags = &(inGameRoot->worldRuntime).runtimeFlags;
                        *worldRuntimeFlags = *worldRuntimeFlags | WORLD_RUNTIME_FLAG_LINK_ROTATION_ZOOM;
                      }
                      if ((settingOrFactionToken & PERSISTENT_LINK_OPTION_ROTATION_TILT) == 0) {
                        worldRuntimeFlags = &(inGameRoot->worldRuntime).runtimeFlags;
                        *worldRuntimeFlags = *worldRuntimeFlags & ~WORLD_RUNTIME_FLAG_LINK_ROTATION_TILT;
                      }
                      else {
                        worldRuntimeFlags = &(inGameRoot->worldRuntime).runtimeFlags;
                        *worldRuntimeFlags = *worldRuntimeFlags | WORLD_RUNTIME_FLAG_LINK_ROTATION_TILT;
                      }
                      if ((settingOrFactionToken & PERSISTENT_LINK_OPTION_HIDE_PANEL) == 0) {
                        worldRuntimeFlags = &(inGameRoot->worldRuntime).runtimeFlags;
                        *worldRuntimeFlags = *worldRuntimeFlags & ~WORLD_RUNTIME_FLAG_HIDE_PANEL;
                      }
                      else {
                        worldRuntimeFlags = &(inGameRoot->worldRuntime).runtimeFlags;
                        *worldRuntimeFlags = *worldRuntimeFlags | WORLD_RUNTIME_FLAG_HIDE_PANEL;
                      }
                      rootCursorOrError = (InGameRuntimeRoot *)
                                     PersistentSettings_Read(0,PERSISTENT_SETTING_MAP_MOUSE_OPTION_FLAGS);
                      if (((uint32_t)rootCursorOrError & 4) != 0) {
                        UiPageStack_SetActiveIndex(1,&inGameRoot->sidePanelPageStack);
                        rootCursorOrError = (InGameRuntimeRoot *)
                                       &inGameRoot->resourceBarModePageStack;
                        UiPageStack_SetActiveIndex(0,(UiPageStackControl *)rootCursorOrError);
                        UiPageStack_SetActiveIndex(0,&inGameRoot->gamePanelsModePageStack);
                        inGameRoot->worldViewAreaRightOffset = 0;
                        UiContainer_LayoutChildren((UiNodeBase *)inGameRoot);
                      }
                      subsystemFailed = (bool)InGameRuntime_InitializeOptionalSubsystemAlwaysSuccess
                                               ((uint32_t)(inGameRoot->worldRuntime).fieldGrid);
                      if (!subsystemFailed) {
                        /* a campaign carries units over from the previous level */
                        if (g_FrontendLoadedCampaignAsset == 0) {
                          OldUnitRuntime_ResetPendingTables();
                        }
                        else {
#ifdef THANDOR_TEST_AIDS
                          Thandor_Log("level start: campaign carries over %u units from the previous level",
                                      (unsigned)g_OldUnitRecordCount);
#endif
                          OldUnitRuntime_MergeMasksAndReplayRecords();
#ifdef THANDOR_TEST_AIDS
                          if (g_InGameRuntimeRoot != NULL && g_OldUnitRecordCount != 0) {
                            /* where the carried units of the local faction landed, against the level's start view */
                            WorldRuntimeContext *world = &g_InGameRuntimeRoot->worldRuntime;
                            unsigned perFaction[8] = {0}, record, shown = 0;
                            unsigned localFaction = world->activeFactionRuntimeIndex;
                            for (record = 0; record < (unsigned)g_OldUnitRecordCount && record < 0x200; record++) {
                              perFaction[g_OldUnitPrimaryTable[record * 8 + 1] & 7]++;
                            }
                            Thandor_Log("level start: view target (%d,%d), local faction %u, carried per faction "
                                        "%u %u %u %u %u %u %u %u", world->motion.targetPositionXQ12,
                                        world->motion.targetPositionYQ12, localFaction, perFaction[0], perFaction[1],
                                        perFaction[2], perFaction[3], perFaction[4], perFaction[5], perFaction[6],
                                        perFaction[7]);
                            for (record = 0; record < (unsigned)g_OldUnitRecordCount && record < 0x200 && shown < 4;
                                 record++) {
                              if (g_OldUnitPrimaryTable[record * 8 + 1] == localFaction) {
                                shown++;
                                Thandor_Log("level start: local carried unit, army asset %u at (%d,%d)",
                                            (unsigned)g_OldUnitPrimaryTable[record * 8],
                                            (int)g_OldUnitPrimaryTable[record * 8 + 2],
                                            (int)g_OldUnitPrimaryTable[record * 8 + 3]);
                              }
                            }
                          }
#endif
                        }
                        gridScratch = GridScratch_AllocateForFieldGrid
                                           ((inGameRoot->worldRuntime).fieldGrid);
                        rootCursorOrError = (InGameRuntimeRoot *)gridScratch.valueOrError;
                        if (!gridScratch.failed) {
                          GridScratch_RebuildTerrainAndRuntimeClassificationMasks(world);
                          GridInfluence_ClearDistanceBandsAndRefreshEntities
                                    ((inGameRoot->worldRuntime).ownerListHead);
                          TechnologyRuntime_RebuildDerivedLimitsAndCategoryMasks();
                          if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_NETWORKED_MASK) ==
                              SESSION_NETWORK_ROLE_LOCAL) {
                            inGameRoot->worldViewWrappedTextNodeFlags =
                                 inGameRoot->worldViewWrappedTextNodeFlags | 8;
                          }
                          else {
                            inGameRoot->worldViewWrappedTextNodeFlags =
                                 inGameRoot->worldViewWrappedTextNodeFlags & ~8u;
                          }
                          InGameBuildCatalog_RebuildGrid((UiNodeBase *)inGameRoot);
                          InGameSpecialBuildCatalog_RebuildGrid((UiNodeBase *)inGameRoot);
                          InGameArmyStock_RebuildGrid((UiNodeBase *)inGameRoot);
                          InGameOtherPlayerCommand_RebuildTargetEntries((UiNodeBase *)inGameRoot);
                          WorldLightingRuntime_UpdateInterpolatedTerrainLighting();
                          /* report this player as loaded */
                          if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_NETWORKED_MASK) ==
                              SESSION_NETWORK_ROLE_LOCAL) {
                            FrontendPlayerRuntime_IncrementReadyCountAndResolveConsensus
                                      (g_LocalPlayerRuntimeId,0,0,0);
                          }
                          else {
                            InGameCommandQueue_AppendLocalPlayerCommand(INGAME_COMMAND_PLAYER_READY,0,0,0);
                          }
                          g_SpinLockRelease(&g_InGameStateTickSpinLock);
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
                          g_GraphicsCursorSetFrame(GRAPHICS_CURSOR_FRAME_ARROW);
                          /* Lost load: the original reads the first of five level intro
                             notification movies from the level image. */
                          notificationMovieId =
                               g_InGameLevelRuntimeGlobalBlock.conditionStorage->levelImage.worldSettings.
                               introNotificationMovieId;
                          g_TimerRegisterPeriodic
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
                          return THANDOR_BITCAST(uint64_t, NewSessionInitResult,
                               (THANDOR_BITCAST(ArenaAllocResult, uint64_t,
                                                 allocation) & UINT32_MAX));
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
   Continues a saved game: mounts the save package, takes the session name from its header, loads the campaign
   and level entries, and then follows the same steps as InGameRuntime_InitializeNewSession, except that the local
   player is always player 0 of a single block, the world comes from the saved external tables and field grid
   (InGameLevelRuntime_LoadResourcesAfterExternalTables) instead of a fresh level, and no intro notifications are
   queued. The package and the level entry are released again at the end. CF set (failed) returns the error of the
   failing step.
*/
LoadedSessionInitResult InGameRuntime_InitializeLoadedSession(uint16_t *savePackagePath)

{
  WorldRuntimeFlags *worldRuntimeFlags;
  WorldRuntimeContext *world;
  SelectionPlayerRuntimeBlock *entitySlots;
  InGameRuntimeRoot *mountResult;
  FrontendLoadedLevelAsset *levelImage;
  InGameRuntimeRoot *inGameRoot;
  uint32_t settingOrFactionToken;
  uint32_t gridHalfSize;
  uint32_t subresourceCount;
  InGameRuntimeRoot *rootCursorOrError;
  int remainingCount;
  uint8_t *saveNameSource;
  uint32_t *templateCursor;
  uint16_t *sessionNameClearCursor;
  uint8_t *scanCursor;
  uint8_t *scanEnd;
  uint32_t copyCount;
  short *sessionNameCursor;
  SelectionPlayerRuntimeBlock *selectionBlockCursor;
  InGameNotificationQueueRecord *queueRecord;
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
  InGameRuntimeRoot *mountedPackage;
  FrontendLoadedLevelAsset *loadedLevelAsset;
  
  g_TextureDownsampleShift = PersistentSettings_Read(0,PERSISTENT_SETTING_TEXTURE_QUALITY);
  mountedPackage = NULL;
  loadedLevelAsset = NULL;
  statusResult = Package_Mount(savePackagePath);
  saveNameSource = g_PackageScratchBuffer;
  mountResult = (InGameRuntimeRoot *)statusResult.valueOrError;
  rootCursorOrError = mountResult;
  if (!statusResult.failed) {
    sessionNameClearCursor = &g_InGameSessionNameScratchUtf16;
    for (remainingCount = 32; remainingCount != 0; remainingCount--) {
      *sessionNameClearCursor = 0;
      sessionNameClearCursor++;
    }
    /* the session name is the UTF-16 string at offset 256 of the package header */
    g_FileSystemSeek(FILESYSTEM_SEEK_BEGIN,0,mountResult);
    g_FileSystemReadExact(PCK_ENTRY_HEADER_BYTES,saveNameSource,mountResult);
    saveNameSource = saveNameSource + 256;
    terminatorOrFailure = true;
    remainingCount = 36;
    scanCursor = saveNameSource;
    do {
      scanEnd = scanCursor;
      if (remainingCount == 0) break;
      remainingCount--;
      scanEnd = scanCursor + 2;
      terminatorOrFailure = *(short *)scanCursor == 0;
      scanCursor = scanEnd;
    } while (!terminatorOrFailure);
    if (terminatorOrFailure) {
      /* scanEnd is just past the terminator: bytes -10..-3 are the four characters before the terminator (the
         file extension), which are cut off */
      scanEnd[-6] = 0;
      scanEnd[-5] = 0;
      scanEnd[-4] = 0;
      scanEnd[-3] = 0;
      scanEnd[-10] = 0;
      scanEnd[-9] = 0;
      scanEnd[-8] = 0;
      scanEnd[-7] = 0;
      copyCount = (uint32_t)((int)scanEnd - (int)saveNameSource) >> 1;
      sessionNameCursor = &g_InGameSessionNameScratchUtf16;
      if (31 < copyCount) {
        copyCount = 31;
      }
      for (; copyCount != 0; copyCount--) {
        *sessionNameCursor = *(short *)saveNameSource;
        saveNameSource = saveNameSource + 2;
        sessionNameCursor++;
      }
    }
    packageEntry = Package_LoadEntry((uint16_t *)u_campagne_hex_0050e068);
    if (!packageEntry.failed) {
      g_FrontendLoadedCampaignAsset = packageEntry.bufferOrError;
    }
    packageEntry = Package_LoadEntry((uint16_t *)u_level_hex_0050e040);
    levelImage = packageEntry.bufferOrError;
    rootCursorOrError = (InGameRuntimeRoot *)levelImage;
    mountedPackage = mountResult;
    if (!packageEntry.failed) {
      /* only one selection block is set up: local player 0 with the saved faction */
      settingOrFactionToken = levelImage->playerSlots[6].aiClassOrMode;
      selectionBlockCursor = g_SelectionPlayerBlocks;
      for (remainingCount = 0x10230; entitySlots = g_SelectionPlayerBlocks, remainingCount != 0; remainingCount--) {
        (selectionBlockCursor->selection).entries[0] = NULL;
        selectionBlockCursor = (SelectionPlayerRuntimeBlock *)((selectionBlockCursor->selection).entries + 1);
      }
      g_EndMovieSelectionIndex = UINT32_MAX;
      g_EndMovieVariantIndex = 0;
      g_EndMoviePath = NULL;
      g_LocalPlayerRuntimeId = 0;
      g_SelectionPlayerRuntimeBlockPointers[0] = g_SelectionPlayerBlocks;
      g_SelectionPlayerBlocks->factionIndex = settingOrFactionToken;
      entitySlots->simulationStepTicks = 1;
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
                (InGameRuntime_UpdateSimulationAndNetworkTick,&g_InGameStateTickSpinLock);
      /* 4 MB pool for the world objects (0x4000 records of 0x100 bytes), zeroed */
      allocation = g_MemoryApi.alloc(INGAME_WORLD_OBJECT_RECORD_COUNT * sizeof(WorldObjectRecord));
      rootCursorOrError = (InGameRuntimeRoot *)allocation.payloadOrError;
      loadedLevelAsset = levelImage;
      if (!allocation.failed) {
        g_RuntimeObjectRebaseBaseMinusOne = rootCursorOrError[-1].reservedA06C_C3E3 + 0x2377;
        g_InGameWorldObjectRecords = (WorldObjectRecord *)rootCursorOrError;
        for (remainingCount = INGAME_WORLD_OBJECT_RECORD_COUNT * sizeof(WorldObjectRecord) / 4; remainingCount != 0; remainingCount--) {
          (rootCursorOrError->rootUi).base.nextSibling = NULL;
          rootCursorOrError = (InGameRuntimeRoot *)&(rootCursorOrError->rootUi).base.firstChild;
        }
        statusResult = SelectionInfoPanel_InitResources((SelectionInfoEntitySlots *)entitySlots);
        rootCursorOrError = (InGameRuntimeRoot *)statusResult.valueOrError;
        if (!statusResult.failed) {
          allocation = g_MemoryApi.alloc(sizeof(InGameRuntimeRoot));
          inGameRoot = (InGameRuntimeRoot *)allocation.payloadOrError;
          rootCursorOrError = inGameRoot;
          if (!allocation.failed) {
            templateCursor = (uint32_t *)&g_InGameRuntimeDefaultImageTemplate;
            g_InGameRuntimeRoot = inGameRoot;
            /* copy the in-game root template (0x30F9 dwords = 0xC3E4 bytes) */
            for (remainingCount = sizeof(InGameRuntimeRoot) / 4; remainingCount != 0; remainingCount--) {
              (rootCursorOrError->rootUi).base.nextSibling = (UiNodeBase *)*templateCursor;
              templateCursor++;
              rootCursorOrError = (InGameRuntimeRoot *)&(rootCursorOrError->rootUi).base.firstChild;
            }
            world = &inGameRoot->worldRuntime;
            statusResult = InGameUiRuntime_InitializeControlTreeResources((UiRootNode *)inGameRoot);
            rootCursorOrError = (InGameRuntimeRoot *)statusResult.valueOrError;
            if (!statusResult.failed) {
              inGameRoot->worldOverlayCallback =
                   InGameWorldOverlay_RebuildOrReleaseTransientMarkers;
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
              (inGameRoot->worldRuntime).fieldRegion.clearTransientStateCallback =
                   InGameUiRuntime_ResetNotificationButtonCursor;
              (inGameRoot->worldRuntime).selection.dispatchWorldContextActionCallback =
                   InGameUiRuntime_DispatchWorldContextActionCallback;
              (inGameRoot->worldRuntime).minimumCameraDistanceQ12 = 8 * Q12_ONE;
              (inGameRoot->worldRuntime).maximumCameraDistanceQ12 = 19 * Q12_ONE;
              (inGameRoot->worldRuntime).motion.minimumPitchAngle = 0xffffc400;
              (inGameRoot->worldRuntime).motion.maximumPitchAngle = 0xffffe800;
              (inGameRoot->worldRuntime).tickSpinLock = &g_InGameStateTickSpinLock;
              (inGameRoot->worldRuntime).simulationAndNetworkTickCallback =
                   InGameRuntime_UpdateSimulationAndNetworkTick;
              selectionBlockCursor = g_SelectionPlayerRuntimeBlockPointers[g_LocalPlayerRuntimeId];
              inGameRoot->localPlayerMarkedCellCount = 0;
              inGameRoot->localPlayerMarkedCells = selectionBlockCursor->markedCells;
              UiRootStack_Push(&g_UiRootCallbacks_0054FBC0,(UiRootNode *)inGameRoot);
              WidePath_CombineDirectoryAndLeaf
                        (&g_FrontendScenarioPathScratchUtf16,
                         (uint16_t *)(levelImage->header).opaque100_16F,(uint16_t *)u_level_0050daac);
              WidePath_SetExtensionCode(0x76656c,&g_FrontendScenarioPathScratchUtf16); /* "lev" */
              endingMoviePath = LevelAsset_PrepareEndingMoviePath
                                 (savePackagePath,(LevelAssetHeader *)levelImage);
              rootCursorOrError = (InGameRuntimeRoot *)endingMoviePath.moviePath;
              if (!endingMoviePath.failed) {
                movieOpen = Movie_Open(MOVIE_OPEN_PACKAGE_ONLY,(uint16_t *)rootCursorOrError);
                rootCursorOrError = (InGameRuntimeRoot *)movieOpen.frameCountOrError;
                if (!movieOpen.failed) {
                  firstFrame = Movie_AdvanceFrame();
                  rootCursorOrError = (InGameRuntimeRoot *)firstFrame.movieOrError;
                  if (!firstFrame.ended) {
                    inGameRoot->levelMovieRuntime = (MovieRuntime *)rootCursorOrError;
                    g_MoviePlaybackBaseFrameGroup = 0;
                    g_MoviePlaybackScheduleCounter = 0;
                    g_MoviePlaybackScheduleSpan = 0;
                    g_MoviePlaybackCurrentFrame = 0;
                    MoviePlayback_AdvanceToFrameAndPresent(0);
                    MoviePlayback_AdvanceToFrameAndPresent(1);
                    RecentTextHistory_SortAndBuildPointerList(8,&inGameRoot->recentTextHistory);
                    selectionBlockCursor = g_SelectionPlayerBlocks;
                    WorldRuntime_AttachObjectArray(INGAME_WORLD_OBJECT_RECORD_COUNT,g_InGameWorldObjectRecords,world);
                    rootCursorOrError =
                         (InGameRuntimeRoot *)selectionBlockCursor->factionIndex;
                    (inGameRoot->worldRuntime).activeFactionRuntimeIndex =
                         (FactionRuntimeIndex)rootCursorOrError;
                    (inGameRoot->worldRuntime).selection.activePlayerRuntimeId = 0;
                    WorldRuntime_AttachAndClearDwordArray
                              (INGAME_WORLD_DWORD_ARRAY_COUNT,(uint32_t *)&g_InGameWorldRuntimeDwordArray256,world);
                    terminatorOrFailure = GameData_LoadExternalTables();
                    if (!terminatorOrFailure) {
                      packageEntry = Package_LoadEntry((uint16_t *)u_field_hex_0050e002);
                      rootCursorOrError = packageEntry.bufferOrError;
                      if (!packageEntry.failed) {
                        (levelImage->header).pathState.levelPathOffsetOrLoadedFieldGrid =
                             (uint32_t)rootCursorOrError;
                        levelLoad = InGameLevelRuntime_LoadResourcesAfterExternalTables
                                           (levelImage,world);
                        rootCursorOrError = (InGameRuntimeRoot *)levelLoad.errorOrValue;
                        if (!levelLoad.failed) {
                          queueRecord = inGameRoot->notificationQueue;
                          for (remainingCount = 32; remainingCount != 0; remainingCount--) {
                            queueRecord->movieId = 0;
                            queueRecord = (InGameNotificationQueueRecord *)&queueRecord->priority;
                          }
                          statusResult = TerrainCompositeTexture_Create();
                          rootCursorOrError = (InGameRuntimeRoot *)statusResult.valueOrError;
                          if (!statusResult.failed) {
                            /* hold the step off while the world is finished */
                            g_SpinLockAcquire(&g_InGameStateTickSpinLock);
                            InGameBuildCatalog_RebuildGrid((UiNodeBase *)inGameRoot);
                            InGameSpecialBuildCatalog_RebuildGrid((UiNodeBase *)inGameRoot);
                            InGameArmyStock_RebuildGrid((UiNodeBase *)inGameRoot);
                            InGameOtherPlayerCommand_RebuildTargetEntries((UiNodeBase *)inGameRoot);
                            g_InGameSimulationStepTicks = 1;
                            settingOrFactionToken =
                                 PersistentSettings_Read(PERSISTENT_DEFAULT_SHADING_TEXTURE_DIMENSION,PERSISTENT_SETTING_SHADING_TEXTURE_DIMENSION);
                            gridHalfSize =
                                 PersistentSettings_Read(PERSISTENT_DEFAULT_SHADING_GRID_HALF_SIZE,PERSISTENT_SETTING_SHADING_GRID_HALF_SIZE);
                            subresourceCount =
                                 PersistentSettings_Read(PERSISTENT_DEFAULT_SHADING_SUBRESOURCE_COUNT,PERSISTENT_SETTING_SHADING_SUBRESOURCE_COUNT);
                            GraphicsShadingRuntime_InitializeGeneratedTexture
                                      (subresourceCount,gridHalfSize,settingOrFactionToken);
                            /* mirror the shading and mouse/panel options into the world runtime flags */
                            settingOrFactionToken = PersistentSettings_Read(1,PERSISTENT_SETTING_SHADING_ENABLED);
                            if (settingOrFactionToken == 0) {
                              worldRuntimeFlags = &(inGameRoot->worldRuntime).runtimeFlags;
                              *worldRuntimeFlags = *worldRuntimeFlags & ~WORLD_RUNTIME_FLAG_SHADING_ENABLED;
                            }
                            else {
                              worldRuntimeFlags = &(inGameRoot->worldRuntime).runtimeFlags;
                              *worldRuntimeFlags = *worldRuntimeFlags | WORLD_RUNTIME_FLAG_SHADING_ENABLED;
                            }
                            rootCursorOrError = (InGameRuntimeRoot *)
                                     PersistentSettings_Read(0,PERSISTENT_SETTING_MOUSE_LINK_PANEL_OPTION_FLAGS);
                            if (((uint32_t)rootCursorOrError & PERSISTENT_LINK_OPTION_ROTATION_ZOOM) == 0) {
                              worldRuntimeFlags = &(inGameRoot->worldRuntime).runtimeFlags;
                              *worldRuntimeFlags = *worldRuntimeFlags & ~WORLD_RUNTIME_FLAG_LINK_ROTATION_ZOOM;
                            }
                            else {
                              worldRuntimeFlags = &(inGameRoot->worldRuntime).runtimeFlags;
                              *worldRuntimeFlags = *worldRuntimeFlags | WORLD_RUNTIME_FLAG_LINK_ROTATION_ZOOM;
                            }
                            if (((uint32_t)rootCursorOrError & PERSISTENT_LINK_OPTION_ROTATION_TILT) == 0) {
                              worldRuntimeFlags = &(inGameRoot->worldRuntime).runtimeFlags;
                              *worldRuntimeFlags = *worldRuntimeFlags & ~WORLD_RUNTIME_FLAG_LINK_ROTATION_TILT;
                            }
                            else {
                              worldRuntimeFlags = &(inGameRoot->worldRuntime).runtimeFlags;
                              *worldRuntimeFlags = *worldRuntimeFlags | WORLD_RUNTIME_FLAG_LINK_ROTATION_TILT;
                            }
                            if (((uint32_t)rootCursorOrError & PERSISTENT_LINK_OPTION_HIDE_PANEL) == 0) {
                              worldRuntimeFlags = &(inGameRoot->worldRuntime).runtimeFlags;
                              *worldRuntimeFlags = *worldRuntimeFlags & ~WORLD_RUNTIME_FLAG_HIDE_PANEL;
                            }
                            else {
                              worldRuntimeFlags = &(inGameRoot->worldRuntime).runtimeFlags;
                              *worldRuntimeFlags = *worldRuntimeFlags | WORLD_RUNTIME_FLAG_HIDE_PANEL;
                            }
                            terminatorOrFailure = (bool)InGameRuntime_InitializeOptionalSubsystemAlwaysSuccess
                                                     ((uint32_t)(inGameRoot->worldRuntime).
                                                             fieldGrid);
                            if (!terminatorOrFailure) {
                              gridScratch = GridScratch_AllocateForFieldGrid
                                                 ((inGameRoot->worldRuntime).fieldGrid);
                              rootCursorOrError = (InGameRuntimeRoot *)gridScratch.valueOrError;
                              if (!gridScratch.failed) {
                                GridScratch_RebuildTerrainAndRuntimeClassificationMasks(world);
                                GridInfluence_ClearDistanceBandsAndRefreshEntities
                                          ((inGameRoot->worldRuntime).ownerListHead);
                                TechnologyRuntime_RebuildDerivedLimitsAndCategoryMasks();
                                WorldLightingRuntime_UpdateInterpolatedTerrainLighting();
                                if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_NETWORKED_MASK
                                    ) == SESSION_NETWORK_ROLE_LOCAL) {
                                  inGameRoot->worldViewWrappedTextNodeFlags =
                                       inGameRoot->worldViewWrappedTextNodeFlags | 8;
                                }
                                else {
                                  inGameRoot->worldViewWrappedTextNodeFlags =
                                       inGameRoot->worldViewWrappedTextNodeFlags & ~8u;
                                }
                                /* report this player as loaded */
                                if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_NETWORKED_MASK
                                    ) == SESSION_NETWORK_ROLE_LOCAL) {
                                  FrontendPlayerRuntime_IncrementReadyCountAndResolveConsensus
                                            (g_LocalPlayerRuntimeId,0,0,0);
                                }
                                else {
                                  InGameCommandQueue_AppendLocalPlayerCommand(INGAME_COMMAND_PLAYER_READY,0,0,0);
                                }
                                g_SpinLockRelease(&g_InGameStateTickSpinLock);
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
                                Package_Unmount((EngineFileHandle)mountResult);
                                g_GraphicsCursorSetFrame(GRAPHICS_CURSOR_FRAME_ARROW);
                                g_TimerRegisterPeriodic
                                          (10,InGameRuntime_ProcessQueuedSessionNotificationTimer);
                                return THANDOR_BITCAST(uint64_t, LoadedSessionInitResult,
                                     (THANDOR_BITCAST(ArenaAllocResult, uint64_t,
                                                       allocation) & UINT32_MAX));
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
   Ends an in-game session (counterpart of InGameRuntime_InitializeNewSession/InitializeLoadedSession): stops the
   step timer, shows a black screen with the busy cursor, then releases the world (every entity's bindings, the
   level assets), the in-game UI root, the faction scratch buffers, the object pool, the level movie, the terrain
   texture and the four panel texture packages, and resets the sprite registry and pending input so the frontend
   starts clean.
*/
void InGameRuntime_ShutdownAndReleaseResources(void)

{
  WorldRuntimeContext *world;
  InGameRuntimeRoot *inGameRoot;
  bool beginAccessFailed;
  
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
    WorldRuntime_ForEachOwnerListNode
              (world,WorldRuntimeNode_ReleaseShutdownBindingsCallback,world);
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
  g_GraphicsTextureSourceLifecycleCallbacks3.releasePackage(g_InGameDiagramTextureSource);
  g_GraphicsTextureSourceLifecycleCallbacks3.releasePackage(g_InGamePanelTextureSource);
  g_GraphicsTextureSourceLifecycleCallbacks3.releasePackage(g_InGameTechnologyTextureSource);
  g_GraphicsTextureSourceLifecycleCallbacks3.releasePackage(g_InGameWindowTextureSource);
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


/* Address: 0x0050E0D0.
   Frees the two scratch buffers of each of the eight factions (sets A and B) at session shutdown and clears the
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


/* Address: 0x0050E120.
   The level script, evaluated every 20 simulation steps: first promotes factions that were marked as ending to
   ended, then re-evaluates the level's 64 scheduled conditions (bit 0 of each record's kind = satisfied: unit
   counts, resource amounts, map share, countdowns, boolean expressions over other conditions), and finally checks
   the 16 end triggers. The first active trigger whose condition holds ends its faction: its units are disabled,
   the local player loses map input when it is theirs, and unless two remaining active factions are still not
   allied (relation state below 8) the end movie is chosen and UI_COMMAND_RUNTIME_FLAG_END_MOVIE_PENDING ends the
   session. A trigger with skipArmyDisableWhenOne == 1 goes to the end movie directly.
   Expression tokens: 0xFC end, 0xFD NOT, 0xFE AND, 0xFF OR, anything else pushes that condition's result bit
   (the value is a bit stack in kindOrExpressionValue).
*/
void InGameConditionRuntime_UpdateScheduledRecords(void)

{
  ResourceExtractionDescriptor32 *cellExtractionFlags;
  uint32_t secondFactionIndex;
  ArmyRuntimeSlot *conditionArmy;
  InGameLevelConditionStorage *levelConditionStorage;
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
  WorldOwnerListNode *worldNode;
  InGameConditionSchedule *scheduledCondition;
  InGameEndConditionTriggerRecord8 *endTrigger;
  FieldGridAsset *conditionFieldGrid;
  InGameRuntimeRoot *triggerRoot;
  InGameRuntimeRoot *relationRoot;
  
#ifdef THANDOR_TEST_AIDS
  if (g_GameFactionRuntimeImage.tail.simulationTick == 20) {
    /* dump the level script before its first evaluation: every used condition (16 raw bytes) and trigger */
    const uint8_t *raw = (const uint8_t *)&(g_InGameLevelRuntimeGlobalBlock.conditionStorage)->schedule;
    int index;
    for (index = 0; index < 64; index++) {
      const uint8_t *c = raw + index * 16;
      if (c[0] != 0) {
        Thandor_Log("level script: condition %2d: %02x %02x %02x %02x | %02x %02x %02x %02x | %02x %02x %02x %02x | "
                    "%02x %02x %02x %02x",index,c[0],c[1],c[2],c[3],c[4],c[5],c[6],c[7],c[8],c[9],c[10],c[11],
                    c[12],c[13],c[14],c[15]);
      }
    }
    for (index = 0; index < 16; index++) {
      const uint8_t *t = (const uint8_t *)&(g_InGameLevelRuntimeGlobalBlock.conditionStorage)->schedule.triggers[index];
      if (t[0] != 0) {
        Thandor_Log("level script: trigger %2d: %02x %02x %02x %02x %02x %02x %02x %02x",index,t[0],t[1],t[2],t[3],
                    t[4],t[5],t[6],t[7]);
      }
    }
  }
#endif
  lifecycleState = g_GameFactionRuntimeImage.tail.factionLifecycleStates;
  remainingCount = 7;
  do {
    lifecycleState++;
    if (*lifecycleState == FACTION_RUNTIME_LIFECYCLE_ENDING_PENDING) {
      /* ENDING_PENDING + 1 = ENDED_OR_TRANSITIONED */
      *lifecycleState = *lifecycleState + FACTION_RUNTIME_LIFECYCLE_ACTIVE;
    }
    levelConditionStorage = g_InGameLevelRuntimeGlobalBlock.conditionStorage;
    remainingCount--;
  } while (remainingCount != 0);
  remainingCount = INGAME_SCHEDULED_CONDITION_COUNT;
  scheduledCondition = &(g_InGameLevelRuntimeGlobalBlock.conditionStorage)->schedule;
  do {
    /* clear the satisfied bit, then set it again when the condition holds */
    kindOrExpressionValue = scheduledCondition->conditions[0].statusAndKind.kind;
    scheduledCondition->conditions[0].statusAndKind.kind =
         scheduledCondition->conditions[0].statusAndKind.kind & ~INGAME_SCHEDULED_CONDITION_SATISFIED;
    switch(kindOrExpressionValue & INGAME_SCHEDULED_CONDITION_KIND_MASK) {
    case INGAME_SCHEDULED_CONDITION_FACTION_HAS_NO_ARMY:
      for (worldNode = (g_InGameRuntimeRoot->worldRuntime).ownerListHead;
          worldNode != NULL; worldNode = worldNode->nextNode) {
        if ((worldNode->ownerClassId == WORLD_OWNER_RUNTIME_MODEL) &&
           (scheduledCondition->conditions[0].payload.operands[0] ==
            ((ModelRuntimeSlot *)worldNode->runtimePayload)->ownerArmyRuntimeOrSavedOffset.armyRuntime->factionIndex))
        goto next_condition;
      }
      goto condition_satisfied;
    case INGAME_SCHEDULED_CONDITION_FACTION_HAS_NO_COMMAND_GROUP_A_ARMY:
      for (worldNode = (g_InGameRuntimeRoot->worldRuntime).ownerListHead;
          worldNode != NULL; worldNode = worldNode->nextNode) {
        if (((worldNode->ownerClassId == WORLD_OWNER_RUNTIME_MODEL) &&
            (g_ArmyRuntimeOrderHandlerMatrix11Columns24Classes.classCommand
             [((ModelRuntimeSlot *)worldNode->runtimePayload)->definitionOrSavedId.runtimeDefinition->runtimeClassId] ==
             ArmyRuntime_ClassCommandHandlerGroupA)) &&
           (((ModelRuntimeSlot *)worldNode->runtimePayload)->ownerArmyRuntimeOrSavedOffset.armyRuntime->factionIndex ==
            scheduledCondition->conditions[0].payload.operands[0]))
        goto next_condition;
      }
      goto condition_satisfied;
    case INGAME_SCHEDULED_CONDITION_FACTION_HAS_NO_ARMY_OF_ASSET:
      for (worldNode = (g_InGameRuntimeRoot->worldRuntime).ownerListHead;
          worldNode != NULL; worldNode = worldNode->nextNode) {
        if (((worldNode->ownerClassId == WORLD_OWNER_RUNTIME_MODEL) &&
            (conditionArmy = ((ModelRuntimeSlot *)worldNode->runtimePayload)->ownerArmyRuntimeOrSavedOffset.armyRuntime,
            scheduledCondition->conditions[0].payload.operands[0] == conditionArmy->factionIndex)) &&
           (conditionArmy->armyAssetId == scheduledCondition->conditions[0].payload.operands[2]))
        goto next_condition;
      }
      goto condition_satisfied;
    case INGAME_SCHEDULED_CONDITION_FACTION_INACTIVE_OR_RELATION_AT_LEAST_8:
      operandValue = scheduledCondition->conditions[0].payload.operands[1];
      secondFactionIndex = scheduledCondition->conditions[0].payload.operands[0];
      if (((g_GameFactionRuntimeImage.tail.factionLifecycleStates[operandValue] !=
            FACTION_RUNTIME_LIFECYCLE_ACTIVE) ||
          (g_GameFactionRuntimeImage.tail.factionLifecycleStates[secondFactionIndex] !=
           FACTION_RUNTIME_LIFECYCLE_ACTIVE)) ||
         (FACTION_RELATION_STATE_ALLIED - 1 < (g_GameFactionRuntimeImage.records[operandValue].packedRelationStates >>
               ((char)secondFactionIndex * 4 & 31U) & 0xf)))
      goto condition_satisfied;
      break;
    case INGAME_SCHEDULED_CONDITION_XENITE_AT_LEAST:
      if ((int)scheduledCondition->conditions[0].payload.operands[1] <=
          (int)g_GameFactionRuntimeImage.records[scheduledCondition->conditions[0].payload.operands[0]].
               xeniteCurrentQ4) {
        scheduledCondition->conditions[0].statusAndKind.kind =
             scheduledCondition->conditions[0].statusAndKind.kind | INGAME_SCHEDULED_CONDITION_SATISFIED;
      }
      break;
    case INGAME_SCHEDULED_CONDITION_TRITIUM_AT_LEAST:
      if ((int)scheduledCondition->conditions[0].payload.operands[1] <=
          (int)g_GameFactionRuntimeImage.records[scheduledCondition->conditions[0].payload.operands[0]].
               tritiumCurrentQ4) {
        scheduledCondition->conditions[0].statusAndKind.kind =
             scheduledCondition->conditions[0].statusAndKind.kind | INGAME_SCHEDULED_CONDITION_SATISFIED;
      }
      break;
    case INGAME_SCHEDULED_CONDITION_TRITIUM_EXTRACTION_RATE_AT_LEAST:
      if ((int)scheduledCondition->conditions[0].payload.operands[1] <=
          (int)g_GameFactionRuntimeImage.records[scheduledCondition->conditions[0].payload.operands[0]].
               tritiumExtractionRateQ4PerTick) {
        scheduledCondition->conditions[0].statusAndKind.kind =
             scheduledCondition->conditions[0].statusAndKind.kind | INGAME_SCHEDULED_CONDITION_SATISFIED;
      }
      break;
    case INGAME_SCHEDULED_CONDITION_ARMY_OF_ASSET_COUNT_AT_LEAST:
      operandValue = scheduledCondition->conditions[0].payload.operands[1];
      for (worldNode = (g_InGameRuntimeRoot->worldRuntime).ownerListHead;
          worldNode != NULL; worldNode = worldNode->nextNode) {
        if (((worldNode->ownerClassId == WORLD_OWNER_RUNTIME_MODEL) &&
            (((ModelRuntimeSlot *)worldNode->runtimePayload)->ownerArmyRuntimeOrSavedOffset.armyRuntime->factionIndex ==
             scheduledCondition->conditions[0].payload.operands[0])) &&
           ((((ModelRuntimeSlot *)worldNode->runtimePayload)->ownerArmyRuntimeOrSavedOffset.armyRuntime->armyAssetId ==
             scheduledCondition->conditions[0].payload.operands[2] &&
                  (operandValue = operandValue - 1, operandValue == 0))))
        goto condition_satisfied;
      }
      break;
    case INGAME_SCHEDULED_CONDITION_FACTION_TERRAIN_OCCUPANCY_MASK_F9_PERCENT_AT_LEAST:
      conditionFieldGrid = (g_InGameRuntimeRoot->worldRuntime).fieldGrid;
      cellCount = conditionFieldGrid->gridWidth * conditionFieldGrid->gridHeight;
      byteCursor = (uint8_t *)conditionFieldGrid->cells + scheduledCondition->conditions[0].payload.operands[0];
      countOrFactionIndex = 0;
      cellsLeftOrFaction = cellCount;
      do {
        cellExtractionFlags = (ResourceExtractionDescriptor32 *)(byteCursor + offsetof(FieldGridCell, occupancyMask));
        byteCursor = byteCursor + sizeof(FieldGridCell);
        countOrFactionIndex = countOrFactionIndex + ((*cellExtractionFlags & FIELD_CELL_OCCUPANCY_PRESENCE_BITS) != 0);
        cellsLeftOrFaction--;
      } while (cellsLeftOrFaction != 0);
      if ((int)scheduledCondition->conditions[0].payload.operands[1] <=
          (int)(((uint64_t)countOrFactionIndex * 100) / (uint64_t)cellCount)) {
        scheduledCondition->conditions[0].statusAndKind.kind =
             scheduledCondition->conditions[0].statusAndKind.kind | INGAME_SCHEDULED_CONDITION_SATISFIED;
      }
      break;
    case INGAME_SCHEDULED_CONDITION_COUNTDOWN_ELAPSED:
      operandValue = scheduledCondition->conditions[0].payload.operands[1] - g_InGameSimulationStepTicks;
      scheduledCondition->conditions[0].payload.operands[1] = operandValue;
      if ((int)operandValue < 1) {
        scheduledCondition->conditions[0].statusAndKind.kind =
             scheduledCondition->conditions[0].statusAndKind.kind | INGAME_SCHEDULED_CONDITION_SATISFIED;
        scheduledCondition->conditions[0].payload.operands[1] = 0;
      }
      break;
    case INGAME_SCHEDULED_CONDITION_XENITE_STORAGE_LIMIT_AT_MOST_0FA0:
      if ((int)g_GameFactionRuntimeImage.records[scheduledCondition->conditions[0].payload.operands[0]].
               xeniteStorageLimitQ4 < (250 << Q4_SHIFT) + 1) {
        scheduledCondition->conditions[0].statusAndKind.kind =
             scheduledCondition->conditions[0].statusAndKind.kind | INGAME_SCHEDULED_CONDITION_SATISFIED;
      }
      break;
    case INGAME_SCHEDULED_CONDITION_NO_ARMY_OF_CLASS_OUTSIDE_COMMAND_GROUP_A:
      for (worldNode = (g_InGameRuntimeRoot->worldRuntime).ownerListHead;
          worldNode != NULL; worldNode = worldNode->nextNode) {
        if (((worldNode->ownerClassId == WORLD_OWNER_RUNTIME_MODEL) &&
            (operandValue =
             ((ModelRuntimeSlot *)worldNode->runtimePayload)->definitionOrSavedId.runtimeDefinition->runtimeClassId,
            g_ArmyRuntimeOrderHandlerMatrix11Columns24Classes.classCommand[operandValue] !=
            ArmyRuntime_ClassCommandHandlerGroupA)) &&
           (operandValue == scheduledCondition->conditions[0].payload.operands[0]))
        goto next_condition;
      }
condition_satisfied:
      scheduledCondition->conditions[0].statusAndKind.kind =
             scheduledCondition->conditions[0].statusAndKind.kind | INGAME_SCHEDULED_CONDITION_SATISFIED;
      break;
    case INGAME_SCHEDULED_CONDITION_BOOLEAN_POSTFIX_EXPRESSION:
      byteCursor = &scheduledCondition->conditions[0].statusAndKind.kindAndExpression[1];
      kindOrExpressionValue = INGAME_SCHEDULED_CONDITION_NONE_OR_UNUSED;
      while( true ) {
        while( true ) {
          while( true ) {
            while( true ) {
              tokenOrShift = *byteCursor;
              byteCursor++;
              if (tokenOrShift != INGAME_CONDITION_TOKEN_OR) break;
              kindOrExpressionValue = kindOrExpressionValue >> 1 | kindOrExpressionValue & 1;
            }
            if (tokenOrShift != INGAME_CONDITION_TOKEN_AND) break;
            kindOrExpressionValue = kindOrExpressionValue >> 1 & (kindOrExpressionValue | ~1u);
          }
          if (tokenOrShift != INGAME_CONDITION_TOKEN_NOT) break;
          kindOrExpressionValue = kindOrExpressionValue ^ 1;
        }
        if (tokenOrShift == INGAME_CONDITION_TOKEN_END) break;
        kindOrExpressionValue =
             ((levelConditionStorage->schedule).conditions[tokenOrShift].statusAndKind.kind &
              INGAME_SCHEDULED_CONDITION_SATISFIED) + kindOrExpressionValue * 2;
      }
      scheduledCondition->conditions[0].statusAndKind.kind =
           scheduledCondition->conditions[0].statusAndKind.kind | kindOrExpressionValue & 1;
    }
next_condition:
    scheduledCondition = (InGameConditionSchedule *)(scheduledCondition->conditions + 1);
    remainingCount--;
    if (remainingCount == 0) {
#ifdef THANDOR_TEST_AIDS
      if (g_GameFactionRuntimeImage.tail.simulationTick == 20) {
        const uint8_t *c = (const uint8_t *)&(levelConditionStorage->schedule).conditions[10];
        Thandor_Log("level script: after evaluation condition 10: %02x %02x %02x %02x | %02x, storage %p/%p",
                    c[0],c[1],c[2],c[3],c[4],(void *)levelConditionStorage,
                    (void *)g_InGameLevelRuntimeGlobalBlock.conditionStorage);
      }
#endif
      endTrigger = (InGameEndConditionTriggerRecord8 *)(levelConditionStorage->schedule).triggers;
      remainingCount = INGAME_END_CONDITION_TRIGGER_COUNT;
      do {
        if ((endTrigger->stateFlags == INGAME_END_CONDITION_TRIGGER_ACTIVE) &&
           (((levelConditionStorage->schedule).conditions[endTrigger->conditionIndex].statusAndKind.kind &
             INGAME_SCHEDULED_CONDITION_SATISFIED) != 0)) {
          endTrigger->stateFlags = endTrigger->stateFlags | INGAME_END_CONDITION_TRIGGER_PROCESSED;
#ifdef THANDOR_TEST_AIDS
          {
            InGameScheduledConditionRecord10 *condition =
                 &(levelConditionStorage->schedule).conditions[endTrigger->conditionIndex];
            Thandor_Log("level script: end trigger %u fired at tick %u: condition %u kind %u operands %d %d %d, "
                        "faction %u (local %u, lifecycle %u), end selection %u",
                        (unsigned)(16 - remainingCount),(unsigned)g_GameFactionRuntimeImage.tail.simulationTick,
                        (unsigned)endTrigger->conditionIndex,(unsigned)(condition->statusAndKind.kind & ~1u),
                        ((int *)condition)[1],((int *)condition)[2],((int *)condition)[3],
                        (unsigned)endTrigger->factionRuntimeIndex,
                        (unsigned)(g_InGameRuntimeRoot->worldRuntime).activeFactionRuntimeIndex,
                        (unsigned)g_GameFactionRuntimeImage.tail.factionLifecycleStates[endTrigger->factionRuntimeIndex],
                        (unsigned)endTrigger->endMovieSelectionIndex);
          }
#endif
          triggerRoot = g_InGameRuntimeRoot;
          cellsLeftOrFaction = (uint32_t)endTrigger->factionRuntimeIndex;
          if (g_GameFactionRuntimeImage.tail.factionLifecycleStates[cellsLeftOrFaction] ==
              FACTION_RUNTIME_LIFECYCLE_ACTIVE) {
            contextArg = &g_InGameRuntimeRoot->worldRuntime;
            g_GameFactionRuntimeImage.tail.factionLifecycleStates[cellsLeftOrFaction] =
                 FACTION_RUNTIME_LIFECYCLE_ENDING_PENDING;
            if (endTrigger->skipArmyDisableWhenOne != 1) {
              worldNode = (triggerRoot->worldRuntime).ownerListHead;
              if (worldNode != NULL) {
                do {
                  if ((worldNode->ownerClassId == WORLD_OWNER_RUNTIME_MODEL) &&
                     (conditionArmy = ((ModelRuntimeSlot *)worldNode->runtimePayload)->
                                      ownerArmyRuntimeOrSavedOffset.armyRuntime,
                     cellsLeftOrFaction == conditionArmy->factionIndex)) {
                    ModelRuntimeHierarchy_MarkDestroyedRecursive(contextArg,conditionArmy);
                  }
                  worldNode = worldNode->nextNode;
                } while (worldNode != NULL);
                g_GameFactionRuntimeImage.records[cellsLeftOrFaction].secondaryArmyAssetCount = 0;
                g_GameFactionRuntimeImage.records[cellsLeftOrFaction].primaryArmyAssetCount = 0;
              }
              relationRoot = g_InGameRuntimeRoot;
              if (cellsLeftOrFaction == (triggerRoot->worldRuntime).activeFactionRuntimeIndex) {
                g_UiCommandRuntimeFlags =
                     g_UiCommandRuntimeFlags |
                     (UI_COMMAND_RUNTIME_FLAG_WORLD_INPUT_DISABLED | UI_COMMAND_RUNTIME_FLAG_LOCAL_FACTION_ENDED);
              }
              /* the game goes on while two active factions (1..7) are not allied */
              lifecycleState = g_GameFactionRuntimeImage.tail.factionLifecycleStates;
              cellsLeftOrFaction = 1;
              tokenOrShift = 4;
              do {
                lifecycleState++;
                if (*lifecycleState == FACTION_RUNTIME_LIFECYCLE_ACTIVE) {
                  countOrFactionIndex = cellsLeftOrFaction + 1;
                  otherLifecycleState = lifecycleState;
                  do {
                    otherLifecycleState++;
                    if ((*otherLifecycleState == FACTION_RUNTIME_LIFECYCLE_ACTIVE) &&
                       ((g_GameFactionRuntimeImage.records[countOrFactionIndex].packedRelationStates >>
                         (tokenOrShift & 31) & 0xf) < FACTION_RELATION_STATE_ALLIED)) {
                      if ((uint32_t)endTrigger->factionRuntimeIndex ==
                          (g_InGameRuntimeRoot->worldRuntime).activeFactionRuntimeIndex) {
                        g_InGameRuntimeRoot->diplomacyPanelNodeFlags =
                             g_InGameRuntimeRoot->diplomacyPanelNodeFlags | 8;
                        INGAME_UI(relationRoot,buildCatalogPanel)->nodeFlags =
                             INGAME_UI(relationRoot,buildCatalogPanel)->nodeFlags | 8;
                        INGAME_UI(relationRoot,specialBuildCatalogPanel)->nodeFlags =
                             INGAME_UI(relationRoot,specialBuildCatalogPanel)->nodeFlags | 8;
                        INGAME_UI(relationRoot,armyStockPanel)->nodeFlags =
                             INGAME_UI(relationRoot,armyStockPanel)->nodeFlags | 8;
                      }
                      return;
                    }
                    countOrFactionIndex++;
                  } while (countOrFactionIndex < 8);
                }
                cellsLeftOrFaction++;
                tokenOrShift = tokenOrShift + 4;
              } while (cellsLeftOrFaction < 7);
              contextArg = &g_InGameRuntimeRoot->worldRuntime;
              cellsLeftOrFaction = (uint32_t)endTrigger->factionRuntimeIndex;
            }
            /* end movie variant from the local faction's view: the trigger's variant when the ended faction is
               the local one or one it rates above 3, the other variant when the local faction has not ended and
               rates it 3 or below, variant 0 when the local faction has ended too (or is unused) */
            countOrFactionIndex = contextArg->activeFactionRuntimeIndex;
            g_EndMovieVariantIndex = (uint32_t)endTrigger->movieVariantSelector;
            if (((countOrFactionIndex != cellsLeftOrFaction) &&
                (g_EndMovieVariantIndex = 0,
                g_GameFactionRuntimeImage.tail.factionLifecycleStates[countOrFactionIndex] <
                FACTION_RUNTIME_LIFECYCLE_ENDED_OR_TRANSITIONED)) &&
               (g_GameFactionRuntimeImage.tail.factionLifecycleStates[countOrFactionIndex] !=
                FACTION_RUNTIME_LIFECYCLE_INACTIVE)) {
              g_EndMovieVariantIndex = endTrigger->movieVariantSelector ^ 1;
              if (FACTION_RELATION_STATE_FRIENDLY - 1 <
                  (g_GameFactionRuntimeImage.records[countOrFactionIndex].packedRelationStates >>
                       ((char)cellsLeftOrFaction * 4 & 31U) & 0xf)) {
                g_EndMovieVariantIndex = (uint32_t)endTrigger->movieVariantSelector;
              }
            }
            g_EndMovieSelectionIndex = (uint32_t)endTrigger->endMovieSelectionIndex;
            g_EndMoviePath = (uint16_t *)u_flm_ende0000_flm_0050df06;
            if (g_EndMovieVariantIndex == 0) {
              g_EndMoviePath = (uint16_t *)u_flm_ende0001_flm_0050df28;
            }
            g_UiCommandRuntimeFlags = g_UiCommandRuntimeFlags | UI_COMMAND_RUNTIME_FLAG_END_MOVIE_PENDING;
            return;
          }
        }
        endTrigger++;
        remainingCount--;
        if (remainingCount == 0) {
          return;
        }
      } while( true );
    }
  } while( true );
}


/* Address: 0x00513160.
   The faction economy, run every 8th simulation step (job 0 of InGameRuntime_UpdateSimulationAndNetworkTick):
   1. per faction: reset the step's energy demand and extraction rates, decay the pair-pressure matrix by 7/8,
      count the notification/anchor cooldowns down;
   2. mining: every connected region of Xenite cells (FIELD_CELL_XENITE_SUPPORT), then of Tritium cells
      (FIELD_CELL_TRITIUM_SUPPORT), pays its owning factions (rate, stock and total), then stocks are capped at
      the storage limits;
   3. energy: all powered models (and the attached parts of models whose class has flag 0x80) are sorted by the
      priority of their class (g_FactionEnergyAllocationPriorityByModelClass); per faction 7..1 the supply
      (baselineEnergySupplyQ4 + Tritium stock * 16, capped by energyGenerationCapacityQ4) first covers the fixed
      demand of its army assets, then the consumers in priority order; consumers left over are flagged unpowered
      and the local player gets notification 400 (generation capacity too low) or 401 (supply too low), at
      most every 150 economy runs. The energy used above the baseline burns Tritium;
   4. every 128 steps the progress/score metrics of factions 1..7 are stored in the stat table
      (g_GameStatTableImage, shown by the results screen).
   All amounts are Q4 fixed point.
*/
void __fastcall InGameRuntime_UpdateFactionResourceExtractionAndEnergyAllocationState(void)

{
  uint8_t *totalAccumulator;
  FieldGridDimension fieldGridWidth;
  int *attachedRuntime;
  InGameLevelConditionStorage *levelConditionStorage;
  InGameRuntimeRoot *inGameRoot;
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
  WorldOwnerListNode *worldNode;
  
  inGameRoot = g_InGameRuntimeRoot;
  factionRecord = g_GameFactionRuntimeImage.records;
  pairPressureRow = g_GameDataAuxState.pairPressureMatrix8x8;
  counterOrValue = 8;
  /* 1. per-faction resets, decays and cooldowns (anchorCooldown1/2 are the energy notification cooldowns) */
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
      factionRecord->anchorCooldown1--;
    }
    if (factionRecord->anchorCooldown2 != 0) {
      factionRecord->anchorCooldown2--;
    }
    if (factionRecord->primaryAnchorCooldown != 0) {
      factionRecord->primaryAnchorCooldown--;
    }
    if (factionRecord->anchorCooldown0 != 0) {
      factionRecord->anchorCooldown0--;
    }
    factionRecord->relationTransitionTick++;
    pairPressureRow = pairPressureRow + 8;
    factionRecord++;
    counterOrValue--;
  } while (counterOrValue != 0);
  /* 2. mining: first pass Xenite cells (record offsets +0), second pass Tritium cells (+0x10). Each unvisited
     region pays every faction listed in the collected entries (faction in bits 13..23, share in bits 24..31) */
  factionFieldGrid = (inGameRoot->worldRuntime).fieldGrid;
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
      counterOrValue--;
      cellsRemaining = cellCountOrValue;
      cell = firstCell;
      clearCursor++;
    } while (counterOrValue != 0);
    do {
      if (((cell->flagsAndMaterial & (FIELD_CELL_GRID_EDGE_MASK | FIELD_CELL_CONNECTED_REGION_VISITED)) == 0) &&
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
            factionOrDemand = *entryCursor >> 13 & 0x7ff;
            valueOrFactionIndex = counterOrValue * 2 * (*entryCursor >> 24) *
                    g_GameFactionRuntimeImage.records[factionOrDemand].terrainContributionScaleQ8 >> 15;
            entryCountOrValue = entryCursor[1];
            /* the Xenite fields of the faction record, or the Tritium ones (resourceOffsetOrValue 0x10) */
            runtimeOrStatCursor = (int *)(resourceOffsetOrValue +
                                  (uintptr_t)&g_GameFactionRuntimeImage.records[0].xeniteExtractionRateQ4PerTick +
                                  factionOrDemand * GAME_FACTION_RUNTIME_RECORD_BYTES);
            *runtimeOrStatCursor = *runtimeOrStatCursor + valueOrFactionIndex;
            tickContribution = valueOrFactionIndex * g_InGameSimulationStepTicks;
            totalAccumulator = (uint8_t *)&g_GameFactionRuntimeImage.records[factionOrDemand].xeniteCurrentQ4 +
                 resourceOffsetOrValue;
            *(int *)totalAccumulator = *(int *)totalAccumulator + tickContribution;
            runtimeOrStatCursor = (int *)(resourceOffsetOrValue +
                                  (uintptr_t)&g_GameFactionRuntimeImage.records[0].xeniteExtractedTotalQ4 +
                                  factionOrDemand * GAME_FACTION_RUNTIME_RECORD_BYTES);
            *runtimeOrStatCursor = *runtimeOrStatCursor + tickContribution;
            if ((entryCountOrValue != 0) &&
               (rebasedModel = entryCountOrValue + g_ModelRuntimeRebaseDelta,
                ((ModelRuntimeSlot *)rebasedModel)->rootModelNodeOrSavedOffset.raw != 0)) {
              /* the extracting model shows its current yield */
              ((ModelRuntimeSlot *)rebasedModel)->classLinkState.modelLinkOrState.signedScalarState =
                   tickContribution;
            }
            entryCursor = entryCursor + 2;
            remainingRegionEntries--;
          } while (remainingRegionEntries != 0);
        }
      }
      cellsRemaining--;
      cell++;
    } while (cellsRemaining != 0);
    requiredOccupancyMask = requiredOccupancyMask * 2;
    resourceOffsetOrValue = resourceOffsetOrValue + 16;
    counterOrValue = cellCountOrValue;
    clearCursor = firstCell;
  } while (requiredOccupancyMask == FIELD_CELL_TRITIUM_SUPPORT);
  /* cap the stocks at the storage limits */
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
    counterOrValue--;
  } while (counterOrValue != 0);
  /* 3. energy consumers, collected into the region scratch buffer as 16-byte entries
     {runtime, faction, demand, priority}, at most 256 */
  entryCountOrValue = 0;
  entryCursor = g_TerrainRegionCollectionEntries;
  for (worldNode = (g_InGameRuntimeRoot->worldRuntime).ownerListHead;
      worldNode != NULL; worldNode = worldNode->nextNode) {
    if ((worldNode->ownerClassId == WORLD_OWNER_RUNTIME_MODEL) &&
       (runtimeOrStatCursor = worldNode->runtimePayload, (runtimeOrStatCursor[59] & ARMY_MODEL_STATE_DISMANTLING) == 0)) {
      if (runtimeOrStatCursor[61] != 0) {
        if (255 < entryCountOrValue) continue;
        valueOrFactionIndex = ((ArmyRuntimeSlot *)runtimeOrStatCursor[2])->factionIndex;
        counterOrValue = ((ModelDefinition *)*runtimeOrStatCursor)->runtimeClassId;
        *entryCursor = (uint32_t)runtimeOrStatCursor;
        entryCursor[1] = valueOrFactionIndex;
        valueOrFactionIndex = runtimeOrStatCursor[61];
        entryCursor[3] = *(uint32_t *)(&g_FactionEnergyAllocationPriorityByModelClass + counterOrValue * 4);
        entryCursor[2] = valueOrFactionIndex;
        entryCountOrValue++;
        entryCursor = entryCursor + 4;
      }
      counterOrValue = runtimeOrStatCursor[3];
      if ((entryCountOrValue < 256) && ((((ModelDefinition *)*runtimeOrStatCursor)->modelFlags & MODEL_DEFINITION_FLAG_COUNT_ATTACHED_ENERGY) != 0)) {
        for (; counterOrValue != 0; counterOrValue--) {
          attachedRuntime = (int *)runtimeOrStatCursor[80];
          if (((attachedRuntime != NULL) && (attachedRuntime[61] != 0)) && (entryCountOrValue < 256)) {
            valueOrFactionIndex = ((ArmyRuntimeSlot *)attachedRuntime[2])->factionIndex;
            cellCountOrValue = ((ModelDefinition *)*attachedRuntime)->runtimeClassId;
            *entryCursor = (uint32_t)attachedRuntime;
            entryCursor[1] = valueOrFactionIndex;
            valueOrFactionIndex = attachedRuntime[61];
            entryCursor[3] = *(uint32_t *)(&g_FactionEnergyAllocationPriorityByModelClass + cellCountOrValue * 4);
            entryCursor[2] = valueOrFactionIndex;
            entryCountOrValue++;
            entryCursor = entryCursor + 4;
          }
          runtimeOrStatCursor = runtimeOrStatCursor + 8;
        }
      }
    }
  }
  /* without any consumer the whole allocation below is skipped (no army-asset demand, no Tritium burn) */
  if (entryCountOrValue != 0) {
    if (1 < entryCountOrValue) {
      /* selection sort by priority (entry[3]), highest first; the original swaps with XCHG */
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
          counterOrValue--;
        } while (counterOrValue != 0);
        if (factionOrDemand - 1 < 2) break;
        valueOrFactionIndex = sortBaseOrFlags[7];
        entryCursor = sortBaseOrFlags + 8;
        counterOrValue = factionOrDemand - 2;
        factionOrDemand--;
        sortBaseOrFlags = sortBaseOrFlags + 4;
      }
    }
    /* allocate per faction 7..1 (faction 0 gets nothing) */
    valueOrFactionIndex = 7;
    reverseFactionRecord = g_GameFactionRuntimeImage.records + 7;
    do {
      entryCursor = g_TerrainRegionCollectionEntries;
      counterOrValue = 0;
      factionOrDemand = 0;
      /* fixed demand: 1 energy (0x10 Q4) per army asset, 5 (0x50) when its definitionClassValue74 is set */
      for (remainingArmyAssets = reverseFactionRecord->primaryArmyAssetCount; remainingArmyAssets !=
           0; remainingArmyAssets--) {
        if (((ArmyAssetRecord *)reverseFactionRecord->primaryArmyAssetPointersOrIds[counterOrValue])->
            definitionClassValue74 == 0) {
          factionOrDemand = factionOrDemand + 16;
        }
        else {
          factionOrDemand = factionOrDemand + 80;
        }
        counterOrValue++;
      }
      supplyOrSwapValue = reverseFactionRecord->tritiumCurrentQ4 * 16 +
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
      /* consumers of this faction in priority order; runtime +0xEC bit 0 = unpowered */
      do {
        factionOrDemand = entryCursor[2];
        if (valueOrFactionIndex == entryCursor[1]) {
          supplyOrSwapValue = *entryCursor;
          if (remainingEnergy < factionOrDemand) {
            sortBaseOrFlags = &((ModelRuntimeSlot *)supplyOrSwapValue)->classState.stateFlags;
            *sortBaseOrFlags = *sortBaseOrFlags | 1;
            reverseFactionRecord->unpoweredEnergyDemandQ4 =
                 reverseFactionRecord->unpoweredEnergyDemandQ4 + factionOrDemand;
          }
          else {
            remainingEnergy = remainingEnergy - factionOrDemand;
            reverseFactionRecord->suppliedEnergyDemandQ4 =
                 reverseFactionRecord->suppliedEnergyDemandQ4 + factionOrDemand;
            sortBaseOrFlags = &((ModelRuntimeSlot *)supplyOrSwapValue)->classState.stateFlags;
            *sortBaseOrFlags = *sortBaseOrFlags & ~1u;
          }
        }
        levelConditionStorage = g_InGameLevelRuntimeGlobalBlock.conditionStorage;
        entryCursor = entryCursor + 4;
        remainingEntries--;
      } while (remainingEntries != 0);
      /* energy above the baseline supply is Tritium burnt (see the end of the loop) */
      factionOrDemand = reverseFactionRecord->suppliedEnergyDemandQ4;
      supplyOrSwapValue = factionOrDemand - reverseFactionRecord->baselineEnergySupplyQ4;
      if (factionOrDemand < reverseFactionRecord->baselineEnergySupplyQ4) {
        supplyOrSwapValue = 0;
      }
      if (reverseFactionRecord->unpoweredEnergyDemandQ4 == 0) {
        reverseFactionRecord->anchorCooldown1 = 0;
      }
      else if ((g_InGameRuntimeRoot->worldRuntime).activeFactionRuntimeIndex == valueOrFactionIndex) {
        queueCapacityNotification = false;
        if (reverseFactionRecord->energyGenerationCapacityQ4 <
            factionOrDemand + reverseFactionRecord->unpoweredEnergyDemandQ4) {
          if (reverseFactionRecord->anchorCooldown1 == 0) {
            notificationMovieId = 400;
            reverseFactionRecord->anchorCooldown1 = 150;
            queueCapacityNotification = true;
          }
        }
        else if (reverseFactionRecord->anchorCooldown2 == 0) {
          notificationMovieId = 401;
          reverseFactionRecord->anchorCooldown2 = 150;
          queueCapacityNotification = true;
        }
        if (queueCapacityNotification) {
          /* Level header text starting with UTF-16 "t00_tu": a fixed notification, and both cooldowns never
             expire. */
          if (((*(int *)(levelConditionStorage->levelImage).header.opaque100_16F == 0x300074) &&
              (*(int *)((levelConditionStorage->levelImage).header.opaque100_16F + 4) == 0x5f0030)) &&
             (*(int *)((levelConditionStorage->levelImage).header.opaque100_16F + 8) == 0x750074)) {
            notificationMovieId = 402;
            reverseFactionRecord->anchorCooldown1 = INT32_MAX;
            reverseFactionRecord->anchorCooldown2 = INT32_MAX;
          }
          InGameNotificationQueue_InsertPriorityRecord(NOTIFICATION_PAYLOAD_NONE,0,0,0,0,0,3,notificationMovieId);
        }
      }
      reverseFactionRecord->tritiumCurrentQ4 =
           reverseFactionRecord->tritiumCurrentQ4 - (supplyOrSwapValue >> 4) * g_InGameSimulationStepTicks;
      reverseFactionRecord--;
      valueOrFactionIndex--;
    } while (valueOrFactionIndex != 0);
  }
  /* 4. every 128 steps: stat table row simulationTick / 128 (0x1000 rows of 7 factions x 2 dwords), the two
     metrics combinedProgressScore/activeArmyContribution clamped at zero */
  if ((g_GameFactionRuntimeImage.tail.simulationTick & 0x78) == 0) {
    counterOrValue = (int)&g_GameFactionRuntimeImage.records[1];
    worldRuntime = &g_InGameRuntimeRoot->worldRuntime;
    if (g_GameFactionRuntimeImage.tail.simulationTick >> 7 < 4096) {
      entryCountOrValue = 1;
      runtimeOrStatCursor = (int *)((g_GameFactionRuntimeImage.tail.simulationTick >> 7) * 56 +
                       (int)g_GameStatTableImage);
      do {
        GameFactionRuntime_RecomputeProgressAndScoreMetrics(entryCountOrValue,worldRuntime);
        cellCountOrValue = ((GameFactionRuntimeRecord *)counterOrValue)->combinedProgressScore;
        resourceOffsetOrValue = ((GameFactionRuntimeRecord *)counterOrValue)->activeArmyContribution;
        if (cellCountOrValue < 0) {
          cellCountOrValue = 0;
        }
        if (resourceOffsetOrValue < 0) {
          resourceOffsetOrValue = 0;
        }
        *runtimeOrStatCursor = cellCountOrValue;
        runtimeOrStatCursor[1] = resourceOffsetOrValue;
        entryCountOrValue++;
        counterOrValue = counterOrValue + sizeof(GameFactionRuntimeRecord);
        runtimeOrStatCursor = runtimeOrStatCursor + 2;
      } while (entryCountOrValue < 8);
    }
  }
  return;
}


/* Address: 0x0053D4F0.
   Stores the field-grid cell under the target position of the in-game world motion (the cursor/view target)
   and, unless automatic rotation or zoom is switched off in the map settings,
        copies its heading and a zoom value derived from the committed distance
   (distance * 3/128) into the in-game root's view cache.
*/
void InGameRuntime_UpdateCursorGridAndViewScaleCache(void)

{
  UQ12 committedDistance;
  uint32_t viewSettings;
  FieldGridCoordinates cursorGridPosition;
  InGameRuntimeRoot *inGameRoot;
  
  inGameRoot = g_InGameRuntimeRoot;
  cursorGridPosition = FieldGrid_WorldToGridQ12
                    ((g_InGameRuntimeRoot->worldRuntime).motion.targetPositionYQ12,
                     (g_InGameRuntimeRoot->worldRuntime).motion.targetPositionXQ12);
  inGameRoot->minimapOriginGridPosition =
       THANDOR_BITCAST(FieldGridCoordinates, FixedPlanarPointEdxEax8, cursorGridPosition);
  viewSettings = PersistentSettings_Read(0,PERSISTENT_SETTING_MAP_MOUSE_OPTION_FLAGS);
  committedDistance = (inGameRoot->worldRuntime).motion.committedDistanceQ12;
  if ((viewSettings & PERSISTENT_MAP_OPTION_AUTOMATIC_ROTATION_OFF) == 0) {
    inGameRoot->minimapRotationAngle =
         (inGameRoot->worldRuntime).motion.headingAngle;
  }
  if ((viewSettings & PERSISTENT_MAP_OPTION_AUTOMATIC_ZOOM_OFF) == 0) {
    /* high dword of distance * 0x6000000 (FIXED_MUL_HIGH) */
    inGameRoot->minimapSampleScaleQ12 =
         FIXED_MUL_HIGH((int)committedDistance,0x6000000);
  }
  return;
}


/* Address: 0x005651A0.
   Called before the session shutdown: remembers which info text the world view shows (text resource
   0x112..0x117, cycled by the player) in the in-game template and in the frontend template's status text, so the
   choice survives the next copy of the templates.
*/
void InGameRuntime_SaveWorldViewInfoTextChoice(UiRootNode *inGameRoot)

{
  g_InGameTemplateWorldViewInfoTextResourceId =
       (TextResourceId)((UiSingleLineTextControl *)INGAME_UI(inGameRoot,worldViewCyclingInfoText))->text;
  g_FrontendTemplateStatusTextResourceId = g_InGameTemplateWorldViewInfoTextResourceId;
  return;
}


/* Address: 0x00565E30.
   One simulation step of the running game: the heart of the game loop. It is not called from a fixed place in
   the frame; the UI runtime calls it as its synchronization hook (UiRuntime_SetSynchronizationHooks) every time
   it releases the in-game tick lock, and the loading loops and movie playback call it directly. Pacing therefore
   happens here: the call is ignored while the lock is held, while the simulation is more than two steps ahead of
   the renderer (g_InGamePendingSimulationTicks, two are consumed per drawn frame) and until the 80 Hz periodic
   timer has counted g_InGameNetworkTickCountdown down to zero (at most 20 steps per second).

   Order of one step:
   1. Network lockstep (every g_SessionNetworkTickInterval steps is an interval boundary):
      - host: takes in the peers' command submissions (FrontendTransfer_HostHandleCommandSubmitOrWaitAck),
        broadcasts the collected command batch once every peer has submitted (at the boundary it waits for
        the peers until then) and at the boundary executes the staged batch
        (FrontendTransfer_DispatchStagedCommandRecords: player commands take effect here);
      - client: at the boundary waits for the host's batch and executes it
        (FrontendNetwork_HandleCommandBatchAndPlayerTimeout);
      - single player: only the timer pacing. A step that has to wait returns before anything below.
   2. Nothing more while the session waits for players or is paused (UI_COMMAND_RUNTIME_FLAG_WAITING_FOR_PLAYERS,
      UI_COMMAND_RUNTIME_FLAG_PAUSED); otherwise simulationTick is advanced.
   3. Per-entity update: every node of the world owner list (models = units and buildings, shots, effects)
      is dispatched through g_RuntimeMaintenanceCallbackPhases.primaryUpdate[ownerClassId]
      (ArmyRuntimeMaintenance_UpdateHierarchyAiAndTimers, ShotModelRuntimeMaintenance_UpdateProjectile...,
      EffectModelRuntimeMaintenance_UpdateLifecycle...): unit hierarchy, AI and timers, projectile motion and
      hits, effect lifetimes.
   4. Every 20 steps: the level's scripted conditions and end triggers
      (InGameConditionRuntime_UpdateScheduledRecords).
   5. One of eight world jobs by simulationTick & 7, so each runs every 8th step:
      0 faction economy (resources, energy), faction metric cache, technology sync, terrain lighting;
      1 and 5 terrain height relaxation (forward / reverse); 2 and 6 faction AI planning;
      3 field-grid clamp and per-entity terrain state (terrainStateRefresh callbacks);
      4 influence distance bands; 7 occupancy rebuild (occupancyRebuild and terrainStateRefresh callbacks).
   While g_UiCommandRuntimeFlags bit 2 (0x04, origin not identified) is set, steps 2-5 are replaced by a
   reduced update that ignores pause and only
   re-seats every model on the terrain every second step (g_ArmyPlacementContactKindDispatchTable) and refreshes
   the influence / classification grids every 16th step.
*/
void InGameRuntime_UpdateSimulationAndNetworkTick(void)

{
  uint32_t modelDefinition;
  uint32_t tickPhase;
  WorldRuntimeContext *worldRuntime;
  WorldOwnerListNode *worldNode;
  ModelRuntimeNode *modelNode;
  bool callResult;
  RecordRingDiscardResult ringRecord;
  InGameRuntimeRoot *inGameRoot;

  callResult = g_SpinLockTryAcquire(&g_InGameStateTickSpinLock);
  inGameRoot = g_InGameRuntimeRoot;
  if (callResult) {
    return;
  }
  if ((g_UiCommandRuntimeFlags & UI_COMMAND_RUNTIME_FLAG_WAITING_FOR_PLAYERS) == 0) {
    /* do not run ahead of the renderer by more than a few steps */
    if (2 < (int)g_InGamePendingSimulationTicks) goto release_tick_lock;
    g_InGamePendingSimulationTicks++;
  }
  if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_CLIENT) == SESSION_NETWORK_ROLE_LOCAL) {
    /* host or single player */
    if (g_InGameNetworkTickCountdown != 0) goto release_tick_lock;
    g_InGameNetworkTickCountdown = INGAME_TIMER_TICKS_PER_SIMULATION_STEP;
    if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_HOST) != SESSION_NETWORK_ROLE_LOCAL) {
      if (g_SessionNetworkTickCounter % g_SessionNetworkTickInterval == 0) {
        /* interval boundary: the batch must be out before it is executed, else wait for the peers */
        while (g_HostCommandBatchSyncSentThisInterval == 0) {
          ringRecord = UiRuntimeRecordRing_DiscardOldest();
          if (ringRecord.empty) {
            callResult = FrontendTransfer_BroadcastPendingCommandBatchAndSyncState(1);
            if (callResult) goto release_tick_lock;
            break;
          }
          FrontendTransfer_HostHandleCommandSubmitOrWaitAck
                    ((NetworkSessionContext *)ringRecord.endpointOrReadIndex,
                     (FrontendTransferPacketUnion *)ringRecord.payloadOrReadIndex);
        }
        FrontendTransfer_DispatchStagedCommandRecords();
        g_HostCommandBatchSyncSentThisInterval = 0;
      }
      else {
        /* within the interval: handle sync requests and broadcast the batch as soon as every peer is ready */
        while( true ) {
          ringRecord = UiRuntimeRecordRing_DiscardOldest();
          if (ringRecord.empty) break;
          FrontendTransfer_HostHandleCommandSubmitOrWaitAck
                    ((NetworkSessionContext *)ringRecord.endpointOrReadIndex,
                     (FrontendTransferPacketUnion *)ringRecord.payloadOrReadIndex);
        }
        if ((g_HostCommandBatchSyncSentThisInterval == 0) &&
           (callResult = FrontendTransfer_BroadcastPendingCommandBatchAndSyncState(0), !callResult)) {
          g_HostCommandBatchSyncSentThisInterval = 1;
        }
      }
    }
  }
  else {
    if (g_SessionNetworkTickCounter % g_SessionNetworkTickInterval == 0) {
      /* client at an interval boundary: wait for the host's command batch, then execute it */
      callResult = UiRuntimeRecordRing_ContainsId(g_FrontendSessionToken);
      if (!callResult) goto release_tick_lock;
      do {
        ringRecord = UiRuntimeRecordRing_DiscardOldest();
        if (ringRecord.empty) break;
        callResult = FrontendNetwork_HandleCommandBatchAndPlayerTimeout
                          ((NetworkSessionContext *)ringRecord.endpointOrReadIndex,
                           (FrontendTransferPacketUnion *)ringRecord.payloadOrReadIndex);
      } while (!callResult);
      callResult = FrontendTransfer_ConsumeProcessedFlag();
      if (callResult) goto release_tick_lock;
    }
    else if (g_InGameNetworkTickCountdown != 0) goto release_tick_lock;
    g_InGameNetworkTickCountdown = INGAME_TIMER_TICKS_PER_SIMULATION_STEP;
  }
  g_SessionNetworkTickCounter++;
  if ((g_UiCommandRuntimeFlags & UI_COMMAND_RUNTIME_FLAG_INTERACTION_SUBSYSTEM_ACTIVE) == 0) {
    if ((g_UiCommandRuntimeFlags &
        (UI_COMMAND_RUNTIME_FLAG_WAITING_FOR_PLAYERS | UI_COMMAND_RUNTIME_FLAG_PAUSED)) == 0) {
      g_GameFactionRuntimeImage.tail.simulationTick++;
      worldRuntime = &inGameRoot->worldRuntime;
      tickPhase = g_GameFactionRuntimeImage.tail.simulationTick & 7;
      /* per-entity update of every model, shot and effect, dispatched by owner class */
      for (worldNode = (inGameRoot->worldRuntime).ownerListHead;
          worldNode != NULL; worldNode = worldNode->nextNode) {
        (*(&g_RuntimeMaintenanceCallbackPhases.primaryUpdate.army)[worldNode->ownerClassId])
                  (worldRuntime,worldNode);
      }
      if (g_GameFactionRuntimeImage.tail.simulationTick % 20 == 0) {
        InGameConditionRuntime_UpdateScheduledRecords();
      }
      /* the world jobs: one per step, each runs every 8th step */
      switch(tickPhase) {
      case 0:
        InGameRuntime_UpdateFactionResourceExtractionAndEnergyAllocationState();
        FrontendRuntime_UpdateCurrentFactionMetricCache();
        GameFactionRuntime_SynchronizeTechnologiesForRelationStates8To10();
        WorldLightingRuntime_UpdateInterpolatedTerrainLighting();
        break;
      case 1:
        TerrainGrid_RelaxNeighborHeightsForwardWithSignGate
                  ((inGameRoot->worldRuntime).fieldGrid);
        break;
      case 2:
        AiFactionRuntime_RebuildPlanningCapacityState();
        break;
      case 3:
        modelNode = (ModelRuntimeNode *)(inGameRoot->worldRuntime).ownerListHead;
        FieldGrid_ApplyByteClampLookupToCells
                  ((inGameRoot->worldRuntime).activeFactionRuntimeIndex,
                   (inGameRoot->worldRuntime).fieldGrid);
        if (modelNode != NULL) {
          do {
            (*(&g_RuntimeMaintenanceCallbackPhases.terrainStateRefresh.army)
              [modelNode->ownerClassId])(worldRuntime,modelNode);
            modelNode = (ModelRuntimeNode *)(modelNode->common).nextNode;
          } while (modelNode != NULL);
          FieldGrid_ApplyByteClampLookupToCells
                    ((inGameRoot->worldRuntime).activeFactionRuntimeIndex,
                     (inGameRoot->worldRuntime).fieldGrid);
        }
        break;
      case 4:
        GridInfluence_ClearDistanceBandsAndRefreshEntities
                  ((inGameRoot->worldRuntime).ownerListHead);
        break;
      case 5:
        TerrainGrid_RelaxNeighborHeightsReverseWithSignGate
                  ((inGameRoot->worldRuntime).fieldGrid);
        break;
      case 6:
        AiFactionRuntime_RebuildPlanningCapacityState();
        break;
      case 7:
        worldNode = (inGameRoot->worldRuntime).ownerListHead;
        FieldGrid_ClearOccupancyMaskBits0To6AllCells((inGameRoot->worldRuntime).fieldGrid);
        if ((g_UiCommandRuntimeFlags & UI_COMMAND_RUNTIME_FLAG_LOCAL_FACTION_ENDED) != 0) {
          FieldGrid_SetOccupancyMaskByteBit0AllCells
                    ((inGameRoot->worldRuntime).activeFactionRuntimeIndex,
                     (inGameRoot->worldRuntime).fieldGrid);
        }
        if (worldNode != NULL) {
          do {
            (*(&g_RuntimeMaintenanceCallbackPhases.occupancyRebuild.army)[worldNode->ownerClassId])
                      (worldRuntime,worldNode);
            worldNode = worldNode->nextNode;
          } while (worldNode != NULL);
          modelNode = (ModelRuntimeNode *)(inGameRoot->worldRuntime).ownerListHead;
          do {
            (*(&g_RuntimeMaintenanceCallbackPhases.terrainStateRefresh.army)
              [modelNode->ownerClassId])(worldRuntime,modelNode);
            modelNode = (ModelRuntimeNode *)(modelNode->common).nextNode;
          } while (modelNode != NULL);
          FieldGrid_ApplyByteClampLookupToCells
                    ((inGameRoot->worldRuntime).activeFactionRuntimeIndex,
                     (inGameRoot->worldRuntime).fieldGrid);
          GridScratch_PropagateFieldOccupancyMaskNeighborhood
                    ((inGameRoot->worldRuntime).fieldGrid);
        }
      }
    }
  }
  else {
    /* reduced update while flag 0x04 is set (also runs while paused) */
    modelNode = (ModelRuntimeNode *)(inGameRoot->worldRuntime).ownerListHead;
    g_GameFactionRuntimeImage.tail.simulationTick++;
    if ((g_GameFactionRuntimeImage.tail.simulationTick & 1) == 0) {
      for (; modelNode != NULL;
          modelNode = (ModelRuntimeNode *)(modelNode->common).nextNode) {
        if (modelNode->ownerClassId == WORLD_OWNER_RUNTIME_MODEL) {
          modelDefinition = (((modelNode->runtimePayload).modelRuntime)->definitionOrSavedId).savedIdOrOffset;
          g_ArmyPlacementContactKindDispatchTable.callbacks
          [((ModelDefinition *)modelDefinition)->placementContactKindIndex]
                    (((ModelDefinition *)modelDefinition)->placementHeightOffsetQ12,
                     (modelNode->worldTransform).translation.y,
                     (modelNode->worldTransform).translation.x,modelNode,
                     &inGameRoot->worldRuntime);
          ModelNodeRuntime_RebuildTransformsFromRoot(modelNode);
        }
      }
      if ((g_GameFactionRuntimeImage.tail.simulationTick & 0xc) == 0) {
        if ((g_GameFactionRuntimeImage.tail.simulationTick & 2) == 0) {
          GridInfluence_ClearDistanceBandsAndRefreshEntities
                    ((inGameRoot->worldRuntime).ownerListHead);
        }
        else {
          GridScratch_RebuildTerrainAndRuntimeClassificationMasks(&inGameRoot->worldRuntime);
        }
      }
    }
  }
release_tick_lock:
  g_SpinLockRelease(&g_InGameStateTickSpinLock);
  return;
}


/* Address: 0x0050E0B0.
   Optional initialisation step of new and loaded sessions; it always succeeds (CF clear), so the callers' failure
   branches never run. The unreachable CF-set epilogue at 0x0050E0C4 is not part of the function.
*/
uint8_t InGameRuntime_InitializeOptionalSubsystemAlwaysSuccess(uint32_t unusedArgument)

{
  return 0;
}


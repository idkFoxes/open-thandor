/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/src/ui/ingame/root_frame.cpp
 * Reverse engineering by idkFoxes 2026
 */

#include <thandor/ui/ingame/root_frame.h>
#include <thandor/thandor.h>
#include <thandor/platform/bootstrap/image.h>
#include <thandor/platform/debug/hooks.h>

/* Module data. */

THANDOR_ALIGN(16) UiRootCallbacks g_InGameUiRootCallbacks = {
    .frameUpdate = UI_SLOT(InGameUiRoot_UpdateFrame),
    .keyboardFallback = UI_SLOT(InGameHotkeys_DispatchCommandByFlags)};

SoundVoice *g_InGameActiveEffectVoice = nullptr;

uint32_t g_InGameEffectsEnabled = 0;

SoundVoice *g_InGameActiveMusicVoice = nullptr;

uint32_t g_InGameMusicNextTrackCountdown = 0;

uint16_t g_InGameCountdownTextUtf16[8] = {};

/* The largest countdown that fits g_InGameCountdownTextUtf16: "9999:59" plus the terminator. */
#define INGAME_COUNTDOWN_MAX_SECONDS (9999u * 60u + 59u)

/* Placement overlay: grey the field and mark where the pending army asset fits; refreshed every 8th simulation
   tick, removed once the placement ends. */
static void InGameUiRoot_UpdatePlacementOverlay(InGameRuntimeRootFrameView *inGameRoot)

{
  if (!Any(g_UiCommandRuntimeFlags & UI_COMMAND_RUNTIME_FLAG_PLACEMENT_OVERLAY_SHOWN)) {
    if (Any(g_UiCommandRuntimeFlags & UI_COMMAND_RUNTIME_FLAG_PLACEMENT_PENDING)) {
      g_UiCommandRuntimeFlags = g_UiCommandRuntimeFlags | UI_COMMAND_RUNTIME_FLAG_PLACEMENT_OVERLAY_SHOWN;
      FieldGrid_SetAllCellOverlayColors(INGAME_PLACEMENT_OVERLAY_ARGB,(inGameRoot->worldRuntime).fieldGrid);
      WorldRuntime_EmitModelDefinitionOverlayForMatchingEntries
                (THANDOR_PTR(g_InGamePendingPlacementArmyAsset),&inGameRoot->worldRuntime);
    }
  }
  else if (!Any(g_UiCommandRuntimeFlags & UI_COMMAND_RUNTIME_FLAG_PLACEMENT_PENDING)) {
    g_UiCommandRuntimeFlags = g_UiCommandRuntimeFlags & ~UI_COMMAND_RUNTIME_FLAG_PLACEMENT_OVERLAY_SHOWN;
    FieldGrid_SetAllCellOverlayColors(ARGB8888_OPAQUE_WHITE,(inGameRoot->worldRuntime).fieldGrid);
  }
  else if ((g_GameFactionRuntimeImage.tail.simulationTick & 7) == 0) {
    FieldGrid_SetAllCellOverlayColors(INGAME_PLACEMENT_OVERLAY_ARGB,(inGameRoot->worldRuntime).fieldGrid);
    WorldRuntime_EmitModelDefinitionOverlayForMatchingEntries
              (THANDOR_PTR(g_InGamePendingPlacementArmyAsset),&inGameRoot->worldRuntime);
  }
}

/* Keeps the camera target within 16 cells of the field: clamps the grid position, converts it back to world
   coordinates and moves target and camera by the difference. */
static void InGameUiRoot_KeepCameraTargetNearField(InGameRuntimeRootFrameView *inGameRoot)

{
  WorldRuntimeContext *worldRuntime;
  WorldMotionState *cameraMotion;
  FieldGridAsset *cameraFieldGrid;
  FieldGridCoordinates targetGridPosition;
  int64_t projectedProduct;
  int targetColumnQ12;
  int targetRowQ12;
  int gridColumn;
  int gridRow;
  int outOfBoundsAxisCount;
  int deltaX;
  int deltaY;

  worldRuntime = &inGameRoot->worldRuntime;
  cameraMotion = &worldRuntime->motion;
  cameraFieldGrid = worldRuntime->fieldGrid;
  outOfBoundsAxisCount = 0;
  targetGridPosition = FieldGrid_WorldToGridQ12(cameraMotion->targetPositionYQ12,cameraMotion->targetPositionXQ12);
  targetRowQ12 = targetGridPosition.rowQ12;
  targetColumnQ12 = targetGridPosition.columnQ12;
  gridColumn = (targetColumnQ12 >> Q12_SHIFT) - 8;
  gridRow = (targetRowQ12 >> Q12_SHIFT) - 8;
  if (gridColumn < -16) {
    targetColumnQ12 = -8 * Q12_ONE;
    outOfBoundsAxisCount = 1;
  }
  else if ((int)cameraFieldGrid->gridWidth < gridColumn) {
    outOfBoundsAxisCount = 1;
    targetColumnQ12 = (cameraFieldGrid->gridWidth + 8) * Q12_ONE;
  }
  if (gridRow < -16) {
    targetRowQ12 = -8 * Q12_ONE;
    outOfBoundsAxisCount++;
  }
  else if ((int)cameraFieldGrid->gridHeight < gridRow) {
    outOfBoundsAxisCount++;
    targetRowQ12 = (cameraFieldGrid->gridHeight + 8) * Q12_ONE;
  }
  if (outOfBoundsAxisCount != 0) {
    projectedProduct = (int64_t)(targetRowQ12 + targetColumnQ12 * 2) * FIELD_GRID_WORLD_COLUMN_STEP_X;
    deltaX = FIXED_PRODUCT_SHR(projectedProduct,13) - cameraMotion->targetPositionXQ12;
    deltaY = FIXED_PRODUCT_SHR(((int64_t)targetRowQ12 * -1999),Q12_SHIFT) - cameraMotion->targetPositionYQ12;
    cameraMotion->targetPositionXQ12 = cameraMotion->targetPositionXQ12 + deltaX;
    cameraMotion->targetPositionYQ12 = cameraMotion->targetPositionYQ12 + deltaY;
    cameraMotion->positionXQ12 = cameraMotion->positionXQ12 + deltaX;
    cameraMotion->positionYQ12 = cameraMotion->positionYQ12 + deltaY;
    WorldRuntime_ClearFieldGridDirtyFlag(worldRuntime);
  }
}

/* Ambient effect sounds (only called with the effects sound option on). Every 8th frame the spatial sound gains of
   all world objects are recomputed. g_InGameEffectsEnabled counts frames until the next ambient effect sound: at 0
   it waits for the current one to end and starts a new delay of 1..64 frames; when it counts down to 0 one of the
   four level effects plays. */
static void InGameUiRoot_UpdateEffectSounds
          (InGameRuntimeRootFrameView *inGameRoot,InGamePresentationTick currentPresentationTick)

{
  WorldRuntimeContext *worldRuntime;
  WorldOwnerListNode *ownerNode;
  uint32_t randomValue;
  uint32_t effectsGain;
  SoundVoice *playedVoice;

  worldRuntime = &inGameRoot->worldRuntime;
  if ((currentPresentationTick & 7) == 0) {
    SpatialSoundPool_ClearDesiredGains();
    for (ownerNode = worldRuntime->ownerListHead; ownerNode != nullptr; ownerNode = ownerNode->nextNode) {
      /* the callback of the node's owner class (model, shot or effect) */
      (*(&g_RuntimeMaintenanceCallbackPhases.audioRefresh.army)[ownerNode->ownerClassId])(worldRuntime,ownerNode);
    }
    SpatialSoundPool_ApplyDesiredGains();
    InGameSelectionDetailPanel_Rebuild();
  }
  if (g_InGameEffectsEnabled == 0) {
    if (g_SoundIsVoiceFinished(g_InGameActiveEffectVoice)) {
      g_InGameActiveEffectVoice = nullptr;
      randomValue = Random_NextPrimary();
      g_InGameEffectsEnabled = (randomValue & INGAME_AMBIENT_SOUND_DELAY_MASK) + 1;
    }
  }
  else {
    g_InGameEffectsEnabled--;
    if (g_InGameEffectsEnabled == 0) {
      effectsGain = PersistentSettings_Read(PERSISTENT_DEFAULT_GAIN_Q15,PERSISTENT_SETTING_EFFECTS_GAIN);
      randomValue = Random_NextPrimary();
      if (g_SoundPlayOneShot
                    (effectsGain,effectsGain,
                     g_InGameLevelEffectVoiceSets[randomValue & 3],&playedVoice)) {
        g_InGameActiveEffectVoice = playedVoice;
      }
    }
  }
}

/* Music (with the music sound option on): same delay scheme as the ambient effect sounds, then the best-suited of
   the four level tracks plays; nothing plays when no track scores above 0. */
static void InGameUiRoot_UpdateMusic(WorldRuntimeContext *worldRuntime)

{
  uint32_t soundOptionFlags;
  InGameLevelConditionStorage *levelConditionStorage;
  uint32_t randomValue;
  uint32_t trackIndex;
  uint32_t trackScore;
  uint32_t bestTrackIndex;
  uint32_t bestTrackScore;
  uint32_t musicGain;
  LevelMusicSampleNumber selectedMusicTrackId;
  SoundVoice *playedVoice;

  soundOptionFlags = PersistentSettings_Read(PERSISTENT_SOUND_OPTION_DEFAULT,PERSISTENT_SETTING_SOUND_OPTION_FLAGS);
  levelConditionStorage = g_InGameLevelRuntimeGlobalBlock.conditionStorage;
  if ((soundOptionFlags & PERSISTENT_SOUND_OPTION_MUSIC) == 0) {
    return;
  }
  if (g_InGameMusicNextTrackCountdown == 0) {
    if (g_SoundIsVoiceFinished(g_InGameActiveMusicVoice)) {
      g_InGameActiveMusicVoice = nullptr;
      randomValue = Random_NextPrimary();
      g_InGameMusicNextTrackCountdown = (randomValue & INGAME_AMBIENT_SOUND_DELAY_MASK) + 1;
    }
    return;
  }
  g_InGameMusicNextTrackCountdown--;
  if (g_InGameMusicNextTrackCountdown != 0) {
    return;
  }
  bestTrackScore = 0;
  bestTrackIndex = 0;
  for (trackIndex = 0; trackIndex < 4; trackIndex++) {
    trackScore = InGameMusic_ComputeTrackSuitabilityScore
                      ((levelConditionStorage->levelImage).worldSettings.musicSampleNumbers[trackIndex],worldRuntime);
    if ((int)bestTrackScore < (int)trackScore) {
      bestTrackIndex = trackIndex;
      bestTrackScore = trackScore;
    }
  }
  if (bestTrackScore != 0) {
    selectedMusicTrackId = (levelConditionStorage->levelImage).worldSettings.musicSampleNumbers[bestTrackIndex];
    musicGain = PersistentSettings_Read(PERSISTENT_DEFAULT_GAIN_Q15,PERSISTENT_SETTING_MUSIC_GAIN);
    g_EndGameResultsCurrentMusicTrackId = selectedMusicTrackId;
    if (g_SoundPlayOneShot
                  (musicGain,musicGain,g_InGameLevelMusicVoiceSets[bestTrackIndex],
                   &playedVoice)) {
      g_InGameActiveMusicVoice = playedVoice;
    }
  }
}

/* Camera keys: arrows scroll by the configured step, Page Up/Down tilt, Insert/Delete rotate, Home/End zoom
   (0x400 = 1/64 turn, 0x800 = 0.5 in Q12). */
static void InGameUiRoot_ApplyCameraKeys(InGameRuntimeRootFrameView *inGameRoot)

{
  WorldRuntimeContext *worldRuntime;
  WorldMotionState *cameraMotion;
  uint32_t scrollStep;
  AngleTurn32 clampedPitchAngle;
  UQ12 clampedTargetDistance;

  worldRuntime = &inGameRoot->worldRuntime;
  cameraMotion = &worldRuntime->motion;
  if (g_KeyboardSpecialKeyDown[KEYBOARD_SPECIAL_KEY_LEFT] != 0) {
    scrollStep = PersistentSettings_Read(PERSISTENT_DEFAULT_CAMERA_SCROLL_STEP,PERSISTENT_SETTING_CAMERA_SCROLL_STEP);
    WorldRuntime_TranslateCameraByScreenDelta(0,-scrollStep,worldRuntime);
    WorldRuntime_RecomputeMotionEndpointAgainstFieldSurface(worldRuntime);
  }
  if (g_KeyboardSpecialKeyDown[KEYBOARD_SPECIAL_KEY_RIGHT] != 0) {
    scrollStep = PersistentSettings_Read(PERSISTENT_DEFAULT_CAMERA_SCROLL_STEP,PERSISTENT_SETTING_CAMERA_SCROLL_STEP);
    WorldRuntime_TranslateCameraByScreenDelta(0,scrollStep,worldRuntime);
    WorldRuntime_RecomputeMotionEndpointAgainstFieldSurface(worldRuntime);
  }
  if (g_KeyboardSpecialKeyDown[KEYBOARD_SPECIAL_KEY_UP] != 0) {
    scrollStep = PersistentSettings_Read(PERSISTENT_DEFAULT_CAMERA_SCROLL_STEP,PERSISTENT_SETTING_CAMERA_SCROLL_STEP);
    WorldRuntime_TranslateCameraByScreenDelta(-scrollStep,0,worldRuntime);
    WorldRuntime_RecomputeMotionEndpointAgainstFieldSurface(worldRuntime);
  }
  if (g_KeyboardSpecialKeyDown[KEYBOARD_SPECIAL_KEY_DOWN] != 0) {
    scrollStep = PersistentSettings_Read(PERSISTENT_DEFAULT_CAMERA_SCROLL_STEP,PERSISTENT_SETTING_CAMERA_SCROLL_STEP);
    WorldRuntime_TranslateCameraByScreenDelta(scrollStep,0,worldRuntime);
    WorldRuntime_RecomputeMotionEndpointAgainstFieldSurface(worldRuntime);
  }
  if (g_KeyboardSpecialKeyDown[KEYBOARD_SPECIAL_KEY_PAGE_UP] != 0) {
    clampedPitchAngle = cameraMotion->pitchAngle - INGAME_CAMERA_KEY_ANGLE_STEP;
    if ((int)clampedPitchAngle < (int)cameraMotion->minimumPitchAngle) {
      clampedPitchAngle = cameraMotion->minimumPitchAngle;
    }
    WorldRuntime_PointCameraAtTarget
              (clampedPitchAngle,cameraMotion->headingAngle,cameraMotion->targetDistanceQ12,
               cameraMotion->targetPositionZQ12,cameraMotion->targetPositionYQ12,cameraMotion->targetPositionXQ12,
               worldRuntime);
  }
  if (g_KeyboardSpecialKeyDown[KEYBOARD_SPECIAL_KEY_PAGE_DOWN] != 0) {
    clampedPitchAngle = cameraMotion->pitchAngle + INGAME_CAMERA_KEY_ANGLE_STEP;
    if ((int)cameraMotion->maximumPitchAngle < (int)clampedPitchAngle) {
      clampedPitchAngle = cameraMotion->maximumPitchAngle;
    }
    WorldRuntime_PointCameraAtTarget
              (clampedPitchAngle,cameraMotion->headingAngle,cameraMotion->targetDistanceQ12,
               cameraMotion->targetPositionZQ12,cameraMotion->targetPositionYQ12,cameraMotion->targetPositionXQ12,
               worldRuntime);
  }
  if (g_KeyboardSpecialKeyDown[KEYBOARD_SPECIAL_KEY_INSERT] != 0) {
    WorldRuntime_PointCameraAtTarget
              (cameraMotion->pitchAngle,
               (cameraMotion->headingAngle - INGAME_CAMERA_KEY_ANGLE_STEP) & FIXED_ANGLE16_MASK,
               cameraMotion->targetDistanceQ12,cameraMotion->targetPositionZQ12,cameraMotion->targetPositionYQ12,
               cameraMotion->targetPositionXQ12,worldRuntime);
  }
  if (g_KeyboardSpecialKeyDown[KEYBOARD_SPECIAL_KEY_DELETE] != 0) {
    WorldRuntime_PointCameraAtTarget
              (cameraMotion->pitchAngle,
               (cameraMotion->headingAngle + INGAME_CAMERA_KEY_ANGLE_STEP) & FIXED_ANGLE16_MASK,
               cameraMotion->targetDistanceQ12,cameraMotion->targetPositionZQ12,cameraMotion->targetPositionYQ12,
               cameraMotion->targetPositionXQ12,worldRuntime);
  }
  if (g_KeyboardSpecialKeyDown[KEYBOARD_SPECIAL_KEY_HOME] != 0) {
    clampedTargetDistance = cameraMotion->targetDistanceQ12 - INGAME_CAMERA_KEY_DISTANCE_STEP_Q12;
    if ((int)clampedTargetDistance < (int)worldRuntime->minimumCameraDistanceQ12) {
      clampedTargetDistance = worldRuntime->minimumCameraDistanceQ12;
    }
    WorldRuntime_PointCameraAtTarget
              (cameraMotion->pitchAngle,cameraMotion->headingAngle,clampedTargetDistance,
               cameraMotion->targetPositionZQ12,cameraMotion->targetPositionYQ12,cameraMotion->targetPositionXQ12,
               worldRuntime);
  }
  if (g_KeyboardSpecialKeyDown[KEYBOARD_SPECIAL_KEY_END] != 0) {
    clampedTargetDistance = cameraMotion->targetDistanceQ12 + INGAME_CAMERA_KEY_DISTANCE_STEP_Q12;
    if ((int)worldRuntime->maximumCameraDistanceQ12 < (int)clampedTargetDistance) {
      clampedTargetDistance = worldRuntime->maximumCameraDistanceQ12;
    }
    WorldRuntime_PointCameraAtTarget
              (cameraMotion->pitchAngle,cameraMotion->headingAngle,clampedTargetDistance,
               cameraMotion->targetPositionZQ12,cameraMotion->targetPositionYQ12,cameraMotion->targetPositionXQ12,
               worldRuntime);
  }
}

/* Countdown text: the first of the 64 scheduled conditions that is a running countdown shows its remaining seconds
   as minutes:seconds; without one the timer node stays hidden. */
static void InGameUiRoot_UpdateCountdownText(InGameRuntimeRootFrameView *inGameRoot)

{
  InGameConditionSchedule *schedule;
  uint8_t *countdownText;
  uint32_t conditionIndex;
  uint32_t secondsLeft;
  uint32_t minutesByteLength;

  schedule = &g_InGameLevelRuntimeGlobalBlock.conditionStorage->schedule;
  countdownText = reinterpret_cast<uint8_t *>(g_InGameCountdownTextUtf16);
  inGameRoot->countdownPanelNodeFlags = inGameRoot->countdownPanelNodeFlags & ~UI_NODE_SUPPRESSED;
  for (conditionIndex = 0; conditionIndex < INGAME_SCHEDULED_CONDITION_COUNT; conditionIndex++) {
    if ((schedule->conditions[conditionIndex].statusAndKind.kind & INGAME_SCHEDULED_CONDITION_KIND_MASK) ==
        INGAME_SCHEDULED_CONDITION_COUNTDOWN_ELAPSED) {
      secondsLeft = schedule->conditions[conditionIndex].payload.operands[1];
      if (secondsLeft != 0) {
        /* The original formats any operand; from 600000 seconds on the minutes need five digits and the text
           runs past the 8 units. Clamped here to 9999:59 because the operand comes from level data. */
        if (INGAME_COUNTDOWN_MAX_SECONDS < secondsLeft) {
          static Bool8 s_countdownClampLogged = false;
          if (!s_countdownClampLogged) {
            Thandor_Log("countdown: %u seconds clamped to %u for display",secondsLeft,
                        INGAME_COUNTDOWN_MAX_SECONDS);
            s_countdownClampLogged = true;
          }
          secondsLeft = INGAME_COUNTDOWN_MAX_SECONDS;
        }
        minutesByteLength = g_WideNumberFormatUtf16
                           (WIDE_FORMAT_PAD_WITH_SPACE,0,2,1,secondsLeft / 60,reinterpret_cast<uint16_t *>(countdownText));
        *reinterpret_cast<uint16_t *>(countdownText + minutesByteLength) = ':';
        g_WideNumberFormatUtf16
                  (WIDE_FORMAT_WRITE_TERMINATOR|WIDE_FORMAT_PAD_WITH_ZERO,0,2,1,secondsLeft % 60,
                   reinterpret_cast<uint16_t *>(countdownText + minutesByteLength + 2));
        return;
      }
    }
  }
  inGameRoot->countdownPanelNodeFlags = inGameRoot->countdownPanelNodeFlags | UI_NODE_SUPPRESSED;
}

/* Frame update of the in-game UI root for the whole session: network session upkeep, the
   placement overlay, cursor frame and edge scrolling, keeping the camera target near the field, and, unless the
   interaction subsystem is active, ambient effect sounds, music selection, the camera keys, the countdown text and
   the terrain texture refresh. Nothing but the network upkeep runs while waiting for players.
*/

void InGameUiRoot_UpdateFrame(InGameRuntimeRootFrameView *inGameRoot)

{
  WorldRuntimeContext *worldRuntime;
  UiNodeBase *hoveredNode;
  uint32_t cursorFrame;
  uint32_t edgeScrollCursorFrame;
  uint32_t soundOptionFlags;
  uint32_t activePageIndex;
  InGamePresentationTick currentPresentationTick;

  if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_CLIENT) == SESSION_NETWORK_ROLE_LOCAL) {
    if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_HOST) != SESSION_NETWORK_ROLE_LOCAL) {
      FrontendHostSession_TickPeerTimeoutsAndDropPlayers();
    }
  }
  else {
    FrontendClientSession_TickHostTimeout();
  }
  if (!Any(g_UiCommandRuntimeFlags & UI_COMMAND_RUNTIME_FLAG_WAITING_FOR_PLAYERS)) {
    InGameUiRoot_UpdatePlacementOverlay(inGameRoot);
    cursorFrame = 0;
    hoveredNode = (*((inGameRoot->rootUi).base.vtable)->hitTest)
                       (g_CursorOverrideY,g_CursorOverrideX,&(inGameRoot->rootUi).base);
    if (hoveredNode != UI_NODE_NONE) { /* hit test found a node */
      cursorFrame = hoveredNode->vtable->pointerMove(g_CursorOverrideY,g_CursorOverrideX,hoveredNode);
    }
    g_GameFactionRuntimeImage.tail.presentationTick++;
    RecentTextHistory_SortAndBuildPointerList(8,&inGameRoot->recentTextHistory);
    currentPresentationTick = g_GameFactionRuntimeImage.tail.presentationTick;
    worldRuntime = &inGameRoot->worldRuntime;
    InGameHud_UpdateStatusCountersAndSessionPrompts();
    activePageIndex = UiPageStack_ActivePageIndex(&inGameRoot->worldViewAreaPageStack);
    /* edge scrolling only on the plain world view (no drag selection or notification jump, first page, no
       interaction node flag 8) and while cursor button bit 2 is up; its scroll-arrow frame wins over the hovered
       node's frame */
    if ((!Any(worldRuntime->runtimeFlags & (WORLD_RUNTIME_FLAG_DRAG_SELECTING | WORLD_RUNTIME_FLAG_NOTIFICATION_GOTO))) &&
        (activePageIndex == 0) && !Any(worldRuntime->interaction.nodeFlags & UI_NODE_SUPPRESSED) &&
        ((g_CursorButtonState & 4) == 0)) {
      edgeScrollCursorFrame = WorldRuntime_ApplyEdgeScrollAndGetCursorFrame(worldRuntime);
      if (edgeScrollCursorFrame != 0) {
        cursorFrame = edgeScrollCursorFrame;
      }
    }
    g_GraphicsCursorSetFrame(cursorFrame);
    InGameUiRoot_KeepCameraTargetNearField(inGameRoot);
    if (!Any(g_UiCommandRuntimeFlags & UI_COMMAND_RUNTIME_FLAG_INTERACTION_SUBSYSTEM_ACTIVE)) {
      TerrainDirectionTable_AdvanceAndRebuildVectors();
      soundOptionFlags =
           PersistentSettings_Read(PERSISTENT_SOUND_OPTION_DEFAULT,PERSISTENT_SETTING_SOUND_OPTION_FLAGS);
      if ((soundOptionFlags & PERSISTENT_SOUND_OPTION_EFFECTS) != 0) {
        InGameUiRoot_UpdateEffectSounds(inGameRoot,currentPresentationTick);
      }
      InGameUiRoot_UpdateMusic(worldRuntime);
      if (Any(g_UiCommandRuntimeFlags & UI_COMMAND_RUNTIME_FLAG_PAUSED)) {
        /* paused: no camera keys, countdown or terrain refresh */
        InGameRuntime_UpdateCursorGridAndViewScaleCache();
        return;
      }
      activePageIndex = UiPageStack_ActivePageIndex(&inGameRoot->gameWindowPageStack);
      if (activePageIndex == 2) {
        InGameTechnologyPanel_Rebuild(&inGameRoot->rootUi);
      }
      InterpolationStateTable_Advance256ByTicks(g_InGameSimulationStepTicks);
      InGameUiRoot_ApplyCameraKeys(inGameRoot);
      InGameUiRoot_UpdateCountdownText(inGameRoot);
      currentPresentationTick = g_GameFactionRuntimeImage.tail.presentationTick;
    }
    if ((currentPresentationTick & 31) == 0) {
      TerrainCompositeTexture_FillPlane1();
    }
    if ((currentPresentationTick & 3) == 0) {
      TerrainCompositeTexture_RebuildPlane0();
    }
  }
  InGameRuntime_UpdateCursorGridAndViewScaleCache();
}

/* Stores the field-grid cell under the target position of the in-game world motion (the cursor/view target)
   and, unless automatic rotation or zoom is switched off in the map settings,
        copies its heading and a zoom value derived from the committed distance
   (distance * 3/128) into the in-game root's view cache.
*/
void InGameRuntime_UpdateCursorGridAndViewScaleCache()

{
  UQ12 committedDistance;
  uint32_t viewSettings;
  FieldGridCoordinates cursorGridPosition;
  InGameRuntimeRoot *inGameRoot;
  
  inGameRoot = g_InGameRuntimeRoot;
  cursorGridPosition = FieldGrid_WorldToGridQ12
                    ((g_InGameRuntimeRoot->worldRuntime).motion.targetPositionYQ12,
                     (g_InGameRuntimeRoot->worldRuntime).motion.targetPositionXQ12);
  inGameRoot->minimapOriginGridPosition = cursorGridPosition;
  viewSettings = PersistentSettings_Read(0,PERSISTENT_SETTING_MAP_MOUSE_OPTION_FLAGS);
  committedDistance = (inGameRoot->worldRuntime).motion.committedDistanceQ12;
  if ((viewSettings & PERSISTENT_MAP_OPTION_AUTOMATIC_ROTATION_OFF) == 0) {
    inGameRoot->minimapRotationAngle =
         (inGameRoot->worldRuntime).motion.headingAngle;
  }
  if ((viewSettings & PERSISTENT_MAP_OPTION_AUTOMATIC_ZOOM_OFF) == 0) {
    /* high dword of distance * 0x6000000 (FIXED_MUL_HIGH) */
    inGameRoot->minimapSampleScaleQ12 =
         FIXED_MUL_HIGH((int)committedDistance,INGAME_MINIMAP_DISTANCE_SCALE_Q32);
  }
}

/* Called before the session shutdown: remembers which info text the world view shows (text resource
   0x112..0x117, cycled by the player) in the in-game template and in the frontend template's status text, so the
   choice survives the next copy of the templates.
*/
void InGameRuntime_SaveWorldViewInfoTextChoice(UiRootNode *inGameRoot)

{
  /* the text slots hold a TextResourceId (labelFlags & 0x10 clear), stored as the slot's 32 bits */
  g_InGameRuntimeDefaultImageTemplate.worldViewCyclingInfoText.text = THANDOR_PTR32_BITS(
       (TextResourceId)(uintptr_t)InGameUi_Image(inGameRoot)->worldViewCyclingInfoText.text);
  g_FrontendRootInitializationTemplate.bottomBarStatusText.text = THANDOR_PTR32_BITS(
       g_InGameRuntimeDefaultImageTemplate.worldViewCyclingInfoText.text.value);
}

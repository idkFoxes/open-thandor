/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/src/ui/ingame/commands.c
 * Reverse engineering by idkFoxes 2026
 */

#include <thandor/ui/ingame/commands.h>
#include <thandor/thandor.h>

/* Module data. */

/* uint32_t ARGB mask applied to terrain vertex diffuse colours (0x00FFFFFF raw, other value in masked command mode); its alpha byte also switches overlay/projection paths */
__declspec(align(4)) uint32_t g_UiCommandModeGColorVariantLimit = 16777215;

static int32_t g_UiCommandSpriteVariantAOffsets[24] = {
    /*  0 */ 36116, 36240, 36364, 36488, 36612, 36736, 36860, 36984,
    /*  8 */ 37108, 37232, 37356, 37480, 37604, 37728, 37852, 37976,
    /* 16 */ 38100, 38224, 38348, 38472, 38596, 38720, 38844, 38968};

static int32_t g_UiAction100AControlOffsets[8] = {45584, 45712, 45840, 45968, 46096, 46224, 46352, 46480};

static uint32_t g_UiCommandSpriteVariantAColumnCount = 0;

static int32_t *g_UiCommandSpriteVariantAOffsetTables[5] = {
    /* 0 */ (void *)&g_UiCommandSpriteVariantAOffsets,
    /* 1 */ (void *)&g_UiCommandSpriteVariantAOffsets,
    /* 2 */ (void *)&g_UiCommandSpriteVariantAOffsets,
    /* 3 */ (void *)&g_UiCommandSpriteVariantAOffsets,
    /* 4 */ (void *)&g_UiCommandSpriteVariantAOffsets};

static UiCommandRuntimeRecordPrefix *g_UiCommandSpriteVariantARecords[24] = {0};

static uint32_t g_UiCommandSelectionPageBaseIndex = 0;

static int32_t g_UiMappedCommandControlOffsets[12] = {42892, 43076, 43260, 43444, 43628, 43812, 43996, 44180, 44364, 44548, 44732, 44916};

/* uint32_t render-state flag word copied into terrain packets (primitives.c); ui/ingame/commands.c sets/clears the masked G-colour variant bit */
uint32_t g_UiCommandModeGColorVariantFlags = 0x10000;

uint32_t g_UiCommandModeE = 0;

uint32_t g_UiCommandModeA = 0;

uint32_t g_UiCommandModeB = 0;

uint32_t g_UiCommandModeF = 0;

/* Implementation ownership: ui/ingame/commands. */

/* Editor mode tab G0, terrain height tool (action 0x1100: g_InGameUiActionHandlersPage11[0],
   g_UiCommandModeGHandlers[0]; also called by the editor hotkeys in ui/ingame/runtime.c). Selects the tab, shows
   the tool's pages and switches the world view to the height tool overlays: surface point, terrain point, grid
   vertex and secondary surface markers, with the unmasked terrain colour ramp.
*/
void InGameCommandModeG_Select0(UiSelectableControl *source)

{
  InGameRuntimeRoot *runtimeRoot;
  WorldRuntimeContext *worldRuntime;

  runtimeRoot = UiCommandModeG_SelectAndSyncPages(EDITOR_MODE_TERRAIN_HEIGHT,source);
  worldRuntime = &runtimeRoot->worldRuntime;
  UiCommandModeG_ShowSurfacePointMarker(worldRuntime);
  UiCommandModeG_ShowTerrainPointMarkers(worldRuntime);
  UiCommandModeG_HideArmyMetricsAndEndDragSelect(worldRuntime);
  UiCommandModeG_ShowGridVertexMarkers(worldRuntime);
  UiCommandModeG_SetSecondarySurfaceOnly(worldRuntime);
  UiCommandModeG_ApplyRawColorVariant(worldRuntime);
  UiCommandModeG_HideRegionMarkers(worldRuntime);
  return;
}


/* Editor mode tab G1, terrain material tool (action 0x1101: g_InGameUiActionHandlersPage11[1],
   g_UiCommandModeGHandlers[1]; also called by the editor hotkeys in ui/ingame/runtime.c). Like G0, but without the
   secondary surface markers.
*/
void InGameCommandModeG_Select1(UiSelectableControl *source)

{
  InGameRuntimeRoot *runtimeRoot;
  WorldRuntimeContext *worldRuntime;

  runtimeRoot = UiCommandModeG_SelectAndSyncPages(EDITOR_MODE_TERRAIN_MATERIAL,source);
  worldRuntime = &runtimeRoot->worldRuntime;
  UiCommandModeG_ShowSurfacePointMarker(worldRuntime);
  UiCommandModeG_ShowTerrainPointMarkers(worldRuntime);
  UiCommandModeG_HideArmyMetricsAndEndDragSelect(worldRuntime);
  UiCommandModeG_ShowGridVertexMarkers(worldRuntime);
  UiCommandModeG_ClearSecondarySurfaceOnly(worldRuntime);
  UiCommandModeG_ApplyRawColorVariant(worldRuntime);
  UiCommandModeG_HideRegionMarkers(worldRuntime);
  return;
}


/* Editor mode tab G2, terrain smoothing tool (action 0x1102: g_InGameUiActionHandlersPage11[2],
   g_UiCommandModeGHandlers[2]; also called by the editor hotkeys in ui/ingame/runtime.c). Shows the surface point,
   grid vertex and secondary surface markers and is the only mode with the masked terrain colours
   (UiCommandModeG_ApplyMaskedColorVariant).
*/
void InGameCommandModeG_Select2(UiSelectableControl *source)

{
  InGameRuntimeRoot *runtimeRoot;
  WorldRuntimeContext *worldRuntime;

  runtimeRoot = UiCommandModeG_SelectAndSyncPages(EDITOR_MODE_TERRAIN_SMOOTHING,source);
  worldRuntime = &runtimeRoot->worldRuntime;
  UiCommandModeG_ShowSurfacePointMarker(worldRuntime);
  UiCommandModeG_HideTerrainPointMarkers(worldRuntime);
  UiCommandModeG_HideArmyMetricsAndEndDragSelect(worldRuntime);
  UiCommandModeG_ShowGridVertexMarkers(worldRuntime);
  UiCommandModeG_SetSecondarySurfaceOnly(worldRuntime);
  UiCommandModeG_ApplyMaskedColorVariant(worldRuntime);
  UiCommandModeG_HideRegionMarkers(worldRuntime);
  return;
}


/* Editor mode tab G3, unit placement tool (action 0x1105: g_InGameUiActionHandlersPage11[5],
   g_UiCommandModeGHandlers[3]; also called by the editor hotkeys in ui/ingame/runtime.c). Shows the army metrics
   and grid vertex markers and puts the army asset g_UiCommandModeGArmyAssetId into the selection detail panel;
   an unknown asset id is fatal.
*/
void InGameCommandModeG_Select3(UiSelectableControl *source)

{
  InGameRuntimeRoot *runtimeRoot;
  WorldRuntimeContext *worldRuntime;
  uint32_t lookupError;
  ArmyAssetRecordPrefix *armyRecord;
  uint32_t checkedAssetLookup;

  runtimeRoot = UiCommandModeG_SelectAndSyncPages(EDITOR_MODE_UNIT_PLACEMENT,source);
  worldRuntime = &runtimeRoot->worldRuntime;
  UiCommandModeG_HideSurfacePointMarker(worldRuntime);
  UiCommandModeG_HideTerrainPointMarkers(worldRuntime);
  UiCommandModeG_ShowArmyMetrics(worldRuntime);
  UiCommandModeG_ShowGridVertexMarkers(worldRuntime);
  UiCommandModeG_ClearSecondarySurfaceOnly(worldRuntime);
  UiCommandModeG_ApplyRawColorVariant(worldRuntime);
  UiCommandModeG_HideRegionMarkers(worldRuntime);
  lookupError = ArmyAssetRegistry_FindById(g_UiCommandModeGArmyAssetId,&armyRecord);
  checkedAssetLookup = FatalError_ExitIfFailed(lookupError != 0 ? lookupError : (uint32_t)armyRecord,
                                               lookupError != 0);
  g_UiHoverSelectionRecord = (UiCommandRuntimeRecordPrefix *)checkedAssetLookup;
  InGameSelectionDetailPanel_Rebuild();
  return;
}


/* Editor mode tab G4, object placement tool (action 0x1106: g_InGameUiActionHandlersPage11[6],
   g_UiCommandModeGHandlers[4]; also called by the editor hotkeys in ui/ingame/runtime.c). Same overlays as G3
   (army metrics and grid vertex markers) without the detail panel update.
*/
void InGameCommandModeG_Select4(UiSelectableControl *source)

{
  InGameRuntimeRoot *runtimeRoot;
  WorldRuntimeContext *worldRuntime;

  runtimeRoot = UiCommandModeG_SelectAndSyncPages(EDITOR_MODE_OBJECT_PLACEMENT,source);
  worldRuntime = &runtimeRoot->worldRuntime;
  UiCommandModeG_HideSurfacePointMarker(worldRuntime);
  UiCommandModeG_HideTerrainPointMarkers(worldRuntime);
  UiCommandModeG_ShowArmyMetrics(worldRuntime);
  UiCommandModeG_ShowGridVertexMarkers(worldRuntime);
  UiCommandModeG_ClearSecondarySurfaceOnly(worldRuntime);
  UiCommandModeG_ApplyRawColorVariant(worldRuntime);
  UiCommandModeG_HideRegionMarkers(worldRuntime);
  return;
}


/* Editor mode tab G5, region tool (action 0x1104: g_InGameUiActionHandlersPage11[4],
   g_UiCommandModeGHandlers[5]; also called by ui/ingame/runtime.c). Shows the surface point, army metrics, grid
   vertex and region markers; the region markers draw the variant chosen by mode F, so g_UiCommandModeF is copied
   into the world runtime (fieldRegion.regionToolMode) as well.
*/
void InGameCommandModeG_Select5(UiSelectableControl *source)

{
  InGameRuntimeRoot *runtimeRoot;
  WorldRuntimeContext *worldRuntime;

  runtimeRoot = UiCommandModeG_SelectAndSyncPages(EDITOR_MODE_REGION,source);
  worldRuntime = &runtimeRoot->worldRuntime;
  UiCommandModeG_ShowSurfacePointMarker(worldRuntime);
  UiCommandModeG_HideTerrainPointMarkers(worldRuntime);
  UiCommandModeG_ShowArmyMetrics(worldRuntime);
  UiCommandModeG_ShowGridVertexMarkers(worldRuntime);
  UiCommandModeG_ClearSecondarySurfaceOnly(worldRuntime);
  UiCommandModeG_ApplyRawColorVariant(worldRuntime);
  UiCommandModeG_ShowRegionMarkers(worldRuntime);
  (runtimeRoot->worldRuntime).fieldRegion.regionToolMode = g_UiCommandModeF;
  return;
}


/* Results screen continue button (action 0x101B, g_InGameUiActionHandlersPage10[27]). A local game or network host
   sets UI_COMMAND_RUNTIME_FLAG_RESULTS_CLOSED (the host through command 0x310 so every machine sees it); a network
   client instead reports itself ready, which lets the host show its own continue button.
*/
void InGameResultsScreen_ContinueOrMarkReady(void *source)

{
  if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_CLIENT) == SESSION_NETWORK_ROLE_LOCAL) {
    if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_NETWORKED_MASK) ==
        SESSION_NETWORK_ROLE_LOCAL) {
      UiCommandRuntimeFlags_ApplyClearSetToggleMasks
                (g_LocalPlayerRuntimeId,0,UI_COMMAND_RUNTIME_FLAG_RESULTS_CLOSED,0);
    }
    else {
      InGameCommandQueue_AppendLocalPlayerCommand
                (INGAME_COMMAND_APPLY_UI_FLAG_MASKS,0,UI_COMMAND_RUNTIME_FLAG_RESULTS_CLOSED,0);
    }
  }
  else if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_NETWORKED_MASK) ==
           SESSION_NETWORK_ROLE_LOCAL) {
    FrontendPlayerRuntime_MarkResultsReadyAndUpdateContinueButton(g_LocalPlayerRuntimeId);
  }
  else {
    InGameCommandQueue_AppendLocalPlayerCommand(INGAME_COMMAND_RESULTS_READY,0,0,0);
  }
  return;
}


/* End movie view click (action 0x1009, g_InGameUiActionHandlersPage10[9]): skips the end movie by clearing
   UI_COMMAND_RUNTIME_FLAG_END_MOVIE_PENDING (clear mask of command 0x310, sent to every machine in a network
   game).
*/
void InGameEndMovie_Skip(void *source)

{
  if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_NETWORKED_MASK) ==
      SESSION_NETWORK_ROLE_LOCAL) {
    UiCommandRuntimeFlags_ApplyClearSetToggleMasks
              (g_LocalPlayerRuntimeId,0,0,UI_COMMAND_RUNTIME_FLAG_END_MOVIE_PENDING);
  }
  else {
    InGameCommandQueue_AppendLocalPlayerCommand
              (INGAME_COMMAND_APPLY_UI_FLAG_MASKS,0,0,UI_COMMAND_RUNTIME_FLAG_END_MOVIE_PENDING);
  }
  return;
}


/* Quit game window restart button (action INGAME_ACTION_QUIT_RESTART_MISSION 0x1027,
   g_InGameUiActionHandlersPage10[39]): deselects and closes the in-game menu, then issues command 0x150 with
   INGAME_PLAYER_DEPARTURE_FLAG_CLOSE_SESSION, which ends the session.
*/
void InGameQuitMenu_RestartMission(UiNodeBase *source)

{

  /* source becomes the in-game UI root */
  while (source->parent != UI_NODE_NONE) {
    source = source->parent;
  }
  UiSelectableControl_SetSelected(0,(UiSelectableControl *)INGAME_UI(source,inGameMenuButton));
  InGameSettingsPage_ToggleAndSynchronizeControls((UiSelectableControl *)INGAME_UI(source,inGameMenuButton));
  if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_NETWORKED_MASK) ==
      SESSION_NETWORK_ROLE_LOCAL) {
    InGameCommand_HandlePlayerDeparture
              (g_LocalPlayerRuntimeId,0,0,INGAME_PLAYER_DEPARTURE_FLAG_CLOSE_SESSION);
  }
  else {
    InGameCommandQueue_AppendLocalPlayerCommand
              (INGAME_COMMAND_PLAYER_DEPARTURE,0,0,INGAME_PLAYER_DEPARTURE_FLAG_CLOSE_SESSION);
  }
  return;
}


/* Terrain material swatch click (action 0x1110, g_InGameUiActionHandlersPage11[16]): finds which of the
   twelve swatch controls (g_UiMappedCommandControlOffsets) was clicked and selects the material at that position
   of the current page. Clicks on other controls are ignored.
*/
void InGameCommandMatrix_SelectMappedControl(UiNodeBase *source)

{
  UiNodeBase *root;
  int mappingsRemaining;
  int mappingIndex;

  root = source;
  while (root->parent != UI_NODE_NONE) {
    root = root->parent;
  }
  for (mappingIndex = 0, mappingsRemaining = MATERIAL_SWATCH_COUNT; mappingsRemaining != 0;
       mappingIndex++, mappingsRemaining--) {
    if ((int)source - (int)root == g_UiMappedCommandControlOffsets[mappingIndex]) {
      UiCommandMatrix_SelectIndex(mappingIndex + g_UiCommandSelectionPageBaseIndex,root);
      return;
    }
  }
  return;
}


/* Pointer press of the command sprite buttons (nonRightPress and rightPress of g_UiNodeVtable_005162C0,
   g_UiNodeVtable_00516310 and g_UiCatalogEntryControlVtable): shows the button pressed and starts a new
   activationInputState, marking a double click when the node reports one.
*/
void UiCommandSpriteButtonControl_BeginPress
          (UiPointerWheelDelta wheelDelta,UiPixelCoordinate pointerY,UiPixelCoordinate pointerX,
          UiCommandSpriteButtonControl *control)

{
  UiSelectableStateFlags *stateFlagsField;
  
  if (((control->sprite).selectable.base.nodeFlags & UI_NODE_SUPPRESSED) == 0) {
    control->activationInputState = 0;
    stateFlagsField = &(control->sprite).selectable.stateFlags;
    *stateFlagsField = *stateFlagsField | UI_SELECTABLE_SELECTED_OR_CHECKED;
    UiNode_InvalidateRoot((UiNodeBase *)control);
    if (((control->sprite).selectable.base.nodeFlags & UI_NODE_REPEAT_OR_DOUBLE_CLICK) != 0) {
      control->activationInputState =
           control->activationInputState | UI_COMMAND_ACTIVATION_REPEAT_OR_DOUBLE_CLICK;
    }
  }
  return;
}


/* Left/middle button release of the command sprite buttons (nonRightRelease of g_UiNodeVtable_005162C0 and
   g_UiNodeVtable_00516310): when the press started on this button, adds the modifier keys held now to
   activationInputState, plays the activation sound if enabled and queues the button's action; the action
   handler reads activationInputState to choose what to do.
*/
void UiCommandSpriteButtonControl_NonRightRelease
          (UiPointerWheelDelta wheelDelta,UiPixelCoordinate pointerY,UiPixelCoordinate pointerX,
          UiCommandSpriteButtonControl *control)

{
  UiCommandActivationStateFlags inputStateBits;
  UiSelectableStateFlags *stateFlagsField;
  
  if ((((control->sprite).selectable.base.nodeFlags & UI_NODE_SUPPRESSED) == 0) &&
     (((control->sprite).selectable.stateFlags & UI_SELECTABLE_SELECTED_OR_CHECKED) != 0)) {
    inputStateBits = g_KeyboardStateMask &
            ~(UI_COMMAND_ACTIVATION_ALTERNATE_BUTTON|UI_COMMAND_ACTIVATION_REPEAT_OR_DOUBLE_CLICK);
    stateFlagsField = &(control->sprite).selectable.stateFlags;
    *stateFlagsField = *stateFlagsField & ~UI_SELECTABLE_SELECTED_OR_CHECKED;
    control->activationInputState = control->activationInputState | inputStateBits;
    if ((((control->sprite).selectable.stateFlags & UI_SPRITE_BUTTON_ACTIVATION_SOUND) != 0) &&
       ((control->sprite).activationSound != NULL)) {
      g_SoundPlayOneShot
                (g_UiSoundGainQ15,g_UiSoundGainQ15,
                 (control->sprite).activationSound,NULL);
    }
    UiActionQueue_Enqueue((control->sprite).selectable.actionId,control);
    UiNode_InvalidateRoot((UiNodeBase *)control);
  }
  return;
}


/* Right button release of the command sprite buttons (rightRelease of g_UiNodeVtable_005162C0,
   g_UiNodeVtable_00516310 and g_UiCatalogEntryControlVtable): like the left release, but replaces activationInputState
   with the modifier keys plus UI_COMMAND_ACTIVATION_ALTERNATE_BUTTON (which also drops the double-click marker).
*/
void UiCommandSpriteButtonControl_RightRelease
          (UiPointerWheelDelta wheelDelta,UiPixelCoordinate pointerY,UiPixelCoordinate pointerX,
          UiCommandSpriteButtonControl *control)

{
  UiCommandActivationStateFlags inputStateBits;

  if ((((control->sprite).selectable.base.nodeFlags & UI_NODE_SUPPRESSED) == 0) &&
      (((control->sprite).selectable.stateFlags & UI_SELECTABLE_SELECTED_OR_CHECKED) != 0)) {
    inputStateBits = g_KeyboardStateMask & ~UI_COMMAND_ACTIVATION_REPEAT_OR_DOUBLE_CLICK;
    (control->sprite).selectable.stateFlags &= ~UI_SELECTABLE_SELECTED_OR_CHECKED;
    control->activationInputState = inputStateBits | UI_COMMAND_ACTIVATION_ALTERNATE_BUTTON;
    if ((((control->sprite).selectable.stateFlags & UI_SPRITE_BUTTON_ACTIVATION_SOUND) != 0) &&
       ((control->sprite).activationSound != NULL)) {
      g_SoundPlayOneShot
                (g_UiSoundGainQ15,g_UiSoundGainQ15,
                 (control->sprite).activationSound,NULL);
    }
    UiActionQueue_Enqueue((control->sprite).selectable.actionId,control);
    UiNode_InvalidateRoot((UiNodeBase *)control);
  }
  return;
}


/* Pointer move over an army stock slot (pointerMove of g_UiNodeVtable_005162C0, which the seven diplomacy
   relation buttons share; for them no slot matches and only the cursor frame is returned): shows the slot's
   army asset in the selection detail panel and returns the cursor frame, 12 while Ctrl is held (a click then
   sells the army, see InGameArmyStock_TakeOrSellSlotArmy), 10 otherwise.
*/
GraphicsCursorFrameIndex InGameArmyStock_PointerMoveShowSlotDetails(UiPixelCoordinate pointerY,UiPixelCoordinate pointerX,
          UiCommandSpriteButtonControl *control)

{
  GraphicsCursorFrameIndex cursorFrame;
  int recordIndex;

  if (((control->sprite).selectable.base.nodeFlags & UI_NODE_SUPPRESSED) == 0) {
    recordIndex = ARMY_STOCK_ENTRY_COUNT - 1;
    do {
      if ((int)control - (int)g_InGameRuntimeRoot ==
          g_UiCommandSpriteVariantAOffsetTables[g_UiCommandSpriteVariantAColumnCount][recordIndex])
      {
        g_UiHoverSelectionRecord = g_UiCommandSpriteVariantARecords[recordIndex];
        InGameSelectionDetailPanel_Rebuild();
        break;
      }
      recordIndex--;
    } while (-1 < recordIndex);
  }
  cursorFrame = INGAME_CURSOR_FRAME_ARMY_STOCK;
  if ((g_KeyboardStateMask & KEYBOARD_STATE_CTRL) != 0) {
    cursorFrame = INGAME_CURSOR_FRAME_ARMY_STOCK_SELL;
  }
  return cursorFrame;
}


/* drawClipped of g_UiCommandVisibilityWrappedTextVtable (the wrapped world view status text): draws the text
   unless g_UiCommandRuntimeFlags bit 0x200 hides all these texts; with label flag 0x800 only while the game is
   paused.
*/
void UiCommandVisibilityWrappedText_DrawWhenAllowed
          (UiPixelCoordinate clipBottom,UiPixelCoordinate clipRight,UiPixelCoordinate clipTop,
          UiPixelCoordinate clipLeft,UiNodeBase *control)

{
  if ((g_UiCommandRuntimeFlags & UI_COMMAND_RUNTIME_FLAG_HIDE_WORLD_TEXTS) == 0 &&
      ((((UiWrappedTextControl *)control)->labelFlags & UI_WORLD_TEXT_PAUSED_ONLY) == 0 ||
       (g_UiCommandRuntimeFlags & UI_COMMAND_RUNTIME_FLAG_PAUSED) != 0) &&
      (control->nodeFlags & UI_NODE_SUPPRESSED) == 0) {
    UiWrappedTextControl_DrawClipped(clipBottom,clipRight,clipTop,clipLeft,(UiWrappedTextControl *)control);
  }
  return;
}


/* drawClipped of g_UiCommandVisibilitySingleLineTextVtable (the single-line world view texts): same visibility
   rules as the wrapped text; label flag 0x1000 additionally needs g_InGameSimulationStepTicks > 1 and draws the
   text shifted by ticks - 2 bytes (the text pointer is restored afterwards).
*/
void UiCommandVisibilitySingleLineText_DrawWhenAllowed
          (UiPixelCoordinate clipBottom,UiPixelCoordinate clipRight,UiPixelCoordinate clipTop,
          UiPixelCoordinate clipLeft,UiNodeBase *control)

{
  UiSingleLineTextControl *textControl;
  int drawOffsetAdjust;

  textControl = (UiSingleLineTextControl *)control;
  if ((g_UiCommandRuntimeFlags & UI_COMMAND_RUNTIME_FLAG_HIDE_WORLD_TEXTS) != 0) {
    return;
  }
  if ((textControl->labelFlags & UI_WORLD_TEXT_PAUSED_ONLY) != 0 &&
      (g_UiCommandRuntimeFlags & UI_COMMAND_RUNTIME_FLAG_PAUSED) == 0) {
    return;
  }
  drawOffsetAdjust = 0;
  if ((textControl->labelFlags & UI_WORLD_TEXT_SHIFT_BY_STEP_TICKS) != 0) {
    if (!(1 < g_InGameSimulationStepTicks)) {
      return;
    }
    drawOffsetAdjust = g_InGameSimulationStepTicks - 2;
  }
  textControl->text = (uint16_t *)((uint8_t *)textControl->text + drawOffsetAdjust);
  UiSingleLineTextControl_DrawClipped(clipBottom,clipRight,clipTop,clipLeft,textControl);
  textControl->text = (uint16_t *)((uint8_t *)textControl->text - drawOffsetAdjust);
  return;
}


/* In-game command handler 0x370 (key P): toggles the player's pause request, then toggles the global pause once
   every player agrees - the game pauses when all players request it and resumes when none does any more.
*/
void InGameCommand_TogglePauseRequest
          (PlayerRuntimeId playerRuntimeId,uint32_t callbackArg1,uint32_t callbackArg2,uint32_t callbackArg3)

{
  FrontendPlayerRuntimeBlockCount remainingPlayers;
  FrontendPlayerRuntimeRecord *playerRecord;

  g_SelectionPlayerRuntimeBlockPointers[playerRuntimeId]->sessionFlags =
       g_SelectionPlayerRuntimeBlockPointers[playerRuntimeId]->sessionFlags ^ PLAYER_SESSION_FLAG_PAUSE_REQUESTED;
  remainingPlayers = g_FrontendPlayerRuntimeBlockCount;
  playerRecord = g_FrontendPlayerRuntimeBlocks;
  do {
    if ((g_UiCommandRuntimeFlags & UI_COMMAND_RUNTIME_FLAG_PAUSED) == 0) {
      if ((g_SelectionPlayerRuntimeBlockPointers[playerRecord->playerRuntimeId]->sessionFlags &
           PLAYER_SESSION_FLAG_PAUSE_REQUESTED) == 0) {
        return;
      }
    }
    else if ((g_SelectionPlayerRuntimeBlockPointers[playerRecord->playerRuntimeId]->sessionFlags &
              PLAYER_SESSION_FLAG_PAUSE_REQUESTED) != 0) {
      return;
    }
    playerRecord++;
    remainingPlayers--;
  } while (remainingPlayers != 0);
  g_UiCommandRuntimeFlags = g_UiCommandRuntimeFlags ^ UI_COMMAND_RUNTIME_FLAG_PAUSED;
  return;
}


/* In-game command handler INGAME_COMMAND_PLACE_ARMY: takes the player's pending army asset (stock entry chosen
   in the army stock panel), validates the placement at the clicked point and creates the army there with the
   given heading, counts it for the faction and spawns the asset's placement effect. A rejected placement leaves
   the asset pending; a successful one ends the local placement mode.
*/
void InGameCommand_ExecuteLocalPlacementFromSelection(PlayerRuntimeId playerId,CommandPayload headingAngle,
          CommandPayload worldXQ12,CommandPayload worldYQ12)

{
  FactionRelationCounter *relationCounter;
  uint32_t pendingEntry;
  uint32_t ownerFactionIndex;
  SelectionPlayerRuntimeBlock *playerBlock;
  ArmyRuntimeSlot *modelNodeRuntime;
  ArmyRuntimeSlot *armySlot;
  ModelRuntimeSlot *slotModelRuntime;
  InGameRuntimeRoot *runtimeRoot;
  ArmyRuntimeSlot **createdArmySlots;
  WorldRuntimeContext *worldRuntime;
  Bool8 placementRejected;

  runtimeRoot = g_InGameRuntimeRoot;
  playerBlock = g_SelectionPlayerRuntimeBlockPointers[playerId];
  worldRuntime = &g_InGameRuntimeRoot->worldRuntime;
  /* take the pending entry and clear it atomically, as in the original */
  LOCK();
  pendingEntry = playerBlock->pendingPlacementArmyAsset;
  playerBlock->pendingPlacementArmyAsset = 0;
  UNLOCK();
  if (pendingEntry != 0) {
    /* the pending entry is the chosen army asset record */
    placementRejected = ArmyPlacement_ValidateAssetAtPointAndCellCorners
                      (0,headingAngle,worldXQ12,worldYQ12,
                       (ArmyPlacementContext)((ArmyAssetRecordPrefix *)pendingEntry)->registryId,playerBlock->factionIndex,
                       worldRuntime);
    if (!placementRejected) {
      /* the validator leaves the accepted (possibly snapped) point in g_ArmyPlacementValidatedWorldX/YQ12 */
      createdArmySlots = (ArmyRuntimeSlot **)ArmyRuntime_CreateInstanceFromAsset
                        (4,headingAngle,g_ArmyPlacementValidatedWorldYQ12,
                         g_ArmyPlacementValidatedWorldXQ12,
                         playerBlock->factionIndex,
                         ((ArmyAssetRecordPrefix *)pendingEntry)->registryId,worldRuntime,NULL);
      if (createdArmySlots != NULL) {
        ownerFactionIndex = playerBlock->factionIndex;
        modelNodeRuntime = createdArmySlots[1];
        armySlot = *createdArmySlots;
        modelNodeRuntime->movementPosition0Q12 = 0;
        if (ownerFactionIndex == (runtimeRoot->worldRuntime).activeFactionRuntimeIndex) {
          modelNodeRuntime->movementPosition0Q12 = INT32_MAX;
        }
        relationCounter = &g_GameFactionRuntimeImage.records[ownerFactionIndex].relationCounterB;
        *relationCounter = *relationCounter + 1;
        slotModelRuntime = (armySlot->modelRuntimeOrSavedOffset).modelRuntime;
        ModelNodeRuntime_RebuildTransformsFromRoot((ModelRuntimeNode *)modelNodeRuntime);
        ArmyRuntime_DispatchClassCommand((ArmyRuntimeSlot *)createdArmySlots,worldRuntime); /* the created army */
        EffectRuntimePool_CreateInstanceFromDefinition
                  (EFFECT_RUNTIME_COMPLETION_NONE,(EffectRuntimeOwnerReference){ .modelNode = NULL },
                   ((ModelRuntimeNode *)modelNodeRuntime)->modelPayload.worldRotationAngle2,
                   ((ModelRuntimeNode *)modelNodeRuntime)->modelPayload.worldRotationAngle1,
                   ((ModelRuntimeNode *)modelNodeRuntime)->modelPayload.worldRotationAngle0,
                   ((ModelRuntimeNode *)modelNodeRuntime)->worldTransform.translation.z,
                   ((ModelRuntimeNode *)modelNodeRuntime)->worldTransform.translation.y,
                   ((ModelRuntimeNode *)modelNodeRuntime)->worldTransform.translation.x,
                   (EffectDefinition *)slotModelRuntime->attachments[2].childLocalRotationAngle0,
                   worldRuntime);
        InGameBuildCatalog_RebuildGrid((UiNodeBase *)g_InGameRuntimeRoot);
        InGameSpecialBuildCatalog_RebuildGrid((UiNodeBase *)g_InGameRuntimeRoot);
        if (playerId != g_LocalPlayerRuntimeId) {
          return;
        }
        g_UiCommandRuntimeFlags = g_UiCommandRuntimeFlags & ~UI_COMMAND_RUNTIME_FLAG_PLACEMENT_PENDING;
        g_InGamePendingPlacementArmyAsset = 0;
        return;
      }
    }
    g_SelectionPlayerRuntimeBlockPointers[playerId]->pendingPlacementArmyAsset = pendingEntry;
  }
  return;
}


/* Rebuilds the army stock panel: the active faction's pooled army assets that have a texture (at most 24,
   none while the world input is disabled) fill g_UiCommandSpriteVariantARecords and the slot buttons in a
   grid of at most four columns; the frame is sized to the grid (smaller margins below 800 pixels width) and
   hidden when the stock is empty, unused slots are hidden.
*/
void InGameArmyStock_RebuildGrid(UiNodeBase *node)

{
  int32_t *offsetTable;
  uint32_t columnCount;
  GraphicsTextureSourceAsset *slotTexture;
  int remainingSlots;
  int panelWidth;
  int slotOffset;
  uint32_t itemCount;
  FactionArmyAssetCount remainingAssets;
  int panelHeight;
  uint32_t slotIndex;
  UiCommandRuntimeRecordPrefix **recordCursor;
  uint32_t *assetCursor;
  UiGridDimensions gridDimensions;

  /* node becomes the in-game UI root */
  while (node->parent != UI_NODE_NONE) {
    node = node->parent;
  }
  recordCursor = g_UiCommandSpriteVariantARecords;
  for (remainingSlots = ARMY_STOCK_ENTRY_COUNT; remainingSlots != 0; remainingSlots--) {
    *recordCursor = NULL;
    recordCursor++;
  }
  recordCursor = g_UiCommandSpriteVariantARecords;
  remainingAssets = g_GameFactionRuntimeImage.records[((WorldRuntimeContext *)INGAME_UI(node,worldView))->activeFactionRuntimeIndex].primaryArmyAssetCount;
  itemCount = 0;
  assetCursor = g_GameFactionRuntimeImage.records[((WorldRuntimeContext *)INGAME_UI(node,worldView))->activeFactionRuntimeIndex].primaryArmyAssetPointersOrIds;
  if ((g_UiCommandRuntimeFlags & UI_COMMAND_RUNTIME_FLAG_WORLD_INPUT_DISABLED) == 0) {
    for (; remainingAssets != 0; remainingAssets--) {
      if ((((UiCommandRuntimeRecordPrefix *)*assetCursor)->textureSource != NULL) && (itemCount < ARMY_STOCK_ENTRY_COUNT)) {
        *recordCursor = (UiCommandRuntimeRecordPrefix *)*assetCursor;
        itemCount++;
        recordCursor++;
      }
      assetCursor++;
    }
  }
  gridDimensions = UiGrid_ComputeDimensionsPacked(6,itemCount);
  columnCount = gridDimensions.columnCount;
  if (ARMY_STOCK_MAX_COLUMNS < columnCount) {
    columnCount = ARMY_STOCK_MAX_COLUMNS;
  }
  panelWidth = columnCount * g_InGamePanelTextureSubresource34Width + g_InGamePanelTextureSubresource27Width +
          g_InGamePanelTextureSubresource28Width;
  panelHeight = (int)gridDimensions.rowCount * g_InGamePanelTextureSubresource34Height +
          g_InGamePanelTextureSubresource26Height + g_InGamePanelTextureSubresource31Height;
  g_UiCommandSpriteVariantAColumnCount = columnCount;
  if ((int)g_FramebufferWidth < 800) {
    INGAME_UI(node,armyStockFrame)->leftOffset = -31;
    INGAME_UI(node,armyStockFrame)->rightOffset = -31;
    INGAME_UI(node,armyStockFrame)->topOffset = -13;
    INGAME_UI(node,armyStockFrame)->bottomOffset = -13;
  }
  else {
    INGAME_UI(node,armyStockFrame)->leftOffset = -39;
    INGAME_UI(node,armyStockFrame)->rightOffset = -39;
    INGAME_UI(node,armyStockFrame)->topOffset = -18;
    INGAME_UI(node,armyStockFrame)->bottomOffset = -18;
  }
  INGAME_UI(node,armyStockFrame)->leftOffset -= panelWidth;
  INGAME_UI(node,armyStockFrame)->topOffset -= panelHeight;
  if (itemCount == 0) {
    INGAME_UI(node,armyStockFrame)->nodeFlags |= UI_NODE_SUPPRESSED;
  }
  else {
    INGAME_UI(node,armyStockFrame)->nodeFlags &= ~UI_NODE_SUPPRESSED;
  }
  offsetTable = g_UiCommandSpriteVariantAOffsetTables[columnCount];
  for (slotIndex = 0; slotIndex < ARMY_STOCK_ENTRY_COUNT; slotIndex++) {
    slotOffset = offsetTable[slotIndex];
    if (slotIndex < itemCount) {
      THANDOR_UI_AT(node,slotOffset)->nodeFlags &= ~UI_NODE_SUPPRESSED;
      slotTexture = g_UiCommandSpriteVariantARecords[slotIndex]->textureSource;
    }
    else {
      THANDOR_UI_AT(node,slotOffset)->nodeFlags |= UI_NODE_SUPPRESSED;
      slotTexture = NULL;
    }
    ((UiCommandSpriteButtonControl *)THANDOR_UI_AT(node,slotOffset))->sprite.primaryTextureSource = slotTexture;
  }
  INGAME_UI(node,armyStockPanel)->vtable->layout(INGAME_UI(node,armyStockPanel));
  return;
}


/* Technology window close button (action 0x1011, g_InGameUiActionHandlersPage10[17]): shows the world view again,
   closes the window (page 0 of the game window page stack) and, if something is selected, sends
   INGAME_COMMAND_CLOSE_TECHNOLOGY_PAGE with -1 (cancel) for the first selected building, which gives back what
   opening the page took away.
*/
void InGameCommandAction_ClearSelectedArmyTokenAndClosePage(UiNodeBase *control)

{
  GameEntityRuntime *firstSelectedEntity;
  CommandPayload modelOffset;

  /* control becomes the in-game UI root */
  while (control->parent != UI_NODE_NONE) {
    control = control->parent;
  }
  INGAME_UI(control,worldView)->nodeFlags &= ~UI_NODE_SUPPRESSED;
  UiPageStack_SetActiveIndex(INGAME_WINDOW_PAGE_NONE,(UiPageStackControl *)INGAME_UI(control,gameWindowPageStack));
  firstSelectedEntity = SelectionInfo_GetFirstEntry();
  if (firstSelectedEntity != NULL) {
    modelOffset = (int)(firstSelectedEntity->common).ownership.definitionOrClassRecord -
                  g_ModelRuntimeRebaseDelta;
    if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_NETWORKED_MASK) ==
        SESSION_NETWORK_ROLE_LOCAL) {
      FrontendPlayerRuntime_ClearArmyTokenAndRestoreOrApplyTechnology
                (g_LocalPlayerRuntimeId,0,0xffffffff,modelOffset);
    }
    else {
      InGameCommandQueue_AppendLocalPlayerCommand(INGAME_COMMAND_CLOSE_TECHNOLOGY_PAGE,0,0xffffffff,modelOffset);
    }
  }
  return;
}


/* UI action 0x1200 (game menu quit button): opens the quit game window (page 4 of the in-game window page
   stack). Its restart button is only offered in local games, its surrender button only while the local
   faction is still in play (world input enabled).
*/
void InGameQuitMenu_OpenAndRefreshButtons(InGameCommandPanelSourceAddress32 source)

{
  UiNodeBase *firstNode;

  /* source is InGameUiImage.gameMenuQuitButton */
  UiPageStack_SetActiveIndex(INGAME_WINDOW_PAGE_QUIT_MENU,(UiPageStackControl *)
                             THANDOR_UI_SIBLING(source,InGameUiImage,gameMenuQuitButton,gameWindowPageStack));
  firstNode = THANDOR_UI_AT(source,-(int)offsetof(InGameUiImage,gameMenuQuitButton)); /* the in-game UI root */
  if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_NETWORKED_MASK) ==
      SESSION_NETWORK_ROLE_LOCAL) {
    UiNodeList_UnsuppressActionId(INGAME_ACTION_QUIT_RESTART_MISSION,firstNode);
  }
  else {
    UiNodeList_SuppressActionId(INGAME_ACTION_QUIT_RESTART_MISSION,firstNode);
  }
  if ((g_UiCommandRuntimeFlags & UI_COMMAND_RUNTIME_FLAG_WORLD_INPUT_DISABLED) == 0) {
    UiNodeList_UnsuppressActionId(INGAME_ACTION_QUIT_SURRENDER,firstNode);
  }
  else {
    UiNodeList_SuppressActionId(INGAME_ACTION_QUIT_SURRENDER,firstNode);
  }
  return;
}


/* Build catalog entry click (action 0x100B, g_InGameUiActionHandlersPage10[11]): finds the entry among the 48
   build catalog slots of the current column layout and queues its army asset for the active faction, or with Ctrl
   (activationInputState & KEYBOARD_STATE_CTRL) cancels a queued one with refund. Ignored while paused or while the
   world input is disabled.
*/
void InGameBuildCatalog_QueueOrCancelEntry(UiCatalogEntryControl *source)

{
  FactionRuntimeIndex factionIndex;
  UiCatalogEntryControl *root;
  PckArmyAssetIdCatalog assetId;
  int entryIndex;

  if ((g_UiCommandRuntimeFlags &
      (UI_COMMAND_RUNTIME_FLAG_PAUSED | UI_COMMAND_RUNTIME_FLAG_WORLD_INPUT_DISABLED)) == 0) {
    root = source;
    while ((root->command).sprite.selectable.base.parent != UI_NODE_NONE) {
      root = (UiCatalogEntryControl *)(root->command).sprite.selectable.base.parent;
    }
    entryIndex = BUILD_CATALOG_ENTRY_COUNT - 1;
    while ((int)source - (int)root !=
           g_UiCatalogGroup48OffsetTables[g_UiCatalogGroup48ColumnCount][entryIndex]) {
      entryIndex--;
      if (entryIndex < 0) {
        return;
      }
    }
    if (((source->command).activationInputState & UI_COMMAND_ACTIVATION_RELATION_RESET_REQUEST_MASK)
        == 0) {
      factionIndex = ((WorldRuntimeContext *)INGAME_UI(root,worldView))->activeFactionRuntimeIndex;
      assetId = g_UiCatalogGroup48Records[entryIndex]->armyAssetId;
      if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_NETWORKED_MASK) ==
          SESSION_NETWORK_ROLE_LOCAL) {
        GameFactionRuntime_RegisterArmyAssetPointers
                  (g_LocalPlayerRuntimeId,1,assetId,factionIndex);
      }
      else {
        InGameCommandQueue_AppendLocalPlayerCommand
                  (INGAME_COMMAND_QUEUE_ARMY,1,assetId,(CommandPayload)factionIndex);
      }
    }
    else {
      factionIndex = ((WorldRuntimeContext *)INGAME_UI(root,worldView))->activeFactionRuntimeIndex;
      assetId = g_UiCatalogGroup48Records[entryIndex]->armyAssetId;
      if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_NETWORKED_MASK) ==
          SESSION_NETWORK_ROLE_LOCAL) {
        GameFactionRuntime_CancelQueuedArmyAssetsAndRefund
                  (g_LocalPlayerRuntimeId,1,assetId,factionIndex);
      }
      else {
        InGameCommandQueue_AppendLocalPlayerCommand
                  (INGAME_COMMAND_CANCEL_QUEUED_ARMY,1,assetId,(CommandPayload)factionIndex);
      }
    }
  }
  return;
}


/* Special build catalog entry click (action 0x100C, g_InGameUiActionHandlersPage10[12]): the same as
   InGameBuildCatalog_QueueOrCancelEntry for the 42 slots of the special build catalog.
*/
void InGameSpecialBuildCatalog_QueueOrCancelEntry(UiCatalogEntryControl *source)

{
  FactionRuntimeIndex factionIndex;
  UiCatalogEntryControl *root;
  PckArmyAssetIdCatalog assetId;
  int entryIndex;

  if ((g_UiCommandRuntimeFlags &
      (UI_COMMAND_RUNTIME_FLAG_PAUSED | UI_COMMAND_RUNTIME_FLAG_WORLD_INPUT_DISABLED)) == 0) {
    root = source;
    while ((root->command).sprite.selectable.base.parent != UI_NODE_NONE) {
      root = (UiCatalogEntryControl *)(root->command).sprite.selectable.base.parent;
    }
    entryIndex = SPECIAL_BUILD_CATALOG_ENTRY_COUNT - 1;
    while ((int)source - (int)root !=
           g_UiCatalogGroup42OffsetTables[g_UiCatalogGroup42ColumnCount][entryIndex]) {
      entryIndex--;
      if (entryIndex < 0) {
        return;
      }
    }
    if (((source->command).activationInputState & UI_COMMAND_ACTIVATION_RELATION_RESET_REQUEST_MASK)
        == 0) {
      factionIndex = ((WorldRuntimeContext *)INGAME_UI(root,worldView))->activeFactionRuntimeIndex;
      assetId = g_UiCatalogGroup42Records[entryIndex]->armyAssetId;
      if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_NETWORKED_MASK) ==
          SESSION_NETWORK_ROLE_LOCAL) {
        GameFactionRuntime_RegisterArmyAssetPointers
                  (g_LocalPlayerRuntimeId,1,assetId,factionIndex);
      }
      else {
        InGameCommandQueue_AppendLocalPlayerCommand
                  (INGAME_COMMAND_QUEUE_ARMY,1,assetId,(CommandPayload)factionIndex);
      }
    }
    else {
      factionIndex = ((WorldRuntimeContext *)INGAME_UI(root,worldView))->activeFactionRuntimeIndex;
      assetId = g_UiCatalogGroup42Records[entryIndex]->armyAssetId;
      if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_NETWORKED_MASK) ==
          SESSION_NETWORK_ROLE_LOCAL) {
        GameFactionRuntime_CancelQueuedArmyAssetsAndRefund
                  (g_LocalPlayerRuntimeId,1,assetId,factionIndex);
      }
      else {
        InGameCommandQueue_AppendLocalPlayerCommand
                  (INGAME_COMMAND_CANCEL_QUEUED_ARMY,1,assetId,(CommandPayload)factionIndex);
      }
    }
  }
  return;
}


/* Army stock slot click (action 0x1001, g_InGameUiActionHandlersPage10[1]): first drops any army still waiting
   for placement (command 0x14F0), then takes the slot's army for placement on the map, or sells it with Ctrl
   (activationInputState & KEYBOARD_STATE_CTRL). Ignored while paused, while the world input is disabled and while
   world runtime flag 0x10 is set.
*/
void InGameArmyStock_TakeOrSellSlotArmy(UiCommandSpriteButtonControl *control)

{
  int32_t *flagsField;
  UiCommandSpriteButtonControl *root;
  UiCommandRuntimeRecordPrefix *runtimeRecord;
  FactionRuntimeIndex factionIndex;
  PckArmyAssetIdCatalog assetId;
  int slotIndex;

  if ((g_UiCommandRuntimeFlags &
      (UI_COMMAND_RUNTIME_FLAG_PAUSED | UI_COMMAND_RUNTIME_FLAG_WORLD_INPUT_DISABLED)) == 0) {
    root = control;
    while ((root->sprite).selectable.base.parent != UI_NODE_NONE) {
      root = (UiCommandSpriteButtonControl *)(root->sprite).selectable.base.parent;
    }
    /* end any hover of the stock panel (image control) */
    g_UiImageControlHoverTarget = NULL;
    flagsField = (int32_t *)&((UiImageControl *)INGAME_UI(root,armyStockPanel))->selectable.stateFlags;
    *flagsField = *flagsField & ~UI_IMAGE_CONTROL_HOVER_STATE_BITS;
    if ((((WorldRuntimeContext *)INGAME_UI(root,worldView))->runtimeFlags & WORLD_RUNTIME_FLAG_NOTIFICATION_GOTO) == 0) {
      slotIndex = ARMY_STOCK_ENTRY_COUNT - 1;
      while ((int)control - (int)root !=
             g_UiCommandSpriteVariantAOffsetTables[g_UiCommandSpriteVariantAColumnCount][slotIndex]) {
        slotIndex--;
        if (slotIndex < 0) {
          return;
        }
      }
      runtimeRecord = g_UiCommandSpriteVariantARecords[slotIndex];
      factionIndex = ((WorldRuntimeContext *)INGAME_UI(root,worldView))->activeFactionRuntimeIndex;
      if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_NETWORKED_MASK) ==
          SESSION_NETWORK_ROLE_LOCAL) {
        GameFactionRuntime_ConsumePendingArmyAssetAndRefreshGrid
                  (g_LocalPlayerRuntimeId,0,0,factionIndex);
      }
      else {
        InGameCommandQueue_AppendLocalPlayerCommand
                  (INGAME_COMMAND_CONSUME_PENDING_ARMY,0,0,(CommandPayload)factionIndex);
      }
      if ((control->activationInputState & UI_COMMAND_ACTIVATION_RELATION_RESET_REQUEST_MASK) == 0)
      {
        factionIndex = ((WorldRuntimeContext *)INGAME_UI(root,worldView))->activeFactionRuntimeIndex;
        assetId = runtimeRecord->armyAssetId;
        if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_NETWORKED_MASK) ==
            SESSION_NETWORK_ROLE_LOCAL) {
          GameFactionRuntime_RemoveArmyAssetAndStagePlayerTransfer
                    (g_LocalPlayerRuntimeId,0,assetId,factionIndex);
        }
        else {
          InGameCommandQueue_AppendLocalPlayerCommand
                    (INGAME_COMMAND_TAKE_ARMY_FOR_PLACEMENT,0,assetId,(CommandPayload)factionIndex);
        }
      }
      else {
        factionIndex = ((WorldRuntimeContext *)INGAME_UI(root,worldView))->activeFactionRuntimeIndex;
        assetId = runtimeRecord->armyAssetId;
        if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_NETWORKED_MASK) ==
            SESSION_NETWORK_ROLE_LOCAL) {
          GameFactionRuntime_SellArmyAssetAndRefundSevenEighths
                    (g_LocalPlayerRuntimeId,0,assetId,factionIndex);
        }
        else {
          InGameCommandQueue_AppendLocalPlayerCommand
                    (INGAME_COMMAND_SELL_ARMY,0,assetId,(CommandPayload)factionIndex);
        }
      }
    }
  }
  return;
}


/* Selection group button click (action 0x100A, g_InGameUiActionHandlersPage10[10]; the 8 buttons of
   g_UiAction100AControlOffsets): the mouse version of the 1..8 group keys. A plain click recalls the group, a
   modifier key merges (SELECTION_TRANSFER_MERGE), the right button stores the selection into the group
   (SELECTION_TRANSFER_TO_GROUP) and a double click also centres the view. Every variant except the plain recall
   is refused when SelectionInfo_AllEntriesEmptyOrMatchOwner reports so for the active faction.
*/
void InGameSelectionGroupButton_RecallOrStoreGroup(UiCommandSpriteButtonControl *control)

{
  UiCommandSpriteButtonControl *root;
  FactionRuntimeIndex factionIndex;
  CommandPayload groupIndex;
  CommandPayload transferModeFlags;

  if ((g_UiCommandRuntimeFlags &
      (UI_COMMAND_RUNTIME_FLAG_PAUSED | UI_COMMAND_RUNTIME_FLAG_WORLD_INPUT_DISABLED)) != 0) {
    return;
  }
  root = control;
  while ((root->sprite).selectable.base.parent != UI_NODE_NONE) {
    root = (UiCommandSpriteButtonControl *)(root->sprite).selectable.base.parent;
  }
  /* find the group of the clicked button */
  groupIndex = SELECTION_GROUP_COUNT - 1;
  while ((int)control - (int)root != g_UiAction100AControlOffsets[groupIndex]) {
    groupIndex--;
    if ((int)groupIndex < 0) {
      return;
    }
  }
  transferModeFlags = 0;
  if ((control->activationInputState & UI_COMMAND_ACTIVATION_LOW_INPUT_NIBBLE_MASK) != 0) {
    transferModeFlags = SELECTION_TRANSFER_MERGE;
  }
  if ((control->activationInputState & UI_COMMAND_ACTIVATION_ALTERNATE_BUTTON) != 0) {
    transferModeFlags = transferModeFlags | SELECTION_TRANSFER_TO_GROUP;
  }
  if ((control->activationInputState & UI_COMMAND_ACTIVATION_REPEAT_OR_DOUBLE_CLICK) != 0) {
    transferModeFlags = transferModeFlags | SELECTION_TRANSFER_CENTER_VIEW;
  }
  if ((transferModeFlags != 0) &&
      SelectionInfo_AllEntriesEmptyOrMatchOwner
           ((FactionRuntimeIndex)((WorldRuntimeContext *)INGAME_UI(root,worldView))->activeFactionRuntimeIndex)) {
    return;
  }
  factionIndex = ((WorldRuntimeContext *)INGAME_UI(root,worldView))->activeFactionRuntimeIndex;
  if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_NETWORKED_MASK) != SESSION_NETWORK_ROLE_LOCAL) {
    InGameCommandQueue_AppendLocalPlayerCommand
              (INGAME_COMMAND_SELECTION_GROUP,(CommandPayload)factionIndex,transferModeFlags,groupIndex);
    return;
  }
  FrontendPlayerSelection_TransferFactionGroupWithModeAndRefresh
            (g_LocalPlayerRuntimeId,factionIndex,transferModeFlags,groupIndex);
  return;
}


/* Second results screen button (action 0x1025, g_InGameUiActionHandlersPage10[37]; resultsSecondaryExitButton,
   only offered in network games): sets UI_COMMAND_RUNTIME_FLAG_RESULTS_CLOSED on this machine only.
*/
void InGameResultsScreen_CloseLocally(UiNodeBase *source)

{
  g_UiCommandRuntimeFlags = g_UiCommandRuntimeFlags | UI_COMMAND_RUNTIME_FLAG_RESULTS_CLOSED;
  return;
}

/* Results chart mode buttons (action 0x1026, g_InGameUiActionHandlersPage10[38]): selects the clicked one of the
   two buttons and copies the chosen mode (0 or 1) into the modeFlags of the three results charts (graph or table
   drawing) and into the image subresource of the results screen background.
*/
void InGameCommandState_SelectAndPropagateBinaryMode(UiSelectableControl *source)

{
  UiSelectableControl *root;
  uint32_t selectedIndexValue;

  root = source;
  while ((root->base).parent != UI_NODE_NONE) {
    root = (UiSelectableControl *)(root->base).parent;
  }
  UiSelectableGroup_SelectExclusive(2,&source->base,
      INGAME_UI(root,resultsChartModeButtonB),
      INGAME_UI(root,resultsChartModeButtonA));
  /* Original quirk: the result is not tested; with no visible button selected the index is 2 */
  UiSelectableGroup_FindVisibleSelected(NULL,&selectedIndexValue,2,
      INGAME_UI(root,resultsChartModeButtonA),
      INGAME_UI(root,resultsChartModeButtonB));
  /* Mode 0/1 picks each chart's drawing path (modeFlags bit 0) and the results background image. */
  ((FrontendResultsColumnSequenceControl *)INGAME_UI(root,resultsChart1))->modeFlags =
       (uint32_t)selectedIndexValue;
  ((FrontendResultsColumnSequenceControl *)INGAME_UI(root,resultsChart2))->modeFlags =
       (uint32_t)selectedIndexValue;
  ((FrontendResultsColumnSequenceControl *)INGAME_UI(root,resultsChart3))->modeFlags =
       (uint32_t)selectedIndexValue;
  ((UiImagePanelControl *)INGAME_UI(root,resultsScreenPanel))->subresource =
       (GraphicsSubresourceIndex)selectedIndexValue;
  return;
}


/* Hides the grid vertex markers of the world view (clears WORLD_RUNTIME_FLAG_DRAW_GRID_VERTEX_MARKERS); called
   when the editor is switched off (InGameUiCommandRuntime_ApplyInteractionSubsystemActiveState).
*/
void UiCommandModeG_HideGridVertexMarkers(WorldRuntimeContext *context)

{
  context->runtimeFlags = context->runtimeFlags & ~WORLD_RUNTIME_FLAG_DRAW_GRID_VERTEX_MARKERS;
  return;
}


/* Height tool option 0 (action 0x1108, g_InGameUiActionHandlersPage11[8]; also the editor hotkeys in
   ui/ingame/runtime.c): selects heightToolOption0 among the four height tool buttons and sets g_UiCommandModeC = 0.
*/
void InGameCommandModeC_Select0(UiSpriteButtonControl *source)

{
  UiSelectableGroup_SelectExclusive(4,(UiNodeBase *)source,
      THANDOR_UI_SIBLING(source,InGameUiImage,heightToolOption0,heightToolOption3),
      THANDOR_UI_SIBLING(source,InGameUiImage,heightToolOption0,heightToolOption2),
      THANDOR_UI_SIBLING(source,InGameUiImage,heightToolOption0,heightToolOption1),
      THANDOR_UI_SIBLING(source,InGameUiImage,heightToolOption0,heightToolOption0));
  g_UiCommandModeC = 0;
  return;
}


/* Height tool option 1 (action 0x1109, g_InGameUiActionHandlersPage11[9]; also the editor hotkeys in
   ui/ingame/runtime.c): selects heightToolOption1 among the four height tool buttons and sets g_UiCommandModeC = 1.
*/
void InGameCommandModeC_Select1(UiSpriteButtonControl *source)

{
  UiSelectableGroup_SelectExclusive(4,(UiNodeBase *)source,
      THANDOR_UI_SIBLING(source,InGameUiImage,heightToolOption1,heightToolOption3),
      THANDOR_UI_SIBLING(source,InGameUiImage,heightToolOption1,heightToolOption2),
      THANDOR_UI_SIBLING(source,InGameUiImage,heightToolOption1,heightToolOption1),
      THANDOR_UI_SIBLING(source,InGameUiImage,heightToolOption1,heightToolOption0));
  g_UiCommandModeC = 1;
  return;
}


/* Height tool option 2 (action 0x110A, g_InGameUiActionHandlersPage11[10]; also the editor hotkeys in
   ui/ingame/runtime.c): selects heightToolOption2 among the four height tool buttons and sets g_UiCommandModeC = 2.
*/
void InGameCommandModeC_Select2(UiSpriteButtonControl *source)

{
  UiSelectableGroup_SelectExclusive(4,(UiNodeBase *)source,
      THANDOR_UI_SIBLING(source,InGameUiImage,heightToolOption2,heightToolOption3),
      THANDOR_UI_SIBLING(source,InGameUiImage,heightToolOption2,heightToolOption2),
      THANDOR_UI_SIBLING(source,InGameUiImage,heightToolOption2,heightToolOption1),
      THANDOR_UI_SIBLING(source,InGameUiImage,heightToolOption2,heightToolOption0));
  g_UiCommandModeC = 2;
  return;
}


/* Height tool option 3 (action 0x110B, g_InGameUiActionHandlersPage11[11]; also the editor hotkeys in
   ui/ingame/runtime.c): selects heightToolOption3 among the four height tool buttons and sets g_UiCommandModeC = 3.
*/
void InGameCommandModeC_Select3(UiSpriteButtonControl *source)

{
  UiSelectableGroup_SelectExclusive(4,(UiNodeBase *)source,
      THANDOR_UI_SIBLING(source,InGameUiImage,heightToolOption3,heightToolOption3),
      THANDOR_UI_SIBLING(source,InGameUiImage,heightToolOption3,heightToolOption2),
      THANDOR_UI_SIBLING(source,InGameUiImage,heightToolOption3,heightToolOption1),
      THANDOR_UI_SIBLING(source,InGameUiImage,heightToolOption3,heightToolOption0));
  g_UiCommandModeC = 3;
  return;
}


/* Material tool option 0 (action 0x110C, g_InGameUiActionHandlersPage11[12]; also the editor hotkeys in
   ui/ingame/runtime.c): selects materialToolOption0 among the four material tool buttons and sets
   g_UiCommandModeD = 0.
*/
void InGameCommandModeD_Select0(UiSpriteButtonControl *source)

{
  UiSelectableGroup_SelectExclusive(4,(UiNodeBase *)source,
      THANDOR_UI_SIBLING(source,InGameUiImage,materialToolOption0,materialToolOption3),
      THANDOR_UI_SIBLING(source,InGameUiImage,materialToolOption0,materialToolOption2),
      THANDOR_UI_SIBLING(source,InGameUiImage,materialToolOption0,materialToolOption1),
      THANDOR_UI_SIBLING(source,InGameUiImage,materialToolOption0,materialToolOption0));
  g_UiCommandModeD = 0;
  return;
}


/* Material tool option 1 (action 0x110D, g_InGameUiActionHandlersPage11[13]; also the editor hotkeys in
   ui/ingame/runtime.c): selects materialToolOption1 among the four material tool buttons and sets
   g_UiCommandModeD = 1.
*/
void InGameCommandModeD_Select1(UiSpriteButtonControl *source)

{
  UiSelectableGroup_SelectExclusive(4,(UiNodeBase *)source,
      THANDOR_UI_SIBLING(source,InGameUiImage,materialToolOption1,materialToolOption3),
      THANDOR_UI_SIBLING(source,InGameUiImage,materialToolOption1,materialToolOption2),
      THANDOR_UI_SIBLING(source,InGameUiImage,materialToolOption1,materialToolOption1),
      THANDOR_UI_SIBLING(source,InGameUiImage,materialToolOption1,materialToolOption0));
  g_UiCommandModeD = 1;
  return;
}


/* Material tool option 2 (action 0x110E, g_InGameUiActionHandlersPage11[14]; also the editor hotkeys in
   ui/ingame/runtime.c): selects materialToolOption2 among the four material tool buttons and sets
   g_UiCommandModeD = 2.
*/
void InGameCommandModeD_Select2(UiSpriteButtonControl *source)

{
  UiSelectableGroup_SelectExclusive(4,(UiNodeBase *)source,
      THANDOR_UI_SIBLING(source,InGameUiImage,materialToolOption2,materialToolOption3),
      THANDOR_UI_SIBLING(source,InGameUiImage,materialToolOption2,materialToolOption2),
      THANDOR_UI_SIBLING(source,InGameUiImage,materialToolOption2,materialToolOption1),
      THANDOR_UI_SIBLING(source,InGameUiImage,materialToolOption2,materialToolOption0));
  g_UiCommandModeD = 2;
  return;
}


/* Material tool option 3 (action 0x110F, g_InGameUiActionHandlersPage11[15]; also the editor hotkeys in
   ui/ingame/runtime.c): selects materialToolOption3 among the four material tool buttons and sets
   g_UiCommandModeD = 3.
*/
void InGameCommandModeD_Select3(UiSpriteButtonControl *source)

{
  UiSelectableGroup_SelectExclusive(4,(UiNodeBase *)source,
      THANDOR_UI_SIBLING(source,InGameUiImage,materialToolOption3,materialToolOption3),
      THANDOR_UI_SIBLING(source,InGameUiImage,materialToolOption3,materialToolOption2),
      THANDOR_UI_SIBLING(source,InGameUiImage,materialToolOption3,materialToolOption1),
      THANDOR_UI_SIBLING(source,InGameUiImage,materialToolOption3,materialToolOption0));
  g_UiCommandModeD = 3;
  return;
}


/* Unit placement option 0 (action 0x1111, g_InGameUiActionHandlersPage11[17]; also the editor hotkeys in
   ui/ingame/runtime.c): selects unitPlacementOption0 among the three unit placement buttons and sets
   g_UiCommandModeA = 0.
*/
void InGameCommandModeA_Select0(UiSpriteButtonControl *source)

{
  UiSelectableGroup_SelectExclusive(3,(UiNodeBase *)source,
      THANDOR_UI_SIBLING(source,InGameUiImage,unitPlacementOption0,unitPlacementOption1),
      THANDOR_UI_SIBLING(source,InGameUiImage,unitPlacementOption0,unitPlacementOption2),
      THANDOR_UI_SIBLING(source,InGameUiImage,unitPlacementOption0,unitPlacementOption0));
  g_UiCommandModeA = 0;
  return;
}


/* Unit placement option 1 (action 0x1112, g_InGameUiActionHandlersPage11[18]; also the editor hotkeys in
   ui/ingame/runtime.c): selects unitPlacementOption1 among the three unit placement buttons and sets
   g_UiCommandModeA = 1.
*/
void InGameCommandModeA_Select1(UiSpriteButtonControl *source)

{
  UiSelectableGroup_SelectExclusive(3,(UiNodeBase *)source,
      THANDOR_UI_SIBLING(source,InGameUiImage,unitPlacementOption1,unitPlacementOption1),
      THANDOR_UI_SIBLING(source,InGameUiImage,unitPlacementOption1,unitPlacementOption2),
      THANDOR_UI_SIBLING(source,InGameUiImage,unitPlacementOption1,unitPlacementOption0));
  g_UiCommandModeA = 1;
  return;
}


/* Unit placement option 2 (action 0x1113, g_InGameUiActionHandlersPage11[19]; also the editor hotkeys in
   ui/ingame/runtime.c): selects unitPlacementOption2 among the three unit placement buttons and sets
   g_UiCommandModeA = 2.
*/
void InGameCommandModeA_Select2(UiSpriteButtonControl *source)

{
  UiSelectableGroup_SelectExclusive(3,(UiNodeBase *)source,
      THANDOR_UI_SIBLING(source,InGameUiImage,unitPlacementOption2,unitPlacementOption1),
      THANDOR_UI_SIBLING(source,InGameUiImage,unitPlacementOption2,unitPlacementOption2),
      THANDOR_UI_SIBLING(source,InGameUiImage,unitPlacementOption2,unitPlacementOption0));
  g_UiCommandModeA = 2;
  return;
}


/* Object placement option 0 (action 0x1114, g_InGameUiActionHandlersPage11[20]; also the editor hotkeys
   in ui/ingame/runtime.c): selects objectPlacementOption0 among the three object placement buttons and sets
   g_UiCommandModeB = 0.
*/
void InGameCommandModeB_Select0(UiSpriteButtonControl *source)

{
  UiSelectableGroup_SelectExclusive(3,(UiNodeBase *)source,
      THANDOR_UI_SIBLING(source,InGameUiImage,objectPlacementOption0,objectPlacementOption1),
      THANDOR_UI_SIBLING(source,InGameUiImage,objectPlacementOption0,objectPlacementOption2),
      THANDOR_UI_SIBLING(source,InGameUiImage,objectPlacementOption0,objectPlacementOption0));
  g_UiCommandModeB = 0;
  return;
}


/* Object placement option 1 (action 0x1115, g_InGameUiActionHandlersPage11[21]; also the editor hotkeys
   in ui/ingame/runtime.c): selects objectPlacementOption1 among the three object placement buttons and sets
   g_UiCommandModeB = 1.
*/
void InGameCommandModeB_Select1(UiSpriteButtonControl *source)

{
  UiSelectableGroup_SelectExclusive(3,(UiNodeBase *)source,
      THANDOR_UI_SIBLING(source,InGameUiImage,objectPlacementOption1,objectPlacementOption1),
      THANDOR_UI_SIBLING(source,InGameUiImage,objectPlacementOption1,objectPlacementOption2),
      THANDOR_UI_SIBLING(source,InGameUiImage,objectPlacementOption1,objectPlacementOption0));
  g_UiCommandModeB = 1;
  return;
}


/* Object placement option 2 (action 0x1116, g_InGameUiActionHandlersPage11[22]; also the editor hotkeys
   in ui/ingame/runtime.c): selects objectPlacementOption2 among the three object placement buttons and sets
   g_UiCommandModeB = 2.
*/
void InGameCommandModeB_Select2(UiSpriteButtonControl *source)

{
  UiSelectableGroup_SelectExclusive(3,(UiNodeBase *)source,
      THANDOR_UI_SIBLING(source,InGameUiImage,objectPlacementOption2,objectPlacementOption1),
      THANDOR_UI_SIBLING(source,InGameUiImage,objectPlacementOption2,objectPlacementOption2),
      THANDOR_UI_SIBLING(source,InGameUiImage,objectPlacementOption2,objectPlacementOption0));
  g_UiCommandModeB = 2;
  return;
}


/* Smoothing tool option 0 (action 0x1117, g_InGameUiActionHandlersPage11[23]; also the editor hotkeys in
   ui/ingame/runtime.c): selects smoothingToolOption0 and sets g_UiCommandModeE = 0. Unlike options 1 and 2 its
   exclusive group also contains smoothingRelaxLandButton.
*/
void InGameCommandModeE_Select0(UiSpriteButtonControl *source)

{
  UiSelectableGroup_SelectExclusive(4,(UiNodeBase *)source,
      THANDOR_UI_SIBLING(source,InGameUiImage,smoothingToolOption0,smoothingRelaxLandButton),
      THANDOR_UI_SIBLING(source,InGameUiImage,smoothingToolOption0,smoothingToolOption2),
      THANDOR_UI_SIBLING(source,InGameUiImage,smoothingToolOption0,smoothingToolOption1),
      THANDOR_UI_SIBLING(source,InGameUiImage,smoothingToolOption0,smoothingToolOption0));
  g_UiCommandModeE = 0;
  return;
}


/* Smoothing tool option 1 (action 0x1118, g_InGameUiActionHandlersPage11[24]; also the editor hotkeys in
   ui/ingame/runtime.c): selects smoothingToolOption1 among the three smoothing tool buttons and sets
   g_UiCommandModeE = 1.
*/
void InGameCommandModeE_Select1(UiSpriteButtonControl *source)

{
  UiSelectableGroup_SelectExclusive(3,(UiNodeBase *)source,
      THANDOR_UI_SIBLING(source,InGameUiImage,smoothingToolOption1,smoothingToolOption2),
      THANDOR_UI_SIBLING(source,InGameUiImage,smoothingToolOption1,smoothingToolOption1),
      THANDOR_UI_SIBLING(source,InGameUiImage,smoothingToolOption1,smoothingToolOption0));
  g_UiCommandModeE = 1;
  return;
}


/* Smoothing tool option 2 (action 0x1119, g_InGameUiActionHandlersPage11[25]; also the editor hotkeys in
   ui/ingame/runtime.c): selects smoothingToolOption2 among the three smoothing tool buttons and sets
   g_UiCommandModeE = 2.
*/
void InGameCommandModeE_Select2(UiSpriteButtonControl *source)

{
  UiSelectableGroup_SelectExclusive(3,(UiNodeBase *)source,
      THANDOR_UI_SIBLING(source,InGameUiImage,smoothingToolOption2,smoothingToolOption2),
      THANDOR_UI_SIBLING(source,InGameUiImage,smoothingToolOption2,smoothingToolOption1),
      THANDOR_UI_SIBLING(source,InGameUiImage,smoothingToolOption2,smoothingToolOption0));
  g_UiCommandModeE = 2;
  return;
}


/* Smoothing page button smoothingRelaxGatedButton (action 0x111A, g_InGameUiActionHandlersPage11[26]; also
   an editor hotkey in ui/ingame/runtime.c): runs 128 sign-gated terrain relaxation passes over the field, in a
   network game through command 0x3200 on every machine.
*/
void InGameCommandRange_DispatchState0(UiNodeBase *source)

{
  if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_NETWORKED_MASK) ==
      SESSION_NETWORK_ROLE_LOCAL) {
    TerrainGrid_RunDirectionalRelaxationPasses
              (g_LocalPlayerRuntimeId,0,TERRAIN_RELAXATION_BUTTON_PASSES,TERRAIN_RELAXATION_SIGN_GATED);
  }
  else {
    InGameCommandQueue_AppendLocalPlayerCommand
              (INGAME_COMMAND_TERRAIN_RELAXATION,0,TERRAIN_RELAXATION_BUTTON_PASSES,TERRAIN_RELAXATION_SIGN_GATED);
  }
  return;
}


/* Smoothing page button smoothingRelaxLandButton (action 0x111B, g_InGameUiActionHandlersPage11[27]; also
   an editor hotkey in ui/ingame/runtime.c): like InGameCommandRange_DispatchState0 with the ungated land tool
   relaxation mode.
*/
void InGameCommandRange_DispatchState1(UiNodeBase *source)

{
  if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_NETWORKED_MASK) ==
      SESSION_NETWORK_ROLE_LOCAL) {
    TerrainGrid_RunDirectionalRelaxationPasses
              (g_LocalPlayerRuntimeId,0,TERRAIN_RELAXATION_BUTTON_PASSES,TERRAIN_RELAXATION_UNGATED_LAND_TOOL);
  }
  else {
    InGameCommandQueue_AppendLocalPlayerCommand
              (INGAME_COMMAND_TERRAIN_RELAXATION,0,TERRAIN_RELAXATION_BUTTON_PASSES,TERRAIN_RELAXATION_UNGATED_LAND_TOOL);
  }
  return;
}


/* Region tool option 0 (action 0x111C, g_InGameUiActionHandlersPage11[28]): selects regionToolOption0 of the
   two region tool buttons, sets g_UiCommandModeF = 0 and copies it into the world runtime (fieldRegion.regionToolMode), where the
   region markers of the world view read it.
*/
void InGameCommandModeF_Select0(UiSpriteButtonControl *source)

{
  UiSelectableGroup_SelectExclusive(2,(UiNodeBase *)source,
      THANDOR_UI_SIBLING(source,InGameUiImage,regionToolOption0,regionToolOption1),
      THANDOR_UI_SIBLING(source,InGameUiImage,regionToolOption0,regionToolOption0));
  g_UiCommandModeF = 0;
  ((WorldRuntimeContext *)THANDOR_UI_SIBLING(source,InGameUiImage,regionToolOption0,worldView))
       ->fieldRegion.regionToolMode = 0;
  return;
}


/* Region tool option 1 (action 0x111D, g_InGameUiActionHandlersPage11[29]): selects regionToolOption1 and
   sets g_UiCommandModeF and its world runtime copy (fieldRegion.regionToolMode) to 1.
*/
void InGameCommandModeF_Select1(UiSpriteButtonControl *source)

{
  UiSelectableGroup_SelectExclusive(2,(UiNodeBase *)source,
      THANDOR_UI_SIBLING(source,InGameUiImage,regionToolOption1,regionToolOption1),
      THANDOR_UI_SIBLING(source,InGameUiImage,regionToolOption1,regionToolOption0));
  g_UiCommandModeF = 1;
  ((WorldRuntimeContext *)THANDOR_UI_SIBLING(source,InGameUiImage,regionToolOption1,worldView))
       ->fieldRegion.regionToolMode = 1;
  return;
}


/* Empty callback: InGameUiCommandRuntime_ApplyInteractionSubsystemActiveState installs it as
   fieldRegion.clearTransientStateCallback of the world runtime while the editor is active.
*/
void UiCommandRuntime_CallbackNoOp(void)

{
  return;
}

/* In-game command handler 0x150 (quit game window and player departure): CLOSE_SESSION ends the session,
   SURRENDER destroys every army of the player's faction. Without flags the player has left: another player's
   departure is announced in the message history (text 0xFF08); the local player's own departure marks the
   session as left and, in a network game, shuts the network backend down and falls back to a one-player setup.
*/
void InGameCommand_HandlePlayerDeparture
          (PlayerOrFactionRuntimeId32 playerOrFactionId,uint32_t value1,uint32_t value2,
          GameEntityCommandFlags flags)

{
  WorldRuntimeContext *worldRuntime;
  uint32_t factionToken;
  GameEntityRuntime *entityRuntime;
  InGameRuntimeRoot *runtimeRoot;
  FrontendPlayerRuntimeBlockCount remainingPlayers;
  FrontendPlayerRuntimeRecord *playerRecord;
  uint16_t *departureText;
  WorldOwnerListNode *ownerNode;
  
  runtimeRoot = g_InGameRuntimeRoot;
  if ((flags & INGAME_PLAYER_DEPARTURE_FLAG_CLOSE_SESSION) != 0) {
    g_UiCommandRuntimeFlags = g_UiCommandRuntimeFlags | UI_COMMAND_RUNTIME_FLAG_SESSION_CLOSED;
    return;
  }
  worldRuntime = &g_InGameRuntimeRoot->worldRuntime;
  if ((flags & INGAME_PLAYER_DEPARTURE_FLAG_SURRENDER) != 0) {
    factionToken = g_SelectionPlayerRuntimeBlockPointers[playerOrFactionId]->factionIndex;
    for (ownerNode = (g_InGameRuntimeRoot->worldRuntime).ownerListHead;
        ownerNode != NULL; ownerNode = ownerNode->nextNode) {
      if (ownerNode->ownerClassId == WORLD_OWNER_RUNTIME_MODEL) {
        entityRuntime = (GameEntityRuntime *)
             (((ModelRuntimeSlot *)ownerNode->runtimePayload)->ownerArmyRuntimeOrSavedOffset).armyRuntime;
        if (factionToken == (entityRuntime->common).ownership.ownerIndex) {
          ArmyRuntime_DestroyInstanceAndRefreshUi(worldRuntime,entityRuntime);
        }
      }
    }
    return;
  }
  remainingPlayers = g_FrontendPlayerRuntimeBlockCount;
  playerRecord = g_FrontendPlayerRuntimeBlocks;
  do {
    if (playerOrFactionId == playerRecord->playerRuntimeId) {
      playerRecord->heartbeatExpiryTicks = 0;
      if (playerRecord == g_FrontendPlayerRuntimeBlocks) {
        g_SessionTransferTimeoutTicks = 0;
      }
      if (playerOrFactionId == (runtimeRoot->worldRuntime).selection.activePlayerRuntimeId) {
        g_UiCommandRuntimeFlags = g_UiCommandRuntimeFlags | UI_COMMAND_RUNTIME_FLAG_LOCAL_PLAYER_LEFT;
        Resource_Release((void *)(uintptr_t)g_FrontendLoadedCampaignAsset);
        g_FrontendLoadedCampaignAsset = 0;
        if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_NETWORKED_MASK) ==
            SESSION_NETWORK_ROLE_LOCAL) {
          g_FrontendLoadedCampaignAsset = 0; /* stored twice, as in the original */
          return;
        }
        g_SessionNetworkRoleFlags =
             g_SessionNetworkRoleFlags & ~SESSION_NETWORK_ROLE_NETWORKED_MASK;
        g_SessionTransferTimeoutTicks = 0;
        g_NetworkBackendSlot3();
        g_NetworkBackendSlot1();
        playerRecord = g_FrontendPlayerRuntimeBlocks;
        g_FrontendPlayerRuntimeBlockCount = 1;
        g_LocalPlayerRuntimeId = 0;
        (playerRecord->playerName).textUtf16[0] = 0;
        (playerRecord->playerName).textUtf16[1] = 0;
        playerRecord->playerRuntimeId = 0;
        (playerRecord->factionAssignment).roleStateFlags = 0;
        return;
      }
      /* departure message with the player name patched in */
      departureText = TextResource_Resolve(TEXT_ID_PLAYER_DEPARTED);
      RichTextCommandStream_PatchPayloadBySelector(0,&playerRecord->playerName,departureText);
      InGameRecentTextHistory_InsertAndRebuild8(departureText);
      return;
    }
    remainingPlayers--;
    playerRecord++;
  } while (remainingPlayers != 0);
  return;
}


/* Terrain colours of the smoothing tool (InGameCommandModeG_Select2): rebuilds the terrain lighting colour ramp
   from the world runtime's lighting colours (lighting.baseColorArgb, rampStepColorArgb) with their alpha removed and the secondary
   colour (secondaryColorArgb) made opaque, relights the field grid with the light angles stored in the root, then sets
   UI_COMMAND_MODE_G_COLOR_VARIANT_MASKED and the limit read by the terrain triangle and marker drawing.
*/
void UiCommandModeG_ApplyMaskedColorVariant(void *worldRuntime)

{
  TerrainLighting_BuildColorRampAndSetBaseColor
            (((WorldRuntimeContext *)worldRuntime)->lighting.secondaryColorArgb | ARGB8888_ALPHA_MASK,
             ((WorldRuntimeContext *)worldRuntime)->lighting.baseColorArgb & 0xffffff,
             ((WorldRuntimeContext *)worldRuntime)->lighting.rampStepColorArgb & 0xffffff);
  FieldGrid_RecomputeInteriorDirectionalLighting
            (THANDOR_CONTAINER_OF(worldRuntime,InGameRuntimeRoot,worldRuntime)->lightElevationAngle,THANDOR_CONTAINER_OF(worldRuntime,InGameRuntimeRoot,worldRuntime)->lightAzimuthAngle,
             ((WorldRuntimeContext *)worldRuntime)->fieldGrid);
  g_UiCommandModeGColorVariantFlags = g_UiCommandModeGColorVariantFlags | UI_COMMAND_MODE_G_COLOR_VARIANT_MASKED;
  g_UiCommandModeGColorVariantLimit = UI_COMMAND_MODE_G_COLOR_LIMIT_MASKED;
  return;
}


/* Shows the region markers of the world view (sets WORLD_RUNTIME_FLAG_DRAW_REGION_MARKERS); only the region tool
   (InGameCommandModeG_Select5) uses it.
*/
void UiCommandModeG_ShowRegionMarkers(WorldRuntimeContext *context)

{
  context->runtimeFlags = context->runtimeFlags | WORLD_RUNTIME_FLAG_DRAW_REGION_MARKERS;
  return;
}


/* Texture shown by a material swatch: the first texture of the material's set, NULL for an empty entry. */
static GraphicsTextureSourceAsset *TerrainMaterial_SwatchTexture(uint32_t materialIndex)
{
  if (g_TerrainMaterialTextureSets[materialIndex] == NULL) {
    return NULL;
  }
  return g_TerrainMaterialTextureSets[materialIndex]->entries[0].sourceAsset;
}

/* Selects terrain material absoluteIndex for the material tool (InGameCommandMatrix_SelectMappedControl and the
   editor hotkeys/initialisation in ui/ingame/runtime.c): shows its texture in materialToolSelectedSwatch,
   scrolls the twelve-swatch page in rows of three until the material is visible, fills the twelve swatches from
   g_TerrainMaterialTextureSets (empty entries show nothing) and selects the material's swatch.
*/
void UiCommandMatrix_SelectIndex(UiCommandModeIndex absoluteIndex,UiNodeBase *root)

{
  uint32_t pageEnd;
  uint32_t pageBase;
  
  g_UiCommandAbsoluteSelectionIndex = absoluteIndex;
  ((UiImagePanelControl *)INGAME_UI(root,materialToolSelectedSwatch))->textureSource =
       g_TerrainMaterialTextureSets[absoluteIndex]->entries[0].sourceAsset;
  pageEnd = g_UiCommandSelectionPageBaseIndex + MATERIAL_SWATCH_COUNT;
  pageBase = g_UiCommandSelectionPageBaseIndex;
  /* move the page by rows of three swatches until absoluteIndex lies in [pageBase, pageEnd) */
  while (absoluteIndex < pageBase) {
    pageBase = pageBase - MATERIAL_SWATCH_ROW_LENGTH;
    pageEnd = pageEnd - MATERIAL_SWATCH_ROW_LENGTH;
  }
  while (absoluteIndex >= pageEnd) {
    pageBase = pageBase + MATERIAL_SWATCH_ROW_LENGTH;
    pageEnd = pageEnd + MATERIAL_SWATCH_ROW_LENGTH;
  }
  g_UiCommandSelectionPageBaseIndex = pageBase;
  ((UiImagePanelControl *)INGAME_UI(root,materialSwatch00))->textureSource = TerrainMaterial_SwatchTexture(pageBase);
  ((UiImagePanelControl *)INGAME_UI(root,materialSwatch01))->textureSource = TerrainMaterial_SwatchTexture(pageBase + 1);
  ((UiImagePanelControl *)INGAME_UI(root,materialSwatch02))->textureSource = TerrainMaterial_SwatchTexture(pageBase + 2);
  ((UiImagePanelControl *)INGAME_UI(root,materialSwatch03))->textureSource = TerrainMaterial_SwatchTexture(pageBase + 3);
  ((UiImagePanelControl *)INGAME_UI(root,materialSwatch04))->textureSource = TerrainMaterial_SwatchTexture(pageBase + 4);
  ((UiImagePanelControl *)INGAME_UI(root,materialSwatch05))->textureSource = TerrainMaterial_SwatchTexture(pageBase + 5);
  ((UiImagePanelControl *)INGAME_UI(root,materialSwatch06))->textureSource = TerrainMaterial_SwatchTexture(pageBase + 6);
  ((UiImagePanelControl *)INGAME_UI(root,materialSwatch07))->textureSource = TerrainMaterial_SwatchTexture(pageBase + 7);
  ((UiImagePanelControl *)INGAME_UI(root,materialSwatch08))->textureSource = TerrainMaterial_SwatchTexture(pageBase + 8);
  ((UiImagePanelControl *)INGAME_UI(root,materialSwatch09))->textureSource = TerrainMaterial_SwatchTexture(pageBase + 9);
  ((UiImagePanelControl *)INGAME_UI(root,materialSwatch10))->textureSource = TerrainMaterial_SwatchTexture(pageBase + 10);
  ((UiImagePanelControl *)INGAME_UI(root,materialSwatch11))->textureSource = TerrainMaterial_SwatchTexture(pageBase + 11);
  /* The original pushes all twelve command controls (offsets 11..0) as the variadic list. */
  UiSelectableGroup_SelectExclusive
            (MATERIAL_SWATCH_COUNT,THANDOR_UI_AT(root,g_UiMappedCommandControlOffsets[absoluteIndex - pageBase]),
      THANDOR_UI_AT(root,g_UiMappedCommandControlOffsets[0]),
      THANDOR_UI_AT(root,g_UiMappedCommandControlOffsets[1]),
      THANDOR_UI_AT(root,g_UiMappedCommandControlOffsets[2]),
      THANDOR_UI_AT(root,g_UiMappedCommandControlOffsets[3]),
      THANDOR_UI_AT(root,g_UiMappedCommandControlOffsets[4]),
      THANDOR_UI_AT(root,g_UiMappedCommandControlOffsets[5]),
      THANDOR_UI_AT(root,g_UiMappedCommandControlOffsets[6]),
      THANDOR_UI_AT(root,g_UiMappedCommandControlOffsets[7]),
      THANDOR_UI_AT(root,g_UiMappedCommandControlOffsets[8]),
      THANDOR_UI_AT(root,g_UiMappedCommandControlOffsets[9]),
      THANDOR_UI_AT(root,g_UiMappedCommandControlOffsets[10]),
      THANDOR_UI_AT(root,g_UiMappedCommandControlOffsets[11]));
  return;
}


/* Changes the global g_UiCommandRuntimeFlags: first clears clearMask, then sets setMask, then toggles toggleMask
   (the masks come in the reverse order as arguments). Local games call it directly, network games send the
   same masks as player command 0x310. playerRuntimeId is not used: the flags are not per player.
*/
void UiCommandRuntimeFlags_ApplyClearSetToggleMasks(PlayerRuntimeId playerRuntimeId,UiCommandRuntimeFlagMask toggleMask,
          UiCommandRuntimeFlagMask setMask,UiCommandRuntimeFlagMask clearMask)

{
  g_UiCommandRuntimeFlags = g_UiCommandRuntimeFlags & ~clearMask;
  g_UiCommandRuntimeFlags = g_UiCommandRuntimeFlags | setMask;
  g_UiCommandRuntimeFlags = g_UiCommandRuntimeFlags ^ toggleMask;
}


/* Hides the surface point marker of the world view (clears WORLD_RUNTIME_FLAG_DRAW_SURFACE_POINT_MARKER); editor
   mode tabs G3/G4 and InGameUiCommandRuntime_ApplyInteractionSubsystemActiveState.
*/
void UiCommandModeG_HideSurfacePointMarker(WorldRuntimeContext *context)

{
  context->runtimeFlags = context->runtimeFlags & ~WORLD_RUNTIME_FLAG_DRAW_SURFACE_POINT_MARKER;
  return;
}


/* Shows the terrain point markers of the world view (sets WORLD_RUNTIME_FLAG_DRAW_TERRAIN_POINT_MARKERS); editor
   mode tabs G0/G1 (height and material tools).
*/
void UiCommandModeG_ShowTerrainPointMarkers(WorldRuntimeContext *context)

{
  context->runtimeFlags = context->runtimeFlags | WORLD_RUNTIME_FLAG_DRAW_TERRAIN_POINT_MARKERS;
  return;
}


/* Sets WORLD_RUNTIME_FLAG_SECONDARY_SURFACE_ONLY (view ray and markers use only the secondary field surface);
   editor mode tabs G0 and G2.
*/
void UiCommandModeG_SetSecondarySurfaceOnly(WorldRuntimeContext *context)

{
  context->runtimeFlags = context->runtimeFlags | WORLD_RUNTIME_FLAG_SECONDARY_SURFACE_ONLY;
  return;
}


/* Shows the army metrics overlay of the world view (sets WORLD_RUNTIME_FLAG_DRAW_ARMY_METRICS); editor mode tabs
   G3-G5 and InGameUiCommandRuntime_ApplyInteractionSubsystemActiveState when the editor is switched off.
*/
void UiCommandModeG_ShowArmyMetrics(WorldRuntimeContext *context)

{
  context->runtimeFlags = context->runtimeFlags | WORLD_RUNTIME_FLAG_DRAW_ARMY_METRICS;
  return;
}


/* Hides the army metrics overlay and ends a drag selection (clears WORLD_RUNTIME_FLAG_DRAW_ARMY_METRICS and
   WORLD_RUNTIME_FLAG_DRAG_SELECTING, which also draws the selection frame); editor mode tabs G0-G2.
*/
void UiCommandModeG_HideArmyMetricsAndEndDragSelect(WorldRuntimeContext *context)

{
  context->runtimeFlags =
       context->runtimeFlags & ~(WORLD_RUNTIME_FLAG_DRAW_ARMY_METRICS | WORLD_RUNTIME_FLAG_DRAG_SELECTING);
  return;
}


/* Shows the surface point marker of the world view (sets WORLD_RUNTIME_FLAG_DRAW_SURFACE_POINT_MARKER); editor
   mode tabs G0, G1, G2 and G5.
*/
void UiCommandModeG_ShowSurfacePointMarker(WorldRuntimeContext *context)

{
  context->runtimeFlags = context->runtimeFlags | WORLD_RUNTIME_FLAG_DRAW_SURFACE_POINT_MARKER;
  return;
}


/* Hides the terrain point markers of the world view (clears WORLD_RUNTIME_FLAG_DRAW_TERRAIN_POINT_MARKERS); editor
   mode tabs G2-G5 and InGameUiCommandRuntime_ApplyInteractionSubsystemActiveState.
*/
void UiCommandModeG_HideTerrainPointMarkers(WorldRuntimeContext *context)

{
  context->runtimeFlags = context->runtimeFlags & ~WORLD_RUNTIME_FLAG_DRAW_TERRAIN_POINT_MARKERS;
  return;
}


/* Clears WORLD_RUNTIME_FLAG_SECONDARY_SURFACE_ONLY; editor mode tabs G1, G3-G5 and
   InGameUiCommandRuntime_ApplyInteractionSubsystemActiveState.
*/
void UiCommandModeG_ClearSecondarySurfaceOnly(WorldRuntimeContext *context)

{
  context->runtimeFlags = context->runtimeFlags & ~WORLD_RUNTIME_FLAG_SECONDARY_SURFACE_ONLY;
  return;
}


/* Terrain colours of every editor mode except smoothing (and of InGameUiCommandRuntime_ApplyInteractionSubsystemActiveState
   when the editor is switched off): rebuilds the terrain colour ramp from the world runtime's lighting colours
   (lighting.baseColorArgb, rampStepColorArgb, secondaryColorArgb) unchanged, relights the field grid with the light
   angles stored in the root, clears bit 0x1000 of g_UiCommandModeGColorVariantFlags and sets the limit to 0x00FFFFFF. The
   counterpart of UiCommandModeG_ApplyMaskedColorVariant.
*/
void UiCommandModeG_ApplyRawColorVariant(void *worldRuntime)

{
  TerrainLighting_BuildColorRampAndSetBaseColor
            (((WorldRuntimeContext *)worldRuntime)->lighting.secondaryColorArgb,((WorldRuntimeContext *)worldRuntime)->lighting.baseColorArgb,
             ((WorldRuntimeContext *)worldRuntime)->lighting.rampStepColorArgb);
  FieldGrid_RecomputeInteriorDirectionalLighting
            (THANDOR_CONTAINER_OF(worldRuntime,InGameRuntimeRoot,worldRuntime)->lightElevationAngle,THANDOR_CONTAINER_OF(worldRuntime,InGameRuntimeRoot,worldRuntime)->lightAzimuthAngle,
             ((WorldRuntimeContext *)worldRuntime)->fieldGrid);
  g_UiCommandModeGColorVariantFlags = g_UiCommandModeGColorVariantFlags & ~UI_COMMAND_MODE_G_COLOR_VARIANT_MASKED;
  g_UiCommandModeGColorVariantLimit = UI_COMMAND_MODE_G_COLOR_LIMIT_RAW;
  return;
}


/* Hides the region markers of the world view (clears WORLD_RUNTIME_FLAG_DRAW_REGION_MARKERS); every editor mode tab
   except G5 and InGameUiCommandRuntime_ApplyInteractionSubsystemActiveState.
*/
void UiCommandModeG_HideRegionMarkers(WorldRuntimeContext *context)

{
  context->runtimeFlags = context->runtimeFlags & ~WORLD_RUNTIME_FLAG_DRAW_REGION_MARKERS;
  return;
}


/* Shows the grid vertex markers of the world view (sets WORLD_RUNTIME_FLAG_DRAW_GRID_VERTEX_MARKERS); every
   editor mode tab.
*/
void UiCommandModeG_ShowGridVertexMarkers(WorldRuntimeContext *context)

{
  context->runtimeFlags = context->runtimeFlags | WORLD_RUNTIME_FLAG_DRAW_GRID_VERTEX_MARKERS;
  return;
}


/* Common part of the editor mode tabs InGameCommandModeG_Select0..5: selects the clicked tab among the six, shows
   the mode's pages in modePreviewPageStack, modeDetailPageStack and modeCommandPageStack (page tables
   g_UiCommandModeG*PageIndices) and stores the mode in g_UiCommandModeG. Returns the in-game root.
*/
InGameRuntimeRoot * UiCommandModeG_SelectAndSyncPages(UiCommandModeIndex modeIndex,UiSelectableControl *source)

{
  InGameRuntimeRoot *root;

  root = (InGameRuntimeRoot *)source;
  while ((root->rootUi).base.parent != UI_NODE_NONE) {
    root = (InGameRuntimeRoot *)(root->rootUi).base.parent;
  }
  UiSelectableGroup_FindVisibleSelected(NULL,NULL,6,
      INGAME_UI(root,editorModeTabRegion),
      INGAME_UI(root,editorModeTabObjectPlacement),
      INGAME_UI(root,editorModeTabUnitPlacement),
      INGAME_UI(root,editorModeTabTerrainSmoothing),
      INGAME_UI(root,editorModeTabTerrainMaterial),
      INGAME_UI(root,editorModeTabTerrainHeight));
  UiSelectableGroup_SelectExclusive(6,&source->base,
      INGAME_UI(root,editorModeTabRegion),
      INGAME_UI(root,editorModeTabObjectPlacement),
      INGAME_UI(root,editorModeTabUnitPlacement),
      INGAME_UI(root,editorModeTabTerrainSmoothing),
      INGAME_UI(root,editorModeTabTerrainMaterial),
      INGAME_UI(root,editorModeTabTerrainHeight));
  UiPageStack_SetActiveIndex
            (g_UiCommandModeGPrimaryPageIndices[modeIndex],
             (UiPageStackControl *)INGAME_UI(root,modePreviewPageStack));
  UiPageStack_SetActiveIndex
            (g_UiCommandModeGSecondaryPageIndices[modeIndex],
             (UiPageStackControl *)INGAME_UI(root,modeDetailPageStack));
  UiPageStack_SetActiveIndex
            (g_UiCommandModeGTertiaryPageIndices[modeIndex],
             (UiPageStackControl *)INGAME_UI(root,modeCommandPageStack));
  g_UiCommandModeG = modeIndex;
  return root;
}


/* Class vtables. */

UiNodeVtable g_UiNodeVtable_005162C0 = {
        .relocate = (void *)UiSpriteButtonControl_Relocate,
        .method04 = (void *)UiNode_DefaultMethod04_NoOp,
        .drawClipped = (void *)UiSpriteButtonControl_DrawClipped,
        .layout = (void *)UiContainer_LayoutChildren,
        .nonRightPress = (void *)UiCommandSpriteButtonControl_BeginPress,
        .nonRightRelease = (void *)UiCommandSpriteButtonControl_NonRightRelease,
        .rightPress = (void *)UiCommandSpriteButtonControl_BeginPress,
        .rightRelease = (void *)UiCommandSpriteButtonControl_RightRelease,
        .nonRightDrag = (void *)UiSpriteButtonControl_NonRightDrag,
        .rightDrag = (void *)UiSpriteButtonControl_NonRightDrag,
        .pointerMove = (void *)InGameArmyStock_PointerMoveShowSlotDetails,
        .hitTest = (void *)UiSpriteButtonControl_HitTestOpaque,
        .keyboardEvent = (void *)UiSelectableControl_KeyboardEvent,
        .applyFlags = (void *)UiNode_ApplyFlagsRecursive,
        .suppressActionId = (void *)UiSelectableControl_SuppressIfActionId,
        .unsuppressActionId = (void *)UiSelectableControl_UnsuppressIfActionId,
        .tick = (void *)UiNode_DefaultTick,
        .pointerWheel = (void *)UiNode_ForwardPointerWheelToParent,
};

UiNodeVtable g_UiNodeVtable_00516310 = {
        .relocate = (void *)UiSpriteButtonControl_Relocate,
        .method04 = (void *)UiNode_DefaultMethod04_NoOp,
        .drawClipped = (void *)UiSpriteButtonControl_DrawClipped,
        .layout = (void *)UiContainer_LayoutChildren,
        .nonRightPress = (void *)UiCommandSpriteButtonControl_BeginPress,
        .nonRightRelease = (void *)UiCommandSpriteButtonControl_NonRightRelease,
        .rightPress = (void *)UiCommandSpriteButtonControl_BeginPress,
        .rightRelease = (void *)UiCommandSpriteButtonControl_RightRelease,
        .nonRightDrag = (void *)UiSpriteButtonControl_NonRightDrag,
        .rightDrag = (void *)UiSpriteButtonControl_NonRightDrag,
        .pointerMove = (void *)UiNode_DefaultPointerMove,
        .hitTest = (void *)UiSpriteButtonControl_HitTestOpaque,
        .keyboardEvent = (void *)UiSelectableControl_KeyboardEvent,
        .applyFlags = (void *)UiNode_ApplyFlagsRecursive,
        .suppressActionId = (void *)UiSelectableControl_SuppressIfActionId,
        .unsuppressActionId = (void *)UiSelectableControl_UnsuppressIfActionId,
        .tick = (void *)UiNode_DefaultTick,
        .pointerWheel = (void *)UiNode_ForwardPointerWheelToParent,
};

InGameUiCommandModeActionHandlerPage11 g_InGameUiActionHandlersPage11 = {
        .handlers = {
            /*  0 */ (void *)InGameCommandModeG_Select0,
            /*  1 */ (void *)InGameCommandModeG_Select1,
            /*  2 */ (void *)InGameCommandModeG_Select2,
            /*  3 */ 0,
            /*  4 */ (void *)InGameCommandModeG_Select5,
            /*  5 */ (void *)InGameCommandModeG_Select3,
            /*  6 */ (void *)InGameCommandModeG_Select4,
            /*  7 */ 0,
            /*  8 */ (void *)InGameCommandModeC_Select0,
            /*  9 */ (void *)InGameCommandModeC_Select1,
            /* 10 */ (void *)InGameCommandModeC_Select2,
            /* 11 */ (void *)InGameCommandModeC_Select3,
            /* 12 */ (void *)InGameCommandModeD_Select0,
            /* 13 */ (void *)InGameCommandModeD_Select1,
            /* 14 */ (void *)InGameCommandModeD_Select2,
            /* 15 */ (void *)InGameCommandModeD_Select3,
            /* 16 */ (void *)InGameCommandMatrix_SelectMappedControl,
            /* 17 */ (void *)InGameCommandModeA_Select0,
            /* 18 */ (void *)InGameCommandModeA_Select1,
            /* 19 */ (void *)InGameCommandModeA_Select2,
            /* 20 */ (void *)InGameCommandModeB_Select0,
            /* 21 */ (void *)InGameCommandModeB_Select1,
            /* 22 */ (void *)InGameCommandModeB_Select2,
            /* 23 */ (void *)InGameCommandModeE_Select0,
            /* 24 */ (void *)InGameCommandModeE_Select1,
            /* 25 */ (void *)InGameCommandModeE_Select2,
            /* 26 */ (void *)InGameCommandRange_DispatchState0,
            /* 27 */ (void *)InGameCommandRange_DispatchState1,
            /* 28 */ (void *)InGameCommandModeF_Select0,
            /* 29 */ (void *)InGameCommandModeF_Select1
        }};

void *g_UiCommandModeGHandlers[6] = {
    /* 0 */ (void *)InGameCommandModeG_Select0,
    /* 1 */ (void *)InGameCommandModeG_Select1,
    /* 2 */ (void *)InGameCommandModeG_Select2,
    /* 3 */ (void *)InGameCommandModeG_Select3,
    /* 4 */ (void *)InGameCommandModeG_Select4,
    /* 5 */ (void *)InGameCommandModeG_Select5};

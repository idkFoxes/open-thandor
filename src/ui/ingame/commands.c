/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/src/ui/ingame/commands.c
 * Reverse engineering by idkFoxes 2026
 */

#include <thandor/ui/ingame/commands.h>
#include <thandor/thandor.h>

/* Implementation ownership: ui/ingame/commands. */

/* Address: 0x0056DB40.
   Editor mode tab G0, terrain height tool (action 0x1100: g_InGameUiCommandModeActionHandlers30[0],
   g_UiCommandModeGHandlers[0]; also called by the editor hotkeys in ui/ingame/runtime.c). Selects the tab, shows
   the tool's pages and switches the world view to the height tool overlays: surface point, terrain point, grid
   vertex and secondary surface markers, with the unmasked terrain colour ramp.
*/
void __thandor_void_preserve_eax_ecx_edx InGameCommandModeG_Select0(UiSelectableControl *source)

{
  InGameRuntimeRootImageC3E4 *runtimeRoot;
  WorldRuntimeContext *worldRuntime;

  runtimeRoot = UiCommandModeG_SelectAndSyncPages(EDITOR_MODE_TERRAIN_HEIGHT,source);
  worldRuntime = &runtimeRoot->worldRuntime0A30;
  UiCommandModeG_SetNodeFlag00100000(worldRuntime);
  UiCommandModeG_SetNodeFlag00200000(worldRuntime);
  UiCommandModeG_ClearNodeFlags00000480(worldRuntime);
  UiCommandModeG_SetNodeFlag00800000(worldRuntime);
  UiCommandModeG_SetNodeFlag01000000(worldRuntime);
  UiCommandModeG_ApplyRawColorVariant(worldRuntime);
  UiCommandModeG_ClearNodeFlag02000000(worldRuntime);
  return;
}


/* Address: 0x0056DB90.
   Editor mode tab G1, terrain material tool (action 0x1101: g_InGameUiCommandModeActionHandlers30[1],
   g_UiCommandModeGHandlers[1]; also called by the editor hotkeys in ui/ingame/runtime.c). Like G0, but without the
   secondary surface markers.
*/
void __thandor_void_preserve_eax_ecx_edx InGameCommandModeG_Select1(UiSelectableControl *source)

{
  InGameRuntimeRootImageC3E4 *runtimeRoot;
  WorldRuntimeContext *worldRuntime;

  runtimeRoot = UiCommandModeG_SelectAndSyncPages(EDITOR_MODE_TERRAIN_MATERIAL,source);
  worldRuntime = &runtimeRoot->worldRuntime0A30;
  UiCommandModeG_SetNodeFlag00100000(worldRuntime);
  UiCommandModeG_SetNodeFlag00200000(worldRuntime);
  UiCommandModeG_ClearNodeFlags00000480(worldRuntime);
  UiCommandModeG_SetNodeFlag00800000(worldRuntime);
  UiCommandModeG_ClearNodeFlag01000000(worldRuntime);
  UiCommandModeG_ApplyRawColorVariant(worldRuntime);
  UiCommandModeG_ClearNodeFlag02000000(worldRuntime);
  return;
}


/* Address: 0x0056DBE0.
   Editor mode tab G2, terrain smoothing tool (action 0x1102: g_InGameUiCommandModeActionHandlers30[2],
   g_UiCommandModeGHandlers[2]; also called by the editor hotkeys in ui/ingame/runtime.c). Shows the surface point,
   grid vertex and secondary surface markers and is the only mode with the masked terrain colours
   (UiCommandModeG_ApplyMaskedColorVariant).
*/
void __thandor_void_preserve_eax_ecx_edx InGameCommandModeG_Select2(UiSelectableControl *source)

{
  InGameRuntimeRootImageC3E4 *runtimeRoot;
  WorldRuntimeContext *worldRuntime;

  runtimeRoot = UiCommandModeG_SelectAndSyncPages(EDITOR_MODE_TERRAIN_SMOOTHING,source);
  worldRuntime = &runtimeRoot->worldRuntime0A30;
  UiCommandModeG_SetNodeFlag00100000(worldRuntime);
  UiCommandModeG_ClearNodeFlag00200000(worldRuntime);
  UiCommandModeG_ClearNodeFlags00000480(worldRuntime);
  UiCommandModeG_SetNodeFlag00800000(worldRuntime);
  UiCommandModeG_SetNodeFlag01000000(worldRuntime);
  UiCommandModeG_ApplyMaskedColorVariant(worldRuntime);
  UiCommandModeG_ClearNodeFlag02000000(worldRuntime);
  return;
}


/* Address: 0x0056DC30.
   Editor mode tab G3, unit placement tool (action 0x1105: g_InGameUiCommandModeActionHandlers30[5],
   g_UiCommandModeGHandlers[3]; also called by the editor hotkeys in ui/ingame/runtime.c). Shows the army metrics
   and grid vertex markers and puts the army asset g_UiCommandModeGArmyAssetId into the selection detail panel;
   an unknown asset id is fatal.
*/
void __thandor_void_preserve_eax_ecx_edx InGameCommandModeG_Select3(UiSelectableControl *source)

{
  InGameRuntimeRootImageC3E4 *runtimeRoot;
  WorldRuntimeContext *worldRuntime;
  ArmyAssetLookupResult armyAssetLookup;
  FatalErrorCheckResult checkedAssetLookup;

  runtimeRoot = UiCommandModeG_SelectAndSyncPages(EDITOR_MODE_UNIT_PLACEMENT,source);
  worldRuntime = &runtimeRoot->worldRuntime0A30;
  UiCommandModeG_ClearNodeFlag00100000(worldRuntime);
  UiCommandModeG_ClearNodeFlag00200000(worldRuntime);
  UiCommandModeG_SetNodeFlag00000400(worldRuntime);
  UiCommandModeG_SetNodeFlag00800000(worldRuntime);
  UiCommandModeG_ClearNodeFlag01000000(worldRuntime);
  UiCommandModeG_ApplyRawColorVariant(worldRuntime);
  UiCommandModeG_ClearNodeFlag02000000(worldRuntime);
  armyAssetLookup = ArmyAssetRegistry_FindById(g_UiCommandModeGArmyAssetId);
  checkedAssetLookup = FatalError_ExitIfFailed((uint32_t)armyAssetLookup.recordOrError,armyAssetLookup.notFound);
  g_UiHoverSelectionRecord = (UiCommandRuntimeRecordPrefix *)checkedAssetLookup.valueOrError;
  InGameSelectionDetailPanel_Rebuild();
  return;
}


/* Address: 0x0056DCA0.
   Editor mode tab G4, object placement tool (action 0x1106: g_InGameUiCommandModeActionHandlers30[6],
   g_UiCommandModeGHandlers[4]; also called by the editor hotkeys in ui/ingame/runtime.c). Same overlays as G3
   (army metrics and grid vertex markers) without the detail panel update.
*/
void __thandor_void_preserve_eax_ecx_edx InGameCommandModeG_Select4(UiSelectableControl *source)

{
  InGameRuntimeRootImageC3E4 *runtimeRoot;
  WorldRuntimeContext *worldRuntime;

  runtimeRoot = UiCommandModeG_SelectAndSyncPages(EDITOR_MODE_OBJECT_PLACEMENT,source);
  worldRuntime = &runtimeRoot->worldRuntime0A30;
  UiCommandModeG_ClearNodeFlag00100000(worldRuntime);
  UiCommandModeG_ClearNodeFlag00200000(worldRuntime);
  UiCommandModeG_SetNodeFlag00000400(worldRuntime);
  UiCommandModeG_SetNodeFlag00800000(worldRuntime);
  UiCommandModeG_ClearNodeFlag01000000(worldRuntime);
  UiCommandModeG_ApplyRawColorVariant(worldRuntime);
  UiCommandModeG_ClearNodeFlag02000000(worldRuntime);
  return;
}


/* Address: 0x0056DCF0.
   Editor mode tab G5, region tool (action 0x1104: g_InGameUiCommandModeActionHandlers30[4],
   g_UiCommandModeGHandlers[5]; also called by ui/ingame/runtime.c). Shows the surface point, army metrics, grid
   vertex and region markers; the region markers draw the variant chosen by mode F, so g_UiCommandModeF is copied
   into the world runtime (+0xB4) as well.
*/
void __thandor_void_preserve_eax_ecx_edx InGameCommandModeG_Select5(UiSelectableControl *source)

{
  InGameRuntimeRootImageC3E4 *runtimeRoot;
  WorldRuntimeContext *worldRuntime;

  runtimeRoot = UiCommandModeG_SelectAndSyncPages(EDITOR_MODE_REGION,source);
  worldRuntime = &runtimeRoot->worldRuntime0A30;
  UiCommandModeG_SetNodeFlag00100000(worldRuntime);
  UiCommandModeG_ClearNodeFlag00200000(worldRuntime);
  UiCommandModeG_SetNodeFlag00000400(worldRuntime);
  UiCommandModeG_SetNodeFlag00800000(worldRuntime);
  UiCommandModeG_ClearNodeFlag01000000(worldRuntime);
  UiCommandModeG_ApplyRawColorVariant(worldRuntime);
  UiCommandModeG_SetNodeFlag02000000(worldRuntime);
  (runtimeRoot->worldRuntime0A30).fieldRegion.reservedCallbackState04 = g_UiCommandModeF;
  return;
}


/* Address: 0x0056AC50.
   Results screen continue button (action 0x101B, g_InGameUiActionHandlersPage10[27]). A local game or network host
   sets UI_COMMAND_RUNTIME_FLAG_RESULTS_CLOSED (the host through command 0x310 so every machine sees it); a network
   client instead reports itself ready, which lets the host show its own continue button.
*/
void __thandor_preserve_eax InGameCommandAction_SetFlag1000OrMarkReady(void *source)

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
    FrontendPlayerRuntime_MarkReadyByIdAndUpdateAction101B(g_LocalPlayerRuntimeId);
  }
  else {
    InGameCommandQueue_AppendLocalPlayerCommand(INGAME_COMMAND_MARK_PLAYER_READY_101B,0,0,0);
  }
  return;
}


/* Address: 0x0056ACC0.
   End movie view click (action 0x1009, g_InGameUiActionHandlersPage10[9]): skips the end movie. Despite the name
   it does not toggle but clears UI_COMMAND_RUNTIME_FLAG_END_MOVIE_PENDING (clear mask of command 0x310, sent to
   every machine in a network game).
*/
void __thandor_preserve_eax InGameCommandAction_ToggleRuntimeFlag0800(void *source)

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


/* Address: 0x0056D6C0.
   Quit game window restart button (action INGAME_ACTION_QUIT_RESTART_MISSION 0x1027,
   g_InGameUiActionHandlersPage10[39]): deselects and closes the in-game menu, then issues command 0x150 with
   INGAME_COMMAND150_FLAG_CLOSE_SESSION, which ends the session.
*/
void __thandor_preserve_eax
InGameCommandState_CloseSettingsAndDispatchOperation150(UiNodeBase *source)

{
  UiNodeBase *parentCursor;

  /* source becomes the in-game UI root (parent -1) */
  parentCursor = source->parent;
  while (parentCursor != UI_NODE_NONE) {
    source = source->parent;
    parentCursor = source->parent;
  }
  UiSelectableControl_SetSelected(0,(UiSelectableControl *)INGAME_UI(source,inGameMenuButton));
  InGameSettingsPage_ToggleAndSynchronizeControls((UiSelectableControl *)INGAME_UI(source,inGameMenuButton));
  if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_NETWORKED_MASK) ==
      SESSION_NETWORK_ROLE_LOCAL) {
    InGameCommand150_HandlePlayerDepartureAndOwnership
              (g_LocalPlayerRuntimeId,0,0,INGAME_COMMAND150_FLAG_CLOSE_SESSION);
  }
  else {
    InGameCommandQueue_AppendLocalPlayerCommand
              (INGAME_COMMAND_PLAYER_DEPARTURE,0,0,INGAME_COMMAND150_FLAG_CLOSE_SESSION);
  }
  return;
}


/* Address: 0x0056DFD0.
   Terrain material swatch click (action 0x1110, g_InGameUiCommandModeActionHandlers30[16]): finds which of the
   twelve swatch controls (g_UiMappedCommandControlOffsets) was clicked and selects the material at that position
   of the current page. Clicks on other controls are ignored.
*/
void __thandor_void_preserve_eax_ecx_edx InGameCommandMatrix_SelectMappedControl(UiNodeBase *source)

{
  UiNodeBase *root;
  int mappingsRemaining;
  int mappingIndex;
  UiNodeBase *ancestorCursor;

  ancestorCursor = source->parent;
  root = source;
  while (ancestorCursor != UI_NODE_NONE) {
    root = root->parent;
    ancestorCursor = root->parent;
  }
  mappingIndex = 0;
  mappingsRemaining = 12;
  do {
    if ((int)source - (int)root == g_UiMappedCommandControlOffsets[mappingIndex]) {
      UiCommandMatrix_SelectIndex(mappingIndex + g_UiCommandSelectionPageBaseIndex,root);
      return;
    }
    mappingIndex = mappingIndex + 1;
    mappingsRemaining = mappingsRemaining + -1;
  } while (mappingsRemaining != 0);
  return;
}


/* Address: 0x00516360.
   Pointer press of the command sprite buttons (nonRightPress and rightPress of g_UiNodeVtable_005162C0,
   g_UiNodeVtable_00516310 and g_UiNodeVtable_00516530): shows the button pressed and starts a new
   activationInputState, marking a double click when the node reports one.
*/
void __thandor_void_preserve_eax_ecx_edx
UiCommandSpriteButtonControl_BeginPress
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


/* Address: 0x005163A0.
   Left/middle button release of the command sprite buttons (nonRightRelease of g_UiNodeVtable_005162C0 and
   g_UiNodeVtable_00516310): when the press started on this button, adds the modifier keys held now to
   activationInputState, plays the activation sound if enabled and queues the button's action; the action
   handler reads activationInputState to choose what to do.
*/
void __thandor_void_preserve_eax_ecx_edx
UiCommandSpriteButtonControl_NonRightRelease
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
    /* stateFlags 0x200: play the activation sound on release */
    if ((((control->sprite).selectable.stateFlags & 0x200) != 0) &&
       ((control->sprite).activationSoundId != 0)) {
      g_SoundPlayOneShot
                (g_UiSoundGainQ15,g_UiSoundGainQ15,
                 (DirectSoundVoiceSet *)(control->sprite).activationSoundId);
    }
    UiActionQueue_Enqueue((control->sprite).selectable.actionId,control);
    UiNode_InvalidateRoot((UiNodeBase *)control);
  }
  return;
}


/* Address: 0x00516410.
   Right button release of the command sprite buttons (rightRelease of g_UiNodeVtable_005162C0,
   g_UiNodeVtable_00516310 and g_UiNodeVtable_00516530): like the left release, but replaces activationInputState
   with the modifier keys plus UI_COMMAND_ACTIVATION_ALTERNATE_BUTTON (which also drops the double-click marker).
*/
void __thandor_void_preserve_eax_ecx_edx
UiCommandSpriteButtonControl_RightRelease
          (UiPointerWheelDelta wheelDelta,UiPixelCoordinate pointerY,UiPixelCoordinate pointerX,
          UiCommandSpriteButtonControl *control)

{
  UiCommandActivationStateFlags inputStateBits;
  UiSelectableStateFlags *stateFlagsField;
  
  if ((((control->sprite).selectable.base.nodeFlags & UI_NODE_SUPPRESSED) == 0) &&
     (inputStateBits = g_KeyboardStateMask & ~UI_COMMAND_ACTIVATION_REPEAT_OR_DOUBLE_CLICK,
     ((control->sprite).selectable.stateFlags & UI_SELECTABLE_SELECTED_OR_CHECKED) != 0)) {
    stateFlagsField = &(control->sprite).selectable.stateFlags;
    *stateFlagsField = *stateFlagsField & ~UI_SELECTABLE_SELECTED_OR_CHECKED;
    control->activationInputState = inputStateBits | UI_COMMAND_ACTIVATION_ALTERNATE_BUTTON;
    /* stateFlags 0x200: play the activation sound on release */
    if ((((control->sprite).selectable.stateFlags & 0x200) != 0) &&
       ((control->sprite).activationSoundId != 0)) {
      g_SoundPlayOneShot
                (g_UiSoundGainQ15,g_UiSoundGainQ15,
                 (DirectSoundVoiceSet *)(control->sprite).activationSoundId);
    }
    UiActionQueue_Enqueue((control->sprite).selectable.actionId,control);
    UiNode_InvalidateRoot((UiNodeBase *)control);
  }
  return;
}


/* Address: 0x00516490.
   Pointer move over an army stock slot (pointerMove of g_UiNodeVtable_005162C0): shows the slot's army asset in
   the selection detail panel and returns the cursor frame, 12 while Ctrl is held (a click then sells the army,
   see InGameCommandSprite_DispatchVariantAControl24), 10 otherwise.
*/
GraphicsCursorFrameIndex __thandor_eax_preserve_ecx_edx
UiCommandSpriteVariantA_PointerMove
          (UiPixelCoordinate pointerY,UiPixelCoordinate pointerX,
          UiCommandSpriteButtonControl *control)

{
  GraphicsCursorFrameIndex cursorFrame;
  int recordIndex;

  if (((control->sprite).selectable.base.nodeFlags & UI_NODE_SUPPRESSED) == 0) {
    recordIndex = 23;
    do {
      if ((int)control - (int)g_InGameRuntimeRoot ==
          g_UiCommandSpriteVariantAOffsetTables[g_UiCommandSpriteVariantAColumnCount][recordIndex])
      {
        g_UiHoverSelectionRecord = g_UiCommandSpriteVariantARecords[recordIndex];
        InGameSelectionDetailPanel_Rebuild();
        break;
      }
      recordIndex = recordIndex + -1;
    } while (-1 < recordIndex);
  }
  cursorFrame = 10;
  if ((g_KeyboardStateMask & KEYBOARD_STATE_CTRL) != 0) {
    cursorFrame = 12;
  }
  return cursorFrame;
}


/* Address: 0x00517F60.
   drawClipped of g_UiCommandVisibilityWrappedTextVtable (the wrapped world view status text): draws the text
   unless g_UiCommandRuntimeFlags bit 0x200 hides all these texts; with label flag 0x800 only while the game is
   paused.
*/
void __thandor_void_preserve_eax_ecx_edx
UiCommandVisibilityWrappedText_DrawWhenAllowed
          (UiPixelCoordinate clipTop,UiPixelCoordinate clipLeft,UiPixelCoordinate clipBottom,
          UiPixelCoordinate clipRight,UiNodeBase *control)

{
  /* bit 0x200 has no writer with a constant mask; it can only come from command 0x310 */
  if (((g_UiCommandRuntimeFlags & 0x200) == 0) &&
     ((((((UiWrappedTextControl *)control)->labelFlags & 0x800) == 0 ||
        ((g_UiCommandRuntimeFlags & UI_COMMAND_RUNTIME_FLAG_PAUSED) != 0)) &&
      ((control->nodeFlags & UI_NODE_SUPPRESSED) == 0)))) {
    UiWrappedTextControl_DrawClipped(clipTop,clipLeft,clipBottom,clipRight,(UiWrappedTextControl *)control);
  }
  return;
}


/* Address: 0x00518010.
   drawClipped of g_UiCommandVisibilitySingleLineTextVtable (the single-line world view texts): same visibility
   rules as the wrapped text; label flag 0x1000 additionally needs g_InGameSimulationStepTicks > 1 and draws the
   text shifted by ticks - 2 bytes (the text pointer is restored afterwards).
*/
void __thandor_void_preserve_eax_ecx_edx
UiCommandVisibilitySingleLineText_DrawWhenAllowed
          (UiPixelCoordinate clipTop,UiPixelCoordinate clipLeft,UiPixelCoordinate clipBottom,
          UiPixelCoordinate clipRight,UiNodeBase *control)

{
  int drawOffsetAdjust;

  drawOffsetAdjust = 0;
  if ((((g_UiCommandRuntimeFlags & 0x200) == 0) &&
      (((((UiSingleLineTextControl *)control)->labelFlags & 0x800) == 0 ||
        ((g_UiCommandRuntimeFlags & UI_COMMAND_RUNTIME_FLAG_PAUSED) != 0)))) &&
     (((((UiSingleLineTextControl *)control)->labelFlags & 0x1000) == 0 ||
      (drawOffsetAdjust = g_InGameSimulationStepTicks - 2, 1 < g_InGameSimulationStepTicks)))) {
    ((UiSingleLineTextControl *)control)->text = (uint16_t *)((int)((UiSingleLineTextControl *)control)->text + drawOffsetAdjust);
    UiSingleLineTextControl_DrawClipped(clipTop,clipLeft,clipBottom,clipRight,(UiSingleLineTextControl *)control);
    ((UiSingleLineTextControl *)control)->text = (uint16_t *)((int)((UiSingleLineTextControl *)control)->text - drawOffsetAdjust);
  }
  return;
}


/* Address: 0x0055F4A0.
   In-game command handler 0x370 (key P): toggles the player's pause request, then toggles the global pause once
   every player agrees - the game pauses when all players request it and resumes when none does any more.
*/
void __thandor_void_preserve_eax_ecx_edx
InGameCommandMode_TogglePlayerFlagBit0AndReconcileGlobal
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


/* Address: 0x005604D0.
   In-game command handler INGAME_COMMAND_PLACE_ARMY: takes the player's pending army asset (stock entry chosen
   in the army stock panel), validates the placement at the clicked point and creates the army there with the
   given heading, counts it for the faction and spawns the asset's placement effect. A rejected placement leaves
   the asset pending; a successful one ends the local placement mode.
*/
void __thandor_void_preserve_eax_ecx_edx
InGameCommand_ExecuteLocalPlacementFromSelection
          (PlayerRuntimeId playerId,CommandPayloadDword04 headingAngle,
          CommandPayloadDword08 worldXQ12,CommandPayloadDword0C worldYQ12)

{
  FactionRelationCounter *relationCounter;
  uint32_t pendingEntryOrFactionToken;
  SelectionPlayerRuntimeBlock *playerBlock;
  ArmyRuntimeSlot *modelNodeRuntime;
  ArmyRuntimeSlot *armySlot;
  ModelRuntimeSlot *slotModelRuntime;
  InGameRuntimeRootImageC3E4 *runtimeRoot;
  ArmyRuntimeSlot **createdArmySlots;
  WorldRuntimeContext *worldRuntime;
  bool placementRejected;
  ArmyRuntimeCreateResult createResult;

  runtimeRoot = g_InGameRuntimeRoot;
  playerBlock = g_SelectionPlayerRuntimeBlockPointers[playerId];
  worldRuntime = &g_InGameRuntimeRoot->worldRuntime0A30;
  /* XCHG in the original: take the pending entry and clear it atomically */
  LOCK();
  pendingEntryOrFactionToken = playerBlock->pendingSelectionEntityOffset8098;
  playerBlock->pendingSelectionEntityOffset8098 = 0;
  UNLOCK();
  if (pendingEntryOrFactionToken != 0) {
    /* the pending entry holds the army asset id at +8 */
    placementRejected = ArmyPlacement_ValidateAssetAtPointAndCellCorners
                      (0,headingAngle,worldXQ12,worldYQ12,
                       *(ArmyPlacementContext *)(pendingEntryOrFactionToken + 8),playerBlock->primaryEntityOrFactionToken8080,
                       worldRuntime);
    if (!placementRejected) {
      /* ECX/EDX of the validator: the accepted (possibly snapped) point. */
      createResult = ArmyRuntime_CreateInstanceFromAsset
                        (4,headingAngle,g_ArmyPlacementValidatedWorldYQ12,
                         g_ArmyPlacementValidatedWorldXQ12,
                         playerBlock->primaryEntityOrFactionToken8080,
                         *(PckArmyAssetIdCatalog *)(pendingEntryOrFactionToken + 8),worldRuntime);
      createdArmySlots = (ArmyRuntimeSlot **)createResult.armyRuntimeOrError;
      if (!createResult.failed) {
        pendingEntryOrFactionToken = playerBlock->primaryEntityOrFactionToken8080;
        modelNodeRuntime = createdArmySlots[1];
        armySlot = *createdArmySlots;
        /* pendingEntryOrFactionToken now holds the owning faction */
        modelNodeRuntime->movementPosition0Q12 = 0;
        if (pendingEntryOrFactionToken == (runtimeRoot->worldRuntime0A30).activeFactionRuntimeIndex) {
          modelNodeRuntime->movementPosition0Q12 = 0x7fffffff;
        }
        relationCounter = &g_GameFactionRuntimeImage.records[pendingEntryOrFactionToken].relationCounterB;
        *relationCounter = *relationCounter + 1;
        slotModelRuntime = (armySlot->modelRuntimeOrSavedOffset).modelRuntime;
        ModelNodeRuntime_RebuildTransformsFromRoot((ModelRuntimeNode *)modelNodeRuntime);
        ArmyRuntime_DispatchClassCommand(createdArmySlots,worldRuntime);
        EffectRuntimePool_CreateInstanceFromDefinition
                  (EFFECT_RUNTIME_COMPLETION_NONE,THANDOR_BITCAST(int, EffectRuntimeOwnerReference4, 0x0),
                   (modelNodeRuntime->movementControl).turnVelocityAngle16,
                   (modelNodeRuntime->movementControl).movementAdvancePerTickQ12,
                   ((WorldRuntimeNodeModelPayload *)&modelNodeRuntime->factionIndex)->
                   worldRotationAngle0,modelNodeRuntime->depthBinClass,
                   modelNodeRuntime->runtimeState98,
                   ((GraphicsFixedVec3 *)&modelNodeRuntime->runtimeState94)->x,
                   (EffectDefinition *)slotModelRuntime->attachments140[2].childLocalRotationAngle0,
                   worldRuntime);
        UiCatalogGroup48_RebuildGrid((UiNodeBase *)g_InGameRuntimeRoot);
        UiCatalogGroup42_RebuildGrid((UiNodeBase *)g_InGameRuntimeRoot);
        if (playerId != g_LocalPlayerRuntimeId) {
          return;
        }
        g_UiCommandRuntimeFlags = g_UiCommandRuntimeFlags & ~UI_COMMAND_RUNTIME_FLAG_PLACEMENT_PENDING;
        g_InGamePendingPlacementArmyAsset = 0;
        return;
      }
    }
    g_SelectionPlayerRuntimeBlockPointers[playerId]->pendingSelectionEntityOffset8098 = pendingEntryOrFactionToken;
  }
  return;
}


/* Address: 0x0056A2A0.
   Rebuilds the army stock panel: the active faction's pooled army assets that have a texture (at most 24,
   none while the world input is disabled) fill g_UiCommandSpriteVariantARecords and the slot buttons in a
   grid of at most four columns; the frame is sized to the grid (smaller margins below 800 pixels width) and
   hidden when the stock is empty, unused slots are hidden.
*/
void __thandor_void_preserve_eax_ecx_edx UiCommandSpriteVariantA_RebuildGrid(UiNodeBase *node)

{
  uint32_t *controlFlags;
  UiNodeBase *parentCursor;
  int32_t *offsetTable;
  UiCommandRuntimeRecordPrefix *runtimeRecord;
  uint32_t columnCount;
  GraphicsTextureSourceAsset *slotTexture;
  int countWidthOrOffset;
  uint32_t itemCount;
  FactionArmyAssetCount remainingAssets;
  int panelHeight;
  uint32_t slotIndex;
  UiCommandRuntimeRecordPrefix **recordCursor;
  uint32_t *assetCursor;
  UiGridDimensionsEdxEax8 gridDimensions;

  /* node becomes the in-game UI root (parent -1) */
  parentCursor = node->parent;
  while (parentCursor != (UiNodeBase *)0xffffffff) {
    node = node->parent;
    parentCursor = node->parent;
  }
  /* countWidthOrOffset: slot counter here, then the extra frame width, then a slot's control offset */
  recordCursor = g_UiCommandSpriteVariantARecords;
  for (countWidthOrOffset = 24; countWidthOrOffset != 0; countWidthOrOffset--) {
    *recordCursor = NULL;
    recordCursor++;
  }
  recordCursor = g_UiCommandSpriteVariantARecords;
  remainingAssets = g_GameFactionRuntimeImage.records[((WorldRuntimeContext *)INGAME_UI(node,worldView))->activeFactionRuntimeIndex].primaryArmyAssetCount;
  itemCount = 0;
  assetCursor = g_GameFactionRuntimeImage.records[((WorldRuntimeContext *)INGAME_UI(node,worldView))->activeFactionRuntimeIndex].primaryArmyAssetPointersOrIds;
  if ((remainingAssets != 0) && ((g_UiCommandRuntimeFlags & UI_COMMAND_RUNTIME_FLAG_WORLD_INPUT_DISABLED) == 0)) {
    do {
      if ((((UiCommandRuntimeRecordPrefix *)*assetCursor)->textureSource != NULL) && (itemCount < 24)) {
        *recordCursor = (UiCommandRuntimeRecordPrefix *)*assetCursor;
        itemCount++;
        recordCursor++;
      }
      assetCursor++;
      remainingAssets--;
    } while (remainingAssets != 0);
  }
  gridDimensions = UiGrid_ComputeDimensionsPacked(6,itemCount);
  columnCount = (uint32_t)gridDimensions;
  if (4 < columnCount) {
    columnCount = 4;
  }
  countWidthOrOffset = columnCount * g_InGamePanelTextureSubresource34Width + g_InGamePanelTextureSubresource27Width +
          g_InGamePanelTextureSubresource28Width;
  panelHeight = (int)(gridDimensions >> 32) * g_InGamePanelTextureSubresource34Height +
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
  INGAME_UI(node,armyStockFrame)->leftOffset = INGAME_UI(node,armyStockFrame)->leftOffset - countWidthOrOffset;
  INGAME_UI(node,armyStockFrame)->topOffset = INGAME_UI(node,armyStockFrame)->topOffset - panelHeight;
  if (itemCount == 0) {
    INGAME_UI(node,armyStockFrame)->nodeFlags = INGAME_UI(node,armyStockFrame)->nodeFlags | UI_NODE_SUPPRESSED;
  }
  else {
    INGAME_UI(node,armyStockFrame)->nodeFlags = INGAME_UI(node,armyStockFrame)->nodeFlags & ~UI_NODE_SUPPRESSED;
  }
  offsetTable = g_UiCommandSpriteVariantAOffsetTables[columnCount];
  slotIndex = 0;
  recordCursor = g_UiCommandSpriteVariantARecords;
  do {
    countWidthOrOffset = offsetTable[slotIndex];
    runtimeRecord = *recordCursor;
    if (slotIndex < itemCount) {
      controlFlags = (uint32_t *)((int)&node->nodeFlags + countWidthOrOffset);
      *controlFlags = *controlFlags & ~UI_NODE_SUPPRESSED;
      slotTexture = runtimeRecord->textureSource;
    }
    else {
      controlFlags = (uint32_t *)((int)&node->nodeFlags + countWidthOrOffset);
      *controlFlags = *controlFlags | UI_NODE_SUPPRESSED;
      slotTexture = NULL;
    }
    slotIndex++;
    ((UiCommandSpriteButtonControl *)((int)node + countWidthOrOffset))->sprite.primaryTextureSource = slotTexture;
    recordCursor++;
  } while (slotIndex < 24);
  INGAME_UI(node,armyStockPanel)->vtable->layout(INGAME_UI(node,armyStockPanel));
  return;
}


/* Address: 0x0056AFD0.
   Technology window close button (action 0x1011, g_InGameUiActionHandlersPage10[17]): shows the world view again,
   closes the window (page 0 of the game window page stack) and, if something is selected, sends
   INGAME_COMMAND_CLOSE_TECHNOLOGY_PAGE with -1 (cancel) for the first selected building, which gives back what
   opening the page took away.
*/
void __thandor_void_preserve_eax_ecx_edx
InGameCommandAction_ClearSelectedArmyTokenAndClosePage(UiNodeBase *control)

{
  UiNodeBase *parentCursor;
  GameEntityRuntime *firstSelectedEntity;
  CommandPayloadDword04 modelOffset;

  /* control becomes the in-game UI root (parent -1) */
  parentCursor = control->parent;
  while (parentCursor != UI_NODE_NONE) {
    control = control->parent;
    parentCursor = control->parent;
  }
  INGAME_UI(control,worldView)->nodeFlags = INGAME_UI(control,worldView)->nodeFlags & ~UI_NODE_SUPPRESSED;
  UiPageStack_SetActiveIndex(0,(UiPageStackControl *)INGAME_UI(control,gameWindowPageStack));
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


/* Address: 0x0056C660.
   UI action 0x1200 (game menu quit button): opens the quit game window (page 4 of the in-game window page
   stack). Its restart button is only offered in local games, its surrender button only while the local
   faction is still in play (world input enabled).
*/
void __thandor_preserve_eax
InGameCommandPanel_OpenPage4AndRefreshAvailability(InGameCommandPanelSourceAddress32 source)

{
  UiNodeBase *firstNode;

  /* source is InGameUiImage.gameMenuQuitButton (+0x25B0); -0x19E0 lands on gameWindowPageStack (+0xBD0) */
  UiPageStack_SetActiveIndex(4,(UiPageStackControl *)(source + -0x19e0));
  firstNode = (UiNodeBase *)(source + -0x25b0); /* the in-game UI root */
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


/* Address: 0x0056CFA0.
   Build catalog entry click (action 0x100B, g_InGameUiActionHandlersPage10[11]): finds the entry among the 48
   build catalog slots of the current column layout and queues its army asset for the active faction, or with Ctrl
   (activationInputState & KEYBOARD_STATE_CTRL) cancels a queued one with refund. Ignored while paused or while the
   world input is disabled.
*/
void __thandor_void_preserve_eax_ecx_edx
InGameCommandCatalog_SubmitGroup48Entry(UiCatalogEntryControl *source)

{
  UiNodeBase *parentOrFactionIndex; /* ancestor cursor, then the active faction index (one register) */
  UiCatalogEntryControl *root;
  PckArmyAssetIdCatalog assetId;
  int entryIndex;

  if ((g_UiCommandRuntimeFlags &
      (UI_COMMAND_RUNTIME_FLAG_PAUSED | UI_COMMAND_RUNTIME_FLAG_WORLD_INPUT_DISABLED)) == 0) {
    parentOrFactionIndex = (source->command).sprite.selectable.base.parent;
    root = source;
    while (parentOrFactionIndex != UI_NODE_NONE) {
      root = (UiCatalogEntryControl *)(root->command).sprite.selectable.base.parent;
      parentOrFactionIndex = (root->command).sprite.selectable.base.parent;
    }
    entryIndex = 47;
    while ((int)source - (int)root !=
           g_UiCatalogGroup48OffsetTables[g_UiCatalogGroup48ColumnCount][entryIndex]) {
      entryIndex = entryIndex + -1;
      if (entryIndex < 0) {
        return;
      }
    }
    if (((source->command).activationInputState & UI_COMMAND_ACTIVATION_RELATION_RESET_REQUEST_MASK)
        == 0) {
      parentOrFactionIndex =
           (UiNodeBase *)((WorldRuntimeContext *)INGAME_UI(root,worldView))->activeFactionRuntimeIndex;
      assetId = g_UiCatalogGroup48Records[entryIndex]->armyAssetId;
      if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_NETWORKED_MASK) ==
          SESSION_NETWORK_ROLE_LOCAL) {
        GameFactionRuntime_RegisterArmyAssetPointers
                  (g_LocalPlayerRuntimeId,1,assetId,(FactionRuntimeIndex)parentOrFactionIndex);
      }
      else {
        InGameCommandQueue_AppendLocalPlayerCommand
                  (INGAME_COMMAND_QUEUE_ARMY,1,assetId,(CommandPayloadDword04)parentOrFactionIndex);
      }
    }
    else {
      parentOrFactionIndex =
           (UiNodeBase *)((WorldRuntimeContext *)INGAME_UI(root,worldView))->activeFactionRuntimeIndex;
      assetId = g_UiCatalogGroup48Records[entryIndex]->armyAssetId;
      if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_NETWORKED_MASK) ==
          SESSION_NETWORK_ROLE_LOCAL) {
        GameFactionRuntime_CancelQueuedArmyAssetsAndRefund
                  (g_LocalPlayerRuntimeId,1,assetId,(FactionRuntimeIndex)parentOrFactionIndex);
      }
      else {
        InGameCommandQueue_AppendLocalPlayerCommand
                  (INGAME_COMMAND_CANCEL_QUEUED_ARMY,1,assetId,(CommandPayloadDword04)parentOrFactionIndex);
      }
    }
  }
  return;
}


/* Address: 0x0056D090.
   Special build catalog entry click (action 0x100C, g_InGameUiActionHandlersPage10[12]): the same as
   InGameCommandCatalog_SubmitGroup48Entry for the 42 slots of the special build catalog.
*/
void __thandor_void_preserve_eax_ecx_edx
InGameCommandCatalog_SubmitGroup42Entry(UiCatalogEntryControl *source)

{
  UiNodeBase *parentOrFactionIndex; /* ancestor cursor, then the active faction index (one register) */
  UiCatalogEntryControl *root;
  PckArmyAssetIdCatalog assetId;
  int entryIndex;

  if ((g_UiCommandRuntimeFlags &
      (UI_COMMAND_RUNTIME_FLAG_PAUSED | UI_COMMAND_RUNTIME_FLAG_WORLD_INPUT_DISABLED)) == 0) {
    parentOrFactionIndex = (source->command).sprite.selectable.base.parent;
    root = source;
    while (parentOrFactionIndex != UI_NODE_NONE) {
      root = (UiCatalogEntryControl *)(root->command).sprite.selectable.base.parent;
      parentOrFactionIndex = (root->command).sprite.selectable.base.parent;
    }
    entryIndex = 41;
    while ((int)source - (int)root !=
           g_UiCatalogGroup42OffsetTables[g_UiCatalogGroup42ColumnCount][entryIndex]) {
      entryIndex = entryIndex + -1;
      if (entryIndex < 0) {
        return;
      }
    }
    if (((source->command).activationInputState & UI_COMMAND_ACTIVATION_RELATION_RESET_REQUEST_MASK)
        == 0) {
      parentOrFactionIndex =
           (UiNodeBase *)((WorldRuntimeContext *)INGAME_UI(root,worldView))->activeFactionRuntimeIndex;
      assetId = g_UiCatalogGroup42Records[entryIndex]->armyAssetId;
      if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_NETWORKED_MASK) ==
          SESSION_NETWORK_ROLE_LOCAL) {
        GameFactionRuntime_RegisterArmyAssetPointers
                  (g_LocalPlayerRuntimeId,1,assetId,(FactionRuntimeIndex)parentOrFactionIndex);
      }
      else {
        InGameCommandQueue_AppendLocalPlayerCommand
                  (INGAME_COMMAND_QUEUE_ARMY,1,assetId,(CommandPayloadDword04)parentOrFactionIndex);
      }
    }
    else {
      parentOrFactionIndex =
           (UiNodeBase *)((WorldRuntimeContext *)INGAME_UI(root,worldView))->activeFactionRuntimeIndex;
      assetId = g_UiCatalogGroup42Records[entryIndex]->armyAssetId;
      if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_NETWORKED_MASK) ==
          SESSION_NETWORK_ROLE_LOCAL) {
        GameFactionRuntime_CancelQueuedArmyAssetsAndRefund
                  (g_LocalPlayerRuntimeId,1,assetId,(FactionRuntimeIndex)parentOrFactionIndex);
      }
      else {
        InGameCommandQueue_AppendLocalPlayerCommand
                  (INGAME_COMMAND_CANCEL_QUEUED_ARMY,1,assetId,(CommandPayloadDword04)parentOrFactionIndex);
      }
    }
  }
  return;
}


/* Address: 0x0056D180.
   Army stock slot click (action 0x1001, g_InGameUiActionHandlersPage10[1]): first drops any army still waiting
   for placement (command 0x14F0), then takes the slot's army for placement on the map, or sells it with Ctrl
   (activationInputState & KEYBOARD_STATE_CTRL). Ignored while paused, while the world input is disabled and while
   world runtime flag 0x10 is set.
*/
void __thandor_void_preserve_eax_ecx_edx
InGameCommandSprite_DispatchVariantAControl24(UiCommandSpriteButtonControl *control)

{
  int32_t *flagsField;
  UiNodeBase *parentCursor;
  UiCommandSpriteButtonControl *root;
  UiCommandRuntimeRecordPrefix *runtimeRecord;
  GraphicsTextureSourceAsset *factionToken; /* holds the active faction index */
  PckArmyAssetIdCatalog assetId;
  int slotIndex;

  if ((g_UiCommandRuntimeFlags &
      (UI_COMMAND_RUNTIME_FLAG_PAUSED | UI_COMMAND_RUNTIME_FLAG_WORLD_INPUT_DISABLED)) == 0) {
    parentCursor = (control->sprite).selectable.base.parent;
    root = control;
    while (parentCursor != UI_NODE_NONE) {
      root = (UiCommandSpriteButtonControl *)(root->sprite).selectable.base.parent;
      parentCursor = (root->sprite).selectable.base.parent;
    }
    /* end any hover of the stock panel (image control) */
    g_UiImageControlHoverTarget = NULL;
    flagsField = (int32_t *)&((UiImageControl *)INGAME_UI(root,armyStockPanel))->selectable.stateFlags;
    *flagsField = *flagsField & 0xfffff9fc;
    if ((((WorldRuntimeContext *)INGAME_UI(root,worldView))->runtimeFlags & 0x10U) == 0) {
      slotIndex = 23;
      while ((int)control - (int)root !=
             g_UiCommandSpriteVariantAOffsetTables[g_UiCommandSpriteVariantAColumnCount][slotIndex]) {
        slotIndex = slotIndex + -1;
        if (slotIndex < 0) {
          return;
        }
      }
      runtimeRecord = g_UiCommandSpriteVariantARecords[slotIndex];
      factionToken = (GraphicsTextureSourceAsset *)
                     ((WorldRuntimeContext *)INGAME_UI(root,worldView))->activeFactionRuntimeIndex;
      if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_NETWORKED_MASK) ==
          SESSION_NETWORK_ROLE_LOCAL) {
        GameFactionRuntime_ConsumePendingArmyAssetAndRefreshGrid
                  (g_LocalPlayerRuntimeId,0,0,(FactionRuntimeIndex)factionToken);
      }
      else {
        InGameCommandQueue_AppendLocalPlayerCommand
                  (INGAME_COMMAND_CONSUME_PENDING_ARMY,0,0,(CommandPayloadDword04)factionToken);
      }
      if ((control->activationInputState & UI_COMMAND_ACTIVATION_RELATION_RESET_REQUEST_MASK) == 0)
      {
        factionToken = (GraphicsTextureSourceAsset *)
                       ((WorldRuntimeContext *)INGAME_UI(root,worldView))->activeFactionRuntimeIndex;
        assetId = runtimeRecord->armyAssetId;
        if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_NETWORKED_MASK) ==
            SESSION_NETWORK_ROLE_LOCAL) {
          GameFactionRuntime_RemoveArmyAssetAndStagePlayerTransfer
                    (g_LocalPlayerRuntimeId,0,assetId,(FactionRuntimeIndex)factionToken);
        }
        else {
          InGameCommandQueue_AppendLocalPlayerCommand
                    (INGAME_COMMAND_TAKE_ARMY_FOR_PLACEMENT,0,assetId,(CommandPayloadDword04)factionToken);
        }
      }
      else {
        factionToken = (GraphicsTextureSourceAsset *)
                       ((WorldRuntimeContext *)INGAME_UI(root,worldView))->activeFactionRuntimeIndex;
        assetId = runtimeRecord->armyAssetId;
        if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_NETWORKED_MASK) ==
            SESSION_NETWORK_ROLE_LOCAL) {
          GameFactionRuntime_SellArmyAssetAndRefundSevenEighths
                    (g_LocalPlayerRuntimeId,0,assetId,(FactionRuntimeIndex)factionToken);
        }
        else {
          InGameCommandQueue_AppendLocalPlayerCommand
                    (INGAME_COMMAND_SELL_ARMY,0,assetId,(CommandPayloadDword04)factionToken);
        }
      }
    }
  }
  return;
}


/* Address: 0x0056D540.
   Selection group button click (action 0x100A, g_InGameUiActionHandlersPage10[10]; the 8 buttons of
   g_UiAction100AControlOffsets): the mouse version of the 1..8 group keys. A plain click recalls the group, a
   modifier key merges (SELECTION_TRANSFER_MERGE), the right button stores the selection into the group
   (SELECTION_TRANSFER_TO_GROUP) and a double click also centres the view. Every variant except the plain recall
   is refused when SelectionInfo_AllEntriesEmptyOrMatchOwner reports so for the active faction.
*/
void __thandor_void_preserve_eax_ecx_edx
InGameCommandSprite_DispatchFixedControl8(UiCommandSpriteButtonControl *control)

{
  UiNodeBase *parentCursor;
  UiCommandSpriteButtonControl *root;
  GraphicsTextureSourceAsset *factionToken; /* holds the active faction index */
  CommandPayloadDword04 groupIndex;
  CommandPayloadDword08 transferModeFlags;
  bool selectionBlocked;

  if ((g_UiCommandRuntimeFlags &
      (UI_COMMAND_RUNTIME_FLAG_PAUSED | UI_COMMAND_RUNTIME_FLAG_WORLD_INPUT_DISABLED)) == 0) {
    parentCursor = (control->sprite).selectable.base.parent;
    root = control;
    while (parentCursor != UI_NODE_NONE) {
      root = (UiCommandSpriteButtonControl *)(root->sprite).selectable.base.parent;
      parentCursor = (root->sprite).selectable.base.parent;
    }
    groupIndex = SELECTION_GROUP_COUNT - 1;
    do {
      if ((int)control - (int)root == g_UiAction100AControlOffsets[groupIndex]) {
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
           (selectionBlocked = SelectionInfo_AllEntriesEmptyOrMatchOwner
                              ((FactionRuntimeIndex)
                               ((WorldRuntimeContext *)INGAME_UI(root,worldView))->activeFactionRuntimeIndex),
           selectionBlocked)) {
          return;
        }
        factionToken = (GraphicsTextureSourceAsset *)
                       ((WorldRuntimeContext *)INGAME_UI(root,worldView))->activeFactionRuntimeIndex;
        if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_NETWORKED_MASK) !=
            SESSION_NETWORK_ROLE_LOCAL) {
          InGameCommandQueue_AppendLocalPlayerCommand
                    (INGAME_COMMAND_SELECTION_GROUP,(CommandPayloadDword0C)factionToken,transferModeFlags,
                     groupIndex);
          return;
        }
        FrontendPlayerSelection_TransferFactionGroupWithModeAndRefresh
                  (g_LocalPlayerRuntimeId,(FactionRuntimeIndex)factionToken,transferModeFlags,
                   groupIndex);
        return;
      }
      groupIndex = groupIndex - 1;
    } while (-1 < (int)groupIndex);
  }
  return;
}


/* Address: 0x0056D620.
   Second results screen button (action 0x1025, g_InGameUiActionHandlersPage10[37]; resultsSecondaryExitButton,
   only offered in network games): sets UI_COMMAND_RUNTIME_FLAG_RESULTS_CLOSED on this machine only.
*/
void InGameCommandState_SetRuntimeFlag1000(UiNodeBase *source)

{
  g_UiCommandRuntimeFlags = g_UiCommandRuntimeFlags | UI_COMMAND_RUNTIME_FLAG_RESULTS_CLOSED;
  return;
}

/* Address: 0x0056D640.
   Results chart mode buttons (action 0x1026, g_InGameUiActionHandlersPage10[38]): selects the clicked one of the
   two buttons and copies the chosen mode (0 or 1) into the modeFlags of the three results charts (graph or table
   drawing) and into the image subresource of the results screen background.
*/
void __thandor_void_preserve_eax_ecx_edx
InGameCommandState_SelectAndPropagateBinaryMode(UiSelectableControl *source)

{
  UiNodeBase *parentCursor;
  UiSelectableControl *root;
  UiNodeVtable *selectedIndexValue;
  SelectableGroupNodeResult selectionResult;
  
  parentCursor = (source->base).parent;
  root = source;
  while (parentCursor != UI_NODE_NONE) {
    root = (UiSelectableControl *)(root->base).parent;
    parentCursor = (root->base).parent;
  }
  UiSelectableGroup_SelectExclusive(2,&source->base,
      INGAME_UI(root,resultsChartModeButtonB),
      INGAME_UI(root,resultsChartModeButtonA));
  selectionResult = UiSelectableGroup_NoneVisibleSelected(2,
      INGAME_UI(root,resultsChartModeButtonA),
      INGAME_UI(root,resultsChartModeButtonB));
  selectedIndexValue = (UiNodeVtable *)selectionResult.controlIndexOrCount;
  /* Mode 0/1 picks each chart's drawing path (modeFlags bit 0) and the results background image. */
  ((FrontendResultsColumnSequenceControl68 *)INGAME_UI(root,resultsChart1))->modeFlags =
       (uint32_t)selectedIndexValue;
  ((FrontendResultsColumnSequenceControl68 *)INGAME_UI(root,resultsChart2))->modeFlags =
       (uint32_t)selectedIndexValue;
  ((FrontendResultsColumnSequenceControl68 *)INGAME_UI(root,resultsChart3))->modeFlags =
       (uint32_t)selectedIndexValue;
  ((UiImagePanelControl *)INGAME_UI(root,resultsScreenPanel))->subresource =
       (GraphicsSubresourceIndex)selectedIndexValue;
  return;
}


/* Address: 0x0056D920.
   Hides the grid vertex markers of the world view (clears WORLD_RUNTIME_FLAG_DRAW_GRID_VERTEX_MARKERS); called
   when the editor is switched off (InGameUiCommandRuntime_ApplyInteractionSubsystemActiveState).
*/
void __thandor_void_preserve_eax_ecx_edx UiCommandModeG_ClearNodeFlag00800000(WorldRuntimeContext *context)

{
  context->runtimeFlags = context->runtimeFlags & ~WORLD_RUNTIME_FLAG_DRAW_GRID_VERTEX_MARKERS;
  return;
}


/* Address: 0x0056DD50.
   Height tool option 0 (action 0x1108, g_InGameUiCommandModeActionHandlers30[8]; also the editor hotkeys in
   ui/ingame/runtime.c): selects heightToolOption0 among the four height tool buttons and sets g_UiCommandModeC = 0.
*/
void __thandor_void_preserve_eax_ecx_edx InGameCommandModeC_Select0(UiSpriteButtonControl *source)

{
  UiSelectableGroup_SelectExclusive(4,(UiNodeBase *)source,
      THANDOR_UI_SIBLING(source,InGameUiImage,heightToolOption0,heightToolOption3),
      THANDOR_UI_SIBLING(source,InGameUiImage,heightToolOption0,heightToolOption2),
      THANDOR_UI_SIBLING(source,InGameUiImage,heightToolOption0,heightToolOption1),
      THANDOR_UI_SIBLING(source,InGameUiImage,heightToolOption0,heightToolOption0));
  g_UiCommandModeC = 0;
  return;
}


/* Address: 0x0056DDA0.
   Height tool option 1 (action 0x1109, g_InGameUiCommandModeActionHandlers30[9]; also the editor hotkeys in
   ui/ingame/runtime.c): selects heightToolOption1 among the four height tool buttons and sets g_UiCommandModeC = 1.
*/
void __thandor_void_preserve_eax_ecx_edx InGameCommandModeC_Select1(UiSpriteButtonControl *source)

{
  UiSelectableGroup_SelectExclusive(4,(UiNodeBase *)source,
      THANDOR_UI_SIBLING(source,InGameUiImage,heightToolOption1,heightToolOption3),
      THANDOR_UI_SIBLING(source,InGameUiImage,heightToolOption1,heightToolOption2),
      THANDOR_UI_SIBLING(source,InGameUiImage,heightToolOption1,heightToolOption1),
      THANDOR_UI_SIBLING(source,InGameUiImage,heightToolOption1,heightToolOption0));
  g_UiCommandModeC = 1;
  return;
}


/* Address: 0x0056DDF0.
   Height tool option 2 (action 0x110A, g_InGameUiCommandModeActionHandlers30[10]; also the editor hotkeys in
   ui/ingame/runtime.c): selects heightToolOption2 among the four height tool buttons and sets g_UiCommandModeC = 2.
*/
void __thandor_void_preserve_eax_ecx_edx InGameCommandModeC_Select2(UiSpriteButtonControl *source)

{
  UiSelectableGroup_SelectExclusive(4,(UiNodeBase *)source,
      THANDOR_UI_SIBLING(source,InGameUiImage,heightToolOption2,heightToolOption3),
      THANDOR_UI_SIBLING(source,InGameUiImage,heightToolOption2,heightToolOption2),
      THANDOR_UI_SIBLING(source,InGameUiImage,heightToolOption2,heightToolOption1),
      THANDOR_UI_SIBLING(source,InGameUiImage,heightToolOption2,heightToolOption0));
  g_UiCommandModeC = 2;
  return;
}


/* Address: 0x0056DE40.
   Height tool option 3 (action 0x110B, g_InGameUiCommandModeActionHandlers30[11]; also the editor hotkeys in
   ui/ingame/runtime.c): selects heightToolOption3 among the four height tool buttons and sets g_UiCommandModeC = 3.
*/
void __thandor_void_preserve_eax_ecx_edx InGameCommandModeC_Select3(UiSpriteButtonControl *source)

{
  UiSelectableGroup_SelectExclusive(4,(UiNodeBase *)source,
      THANDOR_UI_SIBLING(source,InGameUiImage,heightToolOption3,heightToolOption3),
      THANDOR_UI_SIBLING(source,InGameUiImage,heightToolOption3,heightToolOption2),
      THANDOR_UI_SIBLING(source,InGameUiImage,heightToolOption3,heightToolOption1),
      THANDOR_UI_SIBLING(source,InGameUiImage,heightToolOption3,heightToolOption0));
  g_UiCommandModeC = 3;
  return;
}


/* Address: 0x0056DE90.
   Material tool option 0 (action 0x110C, g_InGameUiCommandModeActionHandlers30[12]; also the editor hotkeys in
   ui/ingame/runtime.c): selects materialToolOption0 among the four material tool buttons and sets
   g_UiCommandModeD = 0.
*/
void __thandor_void_preserve_eax_ecx_edx InGameCommandModeD_Select0(UiSpriteButtonControl *source)

{
  UiSelectableGroup_SelectExclusive(4,(UiNodeBase *)source,
      THANDOR_UI_SIBLING(source,InGameUiImage,materialToolOption0,materialToolOption3),
      THANDOR_UI_SIBLING(source,InGameUiImage,materialToolOption0,materialToolOption2),
      THANDOR_UI_SIBLING(source,InGameUiImage,materialToolOption0,materialToolOption1),
      THANDOR_UI_SIBLING(source,InGameUiImage,materialToolOption0,materialToolOption0));
  g_UiCommandModeD = 0;
  return;
}


/* Address: 0x0056DEE0.
   Material tool option 1 (action 0x110D, g_InGameUiCommandModeActionHandlers30[13]; also the editor hotkeys in
   ui/ingame/runtime.c): selects materialToolOption1 among the four material tool buttons and sets
   g_UiCommandModeD = 1.
*/
void __thandor_void_preserve_eax_ecx_edx InGameCommandModeD_Select1(UiSpriteButtonControl *source)

{
  UiSelectableGroup_SelectExclusive(4,(UiNodeBase *)source,
      THANDOR_UI_SIBLING(source,InGameUiImage,materialToolOption1,materialToolOption3),
      THANDOR_UI_SIBLING(source,InGameUiImage,materialToolOption1,materialToolOption2),
      THANDOR_UI_SIBLING(source,InGameUiImage,materialToolOption1,materialToolOption1),
      THANDOR_UI_SIBLING(source,InGameUiImage,materialToolOption1,materialToolOption0));
  g_UiCommandModeD = 1;
  return;
}


/* Address: 0x0056DF30.
   Material tool option 2 (action 0x110E, g_InGameUiCommandModeActionHandlers30[14]; also the editor hotkeys in
   ui/ingame/runtime.c): selects materialToolOption2 among the four material tool buttons and sets
   g_UiCommandModeD = 2.
*/
void __thandor_void_preserve_eax_ecx_edx InGameCommandModeD_Select2(UiSpriteButtonControl *source)

{
  UiSelectableGroup_SelectExclusive(4,(UiNodeBase *)source,
      THANDOR_UI_SIBLING(source,InGameUiImage,materialToolOption2,materialToolOption3),
      THANDOR_UI_SIBLING(source,InGameUiImage,materialToolOption2,materialToolOption2),
      THANDOR_UI_SIBLING(source,InGameUiImage,materialToolOption2,materialToolOption1),
      THANDOR_UI_SIBLING(source,InGameUiImage,materialToolOption2,materialToolOption0));
  g_UiCommandModeD = 2;
  return;
}


/* Address: 0x0056DF80.
   Material tool option 3 (action 0x110F, g_InGameUiCommandModeActionHandlers30[15]; also the editor hotkeys in
   ui/ingame/runtime.c): selects materialToolOption3 among the four material tool buttons and sets
   g_UiCommandModeD = 3.
*/
void __thandor_void_preserve_eax_ecx_edx InGameCommandModeD_Select3(UiSpriteButtonControl *source)

{
  UiSelectableGroup_SelectExclusive(4,(UiNodeBase *)source,
      THANDOR_UI_SIBLING(source,InGameUiImage,materialToolOption3,materialToolOption3),
      THANDOR_UI_SIBLING(source,InGameUiImage,materialToolOption3,materialToolOption2),
      THANDOR_UI_SIBLING(source,InGameUiImage,materialToolOption3,materialToolOption1),
      THANDOR_UI_SIBLING(source,InGameUiImage,materialToolOption3,materialToolOption0));
  g_UiCommandModeD = 3;
  return;
}


/* Address: 0x0056E050.
   Unit placement option 0 (action 0x1111, g_InGameUiCommandModeActionHandlers30[17]; also the editor hotkeys in
   ui/ingame/runtime.c): selects unitPlacementOption0 among the three unit placement buttons and sets
   g_UiCommandModeA = 0.
*/
void __thandor_void_preserve_eax_ecx_edx InGameCommandModeA_Select0(UiSpriteButtonControl *source)

{
  UiSelectableGroup_SelectExclusive(3,(UiNodeBase *)source,
      THANDOR_UI_SIBLING(source,InGameUiImage,unitPlacementOption0,unitPlacementOption1),
      THANDOR_UI_SIBLING(source,InGameUiImage,unitPlacementOption0,unitPlacementOption2),
      THANDOR_UI_SIBLING(source,InGameUiImage,unitPlacementOption0,unitPlacementOption0));
  g_UiCommandModeA = 0;
  return;
}


/* Address: 0x0056E090.
   Unit placement option 1 (action 0x1112, g_InGameUiCommandModeActionHandlers30[18]; also the editor hotkeys in
   ui/ingame/runtime.c): selects unitPlacementOption1 among the three unit placement buttons and sets
   g_UiCommandModeA = 1.
*/
void __thandor_void_preserve_eax_ecx_edx InGameCommandModeA_Select1(UiSpriteButtonControl *source)

{
  UiSelectableGroup_SelectExclusive(3,(UiNodeBase *)source,
      THANDOR_UI_SIBLING(source,InGameUiImage,unitPlacementOption1,unitPlacementOption1),
      THANDOR_UI_SIBLING(source,InGameUiImage,unitPlacementOption1,unitPlacementOption2),
      THANDOR_UI_SIBLING(source,InGameUiImage,unitPlacementOption1,unitPlacementOption0));
  g_UiCommandModeA = 1;
  return;
}


/* Address: 0x0056E0D0.
   Unit placement option 2 (action 0x1113, g_InGameUiCommandModeActionHandlers30[19]; also the editor hotkeys in
   ui/ingame/runtime.c): selects unitPlacementOption2 among the three unit placement buttons and sets
   g_UiCommandModeA = 2.
*/
void __thandor_void_preserve_eax_ecx_edx InGameCommandModeA_Select2(UiSpriteButtonControl *source)

{
  UiSelectableGroup_SelectExclusive(3,(UiNodeBase *)source,
      THANDOR_UI_SIBLING(source,InGameUiImage,unitPlacementOption2,unitPlacementOption1),
      THANDOR_UI_SIBLING(source,InGameUiImage,unitPlacementOption2,unitPlacementOption2),
      THANDOR_UI_SIBLING(source,InGameUiImage,unitPlacementOption2,unitPlacementOption0));
  g_UiCommandModeA = 2;
  return;
}


/* Address: 0x0056E110.
   Object placement option 0 (action 0x1114, g_InGameUiCommandModeActionHandlers30[20]; also the editor hotkeys
   in ui/ingame/runtime.c): selects objectPlacementOption0 among the three object placement buttons and sets
   g_UiCommandModeB = 0.
*/
void __thandor_void_preserve_eax_ecx_edx InGameCommandModeB_Select0(UiSpriteButtonControl *source)

{
  UiSelectableGroup_SelectExclusive(3,(UiNodeBase *)source,
      THANDOR_UI_SIBLING(source,InGameUiImage,objectPlacementOption0,objectPlacementOption1),
      THANDOR_UI_SIBLING(source,InGameUiImage,objectPlacementOption0,objectPlacementOption2),
      THANDOR_UI_SIBLING(source,InGameUiImage,objectPlacementOption0,objectPlacementOption0));
  g_UiCommandModeB = 0;
  return;
}


/* Address: 0x0056E150.
   Object placement option 1 (action 0x1115, g_InGameUiCommandModeActionHandlers30[21]; also the editor hotkeys
   in ui/ingame/runtime.c): selects objectPlacementOption1 among the three object placement buttons and sets
   g_UiCommandModeB = 1.
*/
void __thandor_void_preserve_eax_ecx_edx InGameCommandModeB_Select1(UiSpriteButtonControl *source)

{
  UiSelectableGroup_SelectExclusive(3,(UiNodeBase *)source,
      THANDOR_UI_SIBLING(source,InGameUiImage,objectPlacementOption1,objectPlacementOption1),
      THANDOR_UI_SIBLING(source,InGameUiImage,objectPlacementOption1,objectPlacementOption2),
      THANDOR_UI_SIBLING(source,InGameUiImage,objectPlacementOption1,objectPlacementOption0));
  g_UiCommandModeB = 1;
  return;
}


/* Address: 0x0056E190.
   Object placement option 2 (action 0x1116, g_InGameUiCommandModeActionHandlers30[22]; also the editor hotkeys
   in ui/ingame/runtime.c): selects objectPlacementOption2 among the three object placement buttons and sets
   g_UiCommandModeB = 2.
*/
void __thandor_void_preserve_eax_ecx_edx InGameCommandModeB_Select2(UiSpriteButtonControl *source)

{
  UiSelectableGroup_SelectExclusive(3,(UiNodeBase *)source,
      THANDOR_UI_SIBLING(source,InGameUiImage,objectPlacementOption2,objectPlacementOption1),
      THANDOR_UI_SIBLING(source,InGameUiImage,objectPlacementOption2,objectPlacementOption2),
      THANDOR_UI_SIBLING(source,InGameUiImage,objectPlacementOption2,objectPlacementOption0));
  g_UiCommandModeB = 2;
  return;
}


/* Address: 0x0056E1D0.
   Smoothing tool option 0 (action 0x1117, g_InGameUiCommandModeActionHandlers30[23]; also the editor hotkeys in
   ui/ingame/runtime.c): selects smoothingToolOption0 and sets g_UiCommandModeE = 0. Unlike options 1 and 2 its
   exclusive group also contains smoothingRelaxLandButton.
*/
void __thandor_void_preserve_eax_ecx_edx InGameCommandModeE_Select0(UiSpriteButtonControl *source)

{
  UiSelectableGroup_SelectExclusive(4,(UiNodeBase *)source,
      THANDOR_UI_SIBLING(source,InGameUiImage,smoothingToolOption0,smoothingRelaxLandButton),
      THANDOR_UI_SIBLING(source,InGameUiImage,smoothingToolOption0,smoothingToolOption2),
      THANDOR_UI_SIBLING(source,InGameUiImage,smoothingToolOption0,smoothingToolOption1),
      THANDOR_UI_SIBLING(source,InGameUiImage,smoothingToolOption0,smoothingToolOption0));
  g_UiCommandModeE = 0;
  return;
}


/* Address: 0x0056E220.
   Smoothing tool option 1 (action 0x1118, g_InGameUiCommandModeActionHandlers30[24]; also the editor hotkeys in
   ui/ingame/runtime.c): selects smoothingToolOption1 among the three smoothing tool buttons and sets
   g_UiCommandModeE = 1.
*/
void __thandor_void_preserve_eax_ecx_edx InGameCommandModeE_Select1(UiSpriteButtonControl *source)

{
  UiSelectableGroup_SelectExclusive(3,(UiNodeBase *)source,
      THANDOR_UI_SIBLING(source,InGameUiImage,smoothingToolOption1,smoothingToolOption2),
      THANDOR_UI_SIBLING(source,InGameUiImage,smoothingToolOption1,smoothingToolOption1),
      THANDOR_UI_SIBLING(source,InGameUiImage,smoothingToolOption1,smoothingToolOption0));
  g_UiCommandModeE = 1;
  return;
}


/* Address: 0x0056E260.
   Smoothing tool option 2 (action 0x1119, g_InGameUiCommandModeActionHandlers30[25]; also the editor hotkeys in
   ui/ingame/runtime.c): selects smoothingToolOption2 among the three smoothing tool buttons and sets
   g_UiCommandModeE = 2.
*/
void __thandor_void_preserve_eax_ecx_edx InGameCommandModeE_Select2(UiSpriteButtonControl *source)

{
  UiSelectableGroup_SelectExclusive(3,(UiNodeBase *)source,
      THANDOR_UI_SIBLING(source,InGameUiImage,smoothingToolOption2,smoothingToolOption2),
      THANDOR_UI_SIBLING(source,InGameUiImage,smoothingToolOption2,smoothingToolOption1),
      THANDOR_UI_SIBLING(source,InGameUiImage,smoothingToolOption2,smoothingToolOption0));
  g_UiCommandModeE = 2;
  return;
}


/* Address: 0x0056E2A0.
   Smoothing page button smoothingRelaxGatedButton (action 0x111A, g_InGameUiCommandModeActionHandlers30[26]; also
   an editor hotkey in ui/ingame/runtime.c): runs 128 sign-gated terrain relaxation passes over the field, in a
   network game through command 0x3200 on every machine.
*/
void __thandor_void_preserve_eax_ecx_edx InGameCommandRange_DispatchState0(UiNodeBase *source)

{
  if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_NETWORKED_MASK) ==
      SESSION_NETWORK_ROLE_LOCAL) {
    TerrainGrid_RunDirectionalRelaxationPasses
              (g_LocalPlayerRuntimeId,0,128,TERRAIN_RELAXATION_SIGN_GATED);
  }
  else {
    InGameCommandQueue_AppendLocalPlayerCommand
              (INGAME_COMMAND_TERRAIN_RELAXATION,0,128,TERRAIN_RELAXATION_SIGN_GATED);
  }
  return;
}


/* Address: 0x0056E2E0.
   Smoothing page button smoothingRelaxLandButton (action 0x111B, g_InGameUiCommandModeActionHandlers30[27]; also
   an editor hotkey in ui/ingame/runtime.c): like InGameCommandRange_DispatchState0 with the ungated land tool
   relaxation mode.
*/
void __thandor_void_preserve_eax_ecx_edx InGameCommandRange_DispatchState1(UiNodeBase *source)

{
  if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_NETWORKED_MASK) ==
      SESSION_NETWORK_ROLE_LOCAL) {
    TerrainGrid_RunDirectionalRelaxationPasses
              (g_LocalPlayerRuntimeId,0,128,TERRAIN_RELAXATION_UNGATED_LAND_TOOL);
  }
  else {
    InGameCommandQueue_AppendLocalPlayerCommand
              (INGAME_COMMAND_TERRAIN_RELAXATION,0,128,TERRAIN_RELAXATION_UNGATED_LAND_TOOL);
  }
  return;
}


/* Address: 0x0056E320.
   Region tool option 0 (action 0x111C, g_InGameUiCommandModeActionHandlers30[28]): selects regionToolOption0 of the
   two region tool buttons, sets g_UiCommandModeF = 0 and copies it into the world runtime (+0xB4), where the
   region markers of the world view read it.
*/
void __thandor_void_preserve_eax_ecx_edx InGameCommandModeF_Select0(UiSpriteButtonControl *source)

{
  UiSelectableGroup_SelectExclusive(2,(UiNodeBase *)source,
      THANDOR_UI_SIBLING(source,InGameUiImage,regionToolOption0,regionToolOption1),
      THANDOR_UI_SIBLING(source,InGameUiImage,regionToolOption0,regionToolOption0));
  g_UiCommandModeF = 0;
  ((WorldRuntimeContext *)THANDOR_UI_SIBLING(source,InGameUiImage,regionToolOption0,worldView))
       ->fieldRegion.reservedCallbackState04 = 0;
  return;
}


/* Address: 0x0056E370.
   Region tool option 1 (action 0x111D, g_InGameUiCommandModeActionHandlers30[29]): selects regionToolOption1 and
   sets g_UiCommandModeF and its world runtime copy (+0xB4) to 1.
*/
void __thandor_void_preserve_eax_ecx_edx InGameCommandModeF_Select1(UiSpriteButtonControl *source)

{
  UiSelectableGroup_SelectExclusive(2,(UiNodeBase *)source,
      THANDOR_UI_SIBLING(source,InGameUiImage,regionToolOption1,regionToolOption1),
      THANDOR_UI_SIBLING(source,InGameUiImage,regionToolOption1,regionToolOption0));
  g_UiCommandModeF = 1;
  ((WorldRuntimeContext *)THANDOR_UI_SIBLING(source,InGameUiImage,regionToolOption1,worldView))
       ->fieldRegion.reservedCallbackState04 = 1;
  return;
}


/* Address: 0x00570F20.
   Empty callback: InGameUiCommandRuntime_ApplyInteractionSubsystemActiveState installs it as
   fieldRegion.clearTransientStateCallback of the world runtime while the editor is active.
*/
void UiCommandRuntime_CallbackNoOp(void)

{
  return;
}

/* Address: 0x0055F280.
   In-game command handler 0x150 (quit game window and player departure): CLOSE_SESSION ends the session,
   SURRENDER destroys every army of the player's faction. Without flags the player has left: another player's
   departure is announced in the message history (text 0xFF08); the local player's own departure marks the
   session as left and, in a network game, shuts the network backend down and falls back to a one-player setup.
*/
void __thandor_void_preserve_eax_ecx
InGameCommand150_HandlePlayerDepartureAndOwnership
          (PlayerOrFactionRuntimeId32 playerOrFactionId,uint32_t value1,uint32_t value2,
          GameEntityCommandFlags flags)

{
  WorldRuntimeContext *worldRuntime;
  uint32_t factionToken;
  GameEntityRuntime *entityRuntime;
  InGameRuntimeRootImageC3E4 *runtimeRoot;
  FrontendPlayerRuntimeBlockCount remainingPlayers;
  FrontendPlayerRuntimeRecord *playerRecord;
  TextResolveResult departureText;
  WorldOwnerListNode100 *ownerNode;
  
  runtimeRoot = g_InGameRuntimeRoot;
  if ((flags & INGAME_COMMAND150_FLAG_CLOSE_SESSION) == 0) {
    worldRuntime = &g_InGameRuntimeRoot->worldRuntime0A30;
    remainingPlayers = g_FrontendPlayerRuntimeBlockCount;
    playerRecord = g_FrontendPlayerRuntimeBlocks;
    if ((flags & INGAME_COMMAND150_FLAG_SURRENDER) == 0) {
      do {
        if (playerOrFactionId == playerRecord->playerRuntimeId) {
          playerRecord->heartbeatExpiryTicks = 0;
          if (playerRecord == g_FrontendPlayerRuntimeBlocks) {
            g_SessionTransferTimeoutTicks = 0;
          }
          if (playerOrFactionId == (runtimeRoot->worldRuntime0A30).selection.activePlayerRuntimeId) {
            g_UiCommandRuntimeFlags = g_UiCommandRuntimeFlags | UI_COMMAND_RUNTIME_FLAG_LOCAL_PLAYER_LEFT;
            Resource_Release(g_FrontendLoadedCampaignAsset);
            g_FrontendLoadedCampaignAsset = NULL;
            if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_NETWORKED_MASK) ==
                SESSION_NETWORK_ROLE_LOCAL) {
              g_FrontendLoadedCampaignAsset = NULL; /* stored twice, as in the original */
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
          /* departure message: the player name is patched into text 0xFF08 */
          departureText = TextResource_Resolve(0xff08);
          RichTextCommandStream_PatchPayloadBySelector(0,&playerRecord->playerName,departureText.text);
          InGameRecentTextHistory_InsertAndRebuild8(departureText.text);
          return;
        }
        remainingPlayers--;
        playerRecord++;
      } while (remainingPlayers != 0);
    }
    else {
      factionToken = g_SelectionPlayerRuntimeBlockPointers[playerOrFactionId]->
              primaryEntityOrFactionToken8080;
      for (ownerNode = (g_InGameRuntimeRoot->worldRuntime0A30).ownerListHead;
          ownerNode != NULL; ownerNode = ownerNode->nextNode) {
        if ((ownerNode->ownerClassId == WORLD_OWNER_RUNTIME_MODEL) &&
           (entityRuntime = *(GameEntityRuntime **)((int)ownerNode->runtimePayload + 8),
           factionToken == (entityRuntime->common).ownership.ownerIndex)) {
          ArmyRuntime_DestroyInstanceAndRefreshUi(worldRuntime,entityRuntime);
        }
      }
    }
  }
  else {
    g_UiCommandRuntimeFlags = g_UiCommandRuntimeFlags | UI_COMMAND_RUNTIME_FLAG_SESSION_CLOSED;
  }
  return;
}


/* Address: 0x0056D980.
   Terrain colours of the smoothing tool (InGameCommandModeG_Select2): rebuilds the terrain lighting colour ramp
   from the world runtime's lighting colours (+0x120 ramp, +0x124 base) with their alpha removed and the secondary
   colour (+0x12C) made opaque, relights the field grid with the light angles at +0x178/+0x17C, then sets bit 0x1000
   of g_UiCommandModeGColorVariantFlags and the limit 0x7FFFFFFF read by the terrain triangle and marker drawing.
*/
void __thandor_void_preserve_eax_ecx_edx UiCommandModeG_ApplyMaskedColorVariant(void *worldRuntime)

{
  TerrainLighting_BuildColorRampAndSetBaseColor
            (*(uint32_t *)((int)worldRuntime + 0x12c) | 0xff000000,
             *(uint32_t *)((int)worldRuntime + 0x124) & 0xffffff,
             *(uint32_t *)((int)worldRuntime + 0x120) & 0xffffff);
  FieldGrid_RecomputeInteriorDirectionalLighting
            (*(AngleTurn32 *)((int)worldRuntime + 0x17c),*(AngleTurn32 *)((int)worldRuntime + 0x178),
             *(FieldGridAsset **)((int)worldRuntime + 0x54));
  g_UiCommandModeGColorVariantFlags = g_UiCommandModeGColorVariantFlags | 0x1000;
  g_UiCommandModeGColorVariantLimit = 0x7fffffff;
  return;
}


/* Address: 0x0056DA50.
   Shows the region markers of the world view (sets WORLD_RUNTIME_FLAG_DRAW_REGION_MARKERS); only the region tool
   (InGameCommandModeG_Select5) uses it.
*/
void __thandor_void_preserve_eax_ecx_edx UiCommandModeG_SetNodeFlag02000000(WorldRuntimeContext *context)

{
  context->runtimeFlags = context->runtimeFlags | WORLD_RUNTIME_FLAG_DRAW_REGION_MARKERS;
  return;
}


/* Address: 0x00571440.
   Selects terrain material absoluteIndex for the material tool (InGameCommandMatrix_SelectMappedControl and the
   editor hotkeys/initialisation in ui/ingame/runtime.c): shows its texture in materialToolSelectedSwatch,
   scrolls the twelve-swatch page in rows of three until the material is visible, fills the twelve swatches from
   g_TerrainMaterialTextureSets (empty entries show nothing) and selects the material's swatch.
*/
void __thandor_void_preserve_eax_ecx_edx
UiCommandMatrix_SelectIndex(UiCommandModeIndex absoluteIndex,UiNodeBase *root)

{
  GraphicsTextureSourceAsset *firstTexture;
  GraphicsTextureSourceAsset *secondTexture;
  int controlIndex;
  uint32_t pageEnd;
  uint32_t pageBase;
  
  g_UiCommandAbsoluteSelectionIndex = absoluteIndex;
  ((UiImagePanelControl *)INGAME_UI(root,materialToolSelectedSwatch))->textureSource =
       g_TerrainMaterialTextureSets[absoluteIndex]->entries[0].sourceAsset;
  pageEnd = g_UiCommandSelectionPageBaseIndex + 12;
  pageBase = g_UiCommandSelectionPageBaseIndex;
  /* move the page by rows of three swatches until absoluteIndex lies in [pageBase, pageEnd) */
  while( true ) {
    for (; absoluteIndex < pageBase; pageBase = pageBase - 3) {
      pageEnd = pageEnd - 3;
    }
    if (absoluteIndex < pageEnd) break;
    pageBase = pageBase + 3;
    pageEnd = pageEnd + 3;
  }
  firstTexture = NULL;
  if (g_TerrainMaterialTextureSets[pageBase] != NULL) {
    firstTexture = g_TerrainMaterialTextureSets[pageBase]->entries[0].sourceAsset;
  }
  secondTexture = NULL;
  if (g_TerrainMaterialTextureSets[pageBase + 1] != NULL) {
    secondTexture = g_TerrainMaterialTextureSets[pageBase + 1]->entries[0].sourceAsset;
  }
  g_UiCommandSelectionPageBaseIndex = pageBase;
  ((UiImagePanelControl *)INGAME_UI(root,materialSwatch00))->textureSource = firstTexture;
  ((UiImagePanelControl *)INGAME_UI(root,materialSwatch01))->textureSource = secondTexture;
  firstTexture = NULL;
  if (g_TerrainMaterialTextureSets[pageBase + 2] != NULL) {
    firstTexture = g_TerrainMaterialTextureSets[pageBase + 2]->entries[0].sourceAsset;
  }
  secondTexture = NULL;
  if (g_TerrainMaterialTextureSets[pageBase + 3] != NULL) {
    secondTexture = g_TerrainMaterialTextureSets[pageBase + 3]->entries[0].sourceAsset;
  }
  ((UiImagePanelControl *)INGAME_UI(root,materialSwatch02))->textureSource = firstTexture;
  ((UiImagePanelControl *)INGAME_UI(root,materialSwatch03))->textureSource = secondTexture;
  firstTexture = NULL;
  if (g_TerrainMaterialTextureSets[pageBase + 4] != NULL) {
    firstTexture = g_TerrainMaterialTextureSets[pageBase + 4]->entries[0].sourceAsset;
  }
  secondTexture = NULL;
  if (g_TerrainMaterialTextureSets[pageBase + 5] != NULL) {
    secondTexture = g_TerrainMaterialTextureSets[pageBase + 5]->entries[0].sourceAsset;
  }
  ((UiImagePanelControl *)INGAME_UI(root,materialSwatch04))->textureSource = firstTexture;
  ((UiImagePanelControl *)INGAME_UI(root,materialSwatch05))->textureSource = secondTexture;
  firstTexture = NULL;
  if (g_TerrainMaterialTextureSets[pageBase + 6] != NULL) {
    firstTexture = g_TerrainMaterialTextureSets[pageBase + 6]->entries[0].sourceAsset;
  }
  secondTexture = NULL;
  if (g_TerrainMaterialTextureSets[pageBase + 7] != NULL) {
    secondTexture = g_TerrainMaterialTextureSets[pageBase + 7]->entries[0].sourceAsset;
  }
  ((UiImagePanelControl *)INGAME_UI(root,materialSwatch06))->textureSource = firstTexture;
  ((UiImagePanelControl *)INGAME_UI(root,materialSwatch07))->textureSource = secondTexture;
  firstTexture = NULL;
  if (g_TerrainMaterialTextureSets[pageBase + 8] != NULL) {
    firstTexture = g_TerrainMaterialTextureSets[pageBase + 8]->entries[0].sourceAsset;
  }
  secondTexture = NULL;
  if (g_TerrainMaterialTextureSets[pageBase + 9] != NULL) {
    secondTexture = g_TerrainMaterialTextureSets[pageBase + 9]->entries[0].sourceAsset;
  }
  ((UiImagePanelControl *)INGAME_UI(root,materialSwatch08))->textureSource = firstTexture;
  ((UiImagePanelControl *)INGAME_UI(root,materialSwatch09))->textureSource = secondTexture;
  firstTexture = NULL;
  if (g_TerrainMaterialTextureSets[pageBase + 10] != NULL) {
    firstTexture = g_TerrainMaterialTextureSets[pageBase + 10]->entries[0].sourceAsset;
  }
  secondTexture = NULL;
  if (g_TerrainMaterialTextureSets[pageBase + 11] != NULL) {
    secondTexture = g_TerrainMaterialTextureSets[pageBase + 11]->entries[0].sourceAsset;
  }
  ((UiImagePanelControl *)INGAME_UI(root,materialSwatch10))->textureSource = firstTexture;
  ((UiImagePanelControl *)INGAME_UI(root,materialSwatch11))->textureSource = secondTexture;
  /* empty remainder of the original loop that pushes the twelve swatch controls */
  controlIndex = 11;
  do {
    controlIndex = controlIndex + -1;
  } while (-1 < controlIndex);
  /* The original pushes all twelve command controls (offsets 11..0) as the variadic list. */
  UiSelectableGroup_SelectExclusive
            (12,(UiNodeBase *)
                 ((int)&root->nextSibling + g_UiMappedCommandControlOffsets[absoluteIndex - pageBase]),
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


/* Address: 0x0055F440.
   Changes the global g_UiCommandRuntimeFlags: first clears clearMask, then sets setMask, then toggles toggleMask
   (the masks come in the reverse order as arguments). Local games call it directly, network games send the
   same masks as player command 0x310. playerRuntimeId is not used: the flags are not per player.
*/
void __thandor_void_preserve_eax_ecx_edx
UiCommandRuntimeFlags_ApplyClearSetToggleMasks
          (PlayerRuntimeId playerRuntimeId,UiCommandRuntimeFlagMask toggleMask,
          UiCommandRuntimeFlagMask setMask,UiCommandRuntimeFlagMask clearMask)

{
  g_UiCommandRuntimeFlags = g_UiCommandRuntimeFlags & ~clearMask;
  g_UiCommandRuntimeFlags = g_UiCommandRuntimeFlags | setMask;
  g_UiCommandRuntimeFlags = g_UiCommandRuntimeFlags ^ toggleMask;
}


/* Address: 0x0056D860.
   Hides the surface point marker of the world view (clears WORLD_RUNTIME_FLAG_DRAW_SURFACE_POINT_MARKER); editor
   mode tabs G3/G4 and InGameUiCommandRuntime_ApplyInteractionSubsystemActiveState.
*/
void __thandor_void_preserve_eax_ecx_edx UiCommandModeG_ClearNodeFlag00100000(WorldRuntimeContext *context)

{
  context->runtimeFlags = context->runtimeFlags & ~WORLD_RUNTIME_FLAG_DRAW_SURFACE_POINT_MARKER;
  return;
}


/* Address: 0x0056D880.
   Shows the terrain point markers of the world view (sets WORLD_RUNTIME_FLAG_DRAW_TERRAIN_POINT_MARKERS); editor
   mode tabs G0/G1 (height and material tools).
*/
void __thandor_void_preserve_eax_ecx_edx UiCommandModeG_SetNodeFlag00200000(WorldRuntimeContext *context)

{
  context->runtimeFlags = context->runtimeFlags | WORLD_RUNTIME_FLAG_DRAW_TERRAIN_POINT_MARKERS;
  return;
}


/* Address: 0x0056D940.
   Sets WORLD_RUNTIME_FLAG_SECONDARY_SURFACE_ONLY (view ray and markers use only the secondary field surface);
   editor mode tabs G0 and G2.
*/
void __thandor_void_preserve_eax_ecx_edx UiCommandModeG_SetNodeFlag01000000(WorldRuntimeContext *context)

{
  context->runtimeFlags = context->runtimeFlags | WORLD_RUNTIME_FLAG_SECONDARY_SURFACE_ONLY;
  return;
}


/* Address: 0x0056D8C0.
   Shows the army metrics overlay of the world view (sets WORLD_RUNTIME_FLAG_DRAW_ARMY_METRICS); editor mode tabs
   G3-G5 and InGameUiCommandRuntime_ApplyInteractionSubsystemActiveState when the editor is switched off.
*/
void __thandor_void_preserve_eax_ecx_edx UiCommandModeG_SetNodeFlag00000400(WorldRuntimeContext *context)

{
  context->runtimeFlags = context->runtimeFlags | WORLD_RUNTIME_FLAG_DRAW_ARMY_METRICS;
  return;
}


/* Address: 0x0056D8E0.
   Hides the army metrics overlay and ends a drag selection (clears WORLD_RUNTIME_FLAG_DRAW_ARMY_METRICS and
   WORLD_RUNTIME_FLAG_DRAG_SELECTING, which also draws the selection frame); editor mode tabs G0-G2.
*/
void __thandor_void_preserve_eax_ecx_edx UiCommandModeG_ClearNodeFlags00000480(WorldRuntimeContext *context)

{
  context->runtimeFlags =
       context->runtimeFlags & ~(WORLD_RUNTIME_FLAG_DRAW_ARMY_METRICS | WORLD_RUNTIME_FLAG_DRAG_SELECTING);
  return;
}


/* Address: 0x0056D840.
   Shows the surface point marker of the world view (sets WORLD_RUNTIME_FLAG_DRAW_SURFACE_POINT_MARKER); editor
   mode tabs G0, G1, G2 and G5.
*/
void __thandor_void_preserve_eax_ecx_edx UiCommandModeG_SetNodeFlag00100000(WorldRuntimeContext *context)

{
  context->runtimeFlags = context->runtimeFlags | WORLD_RUNTIME_FLAG_DRAW_SURFACE_POINT_MARKER;
  return;
}


/* Address: 0x0056D8A0.
   Hides the terrain point markers of the world view (clears WORLD_RUNTIME_FLAG_DRAW_TERRAIN_POINT_MARKERS); editor
   mode tabs G2-G5 and InGameUiCommandRuntime_ApplyInteractionSubsystemActiveState.
*/
void __thandor_void_preserve_eax_ecx_edx UiCommandModeG_ClearNodeFlag00200000(WorldRuntimeContext *context)

{
  context->runtimeFlags = context->runtimeFlags & ~WORLD_RUNTIME_FLAG_DRAW_TERRAIN_POINT_MARKERS;
  return;
}


/* Address: 0x0056D960.
   Clears WORLD_RUNTIME_FLAG_SECONDARY_SURFACE_ONLY; editor mode tabs G1, G3-G5 and
   InGameUiCommandRuntime_ApplyInteractionSubsystemActiveState.
*/
void __thandor_void_preserve_eax_ecx_edx UiCommandModeG_ClearNodeFlag01000000(WorldRuntimeContext *context)

{
  context->runtimeFlags = context->runtimeFlags & ~WORLD_RUNTIME_FLAG_SECONDARY_SURFACE_ONLY;
  return;
}


/* Address: 0x0056D9F0.
   Terrain colours of every editor mode except smoothing (and of InGameUiCommandRuntime_ApplyInteractionSubsystemActiveState
   when the editor is switched off): rebuilds the terrain colour ramp from the world runtime's lighting colours
   (+0x120 ramp, +0x124 base, +0x12C secondary) unchanged, relights the field grid with the light angles at
   +0x178/+0x17C, clears bit 0x1000 of g_UiCommandModeGColorVariantFlags and sets the limit to 0x00FFFFFF. The
   counterpart of UiCommandModeG_ApplyMaskedColorVariant.
*/
void __thandor_void_preserve_eax_ecx_edx UiCommandModeG_ApplyRawColorVariant(void *worldRuntime)

{
  TerrainLighting_BuildColorRampAndSetBaseColor
            (*(PackedArgb32 *)((int)worldRuntime + 0x12c),*(PackedArgb32 *)((int)worldRuntime + 0x124),
             *(PackedArgb32 *)((int)worldRuntime + 0x120));
  FieldGrid_RecomputeInteriorDirectionalLighting
            (*(AngleTurn32 *)((int)worldRuntime + 0x17c),*(AngleTurn32 *)((int)worldRuntime + 0x178),
             *(FieldGridAsset **)((int)worldRuntime + 0x54));
  g_UiCommandModeGColorVariantFlags = g_UiCommandModeGColorVariantFlags & 0xffffefff;
  g_UiCommandModeGColorVariantLimit = 0xffffff;
  return;
}


/* Address: 0x0056DA70.
   Hides the region markers of the world view (clears WORLD_RUNTIME_FLAG_DRAW_REGION_MARKERS); every editor mode tab
   except G5 and InGameUiCommandRuntime_ApplyInteractionSubsystemActiveState.
*/
void __thandor_void_preserve_eax_ecx_edx UiCommandModeG_ClearNodeFlag02000000(WorldRuntimeContext *context)

{
  context->runtimeFlags = context->runtimeFlags & ~WORLD_RUNTIME_FLAG_DRAW_REGION_MARKERS;
  return;
}


/* Address: 0x0056D900.
   Shows the grid vertex markers of the world view (sets WORLD_RUNTIME_FLAG_DRAW_GRID_VERTEX_MARKERS); every
   editor mode tab.
*/
void __thandor_void_preserve_eax_ecx_edx UiCommandModeG_SetNodeFlag00800000(WorldRuntimeContext *context)

{
  context->runtimeFlags = context->runtimeFlags | WORLD_RUNTIME_FLAG_DRAW_GRID_VERTEX_MARKERS;
  return;
}


/* Address: 0x0056DA90.
   Common part of the editor mode tabs InGameCommandModeG_Select0..5: selects the clicked tab among the six, shows
   the mode's pages in modePreviewPageStack, modeDetailPageStack and modeCommandPageStack (page tables
   g_UiCommandModeG*PageIndices) and stores the mode in g_UiCommandModeG. Returns the in-game root.
*/
InGameRuntimeRootImageC3E4 * __thandor_eax_edx_cf_preserve_ecx
UiCommandModeG_SelectAndSyncPages(UiCommandModeIndex modeIndex,UiSelectableControl *source)

{
  UiNodeBase *parentCursor;
  InGameRuntimeRootImageC3E4 *root;

  parentCursor = (source->base).parent;
  root = (InGameRuntimeRootImageC3E4 *)source;
  while (parentCursor != UI_NODE_NONE) {
    root = (InGameRuntimeRootImageC3E4 *)(root->rootUi0000).base.parent;
    parentCursor = (root->rootUi0000).base.parent;
  }
  UiSelectableGroup_NoneVisibleSelected(6,
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


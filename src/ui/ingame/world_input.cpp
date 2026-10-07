/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/src/ui/ingame/world_input.cpp
 * Reverse engineering by idkFoxes 2026
 */

#include <thandor/ui/ingame/world_input.h>
#include <thandor/thandor.h>
#include <thandor/platform/bootstrap/image.h>

/* Module data. */

static uint32_t g_InGamePlacementPointerCaptureX = 0;

static uint32_t g_InGamePlacementPointerCaptureY = 0;

static uint32_t g_InGameCommandPointerCaptureX = 0;

static uint32_t g_InGameCommandPointerCaptureY = 0;

/* uint32_t[8]: command id per pointer mode (modifier mask & variant mask) */
static const uint32_t g_InGamePointerModeCommandIds[8] = {26, 38, 39, 40, 41, 42, 43, 44};

InGameCommandPayloadTripletValue32 g_InGameSelectionInsertTripletDwords[12] = {};

InGameCommandPayloadTripletValue32 g_InGameSelectionRemoveTripletDwords[12] = {};

uint32_t g_InGamePlacementHeading16 = 0;

uint32_t g_InGamePlacementWorldYQ12 = 0;

uint32_t g_InGamePlacementWorldXQ12 = 0;

uint32_t g_InGameCommandPreviewHeading16 = 0;

uint32_t g_InGameCommandPreviewWorldYQ12 = 0;

uint32_t g_InGameCommandPreviewWorldXQ12 = 0;

uint32_t g_InGameCommandPreviewSurfaceHeightQ12OrSentinel = 0;

uint32_t g_InGamePointerInteractionStateFlags = 0;

int32_t g_InGamePlacementSurfaceHeightQ12OrSentinel = 0;

static GraphicsFixedVec3 g_GraphicsProjectionScratchVec3 = {};

/* Hover cursor of InGameWorldInput_ResolveContextActionAndCursor when the selection has an entry with
   nonnegative weapon damage (an attack is possible); entry is NULL without a candidate army. */
static uint32_t InGameWorldInput_ResolveWeaponTargetCursor(GameEntityRuntime *entry,int ownerIndex)

{
  if (SelectionInfo_TestNoEntryHasWeaponDamage()) {
    return GRAPHICS_CURSOR_FRAME_ARROW;
  }
  if (entry == nullptr) {
    return WORLD_CURSOR_TARGET;
  }
  if (GameFactionRuntime_TestCapabilityBitClear((entry->common).ownership.ownerIndex,ownerIndex)) {
    return WORLD_CURSOR_TARGET;
  }
  if (SelectionInfo_IsEntryAbsent(entry)) {
    return WORLD_CURSOR_TARGET_REJECTED;
  }
  return GRAPHICS_CURSOR_FRAME_ARROW;
}

/* Hover cursor of InGameWorldInput_ResolveContextActionAndCursor when no attack is possible: decided by the
   candidate army's faction capability bit and hierarchy condition ratio; entry is NULL without a candidate. */
static uint32_t InGameWorldInput_ResolveCandidateConditionCursor(GameEntityRuntime *entry,int ownerIndex)

{
  Q12 conditionRatioQ12;

  if (entry == nullptr) {
    return GRAPHICS_CURSOR_FRAME_ARROW;
  }
  if (GameFactionRuntime_TestCapabilityBitClear((entry->common).ownership.ownerIndex,ownerIndex)) {
    return WORLD_CURSOR_TARGET_REJECTED;
  }
  conditionRatioQ12 = ModelRuntime_QueryHierarchyConditionRatioQ12(ModelView_Cast<RuntimeModelFactionPrefix>(entry));
  if (conditionRatioQ12 != Q12_ONE) {
    if (SelectionInfo_IsEntryAbsent(entry)) {
      return WORLD_CURSOR_TARGET;
    }
    return GRAPHICS_CURSOR_FRAME_ARROW;
  }
  return (ownerIndex != (entry->common).ownership.ownerIndex) ? WORLD_CURSOR_FOREIGN_ARMY : WORLD_CURSOR_OWN_ARMY;
}

/* Hover callback of the world view: picks the cursor frame for the pointer position (placement valid/blocked,
   command-mode preview, own/foreign army, move or target) and records the hovered army as the selected entity,
   so the cursor always shows what a click at this point would do.
*/
uint32_t InGameWorldInput_ResolveContextActionAndCursor
                (InGamePointerCallbackValue0 pickedHeightQ12,InGamePointerCallbackValue1 pointerWorldXQ12
                ,InGamePointerCallbackValue2 pointerWorldYQ12,InGamePointerCallbackValue3 candidateHeightQ12
                ,WorldOwnerListNode *candidateNode,WorldRuntimeContext *inGameRuntime)

{
  uint32_t placementCursor;
  uint32_t variantMask;
  int ownerIndex;
  uint32_t modifierModeMask;
  GameEntityRuntime *entry;
  Bool8 testResult;

  g_InGameCommandPreviewSurfaceHeightQ12OrSentinel = WORLD_POINTER_NO_HIT;
  (inGameRuntime->selection).selectedEntity = nullptr;
  if (((inGameRuntime->interaction).nodeFlags & 8) != 0) {
    return GRAPHICS_CURSOR_FRAME_ARROW;
  }
  if ((g_UiCommandRuntimeFlags & UI_COMMAND_RUNTIME_FLAG_WORLD_INPUT_DISABLED) != 0) {
    return GRAPHICS_CURSOR_FRAME_ARROW;
  }
  ownerIndex = inGameRuntime->activeFactionRuntimeIndex;
  if ((inGameRuntime->runtimeFlags & WORLD_RUNTIME_FLAG_NOTIFICATION_GOTO) != 0) {
    return GRAPHICS_CURSOR_FRAME_BUSY;
  }
  if ((g_UiCommandRuntimeFlags & UI_COMMAND_RUNTIME_FLAG_PLACEMENT_PENDING) != 0) {
    g_InGamePlacementSurfaceHeightQ12OrSentinel = pickedHeightQ12;
    g_InGamePlacementWorldYQ12 = pointerWorldYQ12;
    g_InGamePlacementWorldXQ12 = pointerWorldXQ12;
    if (pickedHeightQ12 == WORLD_POINTER_NO_HIT) {
      return WORLD_CURSOR_NO_TARGET;
    }
    g_UiHoverSelectionRecord =
         (UiCommandRuntimeRecordPrefix *)
         g_SelectionPlayerRuntimeBlockPointers[(inGameRuntime->selection).activePlayerRuntimeId]->
         pendingPlacementArmyAsset;
    placementCursor = WORLD_CURSOR_PLACEMENT_VALID;
    testResult = ArmyPlacement_ValidateAssetAtPointAndCellCorners
                      (0,g_InGamePlacementHeading16,pointerWorldXQ12,pointerWorldYQ12,
                       g_UiHoverSelectionRecord->armyAssetId,
                       inGameRuntime->activeFactionRuntimeIndex,inGameRuntime);
    if (!testResult) {
      placementCursor = WORLD_CURSOR_PLACEMENT_BLOCKED;
    }
    InGameSelectionDetailPanel_Rebuild();
    return placementCursor;
  }
  testResult = SelectionInfo_TestNotOwnAircraftPadsWithAircraft(ownerIndex);
  if (!testResult) {
    /* command mode: an own army under the pointer (candidate height at most 1.0 in Q12 above the picked
       height) is selected on click; otherwise the modifier keys pick the pointer-mode command to preview */
    g_InGamePointerInteractionStateFlags = g_InGamePointerInteractionStateFlags & ~WORLD_POINTER_STATE_OVER_OWN_ARMY;
    if ((candidateNode != nullptr) &&
        (candidateNode->ownerClassId == WORLD_OWNER_RUNTIME_MODEL) &&
        ((int)(candidateHeightQ12 - Q12_ONE) <= (int)pickedHeightQ12) &&
        !GameFactionRuntime_TestCapabilityBitClear
              (WorldOwnerNode_ModelRuntime(candidateNode)->ownerArmyRuntimeOrSavedOffset.armyRuntime->
               factionIndex,
               ownerIndex)) {
      g_InGamePointerInteractionStateFlags = g_InGamePointerInteractionStateFlags | WORLD_POINTER_STATE_OVER_OWN_ARMY;
      return WORLD_CURSOR_OWN_ARMY;
    }
    modifierModeMask = 0;
    if ((g_KeyboardStateMask & KEYBOARD_STATE_ANY_MODIFIER) == 0) {
      modifierModeMask = 7;
    }
    if ((g_KeyboardStateMask & KEYBOARD_STATE_SHIFT) != 0) {
      modifierModeMask = modifierModeMask | 1;
    }
    if ((g_KeyboardStateMask & KEYBOARD_STATE_ALT) != 0) {
      modifierModeMask = modifierModeMask | 2;
    }
    if ((g_KeyboardStateMask & KEYBOARD_STATE_CTRL) != 0) {
      modifierModeMask = modifierModeMask | 4;
    }
    variantMask = SelectionInfo_CollectAttachmentEffectVariantMask();
    g_InGameCommandPreviewWorldYQ12 = pointerWorldYQ12;
    g_InGameCommandPreviewWorldXQ12 = pointerWorldXQ12;
    g_InGameCommandPreviewSurfaceHeightQ12OrSentinel = pickedHeightQ12;
    g_InGameCommandPreviewArmyAssetId = g_InGamePointerModePreviewArmyIds[modifierModeMask & variantMask];
    return g_InGamePointerModeCommandIds[modifierModeMask & variantMask];
  }
  if ((inGameRuntime->runtimeFlags & WORLD_RUNTIME_FLAG_DRAG_SELECTING) != 0) {
    return GRAPHICS_CURSOR_FRAME_ARROW;
  }
  /* selection mode: only an owned army at most 1.0 (Q12) above the picked height counts as candidate;
     entry stays NULL without a candidate army */
  entry = nullptr;
  if ((candidateNode != nullptr) && (candidateNode->ownerClassId == WORLD_OWNER_RUNTIME_MODEL)) {
    entry = ModelView_Cast<GameEntityRuntime>
            (WorldOwnerNode_ModelRuntime(candidateNode)->ownerArmyRuntimeOrSavedOffset.armyRuntime);
    if (((int)pickedHeightQ12 < (int)(candidateHeightQ12 - Q12_ONE)) ||
        ((entry->common).ownership.ownerIndex == 0)) {
      entry = nullptr;
    }
  }
  if (entry != nullptr) {
    (inGameRuntime->selection).selectedEntity = entry;
  }
  if (!SelectionInfo_HasAnyEntry() || SelectionInfo_AllEntriesEmptyOrMatchOwner(ownerIndex)) {
    if (entry == nullptr) {
      return GRAPHICS_CURSOR_FRAME_ARROW;
    }
    return (ownerIndex != (entry->common).ownership.ownerIndex) ? WORLD_CURSOR_FOREIGN_ARMY : WORLD_CURSOR_OWN_ARMY;
  }
  if ((g_KeyboardStateMask & KEYBOARD_STATE_CTRL) != 0) {
    if ((g_KeyboardStateMask & (KEYBOARD_STATE_SHIFT | KEYBOARD_STATE_ALT)) != 0) {
      if (entry == nullptr) {
        return GRAPHICS_CURSOR_FRAME_ARROW;
      }
      return (ownerIndex == (entry->common).ownership.ownerIndex) ? WORLD_CURSOR_OWN_ARMY : WORLD_CURSOR_FOREIGN_ARMY;
    }
    if (SelectionInfo_TestAnyEntryWeaponDamageNonnegative()) {
      return InGameWorldInput_ResolveWeaponTargetCursor(entry,ownerIndex);
    }
    return InGameWorldInput_ResolveCandidateConditionCursor(entry,ownerIndex);
  }
  if (entry == nullptr) {
    /* ground: move cursor unless no move is possible here */
    if (SelectionInfo_TestAnyActiveOrSingleClass13()) {
      return GRAPHICS_CURSOR_FRAME_ARROW;
    }
    if (SelectionInfo_TestPositionCommandAtWorldPoint(pointerWorldXQ12,pointerWorldYQ12,inGameRuntime)) {
      return WORLD_CURSOR_NO_TARGET;
    }
    if (pickedHeightQ12 == WORLD_POINTER_NO_HIT) {
      return WORLD_CURSOR_NO_TARGET;
    }
    return WORLD_CURSOR_MOVE;
  }
  if ((g_KeyboardStateMask & (KEYBOARD_STATE_SHIFT | KEYBOARD_STATE_ALT)) != 0) {
    return (ownerIndex == (entry->common).ownership.ownerIndex) ? WORLD_CURSOR_OWN_ARMY : WORLD_CURSOR_FOREIGN_ARMY;
  }
  if (!SelectionInfo_TestAnyEntryWeaponDamageNonnegative()) {
    return InGameWorldInput_ResolveCandidateConditionCursor(entry,ownerIndex);
  }
  if (!GameFactionRuntime_TestCapabilityBitClear((entry->common).ownership.ownerIndex,ownerIndex)) {
    return (ownerIndex != (entry->common).ownership.ownerIndex) ? WORLD_CURSOR_FOREIGN_ARMY : WORLD_CURSOR_OWN_ARMY;
  }
  return InGameWorldInput_ResolveWeaponTargetCursor(entry,ownerIndex);
}

/* Pointer-press callback of the world view: restores a saved camera, remembers the press position for the
   placement or command-mode heading drag, marks a selection-mode capture, or in command mode selects an own
   army under the pointer right away. The release is handled by InGameWorldInput_CommitPointerAction.
*/
void InGameWorldInput_BeginPointerCapture
          (InGamePointerCallbackValue0 pickedHeightQ12,uint32_t pointerWorldXQ12,uint32_t pointerWorldYQ12,
          InGamePointerCallbackValue3 candidateHeightQ12,WorldOwnerListNode *candidateNode,
          WorldRuntimeContext *inGameRuntime)

{
  FactionRuntimeIndex ownerIndex;
  ArmyRuntimeSlot *candidateArmy;
  CommandPayload modelToken;

  if ((g_UiCommandRuntimeFlags & (UI_COMMAND_RUNTIME_FLAG_PAUSED | UI_COMMAND_RUNTIME_FLAG_WORLD_INPUT_DISABLED)) !=
      0) {
    return;
  }
  inGameRuntime->runtimeFlags = inGameRuntime->runtimeFlags & ~WORLD_RUNTIME_FLAG_REPLACE_SELECTION;
  if (((inGameRuntime->interaction).nodeFlags & 8) != 0) {
    return;
  }
  if (((inGameRuntime->interaction).nodeFlags & UI_NODE_REPEAT_OR_DOUBLE_CLICK) != 0) {
    /* makes the release replace the selection instead of selecting a single army */
    inGameRuntime->runtimeFlags = inGameRuntime->runtimeFlags | WORLD_RUNTIME_FLAG_REPLACE_SELECTION;
  }
  ownerIndex = inGameRuntime->activeFactionRuntimeIndex;
  if ((inGameRuntime->runtimeFlags & WORLD_RUNTIME_FLAG_NOTIFICATION_GOTO) != 0) {
    inGameRuntime->runtimeFlags = inGameRuntime->runtimeFlags & ~WORLD_RUNTIME_FLAG_NOTIFICATION_GOTO;
    WorldRuntime_RestoreMotionStateFromSnapshot(inGameRuntime);
    return;
  }
  if ((g_UiCommandRuntimeFlags & UI_COMMAND_RUNTIME_FLAG_PLACEMENT_PENDING) != 0) {
    g_InGamePlacementPointerCaptureX =
         THANDOR_CONTAINER_OF(inGameRuntime, InGameRuntimeRoot, worldRuntime)->pointerPressX;
    g_InGamePlacementPointerCaptureY =
         THANDOR_CONTAINER_OF(inGameRuntime, InGameRuntimeRoot, worldRuntime)->pointerPressY;
    return;
  }
  if (SelectionInfo_TestNotOwnAircraftPadsWithAircraft(ownerIndex)) {
    g_InGamePointerInteractionStateFlags = g_InGamePointerInteractionStateFlags |
         WORLD_POINTER_STATE_SELECTION_CAPTURE;
    return;
  }
  /* command mode: an own army under the pointer is selected right away */
  if ((candidateNode != nullptr) && (candidateNode->ownerClassId == WORLD_OWNER_RUNTIME_MODEL)) {
    candidateArmy = WorldOwnerNode_ModelRuntime(candidateNode)->ownerArmyRuntimeOrSavedOffset.armyRuntime;
    if (((int)(candidateHeightQ12 - Q12_ONE) <= (int)pickedHeightQ12) &&
        !GameFactionRuntime_TestCapabilityBitClear(candidateArmy->factionIndex,ownerIndex)) {
      modelToken = ArmyRuntime_Token(candidateArmy);
      if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_NETWORKED_MASK) != SESSION_NETWORK_ROLE_LOCAL) {
        InGameCommandQueue_AppendLocalPlayerCommand(INGAME_COMMAND_SELECT_SINGLE_ARMY,0,0,modelToken);
        return;
      }
      FrontendPlayerRuntime_AssignModelTokenAndRefreshSelection(g_LocalPlayerRuntimeId,0,0,modelToken);
      return;
    }
  }
  g_InGameCommandPointerCaptureX =
       THANDOR_CONTAINER_OF(inGameRuntime, InGameRuntimeRoot, worldRuntime)->pointerPressX;
  g_InGameCommandPointerCaptureY =
       THANDOR_CONTAINER_OF(inGameRuntime, InGameRuntimeRoot, worldRuntime)->pointerPressY;
  g_UiCommandRuntimeFlags = g_UiCommandRuntimeFlags | UI_COMMAND_RUNTIME_FLAG_COMMAND_POINTER_CAPTURED;
}

/* Selection-mode capture of InGameWorldInput_UpdateDragSelectionAndCamera: once the pointer travelled more than
   WORLD_DRAG_SELECTION_THRESHOLD pixels from the press on either axis, starts the drag selection and clears the
   selection. */
static void InGameWorldInput_BeginDragSelectionIfMoved(WorldRuntimeContext *inGameRuntime)

{
  InGameRuntimeRoot *root;
  uint32_t deltaX;
  uint32_t deltaY;

  /* pointer travel since the press (the pointer positions are kept in the in-game root) */
  root = THANDOR_CONTAINER_OF(inGameRuntime, InGameRuntimeRoot, worldRuntime);
  deltaX = root->pointerPressX - root->pointerX;
  if ((int)deltaX < 0) {
    deltaX = 0u - deltaX;
  }
  deltaY = root->pointerPressY - root->pointerY;
  if ((int)deltaY < 0) {
    deltaY = 0u - deltaY;
  }
  if ((WORLD_DRAG_SELECTION_THRESHOLD < deltaX) || (WORLD_DRAG_SELECTION_THRESHOLD < deltaY)) {
    inGameRuntime->runtimeFlags = inGameRuntime->runtimeFlags | WORLD_RUNTIME_FLAG_DRAG_SELECTING;
    InGameCommand_Issue<FrontendPlayerSelection_ClearAndRefreshLocalPanels>(0,0,0);
  }
}

/* Stores one rebased army offset in a 12-dword drag-selection batch and counts it.
   Original quirk: the count stops at 11, so every further value overwrites the twelfth slot; the flush still
   sends that slot as part of the fourth triplet. */
static void InGameWorldInput_AppendDragSelectionBatchValue
          (InGameCommandPayloadTripletValue32 *batchBase,int32_t *batchCount,
           InGameCommandPayloadTripletValue32 payloadValue)

{
  uint32_t slotIndex;

  slotIndex = (uint32_t)*batchCount;
  batchBase[slotIndex] = payloadValue;
  if (slotIndex < 11) {
    *batchCount = *batchCount + 1;
  }
}

/* Drag selection: clears both batches, then sorts every own model's army into the insert batch (inside the
   rectangle, not yet selected, not already queued for insertion) or the remove batch (outside, selected, not
   already queued for removal). */
static void InGameWorldInput_CollectDragSelectionBatches(WorldRuntimeContext *inGameRuntime)

{
  int clearIndex;
  WorldOwnerListNode *runtimeNode;
  int ownerIndex;
  GameEntityRuntime *entry;
  InGameCommandPayloadTripletValue32 payloadValue;

  /* clears both 12-dword batches and their two counters, in memory order (the original clears the 0x1A
     contiguous dwords in one run) */
  for (clearIndex = 0; clearIndex < 12; clearIndex++) {
    g_InGameSelectionInsertTripletDwords[clearIndex] = 0;
  }
  for (clearIndex = 0; clearIndex < 12; clearIndex++) {
    g_InGameSelectionRemoveTripletDwords[clearIndex] = 0;
  }
  g_InGameSelectionInsertTripletDwordCount = 0;
  g_InGameSelectionRemoveTripletDwordCount = 0;
  ownerIndex = inGameRuntime->activeFactionRuntimeIndex;
  for (runtimeNode = inGameRuntime->ownerListHead; runtimeNode != nullptr; runtimeNode = runtimeNode->nextNode) {
    if ((runtimeNode->ownerClassId != WORLD_OWNER_RUNTIME_MODEL) || !Any(runtimeNode->runtimeFlags & MODEL_NODE_FLAG_RENDERED)) {
      continue;
    }
    entry = ModelView_Cast<GameEntityRuntime>
            (WorldOwnerNode_ModelRuntime(runtimeNode)->ownerArmyRuntimeOrSavedOffset.armyRuntime);
    if (!Any(runtimeNode->runtimeFlags & MODEL_NODE_FLAG_FACTION_OWNED) ||
        (ownerIndex != (entry->common).ownership.ownerIndex)) {
      continue;
    }
    payloadValue = ArmyRuntime_Token(entry);
    if (WorldRuntimeNode_IsPositionInsideBounds(runtimeNode,UiNode_As<WorldRuntimeExtendedMapControlView>(inGameRuntime))) {
      if (SelectionInfo_IsEntryAbsent(entry) &&
          !InGameCommandQueue_ContainsTripletValue
                (payloadValue,INGAME_COMMAND_CODE_BASE + INGAME_COMMAND_SELECTION_INSERT)) {
        InGameWorldInput_AppendDragSelectionBatchValue
                  (g_InGameSelectionInsertTripletDwords,&g_InGameSelectionInsertTripletDwordCount,payloadValue);
      }
    }
    else if (!SelectionInfo_IsEntryAbsent(entry) &&
             !InGameCommandQueue_ContainsTripletValue
                   (payloadValue,INGAME_COMMAND_CODE_BASE + INGAME_COMMAND_SELECTION_REMOVE)) {
      InGameWorldInput_AppendDragSelectionBatchValue
                (g_InGameSelectionRemoveTripletDwords,&g_InGameSelectionRemoveTripletDwordCount,payloadValue);
    }
  }
}

/* Drag selection: sends the remove batch, then the insert batch, three values per command. The counters end
   at or below zero (they are cleared again before the next collection). */
static void InGameWorldInput_FlushDragSelectionBatches()

{
  CommandPayload *tripletCursor;
  int32_t countBeforeTriplet;

  if (g_InGameSelectionRemoveTripletDwordCount != 0) {
    tripletCursor = g_InGameSelectionRemoveTripletDwords;
    do {
      InGameCommand_Issue<FrontendPlayerSelection_RemoveThreeEntriesAndRefresh>
                (tripletCursor[2],tripletCursor[1],*tripletCursor);
      countBeforeTriplet = g_InGameSelectionRemoveTripletDwordCount;
      tripletCursor = tripletCursor + 3;
      g_InGameSelectionRemoveTripletDwordCount = g_InGameSelectionRemoveTripletDwordCount - 3;
    } while (g_InGameSelectionRemoveTripletDwordCount != 0 && 2 < countBeforeTriplet);
  }
  if (g_InGameSelectionInsertTripletDwordCount != 0) {
    tripletCursor = g_InGameSelectionInsertTripletDwords;
    do {
      InGameCommand_Issue<FrontendPlayerSelection_InsertThreeEntriesAndRefresh>
                (tripletCursor[2],tripletCursor[1],*tripletCursor);
      countBeforeTriplet = g_InGameSelectionInsertTripletDwordCount;
      tripletCursor = tripletCursor + 3;
      g_InGameSelectionInsertTripletDwordCount = g_InGameSelectionInsertTripletDwordCount - 3;
    } while (g_InGameSelectionInsertTripletDwordCount != 0 && 2 < countBeforeTriplet);
  }
}

/* Placement / command mode heading drag: without button bit 4 the horizontal pointer travel since the capture
   turns the heading by 0x40 per pixel (of the 0x10000 full circle) and the pointer is snapped back to the
   capture position; with it, the capture position follows the pointer. */
static void InGameWorldInput_RotateHeadingByPointerTravel
          (WorldRuntimeContext *inGameRuntime,uint32_t *heading16,uint32_t *captureX,uint32_t *captureY)

{
  int deltaX;

  if ((g_CursorButtonState & 4) == 0) {
    /* The original adds the horizontal mouse delta since capture (computed before snapping the
       pointer back) - not the pointer function's return value. */
    deltaX = THANDOR_CONTAINER_OF(inGameRuntime, InGameRuntimeRoot, worldRuntime)->pointerX - *captureX;
    g_PointerSetPosition(*captureY,*captureX);
    *heading16 = *heading16 + deltaX * 64;
    *heading16 = *heading16 & FIXED_ANGLE16_MASK;
  }
  else {
    *captureX = THANDOR_CONTAINER_OF(inGameRuntime, InGameRuntimeRoot, worldRuntime)->pointerX;
    *captureY = THANDOR_CONTAINER_OF(inGameRuntime, InGameRuntimeRoot, worldRuntime)->pointerY;
  }
}

/* Pointer-move callback while the pointer is captured. In selection mode a press that moved more than 23 pixels
   becomes a drag selection: every own army inside the rectangle is inserted, every one outside removed (in
   batches of three per command, skipping armies already queued). In placement and command mode, horizontal
   travel rotates the placement/command heading and the pointer is snapped back to the press position.
*/
void InGameWorldInput_UpdateDragSelectionAndCamera
          (InGamePointerCallbackValue0 pickedHeightQ12,uint32_t pointerWorldXQ12,uint32_t pointerWorldYQ12,
          uint32_t candidateHeightQ12,WorldOwnerListNode *candidateNode,
          WorldRuntimeContext *inGameRuntime)

{
  if (((g_UiCommandRuntimeFlags & (UI_COMMAND_RUNTIME_FLAG_PAUSED | UI_COMMAND_RUNTIME_FLAG_WORLD_INPUT_DISABLED)) !=
       0) ||
      (((inGameRuntime->interaction).nodeFlags & 8) != 0) ||
      ((inGameRuntime->runtimeFlags & WORLD_RUNTIME_FLAG_NOTIFICATION_GOTO) != 0)) {
    return;
  }
  if ((g_UiCommandRuntimeFlags & UI_COMMAND_RUNTIME_FLAG_PLACEMENT_PENDING) != 0) {
    if (pickedHeightQ12 != WORLD_POINTER_NO_HIT) {
      InGameWorldInput_RotateHeadingByPointerTravel
                (inGameRuntime,&g_InGamePlacementHeading16,&g_InGamePlacementPointerCaptureX,
                 &g_InGamePlacementPointerCaptureY);
    }
    return;
  }
  if (!SelectionInfo_TestNotOwnAircraftPadsWithAircraft(inGameRuntime->activeFactionRuntimeIndex)) {
    /* command mode: only a press over the ground (not over an own army, no selection capture) turns */
    if ((pickedHeightQ12 != WORLD_POINTER_NO_HIT) &&
        ((g_InGamePointerInteractionStateFlags &
          (WORLD_POINTER_STATE_OVER_OWN_ARMY | WORLD_POINTER_STATE_SELECTION_CAPTURE)) == 0)) {
      InGameWorldInput_RotateHeadingByPointerTravel
                (inGameRuntime,&g_InGameCommandPreviewHeading16,&g_InGameCommandPointerCaptureX,
                 &g_InGameCommandPointerCaptureY);
    }
    return;
  }
  if ((inGameRuntime->runtimeFlags & WORLD_RUNTIME_FLAG_DRAG_SELECTING) == 0) {
    InGameWorldInput_BeginDragSelectionIfMoved(inGameRuntime);
    return;
  }
  InGameWorldInput_CollectDragSelectionBatches(inGameRuntime);
  InGameWorldInput_FlushDragSelectionBatches();
}

/* Command-mode release of InGameWorldInput_CommitPointerAction: when the press captured the pointer over the
   ground (not over an own army, no selection capture), issues the command that the modifier keys pick from the
   selection's attachment effect variants and then clears the selection. */
static void InGameWorldInput_CommitCommandModeRelease
          (InGamePointerCallbackValue0 pickedHeightQ12,InGamePointerCallbackValue1 pointerWorldXQ12,
          InGamePointerCallbackValue2 pointerWorldYQ12)

{
  InGamePointerModeHandler *modeHandler;
  uint32_t modifierModeMask;
  uint32_t variantMask;

  if ((pickedHeightQ12 == WORLD_POINTER_NO_HIT) ||
      ((g_UiCommandRuntimeFlags & UI_COMMAND_RUNTIME_FLAG_COMMAND_POINTER_CAPTURED) == 0) ||
      ((g_InGamePointerInteractionStateFlags &
        (WORLD_POINTER_STATE_OVER_OWN_ARMY | WORLD_POINTER_STATE_SELECTION_CAPTURE)) != 0)) {
    return;
  }
  g_UiCommandRuntimeFlags = g_UiCommandRuntimeFlags & ~UI_COMMAND_RUNTIME_FLAG_COMMAND_POINTER_CAPTURED;
  modifierModeMask = 0;
  if ((g_KeyboardStateMask & KEYBOARD_STATE_ANY_MODIFIER) == 0) {
    modifierModeMask = 7;
  }
  if ((g_KeyboardStateMask & KEYBOARD_STATE_SHIFT) != 0) {
    modifierModeMask = modifierModeMask | 1;
  }
  if ((g_KeyboardStateMask & KEYBOARD_STATE_ALT) != 0) {
    modifierModeMask = modifierModeMask | 2;
  }
  if ((g_KeyboardStateMask & KEYBOARD_STATE_CTRL) != 0) {
    modifierModeMask = modifierModeMask | 4;
  }
  variantMask = SelectionInfo_CollectAttachmentEffectVariantMask();
  if ((modifierModeMask & variantMask) == 0) {
    return;
  }
  modeHandler = g_InGamePointerModeHandlers[modifierModeMask & variantMask];
  /* The original pushes the same four arguments locally (local player id) and networked (the handler's code in
     the in-game command table; in the original its address minus INGAME_COMMAND_CODE_BASE). */
  InGameCommand_IssueHandler
            (reinterpret_cast<CommandQueueHandlerProc *>(modeHandler) /* int first parameter: same ABI */,
             g_InGameCommandPreviewHeading16,pointerWorldXQ12,
             pointerWorldYQ12);
  InGameCommand_Issue<FrontendPlayerSelection_ClearAndRefreshLocalPanels>(0,0,0);
}

/* Selection-mode click: adds the candidate army to the selection (INGAME_COMMAND_SELECT_ARMY). */
static void InGameWorldInput_SelectCandidateArmy(GameEntityRuntime *entry)

{
  InGameCommand_Issue<InGamePlayerSelection_SelectArmyRuntimeIndex>(0,0,ArmyRuntime_Token(entry));
}

/* Selection-mode click on an own candidate army: selects it alone, or replaces the selection with it when the
   press set WORLD_RUNTIME_FLAG_REPLACE_SELECTION (see BeginPointerCapture). A foreign army is ignored. */
static void InGameWorldInput_SelectOwnCandidateArmy
          (WorldRuntimeContext *inGameRuntime,int ownerIndex,GameEntityRuntime *entry)

{
  CommandPayload armyRuntimeIndex;

  if (ownerIndex != (entry->common).ownership.ownerIndex) {
    return;
  }
  armyRuntimeIndex = ArmyRuntime_Token(entry);
  if ((inGameRuntime->runtimeFlags & WORLD_RUNTIME_FLAG_REPLACE_SELECTION) == 0) {
    InGameCommand_Issue<FrontendPlayerRuntime_AssignModelTokenAndRefreshSelection>(0,0,armyRuntimeIndex);
  }
  else {
    InGameCommand_Issue<InGamePlayerSelection_ReplaceWithArmyRuntimeIndex>(0,0,armyRuntimeIndex);
  }
}

/* Selection-mode click on a candidate while no selected entry has a nonnegative weapon damage: a candidate whose
   faction capability bit is clear is ignored, one whose hierarchy condition ratio is not Q12_ONE is added to
   the selection, otherwise an own army is selected alone (InGameWorldInput_SelectOwnCandidateArmy). */
static void InGameWorldInput_CommitCandidateConditionClick
          (WorldRuntimeContext *inGameRuntime,int ownerIndex,GameEntityRuntime *entry)

{
  Bool8 capabilityClear;
  Q12 conditionRatioQ12;

  capabilityClear = GameFactionRuntime_TestCapabilityBitClear((entry->common).ownership.ownerIndex,ownerIndex);
  if (capabilityClear) {
    return;
  }
  conditionRatioQ12 = ModelRuntime_QueryHierarchyConditionRatioQ12(ModelView_Cast<RuntimeModelFactionPrefix>(entry));
  if (conditionRatioQ12 != Q12_ONE) {
    InGameWorldInput_SelectCandidateArmy(entry);
    return;
  }
  InGameWorldInput_SelectOwnCandidateArmy(inGameRuntime,ownerIndex,entry);
}

/* Shift/Alt-click (with or without Ctrl) on a candidate: toggles an own army in the selection
   (SelectionInfo_IsEntryAbsent is true when the entry is absent), selects a foreign one alone. */
static void InGameWorldInput_ToggleCandidateArmy(int ownerIndex,GameEntityRuntime *entry)

{
  Bool8 entryAbsent;

  if (ownerIndex == (entry->common).ownership.ownerIndex) {
    entryAbsent = SelectionInfo_IsEntryAbsent(entry);
    if (entryAbsent) {
      InGameCommand_Issue<FrontendPlayerSelection_InsertThreeEntriesAndRefresh>(0,0,ArmyRuntime_Token(entry));
    }
    else {
      InGameCommand_Issue<FrontendPlayerSelection_RemoveThreeEntriesAndRefresh>(0,0,ArmyRuntime_Token(entry));
    }
  }
  else {
    InGameCommand_Issue<FrontendPlayerRuntime_AssignModelTokenAndRefreshSelection>(0,0,ArmyRuntime_Token(entry));
  }
}

/* Selection-mode release of InGameWorldInput_CommitPointerAction: resolves the army under the pointer and turns
   the click into select / add / remove, move, position, target-position or target-army commands. */
static void InGameWorldInput_CommitSelectionModeRelease
          (InGamePointerCallbackValue0 pickedHeightQ12,InGamePointerCallbackValue1 pointerWorldXQ12,
          InGamePointerCallbackValue2 pointerWorldYQ12,InGamePointerCallbackValue3 candidateHeightQ12,
          WorldOwnerListNode *candidateNode,WorldRuntimeContext *inGameRuntime)

{
  int ownerIndex;
  GameEntityRuntime *entry;
  uint32_t surfaceHeightQ12;
  Bool8 capabilityClear;

  /* same candidate filter as InGameWorldInput_ResolveContextActionAndCursor; entry stays NULL without a
     candidate army */
  ownerIndex = inGameRuntime->activeFactionRuntimeIndex;
  entry = nullptr;
  if ((candidateNode != nullptr) && (candidateNode->ownerClassId == WORLD_OWNER_RUNTIME_MODEL)) {
    entry = ModelView_Cast<GameEntityRuntime>(WorldOwnerNode_ModelRuntime(candidateNode)->ownerArmyRuntimeOrSavedOffset.armyRuntime);
    if (((int)pickedHeightQ12 < (int)(candidateHeightQ12 - Q12_ONE)) ||
        ((entry->common).ownership.ownerIndex == 0)) {
      entry = nullptr;
    }
  }
  if (!SelectionInfo_HasAnyEntry() || SelectionInfo_AllEntriesEmptyOrMatchOwner(ownerIndex)) {
    if (entry != nullptr) {
      InGameCommand_Issue<FrontendPlayerRuntime_AssignModelTokenAndRefreshSelection>(0,0,ArmyRuntime_Token(entry));
    }
    return;
  }
  if ((g_KeyboardStateMask & KEYBOARD_STATE_CTRL) == 0) {
    if (entry == nullptr) {
      /* ground click: move, or position with Shift/Alt */
      if (!SelectionInfo_TestAnyActiveOrSingleClass13() && (pickedHeightQ12 != WORLD_POINTER_NO_HIT)) {
        if ((g_KeyboardStateMask & (KEYBOARD_STATE_SHIFT | KEYBOARD_STATE_ALT)) == 0) {
          InGameCommand_Issue<InGamePlayerSelection_ApplyMoveCommand>(0,pointerWorldXQ12,pointerWorldYQ12);
        }
        else {
          InGameCommand_Issue<InGamePlayerSelection_ApplyPositionCommand>(0,pointerWorldXQ12,pointerWorldYQ12);
        }
      }
      return;
    }
    if ((g_KeyboardStateMask & (KEYBOARD_STATE_SHIFT | KEYBOARD_STATE_ALT)) == 0) {
      if (SelectionInfo_TestAnyEntryWeaponDamageNonnegative()) {
        capabilityClear =
             GameFactionRuntime_TestCapabilityBitClear((entry->common).ownership.ownerIndex,ownerIndex);
        if (capabilityClear) {
          InGameWorldInput_SelectCandidateArmy(entry);
        }
        else {
          InGameWorldInput_SelectOwnCandidateArmy(inGameRuntime,ownerIndex,entry);
        }
      }
      else {
        InGameWorldInput_CommitCandidateConditionClick(inGameRuntime,ownerIndex,entry);
      }
      return;
    }
  }
  else if ((g_KeyboardStateMask & (KEYBOARD_STATE_SHIFT | KEYBOARD_STATE_ALT)) == 0) {
    if (entry == nullptr) {
      /* Ctrl ground click: target position at the top surface height under the pointer */
      if (pickedHeightQ12 != WORLD_POINTER_NO_HIT) {
        surfaceHeightQ12 = WorldRuntime_InterpolateTopSurfaceHeightOrSentinel
                                (pointerWorldXQ12,pointerWorldYQ12,inGameRuntime);
        InGameCommand_Issue<InGamePlayerSelection_ApplyTargetPositionCommand>
                  (surfaceHeightQ12,pointerWorldXQ12,pointerWorldYQ12);
      }
      return;
    }
    if (!SelectionInfo_TestAnyEntryWeaponDamageNonnegative()) {
      InGameWorldInput_CommitCandidateConditionClick(inGameRuntime,ownerIndex,entry);
      return;
    }
    capabilityClear = GameFactionRuntime_TestCapabilityBitClear((entry->common).ownership.ownerIndex,ownerIndex);
    if (!capabilityClear && !SelectionInfo_IsEntryAbsent(entry)) {
      return;
    }
    InGameWorldInput_SelectCandidateArmy(entry);
    return;
  }
  if (entry != nullptr) {
    InGameWorldInput_ToggleCandidateArmy(ownerIndex,entry);
  }
}

/* Release handling of InGameWorldInput_CommitPointerAction except the final end of the selection-mode capture,
   which the caller does on every path. */
static void InGameWorldInput_DispatchPointerRelease
          (InGamePointerCallbackValue0 pickedHeightQ12,InGamePointerCallbackValue1 pointerWorldXQ12,
          InGamePointerCallbackValue2 pointerWorldYQ12,InGamePointerCallbackValue3 candidateHeightQ12,
          WorldOwnerListNode *candidateNode,WorldRuntimeContext *inGameRuntime)

{
  if (((g_UiCommandRuntimeFlags & (UI_COMMAND_RUNTIME_FLAG_PAUSED | UI_COMMAND_RUNTIME_FLAG_WORLD_INPUT_DISABLED)) !=
       0) ||
      (((inGameRuntime->interaction).nodeFlags & 8) != 0) ||
      ((g_UiCommandRuntimeFlags & UI_COMMAND_RUNTIME_FLAG_WORLD_INPUT_DISABLED) != 0) ||
      ((inGameRuntime->runtimeFlags & WORLD_RUNTIME_FLAG_NOTIFICATION_GOTO) != 0)) {
    return;
  }
  if ((g_UiCommandRuntimeFlags & UI_COMMAND_RUNTIME_FLAG_PLACEMENT_PENDING) != 0) {
    if (pickedHeightQ12 != WORLD_POINTER_NO_HIT) {
      InGameCommand_Issue<InGameCommand_ExecuteLocalPlacementFromSelection>
                (g_InGamePlacementHeading16,pointerWorldXQ12,pointerWorldYQ12);
    }
    return;
  }
  if (!SelectionInfo_TestNotOwnAircraftPadsWithAircraft(inGameRuntime->activeFactionRuntimeIndex)) {
    InGameWorldInput_CommitCommandModeRelease(pickedHeightQ12,pointerWorldXQ12,pointerWorldYQ12);
    return;
  }
  if ((inGameRuntime->runtimeFlags & WORLD_RUNTIME_FLAG_DRAG_SELECTING) != 0) {
    inGameRuntime->runtimeFlags = inGameRuntime->runtimeFlags & ~WORLD_RUNTIME_FLAG_DRAG_SELECTING;
    return;
  }
  InGameWorldInput_CommitSelectionModeRelease
            (pickedHeightQ12,pointerWorldXQ12,pointerWorldYQ12,candidateHeightQ12,candidateNode,inGameRuntime);
}

/* Pointer-release callback of the world view: places the pending army, issues the command-mode command chosen by
   the modifier keys, ends a drag selection, or (selection mode) turns the click into select / add / remove,
   move, target-position or target-army commands. Every action goes through the command queue in network games
   and calls the handler directly in single player. Always ends the selection-mode capture.
*/
void InGameWorldInput_CommitPointerAction
          (InGamePointerCallbackValue0 pickedHeightQ12,InGamePointerCallbackValue1 pointerWorldXQ12,
          InGamePointerCallbackValue2 pointerWorldYQ12,InGamePointerCallbackValue3 candidateHeightQ12,
          WorldOwnerListNode *candidateNode,WorldRuntimeContext *inGameRuntime)

{
  InGameWorldInput_DispatchPointerRelease
            (pickedHeightQ12,pointerWorldXQ12,pointerWorldYQ12,candidateHeightQ12,candidateNode,inGameRuntime);
  g_InGamePointerInteractionStateFlags = g_InGamePointerInteractionStateFlags & ~WORLD_POINTER_STATE_SELECTION_CAPTURE;
}

/* Drag selection test: projects the node's world position to the screen and returns true when that pixel
   lies inside the rectangle spanned by the pointer press position and the current pointer position of
   boundsControl (inclusive, in either corner order).
*/
Bool8 WorldRuntimeNode_IsPositionInsideBounds
          (WorldOwnerListNode *runtimeNode,WorldRuntimeExtendedMapControlView *boundsControl)

{
  int boundsSecondX;
  int boundsSecondY;
  int projectedScreenX;
  int boundsMaxX;
  int projectedScreenY;
  int boundsMinX;
  int boundsMaxY;
  int boundsMinY;
  GraphicsProjectedPointPair projectedPosition;
  
  FixedTransform_ApplyPoint
            (&g_GraphicsProjectionScratchVec3,reinterpret_cast<GraphicsFixedVec3 *>(&runtimeNode->worldXQ12) /* worldX/Y/ZQ12 */,
             &g_ViewProjectionMatrixFixed);
  projectedPosition = Graphics_ProjectViewPoint(&g_GraphicsProjectionScratchVec3);
  boundsMinX = boundsControl->pointerPressX;
  boundsSecondX = boundsControl->pointerX;
  boundsMinY = boundsControl->pointerPressY;
  boundsSecondY = boundsControl->pointerY;
  /* the projection is in Q12 screen pixels */
  projectedScreenX = projectedPosition.projectedX >> 12;
  projectedScreenY = projectedPosition.projectedY >> 12;
  boundsMaxX = boundsSecondX;
  if (boundsSecondX < boundsMinX) {
    boundsMaxX = boundsMinX;
    boundsMinX = boundsSecondX;
  }
  boundsMaxY = boundsSecondY;
  if (boundsSecondY < boundsMinY) {
    boundsMaxY = boundsMinY;
    boundsMinY = boundsSecondY;
  }
  if (boundsMinX <= projectedScreenX && projectedScreenX <= boundsMaxX && boundsMinY <= projectedScreenY &&
      projectedScreenY <= boundsMaxY) {
    return true;
  }
  return false;
}

/* Edge scrolling: while the cursor presses against a screen edge (g_CursorOverflow*), moves the camera by the
   configured scroll step in that direction and returns the matching scroll-arrow cursor frame
   (WORLD_CURSOR_SCROLL_*), or 0 when no edge is touched.
*/
uint32_t WorldRuntime_ApplyEdgeScrollAndGetCursorFrame(WorldRuntimeContext *worldRuntime)

{
  uint32_t edgeScrollStep;
  uint32_t rightStep;
  uint32_t bottomStep;
  uint32_t screenDeltaRight;
  uint32_t screenDeltaDown;

  edgeScrollStep = PersistentSettings_Read(PERSISTENT_DEFAULT_CAMERA_SCROLL_STEP,PERSISTENT_SETTING_CAMERA_SCROLL_STEP);
  rightStep = 0;
  if (g_CursorOverflowRight != 0) {
    rightStep = edgeScrollStep;
  }
  bottomStep = 0;
  if (g_CursorOverflowBottom != 0) {
    bottomStep = edgeScrollStep;
  }
  /* delta = right/bottom step - left/top overflow; a negative result becomes -step */
  screenDeltaRight = rightStep - g_CursorOverflowLeft;
  if ((int)screenDeltaRight < 0) {
    screenDeltaRight = 0u - edgeScrollStep;
  }
  screenDeltaDown = bottomStep - g_CursorOverflowTop;
  if ((int)screenDeltaDown < 0) {
    screenDeltaDown = 0u - edgeScrollStep;
  }
  WorldRuntime_TranslateCameraByScreenDelta(screenDeltaDown,screenDeltaRight,worldRuntime);
  WorldRuntime_RecomputeMotionEndpointAgainstFieldSurface(worldRuntime);
  if (screenDeltaRight == 0) {
    if (screenDeltaDown == 0) {
      return 0;
    }
    if ((int)screenDeltaDown < 0) {
      return WORLD_CURSOR_SCROLL_UP;
    }
    return WORLD_CURSOR_SCROLL_DOWN;
  }
  if ((int)screenDeltaRight < 0) {
    if (screenDeltaDown == 0) {
      return WORLD_CURSOR_SCROLL_LEFT;
    }
    if ((int)screenDeltaDown < 0) {
      return WORLD_CURSOR_SCROLL_UP_LEFT;
    }
    return WORLD_CURSOR_SCROLL_DOWN_LEFT;
  }
  if (screenDeltaDown == 0) {
    return WORLD_CURSOR_SCROLL_RIGHT;
  }
  if ((int)screenDeltaDown < 0) {
    return WORLD_CURSOR_SCROLL_UP_RIGHT;
  }
  return WORLD_CURSOR_SCROLL_DOWN_RIGHT;
}

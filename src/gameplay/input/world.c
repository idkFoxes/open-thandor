/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/src/gameplay/input/world.c
 * Reverse engineering by idkFoxes 2026
 */

#include <thandor/gameplay/input/world.h>
#include <thandor/thandor.h>
#include <thandor/platform/bootstrap/image.h>

/* Implementation ownership: gameplay/input/world. */

/* Entries of g_InGamePointerModeHandlers (InGameSelection_ApplyType16MarkerCoordinatesVariant1/2,
   SelectionMarkerCoordinates_ApplyType3..7): four stack arguments, RET 0x10. */
typedef void InGamePointerModeHandler
          (SelectionMarkerIndex selectionIndex,SelectionMarkerCoordinateValue32 valueC,
          SelectionMarkerCoordinateValue32 valueB,SelectionMarkerCoordinateValue32 valueA);

/* Address: 0x0056D2D0.
   "Go to" action of the active in-game notification. In state 27 it only cancels (restores the camera, see
   InGameTargetingContext_CancelAndRestoreState). In state 7 it saves the camera state (unless bit 0x10 of the
   world runtimeFlags is set) and then, by payload kind: TECHNOLOGY_UNLOCK_POSITION selects the own model standing at
   the payload position (locally or as INGAME_COMMAND_SELECT_MODEL_AND_ARMY) and ends the interaction;
   FACTION_IMPACT_ANCHOR remembers the position and, like ARMY_CREATED, moves the camera onto the terrain point
   there and switches the interaction to state 27. Jump table 0x0056D340: kinds 1, 2, 3 are handled, the rest
   do nothing.
*/
void InGameTargetingContext_AdvanceOrResolveTarget(InGameTargetingRootTraversalView9E60 *targetingContext)

{
  WorldRuntimeFlags *runtimeFlagsField;
  UiNodeBase *parentNode;
  InGameNotificationPayloadKind payloadKind;
  Q12 secondaryCoordinateQ12;
  WorldOwnerListNode100 *ownerNode;
  int payloadEntityAddress;
  CommandPayloadDword04 modelToken;
  CommandPayloadDword08 armyToken;
  TerrainPointResult nearestTerrainPoint;
  
  if (targetingContext->actionState == INGAME_TARGETING_OBSERVED_CANCEL_AND_RESTORE) {
    InGameTargetingContext_CancelAndRestoreState(targetingContext);
  }
  else if (targetingContext->actionState == INGAME_TARGETING_OBSERVED_ADVANCE_OR_RESOLVE) {
    /* walk up to the in-game root node */
    parentNode = targetingContext->base.parent;
    while (parentNode != UI_NODE_NONE) {
      targetingContext = (InGameTargetingRootTraversalView9E60 *)targetingContext->base.parent;
      parentNode = targetingContext->base.parent;
    }
    payloadKind = targetingContext->activeNotificationPayload9E40.payloadKind14;
    if ((targetingContext->worldRuntime0A30.runtimeFlags & 0x10) == 0) {
      WorldRuntime_CaptureMotionStateToSnapshot(&targetingContext->worldRuntime0A30);
    }
    switch(payloadKind) {
    case TECHNOLOGY_UNLOCK_POSITION:
      /* own model at exactly the payload position; the tokens are the rebased army/model offsets */
      for (ownerNode = targetingContext->worldRuntime0A30.ownerListHead;
          ownerNode != NULL; ownerNode = ownerNode->nextNode) {
        if ((((ownerNode->ownerClassId == WORLD_OWNER_RUNTIME_MODEL) &&
             (ownerNode->worldXQ12 ==
              (targetingContext->activeNotificationPayload9E40).primaryWorldCoordinateQ12_00)) &&
            (payloadEntityAddress =
                  (int)((ModelRuntimeSlot *)ownerNode->runtimePayload)->ownerArmyRuntimeOrSavedOffset.armyRuntime,
            ownerNode->worldYQ12 ==
            (targetingContext->activeNotificationPayload9E40).secondaryWorldCoordinateQ12_04)) &&
           ((targetingContext->worldRuntime0A30).activeFactionRuntimeIndex ==
            ((ArmyRuntimeSlot *)payloadEntityAddress)->factionIndex)
           ) {
          modelToken = payloadEntityAddress - (int)g_ArmyRuntimeRebaseBaseMinusOne;
          armyToken = (int)ownerNode->runtimePayload - g_ModelRuntimeRebaseDelta;
          if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_NETWORKED_MASK) ==
              SESSION_NETWORK_ROLE_LOCAL) {
            FrontendPlayerRuntime_AssignModelAndArmyTokensAndRefreshLocalPanel
                      (g_LocalPlayerRuntimeId,0,armyToken,modelToken);
          }
          else {
            InGameCommandQueue_AppendLocalPlayerCommand(INGAME_COMMAND_SELECT_MODEL_AND_ARMY,0,armyToken,modelToken);
          }
          targetingContext->sessionNotificationInteractionState9B4C = NOTIFICATION_INTERACTION_NONE;
          return;
        }
      }
      break;
    case FACTION_IMPACT_ANCHOR:
      secondaryCoordinateQ12 = (targetingContext->activeNotificationPayload9E40).secondaryWorldCoordinateQ12_04;
      targetingContext->targetingPrimaryWorldCoordinateQ12_9E58 =
           (targetingContext->activeNotificationPayload9E40).primaryWorldCoordinateQ12_00;
      targetingContext->targetingSecondaryWorldCoordinateQ12_9E5C = secondaryCoordinateQ12;
      /* falls through */
    case ARMY_CREATED:
      nearestTerrainPoint = FieldGrid_GetNearestTerrainPoint
                        ((targetingContext->activeNotificationPayload9E40).
                         secondaryWorldCoordinateQ12_04,
                         (targetingContext->activeNotificationPayload9E40).
                         primaryWorldCoordinateQ12_00,(targetingContext->worldRuntime0A30).fieldGrid
                        );
      WorldRuntime_SetPosition80AndRebuildPosition60FromAngles
                ((targetingContext->worldRuntime0A30).motion.pitchAngle,
                 (targetingContext->activeNotificationPayload9E40).primaryOrientationAngle08,
                 (targetingContext->worldRuntime0A30).motion.targetDistanceQ12,nearestTerrainPoint.terrainHeightQ12,
                 (targetingContext->activeNotificationPayload9E40).secondaryWorldCoordinateQ12_04,
                 (targetingContext->activeNotificationPayload9E40).primaryWorldCoordinateQ12_00,
                 &targetingContext->worldRuntime0A30);
      runtimeFlagsField = &targetingContext->worldRuntime0A30.runtimeFlags;
      *runtimeFlagsField = *runtimeFlagsField & ~0x10;
      targetingContext->sessionNotificationInteractionState9B4C = 0x1b; /* 27: next click cancels */
    }
  }
  return;
}


/* Address: 0x005688A0.
   Hover callback of the world view: picks the cursor frame for the pointer position (placement valid/blocked,
   command-mode preview, own/foreign army, move or target) and records the hovered army as the selected entity,
   so the cursor always shows what a click at this point would do.
*/
uint32_t InGameWorldInput_ResolveContextActionAndCursor
                (InGamePointerCallbackValue0 pickedHeightQ12,InGamePointerCallbackValue1 pointerWorldXQ12
                ,InGamePointerCallbackValue2 pointerWorldYQ12,InGamePointerCallbackValue3 candidateHeightQ12
                ,WorldOwnerListNode100 *candidateNode,WorldRuntimeContext *inGameRuntime)

{
  uint32_t cursorOrVariantMask;
  int ownerIndex;
  uint32_t modifierModeMask;
  GameEntityRuntime *entry;
  bool testResult;
  bool classifySelectedState;
  ModelRuntimeScaleRatioRegisterPairQ12 scaleRatio;

  classifySelectedState = false;
  g_InGameCommandPreviewSurfaceHeightQ12OrSentinel = WORLD_POINTER_NO_HIT;
  (inGameRuntime->selection).selectedEntity = NULL;
  if (((inGameRuntime->interaction).interactionFlags48 & 8) != 0) {
    return GRAPHICS_CURSOR_FRAME_ARROW;
  }
  if ((g_UiCommandRuntimeFlags & UI_COMMAND_RUNTIME_FLAG_WORLD_INPUT_DISABLED) != 0) {
    return GRAPHICS_CURSOR_FRAME_ARROW;
  }
  ownerIndex = inGameRuntime->activeFactionRuntimeIndex;
  if ((inGameRuntime->runtimeFlags & 0x10) != 0) {
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
         pendingSelectionEntityOffset8098;
    cursorOrVariantMask = WORLD_CURSOR_PLACEMENT_VALID;
    testResult = ArmyPlacement_ValidateAssetAtPointAndCellCorners
                      (0,g_InGamePlacementHeading16,pointerWorldXQ12,pointerWorldYQ12,
                       g_UiHoverSelectionRecord->armyAssetId,
                       inGameRuntime->activeFactionRuntimeIndex,inGameRuntime);
    if (!testResult) {
      cursorOrVariantMask = WORLD_CURSOR_PLACEMENT_BLOCKED;
    }
    InGameSelectionDetailPanel_Rebuild();
    return cursorOrVariantMask;
  }
  testResult = SelectionInfo_ValidateOwnerType16AndAnyActive(ownerIndex);
  if (!testResult) {
    /* command mode: an own army under the pointer (candidate height at most 1.0 in Q12 above the picked
       height) is selected on click; otherwise the modifier keys pick the pointer-mode command to preview */
    g_InGamePointerInteractionStateFlags = g_InGamePointerInteractionStateFlags & ~WORLD_POINTER_STATE_OVER_OWN_ARMY;
    if ((((candidateNode != NULL) &&
         (candidateNode->ownerClassId == WORLD_OWNER_RUNTIME_MODEL)) &&
        ((int)(candidateHeightQ12 - 0x1000) <= (int)pickedHeightQ12)) &&
       (testResult = GameFactionRuntime_TestCapabilityBitClear
                          (((ModelRuntimeSlot *)candidateNode->runtimePayload)->ownerArmyRuntimeOrSavedOffset.armyRuntime->
                           factionIndex,
                           ownerIndex), !testResult)) {
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
    cursorOrVariantMask = SelectionInfo_CollectAttachmentEffectVariantMask();
    g_InGameCommandPreviewWorldYQ12 = pointerWorldYQ12;
    g_InGameCommandPreviewWorldXQ12 = pointerWorldXQ12;
    g_InGameCommandPreviewSurfaceHeightQ12OrSentinel = pickedHeightQ12;
    g_InGameCommandPreviewArmyAssetId = g_InGamePointerModePreviewArmyIds[modifierModeMask & cursorOrVariantMask];
    return g_InGamePointerModeCommandIds[modifierModeMask & cursorOrVariantMask];
  }
  if ((inGameRuntime->runtimeFlags & WORLD_RUNTIME_FLAG_DRAG_SELECTING) != 0) {
    return GRAPHICS_CURSOR_FRAME_ARROW;
  }
  /* selection mode: only an owned army at most 1.0 (Q12) above the picked height counts as candidate */
  entry = (GameEntityRuntime *)inGameRuntime;
  if (((candidateNode == NULL) ||
      (candidateNode->ownerClassId != WORLD_OWNER_RUNTIME_MODEL)) ||
     ((entry = *(GameEntityRuntime **)((int)candidateNode->runtimePayload + 8),
      (int)pickedHeightQ12 < (int)(candidateHeightQ12 - 0x1000) ||
      ((entry->common).ownership.ownerIndex == 0)))) {
    candidateNode = NULL;
  }
  if (candidateNode != NULL) {
    (inGameRuntime->selection).selectedEntity = entry;
  }
  testResult = SelectionInfo_HasAnyEntry();
  if ((!testResult) || (SelectionInfo_AllEntriesEmptyOrMatchOwner(ownerIndex))) {
    if (candidateNode == NULL) {
      return GRAPHICS_CURSOR_FRAME_ARROW;
    }
    return (ownerIndex != (entry->common).ownership.ownerIndex) ? WORLD_CURSOR_FOREIGN_ARMY : WORLD_CURSOR_OWN_ARMY;
  }
  if ((g_KeyboardStateMask & KEYBOARD_STATE_CTRL) == 0) {
    if (candidateNode == NULL) {
      testResult = SelectionInfo_TestAnyActiveOrSingleClass13();
      if (testResult) {
        return GRAPHICS_CURSOR_FRAME_ARROW;
      }
      testResult = SelectionInfo_TestPositionCommandAtWorldPoint
                        (pointerWorldXQ12,pointerWorldYQ12,inGameRuntime);
      if (testResult) {
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
    testResult = SelectionInfo_TestAnyStateField100Nonnegative();
    if (testResult) {
      testResult = GameFactionRuntime_TestCapabilityBitClear
                        ((entry->common).ownership.ownerIndex,ownerIndex);
      if (!testResult) {
        return (ownerIndex != (entry->common).ownership.ownerIndex) ? WORLD_CURSOR_FOREIGN_ARMY : WORLD_CURSOR_OWN_ARMY;
      }
      classifySelectedState = true;
    }
  }
  else {
    if ((g_KeyboardStateMask & (KEYBOARD_STATE_SHIFT | KEYBOARD_STATE_ALT)) != 0) {
      if (candidateNode == NULL) {
        return GRAPHICS_CURSOR_FRAME_ARROW;
      }
      return (ownerIndex == (entry->common).ownership.ownerIndex) ? WORLD_CURSOR_OWN_ARMY : WORLD_CURSOR_FOREIGN_ARMY;
    }
    classifySelectedState = SelectionInfo_TestAnyStateField100Nonnegative();
  }
  if (classifySelectedState) {
    testResult = SelectionInfo_TestAllStateField100Nonpositive();
    if (testResult) {
      return GRAPHICS_CURSOR_FRAME_ARROW;
    }
    if (candidateNode == NULL) {
      return 0x19;
    }
    testResult = GameFactionRuntime_TestCapabilityBitClear
                      ((entry->common).ownership.ownerIndex,ownerIndex);
    if (testResult) {
      return 0x19;
    }
    testResult = SelectionInfo_FindEntry(entry);
    if (testResult) {
      return 0x1a;
    }
    return GRAPHICS_CURSOR_FRAME_ARROW;
  }
  if (candidateNode == NULL) {
    return GRAPHICS_CURSOR_FRAME_ARROW;
  }
  testResult = GameFactionRuntime_TestCapabilityBitClear
                    ((entry->common).ownership.ownerIndex,ownerIndex);
  if (testResult) {
    return 0x1a;
  }
  scaleRatio = ModelRuntime_QueryHierarchyScaleRatioQ12Regs((RuntimeModelFactionPrefix10 *)entry);
  if ((int)scaleRatio != (int)(scaleRatio >> 32)) {
    testResult = SelectionInfo_FindEntry(entry);
    if (testResult) {
      return 0x19;
    }
    return GRAPHICS_CURSOR_FRAME_ARROW;
  }
  return (ownerIndex != (entry->common).ownership.ownerIndex) ? WORLD_CURSOR_FOREIGN_ARMY : WORLD_CURSOR_OWN_ARMY;
}


/* Address: 0x00568CB0.
   Pointer-press callback of the world view: restores a saved camera, remembers the press position for the
   placement or command-mode heading drag, marks a selection-mode capture, or in command mode selects an own
   army under the pointer right away. The release is handled by InGameWorldInput_CommitPointerAction.
*/
void InGameWorldInput_BeginPointerCapture
          (InGamePointerCallbackValue0 pickedHeightQ12,uint32_t pointerWorldXQ12,uint32_t pointerWorldYQ12,
          InGamePointerCallbackValue3 candidateHeightQ12,WorldOwnerListNode100 *candidateNode,
          WorldRuntimeContext *inGameRuntime)

{
  FactionRuntimeIndex ownerIndex;
  int payloadEntityAddress;
  CommandPayloadDword04 modelToken;
  bool testResult;
  
  if (((g_UiCommandRuntimeFlags & (UI_COMMAND_RUNTIME_FLAG_PAUSED | UI_COMMAND_RUNTIME_FLAG_WORLD_INPUT_DISABLED)) == 0) &&
     (inGameRuntime->runtimeFlags = inGameRuntime->runtimeFlags & 0xf7ffffff,
     ((inGameRuntime->interaction).interactionFlags48 & 8) == 0)) {
    if (((inGameRuntime->interaction).interactionFlags48 & 0x80) != 0) {
      /* makes the release replace the selection instead of selecting a single army */
      inGameRuntime->runtimeFlags = inGameRuntime->runtimeFlags | 0x8000000;
    }
    ownerIndex = inGameRuntime->activeFactionRuntimeIndex;
    if ((inGameRuntime->runtimeFlags & 0x10) == 0) {
      if ((g_UiCommandRuntimeFlags & UI_COMMAND_RUNTIME_FLAG_PLACEMENT_PENDING) == 0) {
        testResult = SelectionInfo_ValidateOwnerType16AndAnyActive(ownerIndex);
        if (testResult) {
          g_InGamePointerInteractionStateFlags = g_InGamePointerInteractionStateFlags | WORLD_POINTER_STATE_SELECTION_CAPTURE;
        }
        else {
          if ((((candidateNode != NULL) &&
               (candidateNode->ownerClassId == WORLD_OWNER_RUNTIME_MODEL)) &&
              (payloadEntityAddress =
                    (int)((ModelRuntimeSlot *)candidateNode->runtimePayload)->ownerArmyRuntimeOrSavedOffset.armyRuntime,
              (int)(candidateHeightQ12 - 0x1000) <= (int)pickedHeightQ12)) &&
             (testResult = GameFactionRuntime_TestCapabilityBitClear
                                (((ArmyRuntimeSlot *)payloadEntityAddress)->factionIndex,ownerIndex), !testResult)) {
            modelToken = payloadEntityAddress - (int)g_ArmyRuntimeRebaseBaseMinusOne;
            if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_NETWORKED_MASK) !=
                SESSION_NETWORK_ROLE_LOCAL) {
              InGameCommandQueue_AppendLocalPlayerCommand(INGAME_COMMAND_SELECT_SINGLE_ARMY,0,0,modelToken);
              return;
            }
            FrontendPlayerRuntime_AssignModelTokenAndRefreshSelection
                      (g_LocalPlayerRuntimeId,0,0,modelToken);
            return;
          }
          g_InGameCommandPointerCaptureX =
               THANDOR_CONTAINER_OF(inGameRuntime, InGameRuntimeRootImageC3E4, worldRuntime0A30)->pointerPressX0B90;
          g_InGameCommandPointerCaptureY =
               THANDOR_CONTAINER_OF(inGameRuntime, InGameRuntimeRootImageC3E4, worldRuntime0A30)->pointerPressY0B94;
          g_UiCommandRuntimeFlags = g_UiCommandRuntimeFlags | UI_COMMAND_RUNTIME_FLAG_COMMAND_POINTER_CAPTURED;
        }
      }
      else {
        g_InGamePlacementPointerCaptureX =
             THANDOR_CONTAINER_OF(inGameRuntime, InGameRuntimeRootImageC3E4, worldRuntime0A30)->pointerPressX0B90;
        g_InGamePlacementPointerCaptureY =
             THANDOR_CONTAINER_OF(inGameRuntime, InGameRuntimeRootImageC3E4, worldRuntime0A30)->pointerPressY0B94;
      }
    }
    else {
      inGameRuntime->runtimeFlags = inGameRuntime->runtimeFlags & 0xffffffef;
      WorldRuntime_RestoreMotionStateFromSnapshot(inGameRuntime);
    }
  }
  return;
}


/* Address: 0x00568E10.
   Pointer-move callback while the pointer is captured. In selection mode a press that moved more than 23 pixels
   becomes a drag selection: every own army inside the rectangle is inserted, every one outside removed (in
   batches of three per command, skipping armies already queued). In placement and command mode, horizontal
   travel rotates the placement/command heading and the pointer is snapped back to the press position.
*/
void InGameWorldInput_UpdateDragSelectionAndCamera
          (InGamePointerCallbackValue0 pickedHeightQ12,uint32_t pointerWorldXQ12,uint32_t pointerWorldYQ12,
          uint32_t candidateHeightQ12,WorldOwnerListNode100 *candidateNode,
          WorldRuntimeContext *inGameRuntime)

{
  uint32_t deltaXOrTripletCount;
  int countOrOwnerOrDelta;
  uint32_t deltaY;
  InGameCommandPayloadTripletValue32 payloadValue;
  uint32_t *clearCursor;
  WorldOwnerListNode100 *runtimeNode;
  CommandPayloadDword04 *tripletCursor;
  bool testResult;
  GameEntityRuntime *entry;
  
  if (((((g_UiCommandRuntimeFlags & (UI_COMMAND_RUNTIME_FLAG_PAUSED | UI_COMMAND_RUNTIME_FLAG_WORLD_INPUT_DISABLED)) == 0) &&
       (((inGameRuntime->interaction).interactionFlags48 & 8) == 0)) &&
      ((g_UiCommandRuntimeFlags & UI_COMMAND_RUNTIME_FLAG_WORLD_INPUT_DISABLED) == 0)) &&
     ((inGameRuntime->runtimeFlags & 0x10) == 0)) {
    if ((g_UiCommandRuntimeFlags & UI_COMMAND_RUNTIME_FLAG_PLACEMENT_PENDING) == 0) {
      testResult = SelectionInfo_ValidateOwnerType16AndAnyActive
                        (inGameRuntime->activeFactionRuntimeIndex);
      if (testResult) {
        if ((inGameRuntime->runtimeFlags & WORLD_RUNTIME_FLAG_DRAG_SELECTING) == 0) {
          /* pointer travel since the press (the pointer positions are kept in the in-game root) */
          deltaXOrTripletCount =
               THANDOR_CONTAINER_OF(inGameRuntime, InGameRuntimeRootImageC3E4, worldRuntime0A30)->pointerPressX0B90 -
               THANDOR_CONTAINER_OF(inGameRuntime, InGameRuntimeRootImageC3E4, worldRuntime0A30)->pointerX0B98;
          if ((int)deltaXOrTripletCount < 0) {
            deltaXOrTripletCount = -deltaXOrTripletCount;
          }
          deltaY = THANDOR_CONTAINER_OF(inGameRuntime, InGameRuntimeRootImageC3E4, worldRuntime0A30)->pointerPressY0B94 -
                  THANDOR_CONTAINER_OF(inGameRuntime, InGameRuntimeRootImageC3E4, worldRuntime0A30)->pointerY0B9C;
          if ((int)deltaY < 0) {
            deltaY = -deltaY;
          }
          if ((WORLD_DRAG_SELECTION_THRESHOLD < deltaXOrTripletCount) ||
              (WORLD_DRAG_SELECTION_THRESHOLD < deltaY)) {
            inGameRuntime->runtimeFlags = inGameRuntime->runtimeFlags | WORLD_RUNTIME_FLAG_DRAG_SELECTING;
            if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_NETWORKED_MASK) ==
                SESSION_NETWORK_ROLE_LOCAL) {
              FrontendPlayerSelection_ClearAndRefreshLocalPanels(g_LocalPlayerRuntimeId,0,0,0);
            }
            else {
              InGameCommandQueue_AppendLocalPlayerCommand(INGAME_COMMAND_SELECTION_CLEAR,0,0,0);
            }
          }
        }
        else {
          /* clears both 12-dword batches and their two counters (0x1A dwords from 0x0055F0C4) */
          clearCursor = (uint32_t *)&g_InGameSelectionInsertTripletDwords;
          for (countOrOwnerOrDelta = 0x1a; countOrOwnerOrDelta != 0; countOrOwnerOrDelta--) {
            *clearCursor = 0;
            clearCursor = clearCursor + 1;
          }
          runtimeNode = inGameRuntime->ownerListHead;
          countOrOwnerOrDelta = inGameRuntime->activeFactionRuntimeIndex;
          if (runtimeNode != NULL) {
            do {
              if (((runtimeNode->ownerClassId == WORLD_OWNER_RUNTIME_MODEL) &&
                  ((runtimeNode->runtimeFlags & 2) != 0)) &&
                 ((entry = *(GameEntityRuntime **)((int)runtimeNode->runtimePayload + 8),
                  (runtimeNode->runtimeFlags & 0x20) != 0 &&
                  (countOrOwnerOrDelta == (entry->common).ownership.ownerIndex)))) {
                payloadValue = (int)entry - (int)g_ArmyRuntimeRebaseBaseMinusOne;
                testResult = WorldRuntimeNode_IsPositionInsideBounds
                                  (runtimeNode,
                                   (WorldRuntimeExtendedMapControlView170 *)inGameRuntime);
                if (testResult) {
                  testResult = SelectionInfo_FindEntry(entry);
                  deltaXOrTripletCount = g_InGameSelectionInsertTripletDwordCount;
                  if (testResult) {
                    testResult = InGameCommandQueue_ContainsTripletValue
                                      (payloadValue,INGAME_COMMAND_CODE_BASE + INGAME_COMMAND_SELECTION_INSERT);
                    if ((!testResult) &&
                       (*(InGameCommandPayloadTripletValue32 *)
                         (&g_InGameSelectionInsertTripletDwords + deltaXOrTripletCount * 4) = payloadValue,
                       deltaXOrTripletCount < 0xb)) {
                      g_InGameSelectionInsertTripletDwordCount =
                           g_InGameSelectionInsertTripletDwordCount + 1;
                    }
                  }
                }
                else {
                  testResult = SelectionInfo_FindEntry(entry);
                  deltaXOrTripletCount = g_InGameSelectionRemoveTripletDwordCount;
                  if (!testResult) {
                    testResult = InGameCommandQueue_ContainsTripletValue
                                      (payloadValue,INGAME_COMMAND_CODE_BASE + INGAME_COMMAND_SELECTION_REMOVE);
                    if ((!testResult) &&
                       (*(InGameCommandPayloadTripletValue32 *)
                         (&g_InGameSelectionRemoveTripletDwords + deltaXOrTripletCount * 4) = payloadValue,
                       deltaXOrTripletCount < 0xb)) {
                      g_InGameSelectionRemoveTripletDwordCount =
                           g_InGameSelectionRemoveTripletDwordCount + 1;
                    }
                  }
                }
              }
              runtimeNode = runtimeNode->nextNode;
            } while (runtimeNode != NULL);
            if (g_InGameSelectionRemoveTripletDwordCount != 0) {
              tripletCursor = (CommandPayloadDword04 *)&g_InGameSelectionRemoveTripletDwords;
              do {
                if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_NETWORKED_MASK) ==
                    SESSION_NETWORK_ROLE_LOCAL) {
                  FrontendPlayerSelection_RemoveThreeEntriesAndRefresh
                            (g_LocalPlayerRuntimeId,tripletCursor[2],tripletCursor[1],*tripletCursor);
                }
                else {
                  InGameCommandQueue_AppendLocalPlayerCommand
                            (INGAME_COMMAND_SELECTION_REMOVE,tripletCursor[2],tripletCursor[1],*tripletCursor);
                }
                deltaXOrTripletCount = g_InGameSelectionRemoveTripletDwordCount;
                tripletCursor = tripletCursor + 3;
                g_InGameSelectionRemoveTripletDwordCount =
                     g_InGameSelectionRemoveTripletDwordCount - 3;
              } while (g_InGameSelectionRemoveTripletDwordCount != 0 && 2 < (int)deltaXOrTripletCount);
            }
            if (g_InGameSelectionInsertTripletDwordCount != 0) {
              tripletCursor = (CommandPayloadDword04 *)&g_InGameSelectionInsertTripletDwords;
              do {
                if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_NETWORKED_MASK) ==
                    SESSION_NETWORK_ROLE_LOCAL) {
                  FrontendPlayerSelection_InsertThreeEntriesAndRefresh
                            (g_LocalPlayerRuntimeId,tripletCursor[2],tripletCursor[1],*tripletCursor);
                }
                else {
                  InGameCommandQueue_AppendLocalPlayerCommand
                            (INGAME_COMMAND_SELECTION_INSERT,tripletCursor[2],tripletCursor[1],*tripletCursor);
                }
                deltaXOrTripletCount = g_InGameSelectionInsertTripletDwordCount;
                tripletCursor = tripletCursor + 3;
                g_InGameSelectionInsertTripletDwordCount =
                     g_InGameSelectionInsertTripletDwordCount - 3;
              } while (g_InGameSelectionInsertTripletDwordCount != 0 && 2 < (int)deltaXOrTripletCount);
            }
          }
        }
      }
      else if ((pickedHeightQ12 != WORLD_POINTER_NO_HIT) &&
               ((g_InGamePointerInteractionStateFlags &
                 (WORLD_POINTER_STATE_OVER_OWN_ARMY | WORLD_POINTER_STATE_SELECTION_CAPTURE)) == 0)) {
        /* one pixel of horizontal travel turns the heading by 0x40 of the 0x10000 full circle */
        if ((g_CursorButtonState & 4) == 0) {
          /* The original adds the horizontal mouse delta since capture (computed before snapping the
             pointer back) - not the pointer function's return value. */
          countOrOwnerOrDelta =
               THANDOR_CONTAINER_OF(inGameRuntime, InGameRuntimeRootImageC3E4, worldRuntime0A30)->pointerX0B98 - g_InGameCommandPointerCaptureX;
          g_PointerSetPosition(g_InGameCommandPointerCaptureY,g_InGameCommandPointerCaptureX);
          g_InGameCommandPreviewHeading16 = g_InGameCommandPreviewHeading16 + countOrOwnerOrDelta * 0x40;
          g_InGameCommandPreviewHeading16 = g_InGameCommandPreviewHeading16 & 0xffff;
        }
        else {
          g_InGameCommandPointerCaptureX =
               THANDOR_CONTAINER_OF(inGameRuntime, InGameRuntimeRootImageC3E4, worldRuntime0A30)->pointerX0B98;
          g_InGameCommandPointerCaptureY =
               THANDOR_CONTAINER_OF(inGameRuntime, InGameRuntimeRootImageC3E4, worldRuntime0A30)->pointerY0B9C;
        }
      }
    }
    else if (pickedHeightQ12 != WORLD_POINTER_NO_HIT) {
      if ((g_CursorButtonState & 4) == 0) {
        /* The original adds the horizontal mouse delta since capture (computed before snapping the
           pointer back) - not the pointer function's return value. */
        countOrOwnerOrDelta =
             THANDOR_CONTAINER_OF(inGameRuntime, InGameRuntimeRootImageC3E4, worldRuntime0A30)->pointerX0B98 - g_InGamePlacementPointerCaptureX;
        g_PointerSetPosition(g_InGamePlacementPointerCaptureY,g_InGamePlacementPointerCaptureX);
        g_InGamePlacementHeading16 = g_InGamePlacementHeading16 + countOrOwnerOrDelta * 0x40;
        g_InGamePlacementHeading16 = g_InGamePlacementHeading16 & 0xffff;
      }
      else {
        g_InGamePlacementPointerCaptureX =
             THANDOR_CONTAINER_OF(inGameRuntime, InGameRuntimeRootImageC3E4, worldRuntime0A30)->pointerX0B98;
        g_InGamePlacementPointerCaptureY =
             THANDOR_CONTAINER_OF(inGameRuntime, InGameRuntimeRootImageC3E4, worldRuntime0A30)->pointerY0B9C;
      }
    }
  }
  return;
}


/* Address: 0x005691B0.
   Pointer-release callback of the world view: places the pending army, issues the command-mode command chosen by
   the modifier keys, ends a drag selection, or (selection mode) turns the click into select / add / remove,
   move, target-position or target-army commands. Every action goes through the command queue in network games
   and calls the handler directly in single player. Always ends the selection-mode capture.
*/
void InGameWorldInput_CommitPointerAction
          (InGamePointerCallbackValue0 pickedHeightQ12,InGamePointerCallbackValue1 pointerWorldXQ12,
          InGamePointerCallbackValue2 pointerWorldYQ12,InGamePointerCallbackValue3 candidateHeightQ12,
          WorldOwnerListNode100 *candidateNode,WorldRuntimeContext *inGameRuntime)

{
  InGamePointerModeHandler *modeHandler;
  uint32_t variantMaskOrSurfaceHeight;
  CommandPayloadDword04 payloadDword0C;
  uint32_t modifierModeMask;
  int ownerIndex;
  CommandPayloadDword08 payloadDword08;
  GameEntityRuntime *entry;
  CommandPayloadDword04 armyRuntimeIndex;
  bool testResult;
  ModelRuntimeScaleRatioRegisterPairQ12 scaleRatio;
  
  if (((((g_UiCommandRuntimeFlags & (UI_COMMAND_RUNTIME_FLAG_PAUSED | UI_COMMAND_RUNTIME_FLAG_WORLD_INPUT_DISABLED)) != 0) ||
       (((inGameRuntime->interaction).interactionFlags48 & 8) != 0)) ||
      ((g_UiCommandRuntimeFlags & UI_COMMAND_RUNTIME_FLAG_WORLD_INPUT_DISABLED) != 0)) ||
     ((inGameRuntime->runtimeFlags & 0x10) != 0))
  goto InGameWorldInput_ReleasePointerCapture;
  if ((g_UiCommandRuntimeFlags & UI_COMMAND_RUNTIME_FLAG_PLACEMENT_PENDING) != 0) {
    if (pickedHeightQ12 != WORLD_POINTER_NO_HIT) {
      if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_NETWORKED_MASK) ==
          SESSION_NETWORK_ROLE_LOCAL) {
        InGameCommand_ExecuteLocalPlacementFromSelection
                  (g_LocalPlayerRuntimeId,g_InGamePlacementHeading16,pointerWorldXQ12,pointerWorldYQ12);
      }
      else {
        InGameCommandQueue_AppendLocalPlayerCommand
                  (INGAME_COMMAND_PLACE_ARMY,g_InGamePlacementHeading16,pointerWorldXQ12,pointerWorldYQ12);
      }
    }
    goto InGameWorldInput_ReleasePointerCapture;
  }
  testResult = SelectionInfo_ValidateOwnerType16AndAnyActive(inGameRuntime->activeFactionRuntimeIndex);
  if (!testResult) {
    if (((pickedHeightQ12 != WORLD_POINTER_NO_HIT) &&
        ((g_UiCommandRuntimeFlags & UI_COMMAND_RUNTIME_FLAG_COMMAND_POINTER_CAPTURED) != 0)) &&
       ((g_InGamePointerInteractionStateFlags &
         (WORLD_POINTER_STATE_OVER_OWN_ARMY | WORLD_POINTER_STATE_SELECTION_CAPTURE)) == 0)) {
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
      variantMaskOrSurfaceHeight = SelectionInfo_CollectAttachmentEffectVariantMask();
      if ((modifierModeMask & variantMaskOrSurfaceHeight) != 0) {
        modeHandler = (InGamePointerModeHandler *)
                      g_InGamePointerModeHandlers[modifierModeMask & variantMaskOrSurfaceHeight];
        if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_NETWORKED_MASK) ==
            SESSION_NETWORK_ROLE_LOCAL) {
          /* The original pushes the same four arguments as the networked command below, with the local player
             id in place of the command id. */
          modeHandler(g_LocalPlayerRuntimeId,g_InGameCommandPreviewHeading16,pointerWorldXQ12,pointerWorldYQ12);
        }
        else {
          /* the command code is the handler's original address minus INGAME_COMMAND_CODE_BASE; the table
             holds recovered C functions, so map back to the original address first */
          InGameCommandQueue_AppendLocalPlayerCommand
                    ((UiActionId)(Thandor_OriginalAddressOfFunction((const void *)modeHandler) -
                                  INGAME_COMMAND_CODE_BASE),
                     g_InGameCommandPreviewHeading16,pointerWorldXQ12,pointerWorldYQ12);
        }
        if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_NETWORKED_MASK) ==
            SESSION_NETWORK_ROLE_LOCAL) {
          FrontendPlayerSelection_ClearAndRefreshLocalPanels(g_LocalPlayerRuntimeId,0,0,0);
        }
        else {
          InGameCommandQueue_AppendLocalPlayerCommand(INGAME_COMMAND_SELECTION_CLEAR,0,0,0);
        }
      }
    }
    goto InGameWorldInput_ReleasePointerCapture;
  }
  if ((inGameRuntime->runtimeFlags & WORLD_RUNTIME_FLAG_DRAG_SELECTING) != 0) {
    inGameRuntime->runtimeFlags = inGameRuntime->runtimeFlags & ~WORLD_RUNTIME_FLAG_DRAG_SELECTING;
    goto InGameWorldInput_ReleasePointerCapture;
  }
  /* same candidate filter as InGameWorldInput_ResolveContextActionAndCursor */
  ownerIndex = inGameRuntime->activeFactionRuntimeIndex;
  entry = (GameEntityRuntime *)inGameRuntime;
  if (((candidateNode == NULL) ||
      (candidateNode->ownerClassId != WORLD_OWNER_RUNTIME_MODEL)) ||
     ((entry = *(GameEntityRuntime **)((int)candidateNode->runtimePayload + 8),
      (int)pickedHeightQ12 < (int)(candidateHeightQ12 - 0x1000) ||
      ((entry->common).ownership.ownerIndex == 0)))) {
    candidateNode = NULL;
  }
  testResult = SelectionInfo_HasAnyEntry();
  if ((!testResult) || (testResult = SelectionInfo_AllEntriesEmptyOrMatchOwner(ownerIndex), testResult)) {
    if (candidateNode != NULL) {
      if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_NETWORKED_MASK) ==
          SESSION_NETWORK_ROLE_LOCAL) {
        FrontendPlayerRuntime_AssignModelTokenAndRefreshSelection
                  (g_LocalPlayerRuntimeId,0,0,(int)entry - (int)g_ArmyRuntimeRebaseBaseMinusOne);
      }
      else {
        InGameCommandQueue_AppendLocalPlayerCommand
                  (INGAME_COMMAND_SELECT_SINGLE_ARMY,0,0,(int)entry - (int)g_ArmyRuntimeRebaseBaseMinusOne);
      }
    }
    goto InGameWorldInput_ReleasePointerCapture;
  }
  if ((g_KeyboardStateMask & KEYBOARD_STATE_CTRL) == 0) {
    if (candidateNode == NULL) {
      testResult = SelectionInfo_TestAnyActiveOrSingleClass13();
      if ((!testResult) && (pickedHeightQ12 != WORLD_POINTER_NO_HIT)) {
        if ((g_KeyboardStateMask & (KEYBOARD_STATE_SHIFT | KEYBOARD_STATE_ALT)) == 0) {
          if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_NETWORKED_MASK) ==
              SESSION_NETWORK_ROLE_LOCAL) {
            InGamePlayerSelection_ApplyPositionCommandVariantB
                      (g_LocalPlayerRuntimeId,0,pointerWorldXQ12,pointerWorldYQ12);
          }
          else {
            InGameCommandQueue_AppendLocalPlayerCommand(INGAME_COMMAND_POSITION_VARIANT_B,0,pointerWorldXQ12,pointerWorldYQ12);
          }
        }
        else if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_NETWORKED_MASK) ==
                 SESSION_NETWORK_ROLE_LOCAL) {
          InGamePlayerSelection_ApplyPositionCommand
                    (g_LocalPlayerRuntimeId,0,pointerWorldXQ12,pointerWorldYQ12);
        }
        else {
          InGameCommandQueue_AppendLocalPlayerCommand(INGAME_COMMAND_POSITION,0,pointerWorldXQ12,pointerWorldYQ12);
        }
      }
      goto InGameWorldInput_ReleasePointerCapture;
    }
    if ((g_KeyboardStateMask & (KEYBOARD_STATE_SHIFT | KEYBOARD_STATE_ALT)) == 0) {
      testResult = SelectionInfo_TestAnyStateField100Nonnegative();
      if (testResult) {
        testResult = GameFactionRuntime_TestCapabilityBitClear
                          ((entry->common).ownership.ownerIndex,ownerIndex);
        if (testResult) goto InGameWorldInput_SelectCandidateArmy;
      }
      else {
InGameWorldInput_TestCandidateCapability:
        testResult = GameFactionRuntime_TestCapabilityBitClear
                          ((entry->common).ownership.ownerIndex,ownerIndex);
        if (testResult) goto InGameWorldInput_ReleasePointerCapture;
        scaleRatio = ModelRuntime_QueryHierarchyScaleRatioQ12Regs((RuntimeModelFactionPrefix10 *)entry);
        if ((int)scaleRatio != (int)(scaleRatio >> 32)) goto InGameWorldInput_SelectCandidateArmy;
      }
      if (ownerIndex == (entry->common).ownership.ownerIndex) {
        armyRuntimeIndex = (int)entry - (int)g_ArmyRuntimeRebaseBaseMinusOne;
        if ((inGameRuntime->runtimeFlags & 0x8000000) == 0) { /* set on press, see BeginPointerCapture */
          if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_NETWORKED_MASK) ==
              SESSION_NETWORK_ROLE_LOCAL) {
            FrontendPlayerRuntime_AssignModelTokenAndRefreshSelection
                      (g_LocalPlayerRuntimeId,0,0,armyRuntimeIndex);
          }
          else {
            InGameCommandQueue_AppendLocalPlayerCommand(INGAME_COMMAND_SELECT_SINGLE_ARMY,0,0,armyRuntimeIndex);
          }
        }
        else if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_NETWORKED_MASK) ==
                 SESSION_NETWORK_ROLE_LOCAL) {
          InGamePlayerSelection_ReplaceWithArmyRuntimeIndex
                    (g_LocalPlayerRuntimeId,0,0,armyRuntimeIndex);
        }
        else {
          InGameCommandQueue_AppendLocalPlayerCommand(INGAME_COMMAND_REPLACE_SELECTION,0,0,armyRuntimeIndex);
        }
      }
      goto InGameWorldInput_ReleasePointerCapture;
    }
  }
  else if ((g_KeyboardStateMask & (KEYBOARD_STATE_SHIFT | KEYBOARD_STATE_ALT)) == 0) {
    if (candidateNode == NULL) {
      if (pickedHeightQ12 != WORLD_POINTER_NO_HIT) {
        variantMaskOrSurfaceHeight = WorldRuntime_InterpolateTopSurfaceHeightOrSentinel
                          (pointerWorldXQ12,pointerWorldYQ12,inGameRuntime);
        /* The original passes the same EDX/ECX point on (lost locals in the decompilation). */
        payloadDword08 = pointerWorldXQ12;
        payloadDword0C = pointerWorldYQ12;
        if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_NETWORKED_MASK) ==
            SESSION_NETWORK_ROLE_LOCAL) {
          InGamePlayerSelection_ApplyTargetPositionCommand
                    (g_LocalPlayerRuntimeId,variantMaskOrSurfaceHeight,payloadDword08,payloadDword0C);
        }
        else {
          InGameCommandQueue_AppendLocalPlayerCommand(INGAME_COMMAND_TARGET_POSITION,variantMaskOrSurfaceHeight,payloadDword08,payloadDword0C);
        }
      }
      goto InGameWorldInput_ReleasePointerCapture;
    }
    testResult = SelectionInfo_TestAnyStateField100Nonnegative();
    if (!testResult) goto InGameWorldInput_TestCandidateCapability;
    testResult = GameFactionRuntime_TestCapabilityBitClear
                      ((entry->common).ownership.ownerIndex,ownerIndex);
    if ((!testResult) && (testResult = SelectionInfo_FindEntry(entry), !testResult))
    goto InGameWorldInput_ReleasePointerCapture;
InGameWorldInput_SelectCandidateArmy:
    if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_NETWORKED_MASK) ==
        SESSION_NETWORK_ROLE_LOCAL) {
      InGamePlayerSelection_SelectArmyRuntimeIndex
                (g_LocalPlayerRuntimeId,0,0,(int)entry - (int)g_ArmyRuntimeRebaseBaseMinusOne);
    }
    else {
      InGameCommandQueue_AppendLocalPlayerCommand
                (INGAME_COMMAND_SELECT_ARMY,0,0,(int)entry - (int)g_ArmyRuntimeRebaseBaseMinusOne);
    }
    goto InGameWorldInput_ReleasePointerCapture;
  }
  /* Shift/Alt-click (with or without Ctrl): toggle an own army in the selection (SelectionInfo_FindEntry is true when the entry
     is absent), select a foreign one alone */
  if (candidateNode != NULL) {
    if (ownerIndex == (entry->common).ownership.ownerIndex) {
      testResult = SelectionInfo_FindEntry(entry);
      if (testResult) {
        if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_NETWORKED_MASK) ==
            SESSION_NETWORK_ROLE_LOCAL) {
          FrontendPlayerSelection_InsertThreeEntriesAndRefresh
                    (g_LocalPlayerRuntimeId,0,0,(int)entry - (int)g_ArmyRuntimeRebaseBaseMinusOne);
        }
        else {
          InGameCommandQueue_AppendLocalPlayerCommand
                    (INGAME_COMMAND_SELECTION_INSERT,0,0,(int)entry - (int)g_ArmyRuntimeRebaseBaseMinusOne);
        }
      }
      else if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_NETWORKED_MASK) ==
               SESSION_NETWORK_ROLE_LOCAL) {
        FrontendPlayerSelection_RemoveThreeEntriesAndRefresh
                  (g_LocalPlayerRuntimeId,0,0,(int)entry - (int)g_ArmyRuntimeRebaseBaseMinusOne);
      }
      else {
        InGameCommandQueue_AppendLocalPlayerCommand
                  (INGAME_COMMAND_SELECTION_REMOVE,0,0,(int)entry - (int)g_ArmyRuntimeRebaseBaseMinusOne);
      }
    }
    else if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_NETWORKED_MASK) ==
             SESSION_NETWORK_ROLE_LOCAL) {
      FrontendPlayerRuntime_AssignModelTokenAndRefreshSelection
                (g_LocalPlayerRuntimeId,0,0,(int)entry - (int)g_ArmyRuntimeRebaseBaseMinusOne);
    }
    else {
      InGameCommandQueue_AppendLocalPlayerCommand
                (INGAME_COMMAND_SELECT_SINGLE_ARMY,0,0,(int)entry - (int)g_ArmyRuntimeRebaseBaseMinusOne);
    }
  }
InGameWorldInput_ReleasePointerCapture:
  g_InGamePointerInteractionStateFlags = g_InGamePointerInteractionStateFlags & ~WORLD_POINTER_STATE_SELECTION_CAPTURE;
  return;
}


/* Address: 0x0056F230.
   Camera key commands of the world view while the interaction subsystem is active (game paused): installed as
   the world view's dispatchCommandCallback by the activating path of
   InGameUiCommandRuntime_ApplyInteractionSubsystemActiveState (0x005609F0), in place of
   InGameUiRuntime_DispatchCommandByCodeAndModifierFlags. The first g_InGameCameraCommandDispatchRecords16 record
   with this key and a matching modifier selects the command:
     1..7      move the camera to level camera bookmark n
     Alt+1..7  store the current camera as bookmark n
     Alt+S     toggle WORLD_RUNTIME_FLAG_SHADING_ENABLED
     Ctrl+C    toggle WORLD_RUNTIME_FLAG_UNLIMITED_CAMERA
   The original returns CF set when no record matches and CF clear after a command.
*/
void InGameCameraCommand_DispatchByCodeAndModifierFlags
          (uint32_t modifierFlags,uint32_t commandCode,WorldRuntimeContext *worldRuntime)

{
  InGameCameraCommandKeyCode recordKeyCode;
  uint32_t requiredModifiers;
  uint32_t bookmark1PackedAngles;
  uint32_t bookmark2PackedAngles;
  uint32_t bookmark3PackedAngles;
  uint32_t bookmark4PackedAngles;
  uint32_t bookmark5PackedAngles;
  uint32_t bookmark6PackedAngles;
  uint32_t bookmark7PackedAngles;
  InGameCameraCommandDispatchTable *currentRecord;
  InGameCameraCommandDispatchTable *nextRecord;
  
  bookmark7PackedAngles = g_LevelCameraBookmark7PackedHeadingLow16PitchHigh16;
  bookmark6PackedAngles = g_LevelCameraBookmark6PackedHeadingLow16PitchHigh16;
  bookmark5PackedAngles = g_LevelCameraBookmark5PackedHeadingLow16PitchHigh16;
  bookmark4PackedAngles = g_LevelCameraBookmark4PackedHeadingLow16PitchHigh16;
  bookmark3PackedAngles = g_LevelCameraBookmark3PackedHeadingLow16PitchHigh16;
  bookmark2PackedAngles = g_LevelCameraBookmark2PackedHeadingLow16PitchHigh16;
  bookmark1PackedAngles = g_LevelCameraBookmark1PackedHeadingLow16PitchHigh16;
  /* First record with this key whose modifier requirement matches: a record without required modifiers only
     matches when neither Ctrl nor Alt is held (Shift is ignored). The key-code 0 record terminates the table. */
  nextRecord = &g_InGameCameraCommandDispatchRecords16;
  while( true ) {
    currentRecord = nextRecord;
    recordKeyCode = currentRecord->records[0].keyCode;
    requiredModifiers = currentRecord->records[0].requiredModifierMask;
    if (recordKeyCode == 0) {
      return;
    }
    nextRecord = (InGameCameraCommandDispatchTable *)(currentRecord->records + 1);
    if (recordKeyCode != commandCode) continue;
    if ((requiredModifiers == 0) ? ((modifierFlags & (KEYBOARD_STATE_CTRL | KEYBOARD_STATE_ALT)) == 0)
                                 : ((modifierFlags & requiredModifiers) != 0))
    break;
  }
  /* The original jumps to the record's continuation address; the cases are those addresses. */
  switch(currentRecord->records[0].continuationEntryAddress) {
  case 0x56f360: /* 1..7: recall bookmark n */
    WorldRuntime_SetPosition60AndDistanceFromPosition80
              (g_LevelCameraBookmark1PositionZQ12,g_LevelCameraBookmark1PositionYQ12,
               g_LevelCameraBookmark1PositionXQ12,worldRuntime);
    WorldRuntime_SetMotionParameters6CThrough78Clamped
              (2,(int)bookmark1PackedAngles >> 16,bookmark1PackedAngles & 0xffff,g_LevelCameraBookmark1PositionMagnitudeQ12,
               worldRuntime);
    WorldRuntime_RecomputeMotionEndpointAgainstFieldSurface(worldRuntime);
    WorldRuntime_CommitScalar7CFrom8C(worldRuntime);
    break;
  case 0x56f3b0:
    WorldRuntime_SetPosition60AndDistanceFromPosition80
              (g_LevelCameraBookmark2PositionZQ12,g_LevelCameraBookmark2PositionYQ12,
               g_LevelCameraBookmark2PositionXQ12,worldRuntime);
    WorldRuntime_SetMotionParameters6CThrough78Clamped
              (2,(int)bookmark2PackedAngles >> 16,bookmark2PackedAngles & 0xffff,g_LevelCameraBookmark2PositionMagnitudeQ12,
               worldRuntime);
    WorldRuntime_RecomputeMotionEndpointAgainstFieldSurface(worldRuntime);
    WorldRuntime_CommitScalar7CFrom8C(worldRuntime);
    break;
  case 0x56f400:
    WorldRuntime_SetPosition60AndDistanceFromPosition80
              (g_LevelCameraBookmark3PositionZQ12,g_LevelCameraBookmark3PositionYQ12,
               g_LevelCameraBookmark3PositionXQ12,worldRuntime);
    WorldRuntime_SetMotionParameters6CThrough78Clamped
              (2,(int)bookmark3PackedAngles >> 16,bookmark3PackedAngles & 0xffff,g_LevelCameraBookmark3PositionMagnitudeQ12,
               worldRuntime);
    WorldRuntime_RecomputeMotionEndpointAgainstFieldSurface(worldRuntime);
    WorldRuntime_CommitScalar7CFrom8C(worldRuntime);
    break;
  case 0x56f450:
    WorldRuntime_SetPosition60AndDistanceFromPosition80
              (g_LevelCameraBookmark4PositionZQ12,g_LevelCameraBookmark4PositionYQ12,
               g_LevelCameraBookmark4PositionXQ12,worldRuntime);
    WorldRuntime_SetMotionParameters6CThrough78Clamped
              (2,(int)bookmark4PackedAngles >> 16,bookmark4PackedAngles & 0xffff,g_LevelCameraBookmark4PositionMagnitudeQ12,
               worldRuntime);
    WorldRuntime_RecomputeMotionEndpointAgainstFieldSurface(worldRuntime);
    WorldRuntime_CommitScalar7CFrom8C(worldRuntime);
    break;
  case 0x56f4a0:
    WorldRuntime_SetPosition60AndDistanceFromPosition80
              (g_LevelCameraBookmark5PositionZQ12,g_LevelCameraBookmark5PositionYQ12,
               g_LevelCameraBookmark5PositionXQ12,worldRuntime);
    WorldRuntime_SetMotionParameters6CThrough78Clamped
              (2,(int)bookmark5PackedAngles >> 16,bookmark5PackedAngles & 0xffff,g_LevelCameraBookmark5PositionMagnitudeQ12,
               worldRuntime);
    WorldRuntime_RecomputeMotionEndpointAgainstFieldSurface(worldRuntime);
    WorldRuntime_CommitScalar7CFrom8C(worldRuntime);
    break;
  case 0x56f4f0:
    WorldRuntime_SetPosition60AndDistanceFromPosition80
              (g_LevelCameraBookmark6PositionZQ12,g_LevelCameraBookmark6PositionYQ12,
               g_LevelCameraBookmark6PositionXQ12,worldRuntime);
    WorldRuntime_SetMotionParameters6CThrough78Clamped
              (2,(int)bookmark6PackedAngles >> 16,bookmark6PackedAngles & 0xffff,g_LevelCameraBookmark6PositionMagnitudeQ12,
               worldRuntime);
    WorldRuntime_RecomputeMotionEndpointAgainstFieldSurface(worldRuntime);
    WorldRuntime_CommitScalar7CFrom8C(worldRuntime);
    break;
  case 0x56f540:
    WorldRuntime_SetPosition60AndDistanceFromPosition80
              (g_LevelCameraBookmark7PositionZQ12,g_LevelCameraBookmark7PositionYQ12,
               g_LevelCameraBookmark7PositionXQ12,worldRuntime);
    WorldRuntime_SetMotionParameters6CThrough78Clamped
              (2,(int)bookmark7PackedAngles >> 16,bookmark7PackedAngles & 0xffff,g_LevelCameraBookmark7PositionMagnitudeQ12,
               worldRuntime);
    WorldRuntime_RecomputeMotionEndpointAgainstFieldSurface(worldRuntime);
    WorldRuntime_CommitScalar7CFrom8C(worldRuntime);
    break;
  case 0x56f590: /* Alt+1..7: store the camera as bookmark n (heading low word, pitch high word) */
    g_LevelCameraBookmark1PositionXQ12 = (worldRuntime->motion).positionXQ12;
    g_LevelCameraBookmark1PositionYQ12 = (worldRuntime->motion).positionYQ12;
    g_LevelCameraBookmark1PositionZQ12 = (worldRuntime->motion).positionZQ12;
    g_LevelCameraBookmark1PositionMagnitudeQ12 = (worldRuntime->motion).positionMagnitudeQ12;
    g_LevelCameraBookmark1PackedHeadingLow16PitchHigh16 =
         (worldRuntime->motion).pitchAngle << 16 | (worldRuntime->motion).headingAngle;
    break;
  case 0x56f5e0:
    g_LevelCameraBookmark2PositionXQ12 = (worldRuntime->motion).positionXQ12;
    g_LevelCameraBookmark2PositionYQ12 = (worldRuntime->motion).positionYQ12;
    g_LevelCameraBookmark2PositionZQ12 = (worldRuntime->motion).positionZQ12;
    g_LevelCameraBookmark2PositionMagnitudeQ12 = (worldRuntime->motion).positionMagnitudeQ12;
    g_LevelCameraBookmark2PackedHeadingLow16PitchHigh16 =
         (worldRuntime->motion).pitchAngle << 16 | (worldRuntime->motion).headingAngle;
    break;
  case 0x56f630:
    g_LevelCameraBookmark3PositionXQ12 = (worldRuntime->motion).positionXQ12;
    g_LevelCameraBookmark3PositionYQ12 = (worldRuntime->motion).positionYQ12;
    g_LevelCameraBookmark3PositionZQ12 = (worldRuntime->motion).positionZQ12;
    g_LevelCameraBookmark3PositionMagnitudeQ12 = (worldRuntime->motion).positionMagnitudeQ12;
    g_LevelCameraBookmark3PackedHeadingLow16PitchHigh16 =
         (worldRuntime->motion).pitchAngle << 16 | (worldRuntime->motion).headingAngle;
    break;
  case 0x56f680:
    g_LevelCameraBookmark4PositionXQ12 = (worldRuntime->motion).positionXQ12;
    g_LevelCameraBookmark4PositionYQ12 = (worldRuntime->motion).positionYQ12;
    g_LevelCameraBookmark4PositionZQ12 = (worldRuntime->motion).positionZQ12;
    g_LevelCameraBookmark4PositionMagnitudeQ12 = (worldRuntime->motion).positionMagnitudeQ12;
    g_LevelCameraBookmark4PackedHeadingLow16PitchHigh16 =
         (worldRuntime->motion).pitchAngle << 16 | (worldRuntime->motion).headingAngle;
    break;
  case 0x56f6d0:
    g_LevelCameraBookmark5PositionXQ12 = (worldRuntime->motion).positionXQ12;
    g_LevelCameraBookmark5PositionYQ12 = (worldRuntime->motion).positionYQ12;
    g_LevelCameraBookmark5PositionZQ12 = (worldRuntime->motion).positionZQ12;
    g_LevelCameraBookmark5PositionMagnitudeQ12 = (worldRuntime->motion).positionMagnitudeQ12;
    g_LevelCameraBookmark5PackedHeadingLow16PitchHigh16 =
         (worldRuntime->motion).pitchAngle << 16 | (worldRuntime->motion).headingAngle;
    break;
  case 0x56f720:
    g_LevelCameraBookmark6PositionXQ12 = (worldRuntime->motion).positionXQ12;
    g_LevelCameraBookmark6PositionYQ12 = (worldRuntime->motion).positionYQ12;
    g_LevelCameraBookmark6PositionZQ12 = (worldRuntime->motion).positionZQ12;
    g_LevelCameraBookmark6PositionMagnitudeQ12 = (worldRuntime->motion).positionMagnitudeQ12;
    g_LevelCameraBookmark6PackedHeadingLow16PitchHigh16 =
         (worldRuntime->motion).pitchAngle << 16 | (worldRuntime->motion).headingAngle;
    break;
  case 0x56f770:
    g_LevelCameraBookmark7PositionXQ12 = (worldRuntime->motion).positionXQ12;
    g_LevelCameraBookmark7PositionYQ12 = (worldRuntime->motion).positionYQ12;
    g_LevelCameraBookmark7PositionZQ12 = (worldRuntime->motion).positionZQ12;
    g_LevelCameraBookmark7PositionMagnitudeQ12 = (worldRuntime->motion).positionMagnitudeQ12;
    g_LevelCameraBookmark7PackedHeadingLow16PitchHigh16 =
         (worldRuntime->motion).pitchAngle << 16 | (worldRuntime->motion).headingAngle;
    break;
  case 0x56f7d0: /* Alt+S */
    worldRuntime->runtimeFlags = worldRuntime->runtimeFlags ^ WORLD_RUNTIME_FLAG_SHADING_ENABLED;
    break;
  case 0x56f7e0: /* Ctrl+C */
    worldRuntime->runtimeFlags = worldRuntime->runtimeFlags ^ WORLD_RUNTIME_FLAG_UNLIMITED_CAMERA;
  }
  return;
}


/* Address: 0x0056D4B0.
   Ends a notification "go to" (state 27): resets the state to idle, walks up to the in-game root, clears bit 0x10
   of its world runtimeFlags and restores the camera saved by InGameTargetingContext_AdvanceOrResolveTarget.
   Also the queued UI action handler for INGAME_PAGE10[14] (0x100E).
*/
void InGameTargetingContext_CancelAndRestoreState(InGameTargetingRootTraversalView9E60 *targetingContext)

{
  WorldRuntimeFlags *runtimeFlagsField;
  UiNodeBase *parentCursor;

  if (targetingContext->actionState == INGAME_TARGETING_OBSERVED_CANCEL_AND_RESTORE) {
    targetingContext->actionState = INGAME_TARGETING_OBSERVED_IDLE;
    parentCursor = targetingContext->base.parent;
    while (parentCursor != UI_NODE_NONE) {
      targetingContext = (InGameTargetingRootTraversalView9E60 *)targetingContext->base.parent;
      parentCursor = targetingContext->base.parent;
    }
    runtimeFlagsField = &targetingContext->worldRuntime0A30.runtimeFlags;
    *runtimeFlagsField = *runtimeFlagsField & ~0x10;
    WorldRuntime_RestoreMotionStateFromSnapshot(&targetingContext->worldRuntime0A30);
  }
  return;
}


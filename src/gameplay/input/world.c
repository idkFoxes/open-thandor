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

/* Entries of g_InGamePointerModeHandlers (InGameSelection_SetAircraftPadTargetLane1/2,
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
void InGameTargetingContext_AdvanceOrResolveTarget(InGameTargetingRootTraversalView *targetingContext)

{
  WorldRuntimeFlags *runtimeFlagsField;
  UiNodeBase *parentNode;
  InGameNotificationPayloadKind payloadKind;
  Q12 secondaryCoordinateQ12;
  WorldOwnerListNode *ownerNode;
  int payloadEntityAddress;
  CommandPayload modelToken;
  CommandPayload armyToken;
  FixedVectorQ12 nearestTerrainPoint;
  
  if (targetingContext->actionState == INGAME_TARGETING_OBSERVED_CANCEL_AND_RESTORE) {
    InGameTargetingContext_CancelAndRestoreState(targetingContext);
  }
  else if (targetingContext->actionState == INGAME_TARGETING_OBSERVED_ADVANCE_OR_RESOLVE) {
    /* walk up to the in-game root node */
    parentNode = targetingContext->base.parent;
    while (parentNode != UI_NODE_NONE) {
      targetingContext = (InGameTargetingRootTraversalView *)targetingContext->base.parent;
      parentNode = targetingContext->base.parent;
    }
    payloadKind = targetingContext->activeNotificationPayload.payloadKind;
    if ((targetingContext->worldRuntime.runtimeFlags & WORLD_RUNTIME_FLAG_NOTIFICATION_GOTO) == 0) {
      WorldRuntime_CaptureMotionStateToSnapshot(&targetingContext->worldRuntime);
    }
    switch(payloadKind) {
    case TECHNOLOGY_UNLOCK_POSITION:
      /* own model at exactly the payload position; the tokens are the rebased army/model offsets */
      for (ownerNode = targetingContext->worldRuntime.ownerListHead;
          ownerNode != NULL; ownerNode = ownerNode->nextNode) {
        if ((((ownerNode->ownerClassId == WORLD_OWNER_RUNTIME_MODEL) &&
             (ownerNode->worldXQ12 ==
              (targetingContext->activeNotificationPayload).worldXQ12)) &&
            (payloadEntityAddress =
                  (int)((ModelRuntimeSlot *)ownerNode->runtimePayload)->ownerArmyRuntimeOrSavedOffset.armyRuntime,
            ownerNode->worldYQ12 ==
            (targetingContext->activeNotificationPayload).worldYQ12)) &&
           ((targetingContext->worldRuntime).activeFactionRuntimeIndex ==
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
          targetingContext->notificationButtonCursorFrame = NOTIFICATION_INTERACTION_NONE;
          return;
        }
      }
      break;
    case FACTION_IMPACT_ANCHOR:
      secondaryCoordinateQ12 = (targetingContext->activeNotificationPayload).worldYQ12;
      targetingContext->targetingWorldXQ12 =
           (targetingContext->activeNotificationPayload).worldXQ12;
      targetingContext->targetingWorldYQ12 = secondaryCoordinateQ12;
      /* falls through */
    case ARMY_CREATED:
      FieldGrid_GetNearestTerrainPoint
                ((targetingContext->activeNotificationPayload).worldYQ12,
                 (targetingContext->activeNotificationPayload).worldXQ12,
                 (targetingContext->worldRuntime).fieldGrid,&nearestTerrainPoint);
      WorldRuntime_PointCameraAtTarget
                ((targetingContext->worldRuntime).motion.pitchAngle,
                 (targetingContext->activeNotificationPayload).headingAngle,
                 (targetingContext->worldRuntime).motion.targetDistanceQ12,nearestTerrainPoint.zQ12,
                 (targetingContext->activeNotificationPayload).worldYQ12,
                 (targetingContext->activeNotificationPayload).worldXQ12,
                 &targetingContext->worldRuntime);
      runtimeFlagsField = &targetingContext->worldRuntime.runtimeFlags;
      *runtimeFlagsField = *runtimeFlagsField & ~WORLD_RUNTIME_FLAG_NOTIFICATION_GOTO;
      targetingContext->notificationButtonCursorFrame = 27; /* next click cancels */
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
                ,WorldOwnerListNode *candidateNode,WorldRuntimeContext *inGameRuntime)

{
  uint32_t cursorOrVariantMask;
  int ownerIndex;
  uint32_t modifierModeMask;
  GameEntityRuntime *entry;
  bool testResult;
  bool classifySelectedState;
  Q12 conditionRatioQ12;

  classifySelectedState = false;
  g_InGameCommandPreviewSurfaceHeightQ12OrSentinel = WORLD_POINTER_NO_HIT;
  (inGameRuntime->selection).selectedEntity = NULL;
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
  testResult = SelectionInfo_TestNotOwnAircraftPadsWithAircraft(ownerIndex);
  if (!testResult) {
    /* command mode: an own army under the pointer (candidate height at most 1.0 in Q12 above the picked
       height) is selected on click; otherwise the modifier keys pick the pointer-mode command to preview */
    g_InGamePointerInteractionStateFlags = g_InGamePointerInteractionStateFlags & ~WORLD_POINTER_STATE_OVER_OWN_ARMY;
    if ((((candidateNode != NULL) &&
         (candidateNode->ownerClassId == WORLD_OWNER_RUNTIME_MODEL)) &&
        ((int)(candidateHeightQ12 - Q12_ONE) <= (int)pickedHeightQ12)) &&
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
      (int)pickedHeightQ12 < (int)(candidateHeightQ12 - Q12_ONE) ||
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
    testResult = SelectionInfo_TestAnyEntryWeaponDamageNonnegative();
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
    classifySelectedState = SelectionInfo_TestAnyEntryWeaponDamageNonnegative();
  }
  if (classifySelectedState) {
    testResult = SelectionInfo_TestNoEntryHasWeaponDamage();
    if (testResult) {
      return GRAPHICS_CURSOR_FRAME_ARROW;
    }
    if (candidateNode == NULL) {
      return WORLD_CURSOR_TARGET;
    }
    testResult = GameFactionRuntime_TestCapabilityBitClear
                      ((entry->common).ownership.ownerIndex,ownerIndex);
    if (testResult) {
      return WORLD_CURSOR_TARGET;
    }
    testResult = SelectionInfo_IsEntryAbsent(entry);
    if (testResult) {
      return WORLD_CURSOR_TARGET_REJECTED;
    }
    return GRAPHICS_CURSOR_FRAME_ARROW;
  }
  if (candidateNode == NULL) {
    return GRAPHICS_CURSOR_FRAME_ARROW;
  }
  testResult = GameFactionRuntime_TestCapabilityBitClear
                    ((entry->common).ownership.ownerIndex,ownerIndex);
  if (testResult) {
    return WORLD_CURSOR_TARGET_REJECTED;
  }
  conditionRatioQ12 = ModelRuntime_QueryHierarchyConditionRatioQ12((RuntimeModelFactionPrefix *)entry);
  if (conditionRatioQ12 != Q12_ONE) {
    testResult = SelectionInfo_IsEntryAbsent(entry);
    if (testResult) {
      return WORLD_CURSOR_TARGET;
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
          InGamePointerCallbackValue3 candidateHeightQ12,WorldOwnerListNode *candidateNode,
          WorldRuntimeContext *inGameRuntime)

{
  FactionRuntimeIndex ownerIndex;
  int payloadEntityAddress;
  CommandPayload modelToken;
  bool testResult;
  
  if (((g_UiCommandRuntimeFlags & (UI_COMMAND_RUNTIME_FLAG_PAUSED | UI_COMMAND_RUNTIME_FLAG_WORLD_INPUT_DISABLED)) ==
       0) &&
     (inGameRuntime->runtimeFlags = inGameRuntime->runtimeFlags & ~WORLD_RUNTIME_FLAG_REPLACE_SELECTION,
     ((inGameRuntime->interaction).nodeFlags & 8) == 0)) {
    if (((inGameRuntime->interaction).nodeFlags & UI_NODE_REPEAT_OR_DOUBLE_CLICK) != 0) {
      /* makes the release replace the selection instead of selecting a single army */
      inGameRuntime->runtimeFlags = inGameRuntime->runtimeFlags | WORLD_RUNTIME_FLAG_REPLACE_SELECTION;
    }
    ownerIndex = inGameRuntime->activeFactionRuntimeIndex;
    if ((inGameRuntime->runtimeFlags & WORLD_RUNTIME_FLAG_NOTIFICATION_GOTO) == 0) {
      if ((g_UiCommandRuntimeFlags & UI_COMMAND_RUNTIME_FLAG_PLACEMENT_PENDING) == 0) {
        testResult = SelectionInfo_TestNotOwnAircraftPadsWithAircraft(ownerIndex);
        if (testResult) {
          g_InGamePointerInteractionStateFlags = g_InGamePointerInteractionStateFlags |
               WORLD_POINTER_STATE_SELECTION_CAPTURE;
        }
        else {
          if ((((candidateNode != NULL) &&
               (candidateNode->ownerClassId == WORLD_OWNER_RUNTIME_MODEL)) &&
              (payloadEntityAddress =
                    (int)((ModelRuntimeSlot *)candidateNode->runtimePayload)->ownerArmyRuntimeOrSavedOffset.armyRuntime,
              (int)(candidateHeightQ12 - Q12_ONE) <= (int)pickedHeightQ12)) &&
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
               THANDOR_CONTAINER_OF(inGameRuntime, InGameRuntimeRoot, worldRuntime)->pointerPressX;
          g_InGameCommandPointerCaptureY =
               THANDOR_CONTAINER_OF(inGameRuntime, InGameRuntimeRoot, worldRuntime)->pointerPressY;
          g_UiCommandRuntimeFlags = g_UiCommandRuntimeFlags | UI_COMMAND_RUNTIME_FLAG_COMMAND_POINTER_CAPTURED;
        }
      }
      else {
        g_InGamePlacementPointerCaptureX =
             THANDOR_CONTAINER_OF(inGameRuntime, InGameRuntimeRoot, worldRuntime)->pointerPressX;
        g_InGamePlacementPointerCaptureY =
             THANDOR_CONTAINER_OF(inGameRuntime, InGameRuntimeRoot, worldRuntime)->pointerPressY;
      }
    }
    else {
      inGameRuntime->runtimeFlags = inGameRuntime->runtimeFlags & ~WORLD_RUNTIME_FLAG_NOTIFICATION_GOTO;
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
          uint32_t candidateHeightQ12,WorldOwnerListNode *candidateNode,
          WorldRuntimeContext *inGameRuntime)

{
  uint32_t deltaXOrTripletCount;
  int countOrOwnerOrDelta;
  uint32_t deltaY;
  InGameCommandPayloadTripletValue32 payloadValue;
  uint32_t *clearCursor;
  WorldOwnerListNode *runtimeNode;
  CommandPayload *tripletCursor;
  bool testResult;
  GameEntityRuntime *entry;
  
  if (((((g_UiCommandRuntimeFlags & (UI_COMMAND_RUNTIME_FLAG_PAUSED | UI_COMMAND_RUNTIME_FLAG_WORLD_INPUT_DISABLED)) ==
         0) &&
       (((inGameRuntime->interaction).nodeFlags & 8) == 0)) &&
      ((g_UiCommandRuntimeFlags & UI_COMMAND_RUNTIME_FLAG_WORLD_INPUT_DISABLED) == 0)) &&
     ((inGameRuntime->runtimeFlags & WORLD_RUNTIME_FLAG_NOTIFICATION_GOTO) == 0)) {
    if ((g_UiCommandRuntimeFlags & UI_COMMAND_RUNTIME_FLAG_PLACEMENT_PENDING) == 0) {
      testResult = SelectionInfo_TestNotOwnAircraftPadsWithAircraft
                        (inGameRuntime->activeFactionRuntimeIndex);
      if (testResult) {
        if ((inGameRuntime->runtimeFlags & WORLD_RUNTIME_FLAG_DRAG_SELECTING) == 0) {
          /* pointer travel since the press (the pointer positions are kept in the in-game root) */
          deltaXOrTripletCount =
               THANDOR_CONTAINER_OF(inGameRuntime, InGameRuntimeRoot, worldRuntime)->pointerPressX -
               THANDOR_CONTAINER_OF(inGameRuntime, InGameRuntimeRoot, worldRuntime)->pointerX;
          if ((int)deltaXOrTripletCount < 0) {
            deltaXOrTripletCount = -deltaXOrTripletCount;
          }
          deltaY = THANDOR_CONTAINER_OF(inGameRuntime, InGameRuntimeRoot,
                                        worldRuntime)->pointerPressY -
                  THANDOR_CONTAINER_OF(inGameRuntime, InGameRuntimeRoot, worldRuntime)->pointerY;
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
          for (countOrOwnerOrDelta = 26; countOrOwnerOrDelta != 0; countOrOwnerOrDelta--) {
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
                  (runtimeNode->runtimeFlags & MODEL_NODE_FLAG_FACTION_OWNED) != 0 &&
                  (countOrOwnerOrDelta == (entry->common).ownership.ownerIndex)))) {
                payloadValue = (int)entry - (int)g_ArmyRuntimeRebaseBaseMinusOne;
                testResult = WorldRuntimeNode_IsPositionInsideBounds
                                  (runtimeNode,
                                   (WorldRuntimeExtendedMapControlView *)inGameRuntime);
                if (testResult) {
                  testResult = SelectionInfo_IsEntryAbsent(entry);
                  deltaXOrTripletCount = g_InGameSelectionInsertTripletDwordCount;
                  if (testResult) {
                    testResult = InGameCommandQueue_ContainsTripletValue
                                      (payloadValue,INGAME_COMMAND_CODE_BASE + INGAME_COMMAND_SELECTION_INSERT);
                    if ((!testResult) &&
                       (*(InGameCommandPayloadTripletValue32 *)
                         (&g_InGameSelectionInsertTripletDwords + deltaXOrTripletCount * 4) = payloadValue,
                       deltaXOrTripletCount < 11)) {
                      g_InGameSelectionInsertTripletDwordCount =
                           g_InGameSelectionInsertTripletDwordCount + 1;
                    }
                  }
                }
                else {
                  testResult = SelectionInfo_IsEntryAbsent(entry);
                  deltaXOrTripletCount = g_InGameSelectionRemoveTripletDwordCount;
                  if (!testResult) {
                    testResult = InGameCommandQueue_ContainsTripletValue
                                      (payloadValue,INGAME_COMMAND_CODE_BASE + INGAME_COMMAND_SELECTION_REMOVE);
                    if ((!testResult) &&
                       (*(InGameCommandPayloadTripletValue32 *)
                         (&g_InGameSelectionRemoveTripletDwords + deltaXOrTripletCount * 4) = payloadValue,
                       deltaXOrTripletCount < 11)) {
                      g_InGameSelectionRemoveTripletDwordCount =
                           g_InGameSelectionRemoveTripletDwordCount + 1;
                    }
                  }
                }
              }
              runtimeNode = runtimeNode->nextNode;
            } while (runtimeNode != NULL);
            if (g_InGameSelectionRemoveTripletDwordCount != 0) {
              tripletCursor = (CommandPayload *)&g_InGameSelectionRemoveTripletDwords;
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
              tripletCursor = (CommandPayload *)&g_InGameSelectionInsertTripletDwords;
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
               THANDOR_CONTAINER_OF(inGameRuntime, InGameRuntimeRoot, worldRuntime)->pointerX -
                    g_InGameCommandPointerCaptureX;
          g_PointerSetPosition(g_InGameCommandPointerCaptureY,g_InGameCommandPointerCaptureX);
          g_InGameCommandPreviewHeading16 = g_InGameCommandPreviewHeading16 + countOrOwnerOrDelta * 64;
          g_InGameCommandPreviewHeading16 = g_InGameCommandPreviewHeading16 & FIXED_ANGLE16_MASK;
        }
        else {
          g_InGameCommandPointerCaptureX =
               THANDOR_CONTAINER_OF(inGameRuntime, InGameRuntimeRoot, worldRuntime)->pointerX;
          g_InGameCommandPointerCaptureY =
               THANDOR_CONTAINER_OF(inGameRuntime, InGameRuntimeRoot, worldRuntime)->pointerY;
        }
      }
    }
    else if (pickedHeightQ12 != WORLD_POINTER_NO_HIT) {
      if ((g_CursorButtonState & 4) == 0) {
        /* The original adds the horizontal mouse delta since capture (computed before snapping the
           pointer back) - not the pointer function's return value. */
        countOrOwnerOrDelta =
             THANDOR_CONTAINER_OF(inGameRuntime, InGameRuntimeRoot, worldRuntime)->pointerX -
                  g_InGamePlacementPointerCaptureX;
        g_PointerSetPosition(g_InGamePlacementPointerCaptureY,g_InGamePlacementPointerCaptureX);
        g_InGamePlacementHeading16 = g_InGamePlacementHeading16 + countOrOwnerOrDelta * 64;
        g_InGamePlacementHeading16 = g_InGamePlacementHeading16 & FIXED_ANGLE16_MASK;
      }
      else {
        g_InGamePlacementPointerCaptureX =
             THANDOR_CONTAINER_OF(inGameRuntime, InGameRuntimeRoot, worldRuntime)->pointerX;
        g_InGamePlacementPointerCaptureY =
             THANDOR_CONTAINER_OF(inGameRuntime, InGameRuntimeRoot, worldRuntime)->pointerY;
      }
    }
  }
  return;
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
  modeHandler = (InGamePointerModeHandler *)g_InGamePointerModeHandlers[modifierModeMask & variantMask];
  if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_NETWORKED_MASK) == SESSION_NETWORK_ROLE_LOCAL) {
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
  if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_NETWORKED_MASK) == SESSION_NETWORK_ROLE_LOCAL) {
    FrontendPlayerSelection_ClearAndRefreshLocalPanels(g_LocalPlayerRuntimeId,0,0,0);
  }
  else {
    InGameCommandQueue_AppendLocalPlayerCommand(INGAME_COMMAND_SELECTION_CLEAR,0,0,0);
  }
  return;
}


/* Selection-mode click: adds the candidate army to the selection (INGAME_COMMAND_SELECT_ARMY). */
static void InGameWorldInput_SelectCandidateArmy(GameEntityRuntime *entry)

{
  if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_NETWORKED_MASK) == SESSION_NETWORK_ROLE_LOCAL) {
    InGamePlayerSelection_SelectArmyRuntimeIndex
              (g_LocalPlayerRuntimeId,0,0,(int)entry - (int)g_ArmyRuntimeRebaseBaseMinusOne);
  }
  else {
    InGameCommandQueue_AppendLocalPlayerCommand
              (INGAME_COMMAND_SELECT_ARMY,0,0,(int)entry - (int)g_ArmyRuntimeRebaseBaseMinusOne);
  }
  return;
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
  armyRuntimeIndex = (int)entry - (int)g_ArmyRuntimeRebaseBaseMinusOne;
  if ((inGameRuntime->runtimeFlags & WORLD_RUNTIME_FLAG_REPLACE_SELECTION) == 0) {
    if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_NETWORKED_MASK) == SESSION_NETWORK_ROLE_LOCAL) {
      FrontendPlayerRuntime_AssignModelTokenAndRefreshSelection(g_LocalPlayerRuntimeId,0,0,armyRuntimeIndex);
    }
    else {
      InGameCommandQueue_AppendLocalPlayerCommand(INGAME_COMMAND_SELECT_SINGLE_ARMY,0,0,armyRuntimeIndex);
    }
  }
  else if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_NETWORKED_MASK) == SESSION_NETWORK_ROLE_LOCAL) {
    InGamePlayerSelection_ReplaceWithArmyRuntimeIndex(g_LocalPlayerRuntimeId,0,0,armyRuntimeIndex);
  }
  else {
    InGameCommandQueue_AppendLocalPlayerCommand(INGAME_COMMAND_REPLACE_SELECTION,0,0,armyRuntimeIndex);
  }
  return;
}


/* Selection-mode click on a candidate while no selected entry has a nonnegative weapon damage: a candidate whose
   faction capability bit is clear is ignored, one whose hierarchy condition ratio is not Q12_ONE is added to
   the selection, otherwise an own army is selected alone (InGameWorldInput_SelectOwnCandidateArmy). */
static void InGameWorldInput_CommitCandidateConditionClick
          (WorldRuntimeContext *inGameRuntime,int ownerIndex,GameEntityRuntime *entry)

{
  bool capabilityClear;
  Q12 conditionRatioQ12;

  capabilityClear = GameFactionRuntime_TestCapabilityBitClear((entry->common).ownership.ownerIndex,ownerIndex);
  if (capabilityClear) {
    return;
  }
  conditionRatioQ12 = ModelRuntime_QueryHierarchyConditionRatioQ12((RuntimeModelFactionPrefix *)entry);
  if (conditionRatioQ12 != Q12_ONE) {
    InGameWorldInput_SelectCandidateArmy(entry);
    return;
  }
  InGameWorldInput_SelectOwnCandidateArmy(inGameRuntime,ownerIndex,entry);
  return;
}


/* Shift/Alt-click (with or without Ctrl) on a candidate: toggles an own army in the selection
   (SelectionInfo_IsEntryAbsent is true when the entry is absent), selects a foreign one alone. */
static void InGameWorldInput_ToggleCandidateArmy(int ownerIndex,GameEntityRuntime *entry)

{
  bool entryAbsent;

  if (ownerIndex == (entry->common).ownership.ownerIndex) {
    entryAbsent = SelectionInfo_IsEntryAbsent(entry);
    if (entryAbsent) {
      if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_NETWORKED_MASK) == SESSION_NETWORK_ROLE_LOCAL) {
        FrontendPlayerSelection_InsertThreeEntriesAndRefresh
                  (g_LocalPlayerRuntimeId,0,0,(int)entry - (int)g_ArmyRuntimeRebaseBaseMinusOne);
      }
      else {
        InGameCommandQueue_AppendLocalPlayerCommand
                  (INGAME_COMMAND_SELECTION_INSERT,0,0,(int)entry - (int)g_ArmyRuntimeRebaseBaseMinusOne);
      }
    }
    else if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_NETWORKED_MASK) == SESSION_NETWORK_ROLE_LOCAL) {
      FrontendPlayerSelection_RemoveThreeEntriesAndRefresh
                (g_LocalPlayerRuntimeId,0,0,(int)entry - (int)g_ArmyRuntimeRebaseBaseMinusOne);
    }
    else {
      InGameCommandQueue_AppendLocalPlayerCommand
                (INGAME_COMMAND_SELECTION_REMOVE,0,0,(int)entry - (int)g_ArmyRuntimeRebaseBaseMinusOne);
    }
  }
  else if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_NETWORKED_MASK) == SESSION_NETWORK_ROLE_LOCAL) {
    FrontendPlayerRuntime_AssignModelTokenAndRefreshSelection
              (g_LocalPlayerRuntimeId,0,0,(int)entry - (int)g_ArmyRuntimeRebaseBaseMinusOne);
  }
  else {
    InGameCommandQueue_AppendLocalPlayerCommand
              (INGAME_COMMAND_SELECT_SINGLE_ARMY,0,0,(int)entry - (int)g_ArmyRuntimeRebaseBaseMinusOne);
  }
  return;
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
  bool capabilityClear;

  /* same candidate filter as InGameWorldInput_ResolveContextActionAndCursor; entry stays NULL without a
     candidate army */
  ownerIndex = inGameRuntime->activeFactionRuntimeIndex;
  entry = NULL;
  if ((candidateNode != NULL) && (candidateNode->ownerClassId == WORLD_OWNER_RUNTIME_MODEL)) {
    entry = *(GameEntityRuntime **)((uint8_t *)candidateNode->runtimePayload + 8);
    if (((int)pickedHeightQ12 < (int)(candidateHeightQ12 - Q12_ONE)) ||
        ((entry->common).ownership.ownerIndex == 0)) {
      entry = NULL;
    }
  }
  if (!SelectionInfo_HasAnyEntry() || SelectionInfo_AllEntriesEmptyOrMatchOwner(ownerIndex)) {
    if (entry != NULL) {
      if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_NETWORKED_MASK) == SESSION_NETWORK_ROLE_LOCAL) {
        FrontendPlayerRuntime_AssignModelTokenAndRefreshSelection
                  (g_LocalPlayerRuntimeId,0,0,(int)entry - (int)g_ArmyRuntimeRebaseBaseMinusOne);
      }
      else {
        InGameCommandQueue_AppendLocalPlayerCommand
                  (INGAME_COMMAND_SELECT_SINGLE_ARMY,0,0,(int)entry - (int)g_ArmyRuntimeRebaseBaseMinusOne);
      }
    }
    return;
  }
  if ((g_KeyboardStateMask & KEYBOARD_STATE_CTRL) == 0) {
    if (entry == NULL) {
      /* ground click: move, or position with Shift/Alt */
      if (!SelectionInfo_TestAnyActiveOrSingleClass13() && (pickedHeightQ12 != WORLD_POINTER_NO_HIT)) {
        if ((g_KeyboardStateMask & (KEYBOARD_STATE_SHIFT | KEYBOARD_STATE_ALT)) == 0) {
          if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_NETWORKED_MASK) == SESSION_NETWORK_ROLE_LOCAL) {
            InGamePlayerSelection_ApplyMoveCommand(g_LocalPlayerRuntimeId,0,pointerWorldXQ12,pointerWorldYQ12);
          }
          else {
            InGameCommandQueue_AppendLocalPlayerCommand(INGAME_COMMAND_MOVE,0,pointerWorldXQ12,pointerWorldYQ12);
          }
        }
        else if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_NETWORKED_MASK) == SESSION_NETWORK_ROLE_LOCAL) {
          InGamePlayerSelection_ApplyPositionCommand(g_LocalPlayerRuntimeId,0,pointerWorldXQ12,pointerWorldYQ12);
        }
        else {
          InGameCommandQueue_AppendLocalPlayerCommand(INGAME_COMMAND_POSITION,0,pointerWorldXQ12,pointerWorldYQ12);
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
    if (entry == NULL) {
      /* Ctrl ground click: target position at the top surface height under the pointer */
      if (pickedHeightQ12 != WORLD_POINTER_NO_HIT) {
        surfaceHeightQ12 = WorldRuntime_InterpolateTopSurfaceHeightOrSentinel
                                (pointerWorldXQ12,pointerWorldYQ12,inGameRuntime);
        if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_NETWORKED_MASK) == SESSION_NETWORK_ROLE_LOCAL) {
          InGamePlayerSelection_ApplyTargetPositionCommand
                    (g_LocalPlayerRuntimeId,surfaceHeightQ12,pointerWorldXQ12,pointerWorldYQ12);
        }
        else {
          InGameCommandQueue_AppendLocalPlayerCommand(INGAME_COMMAND_TARGET_POSITION,surfaceHeightQ12,
                                                      pointerWorldXQ12,pointerWorldYQ12);
        }
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
  if (entry != NULL) {
    InGameWorldInput_ToggleCandidateArmy(ownerIndex,entry);
  }
  return;
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
      if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_NETWORKED_MASK) == SESSION_NETWORK_ROLE_LOCAL) {
        InGameCommand_ExecuteLocalPlacementFromSelection
                  (g_LocalPlayerRuntimeId,g_InGamePlacementHeading16,pointerWorldXQ12,pointerWorldYQ12);
      }
      else {
        InGameCommandQueue_AppendLocalPlayerCommand
                  (INGAME_COMMAND_PLACE_ARMY,g_InGamePlacementHeading16,pointerWorldXQ12,pointerWorldYQ12);
      }
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
          WorldOwnerListNode *candidateNode,WorldRuntimeContext *inGameRuntime)

{
  InGameWorldInput_DispatchPointerRelease
            (pickedHeightQ12,pointerWorldXQ12,pointerWorldYQ12,candidateHeightQ12,candidateNode,inGameRuntime);
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
  for (currentRecord = nextRecord;
       recordKeyCode = currentRecord->records[0].keyCode,
       requiredModifiers = currentRecord->records[0].requiredModifierMask, recordKeyCode != 0;
       currentRecord = nextRecord) {
    nextRecord = (InGameCameraCommandDispatchTable *)(currentRecord->records + 1);
    if ((recordKeyCode != commandCode) ||
        !((requiredModifiers == 0) ? ((modifierFlags & (KEYBOARD_STATE_CTRL | KEYBOARD_STATE_ALT)) == 0)
                                   : ((modifierFlags & requiredModifiers) != 0))) continue;
    /* Matching record: run its command and stop. The original jumps to the record's continuation address; the
       cases are those addresses. */
    switch(currentRecord->records[0].continuationEntryAddress) {
    case 0x56f360: /* 1..7: recall bookmark n */
      WorldRuntime_SetCameraPositionKeepingTarget
                (g_LevelCameraBookmark1PositionZQ12,g_LevelCameraBookmark1PositionYQ12,
                 g_LevelCameraBookmark1PositionXQ12,worldRuntime);
      WorldRuntime_SetCameraAnglesAndMagnitudeClamped
                (2,(int)bookmark1PackedAngles >> 16,bookmark1PackedAngles & FIXED_ANGLE16_MASK,
                 g_LevelCameraBookmark1PositionMagnitudeQ12,
                 worldRuntime);
      WorldRuntime_RecomputeMotionEndpointAgainstFieldSurface(worldRuntime);
      WorldRuntime_CommitCameraTargetDistance(worldRuntime);
      break;
    case 0x56f3b0:
      WorldRuntime_SetCameraPositionKeepingTarget
                (g_LevelCameraBookmark2PositionZQ12,g_LevelCameraBookmark2PositionYQ12,
                 g_LevelCameraBookmark2PositionXQ12,worldRuntime);
      WorldRuntime_SetCameraAnglesAndMagnitudeClamped
                (2,(int)bookmark2PackedAngles >> 16,bookmark2PackedAngles & FIXED_ANGLE16_MASK,
                 g_LevelCameraBookmark2PositionMagnitudeQ12,
                 worldRuntime);
      WorldRuntime_RecomputeMotionEndpointAgainstFieldSurface(worldRuntime);
      WorldRuntime_CommitCameraTargetDistance(worldRuntime);
      break;
    case 0x56f400:
      WorldRuntime_SetCameraPositionKeepingTarget
                (g_LevelCameraBookmark3PositionZQ12,g_LevelCameraBookmark3PositionYQ12,
                 g_LevelCameraBookmark3PositionXQ12,worldRuntime);
      WorldRuntime_SetCameraAnglesAndMagnitudeClamped
                (2,(int)bookmark3PackedAngles >> 16,bookmark3PackedAngles & FIXED_ANGLE16_MASK,
                 g_LevelCameraBookmark3PositionMagnitudeQ12,
                 worldRuntime);
      WorldRuntime_RecomputeMotionEndpointAgainstFieldSurface(worldRuntime);
      WorldRuntime_CommitCameraTargetDistance(worldRuntime);
      break;
    case 0x56f450:
      WorldRuntime_SetCameraPositionKeepingTarget
                (g_LevelCameraBookmark4PositionZQ12,g_LevelCameraBookmark4PositionYQ12,
                 g_LevelCameraBookmark4PositionXQ12,worldRuntime);
      WorldRuntime_SetCameraAnglesAndMagnitudeClamped
                (2,(int)bookmark4PackedAngles >> 16,bookmark4PackedAngles & FIXED_ANGLE16_MASK,
                 g_LevelCameraBookmark4PositionMagnitudeQ12,
                 worldRuntime);
      WorldRuntime_RecomputeMotionEndpointAgainstFieldSurface(worldRuntime);
      WorldRuntime_CommitCameraTargetDistance(worldRuntime);
      break;
    case 0x56f4a0:
      WorldRuntime_SetCameraPositionKeepingTarget
                (g_LevelCameraBookmark5PositionZQ12,g_LevelCameraBookmark5PositionYQ12,
                 g_LevelCameraBookmark5PositionXQ12,worldRuntime);
      WorldRuntime_SetCameraAnglesAndMagnitudeClamped
                (2,(int)bookmark5PackedAngles >> 16,bookmark5PackedAngles & FIXED_ANGLE16_MASK,
                 g_LevelCameraBookmark5PositionMagnitudeQ12,
                 worldRuntime);
      WorldRuntime_RecomputeMotionEndpointAgainstFieldSurface(worldRuntime);
      WorldRuntime_CommitCameraTargetDistance(worldRuntime);
      break;
    case 0x56f4f0:
      WorldRuntime_SetCameraPositionKeepingTarget
                (g_LevelCameraBookmark6PositionZQ12,g_LevelCameraBookmark6PositionYQ12,
                 g_LevelCameraBookmark6PositionXQ12,worldRuntime);
      WorldRuntime_SetCameraAnglesAndMagnitudeClamped
                (2,(int)bookmark6PackedAngles >> 16,bookmark6PackedAngles & FIXED_ANGLE16_MASK,
                 g_LevelCameraBookmark6PositionMagnitudeQ12,
                 worldRuntime);
      WorldRuntime_RecomputeMotionEndpointAgainstFieldSurface(worldRuntime);
      WorldRuntime_CommitCameraTargetDistance(worldRuntime);
      break;
    case 0x56f540:
      WorldRuntime_SetCameraPositionKeepingTarget
                (g_LevelCameraBookmark7PositionZQ12,g_LevelCameraBookmark7PositionYQ12,
                 g_LevelCameraBookmark7PositionXQ12,worldRuntime);
      WorldRuntime_SetCameraAnglesAndMagnitudeClamped
                (2,(int)bookmark7PackedAngles >> 16,bookmark7PackedAngles & FIXED_ANGLE16_MASK,
                 g_LevelCameraBookmark7PositionMagnitudeQ12,
                 worldRuntime);
      WorldRuntime_RecomputeMotionEndpointAgainstFieldSurface(worldRuntime);
      WorldRuntime_CommitCameraTargetDistance(worldRuntime);
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
  return;
}


/* Address: 0x0056D4B0.
   Ends a notification "go to" (state 27): resets the state to idle, walks up to the in-game root, clears bit 0x10
   of its world runtimeFlags and restores the camera saved by InGameTargetingContext_AdvanceOrResolveTarget.
   Also the queued UI action handler for INGAME_PAGE10[14] (0x100E).
*/
void InGameTargetingContext_CancelAndRestoreState(InGameTargetingRootTraversalView *targetingContext)

{
  WorldRuntimeFlags *runtimeFlagsField;
  UiNodeBase *parentCursor;

  if (targetingContext->actionState == INGAME_TARGETING_OBSERVED_CANCEL_AND_RESTORE) {
    targetingContext->actionState = INGAME_TARGETING_OBSERVED_IDLE;
    parentCursor = targetingContext->base.parent;
    while (parentCursor != UI_NODE_NONE) {
      targetingContext = (InGameTargetingRootTraversalView *)targetingContext->base.parent;
      parentCursor = targetingContext->base.parent;
    }
    runtimeFlagsField = &targetingContext->worldRuntime.runtimeFlags;
    *runtimeFlagsField = *runtimeFlagsField & ~WORLD_RUNTIME_FLAG_NOTIFICATION_GOTO;
    WorldRuntime_RestoreMotionStateFromSnapshot(&targetingContext->worldRuntime);
  }
  return;
}


/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/src/gameplay/input/world.c
 * Reverse engineering by idkFoxes 2026
 */

#include <thandor/gameplay/input/world.h>
#include <thandor/thandor.h>

/* Implementation ownership: gameplay/input/world. */

/* Entries of g_InGamePointerModeHandlers (InGameSelection_ApplyType16MarkerCoordinatesVariant1/2,
   SelectionMarkerCoordinates_ApplyType3..7): four stack arguments, RET 0x10. */
typedef void InGamePointerModeHandler
          (SelectionMarkerIndex selectionIndex,SelectionMarkerCoordinateValue32 valueC,
          SelectionMarkerCoordinateValue32 valueB,SelectionMarkerCoordinateValue32 valueA);

/* Address: 0x0056D2D0.
   Ownership: gameplay/input/world.
   Purpose: Handles targeting-context state 7 or 0x1B. State 0x1B delegates to the cancellation handler. State 7
   resolves the root targeting context, updates or searches the active target according to the numeric mode at
   root+0x9E54, and may dispatch backend operation 0x1620. Exact original target-mode labels are not preserved.
   Table 0056D340: 0056D4A0, 0056D3A0, 0056D3F0, 0056D380, 0056D4A0, 0056D4A0, 0056D4A0, 0056D4A0, 0056D4A0,
   0056D4A0, 0056D4A0, 0056D4A0, 0056D4A0, 0056D4A0, 0056D4A0, 0056D4A0.
   Local calls: InGameTargetingContext_CancelAndRestoreState.
   Cross-module calls: WorldRuntime_CaptureMotionStateToSnapshot [world/runtime/core],
   FrontendPlayerRuntime_AssignModelAndArmyTokensAndRefreshLocalPanel [ui/frontend/player],
   InGameCommandQueue_AppendLocalPlayerCommand [network/protocol/commands], FieldGrid_GetNearestTerrainPoint
   [world/terrain/grid], WorldRuntime_SetPosition80AndRebuildPosition60FromAngles [world/runtime/core].
*/
void __thandor_void_preserve_eax_ecx_edx
InGameTargetingContext_AdvanceOrResolveTarget
          (InGameTargetingRootTraversalView9E60 *targetingContext)

{
  WorldRuntimeFlags *runtimeFlagsField;
  UiNodeBase *parentNode;
  InGameNotificationPayloadKind payloadKind;
  Q12 secondaryCoordinateQ12;
  WorldOwnerListNode100 *ownerNode;
  int payloadEntityAddress;
  CommandPayloadDword04 modelToken;
  CommandPayloadDword08 armyToken;
  FieldGridNearestPointRegsCf13 nearestTerrainPoint;
  
  if (targetingContext->actionState == INGAME_TARGETING_OBSERVED_CANCEL_AND_RESTORE) {
    InGameTargetingContext_CancelAndRestoreState(targetingContext);
  }
  else if (targetingContext->actionState == INGAME_TARGETING_OBSERVED_ADVANCE_OR_RESOLVE) {
    parentNode = (targetingContext->base).parent;
    while (parentNode != (UiNodeBase *)0xffffffff) {
      targetingContext = (InGameTargetingRootTraversalView9E60 *)(targetingContext->base).parent;
      parentNode = (targetingContext->base).parent;
    }
    payloadKind = (targetingContext->activeNotificationPayload9E40).payloadKind14;
    if (((targetingContext->worldRuntime0A30).runtimeFlags & 0x10) == 0) {
      WorldRuntime_CaptureMotionStateToSnapshot(&targetingContext->worldRuntime0A30);
    }
                    // WARNING: Switch is manually overridden
    switch(payloadKind) {
    case TECHNOLOGY_UNLOCK_POSITION:
      for (ownerNode = (targetingContext->worldRuntime0A30).ownerListHead;
          ownerNode != (WorldOwnerListNode100 *)0x0; ownerNode = ownerNode->nextNode) {
        if ((((ownerNode->ownerClassId == WORLD_OWNER_RUNTIME_MODEL) &&
             (ownerNode->worldXQ12 ==
              (targetingContext->activeNotificationPayload9E40).primaryWorldCoordinateQ12_00)) &&
            (payloadEntityAddress = *(int *)((int)ownerNode->runtimePayload + 8),
            ownerNode->worldYQ12 ==
            (targetingContext->activeNotificationPayload9E40).secondaryWorldCoordinateQ12_04)) &&
           ((targetingContext->worldRuntime0A30).activeFactionRuntimeIndex == *(int *)(payloadEntityAddress + 0xc))
           ) {
          modelToken = payloadEntityAddress - (int)g_ArmyRuntimeRebaseBaseMinusOne;
          armyToken = (int)ownerNode->runtimePayload - g_ModelRuntimeRebaseDelta;
          if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_NETWORKED_MASK) ==
              SESSION_NETWORK_ROLE_LOCAL) {
            FrontendPlayerRuntime_AssignModelAndArmyTokensAndRefreshLocalPanel
                      (g_LocalPlayerRuntimeId,0,armyToken,modelToken);
          }
          else {
            InGameCommandQueue_AppendLocalPlayerCommand(0x1620,0,armyToken,modelToken);
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
                 (targetingContext->worldRuntime0A30).motion.targetDistanceQ12,nearestTerrainPoint.edx,
                 (targetingContext->activeNotificationPayload9E40).secondaryWorldCoordinateQ12_04,
                 (targetingContext->activeNotificationPayload9E40).primaryWorldCoordinateQ12_00,
                 &targetingContext->worldRuntime0A30);
      runtimeFlagsField = &(targetingContext->worldRuntime0A30).runtimeFlags;
      *runtimeFlagsField = *runtimeFlagsField & 0xffffffef;
      targetingContext->sessionNotificationInteractionState9B4C = 0x1b;
    }
  }
  return;
}


/* Address: 0x005688A0.
   Ownership: gameplay/input/world.
   Purpose: Resolves the world-context action and cursor value for the current pointer position. Typed parameters:
   p0 pointerValue0→InGamePointerCallbackValue0_V344, p1 pointerValue1→InGamePointerCallbackValue1_V344, p2
   pointerValue2→InGamePointerCallbackValue2_V344, p3 pointerValue3→InGamePointerCallbackValue3_V344. Calling
   convention, complete VariableStorage serialization, function bytes, control flow, globals, locals, and
   executable data remain unchanged.
   Cross-module calls: ArmyPlacement_ValidateAssetAtPointAndCellCornersCf [gameplay/army/placement],
   InGameSelectionDetailPanel_Rebuild [ui/ingame/runtime], SelectionInfo_ValidateOwnerType16AndAnyActiveCf
   [gameplay/selection/runtime], GameFactionRuntime_TestCapabilityBitClearCf [gameplay/faction/runtime],
   SelectionInfo_CollectAttachmentEffectVariantMask [gameplay/selection/runtime], SelectionInfo_HasAnyEntryCf
   [gameplay/selection/runtime].
*/
uint32_t InGameWorldInput_ResolveContextActionAndCursorCf
                (InGamePointerCallbackValue0 pointerValue0,InGamePointerCallbackValue1 pointerValue1
                ,InGamePointerCallbackValue2 pointerValue2,InGamePointerCallbackValue3 pointerValue3
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
  g_InGameCommandPreviewSurfaceHeightQ12OrSentinel = 0x7fffffff;
  (inGameRuntime->selection).selectedEntity = (GameEntityRuntime *)0x0;
  if (((inGameRuntime->interaction).interactionFlags48 & 8) != 0) {
    return 0;
  }
  if ((g_UiCommandRuntimeFlags & 0x100) != 0) {
    return 0;
  }
  ownerIndex = inGameRuntime->activeFactionRuntimeIndex;
  if ((inGameRuntime->runtimeFlags & 0x10) != 0) {
    return 6;
  }
  if ((g_UiCommandRuntimeFlags & 0x20) != 0) {
    g_InGamePlacementSurfaceHeightQ12OrSentinel = pointerValue0;
    g_InGamePlacementWorldYQ12 = pointerValue2;
    g_InGamePlacementWorldXQ12 = pointerValue1;
    if (pointerValue0 == 0x7fffffff) {
      return 0x18;
    }
    g_UiHoverSelectionRecord =
         (UiCommandRuntimeRecordPrefix *)
         g_SelectionPlayerRuntimeBlockPointers[(inGameRuntime->selection).activePlayerRuntimeId]->
         pendingSelectionEntityOffset8098;
    cursorOrVariantMask = 0x2e;
    testResult = ArmyPlacement_ValidateAssetAtPointAndCellCornersCf
                      (0,g_InGamePlacementHeading16,pointerValue1,pointerValue2,
                       g_UiHoverSelectionRecord->armyAssetId,
                       inGameRuntime->activeFactionRuntimeIndex,inGameRuntime);
    if (!testResult) {
      cursorOrVariantMask = 0x2d;
    }
    InGameSelectionDetailPanel_Rebuild();
    return cursorOrVariantMask;
  }
  testResult = SelectionInfo_ValidateOwnerType16AndAnyActiveCf(ownerIndex);
  if (!testResult) {
    g_InGamePointerInteractionStateFlags = g_InGamePointerInteractionStateFlags & 0xfffffffe;
    if ((((candidateNode != (WorldOwnerListNode100 *)0x0) &&
         (candidateNode->ownerClassId == WORLD_OWNER_RUNTIME_MODEL)) &&
        ((int)(pointerValue3 - 0x1000) <= (int)pointerValue0)) &&
       (testResult = GameFactionRuntime_TestCapabilityBitClearCf
                          (*(uint32_t *)(*(int *)((int)candidateNode->runtimePayload + 8) + 0xc),
                           ownerIndex), !testResult)) {
      g_InGamePointerInteractionStateFlags = g_InGamePointerInteractionStateFlags | 1;
      return 0x15;
    }
    modifierModeMask = 0;
    if ((g_KeyboardStateMask & 0x3f) == 0) {
      modifierModeMask = 7;
    }
    if ((g_KeyboardStateMask & 3) != 0) {
      modifierModeMask = modifierModeMask | 1;
    }
    if ((g_KeyboardStateMask & 0x30) != 0) {
      modifierModeMask = modifierModeMask | 2;
    }
    if ((g_KeyboardStateMask & 0xc) != 0) {
      modifierModeMask = modifierModeMask | 4;
    }
    cursorOrVariantMask = SelectionInfo_CollectAttachmentEffectVariantMask();
    g_InGameCommandPreviewWorldYQ12 = pointerValue2;
    g_InGameCommandPreviewWorldXQ12 = pointerValue1;
    g_InGameCommandPreviewSurfaceHeightQ12OrSentinel = pointerValue0;
    g_InGameCommandPreviewArmyAssetId = g_InGamePointerModePreviewArmyIds[modifierModeMask & cursorOrVariantMask];
    return g_InGamePointerModeCommandIds[modifierModeMask & cursorOrVariantMask];
  }
  if ((inGameRuntime->runtimeFlags & 0x80) != 0) {
    return 0;
  }
  entry = (GameEntityRuntime *)inGameRuntime;
  if (((candidateNode == (WorldOwnerListNode100 *)0x0) ||
      (candidateNode->ownerClassId != WORLD_OWNER_RUNTIME_MODEL)) ||
     ((entry = *(GameEntityRuntime **)((int)candidateNode->runtimePayload + 8),
      (int)pointerValue0 < (int)(pointerValue3 - 0x1000) ||
      ((entry->common).ownership.ownerIndex == 0)))) {
    candidateNode = (WorldOwnerListNode100 *)0x0;
  }
  if (candidateNode != (WorldOwnerListNode100 *)0x0) {
    (inGameRuntime->selection).selectedEntity = entry;
  }
  /* Ownership cursor used by several paths: 0 without a candidate, 0x15 for an own and 0x16 for a foreign
     candidate. */
  testResult = SelectionInfo_HasAnyEntryCf();
  if ((!testResult) || (SelectionInfo_AllEntriesEmptyOrMatchOwnerCf(ownerIndex))) {
    if (candidateNode == (WorldOwnerListNode100 *)0x0) {
      return 0;
    }
    return (ownerIndex != (entry->common).ownership.ownerIndex) ? 0x16 : 0x15;
  }
  if ((g_KeyboardStateMask & 0xc) == 0) {
    if (candidateNode == (WorldOwnerListNode100 *)0x0) {
      testResult = SelectionInfo_TestAnyActiveOrSingleClass13Cf();
      if (testResult) {
        return 0;
      }
      testResult = SelectionInfo_TestPositionCommandAtWorldPointCf
                        (pointerValue1,pointerValue2,inGameRuntime);
      if (testResult) {
        return 0x18;
      }
      if (pointerValue0 == 0x7fffffff) {
        return 0x18;
      }
      return 0x17;
    }
    if ((g_KeyboardStateMask & 0x33) != 0) {
      return (ownerIndex == (entry->common).ownership.ownerIndex) ? 0x15 : 0x16;
    }
    testResult = SelectionInfo_TestAnyStateField100NonnegativeCf();
    if (testResult) {
      testResult = GameFactionRuntime_TestCapabilityBitClearCf
                        ((entry->common).ownership.ownerIndex,ownerIndex);
      if (!testResult) {
        return (ownerIndex != (entry->common).ownership.ownerIndex) ? 0x16 : 0x15;
      }
      classifySelectedState = true;
    }
  }
  else {
    if ((g_KeyboardStateMask & 0x33) != 0) {
      if (candidateNode == (WorldOwnerListNode100 *)0x0) {
        return 0;
      }
      return (ownerIndex == (entry->common).ownership.ownerIndex) ? 0x15 : 0x16;
    }
    classifySelectedState = SelectionInfo_TestAnyStateField100NonnegativeCf();
  }
  if (classifySelectedState) {
    testResult = SelectionInfo_TestAllStateField100NonpositiveCf();
    if (testResult) {
      return 0;
    }
    if (candidateNode == (WorldOwnerListNode100 *)0x0) {
      return 0x19;
    }
    testResult = GameFactionRuntime_TestCapabilityBitClearCf
                      ((entry->common).ownership.ownerIndex,ownerIndex);
    if (testResult) {
      return 0x19;
    }
    testResult = SelectionInfo_FindEntryCf(entry);
    if (testResult) {
      return 0x1a;
    }
    return 0;
  }
  if (candidateNode == (WorldOwnerListNode100 *)0x0) {
    return 0;
  }
  testResult = GameFactionRuntime_TestCapabilityBitClearCf
                    ((entry->common).ownership.ownerIndex,ownerIndex);
  if (testResult) {
    return 0x1a;
  }
  scaleRatio = ModelRuntime_QueryHierarchyScaleRatioQ12Regs((RuntimeModelFactionPrefix10 *)entry);
  if ((int)scaleRatio != (int)(scaleRatio >> 0x20)) {
    testResult = SelectionInfo_FindEntryCf(entry);
    if (testResult) {
      return 0x19;
    }
    return 0;
  }
  return (ownerIndex != (entry->common).ownership.ownerIndex) ? 0x16 : 0x15;
}


/* Address: 0x00568CB0.
   Ownership: gameplay/input/world.
   Purpose: Begins in-game pointer capture and seeds drag or selection state. Typed parameters: p0
   pointerValue0→InGamePointerCallbackValue0_V344, p3 pointerValue3→InGamePointerCallbackValue3_V344. Calling
   convention, complete VariableStorage serialization, function bytes, control flow, globals, locals, and
   executable data remain unchanged.
   Cross-module calls: SelectionInfo_ValidateOwnerType16AndAnyActiveCf [gameplay/selection/runtime],
   GameFactionRuntime_TestCapabilityBitClearCf [gameplay/faction/runtime],
   InGameCommandQueue_AppendLocalPlayerCommand [network/protocol/commands],
   FrontendPlayerRuntime_AssignModelTokenAndRefreshSelection [ui/frontend/player],
   WorldRuntime_RestoreMotionStateFromSnapshot [world/runtime/core].
*/
void __thandor_void_preserve_eax_ecx_edx
InGameWorldInput_BeginPointerCaptureCf
          (InGamePointerCallbackValue0 pointerValue0,uint32_t pointerValue1,uint32_t pointerValue2,
          InGamePointerCallbackValue3 pointerValue3,WorldOwnerListNode100 *candidateNode,
          WorldRuntimeContext *inGameRuntime)

{
  FactionRuntimeIndex ownerIndex;
  int payloadEntityAddress;
  CommandPayloadDword04 modelToken;
  bool testResult;
  
  if (((g_UiCommandRuntimeFlags & 0x101) == 0) &&
     (inGameRuntime->runtimeFlags = inGameRuntime->runtimeFlags & 0xf7ffffff,
     ((inGameRuntime->interaction).interactionFlags48 & 8) == 0)) {
    if (((inGameRuntime->interaction).interactionFlags48 & 0x80) != 0) {
      inGameRuntime->runtimeFlags = inGameRuntime->runtimeFlags | 0x8000000;
    }
    ownerIndex = inGameRuntime->activeFactionRuntimeIndex;
    if ((inGameRuntime->runtimeFlags & 0x10) == 0) {
      if ((g_UiCommandRuntimeFlags & 0x20) == 0) {
        testResult = SelectionInfo_ValidateOwnerType16AndAnyActiveCf(ownerIndex);
        if (testResult) {
          g_InGamePointerInteractionStateFlags = g_InGamePointerInteractionStateFlags | 2;
        }
        else {
          if ((((candidateNode != (WorldOwnerListNode100 *)0x0) &&
               (candidateNode->ownerClassId == WORLD_OWNER_RUNTIME_MODEL)) &&
              (payloadEntityAddress = *(int *)((int)candidateNode->runtimePayload + 8),
              (int)(pointerValue3 - 0x1000) <= (int)pointerValue0)) &&
             (testResult = GameFactionRuntime_TestCapabilityBitClearCf
                                (*(uint32_t *)(payloadEntityAddress + 0xc),ownerIndex), !testResult)) {
            modelToken = payloadEntityAddress - (int)g_ArmyRuntimeRebaseBaseMinusOne;
            if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_NETWORKED_MASK) !=
                SESSION_NETWORK_ROLE_LOCAL) {
              InGameCommandQueue_AppendLocalPlayerCommand(0x9a0,0,0,modelToken);
              return;
            }
            FrontendPlayerRuntime_AssignModelTokenAndRefreshSelection
                      (g_LocalPlayerRuntimeId,0,0,modelToken);
            return;
          }
          g_InGameCommandPointerCaptureX =
               *(int32_t *)(inGameRuntime[1].interaction.reserved00_47 + 4);
          g_InGameCommandPointerCaptureY =
               *(int32_t *)(inGameRuntime[1].interaction.reserved00_47 + 8);
          g_UiCommandRuntimeFlags = g_UiCommandRuntimeFlags | 0x80;
        }
      }
      else {
        g_InGamePlacementPointerCaptureX =
             *(int32_t *)(inGameRuntime[1].interaction.reserved00_47 + 4);
        g_InGamePlacementPointerCaptureY =
             *(int32_t *)(inGameRuntime[1].interaction.reserved00_47 + 8);
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
   Ownership: gameplay/input/world.
   Purpose: Updates drag selection and camera movement while pointer capture remains active. Typed parameters: p0
   pointerValue0→InGamePointerCallbackValue0_V344. Calling convention, complete VariableStorage serialization,
   function bytes, control flow, globals, locals, and executable data remain unchanged.
   [VERSIONLESS_CANONICAL_DATATYPE_CLOSURE] Retired detached enum dictionary InGameCameraWorldToggleFlag after
   transferring its complete value vocabulary to code annotation. It is not a safe whole-value storage type.
   Cross-module calls: SelectionInfo_ValidateOwnerType16AndAnyActiveCf [gameplay/selection/runtime],
   FrontendPlayerSelection_ClearAndRefreshLocalPanels [ui/frontend/player],
   InGameCommandQueue_AppendLocalPlayerCommand [network/protocol/commands],
   WorldRuntimeNode_IsPositionInsideBoundsCf [world/runtime/core], SelectionInfo_FindEntryCf
   [gameplay/selection/runtime], InGameCommandQueue_ContainsTripletValueCf [network/protocol/commands].
*/
void __thandor_void_preserve_eax_ecx_edx
InGameWorldInput_UpdateDragSelectionAndCameraCf
          (InGamePointerCallbackValue0 pointerValue0,uint32_t pointerValue1,uint32_t pointerValue2,
          uint32_t pointerValue3,WorldOwnerListNode100 *candidateNode,
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
  
  if (((((g_UiCommandRuntimeFlags & 0x101) == 0) &&
       (((inGameRuntime->interaction).interactionFlags48 & 8) == 0)) &&
      ((g_UiCommandRuntimeFlags & 0x100) == 0)) && ((inGameRuntime->runtimeFlags & 0x10) == 0)) {
    if ((g_UiCommandRuntimeFlags & 0x20) == 0) {
      testResult = SelectionInfo_ValidateOwnerType16AndAnyActiveCf
                        (inGameRuntime->activeFactionRuntimeIndex);
      if (testResult) {
        if ((inGameRuntime->runtimeFlags & 0x80) == 0) {
          deltaXOrTripletCount = *(int *)(inGameRuntime[1].interaction.reserved00_47 + 4) -
                  *(int *)(inGameRuntime[1].interaction.reserved00_47 + 0xc);
          if ((int)deltaXOrTripletCount < 0) {
            deltaXOrTripletCount = -deltaXOrTripletCount;
          }
          deltaY = *(int *)(inGameRuntime[1].interaction.reserved00_47 + 8) -
                  *(int *)(inGameRuntime[1].interaction.reserved00_47 + 0x10);
          if ((int)deltaY < 0) {
            deltaY = -deltaY;
          }
          if ((0x17 < deltaXOrTripletCount) || (0x17 < deltaY)) {
            inGameRuntime->runtimeFlags = inGameRuntime->runtimeFlags | 0x80;
            if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_NETWORKED_MASK) ==
                SESSION_NETWORK_ROLE_LOCAL) {
              FrontendPlayerSelection_ClearAndRefreshLocalPanels(g_LocalPlayerRuntimeId,0,0,0);
            }
            else {
              InGameCommandQueue_AppendLocalPlayerCommand(0xba0,0,0,0);
            }
          }
        }
        else {
          clearCursor = (uint32_t *)&g_InGameSelectionInsertTripletDwords;
          for (countOrOwnerOrDelta = 0x1a; countOrOwnerOrDelta != 0; countOrOwnerOrDelta = countOrOwnerOrDelta + -1) {
            *clearCursor = 0;
            clearCursor = clearCursor + 1;
          }
          runtimeNode = inGameRuntime->ownerListHead;
          countOrOwnerOrDelta = inGameRuntime->activeFactionRuntimeIndex;
          if (runtimeNode != (WorldOwnerListNode100 *)0x0) {
            do {
              if (((runtimeNode->ownerClassId == WORLD_OWNER_RUNTIME_MODEL) &&
                  ((runtimeNode->runtimeFlags & 2) != 0)) &&
                 ((entry = *(GameEntityRuntime **)((int)runtimeNode->runtimePayload + 8),
                  (runtimeNode->runtimeFlags & 0x20) != 0 &&
                  (countOrOwnerOrDelta == (entry->common).ownership.ownerIndex)))) {
                payloadValue = (int)entry - (int)g_ArmyRuntimeRebaseBaseMinusOne;
                testResult = WorldRuntimeNode_IsPositionInsideBoundsCf
                                  (runtimeNode,
                                   (WorldRuntimeExtendedMapControlView170 *)inGameRuntime);
                if (testResult) {
                  testResult = SelectionInfo_FindEntryCf(entry);
                  deltaXOrTripletCount = g_InGameSelectionInsertTripletDwordCount;
                  if (testResult) {
                    testResult = InGameCommandQueue_ContainsTripletValueCf(payloadValue,0x55fb90);
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
                  testResult = SelectionInfo_FindEntryCf(entry);
                  deltaXOrTripletCount = g_InGameSelectionRemoveTripletDwordCount;
                  if (!testResult) {
                    testResult = InGameCommandQueue_ContainsTripletValueCf(payloadValue,0x55fc30);
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
            } while (runtimeNode != (WorldOwnerListNode100 *)0x0);
            if (g_InGameSelectionRemoveTripletDwordCount != 0) {
              tripletCursor = (CommandPayloadDword04 *)&g_InGameSelectionRemoveTripletDwords;
              do {
                if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_NETWORKED_MASK) ==
                    SESSION_NETWORK_ROLE_LOCAL) {
                  FrontendPlayerSelection_RemoveThreeEntriesAndRefresh
                            (g_LocalPlayerRuntimeId,tripletCursor[2],tripletCursor[1],*tripletCursor);
                }
                else {
                  InGameCommandQueue_AppendLocalPlayerCommand(0xb00,tripletCursor[2],tripletCursor[1],*tripletCursor);
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
                  InGameCommandQueue_AppendLocalPlayerCommand(0xa60,tripletCursor[2],tripletCursor[1],*tripletCursor);
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
      else if ((pointerValue0 != 0x7fffffff) && ((g_InGamePointerInteractionStateFlags & 3) == 0)) {
        if ((g_CursorButtonState & 4) == 0) {
          /* The original adds the horizontal mouse delta since capture (computed before snapping the
             pointer back) - not the pointer function's return value. */
          countOrOwnerOrDelta = *(int32_t *)(inGameRuntime[1].interaction.reserved00_47 + 0xc) - g_InGameCommandPointerCaptureX;
          g_PointerSetPosition(g_InGameCommandPointerCaptureY,g_InGameCommandPointerCaptureX);
          g_InGameCommandPreviewHeading16 = g_InGameCommandPreviewHeading16 + countOrOwnerOrDelta * 0x40;
          g_InGameCommandPreviewHeading16 = g_InGameCommandPreviewHeading16 & 0xffff;
        }
        else {
          g_InGameCommandPointerCaptureX =
               *(int32_t *)(inGameRuntime[1].interaction.reserved00_47 + 0xc);
          g_InGameCommandPointerCaptureY =
               *(int32_t *)(inGameRuntime[1].interaction.reserved00_47 + 0x10);
        }
      }
    }
    else if (pointerValue0 != 0x7fffffff) {
      if ((g_CursorButtonState & 4) == 0) {
        /* The original adds the horizontal mouse delta since capture (computed before snapping the
           pointer back) - not the pointer function's return value. */
        countOrOwnerOrDelta = *(int32_t *)(inGameRuntime[1].interaction.reserved00_47 + 0xc) - g_InGamePlacementPointerCaptureX;
        g_PointerSetPosition(g_InGamePlacementPointerCaptureY,g_InGamePlacementPointerCaptureX);
        g_InGamePlacementHeading16 = g_InGamePlacementHeading16 + countOrOwnerOrDelta * 0x40;
        g_InGamePlacementHeading16 = g_InGamePlacementHeading16 & 0xffff;
      }
      else {
        g_InGamePlacementPointerCaptureX =
             *(int32_t *)(inGameRuntime[1].interaction.reserved00_47 + 0xc);
        g_InGamePlacementPointerCaptureY =
             *(int32_t *)(inGameRuntime[1].interaction.reserved00_47 + 0x10);
      }
    }
  }
  return;
}


/* Address: 0x005691B0.
   Ownership: gameplay/input/world.
   Purpose: Commits the active pointer action, including selection, command, and shared-tail insertion paths. Typed
   parameters: p0 pointerValue0→InGamePointerCallbackValue0_V344, p1
   pointerValue1→InGamePointerCallbackValue1_V344, p2 pointerValue2→InGamePointerCallbackValue2_V344, p3
   pointerValue3→InGamePointerCallbackValue3_V344. Calling convention, complete VariableStorage serialization,
   function bytes, control flow, globals, locals, and executable data remain unchanged.
   Cross-module calls: InGameCommand_ExecuteLocalPlacementFromSelection [ui/ingame/commands],
   InGameCommandQueue_AppendLocalPlayerCommand [network/protocol/commands],
   SelectionInfo_ValidateOwnerType16AndAnyActiveCf [gameplay/selection/runtime],
   SelectionInfo_CollectAttachmentEffectVariantMask [gameplay/selection/runtime],
   FrontendPlayerSelection_ClearAndRefreshLocalPanels [ui/frontend/player], SelectionInfo_HasAnyEntryCf
   [gameplay/selection/runtime].
*/
void __thandor_void_preserve_eax_ecx_edx
InGameWorldInput_CommitPointerActionCf
          (InGamePointerCallbackValue0 pointerValue0,InGamePointerCallbackValue1 pointerValue1,
          InGamePointerCallbackValue2 pointerValue2,InGamePointerCallbackValue3 pointerValue3,
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
  
  if (((((g_UiCommandRuntimeFlags & 0x101) != 0) ||
       (((inGameRuntime->interaction).interactionFlags48 & 8) != 0)) ||
      ((g_UiCommandRuntimeFlags & 0x100) != 0)) || ((inGameRuntime->runtimeFlags & 0x10) != 0))
  goto InGameWorldInput_ReleasePointerCapture;
  if ((g_UiCommandRuntimeFlags & 0x20) != 0) {
    if (pointerValue0 != 0x7fffffff) {
      if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_NETWORKED_MASK) ==
          SESSION_NETWORK_ROLE_LOCAL) {
        InGameCommand_ExecuteLocalPlacementFromSelection
                  (g_LocalPlayerRuntimeId,g_InGamePlacementHeading16,pointerValue1,pointerValue2);
      }
      else {
        InGameCommandQueue_AppendLocalPlayerCommand
                  (0x13a0,g_InGamePlacementHeading16,pointerValue1,pointerValue2);
      }
    }
    goto InGameWorldInput_ReleasePointerCapture;
  }
  testResult = SelectionInfo_ValidateOwnerType16AndAnyActiveCf(inGameRuntime->activeFactionRuntimeIndex);
  if (!testResult) {
    if (((pointerValue0 != 0x7fffffff) && ((g_UiCommandRuntimeFlags & 0x80) != 0)) &&
       ((g_InGamePointerInteractionStateFlags & 3) == 0)) {
      g_UiCommandRuntimeFlags = g_UiCommandRuntimeFlags & 0xffffff7f;
      modifierModeMask = 0;
      if ((g_KeyboardStateMask & 0x3f) == 0) {
        modifierModeMask = 7;
      }
      if ((g_KeyboardStateMask & 3) != 0) {
        modifierModeMask = modifierModeMask | 1;
      }
      if ((g_KeyboardStateMask & 0x30) != 0) {
        modifierModeMask = modifierModeMask | 2;
      }
      if ((g_KeyboardStateMask & 0xc) != 0) {
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
          modeHandler(g_LocalPlayerRuntimeId,g_InGameCommandPreviewHeading16,pointerValue1,pointerValue2);
        }
        else {
          InGameCommandQueue_AppendLocalPlayerCommand
                    ((UiActionId)((unsigned char *)modeHandler + -0x55f130) /* TODO: code-address command id, see THANDOR_CODE_AT */,g_InGameCommandPreviewHeading16,pointerValue1,
                     pointerValue2);
        }
        if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_NETWORKED_MASK) ==
            SESSION_NETWORK_ROLE_LOCAL) {
          FrontendPlayerSelection_ClearAndRefreshLocalPanels(g_LocalPlayerRuntimeId,0,0,0);
        }
        else {
          InGameCommandQueue_AppendLocalPlayerCommand(0xba0,0,0,0);
        }
      }
    }
    goto InGameWorldInput_ReleasePointerCapture;
  }
  if ((inGameRuntime->runtimeFlags & 0x80) != 0) {
    inGameRuntime->runtimeFlags = inGameRuntime->runtimeFlags & 0xffffff7f;
    goto InGameWorldInput_ReleasePointerCapture;
  }
  ownerIndex = inGameRuntime->activeFactionRuntimeIndex;
  entry = (GameEntityRuntime *)inGameRuntime;
  if (((candidateNode == (WorldOwnerListNode100 *)0x0) ||
      (candidateNode->ownerClassId != WORLD_OWNER_RUNTIME_MODEL)) ||
     ((entry = *(GameEntityRuntime **)((int)candidateNode->runtimePayload + 8),
      (int)pointerValue0 < (int)(pointerValue3 - 0x1000) ||
      ((entry->common).ownership.ownerIndex == 0)))) {
    candidateNode = (WorldOwnerListNode100 *)0x0;
  }
  testResult = SelectionInfo_HasAnyEntryCf();
  if ((!testResult) || (testResult = SelectionInfo_AllEntriesEmptyOrMatchOwnerCf(ownerIndex), testResult)) {
    if (candidateNode != (WorldOwnerListNode100 *)0x0) {
      if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_NETWORKED_MASK) ==
          SESSION_NETWORK_ROLE_LOCAL) {
        FrontendPlayerRuntime_AssignModelTokenAndRefreshSelection
                  (g_LocalPlayerRuntimeId,0,0,(int)entry - (int)g_ArmyRuntimeRebaseBaseMinusOne);
      }
      else {
        InGameCommandQueue_AppendLocalPlayerCommand
                  (0x9a0,0,0,(int)entry - (int)g_ArmyRuntimeRebaseBaseMinusOne);
      }
    }
    goto InGameWorldInput_ReleasePointerCapture;
  }
  if ((g_KeyboardStateMask & 0xc) == 0) {
    if (candidateNode == (WorldOwnerListNode100 *)0x0) {
      testResult = SelectionInfo_TestAnyActiveOrSingleClass13Cf();
      if ((!testResult) && (pointerValue0 != 0x7fffffff)) {
        if ((g_KeyboardStateMask & 0x33) == 0) {
          if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_NETWORKED_MASK) ==
              SESSION_NETWORK_ROLE_LOCAL) {
            InGamePlayerSelection_ApplyPositionCommandVariantB
                      (g_LocalPlayerRuntimeId,0,pointerValue1,pointerValue2);
          }
          else {
            InGameCommandQueue_AppendLocalPlayerCommand(0xd40,0,pointerValue1,pointerValue2);
          }
        }
        else if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_NETWORKED_MASK) ==
                 SESSION_NETWORK_ROLE_LOCAL) {
          InGamePlayerSelection_ApplyPositionCommand
                    (g_LocalPlayerRuntimeId,0,pointerValue1,pointerValue2);
        }
        else {
          InGameCommandQueue_AppendLocalPlayerCommand(0xd70,0,pointerValue1,pointerValue2);
        }
      }
      goto InGameWorldInput_ReleasePointerCapture;
    }
    if ((g_KeyboardStateMask & 0x33) == 0) {
      testResult = SelectionInfo_TestAnyStateField100NonnegativeCf();
      if (testResult) {
        testResult = GameFactionRuntime_TestCapabilityBitClearCf
                          ((entry->common).ownership.ownerIndex,ownerIndex);
        if (testResult) goto InGameWorldInput_SelectCandidateArmy;
      }
      else {
InGameWorldInput_TestCandidateCapability:
        testResult = GameFactionRuntime_TestCapabilityBitClearCf
                          ((entry->common).ownership.ownerIndex,ownerIndex);
        if (testResult) goto InGameWorldInput_ReleasePointerCapture;
        scaleRatio = ModelRuntime_QueryHierarchyScaleRatioQ12Regs((RuntimeModelFactionPrefix10 *)entry);
        if ((int)scaleRatio != (int)(scaleRatio >> 0x20)) goto InGameWorldInput_SelectCandidateArmy;
      }
      if (ownerIndex == (entry->common).ownership.ownerIndex) {
        armyRuntimeIndex = (int)entry - (int)g_ArmyRuntimeRebaseBaseMinusOne;
        if ((inGameRuntime->runtimeFlags & 0x8000000) == 0) {
          if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_NETWORKED_MASK) ==
              SESSION_NETWORK_ROLE_LOCAL) {
            FrontendPlayerRuntime_AssignModelTokenAndRefreshSelection
                      (g_LocalPlayerRuntimeId,0,0,armyRuntimeIndex);
          }
          else {
            InGameCommandQueue_AppendLocalPlayerCommand(0x9a0,0,0,armyRuntimeIndex);
          }
        }
        else if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_NETWORKED_MASK) ==
                 SESSION_NETWORK_ROLE_LOCAL) {
          InGamePlayerSelection_ReplaceWithArmyRuntimeIndex
                    (g_LocalPlayerRuntimeId,0,0,armyRuntimeIndex);
        }
        else {
          InGameCommandQueue_AppendLocalPlayerCommand(0xa00,0,0,armyRuntimeIndex);
        }
      }
      goto InGameWorldInput_ReleasePointerCapture;
    }
  }
  else if ((g_KeyboardStateMask & 0x33) == 0) {
    if (candidateNode == (WorldOwnerListNode100 *)0x0) {
      if (pointerValue0 != 0x7fffffff) {
        variantMaskOrSurfaceHeight = WorldRuntime_InterpolateTopSurfaceHeightOrSentinel
                          (pointerValue1,pointerValue2,inGameRuntime);
        /* The original passes the same EDX/ECX point on (lost locals in the decompilation). */
        payloadDword08 = pointerValue1;
        payloadDword0C = pointerValue2;
        if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_NETWORKED_MASK) ==
            SESSION_NETWORK_ROLE_LOCAL) {
          InGamePlayerSelection_ApplyTargetPositionCommand
                    (g_LocalPlayerRuntimeId,variantMaskOrSurfaceHeight,payloadDword08,payloadDword0C);
        }
        else {
          InGameCommandQueue_AppendLocalPlayerCommand(0xde0,variantMaskOrSurfaceHeight,payloadDword08,payloadDword0C);
        }
      }
      goto InGameWorldInput_ReleasePointerCapture;
    }
    testResult = SelectionInfo_TestAnyStateField100NonnegativeCf();
    if (!testResult) goto InGameWorldInput_TestCandidateCapability;
    testResult = GameFactionRuntime_TestCapabilityBitClearCf
                      ((entry->common).ownership.ownerIndex,ownerIndex);
    if ((!testResult) && (testResult = SelectionInfo_FindEntryCf(entry), !testResult))
    goto InGameWorldInput_ReleasePointerCapture;
InGameWorldInput_SelectCandidateArmy:
    if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_NETWORKED_MASK) ==
        SESSION_NETWORK_ROLE_LOCAL) {
      InGamePlayerSelection_SelectArmyRuntimeIndex
                (g_LocalPlayerRuntimeId,0,0,(int)entry - (int)g_ArmyRuntimeRebaseBaseMinusOne);
    }
    else {
      InGameCommandQueue_AppendLocalPlayerCommand
                (0xda0,0,0,(int)entry - (int)g_ArmyRuntimeRebaseBaseMinusOne);
    }
    goto InGameWorldInput_ReleasePointerCapture;
  }
  if (candidateNode != (WorldOwnerListNode100 *)0x0) {
    if (ownerIndex == (entry->common).ownership.ownerIndex) {
      testResult = SelectionInfo_FindEntryCf(entry);
      if (testResult) {
        if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_NETWORKED_MASK) ==
            SESSION_NETWORK_ROLE_LOCAL) {
          FrontendPlayerSelection_InsertThreeEntriesAndRefresh
                    (g_LocalPlayerRuntimeId,0,0,(int)entry - (int)g_ArmyRuntimeRebaseBaseMinusOne);
        }
        else {
          InGameCommandQueue_AppendLocalPlayerCommand
                    (0xa60,0,0,(int)entry - (int)g_ArmyRuntimeRebaseBaseMinusOne);
        }
      }
      else if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_NETWORKED_MASK) ==
               SESSION_NETWORK_ROLE_LOCAL) {
        FrontendPlayerSelection_RemoveThreeEntriesAndRefresh
                  (g_LocalPlayerRuntimeId,0,0,(int)entry - (int)g_ArmyRuntimeRebaseBaseMinusOne);
      }
      else {
        InGameCommandQueue_AppendLocalPlayerCommand
                  (0xb00,0,0,(int)entry - (int)g_ArmyRuntimeRebaseBaseMinusOne);
      }
    }
    else if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_NETWORKED_MASK) ==
             SESSION_NETWORK_ROLE_LOCAL) {
      FrontendPlayerRuntime_AssignModelTokenAndRefreshSelection
                (g_LocalPlayerRuntimeId,0,0,(int)entry - (int)g_ArmyRuntimeRebaseBaseMinusOne);
    }
    else {
      InGameCommandQueue_AppendLocalPlayerCommand
                (0x9a0,0,0,(int)entry - (int)g_ArmyRuntimeRebaseBaseMinusOne);
    }
  }
InGameWorldInput_ReleasePointerCapture:
  g_InGamePointerInteractionStateFlags = g_InGamePointerInteractionStateFlags & 0xfffffffd;
  return;
}


/* Address: 0x0056F230.
   Ownership: gameplay/input/world.
   Purpose: Handles in game camera command dispatch by code and modifier flags carry-flag result.
   Cross-module calls: WorldRuntime_SetPosition60AndDistanceFromPosition80 [world/runtime/core],
   WorldRuntime_SetMotionParameters6CThrough78Clamped [world/runtime/core],
   WorldRuntime_RecomputeMotionEndpointAgainstFieldSurface [world/runtime/core], WorldRuntime_CommitScalar7CFrom8C
   [world/runtime/core].
*/
void __thandor_void_preserve_eax_ecx_edx
InGameCameraCommand_DispatchByCodeAndModifierFlagsCf
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
     matches when none of the 0x3C modifiers is held. The key-code 0 record terminates the table. */
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
    if ((requiredModifiers == 0) ? ((modifierFlags & 0x3c) == 0) : ((modifierFlags & requiredModifiers) != 0))
    break;
  }
  /* The original jumps to the record's continuation address; the cases are those addresses. */
  switch(currentRecord->records[0].continuationEntryAddress) {
  case 0x56f360:
    WorldRuntime_SetPosition60AndDistanceFromPosition80
              (g_LevelCameraBookmark1PositionZQ12,g_LevelCameraBookmark1PositionYQ12,
               g_LevelCameraBookmark1PositionXQ12,worldRuntime);
    WorldRuntime_SetMotionParameters6CThrough78Clamped
              (2,(int)bookmark1PackedAngles >> 0x10,bookmark1PackedAngles & 0xffff,g_LevelCameraBookmark1PositionMagnitudeQ12,
               worldRuntime);
    WorldRuntime_RecomputeMotionEndpointAgainstFieldSurface(worldRuntime);
    WorldRuntime_CommitScalar7CFrom8C(worldRuntime);
    break;
  case 0x56f3b0:
    WorldRuntime_SetPosition60AndDistanceFromPosition80
              (g_LevelCameraBookmark2PositionZQ12,g_LevelCameraBookmark2PositionYQ12,
               g_LevelCameraBookmark2PositionXQ12,worldRuntime);
    WorldRuntime_SetMotionParameters6CThrough78Clamped
              (2,(int)bookmark2PackedAngles >> 0x10,bookmark2PackedAngles & 0xffff,g_LevelCameraBookmark2PositionMagnitudeQ12,
               worldRuntime);
    WorldRuntime_RecomputeMotionEndpointAgainstFieldSurface(worldRuntime);
    WorldRuntime_CommitScalar7CFrom8C(worldRuntime);
    break;
  case 0x56f400:
    WorldRuntime_SetPosition60AndDistanceFromPosition80
              (g_LevelCameraBookmark3PositionZQ12,g_LevelCameraBookmark3PositionYQ12,
               g_LevelCameraBookmark3PositionXQ12,worldRuntime);
    WorldRuntime_SetMotionParameters6CThrough78Clamped
              (2,(int)bookmark3PackedAngles >> 0x10,bookmark3PackedAngles & 0xffff,g_LevelCameraBookmark3PositionMagnitudeQ12,
               worldRuntime);
    WorldRuntime_RecomputeMotionEndpointAgainstFieldSurface(worldRuntime);
    WorldRuntime_CommitScalar7CFrom8C(worldRuntime);
    break;
  case 0x56f450:
    WorldRuntime_SetPosition60AndDistanceFromPosition80
              (g_LevelCameraBookmark4PositionZQ12,g_LevelCameraBookmark4PositionYQ12,
               g_LevelCameraBookmark4PositionXQ12,worldRuntime);
    WorldRuntime_SetMotionParameters6CThrough78Clamped
              (2,(int)bookmark4PackedAngles >> 0x10,bookmark4PackedAngles & 0xffff,g_LevelCameraBookmark4PositionMagnitudeQ12,
               worldRuntime);
    WorldRuntime_RecomputeMotionEndpointAgainstFieldSurface(worldRuntime);
    WorldRuntime_CommitScalar7CFrom8C(worldRuntime);
    break;
  case 0x56f4a0:
    WorldRuntime_SetPosition60AndDistanceFromPosition80
              (g_LevelCameraBookmark5PositionZQ12,g_LevelCameraBookmark5PositionYQ12,
               g_LevelCameraBookmark5PositionXQ12,worldRuntime);
    WorldRuntime_SetMotionParameters6CThrough78Clamped
              (2,(int)bookmark5PackedAngles >> 0x10,bookmark5PackedAngles & 0xffff,g_LevelCameraBookmark5PositionMagnitudeQ12,
               worldRuntime);
    WorldRuntime_RecomputeMotionEndpointAgainstFieldSurface(worldRuntime);
    WorldRuntime_CommitScalar7CFrom8C(worldRuntime);
    break;
  case 0x56f4f0:
    WorldRuntime_SetPosition60AndDistanceFromPosition80
              (g_LevelCameraBookmark6PositionZQ12,g_LevelCameraBookmark6PositionYQ12,
               g_LevelCameraBookmark6PositionXQ12,worldRuntime);
    WorldRuntime_SetMotionParameters6CThrough78Clamped
              (2,(int)bookmark6PackedAngles >> 0x10,bookmark6PackedAngles & 0xffff,g_LevelCameraBookmark6PositionMagnitudeQ12,
               worldRuntime);
    WorldRuntime_RecomputeMotionEndpointAgainstFieldSurface(worldRuntime);
    WorldRuntime_CommitScalar7CFrom8C(worldRuntime);
    break;
  case 0x56f540:
    WorldRuntime_SetPosition60AndDistanceFromPosition80
              (g_LevelCameraBookmark7PositionZQ12,g_LevelCameraBookmark7PositionYQ12,
               g_LevelCameraBookmark7PositionXQ12,worldRuntime);
    WorldRuntime_SetMotionParameters6CThrough78Clamped
              (2,(int)bookmark7PackedAngles >> 0x10,bookmark7PackedAngles & 0xffff,g_LevelCameraBookmark7PositionMagnitudeQ12,
               worldRuntime);
    WorldRuntime_RecomputeMotionEndpointAgainstFieldSurface(worldRuntime);
    WorldRuntime_CommitScalar7CFrom8C(worldRuntime);
    break;
  case 0x56f590:
    g_LevelCameraBookmark1PositionXQ12 = (worldRuntime->motion).positionXQ12;
    g_LevelCameraBookmark1PositionYQ12 = (worldRuntime->motion).positionYQ12;
    g_LevelCameraBookmark1PositionZQ12 = (worldRuntime->motion).positionZQ12;
    g_LevelCameraBookmark1PositionMagnitudeQ12 = (worldRuntime->motion).positionMagnitudeQ12;
    g_LevelCameraBookmark1PackedHeadingLow16PitchHigh16 =
         (worldRuntime->motion).pitchAngle << 0x10 | (worldRuntime->motion).headingAngle;
    break;
  case 0x56f5e0:
    g_LevelCameraBookmark2PositionXQ12 = (worldRuntime->motion).positionXQ12;
    g_LevelCameraBookmark2PositionYQ12 = (worldRuntime->motion).positionYQ12;
    g_LevelCameraBookmark2PositionZQ12 = (worldRuntime->motion).positionZQ12;
    g_LevelCameraBookmark2PositionMagnitudeQ12 = (worldRuntime->motion).positionMagnitudeQ12;
    g_LevelCameraBookmark2PackedHeadingLow16PitchHigh16 =
         (worldRuntime->motion).pitchAngle << 0x10 | (worldRuntime->motion).headingAngle;
    break;
  case 0x56f630:
    g_LevelCameraBookmark3PositionXQ12 = (worldRuntime->motion).positionXQ12;
    g_LevelCameraBookmark3PositionYQ12 = (worldRuntime->motion).positionYQ12;
    g_LevelCameraBookmark3PositionZQ12 = (worldRuntime->motion).positionZQ12;
    g_LevelCameraBookmark3PositionMagnitudeQ12 = (worldRuntime->motion).positionMagnitudeQ12;
    g_LevelCameraBookmark3PackedHeadingLow16PitchHigh16 =
         (worldRuntime->motion).pitchAngle << 0x10 | (worldRuntime->motion).headingAngle;
    break;
  case 0x56f680:
    g_LevelCameraBookmark4PositionXQ12 = (worldRuntime->motion).positionXQ12;
    g_LevelCameraBookmark4PositionYQ12 = (worldRuntime->motion).positionYQ12;
    g_LevelCameraBookmark4PositionZQ12 = (worldRuntime->motion).positionZQ12;
    g_LevelCameraBookmark4PositionMagnitudeQ12 = (worldRuntime->motion).positionMagnitudeQ12;
    g_LevelCameraBookmark4PackedHeadingLow16PitchHigh16 =
         (worldRuntime->motion).pitchAngle << 0x10 | (worldRuntime->motion).headingAngle;
    break;
  case 0x56f6d0:
    g_LevelCameraBookmark5PositionXQ12 = (worldRuntime->motion).positionXQ12;
    g_LevelCameraBookmark5PositionYQ12 = (worldRuntime->motion).positionYQ12;
    g_LevelCameraBookmark5PositionZQ12 = (worldRuntime->motion).positionZQ12;
    g_LevelCameraBookmark5PositionMagnitudeQ12 = (worldRuntime->motion).positionMagnitudeQ12;
    g_LevelCameraBookmark5PackedHeadingLow16PitchHigh16 =
         (worldRuntime->motion).pitchAngle << 0x10 | (worldRuntime->motion).headingAngle;
    break;
  case 0x56f720:
    g_LevelCameraBookmark6PositionXQ12 = (worldRuntime->motion).positionXQ12;
    g_LevelCameraBookmark6PositionYQ12 = (worldRuntime->motion).positionYQ12;
    g_LevelCameraBookmark6PositionZQ12 = (worldRuntime->motion).positionZQ12;
    g_LevelCameraBookmark6PositionMagnitudeQ12 = (worldRuntime->motion).positionMagnitudeQ12;
    g_LevelCameraBookmark6PackedHeadingLow16PitchHigh16 =
         (worldRuntime->motion).pitchAngle << 0x10 | (worldRuntime->motion).headingAngle;
    break;
  case 0x56f770:
    g_LevelCameraBookmark7PositionXQ12 = (worldRuntime->motion).positionXQ12;
    g_LevelCameraBookmark7PositionYQ12 = (worldRuntime->motion).positionYQ12;
    g_LevelCameraBookmark7PositionZQ12 = (worldRuntime->motion).positionZQ12;
    g_LevelCameraBookmark7PositionMagnitudeQ12 = (worldRuntime->motion).positionMagnitudeQ12;
    g_LevelCameraBookmark7PackedHeadingLow16PitchHigh16 =
         (worldRuntime->motion).pitchAngle << 0x10 | (worldRuntime->motion).headingAngle;
    break;
  case 0x56f7d0:
    worldRuntime->runtimeFlags = worldRuntime->runtimeFlags ^ 0x20000;
    break;
  case 0x56f7e0:
    worldRuntime->runtimeFlags = worldRuntime->runtimeFlags ^ 0x40000;
  }
  return;
}


/* Address: 0x0056D4B0.
   Ownership: gameplay/input/world.
   Purpose: Cancels targeting-context state 0x1B by resetting the context state, finding the root targeting object
   at +0xA30, clearing its active bit 0x10, and restoring the saved targeting fields. Queued UI action handler for
   INGAME_PAGE10[14] (0x100E). Return datatype is preserved for non-queue direct callers.
   Cross-module calls: WorldRuntime_RestoreMotionStateFromSnapshot [world/runtime/core].
*/
void __thandor_preserve_eax
InGameTargetingContext_CancelAndRestoreState(InGameTargetingRootTraversalView9E60 *targetingContext)

{
  WorldRuntimeFlags *runtimeFlagsField;
  UiNodeBase *parentCursor;
  
  if (targetingContext->actionState == INGAME_TARGETING_OBSERVED_CANCEL_AND_RESTORE) {
    targetingContext->actionState = INGAME_TARGETING_OBSERVED_IDLE;
    parentCursor = (targetingContext->base).parent;
    while (parentCursor != (UiNodeBase *)0xffffffff) {
      targetingContext = (InGameTargetingRootTraversalView9E60 *)(targetingContext->base).parent;
      parentCursor = (targetingContext->base).parent;
    }
    runtimeFlagsField = &(targetingContext->worldRuntime0A30).runtimeFlags;
    *runtimeFlagsField = *runtimeFlagsField & 0xffffffef;
    WorldRuntime_RestoreMotionStateFromSnapshot(&targetingContext->worldRuntime0A30);
  }
  return;
}


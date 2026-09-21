/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/src/gameplay/input/world.c
 * Reverse engineering by idkFoxes 2026
 */

#include <thandor/gameplay/input/world.h>

/* Implementation ownership: gameplay/input/world. */

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
  WorldRuntimeFlags *pWVar1;
  UiNodeBase *pUVar2;
  InGameNotificationPayloadKind IVar3;
  Q12 QVar4;
  WorldOwnerListNode100 *pWVar5;
  int iVar6;
  CommandPayloadDword04 modelToken;
  CommandPayloadDword08 armyToken;
  FieldGridNearestPointRegsCf13 FVar7;
  
  if (targetingContext->actionState == INGAME_TARGETING_OBSERVED_CANCEL_AND_RESTORE) {
    InGameTargetingContext_CancelAndRestoreState(targetingContext);
  }
  else if (targetingContext->actionState == INGAME_TARGETING_OBSERVED_ADVANCE_OR_RESOLVE) {
    pUVar2 = (targetingContext->base).parent;
    while (pUVar2 != (UiNodeBase *)0xffffffff) {
      targetingContext = (InGameTargetingRootTraversalView9E60 *)(targetingContext->base).parent;
      pUVar2 = (targetingContext->base).parent;
    }
    IVar3 = (targetingContext->activeNotificationPayload9E40).payloadKind14;
    if (((targetingContext->worldRuntime0A30).runtimeFlags & 0x10) == 0) {
      WorldRuntime_CaptureMotionStateToSnapshot(&targetingContext->worldRuntime0A30);
    }
                    // WARNING: Switch is manually overridden
    switch(IVar3) {
    case TECHNOLOGY_UNLOCK_POSITION:
      for (pWVar5 = (targetingContext->worldRuntime0A30).ownerListHead;
          pWVar5 != (WorldOwnerListNode100 *)0x0; pWVar5 = pWVar5->nextNode) {
        if ((((pWVar5->ownerClassId == WORLD_OWNER_RUNTIME_MODEL) &&
             (pWVar5->worldXQ12 ==
              (targetingContext->activeNotificationPayload9E40).primaryWorldCoordinateQ12_00)) &&
            (iVar6 = *(int *)((int)pWVar5->runtimePayload + 8),
            pWVar5->worldYQ12 ==
            (targetingContext->activeNotificationPayload9E40).secondaryWorldCoordinateQ12_04)) &&
           ((targetingContext->worldRuntime0A30).activeFactionRuntimeIndex == *(int *)(iVar6 + 0xc))
           ) {
          modelToken = iVar6 - (int)g_ArmyRuntimeRebaseBaseMinusOne;
          armyToken = (int)pWVar5->runtimePayload - g_ModelRuntimeRebaseDelta;
          if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_NETWORKED_MASK) ==
              SESSION_NETWORK_ROLE_LOCAL) {
            FrontendPlayerRuntime_AssignModelAndArmyTokensAndRefreshLocalPanel
                      (g_LocalPlayerRuntimeId,0,armyToken,modelToken);
          }
          else {
            InGameCommandQueue_AppendLocalPlayerCommand(0x1620,0,armyToken,modelToken);
          }
          targetingContext->sessionNotificationInteractionState9B4C = NONE;
          return;
        }
      }
      break;
    case FACTION_IMPACT_ANCHOR:
      QVar4 = (targetingContext->activeNotificationPayload9E40).secondaryWorldCoordinateQ12_04;
      targetingContext->targetingPrimaryWorldCoordinateQ12_9E58 =
           (targetingContext->activeNotificationPayload9E40).primaryWorldCoordinateQ12_00;
      targetingContext->targetingSecondaryWorldCoordinateQ12_9E5C = QVar4;
    case ARMY_CREATED:
      FVar7 = FieldGrid_GetNearestTerrainPoint
                        ((targetingContext->activeNotificationPayload9E40).
                         secondaryWorldCoordinateQ12_04,
                         (targetingContext->activeNotificationPayload9E40).
                         primaryWorldCoordinateQ12_00,(targetingContext->worldRuntime0A30).fieldGrid
                        );
      WorldRuntime_SetPosition80AndRebuildPosition60FromAngles
                ((targetingContext->worldRuntime0A30).motion.pitchAngle,
                 (targetingContext->activeNotificationPayload9E40).primaryOrientationAngle08,
                 (targetingContext->worldRuntime0A30).motion.targetDistanceQ12,FVar7.edx,
                 (targetingContext->activeNotificationPayload9E40).secondaryWorldCoordinateQ12_04,
                 (targetingContext->activeNotificationPayload9E40).primaryWorldCoordinateQ12_00,
                 &targetingContext->worldRuntime0A30);
      pWVar1 = &(targetingContext->worldRuntime0A30).runtimeFlags;
      *pWVar1 = *pWVar1 & 0xffffffef;
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
dword InGameWorldInput_ResolveContextActionAndCursorCf
                (InGamePointerCallbackValue0 pointerValue0,InGamePointerCallbackValue1 pointerValue1
                ,InGamePointerCallbackValue2 pointerValue2,InGamePointerCallbackValue3 pointerValue3
                ,WorldOwnerListNode100 *candidateNode,WorldRuntimeContext *inGameRuntime)

{
  dword dVar1;
  int ownerIndex;
  uint uVar2;
  GameEntityRuntime *entry;
  bool bVar3;
  ModelRuntimeScaleRatioRegisterPairQ12 MVar4;
  
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
    dVar1 = 0x2e;
    bVar3 = ArmyPlacement_ValidateAssetAtPointAndCellCornersCf
                      (0,g_InGamePlacementHeading16,pointerValue1,pointerValue2,
                       g_UiHoverSelectionRecord->armyAssetId,
                       inGameRuntime->activeFactionRuntimeIndex,inGameRuntime);
    if (!bVar3) {
      dVar1 = 0x2d;
    }
    InGameSelectionDetailPanel_Rebuild();
    return dVar1;
  }
  bVar3 = SelectionInfo_ValidateOwnerType16AndAnyActiveCf(ownerIndex);
  if (!bVar3) {
    g_InGamePointerInteractionStateFlags = g_InGamePointerInteractionStateFlags & 0xfffffffe;
    if ((((candidateNode != (WorldOwnerListNode100 *)0x0) &&
         (candidateNode->ownerClassId == WORLD_OWNER_RUNTIME_MODEL)) &&
        ((int)(pointerValue3 - 0x1000) <= (int)pointerValue0)) &&
       (bVar3 = GameFactionRuntime_TestCapabilityBitClearCf
                          (*(dword *)(*(int *)((int)candidateNode->runtimePayload + 8) + 0xc),
                           ownerIndex), !bVar3)) {
      g_InGamePointerInteractionStateFlags = g_InGamePointerInteractionStateFlags | 1;
      return 0x15;
    }
    uVar2 = 0;
    if ((g_KeyboardStateMask & 0x3f) == 0) {
      uVar2 = 7;
    }
    if ((g_KeyboardStateMask & 3) != 0) {
      uVar2 = uVar2 | 1;
    }
    if ((g_KeyboardStateMask & 0x30) != 0) {
      uVar2 = uVar2 | 2;
    }
    if ((g_KeyboardStateMask & 0xc) != 0) {
      uVar2 = uVar2 | 4;
    }
    dVar1 = SelectionInfo_CollectAttachmentEffectVariantMask();
    g_InGameCommandPreviewWorldYQ12 = pointerValue2;
    g_InGameCommandPreviewWorldXQ12 = pointerValue1;
    g_InGameCommandPreviewSurfaceHeightQ12OrSentinel = pointerValue0;
    g_InGameCommandPreviewArmyAssetId = *(undefined4 *)((uVar2 & dVar1) * 4 + 0x563788);
    return *(dword *)((uVar2 & dVar1) * 4 + 0x563768);
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
  bVar3 = SelectionInfo_HasAnyEntryCf();
  if (!bVar3) {
InGameWorldInput_ReturnCandidateOwnershipCursor:
    dVar1 = 0;
    if ((candidateNode != (WorldOwnerListNode100 *)0x0) &&
       (dVar1 = 0x15, ownerIndex != (entry->common).ownership.ownerIndex)) {
      dVar1 = 0x16;
    }
    return dVar1;
  }
  bVar3 = SelectionInfo_AllEntriesEmptyOrMatchOwnerCf(ownerIndex);
  if (bVar3) {
    if (candidateNode == (WorldOwnerListNode100 *)0x0) {
      return 0;
    }
    goto InGameWorldInput_ReturnCandidateOwnershipCursor;
  }
  if ((g_KeyboardStateMask & 0xc) == 0) {
    if (candidateNode == (WorldOwnerListNode100 *)0x0) {
      bVar3 = SelectionInfo_TestAnyActiveOrSingleClass13Cf();
      if (bVar3) {
        return 0;
      }
      bVar3 = SelectionInfo_TestPositionCommandAtWorldPointCf
                        (pointerValue1,pointerValue2,inGameRuntime);
      if (bVar3) {
        return 0x18;
      }
      if (pointerValue0 == 0x7fffffff) {
        return 0x18;
      }
      return 0x17;
    }
    if ((g_KeyboardStateMask & 0x33) != 0) {
InGameWorldInput_ReturnModifiedCandidateCursor:
      if (candidateNode == (WorldOwnerListNode100 *)0x0) {
        return 0;
      }
      if (ownerIndex == (entry->common).ownership.ownerIndex) {
        return 0x15;
      }
      return 0x16;
    }
    bVar3 = SelectionInfo_TestAnyStateField100NonnegativeCf();
    if (bVar3) {
      bVar3 = GameFactionRuntime_TestCapabilityBitClearCf
                        ((entry->common).ownership.ownerIndex,ownerIndex);
      if (bVar3) goto InGameWorldInput_ClassifySelectedStateCommand;
      goto InGameWorldInput_ReturnCandidateOwnerCursor;
    }
  }
  else {
    if ((g_KeyboardStateMask & 0x33) != 0) goto InGameWorldInput_ReturnModifiedCandidateCursor;
    bVar3 = SelectionInfo_TestAnyStateField100NonnegativeCf();
    if (bVar3) {
InGameWorldInput_ClassifySelectedStateCommand:
      bVar3 = SelectionInfo_TestAllStateField100NonpositiveCf();
      if (bVar3) {
        return 0;
      }
      if (candidateNode == (WorldOwnerListNode100 *)0x0) {
        return 0x19;
      }
      bVar3 = GameFactionRuntime_TestCapabilityBitClearCf
                        ((entry->common).ownership.ownerIndex,ownerIndex);
      if (bVar3) {
        return 0x19;
      }
      bVar3 = SelectionInfo_FindEntryCf(entry);
      if (bVar3) {
        return 0x1a;
      }
      return 0;
    }
  }
  if (candidateNode == (WorldOwnerListNode100 *)0x0) {
    return 0;
  }
  bVar3 = GameFactionRuntime_TestCapabilityBitClearCf
                    ((entry->common).ownership.ownerIndex,ownerIndex);
  if (bVar3) {
    return 0x1a;
  }
  MVar4 = ModelRuntime_QueryHierarchyScaleRatioQ12Regs((RuntimeModelFactionPrefix10 *)entry);
  if ((int)MVar4 != (int)(MVar4 >> 0x20)) {
    bVar3 = SelectionInfo_FindEntryCf(entry);
    if (bVar3) {
      return 0x19;
    }
    return 0;
  }
InGameWorldInput_ReturnCandidateOwnerCursor:
  if (ownerIndex != (entry->common).ownership.ownerIndex) {
    return 0x16;
  }
  return 0x15;
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
          (InGamePointerCallbackValue0 pointerValue0,dword pointerValue1,dword pointerValue2,
          InGamePointerCallbackValue3 pointerValue3,WorldOwnerListNode100 *candidateNode,
          WorldRuntimeContext *inGameRuntime)

{
  FactionRuntimeIndex ownerIndex;
  int iVar1;
  CommandPayloadDword04 modelToken;
  bool bVar2;
  
  if (((g_UiCommandRuntimeFlags & 0x101) == 0) &&
     (inGameRuntime->runtimeFlags = inGameRuntime->runtimeFlags & 0xf7ffffff,
     ((inGameRuntime->interaction).interactionFlags48 & 8) == 0)) {
    if (((inGameRuntime->interaction).interactionFlags48 & 0x80) != 0) {
      inGameRuntime->runtimeFlags = inGameRuntime->runtimeFlags | 0x8000000;
    }
    ownerIndex = inGameRuntime->activeFactionRuntimeIndex;
    if ((inGameRuntime->runtimeFlags & 0x10) == 0) {
      if ((g_UiCommandRuntimeFlags & 0x20) == 0) {
        bVar2 = SelectionInfo_ValidateOwnerType16AndAnyActiveCf(ownerIndex);
        if (bVar2) {
          g_InGamePointerInteractionStateFlags = g_InGamePointerInteractionStateFlags | 2;
        }
        else {
          if ((((candidateNode != (WorldOwnerListNode100 *)0x0) &&
               (candidateNode->ownerClassId == WORLD_OWNER_RUNTIME_MODEL)) &&
              (iVar1 = *(int *)((int)candidateNode->runtimePayload + 8),
              (int)(pointerValue3 - 0x1000) <= (int)pointerValue0)) &&
             (bVar2 = GameFactionRuntime_TestCapabilityBitClearCf
                                (*(dword *)(iVar1 + 0xc),ownerIndex), !bVar2)) {
            modelToken = iVar1 - (int)g_ArmyRuntimeRebaseBaseMinusOne;
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
               *(undefined4 *)(inGameRuntime[1].interaction.reserved00_47 + 4);
          g_InGameCommandPointerCaptureY =
               *(undefined4 *)(inGameRuntime[1].interaction.reserved00_47 + 8);
          g_UiCommandRuntimeFlags = g_UiCommandRuntimeFlags | 0x80;
        }
      }
      else {
        g_InGamePlacementPointerCaptureX =
             *(undefined4 *)(inGameRuntime[1].interaction.reserved00_47 + 4);
        g_InGamePlacementPointerCaptureY =
             *(undefined4 *)(inGameRuntime[1].interaction.reserved00_47 + 8);
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
          (InGamePointerCallbackValue0 pointerValue0,dword pointerValue1,dword pointerValue2,
          dword pointerValue3,WorldOwnerListNode100 *candidateNode,
          WorldRuntimeContext *inGameRuntime)

{
  uint uVar1;
  int iVar2;
  uint uVar3;
  InGameCommandPayloadTripletValue32 payloadValue;
  undefined4 *puVar4;
  WorldOwnerListNode100 *runtimeNode;
  CommandPayloadDword04 *pCVar5;
  bool bVar6;
  GameEntityRuntime *entry;
  
  if (((((g_UiCommandRuntimeFlags & 0x101) == 0) &&
       (((inGameRuntime->interaction).interactionFlags48 & 8) == 0)) &&
      ((g_UiCommandRuntimeFlags & 0x100) == 0)) && ((inGameRuntime->runtimeFlags & 0x10) == 0)) {
    if ((g_UiCommandRuntimeFlags & 0x20) == 0) {
      bVar6 = SelectionInfo_ValidateOwnerType16AndAnyActiveCf
                        (inGameRuntime->activeFactionRuntimeIndex);
      if (bVar6) {
        if ((inGameRuntime->runtimeFlags & 0x80) == 0) {
          uVar1 = *(int *)(inGameRuntime[1].interaction.reserved00_47 + 4) -
                  *(int *)(inGameRuntime[1].interaction.reserved00_47 + 0xc);
          if ((int)uVar1 < 0) {
            uVar1 = -uVar1;
          }
          uVar3 = *(int *)(inGameRuntime[1].interaction.reserved00_47 + 8) -
                  *(int *)(inGameRuntime[1].interaction.reserved00_47 + 0x10);
          if ((int)uVar3 < 0) {
            uVar3 = -uVar3;
          }
          if ((0x17 < uVar1) || (0x17 < uVar3)) {
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
          puVar4 = (undefined4 *)&g_InGameSelectionInsertTripletDwords;
          for (iVar2 = 0x1a; iVar2 != 0; iVar2 = iVar2 + -1) {
            *puVar4 = 0;
            puVar4 = puVar4 + 1;
          }
          runtimeNode = inGameRuntime->ownerListHead;
          iVar2 = inGameRuntime->activeFactionRuntimeIndex;
          if (runtimeNode != (WorldOwnerListNode100 *)0x0) {
            do {
              if (((runtimeNode->ownerClassId == WORLD_OWNER_RUNTIME_MODEL) &&
                  ((runtimeNode->runtimeFlags & 2) != 0)) &&
                 ((entry = *(GameEntityRuntime **)((int)runtimeNode->runtimePayload + 8),
                  (runtimeNode->runtimeFlags & 0x20) != 0 &&
                  (iVar2 == (entry->common).ownership.ownerIndex)))) {
                payloadValue = (int)entry - (int)g_ArmyRuntimeRebaseBaseMinusOne;
                bVar6 = WorldRuntimeNode_IsPositionInsideBoundsCf
                                  (runtimeNode,
                                   (WorldRuntimeExtendedMapControlView170 *)inGameRuntime);
                if (bVar6) {
                  bVar6 = SelectionInfo_FindEntryCf(entry);
                  uVar1 = g_InGameSelectionInsertTripletDwordCount;
                  if (bVar6) {
                    bVar6 = InGameCommandQueue_ContainsTripletValueCf(payloadValue,0x55fb90);
                    if ((!bVar6) &&
                       (*(InGameCommandPayloadTripletValue32 *)
                         (&g_InGameSelectionInsertTripletDwords + uVar1 * 4) = payloadValue,
                       uVar1 < 0xb)) {
                      g_InGameSelectionInsertTripletDwordCount =
                           g_InGameSelectionInsertTripletDwordCount + 1;
                    }
                  }
                }
                else {
                  bVar6 = SelectionInfo_FindEntryCf(entry);
                  uVar1 = g_InGameSelectionRemoveTripletDwordCount;
                  if (!bVar6) {
                    bVar6 = InGameCommandQueue_ContainsTripletValueCf(payloadValue,0x55fc30);
                    if ((!bVar6) &&
                       (*(InGameCommandPayloadTripletValue32 *)
                         (&g_InGameSelectionRemoveTripletDwords + uVar1 * 4) = payloadValue,
                       uVar1 < 0xb)) {
                      g_InGameSelectionRemoveTripletDwordCount =
                           g_InGameSelectionRemoveTripletDwordCount + 1;
                    }
                  }
                }
              }
              runtimeNode = runtimeNode->nextNode;
            } while (runtimeNode != (WorldOwnerListNode100 *)0x0);
            if (g_InGameSelectionRemoveTripletDwordCount != 0) {
              pCVar5 = (CommandPayloadDword04 *)&g_InGameSelectionRemoveTripletDwords;
              do {
                if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_NETWORKED_MASK) ==
                    SESSION_NETWORK_ROLE_LOCAL) {
                  FrontendPlayerSelection_RemoveThreeEntriesAndRefresh
                            (g_LocalPlayerRuntimeId,pCVar5[2],pCVar5[1],*pCVar5);
                }
                else {
                  InGameCommandQueue_AppendLocalPlayerCommand(0xb00,pCVar5[2],pCVar5[1],*pCVar5);
                }
                uVar1 = g_InGameSelectionRemoveTripletDwordCount;
                pCVar5 = pCVar5 + 3;
                g_InGameSelectionRemoveTripletDwordCount =
                     g_InGameSelectionRemoveTripletDwordCount - 3;
              } while (g_InGameSelectionRemoveTripletDwordCount != 0 && 2 < (int)uVar1);
            }
            if (g_InGameSelectionInsertTripletDwordCount != 0) {
              pCVar5 = (CommandPayloadDword04 *)&g_InGameSelectionInsertTripletDwords;
              do {
                if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_NETWORKED_MASK) ==
                    SESSION_NETWORK_ROLE_LOCAL) {
                  FrontendPlayerSelection_InsertThreeEntriesAndRefresh
                            (g_LocalPlayerRuntimeId,pCVar5[2],pCVar5[1],*pCVar5);
                }
                else {
                  InGameCommandQueue_AppendLocalPlayerCommand(0xa60,pCVar5[2],pCVar5[1],*pCVar5);
                }
                uVar1 = g_InGameSelectionInsertTripletDwordCount;
                pCVar5 = pCVar5 + 3;
                g_InGameSelectionInsertTripletDwordCount =
                     g_InGameSelectionInsertTripletDwordCount - 3;
              } while (g_InGameSelectionInsertTripletDwordCount != 0 && 2 < (int)uVar1);
            }
          }
        }
      }
      else if ((pointerValue0 != 0x7fffffff) && ((g_InGamePointerInteractionStateFlags & 3) == 0)) {
        if ((g_CursorButtonState & 4) == 0) {
          iVar2 = (*g_PointerSetPosition)
                            (g_InGameCommandPointerCaptureY,g_InGameCommandPointerCaptureX);
          g_InGameCommandPreviewHeading16 = g_InGameCommandPreviewHeading16 + iVar2 * 0x40;
          g_InGameCommandPreviewHeading16 = g_InGameCommandPreviewHeading16 & 0xffff;
        }
        else {
          g_InGameCommandPointerCaptureX =
               *(sdword *)(inGameRuntime[1].interaction.reserved00_47 + 0xc);
          g_InGameCommandPointerCaptureY =
               *(sdword *)(inGameRuntime[1].interaction.reserved00_47 + 0x10);
        }
      }
    }
    else if (pointerValue0 != 0x7fffffff) {
      if ((g_CursorButtonState & 4) == 0) {
        iVar2 = (*g_PointerSetPosition)
                          (g_InGamePlacementPointerCaptureY,g_InGamePlacementPointerCaptureX);
        g_InGamePlacementHeading16 = g_InGamePlacementHeading16 + iVar2 * 0x40;
        g_InGamePlacementHeading16 = g_InGamePlacementHeading16 & 0xffff;
      }
      else {
        g_InGamePlacementPointerCaptureX =
             *(sdword *)(inGameRuntime[1].interaction.reserved00_47 + 0xc);
        g_InGamePlacementPointerCaptureY =
             *(sdword *)(inGameRuntime[1].interaction.reserved00_47 + 0x10);
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
  code *pcVar1;
  dword dVar2;
  CommandPayloadDword04 payloadDword0C;
  uint uVar3;
  int ownerIndex;
  CommandPayloadDword08 payloadDword08;
  GameEntityRuntime *entry;
  CommandPayloadDword04 armyRuntimeIndex;
  bool bVar4;
  ModelRuntimeScaleRatioRegisterPairQ12 MVar5;
  
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
  bVar4 = SelectionInfo_ValidateOwnerType16AndAnyActiveCf(inGameRuntime->activeFactionRuntimeIndex);
  if (!bVar4) {
    if (((pointerValue0 != 0x7fffffff) && ((g_UiCommandRuntimeFlags & 0x80) != 0)) &&
       ((g_InGamePointerInteractionStateFlags & 3) == 0)) {
      g_UiCommandRuntimeFlags = g_UiCommandRuntimeFlags & 0xffffff7f;
      uVar3 = 0;
      if ((g_KeyboardStateMask & 0x3f) == 0) {
        uVar3 = 7;
      }
      if ((g_KeyboardStateMask & 3) != 0) {
        uVar3 = uVar3 | 1;
      }
      if ((g_KeyboardStateMask & 0x30) != 0) {
        uVar3 = uVar3 | 2;
      }
      if ((g_KeyboardStateMask & 0xc) != 0) {
        uVar3 = uVar3 | 4;
      }
      dVar2 = SelectionInfo_CollectAttachmentEffectVariantMask();
      if ((uVar3 & dVar2) != 0) {
        pcVar1 = *(code **)((uVar3 & dVar2) * 4 + 0x563748);
        if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_NETWORKED_MASK) ==
            SESSION_NETWORK_ROLE_LOCAL) {
          (*pcVar1)(g_LocalPlayerRuntimeId);
        }
        else {
          InGameCommandQueue_AppendLocalPlayerCommand
                    ((UiActionId)(pcVar1 + -0x55f130),g_InGameCommandPreviewHeading16,pointerValue1,
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
  bVar4 = SelectionInfo_HasAnyEntryCf();
  if ((!bVar4) || (bVar4 = SelectionInfo_AllEntriesEmptyOrMatchOwnerCf(ownerIndex), bVar4)) {
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
      bVar4 = SelectionInfo_TestAnyActiveOrSingleClass13Cf();
      if ((!bVar4) && (pointerValue0 != 0x7fffffff)) {
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
      bVar4 = SelectionInfo_TestAnyStateField100NonnegativeCf();
      if (bVar4) {
        bVar4 = GameFactionRuntime_TestCapabilityBitClearCf
                          ((entry->common).ownership.ownerIndex,ownerIndex);
        if (bVar4) goto InGameWorldInput_SelectCandidateArmy;
      }
      else {
InGameWorldInput_TestCandidateCapability:
        bVar4 = GameFactionRuntime_TestCapabilityBitClearCf
                          ((entry->common).ownership.ownerIndex,ownerIndex);
        if (bVar4) goto InGameWorldInput_ReleasePointerCapture;
        MVar5 = ModelRuntime_QueryHierarchyScaleRatioQ12Regs((RuntimeModelFactionPrefix10 *)entry);
        if ((int)MVar5 != (int)(MVar5 >> 0x20)) goto InGameWorldInput_SelectCandidateArmy;
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
        dVar2 = WorldRuntime_InterpolateTopSurfaceHeightOrSentinel
                          (pointerValue1,pointerValue2,inGameRuntime);
        if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_NETWORKED_MASK) ==
            SESSION_NETWORK_ROLE_LOCAL) {
          InGamePlayerSelection_ApplyTargetPositionCommand
                    (g_LocalPlayerRuntimeId,dVar2,payloadDword08,payloadDword0C);
        }
        else {
          InGameCommandQueue_AppendLocalPlayerCommand(0xde0,dVar2,payloadDword08,payloadDword0C);
        }
      }
      goto InGameWorldInput_ReleasePointerCapture;
    }
    bVar4 = SelectionInfo_TestAnyStateField100NonnegativeCf();
    if (!bVar4) goto InGameWorldInput_TestCandidateCapability;
    bVar4 = GameFactionRuntime_TestCapabilityBitClearCf
                      ((entry->common).ownership.ownerIndex,ownerIndex);
    if ((!bVar4) && (bVar4 = SelectionInfo_FindEntryCf(entry), !bVar4))
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
      bVar4 = SelectionInfo_FindEntryCf(entry);
      if (bVar4) {
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
          (dword modifierFlags,dword commandCode,WorldRuntimeContext *worldRuntime)

{
  InGameCameraCommandKeyCode IVar1;
  uint uVar2;
  dword dVar3;
  dword dVar4;
  dword dVar5;
  dword dVar6;
  dword dVar7;
  dword dVar8;
  dword dVar9;
  InGameCameraCommandDispatchTable *pIVar10;
  InGameCameraCommandDispatchTable *pIVar11;
  
  dVar9 = g_LevelCameraBookmark7PackedHeadingLow16PitchHigh16;
  dVar8 = g_LevelCameraBookmark6PackedHeadingLow16PitchHigh16;
  dVar7 = g_LevelCameraBookmark5PackedHeadingLow16PitchHigh16;
  dVar6 = g_LevelCameraBookmark4PackedHeadingLow16PitchHigh16;
  dVar5 = g_LevelCameraBookmark3PackedHeadingLow16PitchHigh16;
  dVar4 = g_LevelCameraBookmark2PackedHeadingLow16PitchHigh16;
  dVar3 = g_LevelCameraBookmark1PackedHeadingLow16PitchHigh16;
  pIVar11 = &g_InGameCameraCommandDispatchRecords16;
  do {
    while( true ) {
      do {
        pIVar10 = pIVar11;
        IVar1 = pIVar10->records[0].keyCode;
        uVar2 = pIVar10->records[0].requiredModifierMask;
        if (IVar1 == 0) {
          return;
        }
        pIVar11 = (InGameCameraCommandDispatchTable *)(pIVar10->records + 1);
      } while (IVar1 != commandCode);
      if (uVar2 == 0) break;
      if ((modifierFlags & uVar2) != 0) goto override_jmp_0056f276_switch;
    }
  } while ((modifierFlags & 0x3c) != 0);
override_jmp_0056f276_switch:
                    // WARNING: Switch is manually overridden
  switch(pIVar10->records[0].continuationEntryAddress) {
  case 0x56f360:
    WorldRuntime_SetPosition60AndDistanceFromPosition80
              (g_LevelCameraBookmark1PositionZQ12,g_LevelCameraBookmark1PositionYQ12,
               g_LevelCameraBookmark1PositionXQ12,worldRuntime);
    WorldRuntime_SetMotionParameters6CThrough78Clamped
              (2,(int)dVar3 >> 0x10,dVar3 & 0xffff,g_LevelCameraBookmark1PositionMagnitudeQ12,
               worldRuntime);
    WorldRuntime_RecomputeMotionEndpointAgainstFieldSurface(worldRuntime);
    WorldRuntime_CommitScalar7CFrom8C(worldRuntime);
    break;
  case 0x56f3b0:
    WorldRuntime_SetPosition60AndDistanceFromPosition80
              (g_LevelCameraBookmark2PositionZQ12,g_LevelCameraBookmark2PositionYQ12,
               g_LevelCameraBookmark2PositionXQ12,worldRuntime);
    WorldRuntime_SetMotionParameters6CThrough78Clamped
              (2,(int)dVar4 >> 0x10,dVar4 & 0xffff,g_LevelCameraBookmark2PositionMagnitudeQ12,
               worldRuntime);
    WorldRuntime_RecomputeMotionEndpointAgainstFieldSurface(worldRuntime);
    WorldRuntime_CommitScalar7CFrom8C(worldRuntime);
    break;
  case 0x56f400:
    WorldRuntime_SetPosition60AndDistanceFromPosition80
              (g_LevelCameraBookmark3PositionZQ12,g_LevelCameraBookmark3PositionYQ12,
               g_LevelCameraBookmark3PositionXQ12,worldRuntime);
    WorldRuntime_SetMotionParameters6CThrough78Clamped
              (2,(int)dVar5 >> 0x10,dVar5 & 0xffff,g_LevelCameraBookmark3PositionMagnitudeQ12,
               worldRuntime);
    WorldRuntime_RecomputeMotionEndpointAgainstFieldSurface(worldRuntime);
    WorldRuntime_CommitScalar7CFrom8C(worldRuntime);
    break;
  case 0x56f450:
    WorldRuntime_SetPosition60AndDistanceFromPosition80
              (g_LevelCameraBookmark4PositionZQ12,g_LevelCameraBookmark4PositionYQ12,
               g_LevelCameraBookmark4PositionXQ12,worldRuntime);
    WorldRuntime_SetMotionParameters6CThrough78Clamped
              (2,(int)dVar6 >> 0x10,dVar6 & 0xffff,g_LevelCameraBookmark4PositionMagnitudeQ12,
               worldRuntime);
    WorldRuntime_RecomputeMotionEndpointAgainstFieldSurface(worldRuntime);
    WorldRuntime_CommitScalar7CFrom8C(worldRuntime);
    break;
  case 0x56f4a0:
    WorldRuntime_SetPosition60AndDistanceFromPosition80
              (g_LevelCameraBookmark5PositionZQ12,g_LevelCameraBookmark5PositionYQ12,
               g_LevelCameraBookmark5PositionXQ12,worldRuntime);
    WorldRuntime_SetMotionParameters6CThrough78Clamped
              (2,(int)dVar7 >> 0x10,dVar7 & 0xffff,g_LevelCameraBookmark5PositionMagnitudeQ12,
               worldRuntime);
    WorldRuntime_RecomputeMotionEndpointAgainstFieldSurface(worldRuntime);
    WorldRuntime_CommitScalar7CFrom8C(worldRuntime);
    break;
  case 0x56f4f0:
    WorldRuntime_SetPosition60AndDistanceFromPosition80
              (g_LevelCameraBookmark6PositionZQ12,g_LevelCameraBookmark6PositionYQ12,
               g_LevelCameraBookmark6PositionXQ12,worldRuntime);
    WorldRuntime_SetMotionParameters6CThrough78Clamped
              (2,(int)dVar8 >> 0x10,dVar8 & 0xffff,g_LevelCameraBookmark6PositionMagnitudeQ12,
               worldRuntime);
    WorldRuntime_RecomputeMotionEndpointAgainstFieldSurface(worldRuntime);
    WorldRuntime_CommitScalar7CFrom8C(worldRuntime);
    break;
  case 0x56f540:
    WorldRuntime_SetPosition60AndDistanceFromPosition80
              (g_LevelCameraBookmark7PositionZQ12,g_LevelCameraBookmark7PositionYQ12,
               g_LevelCameraBookmark7PositionXQ12,worldRuntime);
    WorldRuntime_SetMotionParameters6CThrough78Clamped
              (2,(int)dVar9 >> 0x10,dVar9 & 0xffff,g_LevelCameraBookmark7PositionMagnitudeQ12,
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
  WorldRuntimeFlags *pWVar1;
  UiNodeBase *parentCursor;
  
  if (targetingContext->actionState == INGAME_TARGETING_OBSERVED_CANCEL_AND_RESTORE) {
    targetingContext->actionState = INGAME_TARGETING_OBSERVED_IDLE;
    parentCursor = (targetingContext->base).parent;
    while (parentCursor != (UiNodeBase *)0xffffffff) {
      targetingContext = (InGameTargetingRootTraversalView9E60 *)(targetingContext->base).parent;
      parentCursor = (targetingContext->base).parent;
    }
    pWVar1 = &(targetingContext->worldRuntime0A30).runtimeFlags;
    *pWVar1 = *pWVar1 & 0xffffffef;
    WorldRuntime_RestoreMotionStateFromSnapshot(&targetingContext->worldRuntime0A30);
  }
  return;
}


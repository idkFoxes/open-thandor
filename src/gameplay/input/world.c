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
void InGameTargetingContext_AdvanceOrResolveTarget(InGameTargetingContext *targetingContext)

{
  UiAnchorFractionQ31 *worldRuntime;
  UiNodeBase *pUVar1;
  sdword sVar2;
  int iVar3;
  CommandPayloadDword04 modelToken;
  undefined4 extraout_ECX;
  undefined4 uVar4;
  CommandPayloadDword08 armyToken;
  qword qVar5;
  
  if (targetingContext->actionState == INGAME_TARGETING_OBSERVED_CANCEL_AND_RESTORE) {
    InGameTargetingContext_CancelAndRestoreState(targetingContext);
  }
  else if (targetingContext->actionState == INGAME_TARGETING_OBSERVED_ADVANCE_OR_RESOLVE) {
    pUVar1 = (targetingContext->base).parent;
    while (pUVar1 != (UiNodeBase *)0xffffffff) {
      targetingContext = (InGameTargetingContext *)(targetingContext->base).parent;
      pUVar1 = (targetingContext->base).parent;
    }
    worldRuntime = &targetingContext[10].base.leftAnchorQ31;
    uVar4 = *(undefined4 *)targetingContext[0x9e].reserved54_FF;
    if ((*(uint *)(targetingContext[10].reserved54_FF + 0x28) & 0x10) == 0) {
      WorldRuntime_CaptureMotionStateToSnapshot((WorldRuntimeContext *)worldRuntime);
      uVar4 = extraout_ECX;
    }
                    
    switch(uVar4) {
    case 2:
      for (pUVar1 = targetingContext[0xb].base.parent; pUVar1 != (UiNodeBase *)0x0;
          pUVar1 = pUVar1->firstChild) {
        if ((((pUVar1[2].vtable == (UiNodeVtable *)0x0) &&
             (pUVar1[1].nodeFlags == targetingContext[0x9e].base.layoutWidth)) &&
            (iVar3 = *(int *)(pUVar1->nodeFlags + UI_NODE_SUPPRESSED),
            pUVar1[2].nextSibling == (UiNodeBase *)targetingContext[0x9e].base.layoutHeight)) &&
           (*(int *)(targetingContext[10].reserved54_FF + 0x2c) == *(int *)(iVar3 + 0xc))) {
          modelToken = iVar3 - (int)g_ArmyRuntimeRebaseBaseMinusOne;
          armyToken = pUVar1->nodeFlags - g_ModelRuntimeRebaseDelta;
          if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_NETWORKED_MASK) ==
              SESSION_NETWORK_ROLE_LOCAL) {
            FrontendPlayerRuntime_AssignModelAndArmyTokensAndRefreshLocalPanel
                      (g_LocalPlayerRuntimeId,0,armyToken,modelToken);
          }
          else {
            InGameCommandQueue_AppendLocalPlayerCommand(0x1620,0,armyToken,modelToken);
          }
          targetingContext[0x9b].reserved4C_4F[0] = 0;
          targetingContext[0x9b].reserved4C_4F[1] = 0;
          targetingContext[0x9b].reserved4C_4F[2] = 0;
          targetingContext[0x9b].reserved4C_4F[3] = 0;
          return;
        }
      }
      break;
    case 3:
      sVar2 = targetingContext[0x9e].base.layoutHeight;
      *(sdword *)(targetingContext[0x9e].reserved54_FF + 4) =
           targetingContext[0x9e].base.layoutWidth;
      *(sdword *)(targetingContext[0x9e].reserved54_FF + 8) = sVar2;
    case 1:
      qVar5 = FieldGrid_GetNearestTerrainPoint
                        (targetingContext[0x9e].base.layoutHeight,
                         targetingContext[0x9e].base.layoutWidth,
                         *(FieldGridAsset **)(targetingContext[10].reserved54_FF + 0x30));
      WorldRuntime_SetPosition80AndRebuildPosition60FromAngles
                (*(AngleTurn32 *)(targetingContext[10].reserved54_FF + 0x50),
                 targetingContext[0x9e].base.nodeFlags,
                 *(UQ12 *)(targetingContext[10].reserved54_FF + 0x68),(Q12)(qVar5 >> 0x20),
                 targetingContext[0x9e].base.layoutHeight,targetingContext[0x9e].base.layoutWidth,
                 (WorldRuntimeContext *)worldRuntime);
      *(uint *)(targetingContext[10].reserved54_FF + 0x28) =
           *(uint *)(targetingContext[10].reserved54_FF + 0x28) & 0xffffffef;
      targetingContext[0x9b].reserved4C_4F[0] = 0x1b;
      targetingContext[0x9b].reserved4C_4F[1] = 0;
      targetingContext[0x9b].reserved4C_4F[2] = 0;
      targetingContext[0x9b].reserved4C_4F[3] = 0;
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
                ,WorldRuntimeNode *candidateNode,WorldRuntimeContext *inGameRuntime)

{
  dword dVar1;
  dword extraout_EAX;
  dword extraout_EAX_00;
  dword extraout_EAX_01;
  undefined4 extraout_ECX;
  FactionRuntimeIndex ownerIndex;
  int extraout_EDX;
  FactionRuntimeIndex factionIndex;
  FactionRuntimeIndex extraout_EDX_00;
  FactionRuntimeIndex factionIndex_00;
  FactionRuntimeIndex FVar2;
  uint uVar3;
  uint extraout_EDX_01;
  GameEntityRuntime *entry;
  bool bVar4;
  undefined8 uVar5;
  ModelRuntimeScaleRatioRegisterPairQ12 MVar6;
  GameFactionCapabilityCfVolatileContinuityResult GVar7;
  
  g_InGameCommandPreviewSurfaceHeightQ12OrSentinel = 0x7fffffff;
  (inGameRuntime->selection).selectedEntity = (GameEntityRuntime *)0x0;
  if (((inGameRuntime->interaction).interactionFlags48 & 8) != 0) {
    return 0;
  }
  if ((g_UiCommandRuntimeFlags & 0x100) != 0) {
    return 0;
  }
  if ((inGameRuntime->runtimeFlags & 0x10) != 0) {
    return 6;
  }
  bVar4 = false;
  if ((g_UiCommandRuntimeFlags & 0x20) != 0) {
    g_InGamePlacementSurfaceHeightQ12OrSentinel = pointerValue0;
    g_InGamePlacementWorldYQ12 = pointerValue2;
    g_InGamePlacementWorldXQ12 = pointerValue1;
    bVar4 = pointerValue0 < 0x7fffffff;
    if (pointerValue0 == 0x7fffffff) {
      return 0x18;
    }
    g_UiHoverSelectionRecord =
         (UiCommandRuntimeRecordPrefix *)
         g_SelectionPlayerRuntimeBlockPointers[(inGameRuntime->selection).activePlayerRuntimeId]->
         pendingSelectionEntityOffset8098;
    dVar1 = 0x2e;
    ArmyPlacement_ValidateAssetAtPointAndCellCornersCf
              (0,g_InGamePlacementHeading16,pointerValue1,pointerValue2,
               g_UiHoverSelectionRecord->armyAssetId,inGameRuntime->activeFactionRuntimeIndex,
               inGameRuntime);
    if (!bVar4) {
      dVar1 = 0x2d;
    }
    InGameSelectionDetailPanel_Rebuild();
    return dVar1;
  }
  uVar5 = SelectionInfo_ValidateOwnerType16AndAnyActiveCf(inGameRuntime->activeFactionRuntimeIndex);
  if (!bVar4) {
    g_InGamePointerInteractionStateFlags = g_InGamePointerInteractionStateFlags & 0xfffffffe;
    if ((candidateNode != (WorldRuntimeNode *)0x0) &&
       (candidateNode[2].common.nextNode == (WorldRuntimeNode *)0x0)) {
      bVar4 = pointerValue3 - 0x1000 < pointerValue0;
      if (((int)(pointerValue3 - 0x1000) <= (int)pointerValue0) &&
         (GameFactionRuntime_TestCapabilityBitClearCf
                    (*(dword *)(*(int *)((int)candidateNode->runtimePayload + 8) + 0xc),
                     (FactionRuntimeIndex)((ulonglong)uVar5 >> 0x20)), !bVar4)) {
        g_InGamePointerInteractionStateFlags = g_InGamePointerInteractionStateFlags | 1;
        return 0x15;
      }
    }
    dVar1 = SelectionInfo_CollectAttachmentEffectVariantMask();
    g_InGameCommandPreviewWorldYQ12 = pointerValue2;
    g_InGameCommandPreviewWorldXQ12 = pointerValue1;
    g_InGameCommandPreviewSurfaceHeightQ12OrSentinel = extraout_ECX;
    g_InGameCommandPreviewArmyAssetId = *(undefined4 *)((extraout_EDX_01 & dVar1) * 4 + 0x563788);
    return *(dword *)((extraout_EDX_01 & dVar1) * 4 + 0x563768);
  }
  if ((inGameRuntime->runtimeFlags & 0x80) != 0) {
    return 0;
  }
  entry = (GameEntityRuntime *)inGameRuntime;
  if ((((candidateNode == (WorldRuntimeNode *)0x0) ||
       (candidateNode[2].common.nextNode != (WorldRuntimeNode *)0x0)) ||
      (entry = *(GameEntityRuntime **)((int)candidateNode->runtimePayload + 8),
      (int)pointerValue0 < (int)(pointerValue3 - 0x1000))) ||
     ((entry->common).ownership.ownerIndex == 0)) {
    candidateNode = (WorldRuntimeNode *)0x0;
  }
  bVar4 = false;
  if (candidateNode != (WorldRuntimeNode *)0x0) {
    (inGameRuntime->selection).selectedEntity = entry;
  }
  SelectionInfo_HasAnyEntryCf();
  FVar2 = ownerIndex;
  if (!bVar4) {
InGameWorldInput_ReturnCandidateOwnershipCursor:
    dVar1 = 0;
    if ((candidateNode != (WorldRuntimeNode *)0x0) &&
       (dVar1 = 0x15, FVar2 != (entry->common).ownership.ownerIndex)) {
      dVar1 = 0x16;
    }
    return dVar1;
  }
  SelectionInfo_AllEntriesEmptyOrMatchOwnerCf(ownerIndex);
  if (bVar4) {
    FVar2 = extraout_EDX;
    if (candidateNode == (WorldRuntimeNode *)0x0) {
      return 0;
    }
    goto InGameWorldInput_ReturnCandidateOwnershipCursor;
  }
  if ((g_KeyboardStateMask & 0xc) == 0) {
    if (candidateNode == (WorldRuntimeNode *)0x0) {
      bVar4 = false;
      SelectionInfo_TestAnyActiveOrSingleClass13Cf();
      if (bVar4) {
        return extraout_EAX;
      }
      SelectionInfo_TestPositionCommandAtWorldPointCf(pointerValue1,pointerValue2,inGameRuntime);
      if (bVar4) {
        return extraout_EAX_00;
      }
      if (pointerValue0 == 0x7fffffff) {
        return extraout_EAX_00;
      }
      return 0x17;
    }
    bVar4 = false;
    if ((g_KeyboardStateMask & 0x33) != 0) {
InGameWorldInput_ReturnModifiedCandidateCursor:
      if (candidateNode == (WorldRuntimeNode *)0x0) {
        return 0;
      }
      if (extraout_EDX == (entry->common).ownership.ownerIndex) {
        return 0x15;
      }
      return 0x16;
    }
    SelectionInfo_TestAnyStateField100NonnegativeCf();
    FVar2 = factionIndex;
    if (bVar4) {
      GVar7 = GameFactionRuntime_TestCapabilityBitClearCf
                        ((entry->common).ownership.ownerIndex,factionIndex);
      dVar1 = GVar7.preservedEdxCallerValue;
      if (bVar4) goto InGameWorldInput_ClassifySelectedStateCommand;
      goto InGameWorldInput_ReturnCandidateOwnerCursor;
    }
  }
  else {
    bVar4 = false;
    if ((g_KeyboardStateMask & 0x33) != 0) goto InGameWorldInput_ReturnModifiedCandidateCursor;
    SelectionInfo_TestAnyStateField100NonnegativeCf();
    FVar2 = extraout_EDX_00;
    if (bVar4) {
InGameWorldInput_ClassifySelectedStateCommand:
      bVar4 = false;
      SelectionInfo_TestAllStateField100NonpositiveCf();
      if (bVar4) {
        return extraout_EAX_01;
      }
      bVar4 = false;
      if (candidateNode == (WorldRuntimeNode *)0x0) {
        return 0x19;
      }
      GVar7 = GameFactionRuntime_TestCapabilityBitClearCf
                        ((entry->common).ownership.ownerIndex,factionIndex_00);
      if (bVar4) {
        return GVar7.preservedEaxCallerValue;
      }
      dVar1 = SelectionInfo_FindEntryCf(entry);
      if (bVar4) {
        return dVar1;
      }
      return 0;
    }
  }
  bVar4 = false;
  if (candidateNode == (WorldRuntimeNode *)0x0) {
    return 0;
  }
  GVar7 = GameFactionRuntime_TestCapabilityBitClearCf((entry->common).ownership.ownerIndex,FVar2);
  dVar1 = GVar7.preservedEdxCallerValue;
  if (bVar4) {
    return GVar7.preservedEaxCallerValue;
  }
  MVar6 = ModelRuntime_QueryHierarchyScaleRatioQ12Regs((ArmyRuntimeSlot *)entry);
  uVar3 = (uint)(MVar6 >> 0x20);
  bVar4 = (uint)MVar6 < uVar3;
  if ((uint)MVar6 != uVar3) {
    dVar1 = SelectionInfo_FindEntryCf(entry);
    if (bVar4) {
      return dVar1;
    }
    return 0;
  }
InGameWorldInput_ReturnCandidateOwnerCursor:
  if (dVar1 != (entry->common).ownership.ownerIndex) {
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
void InGameWorldInput_BeginPointerCaptureCf
               (InGamePointerCallbackValue0 pointerValue0,dword pointerValue1,dword pointerValue2,
               InGamePointerCallbackValue3 pointerValue3,WorldRuntimeNode *candidateNode,
               WorldRuntimeContext *inGameRuntime)

{
  CommandPayloadDword04 modelToken;
  bool bVar1;
  undefined8 uVar2;
  GameFactionCapabilityCfVolatileContinuityResult GVar3;
  
  if (((g_UiCommandRuntimeFlags & 0x101) == 0) &&
     (inGameRuntime->runtimeFlags = inGameRuntime->runtimeFlags & 0xf7ffffff,
     ((inGameRuntime->interaction).interactionFlags48 & 8) == 0)) {
    if (((inGameRuntime->interaction).interactionFlags48 & 0x80) != 0) {
      inGameRuntime->runtimeFlags = inGameRuntime->runtimeFlags | 0x8000000;
    }
    if ((inGameRuntime->runtimeFlags & 0x10) == 0) {
      bVar1 = false;
      if ((g_UiCommandRuntimeFlags & 0x20) == 0) {
        uVar2 = SelectionInfo_ValidateOwnerType16AndAnyActiveCf
                          (inGameRuntime->activeFactionRuntimeIndex);
        if (bVar1) {
          g_InGamePointerInteractionStateFlags = g_InGamePointerInteractionStateFlags | 2;
        }
        else {
          if ((candidateNode != (WorldRuntimeNode *)0x0) &&
             (candidateNode[2].common.nextNode == (WorldRuntimeNode *)0x0)) {
            bVar1 = pointerValue3 - 0x1000 < pointerValue0;
            if ((int)(pointerValue3 - 0x1000) <= (int)pointerValue0) {
              GVar3 = GameFactionRuntime_TestCapabilityBitClearCf
                                (*(dword *)(*(int *)((int)candidateNode->runtimePayload + 8) + 0xc),
                                 (FactionRuntimeIndex)((ulonglong)uVar2 >> 0x20));
              if (!bVar1) {
                modelToken = GVar3.preservedEcxCallerValue - (int)g_ArmyRuntimeRebaseBaseMinusOne;
                if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_NETWORKED_MASK) !=
                    SESSION_NETWORK_ROLE_LOCAL) {
                  InGameCommandQueue_AppendLocalPlayerCommand(0x9a0,0,0,modelToken);
                  return;
                }
                FrontendPlayerRuntime_AssignModelTokenAndRefreshSelection
                          (g_LocalPlayerRuntimeId,0,0,modelToken);
                return;
              }
            }
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
void InGameWorldInput_UpdateDragSelectionAndCameraCf
               (InGamePointerCallbackValue0 pointerValue0,dword pointerValue1,dword pointerValue2,
               dword pointerValue3,WorldRuntimeNode *candidateNode,
               WorldRuntimeContext *inGameRuntime)

{
  int iVar1;
  uint uVar2;
  int iVar3;
  int extraout_ECX;
  int extraout_ECX_00;
  int extraout_ECX_01;
  int extraout_ECX_02;
  int extraout_ECX_03;
  int extraout_ECX_04;
  int extraout_ECX_05;
  int extraout_ECX_06;
  void *pvVar4;
  uint uVar5;
  void *extraout_EDX;
  void *extraout_EDX_00;
  void *extraout_EDX_01;
  void *extraout_EDX_02;
  void *extraout_EDX_03;
  void *extraout_EDX_04;
  InGameCommandPayloadTripletValue32 payloadValue;
  undefined4 *puVar6;
  WorldRuntimeNode *runtimeNode;
  CommandPayloadDword04 *pCVar7;
  bool bVar8;
  undefined8 uVar9;
  GameEntityRuntime *entry;
  
  if (((((g_UiCommandRuntimeFlags & 0x101) == 0) &&
       (((inGameRuntime->interaction).interactionFlags48 & 8) == 0)) &&
      ((g_UiCommandRuntimeFlags & 0x100) == 0)) && ((inGameRuntime->runtimeFlags & 0x10) == 0)) {
    bVar8 = false;
    if ((g_UiCommandRuntimeFlags & 0x20) == 0) {
      uVar9 = SelectionInfo_ValidateOwnerType16AndAnyActiveCf
                        (inGameRuntime->activeFactionRuntimeIndex);
      pvVar4 = (void *)((ulonglong)uVar9 >> 0x20);
      if (bVar8) {
        if ((inGameRuntime->runtimeFlags & 0x80) == 0) {
          uVar2 = *(int *)(inGameRuntime[1].interaction.reserved00_47 + 4) -
                  *(int *)(inGameRuntime[1].interaction.reserved00_47 + 0xc);
          if ((int)uVar2 < 0) {
            uVar2 = -uVar2;
          }
          uVar5 = *(int *)(inGameRuntime[1].interaction.reserved00_47 + 8) -
                  *(int *)(inGameRuntime[1].interaction.reserved00_47 + 0x10);
          if ((int)uVar5 < 0) {
            uVar5 = -uVar5;
          }
          if ((0x17 < uVar2) || (0x17 < uVar5)) {
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
          puVar6 = (undefined4 *)&g_InGameSelectionInsertTripletDwords;
          for (iVar3 = 0x1a; iVar3 != 0; iVar3 = iVar3 + -1) {
            *puVar6 = 0;
            puVar6 = puVar6 + 1;
          }
          runtimeNode = inGameRuntime->ownerListHead;
          iVar3 = inGameRuntime->activeFactionRuntimeIndex;
          if (runtimeNode != (WorldRuntimeNode *)0x0) {
            do {
              if (((runtimeNode[2].common.nextNode == (WorldRuntimeNode *)0x0) &&
                  (pvVar4 = runtimeNode->runtimePayload, (runtimeNode->runtimeFlags & 2) != 0)) &&
                 ((entry = *(GameEntityRuntime **)((int)pvVar4 + 8),
                  (runtimeNode->runtimeFlags & 0x20) != 0 &&
                  (iVar3 == (entry->common).ownership.ownerIndex)))) {
                bVar8 = entry < g_ArmyRuntimeRebaseBaseMinusOne;
                payloadValue = (int)entry - (int)g_ArmyRuntimeRebaseBaseMinusOne;
                WorldRuntimeNode_IsPositionInsideBoundsCf
                          (runtimeNode,(WorldRuntimeExtendedMapControlAddress32)inGameRuntime);
                if (bVar8) {
                  SelectionInfo_FindEntryCf(entry);
                  iVar3 = extraout_ECX;
                  pvVar4 = extraout_EDX;
                  if (bVar8) {
                    uVar9 = InGameCommandQueue_ContainsTripletValueCf(payloadValue,0x55fb90);
                    pvVar4 = (void *)((ulonglong)uVar9 >> 0x20);
                    iVar3 = extraout_ECX_00;
                    if ((!bVar8) &&
                       (*(InGameCommandPayloadTripletValue32 *)
                         (&g_InGameSelectionInsertTripletDwords + (uint)uVar9 * 4) = payloadValue,
                       (uint)uVar9 < 0xb)) {
                      g_InGameSelectionInsertTripletDwordCount =
                           g_InGameSelectionInsertTripletDwordCount + 1;
                    }
                  }
                }
                else {
                  SelectionInfo_FindEntryCf(entry);
                  iVar3 = extraout_ECX_01;
                  pvVar4 = extraout_EDX_00;
                  if (!bVar8) {
                    uVar9 = InGameCommandQueue_ContainsTripletValueCf(payloadValue,0x55fc30);
                    pvVar4 = (void *)((ulonglong)uVar9 >> 0x20);
                    iVar3 = extraout_ECX_02;
                    if ((!bVar8) &&
                       (*(InGameCommandPayloadTripletValue32 *)
                         (&g_InGameSelectionRemoveTripletDwords + (uint)uVar9 * 4) = payloadValue,
                       (uint)uVar9 < 0xb)) {
                      g_InGameSelectionRemoveTripletDwordCount =
                           g_InGameSelectionRemoveTripletDwordCount + 1;
                    }
                  }
                }
              }
              runtimeNode = (runtimeNode->common).nextNode;
            } while (runtimeNode != (WorldRuntimeNode *)0x0);
            if (g_InGameSelectionRemoveTripletDwordCount != 0) {
              pCVar7 = (CommandPayloadDword04 *)&g_InGameSelectionRemoveTripletDwords;
              do {
                if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_NETWORKED_MASK) ==
                    SESSION_NETWORK_ROLE_LOCAL) {
                  FrontendPlayerSelection_RemoveThreeEntriesAndRefresh
                            (g_LocalPlayerRuntimeId,pCVar7[2],pCVar7[1],*pCVar7);
                  iVar3 = extraout_ECX_04;
                  pvVar4 = extraout_EDX_02;
                }
                else {
                  InGameCommandQueue_AppendLocalPlayerCommand(0xb00,pCVar7[2],pCVar7[1],*pCVar7);
                  iVar3 = extraout_ECX_03;
                  pvVar4 = extraout_EDX_01;
                }
                iVar1 = g_InGameSelectionRemoveTripletDwordCount;
                pCVar7 = pCVar7 + 3;
                g_InGameSelectionRemoveTripletDwordCount =
                     g_InGameSelectionRemoveTripletDwordCount + -3;
              } while (g_InGameSelectionRemoveTripletDwordCount != 0 && 2 < iVar1);
            }
            if (g_InGameSelectionInsertTripletDwordCount != 0) {
              pCVar7 = (CommandPayloadDword04 *)&g_InGameSelectionInsertTripletDwords;
              do {
                if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_NETWORKED_MASK) ==
                    SESSION_NETWORK_ROLE_LOCAL) {
                  FrontendPlayerSelection_InsertThreeEntriesAndRefresh
                            (iVar3,pvVar4,g_LocalPlayerRuntimeId,pCVar7[2],pCVar7[1],*pCVar7);
                  iVar3 = extraout_ECX_06;
                  pvVar4 = extraout_EDX_04;
                }
                else {
                  InGameCommandQueue_AppendLocalPlayerCommand(0xa60,pCVar7[2],pCVar7[1],*pCVar7);
                  iVar3 = extraout_ECX_05;
                  pvVar4 = extraout_EDX_03;
                }
                iVar1 = g_InGameSelectionInsertTripletDwordCount;
                pCVar7 = pCVar7 + 3;
                g_InGameSelectionInsertTripletDwordCount =
                     g_InGameSelectionInsertTripletDwordCount + -3;
              } while (g_InGameSelectionInsertTripletDwordCount != 0 && 2 < iVar1);
            }
          }
        }
      }
      else if ((pointerValue0 != 0x7fffffff) && ((g_InGamePointerInteractionStateFlags & 3) == 0)) {
        if ((g_CursorButtonState & 4) == 0) {
          iVar3 = (*g_PointerSetPosition)
                            (g_InGameCommandPointerCaptureY,g_InGameCommandPointerCaptureX);
          g_InGameCommandPreviewHeading16 = g_InGameCommandPreviewHeading16 + iVar3 * 0x40;
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
        iVar3 = (*g_PointerSetPosition)
                          (g_InGamePlacementPointerCaptureY,g_InGamePlacementPointerCaptureX);
        g_InGamePlacementHeading16 = g_InGamePlacementHeading16 + iVar3 * 0x40;
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
void InGameWorldInput_CommitPointerActionCf
               (InGamePointerCallbackValue0 pointerValue0,InGamePointerCallbackValue1 pointerValue1,
               InGamePointerCallbackValue2 pointerValue2,InGamePointerCallbackValue3 pointerValue3,
               WorldRuntimeNode *candidateNode,WorldRuntimeContext *inGameRuntime)

{
  uint uVar1;
  code *pcVar2;
  dword dVar3;
  CommandPayloadDword04 payloadDword0C;
  undefined4 extraout_ECX;
  uint extraout_ECX_00;
  FactionRuntimeIndex ownerIndex;
  uint extraout_EDX;
  FactionRuntimeIndex factionIndex;
  FactionRuntimeIndex factionIndex_00;
  FactionRuntimeIndex factionIndex_01;
  CommandPayloadDword08 payloadDword08;
  undefined4 extraout_EDX_00;
  GameEntityRuntime *entry;
  CommandPayloadDword04 armyRuntimeIndex;
  bool bVar4;
  undefined1 uVar5;
  ModelRuntimeScaleRatioRegisterPairQ12 MVar6;
  GameFactionCapabilityCfVolatileContinuityResult GVar7;
  
  if (((((g_UiCommandRuntimeFlags & 0x101) != 0) ||
       (((inGameRuntime->interaction).interactionFlags48 & 8) != 0)) ||
      ((g_UiCommandRuntimeFlags & 0x100) != 0)) || ((inGameRuntime->runtimeFlags & 0x10) != 0))
  goto InGameWorldInput_ReleasePointerCapture;
  bVar4 = false;
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
  SelectionInfo_ValidateOwnerType16AndAnyActiveCf(inGameRuntime->activeFactionRuntimeIndex);
  if (!bVar4) {
    if (((pointerValue0 != 0x7fffffff) && ((g_UiCommandRuntimeFlags & 0x80) != 0)) &&
       ((g_InGamePointerInteractionStateFlags & 3) == 0)) {
      g_UiCommandRuntimeFlags = g_UiCommandRuntimeFlags & 0xffffff7f;
      dVar3 = SelectionInfo_CollectAttachmentEffectVariantMask();
      if ((extraout_ECX_00 & dVar3) != 0) {
        pcVar2 = *(code **)((extraout_ECX_00 & dVar3) * 4 + 0x563748);
        if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_NETWORKED_MASK) ==
            SESSION_NETWORK_ROLE_LOCAL) {
          (*pcVar2)(g_LocalPlayerRuntimeId);
        }
        else {
          InGameCommandQueue_AppendLocalPlayerCommand
                    ((UiActionId)(pcVar2 + -0x55f130),g_InGameCommandPreviewHeading16,pointerValue1,
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
  entry = (GameEntityRuntime *)inGameRuntime;
  if (((candidateNode == (WorldRuntimeNode *)0x0) ||
      (candidateNode[2].common.nextNode != (WorldRuntimeNode *)0x0)) ||
     ((entry = *(GameEntityRuntime **)((int)candidateNode->runtimePayload + 8),
      (int)pointerValue0 < (int)(pointerValue3 - 0x1000) ||
      ((entry->common).ownership.ownerIndex == 0)))) {
    candidateNode = (WorldRuntimeNode *)0x0;
  }
  bVar4 = false;
  SelectionInfo_HasAnyEntryCf();
  if ((!bVar4) || (SelectionInfo_AllEntriesEmptyOrMatchOwnerCf(ownerIndex), bVar4)) {
    if (candidateNode != (WorldRuntimeNode *)0x0) {
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
    bVar4 = false;
    if (candidateNode == (WorldRuntimeNode *)0x0) {
      SelectionInfo_TestAnyActiveOrSingleClass13Cf();
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
    uVar5 = false;
    if ((g_KeyboardStateMask & 0x33) == 0) {
      SelectionInfo_TestAnyStateField100NonnegativeCf();
      factionIndex_01 = factionIndex;
      if ((bool)uVar5) {
        GVar7 = GameFactionRuntime_TestCapabilityBitClearCf
                          ((entry->common).ownership.ownerIndex,factionIndex);
        dVar3 = GVar7.preservedEdxCallerValue;
        if ((bool)uVar5) goto InGameWorldInput_SelectCandidateArmy;
      }
      else {
InGameWorldInput_TestCandidateCapability:
        GVar7 = GameFactionRuntime_TestCapabilityBitClearCf
                          ((entry->common).ownership.ownerIndex,factionIndex_01);
        dVar3 = GVar7.preservedEdxCallerValue;
        if ((bool)uVar5) goto InGameWorldInput_ReleasePointerCapture;
        MVar6 = ModelRuntime_QueryHierarchyScaleRatioQ12Regs((ArmyRuntimeSlot *)entry);
        if ((int)MVar6 != (int)(MVar6 >> 0x20)) goto InGameWorldInput_SelectCandidateArmy;
      }
      if (dVar3 == (entry->common).ownership.ownerIndex) {
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
    uVar5 = false;
    if (candidateNode == (WorldRuntimeNode *)0x0) {
      if (pointerValue0 != 0x7fffffff) {
        dVar3 = WorldRuntime_InterpolateTopSurfaceHeightOrSentinel
                          (pointerValue1,pointerValue2,inGameRuntime);
        if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_NETWORKED_MASK) ==
            SESSION_NETWORK_ROLE_LOCAL) {
          InGamePlayerSelection_ApplyTargetPositionCommand
                    (g_LocalPlayerRuntimeId,dVar3,payloadDword08,payloadDword0C);
        }
        else {
          InGameCommandQueue_AppendLocalPlayerCommand(0xde0,dVar3,payloadDword08,payloadDword0C);
        }
      }
      goto InGameWorldInput_ReleasePointerCapture;
    }
    SelectionInfo_TestAnyStateField100NonnegativeCf();
    factionIndex_01 = factionIndex_00;
    if (!(bool)uVar5) goto InGameWorldInput_TestCandidateCapability;
    GameFactionRuntime_TestCapabilityBitClearCf
              ((entry->common).ownership.ownerIndex,factionIndex_00);
    if ((!(bool)uVar5) && (SelectionInfo_FindEntryCf(entry), !(bool)uVar5))
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
  if (candidateNode != (WorldRuntimeNode *)0x0) {
    uVar1 = (entry->common).ownership.ownerIndex;
    bVar4 = extraout_EDX < uVar1;
    if (extraout_EDX == uVar1) {
      SelectionInfo_FindEntryCf(entry);
      if (bVar4) {
        if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_NETWORKED_MASK) ==
            SESSION_NETWORK_ROLE_LOCAL) {
          FrontendPlayerSelection_InsertThreeEntriesAndRefresh
                    (extraout_ECX,extraout_EDX_00,g_LocalPlayerRuntimeId,0,0,
                     (int)entry - (int)g_ArmyRuntimeRebaseBaseMinusOne);
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
void InGameCameraCommand_DispatchByCodeAndModifierFlagsCf
               (dword modifierFlags,dword commandCode,WorldRuntimeContext *worldRuntime)

{
  InGameCameraCommandKeyCode IVar1;
  uint uVar2;
  uint extraout_EAX;
  uint extraout_EAX_00;
  uint extraout_EAX_01;
  uint extraout_EAX_02;
  uint extraout_EAX_03;
  uint extraout_EAX_04;
  uint extraout_EAX_05;
  int extraout_EDX;
  int extraout_EDX_00;
  int extraout_EDX_01;
  int extraout_EDX_02;
  int extraout_EDX_03;
  int extraout_EDX_04;
  int extraout_EDX_05;
  InGameCameraCommandDispatchTable *pIVar3;
  InGameCameraCommandDispatchTable *pIVar4;
  
  pIVar4 = &g_InGameCameraCommandDispatchRecords16;
  do {
    while( true ) {
      do {
        pIVar3 = pIVar4;
        IVar1 = pIVar3->records[0].keyCode;
        uVar2 = pIVar3->records[0].requiredModifierMask;
        if (IVar1 == 0) {
          return;
        }
        pIVar4 = (InGameCameraCommandDispatchTable *)(pIVar3->records + 1);
      } while (IVar1 != commandCode);
      if (uVar2 == 0) break;
      if ((modifierFlags & uVar2) != 0) goto override_jmp_0056f276_switch;
    }
  } while ((modifierFlags & 0x3c) != 0);
override_jmp_0056f276_switch:
                    
  switch(pIVar3->records[0].continuationEntryAddress) {
  case 0x56f360:
    WorldRuntime_SetPosition60AndDistanceFromPosition80
              (g_LevelCameraBookmark1PositionZQ12,g_LevelCameraBookmark1PositionYQ12,
               g_LevelCameraBookmark1PositionXQ12,worldRuntime);
    WorldRuntime_SetMotionParameters6CThrough78Clamped
              (2,extraout_EDX >> 0x10,extraout_EAX & 0xffff,
               g_LevelCameraBookmark1PositionMagnitudeQ12,worldRuntime);
    WorldRuntime_RecomputeMotionEndpointAgainstFieldSurface(worldRuntime);
    WorldRuntime_CommitScalar7CFrom8C(worldRuntime);
    break;
  case 0x56f3b0:
    WorldRuntime_SetPosition60AndDistanceFromPosition80
              (g_LevelCameraBookmark2PositionZQ12,g_LevelCameraBookmark2PositionYQ12,
               g_LevelCameraBookmark2PositionXQ12,worldRuntime);
    WorldRuntime_SetMotionParameters6CThrough78Clamped
              (2,extraout_EDX_00 >> 0x10,extraout_EAX_00 & 0xffff,
               g_LevelCameraBookmark2PositionMagnitudeQ12,worldRuntime);
    WorldRuntime_RecomputeMotionEndpointAgainstFieldSurface(worldRuntime);
    WorldRuntime_CommitScalar7CFrom8C(worldRuntime);
    break;
  case 0x56f400:
    WorldRuntime_SetPosition60AndDistanceFromPosition80
              (g_LevelCameraBookmark3PositionZQ12,g_LevelCameraBookmark3PositionYQ12,
               g_LevelCameraBookmark3PositionXQ12,worldRuntime);
    WorldRuntime_SetMotionParameters6CThrough78Clamped
              (2,extraout_EDX_01 >> 0x10,extraout_EAX_01 & 0xffff,
               g_LevelCameraBookmark3PositionMagnitudeQ12,worldRuntime);
    WorldRuntime_RecomputeMotionEndpointAgainstFieldSurface(worldRuntime);
    WorldRuntime_CommitScalar7CFrom8C(worldRuntime);
    break;
  case 0x56f450:
    WorldRuntime_SetPosition60AndDistanceFromPosition80
              (g_LevelCameraBookmark4PositionZQ12,g_LevelCameraBookmark4PositionYQ12,
               g_LevelCameraBookmark4PositionXQ12,worldRuntime);
    WorldRuntime_SetMotionParameters6CThrough78Clamped
              (2,extraout_EDX_02 >> 0x10,extraout_EAX_02 & 0xffff,
               g_LevelCameraBookmark4PositionMagnitudeQ12,worldRuntime);
    WorldRuntime_RecomputeMotionEndpointAgainstFieldSurface(worldRuntime);
    WorldRuntime_CommitScalar7CFrom8C(worldRuntime);
    break;
  case 0x56f4a0:
    WorldRuntime_SetPosition60AndDistanceFromPosition80
              (g_LevelCameraBookmark5PositionZQ12,g_LevelCameraBookmark5PositionYQ12,
               g_LevelCameraBookmark5PositionXQ12,worldRuntime);
    WorldRuntime_SetMotionParameters6CThrough78Clamped
              (2,extraout_EDX_03 >> 0x10,extraout_EAX_03 & 0xffff,
               g_LevelCameraBookmark5PositionMagnitudeQ12,worldRuntime);
    WorldRuntime_RecomputeMotionEndpointAgainstFieldSurface(worldRuntime);
    WorldRuntime_CommitScalar7CFrom8C(worldRuntime);
    break;
  case 0x56f4f0:
    WorldRuntime_SetPosition60AndDistanceFromPosition80
              (g_LevelCameraBookmark6PositionZQ12,g_LevelCameraBookmark6PositionYQ12,
               g_LevelCameraBookmark6PositionXQ12,worldRuntime);
    WorldRuntime_SetMotionParameters6CThrough78Clamped
              (2,extraout_EDX_04 >> 0x10,extraout_EAX_04 & 0xffff,
               g_LevelCameraBookmark6PositionMagnitudeQ12,worldRuntime);
    WorldRuntime_RecomputeMotionEndpointAgainstFieldSurface(worldRuntime);
    WorldRuntime_CommitScalar7CFrom8C(worldRuntime);
    break;
  case 0x56f540:
    WorldRuntime_SetPosition60AndDistanceFromPosition80
              (g_LevelCameraBookmark7PositionZQ12,g_LevelCameraBookmark7PositionYQ12,
               g_LevelCameraBookmark7PositionXQ12,worldRuntime);
    WorldRuntime_SetMotionParameters6CThrough78Clamped
              (2,extraout_EDX_05 >> 0x10,extraout_EAX_05 & 0xffff,
               g_LevelCameraBookmark7PositionMagnitudeQ12,worldRuntime);
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

/* Address: 0x0056F770.
   Ownership: gameplay/input/world.
   Purpose: This exact function owns the shared CF-clear epilogue at 0056F7C0 once; the other entries tail-jump
   into it. No executable bytes changed.
*/
undefined8 InGameCameraCommand_CaptureBookmarkSlot7EntryAndSharedSuccessTailCf(void)

{
  int unaff_EBX;
  int unaff_EBP;
  
  g_LevelCameraBookmark7PositionXQ12 = *(dword *)(unaff_EBX + 0x60);
  g_LevelCameraBookmark7PositionYQ12 = *(dword *)(unaff_EBX + 100);
  g_LevelCameraBookmark7PositionZQ12 = *(dword *)(unaff_EBX + 0x68);
  g_LevelCameraBookmark7PositionMagnitudeQ12 = *(dword *)(unaff_EBX + 0x6c);
  g_LevelCameraBookmark7PackedHeadingLow16PitchHigh16 =
       *(int *)(unaff_EBX + 0x74) << 0x10 | *(uint *)(unaff_EBX + 0x70);
  return CONCAT44(*(undefined4 *)(unaff_EBP + 0xc),*(undefined4 *)(unaff_EBP + 0x18));
}

/* Address: 0x0056D4B0.
   Ownership: gameplay/input/world.
   Purpose: Cancels targeting-context state 0x1B by resetting the context state, finding the root targeting object
   at +0xA30, clearing its active bit 0x10, and restoring the saved targeting fields. Queued UI action handler for
   INGAME_PAGE10[14] (0x100E). Return datatype is preserved for non-queue direct callers.
   Cross-module calls: WorldRuntime_RestoreMotionStateFromSnapshot [world/runtime/core].
*/
void InGameTargetingContext_CancelAndRestoreState(InGameTargetingContext *targetingContext)

{
  UiNodeBase *parentCursor;
  
  if (targetingContext->actionState == INGAME_TARGETING_OBSERVED_CANCEL_AND_RESTORE) {
    targetingContext->actionState = INGAME_TARGETING_OBSERVED_IDLE;
    parentCursor = (targetingContext->base).parent;
    while (parentCursor != (UiNodeBase *)0xffffffff) {
      targetingContext = (InGameTargetingContext *)(targetingContext->base).parent;
      parentCursor = (targetingContext->base).parent;
    }
    *(uint *)(targetingContext[10].reserved54_FF + 0x28) =
         *(uint *)(targetingContext[10].reserved54_FF + 0x28) & 0xffffffef;
    WorldRuntime_RestoreMotionStateFromSnapshot
              ((WorldRuntimeContext *)&targetingContext[10].base.leftAnchorQ31);
  }
  return;
}

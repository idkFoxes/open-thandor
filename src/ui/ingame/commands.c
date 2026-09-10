#include <thandor/ui/ingame/commands.h>

/* Implementation ownership: ui/ingame/commands. */

/* Address: 0x0056DB40.
   Ownership: ui/ingame/commands.
   Purpose: Action 0x1028. Selects six-choice command mode G value 0, synchronizes the three related page stacks,
   and applies the verified auxiliary visual-state combination for mode 0. Original user-facing label is not
   preserved. Queued UI action handler for INGAME_COMMAND_MODE_PAGE11[0] (0x1100). Return datatype is preserved for
   non-queue direct callers.
   Local calls: UiCommandModeG_SelectAndSyncPages, UiCommandModeG_SetNodeFlag00100000,
   UiCommandModeG_SetNodeFlag00200000, UiCommandModeG_ClearNodeFlags00000480, UiCommandModeG_SetNodeFlag00800000,
   UiCommandModeG_SetNodeFlag01000000, UiCommandModeG_ApplyRawColorVariant, UiCommandModeG_ClearNodeFlag02000000.
*/
void InGameCommandModeG_Select0(UiSelectableControl *source)

{
  int extraout_EDX;
  UiNodeBase *node;
  UiNodeBase *node_00;
  UiNodeBase *node_01;
  UiNodeBase *node_02;
  void *visualState;
  UiNodeBase *node_03;
  
  UiCommandModeG_SelectAndSyncPages(0,source);
  UiCommandModeG_SetNodeFlag00100000((UiNodeBase *)(extraout_EDX + 0xa30));
  UiCommandModeG_SetNodeFlag00200000(node);
  UiCommandModeG_ClearNodeFlags00000480(node_00);
  UiCommandModeG_SetNodeFlag00800000(node_01);
  UiCommandModeG_SetNodeFlag01000000(node_02);
  UiCommandModeG_ApplyRawColorVariant(visualState);
  UiCommandModeG_ClearNodeFlag02000000(node_03);
  return;
}

/* Address: 0x0056DB90.
   Ownership: ui/ingame/commands.
   Purpose: Action 0x1029. Selects six-choice command mode G value 1, synchronizes the three related page stacks,
   and applies the verified auxiliary visual-state combination for mode 1. Original user-facing label is not
   preserved. Queued UI action handler for INGAME_COMMAND_MODE_PAGE11[1] (0x1101). Return datatype is preserved for
   non-queue direct callers.
   Local calls: UiCommandModeG_SelectAndSyncPages, UiCommandModeG_SetNodeFlag00100000,
   UiCommandModeG_SetNodeFlag00200000, UiCommandModeG_ClearNodeFlags00000480, UiCommandModeG_SetNodeFlag00800000,
   UiCommandModeG_ClearNodeFlag01000000, UiCommandModeG_ApplyRawColorVariant, UiCommandModeG_ClearNodeFlag02000000.
*/
void InGameCommandModeG_Select1(UiSelectableControl *source)

{
  int extraout_EDX;
  UiNodeBase *node;
  UiNodeBase *node_00;
  UiNodeBase *node_01;
  UiNodeBase *node_02;
  void *visualState;
  UiNodeBase *node_03;
  
  UiCommandModeG_SelectAndSyncPages(1,source);
  UiCommandModeG_SetNodeFlag00100000((UiNodeBase *)(extraout_EDX + 0xa30));
  UiCommandModeG_SetNodeFlag00200000(node);
  UiCommandModeG_ClearNodeFlags00000480(node_00);
  UiCommandModeG_SetNodeFlag00800000(node_01);
  UiCommandModeG_ClearNodeFlag01000000(node_02);
  UiCommandModeG_ApplyRawColorVariant(visualState);
  UiCommandModeG_ClearNodeFlag02000000(node_03);
  return;
}

/* Address: 0x0056DBE0.
   Ownership: ui/ingame/commands.
   Purpose: Action 0x102A. Selects six-choice command mode G value 2, synchronizes the three related page stacks,
   and applies the verified auxiliary visual-state combination for mode 2. Original user-facing label is not
   preserved. Queued UI action handler for INGAME_COMMAND_MODE_PAGE11[2] (0x1102). Return datatype is preserved for
   non-queue direct callers.
   Local calls: UiCommandModeG_SelectAndSyncPages, UiCommandModeG_SetNodeFlag00100000,
   UiCommandModeG_ClearNodeFlag00200000, UiCommandModeG_ClearNodeFlags00000480, UiCommandModeG_SetNodeFlag00800000,
   UiCommandModeG_SetNodeFlag01000000, UiCommandModeG_ApplyMaskedColorVariant,
   UiCommandModeG_ClearNodeFlag02000000.
*/
void InGameCommandModeG_Select2(UiSelectableControl *source)

{
  int extraout_EDX;
  UiNodeBase *node;
  UiNodeBase *node_00;
  UiNodeBase *node_01;
  UiNodeBase *node_02;
  void *visualState;
  UiNodeBase *node_03;
  
  UiCommandModeG_SelectAndSyncPages(2,source);
  UiCommandModeG_SetNodeFlag00100000((UiNodeBase *)(extraout_EDX + 0xa30));
  UiCommandModeG_ClearNodeFlag00200000(node);
  UiCommandModeG_ClearNodeFlags00000480(node_00);
  UiCommandModeG_SetNodeFlag00800000(node_01);
  UiCommandModeG_SetNodeFlag01000000(node_02);
  UiCommandModeG_ApplyMaskedColorVariant(visualState);
  UiCommandModeG_ClearNodeFlag02000000(node_03);
  return;
}

/* Address: 0x0056DC30.
   Ownership: ui/ingame/commands.
   Purpose: Action 0x102D. Selects six-choice command mode G value 3, synchronizes the three related page stacks,
   applies its auxiliary visual state, updates the verified shared timestamp/state word, and refreshes the command-
   detail panel. Original user-facing label remains unresolved.
   Local calls: UiCommandModeG_SelectAndSyncPages, UiCommandModeG_ClearNodeFlag00100000,
   UiCommandModeG_ClearNodeFlag00200000, UiCommandModeG_SetNodeFlag00000400, UiCommandModeG_SetNodeFlag00800000,
   UiCommandModeG_ClearNodeFlag01000000, UiCommandModeG_ApplyRawColorVariant, UiCommandModeG_ClearNodeFlag02000000.
   Cross-module calls: ArmyAssetRegistry_FindByIdCf [assets/army/catalog], InGameSelectionDetailPanel_Rebuild
   [ui/ingame/runtime].
*/
void InGameCommandModeG_Select3(UiSelectableControl *source)

{
  int extraout_EDX;
  UiNodeBase *node;
  UiNodeBase *node_00;
  UiNodeBase *node_01;
  UiNodeBase *node_02;
  void *visualState;
  UiNodeBase *node_03;
  
  UiCommandModeG_SelectAndSyncPages(3,source);
  UiCommandModeG_ClearNodeFlag00100000((UiNodeBase *)(extraout_EDX + 0xa30));
  UiCommandModeG_ClearNodeFlag00200000(node);
  UiCommandModeG_SetNodeFlag00000400(node_00);
  UiCommandModeG_SetNodeFlag00800000(node_01);
  UiCommandModeG_ClearNodeFlag01000000(node_02);
  UiCommandModeG_ApplyRawColorVariant(visualState);
  UiCommandModeG_ClearNodeFlag02000000(node_03);
  ArmyAssetRegistry_FindByIdCf(g_UiCommandModeGArmyAssetId);
  g_UiHoverSelectionRecord = (UiCommandRuntimeRecordPrefix *)(*g_FatalErrorPrimaryDispatchCf)();
  InGameSelectionDetailPanel_Rebuild();
  return;
}

/* Address: 0x0056DCA0.
   Ownership: ui/ingame/commands.
   Purpose: Action 0x102E. Selects six-choice command mode G value 4, synchronizes the three related page stacks,
   and applies the verified auxiliary visual-state combination for mode 4. Original user-facing label is not
   preserved.
   Local calls: UiCommandModeG_SelectAndSyncPages, UiCommandModeG_ClearNodeFlag00100000,
   UiCommandModeG_ClearNodeFlag00200000, UiCommandModeG_SetNodeFlag00000400, UiCommandModeG_SetNodeFlag00800000,
   UiCommandModeG_ClearNodeFlag01000000, UiCommandModeG_ApplyRawColorVariant, UiCommandModeG_ClearNodeFlag02000000.
*/
void InGameCommandModeG_Select4(UiSelectableControl *source)

{
  int extraout_EDX;
  UiNodeBase *node;
  UiNodeBase *node_00;
  UiNodeBase *node_01;
  UiNodeBase *node_02;
  void *visualState;
  UiNodeBase *node_03;
  
  UiCommandModeG_SelectAndSyncPages(4,source);
  UiCommandModeG_ClearNodeFlag00100000((UiNodeBase *)(extraout_EDX + 0xa30));
  UiCommandModeG_ClearNodeFlag00200000(node);
  UiCommandModeG_SetNodeFlag00000400(node_00);
  UiCommandModeG_SetNodeFlag00800000(node_01);
  UiCommandModeG_ClearNodeFlag01000000(node_02);
  UiCommandModeG_ApplyRawColorVariant(visualState);
  UiCommandModeG_ClearNodeFlag02000000(node_03);
  return;
}

/* Address: 0x0056DCF0.
   Ownership: ui/ingame/commands.
   Purpose: Action 0x102C. Selects six-choice command mode G value 5, synchronizes the three related page stacks,
   applies its auxiliary visual state, and mirrors g_UiCommandModeF into the root-resident derived-control field at
   mode-control base +0xB4. Original user-facing label remains unresolved.
   Local calls: UiCommandModeG_SelectAndSyncPages, UiCommandModeG_SetNodeFlag00100000,
   UiCommandModeG_ClearNodeFlag00200000, UiCommandModeG_SetNodeFlag00000400, UiCommandModeG_SetNodeFlag00800000,
   UiCommandModeG_ClearNodeFlag01000000, UiCommandModeG_ApplyRawColorVariant, UiCommandModeG_SetNodeFlag02000000.
*/
void InGameCommandModeG_Select5(UiSelectableControl *source)

{
  int extraout_EDX;
  UiNodeBase *node;
  UiNodeBase *node_00;
  UiNodeBase *node_01;
  UiNodeBase *node_02;
  void *visualState;
  UiNodeBase *node_03;
  int extraout_EDX_00;
  
  UiCommandModeG_SelectAndSyncPages(5,source);
  UiCommandModeG_SetNodeFlag00100000((UiNodeBase *)(extraout_EDX + 0xa30));
  UiCommandModeG_ClearNodeFlag00200000(node);
  UiCommandModeG_SetNodeFlag00000400(node_00);
  UiCommandModeG_SetNodeFlag00800000(node_01);
  UiCommandModeG_ClearNodeFlag01000000(node_02);
  UiCommandModeG_ApplyRawColorVariant(visualState);
  UiCommandModeG_SetNodeFlag02000000(node_03);
  *(dword *)(extraout_EDX_00 + 0xb4) = g_UiCommandModeF;
  return;
}

/* Address: 0x0056AC50.
   Ownership: ui/ingame/commands.
   Purpose: Binary entry is anchored by g_UiActionPage10InitializedHandlers[27]@005624A0. Queued UI action handler
   for INGAME_PAGE10[27] (0x101B). Return datatype is preserved for non-queue direct callers.
   Local calls: UiCommandRuntimeFlags_ApplyClearSetToggleMasks.
   Cross-module calls: InGameCommandQueue_AppendLocalPlayerCommand [network/protocol/commands],
   FrontendPlayerRuntime_MarkReadyByIdAndUpdateAction101B [ui/frontend/player].
*/
undefined4 InGameCommandAction_SetFlag1000OrMarkReady(void *source)

{
  undefined4 in_EAX;
  undefined4 in_ECX;
  undefined4 in_EDX;
  
  if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_CLIENT) == SESSION_NETWORK_ROLE_LOCAL) {
    if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_NETWORKED_MASK) ==
        SESSION_NETWORK_ROLE_LOCAL) {
      UiCommandRuntimeFlags_ApplyClearSetToggleMasks(g_LocalPlayerRuntimeId,0,0x1000,0);
    }
    else {
      InGameCommandQueue_AppendLocalPlayerCommand(0x310,0,0x1000,0);
    }
  }
  else if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_NETWORKED_MASK) ==
           SESSION_NETWORK_ROLE_LOCAL) {
    FrontendPlayerRuntime_MarkReadyByIdAndUpdateAction101B(in_ECX,in_EDX,g_LocalPlayerRuntimeId);
  }
  else {
    InGameCommandQueue_AppendLocalPlayerCommand(0x470,0,0,0);
  }
  return in_EAX;
}

/* Address: 0x0056ACC0.
   Ownership: ui/ingame/commands.
   Purpose: Binary entry is anchored by g_UiActionPage10InitializedHandlers[9]@005624A0. Queued UI action handler
   for INGAME_PAGE10[9] (0x1009). Return datatype is preserved for non-queue direct callers.
   Local calls: UiCommandRuntimeFlags_ApplyClearSetToggleMasks.
   Cross-module calls: InGameCommandQueue_AppendLocalPlayerCommand [network/protocol/commands].
*/
undefined4 InGameCommandAction_ToggleRuntimeFlag0800(void *source)

{
  undefined4 in_EAX;
  
  if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_NETWORKED_MASK) ==
      SESSION_NETWORK_ROLE_LOCAL) {
    UiCommandRuntimeFlags_ApplyClearSetToggleMasks(g_LocalPlayerRuntimeId,0,0,0x800);
  }
  else {
    InGameCommandQueue_AppendLocalPlayerCommand(0x310,0,0,0x800);
  }
  return in_EAX;
}

/* Address: 0x0056D6C0.
   Ownership: ui/ingame/commands.
   Purpose: Action 0x1027. Finds the UI root, clears the selectable control at root offset 0x4388, invokes
   UiAction1003_ToggleInGameSettingsPage to close the settings page, then dispatches numeric operation 0x150
   through the queued or local path according to shared runtime mode bits. Original operation label remains
   unresolved. Queued UI action handler for INGAME_PAGE10[39] (0x1027). Return datatype is preserved for non-queue
   direct callers.
   Local calls: InGameCommand150_HandlePlayerDepartureAndOwnership.
   Cross-module calls: UiSelectableControl_SetSelected [ui/controls/lists],
   InGameSettingsPage_ToggleAndSynchronizeControls [ui/ingame/settings],
   InGameCommandQueue_AppendLocalPlayerCommand [network/protocol/commands].
*/
void InGameCommandState_CloseSettingsAndDispatchOperation150(UiNodeBase *source)

{
  UiNodeBase *parentCursor;
  
  parentCursor = source->parent;
  while (parentCursor != (UiNodeBase *)0xffffffff) {
    source = source->parent;
    parentCursor = source->parent;
  }
  UiSelectableControl_SetSelected(0,(UiSelectableControl *)&source[0xe3].topOffset);
  InGameSettingsPage_ToggleAndSynchronizeControls((UiSelectableControl *)&source[0xe3].topOffset);
  if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_NETWORKED_MASK) ==
      SESSION_NETWORK_ROLE_LOCAL) {
    InGameCommand150_HandlePlayerDepartureAndOwnership(g_LocalPlayerRuntimeId,0,0,2);
  }
  else {
    InGameCommandQueue_AppendLocalPlayerCommand(0x150,0,0,2);
  }
  return;
}

/* Address: 0x0056DFD0.
   Ownership: ui/ingame/commands.
   Purpose: Action 0x1038. Finds the source offset in the verified twelve-entry command-control table, adds the
   active twelve-entry page base, and delegates selection to UiCommandMatrix_SelectIndex. Unmapped controls are
   ignored.
   Local calls: UiCommandMatrix_SelectIndex.
*/
void InGameCommandMatrix_SelectMappedControl(UiNodeBase *source)

{
  UiNodeBase *root;
  int mappingsRemaining;
  int mappingIndex;
  UiNodeBase *ancestorCursor;
  
  ancestorCursor = source->parent;
  root = source;
  while (ancestorCursor != (UiNodeBase *)0xffffffff) {
    root = root->parent;
    ancestorCursor = root->parent;
  }
  mappingIndex = 0;
  mappingsRemaining = 0xc;
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
   Ownership: ui/ingame/commands.
   Purpose: Begins pointer activation for command-sprite controls: clears activationInputState, sets selected
   state, invalidates the root, and records the repeat/double-click marker when applicable.
   Cross-module calls: UiNode_InvalidateRoot [ui/core/runtime].
*/
void UiCommandSpriteButtonControl_BeginPress
               (UiPointerWheelDelta wheelDelta,UiPixelCoordinate pointerY,UiPixelCoordinate pointerX
               ,UiCommandSpriteButtonControl *control)

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
   Ownership: ui/ingame/commands.
   Purpose: Completes a non-right pointer activation, clears selected state, merges the filtered global input-state
   word into activationInputState, optionally plays the inherited sound, queues actionId, and invalidates the root.
   Cross-module calls: UiActionQueue_Enqueue [ui/core/runtime], UiNode_InvalidateRoot [ui/core/runtime].
*/
void UiCommandSpriteButtonControl_NonRightRelease
               (UiPointerWheelDelta wheelDelta,UiPixelCoordinate pointerY,UiPixelCoordinate pointerX
               ,UiCommandSpriteButtonControl *control)

{
  UiCommandActivationStateFlags UVar1;
  UiSelectableStateFlags *stateFlagsField;
  
  if ((((control->sprite).selectable.base.nodeFlags & UI_NODE_SUPPRESSED) == 0) &&
     (((control->sprite).selectable.stateFlags & UI_SELECTABLE_SELECTED_OR_CHECKED) != 0)) {
    UVar1 = g_KeyboardStateMask &
            ~(UI_COMMAND_ACTIVATION_ALTERNATE_BUTTON|UI_COMMAND_ACTIVATION_REPEAT_OR_DOUBLE_CLICK);
    stateFlagsField = &(control->sprite).selectable.stateFlags;
    *stateFlagsField = *stateFlagsField & ~UI_SELECTABLE_SELECTED_OR_CHECKED;
    control->activationInputState = control->activationInputState | UVar1;
    if ((((control->sprite).selectable.stateFlags & 0x200) != 0) &&
       ((control->sprite).activationSoundId != 0)) {
      (*g_SoundPlayOneShot)
                (g_UiSoundGainQ15,g_UiSoundGainQ15,
                 (DirectSoundVoiceSet *)(control->sprite).activationSoundId);
    }
    UiActionQueue_Enqueue((control->sprite).selectable.actionId,control);
    UiNode_InvalidateRoot((UiNodeBase *)control);
  }
  return;
}

/* Address: 0x00516410.
   Ownership: ui/ingame/commands.
   Purpose: Completes a right-button activation, captures the global input-state word with the alternate-activation
   marker, optionally plays the inherited activation sound, queues actionId, and invalidates the root.
   Cross-module calls: UiActionQueue_Enqueue [ui/core/runtime], UiNode_InvalidateRoot [ui/core/runtime].
*/
void UiCommandSpriteButtonControl_RightRelease
               (UiPointerWheelDelta wheelDelta,UiPixelCoordinate pointerY,UiPixelCoordinate pointerX
               ,UiCommandSpriteButtonControl *control)

{
  UiCommandActivationStateFlags UVar1;
  UiSelectableStateFlags *stateFlagsField;
  
  if ((((control->sprite).selectable.base.nodeFlags & UI_NODE_SUPPRESSED) == 0) &&
     (UVar1 = g_KeyboardStateMask & ~UI_COMMAND_ACTIVATION_REPEAT_OR_DOUBLE_CLICK,
     ((control->sprite).selectable.stateFlags & UI_SELECTABLE_SELECTED_OR_CHECKED) != 0)) {
    stateFlagsField = &(control->sprite).selectable.stateFlags;
    *stateFlagsField = *stateFlagsField & ~UI_SELECTABLE_SELECTED_OR_CHECKED;
    control->activationInputState = UVar1 | UI_COMMAND_ACTIVATION_ALTERNATE_BUTTON;
    if ((((control->sprite).selectable.stateFlags & 0x200) != 0) &&
       ((control->sprite).activationSoundId != 0)) {
      (*g_SoundPlayOneShot)
                (g_UiSoundGainQ15,g_UiSoundGainQ15,
                 (DirectSoundVoiceSet *)(control->sprite).activationSoundId);
    }
    UiActionQueue_Enqueue((control->sprite).selectable.actionId,control);
    UiNode_InvalidateRoot((UiNodeBase *)control);
  }
  return;
}

/* Address: 0x00516490.
   Ownership: ui/ingame/commands.
   Purpose: Maps one of up to 24 variant-A controls through the active control-offset table, stores the matching
   runtime record as the hover selection, refreshes the dependent UI, and returns cursor identifier 10 or 12.
   Cross-module calls: InGameSelectionDetailPanel_Rebuild [ui/ingame/runtime].
*/
GraphicsCursorFrameIndex
UiCommandSpriteVariantA_PointerMove
          (UiPixelCoordinate pointerY,UiPixelCoordinate pointerX,
          UiCommandSpriteButtonControl *control)

{
  GraphicsCursorFrameIndex GVar1;
  int recordIndex;
  
  if (((control->sprite).selectable.base.nodeFlags & UI_NODE_SUPPRESSED) == 0) {
    recordIndex = 0x17;
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
  GVar1 = 10;
  if ((g_KeyboardStateMask & 0xc) != 0) {
    GVar1 = 0xc;
  }
  return GVar1;
}

/* Address: 0x00517F60.
   Ownership: ui/ingame/commands.
   Purpose: Binary entry is anchored by g_UiNodeVtable_00517F10[2]@00517F10.
   Cross-module calls: UiWrappedTextControl_DrawClipped [ui/controls/text].
*/
void UiCommandVisibilityWrappedText_DrawWhenAllowed
               (UiPixelCoordinate clipTop,UiPixelCoordinate clipLeft,UiPixelCoordinate clipBottom,
               UiPixelCoordinate clipRight,UiNodeBase *control)

{
  if (((g_UiCommandRuntimeFlags & 0x200) == 0) &&
     (((((uint)control[1].nextSibling & 0x800) == 0 || ((g_UiCommandRuntimeFlags & 1) != 0)) &&
      ((control->nodeFlags & UI_NODE_SUPPRESSED) == 0)))) {
    UiWrappedTextControl_DrawClipped(clipTop,clipLeft,clipBottom,clipRight,control);
  }
  return;
}

/* Address: 0x00518010.
   Ownership: ui/ingame/commands.
   Purpose: Binary entry is anchored by g_UiNodeVtable_00517FC0[2]@00517FC0.
   Cross-module calls: UiSingleLineTextControl_DrawClipped [ui/controls/text].
*/
void UiCommandVisibilitySingleLineText_DrawWhenAllowed
               (UiPixelCoordinate clipTop,UiPixelCoordinate clipLeft,UiPixelCoordinate clipBottom,
               UiPixelCoordinate clipRight,UiNodeBase *control)

{
  int iVar1;
  int extraout_ECX;
  
  iVar1 = 0;
  if ((((g_UiCommandRuntimeFlags & 0x200) == 0) &&
      ((((uint)control[1].nextSibling & 0x800) == 0 || ((g_UiCommandRuntimeFlags & 1) != 0)))) &&
     ((((uint)control[1].nextSibling & 0x1000) == 0 ||
      (iVar1 = g_InGameSimulationStepTicks - 2, 1 < g_InGameSimulationStepTicks)))) {
    control[1].parent = (UiNodeBase *)((int)&(control[1].parent)->nextSibling + iVar1);
    UiSingleLineTextControl_DrawClipped(clipTop,clipLeft,clipBottom,clipRight,control);
    control[1].parent = (UiNodeBase *)((int)control[1].parent - extraout_ECX);
  }
  return;
}

/* Address: 0x0055F4A0.
   Ownership: ui/ingame/commands.
   Purpose: Handles in game command mode toggle player flag bit0 and reconcile global.
*/
undefined8 __fastcall
InGameCommandMode_TogglePlayerFlagBit0AndReconcileGlobal
          (undefined4 param_1,undefined4 param_2,int param_3)

{
  undefined4 in_EAX;
  FrontendPlayerRuntimeBlockCount FVar1;
  FrontendPlayerRuntimeRecord *pFVar2;
  
  g_SelectionPlayerRuntimeBlockPointers[param_3]->sessionFlags =
       g_SelectionPlayerRuntimeBlockPointers[param_3]->sessionFlags ^ 1;
  FVar1 = g_FrontendPlayerRuntimeBlockCount;
  pFVar2 = g_FrontendPlayerRuntimeBlocks;
  do {
    if ((g_UiCommandRuntimeFlags & 1) == 0) {
      if ((g_SelectionPlayerRuntimeBlockPointers[pFVar2->playerRuntimeId]->sessionFlags & 1) == 0)
      goto LAB_0055f513;
    }
    else if ((g_SelectionPlayerRuntimeBlockPointers[pFVar2->playerRuntimeId]->sessionFlags & 1) != 0
            ) goto LAB_0055f513;
    pFVar2 = pFVar2 + 1;
    FVar1 = FVar1 - 1;
  } while (FVar1 != 0);
  g_UiCommandRuntimeFlags = g_UiCommandRuntimeFlags ^ 1;
LAB_0055f513:
  return CONCAT44(param_2,in_EAX);
}

/* Address: 0x005604D0.
   Ownership: ui/ingame/commands.
   Purpose: Consumes the pending selected placement record, validates and creates the local army/effect object, and
   refreshes local in-game UI state. Typed parameters: p0 playerId→PlayerRuntimeId. Nearby but non-identical
   semantic domains were explicitly deferred. Calling convention, parameter storage, body bytes, control flow,
   globals, locals, and executable data remain unchanged. Typed parameters: p1
   payloadDword04→CommandPayloadDword04_V343, p2 payloadDword08→CommandPayloadDword08_V343, p3
   payloadDword0C→CommandPayloadDword0C_V343.
   Cross-module calls: ArmyPlacement_ValidateAssetAtPointAndCellCornersCf [gameplay/army/placement],
   ArmyRuntime_CreateInstanceFromAssetCf [gameplay/army/runtime], ModelNodeRuntime_RebuildTransformsFromRoot
   [world/model/hierarchy], ArmyRuntime_DispatchClassCommand [gameplay/army/runtime],
   EffectRuntimePool_CreateInstanceFromDefinitionCf [world/effects/runtime], UiCatalogGroup48_RebuildGrid
   [ui/ingame/technology].
*/
void InGameCommand_ExecuteLocalPlacementFromSelection
               (PlayerRuntimeId playerId,CommandPayloadDword04 payloadDword04,
               CommandPayloadDword08 payloadDword08,CommandPayloadDword0C payloadDword0C)

{
  dword *pdVar1;
  FactionRelationCounter *pFVar2;
  dword dVar3;
  SelectionPlayerRuntimeBlock *pSVar4;
  int iVar5;
  ModelRuntimeNode *modelNodeRuntime;
  undefined4 *puVar6;
  InGameRuntimeRootImageC3E4 *pIVar7;
  int extraout_EAX;
  ArmyRuntimeSlot *armySlot1;
  Q12 worldYQ12;
  int extraout_ECX;
  Q12 worldXQ12;
  int extraout_EDX;
  int extraout_EDX_00;
  WorldRuntimeContext *worldRuntime;
  bool bVar8;
  undefined8 uVar9;
  
  pIVar7 = g_InGameRuntimeRoot;
  pSVar4 = g_SelectionPlayerRuntimeBlockPointers[playerId];
  worldRuntime = &g_InGameRuntimeRoot->worldRuntime0A30;
  LOCK();
  pdVar1 = &pSVar4->pendingSelectionEntityOffset8098;
  dVar3 = *pdVar1;
  *pdVar1 = 0;
  UNLOCK();
  bVar8 = false;
  if (dVar3 != 0) {
    ArmyPlacement_ValidateAssetAtPointAndCellCornersCf
              (0,payloadDword04,payloadDword08,payloadDword0C,*(ArmyPlacementContext *)(dVar3 + 8),
               pSVar4->primaryEntityOrFactionToken8080,worldRuntime);
    if ((!bVar8) &&
       (armySlot1 = ArmyRuntime_CreateInstanceFromAssetCf
                              (4,payloadDword04,worldXQ12,worldYQ12,
                               *(FactionRuntimeIndex *)(extraout_EAX + 0x8080),
                               *(PckArmyAssetIdCatalog *)(dVar3 + 8),worldRuntime), !bVar8)) {
      iVar5 = *(int *)(extraout_EDX + 0x8080);
      modelNodeRuntime = armySlot1->modelNodeRuntime;
      puVar6 = armySlot1->definitionOrAsset;
      modelNodeRuntime->tintArgb = 0;
      if (iVar5 == (pIVar7->worldRuntime0A30).activeFactionRuntimeIndex) {
        modelNodeRuntime->tintArgb = 0x7fffffff;
      }
      pFVar2 = &g_GameFactionRuntimeImage.records[iVar5].relationCounterB;
      *pFVar2 = *pFVar2 + 1;
      uVar9 = ModelNodeRuntime_RebuildTransformsFromRoot(modelNodeRuntime,*puVar6,modelNodeRuntime);
      ArmyRuntime_DispatchClassCommand((ArmyRuntimeSlot **)uVar9,worldRuntime);
      EffectRuntimePool_CreateInstanceFromDefinitionCf
                (extraout_ECX,extraout_EDX_00,EFFECT_RUNTIME_COMPLETION_NONE,0,
                 *(AngleTurn32 *)(extraout_ECX + 0x14),*(AngleTurn32 *)(extraout_ECX + 0x10),
                 *(AngleTurn32 *)(extraout_ECX + 0xc),*(Q12 *)(extraout_ECX + 0x9c),
                 *(Q12 *)(extraout_ECX + 0x98),*(Q12 *)(extraout_ECX + 0x94),
                 *(EffectDefinition **)(extraout_EDX_00 + 400),worldRuntime);
      UiCatalogGroup48_RebuildGrid((UiNodeBase *)g_InGameRuntimeRoot);
      UiCatalogGroup42_RebuildGrid((UiNodeBase *)g_InGameRuntimeRoot);
      if (playerId != g_LocalPlayerRuntimeId) {
        return;
      }
      g_UiCommandRuntimeFlags = g_UiCommandRuntimeFlags & 0xffffffdf;
      g_InGamePendingPlacementArmyAsset = 0;
      return;
    }
    g_SelectionPlayerRuntimeBlockPointers[playerId]->pendingSelectionEntityOffset8098 = dVar3;
  }
  return;
}

/* Address: 0x0056A2A0.
   Ownership: ui/ingame/commands.
   Purpose: Collects up to 24 active runtime records for the current command context, chooses a compact grid,
   updates variant-A controls from each record's textureSource, suppresses unused controls, and relayouts the
   container.
   Cross-module calls: UiGrid_ComputeDimensionsPacked [ui/controls/layout].
*/
void UiCommandSpriteVariantA_RebuildGrid(UiNodeBase *node)

{
  uint *puVar1;
  UiNodeBase *pUVar2;
  sdword *psVar3;
  UiCommandRuntimeRecordPrefix *pUVar4;
  dword dVar5;
  GraphicsTextureSourceAsset *pGVar6;
  int iVar7;
  UiControlCount itemCount;
  uint extraout_ECX;
  FactionArmyAssetCount FVar8;
  int iVar9;
  uint uVar10;
  UiCommandRuntimeRecordPrefix **ppUVar11;
  dword *pdVar12;
  UiGridDimensionsEdxEax8 UVar13;
  
  pUVar2 = node->parent;
  while (pUVar2 != (UiNodeBase *)0xffffffff) {
    node = node->parent;
    pUVar2 = node->parent;
  }
  ppUVar11 = g_UiCommandSpriteVariantARecords;
  for (iVar7 = 0x18; iVar7 != 0; iVar7 = iVar7 + -1) {
    *ppUVar11 = (UiCommandRuntimeRecordPrefix *)0x0;
    ppUVar11 = ppUVar11 + 1;
  }
  ppUVar11 = g_UiCommandSpriteVariantARecords;
  FVar8 = g_GameFactionRuntimeImage.records[node[0x23].bottom].primaryArmyAssetCount;
  itemCount = 0;
  pdVar12 = g_GameFactionRuntimeImage.records[node[0x23].bottom].primaryArmyAssetPointersOrIds;
  if ((FVar8 != 0) && ((g_UiCommandRuntimeFlags & 0x100) == 0)) {
    do {
      if ((((UiCommandRuntimeRecordPrefix *)*pdVar12)->textureSource !=
           (GraphicsTextureSourceAsset *)0x0) && (itemCount < 0x18)) {
        *ppUVar11 = (UiCommandRuntimeRecordPrefix *)*pdVar12;
        itemCount = itemCount + 1;
        ppUVar11 = ppUVar11 + 1;
      }
      pdVar12 = pdVar12 + 1;
      FVar8 = FVar8 - 1;
    } while (FVar8 != 0);
  }
  UVar13 = UiGrid_ComputeDimensionsPacked(6,itemCount);
  dVar5 = (dword)UVar13;
  if (4 < dVar5) {
    dVar5 = 4;
  }
  iVar7 = dVar5 * g_InGamePanelTextureSubresource34Width + g_InGamePanelTextureSubresource27Width +
          g_InGamePanelTextureSubresource28Width;
  iVar9 = (int)(UVar13 >> 0x20) * g_InGamePanelTextureSubresource34Height +
          g_InGamePanelTextureSubresource26Height + g_InGamePanelTextureSubresource31Height;
  g_UiCommandSpriteVariantAColumnCount = dVar5;
  if ((int)g_FramebufferWidth < 800) {
    node[0x1da].leftOffset = -0x1f;
    node[0x1da].rightOffset = -0x1f;
    node[0x1da].topOffset = -0xd;
    node[0x1da].bottomOffset = -0xd;
  }
  else {
    node[0x1da].leftOffset = -0x27;
    node[0x1da].rightOffset = -0x27;
    node[0x1da].topOffset = -0x12;
    node[0x1da].bottomOffset = -0x12;
  }
  node[0x1da].leftOffset = node[0x1da].leftOffset - iVar7;
  node[0x1da].topOffset = node[0x1da].topOffset - iVar9;
  if (extraout_ECX == 0) {
    node[0x1da].nodeFlags = node[0x1da].nodeFlags | UI_NODE_SUPPRESSED;
  }
  else {
    node[0x1da].nodeFlags = node[0x1da].nodeFlags & ~UI_NODE_SUPPRESSED;
  }
  psVar3 = g_UiCommandSpriteVariantAOffsetTables[dVar5];
  uVar10 = 0;
  ppUVar11 = g_UiCommandSpriteVariantARecords;
  do {
    iVar7 = psVar3[uVar10];
    pUVar4 = *ppUVar11;
    if (uVar10 < extraout_ECX) {
      puVar1 = (uint *)((int)&node->nodeFlags + iVar7);
      *puVar1 = *puVar1 & 0xfffffff7;
      pGVar6 = pUVar4->textureSource;
    }
    else {
      puVar1 = (uint *)((int)&node->nodeFlags + iVar7);
      *puVar1 = *puVar1 | 8;
      pGVar6 = (GraphicsTextureSourceAsset *)0x0;
    }
    uVar10 = uVar10 + 1;
    *(GraphicsTextureSourceAsset **)((int)&node[1].parent + iVar7) = pGVar6;
    ppUVar11 = ppUVar11 + 1;
  } while (uVar10 < 0x18);
  (**(code **)(node[0x1d8].rightAnchorQ31 + 0xc))(&node[0x1d8].bottomOffset);
  return;
}

/* Address: 0x0056AFD0.
   Ownership: ui/ingame/commands.
   Purpose: Binary entry is anchored by g_UiActionPage10InitializedHandlers[17]@005624A0. Queued UI action handler
   for INGAME_PAGE10[17] (0x1011). Return datatype is preserved for non-queue direct callers. Typed parameters: p0
   control→UiNodeBase *. Calling convention, complete VariableStorage serialization, function bytes, control flow,
   globals, locals, and executable data remain unchanged.
   Cross-module calls: UiPageStack_SetActiveIndex [ui/controls/layout], SelectionInfo_GetFirstEntry
   [gameplay/selection/runtime], FrontendPlayerRuntime_ClearArmyTokenAndRestoreOrApplyTechnology
   [ui/frontend/player], InGameCommandQueue_AppendLocalPlayerCommand [network/protocol/commands].
*/
undefined8 InGameCommandAction_ClearSelectedArmyTokenAndClosePage(UiNodeBase *control)

{
  UiNodeBase *pUVar1;
  undefined4 in_EAX;
  GameEntityRuntime *entityRuntime1;
  undefined4 in_EDX;
  CommandPayloadDword04 modelOffset;
  
  pUVar1 = control->parent;
  while (pUVar1 != (UiNodeBase *)0xffffffff) {
    control = control->parent;
    pUVar1 = control->parent;
  }
  control[0x23].top = control[0x23].top & 0xfffffff7;
  UiPageStack_SetActiveIndex(0,(UiPageStackControl *)&control[0x27].bottomAnchorQ31);
  entityRuntime1 = SelectionInfo_GetFirstEntry();
  if (entityRuntime1 != (GameEntityRuntime *)0x0) {
    modelOffset = (int)(entityRuntime1->common).ownership.definitionOrClassRecord -
                  g_ModelRuntimeRebaseDelta;
    if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_NETWORKED_MASK) ==
        SESSION_NETWORK_ROLE_LOCAL) {
      FrontendPlayerRuntime_ClearArmyTokenAndRestoreOrApplyTechnology
                (g_LocalPlayerRuntimeId,0,0xffffffff,modelOffset);
    }
    else {
      InGameCommandQueue_AppendLocalPlayerCommand(0x1700,0,0xffffffff,modelOffset);
    }
  }
  return CONCAT44(in_EDX,in_EAX);
}

/* Address: 0x0056C660.
   Ownership: ui/ingame/commands.
   Purpose: Binary entry is anchored by g_UiActionPage12InitializedHandlers[0]@005625B8. Queued UI action handler
   for INGAME_PAGE12[0] (0x1200). Return datatype is preserved for non-queue direct callers. Typed parameters: p0
   source→InGameCommandPanelSourceAddress32_V345. Calling convention, complete VariableStorage serialization,
   function bytes, control flow, globals, locals, and executable data remain unchanged.
   Cross-module calls: UiPageStack_SetActiveIndex [ui/controls/layout], UiNodeList_UnsuppressActionId
   [ui/controls/lists], UiNodeList_SuppressActionId [ui/controls/lists].
*/
undefined4
InGameCommandPanel_OpenPage4AndRefreshAvailability(InGameCommandPanelSourceAddress32 source)

{
  undefined4 in_EAX;
  UiNodeBase *extraout_EAX;
  UiNodeBase *extraout_EAX_00;
  UiNodeBase *firstNode;
  
  UiPageStack_SetActiveIndex(4,(UiPageStackControl *)(source + -0x19e0));
  if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_NETWORKED_MASK) ==
      SESSION_NETWORK_ROLE_LOCAL) {
    UiNodeList_UnsuppressActionId(0x1027,(UiNodeBase *)(source + -0x25b0));
    firstNode = extraout_EAX_00;
  }
  else {
    UiNodeList_SuppressActionId(0x1027,(UiNodeBase *)(source + -0x25b0));
    firstNode = extraout_EAX;
  }
  if ((g_UiCommandRuntimeFlags & 0x100) == 0) {
    UiNodeList_UnsuppressActionId(0x101e,firstNode);
  }
  else {
    UiNodeList_SuppressActionId(0x101e,firstNode);
  }
  return in_EAX;
}

/* Address: 0x0056CFA0.
   Ownership: ui/ingame/commands.
   Purpose: Finds the UI root, resolves the source catalog control through the active 48-entry grid offset table,
   obtains the matching UiCommandRuntimeRecordPrefix.backendPayload, and submits backend command variant 0x0FE0 or
   0x1030 according to activationInputState bits 0x0C. Runtime flags 0x0101 inhibit submission. Queued UI action
   handler for INGAME_PAGE10[11] (0x100B). Return datatype is preserved for non-queue direct callers.
   Cross-module calls: GameFactionRuntime_RegisterArmyAssetPointers [gameplay/faction/runtime],
   InGameCommandQueue_AppendLocalPlayerCommand [network/protocol/commands],
   GameFactionRuntime_CancelQueuedArmyAssetsAndRefund [gameplay/faction/runtime].
*/
void InGameCommandCatalog_SubmitGroup48Entry(UiCatalogEntryControl *source)

{
  UiNodeBase *pUVar1;
  UiCatalogEntryControl *pUVar2;
  PckArmyAssetIdCatalog PVar3;
  int iVar4;
  
  if ((g_UiCommandRuntimeFlags & 0x101) == 0) {
    pUVar1 = (source->command).sprite.selectable.base.parent;
    pUVar2 = source;
    while (pUVar1 != (UiNodeBase *)0xffffffff) {
      pUVar2 = (UiCatalogEntryControl *)(pUVar2->command).sprite.selectable.base.parent;
      pUVar1 = (pUVar2->command).sprite.selectable.base.parent;
    }
    iVar4 = 0x2f;
    while ((int)source - (int)pUVar2 !=
           g_UiCatalogGroup48OffsetTables[g_UiCatalogGroup48ColumnCount][iVar4]) {
      iVar4 = iVar4 + -1;
      if (iVar4 < 0) {
        return;
      }
    }
    if (((source->command).activationInputState & UI_COMMAND_ACTIVATION_RELATION_RESET_REQUEST_MASK)
        == 0) {
      pUVar1 = pUVar2[0x15].command.sprite.selectable.base.nextSibling;
      PVar3 = g_UiCatalogGroup48Records[iVar4]->armyAssetId;
      if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_NETWORKED_MASK) ==
          SESSION_NETWORK_ROLE_LOCAL) {
        GameFactionRuntime_RegisterArmyAssetPointers
                  (g_LocalPlayerRuntimeId,1,PVar3,(FactionRuntimeIndex)pUVar1);
      }
      else {
        InGameCommandQueue_AppendLocalPlayerCommand(0xfe0,1,PVar3,(CommandPayloadDword04)pUVar1);
      }
    }
    else {
      pUVar1 = pUVar2[0x15].command.sprite.selectable.base.nextSibling;
      PVar3 = g_UiCatalogGroup48Records[iVar4]->armyAssetId;
      if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_NETWORKED_MASK) ==
          SESSION_NETWORK_ROLE_LOCAL) {
        GameFactionRuntime_CancelQueuedArmyAssetsAndRefund
                  (g_LocalPlayerRuntimeId,1,PVar3,(FactionRuntimeIndex)pUVar1);
      }
      else {
        InGameCommandQueue_AppendLocalPlayerCommand(0x1030,1,PVar3,(CommandPayloadDword04)pUVar1);
      }
    }
  }
  return;
}

/* Address: 0x0056D090.
   Ownership: ui/ingame/commands.
   Purpose: Finds the UI root, resolves the source catalog control through the active 42-entry grid offset table,
   obtains the matching UiCommandRuntimeRecordPrefix.backendPayload, and submits backend command variant 0x0FE0 or
   0x1030 according to activationInputState bits 0x0C. Runtime flags 0x0101 inhibit submission. Queued UI action
   handler for INGAME_PAGE10[12] (0x100C). Return datatype is preserved for non-queue direct callers.
   Cross-module calls: GameFactionRuntime_RegisterArmyAssetPointers [gameplay/faction/runtime],
   InGameCommandQueue_AppendLocalPlayerCommand [network/protocol/commands],
   GameFactionRuntime_CancelQueuedArmyAssetsAndRefund [gameplay/faction/runtime].
*/
void InGameCommandCatalog_SubmitGroup42Entry(UiCatalogEntryControl *source)

{
  UiNodeBase *pUVar1;
  UiCatalogEntryControl *pUVar2;
  PckArmyAssetIdCatalog PVar3;
  int iVar4;
  
  if ((g_UiCommandRuntimeFlags & 0x101) == 0) {
    pUVar1 = (source->command).sprite.selectable.base.parent;
    pUVar2 = source;
    while (pUVar1 != (UiNodeBase *)0xffffffff) {
      pUVar2 = (UiCatalogEntryControl *)(pUVar2->command).sprite.selectable.base.parent;
      pUVar1 = (pUVar2->command).sprite.selectable.base.parent;
    }
    iVar4 = 0x29;
    while ((int)source - (int)pUVar2 !=
           g_UiCatalogGroup42OffsetTables[g_UiCatalogGroup42ColumnCount][iVar4]) {
      iVar4 = iVar4 + -1;
      if (iVar4 < 0) {
        return;
      }
    }
    if (((source->command).activationInputState & UI_COMMAND_ACTIVATION_RELATION_RESET_REQUEST_MASK)
        == 0) {
      pUVar1 = pUVar2[0x15].command.sprite.selectable.base.nextSibling;
      PVar3 = g_UiCatalogGroup42Records[iVar4]->armyAssetId;
      if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_NETWORKED_MASK) ==
          SESSION_NETWORK_ROLE_LOCAL) {
        GameFactionRuntime_RegisterArmyAssetPointers
                  (g_LocalPlayerRuntimeId,1,PVar3,(FactionRuntimeIndex)pUVar1);
      }
      else {
        InGameCommandQueue_AppendLocalPlayerCommand(0xfe0,1,PVar3,(CommandPayloadDword04)pUVar1);
      }
    }
    else {
      pUVar1 = pUVar2[0x15].command.sprite.selectable.base.nextSibling;
      PVar3 = g_UiCatalogGroup42Records[iVar4]->armyAssetId;
      if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_NETWORKED_MASK) ==
          SESSION_NETWORK_ROLE_LOCAL) {
        GameFactionRuntime_CancelQueuedArmyAssetsAndRefund
                  (g_LocalPlayerRuntimeId,1,PVar3,(FactionRuntimeIndex)pUVar1);
      }
      else {
        InGameCommandQueue_AppendLocalPlayerCommand(0x1030,1,PVar3,(CommandPayloadDword04)pUVar1);
      }
    }
  }
  return;
}

/* Address: 0x0056D180.
   Ownership: ui/ingame/commands.
   Purpose: Handles numeric action 0x1001 for a variant-A command-sprite control. It resolves the control index
   through the active 24-slot offset table, loads the matching runtime record, and chooses a backend operation from
   activationInputState bits. Queued UI action handler for INGAME_PAGE10[1] (0x1001). Return datatype is preserved
   for non-queue direct callers.
   Cross-module calls: GameFactionRuntime_ConsumePendingArmyAssetAndRefreshGrid [gameplay/faction/runtime],
   InGameCommandQueue_AppendLocalPlayerCommand [network/protocol/commands],
   GameFactionRuntime_RemoveArmyAssetAndStagePlayerTransfer [gameplay/faction/runtime],
   GameFactionRuntime_SellArmyAssetAndRefundSevenEighths [gameplay/faction/runtime].
*/
void InGameCommandSprite_DispatchVariantAControl24(UiCommandSpriteButtonControl *control)

{
  sdword *psVar1;
  UiNodeBase *pUVar2;
  UiCommandSpriteButtonControl *pUVar3;
  UiCommandRuntimeRecordPrefix *pUVar4;
  GraphicsTextureSourceAsset *payloadDword04;
  PckArmyAssetIdCatalog PVar5;
  int extraout_EAX;
  int extraout_EAX_00;
  int iVar6;
  int extraout_EDX;
  int extraout_EDX_00;
  int iVar7;
  
  if ((g_UiCommandRuntimeFlags & 0x101) == 0) {
    pUVar2 = (control->sprite).selectable.base.parent;
    pUVar3 = control;
    while (pUVar2 != (UiNodeBase *)0xffffffff) {
      pUVar3 = (UiCommandSpriteButtonControl *)(pUVar3->sprite).selectable.base.parent;
      pUVar2 = (pUVar3->sprite).selectable.base.parent;
    }
    g_UiImageControlHoverTarget = (UiImageControl *)0x0;
    psVar1 = &pUVar3[0x122].sprite.selectable.base.leftOffset;
    *psVar1 = *psVar1 & 0xfffff9fc;
    if ((pUVar3[0x15].sprite.selectable.actionId & 0x10U) == 0) {
      iVar6 = 0x17;
      while ((int)control - (int)pUVar3 !=
             g_UiCommandSpriteVariantAOffsetTables[g_UiCommandSpriteVariantAColumnCount][iVar6]) {
        iVar6 = iVar6 + -1;
        if (iVar6 < 0) {
          return;
        }
      }
      pUVar4 = g_UiCommandSpriteVariantARecords[iVar6];
      payloadDword04 = pUVar3[0x15].sprite.primaryTextureSource;
      if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_NETWORKED_MASK) ==
          SESSION_NETWORK_ROLE_LOCAL) {
        GameFactionRuntime_ConsumePendingArmyAssetAndRefreshGrid
                  (g_LocalPlayerRuntimeId,0,0,(FactionRuntimeIndex)payloadDword04);
        iVar6 = extraout_EAX_00;
        iVar7 = extraout_EDX_00;
      }
      else {
        InGameCommandQueue_AppendLocalPlayerCommand
                  (0x14f0,0,0,(CommandPayloadDword04)payloadDword04);
        iVar6 = extraout_EAX;
        iVar7 = extraout_EDX;
      }
      if ((*(uint *)(iVar7 + 0x78) & 0xc) == 0) {
        PVar5 = pUVar4->armyAssetId;
        if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_NETWORKED_MASK) ==
            SESSION_NETWORK_ROLE_LOCAL) {
          GameFactionRuntime_RemoveArmyAssetAndStagePlayerTransfer
                    (g_LocalPlayerRuntimeId,0,PVar5,*(CommandPayloadDword04 *)(iVar6 + 0xa80));
        }
        else {
          InGameCommandQueue_AppendLocalPlayerCommand
                    (0x12d0,0,PVar5,*(CommandPayloadDword04 *)(iVar6 + 0xa80));
        }
      }
      else {
        PVar5 = pUVar4->armyAssetId;
        if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_NETWORKED_MASK) ==
            SESSION_NETWORK_ROLE_LOCAL) {
          GameFactionRuntime_SellArmyAssetAndRefundSevenEighths
                    (g_LocalPlayerRuntimeId,0,PVar5,*(CommandPayloadDword04 *)(iVar6 + 0xa80));
        }
        else {
          InGameCommandQueue_AppendLocalPlayerCommand
                    (0x1570,0,PVar5,*(CommandPayloadDword04 *)(iVar6 + 0xa80));
        }
      }
    }
  }
  return;
}

/* Address: 0x0056D540.
   Ownership: ui/ingame/commands.
   Purpose: Handles numeric action 0x100A for one of eight fixed command-sprite controls. It resolves the control
   through g_UiAction100AControlOffsets, decodes low-nibble, alternate, and repeat markers from
   activationInputState, and dispatches the backend command. Queued UI action handler for INGAME_PAGE10[10]
   (0x100A). Return datatype is preserved for non-queue direct callers.
   Cross-module calls: SelectionInfo_AllEntriesEmptyOrMatchOwnerCf [gameplay/selection/runtime],
   InGameCommandQueue_AppendLocalPlayerCommand [network/protocol/commands],
   FrontendPlayerSelection_TransferFactionGroupWithModeAndRefresh [ui/frontend/player].
*/
void InGameCommandSprite_DispatchFixedControl8(UiCommandSpriteButtonControl *control)

{
  UiNodeBase *pUVar1;
  UiCommandSpriteButtonControl *pUVar2;
  CommandPayloadDword0C payloadDword0C;
  CommandPayloadDword04 payloadDword04;
  CommandPayloadDword04 extraout_ECX;
  uint uVar3;
  CommandPayloadDword08 transferModeFlags;
  bool bVar4;
  ulonglong uVar5;
  
  if ((g_UiCommandRuntimeFlags & 0x101) == 0) {
    pUVar1 = (control->sprite).selectable.base.parent;
    pUVar2 = control;
    while (pUVar1 != (UiNodeBase *)0xffffffff) {
      pUVar2 = (UiCommandSpriteButtonControl *)(pUVar2->sprite).selectable.base.parent;
      pUVar1 = (pUVar2->sprite).selectable.base.parent;
    }
    payloadDword04 = 7;
    do {
      if ((int)control - (int)pUVar2 == g_UiAction100AControlOffsets[payloadDword04]) {
        uVar3 = 0;
        if ((control->activationInputState & UI_COMMAND_ACTIVATION_LOW_INPUT_NIBBLE_MASK) != 0) {
          uVar3 = 2;
        }
        if ((control->activationInputState & UI_COMMAND_ACTIVATION_ALTERNATE_BUTTON) != 0) {
          uVar3 = uVar3 | 1;
        }
        if ((control->activationInputState & UI_COMMAND_ACTIVATION_REPEAT_OR_DOUBLE_CLICK) != 0) {
          uVar3 = uVar3 | 4;
        }
        bVar4 = false;
        uVar5 = ZEXT48(pUVar2);
        if ((uVar3 != 0) &&
           (uVar5 = SelectionInfo_AllEntriesEmptyOrMatchOwnerCf
                              ((FactionRuntimeIndex)pUVar2[0x15].sprite.primaryTextureSource),
           payloadDword04 = extraout_ECX, bVar4)) {
          return;
        }
        transferModeFlags = (CommandPayloadDword08)(uVar5 >> 0x20);
        payloadDword0C = *(CommandPayloadDword0C *)((int)uVar5 + 0xa80);
        if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_NETWORKED_MASK) !=
            SESSION_NETWORK_ROLE_LOCAL) {
          InGameCommandQueue_AppendLocalPlayerCommand
                    (0xbe0,payloadDword0C,transferModeFlags,payloadDword04);
          return;
        }
        FrontendPlayerSelection_TransferFactionGroupWithModeAndRefresh
                  (g_LocalPlayerRuntimeId,payloadDword0C,transferModeFlags,payloadDword04);
        return;
      }
      payloadDword04 = payloadDword04 - 1;
    } while (-1 < (int)payloadDword04);
  }
  return;
}

/* Address: 0x0056D620.
   Ownership: ui/ingame/commands.
   Purpose: Action 0x1025. Sets bit 0x00001000 in g_UiCommandRuntimeFlags. The source argument is unused. The
   original user-facing label and broader bit meaning are not preserved. Queued UI action handler for
   INGAME_PAGE10[37] (0x1025).
*/
void InGameCommandState_SetRuntimeFlag1000(UiNodeBase *source)

{
  g_UiCommandRuntimeFlags = g_UiCommandRuntimeFlags | 0x1000;
  return;
}

/* Address: 0x0056D640.
   Ownership: ui/ingame/commands.
   Purpose: Action 0x1026. Finds the UI root, exclusively selects the source among controls at root offsets 0x764
   and 0x7C4, obtains the visible selected index 0 or 1, and mirrors that index into root offsets 0x454, 0x4D0,
   0x54C, and 0x3A8. Original field and mode labels remain unresolved. Queued UI action handler for
   INGAME_PAGE10[38] (0x1026). Return datatype is preserved for non-queue direct callers.
   Cross-module calls: UiSelectableGroup_SelectExclusive [ui/controls/lists],
   UiSelectableGroup_NoneVisibleSelectedCf [ui/controls/lists].
*/
void InGameCommandState_SelectAndPropagateBinaryMode(UiSelectableControl *source)

{
  UiNodeBase *pUVar1;
  UiSelectableControl *pUVar2;
  int iVar3;
  undefined4 extraout_ECX;
  
  pUVar1 = (source->base).parent;
  pUVar2 = source;
  while (pUVar1 != (UiNodeBase *)0xffffffff) {
    pUVar2 = (UiSelectableControl *)(pUVar2->base).parent;
    pUVar1 = (pUVar2->base).parent;
  }
  iVar3 = UiSelectableGroup_SelectExclusive(2,&source->base);
  UiSelectableGroup_NoneVisibleSelectedCf(2);
  *(undefined4 *)(iVar3 + 0x454) = extraout_ECX;
  *(undefined4 *)(iVar3 + 0x4d0) = extraout_ECX;
  *(undefined4 *)(iVar3 + 0x54c) = extraout_ECX;
  *(undefined4 *)(iVar3 + 0x3a8) = extraout_ECX;
  return;
}

/* Address: 0x0056D920.
   Ownership: ui/ingame/commands.
   Purpose: Clears UiNodeBase.nodeFlags bit 0x00800000. The original visual-state label is not preserved.
*/
void UiCommandModeG_ClearNodeFlag00800000(UiNodeBase *node)

{
  node[1].nextSibling = (UiNodeBase *)((uint)node[1].nextSibling & 0xff7fffff);
  return;
}

/* Address: 0x0056DD50.
   Ownership: ui/ingame/commands.
   Purpose: Action 0x1030. Exclusively selects the source within a four-control UiSpriteButtonControl group and
   stores command mode C value 0. Original user-facing mode label is not preserved.
   Cross-module calls: UiSelectableGroup_SelectExclusive [ui/controls/lists].
*/
void InGameCommandModeC_Select0(UiSpriteButtonControl *source)

{
  UiSelectableGroup_SelectExclusive(4,(UiNodeBase *)source);
  g_UiCommandModeC = 0;
  return;
}

/* Address: 0x0056DDA0.
   Ownership: ui/ingame/commands.
   Purpose: Action 0x1031. Exclusively selects the source within a four-control UiSpriteButtonControl group and
   stores command mode C value 1. Original user-facing mode label is not preserved.
   Cross-module calls: UiSelectableGroup_SelectExclusive [ui/controls/lists].
*/
void InGameCommandModeC_Select1(UiSpriteButtonControl *source)

{
  UiSelectableGroup_SelectExclusive(4,(UiNodeBase *)source);
  g_UiCommandModeC = 1;
  return;
}

/* Address: 0x0056DDF0.
   Ownership: ui/ingame/commands.
   Purpose: Action 0x1032. Exclusively selects the source within a four-control UiSpriteButtonControl group and
   stores command mode C value 2. Original user-facing mode label is not preserved.
   Cross-module calls: UiSelectableGroup_SelectExclusive [ui/controls/lists].
*/
void InGameCommandModeC_Select2(UiSpriteButtonControl *source)

{
  UiSelectableGroup_SelectExclusive(4,(UiNodeBase *)source);
  g_UiCommandModeC = 2;
  return;
}

/* Address: 0x0056DE40.
   Ownership: ui/ingame/commands.
   Purpose: Action 0x1033. Exclusively selects the source within a four-control UiSpriteButtonControl group and
   stores command mode C value 3. Original user-facing mode label is not preserved.
   Cross-module calls: UiSelectableGroup_SelectExclusive [ui/controls/lists].
*/
void InGameCommandModeC_Select3(UiSpriteButtonControl *source)

{
  UiSelectableGroup_SelectExclusive(4,(UiNodeBase *)source);
  g_UiCommandModeC = 3;
  return;
}

/* Address: 0x0056DE90.
   Ownership: ui/ingame/commands.
   Purpose: Action 0x1034. Exclusively selects the source within a four-control UiSpriteButtonControl group and
   stores command mode D value 0. Original user-facing mode label is not preserved.
   Cross-module calls: UiSelectableGroup_SelectExclusive [ui/controls/lists].
*/
void InGameCommandModeD_Select0(UiSpriteButtonControl *source)

{
  UiSelectableGroup_SelectExclusive(4,(UiNodeBase *)source);
  g_UiCommandModeD = 0;
  return;
}

/* Address: 0x0056DEE0.
   Ownership: ui/ingame/commands.
   Purpose: Action 0x1035. Exclusively selects the source within a four-control UiSpriteButtonControl group and
   stores command mode D value 1. Original user-facing mode label is not preserved.
   Cross-module calls: UiSelectableGroup_SelectExclusive [ui/controls/lists].
*/
void InGameCommandModeD_Select1(UiSpriteButtonControl *source)

{
  UiSelectableGroup_SelectExclusive(4,(UiNodeBase *)source);
  g_UiCommandModeD = 1;
  return;
}

/* Address: 0x0056DF30.
   Ownership: ui/ingame/commands.
   Purpose: Action 0x1036. Exclusively selects the source within a four-control UiSpriteButtonControl group and
   stores command mode D value 2. Original user-facing mode label is not preserved.
   Cross-module calls: UiSelectableGroup_SelectExclusive [ui/controls/lists].
*/
void InGameCommandModeD_Select2(UiSpriteButtonControl *source)

{
  UiSelectableGroup_SelectExclusive(4,(UiNodeBase *)source);
  g_UiCommandModeD = 2;
  return;
}

/* Address: 0x0056DF80.
   Ownership: ui/ingame/commands.
   Purpose: Action 0x1037. Exclusively selects the source within a four-control UiSpriteButtonControl group and
   stores command mode D value 3. Original user-facing mode label is not preserved.
   Cross-module calls: UiSelectableGroup_SelectExclusive [ui/controls/lists].
*/
void InGameCommandModeD_Select3(UiSpriteButtonControl *source)

{
  UiSelectableGroup_SelectExclusive(4,(UiNodeBase *)source);
  g_UiCommandModeD = 3;
  return;
}

/* Address: 0x0056E050.
   Ownership: ui/ingame/commands.
   Purpose: Action 0x1039. Exclusively selects the source within a three-control UiSpriteButtonControl group and
   stores command mode A value 0. Original user-facing mode label is not preserved.
   Cross-module calls: UiSelectableGroup_SelectExclusive [ui/controls/lists].
*/
void InGameCommandModeA_Select0(UiSpriteButtonControl *source)

{
  UiSelectableGroup_SelectExclusive(3,(UiNodeBase *)source);
  g_UiCommandModeA = 0;
  return;
}

/* Address: 0x0056E090.
   Ownership: ui/ingame/commands.
   Purpose: Action 0x103A. Exclusively selects the source within a three-control UiSpriteButtonControl group and
   stores command mode A value 1. Original user-facing mode label is not preserved.
   Cross-module calls: UiSelectableGroup_SelectExclusive [ui/controls/lists].
*/
void InGameCommandModeA_Select1(UiSpriteButtonControl *source)

{
  UiSelectableGroup_SelectExclusive(3,(UiNodeBase *)source);
  g_UiCommandModeA = 1;
  return;
}

/* Address: 0x0056E0D0.
   Ownership: ui/ingame/commands.
   Purpose: Action 0x103B. Exclusively selects the source within a three-control UiSpriteButtonControl group and
   stores command mode A value 2. Original user-facing mode label is not preserved.
   Cross-module calls: UiSelectableGroup_SelectExclusive [ui/controls/lists].
*/
void InGameCommandModeA_Select2(UiSpriteButtonControl *source)

{
  UiSelectableGroup_SelectExclusive(3,(UiNodeBase *)source);
  g_UiCommandModeA = 2;
  return;
}

/* Address: 0x0056E110.
   Ownership: ui/ingame/commands.
   Purpose: Action 0x103C. Exclusively selects the source within a three-control UiSpriteButtonControl group and
   stores command mode B value 0. Original user-facing mode label is not preserved.
   Cross-module calls: UiSelectableGroup_SelectExclusive [ui/controls/lists].
*/
void InGameCommandModeB_Select0(UiSpriteButtonControl *source)

{
  UiSelectableGroup_SelectExclusive(3,(UiNodeBase *)source);
  g_UiCommandModeB = 0;
  return;
}

/* Address: 0x0056E150.
   Ownership: ui/ingame/commands.
   Purpose: Action 0x103D. Exclusively selects the source within a three-control UiSpriteButtonControl group and
   stores command mode B value 1. Original user-facing mode label is not preserved.
   Cross-module calls: UiSelectableGroup_SelectExclusive [ui/controls/lists].
*/
void InGameCommandModeB_Select1(UiSpriteButtonControl *source)

{
  UiSelectableGroup_SelectExclusive(3,(UiNodeBase *)source);
  g_UiCommandModeB = 1;
  return;
}

/* Address: 0x0056E190.
   Ownership: ui/ingame/commands.
   Purpose: Action 0x103E. Exclusively selects the source within a three-control UiSpriteButtonControl group and
   stores command mode B value 2. Original user-facing mode label is not preserved.
   Cross-module calls: UiSelectableGroup_SelectExclusive [ui/controls/lists].
*/
void InGameCommandModeB_Select2(UiSpriteButtonControl *source)

{
  UiSelectableGroup_SelectExclusive(3,(UiNodeBase *)source);
  g_UiCommandModeB = 2;
  return;
}

/* Address: 0x0056E1D0.
   Ownership: ui/ingame/commands.
   Purpose: Action 0x103F. Selects mode E value 0. It clears the two other contiguous mode-E controls and one
   linked control at source+0x1E0, then stores zero in g_UiCommandModeE. Original labels are not preserved.
   Cross-module calls: UiSelectableGroup_SelectExclusive [ui/controls/lists].
*/
void InGameCommandModeE_Select0(UiSpriteButtonControl *source)

{
  UiSelectableGroup_SelectExclusive(4,(UiNodeBase *)source);
  g_UiCommandModeE = 0;
  return;
}

/* Address: 0x0056E220.
   Ownership: ui/ingame/commands.
   Purpose: Action 0x1040. Exclusively selects the source within the three contiguous mode-E sprite controls and
   stores value 1 in g_UiCommandModeE.
   Cross-module calls: UiSelectableGroup_SelectExclusive [ui/controls/lists].
*/
void InGameCommandModeE_Select1(UiSpriteButtonControl *source)

{
  UiSelectableGroup_SelectExclusive(3,(UiNodeBase *)source);
  g_UiCommandModeE = 1;
  return;
}

/* Address: 0x0056E260.
   Ownership: ui/ingame/commands.
   Purpose: Action 0x1041. Exclusively selects the source within the three contiguous mode-E sprite controls and
   stores value 2 in g_UiCommandModeE.
   Cross-module calls: UiSelectableGroup_SelectExclusive [ui/controls/lists].
*/
void InGameCommandModeE_Select2(UiSpriteButtonControl *source)

{
  UiSelectableGroup_SelectExclusive(3,(UiNodeBase *)source);
  g_UiCommandModeE = 2;
  return;
}

/* Address: 0x0056E2A0.
   Ownership: ui/ingame/commands.
   Purpose: Action 0x1042. Dispatches fixed range length 0x80 with binary state value 0. When shared runtime mode
   bits 0 or 1 are set, it queues numeric opcode 0x3200; otherwise it applies the local range operation to the
   active context. The source argument is unused. Original labels remain unresolved.
   Cross-module calls: TerrainGrid_RunDirectionalRelaxationPasses [world/terrain/grid],
   InGameCommandQueue_AppendLocalPlayerCommand [network/protocol/commands].
*/
void InGameCommandRange_DispatchState0(UiNodeBase *source)

{
  undefined4 in_ECX;
  undefined4 in_EDX;
  
  if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_NETWORKED_MASK) ==
      SESSION_NETWORK_ROLE_LOCAL) {
    TerrainGrid_RunDirectionalRelaxationPasses
              (in_ECX,in_EDX,g_LocalPlayerRuntimeId,0,0x80,TERRAIN_RELAXATION_SIGN_GATED);
  }
  else {
    InGameCommandQueue_AppendLocalPlayerCommand(0x3200,0,0x80,0);
  }
  return;
}

/* Address: 0x0056E2E0.
   Ownership: ui/ingame/commands.
   Purpose: Action 0x1043. Dispatches fixed range length 0x80 with binary state value 1. When shared runtime mode
   bits 0 or 1 are set, it queues numeric opcode 0x3200; otherwise it applies the local range operation to the
   active context. The source argument is unused. Original labels remain unresolved.
   Cross-module calls: TerrainGrid_RunDirectionalRelaxationPasses [world/terrain/grid],
   InGameCommandQueue_AppendLocalPlayerCommand [network/protocol/commands].
*/
void InGameCommandRange_DispatchState1(UiNodeBase *source)

{
  undefined4 in_ECX;
  undefined4 in_EDX;
  
  if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_NETWORKED_MASK) ==
      SESSION_NETWORK_ROLE_LOCAL) {
    TerrainGrid_RunDirectionalRelaxationPasses
              (in_ECX,in_EDX,g_LocalPlayerRuntimeId,0,0x80,TERRAIN_RELAXATION_UNGATED_LAND_TOOL);
  }
  else {
    InGameCommandQueue_AppendLocalPlayerCommand(0x3200,0,0x80,1);
  }
  return;
}

/* Address: 0x0056E320.
   Ownership: ui/ingame/commands.
   Purpose: Action 0x1044. Selects the first of two mode-F sprite controls, stores zero in g_UiCommandModeF, and
   mirrors zero into the shared owner-relative state field.
   Cross-module calls: UiSelectableGroup_SelectExclusive [ui/controls/lists].
*/
void InGameCommandModeF_Select0(UiSpriteButtonControl *source)

{
  UiSelectableGroup_SelectExclusive(2,(UiNodeBase *)source);
  g_UiCommandModeF = 0;
  source[-0x181].normalSubresourceEndExclusive = 0;
  return;
}

/* Address: 0x0056E370.
   Ownership: ui/ingame/commands.
   Purpose: Action 0x1045. Selects the second of two mode-F sprite controls, stores one in g_UiCommandModeF, and
   mirrors one into the same shared owner-relative state field.
   Cross-module calls: UiSelectableGroup_SelectExclusive [ui/controls/lists].
*/
void InGameCommandModeF_Select1(UiSpriteButtonControl *source)

{
  UiSelectableGroup_SelectExclusive(2,(UiNodeBase *)source);
  g_UiCommandModeF = 1;
  source[-0x182].normalSubresourceEndExclusive = 1;
  return;
}

/* Address: 0x00570F20.
   Ownership: ui/ingame/commands.
   Purpose: One-argument no-op callback installed into the UI command runtime record at offset 0xB0 during
   initialization. The exact callback-slot label remains unresolved.
*/
void UiCommandRuntime_CallbackNoOp(void)

{
  return;
}

/* Address: 0x0055F280.
   Ownership: ui/ingame/commands.
   Purpose: Handles local operation 0x150. Depending on flags, marks runtime state, transfers matching object
   ownership, removes a frontend player, shuts down a departed local network session, or posts localized departure
   message 0xFF08. Typed parameters: p3 flags→GameEntityCommandFlags. Nearby but non-identical semantic domains
   were explicitly deferred. Calling convention, parameter storage, body bytes, control flow, globals, locals, and
   executable data remain unchanged.
   Cross-module calls: Resource_Release [assets/resource/runtime], TextResource_Resolve [assets/text/resources],
   RichTextCommandStream_PatchPayloadBySelector [assets/text/richtext], InGameRecentTextHistory_InsertAndRebuild8
   [ui/ingame/runtime], ArmyRuntime_DestroyInstanceAndRefreshUi [gameplay/army/runtime].
*/
void InGameCommand150_HandlePlayerDepartureAndOwnership
               (PlayerOrFactionRuntimeId32 playerOrFactionId,dword value1,dword value2,
               GameEntityCommandFlags flags)

{
  GameEntityRuntime *entityRuntime;
  InGameRuntimeRootImageC3E4 *pIVar1;
  WorldRuntimeContext *extraout_EAX;
  WorldRuntimeContext *worldRuntime;
  word *stream;
  dword extraout_ECX;
  dword dVar2;
  FrontendPlayerRuntimeBlockCount FVar3;
  void *replacementPayload;
  FrontendPlayerRuntimeRecord *pFVar4;
  WorldRuntimeNode *worldNode1;
  
  pIVar1 = g_InGameRuntimeRoot;
  if ((flags & 2) == 0) {
    worldRuntime = &g_InGameRuntimeRoot->worldRuntime0A30;
    FVar3 = g_FrontendPlayerRuntimeBlockCount;
    pFVar4 = g_FrontendPlayerRuntimeBlocks;
    if ((flags & 1) == 0) {
      do {
        if (playerOrFactionId == pFVar4->playerRuntimeId) {
          pFVar4->heartbeatExpiryTicks = 0;
          if (pFVar4 == g_FrontendPlayerRuntimeBlocks) {
            g_SessionTransferTimeoutTicks = 0;
          }
          if (playerOrFactionId == (pIVar1->worldRuntime0A30).selection.activePlayerRuntimeId) {
            g_UiCommandRuntimeFlags = g_UiCommandRuntimeFlags | 0x20000;
            Resource_Release(g_FrontendLoadedCampaignAsset);
            g_FrontendLoadedCampaignAsset = (void *)0x0;
            if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_NETWORKED_MASK) ==
                SESSION_NETWORK_ROLE_LOCAL) {
              g_FrontendLoadedCampaignAsset = (void *)0x0;
              return;
            }
            g_SessionNetworkRoleFlags =
                 g_SessionNetworkRoleFlags & ~SESSION_NETWORK_ROLE_NETWORKED_MASK;
            g_SessionTransferTimeoutTicks = 0;
            (*g_NetworkBackendSlot3)();
            (*g_NetworkBackendSlot1)();
            pFVar4 = g_FrontendPlayerRuntimeBlocks;
            g_FrontendPlayerRuntimeBlockCount = 1;
            g_LocalPlayerRuntimeId = 0;
            (pFVar4->playerName).textUtf16[0] = 0;
            (pFVar4->playerName).textUtf16[1] = 0;
            pFVar4->playerRuntimeId = 0;
            (pFVar4->factionAssignment).roleStateFlags = 0;
            return;
          }
          stream = TextResource_Resolve(0xff08);
          RichTextCommandStream_PatchPayloadBySelector(0,replacementPayload,stream);
          InGameRecentTextHistory_InsertAndRebuild8();
          return;
        }
        FVar3 = FVar3 - 1;
        pFVar4 = pFVar4 + 1;
      } while (FVar3 != 0);
    }
    else {
      dVar2 = g_SelectionPlayerRuntimeBlockPointers[playerOrFactionId]->
              primaryEntityOrFactionToken8080;
      for (worldNode1 = (g_InGameRuntimeRoot->worldRuntime0A30).ownerListHead;
          worldNode1 != (WorldRuntimeNode *)0x0; worldNode1 = (worldNode1->common).nextNode) {
        if ((worldNode1[2].common.nextNode == (WorldRuntimeNode *)0x0) &&
           (entityRuntime = *(GameEntityRuntime **)((int)worldNode1->runtimePayload + 8),
           dVar2 == (entityRuntime->common).ownership.ownerIndex)) {
          ArmyRuntime_DestroyInstanceAndRefreshUi(worldRuntime,entityRuntime);
          worldRuntime = extraout_EAX;
          dVar2 = extraout_ECX;
        }
      }
    }
  }
  else {
    g_UiCommandRuntimeFlags = g_UiCommandRuntimeFlags | 0x10000;
  }
  return;
}

/* Address: 0x0056D980.
   Ownership: ui/ingame/commands.
   Purpose: Applies the command-mode color variant using 24-bit-masked fields at +0x120/+0x124, forces the high
   byte of the +0x12C field, updates the object through helpers 00505780 and 00505700, sets global bit 0x1000, and
   stores limit 0x7FFFFFFF.
   Cross-module calls: TerrainLighting_BuildColorRampAndSetBaseColor [world/terrain/visuals],
   FieldGrid_RecomputeInteriorDirectionalLighting [world/terrain/grid].
*/
void UiCommandModeG_ApplyMaskedColorVariant(void *visualState)

{
  TerrainLighting_BuildColorRampAndSetBaseColor
            (*(uint *)((int)visualState + 300) | 0xff000000,
             *(uint *)((int)visualState + 0x124) & 0xffffff,
             *(uint *)((int)visualState + 0x120) & 0xffffff);
  FieldGrid_RecomputeInteriorDirectionalLighting
            (*(AngleTurn32 *)((int)visualState + 0x17c),*(AngleTurn32 *)((int)visualState + 0x178),
             *(FieldGridAsset **)((int)visualState + 0x54));
  g_UiCommandModeGColorVariantFlags = g_UiCommandModeGColorVariantFlags | 0x1000;
  g_UiCommandModeGColorVariantLimit = 0x7fffffff;
  return;
}

/* Address: 0x0056DA50.
   Ownership: ui/ingame/commands.
   Purpose: Sets UiNodeBase.nodeFlags bit 0x02000000. The original visual-state label is not preserved.
*/
void UiCommandModeG_SetNodeFlag02000000(UiNodeBase *node)

{
  node[1].nextSibling = (UiNodeBase *)((uint)node[1].nextSibling | 0x2000000);
  return;
}

/* Address: 0x00571440.
   Ownership: ui/ingame/commands.
   Purpose: Selects one absolute command-matrix index, updates the active twelve-entry page, refreshes the twelve
   root-resident display values, and exclusively selects the corresponding mapped control. Original catalog labels
   are not preserved. Typed parameters: p0 absoluteIndex→UiCommandModeIndex_V342. Calling convention, exact
   VariableStorage serialization, function body bytes, control flow, globals, locals, and executable data remain
   unchanged.
   Cross-module calls: UiSelectableGroup_SelectExclusive [ui/controls/lists].
*/
void UiCommandMatrix_SelectIndex(UiCommandModeIndex absoluteIndex,UiNodeBase *root)

{
  GraphicsTextureSourceAsset *pGVar1;
  GraphicsTextureSourceAsset *pGVar2;
  int iVar3;
  uint uVar4;
  dword dVar5;
  
  g_UiCommandAbsoluteSelectionIndex = absoluteIndex;
  root[0x20d].topAnchorQ31 =
       (UiAnchorFractionQ31)g_TerrainMaterialTextureSets[absoluteIndex]->entries[0].sourceAsset;
  uVar4 = g_UiCommandSelectionPageBaseIndex + 0xc;
  dVar5 = g_UiCommandSelectionPageBaseIndex;
  while( true ) {
    for (; absoluteIndex < dVar5; dVar5 = dVar5 - 3) {
      uVar4 = uVar4 - 3;
    }
    if (absoluteIndex < uVar4) break;
    dVar5 = dVar5 + 3;
    uVar4 = uVar4 + 3;
  }
  pGVar1 = (GraphicsTextureSourceAsset *)0x0;
  if (g_TerrainMaterialTextureSets[dVar5] != (GraphicsTextureSet *)0x0) {
    pGVar1 = g_TerrainMaterialTextureSets[dVar5]->entries[0].sourceAsset;
  }
  pGVar2 = (GraphicsTextureSourceAsset *)0x0;
  if (g_TerrainMaterialTextureSets[dVar5 + 1] != (GraphicsTextureSet *)0x0) {
    pGVar2 = g_TerrainMaterialTextureSets[dVar5 + 1]->entries[0].sourceAsset;
  }
  g_UiCommandSelectionPageBaseIndex = dVar5;
  root[0x234].top = (sdword)pGVar1;
  root[0x236].topAnchorQ31 = (UiAnchorFractionQ31)pGVar2;
  pGVar1 = (GraphicsTextureSourceAsset *)0x0;
  if (g_TerrainMaterialTextureSets[dVar5 + 2] != (GraphicsTextureSet *)0x0) {
    pGVar1 = g_TerrainMaterialTextureSets[dVar5 + 2]->entries[0].sourceAsset;
  }
  pGVar2 = (GraphicsTextureSourceAsset *)0x0;
  if (g_TerrainMaterialTextureSets[dVar5 + 3] != (GraphicsTextureSet *)0x0) {
    pGVar2 = g_TerrainMaterialTextureSets[dVar5 + 3]->entries[0].sourceAsset;
  }
  root[0x239].parent = (UiNodeBase *)pGVar1;
  root[0x23b].rightOffset = (sdword)pGVar2;
  pGVar1 = (GraphicsTextureSourceAsset *)0x0;
  if (g_TerrainMaterialTextureSets[dVar5 + 4] != (GraphicsTextureSet *)0x0) {
    pGVar1 = g_TerrainMaterialTextureSets[dVar5 + 4]->entries[0].sourceAsset;
  }
  pGVar2 = (GraphicsTextureSourceAsset *)0x0;
  if (g_TerrainMaterialTextureSets[dVar5 + 5] != (GraphicsTextureSet *)0x0) {
    pGVar2 = g_TerrainMaterialTextureSets[dVar5 + 5]->entries[0].sourceAsset;
  }
  root[0x23d].nodeFlags = (UiNodeFlags)pGVar1;
  root[0x240].bottom = (sdword)pGVar2;
  pGVar1 = (GraphicsTextureSourceAsset *)0x0;
  if (g_TerrainMaterialTextureSets[dVar5 + 6] != (GraphicsTextureSet *)0x0) {
    pGVar1 = g_TerrainMaterialTextureSets[dVar5 + 6]->entries[0].sourceAsset;
  }
  pGVar2 = (GraphicsTextureSourceAsset *)0x0;
  if (g_TerrainMaterialTextureSets[dVar5 + 7] != (GraphicsTextureSet *)0x0) {
    pGVar2 = g_TerrainMaterialTextureSets[dVar5 + 7]->entries[0].sourceAsset;
  }
  root[0x242].bottomAnchorQ31 = (UiAnchorFractionQ31)pGVar1;
  root[0x245].left = (sdword)pGVar2;
  pGVar1 = (GraphicsTextureSourceAsset *)0x0;
  if (g_TerrainMaterialTextureSets[dVar5 + 8] != (GraphicsTextureSet *)0x0) {
    pGVar1 = g_TerrainMaterialTextureSets[dVar5 + 8]->entries[0].sourceAsset;
  }
  pGVar2 = (GraphicsTextureSourceAsset *)0x0;
  if (g_TerrainMaterialTextureSets[dVar5 + 9] != (GraphicsTextureSet *)0x0) {
    pGVar2 = g_TerrainMaterialTextureSets[dVar5 + 9]->entries[0].sourceAsset;
  }
  root[0x247].leftAnchorQ31 = (UiAnchorFractionQ31)pGVar1;
  root[0x24a].firstChild = (UiNodeBase *)pGVar2;
  pGVar1 = (GraphicsTextureSourceAsset *)0x0;
  if (g_TerrainMaterialTextureSets[dVar5 + 10] != (GraphicsTextureSet *)0x0) {
    pGVar1 = g_TerrainMaterialTextureSets[dVar5 + 10]->entries[0].sourceAsset;
  }
  pGVar2 = (GraphicsTextureSourceAsset *)0x0;
  if (g_TerrainMaterialTextureSets[dVar5 + 0xb] != (GraphicsTextureSet *)0x0) {
    pGVar2 = g_TerrainMaterialTextureSets[dVar5 + 0xb]->entries[0].sourceAsset;
  }
  root[0x24c].topOffset = (sdword)pGVar1;
  root[0x24e].layoutHeight = (sdword)pGVar2;
  iVar3 = 0xb;
  do {
    iVar3 = iVar3 + -1;
  } while (-1 < iVar3);
  UiSelectableGroup_SelectExclusive
            (0xc,(UiNodeBase *)
                 ((int)&root->nextSibling + g_UiMappedCommandControlOffsets[absoluteIndex - dVar5]))
  ;
  return;
}

/* Address: 0x0055F440.
   Ownership: ui/ingame/commands.
   Purpose: Applies three masks to g_UiCommandRuntimeFlags in order: clear, set, then toggle. Typed parameters: p3
   toggleMask→UiCommandRuntimeFlagMask_V342, p4 setMask→UiCommandRuntimeFlagMask_V342, p5
   clearMask→UiCommandRuntimeFlagMask_V342. Calling convention, exact VariableStorage serialization, function body
   bytes, control flow, globals, locals, and executable data remain unchanged.
*/
void UiCommandRuntimeFlags_ApplyClearSetToggleMasks
               (undefined4 param_1,UiCommandRuntimeFlagMask toggleMask,
               UiCommandRuntimeFlagMask setMask,UiCommandRuntimeFlagMask clearMask)

{
  g_UiCommandRuntimeFlags = g_UiCommandRuntimeFlags & ~clearMask;
  g_UiCommandRuntimeFlags = g_UiCommandRuntimeFlags | setMask;
  g_UiCommandRuntimeFlags = g_UiCommandRuntimeFlags ^ toggleMask;
  return;
}

/* Address: 0x0056D860.
   Ownership: ui/ingame/commands.
   Purpose: Clears UiNodeBase.nodeFlags bit 0x00100000. The original visual-state label is not preserved.
*/
void UiCommandModeG_ClearNodeFlag00100000(UiNodeBase *node)

{
  node[1].nextSibling = (UiNodeBase *)((uint)node[1].nextSibling & 0xffefffff);
  return;
}

/* Address: 0x0056D880.
   Ownership: ui/ingame/commands.
   Purpose: Sets UiNodeBase.nodeFlags bit 0x00200000. The original visual-state label is not preserved.
*/
void UiCommandModeG_SetNodeFlag00200000(UiNodeBase *node)

{
  node[1].nextSibling = (UiNodeBase *)((uint)node[1].nextSibling | 0x200000);
  return;
}

/* Address: 0x0056D940.
   Ownership: ui/ingame/commands.
   Purpose: Sets UiNodeBase.nodeFlags bit 0x01000000. The original visual-state label is not preserved.
*/
void UiCommandModeG_SetNodeFlag01000000(UiNodeBase *node)

{
  node[1].nextSibling = (UiNodeBase *)((uint)node[1].nextSibling | 0x1000000);
  return;
}

/* Address: 0x0056D8C0.
   Ownership: ui/ingame/commands.
   Purpose: Sets UiNodeBase.nodeFlags bit 0x00000400. The original visual-state label is not preserved.
*/
void UiCommandModeG_SetNodeFlag00000400(UiNodeBase *node)

{
  node[1].nextSibling = (UiNodeBase *)((uint)node[1].nextSibling | 0x400);
  return;
}

/* Address: 0x0056D8E0.
   Ownership: ui/ingame/commands.
   Purpose: Clears UiNodeBase.nodeFlags bits 0x00000400 and 0x00000080, exactly matching mask 0xFFFFFB7F. The
   combined original meaning is unresolved.
*/
void UiCommandModeG_ClearNodeFlags00000480(UiNodeBase *node)

{
  node[1].nextSibling = (UiNodeBase *)((uint)node[1].nextSibling & 0xfffffb7f);
  return;
}

/* Address: 0x0056D840.
   Ownership: ui/ingame/commands.
   Purpose: Sets UiNodeBase.nodeFlags bit 0x00100000. The original visual-state label is not preserved.
*/
void UiCommandModeG_SetNodeFlag00100000(UiNodeBase *node)

{
  node[1].nextSibling = (UiNodeBase *)((uint)node[1].nextSibling | 0x100000);
  return;
}

/* Address: 0x0056D8A0.
   Ownership: ui/ingame/commands.
   Purpose: Clears UiNodeBase.nodeFlags bit 0x00200000. The original visual-state label is not preserved.
*/
void UiCommandModeG_ClearNodeFlag00200000(UiNodeBase *node)

{
  node[1].nextSibling = (UiNodeBase *)((uint)node[1].nextSibling & 0xffdfffff);
  return;
}

/* Address: 0x0056D960.
   Ownership: ui/ingame/commands.
   Purpose: Clears UiNodeBase.nodeFlags bit 0x01000000. The original visual-state label is not preserved.
*/
void UiCommandModeG_ClearNodeFlag01000000(UiNodeBase *node)

{
  node[1].nextSibling = (UiNodeBase *)((uint)node[1].nextSibling & 0xfeffffff);
  return;
}

/* Address: 0x0056D9F0.
   Ownership: ui/ingame/commands.
   Purpose: Applies the command-mode color variant using the unmasked fields at +0x120/+0x124/+0x12C, updates the
   object through helpers 00505780 and 00505700, clears global bit 0x1000, and stores limit 0x00FFFFFF.
   Cross-module calls: TerrainLighting_BuildColorRampAndSetBaseColor [world/terrain/visuals],
   FieldGrid_RecomputeInteriorDirectionalLighting [world/terrain/grid].
*/
void UiCommandModeG_ApplyRawColorVariant(void *visualState)

{
  TerrainLighting_BuildColorRampAndSetBaseColor
            (*(PackedArgb32 *)((int)visualState + 300),*(PackedArgb32 *)((int)visualState + 0x124),
             *(PackedArgb32 *)((int)visualState + 0x120));
  FieldGrid_RecomputeInteriorDirectionalLighting
            (*(AngleTurn32 *)((int)visualState + 0x17c),*(AngleTurn32 *)((int)visualState + 0x178),
             *(FieldGridAsset **)((int)visualState + 0x54));
  g_UiCommandModeGColorVariantFlags = g_UiCommandModeGColorVariantFlags & 0xffffefff;
  g_UiCommandModeGColorVariantLimit = 0xffffff;
  return;
}

/* Address: 0x0056DA70.
   Ownership: ui/ingame/commands.
   Purpose: Clears UiNodeBase.nodeFlags bit 0x02000000. The original visual-state label is not preserved.
*/
void UiCommandModeG_ClearNodeFlag02000000(UiNodeBase *node)

{
  node[1].nextSibling = (UiNodeBase *)((uint)node[1].nextSibling & 0xfdffffff);
  return;
}

/* Address: 0x0056D900.
   Ownership: ui/ingame/commands.
   Purpose: Sets UiNodeBase.nodeFlags bit 0x00800000. The original visual-state label is not preserved.
*/
void UiCommandModeG_SetNodeFlag00800000(UiNodeBase *node)

{
  node[1].nextSibling = (UiNodeBase *)((uint)node[1].nextSibling | 0x800000);
  return;
}

/* Address: 0x0056DA90.
   Ownership: ui/ingame/commands.
   Purpose: Finds the UI root, tests and exclusively selects one of six controls, synchronizes three root-resident
   page stacks using the selected mode's three page-index tables, and stores the active mode in g_UiCommandModeG.
   The helper preserves the exclusive-selection EAX value, but no caller in this executable consumes it. Typed
   parameters: p0 modeIndex→UiCommandModeIndex_V342. Calling convention, exact VariableStorage serialization,
   function body bytes, control flow, globals, locals, and executable data remain unchanged.
   Cross-module calls: UiSelectableGroup_NoneVisibleSelectedCf [ui/controls/lists],
   UiSelectableGroup_SelectExclusive [ui/controls/lists], UiPageStack_SetActiveIndex [ui/controls/layout].
*/
void UiCommandModeG_SelectAndSyncPages(UiCommandModeIndex modeIndex,UiSelectableControl *source)

{
  UiNodeBase *pUVar1;
  UiSelectableControl *pUVar2;
  int extraout_ECX;
  int extraout_ECX_00;
  dword extraout_ECX_01;
  int extraout_EDX;
  int extraout_EDX_00;
  int extraout_EDX_01;
  
  pUVar1 = (source->base).parent;
  pUVar2 = source;
  while (pUVar1 != (UiNodeBase *)0xffffffff) {
    pUVar2 = (UiSelectableControl *)(pUVar2->base).parent;
    pUVar1 = (pUVar2->base).parent;
  }
  UiSelectableGroup_NoneVisibleSelectedCf(6);
  UiSelectableGroup_SelectExclusive(6,&source->base);
  UiPageStack_SetActiveIndex
            (g_UiCommandModeGPrimaryPageIndices[modeIndex],
             (UiPageStackControl *)(extraout_EDX + 0x9a8c));
  UiPageStack_SetActiveIndex
            (g_UiCommandModeGSecondaryPageIndices[extraout_ECX],
             (UiPageStackControl *)(extraout_EDX_00 + 0x9ee0));
  UiPageStack_SetActiveIndex
            (g_UiCommandModeGTertiaryPageIndices[extraout_ECX_00],
             (UiPageStackControl *)(extraout_EDX_01 + 0xb19c));
  g_UiCommandModeG = extraout_ECX_01;
  return;
}

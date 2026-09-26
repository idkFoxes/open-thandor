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
   Ownership: ui/ingame/commands.
   Purpose: Action 0x1028. Selects six-choice command mode G value 0, synchronizes the three related page stacks,
   and applies the verified auxiliary visual-state combination for mode 0. Original user-facing label is not
   preserved. Queued UI action handler for INGAME_COMMAND_MODE_PAGE11[0] (0x1100). Return datatype is preserved for
   non-queue direct callers.
   Local calls: UiCommandModeG_SelectAndSyncPages, UiCommandModeG_SetNodeFlag00100000,
   UiCommandModeG_SetNodeFlag00200000, UiCommandModeG_ClearNodeFlags00000480, UiCommandModeG_SetNodeFlag00800000,
   UiCommandModeG_SetNodeFlag01000000, UiCommandModeG_ApplyRawColorVariant, UiCommandModeG_ClearNodeFlag02000000.
*/
void __thandor_void_preserve_eax_ecx_edx InGameCommandModeG_Select0(UiSelectableControl *source)

{
  InGameRuntimeRootImageC3E4 *runtimeRoot;
  WorldRuntimeContext *node;
  
  runtimeRoot = UiCommandModeG_SelectAndSyncPages(0,source);
  node = &runtimeRoot->worldRuntime0A30;
  UiCommandModeG_SetNodeFlag00100000((UiNodeBase *)node);
  UiCommandModeG_SetNodeFlag00200000((UiNodeBase *)node);
  UiCommandModeG_ClearNodeFlags00000480((UiNodeBase *)node);
  UiCommandModeG_SetNodeFlag00800000((UiNodeBase *)node);
  UiCommandModeG_SetNodeFlag01000000((UiNodeBase *)node);
  UiCommandModeG_ApplyRawColorVariant(node);
  UiCommandModeG_ClearNodeFlag02000000((UiNodeBase *)node);
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
void __thandor_void_preserve_eax_ecx_edx InGameCommandModeG_Select1(UiSelectableControl *source)

{
  InGameRuntimeRootImageC3E4 *runtimeRoot;
  WorldRuntimeContext *node;
  
  runtimeRoot = UiCommandModeG_SelectAndSyncPages(1,source);
  node = &runtimeRoot->worldRuntime0A30;
  UiCommandModeG_SetNodeFlag00100000((UiNodeBase *)node);
  UiCommandModeG_SetNodeFlag00200000((UiNodeBase *)node);
  UiCommandModeG_ClearNodeFlags00000480((UiNodeBase *)node);
  UiCommandModeG_SetNodeFlag00800000((UiNodeBase *)node);
  UiCommandModeG_ClearNodeFlag01000000((UiNodeBase *)node);
  UiCommandModeG_ApplyRawColorVariant(node);
  UiCommandModeG_ClearNodeFlag02000000((UiNodeBase *)node);
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
void __thandor_void_preserve_eax_ecx_edx InGameCommandModeG_Select2(UiSelectableControl *source)

{
  InGameRuntimeRootImageC3E4 *runtimeRoot;
  WorldRuntimeContext *node;
  
  runtimeRoot = UiCommandModeG_SelectAndSyncPages(2,source);
  node = &runtimeRoot->worldRuntime0A30;
  UiCommandModeG_SetNodeFlag00100000((UiNodeBase *)node);
  UiCommandModeG_ClearNodeFlag00200000((UiNodeBase *)node);
  UiCommandModeG_ClearNodeFlags00000480((UiNodeBase *)node);
  UiCommandModeG_SetNodeFlag00800000((UiNodeBase *)node);
  UiCommandModeG_SetNodeFlag01000000((UiNodeBase *)node);
  UiCommandModeG_ApplyMaskedColorVariant(node);
  UiCommandModeG_ClearNodeFlag02000000((UiNodeBase *)node);
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
void __thandor_void_preserve_eax_ecx_edx InGameCommandModeG_Select3(UiSelectableControl *source)

{
  InGameRuntimeRootImageC3E4 *runtimeRoot;
  WorldRuntimeContext *node;
  ArmyRegistryEaxCf5_51b6d0 armyAssetLookup;
  FatalErrorEaxCf5 checkedAssetLookup;
  
  runtimeRoot = UiCommandModeG_SelectAndSyncPages(3,source);
  node = &runtimeRoot->worldRuntime0A30;
  UiCommandModeG_ClearNodeFlag00100000((UiNodeBase *)node);
  UiCommandModeG_ClearNodeFlag00200000((UiNodeBase *)node);
  UiCommandModeG_SetNodeFlag00000400((UiNodeBase *)node);
  UiCommandModeG_SetNodeFlag00800000((UiNodeBase *)node);
  UiCommandModeG_ClearNodeFlag01000000((UiNodeBase *)node);
  UiCommandModeG_ApplyRawColorVariant(node);
  UiCommandModeG_ClearNodeFlag02000000((UiNodeBase *)node);
  armyAssetLookup = ArmyAssetRegistry_FindByIdCf(g_UiCommandModeGArmyAssetId);
  checkedAssetLookup = (*g_FatalErrorPrimaryDispatchCf)((dword)armyAssetLookup.eax,armyAssetLookup.carry);
  g_UiHoverSelectionRecord = (UiCommandRuntimeRecordPrefix *)checkedAssetLookup.eax;
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
void __thandor_void_preserve_eax_ecx_edx InGameCommandModeG_Select4(UiSelectableControl *source)

{
  InGameRuntimeRootImageC3E4 *runtimeRoot;
  WorldRuntimeContext *node;
  
  runtimeRoot = UiCommandModeG_SelectAndSyncPages(4,source);
  node = &runtimeRoot->worldRuntime0A30;
  UiCommandModeG_ClearNodeFlag00100000((UiNodeBase *)node);
  UiCommandModeG_ClearNodeFlag00200000((UiNodeBase *)node);
  UiCommandModeG_SetNodeFlag00000400((UiNodeBase *)node);
  UiCommandModeG_SetNodeFlag00800000((UiNodeBase *)node);
  UiCommandModeG_ClearNodeFlag01000000((UiNodeBase *)node);
  UiCommandModeG_ApplyRawColorVariant(node);
  UiCommandModeG_ClearNodeFlag02000000((UiNodeBase *)node);
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
void __thandor_void_preserve_eax_ecx_edx InGameCommandModeG_Select5(UiSelectableControl *source)

{
  InGameRuntimeRootImageC3E4 *runtimeRoot;
  WorldRuntimeContext *node;
  
  runtimeRoot = UiCommandModeG_SelectAndSyncPages(5,source);
  node = &runtimeRoot->worldRuntime0A30;
  UiCommandModeG_SetNodeFlag00100000((UiNodeBase *)node);
  UiCommandModeG_ClearNodeFlag00200000((UiNodeBase *)node);
  UiCommandModeG_SetNodeFlag00000400((UiNodeBase *)node);
  UiCommandModeG_SetNodeFlag00800000((UiNodeBase *)node);
  UiCommandModeG_ClearNodeFlag01000000((UiNodeBase *)node);
  UiCommandModeG_ApplyRawColorVariant(node);
  UiCommandModeG_SetNodeFlag02000000((UiNodeBase *)node);
  (runtimeRoot->worldRuntime0A30).fieldRegion.reservedCallbackState04 = g_UiCommandModeF;
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
void __thandor_preserve_eax InGameCommandAction_SetFlag1000OrMarkReady(void *source)

{
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
    FrontendPlayerRuntime_MarkReadyByIdAndUpdateAction101B(g_LocalPlayerRuntimeId);
  }
  else {
    InGameCommandQueue_AppendLocalPlayerCommand(0x470,0,0,0);
  }
  return;
}


/* Address: 0x0056ACC0.
   Ownership: ui/ingame/commands.
   Purpose: Binary entry is anchored by g_UiActionPage10InitializedHandlers[9]@005624A0. Queued UI action handler
   for INGAME_PAGE10[9] (0x1009). Return datatype is preserved for non-queue direct callers.
   Local calls: UiCommandRuntimeFlags_ApplyClearSetToggleMasks.
   Cross-module calls: InGameCommandQueue_AppendLocalPlayerCommand [network/protocol/commands].
*/
void __thandor_preserve_eax InGameCommandAction_ToggleRuntimeFlag0800(void *source)

{
  if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_NETWORKED_MASK) ==
      SESSION_NETWORK_ROLE_LOCAL) {
    UiCommandRuntimeFlags_ApplyClearSetToggleMasks(g_LocalPlayerRuntimeId,0,0,0x800);
  }
  else {
    InGameCommandQueue_AppendLocalPlayerCommand(0x310,0,0,0x800);
  }
  return;
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
void __thandor_preserve_eax
InGameCommandState_CloseSettingsAndDispatchOperation150(UiNodeBase *source)

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
void __thandor_void_preserve_eax_ecx_edx InGameCommandMatrix_SelectMappedControl(UiNodeBase *source)

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
   Ownership: ui/ingame/commands.
   Purpose: Completes a non-right pointer activation, clears selected state, merges the filtered global input-state
   word into activationInputState, optionally plays the inherited sound, queues actionId, and invalidates the root.
   Cross-module calls: UiActionQueue_Enqueue [ui/core/runtime], UiNode_InvalidateRoot [ui/core/runtime].
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
GraphicsCursorFrameIndex __thandor_eax_preserve_ecx_edx
UiCommandSpriteVariantA_PointerMove
          (UiPixelCoordinate pointerY,UiPixelCoordinate pointerX,
          UiCommandSpriteButtonControl *control)

{
  GraphicsCursorFrameIndex cursorFrame;
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
  cursorFrame = 10;
  if ((g_KeyboardStateMask & 0xc) != 0) {
    cursorFrame = 0xc;
  }
  return cursorFrame;
}


/* Address: 0x00517F60.
   Ownership: ui/ingame/commands.
   Purpose: Binary entry is anchored by g_UiNodeVtable_00517F10[2]@00517F10.
   Cross-module calls: UiWrappedTextControl_DrawClipped [ui/controls/text].
*/
void __thandor_void_preserve_eax_ecx_edx
UiCommandVisibilityWrappedText_DrawWhenAllowed
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
void __thandor_void_preserve_eax_ecx_edx
UiCommandVisibilitySingleLineText_DrawWhenAllowed
          (UiPixelCoordinate clipTop,UiPixelCoordinate clipLeft,UiPixelCoordinate clipBottom,
          UiPixelCoordinate clipRight,UiNodeBase *control)

{
  int drawOffsetAdjust;
  
  drawOffsetAdjust = 0;
  if ((((g_UiCommandRuntimeFlags & 0x200) == 0) &&
      ((((uint)control[1].nextSibling & 0x800) == 0 || ((g_UiCommandRuntimeFlags & 1) != 0)))) &&
     ((((uint)control[1].nextSibling & 0x1000) == 0 ||
      (drawOffsetAdjust = g_InGameSimulationStepTicks - 2, 1 < g_InGameSimulationStepTicks)))) {
    control[1].parent = (UiNodeBase *)((int)&(control[1].parent)->nextSibling + drawOffsetAdjust);
    UiSingleLineTextControl_DrawClipped(clipTop,clipLeft,clipBottom,clipRight,control);
    control[1].parent = (UiNodeBase *)((int)control[1].parent - drawOffsetAdjust);
  }
  return;
}


/* Address: 0x0055F4A0.
   Ownership: ui/ingame/commands.
   Purpose: Handles in game command mode toggle player flag bit0 and reconcile global.
*/
void __thandor_void_preserve_eax_ecx_edx
InGameCommandMode_TogglePlayerFlagBit0AndReconcileGlobal
          (PlayerRuntimeId playerRuntimeId,dword callbackArg1,dword callbackArg2,dword callbackArg3)

{
  FrontendPlayerRuntimeBlockCount remainingPlayers;
  FrontendPlayerRuntimeRecord *playerRecord;
  
  g_SelectionPlayerRuntimeBlockPointers[playerRuntimeId]->sessionFlags =
       g_SelectionPlayerRuntimeBlockPointers[playerRuntimeId]->sessionFlags ^ 1;
  remainingPlayers = g_FrontendPlayerRuntimeBlockCount;
  playerRecord = g_FrontendPlayerRuntimeBlocks;
  do {
    if ((g_UiCommandRuntimeFlags & 1) == 0) {
      if ((g_SelectionPlayerRuntimeBlockPointers[playerRecord->playerRuntimeId]->sessionFlags & 1) == 0) {
        return;
      }
    }
    else if ((g_SelectionPlayerRuntimeBlockPointers[playerRecord->playerRuntimeId]->sessionFlags & 1) != 0
            ) {
      return;
    }
    playerRecord = playerRecord + 1;
    remainingPlayers = remainingPlayers - 1;
  } while (remainingPlayers != 0);
  g_UiCommandRuntimeFlags = g_UiCommandRuntimeFlags ^ 1;
  return;
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
void __thandor_void_preserve_eax_ecx_edx
InGameCommand_ExecuteLocalPlacementFromSelection
          (PlayerRuntimeId playerId,CommandPayloadDword04 payloadDword04,
          CommandPayloadDword08 payloadDword08,CommandPayloadDword0C payloadDword0C)

{
  FactionRelationCounter *relationCounter;
  dword pendingEntryOrFactionToken;
  SelectionPlayerRuntimeBlock *playerBlock;
  ArmyRuntimeSlot *modelNodeRuntime;
  ArmyRuntimeSlot *armySlot;
  ModelRuntimeSlot *slotModelRuntime;
  InGameRuntimeRootImageC3E4 *runtimeRoot;
  ArmyRuntimeSlot **createdArmySlots;
  Q12 worldYQ12;
  SelectionPlayerRuntimeBlock *worldXQ12;
  WorldRuntimeContext *worldRuntime;
  bool placementRejected;
  ArmyRuntimeCreateEaxCf5 createResult;
  
  runtimeRoot = g_InGameRuntimeRoot;
  playerBlock = g_SelectionPlayerRuntimeBlockPointers[playerId];
  worldRuntime = &g_InGameRuntimeRoot->worldRuntime0A30;
  LOCK();
  pendingEntryOrFactionToken = playerBlock->pendingSelectionEntityOffset8098;
  playerBlock->pendingSelectionEntityOffset8098 = 0;
  UNLOCK();
  if (pendingEntryOrFactionToken != 0) {
    worldXQ12 = playerBlock;
    placementRejected = ArmyPlacement_ValidateAssetAtPointAndCellCornersCf
                      (0,payloadDword04,payloadDword08,payloadDword0C,
                       *(ArmyPlacementContext *)(pendingEntryOrFactionToken + 8),playerBlock->primaryEntityOrFactionToken8080,
                       worldRuntime);
    if (!placementRejected) {
      /* ECX/EDX of the validator: the accepted (possibly snapped) point. */
      createResult = ArmyRuntime_CreateInstanceFromAssetCf
                        (4,payloadDword04,g_ArmyPlacementValidatedWorldYQ12,
                         g_ArmyPlacementValidatedWorldXQ12,
                         playerBlock->primaryEntityOrFactionToken8080,
                         *(PckArmyAssetIdCatalog *)(pendingEntryOrFactionToken + 8),worldRuntime);
      createdArmySlots = (ArmyRuntimeSlot **)createResult.eax;
      if (!createResult.carry) {
        pendingEntryOrFactionToken = playerBlock->primaryEntityOrFactionToken8080;
        modelNodeRuntime = createdArmySlots[1];
        armySlot = *createdArmySlots;
        modelNodeRuntime->movementPosition0Q12 = 0;
        if (pendingEntryOrFactionToken == (runtimeRoot->worldRuntime0A30).activeFactionRuntimeIndex) {
          modelNodeRuntime->movementPosition0Q12 = 0x7fffffff;
        }
        relationCounter = &g_GameFactionRuntimeImage.records[pendingEntryOrFactionToken].relationCounterB;
        *relationCounter = *relationCounter + 1;
        slotModelRuntime = (armySlot->modelRuntimeOrSavedOffset).modelRuntime;
        ModelNodeRuntime_RebuildTransformsFromRoot((ModelRuntimeNode *)modelNodeRuntime);
        ArmyRuntime_DispatchClassCommand(createdArmySlots,worldRuntime);
        EffectRuntimePool_CreateInstanceFromDefinitionCf
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
        g_UiCommandRuntimeFlags = g_UiCommandRuntimeFlags & 0xffffffdf;
        g_InGamePendingPlacementArmyAsset = 0;
        return;
      }
    }
    g_SelectionPlayerRuntimeBlockPointers[playerId]->pendingSelectionEntityOffset8098 = pendingEntryOrFactionToken;
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
void __thandor_void_preserve_eax_ecx_edx UiCommandSpriteVariantA_RebuildGrid(UiNodeBase *node)

{
  uint *controlFlags;
  UiNodeBase *parentCursor;
  sdword *offsetTable;
  UiCommandRuntimeRecordPrefix *runtimeRecord;
  dword columnCount;
  GraphicsTextureSourceAsset *slotTexture;
  int countWidthOrOffset;
  uint itemCount;
  FactionArmyAssetCount remainingAssets;
  int panelHeight;
  uint slotIndex;
  UiCommandRuntimeRecordPrefix **recordCursor;
  dword *assetCursor;
  UiGridDimensionsEdxEax8 gridDimensions;
  
  parentCursor = node->parent;
  while (parentCursor != (UiNodeBase *)0xffffffff) {
    node = node->parent;
    parentCursor = node->parent;
  }
  recordCursor = g_UiCommandSpriteVariantARecords;
  for (countWidthOrOffset = 0x18; countWidthOrOffset != 0; countWidthOrOffset = countWidthOrOffset + -1) {
    *recordCursor = (UiCommandRuntimeRecordPrefix *)0x0;
    recordCursor = recordCursor + 1;
  }
  recordCursor = g_UiCommandSpriteVariantARecords;
  remainingAssets = g_GameFactionRuntimeImage.records[node[0x23].bottom].primaryArmyAssetCount;
  itemCount = 0;
  assetCursor = g_GameFactionRuntimeImage.records[node[0x23].bottom].primaryArmyAssetPointersOrIds;
  if ((remainingAssets != 0) && ((g_UiCommandRuntimeFlags & 0x100) == 0)) {
    do {
      if ((((UiCommandRuntimeRecordPrefix *)*assetCursor)->textureSource !=
           (GraphicsTextureSourceAsset *)0x0) && (itemCount < 0x18)) {
        *recordCursor = (UiCommandRuntimeRecordPrefix *)*assetCursor;
        itemCount = itemCount + 1;
        recordCursor = recordCursor + 1;
      }
      assetCursor = assetCursor + 1;
      remainingAssets = remainingAssets - 1;
    } while (remainingAssets != 0);
  }
  gridDimensions = UiGrid_ComputeDimensionsPacked(6,itemCount);
  columnCount = (dword)gridDimensions;
  if (4 < columnCount) {
    columnCount = 4;
  }
  countWidthOrOffset = columnCount * g_InGamePanelTextureSubresource34Width + g_InGamePanelTextureSubresource27Width +
          g_InGamePanelTextureSubresource28Width;
  panelHeight = (int)(gridDimensions >> 0x20) * g_InGamePanelTextureSubresource34Height +
          g_InGamePanelTextureSubresource26Height + g_InGamePanelTextureSubresource31Height;
  g_UiCommandSpriteVariantAColumnCount = columnCount;
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
  node[0x1da].leftOffset = node[0x1da].leftOffset - countWidthOrOffset;
  node[0x1da].topOffset = node[0x1da].topOffset - panelHeight;
  if (itemCount == 0) {
    node[0x1da].nodeFlags = node[0x1da].nodeFlags | UI_NODE_SUPPRESSED;
  }
  else {
    node[0x1da].nodeFlags = node[0x1da].nodeFlags & ~UI_NODE_SUPPRESSED;
  }
  offsetTable = g_UiCommandSpriteVariantAOffsetTables[columnCount];
  slotIndex = 0;
  recordCursor = g_UiCommandSpriteVariantARecords;
  do {
    countWidthOrOffset = offsetTable[slotIndex];
    runtimeRecord = *recordCursor;
    if (slotIndex < itemCount) {
      controlFlags = (uint *)((int)&node->nodeFlags + countWidthOrOffset);
      *controlFlags = *controlFlags & 0xfffffff7;
      slotTexture = runtimeRecord->textureSource;
    }
    else {
      controlFlags = (uint *)((int)&node->nodeFlags + countWidthOrOffset);
      *controlFlags = *controlFlags | 8;
      slotTexture = (GraphicsTextureSourceAsset *)0x0;
    }
    slotIndex = slotIndex + 1;
    *(GraphicsTextureSourceAsset **)((int)&node[1].parent + countWidthOrOffset) = slotTexture;
    recordCursor = recordCursor + 1;
  } while (slotIndex < 0x18);
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
void __thandor_void_preserve_eax_ecx_edx
InGameCommandAction_ClearSelectedArmyTokenAndClosePage(UiNodeBase *control)

{
  UiNodeBase *parentCursor;
  GameEntityRuntime *firstSelectedEntity;
  CommandPayloadDword04 modelOffset;
  
  parentCursor = control->parent;
  while (parentCursor != (UiNodeBase *)0xffffffff) {
    control = control->parent;
    parentCursor = control->parent;
  }
  control[0x23].top = control[0x23].top & 0xfffffff7;
  UiPageStack_SetActiveIndex(0,(UiPageStackControl *)&control[0x27].bottomAnchorQ31);
  firstSelectedEntity = SelectionInfo_GetFirstEntry();
  if (firstSelectedEntity != (GameEntityRuntime *)0x0) {
    modelOffset = (int)(firstSelectedEntity->common).ownership.definitionOrClassRecord -
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
  return;
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
void __thandor_preserve_eax
InGameCommandPanel_OpenPage4AndRefreshAvailability(InGameCommandPanelSourceAddress32 source)

{
  UiNodeBase *firstNode;
  
  UiPageStack_SetActiveIndex(4,(UiPageStackControl *)(source + -0x19e0));
  firstNode = (UiNodeBase *)(source + -0x25b0);
  if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_NETWORKED_MASK) ==
      SESSION_NETWORK_ROLE_LOCAL) {
    UiNodeList_UnsuppressActionId(0x1027,firstNode);
  }
  else {
    UiNodeList_SuppressActionId(0x1027,firstNode);
  }
  if ((g_UiCommandRuntimeFlags & 0x100) == 0) {
    UiNodeList_UnsuppressActionId(0x101e,firstNode);
  }
  else {
    UiNodeList_SuppressActionId(0x101e,firstNode);
  }
  return;
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
void __thandor_void_preserve_eax_ecx_edx
InGameCommandCatalog_SubmitGroup48Entry(UiCatalogEntryControl *source)

{
  UiNodeBase *parentOrFactionIndex;
  UiCatalogEntryControl *root;
  PckArmyAssetIdCatalog assetId;
  int entryIndex;
  
  if ((g_UiCommandRuntimeFlags & 0x101) == 0) {
    parentOrFactionIndex = (source->command).sprite.selectable.base.parent;
    root = source;
    while (parentOrFactionIndex != (UiNodeBase *)0xffffffff) {
      root = (UiCatalogEntryControl *)(root->command).sprite.selectable.base.parent;
      parentOrFactionIndex = (root->command).sprite.selectable.base.parent;
    }
    entryIndex = 0x2f;
    while ((int)source - (int)root !=
           g_UiCatalogGroup48OffsetTables[g_UiCatalogGroup48ColumnCount][entryIndex]) {
      entryIndex = entryIndex + -1;
      if (entryIndex < 0) {
        return;
      }
    }
    if (((source->command).activationInputState & UI_COMMAND_ACTIVATION_RELATION_RESET_REQUEST_MASK)
        == 0) {
      parentOrFactionIndex = root[0x15].command.sprite.selectable.base.nextSibling;
      assetId = g_UiCatalogGroup48Records[entryIndex]->armyAssetId;
      if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_NETWORKED_MASK) ==
          SESSION_NETWORK_ROLE_LOCAL) {
        GameFactionRuntime_RegisterArmyAssetPointers
                  (g_LocalPlayerRuntimeId,1,assetId,(FactionRuntimeIndex)parentOrFactionIndex);
      }
      else {
        InGameCommandQueue_AppendLocalPlayerCommand(0xfe0,1,assetId,(CommandPayloadDword04)parentOrFactionIndex);
      }
    }
    else {
      parentOrFactionIndex = root[0x15].command.sprite.selectable.base.nextSibling;
      assetId = g_UiCatalogGroup48Records[entryIndex]->armyAssetId;
      if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_NETWORKED_MASK) ==
          SESSION_NETWORK_ROLE_LOCAL) {
        GameFactionRuntime_CancelQueuedArmyAssetsAndRefund
                  (g_LocalPlayerRuntimeId,1,assetId,(FactionRuntimeIndex)parentOrFactionIndex);
      }
      else {
        InGameCommandQueue_AppendLocalPlayerCommand(0x1030,1,assetId,(CommandPayloadDword04)parentOrFactionIndex);
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
void __thandor_void_preserve_eax_ecx_edx
InGameCommandCatalog_SubmitGroup42Entry(UiCatalogEntryControl *source)

{
  UiNodeBase *parentOrFactionIndex;
  UiCatalogEntryControl *root;
  PckArmyAssetIdCatalog assetId;
  int entryIndex;
  
  if ((g_UiCommandRuntimeFlags & 0x101) == 0) {
    parentOrFactionIndex = (source->command).sprite.selectable.base.parent;
    root = source;
    while (parentOrFactionIndex != (UiNodeBase *)0xffffffff) {
      root = (UiCatalogEntryControl *)(root->command).sprite.selectable.base.parent;
      parentOrFactionIndex = (root->command).sprite.selectable.base.parent;
    }
    entryIndex = 0x29;
    while ((int)source - (int)root !=
           g_UiCatalogGroup42OffsetTables[g_UiCatalogGroup42ColumnCount][entryIndex]) {
      entryIndex = entryIndex + -1;
      if (entryIndex < 0) {
        return;
      }
    }
    if (((source->command).activationInputState & UI_COMMAND_ACTIVATION_RELATION_RESET_REQUEST_MASK)
        == 0) {
      parentOrFactionIndex = root[0x15].command.sprite.selectable.base.nextSibling;
      assetId = g_UiCatalogGroup42Records[entryIndex]->armyAssetId;
      if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_NETWORKED_MASK) ==
          SESSION_NETWORK_ROLE_LOCAL) {
        GameFactionRuntime_RegisterArmyAssetPointers
                  (g_LocalPlayerRuntimeId,1,assetId,(FactionRuntimeIndex)parentOrFactionIndex);
      }
      else {
        InGameCommandQueue_AppendLocalPlayerCommand(0xfe0,1,assetId,(CommandPayloadDword04)parentOrFactionIndex);
      }
    }
    else {
      parentOrFactionIndex = root[0x15].command.sprite.selectable.base.nextSibling;
      assetId = g_UiCatalogGroup42Records[entryIndex]->armyAssetId;
      if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_NETWORKED_MASK) ==
          SESSION_NETWORK_ROLE_LOCAL) {
        GameFactionRuntime_CancelQueuedArmyAssetsAndRefund
                  (g_LocalPlayerRuntimeId,1,assetId,(FactionRuntimeIndex)parentOrFactionIndex);
      }
      else {
        InGameCommandQueue_AppendLocalPlayerCommand(0x1030,1,assetId,(CommandPayloadDword04)parentOrFactionIndex);
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
void __thandor_void_preserve_eax_ecx_edx
InGameCommandSprite_DispatchVariantAControl24(UiCommandSpriteButtonControl *control)

{
  sdword *flagsField;
  UiNodeBase *parentCursor;
  UiCommandSpriteButtonControl *root;
  UiCommandRuntimeRecordPrefix *runtimeRecord;
  GraphicsTextureSourceAsset *factionToken;
  PckArmyAssetIdCatalog assetId;
  int slotIndex;
  
  if ((g_UiCommandRuntimeFlags & 0x101) == 0) {
    parentCursor = (control->sprite).selectable.base.parent;
    root = control;
    while (parentCursor != (UiNodeBase *)0xffffffff) {
      root = (UiCommandSpriteButtonControl *)(root->sprite).selectable.base.parent;
      parentCursor = (root->sprite).selectable.base.parent;
    }
    g_UiImageControlHoverTarget = (UiImageControl *)0x0;
    flagsField = &root[0x122].sprite.selectable.base.leftOffset;
    *flagsField = *flagsField & 0xfffff9fc;
    if ((root[0x15].sprite.selectable.actionId & 0x10U) == 0) {
      slotIndex = 0x17;
      while ((int)control - (int)root !=
             g_UiCommandSpriteVariantAOffsetTables[g_UiCommandSpriteVariantAColumnCount][slotIndex]) {
        slotIndex = slotIndex + -1;
        if (slotIndex < 0) {
          return;
        }
      }
      runtimeRecord = g_UiCommandSpriteVariantARecords[slotIndex];
      factionToken = root[0x15].sprite.primaryTextureSource;
      if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_NETWORKED_MASK) ==
          SESSION_NETWORK_ROLE_LOCAL) {
        GameFactionRuntime_ConsumePendingArmyAssetAndRefreshGrid
                  (g_LocalPlayerRuntimeId,0,0,(FactionRuntimeIndex)factionToken);
      }
      else {
        InGameCommandQueue_AppendLocalPlayerCommand(0x14f0,0,0,(CommandPayloadDword04)factionToken);
      }
      if ((control->activationInputState & UI_COMMAND_ACTIVATION_RELATION_RESET_REQUEST_MASK) == 0)
      {
        factionToken = root[0x15].sprite.primaryTextureSource;
        assetId = runtimeRecord->armyAssetId;
        if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_NETWORKED_MASK) ==
            SESSION_NETWORK_ROLE_LOCAL) {
          GameFactionRuntime_RemoveArmyAssetAndStagePlayerTransfer
                    (g_LocalPlayerRuntimeId,0,assetId,(FactionRuntimeIndex)factionToken);
        }
        else {
          InGameCommandQueue_AppendLocalPlayerCommand(0x12d0,0,assetId,(CommandPayloadDword04)factionToken);
        }
      }
      else {
        factionToken = root[0x15].sprite.primaryTextureSource;
        assetId = runtimeRecord->armyAssetId;
        if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_NETWORKED_MASK) ==
            SESSION_NETWORK_ROLE_LOCAL) {
          GameFactionRuntime_SellArmyAssetAndRefundSevenEighths
                    (g_LocalPlayerRuntimeId,0,assetId,(FactionRuntimeIndex)factionToken);
        }
        else {
          InGameCommandQueue_AppendLocalPlayerCommand(0x1570,0,assetId,(CommandPayloadDword04)factionToken);
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
void __thandor_void_preserve_eax_ecx_edx
InGameCommandSprite_DispatchFixedControl8(UiCommandSpriteButtonControl *control)

{
  UiNodeBase *parentCursor;
  UiCommandSpriteButtonControl *root;
  GraphicsTextureSourceAsset *payloadDword0C;
  CommandPayloadDword04 payloadDword04;
  CommandPayloadDword08 transferModeFlags;
  bool selectionBlocked;
  
  if ((g_UiCommandRuntimeFlags & 0x101) == 0) {
    parentCursor = (control->sprite).selectable.base.parent;
    root = control;
    while (parentCursor != (UiNodeBase *)0xffffffff) {
      root = (UiCommandSpriteButtonControl *)(root->sprite).selectable.base.parent;
      parentCursor = (root->sprite).selectable.base.parent;
    }
    payloadDword04 = 7;
    do {
      if ((int)control - (int)root == g_UiAction100AControlOffsets[payloadDword04]) {
        transferModeFlags = 0;
        if ((control->activationInputState & UI_COMMAND_ACTIVATION_LOW_INPUT_NIBBLE_MASK) != 0) {
          transferModeFlags = 2;
        }
        if ((control->activationInputState & UI_COMMAND_ACTIVATION_ALTERNATE_BUTTON) != 0) {
          transferModeFlags = transferModeFlags | 1;
        }
        if ((control->activationInputState & UI_COMMAND_ACTIVATION_REPEAT_OR_DOUBLE_CLICK) != 0) {
          transferModeFlags = transferModeFlags | 4;
        }
        if ((transferModeFlags != 0) &&
           (selectionBlocked = SelectionInfo_AllEntriesEmptyOrMatchOwnerCf
                              ((FactionRuntimeIndex)root[0x15].sprite.primaryTextureSource), selectionBlocked
           )) {
          return;
        }
        payloadDword0C = root[0x15].sprite.primaryTextureSource;
        if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_NETWORKED_MASK) !=
            SESSION_NETWORK_ROLE_LOCAL) {
          InGameCommandQueue_AppendLocalPlayerCommand
                    (0xbe0,(CommandPayloadDword0C)payloadDword0C,transferModeFlags,payloadDword04);
          return;
        }
        FrontendPlayerSelection_TransferFactionGroupWithModeAndRefresh
                  (g_LocalPlayerRuntimeId,(FactionRuntimeIndex)payloadDword0C,transferModeFlags,
                   payloadDword04);
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
void __thandor_void_preserve_eax_ecx_edx
InGameCommandState_SelectAndPropagateBinaryMode(UiSelectableControl *source)

{
  UiNodeBase *parentCursor;
  UiSelectableControl *root;
  UiNodeVtable *selectedIndexValue;
  UiSelectableNodeEaxEcxCf9 selectionResult;
  
  parentCursor = (source->base).parent;
  root = source;
  while (parentCursor != (UiNodeBase *)0xffffffff) {
    root = (UiSelectableControl *)(root->base).parent;
    parentCursor = (root->base).parent;
  }
  UiSelectableGroup_SelectExclusive(2,&source->base,
      THANDOR_UI_AT(Thandor_UiRoot(source),0x7c4),
      THANDOR_UI_AT(Thandor_UiRoot(source),0x764));
  selectionResult = UiSelectableGroup_NoneVisibleSelectedCf(2,
      THANDOR_UI_AT(Thandor_UiRoot(source),0x764),
      THANDOR_UI_AT(Thandor_UiRoot(source),0x7c4));
  selectedIndexValue = (UiNodeVtable *)selectionResult.controlIndexOrCount;
  root[0xd].base.left = (sdword)selectedIndexValue;
  root[0xe].base.rightAnchorQ31 = (UiAnchorFractionQ31)selectedIndexValue;
  root[0x10].base.vtable = selectedIndexValue;
  root[0xb].base.vtable = selectedIndexValue;
  return;
}


/* Address: 0x0056D920.
   Ownership: ui/ingame/commands.
   Purpose: Clears UiNodeBase.nodeFlags bit 0x00800000. The original visual-state label is not preserved.
*/
void __thandor_void_preserve_eax_ecx_edx UiCommandModeG_ClearNodeFlag00800000(UiNodeBase *node)

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
void __thandor_void_preserve_eax_ecx_edx InGameCommandModeC_Select0(UiSpriteButtonControl *source)

{
  UiSelectableGroup_SelectExclusive(4,(UiNodeBase *)source,
      THANDOR_UI_AT(source,0x168),
      THANDOR_UI_AT(source,0xf0),
      THANDOR_UI_AT(source,0x78),
      THANDOR_UI_AT(source,0x0));
  g_UiCommandModeC = 0;
  return;
}


/* Address: 0x0056DDA0.
   Ownership: ui/ingame/commands.
   Purpose: Action 0x1031. Exclusively selects the source within a four-control UiSpriteButtonControl group and
   stores command mode C value 1. Original user-facing mode label is not preserved.
   Cross-module calls: UiSelectableGroup_SelectExclusive [ui/controls/lists].
*/
void __thandor_void_preserve_eax_ecx_edx InGameCommandModeC_Select1(UiSpriteButtonControl *source)

{
  UiSelectableGroup_SelectExclusive(4,(UiNodeBase *)source,
      THANDOR_UI_AT(source,0xf0),
      THANDOR_UI_AT(source,0x78),
      THANDOR_UI_AT(source,0x0),
      THANDOR_UI_AT(source,-0x78));
  g_UiCommandModeC = 1;
  return;
}


/* Address: 0x0056DDF0.
   Ownership: ui/ingame/commands.
   Purpose: Action 0x1032. Exclusively selects the source within a four-control UiSpriteButtonControl group and
   stores command mode C value 2. Original user-facing mode label is not preserved.
   Cross-module calls: UiSelectableGroup_SelectExclusive [ui/controls/lists].
*/
void __thandor_void_preserve_eax_ecx_edx InGameCommandModeC_Select2(UiSpriteButtonControl *source)

{
  UiSelectableGroup_SelectExclusive(4,(UiNodeBase *)source,
      THANDOR_UI_AT(source,0x78),
      THANDOR_UI_AT(source,0x0),
      THANDOR_UI_AT(source,-0x78),
      THANDOR_UI_AT(source,-0xf0));
  g_UiCommandModeC = 2;
  return;
}


/* Address: 0x0056DE40.
   Ownership: ui/ingame/commands.
   Purpose: Action 0x1033. Exclusively selects the source within a four-control UiSpriteButtonControl group and
   stores command mode C value 3. Original user-facing mode label is not preserved.
   Cross-module calls: UiSelectableGroup_SelectExclusive [ui/controls/lists].
*/
void __thandor_void_preserve_eax_ecx_edx InGameCommandModeC_Select3(UiSpriteButtonControl *source)

{
  UiSelectableGroup_SelectExclusive(4,(UiNodeBase *)source,
      THANDOR_UI_AT(source,0x0),
      THANDOR_UI_AT(source,-0x78),
      THANDOR_UI_AT(source,-0xf0),
      THANDOR_UI_AT(source,-0x168));
  g_UiCommandModeC = 3;
  return;
}


/* Address: 0x0056DE90.
   Ownership: ui/ingame/commands.
   Purpose: Action 0x1034. Exclusively selects the source within a four-control UiSpriteButtonControl group and
   stores command mode D value 0. Original user-facing mode label is not preserved.
   Cross-module calls: UiSelectableGroup_SelectExclusive [ui/controls/lists].
*/
void __thandor_void_preserve_eax_ecx_edx InGameCommandModeD_Select0(UiSpriteButtonControl *source)

{
  UiSelectableGroup_SelectExclusive(4,(UiNodeBase *)source,
      THANDOR_UI_AT(source,0x168),
      THANDOR_UI_AT(source,0xf0),
      THANDOR_UI_AT(source,0x78),
      THANDOR_UI_AT(source,0x0));
  g_UiCommandModeD = 0;
  return;
}


/* Address: 0x0056DEE0.
   Ownership: ui/ingame/commands.
   Purpose: Action 0x1035. Exclusively selects the source within a four-control UiSpriteButtonControl group and
   stores command mode D value 1. Original user-facing mode label is not preserved.
   Cross-module calls: UiSelectableGroup_SelectExclusive [ui/controls/lists].
*/
void __thandor_void_preserve_eax_ecx_edx InGameCommandModeD_Select1(UiSpriteButtonControl *source)

{
  UiSelectableGroup_SelectExclusive(4,(UiNodeBase *)source,
      THANDOR_UI_AT(source,0xf0),
      THANDOR_UI_AT(source,0x78),
      THANDOR_UI_AT(source,0x0),
      THANDOR_UI_AT(source,-0x78));
  g_UiCommandModeD = 1;
  return;
}


/* Address: 0x0056DF30.
   Ownership: ui/ingame/commands.
   Purpose: Action 0x1036. Exclusively selects the source within a four-control UiSpriteButtonControl group and
   stores command mode D value 2. Original user-facing mode label is not preserved.
   Cross-module calls: UiSelectableGroup_SelectExclusive [ui/controls/lists].
*/
void __thandor_void_preserve_eax_ecx_edx InGameCommandModeD_Select2(UiSpriteButtonControl *source)

{
  UiSelectableGroup_SelectExclusive(4,(UiNodeBase *)source,
      THANDOR_UI_AT(source,0x78),
      THANDOR_UI_AT(source,0x0),
      THANDOR_UI_AT(source,-0x78),
      THANDOR_UI_AT(source,-0xf0));
  g_UiCommandModeD = 2;
  return;
}


/* Address: 0x0056DF80.
   Ownership: ui/ingame/commands.
   Purpose: Action 0x1037. Exclusively selects the source within a four-control UiSpriteButtonControl group and
   stores command mode D value 3. Original user-facing mode label is not preserved.
   Cross-module calls: UiSelectableGroup_SelectExclusive [ui/controls/lists].
*/
void __thandor_void_preserve_eax_ecx_edx InGameCommandModeD_Select3(UiSpriteButtonControl *source)

{
  UiSelectableGroup_SelectExclusive(4,(UiNodeBase *)source,
      THANDOR_UI_AT(source,0x0),
      THANDOR_UI_AT(source,-0x78),
      THANDOR_UI_AT(source,-0xf0),
      THANDOR_UI_AT(source,-0x168));
  g_UiCommandModeD = 3;
  return;
}


/* Address: 0x0056E050.
   Ownership: ui/ingame/commands.
   Purpose: Action 0x1039. Exclusively selects the source within a three-control UiSpriteButtonControl group and
   stores command mode A value 0. Original user-facing mode label is not preserved.
   Cross-module calls: UiSelectableGroup_SelectExclusive [ui/controls/lists].
*/
void __thandor_void_preserve_eax_ecx_edx InGameCommandModeA_Select0(UiSpriteButtonControl *source)

{
  UiSelectableGroup_SelectExclusive(3,(UiNodeBase *)source,
      THANDOR_UI_AT(source,0xf0),
      THANDOR_UI_AT(source,0x78),
      THANDOR_UI_AT(source,0x0));
  g_UiCommandModeA = 0;
  return;
}


/* Address: 0x0056E090.
   Ownership: ui/ingame/commands.
   Purpose: Action 0x103A. Exclusively selects the source within a three-control UiSpriteButtonControl group and
   stores command mode A value 1. Original user-facing mode label is not preserved.
   Cross-module calls: UiSelectableGroup_SelectExclusive [ui/controls/lists].
*/
void __thandor_void_preserve_eax_ecx_edx InGameCommandModeA_Select1(UiSpriteButtonControl *source)

{
  UiSelectableGroup_SelectExclusive(3,(UiNodeBase *)source,
      THANDOR_UI_AT(source,0x0),
      THANDOR_UI_AT(source,-0x78),
      THANDOR_UI_AT(source,-0xf0));
  g_UiCommandModeA = 1;
  return;
}


/* Address: 0x0056E0D0.
   Ownership: ui/ingame/commands.
   Purpose: Action 0x103B. Exclusively selects the source within a three-control UiSpriteButtonControl group and
   stores command mode A value 2. Original user-facing mode label is not preserved.
   Cross-module calls: UiSelectableGroup_SelectExclusive [ui/controls/lists].
*/
void __thandor_void_preserve_eax_ecx_edx InGameCommandModeA_Select2(UiSpriteButtonControl *source)

{
  UiSelectableGroup_SelectExclusive(3,(UiNodeBase *)source,
      THANDOR_UI_AT(source,0x78),
      THANDOR_UI_AT(source,0x0),
      THANDOR_UI_AT(source,-0x78));
  g_UiCommandModeA = 2;
  return;
}


/* Address: 0x0056E110.
   Ownership: ui/ingame/commands.
   Purpose: Action 0x103C. Exclusively selects the source within a three-control UiSpriteButtonControl group and
   stores command mode B value 0. Original user-facing mode label is not preserved.
   Cross-module calls: UiSelectableGroup_SelectExclusive [ui/controls/lists].
*/
void __thandor_void_preserve_eax_ecx_edx InGameCommandModeB_Select0(UiSpriteButtonControl *source)

{
  UiSelectableGroup_SelectExclusive(3,(UiNodeBase *)source,
      THANDOR_UI_AT(source,0xf0),
      THANDOR_UI_AT(source,0x78),
      THANDOR_UI_AT(source,0x0));
  g_UiCommandModeB = 0;
  return;
}


/* Address: 0x0056E150.
   Ownership: ui/ingame/commands.
   Purpose: Action 0x103D. Exclusively selects the source within a three-control UiSpriteButtonControl group and
   stores command mode B value 1. Original user-facing mode label is not preserved.
   Cross-module calls: UiSelectableGroup_SelectExclusive [ui/controls/lists].
*/
void __thandor_void_preserve_eax_ecx_edx InGameCommandModeB_Select1(UiSpriteButtonControl *source)

{
  UiSelectableGroup_SelectExclusive(3,(UiNodeBase *)source,
      THANDOR_UI_AT(source,0x0),
      THANDOR_UI_AT(source,-0x78),
      THANDOR_UI_AT(source,-0xf0));
  g_UiCommandModeB = 1;
  return;
}


/* Address: 0x0056E190.
   Ownership: ui/ingame/commands.
   Purpose: Action 0x103E. Exclusively selects the source within a three-control UiSpriteButtonControl group and
   stores command mode B value 2. Original user-facing mode label is not preserved.
   Cross-module calls: UiSelectableGroup_SelectExclusive [ui/controls/lists].
*/
void __thandor_void_preserve_eax_ecx_edx InGameCommandModeB_Select2(UiSpriteButtonControl *source)

{
  UiSelectableGroup_SelectExclusive(3,(UiNodeBase *)source,
      THANDOR_UI_AT(source,0x78),
      THANDOR_UI_AT(source,0x0),
      THANDOR_UI_AT(source,-0x78));
  g_UiCommandModeB = 2;
  return;
}


/* Address: 0x0056E1D0.
   Ownership: ui/ingame/commands.
   Purpose: Action 0x103F. Selects mode E value 0. It clears the two other contiguous mode-E controls and one
   linked control at source+0x1E0, then stores zero in g_UiCommandModeE. Original labels are not preserved.
   Cross-module calls: UiSelectableGroup_SelectExclusive [ui/controls/lists].
*/
void __thandor_void_preserve_eax_ecx_edx InGameCommandModeE_Select0(UiSpriteButtonControl *source)

{
  UiSelectableGroup_SelectExclusive(4,(UiNodeBase *)source,
      THANDOR_UI_AT(source,0x1e0),
      THANDOR_UI_AT(source,0xf0),
      THANDOR_UI_AT(source,0x78),
      THANDOR_UI_AT(source,0x0));
  g_UiCommandModeE = 0;
  return;
}


/* Address: 0x0056E220.
   Ownership: ui/ingame/commands.
   Purpose: Action 0x1040. Exclusively selects the source within the three contiguous mode-E sprite controls and
   stores value 1 in g_UiCommandModeE.
   Cross-module calls: UiSelectableGroup_SelectExclusive [ui/controls/lists].
*/
void __thandor_void_preserve_eax_ecx_edx InGameCommandModeE_Select1(UiSpriteButtonControl *source)

{
  UiSelectableGroup_SelectExclusive(3,(UiNodeBase *)source,
      THANDOR_UI_AT(source,0x78),
      THANDOR_UI_AT(source,0x0),
      THANDOR_UI_AT(source,-0x78));
  g_UiCommandModeE = 1;
  return;
}


/* Address: 0x0056E260.
   Ownership: ui/ingame/commands.
   Purpose: Action 0x1041. Exclusively selects the source within the three contiguous mode-E sprite controls and
   stores value 2 in g_UiCommandModeE.
   Cross-module calls: UiSelectableGroup_SelectExclusive [ui/controls/lists].
*/
void __thandor_void_preserve_eax_ecx_edx InGameCommandModeE_Select2(UiSpriteButtonControl *source)

{
  UiSelectableGroup_SelectExclusive(3,(UiNodeBase *)source,
      THANDOR_UI_AT(source,0x0),
      THANDOR_UI_AT(source,-0x78),
      THANDOR_UI_AT(source,-0xf0));
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
void __thandor_void_preserve_eax_ecx_edx InGameCommandRange_DispatchState0(UiNodeBase *source)

{
  if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_NETWORKED_MASK) ==
      SESSION_NETWORK_ROLE_LOCAL) {
    TerrainGrid_RunDirectionalRelaxationPasses
              (g_LocalPlayerRuntimeId,0,0x80,TERRAIN_RELAXATION_SIGN_GATED);
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
void __thandor_void_preserve_eax_ecx_edx InGameCommandRange_DispatchState1(UiNodeBase *source)

{
  if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_NETWORKED_MASK) ==
      SESSION_NETWORK_ROLE_LOCAL) {
    TerrainGrid_RunDirectionalRelaxationPasses
              (g_LocalPlayerRuntimeId,0,0x80,TERRAIN_RELAXATION_UNGATED_LAND_TOOL);
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
void __thandor_void_preserve_eax_ecx_edx InGameCommandModeF_Select0(UiSpriteButtonControl *source)

{
  UiSelectableGroup_SelectExclusive(2,(UiNodeBase *)source,
      THANDOR_UI_AT(source,0x78),
      THANDOR_UI_AT(source,0x0));
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
void __thandor_void_preserve_eax_ecx_edx InGameCommandModeF_Select1(UiSpriteButtonControl *source)

{
  UiSelectableGroup_SelectExclusive(2,(UiNodeBase *)source,
      THANDOR_UI_AT(source,0x0),
      THANDOR_UI_AT(source,-0x78));
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
void __thandor_void_preserve_eax_ecx
InGameCommand150_HandlePlayerDepartureAndOwnership
          (PlayerOrFactionRuntimeId32 playerOrFactionId,dword value1,dword value2,
          GameEntityCommandFlags flags)

{
  WorldRuntimeContext *worldRuntime;
  dword factionToken;
  GameEntityRuntime *entityRuntime;
  InGameRuntimeRootImageC3E4 *runtimeRoot;
  FrontendPlayerRuntimeBlockCount remainingPlayers;
  FrontendPlayerRuntimeRecord *playerRecord;
  TextResourceResolveEaxCf5 departureText;
  WorldOwnerListNode100 *ownerNode;
  
  runtimeRoot = g_InGameRuntimeRoot;
  if ((flags & 2) == 0) {
    worldRuntime = &g_InGameRuntimeRoot->worldRuntime0A30;
    remainingPlayers = g_FrontendPlayerRuntimeBlockCount;
    playerRecord = g_FrontendPlayerRuntimeBlocks;
    if ((flags & 1) == 0) {
      do {
        if (playerOrFactionId == playerRecord->playerRuntimeId) {
          playerRecord->heartbeatExpiryTicks = 0;
          if (playerRecord == g_FrontendPlayerRuntimeBlocks) {
            g_SessionTransferTimeoutTicks = 0;
          }
          if (playerOrFactionId == (runtimeRoot->worldRuntime0A30).selection.activePlayerRuntimeId) {
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
            playerRecord = g_FrontendPlayerRuntimeBlocks;
            g_FrontendPlayerRuntimeBlockCount = 1;
            g_LocalPlayerRuntimeId = 0;
            (playerRecord->playerName).textUtf16[0] = 0;
            (playerRecord->playerName).textUtf16[1] = 0;
            playerRecord->playerRuntimeId = 0;
            (playerRecord->factionAssignment).roleStateFlags = 0;
            return;
          }
          departureText = TextResource_Resolve(0xff08);
          RichTextCommandStream_PatchPayloadBySelector(0,&playerRecord->playerName,departureText.eax);
          InGameRecentTextHistory_InsertAndRebuild8(departureText.eax);
          return;
        }
        remainingPlayers = remainingPlayers - 1;
        playerRecord = playerRecord + 1;
      } while (remainingPlayers != 0);
    }
    else {
      factionToken = g_SelectionPlayerRuntimeBlockPointers[playerOrFactionId]->
              primaryEntityOrFactionToken8080;
      for (ownerNode = (g_InGameRuntimeRoot->worldRuntime0A30).ownerListHead;
          ownerNode != (WorldOwnerListNode100 *)0x0; ownerNode = ownerNode->nextNode) {
        if ((ownerNode->ownerClassId == WORLD_OWNER_RUNTIME_MODEL) &&
           (entityRuntime = *(GameEntityRuntime **)((int)ownerNode->runtimePayload + 8),
           factionToken == (entityRuntime->common).ownership.ownerIndex)) {
          ArmyRuntime_DestroyInstanceAndRefreshUi(worldRuntime,entityRuntime);
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
void __thandor_void_preserve_eax_ecx_edx UiCommandModeG_ApplyMaskedColorVariant(void *visualState)

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
void __thandor_void_preserve_eax_ecx_edx UiCommandModeG_SetNodeFlag02000000(UiNodeBase *node)

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
void __thandor_void_preserve_eax_ecx_edx
UiCommandMatrix_SelectIndex(UiCommandModeIndex absoluteIndex,UiNodeBase *root)

{
  GraphicsTextureSourceAsset *firstTexture;
  GraphicsTextureSourceAsset *secondTexture;
  int controlIndex;
  uint pageEnd;
  dword pageBase;
  
  g_UiCommandAbsoluteSelectionIndex = absoluteIndex;
  root[0x20d].topAnchorQ31 =
       (UiAnchorFractionQ31)g_TerrainMaterialTextureSets[absoluteIndex]->entries[0].sourceAsset;
  pageEnd = g_UiCommandSelectionPageBaseIndex + 0xc;
  pageBase = g_UiCommandSelectionPageBaseIndex;
  while( true ) {
    for (; absoluteIndex < pageBase; pageBase = pageBase - 3) {
      pageEnd = pageEnd - 3;
    }
    if (absoluteIndex < pageEnd) break;
    pageBase = pageBase + 3;
    pageEnd = pageEnd + 3;
  }
  firstTexture = (GraphicsTextureSourceAsset *)0x0;
  if (g_TerrainMaterialTextureSets[pageBase] != (GraphicsTextureSet *)0x0) {
    firstTexture = g_TerrainMaterialTextureSets[pageBase]->entries[0].sourceAsset;
  }
  secondTexture = (GraphicsTextureSourceAsset *)0x0;
  if (g_TerrainMaterialTextureSets[pageBase + 1] != (GraphicsTextureSet *)0x0) {
    secondTexture = g_TerrainMaterialTextureSets[pageBase + 1]->entries[0].sourceAsset;
  }
  g_UiCommandSelectionPageBaseIndex = pageBase;
  root[0x234].top = (sdword)firstTexture;
  root[0x236].topAnchorQ31 = (UiAnchorFractionQ31)secondTexture;
  firstTexture = (GraphicsTextureSourceAsset *)0x0;
  if (g_TerrainMaterialTextureSets[pageBase + 2] != (GraphicsTextureSet *)0x0) {
    firstTexture = g_TerrainMaterialTextureSets[pageBase + 2]->entries[0].sourceAsset;
  }
  secondTexture = (GraphicsTextureSourceAsset *)0x0;
  if (g_TerrainMaterialTextureSets[pageBase + 3] != (GraphicsTextureSet *)0x0) {
    secondTexture = g_TerrainMaterialTextureSets[pageBase + 3]->entries[0].sourceAsset;
  }
  root[0x239].parent = (UiNodeBase *)firstTexture;
  root[0x23b].rightOffset = (sdword)secondTexture;
  firstTexture = (GraphicsTextureSourceAsset *)0x0;
  if (g_TerrainMaterialTextureSets[pageBase + 4] != (GraphicsTextureSet *)0x0) {
    firstTexture = g_TerrainMaterialTextureSets[pageBase + 4]->entries[0].sourceAsset;
  }
  secondTexture = (GraphicsTextureSourceAsset *)0x0;
  if (g_TerrainMaterialTextureSets[pageBase + 5] != (GraphicsTextureSet *)0x0) {
    secondTexture = g_TerrainMaterialTextureSets[pageBase + 5]->entries[0].sourceAsset;
  }
  root[0x23d].nodeFlags = (UiNodeFlags)firstTexture;
  root[0x240].bottom = (sdword)secondTexture;
  firstTexture = (GraphicsTextureSourceAsset *)0x0;
  if (g_TerrainMaterialTextureSets[pageBase + 6] != (GraphicsTextureSet *)0x0) {
    firstTexture = g_TerrainMaterialTextureSets[pageBase + 6]->entries[0].sourceAsset;
  }
  secondTexture = (GraphicsTextureSourceAsset *)0x0;
  if (g_TerrainMaterialTextureSets[pageBase + 7] != (GraphicsTextureSet *)0x0) {
    secondTexture = g_TerrainMaterialTextureSets[pageBase + 7]->entries[0].sourceAsset;
  }
  root[0x242].bottomAnchorQ31 = (UiAnchorFractionQ31)firstTexture;
  root[0x245].left = (sdword)secondTexture;
  firstTexture = (GraphicsTextureSourceAsset *)0x0;
  if (g_TerrainMaterialTextureSets[pageBase + 8] != (GraphicsTextureSet *)0x0) {
    firstTexture = g_TerrainMaterialTextureSets[pageBase + 8]->entries[0].sourceAsset;
  }
  secondTexture = (GraphicsTextureSourceAsset *)0x0;
  if (g_TerrainMaterialTextureSets[pageBase + 9] != (GraphicsTextureSet *)0x0) {
    secondTexture = g_TerrainMaterialTextureSets[pageBase + 9]->entries[0].sourceAsset;
  }
  root[0x247].leftAnchorQ31 = (UiAnchorFractionQ31)firstTexture;
  root[0x24a].firstChild = (UiNodeBase *)secondTexture;
  firstTexture = (GraphicsTextureSourceAsset *)0x0;
  if (g_TerrainMaterialTextureSets[pageBase + 10] != (GraphicsTextureSet *)0x0) {
    firstTexture = g_TerrainMaterialTextureSets[pageBase + 10]->entries[0].sourceAsset;
  }
  secondTexture = (GraphicsTextureSourceAsset *)0x0;
  if (g_TerrainMaterialTextureSets[pageBase + 0xb] != (GraphicsTextureSet *)0x0) {
    secondTexture = g_TerrainMaterialTextureSets[pageBase + 0xb]->entries[0].sourceAsset;
  }
  root[0x24c].topOffset = (sdword)firstTexture;
  root[0x24e].layoutHeight = (sdword)secondTexture;
  controlIndex = 0xb;
  do {
    controlIndex = controlIndex + -1;
  } while (-1 < controlIndex);
  /* The original pushes all twelve command controls (offsets 11..0) as the variadic list. */
  UiSelectableGroup_SelectExclusive
            (0xc,(UiNodeBase *)
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
   Ownership: ui/ingame/commands.
   Purpose: Applies three masks to g_UiCommandRuntimeFlags in order: clear, set, then toggle. Typed parameters: p3
   toggleMask→UiCommandRuntimeFlagMask_V342, p4 setMask→UiCommandRuntimeFlagMask_V342, p5
   clearMask→UiCommandRuntimeFlagMask_V342. Calling convention, exact VariableStorage serialization, function body
   bytes, control flow, globals, locals, and executable data remain unchanged.
*/
void __thandor_void_preserve_eax_ecx_edx
UiCommandRuntimeFlags_ApplyClearSetToggleMasks
          (PlayerRuntimeId playerRuntimeId,UiCommandRuntimeFlagMask toggleMask,
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
void __thandor_void_preserve_eax_ecx_edx UiCommandModeG_ClearNodeFlag00100000(UiNodeBase *node)

{
  node[1].nextSibling = (UiNodeBase *)((uint)node[1].nextSibling & 0xffefffff);
  return;
}


/* Address: 0x0056D880.
   Ownership: ui/ingame/commands.
   Purpose: Sets UiNodeBase.nodeFlags bit 0x00200000. The original visual-state label is not preserved.
*/
void __thandor_void_preserve_eax_ecx_edx UiCommandModeG_SetNodeFlag00200000(UiNodeBase *node)

{
  node[1].nextSibling = (UiNodeBase *)((uint)node[1].nextSibling | 0x200000);
  return;
}


/* Address: 0x0056D940.
   Ownership: ui/ingame/commands.
   Purpose: Sets UiNodeBase.nodeFlags bit 0x01000000. The original visual-state label is not preserved.
*/
void __thandor_void_preserve_eax_ecx_edx UiCommandModeG_SetNodeFlag01000000(UiNodeBase *node)

{
  node[1].nextSibling = (UiNodeBase *)((uint)node[1].nextSibling | 0x1000000);
  return;
}


/* Address: 0x0056D8C0.
   Ownership: ui/ingame/commands.
   Purpose: Sets UiNodeBase.nodeFlags bit 0x00000400. The original visual-state label is not preserved.
*/
void __thandor_void_preserve_eax_ecx_edx UiCommandModeG_SetNodeFlag00000400(UiNodeBase *node)

{
  node[1].nextSibling = (UiNodeBase *)((uint)node[1].nextSibling | 0x400);
  return;
}


/* Address: 0x0056D8E0.
   Ownership: ui/ingame/commands.
   Purpose: Clears UiNodeBase.nodeFlags bits 0x00000400 and 0x00000080, exactly matching mask 0xFFFFFB7F. The
   combined original meaning is unresolved.
*/
void __thandor_void_preserve_eax_ecx_edx UiCommandModeG_ClearNodeFlags00000480(UiNodeBase *node)

{
  node[1].nextSibling = (UiNodeBase *)((uint)node[1].nextSibling & 0xfffffb7f);
  return;
}


/* Address: 0x0056D840.
   Ownership: ui/ingame/commands.
   Purpose: Sets UiNodeBase.nodeFlags bit 0x00100000. The original visual-state label is not preserved.
*/
void __thandor_void_preserve_eax_ecx_edx UiCommandModeG_SetNodeFlag00100000(UiNodeBase *node)

{
  node[1].nextSibling = (UiNodeBase *)((uint)node[1].nextSibling | 0x100000);
  return;
}


/* Address: 0x0056D8A0.
   Ownership: ui/ingame/commands.
   Purpose: Clears UiNodeBase.nodeFlags bit 0x00200000. The original visual-state label is not preserved.
*/
void __thandor_void_preserve_eax_ecx_edx UiCommandModeG_ClearNodeFlag00200000(UiNodeBase *node)

{
  node[1].nextSibling = (UiNodeBase *)((uint)node[1].nextSibling & 0xffdfffff);
  return;
}


/* Address: 0x0056D960.
   Ownership: ui/ingame/commands.
   Purpose: Clears UiNodeBase.nodeFlags bit 0x01000000. The original visual-state label is not preserved.
*/
void __thandor_void_preserve_eax_ecx_edx UiCommandModeG_ClearNodeFlag01000000(UiNodeBase *node)

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
void __thandor_void_preserve_eax_ecx_edx UiCommandModeG_ApplyRawColorVariant(void *visualState)

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
void __thandor_void_preserve_eax_ecx_edx UiCommandModeG_ClearNodeFlag02000000(UiNodeBase *node)

{
  node[1].nextSibling = (UiNodeBase *)((uint)node[1].nextSibling & 0xfdffffff);
  return;
}


/* Address: 0x0056D900.
   Ownership: ui/ingame/commands.
   Purpose: Sets UiNodeBase.nodeFlags bit 0x00800000. The original visual-state label is not preserved.
*/
void __thandor_void_preserve_eax_ecx_edx UiCommandModeG_SetNodeFlag00800000(UiNodeBase *node)

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
InGameRuntimeRootImageC3E4 * __thandor_eax_edx_cf_preserve_ecx
UiCommandModeG_SelectAndSyncPages(UiCommandModeIndex modeIndex,UiSelectableControl *source)

{
  UiNodeBase *parentCursor;
  InGameRuntimeRootImageC3E4 *root;
  
  parentCursor = (source->base).parent;
  root = (InGameRuntimeRootImageC3E4 *)source;
  while (parentCursor != (UiNodeBase *)0xffffffff) {
    root = (InGameRuntimeRootImageC3E4 *)(root->rootUi0000).base.parent;
    parentCursor = (root->rootUi0000).base.parent;
  }
  UiSelectableGroup_NoneVisibleSelectedCf(6,
      THANDOR_UI_AT(Thandor_UiRoot(source),0x98b8),
      THANDOR_UI_AT(Thandor_UiRoot(source),0x99a8),
      THANDOR_UI_AT(Thandor_UiRoot(source),0x9930),
      THANDOR_UI_AT(Thandor_UiRoot(source),0x4c94),
      THANDOR_UI_AT(Thandor_UiRoot(source),0x4c1c),
      THANDOR_UI_AT(Thandor_UiRoot(source),0x4ba4));
  UiSelectableGroup_SelectExclusive(6,&source->base,
      THANDOR_UI_AT(Thandor_UiRoot(source),0x98b8),
      THANDOR_UI_AT(Thandor_UiRoot(source),0x99a8),
      THANDOR_UI_AT(Thandor_UiRoot(source),0x9930),
      THANDOR_UI_AT(Thandor_UiRoot(source),0x4c94),
      THANDOR_UI_AT(Thandor_UiRoot(source),0x4c1c),
      THANDOR_UI_AT(Thandor_UiRoot(source),0x4ba4));
  UiPageStack_SetActiveIndex
            (g_UiCommandModeGPrimaryPageIndices[modeIndex],
             (UiPageStackControl *)(root->opaque9A74_9B4B + 0x18));
  UiPageStack_SetActiveIndex
            (g_UiCommandModeGSecondaryPageIndices[modeIndex],
             (UiPageStackControl *)root->opaque9EE0_9FAB);
  UiPageStack_SetActiveIndex
            (g_UiCommandModeGTertiaryPageIndices[modeIndex],
             (UiPageStackControl *)(root->opaqueA06C_C3E3 + 0x1130));
  g_UiCommandModeG = modeIndex;
  return root;
}


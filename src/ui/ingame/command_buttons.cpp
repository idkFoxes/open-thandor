/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/src/ui/ingame/command_buttons.cpp
 * Reverse engineering by idkFoxes 2026
 */

#include <thandor/ui/ingame/command_buttons.h>
#include <thandor/thandor.h>

/* Implementation ownership: ui/ingame/command_buttons. */

/* Pointer press of the command sprite buttons (nonRightPress and rightPress of g_UiCommandSpriteButtonWithDetailsVtable,
   g_UiCommandSpriteButtonControlVtable and g_UiCatalogEntryControlVtable): shows the button pressed and starts a new
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

/* Left/middle button release of the command sprite buttons (nonRightRelease of g_UiCommandSpriteButtonWithDetailsVtable and
   g_UiCommandSpriteButtonControlVtable): when the press started on this button, adds the modifier keys held now to
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

/* Right button release of the command sprite buttons (rightRelease of g_UiCommandSpriteButtonWithDetailsVtable,
   g_UiCommandSpriteButtonControlVtable and g_UiCatalogEntryControlVtable): like the left release, but replaces activationInputState
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

UiNodeVtable g_UiCommandSpriteButtonWithDetailsVtable = {
        .relocate = THANDOR_FN(UiSpriteButtonControl_Relocate),
        .method04 = THANDOR_FN(UiNode_DefaultMethod04_NoOp),
        .drawClipped = THANDOR_FN(UiSpriteButtonControl_DrawClipped),
        .layout = THANDOR_FN(UiContainer_LayoutChildren),
        .nonRightPress = THANDOR_FN(UiCommandSpriteButtonControl_BeginPress),
        .nonRightRelease = THANDOR_FN(UiCommandSpriteButtonControl_NonRightRelease),
        .rightPress = THANDOR_FN(UiCommandSpriteButtonControl_BeginPress),
        .rightRelease = THANDOR_FN(UiCommandSpriteButtonControl_RightRelease),
        .nonRightDrag = THANDOR_FN(UiSpriteButtonControl_NonRightDrag),
        .rightDrag = THANDOR_FN(UiSpriteButtonControl_NonRightDrag),
        .pointerMove = THANDOR_FN(InGameArmyStock_PointerMoveShowSlotDetails),
        .hitTest = THANDOR_FN(UiSpriteButtonControl_HitTestOpaque),
        .keyboardEvent = THANDOR_FN(UiSelectableControl_KeyboardEvent),
        .applyFlags = THANDOR_FN(UiNode_ApplyFlagsRecursive),
        .suppressActionId = THANDOR_FN(UiSelectableControl_SuppressIfActionId),
        .unsuppressActionId = THANDOR_FN(UiSelectableControl_UnsuppressIfActionId),
        .tick = THANDOR_FN(UiNode_DefaultTick),
        .pointerWheel = THANDOR_FN(UiNode_ForwardPointerWheelToParent),
};

UiNodeVtable g_UiCommandSpriteButtonControlVtable = {
        .relocate = THANDOR_FN(UiSpriteButtonControl_Relocate),
        .method04 = THANDOR_FN(UiNode_DefaultMethod04_NoOp),
        .drawClipped = THANDOR_FN(UiSpriteButtonControl_DrawClipped),
        .layout = THANDOR_FN(UiContainer_LayoutChildren),
        .nonRightPress = THANDOR_FN(UiCommandSpriteButtonControl_BeginPress),
        .nonRightRelease = THANDOR_FN(UiCommandSpriteButtonControl_NonRightRelease),
        .rightPress = THANDOR_FN(UiCommandSpriteButtonControl_BeginPress),
        .rightRelease = THANDOR_FN(UiCommandSpriteButtonControl_RightRelease),
        .nonRightDrag = THANDOR_FN(UiSpriteButtonControl_NonRightDrag),
        .rightDrag = THANDOR_FN(UiSpriteButtonControl_NonRightDrag),
        .pointerMove = THANDOR_FN(UiNode_DefaultPointerMove),
        .hitTest = THANDOR_FN(UiSpriteButtonControl_HitTestOpaque),
        .keyboardEvent = THANDOR_FN(UiSelectableControl_KeyboardEvent),
        .applyFlags = THANDOR_FN(UiNode_ApplyFlagsRecursive),
        .suppressActionId = THANDOR_FN(UiSelectableControl_SuppressIfActionId),
        .unsuppressActionId = THANDOR_FN(UiSelectableControl_UnsuppressIfActionId),
        .tick = THANDOR_FN(UiNode_DefaultTick),
        .pointerWheel = THANDOR_FN(UiNode_ForwardPointerWheelToParent),
};

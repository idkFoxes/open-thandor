/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/src/ui/ingame/command_buttons.cpp
 * Reverse engineering by idkFoxes 2026
 */

#include <thandor/ui/ingame/command_buttons.h>
#include <thandor/thandor.h>
#include <thandor/core/bytes.h>

/* Module data. */

UiNodeVtable g_UiCommandVisibilitySingleLineTextVtable = {
    .relocate = UI_SLOT(UiSingleLineTextControl_RelocateChild),
    .method04 = UI_SLOT(UiNode_DefaultMethod04_NoOp),
    .drawClipped = UI_SLOT(UiCommandVisibilitySingleLineText_DrawWhenAllowed),
    .layout = UI_SLOT(UiContainer_LayoutChildren),
    .nonRightPress = UI_SLOT(UiNode_DefaultNonRightPress),
    .nonRightRelease = UI_SLOT(UiNode_DefaultNonRightRelease),
    .rightPress = UI_SLOT(UiNode_ForwardRightPressToParent),
    .rightRelease = UI_SLOT(UiNode_DefaultRightRelease),
    .nonRightDrag = UI_SLOT(UiNode_DefaultNonRightDrag),
    .rightDrag = UI_SLOT(UiNode_DefaultRightDrag),
    .pointerMove = UI_SLOT(UiNode_DefaultPointerMove),
    .hitTest = UI_SLOT(FrontendResultsTable_HitTestAlwaysNone),
    .keyboardEvent = UI_SLOT(UiNode_DefaultKeyboardEventMoveFocusNext),
    .applyFlags = UI_SLOT(UiNode_ApplyFlagsRecursive),
    .suppressActionId = UI_SLOT(UiContainer_SuppressActionId),
    .unsuppressActionId = UI_SLOT(UiContainer_UnsuppressActionId),
    .tick = UI_SLOT(UiNode_DefaultTick),
    .pointerWheel = UI_SLOT(UiNode_ForwardPointerWheelToParent),
};

UiNodeVtable g_UiCommandVisibilityWrappedTextVtable = {
    .relocate = UI_SLOT(UiWrappedTextControl_RelocateAndApplyDeferredOffset),
    .method04 = UI_SLOT(UiNode_DefaultMethod04_NoOp),
    .drawClipped = UI_SLOT(UiCommandVisibilityWrappedText_DrawWhenAllowed),
    .layout = UI_SLOT(UiContainer_LayoutChildren),
    .nonRightPress = UI_SLOT(UiNode_DefaultNonRightPress),
    .nonRightRelease = UI_SLOT(UiNode_DefaultNonRightRelease),
    .rightPress = UI_SLOT(UiNode_ForwardRightPressToParent),
    .rightRelease = UI_SLOT(UiNode_DefaultRightRelease),
    .nonRightDrag = UI_SLOT(UiNode_DefaultNonRightDrag),
    .rightDrag = UI_SLOT(UiNode_DefaultRightDrag),
    .pointerMove = UI_SLOT(UiNode_DefaultPointerMove),
    .hitTest = UI_SLOT(FrontendResultsTable_HitTestAlwaysNone),
    .keyboardEvent = UI_SLOT(UiNode_DefaultKeyboardEventMoveFocusNext),
    .applyFlags = UI_SLOT(UiNode_ApplyFlagsRecursive),
    .suppressActionId = UI_SLOT(UiContainer_SuppressActionId),
    .unsuppressActionId = UI_SLOT(UiContainer_UnsuppressActionId),
    .tick = UI_SLOT(UiNode_DefaultTick),
    .pointerWheel = UI_SLOT(UiNode_ForwardPointerWheelToParent),
};

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
    control->activationInputState = UiCommandActivationStateFlags{};
    stateFlagsField = &(control->sprite).selectable.stateFlags;
    *stateFlagsField = *stateFlagsField | UI_SELECTABLE_SELECTED_OR_CHECKED;
    UiNode_InvalidateRoot(&(control->sprite).selectable.base);
    if (((control->sprite).selectable.base.nodeFlags & UI_NODE_REPEAT_OR_DOUBLE_CLICK) != 0) {
      control->activationInputState =
           control->activationInputState | UI_COMMAND_ACTIVATION_REPEAT_OR_DOUBLE_CLICK;
    }
  }
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
    inputStateBits = FromBits<UiCommandActivationStateFlags>(g_KeyboardStateMask) &
            ~(UI_COMMAND_ACTIVATION_ALTERNATE_BUTTON|UI_COMMAND_ACTIVATION_REPEAT_OR_DOUBLE_CLICK);
    stateFlagsField = &(control->sprite).selectable.stateFlags;
    *stateFlagsField = *stateFlagsField & ~UI_SELECTABLE_SELECTED_OR_CHECKED;
    control->activationInputState = control->activationInputState | inputStateBits;
    if ((((control->sprite).selectable.stateFlags & UI_SPRITE_BUTTON_ACTIVATION_SOUND) != 0) &&
       ((control->sprite).activationSound != nullptr)) {
      g_SoundPlayOneShot
                (g_UiSoundGainQ15,g_UiSoundGainQ15,
                 (control->sprite).activationSound,nullptr);
    }
    UiActionQueue_Enqueue((control->sprite).selectable.actionId,control);
    UiNode_InvalidateRoot(&(control->sprite).selectable.base);
  }
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
    inputStateBits = FromBits<UiCommandActivationStateFlags>(g_KeyboardStateMask) & ~UI_COMMAND_ACTIVATION_REPEAT_OR_DOUBLE_CLICK;
    (control->sprite).selectable.stateFlags &= ~UI_SELECTABLE_SELECTED_OR_CHECKED;
    control->activationInputState = inputStateBits | UI_COMMAND_ACTIVATION_ALTERNATE_BUTTON;
    if ((((control->sprite).selectable.stateFlags & UI_SPRITE_BUTTON_ACTIVATION_SOUND) != 0) &&
       ((control->sprite).activationSound != nullptr)) {
      g_SoundPlayOneShot
                (g_UiSoundGainQ15,g_UiSoundGainQ15,
                 (control->sprite).activationSound,nullptr);
    }
    UiActionQueue_Enqueue((control->sprite).selectable.actionId,control);
    UiNode_InvalidateRoot(&(control->sprite).selectable.base);
  }
}

/* drawClipped of g_UiCommandVisibilityWrappedTextVtable (the wrapped world view status text): draws the text
   unless g_UiCommandRuntimeFlags bit 0x200 hides all these texts; with label flag 0x800 only while the game is
   paused.
*/
void UiCommandVisibilityWrappedText_DrawWhenAllowed
          (UiPixelCoordinate clipBottom,UiPixelCoordinate clipRight,UiPixelCoordinate clipTop,
          UiPixelCoordinate clipLeft,UiNodeBase *control)

{
  if (!Any(g_UiCommandRuntimeFlags & UI_COMMAND_RUNTIME_FLAG_HIDE_WORLD_TEXTS) &&
      (((UiNode_As<UiWrappedTextControl>(control))->labelFlags & UI_WORLD_TEXT_PAUSED_ONLY) == 0 ||
       Any(g_UiCommandRuntimeFlags & UI_COMMAND_RUNTIME_FLAG_PAUSED)) &&
      (control->nodeFlags & UI_NODE_SUPPRESSED) == 0) {
    UiWrappedTextControl_DrawClipped(clipBottom,clipRight,clipTop,clipLeft,UiNode_As<UiWrappedTextControl>(control));
  }
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

  textControl = UiNode_As<UiSingleLineTextControl>(control);
  if (Any(g_UiCommandRuntimeFlags & UI_COMMAND_RUNTIME_FLAG_HIDE_WORLD_TEXTS)) {
    return;
  }
  if ((textControl->labelFlags & UI_WORLD_TEXT_PAUSED_ONLY) != 0 &&
      !Any(g_UiCommandRuntimeFlags & UI_COMMAND_RUNTIME_FLAG_PAUSED)) {
    return;
  }
  drawOffsetAdjust = 0;
  if ((textControl->labelFlags & UI_WORLD_TEXT_SHIFT_BY_STEP_TICKS) != 0) {
    if (!(1 < g_InGameSimulationStepTicks)) {
      return;
    }
    drawOffsetAdjust = g_InGameSimulationStepTicks - 2;
  }
  textControl->text = reinterpret_cast<uint16_t *>(Thandor_Bytes(textControl->text.get()) + drawOffsetAdjust); /* shifted by bytes */
  UiSingleLineTextControl_DrawClipped(clipBottom,clipRight,clipTop,clipLeft,textControl);
  textControl->text = reinterpret_cast<uint16_t *>(Thandor_Bytes(textControl->text.get()) - drawOffsetAdjust); /* back by the same bytes */
}

UiNodeVtable g_UiCommandSpriteButtonWithDetailsVtable = {
        .relocate = UI_SLOT(UiSpriteButtonControl_Relocate),
        .method04 = UI_SLOT(UiNode_DefaultMethod04_NoOp),
        .drawClipped = UI_SLOT(UiSpriteButtonControl_DrawClipped),
        .layout = UI_SLOT(UiContainer_LayoutChildren),
        .nonRightPress = UI_SLOT(UiCommandSpriteButtonControl_BeginPress),
        .nonRightRelease = UI_SLOT(UiCommandSpriteButtonControl_NonRightRelease),
        .rightPress = UI_SLOT(UiCommandSpriteButtonControl_BeginPress),
        .rightRelease = UI_SLOT(UiCommandSpriteButtonControl_RightRelease),
        .nonRightDrag = UI_SLOT(UiSpriteButtonControl_NonRightDrag),
        .rightDrag = UI_SLOT(UiSpriteButtonControl_NonRightDrag),
        .pointerMove = UI_SLOT(InGameArmyStock_PointerMoveShowSlotDetails),
        .hitTest = UI_SLOT(UiSpriteButtonControl_HitTestOpaque),
        .keyboardEvent = UI_SLOT(UiSelectableControl_KeyboardEvent),
        .applyFlags = UI_SLOT(UiNode_ApplyFlagsRecursive),
        .suppressActionId = UI_SLOT(UiSelectableControl_SuppressIfActionId),
        .unsuppressActionId = UI_SLOT(UiSelectableControl_UnsuppressIfActionId),
        .tick = UI_SLOT(UiNode_DefaultTick),
        .pointerWheel = UI_SLOT(UiNode_ForwardPointerWheelToParent),
};

UiNodeVtable g_UiCommandSpriteButtonControlVtable = {
        .relocate = UI_SLOT(UiSpriteButtonControl_Relocate),
        .method04 = UI_SLOT(UiNode_DefaultMethod04_NoOp),
        .drawClipped = UI_SLOT(UiSpriteButtonControl_DrawClipped),
        .layout = UI_SLOT(UiContainer_LayoutChildren),
        .nonRightPress = UI_SLOT(UiCommandSpriteButtonControl_BeginPress),
        .nonRightRelease = UI_SLOT(UiCommandSpriteButtonControl_NonRightRelease),
        .rightPress = UI_SLOT(UiCommandSpriteButtonControl_BeginPress),
        .rightRelease = UI_SLOT(UiCommandSpriteButtonControl_RightRelease),
        .nonRightDrag = UI_SLOT(UiSpriteButtonControl_NonRightDrag),
        .rightDrag = UI_SLOT(UiSpriteButtonControl_NonRightDrag),
        .pointerMove = UI_SLOT(UiNode_DefaultPointerMove),
        .hitTest = UI_SLOT(UiSpriteButtonControl_HitTestOpaque),
        .keyboardEvent = UI_SLOT(UiSelectableControl_KeyboardEvent),
        .applyFlags = UI_SLOT(UiNode_ApplyFlagsRecursive),
        .suppressActionId = UI_SLOT(UiSelectableControl_SuppressIfActionId),
        .unsuppressActionId = UI_SLOT(UiSelectableControl_UnsuppressIfActionId),
        .tick = UI_SLOT(UiNode_DefaultTick),
        .pointerWheel = UI_SLOT(UiNode_ForwardPointerWheelToParent),
};

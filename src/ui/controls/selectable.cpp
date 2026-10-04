/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/src/ui/controls/selectable.cpp
 * Reverse engineering by idkFoxes 2026
 */

#include <thandor/ui/controls/selectable.h>
#include <thandor/thandor.h>
#include <stdarg.h>

/* Keyboard handler shared by the buttons, check boxes and similar selectable controls (keyboardEvent of
   g_UiSpriteButtonControlVtable, g_UiImageControlVtable, g_UiWindowControlVtable, g_UiFramedTextButtonControlVtable,
   g_UiCatalogEntryControlVtable, g_UiCommandSpriteButtonControlVtable and
   g_UiCommandSpriteButtonWithDetailsVtable). Space on the focused control, or Enter / Escape when the control binds them,
   activates it: a push button queues its action, a toggle flips its selected state, a radio-style control
   gets selected; optionally with the activation sound. Other keys go to the default focus handling.
*/
Bool8 UiSelectableControl_KeyboardEvent(UiKeyboardStateMask keyboardStateMask,UiKeyboardEventCode keyCode,
          UiSoundSelectableControl *control)

{
  Bool8 handled;
  Bool8 activates;

  /* Space activates the focused control (unless disabled); Enter/Escape activate it when the state flags
     bind them. Everything else goes to the default focus handling. */
  activates = false;
  if (((control->selectable).base.nodeFlags & UI_NODE_SUPPRESSED) == 0) {
    if (keyCode == KEYBOARD_KEY_CODE_SPACE) {
      activates = (&(control->selectable).base == g_UiKeyboardFocusNode) &&
                  (((control->selectable).stateFlags & UI_SELECTABLE_IGNORE_FOCUSED_SPACE_ACTIVATION) == 0);
    }
    else if (keyCode == KEYBOARD_KEY_CODE_ENTER) {
      activates = ((control->selectable).stateFlags & UI_SELECTABLE_ACTIVATE_ON_ENTER) != 0;
    }
    else if (keyCode == KEYBOARD_KEY_CODE_ESCAPE) {
      activates = ((control->selectable).stateFlags & UI_SELECTABLE_ACTIVATE_ON_ESCAPE) != 0;
    }
  }
  if (!activates) {
    handled = UiNode_DefaultKeyboardEventMoveFocusNext
                        (keyboardStateMask,keyCode,&(control->selectable).base);
    return handled;
  }
  if (((control->selectable).stateFlags & UI_SELECTABLE_PERSISTENT_ACTIVATION_MODE) == 0) {
    if ((((control->selectable).stateFlags & UI_SELECTABLE_PLAY_KEYBOARD_SOUND) != 0) &&
       (control->activationSound != nullptr)) {
      g_SoundPlayOneShot(g_UiSoundGainQ15,g_UiSoundGainQ15,control->activationSound,nullptr);
    }
    UiActionQueue_Enqueue((control->selectable).actionId,control);
    UiNode_InvalidateRoot(&(control->selectable).base);
    return false;
  }
  if (((control->selectable).stateFlags & UI_SELECTABLE_TOGGLE_ON_ACTIVATION) != 0) {
    if ((((control->selectable).stateFlags & UI_SELECTABLE_PLAY_KEYBOARD_SOUND) != 0) &&
       (control->activationSound != nullptr)) {
      g_SoundPlayOneShot(g_UiSoundGainQ15,g_UiSoundGainQ15,control->activationSound,nullptr);
    }
    (control->selectable).stateFlags =
         (control->selectable).stateFlags ^ UI_SELECTABLE_SELECTED_OR_CHECKED;
    UiActionQueue_Enqueue((control->selectable).actionId,control);
    UiNode_InvalidateRoot(&(control->selectable).base);
    return false;
  }
  if (((control->selectable).stateFlags & UI_SELECTABLE_SELECTED_OR_CHECKED) == 0) {
    if ((((control->selectable).stateFlags & UI_SELECTABLE_PLAY_KEYBOARD_SOUND) != 0) &&
       (control->activationSound != nullptr)) {
      g_SoundPlayOneShot(g_UiSoundGainQ15,g_UiSoundGainQ15,control->activationSound,nullptr);
    }
    (control->selectable).stateFlags =
         (control->selectable).stateFlags | UI_SELECTABLE_SELECTED_OR_CHECKED;
    UiActionQueue_Enqueue((control->selectable).actionId,control);
    UiNode_InvalidateRoot(&(control->selectable).base);
    return false;
  }
  /* Persistent, non-toggling control that is already selected: not consumed. */
  handled = UiNode_DefaultKeyboardEventMoveFocusNext
                      (keyboardStateMask,keyCode,&(control->selectable).base);
  return handled;
}

/* Disables (greys out) a selectable control bound to actionId: sets UI_NODE_SUPPRESSED and gives up the
   keyboard focus if it had it. suppressActionId of g_UiGraphicsAdapterTextButtonVtable,
   g_UiSpriteButtonControlVtable, g_UiImageControlVtable, g_UiWindowControlVtable, g_UiTextButtonControlVtable,
   g_UiNumericPairTextButtonVtable, g_UiPayloadPairTextButtonVtable, g_UiFramedTextButtonControlVtable,
   g_UiCatalogEntryControlVtable, g_UiCommandSpriteButtonControlVtable and g_UiCommandSpriteButtonWithDetailsVtable.
*/
void UiSelectableControl_SuppressIfActionId(UiActionId actionId,UiSelectableControl *control)

{
  UiNodeFlags *controlNodeFlags;
  
  if (actionId == control->actionId) {
    controlNodeFlags = &(control->base).nodeFlags;
    *controlNodeFlags = *controlNodeFlags | UI_NODE_SUPPRESSED;
    UiKeyboardFocus_ReleaseNode(&control->base);
  }
  return;
}

/* Re-enables a selectable control bound to actionId: clears UI_NODE_SUPPRESSED and takes the keyboard focus
   when no node has it. unsuppressActionId of g_UiGraphicsAdapterTextButtonVtable,
   g_UiSpriteButtonControlVtable, g_UiImageControlVtable, g_UiWindowControlVtable, g_UiTextButtonControlVtable,
   g_UiNumericPairTextButtonVtable, g_UiPayloadPairTextButtonVtable, g_UiFramedTextButtonControlVtable,
   g_UiCatalogEntryControlVtable, g_UiCommandSpriteButtonControlVtable and g_UiCommandSpriteButtonWithDetailsVtable.
*/
void UiSelectableControl_UnsuppressIfActionId(UiActionId actionId,UiSelectableControl *control)

{
  UiNodeFlags *controlNodeFlags;
  
  if (actionId == control->actionId) {
    controlNodeFlags = &(control->base).nodeFlags;
    *controlNodeFlags = *controlNodeFlags & ~UI_NODE_SUPPRESSED;
    UiKeyboardFocus_AcquireIfNone(&control->base);
  }
  return;
}

/* Finds the first enabled (not suppressed), selected control of a group (controlCount control pointers
   follow controlCount as variadic arguments). Returns true when there is one, with its node in *outNode and its
   index in *outIndex; false when none is. Both out-parameters are optional (NULL) and written in either case.
   Original quirk: when none is selected, *outNode is the last control of the group and *outIndex is
   controlCount (where the original's search loop stops); some callers use them without testing the result.
*/
Bool8 UiSelectableGroup_FindVisibleSelected
          (UiNodeBase **outNode,uint32_t *outIndex,UiControlCount controlCount,...)

{
  va_list controlArgs;
  UiSelectableControl *control;
  uint32_t controlIndex;
  Bool8 found;

  /* The control pointers follow controlCount as variadic arguments. */
  va_start(controlArgs,controlCount);
  controlIndex = 0;
  found = true;
  control = va_arg(controlArgs,UiSelectableControl *);
  while (((control->base.nodeFlags & UI_NODE_SUPPRESSED) != 0) ||
         ((control->stateFlags & UI_SELECTABLE_SELECTED_OR_CHECKED) == 0)) {
    controlIndex++;
    if (controlCount <= controlIndex) {
      found = false;
      break;
    }
    control = va_arg(controlArgs,UiSelectableControl *);
  }
  va_end(controlArgs);
  if (outNode != nullptr) {
    *outNode = (UiNodeBase *)control;
  }
  if (outIndex != nullptr) {
    *outIndex = controlIndex;
  }
  return found;
}

/* Returns the index of the first selected control of a group (controlCount control pointers follow on the
   stack), suppressed ones included, or controlCount when none is selected. Called by the frontend scenario
   page (src/ui/frontend/state.cpp).
*/
uint32_t UiSelectableGroup_SelectedIndex(UiControlCount controlCount,...)

{
  va_list controlArgs;
  uint32_t controlIndex;

  va_start(controlArgs,controlCount);
  controlIndex = 0;
  do {
    if ((va_arg(controlArgs,UiSelectableControl *)->stateFlags & UI_SELECTABLE_SELECTED_OR_CHECKED) != 0) {
      va_end(controlArgs);
      return controlIndex;
    }
    controlIndex++;
  } while (controlIndex < controlCount);
  va_end(controlArgs);
  return controlIndex;
}

/* Radio-button behaviour for a group (controlCount control pointers follow selectedControl on the stack):
   selects selectedControl, deselects all other group members and redraws them.
*/
void UiSelectableGroup_SelectExclusive(UiControlCount controlCount,UiNodeBase *selectedControl,...)

{
  va_list controlArgs;
  UiSelectableControl *node;
  uint32_t controlIndex;

  va_start(controlArgs,selectedControl);
  controlIndex = 0;
  do {
    node = va_arg(controlArgs,UiSelectableControl *);
    if (&node->base == selectedControl) {
      node->stateFlags = node->stateFlags | UI_SELECTABLE_SELECTED_OR_CHECKED;
    }
    else {
      node->stateFlags = node->stateFlags & ~UI_SELECTABLE_SELECTED_OR_CHECKED;
    }
    UiNode_InvalidateRoot(&node->base);
    controlIndex++;
  } while (controlIndex < controlCount);
  va_end(controlArgs);
  return;
}

/* Tells whether a selectable control counts as selected/checked: only a visible (not suppressed)
   control can be.
*/
uint8_t UiSelectableControl_IsSelected(UiSelectableControl *control)

{
  if ((((control->base).nodeFlags & UI_NODE_SUPPRESSED) == 0) &&
     ((control->stateFlags & UI_SELECTABLE_SELECTED_OR_CHECKED) != 0)) {
    return 1;
  }
  return 0;
}

/* Sets or clears the selected/checked state of a selectable control (checkbox, radio or toggle button)
   from code, without queueing its action, and redraws it.
*/
void UiSelectableControl_SetSelected(UiBooleanState32 selected,UiSelectableControl *control)

{
  control->stateFlags = control->stateFlags & ~UI_SELECTABLE_SELECTED_OR_CHECKED;
  if (selected != 0) {
    control->stateFlags = control->stateFlags | UI_SELECTABLE_SELECTED_OR_CHECKED;
  }
  UiNode_InvalidateRoot(&control->base);
  return;
}

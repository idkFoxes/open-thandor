/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/include/thandor/ui/controls/selectable.h
 * Reverse engineering by idkFoxes 2026
 */

#ifndef THANDOR_UI_CONTROLS_SELECTABLE_H
#define THANDOR_UI_CONTROLS_SELECTABLE_H

#include <thandor/core/types.h>
#include <thandor/ui/controls/types.h>
#include <thandor/ui/frontend/types.h>
#include <thandor/core/contracts.h>

/* Submodule: ui/controls/selectable. */

/* UiSelectableControl stateFlags bits read by UiSelectableControl_KeyboardEvent: Enter / Escape also activate
   the control; 0x80 plays UiSoundSelectableControl.activationSound on a keyboard activation (the same bit
   is UI_SPRITE_BUTTON_ANIMATED for sprite buttons). */
#define UI_SELECTABLE_ACTIVATE_ON_ENTER 0x04
#define UI_SELECTABLE_ACTIVATE_ON_ESCAPE 0x08
#define UI_SELECTABLE_PLAY_KEYBOARD_SOUND 0x80

/* Functions are grouped by semantic ownership. */

Bool8 UiSelectableControl_KeyboardEvent(UiKeyboardStateMask keyboardStateMask,UiKeyboardEventCode keyCode,
          UiSoundSelectableControl *control);

void UiSelectableControl_SuppressIfActionId(UiActionId actionId,UiSelectableControl *control);

void UiSelectableControl_UnsuppressIfActionId(UiActionId actionId,UiSelectableControl *control);

Bool8 UiSelectableGroup_FindVisibleSelected
          (UiNodeBase **outNode,uint32_t *outIndex,UiControlCount controlCount,...);

uint32_t UiSelectableGroup_SelectedIndex(UiControlCount controlCount,...);

void UiSelectableGroup_SelectExclusive(UiControlCount controlCount,UiNodeBase *selectedControl,...);

uint8_t UiSelectableControl_IsSelected(UiSelectableControl *control);

void UiSelectableControl_SetSelected(UiBooleanState32 selected,UiSelectableControl *control);

#endif /* THANDOR_UI_CONTROLS_SELECTABLE_H */

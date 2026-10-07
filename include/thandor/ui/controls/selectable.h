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

/* The UiSelectableControl stateFlags bits UI_SELECTABLE_* are the enum class UiSelectableStateFlags in
   ui/controls/types.h. */

bool UiSelectableControl_KeyboardEvent(UiKeyboardStateMask keyboardStateMask,UiKeyboardEventCode keyCode,
          UiSoundSelectableControl *control);

void UiSelectableControl_SuppressIfActionId(UiActionId actionId,UiSelectableControl *control);

void UiSelectableControl_UnsuppressIfActionId(UiActionId actionId,UiSelectableControl *control);

bool UiSelectableGroup_FindVisibleSelected
          (UiNodeBase **outNode,uint32_t *outIndex,UiControlCount controlCount,...);

uint32_t UiSelectableGroup_SelectedIndex(UiControlCount controlCount,...);

void UiSelectableGroup_SelectExclusive(UiControlCount controlCount,UiNodeBase *selectedControl,...);

bool UiSelectableControl_IsSelected(UiSelectableControl *control);

void UiSelectableControl_SetSelected(UiBooleanState32 selected,UiSelectableControl *control);

#endif /* THANDOR_UI_CONTROLS_SELECTABLE_H */

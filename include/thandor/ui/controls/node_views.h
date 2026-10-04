/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/include/thandor/ui/controls/node_views.h
 */

#ifndef THANDOR_UI_CONTROLS_NODE_VIEWS_H
#define THANDOR_UI_CONTROLS_NODE_VIEWS_H

/* The UI control types as prefixed views of their base node (core/slot.h THANDOR_SLOT_PREFIX): a
   vtable slot taking a UiNodeBase * (or UiSelectableControl *, ...) accepts a function taking the control type.
   Each control starts with its base struct at offset 0 (checked by the registration). */

#include <thandor/core/slot.h>
#include <thandor/ui/controls/types.h>

#ifdef __cplusplus
THANDOR_SLOT_PREFIX(UiNumericTextControl, base);
THANDOR_SLOT_PREFIX(UiSelectableControl, base);
THANDOR_SLOT_PREFIX(UiSoundSelectableControl, selectable);
THANDOR_SLOT_PREFIX(UiPageStackControl, base);
THANDOR_SLOT_PREFIX(UiPointerListControl, base);
THANDOR_SLOT_PREFIX(UiTextEditControl, base);
THANDOR_SLOT_PREFIX(UiTimedListControl, base);
THANDOR_SLOT_PREFIX(UiTimedListTreeControl, base);
THANDOR_SLOT_PREFIX(UiTextListControl, base);
THANDOR_SLOT_PREFIX(UiListControl, base);
THANDOR_SLOT_PREFIX(UiScrollableControl, base);
THANDOR_SLOT_PREFIX(UiRootNode, base);
THANDOR_SLOT_PREFIX(UiPanelControl, root);
THANDOR_SLOT_PREFIX(UiResizableWindowControl, root);
THANDOR_SLOT_PREFIX(UiTitledWindowControl, base);
THANDOR_SLOT_PREFIX(UiTextButtonControl, selectable);
THANDOR_SLOT_PREFIX(UiNumericPairTextButton, base);
THANDOR_SLOT_PREFIX(UiPayloadPairTextButton, base);
THANDOR_SLOT_PREFIX(UiSpriteButtonControl, selectable);
THANDOR_SLOT_PREFIX(UiImageControl, selectable);
THANDOR_SLOT_PREFIX(UiImageActionControl, base);
THANDOR_SLOT_PREFIX(UiConditionalActionControl, base);
THANDOR_SLOT_PREFIX(UiFramedTextButtonControl, selectable);
THANDOR_SLOT_PREFIX(UiWindowControl, selectable);
THANDOR_SLOT_PREFIX(UiRequiredTextEditControl, base);
THANDOR_SLOT_PREFIX(UiPathTextEditControl, base);
THANDOR_SLOT_PREFIX(UiSelectionGeometryControl, base);
THANDOR_SLOT_PREFIX(UiSingleLineTextControl, base);
THANDOR_SLOT_PREFIX(UiWrappedTextControl, base);
THANDOR_SLOT_PREFIX(UiRangeSliderControl, base);
THANDOR_SLOT_PREFIX(UiHorizontalGaugeControl, base);
THANDOR_SLOT_PREFIX(UiImagePanelControl, base);
THANDOR_SLOT_PREFIX(UiArmyMetricsPanel, base);
THANDOR_SLOT_PREFIX(UiFillPanelControl, base);
THANDOR_SLOT_PREFIX(UiNineSlicePanelControl, base);
THANDOR_SLOT_PREFIX(UiSoftwareTexturePreviewControl, base);
THANDOR_SLOT_PREFIX(UiFormattedContainer, base);
THANDOR_SLOT_PREFIX(UiFormattedContainerWithMarker, base);
#endif /* __cplusplus */

#endif /* THANDOR_UI_CONTROLS_NODE_VIEWS_H */

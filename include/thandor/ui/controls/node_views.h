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

/* UiNode_As<To>(node): the named cast between a UI node and the control it is, in either direction (step 13 X8;
   replaces the C-style casts of the control methods: a vtable method receives a UiNodeBase * and works on its
   control, and passes the control on as a UiNodeBase *). It compiles only when one type is registered as starting
   with the other (THANDOR_SLOT_PREFIX above and in ui/ingame, ui/frontend node_views.h, also over several
   registrations: UiImageControl -> UiSelectableControl -> UiNodeBase) or both start with the same registered
   base, so it cannot reach an unrelated type. The conversion is the reinterpret_cast the C-style cast was (the
   base is at offset 0: same address, no adjustment); const is kept, never dropped. */
template <class To, class From> inline To *UiNode_As(From *node)
{
    using ToBare = std::remove_cv_t<To>;
    using FromBare = std::remove_cv_t<From>;
    static_assert(thandor_slot_is_view_of<ToBare, FromBare>() || thandor_slot_is_view_of<FromBare, ToBare>() ||
                      (thandor_slot_is_view_of<ToBare, UiNodeBase>() && thandor_slot_is_view_of<FromBare, UiNodeBase>()),
                  "UiNode_As: the types are not registered views of one UI node");
    return reinterpret_cast<To *>(node);
}
/* The same for a node link (the load of the 32-bit field, then the cast). */
template <class To, class From> inline To *UiNode_As(const Ptr32<From> &link)
{
    return UiNode_As<To>(link.get());
}
#endif /* __cplusplus */

#endif /* THANDOR_UI_CONTROLS_NODE_VIEWS_H */

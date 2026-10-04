/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/include/thandor/ui/frontend/node_views.h
 */

#ifndef THANDOR_UI_FRONTEND_NODE_VIEWS_H
#define THANDOR_UI_FRONTEND_NODE_VIEWS_H

/* The frontend UI node types as prefixed views of their base node (core/slot.h THANDOR_SLOT_PREFIX),
   for the vtable and callback slots (see ui/controls/node_views.h). */

#include <thandor/core/slot.h>
#include <thandor/ui/frontend/types.h>
#include <thandor/ui/controls/node_views.h>

#ifdef __cplusplus
THANDOR_SLOT_PREFIX(FrontendPersistentSettingsPage, pageRoot);
THANDOR_SLOT_PREFIX(FrontendNetworkSetupPageState, rootNode);
THANDOR_SLOT_PREFIX(FrontendRootPageState, rootNode);
THANDOR_SLOT_PREFIX(FrontendModelPointerHitContext, base);
THANDOR_SLOT_PREFIX(UiSettingsValueControl, base);
THANDOR_SLOT_PREFIX(UiSelectableOptionRow60, control);
THANDOR_SLOT_PREFIX(UiSelectableOptionRow68, control);
THANDOR_SLOT_PREFIX(FrontendGraphicsRuntimeSettingsPageState, base);
THANDOR_SLOT_PREFIX(FrontendNetworkSettingsUiNodeView, base);
THANDOR_SLOT_PREFIX(FrontendPointerHintControl, base);
THANDOR_SLOT_PREFIX(FrontendPointerSceneRuntimeView, base);
THANDOR_SLOT_PREFIX(FrontendTaskAssignmentPageInitView, rootNode);
THANDOR_SLOT_PREFIX(FrontendResultsColumnSequenceControl, base);
THANDOR_SLOT_PREFIX(FrontendResultsEightColumnTemplate, base);
#endif /* __cplusplus */

#endif /* THANDOR_UI_FRONTEND_NODE_VIEWS_H */

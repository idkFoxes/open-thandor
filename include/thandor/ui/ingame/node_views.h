/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/include/thandor/ui/ingame/node_views.h
 */

#ifndef THANDOR_UI_INGAME_NODE_VIEWS_H
#define THANDOR_UI_INGAME_NODE_VIEWS_H

/* The in-game UI node types as prefixed views of their base node (core/slot.h THANDOR_SLOT_PREFIX),
   for the vtable and callback slots (see ui/controls/node_views.h). */

#include <thandor/core/slot.h>
#include <thandor/ui/ingame/types.h>
#include <thandor/ui/controls/node_views.h>

#ifdef __cplusplus
THANDOR_SLOT_PREFIX(InGamePersistentSettingsPage3508, settingsPageStack);
THANDOR_SLOT_PREFIX(InGameCommandTextEditControlCC, base);
THANDOR_SLOT_PREFIX(InGameCommandTextEntryPage2320, commandPageStack);
THANDOR_SLOT_PREFIX(InGameRuntimeRoot, rootUi);
THANDOR_SLOT_PREFIX(UiCommandSpriteButtonControl, sprite);
THANDOR_SLOT_PREFIX(UiCatalogEntryControl, command);
THANDOR_SLOT_PREFIX(InGameRuntimeRootUiGridView, rootUi);
THANDOR_SLOT_PREFIX(InGameRuntimeRootFrameView, rootUi);
THANDOR_SLOT_PREFIX(InGameMissionHelpTextPanel, scrollable);
THANDOR_SLOT_PREFIX(InGameMissionHelpRootView, rootUi.base);
THANDOR_SLOT_PREFIX(InGameTargetingRootTraversalView, base);
/* The world view node: interaction.reserved00_47 covers its UiNodeBase up to nodeFlags. */
THANDOR_SLOT_OVERLAY(WorldRuntimeContext, UiNodeBase);
/* The world view with the pointer and press coordinates of its interaction state named (map editor, drag
   selection bounds). */
THANDOR_SLOT_OVERLAY(WorldRuntimeExtendedMapControlView, WorldRuntimeContext);
/* The 16-byte head of an army runtime slot (GameEntityRuntime.common.ownership: model runtime, model node, link,
   faction) as the hierarchy metric and selection panel functions read it; ModelView_Cast reaches it from an
   army slot or a GameEntityRuntime. */
THANDOR_SLOT_OVERLAY(RuntimeModelFactionPrefix, ArmyRuntimeSlot);
/* The build catalog's view of a registered army asset record (g_ArmyAssetRecordRegistry): the record prefix,
   then the flags and the catalog texture source; ModelView_Cast converts between the two. */
THANDOR_SLOT_OVERLAY(UiCommandRuntimeRecordPrefix, ArmyAssetRecordPrefix);

#endif /* __cplusplus */

#endif /* THANDOR_UI_INGAME_NODE_VIEWS_H */

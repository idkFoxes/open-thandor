/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/src/ui/ingame/build_catalog.cpp
 * Reverse engineering by idkFoxes 2026
 */

#include <thandor/ui/ingame/build_catalog.h>
#include <thandor/thandor.h>

/* Implementation ownership: ui/ingame/build_catalog. */

/* Build catalog entry click (action 0x100B, g_InGameUiActionHandlersPage10[11]): finds the entry among the 48
   build catalog slots of the current column layout and queues its army asset for the active faction, or with Ctrl
   (activationInputState & KEYBOARD_STATE_CTRL) cancels a queued one with refund. Ignored while paused or while the
   world input is disabled.
*/
void InGameBuildCatalog_QueueOrCancelEntry(UiCatalogEntryControl *source)

{
  FactionRuntimeIndex factionIndex;
  UiCatalogEntryControl *root;
  PckArmyAssetIdCatalog assetId;
  int entryIndex;

  if ((g_UiCommandRuntimeFlags &
      (UI_COMMAND_RUNTIME_FLAG_PAUSED | UI_COMMAND_RUNTIME_FLAG_WORLD_INPUT_DISABLED)) == 0) {
    root = source;
    while ((root->command).sprite.selectable.base.parent != UI_NODE_NONE) {
      root = (UiCatalogEntryControl *)(root->command).sprite.selectable.base.parent;
    }
    entryIndex = BUILD_CATALOG_ENTRY_COUNT - 1;
    while ((int)((uintptr_t)source - (uintptr_t)root) !=
           g_UiCatalogGroup48OffsetTables[g_UiCatalogGroup48ColumnCount][entryIndex]) {
      entryIndex--;
      if (entryIndex < 0) {
        return;
      }
    }
    if (((source->command).activationInputState & UI_COMMAND_ACTIVATION_RELATION_RESET_REQUEST_MASK)
        == 0) {
      factionIndex = ((WorldRuntimeContext *)INGAME_UI(root,worldView))->activeFactionRuntimeIndex;
      assetId = g_UiCatalogGroup48Records[entryIndex]->armyAssetId;
      if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_NETWORKED_MASK) ==
          SESSION_NETWORK_ROLE_LOCAL) {
        GameFactionRuntime_RegisterArmyAssetPointers
                  (g_LocalPlayerRuntimeId,1,assetId,factionIndex);
      }
      else {
        InGameCommandQueue_AppendLocalPlayerCommand
                  (INGAME_COMMAND_QUEUE_ARMY,1,assetId,(CommandPayload)factionIndex);
      }
    }
    else {
      factionIndex = ((WorldRuntimeContext *)INGAME_UI(root,worldView))->activeFactionRuntimeIndex;
      assetId = g_UiCatalogGroup48Records[entryIndex]->armyAssetId;
      if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_NETWORKED_MASK) ==
          SESSION_NETWORK_ROLE_LOCAL) {
        GameFactionRuntime_CancelQueuedArmyAssetsAndRefund
                  (g_LocalPlayerRuntimeId,1,assetId,factionIndex);
      }
      else {
        InGameCommandQueue_AppendLocalPlayerCommand
                  (INGAME_COMMAND_CANCEL_QUEUED_ARMY,1,assetId,(CommandPayload)factionIndex);
      }
    }
  }
  return;
}

/* Special build catalog entry click (action 0x100C, g_InGameUiActionHandlersPage10[12]): the same as
   InGameBuildCatalog_QueueOrCancelEntry for the 42 slots of the special build catalog.
*/
void InGameSpecialBuildCatalog_QueueOrCancelEntry(UiCatalogEntryControl *source)

{
  FactionRuntimeIndex factionIndex;
  UiCatalogEntryControl *root;
  PckArmyAssetIdCatalog assetId;
  int entryIndex;

  if ((g_UiCommandRuntimeFlags &
      (UI_COMMAND_RUNTIME_FLAG_PAUSED | UI_COMMAND_RUNTIME_FLAG_WORLD_INPUT_DISABLED)) == 0) {
    root = source;
    while ((root->command).sprite.selectable.base.parent != UI_NODE_NONE) {
      root = (UiCatalogEntryControl *)(root->command).sprite.selectable.base.parent;
    }
    entryIndex = SPECIAL_BUILD_CATALOG_ENTRY_COUNT - 1;
    while ((int)((uintptr_t)source - (uintptr_t)root) !=
           g_UiCatalogGroup42OffsetTables[g_UiCatalogGroup42ColumnCount][entryIndex]) {
      entryIndex--;
      if (entryIndex < 0) {
        return;
      }
    }
    if (((source->command).activationInputState & UI_COMMAND_ACTIVATION_RELATION_RESET_REQUEST_MASK)
        == 0) {
      factionIndex = ((WorldRuntimeContext *)INGAME_UI(root,worldView))->activeFactionRuntimeIndex;
      assetId = g_UiCatalogGroup42Records[entryIndex]->armyAssetId;
      if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_NETWORKED_MASK) ==
          SESSION_NETWORK_ROLE_LOCAL) {
        GameFactionRuntime_RegisterArmyAssetPointers
                  (g_LocalPlayerRuntimeId,1,assetId,factionIndex);
      }
      else {
        InGameCommandQueue_AppendLocalPlayerCommand
                  (INGAME_COMMAND_QUEUE_ARMY,1,assetId,(CommandPayload)factionIndex);
      }
    }
    else {
      factionIndex = ((WorldRuntimeContext *)INGAME_UI(root,worldView))->activeFactionRuntimeIndex;
      assetId = g_UiCatalogGroup42Records[entryIndex]->armyAssetId;
      if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_NETWORKED_MASK) ==
          SESSION_NETWORK_ROLE_LOCAL) {
        GameFactionRuntime_CancelQueuedArmyAssetsAndRefund
                  (g_LocalPlayerRuntimeId,1,assetId,factionIndex);
      }
      else {
        InGameCommandQueue_AppendLocalPlayerCommand
                  (INGAME_COMMAND_CANCEL_QUEUED_ARMY,1,assetId,(CommandPayload)factionIndex);
      }
    }
  }
  return;
}

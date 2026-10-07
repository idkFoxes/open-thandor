/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/src/ui/ingame/build_catalog.cpp
 * Reverse engineering by idkFoxes 2026
 */

#include <thandor/ui/ingame/build_catalog.h>
#include <thandor/thandor.h>
#include <thandor/core/bytes.h>

/* Module data. */

UiCommandRuntimeRecordPrefix *g_UiCatalogGroup48Records[48] = {};

UiCommandRuntimeRecordPrefix *g_UiCatalogGroup42Records[42] = {};

/* Build catalog entry click (action 0x100B, g_InGameUiActionHandlersPage10[11]): finds the entry among the 48
   build catalog slots of the current column layout and queues its army asset for the active faction, or with Ctrl
   (activationInputState & KEYBOARD_STATE_CTRL) cancels a queued one with refund. Ignored while paused or while the
   world input is disabled.
*/
void InGameBuildCatalog_QueueOrCancelEntry(UiCatalogEntryControl *source)

{
  FactionRuntimeIndex factionIndex;
  UiCatalogEntryControl *root;
  WorldRuntimeContext *worldRuntime;
  PckArmyAssetIdCatalog assetId;
  int entryIndex;

  if (!Any(g_UiCommandRuntimeFlags &
      (UI_COMMAND_RUNTIME_FLAG_PAUSED | UI_COMMAND_RUNTIME_FLAG_WORLD_INPUT_DISABLED))) {
    root = source;
    while ((root->command).sprite.selectable.base.parent != UI_NODE_NONE) {
      root = reinterpret_cast<UiCatalogEntryControl *>((root->command).sprite.selectable.base.parent.get());
    }
    /* the world view node is also the world runtime (InGameRuntimeRoot.worldRuntime, +0xA30) */
    worldRuntime = FrontendModelPointerContext_AsWorldRuntime(&InGameUi_Image(root)->worldView);
    entryIndex = BUILD_CATALOG_ENTRY_COUNT - 1;
    while ((int)((uintptr_t)source - (uintptr_t)root) !=
           g_UiCatalogGroup48OffsetTables[g_UiCatalogGroup48ColumnCount][entryIndex]) {
      entryIndex--;
      if (entryIndex < 0) {
        return;
      }
    }
    if (!Any((source->command).activationInputState & UI_COMMAND_ACTIVATION_RELATION_RESET_REQUEST_MASK)) {
      factionIndex = worldRuntime->activeFactionRuntimeIndex;
      assetId = g_UiCatalogGroup48Records[entryIndex]->armyAssetId;
      InGameCommand_Issue<GameFactionRuntime_RegisterArmyAssetPointers>(1,assetId,factionIndex);
    }
    else {
      factionIndex = worldRuntime->activeFactionRuntimeIndex;
      assetId = g_UiCatalogGroup48Records[entryIndex]->armyAssetId;
      InGameCommand_Issue<GameFactionRuntime_CancelQueuedArmyAssetsAndRefund>(1,assetId,factionIndex);
    }
  }
}

/* Special build catalog entry click (action 0x100C, g_InGameUiActionHandlersPage10[12]): the same as
   InGameBuildCatalog_QueueOrCancelEntry for the 42 slots of the special build catalog.
*/
void InGameSpecialBuildCatalog_QueueOrCancelEntry(UiCatalogEntryControl *source)

{
  FactionRuntimeIndex factionIndex;
  UiCatalogEntryControl *root;
  WorldRuntimeContext *worldRuntime;
  PckArmyAssetIdCatalog assetId;
  int entryIndex;

  if (!Any(g_UiCommandRuntimeFlags &
      (UI_COMMAND_RUNTIME_FLAG_PAUSED | UI_COMMAND_RUNTIME_FLAG_WORLD_INPUT_DISABLED))) {
    root = source;
    while ((root->command).sprite.selectable.base.parent != UI_NODE_NONE) {
      root = reinterpret_cast<UiCatalogEntryControl *>((root->command).sprite.selectable.base.parent.get());
    }
    /* the world view node is also the world runtime (InGameRuntimeRoot.worldRuntime, +0xA30) */
    worldRuntime = FrontendModelPointerContext_AsWorldRuntime(&InGameUi_Image(root)->worldView);
    entryIndex = SPECIAL_BUILD_CATALOG_ENTRY_COUNT - 1;
    while ((int)((uintptr_t)source - (uintptr_t)root) !=
           g_UiCatalogGroup42OffsetTables[g_UiCatalogGroup42ColumnCount][entryIndex]) {
      entryIndex--;
      if (entryIndex < 0) {
        return;
      }
    }
    if (!Any((source->command).activationInputState & UI_COMMAND_ACTIVATION_RELATION_RESET_REQUEST_MASK)) {
      factionIndex = worldRuntime->activeFactionRuntimeIndex;
      assetId = g_UiCatalogGroup42Records[entryIndex]->armyAssetId;
      InGameCommand_Issue<GameFactionRuntime_RegisterArmyAssetPointers>(1,assetId,factionIndex);
    }
    else {
      factionIndex = worldRuntime->activeFactionRuntimeIndex;
      assetId = g_UiCatalogGroup42Records[entryIndex]->armyAssetId;
      InGameCommand_Issue<GameFactionRuntime_CancelQueuedArmyAssetsAndRefund>(1,assetId,factionIndex);
    }
  }
}

/* Fills the slotCount catalog slot controls (UiCatalogEntryControl at root + slotOffsets[i]): the first itemCount
   show records[i] with its texture and Xenite cost, the rest are hidden and cleared. */
static void BuildCatalog_FillSlots(InGameRuntimeRootUiGridView *inGameUiGridView,const int32_t *slotOffsets,
                                   UiCommandRuntimeRecordPrefix **records,uint32_t slotCount,uint32_t itemCount)

{
  UiCatalogEntryControl *slotControl;
  GraphicsTextureSourceAsset *textureAsset;
  ArmyBuildXeniteCostQ4 xeniteCost;
  uint32_t slotIndex;

  for (slotIndex = 0; slotIndex < slotCount; slotIndex++) {
    slotControl = Thandor_At<UiCatalogEntryControl>(inGameUiGridView,slotOffsets[slotIndex]);
    if (slotIndex < itemCount) {
      slotControl->command.sprite.selectable.base.nodeFlags &= ~UI_NODE_SUPPRESSED;
      xeniteCost = records[slotIndex]->buildXeniteCostQ4;
      textureAsset = records[slotIndex]->textureSource;
    }
    else {
      slotControl->command.sprite.selectable.base.nodeFlags |= UI_NODE_SUPPRESSED;
      xeniteCost = 0;
      textureAsset = nullptr;
    }
    slotControl->command.sprite.primaryTextureSource = textureAsset;
    slotControl->runtimeDisplayValueQ4 = xeniteCost;
  }
}

/* Rebuilds the build catalog (48 entries): the production capabilities come from the selected buildings, or,
   with none selected, from all own models (class 22 adds capability 8, class 13 its definition's flags in
   classParameterC4). Every registered army asset with flag 1, a texture and a matching capability that passes
   the technology and ownership/unlock tests (the results of ModelDefinitionHierarchy_AllTechnologyUnlockedForFaction,
   FactionRuntime_IsArmyAssetNotPending, ArmyAssetRecord_HasFactionUnlockedLinkedDefinition) gets a
   slot, in a grid of at most eight columns with its texture and Xenite cost; the frame is sized to the grid
   (smaller margins below 800 pixels width) and the panel hidden when the catalog is empty.
*/
void InGameBuildCatalog_RebuildGrid(UiNodeBase *node)

{
  WorldOwnerListNode *ownerNode;
  ModelRuntimeSlot *modelRuntime;
  UiCommandRuntimeRecordPrefix *catalogRecord;
  InGameRuntimeRootUiGridView *inGameUiGridView;
  uint32_t capabilityFlags;
  uint32_t columnCount;
  ModelDefinition *definition;
  int registryIndex;
  int panelWidth;
  uint32_t itemCount;
  int factionIndex;
  int panelHeight;
  UiGridDimensions gridDimensions;

  inGameUiGridView = UiNode_As<InGameRuntimeRootUiGridView>(UiNode_GetRoot(node));
  /* the local (active) faction */
  factionIndex = (inGameUiGridView->worldRuntime).activeFactionRuntimeIndex;
  capabilityFlags = SelectionInfo_CollectCapabilityFlags();
  if (capabilityFlags == 0) {
    for (ownerNode = (inGameUiGridView->worldRuntime).ownerListHead; ownerNode != nullptr;
         ownerNode = ownerNode->nextNode) {
      if (ownerNode->ownerClassId != WORLD_OWNER_RUNTIME_MODEL) {
        continue;
      }
      modelRuntime = WorldOwnerNode_ModelRuntime(ownerNode);
      definition = (modelRuntime->definitionOrSavedId).runtimeDefinition;
      if ((modelRuntime->ownerArmyRuntimeOrSavedOffset).armyRuntime->factionIndex != factionIndex) {
        continue;
      }
      if (definition->runtimeClassId == MODEL_RUNTIME_CLASS_22) {
        capabilityFlags = capabilityFlags | BUILD_CATALOG_ASSET_FLAG_CAPABILITY_8;
      }
      else if (definition->runtimeClassId == MODEL_RUNTIME_CLASS_13) {
        capabilityFlags = capabilityFlags | definition->classParameterC4;
      }
    }
  }
  memset(g_UiCatalogGroup48Records,0,sizeof(g_UiCatalogGroup48Records));
  itemCount = 0;
  for (registryIndex = 0; registryIndex < ARMY_ASSET_REGISTRY_SLOT_COUNT; registryIndex++) {
    catalogRecord = ModelView_Cast<UiCommandRuntimeRecordPrefix>(g_ArmyAssetRecordRegistry[registryIndex]);
    if (catalogRecord != nullptr &&
        (catalogRecord->assetFlags14 & BUILD_CATALOG_ASSET_FLAG_BUILDABLE) != 0 &&
        !ModelDefinitionHierarchy_AllTechnologyUnlockedForFaction
             (factionIndex,(ModelDefinitionHierarchyNodeAddress32)catalogRecord) &&
        (catalogRecord->assetFlags14 & BUILD_CATALOG_ASSET_CAPABILITY_MASK) != 0 &&
        catalogRecord->textureSource != nullptr &&
        itemCount < BUILD_CATALOG_ENTRY_COUNT &&
        (catalogRecord->assetFlags14 & capabilityFlags) != 0 &&
        (!FactionRuntime_IsArmyAssetNotPending(factionIndex,ModelView_Cast<ArmyAssetRecordPrefix>(catalogRecord)) ||
         !ArmyAssetRecord_HasFactionUnlockedLinkedDefinition
              (factionIndex,capabilityFlags,ModelView_Cast<ArmyAssetRecordPrefix>(catalogRecord)))) {
      g_UiCatalogGroup48Records[itemCount] = catalogRecord;
      itemCount++;
    }
  }
  gridDimensions = UiGrid_ComputeDimensionsPacked(6,itemCount);
  columnCount = gridDimensions.columnCount;
  if (BUILD_CATALOG_MAX_COLUMNS < columnCount) {
    columnCount = BUILD_CATALOG_MAX_COLUMNS;
  }
  panelWidth = columnCount * g_InGamePanelTextureSubresource34Width + g_InGamePanelTextureSubresource27Width +
          g_InGamePanelTextureSubresource28Width;
  panelHeight = (int)gridDimensions.rowCount * g_InGamePanelTextureSubresource34Height +
          g_InGamePanelTextureSubresource26Height + g_InGamePanelTextureSubresource31Height;
  g_UiCatalogGroup48ColumnCount = columnCount;
  if ((int)g_FramebufferWidth < 800) {
    (inGameUiGridView->buildCatalogFrame).leftOffset = -31;
    (inGameUiGridView->buildCatalogFrame).rightOffset = -31;
    (inGameUiGridView->buildCatalogFrame).topOffset = -71;
    (inGameUiGridView->buildCatalogFrame).bottomOffset = -71;
  }
  else {
    (inGameUiGridView->buildCatalogFrame).leftOffset = -39;
    (inGameUiGridView->buildCatalogFrame).rightOffset = -39;
    (inGameUiGridView->buildCatalogFrame).topOffset = -89;
    (inGameUiGridView->buildCatalogFrame).bottomOffset = -89;
  }
  (inGameUiGridView->buildCatalogFrame).leftOffset -= panelWidth;
  (inGameUiGridView->buildCatalogFrame).topOffset -= panelHeight;
  if (itemCount == 0) {
    (inGameUiGridView->buildCatalogPanel).nodeFlags |= UI_NODE_SUPPRESSED;
  }
  else {
    (inGameUiGridView->buildCatalogPanel).nodeFlags &= ~UI_NODE_SUPPRESSED;
  }
  BuildCatalog_FillSlots(inGameUiGridView,g_UiCatalogGroup48OffsetTables[columnCount],g_UiCatalogGroup48Records,
                         BUILD_CATALOG_ENTRY_COUNT,itemCount);
  (*((inGameUiGridView->buildCatalogPanel).vtable)->layout)
            (&inGameUiGridView->buildCatalogPanel);
}

/* Rebuilds the special build catalog (42 entries), offered only while the active faction owns a model of
   runtime class 11: every registered army asset with flags 1 and 0x10 and a texture that passes the technology
   and ownership/unlock tests (as in InGameBuildCatalog_RebuildGrid) gets a slot, in a grid of at most
   six columns with its texture and Xenite cost. The frame is sized to the grid (smaller margins below 800
   pixels width); an empty catalog hides its panel, and also the army stock panel when the stock is empty.
*/
void InGameSpecialBuildCatalog_RebuildGrid(UiNodeBase *node)

{
  WorldOwnerListNode *ownerNode;
  ModelRuntimeSlot *modelRuntime;
  UiCommandRuntimeRecordPrefix *catalogRecord;
  InGameRuntimeRootUiGridView *inGameUiGridView;
  uint32_t columnCount;
  int factionIndex;
  int panelWidth;
  int registryIndex;
  uint32_t itemCount;
  int structureCount;
  int panelHeight;
  UiGridDimensions gridDimensions;

  inGameUiGridView = UiNode_As<InGameRuntimeRootUiGridView>(UiNode_GetRoot(node));
  factionIndex = (inGameUiGridView->worldRuntime).activeFactionRuntimeIndex;
  /* count the faction's class-11 models */
  structureCount = 0;
  for (ownerNode = (inGameUiGridView->worldRuntime).ownerListHead;
      ownerNode != nullptr; ownerNode = ownerNode->nextNode) {
    if (ownerNode->ownerClassId != WORLD_OWNER_RUNTIME_MODEL) {
      continue;
    }
    modelRuntime = WorldOwnerNode_ModelRuntime(ownerNode);
    if ((modelRuntime->definitionOrSavedId).runtimeDefinition->runtimeClassId == MODEL_RUNTIME_CLASS_11 &&
        (modelRuntime->ownerArmyRuntimeOrSavedOffset).armyRuntime->factionIndex == factionIndex) {
      structureCount++;
    }
  }
  memset(g_UiCatalogGroup42Records,0,sizeof(g_UiCatalogGroup42Records));
  itemCount = 0;
  for (registryIndex = 0; registryIndex < ARMY_ASSET_REGISTRY_SLOT_COUNT; registryIndex++) {
    catalogRecord = ModelView_Cast<UiCommandRuntimeRecordPrefix>(g_ArmyAssetRecordRegistry[registryIndex]);
    if (catalogRecord != nullptr &&
        (catalogRecord->assetFlags14 & BUILD_CATALOG_ASSET_FLAG_BUILDABLE) != 0 &&
        !ModelDefinitionHierarchy_AllTechnologyUnlockedForFaction
             (factionIndex,(ModelDefinitionHierarchyNodeAddress32)catalogRecord) &&
        (catalogRecord->assetFlags14 & BUILD_CATALOG_ASSET_FLAG_SPECIAL) != 0 &&
        catalogRecord->textureSource != nullptr &&
        itemCount < SPECIAL_BUILD_CATALOG_ENTRY_COUNT &&
        structureCount != 0 &&
        (!FactionRuntime_IsArmyAssetNotPending(factionIndex,ModelView_Cast<ArmyAssetRecordPrefix>(catalogRecord)) ||
         !ArmyAssetRecord_HasFactionUnlockedLinkedDefinition
              (factionIndex,BUILD_CATALOG_ASSET_FLAG_SPECIAL,ModelView_Cast<ArmyAssetRecordPrefix>(catalogRecord)))) {
      g_UiCatalogGroup42Records[itemCount] = catalogRecord;
      itemCount++;
    }
  }
  gridDimensions = UiGrid_ComputeDimensionsPacked(7,itemCount);
  columnCount = gridDimensions.columnCount;
  if (SPECIAL_BUILD_CATALOG_MAX_COLUMNS < columnCount) {
    columnCount = SPECIAL_BUILD_CATALOG_MAX_COLUMNS;
  }
  panelWidth = columnCount * g_InGamePanelTextureSubresource34Width + g_InGamePanelTextureSubresource27Width +
          g_InGamePanelTextureSubresource28Width;
  panelHeight = (int)gridDimensions.rowCount * g_InGamePanelTextureSubresource34Height +
           g_InGamePanelTextureSubresource26Height + g_InGamePanelTextureSubresource31Height;
  g_UiCatalogGroup42ColumnCount = columnCount;
  if ((int)g_FramebufferWidth < 800) {
    (inGameUiGridView->specialBuildCatalogFrame).leftOffset = -31;
    (inGameUiGridView->specialBuildCatalogFrame).rightOffset = -31;
    (inGameUiGridView->specialBuildCatalogFrame).topOffset = -42;
    (inGameUiGridView->specialBuildCatalogFrame).bottomOffset = -42;
  }
  else {
    (inGameUiGridView->specialBuildCatalogFrame).leftOffset = -39;
    (inGameUiGridView->specialBuildCatalogFrame).rightOffset = -39;
    (inGameUiGridView->specialBuildCatalogFrame).topOffset = -55;
    (inGameUiGridView->specialBuildCatalogFrame).bottomOffset = -55;
  }
  (inGameUiGridView->specialBuildCatalogFrame).leftOffset -= panelWidth;
  (inGameUiGridView->specialBuildCatalogFrame).topOffset -= panelHeight;
  if (itemCount == 0) {
    (inGameUiGridView->specialBuildCatalogPanel).nodeFlags |= UI_NODE_SUPPRESSED;
    if (g_GameFactionRuntimeImage.records[(inGameUiGridView->worldRuntime).activeFactionRuntimeIndex].
          primaryArmyAssetCount == 0) {
      (inGameUiGridView->armyStockPanel).nodeFlags |= UI_NODE_SUPPRESSED;
    }
    else {
      (inGameUiGridView->armyStockPanel).nodeFlags &= ~UI_NODE_SUPPRESSED;
    }
  }
  else {
    (inGameUiGridView->specialBuildCatalogPanel).nodeFlags &= ~UI_NODE_SUPPRESSED;
    (inGameUiGridView->armyStockPanel).nodeFlags &= ~UI_NODE_SUPPRESSED;
  }
  BuildCatalog_FillSlots(inGameUiGridView,g_UiCatalogGroup42OffsetTables[columnCount],g_UiCatalogGroup42Records,
                         SPECIAL_BUILD_CATALOG_ENTRY_COUNT,itemCount);
  (*((inGameUiGridView->specialBuildCatalogPanel).vtable)->layout)
            (&inGameUiGridView->specialBuildCatalogPanel);
}

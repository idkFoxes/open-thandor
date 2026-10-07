/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/src/ui/ingame/army_stock.cpp
 * Reverse engineering by idkFoxes 2026
 */

#include <thandor/ui/ingame/army_stock.h>
#include <thandor/thandor.h>
#include <thandor/core/bytes.h>

/* Module data. */

/* Byte offsets of the 24 army stock slots (armyStockSlot00..23, UiCommandSpriteButtonControl; slot 23 is typed
   UiCatalogEntryControl, whose command part is the same control) in the in-game UI image. */
static int32_t g_UiCommandSpriteVariantAOffsets[24] = {
    /*  0 */ static_cast<int32_t>(offsetof(InGameUiImage,armyStockSlot00)),
             static_cast<int32_t>(offsetof(InGameUiImage,armyStockSlot01)),
             static_cast<int32_t>(offsetof(InGameUiImage,armyStockSlot02)),
             static_cast<int32_t>(offsetof(InGameUiImage,armyStockSlot03)),
             static_cast<int32_t>(offsetof(InGameUiImage,armyStockSlot04)),
             static_cast<int32_t>(offsetof(InGameUiImage,armyStockSlot05)),
             static_cast<int32_t>(offsetof(InGameUiImage,armyStockSlot06)),
             static_cast<int32_t>(offsetof(InGameUiImage,armyStockSlot07)),
    /*  8 */ static_cast<int32_t>(offsetof(InGameUiImage,armyStockSlot08)),
             static_cast<int32_t>(offsetof(InGameUiImage,armyStockSlot09)),
             static_cast<int32_t>(offsetof(InGameUiImage,armyStockSlot10)),
             static_cast<int32_t>(offsetof(InGameUiImage,armyStockSlot11)),
             static_cast<int32_t>(offsetof(InGameUiImage,armyStockSlot12)),
             static_cast<int32_t>(offsetof(InGameUiImage,armyStockSlot13)),
             static_cast<int32_t>(offsetof(InGameUiImage,armyStockSlot14)),
             static_cast<int32_t>(offsetof(InGameUiImage,armyStockSlot15)),
    /* 16 */ static_cast<int32_t>(offsetof(InGameUiImage,armyStockSlot16)),
             static_cast<int32_t>(offsetof(InGameUiImage,armyStockSlot17)),
             static_cast<int32_t>(offsetof(InGameUiImage,armyStockSlot18)),
             static_cast<int32_t>(offsetof(InGameUiImage,armyStockSlot19)),
             static_cast<int32_t>(offsetof(InGameUiImage,armyStockSlot20)),
             static_cast<int32_t>(offsetof(InGameUiImage,armyStockSlot21)),
             static_cast<int32_t>(offsetof(InGameUiImage,armyStockSlot22)),
             static_cast<int32_t>(offsetof(InGameUiImage,armyStockSlot23))};

static uint32_t g_UiCommandSpriteVariantAColumnCount = 0;

static int32_t *g_UiCommandSpriteVariantAOffsetTables[5] = {
    /* 0 */ THANDOR_PTR(&g_UiCommandSpriteVariantAOffsets),
    /* 1 */ THANDOR_PTR(&g_UiCommandSpriteVariantAOffsets),
    /* 2 */ THANDOR_PTR(&g_UiCommandSpriteVariantAOffsets),
    /* 3 */ THANDOR_PTR(&g_UiCommandSpriteVariantAOffsets),
    /* 4 */ THANDOR_PTR(&g_UiCommandSpriteVariantAOffsets)};

static UiCommandRuntimeRecordPrefix *g_UiCommandSpriteVariantARecords[24] = {};

/* Pointer move over an army stock slot (pointerMove of g_UiCommandSpriteButtonWithDetailsVtable, which the seven diplomacy
   relation buttons share; for them no slot matches and only the cursor frame is returned): shows the slot's
   army asset in the selection detail panel and returns the cursor frame, 12 while Ctrl is held (a click then
   sells the army, see InGameArmyStock_TakeOrSellSlotArmy), 10 otherwise.
*/
GraphicsCursorFrameIndex InGameArmyStock_PointerMoveShowSlotDetails(UiPixelCoordinate pointerY,UiPixelCoordinate pointerX,
          UiCommandSpriteButtonControl *control)

{
  GraphicsCursorFrameIndex cursorFrame;
  int recordIndex;

  if (((control->sprite).selectable.base.nodeFlags & UI_NODE_SUPPRESSED) == 0) {
    recordIndex = ARMY_STOCK_ENTRY_COUNT - 1;
    while (-1 < recordIndex) {
      if ((int)((uintptr_t)control - (uintptr_t)g_InGameRuntimeRoot) ==
          g_UiCommandSpriteVariantAOffsetTables[g_UiCommandSpriteVariantAColumnCount][recordIndex])
      {
        g_UiHoverSelectionRecord = g_UiCommandSpriteVariantARecords[recordIndex];
        InGameSelectionDetailPanel_Rebuild();
        break;
      }
      recordIndex--;
    }
  }
  cursorFrame = INGAME_CURSOR_FRAME_ARMY_STOCK;
  if ((g_KeyboardStateMask & KEYBOARD_STATE_CTRL) != 0) {
    cursorFrame = INGAME_CURSOR_FRAME_ARMY_STOCK_SELL;
  }
  return cursorFrame;
}

/* Rebuilds the army stock panel: the active faction's pooled army assets that have a texture (at most 24,
   none while the world input is disabled) fill g_UiCommandSpriteVariantARecords and the slot buttons in a
   grid of at most four columns; the frame is sized to the grid (smaller margins below 800 pixels width) and
   hidden when the stock is empty, unused slots are hidden.
*/
void InGameArmyStock_RebuildGrid(UiNodeBase *node)

{
  int32_t *offsetTable;
  uint32_t columnCount;
  GraphicsTextureSourceAsset *slotTexture;
  int remainingSlots;
  int panelWidth;
  int slotOffset;
  UiCommandSpriteButtonControl *slotControl;
  uint32_t itemCount;
  FactionArmyAssetCount remainingAssets;
  int panelHeight;
  uint32_t slotIndex;
  UiCommandRuntimeRecordPrefix **recordCursor;
  uint32_t *assetCursor;
  UiGridDimensions gridDimensions;
  InGameUiImage *ui;
  WorldRuntimeContext *worldRuntime;

  /* node becomes the in-game UI root */
  while (node->parent != UI_NODE_NONE) {
    node = node->parent;
  }
  ui = InGameUi_Image(node);
  /* the world view node is also the world runtime (InGameRuntimeRoot.worldRuntime, +0xA30) */
  worldRuntime = FrontendModelPointerContext_AsWorldRuntime(&ui->worldView);
  recordCursor = g_UiCommandSpriteVariantARecords;
  for (remainingSlots = ARMY_STOCK_ENTRY_COUNT; remainingSlots != 0; remainingSlots--) {
    *recordCursor = nullptr;
    recordCursor++;
  }
  recordCursor = g_UiCommandSpriteVariantARecords;
  remainingAssets = g_GameFactionRuntimeImage.records[worldRuntime->activeFactionRuntimeIndex].primaryArmyAssetCount;
  itemCount = 0;
  assetCursor = g_GameFactionRuntimeImage.records[worldRuntime->activeFactionRuntimeIndex].primaryArmyAssetPointersOrIds;
  if ((g_UiCommandRuntimeFlags & UI_COMMAND_RUNTIME_FLAG_WORLD_INPUT_DISABLED) == 0) {
    for (; remainingAssets != 0; remainingAssets--) {
      if ((Thandor_U32ToPointer<UiCommandRuntimeRecordPrefix>(*assetCursor)->textureSource != nullptr) && (itemCount < ARMY_STOCK_ENTRY_COUNT)) { /* 5f-format: GameFactionRuntimeRecord.primaryArmyAssetPointersOrIds */
        *recordCursor = Thandor_U32ToPointer<UiCommandRuntimeRecordPrefix>(*assetCursor); /* 5f-format: GameFactionRuntimeRecord.primaryArmyAssetPointersOrIds */
        itemCount++;
        recordCursor++;
      }
      assetCursor++;
    }
  }
  gridDimensions = UiGrid_ComputeDimensionsPacked(6,itemCount);
  columnCount = gridDimensions.columnCount;
  if (ARMY_STOCK_MAX_COLUMNS < columnCount) {
    columnCount = ARMY_STOCK_MAX_COLUMNS;
  }
  panelWidth = columnCount * g_InGamePanelTextureSubresource34Width + g_InGamePanelTextureSubresource27Width +
          g_InGamePanelTextureSubresource28Width;
  panelHeight = (int)gridDimensions.rowCount * g_InGamePanelTextureSubresource34Height +
          g_InGamePanelTextureSubresource26Height + g_InGamePanelTextureSubresource31Height;
  g_UiCommandSpriteVariantAColumnCount = columnCount;
  if ((int)g_FramebufferWidth < 800) {
    ui->armyStockFrame.base.leftOffset = -31;
    ui->armyStockFrame.base.rightOffset = -31;
    ui->armyStockFrame.base.topOffset = -13;
    ui->armyStockFrame.base.bottomOffset = -13;
  }
  else {
    ui->armyStockFrame.base.leftOffset = -39;
    ui->armyStockFrame.base.rightOffset = -39;
    ui->armyStockFrame.base.topOffset = -18;
    ui->armyStockFrame.base.bottomOffset = -18;
  }
  ui->armyStockFrame.base.leftOffset -= panelWidth;
  ui->armyStockFrame.base.topOffset -= panelHeight;
  if (itemCount == 0) {
    ui->armyStockFrame.base.nodeFlags |= UI_NODE_SUPPRESSED;
  }
  else {
    ui->armyStockFrame.base.nodeFlags &= ~UI_NODE_SUPPRESSED;
  }
  offsetTable = g_UiCommandSpriteVariantAOffsetTables[columnCount];
  for (slotIndex = 0; slotIndex < ARMY_STOCK_ENTRY_COUNT; slotIndex++) {
    slotOffset = offsetTable[slotIndex];
    slotControl = Thandor_At<UiCommandSpriteButtonControl>(ui,slotOffset);
    if (slotIndex < itemCount) {
      slotControl->sprite.selectable.base.nodeFlags &= ~UI_NODE_SUPPRESSED;
      slotTexture = g_UiCommandSpriteVariantARecords[slotIndex]->textureSource;
    }
    else {
      slotControl->sprite.selectable.base.nodeFlags |= UI_NODE_SUPPRESSED;
      slotTexture = nullptr;
    }
    slotControl->sprite.primaryTextureSource = slotTexture;
  }
  ui->armyStockPanel.selectable.base.vtable->layout(&ui->armyStockPanel.selectable.base);
}

/* Army stock slot click (action 0x1001, g_InGameUiActionHandlersPage10[1]): first drops any army still waiting
   for placement (command 0x14F0), then takes the slot's army for placement on the map, or sells it with Ctrl
   (activationInputState & KEYBOARD_STATE_CTRL). Ignored while paused, while the world input is disabled and while
   world runtime flag 0x10 is set.
*/
void InGameArmyStock_TakeOrSellSlotArmy(UiCommandSpriteButtonControl *control)

{
  UiCommandSpriteButtonControl *root;
  InGameUiImage *ui;
  WorldRuntimeContext *worldRuntime;
  UiCommandRuntimeRecordPrefix *runtimeRecord;
  FactionRuntimeIndex factionIndex;
  PckArmyAssetIdCatalog assetId;
  int slotIndex;

  if ((g_UiCommandRuntimeFlags &
      (UI_COMMAND_RUNTIME_FLAG_PAUSED | UI_COMMAND_RUNTIME_FLAG_WORLD_INPUT_DISABLED)) == 0) {
    root = control;
    while ((root->sprite).selectable.base.parent != UI_NODE_NONE) {
      root = reinterpret_cast<UiCommandSpriteButtonControl *>((root->sprite).selectable.base.parent.get());
    }
    ui = InGameUi_Image(root);
    /* the world view node is also the world runtime (InGameRuntimeRoot.worldRuntime, +0xA30) */
    worldRuntime = FrontendModelPointerContext_AsWorldRuntime(&ui->worldView);
    /* end any hover of the stock panel (image control) */
    g_UiImageControlHoverTarget = nullptr;
    ui->armyStockPanel.selectable.stateFlags &= ~UI_IMAGE_CONTROL_HOVER_STATE_BITS;
    if ((worldRuntime->runtimeFlags & WORLD_RUNTIME_FLAG_NOTIFICATION_GOTO) == 0) {
      slotIndex = ARMY_STOCK_ENTRY_COUNT - 1;
      while ((int)((uintptr_t)control - (uintptr_t)root) !=
             g_UiCommandSpriteVariantAOffsetTables[g_UiCommandSpriteVariantAColumnCount][slotIndex]) {
        slotIndex--;
        if (slotIndex < 0) {
          return;
        }
      }
      runtimeRecord = g_UiCommandSpriteVariantARecords[slotIndex];
      factionIndex = worldRuntime->activeFactionRuntimeIndex;
      InGameCommand_Issue<GameFactionRuntime_ConsumePendingArmyAssetAndRefreshGrid>(0,0,factionIndex);
      if (!Any(control->activationInputState & UI_COMMAND_ACTIVATION_RELATION_RESET_REQUEST_MASK))
      {
        factionIndex = worldRuntime->activeFactionRuntimeIndex;
        assetId = runtimeRecord->armyAssetId;
        InGameCommand_Issue<GameFactionRuntime_RemoveArmyAssetAndStagePlayerTransfer>(0,assetId,factionIndex);
      }
      else {
        factionIndex = worldRuntime->activeFactionRuntimeIndex;
        assetId = runtimeRecord->armyAssetId;
        InGameCommand_Issue<GameFactionRuntime_SellArmyAssetAndRefundSevenEighths>(0,assetId,factionIndex);
      }
    }
  }
}

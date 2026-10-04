/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/src/ui/ingame/army_stock.cpp
 * Reverse engineering by idkFoxes 2026
 */

#include <thandor/ui/ingame/army_stock.h>
#include <thandor/thandor.h>

/* Module data. */

static int32_t g_UiCommandSpriteVariantAOffsets[24] = {
    /*  0 */ 36116, 36240, 36364, 36488, 36612, 36736, 36860, 36984,
    /*  8 */ 37108, 37232, 37356, 37480, 37604, 37728, 37852, 37976,
    /* 16 */ 38100, 38224, 38348, 38472, 38596, 38720, 38844, 38968};

static uint32_t g_UiCommandSpriteVariantAColumnCount = 0;

static int32_t *g_UiCommandSpriteVariantAOffsetTables[5] = {
    /* 0 */ THANDOR_PTR(&g_UiCommandSpriteVariantAOffsets),
    /* 1 */ THANDOR_PTR(&g_UiCommandSpriteVariantAOffsets),
    /* 2 */ THANDOR_PTR(&g_UiCommandSpriteVariantAOffsets),
    /* 3 */ THANDOR_PTR(&g_UiCommandSpriteVariantAOffsets),
    /* 4 */ THANDOR_PTR(&g_UiCommandSpriteVariantAOffsets)};

static UiCommandRuntimeRecordPrefix *g_UiCommandSpriteVariantARecords[24] = {0};

/* Implementation ownership: ui/ingame/army_stock. */

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
    do {
      if ((int)((uintptr_t)control - (uintptr_t)g_InGameRuntimeRoot) ==
          g_UiCommandSpriteVariantAOffsetTables[g_UiCommandSpriteVariantAColumnCount][recordIndex])
      {
        g_UiHoverSelectionRecord = g_UiCommandSpriteVariantARecords[recordIndex];
        InGameSelectionDetailPanel_Rebuild();
        break;
      }
      recordIndex--;
    } while (-1 < recordIndex);
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
  uint32_t itemCount;
  FactionArmyAssetCount remainingAssets;
  int panelHeight;
  uint32_t slotIndex;
  UiCommandRuntimeRecordPrefix **recordCursor;
  uint32_t *assetCursor;
  UiGridDimensions gridDimensions;

  /* node becomes the in-game UI root */
  while (node->parent != UI_NODE_NONE) {
    node = node->parent;
  }
  recordCursor = g_UiCommandSpriteVariantARecords;
  for (remainingSlots = ARMY_STOCK_ENTRY_COUNT; remainingSlots != 0; remainingSlots--) {
    *recordCursor = NULL;
    recordCursor++;
  }
  recordCursor = g_UiCommandSpriteVariantARecords;
  remainingAssets = g_GameFactionRuntimeImage.records[((WorldRuntimeContext *)INGAME_UI(node,worldView))->activeFactionRuntimeIndex].primaryArmyAssetCount;
  itemCount = 0;
  assetCursor = g_GameFactionRuntimeImage.records[((WorldRuntimeContext *)INGAME_UI(node,worldView))->activeFactionRuntimeIndex].primaryArmyAssetPointersOrIds;
  if ((g_UiCommandRuntimeFlags & UI_COMMAND_RUNTIME_FLAG_WORLD_INPUT_DISABLED) == 0) {
    for (; remainingAssets != 0; remainingAssets--) {
      if ((((UiCommandRuntimeRecordPrefix *)*assetCursor)->textureSource != NULL) && (itemCount < ARMY_STOCK_ENTRY_COUNT)) { /* 5f-format: GameFactionRuntimeRecord.primaryArmyAssetPointersOrIds */
        *recordCursor = (UiCommandRuntimeRecordPrefix *)*assetCursor; /* 5f-format: GameFactionRuntimeRecord.primaryArmyAssetPointersOrIds */
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
    INGAME_UI(node,armyStockFrame)->leftOffset = -31;
    INGAME_UI(node,armyStockFrame)->rightOffset = -31;
    INGAME_UI(node,armyStockFrame)->topOffset = -13;
    INGAME_UI(node,armyStockFrame)->bottomOffset = -13;
  }
  else {
    INGAME_UI(node,armyStockFrame)->leftOffset = -39;
    INGAME_UI(node,armyStockFrame)->rightOffset = -39;
    INGAME_UI(node,armyStockFrame)->topOffset = -18;
    INGAME_UI(node,armyStockFrame)->bottomOffset = -18;
  }
  INGAME_UI(node,armyStockFrame)->leftOffset -= panelWidth;
  INGAME_UI(node,armyStockFrame)->topOffset -= panelHeight;
  if (itemCount == 0) {
    INGAME_UI(node,armyStockFrame)->nodeFlags |= UI_NODE_SUPPRESSED;
  }
  else {
    INGAME_UI(node,armyStockFrame)->nodeFlags &= ~UI_NODE_SUPPRESSED;
  }
  offsetTable = g_UiCommandSpriteVariantAOffsetTables[columnCount];
  for (slotIndex = 0; slotIndex < ARMY_STOCK_ENTRY_COUNT; slotIndex++) {
    slotOffset = offsetTable[slotIndex];
    if (slotIndex < itemCount) {
      THANDOR_UI_AT(node,slotOffset)->nodeFlags &= ~UI_NODE_SUPPRESSED;
      slotTexture = g_UiCommandSpriteVariantARecords[slotIndex]->textureSource;
    }
    else {
      THANDOR_UI_AT(node,slotOffset)->nodeFlags |= UI_NODE_SUPPRESSED;
      slotTexture = NULL;
    }
    ((UiCommandSpriteButtonControl *)THANDOR_UI_AT(node,slotOffset))->sprite.primaryTextureSource = slotTexture;
  }
  INGAME_UI(node,armyStockPanel)->vtable->layout(INGAME_UI(node,armyStockPanel));
  return;
}

/* Army stock slot click (action 0x1001, g_InGameUiActionHandlersPage10[1]): first drops any army still waiting
   for placement (command 0x14F0), then takes the slot's army for placement on the map, or sells it with Ctrl
   (activationInputState & KEYBOARD_STATE_CTRL). Ignored while paused, while the world input is disabled and while
   world runtime flag 0x10 is set.
*/
void InGameArmyStock_TakeOrSellSlotArmy(UiCommandSpriteButtonControl *control)

{
  int32_t *flagsField;
  UiCommandSpriteButtonControl *root;
  UiCommandRuntimeRecordPrefix *runtimeRecord;
  FactionRuntimeIndex factionIndex;
  PckArmyAssetIdCatalog assetId;
  int slotIndex;

  if ((g_UiCommandRuntimeFlags &
      (UI_COMMAND_RUNTIME_FLAG_PAUSED | UI_COMMAND_RUNTIME_FLAG_WORLD_INPUT_DISABLED)) == 0) {
    root = control;
    while ((root->sprite).selectable.base.parent != UI_NODE_NONE) {
      root = (UiCommandSpriteButtonControl *)(root->sprite).selectable.base.parent;
    }
    /* end any hover of the stock panel (image control) */
    g_UiImageControlHoverTarget = NULL;
    flagsField = (int32_t *)&((UiImageControl *)INGAME_UI(root,armyStockPanel))->selectable.stateFlags;
    *flagsField = *flagsField & ~UI_IMAGE_CONTROL_HOVER_STATE_BITS;
    if ((((WorldRuntimeContext *)INGAME_UI(root,worldView))->runtimeFlags & WORLD_RUNTIME_FLAG_NOTIFICATION_GOTO) == 0) {
      slotIndex = ARMY_STOCK_ENTRY_COUNT - 1;
      while ((int)((uintptr_t)control - (uintptr_t)root) !=
             g_UiCommandSpriteVariantAOffsetTables[g_UiCommandSpriteVariantAColumnCount][slotIndex]) {
        slotIndex--;
        if (slotIndex < 0) {
          return;
        }
      }
      runtimeRecord = g_UiCommandSpriteVariantARecords[slotIndex];
      factionIndex = ((WorldRuntimeContext *)INGAME_UI(root,worldView))->activeFactionRuntimeIndex;
      if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_NETWORKED_MASK) ==
          SESSION_NETWORK_ROLE_LOCAL) {
        GameFactionRuntime_ConsumePendingArmyAssetAndRefreshGrid
                  (g_LocalPlayerRuntimeId,0,0,factionIndex);
      }
      else {
        InGameCommandQueue_AppendLocalPlayerCommand
                  (INGAME_COMMAND_CONSUME_PENDING_ARMY,0,0,(CommandPayload)factionIndex);
      }
      if ((control->activationInputState & UI_COMMAND_ACTIVATION_RELATION_RESET_REQUEST_MASK) == 0)
      {
        factionIndex = ((WorldRuntimeContext *)INGAME_UI(root,worldView))->activeFactionRuntimeIndex;
        assetId = runtimeRecord->armyAssetId;
        if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_NETWORKED_MASK) ==
            SESSION_NETWORK_ROLE_LOCAL) {
          GameFactionRuntime_RemoveArmyAssetAndStagePlayerTransfer
                    (g_LocalPlayerRuntimeId,0,assetId,factionIndex);
        }
        else {
          InGameCommandQueue_AppendLocalPlayerCommand
                    (INGAME_COMMAND_TAKE_ARMY_FOR_PLACEMENT,0,assetId,(CommandPayload)factionIndex);
        }
      }
      else {
        factionIndex = ((WorldRuntimeContext *)INGAME_UI(root,worldView))->activeFactionRuntimeIndex;
        assetId = runtimeRecord->armyAssetId;
        if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_NETWORKED_MASK) ==
            SESSION_NETWORK_ROLE_LOCAL) {
          GameFactionRuntime_SellArmyAssetAndRefundSevenEighths
                    (g_LocalPlayerRuntimeId,0,assetId,factionIndex);
        }
        else {
          InGameCommandQueue_AppendLocalPlayerCommand
                    (INGAME_COMMAND_SELL_ARMY,0,assetId,(CommandPayload)factionIndex);
        }
      }
    }
  }
  return;
}

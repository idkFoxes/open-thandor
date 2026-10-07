/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/include/thandor/ui/ingame/army_stock.h
 * Reverse engineering by idkFoxes 2026
 */

#ifndef THANDOR_UI_INGAME_ARMY_STOCK_H
#define THANDOR_UI_INGAME_ARMY_STOCK_H

#include <thandor/ui/controls/types.h>
#include <thandor/ui/ingame/types.h>
#include <thandor/core/contracts.h>

/* g_UiCommandRuntimeFlags bits of the world view overlays (FrontendModelPointerContext_RenderWorldViewQueuesClipped);
   no writer with a constant mask, so they can only come from command 0x310 */
inline constexpr int32_t UI_COMMAND_RUNTIME_FLAG_HIDE_WORLD_OVERLAYS = 0x8000; /* skips every selection overlay of the world view */
inline constexpr int32_t UI_COMMAND_RUNTIME_FLAG_DRAW_DEBUG_CELL_MARKERS = 0x40; /* debug overlay
                                                                     SelectionOverlay_DrawDebugMarkedCellMarkers */
/* labelFlags bits of those world view status texts */
inline constexpr int32_t UI_WORLD_TEXT_PAUSED_ONLY = 0x800; /* drawn only while the game is paused */
inline constexpr int32_t UI_WORLD_TEXT_SHIFT_BY_STEP_TICKS = 0x1000; /* needs g_InGameSimulationStepTicks > 1; text shifted by ticks - 2
                                                    bytes */
/* Army stock panel (UiCommandSpriteVariantA_*, g_UiCommandSpriteVariantARecords) */
inline constexpr int32_t ARMY_STOCK_ENTRY_COUNT = 24;
inline constexpr int32_t ARMY_STOCK_MAX_COLUMNS = 4;
inline constexpr int32_t INGAME_CURSOR_FRAME_ARMY_STOCK = 10; /* pointer over an army stock slot */
inline constexpr int32_t INGAME_CURSOR_FRAME_ARMY_STOCK_SELL = 12; /* the same with Ctrl held: a click sells the army */

GraphicsCursorFrameIndex InGameArmyStock_PointerMoveShowSlotDetails(UiPixelCoordinate pointerY,UiPixelCoordinate pointerX,
          UiCommandSpriteButtonControl *control);

void InGameArmyStock_RebuildGrid(UiNodeBase *node);

void InGameArmyStock_TakeOrSellSlotArmy(UiCommandSpriteButtonControl *control);

#endif /* THANDOR_UI_INGAME_ARMY_STOCK_H */

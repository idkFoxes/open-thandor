/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/include/thandor/ui/frontend/results.h
 * Reverse engineering by idkFoxes 2026
 */

#ifndef THANDOR_UI_FRONTEND_RESULTS_H
#define THANDOR_UI_FRONTEND_RESULTS_H

#include <thandor/assets/text/types.h>
#include <thandor/ui/controls/types.h>
#include <thandor/ui/frontend/types.h>
#include <thandor/core/contracts.h>

/* Text resource ids of the results table (FrontendResultsTable_Draw*Column): column headers, and the template
   the numeric columns patch their value into (selector 0). The faction-field columns take their header
   (0x21B6..0x21BF) and value template (0x21C1..0x21C3) from FrontendResultsTable_DrawColumnSequenceByType. */
inline constexpr int32_t TEXT_ID_RESULTS_ECONOMY = 0x21B0;
inline constexpr int32_t TEXT_ID_RESULTS_MILITARY = 0x21B1;
inline constexpr int32_t TEXT_ID_RESULTS_COLOUR = 0x21B2;
inline constexpr int32_t TEXT_ID_RESULTS_POINTS = 0x21B3;
inline constexpr int32_t TEXT_ID_RESULTS_PLAYER = 0x21B4;
inline constexpr int32_t TEXT_ID_RESULTS_FACTION = 0x21B5;
inline constexpr int32_t TEXT_ID_RESULTS_VALUE_TEMPLATE = 0x21C4;
inline constexpr int32_t TEXT_ID_RESULTS_FIELD_HEADER_BASE = 0x21B6; /* + n for faction-field column n (0..9) */
inline constexpr int32_t TEXT_ID_RESULTS_FIELD_VALUE_TEMPLATE1 = 0x21C1; /* exploredTerrainPercent */
inline constexpr int32_t TEXT_ID_RESULTS_FIELD_VALUE_TEMPLATE2 = 0x21C2; /* technology count and the relation counters */
inline constexpr int32_t TEXT_ID_RESULTS_FIELD_VALUE_TEMPLATE3 = 0x21C3; /* the two resource components */
/* End-of-game results title: selector 0 = level title, selector 1 = elapsed time (Frontend_MainLoop). */
inline constexpr int32_t TEXT_ID_RESULTS_TITLE_TEMPLATE = 0x21C0;
/* FrontendResultsColumnSequenceControl.modeFlags bit 0: graph (factionWeightRaster) instead of the table. */
inline constexpr int32_t FRONTEND_RESULTS_MODE_GRAPH = 0x1;
/* Column types of the results table (columnTypes0..); each advances by its g_FrontendResultsColumnAdvance*. */
inline constexpr int32_t FRONTEND_RESULTS_COLUMN_SPACER0 = 0;
inline constexpr int32_t FRONTEND_RESULTS_COLUMN_SPACER1 = 1;
inline constexpr int32_t FRONTEND_RESULTS_COLUMN_COLOUR = 2;
inline constexpr int32_t FRONTEND_RESULTS_COLUMN_ECONOMY = 3;
inline constexpr int32_t FRONTEND_RESULTS_COLUMN_MILITARY = 4;
inline constexpr int32_t FRONTEND_RESULTS_COLUMN_POINTS = 5;
inline constexpr int32_t FRONTEND_RESULTS_COLUMN_PLAYER = 6;
inline constexpr int32_t FRONTEND_RESULTS_COLUMN_FACTION = 7;
inline constexpr int32_t FRONTEND_RESULTS_COLUMN_FACTION_FIELD = 8; /* 8..0x11: faction record fields exploredTerrainPercent..relationCounterF */
/* Results graph: g_GameStatTableImage holds one 0x38-byte sample (7 factions x 2 dwords) every 128 simulation
   ticks. */
inline constexpr int32_t RESULTS_STAT_SAMPLE_BYTES = 0x38;
inline constexpr int32_t RESULTS_STAT_SAMPLE_TICK_SHIFT = 7;
/* Faction colour text (TEXT_ID_FACTION_NAME_BASE + colorIndex) read by FrontendResultsTable_DrawColumnSequenceByType:
   code units 1..8 are four digit pairs (blue, green, red, alpha), high digit first; the low nibble of each code
   unit is the digit value. */
inline constexpr int32_t FACTION_COLOUR_TEXT_DIGIT_MASK = 0xf;
/* one digit pair as the top byte (bits 24..31) of a dword; >> 24 gives the byte, >> 22 its offset in a table of
   dwords */
#define FACTION_COLOUR_TEXT_PAIR_TOP_BYTE(highUnit, lowUnit) \
          (((uint8_t)(lowUnit) & FACTION_COLOUR_TEXT_DIGIT_MASK) << 24 | (uint32_t)(uint8_t)(highUnit) << 28)

void FrontendResultsTable_DrawColumnSequenceByType(int clipBottom,int clipRight,int clipTop,int clipLeft,
          FrontendResultsColumnSequenceControl *control);

UiNodeBase * FrontendResultsTable_HitTestAlwaysNone
          (UiPixelCoordinate pointerY,UiPixelCoordinate pointerX,UiNodeBase *control);

void FrontendResultsTable_DrawColourColumn
          (UiPixelCoordinate clipBottom,UiPixelCoordinate clipRight,UiPixelCoordinate clipTop,
          UiPixelCoordinate clipLeft,UiPixelCoordinate drawY,UiPixelCoordinate drawX,
          const FrontendResultsColumnSequenceControl *table);

void FrontendResultsTable_DrawFactionColumn
          (UiPixelCoordinate clipBottom,UiPixelCoordinate clipRight,UiPixelCoordinate clipTop,
          UiPixelCoordinate clipLeft,UiPixelCoordinate drawY,UiPixelCoordinate drawX,
          const FrontendResultsColumnSequenceControl *table);

void FrontendResultsTable_DrawFormattedFactionFieldColumn
          (TextResourceId valueFormatResourceId,TextResourceId headerResourceId,
          FrontendResultsFactionFieldByteOffset factionFieldOffset,UiPixelCoordinate clipBottom,
          UiPixelCoordinate clipRight,UiPixelCoordinate clipTop,UiPixelCoordinate clipLeft,
          UiPixelCoordinate drawY,UiPixelCoordinate drawX,const FrontendResultsColumnSequenceControl *table);

void FrontendResultsTable_DrawPointsColumn
          (UiPixelCoordinate clipBottom,UiPixelCoordinate clipRight,UiPixelCoordinate clipTop,
          UiPixelCoordinate clipLeft,UiPixelCoordinate drawY,UiPixelCoordinate drawX,
          const FrontendResultsColumnSequenceControl *table);

void FrontendResultsTable_DrawEconomyColumn
          (UiPixelCoordinate clipBottom,UiPixelCoordinate clipRight,UiPixelCoordinate clipTop,
          UiPixelCoordinate clipLeft,UiPixelCoordinate drawY,UiPixelCoordinate drawX,
          const FrontendResultsColumnSequenceControl *table);

void FrontendResultsTable_DrawMilitaryColumn
          (UiPixelCoordinate clipBottom,UiPixelCoordinate clipRight,UiPixelCoordinate clipTop,
          UiPixelCoordinate clipLeft,UiPixelCoordinate drawY,UiPixelCoordinate drawX,
          const FrontendResultsColumnSequenceControl *table);

void FrontendResultsTable_DrawPlayerColumn
          (UiPixelCoordinate clipBottom,UiPixelCoordinate clipRight,UiPixelCoordinate clipTop,
          UiPixelCoordinate clipLeft,UiPixelCoordinate drawY,UiPixelCoordinate drawX,
          const FrontendResultsColumnSequenceControl *table);


void FrontendResultsGraph_DrawFactionWeightSumColumn
          (UiPixelCoordinate spanEndY,UiPixelCoordinate spanStartY,UiPixelCoordinate drawX,
          FrontendResultsFactionWeightPair *factionWeights);

void FrontendResultsGraph_DrawFactionWeightLane0Column
          (UiPixelCoordinate spanEndY,UiPixelCoordinate spanStartY,UiPixelCoordinate drawX,
          FrontendResultsFactionWeightPair *factionWeights);

void FrontendResultsGraph_DrawFactionWeightLane1Column
          (UiPixelCoordinate spanEndY,UiPixelCoordinate spanStartY,UiPixelCoordinate drawX,
          FrontendResultsFactionWeightPair *factionWeights);

extern UiNodeVtable g_FrontendResultsTableVtable;

#endif /* THANDOR_UI_FRONTEND_RESULTS_H */

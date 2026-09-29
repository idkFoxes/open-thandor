/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/include/thandor/ui/frontend/results.h
 * Reverse engineering by idkFoxes 2026
 */

#ifndef THANDOR_UI_FRONTEND_RESULTS_H
#define THANDOR_UI_FRONTEND_RESULTS_H

#include <thandor/generated/types.h>
#include <thandor/core/contracts.h>

/* Submodule: ui/frontend/results. */

/* Text resource ids of the results table (FrontendResultsTable_Draw*Column): column headers, and the template
   the numeric columns patch their value into (selector 0). The faction-field columns take their header
   (0x21B6..0x21BF) and value template (0x21C1..0x21C3) from FrontendResultsTable_DrawColumnSequenceByType. */
#define TEXT_ID_RESULTS_ECONOMY 0x21B0
#define TEXT_ID_RESULTS_MILITARY 0x21B1
#define TEXT_ID_RESULTS_COLOUR 0x21B2
#define TEXT_ID_RESULTS_POINTS 0x21B3
#define TEXT_ID_RESULTS_PLAYER 0x21B4
#define TEXT_ID_RESULTS_FACTION 0x21B5
#define TEXT_ID_RESULTS_VALUE_TEMPLATE 0x21C4
#define TEXT_ID_RESULTS_FIELD_HEADER_BASE 0x21B6 /* + n for faction-field column n (0..9) */
#define TEXT_ID_RESULTS_FIELD_VALUE_TEMPLATE1 0x21C1 /* exploredTerrainPercent */
#define TEXT_ID_RESULTS_FIELD_VALUE_TEMPLATE2 0x21C2 /* technology count and the relation counters */
#define TEXT_ID_RESULTS_FIELD_VALUE_TEMPLATE3 0x21C3 /* the two resource components */
/* End-of-game results title: selector 0 = level title, selector 1 = elapsed time (Frontend_MainLoop). */
#define TEXT_ID_RESULTS_TITLE_TEMPLATE 0x21C0
/* FrontendResultsColumnSequenceControl.modeFlags bit 0: graph (factionWeightRaster) instead of the table. */
#define FRONTEND_RESULTS_MODE_GRAPH 0x1
/* Column types of the results table (columnTypes0..); each advances by its g_FrontendResultsColumnAdvance*. */
#define FRONTEND_RESULTS_COLUMN_SPACER0 0
#define FRONTEND_RESULTS_COLUMN_SPACER1 1
#define FRONTEND_RESULTS_COLUMN_COLOUR 2
#define FRONTEND_RESULTS_COLUMN_ECONOMY 3
#define FRONTEND_RESULTS_COLUMN_MILITARY 4
#define FRONTEND_RESULTS_COLUMN_POINTS 5
#define FRONTEND_RESULTS_COLUMN_PLAYER 6
#define FRONTEND_RESULTS_COLUMN_FACTION 7
#define FRONTEND_RESULTS_COLUMN_FACTION_FIELD 8 /* 8..0x11: faction record fields +0x98..+0xBC */
/* Results graph: g_GameStatTableImage holds one 0x38-byte sample (7 factions x 2 dwords) every 128 simulation
   ticks. */
#define RESULTS_STAT_SAMPLE_BYTES 0x38
#define RESULTS_STAT_SAMPLE_TICK_SHIFT 7
/* Functions are grouped by semantic ownership; address comments are executable virtual addresses. */

/* 0x00517020 */
void FrontendResultsTable_DrawColumnSequenceByType(int clipBottom,int clipRight,int clipTop,int clipLeft,
          FrontendResultsColumnSequenceControl *control);

/* 0x00517FB0 */
UiNodeBase * FrontendResultsTable_HitTestAlwaysNone
          (UiPixelCoordinate pointerY,UiPixelCoordinate pointerX,UiNodeBase *control);

/* 0x005177F0 */
void FrontendResultsTable_DrawColourColumn
          (UiPixelCoordinate clipBottom,UiPixelCoordinate clipRight,UiPixelCoordinate clipTop,
          UiPixelCoordinate clipLeft,UiPixelCoordinate drawX,UiPixelCoordinate drawY,
          FrontendResultsRowMetrics *rowMetrics);

/* 0x005178B0 */
void FrontendResultsTable_DrawFactionColumn
          (UiPixelCoordinate clipBottom,UiPixelCoordinate clipRight,UiPixelCoordinate clipTop,
          UiPixelCoordinate clipLeft,UiPixelCoordinate drawX,UiPixelCoordinate drawY,
          FrontendResultsRowMetrics *rowMetrics);

/* 0x00517960 */
void FrontendResultsTable_DrawFormattedFactionFieldColumn
          (TextResourceId valueFormatResourceId,TextResourceId headerResourceId,
          FrontendResultsFactionFieldByteOffset factionFieldOffset,UiPixelCoordinate clipBottom,
          UiPixelCoordinate clipRight,UiPixelCoordinate clipTop,UiPixelCoordinate clipLeft,
          UiPixelCoordinate drawX,UiPixelCoordinate drawY,FrontendResultsRowMetrics *rowMetrics);

/* 0x00517A30 */
void FrontendResultsTable_DrawPointsColumn
          (UiPixelCoordinate clipBottom,UiPixelCoordinate clipRight,UiPixelCoordinate clipTop,
          UiPixelCoordinate clipLeft,UiPixelCoordinate drawX,UiPixelCoordinate drawY,
          FrontendResultsRowMetrics *rowMetrics);

/* 0x00517B10 */
void FrontendResultsTable_DrawEconomyColumn
          (UiPixelCoordinate clipBottom,UiPixelCoordinate clipRight,UiPixelCoordinate clipTop,
          UiPixelCoordinate clipLeft,UiPixelCoordinate drawX,UiPixelCoordinate drawY,
          FrontendResultsRowMetrics *rowMetrics);

/* 0x00517BF0 */
void FrontendResultsTable_DrawMilitaryColumn
          (UiPixelCoordinate clipBottom,UiPixelCoordinate clipRight,UiPixelCoordinate clipTop,
          UiPixelCoordinate clipLeft,UiPixelCoordinate drawX,UiPixelCoordinate drawY,
          FrontendResultsRowMetrics *rowMetrics);

/* 0x00517CD0 */
void FrontendResultsTable_DrawPlayerColumn
          (UiPixelCoordinate clipBottom,UiPixelCoordinate clipRight,UiPixelCoordinate clipTop,
          UiPixelCoordinate clipLeft,UiPixelCoordinate drawX,UiPixelCoordinate drawY,
          FrontendResultsRowMetrics *rowMetrics);


/* 0x005174E0 */
void FrontendResultsGraph_DrawFactionWeightSumColumn
          (UiPixelCoordinate spanEndY,UiPixelCoordinate spanStartY,UiPixelCoordinate drawX,
          FrontendResultsFactionWeightPair *factionWeights);

/* 0x005175F0 */
void FrontendResultsGraph_DrawFactionWeightLane0Column
          (UiPixelCoordinate spanEndY,UiPixelCoordinate spanStartY,UiPixelCoordinate drawX,
          FrontendResultsFactionWeightPair *factionWeights);

/* 0x005176F0 */
void FrontendResultsGraph_DrawFactionWeightLane1Column
          (UiPixelCoordinate spanEndY,UiPixelCoordinate spanStartY,UiPixelCoordinate drawX,
          FrontendResultsFactionWeightPair *factionWeights);

#endif /* THANDOR_UI_FRONTEND_RESULTS_H */

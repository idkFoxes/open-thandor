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
/* Results graph: g_GameStatTableImage holds one 0x38-byte sample (7 factions x 2 dwords) every 128 simulation
   ticks. */
#define RESULTS_STAT_SAMPLE_BYTES 0x38
#define RESULTS_STAT_SAMPLE_TICK_SHIFT 7
/* Functions are grouped by semantic ownership; address comments are executable virtual addresses. */

/* 0x00517020 */
void __thandor_void_preserve_eax_ecx_edx
FrontendResultsTable_DrawColumnSequenceByType
          (int clipTop,int clipLeft,int clipBottom,int clipRight,
          FrontendResultsColumnSequenceControl68 *control);

/* 0x00517FB0 */
UiNodeBase * __thandor_eax_preserve_ecx_edx
FrontendResultsTable_HitTestAlwaysNone
          (UiPixelCoordinate pointerY,UiPixelCoordinate pointerX,UiNodeBase *control);

/* 0x005177F0 */
void __thandor_void_preserve_eax_ecx_edx
FrontendResultsTable_DrawColourColumn
          (UiPixelCoordinate clipTop,UiPixelCoordinate clipLeft,UiPixelCoordinate clipBottom,
          UiPixelCoordinate clipRight,UiPixelCoordinate drawX,UiPixelCoordinate drawY,
          FrontendResultsRowMetrics *rowMetrics);

/* 0x005178B0 */
void __thandor_void_preserve_eax_ecx_edx
FrontendResultsTable_DrawFactionColumn
          (UiPixelCoordinate clipTop,UiPixelCoordinate clipLeft,UiPixelCoordinate clipBottom,
          UiPixelCoordinate clipRight,UiPixelCoordinate drawX,UiPixelCoordinate drawY,
          FrontendResultsRowMetrics *rowMetrics);

/* 0x00517960 */
void __thandor_void_preserve_eax_ecx_edx
FrontendResultsTable_DrawFormattedFactionFieldColumn
          (TextResourceId valueFormatResourceId,TextResourceId headerResourceId,
          FrontendResultsFactionFieldByteOffset factionFieldOffset,UiPixelCoordinate clipTop,
          UiPixelCoordinate clipLeft,UiPixelCoordinate clipBottom,UiPixelCoordinate clipRight,
          UiPixelCoordinate drawX,UiPixelCoordinate drawY,FrontendResultsRowMetrics *rowMetrics);

/* 0x00517A30 */
void __thandor_void_preserve_eax_ecx_edx
FrontendResultsTable_DrawPointsColumn
          (UiPixelCoordinate clipTop,UiPixelCoordinate clipLeft,UiPixelCoordinate clipBottom,
          UiPixelCoordinate clipRight,UiPixelCoordinate drawX,UiPixelCoordinate drawY,
          FrontendResultsRowMetrics *rowMetrics);

/* 0x00517B10 */
void __thandor_void_preserve_eax_ecx_edx
FrontendResultsTable_DrawEconomyColumn
          (UiPixelCoordinate clipTop,UiPixelCoordinate clipLeft,UiPixelCoordinate clipBottom,
          UiPixelCoordinate clipRight,UiPixelCoordinate drawX,UiPixelCoordinate drawY,
          FrontendResultsRowMetrics *rowMetrics);

/* 0x00517BF0 */
void __thandor_void_preserve_eax_ecx_edx
FrontendResultsTable_DrawMilitaryColumn
          (UiPixelCoordinate clipTop,UiPixelCoordinate clipLeft,UiPixelCoordinate clipBottom,
          UiPixelCoordinate clipRight,UiPixelCoordinate drawX,UiPixelCoordinate drawY,
          FrontendResultsRowMetrics *rowMetrics);

/* 0x00517CD0 */
void __thandor_void_preserve_eax_ecx_edx
FrontendResultsTable_DrawPlayerColumn
          (UiPixelCoordinate clipTop,UiPixelCoordinate clipLeft,UiPixelCoordinate clipBottom,
          UiPixelCoordinate clipRight,UiPixelCoordinate drawX,UiPixelCoordinate drawY,
          FrontendResultsRowMetrics *rowMetrics);


/* 0x005174E0 */
void __thandor_void_preserve_eax_ecx_edx
FrontendResultsGraph_DrawFactionWeightSumColumn
          (UiPixelCoordinate spanEndY,UiPixelCoordinate spanStartY,UiPixelCoordinate drawX,
          FrontendResultsFactionWeightPair8 *factionWeights);

/* 0x005175F0 */
void __thandor_void_preserve_eax_ecx_edx
FrontendResultsGraph_DrawFactionWeightLane0Column
          (UiPixelCoordinate spanEndY,UiPixelCoordinate spanStartY,UiPixelCoordinate drawX,
          FrontendResultsFactionWeightPair8 *factionWeights);

/* 0x005176F0 */
void __thandor_void_preserve_eax_ecx_edx
FrontendResultsGraph_DrawFactionWeightLane1Column
          (UiPixelCoordinate spanEndY,UiPixelCoordinate spanStartY,UiPixelCoordinate drawX,
          FrontendResultsFactionWeightPair8 *factionWeights);

#endif /* THANDOR_UI_FRONTEND_RESULTS_H */

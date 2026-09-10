#ifndef THANDOR_UI_FRONTEND_RESULTS_H
#define THANDOR_UI_FRONTEND_RESULTS_H

#include <thandor/generated/v523_types.h>
#include <thandor/core/contracts.h>

/* Submodule: ui/frontend/results. */
/* Functions are grouped by semantic ownership; address comments are executable virtual addresses. */

/* 0x00517020 */
void FrontendResultsTable_DrawColumnSequenceByType (int clipTop,int clipLeft,int clipBottom,int clipRight,UiNodeBase *control);

/* 0x00517FB0 */
undefined4 FrontendResultsTable_HitTestAlwaysNone(void);

/* 0x005177F0 */
undefined8 FrontendResultsTable_DrawColourColumn (UiPixelCoordinate clipTop,UiPixelCoordinate clipLeft,UiPixelCoordinate clipBottom, UiPixelCoordinate clipRight,UiPixelCoordinate drawX,UiPixelCoordinate drawY, FrontendResultsRowMetrics *rowMetrics);

/* 0x005178B0 */
undefined8 FrontendResultsTable_DrawFactionColumn (UiPixelCoordinate clipTop,UiPixelCoordinate clipLeft,UiPixelCoordinate clipBottom, UiPixelCoordinate clipRight,UiPixelCoordinate drawX,UiPixelCoordinate drawY, FrontendResultsRowMetrics *rowMetrics);

/* 0x00517960 */
undefined8 FrontendResultsTable_DrawFormattedFactionFieldColumn (TextResourceId valueFormatResourceId,TextResourceId headerResourceId, FrontendResultsFactionFieldByteOffset factionFieldOffset,UiPixelCoordinate clipTop, UiPixelCoordinate clipLeft,UiPixelCoordinate clipBottom,UiPixelCoordinate clipRight, UiPixelCoordinate drawX,UiPixelCoordinate drawY,FrontendResultsRowMetrics *rowMetrics);

/* 0x00517A30 */
undefined8 FrontendResultsTable_DrawPointsColumn (UiPixelCoordinate clipTop,UiPixelCoordinate clipLeft,UiPixelCoordinate clipBottom, UiPixelCoordinate clipRight,UiPixelCoordinate drawX,UiPixelCoordinate drawY, FrontendResultsRowMetrics *rowMetrics);

/* 0x00517B10 */
undefined8 FrontendResultsTable_DrawEconomyColumn (UiPixelCoordinate clipTop,UiPixelCoordinate clipLeft,UiPixelCoordinate clipBottom, UiPixelCoordinate clipRight,UiPixelCoordinate drawX,UiPixelCoordinate drawY, FrontendResultsRowMetrics *rowMetrics);

/* 0x00517BF0 */
undefined8 FrontendResultsTable_DrawMilitaryColumn (UiPixelCoordinate clipTop,UiPixelCoordinate clipLeft,UiPixelCoordinate clipBottom, UiPixelCoordinate clipRight,UiPixelCoordinate drawX,UiPixelCoordinate drawY, FrontendResultsRowMetrics *rowMetrics);

/* 0x00517CD0 */
undefined8 FrontendResultsTable_DrawPlayerColumn (UiPixelCoordinate clipTop,UiPixelCoordinate clipLeft,UiPixelCoordinate clipBottom, UiPixelCoordinate clipRight,UiPixelCoordinate drawX,UiPixelCoordinate drawY, FrontendResultsRowMetrics *rowMetrics);

#endif /* THANDOR_UI_FRONTEND_RESULTS_H */

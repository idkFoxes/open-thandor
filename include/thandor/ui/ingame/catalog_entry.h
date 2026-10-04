/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/include/thandor/ui/ingame/catalog_entry.h
 * Reverse engineering by idkFoxes 2026
 */

#ifndef THANDOR_UI_INGAME_CATALOG_ENTRY_H
#define THANDOR_UI_INGAME_CATALOG_ENTRY_H

#include <thandor/ui/controls/types.h>
#include <thandor/ui/ingame/types.h>
#include <thandor/core/contracts.h>

/* Submodule: ui/ingame/catalog_entry. */

/* UiCatalogEntryControl_DrawClipped: packed text styles of the overlays (price, count, percentage); the alert
   colour marks an unaffordable price or a flagged army. MEASURE is only used to measure the text. */
#define UI_CATALOG_TEXT_STYLE_NORMAL 0x1040000
#define UI_CATALOG_TEXT_STYLE_ALERT 0x1050000
#define UI_CATALOG_TEXT_STYLE_MEASURE 0x1000000

/* Functions are grouped by semantic ownership. */

void UiCatalogEntryControl_DrawClipped
          (UiPixelCoordinate clipBottom,UiPixelCoordinate clipRight,UiPixelCoordinate clipTop,
          UiPixelCoordinate clipLeft,UiCatalogEntryControl *control);

GraphicsCursorFrameIndex UiCatalogEntryControl_PointerMove
          (UiPixelCoordinate pointerY,UiPixelCoordinate pointerX,UiCatalogEntryControl *control);

void UiCatalogEntryControl_NonRightRelease
          (UiPointerWheelDelta wheelDelta,UiPixelCoordinate pointerY,UiPixelCoordinate pointerX,
          UiCatalogEntryControl *control);

extern uint32_t g_UiCatalogGroup48ColumnCount;
extern uint32_t g_UiCatalogGroup42ColumnCount;
extern int32_t *g_UiCatalogGroup48OffsetTables[9];
extern int32_t *g_UiCatalogGroup42OffsetTables[7];
extern int32_t g_UiCatalogGroup48OffsetsDefault[48];
extern int32_t g_UiCatalogGroup48Offsets5Columns[48];
extern int32_t g_UiCatalogGroup48Offsets6Columns[48];
extern int32_t g_UiCatalogGroup48Offsets7Columns[48];
extern int32_t g_UiCatalogGroup48Offsets8Columns[48];
extern int32_t g_UiCatalogGroup42OffsetsDefault[42];
extern int32_t g_UiCatalogGroup42Offsets5Columns[42];
extern int32_t g_UiCatalogGroup42Offsets6Columns[42];

extern UiNodeVtable g_UiCatalogEntryControlVtable;

#endif /* THANDOR_UI_INGAME_CATALOG_ENTRY_H */

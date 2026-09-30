/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/src/ui/frontend/results.c
 * Reverse engineering by idkFoxes 2026
 */

#include <thandor/ui/frontend/results.h>
#include <thandor/thandor.h>

/* Implementation ownership: ui/frontend/results. */

/* Address: 0x00517020.
   drawClipped of g_UiNodeVtable_00516F60, the three results charts (resultsChart1..3) of the end-of-game
   results screen. Table mode (modeFlags bit 0 clear) draws the control's list of column types one after the
   other, advancing by each type's width (types 0 and 1 are empty spacers). The "columns" advance downwards
   (drawYOrCount starts at base.top) and each one lays its header and its faction entries out from left to
   right (drawX starts at base.left, the entries advance by rowAdvancePixels in x). Graph mode first converts the colour of
   each faction's colour text (TEXT_ID_FACTION_NAME_BASE + colour index at record +0x38) into a packed pixel
   for g_FrontendResultsFactionPackedPixelColors, then draws one pixel column per x through the control's
   factionWeightRaster, each showing the stat table sample at x / width of the game so far.
*/
void FrontendResultsTable_DrawColumnSequenceByType(int clipBottom,int clipRight,int clipTop,int clipLeft,
          FrontendResultsColumnSequenceControl *control)

{
  UiPixelCoordinate drawX;
  SoftwareFramebufferAccess *framebufferAccess;
  void *statTableImage;
  uint16_t *colourResource;
  uint32_t pixelColumn;
  uint32_t remainingColumns;
  int drawYOrCount;
  uint32_t historySampleCount;
  GameFactionRuntimeRecord *factionRecord;
  uint32_t *columnTypeOrColorCursor;
  bool accessFailed;
  uint16_t *resolvedText;
  
  if ((control->modeFlags & FRONTEND_RESULTS_MODE_GRAPH) == 0) {
    drawX = control->base.left;
    drawYOrCount = control->base.top;
    remainingColumns = control->columnTypeCount;
    columnTypeOrColorCursor = &control->columnTypes0;
    accessFailed = g_GraphicsFramebufferBeginAccess();
    if (!accessFailed) {
      do {
        switch(*columnTypeOrColorCursor) {
        case FRONTEND_RESULTS_COLUMN_SPACER0:
          drawYOrCount = drawYOrCount + g_FrontendResultsColumnAdvance00Pixels;
          break;
        case FRONTEND_RESULTS_COLUMN_SPACER1:
          drawYOrCount = drawYOrCount + g_FrontendResultsColumnAdvance01Pixels;
          break;
        case FRONTEND_RESULTS_COLUMN_COLOUR:
          FrontendResultsTable_DrawColourColumn
                    (clipBottom,clipRight,clipTop,clipLeft,drawYOrCount,drawX,
                     (FrontendResultsRowMetrics *)control);
          drawYOrCount = drawYOrCount + g_FrontendResultsColumnAdvanceColourPixels;
          break;
        case FRONTEND_RESULTS_COLUMN_ECONOMY:
          FrontendResultsTable_DrawEconomyColumn
                    (clipBottom,clipRight,clipTop,clipLeft,drawYOrCount,drawX,
                     (FrontendResultsRowMetrics *)control);
          drawYOrCount = drawYOrCount + g_FrontendResultsColumnAdvanceEconomyPixels;
          break;
        case FRONTEND_RESULTS_COLUMN_MILITARY:
          FrontendResultsTable_DrawMilitaryColumn
                    (clipBottom,clipRight,clipTop,clipLeft,drawYOrCount,drawX,
                     (FrontendResultsRowMetrics *)control);
          drawYOrCount = drawYOrCount + g_FrontendResultsColumnAdvanceMilitaryPixels;
          break;
        case FRONTEND_RESULTS_COLUMN_POINTS:
          FrontendResultsTable_DrawPointsColumn
                    (clipBottom,clipRight,clipTop,clipLeft,drawYOrCount,drawX,
                     (FrontendResultsRowMetrics *)control);
          drawYOrCount = drawYOrCount + g_FrontendResultsColumnAdvancePointsPixels;
          break;
        case FRONTEND_RESULTS_COLUMN_PLAYER:
          FrontendResultsTable_DrawPlayerColumn
                    (clipBottom,clipRight,clipTop,clipLeft,drawYOrCount,drawX,
                     (FrontendResultsRowMetrics *)control);
          drawYOrCount = drawYOrCount + g_FrontendResultsColumnAdvancePlayerPixels;
          break;
        case FRONTEND_RESULTS_COLUMN_FACTION:
          FrontendResultsTable_DrawFactionColumn
                    (clipBottom,clipRight,clipTop,clipLeft,drawYOrCount,drawX,
                     (FrontendResultsRowMetrics *)control);
          drawYOrCount = drawYOrCount + g_FrontendResultsColumnAdvanceFactionPixels;
          break;
        /* faction record fields +0x98..+0xBC (value template, header, field offset) */
        case FRONTEND_RESULTS_COLUMN_FACTION_FIELD:
          FrontendResultsTable_DrawFormattedFactionFieldColumn
                    (TEXT_ID_RESULTS_FIELD_VALUE_TEMPLATE1,TEXT_ID_RESULTS_FIELD_HEADER_BASE + 0,
                     offsetof(GameFactionRuntimeRecord,exploredTerrainPercent),clipBottom,clipRight,clipTop,clipLeft,drawYOrCount,drawX,
                     (FrontendResultsRowMetrics *)control);
          drawYOrCount = drawYOrCount + g_FrontendResultsColumnAdvanceFactionField98Pixels;
          break;
        case FRONTEND_RESULTS_COLUMN_FACTION_FIELD + 1:
          FrontendResultsTable_DrawFormattedFactionFieldColumn
                    (TEXT_ID_RESULTS_FIELD_VALUE_TEMPLATE2,TEXT_ID_RESULTS_FIELD_HEADER_BASE + 1,
                     offsetof(GameFactionRuntimeRecord,unlockedTechnologyCountBeyondBaseline),clipBottom,clipRight,clipTop,clipLeft,drawYOrCount,drawX,
                     (FrontendResultsRowMetrics *)control);
          drawYOrCount = drawYOrCount + g_FrontendResultsColumnAdvanceFactionField9CPixels;
          break;
        case FRONTEND_RESULTS_COLUMN_FACTION_FIELD + 2:
          FrontendResultsTable_DrawFormattedFactionFieldColumn
                    (TEXT_ID_RESULTS_FIELD_VALUE_TEMPLATE3,TEXT_ID_RESULTS_FIELD_HEADER_BASE + 2,
                     offsetof(GameFactionRuntimeRecord,primaryResourceComponent),clipBottom,clipRight,clipTop,clipLeft,drawYOrCount,drawX,
                     (FrontendResultsRowMetrics *)control);
          drawYOrCount = drawYOrCount + g_FrontendResultsColumnAdvanceFactionFieldA0Pixels;
          break;
        case FRONTEND_RESULTS_COLUMN_FACTION_FIELD + 3:
          FrontendResultsTable_DrawFormattedFactionFieldColumn
                    (TEXT_ID_RESULTS_FIELD_VALUE_TEMPLATE3,TEXT_ID_RESULTS_FIELD_HEADER_BASE + 3,
                     offsetof(GameFactionRuntimeRecord,secondaryResourceComponent),clipBottom,clipRight,clipTop,clipLeft,drawYOrCount,drawX,
                     (FrontendResultsRowMetrics *)control);
          drawYOrCount = drawYOrCount + g_FrontendResultsColumnAdvanceFactionFieldA4Pixels;
          break;
        case FRONTEND_RESULTS_COLUMN_FACTION_FIELD + 4:
          FrontendResultsTable_DrawFormattedFactionFieldColumn
                    (TEXT_ID_RESULTS_FIELD_VALUE_TEMPLATE2,TEXT_ID_RESULTS_FIELD_HEADER_BASE + 4,
                     offsetof(GameFactionRuntimeRecord,relationCounterA),clipBottom,clipRight,clipTop,clipLeft,drawYOrCount,drawX,
                     (FrontendResultsRowMetrics *)control);
          drawYOrCount = drawYOrCount + g_FrontendResultsColumnAdvanceFactionFieldA8Pixels;
          break;
        case FRONTEND_RESULTS_COLUMN_FACTION_FIELD + 5:
          FrontendResultsTable_DrawFormattedFactionFieldColumn
                    (TEXT_ID_RESULTS_FIELD_VALUE_TEMPLATE2,TEXT_ID_RESULTS_FIELD_HEADER_BASE + 5,
                     offsetof(GameFactionRuntimeRecord,relationCounterB),clipBottom,clipRight,clipTop,clipLeft,drawYOrCount,drawX,
                     (FrontendResultsRowMetrics *)control);
          drawYOrCount = drawYOrCount + g_FrontendResultsColumnAdvanceFactionFieldACPixels;
          break;
        case FRONTEND_RESULTS_COLUMN_FACTION_FIELD + 6:
          FrontendResultsTable_DrawFormattedFactionFieldColumn
                    (TEXT_ID_RESULTS_FIELD_VALUE_TEMPLATE2,TEXT_ID_RESULTS_FIELD_HEADER_BASE + 6,
                     offsetof(GameFactionRuntimeRecord,relationCounterC),clipBottom,clipRight,clipTop,clipLeft,drawYOrCount,drawX,
                     (FrontendResultsRowMetrics *)control);
          drawYOrCount = drawYOrCount + g_FrontendResultsColumnAdvanceFactionFieldB0Pixels;
          break;
        case FRONTEND_RESULTS_COLUMN_FACTION_FIELD + 7:
          FrontendResultsTable_DrawFormattedFactionFieldColumn
                    (TEXT_ID_RESULTS_FIELD_VALUE_TEMPLATE2,TEXT_ID_RESULTS_FIELD_HEADER_BASE + 7,
                     offsetof(GameFactionRuntimeRecord,relationCounterD),clipBottom,clipRight,clipTop,clipLeft,drawYOrCount,drawX,
                     (FrontendResultsRowMetrics *)control);
          drawYOrCount = drawYOrCount + g_FrontendResultsColumnAdvanceFactionFieldB4Pixels;
          break;
        case FRONTEND_RESULTS_COLUMN_FACTION_FIELD + 8:
          FrontendResultsTable_DrawFormattedFactionFieldColumn
                    (TEXT_ID_RESULTS_FIELD_VALUE_TEMPLATE2,TEXT_ID_RESULTS_FIELD_HEADER_BASE + 8,
                     offsetof(GameFactionRuntimeRecord,relationCounterE),clipBottom,clipRight,clipTop,clipLeft,drawYOrCount,drawX,
                     (FrontendResultsRowMetrics *)control);
          drawYOrCount = drawYOrCount + g_FrontendResultsColumnAdvanceFactionFieldB8Pixels;
          break;
        case FRONTEND_RESULTS_COLUMN_FACTION_FIELD + 9:
          FrontendResultsTable_DrawFormattedFactionFieldColumn
                    (TEXT_ID_RESULTS_FIELD_VALUE_TEMPLATE2,TEXT_ID_RESULTS_FIELD_HEADER_BASE + 9,
                     offsetof(GameFactionRuntimeRecord,relationCounterF),clipBottom,clipRight,clipTop,clipLeft,drawYOrCount,drawX,
                     (FrontendResultsRowMetrics *)control);
          drawYOrCount = drawYOrCount + g_FrontendResultsColumnAdvanceFactionFieldBCPixels;
        }
        columnTypeOrColorCursor++;
        remainingColumns--;
      } while (remainingColumns != 0);
      g_GraphicsFramebufferEndAccess();
    }
  }
  else {
    /* factions 1..7: the colour text's code units 1..8 hold the colour digits (low nibble each) */
    drawYOrCount = 7;
    columnTypeOrColorCursor = g_FrontendResultsFactionPackedPixelColors;
    factionRecord = &g_GameFactionRuntimeImage.records[1];
    do {
      resolvedText = TextResource_Resolve(factionRecord->colorIndex + TEXT_ID_FACTION_NAME_BASE);
      colourResource = resolvedText;
      /* alpha stays in the top byte; red and green index their tables by byte offset, blue by entry */
      *columnTypeOrColorCursor = FACTION_COLOUR_TEXT_PAIR_TOP_BYTE(colourResource[7],colourResource[8]) +
                *(int *)((int)g_SoftwarePixelPackTables->red +
                        (FACTION_COLOUR_TEXT_PAIR_TOP_BYTE(colourResource[5],colourResource[6]) >> 22))
                + *(int *)((int)g_SoftwarePixelPackTables->green +
                          (FACTION_COLOUR_TEXT_PAIR_TOP_BYTE(colourResource[3],colourResource[4]) >> 22)) +
                g_SoftwarePixelPackTables->blue
                [FACTION_COLOUR_TEXT_PAIR_TOP_BYTE(colourResource[1],colourResource[2]) >> 24];
      statTableImage = g_GameStatTableImage;
      framebufferAccess = g_FramebufferAccess;
      factionRecord++;
      columnTypeOrColorCursor++;
      drawYOrCount--;
    } while (drawYOrCount != 0);
    drawYOrCount = control->base.layoutWidth;
    historySampleCount = g_GameFactionRuntimeImage.tail.simulationTick >> RESULTS_STAT_SAMPLE_TICK_SHIFT;
    accessFailed = g_GraphicsFramebufferBeginAccess();
    if (!accessFailed) {
      g_FrontendResultsFramebufferBytesPerPixel = framebufferAccess->bytesPerPixel;
      g_FrontendResultsFramebufferScanlineStrideBytes =
           framebufferAccess->width * g_FrontendResultsFramebufferBytesPerPixel;
      pixelColumn = 0;
      do {
        control->factionWeightRaster
                  (control->base.bottom,control->base.top,pixelColumn + control->base.left,
                   (FrontendResultsFactionWeightPair *)
                   ((int)(((uint64_t)pixelColumn * (uint64_t)historySampleCount) /
                         (uint64_t)(uint32_t)(control->base).layoutWidth) * RESULTS_STAT_SAMPLE_BYTES +
                    (int)statTableImage));
        pixelColumn++;
        drawYOrCount--;
      } while (drawYOrCount != 0);
      g_GraphicsFramebufferEndAccess();
    }
  }
  return;
}


/* Address: 0x00517FB0.
   hitTest of g_UiCommandVisibilityWrappedTextVtable and g_UiCommandVisibilitySingleLineTextVtable: these text
   controls are never hit, so the pointer passes through them (UI_NODE_NONE).
*/
UiNodeBase * FrontendResultsTable_HitTestAlwaysNone
          (UiPixelCoordinate pointerY,UiPixelCoordinate pointerX,UiNodeBase *control)

{
  return UI_NODE_NONE;
}


/* Address: 0x005174E0.
   factionWeightRaster of resultsChart1 (set in its template in image_data.c): draws one pixel column of the
   stacked results graph from spanStartY to spanEndY, split among factions 1..7 in proportion to the sum of both
   metrics of the stat table sample, each in the faction's colour. When all are 0, every active faction counts
   as 1 (written back into the sample). Every pixel is stored as a 16-bit word.
*/
void FrontendResultsGraph_DrawFactionWeightSumColumn
          (UiPixelCoordinate spanEndY,UiPixelCoordinate spanStartY,UiPixelCoordinate drawX,
          FrontendResultsFactionWeightPair *factionWeights)

{
  uint32_t packedColor;
  SoftwareFramebufferAccess *framebufferAccess;
  uint32_t weightTotal;
  uint32_t factionIndex;
  uint8_t *pixelCursor;
  int drawnHeight;
  int segmentHeight;
  uint32_t cumulativeWeight;
  
  framebufferAccess = g_FramebufferAccess;
  weightTotal = factionWeights[0].lane0 + factionWeights[1].lane0 + factionWeights[2].lane0 +
          factionWeights[3].lane0 + factionWeights[4].lane0 + factionWeights[5].lane0 +
          factionWeights[6].lane0 +
          factionWeights[0].lane1 + factionWeights[1].lane1 + factionWeights[2].lane1 +
          factionWeights[3].lane1 + factionWeights[4].lane1 + factionWeights[5].lane1 +
          factionWeights[6].lane1;
  if (weightTotal == 0) {
    factionIndex = 1;
    do {
      if (g_GameFactionRuntimeImage.tail.factionLifecycleStates[factionIndex] != 0) {
        factionWeights[factionIndex - 1].lane0 = factionWeights[factionIndex - 1].lane0 + 1;
        weightTotal++;
      }
      factionIndex++;
    } while (factionIndex < 8);
  }
  pixelCursor = framebufferAccess->pixels +
           (spanStartY * framebufferAccess->width + drawX) * g_FrontendResultsFramebufferBytesPerPixel;
  factionIndex = 0;
  cumulativeWeight = 0;
  drawnHeight = 0;
  do {
    cumulativeWeight = cumulativeWeight + factionWeights->lane0 + factionWeights->lane1;
    segmentHeight = (int)(((uint64_t)cumulativeWeight * (uint64_t)(uint32_t)(spanEndY - spanStartY)) /
                     (uint64_t)weightTotal) - drawnHeight;
    if (segmentHeight != 0) {
      drawnHeight = drawnHeight + segmentHeight;
      packedColor = g_FrontendResultsFactionPackedPixelColors[factionIndex];
      /* The original has a separate entry for bytes-per-pixel != 2, but both paths store 16-bit words. */
      do {
        *(short *)pixelCursor = (short)packedColor;
        pixelCursor = pixelCursor + g_FrontendResultsFramebufferScanlineStrideBytes;
        segmentHeight--;
      } while (segmentHeight != 0);
    }
    factionIndex++;
    factionWeights++;
  } while (factionIndex <= 6);
  return;
}

/* Address: 0x005175F0.
   factionWeightRaster of resultsChart2: like FrontendResultsGraph_DrawFactionWeightSumColumn, but from the
   sample's first metric (lane 0, faction record +0x88 when sampled) only.
*/
void FrontendResultsGraph_DrawFactionWeightLane0Column
          (UiPixelCoordinate spanEndY,UiPixelCoordinate spanStartY,UiPixelCoordinate drawX,
          FrontendResultsFactionWeightPair *factionWeights)

{
  uint32_t packedColor;
  SoftwareFramebufferAccess *framebufferAccess;
  uint32_t factionIndex;
  uint8_t *pixelCursor;
  int drawnHeight;
  uint32_t weightTotal;
  int segmentHeight;
  uint32_t cumulativeWeight;
  
  framebufferAccess = g_FramebufferAccess;
  weightTotal = factionWeights[0].lane0 + factionWeights[1].lane0 + factionWeights[2].lane0 +
          factionWeights[3].lane0 + factionWeights[4].lane0 + factionWeights[5].lane0 +
          factionWeights[6].lane0;
  if (weightTotal == 0) {
    factionIndex = 1;
    do {
      if (g_GameFactionRuntimeImage.tail.factionLifecycleStates[factionIndex] != 0) {
        factionWeights[factionIndex - 1].lane0 = factionWeights[factionIndex - 1].lane0 + 1;
        weightTotal++;
      }
      factionIndex++;
    } while (factionIndex < 8);
  }
  pixelCursor = framebufferAccess->pixels +
           (spanStartY * framebufferAccess->width + drawX) * g_FrontendResultsFramebufferBytesPerPixel;
  factionIndex = 0;
  cumulativeWeight = 0;
  drawnHeight = 0;
  do {
    cumulativeWeight = cumulativeWeight + factionWeights->lane0;
    segmentHeight = (int)(((uint64_t)cumulativeWeight * (uint64_t)(uint32_t)(spanEndY - spanStartY)) /
                     (uint64_t)weightTotal) - drawnHeight;
    if (segmentHeight != 0) {
      drawnHeight = drawnHeight + segmentHeight;
      packedColor = g_FrontendResultsFactionPackedPixelColors[factionIndex];
      /* The original has a separate entry for bytes-per-pixel != 2, but both paths store 16-bit words. */
      do {
        *(short *)pixelCursor = (short)packedColor;
        pixelCursor = pixelCursor + g_FrontendResultsFramebufferScanlineStrideBytes;
        segmentHeight--;
      } while (segmentHeight != 0);
    }
    factionIndex++;
    factionWeights++;
  } while (factionIndex <= 6);
  return;
}

/* Address: 0x005176F0.
   factionWeightRaster of resultsChart3: like FrontendResultsGraph_DrawFactionWeightSumColumn, but from the
   sample's second metric (lane 1, faction record +0x8C when sampled) only.
*/
void FrontendResultsGraph_DrawFactionWeightLane1Column
          (UiPixelCoordinate spanEndY,UiPixelCoordinate spanStartY,UiPixelCoordinate drawX,
          FrontendResultsFactionWeightPair *factionWeights)

{
  uint32_t packedColor;
  SoftwareFramebufferAccess *framebufferAccess;
  uint32_t factionIndex;
  uint8_t *pixelCursor;
  int drawnHeight;
  uint32_t weightTotal;
  int segmentHeight;
  uint32_t cumulativeWeight;
  
  framebufferAccess = g_FramebufferAccess;
  weightTotal = factionWeights[0].lane1 + factionWeights[1].lane1 + factionWeights[2].lane1 +
          factionWeights[3].lane1 + factionWeights[4].lane1 + factionWeights[5].lane1 +
          factionWeights[6].lane1;
  if (weightTotal == 0) {
    factionIndex = 1;
    do {
      if (g_GameFactionRuntimeImage.tail.factionLifecycleStates[factionIndex] != 0) {
        factionWeights[factionIndex - 1].lane1 = factionWeights[factionIndex - 1].lane1 + 1;
        weightTotal++;
      }
      factionIndex++;
    } while (factionIndex < 8);
  }
  pixelCursor = framebufferAccess->pixels +
           (spanStartY * framebufferAccess->width + drawX) * g_FrontendResultsFramebufferBytesPerPixel;
  factionIndex = 0;
  cumulativeWeight = 0;
  drawnHeight = 0;
  do {
    cumulativeWeight = cumulativeWeight + factionWeights->lane1;
    segmentHeight = (int)(((uint64_t)cumulativeWeight * (uint64_t)(uint32_t)(spanEndY - spanStartY)) /
                     (uint64_t)weightTotal) - drawnHeight;
    if (segmentHeight != 0) {
      drawnHeight = drawnHeight + segmentHeight;
      packedColor = g_FrontendResultsFactionPackedPixelColors[factionIndex];
      /* The original has a separate entry for bytes-per-pixel != 2, but both paths store 16-bit words. */
      do {
        *(short *)pixelCursor = (short)packedColor;
        pixelCursor = pixelCursor + g_FrontendResultsFramebufferScanlineStrideBytes;
        segmentHeight--;
      } while (segmentHeight != 0);
    }
    factionIndex++;
    factionWeights++;
  } while (factionIndex <= 6);
  return;
}

/* Address: 0x005177F0.
   Results table column type 2 (FrontendResultsTable_DrawColumnSequenceByType): header TEXT_ID_RESULTS_COLOUR,
   then one row per active faction 1..7 with its colour name (TEXT_ID_FACTION_NAME_BASE + colour index at record
   +0x38).
*/
void FrontendResultsTable_DrawColourColumn
          (UiPixelCoordinate clipBottom,UiPixelCoordinate clipRight,UiPixelCoordinate clipTop,
          UiPixelCoordinate clipLeft,UiPixelCoordinate drawY,UiPixelCoordinate drawX,
          FrontendResultsRowMetrics *rowMetrics)

{
  int offsetOrPenX;
  uint32_t factionIndex;
  GameFactionRuntimeRecord *factionRecord;
  int headerPenX;
  uint16_t *resolvedText;
  
  resolvedText = TextResource_Resolve(TEXT_ID_RESULTS_COLOUR);
  offsetOrPenX = rowMetrics->headerBaselineOffsetPixels + -4;
  headerPenX = drawX + offsetOrPenX;
  RichTextCommandStream_DrawSingleLine
            (clipBottom,clipRight,clipTop,clipLeft,1,resolvedText,drawY + 6,headerPenX);
  factionIndex = 1;
  factionRecord = &g_GameFactionRuntimeImage.records[1];
  offsetOrPenX = (headerPenX - offsetOrPenX) + rowMetrics->headerBaselineOffsetPixels +
          (rowMetrics->rowAdvancePixels >> 1);
  do {
    if (g_GameFactionRuntimeImage.tail.factionLifecycleStates[factionIndex] != 0) {
      resolvedText = TextResource_Resolve(factionRecord->colorIndex + TEXT_ID_FACTION_NAME_BASE);
      RichTextCommandStream_DrawSingleLine
                (clipBottom,clipRight,clipTop,clipLeft,2,resolvedText,drawY + 6,offsetOrPenX);
      offsetOrPenX = offsetOrPenX + rowMetrics->rowAdvancePixels;
    }
    factionIndex++;
    factionRecord++;
  } while (factionIndex < 8);
  return;
}


/* Address: 0x005178B0.
   Results table column type 7: header TEXT_ID_RESULTS_FACTION, then the name of each active faction 1..7
   (TEXT_ID_PLAYER_NUMBER_BASE + faction index).
*/
void FrontendResultsTable_DrawFactionColumn
          (UiPixelCoordinate clipBottom,UiPixelCoordinate clipRight,UiPixelCoordinate clipTop,
          UiPixelCoordinate clipLeft,UiPixelCoordinate drawY,UiPixelCoordinate drawX,
          FrontendResultsRowMetrics *rowMetrics)

{
  int offsetOrPenX;
  uint32_t factionIndex;
  int headerPenX;
  uint16_t *resolvedText;
  
  resolvedText = TextResource_Resolve(TEXT_ID_RESULTS_FACTION);
  offsetOrPenX = rowMetrics->headerBaselineOffsetPixels + -4;
  headerPenX = drawX + offsetOrPenX;
  RichTextCommandStream_DrawSingleLine
            (clipBottom,clipRight,clipTop,clipLeft,1,resolvedText,drawY + 6,headerPenX);
  factionIndex = 1;
  offsetOrPenX = (headerPenX - offsetOrPenX) + rowMetrics->headerBaselineOffsetPixels +
          (rowMetrics->rowAdvancePixels >> 1);
  do {
    if (g_GameFactionRuntimeImage.tail.factionLifecycleStates[factionIndex] != 0) {
      resolvedText = TextResource_Resolve(factionIndex + TEXT_ID_PLAYER_NUMBER_BASE);
      RichTextCommandStream_DrawSingleLine
                (clipBottom,clipRight,clipTop,clipLeft,2,resolvedText,drawY + 6,offsetOrPenX);
      offsetOrPenX = offsetOrPenX + rowMetrics->rowAdvancePixels;
    }
    factionIndex++;
  } while (factionIndex < 8);
  return;
}


/* Address: 0x00517960.
   Results table column types 8..0x11: header headerResourceId, then for each active faction 1..7 the signed
   dword at factionFieldOffset of its faction record, patched into the valueFormatResourceId template.
*/
void FrontendResultsTable_DrawFormattedFactionFieldColumn
          (TextResourceId valueFormatResourceId,TextResourceId headerResourceId,
          FrontendResultsFactionFieldByteOffset factionFieldOffset,UiPixelCoordinate clipBottom,
          UiPixelCoordinate clipRight,UiPixelCoordinate clipTop,UiPixelCoordinate clipLeft,
          UiPixelCoordinate drawY,UiPixelCoordinate drawX,FrontendResultsRowMetrics *rowMetrics)

{
  int offsetOrPenX;
  uint32_t factionIndex;
  uint8_t *factionFieldCursor;
  int headerPenX;
  uint16_t *resolvedText;
  
  resolvedText = TextResource_Resolve(headerResourceId);
  offsetOrPenX = rowMetrics->headerBaselineOffsetPixels + -4;
  headerPenX = drawX + offsetOrPenX;
  RichTextCommandStream_DrawSingleLine
            (clipBottom,clipRight,clipTop,clipLeft,1,resolvedText,drawY + 6,headerPenX);
  factionIndex = 1;
  offsetOrPenX = (headerPenX - offsetOrPenX) + rowMetrics->headerBaselineOffsetPixels +
          (rowMetrics->rowAdvancePixels >> 1);
  /* the field at byte offset factionFieldOffset of faction record 1 */
  factionFieldCursor = (uint8_t *)&g_GameFactionRuntimeImage.records[1] + factionFieldOffset;
  do {
    if (g_GameFactionRuntimeImage.tail.factionLifecycleStates[factionIndex] != 0) {
      g_WideNumberFormatUtf16
                (WIDE_FORMAT_WRITE_TERMINATOR|WIDE_FORMAT_SIGNED_VALUE,0,10,1,*(int32_t *)factionFieldCursor,
                 (uint16_t *)&g_FrontendResultsValueTextUtf16);
      resolvedText = TextResource_Resolve(valueFormatResourceId);
      RichTextCommandStream_PatchPayloadBySelector(0,&g_FrontendResultsValueTextUtf16,resolvedText);
      RichTextCommandStream_DrawSingleLine
                (clipBottom,clipRight,clipTop,clipLeft,2,resolvedText,drawY + 6,offsetOrPenX);
      offsetOrPenX = offsetOrPenX + rowMetrics->rowAdvancePixels;
    }
    factionIndex++;
    factionFieldCursor = factionFieldCursor + GAME_FACTION_RUNTIME_RECORD_BYTES;
  } while (factionIndex < 8);
  return;
}


/* Address: 0x00517A30.
   Results table column type 5: header TEXT_ID_RESULTS_POINTS, then for each active faction 1..7 its points,
   the sum of the economy (+0x90) and military (+0x94) values of its faction record.
*/
void FrontendResultsTable_DrawPointsColumn
          (UiPixelCoordinate clipBottom,UiPixelCoordinate clipRight,UiPixelCoordinate clipTop,
          UiPixelCoordinate clipLeft,UiPixelCoordinate drawY,UiPixelCoordinate drawX,
          FrontendResultsRowMetrics *rowMetrics)

{
  int offsetOrPenX;
  uint32_t factionIndex;
  GameFactionRuntimeRecord *factionRecord;
  int headerPenX;
  uint16_t *resolvedText;
  
  resolvedText = TextResource_Resolve(TEXT_ID_RESULTS_POINTS);
  offsetOrPenX = rowMetrics->headerBaselineOffsetPixels + -4;
  headerPenX = drawX + offsetOrPenX;
  RichTextCommandStream_DrawSingleLine
            (clipBottom,clipRight,clipTop,clipLeft,1,resolvedText,drawY + 6,headerPenX);
  factionIndex = 1;
  factionRecord = &g_GameFactionRuntimeImage.records[1];
  offsetOrPenX = (headerPenX - offsetOrPenX) + rowMetrics->headerBaselineOffsetPixels +
          (rowMetrics->rowAdvancePixels >> 1);
  do {
    if (g_GameFactionRuntimeImage.tail.factionLifecycleStates[factionIndex] != 0) {
      g_WideNumberFormatUtf16
                (WIDE_FORMAT_WRITE_TERMINATOR|WIDE_FORMAT_SIGNED_VALUE,0,10,1,
                 factionRecord->economyProgressScore + factionRecord->relationScore,
                 (uint16_t *)&g_FrontendResultsValueTextUtf16);
      resolvedText = TextResource_Resolve(TEXT_ID_RESULTS_VALUE_TEMPLATE);
      RichTextCommandStream_PatchPayloadBySelector(0,&g_FrontendResultsValueTextUtf16,resolvedText);
      RichTextCommandStream_DrawSingleLine
                (clipBottom,clipRight,clipTop,clipLeft,2,resolvedText,drawY + 6,offsetOrPenX);
      offsetOrPenX = offsetOrPenX + rowMetrics->rowAdvancePixels;
    }
    factionIndex++;
    factionRecord++;
  } while (factionIndex < 8);
  return;
}


/* Address: 0x00517B10.
   Results table column type 3: header TEXT_ID_RESULTS_ECONOMY, then for each active faction 1..7 the economy
   value at +0x90 of its faction record.
*/
void FrontendResultsTable_DrawEconomyColumn
          (UiPixelCoordinate clipBottom,UiPixelCoordinate clipRight,UiPixelCoordinate clipTop,
          UiPixelCoordinate clipLeft,UiPixelCoordinate drawY,UiPixelCoordinate drawX,
          FrontendResultsRowMetrics *rowMetrics)

{
  int offsetOrPenX;
  uint32_t factionIndex;
  GameFactionRuntimeRecord *factionRecord;
  int headerPenX;
  uint16_t *resolvedText;
  
  resolvedText = TextResource_Resolve(TEXT_ID_RESULTS_ECONOMY);
  offsetOrPenX = rowMetrics->headerBaselineOffsetPixels + -4;
  headerPenX = drawX + offsetOrPenX;
  RichTextCommandStream_DrawSingleLine
            (clipBottom,clipRight,clipTop,clipLeft,1,resolvedText,drawY + 6,headerPenX);
  factionIndex = 1;
  factionRecord = &g_GameFactionRuntimeImage.records[1];
  offsetOrPenX = (headerPenX - offsetOrPenX) + rowMetrics->headerBaselineOffsetPixels +
          (rowMetrics->rowAdvancePixels >> 1);
  do {
    if (g_GameFactionRuntimeImage.tail.factionLifecycleStates[factionIndex] != 0) {
      g_WideNumberFormatUtf16
                (WIDE_FORMAT_WRITE_TERMINATOR|WIDE_FORMAT_SIGNED_VALUE,0,10,1,
                 factionRecord->economyProgressScore,(uint16_t *)&g_FrontendResultsValueTextUtf16);
      resolvedText = TextResource_Resolve(TEXT_ID_RESULTS_VALUE_TEMPLATE);
      RichTextCommandStream_PatchPayloadBySelector(0,&g_FrontendResultsValueTextUtf16,resolvedText);
      RichTextCommandStream_DrawSingleLine
                (clipBottom,clipRight,clipTop,clipLeft,2,resolvedText,drawY + 6,offsetOrPenX);
      offsetOrPenX = offsetOrPenX + rowMetrics->rowAdvancePixels;
    }
    factionIndex++;
    factionRecord++;
  } while (factionIndex < 8);
  return;
}


/* Address: 0x00517BF0.
   Results table column type 4: header TEXT_ID_RESULTS_MILITARY, then for each active faction 1..7 the military
   value at +0x94 of its faction record.
*/
void FrontendResultsTable_DrawMilitaryColumn
          (UiPixelCoordinate clipBottom,UiPixelCoordinate clipRight,UiPixelCoordinate clipTop,
          UiPixelCoordinate clipLeft,UiPixelCoordinate drawY,UiPixelCoordinate drawX,
          FrontendResultsRowMetrics *rowMetrics)

{
  int offsetOrPenX;
  uint32_t factionIndex;
  GameFactionRuntimeRecord *factionRecord;
  int headerPenX;
  uint16_t *resolvedText;
  
  resolvedText = TextResource_Resolve(TEXT_ID_RESULTS_MILITARY);
  offsetOrPenX = rowMetrics->headerBaselineOffsetPixels + -4;
  headerPenX = drawX + offsetOrPenX;
  RichTextCommandStream_DrawSingleLine
            (clipBottom,clipRight,clipTop,clipLeft,1,resolvedText,drawY + 6,headerPenX);
  factionIndex = 1;
  factionRecord = &g_GameFactionRuntimeImage.records[1];
  offsetOrPenX = (headerPenX - offsetOrPenX) + rowMetrics->headerBaselineOffsetPixels +
          (rowMetrics->rowAdvancePixels >> 1);
  do {
    if (g_GameFactionRuntimeImage.tail.factionLifecycleStates[factionIndex] != 0) {
      g_WideNumberFormatUtf16
                (WIDE_FORMAT_WRITE_TERMINATOR|WIDE_FORMAT_SIGNED_VALUE,0,10,1,
                 factionRecord->relationScore,(uint16_t *)&g_FrontendResultsValueTextUtf16);
      resolvedText = TextResource_Resolve(TEXT_ID_RESULTS_VALUE_TEMPLATE);
      RichTextCommandStream_PatchPayloadBySelector(0,&g_FrontendResultsValueTextUtf16,resolvedText);
      RichTextCommandStream_DrawSingleLine
                (clipBottom,clipRight,clipTop,clipLeft,2,resolvedText,drawY + 6,offsetOrPenX);
      offsetOrPenX = offsetOrPenX + rowMetrics->rowAdvancePixels;
    }
    factionIndex++;
    factionRecord++;
  } while (factionIndex < 8);
  return;
}


/* Address: 0x00517CD0.
   Results table column type 6: header TEXT_ID_RESULTS_PLAYER, then for each active faction 1..7 the names of
   up to three players assigned to it, 26 pixels apart downwards. The right and left clip bounds of each name
   are narrowed to the faction's cell (cellRightX, cellLeftX), so the names are clipped to their cell.
*/
void FrontendResultsTable_DrawPlayerColumn
          (UiPixelCoordinate clipBottom,UiPixelCoordinate clipRight,UiPixelCoordinate clipTop,
          UiPixelCoordinate clipLeft,UiPixelCoordinate drawY,UiPixelCoordinate drawX,
          FrontendResultsRowMetrics *rowMetrics)

{
  int offsetOrPenX;
  uint32_t factionIndex;
  FrontendPlayerNameUtf16 *playerNameCursor;
  FrontendPlayerRuntimeBlockCount remainingBlocks;
  int nameTopY;
  int coordinateOrAdvance;
  uint16_t *resolvedText;
  UiPixelCoordinate nameClipRight;
  UiPixelCoordinate nameClipLeft;
  uint32_t namesDrawn;
  int cellRightX;
  int cellLeftX;
  
  resolvedText = TextResource_Resolve(TEXT_ID_RESULTS_PLAYER);
  offsetOrPenX = rowMetrics->headerBaselineOffsetPixels + -4;
  coordinateOrAdvance = drawX + offsetOrPenX;
  nameTopY = drawY + 6;
  RichTextCommandStream_DrawSingleLine
            (clipBottom,clipRight,clipTop,clipLeft,1,resolvedText,nameTopY,coordinateOrAdvance);
  cellLeftX = (coordinateOrAdvance - offsetOrPenX) + rowMetrics->headerBaselineOffsetPixels;
  factionIndex = 1;
  cellRightX = rowMetrics->rowAdvancePixels + cellLeftX;
  offsetOrPenX = cellLeftX + (rowMetrics->rowAdvancePixels >> 1);
  do {
    if (g_GameFactionRuntimeImage.tail.factionLifecycleStates[factionIndex] != 0) {
      namesDrawn = 0;
      playerNameCursor = &g_FrontendPlayerRuntimeBlocks->playerName;
      remainingBlocks = g_FrontendPlayerRuntimeBlockCount;
      coordinateOrAdvance = nameTopY;
      do {
        /* playerNameCursor walks the player records by their playerName */
        if (factionIndex == FRONTEND_PLAYER_RECORD_OF_NAME(playerNameCursor)->factionAssignment.factionAssignmentIndex &&
            namesDrawn < 3) {
          namesDrawn++;
          nameClipLeft = clipLeft;
          if (clipLeft < cellLeftX) {
            nameClipLeft = cellLeftX;
          }
          nameClipRight = clipRight;
          if (cellRightX < clipRight) {
            nameClipRight = cellRightX;
          }
          RichTextCommandStream_DrawSingleLine
                    (clipBottom,nameClipRight,clipTop,nameClipLeft,2,playerNameCursor->textUtf16,coordinateOrAdvance,
                     offsetOrPenX);
          coordinateOrAdvance = coordinateOrAdvance + 26;
        }
        /* next player block */
        playerNameCursor = playerNameCursor + sizeof(FrontendPlayerRuntimeRecord) / sizeof(FrontendPlayerNameUtf16);
        remainingBlocks--;
      } while (remainingBlocks != 0);
      coordinateOrAdvance = rowMetrics->rowAdvancePixels;
      offsetOrPenX = offsetOrPenX + coordinateOrAdvance;
      cellLeftX = cellLeftX + coordinateOrAdvance;
      cellRightX = cellRightX + coordinateOrAdvance;
    }
    factionIndex++;
  } while (factionIndex < 8);
  return;
}


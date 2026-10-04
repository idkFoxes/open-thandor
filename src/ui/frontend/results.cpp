/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/src/ui/frontend/results.cpp
 * Reverse engineering by idkFoxes 2026
 */

#include <thandor/ui/frontend/results.h>
#include <thandor/thandor.h>
#include <thandor/platform/bootstrap/image.h>

/* Module data. */

static uint16_t g_FrontendResultsValueTextUtf16[32] = {0};

static const int g_FrontendResultsColumnAdvance00Pixels = 0;

static const int g_FrontendResultsColumnAdvance01Pixels = 0;

static const int g_FrontendResultsColumnAdvanceColourPixels = 26;

static const int g_FrontendResultsColumnAdvanceEconomyPixels = 26;

static const int g_FrontendResultsColumnAdvanceMilitaryPixels = 26;

static const int g_FrontendResultsColumnAdvancePointsPixels = 26;

static const int g_FrontendResultsColumnAdvancePlayerPixels = 78;

static const int g_FrontendResultsColumnAdvanceFactionPixels = 26;

static const int g_FrontendResultsColumnAdvanceFactionField98Pixels = 26;

static const int g_FrontendResultsColumnAdvanceFactionField9CPixels = 26;

static const int g_FrontendResultsColumnAdvanceFactionFieldA0Pixels = 26;

static const int g_FrontendResultsColumnAdvanceFactionFieldA4Pixels = 26;

static const int g_FrontendResultsColumnAdvanceFactionFieldA8Pixels = 26;

static const int g_FrontendResultsColumnAdvanceFactionFieldACPixels = 26;

static const int g_FrontendResultsColumnAdvanceFactionFieldB0Pixels = 26;

static const int g_FrontendResultsColumnAdvanceFactionFieldB4Pixels = 26;

static const int g_FrontendResultsColumnAdvanceFactionFieldB8Pixels = 26;

static const int g_FrontendResultsColumnAdvanceFactionFieldBCPixels = 26;

static uint32_t g_FrontendResultsFramebufferBytesPerPixel = 0;

static uint32_t g_FrontendResultsFramebufferScanlineStrideBytes = 0;

static uint32_t g_FrontendResultsFactionPackedPixelColors[7] = {0};

/* Implementation ownership: ui/frontend/results. */

/* drawClipped of g_FrontendResultsTableVtable, the three results charts (resultsChart1..3) of the end-of-game
   results screen. Table mode (modeFlags bit 0 clear) draws the control's list of column types one after the
   other, advancing by each type's width (types 0 and 1 are empty spacers). The "columns" advance downwards
   (drawY starts at base.top) and each one lays its header and its faction entries out from left to
   right (drawX starts at base.left, the entries advance by rowAdvancePixels in x). Graph mode first converts the colour of
   each faction's colour text (TEXT_ID_FACTION_NAME_BASE + the record's colorIndex) into a packed pixel
   for g_FrontendResultsFactionPackedPixelColors, then draws one pixel column per x through the control's
   factionWeightRaster, each showing the stat table sample at x / width of the game so far.
*/
void FrontendResultsTable_DrawColumnSequenceByType(int clipBottom,int clipRight,int clipTop,int clipLeft,
          FrontendResultsColumnSequenceControl *control)

{
  UiPixelCoordinate drawX;
  int drawY;
  SoftwareFramebufferAccess *framebufferAccess;
  void *statTableImage;
  uint16_t *colourText;
  uint32_t pixelColumn;
  uint32_t pixelColumnCount;
  uint32_t remainingColumns;
  uint32_t factionIndex;
  uint32_t historySampleCount;
  GameFactionRuntimeRecord *factionRecord;
  uint32_t *columnTypeCursor;
  uint32_t *packedColor;

  if ((control->modeFlags & FRONTEND_RESULTS_MODE_GRAPH) == 0) {
    drawX = control->base.left;
    drawY = control->base.top;
    remainingColumns = control->columnTypeCount;
    columnTypeCursor = &control->columnTypes0;
    if (!g_GraphicsFramebufferBeginAccess()) {
      /* Original quirk: a do-while, so a column type count of 0 wraps around instead of drawing nothing. */
      do {
        switch(*columnTypeCursor) {
        case FRONTEND_RESULTS_COLUMN_SPACER0:
          drawY = drawY + g_FrontendResultsColumnAdvance00Pixels;
          break;
        case FRONTEND_RESULTS_COLUMN_SPACER1:
          drawY = drawY + g_FrontendResultsColumnAdvance01Pixels;
          break;
        case FRONTEND_RESULTS_COLUMN_COLOUR:
          FrontendResultsTable_DrawColourColumn
                    (clipBottom,clipRight,clipTop,clipLeft,drawY,drawX,
                     (FrontendResultsRowMetrics *)control);
          drawY = drawY + g_FrontendResultsColumnAdvanceColourPixels;
          break;
        case FRONTEND_RESULTS_COLUMN_ECONOMY:
          FrontendResultsTable_DrawEconomyColumn
                    (clipBottom,clipRight,clipTop,clipLeft,drawY,drawX,
                     (FrontendResultsRowMetrics *)control);
          drawY = drawY + g_FrontendResultsColumnAdvanceEconomyPixels;
          break;
        case FRONTEND_RESULTS_COLUMN_MILITARY:
          FrontendResultsTable_DrawMilitaryColumn
                    (clipBottom,clipRight,clipTop,clipLeft,drawY,drawX,
                     (FrontendResultsRowMetrics *)control);
          drawY = drawY + g_FrontendResultsColumnAdvanceMilitaryPixels;
          break;
        case FRONTEND_RESULTS_COLUMN_POINTS:
          FrontendResultsTable_DrawPointsColumn
                    (clipBottom,clipRight,clipTop,clipLeft,drawY,drawX,
                     (FrontendResultsRowMetrics *)control);
          drawY = drawY + g_FrontendResultsColumnAdvancePointsPixels;
          break;
        case FRONTEND_RESULTS_COLUMN_PLAYER:
          FrontendResultsTable_DrawPlayerColumn
                    (clipBottom,clipRight,clipTop,clipLeft,drawY,drawX,
                     (FrontendResultsRowMetrics *)control);
          drawY = drawY + g_FrontendResultsColumnAdvancePlayerPixels;
          break;
        case FRONTEND_RESULTS_COLUMN_FACTION:
          FrontendResultsTable_DrawFactionColumn
                    (clipBottom,clipRight,clipTop,clipLeft,drawY,drawX,
                     (FrontendResultsRowMetrics *)control);
          drawY = drawY + g_FrontendResultsColumnAdvanceFactionPixels;
          break;
        /* faction record fields exploredTerrainPercent..relationCounterF (value template, header, field offset) */
        case FRONTEND_RESULTS_COLUMN_FACTION_FIELD:
          FrontendResultsTable_DrawFormattedFactionFieldColumn
                    (TEXT_ID_RESULTS_FIELD_VALUE_TEMPLATE1,TEXT_ID_RESULTS_FIELD_HEADER_BASE + 0,
                     offsetof(GameFactionRuntimeRecord,exploredTerrainPercent),clipBottom,clipRight,clipTop,clipLeft,drawY,drawX,
                     (FrontendResultsRowMetrics *)control);
          drawY = drawY + g_FrontendResultsColumnAdvanceFactionField98Pixels;
          break;
        case FRONTEND_RESULTS_COLUMN_FACTION_FIELD + 1:
          FrontendResultsTable_DrawFormattedFactionFieldColumn
                    (TEXT_ID_RESULTS_FIELD_VALUE_TEMPLATE2,TEXT_ID_RESULTS_FIELD_HEADER_BASE + 1,
                     offsetof(GameFactionRuntimeRecord,unlockedTechnologyCountBeyondBaseline),clipBottom,clipRight,clipTop,clipLeft,drawY,drawX,
                     (FrontendResultsRowMetrics *)control);
          drawY = drawY + g_FrontendResultsColumnAdvanceFactionField9CPixels;
          break;
        case FRONTEND_RESULTS_COLUMN_FACTION_FIELD + 2:
          FrontendResultsTable_DrawFormattedFactionFieldColumn
                    (TEXT_ID_RESULTS_FIELD_VALUE_TEMPLATE3,TEXT_ID_RESULTS_FIELD_HEADER_BASE + 2,
                     offsetof(GameFactionRuntimeRecord,primaryResourceComponent),clipBottom,clipRight,clipTop,clipLeft,drawY,drawX,
                     (FrontendResultsRowMetrics *)control);
          drawY = drawY + g_FrontendResultsColumnAdvanceFactionFieldA0Pixels;
          break;
        case FRONTEND_RESULTS_COLUMN_FACTION_FIELD + 3:
          FrontendResultsTable_DrawFormattedFactionFieldColumn
                    (TEXT_ID_RESULTS_FIELD_VALUE_TEMPLATE3,TEXT_ID_RESULTS_FIELD_HEADER_BASE + 3,
                     offsetof(GameFactionRuntimeRecord,secondaryResourceComponent),clipBottom,clipRight,clipTop,clipLeft,drawY,drawX,
                     (FrontendResultsRowMetrics *)control);
          drawY = drawY + g_FrontendResultsColumnAdvanceFactionFieldA4Pixels;
          break;
        case FRONTEND_RESULTS_COLUMN_FACTION_FIELD + 4:
          FrontendResultsTable_DrawFormattedFactionFieldColumn
                    (TEXT_ID_RESULTS_FIELD_VALUE_TEMPLATE2,TEXT_ID_RESULTS_FIELD_HEADER_BASE + 4,
                     offsetof(GameFactionRuntimeRecord,relationCounterA),clipBottom,clipRight,clipTop,clipLeft,drawY,drawX,
                     (FrontendResultsRowMetrics *)control);
          drawY = drawY + g_FrontendResultsColumnAdvanceFactionFieldA8Pixels;
          break;
        case FRONTEND_RESULTS_COLUMN_FACTION_FIELD + 5:
          FrontendResultsTable_DrawFormattedFactionFieldColumn
                    (TEXT_ID_RESULTS_FIELD_VALUE_TEMPLATE2,TEXT_ID_RESULTS_FIELD_HEADER_BASE + 5,
                     offsetof(GameFactionRuntimeRecord,relationCounterB),clipBottom,clipRight,clipTop,clipLeft,drawY,drawX,
                     (FrontendResultsRowMetrics *)control);
          drawY = drawY + g_FrontendResultsColumnAdvanceFactionFieldACPixels;
          break;
        case FRONTEND_RESULTS_COLUMN_FACTION_FIELD + 6:
          FrontendResultsTable_DrawFormattedFactionFieldColumn
                    (TEXT_ID_RESULTS_FIELD_VALUE_TEMPLATE2,TEXT_ID_RESULTS_FIELD_HEADER_BASE + 6,
                     offsetof(GameFactionRuntimeRecord,relationCounterC),clipBottom,clipRight,clipTop,clipLeft,drawY,drawX,
                     (FrontendResultsRowMetrics *)control);
          drawY = drawY + g_FrontendResultsColumnAdvanceFactionFieldB0Pixels;
          break;
        case FRONTEND_RESULTS_COLUMN_FACTION_FIELD + 7:
          FrontendResultsTable_DrawFormattedFactionFieldColumn
                    (TEXT_ID_RESULTS_FIELD_VALUE_TEMPLATE2,TEXT_ID_RESULTS_FIELD_HEADER_BASE + 7,
                     offsetof(GameFactionRuntimeRecord,relationCounterD),clipBottom,clipRight,clipTop,clipLeft,drawY,drawX,
                     (FrontendResultsRowMetrics *)control);
          drawY = drawY + g_FrontendResultsColumnAdvanceFactionFieldB4Pixels;
          break;
        case FRONTEND_RESULTS_COLUMN_FACTION_FIELD + 8:
          FrontendResultsTable_DrawFormattedFactionFieldColumn
                    (TEXT_ID_RESULTS_FIELD_VALUE_TEMPLATE2,TEXT_ID_RESULTS_FIELD_HEADER_BASE + 8,
                     offsetof(GameFactionRuntimeRecord,relationCounterE),clipBottom,clipRight,clipTop,clipLeft,drawY,drawX,
                     (FrontendResultsRowMetrics *)control);
          drawY = drawY + g_FrontendResultsColumnAdvanceFactionFieldB8Pixels;
          break;
        case FRONTEND_RESULTS_COLUMN_FACTION_FIELD + 9:
          FrontendResultsTable_DrawFormattedFactionFieldColumn
                    (TEXT_ID_RESULTS_FIELD_VALUE_TEMPLATE2,TEXT_ID_RESULTS_FIELD_HEADER_BASE + 9,
                     offsetof(GameFactionRuntimeRecord,relationCounterF),clipBottom,clipRight,clipTop,clipLeft,drawY,drawX,
                     (FrontendResultsRowMetrics *)control);
          drawY = drawY + g_FrontendResultsColumnAdvanceFactionFieldBCPixels;
        }
        columnTypeCursor++;
        remainingColumns--;
      } while (remainingColumns != 0);
      g_GraphicsFramebufferEndAccess();
    }
  }
  else {
    /* factions 1..7: the colour text's code units 1..8 hold the colour digits (low nibble each) */
    packedColor = g_FrontendResultsFactionPackedPixelColors;
    factionRecord = &g_GameFactionRuntimeImage.records[1];
    for (factionIndex = 1; factionIndex < 8; factionIndex++) {
      colourText = TextResource_Resolve(factionRecord->colorIndex + TEXT_ID_FACTION_NAME_BASE);
      /* alpha stays in the top byte; red, green and blue are looked up in the pixel pack tables */
      *packedColor = FACTION_COLOUR_TEXT_PAIR_TOP_BYTE(colourText[7],colourText[8]) +
                g_SoftwarePixelPackTables->red[FACTION_COLOUR_TEXT_PAIR_TOP_BYTE(colourText[5],colourText[6]) >> 24] +
                g_SoftwarePixelPackTables->green[FACTION_COLOUR_TEXT_PAIR_TOP_BYTE(colourText[3],colourText[4]) >> 24] +
                g_SoftwarePixelPackTables->blue[FACTION_COLOUR_TEXT_PAIR_TOP_BYTE(colourText[1],colourText[2]) >> 24];
      factionRecord++;
      packedColor++;
    }
    statTableImage = g_GameStatTableImage;
    framebufferAccess = g_FramebufferAccess;
    pixelColumnCount = control->base.layoutWidth;
    historySampleCount = g_GameFactionRuntimeImage.tail.simulationTick >> RESULTS_STAT_SAMPLE_TICK_SHIFT;
    if (!g_GraphicsFramebufferBeginAccess()) {
      g_FrontendResultsFramebufferBytesPerPixel = framebufferAccess->bytesPerPixel;
      g_FrontendResultsFramebufferScanlineStrideBytes =
           framebufferAccess->width * g_FrontendResultsFramebufferBytesPerPixel;
      /* Original quirk: a do-while, so a layout width of 0 wraps around (and divides by 0). */
      pixelColumn = 0;
      do {
        control->factionWeightRaster
                  (control->base.bottom,control->base.top,pixelColumn + control->base.left,
                   (FrontendResultsFactionWeightPair *)
                   ((uint8_t *)statTableImage +
                    (int)(((uint64_t)pixelColumn * (uint64_t)historySampleCount) /
                          (uint64_t)(uint32_t)control->base.layoutWidth) * RESULTS_STAT_SAMPLE_BYTES));
        pixelColumn++;
      } while (pixelColumn != pixelColumnCount);
      g_GraphicsFramebufferEndAccess();
    }
  }
}


/* hitTest of g_UiCommandVisibilityWrappedTextVtable and g_UiCommandVisibilitySingleLineTextVtable: these text
   controls are never hit, so the pointer passes through them (UI_NODE_NONE).
*/
UiNodeBase * FrontendResultsTable_HitTestAlwaysNone
          (UiPixelCoordinate pointerY,UiPixelCoordinate pointerX,UiNodeBase *control)

{
  return UI_NODE_NONE;
}


/* The results graph columns below: true when the sample has no weight even after the active-faction fallback.
   The original divided by the total regardless (a division by zero when no faction 1..7 is active); bounded here
   because the column is then skipped (logged once). */
static Bool8 FrontendResultsGraph_RejectZeroWeightTotal(uint32_t weightTotal)
{
  static Bool8 s_loggedZeroWeightTotal;

  if (weightTotal != 0) {
    return false;
  }
  if (!s_loggedZeroWeightTotal) {
    s_loggedZeroWeightTotal = true;
    Thandor_Log("FrontendResultsGraph: sample without weight and no active faction, column skipped");
  }
  return true;
}

/* factionWeightRaster of resultsChart1 (set in its template): draws one pixel column of the
   stacked results graph from spanStartY to spanEndY, split among factions 1..7 in proportion to the sum of both
   metrics of the stat table sample, each in the faction's colour. When all are 0, every active faction counts
   as 1 (written back into the sample). Every pixel is the faction's packed 32-bit colour.
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
  if (FrontendResultsGraph_RejectZeroWeightTotal(weightTotal)) {
    return;
  }
  pixelCursor = framebufferAccess->pixels +
           (int32_t)((spanStartY * framebufferAccess->width + drawX) * g_FrontendResultsFramebufferBytesPerPixel);
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
      /* a whole 32-bit pixel: the original wrote only a 16-bit word, in 32-bit modes too (dropped with 16-bit colour) */
      do {
        *(uint32_t *)pixelCursor = packedColor;
        pixelCursor = pixelCursor + g_FrontendResultsFramebufferScanlineStrideBytes;
        segmentHeight--;
      } while (segmentHeight != 0);
    }
    factionIndex++;
    factionWeights++;
  } while (factionIndex <= 6);
  return;
}

/* factionWeightRaster of resultsChart2: like FrontendResultsGraph_DrawFactionWeightSumColumn, but from the
   sample's first metric (lane 0, the faction record's combinedProgressScore when sampled) only.
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
  if (FrontendResultsGraph_RejectZeroWeightTotal(weightTotal)) {
    return;
  }
  pixelCursor = framebufferAccess->pixels +
           (int32_t)((spanStartY * framebufferAccess->width + drawX) * g_FrontendResultsFramebufferBytesPerPixel);
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
      /* a whole 32-bit pixel: the original wrote only a 16-bit word, in 32-bit modes too (dropped with 16-bit colour) */
      do {
        *(uint32_t *)pixelCursor = packedColor;
        pixelCursor = pixelCursor + g_FrontendResultsFramebufferScanlineStrideBytes;
        segmentHeight--;
      } while (segmentHeight != 0);
    }
    factionIndex++;
    factionWeights++;
  } while (factionIndex <= 6);
  return;
}

/* factionWeightRaster of resultsChart3: like FrontendResultsGraph_DrawFactionWeightSumColumn, but from the
   sample's second metric (lane 1, the faction record's activeArmyContribution when sampled) only.
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
  if (FrontendResultsGraph_RejectZeroWeightTotal(weightTotal)) {
    return;
  }
  pixelCursor = framebufferAccess->pixels +
           (int32_t)((spanStartY * framebufferAccess->width + drawX) * g_FrontendResultsFramebufferBytesPerPixel);
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
      /* a whole 32-bit pixel: the original wrote only a 16-bit word, in 32-bit modes too (dropped with 16-bit colour) */
      do {
        *(uint32_t *)pixelCursor = packedColor;
        pixelCursor = pixelCursor + g_FrontendResultsFramebufferScanlineStrideBytes;
        segmentHeight--;
      } while (segmentHeight != 0);
    }
    factionIndex++;
    factionWeights++;
  } while (factionIndex <= 6);
  return;
}

/* Shared start of the results table column painters: draws the column header headerResourceId (style 1) at
   x = drawX + headerBaselineOffsetPixels - 4, y = drawY + 6, and returns the left edge of the first faction
   cell, drawX + headerBaselineOffsetPixels. */
static int FrontendResultsTable_DrawColumnHeader
          (TextResourceId headerResourceId,UiPixelCoordinate clipBottom,UiPixelCoordinate clipRight,
          UiPixelCoordinate clipTop,UiPixelCoordinate clipLeft,UiPixelCoordinate drawY,UiPixelCoordinate drawX,
          FrontendResultsRowMetrics *rowMetrics)

{
  uint16_t *headerText;

  headerText = TextResource_Resolve(headerResourceId);
  RichTextCommandStream_DrawSingleLine
            (clipBottom,clipRight,clipTop,clipLeft,1,headerText,drawY + 6,
             drawX + (rowMetrics->headerBaselineOffsetPixels + -4));
  return drawX + rowMetrics->headerBaselineOffsetPixels;
}

/* Results table column type 2 (FrontendResultsTable_DrawColumnSequenceByType): header TEXT_ID_RESULTS_COLOUR,
   then one row per active faction 1..7 with its colour name (TEXT_ID_FACTION_NAME_BASE + the record's
   colorIndex).
*/
void FrontendResultsTable_DrawColourColumn
          (UiPixelCoordinate clipBottom,UiPixelCoordinate clipRight,UiPixelCoordinate clipTop,
          UiPixelCoordinate clipLeft,UiPixelCoordinate drawY,UiPixelCoordinate drawX,
          FrontendResultsRowMetrics *rowMetrics)

{
  int penX;
  uint32_t factionIndex;
  GameFactionRuntimeRecord *factionRecord;
  uint16_t *colourName;

  penX = FrontendResultsTable_DrawColumnHeader
           (TEXT_ID_RESULTS_COLOUR,clipBottom,clipRight,clipTop,clipLeft,drawY,drawX,rowMetrics);
  penX = penX + (rowMetrics->rowAdvancePixels >> 1);
  factionRecord = &g_GameFactionRuntimeImage.records[1];
  for (factionIndex = 1; factionIndex < 8; factionIndex++) {
    if (g_GameFactionRuntimeImage.tail.factionLifecycleStates[factionIndex] != 0) {
      colourName = TextResource_Resolve(factionRecord->colorIndex + TEXT_ID_FACTION_NAME_BASE);
      RichTextCommandStream_DrawSingleLine
                (clipBottom,clipRight,clipTop,clipLeft,2,colourName,drawY + 6,penX);
      penX = penX + rowMetrics->rowAdvancePixels;
    }
    factionRecord++;
  }
}


/* Results table column type 7: header TEXT_ID_RESULTS_FACTION, then the name of each active faction 1..7
   (TEXT_ID_PLAYER_NUMBER_BASE + faction index).
*/
void FrontendResultsTable_DrawFactionColumn
          (UiPixelCoordinate clipBottom,UiPixelCoordinate clipRight,UiPixelCoordinate clipTop,
          UiPixelCoordinate clipLeft,UiPixelCoordinate drawY,UiPixelCoordinate drawX,
          FrontendResultsRowMetrics *rowMetrics)

{
  int penX;
  uint32_t factionIndex;
  uint16_t *factionName;

  penX = FrontendResultsTable_DrawColumnHeader
           (TEXT_ID_RESULTS_FACTION,clipBottom,clipRight,clipTop,clipLeft,drawY,drawX,rowMetrics);
  penX = penX + (rowMetrics->rowAdvancePixels >> 1);
  for (factionIndex = 1; factionIndex < 8; factionIndex++) {
    if (g_GameFactionRuntimeImage.tail.factionLifecycleStates[factionIndex] != 0) {
      factionName = TextResource_Resolve(factionIndex + TEXT_ID_PLAYER_NUMBER_BASE);
      RichTextCommandStream_DrawSingleLine
                (clipBottom,clipRight,clipTop,clipLeft,2,factionName,drawY + 6,penX);
      penX = penX + rowMetrics->rowAdvancePixels;
    }
  }
}


/* Results table column types 8..0x11: header headerResourceId, then for each active faction 1..7 the signed
   dword at factionFieldOffset of its faction record, patched into the valueFormatResourceId template.
*/
void FrontendResultsTable_DrawFormattedFactionFieldColumn
          (TextResourceId valueFormatResourceId,TextResourceId headerResourceId,
          FrontendResultsFactionFieldByteOffset factionFieldOffset,UiPixelCoordinate clipBottom,
          UiPixelCoordinate clipRight,UiPixelCoordinate clipTop,UiPixelCoordinate clipLeft,
          UiPixelCoordinate drawY,UiPixelCoordinate drawX,FrontendResultsRowMetrics *rowMetrics)

{
  int penX;
  uint32_t factionIndex;
  uint8_t *factionFieldCursor;
  uint16_t *valueText;

  penX = FrontendResultsTable_DrawColumnHeader
           (headerResourceId,clipBottom,clipRight,clipTop,clipLeft,drawY,drawX,rowMetrics);
  penX = penX + (rowMetrics->rowAdvancePixels >> 1);
  /* the field at byte offset factionFieldOffset of faction record 1 */
  factionFieldCursor = (uint8_t *)&g_GameFactionRuntimeImage.records[1] + factionFieldOffset;
  for (factionIndex = 1; factionIndex < 8; factionIndex++) {
    if (g_GameFactionRuntimeImage.tail.factionLifecycleStates[factionIndex] != 0) {
      g_WideNumberFormatUtf16
                (WIDE_FORMAT_WRITE_TERMINATOR|WIDE_FORMAT_SIGNED_VALUE,0,10,1,*(int32_t *)factionFieldCursor,
                 g_FrontendResultsValueTextUtf16);
      valueText = TextResource_Resolve(valueFormatResourceId);
      RichTextCommandStream_PatchPayloadBySelector(0,g_FrontendResultsValueTextUtf16,valueText);
      RichTextCommandStream_DrawSingleLine
                (clipBottom,clipRight,clipTop,clipLeft,2,valueText,drawY + 6,penX);
      penX = penX + rowMetrics->rowAdvancePixels;
    }
    factionFieldCursor = factionFieldCursor + GAME_FACTION_RUNTIME_RECORD_BYTES;
  }
}


/* Results table column type 5: header TEXT_ID_RESULTS_POINTS, then for each active faction 1..7 its points,
   the sum of the economy (economyProgressScore) and military (relationScore) values of its faction record.
*/
void FrontendResultsTable_DrawPointsColumn
          (UiPixelCoordinate clipBottom,UiPixelCoordinate clipRight,UiPixelCoordinate clipTop,
          UiPixelCoordinate clipLeft,UiPixelCoordinate drawY,UiPixelCoordinate drawX,
          FrontendResultsRowMetrics *rowMetrics)

{
  int penX;
  uint32_t factionIndex;
  GameFactionRuntimeRecord *factionRecord;
  uint16_t *valueText;

  penX = FrontendResultsTable_DrawColumnHeader
           (TEXT_ID_RESULTS_POINTS,clipBottom,clipRight,clipTop,clipLeft,drawY,drawX,rowMetrics);
  penX = penX + (rowMetrics->rowAdvancePixels >> 1);
  factionRecord = &g_GameFactionRuntimeImage.records[1];
  for (factionIndex = 1; factionIndex < 8; factionIndex++) {
    if (g_GameFactionRuntimeImage.tail.factionLifecycleStates[factionIndex] != 0) {
      g_WideNumberFormatUtf16
                (WIDE_FORMAT_WRITE_TERMINATOR|WIDE_FORMAT_SIGNED_VALUE,0,10,1,
                 factionRecord->economyProgressScore + factionRecord->relationScore,
                 g_FrontendResultsValueTextUtf16);
      valueText = TextResource_Resolve(TEXT_ID_RESULTS_VALUE_TEMPLATE);
      RichTextCommandStream_PatchPayloadBySelector(0,g_FrontendResultsValueTextUtf16,valueText);
      RichTextCommandStream_DrawSingleLine
                (clipBottom,clipRight,clipTop,clipLeft,2,valueText,drawY + 6,penX);
      penX = penX + rowMetrics->rowAdvancePixels;
    }
    factionRecord++;
  }
}


/* Results table column type 3: header TEXT_ID_RESULTS_ECONOMY, then for each active faction 1..7 the economy
   value (economyProgressScore) of its faction record.
*/
void FrontendResultsTable_DrawEconomyColumn
          (UiPixelCoordinate clipBottom,UiPixelCoordinate clipRight,UiPixelCoordinate clipTop,
          UiPixelCoordinate clipLeft,UiPixelCoordinate drawY,UiPixelCoordinate drawX,
          FrontendResultsRowMetrics *rowMetrics)

{
  int penX;
  uint32_t factionIndex;
  GameFactionRuntimeRecord *factionRecord;
  uint16_t *valueText;

  penX = FrontendResultsTable_DrawColumnHeader
           (TEXT_ID_RESULTS_ECONOMY,clipBottom,clipRight,clipTop,clipLeft,drawY,drawX,rowMetrics);
  penX = penX + (rowMetrics->rowAdvancePixels >> 1);
  factionRecord = &g_GameFactionRuntimeImage.records[1];
  for (factionIndex = 1; factionIndex < 8; factionIndex++) {
    if (g_GameFactionRuntimeImage.tail.factionLifecycleStates[factionIndex] != 0) {
      g_WideNumberFormatUtf16
                (WIDE_FORMAT_WRITE_TERMINATOR|WIDE_FORMAT_SIGNED_VALUE,0,10,1,
                 factionRecord->economyProgressScore,g_FrontendResultsValueTextUtf16);
      valueText = TextResource_Resolve(TEXT_ID_RESULTS_VALUE_TEMPLATE);
      RichTextCommandStream_PatchPayloadBySelector(0,g_FrontendResultsValueTextUtf16,valueText);
      RichTextCommandStream_DrawSingleLine
                (clipBottom,clipRight,clipTop,clipLeft,2,valueText,drawY + 6,penX);
      penX = penX + rowMetrics->rowAdvancePixels;
    }
    factionRecord++;
  }
}


/* Results table column type 4: header TEXT_ID_RESULTS_MILITARY, then for each active faction 1..7 the military
   value (relationScore) of its faction record.
*/
void FrontendResultsTable_DrawMilitaryColumn
          (UiPixelCoordinate clipBottom,UiPixelCoordinate clipRight,UiPixelCoordinate clipTop,
          UiPixelCoordinate clipLeft,UiPixelCoordinate drawY,UiPixelCoordinate drawX,
          FrontendResultsRowMetrics *rowMetrics)

{
  int penX;
  uint32_t factionIndex;
  GameFactionRuntimeRecord *factionRecord;
  uint16_t *valueText;

  penX = FrontendResultsTable_DrawColumnHeader
           (TEXT_ID_RESULTS_MILITARY,clipBottom,clipRight,clipTop,clipLeft,drawY,drawX,rowMetrics);
  penX = penX + (rowMetrics->rowAdvancePixels >> 1);
  factionRecord = &g_GameFactionRuntimeImage.records[1];
  for (factionIndex = 1; factionIndex < 8; factionIndex++) {
    if (g_GameFactionRuntimeImage.tail.factionLifecycleStates[factionIndex] != 0) {
      g_WideNumberFormatUtf16
                (WIDE_FORMAT_WRITE_TERMINATOR|WIDE_FORMAT_SIGNED_VALUE,0,10,1,
                 factionRecord->relationScore,g_FrontendResultsValueTextUtf16);
      valueText = TextResource_Resolve(TEXT_ID_RESULTS_VALUE_TEMPLATE);
      RichTextCommandStream_PatchPayloadBySelector(0,g_FrontendResultsValueTextUtf16,valueText);
      RichTextCommandStream_DrawSingleLine
                (clipBottom,clipRight,clipTop,clipLeft,2,valueText,drawY + 6,penX);
      penX = penX + rowMetrics->rowAdvancePixels;
    }
    factionRecord++;
  }
}


/* Results table column type 6: header TEXT_ID_RESULTS_PLAYER, then for each active faction 1..7 the names of
   up to three players assigned to it, 26 pixels apart downwards. The right and left clip bounds of each name
   are narrowed to the faction's cell (cellRightX, cellLeftX), so the names are clipped to their cell.
*/
void FrontendResultsTable_DrawPlayerColumn
          (UiPixelCoordinate clipBottom,UiPixelCoordinate clipRight,UiPixelCoordinate clipTop,
          UiPixelCoordinate clipLeft,UiPixelCoordinate drawY,UiPixelCoordinate drawX,
          FrontendResultsRowMetrics *rowMetrics)

{
  int penX;
  uint32_t factionIndex;
  FrontendPlayerNameUtf16 *playerNameCursor;
  FrontendPlayerRuntimeBlockCount remainingBlocks;
  int nameTopY;
  int nameY;
  int rowAdvance;
  UiPixelCoordinate nameClipRight;
  UiPixelCoordinate nameClipLeft;
  uint32_t namesDrawn;
  int cellRightX;
  int cellLeftX;

  nameTopY = drawY + 6;
  cellLeftX = FrontendResultsTable_DrawColumnHeader
                (TEXT_ID_RESULTS_PLAYER,clipBottom,clipRight,clipTop,clipLeft,drawY,drawX,rowMetrics);
  cellRightX = rowMetrics->rowAdvancePixels + cellLeftX;
  penX = cellLeftX + (rowMetrics->rowAdvancePixels >> 1);
  for (factionIndex = 1; factionIndex < 8; factionIndex++) {
    if (g_GameFactionRuntimeImage.tail.factionLifecycleStates[factionIndex] != 0) {
      namesDrawn = 0;
      playerNameCursor = &g_FrontendPlayerRuntimeBlocks->playerName;
      remainingBlocks = g_FrontendPlayerRuntimeBlockCount;
      nameY = nameTopY;
      /* Original quirk: a do-while, so a player block count of 0 wraps around. */
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
                    (clipBottom,nameClipRight,clipTop,nameClipLeft,2,playerNameCursor->textUtf16,nameY,penX);
          nameY = nameY + 26;
        }
        /* next player block */
        playerNameCursor = playerNameCursor + sizeof(FrontendPlayerRuntimeRecord) / sizeof(FrontendPlayerNameUtf16);
        remainingBlocks--;
      } while (remainingBlocks != 0);
      rowAdvance = rowMetrics->rowAdvancePixels;
      penX = penX + rowAdvance;
      cellLeftX = cellLeftX + rowAdvance;
      cellRightX = cellRightX + rowAdvance;
    }
  }
}


/* Class vtables. */

UiNodeVtable g_FrontendResultsTableVtable = {
        .relocate = THANDOR_FN(UiContainer_RelocateChildren),
        .method04 = THANDOR_FN(UiNode_DefaultMethod04_NoOp),
        .drawClipped = THANDOR_FN(FrontendResultsTable_DrawColumnSequenceByType),
        .layout = THANDOR_FN(UiContainer_LayoutChildren),
        .nonRightPress = THANDOR_FN(UiNode_DefaultNonRightPress),
        .nonRightRelease = THANDOR_FN(UiNode_DefaultNonRightRelease),
        .rightPress = THANDOR_FN(UiNode_ForwardRightPressToParent),
        .rightRelease = THANDOR_FN(UiNode_DefaultRightRelease),
        .nonRightDrag = THANDOR_FN(UiNode_DefaultNonRightDrag),
        .rightDrag = THANDOR_FN(UiNode_DefaultRightDrag),
        .pointerMove = THANDOR_FN(UiNode_DefaultPointerMove),
        .hitTest = THANDOR_FN(UiContainer_HitTestChildren),
        .keyboardEvent = THANDOR_FN(UiNode_DefaultKeyboardEventMoveFocusNext),
        .applyFlags = THANDOR_FN(UiNode_ApplyFlagsRecursive),
        .suppressActionId = THANDOR_FN(UiContainer_SuppressActionId),
        .unsuppressActionId = THANDOR_FN(UiContainer_UnsuppressActionId),
        .tick = THANDOR_FN(UiNode_DefaultTick),
        .pointerWheel = THANDOR_FN(UiNode_ForwardPointerWheelToParent),
};

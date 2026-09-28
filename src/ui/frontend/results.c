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
   other, advancing by each type's width (types 0 and 1 are empty spacers). Graph mode first converts the colour of
   each faction's colour text (TEXT_ID_FACTION_NAME_BASE + colour index at record +0x38) into a packed pixel
   for g_FrontendResultsFactionPackedPixelColors, then draws one pixel column per x through the control's
   factionWeightRaster, each showing the stat table sample at x / width of the game so far.
*/
void FrontendResultsTable_DrawColumnSequenceByType(int clipTop,int clipLeft,int clipBottom,int clipRight,
          FrontendResultsColumnSequenceControl68 *control)

{
  UiPixelCoordinate drawY;
  SoftwareFramebufferAccess *framebufferAccess;
  void *statTableImage;
  uint16_t *colourResource;
  uint32_t pixelColumn;
  uint32_t remainingColumns;
  int drawXOrCount;
  uint32_t historySampleCount;
  int factionRecordAddress;
  uint32_t *columnTypeOrColorCursor;
  bool accessFailed;
  TextResolveResult resolvedText;
  
  if ((control->modeFlags & 1) == 0) {
    drawY = (control->base).left;
    drawXOrCount = (control->base).top;
    remainingColumns = control->columnTypeCount;
    columnTypeOrColorCursor = &control->columnTypes0;
    accessFailed = g_GraphicsFramebufferBeginAccess();
    if (!accessFailed) {
      do {
                    // WARNING: Switch is manually overridden
        switch(*columnTypeOrColorCursor) {
        case 0:
          drawXOrCount = drawXOrCount + g_FrontendResultsColumnAdvance00Pixels;
          break;
        case 1:
          drawXOrCount = drawXOrCount + g_FrontendResultsColumnAdvance01Pixels;
          break;
        case 2:
          FrontendResultsTable_DrawColourColumn
                    (clipTop,clipLeft,clipBottom,clipRight,drawXOrCount,drawY,
                     (FrontendResultsRowMetrics *)control);
          drawXOrCount = drawXOrCount + g_FrontendResultsColumnAdvanceColourPixels;
          break;
        case 3:
          FrontendResultsTable_DrawEconomyColumn
                    (clipTop,clipLeft,clipBottom,clipRight,drawXOrCount,drawY,
                     (FrontendResultsRowMetrics *)control);
          drawXOrCount = drawXOrCount + g_FrontendResultsColumnAdvanceEconomyPixels;
          break;
        case 4:
          FrontendResultsTable_DrawMilitaryColumn
                    (clipTop,clipLeft,clipBottom,clipRight,drawXOrCount,drawY,
                     (FrontendResultsRowMetrics *)control);
          drawXOrCount = drawXOrCount + g_FrontendResultsColumnAdvanceMilitaryPixels;
          break;
        case 5:
          FrontendResultsTable_DrawPointsColumn
                    (clipTop,clipLeft,clipBottom,clipRight,drawXOrCount,drawY,
                     (FrontendResultsRowMetrics *)control);
          drawXOrCount = drawXOrCount + g_FrontendResultsColumnAdvancePointsPixels;
          break;
        case 6:
          FrontendResultsTable_DrawPlayerColumn
                    (clipTop,clipLeft,clipBottom,clipRight,drawXOrCount,drawY,
                     (FrontendResultsRowMetrics *)control);
          drawXOrCount = drawXOrCount + g_FrontendResultsColumnAdvancePlayerPixels;
          break;
        case 7:
          FrontendResultsTable_DrawFactionColumn
                    (clipTop,clipLeft,clipBottom,clipRight,drawXOrCount,drawY,
                     (FrontendResultsRowMetrics *)control);
          drawXOrCount = drawXOrCount + g_FrontendResultsColumnAdvanceFactionPixels;
          break;
        /* 8..0x11: faction record fields +0x98..+0xBC (value template, header, field offset) */
        case 8:
          FrontendResultsTable_DrawFormattedFactionFieldColumn
                    (0x21c1,0x21b6,0x98,clipTop,clipLeft,clipBottom,clipRight,drawXOrCount,drawY,
                     (FrontendResultsRowMetrics *)control);
          drawXOrCount = drawXOrCount + g_FrontendResultsColumnAdvanceFactionField98Pixels;
          break;
        case 9:
          FrontendResultsTable_DrawFormattedFactionFieldColumn
                    (0x21c2,0x21b7,0x9c,clipTop,clipLeft,clipBottom,clipRight,drawXOrCount,drawY,
                     (FrontendResultsRowMetrics *)control);
          drawXOrCount = drawXOrCount + g_FrontendResultsColumnAdvanceFactionField9CPixels;
          break;
        case 10:
          FrontendResultsTable_DrawFormattedFactionFieldColumn
                    (0x21c3,0x21b8,0xa0,clipTop,clipLeft,clipBottom,clipRight,drawXOrCount,drawY,
                     (FrontendResultsRowMetrics *)control);
          drawXOrCount = drawXOrCount + g_FrontendResultsColumnAdvanceFactionFieldA0Pixels;
          break;
        case 0xb:
          FrontendResultsTable_DrawFormattedFactionFieldColumn
                    (0x21c3,0x21b9,0xa4,clipTop,clipLeft,clipBottom,clipRight,drawXOrCount,drawY,
                     (FrontendResultsRowMetrics *)control);
          drawXOrCount = drawXOrCount + g_FrontendResultsColumnAdvanceFactionFieldA4Pixels;
          break;
        case 0xc:
          FrontendResultsTable_DrawFormattedFactionFieldColumn
                    (0x21c2,0x21ba,0xa8,clipTop,clipLeft,clipBottom,clipRight,drawXOrCount,drawY,
                     (FrontendResultsRowMetrics *)control);
          drawXOrCount = drawXOrCount + g_FrontendResultsColumnAdvanceFactionFieldA8Pixels;
          break;
        case 0xd:
          FrontendResultsTable_DrawFormattedFactionFieldColumn
                    (0x21c2,0x21bb,0xac,clipTop,clipLeft,clipBottom,clipRight,drawXOrCount,drawY,
                     (FrontendResultsRowMetrics *)control);
          drawXOrCount = drawXOrCount + g_FrontendResultsColumnAdvanceFactionFieldACPixels;
          break;
        case 0xe:
          FrontendResultsTable_DrawFormattedFactionFieldColumn
                    (0x21c2,0x21bc,0xb0,clipTop,clipLeft,clipBottom,clipRight,drawXOrCount,drawY,
                     (FrontendResultsRowMetrics *)control);
          drawXOrCount = drawXOrCount + g_FrontendResultsColumnAdvanceFactionFieldB0Pixels;
          break;
        case 0xf:
          FrontendResultsTable_DrawFormattedFactionFieldColumn
                    (0x21c2,0x21bd,0xb4,clipTop,clipLeft,clipBottom,clipRight,drawXOrCount,drawY,
                     (FrontendResultsRowMetrics *)control);
          drawXOrCount = drawXOrCount + g_FrontendResultsColumnAdvanceFactionFieldB4Pixels;
          break;
        case 0x10:
          FrontendResultsTable_DrawFormattedFactionFieldColumn
                    (0x21c2,0x21be,0xb8,clipTop,clipLeft,clipBottom,clipRight,drawXOrCount,drawY,
                     (FrontendResultsRowMetrics *)control);
          drawXOrCount = drawXOrCount + g_FrontendResultsColumnAdvanceFactionFieldB8Pixels;
          break;
        case 0x11:
          FrontendResultsTable_DrawFormattedFactionFieldColumn
                    (0x21c2,0x21bf,0xbc,clipTop,clipLeft,clipBottom,clipRight,drawXOrCount,drawY,
                     (FrontendResultsRowMetrics *)control);
          drawXOrCount = drawXOrCount + g_FrontendResultsColumnAdvanceFactionFieldBCPixels;
        }
        columnTypeOrColorCursor++;
        remainingColumns--;
      } while (remainingColumns != 0);
      g_GraphicsFramebufferEndAccess();
    }
  }
  else {
    /* factions 1..7: the colour text's code units 1..8 hold the colour digits (low nibble each) */
    drawXOrCount = 7;
    columnTypeOrColorCursor = g_FrontendResultsFactionPackedPixelColors;
    factionRecordAddress = THANDOR_ADDR(g_GameFactionRuntimeImage,GAME_FACTION_RUNTIME_RECORD_BYTES);
    do {
      resolvedText = TextResource_Resolve(((GameFactionRuntimeRecord *)factionRecordAddress)->factionClassOrMode + TEXT_ID_FACTION_NAME_BASE);
      colourResource = resolvedText.text;
      *columnTypeOrColorCursor = (((uint8_t)colourResource[8] & 0xf) << 0x18 | (uint32_t)(uint8_t)colourResource[7] << 0x1c) +
                *(int *)((int)g_SoftwarePixelPackTables->red +
                        ((((uint8_t)colourResource[6] & 0xf) << 0x18 | (uint32_t)(uint8_t)colourResource[5] << 0x1c) >> 0x16))
                + *(int *)((int)g_SoftwarePixelPackTables->green +
                          ((((uint8_t)colourResource[4] & 0xf) << 0x18 | (uint32_t)(uint8_t)colourResource[3] << 0x1c) >> 0x16
                          )) +
                g_SoftwarePixelPackTables->blue
                [(((uint8_t)colourResource[2] & 0xf) << 0x18 | (uint32_t)(uint8_t)colourResource[1] << 0x1c) >> 0x18];
      statTableImage = g_GameStatTableImage;
      framebufferAccess = g_FramebufferAccess;
      factionRecordAddress = factionRecordAddress + GAME_FACTION_RUNTIME_RECORD_BYTES;
      columnTypeOrColorCursor++;
      drawXOrCount--;
    } while (drawXOrCount != 0);
    drawXOrCount = (control->base).layoutWidth;
    historySampleCount = g_GameFactionRuntimeImage.tail.simulationTick >> RESULTS_STAT_SAMPLE_TICK_SHIFT;
    accessFailed = g_GraphicsFramebufferBeginAccess();
    if (!accessFailed) {
      g_FrontendResultsFramebufferBytesPerPixel = framebufferAccess->bytesPerPixel;
      g_FrontendResultsFramebufferScanlineStrideBytes =
           framebufferAccess->width * g_FrontendResultsFramebufferBytesPerPixel;
      pixelColumn = 0;
      do {
        control->factionWeightRaster
                  ((control->base).bottom,(control->base).top,pixelColumn + (control->base).left,
                   (FrontendResultsFactionWeightPair8 *)
                   ((int)(((uint64_t)pixelColumn * (uint64_t)historySampleCount) /
                         (uint64_t)(uint32_t)(control->base).layoutWidth) * RESULTS_STAT_SAMPLE_BYTES +
                    (int)statTableImage));
        pixelColumn++;
        drawXOrCount--;
      } while (drawXOrCount != 0);
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
          FrontendResultsFactionWeightPair8 *factionWeights)

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
  weightTotal = (*factionWeights).lane0 + factionWeights[1].lane0 + factionWeights[2].lane0 +
          factionWeights[3].lane0 + factionWeights[4].lane0 + factionWeights[5].lane0 +
          factionWeights[6].lane0 +
          (*factionWeights).lane1 + factionWeights[1].lane1 + factionWeights[2].lane1 +
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
    if (6 < factionIndex) {
      return;
    }
  } while( true );
}

/* Address: 0x005175F0.
   factionWeightRaster of resultsChart2: like FrontendResultsGraph_DrawFactionWeightSumColumn, but from the
   sample's first metric (lane 0, faction record +0x88 when sampled) only.
*/
void FrontendResultsGraph_DrawFactionWeightLane0Column
          (UiPixelCoordinate spanEndY,UiPixelCoordinate spanStartY,UiPixelCoordinate drawX,
          FrontendResultsFactionWeightPair8 *factionWeights)

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
  weightTotal = (*factionWeights).lane0 + factionWeights[1].lane0 + factionWeights[2].lane0 +
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
    if (6 < factionIndex) {
      return;
    }
  } while( true );
}

/* Address: 0x005176F0.
   factionWeightRaster of resultsChart3: like FrontendResultsGraph_DrawFactionWeightSumColumn, but from the
   sample's second metric (lane 1, faction record +0x8C when sampled) only.
*/
void FrontendResultsGraph_DrawFactionWeightLane1Column
          (UiPixelCoordinate spanEndY,UiPixelCoordinate spanStartY,UiPixelCoordinate drawX,
          FrontendResultsFactionWeightPair8 *factionWeights)

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
  weightTotal = (*factionWeights).lane1 + factionWeights[1].lane1 + factionWeights[2].lane1 +
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
    if (6 < factionIndex) {
      return;
    }
  } while( true );
}

/* Address: 0x005177F0.
   Results table column type 2 (FrontendResultsTable_DrawColumnSequenceByType): header TEXT_ID_RESULTS_COLOUR,
   then one row per active faction 1..7 with its colour name (TEXT_ID_FACTION_NAME_BASE + colour index at record
   +0x38).
*/
void FrontendResultsTable_DrawColourColumn
          (UiPixelCoordinate clipTop,UiPixelCoordinate clipLeft,UiPixelCoordinate clipBottom,
          UiPixelCoordinate clipRight,UiPixelCoordinate drawX,UiPixelCoordinate drawY,
          FrontendResultsRowMetrics *rowMetrics)

{
  int offsetOrRowY;
  uint32_t factionIndex;
  int factionRecordAddress;
  int baselineY;
  TextResolveResult resolvedText;
  
  resolvedText = TextResource_Resolve(TEXT_ID_RESULTS_COLOUR);
  offsetOrRowY = rowMetrics->headerBaselineOffsetPixels + -4;
  baselineY = drawY + offsetOrRowY;
  RichTextCommandStream_DrawSingleLine
            (clipTop,clipLeft,clipBottom,clipRight,1,resolvedText.text,drawX + 6,baselineY);
  factionIndex = 1;
  factionRecordAddress = THANDOR_ADDR(g_GameFactionRuntimeImage,GAME_FACTION_RUNTIME_RECORD_BYTES);
  offsetOrRowY = (baselineY - offsetOrRowY) + rowMetrics->headerBaselineOffsetPixels +
          (rowMetrics->rowAdvancePixels >> 1);
  do {
    if (g_GameFactionRuntimeImage.tail.factionLifecycleStates[factionIndex] != 0) {
      resolvedText = TextResource_Resolve(((GameFactionRuntimeRecord *)factionRecordAddress)->factionClassOrMode + TEXT_ID_FACTION_NAME_BASE);
      RichTextCommandStream_DrawSingleLine
                (clipTop,clipLeft,clipBottom,clipRight,2,resolvedText.text,drawX + 6,offsetOrRowY);
      offsetOrRowY = offsetOrRowY + rowMetrics->rowAdvancePixels;
    }
    factionIndex++;
    factionRecordAddress = factionRecordAddress + GAME_FACTION_RUNTIME_RECORD_BYTES;
  } while (factionIndex < 8);
  return;
}


/* Address: 0x005178B0.
   Results table column type 7: header TEXT_ID_RESULTS_FACTION, then the name of each active faction 1..7
   (TEXT_ID_PLAYER_NUMBER_BASE + faction index).
*/
void FrontendResultsTable_DrawFactionColumn
          (UiPixelCoordinate clipTop,UiPixelCoordinate clipLeft,UiPixelCoordinate clipBottom,
          UiPixelCoordinate clipRight,UiPixelCoordinate drawX,UiPixelCoordinate drawY,
          FrontendResultsRowMetrics *rowMetrics)

{
  int offsetOrRowY;
  uint32_t factionIndex;
  int baselineY;
  TextResolveResult resolvedText;
  
  resolvedText = TextResource_Resolve(TEXT_ID_RESULTS_FACTION);
  offsetOrRowY = rowMetrics->headerBaselineOffsetPixels + -4;
  baselineY = drawY + offsetOrRowY;
  RichTextCommandStream_DrawSingleLine
            (clipTop,clipLeft,clipBottom,clipRight,1,resolvedText.text,drawX + 6,baselineY);
  factionIndex = 1;
  offsetOrRowY = (baselineY - offsetOrRowY) + rowMetrics->headerBaselineOffsetPixels +
          (rowMetrics->rowAdvancePixels >> 1);
  do {
    if (g_GameFactionRuntimeImage.tail.factionLifecycleStates[factionIndex] != 0) {
      resolvedText = TextResource_Resolve(factionIndex + TEXT_ID_PLAYER_NUMBER_BASE);
      RichTextCommandStream_DrawSingleLine
                (clipTop,clipLeft,clipBottom,clipRight,2,resolvedText.text,drawX + 6,offsetOrRowY);
      offsetOrRowY = offsetOrRowY + rowMetrics->rowAdvancePixels;
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
          FrontendResultsFactionFieldByteOffset factionFieldOffset,UiPixelCoordinate clipTop,
          UiPixelCoordinate clipLeft,UiPixelCoordinate clipBottom,UiPixelCoordinate clipRight,
          UiPixelCoordinate drawX,UiPixelCoordinate drawY,FrontendResultsRowMetrics *rowMetrics)

{
  int offsetOrRowY;
  uint32_t factionIndex;
  uint8_t *factionFieldCursor;
  int baselineY;
  TextResolveResult resolvedText;
  
  resolvedText = TextResource_Resolve(headerResourceId);
  offsetOrRowY = rowMetrics->headerBaselineOffsetPixels + -4;
  baselineY = drawY + offsetOrRowY;
  RichTextCommandStream_DrawSingleLine
            (clipTop,clipLeft,clipBottom,clipRight,1,resolvedText.text,drawX + 6,baselineY);
  factionIndex = 1;
  offsetOrRowY = (baselineY - offsetOrRowY) + rowMetrics->headerBaselineOffsetPixels +
          (rowMetrics->rowAdvancePixels >> 1);
  /* the field at byte offset factionFieldOffset of faction record 1 */
  factionFieldCursor = (uint8_t *)&g_GameFactionRuntimeImage.records[1] + factionFieldOffset;
  do {
    if (g_GameFactionRuntimeImage.tail.factionLifecycleStates[factionIndex] != 0) {
      g_WideNumberFormatUtf16
                (WIDE_FORMAT_WRITE_TERMINATOR|WIDE_FORMAT_SIGNED_VALUE,0,10,1,*(int32_t *)factionFieldCursor,
                 (uint16_t *)&g_FrontendResultsValueTextUtf16);
      resolvedText = TextResource_Resolve(valueFormatResourceId);
      RichTextCommandStream_PatchPayloadBySelector(0,&g_FrontendResultsValueTextUtf16,resolvedText.text);
      RichTextCommandStream_DrawSingleLine
                (clipTop,clipLeft,clipBottom,clipRight,2,resolvedText.text,drawX + 6,offsetOrRowY);
      offsetOrRowY = offsetOrRowY + rowMetrics->rowAdvancePixels;
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
          (UiPixelCoordinate clipTop,UiPixelCoordinate clipLeft,UiPixelCoordinate clipBottom,
          UiPixelCoordinate clipRight,UiPixelCoordinate drawX,UiPixelCoordinate drawY,
          FrontendResultsRowMetrics *rowMetrics)

{
  int offsetOrRowY;
  uint32_t factionIndex;
  int factionRecordAddress;
  int baselineY;
  TextResolveResult resolvedText;
  
  resolvedText = TextResource_Resolve(TEXT_ID_RESULTS_POINTS);
  offsetOrRowY = rowMetrics->headerBaselineOffsetPixels + -4;
  baselineY = drawY + offsetOrRowY;
  RichTextCommandStream_DrawSingleLine
            (clipTop,clipLeft,clipBottom,clipRight,1,resolvedText.text,drawX + 6,baselineY);
  factionIndex = 1;
  factionRecordAddress = THANDOR_ADDR(g_GameFactionRuntimeImage,GAME_FACTION_RUNTIME_RECORD_BYTES);
  offsetOrRowY = (baselineY - offsetOrRowY) + rowMetrics->headerBaselineOffsetPixels +
          (rowMetrics->rowAdvancePixels >> 1);
  do {
    if (g_GameFactionRuntimeImage.tail.factionLifecycleStates[factionIndex] != 0) {
      g_WideNumberFormatUtf16
                (WIDE_FORMAT_WRITE_TERMINATOR|WIDE_FORMAT_SIGNED_VALUE,0,10,1,
                 ((GameFactionRuntimeRecord *)factionRecordAddress)->economyProgressScore + ((GameFactionRuntimeRecord *)factionRecordAddress)->relationScore,
                 (uint16_t *)&g_FrontendResultsValueTextUtf16);
      resolvedText = TextResource_Resolve(TEXT_ID_RESULTS_VALUE_TEMPLATE);
      RichTextCommandStream_PatchPayloadBySelector(0,&g_FrontendResultsValueTextUtf16,resolvedText.text);
      RichTextCommandStream_DrawSingleLine
                (clipTop,clipLeft,clipBottom,clipRight,2,resolvedText.text,drawX + 6,offsetOrRowY);
      offsetOrRowY = offsetOrRowY + rowMetrics->rowAdvancePixels;
    }
    factionIndex++;
    factionRecordAddress = factionRecordAddress + GAME_FACTION_RUNTIME_RECORD_BYTES;
  } while (factionIndex < 8);
  return;
}


/* Address: 0x00517B10.
   Results table column type 3: header TEXT_ID_RESULTS_ECONOMY, then for each active faction 1..7 the economy
   value at +0x90 of its faction record.
*/
void FrontendResultsTable_DrawEconomyColumn
          (UiPixelCoordinate clipTop,UiPixelCoordinate clipLeft,UiPixelCoordinate clipBottom,
          UiPixelCoordinate clipRight,UiPixelCoordinate drawX,UiPixelCoordinate drawY,
          FrontendResultsRowMetrics *rowMetrics)

{
  int offsetOrRowY;
  uint32_t factionIndex;
  int factionRecordAddress;
  int baselineY;
  TextResolveResult resolvedText;
  
  resolvedText = TextResource_Resolve(TEXT_ID_RESULTS_ECONOMY);
  offsetOrRowY = rowMetrics->headerBaselineOffsetPixels + -4;
  baselineY = drawY + offsetOrRowY;
  RichTextCommandStream_DrawSingleLine
            (clipTop,clipLeft,clipBottom,clipRight,1,resolvedText.text,drawX + 6,baselineY);
  factionIndex = 1;
  factionRecordAddress = THANDOR_ADDR(g_GameFactionRuntimeImage,GAME_FACTION_RUNTIME_RECORD_BYTES);
  offsetOrRowY = (baselineY - offsetOrRowY) + rowMetrics->headerBaselineOffsetPixels +
          (rowMetrics->rowAdvancePixels >> 1);
  do {
    if (g_GameFactionRuntimeImage.tail.factionLifecycleStates[factionIndex] != 0) {
      g_WideNumberFormatUtf16
                (WIDE_FORMAT_WRITE_TERMINATOR|WIDE_FORMAT_SIGNED_VALUE,0,10,1,
                 ((GameFactionRuntimeRecord *)factionRecordAddress)->economyProgressScore,(uint16_t *)&g_FrontendResultsValueTextUtf16);
      resolvedText = TextResource_Resolve(TEXT_ID_RESULTS_VALUE_TEMPLATE);
      RichTextCommandStream_PatchPayloadBySelector(0,&g_FrontendResultsValueTextUtf16,resolvedText.text);
      RichTextCommandStream_DrawSingleLine
                (clipTop,clipLeft,clipBottom,clipRight,2,resolvedText.text,drawX + 6,offsetOrRowY);
      offsetOrRowY = offsetOrRowY + rowMetrics->rowAdvancePixels;
    }
    factionIndex++;
    factionRecordAddress = factionRecordAddress + GAME_FACTION_RUNTIME_RECORD_BYTES;
  } while (factionIndex < 8);
  return;
}


/* Address: 0x00517BF0.
   Results table column type 4: header TEXT_ID_RESULTS_MILITARY, then for each active faction 1..7 the military
   value at +0x94 of its faction record.
*/
void FrontendResultsTable_DrawMilitaryColumn
          (UiPixelCoordinate clipTop,UiPixelCoordinate clipLeft,UiPixelCoordinate clipBottom,
          UiPixelCoordinate clipRight,UiPixelCoordinate drawX,UiPixelCoordinate drawY,
          FrontendResultsRowMetrics *rowMetrics)

{
  int offsetOrRowY;
  uint32_t factionIndex;
  int factionRecordAddress;
  int baselineY;
  TextResolveResult resolvedText;
  
  resolvedText = TextResource_Resolve(TEXT_ID_RESULTS_MILITARY);
  offsetOrRowY = rowMetrics->headerBaselineOffsetPixels + -4;
  baselineY = drawY + offsetOrRowY;
  RichTextCommandStream_DrawSingleLine
            (clipTop,clipLeft,clipBottom,clipRight,1,resolvedText.text,drawX + 6,baselineY);
  factionIndex = 1;
  factionRecordAddress = THANDOR_ADDR(g_GameFactionRuntimeImage,GAME_FACTION_RUNTIME_RECORD_BYTES);
  offsetOrRowY = (baselineY - offsetOrRowY) + rowMetrics->headerBaselineOffsetPixels +
          (rowMetrics->rowAdvancePixels >> 1);
  do {
    if (g_GameFactionRuntimeImage.tail.factionLifecycleStates[factionIndex] != 0) {
      g_WideNumberFormatUtf16
                (WIDE_FORMAT_WRITE_TERMINATOR|WIDE_FORMAT_SIGNED_VALUE,0,10,1,
                 ((GameFactionRuntimeRecord *)factionRecordAddress)->relationScore,(uint16_t *)&g_FrontendResultsValueTextUtf16);
      resolvedText = TextResource_Resolve(TEXT_ID_RESULTS_VALUE_TEMPLATE);
      RichTextCommandStream_PatchPayloadBySelector(0,&g_FrontendResultsValueTextUtf16,resolvedText.text);
      RichTextCommandStream_DrawSingleLine
                (clipTop,clipLeft,clipBottom,clipRight,2,resolvedText.text,drawX + 6,offsetOrRowY);
      offsetOrRowY = offsetOrRowY + rowMetrics->rowAdvancePixels;
    }
    factionIndex++;
    factionRecordAddress = factionRecordAddress + GAME_FACTION_RUNTIME_RECORD_BYTES;
  } while (factionIndex < 8);
  return;
}


/* Address: 0x00517CD0.
   Results table column type 6: header TEXT_ID_RESULTS_PLAYER, then for each active faction 1..7 the names of
   up to three players assigned to it, 26 pixels apart. The second and fourth clip bounds of each name are
   narrowed to the row (rowBottomY, rowTopY), so the names are clipped to their row.
*/
void FrontendResultsTable_DrawPlayerColumn
          (UiPixelCoordinate clipTop,UiPixelCoordinate clipLeft,UiPixelCoordinate clipBottom,
          UiPixelCoordinate clipRight,UiPixelCoordinate drawX,UiPixelCoordinate drawY,
          FrontendResultsRowMetrics *rowMetrics)

{
  int offsetOrRowY;
  uint32_t factionIndex;
  FrontendPlayerNameUtf16_28 *playerNameCursor;
  FrontendPlayerRuntimeBlockCount remainingBlocks;
  int nameDrawX;
  int coordinateOrAdvance;
  TextResolveResult resolvedText;
  UiPixelCoordinate nameClipLeft;
  UiPixelCoordinate nameClipRight;
  uint32_t namesDrawn;
  int rowBottomY;
  int rowTopY;
  
  resolvedText = TextResource_Resolve(TEXT_ID_RESULTS_PLAYER);
  offsetOrRowY = rowMetrics->headerBaselineOffsetPixels + -4;
  coordinateOrAdvance = drawY + offsetOrRowY;
  nameDrawX = drawX + 6;
  RichTextCommandStream_DrawSingleLine
            (clipTop,clipLeft,clipBottom,clipRight,1,resolvedText.text,nameDrawX,coordinateOrAdvance);
  rowTopY = (coordinateOrAdvance - offsetOrRowY) + rowMetrics->headerBaselineOffsetPixels;
  factionIndex = 1;
  rowBottomY = rowMetrics->rowAdvancePixels + rowTopY;
  offsetOrRowY = rowTopY + (rowMetrics->rowAdvancePixels >> 1);
  do {
    if (g_GameFactionRuntimeImage.tail.factionLifecycleStates[factionIndex] != 0) {
      namesDrawn = 0;
      playerNameCursor = &g_FrontendPlayerRuntimeBlocks->playerName;
      remainingBlocks = g_FrontendPlayerRuntimeBlockCount;
      coordinateOrAdvance = nameDrawX;
      do {
        /* playerNameCursor walks the player records by their playerName */
        if ((factionIndex ==
             ((FrontendPlayerRuntimeRecord *)((uint8_t *)playerNameCursor - offsetof(FrontendPlayerRuntimeRecord,playerName)))
             ->factionAssignment.factionAssignmentIndex) &&
           (namesDrawn < 3)) {
          namesDrawn++;
          nameClipRight = clipRight;
          if (clipRight < rowTopY) {
            nameClipRight = rowTopY;
          }
          nameClipLeft = clipLeft;
          if (rowBottomY < clipLeft) {
            nameClipLeft = rowBottomY;
          }
          RichTextCommandStream_DrawSingleLine
                    (clipTop,nameClipLeft,clipBottom,nameClipRight,2,playerNameCursor->textUtf16,coordinateOrAdvance,
                     offsetOrRowY);
          coordinateOrAdvance = coordinateOrAdvance + 0x1a;
        }
        playerNameCursor = playerNameCursor + 0x7e; /* next player block: 0x13B0 bytes */
        remainingBlocks--;
      } while (remainingBlocks != 0);
      coordinateOrAdvance = rowMetrics->rowAdvancePixels;
      offsetOrRowY = offsetOrRowY + coordinateOrAdvance;
      rowTopY = rowTopY + coordinateOrAdvance;
      rowBottomY = rowBottomY + coordinateOrAdvance;
    }
    factionIndex++;
  } while (factionIndex < 8);
  return;
}


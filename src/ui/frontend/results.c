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
   Ownership: ui/frontend/results.
   Purpose: The 18-entry table at 00517070 is a FrontendResultsColumnType computed-jump dispatch table whose
   targets are interior labels, not independent functions. [VERSIONLESS_CANONICAL_DATATYPE_CLOSURE] Retired
   detached enum dictionary FrontendResultsColumnType after transferring its complete value vocabulary to code
   annotation. It is not a safe whole-value storage type. Values: 0=FRONTEND_RESULTS_COLUMN_RESERVED_00,
   1=FRONTEND_RESULTS_COLUMN_RESERVED_01, 2=FRONTEND_RESULTS_COLUMN_COLOUR, 3=FRONTEND_RESULTS_COLUMN_ECONOMY,
   4=FRONTEND_RESULTS_COLUMN_MILITARY, 5=FRONTEND_RESULTS_COLUMN_POINTS, 6=FRONTEND_RESULTS_COLUMN_PLAYER,
   7=FRONTEND_RESULTS_COLUMN_FACTION, 8=FRONTEND_RESULTS_COLUMN_FACTION_FIELD_98,
   9=FRONTEND_RESULTS_COLUMN_FACTION_FIELD_9C, 10=FRONTEND_RESULTS_COLUMN_FACTION_FIELD_A0,
   11=FRONTEND_RESULTS_COLUMN_FACTION_FIELD_A4, 12=FRONTEND_RESULTS_COLUMN_FACTION_FIELD_A8,
   13=FRONTEND_RESULTS_COLUMN_FACTION_FIELD_AC, 14=FRONTEND_RESULTS_COLUMN_FACTION_FIELD_B0,
   15=FRONTEND_RESULTS_COLUMN_FACTION_FIELD_B4, 16=FRONTEND_RESULTS_COLUMN_FACTION_FIELD_B8,
   17=FRONTEND_RESULTS_COLUMN_FACTION_FIELD_BC
   Local calls: FrontendResultsTable_DrawColourColumn, FrontendResultsTable_DrawEconomyColumn,
   FrontendResultsTable_DrawMilitaryColumn, FrontendResultsTable_DrawPointsColumn,
   FrontendResultsTable_DrawPlayerColumn, FrontendResultsTable_DrawFactionColumn,
   FrontendResultsTable_DrawFormattedFactionFieldColumn.
   Cross-module calls: TextResource_Resolve [assets/text/resources].
*/
void __thandor_void_preserve_eax_ecx_edx
FrontendResultsTable_DrawColumnSequenceByType
          (int clipTop,int clipLeft,int clipBottom,int clipRight,
          FrontendResultsColumnSequenceControl68 *control)

{
  UiPixelCoordinate drawY;
  SoftwareFramebufferAccess *framebufferAccess;
  void *statTableImage;
  word *colourResource;
  uint pixelColumn;
  dword remainingColumns;
  int drawXOrCount;
  uint historySampleCount;
  int factionRecordAddress;
  dword *columnTypeOrColorCursor;
  bool accessFailed;
  TextResourceResolveEaxCf5 resolvedText;
  
  if ((control->modeFlags & 1) == 0) {
    drawY = (control->base).left;
    drawXOrCount = (control->base).top;
    remainingColumns = control->columnTypeCount;
    columnTypeOrColorCursor = &control->columnTypes0;
    accessFailed = (*g_GraphicsFramebufferBeginAccess)();
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
        columnTypeOrColorCursor = columnTypeOrColorCursor + 1;
        remainingColumns = remainingColumns - 1;
      } while (remainingColumns != 0);
      (*g_GraphicsFramebufferEndAccess)();
    }
  }
  else {
    drawXOrCount = 7;
    columnTypeOrColorCursor = g_FrontendResultsFactionPackedPixelColors;
    factionRecordAddress = THANDOR_ADDR(g_GameFactionRuntimeImage,0x740);
    do {
      resolvedText = TextResource_Resolve(*(int *)(factionRecordAddress + 0x38) + 0x2173);
      colourResource = resolvedText.eax;
      *columnTypeOrColorCursor = (((byte)colourResource[8] & 0xf) << 0x18 | (uint)(byte)colourResource[7] << 0x1c) +
                *(int *)((int)g_SoftwarePixelPackTables->red +
                        ((((byte)colourResource[6] & 0xf) << 0x18 | (uint)(byte)colourResource[5] << 0x1c) >> 0x16))
                + *(int *)((int)g_SoftwarePixelPackTables->green +
                          ((((byte)colourResource[4] & 0xf) << 0x18 | (uint)(byte)colourResource[3] << 0x1c) >> 0x16
                          )) +
                g_SoftwarePixelPackTables->blue
                [(((byte)colourResource[2] & 0xf) << 0x18 | (uint)(byte)colourResource[1] << 0x1c) >> 0x18];
      statTableImage = g_GameStatTableImage;
      framebufferAccess = g_FramebufferAccess;
      factionRecordAddress = factionRecordAddress + 0x740;
      columnTypeOrColorCursor = columnTypeOrColorCursor + 1;
      drawXOrCount = drawXOrCount + -1;
    } while (drawXOrCount != 0);
    drawXOrCount = (control->base).layoutWidth;
    historySampleCount = g_GameFactionRuntimeImage.tail.simulationTick >> 7;
    accessFailed = (*g_GraphicsFramebufferBeginAccess)();
    if (!accessFailed) {
      g_FrontendResultsFramebufferBytesPerPixel = framebufferAccess->bytesPerPixel;
      g_FrontendResultsFramebufferScanlineStrideBytes =
           framebufferAccess->width * g_FrontendResultsFramebufferBytesPerPixel;
      pixelColumn = 0;
      do {
        (*control->factionWeightRaster)
                  ((control->base).bottom,(control->base).top,pixelColumn + (control->base).left,
                   (FrontendResultsFactionWeightPair8 *)
                   ((int)(((ulonglong)pixelColumn * (ulonglong)historySampleCount) /
                         (ulonglong)(uint)(control->base).layoutWidth) * 0x38 + (int)statTableImage));
        pixelColumn = pixelColumn + 1;
        drawXOrCount = drawXOrCount + -1;
      } while (drawXOrCount != 0);
      (*g_GraphicsFramebufferEndAccess)();
    }
  }
  return;
}


/* Address: 0x00517FB0.
   Ownership: ui/frontend/results.
   Purpose: Shared UiNodeVtable hit-test callback for two frontend-results controls. It consumes the three hit-test
   arguments and returns the 0xFFFFFFFF no-hit sentinel.
*/
UiNodeBase * __thandor_eax_preserve_ecx_edx
FrontendResultsTable_HitTestAlwaysNone
          (UiPixelCoordinate pointerY,UiPixelCoordinate pointerX,UiNodeBase *control)

{
  return (UiNodeBase *)0xffffffff;
}


/* Address: 0x005174E0.
   Ownership: ui/frontend/results.
   Purpose: Draws the faction-weight sum column in the frontend results graph.
*/
void __thandor_void_preserve_eax_ecx_edx
FrontendResultsGraph_DrawFactionWeightSumColumn
          (UiPixelCoordinate spanEndY,UiPixelCoordinate spanStartY,UiPixelCoordinate drawX,
          FrontendResultsFactionWeightPair8 *factionWeights)

{
  dword packedColor;
  SoftwareFramebufferAccess *framebufferAccess;
  uint weightTotal;
  uint factionIndex;
  byte *pixelCursor;
  int drawnHeight;
  int segmentHeight;
  uint cumulativeWeight;
  
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
        weightTotal = weightTotal + 1;
      }
      factionIndex = factionIndex + 1;
    } while (factionIndex < 8);
  }
  pixelCursor = framebufferAccess->pixels +
           (spanStartY * framebufferAccess->width + drawX) * g_FrontendResultsFramebufferBytesPerPixel;
  factionIndex = 0;
  cumulativeWeight = 0;
  drawnHeight = 0;
  do {
    cumulativeWeight = cumulativeWeight + factionWeights->lane0 + factionWeights->lane1;
    segmentHeight = (int)(((ulonglong)cumulativeWeight * (ulonglong)(uint)(spanEndY - spanStartY)) /
                     (ulonglong)weightTotal) - drawnHeight;
    if (segmentHeight != 0) {
      drawnHeight = drawnHeight + segmentHeight;
      packedColor = g_FrontendResultsFactionPackedPixelColors[factionIndex];
      /* The original has a separate entry for bytes-per-pixel != 2, but both paths store 16-bit words. */
      do {
        *(short *)pixelCursor = (short)packedColor;
        pixelCursor = pixelCursor + g_FrontendResultsFramebufferScanlineStrideBytes;
        segmentHeight = segmentHeight + -1;
      } while (segmentHeight != 0);
    }
    factionIndex = factionIndex + 1;
    factionWeights = factionWeights + 1;
    if (6 < factionIndex) {
      return;
    }
  } while( true );
}

/* Address: 0x005175F0.
   Ownership: ui/frontend/results.
   Purpose: Draws faction-weight lane 0 in the frontend results graph.
*/
void __thandor_void_preserve_eax_ecx_edx
FrontendResultsGraph_DrawFactionWeightLane0Column
          (UiPixelCoordinate spanEndY,UiPixelCoordinate spanStartY,UiPixelCoordinate drawX,
          FrontendResultsFactionWeightPair8 *factionWeights)

{
  dword packedColor;
  SoftwareFramebufferAccess *framebufferAccess;
  uint factionIndex;
  byte *pixelCursor;
  int drawnHeight;
  uint weightTotal;
  int segmentHeight;
  uint cumulativeWeight;
  
  framebufferAccess = g_FramebufferAccess;
  weightTotal = (*factionWeights).lane0 + factionWeights[1].lane0 + factionWeights[2].lane0 +
          factionWeights[3].lane0 + factionWeights[4].lane0 + factionWeights[5].lane0 +
          factionWeights[6].lane0;
  if (weightTotal == 0) {
    factionIndex = 1;
    do {
      if (g_GameFactionRuntimeImage.tail.factionLifecycleStates[factionIndex] != 0) {
        factionWeights[factionIndex - 1].lane0 = factionWeights[factionIndex - 1].lane0 + 1;
        weightTotal = weightTotal + 1;
      }
      factionIndex = factionIndex + 1;
    } while (factionIndex < 8);
  }
  pixelCursor = framebufferAccess->pixels +
           (spanStartY * framebufferAccess->width + drawX) * g_FrontendResultsFramebufferBytesPerPixel;
  factionIndex = 0;
  cumulativeWeight = 0;
  drawnHeight = 0;
  do {
    cumulativeWeight = cumulativeWeight + factionWeights->lane0;
    segmentHeight = (int)(((ulonglong)cumulativeWeight * (ulonglong)(uint)(spanEndY - spanStartY)) /
                     (ulonglong)weightTotal) - drawnHeight;
    if (segmentHeight != 0) {
      drawnHeight = drawnHeight + segmentHeight;
      packedColor = g_FrontendResultsFactionPackedPixelColors[factionIndex];
      /* The original has a separate entry for bytes-per-pixel != 2, but both paths store 16-bit words. */
      do {
        *(short *)pixelCursor = (short)packedColor;
        pixelCursor = pixelCursor + g_FrontendResultsFramebufferScanlineStrideBytes;
        segmentHeight = segmentHeight + -1;
      } while (segmentHeight != 0);
    }
    factionIndex = factionIndex + 1;
    factionWeights = factionWeights + 1;
    if (6 < factionIndex) {
      return;
    }
  } while( true );
}

/* Address: 0x005176F0.
   Ownership: ui/frontend/results.
   Purpose: Draws faction-weight lane 1 in the frontend results graph.
*/
void __thandor_void_preserve_eax_ecx_edx
FrontendResultsGraph_DrawFactionWeightLane1Column
          (UiPixelCoordinate spanEndY,UiPixelCoordinate spanStartY,UiPixelCoordinate drawX,
          FrontendResultsFactionWeightPair8 *factionWeights)

{
  dword packedColor;
  SoftwareFramebufferAccess *framebufferAccess;
  uint factionIndex;
  byte *pixelCursor;
  int drawnHeight;
  uint weightTotal;
  int segmentHeight;
  uint cumulativeWeight;
  
  framebufferAccess = g_FramebufferAccess;
  weightTotal = (*factionWeights).lane1 + factionWeights[1].lane1 + factionWeights[2].lane1 +
          factionWeights[3].lane1 + factionWeights[4].lane1 + factionWeights[5].lane1 +
          factionWeights[6].lane1;
  if (weightTotal == 0) {
    factionIndex = 1;
    do {
      if (g_GameFactionRuntimeImage.tail.factionLifecycleStates[factionIndex] != 0) {
        factionWeights[factionIndex - 1].lane1 = factionWeights[factionIndex - 1].lane1 + 1;
        weightTotal = weightTotal + 1;
      }
      factionIndex = factionIndex + 1;
    } while (factionIndex < 8);
  }
  pixelCursor = framebufferAccess->pixels +
           (spanStartY * framebufferAccess->width + drawX) * g_FrontendResultsFramebufferBytesPerPixel;
  factionIndex = 0;
  cumulativeWeight = 0;
  drawnHeight = 0;
  do {
    cumulativeWeight = cumulativeWeight + factionWeights->lane1;
    segmentHeight = (int)(((ulonglong)cumulativeWeight * (ulonglong)(uint)(spanEndY - spanStartY)) /
                     (ulonglong)weightTotal) - drawnHeight;
    if (segmentHeight != 0) {
      drawnHeight = drawnHeight + segmentHeight;
      packedColor = g_FrontendResultsFactionPackedPixelColors[factionIndex];
      /* The original has a separate entry for bytes-per-pixel != 2, but both paths store 16-bit words. */
      do {
        *(short *)pixelCursor = (short)packedColor;
        pixelCursor = pixelCursor + g_FrontendResultsFramebufferScanlineStrideBytes;
        segmentHeight = segmentHeight + -1;
      } while (segmentHeight != 0);
    }
    factionIndex = factionIndex + 1;
    factionWeights = factionWeights + 1;
    if (6 < factionIndex) {
      return;
    }
  } while( true );
}

/* Address: 0x005177F0.
   Ownership: ui/frontend/results.
   Purpose: Draws localized results-table header 0x21B2 and one colour resource per active faction slot 1 through
   7. Typed parameters: p0 clipTop→UiPixelCoordinate_V297, p1 clipLeft→UiPixelCoordinate_V297, p2
   clipBottom→UiPixelCoordinate_V297, p3 clipRight→UiPixelCoordinate_V297, p4 drawX→UiPixelCoordinate_V297, p5
   drawY→UiPixelCoordinate_V297. Calling convention, exact VariableStorage serialization, function body bytes,
   control flow, globals, locals, and executable data remain unchanged.
   Cross-module calls: TextResource_Resolve [assets/text/resources], RichTextCommandStream_DrawSingleLine
   [assets/text/richtext].
*/
void __thandor_void_preserve_eax_ecx_edx
FrontendResultsTable_DrawColourColumn
          (UiPixelCoordinate clipTop,UiPixelCoordinate clipLeft,UiPixelCoordinate clipBottom,
          UiPixelCoordinate clipRight,UiPixelCoordinate drawX,UiPixelCoordinate drawY,
          FrontendResultsRowMetrics *rowMetrics)

{
  int offsetOrRowY;
  uint factionIndex;
  int factionRecordAddress;
  int baselineY;
  TextResourceResolveEaxCf5 resolvedText;
  
  resolvedText = TextResource_Resolve(0x21b2);
  offsetOrRowY = rowMetrics->headerBaselineOffsetPixels + -4;
  baselineY = drawY + offsetOrRowY;
  RichTextCommandStream_DrawSingleLine
            (clipTop,clipLeft,clipBottom,clipRight,1,resolvedText.eax,drawX + 6,baselineY);
  factionIndex = 1;
  factionRecordAddress = THANDOR_ADDR(g_GameFactionRuntimeImage,0x740);
  offsetOrRowY = (baselineY - offsetOrRowY) + rowMetrics->headerBaselineOffsetPixels +
          (rowMetrics->rowAdvancePixels >> 1);
  do {
    if (g_GameFactionRuntimeImage.tail.factionLifecycleStates[factionIndex] != 0) {
      resolvedText = TextResource_Resolve(*(int *)(factionRecordAddress + 0x38) + 0x2173);
      RichTextCommandStream_DrawSingleLine
                (clipTop,clipLeft,clipBottom,clipRight,2,resolvedText.eax,drawX + 6,offsetOrRowY);
      offsetOrRowY = offsetOrRowY + rowMetrics->rowAdvancePixels;
    }
    factionIndex = factionIndex + 1;
    factionRecordAddress = factionRecordAddress + 0x740;
  } while (factionIndex < 8);
  return;
}


/* Address: 0x005178B0.
   Ownership: ui/frontend/results.
   Purpose: Draws localized results-table header 0x21B5 and one localized faction name per active faction slot 1
   through 7. Typed parameters: p0 clipTop→UiPixelCoordinate_V297, p1 clipLeft→UiPixelCoordinate_V297, p2
   clipBottom→UiPixelCoordinate_V297, p3 clipRight→UiPixelCoordinate_V297, p4 drawX→UiPixelCoordinate_V297, p5
   drawY→UiPixelCoordinate_V297. Calling convention, exact VariableStorage serialization, function body bytes,
   control flow, globals, locals, and executable data remain unchanged.
   Cross-module calls: TextResource_Resolve [assets/text/resources], RichTextCommandStream_DrawSingleLine
   [assets/text/richtext].
*/
void __thandor_void_preserve_eax_ecx_edx
FrontendResultsTable_DrawFactionColumn
          (UiPixelCoordinate clipTop,UiPixelCoordinate clipLeft,UiPixelCoordinate clipBottom,
          UiPixelCoordinate clipRight,UiPixelCoordinate drawX,UiPixelCoordinate drawY,
          FrontendResultsRowMetrics *rowMetrics)

{
  int offsetOrRowY;
  uint factionIndex;
  int baselineY;
  TextResourceResolveEaxCf5 resolvedText;
  
  resolvedText = TextResource_Resolve(0x21b5);
  offsetOrRowY = rowMetrics->headerBaselineOffsetPixels + -4;
  baselineY = drawY + offsetOrRowY;
  RichTextCommandStream_DrawSingleLine
            (clipTop,clipLeft,clipBottom,clipRight,1,resolvedText.eax,drawX + 6,baselineY);
  factionIndex = 1;
  offsetOrRowY = (baselineY - offsetOrRowY) + rowMetrics->headerBaselineOffsetPixels +
          (rowMetrics->rowAdvancePixels >> 1);
  do {
    if (g_GameFactionRuntimeImage.tail.factionLifecycleStates[factionIndex] != 0) {
      resolvedText = TextResource_Resolve(factionIndex + 0x2190);
      RichTextCommandStream_DrawSingleLine
                (clipTop,clipLeft,clipBottom,clipRight,2,resolvedText.eax,drawX + 6,offsetOrRowY);
      offsetOrRowY = offsetOrRowY + rowMetrics->rowAdvancePixels;
    }
    factionIndex = factionIndex + 1;
  } while (factionIndex < 8);
  return;
}


/* Address: 0x00517960.
   Ownership: ui/frontend/results.
   Purpose: Draws a caller-selected header and formats one caller-selected signed faction-record field through a
   caller-selected localized template. Typed parameters: p0 valueFormatResourceId→TextResourceId_V338, p1
   headerResourceId→TextResourceId_V338. Nearby but non-identical semantic domains were explicitly deferred.
   Calling convention, parameter storage, body bytes, control flow, globals, locals, and executable data remain
   unchanged. Typed parameters: p2 factionFieldOffset→FrontendResultsFactionFieldByteOffset_V342, p3
   clipTop→UiPixelCoordinate_V297, p4 clipLeft→UiPixelCoordinate_V297, p5 clipBottom→UiPixelCoordinate_V297, p6
   clipRight→UiPixelCoordinate_V297, p7 drawX→UiPixelCoordinate_V297, p8 drawY→UiPixelCoordinate_V297.
   Cross-module calls: TextResource_Resolve [assets/text/resources], RichTextCommandStream_DrawSingleLine
   [assets/text/richtext], RichTextCommandStream_PatchPayloadBySelector [assets/text/richtext].
*/
void __thandor_void_preserve_eax_ecx_edx
FrontendResultsTable_DrawFormattedFactionFieldColumn
          (TextResourceId valueFormatResourceId,TextResourceId headerResourceId,
          FrontendResultsFactionFieldByteOffset factionFieldOffset,UiPixelCoordinate clipTop,
          UiPixelCoordinate clipLeft,UiPixelCoordinate clipBottom,UiPixelCoordinate clipRight,
          UiPixelCoordinate drawX,UiPixelCoordinate drawY,FrontendResultsRowMetrics *rowMetrics)

{
  int offsetOrRowY;
  uint factionIndex;
  byte *factionFieldCursor;
  int baselineY;
  TextResourceResolveEaxCf5 resolvedText;
  
  resolvedText = TextResource_Resolve(headerResourceId);
  offsetOrRowY = rowMetrics->headerBaselineOffsetPixels + -4;
  baselineY = drawY + offsetOrRowY;
  RichTextCommandStream_DrawSingleLine
            (clipTop,clipLeft,clipBottom,clipRight,1,resolvedText.eax,drawX + 6,baselineY);
  factionIndex = 1;
  offsetOrRowY = (baselineY - offsetOrRowY) + rowMetrics->headerBaselineOffsetPixels +
          (rowMetrics->rowAdvancePixels >> 1);
  factionFieldCursor = g_GameFactionRuntimeImage.records[1].reserved78_87 + (factionFieldOffset - 0x78);
  do {
    if (g_GameFactionRuntimeImage.tail.factionLifecycleStates[factionIndex] != 0) {
      (*g_WideNumberFormatUtf16)
                (WIDE_FORMAT_WRITE_TERMINATOR|WIDE_FORMAT_SIGNED_VALUE,0,10,1,*(sdword *)factionFieldCursor,
                 (word *)&g_FrontendResultsValueTextUtf16);
      resolvedText = TextResource_Resolve(valueFormatResourceId);
      RichTextCommandStream_PatchPayloadBySelector(0,&g_FrontendResultsValueTextUtf16,resolvedText.eax);
      RichTextCommandStream_DrawSingleLine
                (clipTop,clipLeft,clipBottom,clipRight,2,resolvedText.eax,drawX + 6,offsetOrRowY);
      offsetOrRowY = offsetOrRowY + rowMetrics->rowAdvancePixels;
    }
    factionIndex = factionIndex + 1;
    factionFieldCursor = factionFieldCursor + 0x740;
  } while (factionIndex < 8);
  return;
}


/* Address: 0x00517A30.
   Ownership: ui/frontend/results.
   Purpose: Draws localized Points header 0x21B3 and formats the sum of faction fields +0x90 and +0x94. Typed
   parameters: p0 clipTop→UiPixelCoordinate_V297, p1 clipLeft→UiPixelCoordinate_V297, p2
   clipBottom→UiPixelCoordinate_V297, p3 clipRight→UiPixelCoordinate_V297, p4 drawX→UiPixelCoordinate_V297, p5
   drawY→UiPixelCoordinate_V297. Calling convention, exact VariableStorage serialization, function body bytes,
   control flow, globals, locals, and executable data remain unchanged.
   Cross-module calls: TextResource_Resolve [assets/text/resources], RichTextCommandStream_DrawSingleLine
   [assets/text/richtext], RichTextCommandStream_PatchPayloadBySelector [assets/text/richtext].
*/
void __thandor_void_preserve_eax_ecx_edx
FrontendResultsTable_DrawPointsColumn
          (UiPixelCoordinate clipTop,UiPixelCoordinate clipLeft,UiPixelCoordinate clipBottom,
          UiPixelCoordinate clipRight,UiPixelCoordinate drawX,UiPixelCoordinate drawY,
          FrontendResultsRowMetrics *rowMetrics)

{
  int offsetOrRowY;
  uint factionIndex;
  int factionRecordAddress;
  int baselineY;
  TextResourceResolveEaxCf5 resolvedText;
  
  resolvedText = TextResource_Resolve(0x21b3);
  offsetOrRowY = rowMetrics->headerBaselineOffsetPixels + -4;
  baselineY = drawY + offsetOrRowY;
  RichTextCommandStream_DrawSingleLine
            (clipTop,clipLeft,clipBottom,clipRight,1,resolvedText.eax,drawX + 6,baselineY);
  factionIndex = 1;
  factionRecordAddress = THANDOR_ADDR(g_GameFactionRuntimeImage,0x740);
  offsetOrRowY = (baselineY - offsetOrRowY) + rowMetrics->headerBaselineOffsetPixels +
          (rowMetrics->rowAdvancePixels >> 1);
  do {
    if (g_GameFactionRuntimeImage.tail.factionLifecycleStates[factionIndex] != 0) {
      (*g_WideNumberFormatUtf16)
                (WIDE_FORMAT_WRITE_TERMINATOR|WIDE_FORMAT_SIGNED_VALUE,0,10,1,
                 *(int *)(factionRecordAddress + 0x90) + *(int *)(factionRecordAddress + 0x94),
                 (word *)&g_FrontendResultsValueTextUtf16);
      resolvedText = TextResource_Resolve(0x21c4);
      RichTextCommandStream_PatchPayloadBySelector(0,&g_FrontendResultsValueTextUtf16,resolvedText.eax);
      RichTextCommandStream_DrawSingleLine
                (clipTop,clipLeft,clipBottom,clipRight,2,resolvedText.eax,drawX + 6,offsetOrRowY);
      offsetOrRowY = offsetOrRowY + rowMetrics->rowAdvancePixels;
    }
    factionIndex = factionIndex + 1;
    factionRecordAddress = factionRecordAddress + 0x740;
  } while (factionIndex < 8);
  return;
}


/* Address: 0x00517B10.
   Ownership: ui/frontend/results.
   Purpose: Draws localized Economy header 0x21B0 and formats faction field +0x90. Typed parameters: p0
   clipTop→UiPixelCoordinate_V297, p1 clipLeft→UiPixelCoordinate_V297, p2 clipBottom→UiPixelCoordinate_V297, p3
   clipRight→UiPixelCoordinate_V297, p4 drawX→UiPixelCoordinate_V297, p5 drawY→UiPixelCoordinate_V297. Calling
   convention, exact VariableStorage serialization, function body bytes, control flow, globals, locals, and
   executable data remain unchanged.
   Cross-module calls: TextResource_Resolve [assets/text/resources], RichTextCommandStream_DrawSingleLine
   [assets/text/richtext], RichTextCommandStream_PatchPayloadBySelector [assets/text/richtext].
*/
void __thandor_void_preserve_eax_ecx_edx
FrontendResultsTable_DrawEconomyColumn
          (UiPixelCoordinate clipTop,UiPixelCoordinate clipLeft,UiPixelCoordinate clipBottom,
          UiPixelCoordinate clipRight,UiPixelCoordinate drawX,UiPixelCoordinate drawY,
          FrontendResultsRowMetrics *rowMetrics)

{
  int offsetOrRowY;
  uint factionIndex;
  int factionRecordAddress;
  int baselineY;
  TextResourceResolveEaxCf5 resolvedText;
  
  resolvedText = TextResource_Resolve(0x21b0);
  offsetOrRowY = rowMetrics->headerBaselineOffsetPixels + -4;
  baselineY = drawY + offsetOrRowY;
  RichTextCommandStream_DrawSingleLine
            (clipTop,clipLeft,clipBottom,clipRight,1,resolvedText.eax,drawX + 6,baselineY);
  factionIndex = 1;
  factionRecordAddress = THANDOR_ADDR(g_GameFactionRuntimeImage,0x740);
  offsetOrRowY = (baselineY - offsetOrRowY) + rowMetrics->headerBaselineOffsetPixels +
          (rowMetrics->rowAdvancePixels >> 1);
  do {
    if (g_GameFactionRuntimeImage.tail.factionLifecycleStates[factionIndex] != 0) {
      (*g_WideNumberFormatUtf16)
                (WIDE_FORMAT_WRITE_TERMINATOR|WIDE_FORMAT_SIGNED_VALUE,0,10,1,
                 *(sdword *)(factionRecordAddress + 0x90),(word *)&g_FrontendResultsValueTextUtf16);
      resolvedText = TextResource_Resolve(0x21c4);
      RichTextCommandStream_PatchPayloadBySelector(0,&g_FrontendResultsValueTextUtf16,resolvedText.eax);
      RichTextCommandStream_DrawSingleLine
                (clipTop,clipLeft,clipBottom,clipRight,2,resolvedText.eax,drawX + 6,offsetOrRowY);
      offsetOrRowY = offsetOrRowY + rowMetrics->rowAdvancePixels;
    }
    factionIndex = factionIndex + 1;
    factionRecordAddress = factionRecordAddress + 0x740;
  } while (factionIndex < 8);
  return;
}


/* Address: 0x00517BF0.
   Ownership: ui/frontend/results.
   Purpose: Draws localized Military header 0x21B1 and formats faction field +0x94. Typed parameters: p0
   clipTop→UiPixelCoordinate_V297, p1 clipLeft→UiPixelCoordinate_V297, p2 clipBottom→UiPixelCoordinate_V297, p3
   clipRight→UiPixelCoordinate_V297, p4 drawX→UiPixelCoordinate_V297, p5 drawY→UiPixelCoordinate_V297. Calling
   convention, exact VariableStorage serialization, function body bytes, control flow, globals, locals, and
   executable data remain unchanged.
   Cross-module calls: TextResource_Resolve [assets/text/resources], RichTextCommandStream_DrawSingleLine
   [assets/text/richtext], RichTextCommandStream_PatchPayloadBySelector [assets/text/richtext].
*/
void __thandor_void_preserve_eax_ecx_edx
FrontendResultsTable_DrawMilitaryColumn
          (UiPixelCoordinate clipTop,UiPixelCoordinate clipLeft,UiPixelCoordinate clipBottom,
          UiPixelCoordinate clipRight,UiPixelCoordinate drawX,UiPixelCoordinate drawY,
          FrontendResultsRowMetrics *rowMetrics)

{
  int offsetOrRowY;
  uint factionIndex;
  int factionRecordAddress;
  int baselineY;
  TextResourceResolveEaxCf5 resolvedText;
  
  resolvedText = TextResource_Resolve(0x21b1);
  offsetOrRowY = rowMetrics->headerBaselineOffsetPixels + -4;
  baselineY = drawY + offsetOrRowY;
  RichTextCommandStream_DrawSingleLine
            (clipTop,clipLeft,clipBottom,clipRight,1,resolvedText.eax,drawX + 6,baselineY);
  factionIndex = 1;
  factionRecordAddress = THANDOR_ADDR(g_GameFactionRuntimeImage,0x740);
  offsetOrRowY = (baselineY - offsetOrRowY) + rowMetrics->headerBaselineOffsetPixels +
          (rowMetrics->rowAdvancePixels >> 1);
  do {
    if (g_GameFactionRuntimeImage.tail.factionLifecycleStates[factionIndex] != 0) {
      (*g_WideNumberFormatUtf16)
                (WIDE_FORMAT_WRITE_TERMINATOR|WIDE_FORMAT_SIGNED_VALUE,0,10,1,
                 *(sdword *)(factionRecordAddress + 0x94),(word *)&g_FrontendResultsValueTextUtf16);
      resolvedText = TextResource_Resolve(0x21c4);
      RichTextCommandStream_PatchPayloadBySelector(0,&g_FrontendResultsValueTextUtf16,resolvedText.eax);
      RichTextCommandStream_DrawSingleLine
                (clipTop,clipLeft,clipBottom,clipRight,2,resolvedText.eax,drawX + 6,offsetOrRowY);
      offsetOrRowY = offsetOrRowY + rowMetrics->rowAdvancePixels;
    }
    factionIndex = factionIndex + 1;
    factionRecordAddress = factionRecordAddress + 0x740;
  } while (factionIndex < 8);
  return;
}


/* Address: 0x00517CD0.
   Ownership: ui/frontend/results.
   Purpose: Draws localized Player header 0x21B4 and up to three matching names from exact 0x13B0-byte frontend
   player blocks. Typed parameters: p0 clipTop→UiPixelCoordinate_V297, p1 clipLeft→UiPixelCoordinate_V297, p2
   clipBottom→UiPixelCoordinate_V297, p3 clipRight→UiPixelCoordinate_V297, p4 drawX→UiPixelCoordinate_V297, p5
   drawY→UiPixelCoordinate_V297. Calling convention, exact VariableStorage serialization, function body bytes,
   control flow, globals, locals, and executable data remain unchanged.
   Cross-module calls: TextResource_Resolve [assets/text/resources], RichTextCommandStream_DrawSingleLine
   [assets/text/richtext].
*/
void __thandor_void_preserve_eax_ecx_edx
FrontendResultsTable_DrawPlayerColumn
          (UiPixelCoordinate clipTop,UiPixelCoordinate clipLeft,UiPixelCoordinate clipBottom,
          UiPixelCoordinate clipRight,UiPixelCoordinate drawX,UiPixelCoordinate drawY,
          FrontendResultsRowMetrics *rowMetrics)

{
  int offsetOrRowY;
  uint factionIndex;
  FrontendPlayerNameUtf16_28 *commandStream;
  FrontendPlayerRuntimeBlockCount remainingBlocks;
  int nameDrawX;
  int coordinateOrAdvance;
  TextResourceResolveEaxCf5 resolvedText;
  UiPixelCoordinate nameClipLeft;
  UiPixelCoordinate nameClipRight;
  uint namesDrawn;
  int rowBottomY;
  int rowTopY;
  
  resolvedText = TextResource_Resolve(0x21b4);
  offsetOrRowY = rowMetrics->headerBaselineOffsetPixels + -4;
  coordinateOrAdvance = drawY + offsetOrRowY;
  nameDrawX = drawX + 6;
  RichTextCommandStream_DrawSingleLine
            (clipTop,clipLeft,clipBottom,clipRight,1,resolvedText.eax,nameDrawX,coordinateOrAdvance);
  rowTopY = (coordinateOrAdvance - offsetOrRowY) + rowMetrics->headerBaselineOffsetPixels;
  factionIndex = 1;
  rowBottomY = rowMetrics->rowAdvancePixels + rowTopY;
  offsetOrRowY = rowTopY + (rowMetrics->rowAdvancePixels >> 1);
  do {
    if (g_GameFactionRuntimeImage.tail.factionLifecycleStates[factionIndex] != 0) {
      namesDrawn = 0;
      commandStream = &g_FrontendPlayerRuntimeBlocks->playerName;
      remainingBlocks = g_FrontendPlayerRuntimeBlockCount;
      coordinateOrAdvance = nameDrawX;
      do {
        if ((factionIndex == *(FrontendFactionAssignmentIndex *)
                       ((int)((UiTransferEndpointDescriptor *)(commandStream + 1) + 1) + 8)) &&
           (namesDrawn < 3)) {
          namesDrawn = namesDrawn + 1;
          nameClipRight = clipRight;
          if (clipRight < rowTopY) {
            nameClipRight = rowTopY;
          }
          nameClipLeft = clipLeft;
          if (rowBottomY < clipLeft) {
            nameClipLeft = rowBottomY;
          }
          RichTextCommandStream_DrawSingleLine
                    (clipTop,nameClipLeft,clipBottom,nameClipRight,2,commandStream->textUtf16,coordinateOrAdvance,
                     offsetOrRowY);
          coordinateOrAdvance = coordinateOrAdvance + 0x1a;
        }
        commandStream = commandStream + 0x7e;
        remainingBlocks = remainingBlocks - 1;
      } while (remainingBlocks != 0);
      coordinateOrAdvance = rowMetrics->rowAdvancePixels;
      offsetOrRowY = offsetOrRowY + coordinateOrAdvance;
      rowTopY = rowTopY + coordinateOrAdvance;
      rowBottomY = rowBottomY + coordinateOrAdvance;
    }
    factionIndex = factionIndex + 1;
  } while (factionIndex < 8);
  return;
}


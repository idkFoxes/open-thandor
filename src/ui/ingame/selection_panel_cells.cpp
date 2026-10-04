/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/src/ui/ingame/selection_panel_cells.cpp
 * Reverse engineering by idkFoxes 2026
 */

#include <thandor/ui/ingame/selection_panel_cells.h>
#include <thandor/thandor.h>

/* Module data. */

/* UiPackedTextStyle 0x01000000 (font 1, palette 0, left aligned) used to measure and draw the numbers in the selection panel */
static const UiPackedTextStyle g_SelectionPanelNumberTextStyle = 16777216;

static uint16_t g_SelectionPanelNumberScratchUtf16[16] = {};

/* The SELECTION_PANEL_CELL_SIZE-byte record of cellIndex in select.dat (fields SELECTION_PANEL_CELL_*). */
static uint8_t *SelectionPanel_GetCellRecord(SelectionPanelCellIndex cellIndex)

{
  return (uint8_t *)g_SelectionPanelData + cellIndex * SELECTION_PANEL_CELL_SIZE;
}

/* Shared tail of the cell draw functions: the coordinates after a cell drawn at (cellY, cellX) with a sprite of
   spriteSize; SELECTION_PANEL_CELL_FLAG_NO_ADVANCE_* keep an axis at the cell position. */
static SelectionPanelCellAdvance SelectionPanel_AdvancePastCell
          (uint8_t *cell,int cellY,int cellX,GraphicsTextureLogicalSize spriteSize)

{
  uint32_t cellFlags;
  uint32_t advanceWidth;
  uint32_t advanceHeight;
  SelectionPanelCellAdvance cellAdvance;

  advanceWidth = spriteSize.logicalWidthPixels;
  advanceHeight = spriteSize.logicalHeightPixels;
  cellFlags = *(uint32_t *)(cell + SELECTION_PANEL_CELL_FLAGS);
  if ((cellFlags & SELECTION_PANEL_CELL_FLAG_NO_ADVANCE_X) != 0) {
    advanceWidth = 0;
  }
  if ((cellFlags & SELECTION_PANEL_CELL_FLAG_NO_ADVANCE_Y) != 0) {
    advanceHeight = 0;
  }
  cellAdvance.nextX = advanceWidth + cellX;
  cellAdvance.nextY = advanceHeight + cellY;
  return cellAdvance;
}

/* Draws a number cell: the cell's sprite at (originY, originX) plus the cell offsets, with value formatted as
   signed decimal text centred on it. Returns the coordinates after the cell (see SelectionPanelCellAdvance;
   SELECTION_PANEL_CELL_FLAG_NO_ADVANCE_* keep an axis at the origin plus offset). Called by
   SelectionPanel_RenderArmyRuntimeMetrics for the group number.
*/
SelectionPanelCellAdvance SelectionPanel_DrawNumberCellAndAdvance
          (UiPixelCoordinate clipBottom,UiPixelCoordinate clipRight,UiPixelCoordinate clipTop,
          UiPixelCoordinate clipLeft,UiPixelCoordinate originY,UiPixelCoordinate originX,
          SelectionPanelNumericValue32 value,SelectionPanelCellIndex cellIndex)

{
  uint8_t *cell;
  uint32_t subresource;
  int cellY;
  int cellX;
  RichTextExtent textExtent;
  GraphicsTextureLogicalSize spriteSize;

  g_WideNumberFormatUtf16
            (WIDE_FORMAT_WRITE_TERMINATOR|WIDE_FORMAT_SIGNED_VALUE,0,15,1,value,
             g_SelectionPanelNumberScratchUtf16);
  textExtent = RichTextCommandStream_MeasureLine
                    (g_SelectionPanelNumberTextStyle,g_SelectionPanelNumberScratchUtf16);
  cell = SelectionPanel_GetCellRecord(cellIndex);
  cellX = originX + *(int *)(cell + SELECTION_PANEL_CELL_OFFSET_X);
  cellY = originY + *(int *)(cell + SELECTION_PANEL_CELL_OFFSET_Y);
  subresource = *(uint32_t *)(cell + SELECTION_PANEL_CELL_BASE_SUBRESOURCE);
  g_SelectionPanelBlitOpaque
            (clipBottom,clipRight,clipTop,clipLeft,cellY,cellX,subresource,g_SelectionPanelTextureSource,
             g_FramebufferAccess);
  spriteSize = g_GraphicsTextureSourceGetLogicalSize(subresource,g_SelectionPanelTextureSource);
  RichTextCommandStream_DrawSingleLine
            (clipBottom,clipRight,clipTop,clipLeft,g_SelectionPanelNumberTextStyle,
             g_SelectionPanelNumberScratchUtf16,
             ((int)(spriteSize.logicalHeightPixels - textExtent.heightPixels) >> 1) + cellY,
             ((int)(spriteSize.logicalWidthPixels - textExtent.widthPixels) >> 1) + cellX);
  return SelectionPanel_AdvancePastCell(cell,cellY,cellX,spriteSize);
}

/* Draws an icon cell: the cell's sprite at (originY, originX) plus the cell offsets. Returns the coordinates
   after the cell (see SelectionPanel_DrawNumberCellAndAdvance). Called by
   SelectionPanel_RenderArmyRuntimeMetrics for the frame corners.
*/
SelectionPanelCellAdvance SelectionPanel_DrawIconCellAndAdvance
          (UiPixelCoordinate clipBottom,UiPixelCoordinate clipRight,UiPixelCoordinate clipTop,
          UiPixelCoordinate clipLeft,UiPixelCoordinate originY,UiPixelCoordinate originX,
          SelectionPanelCellIndex cellIndex)

{
  uint8_t *cell;
  uint32_t subresource;
  int cellY;
  int cellX;
  GraphicsTextureLogicalSize spriteSize;

  cell = SelectionPanel_GetCellRecord(cellIndex);
  subresource = *(uint32_t *)(cell + SELECTION_PANEL_CELL_BASE_SUBRESOURCE);
  cellY = originY + *(int *)(cell + SELECTION_PANEL_CELL_OFFSET_Y);
  cellX = originX + *(int *)(cell + SELECTION_PANEL_CELL_OFFSET_X);
  g_SelectionPanelBlitOpaque
            (clipBottom,clipRight,clipTop,clipLeft,cellY,cellX,subresource,g_SelectionPanelTextureSource,
             g_FramebufferAccess);
  spriteSize = g_GraphicsTextureSourceGetLogicalSize(subresource,g_SelectionPanelTextureSource);
  return SelectionPanel_AdvancePastCell(cell,cellY,cellX,spriteSize);
}

/* Draws a meter cell: the cell's base sprite at (originY, originX) plus the cell offsets and over it frame
   1..17 of the meter (currentValue clamped to 0..maximumValue, rounded to sixteenths; 17 when maximumValue is
   0). Returns the coordinates after the cell (see SelectionPanel_DrawNumberCellAndAdvance). Called by
   SelectionPanel_RenderArmyRuntimeMetrics for the hierarchy meter.
*/
SelectionPanelCellAdvance SelectionPanel_DrawSteppedMeterCellAndAdvance
          (UiPixelCoordinate clipBottom,UiPixelCoordinate clipRight,UiPixelCoordinate clipTop,
          UiPixelCoordinate clipLeft,UiPixelCoordinate originY,UiPixelCoordinate originX,
          UiNumericValue32 maximumValue,UiNumericValue32 currentValue,
          SelectionPanelCellIndex cellIndex)

{
  uint8_t *cell;
  int baseSubresource;
  int meterFrame;
  int cellX;
  int cellY;
  GraphicsTextureLogicalSize spriteSize;

  if (currentValue < 0) {
    currentValue = 0;
  }
  else if (maximumValue < currentValue) {
    currentValue = maximumValue;
  }
  if (maximumValue == 0) {
    meterFrame = 17;
  }
  else {
    /* round(16 * current / maximum) + 1 */
    meterFrame = ((uint32_t)(currentValue * 32 + maximumValue) / (uint32_t)maximumValue >> 1) + 1;
  }
  cell = SelectionPanel_GetCellRecord(cellIndex);
  baseSubresource = *(int *)(cell + SELECTION_PANEL_CELL_BASE_SUBRESOURCE);
  cellX = originX + *(int *)(cell + SELECTION_PANEL_CELL_OFFSET_X);
  cellY = originY + *(int *)(cell + SELECTION_PANEL_CELL_OFFSET_Y);
  g_SelectionPanelBlitOpaque
            (clipBottom,clipRight,clipTop,clipLeft,cellY,cellX,baseSubresource,g_SelectionPanelTextureSource,
             g_FramebufferAccess);
  g_SelectionPanelBlitOpaque
            (clipBottom,clipRight,clipTop,clipLeft,cellY,cellX,meterFrame + baseSubresource,
             g_SelectionPanelTextureSource,g_FramebufferAccess);
  spriteSize = g_GraphicsTextureSourceGetLogicalSize(baseSubresource,g_SelectionPanelTextureSource);
  return SelectionPanel_AdvancePastCell(cell,cellY,cellX,spriteSize);
}

/* numerator / maximumValue rounded to the nearest integer: the quotient is rounded up when twice the remainder
   (as a 32-bit int) exceeds maximumValue. */
static int SelectionPanel_DivideRounded(int64_t numerator,UiNumericValue32 maximumValue)

{
  int quotient;

  quotient = (int)(numerator / (int64_t)maximumValue);
  if (maximumValue < (int)(numerator % (int64_t)maximumValue) * 2) {
    quotient++;
  }
  return quotient;
}

/* Draws a horizontal value bar in row fixedCoordinate: start cap (base sprite) at barStartCoordinate, end cap
   (+2) ending at barEndCoordinate, and between them a filled part of rounded currentValue / maximumValue of the
   width (currentValue clamped to 0..maximumValue) in fill colour +3..+9 (by rounded sixths of the value, full
   when maximumValue is 0), the rest in the plain fill (+1). Called by SelectionPanel_RenderArmyRuntimeMetrics
   for the top and bottom edge.
*/
void SelectionPanel_DrawProportionalCappedBar
          (UiPixelCoordinate clipBottom,UiPixelCoordinate clipRight,UiPixelCoordinate clipTop,
          UiPixelCoordinate clipLeft,UiPixelCoordinate fixedCoordinate,
          UiPixelCoordinate barEndCoordinate,UiPixelCoordinate barStartCoordinate,
          UiNumericValue32 maximumValue,UiNumericValue32 currentValue,
          SelectionPanelCellIndex cellIndex)

{
  int interiorStart;
  int endCapCoordinate;
  uint32_t baseSubresource;
  int filledSpan;
  int fillFrame;
  int fixedDrawCoordinate;
  uint8_t *cell;
  GraphicsTextureLogicalSize capSize;

  cell = SelectionPanel_GetCellRecord(cellIndex);
  fixedDrawCoordinate = fixedCoordinate + *(int *)(cell + SELECTION_PANEL_CELL_OFFSET_Y);
  baseSubresource = *(uint32_t *)(cell + SELECTION_PANEL_CELL_BASE_SUBRESOURCE);
  capSize = g_GraphicsTextureSourceGetLogicalSize(baseSubresource,g_SelectionPanelTextureSource);
  g_SelectionPanelBlitOpaque
            (clipBottom,clipRight,clipTop,clipLeft,fixedDrawCoordinate,barStartCoordinate,baseSubresource,
             g_SelectionPanelTextureSource,g_FramebufferAccess);
  interiorStart = barStartCoordinate + capSize.logicalWidthPixels;
  capSize = g_GraphicsTextureSourceGetLogicalSize(baseSubresource + 2,g_SelectionPanelTextureSource);
  endCapCoordinate = barEndCoordinate - capSize.logicalWidthPixels;
  g_SelectionPanelBlitOpaque
            (clipBottom,clipRight,clipTop,clipLeft,fixedDrawCoordinate,endCapCoordinate,baseSubresource + 2,g_SelectionPanelTextureSource,
             g_FramebufferAccess);
  filledSpan = endCapCoordinate - interiorStart;
  if (currentValue < 0) {
    currentValue = 0;
  }
  else if (maximumValue < currentValue) {
    currentValue = maximumValue;
  }
  /* filled length and fill frame are rounded to the nearest integer (remainder * 2 > maximum rounds up);
     without a maximum the whole interior is filled with frame 6 */
  if (maximumValue == 0) {
    fillFrame = 6;
  }
  else {
    filledSpan = SelectionPanel_DivideRounded((int64_t)filledSpan * currentValue,maximumValue);
    fillFrame = SelectionPanel_DivideRounded((int64_t)currentValue * 6,maximumValue);
  }
  g_SelectionPanelBlitClipped
            (clipBottom,clipRight,clipTop,clipLeft,GRAPHICS_TILED_BLIT_ONE_TILE,filledSpan + interiorStart,fixedDrawCoordinate,interiorStart,
             fillFrame + 2 + baseSubresource + 1,g_SelectionPanelTextureSource,g_FramebufferAccess);
  g_SelectionPanelBlitClipped
            (clipBottom,clipRight,clipTop,clipLeft,GRAPHICS_TILED_BLIT_ONE_TILE,endCapCoordinate,fixedDrawCoordinate,filledSpan + interiorStart,baseSubresource + 1,
             g_SelectionPanelTextureSource,g_FramebufferAccess);
}

/* Draws a horizontal bar without a value in row fixedCoordinate: start cap (base sprite) at barStartCoordinate,
   end cap (+2) ending at barEndCoordinate and the plain fill (+1) between them. Called by
   SelectionPanel_RenderArmyRuntimeMetrics for the top edge when there is no value to show.
*/
void SelectionPanel_DrawForwardCappedBar
          (UiPixelCoordinate clipBottom,UiPixelCoordinate clipRight,UiPixelCoordinate clipTop,
          UiPixelCoordinate clipLeft,UiPixelCoordinate fixedCoordinate,
          UiPixelCoordinate barEndCoordinate,UiPixelCoordinate barStartCoordinate,
          SelectionPanelCellIndex cellIndex)

{
  int endCapCoordinate;
  uint32_t baseSubresource;
  int fixedDrawCoordinate;
  GraphicsTextureLogicalSize startCapSize;
  GraphicsTextureLogicalSize endCapSize;
  
  fixedDrawCoordinate = fixedCoordinate + *(int *)((uintptr_t)g_SelectionPanelData + cellIndex * SELECTION_PANEL_CELL_SIZE + SELECTION_PANEL_CELL_OFFSET_Y);
  baseSubresource = *(uint32_t *)((uintptr_t)g_SelectionPanelData + cellIndex * SELECTION_PANEL_CELL_SIZE + SELECTION_PANEL_CELL_BASE_SUBRESOURCE);
  startCapSize = g_GraphicsTextureSourceGetLogicalSize(baseSubresource,g_SelectionPanelTextureSource);
  g_SelectionPanelBlitOpaque
            (clipBottom,clipRight,clipTop,clipLeft,fixedDrawCoordinate,barStartCoordinate,baseSubresource,
             g_SelectionPanelTextureSource,g_FramebufferAccess);
  endCapSize = g_GraphicsTextureSourceGetLogicalSize(baseSubresource + 2,g_SelectionPanelTextureSource);
  endCapCoordinate = barEndCoordinate - endCapSize.logicalWidthPixels;
  g_SelectionPanelBlitOpaque
            (clipBottom,clipRight,clipTop,clipLeft,fixedDrawCoordinate,endCapCoordinate,baseSubresource + 2,g_SelectionPanelTextureSource,
             g_FramebufferAccess);
  g_SelectionPanelBlitClipped
            (clipBottom,clipRight,clipTop,clipLeft,GRAPHICS_TILED_BLIT_ONE_TILE,endCapCoordinate,fixedDrawCoordinate,
             barStartCoordinate + startCapSize.logicalWidthPixels,baseSubresource + 1,g_SelectionPanelTextureSource,
             g_FramebufferAccess);
}

/* Vertical counterpart of SelectionPanel_DrawForwardCappedBar: start cap (base sprite) at barStartCoordinate,
   end cap (+2) ending at barEndCoordinate and the plain fill (+1) between them, in column fixedCoordinate.
   Called by SelectionPanel_RenderArmyRuntimeMetrics for the left and right edge.
*/
void SelectionPanel_DrawSolidCappedBar
          (UiPixelCoordinate clipBottom,UiPixelCoordinate clipRight,UiPixelCoordinate clipTop,
          UiPixelCoordinate clipLeft,UiPixelCoordinate barEndCoordinate,
          UiPixelCoordinate barStartCoordinate,UiPixelCoordinate fixedCoordinate,
          SelectionPanelCellIndex cellIndex)

{
  int endCapCoordinate;
  uint32_t baseSubresource;
  int fixedDrawCoordinate;
  GraphicsTextureLogicalSize startCapSize;
  GraphicsTextureLogicalSize endCapSize;
  
  fixedDrawCoordinate = fixedCoordinate + *(int *)((uintptr_t)g_SelectionPanelData + cellIndex * SELECTION_PANEL_CELL_SIZE + SELECTION_PANEL_CELL_OFFSET_X);
  baseSubresource = *(uint32_t *)((uintptr_t)g_SelectionPanelData + cellIndex * SELECTION_PANEL_CELL_SIZE + SELECTION_PANEL_CELL_BASE_SUBRESOURCE);
  startCapSize = g_GraphicsTextureSourceGetLogicalSize(baseSubresource,g_SelectionPanelTextureSource);
  g_SelectionPanelBlitOpaque
            (clipBottom,clipRight,clipTop,clipLeft,barStartCoordinate,fixedDrawCoordinate,baseSubresource,
             g_SelectionPanelTextureSource,g_FramebufferAccess);
  endCapSize = g_GraphicsTextureSourceGetLogicalSize(baseSubresource + 2,g_SelectionPanelTextureSource);
  endCapCoordinate = barEndCoordinate - endCapSize.logicalHeightPixels;
  g_SelectionPanelBlitOpaque
            (clipBottom,clipRight,clipTop,clipLeft,endCapCoordinate,fixedDrawCoordinate,baseSubresource + 2,g_SelectionPanelTextureSource,
             g_FramebufferAccess);
  g_SelectionPanelBlitClipped
            (clipBottom,clipRight,clipTop,clipLeft,endCapCoordinate,GRAPHICS_TILED_BLIT_ONE_TILE,
             barStartCoordinate + startCapSize.logicalHeightPixels,fixedDrawCoordinate,baseSubresource + 1,
             g_SelectionPanelTextureSource,g_FramebufferAccess);
}

/* Draws a vertical segment row in column fixedCoordinate: caps at barStartCoordinate and barEndCoordinate,
   filledSegmentCount full segments (+4) and, with SELECTION_PANEL_CELL_FLAG_SHOW_EMPTY_SEGMENTS, the rest of
   totalSegmentCount as empty segments (+3), stacked from the top (ALIGN_START), from the bottom (ALIGN_END) or
   upwards from the centre, with the plain fill (+1) around them; only the fill when the segments do not fit.
   Called by SelectionPanel_RenderArmyRuntimeMetrics for the left and right edge of class-0x16 entities.
*/
void SelectionPanel_DrawSegmentedCappedBar
          (UiPixelCoordinate clipBottom,UiPixelCoordinate clipRight,UiPixelCoordinate clipTop,
          UiPixelCoordinate clipLeft,UiPixelCoordinate barEndCoordinate,
          UiPixelCoordinate barStartCoordinate,UiPixelCoordinate fixedCoordinate,
          SelectionPanelSegmentCount totalSegmentCount,SelectionPanelSegmentCount filledSegmentCount
          ,SelectionPanelCellIndex cellIndex)

{
  int endCapCoordinate;
  uint32_t baseSubresource;
  int fixedDrawCoordinate;
  int segmentsEnd;
  uint32_t *cellFlags;
  GraphicsTextureLogicalSize spriteSize;
  
  cellFlags = (uint32_t *)((uintptr_t)g_SelectionPanelData + cellIndex * SELECTION_PANEL_CELL_SIZE + SELECTION_PANEL_CELL_FLAGS);
  fixedDrawCoordinate = fixedCoordinate + *(int *)((uintptr_t)g_SelectionPanelData + cellIndex * SELECTION_PANEL_CELL_SIZE + SELECTION_PANEL_CELL_OFFSET_X);
  baseSubresource = *(uint32_t *)((uintptr_t)g_SelectionPanelData + cellIndex * SELECTION_PANEL_CELL_SIZE + SELECTION_PANEL_CELL_BASE_SUBRESOURCE);
  spriteSize = g_GraphicsTextureSourceGetLogicalSize(baseSubresource,g_SelectionPanelTextureSource);
  g_SelectionPanelBlitOpaque
            (clipBottom,clipRight,clipTop,clipLeft,barStartCoordinate,fixedDrawCoordinate,baseSubresource,
             g_SelectionPanelTextureSource,g_FramebufferAccess);
  barStartCoordinate = barStartCoordinate + spriteSize.logicalHeightPixels;
  spriteSize = g_GraphicsTextureSourceGetLogicalSize(baseSubresource + 2,g_SelectionPanelTextureSource);
  endCapCoordinate = barEndCoordinate - spriteSize.logicalHeightPixels;
  g_SelectionPanelBlitOpaque
            (clipBottom,clipRight,clipTop,clipLeft,endCapCoordinate,fixedDrawCoordinate,baseSubresource + 2,g_SelectionPanelTextureSource,
             g_FramebufferAccess);
  spriteSize = g_GraphicsTextureSourceGetLogicalSize(baseSubresource + 3,g_SelectionPanelTextureSource);
  segmentsEnd = filledSegmentCount;
  if ((*cellFlags & SELECTION_PANEL_CELL_FLAG_SHOW_EMPTY_SEGMENTS) != 0) {
    segmentsEnd = totalSegmentCount;
  }
  segmentsEnd = spriteSize.logicalHeightPixels * segmentsEnd + barStartCoordinate;
  if (endCapCoordinate < segmentsEnd) {
    g_SelectionPanelBlitClipped
              (clipBottom,clipRight,clipTop,clipLeft,endCapCoordinate,GRAPHICS_TILED_BLIT_ONE_TILE,barStartCoordinate,fixedDrawCoordinate,
               baseSubresource + 1,g_SelectionPanelTextureSource,g_FramebufferAccess);
  }
  else if ((*cellFlags & SELECTION_PANEL_CELL_FLAG_ALIGN_START) == 0) {
    if ((*cellFlags & SELECTION_PANEL_CELL_FLAG_ALIGN_END) == 0) {
      barEndCoordinate = endCapCoordinate - (endCapCoordinate - segmentsEnd >> 1);
      g_SelectionPanelBlitClipped
                (clipBottom,clipRight,clipTop,clipLeft,endCapCoordinate,GRAPHICS_TILED_BLIT_ONE_TILE,barEndCoordinate,fixedDrawCoordinate,
                 baseSubresource + 1,g_SelectionPanelTextureSource,g_FramebufferAccess);
      spriteSize = g_GraphicsTextureSourceGetLogicalSize(baseSubresource + 4,g_SelectionPanelTextureSource);
      for (; filledSegmentCount != 0; filledSegmentCount--) {
        barEndCoordinate = barEndCoordinate - spriteSize.logicalHeightPixels;
        g_SelectionPanelBlitOpaque
                  (clipBottom,clipRight,clipTop,clipLeft,barEndCoordinate,fixedDrawCoordinate,baseSubresource + 4,
                   g_SelectionPanelTextureSource,g_FramebufferAccess);
        totalSegmentCount--;
      }
      if ((*cellFlags & SELECTION_PANEL_CELL_FLAG_SHOW_EMPTY_SEGMENTS) != 0) {
        for (; totalSegmentCount != 0; totalSegmentCount--) {
          barEndCoordinate = barEndCoordinate - spriteSize.logicalHeightPixels;
          g_SelectionPanelBlitOpaque
                    (clipBottom,clipRight,clipTop,clipLeft,barEndCoordinate,fixedDrawCoordinate,baseSubresource + 3,
                     g_SelectionPanelTextureSource,g_FramebufferAccess);
        }
      }
      g_SelectionPanelBlitClipped
                (clipBottom,clipRight,clipTop,clipLeft,barEndCoordinate,GRAPHICS_TILED_BLIT_ONE_TILE,
                 barStartCoordinate,fixedDrawCoordinate,baseSubresource + 1,g_SelectionPanelTextureSource,g_FramebufferAccess)
      ;
    }
    else {
      spriteSize = g_GraphicsTextureSourceGetLogicalSize(baseSubresource + 4,g_SelectionPanelTextureSource);
      barEndCoordinate = endCapCoordinate;
      for (; filledSegmentCount != 0; filledSegmentCount--) {
        barEndCoordinate = barEndCoordinate - spriteSize.logicalHeightPixels;
        g_SelectionPanelBlitOpaque
                  (clipBottom,clipRight,clipTop,clipLeft,barEndCoordinate,fixedDrawCoordinate,baseSubresource + 4,
                   g_SelectionPanelTextureSource,g_FramebufferAccess);
        totalSegmentCount--;
      }
      if ((*cellFlags & SELECTION_PANEL_CELL_FLAG_SHOW_EMPTY_SEGMENTS) != 0) {
        for (; totalSegmentCount != 0; totalSegmentCount--) {
          barEndCoordinate = barEndCoordinate - spriteSize.logicalHeightPixels;
          g_SelectionPanelBlitOpaque
                    (clipBottom,clipRight,clipTop,clipLeft,barEndCoordinate,fixedDrawCoordinate,baseSubresource + 3,
                     g_SelectionPanelTextureSource,g_FramebufferAccess);
        }
      }
      g_SelectionPanelBlitClipped
                (clipBottom,clipRight,clipTop,clipLeft,barEndCoordinate,GRAPHICS_TILED_BLIT_ONE_TILE,
                 barStartCoordinate,fixedDrawCoordinate,baseSubresource + 1,g_SelectionPanelTextureSource,g_FramebufferAccess)
      ;
    }
  }
  else {
    spriteSize = g_GraphicsTextureSourceGetLogicalSize(baseSubresource + 4,g_SelectionPanelTextureSource);
    for (; filledSegmentCount != 0; filledSegmentCount--) {
      g_SelectionPanelBlitOpaque
                (clipBottom,clipRight,clipTop,clipLeft,barStartCoordinate,fixedDrawCoordinate,baseSubresource + 4,
                 g_SelectionPanelTextureSource,g_FramebufferAccess);
      barStartCoordinate = barStartCoordinate + spriteSize.logicalHeightPixels;
      totalSegmentCount--;
    }
    if ((*cellFlags & SELECTION_PANEL_CELL_FLAG_SHOW_EMPTY_SEGMENTS) != 0) {
      for (; totalSegmentCount != 0; totalSegmentCount--) {
        g_SelectionPanelBlitOpaque
                  (clipBottom,clipRight,clipTop,clipLeft,barStartCoordinate,fixedDrawCoordinate,baseSubresource + 3,
                   g_SelectionPanelTextureSource,g_FramebufferAccess);
        barStartCoordinate = barStartCoordinate + spriteSize.logicalHeightPixels;
      }
    }
    g_SelectionPanelBlitClipped
              (clipBottom,clipRight,clipTop,clipLeft,endCapCoordinate,GRAPHICS_TILED_BLIT_ONE_TILE,barStartCoordinate,fixedDrawCoordinate,
               baseSubresource + 1,g_SelectionPanelTextureSource,g_FramebufferAccess);
  }
}

/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/src/ui/text/richtext_render.cpp
 * Reverse engineering by idkFoxes 2026
 */

#include <thandor/ui/text/richtext_render.h>
#include <thandor/thandor.h>
#include <thandor/platform/bootstrap/image.h>

/* Module data. */

/* rich-text colour palette, indexed by the 3-bit palette field of the packed text style and set by
   RICHTEXT_OP_COLOR_PALETTE_0..3. The original's table has six entries and its palette field 3 bits: entries 6
   and 7 read on into the shadow offset table (the original's next object). Both tables have eight entries here,
   so every index stays inside them; entries 6 and 7 hold what the original reads there. */
PackedArgb32 g_RichTextColorPaletteArgb[TEXT_STYLE_INDEX_MASK + 1] = {
    0xFFB0B0B0, /* [0] grey; also the normal cost colour of the technology panel */
    0xFFE0E0E0, /* [1] light grey, RICHTEXT_OP_COLOR_PALETTE_1 */
    0xFF707070, /* [2] dark grey, RICHTEXT_OP_COLOR_PALETTE_2 */
    0xFFE0E0E0, /* [3] light grey, RICHTEXT_OP_COLOR_PALETTE_3 */
    0xFF209020, /* [4] green; only reachable through the packed text style's palette index */
    0xFFF02020, /* [5] red; also technology costs the player cannot afford (ui/ingame/technology.c) */
    0x00000002, /* [6] Original quirk: shadow offset entry 0 */
    0x00000002, /* [7] Original quirk: shadow offset entry 1 */
};

/* text shadow offset in pixels per colour palette entry, indexed like g_RichTextColorPaletteArgb (through
   RichTextStyle_ShadowOffset for the style's palette field). The original's table has six entries; entries 6
   and 7 read on into g_ActiveFontIndex and g_RichTextCurrentColorArgb (its next objects). */
static uint32_t g_RichTextShadowOffsetPalette[TEXT_STYLE_INDEX_MASK + 1] = {
    2, /* [0] */
    2, /* [1] */
    1, /* [2] */
    2, /* [3] */
    0, /* [4] */
    0, /* [5] */
    0, /* [6] Original quirk: g_ActiveFontIndex, read by RichTextStyle_ShadowOffset instead of this entry */
    2, /* [7] Original quirk: g_RichTextCurrentColorArgb, set just before to palette entry 7 (2) */
};

/* the stream entered by a nested-stream command without a target: just a terminator */
static uint16_t g_RichTextEmptyStream[1] = {0};

static uint32_t g_RichTextSavedColorArgb = 0;

static uint32_t g_RichTextSavedShadowOffset = 0;

uint32_t g_ActiveFontIndex = 0;

uint32_t g_RichTextCurrentColorArgb = 0;

uint32_t g_RichTextCurrentShadowOffset = 0;

/* Implementation ownership: ui/text/richtext_render. */

/* Shadow offset for the palette field of a packed text style; called right after g_ActiveFontIndex and
   g_RichTextCurrentColorArgb were set from the same style. Original quirk: palette entry 6 reads its shadow
   offset from g_ActiveFontIndex (the object behind the original's shadow table); entry 7 reads
   g_RichTextCurrentColorArgb, which the table holds as its constant value 2. */
static uint32_t RichTextStyle_ShadowOffset(uint32_t paletteIndex)
{
  if (paletteIndex == 6) {
    return g_ActiveFontIndex;
  }
  return g_RichTextShadowOffsetPalette[paletteIndex];
}

/* Target of a nested-stream command (RICHTEXT_OP_CALL_NESTED / RICHTEXT_OP_JUMP_NESTED) from its payload. The
   original enters the pointer unchecked; bounded here because a null target only comes from malformed text:
   it enters an empty stream (logged once). */
static uint16_t *RichTextCommand_NestedTarget(uint16_t *payload)
{
  static int s_loggedNullNestedStream;
  uint16_t *target;

  target = THANDOR_PTR32_AT(uint16_t, payload);
  if (target == nullptr) {
    if (s_loggedNullNestedStream == 0) {
      s_loggedNullNestedStream = 1;
      Thandor_Log("rich text: nested-stream command without a target, skipped");
    }
    target = g_RichTextEmptyStream;
  }
  return target;
}

/* Measures a text block wrapped to maximumWidth: flattens the stream into the font runtime buffer, sets the
   style's font and colour, and sums the heights of all wrapped lines. Returns maximumWidth itself as the width
   (not the widest line) and the total height.
*/
RichTextExtent RichTextCommandStream_MeasureWrappedBlock
          (uint32_t packedStyle,uint16_t *commandStream,UiPixelExtent maximumWidth)

{
  uint32_t colorPaletteIndex;
  int totalHeight;
  RichTextExtent blockExtent;
  UiPixelExtent lineHeight;

  RichTextCommandStream_FlattenNestedToRuntimeBuffer(commandStream);
  colorPaletteIndex = packedStyle >> TEXT_STYLE_PALETTE_SHIFT & TEXT_STYLE_INDEX_MASK;
  g_ActiveFontIndex = packedStyle >> TEXT_STYLE_FONT_SHIFT & TEXT_STYLE_INDEX_MASK;
  g_RichTextCurrentColorArgb = g_RichTextColorPaletteArgb[colorPaletteIndex];
  g_RichTextCurrentShadowOffset = RichTextStyle_ShadowOffset(colorPaletteIndex);
  totalHeight = 0;
  g_RichTextSavedColorArgb = g_RichTextCurrentColorArgb;
  g_RichTextSavedShadowOffset = g_RichTextCurrentShadowOffset;
  while (RichTextCommandStream_MeasureNextWrappedLine(maximumWidth,&lineHeight)) {
    totalHeight = totalHeight + lineHeight;
  }
  blockExtent.heightPixels = totalHeight + lineHeight;
  blockExtent.widthPixels = maximumWidth;
  return blockExtent;
}

/* Draws a rich-text block wrapped to maximumWidth with its top-left corner at (drawX, drawY), clipped to the
   given rectangle: flattens the stream into the font runtime buffer, sets the style's font and colour, and
   draws line after line (RichTextCommandStream_DrawNextWrappedLine) until the end of the text. Called directly
   by UiWrappedTextControl_DrawClipped.
*/
void RichTextCommandStream_DrawWrappedBlock
          (UiPixelCoordinate clipBottom,UiPixelCoordinate clipRight,UiPixelCoordinate clipTop,
          UiPixelCoordinate clipLeft,uint32_t packedStyle,uint16_t *commandStream,
          UiPixelExtent maximumWidth,UiPixelCoordinate drawY,UiPixelCoordinate drawX)

{
  uint32_t colorPaletteIndex;
  UiPixelExtent lineHeight;

  RichTextCommandStream_FlattenNestedToRuntimeBuffer(commandStream);
  colorPaletteIndex = packedStyle >> TEXT_STYLE_PALETTE_SHIFT & TEXT_STYLE_INDEX_MASK;
  g_ActiveFontIndex = packedStyle >> TEXT_STYLE_FONT_SHIFT & TEXT_STYLE_INDEX_MASK;
  g_RichTextCurrentColorArgb = g_RichTextColorPaletteArgb[colorPaletteIndex];
  g_RichTextCurrentShadowOffset = RichTextStyle_ShadowOffset(colorPaletteIndex);
  g_RichTextSavedColorArgb = g_RichTextCurrentColorArgb;
  g_RichTextSavedShadowOffset = g_RichTextCurrentShadowOffset;
  while (RichTextCommandStream_DrawNextWrappedLine
                (clipBottom,clipRight,clipTop,clipLeft,maximumWidth,drawY,drawX,&lineHeight)) {
    drawY = drawY + lineHeight;
  }
  return;
}

/* Draws one rich-text line: measures it first to align it (right or centred on penX, per the packed style) and
   to place the baseline below lineTopY, then interprets glyphs, colour, font, nested-stream and inline-image
   commands until the end of the stream or a line break.
*/
Bool8 RichTextCommandStream_DrawSingleLine
          (UiPixelCoordinate clipBottom,UiPixelCoordinate clipRight,UiPixelCoordinate clipTop,
          UiPixelCoordinate clipLeft,UiPackedTextStyle packedStyle,uint16_t *commandStream,
          UiPixelCoordinate lineTopY,UiPixelCoordinate penX)

{
  int lineBaselineY;
  uint32_t lineWidth;
  uint32_t paletteIndex;
  GraphicsSubresourceIndex glyphSubresource;
  int glyphAdvance;
  uint32_t imageWidth;
  uint16_t *commandCursor;
  RichTextExtent lineExtent;
  GraphicsTextureLogicalSize imageSize;
  uint16_t *nestedReturnStack[RICHTEXT_NESTING_LIMIT]; /* the original's machine-stack chain */
  int nestedDepth;
  
  lineExtent = RichTextCommandStream_MeasureLine(packedStyle,commandStream);
  lineWidth = lineExtent.widthPixels;
  lineBaselineY = lineTopY + lineExtent.heightPixels;
  if ((packedStyle & TEXT_STYLE_ALIGN_RIGHT) != 0) {
    penX = penX - lineWidth;
  }
  else if ((packedStyle & TEXT_STYLE_ALIGN_CENTER) != 0) {
    penX = penX - (lineWidth >> 1);
  }
  paletteIndex = packedStyle >> TEXT_STYLE_PALETTE_SHIFT & TEXT_STYLE_INDEX_MASK;
  g_ActiveFontIndex = packedStyle >> TEXT_STYLE_FONT_SHIFT & TEXT_STYLE_INDEX_MASK;
  g_RichTextCurrentColorArgb = g_RichTextColorPaletteArgb[paletteIndex];
  g_RichTextCurrentShadowOffset = RichTextStyle_ShadowOffset(paletteIndex);
  nestedDepth = 0;
  g_RichTextSavedColorArgb = g_RichTextCurrentColorArgb;
  g_RichTextSavedShadowOffset = g_RichTextCurrentShadowOffset;
  /* Runs until the terminator of the outermost stream (the terminator of a nested stream returns to the
     caller stream). */
  while ((*commandStream != 0) || (nestedDepth != 0)) {
    commandCursor = commandStream;
    glyphSubresource = (GraphicsSubresourceIndex)(short)*commandCursor;
    commandStream = commandCursor + 1;
    if (glyphSubresource == 0) {
      commandStream = (uint16_t *)((uint8_t *)nestedReturnStack[--nestedDepth] + RICHTEXT_NESTED_PAYLOAD_BYTES);
      continue;
    }
    if ((int)glyphSubresource >= 0) {
      glyphAdvance = FontGlyph_DrawBottomAligned
                        (clipBottom,clipRight,clipTop,clipLeft,glyphSubresource,lineBaselineY,penX);
      penX = penX + glyphAdvance;
      continue;
    }
    switch(glyphSubresource & RICHTEXT_OPCODE_MASK) {
    case RICHTEXT_OP_COLOR_PALETTE_0:
      g_RichTextCurrentColorArgb = g_RichTextColorPaletteArgb[0];
      g_RichTextCurrentShadowOffset = g_RichTextShadowOffsetPalette[0];
      break;
    case RICHTEXT_OP_COLOR_PALETTE_1:
      g_RichTextCurrentColorArgb = g_RichTextColorPaletteArgb[1];
      g_RichTextCurrentShadowOffset = g_RichTextShadowOffsetPalette[1];
      break;
    case RICHTEXT_OP_COLOR_PALETTE_2:
      g_RichTextCurrentColorArgb = g_RichTextColorPaletteArgb[2];
      g_RichTextCurrentShadowOffset = g_RichTextShadowOffsetPalette[2];
      break;
    case RICHTEXT_OP_COLOR_PALETTE_3:
      g_RichTextCurrentColorArgb = g_RichTextColorPaletteArgb[3];
      g_RichTextCurrentShadowOffset = g_RichTextShadowOffsetPalette[3];
      break;
    case RICHTEXT_OP_SAVE_COLOR:
      g_RichTextSavedColorArgb = g_RichTextCurrentColorArgb;
      g_RichTextSavedShadowOffset = g_RichTextCurrentShadowOffset;
      break;
    case RICHTEXT_OP_RESTORE_COLOR:
      g_RichTextCurrentColorArgb = g_RichTextSavedColorArgb;
      g_RichTextCurrentShadowOffset = g_RichTextSavedShadowOffset;
      break;
    case RICHTEXT_OP_LITERAL_COLOR:
      /* The eight payload code units are hex digits (only their low nibble is used, shifted in four bits
         at a time): units 1-2 form the lowest colour byte, 7-8 the highest, each pair high digit first. */
      g_RichTextCurrentColorArgb =
           ((((((((uint8_t)commandCursor[2] & 0xf) << (RICHTEXT_COLOR_DIGIT_SHIFT - RICHTEXT_COLOR_DIGIT_BITS) |
                 (uint32_t)(uint8_t)*commandStream << RICHTEXT_COLOR_DIGIT_SHIFT) >> RICHTEXT_COLOR_DIGIT_BITS |
                (uint32_t)(uint8_t)commandCursor[4] << RICHTEXT_COLOR_DIGIT_SHIFT) >> RICHTEXT_COLOR_DIGIT_BITS |
               (uint32_t)(uint8_t)commandCursor[3] << RICHTEXT_COLOR_DIGIT_SHIFT) >> RICHTEXT_COLOR_DIGIT_BITS |
              (uint32_t)(uint8_t)commandCursor[6] << RICHTEXT_COLOR_DIGIT_SHIFT) >> RICHTEXT_COLOR_DIGIT_BITS |
             (uint32_t)(uint8_t)commandCursor[5] << RICHTEXT_COLOR_DIGIT_SHIFT) >> RICHTEXT_COLOR_DIGIT_BITS |
            (uint32_t)(uint8_t)commandCursor[8] << RICHTEXT_COLOR_DIGIT_SHIFT) >> RICHTEXT_COLOR_DIGIT_BITS |
           (uint32_t)(uint8_t)commandCursor[7] << RICHTEXT_COLOR_DIGIT_SHIFT;
      commandStream = commandCursor + RICHTEXT_RECORD_UNITS_LITERAL_COLOR;
      break;
    case RICHTEXT_OP_SELECT_FONT_FIRST:
    case RICHTEXT_OP_SELECT_FONT_FIRST + 1:
    case RICHTEXT_OP_SELECT_FONT_FIRST + 2:
    case RICHTEXT_OP_SELECT_FONT_FIRST + 3:
    case RICHTEXT_OP_SELECT_FONT_FIRST + 4:
    case RICHTEXT_OP_SELECT_FONT_FIRST + 5:
    case RICHTEXT_OP_SELECT_FONT_FIRST + 6:
    case RICHTEXT_OP_SELECT_FONT_FIRST + 7:
      g_ActiveFontIndex = glyphSubresource & 0xf;
      break;
    case RICHTEXT_OP_FIXED_SPACE:
      glyphAdvance = FontGlyph_DrawBottomAligned
                        (clipBottom,clipRight,clipTop,clipLeft,' ',lineBaselineY,penX);
      penX = penX + glyphAdvance;
      break;
    case RICHTEXT_OP_LINE_BREAK:
      return false;
    case RICHTEXT_OP_INLINE_VALUE_0:
    case RICHTEXT_OP_INLINE_VALUE_1:
    case RICHTEXT_OP_INLINE_VALUE_2:
      commandStream = commandCursor + RICHTEXT_RECORD_UNITS_INLINE_VALUE;
      break;
    case RICHTEXT_OP_CALL_NESTED:
      if (nestedDepth == RICHTEXT_NESTING_LIMIT) {
        return false;
      }
      nestedReturnStack[nestedDepth++] = commandStream;
      commandStream = RichTextCommand_NestedTarget(commandStream);
      break;
    case RICHTEXT_OP_JUMP_NESTED:
      commandStream = RichTextCommand_NestedTarget(commandStream);
      break;
    case RICHTEXT_OP_INLINE_IMAGE:
      /* payload: texture source at commandCursor + 1, subresource at commandCursor + 3; the image sits on the
         baseline */
      imageSize = g_GraphicsTextureSourceGetLogicalSize
                        (*(uint32_t *)(commandCursor + 3),THANDOR_PTR32_AT(GraphicsTextureSourceAsset, commandStream));
      imageWidth = imageSize.logicalWidthPixels;
      g_GraphicsTextureSourceBlitSourceAlpha
                (clipBottom,clipRight,clipTop,clipLeft,lineBaselineY - imageSize.logicalHeightPixels,
                 penX,*(uint32_t *)(commandCursor + 3),THANDOR_PTR32_AT(GraphicsTextureSourceAsset, commandStream),
                 g_FramebufferAccess);
      penX = penX + imageWidth;
      commandStream = commandCursor + RICHTEXT_RECORD_UNITS_INLINE_IMAGE;
    }
  }
  return false;
}

/* Measures one line of a rich-text command stream (up to its end or the first line break), following nested
   streams and font changes and including inline images. Returns the total width and the height of the tallest
   glyph or image; used to align a line before it is drawn.
*/
RichTextExtent RichTextCommandStream_MeasureLine(UiPackedTextStyle packedStyle,uint16_t *commandStream)

{
  /* Command 0x18 enters a nested stream and remembers the return position (the original keeps it on the
     machine stack); the nested stream's terminator resumes behind the command's payload (8 bytes later). */
  uint16_t *returnStack[RICHTEXT_NESTING_LIMIT];
  int nesting = 0;
  RichTextExtent extent;
  uint32_t glyphWidth;
  uint32_t glyphLineHeight;
  GraphicsTextureLogicalSize textureSize;
  uint16_t *command;
  int value; /* the code unit, sign-extended: negative for commands */

  extent.widthPixels = 0;
  extent.heightPixels = 0;
  g_ActiveFontIndex = packedStyle >> TEXT_STYLE_FONT_SHIFT & TEXT_STYLE_INDEX_MASK;
  /* Runs until the terminator of the outermost stream or the first line break. */
  while ((*commandStream != 0) || (nesting != 0)) {
    command = commandStream;
    value = (int)(short)*command;
    commandStream = command + 1;
    if (value == 0) {
      commandStream = (uint16_t *)((uint8_t *)returnStack[--nesting] + RICHTEXT_NESTED_PAYLOAD_BYTES);
      continue;
    }
    if (value > 0) {
      glyphWidth = FontGlyph_GetLogicalSizeActiveFont((GraphicsSubresourceIndex)value,&glyphLineHeight);
      extent.widthPixels = extent.widthPixels + glyphWidth;
      if (extent.heightPixels < glyphLineHeight) {
        extent.heightPixels = glyphLineHeight;
      }
      continue;
    }
    switch (value & RICHTEXT_OPCODE_MASK) {
    case RICHTEXT_OP_LITERAL_COLOR:
      commandStream = command + RICHTEXT_RECORD_UNITS_LITERAL_COLOR;
      break;
    case RICHTEXT_OP_SELECT_FONT_FIRST:
    case RICHTEXT_OP_SELECT_FONT_FIRST + 1:
    case RICHTEXT_OP_SELECT_FONT_FIRST + 2:
    case RICHTEXT_OP_SELECT_FONT_FIRST + 3:
    case RICHTEXT_OP_SELECT_FONT_FIRST + 4:
    case RICHTEXT_OP_SELECT_FONT_FIRST + 5:
    case RICHTEXT_OP_SELECT_FONT_FIRST + 6:
    case RICHTEXT_OP_SELECT_FONT_FIRST + 7:
      g_ActiveFontIndex = value & 0xf;
      break;
    case RICHTEXT_OP_FIXED_SPACE:
      glyphWidth = FontGlyph_GetLogicalSizeActiveFont(' ',&glyphLineHeight);
      extent.widthPixels = extent.widthPixels + glyphWidth;
      if (extent.heightPixels < glyphLineHeight) {
        extent.heightPixels = glyphLineHeight;
      }
      break;
    case RICHTEXT_OP_LINE_BREAK:
      return extent;
    case RICHTEXT_OP_INLINE_VALUE_0:
    case RICHTEXT_OP_INLINE_VALUE_1:
    case RICHTEXT_OP_INLINE_VALUE_2:
      commandStream = command + RICHTEXT_RECORD_UNITS_INLINE_VALUE;
      break;
    case RICHTEXT_OP_CALL_NESTED:
      if (nesting == RICHTEXT_NESTING_LIMIT) {
        return extent;
      }
      returnStack[nesting++] = commandStream;
      commandStream = RichTextCommand_NestedTarget(commandStream);
      break;
    case RICHTEXT_OP_JUMP_NESTED:
      commandStream = RichTextCommand_NestedTarget(commandStream);
      break;
    case RICHTEXT_OP_INLINE_IMAGE:
      textureSize = g_GraphicsTextureSourceGetLogicalSize
                              (*(uint32_t *)(command + 3),THANDOR_PTR32_AT(GraphicsTextureSourceAsset, commandStream));
      extent.widthPixels = extent.widthPixels + textureSize.logicalWidthPixels;
      commandStream = command + RICHTEXT_RECORD_UNITS_INLINE_IMAGE;
      if (extent.heightPixels < textureSize.logicalHeightPixels) {
        extent.heightPixels = textureSize.logicalHeightPixels;
      }
      break;
    }
  }
  return extent;
}

/* Line measuring shared by RichTextCommandStream_MeasureNextWrappedLine and
   RichTextCommandStream_DrawNextWrappedLine: scans the flat stream from lineStart and returns the wrap point,
   i.e. the position behind the last space or soft hyphen that still fits into maximumWidth (behind a line
   break or the terminator when the line fits up to there), or the position where the scan stopped when no
   wrap opportunity fit. Raises *lineHeight to the tallest glyph or image height of the line. Font commands are
   applied on the way. */
static uint16_t *RichTextCommandStream_FindWrapPoint
          (UiPixelExtent maximumWidth,uint16_t *lineStart,uint32_t *lineHeight)
{
  GraphicsSubresourceIndex glyphSubresource;
  uint32_t lineWidth;
  uint16_t *commandCursor;
  uint16_t *readCursor;
  uint32_t glyphWidth;
  uint32_t glyphLineHeight;
  GraphicsTextureLogicalSize imageSize;
  uint16_t *wrapPoint;

  lineWidth = 0;
  wrapPoint = nullptr;
  for (commandCursor = lineStart; *commandCursor != 0; commandCursor = readCursor) {
    glyphSubresource = (GraphicsSubresourceIndex)(short)*commandCursor;
    readCursor = commandCursor + 1;
    if (glyphSubresource == ' ') {
      /* A space is a wrap opportunity while the line up to it still fits. */
      glyphWidth = FontGlyph_GetLogicalSizeActiveFont(' ',&glyphLineHeight);
      if (maximumWidth < lineWidth) {
        return (wrapPoint != nullptr) ? wrapPoint : readCursor;
      }
      lineWidth = lineWidth + glyphWidth;
      wrapPoint = readCursor;
      continue;
    }
    if ((int)glyphSubresource >= 0) { /* no RICHTEXT_COMMAND_FLAG: a glyph */
      glyphWidth = FontGlyph_GetLogicalSizeActiveFont(glyphSubresource,&glyphLineHeight);
      lineWidth = lineWidth + glyphWidth;
      if (*lineHeight < glyphLineHeight) {
        *lineHeight = glyphLineHeight;
      }
      continue;
    }
    switch(glyphSubresource & RICHTEXT_OPCODE_MASK) {
    case RICHTEXT_OP_LITERAL_COLOR:
      readCursor = commandCursor + RICHTEXT_RECORD_UNITS_LITERAL_COLOR;
      break;
    case RICHTEXT_OP_SELECT_FONT_FIRST:
    case RICHTEXT_OP_SELECT_FONT_FIRST + 1:
    case RICHTEXT_OP_SELECT_FONT_FIRST + 2:
    case RICHTEXT_OP_SELECT_FONT_FIRST + 3:
    case RICHTEXT_OP_SELECT_FONT_FIRST + 4:
    case RICHTEXT_OP_SELECT_FONT_FIRST + 5:
    case RICHTEXT_OP_SELECT_FONT_FIRST + 6:
    case RICHTEXT_OP_SELECT_FONT_FIRST + 7:
      g_ActiveFontIndex = glyphSubresource & 0xf;
      break;
    case RICHTEXT_OP_FIXED_SPACE:
      glyphWidth = FontGlyph_GetLogicalSizeActiveFont(' ',&glyphLineHeight);
      lineWidth = lineWidth + glyphWidth;
      if (*lineHeight < glyphLineHeight) {
        *lineHeight = glyphLineHeight;
      }
      break;
    case RICHTEXT_OP_SOFT_HYPHEN:
      /* Soft hyphen: a wrap opportunity when the hyphen still fits. */
      glyphWidth = FontGlyph_GetLogicalSizeActiveFont('-',&glyphLineHeight);
      if (maximumWidth < glyphWidth + lineWidth) {
        return (wrapPoint != nullptr) ? wrapPoint : readCursor;
      }
      wrapPoint = readCursor;
      break;
    case RICHTEXT_OP_LINE_BREAK:
      if (lineWidth <= maximumWidth) {
        wrapPoint = readCursor;
      }
      return (wrapPoint != nullptr) ? wrapPoint : readCursor;
    case RICHTEXT_OP_INLINE_IMAGE:
      /* payload: texture source pointer (code units 1-2), subresource (code units 3-4) */
      imageSize = g_GraphicsTextureSourceGetLogicalSize
                        (*(uint32_t *)(commandCursor + 3),THANDOR_PTR32_AT(GraphicsTextureSourceAsset, readCursor));
      lineWidth = lineWidth + imageSize.logicalWidthPixels;
      readCursor = commandCursor + RICHTEXT_RECORD_UNITS_INLINE_IMAGE;
      if (*lineHeight < imageSize.logicalHeightPixels) {
        *lineHeight = imageSize.logicalHeightPixels;
      }
    }
  }
  /* End of stream: handled like RICHTEXT_OP_LINE_BREAK. */
  readCursor = commandCursor + 1;
  if (lineWidth <= maximumWidth) {
    wrapPoint = readCursor;
  }
  return (wrapPoint != nullptr) ? wrapPoint : readCursor;
}

/* Measures the next line of the flattened rich-text runtime buffer that fits into maximumWidth, wrapping after
   the last space or soft hyphen that still fits (or at a line break), and advances
   g_RichTextRuntimeBufferUsedWords past it. Font commands are applied on the way. Stores the tallest glyph or
   image height of the line in *lineHeight (also for the last line) and returns true while more lines follow,
   false when this line ends the text.
*/
Bool8 RichTextCommandStream_MeasureNextWrappedLine(UiPixelExtent maximumWidth,UiPixelExtent *lineHeight)

{
  uint32_t glyphLineHeight;
  uint32_t maxLineHeight;
  uint16_t *wrapPoint;

  FontGlyph_GetLogicalSizeActiveFont(0,&glyphLineHeight);
  maxLineHeight = glyphLineHeight;
  wrapPoint = RichTextCommandStream_FindWrapPoint
                        (maximumWidth,
                         (uint16_t *)(g_FontRuntimeBuffer + g_RichTextRuntimeBufferUsedWords * sizeof(uint16_t)),
                         &maxLineHeight);
  g_RichTextRuntimeBufferUsedWords = (uint32_t)((uint8_t *)wrapPoint - g_FontRuntimeBuffer) >> 1;
  *lineHeight = maxLineHeight;
  return (short)wrapPoint[-1] != 0; /* false: the line ended at the stream terminator */
}

/* End of RichTextCommandStream_DrawNextWrappedLine: the next line starts at nextLine; stores the line height
   and passes moreLinesFollow through. */
static Bool8 RichTextCommandStream_EndWrappedLine
          (uint16_t *nextLine,uint32_t lineHeight,UiPixelExtent *lineAdvance,Bool8 moreLinesFollow)
{
  g_RichTextRuntimeBufferUsedWords = (uint32_t)((uint8_t *)nextLine - g_FontRuntimeBuffer) >> 1;
  *lineAdvance = lineHeight;
  return moreLinesFollow;
}

/* Draws the next line of the flattened rich-text runtime buffer at (drawX, drawY), wrapped to maximumWidth:
   a measure pass with the rules of RichTextCommandStream_MeasureNextWrappedLine finds the wrap point and the
   line height, then the draw pass renders glyphs, images and colour/font commands up to it (drawing the hyphen
   when the line wraps at a soft hyphen) and advances g_RichTextRuntimeBufferUsedWords. Stores the line height in
   *lineAdvance (also for the last line) and returns true while more lines follow, false when this line ends
   the text. Called directly by RichTextCommandStream_DrawWrappedBlock.
*/
Bool8 RichTextCommandStream_DrawNextWrappedLine
          (UiPixelCoordinate clipBottom,UiPixelCoordinate clipRight,UiPixelCoordinate clipTop,
          UiPixelCoordinate clipLeft,UiPixelExtent maximumWidth,UiPixelCoordinate drawY,
          UiPixelCoordinate drawX,UiPixelExtent *lineAdvance)

{
  int lineBottom;
  GraphicsSubresourceIndex glyphSubresource;
  int glyphAdvance;
  uint32_t savedFontIndex;
  uint32_t imageWidth;
  uint32_t lineHeight;
  uint16_t *commandCursor;
  uint16_t *drawCursor;
  uint32_t glyphLineHeight;
  GraphicsTextureLogicalSize imageSize;
  uint16_t *lineStart;
  uint16_t *wrapPoint;

  FontGlyph_GetLogicalSizeActiveFont(0,&glyphLineHeight);
  lineStart = (uint16_t *)(g_FontRuntimeBuffer + g_RichTextRuntimeBufferUsedWords * sizeof(uint16_t));
  lineHeight = glyphLineHeight;
  savedFontIndex = g_ActiveFontIndex; /* font commands of the measure pass are undone below */
  /* Measure pass (same rules as RichTextCommandStream_MeasureNextWrappedLine): find the wrap point and
     the line height. */
  wrapPoint = RichTextCommandStream_FindWrapPoint(maximumWidth,lineStart,&lineHeight);
  g_ActiveFontIndex = savedFontIndex;
  lineBottom = drawY + lineHeight;
  /* Draw pass: the line ends at a space or soft hyphen at/after the wrap point, at a line break, or at the
     end of the text. */
  for (commandCursor = lineStart; *commandCursor != 0; commandCursor = drawCursor) {
    glyphSubresource = (GraphicsSubresourceIndex)(short)*commandCursor;
    drawCursor = commandCursor + 1;
    if (glyphSubresource == ' ') {
      if (wrapPoint <= drawCursor) {
        return RichTextCommandStream_EndWrappedLine(drawCursor,lineHeight,lineAdvance,true);
      }
      glyphAdvance = FontGlyph_DrawVerticallyCentered
                        (clipBottom,clipRight,clipTop,clipLeft,' ',lineHeight,lineBottom,drawX);
      drawX = drawX + glyphAdvance;
      continue;
    }
    if ((int)glyphSubresource >= 0) {
      glyphAdvance = FontGlyph_DrawVerticallyCentered
                        (clipBottom,clipRight,clipTop,clipLeft,glyphSubresource,lineHeight,lineBottom,drawX);
      drawX = drawX + glyphAdvance;
      continue;
    }
    switch(glyphSubresource & RICHTEXT_OPCODE_MASK) {
    case RICHTEXT_OP_COLOR_PALETTE_0:
      g_RichTextCurrentColorArgb = g_RichTextColorPaletteArgb[0];
      g_RichTextCurrentShadowOffset = g_RichTextShadowOffsetPalette[0];
      break;
    case RICHTEXT_OP_COLOR_PALETTE_1:
      g_RichTextCurrentColorArgb = g_RichTextColorPaletteArgb[1];
      g_RichTextCurrentShadowOffset = g_RichTextShadowOffsetPalette[1];
      break;
    case RICHTEXT_OP_COLOR_PALETTE_2:
      g_RichTextCurrentColorArgb = g_RichTextColorPaletteArgb[2];
      g_RichTextCurrentShadowOffset = g_RichTextShadowOffsetPalette[2];
      break;
    case RICHTEXT_OP_COLOR_PALETTE_3:
      g_RichTextCurrentColorArgb = g_RichTextColorPaletteArgb[3];
      g_RichTextCurrentShadowOffset = g_RichTextShadowOffsetPalette[3];
      break;
    case RICHTEXT_OP_SAVE_COLOR:
      g_RichTextSavedColorArgb = g_RichTextCurrentColorArgb;
      g_RichTextSavedShadowOffset = g_RichTextCurrentShadowOffset;
      break;
    case RICHTEXT_OP_RESTORE_COLOR:
      g_RichTextCurrentShadowOffset = g_RichTextSavedShadowOffset;
      g_RichTextCurrentColorArgb = g_RichTextSavedColorArgb;
      break;
    case RICHTEXT_OP_LITERAL_COLOR:
      /* eight hex-digit code units, only their low bytes are used, as in
         RichTextCommandStream_DrawSingleLine */
      g_RichTextCurrentColorArgb =
           ((((((((uint8_t)commandCursor[2] & 0xf) << (RICHTEXT_COLOR_DIGIT_SHIFT - RICHTEXT_COLOR_DIGIT_BITS) |
                (uint32_t)(uint8_t)*drawCursor << RICHTEXT_COLOR_DIGIT_SHIFT) >> RICHTEXT_COLOR_DIGIT_BITS |
               (uint32_t)(uint8_t)commandCursor[4] << RICHTEXT_COLOR_DIGIT_SHIFT) >> RICHTEXT_COLOR_DIGIT_BITS |
              (uint32_t)(uint8_t)commandCursor[3] << RICHTEXT_COLOR_DIGIT_SHIFT) >> RICHTEXT_COLOR_DIGIT_BITS |
             (uint32_t)(uint8_t)commandCursor[6] << RICHTEXT_COLOR_DIGIT_SHIFT) >> RICHTEXT_COLOR_DIGIT_BITS |
            (uint32_t)(uint8_t)commandCursor[5] << RICHTEXT_COLOR_DIGIT_SHIFT) >> RICHTEXT_COLOR_DIGIT_BITS |
           (uint32_t)(uint8_t)commandCursor[8] << RICHTEXT_COLOR_DIGIT_SHIFT) >> RICHTEXT_COLOR_DIGIT_BITS |
           (uint32_t)(uint8_t)commandCursor[7] << RICHTEXT_COLOR_DIGIT_SHIFT;
      drawCursor = commandCursor + RICHTEXT_RECORD_UNITS_LITERAL_COLOR;
      break;
    case RICHTEXT_OP_SELECT_FONT_FIRST:
    case RICHTEXT_OP_SELECT_FONT_FIRST + 1:
    case RICHTEXT_OP_SELECT_FONT_FIRST + 2:
    case RICHTEXT_OP_SELECT_FONT_FIRST + 3:
    case RICHTEXT_OP_SELECT_FONT_FIRST + 4:
    case RICHTEXT_OP_SELECT_FONT_FIRST + 5:
    case RICHTEXT_OP_SELECT_FONT_FIRST + 6:
    case RICHTEXT_OP_SELECT_FONT_FIRST + 7:
      g_ActiveFontIndex = glyphSubresource & 0xf;
      break;
    case RICHTEXT_OP_FIXED_SPACE:
      glyphAdvance = FontGlyph_DrawVerticallyCentered
                        (clipBottom,clipRight,clipTop,clipLeft,' ',lineHeight,lineBottom,drawX);
      drawX = drawX + glyphAdvance;
      break;
    case RICHTEXT_OP_SOFT_HYPHEN:
      if (wrapPoint <= drawCursor) {
        /* The line wraps at this soft hyphen: draw the hyphen and end the line. */
        FontGlyph_DrawVerticallyCentered
                  (clipBottom,clipRight,clipTop,clipLeft,'-',lineHeight,lineBottom,drawX);
        return RichTextCommandStream_EndWrappedLine(drawCursor,lineHeight,lineAdvance,true);
      }
      break;
    case RICHTEXT_OP_LINE_BREAK:
      return RichTextCommandStream_EndWrappedLine(drawCursor,lineHeight,lineAdvance,true);
    case RICHTEXT_OP_INLINE_IMAGE:
      /* payload: texture source at commandCursor + 1, subresource at commandCursor + 3; the image sits on the
         line's bottom edge */
      imageSize = g_GraphicsTextureSourceGetLogicalSize
                         (*(uint32_t *)(commandCursor + 3),THANDOR_PTR32_AT(GraphicsTextureSourceAsset, drawCursor));
      imageWidth = imageSize.logicalWidthPixels;
      g_GraphicsTextureSourceBlitSourceAlpha
                (clipBottom,clipRight,clipTop,clipLeft,lineBottom - imageSize.logicalHeightPixels,drawX,
                 *(uint32_t *)(commandCursor + 3),THANDOR_PTR32_AT(GraphicsTextureSourceAsset, drawCursor),g_FramebufferAccess);
      drawX = drawX + imageWidth;
      drawCursor = commandCursor + RICHTEXT_RECORD_UNITS_INLINE_IMAGE;
    }
  }
  /* end of the text: the next line starts behind the terminator */
  return RichTextCommandStream_EndWrappedLine(commandCursor + 1,lineHeight,lineAdvance,false);
}

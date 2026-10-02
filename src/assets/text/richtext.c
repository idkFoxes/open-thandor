/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/src/assets/text/richtext.c
 * Reverse engineering by idkFoxes 2026
 */

#include <thandor/assets/text/richtext.h>
#include <thandor/thandor.h>

/* Depth of the machine-stack return chains the original keeps for nested (0x18) streams. */
#define RICHTEXT_NESTING_LIMIT 64

/* Tags RichTextMarkup_ParseAndBuildStringAsset can collect (the original keeps them on the machine stack). */
#define RICHTEXT_MARKUP_TAG_LIMIT 4096

/* Implementation ownership: assets/text/richtext. */

/* Address: 0x0041D300.
   Measures a text block wrapped to maximumWidth: flattens the stream into the font runtime buffer, sets the
   style's font and colour, and sums the heights of all wrapped lines. Returns maximumWidth itself in EAX (not
   the widest line) and the total height in EDX.
*/
RichTextExtentRegs RichTextCommandStream_MeasureWrappedBlockRegs
          (uint32_t packedStyle,uint16_t *commandStream,UiPixelExtent maximumWidth)

{
  uint32_t colorPaletteIndex;
  int totalHeight;
  RichTextExtentRegs blockExtent;
  UiPixelExtent lineHeight;

  RichTextCommandStream_FlattenNestedToRuntimeBuffer(commandStream);
  colorPaletteIndex = packedStyle >> TEXT_STYLE_PALETTE_SHIFT & TEXT_STYLE_INDEX_MASK;
  g_ActiveFontIndex = packedStyle >> TEXT_STYLE_FONT_SHIFT & TEXT_STYLE_INDEX_MASK;
  g_RichTextCurrentColorArgb = (&g_RichTextColorPalette0Argb)[colorPaletteIndex];
  g_RichTextCurrentShadowOffset = (&g_RichTextShadowOffsetPalette0)[colorPaletteIndex];
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


/* Address: 0x0041D7C0.
   Draws a rich-text block wrapped to maximumWidth with its top-left corner at (drawX, drawY), clipped to the
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
  g_RichTextCurrentColorArgb = (&g_RichTextColorPalette0Argb)[colorPaletteIndex];
  g_RichTextCurrentShadowOffset = (&g_RichTextShadowOffsetPalette0)[colorPaletteIndex];
  g_RichTextSavedColorArgb = g_RichTextCurrentColorArgb;
  g_RichTextSavedShadowOffset = g_RichTextCurrentShadowOffset;
  while (RichTextCommandStream_DrawNextWrappedLine
                (clipBottom,clipRight,clipTop,clipLeft,maximumWidth,drawY,drawX,&lineHeight)) {
    drawY = drawY + lineHeight;
  }
  return;
}


/* Address: 0x0041D4A0.
   Draws one rich-text line: measures it first to align it (right or centred on penX, per the packed style) and
   to place the baseline below lineTopY, then interprets glyphs, colour, font, nested-stream and inline-image
   commands until the end of the stream or a line break.
*/
bool RichTextCommandStream_DrawSingleLine
          (UiPixelCoordinate clipBottom,UiPixelCoordinate clipRight,UiPixelCoordinate clipTop,
          UiPixelCoordinate clipLeft,UiPackedTextStyle packedStyle,uint16_t *commandStream,
          UiPixelCoordinate lineTopY,UiPixelCoordinate penX)

{
  int lineBaselineY;
  uint32_t alignShiftOrPaletteIndex;
  GraphicsSubresourceIndex glyphSubresource;
  int glyphAdvance;
  uint32_t imageWidth;
  uint16_t *commandCursor;
  RichTextExtentRegs lineExtent;
  GraphicsTextureLogicalSize imageSize;
  uint16_t *nestedReturnStack[RICHTEXT_NESTING_LIMIT]; /* the original's machine-stack chain */
  int nestedDepth;
  
  lineExtent = RichTextCommandStream_MeasureRegs(packedStyle,commandStream);
  alignShiftOrPaletteIndex = lineExtent.widthPixels;
  lineBaselineY = lineTopY + lineExtent.heightPixels;
  if ((packedStyle & TEXT_STYLE_ALIGN_RIGHT) != 0) {
    penX = penX - alignShiftOrPaletteIndex;
  }
  else if ((packedStyle & TEXT_STYLE_ALIGN_CENTER) != 0) {
    penX = penX - (alignShiftOrPaletteIndex >> 1);
  }
  alignShiftOrPaletteIndex = packedStyle >> TEXT_STYLE_PALETTE_SHIFT & TEXT_STYLE_INDEX_MASK;
  g_ActiveFontIndex = packedStyle >> TEXT_STYLE_FONT_SHIFT & TEXT_STYLE_INDEX_MASK;
  g_RichTextCurrentColorArgb = (&g_RichTextColorPalette0Argb)[alignShiftOrPaletteIndex];
  g_RichTextCurrentShadowOffset = (&g_RichTextShadowOffsetPalette0)[alignShiftOrPaletteIndex];
  nestedDepth = 0;
  g_RichTextSavedColorArgb = g_RichTextCurrentColorArgb;
  g_RichTextSavedShadowOffset = g_RichTextCurrentShadowOffset;
  for (;;) {
    commandCursor = commandStream;
    glyphSubresource = (GraphicsSubresourceIndex)(short)*commandCursor;
    commandStream = commandCursor + 1;
    if (glyphSubresource == 0) {
      if (nestedDepth == 0) {
        return false;
      }
      commandStream = (uint16_t *)((uint8_t *)nestedReturnStack[--nestedDepth] + RICHTEXT_NESTED_PAYLOAD_BYTES);
      continue;
    }
    if (-1 < (int)glyphSubresource) {
      glyphAdvance = FontGlyph_DrawBottomAligned
                        (clipBottom,clipRight,clipTop,clipLeft,glyphSubresource,lineBaselineY,penX);
      penX = penX + glyphAdvance;
      continue;
    }
    switch(glyphSubresource & RICHTEXT_OPCODE_MASK) {
    case RICHTEXT_OP_COLOR_PALETTE_0:
      g_RichTextCurrentColorArgb = g_RichTextColorPalette0Argb;
      g_RichTextCurrentShadowOffset = g_RichTextShadowOffsetPalette0;
      break;
    case RICHTEXT_OP_COLOR_PALETTE_1:
      g_RichTextCurrentColorArgb = g_RichTextColorPalette1Argb;
      g_RichTextCurrentShadowOffset = g_RichTextShadowOffsetPalette1;
      break;
    case RICHTEXT_OP_COLOR_PALETTE_2:
      g_RichTextCurrentColorArgb = g_RichTextColorPalette2Argb;
      g_RichTextCurrentShadowOffset = g_RichTextShadowOffsetPalette2;
      break;
    case RICHTEXT_OP_COLOR_PALETTE_3:
      g_RichTextCurrentColorArgb = g_RichTextColorPalette3Argb;
      g_RichTextCurrentShadowOffset = g_RichTextShadowOffsetPalette3;
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
      /* The eight payload code units are hex digits (only their low nibble is used, SHRD EDX,EAX,4 in the
         original): units 1-2 form the lowest colour byte, 7-8 the highest, each pair high digit first. */
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
      commandStream = *(uint16_t **)commandStream;
      break;
    case RICHTEXT_OP_JUMP_NESTED:
      commandStream = *(uint16_t **)commandStream;
      break;
    case RICHTEXT_OP_INLINE_IMAGE:
      /* payload: texture source at commandCursor + 1, subresource at commandCursor + 3; the image sits on the
         baseline */
      imageSize = g_GraphicsTextureSourceGetLogicalSize
                        (*(uint32_t *)(commandCursor + 3),*(GraphicsTextureSourceAsset **)commandStream);
      imageWidth = imageSize.logicalWidthPixels;
      g_GraphicsTextureSourceBlitSourceAlpha
                (clipBottom,clipRight,clipTop,clipLeft,lineBaselineY - imageSize.logicalHeightPixels,
                 penX,*(uint32_t *)(commandCursor + 3),*(GraphicsTextureSourceAsset **)commandStream,
                 g_FramebufferAccess);
      penX = penX + imageWidth;
      commandStream = commandCursor + RICHTEXT_RECORD_UNITS_INLINE_IMAGE;
    }
  }
}


/* Address: 0x0041B100.
   Walks one command stream (without following nested streams) and points every nested-stream command
   (0x18/0x19) whose selector matches at replacementPayload, so a text can have its placeholders bound to
   concrete sub-streams at run time.
*/
void RichTextCommandStream_PatchPayloadBySelector
          (RichTextCommandSelector selector,void *replacementPayload,uint16_t *stream)

{
  uint16_t commandCodeUnit;
  uint16_t *commandCursor;

  while ((commandCodeUnit = *(commandCursor = stream)) != 0) {
    stream = commandCursor + 1;
    if ((short)commandCodeUnit < 0) {
      switch(commandCodeUnit & RICHTEXT_OPCODE_MASK) {
      case RICHTEXT_OP_LITERAL_COLOR:
        stream = commandCursor + RICHTEXT_RECORD_UNITS_LITERAL_COLOR;
        break;
      case RICHTEXT_OP_INLINE_VALUE_0:
      case RICHTEXT_OP_INLINE_VALUE_1:
      case RICHTEXT_OP_INLINE_VALUE_2:
        stream = commandCursor + RICHTEXT_RECORD_UNITS_INLINE_VALUE;
        break;
      case RICHTEXT_OP_CALL_NESTED:
      case RICHTEXT_OP_JUMP_NESTED:
        /* payload: stream pointer at commandCursor + 1, selector at commandCursor + 3 */
        stream = commandCursor + RICHTEXT_RECORD_UNITS_NESTED;
        if (selector == *(int *)(commandCursor + 3)) {
          *(void **)(commandCursor + 1) = replacementPayload;
        }
        break;
      case RICHTEXT_OP_INLINE_IMAGE:
        stream = commandCursor + RICHTEXT_RECORD_UNITS_INLINE_IMAGE;
      }
    }
  }
}


/* Address: 0x0041B200.
   Walks one command stream (without following nested streams) and sets the texture source of every inline
   image command to textureSource, so a text's icons can be bound to the texture they are drawn from.
*/
void RichTextCommandStream_BindTextureSource(GraphicsTextureSourceAsset *textureSource,uint16_t *stream)

{
  uint16_t *commandCursor;
  uint16_t *streamCursor;
  uint16_t commandCodeUnit;

  streamCursor = stream;
  while ((commandCodeUnit = *(commandCursor = streamCursor)) != 0) {
    streamCursor = commandCursor + 1;
    if ((short)commandCodeUnit < 0) { /* RICHTEXT_COMMAND_FLAG set */
      switch(commandCodeUnit & RICHTEXT_OPCODE_MASK) {
      case RICHTEXT_OP_LITERAL_COLOR:
        streamCursor = commandCursor + RICHTEXT_RECORD_UNITS_LITERAL_COLOR;
        break;
      case RICHTEXT_OP_INLINE_VALUE_0:
      case RICHTEXT_OP_INLINE_VALUE_1:
      case RICHTEXT_OP_INLINE_VALUE_2:
        streamCursor = commandCursor + RICHTEXT_RECORD_UNITS_INLINE_VALUE;
        break;
      case RICHTEXT_OP_CALL_NESTED:
      case RICHTEXT_OP_JUMP_NESTED:
        streamCursor = commandCursor + RICHTEXT_RECORD_UNITS_NESTED;
        break;
      case RICHTEXT_OP_INLINE_IMAGE:
        /* the texture source pointer is the first payload dword, right after the command */
        *(GraphicsTextureSourceAsset **)streamCursor = textureSource;
        streamCursor = commandCursor + RICHTEXT_RECORD_UNITS_INLINE_IMAGE;
      }
    }
  }
  return;
}


/* Address: 0x0041B300.
   Sets the 32-bit payload of the commandOrdinal-th (0-based) inline-value command (0x14..0x16) of one command
   stream (nested streams are not followed) to payloadValue and returns false (CF clear); true (CF set) when
   the stream has fewer such commands. No caller and no function-pointer table entry for it was found in src/
   or src/generated/image_data.c.
*/
bool RichTextCommandStream_SetNthInlineValuePayload
          (RichTextCommandOrdinal commandOrdinal,RichTextCommandPayload32 payloadValue,
          uint16_t *commandStream)

{
  uint16_t commandCodeUnit;
  int remainingCount;
  uint16_t *commandCursor;
  
  remainingCount = commandOrdinal + 1;
  do {
    do {
      commandCursor = commandStream;
      commandCodeUnit = *commandCursor;
      if (commandCodeUnit == 0) {
        return true;
      }
      commandStream = commandCursor + 1;
    } while (-1 < (short)commandCodeUnit); /* skip glyphs up to the next RICHTEXT_COMMAND_FLAG unit */
    switch(commandCodeUnit & RICHTEXT_OPCODE_MASK) {
    case RICHTEXT_OP_LITERAL_COLOR:
      commandStream = commandCursor + RICHTEXT_RECORD_UNITS_LITERAL_COLOR;
      break;
    case RICHTEXT_OP_INLINE_VALUE_0:
    case RICHTEXT_OP_INLINE_VALUE_1:
    case RICHTEXT_OP_INLINE_VALUE_2:
      remainingCount--;
      commandStream = commandCursor + RICHTEXT_RECORD_UNITS_INLINE_VALUE;
      if (remainingCount == 0) {
        *(RichTextCommandPayload32 *)(commandCursor + 1) = payloadValue;
        return false;
      }
      break;
    case RICHTEXT_OP_CALL_NESTED:
    case RICHTEXT_OP_JUMP_NESTED:
    case RICHTEXT_OP_INLINE_IMAGE:
      commandStream = commandCursor + RICHTEXT_RECORD_UNITS_NESTED;
    }
  } while (true);
}


/* Address: 0x0041B420.
   Walks one command stream (without following nested streams) and sets the stream pointer of every
   nested-stream command (0x18/0x19) to nestedStreamPointerValue, regardless of its selector (compare
   RichTextCommandStream_PatchPayloadBySelector). No caller and no function-pointer table entry for it was
   found in src/ or src/generated/image_data.c.
*/
void RichTextCommandStream_PatchNestedStreamPointerPayloads
          (RichTextNestedStreamPointerValue32 nestedStreamPointerValue,uint16_t *commandStream)

{
  uint16_t *commandCursor;
  RichTextNestedStreamPointerValue32 *streamCursor;
  uint16_t commandCodeUnit;
  
  commandCursor = commandStream;
  while ((commandCodeUnit = *commandCursor) != 0) {
    streamCursor = (RichTextNestedStreamPointerValue32 *)(commandCursor + 1);
    if ((short)commandCodeUnit < 0) { /* RICHTEXT_COMMAND_FLAG set */
      switch(commandCodeUnit & RICHTEXT_OPCODE_MASK) {
      case RICHTEXT_OP_LITERAL_COLOR:
        streamCursor = (RichTextNestedStreamPointerValue32 *)(commandCursor + RICHTEXT_RECORD_UNITS_LITERAL_COLOR);
        break;
      case RICHTEXT_OP_INLINE_VALUE_0:
      case RICHTEXT_OP_INLINE_VALUE_1:
      case RICHTEXT_OP_INLINE_VALUE_2:
        streamCursor = (RichTextNestedStreamPointerValue32 *)(commandCursor + RICHTEXT_RECORD_UNITS_INLINE_VALUE);
        break;
      case RICHTEXT_OP_CALL_NESTED:
      case RICHTEXT_OP_JUMP_NESTED:
        /* the stream pointer is the first payload dword, right after the command */
        *streamCursor = nestedStreamPointerValue;
        streamCursor = (RichTextNestedStreamPointerValue32 *)(commandCursor + RICHTEXT_RECORD_UNITS_NESTED);
        break;
      case RICHTEXT_OP_INLINE_IMAGE:
        streamCursor = (RichTextNestedStreamPointerValue32 *)(commandCursor + RICHTEXT_RECORD_UNITS_INLINE_IMAGE);
      }
    }
    commandCursor = (uint16_t *)streamCursor;
  }
  return;
}


/* Address: 0x0041B520.
   Walks one command stream (without following nested streams) and sets both payload dwords of every
   inline-image command (0x1A): the texture source to textureSourceValue and the subresource to
   imageSubresourceValue (see RichTextCommandStream_BindTextureSource for the texture source alone). No caller
   and no function-pointer table entry for it was found in src/ or src/generated/image_data.c.
*/
void RichTextCommandStream_PatchInlineImagePayloads(RichTextOpcode1APayloadValue32 imageSubresourceValue,
          RichTextCommandPayload32 textureSourceValue,uint16_t *commandStream)

{
  uint16_t *commandCursor;
  RichTextCommandPayload32 *streamCursor;
  uint16_t commandCodeUnit;
  
  commandCursor = commandStream;
  while ((commandCodeUnit = *commandCursor) != 0) {
    streamCursor = (RichTextCommandPayload32 *)(commandCursor + 1);
    if ((short)commandCodeUnit < 0) { /* RICHTEXT_COMMAND_FLAG set */
      switch(commandCodeUnit & RICHTEXT_OPCODE_MASK) {
      case RICHTEXT_OP_LITERAL_COLOR:
        streamCursor = (RichTextCommandPayload32 *)(commandCursor + RICHTEXT_RECORD_UNITS_LITERAL_COLOR);
        break;
      case RICHTEXT_OP_INLINE_VALUE_0:
      case RICHTEXT_OP_INLINE_VALUE_1:
      case RICHTEXT_OP_INLINE_VALUE_2:
        streamCursor = (RichTextCommandPayload32 *)(commandCursor + RICHTEXT_RECORD_UNITS_INLINE_VALUE);
        break;
      case RICHTEXT_OP_CALL_NESTED:
      case RICHTEXT_OP_JUMP_NESTED:
        streamCursor = (RichTextCommandPayload32 *)(commandCursor + RICHTEXT_RECORD_UNITS_NESTED);
        break;
      case RICHTEXT_OP_INLINE_IMAGE:
        /* payload: texture source at commandCursor + 1, subresource at commandCursor + 3 */
        *streamCursor = textureSourceValue;
        *(RichTextOpcode1APayloadValue32 *)(commandCursor + 3) = imageSubresourceValue;
        streamCursor = (RichTextCommandPayload32 *)(commandCursor + RICHTEXT_RECORD_UNITS_INLINE_IMAGE);
      }
    }
    commandCursor = (uint16_t *)streamCursor;
  }
  return;
}


/* Address: 0x0041B620.
   Walks one command stream (without following nested streams) and sets the 32-bit payload of every
   inline-value command (0x14..0x16) to inlinePayloadValue. No caller and no function-pointer table entry
   for it was found in src/ or src/generated/image_data.c.
*/
void RichTextCommandStream_PatchInlinePayloads(RichTextInlinePayloadValue32 inlinePayloadValue,uint16_t *commandStream)

{
  uint16_t *commandCursor;
  RichTextInlinePayloadValue32 *streamCursor;
  uint16_t commandCodeUnit;
  
  commandCursor = commandStream;
  while ((commandCodeUnit = *commandCursor) != 0) {
    streamCursor = (RichTextInlinePayloadValue32 *)(commandCursor + 1);
    if ((short)commandCodeUnit < 0) { /* RICHTEXT_COMMAND_FLAG set */
      switch(commandCodeUnit & RICHTEXT_OPCODE_MASK) {
      case RICHTEXT_OP_LITERAL_COLOR:
        streamCursor = (RichTextInlinePayloadValue32 *)(commandCursor + RICHTEXT_RECORD_UNITS_LITERAL_COLOR);
        break;
      case RICHTEXT_OP_INLINE_VALUE_0:
      case RICHTEXT_OP_INLINE_VALUE_1:
      case RICHTEXT_OP_INLINE_VALUE_2:
        *streamCursor = inlinePayloadValue;
        streamCursor = (RichTextInlinePayloadValue32 *)(commandCursor + RICHTEXT_RECORD_UNITS_INLINE_VALUE);
        break;
      case RICHTEXT_OP_CALL_NESTED:
      case RICHTEXT_OP_JUMP_NESTED:
      case RICHTEXT_OP_INLINE_IMAGE:
        streamCursor = (RichTextInlinePayloadValue32 *)(commandCursor + RICHTEXT_RECORD_UNITS_NESTED);
      }
    }
    commandCursor = (uint16_t *)streamCursor;
  }
  return;
}


/* Address: 0x0041B720.
   Rewrites the command code unit of the commandOrdinal-th (0-based) inline-value command (0x14..0x16) of one
   command stream: keeps RICHTEXT_COMMAND_FLAG and opcode bits 0x14, clears the variant and the other bits, ORs
   in flagBits, and returns false (CF clear); true (CF set) when the stream has fewer such commands. The dword
   access also covers the low half of the payload, which the mask 0xFFFF8014 keeps. No caller and no
   function-pointer table entry for it was found in src/ or src/generated/image_data.c.
*/
bool RichTextCommandStream_SetNthInlineValueFlags(int commandOrdinal,uint32_t flagBits,uint32_t *commandStream)

{
  uint32_t *streamCursor;
  int remainingCount;
  uint32_t *commandCursor;
  uint16_t commandCodeUnit;
  
  remainingCount = commandOrdinal + 1;
  streamCursor = commandStream;
  do {
    do {
      commandCursor = streamCursor;
      commandCodeUnit = (uint16_t)*commandCursor;
      if (commandCodeUnit == 0) {
        return true;
      }
      streamCursor = (uint32_t *)((uint8_t *)commandCursor + 2);
    } while (-1 < (short)commandCodeUnit); /* skip glyphs up to the next RICHTEXT_COMMAND_FLAG unit */
    /* the cursors advance in bytes: record lengths are code units * 2 */
    switch(commandCodeUnit & RICHTEXT_OPCODE_MASK) {
    case RICHTEXT_OP_LITERAL_COLOR:
      streamCursor = (uint32_t *)((uint8_t *)commandCursor + RICHTEXT_RECORD_UNITS_LITERAL_COLOR * 2);
      break;
    case RICHTEXT_OP_INLINE_VALUE_0:
    case RICHTEXT_OP_INLINE_VALUE_1:
    case RICHTEXT_OP_INLINE_VALUE_2:
      remainingCount--;
      streamCursor = (uint32_t *)((uint8_t *)commandCursor + RICHTEXT_RECORD_UNITS_INLINE_VALUE * 2);
      if (remainingCount == 0) {
        *commandCursor = *commandCursor & (0xffff0000U | RICHTEXT_COMMAND_FLAG | RICHTEXT_OP_INLINE_VALUE_0);
        *commandCursor = *commandCursor | flagBits;
        return false;
      }
      break;
    case RICHTEXT_OP_CALL_NESTED:
    case RICHTEXT_OP_JUMP_NESTED:
    case RICHTEXT_OP_INLINE_IMAGE:
      streamCursor = (uint32_t *)((uint8_t *)commandCursor + RICHTEXT_RECORD_UNITS_NESTED * 2);
    }
  } while( true );
}


/* Address: 0x0041B840.
   Stores the variant (code & 3, i.e. 0..2 for opcodes 0x14..0x16) of the commandOrdinal-th (0-based)
   inline-value command of one command stream in *commandVariant and returns true; when the stream has fewer
   such commands, stores 0 and returns false. No caller and no function-pointer table entry for it was found
   in src/ or src/generated/image_data.c.
*/
bool RichTextCommandStream_QueryNthInlineValueVariant
          (int commandOrdinal,uint16_t *commandStream,uint32_t *commandVariant)

{
  uint16_t commandCodeUnit;
  int remainingCount;
  uint32_t *commandCursor;

  remainingCount = commandOrdinal + 1;
  do {
    do {
      commandCursor = (uint32_t *)commandStream;
      commandCodeUnit = (uint16_t)*commandCursor;
      if (commandCodeUnit == 0) {
        *commandVariant = 0;
        return false;
      }
      commandStream = (uint16_t *)commandCursor + 1;
    } while (-1 < (short)commandCodeUnit); /* skip glyphs up to the next RICHTEXT_COMMAND_FLAG unit */
    /* the cursors advance in bytes: record lengths are code units * 2 */
    switch(commandCodeUnit & RICHTEXT_OPCODE_MASK) {
    case RICHTEXT_OP_LITERAL_COLOR:
      commandStream = (uint16_t *)commandCursor + RICHTEXT_RECORD_UNITS_LITERAL_COLOR;
      break;
    case RICHTEXT_OP_INLINE_VALUE_0:
    case RICHTEXT_OP_INLINE_VALUE_1:
    case RICHTEXT_OP_INLINE_VALUE_2:
      remainingCount--;
      commandStream = (uint16_t *)commandCursor + RICHTEXT_RECORD_UNITS_INLINE_VALUE;
      if (remainingCount == 0) {
        *commandVariant = *commandCursor & 3;
        return true;
      }
      break;
    case RICHTEXT_OP_CALL_NESTED:
    case RICHTEXT_OP_JUMP_NESTED:
    case RICHTEXT_OP_INLINE_IMAGE:
      commandStream = (uint16_t *)commandCursor + RICHTEXT_RECORD_UNITS_NESTED;
    }
  } while( true );
}


/* Address: 0x0041B950.
   Converts a rich-text command stream into a NUL-terminated 8-bit string (for Win32 text such as message boxes):
   follows nested streams, turns the fixed-space and line-break commands into ' ' and CR LF, drops all other
   commands and every code unit above 0xFF. Returns true when the whole text fit; on overflow (or nesting deeper
   than RICHTEXT_NESTING_LIMIT) the output is cut and terminated and false is returned. (The original also
   returned the byte count including the terminator, which no caller reads.)
*/
bool RichTextCommandStream_CopyToNarrow
          (TextOutputCapacityBytes capacityBytes,uint8_t *destination,uint16_t *source)

{
  uint16_t *readCursor;
  uint32_t remainingCapacityBytes;
  uint16_t *commandCursor;
  uint16_t *streamCursor;
  bool newlineCapacityUnderflow;
  uint16_t *nestedReturnStack[RICHTEXT_NESTING_LIMIT]; /* the original's machine-stack chain */
  int nestedDepth;
  uint16_t commandOrCodeUnit;
  
  nestedDepth = 0;
  remainingCapacityBytes = capacityBytes;
  readCursor = source;
  while( true ) {
    while ((commandOrCodeUnit = *(commandCursor = readCursor)) != 0) {
      streamCursor = commandCursor + 1;
      if ((short)commandOrCodeUnit < 0) {
        readCursor = streamCursor;
        switch(commandOrCodeUnit & RICHTEXT_OPCODE_MASK) {
        case RICHTEXT_OP_LITERAL_COLOR:
          readCursor = commandCursor + RICHTEXT_RECORD_UNITS_LITERAL_COLOR;
          break;
        case RICHTEXT_OP_FIXED_SPACE:
          remainingCapacityBytes--;
          if (remainingCapacityBytes == 0) {
            destination[-1] = 0;
            return false;
          }
          *destination = ' ';
          destination++;
          readCursor = streamCursor;
          break;
        case RICHTEXT_OP_LINE_BREAK:
          newlineCapacityUnderflow = remainingCapacityBytes < 2;
          remainingCapacityBytes = remainingCapacityBytes - 2;
          if (newlineCapacityUnderflow || remainingCapacityBytes == 0) {
            destination[-1] = 0;
            return false;
          }
          destination[0] = '\r';
          destination[1] = '\n';
          destination += 2;
          readCursor = streamCursor;
          break;
        case RICHTEXT_OP_INLINE_VALUE_0:
        case RICHTEXT_OP_INLINE_VALUE_1:
        case RICHTEXT_OP_INLINE_VALUE_2:
          readCursor = commandCursor + RICHTEXT_RECORD_UNITS_INLINE_VALUE;
          break;
        case RICHTEXT_OP_CALL_NESTED:
          if (nestedDepth == RICHTEXT_NESTING_LIMIT) {
            destination[-1] = 0;
            return false;
          }
          nestedReturnStack[nestedDepth++] = streamCursor;
          readCursor = *(uint16_t **)streamCursor;
          break;
        case RICHTEXT_OP_JUMP_NESTED:
          readCursor = *(uint16_t **)streamCursor;
          break;
        case RICHTEXT_OP_INLINE_IMAGE:
          readCursor = commandCursor + RICHTEXT_RECORD_UNITS_INLINE_IMAGE;
        }
      }
      else {
        readCursor = streamCursor;
        if ((commandOrCodeUnit & 0xff00) == 0) {
          remainingCapacityBytes--;
          if (remainingCapacityBytes == 0) {
            destination[-1] = 0;
            return false;
          }
          *destination = (uint8_t)commandOrCodeUnit;
          destination++;
          readCursor = streamCursor;
        }
      }
    }
    if (nestedDepth == 0) break;
    readCursor = (uint16_t *)((uint8_t *)nestedReturnStack[--nestedDepth] + RICHTEXT_NESTED_PAYLOAD_BYTES);
  }
  if (0 < (int)remainingCapacityBytes) {
    *destination = 0;
    return true;
  }
  destination[-1] = 0;
  return false;
}


/* Address: 0x0041BCB0.
   Leftover of the TXT2STR converter (no caller and no function-pointer table entry in src/ or
   src/generated/image_data.c): compiles text markup into a 'str' string asset. Text between '#<' and '#>'
   becomes one NUL-terminated, dword-padded rich-text string keyed by the last '#ddd' number (text outside is
   ignored); further '#' escapes select a code page ('#@'..'#~'), raw command codes ('#!'), a soft hyphen
   ('#-', emitted also outside a tag), a literal '#' ('##') or a line continuation, a CR inside a tag is a line
   break, and '#.' ends the input and groups the strings by key: a 0x200-byte header, then per key (in the order
   of the key's first string) a 0x10-byte group header {size, string count, key, 0}, the string offsets
   (relative to the group header) and the strings.
   Returns true with the new asset in *outAsset on success. On failure returns false with the error value in
   *outError: for an invalid character (control byte, unknown escape, misplaced tag) the address of the
   formatted "TXT2STR: unknown character" message (with its byte offset), when the arena has no free block the
   allocator's error, when the arena space runs out FATAL_ERROR_GENERAL_FAILURE. On success the original also
   returns the asset size in ECX and asset + 0x100 in EDX; they are dropped (there is no caller).
*/
bool RichTextMarkup_ParseAndBuildStringAsset(uint8_t *markupBytes,void **outAsset,uint32_t *outError)

{
  /* Rewritten from the assembly (0x0041BCB0-0x0041C8C2). The original pushes a (string start, key) pair per
     '#<' on the machine stack (and an (end of output, -1) sentinel at '#.'); tagStarts/tagKeys hold those
     pairs in push order, so string k ends at tagStarts[k + 1]. More than RICHTEXT_MARKUP_TAG_LIMIT - 1 tags
     (the original is limited only by its stack) fail like an arena overflow. */
  static uint16_t *tagStarts[RICHTEXT_MARKUP_TAG_LIMIT];
  static int32_t tagKeys[RICHTEXT_MARKUP_TAG_LIMIT];
  uint32_t arenaError;
  uint32_t largestBlockSize;
  uint16_t *memory;
  uint16_t *outputCursor;
  uint32_t remainingCapacityBytes;
  uint8_t *markupCursor;
  uint32_t markupByte;
  uint32_t codeUnitBias;
  int32_t key;
  bool insideTag;
  uint32_t tagCount;
  uint32_t *asset;
  TextResourceAssetHeader *header;
  AssetBuildTimestampSet *timestamps;
  uint32_t *assetCursor;
  uint32_t *groupHeader;
  uint32_t *offsetCursor;
  uint32_t groupCount;
  int32_t groupKey;
  uint32_t spanBytes;
  uint32_t dwordCount;
  uint32_t *copySource;
  uint32_t first;
  uint32_t index;
  uint32_t assetSize;

  arenaError = g_MemoryApi.allocLargestFreeBlock((void **)&memory,&largestBlockSize);
  if (arenaError != 0) {
    *outError = arenaError;
    return false;
  }
  remainingCapacityBytes = largestBlockSize;
  asset = NULL;
  markupCursor = markupBytes;
  outputCursor = memory;
  key = 0;
  codeUnitBias = 0;
  insideTag = false;
  tagCount = 0;
  for (;;) {
    markupByte = *markupCursor++;
    switch (markupByte) {
    /* control characters other than tab, LF and CR, and DEL */
    case 0: case 1: case 2: case 3: case 4: case 5: case 6: case 7: case 8:
    case 11: case 12: case 14: case 15:
    case 16: case 17: case 18: case 19: case 20: case 21: case 22: case 23:
    case 24: case 25: case 26: case 27: case 28: case 29: case 30: case 31:
    case 127:
      goto reportInvalidCharacter;
    case '\t':
    case '\n':
      continue;
    case '\r':
      if (insideTag) {
        if (remainingCapacityBytes <= 2) goto freePrimaryBufferAndFail;
        remainingCapacityBytes -= 2;
        *outputCursor++ = RICHTEXT_COMMAND_FLAG | RICHTEXT_OP_LINE_BREAK;
      }
      continue;
    default:
emitCodeUnit:
      if (insideTag) {
        if (remainingCapacityBytes <= 2) goto freePrimaryBufferAndFail;
        remainingCapacityBytes -= 2;
        *outputCursor++ = (uint16_t)(markupByte + codeUnitBias);
      }
      continue;
    case '#':
      break;
    }
    markupByte = *markupCursor++;
    switch (markupByte) {
    default:
      goto reportInvalidCharacter;
    case '\n':
    case '\r':
      /* line continuation: skip the line end */
      while ((markupByte = *markupCursor) < ' ') {
        markupCursor++;
        if ((markupByte != '\n') && (markupByte != '\r')) goto reportInvalidCharacter;
      }
      continue;
    case '!':
      codeUnitBias = RICHTEXT_MARKUP_COMMAND_BIAS; /* '@'..'_' become RICHTEXT_COMMAND_FLAG | 0x00..0x1F */
      continue;
    case '#':
      goto emitCodeUnit;
    case '-':
      /* no inside-tag test in the original */
      if (remainingCapacityBytes <= 2) goto freePrimaryBufferAndFail;
      remainingCapacityBytes -= 2;
      *outputCursor++ = RICHTEXT_COMMAND_FLAG | RICHTEXT_OP_SOFT_HYPHEN;
      continue;
    case '0': case '1': case '2': case '3': case '4': case '5': case '6': case '7': case '8': case '9':
      /* '#ddd': three decimal digits, the key of the following tags (EDX in the original) */
      key = (int32_t)(markupByte - '0') * 10;
      if ((markupCursor[0] < '0') || ('9' < markupCursor[0])) goto reportInvalidCharacter;
      key = key + (markupCursor[0] - '0');
      if ((markupCursor[1] < '0') || ('9' < markupCursor[1])) goto reportInvalidCharacter;
      key = key * 10 + (markupCursor[1] - '0');
      markupCursor += 2;
      continue;
    case '<':
      if (insideTag) goto reportInvalidCharacter;
      /* PUSH EDI / PUSH EDX: the string starts here and gets the current key */
      if (tagCount >= RICHTEXT_MARKUP_TAG_LIMIT - 1) goto freePrimaryBufferAndFail;
      tagStarts[tagCount] = outputCursor;
      tagKeys[tagCount] = key;
      tagCount++;
      insideTag = true;
      continue;
    case '>':
      /* ends the string: NUL terminator, padded to a dword boundary */
      if (!insideTag) goto reportInvalidCharacter;
      insideTag = false;
      if (((uint32_t)(uintptr_t)outputCursor & 2) == 0) {
        if (remainingCapacityBytes <= 4) goto freePrimaryBufferAndFail;
        remainingCapacityBytes -= 4;
        outputCursor[0] = 0;
        outputCursor[1] = 0;
        outputCursor += 2;
      }
      else {
        if (remainingCapacityBytes <= 2) goto freePrimaryBufferAndFail;
        remainingCapacityBytes -= 2;
        *outputCursor++ = 0;
      }
      continue;
    case '.':
      if ((tagCount == 0) || insideTag) goto reportInvalidCharacter;
      break;
    case '@': case 'A': case 'B': case 'C': case 'D': case 'E': case 'F': case 'G':
    case 'H': case 'I': case 'J': case 'K': case 'L': case 'M': case 'N': case 'O':
    case 'P': case 'Q': case 'R': case 'S': case 'T': case 'U': case 'V': case 'W':
    case 'X': case 'Y': case 'Z': case '[': case '\\': case ']': case '^': case '_':
    case '`': case 'a': case 'b': case 'c': case 'd': case 'e': case 'f': case 'g':
    case 'h': case 'i': case 'j': case 'k': case 'l': case 'm': case 'n': case 'o':
    case 'p': case 'q': case 'r': case 's': case 't': case 'u': case 'v': case 'w':
    case 'x': case 'y': case 'z': case '{': case '|': case '}': case '~':
      /* '#@'..'#~': code page select, following bytes are emitted + (c - '@') * 0x80 */
      codeUnitBias = markupByte * RICHTEXT_MARKUP_CODE_PAGE_UNITS - '@' * RICHTEXT_MARKUP_CODE_PAGE_UNITS;
      continue;
    }
    break; /* only '#.' gets here: the end of the input */
  }

  /* PUSH EDI / PUSH -1: the sentinel, its start is the end of the last string */
  tagStarts[tagCount] = outputCursor;
  tagKeys[tagCount] = -1;
  tagCount++;
  if (g_MemoryApi.shrinkInPlace((uint32_t)((uint8_t *)outputCursor - (uint8_t *)memory),memory) != 0) {
    goto freePrimaryBufferAndFail;
  }
  if (g_MemoryApi.allocLargestFreeBlock((void **)&asset,&largestBlockSize) != 0) goto freePrimaryBufferAndFail;
  if (largestBlockSize <= sizeof(TextResourceAssetHeader)) goto freeAssetBufferAndFail;
  remainingCapacityBytes = largestBlockSize - sizeof(TextResourceAssetHeader);

  for (index = 0; index < sizeof(TextResourceAssetHeader) / sizeof(uint32_t); index++) {
    asset[index] = 0;
  }
  assetCursor = asset + sizeof(TextResourceAssetHeader) / sizeof(uint32_t);
  groupCount = 0;
  for (;;) {
    /* the next key in the order of its first string; copied strings carry key -1 */
    for (first = 0; (first < tagCount) && (tagKeys[first] == -1); first++) {
    }
    if (first == tagCount) break;
    groupKey = tagKeys[first];
    if (remainingCapacityBytes <= sizeof(TextResourceLocaleBlockPrefix)) goto freeAssetBufferAndFail;
    remainingCapacityBytes -= sizeof(TextResourceLocaleBlockPrefix);
    /* groupHeader is a TextResourceLocaleBlockPrefix: [0] block size, [1] string count, [2] key */
    groupHeader = assetCursor;
    groupHeader[2] = (uint32_t)groupKey;
    groupHeader[0] = sizeof(TextResourceLocaleBlockPrefix);
    groupHeader[1] = 0;
    for (index = first; index < tagCount; index++) {
      if (tagKeys[index] == groupKey) {
        groupHeader[1]++;
        spanBytes = (uint32_t)((uint8_t *)tagStarts[index + 1] - (uint8_t *)tagStarts[index]) + 4;
        groupHeader[0] += spanBytes;
        if (remainingCapacityBytes <= spanBytes) goto freeAssetBufferAndFail;
        remainingCapacityBytes -= spanBytes;
      }
    }
    groupCount++;
    offsetCursor = groupHeader + sizeof(TextResourceLocaleBlockPrefix) / sizeof(uint32_t);
    assetCursor = offsetCursor + groupHeader[1];
    for (index = 0; index < tagCount; index++) {
      if (tagKeys[index] == groupKey) {
        tagKeys[index] = -1;
        *offsetCursor++ = (uint32_t)((uint8_t *)assetCursor - (uint8_t *)groupHeader);
        copySource = (uint32_t *)tagStarts[index];
        for (dwordCount = (uint32_t)((uint8_t *)tagStarts[index + 1] - (uint8_t *)tagStarts[index]) >> 2;
             dwordCount != 0; dwordCount--) {
          *assetCursor++ = *copySource++;
        }
      }
    }
  }
  g_MemoryApi.free(memory);
  assetSize = (uint32_t)((uint8_t *)assetCursor - (uint8_t *)asset);
  g_MemoryApi.shrinkInPlace(assetSize,asset);
  header = (TextResourceAssetHeader *)asset;
  header->localeCountHeader.localeBlockCount = groupCount; /* +0xB0 */
  header->localeCountHeader.common.allocationSizeBytes = assetSize;
  header->localeCountHeader.common.magic = ASSET_MAGIC_STR;
  header->localeCountHeader.common.formatVersion = 1;
  header->localeCountHeader.common.converterVersion = 0;
  timestamps = &header->localeCountHeader.common.buildMetadata.timestamps;
  timestamps->timeValue0 = g_LocaleGetPackedCurrentTime();
  timestamps->timeValue1 = timestamps->timeValue0;
  timestamps->timeValue2 = timestamps->timeValue0;
  timestamps->dateValue0 = g_LocaleGetPackedCurrentDate();
  timestamps->dateValue1 = timestamps->dateValue0;
  timestamps->dateValue2 = timestamps->dateValue0;
  g_LocaleCopyDefaultComputerLabelUtf16(header->localeCountHeader.common.buildMetadata.names.producerName);
  g_LocaleCopyDefaultComputerLabelUtf16(header->localeCountHeader.common.buildMetadata.names.sourceName);
  *outAsset = asset;
  return true;

freeAssetBufferAndFail:
  g_MemoryApi.free(asset);
freePrimaryBufferAndFail:
  g_MemoryApi.free(memory);
  *outError = FATAL_ERROR_GENERAL_FAILURE;
  return false;

reportInvalidCharacter:
  g_MemoryApi.free(memory);
  /* the byte offset after the offending character(s), written into the message at +0x4C */
  g_WideNumberFormatUtf16
            (WIDE_FORMAT_WRITE_TERMINATOR,0,10,1,(int32_t)(markupCursor - markupBytes),
             &u_error__TXT2STR__unknown_characte_0041afac[RICHTEXT_MARKUP_ERROR_OFFSET_UNIT]);
  *outError = (uint32_t)(uintptr_t)u_error__TXT2STR__unknown_characte_0041afac;
  return false;
}


/* Address: 0x0041C8D0.
   Copies a rich-text command stream into a bounded buffer with every nested stream (0x18/0x19) inlined, so the
   copy no longer depends on the streams it referenced. Commands are normalised to RICHTEXT_COMMAND_FLAG | opcode;
   payload records are copied unchanged. Returns true when the whole text fit and stores the byte count without
   the terminator in *outBytesWritten (may be NULL); on overflow (or nesting deeper than RICHTEXT_NESTING_LIMIT)
   the output is cut and terminated, *outBytesWritten is left untouched and false is returned.
*/
bool RichTextCommandStream_CopyExpanded
          (TextOutputCapacityBytes capacityBytes,uint16_t *destination,uint16_t *source,
           uint32_t *outBytesWritten)

{
  uint16_t commandCodeUnit;
  int wordsRemaining;
  uint16_t *nextSource;
  uint16_t *destinationCursor;
  bool capacityUnderflow;
  uint16_t *nestedReturnStack[RICHTEXT_NESTING_LIMIT]; /* the original's machine-stack chain */
  int nestedDepth;
  
  nestedDepth = 0;
  destinationCursor = destination;
  while( true ) {
    while ((commandCodeUnit = *source) != 0) {
      nextSource = source + 1;
      if ((short)commandCodeUnit < 0) {
        switch(commandCodeUnit & RICHTEXT_OPCODE_MASK) {
        default:
          capacityUnderflow = capacityBytes < 2;
          capacityBytes = capacityBytes - 2;
          if (capacityUnderflow || capacityBytes == 0) {
            destinationCursor[-1] = 0;
            return false;
          }
          *destinationCursor = commandCodeUnit & RICHTEXT_OPCODE_MASK | RICHTEXT_COMMAND_FLAG;
          destinationCursor++;
          source = nextSource;
          break;
        case RICHTEXT_OP_LITERAL_COLOR:
          capacityUnderflow = capacityBytes < RICHTEXT_RECORD_UNITS_LITERAL_COLOR * 2;
          capacityBytes = capacityBytes - RICHTEXT_RECORD_UNITS_LITERAL_COLOR * 2;
          if (capacityUnderflow || capacityBytes == 0) {
            destinationCursor[-1] = 0;
            return false;
          }
          for (wordsRemaining = RICHTEXT_RECORD_UNITS_LITERAL_COLOR; wordsRemaining != 0; wordsRemaining--) {
            *destinationCursor = *source;
            source++;
            destinationCursor++;
          }
          break;
        case RICHTEXT_OP_INLINE_VALUE_0:
        case RICHTEXT_OP_INLINE_VALUE_1:
        case RICHTEXT_OP_INLINE_VALUE_2:
          capacityUnderflow = capacityBytes < RICHTEXT_RECORD_UNITS_INLINE_VALUE * 2;
          capacityBytes = capacityBytes - RICHTEXT_RECORD_UNITS_INLINE_VALUE * 2;
          if (capacityUnderflow || capacityBytes == 0) {
            destinationCursor[-1] = 0;
            return false;
          }
          for (wordsRemaining = RICHTEXT_RECORD_UNITS_INLINE_VALUE; wordsRemaining != 0; wordsRemaining--) {
            *destinationCursor = *source;
            source++;
            destinationCursor++;
          }
          break;
        case RICHTEXT_OP_CALL_NESTED:
          if (nestedDepth == RICHTEXT_NESTING_LIMIT) {
            destinationCursor[-1] = 0;
            return false;
          }
          nestedReturnStack[nestedDepth++] = nextSource;
          source = *(uint16_t **)nextSource;
          break;
        case RICHTEXT_OP_JUMP_NESTED:
          source = *(uint16_t **)nextSource;
          break;
        case RICHTEXT_OP_INLINE_IMAGE:
          capacityUnderflow = capacityBytes < RICHTEXT_RECORD_UNITS_INLINE_IMAGE * 2;
          capacityBytes = capacityBytes - RICHTEXT_RECORD_UNITS_INLINE_IMAGE * 2;
          if (capacityUnderflow || capacityBytes == 0) {
            destinationCursor[-1] = 0;
            return false;
          }
          for (wordsRemaining = RICHTEXT_RECORD_UNITS_INLINE_IMAGE; wordsRemaining != 0; wordsRemaining--) {
            *destinationCursor = *source;
            source++;
            destinationCursor++;
          }
        }
      }
      else {
        capacityUnderflow = capacityBytes < 2;
        capacityBytes = capacityBytes - 2;
        if (capacityUnderflow || capacityBytes == 0) {
          destinationCursor[-1] = 0;
          return false;
        }
        *destinationCursor = commandCodeUnit;
        destinationCursor++;
        source = nextSource;
      }
    }
    if (nestedDepth == 0) break;
    source = (uint16_t *)((uint8_t *)nestedReturnStack[--nestedDepth] + RICHTEXT_NESTED_PAYLOAD_BYTES);
  }
  if (1 < (int)capacityBytes) {
    *destinationCursor = 0;
    if (outBytesWritten != NULL) {
      *outBytesWritten = (int)destinationCursor - (int)destination;
    }
    return true;
  }
  destinationCursor[-1] = 0;
  return false;
}


/* Address: 0x0041CF30.
   Measures one line of a rich-text command stream (up to its end or the first line break), following nested
   streams and font changes and including inline images. Returns the total width in EAX and the tallest glyph or
   image in EDX; used to align a line before it is drawn.
*/
RichTextExtentRegs RichTextCommandStream_MeasureRegs(UiPackedTextStyle packedStyle,uint16_t *commandStream)

{
  /* Rewritten from the assembly (0x0041CF30-0x0041D0E0). Command 0x18 enters a nested stream and pushes
     the return position on the machine stack; its terminator pops it and resumes 8 bytes later.
     Ghidra turned that stack into a counter, so nested text was measured forever. */
  uint16_t *returnStack[RICHTEXT_NESTING_LIMIT];
  int nesting = 0;
  RichTextExtentRegs extent;
  uint32_t glyphWidth;
  uint32_t glyphLineHeight;
  GraphicsTextureLogicalSize textureSize;
  uint16_t *command;
  int value;

  extent.widthPixels = 0;
  extent.heightPixels = 0;
  g_ActiveFontIndex = packedStyle >> TEXT_STYLE_FONT_SHIFT & TEXT_STYLE_INDEX_MASK;
  for (;;) {
    command = commandStream;
    value = (int)(short)*command;
    commandStream = command + 1;
    if (value == 0) {
      if (nesting == 0) {
        return extent;
      }
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
      commandStream = *(uint16_t **)commandStream;
      break;
    case RICHTEXT_OP_JUMP_NESTED:
      commandStream = *(uint16_t **)commandStream;
      break;
    case RICHTEXT_OP_INLINE_IMAGE:
      textureSize = g_GraphicsTextureSourceGetLogicalSize
                              (*(uint32_t *)(command + 3),*(GraphicsTextureSourceAsset **)commandStream);
      extent.widthPixels = extent.widthPixels + textureSize.logicalWidthPixels;
      commandStream = command + RICHTEXT_RECORD_UNITS_INLINE_IMAGE;
      if (extent.heightPixels < textureSize.logicalHeightPixels) {
        extent.heightPixels = textureSize.logicalHeightPixels;
      }
      break;
    }
  }
}


/* Address: 0x0041D0F0.
   Measures the next line of the flattened rich-text runtime buffer that fits into maximumWidth, wrapping after
   the last space or soft hyphen that still fits (or at a line break), and advances
   g_RichTextRuntimeBufferUsedWords past it. Font commands are applied on the way. Stores the tallest glyph or
   image height of the line in *lineHeight (also for the last line) and returns true while more lines follow,
   false when this line ends the text.
*/
bool RichTextCommandStream_MeasureNextWrappedLine(UiPixelExtent maximumWidth,UiPixelExtent *lineHeight)

{
  GraphicsSubresourceIndex glyphSubresource;
  uint32_t lineWidth;
  uint32_t maxLineHeight;
  uint8_t *commandCursor;
  uint8_t *readCursor;
  uint32_t glyphWidth;
  uint32_t glyphLineHeight;
  GraphicsTextureLogicalSize imageSize;
  uint8_t *wrapPoint;

  FontGlyph_GetLogicalSizeActiveFont(0,&glyphLineHeight);
  lineWidth = 0;
  wrapPoint = NULL;
  maxLineHeight = glyphLineHeight;
  readCursor = g_FontRuntimeBuffer + g_RichTextRuntimeBufferUsedWords * sizeof(uint16_t);
  for (;;) {
    commandCursor = readCursor;
    glyphSubresource = (GraphicsSubresourceIndex)*(short *)commandCursor;
    readCursor = commandCursor + 2;
    if (glyphSubresource == ' ') {
      /* A space is a wrap opportunity while the line up to it still fits. */
      glyphWidth = FontGlyph_GetLogicalSizeActiveFont(' ',&glyphLineHeight);
      if (maximumWidth < lineWidth) break;
      lineWidth = lineWidth + glyphWidth;
      wrapPoint = readCursor;
      continue;
    }
    if (glyphSubresource == 0) {
      /* End of stream: handled like RICHTEXT_OP_LINE_BREAK. */
      if (lineWidth <= maximumWidth) {
        wrapPoint = readCursor;
      }
      break;
    }
    if (-1 < (int)glyphSubresource) { /* no RICHTEXT_COMMAND_FLAG: a glyph */
      glyphWidth = FontGlyph_GetLogicalSizeActiveFont(glyphSubresource,&glyphLineHeight);
      lineWidth = lineWidth + glyphWidth;
      if (maxLineHeight < glyphLineHeight) {
        maxLineHeight = glyphLineHeight;
      }
      continue;
    }
    /* the cursors are byte pointers: record lengths are code units * 2 */
    switch(glyphSubresource & RICHTEXT_OPCODE_MASK) {
    case RICHTEXT_OP_LITERAL_COLOR:
      readCursor = commandCursor + RICHTEXT_RECORD_UNITS_LITERAL_COLOR * sizeof(uint16_t);
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
      if (maxLineHeight < glyphLineHeight) {
        maxLineHeight = glyphLineHeight;
      }
      break;
    case RICHTEXT_OP_SOFT_HYPHEN:
      /* Soft hyphen: a wrap opportunity when the hyphen still fits. */
      glyphWidth = FontGlyph_GetLogicalSizeActiveFont('-',&glyphLineHeight);
      if (maximumWidth < glyphWidth + lineWidth)
        goto commitWrapBoundary;
      wrapPoint = readCursor;
      break;
    case RICHTEXT_OP_LINE_BREAK:
      if (lineWidth <= maximumWidth) {
        wrapPoint = readCursor;
      }
      goto commitWrapBoundary;
    case RICHTEXT_OP_INLINE_IMAGE:
      /* payload: texture source pointer (code units 1-2), subresource (code units 3-4) */
      imageSize = g_GraphicsTextureSourceGetLogicalSize
                        (*(uint32_t *)(commandCursor + 6),*(GraphicsTextureSourceAsset **)readCursor);
      lineWidth = lineWidth + imageSize.logicalWidthPixels;
      readCursor = commandCursor + RICHTEXT_RECORD_UNITS_INLINE_IMAGE * sizeof(uint16_t);
      if (maxLineHeight < imageSize.logicalHeightPixels) {
        maxLineHeight = imageSize.logicalHeightPixels;
      }
    }
  }
commitWrapBoundary:
  if (wrapPoint == NULL) {
    wrapPoint = readCursor;
  }
  g_RichTextRuntimeBufferUsedWords = (uint32_t)((int)wrapPoint - (int)g_FontRuntimeBuffer) >> 1;
  *lineHeight = maxLineHeight;
  return *(short *)(wrapPoint - 2) != 0; /* false: the line ended at the stream terminator */
}


/* Address: 0x0041D9F0.
   Draws the next line of the flattened rich-text runtime buffer at (drawX, drawY), wrapped to maximumWidth:
   a measure pass with the rules of RichTextCommandStream_MeasureNextWrappedLine finds the wrap point and the
   line height, then the draw pass renders glyphs, images and colour/font commands up to it (drawing the hyphen
   when the line wraps at a soft hyphen) and advances g_RichTextRuntimeBufferUsedWords. Stores the line height in
   *lineAdvance (also for the last line) and returns true while more lines follow, false when this line ends
   the text. Called directly by RichTextCommandStream_DrawWrappedBlock.
*/
bool RichTextCommandStream_DrawNextWrappedLine
          (UiPixelCoordinate clipBottom,UiPixelCoordinate clipRight,UiPixelCoordinate clipTop,
          UiPixelCoordinate clipLeft,UiPixelExtent maximumWidth,UiPixelCoordinate drawY,
          UiPixelCoordinate drawX,UiPixelExtent *lineAdvance)

{
  int lineBottom;
  GraphicsSubresourceIndex glyphSubresource;
  int glyphAdvance;
  uint32_t savedFontIndexOrImageWidth;
  uint32_t lineWidth;
  uint32_t lineHeight;
  uint8_t *measureCommand;
  uint8_t *scanCursor;
  uint8_t *drawCursor;
  uint32_t glyphWidth;
  uint32_t glyphLineHeight;
  GraphicsTextureLogicalSize imageSize;
  uint8_t *wrapPoint;
  
  FontGlyph_GetLogicalSizeActiveFont(0,&glyphLineHeight);
  lineWidth = 0;
  drawCursor = g_FontRuntimeBuffer + g_RichTextRuntimeBufferUsedWords * sizeof(uint16_t);
  wrapPoint = NULL;
  lineHeight = glyphLineHeight;
  scanCursor = drawCursor;
  savedFontIndexOrImageWidth = g_ActiveFontIndex; /* font commands of the measure pass are undone below */
  /* Measure pass (same rules as RichTextCommandStream_MeasureNextWrappedLine): find the wrap point and
     the line height. The cursors are byte pointers: record lengths are code units * 2. */
  for (;;) {
    measureCommand = scanCursor;
    glyphSubresource = (GraphicsSubresourceIndex)*(short *)measureCommand;
    scanCursor = measureCommand + 2;
    if (glyphSubresource == ' ') {
      glyphWidth = FontGlyph_GetLogicalSizeActiveFont(' ',&glyphLineHeight);
      if (maximumWidth < lineWidth) break;
      lineWidth = lineWidth + glyphWidth;
      wrapPoint = scanCursor;
      continue;
    }
    if (glyphSubresource == 0) {
      if (lineWidth <= maximumWidth) {
        wrapPoint = scanCursor;
      }
      break;
    }
    if (-1 < (int)glyphSubresource) { /* no RICHTEXT_COMMAND_FLAG: a glyph */
      glyphWidth = FontGlyph_GetLogicalSizeActiveFont(glyphSubresource,&glyphLineHeight);
      lineWidth = lineWidth + glyphWidth;
      if (lineHeight < glyphLineHeight) {
        lineHeight = glyphLineHeight;
      }
      continue;
    }
    switch(glyphSubresource & RICHTEXT_OPCODE_MASK) {
    case RICHTEXT_OP_LITERAL_COLOR:
      scanCursor = measureCommand + RICHTEXT_RECORD_UNITS_LITERAL_COLOR * sizeof(uint16_t);
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
      if (lineHeight < glyphLineHeight) {
        lineHeight = glyphLineHeight;
      }
      break;
    case RICHTEXT_OP_SOFT_HYPHEN:
      glyphWidth = FontGlyph_GetLogicalSizeActiveFont('-',&glyphLineHeight);
      if (maximumWidth < glyphWidth + lineWidth)
        goto commitWrapBoundary;
      wrapPoint = scanCursor;
      break;
    case RICHTEXT_OP_LINE_BREAK:
      if (lineWidth <= maximumWidth) {
        wrapPoint = scanCursor;
      }
      goto commitWrapBoundary;
    case RICHTEXT_OP_INLINE_IMAGE:
      /* payload: texture source pointer (code units 1-2), subresource (code units 3-4) */
      imageSize = g_GraphicsTextureSourceGetLogicalSize
                         (*(uint32_t *)(measureCommand + 6),*(GraphicsTextureSourceAsset **)scanCursor);
      lineWidth = lineWidth + imageSize.logicalWidthPixels;
      scanCursor = measureCommand + RICHTEXT_RECORD_UNITS_INLINE_IMAGE * sizeof(uint16_t);
      if (lineHeight < imageSize.logicalHeightPixels) {
        lineHeight = imageSize.logicalHeightPixels;
      }
    }
  }
commitWrapBoundary:
  g_ActiveFontIndex = savedFontIndexOrImageWidth;
  if (wrapPoint == NULL) {
    wrapPoint = scanCursor;
  }
  lineBottom = drawY + lineHeight;
  /* Draw pass: the line ends at a space or soft hyphen at/after the wrap point, at a line break, or at the
     end of the text. */
  for (;;) {
    scanCursor = drawCursor;
    glyphSubresource = (GraphicsSubresourceIndex)*(short *)scanCursor;
    drawCursor = scanCursor + 2;
    if (glyphSubresource == 0) {
      g_RichTextRuntimeBufferUsedWords = (uint32_t)((int)drawCursor - (int)g_FontRuntimeBuffer) >> 1;
      *lineAdvance = lineHeight;
      return false;
    }
    if (glyphSubresource == ' ') {
      if (wrapPoint <= drawCursor) break;
      glyphAdvance = FontGlyph_DrawVerticallyCentered
                        (clipBottom,clipRight,clipTop,clipLeft,' ',lineHeight,lineBottom,drawX);
      drawX = drawX + glyphAdvance;
      continue;
    }
    if (-1 < (int)glyphSubresource) {
      glyphAdvance = FontGlyph_DrawVerticallyCentered
                        (clipBottom,clipRight,clipTop,clipLeft,glyphSubresource,lineHeight,lineBottom,drawX);
      drawX = drawX + glyphAdvance;
      continue;
    }
    switch(glyphSubresource & RICHTEXT_OPCODE_MASK) {
    case RICHTEXT_OP_COLOR_PALETTE_0:
      g_RichTextCurrentColorArgb = g_RichTextColorPalette0Argb;
      g_RichTextCurrentShadowOffset = g_RichTextShadowOffsetPalette0;
      break;
    case RICHTEXT_OP_COLOR_PALETTE_1:
      g_RichTextCurrentColorArgb = g_RichTextColorPalette1Argb;
      g_RichTextCurrentShadowOffset = g_RichTextShadowOffsetPalette1;
      break;
    case RICHTEXT_OP_COLOR_PALETTE_2:
      g_RichTextCurrentColorArgb = g_RichTextColorPalette2Argb;
      g_RichTextCurrentShadowOffset = g_RichTextShadowOffsetPalette2;
      break;
    case RICHTEXT_OP_COLOR_PALETTE_3:
      g_RichTextCurrentColorArgb = g_RichTextColorPalette3Argb;
      g_RichTextCurrentShadowOffset = g_RichTextShadowOffsetPalette3;
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
      /* eight hex-digit code units (low byte of unit k at scanCursor[2k]), as in
         RichTextCommandStream_DrawSingleLine */
      g_RichTextCurrentColorArgb =
           (((((((scanCursor[4] & 0xf) << (RICHTEXT_COLOR_DIGIT_SHIFT - RICHTEXT_COLOR_DIGIT_BITS) |
                (uint32_t)*drawCursor << RICHTEXT_COLOR_DIGIT_SHIFT) >> RICHTEXT_COLOR_DIGIT_BITS |
               (uint32_t)scanCursor[8] << RICHTEXT_COLOR_DIGIT_SHIFT) >> RICHTEXT_COLOR_DIGIT_BITS |
              (uint32_t)scanCursor[6] << RICHTEXT_COLOR_DIGIT_SHIFT) >> RICHTEXT_COLOR_DIGIT_BITS |
             (uint32_t)scanCursor[12] << RICHTEXT_COLOR_DIGIT_SHIFT) >> RICHTEXT_COLOR_DIGIT_BITS |
            (uint32_t)scanCursor[10] << RICHTEXT_COLOR_DIGIT_SHIFT) >> RICHTEXT_COLOR_DIGIT_BITS |
           (uint32_t)scanCursor[16] << RICHTEXT_COLOR_DIGIT_SHIFT) >> RICHTEXT_COLOR_DIGIT_BITS |
           (uint32_t)scanCursor[14] << RICHTEXT_COLOR_DIGIT_SHIFT;
      drawCursor = scanCursor + RICHTEXT_RECORD_UNITS_LITERAL_COLOR * sizeof(uint16_t);
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
        g_RichTextRuntimeBufferUsedWords = (uint32_t)((int)drawCursor - (int)g_FontRuntimeBuffer) >> 1;
        *lineAdvance = lineHeight;
        return true;
      }
      break;
    case RICHTEXT_OP_LINE_BREAK:
      g_RichTextRuntimeBufferUsedWords = (uint32_t)((int)drawCursor - (int)g_FontRuntimeBuffer) >> 1;
      *lineAdvance = lineHeight;
      return true;
    case RICHTEXT_OP_INLINE_IMAGE:
      /* the image sits on the line's bottom edge */
      imageSize = g_GraphicsTextureSourceGetLogicalSize
                         (*(uint32_t *)(scanCursor + 6),*(GraphicsTextureSourceAsset **)drawCursor);
      savedFontIndexOrImageWidth = imageSize.logicalWidthPixels;
      g_GraphicsTextureSourceBlitSourceAlpha
                (clipBottom,clipRight,clipTop,clipLeft,lineBottom - imageSize.logicalHeightPixels,drawX,
                 *(uint32_t *)(scanCursor + 6),*(GraphicsTextureSourceAsset **)drawCursor,g_FramebufferAccess);
      drawX = drawX + savedFontIndexOrImageWidth;
      drawCursor = scanCursor + RICHTEXT_RECORD_UNITS_INLINE_IMAGE * sizeof(uint16_t);
    }
  }
  g_RichTextRuntimeBufferUsedWords = (uint32_t)((int)drawCursor - (int)g_FontRuntimeBuffer) >> 1;
  *lineAdvance = lineHeight;
  return true;
}


/* Address: 0x0041D840.
   Copies commandStream into g_FontRuntimeBuffer with every nested stream inlined, so the line measuring and
   drawing code can walk one flat stream: glyphs and most commands are copied, literal colours and inline images
   with their payload, nested-stream commands are followed instead of copied and the reserved and inline-value
   commands are dropped. Output beyond RICHTEXT_RUNTIME_BUFFER_UNITS is discarded; the read position
   g_RichTextRuntimeBufferUsedWords is reset to the start.
*/
void RichTextCommandStream_FlattenNestedToRuntimeBuffer(uint16_t *commandStream)

{
  uint16_t commandCodeUnit;
  uint32_t remainingWords;
  int nestedDepth;
  uint16_t *nestedReturnStack[RICHTEXT_NESTING_LIMIT]; /* the original's machine-stack chain */
  uint16_t *commandCursor;
  uint16_t *outputCursor;
  
  remainingWords = RICHTEXT_RUNTIME_BUFFER_UNITS;
  nestedDepth = 0;
  outputCursor = (uint16_t *)g_FontRuntimeBuffer;
  for (;;) {
    commandCursor = commandStream;
    commandCodeUnit = *commandCursor;
    commandStream = commandCursor + 1;
    if (commandCodeUnit == 0) {
      if (nestedDepth == 0) {
        *outputCursor = 0;
        g_RichTextRuntimeBufferUsedWords = 0;
        return;
      }
      nestedDepth--;
      /* resume behind the nested-stream command's payload */
      commandStream = (uint16_t *)((uint8_t *)nestedReturnStack[nestedDepth] + RICHTEXT_NESTED_PAYLOAD_BYTES);
      continue;
    }
    /* Plain code units take the same path as command 0 (copy one word). */
    switch(((short)commandCodeUnit < 0) ? (commandCodeUnit & RICHTEXT_OPCODE_MASK) : 0) {
    default:
      if (remainingWords != 0) {
        *outputCursor = commandCodeUnit;
        remainingWords--;
        outputCursor++;
      }
      break;
    case RICHTEXT_OP_LITERAL_COLOR:
      /* without room for the whole record only the command code unit is skipped */
      if (RICHTEXT_RECORD_UNITS_LITERAL_COLOR < remainingWords) {
        *outputCursor = commandCodeUnit;
        remainingWords = remainingWords - RICHTEXT_RECORD_UNITS_LITERAL_COLOR;
        *(uint32_t *)(outputCursor + 1) = *(uint32_t *)commandStream;
        *(uint32_t *)(outputCursor + 3) = *(uint32_t *)(commandCursor + 3);
        *(uint32_t *)(outputCursor + 5) = *(uint32_t *)(commandCursor + 5);
        *(uint32_t *)(outputCursor + 7) = *(uint32_t *)(commandCursor + 7);
        outputCursor = outputCursor + RICHTEXT_RECORD_UNITS_LITERAL_COLOR;
        commandStream = commandCursor + RICHTEXT_RECORD_UNITS_LITERAL_COLOR;
      }
      break;
    /* dropped; only the command code unit is skipped, so an inline value's payload units follow as code
       units of their own */
    case RICHTEXT_OP_UNUSED_07:
    case RICHTEXT_OP_UNUSED_13:
    case RICHTEXT_OP_INLINE_VALUE_0:
    case RICHTEXT_OP_INLINE_VALUE_1:
    case RICHTEXT_OP_INLINE_VALUE_2:
    case RICHTEXT_OP_UNUSED_17:
    case RICHTEXT_OP_UNUSED_1B:
    case RICHTEXT_OP_UNUSED_1C:
    case RICHTEXT_OP_UNUSED_1D:
    case RICHTEXT_OP_UNUSED_1E:
    case RICHTEXT_OP_UNUSED_1F:
      break;
    case RICHTEXT_OP_CALL_NESTED:
      if (nestedDepth == RICHTEXT_NESTING_LIMIT) {
        break;
      }
      nestedReturnStack[nestedDepth] = commandStream;
      nestedDepth++;
      /* fall through: enter the nested stream */
    case RICHTEXT_OP_JUMP_NESTED:
      commandStream = *(uint16_t **)commandStream;
      break;
    case RICHTEXT_OP_INLINE_IMAGE:
      if (RICHTEXT_RECORD_UNITS_INLINE_IMAGE < remainingWords) {
        *outputCursor = commandCodeUnit;
        remainingWords = remainingWords - RICHTEXT_RECORD_UNITS_INLINE_IMAGE;
        *(uint32_t *)(outputCursor + 1) = *(uint32_t *)commandStream;
        *(uint32_t *)(outputCursor + 3) = *(uint32_t *)(commandCursor + 3);
        outputCursor = outputCursor + RICHTEXT_RECORD_UNITS_INLINE_IMAGE;
        commandStream = commandCursor + RICHTEXT_RECORD_UNITS_INLINE_IMAGE;
      }
    }
  }
}


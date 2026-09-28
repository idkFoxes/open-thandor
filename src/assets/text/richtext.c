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

/* Implementation ownership: assets/text/richtext. */

/* Address: 0x0041D300.
   Measures a text block wrapped to maximumWidth: flattens the stream into the font runtime buffer, sets the
   style's font and colour, and sums the heights of all wrapped lines. Returns maximumWidth itself in EAX (not
   the widest line) and the total height in EDX.
*/
RichTextExtentRegs __thandor_eax_edx_cf_preserve_ecx
RichTextCommandStream_MeasureWrappedBlockRegs
          (uint32_t packedStyle,uint16_t *commandStream,UiPixelExtent maximumWidth)

{
  uint32_t colorPaletteIndex;
  int totalHeight;
  RichTextExtentRegs blockExtent;
  WrappedLineResult lineResult;
  
  RichTextCommandStream_FlattenNestedToRuntimeBuffer(commandStream);
  colorPaletteIndex = packedStyle >> TEXT_STYLE_PALETTE_SHIFT & TEXT_STYLE_INDEX_MASK;
  g_ActiveFontIndex = packedStyle >> TEXT_STYLE_FONT_SHIFT & TEXT_STYLE_INDEX_MASK;
  g_RichTextCurrentColorArgb = (&g_RichTextColorPalette0Argb)[colorPaletteIndex];
  g_RichTextCurrentShadowOffset = (&g_RichTextShadowOffsetPalette0)[colorPaletteIndex];
  totalHeight = 0;
  g_RichTextSavedColorArgb = g_RichTextCurrentColorArgb;
  g_RichTextSavedShadowOffset = g_RichTextCurrentShadowOffset;
  while( true ) {
    lineResult = RichTextCommandStream_MeasureNextWrappedLine(maximumWidth);
    if (lineResult.endOfText) break;
    totalHeight = totalHeight + lineResult.lineAdvancePixels;
  }
  blockExtent.heightPixels = totalHeight + lineResult.lineAdvancePixels;
  blockExtent.widthPixels = maximumWidth;
  return blockExtent;
}


/* Address: 0x0041D7C0.
   Draws a rich-text block wrapped to maximumWidth with its top-left corner at (drawX, drawY), clipped to the
   given rectangle: flattens the stream into the font runtime buffer, sets the style's font and colour, and
   draws line after line (RichTextCommandStream_DrawNextWrappedLine) until the end of the text. Called directly
   by UiWrappedTextControl_DrawClipped.
*/
void __thandor_void_preserve_eax_ecx_edx
RichTextCommandStream_DrawWrappedBlock
          (UiPixelCoordinate clipTop,UiPixelCoordinate clipLeft,UiPixelCoordinate clipBottom,
          UiPixelCoordinate clipRight,uint32_t packedStyle,uint16_t *commandStream,
          UiPixelExtent maximumWidth,UiPixelCoordinate drawY,UiPixelCoordinate drawX)

{
  uint32_t colorPaletteIndex;
  WrappedLineResult lineResult;
  
  RichTextCommandStream_FlattenNestedToRuntimeBuffer(commandStream);
  colorPaletteIndex = packedStyle >> TEXT_STYLE_PALETTE_SHIFT & TEXT_STYLE_INDEX_MASK;
  g_ActiveFontIndex = packedStyle >> TEXT_STYLE_FONT_SHIFT & TEXT_STYLE_INDEX_MASK;
  g_RichTextCurrentColorArgb = (&g_RichTextColorPalette0Argb)[colorPaletteIndex];
  g_RichTextCurrentShadowOffset = (&g_RichTextShadowOffsetPalette0)[colorPaletteIndex];
  g_RichTextSavedColorArgb = g_RichTextCurrentColorArgb;
  g_RichTextSavedShadowOffset = g_RichTextCurrentShadowOffset;
  while( true ) {
    lineResult = RichTextCommandStream_DrawNextWrappedLine
                      (clipTop,clipLeft,clipBottom,clipRight,maximumWidth,drawY,drawX);
    if (lineResult.endOfText) break;
    drawY = drawY + lineResult.lineAdvancePixels;
  }
  return;
}


/* Address: 0x0041D4A0.
   Draws one rich-text line: measures it first to align it (right or centred on penX, per the packed style) and
   to place the baseline below lineTopY, then interprets glyphs, colour, font, nested-stream and inline-image
   commands until the end of the stream or a line break.
*/
bool __thandor_cf_preserve_eax_ecx_edx
RichTextCommandStream_DrawSingleLine
          (UiPixelCoordinate clipTop,UiPixelCoordinate clipLeft,UiPixelCoordinate clipBottom,
          UiPixelCoordinate clipRight,UiPackedTextStyle packedStyle,uint16_t *commandStream,
          UiPixelCoordinate lineTopY,UiPixelCoordinate penX)

{
  int lineBaselineY;
  uint32_t alignShiftOrPaletteIndex;
  GraphicsSubresourceIndex glyphSubresource;
  int glyphAdvance;
  uint32_t imageWidth;
  uint16_t *commandCursor;
  RichTextExtentRegs lineExtent;
  TextureSizeResult imageSize;
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
                        (clipTop,clipLeft,clipBottom,clipRight,glyphSubresource,lineBaselineY,penX);
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
           ((((((((uint8_t)commandCursor[2] & 0xf) << 0x18 | (uint32_t)(uint8_t)*commandStream << 0x1c) >> 4 |
               (uint32_t)(uint8_t)commandCursor[4] << 0x1c) >> 4 | (uint32_t)(uint8_t)commandCursor[3] << 0x1c) >> 4 |
             (uint32_t)(uint8_t)commandCursor[6] << 0x1c) >> 4 | (uint32_t)(uint8_t)commandCursor[5] << 0x1c) >> 4 |
           (uint32_t)(uint8_t)commandCursor[8] << 0x1c) >> 4 | (uint32_t)(uint8_t)commandCursor[7] << 0x1c;
      commandStream = commandCursor + RICHTEXT_RECORD_UNITS_LITERAL_COLOR;
      break;
    case RICHTEXT_OP_SELECT_FONT_FIRST:
    case 9:
    case 10:
    case 0xb:
    case 0xc:
    case 0xd:
    case 0xe:
    case 0xf:
      g_ActiveFontIndex = glyphSubresource & 0xf;
      break;
    case RICHTEXT_OP_FIXED_SPACE:
      glyphAdvance = FontGlyph_DrawBottomAligned
                        (clipTop,clipLeft,clipBottom,clipRight,' ',lineBaselineY,penX);
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
                (clipTop,clipLeft,clipBottom,clipRight,lineBaselineY - imageSize.logicalHeightPixels,
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
void __thandor_void_preserve_eax_ecx_edx
RichTextCommandStream_PatchPayloadBySelector
          (RichTextCommandSelector selector,void *replacementPayload,uint16_t *stream)

{
  uint16_t commandCodeUnit;
  uint16_t *commandCursor;

  while( true ) {
    commandCursor = stream;
    commandCodeUnit = *commandCursor;
    if (commandCodeUnit == 0) break;
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
void __thandor_void_preserve_eax_ecx
RichTextCommandStream_BindTextureSource(GraphicsTextureSourceAsset *textureSource,uint16_t *stream)

{
  uint16_t *commandCursor;
  uint16_t *streamCursor;
  uint16_t commandCodeUnit;

  streamCursor = stream;
  while( true ) {
    commandCursor = streamCursor;
    commandCodeUnit = *commandCursor;
    streamCursor = commandCursor + 1;
    if (commandCodeUnit == 0) break;
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
bool __thandor_cf_preserve_eax_ecx_edx
RichTextCommandStream_FindNthCommandPayloadPair
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
  } while( true );
}


/* Address: 0x0041B420.
   Walks one command stream (without following nested streams) and sets the stream pointer of every
   nested-stream command (0x18/0x19) to nestedStreamPointerValue, regardless of its selector (compare
   RichTextCommandStream_PatchPayloadBySelector). No caller and no function-pointer table entry for it was
   found in src/ or src/generated/image_data.c.
*/
void __thandor_void_preserve_eax_ecx_edx
RichTextCommandStream_PatchNestedStreamPointerPayloads
          (RichTextNestedStreamPointerValue32 nestedStreamPointerValue,uint16_t *commandStream)

{
  uint16_t *commandCursor;
  RichTextNestedStreamPointerValue32 *streamCursor;
  uint16_t commandCodeUnit;
  
  streamCursor = (RichTextNestedStreamPointerValue32 *)commandStream;
  while( true ) {
    commandCursor = (uint16_t *)streamCursor;
    commandCodeUnit = *commandCursor;
    streamCursor = (RichTextNestedStreamPointerValue32 *)(commandCursor + 1);
    if (commandCodeUnit == 0) break;
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
  }
  return;
}


/* Address: 0x0041B520.
   Walks one command stream (without following nested streams) and sets both payload dwords of every
   inline-image command (0x1A): the texture source to textureSourceValue and the subresource to
   imageSubresourceValue (see RichTextCommandStream_BindTextureSource for the texture source alone). No caller
   and no function-pointer table entry for it was found in src/ or src/generated/image_data.c.
*/
void __thandor_void_preserve_eax_ecx_edx
RichTextCommandStream_PatchOpcode1APayloadPair
          (RichTextOpcode1APayloadValue32 imageSubresourceValue,
          RichTextCommandPayload32 textureSourceValue,uint16_t *commandStream)

{
  uint16_t *commandCursor;
  RichTextCommandPayload32 *streamCursor;
  uint16_t commandCodeUnit;
  
  streamCursor = (RichTextCommandPayload32 *)commandStream;
  while( true ) {
    commandCursor = (uint16_t *)streamCursor;
    commandCodeUnit = *commandCursor;
    streamCursor = (RichTextCommandPayload32 *)(commandCursor + 1);
    if (commandCodeUnit == 0) break;
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
  }
  return;
}


/* Address: 0x0041B620.
   Walks one command stream (without following nested streams) and sets the 32-bit payload of every
   inline-value command (0x14..0x16) to inlinePayloadValue. No caller and no function-pointer table entry
   for it was found in src/ or src/generated/image_data.c.
*/
void __thandor_void_preserve_eax_ecx_edx
RichTextCommandStream_PatchInlinePayloads
          (RichTextInlinePayloadValue32 inlinePayloadValue,uint16_t *commandStream)

{
  uint16_t *commandCursor;
  RichTextInlinePayloadValue32 *streamCursor;
  uint16_t commandCodeUnit;
  
  streamCursor = (RichTextInlinePayloadValue32 *)commandStream;
  while( true ) {
    commandCursor = (uint16_t *)streamCursor;
    commandCodeUnit = *commandCursor;
    streamCursor = (RichTextInlinePayloadValue32 *)(commandCursor + 1);
    if (commandCodeUnit == 0) break;
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
bool __thandor_cf_preserve_eax_ecx_edx
RichTextCommandStream_FindNthCommandFlagsPair(int commandOrdinal,uint32_t flagBits,uint32_t *commandStream)

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
      streamCursor = (uint32_t *)((int)commandCursor + 2);
    } while (-1 < (short)commandCodeUnit); /* skip glyphs up to the next RICHTEXT_COMMAND_FLAG unit */
    /* the cursors advance in bytes: record lengths are code units * 2 */
    switch(commandCodeUnit & RICHTEXT_OPCODE_MASK) {
    case RICHTEXT_OP_LITERAL_COLOR:
      streamCursor = (uint32_t *)((int)commandCursor + 0x12);
      break;
    case RICHTEXT_OP_INLINE_VALUE_0:
    case RICHTEXT_OP_INLINE_VALUE_1:
    case RICHTEXT_OP_INLINE_VALUE_2:
      remainingCount--;
      streamCursor = (uint32_t *)((int)commandCursor + 6);
      if (remainingCount == 0) {
        *commandCursor = *commandCursor & 0xffff8014;
        *commandCursor = *commandCursor | flagBits;
        return false;
      }
      break;
    case RICHTEXT_OP_CALL_NESTED:
    case RICHTEXT_OP_JUMP_NESTED:
    case RICHTEXT_OP_INLINE_IMAGE:
      streamCursor = (uint32_t *)((int)commandCursor + 10);
    }
  } while( true );
}


/* Address: 0x0041B840.
   Returns the variant (code & 3, i.e. 0..2 for opcodes 0x14..0x16) of the commandOrdinal-th (0-based)
   inline-value command of one command stream with CF clear, or 0 with CF set when the stream has fewer
   such commands. No caller and no function-pointer table entry for it was found in src/ or
   src/generated/image_data.c.
*/
RichTextCommandQueryResult __thandor_eax_cf_preserve_ecx_edx
RichTextCommandStream_QueryNthCommandFlags(int commandOrdinal,uint16_t *commandStream)

{
  uint16_t commandCodeUnit;
  int remainingCount;
  uint32_t *commandCursor;
  RichTextCommandQueryResult foundResult;
  RichTextCommandQueryResult endResult;
  
  remainingCount = commandOrdinal + 1;
  do {
    do {
      commandCursor = (uint32_t *)commandStream;
      commandCodeUnit = (uint16_t)*commandCursor;
      if (commandCodeUnit == 0) {
        endResult.commandVariant = 0;
        endResult.endOfStream = true;
        return endResult;
      }
      commandStream = (uint16_t *)((int)commandCursor + 2);
    } while (-1 < (short)commandCodeUnit); /* skip glyphs up to the next RICHTEXT_COMMAND_FLAG unit */
    /* the cursors advance in bytes: record lengths are code units * 2 */
    switch(commandCodeUnit & RICHTEXT_OPCODE_MASK) {
    case RICHTEXT_OP_LITERAL_COLOR:
      commandStream = (uint16_t *)((int)commandCursor + 0x12);
      break;
    case RICHTEXT_OP_INLINE_VALUE_0:
    case RICHTEXT_OP_INLINE_VALUE_1:
    case RICHTEXT_OP_INLINE_VALUE_2:
      remainingCount--;
      commandStream = (uint16_t *)((int)commandCursor + 6);
      if (remainingCount == 0) {
        foundResult.commandVariant = *commandCursor & 3;
        foundResult.endOfStream = false;
        return foundResult;
      }
      break;
    case RICHTEXT_OP_CALL_NESTED:
    case RICHTEXT_OP_JUMP_NESTED:
    case RICHTEXT_OP_INLINE_IMAGE:
      commandStream = (uint16_t *)((int)commandCursor + 10);
    }
  } while( true );
}


/* Address: 0x0041B950.
   Converts a rich-text command stream into a NUL-terminated 8-bit string (for Win32 text such as message boxes):
   follows nested streams, turns the fixed-space and line-break commands into ' ' and CR LF, drops all other
   commands and every code unit above 0xFF. Returns the byte count including the terminator; on overflow the
   output is cut and terminated and CF is set with FATAL_ERROR_GENERAL_FAILURE.
*/
StatusResult __thandor_eax_cf_preserve_ecx_edx
RichTextCommandStream_CopyToNarrow
          (TextOutputCapacityBytes capacityBytes,uint8_t *destination,uint16_t *source)

{
  uint16_t *readCursor;
  uint32_t remainingCapacityBytes;
  uint16_t *commandCursor;
  uint16_t *streamCursor;
  bool newlineCapacityUnderflow;
  StatusResult successResult;
  StatusResult errorResult;
  uint16_t *nestedReturnStack[RICHTEXT_NESTING_LIMIT]; /* the original's machine-stack chain */
  int nestedDepth;
  uint16_t commandOrCodeUnit;
  
  nestedDepth = 0;
  remainingCapacityBytes = capacityBytes;
  readCursor = source;
  while( true ) {
    while( true ) {
      commandCursor = readCursor;
      commandOrCodeUnit = *commandCursor;
      streamCursor = commandCursor + 1;
      if (commandOrCodeUnit == 0) break;
      if ((short)commandOrCodeUnit < 0) {
        readCursor = streamCursor;
        switch(commandOrCodeUnit & RICHTEXT_OPCODE_MASK) {
        case RICHTEXT_OP_LITERAL_COLOR:
          readCursor = commandCursor + RICHTEXT_RECORD_UNITS_LITERAL_COLOR;
          break;
        case RICHTEXT_OP_FIXED_SPACE:
          remainingCapacityBytes = remainingCapacityBytes - 1;
          if (remainingCapacityBytes == 0)
          goto RichTextCommandStream_CopyToNarrow_TerminateOutputAndReturnCapacityError;
          *destination = ' ';
          destination++;
          readCursor = streamCursor;
          break;
        case RICHTEXT_OP_LINE_BREAK:
          newlineCapacityUnderflow = remainingCapacityBytes < 2;
          remainingCapacityBytes = remainingCapacityBytes - 2;
          if (newlineCapacityUnderflow || remainingCapacityBytes == 0)
          goto RichTextCommandStream_CopyToNarrow_TerminateOutputAndReturnCapacityError;
          destination[0] = '\r';
          destination[1] = '\n';
          destination = destination + 2;
          readCursor = streamCursor;
          break;
        case RICHTEXT_OP_INLINE_VALUE_0:
        case RICHTEXT_OP_INLINE_VALUE_1:
        case RICHTEXT_OP_INLINE_VALUE_2:
          readCursor = commandCursor + RICHTEXT_RECORD_UNITS_INLINE_VALUE;
          break;
        case RICHTEXT_OP_CALL_NESTED:
          if (nestedDepth == RICHTEXT_NESTING_LIMIT)
          goto RichTextCommandStream_CopyToNarrow_TerminateOutputAndReturnCapacityError;
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
          remainingCapacityBytes = remainingCapacityBytes - 1;
          if (remainingCapacityBytes == 0)
          goto RichTextCommandStream_CopyToNarrow_TerminateOutputAndReturnCapacityError;
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
    successResult.valueOrError = capacityBytes - (remainingCapacityBytes - 1);
    successResult.failed = false;
    return successResult;
  }
RichTextCommandStream_CopyToNarrow_TerminateOutputAndReturnCapacityError:
  destination[-1] = 0;
  errorResult.failed = true;
  errorResult.valueOrError = FATAL_ERROR_GENERAL_FAILURE;
  return errorResult;
}


/* Address: 0x0041BCB0.
   Leftover of the TXT2STR converter (no caller and no function-pointer table entry in src/ or
   src/generated/image_data.c): compiles text markup into a 'str' string asset. Text between '#<' and '#>'
   becomes one NUL-terminated, dword-padded rich-text string keyed by the last '#ddd' number (text outside is
   ignored); further '#' escapes select a code page ('#@'..'#~'), raw command codes ('#!'), a soft hyphen
   ('#-'), a literal '#' ('##') or a line continuation, a CR inside a tag is a line break, and '#.' ends the
   input and groups the strings by key.
   An unknown character returns the formatted "TXT2STR: unknown character" message (with its byte offset)
   and CF set; running out of arena space returns FATAL_ERROR_GENERAL_FAILURE.
*/
RichTextAssetResult __thandor_eax_cf_preserve_edx
RichTextMarkup_ParseAndBuildStringAsset(uint8_t *markupBytes)

{
  /* Unreachable: nothing in the original image calls 0x0041BCB0 or stores its address (a leftover
     of the TXT2STR converter). The original pushes a (string start, key) pair per tag on the machine
     stack at '#<'; those slots were never recovered, so the body is kept only for completeness and does not
     reproduce the original (the '#ddd' key in EDX is lost entirely). */
  uint8_t thandor_stack_frame[0x100]; /* unrecovered Ghidra stack slots (stack0x...), entry ESP at index 0x80 */
  uint8_t markupByte;
  uint16_t codeUnit;
  wchar_t *memory;
  uint32_t spanSizeOrDwordCount;
  uint32_t remainingCapacityBytes;
  int groupKeyOrIndex;
  int *offsetTableCursor;
  int entryIndexOrOffset;
  int entryEndOrIndex;
  short codeUnitBias;
  uint8_t *stackSlot;
  uint8_t *callStackSlot;
  uint8_t *tokenStart;
  uint8_t *markupCursor;
  int *copySource;
  wchar_t *outputCursor;
  int *groupHeader;
  uint32_t assetSizeOrTimestamp;
  int *assetWriteCursor;
  bool capacityUnderflow;
  bool insideTagOrUnderflow;
  RichTextAssetResult errorMessageResult;
  RichTextAssetResult capacityErrorResult;
  ArenaShrinkResult shrinkResult;
  RichTextAssetResult stringAsset;
  ArenaLargestAllocResult largestBlock;
  WideNumberFormatFlags aWStackY_44 [2];
  uint32_t dStackY_3c;
  int assetGroupCount;
  int tagCount;
  
  largestBlock = g_MemoryApi.allocLargestFreeBlock();
  remainingCapacityBytes = largestBlock.blockSizeOrSentinel;
  memory = (wchar_t *)largestBlock.allocationOrError;
  if (!largestBlock.failed) {
    codeUnitBias = 0;
    tagCount = 0;
    markupCursor = markupBytes;
    outputCursor = memory;
    insideTagOrUnderflow = false;
RichTextMarkup_ParseAndBuildStringAsset_ParseNextByte:
    tokenStart = markupCursor;
    codeUnit = (uint16_t)*tokenStart;
    markupCursor = tokenStart + 1;
    switch(*tokenStart) {
    case 0:
    case 1:
    case 2:
    case 3:
    case 4:
    case 5:
    case 6:
    case 7:
    case 8:
    case 0xb:
    case 0xc:
    case 0xe:
    case 0xf:
    case 0x10:
    case 0x11:
    case 0x12:
    case 0x13:
    case 0x14:
    case 0x15:
    case 0x16:
    case 0x17:
    case 0x18:
    case 0x19:
    case 0x1a:
    case 0x1b:
    case 0x1c:
    case 0x1d:
    case 0x1e:
    case 0x1f:
    case 0x7f:
      goto RichTextMarkup_ParseAndBuildStringAsset_ReportUnknownCharacter;
    case '\t':
    case '\n':
      goto RichTextMarkup_ParseAndBuildStringAsset_ParseNextByte;
    case '\r':
      if (insideTagOrUnderflow) {
        capacityUnderflow = remainingCapacityBytes < 2;
        remainingCapacityBytes = remainingCapacityBytes - 2;
        if (capacityUnderflow || remainingCapacityBytes == 0)
        goto RichTextMarkup_ParseAndBuildStringAsset_FreePrimaryBufferAndReturnCapacityError;
        *outputCursor = RICHTEXT_COMMAND_FLAG | RICHTEXT_OP_LINE_BREAK;
        outputCursor++;
      }
      goto RichTextMarkup_ParseAndBuildStringAsset_ParseNextByte;
    default:
RichTextMarkup_ParseAndBuildStringAsset_EmitLiteralCodeUnit:
      if (insideTagOrUnderflow) {
        capacityUnderflow = remainingCapacityBytes < 2;
        remainingCapacityBytes = remainingCapacityBytes - 2;
        if (capacityUnderflow || remainingCapacityBytes == 0)
        goto RichTextMarkup_ParseAndBuildStringAsset_FreePrimaryBufferAndReturnCapacityError;
        *outputCursor = codeUnit + codeUnitBias;
        outputCursor++;
      }
      goto RichTextMarkup_ParseAndBuildStringAsset_ParseNextByte;
    case '#':
      markupByte = *markupCursor;
      codeUnit = (uint16_t)markupByte;
      markupCursor = tokenStart + 2;
      switch(markupByte) {
      default:
        goto RichTextMarkup_ParseAndBuildStringAsset_ReportUnknownCharacter;
      case '\n':
      case '\r':
        /* line continuation: skip the line end */
        while (markupByte = *markupCursor, markupByte < 0x20) {
          markupCursor++;
          if ((markupByte != '\n') && (markupByte != '\r')) goto RichTextMarkup_ParseAndBuildStringAsset_ReportUnknownCharacter;
        }
        break;
      case '!':
        codeUnitBias = 0x7fc0; /* '@'..'_' become RICHTEXT_COMMAND_FLAG | 0x00..0x1F */
        break;
      case '#':
        goto RichTextMarkup_ParseAndBuildStringAsset_EmitLiteralCodeUnit;
      case '-':
        capacityUnderflow = remainingCapacityBytes < 2;
        remainingCapacityBytes = remainingCapacityBytes - 2;
        if (capacityUnderflow || remainingCapacityBytes == 0)
        goto RichTextMarkup_ParseAndBuildStringAsset_FreePrimaryBufferAndReturnCapacityError;
        *outputCursor = RICHTEXT_COMMAND_FLAG | RICHTEXT_OP_SOFT_HYPHEN;
        outputCursor++;
        break;
      case '.':
        /* end of input: build the asset (0x200-byte header, then one group per key) */
        if ((tagCount != 0) && (!insideTagOrUnderflow)) {
          tagCount++;
          dStackY_3c = 0x41c70a;
          shrinkResult = g_MemoryApi.shrinkInPlace((int)outputCursor - (int)memory,memory);
          if (!shrinkResult.failed) {
            largestBlock = g_MemoryApi.allocLargestFreeBlock();
            stringAsset.assetOrError = (int *)largestBlock.allocationOrError;
            if (!largestBlock.failed) {
              remainingCapacityBytes = largestBlock.blockSizeOrSentinel - 0x200;
              if (0x1ff < largestBlock.blockSizeOrSentinel && remainingCapacityBytes != 0) {
                assetGroupCount = 0;
                assetWriteCursor = stringAsset.assetOrError;
                for (groupKeyOrIndex = 0x80; entryIndexOrOffset = tagCount, groupKeyOrIndex != 0; groupKeyOrIndex--) {
                  *assetWriteCursor = 0;
                  assetWriteCursor++;
                }
                while( true ) {
                  while (groupHeader = assetWriteCursor, groupKeyOrIndex = *(int *)(&thandor_stack_frame[0x80 - 0x30] + entryIndexOrOffset * 8),
                        groupKeyOrIndex == -1) {
                    entryIndexOrOffset--;
                    assetWriteCursor = groupHeader;
                    if (entryIndexOrOffset == 0) {
                      g_MemoryApi.free(memory);
                      assetSizeOrTimestamp = (int)groupHeader - (int)stringAsset.assetOrError;
                      g_MemoryApi.shrinkInPlace(assetSizeOrTimestamp,stringAsset.assetOrError);
                      tagCount = tagCount * 8;
                      *(uint32_t *)(&thandor_stack_frame[0x80 - 0x2c] + tagCount) = assetSizeOrTimestamp;
                      ((int *)stringAsset.assetOrError)[0x2c] = assetGroupCount;
                      ((int *)stringAsset.assetOrError)[1] = assetSizeOrTimestamp;
                      *(int *)stringAsset.assetOrError = 0x727473; /* "str" */
                      ((int *)stringAsset.assetOrError)[2] = 1;
                      ((int *)stringAsset.assetOrError)[3] = 0;
                      stackSlot = &thandor_stack_frame[0x80 - 0x30] + tagCount;
                      *(uint32_t *)(&thandor_stack_frame[0x80 - 0x30] + tagCount) = 0x41c7b5;
                      assetSizeOrTimestamp = g_LocaleGetPackedCurrentTime();
                      ((int *)stringAsset.assetOrError)[4] = assetSizeOrTimestamp;
                      ((int *)stringAsset.assetOrError)[6] = assetSizeOrTimestamp;
                      ((int *)stringAsset.assetOrError)[8] = assetSizeOrTimestamp;
                      callStackSlot = stackSlot + -4;
                      *(uint32_t *)(stackSlot + -4) = 0x41c7cd;
                      assetSizeOrTimestamp = g_LocaleGetPackedCurrentDate();
                      ((int *)stringAsset.assetOrError)[5] = assetSizeOrTimestamp;
                      ((int *)stringAsset.assetOrError)[7] = assetSizeOrTimestamp;
                      ((int *)stringAsset.assetOrError)[9] = assetSizeOrTimestamp;
                      *(int **)(callStackSlot + -4) = (int *)stringAsset.assetOrError + 0xc;
                      *(uint32_t *)(callStackSlot + -8) = 0x41c7ec;
                      g_LocaleCopyDefaultComputerLabelUtf16(*(uint16_t **)(callStackSlot + -4));
                      *(int **)(callStackSlot + -4) = (int *)stringAsset.assetOrError + 0x1c;
                      *(uint32_t *)(callStackSlot + -8) = 0x41c7f9;
                      g_LocaleCopyDefaultComputerLabelUtf16(*(uint16_t **)(callStackSlot + -4));
                      stringAsset.failed = false;
                      return stringAsset;
                    }
                  }
                  insideTagOrUnderflow = remainingCapacityBytes < 0x10;
                  remainingCapacityBytes = remainingCapacityBytes - 0x10;
                  if (insideTagOrUnderflow || remainingCapacityBytes == 0) break;
                  groupHeader[2] = groupKeyOrIndex;
                  *groupHeader = 0x10;
                  groupHeader[1] = 0;
                  do {
                    if (groupKeyOrIndex == *(int *)(&thandor_stack_frame[0x80 - 0x30] + entryIndexOrOffset * 8)) {
                      entryEndOrIndex = *(int *)(&thandor_stack_frame[0x80 - 0x34] + entryIndexOrOffset * 8);
                      groupHeader[1]++;
                      spanSizeOrDwordCount = (entryEndOrIndex - *(int *)(&thandor_stack_frame[0x80 - 0x2c] + entryIndexOrOffset * 8)) + 4;
                      *groupHeader = *groupHeader + spanSizeOrDwordCount;
                      insideTagOrUnderflow = remainingCapacityBytes < spanSizeOrDwordCount;
                      remainingCapacityBytes = remainingCapacityBytes - spanSizeOrDwordCount;
                      if (insideTagOrUnderflow || remainingCapacityBytes == 0)
                      goto 
                      RichTextMarkup_ParseAndBuildStringAsset_FreeTemporaryExpansionBufferBeforeCapacityError
                      ;
                    }
                    entryIndexOrOffset--;
                  } while (entryIndexOrOffset != 0);
                  offsetTableCursor = groupHeader + 4;
                  assetGroupCount++;
                  assetWriteCursor = offsetTableCursor + groupHeader[1];
                  entryEndOrIndex = tagCount;
                  do {
                    if (groupKeyOrIndex == *(int *)(&thandor_stack_frame[0x80 - 0x30] + entryEndOrIndex * 8)) {
                      *(uint32_t *)(&thandor_stack_frame[0x80 - 0x30] + entryEndOrIndex * 8) = 0xffffffff;
                      entryIndexOrOffset = (int)assetWriteCursor - (int)groupHeader;
                      copySource = *(int **)(&thandor_stack_frame[0x80 - 0x2c] + entryEndOrIndex * 8);
                      for (spanSizeOrDwordCount = (uint32_t)(*(int *)(&thandor_stack_frame[0x80 - 0x34] + entryEndOrIndex * 8) -
                                         (int)*(int **)(&thandor_stack_frame[0x80 - 0x2c] + entryEndOrIndex * 8)) >> 2;
                          spanSizeOrDwordCount != 0; spanSizeOrDwordCount--) {
                        *assetWriteCursor = *copySource;
                        copySource++;
                        assetWriteCursor++;
                      }
                      *offsetTableCursor = entryIndexOrOffset;
                      offsetTableCursor++;
                    }
                    entryEndOrIndex--;
                    entryIndexOrOffset = tagCount;
                  } while (entryEndOrIndex != 0);
                }
              }
RichTextMarkup_ParseAndBuildStringAsset_FreeTemporaryExpansionBufferBeforeCapacityError:
              g_MemoryApi.free(stringAsset.assetOrError);
            }
          }
RichTextMarkup_ParseAndBuildStringAsset_FreePrimaryBufferAndReturnCapacityError:
          tagCount = tagCount * 8;
          *(wchar_t **)(&thandor_stack_frame[0x80 - 0x2c] + tagCount) = memory;
          *(uint32_t *)(&thandor_stack_frame[0x80 - 0x30] + tagCount) = 0x41c5e3;
          g_MemoryApi.free(*(void **)(&thandor_stack_frame[0x80 - 0x2c] + tagCount));
          capacityErrorResult.failed = true;
          capacityErrorResult.assetOrError = (void *)FATAL_ERROR_GENERAL_FAILURE;
          return capacityErrorResult;
        }
        goto RichTextMarkup_ParseAndBuildStringAsset_ReportUnknownCharacter;
      case '0':
      case '1':
      case '2':
      case '3':
      case '4':
      case '5':
      case '6':
      case '7':
      case '8':
      case '9':
        /* '#ddd': three decimal digits, the key of the following tags (kept in EDX by the original) */
        if ((((*markupCursor < '0') || ('9' < *markupCursor)) || (tokenStart[3] < '0')) || ('9' < tokenStart[3])
           ) goto RichTextMarkup_ParseAndBuildStringAsset_ReportUnknownCharacter;
        markupCursor = tokenStart + 4;
        break;
      case '<':
        if (insideTagOrUnderflow) goto RichTextMarkup_ParseAndBuildStringAsset_ReportUnknownCharacter;
        tagCount++;
        insideTagOrUnderflow = true;
        break;
      case '>':
        /* ends the string: NUL terminator, padded to a dword boundary */
        if (!insideTagOrUnderflow) goto RichTextMarkup_ParseAndBuildStringAsset_ReportUnknownCharacter;
        insideTagOrUnderflow = false;
        if (((uint32_t)outputCursor & 2) == 0) {
          insideTagOrUnderflow = remainingCapacityBytes < 4;
          remainingCapacityBytes = remainingCapacityBytes - 4;
          if (insideTagOrUnderflow || remainingCapacityBytes == 0)
          goto RichTextMarkup_ParseAndBuildStringAsset_FreePrimaryBufferAndReturnCapacityError;
          outputCursor[0] = L'\0';
          outputCursor[1] = L'\0';
          outputCursor += 2;
          insideTagOrUnderflow = false;
        }
        else {
          capacityUnderflow = remainingCapacityBytes < 2;
          remainingCapacityBytes = remainingCapacityBytes - 2;
          if (capacityUnderflow || remainingCapacityBytes == 0)
          goto RichTextMarkup_ParseAndBuildStringAsset_FreePrimaryBufferAndReturnCapacityError;
          *outputCursor = L'\0';
          outputCursor++;
        }
        break;
      case 0x40:
      case 0x41:
      case 0x42:
      case 0x43:
      case 0x44:
      case 0x45:
      case 0x46:
      case 0x47:
      case 0x48:
      case 0x49:
      case 0x4a:
      case 0x4b:
      case 0x4c:
      case 0x4d:
      case 0x4e:
      case 0x4f:
      case 0x50:
      case 0x51:
      case 0x52:
      case 0x53:
      case 0x54:
      case 0x55:
      case 0x56:
      case 0x57:
      case 0x58:
      case 0x59:
      case 0x5a:
      case 0x5b:
      case 0x5c:
      case 0x5d:
      case 0x5e:
      case 0x5f:
      case 0x60:
      case 0x61:
      case 0x62:
      case 99:
      case 100:
      case 0x65:
      case 0x66:
      case 0x67:
      case 0x68:
      case 0x69:
      case 0x6a:
      case 0x6b:
      case 0x6c:
      case 0x6d:
      case 0x6e:
      case 0x6f:
      case 0x70:
      case 0x71:
      case 0x72:
      case 0x73:
      case 0x74:
      case 0x75:
      case 0x76:
      case 0x77:
      case 0x78:
      case 0x79:
      case 0x7a:
      case 0x7b:
      case 0x7c:
      case 0x7d:
      case 0x7e:
        /* '#@'..'#~': code page select, following bytes are emitted + (c - '@') * 0x80 */
        codeUnitBias = codeUnit * 0x80 - 0x2000;
      }
      goto RichTextMarkup_ParseAndBuildStringAsset_ParseNextByte;
    }
  }
RichTextMarkup_ParseAndBuildStringAsset_ReturnError:
  errorMessageResult.failed = true;
  errorMessageResult.assetOrError = memory;
  return errorMessageResult;
RichTextMarkup_ParseAndBuildStringAsset_ReportUnknownCharacter:
  groupKeyOrIndex = tagCount * 8;
  *(wchar_t **)(&thandor_stack_frame[0x80 - 0x2c] + groupKeyOrIndex) = memory;
  *(uint32_t *)(&thandor_stack_frame[0x80 - 0x30] + groupKeyOrIndex) = 0x41c5a2;
  g_MemoryApi.free(*(void **)(&thandor_stack_frame[0x80 - 0x2c] + groupKeyOrIndex));
  *(wchar_t **)(&thandor_stack_frame[0x80 - 0x2c] + groupKeyOrIndex) = u_error__TXT2STR__unknown_characte_0041afac + 0x26;
  *(int *)(&thandor_stack_frame[0x80 - 0x30] + groupKeyOrIndex) = (int)markupCursor - (int)markupBytes;
  *(uint32_t *)(&thandor_stack_frame[0x80 - 0x34] + groupKeyOrIndex) = 1;
  *(uint32_t *)(&thandor_stack_frame[0x80 - 0x38] + groupKeyOrIndex) = 10;
  (&dStackY_3c)[tagCount * 2] = 0;
  aWStackY_44[tagCount * 2 + 1] = 0x40;
  aWStackY_44[tagCount * 2] = 0x41c5bb;
  g_WideNumberFormatUtf16
            (aWStackY_44[tagCount * 2 + 1],(&dStackY_3c)[tagCount * 2],
             *(uint32_t *)(&thandor_stack_frame[0x80 - 0x38] + groupKeyOrIndex),*(uint32_t *)(&thandor_stack_frame[0x80 - 0x34] + groupKeyOrIndex),
             *(int32_t *)(&thandor_stack_frame[0x80 - 0x30] + groupKeyOrIndex),*(uint16_t **)(&thandor_stack_frame[0x80 - 0x2c] + groupKeyOrIndex));
  memory = u_error__TXT2STR__unknown_characte_0041afac;
  goto RichTextMarkup_ParseAndBuildStringAsset_ReturnError;
}


/* Address: 0x0041C8D0.
   Copies a rich-text command stream into a bounded buffer with every nested stream (0x18/0x19) inlined, so the
   copy no longer depends on the streams it referenced. Commands are normalised to RICHTEXT_COMMAND_FLAG | opcode;
   payload records are copied unchanged. Returns the byte count without the terminator; on overflow the output is
   cut and terminated and CF is set with FATAL_ERROR_GENERAL_FAILURE.
*/
RichTextCopyResult __thandor_eax_cf_preserve_ecx_edx
RichTextCommandStream_CopyExpanded
          (TextOutputCapacityBytes capacityBytes,uint16_t *destination,uint16_t *source)

{
  uint16_t commandCodeUnit;
  int wordsRemaining;
  uint16_t *nextSource;
  uint16_t *destinationCursor;
  bool capacityUnderflow;
  RichTextCopyResult successResult;
  RichTextCopyResult errorResult;
  uint16_t *nestedReturnStack[RICHTEXT_NESTING_LIMIT]; /* the original's machine-stack chain */
  int nestedDepth;
  
  nestedDepth = 0;
  destinationCursor = destination;
  while( true ) {
    while( true ) {
      commandCodeUnit = *source;
      nextSource = source + 1;
      if (commandCodeUnit == 0) break;
      if ((short)commandCodeUnit < 0) {
        switch(commandCodeUnit & RICHTEXT_OPCODE_MASK) {
        default:
          capacityUnderflow = capacityBytes < 2;
          capacityBytes = capacityBytes - 2;
          if (capacityUnderflow || capacityBytes == 0)
          goto RichTextCommandStream_CopyExpanded_TerminateOutputAndReturnCapacityError;
          *destinationCursor = commandCodeUnit & RICHTEXT_OPCODE_MASK | RICHTEXT_COMMAND_FLAG;
          destinationCursor++;
          source = nextSource;
          break;
        case RICHTEXT_OP_LITERAL_COLOR:
          capacityUnderflow = capacityBytes < RICHTEXT_RECORD_UNITS_LITERAL_COLOR * 2;
          capacityBytes = capacityBytes - RICHTEXT_RECORD_UNITS_LITERAL_COLOR * 2;
          if (capacityUnderflow || capacityBytes == 0)
          goto RichTextCommandStream_CopyExpanded_TerminateOutputAndReturnCapacityError;
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
          if (capacityUnderflow || capacityBytes == 0)
          goto RichTextCommandStream_CopyExpanded_TerminateOutputAndReturnCapacityError;
          for (wordsRemaining = RICHTEXT_RECORD_UNITS_INLINE_VALUE; wordsRemaining != 0; wordsRemaining--) {
            *destinationCursor = *source;
            source++;
            destinationCursor++;
          }
          break;
        case RICHTEXT_OP_CALL_NESTED:
          if (nestedDepth == RICHTEXT_NESTING_LIMIT)
          goto RichTextCommandStream_CopyExpanded_TerminateOutputAndReturnCapacityError;
          nestedReturnStack[nestedDepth++] = nextSource;
          source = *(uint16_t **)nextSource;
          break;
        case RICHTEXT_OP_JUMP_NESTED:
          source = *(uint16_t **)nextSource;
          break;
        case RICHTEXT_OP_INLINE_IMAGE:
          capacityUnderflow = capacityBytes < RICHTEXT_RECORD_UNITS_INLINE_IMAGE * 2;
          capacityBytes = capacityBytes - RICHTEXT_RECORD_UNITS_INLINE_IMAGE * 2;
          if (capacityUnderflow || capacityBytes == 0)
          goto RichTextCommandStream_CopyExpanded_TerminateOutputAndReturnCapacityError;
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
        if (capacityUnderflow || capacityBytes == 0)
        goto RichTextCommandStream_CopyExpanded_TerminateOutputAndReturnCapacityError;
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
    successResult.bytesWritten = (int)destinationCursor - (int)destination;
    successResult.overflowed = false;
    return successResult;
  }
RichTextCommandStream_CopyExpanded_TerminateOutputAndReturnCapacityError:
  destinationCursor[-1] = 0;
  errorResult.overflowed = true;
  errorResult.bytesWritten = FATAL_ERROR_GENERAL_FAILURE;
  return errorResult;
}


/* Address: 0x0041CF30.
   Measures one line of a rich-text command stream (up to its end or the first line break), following nested
   streams and font changes and including inline images. Returns the total width in EAX and the tallest glyph or
   image in EDX; used to align a line before it is drawn.
*/
RichTextExtentRegs __thandor_eax_edx_cf_preserve_ecx
RichTextCommandStream_MeasureRegs(UiPackedTextStyle packedStyle,uint16_t *commandStream)

{
  /* Rewritten from the assembly (0x0041CF30-0x0041D0E0). Command 0x18 enters a nested stream and pushes
     the return position on the machine stack; its terminator pops it and resumes 8 bytes later.
     Ghidra turned that stack into a counter, so nested text was measured forever. */
  uint16_t *returnStack[RICHTEXT_NESTING_LIMIT];
  int nesting = 0;
  RichTextExtentRegs extent;
  GlyphSizeResult glyphSize;
  TextureSizeResult textureSize;
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
      glyphSize = FontGlyph_GetLogicalSizeActiveRegs((GraphicsSubresourceIndex)value);
      extent.widthPixels = extent.widthPixels + glyphSize.width;
      if (extent.heightPixels < glyphSize.lineHeight) {
        extent.heightPixels = glyphSize.lineHeight;
      }
      continue;
    }
    switch (value & RICHTEXT_OPCODE_MASK) {
    case RICHTEXT_OP_LITERAL_COLOR:
      commandStream = command + RICHTEXT_RECORD_UNITS_LITERAL_COLOR;
      break;
    case RICHTEXT_OP_SELECT_FONT_FIRST:
    case 9:
    case 10:
    case 0xb:
    case 0xc:
    case 0xd:
    case 0xe:
    case 0xf:
      g_ActiveFontIndex = value & 0xf;
      break;
    case RICHTEXT_OP_FIXED_SPACE:
      glyphSize = FontGlyph_GetLogicalSizeActiveRegs(' ');
      extent.widthPixels = extent.widthPixels + glyphSize.width;
      if (extent.heightPixels < glyphSize.lineHeight) {
        extent.heightPixels = glyphSize.lineHeight;
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
   g_RichTextRuntimeBufferUsedWords past it. Font commands are applied on the way. Returns the tallest glyph or
   image height of the line in EAX; CF is set when the line ends the text.
*/
WrappedLineResult __thandor_eax_cf_preserve_ecx_edx
RichTextCommandStream_MeasureNextWrappedLine(UiPixelExtent maximumWidth)

{
  GraphicsSubresourceIndex glyphSubresource;
  uint32_t lineWidth;
  uint32_t maxLineHeight;
  uint8_t *commandCursor;
  uint8_t *readCursor;
  WrappedLineResult lineResult;
  GlyphSizeResult glyphSize;
  TextureSizeResult imageSize;
  uint8_t *wrapPoint;

  glyphSize = FontGlyph_GetLogicalSizeActiveRegs(0);
  lineWidth = 0;
  wrapPoint = NULL;
  maxLineHeight = glyphSize.lineHeight;
  readCursor = g_FontRuntimeBuffer + g_RichTextRuntimeBufferUsedWords * sizeof(uint16_t);
  for (;;) {
    commandCursor = readCursor;
    glyphSubresource = (GraphicsSubresourceIndex)*(short *)commandCursor;
    readCursor = commandCursor + 2;
    if (glyphSubresource == ' ') {
      /* A space is a wrap opportunity while the line up to it still fits. */
      glyphSize = FontGlyph_GetLogicalSizeActiveRegs(' ');
      if (maximumWidth < lineWidth) break;
      lineWidth = lineWidth + glyphSize.width;
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
      glyphSize = FontGlyph_GetLogicalSizeActiveRegs(glyphSubresource);
      lineWidth = lineWidth + glyphSize.width;
      if (maxLineHeight < glyphSize.lineHeight) {
        maxLineHeight = glyphSize.lineHeight;
      }
      continue;
    }
    /* the cursors are byte pointers: record lengths are code units * 2 */
    switch(glyphSubresource & RICHTEXT_OPCODE_MASK) {
    case RICHTEXT_OP_LITERAL_COLOR:
      readCursor = commandCursor + RICHTEXT_RECORD_UNITS_LITERAL_COLOR * sizeof(uint16_t);
      break;
    case RICHTEXT_OP_SELECT_FONT_FIRST:
    case 9:
    case 10:
    case 0xb:
    case 0xc:
    case 0xd:
    case 0xe:
    case 0xf:
      g_ActiveFontIndex = glyphSubresource & 0xf;
      break;
    case RICHTEXT_OP_FIXED_SPACE:
      glyphSize = FontGlyph_GetLogicalSizeActiveRegs(' ');
      lineWidth = lineWidth + glyphSize.width;
      if (maxLineHeight < glyphSize.lineHeight) {
        maxLineHeight = glyphSize.lineHeight;
      }
      break;
    case RICHTEXT_OP_SOFT_HYPHEN:
      /* Soft hyphen: a wrap opportunity when the hyphen still fits. */
      glyphSize = FontGlyph_GetLogicalSizeActiveRegs('-');
      if (maximumWidth < glyphSize.width + lineWidth)
      goto RichTextCommandStream_MeasureNextWrappedLine_CommitWrapBoundary;
      wrapPoint = readCursor;
      break;
    case RICHTEXT_OP_LINE_BREAK:
      if (lineWidth <= maximumWidth) {
        wrapPoint = readCursor;
      }
      goto RichTextCommandStream_MeasureNextWrappedLine_CommitWrapBoundary;
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
RichTextCommandStream_MeasureNextWrappedLine_CommitWrapBoundary:
  if (wrapPoint == NULL) {
    wrapPoint = readCursor;
  }
  g_RichTextRuntimeBufferUsedWords = (uint32_t)((int)wrapPoint - (int)g_FontRuntimeBuffer) >> 1;
  lineResult.endOfText = *(short *)(wrapPoint - 2) == 0; /* the line ended at the stream terminator */
  lineResult.lineAdvancePixels = maxLineHeight;
  return lineResult;
}


/* Address: 0x0041D9F0.
   Draws the next line of the flattened rich-text runtime buffer at (drawX, drawY), wrapped to maximumWidth:
   a measure pass with the rules of RichTextCommandStream_MeasureNextWrappedLine finds the wrap point and the
   line height, then the draw pass renders glyphs, images and colour/font commands up to it (drawing the hyphen
   when the line wraps at a soft hyphen) and advances g_RichTextRuntimeBufferUsedWords. Returns the line height;
   CF is set when the line ends the text. Called directly by RichTextCommandStream_DrawWrappedBlock.
*/
WrappedLineResult __thandor_eax_cf_preserve_ecx_edx
RichTextCommandStream_DrawNextWrappedLine
          (UiPixelCoordinate clipTop,UiPixelCoordinate clipLeft,UiPixelCoordinate clipBottom,
          UiPixelCoordinate clipRight,UiPixelExtent maximumWidth,UiPixelCoordinate drawY,
          UiPixelCoordinate drawX)

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
  WrappedLineResult moreLinesResult;
  WrappedLineResult endResult;
  GlyphSizeResult glyphSize;
  TextureSizeResult imageSize;
  uint8_t *wrapPoint;
  
  glyphSize = FontGlyph_GetLogicalSizeActiveRegs(0);
  lineWidth = 0;
  drawCursor = g_FontRuntimeBuffer + g_RichTextRuntimeBufferUsedWords * sizeof(uint16_t);
  wrapPoint = NULL;
  lineHeight = glyphSize.lineHeight;
  scanCursor = drawCursor;
  savedFontIndexOrImageWidth = g_ActiveFontIndex; /* font commands of the measure pass are undone below */
  /* Measure pass (same rules as RichTextCommandStream_MeasureNextWrappedLine): find the wrap point and
     the line height. The cursors are byte pointers: record lengths are code units * 2. */
  for (;;) {
    measureCommand = scanCursor;
    glyphSubresource = (GraphicsSubresourceIndex)*(short *)measureCommand;
    scanCursor = measureCommand + 2;
    if (glyphSubresource == ' ') {
      glyphSize = FontGlyph_GetLogicalSizeActiveRegs(' ');
      if (maximumWidth < lineWidth) break;
      lineWidth = lineWidth + glyphSize.width;
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
      glyphSize = FontGlyph_GetLogicalSizeActiveRegs(glyphSubresource);
      lineWidth = lineWidth + glyphSize.width;
      if (lineHeight < glyphSize.lineHeight) {
        lineHeight = glyphSize.lineHeight;
      }
      continue;
    }
    switch(glyphSubresource & RICHTEXT_OPCODE_MASK) {
    case RICHTEXT_OP_LITERAL_COLOR:
      scanCursor = measureCommand + RICHTEXT_RECORD_UNITS_LITERAL_COLOR * sizeof(uint16_t);
      break;
    case RICHTEXT_OP_SELECT_FONT_FIRST:
    case 9:
    case 10:
    case 0xb:
    case 0xc:
    case 0xd:
    case 0xe:
    case 0xf:
      g_ActiveFontIndex = glyphSubresource & 0xf;
      break;
    case RICHTEXT_OP_FIXED_SPACE:
      glyphSize = FontGlyph_GetLogicalSizeActiveRegs(' ');
      lineWidth = lineWidth + glyphSize.width;
      if (lineHeight < glyphSize.lineHeight) {
        lineHeight = glyphSize.lineHeight;
      }
      break;
    case RICHTEXT_OP_SOFT_HYPHEN:
      glyphSize = FontGlyph_GetLogicalSizeActiveRegs('-');
      if (maximumWidth < glyphSize.width + lineWidth)
      goto RichTextCommandStream_DrawNextWrappedLine_CommitWrapBoundaryAndBeginDrawing;
      wrapPoint = scanCursor;
      break;
    case RICHTEXT_OP_LINE_BREAK:
      if (lineWidth <= maximumWidth) {
        wrapPoint = scanCursor;
      }
      goto RichTextCommandStream_DrawNextWrappedLine_CommitWrapBoundaryAndBeginDrawing;
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
RichTextCommandStream_DrawNextWrappedLine_CommitWrapBoundaryAndBeginDrawing:
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
      endResult.endOfText = true;
      endResult.lineAdvancePixels = lineHeight;
      return endResult;
    }
    if (glyphSubresource == ' ') {
      if (wrapPoint <= drawCursor) break;
      glyphAdvance = FontGlyph_DrawVerticallyCentered
                        (clipTop,clipLeft,clipBottom,clipRight,' ',lineHeight,lineBottom,drawX);
      drawX = drawX + glyphAdvance;
      continue;
    }
    if (-1 < (int)glyphSubresource) {
      glyphAdvance = FontGlyph_DrawVerticallyCentered
                        (clipTop,clipLeft,clipBottom,clipRight,glyphSubresource,lineHeight,lineBottom,drawX);
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
           (((((((scanCursor[4] & 0xf) << 0x18 | (uint32_t)*drawCursor << 0x1c) >> 4 | (uint32_t)scanCursor[8] << 0x1c) >>
               4 | (uint32_t)scanCursor[6] << 0x1c) >> 4 | (uint32_t)scanCursor[0xc] << 0x1c) >> 4 |
            (uint32_t)scanCursor[10] << 0x1c) >> 4 | (uint32_t)scanCursor[0x10] << 0x1c) >> 4 |
           (uint32_t)scanCursor[0xe] << 0x1c;
      drawCursor = scanCursor + RICHTEXT_RECORD_UNITS_LITERAL_COLOR * sizeof(uint16_t);
      break;
    case RICHTEXT_OP_SELECT_FONT_FIRST:
    case 9:
    case 10:
    case 0xb:
    case 0xc:
    case 0xd:
    case 0xe:
    case 0xf:
      g_ActiveFontIndex = glyphSubresource & 0xf;
      break;
    case RICHTEXT_OP_FIXED_SPACE:
      glyphAdvance = FontGlyph_DrawVerticallyCentered
                        (clipTop,clipLeft,clipBottom,clipRight,' ',lineHeight,lineBottom,drawX);
      drawX = drawX + glyphAdvance;
      break;
    case RICHTEXT_OP_SOFT_HYPHEN:
      if (wrapPoint <= drawCursor) {
        /* The line wraps at this soft hyphen: draw the hyphen and end the line. */
        FontGlyph_DrawVerticallyCentered
                  (clipTop,clipLeft,clipBottom,clipRight,'-',lineHeight,lineBottom,drawX);
        goto RichTextCommandStream_DrawNextWrappedLine_EndLine;
      }
      break;
    case RICHTEXT_OP_LINE_BREAK:
      goto RichTextCommandStream_DrawNextWrappedLine_EndLine;
    case RICHTEXT_OP_INLINE_IMAGE:
      /* the image sits on the line's bottom edge */
      imageSize = g_GraphicsTextureSourceGetLogicalSize
                         (*(uint32_t *)(scanCursor + 6),*(GraphicsTextureSourceAsset **)drawCursor);
      savedFontIndexOrImageWidth = imageSize.logicalWidthPixels;
      g_GraphicsTextureSourceBlitSourceAlpha
                (clipTop,clipLeft,clipBottom,clipRight,lineBottom - imageSize.logicalHeightPixels,drawX,
                 *(uint32_t *)(scanCursor + 6),*(GraphicsTextureSourceAsset **)drawCursor,g_FramebufferAccess);
      drawX = drawX + savedFontIndexOrImageWidth;
      drawCursor = scanCursor + RICHTEXT_RECORD_UNITS_INLINE_IMAGE * sizeof(uint16_t);
    }
  }
RichTextCommandStream_DrawNextWrappedLine_EndLine:
  g_RichTextRuntimeBufferUsedWords = (uint32_t)((int)drawCursor - (int)g_FontRuntimeBuffer) >> 1;
  moreLinesResult.endOfText = false;
  moreLinesResult.lineAdvancePixels = lineHeight;
  return moreLinesResult;
}


/* Address: 0x0041D840.
   Copies commandStream into g_FontRuntimeBuffer with every nested stream inlined, so the line measuring and
   drawing code can walk one flat stream: glyphs and most commands are copied, literal colours and inline images
   with their payload, nested-stream commands are followed instead of copied and the reserved and inline-value
   commands are dropped. Output beyond RICHTEXT_RUNTIME_BUFFER_UNITS is discarded; the read position
   g_RichTextRuntimeBufferUsedWords is reset to the start.
*/
void __thandor_void_preserve_eax_ecx_edx
RichTextCommandStream_FlattenNestedToRuntimeBuffer(uint16_t *commandStream)

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
        remainingWords = remainingWords - 1;
        outputCursor = outputCursor + 1;
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
    case 7:
    case 0x13:
    case RICHTEXT_OP_INLINE_VALUE_0:
    case RICHTEXT_OP_INLINE_VALUE_1:
    case RICHTEXT_OP_INLINE_VALUE_2:
    case 0x17:
    case 0x1b:
    case 0x1c:
    case 0x1d:
    case 0x1e:
    case 0x1f:
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


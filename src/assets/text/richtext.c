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
   Ownership: assets/text/richtext.
   Purpose: Flattens a nested rich-text command stream, initializes packed style state, repeatedly measures wrapped
   lines, and returns maximum width in EAX with total height in EDX. Typed parameters: p2
   maximumWidth→UiPixelExtent_V301. Calling convention, parameter storage, body bytes, control flow, globals,
   locals, and executable data remain unchanged.
   Local calls: RichTextCommandStream_FlattenNestedToRuntimeBuffer, RichTextCommandStream_MeasureNextWrappedLineCf.
*/
RichTextExtentRegs __thandor_eax_edx_cf_preserve_ecx
RichTextCommandStream_MeasureWrappedBlockRegs
          (dword packedStyle,word *commandStream,UiPixelExtent maximumWidth)

{
  uint colorPaletteIndex;
  int totalHeight;
  RichTextExtentRegs blockExtent;
  RichTextLineAdvanceEaxCf5 lineResult;
  
  RichTextCommandStream_FlattenNestedToRuntimeBuffer(commandStream);
  colorPaletteIndex = packedStyle >> 0x10 & 7;
  g_ActiveFontIndex = packedStyle >> 0x18 & 7;
  g_RichTextCurrentColorArgb = (&g_RichTextColorPalette0Argb)[colorPaletteIndex];
  g_RichTextCurrentShadowOffset = (&g_RichTextShadowOffsetPalette0)[colorPaletteIndex];
  totalHeight = 0;
  g_RichTextSavedColorArgb = g_RichTextCurrentColorArgb;
  g_RichTextSavedShadowOffset = g_RichTextCurrentShadowOffset;
  while( true ) {
    lineResult = RichTextCommandStream_MeasureNextWrappedLineCf(maximumWidth);
    if (lineResult.carry) break;
    totalHeight = totalHeight + lineResult.lineAdvancePixels;
  }
  blockExtent.heightPixels = totalHeight + lineResult.lineAdvancePixels;
  blockExtent.widthPixels = maximumWidth;
  return blockExtent;
}


/* Address: 0x0041D7C0.
   Ownership: assets/text/richtext.
   Purpose: Flattens a nested rich-text stream, initializes packed style state, and draws successive wrapped lines
   until the final CF-set line result. Typed parameters: p0 clipTop→UiPixelCoordinate_V297, p1
   clipLeft→UiPixelCoordinate_V297, p2 clipBottom→UiPixelCoordinate_V297, p3 clipRight→UiPixelCoordinate_V297, p6
   maximumWidth→UiPixelExtent_V301, p7 drawY→UiPixelCoordinate_V297, p8 drawX→UiPixelCoordinate_V297. Calling
   convention, parameter storage, body bytes, control flow, globals, locals, and executable data remain unchanged.
   Local calls: RichTextCommandStream_FlattenNestedToRuntimeBuffer, RichTextCommandStream_DrawNextWrappedLineCf.
*/
void __thandor_void_preserve_eax_ecx_edx
RichTextCommandStream_DrawWrappedBlockCf
          (UiPixelCoordinate clipTop,UiPixelCoordinate clipLeft,UiPixelCoordinate clipBottom,
          UiPixelCoordinate clipRight,dword packedStyle,word *commandStream,
          UiPixelExtent maximumWidth,UiPixelCoordinate drawY,UiPixelCoordinate drawX)

{
  uint colorPaletteIndex;
  RichTextLineAdvanceEaxCf5 lineResult;
  
  RichTextCommandStream_FlattenNestedToRuntimeBuffer(commandStream);
  colorPaletteIndex = packedStyle >> 0x10 & 7;
  g_ActiveFontIndex = packedStyle >> 0x18 & 7;
  g_RichTextCurrentColorArgb = (&g_RichTextColorPalette0Argb)[colorPaletteIndex];
  g_RichTextCurrentShadowOffset = (&g_RichTextShadowOffsetPalette0)[colorPaletteIndex];
  g_RichTextSavedColorArgb = g_RichTextCurrentColorArgb;
  g_RichTextSavedShadowOffset = g_RichTextCurrentShadowOffset;
  while( true ) {
    lineResult = RichTextCommandStream_DrawNextWrappedLineCf
                      (clipTop,clipLeft,clipBottom,clipRight,maximumWidth,drawY,drawX);
    if (lineResult.carry) break;
    drawY = drawY + lineResult.lineAdvancePixels;
  }
  return;
}


/* Address: 0x0041D4A0.
   Ownership: assets/text/richtext.
   Purpose: Measures, aligns, and draws one rich-text line while interpreting glyph, style, color, font, and
   embedded-image commands. Existing register returns are preserved. Typed parameters: p0
   clipTop→UiPixelCoordinate_V297, p1 clipLeft→UiPixelCoordinate_V297, p2 clipBottom→UiPixelCoordinate_V297, p3
   clipRight→UiPixelCoordinate_V297, p4 packedStyle→UiPackedTextStyle_V301, p6 drawX→UiPixelCoordinate_V297, p7
   baselineY→UiPixelCoordinate_V297. Calling convention, parameter storage, body bytes, control flow, globals,
   locals, and executable data remain unchanged.
   Local calls: RichTextCommandStream_MeasureRegs.
   Cross-module calls: FontGlyph_DrawBottomAligned [assets/text/resources].
*/
bool __thandor_cf_preserve_eax_ecx_edx
RichTextCommandStream_DrawSingleLine
          (UiPixelCoordinate clipTop,UiPixelCoordinate clipLeft,UiPixelCoordinate clipBottom,
          UiPixelCoordinate clipRight,UiPackedTextStyle packedStyle,word *commandStream,
          UiPixelCoordinate lineTopY,UiPixelCoordinate penX)

{
  int lineBaselineY;
  uint alignShiftOrPaletteIndex;
  GraphicsSubresourceIndex glyphSubresource;
  int glyphAdvance;
  dword imageWidth;
  word *commandCursor;
  RichTextExtentRegs lineExtent;
  GraphicsTextureSizeEaxEdxCf9 imageSize;
  word *nestedReturnStack[RICHTEXT_NESTING_LIMIT]; /* the original's machine-stack chain */
  int nestedDepth;
  
  lineExtent = RichTextCommandStream_MeasureRegs(packedStyle,commandStream);
  alignShiftOrPaletteIndex = lineExtent.widthPixels;
  lineBaselineY = lineTopY + lineExtent.heightPixels;
  if ((packedStyle & 1) == 0) {
    if ((packedStyle & 2) == 0)
    goto RichTextCommandStream_DrawSingleLine_InitializeStyleAndBeginDrawing;
    alignShiftOrPaletteIndex = alignShiftOrPaletteIndex >> 1;
  }
  penX = penX - alignShiftOrPaletteIndex;
RichTextCommandStream_DrawSingleLine_InitializeStyleAndBeginDrawing:
  alignShiftOrPaletteIndex = packedStyle >> 0x10 & 7;
  g_ActiveFontIndex = packedStyle >> 0x18 & 7;
  g_RichTextCurrentColorArgb = (&g_RichTextColorPalette0Argb)[alignShiftOrPaletteIndex];
  g_RichTextCurrentShadowOffset = (&g_RichTextShadowOffsetPalette0)[alignShiftOrPaletteIndex];
  nestedDepth = 0;
  g_RichTextSavedColorArgb = g_RichTextCurrentColorArgb;
  g_RichTextSavedShadowOffset = g_RichTextCurrentShadowOffset;
switchD_0041d537_caseD_7:
  while( true ) {
    commandCursor = commandStream;
    glyphSubresource = (GraphicsSubresourceIndex)(short)*commandCursor;
    commandStream = commandCursor + 1;
    if (glyphSubresource != 0) break;
    if (nestedDepth == 0) {
      return false;
    }
    commandStream = (word *)((byte *)nestedReturnStack[--nestedDepth] + 8);
  }
  if ((int)glyphSubresource < 0) goto code_r0x0041d534;
  goto RichTextCommandStream_DrawSingleLine_DrawGlyphAndAdvanceX;
code_r0x0041d534:
  switch(glyphSubresource & 0x1f) {
  case 0:
    g_RichTextCurrentColorArgb = g_RichTextColorPalette0Argb;
    g_RichTextCurrentShadowOffset = g_RichTextShadowOffsetPalette0;
    break;
  case 1:
    g_RichTextCurrentColorArgb = g_RichTextColorPalette1Argb;
    g_RichTextCurrentShadowOffset = g_RichTextShadowOffsetPalette1;
    break;
  case 2:
    g_RichTextCurrentColorArgb = g_RichTextColorPalette2Argb;
    g_RichTextCurrentShadowOffset = g_RichTextShadowOffsetPalette2;
    break;
  case 3:
    g_RichTextCurrentColorArgb = g_RichTextColorPalette3Argb;
    g_RichTextCurrentShadowOffset = g_RichTextShadowOffsetPalette3;
    break;
  case 4:
    g_RichTextSavedColorArgb = g_RichTextCurrentColorArgb;
    g_RichTextSavedShadowOffset = g_RichTextCurrentShadowOffset;
    break;
  case 5:
    g_RichTextCurrentColorArgb = g_RichTextSavedColorArgb;
    g_RichTextCurrentShadowOffset = g_RichTextSavedShadowOffset;
    break;
  case 6:
    g_RichTextCurrentColorArgb =
         ((((((((byte)commandCursor[2] & 0xf) << 0x18 | (uint)(byte)*commandStream << 0x1c) >> 4 |
             (uint)(byte)commandCursor[4] << 0x1c) >> 4 | (uint)(byte)commandCursor[3] << 0x1c) >> 4 |
           (uint)(byte)commandCursor[6] << 0x1c) >> 4 | (uint)(byte)commandCursor[5] << 0x1c) >> 4 |
         (uint)(byte)commandCursor[8] << 0x1c) >> 4 | (uint)(byte)commandCursor[7] << 0x1c;
    commandStream = commandCursor + 9;
    break;
  case 8:
  case 9:
  case 10:
  case 0xb:
  case 0xc:
  case 0xd:
  case 0xe:
  case 0xf:
    g_ActiveFontIndex = glyphSubresource & 0xf;
    break;
  case 0x10:
    glyphSubresource = 0x20;
RichTextCommandStream_DrawSingleLine_DrawGlyphAndAdvanceX:
    glyphAdvance = FontGlyph_DrawBottomAligned
                      (clipTop,clipLeft,clipBottom,clipRight,glyphSubresource,lineBaselineY,penX
                      );
    penX = penX + glyphAdvance;
    break;
  case 0x12:
    return false;
  case 0x14:
  case 0x15:
  case 0x16:
    commandStream = commandCursor + 3;
    break;
  case 0x18:
    if (nestedDepth == RICHTEXT_NESTING_LIMIT) {
      return false;
    }
    nestedReturnStack[nestedDepth++] = commandStream;
    commandStream = *(word **)commandStream;
    break;
  case 0x19:
    commandStream = *(word **)commandStream;
    break;
  case 0x1a:
    imageSize = (*g_GraphicsTextureSourceGetLogicalSize)
                      (*(dword *)(commandCursor + 3),*(GraphicsTextureSourceAsset **)commandStream);
    imageWidth = imageSize.logicalWidthPixels;
    (*g_GraphicsTextureSourceBlitSourceAlpha)
              (clipTop,clipLeft,clipBottom,clipRight,lineBaselineY - imageSize.logicalHeightPixels,
               penX,*(dword *)(commandCursor + 3),*(GraphicsTextureSourceAsset **)commandStream,
               g_FramebufferAccess);
    penX = penX + imageWidth;
    commandStream = commandCursor + 5;
  }
  goto switchD_0041d537_caseD_7;
}


/* Address: 0x0041B100.
   Ownership: assets/text/richtext.
   Purpose: Scans the engine UTF-16 command stream and replaces the payload pointer in command tags 0x18/0x19 whose
   selector matches. Typed parameters: p0 selector→RichTextCommandSelector_V342. Calling convention, exact
   VariableStorage serialization, function body bytes, control flow, globals, locals, and executable data remain
   unchanged.
*/
void __thandor_void_preserve_eax_ecx_edx
RichTextCommandStream_PatchPayloadBySelector
          (RichTextCommandSelector selector,void *replacementPayload,word *stream)

{
  ushort commandCodeUnit;
  ushort *commandCursor;
  
  while( true ) {
    commandCursor = stream;
    commandCodeUnit = *commandCursor;
    if (commandCodeUnit == 0) break;
    stream = commandCursor + 1;
    if ((short)commandCodeUnit < 0) {
      switch(commandCodeUnit & 0x1f) {
      case 6:
        stream = commandCursor + 9;
        break;
      case 0x14:
      case 0x15:
      case 0x16:
        stream = commandCursor + 3;
        break;
      case 0x18:
      case 0x19:
        stream = commandCursor + 5;
        if (selector == *(int *)(commandCursor + 3)) {
          *(void **)(commandCursor + 1) = replacementPayload;
        }
        break;
      case 0x1a:
        stream = commandCursor + 5;
      }
    }
  }
  return;
}


/* Address: 0x0041B200.
   Ownership: assets/text/richtext.
   Purpose: Handles rich text command stream bind texture source.
*/
void __thandor_void_preserve_eax_ecx
RichTextCommandStream_BindTextureSource(GraphicsTextureSourceAsset *textureSource,word *stream)

{
  ushort *commandCursor;
  word *streamCursor;
  ushort commandCodeUnit;
  
  streamCursor = stream;
  while( true ) {
    commandCursor = streamCursor;
    commandCodeUnit = *commandCursor;
    streamCursor = commandCursor + 1;
    if (commandCodeUnit == 0) break;
    if ((short)commandCodeUnit < 0) {
      switch(commandCodeUnit & 0x1f) {
      case 6:
        streamCursor = commandCursor + 9;
        break;
      case 0x14:
      case 0x15:
      case 0x16:
        streamCursor = commandCursor + 3;
        break;
      case 0x18:
      case 0x19:
        streamCursor = commandCursor + 5;
        break;
      case 0x1a:
        *(GraphicsTextureSourceAsset **)streamCursor = textureSource;
        streamCursor = commandCursor + 5;
      }
    }
  }
  return;
}


/* Address: 0x0041B300.
   Ownership: assets/text/richtext.
   Purpose: Physical RET cleanup=12 stack bytes. This function object claims Listing ownership for a previously
   unowned multi-entry/shared-tail/computed-dispatch region; it does not assert that every member entry is an
   independent ABI-level function. Body boundaries remain exact and are not split into speculative ABI functions.
   Typed parameters: p0 param_1→RichTextCommandPayload32_V342, p1 param_2→RichTextCommandOrdinal_V342. Calling
   convention, exact VariableStorage serialization, function body bytes, control flow, globals, locals, and
   executable data remain unchanged.
*/
bool __thandor_cf_preserve_eax_ecx_edx
RichTextCommandStream_FindNthCommandPayloadPair
          (RichTextCommandOrdinal commandOrdinal,RichTextCommandPayload32 payloadValue,
          ushort *commandStream)

{
  ushort commandCodeUnit;
  int remainingCount;
  ushort *commandCursor;
  
  remainingCount = commandOrdinal + 1;
  do {
    do {
      commandCursor = commandStream;
      commandCodeUnit = *commandCursor;
      if (commandCodeUnit == 0) {
        return true;
      }
      commandStream = commandCursor + 1;
    } while (-1 < (short)commandCodeUnit);
    switch(commandCodeUnit & 0x1f) {
    case 6:
      commandStream = commandCursor + 9;
      break;
    case 0x14:
    case 0x15:
    case 0x16:
      remainingCount = remainingCount + -1;
      commandStream = commandCursor + 3;
      if (remainingCount == 0) {
        *(RichTextCommandPayload32 *)(commandCursor + 1) = payloadValue;
        return false;
      }
      break;
    case 0x18:
    case 0x19:
    case 0x1a:
      commandStream = commandCursor + 5;
    }
  } while( true );
}


/* Address: 0x0041B420.
   Ownership: assets/text/richtext.
   Purpose: Physical RET cleanup=8 stack bytes. This function object claims Listing ownership for a previously
   unowned multi-entry/shared-tail/computed-dispatch region; it does not assert that every member entry is an
   independent ABI-level function. Body boundaries remain exact and are not split into speculative ABI functions.
   Typed parameters: p0 param_1→RichTextNestedStreamPointerValue32_V345. Calling convention, complete
   VariableStorage serialization, function bytes, control flow, globals, locals, and executable data remain
   unchanged.
*/
void __thandor_void_preserve_eax_ecx_edx
RichTextCommandStream_PatchNestedStreamPointerPayloads
          (RichTextNestedStreamPointerValue32 nestedStreamPointerValue,ushort *commandStream)

{
  ushort *commandCursor;
  RichTextNestedStreamPointerValue32 *streamCursor;
  ushort commandCodeUnit;
  
  streamCursor = (RichTextNestedStreamPointerValue32 *)commandStream;
  while( true ) {
    commandCursor = (ushort *)streamCursor;
    commandCodeUnit = *commandCursor;
    streamCursor = (RichTextNestedStreamPointerValue32 *)(commandCursor + 1);
    if (commandCodeUnit == 0) break;
    if ((short)commandCodeUnit < 0) {
      switch(commandCodeUnit & 0x1f) {
      case 6:
        streamCursor = (RichTextNestedStreamPointerValue32 *)(commandCursor + 9);
        break;
      case 0x14:
      case 0x15:
      case 0x16:
        streamCursor = (RichTextNestedStreamPointerValue32 *)(commandCursor + 3);
        break;
      case 0x18:
      case 0x19:
        *streamCursor = nestedStreamPointerValue;
        streamCursor = (RichTextNestedStreamPointerValue32 *)(commandCursor + 5);
        break;
      case 0x1a:
        streamCursor = (RichTextNestedStreamPointerValue32 *)(commandCursor + 5);
      }
    }
  }
  return;
}


/* Address: 0x0041B520.
   Ownership: assets/text/richtext.
   Purpose: Physical RET cleanup=12 stack bytes. This function object claims Listing ownership for a previously
   unowned multi-entry/shared-tail/computed-dispatch region; it does not assert that every member entry is an
   independent ABI-level function. Body boundaries remain exact and are not split into speculative ABI functions.
   Typed parameters: p0 param_1→RichTextOpcode1APayloadValue32_V345. Calling convention, complete VariableStorage
   serialization, function bytes, control flow, globals, locals, and executable data remain unchanged.
*/
void __thandor_void_preserve_eax_ecx_edx
RichTextCommandStream_PatchOpcode1APayloadPair
          (RichTextOpcode1APayloadValue32 opcode1APayloadValue,
          RichTextCommandPayload32 leadingPayloadValue,ushort *commandStream)

{
  ushort *commandCursor;
  RichTextCommandPayload32 *streamCursor;
  ushort commandCodeUnit;
  
  streamCursor = (RichTextCommandPayload32 *)commandStream;
  while( true ) {
    commandCursor = (ushort *)streamCursor;
    commandCodeUnit = *commandCursor;
    streamCursor = (RichTextCommandPayload32 *)(commandCursor + 1);
    if (commandCodeUnit == 0) break;
    if ((short)commandCodeUnit < 0) {
      switch(commandCodeUnit & 0x1f) {
      case 6:
        streamCursor = (RichTextCommandPayload32 *)(commandCursor + 9);
        break;
      case 0x14:
      case 0x15:
      case 0x16:
        streamCursor = (RichTextCommandPayload32 *)(commandCursor + 3);
        break;
      case 0x18:
      case 0x19:
        streamCursor = (RichTextCommandPayload32 *)(commandCursor + 5);
        break;
      case 0x1a:
        *streamCursor = leadingPayloadValue;
        *(RichTextOpcode1APayloadValue32 *)(commandCursor + 3) = opcode1APayloadValue;
        streamCursor = (RichTextCommandPayload32 *)(commandCursor + 5);
      }
    }
  }
  return;
}


/* Address: 0x0041B620.
   Ownership: assets/text/richtext.
   Purpose: Physical RET cleanup=8 stack bytes. This function object claims Listing ownership for a previously
   unowned multi-entry/shared-tail/computed-dispatch region; it does not assert that every member entry is an
   independent ABI-level function. Body boundaries remain exact and are not split into speculative ABI functions.
   Typed parameters: p0 param_1→RichTextInlinePayloadValue32_V345. Calling convention, complete VariableStorage
   serialization, function bytes, control flow, globals, locals, and executable data remain unchanged.
*/
void __thandor_void_preserve_eax_ecx_edx
RichTextCommandStream_PatchInlinePayloads
          (RichTextInlinePayloadValue32 inlinePayloadValue,ushort *commandStream)

{
  ushort *commandCursor;
  RichTextInlinePayloadValue32 *streamCursor;
  ushort commandCodeUnit;
  
  streamCursor = (RichTextInlinePayloadValue32 *)commandStream;
  while( true ) {
    commandCursor = (ushort *)streamCursor;
    commandCodeUnit = *commandCursor;
    streamCursor = (RichTextInlinePayloadValue32 *)(commandCursor + 1);
    if (commandCodeUnit == 0) break;
    if ((short)commandCodeUnit < 0) {
      switch(commandCodeUnit & 0x1f) {
      case 6:
        streamCursor = (RichTextInlinePayloadValue32 *)(commandCursor + 9);
        break;
      case 0x14:
      case 0x15:
      case 0x16:
        *streamCursor = inlinePayloadValue;
        streamCursor = (RichTextInlinePayloadValue32 *)(commandCursor + 3);
        break;
      case 0x18:
      case 0x19:
      case 0x1a:
        streamCursor = (RichTextInlinePayloadValue32 *)(commandCursor + 5);
      }
    }
  }
  return;
}


/* Address: 0x0041B720.
   Ownership: assets/text/richtext.
   Purpose: Physical RET cleanup=12 stack bytes. This function object claims Listing ownership for a previously
   unowned multi-entry/shared-tail/computed-dispatch region; it does not assert that every member entry is an
   independent ABI-level function. Body boundaries remain exact and are not split into speculative ABI functions.
   Typed parameters: p0 param_1→RichTextCommandFlagBits_V342, p1 param_2→RichTextCommandOrdinal_V342. Calling
   convention, exact VariableStorage serialization, function body bytes, control flow, globals, locals, and
   executable data remain unchanged.
*/
bool __thandor_cf_preserve_eax_ecx_edx
RichTextCommandStream_FindNthCommandFlagsPair(int commandOrdinal,uint flagBits,uint *commandStream)

{
  uint *streamCursor;
  int remainingCount;
  uint *commandCursor;
  ushort commandCodeUnit;
  
  remainingCount = commandOrdinal + 1;
  streamCursor = commandStream;
  do {
    do {
      commandCursor = streamCursor;
      commandCodeUnit = (ushort)*commandCursor;
      if (commandCodeUnit == 0) {
        return true;
      }
      streamCursor = (uint *)((int)commandCursor + 2);
    } while (-1 < (short)commandCodeUnit);
    switch(commandCodeUnit & 0x1f) {
    case 6:
      streamCursor = (uint *)((int)commandCursor + 0x12);
      break;
    case 0x14:
    case 0x15:
    case 0x16:
      remainingCount = remainingCount + -1;
      streamCursor = (uint *)((int)commandCursor + 6);
      if (remainingCount == 0) {
        *commandCursor = *commandCursor & 0xffff8014;
        *commandCursor = *commandCursor | flagBits;
        return false;
      }
      break;
    case 0x18:
    case 0x19:
    case 0x1a:
      streamCursor = (uint *)((int)commandCursor + 10);
    }
  } while( true );
}


/* Address: 0x0041B840.
   Ownership: assets/text/richtext.
   Purpose: Physical RET cleanup=8 stack bytes. This function object claims Listing ownership for a previously
   unowned multi-entry/shared-tail/computed-dispatch region; it does not assert that every member entry is an
   independent ABI-level function. Body boundaries remain exact and are not split into speculative ABI functions.
   Typed parameters: p1 param_2→RichTextCommandOrdinal_V342. Calling convention, exact VariableStorage
   serialization, function body bytes, control flow, globals, locals, and executable data remain unchanged.
*/
StatusValueEaxCf5 __thandor_eax_cf_preserve_ecx_edx
RichTextCommandStream_QueryNthCommandFlags(int commandOrdinal,ushort *commandStream)

{
  ushort commandCodeUnit;
  int remainingCount;
  uint *commandCursor;
  StatusValueEaxCf5 foundResult;
  StatusValueEaxCf5 endResult;
  
  remainingCount = commandOrdinal + 1;
  do {
    do {
      commandCursor = (uint *)commandStream;
      commandCodeUnit = (ushort)*commandCursor;
      if (commandCodeUnit == 0) {
        endResult.valueOrError = 0;
        endResult.carry = true;
        return endResult;
      }
      commandStream = (ushort *)((int)commandCursor + 2);
    } while (-1 < (short)commandCodeUnit);
    switch(commandCodeUnit & 0x1f) {
    case 6:
      commandStream = (ushort *)((int)commandCursor + 0x12);
      break;
    case 0x14:
    case 0x15:
    case 0x16:
      remainingCount = remainingCount + -1;
      commandStream = (ushort *)((int)commandCursor + 6);
      if (remainingCount == 0) {
        foundResult.valueOrError = *commandCursor & 3;
        foundResult.carry = false;
        return foundResult;
      }
      break;
    case 0x18:
    case 0x19:
    case 0x1a:
      commandStream = (ushort *)((int)commandCursor + 10);
    }
  } while( true );
}


/* Address: 0x0041B950.
   Ownership: assets/text/richtext.
   Purpose: Flattens one engine UTF-16 command stream into a bounded narrow buffer, follows nested tag 0x18/0x19
   streams, converts spacing/newline commands, and skips code units above 0xFF. CF reports insufficient capacity.
   Typed parameters: p0 capacityBytes→TextOutputCapacityBytes_V342. Calling convention, exact VariableStorage
   serialization, function body bytes, control flow, globals, locals, and executable data remain unchanged.
*/
StatusValueEaxCf5 __thandor_eax_cf_preserve_ecx_edx
RichTextCommandStream_CopyToNarrowCf
          (TextOutputCapacityBytes capacityBytes,byte *destination,word *source)

{
  word *readCursor;
  dword remainingCapacityBytes;
  ushort *commandCursor;
  word *streamCursor;
  bool newlineCapacityUnderflow;
  StatusValueEaxCf5 successResult;
  StatusValueEaxCf5 errorResult;
  word *nestedReturnStack[RICHTEXT_NESTING_LIMIT]; /* the original's machine-stack chain */
  int nestedDepth;
  ushort commandOrCodeUnit;
  
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
        switch(commandOrCodeUnit & 0x1f) {
        case 6:
          readCursor = commandCursor + 9;
          break;
        case 0x10:
          remainingCapacityBytes = remainingCapacityBytes - 1;
          if (remainingCapacityBytes == 0)
          goto RichTextCommandStream_CopyToNarrow_TerminateOutputAndReturnCapacityError;
          *destination = 0x20;
          destination = destination + 1;
          readCursor = streamCursor;
          break;
        case 0x12:
          newlineCapacityUnderflow = remainingCapacityBytes < 2;
          remainingCapacityBytes = remainingCapacityBytes - 2;
          if (newlineCapacityUnderflow || remainingCapacityBytes == 0)
          goto RichTextCommandStream_CopyToNarrow_TerminateOutputAndReturnCapacityError;
          destination[0] = 0xd;
          destination[1] = 10;
          destination = destination + 2;
          readCursor = streamCursor;
          break;
        case 0x14:
        case 0x15:
        case 0x16:
          readCursor = commandCursor + 3;
          break;
        case 0x18:
          if (nestedDepth == RICHTEXT_NESTING_LIMIT)
          goto RichTextCommandStream_CopyToNarrow_TerminateOutputAndReturnCapacityError;
          nestedReturnStack[nestedDepth++] = streamCursor;
          readCursor = *(ushort **)streamCursor;
          break;
        case 0x19:
          readCursor = *(ushort **)streamCursor;
          break;
        case 0x1a:
          readCursor = commandCursor + 5;
        }
      }
      else {
        readCursor = streamCursor;
        if ((commandOrCodeUnit & 0xff00) == 0) {
          remainingCapacityBytes = remainingCapacityBytes - 1;
          if (remainingCapacityBytes == 0)
          goto RichTextCommandStream_CopyToNarrow_TerminateOutputAndReturnCapacityError;
          *destination = (byte)commandOrCodeUnit;
          destination = destination + 1;
          readCursor = streamCursor;
        }
      }
    }
    if (nestedDepth == 0) break;
    readCursor = (ushort *)((byte *)nestedReturnStack[--nestedDepth] + 8);
  }
  if (0 < (int)remainingCapacityBytes) {
    *destination = 0;
    successResult.valueOrError = capacityBytes - (remainingCapacityBytes - 1);
    successResult.carry = false;
    return successResult;
  }
RichTextCommandStream_CopyToNarrow_TerminateOutputAndReturnCapacityError:
  destination[-1] = 0;
  errorResult.carry = true;
  errorResult.valueOrError = 0x14;
  return errorResult;
}


/* Address: 0x0041BCB0.
   Ownership: assets/text/richtext.
   Purpose: Physical RET cleanup=4 stack bytes. This function object claims Listing ownership for a previously
   unowned multi-entry/shared-tail/computed-dispatch region; it does not assert that every member entry is an
   independent ABI-level function. Body boundaries remain exact and are not split into speculative ABI functions.
   Typed parameters: p0 param_1→RichTextMarkupCapacityCodeUnits_V345. Calling convention, complete VariableStorage
   serialization, function bytes, control flow, globals, locals, and executable data remain unchanged.
*/
RichTextStringAssetEaxCf5 __thandor_eax_cf_preserve_edx
RichTextMarkup_ParseAndBuildStringAsset(byte *markupBytes)

{
  /* Unreachable: nothing in the original image calls 0x0041BCB0 or stores its address (a leftover
     of the TXT2STR converter). The stack slots below were never recovered; the body is kept
     only for completeness. */
  byte thandor_stack_frame[0x100]; /* unrecovered Ghidra stack slots (stack0x...), entry ESP at index 0x80 */
  byte markupByte;
  ushort codeUnit;
  wchar_t *memory;
  uint spanSizeOrDwordCount;
  uint remainingCapacityBytes;
  int groupKeyOrIndex;
  int *offsetTableCursor;
  int entryIndexOrOffset;
  int entryEndOrIndex;
  short codeUnitBias;
  undefined1 *stackSlot;
  undefined1 *callStackSlot;
  byte *tokenStart;
  byte *markupCursor;
  int *copySource;
  wchar_t *outputCursor;
  int *groupHeader;
  dword assetSizeOrTimestamp;
  int *assetWriteCursor;
  bool capacityUnderflow;
  bool insideTagOrUnderflow;
  RichTextStringAssetEaxCf5 errorMessageResult;
  RichTextStringAssetEaxCf5 capacityErrorResult;
  ArenaShrinkEaxCf5 shrinkResult;
  RichTextStringAssetEaxCf5 stringAsset;
  ArenaLargestAllocationEaxEcxCf9 largestBlock;
  WideNumberFormatFlags aWStackY_44 [2];
  dword dStackY_3c;
  int assetGroupCount;
  int tagCount;
  
  largestBlock = (*g_MemoryApi.allocLargestFreeBlock)();
  remainingCapacityBytes = largestBlock.blockSizeOrSentinel;
  memory = (wchar_t *)largestBlock.allocationOrError;
  if (!largestBlock.carry) {
    codeUnitBias = 0;
    tagCount = 0;
    markupCursor = markupBytes;
    outputCursor = memory;
    insideTagOrUnderflow = false;
RichTextMarkup_ParseAndBuildStringAsset:
    tokenStart = markupCursor;
    codeUnit = (ushort)*tokenStart;
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
      goto switchD_0041c155_caseD_0;
    case 9:
    case 10:
      goto RichTextMarkup_ParseAndBuildStringAsset;
    case 0xd:
      if (insideTagOrUnderflow) {
        capacityUnderflow = remainingCapacityBytes < 2;
        remainingCapacityBytes = remainingCapacityBytes - 2;
        if (capacityUnderflow || remainingCapacityBytes == 0)
        goto RichTextMarkup_ParseAndBuildStringAsset_FreePrimaryBufferAndReturnCapacityError;
        *outputCursor = L'耒';
        outputCursor = outputCursor + 1;
      }
      goto RichTextMarkup_ParseAndBuildStringAsset;
    default:
switchD_0041c155_caseD_23:
      if (insideTagOrUnderflow) {
        capacityUnderflow = remainingCapacityBytes < 2;
        remainingCapacityBytes = remainingCapacityBytes - 2;
        if (capacityUnderflow || remainingCapacityBytes == 0)
        goto RichTextMarkup_ParseAndBuildStringAsset_FreePrimaryBufferAndReturnCapacityError;
        *outputCursor = codeUnit + codeUnitBias;
        outputCursor = outputCursor + 1;
      }
      goto RichTextMarkup_ParseAndBuildStringAsset;
    case 0x23:
      markupByte = *markupCursor;
      codeUnit = (ushort)markupByte;
      markupCursor = tokenStart + 2;
      switch(markupByte) {
      default:
        goto switchD_0041c155_caseD_0;
      case 10:
      case 0xd:
        while (markupByte = *markupCursor, markupByte < 0x20) {
          markupCursor = markupCursor + 1;
          if ((markupByte != 10) && (markupByte != 0xd)) goto switchD_0041c155_caseD_0;
        }
        break;
      case 0x21:
        codeUnitBias = 0x7fc0;
        break;
      case 0x23:
        goto switchD_0041c155_caseD_23;
      case 0x2d:
        capacityUnderflow = remainingCapacityBytes < 2;
        remainingCapacityBytes = remainingCapacityBytes - 2;
        if (capacityUnderflow || remainingCapacityBytes == 0)
        goto RichTextMarkup_ParseAndBuildStringAsset_FreePrimaryBufferAndReturnCapacityError;
        *outputCursor = L'耑';
        outputCursor = outputCursor + 1;
        break;
      case 0x2e:
        if ((tagCount != 0) && (!insideTagOrUnderflow)) {
          tagCount = tagCount + 1;
          dStackY_3c = 0x41c70a;
          shrinkResult = (*g_MemoryApi.shrinkInPlace)((int)outputCursor - (int)memory,memory);
          if (!shrinkResult.carry) {
            largestBlock = (*g_MemoryApi.allocLargestFreeBlock)();
            stringAsset.assetOrError = (int *)largestBlock.allocationOrError;
            if (!largestBlock.carry) {
              remainingCapacityBytes = largestBlock.blockSizeOrSentinel - 0x200;
              if (0x1ff < largestBlock.blockSizeOrSentinel && remainingCapacityBytes != 0) {
                assetGroupCount = 0;
                assetWriteCursor = stringAsset.assetOrError;
                for (groupKeyOrIndex = 0x80; entryIndexOrOffset = tagCount, groupKeyOrIndex != 0; groupKeyOrIndex = groupKeyOrIndex + -1) {
                  *assetWriteCursor = 0;
                  assetWriteCursor = assetWriteCursor + 1;
                }
                while( true ) {
                  while (groupHeader = assetWriteCursor, groupKeyOrIndex = *(int *)(&thandor_stack_frame[0x80 - 0x30] + entryIndexOrOffset * 8),
                        groupKeyOrIndex == -1) {
                    entryIndexOrOffset = entryIndexOrOffset + -1;
                    assetWriteCursor = groupHeader;
                    if (entryIndexOrOffset == 0) {
                      (*g_MemoryApi.free)(memory);
                      assetSizeOrTimestamp = (int)groupHeader - (int)stringAsset.assetOrError;
                      (*g_MemoryApi.shrinkInPlace)(assetSizeOrTimestamp,stringAsset.assetOrError);
                      tagCount = tagCount * 8;
                      *(dword *)(&thandor_stack_frame[0x80 - 0x2c] + tagCount) = assetSizeOrTimestamp;
                      ((int *)stringAsset.assetOrError)[0x2c] = assetGroupCount;
                      ((int *)stringAsset.assetOrError)[1] = assetSizeOrTimestamp;
                      *(int *)stringAsset.assetOrError = 0x727473;
                      ((int *)stringAsset.assetOrError)[2] = 1;
                      ((int *)stringAsset.assetOrError)[3] = 0;
                      stackSlot = &thandor_stack_frame[0x80 - 0x30] + tagCount;
                      *(undefined4 *)(&thandor_stack_frame[0x80 - 0x30] + tagCount) = 0x41c7b5;
                      assetSizeOrTimestamp = (*g_LocaleGetPackedCurrentTime)();
                      ((int *)stringAsset.assetOrError)[4] = assetSizeOrTimestamp;
                      ((int *)stringAsset.assetOrError)[6] = assetSizeOrTimestamp;
                      ((int *)stringAsset.assetOrError)[8] = assetSizeOrTimestamp;
                      callStackSlot = stackSlot + -4;
                      *(undefined4 *)(stackSlot + -4) = 0x41c7cd;
                      assetSizeOrTimestamp = (*g_LocaleGetPackedCurrentDate)();
                      ((int *)stringAsset.assetOrError)[5] = assetSizeOrTimestamp;
                      ((int *)stringAsset.assetOrError)[7] = assetSizeOrTimestamp;
                      ((int *)stringAsset.assetOrError)[9] = assetSizeOrTimestamp;
                      *(int **)(callStackSlot + -4) = (int *)stringAsset.assetOrError + 0xc;
                      *(undefined4 *)(callStackSlot + -8) = 0x41c7ec;
                      (*g_LocaleCopyDefaultComputerLabelUtf16)(*(word **)(callStackSlot + -4));
                      *(int **)(callStackSlot + -4) = (int *)stringAsset.assetOrError + 0x1c;
                      *(undefined4 *)(callStackSlot + -8) = 0x41c7f9;
                      (*g_LocaleCopyDefaultComputerLabelUtf16)(*(word **)(callStackSlot + -4));
                      stringAsset.carry = false;
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
                      groupHeader[1] = groupHeader[1] + 1;
                      spanSizeOrDwordCount = (entryEndOrIndex - *(int *)(&thandor_stack_frame[0x80 - 0x2c] + entryIndexOrOffset * 8)) + 4;
                      *groupHeader = *groupHeader + spanSizeOrDwordCount;
                      insideTagOrUnderflow = remainingCapacityBytes < spanSizeOrDwordCount;
                      remainingCapacityBytes = remainingCapacityBytes - spanSizeOrDwordCount;
                      if (insideTagOrUnderflow || remainingCapacityBytes == 0)
                      goto 
                      RichTextMarkup_ParseAndBuildStringAsset_FreeTemporaryExpansionBufferBeforeCapacityError
                      ;
                    }
                    entryIndexOrOffset = entryIndexOrOffset + -1;
                  } while (entryIndexOrOffset != 0);
                  offsetTableCursor = groupHeader + 4;
                  assetGroupCount = assetGroupCount + 1;
                  assetWriteCursor = offsetTableCursor + groupHeader[1];
                  entryEndOrIndex = tagCount;
                  do {
                    if (groupKeyOrIndex == *(int *)(&thandor_stack_frame[0x80 - 0x30] + entryEndOrIndex * 8)) {
                      *(undefined4 *)(&thandor_stack_frame[0x80 - 0x30] + entryEndOrIndex * 8) = 0xffffffff;
                      entryIndexOrOffset = (int)assetWriteCursor - (int)groupHeader;
                      copySource = *(int **)(&thandor_stack_frame[0x80 - 0x2c] + entryEndOrIndex * 8);
                      for (spanSizeOrDwordCount = (uint)(*(int *)(&thandor_stack_frame[0x80 - 0x34] + entryEndOrIndex * 8) -
                                         (int)*(int **)(&thandor_stack_frame[0x80 - 0x2c] + entryEndOrIndex * 8)) >> 2;
                          spanSizeOrDwordCount != 0; spanSizeOrDwordCount = spanSizeOrDwordCount - 1) {
                        *assetWriteCursor = *copySource;
                        copySource = copySource + 1;
                        assetWriteCursor = assetWriteCursor + 1;
                      }
                      *offsetTableCursor = entryIndexOrOffset;
                      offsetTableCursor = offsetTableCursor + 1;
                    }
                    entryEndOrIndex = entryEndOrIndex + -1;
                    entryIndexOrOffset = tagCount;
                  } while (entryEndOrIndex != 0);
                }
              }
RichTextMarkup_ParseAndBuildStringAsset_FreeTemporaryExpansionBufferBeforeCapacityError:
              (*g_MemoryApi.free)(stringAsset.assetOrError);
            }
          }
RichTextMarkup_ParseAndBuildStringAsset_FreePrimaryBufferAndReturnCapacityError:
          tagCount = tagCount * 8;
          *(wchar_t **)(&thandor_stack_frame[0x80 - 0x2c] + tagCount) = memory;
          *(undefined4 *)(&thandor_stack_frame[0x80 - 0x30] + tagCount) = 0x41c5e3;
          (*g_MemoryApi.free)(*(void **)(&thandor_stack_frame[0x80 - 0x2c] + tagCount));
          capacityErrorResult.carry = true;
          capacityErrorResult.assetOrError = (void *)0x14;
          return capacityErrorResult;
        }
        goto switchD_0041c155_caseD_0;
      case 0x30:
      case 0x31:
      case 0x32:
      case 0x33:
      case 0x34:
      case 0x35:
      case 0x36:
      case 0x37:
      case 0x38:
      case 0x39:
        if ((((*markupCursor < 0x30) || (0x39 < *markupCursor)) || (tokenStart[3] < 0x30)) || (0x39 < tokenStart[3])
           ) goto switchD_0041c155_caseD_0;
        markupCursor = tokenStart + 4;
        break;
      case 0x3c:
        if (insideTagOrUnderflow) goto switchD_0041c155_caseD_0;
        tagCount = tagCount + 1;
        insideTagOrUnderflow = true;
        break;
      case 0x3e:
        if (!insideTagOrUnderflow) goto switchD_0041c155_caseD_0;
        insideTagOrUnderflow = false;
        if (((uint)outputCursor & 2) == 0) {
          insideTagOrUnderflow = remainingCapacityBytes < 4;
          remainingCapacityBytes = remainingCapacityBytes - 4;
          if (insideTagOrUnderflow || remainingCapacityBytes == 0)
          goto RichTextMarkup_ParseAndBuildStringAsset_FreePrimaryBufferAndReturnCapacityError;
          outputCursor[0] = L'\0';
          outputCursor[1] = L'\0';
          outputCursor = outputCursor + 2;
          insideTagOrUnderflow = false;
        }
        else {
          capacityUnderflow = remainingCapacityBytes < 2;
          remainingCapacityBytes = remainingCapacityBytes - 2;
          if (capacityUnderflow || remainingCapacityBytes == 0)
          goto RichTextMarkup_ParseAndBuildStringAsset_FreePrimaryBufferAndReturnCapacityError;
          *outputCursor = L'\0';
          outputCursor = outputCursor + 1;
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
        codeUnitBias = codeUnit * 0x80 + -0x2000;
      }
      goto RichTextMarkup_ParseAndBuildStringAsset;
    }
  }
LAB_0041c5c0:
  errorMessageResult.carry = true;
  errorMessageResult.assetOrError = memory;
  return errorMessageResult;
switchD_0041c155_caseD_0:
  groupKeyOrIndex = tagCount * 8;
  *(wchar_t **)(&thandor_stack_frame[0x80 - 0x2c] + groupKeyOrIndex) = memory;
  *(undefined4 *)(&thandor_stack_frame[0x80 - 0x30] + groupKeyOrIndex) = 0x41c5a2;
  (*g_MemoryApi.free)(*(void **)(&thandor_stack_frame[0x80 - 0x2c] + groupKeyOrIndex));
  *(wchar_t **)(&thandor_stack_frame[0x80 - 0x2c] + groupKeyOrIndex) = u_error__TXT2STR__unknown_characte_0041afac + 0x26;
  *(int *)(&thandor_stack_frame[0x80 - 0x30] + groupKeyOrIndex) = (int)markupCursor - (int)markupBytes;
  *(undefined4 *)(&thandor_stack_frame[0x80 - 0x34] + groupKeyOrIndex) = 1;
  *(undefined4 *)(&thandor_stack_frame[0x80 - 0x38] + groupKeyOrIndex) = 10;
  (&dStackY_3c)[tagCount * 2] = 0;
  aWStackY_44[tagCount * 2 + 1] = 0x40;
  aWStackY_44[tagCount * 2] = 0x41c5bb;
  (*g_WideNumberFormatUtf16)
            (aWStackY_44[tagCount * 2 + 1],(&dStackY_3c)[tagCount * 2],
             *(dword *)(&thandor_stack_frame[0x80 - 0x38] + groupKeyOrIndex),*(dword *)(&thandor_stack_frame[0x80 - 0x34] + groupKeyOrIndex),
             *(sdword *)(&thandor_stack_frame[0x80 - 0x30] + groupKeyOrIndex),*(word **)(&thandor_stack_frame[0x80 - 0x2c] + groupKeyOrIndex));
  memory = u_error__TXT2STR__unknown_characte_0041afac;
  goto LAB_0041c5c0;
}


/* Address: 0x0041C8D0.
   Ownership: assets/text/richtext.
   Purpose: Copies a command stream into a bounded destination, follows tag 0x18/0x19 nested stream references,
   preserves fixed command records, and returns byte count. CF reports insufficient capacity. Typed parameters: p0
   capacityBytes→TextOutputCapacityBytes_V342. Calling convention, exact VariableStorage serialization, function
   body bytes, control flow, globals, locals, and executable data remain unchanged.
*/
RichTextCopyExpandedEaxCf5 __thandor_eax_cf_preserve_ecx_edx
RichTextCommandStream_CopyExpandedCf
          (TextOutputCapacityBytes capacityBytes,word *destination,word *source)

{
  ushort commandCodeUnit;
  int wordsRemaining;
  ushort *nextSource;
  ushort *destinationCursor;
  bool capacityUnderflow;
  RichTextCopyExpandedEaxCf5 successResult;
  RichTextCopyExpandedEaxCf5 errorResult;
  word *nestedReturnStack[RICHTEXT_NESTING_LIMIT]; /* the original's machine-stack chain */
  int nestedDepth;
  
  nestedDepth = 0;
  destinationCursor = destination;
  while( true ) {
    while( true ) {
      commandCodeUnit = *source;
      nextSource = source + 1;
      if (commandCodeUnit == 0) break;
      if ((short)commandCodeUnit < 0) {
        switch(commandCodeUnit & 0x1f) {
        default:
          capacityUnderflow = capacityBytes < 2;
          capacityBytes = capacityBytes - 2;
          if (capacityUnderflow || capacityBytes == 0)
          goto RichTextCommandStream_CopyExpanded_TerminateOutputAndReturnCapacityError;
          *destinationCursor = commandCodeUnit & 0x1f | 0x8000;
          destinationCursor = destinationCursor + 1;
          source = nextSource;
          break;
        case 6:
          capacityUnderflow = capacityBytes < 0x12;
          capacityBytes = capacityBytes - 0x12;
          if (capacityUnderflow || capacityBytes == 0)
          goto RichTextCommandStream_CopyExpanded_TerminateOutputAndReturnCapacityError;
          for (wordsRemaining = 9; wordsRemaining != 0; wordsRemaining = wordsRemaining + -1) {
            *destinationCursor = *source;
            source = source + 1;
            destinationCursor = destinationCursor + 1;
          }
          break;
        case 0x14:
        case 0x15:
        case 0x16:
          capacityUnderflow = capacityBytes < 6;
          capacityBytes = capacityBytes - 6;
          if (capacityUnderflow || capacityBytes == 0)
          goto RichTextCommandStream_CopyExpanded_TerminateOutputAndReturnCapacityError;
          for (wordsRemaining = 3; wordsRemaining != 0; wordsRemaining = wordsRemaining + -1) {
            *destinationCursor = *source;
            source = source + 1;
            destinationCursor = destinationCursor + 1;
          }
          break;
        case 0x18:
          if (nestedDepth == RICHTEXT_NESTING_LIMIT)
          goto RichTextCommandStream_CopyExpanded_TerminateOutputAndReturnCapacityError;
          nestedReturnStack[nestedDepth++] = nextSource;
          source = *(word **)nextSource;
          break;
        case 0x19:
          source = *(word **)nextSource;
          break;
        case 0x1a:
          capacityUnderflow = capacityBytes < 10;
          capacityBytes = capacityBytes - 10;
          if (capacityUnderflow || capacityBytes == 0)
          goto RichTextCommandStream_CopyExpanded_TerminateOutputAndReturnCapacityError;
          for (wordsRemaining = 5; wordsRemaining != 0; wordsRemaining = wordsRemaining + -1) {
            *destinationCursor = *source;
            source = source + 1;
            destinationCursor = destinationCursor + 1;
          }
        }
      }
      else {
        capacityUnderflow = capacityBytes < 2;
        capacityBytes = capacityBytes - 2;
        if (capacityUnderflow || capacityBytes == 0)
        goto RichTextCommandStream_CopyExpanded_TerminateOutputAndReturnCapacityError;
        *destinationCursor = commandCodeUnit;
        destinationCursor = destinationCursor + 1;
        source = nextSource;
      }
    }
    if (nestedDepth == 0) break;
    source = (ushort *)((byte *)nestedReturnStack[--nestedDepth] + 8);
  }
  if (1 < (int)capacityBytes) {
    *destinationCursor = 0;
    successResult.eax = (int)destinationCursor - (int)destination;
    successResult.carry = false;
    return successResult;
  }
RichTextCommandStream_CopyExpanded_TerminateOutputAndReturnCapacityError:
  destinationCursor[-1] = 0;
  errorResult.carry = true;
  errorResult.eax = 0x14;
  return errorResult;
}


/* Address: 0x0041CF30.
   Ownership: assets/text/richtext.
   Purpose: Measures a UTF-16 rich-text command stream, follows nested stream tags, applies font-selection
   commands, and includes embedded texture dimensions. EAX is width and EDX is maximum height. Typed parameters: p0
   packedStyle→UiPackedTextStyle_V301. Calling convention, parameter storage, body bytes, control flow, globals,
   locals, and executable data remain unchanged.
   Cross-module calls: FontGlyph_GetLogicalSizeActiveRegs [assets/text/resources].
*/
RichTextExtentRegs __thandor_eax_edx_cf_preserve_ecx
RichTextCommandStream_MeasureRegs(UiPackedTextStyle packedStyle,word *commandStream)

{
  /* Rewritten from the assembly (0x0041CF30-0x0041D0E0). Command 0x18 enters a nested stream and pushes
     the return position on the machine stack; its terminator pops it and resumes 8 bytes later.
     Ghidra turned that stack into a counter, so nested text was measured forever. */
  word *returnStack[RICHTEXT_NESTING_LIMIT];
  int nesting = 0;
  RichTextExtentRegs extent;
  FontGlyphSizeEaxEdxCf9 glyphSize;
  GraphicsTextureSizeEaxEdxCf9 textureSize;
  word *command;
  int value;

  extent.widthPixels = 0;
  extent.heightPixels = 0;
  g_ActiveFontIndex = packedStyle >> 0x18 & 7;
  for (;;) {
    command = commandStream;
    value = (int)(short)*command;
    commandStream = command + 1;
    if (value == 0) {
      if (nesting == 0) {
        return extent;
      }
      commandStream = (word *)((byte *)returnStack[--nesting] + 8);
      continue;
    }
    if (value > 0) {
      glyphSize = FontGlyph_GetLogicalSizeActiveRegs((GraphicsSubresourceIndex)value);
      goto accumulate_glyph;
    }
    switch (value & 0x1f) {
    case 6:
      commandStream = command + 9;
      break;
    case 8:
    case 9:
    case 10:
    case 0xb:
    case 0xc:
    case 0xd:
    case 0xe:
    case 0xf:
      g_ActiveFontIndex = value & 0xf;
      break;
    case 0x10:
      glyphSize = FontGlyph_GetLogicalSizeActiveRegs(0x20);
accumulate_glyph:
      extent.widthPixels = extent.widthPixels + glyphSize.width;
      if (extent.heightPixels < glyphSize.lineHeight) {
        extent.heightPixels = glyphSize.lineHeight;
      }
      break;
    case 0x12:
      return extent;
    case 0x14:
    case 0x15:
    case 0x16:
      commandStream = command + 3;
      break;
    case 0x18:
      if (nesting == RICHTEXT_NESTING_LIMIT) {
        return extent;
      }
      returnStack[nesting++] = commandStream;
      commandStream = *(word **)commandStream;
      break;
    case 0x19:
      commandStream = *(word **)commandStream;
      break;
    case 0x1a:
      textureSize = (*g_GraphicsTextureSourceGetLogicalSize)
                              (*(dword *)(command + 3),*(GraphicsTextureSourceAsset **)commandStream);
      extent.widthPixels = extent.widthPixels + textureSize.logicalWidthPixels;
      commandStream = command + 5;
      if (extent.heightPixels < textureSize.logicalHeightPixels) {
        extent.heightPixels = textureSize.logicalHeightPixels;
      }
      break;
    }
  }
}


/* Address: 0x0041D0F0.
   Ownership: assets/text/richtext.
   Purpose: Measures the next line in the flattened rich-text runtime buffer, wraps at spaces or soft hyphens,
   publishes the next word index, returns line height in EAX, and sets CF only when the stream has ended. Typed
   parameters: p0 maximumWidth→UiPixelExtent_V301. Calling convention, parameter storage, body bytes, control flow,
   globals, locals, and executable data remain unchanged.
   Cross-module calls: FontGlyph_GetLogicalSizeActiveRegs [assets/text/resources].
*/
RichTextLineAdvanceEaxCf5 __thandor_eax_cf_preserve_ecx_edx
RichTextCommandStream_MeasureNextWrappedLineCf(UiPixelExtent maximumWidth)

{
  byte *pendingWrapPoint;
  GraphicsSubresourceIndex glyphSubresource;
  uint lineWidth;
  uint maxLineHeight;
  byte *commandCursor;
  byte *readCursor;
  RichTextLineAdvanceEaxCf5 lineResult;
  FontGlyphSizeEaxEdxCf9 glyphSize;
  GraphicsTextureSizeEaxEdxCf9 imageSize;
  byte *wrapPoint;
  
  glyphSize = FontGlyph_GetLogicalSizeActiveRegs(0);
  lineWidth = 0;
  wrapPoint = (byte *)0x0;
  maxLineHeight = glyphSize.lineHeight;
  readCursor = g_FontRuntimeBuffer + g_RichTextRuntimeBufferUsedWords * 2;
  pendingWrapPoint = wrapPoint;
switchD_0041d140_caseD_0:
  while( true ) {
    wrapPoint = pendingWrapPoint;
    commandCursor = readCursor;
    glyphSubresource = (GraphicsSubresourceIndex)*(short *)commandCursor;
    readCursor = commandCursor + 2;
    if (glyphSubresource != 0x20) break;
    glyphSize = FontGlyph_GetLogicalSizeActiveRegs(0x20);
    if (maximumWidth < lineWidth)
    goto RichTextCommandStream_MeasureNextWrappedLine_CommitWrapBoundaryAndReturnHeight;
    lineWidth = lineWidth + glyphSize.width;
    pendingWrapPoint = readCursor;
  }
  if (glyphSubresource != 0) {
    pendingWrapPoint = wrapPoint;
    if ((int)glyphSubresource < 0) goto code_r0x0041d13d;
    goto RichTextCommandStream_MeasureNextWrappedLine_AccumulateGlyphExtent;
  }
  goto switchD_0041d140_caseD_12;
code_r0x0041d13d:
  switch(glyphSubresource & 0x1f) {
  case 6:
    readCursor = commandCursor + 0x12;
    break;
  case 8:
  case 9:
  case 10:
  case 0xb:
  case 0xc:
  case 0xd:
  case 0xe:
  case 0xf:
    g_ActiveFontIndex = glyphSubresource & 0xf;
    break;
  case 0x10:
    glyphSubresource = 0x20;
RichTextCommandStream_MeasureNextWrappedLine_AccumulateGlyphExtent:
    glyphSize = FontGlyph_GetLogicalSizeActiveRegs(glyphSubresource);
    lineWidth = lineWidth + glyphSize.width;
    if (maxLineHeight < glyphSize.lineHeight) {
      maxLineHeight = glyphSize.lineHeight;
    }
    break;
  case 0x11:
    glyphSize = FontGlyph_GetLogicalSizeActiveRegs(0x2d);
    pendingWrapPoint = readCursor;
    if (maximumWidth < glyphSize.width + lineWidth)
    goto RichTextCommandStream_MeasureNextWrappedLine_CommitWrapBoundaryAndReturnHeight;
    break;
  case 0x12:
switchD_0041d140_caseD_12:
    if (lineWidth <= maximumWidth) {
      wrapPoint = readCursor;
    }
RichTextCommandStream_MeasureNextWrappedLine_CommitWrapBoundaryAndReturnHeight:
    if (wrapPoint == (byte *)0x0) {
      wrapPoint = readCursor;
    }
    g_RichTextRuntimeBufferUsedWords = (uint)((int)wrapPoint - (int)g_FontRuntimeBuffer) >> 1;
    lineResult.carry = *(short *)(wrapPoint + -2) == 0;
    lineResult.lineAdvancePixels = maxLineHeight;
    return lineResult;
  case 0x1a:
    imageSize = (*g_GraphicsTextureSourceGetLogicalSize)
                      (*(dword *)(commandCursor + 6),*(GraphicsTextureSourceAsset **)readCursor);
    lineWidth = lineWidth + imageSize.logicalWidthPixels;
    readCursor = commandCursor + 10;
    if (maxLineHeight < imageSize.logicalHeightPixels) {
      maxLineHeight = imageSize.logicalHeightPixels;
    }
  }
  goto switchD_0041d140_caseD_0;
}


/* Address: 0x0041D9F0.
   Ownership: assets/text/richtext.
   Purpose: Measures and draws the next wrapped line from the flattened runtime buffer, handles soft hyphens,
   images, style, color, and font commands, publishes the next word index, and encodes more-lines versus end-of-
   stream through CF. Typed parameters: p0 clipTop→UiPixelCoordinate_V297, p1 clipLeft→UiPixelCoordinate_V297, p2
   clipBottom→UiPixelCoordinate_V297, p3 clipRight→UiPixelCoordinate_V297, p4 maximumWidth→UiPixelExtent_V301, p5
   drawY→UiPixelCoordinate_V297, p6 drawX→UiPixelCoordinate_V297. Calling convention, parameter storage, body
   bytes, control flow, globals, locals, and executable data remain unchanged.
   Cross-module calls: FontGlyph_GetLogicalSizeActiveRegs [assets/text/resources], FontGlyph_DrawVerticallyCentered
   [assets/text/resources].
*/
RichTextLineAdvanceEaxCf5 __thandor_eax_cf_preserve_ecx_edx
RichTextCommandStream_DrawNextWrappedLineCf
          (UiPixelCoordinate clipTop,UiPixelCoordinate clipLeft,UiPixelCoordinate clipBottom,
          UiPixelCoordinate clipRight,UiPixelExtent maximumWidth,UiPixelCoordinate drawY,
          UiPixelCoordinate drawX)

{
  int lineBottom;
  byte *pendingWrapPoint;
  GraphicsSubresourceIndex glyphSubresource;
  int glyphAdvance;
  dword fontIndexOrImageWidth;
  uint lineWidth;
  uint lineTop;
  byte *measureCommand;
  byte *scanCursor;
  byte *drawCursor;
  RichTextLineAdvanceEaxCf5 moreLinesResult;
  RichTextLineAdvanceEaxCf5 endResult;
  FontGlyphSizeEaxEdxCf9 glyphSize;
  GraphicsTextureSizeEaxEdxCf9 imageSize;
  byte *wrapPoint;
  
  glyphSize = FontGlyph_GetLogicalSizeActiveRegs(0);
  lineWidth = 0;
  drawCursor = g_FontRuntimeBuffer + g_RichTextRuntimeBufferUsedWords * 2;
  wrapPoint = (byte *)0x0;
  lineTop = glyphSize.lineHeight;
  scanCursor = drawCursor;
  fontIndexOrImageWidth = g_ActiveFontIndex;
  pendingWrapPoint = wrapPoint;
switchD_0041da40_caseD_0:
  while( true ) {
    wrapPoint = pendingWrapPoint;
    measureCommand = scanCursor;
    glyphSubresource = (GraphicsSubresourceIndex)*(short *)measureCommand;
    scanCursor = measureCommand + 2;
    if (glyphSubresource != 0x20) break;
    glyphSize = FontGlyph_GetLogicalSizeActiveRegs(0x20);
    if (maximumWidth < lineWidth)
    goto RichTextCommandStream_DrawNextWrappedLine_CommitWrapBoundaryAndBeginDrawing;
    lineWidth = lineWidth + glyphSize.width;
    pendingWrapPoint = scanCursor;
  }
  if (glyphSubresource != 0) {
    pendingWrapPoint = wrapPoint;
    if ((int)glyphSubresource < 0) goto code_r0x0041da3d;
    goto RichTextCommandStream_DrawNextWrappedLine_AccumulateGlyphExtent;
  }
  goto switchD_0041da40_caseD_12;
code_r0x0041da3d:
  switch(glyphSubresource & 0x1f) {
  case 6:
    scanCursor = measureCommand + 0x12;
    break;
  case 8:
  case 9:
  case 10:
  case 0xb:
  case 0xc:
  case 0xd:
  case 0xe:
  case 0xf:
    g_ActiveFontIndex = glyphSubresource & 0xf;
    break;
  case 0x10:
    glyphSubresource = 0x20;
RichTextCommandStream_DrawNextWrappedLine_AccumulateGlyphExtent:
    glyphSize = FontGlyph_GetLogicalSizeActiveRegs(glyphSubresource);
    lineWidth = lineWidth + glyphSize.width;
    if (lineTop < glyphSize.lineHeight) {
      lineTop = glyphSize.lineHeight;
    }
    break;
  case 0x11:
    glyphSize = FontGlyph_GetLogicalSizeActiveRegs(0x2d);
    pendingWrapPoint = scanCursor;
    if (maximumWidth < glyphSize.width + lineWidth)
    goto RichTextCommandStream_DrawNextWrappedLine_CommitWrapBoundaryAndBeginDrawing;
    break;
  case 0x12:
    goto switchD_0041da40_caseD_12;
  case 0x1a:
    imageSize = (*g_GraphicsTextureSourceGetLogicalSize)
                       (*(dword *)(measureCommand + 6),*(GraphicsTextureSourceAsset **)scanCursor);
    lineWidth = lineWidth + imageSize.logicalWidthPixels;
    scanCursor = measureCommand + 10;
    if (lineTop < imageSize.logicalHeightPixels) {
      lineTop = imageSize.logicalHeightPixels;
    }
  }
  goto switchD_0041da40_caseD_0;
switchD_0041da40_caseD_12:
  if (lineWidth <= maximumWidth) {
    wrapPoint = scanCursor;
  }
RichTextCommandStream_DrawNextWrappedLine_CommitWrapBoundaryAndBeginDrawing:
  g_ActiveFontIndex = fontIndexOrImageWidth;
  if (wrapPoint == (byte *)0x0) {
    wrapPoint = scanCursor;
  }
  lineBottom = drawY + lineTop;
switchD_0041dbd0_caseD_7:
  scanCursor = drawCursor;
  glyphSubresource = (GraphicsSubresourceIndex)*(short *)scanCursor;
  drawCursor = scanCursor + 2;
  if (glyphSubresource != 0x20) {
    if (glyphSubresource == 0) {
      g_RichTextRuntimeBufferUsedWords = (uint)((int)drawCursor - (int)g_FontRuntimeBuffer) >> 1;
      endResult.carry = true;
      endResult.lineAdvancePixels = lineTop;
      return endResult;
    }
    if (-1 < (int)glyphSubresource) goto RichTextCommandStream_DrawNextWrappedLine_DrawGlyphAndAdvanceX;
    goto code_r0x0041dbcd;
  }
  if (drawCursor < wrapPoint) goto RichTextCommandStream_DrawNextWrappedLine_DrawGlyphAndAdvanceX;
  goto switchD_0041dbd0_caseD_12;
code_r0x0041dbcd:
  switch(glyphSubresource & 0x1f) {
  case 0:
    g_RichTextCurrentColorArgb = g_RichTextColorPalette0Argb;
    g_RichTextCurrentShadowOffset = g_RichTextShadowOffsetPalette0;
    break;
  case 1:
    g_RichTextCurrentColorArgb = g_RichTextColorPalette1Argb;
    g_RichTextCurrentShadowOffset = g_RichTextShadowOffsetPalette1;
    break;
  case 2:
    g_RichTextCurrentColorArgb = g_RichTextColorPalette2Argb;
    g_RichTextCurrentShadowOffset = g_RichTextShadowOffsetPalette2;
    break;
  case 3:
    g_RichTextCurrentColorArgb = g_RichTextColorPalette3Argb;
    g_RichTextCurrentShadowOffset = g_RichTextShadowOffsetPalette3;
    break;
  case 4:
    g_RichTextSavedColorArgb = g_RichTextCurrentColorArgb;
    g_RichTextSavedShadowOffset = g_RichTextCurrentShadowOffset;
    break;
  case 5:
    g_RichTextCurrentShadowOffset = g_RichTextSavedShadowOffset;
    g_RichTextCurrentColorArgb = g_RichTextSavedColorArgb;
    break;
  case 6:
    g_RichTextCurrentColorArgb =
         (((((((scanCursor[4] & 0xf) << 0x18 | (uint)*drawCursor << 0x1c) >> 4 | (uint)scanCursor[8] << 0x1c) >>
             4 | (uint)scanCursor[6] << 0x1c) >> 4 | (uint)scanCursor[0xc] << 0x1c) >> 4 |
          (uint)scanCursor[10] << 0x1c) >> 4 | (uint)scanCursor[0x10] << 0x1c) >> 4 |
         (uint)scanCursor[0xe] << 0x1c;
    drawCursor = scanCursor + 0x12;
    break;
  case 8:
  case 9:
  case 10:
  case 0xb:
  case 0xc:
  case 0xd:
  case 0xe:
  case 0xf:
    g_ActiveFontIndex = glyphSubresource & 0xf;
    break;
  case 0x10:
    glyphSubresource = 0x20;
RichTextCommandStream_DrawNextWrappedLine_DrawGlyphAndAdvanceX:
    glyphAdvance = FontGlyph_DrawVerticallyCentered
                      (clipTop,clipLeft,clipBottom,clipRight,glyphSubresource,lineTop,lineBottom,drawX);
    drawX = drawX + glyphAdvance;
    break;
  case 0x11:
    if (wrapPoint <= drawCursor) {
      FontGlyph_DrawVerticallyCentered
                (clipTop,clipLeft,clipBottom,clipRight,0x2d,lineTop,lineBottom,drawX);
      goto switchD_0041dbd0_caseD_12;
    }
    break;
  case 0x12:
switchD_0041dbd0_caseD_12:
    g_RichTextRuntimeBufferUsedWords = (uint)((int)drawCursor - (int)g_FontRuntimeBuffer) >> 1;
    moreLinesResult.carry = false;
    moreLinesResult.lineAdvancePixels = lineTop;
    return moreLinesResult;
  case 0x1a:
    imageSize = (*g_GraphicsTextureSourceGetLogicalSize)
                       (*(dword *)(scanCursor + 6),*(GraphicsTextureSourceAsset **)drawCursor);
    fontIndexOrImageWidth = imageSize.logicalWidthPixels;
    (*g_GraphicsTextureSourceBlitSourceAlpha)
              (clipTop,clipLeft,clipBottom,clipRight,lineBottom - imageSize.logicalHeightPixels,drawX,
               *(dword *)(scanCursor + 6),*(GraphicsTextureSourceAsset **)drawCursor,g_FramebufferAccess);
    drawX = drawX + fontIndexOrImageWidth;
    drawCursor = scanCursor + 10;
  }
  goto switchD_0041dbd0_caseD_7;
}


/* Address: 0x0041D840.
   Ownership: assets/text/richtext.
   Purpose: Flattens nested rich-text command streams into the exact 0x2000-word runtime buffer, copies fixed color
   and image commands, follows nested stream pointers, writes a terminator, and resets the shared word index.
*/
void __thandor_void_preserve_eax_ecx_edx
RichTextCommandStream_FlattenNestedToRuntimeBuffer(word *commandStream)

{
  ushort commandCodeUnit;
  uint remainingWords;
  int nestedDepth;
  ushort *nestedReturnStack[RICHTEXT_NESTING_LIMIT]; /* the original's machine-stack chain */
  ushort *commandCursor;
  ushort *outputCursor;
  
  remainingWords = 0x2000;
  nestedDepth = 0;
  outputCursor = (ushort *)g_FontRuntimeBuffer;
switchD_0041d87b_caseD_7:
  while( true ) {
    commandCursor = commandStream;
    commandCodeUnit = *commandCursor;
    commandStream = commandCursor + 1;
    if (commandCodeUnit != 0) break;
    if (nestedDepth == 0) {
      *outputCursor = 0;
      g_RichTextRuntimeBufferUsedWords = 0;
      return;
    }
    nestedDepth = nestedDepth + -1;
    commandStream = (ushort *)((byte *)nestedReturnStack[nestedDepth] + 8);
  }
  if ((short)commandCodeUnit < 0) goto switchD_0041d87b_switchD;
  goto switchD_0041d87b_caseD_0;
switchD_0041d87b_switchD:
  switch(commandCodeUnit & 0x1f) {
  default:
switchD_0041d87b_caseD_0:
    if (remainingWords != 0) {
      *outputCursor = commandCodeUnit;
      remainingWords = remainingWords - 1;
      outputCursor = outputCursor + 1;
    }
    break;
  case 6:
    if (9 < remainingWords) {
      *outputCursor = commandCodeUnit;
      remainingWords = remainingWords - 9;
      *(undefined4 *)(outputCursor + 1) = *(undefined4 *)commandStream;
      *(undefined4 *)(outputCursor + 3) = *(undefined4 *)(commandCursor + 3);
      *(undefined4 *)(outputCursor + 5) = *(undefined4 *)(commandCursor + 5);
      *(undefined4 *)(outputCursor + 7) = *(undefined4 *)(commandCursor + 7);
      outputCursor = outputCursor + 9;
      commandStream = commandCursor + 9;
    }
    break;
  case 7:
  case 0x13:
  case 0x14:
  case 0x15:
  case 0x16:
  case 0x17:
  case 0x1b:
  case 0x1c:
  case 0x1d:
  case 0x1e:
  case 0x1f:
    break;
  case 0x18:
    if (nestedDepth == RICHTEXT_NESTING_LIMIT) {
      break;
    }
    nestedReturnStack[nestedDepth] = commandStream;
    nestedDepth = nestedDepth + 1;
  case 0x19:
    commandStream = *(ushort **)commandStream;
    break;
  case 0x1a:
    if (5 < remainingWords) {
      *outputCursor = commandCodeUnit;
      remainingWords = remainingWords - 5;
      *(undefined4 *)(outputCursor + 1) = *(undefined4 *)commandStream;
      *(undefined4 *)(outputCursor + 3) = *(undefined4 *)(commandCursor + 3);
      outputCursor = outputCursor + 5;
      commandStream = commandCursor + 5;
    }
  }
  goto switchD_0041d87b_caseD_7;
}


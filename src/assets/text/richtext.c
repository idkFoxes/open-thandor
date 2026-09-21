/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/src/assets/text/richtext.c
 * Reverse engineering by idkFoxes 2026
 */

#include <thandor/assets/text/richtext.h>

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
  uint uVar1;
  int iVar2;
  RichTextExtentRegs RVar3;
  RichTextLineAdvanceEaxCf5 RVar4;
  
  RichTextCommandStream_FlattenNestedToRuntimeBuffer(commandStream);
  uVar1 = packedStyle >> 0x10 & 7;
  g_ActiveFontIndex = packedStyle >> 0x18 & 7;
  g_RichTextCurrentColorArgb = (&g_RichTextColorPalette0Argb)[uVar1];
  g_RichTextCurrentShadowOffset = (&g_RichTextShadowOffsetPalette0)[uVar1];
  iVar2 = 0;
  g_RichTextSavedColorArgb = g_RichTextCurrentColorArgb;
  g_RichTextSavedShadowOffset = g_RichTextCurrentShadowOffset;
  while( true ) {
    RVar4 = RichTextCommandStream_MeasureNextWrappedLineCf(maximumWidth);
    if (RVar4.carry) break;
    iVar2 = iVar2 + RVar4.lineAdvancePixels;
  }
  RVar3.heightPixels = iVar2 + RVar4.lineAdvancePixels;
  RVar3.widthPixels = maximumWidth;
  return RVar3;
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
  uint uVar1;
  RichTextLineAdvanceEaxCf5 RVar2;
  
  RichTextCommandStream_FlattenNestedToRuntimeBuffer(commandStream);
  uVar1 = packedStyle >> 0x10 & 7;
  g_ActiveFontIndex = packedStyle >> 0x18 & 7;
  g_RichTextCurrentColorArgb = (&g_RichTextColorPalette0Argb)[uVar1];
  g_RichTextCurrentShadowOffset = (&g_RichTextShadowOffsetPalette0)[uVar1];
  g_RichTextSavedColorArgb = g_RichTextCurrentColorArgb;
  g_RichTextSavedShadowOffset = g_RichTextCurrentShadowOffset;
  while( true ) {
    RVar2 = RichTextCommandStream_DrawNextWrappedLineCf
                      (clipTop,clipLeft,clipBottom,clipRight,maximumWidth,drawY,drawX);
    if (RVar2.carry) break;
    drawY = drawY + RVar2.lineAdvancePixels;
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
          UiPixelCoordinate drawX,UiPixelCoordinate baselineY)

{
  int baselineY_00;
  uint uVar1;
  GraphicsSubresourceIndex glyphSubresource;
  int iVar2;
  dword dVar3;
  word *pwVar4;
  RichTextExtentRegs RVar5;
  GraphicsTextureSizeEaxEdxCf9 GVar6;
  byte *local_1c;
  
  RVar5 = RichTextCommandStream_MeasureRegs(packedStyle,commandStream);
  uVar1 = RVar5.widthPixels;
  baselineY_00 = drawX + RVar5.heightPixels;
  if ((packedStyle & 1) == 0) {
    if ((packedStyle & 2) == 0)
    goto RichTextCommandStream_DrawSingleLine_InitializeStyleAndBeginDrawing;
    uVar1 = uVar1 >> 1;
  }
  baselineY = baselineY - uVar1;
RichTextCommandStream_DrawSingleLine_InitializeStyleAndBeginDrawing:
  uVar1 = packedStyle >> 0x10 & 7;
  g_ActiveFontIndex = packedStyle >> 0x18 & 7;
  g_RichTextCurrentColorArgb = (&g_RichTextColorPalette0Argb)[uVar1];
  g_RichTextCurrentShadowOffset = (&g_RichTextShadowOffsetPalette0)[uVar1];
  local_1c = (byte *)0x0;
  g_RichTextSavedColorArgb = g_RichTextCurrentColorArgb;
  g_RichTextSavedShadowOffset = g_RichTextCurrentShadowOffset;
switchD_0041d537_caseD_7:
  while( true ) {
    pwVar4 = commandStream;
    glyphSubresource = (GraphicsSubresourceIndex)(short)*pwVar4;
    commandStream = pwVar4 + 1;
    if (glyphSubresource != 0) break;
    if (local_1c == (byte *)0x0) {
      return false;
    }
    commandStream = (word *)(local_1c + 8);
    local_1c = local_1c + -1;
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
         ((((((((byte)pwVar4[2] & 0xf) << 0x18 | (uint)(byte)*commandStream << 0x1c) >> 4 |
             (uint)(byte)pwVar4[4] << 0x1c) >> 4 | (uint)(byte)pwVar4[3] << 0x1c) >> 4 |
           (uint)(byte)pwVar4[6] << 0x1c) >> 4 | (uint)(byte)pwVar4[5] << 0x1c) >> 4 |
         (uint)(byte)pwVar4[8] << 0x1c) >> 4 | (uint)(byte)pwVar4[7] << 0x1c;
    commandStream = pwVar4 + 9;
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
    iVar2 = FontGlyph_DrawBottomAligned
                      (clipTop,clipLeft,clipBottom,clipRight,glyphSubresource,baselineY_00,baselineY
                      );
    baselineY = baselineY + iVar2;
    break;
  case 0x12:
    return false;
  case 0x14:
  case 0x15:
  case 0x16:
    commandStream = pwVar4 + 3;
    break;
  case 0x18:
    local_1c = (byte *)((int)pwVar4 + 3);
    commandStream = *(word **)commandStream;
    break;
  case 0x19:
    commandStream = *(word **)commandStream;
    break;
  case 0x1a:
    GVar6 = (*g_GraphicsTextureSourceGetLogicalSize)
                      (*(dword *)(pwVar4 + 3),*(GraphicsTextureSourceAsset **)commandStream);
    dVar3 = GVar6.logicalWidthPixels;
    (*g_GraphicsTextureSourceBlitSourceAlpha)
              (clipTop,clipLeft,clipBottom,clipRight,baselineY_00 - GVar6.logicalHeightPixels,
               baselineY,*(dword *)(pwVar4 + 3),*(GraphicsTextureSourceAsset **)commandStream,
               g_FramebufferAccess);
    baselineY = baselineY + dVar3;
    commandStream = pwVar4 + 5;
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
  ushort uVar1;
  ushort *puVar2;
  
  while( true ) {
    puVar2 = stream;
    uVar1 = *puVar2;
    if (uVar1 == 0) break;
    stream = puVar2 + 1;
    if ((short)uVar1 < 0) {
      switch(uVar1 & 0x1f) {
      case 6:
        stream = puVar2 + 9;
        break;
      case 0x14:
      case 0x15:
      case 0x16:
        stream = puVar2 + 3;
        break;
      case 0x18:
      case 0x19:
        stream = puVar2 + 5;
        if (selector == *(int *)(puVar2 + 3)) {
          *(void **)(puVar2 + 1) = replacementPayload;
        }
        break;
      case 0x1a:
        stream = puVar2 + 5;
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
  word *pwVar1;
  ushort commandCodeUnit;
  
  pwVar1 = stream;
  while( true ) {
    commandCursor = pwVar1;
    commandCodeUnit = *commandCursor;
    pwVar1 = commandCursor + 1;
    if (commandCodeUnit == 0) break;
    if ((short)commandCodeUnit < 0) {
      switch(commandCodeUnit & 0x1f) {
      case 6:
        pwVar1 = commandCursor + 9;
        break;
      case 0x14:
      case 0x15:
      case 0x16:
        pwVar1 = commandCursor + 3;
        break;
      case 0x18:
      case 0x19:
        pwVar1 = commandCursor + 5;
        break;
      case 0x1a:
        *(GraphicsTextureSourceAsset **)pwVar1 = textureSource;
        pwVar1 = commandCursor + 5;
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
  ushort uVar1;
  int iVar2;
  ushort *puVar3;
  
  iVar2 = commandOrdinal + 1;
  do {
    do {
      puVar3 = commandStream;
      uVar1 = *puVar3;
      if (uVar1 == 0) {
        return true;
      }
      commandStream = puVar3 + 1;
    } while (-1 < (short)uVar1);
    switch(uVar1 & 0x1f) {
    case 6:
      commandStream = puVar3 + 9;
      break;
    case 0x14:
    case 0x15:
    case 0x16:
      iVar2 = iVar2 + -1;
      commandStream = puVar3 + 3;
      if (iVar2 == 0) {
        *(RichTextCommandPayload32 *)(puVar3 + 1) = payloadValue;
        return false;
      }
      break;
    case 0x18:
    case 0x19:
    case 0x1a:
      commandStream = puVar3 + 5;
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
  RichTextNestedStreamPointerValue32 *pRVar1;
  ushort commandCodeUnit;
  
  pRVar1 = (RichTextNestedStreamPointerValue32 *)commandStream;
  while( true ) {
    commandCursor = (ushort *)pRVar1;
    commandCodeUnit = *commandCursor;
    pRVar1 = (RichTextNestedStreamPointerValue32 *)(commandCursor + 1);
    if (commandCodeUnit == 0) break;
    if ((short)commandCodeUnit < 0) {
      switch(commandCodeUnit & 0x1f) {
      case 6:
        pRVar1 = (RichTextNestedStreamPointerValue32 *)(commandCursor + 9);
        break;
      case 0x14:
      case 0x15:
      case 0x16:
        pRVar1 = (RichTextNestedStreamPointerValue32 *)(commandCursor + 3);
        break;
      case 0x18:
      case 0x19:
        *pRVar1 = nestedStreamPointerValue;
        pRVar1 = (RichTextNestedStreamPointerValue32 *)(commandCursor + 5);
        break;
      case 0x1a:
        pRVar1 = (RichTextNestedStreamPointerValue32 *)(commandCursor + 5);
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
  RichTextCommandPayload32 *pRVar1;
  ushort commandCodeUnit;
  
  pRVar1 = (RichTextCommandPayload32 *)commandStream;
  while( true ) {
    commandCursor = (ushort *)pRVar1;
    commandCodeUnit = *commandCursor;
    pRVar1 = (RichTextCommandPayload32 *)(commandCursor + 1);
    if (commandCodeUnit == 0) break;
    if ((short)commandCodeUnit < 0) {
      switch(commandCodeUnit & 0x1f) {
      case 6:
        pRVar1 = (RichTextCommandPayload32 *)(commandCursor + 9);
        break;
      case 0x14:
      case 0x15:
      case 0x16:
        pRVar1 = (RichTextCommandPayload32 *)(commandCursor + 3);
        break;
      case 0x18:
      case 0x19:
        pRVar1 = (RichTextCommandPayload32 *)(commandCursor + 5);
        break;
      case 0x1a:
        *pRVar1 = leadingPayloadValue;
        *(RichTextOpcode1APayloadValue32 *)(commandCursor + 3) = opcode1APayloadValue;
        pRVar1 = (RichTextCommandPayload32 *)(commandCursor + 5);
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
  RichTextInlinePayloadValue32 *pRVar1;
  ushort commandCodeUnit;
  
  pRVar1 = (RichTextInlinePayloadValue32 *)commandStream;
  while( true ) {
    commandCursor = (ushort *)pRVar1;
    commandCodeUnit = *commandCursor;
    pRVar1 = (RichTextInlinePayloadValue32 *)(commandCursor + 1);
    if (commandCodeUnit == 0) break;
    if ((short)commandCodeUnit < 0) {
      switch(commandCodeUnit & 0x1f) {
      case 6:
        pRVar1 = (RichTextInlinePayloadValue32 *)(commandCursor + 9);
        break;
      case 0x14:
      case 0x15:
      case 0x16:
        *pRVar1 = inlinePayloadValue;
        pRVar1 = (RichTextInlinePayloadValue32 *)(commandCursor + 3);
        break;
      case 0x18:
      case 0x19:
      case 0x1a:
        pRVar1 = (RichTextInlinePayloadValue32 *)(commandCursor + 5);
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
  uint *puVar1;
  int iVar2;
  uint *commandCursor;
  ushort commandCodeUnit;
  
  iVar2 = commandOrdinal + 1;
  puVar1 = commandStream;
  do {
    do {
      commandCursor = puVar1;
      commandCodeUnit = (ushort)*commandCursor;
      if (commandCodeUnit == 0) {
        return true;
      }
      puVar1 = (uint *)((int)commandCursor + 2);
    } while (-1 < (short)commandCodeUnit);
    switch(commandCodeUnit & 0x1f) {
    case 6:
      puVar1 = (uint *)((int)commandCursor + 0x12);
      break;
    case 0x14:
    case 0x15:
    case 0x16:
      iVar2 = iVar2 + -1;
      puVar1 = (uint *)((int)commandCursor + 6);
      if (iVar2 == 0) {
        *commandCursor = *commandCursor & 0xffff8014;
        *commandCursor = *commandCursor | flagBits;
        return false;
      }
      break;
    case 0x18:
    case 0x19:
    case 0x1a:
      puVar1 = (uint *)((int)commandCursor + 10);
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
  ushort uVar1;
  int iVar2;
  uint *puVar3;
  StatusValueEaxCf5 SVar4;
  StatusValueEaxCf5 SVar5;
  
  iVar2 = commandOrdinal + 1;
  do {
    do {
      puVar3 = (uint *)commandStream;
      uVar1 = (ushort)*puVar3;
      if (uVar1 == 0) {
        SVar5.valueOrError = 0;
        SVar5.carry = true;
        return SVar5;
      }
      commandStream = (ushort *)((int)puVar3 + 2);
    } while (-1 < (short)uVar1);
    switch(uVar1 & 0x1f) {
    case 6:
      commandStream = (ushort *)((int)puVar3 + 0x12);
      break;
    case 0x14:
    case 0x15:
    case 0x16:
      iVar2 = iVar2 + -1;
      commandStream = (ushort *)((int)puVar3 + 6);
      if (iVar2 == 0) {
        SVar4.valueOrError = *puVar3 & 3;
        SVar4.carry = false;
        return SVar4;
      }
      break;
    case 0x18:
    case 0x19:
    case 0x1a:
      commandStream = (ushort *)((int)puVar3 + 10);
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
  word *pwVar1;
  dword remainingCapacityBytes;
  ushort *commandCursor;
  word *streamCursor;
  bool newlineCapacityUnderflow;
  StatusValueEaxCf5 SVar2;
  StatusValueEaxCf5 SVar3;
  int nestedReturnCursor;
  ushort commandOrCodeUnit;
  
  nestedReturnCursor = 0;
  remainingCapacityBytes = capacityBytes;
  pwVar1 = source;
  while( true ) {
    while( true ) {
      commandCursor = pwVar1;
      commandOrCodeUnit = *commandCursor;
      streamCursor = commandCursor + 1;
      if (commandOrCodeUnit == 0) break;
      if ((short)commandOrCodeUnit < 0) {
        pwVar1 = streamCursor;
        switch(commandOrCodeUnit & 0x1f) {
        case 6:
          pwVar1 = commandCursor + 9;
          break;
        case 0x10:
          remainingCapacityBytes = remainingCapacityBytes - 1;
          if (remainingCapacityBytes == 0)
          goto RichTextCommandStream_CopyToNarrow_TerminateOutputAndReturnCapacityError;
          *destination = 0x20;
          destination = destination + 1;
          pwVar1 = streamCursor;
          break;
        case 0x12:
          newlineCapacityUnderflow = remainingCapacityBytes < 2;
          remainingCapacityBytes = remainingCapacityBytes - 2;
          if (newlineCapacityUnderflow || remainingCapacityBytes == 0)
          goto RichTextCommandStream_CopyToNarrow_TerminateOutputAndReturnCapacityError;
          destination[0] = 0xd;
          destination[1] = 10;
          destination = destination + 2;
          pwVar1 = streamCursor;
          break;
        case 0x14:
        case 0x15:
        case 0x16:
          pwVar1 = commandCursor + 3;
          break;
        case 0x18:
          nestedReturnCursor = (int)commandCursor + 3;
          pwVar1 = *(ushort **)streamCursor;
          break;
        case 0x19:
          pwVar1 = *(ushort **)streamCursor;
          break;
        case 0x1a:
          pwVar1 = commandCursor + 5;
        }
      }
      else {
        pwVar1 = streamCursor;
        if ((commandOrCodeUnit & 0xff00) == 0) {
          remainingCapacityBytes = remainingCapacityBytes - 1;
          if (remainingCapacityBytes == 0)
          goto RichTextCommandStream_CopyToNarrow_TerminateOutputAndReturnCapacityError;
          *destination = (byte)commandOrCodeUnit;
          destination = destination + 1;
          pwVar1 = streamCursor;
        }
      }
    }
    if (nestedReturnCursor == 0) break;
    pwVar1 = (ushort *)(nestedReturnCursor + 8);
    nestedReturnCursor = nestedReturnCursor + -1;
  }
  if (0 < (int)remainingCapacityBytes) {
    *destination = 0;
    SVar2.valueOrError = capacityBytes - (remainingCapacityBytes - 1);
    SVar2.carry = false;
    return SVar2;
  }
RichTextCommandStream_CopyToNarrow_TerminateOutputAndReturnCapacityError:
  destination[-1] = 0;
  SVar3.carry = true;
  SVar3.valueOrError = 0x14;
  return SVar3;
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
  byte bVar1;
  ushort uVar2;
  wchar_t *memory;
  uint uVar3;
  uint uVar4;
  int iVar5;
  int *piVar6;
  int iVar7;
  int iVar8;
  short sVar9;
  undefined1 *puVar10;
  undefined1 *puVar11;
  byte *pbVar12;
  byte *pbVar13;
  int *piVar14;
  wchar_t *pwVar15;
  int *piVar16;
  dword dVar17;
  int *piVar18;
  bool bVar19;
  bool bVar20;
  RichTextStringAssetEaxCf5 RVar21;
  RichTextStringAssetEaxCf5 RVar22;
  ArenaShrinkEaxCf5 AVar23;
  RichTextStringAssetEaxCf5 RVar24;
  ArenaLargestAllocationEaxEcxCf9 AVar25;
  WideNumberFormatFlags aWStackY_44 [2];
  dword dStackY_3c;
  int iStack_24;
  int iStack_20;
  
  AVar25 = (*g_MemoryApi.allocLargestFreeBlock)();
  uVar4 = AVar25.blockSizeOrSentinel;
  memory = (wchar_t *)AVar25.allocationOrError;
  if (!AVar25.carry) {
    sVar9 = 0;
    iStack_20 = 0;
    pbVar13 = markupBytes;
    pwVar15 = memory;
    bVar20 = false;
RichTextMarkup_ParseAndBuildStringAsset:
    pbVar12 = pbVar13;
    uVar2 = (ushort)*pbVar12;
    pbVar13 = pbVar12 + 1;
    switch(*pbVar12) {
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
      if (bVar20) {
        bVar19 = uVar4 < 2;
        uVar4 = uVar4 - 2;
        if (bVar19 || uVar4 == 0)
        goto RichTextMarkup_ParseAndBuildStringAsset_FreePrimaryBufferAndReturnCapacityError;
        *pwVar15 = L'耒';
        pwVar15 = pwVar15 + 1;
      }
      goto RichTextMarkup_ParseAndBuildStringAsset;
    default:
switchD_0041c155_caseD_23:
      if (bVar20) {
        bVar19 = uVar4 < 2;
        uVar4 = uVar4 - 2;
        if (bVar19 || uVar4 == 0)
        goto RichTextMarkup_ParseAndBuildStringAsset_FreePrimaryBufferAndReturnCapacityError;
        *pwVar15 = uVar2 + sVar9;
        pwVar15 = pwVar15 + 1;
      }
      goto RichTextMarkup_ParseAndBuildStringAsset;
    case 0x23:
      bVar1 = *pbVar13;
      uVar2 = (ushort)bVar1;
      pbVar13 = pbVar12 + 2;
      switch(bVar1) {
      default:
        goto switchD_0041c155_caseD_0;
      case 10:
      case 0xd:
        while (bVar1 = *pbVar13, bVar1 < 0x20) {
          pbVar13 = pbVar13 + 1;
          if ((bVar1 != 10) && (bVar1 != 0xd)) goto switchD_0041c155_caseD_0;
        }
        break;
      case 0x21:
        sVar9 = 0x7fc0;
        break;
      case 0x23:
        goto switchD_0041c155_caseD_23;
      case 0x2d:
        bVar19 = uVar4 < 2;
        uVar4 = uVar4 - 2;
        if (bVar19 || uVar4 == 0)
        goto RichTextMarkup_ParseAndBuildStringAsset_FreePrimaryBufferAndReturnCapacityError;
        *pwVar15 = L'耑';
        pwVar15 = pwVar15 + 1;
        break;
      case 0x2e:
        if ((iStack_20 != 0) && (!bVar20)) {
          iStack_20 = iStack_20 + 1;
          dStackY_3c = 0x41c70a;
          AVar23 = (*g_MemoryApi.shrinkInPlace)((int)pwVar15 - (int)memory,memory);
          if (!AVar23.carry) {
            AVar25 = (*g_MemoryApi.allocLargestFreeBlock)();
            RVar24.assetOrError = (int *)AVar25.allocationOrError;
            if (!AVar25.carry) {
              uVar4 = AVar25.blockSizeOrSentinel - 0x200;
              if (0x1ff < AVar25.blockSizeOrSentinel && uVar4 != 0) {
                iStack_24 = 0;
                piVar18 = RVar24.assetOrError;
                for (iVar5 = 0x80; iVar7 = iStack_20, iVar5 != 0; iVar5 = iVar5 + -1) {
                  *piVar18 = 0;
                  piVar18 = piVar18 + 1;
                }
                while( true ) {
                  while (piVar16 = piVar18, iVar5 = *(int *)(&stack0xffffffd0 + iVar7 * 8),
                        iVar5 == -1) {
                    iVar7 = iVar7 + -1;
                    piVar18 = piVar16;
                    if (iVar7 == 0) {
                      (*g_MemoryApi.free)(memory);
                      dVar17 = (int)piVar16 - (int)RVar24.assetOrError;
                      (*g_MemoryApi.shrinkInPlace)(dVar17,RVar24.assetOrError);
                      iStack_20 = iStack_20 * 8;
                      *(dword *)(&stack0xffffffd4 + iStack_20) = dVar17;
                      RVar24.assetOrError[0x2c] = iStack_24;
                      RVar24.assetOrError[1] = dVar17;
                      *RVar24.assetOrError = 0x727473;
                      RVar24.assetOrError[2] = 1;
                      RVar24.assetOrError[3] = 0;
                      puVar10 = &stack0xffffffd0 + iStack_20;
                      *(undefined4 *)(&stack0xffffffd0 + iStack_20) = 0x41c7b5;
                      dVar17 = (*g_LocaleGetPackedCurrentTime)();
                      RVar24.assetOrError[4] = dVar17;
                      RVar24.assetOrError[6] = dVar17;
                      RVar24.assetOrError[8] = dVar17;
                      puVar11 = puVar10 + -4;
                      *(undefined4 *)(puVar10 + -4) = 0x41c7cd;
                      dVar17 = (*g_LocaleGetPackedCurrentDate)();
                      RVar24.assetOrError[5] = dVar17;
                      RVar24.assetOrError[7] = dVar17;
                      RVar24.assetOrError[9] = dVar17;
                      *(int **)(puVar11 + -4) = RVar24.assetOrError + 0xc;
                      *(undefined4 *)(puVar11 + -8) = 0x41c7ec;
                      (*g_LocaleCopyDefaultComputerLabelUtf16)(*(word **)(puVar11 + -4));
                      *(int **)(puVar11 + -4) = RVar24.assetOrError + 0x1c;
                      *(undefined4 *)(puVar11 + -8) = 0x41c7f9;
                      (*g_LocaleCopyDefaultComputerLabelUtf16)(*(word **)(puVar11 + -4));
                      RVar24.carry = false;
                      return RVar24;
                    }
                  }
                  bVar20 = uVar4 < 0x10;
                  uVar4 = uVar4 - 0x10;
                  if (bVar20 || uVar4 == 0) break;
                  piVar16[2] = iVar5;
                  *piVar16 = 0x10;
                  piVar16[1] = 0;
                  do {
                    if (iVar5 == *(int *)(&stack0xffffffd0 + iVar7 * 8)) {
                      iVar8 = *(int *)(&stack0xffffffcc + iVar7 * 8);
                      piVar16[1] = piVar16[1] + 1;
                      uVar3 = (iVar8 - *(int *)(&stack0xffffffd4 + iVar7 * 8)) + 4;
                      *piVar16 = *piVar16 + uVar3;
                      bVar20 = uVar4 < uVar3;
                      uVar4 = uVar4 - uVar3;
                      if (bVar20 || uVar4 == 0)
                      goto 
                      RichTextMarkup_ParseAndBuildStringAsset_FreeTemporaryExpansionBufferBeforeCapacityError
                      ;
                    }
                    iVar7 = iVar7 + -1;
                  } while (iVar7 != 0);
                  piVar6 = piVar16 + 4;
                  iStack_24 = iStack_24 + 1;
                  piVar18 = piVar6 + piVar16[1];
                  iVar8 = iStack_20;
                  do {
                    if (iVar5 == *(int *)(&stack0xffffffd0 + iVar8 * 8)) {
                      *(undefined4 *)(&stack0xffffffd0 + iVar8 * 8) = 0xffffffff;
                      iVar7 = (int)piVar18 - (int)piVar16;
                      piVar14 = *(int **)(&stack0xffffffd4 + iVar8 * 8);
                      for (uVar3 = (uint)(*(int *)(&stack0xffffffcc + iVar8 * 8) -
                                         (int)*(int **)(&stack0xffffffd4 + iVar8 * 8)) >> 2;
                          uVar3 != 0; uVar3 = uVar3 - 1) {
                        *piVar18 = *piVar14;
                        piVar14 = piVar14 + 1;
                        piVar18 = piVar18 + 1;
                      }
                      *piVar6 = iVar7;
                      piVar6 = piVar6 + 1;
                    }
                    iVar8 = iVar8 + -1;
                    iVar7 = iStack_20;
                  } while (iVar8 != 0);
                }
              }
RichTextMarkup_ParseAndBuildStringAsset_FreeTemporaryExpansionBufferBeforeCapacityError:
              (*g_MemoryApi.free)(RVar24.assetOrError);
            }
          }
RichTextMarkup_ParseAndBuildStringAsset_FreePrimaryBufferAndReturnCapacityError:
          iStack_20 = iStack_20 * 8;
          *(wchar_t **)(&stack0xffffffd4 + iStack_20) = memory;
          *(undefined4 *)(&stack0xffffffd0 + iStack_20) = 0x41c5e3;
          (*g_MemoryApi.free)(*(void **)(&stack0xffffffd4 + iStack_20));
          RVar22.carry = true;
          RVar22.assetOrError = (void *)0x14;
          return RVar22;
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
        if ((((*pbVar13 < 0x30) || (0x39 < *pbVar13)) || (pbVar12[3] < 0x30)) || (0x39 < pbVar12[3])
           ) goto switchD_0041c155_caseD_0;
        pbVar13 = pbVar12 + 4;
        break;
      case 0x3c:
        if (bVar20) goto switchD_0041c155_caseD_0;
        iStack_20 = iStack_20 + 1;
        bVar20 = true;
        break;
      case 0x3e:
        if (!bVar20) goto switchD_0041c155_caseD_0;
        bVar20 = false;
        if (((uint)pwVar15 & 2) == 0) {
          bVar20 = uVar4 < 4;
          uVar4 = uVar4 - 4;
          if (bVar20 || uVar4 == 0)
          goto RichTextMarkup_ParseAndBuildStringAsset_FreePrimaryBufferAndReturnCapacityError;
          pwVar15[0] = L'\0';
          pwVar15[1] = L'\0';
          pwVar15 = pwVar15 + 2;
          bVar20 = false;
        }
        else {
          bVar19 = uVar4 < 2;
          uVar4 = uVar4 - 2;
          if (bVar19 || uVar4 == 0)
          goto RichTextMarkup_ParseAndBuildStringAsset_FreePrimaryBufferAndReturnCapacityError;
          *pwVar15 = L'\0';
          pwVar15 = pwVar15 + 1;
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
        sVar9 = uVar2 * 0x80 + -0x2000;
      }
      goto RichTextMarkup_ParseAndBuildStringAsset;
    }
  }
LAB_0041c5c0:
  RVar21.carry = true;
  RVar21.assetOrError = memory;
  return RVar21;
switchD_0041c155_caseD_0:
  iVar5 = iStack_20 * 8;
  *(wchar_t **)(&stack0xffffffd4 + iVar5) = memory;
  *(undefined4 *)(&stack0xffffffd0 + iVar5) = 0x41c5a2;
  (*g_MemoryApi.free)(*(void **)(&stack0xffffffd4 + iVar5));
  *(wchar_t **)(&stack0xffffffd4 + iVar5) = u_error__TXT2STR__unknown_characte_0041afac + 0x26;
  *(int *)(&stack0xffffffd0 + iVar5) = (int)pbVar13 - (int)markupBytes;
  *(undefined4 *)(&stack0xffffffcc + iVar5) = 1;
  *(undefined4 *)(&stack0xffffffc8 + iVar5) = 10;
  (&dStackY_3c)[iStack_20 * 2] = 0;
  aWStackY_44[iStack_20 * 2 + 1] = 0x40;
  aWStackY_44[iStack_20 * 2] = 0x41c5bb;
  (*g_WideNumberFormatUtf16)
            (aWStackY_44[iStack_20 * 2 + 1],(&dStackY_3c)[iStack_20 * 2],
             *(dword *)(&stack0xffffffc8 + iVar5),*(dword *)(&stack0xffffffcc + iVar5),
             *(sdword *)(&stack0xffffffd0 + iVar5),*(word **)(&stack0xffffffd4 + iVar5));
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
  ushort uVar1;
  int iVar2;
  ushort *puVar3;
  ushort *puVar4;
  bool bVar5;
  RichTextCopyExpandedEaxCf5 RVar6;
  RichTextCopyExpandedEaxCf5 RVar7;
  int local_14;
  
  local_14 = 0;
  puVar4 = destination;
  while( true ) {
    while( true ) {
      uVar1 = *source;
      puVar3 = source + 1;
      if (uVar1 == 0) break;
      if ((short)uVar1 < 0) {
        switch(uVar1 & 0x1f) {
        default:
          bVar5 = capacityBytes < 2;
          capacityBytes = capacityBytes - 2;
          if (bVar5 || capacityBytes == 0)
          goto RichTextCommandStream_CopyExpanded_TerminateOutputAndReturnCapacityError;
          *puVar4 = uVar1 & 0x1f | 0x8000;
          puVar4 = puVar4 + 1;
          source = puVar3;
          break;
        case 6:
          bVar5 = capacityBytes < 0x12;
          capacityBytes = capacityBytes - 0x12;
          if (bVar5 || capacityBytes == 0)
          goto RichTextCommandStream_CopyExpanded_TerminateOutputAndReturnCapacityError;
          for (iVar2 = 9; iVar2 != 0; iVar2 = iVar2 + -1) {
            *puVar4 = *source;
            source = source + 1;
            puVar4 = puVar4 + 1;
          }
          break;
        case 0x14:
        case 0x15:
        case 0x16:
          bVar5 = capacityBytes < 6;
          capacityBytes = capacityBytes - 6;
          if (bVar5 || capacityBytes == 0)
          goto RichTextCommandStream_CopyExpanded_TerminateOutputAndReturnCapacityError;
          for (iVar2 = 3; iVar2 != 0; iVar2 = iVar2 + -1) {
            *puVar4 = *source;
            source = source + 1;
            puVar4 = puVar4 + 1;
          }
          break;
        case 0x18:
          local_14 = (int)source + 3;
          source = *(word **)puVar3;
          break;
        case 0x19:
          source = *(word **)puVar3;
          break;
        case 0x1a:
          bVar5 = capacityBytes < 10;
          capacityBytes = capacityBytes - 10;
          if (bVar5 || capacityBytes == 0)
          goto RichTextCommandStream_CopyExpanded_TerminateOutputAndReturnCapacityError;
          for (iVar2 = 5; iVar2 != 0; iVar2 = iVar2 + -1) {
            *puVar4 = *source;
            source = source + 1;
            puVar4 = puVar4 + 1;
          }
        }
      }
      else {
        bVar5 = capacityBytes < 2;
        capacityBytes = capacityBytes - 2;
        if (bVar5 || capacityBytes == 0)
        goto RichTextCommandStream_CopyExpanded_TerminateOutputAndReturnCapacityError;
        *puVar4 = uVar1;
        puVar4 = puVar4 + 1;
        source = puVar3;
      }
    }
    if (local_14 == 0) break;
    source = (ushort *)(local_14 + 8);
    local_14 = local_14 + -1;
  }
  if (1 < (int)capacityBytes) {
    *puVar4 = 0;
    RVar6.eax = (int)puVar4 - (int)destination;
    RVar6.carry = false;
    return RVar6;
  }
RichTextCommandStream_CopyExpanded_TerminateOutputAndReturnCapacityError:
  puVar4[-1] = 0;
  RVar7.carry = true;
  RVar7.eax = 0x14;
  return RVar7;
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
  GraphicsSubresourceIndex glyphSubresource;
  dword dVar1;
  word *pwVar2;
  uint uVar3;
  RichTextExtentRegs RVar4;
  RichTextExtentRegs RVar5;
  FontGlyphSizeEaxEdxCf9 FVar6;
  GraphicsTextureSizeEaxEdxCf9 GVar7;
  int local_18;
  
  dVar1 = 0;
  g_ActiveFontIndex = packedStyle >> 0x18 & 7;
  local_18 = 0;
  uVar3 = 0;
switchD_0041cf77_caseD_0:
  while( true ) {
    pwVar2 = commandStream;
    glyphSubresource = (GraphicsSubresourceIndex)(short)*pwVar2;
    commandStream = pwVar2 + 1;
    if (glyphSubresource != 0) break;
    if (local_18 == 0) {
      RVar5.heightPixels = uVar3;
      RVar5.widthPixels = dVar1;
      return RVar5;
    }
    commandStream = (word *)(local_18 + 8);
    local_18 = local_18 + -1;
  }
  if ((int)glyphSubresource < 0) goto code_r0x0041cf74;
  goto RichTextCommandStream_Measure_AccumulateGlyphExtent;
code_r0x0041cf74:
  switch(glyphSubresource & 0x1f) {
  case 6:
    commandStream = pwVar2 + 9;
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
RichTextCommandStream_Measure_AccumulateGlyphExtent:
    FVar6 = FontGlyph_GetLogicalSizeActiveRegs(glyphSubresource);
    dVar1 = dVar1 + FVar6.width;
    if (uVar3 < FVar6.lineHeight) {
      uVar3 = FVar6.lineHeight;
    }
    break;
  case 0x12:
    RVar4.heightPixels = uVar3;
    RVar4.widthPixels = dVar1;
    return RVar4;
  case 0x14:
  case 0x15:
  case 0x16:
    commandStream = pwVar2 + 3;
    break;
  case 0x18:
    local_18 = (int)pwVar2 + 3;
    commandStream = *(word **)commandStream;
    break;
  case 0x19:
    commandStream = *(word **)commandStream;
    break;
  case 0x1a:
    GVar7 = (*g_GraphicsTextureSourceGetLogicalSize)
                      (*(dword *)(pwVar2 + 3),*(GraphicsTextureSourceAsset **)commandStream);
    dVar1 = dVar1 + GVar7.logicalWidthPixels;
    commandStream = pwVar2 + 5;
    if (uVar3 < GVar7.logicalHeightPixels) {
      uVar3 = GVar7.logicalHeightPixels;
    }
  }
  goto switchD_0041cf77_caseD_0;
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
  byte *pbVar1;
  GraphicsSubresourceIndex glyphSubresource;
  uint uVar2;
  uint uVar3;
  byte *pbVar4;
  byte *pbVar5;
  RichTextLineAdvanceEaxCf5 RVar6;
  FontGlyphSizeEaxEdxCf9 FVar7;
  GraphicsTextureSizeEaxEdxCf9 GVar8;
  byte *local_1c;
  
  FVar7 = FontGlyph_GetLogicalSizeActiveRegs(0);
  uVar2 = 0;
  local_1c = (byte *)0x0;
  uVar3 = FVar7.lineHeight;
  pbVar5 = g_FontRuntimeBuffer + g_RichTextRuntimeBufferUsedWords * 2;
  pbVar1 = local_1c;
switchD_0041d140_caseD_0:
  while( true ) {
    local_1c = pbVar1;
    pbVar4 = pbVar5;
    glyphSubresource = (GraphicsSubresourceIndex)*(short *)pbVar4;
    pbVar5 = pbVar4 + 2;
    if (glyphSubresource != 0x20) break;
    FVar7 = FontGlyph_GetLogicalSizeActiveRegs(0x20);
    if (maximumWidth < uVar2)
    goto RichTextCommandStream_MeasureNextWrappedLine_CommitWrapBoundaryAndReturnHeight;
    uVar2 = uVar2 + FVar7.width;
    pbVar1 = pbVar5;
  }
  if (glyphSubresource != 0) {
    pbVar1 = local_1c;
    if ((int)glyphSubresource < 0) goto code_r0x0041d13d;
    goto RichTextCommandStream_MeasureNextWrappedLine_AccumulateGlyphExtent;
  }
  goto switchD_0041d140_caseD_12;
code_r0x0041d13d:
  switch(glyphSubresource & 0x1f) {
  case 6:
    pbVar5 = pbVar4 + 0x12;
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
    FVar7 = FontGlyph_GetLogicalSizeActiveRegs(glyphSubresource);
    uVar2 = uVar2 + FVar7.width;
    if (uVar3 < FVar7.lineHeight) {
      uVar3 = FVar7.lineHeight;
    }
    break;
  case 0x11:
    FVar7 = FontGlyph_GetLogicalSizeActiveRegs(0x2d);
    pbVar1 = pbVar5;
    if (maximumWidth < FVar7.width + uVar2)
    goto RichTextCommandStream_MeasureNextWrappedLine_CommitWrapBoundaryAndReturnHeight;
    break;
  case 0x12:
switchD_0041d140_caseD_12:
    if (uVar2 <= maximumWidth) {
      local_1c = pbVar5;
    }
RichTextCommandStream_MeasureNextWrappedLine_CommitWrapBoundaryAndReturnHeight:
    if (local_1c == (byte *)0x0) {
      local_1c = pbVar5;
    }
    g_RichTextRuntimeBufferUsedWords = (uint)((int)local_1c - (int)g_FontRuntimeBuffer) >> 1;
    RVar6.carry = *(short *)(local_1c + -2) == 0;
    RVar6.lineAdvancePixels = uVar3;
    return RVar6;
  case 0x1a:
    GVar8 = (*g_GraphicsTextureSourceGetLogicalSize)
                      (*(dword *)(pbVar4 + 6),*(GraphicsTextureSourceAsset **)pbVar5);
    uVar2 = uVar2 + GVar8.logicalWidthPixels;
    pbVar5 = pbVar4 + 10;
    if (uVar3 < GVar8.logicalHeightPixels) {
      uVar3 = GVar8.logicalHeightPixels;
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
  byte *pbVar1;
  GraphicsSubresourceIndex GVar2;
  int iVar3;
  dword dVar4;
  uint uVar5;
  uint lineTop;
  byte *pbVar6;
  byte *pbVar7;
  byte *pbVar8;
  RichTextLineAdvanceEaxCf5 RVar9;
  RichTextLineAdvanceEaxCf5 RVar10;
  FontGlyphSizeEaxEdxCf9 FVar11;
  GraphicsTextureSizeEaxEdxCf9 GVar12;
  byte *local_1c;
  
  FVar11 = FontGlyph_GetLogicalSizeActiveRegs(0);
  uVar5 = 0;
  pbVar8 = g_FontRuntimeBuffer + g_RichTextRuntimeBufferUsedWords * 2;
  local_1c = (byte *)0x0;
  lineTop = FVar11.lineHeight;
  pbVar7 = pbVar8;
  dVar4 = g_ActiveFontIndex;
  pbVar1 = local_1c;
switchD_0041da40_caseD_0:
  while( true ) {
    local_1c = pbVar1;
    pbVar6 = pbVar7;
    GVar2 = (GraphicsSubresourceIndex)*(short *)pbVar6;
    pbVar7 = pbVar6 + 2;
    if (GVar2 != 0x20) break;
    FVar11 = FontGlyph_GetLogicalSizeActiveRegs(0x20);
    if (maximumWidth < uVar5)
    goto RichTextCommandStream_DrawNextWrappedLine_CommitWrapBoundaryAndBeginDrawing;
    uVar5 = uVar5 + FVar11.width;
    pbVar1 = pbVar7;
  }
  if (GVar2 != 0) {
    pbVar1 = local_1c;
    if ((int)GVar2 < 0) goto code_r0x0041da3d;
    goto RichTextCommandStream_DrawNextWrappedLine_AccumulateGlyphExtent;
  }
  goto switchD_0041da40_caseD_12;
code_r0x0041da3d:
  switch(GVar2 & 0x1f) {
  case 6:
    pbVar7 = pbVar6 + 0x12;
    break;
  case 8:
  case 9:
  case 10:
  case 0xb:
  case 0xc:
  case 0xd:
  case 0xe:
  case 0xf:
    g_ActiveFontIndex = GVar2 & 0xf;
    break;
  case 0x10:
    GVar2 = 0x20;
RichTextCommandStream_DrawNextWrappedLine_AccumulateGlyphExtent:
    FVar11 = FontGlyph_GetLogicalSizeActiveRegs(GVar2);
    uVar5 = uVar5 + FVar11.width;
    if (lineTop < FVar11.lineHeight) {
      lineTop = FVar11.lineHeight;
    }
    break;
  case 0x11:
    FVar11 = FontGlyph_GetLogicalSizeActiveRegs(0x2d);
    pbVar1 = pbVar7;
    if (maximumWidth < FVar11.width + uVar5)
    goto RichTextCommandStream_DrawNextWrappedLine_CommitWrapBoundaryAndBeginDrawing;
    break;
  case 0x12:
    goto switchD_0041da40_caseD_12;
  case 0x1a:
    GVar12 = (*g_GraphicsTextureSourceGetLogicalSize)
                       (*(dword *)(pbVar6 + 6),*(GraphicsTextureSourceAsset **)pbVar7);
    uVar5 = uVar5 + GVar12.logicalWidthPixels;
    pbVar7 = pbVar6 + 10;
    if (lineTop < GVar12.logicalHeightPixels) {
      lineTop = GVar12.logicalHeightPixels;
    }
  }
  goto switchD_0041da40_caseD_0;
switchD_0041da40_caseD_12:
  if (uVar5 <= maximumWidth) {
    local_1c = pbVar7;
  }
RichTextCommandStream_DrawNextWrappedLine_CommitWrapBoundaryAndBeginDrawing:
  g_ActiveFontIndex = dVar4;
  if (local_1c == (byte *)0x0) {
    local_1c = pbVar7;
  }
  lineBottom = drawY + lineTop;
switchD_0041dbd0_caseD_7:
  pbVar7 = pbVar8;
  GVar2 = (GraphicsSubresourceIndex)*(short *)pbVar7;
  pbVar8 = pbVar7 + 2;
  if (GVar2 != 0x20) {
    if (GVar2 == 0) {
      g_RichTextRuntimeBufferUsedWords = (uint)((int)pbVar8 - (int)g_FontRuntimeBuffer) >> 1;
      RVar10.carry = true;
      RVar10.lineAdvancePixels = lineTop;
      return RVar10;
    }
    if (-1 < (int)GVar2) goto RichTextCommandStream_DrawNextWrappedLine_DrawGlyphAndAdvanceX;
    goto code_r0x0041dbcd;
  }
  if (pbVar8 < local_1c) goto RichTextCommandStream_DrawNextWrappedLine_DrawGlyphAndAdvanceX;
  goto switchD_0041dbd0_caseD_12;
code_r0x0041dbcd:
  switch(GVar2 & 0x1f) {
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
         (((((((pbVar7[4] & 0xf) << 0x18 | (uint)*pbVar8 << 0x1c) >> 4 | (uint)pbVar7[8] << 0x1c) >>
             4 | (uint)pbVar7[6] << 0x1c) >> 4 | (uint)pbVar7[0xc] << 0x1c) >> 4 |
          (uint)pbVar7[10] << 0x1c) >> 4 | (uint)pbVar7[0x10] << 0x1c) >> 4 |
         (uint)pbVar7[0xe] << 0x1c;
    pbVar8 = pbVar7 + 0x12;
    break;
  case 8:
  case 9:
  case 10:
  case 0xb:
  case 0xc:
  case 0xd:
  case 0xe:
  case 0xf:
    g_ActiveFontIndex = GVar2 & 0xf;
    break;
  case 0x10:
    GVar2 = 0x20;
RichTextCommandStream_DrawNextWrappedLine_DrawGlyphAndAdvanceX:
    iVar3 = FontGlyph_DrawVerticallyCentered
                      (clipTop,clipLeft,clipBottom,clipRight,GVar2,lineTop,lineBottom,drawX);
    drawX = drawX + iVar3;
    break;
  case 0x11:
    if (local_1c <= pbVar8) {
      FontGlyph_DrawVerticallyCentered
                (clipTop,clipLeft,clipBottom,clipRight,0x2d,lineTop,lineBottom,drawX);
      goto switchD_0041dbd0_caseD_12;
    }
    break;
  case 0x12:
switchD_0041dbd0_caseD_12:
    g_RichTextRuntimeBufferUsedWords = (uint)((int)pbVar8 - (int)g_FontRuntimeBuffer) >> 1;
    RVar9.carry = false;
    RVar9.lineAdvancePixels = lineTop;
    return RVar9;
  case 0x1a:
    GVar12 = (*g_GraphicsTextureSourceGetLogicalSize)
                       (*(dword *)(pbVar7 + 6),*(GraphicsTextureSourceAsset **)pbVar8);
    dVar4 = GVar12.logicalWidthPixels;
    (*g_GraphicsTextureSourceBlitSourceAlpha)
              (clipTop,clipLeft,clipBottom,clipRight,lineBottom - GVar12.logicalHeightPixels,drawX,
               *(dword *)(pbVar7 + 6),*(GraphicsTextureSourceAsset **)pbVar8,g_FramebufferAccess);
    drawX = drawX + dVar4;
    pbVar8 = pbVar7 + 10;
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
  ushort uVar1;
  uint uVar2;
  int iVar3;
  int unaff_EBP;
  ushort *puVar4;
  ushort *puVar5;
  
  uVar2 = 0x2000;
  iVar3 = 0;
  puVar5 = (ushort *)g_FontRuntimeBuffer;
switchD_0041d87b_caseD_7:
  while( true ) {
    puVar4 = commandStream;
    uVar1 = *puVar4;
    commandStream = puVar4 + 1;
    if (uVar1 != 0) break;
    if (iVar3 == 0) {
      *puVar5 = 0;
      g_RichTextRuntimeBufferUsedWords = 0;
      return;
    }
    iVar3 = iVar3 + -1;
    commandStream = (ushort *)(unaff_EBP + 8);
  }
  if ((short)uVar1 < 0) goto switchD_0041d87b_switchD;
  goto switchD_0041d87b_caseD_0;
switchD_0041d87b_switchD:
  switch(uVar1 & 0x1f) {
  default:
switchD_0041d87b_caseD_0:
    if (uVar2 != 0) {
      *puVar5 = uVar1;
      uVar2 = uVar2 - 1;
      puVar5 = puVar5 + 1;
    }
    break;
  case 6:
    if (9 < uVar2) {
      *puVar5 = uVar1;
      uVar2 = uVar2 - 9;
      *(undefined4 *)(puVar5 + 1) = *(undefined4 *)commandStream;
      *(undefined4 *)(puVar5 + 3) = *(undefined4 *)(puVar4 + 3);
      *(undefined4 *)(puVar5 + 5) = *(undefined4 *)(puVar4 + 5);
      *(undefined4 *)(puVar5 + 7) = *(undefined4 *)(puVar4 + 7);
      puVar5 = puVar5 + 9;
      commandStream = puVar4 + 9;
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
    iVar3 = iVar3 + 1;
  case 0x19:
    commandStream = *(ushort **)commandStream;
    break;
  case 0x1a:
    if (5 < uVar2) {
      *puVar5 = uVar1;
      uVar2 = uVar2 - 5;
      *(undefined4 *)(puVar5 + 1) = *(undefined4 *)commandStream;
      *(undefined4 *)(puVar5 + 3) = *(undefined4 *)(puVar4 + 3);
      puVar5 = puVar5 + 5;
      commandStream = puVar4 + 5;
    }
  }
  goto switchD_0041d87b_caseD_7;
}


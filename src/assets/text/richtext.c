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
RichTextExtentRegs
RichTextCommandStream_MeasureWrappedBlockRegs
          (dword packedStyle,word *commandStream,UiPixelExtent maximumWidth)

{
  uint extraout_ECX;
  uint uVar1;
  bool bVar2;
  undefined8 uVar3;
  RichTextExtentRegs RVar4;
  
  RichTextCommandStream_FlattenNestedToRuntimeBuffer(commandStream);
  uVar1 = extraout_ECX >> 0x10 & 7;
  g_ActiveFontIndex = extraout_ECX >> 0x18 & 7;
  g_RichTextCurrentColorArgb = (&g_RichTextColorPalette0Argb)[uVar1];
  g_RichTextCurrentShadowOffset = (&g_RichTextShadowOffsetPalette0)[uVar1];
  bVar2 = false;
  g_RichTextSavedColorArgb = g_RichTextCurrentColorArgb;
  g_RichTextSavedShadowOffset = g_RichTextCurrentShadowOffset;
  while( true ) {
    uVar3 = RichTextCommandStream_MeasureNextWrappedLineCf(maximumWidth);
    uVar1 = (uint)((ulonglong)uVar3 >> 0x20);
    if (bVar2) break;
    bVar2 = CARRY4(uVar1,(uint)uVar3);
  }
  RVar4.heightPixels = uVar1 + (uint)uVar3;
  RVar4.widthPixels = maximumWidth;
  return RVar4;
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
void RichTextCommandStream_DrawWrappedBlockCf
               (UiPixelCoordinate clipTop,UiPixelCoordinate clipLeft,UiPixelCoordinate clipBottom,
               UiPixelCoordinate clipRight,dword packedStyle,word *commandStream,
               UiPixelExtent maximumWidth,UiPixelCoordinate drawY,UiPixelCoordinate drawX)

{
  uint extraout_ECX;
  uint uVar1;
  bool bVar2;
  undefined8 uVar3;
  
  RichTextCommandStream_FlattenNestedToRuntimeBuffer(commandStream);
  uVar1 = extraout_ECX >> 0x10 & 7;
  bVar2 = false;
  g_ActiveFontIndex = extraout_ECX >> 0x18 & 7;
  g_RichTextCurrentColorArgb = (&g_RichTextColorPalette0Argb)[uVar1];
  g_RichTextCurrentShadowOffset = (&g_RichTextShadowOffsetPalette0)[uVar1];
  g_RichTextSavedColorArgb = g_RichTextCurrentColorArgb;
  g_RichTextSavedShadowOffset = g_RichTextCurrentShadowOffset;
  while( true ) {
    uVar3 = RichTextCommandStream_DrawNextWrappedLineCf
                      (clipTop,clipLeft,clipBottom,clipRight,maximumWidth,drawY,drawX);
    if (bVar2) break;
    bVar2 = CARRY4(drawY,(uint)uVar3);
    drawY = drawY + (uint)uVar3;
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
undefined8
RichTextCommandStream_DrawSingleLine
          (UiPixelCoordinate clipTop,UiPixelCoordinate clipLeft,UiPixelCoordinate clipBottom,
          UiPixelCoordinate clipRight,UiPackedTextStyle packedStyle,word *commandStream,
          UiPixelCoordinate drawX,UiPixelCoordinate baselineY)

{
  int baselineY_00;
  undefined4 in_EAX;
  uint uVar1;
  GraphicsSubresourceIndex glyphSubresource;
  int iVar2;
  uint extraout_ECX;
  undefined4 in_EDX;
  word *pwVar3;
  RichTextExtentRegs RVar4;
  qword qVar5;
  byte *local_1c;
  
  RVar4 = RichTextCommandStream_MeasureRegs(packedStyle,commandStream);
  uVar1 = RVar4.widthPixels;
  baselineY_00 = drawX + RVar4.heightPixels;
  if ((extraout_ECX & 1) == 0) {
    if ((extraout_ECX & 2) == 0)
    goto RichTextCommandStream_DrawSingleLine_InitializeStyleAndBeginDrawing;
    uVar1 = uVar1 >> 1;
  }
  baselineY = baselineY - uVar1;
RichTextCommandStream_DrawSingleLine_InitializeStyleAndBeginDrawing:
  uVar1 = extraout_ECX >> 0x10 & 7;
  g_ActiveFontIndex = extraout_ECX >> 0x18 & 7;
  g_RichTextCurrentColorArgb = (&g_RichTextColorPalette0Argb)[uVar1];
  g_RichTextCurrentShadowOffset = (&g_RichTextShadowOffsetPalette0)[uVar1];
  local_1c = (byte *)0x0;
  g_RichTextSavedColorArgb = g_RichTextCurrentColorArgb;
  g_RichTextSavedShadowOffset = g_RichTextCurrentShadowOffset;
switchD_0041d537_caseD_7:
  while( true ) {
    pwVar3 = commandStream;
    glyphSubresource = (GraphicsSubresourceIndex)(short)*pwVar3;
    commandStream = pwVar3 + 1;
    if (glyphSubresource != 0) break;
    if (local_1c == (byte *)0x0) {
      return CONCAT44(in_EDX,in_EAX);
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
         ((((((((byte)pwVar3[2] & 0xf) << 0x18 | (uint)(byte)*commandStream << 0x1c) >> 4 |
             (uint)(byte)pwVar3[4] << 0x1c) >> 4 | (uint)(byte)pwVar3[3] << 0x1c) >> 4 |
           (uint)(byte)pwVar3[6] << 0x1c) >> 4 | (uint)(byte)pwVar3[5] << 0x1c) >> 4 |
         (uint)(byte)pwVar3[8] << 0x1c) >> 4 | (uint)(byte)pwVar3[7] << 0x1c;
    commandStream = pwVar3 + 9;
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
    return CONCAT44(in_EDX,in_EAX);
  case 0x14:
  case 0x15:
  case 0x16:
    commandStream = pwVar3 + 3;
    break;
  case 0x18:
    local_1c = (byte *)((int)pwVar3 + 3);
    commandStream = *(word **)commandStream;
    break;
  case 0x19:
    commandStream = *(word **)commandStream;
    break;
  case 0x1a:
    qVar5 = (*g_GraphicsTextureSourceGetLogicalSize)
                      (*(dword *)(pwVar3 + 3),*(GraphicsTextureSourceAsset **)commandStream);
    iVar2 = (int)qVar5;
    (*g_GraphicsTextureSourceBlitSourceAlpha)
              (clipTop,clipLeft,clipBottom,clipRight,baselineY_00 - (int)(qVar5 >> 0x20),baselineY,
               *(dword *)(pwVar3 + 3),*(GraphicsTextureSourceAsset **)commandStream,
               g_FramebufferAccess);
    baselineY = baselineY + iVar2;
    commandStream = pwVar3 + 5;
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
undefined8
RichTextCommandStream_PatchPayloadBySelector
          (RichTextCommandSelector selector,void *replacementPayload,word *stream)

{
  ushort uVar1;
  undefined4 in_EAX;
  undefined4 in_EDX;
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
  return CONCAT44(in_EDX,in_EAX);
}

/* Address: 0x0041B200.
   Ownership: assets/text/richtext.
   Purpose: Handles rich text command stream bind texture source.
*/
void RichTextCommandStream_BindTextureSource(GraphicsTextureSourceAsset *textureSource,word *stream)

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
undefined8 __fastcall
RichTextCommandStream_FindNthCommandPayloadPair
          (undefined4 param_1,undefined4 param_2,int param_3,undefined4 param_4,ushort *param_5)

{
  ushort uVar1;
  undefined4 in_EAX;
  int iVar2;
  ushort *puVar3;
  
  iVar2 = param_3 + 1;
  do {
    do {
      puVar3 = param_5;
      uVar1 = *puVar3;
      if (uVar1 == 0) {
        return CONCAT44(param_2,in_EAX);
      }
      param_5 = puVar3 + 1;
    } while (-1 < (short)uVar1);
    switch(uVar1 & 0x1f) {
    case 6:
      param_5 = puVar3 + 9;
      break;
    case 0x14:
    case 0x15:
    case 0x16:
      iVar2 = iVar2 + -1;
      param_5 = puVar3 + 3;
      if (iVar2 == 0) {
        *(undefined4 *)(puVar3 + 1) = param_4;
        return CONCAT44(param_2,in_EAX);
      }
      break;
    case 0x18:
    case 0x19:
    case 0x1a:
      param_5 = puVar3 + 5;
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
undefined4 RichTextCommandStream_FindNestedStreamPointer(undefined4 param_1,ushort *param_2)

{
  undefined4 in_EAX;
  ushort *commandCursor;
  ushort *puVar1;
  ushort commandCodeUnit;
  
  puVar1 = param_2;
  while( true ) {
    commandCursor = puVar1;
    commandCodeUnit = *commandCursor;
    puVar1 = commandCursor + 1;
    if (commandCodeUnit == 0) break;
    if ((short)commandCodeUnit < 0) {
      switch(commandCodeUnit & 0x1f) {
      case 6:
        puVar1 = commandCursor + 9;
        break;
      case 0x14:
      case 0x15:
      case 0x16:
        puVar1 = commandCursor + 3;
        break;
      case 0x18:
      case 0x19:
        *(undefined4 *)puVar1 = param_1;
        puVar1 = commandCursor + 5;
        break;
      case 0x1a:
        puVar1 = commandCursor + 5;
      }
    }
  }
  return in_EAX;
}

/* Address: 0x0041B520.
   Ownership: assets/text/richtext.
   Purpose: Physical RET cleanup=12 stack bytes. This function object claims Listing ownership for a previously
   unowned multi-entry/shared-tail/computed-dispatch region; it does not assert that every member entry is an
   independent ABI-level function. Body boundaries remain exact and are not split into speculative ABI functions.
   Typed parameters: p0 param_1→RichTextOpcode1APayloadValue32_V345. Calling convention, complete VariableStorage
   serialization, function bytes, control flow, globals, locals, and executable data remain unchanged.
*/
undefined4
RichTextCommandStream_FindOpcode1APayloadPair(undefined4 param_1,undefined4 param_2,ushort *param_3)

{
  undefined4 in_EAX;
  ushort *commandCursor;
  ushort *puVar1;
  ushort commandCodeUnit;
  
  puVar1 = param_3;
  while( true ) {
    commandCursor = puVar1;
    commandCodeUnit = *commandCursor;
    puVar1 = commandCursor + 1;
    if (commandCodeUnit == 0) break;
    if ((short)commandCodeUnit < 0) {
      switch(commandCodeUnit & 0x1f) {
      case 6:
        puVar1 = commandCursor + 9;
        break;
      case 0x14:
      case 0x15:
      case 0x16:
        puVar1 = commandCursor + 3;
        break;
      case 0x18:
      case 0x19:
        puVar1 = commandCursor + 5;
        break;
      case 0x1a:
        *(undefined4 *)puVar1 = param_2;
        *(undefined4 *)(commandCursor + 3) = param_1;
        puVar1 = commandCursor + 5;
      }
    }
  }
  return in_EAX;
}

/* Address: 0x0041B620.
   Ownership: assets/text/richtext.
   Purpose: Physical RET cleanup=8 stack bytes. This function object claims Listing ownership for a previously
   unowned multi-entry/shared-tail/computed-dispatch region; it does not assert that every member entry is an
   independent ABI-level function. Body boundaries remain exact and are not split into speculative ABI functions.
   Typed parameters: p0 param_1→RichTextInlinePayloadValue32_V345. Calling convention, complete VariableStorage
   serialization, function bytes, control flow, globals, locals, and executable data remain unchanged.
*/
undefined4 RichTextCommandStream_FindInlinePayloadPair(undefined4 param_1,ushort *param_2)

{
  undefined4 in_EAX;
  ushort *commandCursor;
  ushort *puVar1;
  ushort commandCodeUnit;
  
  puVar1 = param_2;
  while( true ) {
    commandCursor = puVar1;
    commandCodeUnit = *commandCursor;
    puVar1 = commandCursor + 1;
    if (commandCodeUnit == 0) break;
    if ((short)commandCodeUnit < 0) {
      switch(commandCodeUnit & 0x1f) {
      case 6:
        puVar1 = commandCursor + 9;
        break;
      case 0x14:
      case 0x15:
      case 0x16:
        *(undefined4 *)puVar1 = param_1;
        puVar1 = commandCursor + 3;
        break;
      case 0x18:
      case 0x19:
      case 0x1a:
        puVar1 = commandCursor + 5;
      }
    }
  }
  return in_EAX;
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
undefined8 __fastcall
RichTextCommandStream_FindNthCommandFlagsPair
          (undefined4 param_1,undefined4 param_2,int param_3,uint param_4,uint *param_5)

{
  uint *puVar1;
  undefined4 in_EAX;
  int iVar2;
  uint *commandCursor;
  ushort commandCodeUnit;
  
  iVar2 = param_3 + 1;
  puVar1 = param_5;
  do {
    do {
      commandCursor = puVar1;
      commandCodeUnit = (ushort)*commandCursor;
      if (commandCodeUnit == 0) {
        return CONCAT44(param_2,in_EAX);
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
        *commandCursor = *commandCursor | param_4;
        return CONCAT44(param_2,in_EAX);
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
ulonglong __fastcall
RichTextCommandStream_QueryNthCommandFlags
          (undefined4 param_1,uint param_2,int param_3,ushort *param_4)

{
  ushort uVar1;
  int iVar2;
  ushort *puVar3;
  
  iVar2 = param_3 + 1;
  do {
    do {
      puVar3 = param_4;
      uVar1 = *puVar3;
      if (uVar1 == 0) {
        return (ulonglong)param_2 << 0x20;
      }
      param_4 = puVar3 + 1;
    } while (-1 < (short)uVar1);
    switch(uVar1 & 0x1f) {
    case 6:
      param_4 = puVar3 + 9;
      break;
    case 0x14:
    case 0x15:
    case 0x16:
      iVar2 = iVar2 + -1;
      param_4 = puVar3 + 3;
      if (iVar2 == 0) {
        return CONCAT44(param_2,*(undefined4 *)puVar3) & 0xffffffff00000003;
      }
      break;
    case 0x18:
    case 0x19:
    case 0x1a:
      param_4 = puVar3 + 5;
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
int RichTextCommandStream_CopyToNarrowCf
              (TextOutputCapacityBytes capacityBytes,byte *destination,word *source)

{
  word *pwVar1;
  dword remainingCapacityBytes;
  ushort *commandCursor;
  word *streamCursor;
  bool newlineCapacityUnderflow;
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
    return capacityBytes - (remainingCapacityBytes - 1);
  }
RichTextCommandStream_CopyToNarrow_TerminateOutputAndReturnCapacityError:
  destination[-1] = 0;
  return 0x14;
}

/* Address: 0x0041BCB0.
   Ownership: assets/text/richtext.
   Purpose: Physical RET cleanup=4 stack bytes. This function object claims Listing ownership for a previously
   unowned multi-entry/shared-tail/computed-dispatch region; it does not assert that every member entry is an
   independent ABI-level function. Body boundaries remain exact and are not split into speculative ABI functions.
   Typed parameters: p0 param_1→RichTextMarkupCapacityCodeUnits_V345. Calling convention, complete VariableStorage
   serialization, function bytes, control flow, globals, locals, and executable data remain unchanged.
*/
undefined8 __fastcall
RichTextMarkup_ParseAndBuildStringAsset(undefined4 param_1,undefined4 param_2,byte *param_3)

{
  byte bVar1;
  ushort uVar2;
  wchar_t *memory;
  int *memory_00;
  uint uVar3;
  uint extraout_ECX;
  uint uVar4;
  uint extraout_ECX_00;
  int iVar5;
  int *piVar6;
  int iVar7;
  int iVar8;
  short sVar9;
  byte *pbVar10;
  byte *pbVar11;
  int *piVar12;
  wchar_t *pwVar13;
  int *piVar14;
  dword dVar15;
  int *piVar16;
  bool bVar17;
  bool bVar18;
  WideNumberFormatFlags aWStackY_44 [2];
  dword dStackY_3c;
  int iStack_24;
  int iStack_20;
  
  bVar17 = &stack0xffffffec < (undefined1 *)0x14;
  memory = (*g_MemoryApi.allocLargestFreeBlock)();
  if (!bVar17) {
    sVar9 = 0;
    iStack_20 = 0;
    uVar4 = extraout_ECX;
    pbVar11 = param_3;
    pwVar13 = memory;
    bVar17 = false;
RichTextMarkup_ParseAndBuildStringAsset:
    pbVar10 = pbVar11;
    uVar2 = (ushort)*pbVar10;
    pbVar11 = pbVar10 + 1;
    switch(*pbVar10) {
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
      if (bVar17) {
        bVar18 = uVar4 < 2;
        uVar4 = uVar4 - 2;
        if (bVar18 || uVar4 == 0)
        goto RichTextMarkup_ParseAndBuildStringAsset_FreePrimaryBufferAndReturnCapacityError;
        *pwVar13 = L'耒';
        pwVar13 = pwVar13 + 1;
      }
      goto RichTextMarkup_ParseAndBuildStringAsset;
    default:
switchD_0041c155_caseD_23:
      if (bVar17) {
        bVar18 = uVar4 < 2;
        uVar4 = uVar4 - 2;
        if (bVar18 || uVar4 == 0)
        goto RichTextMarkup_ParseAndBuildStringAsset_FreePrimaryBufferAndReturnCapacityError;
        *pwVar13 = uVar2 + sVar9;
        pwVar13 = pwVar13 + 1;
      }
      goto RichTextMarkup_ParseAndBuildStringAsset;
    case 0x23:
      bVar1 = *pbVar11;
      uVar2 = (ushort)bVar1;
      pbVar11 = pbVar10 + 2;
      switch(bVar1) {
      default:
        goto switchD_0041c155_caseD_0;
      case 10:
      case 0xd:
        while (bVar1 = *pbVar11, bVar1 < 0x20) {
          pbVar11 = pbVar11 + 1;
          if ((bVar1 != 10) && (bVar1 != 0xd)) goto switchD_0041c155_caseD_0;
        }
        break;
      case 0x21:
        sVar9 = 0x7fc0;
        break;
      case 0x23:
        goto switchD_0041c155_caseD_23;
      case 0x2d:
        bVar18 = uVar4 < 2;
        uVar4 = uVar4 - 2;
        if (bVar18 || uVar4 == 0)
        goto RichTextMarkup_ParseAndBuildStringAsset_FreePrimaryBufferAndReturnCapacityError;
        *pwVar13 = L'耑';
        pwVar13 = pwVar13 + 1;
        break;
      case 0x2e:
        if ((iStack_20 != 0) && (!bVar17)) {
          iStack_20 = iStack_20 + 1;
          bVar17 = pwVar13 < memory;
          dStackY_3c = 0x41c70a;
          (*g_MemoryApi.shrinkInPlace)((int)pwVar13 - (int)memory,memory);
          if ((!bVar17) && (memory_00 = (*g_MemoryApi.allocLargestFreeBlock)(), !bVar17)) {
            uVar4 = extraout_ECX_00 - 0x200;
            if (0x1ff < extraout_ECX_00 && uVar4 != 0) {
              iStack_24 = 0;
              piVar16 = memory_00;
              for (iVar5 = 0x80; iVar7 = iStack_20, iVar5 != 0; iVar5 = iVar5 + -1) {
                *piVar16 = 0;
                piVar16 = piVar16 + 1;
              }
              while( true ) {
                while (piVar14 = piVar16, iVar5 = *(int *)(&stack0xffffffd0 + iVar7 * 8),
                      iVar5 == -1) {
                  iVar7 = iVar7 + -1;
                  piVar16 = piVar14;
                  if (iVar7 == 0) {
                    (*g_MemoryApi.free)(memory);
                    dVar15 = (int)piVar14 - (int)memory_00;
                    (*g_MemoryApi.shrinkInPlace)(dVar15,memory_00);
                    iStack_20 = iStack_20 * 8;
                    *(dword *)(&stack0xffffffd4 + iStack_20) = dVar15;
                    memory_00[0x2c] = iStack_24;
                    memory_00[1] = dVar15;
                    *memory_00 = 0x727473;
                    memory_00[2] = 1;
                    memory_00[3] = 0;
                    *(undefined4 *)(&stack0xffffffd0 + iStack_20) = 0x41c7b5;
                    dVar15 = (*g_LocaleGetPackedCurrentTime)();
                    memory_00[4] = dVar15;
                    memory_00[6] = dVar15;
                    memory_00[8] = dVar15;
                    *(undefined4 *)(&stack0xffffffd0 + iStack_20) = 0x41c7cd;
                    dVar15 = (*g_LocaleGetPackedCurrentDate)();
                    memory_00[5] = dVar15;
                    memory_00[7] = dVar15;
                    memory_00[9] = dVar15;
                    *(int **)(&stack0xffffffd0 + iStack_20) = memory_00 + 0xc;
                    *(undefined4 *)(&stack0xffffffcc + iStack_20) = 0x41c7ec;
                    (*g_LocaleCopyDefaultComputerLabelUtf16)
                              (*(word **)(&stack0xffffffd0 + iStack_20));
                    *(int **)(&stack0xffffffd0 + iStack_20) = memory_00 + 0x1c;
                    *(undefined4 *)(&stack0xffffffcc + iStack_20) = 0x41c7f9;
                    (*g_LocaleCopyDefaultComputerLabelUtf16)
                              (*(word **)(&stack0xffffffd0 + iStack_20));
                    return CONCAT44(param_2,memory_00);
                  }
                }
                bVar17 = uVar4 < 0x10;
                uVar4 = uVar4 - 0x10;
                if (bVar17 || uVar4 == 0) break;
                piVar14[2] = iVar5;
                *piVar14 = 0x10;
                piVar14[1] = 0;
                do {
                  if (iVar5 == *(int *)(&stack0xffffffd0 + iVar7 * 8)) {
                    iVar8 = *(int *)(&stack0xffffffcc + iVar7 * 8);
                    piVar14[1] = piVar14[1] + 1;
                    uVar3 = (iVar8 - *(int *)(&stack0xffffffd4 + iVar7 * 8)) + 4;
                    *piVar14 = *piVar14 + uVar3;
                    bVar17 = uVar4 < uVar3;
                    uVar4 = uVar4 - uVar3;
                    if (bVar17 || uVar4 == 0)
                    goto 
                    RichTextMarkup_ParseAndBuildStringAsset_FreeTemporaryExpansionBufferBeforeCapacityError
                    ;
                  }
                  iVar7 = iVar7 + -1;
                } while (iVar7 != 0);
                piVar6 = piVar14 + 4;
                iStack_24 = iStack_24 + 1;
                piVar16 = piVar6 + piVar14[1];
                iVar8 = iStack_20;
                do {
                  if (iVar5 == *(int *)(&stack0xffffffd0 + iVar8 * 8)) {
                    *(undefined4 *)(&stack0xffffffd0 + iVar8 * 8) = 0xffffffff;
                    iVar7 = (int)piVar16 - (int)piVar14;
                    piVar12 = *(int **)(&stack0xffffffd4 + iVar8 * 8);
                    for (uVar3 = (uint)(*(int *)(&stack0xffffffcc + iVar8 * 8) -
                                       (int)*(int **)(&stack0xffffffd4 + iVar8 * 8)) >> 2;
                        uVar3 != 0; uVar3 = uVar3 - 1) {
                      *piVar16 = *piVar12;
                      piVar12 = piVar12 + 1;
                      piVar16 = piVar16 + 1;
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
            (*g_MemoryApi.free)(memory_00);
          }
RichTextMarkup_ParseAndBuildStringAsset_FreePrimaryBufferAndReturnCapacityError:
          iStack_20 = iStack_20 * 8;
          *(wchar_t **)(&stack0xffffffd4 + iStack_20) = memory;
          *(undefined4 *)(&stack0xffffffd0 + iStack_20) = 0x41c5e3;
          (*g_MemoryApi.free)(*(void **)(&stack0xffffffd4 + iStack_20));
          return CONCAT44(param_2,0x14);
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
        if ((((*pbVar11 < 0x30) || (0x39 < *pbVar11)) || (pbVar10[3] < 0x30)) || (0x39 < pbVar10[3])
           ) goto switchD_0041c155_caseD_0;
        pbVar11 = pbVar10 + 4;
        break;
      case 0x3c:
        if (bVar17) goto switchD_0041c155_caseD_0;
        iStack_20 = iStack_20 + 1;
        bVar17 = true;
        break;
      case 0x3e:
        if (!bVar17) goto switchD_0041c155_caseD_0;
        bVar17 = false;
        if (((uint)pwVar13 & 2) == 0) {
          bVar17 = uVar4 < 4;
          uVar4 = uVar4 - 4;
          if (bVar17 || uVar4 == 0)
          goto RichTextMarkup_ParseAndBuildStringAsset_FreePrimaryBufferAndReturnCapacityError;
          pwVar13[0] = L'\0';
          pwVar13[1] = L'\0';
          pwVar13 = pwVar13 + 2;
          bVar17 = false;
        }
        else {
          bVar18 = uVar4 < 2;
          uVar4 = uVar4 - 2;
          if (bVar18 || uVar4 == 0)
          goto RichTextMarkup_ParseAndBuildStringAsset_FreePrimaryBufferAndReturnCapacityError;
          *pwVar13 = L'\0';
          pwVar13 = pwVar13 + 1;
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
  return CONCAT44(param_2,memory);
switchD_0041c155_caseD_0:
  iVar5 = iStack_20 * 8;
  *(wchar_t **)(&stack0xffffffd4 + iVar5) = memory;
  *(undefined4 *)(&stack0xffffffd0 + iVar5) = 0x41c5a2;
  (*g_MemoryApi.free)(*(void **)(&stack0xffffffd4 + iVar5));
  *(wchar_t **)(&stack0xffffffd4 + iVar5) = u_error__TXT2STR__unknown_characte_0041afac + 0x26;
  *(int *)(&stack0xffffffd0 + iVar5) = (int)pbVar11 - (int)param_3;
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
int RichTextCommandStream_CopyExpandedCf
              (TextOutputCapacityBytes capacityBytes,word *destination,word *source)

{
  ushort uVar1;
  int iVar2;
  ushort *puVar3;
  ushort *puVar4;
  bool bVar5;
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
    return (int)puVar4 - (int)destination;
  }
RichTextCommandStream_CopyExpanded_TerminateOutputAndReturnCapacityError:
  puVar4[-1] = 0;
  return 0x14;
}

/* Address: 0x0041CF30.
   Ownership: assets/text/richtext.
   Purpose: Measures a UTF-16 rich-text command stream, follows nested stream tags, applies font-selection
   commands, and includes embedded texture dimensions. EAX is width and EDX is maximum height. Typed parameters: p0
   packedStyle→UiPackedTextStyle_V301. Calling convention, parameter storage, body bytes, control flow, globals,
   locals, and executable data remain unchanged.
   Cross-module calls: FontGlyph_GetLogicalSizeActiveRegs [assets/text/resources].
*/
RichTextExtentRegs
RichTextCommandStream_MeasureRegs(UiPackedTextStyle packedStyle,word *commandStream)

{
  GraphicsSubresourceIndex glyphSubresource;
  int iVar1;
  dword dVar2;
  int extraout_ECX;
  int extraout_ECX_00;
  uint extraout_EDX;
  uint uVar3;
  word *pwVar4;
  uint uVar5;
  qword qVar6;
  RichTextExtentRegs RVar7;
  RichTextExtentRegs RVar8;
  int local_18;
  
  dVar2 = 0;
  g_ActiveFontIndex = packedStyle >> 0x18 & 7;
  local_18 = 0;
  uVar5 = 0;
switchD_0041cf77_caseD_0:
  while( true ) {
    pwVar4 = commandStream;
    glyphSubresource = (GraphicsSubresourceIndex)(short)*pwVar4;
    commandStream = pwVar4 + 1;
    if (glyphSubresource != 0) break;
    if (local_18 == 0) {
      RVar8.heightPixels = uVar5;
      RVar8.widthPixels = dVar2;
      return RVar8;
    }
    commandStream = (word *)(local_18 + 8);
    local_18 = local_18 + -1;
  }
  if ((int)glyphSubresource < 0) goto code_r0x0041cf74;
  goto RichTextCommandStream_Measure_AccumulateGlyphExtent;
code_r0x0041cf74:
  switch(glyphSubresource & 0x1f) {
  case 6:
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
RichTextCommandStream_Measure_AccumulateGlyphExtent:
    iVar1 = FontGlyph_GetLogicalSizeActiveRegs(glyphSubresource);
    dVar2 = extraout_ECX + iVar1;
    if (uVar5 < extraout_EDX) {
      uVar5 = extraout_EDX;
    }
    break;
  case 0x12:
    RVar7.heightPixels = uVar5;
    RVar7.widthPixels = dVar2;
    return RVar7;
  case 0x14:
  case 0x15:
  case 0x16:
    commandStream = pwVar4 + 3;
    break;
  case 0x18:
    local_18 = (int)pwVar4 + 3;
    commandStream = *(word **)commandStream;
    break;
  case 0x19:
    commandStream = *(word **)commandStream;
    break;
  case 0x1a:
    qVar6 = (*g_GraphicsTextureSourceGetLogicalSize)
                      (*(dword *)(pwVar4 + 3),*(GraphicsTextureSourceAsset **)commandStream);
    uVar3 = (uint)(qVar6 >> 0x20);
    dVar2 = extraout_ECX_00 + (int)qVar6;
    commandStream = pwVar4 + 5;
    if (uVar5 < uVar3) {
      uVar5 = uVar3;
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
undefined8 RichTextCommandStream_MeasureNextWrappedLineCf(UiPixelExtent maximumWidth)

{
  byte *pbVar1;
  GraphicsSubresourceIndex glyphSubresource;
  int iVar2;
  uint uVar3;
  int extraout_ECX;
  int extraout_ECX_00;
  uint extraout_ECX_01;
  uint extraout_ECX_02;
  undefined4 in_EDX;
  uint extraout_EDX;
  uint extraout_EDX_00;
  uint uVar4;
  uint uVar5;
  byte *pbVar6;
  byte *pbVar7;
  qword qVar8;
  byte *local_1c;
  
  FontGlyph_GetLogicalSizeActiveRegs(0);
  uVar3 = 0;
  local_1c = (byte *)0x0;
  uVar5 = extraout_EDX;
  pbVar7 = g_FontRuntimeBuffer + g_RichTextRuntimeBufferUsedWords * 2;
  pbVar1 = local_1c;
switchD_0041d140_caseD_0:
  while( true ) {
    local_1c = pbVar1;
    pbVar6 = pbVar7;
    glyphSubresource = (GraphicsSubresourceIndex)*(short *)pbVar6;
    pbVar7 = pbVar6 + 2;
    if (glyphSubresource != 0x20) break;
    iVar2 = FontGlyph_GetLogicalSizeActiveRegs(0x20);
    if (maximumWidth < extraout_ECX_02)
    goto RichTextCommandStream_MeasureNextWrappedLine_CommitWrapBoundaryAndReturnHeight;
    uVar3 = extraout_ECX_02 + iVar2;
    pbVar1 = pbVar7;
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
    g_ActiveFontIndex = glyphSubresource & 0xf;
    break;
  case 0x10:
    glyphSubresource = 0x20;
RichTextCommandStream_MeasureNextWrappedLine_AccumulateGlyphExtent:
    iVar2 = FontGlyph_GetLogicalSizeActiveRegs(glyphSubresource);
    uVar3 = extraout_ECX + iVar2;
    if (uVar5 < extraout_EDX_00) {
      uVar5 = extraout_EDX_00;
    }
    break;
  case 0x11:
    iVar2 = FontGlyph_GetLogicalSizeActiveRegs(0x2d);
    uVar3 = extraout_ECX_01;
    pbVar1 = pbVar7;
    if (maximumWidth < iVar2 + extraout_ECX_01)
    goto RichTextCommandStream_MeasureNextWrappedLine_CommitWrapBoundaryAndReturnHeight;
    break;
  case 0x12:
switchD_0041d140_caseD_12:
    if (uVar3 <= maximumWidth) {
      local_1c = pbVar7;
    }
RichTextCommandStream_MeasureNextWrappedLine_CommitWrapBoundaryAndReturnHeight:
    if (local_1c == (byte *)0x0) {
      local_1c = pbVar7;
    }
    g_RichTextRuntimeBufferUsedWords = (uint)((int)local_1c - (int)g_FontRuntimeBuffer) >> 1;
    return CONCAT44(in_EDX,uVar5);
  case 0x1a:
    qVar8 = (*g_GraphicsTextureSourceGetLogicalSize)
                      (*(dword *)(pbVar6 + 6),*(GraphicsTextureSourceAsset **)pbVar7);
    uVar4 = (uint)(qVar8 >> 0x20);
    uVar3 = extraout_ECX_00 + (int)qVar8;
    pbVar7 = pbVar6 + 10;
    if (uVar5 < uVar4) {
      uVar5 = uVar4;
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
undefined8
RichTextCommandStream_DrawNextWrappedLineCf
          (UiPixelCoordinate clipTop,UiPixelCoordinate clipLeft,UiPixelCoordinate clipBottom,
          UiPixelCoordinate clipRight,UiPixelExtent maximumWidth,UiPixelCoordinate drawY,
          UiPixelCoordinate drawX)

{
  byte *pbVar1;
  GraphicsSubresourceIndex GVar2;
  int iVar3;
  int iVar4;
  uint uVar5;
  int extraout_ECX;
  int extraout_ECX_00;
  uint extraout_ECX_01;
  uint extraout_ECX_02;
  undefined4 in_EDX;
  uint extraout_EDX;
  uint extraout_EDX_00;
  uint uVar6;
  uint lineTop;
  byte *pbVar7;
  byte *pbVar8;
  byte *pbVar9;
  qword qVar10;
  dword dVar11;
  byte *local_1c;
  
  FontGlyph_GetLogicalSizeActiveRegs(0);
  uVar5 = 0;
  pbVar9 = g_FontRuntimeBuffer + g_RichTextRuntimeBufferUsedWords * 2;
  local_1c = (byte *)0x0;
  lineTop = extraout_EDX;
  pbVar8 = pbVar9;
  dVar11 = g_ActiveFontIndex;
  pbVar1 = local_1c;
switchD_0041da40_caseD_0:
  while( true ) {
    local_1c = pbVar1;
    pbVar7 = pbVar8;
    GVar2 = (GraphicsSubresourceIndex)*(short *)pbVar7;
    pbVar8 = pbVar7 + 2;
    if (GVar2 != 0x20) break;
    iVar3 = FontGlyph_GetLogicalSizeActiveRegs(0x20);
    if (maximumWidth < extraout_ECX_02)
    goto RichTextCommandStream_DrawNextWrappedLine_CommitWrapBoundaryAndBeginDrawing;
    uVar5 = extraout_ECX_02 + iVar3;
    pbVar1 = pbVar8;
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
RichTextCommandStream_DrawNextWrappedLine_AccumulateGlyphExtent:
    iVar3 = FontGlyph_GetLogicalSizeActiveRegs(GVar2);
    uVar5 = extraout_ECX + iVar3;
    if (lineTop < extraout_EDX_00) {
      lineTop = extraout_EDX_00;
    }
    break;
  case 0x11:
    iVar3 = FontGlyph_GetLogicalSizeActiveRegs(0x2d);
    uVar5 = extraout_ECX_01;
    pbVar1 = pbVar8;
    if (maximumWidth < iVar3 + extraout_ECX_01)
    goto RichTextCommandStream_DrawNextWrappedLine_CommitWrapBoundaryAndBeginDrawing;
    break;
  case 0x12:
    goto switchD_0041da40_caseD_12;
  case 0x1a:
    qVar10 = (*g_GraphicsTextureSourceGetLogicalSize)
                       (*(dword *)(pbVar7 + 6),*(GraphicsTextureSourceAsset **)pbVar8);
    uVar6 = (uint)(qVar10 >> 0x20);
    uVar5 = extraout_ECX_00 + (int)qVar10;
    pbVar8 = pbVar7 + 10;
    if (lineTop < uVar6) {
      lineTop = uVar6;
    }
  }
  goto switchD_0041da40_caseD_0;
switchD_0041da40_caseD_12:
  if (uVar5 <= maximumWidth) {
    local_1c = pbVar8;
  }
RichTextCommandStream_DrawNextWrappedLine_CommitWrapBoundaryAndBeginDrawing:
  g_ActiveFontIndex = dVar11;
  if (local_1c == (byte *)0x0) {
    local_1c = pbVar8;
  }
  iVar3 = drawY + lineTop;
switchD_0041dbd0_caseD_7:
  pbVar8 = pbVar9;
  GVar2 = (GraphicsSubresourceIndex)*(short *)pbVar8;
  pbVar9 = pbVar8 + 2;
  if (GVar2 != 0x20) {
    if (GVar2 == 0) {
      g_RichTextRuntimeBufferUsedWords = (uint)((int)pbVar9 - (int)g_FontRuntimeBuffer) >> 1;
      return CONCAT44(in_EDX,lineTop);
    }
    if (-1 < (int)GVar2) goto RichTextCommandStream_DrawNextWrappedLine_DrawGlyphAndAdvanceX;
    goto code_r0x0041dbcd;
  }
  if (pbVar9 < local_1c) goto RichTextCommandStream_DrawNextWrappedLine_DrawGlyphAndAdvanceX;
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
         (((((((pbVar8[4] & 0xf) << 0x18 | (uint)*pbVar9 << 0x1c) >> 4 | (uint)pbVar8[8] << 0x1c) >>
             4 | (uint)pbVar8[6] << 0x1c) >> 4 | (uint)pbVar8[0xc] << 0x1c) >> 4 |
          (uint)pbVar8[10] << 0x1c) >> 4 | (uint)pbVar8[0x10] << 0x1c) >> 4 |
         (uint)pbVar8[0xe] << 0x1c;
    pbVar9 = pbVar8 + 0x12;
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
    iVar4 = FontGlyph_DrawVerticallyCentered
                      (clipTop,clipLeft,clipBottom,clipRight,GVar2,lineTop,iVar3,drawX);
    drawX = drawX + iVar4;
    break;
  case 0x11:
    if (local_1c <= pbVar9) {
      FontGlyph_DrawVerticallyCentered
                (clipTop,clipLeft,clipBottom,clipRight,0x2d,lineTop,iVar3,drawX);
      goto switchD_0041dbd0_caseD_12;
    }
    break;
  case 0x12:
switchD_0041dbd0_caseD_12:
    g_RichTextRuntimeBufferUsedWords = (uint)((int)pbVar9 - (int)g_FontRuntimeBuffer) >> 1;
    return CONCAT44(in_EDX,lineTop);
  case 0x1a:
    qVar10 = (*g_GraphicsTextureSourceGetLogicalSize)
                       (*(dword *)(pbVar8 + 6),*(GraphicsTextureSourceAsset **)pbVar9);
    iVar4 = (int)qVar10;
    (*g_GraphicsTextureSourceBlitSourceAlpha)
              (clipTop,clipLeft,clipBottom,clipRight,iVar3 - (int)(qVar10 >> 0x20),drawX,
               *(dword *)(pbVar8 + 6),*(GraphicsTextureSourceAsset **)pbVar9,g_FramebufferAccess);
    drawX = drawX + iVar4;
    pbVar9 = pbVar8 + 10;
  }
  goto switchD_0041dbd0_caseD_7;
}

/* Address: 0x0041D840.
   Ownership: assets/text/richtext.
   Purpose: Flattens nested rich-text command streams into the exact 0x2000-word runtime buffer, copies fixed color
   and image commands, follows nested stream pointers, writes a terminator, and resets the shared word index.
*/
void RichTextCommandStream_FlattenNestedToRuntimeBuffer(word *commandStream)

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

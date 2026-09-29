/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/src/assets/text/resources.c
 * Reverse engineering by idkFoxes 2026
 */

#include <thandor/assets/text/resources.h>
#include <thandor/thandor.h>
#include <thandor/platform/bootstrap/image.h>

/* Implementation ownership: assets/text/resources. */

/* Address: 0x0041CD30.
   Loads the level's own text page (the .str entry of a level package) as page 0x30 and makes its title,
   description and 14 further description lines reachable under the global ids the frontend uses for that
   level: TEXT_ID_LEVEL_TITLE_BASE + title index and TEXT_ID_LEVEL_DESCRIPTION_BASE + TEXT_ID_LEVEL_DESCRIPTION_STRIDE *
   title index (+1..14).
   CF is set when the page cannot be loaded or one of the strings is missing.
*/
bool TextResourcePage_LoadCompatibilityAliases(uint32_t levelTitleIndex,uint16_t *path)

{
  uint16_t *resolvedText;
  int lineIndex;
  bool failed;
  TextPageLoadResult pageLoadResult;
  TextResolveResult resolveResult;

  pageLoadResult = TextResourcePage_Load(TEXT_RESOURCE_PAGE_LEVEL,path);
  failed = pageLoadResult.failed;
  if (!failed) {
    resolveResult = TextResource_Resolve(TEXT_ID_LEVEL_PAGE_TITLE);
    failed = resolveResult.notFound;
    resolvedText = resolveResult.text;
    if (!failed) {
      TextResourceOverride_Register(levelTitleIndex + TEXT_ID_LEVEL_TITLE_BASE,resolvedText);
      resolveResult = TextResource_Resolve(TEXT_ID_LEVEL_PAGE_DESCRIPTION);
      failed = resolveResult.notFound;
      if (!failed) {
        lineIndex = TEXT_LEVEL_EXTRA_LINE_COUNT - 1;
        TextResourceOverride_Register(levelTitleIndex * TEXT_ID_LEVEL_DESCRIPTION_STRIDE + TEXT_ID_LEVEL_DESCRIPTION_BASE,
                                      resolveResult.text);
        /* the extra lines are registered from the last one down */
        do {
          resolveResult = TextResource_Resolve(lineIndex + TEXT_ID_LEVEL_PAGE_EXTRA_LINES);
          if (resolveResult.notFound) {
            return true;
          }
          TextResourceOverride_Register
                    (levelTitleIndex * TEXT_ID_LEVEL_DESCRIPTION_STRIDE + (TEXT_ID_LEVEL_DESCRIPTION_BASE + 1) + lineIndex,
                     resolveResult.text);
          lineIndex--;
        } while (-1 < lineIndex);
        failed = false;
      }
    }
  }
  return failed;
}


/* Address: 0x0041B080.
   Loads the two font texture sources from the consecutive UTF-16 paths at u_engine_font_gfx_0041b030, allocates
   the 16 KiB font runtime buffer (the flattened text of the wrapped-text functions) and the text-resource
   override table, and fills that whole table (ids and text pointers) with 0xFFFFFFFF. Any failure is fatal.
*/
void FontRuntime_Init(void)

{
  wchar_t pathChar;
  int scanLimitOrSlotCount;
  int remainingSources;
  GraphicsTextureSourceAsset **textureSourceSlot;
  wchar_t *pathUtf16;
  wchar_t *pathCursor;
  uint32_t *overrideDword;
  TextureSourceLoadResult textureLoadResult;
  FatalErrorCheckResult checkedResult;
  ArenaAllocResult allocResult;
  
  pathUtf16 = u_engine_font_gfx_0041b030;
  textureSourceSlot = g_FontTextureSources;
  remainingSources = 2;
  scanLimitOrSlotCount = 0x21; /* REPNE SCASW limit (0x42 >> 1 code units), shared by both path scans */
  do {
    textureLoadResult = g_GraphicsTextureSourceLoadPackageAsset((uint16_t *)pathUtf16);
    checkedResult = FatalError_ExitIfFailed((uint32_t)textureLoadResult.textureSource,textureLoadResult.failed);
    *textureSourceSlot = (GraphicsTextureSourceAsset *)checkedResult.valueOrError;
    /* step pathUtf16 past the terminator to the next path */
    pathCursor = pathUtf16;
    do {
      pathUtf16 = pathCursor;
      if (scanLimitOrSlotCount == 0) break;
      scanLimitOrSlotCount--;
      pathUtf16 = pathCursor + 1;
      pathChar = *pathCursor;
      pathCursor = pathUtf16;
    } while (pathChar != L'\0');
    textureSourceSlot++;
    remainingSources--;
  } while (remainingSources != 0);
  allocResult = g_MemoryApi.alloc(0x4000); /* 16 KiB font runtime buffer */
  checkedResult = FatalError_ExitIfFailed(allocResult.payloadOrError,allocResult.failed);
  g_FontRuntimeBuffer = (uint8_t *)checkedResult.valueOrError;
  allocResult = g_MemoryApi.alloc(sizeof(TextResourceOverrideTable));
  checkedResult = FatalError_ExitIfFailed(allocResult.payloadOrError,allocResult.failed);
  g_TextResourceOverrides = (TextResourceOverrideTable *)checkedResult.valueOrError;
  overrideDword = g_TextResourceOverrides->resourceIds;
  /* all dwords: resourceIds and textPointers. TextResourceOverride_Register looks for a zero id, so
     after this fill it finds no free slot (the original does the same: OR EAX,-1 / REP STOSD). */
  for (scanLimitOrSlotCount = sizeof(TextResourceOverrideTable) / 4; scanLimitOrSlotCount != 0;
       scanLimitOrSlotCount--) {
    *overrideDword = TEXT_RESOURCE_ID_NONE;
    overrideDword++;
  }
  return;
}


/* Address: 0x0041CCB0.
   Unloads text page pageIndex (the counterpart of TextResourcePage_Load): releases its 'str' asset and clears
   the page's binding (selected locale block and asset). No caller or table slot referencing it was found in
   src/ or src/generated/image_data.c.
*/
void TextResourcePage_Unload(TextResourcePageIndex pageIndex)

{
  Resource_Release(g_TextResourcePageBindings[pageIndex].asset);
  g_TextResourcePageBindings[pageIndex].selectedLocaleBlock = NULL;
  g_TextResourcePageBindings[pageIndex].asset = NULL;
  return;
}


/* Address: 0x0041CDB0.
   Returns the number of locale blocks (dword +0xB0) of a 'str' text asset with CF clear; CF set when the asset
   lacks the 'str' signature. No caller or table slot referencing it was found in src/ or
   src/generated/image_data.c.
*/
AssetRecordCount TextResourceAsset_GetLocaleBlockCount(TextResourceAssetHeader *asset)

{
  if ((asset->localeCountHeader).common.magic == ASSET_MAGIC_STR) {
    return (asset->localeCountHeader).localeBlockCount;
  }
  return 0; /* CF-set error path: the original leaves the caller's EAX (the function has no callers) */
}


/* Address: 0x0041CEB0.
   Returns the width of one glyph (EAX, 0 when the font has no such glyph) and the line height (EDX, the height
   of glyph 0) in the active font; used to measure text before it is laid out.
*/
GlyphSizeResult FontGlyph_GetLogicalSizeActiveRegs(GraphicsSubresourceIndex glyphSubresource)

{
  uint32_t glyphWidth;
  TextureSizeResult textureSize;
  GlyphSizeResult glyphSize;
  uint32_t fontIndex;
  
  fontIndex = g_ActiveFontIndex;
  textureSize = g_GraphicsTextureSourceGetLogicalSize
                    (glyphSubresource,g_FontTextureSources[g_ActiveFontIndex]);
  glyphWidth = textureSize.logicalWidthPixels;
  if (textureSize.failed) {
    glyphWidth = 0;
  }
  textureSize = g_GraphicsTextureSourceGetLogicalSize(0,g_FontTextureSources[fontIndex]);
  glyphSize.lineHeight = textureSize.logicalHeightPixels;
  glyphSize.width = glyphWidth;
  glyphSize.failed = false;
  return glyphSize;
}


/* Address: 0x0041CEF0.
   Returns the width of one glyph (EAX, 0 when the font has no such glyph) and the line height (EDX, the height
   of glyph 0) in the font selected by packedStyle, without changing the active font.
*/
GlyphSizeResult FontGlyph_GetLogicalSizeForStyleRegs
          (UiPackedTextStyle packedStyle,GraphicsSubresourceIndex glyphSubresource)

{
  uint32_t glyphWidth;
  uint32_t fontIndex;
  TextureSizeResult textureSize;
  GlyphSizeResult glyphSize;
  
  fontIndex = packedStyle >> TEXT_STYLE_FONT_SHIFT & TEXT_STYLE_INDEX_MASK;
  textureSize = g_GraphicsTextureSourceGetLogicalSize(glyphSubresource,g_FontTextureSources[fontIndex]);
  glyphWidth = textureSize.logicalWidthPixels;
  if (textureSize.failed) {
    glyphWidth = 0;
  }
  textureSize = g_GraphicsTextureSourceGetLogicalSize(0,g_FontTextureSources[fontIndex]);
  glyphSize.lineHeight = textureSize.logicalHeightPixels;
  glyphSize.width = glyphWidth;
  glyphSize.failed = false;
  return glyphSize;
}


/* Address: 0x0041D370.
   Draws one glyph of the active font with its bottom edge on baselineY at drawX, clipped to the given
   rectangle, in the current rich-text colour; with a shadow offset set, a half-transparent black copy is drawn
   first, shifted down and right. Returns the glyph width (0 without a loaded font) so the caller can advance.
*/
uint32_t FontGlyph_DrawBottomAligned
               (UiPixelCoordinate clipBottom,UiPixelCoordinate clipRight,UiPixelCoordinate clipTop,
               UiPixelCoordinate clipLeft,GraphicsSubresourceIndex glyphSubresource,
               UiPixelCoordinate baselineY,int32_t drawX)

{
  int drawY;
  TextureSizeResult textureSize;
  uint32_t colorArgb;
  GraphicsTextureSourceAsset *fontTexture;
  SoftwareFramebufferAccess *framebuffer;

  fontTexture = g_FontTextureSources[g_ActiveFontIndex];
  if (fontTexture != NULL) {
    textureSize = g_GraphicsTextureSourceGetLogicalSize(glyphSubresource,fontTexture);
    drawY = baselineY - textureSize.logicalHeightPixels;
    colorArgb = g_RichTextCurrentColorArgb;
    framebuffer = g_FramebufferAccess;
    if (g_RichTextCurrentShadowOffset != 0) {
      g_GraphicsTextureSourceBlitModulatedSourceAlpha
                (clipBottom,clipRight,clipTop,clipLeft,drawY + g_RichTextCurrentShadowOffset,
                 drawX + g_RichTextCurrentShadowOffset,TEXT_SHADOW_COLOR_ARGB,glyphSubresource,fontTexture,
                 g_FramebufferAccess);
    }
    g_GraphicsTextureSourceBlitModulatedSourceAlpha
              (clipBottom,clipRight,clipTop,clipLeft,drawY,drawX,colorArgb,glyphSubresource,fontTexture,framebuffer);
    /* EAX still holds the width from GetLogicalSize: both blits preserve EAX/ECX/EDX. */
    return textureSize.logicalWidthPixels;
  }
  return 0; /* EAX = fontTexture = NULL */
}


/* Address: 0x0041D400.
   Draws one glyph of the active font at drawX, centred vertically in the text line that ends at lineBottom and
   is lineHeight pixels high, clipped to the given rectangle, in the current rich-text colour (with the
   half-transparent shadow copy first when a shadow offset is set). Returns the glyph width (0 without a loaded
   font) so the caller can advance. Called directly by the wrapped-line drawing of rich text
   (assets/text/richtext.c) for glyphs, spaces and the wrap hyphen.
*/
uint32_t FontGlyph_DrawVerticallyCentered
               (UiPixelCoordinate clipBottom,UiPixelCoordinate clipRight,UiPixelCoordinate clipTop,
               UiPixelCoordinate clipLeft,GraphicsSubresourceIndex glyphSubresource,
               UiPixelCoordinate lineHeight,UiPixelCoordinate lineBottom,int32_t drawX)

{
  int drawY;
  TextureSizeResult textureSize;
  uint32_t colorArgb;
  GraphicsTextureSourceAsset *fontTexture;
  SoftwareFramebufferAccess *framebuffer;

  fontTexture = g_FontTextureSources[g_ActiveFontIndex];
  if (fontTexture != NULL) {
    textureSize = g_GraphicsTextureSourceGetLogicalSize(glyphSubresource,fontTexture);
    /* line top plus half the space the glyph leaves free */
    drawY = (lineBottom - lineHeight) + ((int)(lineHeight - textureSize.logicalHeightPixels) >> 1);
    colorArgb = g_RichTextCurrentColorArgb;
    framebuffer = g_FramebufferAccess;
    if (g_RichTextCurrentShadowOffset != 0) {
      g_GraphicsTextureSourceBlitModulatedSourceAlpha
                (clipBottom,clipRight,clipTop,clipLeft,drawY + g_RichTextCurrentShadowOffset,
                 drawX + g_RichTextCurrentShadowOffset,TEXT_SHADOW_COLOR_ARGB,glyphSubresource,fontTexture,
                 g_FramebufferAccess);
    }
    g_GraphicsTextureSourceBlitModulatedSourceAlpha
              (clipBottom,clipRight,clipTop,clipLeft,drawY,drawX,colorArgb,glyphSubresource,fontTexture,framebuffer);
    /* EAX still holds the width from GetLogicalSize: both blits preserve EAX/ECX/EDX. */
    return textureSize.logicalWidthPixels;
  }
  return 0; /* EAX = fontTexture = NULL */
}


/* Address: 0x0041CA50.
   Loads a 'str' text asset as page pageIndex: picks the locale block of the configured (or system) country,
   else the Great Britain block, else the first one, binds it, and prepares every string's command records for
   run time (see the switch). Returns the selected block with CF clear; a package error, or
   TEXT_RESOURCE_MISSING_SENTINEL_0x33 for a non-'str' asset (which is released), with CF set.
*/
TextPageLoadResult TextResourcePage_Load(TextResourcePageIndex pageIndex,uint16_t *path)

{
  uint16_t codeUnit;
  uint32_t packedHighDigits;
  TextResourceAssetHeader *allocation;
  TextResourceAssetHeader *localeBlockOrError;
  LocaleTelephoneCountryCode countryCode;
  AssetRecordCount remainingBlocks;
  TextResourceStringCount remainingStrings;
  uint16_t *recordStart;
  uint16_t *textCursor;
  int stringIndex;
  PackageLoadResult loadResult;
  TextPageLoadResult failureResult;
  TextPageLoadResult successResult;
  
  loadResult = Package_LoadEntry(path);
  if (loadResult.failed) {
    Thandor_Log("text page 0x%02X \"%ls\": load failed 0x%08X", pageIndex, (wchar_t *)path,
                (uint32_t)loadResult.bufferOrError);
  }
  allocation = loadResult.bufferOrError;
  localeBlockOrError = allocation;
  if (!loadResult.failed) {
    localeBlockOrError = (TextResourceAssetHeader *)TEXT_RESOURCE_MISSING_SENTINEL_0x33;
    if ((allocation->localeCountHeader).common.magic == ASSET_MAGIC_STR) {
      remainingBlocks = (allocation->localeCountHeader).localeBlockCount;
      countryCode = g_LocaleCountryCodeOverride;
      if (g_LocaleCountryCodeOverride == 0) {
        countryCode = g_LocaleGetDefaultTelephoneCountryCode();
      }
      /* Select the block for the country code, else the Great Britain block, else the first block. The blocks
         (TextResourceLocaleBlockPrefix) follow the 0x200-byte asset header; block + blockSizeBytes is the next
         block. */
      localeBlockOrError = allocation + 1;
      do {
        if (countryCode == ((TextResourceLocaleBlockPrefix *)localeBlockOrError)->countryCode) break;
        localeBlockOrError = (TextResourceAssetHeader *)
                 ((uint8_t *)localeBlockOrError + ((TextResourceLocaleBlockPrefix *)localeBlockOrError)->blockSizeBytes);
        remainingBlocks--;
      } while (remainingBlocks != 0);
      if (remainingBlocks == 0) {
        remainingBlocks = (allocation->localeCountHeader).localeBlockCount;
        localeBlockOrError = allocation + 1;
        do {
          if (((TextResourceLocaleBlockPrefix *)localeBlockOrError)->countryCode == LOCALE_COUNTRY_GREAT_BRITAIN) break;
          localeBlockOrError = (TextResourceAssetHeader *)
                   ((uint8_t *)localeBlockOrError + ((TextResourceLocaleBlockPrefix *)localeBlockOrError)->blockSizeBytes);
          remainingBlocks--;
        } while (remainingBlocks != 0);
        if (remainingBlocks == 0) {
          localeBlockOrError = allocation + 1;
        }
      }
      g_TextResourcePageBindings[pageIndex].selectedLocaleBlock =
           (TextResourceLocaleBlockPrefix *)localeBlockOrError;
      g_TextResourcePageBindings[pageIndex].asset = allocation;
      /* the string offsets (relative to the block) follow the 16-byte block prefix */
      stringIndex = 0;
      for (remainingStrings = ((TextResourceLocaleBlockPrefix *)localeBlockOrError)->stringCount; remainingStrings != 0;
          remainingStrings--) {
        textCursor = (uint16_t *)((uint8_t *)localeBlockOrError +
                                  ((uint32_t *)((TextResourceLocaleBlockPrefix *)localeBlockOrError + 1))[stringIndex]);
        while( true ) {
          recordStart = textCursor;
          codeUnit = *recordStart;
          textCursor = recordStart + 1;
          if (codeUnit == 0) break;
          if ((short)codeUnit < 0) {
            /* The converter stores the nested-stream selector and the image subresource as four UTF-16 decimal
               digits d0..d3 filling both payload dwords. They become a binary number (d0*1000 + d1*100 + d2*10
               + d3) in the second dword, and the first dword becomes the pointer slot: the missing-text stream
               for nested streams, NULL for images (bound later by RichTextCommandStream_BindTextureSource). */
            switch(codeUnit & RICHTEXT_OPCODE_MASK) {
            case RICHTEXT_OP_LITERAL_COLOR:
              textCursor = recordStart + RICHTEXT_RECORD_UNITS_LITERAL_COLOR;
              break;
            case RICHTEXT_OP_INLINE_VALUE_0:
            case RICHTEXT_OP_INLINE_VALUE_1:
            case RICHTEXT_OP_INLINE_VALUE_2:
              textCursor = recordStart + RICHTEXT_RECORD_UNITS_INLINE_VALUE;
              break;
            case RICHTEXT_OP_CALL_NESTED:
            case RICHTEXT_OP_JUMP_NESTED:
              packedHighDigits = *(uint32_t *)textCursor;
              *(uint32_t *)(recordStart + 3) =
                   (*(uint32_t *)(recordStart + 3) >> 0x10 & 0xf) + (*(uint32_t *)(recordStart + 3) & 0xf) * 10;
              *(void **)textCursor = &g_MissingTextResourceFallbackStream;
              *(uint32_t *)(recordStart + 3) =
                   *(int *)(recordStart + 3) + (packedHighDigits >> 0x10 & 0xf) * 100 + (packedHighDigits & 0xf) * 1000;
              textCursor = recordStart + RICHTEXT_RECORD_UNITS_NESTED;
              break;
            case RICHTEXT_OP_INLINE_IMAGE:
              /* The high digits are read before the pointer slot is cleared (0x0041CC23 MOV EBX,[ESI] precedes
                 0x0041CC3E MOV [ESI],0). */
              packedHighDigits = *(uint32_t *)textCursor;
              *(uint32_t *)(recordStart + 3) =
                   (*(uint32_t *)(recordStart + 3) >> 0x10 & 0xf) + (*(uint32_t *)(recordStart + 3) & 0xf) * 10;
              *(uint32_t *)textCursor = 0;
              *(uint32_t *)(recordStart + 3) =
                   *(int *)(recordStart + 3) + (packedHighDigits >> 0x10 & 0xf) * 100 + (packedHighDigits & 0xf) * 1000;
              textCursor = recordStart + RICHTEXT_RECORD_UNITS_INLINE_IMAGE;
            }
          }
        }
        stringIndex++;
      }
      successResult.failed = false;
      successResult.errorOrValue = (uint32_t)localeBlockOrError;
      return successResult;
    }
    Resource_Release(allocation);
  }
  failureResult.failed = true;
  failureResult.errorOrValue = (uint32_t)localeBlockOrError;
  return failureResult;
}


/* Address: 0x0041CCF0.
   Makes resourceId resolve to text (checked by TextResource_Resolve before the locale blocks) by storing the
   pair in the first override entry whose id is zero. Without an override table, or when it is full, nothing
   is registered.
*/
void TextResourceOverride_Register(TextResourceId resourceId,uint16_t *text)

{
  int overrideSlotsRemaining;
  TextResourceOverrideParallelWord4 *overrideWordScanCursor;
  TextResourceOverrideParallelWord4 *overrideWordCursorAfterScan;
  bool availableOverrideSlotFound;

  overrideSlotsRemaining = TEXT_RESOURCE_OVERRIDE_CAPACITY;
  availableOverrideSlotFound = g_TextResourceOverrides == NULL;
  overrideWordScanCursor = (TextResourceOverrideParallelWord4 *)g_TextResourceOverrides;
  if (!availableOverrideSlotFound) {
    /* REPNE SCASD over the id array for a zero id; the cursor ends one entry past the match */
    do {
      overrideWordCursorAfterScan = overrideWordScanCursor;
      if (overrideSlotsRemaining == 0) break;
      overrideSlotsRemaining--;
      overrideWordCursorAfterScan = overrideWordScanCursor + 1;
      availableOverrideSlotFound = overrideWordScanCursor->resourceId == 0;
      overrideWordScanCursor = overrideWordCursorAfterScan;
    } while (!availableOverrideSlotFound);
    if (availableOverrideSlotFound) {
      /* the text pointer array follows the id array, TEXT_RESOURCE_OVERRIDE_CAPACITY entries further on */
      overrideWordCursorAfterScan[-1].resourceId = resourceId;
      overrideWordCursorAfterScan[TEXT_RESOURCE_OVERRIDE_CAPACITY - 1].textPointer = text;
    }
  }
  return;
}


/* Address: 0x0041CDE0.
   Returns the text of a resource id: TEXT_RESOURCE_ID_NONE gives the shared empty string, then the runtime
   override table is searched, then the bound locale block of the id's page (compact or extended id, see
   resources.h). A missing text sets CF and returns the value TEXT_RESOURCE_MISSING_SENTINEL_0x33.
*/
TextResolveResult TextResource_Resolve(TextResourceId resourceId)

{
  TextResourceLocaleBlockPrefix *localeBlock;
  int remainingSlots;
  uint32_t *scanCursor;
  uint32_t *cursorAfterScan;
  bool overrideFound;
  TextResolveResult overrideResult;
  TextResolveResult compactResult;
  TextResolveResult extendedResult;
  TextResolveResult emptyResult;
  TextResolveResult missingResult;
  
  if (resourceId == TEXT_RESOURCE_ID_NONE) {
    emptyResult.text = (uint16_t *)THANDOR_ADDR(g_EmptyTextResourceUtf16,0);
    emptyResult.notFound = false;
    return emptyResult;
  }
  remainingSlots = TEXT_RESOURCE_OVERRIDE_CAPACITY;
  overrideFound = g_TextResourceOverrides == NULL;
  scanCursor = (uint32_t *)g_TextResourceOverrides;
  if (!overrideFound) {
    do {
      cursorAfterScan = scanCursor;
      if (remainingSlots == 0) break;
      remainingSlots--;
      cursorAfterScan = scanCursor + 1;
      overrideFound = resourceId == *scanCursor;
      scanCursor = cursorAfterScan;
    } while (!overrideFound);
    if (overrideFound) {
      overrideResult.notFound = false;
      /* cursorAfterScan is one past the matching id; its text pointer is TEXT_RESOURCE_OVERRIDE_CAPACITY
         dwords further, in textPointers */
      overrideResult.text = (uint16_t *)cursorAfterScan[TEXT_RESOURCE_OVERRIDE_CAPACITY - 1];
      return overrideResult;
    }
  }
  if ((resourceId & 0xff0000) == 0) {
    /* compact id: page << 8 | 8-bit index; the string offsets follow the 16-byte block prefix */
    localeBlock = g_TextResourcePageBindings[resourceId >> 8].selectedLocaleBlock;
    if ((localeBlock != NULL) &&
       ((resourceId & 0xff) < localeBlock->stringCount)) {
      /* the string offsets are relative to the block */
      compactResult.text =
           (uint16_t *)((uint8_t *)localeBlock + ((uint32_t *)(localeBlock + 1))[resourceId & 0xff]);
      compactResult.notFound = false;
      return compactResult;
    }
  }
  else {
    /* extended id: page << 16 | 16-bit index */
    localeBlock = g_TextResourcePageBindings[resourceId >> 16].selectedLocaleBlock;
    if ((localeBlock != NULL) &&
       ((resourceId & 0xffff) < localeBlock->stringCount)) {
      extendedResult.text =
           (uint16_t *)((uint8_t *)localeBlock + ((uint32_t *)(localeBlock + 1))[resourceId & 0xffff]);
      extendedResult.notFound = false;
      return extendedResult;
    }
  }
  Thandor_Log("text resource 0x%08X missing (page binding %p)", resourceId,
              g_TextResourcePageBindings[(resourceId & 0xff0000) == 0 ? resourceId >> 8 : resourceId >> 16].selectedLocaleBlock);
  missingResult.notFound = true;
  missingResult.text = (uint16_t *)&k_LowAddressLiteral00000033;
  return missingResult;
}


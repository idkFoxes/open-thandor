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
   Ownership: assets/text/resources.
   Purpose: The first argument supplies the runtime alias-address base used by those mappings. CF propagates load
   or resolve failure.
   Local calls: TextResourcePage_Load, TextResource_Resolve, TextResourceOverride_Register.
*/
bool __thandor_cf_preserve_eax_ecx_edx
TextResourcePage_LoadCompatibilityAliases(uint32_t aliasAddressBase,uint16_t *path)

{
  uint16_t *resolvedText;
  int aliasIndex;
  bool failed;
  TextPageLoadResult pageLoadResult;
  TextResolveResult resolveResult;
  
  pageLoadResult = TextResourcePage_Load(0x30,path);
  failed = pageLoadResult.failed;
  if (!failed) {
    resolveResult = TextResource_Resolve(0x3000);
    failed = resolveResult.notFound;
    resolvedText = resolveResult.text;
    if (!failed) {
      TextResourceOverride_Register(aliasAddressBase + 0x2230,resolvedText);
      resolveResult = TextResource_Resolve(0x3001);
      failed = resolveResult.notFound;
      if (!failed) {
        aliasIndex = 0xd;
        TextResourceOverride_Register(aliasAddressBase * 0x10 + 0x230010,resolveResult.text);
        do {
          resolveResult = TextResource_Resolve(aliasIndex + 0x3002);
          if (resolveResult.notFound) {
            return true;
          }
          TextResourceOverride_Register(aliasAddressBase * 0x10 + 0x230011 + aliasIndex,resolveResult.text);
          aliasIndex = aliasIndex + -1;
        } while (-1 < aliasIndex);
        failed = false;
      }
    }
  }
  return failed;
}


/* Address: 0x0041B080.
   Ownership: assets/text/resources.
   Purpose: Loads engine\font.gfx twice into a two-entry texture-source array, allocates a 0x4000-byte opaque
   runtime buffer, allocates an 8192-dword slot table, and fills every slot with 0xFFFFFFFF. The pass does not
   assign glyph-cache field semantics beyond those exact operations.
*/
void __thandor_void_preserve_eax_ecx_edx FontRuntime_Init(void)

{
  wchar_t pathChar;
  int scanLimitOrSlotCount;
  int remainingSources;
  GraphicsTextureSourceAsset **textureSourceSlot;
  wchar_t *pathUtf16;
  wchar_t *pathCursor;
  TextResourceOverrideTable *overrideSlot;
  TextureSourceLoadResult textureLoadResult;
  FatalErrorCheckResult checkedResult;
  ArenaAllocResult allocResult;
  
  pathUtf16 = u_engine_font_gfx_0041b030;
  textureSourceSlot = g_FontTextureSources;
  remainingSources = 2;
  scanLimitOrSlotCount = 0x21;
  do {
    textureLoadResult = (*g_GraphicsTextureSourceLoadPackageAsset)((uint16_t *)pathUtf16);
    checkedResult = FatalError_ExitIfFailed((uint32_t)textureLoadResult.textureSource,textureLoadResult.failed);
    *textureSourceSlot = (GraphicsTextureSourceAsset *)checkedResult.valueOrError;
    pathCursor = pathUtf16;
    do {
      pathUtf16 = pathCursor;
      if (scanLimitOrSlotCount == 0) break;
      scanLimitOrSlotCount = scanLimitOrSlotCount + -1;
      pathUtf16 = pathCursor + 1;
      pathChar = *pathCursor;
      pathCursor = pathUtf16;
    } while (pathChar != L'\0');
    textureSourceSlot = textureSourceSlot + 1;
    remainingSources = remainingSources + -1;
    if (remainingSources == 0) {
      allocResult = (*g_MemoryApi.alloc)(0x4000);
      checkedResult = FatalError_ExitIfFailed(allocResult.payloadOrError,allocResult.failed);
      g_FontRuntimeBuffer = (uint8_t *)checkedResult.valueOrError;
      allocResult = (*g_MemoryApi.alloc)(0x8000);
      checkedResult = FatalError_ExitIfFailed(allocResult.payloadOrError,allocResult.failed);
      g_TextResourceOverrides = (TextResourceOverrideTable *)checkedResult.valueOrError;
      overrideSlot = g_TextResourceOverrides;
      for (scanLimitOrSlotCount = 0x2000; scanLimitOrSlotCount != 0; scanLimitOrSlotCount = scanLimitOrSlotCount + -1) {
        overrideSlot->resourceIds[0] = 0xffffffff;
        overrideSlot = (TextResourceOverrideTable *)(overrideSlot->resourceIds + 1);
      }
      return;
    }
  } while( true );
}


/* Address: 0x0041CCB0.
   Ownership: assets/text/resources.
   Purpose: Releases the owning asset for one text-resource page and clears both dwords in its binding.
   Cross-module calls: Resource_Release [assets/resource/runtime].
*/
void __thandor_preserve_eax TextResourcePage_Unload(TextResourcePageIndex pageIndex)

{
  Resource_Release(g_TextResourcePageBindings[pageIndex].asset);
  g_TextResourcePageBindings[pageIndex].selectedLocaleBlock = (TextResourceLocaleBlockPrefix *)0x0;
  g_TextResourcePageBindings[pageIndex].asset = (TextResourceAssetHeader *)0x0;
  return;
}


/* Address: 0x0041CDB0.
   Ownership: assets/text/resources.
   Purpose: Validates the 'str' magic and returns localeBlockCount from +0xB0 with CF clear. Invalid input returns
   with CF set.
*/
AssetRecordCount __thandor_eax_cf_preserve_ecx_edx
TextResourceAsset_GetLocaleBlockCount(TextResourceAssetHeader *asset)

{
  if ((asset->localeCountHeader).common.magic == ASSET_MAGIC_STR) {
    return (asset->localeCountHeader).localeBlockCount;
  }
  return 0; /* CF-set error path: the original leaves the caller's EAX (the function has no callers) */
}


/* Address: 0x0041CEB0.
   Ownership: assets/text/resources.
   Purpose: Queries one glyph from the currently active font texture. Width is returned in EAX and line height in
   EDX. It selects an existing resource facet and does not imply sprite, model, or effect identity. Typed
   parameters: p0 glyphSubresource→GraphicsSubresourceIndex_V338. Calling convention, storage, body bytes, control
   flow, and executable data remain unchanged.
*/
GlyphSizeResult __thandor_eax_edx_cf_preserve_ecx
FontGlyph_GetLogicalSizeActiveRegs(GraphicsSubresourceIndex glyphSubresource)

{
  uint32_t glyphWidth;
  TextureSizeResult textureSize;
  GlyphSizeResult glyphSize;
  uint32_t fontIndex;
  
  fontIndex = g_ActiveFontIndex;
  textureSize = (*g_GraphicsTextureSourceGetLogicalSize)
                    (glyphSubresource,g_FontTextureSources[g_ActiveFontIndex]);
  glyphWidth = textureSize.logicalWidthPixels;
  if (textureSize.failed) {
    glyphWidth = 0;
  }
  textureSize = (*g_GraphicsTextureSourceGetLogicalSize)(0,g_FontTextureSources[fontIndex]);
  glyphSize.lineHeight = textureSize.logicalHeightPixels;
  glyphSize.width = glyphWidth;
  glyphSize.failed = false;
  return glyphSize;
}


/* Address: 0x0041CEF0.
   Ownership: assets/text/resources.
   Purpose: Selects the font from packedStyle bits 24-26 and returns glyph width in EAX and line height in EDX. It
   selects an existing resource facet and does not imply sprite, model, or effect identity. Typed parameters: p1
   glyphSubresource→GraphicsSubresourceIndex_V338. Calling convention, storage, body bytes, control flow, and
   executable data remain unchanged. Typed parameters: p0 packedStyle→UiPackedTextStyle_V301.
*/
GlyphSizeResult __thandor_eax_edx_cf_preserve_ecx
FontGlyph_GetLogicalSizeForStyleRegs
          (UiPackedTextStyle packedStyle,GraphicsSubresourceIndex glyphSubresource)

{
  uint32_t glyphWidth;
  uint32_t fontIndex;
  TextureSizeResult textureSize;
  GlyphSizeResult glyphSize;
  
  fontIndex = packedStyle >> 0x18 & 7;
  textureSize = (*g_GraphicsTextureSourceGetLogicalSize)(glyphSubresource,g_FontTextureSources[fontIndex])
  ;
  glyphWidth = textureSize.logicalWidthPixels;
  if (textureSize.failed) {
    glyphWidth = 0;
  }
  textureSize = (*g_GraphicsTextureSourceGetLogicalSize)(0,g_FontTextureSources[fontIndex]);
  glyphSize.lineHeight = textureSize.logicalHeightPixels;
  glyphSize.width = glyphWidth;
  glyphSize.failed = false;
  return glyphSize;
}


/* Address: 0x0041D370.
   Ownership: assets/text/resources.
   Purpose: EAX returns glyph width and CF is cleared. It selects an existing resource facet and does not imply
   sprite, model, or effect identity. Typed parameters: p4 glyphSubresource→GraphicsSubresourceIndex_V338. Calling
   convention, storage, body bytes, control flow, and executable data remain unchanged. Typed parameters: p0
   clipTop→UiPixelCoordinate_V297, p1 clipLeft→UiPixelCoordinate_V297, p2 clipBottom→UiPixelCoordinate_V297, p3
   clipRight→UiPixelCoordinate_V297, p5 baselineY→UiPixelCoordinate_V297.
*/
uint32_t FontGlyph_DrawBottomAligned
               (UiPixelCoordinate clipTop,UiPixelCoordinate clipLeft,UiPixelCoordinate clipBottom,
               UiPixelCoordinate clipRight,GraphicsSubresourceIndex glyphSubresource,
               UiPixelCoordinate baselineY,int32_t drawX)

{
  int drawY;
  TextureSizeResult textureSize;
  uint32_t colorArgb;
  GraphicsTextureSourceAsset *fontTexture;
  SoftwareFramebufferAccess *framebuffer;
  
  fontTexture = g_FontTextureSources[g_ActiveFontIndex];
  if (fontTexture != (GraphicsTextureSourceAsset *)0x0) {
    textureSize = (*g_GraphicsTextureSourceGetLogicalSize)(glyphSubresource,fontTexture);
    drawY = baselineY - textureSize.logicalHeightPixels;
    colorArgb = g_RichTextCurrentColorArgb;
    framebuffer = g_FramebufferAccess;
    if (g_RichTextCurrentShadowOffset != 0) {
      (*g_GraphicsTextureSourceBlitModulatedSourceAlpha)
                (clipTop,clipLeft,clipBottom,clipRight,drawY + g_RichTextCurrentShadowOffset,
                 drawX + g_RichTextCurrentShadowOffset,0x7f000000,glyphSubresource,fontTexture,
                 g_FramebufferAccess);
    }
    (*g_GraphicsTextureSourceBlitModulatedSourceAlpha)
              (clipTop,clipLeft,clipBottom,clipRight,drawY,drawX,colorArgb,glyphSubresource,fontTexture,framebuffer);
    /* EAX still holds the width from GetLogicalSize: both blits preserve EAX/ECX/EDX. */
    return textureSize.logicalWidthPixels;
  }
  return 0; /* EAX = fontTexture = NULL */
}


/* Address: 0x0041D400.
   Ownership: assets/text/resources.
   Purpose: EAX returns glyph width and CF is cleared. It selects an existing resource facet and does not imply
   sprite, model, or effect identity. Typed parameters: p4 glyphSubresource→GraphicsSubresourceIndex_V338. Calling
   convention, storage, body bytes, control flow, and executable data remain unchanged. Typed parameters: p0
   clipTop→UiPixelCoordinate_V297, p1 clipLeft→UiPixelCoordinate_V297, p2 clipBottom→UiPixelCoordinate_V297, p3
   clipRight→UiPixelCoordinate_V297, p5 lineTop→UiPixelCoordinate_V297, p6 lineBottom→UiPixelCoordinate_V297.
*/
uint32_t FontGlyph_DrawVerticallyCentered
               (UiPixelCoordinate clipTop,UiPixelCoordinate clipLeft,UiPixelCoordinate clipBottom,
               UiPixelCoordinate clipRight,GraphicsSubresourceIndex glyphSubresource,
               UiPixelCoordinate lineTop,UiPixelCoordinate lineBottom,int32_t drawX)

{
  int drawY;
  TextureSizeResult textureSize;
  uint32_t colorArgb;
  GraphicsTextureSourceAsset *fontTexture;
  SoftwareFramebufferAccess *framebuffer;
  
  fontTexture = g_FontTextureSources[g_ActiveFontIndex];
  if (fontTexture != (GraphicsTextureSourceAsset *)0x0) {
    textureSize = (*g_GraphicsTextureSourceGetLogicalSize)(glyphSubresource,fontTexture);
    drawY = (lineBottom - lineTop) + ((int)(lineTop - textureSize.logicalHeightPixels) >> 1);
    colorArgb = g_RichTextCurrentColorArgb;
    framebuffer = g_FramebufferAccess;
    if (g_RichTextCurrentShadowOffset != 0) {
      (*g_GraphicsTextureSourceBlitModulatedSourceAlpha)
                (clipTop,clipLeft,clipBottom,clipRight,drawY + g_RichTextCurrentShadowOffset,
                 drawX + g_RichTextCurrentShadowOffset,0x7f000000,glyphSubresource,fontTexture,
                 g_FramebufferAccess);
    }
    (*g_GraphicsTextureSourceBlitModulatedSourceAlpha)
              (clipTop,clipLeft,clipBottom,clipRight,drawY,drawX,colorArgb,glyphSubresource,fontTexture,framebuffer);
    /* EAX still holds the width from GetLogicalSize: both blits preserve EAX/ECX/EDX. */
    return textureSize.logicalWidthPixels;
  }
  return 0; /* EAX = fontTexture = NULL */
}


/* Address: 0x0041CA50.
   Ownership: assets/text/resources.
   Purpose: Loads a generated 'str' asset, chooses a locale block by the configured or current country code, falls
   back to country code 44 and then to the first block, stores the selected block plus owning allocation in one of
   256 page bindings, and normalizes verified inline control records. CF clear indicates success; CF set returns a
   package or format error in EAX.
   Cross-module calls: Package_LoadEntry [assets/package/runtime], Resource_Release [assets/resource/runtime].
*/
TextPageLoadResult __thandor_eax_cf_preserve_ecx_edx
TextResourcePage_Load(TextResourcePageIndex pageIndex,uint16_t *path)

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
    localeBlockOrError = (TextResourceAssetHeader *)0x33;
    if ((allocation->localeCountHeader).common.magic == ASSET_MAGIC_STR) {
      remainingBlocks = (allocation->localeCountHeader).localeBlockCount;
      countryCode = g_LocaleCountryCodeOverride;
      if (g_LocaleCountryCodeOverride == 0) {
        countryCode = (*g_LocaleGetDefaultTelephoneCountryCode)();
      }
      /* Select the block for the country code, else the Great Britain block, else the first block. */
      localeBlockOrError = allocation + 1;
      do {
        if (countryCode == (localeBlockOrError->localeCountHeader).common.formatVersion) break;
        localeBlockOrError = (TextResourceAssetHeader *)
                 ((localeBlockOrError->localeCountHeader).common.buildMetadata.assetRelativeAddressAnchor28 +
                 ((localeBlockOrError->localeCountHeader).common.magic - 0x28));
        remainingBlocks = remainingBlocks - 1;
      } while (remainingBlocks != 0);
      if (remainingBlocks == 0) {
        remainingBlocks = (allocation->localeCountHeader).localeBlockCount;
        localeBlockOrError = allocation + 1;
        do {
          if ((localeBlockOrError->localeCountHeader).common.formatVersion == LOCALE_COUNTRY_GREAT_BRITAIN) break;
          localeBlockOrError = (TextResourceAssetHeader *)
                   ((localeBlockOrError->localeCountHeader).common.buildMetadata.assetRelativeAddressAnchor28 +
                   ((localeBlockOrError->localeCountHeader).common.magic - 0x28));
          remainingBlocks = remainingBlocks - 1;
        } while (remainingBlocks != 0);
        if (remainingBlocks == 0) {
          localeBlockOrError = allocation + 1;
        }
      }
      g_TextResourcePageBindings[pageIndex].selectedLocaleBlock =
           (TextResourceLocaleBlockPrefix *)localeBlockOrError;
      g_TextResourcePageBindings[pageIndex].asset = allocation;
      stringIndex = 0;
      for (remainingStrings = (localeBlockOrError->localeCountHeader).common.allocationSizeBytes; remainingStrings != 0;
          remainingStrings = remainingStrings - 1) {
        textCursor = (uint16_t *)((localeBlockOrError->localeCountHeader).common.buildMetadata.
                          assetRelativeAddressAnchor28 +
                         *(int *)((localeBlockOrError->localeCountHeader).common.buildMetadata.
                                  assetRelativeAddressAnchor28 + stringIndex * 4 + -0x18) + -0x28);
        while( true ) {
          recordStart = textCursor;
          codeUnit = *recordStart;
          textCursor = recordStart + 1;
          if (codeUnit == 0) break;
          if ((short)codeUnit < 0) {
            switch(codeUnit & 0x1f) {
            case 6:
              textCursor = recordStart + 9;
              break;
            case 0x14:
            case 0x15:
            case 0x16:
              textCursor = recordStart + 3;
              break;
            case 0x18:
            case 0x19:
              packedHighDigits = *(uint32_t *)textCursor;
              *(uint32_t *)(recordStart + 3) =
                   (*(uint32_t *)(recordStart + 3) >> 0x10 & 0xf) + (*(uint32_t *)(recordStart + 3) & 0xf) * 10;
              *(void **)textCursor = &g_MissingTextResourceFallbackStream;
              *(uint32_t *)(recordStart + 3) =
                   *(int *)(recordStart + 3) + (packedHighDigits >> 0x10 & 0xf) * 100 + (packedHighDigits & 0xf) * 1000;
              textCursor = recordStart + 5;
              break;
            case 0x1a:
              /* Like 0x18/0x19, but the pointer slot is cleared. The high digits are read before the
                 clear (0x0041CC23 MOV EBX,[ESI] precedes 0x0041CC3E MOV [ESI],0). */
              packedHighDigits = *(uint32_t *)textCursor;
              *(uint32_t *)(recordStart + 3) =
                   (*(uint32_t *)(recordStart + 3) >> 0x10 & 0xf) + (*(uint32_t *)(recordStart + 3) & 0xf) * 10;
              *(uint32_t *)textCursor = 0;
              *(uint32_t *)(recordStart + 3) =
                   *(int *)(recordStart + 3) + (packedHighDigits >> 0x10 & 0xf) * 100 + (packedHighDigits & 0xf) * 1000;
              textCursor = recordStart + 5;
            }
          }
        }
        stringIndex = stringIndex + 1;
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
   Ownership: assets/text/resources.
   Purpose: A null table or full table leaves state unchanged. Kept distinct from glyph/subresource selectors and
   localized string pointers. Typed parameters: p0 resourceId→TextResourceId_V338. Calling convention, storage,
   body bytes, control flow, and executable data remain unchanged.
*/
void __thandor_void_preserve_eax_ecx_edx
TextResourceOverride_Register(TextResourceId resourceId,uint16_t *text)

{
  int overrideSlotsRemaining;
  TextResourceOverrideParallelWord4 *overrideWordScanCursor;
  TextResourceOverrideParallelWord4 *overrideWordCursorAfterScan;
  bool availableOverrideSlotFound;
  
  overrideSlotsRemaining = 0x1000;
  availableOverrideSlotFound = g_TextResourceOverrides == (TextResourceOverrideTable *)0x0;
  overrideWordScanCursor = (TextResourceOverrideParallelWord4 *)g_TextResourceOverrides;
  if (!availableOverrideSlotFound) {
    do {
      overrideWordCursorAfterScan = overrideWordScanCursor;
      if (overrideSlotsRemaining == 0) break;
      overrideSlotsRemaining = overrideSlotsRemaining + -1;
      overrideWordCursorAfterScan = overrideWordScanCursor + 1;
      availableOverrideSlotFound = overrideWordScanCursor->resourceId == 0;
      overrideWordScanCursor = overrideWordCursorAfterScan;
    } while (!availableOverrideSlotFound);
    if (availableOverrideSlotFound) {
      overrideWordCursorAfterScan[-1].resourceId = resourceId;
      overrideWordCursorAfterScan[0xfff].textPointer = text;
    }
  }
  return;
}


/* Address: 0x0041CDE0.
   Ownership: assets/text/resources.
   Purpose: Resolves one resource identifier. Runtime overrides are checked first, followed by the selected page
   binding. Compact IDs use an 8-bit page and 8-bit index; extended IDs use an 8-bit page and 16-bit index.
   Identifier 0xFFFFFFFF returns the shared empty string. Missing resources return error 0x33 with CF set.
*/
TextResolveResult __thandor_eax_cf_preserve_ecx_edx
TextResource_Resolve(TextResourceId resourceId)

{
  TextResourceLocaleBlockPrefix *localeBlock;
  int remainingSlots;
  TextResourceOverrideTable *scanCursor;
  TextResourceOverrideTable *cursorAfterScan;
  bool overrideFound;
  TextResolveResult overrideResult;
  TextResolveResult compactResult;
  TextResolveResult extendedResult;
  TextResolveResult emptyResult;
  TextResolveResult missingResult;
  
  if (resourceId == 0xffffffff) {
    emptyResult.text = (uint16_t *)THANDOR_ADDR(g_EmptyTextResourceUtf16,0);
    emptyResult.notFound = false;
    return emptyResult;
  }
  remainingSlots = 0x1000;
  overrideFound = g_TextResourceOverrides == (TextResourceOverrideTable *)0x0;
  scanCursor = g_TextResourceOverrides;
  if (!overrideFound) {
    do {
      cursorAfterScan = scanCursor;
      if (remainingSlots == 0) break;
      remainingSlots = remainingSlots + -1;
      cursorAfterScan = (TextResourceOverrideTable *)(scanCursor->resourceIds + 1);
      overrideFound = resourceId == scanCursor->resourceIds[0];
      scanCursor = cursorAfterScan;
    } while (!overrideFound);
    if (overrideFound) {
      overrideResult.notFound = false;
      overrideResult.text = (uint16_t *)cursorAfterScan->resourceIds[0xfff];
      return overrideResult;
    }
  }
  if ((resourceId & 0xff0000) == 0) {
    localeBlock = g_TextResourcePageBindings[resourceId >> 8].selectedLocaleBlock;
    if ((localeBlock != (TextResourceLocaleBlockPrefix *)0x0) &&
       ((resourceId & 0xff) < localeBlock->stringCount)) {
      compactResult.text = (int)&localeBlock->blockSizeBytes + (&localeBlock[1].blockSizeBytes)[resourceId & 0xff];
      compactResult.notFound = false;
      return compactResult;
    }
  }
  else {
    localeBlock = g_TextResourcePageBindings[resourceId >> 0x10].selectedLocaleBlock;
    if ((localeBlock != (TextResourceLocaleBlockPrefix *)0x0) &&
       ((resourceId & 0xffff) < localeBlock->stringCount)) {
      extendedResult.text = (int)&localeBlock->blockSizeBytes + (&localeBlock[1].blockSizeBytes)[resourceId & 0xffff];
      extendedResult.notFound = false;
      return extendedResult;
    }
  }
  Thandor_Log("text resource 0x%08X missing (page binding %p)", resourceId,
              g_TextResourcePageBindings[(resourceId & 0xff0000) == 0 ? resourceId >> 8 : resourceId >> 0x10].selectedLocaleBlock);
  missingResult.notFound = true;
  missingResult.text = (uint16_t *)&k_LowAddressLiteral00000033;
  return missingResult;
}


/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/src/assets/text/resources.c
 * Reverse engineering by idkFoxes 2026
 */

#include <thandor/assets/text/resources.h>

/* Implementation ownership: assets/text/resources. */

/* Address: 0x0041CD30.
   Ownership: assets/text/resources.
   Purpose: The first argument supplies the runtime alias-address base used by those mappings. CF propagates load
   or resolve failure.
   Local calls: TextResourcePage_Load, TextResource_Resolve, TextResourceOverride_Register.
*/
bool __thandor_cf_preserve_eax_ecx_edx
TextResourcePage_LoadCompatibilityAliases(dword aliasAddressBase,word *path)

{
  word *resolvedText;
  int aliasIndex;
  bool bVar1;
  TextResourceLoadEaxCf5 TVar2;
  TextResourceResolveEaxCf5 TVar3;
  
  TVar2 = TextResourcePage_Load(0x30,path);
  bVar1 = TVar2.carry;
  if (!bVar1) {
    TVar3 = TextResource_Resolve(0x3000);
    bVar1 = TVar3.carry;
    resolvedText = TVar3.eax;
    if (!bVar1) {
      TextResourceOverride_Register(aliasAddressBase + 0x2230,resolvedText);
      TVar3 = TextResource_Resolve(0x3001);
      bVar1 = TVar3.carry;
      if (!bVar1) {
        aliasIndex = 0xd;
        TextResourceOverride_Register(aliasAddressBase * 0x10 + 0x230010,TVar3.eax);
        do {
          TVar3 = TextResource_Resolve(aliasIndex + 0x3002);
          if (TVar3.carry) {
            return true;
          }
          TextResourceOverride_Register(aliasAddressBase * 0x10 + 0x230011 + aliasIndex,TVar3.eax);
          aliasIndex = aliasIndex + -1;
        } while (-1 < aliasIndex);
        bVar1 = false;
      }
    }
  }
  return bVar1;
}


/* Address: 0x0041B080.
   Ownership: assets/text/resources.
   Purpose: Loads engine\font.gfx twice into a two-entry texture-source array, allocates a 0x4000-byte opaque
   runtime buffer, allocates an 8192-dword slot table, and fills every slot with 0xFFFFFFFF. The pass does not
   assign glyph-cache field semantics beyond those exact operations.
*/
void __thandor_void_preserve_eax_ecx_edx FontRuntime_Init(void)

{
  wchar_t wVar1;
  int iVar2;
  int iVar3;
  GraphicsTextureSourceAsset **ppGVar4;
  wchar_t *pathUtf16;
  wchar_t *pwVar5;
  TextResourceOverrideTable *pTVar6;
  GraphicsTextureSourceLoadEaxCf5 GVar7;
  FatalErrorEaxCf5 FVar8;
  ArenaAllocEaxCf5 AVar9;
  
  pathUtf16 = u_engine_font_gfx_0041b030;
  ppGVar4 = g_FontTextureSources;
  iVar3 = 2;
  iVar2 = 0x21;
  do {
    GVar7 = (*g_GraphicsTextureSourceLoadPackageAsset)((word *)pathUtf16);
    FVar8 = (*g_FatalErrorPrimaryDispatchCf)((dword)GVar7.eax,GVar7.carry);
    *ppGVar4 = (GraphicsTextureSourceAsset *)FVar8.eax;
    pwVar5 = pathUtf16;
    do {
      pathUtf16 = pwVar5;
      if (iVar2 == 0) break;
      iVar2 = iVar2 + -1;
      pathUtf16 = pwVar5 + 1;
      wVar1 = *pwVar5;
      pwVar5 = pathUtf16;
    } while (wVar1 != L'\0');
    ppGVar4 = ppGVar4 + 1;
    iVar3 = iVar3 + -1;
    if (iVar3 == 0) {
      AVar9 = (*g_MemoryApi.alloc)(0x4000);
      FVar8 = (*g_FatalErrorPrimaryDispatchCf)(AVar9.eax,AVar9.carry);
      g_FontRuntimeBuffer = (byte *)FVar8.eax;
      AVar9 = (*g_MemoryApi.alloc)(0x8000);
      FVar8 = (*g_FatalErrorPrimaryDispatchCf)(AVar9.eax,AVar9.carry);
      g_TextResourceOverrides = (TextResourceOverrideTable *)FVar8.eax;
      pTVar6 = g_TextResourceOverrides;
      for (iVar2 = 0x2000; iVar2 != 0; iVar2 = iVar2 + -1) {
        pTVar6->resourceIds[0] = 0xffffffff;
        pTVar6 = (TextResourceOverrideTable *)(pTVar6->resourceIds + 1);
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
  AssetRecordCount in_EAX;
  
  if ((asset->localeCountHeader).common.magic == ASSET_MAGIC_STR) {
    return (asset->localeCountHeader).localeBlockCount;
  }
  return in_EAX;
}


/* Address: 0x0041CEB0.
   Ownership: assets/text/resources.
   Purpose: Queries one glyph from the currently active font texture. Width is returned in EAX and line height in
   EDX. It selects an existing resource facet and does not imply sprite, model, or effect identity. Typed
   parameters: p0 glyphSubresource→GraphicsSubresourceIndex_V338. Calling convention, storage, body bytes, control
   flow, and executable data remain unchanged.
*/
FontGlyphSizeEaxEdxCf9 __thandor_eax_edx_cf_preserve_ecx
FontGlyph_GetLogicalSizeActiveRegs(GraphicsSubresourceIndex glyphSubresource)

{
  dword dVar1;
  GraphicsTextureSizeEaxEdxCf9 GVar2;
  FontGlyphSizeEaxEdxCf9 FVar3;
  dword fontIndex;
  
  fontIndex = g_ActiveFontIndex;
  GVar2 = (*g_GraphicsTextureSourceGetLogicalSize)
                    (glyphSubresource,g_FontTextureSources[g_ActiveFontIndex]);
  dVar1 = GVar2.logicalWidthPixels;
  if (GVar2.carry) {
    dVar1 = 0;
  }
  GVar2 = (*g_GraphicsTextureSourceGetLogicalSize)(0,g_FontTextureSources[fontIndex]);
  FVar3.lineHeight = GVar2.logicalHeightPixels;
  FVar3.width = dVar1;
  FVar3.carry = false;
  return FVar3;
}


/* Address: 0x0041CEF0.
   Ownership: assets/text/resources.
   Purpose: Selects the font from packedStyle bits 24-26 and returns glyph width in EAX and line height in EDX. It
   selects an existing resource facet and does not imply sprite, model, or effect identity. Typed parameters: p1
   glyphSubresource→GraphicsSubresourceIndex_V338. Calling convention, storage, body bytes, control flow, and
   executable data remain unchanged. Typed parameters: p0 packedStyle→UiPackedTextStyle_V301.
*/
FontGlyphSizeEaxEdxCf9 __thandor_eax_edx_cf_preserve_ecx
FontGlyph_GetLogicalSizeForStyleRegs
          (UiPackedTextStyle packedStyle,GraphicsSubresourceIndex glyphSubresource)

{
  dword dVar1;
  uint fontIndex;
  GraphicsTextureSizeEaxEdxCf9 GVar2;
  FontGlyphSizeEaxEdxCf9 FVar3;
  
  fontIndex = packedStyle >> 0x18 & 7;
  GVar2 = (*g_GraphicsTextureSourceGetLogicalSize)(glyphSubresource,g_FontTextureSources[fontIndex])
  ;
  dVar1 = GVar2.logicalWidthPixels;
  if (GVar2.carry) {
    dVar1 = 0;
  }
  GVar2 = (*g_GraphicsTextureSourceGetLogicalSize)(0,g_FontTextureSources[fontIndex]);
  FVar3.lineHeight = GVar2.logicalHeightPixels;
  FVar3.width = dVar1;
  FVar3.carry = false;
  return FVar3;
}


/* Address: 0x0041D370.
   Ownership: assets/text/resources.
   Purpose: EAX returns glyph width and CF is cleared. It selects an existing resource facet and does not imply
   sprite, model, or effect identity. Typed parameters: p4 glyphSubresource→GraphicsSubresourceIndex_V338. Calling
   convention, storage, body bytes, control flow, and executable data remain unchanged. Typed parameters: p0
   clipTop→UiPixelCoordinate_V297, p1 clipLeft→UiPixelCoordinate_V297, p2 clipBottom→UiPixelCoordinate_V297, p3
   clipRight→UiPixelCoordinate_V297, p5 baselineY→UiPixelCoordinate_V297.
*/
void FontGlyph_DrawBottomAligned
               (UiPixelCoordinate clipTop,UiPixelCoordinate clipLeft,UiPixelCoordinate clipBottom,
               UiPixelCoordinate clipRight,GraphicsSubresourceIndex glyphSubresource,
               UiPixelCoordinate baselineY,sdword drawX)

{
  int arg4;
  GraphicsTextureSizeEaxEdxCf9 GVar1;
  dword arg6;
  GraphicsTextureSourceAsset *arg1;
  SoftwareFramebufferAccess *arg9;
  
  arg1 = g_FontTextureSources[g_ActiveFontIndex];
  if (arg1 != (GraphicsTextureSourceAsset *)0x0) {
    GVar1 = (*g_GraphicsTextureSourceGetLogicalSize)(glyphSubresource,arg1);
    arg4 = baselineY - GVar1.logicalHeightPixels;
    arg6 = g_RichTextCurrentColorArgb;
    arg9 = g_FramebufferAccess;
    if (g_RichTextCurrentShadowOffset != 0) {
      (*g_GraphicsTextureSourceBlitModulatedSourceAlpha)
                (clipTop,clipLeft,clipBottom,clipRight,arg4 + g_RichTextCurrentShadowOffset,
                 drawX + g_RichTextCurrentShadowOffset,0x7f000000,glyphSubresource,arg1,
                 g_FramebufferAccess);
    }
    (*g_GraphicsTextureSourceBlitModulatedSourceAlpha)
              (clipTop,clipLeft,clipBottom,clipRight,arg4,drawX,arg6,glyphSubresource,arg1,arg9);
  }
  return;
}


/* Address: 0x0041D400.
   Ownership: assets/text/resources.
   Purpose: EAX returns glyph width and CF is cleared. It selects an existing resource facet and does not imply
   sprite, model, or effect identity. Typed parameters: p4 glyphSubresource→GraphicsSubresourceIndex_V338. Calling
   convention, storage, body bytes, control flow, and executable data remain unchanged. Typed parameters: p0
   clipTop→UiPixelCoordinate_V297, p1 clipLeft→UiPixelCoordinate_V297, p2 clipBottom→UiPixelCoordinate_V297, p3
   clipRight→UiPixelCoordinate_V297, p5 lineTop→UiPixelCoordinate_V297, p6 lineBottom→UiPixelCoordinate_V297.
*/
void FontGlyph_DrawVerticallyCentered
               (UiPixelCoordinate clipTop,UiPixelCoordinate clipLeft,UiPixelCoordinate clipBottom,
               UiPixelCoordinate clipRight,GraphicsSubresourceIndex glyphSubresource,
               UiPixelCoordinate lineTop,UiPixelCoordinate lineBottom,sdword drawX)

{
  int arg4;
  GraphicsTextureSizeEaxEdxCf9 GVar1;
  dword arg6;
  GraphicsTextureSourceAsset *arg1;
  SoftwareFramebufferAccess *arg9;
  
  arg1 = g_FontTextureSources[g_ActiveFontIndex];
  if (arg1 != (GraphicsTextureSourceAsset *)0x0) {
    GVar1 = (*g_GraphicsTextureSourceGetLogicalSize)(glyphSubresource,arg1);
    arg4 = (lineBottom - lineTop) + ((int)(lineTop - GVar1.logicalHeightPixels) >> 1);
    arg6 = g_RichTextCurrentColorArgb;
    arg9 = g_FramebufferAccess;
    if (g_RichTextCurrentShadowOffset != 0) {
      (*g_GraphicsTextureSourceBlitModulatedSourceAlpha)
                (clipTop,clipLeft,clipBottom,clipRight,arg4 + g_RichTextCurrentShadowOffset,
                 drawX + g_RichTextCurrentShadowOffset,0x7f000000,glyphSubresource,arg1,
                 g_FramebufferAccess);
    }
    (*g_GraphicsTextureSourceBlitModulatedSourceAlpha)
              (clipTop,clipLeft,clipBottom,clipRight,arg4,drawX,arg6,glyphSubresource,arg1,arg9);
  }
  return;
}


/* Address: 0x0041CA50.
   Ownership: assets/text/resources.
   Purpose: Loads a generated 'str' asset, chooses a locale block by the configured or current country code, falls
   back to country code 44 and then to the first block, stores the selected block plus owning allocation in one of
   256 page bindings, and normalizes verified inline control records. CF clear indicates success; CF set returns a
   package or format error in EAX.
   Cross-module calls: Package_LoadEntry [assets/package/runtime], Resource_Release [assets/resource/runtime].
*/
TextResourceLoadEaxCf5 __thandor_eax_cf_preserve_ecx_edx
TextResourcePage_Load(TextResourcePageIndex pageIndex,word *path)

{
  ushort uVar1;
  uint uVar2;
  TextResourceAssetHeader *allocation;
  TextResourceAssetHeader *pTVar3;
  LocaleTelephoneCountryCode LVar4;
  AssetRecordCount AVar5;
  TextResourceStringCount TVar6;
  word *pwVar7;
  word *pwVar8;
  int iVar9;
  PackageLoadEntryEaxCf5 PVar10;
  TextResourceLoadEaxCf5 TVar11;
  TextResourceLoadEaxCf5 TVar12;
  
  PVar10 = Package_LoadEntry(path);
  allocation = PVar10.bufferOrError;
  pTVar3 = allocation;
  if (!PVar10.carry) {
    pTVar3 = (TextResourceAssetHeader *)&k_LowAddressLiteral00000033;
    if ((allocation->localeCountHeader).common.magic == ASSET_MAGIC_STR) {
      AVar5 = (allocation->localeCountHeader).localeBlockCount;
      LVar4 = g_LocaleCountryCodeOverride;
      if (g_LocaleCountryCodeOverride == 0) {
        LVar4 = (*g_LocaleGetDefaultTelephoneCountryCode)();
      }
      pTVar3 = allocation + 1;
      do {
        if (LVar4 == (pTVar3->localeCountHeader).common.formatVersion)
        goto TextResourcePage_Load_BindSelectedLocaleBlockAndPatchEmbeddedReferences;
        pTVar3 = (TextResourceAssetHeader *)
                 ((pTVar3->localeCountHeader).common.buildMetadata.assetRelativeAddressAnchor28 +
                 ((pTVar3->localeCountHeader).common.magic - 0x28));
        AVar5 = AVar5 - 1;
      } while (AVar5 != 0);
      AVar5 = (allocation->localeCountHeader).localeBlockCount;
      pTVar3 = allocation + 1;
      do {
        if ((pTVar3->localeCountHeader).common.formatVersion == LOCALE_COUNTRY_GREAT_BRITAIN)
        goto TextResourcePage_Load_BindSelectedLocaleBlockAndPatchEmbeddedReferences;
        pTVar3 = (TextResourceAssetHeader *)
                 ((pTVar3->localeCountHeader).common.buildMetadata.assetRelativeAddressAnchor28 +
                 ((pTVar3->localeCountHeader).common.magic - 0x28));
        AVar5 = AVar5 - 1;
      } while (AVar5 != 0);
      pTVar3 = allocation + 1;
TextResourcePage_Load_BindSelectedLocaleBlockAndPatchEmbeddedReferences:
      g_TextResourcePageBindings[pageIndex].selectedLocaleBlock =
           (TextResourceLocaleBlockPrefix *)pTVar3;
      g_TextResourcePageBindings[pageIndex].asset = allocation;
      iVar9 = 0;
      for (TVar6 = (pTVar3->localeCountHeader).common.allocationSizeBytes; TVar6 != 0;
          TVar6 = TVar6 - 1) {
        pwVar8 = (word *)((pTVar3->localeCountHeader).common.buildMetadata.
                          assetRelativeAddressAnchor28 +
                         *(int *)((pTVar3->localeCountHeader).common.buildMetadata.
                                  assetRelativeAddressAnchor28 + iVar9 * 4 + -0x18) + -0x28);
        while( true ) {
          pwVar7 = pwVar8;
          uVar1 = *pwVar7;
          pwVar8 = pwVar7 + 1;
          if (uVar1 == 0) break;
          if ((short)uVar1 < 0) {
            switch(uVar1 & 0x1f) {
            case 6:
              pwVar8 = pwVar7 + 9;
              break;
            case 0x14:
            case 0x15:
            case 0x16:
              pwVar8 = pwVar7 + 3;
              break;
            case 0x18:
            case 0x19:
              uVar2 = *(uint *)pwVar8;
              *(uint *)(pwVar7 + 3) =
                   (*(uint *)(pwVar7 + 3) >> 0x10 & 0xf) + (*(uint *)(pwVar7 + 3) & 0xf) * 10;
              *(undefined **)pwVar8 = &g_MissingTextResourceFallbackStream;
              *(uint *)(pwVar7 + 3) =
                   *(int *)(pwVar7 + 3) + (uVar2 >> 0x10 & 0xf) * 100 + (uVar2 & 0xf) * 1000;
              pwVar8 = pwVar7 + 5;
              break;
            case 0x1a:
              *(uint *)(pwVar7 + 3) =
                   (*(uint *)(pwVar7 + 3) >> 0x10 & 0xf) + (*(uint *)(pwVar7 + 3) & 0xf) * 10;
              pwVar8[0] = 0;
              pwVar8[1] = 0;
              *(uint *)(pwVar7 + 3) =
                   *(int *)(pwVar7 + 3) +
                   (*(uint *)pwVar8 >> 0x10 & 0xf) * 100 + (*(uint *)pwVar8 & 0xf) * 1000;
              pwVar8 = pwVar7 + 5;
            }
          }
        }
        iVar9 = iVar9 + 1;
      }
      TVar12.carry = false;
      TVar12.errorOrValue = (dword)pTVar3;
      return TVar12;
    }
    Resource_Release(allocation);
  }
  TVar11.carry = true;
  TVar11.errorOrValue = (dword)pTVar3;
  return TVar11;
}


/* Address: 0x0041CCF0.
   Ownership: assets/text/resources.
   Purpose: A null table or full table leaves state unchanged. Kept distinct from glyph/subresource selectors and
   localized string pointers. Typed parameters: p0 resourceId→TextResourceId_V338. Calling convention, storage,
   body bytes, control flow, and executable data remain unchanged.
*/
void __thandor_void_preserve_eax_ecx_edx
TextResourceOverride_Register(TextResourceId resourceId,word *text)

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
TextResourceResolveEaxCf5 __thandor_eax_cf_preserve_ecx_edx
TextResource_Resolve(TextResourceId resourceId)

{
  TextResourceLocaleBlockPrefix *pTVar1;
  int iVar2;
  TextResourceOverrideTable *pTVar3;
  TextResourceOverrideTable *pTVar4;
  bool bVar5;
  TextResourceResolveEaxCf5 TVar6;
  TextResourceResolveEaxCf5 TVar7;
  TextResourceResolveEaxCf5 TVar8;
  TextResourceResolveEaxCf5 TVar9;
  TextResourceResolveEaxCf5 TVar10;
  
  if (resourceId == 0xffffffff) {
    TVar9.eax = (word *)0x41afa4;
    TVar9.carry = false;
    return TVar9;
  }
  iVar2 = 0x1000;
  bVar5 = g_TextResourceOverrides == (TextResourceOverrideTable *)0x0;
  pTVar3 = g_TextResourceOverrides;
  if (!bVar5) {
    do {
      pTVar4 = pTVar3;
      if (iVar2 == 0) break;
      iVar2 = iVar2 + -1;
      pTVar4 = (TextResourceOverrideTable *)(pTVar3->resourceIds + 1);
      bVar5 = resourceId == pTVar3->resourceIds[0];
      pTVar3 = pTVar4;
    } while (!bVar5);
    if (bVar5) {
      TVar6.carry = false;
      TVar6.eax = (word *)pTVar4->resourceIds[0xfff];
      return TVar6;
    }
  }
  if ((resourceId & 0xff0000) == 0) {
    pTVar1 = g_TextResourcePageBindings[resourceId >> 8].selectedLocaleBlock;
    if ((pTVar1 != (TextResourceLocaleBlockPrefix *)0x0) &&
       ((resourceId & 0xff) < pTVar1->stringCount)) {
      TVar7.eax = (int)&pTVar1->blockSizeBytes + (&pTVar1[1].blockSizeBytes)[resourceId & 0xff];
      TVar7.carry = false;
      return TVar7;
    }
  }
  else {
    pTVar1 = g_TextResourcePageBindings[resourceId >> 0x10].selectedLocaleBlock;
    if ((pTVar1 != (TextResourceLocaleBlockPrefix *)0x0) &&
       ((resourceId & 0xffff) < pTVar1->stringCount)) {
      TVar8.eax = (int)&pTVar1->blockSizeBytes + (&pTVar1[1].blockSizeBytes)[resourceId & 0xffff];
      TVar8.carry = false;
      return TVar8;
    }
  }
  TVar10.carry = true;
  TVar10.eax = (word *)&k_LowAddressLiteral00000033;
  return TVar10;
}


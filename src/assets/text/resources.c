#include <thandor/assets/text/resources.h>

/* Implementation ownership: assets/text/resources. */

/* Address: 0x0041CD30.
   Ownership: assets/text/resources.
   Purpose: The first argument supplies the runtime alias-address base used by those mappings. CF propagates load
   or resolve failure.
   Local calls: TextResourcePage_Load, TextResource_Resolve, TextResourceOverride_Register.
*/
void TextResourcePage_LoadCompatibilityAliases(dword aliasAddressBase,word *path)

{
  word *resolvedText;
  word *pwVar1;
  int extraout_ECX;
  int extraout_ECX_00;
  int extraout_ECX_01;
  int extraout_ECX_02;
  int aliasIndex;
  undefined1 in_CF;
  undefined1 uVar2;
  
  TextResourcePage_Load(0x30,path);
  if ((!(bool)in_CF) && (resolvedText = TextResource_Resolve(0x3000), !(bool)in_CF)) {
    TextResourceOverride_Register(extraout_ECX + 0x2230,resolvedText);
    uVar2 = extraout_ECX_00 << 3 < 0;
    pwVar1 = TextResource_Resolve(0x3001);
    if (!(bool)uVar2) {
      aliasIndex = 0xd;
      TextResourceOverride_Register(extraout_ECX_01 + 0x230010,pwVar1);
      do {
        pwVar1 = TextResource_Resolve(aliasIndex + 0x3002);
        if ((bool)uVar2) {
          return;
        }
        TextResourceOverride_Register(extraout_ECX_02 + 0x230011 + aliasIndex,pwVar1);
        aliasIndex = aliasIndex + -1;
      } while (-1 < aliasIndex);
    }
  }
  return;
}

/* Address: 0x0041B080.
   Ownership: assets/text/resources.
   Purpose: Loads engine\font.gfx twice into a two-entry texture-source array, allocates a 0x4000-byte opaque
   runtime buffer, allocates an 8192-dword slot table, and fills every slot with 0xFFFFFFFF. The pass does not
   assign glyph-cache field semantics beyond those exact operations.
*/
void FontRuntime_Init(void)

{
  wchar_t wVar1;
  dword arg0;
  dword extraout_ECX;
  int iVar2;
  dword arg1;
  GraphicsTextureSourceAsset **ppGVar3;
  wchar_t *arg2;
  wchar_t *pwVar4;
  TextResourceOverrideTable *pTVar5;
  undefined8 uVar6;
  
  arg2 = u_engine_font_gfx_0041b030;
  ppGVar3 = g_FontTextureSources;
  arg1 = 2;
  arg0 = 0x21;
  do {
    (*g_GraphicsTextureSourceLoadPackageAsset)(arg0,arg1,(word *)arg2);
    uVar6 = (*g_FatalErrorPrimaryDispatchCf)();
    *ppGVar3 = (GraphicsTextureSourceAsset *)uVar6;
    arg0 = extraout_ECX;
    pwVar4 = arg2;
    do {
      arg2 = pwVar4;
      if (arg0 == 0) break;
      arg0 = arg0 - 1;
      arg2 = pwVar4 + 1;
      wVar1 = *pwVar4;
      pwVar4 = arg2;
    } while (wVar1 != L'\0');
    ppGVar3 = ppGVar3 + 1;
    arg1 = (int)((ulonglong)uVar6 >> 0x20) - 1;
    if (arg1 == 0) {
      (*g_MemoryApi.alloc)(0x4000);
      g_FontRuntimeBuffer = (byte *)(*g_FatalErrorPrimaryDispatchCf)();
      (*g_MemoryApi.alloc)(0x8000);
      g_TextResourceOverrides = (TextResourceOverrideTable *)(*g_FatalErrorPrimaryDispatchCf)();
      pTVar5 = g_TextResourceOverrides;
      for (iVar2 = 0x2000; iVar2 != 0; iVar2 = iVar2 + -1) {
        pTVar5->resourceIds[0] = 0xffffffff;
        pTVar5 = (TextResourceOverrideTable *)(pTVar5->resourceIds + 1);
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
void TextResourcePage_Unload(TextResourcePageIndex pageIndex)

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
AssetRecordCount TextResourceAsset_GetLocaleBlockCount(TextResourceAssetHeader *asset)

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
undefined4 FontGlyph_GetLogicalSizeActiveRegs(GraphicsSubresourceIndex glyphSubresource)

{
  undefined4 uVar1;
  dword fontIndex;
  
  fontIndex = g_ActiveFontIndex;
  (*g_GraphicsTextureSourceGetLogicalSize)(glyphSubresource,g_FontTextureSources[g_ActiveFontIndex])
  ;
  uVar1 = 0;
  (*g_GraphicsTextureSourceGetLogicalSize)(0,g_FontTextureSources[fontIndex]);
  return uVar1;
}

/* Address: 0x0041CEF0.
   Ownership: assets/text/resources.
   Purpose: Selects the font from packedStyle bits 24-26 and returns glyph width in EAX and line height in EDX. It
   selects an existing resource facet and does not imply sprite, model, or effect identity. Typed parameters: p1
   glyphSubresource→GraphicsSubresourceIndex_V338. Calling convention, storage, body bytes, control flow, and
   executable data remain unchanged. Typed parameters: p0 packedStyle→UiPackedTextStyle_V301.
*/
undefined4
FontGlyph_GetLogicalSizeForStyleRegs
          (UiPackedTextStyle packedStyle,GraphicsSubresourceIndex glyphSubresource)

{
  uint fontIndex;
  undefined4 uVar1;
  
  fontIndex = packedStyle >> 0x18 & 7;
  (*g_GraphicsTextureSourceGetLogicalSize)(glyphSubresource,g_FontTextureSources[fontIndex]);
  uVar1 = 0;
  (*g_GraphicsTextureSourceGetLogicalSize)(0,g_FontTextureSources[fontIndex]);
  return uVar1;
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
  int extraout_ECX;
  int arg4;
  qword qVar1;
  int arg5;
  dword arg6;
  GraphicsTextureSourceAsset *arg1;
  SoftwareFramebufferAccess *arg9;
  
  arg1 = g_FontTextureSources[g_ActiveFontIndex];
  if (arg1 != (GraphicsTextureSourceAsset *)0x0) {
    qVar1 = (*g_GraphicsTextureSourceGetLogicalSize)(glyphSubresource,arg1);
    arg4 = baselineY - (int)(qVar1 >> 0x20);
    arg5 = extraout_ECX;
    arg6 = g_RichTextCurrentColorArgb;
    arg9 = g_FramebufferAccess;
    if (g_RichTextCurrentShadowOffset != 0) {
      (*g_GraphicsTextureSourceBlitModulatedSourceAlpha)
                (clipTop,clipLeft,clipBottom,clipRight,arg4 + g_RichTextCurrentShadowOffset,
                 extraout_ECX + g_RichTextCurrentShadowOffset,0x7f000000,glyphSubresource,arg1,
                 g_FramebufferAccess);
    }
    (*g_GraphicsTextureSourceBlitModulatedSourceAlpha)
              (clipTop,clipLeft,clipBottom,clipRight,arg4,arg5,arg6,glyphSubresource,arg1,arg9);
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
  int extraout_ECX;
  int arg4;
  qword qVar1;
  int arg5;
  dword arg6;
  GraphicsTextureSourceAsset *arg1;
  SoftwareFramebufferAccess *arg9;
  
  arg1 = g_FontTextureSources[g_ActiveFontIndex];
  if (arg1 != (GraphicsTextureSourceAsset *)0x0) {
    qVar1 = (*g_GraphicsTextureSourceGetLogicalSize)(glyphSubresource,arg1);
    arg4 = (lineBottom - lineTop) + (lineTop - (int)(qVar1 >> 0x20) >> 1);
    arg5 = extraout_ECX;
    arg6 = g_RichTextCurrentColorArgb;
    arg9 = g_FramebufferAccess;
    if (g_RichTextCurrentShadowOffset != 0) {
      (*g_GraphicsTextureSourceBlitModulatedSourceAlpha)
                (clipTop,clipLeft,clipBottom,clipRight,arg4 + g_RichTextCurrentShadowOffset,
                 extraout_ECX + g_RichTextCurrentShadowOffset,0x7f000000,glyphSubresource,arg1,
                 g_FramebufferAccess);
    }
    (*g_GraphicsTextureSourceBlitModulatedSourceAlpha)
              (clipTop,clipLeft,clipBottom,clipRight,arg4,arg5,arg6,glyphSubresource,arg1,arg9);
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
void TextResourcePage_Load(TextResourcePageIndex pageIndex,word *path)

{
  ushort uVar1;
  uint uVar2;
  TextResourceAssetHeader *allocation;
  LocaleTelephoneCountryCode LVar3;
  AssetRecordCount extraout_ECX;
  AssetRecordCount AVar4;
  TextResourceStringCount TVar5;
  TextResourceAssetHeader *pTVar6;
  word *pwVar7;
  word *pwVar8;
  int iVar9;
  bool bVar10;
  
  bVar10 = &stack0xffffffe8 < (undefined1 *)0x4;
  allocation = Package_LoadEntry(path);
  if (!bVar10) {
    if ((allocation->localeCountHeader).common.magic == ASSET_MAGIC_STR) {
      AVar4 = (allocation->localeCountHeader).localeBlockCount;
      LVar3 = g_LocaleCountryCodeOverride;
      if (g_LocaleCountryCodeOverride == 0) {
        LVar3 = (*g_LocaleGetDefaultTelephoneCountryCode)();
        AVar4 = extraout_ECX;
      }
      pTVar6 = allocation + 1;
      do {
        if (LVar3 == (pTVar6->localeCountHeader).common.formatVersion)
        goto TextResourcePage_Load_BindSelectedLocaleBlockAndPatchEmbeddedReferences;
        pTVar6 = (TextResourceAssetHeader *)
                 ((pTVar6->localeCountHeader).common.buildMetadata.assetRelativeAddressAnchor28 +
                 ((pTVar6->localeCountHeader).common.magic - 0x28));
        AVar4 = AVar4 - 1;
      } while (AVar4 != 0);
      AVar4 = (allocation->localeCountHeader).localeBlockCount;
      pTVar6 = allocation + 1;
      do {
        if ((pTVar6->localeCountHeader).common.formatVersion == LOCALE_COUNTRY_GREAT_BRITAIN)
        goto TextResourcePage_Load_BindSelectedLocaleBlockAndPatchEmbeddedReferences;
        pTVar6 = (TextResourceAssetHeader *)
                 ((pTVar6->localeCountHeader).common.buildMetadata.assetRelativeAddressAnchor28 +
                 ((pTVar6->localeCountHeader).common.magic - 0x28));
        AVar4 = AVar4 - 1;
      } while (AVar4 != 0);
      pTVar6 = allocation + 1;
TextResourcePage_Load_BindSelectedLocaleBlockAndPatchEmbeddedReferences:
      g_TextResourcePageBindings[pageIndex].selectedLocaleBlock =
           (TextResourceLocaleBlockPrefix *)pTVar6;
      g_TextResourcePageBindings[pageIndex].asset = allocation;
      iVar9 = 0;
      for (TVar5 = (pTVar6->localeCountHeader).common.allocationSizeBytes; TVar5 != 0;
          TVar5 = TVar5 - 1) {
        pwVar8 = (word *)((pTVar6->localeCountHeader).common.buildMetadata.
                          assetRelativeAddressAnchor28 +
                         *(int *)((pTVar6->localeCountHeader).common.buildMetadata.
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
      return;
    }
    Resource_Release(allocation);
  }
  return;
}

/* Address: 0x0041CCF0.
   Ownership: assets/text/resources.
   Purpose: A null table or full table leaves state unchanged. Kept distinct from glyph/subresource selectors and
   localized string pointers. Typed parameters: p0 resourceId→TextResourceId_V338. Calling convention, storage,
   body bytes, control flow, and executable data remain unchanged.
*/
void TextResourceOverride_Register(TextResourceId resourceId,word *text)

{
  int overrideSlotsRemaining;
  TextResourceOverrideTable *overrideSlotCursor;
  TextResourceOverrideTable *candidateOverrideSlot;
  bool availableOverrideSlotFound;
  
  overrideSlotsRemaining = 0x1000;
  availableOverrideSlotFound = g_TextResourceOverrides == (TextResourceOverrideTable *)0x0;
  overrideSlotCursor = g_TextResourceOverrides;
  if (!availableOverrideSlotFound) {
    do {
      candidateOverrideSlot = overrideSlotCursor;
      if (overrideSlotsRemaining == 0) break;
      overrideSlotsRemaining = overrideSlotsRemaining + -1;
      candidateOverrideSlot = (TextResourceOverrideTable *)(overrideSlotCursor->resourceIds + 1);
      availableOverrideSlotFound = overrideSlotCursor->resourceIds[0] == 0;
      overrideSlotCursor = candidateOverrideSlot;
    } while (!availableOverrideSlotFound);
    if (availableOverrideSlotFound) {
      candidateOverrideSlot[-1].textPointers[0xfff] = (word *)resourceId;
      candidateOverrideSlot->resourceIds[0xfff] = (dword)text;
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
word * TextResource_Resolve(TextResourceId resourceId)

{
  TextResourceLocaleBlockPrefix *pTVar1;
  int iVar2;
  TextResourceOverrideTable *pTVar3;
  TextResourceOverrideTable *pTVar4;
  bool bVar5;
  
  if (resourceId == 0xffffffff) {
    return (word *)&g_EmptyTextResourceUtf16;
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
      return (word *)pTVar4->resourceIds[0xfff];
    }
  }
  if ((resourceId & 0xff0000) == 0) {
    pTVar1 = g_TextResourcePageBindings[resourceId >> 8].selectedLocaleBlock;
    if ((pTVar1 != (TextResourceLocaleBlockPrefix *)0x0) &&
       ((resourceId & 0xff) < pTVar1->stringCount)) {
      return (word *)((int)&pTVar1->blockSizeBytes + (&pTVar1[1].blockSizeBytes)[resourceId & 0xff])
      ;
    }
  }
  else {
    pTVar1 = g_TextResourcePageBindings[resourceId >> 0x10].selectedLocaleBlock;
    if ((pTVar1 != (TextResourceLocaleBlockPrefix *)0x0) &&
       ((resourceId & 0xffff) < pTVar1->stringCount)) {
      return (word *)((int)&pTVar1->blockSizeBytes +
                     (&pTVar1[1].blockSizeBytes)[resourceId & 0xffff]);
    }
  }
  return (word *)&k_LowAddressLiteral00000033;
}

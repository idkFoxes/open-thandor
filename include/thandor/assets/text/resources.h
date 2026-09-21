/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/include/thandor/assets/text/resources.h
 * Reverse engineering by idkFoxes 2026
 */

#ifndef THANDOR_ASSETS_TEXT_RESOURCES_H
#define THANDOR_ASSETS_TEXT_RESOURCES_H

#include <thandor/generated/types.h>
#include <thandor/core/contracts.h>

/* Submodule: assets/text/resources. */
/* Functions are grouped by semantic ownership; address comments are executable virtual addresses. */

/* 0x0041CD30 */
bool __thandor_cf_preserve_eax_ecx_edx
TextResourcePage_LoadCompatibilityAliases(dword aliasAddressBase,word *path);

/* 0x0041B080 */
void __thandor_void_preserve_eax_ecx_edx FontRuntime_Init(void);

/* 0x0041CCB0 */
void __thandor_preserve_eax TextResourcePage_Unload(TextResourcePageIndex pageIndex);

/* 0x0041CDB0 */
AssetRecordCount __thandor_eax_cf_preserve_ecx_edx
TextResourceAsset_GetLocaleBlockCount(TextResourceAssetHeader *asset);

/* 0x0041CEB0 */
FontGlyphSizeEaxEdxCf9 __thandor_eax_edx_cf_preserve_ecx
FontGlyph_GetLogicalSizeActiveRegs(GraphicsSubresourceIndex glyphSubresource);

/* 0x0041CEF0 */
FontGlyphSizeEaxEdxCf9 __thandor_eax_edx_cf_preserve_ecx
FontGlyph_GetLogicalSizeForStyleRegs
          (UiPackedTextStyle packedStyle,GraphicsSubresourceIndex glyphSubresource);

/* 0x0041D370 */
void FontGlyph_DrawBottomAligned (UiPixelCoordinate clipTop,UiPixelCoordinate clipLeft,UiPixelCoordinate clipBottom, UiPixelCoordinate clipRight,GraphicsSubresourceIndex glyphSubresource, UiPixelCoordinate baselineY,sdword drawX);

/* 0x0041D400 */
void FontGlyph_DrawVerticallyCentered (UiPixelCoordinate clipTop,UiPixelCoordinate clipLeft,UiPixelCoordinate clipBottom, UiPixelCoordinate clipRight,GraphicsSubresourceIndex glyphSubresource, UiPixelCoordinate lineTop,UiPixelCoordinate lineBottom,sdword drawX);

/* 0x0041CA50 */
TextResourceLoadEaxCf5 __thandor_eax_cf_preserve_ecx_edx
TextResourcePage_Load(TextResourcePageIndex pageIndex,word *path);

/* 0x0041CCF0 */
void __thandor_void_preserve_eax_ecx_edx
TextResourceOverride_Register(TextResourceId resourceId,word *text);

/* 0x0041CDE0 */
TextResourceResolveEaxCf5 __thandor_eax_cf_preserve_ecx_edx
TextResource_Resolve(TextResourceId resourceId);

#endif /* THANDOR_ASSETS_TEXT_RESOURCES_H */

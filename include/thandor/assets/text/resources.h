#ifndef THANDOR_ASSETS_TEXT_RESOURCES_H
#define THANDOR_ASSETS_TEXT_RESOURCES_H

#include <thandor/generated/v523_types.h>
#include <thandor/core/contracts.h>

/* Submodule: assets/text/resources. */
/* Functions are grouped by semantic ownership; address comments are executable virtual addresses. */

/* 0x0041CD30 */
void TextResourcePage_LoadCompatibilityAliases(dword aliasAddressBase,word *path);

/* 0x0041B080 */
void FontRuntime_Init(void);

/* 0x0041CCB0 */
void TextResourcePage_Unload(TextResourcePageIndex pageIndex);

/* 0x0041CDB0 */
AssetRecordCount TextResourceAsset_GetLocaleBlockCount(TextResourceAssetHeader *asset);

/* 0x0041CEB0 */
undefined4 FontGlyph_GetLogicalSizeActiveRegs(GraphicsSubresourceIndex glyphSubresource);

/* 0x0041CEF0 */
undefined4 FontGlyph_GetLogicalSizeForStyleRegs (UiPackedTextStyle packedStyle,GraphicsSubresourceIndex glyphSubresource);

/* 0x0041D370 */
void FontGlyph_DrawBottomAligned (UiPixelCoordinate clipTop,UiPixelCoordinate clipLeft,UiPixelCoordinate clipBottom, UiPixelCoordinate clipRight,GraphicsSubresourceIndex glyphSubresource, UiPixelCoordinate baselineY,sdword drawX);

/* 0x0041D400 */
void FontGlyph_DrawVerticallyCentered (UiPixelCoordinate clipTop,UiPixelCoordinate clipLeft,UiPixelCoordinate clipBottom, UiPixelCoordinate clipRight,GraphicsSubresourceIndex glyphSubresource, UiPixelCoordinate lineTop,UiPixelCoordinate lineBottom,sdword drawX);

/* 0x0041CA50 */
void TextResourcePage_Load(TextResourcePageIndex pageIndex,word *path);

/* 0x0041CCF0 */
void TextResourceOverride_Register(TextResourceId resourceId,word *text);

/* 0x0041CDE0 */
word * TextResource_Resolve(TextResourceId resourceId);

#endif /* THANDOR_ASSETS_TEXT_RESOURCES_H */

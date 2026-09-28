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

/* Text resource ids: a compact id (bits 16-23 zero) is page << 8 | index, an extended id page << 16 | index
   with a 16-bit index. TEXT_RESOURCE_ID_NONE resolves to the shared empty string; FontRuntime_Init
   fills the whole override table with it. */
#define TEXT_RESOURCE_ID_NONE 0xFFFFFFFF
#define TEXT_RESOURCE_OVERRIDE_CAPACITY 0x1000 /* entries of TextResourceOverrideTable */
/* Page 0x30 holds the text of the loaded level (its .str entry, see TextResourcePage_LoadCompatibilityAliases):
   index 0 the title, 1 the description, 2..15 fourteen further description lines. */
#define TEXT_RESOURCE_PAGE_LEVEL 0x30
#define TEXT_ID_LEVEL_PAGE_TITLE 0x3000
#define TEXT_ID_LEVEL_PAGE_DESCRIPTION 0x3001
#define TEXT_ID_LEVEL_PAGE_EXTRA_LINES 0x3002
#define TEXT_LEVEL_EXTRA_LINE_COUNT 14
/* Colour of the drop shadow drawn under glyphs and UI icons: black at half alpha (ARGB). */
#ifndef TEXT_SHADOW_COLOR_ARGB
#define TEXT_SHADOW_COLOR_ARGB 0x7F000000
#endif
/* Functions are grouped by semantic ownership; address comments are executable virtual addresses. */

/* 0x0041CD30 */
bool TextResourcePage_LoadCompatibilityAliases(uint32_t levelTitleIndex,uint16_t *path);

/* 0x0041B080 */
void FontRuntime_Init(void);

/* 0x0041CCB0 */
void TextResourcePage_Unload(TextResourcePageIndex pageIndex);

/* 0x0041CDB0 */
AssetRecordCount TextResourceAsset_GetLocaleBlockCount(TextResourceAssetHeader *asset);

/* 0x0041CEB0 */
GlyphSizeResult FontGlyph_GetLogicalSizeActiveRegs(GraphicsSubresourceIndex glyphSubresource);

/* 0x0041CEF0 */
GlyphSizeResult FontGlyph_GetLogicalSizeForStyleRegs
          (UiPackedTextStyle packedStyle,GraphicsSubresourceIndex glyphSubresource);

/* 0x0041D370 */
uint32_t FontGlyph_DrawBottomAligned (UiPixelCoordinate clipTop,UiPixelCoordinate clipLeft,UiPixelCoordinate clipBottom, UiPixelCoordinate clipRight,GraphicsSubresourceIndex glyphSubresource, UiPixelCoordinate baselineY,int32_t drawX);

/* 0x0041D400 */
uint32_t FontGlyph_DrawVerticallyCentered (UiPixelCoordinate clipTop,UiPixelCoordinate clipLeft,UiPixelCoordinate clipBottom, UiPixelCoordinate clipRight,GraphicsSubresourceIndex glyphSubresource, UiPixelCoordinate lineHeight,UiPixelCoordinate lineBottom,int32_t drawX);

/* 0x0041CA50 */
TextPageLoadResult TextResourcePage_Load(TextResourcePageIndex pageIndex,uint16_t *path);

/* 0x0041CCF0 */
void TextResourceOverride_Register(TextResourceId resourceId,uint16_t *text);

/* 0x0041CDE0 */
TextResolveResult TextResource_Resolve(TextResourceId resourceId);

#endif /* THANDOR_ASSETS_TEXT_RESOURCES_H */

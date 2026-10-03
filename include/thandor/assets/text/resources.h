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
/* FontRuntime_Init: terminator-scan limit in code units for stepping over the two consecutive font paths at
   g_FontTexturePathsUtf16 (0x42 bytes); shared by both scans. */
#define FONT_TEXTURE_PATHS_SCAN_UNITS 0x21
/* Colour of the drop shadow drawn under glyphs and UI icons: black at half alpha (ARGB). */
#ifndef TEXT_SHADOW_COLOR_ARGB
#define TEXT_SHADOW_COLOR_ARGB 0x7F000000
#endif
/* Functions are grouped by semantic ownership. */

bool TextResourcePage_LoadCompatibilityAliases(uint32_t levelTitleIndex,uint16_t *path);

void FontRuntime_Init(void);

uint32_t FontGlyph_GetLogicalSizeActiveFont(GraphicsSubresourceIndex glyphSubresource,uint32_t *outLineHeight);

uint32_t FontGlyph_GetLogicalSizeForStyle
          (UiPackedTextStyle packedStyle,GraphicsSubresourceIndex glyphSubresource,uint32_t *outLineHeight);

uint32_t FontGlyph_DrawBottomAligned (UiPixelCoordinate clipBottom,UiPixelCoordinate clipRight,UiPixelCoordinate clipTop, UiPixelCoordinate clipLeft,GraphicsSubresourceIndex glyphSubresource, UiPixelCoordinate baselineY,int32_t drawX);

uint32_t FontGlyph_DrawVerticallyCentered (UiPixelCoordinate clipBottom,UiPixelCoordinate clipRight,UiPixelCoordinate clipTop, UiPixelCoordinate clipLeft,GraphicsSubresourceIndex glyphSubresource, UiPixelCoordinate lineHeight,UiPixelCoordinate lineBottom,int32_t drawX);

bool TextResourcePage_Load(TextResourcePageIndex pageIndex,uint16_t *path,uint32_t *outLocaleBlockOrError);

void TextResourceOverride_Register(TextResourceId resourceId,uint16_t *text);

bool TextResource_TryResolve(TextResourceId resourceId,uint16_t **outText);

/* TextResource_TryResolve without the found flag (a missing text gives TEXT_RESOURCE_MISSING_SENTINEL_0x33) */
uint16_t *TextResource_Resolve(TextResourceId resourceId);

#endif /* THANDOR_ASSETS_TEXT_RESOURCES_H */

/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/include/thandor/ui/text/font.h
 * Reverse engineering by idkFoxes 2026
 */

#ifndef THANDOR_UI_TEXT_FONT_H
#define THANDOR_UI_TEXT_FONT_H

#include <thandor/graphics/resources/types.h>
#include <thandor/ui/controls/types.h>
#include <thandor/core/contracts.h>

/* FontRuntime_Init: terminator-scan limit in code units for stepping over the two consecutive font paths at
   g_FontTexturePathsUtf16 (0x42 bytes); shared by both scans. */
inline constexpr int32_t FONT_TEXTURE_PATHS_SCAN_UNITS = 0x21;
/* Number of fonts (engine\font.gfx and engine\fontk.gfx); the valid font indices are 0 and 1. */
inline constexpr int32_t FONT_TEXTURE_SOURCE_COUNT = 2;
/* Colour of the drop shadow drawn under glyphs and UI icons: black at half alpha (ARGB). */
#ifndef TEXT_SHADOW_COLOR_ARGB
#define TEXT_SHADOW_COLOR_ARGB 0x7F000000
#endif

void FontRuntime_Init();

uint32_t FontGlyph_GetLogicalSizeActiveFont(GraphicsSubresourceIndex glyphSubresource,uint32_t *outLineHeight);

uint32_t FontGlyph_GetLogicalSizeForStyle
          (UiPackedTextStyle packedStyle,GraphicsSubresourceIndex glyphSubresource,uint32_t *outLineHeight);

uint32_t FontGlyph_DrawBottomAligned (UiPixelCoordinate clipBottom,UiPixelCoordinate clipRight,UiPixelCoordinate clipTop, UiPixelCoordinate clipLeft,GraphicsSubresourceIndex glyphSubresource, UiPixelCoordinate baselineY,int32_t drawX);

uint32_t FontGlyph_DrawVerticallyCentered (UiPixelCoordinate clipBottom,UiPixelCoordinate clipRight,UiPixelCoordinate clipTop, UiPixelCoordinate clipLeft,GraphicsSubresourceIndex glyphSubresource, UiPixelCoordinate lineHeight,UiPixelCoordinate lineBottom,int32_t drawX);

extern GraphicsTextureSourceAsset *g_FontTextureSources[FONT_TEXTURE_SOURCE_COUNT];

#endif /* THANDOR_UI_TEXT_FONT_H */

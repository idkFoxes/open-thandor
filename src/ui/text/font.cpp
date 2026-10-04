/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/src/ui/text/font.cpp
 * Reverse engineering by idkFoxes 2026
 */

#include <thandor/ui/text/font.h>
#include <thandor/thandor.h>
#include <thandor/platform/bootstrap/image.h>

/* Module data. */

/* the two font texture paths L"engine\\font.gfx" and L"engine\\fontk.gfx", back to back:
   FontRuntime_Init scans past the first terminator to reach the second */
static uint16_t g_FontTexturePathsUtf16[33] = {'e', 'n', 'g', 'i', 'n', 'e', '\\', 'f', 'o', 'n', 't', '.', 'g', 'f', 'x', 0, 'e', 'n', 'g', 'i', 'n', 'e', '\\', 'f', 'o', 'n', 't', 'k', '.', 'g', 'f', 'x', 0}; /* L"engine\\font.gfx\0engine\\fontk.gfx" */

GraphicsTextureSourceAsset *g_FontTextureSources[FONT_TEXTURE_SOURCE_COUNT] = {};

/* Implementation ownership: ui/text/font. */

/* Returns the texture source of font fontIndex. The font index comes from the 3-bit font field of a packed text
   style (0..7) or from a font-select command (opcode & 0xF, 8..15). The original indexes its two-entry table
   with it unchecked and reads on into the font path text for anything above 1; bounded here because such an
   index only comes from malformed text or styles (the game's texts use fonts 0 and 1): it falls back to font 0
   and is logged once. */
static GraphicsTextureSourceAsset *FontTextureSource_Get(uint32_t fontIndex)
{
  static int s_loggedInvalidFontIndex;

  if (fontIndex >= FONT_TEXTURE_SOURCE_COUNT) {
    if (s_loggedInvalidFontIndex == 0) {
      s_loggedInvalidFontIndex = 1;
      Thandor_Log("font: invalid font index %u, using font 0",fontIndex);
    }
    fontIndex = 0;
  }
  return g_FontTextureSources[fontIndex];
}

/* Loads the two font texture sources from the consecutive UTF-16 paths in g_FontTexturePathsUtf16, allocates
   the 16 KiB font runtime buffer (the flattened text of the wrapped-text functions) and the text-resource
   override table, and fills that whole table (ids and text pointers) with 0xFFFFFFFF. Any failure is fatal.
*/
void FontRuntime_Init(void)

{
  wchar_t pathChar;
  int scanUnitsLeft;
  int sourceIndex;
  int dwordsLeft;
  const wchar_t *pathUtf16;
  uint32_t *overrideDword;
  GraphicsTextureSourceAsset *loadedTexture;
  uint32_t textureLoadError;
  uintptr_t checkedValue;
  uint32_t allocError;
  void *allocPayload;

  pathUtf16 = (const wchar_t *)g_FontTexturePathsUtf16;
  /* one scan budget for both paths: the original keeps a single terminator-scan count across the loop */
  scanUnitsLeft = FONT_TEXTURE_PATHS_SCAN_UNITS;
  for (sourceIndex = 0; sourceIndex < 2; sourceIndex++) {
    loadedTexture = g_GraphicsTextureSourceLoadPackageAsset((uint16_t *)pathUtf16,&textureLoadError);
    checkedValue = FatalError_ExitIfFailed(loadedTexture != nullptr ? (uintptr_t)loadedTexture : textureLoadError,
                                            loadedTexture == nullptr);
    g_FontTextureSources[sourceIndex] = (GraphicsTextureSourceAsset *)checkedValue;
    /* step pathUtf16 past the terminator to the next path */
    while (scanUnitsLeft != 0) {
      scanUnitsLeft--;
      pathChar = *pathUtf16;
      pathUtf16++;
      if (pathChar == L'\0') break;
    }
  }
  allocError = g_MemoryApi.alloc(RICHTEXT_RUNTIME_BUFFER_UNITS * sizeof(uint16_t),&allocPayload); /* 16 KiB */
  checkedValue = FatalError_ExitIfFailed(allocError != 0 ? allocError : (uintptr_t)allocPayload,allocError != 0);
  g_FontRuntimeBuffer = (uint8_t *)checkedValue;
  allocError = g_MemoryApi.alloc(sizeof(TextResourceOverrideTable),&allocPayload);
  checkedValue = FatalError_ExitIfFailed(allocError != 0 ? allocError : (uintptr_t)allocPayload,allocError != 0);
  g_TextResourceOverrides = (TextResourceOverrideTable *)checkedValue;
  overrideDword = g_TextResourceOverrides->resourceIds;
  /* all dwords: resourceIds and textPointers. TextResourceOverride_Register looks for a zero id, so
     after this fill it finds no free slot (the original fills the table with -1 the same way). */
  for (dwordsLeft = sizeof(TextResourceOverrideTable) / 4; dwordsLeft != 0; dwordsLeft--) {
    *overrideDword = TEXT_RESOURCE_ID_NONE;
    overrideDword++;
  }
  return;
}

/* Returns the width of one glyph (0 when the font has no such glyph) in the active font and stores the line
   height (the height of glyph 0) in *outLineHeight unless it is NULL; used to measure text before it is laid
   out. Both sizes are always queried.
*/
uint32_t FontGlyph_GetLogicalSizeActiveFont(GraphicsSubresourceIndex glyphSubresource,uint32_t *outLineHeight)

{
  uint32_t glyphWidth;
  GraphicsTextureLogicalSize textureSize;
  GraphicsTextureSourceAsset *fontTexture;

  fontTexture = FontTextureSource_Get(g_ActiveFontIndex);
  textureSize = g_GraphicsTextureSourceGetLogicalSize(glyphSubresource,fontTexture);
  glyphWidth = textureSize.logicalWidthPixels; /* 0 for a missing glyph */
  textureSize = g_GraphicsTextureSourceGetLogicalSize(0,fontTexture);
  if (outLineHeight != nullptr) {
    *outLineHeight = textureSize.logicalHeightPixels;
  }
  return glyphWidth;
}

/* Returns the width of one glyph (0 when the font has no such glyph) in the font selected by packedStyle,
   without changing the active font, and stores the line height (the height of glyph 0) in *outLineHeight
   unless it is NULL. Both sizes are always queried.
*/
uint32_t FontGlyph_GetLogicalSizeForStyle
          (UiPackedTextStyle packedStyle,GraphicsSubresourceIndex glyphSubresource,uint32_t *outLineHeight)

{
  uint32_t glyphWidth;
  GraphicsTextureSourceAsset *fontTexture;
  GraphicsTextureLogicalSize textureSize;

  fontTexture = FontTextureSource_Get(packedStyle >> TEXT_STYLE_FONT_SHIFT & TEXT_STYLE_INDEX_MASK);
  textureSize = g_GraphicsTextureSourceGetLogicalSize(glyphSubresource,fontTexture);
  glyphWidth = textureSize.logicalWidthPixels; /* 0 for a missing glyph */
  textureSize = g_GraphicsTextureSourceGetLogicalSize(0,fontTexture);
  if (outLineHeight != nullptr) {
    *outLineHeight = textureSize.logicalHeightPixels;
  }
  return glyphWidth;
}

/* Draws one glyph of the active font with its bottom edge on baselineY at drawX, clipped to the given
   rectangle, in the current rich-text colour; with a shadow offset set, a half-transparent black copy is drawn
   first, shifted down and right. Returns the glyph width (0 without a loaded font) so the caller can advance.
*/
uint32_t FontGlyph_DrawBottomAligned
               (UiPixelCoordinate clipBottom,UiPixelCoordinate clipRight,UiPixelCoordinate clipTop,
               UiPixelCoordinate clipLeft,GraphicsSubresourceIndex glyphSubresource,
               UiPixelCoordinate baselineY,int32_t drawX)

{
  int drawY;
  GraphicsTextureLogicalSize textureSize;
  uint32_t colorArgb;
  GraphicsTextureSourceAsset *fontTexture;
  SoftwareFramebufferAccess *framebuffer;

  fontTexture = FontTextureSource_Get(g_ActiveFontIndex);
  if (fontTexture != nullptr) {
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
    /* the glyph width measured before the blits */
    return textureSize.logicalWidthPixels;
  }
  return 0; /* no font loaded */
}

/* Draws one glyph of the active font at drawX, centred vertically in the text line that ends at lineBottom and
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
  GraphicsTextureLogicalSize textureSize;
  uint32_t colorArgb;
  GraphicsTextureSourceAsset *fontTexture;
  SoftwareFramebufferAccess *framebuffer;

  fontTexture = FontTextureSource_Get(g_ActiveFontIndex);
  if (fontTexture != nullptr) {
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
    /* the glyph width measured before the blits */
    return textureSize.logicalWidthPixels;
  }
  return 0; /* no font loaded */
}

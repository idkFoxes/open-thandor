/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/include/thandor/ui/text/hd_font.h
 * Project code (not in the original game)
 */

#ifndef THANDOR_UI_TEXT_HD_FONT_H
#define THANDOR_UI_TEXT_HD_FONT_H

/* Not in the original: the high-resolution game font ("Hochaufgeloeste Schrift", [game] hd_font). The glyphs of
   engine\font.gfx and engine\fontk.gfx rebuilt at 2x and 3x (tools/data/font_hd/make_hd_font.py, generated into
   src/assets/text/hd_font_data.cpp). Each one is exactly (width * N) x (height * N) target pixels, so advances,
   line height, wrapping and layout stay the original ones; only the GPU renderers use them, at UI scale 2 and 3,
   where a glyph's N x N target pixels per logical pixel would otherwise repeat the 1x texel (nearest sampling).
   The software renderer and UI scale 1 never see them.

   The glyphs are kept as two 'gfx' texture sources per font (one per scale) with direct-colour texels (white,
   the coverage in alpha, as the fonts' palettes hold it), held outside the game's memory arena. A glyph has its
   high-resolution form only when its own 1x glyph in the loaded font matches the one the data was made from (a
   hash per glyph); any other glyph keeps the plain scaling. */

struct GraphicsTextureSourceAsset;

/* True when the option is on ([game] hd_font) and "Original Thandor" is off (OT_FEATURE_HD_FONT). Read at every
   call, so a change applies with the next frame. */
bool HdFont_Enabled();

/* The high-resolution texture source to draw a glyph of fontSource from at UI scale uiScale, or nullptr: when the
   option is off, uiScale is not 2 or 3, fontSource is not one of the two loaded fonts (g_FontTextureSources) or
   the data could not be unpacked. Its subresource i is glyph i, (pixel width * uiScale) x (pixel height *
   uiScale) texels; a glyph without a high-resolution form (other font version) has the size 0 x 0. Built on the
   first call per font and scale, rebuilt when the loaded font changes. */
const GraphicsTextureSourceAsset *HdFont_GlyphSource(const GraphicsTextureSourceAsset *fontSource, int uiScale);

#endif /* THANDOR_UI_TEXT_HD_FONT_H */

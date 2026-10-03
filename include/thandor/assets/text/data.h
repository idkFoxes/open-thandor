/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/include/thandor/assets/text/data.h
 */

#ifndef THANDOR_ASSETS_TEXT_DATA_H
#define THANDOR_ASSETS_TEXT_DATA_H

#include <thandor/generated/types.h>
#include <thandor/generated/ui_templates.h>

/* 0041A750 g_RichTextColorPaletteArgb: rich-text colour palette, indexed by the 3-bit palette field of the
   packed text style (richtext.c) and set by RICHTEXT_OP_COLOR_PALETTE_0..3:
   [0] 0xFFB0B0B0 grey, also the normal cost colour of the technology panel;
   [1] 0xFFE0E0E0 light grey; [2] 0xFF707070 dark grey; [3] 0xFFE0E0E0 light grey;
   [4] 0xFF209020 green, only reachable through the packed text style's palette index;
   [5] 0xFFF02020 red, also the colour of technology costs the player cannot afford (ui/ingame/technology.c).
   The original's palette field has 3 bits; entries 6 and 7 would read on into the shadow offsets. */
extern PackedArgb32 g_RichTextColorPaletteArgb[6];

/* 0041A768 g_RichTextShadowOffsetPalette: text shadow offset in pixels per colour palette entry, indexed like
   g_RichTextColorPaletteArgb: 2, 2, 1, 2, 0, 0. */
extern uint32_t g_RichTextShadowOffsetPalette[6];

extern uint32_t g_ActiveFontIndex; /* 0041A780 g_ActiveFontIndex */

extern uint32_t g_RichTextCurrentColorArgb; /* 0041A784 g_RichTextCurrentColorArgb */

extern uint32_t g_RichTextCurrentShadowOffset; /* 0041A788 g_RichTextCurrentShadowOffset */

extern uint32_t g_RichTextSavedColorArgb; /* 0041A78C g_RichTextSavedColorArgb */

extern uint32_t g_RichTextSavedShadowOffset; /* 0041A790 g_RichTextSavedShadowOffset */

extern TextResourcePageBinding g_TextResourcePageBindings[256]; /* 0041A794 g_TextResourcePageBindings */

extern TextResourceOverrideTable *g_TextResourceOverrides; /* 0041AF94 g_TextResourceOverrides */

extern uint8_t *g_FontRuntimeBuffer; /* 0041AF98 g_FontRuntimeBuffer */

extern uint32_t g_RichTextRuntimeBufferUsedWords; /* 0041AF9C g_RichTextRuntimeBufferUsedWords */

extern uint16_t g_EmptyTextResourceUtf16[2]; /* 0041AFA4 g_EmptyTextResourceUtf16 */

/* 0041AFA8 g_MissingTextResourceFallbackStream: UTF-16 rich-text stream L"-" (code unit '-' plus terminator) that
   unresolved nested-stream records point to */
extern uint16_t g_MissingTextResourceFallbackStream[2];

extern uint16_t u_error__TXT2STR__unknown_characte_0041afac[62]; /* 0041AFAC u_error__TXT2STR__unknown_characte_0041afac */

extern GraphicsTextureSourceAsset *g_FontTextureSources[2]; /* 0041B028 g_FontTextureSources */

/* 0041B030 the two font texture paths L"engine\\font.gfx" and L"engine\\fontk.gfx", back to back:
   FontRuntime_Init scans past the first terminator to reach the second */
extern uint16_t g_FontTexturePathsUtf16[33];

#endif

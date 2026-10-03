/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/src/assets/text/data.c
 */

/* Data of the original image that this module uses (moved here from the generated image data in
   step 4c); declared in <thandor/assets/text/data.h>. Original addresses in the comments. */

#include <thandor/thandor.h>

#pragma warning(disable : 4152) /* function pointer fields initialized through (void *) */

/* rich-text colour palette, indexed by the 3-bit palette field of the
   packed text style (richtext.c) and set by RICHTEXT_OP_COLOR_PALETTE_0..3. */
__declspec(align(16)) PackedArgb32 g_RichTextColorPaletteArgb[6] = {
    0xFFB0B0B0, /* 0041A750 [0] grey; also the normal cost colour of the technology panel */
    0xFFE0E0E0, /* 0041A754 [1] light grey, RICHTEXT_OP_COLOR_PALETTE_1 */
    0xFF707070, /* 0041A758 [2] dark grey, RICHTEXT_OP_COLOR_PALETTE_2 */
    0xFFE0E0E0, /* 0041A75C [3] light grey, RICHTEXT_OP_COLOR_PALETTE_3 */
    0xFF209020, /* 0041A760 [4] green; only reachable through the packed text style's palette index */
    0xFFF02020, /* 0041A764 [5] red; also technology costs the player cannot afford (ui/ingame/technology.c) */
};

/* text shadow offset in pixels per colour palette entry, indexed like
   g_RichTextColorPaletteArgb. */
__declspec(align(8)) uint32_t g_RichTextShadowOffsetPalette[6] = {
    2, /* 0041A768 [0] */
    2, /* 0041A76C [1] */
    1, /* 0041A770 [2] */
    2, /* 0041A774 [3] */
    0, /* 0041A778 [4] */
    0, /* 0041A77C [5] */
};

__declspec(align(16)) uint32_t g_ActiveFontIndex = 0;

__declspec(align(4)) uint32_t g_RichTextCurrentColorArgb = 0;

__declspec(align(8)) uint32_t g_RichTextCurrentShadowOffset = 0;

__declspec(align(4)) uint32_t g_RichTextSavedColorArgb = 0;

__declspec(align(16)) uint32_t g_RichTextSavedShadowOffset = 0;

__declspec(align(4)) TextResourcePageBinding g_TextResourcePageBindings[256] = {0};

__declspec(align(4)) TextResourceOverrideTable *g_TextResourceOverrides = 0;

__declspec(align(8)) uint8_t *g_FontRuntimeBuffer = 0;

__declspec(align(4)) uint32_t g_RichTextRuntimeBufferUsedWords = 0;

__declspec(align(4)) uint16_t g_EmptyTextResourceUtf16[2] = {0};

/* L"-" */
__declspec(align(8)) uint16_t g_MissingTextResourceFallbackStream[2] = {0x002D, 0x0000};

__declspec(align(4)) uint16_t u_error__TXT2STR__unknown_characte_0041afac[62] = L"error: TXT2STR: unknown character at:                        ";

__declspec(align(8)) GraphicsTextureSourceAsset *g_FontTextureSources[2] = {0};

/* 0041B030 u_engine_font_gfx_0041b030, 0041B050 str_0041B050 */
__declspec(align(16)) uint16_t g_FontTexturePathsUtf16[33] = L"engine\\font.gfx\0engine\\fontk.gfx";

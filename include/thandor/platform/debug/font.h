/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/include/thandor/platform/debug/font.h
 * Project code (not in the original game)
 */

#ifndef THANDOR_PLATFORM_DEBUG_FONT_H
#define THANDOR_PLATFORM_DEBUG_FONT_H

/* Built-in 5x7 debug font, drawn straight into the framebuffer (debug tools only). */

/* Text colour (32 bits per pixel), and the box behind the text (opaque black) */
#define DEBUG_FONT_TEXT_COLOR_32BPP 0xffffff40
#define DEBUG_FONT_BOX_COLOR_32BPP ARGB8888_ALPHA_MASK

/* Draws text at (x0, y0) at 1x scale with a black box behind it, into the locked framebuffer
   (g_FramebufferAccess, 32 bits per pixel); characters without a glyph are left blank. */
void DebugFont_DrawText(int x0, int y0, const char *text);

#endif /* THANDOR_PLATFORM_DEBUG_FONT_H */

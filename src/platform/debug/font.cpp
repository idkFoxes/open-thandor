/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/src/platform/debug/font.cpp
 * Project code (not in the original game)
 */

#include <string.h>
#include <thandor/thandor.h>
#include <thandor/platform/debug/font.h>

/* 5x7 debug font: each entry is the character followed by its 7 rows of 5 pixels, top row first; in a row the
   leftmost pixel is bit 4. The glyphs are capitals stored under lower-case keys (DebugFont_DrawText folds
   A-Z to a-z). In the table X is a set pixel and o a clear one, so each glyph reads as a picture. */
#define FONT_ROW(a,b,c,d,e) ((a) << 4 | (b) << 3 | (c) << 2 | (d) << 1 | (e))
#define X 1
#define o 0

static const uint8_t g_DebugFont5x7[][8] = {
  {' ', FONT_ROW(o,o,o,o,o),
        FONT_ROW(o,o,o,o,o),
        FONT_ROW(o,o,o,o,o),
        FONT_ROW(o,o,o,o,o),
        FONT_ROW(o,o,o,o,o),
        FONT_ROW(o,o,o,o,o),
        FONT_ROW(o,o,o,o,o)},
  {'.', FONT_ROW(o,o,o,o,o),
        FONT_ROW(o,o,o,o,o),
        FONT_ROW(o,o,o,o,o),
        FONT_ROW(o,o,o,o,o),
        FONT_ROW(o,o,o,o,o),
        FONT_ROW(o,X,X,o,o),
        FONT_ROW(o,X,X,o,o)},
  {'/', FONT_ROW(o,o,o,o,X),
        FONT_ROW(o,o,o,X,o),
        FONT_ROW(o,o,o,X,o),
        FONT_ROW(o,o,X,o,o),
        FONT_ROW(o,X,o,o,o),
        FONT_ROW(o,X,o,o,o),
        FONT_ROW(X,o,o,o,o)},
  {':', FONT_ROW(o,o,o,o,o),
        FONT_ROW(o,X,X,o,o),
        FONT_ROW(o,X,X,o,o),
        FONT_ROW(o,o,o,o,o),
        FONT_ROW(o,X,X,o,o),
        FONT_ROW(o,X,X,o,o),
        FONT_ROW(o,o,o,o,o)},
  {'-', FONT_ROW(o,o,o,o,o),
        FONT_ROW(o,o,o,o,o),
        FONT_ROW(o,o,o,o,o),
        FONT_ROW(X,X,X,X,X),
        FONT_ROW(o,o,o,o,o),
        FONT_ROW(o,o,o,o,o),
        FONT_ROW(o,o,o,o,o)},
  {'_', FONT_ROW(o,o,o,o,o),
        FONT_ROW(o,o,o,o,o),
        FONT_ROW(o,o,o,o,o),
        FONT_ROW(o,o,o,o,o),
        FONT_ROW(o,o,o,o,o),
        FONT_ROW(o,o,o,o,o),
        FONT_ROW(X,X,X,X,X)},
  {'0', FONT_ROW(o,X,X,X,o),
        FONT_ROW(X,o,o,o,X),
        FONT_ROW(X,o,o,X,X),
        FONT_ROW(X,o,X,o,X),
        FONT_ROW(X,X,o,o,X),
        FONT_ROW(X,o,o,o,X),
        FONT_ROW(o,X,X,X,o)},
  {'1', FONT_ROW(o,o,X,o,o),
        FONT_ROW(o,X,X,o,o),
        FONT_ROW(o,o,X,o,o),
        FONT_ROW(o,o,X,o,o),
        FONT_ROW(o,o,X,o,o),
        FONT_ROW(o,o,X,o,o),
        FONT_ROW(o,X,X,X,o)},
  {'2', FONT_ROW(o,X,X,X,o),
        FONT_ROW(X,o,o,o,X),
        FONT_ROW(o,o,o,o,X),
        FONT_ROW(o,o,o,X,o),
        FONT_ROW(o,o,X,o,o),
        FONT_ROW(o,X,o,o,o),
        FONT_ROW(X,X,X,X,X)},
  {'3', FONT_ROW(X,X,X,X,X),
        FONT_ROW(o,o,o,X,o),
        FONT_ROW(o,o,X,o,o),
        FONT_ROW(o,o,o,X,o),
        FONT_ROW(o,o,o,o,X),
        FONT_ROW(X,o,o,o,X),
        FONT_ROW(o,X,X,X,o)},
  {'4', FONT_ROW(o,o,o,X,o),
        FONT_ROW(o,o,X,X,o),
        FONT_ROW(o,X,o,X,o),
        FONT_ROW(X,o,o,X,o),
        FONT_ROW(X,X,X,X,X),
        FONT_ROW(o,o,o,X,o),
        FONT_ROW(o,o,o,X,o)},
  {'5', FONT_ROW(X,X,X,X,X),
        FONT_ROW(X,o,o,o,o),
        FONT_ROW(X,X,X,X,o),
        FONT_ROW(o,o,o,o,X),
        FONT_ROW(o,o,o,o,X),
        FONT_ROW(X,o,o,o,X),
        FONT_ROW(o,X,X,X,o)},
  {'6', FONT_ROW(o,o,X,X,o),
        FONT_ROW(o,X,o,o,o),
        FONT_ROW(X,o,o,o,o),
        FONT_ROW(X,X,X,X,o),
        FONT_ROW(X,o,o,o,X),
        FONT_ROW(X,o,o,o,X),
        FONT_ROW(o,X,X,X,o)},
  {'7', FONT_ROW(X,X,X,X,X),
        FONT_ROW(o,o,o,o,X),
        FONT_ROW(o,o,o,X,o),
        FONT_ROW(o,o,X,o,o),
        FONT_ROW(o,X,o,o,o),
        FONT_ROW(o,X,o,o,o),
        FONT_ROW(o,X,o,o,o)},
  {'8', FONT_ROW(o,X,X,X,o),
        FONT_ROW(X,o,o,o,X),
        FONT_ROW(X,o,o,o,X),
        FONT_ROW(o,X,X,X,o),
        FONT_ROW(X,o,o,o,X),
        FONT_ROW(X,o,o,o,X),
        FONT_ROW(o,X,X,X,o)},
  {'9', FONT_ROW(o,X,X,X,o),
        FONT_ROW(X,o,o,o,X),
        FONT_ROW(X,o,o,o,X),
        FONT_ROW(o,X,X,X,X),
        FONT_ROW(o,o,o,o,X),
        FONT_ROW(o,o,o,X,o),
        FONT_ROW(o,X,X,o,o)},
  {'a', FONT_ROW(o,X,X,X,o),
        FONT_ROW(X,o,o,o,X),
        FONT_ROW(X,o,o,o,X),
        FONT_ROW(X,X,X,X,X),
        FONT_ROW(X,o,o,o,X),
        FONT_ROW(X,o,o,o,X),
        FONT_ROW(X,o,o,o,X)},
  {'b', FONT_ROW(X,X,X,X,o),
        FONT_ROW(X,o,o,o,X),
        FONT_ROW(X,o,o,o,X),
        FONT_ROW(X,X,X,X,o),
        FONT_ROW(X,o,o,o,X),
        FONT_ROW(X,o,o,o,X),
        FONT_ROW(X,X,X,X,o)},
  {'c', FONT_ROW(o,X,X,X,o),
        FONT_ROW(X,o,o,o,X),
        FONT_ROW(X,o,o,o,o),
        FONT_ROW(X,o,o,o,o),
        FONT_ROW(X,o,o,o,o),
        FONT_ROW(X,o,o,o,X),
        FONT_ROW(o,X,X,X,o)},
  {'d', FONT_ROW(X,X,X,o,o),
        FONT_ROW(X,o,o,X,o),
        FONT_ROW(X,o,o,o,X),
        FONT_ROW(X,o,o,o,X),
        FONT_ROW(X,o,o,o,X),
        FONT_ROW(X,o,o,X,o),
        FONT_ROW(X,X,X,o,o)},
  {'e', FONT_ROW(X,X,X,X,X),
        FONT_ROW(X,o,o,o,o),
        FONT_ROW(X,o,o,o,o),
        FONT_ROW(X,X,X,X,o),
        FONT_ROW(X,o,o,o,o),
        FONT_ROW(X,o,o,o,o),
        FONT_ROW(X,X,X,X,X)},
  {'f', FONT_ROW(X,X,X,X,X),
        FONT_ROW(X,o,o,o,o),
        FONT_ROW(X,o,o,o,o),
        FONT_ROW(X,X,X,X,o),
        FONT_ROW(X,o,o,o,o),
        FONT_ROW(X,o,o,o,o),
        FONT_ROW(X,o,o,o,o)},
  {'g', FONT_ROW(o,X,X,X,o),
        FONT_ROW(X,o,o,o,X),
        FONT_ROW(X,o,o,o,o),
        FONT_ROW(X,o,X,X,X),
        FONT_ROW(X,o,o,o,X),
        FONT_ROW(X,o,o,o,X),
        FONT_ROW(o,X,X,X,X)},
  {'h', FONT_ROW(X,o,o,o,X),
        FONT_ROW(X,o,o,o,X),
        FONT_ROW(X,o,o,o,X),
        FONT_ROW(X,X,X,X,X),
        FONT_ROW(X,o,o,o,X),
        FONT_ROW(X,o,o,o,X),
        FONT_ROW(X,o,o,o,X)},
  {'i', FONT_ROW(o,X,X,X,o),
        FONT_ROW(o,o,X,o,o),
        FONT_ROW(o,o,X,o,o),
        FONT_ROW(o,o,X,o,o),
        FONT_ROW(o,o,X,o,o),
        FONT_ROW(o,o,X,o,o),
        FONT_ROW(o,X,X,X,o)},
  {'j', FONT_ROW(o,o,X,X,X),
        FONT_ROW(o,o,o,X,o),
        FONT_ROW(o,o,o,X,o),
        FONT_ROW(o,o,o,X,o),
        FONT_ROW(o,o,o,X,o),
        FONT_ROW(X,o,o,X,o),
        FONT_ROW(o,X,X,o,o)},
  {'k', FONT_ROW(X,o,o,o,X),
        FONT_ROW(X,o,o,X,o),
        FONT_ROW(X,o,X,o,o),
        FONT_ROW(X,X,o,o,o),
        FONT_ROW(X,o,X,o,o),
        FONT_ROW(X,o,o,X,o),
        FONT_ROW(X,o,o,o,X)},
  {'l', FONT_ROW(X,o,o,o,o),
        FONT_ROW(X,o,o,o,o),
        FONT_ROW(X,o,o,o,o),
        FONT_ROW(X,o,o,o,o),
        FONT_ROW(X,o,o,o,o),
        FONT_ROW(X,o,o,o,o),
        FONT_ROW(X,X,X,X,X)},
  {'m', FONT_ROW(X,o,o,o,X),
        FONT_ROW(X,X,o,X,X),
        FONT_ROW(X,o,X,o,X),
        FONT_ROW(X,o,X,o,X),
        FONT_ROW(X,o,o,o,X),
        FONT_ROW(X,o,o,o,X),
        FONT_ROW(X,o,o,o,X)},
  {'n', FONT_ROW(X,o,o,o,X),
        FONT_ROW(X,o,o,o,X),
        FONT_ROW(X,X,o,o,X),
        FONT_ROW(X,o,X,o,X),
        FONT_ROW(X,o,o,X,X),
        FONT_ROW(X,o,o,o,X),
        FONT_ROW(X,o,o,o,X)},
  {'o', FONT_ROW(o,X,X,X,o),
        FONT_ROW(X,o,o,o,X),
        FONT_ROW(X,o,o,o,X),
        FONT_ROW(X,o,o,o,X),
        FONT_ROW(X,o,o,o,X),
        FONT_ROW(X,o,o,o,X),
        FONT_ROW(o,X,X,X,o)},
  {'p', FONT_ROW(X,X,X,X,o),
        FONT_ROW(X,o,o,o,X),
        FONT_ROW(X,o,o,o,X),
        FONT_ROW(X,X,X,X,o),
        FONT_ROW(X,o,o,o,o),
        FONT_ROW(X,o,o,o,o),
        FONT_ROW(X,o,o,o,o)},
  {'q', FONT_ROW(o,X,X,X,o),
        FONT_ROW(X,o,o,o,X),
        FONT_ROW(X,o,o,o,X),
        FONT_ROW(X,o,o,o,X),
        FONT_ROW(X,o,X,o,X),
        FONT_ROW(X,o,o,X,o),
        FONT_ROW(o,X,X,o,X)},
  {'r', FONT_ROW(X,X,X,X,o),
        FONT_ROW(X,o,o,o,X),
        FONT_ROW(X,o,o,o,X),
        FONT_ROW(X,X,X,X,o),
        FONT_ROW(X,o,X,o,o),
        FONT_ROW(X,o,o,X,o),
        FONT_ROW(X,o,o,o,X)},
  {'s', FONT_ROW(o,X,X,X,X),
        FONT_ROW(X,o,o,o,o),
        FONT_ROW(X,o,o,o,o),
        FONT_ROW(o,X,X,X,o),
        FONT_ROW(o,o,o,o,X),
        FONT_ROW(o,o,o,o,X),
        FONT_ROW(X,X,X,X,o)},
  {'t', FONT_ROW(X,X,X,X,X),
        FONT_ROW(o,o,X,o,o),
        FONT_ROW(o,o,X,o,o),
        FONT_ROW(o,o,X,o,o),
        FONT_ROW(o,o,X,o,o),
        FONT_ROW(o,o,X,o,o),
        FONT_ROW(o,o,X,o,o)},
  {'u', FONT_ROW(X,o,o,o,X),
        FONT_ROW(X,o,o,o,X),
        FONT_ROW(X,o,o,o,X),
        FONT_ROW(X,o,o,o,X),
        FONT_ROW(X,o,o,o,X),
        FONT_ROW(X,o,o,o,X),
        FONT_ROW(o,X,X,X,o)},
  {'v', FONT_ROW(X,o,o,o,X),
        FONT_ROW(X,o,o,o,X),
        FONT_ROW(X,o,o,o,X),
        FONT_ROW(X,o,o,o,X),
        FONT_ROW(X,o,o,o,X),
        FONT_ROW(o,X,o,X,o),
        FONT_ROW(o,o,X,o,o)},
  {'w', FONT_ROW(X,o,o,o,X),
        FONT_ROW(X,o,o,o,X),
        FONT_ROW(X,o,o,o,X),
        FONT_ROW(X,o,X,o,X),
        FONT_ROW(X,o,X,o,X),
        FONT_ROW(X,o,X,o,X),
        FONT_ROW(o,X,o,X,o)},
  {'x', FONT_ROW(X,o,o,o,X),
        FONT_ROW(X,o,o,o,X),
        FONT_ROW(o,X,o,X,o),
        FONT_ROW(o,o,X,o,o),
        FONT_ROW(o,X,o,X,o),
        FONT_ROW(X,o,o,o,X),
        FONT_ROW(X,o,o,o,X)},
  {'y', FONT_ROW(X,o,o,o,X),
        FONT_ROW(X,o,o,o,X),
        FONT_ROW(X,o,o,o,X),
        FONT_ROW(o,X,o,X,o),
        FONT_ROW(o,o,X,o,o),
        FONT_ROW(o,o,X,o,o),
        FONT_ROW(o,o,X,o,o)},
  {'z', FONT_ROW(X,X,X,X,X),
        FONT_ROW(o,o,o,o,X),
        FONT_ROW(o,o,o,X,o),
        FONT_ROW(o,o,X,o,o),
        FONT_ROW(o,X,o,o,o),
        FONT_ROW(X,o,o,o,o),
        FONT_ROW(X,X,X,X,X)},
};

#undef X
#undef o
#undef FONT_ROW

/* Draws text at 1x scale with a black box behind it; framebuffer: [0] pitch in pixels,
   [2] bytes per pixel, [3] pixels. */
void DebugFont_DrawText(int x0, int y0, const char *text)
{
  uint32_t *fb = (uint32_t *)g_FramebufferAccess;
  int scale = 1;
  int length = (int)strlen(text);
  int boxWidth = length * 6 * scale + 2 * scale;
  int boxHeight = 9 * scale;
  uint32_t pitch;
  uint32_t bpp;
  uint8_t *pixels;
  int x;
  int y;
  int c;
  if (fb == NULL) {
    return;
  }
  pitch = fb[0];
  bpp = fb[2];
  pixels = (uint8_t *)(uintptr_t)fb[3];
  if ((pixels == NULL) || (bpp != 4)) {
    return;
  }
  if (x0 + boxWidth > (int)g_FramebufferWidth) boxWidth = (int)g_FramebufferWidth - x0;
#define DEBUG_PUT(px, py, white)                                                          \
  do {                                                                                    \
    ((uint32_t *)pixels)[(py) * pitch + (px)] = (white) ? DEBUG_FONT_TEXT_COLOR_32BPP : DEBUG_FONT_BOX_COLOR_32BPP; \
  } while (0)
  for (y = 0; y < boxHeight; y++) {
    for (x = 0; x < boxWidth; x++) {
      DEBUG_PUT(x0 + x, y0 + y, 0);
    }
  }
  for (c = 0; c < length; c++) {
    char ch = text[c];
    const uint8_t *glyph = NULL;
    unsigned g;
    if ((ch >= 'A') && (ch <= 'Z')) ch = (char)(ch - 'A' + 'a');
    for (g = 0; g < sizeof g_DebugFont5x7 / sizeof g_DebugFont5x7[0]; g++) {
      if (g_DebugFont5x7[g][0] == (uint8_t)ch) { glyph = g_DebugFont5x7[g] + 1; break; }
    }
    if (glyph == NULL) continue;
    for (y = 0; y < 7 * scale; y++) {
      for (x = 0; x < 5 * scale; x++) {
        int px = x0 + scale + c * 6 * scale + x;
        if (px >= (int)g_FramebufferWidth) break;
        if ((glyph[y / scale] >> (4 - x / scale)) & 1) {
          DEBUG_PUT(px, y0 + scale + y, 1);
        }
      }
    }
  }
#undef DEBUG_PUT
}

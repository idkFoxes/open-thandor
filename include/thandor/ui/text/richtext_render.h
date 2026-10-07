/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/include/thandor/ui/text/richtext_render.h
 * Reverse engineering by idkFoxes 2026
 */

#ifndef THANDOR_UI_TEXT_RICHTEXT_RENDER_H
#define THANDOR_UI_TEXT_RICHTEXT_RENDER_H

#include <thandor/core/types.h>
#include <thandor/ui/controls/types.h>
#include <thandor/ui/text/types.h>
#include <thandor/core/contracts.h>

/* UiPackedTextStyle fields as the rich-text interpreters decode them. */
inline constexpr int32_t TEXT_STYLE_ALIGN_RIGHT = 0x1; /* the line ends at the given x */
inline constexpr int32_t TEXT_STYLE_ALIGN_CENTER = 0x2; /* the line is centred on the given x (ignored with ALIGN_RIGHT) */
inline constexpr int32_t TEXT_STYLE_PALETTE_SHIFT = 16; /* bits 16-18: colour/shadow palette entry */
inline constexpr int32_t TEXT_STYLE_FONT_SHIFT = 24; /* bits 24-26: font index */
inline constexpr int32_t TEXT_STYLE_INDEX_MASK = 7;

RichTextExtent RichTextCommandStream_MeasureWrappedBlock
          (uint32_t packedStyle,uint16_t *commandStream,UiPixelExtent maximumWidth);

void RichTextCommandStream_DrawWrappedBlock
          (UiPixelCoordinate clipBottom,UiPixelCoordinate clipRight,UiPixelCoordinate clipTop,
          UiPixelCoordinate clipLeft,uint32_t packedStyle,uint16_t *commandStream,
          UiPixelExtent maximumWidth,UiPixelCoordinate drawY,UiPixelCoordinate drawX);

bool RichTextCommandStream_DrawSingleLine
          (UiPixelCoordinate clipBottom,UiPixelCoordinate clipRight,UiPixelCoordinate clipTop,
          UiPixelCoordinate clipLeft,UiPackedTextStyle packedStyle,uint16_t *commandStream,
          UiPixelCoordinate lineTopY,UiPixelCoordinate penX);

RichTextExtent RichTextCommandStream_MeasureLine(UiPackedTextStyle packedStyle,uint16_t *commandStream);

bool RichTextCommandStream_MeasureNextWrappedLine(UiPixelExtent maximumWidth,UiPixelExtent *lineHeight);

bool RichTextCommandStream_DrawNextWrappedLine
          (UiPixelCoordinate clipBottom,UiPixelCoordinate clipRight,UiPixelCoordinate clipTop,
          UiPixelCoordinate clipLeft,UiPixelExtent maximumWidth,UiPixelCoordinate drawY,
          UiPixelCoordinate drawX,UiPixelExtent *lineAdvance);

extern PackedArgb32 g_RichTextColorPaletteArgb[TEXT_STYLE_INDEX_MASK + 1];
extern uint32_t g_ActiveFontIndex;
extern uint32_t g_RichTextCurrentColorArgb;
extern uint32_t g_RichTextCurrentShadowOffset;

#endif /* THANDOR_UI_TEXT_RICHTEXT_RENDER_H */

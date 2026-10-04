/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/include/thandor/ui/controls/text.h
 * Reverse engineering by idkFoxes 2026
 */

#ifndef THANDOR_UI_CONTROLS_TEXT_H
#define THANDOR_UI_CONTROLS_TEXT_H

#include <thandor/ui/controls/types.h>
#include <thandor/core/contracts.h>

/* Submodule: ui/controls/text. */

/* labelFlags of UiSingleLineTextControl and UiWrappedTextControl. */
#define UI_LABEL_CENTER_X 0x01
#define UI_LABEL_ALIGN_RIGHT 0x02
#define UI_LABEL_CENTER_Y 0x04
#define UI_LABEL_ALIGN_BOTTOM 0x08
#define UI_LABEL_TEXT_IS_STREAM 0x10 /* text is a rich-text command stream, else a TextResourceId */
#define UI_LABEL_HIDE_WHILE_SUPPRESSED 0x40 /* single-line label */
#define UI_LABEL_KEEP_WRAP_WIDTH 0x40 /* wrapped label: wrapWidth does not follow layoutWidth */
#define UI_LABEL_OWN_STYLE_FONT 0x100 /* the font byte of styleOverride replaces g_UiTextStyleNormal's */
#define UI_LABEL_OWN_STYLE_PALETTE 0x200 /* the palette byte of styleOverride replaces g_UiTextStyleNormal's */

/* Functions are grouped by semantic ownership. */

void UiSingleLineTextControl_DrawClipped
          (UiPixelCoordinate clipBottom,UiPixelCoordinate clipRight,UiPixelCoordinate clipTop,
          UiPixelCoordinate clipLeft,UiSingleLineTextControl *control);

void UiWrappedTextControl_DrawClipped(UiPixelCoordinate clipBottom,UiPixelCoordinate clipRight,UiPixelCoordinate clipTop,
          UiPixelCoordinate clipLeft,UiWrappedTextControl *control);

void UiWrappedTextControl_RelocateAndApplyDeferredOffset
          (UiSerializedRelocationDelta relocationDelta,UiWrappedTextControl *control);

#endif /* THANDOR_UI_CONTROLS_TEXT_H */

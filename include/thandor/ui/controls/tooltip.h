/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/include/thandor/ui/controls/tooltip.h
 * Reverse engineering by idkFoxes 2026
 */

#ifndef THANDOR_UI_CONTROLS_TOOLTIP_H
#define THANDOR_UI_CONTROLS_TOOLTIP_H

#include <thandor/ui/controls/types.h>
#include <thandor/core/contracts.h>

/* Submodule: ui/controls/tooltip. */

/* UiTooltip_Draw: the one-line tooltip box from g_UiWindowTextureSource, and the dimming of the whole screen
   while no root is open (black at half alpha). */
#define UI_WINDOW_SUBRESOURCE_TOOLTIP_LEFT 0xBC
#define UI_WINDOW_SUBRESOURCE_TOOLTIP_MIDDLE 0xBD
#define UI_WINDOW_SUBRESOURCE_TOOLTIP_RIGHT 0xBE
#define UI_TOOLTIP_NO_ROOT_DIM_ARGB 0x80000000
/* UiGraphicsAdapterTextButton_DrawFormattedAdapterText: stateFlags bits choosing the payloads, and the text
   shown as device name of the primary display adapter (GUID 0). */
#define UI_ADAPTER_TEXT_BUTTON_SINGLE_NUMBER 0x80
#define UI_ADAPTER_TEXT_BUTTON_ADAPTER_NAME 0x800
#define TEXT_ID_PRIMARY_DISPLAY_ADAPTER 0x111
/* Windows-1252 key codes typed with AltGr on a German keyboard (the text edits' keyboard handlers accept them
   without the Ctrl/Alt shortcut check). */
#define CP1252_SUPERSCRIPT_TWO 0xb2
#define CP1252_SUPERSCRIPT_THREE 0xb3
#define CP1252_MICRO_SIGN 0xb5
#define CP1252_EURO_SIGN 0x80
/* UiNumericTextEdit hexadecimal output: a bit index 0..31 rounded down to the lowest bit of its hex digit */
#define UI_NUMERIC_TEXT_HEX_DIGIT_BIT_MASK 0x1c

/* Functions are grouped by semantic ownership. */

void UiTooltip_TickCountdown(void);

void UiTooltip_Draw(UiPixelCoordinate clipBottom,UiPixelCoordinate clipRight,UiPixelCoordinate clipTop,
              UiPixelCoordinate clipLeft);

void UiTooltip_UpdateHoverTarget(UiPixelCoordinate pointerY,UiPixelCoordinate pointerX);

void UiTooltip_PrepareTargetText(UiNodeBase *node);

extern UiTooltipState g_UiTooltipState;

#endif /* THANDOR_UI_CONTROLS_TOOLTIP_H */

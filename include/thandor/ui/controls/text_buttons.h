/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/include/thandor/ui/controls/text_buttons.h
 * Reverse engineering by idkFoxes 2026
 */

#ifndef THANDOR_UI_CONTROLS_TEXT_BUTTONS_H
#define THANDOR_UI_CONTROLS_TEXT_BUTTONS_H

#include <thandor/generated/types.h>
#include <thandor/core/contracts.h>

/* Submodule: ui/controls/text_buttons. */

/* Pieces of g_UiWindowTextureSource (win.gfx) drawn by the text controls. A frame is UI_WINDOW_FRAME_PIECE_COUNT
   consecutive pieces: top-left, top-right, bottom-left and bottom-right corner, then the top, left, right and
   bottom edge. */
#define UI_WINDOW_FRAME_PIECE_COUNT 8
#define UI_WINDOW_SUBRESOURCE_BUTTON_FRAME 0x4A /* the selected frame follows it */
#define UI_WINDOW_SUBRESOURCE_BUTTON_FRAME_DISABLED 0x8C
#define UI_WINDOW_SUBRESOURCE_INSET_BUTTON_FRAME 0x94 /* UI_BUTTON_FRAME_INSET; the selected frame follows it */
#define UI_WINDOW_SUBRESOURCE_INSET_BUTTON_FRAME_DISABLED 0xA4
#define UI_WINDOW_SUBRESOURCE_TEXT_EDIT_FRAME 0x6A
#define UI_WINDOW_SUBRESOURCE_TEXT_EDIT_INTERIOR 0x7A
#define UI_WINDOW_SUBRESOURCE_CHECKBOX 0x40 /* toggle text button: +2 checked, +4 alternate state, +1 disabled */
#define UI_WINDOW_SUBRESOURCE_PUSH_BUTTON 0x46 /* push text button: +2 pressed, +1 disabled */
#define UI_WINDOW_SUBRESOURCE_LIST_SELECTION 0x82 /* selected list row, list without focus */
#define UI_WINDOW_SUBRESOURCE_LIST_FOCUS_SELECTION_LEFT 0x83 /* selected list row with focus: left cap, */
#define UI_WINDOW_SUBRESOURCE_LIST_FOCUS_SELECTION_MIDDLE 0x84 /* tiled middle */
#define UI_WINDOW_SUBRESOURCE_LIST_FOCUS_SELECTION_RIGHT 0x85 /* and right cap */
#define UI_WINDOW_SUBRESOURCE_FOCUS_MARK_LEFT 0x86 /* keyboard-focus mark behind a focused label: left cap, */
#define UI_WINDOW_SUBRESOURCE_FOCUS_MARK_MIDDLE 0x87 /* tiled middle */
#define UI_WINDOW_SUBRESOURCE_FOCUS_MARK_RIGHT 0x88 /* and right cap */
#define UI_WINDOW_SUBRESOURCE_CARET_INSERT 0x89
#define UI_WINDOW_SUBRESOURCE_CARET_OVERWRITE 0x8A
#define UI_WINDOW_SUBRESOURCE_TEXT_SELECTION 0x8B

/* stateFlags bits (UiSelectableControl) that the text and framed buttons read. */
#define UI_BUTTON_FRAME_INSET 0x04 /* framed buttons: inset frame, the hit area shrinks by g_UiWindowFrameInset */
#define UI_BUTTON_ALTERNATE_STATE 0x40 /* text buttons: third checkbox state, alternate text style; cleared on toggle */
#define UI_BUTTON_PLAY_ACTIVATION_SOUND 0x80 /* play activationSound on activation */
#define UI_BUTTON_OWN_STYLE_FONT 0x100 /* the font byte of packedTextStyle replaces the state style's */
#define UI_BUTTON_OWN_STYLE_PALETTE 0x200 /* the palette byte of packedTextStyle replaces the state style's */
#define UI_BUTTON_HIDDEN_WHILE_SUPPRESSED 0x400
#define UI_BUTTON_NO_FRAME_WHILE_SUPPRESSED 0x800 /* framed text button */
#define UI_BUTTON_NO_FOCUS_MARK 0x1000

/* Packed text style (UiPackedTextStyle): a control's own style (packedTextStyle, styleOverride) may replace
   the font byte and the palette byte of the state style (UI_BUTTON_OWN_STYLE_*, UI_LABEL_OWN_STYLE_*). */
#define UI_TEXT_STYLE_FONT_BYTE 0xff000000u
#define UI_TEXT_STYLE_PALETTE_BYTE 0x00ff0000u

/* Functions are grouped by semantic ownership. */

void UiGraphicsAdapterTextButton_DrawFormattedAdapterText
          (UiPixelCoordinate clipBottom,UiPixelCoordinate clipRight,UiPixelCoordinate clipTop,
          UiPixelCoordinate clipLeft,UiTextButtonControl *control);

void UiNumericPairTextButton_DrawFormattedValues
          (UiPixelCoordinate clipBottom,UiPixelCoordinate clipRight,UiPixelCoordinate clipTop,
          UiPixelCoordinate clipLeft,UiNumericPairTextButton *control);

void UiPayloadPairTextButton_DrawFormattedPayloads
          (UiPixelCoordinate clipBottom,UiPixelCoordinate clipRight,UiPixelCoordinate clipTop,
          UiPixelCoordinate clipLeft,UiPayloadPairTextButton *control);

void UiFramedTextButtonControl_Relocate(UiSerializedRelocationDelta relocationDelta,UiFramedTextButtonControl *control);

void UiFramedTextButtonControl_DrawClipped
          (UiPixelCoordinate clipBottom,UiPixelCoordinate clipRight,UiPixelCoordinate clipTop,
          UiPixelCoordinate clipLeft,UiFramedTextButtonControl *control);

void UiFramedTextButtonControl_NonRightPress
          (UiPointerWheelDelta wheelDelta,UiPixelCoordinate pointerY,UiPixelCoordinate pointerX,
          UiFramedTextButtonControl *control);

void UiFramedTextButtonControl_NonRightRelease
          (UiPointerWheelDelta wheelDelta,UiPixelCoordinate pointerY,UiPixelCoordinate pointerX,
          UiFramedTextButtonControl *control);

void UiFramedTextButtonControl_NonRightDrag
          (UiPointerWheelDelta wheelDelta,UiPixelCoordinate pointerY,UiPixelCoordinate pointerX,
          UiFramedTextButtonControl *control);

UiNodeBase * UiFramedTextButtonControl_HitTestRect
          (UiPixelCoordinate pointerY,UiPixelCoordinate pointerX,UiFramedTextButtonControl *control);

void UiTextButtonControl_Relocate(UiSerializedRelocationDelta relocationDelta,UiTextButtonControl *control);

void UiTextButtonControl_NonRightPress
          (UiPointerWheelDelta wheelDelta,UiPixelCoordinate pointerY,UiPixelCoordinate pointerX,
          UiTextButtonControl *control);

Bool8 UiTextButtonControl_KeyboardEvent(UiKeyboardStateMask keyboardStateMask,UiKeyboardEventCode keyCode,
          UiTextButtonControl *control);

void UiTextButtonControl_DrawClipped(UiPixelCoordinate clipBottom,UiPixelCoordinate clipRight,UiPixelCoordinate clipTop,
          UiPixelCoordinate clipLeft,UiTextButtonControl *control);

extern UiNodeVtable g_UiGraphicsAdapterTextButtonVtable;
extern UiNodeVtable g_UiFramedTextButtonControlVtable; /* (framed text button) */

extern UiNodeVtable g_UiTextButtonControlVtable; /* (text button) */

extern UiNodeVtable g_UiNumericPairTextButtonVtable;
extern UiNodeVtable g_UiPayloadPairTextButtonVtable;

extern const uint32_t g_UiTextStyleNormal;
extern const int32_t g_UiWindowFrameInset;

/* Read live by the blend overread emulation (graphics/backend/software_raster.h). */
extern uint16_t g_GraphicsAdapterFormatScratch0Utf16[16];
extern uint16_t g_GraphicsAdapterFormatScratch1Utf16[16];
extern const UiPackedTextStyle g_UiTextStyleSelected;
extern const UiPackedTextStyle g_UiTextStyleDisabled;

#endif /* THANDOR_UI_CONTROLS_TEXT_BUTTONS_H */

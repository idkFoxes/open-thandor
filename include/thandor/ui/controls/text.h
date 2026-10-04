/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/include/thandor/ui/controls/text.h
 * Reverse engineering by idkFoxes 2026
 */

#ifndef THANDOR_UI_CONTROLS_TEXT_H
#define THANDOR_UI_CONTROLS_TEXT_H

#include <thandor/generated/types.h>
#include <thandor/core/contracts.h>

/* Submodule: ui/controls/text. */

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

/* panelFlags of UiImagePanelControl (also the base of UiArmyMetricsPanel). */
#define UI_IMAGE_PANEL_CENTER_X 0x01
#define UI_IMAGE_PANEL_ALIGN_RIGHT 0x02
#define UI_IMAGE_PANEL_CENTER_Y 0x04
#define UI_IMAGE_PANEL_ALIGN_BOTTOM 0x08
#define UI_IMAGE_PANEL_DROP_SHADOW 0x10
#define UI_IMAGE_PANEL_NEVER_HIT 0x20
#define UI_IMAGE_PANEL_HIT_WHOLE_BOX 0x40 /* skip the opaque-pixel test */
#define UI_IMAGE_PANEL_STRETCH 0x80 /* stretch the texture over the layout box */

/* fillFlags of UiFillPanelControl. */
#define UI_FILL_PANEL_TILE_X 0x01
#define UI_FILL_PANEL_TILE_Y 0x02
#define UI_FILL_PANEL_DROP_SHADOW 0x10

/* gaugeFlags of UiFormattedContainer. */
#define UI_GAUGE_TWO_SIDED_SCALE 0x01
#define UI_GAUGE_HAS_MARKER 0x02 /* the node is a UiFormattedContainerWithMarker */
/* UiFormattedContainer_DrawClipped: frame offsets from firstFrameSubresource. Each bar look is three frames
   (left cap, tiled middle, right cap); the empty bar is look 0, the six fill colours start at 3, 6, ... 18. */
#define UI_GAUGE_FRAME_TRACK 1
#define UI_GAUGE_FRAME_END_CAP 2
#define UI_GAUGE_FRAME_MARKER 21

/* editStateFlags bits of UiPathTextEditControl beyond UiTextEditStateFlags; bits 1-2 are passed on to
   g_FileSystemValidateDos83Path. */
#define UI_PATH_TEXT_ALLOW_WILDCARDS 0x02 /* accept '*' and '?' */
#define UI_PATH_TEXT_NAME_ONLY 0x04 /* refuse ':' and '\' */

/* Code units of the fixed text buffers (the terminator included). */
#define UI_NUMERIC_TEXT_BUFFER_UNITS 16
#define UI_PATH_TEXT_BUFFER_UNITS 256
/* Byte capacity of each expanded-text scratch buffer of UiPointerList_CompareExpandedText. */
#define UI_POINTER_LIST_COMPARE_SCRATCH_BYTES 0x400

/* The top byte of editStateFlags (text edits) and listStateFlags (text lists) counts frames down. */
#define UI_STATE_FRAME_COUNTER_UNIT 0x1000000
#define UI_STATE_FLAGS_MASK 0xffffff /* the flag bits below the counter */

/* Packed text style (UiPackedTextStyle): a control's own style (packedTextStyle, styleOverride) may replace
   the font byte and the palette byte of the state style (UI_BUTTON_OWN_STYLE_*, UI_LABEL_OWN_STYLE_*). */
#define UI_TEXT_STYLE_FONT_BYTE 0xff000000u
#define UI_TEXT_STYLE_PALETTE_BYTE 0x00ff0000u

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

Bool8 UiNumericTextEditControl_HandleKeyboardAndCommit(UiKeyboardStateMask keyboardStateMask,UiKeyboardEventCode keyCode,
          UiNumericTextControl *control);

Bool8 UiPathTextEditControl_HandleKeyboardAndValidate(UiKeyboardStateMask keyboardStateMask,UiKeyboardEventCode keyCode,
          UiPathTextEditControl *control);

Bool8 UiRequiredTextEditControl_HandleKeyboardAndValidate
          (UiKeyboardStateMask keyboardStateMask,UiKeyboardEventCode keyCode,
          UiRequiredTextEditControl *control);

void UiGraphicsAdapterTextButton_DrawFormattedAdapterText
          (UiPixelCoordinate clipBottom,UiPixelCoordinate clipRight,UiPixelCoordinate clipTop,
          UiPixelCoordinate clipLeft,UiTextButtonControl *control);

void UiNumericTextEditControl_RelocateAndRebuildText
          (UiSerializedRelocationDelta relocationDelta,UiNumericTextControl *control);

void UiTextEditControl_DrawTextSelectionAndCaret
          (UiPixelCoordinate clipBottom,UiPixelCoordinate clipRight,UiPixelCoordinate clipTop,
          UiPixelCoordinate clipLeft,UiTextEditControl *control);

void UiTextEditControl_BeginSelectionAtPointer
          (UiPointerWheelDelta wheelDelta,UiPixelCoordinate pointerY,UiPixelCoordinate pointerX,
          UiTextEditControl *control);

void UiTextEditControl_UpdateSelectionFromPointer
          (UiPointerWheelDelta wheelDelta,UiPixelCoordinate pointerY,UiPixelCoordinate pointerX,
          UiTextEditControl *control);

void UiPathTextEditControl_RelocateAndValidateDos83
          (UiSerializedRelocationDelta relocationDelta,UiPathTextEditControl *control);

void UiRequiredTextEditControl_RelocateAndValidateNonEmpty
          (UiSerializedRelocationDelta relocationDelta,UiRequiredTextEditControl *control);

void UiPointerList_SortByExpandedTextFieldAscending
          (UiPointerListFieldByteOffset fieldOffset,UiPointerListControl *control);

void UiNumericPairTextButton_DrawFormattedValues
          (UiPixelCoordinate clipBottom,UiPixelCoordinate clipRight,UiPixelCoordinate clipTop,
          UiPixelCoordinate clipLeft,UiNumericPairTextButton *control);

void UiPayloadPairTextButton_DrawFormattedPayloads
          (UiPixelCoordinate clipBottom,UiPixelCoordinate clipRight,UiPixelCoordinate clipTop,
          UiPixelCoordinate clipLeft,UiPayloadPairTextButton *control);

void UiTooltip_Draw(UiPixelCoordinate clipBottom,UiPixelCoordinate clipRight,UiPixelCoordinate clipTop,
              UiPixelCoordinate clipLeft);

Bool8 UiRootStack_PopUntilWindowTextureBoundary(void);

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

void UiWindowControl_DrawFramedTextAndChrome
          (UiPixelCoordinate clipBottom,UiPixelCoordinate clipRight,UiPixelCoordinate clipTop,
          UiPixelCoordinate clipLeft,UiWindowControl *control);

void UiTextButtonControl_Relocate(UiSerializedRelocationDelta relocationDelta,UiTextButtonControl *control);

void UiTextButtonControl_NonRightPress
          (UiPointerWheelDelta wheelDelta,UiPixelCoordinate pointerY,UiPixelCoordinate pointerX,
          UiTextButtonControl *control);

Bool8 UiTextButtonControl_KeyboardEvent(UiKeyboardStateMask keyboardStateMask,UiKeyboardEventCode keyCode,
          UiTextButtonControl *control);

void UiImagePanelControl_DrawAlignedTextureAndChildren
          (UiPixelCoordinate clipBottom,UiPixelCoordinate clipRight,UiPixelCoordinate clipTop,
          UiPixelCoordinate clipLeft,UiImagePanelControl *control);

UiNodeBase * UiImagePanelControl_HitTestAlignedTextureAndChildren(int pointerY,int pointerX,UiImagePanelControl *control);

void UiFillPanelControl_DrawColorOrTiledTextureAndChildren
          (UiPixelCoordinate clipBottom,UiPixelCoordinate clipRight,UiPixelCoordinate clipTop,
          UiPixelCoordinate clipLeft,UiFillPanelControl *control);

void UiTextEditControl_EndSelection
               (UiPointerWheelDelta wheelDelta,UiPixelCoordinate pointerY,UiPixelCoordinate pointerX
               ,UiTextEditControl *control);

void UiTextEditControl_SuppressIfActionId(UiActionId actionId,UiTextEditControl *control);

void UiTextEditControl_UnsuppressIfActionId(UiActionId actionId,UiTextEditControl *control);

void UiTextEditControl_TickCaretBlink(UiTextEditControl *control);

void UiSingleLineTextControl_DrawClipped
          (UiPixelCoordinate clipBottom,UiPixelCoordinate clipRight,UiPixelCoordinate clipTop,
          UiPixelCoordinate clipLeft,UiSingleLineTextControl *control);

void UiTextListControl_DrawRowsAndSelection
          (UiPixelCoordinate clipBottom,UiPixelCoordinate clipRight,UiPixelCoordinate clipTop,
          UiPixelCoordinate clipLeft,UiTextListControl *control);

void UiTextListControl_SelectRowFromPointer
          (UiPointerWheelDelta wheelDelta,UiPixelCoordinate pointerY,UiPixelCoordinate pointerX,
          UiTextListControl *control);

Bool8 UiTextListControl_HandleKeyboardNavigationAndSearch
          (UiKeyboardStateMask keyboardStateMask,UiKeyboardEventCode keyCode,
          UiTextListControl *control);

void UiTextListControl_TickActivationPulse(UiTextListControl *control);

void UiTextListControl_UnsuppressIfActionId(UiActionId actionId,UiTextListControl *control);

void UiTextListControl_SuppressIfActionId(UiActionId actionId,UiTextListControl *control);

void UiPointerList_InitializeMeasuredTextRows(UiListRowCount rowCount,Ptr32<void> *rowPointers,UiPointerListControl *control);

void UiWrappedTextControl_DrawClipped(UiPixelCoordinate clipBottom,UiPixelCoordinate clipRight,UiPixelCoordinate clipTop,
          UiPixelCoordinate clipLeft,UiWrappedTextControl *control);

void UiNineSlicePanelControl_DrawTextureFrameAndChildren
          (UiPixelCoordinate clipBottom,UiPixelCoordinate clipRight,UiPixelCoordinate clipTop,
          UiPixelCoordinate clipLeft,UiNineSlicePanelControl *control);

void UiFormattedContainer_RelocateWithPatchedTextPayloads
          (UiSerializedRelocationDelta relocationDelta,UiFormattedContainer *control);

void UiFormattedContainer_DrawClipped
          (int clipBottom,int clipRight,int clipTop,int clipLeft,UiFormattedContainer *control);

void UiArmyMetricsPanel_DrawTextureMetricsAndChildren
          (UiPixelCoordinate clipBottom,UiPixelCoordinate clipRight,UiPixelCoordinate clipTop,
          UiPixelCoordinate clipLeft,UiArmyMetricsPanel *control);

void UiSoftwareTexturePreviewControl_DrawScaledTextureAndChildren
          (UiPixelCoordinate clipBottom,UiPixelCoordinate clipRight,UiPixelCoordinate clipTop,
          UiPixelCoordinate clipLeft,UiSoftwareTexturePreviewControl *control);

void UiSoftwareTexturePreviewControl_EnqueueActionOnPrimaryPress
          (UiPointerWheelDelta wheelDelta,UiPixelCoordinate pointerY,UiPixelCoordinate pointerX,
          UiSoftwareTexturePreviewControl *control);

void UiSoftwareTexturePreviewControl_EnqueueActionOnSecondaryPress
          (UiPointerWheelDelta wheelDelta,UiPixelCoordinate pointerY,UiPixelCoordinate pointerX,
          UiSoftwareTexturePreviewControl *control);

Bool8 UiSoftwareTexturePreviewControl_HandleKeyboardActivation
          (UiKeyboardStateMask keyboardStateMask,UiKeyboardEventCode keyCode,UiSoftwareTexturePreviewControl *control);

void UiTooltip_UpdateHoverTarget(UiPixelCoordinate pointerY,UiPixelCoordinate pointerX);

void UiNumericTextControl_RebuildTextFromValue(UiNumericTextControl *control);

void UiNumericTextControl_ParseAndCommitValue(UiNumericTextControl *control);

void UiTooltip_PrepareTargetText(UiNodeBase *node);

void UiNumericTextControl_UpdateRangeValidity(UiNumericTextControl *control);

UiPixelCoordinate UiTextEditControl_MeasurePrefixWidth(UiTextCodeUnitCount prefixLength,UiTextEditControl *control);

UiTextCodeUnitCount UiTextEditControl_FindCursorIndexAtX(UiPixelCoordinate pointerX,UiTextEditControl *control);

void UiPathTextControl_UpdateDos83Validity(UiPathTextEditControl *control);

void UiTextControl_UpdateNonEmptyValidity(UiTextEditControl *control);

int UiPointerList_CompareExpandedText(uint16_t *rightText,uint16_t *leftText);

void UiTextEditControl_RecomputeLayoutAndClampScroll(UiTextEditControl *control);

void UiTextButtonControl_DrawClipped(UiPixelCoordinate clipBottom,UiPixelCoordinate clipRight,UiPixelCoordinate clipTop,
          UiPixelCoordinate clipLeft,UiTextButtonControl *control);

extern UiNodeVtable g_UiGraphicsAdapterTextButtonVtable;
extern UiNodeVtable g_UiFramedTextButtonControlVtable; /* (framed text button) */
extern UiNodeVtable g_UiWindowControlVtable;
extern UiNodeVtable g_UiTextButtonControlVtable; /* (text button) */
extern UiNodeVtable g_UiImagePanelControlVtable;
extern UiNodeVtable g_UiNumericTextEditControlVtable;
extern UiNodeVtable g_UiPathTextEditControlVtable;
extern UiNodeVtable g_UiRequiredTextEditControlVtable;
extern UiNodeVtable g_UiTextListControlVtable;
extern UiNodeVtable g_UiNineSlicePanelControlVtable;
extern UiNodeVtable g_UiNumericPairTextButtonVtable;
extern UiNodeVtable g_UiPayloadPairTextButtonVtable;
extern UiNodeVtable g_UiFormattedContainerVtable;
extern UiNodeVtable g_UiArmyMetricsPanelVtable;
extern UiNodeVtable g_UiSoftwareTexturePreviewControlVtable;

extern UiTooltipState g_UiTooltipState;
extern AudioMixerGainQ15 g_UiSoundGainQ15;
extern const UiFrameDelayFrames g_UiListActivationPulseFrames; /* UiFrameDelayFrames, 8: frames of the activation pulse after Enter on a list/text list before its action is queued (src/ui/controls/lists.c, text.c). */
extern const uint32_t g_UiTextStyleNormal;
extern const int32_t g_UiWindowFrameInset;
extern const uint32_t g_UiListTextStyle;

/* Read live by the blend overread emulation (graphics/backend/software_raster.h). */
extern uint16_t g_GraphicsAdapterFormatScratch0Utf16[16];
extern uint16_t g_GraphicsAdapterFormatScratch1Utf16[16];

#endif /* THANDOR_UI_CONTROLS_TEXT_H */

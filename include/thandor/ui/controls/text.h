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
#define UI_BUTTON_PLAY_ACTIVATION_SOUND 0x80 /* play activationSoundId on activation */
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

/* editStateFlags bits of UiPathTextEditControl beyond UiTextEditStateFlags; bits 1-2 are passed on to
   g_FileSystemValidateDos83Path. */
#define UI_PATH_TEXT_ALLOW_WILDCARDS 0x02 /* accept '*' and '?' */
#define UI_PATH_TEXT_NAME_ONLY 0x04 /* refuse ':' and '\' */

/* Code units of the fixed text buffers (the terminator included). */
#define UI_NUMERIC_TEXT_BUFFER_UNITS 16
#define UI_PATH_TEXT_BUFFER_UNITS 256

/* The top byte of editStateFlags (text edits) and listStateFlags (text lists) counts frames down. */
#define UI_STATE_FRAME_COUNTER_UNIT 0x1000000
/* Functions are grouped by semantic ownership; address comments are executable virtual addresses. */

/* 0x004B0200 */
void UiTooltip_TickCountdown(void);

/* 0x004B5F20 */
bool UiNumericTextEditControl_HandleKeyboardAndCommit(UiKeyboardStateMask keyboardStateMask,UiKeyboardEventCode keyCode,
          UiNumericTextControl *control);

/* 0x004B68C0 */
bool UiPathTextEditControl_HandleKeyboardAndValidate(UiKeyboardStateMask keyboardStateMask,UiKeyboardEventCode keyCode,
          UiPathTextEditControl *control);

/* 0x004B7110 */
bool UiRequiredTextEditControl_HandleKeyboardAndValidate
          (UiKeyboardStateMask keyboardStateMask,UiKeyboardEventCode keyCode,
          UiRequiredTextEditControl *control);

/* 0x004227B0 */
void UiGraphicsAdapterTextButton_DrawFormattedAdapterText
          (UiPixelCoordinate clipTop,UiPixelCoordinate clipLeft,UiPixelCoordinate clipBottom,
          UiPixelCoordinate clipRight,UiTextButtonControl *control);

/* 0x004B58F0 */
void UiNumericTextEditControl_RelocateAndRebuildText
          (UiSerializedRelocationDelta relocationDelta,UiNumericTextControl *control);

/* 0x004B5960 */
void UiTextEditControl_DrawTextSelectionAndCaret
          (UiPixelCoordinate clipTop,UiPixelCoordinate clipLeft,UiPixelCoordinate clipBottom,
          UiPixelCoordinate clipRight,UiTextEditControl *control);

/* 0x004B5DF0 */
void UiTextEditControl_BeginSelectionAtPointer
          (UiPointerWheelDelta wheelDelta,UiPixelCoordinate pointerY,UiPixelCoordinate pointerX,
          UiTextEditControl *control);

/* 0x004B5EA0 */
void UiTextEditControl_UpdateSelectionFromPointer
          (UiPointerWheelDelta wheelDelta,UiPixelCoordinate pointerY,UiPixelCoordinate pointerX,
          UiTextEditControl *control);

/* 0x004B6850 */
void UiPathTextEditControl_RelocateAndValidateDos83
          (UiSerializedRelocationDelta relocationDelta,UiPathTextEditControl *control);

/* 0x004B70A0 */
void UiRequiredTextEditControl_RelocateAndValidateNonEmpty
          (UiSerializedRelocationDelta relocationDelta,UiRequiredTextEditControl *control);

/* 0x004BB5C0 */
void UiPointerList_SortByExpandedTextFieldAscending
          (UiPointerListFieldByteOffset fieldOffset,UiPointerListControl *control);

/* 0x004BB6B0 */
void UiPointerList_SortByExpandedTextFieldDescending
          (UiPointerListFieldByteOffset fieldOffset,UiPointerListControl *control);

/* 0x005156A0 */
void UiNumericPairTextButton_DrawFormattedValues
          (UiPixelCoordinate clipTop,UiPixelCoordinate clipLeft,UiPixelCoordinate clipBottom,
          UiPixelCoordinate clipRight,UiNumericPairTextButton *control);

/* 0x00515780 */
void UiPayloadPairTextButton_DrawFormattedPayloads
          (UiPixelCoordinate clipTop,UiPixelCoordinate clipLeft,UiPixelCoordinate clipBottom,
          UiPixelCoordinate clipRight,UiPayloadPairTextButton *control);

/* 0x004B0320 */
void UiTooltip_Draw(UiPixelCoordinate clipBottom,UiPixelCoordinate clipRight,UiPixelCoordinate clipTop,
              UiPixelCoordinate clipLeft);

/* 0x004B0F90 */
bool UiRootStack_PopUntilWindowTextureBoundary(void);

/* 0x004B1DD0 */
void UiFramedTextButtonControl_Relocate(UiSerializedRelocationDelta relocationDelta,UiFramedTextButtonControl *control);

/* 0x004B1E10 */
void UiFramedTextButtonControl_DrawClipped
          (UiPixelCoordinate clipTop,UiPixelCoordinate clipLeft,UiPixelCoordinate clipBottom,
          UiPixelCoordinate clipRight,UiFramedTextButtonControl *control);

/* 0x004B22A0 */
void UiFramedTextButtonControl_NonRightPress
          (UiPointerWheelDelta wheelDelta,UiPixelCoordinate pointerY,UiPixelCoordinate pointerX,
          UiFramedTextButtonControl *control);

/* 0x004B2380 */
void UiFramedTextButtonControl_NonRightRelease
          (UiPointerWheelDelta wheelDelta,UiPixelCoordinate pointerY,UiPixelCoordinate pointerX,
          UiFramedTextButtonControl *control);

/* 0x004B23F0 */
void UiFramedTextButtonControl_NonRightDrag
          (UiPointerWheelDelta wheelDelta,UiPixelCoordinate pointerY,UiPixelCoordinate pointerX,
          UiFramedTextButtonControl *control);

/* 0x004B24C0 */
UiNodeBase * UiFramedTextButtonControl_HitTestRect
          (UiPixelCoordinate pointerY,UiPixelCoordinate pointerX,UiFramedTextButtonControl *control);

/* 0x004B27D0 */
void UiWindowControl_DrawFramedTextAndChrome
          (UiPixelCoordinate clipTop,UiPixelCoordinate clipLeft,UiPixelCoordinate clipBottom,
          UiPixelCoordinate clipRight,UiWindowControl *control);

/* 0x004B2E40 */
void UiTextButtonControl_Relocate(UiSerializedRelocationDelta relocationDelta,UiTextButtonControl *control);

/* 0x004B31B0 */
void UiTextButtonControl_NonRightPress
          (UiPointerWheelDelta wheelDelta,UiPixelCoordinate pointerY,UiPixelCoordinate pointerX,
          UiTextButtonControl *control);

/* 0x004B32C0 */
bool UiTextButtonControl_KeyboardEvent(UiKeyboardStateMask keyboardStateMask,UiKeyboardEventCode keyCode,
          UiTextButtonControl *control);

/* 0x004B37C0 */
void UiImagePanelControl_DrawAlignedTextureAndChildren
          (UiPixelCoordinate clipTop,UiPixelCoordinate clipLeft,UiPixelCoordinate clipBottom,
          UiPixelCoordinate clipRight,UiImagePanelControl *control);

/* 0x004B3960 */
UiNodeBase * UiImagePanelControl_HitTestAlignedTextureAndChildren(int pointerY,int pointerX,UiImagePanelControl *control);

/* 0x004B3AA0 */
void UiFillPanelControl_DrawColorOrTiledTextureAndChildren
          (UiPixelCoordinate clipTop,UiPixelCoordinate clipLeft,UiPixelCoordinate clipBottom,
          UiPixelCoordinate clipRight,UiFillPanelControl *control);

/* 0x004B5E60 */
void UiTextEditControl_EndSelection
               (UiPointerWheelDelta wheelDelta,UiPixelCoordinate pointerY,UiPixelCoordinate pointerX
               ,UiTextEditControl *control);

/* 0x004B6480 */
void UiTextEditControl_SuppressIfActionId(UiActionId actionId,UiTextEditControl *control);

/* 0x004B64B0 */
void UiTextEditControl_UnsuppressIfActionId(UiActionId actionId,UiTextEditControl *control);

/* 0x004B64E0 */
void UiTextEditControl_TickCaretBlink(UiTextEditControl *control);

/* 0x004B95E0 */
void UiSingleLineTextControl_DrawClipped
          (UiPixelCoordinate clipTop,UiPixelCoordinate clipLeft,UiPixelCoordinate clipBottom,
          UiPixelCoordinate clipRight,UiSingleLineTextControl *control);

/* 0x004B9E90 */
void UiTextListControl_DrawRowsAndSelection
          (UiPixelCoordinate clipTop,UiPixelCoordinate clipLeft,UiPixelCoordinate clipBottom,
          UiPixelCoordinate clipRight,UiTextListControl *control);

/* 0x004BA040 */
void UiTextListControl_SelectRowFromPointer
          (UiPointerWheelDelta wheelDelta,UiPixelCoordinate pointerY,UiPixelCoordinate pointerX,
          UiTextListControl *control);

/* 0x004BA130 */
bool UiTextListControl_HandleKeyboardNavigationAndSearch
          (UiKeyboardStateMask keyboardStateMask,UiKeyboardEventCode keyCode,
          UiTextListControl *control);

/* 0x004BA390 */
void UiTextListControl_TickActivationPulse(UiTextListControl *control);

/* 0x004BA3D0 */
void UiTextListControl_UnsuppressIfActionId(UiActionId actionId,UiTextListControl *control);

/* 0x004BA400 */
void UiTextListControl_SuppressIfActionId(UiActionId actionId,UiTextListControl *control);

/* 0x004BA430 */
void UiPointerList_InitializeMeasuredTextRows(UiListRowCount rowCount,void **rowPointers,UiPointerListControl *control);

/* 0x004BC490 */
void UiWrappedTextControl_DrawClipped(UiPixelCoordinate clipTop,UiPixelCoordinate clipLeft,UiPixelCoordinate clipBottom,
          UiPixelCoordinate clipRight,UiWrappedTextControl *control);

/* 0x004BCC80 */
void UiNineSlicePanelControl_DrawTextureFrameAndChildren
          (UiPixelCoordinate clipTop,UiPixelCoordinate clipLeft,UiPixelCoordinate clipBottom,
          UiPixelCoordinate clipRight,UiNineSlicePanelControl *control);

/* 0x00515830 */
void UiFormattedContainer_RelocateWithPatchedTextPayloads
          (UiSerializedRelocationDelta relocationDelta,UiFormattedContainer *control);

/* 0x005158B0 */
void UiFormattedContainer_DrawClipped
          (int clipTop,int clipLeft,int clipBottom,int clipRight,UiFormattedContainer *control);

/* 0x00516D10 */
void UiArmyMetricsPanel_DrawTextureMetricsAndChildren
          (UiPixelCoordinate clipTop,UiPixelCoordinate clipLeft,UiPixelCoordinate clipBottom,
          UiPixelCoordinate clipRight,UiArmyMetricsPanel *control);

/* 0x00519110 */
void UiSoftwareTexturePreviewControl_DrawScaledTextureAndChildren
          (UiPixelCoordinate clipTop,UiPixelCoordinate clipLeft,UiPixelCoordinate clipBottom,
          UiPixelCoordinate clipRight,UiSoftwareTexturePreviewControl *control);

/* 0x00519190 */
void UiSoftwareTexturePreviewControl_EnqueueActionOnPrimaryPress
          (UiPointerWheelDelta wheelDelta,UiPixelCoordinate pointerY,UiPixelCoordinate pointerX,
          UiSoftwareTexturePreviewControl *control);

/* 0x005191B0 */
void UiSoftwareTexturePreviewControl_EnqueueActionOnSecondaryPress
          (UiPointerWheelDelta wheelDelta,UiPixelCoordinate pointerY,UiPixelCoordinate pointerX,
          UiSoftwareTexturePreviewControl *control);

/* 0x005191D0 */
bool UiSoftwareTexturePreviewControl_HandleKeyboardActivation
          (UiKeyboardStateMask keyboardStateMask,UiKeyboardEventCode keyCode,UiSoftwareTexturePreviewControl *control);

/* 0x004B0150 */
void UiTooltip_UpdateHoverTarget(UiPixelCoordinate pointerY,UiPixelCoordinate pointerX);

/* 0x004B6520 */
void UiNumericTextControl_RebuildTextFromValue(UiNumericTextControl *control);

/* 0x004B65F0 */
void UiNumericTextControl_ParseAndCommitValue(UiNumericTextControl *control);

/* 0x004B0250 */
void UiTooltip_PrepareTargetText(UiNodeBase *node);

/* 0x004B66E0 */
void UiNumericTextControl_UpdateRangeValidity(UiNumericTextControl *control);

/* 0x004B6740 */
UiPixelCoordinate UiTextEditControl_MeasurePrefixWidth(UiTextCodeUnitCount prefixLength,UiTextEditControl *control);

/* 0x004B6790 */
UiTextCodeUnitCount UiTextEditControl_FindCursorIndexAtX(UiPixelCoordinate pointerX,UiTextEditControl *control);

/* 0x004B7010 */
void UiPathTextControl_UpdateDos83Validity(UiPathTextEditControl *control);

/* 0x004B78F0 */
void UiTextControl_UpdateNonEmptyValidity(UiTextEditControl *control);

/* 0x004BB570 */
TextCompareResult UiPointerList_CompareExpandedTextFlags(uint16_t *rightText,uint16_t *leftText);

/* 0x004B5D00 */
void UiTextEditControl_RecomputeLayoutAndClampScroll(UiTextEditControl *control);

/* 0x004B2E60 */
void UiTextButtonControl_DrawClipped(UiPixelCoordinate clipTop,UiPixelCoordinate clipLeft,UiPixelCoordinate clipBottom,
          UiPixelCoordinate clipRight,UiTextButtonControl *control);

#endif /* THANDOR_UI_CONTROLS_TEXT_H */

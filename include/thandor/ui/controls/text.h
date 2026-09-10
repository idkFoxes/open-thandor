#ifndef THANDOR_UI_CONTROLS_TEXT_H
#define THANDOR_UI_CONTROLS_TEXT_H

#include <thandor/generated/v523_types.h>
#include <thandor/core/contracts.h>

/* Submodule: ui/controls/text. */
/* Functions are grouped by semantic ownership; address comments are executable virtual addresses. */

/* 0x004B0200 */
void UiTooltip_TickCountdown(void);

/* 0x004B5F20 */
void UiNumericTextEditControl_HandleKeyboardAndCommitCf (UiKeyboardStateMask keyboardStateMask,UiKeyboardEventCode keyCode, UiNumericTextControl *control);

/* 0x004B68C0 */
void UiPathTextEditControl_HandleKeyboardAndValidateCf (UiKeyboardStateMask keyboardStateMask,UiKeyboardEventCode keyCode, UiPathTextEditControl *control);

/* 0x004B7110 */
void UiRequiredTextEditControl_HandleKeyboardAndValidateCf (UiKeyboardStateMask keyboardStateMask,UiKeyboardEventCode keyCode, UiRequiredTextEditControl *control);

/* 0x004227B0 */
void UiGraphicsAdapterTextButton_DrawFormattedAdapterText (UiPixelCoordinate clipTop,UiPixelCoordinate clipLeft,UiPixelCoordinate clipBottom, UiPixelCoordinate clipRight,UiTextButtonControl *control);

/* 0x004B58F0 */
void UiNumericTextEditControl_RelocateAndRebuildText (UiSerializedRelocationDelta relocationDelta,UiNumericTextControl *control);

/* 0x004B5960 */
void UiTextEditControl_DrawTextSelectionAndCaret (UiPixelCoordinate clipTop,UiPixelCoordinate clipLeft,UiPixelCoordinate clipBottom, UiPixelCoordinate clipRight,UiTextEditControl *control);

/* 0x004B5DF0 */
void UiTextEditControl_BeginSelectionAtPointer (UiPointerWheelDelta wheelDelta,UiPixelCoordinate pointerY,UiPixelCoordinate pointerX ,UiTextEditControl *control);

/* 0x004B5EA0 */
void UiTextEditControl_UpdateSelectionFromPointer (UiPointerWheelDelta wheelDelta,UiPixelCoordinate pointerY,UiPixelCoordinate pointerX ,UiTextEditControl *control);

/* 0x004B6850 */
void UiPathTextEditControl_RelocateAndValidateDos83 (UiSerializedRelocationDelta relocationDelta,UiPathTextEditControl *control);

/* 0x004B70A0 */
void UiRequiredTextEditControl_RelocateAndValidateNonEmpty (UiSerializedRelocationDelta relocationDelta,UiRequiredTextEditControl *control);

/* 0x004BB5C0 */
void UiPointerList_SortByExpandedTextFieldAscending (UiPointerListFieldByteOffset fieldOffset,UiPointerListControl *control);

/* 0x004BB6B0 */
undefined8 UiPointerList_SortByExpandedTextFieldDescending (UiPointerListFieldByteOffset fieldOffset,UiPointerListControl *control);

/* 0x005156A0 */
void UiNumericPairTextButton_DrawFormattedValues (UiPixelCoordinate clipTop,UiPixelCoordinate clipLeft,UiPixelCoordinate clipBottom, UiPixelCoordinate clipRight,UiTextButtonControl *control);

/* 0x00515780 */
void UiPayloadPairTextButton_DrawFormattedPayloads (UiPixelCoordinate clipTop,UiPixelCoordinate clipLeft,UiPixelCoordinate clipBottom, UiPixelCoordinate clipRight,UiTextButtonControl *control);

/* 0x004B0320 */
void UiTooltip_Draw(UiPixelCoordinate clipBottom,UiPixelCoordinate clipRight, UiPixelCoordinate clipTop,UiPixelCoordinate clipLeft);

/* 0x004B0F90 */
undefined4 UiRootStack_PopUntilWindowTextureBoundaryCf(void);

/* 0x004B1DD0 */
void UiFramedTextButtonControl_Relocate (UiSerializedRelocationDelta relocationDelta,UiFramedTextButtonControl *control);

/* 0x004B1E10 */
void UiFramedTextButtonControl_DrawClipped (UiPixelCoordinate clipTop,UiPixelCoordinate clipLeft,UiPixelCoordinate clipBottom, UiPixelCoordinate clipRight,UiFramedTextButtonControl *control);

/* 0x004B22A0 */
void UiFramedTextButtonControl_NonRightPress (UiPointerWheelDelta wheelDelta,UiPixelCoordinate pointerY,UiPixelCoordinate pointerX ,UiFramedTextButtonControl *control);

/* 0x004B2380 */
void UiFramedTextButtonControl_NonRightRelease (UiPointerWheelDelta wheelDelta,UiPixelCoordinate pointerY,UiPixelCoordinate pointerX ,UiFramedTextButtonControl *control);

/* 0x004B23F0 */
void UiFramedTextButtonControl_NonRightDrag (UiPointerWheelDelta wheelDelta,UiPixelCoordinate pointerY,UiPixelCoordinate pointerX ,UiFramedTextButtonControl *control);

/* 0x004B24C0 */
UiNodeBase * UiFramedTextButtonControl_HitTestRect (UiPixelCoordinate pointerY,UiPixelCoordinate pointerX,UiFramedTextButtonControl *control);

/* 0x004B27D0 */
void UiWindowControl_DrawFramedTextAndChrome (UiPixelCoordinate clipTop,UiPixelCoordinate clipLeft,UiPixelCoordinate clipBottom, UiPixelCoordinate clipRight,UiNodeBase *control);

/* 0x004B2E40 */
void UiTextButtonControl_Relocate (UiSerializedRelocationDelta relocationDelta,UiTextButtonControl *control);

/* 0x004B31B0 */
void UiTextButtonControl_NonRightPress (UiPointerWheelDelta wheelDelta,UiPixelCoordinate pointerY,UiPixelCoordinate pointerX ,UiTextButtonControl *control);

/* 0x004B32C0 */
void UiTextButtonControl_KeyboardEventCf (UiKeyboardStateMask keyboardStateMask,UiKeyboardEventCode keyCode, UiTextButtonControl *control);

/* 0x004B37C0 */
void UiImagePanelControl_DrawAlignedTextureAndChildren (UiPixelCoordinate clipTop,UiPixelCoordinate clipLeft,UiPixelCoordinate clipBottom, UiPixelCoordinate clipRight,UiNodeBase *control);

/* 0x004B3960 */
UiNodeBase * UiImagePanelControl_HitTestAlignedTextureAndChildren(int pointerY,int pointerX,UiNodeBase *control);

/* 0x004B3AA0 */
void UiFillPanelControl_DrawColorOrTiledTextureAndChildren (UiPixelCoordinate clipTop,UiPixelCoordinate clipLeft,UiPixelCoordinate clipBottom, UiPixelCoordinate clipRight,UiNodeBase *control);

/* 0x004B5E60 */
void UiTextEditControl_EndSelection(void);

/* 0x004B6480 */
void UiTextEditControl_SuppressIfActionId(UiActionId actionId,UiTextEditControl *control);

/* 0x004B64B0 */
void UiTextEditControl_UnsuppressIfActionId(UiActionId actionId,UiTextEditControl *control);

/* 0x004B64E0 */
void UiTextEditControl_TickCaretBlink(UiTextEditControl *control);

/* 0x004B95E0 */
void UiSingleLineTextControl_DrawClipped (UiPixelCoordinate clipTop,UiPixelCoordinate clipLeft,UiPixelCoordinate clipBottom, UiPixelCoordinate clipRight,UiNodeBase *control);

/* 0x004B9E90 */
void UiTextListControl_DrawRowsAndSelection (UiPixelCoordinate clipTop,UiPixelCoordinate clipLeft,UiPixelCoordinate clipBottom, UiPixelCoordinate clipRight,UiTextListControl *control);

/* 0x004BA040 */
void UiTextListControl_SelectRowFromPointer (UiPointerWheelDelta wheelDelta,UiPixelCoordinate pointerY,UiPixelCoordinate pointerX ,UiTextListControl *control);

/* 0x004BA130 */
void UiTextListControl_HandleKeyboardNavigationAndSearchCf (UiKeyboardStateMask keyboardStateMask,UiKeyboardEventCode keyCode, UiTextListControl *control);

/* 0x004BA390 */
void UiTextListControl_TickActivationPulse(UiTextListControl *control);

/* 0x004BA3D0 */
void UiTextListControl_UnsuppressIfActionId(UiActionId actionId,UiTextListControl *control);

/* 0x004BA400 */
void UiTextListControl_SuppressIfActionId(UiActionId actionId,UiTextListControl *control);

/* 0x004BA430 */
void UiPointerList_InitializeMeasuredTextRows (UiListRowCount rowCount,void **rowPointers,UiPointerListControl *control);

/* 0x004BC490 */
void UiWrappedTextControl_DrawClipped (UiPixelCoordinate clipTop,UiPixelCoordinate clipLeft,UiPixelCoordinate clipBottom, UiPixelCoordinate clipRight,UiNodeBase *control);

/* 0x004BCC80 */
void UiNineSlicePanelControl_DrawTextureFrameAndChildren (UiPixelCoordinate clipTop,UiPixelCoordinate clipLeft,UiPixelCoordinate clipBottom, UiPixelCoordinate clipRight,UiNodeBase *control);

/* 0x00515830 */
void UiFormattedContainer_RelocateWithPatchedTextPayloads (UiSerializedRelocationDelta relocationDelta,UiNodeBase *control);

/* 0x005158B0 */
void UiFormattedContainer_DrawClipped (int clipTop,int clipLeft,int clipBottom,int clipRight,UiNodeBase *control);

/* 0x00516D10 */
void UiArmyMetricsPanel_DrawTextureMetricsAndChildren (UiPixelCoordinate clipTop,UiPixelCoordinate clipLeft,UiPixelCoordinate clipBottom, UiPixelCoordinate clipRight,UiNodeBase *control);

/* 0x00519110 */
void UiSoftwareTexturePreviewControl_DrawScaledTextureAndChildren (UiPixelCoordinate clipTop,UiPixelCoordinate clipLeft,UiPixelCoordinate clipBottom, UiPixelCoordinate clipRight,UiNodeBase *control);

/* 0x00519190 */
void UiSoftwareTexturePreviewControl_EnqueueActionOnPrimaryPress(void);

/* 0x005191B0 */
void UiSoftwareTexturePreviewControl_EnqueueActionOnSecondaryPress(void);

/* 0x005191D0 */
void UiSoftwareTexturePreviewControl_HandleKeyboardActivationCf (UiKeyboardStateMask keyboardStateMask,UiKeyboardEventCode keyCode, UiNodeBase *control);

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
undefined8 UiTextEditControl_MeasurePrefixWidth(UiTextCodeUnitCount prefixLength,UiTextEditControl *control);

/* 0x004B6790 */
undefined8 UiTextEditControl_FindCursorIndexAtX(UiPixelCoordinate pointerX,UiTextEditControl *control);

/* 0x004B7010 */
void UiPathTextControl_UpdateDos83Validity(UiPathTextEditControl *control);

/* 0x004B78F0 */
undefined4 UiTextControl_UpdateNonEmptyValidity(UiTextEditControl *control);

/* 0x004BB570 */
undefined8 UiPointerList_CompareExpandedTextFlags(word *rightText,word *leftText);

/* 0x004B5D00 */
undefined8 UiTextEditControl_RecomputeLayoutAndClampScroll(UiTextEditControl *control);

/* 0x004B2E60 */
void UiTextButtonControl_DrawClipped (UiPixelCoordinate clipTop,UiPixelCoordinate clipLeft,UiPixelCoordinate clipBottom, UiPixelCoordinate clipRight,UiTextButtonControl *control);

#endif /* THANDOR_UI_CONTROLS_TEXT_H */

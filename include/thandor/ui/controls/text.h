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
/* Functions are grouped by semantic ownership; address comments are executable virtual addresses. */

/* 0x004B0200 */
void __thandor_void_preserve_eax_ecx_edx UiTooltip_TickCountdown(void);

/* 0x004B5F20 */
bool __thandor_cf_preserve_eax_ecx_edx
UiNumericTextEditControl_HandleKeyboardAndCommitCf
          (UiKeyboardStateMask keyboardStateMask,UiKeyboardEventCode keyCode,
          UiNumericTextControl *control);

/* 0x004B68C0 */
bool __thandor_cf_preserve_eax_ecx_edx
UiPathTextEditControl_HandleKeyboardAndValidateCf
          (UiKeyboardStateMask keyboardStateMask,UiKeyboardEventCode keyCode,
          UiPathTextEditControl *control);

/* 0x004B7110 */
bool __thandor_cf_preserve_eax_ecx_edx
UiRequiredTextEditControl_HandleKeyboardAndValidateCf
          (UiKeyboardStateMask keyboardStateMask,UiKeyboardEventCode keyCode,
          UiRequiredTextEditControl *control);

/* 0x004227B0 */
void __thandor_void_preserve_eax_ecx_edx
UiGraphicsAdapterTextButton_DrawFormattedAdapterText
          (UiPixelCoordinate clipTop,UiPixelCoordinate clipLeft,UiPixelCoordinate clipBottom,
          UiPixelCoordinate clipRight,UiTextButtonControl *control);

/* 0x004B58F0 */
void __thandor_void_preserve_eax_ecx_edx
UiNumericTextEditControl_RelocateAndRebuildText
          (UiSerializedRelocationDelta relocationDelta,UiNumericTextControl *control);

/* 0x004B5960 */
void __thandor_void_preserve_eax_ecx_edx
UiTextEditControl_DrawTextSelectionAndCaret
          (UiPixelCoordinate clipTop,UiPixelCoordinate clipLeft,UiPixelCoordinate clipBottom,
          UiPixelCoordinate clipRight,UiTextEditControl *control);

/* 0x004B5DF0 */
void __thandor_preserve_eax
UiTextEditControl_BeginSelectionAtPointer
          (UiPointerWheelDelta wheelDelta,UiPixelCoordinate pointerY,UiPixelCoordinate pointerX,
          UiTextEditControl *control);

/* 0x004B5EA0 */
void __thandor_preserve_eax_edx
UiTextEditControl_UpdateSelectionFromPointer
          (UiPointerWheelDelta wheelDelta,UiPixelCoordinate pointerY,UiPixelCoordinate pointerX,
          UiTextEditControl *control);

/* 0x004B6850 */
void __thandor_void_preserve_eax_ecx_edx
UiPathTextEditControl_RelocateAndValidateDos83
          (UiSerializedRelocationDelta relocationDelta,UiPathTextEditControl *control);

/* 0x004B70A0 */
void __thandor_void_preserve_eax_ecx_edx
UiRequiredTextEditControl_RelocateAndValidateNonEmpty
          (UiSerializedRelocationDelta relocationDelta,UiRequiredTextEditControl *control);

/* 0x004BB5C0 */
void __thandor_void_preserve_eax_ecx_edx
UiPointerList_SortByExpandedTextFieldAscending
          (UiPointerListFieldByteOffset fieldOffset,UiPointerListControl *control);

/* 0x004BB6B0 */
void __thandor_void_preserve_eax_ecx_edx
UiPointerList_SortByExpandedTextFieldDescending
          (UiPointerListFieldByteOffset fieldOffset,UiPointerListControl *control);

/* 0x005156A0 */
void __thandor_void_preserve_eax_ecx_edx
UiNumericPairTextButton_DrawFormattedValues
          (UiPixelCoordinate clipTop,UiPixelCoordinate clipLeft,UiPixelCoordinate clipBottom,
          UiPixelCoordinate clipRight,UiTextButtonControl *control);

/* 0x00515780 */
void __thandor_void_preserve_eax_ecx_edx
UiPayloadPairTextButton_DrawFormattedPayloads
          (UiPixelCoordinate clipTop,UiPixelCoordinate clipLeft,UiPixelCoordinate clipBottom,
          UiPixelCoordinate clipRight,UiTextButtonControl *control);

/* 0x004B0320 */
void __thandor_void_preserve_eax_ecx_edx
UiTooltip_Draw(UiPixelCoordinate clipBottom,UiPixelCoordinate clipRight,UiPixelCoordinate clipTop,
              UiPixelCoordinate clipLeft);

/* 0x004B0F90 */
bool __thandor_cf_preserve_eax UiRootStack_PopUntilWindowTextureBoundaryCf(void);

/* 0x004B1DD0 */
void __thandor_void_preserve_eax_ecx_edx
UiFramedTextButtonControl_Relocate
          (UiSerializedRelocationDelta relocationDelta,UiFramedTextButtonControl *control);

/* 0x004B1E10 */
void __thandor_void_preserve_eax_ecx_edx
UiFramedTextButtonControl_DrawClipped
          (UiPixelCoordinate clipTop,UiPixelCoordinate clipLeft,UiPixelCoordinate clipBottom,
          UiPixelCoordinate clipRight,UiFramedTextButtonControl *control);

/* 0x004B22A0 */
void __thandor_void_preserve_eax_ecx_edx
UiFramedTextButtonControl_NonRightPress
          (UiPointerWheelDelta wheelDelta,UiPixelCoordinate pointerY,UiPixelCoordinate pointerX,
          UiFramedTextButtonControl *control);

/* 0x004B2380 */
void __thandor_void_preserve_eax_ecx_edx
UiFramedTextButtonControl_NonRightRelease
          (UiPointerWheelDelta wheelDelta,UiPixelCoordinate pointerY,UiPixelCoordinate pointerX,
          UiFramedTextButtonControl *control);

/* 0x004B23F0 */
void __thandor_preserve_eax
UiFramedTextButtonControl_NonRightDrag
          (UiPointerWheelDelta wheelDelta,UiPixelCoordinate pointerY,UiPixelCoordinate pointerX,
          UiFramedTextButtonControl *control);

/* 0x004B24C0 */
UiNodeBase * __thandor_eax_preserve_ecx_edx
UiFramedTextButtonControl_HitTestRect
          (UiPixelCoordinate pointerY,UiPixelCoordinate pointerX,UiFramedTextButtonControl *control);

/* 0x004B27D0 */
void __thandor_void_preserve_eax_ecx_edx
UiWindowControl_DrawFramedTextAndChrome
          (UiPixelCoordinate clipTop,UiPixelCoordinate clipLeft,UiPixelCoordinate clipBottom,
          UiPixelCoordinate clipRight,UiNodeBase *control);

/* 0x004B2E40 */
void __thandor_void_preserve_eax_ecx_edx
UiTextButtonControl_Relocate
          (UiSerializedRelocationDelta relocationDelta,UiTextButtonControl *control);

/* 0x004B31B0 */
void __thandor_void_preserve_ecx_edx
UiTextButtonControl_NonRightPress
          (UiPointerWheelDelta wheelDelta,UiPixelCoordinate pointerY,UiPixelCoordinate pointerX,
          UiTextButtonControl *control);

/* 0x004B32C0 */
bool __thandor_cf_preserve_eax_ecx_edx
UiTextButtonControl_KeyboardEventCf
          (UiKeyboardStateMask keyboardStateMask,UiKeyboardEventCode keyCode,
          UiTextButtonControl *control);

/* 0x004B37C0 */
void __thandor_void_preserve_eax_ecx_edx
UiImagePanelControl_DrawAlignedTextureAndChildren
          (UiPixelCoordinate clipTop,UiPixelCoordinate clipLeft,UiPixelCoordinate clipBottom,
          UiPixelCoordinate clipRight,UiNodeBase *control);

/* 0x004B3960 */
UiNodeBase * __thandor_eax_preserve_ecx_edx
UiImagePanelControl_HitTestAlignedTextureAndChildren(int pointerY,int pointerX,UiNodeBase *control);

/* 0x004B3AA0 */
void __thandor_void_preserve_eax_ecx_edx
UiFillPanelControl_DrawColorOrTiledTextureAndChildren
          (UiPixelCoordinate clipTop,UiPixelCoordinate clipLeft,UiPixelCoordinate clipBottom,
          UiPixelCoordinate clipRight,UiNodeBase *control);

/* 0x004B5E60 */
void UiTextEditControl_EndSelection
               (UiPointerWheelDelta wheelDelta,UiPixelCoordinate pointerY,UiPixelCoordinate pointerX
               ,UiNodeBase *control);

/* 0x004B6480 */
void __thandor_void_preserve_eax_ecx_edx
UiTextEditControl_SuppressIfActionId(UiActionId actionId,UiTextEditControl *control);

/* 0x004B64B0 */
void __thandor_void_preserve_eax_ecx_edx
UiTextEditControl_UnsuppressIfActionId(UiActionId actionId,UiTextEditControl *control);

/* 0x004B64E0 */
void __thandor_preserve_eax UiTextEditControl_TickCaretBlink(UiTextEditControl *control);

/* 0x004B95E0 */
void __thandor_void_preserve_eax_ecx_edx
UiSingleLineTextControl_DrawClipped
          (UiPixelCoordinate clipTop,UiPixelCoordinate clipLeft,UiPixelCoordinate clipBottom,
          UiPixelCoordinate clipRight,UiNodeBase *control);

/* 0x004B9E90 */
void __thandor_void_preserve_eax_ecx_edx
UiTextListControl_DrawRowsAndSelection
          (UiPixelCoordinate clipTop,UiPixelCoordinate clipLeft,UiPixelCoordinate clipBottom,
          UiPixelCoordinate clipRight,UiTextListControl *control);

/* 0x004BA040 */
void __thandor_void_preserve_eax_ecx_edx
UiTextListControl_SelectRowFromPointer
          (UiPointerWheelDelta wheelDelta,UiPixelCoordinate pointerY,UiPixelCoordinate pointerX,
          UiTextListControl *control);

/* 0x004BA130 */
bool __thandor_cf_preserve_eax_ecx_edx
UiTextListControl_HandleKeyboardNavigationAndSearchCf
          (UiKeyboardStateMask keyboardStateMask,UiKeyboardEventCode keyCode,
          UiTextListControl *control);

/* 0x004BA390 */
void __thandor_preserve_eax UiTextListControl_TickActivationPulse(UiTextListControl *control);

/* 0x004BA3D0 */
void __thandor_void_preserve_eax_ecx_edx
UiTextListControl_UnsuppressIfActionId(UiActionId actionId,UiTextListControl *control);

/* 0x004BA400 */
void __thandor_void_preserve_eax_ecx_edx
UiTextListControl_SuppressIfActionId(UiActionId actionId,UiTextListControl *control);

/* 0x004BA430 */
void __thandor_void_preserve_eax_ecx_edx
UiPointerList_InitializeMeasuredTextRows
          (UiListRowCount rowCount,void **rowPointers,UiPointerListControl *control);

/* 0x004BC490 */
void __thandor_void_preserve_eax_ecx_edx
UiWrappedTextControl_DrawClipped
          (UiPixelCoordinate clipTop,UiPixelCoordinate clipLeft,UiPixelCoordinate clipBottom,
          UiPixelCoordinate clipRight,UiNodeBase *control);

/* 0x004BCC80 */
void __thandor_void_preserve_eax_ecx_edx
UiNineSlicePanelControl_DrawTextureFrameAndChildren
          (UiPixelCoordinate clipTop,UiPixelCoordinate clipLeft,UiPixelCoordinate clipBottom,
          UiPixelCoordinate clipRight,UiNodeBase *control);

/* 0x00515830 */
void __thandor_void_preserve_eax_ecx_edx
UiFormattedContainer_RelocateWithPatchedTextPayloads
          (UiSerializedRelocationDelta relocationDelta,UiNodeBase *control);

/* 0x005158B0 */
void __thandor_void_preserve_eax_ecx_edx
UiFormattedContainer_DrawClipped
          (int clipTop,int clipLeft,int clipBottom,int clipRight,UiNodeBase *control);

/* 0x00516D10 */
void __thandor_void_preserve_eax_ecx_edx
UiArmyMetricsPanel_DrawTextureMetricsAndChildren
          (UiPixelCoordinate clipTop,UiPixelCoordinate clipLeft,UiPixelCoordinate clipBottom,
          UiPixelCoordinate clipRight,UiNodeBase *control);

/* 0x00519110 */
void __thandor_void_preserve_eax_ecx_edx
UiSoftwareTexturePreviewControl_DrawScaledTextureAndChildren
          (UiPixelCoordinate clipTop,UiPixelCoordinate clipLeft,UiPixelCoordinate clipBottom,
          UiPixelCoordinate clipRight,UiNodeBase *control);

/* 0x00519190 */
void __thandor_preserve_eax
UiSoftwareTexturePreviewControl_EnqueueActionOnPrimaryPress
          (UiPointerWheelDelta wheelDelta,UiPixelCoordinate pointerY,UiPixelCoordinate pointerX,
          UiNodeBase *control);

/* 0x005191B0 */
void __thandor_preserve_eax
UiSoftwareTexturePreviewControl_EnqueueActionOnSecondaryPress
          (UiPointerWheelDelta wheelDelta,UiPixelCoordinate pointerY,UiPixelCoordinate pointerX,
          UiNodeBase *control);

/* 0x005191D0 */
bool __thandor_cf_preserve_eax_ecx_edx
UiSoftwareTexturePreviewControl_HandleKeyboardActivationCf
          (UiKeyboardStateMask keyboardStateMask,UiKeyboardEventCode keyCode,UiNodeBase *control);

/* 0x004B0150 */
void __thandor_void_preserve_eax_ecx_edx
UiTooltip_UpdateHoverTarget(UiPixelCoordinate pointerY,UiPixelCoordinate pointerX);

/* 0x004B6520 */
void __thandor_void_preserve_eax_ecx_edx
UiNumericTextControl_RebuildTextFromValue(UiNumericTextControl *control);

/* 0x004B65F0 */
void __thandor_preserve_eax_edx
UiNumericTextControl_ParseAndCommitValue(UiNumericTextControl *control);

/* 0x004B0250 */
void __thandor_void_preserve_eax_ecx_edx UiTooltip_PrepareTargetText(UiNodeBase *node);

/* 0x004B66E0 */
void __thandor_preserve_eax UiNumericTextControl_UpdateRangeValidity(UiNumericTextControl *control);

/* 0x004B6740 */
UiPixelCoordinate __thandor_eax_preserve_ecx_edx
UiTextEditControl_MeasurePrefixWidth(UiTextCodeUnitCount prefixLength,UiTextEditControl *control);

/* 0x004B6790 */
UiTextCodeUnitCount __thandor_eax_preserve_ecx_edx
UiTextEditControl_FindCursorIndexAtX(UiPixelCoordinate pointerX,UiTextEditControl *control);

/* 0x004B7010 */
void __thandor_void_preserve_eax_ecx_edx
UiPathTextControl_UpdateDos83Validity(UiPathTextEditControl *control);

/* 0x004B78F0 */
void __thandor_void_preserve_eax_ecx_edx
UiTextControl_UpdateNonEmptyValidity(UiTextEditControl *control);

/* 0x004BB570 */
CompareFlagsCfZf2 __thandor_void_preserve_eax_ecx_edx
UiPointerList_CompareExpandedTextFlags(word *rightText,word *leftText);

/* 0x004B5D00 */
void __thandor_void_preserve_eax_ecx_edx
UiTextEditControl_RecomputeLayoutAndClampScroll(UiTextEditControl *control);

/* 0x004B2E60 */
void __thandor_void_preserve_eax_ecx_edx
UiTextButtonControl_DrawClipped
          (UiPixelCoordinate clipTop,UiPixelCoordinate clipLeft,UiPixelCoordinate clipBottom,
          UiPixelCoordinate clipRight,UiTextButtonControl *control);

#endif /* THANDOR_UI_CONTROLS_TEXT_H */

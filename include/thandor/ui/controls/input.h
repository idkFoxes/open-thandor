/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/include/thandor/ui/controls/input.h
 * Reverse engineering by idkFoxes 2026
 */

#ifndef THANDOR_UI_CONTROLS_INPUT_H
#define THANDOR_UI_CONTROLS_INPUT_H

#include <thandor/generated/types.h>
#include <thandor/core/contracts.h>

/* Submodule: ui/controls/input. */
/* Functions are grouped by semantic ownership; address comments are executable virtual addresses. */

/* 0x004AF500 */
void __thandor_void_preserve_eax_ecx_edx UiPointer_DispatchPendingEvents(void);

/* 0x004B00F0 */
void __thandor_void_preserve_eax_ecx_edx UiKeyboardFocus_ReleaseNode(UiNodeBase *node);

/* 0x004AF3D0 */
void __thandor_void_preserve_eax_ecx_edx UiKeyboard_DispatchPendingEvents(void);

/* 0x004B0030 */
void __thandor_void_preserve_ecx_edx UiKeyboardFocus_SelectInitial(UiNodeBase *root);

/* 0x004B0120 */
void __thandor_void_preserve_eax_ecx_edx UiKeyboardFocus_AcquireIfNone(UiNodeBase *node);

/* 0x004B4420 */
bool __thandor_cf_preserve_eax_ecx_edx
UiRangeSliderControl_HandleKeyboardCf
          (UiKeyboardStateMask keyboardStateMask,UiKeyboardEventCode keyCode,UiNodeBase *control);

/* 0x004B9CB0 */
bool __thandor_cf_preserve_eax_ecx_edx
UiFocusProxyControl_ForwardKeyboardEventToChildCf
          (UiKeyboardStateMask keyboardStateMask,UiKeyboardEventCode keyCode,UiNodeBase *control);

/* 0x004B9DA0 */
void __thandor_preserve_eax_edx
UiFocusProxyControl_ForwardPointerWheelToChildOrParent
          (UiPointerWheelDelta wheelDelta,UiPixelCoordinate pointerY,UiPixelCoordinate pointerX,
          UiNodeBase *control);

/* 0x004B07F0 */
GraphicsCursorFrameIndex __thandor_eax_preserve_ecx_edx
UiNode_DefaultPointerMove(UiPixelCoordinate pointerY,UiPixelCoordinate pointerX,UiNodeBase *control);

/* 0x004B42D0 */
void __thandor_void_preserve_eax_ecx_edx
UiRangeSliderControl_UpdateValueFromPointer
          (UiPointerWheelDelta wheelDelta,UiPixelCoordinate pointerY,UiPixelCoordinate pointerX,
          UiNodeBase *control);

/* 0x004B4570 */
void __thandor_void_preserve_eax_ecx
UiRangeSliderControl_HandlePointerWheel
          (UiPointerWheelDelta wheelDelta,UiPixelCoordinate pointerY,UiPixelCoordinate pointerX,
          UiNodeBase *control);

/* 0x004B9580 */
void __thandor_void_preserve_eax_ecx_edx
UiFocusProxyControl_RelocateChild
          (UiSerializedRelocationDelta relocationDelta,UiNodeBase *control,UiNodeBase *controlReg);

/* 0x004B99A0 */
void __thandor_preserve_eax_edx
UiFocusProxyControl_ForwardNonRightPressToChild
          (UiPointerWheelDelta wheelDelta,UiPixelCoordinate pointerY,UiPixelCoordinate pointerX,
          UiNodeBase *control);

/* 0x004B9A00 */
void __thandor_preserve_eax_edx
UiFocusProxyControl_ForwardNonRightReleaseToChild
          (UiPointerWheelDelta wheelDelta,UiPixelCoordinate pointerY,UiPixelCoordinate pointerX,
          UiNodeBase *control);

/* 0x004B9A60 */
void __thandor_preserve_eax_edx
UiFocusProxyControl_ForwardRightPressToChild
          (UiPointerWheelDelta wheelDelta,UiPixelCoordinate pointerY,UiPixelCoordinate pointerX,
          UiNodeBase *control);

/* 0x004B9AD0 */
void __thandor_preserve_eax_edx
UiFocusProxyControl_ForwardRightReleaseToChild
          (UiPointerWheelDelta wheelDelta,UiPixelCoordinate pointerY,UiPixelCoordinate pointerX,
          UiNodeBase *control);

/* 0x004B9B30 */
void __thandor_preserve_eax_edx
UiFocusProxyControl_ForwardNonRightDragToChild
          (UiPointerWheelDelta wheelDelta,UiPixelCoordinate pointerY,UiPixelCoordinate pointerX,
          UiNodeBase *control);

/* 0x004B9B90 */
void __thandor_preserve_eax_edx
UiFocusProxyControl_ForwardRightDragToChild
          (UiPointerWheelDelta wheelDelta,UiPixelCoordinate pointerY,UiPixelCoordinate pointerX,
          UiNodeBase *control);

/* 0x004B9BF0 */
GraphicsCursorFrameIndex __thandor_eax_preserve_ecx_edx
UiFocusProxyControl_ForwardPointerMoveToChild
          (UiPixelCoordinate pointerY,UiPixelCoordinate pointerX,UiNodeBase *control);

/* 0x004B9C50 */
UiNodeBase * __thandor_eax_preserve_ecx_edx
UiFocusProxyControl_HitTestChildProxy
          (UiPixelCoordinate pointerY,UiPixelCoordinate pointerX,UiNodeBase *control);

/* 0x004B9D40 */
void __thandor_preserve_eax_edx UiFocusProxyControl_ForwardTickToChild(UiNodeBase *control);

/* 0x004BCA70 */
GraphicsCursorFrameIndex __thandor_eax_preserve_ecx_edx
UiImageControl_PointerMove
          (UiPixelCoordinate pointerY,UiPixelCoordinate pointerX,UiImageControl *control);

/* 0x00515CC0 */
void __thandor_void_preserve_eax_ecx_edx
UiSelectionGeometryControl_DrawClipped
          (int clipTop,int clipLeft,int clipBottom,int clipRight,UiSelectionGeometryControl *control
          );

/* 0x005161A0 */
void __thandor_void_preserve_eax_ecx_edx
UiSelectionGeometryControl_ConvertPointerAndEnqueueAction
          (UiPointerWheelDelta wheelDelta,UiPixelCoordinate pointerY,UiPixelCoordinate pointerX,
          UiSelectionGeometryControl *control);

/* 0x004AFA60 */
void __thandor_void_preserve_eax_ecx_edx
UiPointer_DispatchLeftPress
          (GraphicsCursorButtonState buttonMask,UiPointerWheelDelta wheelDelta,
          UiPixelCoordinate pointerY,UiPixelCoordinate pointerX);

/* 0x004AFBC0 */
void __thandor_void_preserve_eax_ecx_edx
UiPointer_DispatchMiddlePress
          (UiPointerButtonMask buttonMask,UiPointerWheelDelta wheelDelta,UiPixelCoordinate pointerY,
          UiPixelCoordinate pointerX);

/* 0x004AFD10 */
void __thandor_void_preserve_eax_ecx_edx
UiPointer_DispatchRightPress
          (UiPointerButtonMask buttonMask,UiPointerWheelDelta wheelDelta,UiPixelCoordinate pointerY,
          UiPixelCoordinate pointerX);

/* 0x004AFFA0 */
void __thandor_void_preserve_eax_ecx_edx UiKeyboardFocus_MoveNext(void);

/* 0x004AFE40 */
void __thandor_void_preserve_eax_ecx_edx
UiPointer_DispatchMotionAndWheel
          (UiPointerWheelDelta wheelDelta,UiPixelCoordinate pointerY,UiPixelCoordinate pointerX);

/* 0x004B09F0 */
void __thandor_preserve_eax_edx
UiNode_ForwardPointerWheelToParent
          (UiPointerWheelDelta wheelDelta,UiPixelCoordinate pointerY,UiPixelCoordinate pointerX,
          UiNodeBase *control);

/* 0x004B08C0 */
bool __thandor_cf_preserve_eax_ecx_edx
UiNode_DefaultKeyboardEventMoveFocusNextCf
          (UiKeyboardStateMask keyboardStateMask,UiKeyboardEventCode keyCode,UiNodeBase *control);

/* 0x004AFF60 */
void __thandor_void_preserve_eax_ecx_edx UiKeyboardFocus_Set(UiNodeBase *node);

#endif /* THANDOR_UI_CONTROLS_INPUT_H */

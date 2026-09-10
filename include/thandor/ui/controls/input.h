#ifndef THANDOR_UI_CONTROLS_INPUT_H
#define THANDOR_UI_CONTROLS_INPUT_H

#include <thandor/generated/v523_types.h>
#include <thandor/core/contracts.h>

/* Submodule: ui/controls/input. */
/* Functions are grouped by semantic ownership; address comments are executable virtual addresses. */

/* 0x004AF500 */
void __cdecl UiPointer_DispatchPendingEvents(void);

/* 0x004B00F0 */
void UiKeyboardFocus_ReleaseNode(UiNodeBase *node);

/* 0x004AF3D0 */
void __cdecl UiKeyboard_DispatchPendingEvents(void);

/* 0x004B0030 */
void UiKeyboardFocus_SelectInitial(UiNodeBase *root);

/* 0x004B0120 */
void UiKeyboardFocus_AcquireIfNone(UiNodeBase *node);

/* 0x004B4420 */
void UiRangeSliderControl_HandleKeyboardCf (UiKeyboardStateMask keyboardStateMask,UiKeyboardEventCode keyCode, UiNodeBase *control);

/* 0x004B9CB0 */
void UiFocusProxyControl_ForwardKeyboardEventToChildCf (UiKeyboardStateMask keyboardStateMask,UiKeyboardEventCode keyCode, UiNodeBase *control);

/* 0x004B9DA0 */
void UiFocusProxyControl_ForwardPointerWheelToChildOrParent (UiPointerWheelDelta wheelDelta,UiPixelCoordinate pointerY,UiPixelCoordinate pointerX ,UiNodeBase *control);

/* 0x004B07F0 */
GraphicsCursorFrameIndex UiNode_DefaultPointerMove(UiPixelCoordinate pointerY,UiPixelCoordinate pointerX,UiNodeBase *control);

/* 0x004B42D0 */
void UiRangeSliderControl_UpdateValueFromPointer (UiPointerWheelDelta wheelDelta,UiPixelCoordinate pointerY,UiPixelCoordinate pointerX ,UiNodeBase *control);

/* 0x004B4570 */
void UiRangeSliderControl_HandlePointerWheel (UiPointerWheelDelta wheelDelta,UiPixelCoordinate pointerY,UiPixelCoordinate pointerX ,UiNodeBase *control);

/* 0x004B9580 */
void UiFocusProxyControl_RelocateChild (UiSerializedRelocationDelta relocationDelta,UiNodeBase *control);

/* 0x004B99A0 */
void UiFocusProxyControl_ForwardNonRightPressToChild (UiPointerWheelDelta wheelDelta,UiPixelCoordinate pointerY,UiPixelCoordinate pointerX ,UiNodeBase *control);

/* 0x004B9A00 */
void UiFocusProxyControl_ForwardNonRightReleaseToChild (UiPointerWheelDelta wheelDelta,UiPixelCoordinate pointerY,UiPixelCoordinate pointerX ,UiNodeBase *control);

/* 0x004B9A60 */
void UiFocusProxyControl_ForwardRightPressToChild (UiPointerWheelDelta wheelDelta,UiPixelCoordinate pointerY,UiPixelCoordinate pointerX ,UiNodeBase *control);

/* 0x004B9AD0 */
void UiFocusProxyControl_ForwardRightReleaseToChild (UiPointerWheelDelta wheelDelta,UiPixelCoordinate pointerY,UiPixelCoordinate pointerX ,UiNodeBase *control);

/* 0x004B9B30 */
void UiFocusProxyControl_ForwardNonRightDragToChild (UiPointerWheelDelta wheelDelta,UiPixelCoordinate pointerY,UiPixelCoordinate pointerX ,UiNodeBase *control);

/* 0x004B9B90 */
void UiFocusProxyControl_ForwardRightDragToChild (UiPointerWheelDelta wheelDelta,UiPixelCoordinate pointerY,UiPixelCoordinate pointerX ,UiNodeBase *control);

/* 0x004B9BF0 */
GraphicsCursorFrameIndex UiFocusProxyControl_ForwardPointerMoveToChild (UiPixelCoordinate pointerY,UiPixelCoordinate pointerX,UiNodeBase *control);

/* 0x004B9C50 */
UiNodeBase * UiFocusProxyControl_HitTestChildProxy (UiPixelCoordinate pointerY,UiPixelCoordinate pointerX,UiNodeBase *control);

/* 0x004B9D40 */
void UiFocusProxyControl_ForwardTickToChild(UiNodeBase *control);

/* 0x004BCA70 */
GraphicsCursorFrameIndex UiImageControl_PointerMove (UiPixelCoordinate pointerY,UiPixelCoordinate pointerX,UiImageControl *control);

/* 0x00515CC0 */
void UiSelectionGeometryControl_DrawClipped (int clipTop,int clipLeft,int clipBottom,int clipRight,UiNodeBase *control);

/* 0x005161A0 */
void UiSelectionGeometryControl_ConvertPointerAndEnqueueAction (UiPointerWheelDelta wheelDelta,UiPixelCoordinate pointerY,UiPixelCoordinate pointerX ,UiNodeBase *control);

/* 0x004AFA60 */
void UiPointer_DispatchLeftPress (GraphicsCursorButtonState buttonMask,UiPointerWheelDelta wheelDelta, UiPixelCoordinate pointerY,UiPixelCoordinate pointerX);

/* 0x004AFBC0 */
void UiPointer_DispatchMiddlePress (UiPointerButtonMask buttonMask,UiPointerWheelDelta wheelDelta, UiPixelCoordinate pointerY,UiPixelCoordinate pointerX);

/* 0x004AFD10 */
void UiPointer_DispatchRightPress (UiPointerButtonMask buttonMask,UiPointerWheelDelta wheelDelta, UiPixelCoordinate pointerY,UiPixelCoordinate pointerX);

/* 0x004AFFA0 */
void __cdecl UiKeyboardFocus_MoveNext(void);

/* 0x004AFE40 */
void UiPointer_DispatchMotionAndWheel (UiPointerWheelDelta wheelDelta,UiPixelCoordinate pointerY,UiPixelCoordinate pointerX );

/* 0x004B09F0 */
void UiNode_ForwardPointerWheelToParent (UiPointerWheelDelta wheelDelta,UiPixelCoordinate pointerY,UiPixelCoordinate pointerX ,UiNodeBase *control);

/* 0x004B08C0 */
void UiNode_DefaultKeyboardEventMoveFocusNextCf (UiKeyboardStateMask keyboardStateMask,UiKeyboardEventCode keyCode, UiNodeBase *control);

/* 0x004AFF60 */
void UiKeyboardFocus_Set(UiNodeBase *node);

#endif /* THANDOR_UI_CONTROLS_INPUT_H */

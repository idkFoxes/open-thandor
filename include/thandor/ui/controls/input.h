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

/* Bit 31 of a press event's button mask: a repeated (double) click. UiPointer_DispatchLeftPress/RightPress
   copy it into the pressed node's UI_NODE_REPEAT_OR_DOUBLE_CLICK flag. */
#define UI_POINTER_BUTTON_REPEAT_CLICK 0x80000000
/* UiImageControl stateFlags bits 0, 1, 9 and 10, cleared when a hovered image control loses the pointer
   (UiPointer_Dispatch*Press, UiImageControl pointer handlers). */
#define UI_IMAGE_CONTROL_HOVER_STATE_BITS 0x603
/* UiImageControl stateFlags bit: hit-test against alternateSubresource instead of normalSubresource
   (UiImageControl_PointerMove). */
#define UI_IMAGE_CONTROL_ALTERNATE_HIT_SHAPE 0x40
/* Cursor frame an image control in persistent-activation mode shows over a transparent pixel with no child
   below while no image control is hovered (UiImageControl_PointerMove). */
#define UI_IMAGE_CONTROL_CURSOR_FRAME_IDLE 8
/* UiRangeSliderControl sliderFlags (UiRangeSliderControl_* in ui/controls/input.c and misc.c). */
#define UI_RANGE_SLIDER_VERTICAL 0x1
#define UI_RANGE_SLIDER_DRAGGING 0x2 /* thumb drag in progress */
#define UI_RANGE_SLIDER_CLICK_SOUND 0x4 /* play clickSound on press/release/key step */
#define UI_RANGE_SLIDER_REVERSED 0x8 /* maximum at the left/bottom */
/* Slider thumb pieces in g_UiWindowTextureSource; their size sets the usable track length. */
#define UI_RANGE_SLIDER_SUBRESOURCE_HORIZONTAL_THUMB 0xAF
#define UI_RANGE_SLIDER_SUBRESOURCE_VERTICAL_THUMB 0xB7
/* UiRangeSliderControl_DrawTrackAndThumb: each slider look is four pieces (start cap, tiled track, end cap,
   thumb) from this base; the suppressed look starts 4 pieces later, the vertical pieces 8 pieces later. */
#define UI_RANGE_SLIDER_SUBRESOURCE_BASE 0xAC
#define UI_RANGE_SLIDER_SUBRESOURCE_BASE_SUPPRESSED 0xB0
#define UI_RANGE_SLIDER_SUBRESOURCE_VERTICAL_OFFSET 8
/* labelFlags of a UiSingleLineTextControl behind g_UiFocusProxyControlVtable (UiSingleLineTextControl_*
   forwarding handlers), next to the UI_LABEL_* bits in text.h. */
#define UI_LABEL_TEXT_NEEDS_RELOCATION 0x20 /* text is still a serialized offset */
#define UI_LABEL_WHEEL_FORWARD_ACTIVE 0x400 /* re-entry guard of the pointer-wheel forwarding */
#define UI_LABEL_SWALLOW_CHARACTERS 0x8000 /* typed characters with bit 0x10 or 0x20 are consumed, not forwarded */

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
UiRangeSliderControl_HandleKeyboard
          (UiKeyboardStateMask keyboardStateMask,UiKeyboardEventCode keyCode,UiRangeSliderControl *control);

/* 0x004B9CB0 */
bool __thandor_cf_preserve_eax_ecx_edx
UiSingleLineTextControl_ForwardKeyboardEventToChild
          (UiKeyboardStateMask keyboardStateMask,UiKeyboardEventCode keyCode,UiSingleLineTextControl *control);

/* 0x004B9DA0 */
void __thandor_preserve_eax_edx
UiSingleLineTextControl_ForwardPointerWheelToChildOrParent
          (UiPointerWheelDelta wheelDelta,UiPixelCoordinate pointerY,UiPixelCoordinate pointerX,
          UiSingleLineTextControl *control);

/* 0x004B07F0 */
GraphicsCursorFrameIndex __thandor_eax_preserve_ecx_edx
UiNode_DefaultPointerMove(UiPixelCoordinate pointerY,UiPixelCoordinate pointerX,UiNodeBase *control);

/* 0x004B42D0 */
void __thandor_void_preserve_eax_ecx_edx
UiRangeSliderControl_UpdateValueFromPointer
          (UiPointerWheelDelta wheelDelta,UiPixelCoordinate pointerY,UiPixelCoordinate pointerX,
          UiRangeSliderControl *control);

/* 0x004B4570 */
void __thandor_void_preserve_eax_ecx
UiRangeSliderControl_HandlePointerWheel
          (UiPointerWheelDelta wheelDelta,UiPixelCoordinate pointerY,UiPixelCoordinate pointerX,
          UiRangeSliderControl *control);

/* 0x004B9580 */
void __thandor_void_preserve_eax_ecx_edx
UiSingleLineTextControl_RelocateChild
          (UiSerializedRelocationDelta relocationDelta,UiSingleLineTextControl *control);

/* 0x004B99A0 */
void __thandor_preserve_eax_edx
UiSingleLineTextControl_ForwardNonRightPressToChild
          (UiPointerWheelDelta wheelDelta,UiPixelCoordinate pointerY,UiPixelCoordinate pointerX,
          UiSingleLineTextControl *control);

/* 0x004B9A00 */
void __thandor_preserve_eax_edx
UiSingleLineTextControl_ForwardNonRightReleaseToChild
          (UiPointerWheelDelta wheelDelta,UiPixelCoordinate pointerY,UiPixelCoordinate pointerX,
          UiSingleLineTextControl *control);

/* 0x004B9A60 */
void __thandor_preserve_eax_edx
UiSingleLineTextControl_ForwardRightPressToChild
          (UiPointerWheelDelta wheelDelta,UiPixelCoordinate pointerY,UiPixelCoordinate pointerX,
          UiSingleLineTextControl *control);

/* 0x004B9AD0 */
void __thandor_preserve_eax_edx
UiSingleLineTextControl_ForwardRightReleaseToChild
          (UiPointerWheelDelta wheelDelta,UiPixelCoordinate pointerY,UiPixelCoordinate pointerX,
          UiSingleLineTextControl *control);

/* 0x004B9B30 */
void __thandor_preserve_eax_edx
UiSingleLineTextControl_ForwardNonRightDragToChild
          (UiPointerWheelDelta wheelDelta,UiPixelCoordinate pointerY,UiPixelCoordinate pointerX,
          UiSingleLineTextControl *control);

/* 0x004B9B90 */
void __thandor_preserve_eax_edx
UiSingleLineTextControl_ForwardRightDragToChild
          (UiPointerWheelDelta wheelDelta,UiPixelCoordinate pointerY,UiPixelCoordinate pointerX,
          UiSingleLineTextControl *control);

/* 0x004B9BF0 */
GraphicsCursorFrameIndex __thandor_eax_preserve_ecx_edx
UiSingleLineTextControl_ForwardPointerMoveToChild
          (UiPixelCoordinate pointerY,UiPixelCoordinate pointerX,UiSingleLineTextControl *control);

/* 0x004B9C50 */
UiNodeBase * __thandor_eax_preserve_ecx_edx
UiSingleLineTextControl_HitTestChildProxy
          (UiPixelCoordinate pointerY,UiPixelCoordinate pointerX,UiSingleLineTextControl *control);

/* 0x004B9D40 */
void __thandor_preserve_eax_edx UiSingleLineTextControl_ForwardTickToChild(UiSingleLineTextControl *control);

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
UiNode_DefaultKeyboardEventMoveFocusNext
          (UiKeyboardStateMask keyboardStateMask,UiKeyboardEventCode keyCode,UiNodeBase *control);

/* 0x004AFF60 */
void __thandor_void_preserve_eax_ecx_edx UiKeyboardFocus_Set(UiNodeBase *node);

#endif /* THANDOR_UI_CONTROLS_INPUT_H */

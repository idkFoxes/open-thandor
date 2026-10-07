/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/include/thandor/ui/controls/input.h
 * Reverse engineering by idkFoxes 2026
 */

#ifndef THANDOR_UI_CONTROLS_INPUT_H
#define THANDOR_UI_CONTROLS_INPUT_H

#include <thandor/core/types.h>
#include <thandor/graphics/core/types.h>
#include <thandor/ui/controls/types.h>
#include <thandor/core/contracts.h>

/* Bit 31 of a press event's button mask: a repeated (double) click. UiPointer_DispatchLeftPress/RightPress
   copy it into the pressed node's UI_NODE_REPEAT_OR_DOUBLE_CLICK flag. */
inline constexpr GraphicsCursorButtonState UI_POINTER_BUTTON_REPEAT_CLICK = GRAPHICS_CURSOR_BUTTON_DOUBLE_CLICK;
/* UiImageControl stateFlags bits 0, 1, 9 and 10, cleared when a hovered image control loses the pointer
   (UiPointer_Dispatch*Press, UiImageControl pointer handlers). */
inline constexpr UiSelectableStateFlags UI_IMAGE_CONTROL_HOVER_STATE_BITS = FromBits<UiSelectableStateFlags>(0x603);

void UiPointer_DispatchPendingEvents();

void UiKeyboardFocus_ReleaseNode(UiNodeBase *node);

void UiKeyboard_DispatchPendingEvents();

void UiKeyboardFocus_SelectInitial(UiNodeBase *root);

void UiKeyboardFocus_AcquireIfNone(UiNodeBase *node);

GraphicsCursorFrameIndex UiNode_DefaultPointerMove(UiPixelCoordinate pointerY,UiPixelCoordinate pointerX,UiNodeBase *control);

void UiPointer_DispatchLeftPress(GraphicsCursorButtonState buttonMask,UiPointerWheelDelta wheelDelta,
          UiPixelCoordinate pointerY,UiPixelCoordinate pointerX);

void UiPointer_DispatchMiddlePress
          (GraphicsCursorButtonState buttonMask,UiPointerWheelDelta wheelDelta,UiPixelCoordinate pointerY,
          UiPixelCoordinate pointerX);

void UiPointer_DispatchRightPress
          (GraphicsCursorButtonState buttonMask,UiPointerWheelDelta wheelDelta,UiPixelCoordinate pointerY,
          UiPixelCoordinate pointerX);

void UiKeyboardFocus_MoveNext();

void UiPointer_DispatchMotionAndWheel
          (UiPointerWheelDelta wheelDelta,UiPixelCoordinate pointerY,UiPixelCoordinate pointerX);

void UiNode_ForwardPointerWheelToParent
          (UiPointerWheelDelta wheelDelta,UiPixelCoordinate pointerY,UiPixelCoordinate pointerX,
          UiNodeBase *control);

bool UiNode_DefaultKeyboardEventMoveFocusNext
          (UiKeyboardStateMask keyboardStateMask,UiKeyboardEventCode keyCode,UiNodeBase *control);

void UiKeyboardFocus_Set(UiNodeBase *node);

extern UiNodeBase *g_UiPointerCaptureTarget;

extern UiPointerCaptureButton g_UiPointerCaptureButton;

#endif /* THANDOR_UI_CONTROLS_INPUT_H */

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
/* Functions are grouped by semantic ownership. */

/* Bit 31 of a press event's button mask: a repeated (double) click. UiPointer_DispatchLeftPress/RightPress
   copy it into the pressed node's UI_NODE_REPEAT_OR_DOUBLE_CLICK flag. */
#define UI_POINTER_BUTTON_REPEAT_CLICK 0x80000000
/* UiImageControl stateFlags bits 0, 1, 9 and 10, cleared when a hovered image control loses the pointer
   (UiPointer_Dispatch*Press, UiImageControl pointer handlers). */
#define UI_IMAGE_CONTROL_HOVER_STATE_BITS 0x603
/* UiImageControl stateFlags bit: hit-test against alternateSubresource instead of normalSubresource
   (UiImageControl_PointerMove). */
#define UI_IMAGE_CONTROL_ALTERNATE_HIT_SHAPE 0x40
/* Further UiImageControl stateFlags bits (UiImageControl_* in ui/controls/misc.c): POINTER_SOUND plays
   pointerActivationSound when the image opens/closes; RIGHT_BUTTON_LATCHED holds a right-button press until
   its release (UiImageControl_TickHover); PRESSED_ON_IMAGE is set by a press and cleared when the pointer
   leaves the image's opaque pixels; OPEN keeps the image open after the release; PRESS_STARTED is set by a
   press and cleared by the right-button re-dispatch. A press sets / clears the PRESS_STATE_BITS (bits 0, 1,
   9 and 11) at once. */
#define UI_IMAGE_CONTROL_POINTER_SOUND 0x20
#define UI_IMAGE_CONTROL_RIGHT_BUTTON_LATCHED 0x100
#define UI_IMAGE_CONTROL_PRESSED_ON_IMAGE 0x200
#define UI_IMAGE_CONTROL_OPEN 0x400
#define UI_IMAGE_CONTROL_PRESS_STARTED 0x800
#define UI_IMAGE_CONTROL_PRESS_STATE_BITS 0xA03
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
#define UI_RANGE_SLIDER_PIECE_TRACK 1 /* piece offsets from the look's base; +0 is the start cap */
#define UI_RANGE_SLIDER_PIECE_END_CAP 2
#define UI_RANGE_SLIDER_PIECE_THUMB 3
/* labelFlags of a UiSingleLineTextControl behind g_UiFocusProxyControlVtable (UiSingleLineTextControl_*
   forwarding handlers), next to the UI_LABEL_* bits in text.h. */
#define UI_LABEL_TEXT_NEEDS_RELOCATION 0x20 /* text is still a serialized offset */
#define UI_LABEL_WHEEL_FORWARD_ACTIVE 0x400 /* re-entry guard of the pointer-wheel forwarding */
#define UI_LABEL_SWALLOW_CHARACTERS 0x8000 /* typed characters with bit 0x10 or 0x20 are consumed, not forwarded */
/* Character-code bits UI_LABEL_SWALLOW_CHARACTERS tests (0x10 | 0x20) */
#define UI_LABEL_SWALLOWED_CHARACTER_BITS 0x30

void UiPointer_DispatchPendingEvents(void);

void UiKeyboardFocus_ReleaseNode(UiNodeBase *node);

void UiKeyboard_DispatchPendingEvents(void);

void UiKeyboardFocus_SelectInitial(UiNodeBase *root);

void UiKeyboardFocus_AcquireIfNone(UiNodeBase *node);

Bool8 UiRangeSliderControl_HandleKeyboard
          (UiKeyboardStateMask keyboardStateMask,UiKeyboardEventCode keyCode,UiRangeSliderControl *control);

Bool8 UiSingleLineTextControl_ForwardKeyboardEventToChild
          (UiKeyboardStateMask keyboardStateMask,UiKeyboardEventCode keyCode,UiSingleLineTextControl *control);

void UiSingleLineTextControl_ForwardPointerWheelToChildOrParent
          (UiPointerWheelDelta wheelDelta,UiPixelCoordinate pointerY,UiPixelCoordinate pointerX,
          UiSingleLineTextControl *control);

GraphicsCursorFrameIndex UiNode_DefaultPointerMove(UiPixelCoordinate pointerY,UiPixelCoordinate pointerX,UiNodeBase *control);

void UiRangeSliderControl_UpdateValueFromPointer
          (UiPointerWheelDelta wheelDelta,UiPixelCoordinate pointerY,UiPixelCoordinate pointerX,
          UiRangeSliderControl *control);

void UiRangeSliderControl_HandlePointerWheel
          (UiPointerWheelDelta wheelDelta,UiPixelCoordinate pointerY,UiPixelCoordinate pointerX,
          UiRangeSliderControl *control);

void UiSingleLineTextControl_RelocateChild
          (UiSerializedRelocationDelta relocationDelta,UiSingleLineTextControl *control);

void UiSingleLineTextControl_ForwardNonRightPressToChild
          (UiPointerWheelDelta wheelDelta,UiPixelCoordinate pointerY,UiPixelCoordinate pointerX,
          UiSingleLineTextControl *control);

void UiSingleLineTextControl_ForwardNonRightReleaseToChild
          (UiPointerWheelDelta wheelDelta,UiPixelCoordinate pointerY,UiPixelCoordinate pointerX,
          UiSingleLineTextControl *control);

void UiSingleLineTextControl_ForwardRightPressToChild
          (UiPointerWheelDelta wheelDelta,UiPixelCoordinate pointerY,UiPixelCoordinate pointerX,
          UiSingleLineTextControl *control);

void UiSingleLineTextControl_ForwardRightReleaseToChild
          (UiPointerWheelDelta wheelDelta,UiPixelCoordinate pointerY,UiPixelCoordinate pointerX,
          UiSingleLineTextControl *control);

void UiSingleLineTextControl_ForwardNonRightDragToChild
          (UiPointerWheelDelta wheelDelta,UiPixelCoordinate pointerY,UiPixelCoordinate pointerX,
          UiSingleLineTextControl *control);

void UiSingleLineTextControl_ForwardRightDragToChild
          (UiPointerWheelDelta wheelDelta,UiPixelCoordinate pointerY,UiPixelCoordinate pointerX,
          UiSingleLineTextControl *control);

GraphicsCursorFrameIndex UiSingleLineTextControl_ForwardPointerMoveToChild
          (UiPixelCoordinate pointerY,UiPixelCoordinate pointerX,UiSingleLineTextControl *control);

UiNodeBase * UiSingleLineTextControl_HitTestChildProxy
          (UiPixelCoordinate pointerY,UiPixelCoordinate pointerX,UiSingleLineTextControl *control);

void UiSingleLineTextControl_ForwardTickToChild(UiSingleLineTextControl *control);

GraphicsCursorFrameIndex UiImageControl_PointerMove
          (UiPixelCoordinate pointerY,UiPixelCoordinate pointerX,UiImageControl *control);

void UiSelectionGeometryControl_DrawClipped
          (int clipBottom,int clipRight,int clipTop,int clipLeft,UiSelectionGeometryControl *control
          );

void UiSelectionGeometryControl_ConvertPointerAndEnqueueAction
          (UiPointerWheelDelta wheelDelta,UiPixelCoordinate pointerY,UiPixelCoordinate pointerX,
          UiSelectionGeometryControl *control);

void UiPointer_DispatchLeftPress(GraphicsCursorButtonState buttonMask,UiPointerWheelDelta wheelDelta,
          UiPixelCoordinate pointerY,UiPixelCoordinate pointerX);

void UiPointer_DispatchMiddlePress
          (UiPointerButtonMask buttonMask,UiPointerWheelDelta wheelDelta,UiPixelCoordinate pointerY,
          UiPixelCoordinate pointerX);

void UiPointer_DispatchRightPress
          (UiPointerButtonMask buttonMask,UiPointerWheelDelta wheelDelta,UiPixelCoordinate pointerY,
          UiPixelCoordinate pointerX);

void UiKeyboardFocus_MoveNext(void);

void UiPointer_DispatchMotionAndWheel
          (UiPointerWheelDelta wheelDelta,UiPixelCoordinate pointerY,UiPixelCoordinate pointerX);

void UiNode_ForwardPointerWheelToParent
          (UiPointerWheelDelta wheelDelta,UiPixelCoordinate pointerY,UiPixelCoordinate pointerX,
          UiNodeBase *control);

Bool8 UiNode_DefaultKeyboardEventMoveFocusNext
          (UiKeyboardStateMask keyboardStateMask,UiKeyboardEventCode keyCode,UiNodeBase *control);

void UiKeyboardFocus_Set(UiNodeBase *node);

/* Not in the original: builds the bilinear scaler weight tables (called once at startup). */
void UiScaler_BuildPixelWeightTables(void);

extern UiNodeVtable g_UiFocusProxyControlVtable;
extern UiNodeVtable g_UiSelectionGeometryControlVtable;
extern UiNodeVtable g_UiCommandVisibilitySingleLineTextVtable;

extern UiNodeBase *g_UiPointerCaptureTarget;
extern UiNodeBase *g_UiKeyboardFocusNode;
extern UiPointerCaptureButton g_UiPointerCaptureButton;
extern UiImageControl * g_UiImageControlHoverTarget;

#endif /* THANDOR_UI_CONTROLS_INPUT_H */

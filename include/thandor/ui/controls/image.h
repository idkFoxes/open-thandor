/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/include/thandor/ui/controls/image.h
 * Reverse engineering by idkFoxes 2026
 */

#ifndef THANDOR_UI_CONTROLS_IMAGE_H
#define THANDOR_UI_CONTROLS_IMAGE_H

#include <thandor/ui/controls/types.h>
#include <thandor/core/contracts.h>

/* Submodule: ui/controls/image. */

/* Functions are grouped by semantic ownership. */

void UiImageControl_LayoutChildrenToParent(UiImageControl *control);

void UiImageControl_NonRightDrag(UiPointerWheelDelta wheelDelta,UiPixelCoordinate pointerY,UiPixelCoordinate pointerX,
          UiImageControl *control);

void UiImageControl_TickHover(UiImageControl *control);

void UiImageControl_DrawClipped(UiPixelCoordinate clipBottom,UiPixelCoordinate clipRight,UiPixelCoordinate clipTop,
          UiPixelCoordinate clipLeft,UiImageControl *control);

void UiImageControl_NonRightPress(UiPointerWheelDelta wheelDelta,UiPixelCoordinate pointerY,UiPixelCoordinate pointerX,
          UiImageControl *control);

void UiImageControl_NonRightRelease
          (UiPointerWheelDelta wheelDelta,UiPixelCoordinate pointerY,UiPixelCoordinate pointerX,
          UiImageControl *control);

UiNodeBase * UiImageControl_HitTestOpaque
          (UiPixelCoordinate pointerY,UiPixelCoordinate pointerX,UiImageControl *control);

extern UiNodeVtable g_UiImageControlVtable;

/* UiImageControl stateFlags bit: hit-test against alternateSubresource instead of normalSubresource
   (UiImageControl_PointerMove). */
#define UI_IMAGE_CONTROL_ALTERNATE_HIT_SHAPE 0x40

/* Further UiImageControl stateFlags bits (UiImageControl_* in ui/controls/image.cpp): POINTER_SOUND plays
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

GraphicsCursorFrameIndex UiImageControl_PointerMove
          (UiPixelCoordinate pointerY,UiPixelCoordinate pointerX,UiImageControl *control);

extern UiImageControl * g_UiImageControlHoverTarget;

#endif /* THANDOR_UI_CONTROLS_IMAGE_H */

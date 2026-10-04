/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/include/thandor/ui/controls/focus_proxy.h
 * Reverse engineering by idkFoxes 2026
 */

#ifndef THANDOR_UI_CONTROLS_FOCUS_PROXY_H
#define THANDOR_UI_CONTROLS_FOCUS_PROXY_H

#include <thandor/core/types.h>
#include <thandor/ui/controls/types.h>
#include <thandor/core/contracts.h>

/* labelFlags of a UiSingleLineTextControl behind g_UiFocusProxyControlVtable (UiSingleLineTextControl_*
   forwarding handlers), next to the UI_LABEL_* bits in text.h. */
#define UI_LABEL_TEXT_NEEDS_RELOCATION 0x20 /* text is still a serialized offset */
#define UI_LABEL_WHEEL_FORWARD_ACTIVE 0x400 /* re-entry guard of the pointer-wheel forwarding */
#define UI_LABEL_SWALLOW_CHARACTERS 0x8000 /* typed characters with bit 0x10 or 0x20 are consumed, not forwarded */
/* Character-code bits UI_LABEL_SWALLOW_CHARACTERS tests (0x10 | 0x20) */
#define UI_LABEL_SWALLOWED_CHARACTER_BITS 0x30

Bool8 UiSingleLineTextControl_ForwardKeyboardEventToChild
          (UiKeyboardStateMask keyboardStateMask,UiKeyboardEventCode keyCode,UiSingleLineTextControl *control);

void UiSingleLineTextControl_ForwardPointerWheelToChildOrParent
          (UiPointerWheelDelta wheelDelta,UiPixelCoordinate pointerY,UiPixelCoordinate pointerX,
          UiSingleLineTextControl *control);

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

extern UiNodeVtable g_UiFocusProxyControlVtable;

extern UiNodeBase *g_UiKeyboardFocusNode;

#endif /* THANDOR_UI_CONTROLS_FOCUS_PROXY_H */

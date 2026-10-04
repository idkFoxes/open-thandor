/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/src/ui/controls/focus_proxy.cpp
 * Reverse engineering by idkFoxes 2026
 */

#include <thandor/ui/controls/focus_proxy.h>
#include <thandor/thandor.h>
#include <thandor/platform/bootstrap/image.h>

/* Module data. */

UiNodeBase *g_UiKeyboardFocusNode = UI_NODE_NONE;

/* Implementation ownership: ui/controls/focus_proxy. */

/* keyboardEvent slot of g_UiFocusProxyControlVtable. Hands the key to the framed focus child and redraws
   when the child consumed it. Tab goes to the default handler (passed on); with UI_LABEL_SWALLOW_CHARACTERS
   typed characters with bit 0x10 or 0x20 set are consumed without reaching the child. Returns false
   when consumed.
*/
Bool8 UiSingleLineTextControl_ForwardKeyboardEventToChild
          (UiKeyboardStateMask keyboardStateMask,UiKeyboardEventCode keyCode,UiSingleLineTextControl *control)

{
  UiNodeBase *childControl;
  Bool8 eventResult;

  childControl = control->focusChild;
  /* key codes without a high word are typed characters (Keyboard_OnChar) and KEYBOARD_KEY_CODE_SPACE */
  if ((keyCode & KEYBOARD_KEY_CODE_FAMILY_MASK) == 0) {
    if (((control->labelFlags & UI_LABEL_SWALLOW_CHARACTERS) != 0) && ((keyCode & UI_LABEL_SWALLOWED_CHARACTER_BITS) != 0)) {
      return false;
    }
  }
  else if (keyCode == KEYBOARD_KEY_CODE_TAB) {
    eventResult = UiNode_DefaultKeyboardEventMoveFocusNext(keyboardStateMask,KEYBOARD_KEY_CODE_TAB,&control->base);
    return eventResult;
  }
  if (childControl != nullptr) {
    eventResult = childControl->vtable->keyboardEvent(keyboardStateMask,keyCode,childControl);
    if (!eventResult) {
      UiNode_InvalidateRoot(&control->base);
      return false;
    }
  }
  return true;
}

/* pointerWheel slot of g_UiFocusProxyControlVtable. Forwards the wheel to the focus child, lending it the
   keyboard focus for the call, and redraws. UI_LABEL_WHEEL_FORWARD_ACTIVE guards against re-entry: a
   wheel event that comes back while forwarding (a child passing it to its parent) goes on to this
   control's parent instead.
*/
void UiSingleLineTextControl_ForwardPointerWheelToChildOrParent
          (UiPointerWheelDelta wheelDelta,UiPixelCoordinate pointerY,UiPixelCoordinate pointerX,
          UiSingleLineTextControl *control)

{
  UiNodeBase *childControl;

  if ((control->labelFlags & UI_LABEL_WHEEL_FORWARD_ACTIVE) == 0) {
    childControl = control->focusChild;
    control->labelFlags = control->labelFlags | UI_LABEL_WHEEL_FORWARD_ACTIVE;
    if (childControl != nullptr) {
      if (&control->base == g_UiKeyboardFocusNode) {
        g_UiKeyboardFocusNode = childControl;
        childControl->nodeFlags = childControl->nodeFlags | UI_NODE_HAS_KEYBOARD_FOCUS;
      }
      childControl->vtable->pointerWheel(wheelDelta,pointerY,pointerX,childControl);
      if (childControl == g_UiKeyboardFocusNode) {
        g_UiKeyboardFocusNode = &control->base;
        childControl->nodeFlags = childControl->nodeFlags & ~UI_NODE_HAS_KEYBOARD_FOCUS;
      }
      UiNode_InvalidateRoot(&control->base);
    }
    control->labelFlags = control->labelFlags & ~UI_LABEL_WHEEL_FORWARD_ACTIVE;
    return;
  }
  UiNode_ForwardPointerWheelToParent(wheelDelta,pointerY,pointerX,&control->base);
  return;
}

/* relocate slot of g_UiFocusProxyControlVtable and g_UiCommandVisibilitySingleLineTextVtable. A label with
   a focus child becomes a fallback focus target in the child's place (the child loses its focus-target
   flags), the children and the focusChild offset are relocated, and a serialized text offset
   (UI_LABEL_TEXT_NEEDS_RELOCATION) is turned into a pointer once.
*/
void UiSingleLineTextControl_RelocateChild(UiSerializedRelocationDelta relocationDelta,UiSingleLineTextControl *control)

{
  UiSingleLineTextControl *controlReg = control;
  UiNodeFlags *childNodeFlagsField;

  if ((((controlReg->base).nodeFlags &
        (UI_NODE_FALLBACK_FOCUS_TARGET|UI_NODE_PREFERRED_FOCUS_TARGET)) == 0) &&
     (controlReg->focusChild != nullptr)) {
    (controlReg->base).nodeFlags = (controlReg->base).nodeFlags | UI_NODE_FALLBACK_FOCUS_TARGET;
  }
  UiContainer_RelocateChildren(relocationDelta,&controlReg->base);
  if (controlReg->focusChild != nullptr) {
    controlReg->focusChild = (UiNodeBase *)((uint8_t *)controlReg->focusChild + relocationDelta);
    childNodeFlagsField = &(controlReg->focusChild)->nodeFlags;
    *childNodeFlagsField =
         *childNodeFlagsField & ~(UI_NODE_FALLBACK_FOCUS_TARGET|UI_NODE_PREFERRED_FOCUS_TARGET);
  }
  if ((control->labelFlags & UI_LABEL_TEXT_NEEDS_RELOCATION) != 0) {
    control->text = (uint16_t *)((uint8_t *)control->text + relocationDelta);
    control->labelFlags = control->labelFlags & ~UI_LABEL_TEXT_NEEDS_RELOCATION;
  }
  return;
}

/* nonRightPress slot of g_UiFocusProxyControlVtable. Forwards the left press to the focus child; if this
   control has the keyboard focus, the child holds it for the duration of the call so it acts as focused.
   Redraws afterwards.
*/
void UiSingleLineTextControl_ForwardNonRightPressToChild
          (UiPointerWheelDelta wheelDelta,UiPixelCoordinate pointerY,UiPixelCoordinate pointerX,
          UiSingleLineTextControl *control)

{
  UiNodeBase *childControl;
  
  childControl = control->focusChild;
  if (childControl != nullptr) {
    if (&control->base == g_UiKeyboardFocusNode) {
      g_UiKeyboardFocusNode = childControl;
      childControl->nodeFlags = childControl->nodeFlags | UI_NODE_HAS_KEYBOARD_FOCUS;
    }
    childControl->vtable->nonRightPress(wheelDelta,pointerY,pointerX,childControl);
    if (childControl == g_UiKeyboardFocusNode) {
      g_UiKeyboardFocusNode = &control->base;
      childControl->nodeFlags = childControl->nodeFlags & ~UI_NODE_HAS_KEYBOARD_FOCUS;
    }
    UiNode_InvalidateRoot(&control->base);
  }
  return;
}

/* nonRightRelease slot of g_UiFocusProxyControlVtable. Forwards the left release to the focus child,
   lending it the keyboard focus for the call like UiSingleLineTextControl_ForwardNonRightPressToChild.
*/
void UiSingleLineTextControl_ForwardNonRightReleaseToChild
          (UiPointerWheelDelta wheelDelta,UiPixelCoordinate pointerY,UiPixelCoordinate pointerX,
          UiSingleLineTextControl *control)

{
  UiNodeBase *childControl;
  
  childControl = control->focusChild;
  if (childControl != nullptr) {
    if (&control->base == g_UiKeyboardFocusNode) {
      g_UiKeyboardFocusNode = childControl;
      childControl->nodeFlags = childControl->nodeFlags | UI_NODE_HAS_KEYBOARD_FOCUS;
    }
    childControl->vtable->nonRightRelease(wheelDelta,pointerY,pointerX,childControl);
    if (childControl == g_UiKeyboardFocusNode) {
      g_UiKeyboardFocusNode = &control->base;
      childControl->nodeFlags = childControl->nodeFlags & ~UI_NODE_HAS_KEYBOARD_FOCUS;
    }
    UiNode_InvalidateRoot(&control->base);
  }
  return;
}

/* rightPress slot of g_UiFocusProxyControlVtable. Forwards the right press to the focus child (lending it
   the keyboard focus), but not when the child's parent is this control: most rightPress handlers
   (UiNode_ForwardRightPressToParent) would hand the press straight back.
*/
void UiSingleLineTextControl_ForwardRightPressToChild
          (UiPointerWheelDelta wheelDelta,UiPixelCoordinate pointerY,UiPixelCoordinate pointerX,
          UiSingleLineTextControl *control)

{
  UiNodeBase *childControl;
  
  childControl = control->focusChild;
  if (childControl != nullptr) {
    if (&control->base == g_UiKeyboardFocusNode) {
      g_UiKeyboardFocusNode = childControl;
      childControl->nodeFlags = childControl->nodeFlags | UI_NODE_HAS_KEYBOARD_FOCUS;
    }
    if (&control->base != childControl->parent) {
      childControl->vtable->rightPress(wheelDelta,pointerY,pointerX,childControl);
    }
    if (childControl == g_UiKeyboardFocusNode) {
      g_UiKeyboardFocusNode = &control->base;
      childControl->nodeFlags = childControl->nodeFlags & ~UI_NODE_HAS_KEYBOARD_FOCUS;
    }
    UiNode_InvalidateRoot(&control->base);
  }
  return;
}

/* rightRelease slot of g_UiFocusProxyControlVtable. Forwards the right release to the focus child,
   lending it the keyboard focus for the call.
*/
void UiSingleLineTextControl_ForwardRightReleaseToChild
          (UiPointerWheelDelta wheelDelta,UiPixelCoordinate pointerY,UiPixelCoordinate pointerX,
          UiSingleLineTextControl *control)

{
  UiNodeBase *childControl;
  
  childControl = control->focusChild;
  if (childControl != nullptr) {
    if (&control->base == g_UiKeyboardFocusNode) {
      g_UiKeyboardFocusNode = childControl;
      childControl->nodeFlags = childControl->nodeFlags | UI_NODE_HAS_KEYBOARD_FOCUS;
    }
    childControl->vtable->rightRelease(wheelDelta,pointerY,pointerX,childControl);
    if (childControl == g_UiKeyboardFocusNode) {
      g_UiKeyboardFocusNode = &control->base;
      childControl->nodeFlags = childControl->nodeFlags & ~UI_NODE_HAS_KEYBOARD_FOCUS;
    }
    UiNode_InvalidateRoot(&control->base);
  }
  return;
}

/* nonRightDrag slot of g_UiFocusProxyControlVtable. Forwards the left-button drag to the focus child,
   lending it the keyboard focus for the call.
*/
void UiSingleLineTextControl_ForwardNonRightDragToChild
          (UiPointerWheelDelta wheelDelta,UiPixelCoordinate pointerY,UiPixelCoordinate pointerX,
          UiSingleLineTextControl *control)

{
  UiNodeBase *childControl;
  
  childControl = control->focusChild;
  if (childControl != nullptr) {
    if (&control->base == g_UiKeyboardFocusNode) {
      g_UiKeyboardFocusNode = childControl;
      childControl->nodeFlags = childControl->nodeFlags | UI_NODE_HAS_KEYBOARD_FOCUS;
    }
    childControl->vtable->nonRightDrag(wheelDelta,pointerY,pointerX,childControl);
    if (childControl == g_UiKeyboardFocusNode) {
      g_UiKeyboardFocusNode = &control->base;
      childControl->nodeFlags = childControl->nodeFlags & ~UI_NODE_HAS_KEYBOARD_FOCUS;
    }
    UiNode_InvalidateRoot(&control->base);
  }
  return;
}

/* rightDrag slot of g_UiFocusProxyControlVtable. Forwards the right-button drag to the focus child,
   lending it the keyboard focus for the call.
*/
void UiSingleLineTextControl_ForwardRightDragToChild
          (UiPointerWheelDelta wheelDelta,UiPixelCoordinate pointerY,UiPixelCoordinate pointerX,
          UiSingleLineTextControl *control)

{
  UiNodeBase *childControl;
  
  childControl = control->focusChild;
  if (childControl != nullptr) {
    if (&control->base == g_UiKeyboardFocusNode) {
      g_UiKeyboardFocusNode = childControl;
      childControl->nodeFlags = childControl->nodeFlags | UI_NODE_HAS_KEYBOARD_FOCUS;
    }
    childControl->vtable->rightDrag(wheelDelta,pointerY,pointerX,childControl);
    if (childControl == g_UiKeyboardFocusNode) {
      g_UiKeyboardFocusNode = &control->base;
      childControl->nodeFlags = childControl->nodeFlags & ~UI_NODE_HAS_KEYBOARD_FOCUS;
    }
    UiNode_InvalidateRoot(&control->base);
  }
  return;
}

/* pointerMove slot of g_UiFocusProxyControlVtable. Returns the focus child's cursor frame (asked with the
   keyboard focus lent to it), or the arrow (0) without a child.
*/
GraphicsCursorFrameIndex UiSingleLineTextControl_ForwardPointerMoveToChild
          (UiPixelCoordinate pointerY,UiPixelCoordinate pointerX,UiSingleLineTextControl *control)

{
  UiNodeBase *childControl;
  GraphicsCursorFrameIndex cursorFrame;
  
  cursorFrame = 0;
  childControl = control->focusChild;
  if (childControl != nullptr) {
    if (&control->base == g_UiKeyboardFocusNode) {
      g_UiKeyboardFocusNode = childControl;
      childControl->nodeFlags = childControl->nodeFlags | UI_NODE_HAS_KEYBOARD_FOCUS;
    }
    cursorFrame = childControl->vtable->pointerMove(pointerY,pointerX,childControl);
    if (childControl == g_UiKeyboardFocusNode) {
      g_UiKeyboardFocusNode = &control->base;
      childControl->nodeFlags = childControl->nodeFlags & ~UI_NODE_HAS_KEYBOARD_FOCUS;
    }
    UiNode_InvalidateRoot(&control->base);
  }
  return cursorFrame;
}

/* hitTest slot of g_UiFocusProxyControlVtable. A hit on the focus child is reported as this control, so
   the proxy receives the input and forwards it; hits on the child or the control itself count as
   misses (UI_NODE_NONE) while the child is suppressed.
*/
UiNodeBase * UiSingleLineTextControl_HitTestChildProxy
          (UiPixelCoordinate pointerY,UiPixelCoordinate pointerX,UiSingleLineTextControl *control)

{
  UiNodeBase *hitNode;
  UiNodeBase *returnedNode;
  
  hitNode = UiContainer_HitTestChildren(pointerY,pointerX,&control->base);
  if (hitNode == control->focusChild) {
    returnedNode = &control->base;
    if ((hitNode->nodeFlags & UI_NODE_SUPPRESSED) != 0) {
      returnedNode = UI_NODE_NONE;
    }
  }
  else {
    returnedNode = hitNode;
    if (((hitNode == &control->base) && (control->focusChild != nullptr)) &&
       (((control->focusChild)->nodeFlags & UI_NODE_SUPPRESSED) != 0)) {
      returnedNode = UI_NODE_NONE;
    }
  }
  return returnedNode;
}

/* tick slot of g_UiFocusProxyControlVtable. Forwards the per-frame tick to the focus child, lending it the
   keyboard focus for the call, and redraws.
*/
void UiSingleLineTextControl_ForwardTickToChild(UiSingleLineTextControl *control)

{
  UiNodeBase *childControl;
  
  childControl = control->focusChild;
  if (childControl != nullptr) {
    if (&control->base == g_UiKeyboardFocusNode) {
      g_UiKeyboardFocusNode = childControl;
      childControl->nodeFlags = childControl->nodeFlags | UI_NODE_HAS_KEYBOARD_FOCUS;
    }
    childControl->vtable->tick(childControl);
    if (childControl == g_UiKeyboardFocusNode) {
      g_UiKeyboardFocusNode = &control->base;
      childControl->nodeFlags = childControl->nodeFlags & ~UI_NODE_HAS_KEYBOARD_FOCUS;
    }
    UiNode_InvalidateRoot(&control->base);
  }
  return;
}

UiNodeVtable g_UiFocusProxyControlVtable = {
        .relocate = THANDOR_FN(UiSingleLineTextControl_RelocateChild),
        .method04 = THANDOR_FN(UiNode_DefaultMethod04_NoOp),
        .drawClipped = THANDOR_FN(UiSingleLineTextControl_DrawClipped),
        .layout = THANDOR_FN(UiContainer_LayoutChildren),
        .nonRightPress = THANDOR_FN(UiSingleLineTextControl_ForwardNonRightPressToChild),
        .nonRightRelease = THANDOR_FN(UiSingleLineTextControl_ForwardNonRightReleaseToChild),
        .rightPress = THANDOR_FN(UiSingleLineTextControl_ForwardRightPressToChild),
        .rightRelease = THANDOR_FN(UiSingleLineTextControl_ForwardRightReleaseToChild),
        .nonRightDrag = THANDOR_FN(UiSingleLineTextControl_ForwardNonRightDragToChild),
        .rightDrag = THANDOR_FN(UiSingleLineTextControl_ForwardRightDragToChild),
        .pointerMove = THANDOR_FN(UiSingleLineTextControl_ForwardPointerMoveToChild),
        .hitTest = THANDOR_FN(UiSingleLineTextControl_HitTestChildProxy),
        .keyboardEvent = THANDOR_FN(UiSingleLineTextControl_ForwardKeyboardEventToChild),
        .applyFlags = THANDOR_FN(UiNode_ApplyFlagsRecursive),
        .suppressActionId = THANDOR_FN(UiContainer_SuppressActionId),
        .unsuppressActionId = THANDOR_FN(UiContainer_UnsuppressActionId),
        .tick = THANDOR_FN(UiSingleLineTextControl_ForwardTickToChild),
        .pointerWheel = THANDOR_FN(UiSingleLineTextControl_ForwardPointerWheelToChildOrParent)};

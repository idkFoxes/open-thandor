/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/src/ui/controls/input.c
 * Reverse engineering by idkFoxes 2026
 */

#include <thandor/ui/controls/input.h>
#include <thandor/thandor.h>
#include <thandor/platform/bootstrap/image.h>

/* Diagnostics (open-thandor only): a UI link that is neither UI_NODE_NONE nor a readable node ends the
   walk as UI_NODE_NONE instead of crashing the focus traversal; the first 20 such links are logged. */
static UiNodeBase *UiKeyboard_CheckedLink(UiNodeBase *holder,const char *field,UiNodeBase *link) {
  static int logged;
  if ((link == UI_NODE_NONE) || Thandor_IsReadable(link,sizeof(UiNodeBase))) {
    return link;
  }
  if (logged < 20) {
    logged++;
    Thandor_Log("ui focus walk: node %p (vtable %s flags %08x) has bad %s link %p; focus %p (vtable %s)",
                (void *)holder,Thandor_SymbolName(holder->vtable),holder->nodeFlags,field,(void *)link,
                (void *)g_UiKeyboardFocusNode,
                g_UiKeyboardFocusNode != UI_NODE_NONE ?
                Thandor_SymbolName(g_UiKeyboardFocusNode->vtable) : "-");
  }
  return UI_NODE_NONE;
}

/* Implementation ownership: ui/controls/input. */

/* Address: 0x004AF500.
   Delivers the queued mouse events to the UI under the frame lock (polling DirectInput first when it is
   the active mouse). Presses and motion go to the node under the pointer; a release goes to the node
   that captured the pointer with that button, which then loses the capture and the pointer position is
   dispatched again as motion. A release without a matching capture is dropped.
*/
void UiPointer_DispatchPendingEvents(void)

{
  UiNodeBase *control;
  char eventKind;
  UiPixelCoordinate pointerX;
  UiPixelCoordinate pointerY;
  GraphicsCursorButtonState buttonMask;
  UiPointerWheelDelta wheelDelta;
  CursorEventResult pointerEvent;
  
  g_SpinLockAcquire(g_UiRuntimeFrameLock);
  if (g_PointerSetPosition == DirectInputMouse_SetPosition) {
    DirectInputMouse_PollBufferedEvents();
  }
  while( true ) {
    control = g_UiPointerCaptureTarget;
    pointerEvent = g_GraphicsCursorConsumeEvent();
    pointerX = pointerEvent.pointerX;
    wheelDelta = pointerEvent.wheelDelta;
    pointerY = pointerEvent.pointerY;
    buttonMask = pointerEvent.buttonState;
    if (pointerEvent.queueEmpty) break;
    eventKind = (char)pointerEvent.eventType; /* GraphicsCursorEventType */
    if (eventKind < RIGHT_RELEASE) {
      if (eventKind == MIDDLE_RELEASE) {
        if ((control != UI_NODE_NONE) &&
           (g_UiPointerCaptureButton == UI_POINTER_CAPTURE_MIDDLE)) {
          control->vtable->nonRightRelease(wheelDelta,pointerY,pointerX,control);
          g_UiPointerCaptureButton = UI_POINTER_CAPTURE_NONE;
          g_UiPointerCaptureTarget = UI_NODE_NONE;
          UiPointer_DispatchMotionAndWheel(wheelDelta,pointerY,pointerX);
          Random_NextPrimary(); /* only this release also advances the random generator */
        }
      }
      else if (eventKind < 4) { /* motion and the presses */
        if (eventKind == RIGHT_PRESS) {
          UiPointer_DispatchRightPress(buttonMask,wheelDelta,pointerY,pointerX);
        }
        else if (eventKind < MIDDLE_PRESS) {
          if (eventKind == LEFT_PRESS) {
            UiPointer_DispatchLeftPress(buttonMask,wheelDelta,pointerY,pointerX);
          }
          else {
            UiPointer_DispatchMotionAndWheel(wheelDelta,pointerY,pointerX);
          }
        }
        else {
          UiPointer_DispatchMiddlePress(buttonMask,wheelDelta,pointerY,pointerX);
        }
      }
      /* LEFT_RELEASE (and the unused code 4) */
      else if ((control != UI_NODE_NONE) &&
              (g_UiPointerCaptureButton == UI_POINTER_CAPTURE_LEFT)) {
        control->vtable->nonRightRelease(wheelDelta,pointerY,pointerX,control);
        g_UiPointerCaptureButton = UI_POINTER_CAPTURE_NONE;
        g_UiPointerCaptureTarget = UI_NODE_NONE;
        UiPointer_DispatchMotionAndWheel(wheelDelta,pointerY,pointerX);
      }
    }
    /* RIGHT_RELEASE */
    else if ((control != UI_NODE_NONE) &&
            (g_UiPointerCaptureButton == UI_POINTER_CAPTURE_RIGHT)) {
      control->vtable->rightRelease(wheelDelta,pointerY,pointerX,control);
      g_UiPointerCaptureButton = UI_POINTER_CAPTURE_NONE;
      g_UiPointerCaptureTarget = UI_NODE_NONE;
      UiPointer_DispatchMotionAndWheel(wheelDelta,pointerY,pointerX);
    }
  }
  g_SpinLockReleaseAndInvoke
            ((SpinLockReleaseCallbackProc *)g_UiRuntimePostUnlockCallback,g_UiRuntimeFrameLock);
  return;
}


/* Address: 0x004B00F0.
   Takes the keyboard focus away from node (e.g. before it is hidden or removed): the focus moves on to the
   next focus target, or is cleared when node is the only one.
*/
void UiKeyboardFocus_ReleaseNode(UiNodeBase *node)

{
  if (node == g_UiKeyboardFocusNode) {
    UiKeyboardFocus_MoveNext();
    if (node == g_UiKeyboardFocusNode) {
      UiKeyboardFocus_Set(UI_NODE_NONE);
    }
  }
  return;
}


/* Address: 0x004AF3D0.
   Delivers the queued key events to the UI under the frame lock. A key goes to the focused node; if it
   passes the key on (CF), the next focus targets in tree order get it and the first one that takes it
   receives the focus. Keys nobody takes, or pressed with no focus, go to the top root's keyboard
   fallback. While a node has captured the pointer (a mouse button is held), keys are discarded.
*/
void UiKeyboard_DispatchPendingEvents(void)

{
  bool wrappedOnce;
  bool dispatchToRoot;
  UiKeyboardEventCode keyCode;
  UiKeyboardStateMask keyboardStateMask;
  UiNodeBase *control;
  UiNodeBase *walkNode;
  bool passToNext;
  KeyboardEventResult keyboardEvent;

  g_SpinLockAcquire(g_UiRuntimeFrameLock);
  while( true ) {
    keyboardEvent = g_KeyboardReadEvent();
    if (keyboardEvent.queueEmpty) break;
    if (g_UiPointerCaptureTarget != UI_NODE_NONE) continue;
    keyboardStateMask = keyboardEvent.eventData;
    keyCode = keyboardEvent.eventCode;
    control = g_UiKeyboardFocusNode;
    dispatchToRoot = true;
    if (control != UI_NODE_NONE) {
      dispatchToRoot = false;
      wrappedOnce = false;
      passToNext = control->vtable->keyboardEvent(keyboardStateMask,keyCode,control);
      /* CF set: offer the event to the following focus targets in pre-order, wrapping around once
         through the topmost ancestor; the first one that takes it gets the keyboard focus. */
      while (passToNext) {
        walkNode = UiKeyboard_CheckedLink(control,"firstChild",control->firstChild);
        if (walkNode == UI_NODE_NONE) {
          while (walkNode = UiKeyboard_CheckedLink(control,"nextSibling",control->nextSibling),
                walkNode == UI_NODE_NONE) {
            walkNode = UiKeyboard_CheckedLink(control,"parent",control->parent);
            if (walkNode == UI_NODE_NONE) break;
            control = walkNode;
          }
          if (walkNode == UI_NODE_NONE) {
            if (wrappedOnce) {
              dispatchToRoot = true;
              break;
            }
            wrappedOnce = true;
            walkNode = control;
          }
        }
        control = walkNode;
        if ((control->nodeFlags & (UI_NODE_FALLBACK_FOCUS_TARGET|UI_NODE_PREFERRED_FOCUS_TARGET)) == 0) continue;
        if (control == g_UiKeyboardFocusNode) {
          dispatchToRoot = true;
          break;
        }
        if ((control->nodeFlags & UI_NODE_SUPPRESSED) != 0) continue;
        passToNext = control->vtable->keyboardEvent(keyboardStateMask,keyCode,control);
        if (!passToNext) {
          UiKeyboardFocus_Set(control);
        }
      }
    }
    if ((dispatchToRoot) && (g_UiRootNode != UI_ROOT_STACK_END) &&
       (g_UiRootNode->callbacks->keyboardFallback != NULL)) {
      g_UiRootNode->callbacks->keyboardFallback(keyboardStateMask,keyCode,g_UiRootNode);
    }
  }
  g_SpinLockReleaseAndInvoke
            ((SpinLockReleaseCallbackProc *)g_UiRuntimePostUnlockCallback,g_UiRuntimeFrameLock);
  return;
}


/* Address: 0x004B0030.
   Gives the keyboard focus to the first node from root on that is a preferred focus target, else to the
   last fallback focus target found (suppressed nodes are skipped); without any the focus stays as it is.
*/
void UiKeyboardFocus_SelectInitial(UiNodeBase *root)

{
  UiNodeBase *fallbackFocusNode;
  UiNodeBase *searchNodeCursor;
  UiNodeBase *nextNode;

  /* Pre-order walk starting at root (continuing past its subtree through the parents' siblings):
     the first unsuppressed preferred focus target wins, else the last unsuppressed fallback. */
  fallbackFocusNode = UI_NODE_NONE;
  searchNodeCursor = root;
  while( true ) {
    if ((searchNodeCursor->nodeFlags & UI_NODE_SUPPRESSED) == 0) {
      if ((searchNodeCursor->nodeFlags & UI_NODE_PREFERRED_FOCUS_TARGET) != 0) break;
      if ((searchNodeCursor->nodeFlags & UI_NODE_FALLBACK_FOCUS_TARGET) != 0) {
        fallbackFocusNode = searchNodeCursor;
      }
    }
    nextNode = searchNodeCursor->firstChild;
    if (nextNode == UI_NODE_NONE) {
      while ((searchNodeCursor != UI_NODE_NONE) &&
             (nextNode = searchNodeCursor->nextSibling, nextNode == UI_NODE_NONE)) {
        searchNodeCursor = searchNodeCursor->parent;
      }
      if (searchNodeCursor == UI_NODE_NONE) {
        searchNodeCursor = fallbackFocusNode;
        if (fallbackFocusNode == UI_NODE_NONE) {
          return;
        }
        break;
      }
    }
    searchNodeCursor = nextNode;
  }
  if (searchNodeCursor != g_UiKeyboardFocusNode) {
    UiKeyboardFocus_Set(searchNodeCursor);
  }
  return;
}


/* Address: 0x004B0120.
   Called when a control is unsuppressed or its page becomes active: gives it the keyboard focus if no node
   holds the focus yet and the control is a focus target.
*/
void UiKeyboardFocus_AcquireIfNone(UiNodeBase *node)

{
  if ((g_UiKeyboardFocusNode == UI_NODE_NONE) &&
     ((node->nodeFlags & (UI_NODE_FALLBACK_FOCUS_TARGET|UI_NODE_PREFERRED_FOCUS_TARGET)) != 0)) {
    UiKeyboardFocus_Set(node);
  }
  return;
}


/* Address: 0x004B4420.
   keyboardEvent slot of g_UiRangeSliderControlVtable. Left/Right (Down/Up for a vertical slider) move the
   value by stepValue, with Ctrl straight to the minimum/maximum; each step plays the click sound, queues
   actionId and redraws. Other keys, and all keys while suppressed, go to the default handler, which passes
   them on. CF clear when the key was consumed.
*/
bool UiRangeSliderControl_HandleKeyboard
          (UiKeyboardStateMask keyboardStateMask,UiKeyboardEventCode keyCode,UiRangeSliderControl *control)

{
  int32_t adjustedSliderValue;
  UiKeyboardEventCode decreaseKey;
  UiKeyboardEventCode increaseKey;
  bool delegatedResult;

  if ((control->base.nodeFlags & UI_NODE_SUPPRESSED) != 0) {
    delegatedResult = UiNode_DefaultKeyboardEventMoveFocusNext(keyboardStateMask,keyCode,&control->base);
    return delegatedResult;
  }
  if ((control->sliderFlags & UI_RANGE_SLIDER_VERTICAL) == 0) {
    decreaseKey = KEYBOARD_KEY_CODE_LEFT;
    increaseKey = KEYBOARD_KEY_CODE_RIGHT;
  }
  else {
    decreaseKey = KEYBOARD_KEY_CODE_DOWN;
    increaseKey = KEYBOARD_KEY_CODE_UP;
  }
  if (keyCode == decreaseKey) {
    if (((keyboardStateMask & KEYBOARD_STATE_CTRL) != 0) ||
       (adjustedSliderValue = control->value - control->stepValue,
       adjustedSliderValue < control->minimumValue)) {
      adjustedSliderValue = control->minimumValue;
    }
  }
  else if (keyCode == increaseKey) {
    if (((keyboardStateMask & KEYBOARD_STATE_CTRL) != 0) ||
       (adjustedSliderValue = control->value + control->stepValue,
       control->maximumValue < adjustedSliderValue)) {
      adjustedSliderValue = control->maximumValue;
    }
  }
  else {
    delegatedResult = UiNode_DefaultKeyboardEventMoveFocusNext(keyboardStateMask,keyCode,&control->base);
    return delegatedResult;
  }
  control->value = adjustedSliderValue;
  if (((control->sliderFlags & UI_RANGE_SLIDER_CLICK_SOUND) != 0) && (control->clickSound != NULL)) {
    g_SoundPlayOneShot(g_UiSoundGainQ15,g_UiSoundGainQ15,control->clickSound);
  }
  UiActionQueue_Enqueue(control->actionId,&control->base);
  UiNode_InvalidateRoot(&control->base);
  return false;
}


/* Address: 0x004B9CB0.
   keyboardEvent slot of g_UiFocusProxyControlVtable. Hands the key to the framed focus child and redraws
   when the child consumed it. Tab goes to the default handler (passed on); with UI_LABEL_SWALLOW_CHARACTERS
   typed characters with bit 0x10 or 0x20 set are consumed without reaching the child. CF clear when
   consumed.
*/
bool UiSingleLineTextControl_ForwardKeyboardEventToChild
          (UiKeyboardStateMask keyboardStateMask,UiKeyboardEventCode keyCode,UiSingleLineTextControl *control)

{
  UiNodeBase *childControl;
  bool eventResult;

  childControl = control->focusChild;
  /* key codes without a high word are typed characters (Keyboard_OnChar) and KEYBOARD_KEY_CODE_SPACE */
  if ((keyCode & KEYBOARD_KEY_CODE_FAMILY_MASK) == 0) {
    if (((control->labelFlags & UI_LABEL_SWALLOW_CHARACTERS) != 0) && ((keyCode & 0x30) != 0)) {
      return false;
    }
  }
  else if (keyCode == KEYBOARD_KEY_CODE_TAB) {
    eventResult = UiNode_DefaultKeyboardEventMoveFocusNext(keyboardStateMask,KEYBOARD_KEY_CODE_TAB,&control->base);
    return eventResult;
  }
  if (childControl != NULL) {
    eventResult = childControl->vtable->keyboardEvent(keyboardStateMask,keyCode,childControl);
    if (!eventResult) {
      UiNode_InvalidateRoot(&control->base);
      return false;
    }
  }
  return true;
}


/* Address: 0x004B9DA0.
   pointerWheel slot of g_UiFocusProxyControlVtable. Forwards the wheel to the focus child, lending it the
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
    if (childControl != NULL) {
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


/* Address: 0x004B07F0.
   Default pointerMove slot of most UI vtables (range sliders, labels, lists, ...): the node asks for
   cursor frame 0 (GRAPHICS_CURSOR_FRAME_ARROW).
*/
GraphicsCursorFrameIndex UiNode_DefaultPointerMove(UiPixelCoordinate pointerY,UiPixelCoordinate pointerX,
                                                   UiNodeBase *control)

{
  return GRAPHICS_CURSOR_FRAME_ARROW;
}


/* Address: 0x004B42D0.
   nonRightDrag slot of g_UiRangeSliderControlVtable. While the thumb is dragged, maps the pointer position
   (thumb centre) along the track onto minimumValue..maximumValue, rounded to nearest and mirrored for
   reversed sliders, then queues actionId and redraws.
*/
void UiRangeSliderControl_UpdateValueFromPointer
          (UiPointerWheelDelta wheelDelta,UiPixelCoordinate pointerY,UiPixelCoordinate pointerX,
          UiRangeSliderControl *control)

{
  uint64_t scaledOffset;
  uint32_t pointerOffset;
  int32_t sliderValue;
  uint32_t trackLength;
  TextureSizeResult thumbSize;

  if ((control->sliderFlags & UI_RANGE_SLIDER_DRAGGING) != 0) {
    if ((control->sliderFlags & UI_RANGE_SLIDER_VERTICAL) == 0) {
      thumbSize = g_GraphicsTextureSourceGetLogicalSize(UI_RANGE_SLIDER_SUBRESOURCE_HORIZONTAL_THUMB,
                                                        g_UiWindowTextureSource);
      trackLength = control->base.layoutWidth - thumbSize.logicalWidthPixels;
      if (trackLength == 0) {
        trackLength = 1;
      }
      pointerOffset = (pointerX - ((int)thumbSize.logicalWidthPixels >> 1)) - control->base.left;
      if ((int)pointerOffset < 0) {
        pointerOffset = 0;
      }
      scaledOffset = (uint64_t)pointerOffset *
              (uint64_t)(uint32_t)(control->maximumValue - control->minimumValue);
      /* offset * range / trackLength, plus one when the remainder is more than half the track */
      sliderValue = control->minimumValue +
               (uint32_t)(trackLength < (uint32_t)((int)(scaledOffset % (uint64_t)trackLength) * 2)) +
               (int)(scaledOffset / trackLength);
      if (control->maximumValue < sliderValue) {
        sliderValue = control->maximumValue;
      }
      if ((control->sliderFlags & UI_RANGE_SLIDER_REVERSED) != 0) {
        sliderValue = control->maximumValue - (sliderValue - control->minimumValue);
      }
      control->value = sliderValue;
      UiActionQueue_Enqueue(control->actionId,&control->base);
      UiNode_InvalidateRoot(&control->base);
      return;
    }
    thumbSize = g_GraphicsTextureSourceGetLogicalSize(UI_RANGE_SLIDER_SUBRESOURCE_VERTICAL_THUMB,
                                                      g_UiWindowTextureSource);
    trackLength = control->base.layoutHeight - thumbSize.logicalHeightPixels;
    if (trackLength == 0) {
      trackLength = 1;
    }
    pointerOffset = (control->base.bottom - pointerY) - ((int)thumbSize.logicalHeightPixels >> 1);
    if ((int)pointerOffset < 0) {
      pointerOffset = 0;
    }
    scaledOffset = (uint64_t)pointerOffset *
            (uint64_t)(uint32_t)(control->maximumValue - control->minimumValue);
    sliderValue = control->minimumValue +
             (uint32_t)(trackLength < (uint32_t)((int)(scaledOffset % (uint64_t)trackLength) * 2)) +
             (int)(scaledOffset / trackLength);
    if (control->maximumValue < sliderValue) {
      sliderValue = control->maximumValue;
    }
    if ((control->sliderFlags & UI_RANGE_SLIDER_REVERSED) != 0) {
      sliderValue = control->maximumValue - (sliderValue - control->minimumValue);
    }
    control->value = sliderValue;
    UiActionQueue_Enqueue(control->actionId,&control->base);
    UiNode_InvalidateRoot(&control->base);
  }
  return;
}


/* Address: 0x004B4570.
   pointerWheel slot of g_UiRangeSliderControlVtable. Unless the thumb is being dragged, each wheel notch
   moves the value by stepValue * g_UiRangeSliderDragScale, clamped to the range; then actionId is queued
   and the slider redrawn.
*/
void UiRangeSliderControl_HandlePointerWheel
          (UiPointerWheelDelta wheelDelta,UiPixelCoordinate pointerY,UiPixelCoordinate pointerX,
          UiRangeSliderControl *control)

{
  int32_t adjustedSliderValue;

  if ((control->sliderFlags & UI_RANGE_SLIDER_DRAGGING) == 0 && (control->base.nodeFlags & UI_NODE_SUPPRESSED) == 0 &&
      wheelDelta != 0) {
    adjustedSliderValue =
         control->value + wheelDelta * g_UiRangeSliderDragScale * control->stepValue;
    if (adjustedSliderValue < control->minimumValue) {
      adjustedSliderValue = control->minimumValue;
    }
    if (control->maximumValue < adjustedSliderValue) {
      adjustedSliderValue = control->maximumValue;
    }
    control->value = adjustedSliderValue;
    UiActionQueue_Enqueue(control->actionId,&control->base);
    UiNode_InvalidateRoot(&control->base);
  }
  return;
}


/* Address: 0x004B9580.
   relocate slot of g_UiFocusProxyControlVtable and g_UiCommandVisibilitySingleLineTextVtable. A label with
   a focus child becomes a fallback focus target in the child's place (the child loses its focus-target
   flags), the children and the focusChild offset are relocated, and a serialized text offset
   (UI_LABEL_TEXT_NEEDS_RELOCATION) is turned into a pointer once.
*/
void UiSingleLineTextControl_RelocateChild(UiSerializedRelocationDelta relocationDelta,UiSingleLineTextControl *control)

{
  /* EBX is the control, the same node as the stack argument; the relocate vtable slot passes
     only (delta, control). */
  UiSingleLineTextControl *controlReg = control;
  UiNodeFlags *childNodeFlagsField;

  if ((((controlReg->base).nodeFlags &
        (UI_NODE_FALLBACK_FOCUS_TARGET|UI_NODE_PREFERRED_FOCUS_TARGET)) == 0) &&
     (controlReg->focusChild != NULL)) {
    (controlReg->base).nodeFlags = (controlReg->base).nodeFlags | UI_NODE_FALLBACK_FOCUS_TARGET;
  }
  UiContainer_RelocateChildren(relocationDelta,&controlReg->base);
  if (controlReg->focusChild != NULL) {
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


/* Address: 0x004B99A0.
   nonRightPress slot of g_UiFocusProxyControlVtable. Forwards the left press to the focus child; if this
   control has the keyboard focus, the child holds it for the duration of the call so it acts as focused.
   Redraws afterwards.
*/
void UiSingleLineTextControl_ForwardNonRightPressToChild
          (UiPointerWheelDelta wheelDelta,UiPixelCoordinate pointerY,UiPixelCoordinate pointerX,
          UiSingleLineTextControl *control)

{
  UiNodeBase *childControl;
  
  childControl = control->focusChild;
  if (childControl != NULL) {
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


/* Address: 0x004B9A00.
   nonRightRelease slot of g_UiFocusProxyControlVtable. Forwards the left release to the focus child,
   lending it the keyboard focus for the call like UiSingleLineTextControl_ForwardNonRightPressToChild.
*/
void UiSingleLineTextControl_ForwardNonRightReleaseToChild
          (UiPointerWheelDelta wheelDelta,UiPixelCoordinate pointerY,UiPixelCoordinate pointerX,
          UiSingleLineTextControl *control)

{
  UiNodeBase *childControl;
  
  childControl = control->focusChild;
  if (childControl != NULL) {
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


/* Address: 0x004B9A60.
   rightPress slot of g_UiFocusProxyControlVtable. Forwards the right press to the focus child (lending it
   the keyboard focus), but not when the child's parent is this control: most rightPress handlers
   (UiNode_ForwardRightPressToParent) would hand the press straight back.
*/
void UiSingleLineTextControl_ForwardRightPressToChild
          (UiPointerWheelDelta wheelDelta,UiPixelCoordinate pointerY,UiPixelCoordinate pointerX,
          UiSingleLineTextControl *control)

{
  UiNodeBase *childControl;
  
  childControl = control->focusChild;
  if (childControl != NULL) {
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


/* Address: 0x004B9AD0.
   rightRelease slot of g_UiFocusProxyControlVtable. Forwards the right release to the focus child,
   lending it the keyboard focus for the call.
*/
void UiSingleLineTextControl_ForwardRightReleaseToChild
          (UiPointerWheelDelta wheelDelta,UiPixelCoordinate pointerY,UiPixelCoordinate pointerX,
          UiSingleLineTextControl *control)

{
  UiNodeBase *childControl;
  
  childControl = control->focusChild;
  if (childControl != NULL) {
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


/* Address: 0x004B9B30.
   nonRightDrag slot of g_UiFocusProxyControlVtable. Forwards the left-button drag to the focus child,
   lending it the keyboard focus for the call.
*/
void UiSingleLineTextControl_ForwardNonRightDragToChild
          (UiPointerWheelDelta wheelDelta,UiPixelCoordinate pointerY,UiPixelCoordinate pointerX,
          UiSingleLineTextControl *control)

{
  UiNodeBase *childControl;
  
  childControl = control->focusChild;
  if (childControl != NULL) {
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


/* Address: 0x004B9B90.
   rightDrag slot of g_UiFocusProxyControlVtable. Forwards the right-button drag to the focus child,
   lending it the keyboard focus for the call.
*/
void UiSingleLineTextControl_ForwardRightDragToChild
          (UiPointerWheelDelta wheelDelta,UiPixelCoordinate pointerY,UiPixelCoordinate pointerX,
          UiSingleLineTextControl *control)

{
  UiNodeBase *childControl;
  
  childControl = control->focusChild;
  if (childControl != NULL) {
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


/* Address: 0x004B9BF0.
   pointerMove slot of g_UiFocusProxyControlVtable. Returns the focus child's cursor frame (asked with the
   keyboard focus lent to it), or the arrow (0) without a child.
*/
GraphicsCursorFrameIndex UiSingleLineTextControl_ForwardPointerMoveToChild
          (UiPixelCoordinate pointerY,UiPixelCoordinate pointerX,UiSingleLineTextControl *control)

{
  UiNodeBase *childControl;
  GraphicsCursorFrameIndex cursorFrame;
  
  cursorFrame = 0;
  childControl = control->focusChild;
  if (childControl != NULL) {
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


/* Address: 0x004B9C50.
   hitTest slot of g_UiFocusProxyControlVtable. A hit on the focus child is reported as this control, so
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
    if (((hitNode == &control->base) && (control->focusChild != NULL)) &&
       (((control->focusChild)->nodeFlags & UI_NODE_SUPPRESSED) != 0)) {
      returnedNode = UI_NODE_NONE;
    }
  }
  return returnedNode;
}


/* Address: 0x004B9D40.
   tick slot of g_UiFocusProxyControlVtable. Forwards the per-frame tick to the focus child, lending it the
   keyboard focus for the call, and redraws.
*/
void UiSingleLineTextControl_ForwardTickToChild(UiSingleLineTextControl *control)

{
  UiNodeBase *childControl;
  
  childControl = control->focusChild;
  if (childControl != NULL) {
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


/* Address: 0x004BCA70.
   pointerMove slot of the image-control vtable at 0x004BC570. Over an opaque pixel of the image the arrow
   is shown. Over a transparent pixel of a persistent-activation image, a child under the pointer supplies
   the cursor; without one, UI_IMAGE_CONTROL_CURSOR_FRAME_IDLE while no image control is hovered.
*/
GraphicsCursorFrameIndex UiImageControl_PointerMove
          (UiPixelCoordinate pointerY,UiPixelCoordinate pointerX,UiImageControl *control)

{
  UiImageControl *hitControl;
  GraphicsCursorFrameIndex cursorFrame;
  bool overOpaquePixel;

  if (((control->selectable).base.nodeFlags & UI_NODE_SUPPRESSED) == 0) {
    if (((control->selectable).stateFlags & UI_IMAGE_CONTROL_ALTERNATE_HIT_SHAPE) == 0) {
      overOpaquePixel = g_GraphicsTextureSourceTestOpaquePixel
                        (pointerY,pointerX,(control->selectable).base.top,
                         (control->selectable).base.left,control->normalSubresource,
                         control->textureSource);
      if (overOpaquePixel) {
        return GRAPHICS_CURSOR_FRAME_ARROW;
      }
    }
    else {
      overOpaquePixel = g_GraphicsTextureSourceTestOpaquePixel
                        (pointerY,pointerX,(control->selectable).base.top,
                         (control->selectable).base.left,control->alternateSubresource,
                         control->textureSource);
      if (overOpaquePixel) {
        return GRAPHICS_CURSOR_FRAME_ARROW;
      }
    }
    if (((control->selectable).stateFlags & UI_SELECTABLE_PERSISTENT_ACTIVATION_MODE) != 0) {
      hitControl = (UiImageControl *)
                   UiContainer_HitTestChildren(pointerY,pointerX,(UiNodeBase *)control);
      if (hitControl != control) {
        cursorFrame = (*((hitControl->selectable).base.vtable)->pointerMove)
                          (pointerY,pointerX,(UiNodeBase *)hitControl);
        return cursorFrame;
      }
      if (g_UiImageControlHoverTarget == NULL) {
        return UI_IMAGE_CONTROL_CURSOR_FRAME_IDLE;
      }
    }
  }
  return GRAPHICS_CURSOR_FRAME_ARROW;
}


/* MMX lane helpers for the bilinear scaler below (lanes are little-endian 16-bit words). */

/* PUNPCKLBW mm,mm then PSRLW mm,shift: byte i of pixel becomes word lane i = (byte * 0x101) >> shift. */
static __inline uint64_t UiScaler_UnpackBytesToWordLanes(uint32_t pixel,int shift) {
  uint64_t lanes;
  int lane;

  lanes = 0;
  for (lane = 0; lane < 4; lane++) {
    lanes = lanes |
            (uint64_t)(uint16_t)((uint16_t)(((pixel >> (lane * 8)) & 0xff) * 0x101) >> shift) << (lane * 16);
  }
  return lanes;
}

/* PADDW: lane-wise wrapping 16-bit add. */
static __inline uint64_t UiScaler_AddWordLanes(uint64_t left,uint64_t right) {
  uint64_t sum;
  int shift;

  sum = 0;
  for (shift = 0; shift < 64; shift += 16) {
    sum = sum | (uint64_t)(uint16_t)((uint16_t)(left >> shift) + (uint16_t)(right >> shift)) << shift;
  }
  return sum;
}

/* PSRLW mm,shift then PACKUSWB (low dword): each lane shifted right, saturated to an unsigned
   byte. The logical shift leaves every lane non-negative, so only the 0xFF clamp applies. */
static __inline uint32_t UiScaler_ShiftAndPackWordLanes(uint64_t lanes,int shift) {
  uint32_t packed;
  uint16_t laneValue;
  int lane;

  packed = 0;
  for (lane = 0; lane < 4; lane++) {
    laneValue = (uint16_t)(lanes >> (lane * 16)) >> shift;
    packed = packed | (uint32_t)(0xff < laneValue ? 0xff : laneValue) << (lane * 8);
  }
  return packed;
}

/* Bilinear blend of a 2x2 texel quad with the scaler weight tables (256 steps per axis):
   rows blended across the column fraction, then across the row fraction, then >> 2 and packed. */
static __inline PackedArgb32 UiScaler_BlendBilinear
          (PackedArgb32 topLeft,PackedArgb32 topRight,PackedArgb32 bottomLeft,PackedArgb32 bottomRight,
          int columnWeight,int rowWeight) {
  uint64_t topRow;
  uint64_t bottomRow;

  topRow = UiScaler_AddWordLanes
                     (pmulhw(UiScaler_UnpackBytesToWordLanes(topLeft,2),
                             g_UiScalerFirstPixelWeights[columnWeight]),
                      pmulhw(UiScaler_UnpackBytesToWordLanes(topRight,2),
                             g_UiScalerSecondPixelWeights[columnWeight]));
  bottomRow = UiScaler_AddWordLanes
                        (pmulhw(UiScaler_UnpackBytesToWordLanes(bottomLeft,2),
                                g_UiScalerFirstPixelWeights[columnWeight]),
                         pmulhw(UiScaler_UnpackBytesToWordLanes(bottomRight,2),
                                g_UiScalerSecondPixelWeights[columnWeight]));
  return UiScaler_ShiftAndPackWordLanes
                   (UiScaler_AddWordLanes(pmulhw(topRow,g_UiScalerFirstPixelWeights[rowWeight]),
                                          pmulhw(bottomRow,g_UiScalerSecondPixelWeights[rowWeight])),2);
}


/* Address: 0x00515CC0.
   drawClipped slot of g_UiSelectionGeometryControlVtable. Fills the node (clipped) with its texture,
   rotated by rotationAngle and scaled by sampleScaleQ12 about sourceOrigin: every screen pixel is mapped
   back to a Q12 source position and bilinearly filtered from the 2x2 texels around it (texels outside the
   texture count as 0), for 16- and 32-bit framebuffers. Only direct-colour subresources (negative
   paletteIndex) are drawn.
*/
void UiSelectionGeometryControl_DrawClipped
          (int clipTop,int clipLeft,int clipBottom,int clipRight,UiSelectionGeometryControl *control)

{
  GraphicsTextureSourceAsset *sourceTexture;
  AssetRelativeOffset subresourceTable;
  int sourceWidth;
  int sourceHeight;
  int pixelDataOffset;
  int64_t rotationProductA;
  int64_t rotationProductB;
  int64_t rotationProductC;
  int sourceColumn;
  uint32_t sinTermOrSourceU;
  uint32_t stepTermOrRowStartU;
  uint32_t stepTermOrRowStartV;
  int sourceRow;
  int clipHeightOrColumnTerm;
  int clipWidth;
  int texelIndexOrFraction;
  uint8_t *destPixel;
  bool framebufferUnavailable;
  uint32_t pixelStepU;
  uint32_t pixelStepV;
  uint64_t rowStepU;
  int rowStepUHigh;
  int cosTermOrRowStepV;
  uint64_t sourceStartU;
  uint32_t sourceV;
  PackedArgb32 sourcePixelSample0; /* texel (column, row) */
  PackedArgb32 sourcePixelSample1; /* texel (column + 1, row) */
  PackedArgb32 sourcePixelSample2; /* texel (column, row + 1) */
  PackedArgb32 sourcePixelSample3; /* texel (column + 1, row + 1) */
  PackedArgb32 blendedPixel;
  uint64_t packedLanes;
  int remainingColumns;
  uint8_t *destRowStart;

  /* intersect the clip rectangle with the node (clipRight/clipBottom act as the left/top bound here) */
  if (clipRight < (control->base).left) {
    clipRight = (control->base).left;
  }
  if (clipBottom < (control->base).top) {
    clipBottom = (control->base).top;
  }
  if ((control->base).right < clipLeft) {
    clipLeft = (control->base).right;
  }
  if ((control->base).bottom < clipTop) {
    clipTop = (control->base).bottom;
  }
  clipWidth = clipLeft - clipRight;
  if (((clipWidth != 0 && clipRight <= clipLeft) &&
      (clipHeightOrColumnTerm = clipTop - clipBottom, clipHeightOrColumnTerm != 0 && clipBottom <= clipTop)) &&
     (control->textureSource != NULL)) {
    rotationProductA = (int64_t)control->sampleScaleQ12 * (int64_t)g_FixedCosQ28[control->rotationAngle];
    cosTermOrRowStepV = -((int)((uint64_t)rotationProductA >> 0x20) << 4 | (uint32_t)rotationProductA >> 0x1c);
    rotationProductA = (int64_t)control->sampleScaleQ12 * (int64_t)g_FixedSinQ28[control->rotationAngle];
    sinTermOrSourceU = (int)((uint64_t)rotationProductA >> 0x20) << 4 | (uint32_t)rotationProductA >> 0x1c;
    rotationProductA = (int64_t)(int)sinTermOrSourceU * 0x1c6e9c;
    rotationProductB = (int64_t)cosTermOrRowStepV * -0x20c8cc;
    stepTermOrRowStartU = (int)((uint64_t)rotationProductB >> 0x20) << 0xb | (uint32_t)rotationProductB >> 0x15;
    rotationProductB = (int64_t)cosTermOrRowStepV * 0x1c6e9c;
    rotationProductC = (int64_t)(int)-sinTermOrSourceU * -0x20c8cc;
    stepTermOrRowStartV = (int)((uint64_t)rotationProductC >> 0x20) << 0xb | (uint32_t)rotationProductC >> 0x15;
    sinTermOrSourceU =
         ((int)((uint64_t)rotationProductB >> 0x20) << 0xc | (uint32_t)rotationProductB >> 0x14) - stepTermOrRowStartV;
    cosTermOrRowStepV = stepTermOrRowStartV * 2;
    rowStepU = (uint64_t)sinTermOrSourceU;
    sourceStartU = (uint64_t)
             (control->sourceOriginYQ12 -
             (sinTermOrSourceU * (((control->base).top + (control->base).bottom >> 1) - clipBottom) +
             (((int)((uint64_t)rotationProductA >> 0x20) << 0xc | (uint32_t)rotationProductA >> 0x14) -
              stepTermOrRowStartU) *
             (((control->base).left + (control->base).right >> 1) - clipRight)));
    /* Per-pixel texture step (MM0 low/high in the original); the decompiler lost both. */
    pixelStepU =
         ((int)((uint64_t)rotationProductA >> 0x20) << 0xc | (uint32_t)rotationProductA >> 0x14) - stepTermOrRowStartU;
    pixelStepV = stepTermOrRowStartU * 2;
    texelIndexOrFraction = control->sourceOriginXQ12 -
             (cosTermOrRowStepV * (((control->base).top + (control->base).bottom >> 1) - clipBottom) +
             stepTermOrRowStartU * 2 * (((control->base).left + (control->base).right >> 1) - clipRight));
    sourceTexture = control->textureSource;
    subresourceTable = (sourceTexture->tableDescriptor).subresourceTableOffset;
    /* the subresource entry fields (paletteIndex, dataOffset, pixelWidth, pixelHeight) are read relative to
       the asset's address anchor */
    if (*(int *)((sourceTexture->common).buildMetadata.assetRelativeAddressAnchor28 + (subresourceTable - 0x20)) < 0) {
      sourceWidth =
           *(int *)((sourceTexture->common).buildMetadata.assetRelativeAddressAnchor28 + (subresourceTable - 0x10));
      sourceHeight =
           *(int *)((sourceTexture->common).buildMetadata.assetRelativeAddressAnchor28 + (subresourceTable - 0xc));
      pixelDataOffset =
           *(int *)((sourceTexture->common).buildMetadata.assetRelativeAddressAnchor28 + (subresourceTable - 0x1c));
      framebufferUnavailable = g_GraphicsFramebufferBeginAccess();
      if (!framebufferUnavailable) {
        rowStepUHigh = (int)(rowStepU >> 0x20);
        sinTermOrSourceU = (uint32_t)sourceStartU;
        sourceColumn = (int)(sourceStartU >> 0x20);
        clipTop = clipHeightOrColumnTerm;
        remainingColumns = clipWidth;
        if (g_FramebufferAccess->bytesPerPixel == SOFTWARE_FRAMEBUFFER_PIXEL_BYTES_16BIT) {
          destPixel = g_FramebufferAccess->pixels +
                    g_FramebufferRowStrideBytes * clipBottom + clipRight * 2;
          sourceV = sourceColumn + texelIndexOrFraction;
          stepTermOrRowStartU = sinTermOrSourceU;
          stepTermOrRowStartV = sourceV;
          destRowStart = destPixel;
          do {
            do {
              sourcePixelSample0 = 0;
              sourcePixelSample1 = 0;
              sourcePixelSample2 = 0;
              sourcePixelSample3 = 0;
              sourceRow = (int)sourceV >> 0xc;
              sourceColumn = (int)sinTermOrSourceU >> 0xc;
              texelIndexOrFraction = sourceWidth * sourceRow + sourceColumn;
              clipHeightOrColumnTerm = sourceColumn + 1;
              if (sourceRow < sourceHeight) {
                if ((-1 < sourceRow) && (sourceColumn < sourceWidth)) {
                  if (-1 < sourceColumn) {
                    sourcePixelSample0 =
                         *(PackedArgb32 *)
                          ((sourceTexture->common).buildMetadata.assetRelativeAddressAnchor28 +
                          texelIndexOrFraction * 4 + pixelDataOffset + -0x28);
                  }
                  if ((-1 < clipHeightOrColumnTerm) && (clipHeightOrColumnTerm < sourceWidth)) {
                    sourcePixelSample1 =
                         *(PackedArgb32 *)
                          ((sourceTexture->common).buildMetadata.assetRelativeAddressAnchor28 +
                          texelIndexOrFraction * 4 + pixelDataOffset + -0x24);
                  }
                }
                if (((-1 < sourceRow + 1) && (sourceRow + 1 < sourceHeight)) && (sourceColumn < sourceWidth)) {
                  if (-1 < sourceColumn) {
                    sourcePixelSample2 =
                         *(PackedArgb32 *)
                          ((sourceTexture->common).buildMetadata.assetRelativeAddressAnchor28 +
                          (texelIndexOrFraction + sourceWidth) * 4 + pixelDataOffset + -0x28);
                  }
                  if ((-1 < clipHeightOrColumnTerm) && (clipHeightOrColumnTerm < sourceWidth)) {
                    sourcePixelSample3 =
                         *(PackedArgb32 *)
                          ((sourceTexture->common).buildMetadata.assetRelativeAddressAnchor28 +
                          (texelIndexOrFraction + sourceWidth) * 4 + pixelDataOffset + -0x24);
                  }
                }
              }
              blendedPixel = UiScaler_BlendBilinear
                                       (sourcePixelSample0,sourcePixelSample1,sourcePixelSample2,
                                        sourcePixelSample3,(int)(sinTermOrSourceU & 0xfff) >> 4,
                                        (int)(sourceV & 0xfff) >> 4);
              /* 32-bit colour to 16-bit: PUNPCKLBW/PSRLW 4, PAND quantize masks, PMADDWD pack weights,
                 then (q >> 40) + (q >> 8) with PADDW; the low word is the pixel. */
              packedLanes = pmaddwd(UiScaler_UnpackBytesToWordLanes(blendedPixel,4) &
                                    THANDOR_BITCAST(SoftwareRgbWordLanes, uint64_t,
                                                    g_SoftwarePixelMmxConstants.quantizeMasksQ12),
                                    g_SoftwarePixelMmxConstants.packWeights);
              *(short *)destPixel = (short)(packedLanes >> 0x28) + (short)(packedLanes >> 8);
              sinTermOrSourceU = sinTermOrSourceU + (int)pixelStepU;
              sourceV = sourceV + pixelStepV;
              destPixel = destPixel + 2;
              remainingColumns = remainingColumns - 1;
            } while (remainingColumns != 0);
            sinTermOrSourceU = stepTermOrRowStartU + (int)rowStepU;
            sourceV = stepTermOrRowStartV + rowStepUHigh + cosTermOrRowStepV;
            destPixel = destRowStart + g_FramebufferRowStrideBytes;
            clipTop = clipTop - 1; /* now the remaining row count */
            stepTermOrRowStartU = sinTermOrSourceU;
            stepTermOrRowStartV = sourceV;
            remainingColumns = clipWidth;
            destRowStart = destPixel;
          } while (clipTop != 0);
        }
        else {
          destPixel = g_FramebufferAccess->pixels +
                    g_FramebufferRowStrideBytes * clipBottom + clipRight * 4;
          sourceV = sourceColumn + texelIndexOrFraction;
          stepTermOrRowStartU = sinTermOrSourceU;
          stepTermOrRowStartV = sourceV;
          destRowStart = destPixel;
          do {
            do {
              sourcePixelSample0 = 0;
              sourcePixelSample1 = 0;
              sourcePixelSample2 = 0;
              sourcePixelSample3 = 0;
              sourceRow = (int)sourceV >> 0xc;
              sourceColumn = (int)sinTermOrSourceU >> 0xc;
              texelIndexOrFraction = sourceWidth * sourceRow + sourceColumn;
              clipHeightOrColumnTerm = sourceColumn + 1;
              if (sourceRow < sourceHeight) {
                if ((-1 < sourceRow) && (sourceColumn < sourceWidth)) {
                  if (-1 < sourceColumn) {
                    sourcePixelSample0 =
                         *(PackedArgb32 *)
                          ((sourceTexture->common).buildMetadata.assetRelativeAddressAnchor28 +
                          texelIndexOrFraction * 4 + pixelDataOffset + -0x28);
                  }
                  if ((-1 < clipHeightOrColumnTerm) && (clipHeightOrColumnTerm < sourceWidth)) {
                    sourcePixelSample1 =
                         *(PackedArgb32 *)
                          ((sourceTexture->common).buildMetadata.assetRelativeAddressAnchor28 +
                          texelIndexOrFraction * 4 + pixelDataOffset + -0x24);
                  }
                }
                if (((-1 < sourceRow + 1) && (sourceRow + 1 < sourceHeight)) && (sourceColumn < sourceWidth)) {
                  if (-1 < sourceColumn) {
                    sourcePixelSample2 =
                         *(PackedArgb32 *)
                          ((sourceTexture->common).buildMetadata.assetRelativeAddressAnchor28 +
                          (texelIndexOrFraction + sourceWidth) * 4 + pixelDataOffset + -0x28);
                  }
                  if ((-1 < clipHeightOrColumnTerm) && (clipHeightOrColumnTerm < sourceWidth)) {
                    sourcePixelSample3 =
                         *(PackedArgb32 *)
                          ((sourceTexture->common).buildMetadata.assetRelativeAddressAnchor28 +
                          (texelIndexOrFraction + sourceWidth) * 4 + pixelDataOffset + -0x24);
                  }
                }
              }
              *(PackedArgb32 *)destPixel =
                   UiScaler_BlendBilinear
                             (sourcePixelSample0,sourcePixelSample1,sourcePixelSample2,sourcePixelSample3,
                              (int)(sinTermOrSourceU & 0xfff) >> 4,(int)(sourceV & 0xfff) >> 4);
              sinTermOrSourceU = sinTermOrSourceU + (int)pixelStepU;
              sourceV = sourceV + pixelStepV;
              destPixel = destPixel + 4;
              remainingColumns = remainingColumns - 1;
            } while (remainingColumns != 0);
            sinTermOrSourceU = stepTermOrRowStartU + (int)rowStepU;
            sourceV = stepTermOrRowStartV + rowStepUHigh + cosTermOrRowStepV;
            destPixel = destRowStart + g_FramebufferRowStrideBytes;
            clipTop = clipTop - 1; /* now the remaining row count */
            stepTermOrRowStartU = sinTermOrSourceU;
            stepTermOrRowStartV = sourceV;
            remainingColumns = clipWidth;
            destRowStart = destPixel;
          } while (clipTop != 0);
        }
        g_GraphicsFramebufferEndAccess();
      }
    }
  }
  return;
}


/* Address: 0x005161A0.
   nonRightPress slot of g_UiSelectionGeometryControlVtable. Maps the clicked screen point back into
   texture space with the same rotation/scale as UiSelectionGeometryControl_DrawClipped, stores it in
   selectedSourceYQ12/XQ12 and queues actionId so the handler can read the picked source position.
*/
void UiSelectionGeometryControl_ConvertPointerAndEnqueueAction
          (UiPointerWheelDelta wheelDelta,UiPixelCoordinate pointerY,UiPixelCoordinate pointerX,
          UiSelectionGeometryControl *control)

{
  int cosTermOrBoundsLeft;
  int boundsRight;
  int boundsTop;
  int boundsBottom;
  int64_t rotationProductA;
  int64_t rotationProductB;
  int64_t rotationProductC;
  uint32_t sinTermOrStepY;
  uint32_t stepTermX;
  
  rotationProductA = (int64_t)control->sampleScaleQ12 * (int64_t)g_FixedCosQ28[control->rotationAngle];
  cosTermOrBoundsLeft = -((int)((uint64_t)rotationProductA >> 0x20) << 4 | (uint32_t)rotationProductA >> 0x1c);
  rotationProductA = (int64_t)control->sampleScaleQ12 * (int64_t)g_FixedSinQ28[control->rotationAngle];
  sinTermOrStepY = (int)((uint64_t)rotationProductA >> 0x20) << 4 | (uint32_t)rotationProductA >> 0x1c;
  rotationProductA = (int64_t)(int)sinTermOrStepY * 0x1c6e9c;
  rotationProductB = (int64_t)cosTermOrBoundsLeft * -0x20c8cc;
  stepTermX = (int)((uint64_t)rotationProductB >> 0x20) << 0xb | (uint32_t)rotationProductB >> 0x15;
  rotationProductB = (int64_t)cosTermOrBoundsLeft * 0x1c6e9c;
  rotationProductC = (int64_t)(int)-sinTermOrStepY * -0x20c8cc;
  sinTermOrStepY = (int)((uint64_t)rotationProductC >> 0x20) << 0xb | (uint32_t)rotationProductC >> 0x15;
  cosTermOrBoundsLeft = (control->base).left;
  boundsRight = (control->base).right;
  boundsTop = (control->base).top;
  boundsBottom = (control->base).bottom;
  control->selectedSourceYQ12 =
       control->sourceOriginYQ12 -
       ((((int)((uint64_t)rotationProductB >> 0x20) << 0xc | (uint32_t)rotationProductB >> 0x14) - sinTermOrStepY) *
        (((control->base).top + (control->base).bottom >> 1) - pointerY) +
       (((int)((uint64_t)rotationProductA >> 0x20) << 0xc | (uint32_t)rotationProductA >> 0x14) - stepTermX) *
       (((control->base).left + (control->base).right >> 1) - pointerX));
  control->selectedSourceXQ12 =
       (control->sourceOriginXQ12 - stepTermX * 2 * ((cosTermOrBoundsLeft + boundsRight >> 1) - pointerX)) -
       sinTermOrStepY * 2 * ((boundsTop + boundsBottom >> 1) - pointerY);
  UiActionQueue_Enqueue(control->actionId,control);
  return;
}


/* Address: 0x004AFA60.
   Left button press: the node under the pointer (a hovered image control on an opaque pixel, else the hit
   test of the topmost root containing the pointer; pressing into a lower root brings it to the front
   first) captures the pointer for the left button, takes the keyboard focus when it is a focus target and
   gets nonRightPress followed by nonRightDrag. Ignored while any button holds a capture.
*/
void UiPointer_DispatchLeftPress(GraphicsCursorButtonState buttonMask,UiPointerWheelDelta wheelDelta,
          UiPixelCoordinate pointerY,UiPixelCoordinate pointerX)

{
  UiSelectableStateFlags *stateFlagsField;
  UiNodeFlags *nodeFlagsField;
  UiRootCallbacks **callbacksField;
  UiNodeVtable *nodeVtable;
  UiRootNode *topRoot;
  UiNodeBase *opaqueHit;
  UiImageControl *node;
  UiRootNode *root;
  bool handled;

  node = g_UiImageControlHoverTarget;
  topRoot = g_UiRootNode;
  if (g_UiPointerCaptureButton != UI_POINTER_CAPTURE_NONE) {
    return;
  }
  /* A hovered image control keeps the press when the pointer is on one of its opaque pixels. */
  opaqueHit = UI_NODE_NONE;
  if (g_UiImageControlHoverTarget != NULL) {
    opaqueHit = UiImageControl_HitTestOpaque(pointerY,pointerX,g_UiImageControlHoverTarget);
    g_UiImageControlHoverTarget = NULL;
    if (opaqueHit == UI_NODE_NONE) {
      stateFlagsField = &(node->selectable).stateFlags;
      *stateFlagsField = *stateFlagsField & ~UI_IMAGE_CONTROL_HOVER_STATE_BITS;
    }
  }
  if (opaqueHit == UI_NODE_NONE) {
    /* Otherwise hit-test the root stack from the top; a root's method08 may end the search. */
    root = topRoot;
    while( true ) {
      if (root == UI_ROOT_STACK_END) {
        return;
      }
      if (((((root->rootFlags & UI_ROOT_DISABLE_POINTER_HIT_TEST) == 0) &&
           ((root->base).left <= pointerX)) && ((root->base).top <= pointerY)) &&
         ((pointerX < (root->base).right && (pointerY < (root->base).bottom)))) break;
      callbacksField = &root->callbacks;
      root = root->previousRoot;
      if (((*callbacksField)->method08 != NULL) &&
         (handled = (*(*callbacksField)->method08)(root), handled)) {
        return;
      }
    }
    node = (UiImageControl *)(*((root->base).vtable)->hitTest)(pointerY,pointerX,&root->base);
    if ((root != topRoot) && (handled = UiRootStack_BringToFront(root), handled)) {
      return;
    }
    if (node == (UiImageControl *)UI_NODE_NONE) {
      return;
    }
  }
  g_UiPointerCaptureButton = UI_POINTER_CAPTURE_LEFT;
  if ((buttonMask & UI_POINTER_BUTTON_REPEAT_CLICK) == CURSOR_BUTTON_NONE) {
    nodeFlagsField = &(node->selectable).base.nodeFlags;
    *nodeFlagsField = *nodeFlagsField & ~UI_NODE_REPEAT_OR_DOUBLE_CLICK;
  }
  else {
    nodeFlagsField = &(node->selectable).base.nodeFlags;
    *nodeFlagsField = *nodeFlagsField | UI_NODE_REPEAT_OR_DOUBLE_CLICK;
  }
  nodeVtable = (node->selectable).base.vtable;
  g_UiPointerCaptureTarget = (UiNodeBase *)node;
  if (((node->selectable).base.nodeFlags &
      (UI_NODE_FALLBACK_FOCUS_TARGET|UI_NODE_PREFERRED_FOCUS_TARGET)) != 0) {
    UiKeyboardFocus_Set((UiNodeBase *)node);
  }
  nodeVtable->nonRightPress(wheelDelta,pointerY,pointerX,(UiNodeBase *)node);
  /* the press handler may have released the capture already */
  if (g_UiPointerCaptureTarget == UI_NODE_NONE) {
    return;
  }
  g_UiPointerCaptureTarget->vtable->nonRightDrag
            (wheelDelta,pointerY,pointerX,g_UiPointerCaptureTarget);
  return;
}


/* Address: 0x004AFBC0.
   Middle button press: like UiPointer_DispatchLeftPress (same node selection, focus and nonRightPress /
   nonRightDrag), but captures the pointer for the middle button and always marks the node's press as a
   repeated click (UI_NODE_REPEAT_OR_DOUBLE_CLICK), whatever buttonMask says.
*/
void UiPointer_DispatchMiddlePress
          (UiPointerButtonMask buttonMask,UiPointerWheelDelta wheelDelta,UiPixelCoordinate pointerY,
          UiPixelCoordinate pointerX)

{
  UiSelectableStateFlags *stateFlagsField;
  UiNodeFlags *nodeFlagsField;
  UiRootCallbacks **callbacksField;
  UiNodeVtable *nodeVtable;
  UiRootNode *topRoot;
  UiNodeBase *opaqueHit;
  UiImageControl *node;
  UiRootNode *root;
  bool handled;
  
  node = g_UiImageControlHoverTarget;
  topRoot = g_UiRootNode;
  if (g_UiPointerCaptureButton != UI_POINTER_CAPTURE_NONE) {
    return;
  }
  /* A hovered image control keeps the press when the pointer is on one of its opaque pixels. */
  opaqueHit = UI_NODE_NONE;
  if (g_UiImageControlHoverTarget != NULL) {
    opaqueHit = UiImageControl_HitTestOpaque(pointerY,pointerX,g_UiImageControlHoverTarget);
    g_UiImageControlHoverTarget = NULL;
    if (opaqueHit == UI_NODE_NONE) {
      stateFlagsField = &(node->selectable).stateFlags;
      *stateFlagsField = *stateFlagsField & ~UI_IMAGE_CONTROL_HOVER_STATE_BITS;
    }
  }
  if (opaqueHit == UI_NODE_NONE) {
    /* Otherwise hit-test the root stack from the top; a root's method08 may end the search. */
    root = topRoot;
    while( true ) {
      if (root == UI_ROOT_STACK_END) {
        return;
      }
      if (((((root->rootFlags & UI_ROOT_DISABLE_POINTER_HIT_TEST) == 0) &&
           ((root->base).left <= pointerX)) && ((root->base).top <= pointerY)) &&
         ((pointerX < (root->base).right && (pointerY < (root->base).bottom)))) break;
      callbacksField = &root->callbacks;
      root = root->previousRoot;
      if (((*callbacksField)->method08 != NULL) &&
         (handled = (*(*callbacksField)->method08)(root), handled)) {
        return;
      }
    }
    node = (UiImageControl *)(*((root->base).vtable)->hitTest)(pointerY,pointerX,&root->base);
    if ((root != topRoot) && (handled = UiRootStack_BringToFront(root), handled)) {
      return;
    }
    if (node == (UiImageControl *)UI_NODE_NONE) {
      return;
    }
  }
  g_UiPointerCaptureButton = UI_POINTER_CAPTURE_MIDDLE;
  nodeFlagsField = &(node->selectable).base.nodeFlags;
  *nodeFlagsField = *nodeFlagsField | UI_NODE_REPEAT_OR_DOUBLE_CLICK;
  nodeVtable = (node->selectable).base.vtable;
  g_UiPointerCaptureTarget = (UiNodeBase *)node;
  if (((node->selectable).base.nodeFlags &
      (UI_NODE_FALLBACK_FOCUS_TARGET|UI_NODE_PREFERRED_FOCUS_TARGET)) != 0) {
    UiKeyboardFocus_Set((UiNodeBase *)node);
  }
  nodeVtable->nonRightPress(wheelDelta,pointerY,pointerX,(UiNodeBase *)node);
  /* the press handler may have released the capture already */
  if (g_UiPointerCaptureTarget == UI_NODE_NONE) {
    return;
  }
  g_UiPointerCaptureTarget->vtable->nonRightDrag
            (wheelDelta,pointerY,pointerX,g_UiPointerCaptureTarget);
  return;
}


/* Address: 0x004AFD10.
   Right button press: the hit test of the topmost root containing the pointer (a hovered image control
   only loses its hover state, it gets no opaque-pixel check) picks the node, which captures the pointer for
   the right button, takes the keyboard focus when it is a focus target and gets rightPress followed by
   rightDrag. Ignored while any button holds a capture.
*/
void UiPointer_DispatchRightPress
          (UiPointerButtonMask buttonMask,UiPointerWheelDelta wheelDelta,UiPixelCoordinate pointerY,
          UiPixelCoordinate pointerX)

{
  UiSelectableStateFlags *stateFlagsField;
  UiRootCallbacks **callbacksField;
  UiNodeVtable *nodeVtable;
  UiRootNode *topRoot;
  UiNodeBase *node;
  UiRootNode *root;
  bool handled;
  
  topRoot = g_UiRootNode;
  if (g_UiPointerCaptureButton == UI_POINTER_CAPTURE_NONE) {
    root = topRoot;
    if (g_UiImageControlHoverTarget != NULL) {
      stateFlagsField = &(g_UiImageControlHoverTarget->selectable).stateFlags;
      *stateFlagsField = *stateFlagsField & ~UI_IMAGE_CONTROL_HOVER_STATE_BITS;
    }
    while (root != UI_ROOT_STACK_END) {
      if (((((root->rootFlags & UI_ROOT_DISABLE_POINTER_HIT_TEST) == 0) &&
           ((root->base).left <= pointerX)) && ((root->base).top <= pointerY)) &&
         ((pointerX < (root->base).right && (pointerY < (root->base).bottom)))) {
        node = (*((root->base).vtable)->hitTest)(pointerY,pointerX,&root->base);
        if ((root != topRoot) && (handled = UiRootStack_BringToFront(root), handled)) {
          return;
        }
        if (node == UI_NODE_NONE) {
          return;
        }
        g_UiPointerCaptureButton = UI_POINTER_CAPTURE_RIGHT;
        if ((buttonMask & UI_POINTER_BUTTON_REPEAT_CLICK) == 0) {
          node->nodeFlags = node->nodeFlags & ~UI_NODE_REPEAT_OR_DOUBLE_CLICK;
        }
        else {
          node->nodeFlags = node->nodeFlags | UI_NODE_REPEAT_OR_DOUBLE_CLICK;
        }
        nodeVtable = node->vtable;
        g_UiPointerCaptureTarget = node;
        if ((node->nodeFlags & (UI_NODE_FALLBACK_FOCUS_TARGET|UI_NODE_PREFERRED_FOCUS_TARGET)) != 0) {
          UiKeyboardFocus_Set(node);
        }
        nodeVtable->rightPress(wheelDelta,pointerY,pointerX,node);
        /* the press handler may have released the capture already */
        if (g_UiPointerCaptureTarget == UI_NODE_NONE) {
          return;
        }
        g_UiPointerCaptureTarget->vtable->rightDrag
                  (wheelDelta,pointerY,pointerX,g_UiPointerCaptureTarget);
        return;
      }
      callbacksField = &root->callbacks;
      root = root->previousRoot;
      if (((*callbacksField)->method08 != NULL) &&
         (handled = (*(*callbacksField)->method08)(root), handled)) {
        return;
      }
    }
  }
  return;
}


/* Address: 0x004AFFA0.
   Moves the keyboard focus to the next focus target after the current one in depth-first tree order,
   wrapping around through the topmost ancestor and skipping suppressed nodes. Nothing changes when there is
   no focus or no other focus target.
*/
void UiKeyboardFocus_MoveNext(void)

{
  UiNodeBase *node;
  UiNodeBase *nextNode;

  node = g_UiKeyboardFocusNode;
  if (g_UiKeyboardFocusNode == UI_NODE_NONE) {
    return;
  }
  /* Pre-order walk from the focus node, wrapping around through the topmost ancestor. */
  while( true ) {
    nextNode = node->firstChild;
    if (nextNode == UI_NODE_NONE) {
      while ((nextNode = node->nextSibling, nextNode == UI_NODE_NONE) &&
             (node->parent != UI_NODE_NONE)) {
        node = node->parent;
      }
      if (nextNode == UI_NODE_NONE) {
        nextNode = node;
      }
    }
    node = nextNode;
    if ((node->nodeFlags & (UI_NODE_FALLBACK_FOCUS_TARGET|UI_NODE_PREFERRED_FOCUS_TARGET)) == 0) continue;
    if (node == g_UiKeyboardFocusNode) {
      return;
    }
    if ((node->nodeFlags & UI_NODE_SUPPRESSED) == 0) break;
  }
  UiKeyboardFocus_Set(node);
  return;
}


/* Address: 0x004AFE40.
   Pointer motion (and wheel): drops a hovered in-game selection record (rebuilding the detail panel) and
   updates the tooltip target. While a node holds the pointer capture it gets the drag for its button;
   otherwise the node under the pointer in the topmost root containing it gets pointerMove and, for a
   non-zero wheel delta, pointerWheel. A root the pointer misses passes it on to the root below only when
   its pointerMissPolicy returns a negative value.
*/
void UiPointer_DispatchMotionAndWheel
          (UiPointerWheelDelta wheelDelta,UiPixelCoordinate pointerY,UiPixelCoordinate pointerX)

{
  UiNodeVtable *nodeVtable;
  UiRootPointerMissPolicyCallback *missPolicy;
  UiNodeBase *targetNode;
  int missPolicyResult;
  UiRootNode *root;
  
  targetNode = g_UiPointerCaptureTarget;
  if (g_UiHoverSelectionRecord != NULL) {
    g_UiHoverSelectionRecord = NULL;
    InGameSelectionDetailPanel_Rebuild();
  }
  UiTooltip_UpdateHoverTarget(pointerY,pointerX);
  root = g_UiRootNode;
  if (targetNode != UI_NODE_NONE) {
    if (g_UiPointerCaptureButton == UI_POINTER_CAPTURE_RIGHT) {
      targetNode->vtable->rightDrag(wheelDelta,pointerY,pointerX,targetNode);
    }
    else {
      targetNode->vtable->nonRightDrag(wheelDelta,pointerY,pointerX,targetNode);
    }
    return;
  }
  while( true ) {
    if (root == UI_ROOT_STACK_END) {
      return;
    }
    if (((((root->base).left <= pointerX) && ((root->base).top <= pointerY)) &&
        (pointerX < (root->base).right)) && (pointerY < (root->base).bottom)) {
      targetNode = (*((root->base).vtable)->hitTest)(pointerY,pointerX,&root->base);
      if (targetNode == UI_NODE_NONE) {
        return;
      }
      nodeVtable = targetNode->vtable;
      nodeVtable->pointerMove(pointerY,pointerX,targetNode);
      if (wheelDelta == 0) {
        return;
      }
      nodeVtable->pointerWheel(wheelDelta,pointerY,pointerX,targetNode);
      return;
    }
    missPolicy = root->callbacks->pointerMissPolicy;
    if (missPolicy == NULL) {
      return;
    }
    missPolicyResult = missPolicy(root);
    if (-1 < missPolicyResult) break;
    root = root->previousRoot;
  }
  return;
}


/* Address: 0x004B09F0.
   Default pointerWheel slot of most UI vtables: passes the wheel event up to the parent node (if any), so
   it reaches the nearest ancestor that handles the wheel.
*/
void UiNode_ForwardPointerWheelToParent
          (UiPointerWheelDelta wheelDelta,UiPixelCoordinate pointerY,UiPixelCoordinate pointerX,
          UiNodeBase *control)

{
  UiNodeBase *parentControl;
  
  parentControl = control->parent;
  if (parentControl != UI_NODE_NONE) {
    parentControl->vtable->pointerWheel(wheelDelta,pointerY,pointerX,parentControl);
  }
  return;
}


/* Address: 0x004B08C0.
   Default keyboardEvent slot of many UI vtables, also the fallback of the slider and focus-proxy handlers.
   It always returns CF set (key not consumed): the original compares the key with KEYBOARD_KEY_CODE_TAB but
   then sets CF unconditionally (CMP; STC; RET 0xc), so the focus move its name suggests never happens.
*/
bool UiNode_DefaultKeyboardEventMoveFocusNext
          (UiKeyboardStateMask keyboardStateMask,UiKeyboardEventCode keyCode,UiNodeBase *control)

{
  return true;
}


/* Address: 0x004AFF60.
   Moves the keyboard focus to node (UI_NODE_NONE clears it), keeping UI_NODE_HAS_KEYBOARD_FOCUS on the
   focused node only, and redraws every root (also when the focus did not change).
*/
void UiKeyboardFocus_Set(UiNodeBase *node)

{
  if (g_UiKeyboardFocusNode != node) {
    if (g_UiKeyboardFocusNode != UI_NODE_NONE) {
      g_UiKeyboardFocusNode->nodeFlags =
           g_UiKeyboardFocusNode->nodeFlags & ~UI_NODE_HAS_KEYBOARD_FOCUS;
    }
    g_UiKeyboardFocusNode = node;
    if (node != UI_NODE_NONE) {
      node->nodeFlags = node->nodeFlags | UI_NODE_HAS_KEYBOARD_FOCUS;
    }
  }
  UiRootStack_InvalidateAll();
  return;
}


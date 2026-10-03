/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/src/ui/controls/input.c
 * Reverse engineering by idkFoxes 2026
 */

#include <thandor/ui/controls/input.h>
#include <thandor/thandor.h>
#include <thandor/platform/bootstrap/image.h>

/* Module data. */

UiNodeBase *g_UiPointerCaptureTarget = UI_NODE_NONE;

UiNodeBase *g_UiKeyboardFocusNode = UI_NODE_NONE;

UiPointerCaptureButton g_UiPointerCaptureButton = 255;

UiImageControl * g_UiImageControlHoverTarget = 0;

/* int32_t, 1: multiplier of wheelDelta * stepValue when the mouse wheel moves a range slider (src/ui/controls/input.c). */
static const int32_t g_UiRangeSliderDragScale = 1;

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

/* A release of captureButton ends the pointer capture of control (the capture target read before the event):
   the node gets the release (rightRelease for the right button, nonRightRelease otherwise), loses the capture
   and the pointer position is dispatched again as motion. Returns false, doing nothing, when control does not
   hold a capture with that button. */
static Bool8 UiPointer_ReleaseCapture
          (UiNodeBase *control,UiPointerCaptureButton captureButton,UiPointerWheelDelta wheelDelta,
          UiPixelCoordinate pointerY,UiPixelCoordinate pointerX)
{
  if ((control == UI_NODE_NONE) || (g_UiPointerCaptureButton != captureButton)) {
    return false;
  }
  if (captureButton == UI_POINTER_CAPTURE_RIGHT) {
    control->vtable->rightRelease(wheelDelta,pointerY,pointerX,control);
  }
  else {
    control->vtable->nonRightRelease(wheelDelta,pointerY,pointerX,control);
  }
  g_UiPointerCaptureButton = UI_POINTER_CAPTURE_NONE;
  g_UiPointerCaptureTarget = UI_NODE_NONE;
  UiPointer_DispatchMotionAndWheel(wheelDelta,pointerY,pointerX);
  return true;
}

/* Delivers the queued mouse events to the UI under the frame lock (polling DirectInput first when it is
   the active mouse). Presses and motion go to the node under the pointer; a release goes to the node
   that captured the pointer with that button, which then loses the capture and the pointer position is
   dispatched again as motion. A release without a matching capture is dropped.
   Original quirk: a captured middle-button release also steps the primary random stream (a call of
   Random_NextPrimary after the motion dispatch; the left/right release paths do not), so the primary seed
   depends on local mouse input. The session simulation draws through g_RandomGeneratorState.next, which is
   Random_NextSecondary during a network session, so this does not desync the game state; only the local
   users of the primary stream (ambient sound/music choice, button animation phases, the field's cell
   animation phases and material variants at level load, sequence tokens) can differ between machines.
*/
void UiPointer_DispatchPendingEvents(void)

{
  UiNodeBase *control;
  char eventKind;
  UiPixelCoordinate pointerX;
  UiPixelCoordinate pointerY;
  GraphicsCursorButtonState buttonMask;
  UiPointerWheelDelta wheelDelta;
  CursorPointerEvent pointerEvent;

  g_SpinLockAcquire(g_UiRuntimeFrameLock);
  if (g_PointerSetPosition == DirectInputMouse_SetPosition) {
    DirectInputMouse_PollBufferedEvents();
  }
  while (g_GraphicsCursorConsumeEvent(&pointerEvent)) {
    control = g_UiPointerCaptureTarget; /* the consume call does not touch the capture */
    pointerX = pointerEvent.pointerX;
    wheelDelta = pointerEvent.wheelDelta;
    pointerY = pointerEvent.pointerY;
    buttonMask = pointerEvent.buttonState;
    eventKind = (char)pointerEvent.eventType; /* GraphicsCursorEventType */
    switch (eventKind) {
    case MOTION_OR_WHEEL:
      UiPointer_DispatchMotionAndWheel(wheelDelta,pointerY,pointerX);
      break;
    case LEFT_PRESS:
      UiPointer_DispatchLeftPress(buttonMask,wheelDelta,pointerY,pointerX);
      break;
    case MIDDLE_PRESS:
      UiPointer_DispatchMiddlePress(buttonMask,wheelDelta,pointerY,pointerX);
      break;
    case RIGHT_PRESS:
      UiPointer_DispatchRightPress(buttonMask,wheelDelta,pointerY,pointerX);
      break;
    case 4: /* unused code, handled like a left release */
    case LEFT_RELEASE:
      UiPointer_ReleaseCapture(control,UI_POINTER_CAPTURE_LEFT,wheelDelta,pointerY,pointerX);
      break;
    case MIDDLE_RELEASE:
      if (UiPointer_ReleaseCapture(control,UI_POINTER_CAPTURE_MIDDLE,wheelDelta,pointerY,pointerX)) {
        Random_NextPrimary(); /* original quirk: only this release steps the primary random stream */
      }
      break;
    case RIGHT_RELEASE:
      UiPointer_ReleaseCapture(control,UI_POINTER_CAPTURE_RIGHT,wheelDelta,pointerY,pointerX);
      break;
    default:
      /* codes the queue never holds: the original compare chain treats those below 0 as motion and those
         above RIGHT_RELEASE as a right release */
      if (eventKind < MOTION_OR_WHEEL) {
        UiPointer_DispatchMotionAndWheel(wheelDelta,pointerY,pointerX);
      }
      else {
        UiPointer_ReleaseCapture(control,UI_POINTER_CAPTURE_RIGHT,wheelDelta,pointerY,pointerX);
      }
      break;
    }
  }
  g_SpinLockReleaseAndInvoke
            ((SpinLockReleaseCallbackProc *)g_UiRuntimePostUnlockCallback,g_UiRuntimeFrameLock);
  return;
}


/* Takes the keyboard focus away from node (e.g. before it is hidden or removed): the focus moves on to the
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


/* The node after node in pre-order (first child, else the next sibling of node or of its nearest ancestor
   that has one), following only links UiKeyboard_CheckedLink accepts. At the end of the tree it returns the
   topmost ancestor (the walk wraps around) and sets *wrapped. */
static UiNodeBase *UiKeyboard_NextInPreOrder(UiNodeBase *node,Bool8 *wrapped)
{
  UiNodeBase *nextNode;
  UiNodeBase *parent;

  nextNode = UiKeyboard_CheckedLink(node,"firstChild",node->firstChild);
  if (nextNode != UI_NODE_NONE) {
    return nextNode;
  }
  nextNode = UiKeyboard_CheckedLink(node,"nextSibling",node->nextSibling);
  while (nextNode == UI_NODE_NONE) {
    parent = UiKeyboard_CheckedLink(node,"parent",node->parent);
    if (parent == UI_NODE_NONE) {
      *wrapped = true;
      return node;
    }
    node = parent;
    nextNode = UiKeyboard_CheckedLink(node,"nextSibling",node->nextSibling);
  }
  return nextNode;
}

/* A key the focus node passed on (keyboardEvent returned true): offers it to the following focus targets in
   pre-order, skipping suppressed ones and wrapping around at most once through the topmost ancestor. The
   first one whose keyboardEvent returns false (takes the key) gets the keyboard focus. Returns true when
   nobody took the key (the walk came back to the focus node or would wrap a second time). */
static Bool8 UiKeyboard_PassToFollowingFocusTargets
          (UiNodeBase *control,UiKeyboardStateMask keyboardStateMask,UiKeyboardEventCode keyCode)
{
  Bool8 wrappedOnce;
  Bool8 wrapped;
  Bool8 passToNext;

  wrappedOnce = false;
  passToNext = true;
  while (passToNext) {
    wrapped = false;
    control = UiKeyboard_NextInPreOrder(control,&wrapped);
    if (wrapped) {
      if (wrappedOnce) {
        return true;
      }
      wrappedOnce = true;
    }
    if ((control->nodeFlags & (UI_NODE_FALLBACK_FOCUS_TARGET|UI_NODE_PREFERRED_FOCUS_TARGET)) == 0) continue;
    if (control == g_UiKeyboardFocusNode) {
      return true;
    }
    if ((control->nodeFlags & UI_NODE_SUPPRESSED) != 0) continue;
    passToNext = control->vtable->keyboardEvent(keyboardStateMask,keyCode,control);
  }
  UiKeyboardFocus_Set(control);
  return false;
}

/* Delivers the queued key events to the UI under the frame lock. A key goes to the focused node; if it
   passes the key on (keyboardEvent returns true), the next focus targets in tree order
   get it and the first one that takes it receives the focus. Keys nobody takes, or pressed with no focus, go
   to the top root's keyboard fallback. While a node has captured the pointer (a mouse button is held), keys
   are discarded.
*/
void UiKeyboard_DispatchPendingEvents(void)

{
  Bool8 dispatchToRoot;
  UiKeyboardEventCode keyCode;
  UiKeyboardStateMask keyboardStateMask;
  UiNodeBase *control;

  g_SpinLockAcquire(g_UiRuntimeFrameLock);
  while (g_KeyboardReadEvent(&keyCode,&keyboardStateMask)) {
    if (g_UiPointerCaptureTarget != UI_NODE_NONE) continue;
    control = g_UiKeyboardFocusNode;
    if (control == UI_NODE_NONE) {
      dispatchToRoot = true;
    }
    else if (control->vtable->keyboardEvent(keyboardStateMask,keyCode,control)) {
      dispatchToRoot = UiKeyboard_PassToFollowingFocusTargets(control,keyboardStateMask,keyCode);
    }
    else {
      dispatchToRoot = false;
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


/* The node after node in pre-order: its first child, else the next sibling of node or of its nearest ancestor
   that has one; UI_NODE_NONE at the end of the tree (no wrap-around). */
static UiNodeBase *UiKeyboardFocus_NextInPreOrderOrNone(UiNodeBase *node)
{
  UiNodeBase *nextNode;

  nextNode = node->firstChild;
  if (nextNode != UI_NODE_NONE) {
    return nextNode;
  }
  while (node != UI_NODE_NONE) {
    nextNode = node->nextSibling;
    if (nextNode != UI_NODE_NONE) {
      return nextNode;
    }
    node = node->parent;
  }
  return UI_NODE_NONE;
}

/* Gives the keyboard focus to the first node from root on that is a preferred focus target, else to the
   last fallback focus target found (suppressed nodes are skipped); without any the focus stays as it is.
*/
void UiKeyboardFocus_SelectInitial(UiNodeBase *root)

{
  UiNodeBase *fallbackFocusNode;
  UiNodeBase *node;

  /* Pre-order walk starting at root (continuing past its subtree through the parents' siblings):
     the first unsuppressed preferred focus target wins, else the last unsuppressed fallback. */
  fallbackFocusNode = UI_NODE_NONE;
  for (node = root; node != UI_NODE_NONE; node = UiKeyboardFocus_NextInPreOrderOrNone(node)) {
    if ((node->nodeFlags & UI_NODE_SUPPRESSED) != 0) continue;
    if ((node->nodeFlags & UI_NODE_PREFERRED_FOCUS_TARGET) != 0) break;
    if ((node->nodeFlags & UI_NODE_FALLBACK_FOCUS_TARGET) != 0) {
      fallbackFocusNode = node;
    }
  }
  if (node == UI_NODE_NONE) {
    node = fallbackFocusNode;
    if (node == UI_NODE_NONE) {
      return;
    }
  }
  if (node != g_UiKeyboardFocusNode) {
    UiKeyboardFocus_Set(node);
  }
  return;
}


/* Called when a control is unsuppressed or its page becomes active: gives it the keyboard focus if no node
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


/* keyboardEvent slot of g_UiRangeSliderControlVtable. Left/Right (Down/Up for a vertical slider) move the
   value by stepValue, with Ctrl straight to the minimum/maximum; each step plays the click sound, queues
   actionId and redraws. Other keys, and all keys while suppressed, go to the default handler, which passes
   them on. Returns false when the key was consumed.
*/
Bool8 UiRangeSliderControl_HandleKeyboard
          (UiKeyboardStateMask keyboardStateMask,UiKeyboardEventCode keyCode,UiRangeSliderControl *control)

{
  int32_t adjustedSliderValue;
  UiKeyboardEventCode decreaseKey;
  UiKeyboardEventCode increaseKey;

  if ((control->base.nodeFlags & UI_NODE_SUPPRESSED) != 0) {
    return UiNode_DefaultKeyboardEventMoveFocusNext(keyboardStateMask,keyCode,&control->base);
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
    if ((keyboardStateMask & KEYBOARD_STATE_CTRL) != 0) {
      adjustedSliderValue = control->minimumValue;
    }
    else {
      adjustedSliderValue = control->value - control->stepValue;
      if (adjustedSliderValue < control->minimumValue) {
        adjustedSliderValue = control->minimumValue;
      }
    }
  }
  else if (keyCode == increaseKey) {
    if ((keyboardStateMask & KEYBOARD_STATE_CTRL) != 0) {
      adjustedSliderValue = control->maximumValue;
    }
    else {
      adjustedSliderValue = control->value + control->stepValue;
      if (control->maximumValue < adjustedSliderValue) {
        adjustedSliderValue = control->maximumValue;
      }
    }
  }
  else {
    return UiNode_DefaultKeyboardEventMoveFocusNext(keyboardStateMask,keyCode,&control->base);
  }
  control->value = adjustedSliderValue;
  if (((control->sliderFlags & UI_RANGE_SLIDER_CLICK_SOUND) != 0) && (control->clickSound != NULL)) {
    g_SoundPlayOneShot(g_UiSoundGainQ15,g_UiSoundGainQ15,control->clickSound,NULL);
  }
  UiActionQueue_Enqueue(control->actionId,&control->base);
  UiNode_InvalidateRoot(&control->base);
  return false;
}


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
  if (childControl != NULL) {
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


/* Default pointerMove slot of most UI vtables (range sliders, labels, lists, ...): the node asks for
   cursor frame 0 (GRAPHICS_CURSOR_FRAME_ARROW).
*/
GraphicsCursorFrameIndex UiNode_DefaultPointerMove(UiPixelCoordinate pointerY,UiPixelCoordinate pointerX,
                                                   UiNodeBase *control)

{
  return GRAPHICS_CURSOR_FRAME_ARROW;
}


/* nonRightDrag slot of g_UiRangeSliderControlVtable. While the thumb is dragged, maps the pointer position
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
  GraphicsTextureLogicalSize thumbSize;

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


/* pointerWheel slot of g_UiRangeSliderControlVtable. Unless the thumb is being dragged, each wheel notch
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


/* nonRightRelease slot of g_UiFocusProxyControlVtable. Forwards the left release to the focus child,
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


/* rightRelease slot of g_UiFocusProxyControlVtable. Forwards the right release to the focus child,
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


/* nonRightDrag slot of g_UiFocusProxyControlVtable. Forwards the left-button drag to the focus child,
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


/* rightDrag slot of g_UiFocusProxyControlVtable. Forwards the right-button drag to the focus child,
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
    if (((hitNode == &control->base) && (control->focusChild != NULL)) &&
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


/* pointerMove slot of g_UiImageControlVtable. Over an opaque pixel of the image the arrow
   is shown. Over a transparent pixel of a persistent-activation image, a child under the pointer supplies
   the cursor; without one, UI_IMAGE_CONTROL_CURSOR_FRAME_IDLE while no image control is hovered.
*/
GraphicsCursorFrameIndex UiImageControl_PointerMove
          (UiPixelCoordinate pointerY,UiPixelCoordinate pointerX,UiImageControl *control)

{
  UiImageControl *hitControl;
  GraphicsCursorFrameIndex cursorFrame;
  Bool8 overOpaquePixel;

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


/* Weights of the bilinear scaler below, indexed by the 8-bit fraction between two source pixels: the first
   pixel's weight is g_UiScalerFirstPixelWeights, the second's g_UiScalerSecondPixelWeights, all four lanes
   equal. Precomputed tables in the original. */
static SoftwareBgraWordLanes g_UiScalerFirstPixelWeights[256];
static SoftwareBgraWordLanes g_UiScalerSecondPixelWeights[256];

/* Not in the original (it carried the tables precomputed): builds the scaler weights. Fractions 0..63 take
   only the first pixel (0x4000), 64..191 blend in steps t = 2 * (fraction - 64) of 0x4040 / 256 (the pair
   sums to 0x4040 or 0x403F, not 0x4000), 192..255 take only the second pixel (0x4000). This reproduces every
   entry of the original tables. Called once at startup. */
void UiScaler_BuildPixelWeightTables(void)
{
  int fraction;
  int blendStep;
  uint16_t firstWeight;
  uint16_t secondWeight;

  for (fraction = 0; fraction < 256; fraction++) {
    if (fraction < 64) {
      firstWeight = 0x4000;
      secondWeight = 0;
    }
    else if (fraction < 192) {
      blendStep = (fraction - 64) * 2;
      firstWeight = (uint16_t)(((256 - blendStep) * 0x4040) >> 8);
      secondWeight = (uint16_t)((blendStep * 0x4040) >> 8);
    }
    else {
      firstWeight = 0;
      secondWeight = 0x4000;
    }
    g_UiScalerFirstPixelWeights[fraction].blue = firstWeight;
    g_UiScalerFirstPixelWeights[fraction].green = firstWeight;
    g_UiScalerFirstPixelWeights[fraction].red = firstWeight;
    g_UiScalerFirstPixelWeights[fraction].alpha = firstWeight;
    g_UiScalerSecondPixelWeights[fraction].blue = secondWeight;
    g_UiScalerSecondPixelWeights[fraction].green = secondWeight;
    g_UiScalerSecondPixelWeights[fraction].red = secondWeight;
    g_UiScalerSecondPixelWeights[fraction].alpha = secondWeight;
  }
}


/* MMX lane helpers for the bilinear scaler below (lanes are little-endian 16-bit words). */

/* PUNPCKLBW mm,mm then PSRLW mm,shift: byte i of pixel becomes word lane i = (byte * 0x101) >> shift. */
static __inline uint64_t UiScaler_UnpackBytesToWordLanes(uint32_t pixel,int shift) {
  uint64_t lanes;
  int lane;

  lanes = 0;
  for (lane = 0; lane < 4; lane++) {
    lanes = lanes |
            (uint64_t)(uint16_t)((uint16_t)(((pixel >> (lane * 8)) & 0xff) * COLOR_CHANNEL_TO_WORD_LANE) >> shift) << (lane * 16);
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


/* One screen pixel of UiSelectionGeometryControl_DrawClipped: the 2x2 texels around the Q12 source position
   (sourceU, sourceV) of a sourceWidth x sourceHeight 32-bit texture (pixel data at pixelDataOffset from the
   asset; texels outside it count as 0), bilinearly blended by the position's fractions. */
static PackedArgb32 UiSelectionGeometryControl_SampleBilinear
          (GraphicsTextureSourceAsset *sourceTexture,int pixelDataOffset,int sourceWidth,int sourceHeight,
          uint32_t sourceU,uint32_t sourceV)
{
  int sourceRow;
  int sourceColumn;
  int nextColumn;
  int texelIndex;
  PackedArgb32 sourcePixelSample0; /* texel (column, row) */
  PackedArgb32 sourcePixelSample1; /* texel (column + 1, row) */
  PackedArgb32 sourcePixelSample2; /* texel (column, row + 1) */
  PackedArgb32 sourcePixelSample3; /* texel (column + 1, row + 1) */

  sourcePixelSample0 = 0;
  sourcePixelSample1 = 0;
  sourcePixelSample2 = 0;
  sourcePixelSample3 = 0;
  sourceRow = (int)sourceV >> Q12_SHIFT;
  sourceColumn = (int)sourceU >> Q12_SHIFT;
  texelIndex = sourceWidth * sourceRow + sourceColumn;
  nextColumn = sourceColumn + 1;
  if (sourceRow < sourceHeight) {
    if ((-1 < sourceRow) && (sourceColumn < sourceWidth)) {
      if (-1 < sourceColumn) {
        sourcePixelSample0 =
             *(PackedArgb32 *)((uint8_t *)sourceTexture + texelIndex * 4 + pixelDataOffset);
      }
      if ((-1 < nextColumn) && (nextColumn < sourceWidth)) {
        sourcePixelSample1 =
             *(PackedArgb32 *)((uint8_t *)sourceTexture + texelIndex * 4 + pixelDataOffset + 4);
      }
    }
    if (((-1 < sourceRow + 1) && (sourceRow + 1 < sourceHeight)) && (sourceColumn < sourceWidth)) {
      if (-1 < sourceColumn) {
        sourcePixelSample2 =
             *(PackedArgb32 *)((uint8_t *)sourceTexture + (texelIndex + sourceWidth) * 4 + pixelDataOffset);
      }
      if ((-1 < nextColumn) && (nextColumn < sourceWidth)) {
        sourcePixelSample3 =
             *(PackedArgb32 *)((uint8_t *)sourceTexture + (texelIndex + sourceWidth) * 4 + pixelDataOffset + 4);
      }
    }
  }
  return UiScaler_BlendBilinear(sourcePixelSample0,sourcePixelSample1,sourcePixelSample2,sourcePixelSample3,
                                (int)(sourceU & Q12_FRACTION_MASK) >> 4,(int)(sourceV & Q12_FRACTION_MASK) >> 4);
}

/* drawClipped slot of g_UiSelectionGeometryControlVtable. Fills the node (clipped) with its texture,
   rotated by rotationAngle and scaled by sampleScaleQ12 about sourceOrigin: every screen pixel is mapped
   back to a Q12 source position and bilinearly filtered from the 2x2 texels around it (texels outside the
   texture count as 0), for 16- and 32-bit framebuffers. Only direct-colour subresources (negative
   paletteIndex) are drawn.
*/
void UiSelectionGeometryControl_DrawClipped
          (int clipBottom,int clipRight,int clipTop,int clipLeft,UiSelectionGeometryControl *control)

{
  GraphicsTextureSourceAsset *sourceTexture;
  AssetRelativeOffset subresourceTable;
  int sourceWidth;
  int sourceHeight;
  int pixelDataOffset;
  int64_t rotationProductA;
  int64_t rotationProductB;
  int64_t rotationProductC;
  int cosTerm;
  uint32_t sinTerm;
  uint32_t stepTermU;
  uint32_t stepTermV;
  int clipWidth;
  int clipHeight;
  uint32_t pixelStepU;
  uint32_t pixelStepV;
  uint32_t rowStepU;
  uint64_t rowStepUWide; /* rowStepU zero-extended, so its high half is always 0 */
  int rowStepUHigh;
  int rowStepV;
  uint64_t sourceStartU; /* a zero-extended 32-bit value, so its high half is always 0 */
  int sourceStartUHigh;
  int sourceStartV;
  uint32_t sourceU;
  uint32_t sourceV;
  uint32_t rowStartU;
  uint32_t rowStartV;
  uint8_t *destPixel;
  uint8_t *destRowStart;
  int remainingColumns;
  int remainingRows;
  Bool8 framebufferUnavailable;
  PackedArgb32 blendedPixel;
  uint64_t packedLanes;
  uint64_t quantizeMaskLanes; /* the four 16-bit quantize masks as one 64-bit lane vector */

  /* intersect the clip rectangle with the node */
  if (clipLeft < (control->base).left) {
    clipLeft = (control->base).left;
  }
  if (clipTop < (control->base).top) {
    clipTop = (control->base).top;
  }
  if ((control->base).right < clipRight) {
    clipRight = (control->base).right;
  }
  if ((control->base).bottom < clipBottom) {
    clipBottom = (control->base).bottom;
  }
  clipWidth = clipRight - clipLeft;
  clipHeight = clipBottom - clipTop;
  if ((clipWidth == 0) || (clipRight < clipLeft) || (clipHeight == 0) || (clipBottom < clipTop) ||
      (control->textureSource == NULL)) {
    return;
  }
  rotationProductA = (int64_t)control->sampleScaleQ12 * (int64_t)g_FixedSineQ28[FIXED_SINE_TABLE_COS + control->rotationAngle];
  cosTerm = -(FIXED_PRODUCT_SHR(rotationProductA, Q28_SHIFT));
  rotationProductA = (int64_t)control->sampleScaleQ12 * (int64_t)g_FixedSineQ28[FIXED_SINE_TABLE_SIN + control->rotationAngle];
  sinTerm = FIXED_PRODUCT_SHR(rotationProductA, Q28_SHIFT);
  /* scale*(sin, cos) mapped through the field-grid lattice factors: products by the column factor are taken
     >> Q20_SHIFT, those by the row factor >> (Q20_SHIFT + 1) (half a row, the lattice skew) */
  rotationProductA = (int64_t)(int)sinTerm * FIELD_GRID_WORLD_X_TO_COLUMN_Q20;
  rotationProductB = (int64_t)cosTerm * FIELD_GRID_WORLD_Y_TO_ROW_Q20;
  stepTermU = FIXED_PRODUCT_SHR(rotationProductB, Q20_SHIFT + 1);
  rotationProductB = (int64_t)cosTerm * FIELD_GRID_WORLD_X_TO_COLUMN_Q20;
  rotationProductC = (int64_t)(int)-sinTerm * FIELD_GRID_WORLD_Y_TO_ROW_Q20;
  stepTermV = FIXED_PRODUCT_SHR(rotationProductC, Q20_SHIFT + 1);
  /* per-row texture step */
  rowStepU = (FIXED_PRODUCT_SHR(rotationProductB, Q20_SHIFT)) - stepTermV;
  rowStepV = stepTermV * 2;
  rowStepUWide = (uint64_t)rowStepU;
  sourceStartU = (uint64_t)
           (control->sourceOriginXQ12 -
           (rowStepU * (((control->base).top + (control->base).bottom >> 1) - clipTop) +
           ((FIXED_PRODUCT_SHR(rotationProductA, Q20_SHIFT)) -
            stepTermU) *
           (((control->base).left + (control->base).right >> 1) - clipLeft)));
  /* per-pixel texture step (u and v) */
  pixelStepU =
       (FIXED_PRODUCT_SHR(rotationProductA, Q20_SHIFT)) - stepTermU;
  pixelStepV = stepTermU * 2;
  sourceStartV = control->sourceOriginYQ12 -
           (rowStepV * (((control->base).top + (control->base).bottom >> 1) - clipTop) +
           stepTermU * 2 * (((control->base).left + (control->base).right >> 1) - clipLeft));
  sourceTexture = control->textureSource;
  subresourceTable = (sourceTexture->tableDescriptor).subresourceTableOffset;
  /* fields of the first subresource record (asset + subresourceTableOffset) */
  if (*(int *)((uint8_t *)sourceTexture + subresourceTable + GFX_SUBRESOURCE_PALETTE_INDEX) >= 0) {
    return;
  }
  sourceWidth =
       *(int *)((uint8_t *)sourceTexture + subresourceTable + GFX_SUBRESOURCE_PIXEL_WIDTH);
  sourceHeight =
       *(int *)((uint8_t *)sourceTexture + subresourceTable + GFX_SUBRESOURCE_PIXEL_HEIGHT);
  pixelDataOffset =
       *(int *)((uint8_t *)sourceTexture + subresourceTable + GFX_SUBRESOURCE_PIXEL_OFFSET);
  framebufferUnavailable = g_GraphicsFramebufferBeginAccess();
  if (framebufferUnavailable) {
    return;
  }
  rowStepUHigh = (int)(rowStepUWide >> 32);
  sourceU = (uint32_t)sourceStartU;
  sourceStartUHigh = (int)(sourceStartU >> 32);
  remainingRows = clipHeight;
  remainingColumns = clipWidth;
  if (g_FramebufferAccess->bytesPerPixel == SOFTWARE_FRAMEBUFFER_PIXEL_BYTES_16BIT) {
    destPixel = g_FramebufferAccess->pixels +
              g_FramebufferRowStrideBytes * clipTop + clipLeft * 2;
    sourceV = sourceStartUHigh + sourceStartV;
    rowStartU = sourceU;
    rowStartV = sourceV;
    destRowStart = destPixel;
    do {
      do {
        blendedPixel = UiSelectionGeometryControl_SampleBilinear
                                 (sourceTexture,pixelDataOffset,sourceWidth,sourceHeight,sourceU,sourceV);
        /* 32-bit colour to 16-bit: PUNPCKLBW/PSRLW 4, PAND quantize masks, PMADDWD pack weights,
           then (q >> 40) + (q >> 8) with PADDW; the low word is the pixel. */
        memcpy(&quantizeMaskLanes,&g_SoftwarePixelMmxConstants.quantizeMasksQ12,sizeof(quantizeMaskLanes));
        packedLanes = pmaddwd(UiScaler_UnpackBytesToWordLanes(blendedPixel,4) & quantizeMaskLanes,
                              g_SoftwarePixelMmxConstants.packWeights);
        *(short *)destPixel = (short)(packedLanes >> 40) + (short)(packedLanes >> 8);
        sourceU = sourceU + (int)pixelStepU;
        sourceV = sourceV + pixelStepV;
        destPixel = destPixel + 2;
        remainingColumns = remainingColumns - 1;
      } while (remainingColumns != 0);
      sourceU = rowStartU + (int)rowStepUWide;
      sourceV = rowStartV + rowStepUHigh + rowStepV;
      destPixel = destRowStart + g_FramebufferRowStrideBytes;
      remainingRows = remainingRows - 1;
      rowStartU = sourceU;
      rowStartV = sourceV;
      remainingColumns = clipWidth;
      destRowStart = destPixel;
    } while (remainingRows != 0);
  }
  else {
    destPixel = g_FramebufferAccess->pixels +
              g_FramebufferRowStrideBytes * clipTop + clipLeft * 4;
    sourceV = sourceStartUHigh + sourceStartV;
    rowStartU = sourceU;
    rowStartV = sourceV;
    destRowStart = destPixel;
    do {
      do {
        *(PackedArgb32 *)destPixel =
             UiSelectionGeometryControl_SampleBilinear
                       (sourceTexture,pixelDataOffset,sourceWidth,sourceHeight,sourceU,sourceV);
        sourceU = sourceU + (int)pixelStepU;
        sourceV = sourceV + pixelStepV;
        destPixel = destPixel + 4;
        remainingColumns = remainingColumns - 1;
      } while (remainingColumns != 0);
      sourceU = rowStartU + (int)rowStepUWide;
      sourceV = rowStartV + rowStepUHigh + rowStepV;
      destPixel = destRowStart + g_FramebufferRowStrideBytes;
      remainingRows = remainingRows - 1;
      rowStartU = sourceU;
      rowStartV = sourceV;
      remainingColumns = clipWidth;
      destRowStart = destPixel;
    } while (remainingRows != 0);
  }
  g_GraphicsFramebufferEndAccess();
  return;
}


/* nonRightPress slot of g_UiSelectionGeometryControlVtable. Maps the clicked screen point back into
   texture space with the same rotation/scale as UiSelectionGeometryControl_DrawClipped, stores it in
   selectedSourceXQ12/YQ12 and queues actionId so the handler can read the picked source position.
*/
void UiSelectionGeometryControl_ConvertPointerAndEnqueueAction
          (UiPointerWheelDelta wheelDelta,UiPixelCoordinate pointerY,UiPixelCoordinate pointerX,
          UiSelectionGeometryControl *control)

{
  int cosTerm;
  int boundsLeft;
  int boundsRight;
  int boundsTop;
  int boundsBottom;
  int64_t rotationProductA;
  int64_t rotationProductB;
  int64_t rotationProductC;
  uint32_t sinTerm;
  uint32_t stepTermX;
  uint32_t stepTermY;

  rotationProductA = (int64_t)control->sampleScaleQ12 * (int64_t)g_FixedSineQ28[FIXED_SINE_TABLE_COS + control->rotationAngle];
  cosTerm = -(FIXED_PRODUCT_SHR(rotationProductA, Q28_SHIFT));
  rotationProductA = (int64_t)control->sampleScaleQ12 * (int64_t)g_FixedSineQ28[FIXED_SINE_TABLE_SIN + control->rotationAngle];
  sinTerm = FIXED_PRODUCT_SHR(rotationProductA, Q28_SHIFT);
  /* scale*(sin, cos) mapped through the field-grid lattice factors: products by the column factor are taken
     >> Q20_SHIFT, those by the row factor >> (Q20_SHIFT + 1) (half a row, the lattice skew) */
  rotationProductA = (int64_t)(int)sinTerm * FIELD_GRID_WORLD_X_TO_COLUMN_Q20;
  rotationProductB = (int64_t)cosTerm * FIELD_GRID_WORLD_Y_TO_ROW_Q20;
  stepTermX = FIXED_PRODUCT_SHR(rotationProductB, Q20_SHIFT + 1);
  rotationProductB = (int64_t)cosTerm * FIELD_GRID_WORLD_X_TO_COLUMN_Q20;
  rotationProductC = (int64_t)(int)-sinTerm * FIELD_GRID_WORLD_Y_TO_ROW_Q20;
  stepTermY = FIXED_PRODUCT_SHR(rotationProductC, Q20_SHIFT + 1);
  boundsLeft = (control->base).left;
  boundsRight = (control->base).right;
  boundsTop = (control->base).top;
  boundsBottom = (control->base).bottom;
  control->selectedSourceXQ12 =
       control->sourceOriginXQ12 -
       (((FIXED_PRODUCT_SHR(rotationProductB, Q20_SHIFT)) - stepTermY) *
        (((control->base).top + (control->base).bottom >> 1) - pointerY) +
       ((FIXED_PRODUCT_SHR(rotationProductA, Q20_SHIFT)) - stepTermX) *
       (((control->base).left + (control->base).right >> 1) - pointerX));
  control->selectedSourceYQ12 =
       (control->sourceOriginYQ12 - stepTermX * 2 * ((boundsLeft + boundsRight >> 1) - pointerX)) -
       stepTermY * 2 * ((boundsTop + boundsBottom >> 1) - pointerY);
  UiActionQueue_Enqueue(control->actionId,control);
  return;
}


/* Whether the pointer lies inside root and root takes part in the pointer hit test. */
static Bool8 UiPointer_RootContainsPointer(UiRootNode *root,UiPixelCoordinate pointerY,UiPixelCoordinate pointerX)
{
  return ((root->rootFlags & UI_ROOT_DISABLE_POINTER_HIT_TEST) == 0) &&
         ((root->base).left <= pointerX) && ((root->base).top <= pointerY) &&
         (pointerX < (root->base).right) && (pointerY < (root->base).bottom);
}

/* Press target in the root stack: the hit test of the topmost root (from topRoot down) that contains the
   pointer. After each root the pointer misses, that root's method08 (if any) is called with the root below
   it (possibly UI_ROOT_STACK_END) and ends the search when it returns true. A hit in a root below topRoot
   brings that root to the front first; when UiRootStack_BringToFront returns true the press is dropped.
   Returns UI_NODE_NONE when the press goes nowhere. */
static UiNodeBase *UiPointer_HitTestRootStack
          (UiRootNode *topRoot,UiPixelCoordinate pointerY,UiPixelCoordinate pointerX)
{
  UiRootCallbacks *missedCallbacks;
  UiNodeBase *node;
  UiRootNode *root;

  for (root = topRoot; root != UI_ROOT_STACK_END; ) {
    if (UiPointer_RootContainsPointer(root,pointerY,pointerX)) {
      node = (*((root->base).vtable)->hitTest)(pointerY,pointerX,&root->base);
      if ((root != topRoot) && UiRootStack_BringToFront(root)) {
        return UI_NODE_NONE;
      }
      return node;
    }
    missedCallbacks = root->callbacks;
    root = root->previousRoot;
    if ((missedCallbacks->method08 != NULL) && missedCallbacks->method08(root)) {
      return UI_NODE_NONE;
    }
  }
  return UI_NODE_NONE;
}

/* Press target for the left and middle buttons. A hovered image control (hoverTarget, read before the
   capture check) stops being the hover target; it keeps the press when the pointer is on one of its opaque
   pixels, else it loses its hover state bits and the root stack is hit-tested from topRoot. Returns
   UI_NODE_NONE when the press goes nowhere. */
static UiNodeBase *UiPointer_FindNonRightPressTarget
          (UiImageControl *hoverTarget,UiRootNode *topRoot,UiPixelCoordinate pointerY,UiPixelCoordinate pointerX)
{
  UiSelectableStateFlags *stateFlagsField;
  UiNodeBase *opaqueHit;

  if (hoverTarget != NULL) {
    opaqueHit = UiImageControl_HitTestOpaque(pointerY,pointerX,hoverTarget);
    g_UiImageControlHoverTarget = NULL;
    if (opaqueHit != UI_NODE_NONE) {
      return (UiNodeBase *)hoverTarget;
    }
    stateFlagsField = &(hoverTarget->selectable).stateFlags;
    *stateFlagsField = *stateFlagsField & ~UI_IMAGE_CONTROL_HOVER_STATE_BITS;
  }
  return UiPointer_HitTestRootStack(topRoot,pointerY,pointerX);
}

/* Press tail for all three buttons: node captures the pointer for captureButton, its
   UI_NODE_REPEAT_OR_DOUBLE_CLICK flag records repeatClick, it takes the keyboard focus when it is a focus
   target and gets the press and then, unless the press handler released the capture already, the capture
   target gets the drag (rightPress/rightDrag for the right button, nonRightPress/nonRightDrag otherwise). */
static void UiPointer_CaptureAndPress
          (UiNodeBase *node,UiPointerCaptureButton captureButton,Bool8 repeatClick,UiPointerWheelDelta wheelDelta,
          UiPixelCoordinate pointerY,UiPixelCoordinate pointerX)
{
  UiNodeVtable *nodeVtable;

  g_UiPointerCaptureButton = captureButton;
  if (repeatClick) {
    node->nodeFlags = node->nodeFlags | UI_NODE_REPEAT_OR_DOUBLE_CLICK;
  }
  else {
    node->nodeFlags = node->nodeFlags & ~UI_NODE_REPEAT_OR_DOUBLE_CLICK;
  }
  nodeVtable = node->vtable;
  g_UiPointerCaptureTarget = node;
  if ((node->nodeFlags & (UI_NODE_FALLBACK_FOCUS_TARGET|UI_NODE_PREFERRED_FOCUS_TARGET)) != 0) {
    UiKeyboardFocus_Set(node);
  }
  if (captureButton == UI_POINTER_CAPTURE_RIGHT) {
    nodeVtable->rightPress(wheelDelta,pointerY,pointerX,node);
  }
  else {
    nodeVtable->nonRightPress(wheelDelta,pointerY,pointerX,node);
  }
  /* the press handler may have released the capture already */
  if (g_UiPointerCaptureTarget == UI_NODE_NONE) {
    return;
  }
  if (captureButton == UI_POINTER_CAPTURE_RIGHT) {
    g_UiPointerCaptureTarget->vtable->rightDrag(wheelDelta,pointerY,pointerX,g_UiPointerCaptureTarget);
  }
  else {
    g_UiPointerCaptureTarget->vtable->nonRightDrag(wheelDelta,pointerY,pointerX,g_UiPointerCaptureTarget);
  }
}

/* Left button press: the node under the pointer (a hovered image control on an opaque pixel, else the hit
   test of the topmost root containing the pointer; pressing into a lower root brings it to the front
   first) captures the pointer for the left button, takes the keyboard focus when it is a focus target and
   gets nonRightPress followed by nonRightDrag. Ignored while any button holds a capture.
*/
void UiPointer_DispatchLeftPress(GraphicsCursorButtonState buttonMask,UiPointerWheelDelta wheelDelta,
          UiPixelCoordinate pointerY,UiPixelCoordinate pointerX)

{
  UiImageControl *hoverTarget;
  UiRootNode *topRoot;
  UiNodeBase *node;

  hoverTarget = g_UiImageControlHoverTarget;
  topRoot = g_UiRootNode;
  if (g_UiPointerCaptureButton != UI_POINTER_CAPTURE_NONE) {
    return;
  }
  node = UiPointer_FindNonRightPressTarget(hoverTarget,topRoot,pointerY,pointerX);
  if (node == UI_NODE_NONE) {
    return;
  }
  UiPointer_CaptureAndPress(node,UI_POINTER_CAPTURE_LEFT,
                            (buttonMask & UI_POINTER_BUTTON_REPEAT_CLICK) != CURSOR_BUTTON_NONE,
                            wheelDelta,pointerY,pointerX);
  return;
}


/* Middle button press: like UiPointer_DispatchLeftPress (same node selection, focus and nonRightPress /
   nonRightDrag), but captures the pointer for the middle button and always marks the node's press as a
   repeated click (UI_NODE_REPEAT_OR_DOUBLE_CLICK), whatever buttonMask says.
*/
void UiPointer_DispatchMiddlePress
          (UiPointerButtonMask buttonMask,UiPointerWheelDelta wheelDelta,UiPixelCoordinate pointerY,
          UiPixelCoordinate pointerX)

{
  UiImageControl *hoverTarget;
  UiRootNode *topRoot;
  UiNodeBase *node;

  hoverTarget = g_UiImageControlHoverTarget;
  topRoot = g_UiRootNode;
  if (g_UiPointerCaptureButton != UI_POINTER_CAPTURE_NONE) {
    return;
  }
  node = UiPointer_FindNonRightPressTarget(hoverTarget,topRoot,pointerY,pointerX);
  if (node == UI_NODE_NONE) {
    return;
  }
  UiPointer_CaptureAndPress(node,UI_POINTER_CAPTURE_MIDDLE,true,wheelDelta,pointerY,pointerX);
  return;
}


/* Right button press: the hit test of the topmost root containing the pointer (a hovered image control
   only loses its hover state, it gets no opaque-pixel check) picks the node, which captures the pointer for
   the right button, takes the keyboard focus when it is a focus target and gets rightPress followed by
   rightDrag. Ignored while any button holds a capture.
*/
void UiPointer_DispatchRightPress
          (UiPointerButtonMask buttonMask,UiPointerWheelDelta wheelDelta,UiPixelCoordinate pointerY,
          UiPixelCoordinate pointerX)

{
  UiSelectableStateFlags *stateFlagsField;
  UiRootNode *topRoot;
  UiNodeBase *node;

  topRoot = g_UiRootNode;
  if (g_UiPointerCaptureButton != UI_POINTER_CAPTURE_NONE) {
    return;
  }
  if (g_UiImageControlHoverTarget != NULL) {
    stateFlagsField = &(g_UiImageControlHoverTarget->selectable).stateFlags;
    *stateFlagsField = *stateFlagsField & ~UI_IMAGE_CONTROL_HOVER_STATE_BITS;
  }
  node = UiPointer_HitTestRootStack(topRoot,pointerY,pointerX);
  if (node == UI_NODE_NONE) {
    return;
  }
  UiPointer_CaptureAndPress(node,UI_POINTER_CAPTURE_RIGHT,(buttonMask & UI_POINTER_BUTTON_REPEAT_CLICK) != 0,
                            wheelDelta,pointerY,pointerX);
  return;
}


/* The node after node in pre-order: its first child, else the next sibling of node or of its nearest ancestor
   that has one; at the end of the tree the topmost ancestor (the walk wraps around). */
static UiNodeBase *UiKeyboardFocus_NextInPreOrderWrapping(UiNodeBase *node)
{
  UiNodeBase *nextNode;

  nextNode = node->firstChild;
  if (nextNode != UI_NODE_NONE) {
    return nextNode;
  }
  for (nextNode = node->nextSibling; nextNode == UI_NODE_NONE; nextNode = node->nextSibling) {
    if (node->parent == UI_NODE_NONE) {
      return node;
    }
    node = node->parent;
  }
  return nextNode;
}

/* Moves the keyboard focus to the next focus target after the current one in depth-first tree order,
   wrapping around through the topmost ancestor and skipping suppressed nodes. Nothing changes when there is
   no focus or no other focus target.
*/
void UiKeyboardFocus_MoveNext(void)

{
  UiNodeBase *node;

  node = g_UiKeyboardFocusNode;
  if (g_UiKeyboardFocusNode == UI_NODE_NONE) {
    return;
  }
  /* Pre-order walk from the focus node, wrapping around through the topmost ancestor, until a focus target
     that is the focus node itself or is not suppressed. */
  do {
    node = UiKeyboardFocus_NextInPreOrderWrapping(node);
  } while (((node->nodeFlags & (UI_NODE_FALLBACK_FOCUS_TARGET|UI_NODE_PREFERRED_FOCUS_TARGET)) == 0) ||
           ((node != g_UiKeyboardFocusNode) && ((node->nodeFlags & UI_NODE_SUPPRESSED) != 0)));
  if (node == g_UiKeyboardFocusNode) {
    return; /* back at the focus node: no other focus target */
  }
  UiKeyboardFocus_Set(node);
  return;
}


/* Pointer motion (and wheel): drops a hovered in-game selection record (rebuilding the detail panel) and
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
  while (root != UI_ROOT_STACK_END) {
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
    if (-1 < missPolicyResult) {
      return;
    }
    root = root->previousRoot;
  }
  return;
}


/* Default pointerWheel slot of most UI vtables: passes the wheel event up to the parent node (if any), so
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


/* Default keyboardEvent slot of many UI vtables, also the fallback of the slider and focus-proxy handlers.
   It always returns true (key not consumed): the original compares the key with KEYBOARD_KEY_CODE_TAB but
   then reports "not consumed" regardless of the result, so the focus move its name suggests never happens.
*/
Bool8 UiNode_DefaultKeyboardEventMoveFocusNext
          (UiKeyboardStateMask keyboardStateMask,UiKeyboardEventCode keyCode,UiNodeBase *control)

{
  return true;
}


/* Moves the keyboard focus to node (UI_NODE_NONE clears it), keeping UI_NODE_HAS_KEYBOARD_FOCUS on the
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


/* Class vtables. */

UiNodeVtable g_UiFocusProxyControlVtable = {
        .relocate = (void *)UiSingleLineTextControl_RelocateChild,
        .method04 = (void *)UiNode_DefaultMethod04_NoOp,
        .drawClipped = (void *)UiSingleLineTextControl_DrawClipped,
        .layout = (void *)UiContainer_LayoutChildren,
        .nonRightPress = (void *)UiSingleLineTextControl_ForwardNonRightPressToChild,
        .nonRightRelease = (void *)UiSingleLineTextControl_ForwardNonRightReleaseToChild,
        .rightPress = (void *)UiSingleLineTextControl_ForwardRightPressToChild,
        .rightRelease = (void *)UiSingleLineTextControl_ForwardRightReleaseToChild,
        .nonRightDrag = (void *)UiSingleLineTextControl_ForwardNonRightDragToChild,
        .rightDrag = (void *)UiSingleLineTextControl_ForwardRightDragToChild,
        .pointerMove = (void *)UiSingleLineTextControl_ForwardPointerMoveToChild,
        .hitTest = (void *)UiSingleLineTextControl_HitTestChildProxy,
        .keyboardEvent = (void *)UiSingleLineTextControl_ForwardKeyboardEventToChild,
        .applyFlags = (void *)UiNode_ApplyFlagsRecursive,
        .suppressActionId = (void *)UiContainer_SuppressActionId,
        .unsuppressActionId = (void *)UiContainer_UnsuppressActionId,
        .tick = (void *)UiSingleLineTextControl_ForwardTickToChild,
        .pointerWheel = (void *)UiSingleLineTextControl_ForwardPointerWheelToChildOrParent};

UiNodeVtable g_UiSelectionGeometryControlVtable = {
        .relocate = (void *)UiContainer_RelocateChildren,
        .method04 = (void *)UiNode_DefaultMethod04_NoOp,
        .drawClipped = (void *)UiSelectionGeometryControl_DrawClipped,
        .layout = (void *)UiContainer_LayoutChildren,
        .nonRightPress = (void *)UiSelectionGeometryControl_ConvertPointerAndEnqueueAction,
        .nonRightRelease = (void *)UiNode_DefaultNonRightRelease,
        .rightPress = (void *)UiNode_ForwardRightPressToParent,
        .rightRelease = (void *)UiNode_DefaultRightRelease,
        .nonRightDrag = (void *)UiNode_DefaultNonRightDrag,
        .rightDrag = (void *)UiNode_DefaultRightDrag,
        .pointerMove = (void *)UiNode_DefaultPointerMove,
        .hitTest = (void *)UiContainer_HitTestChildren,
        .keyboardEvent = (void *)UiNode_DefaultKeyboardEventMoveFocusNext,
        .applyFlags = (void *)UiNode_ApplyFlagsRecursive,
        .suppressActionId = (void *)UiContainer_SuppressActionId,
        .unsuppressActionId = (void *)UiContainer_UnsuppressActionId,
        .tick = (void *)UiNode_DefaultTick,
        .pointerWheel = (void *)UiNode_ForwardPointerWheelToParent,
};

UiNodeVtable g_UiCommandVisibilitySingleLineTextVtable = {
    .relocate = (void *)UiSingleLineTextControl_RelocateChild,
    .method04 = (void *)UiNode_DefaultMethod04_NoOp,
    .drawClipped = (void *)UiCommandVisibilitySingleLineText_DrawWhenAllowed,
    .layout = (void *)UiContainer_LayoutChildren,
    .nonRightPress = (void *)UiNode_DefaultNonRightPress,
    .nonRightRelease = (void *)UiNode_DefaultNonRightRelease,
    .rightPress = (void *)UiNode_ForwardRightPressToParent,
    .rightRelease = (void *)UiNode_DefaultRightRelease,
    .nonRightDrag = (void *)UiNode_DefaultNonRightDrag,
    .rightDrag = (void *)UiNode_DefaultRightDrag,
    .pointerMove = (void *)UiNode_DefaultPointerMove,
    .hitTest = (void *)FrontendResultsTable_HitTestAlwaysNone,
    .keyboardEvent = (void *)UiNode_DefaultKeyboardEventMoveFocusNext,
    .applyFlags = (void *)UiNode_ApplyFlagsRecursive,
    .suppressActionId = (void *)UiContainer_SuppressActionId,
    .unsuppressActionId = (void *)UiContainer_UnsuppressActionId,
    .tick = (void *)UiNode_DefaultTick,
    .pointerWheel = (void *)UiNode_ForwardPointerWheelToParent,
};

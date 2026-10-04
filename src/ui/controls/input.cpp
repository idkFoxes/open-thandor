/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/src/ui/controls/input.cpp
 * Reverse engineering by idkFoxes 2026
 */

#include <thandor/ui/controls/input.h>
#include <thandor/thandor.h>
#include <thandor/platform/bootstrap/image.h>

/* Module data. */

UiNodeBase *g_UiPointerCaptureTarget = UI_NODE_NONE;

UiPointerCaptureButton g_UiPointerCaptureButton = 255;

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
                THANDOR_PTR(holder),Thandor_SymbolName(holder->vtable),holder->nodeFlags,field,THANDOR_PTR(link),
                THANDOR_PTR(g_UiKeyboardFocusNode),
                g_UiKeyboardFocusNode != UI_NODE_NONE ?
                Thandor_SymbolName(g_UiKeyboardFocusNode->vtable) : "-");
  }
  return UI_NODE_NONE;
}

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

/* Delivers the queued mouse events to the UI under the frame lock (the original polled its DirectInput mouse
   first; the SDL3 backend queues the events from the message pump). Presses and motion go to the node under the pointer; a release goes to the node
   that captured the pointer with that button, which then loses the capture and the pointer position is
   dispatched again as motion. A release without a matching capture is dropped.
   Original quirk: a captured middle-button release also steps the primary random stream (a call of
   Random_NextPrimary after the motion dispatch; the left/right release paths do not), so the primary seed
   depends on local mouse input. The session simulation draws through g_RandomGeneratorState.next, which is
   Random_NextSecondary during a network session, so this does not desync the game state; only the local
   users of the primary stream (ambient sound/music choice, button animation phases, the field's cell
   animation phases and material variants at level load, sequence tokens) can differ between machines.
*/
void UiPointer_DispatchPendingEvents()

{
  UiNodeBase *control;
  char eventKind;
  UiPixelCoordinate pointerX;
  UiPixelCoordinate pointerY;
  GraphicsCursorButtonState buttonMask;
  UiPointerWheelDelta wheelDelta;
  CursorPointerEvent pointerEvent;

  g_SpinLockAcquire(g_UiRuntimeFrameLock);
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
  g_SpinLockReleaseAndInvoke(g_UiRuntimePostUnlockCallback,g_UiRuntimeFrameLock);
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
void UiKeyboard_DispatchPendingEvents()

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
       (g_UiRootNode->callbacks->keyboardFallback != nullptr)) {
      g_UiRootNode->callbacks->keyboardFallback(keyboardStateMask,keyCode,g_UiRootNode);
    }
  }
  g_SpinLockReleaseAndInvoke(g_UiRuntimePostUnlockCallback,g_UiRuntimeFrameLock);
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
}

/* Default pointerMove slot of most UI vtables (range sliders, labels, lists, ...): the node asks for
   cursor frame 0 (GRAPHICS_CURSOR_FRAME_ARROW).
*/
GraphicsCursorFrameIndex UiNode_DefaultPointerMove(UiPixelCoordinate pointerY,UiPixelCoordinate pointerX,
                                                   UiNodeBase *control)

{
  return GRAPHICS_CURSOR_FRAME_ARROW;
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
    if ((missedCallbacks->method08 != nullptr) && missedCallbacks->method08(root)) {
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

  if (hoverTarget != nullptr) {
    opaqueHit = UiImageControl_HitTestOpaque(pointerY,pointerX,hoverTarget);
    g_UiImageControlHoverTarget = nullptr;
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
  if (g_UiImageControlHoverTarget != nullptr) {
    stateFlagsField = &(g_UiImageControlHoverTarget->selectable).stateFlags;
    *stateFlagsField = *stateFlagsField & ~UI_IMAGE_CONTROL_HOVER_STATE_BITS;
  }
  node = UiPointer_HitTestRootStack(topRoot,pointerY,pointerX);
  if (node == UI_NODE_NONE) {
    return;
  }
  UiPointer_CaptureAndPress(node,UI_POINTER_CAPTURE_RIGHT,(buttonMask & UI_POINTER_BUTTON_REPEAT_CLICK) != 0,
                            wheelDelta,pointerY,pointerX);
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
void UiKeyboardFocus_MoveNext()

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
  if (g_UiHoverSelectionRecord != nullptr) {
    g_UiHoverSelectionRecord = nullptr;
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
    if (missPolicy == nullptr) {
      return;
    }
    missPolicyResult = missPolicy(root);
    if (-1 < missPolicyResult) {
      return;
    }
    root = root->previousRoot;
  }
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
}

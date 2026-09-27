/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/src/ui/controls/input.c
 * Reverse engineering by idkFoxes 2026
 */

#include <thandor/ui/controls/input.h>
#include <thandor/thandor.h>
#include <thandor/platform/bootstrap/image.h>

/* Diagnostics: a UI link that is neither -1 nor a readable node ends the walk and is logged once
   per holder, instead of crashing the focus traversal. */
static UiNodeBase *UiKeyboard_CheckedLink(UiNodeBase *holder,const char *field,UiNodeBase *link)
{
  static int logged;
  if ((link == (UiNodeBase *)0xffffffff) || Thandor_IsReadable(link,0x4c)) {
    return link;
  }
  if (logged < 20) {
    logged++;
    Thandor_Log("ui focus walk: node %p (vtable %s flags %08x) has bad %s link %p; focus %p (vtable %s)",
                (void *)holder,Thandor_SymbolName(holder->vtable),holder->nodeFlags,field,(void *)link,
                (void *)g_UiKeyboardFocusNode,
                g_UiKeyboardFocusNode != (UiNodeBase *)0xffffffff ?
                Thandor_SymbolName(g_UiKeyboardFocusNode->vtable) : "-");
  }
  return (UiNodeBase *)0xffffffff;
}

/* Implementation ownership: ui/controls/input. */

/* Address: 0x004AF500.
   Ownership: ui/controls/input.
   Purpose: Consumes GraphicsCursorInputEvent records. Event 1 is left press, 2 middle press, 3 right press, 5-7
   the corresponding releases, and 0 motion/wheel.
   Local calls: UiPointer_DispatchMotionAndWheel, UiPointer_DispatchRightPress, UiPointer_DispatchLeftPress,
   UiPointer_DispatchMiddlePress.
   Cross-module calls: DirectInputMouse_PollBufferedEvents [platform/input/devices], Random_NextPrimary
   [core/math/random].
*/
void __thandor_void_preserve_eax_ecx_edx UiPointer_DispatchPendingEvents(void)

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
    eventKind = (char)pointerEvent.eventType;
    if (eventKind < '\a') {
      if (eventKind == '\x06') {
        if ((control != (UiNodeBase *)0xffffffff) &&
           (g_UiPointerCaptureButton == UI_POINTER_CAPTURE_MIDDLE)) {
          control->vtable->nonRightRelease(wheelDelta,pointerY,pointerX,control);
          g_UiPointerCaptureButton = UI_POINTER_CAPTURE_NONE;
          g_UiPointerCaptureTarget = (UiNodeBase *)0xffffffff;
          UiPointer_DispatchMotionAndWheel(wheelDelta,pointerY,pointerX);
          Random_NextPrimary();
        }
      }
      else if (eventKind < '\x04') {
        if (eventKind == '\x03') {
          UiPointer_DispatchRightPress(buttonMask,wheelDelta,pointerY,pointerX);
        }
        else if (eventKind < '\x02') {
          if (eventKind == '\x01') {
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
      else if ((control != (UiNodeBase *)0xffffffff) &&
              (g_UiPointerCaptureButton == UI_POINTER_CAPTURE_LEFT)) {
        control->vtable->nonRightRelease(wheelDelta,pointerY,pointerX,control);
        g_UiPointerCaptureButton = UI_POINTER_CAPTURE_NONE;
        g_UiPointerCaptureTarget = (UiNodeBase *)0xffffffff;
        UiPointer_DispatchMotionAndWheel(wheelDelta,pointerY,pointerX);
      }
    }
    else if ((control != (UiNodeBase *)0xffffffff) &&
            (g_UiPointerCaptureButton == UI_POINTER_CAPTURE_RIGHT)) {
      control->vtable->rightRelease(wheelDelta,pointerY,pointerX,control);
      g_UiPointerCaptureButton = UI_POINTER_CAPTURE_NONE;
      g_UiPointerCaptureTarget = (UiNodeBase *)0xffffffff;
      UiPointer_DispatchMotionAndWheel(wheelDelta,pointerY,pointerX);
    }
  }
  g_SpinLockReleaseAndInvoke
            ((SpinLockReleaseCallbackProc *)g_UiRuntimePostUnlockCallback,g_UiRuntimeFrameLock);
  return;
}


/* Address: 0x004B00F0.
   Ownership: ui/controls/input.
   Purpose: Handles ui keyboard focus release node.
   Local calls: UiKeyboardFocus_MoveNext, UiKeyboardFocus_Set.
*/
void __thandor_void_preserve_eax_ecx_edx UiKeyboardFocus_ReleaseNode(UiNodeBase *node)

{
  if (node == g_UiKeyboardFocusNode) {
    UiKeyboardFocus_MoveNext();
    if (node == g_UiKeyboardFocusNode) {
      UiKeyboardFocus_Set((UiNodeBase *)0xffffffff);
    }
  }
  return;
}


/* Address: 0x004AF3D0.
   Ownership: ui/controls/input.
   Purpose: Drains keyboard events under the UI lock, offers each event to the focused node through
   keyboardEvent, traverses alternate focusable nodes when CF requests it, and falls back to the active root
   handler.
   Local calls: UiKeyboardFocus_Set.
*/
void __thandor_void_preserve_eax_ecx_edx UiKeyboard_DispatchPendingEvents(void)

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
    if (g_UiPointerCaptureTarget != (UiNodeBase *)0xffffffff) continue;
    keyboardStateMask = keyboardEvent.eventData;
    keyCode = keyboardEvent.eventCode;
    control = g_UiKeyboardFocusNode;
    dispatchToRoot = true;
    if (control != (UiNodeBase *)0xffffffff) {
      dispatchToRoot = false;
      wrappedOnce = false;
      passToNext = control->vtable->keyboardEvent(keyboardStateMask,keyCode,control);
      /* CF set: offer the event to the following focus targets in pre-order, wrapping around once
         through the topmost ancestor; the first one that takes it gets the keyboard focus. */
      while (passToNext) {
        walkNode = UiKeyboard_CheckedLink(control,"firstChild",control->firstChild);
        if (walkNode == (UiNodeBase *)0xffffffff) {
          while (walkNode = UiKeyboard_CheckedLink(control,"nextSibling",control->nextSibling),
                walkNode == (UiNodeBase *)0xffffffff) {
            walkNode = UiKeyboard_CheckedLink(control,"parent",control->parent);
            if (walkNode == (UiNodeBase *)0xffffffff) break;
            control = walkNode;
          }
          if (walkNode == (UiNodeBase *)0xffffffff) {
            if (wrappedOnce) {
              dispatchToRoot = true;
              break;
            }
            wrappedOnce = true;
            walkNode = control;
          }
        }
        control = walkNode;
        if ((control->nodeFlags & (UI_NODE_FALLBACK_FOCUS_TARGET|UI_NODE_PREFERRED_FOCUS_TARGET)) == 0)
        continue;
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
    if ((dispatchToRoot) && (g_UiRootNode != (UiRootNode *)0xffffffff) &&
       (g_UiRootNode->callbacks->keyboardFallback != (UiRootKeyboardFallback *)0x0)) {
      g_UiRootNode->callbacks->keyboardFallback(keyboardStateMask,keyCode,g_UiRootNode);
    }
  }
  g_SpinLockReleaseAndInvoke
            ((SpinLockReleaseCallbackProc *)g_UiRuntimePostUnlockCallback,g_UiRuntimeFrameLock);
  return;
}


/* Address: 0x004B0030.
   Ownership: ui/controls/input.
   Purpose: Traverses a subtree for an initial keyboard-focus target, preferring nodeFlags bit 0x02 and using bit
   0x20 as a fallback.
   Local calls: UiKeyboardFocus_Set.
*/
void __thandor_void_preserve_ecx_edx UiKeyboardFocus_SelectInitial(UiNodeBase *root)

{
  UiNodeBase *fallbackFocusNode;
  UiNodeBase *searchNodeCursor;
  UiNodeBase *nextNode;

  /* Pre-order walk starting at root (continuing past its subtree through the parents' siblings):
     the first unsuppressed preferred focus target wins, else the last unsuppressed fallback. */
  fallbackFocusNode = (UiNodeBase *)0xffffffff;
  searchNodeCursor = root;
  while( true ) {
    if ((searchNodeCursor->nodeFlags & UI_NODE_SUPPRESSED) == 0) {
      if ((searchNodeCursor->nodeFlags & UI_NODE_PREFERRED_FOCUS_TARGET) != 0) break;
      if ((searchNodeCursor->nodeFlags & UI_NODE_FALLBACK_FOCUS_TARGET) != 0) {
        fallbackFocusNode = searchNodeCursor;
      }
    }
    nextNode = searchNodeCursor->firstChild;
    if (nextNode == (UiNodeBase *)0xffffffff) {
      while ((searchNodeCursor != (UiNodeBase *)0xffffffff) &&
             (nextNode = searchNodeCursor->nextSibling, nextNode == (UiNodeBase *)0xffffffff)) {
        searchNodeCursor = searchNodeCursor->parent;
      }
      if (searchNodeCursor == (UiNodeBase *)0xffffffff) {
        searchNodeCursor = fallbackFocusNode;
        if (fallbackFocusNode == (UiNodeBase *)0xffffffff) {
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
   Ownership: ui/controls/input.
   Purpose: Handles ui keyboard focus acquire if none.
   Local calls: UiKeyboardFocus_Set.
*/
void __thandor_void_preserve_eax_ecx_edx UiKeyboardFocus_AcquireIfNone(UiNodeBase *node)

{
  if ((g_UiKeyboardFocusNode == (UiNodeBase *)0xffffffff) &&
     ((node->nodeFlags & (UI_NODE_FALLBACK_FOCUS_TARGET|UI_NODE_PREFERRED_FOCUS_TARGET)) != 0)) {
    UiKeyboardFocus_Set(node);
  }
  return;
}


/* Address: 0x004B4420.
   Ownership: ui/controls/input.
   Purpose: Binary entry is anchored by g_UiNodeVtable_004B3EF0[12]@004B3EF0.
   Local calls: UiNode_DefaultKeyboardEventMoveFocusNext.
   Cross-module calls: UiActionQueue_Enqueue [ui/core/runtime], UiNode_InvalidateRoot [ui/core/runtime].
*/
bool __thandor_cf_preserve_eax_ecx_edx
UiRangeSliderControl_HandleKeyboard
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
  /* sliderFlags bit 0 selects the key pair (0x10019/0x10011 instead of 0x10014/0x10016). */
  if ((control->sliderFlags & 1) == 0) {
    decreaseKey = 0x10014;
    increaseKey = 0x10016;
  }
  else {
    decreaseKey = 0x10019;
    increaseKey = 0x10011;
  }
  if (keyCode == decreaseKey) {
    if (((keyboardStateMask & 0xc) != 0) ||
       (adjustedSliderValue = control->value - control->stepValue,
       adjustedSliderValue < control->minimumValue)) {
      adjustedSliderValue = control->minimumValue;
    }
  }
  else if (keyCode == increaseKey) {
    if (((keyboardStateMask & 0xc) != 0) ||
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
  if (((control->sliderFlags & 4) != 0) && (control->clickSound != (DirectSoundVoiceSet *)0x0)) {
    g_SoundPlayOneShot(g_UiSoundGainQ15,g_UiSoundGainQ15,control->clickSound);
  }
  UiActionQueue_Enqueue(control->actionId,&control->base);
  UiNode_InvalidateRoot(&control->base);
  return false;
}


/* Address: 0x004B9CB0.
   Ownership: ui/controls/input.
   Purpose: Binary entry is anchored by g_UiNodeVtable_004B9530[12]@004B9530.
   Local calls: UiNode_DefaultKeyboardEventMoveFocusNext.
   Cross-module calls: UiNode_InvalidateRoot [ui/core/runtime].
*/
bool __thandor_cf_preserve_eax_ecx_edx
UiSingleLineTextControl_ForwardKeyboardEventToChild
          (UiKeyboardStateMask keyboardStateMask,UiKeyboardEventCode keyCode,UiSingleLineTextControl *control)

{
  UiNodeBase *childControl;
  bool childHandledEvent;
  bool eventResult;
  
  childControl = control->focusChild;
  if ((keyCode & 0xffff0000) == 0) {
    if (((control->labelFlags & 0x8000) != 0) && ((keyCode & 0x30) != 0)) {
      return false;
    }
  }
  else if (keyCode == 0x10002) {
    eventResult = UiNode_DefaultKeyboardEventMoveFocusNext(keyboardStateMask,0x10002,&control->base);
    return eventResult;
  }
  if (childControl != (UiNodeBase *)0x0) {
    eventResult = childControl->vtable->keyboardEvent(keyboardStateMask,keyCode,childControl);
    if (!eventResult) {
      UiNode_InvalidateRoot(&control->base);
      return false;
    }
  }
  return true;
}


/* Address: 0x004B9DA0.
   Ownership: ui/controls/input.
   Purpose: Binary entry is anchored by g_UiNodeVtable_004B9530[17]@004B9530.
   Local calls: UiNode_ForwardPointerWheelToParent.
   Cross-module calls: UiNode_InvalidateRoot [ui/core/runtime].
*/
void __thandor_preserve_eax_edx
UiSingleLineTextControl_ForwardPointerWheelToChildOrParent
          (UiPointerWheelDelta wheelDelta,UiPixelCoordinate pointerY,UiPixelCoordinate pointerX,
          UiSingleLineTextControl *control)

{
  UiNodeBase *childControl;
  
  if ((control->labelFlags & 0x400) == 0) {
    childControl = control->focusChild;
    control->labelFlags = control->labelFlags | 0x400;
    if (childControl != (UiNodeBase *)0x0) {
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
    control->labelFlags = control->labelFlags & 0xfffffbff;
    return;
  }
  UiNode_ForwardPointerWheelToParent(wheelDelta,pointerY,pointerX,&control->base);
  return;
}


/* Address: 0x004B07F0.
   Ownership: ui/controls/input.
   Purpose: Default pointer-move handler; returns zero in EAX, which callers ignore.
*/
GraphicsCursorFrameIndex __thandor_eax_preserve_ecx_edx
UiNode_DefaultPointerMove(UiPixelCoordinate pointerY,UiPixelCoordinate pointerX,UiNodeBase *control)

{
  return 0;
}


/* Address: 0x004B42D0.
   Ownership: ui/controls/input.
   Purpose: Binary entry is anchored by g_UiNodeVtable_004B3EF0[8]@004B3EF0.
   Cross-module calls: UiActionQueue_Enqueue [ui/core/runtime], UiNode_InvalidateRoot [ui/core/runtime].
*/
void __thandor_void_preserve_eax_ecx_edx
UiRangeSliderControl_UpdateValueFromPointer
          (UiPointerWheelDelta wheelDelta,UiPixelCoordinate pointerY,UiPixelCoordinate pointerX,
          UiRangeSliderControl *control)

{
  uint64_t scaledOffset;
  uint32_t pointerOffset;
  int32_t sliderValue;
  uint32_t trackLength;
  TextureSizeResult thumbSize;

  if ((control->sliderFlags & 2) != 0) {
    if ((control->sliderFlags & 1) == 0) {
      thumbSize = g_GraphicsTextureSourceGetLogicalSize(0xaf,g_UiWindowTextureSource);
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
      sliderValue = control->minimumValue +
               (uint32_t)(trackLength < (uint32_t)((int)(scaledOffset % (uint64_t)trackLength) * 2)) + (int)(scaledOffset / trackLength);
      if (control->maximumValue < sliderValue) {
        sliderValue = control->maximumValue;
      }
      if ((control->sliderFlags & 8) != 0) {
        sliderValue = control->maximumValue - (sliderValue - control->minimumValue);
      }
      control->value = sliderValue;
      UiActionQueue_Enqueue(control->actionId,&control->base);
      UiNode_InvalidateRoot(&control->base);
      return;
    }
    thumbSize = g_GraphicsTextureSourceGetLogicalSize(0xb7,g_UiWindowTextureSource);
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
             (uint32_t)(trackLength < (uint32_t)((int)(scaledOffset % (uint64_t)trackLength) * 2)) + (int)(scaledOffset / trackLength);
    if (control->maximumValue < sliderValue) {
      sliderValue = control->maximumValue;
    }
    if ((control->sliderFlags & 8) != 0) {
      sliderValue = control->maximumValue - (sliderValue - control->minimumValue);
    }
    control->value = sliderValue;
    UiActionQueue_Enqueue(control->actionId,&control->base);
    UiNode_InvalidateRoot(&control->base);
  }
  return;
}


/* Address: 0x004B4570.
   Ownership: ui/controls/input.
   Purpose: Binary entry is anchored by g_UiNodeVtable_004B3EF0[17]@004B3EF0.
   Cross-module calls: UiActionQueue_Enqueue [ui/core/runtime], UiNode_InvalidateRoot [ui/core/runtime].
*/
void __thandor_void_preserve_eax_ecx
UiRangeSliderControl_HandlePointerWheel
          (UiPointerWheelDelta wheelDelta,UiPixelCoordinate pointerY,UiPixelCoordinate pointerX,
          UiRangeSliderControl *control)

{
  int32_t adjustedSliderValue;

  if ((((control->sliderFlags & 2) == 0) && ((control->base.nodeFlags & UI_NODE_SUPPRESSED) == 0)
      ) && (wheelDelta != 0)) {
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
   Ownership: ui/controls/input.
   Purpose: Binary entry is anchored by g_UiNodeVtable_004B9530[0]@004B9530; g_UiNodeVtable_00517FC0[0]@00517FC0.
   Cross-module calls: UiContainer_RelocateChildren [ui/controls/layout].
*/
void __thandor_void_preserve_eax_ecx_edx
UiSingleLineTextControl_RelocateChild
          (UiSerializedRelocationDelta relocationDelta,UiSingleLineTextControl *control)

{
  /* EBX is the control, the same node as the stack argument; the relocate vtable slot passes
     only (delta, control). */
  UiSingleLineTextControl *controlReg = control;
  UiNodeFlags *childNodeFlagsField;

  if ((((controlReg->base).nodeFlags &
        (UI_NODE_FALLBACK_FOCUS_TARGET|UI_NODE_PREFERRED_FOCUS_TARGET)) == 0) &&
     (controlReg->focusChild != (UiNodeBase *)0x0)) {
    (controlReg->base).nodeFlags = (controlReg->base).nodeFlags | UI_NODE_FALLBACK_FOCUS_TARGET;
  }
  UiContainer_RelocateChildren(relocationDelta,&controlReg->base);
  if (controlReg->focusChild != (UiNodeBase *)0x0) {
    controlReg->focusChild =
         (UiNodeBase *)((int)&(controlReg->focusChild)->nextSibling + relocationDelta);
    childNodeFlagsField = &(controlReg->focusChild)->nodeFlags;
    *childNodeFlagsField =
         *childNodeFlagsField & ~(UI_NODE_FALLBACK_FOCUS_TARGET|UI_NODE_PREFERRED_FOCUS_TARGET);
  }
  if ((control->labelFlags & 0x20) != 0) {
    control->text = (uint16_t *)((int)control->text + relocationDelta);
    control->labelFlags = control->labelFlags & 0xffffffdf;
  }
  return;
}


/* Address: 0x004B99A0.
   Ownership: ui/controls/input.
   Purpose: Binary entry is anchored by g_UiNodeVtable_004B9530[4]@004B9530.
   Cross-module calls: UiNode_InvalidateRoot [ui/core/runtime].
*/
void __thandor_preserve_eax_edx
UiSingleLineTextControl_ForwardNonRightPressToChild
          (UiPointerWheelDelta wheelDelta,UiPixelCoordinate pointerY,UiPixelCoordinate pointerX,
          UiSingleLineTextControl *control)

{
  UiNodeBase *childControl;
  
  childControl = control->focusChild;
  if (childControl != (UiNodeBase *)0x0) {
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
   Ownership: ui/controls/input.
   Purpose: Binary entry is anchored by g_UiNodeVtable_004B9530[5]@004B9530.
   Cross-module calls: UiNode_InvalidateRoot [ui/core/runtime].
*/
void __thandor_preserve_eax_edx
UiSingleLineTextControl_ForwardNonRightReleaseToChild
          (UiPointerWheelDelta wheelDelta,UiPixelCoordinate pointerY,UiPixelCoordinate pointerX,
          UiSingleLineTextControl *control)

{
  UiNodeBase *childControl;
  
  childControl = control->focusChild;
  if (childControl != (UiNodeBase *)0x0) {
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
   Ownership: ui/controls/input.
   Purpose: Binary entry is anchored by g_UiNodeVtable_004B9530[6]@004B9530.
   Cross-module calls: UiNode_InvalidateRoot [ui/core/runtime].
*/
void __thandor_preserve_eax_edx
UiSingleLineTextControl_ForwardRightPressToChild
          (UiPointerWheelDelta wheelDelta,UiPixelCoordinate pointerY,UiPixelCoordinate pointerX,
          UiSingleLineTextControl *control)

{
  UiNodeBase *childControl;
  
  childControl = control->focusChild;
  if (childControl != (UiNodeBase *)0x0) {
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
   Ownership: ui/controls/input.
   Purpose: Binary entry is anchored by g_UiNodeVtable_004B9530[7]@004B9530.
   Cross-module calls: UiNode_InvalidateRoot [ui/core/runtime].
*/
void __thandor_preserve_eax_edx
UiSingleLineTextControl_ForwardRightReleaseToChild
          (UiPointerWheelDelta wheelDelta,UiPixelCoordinate pointerY,UiPixelCoordinate pointerX,
          UiSingleLineTextControl *control)

{
  UiNodeBase *childControl;
  
  childControl = control->focusChild;
  if (childControl != (UiNodeBase *)0x0) {
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
   Ownership: ui/controls/input.
   Purpose: Binary entry is anchored by g_UiNodeVtable_004B9530[8]@004B9530.
   Cross-module calls: UiNode_InvalidateRoot [ui/core/runtime].
*/
void __thandor_preserve_eax_edx
UiSingleLineTextControl_ForwardNonRightDragToChild
          (UiPointerWheelDelta wheelDelta,UiPixelCoordinate pointerY,UiPixelCoordinate pointerX,
          UiSingleLineTextControl *control)

{
  UiNodeBase *childControl;
  
  childControl = control->focusChild;
  if (childControl != (UiNodeBase *)0x0) {
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
   Ownership: ui/controls/input.
   Purpose: Binary entry is anchored by g_UiNodeVtable_004B9530[9]@004B9530.
   Cross-module calls: UiNode_InvalidateRoot [ui/core/runtime].
*/
void __thandor_preserve_eax_edx
UiSingleLineTextControl_ForwardRightDragToChild
          (UiPointerWheelDelta wheelDelta,UiPixelCoordinate pointerY,UiPixelCoordinate pointerX,
          UiSingleLineTextControl *control)

{
  UiNodeBase *childControl;
  
  childControl = control->focusChild;
  if (childControl != (UiNodeBase *)0x0) {
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
   Ownership: ui/controls/input.
   Purpose: Binary entry is anchored by g_UiNodeVtable_004B9530[10]@004B9530.
   Cross-module calls: UiNode_InvalidateRoot [ui/core/runtime].
*/
GraphicsCursorFrameIndex __thandor_eax_preserve_ecx_edx
UiSingleLineTextControl_ForwardPointerMoveToChild
          (UiPixelCoordinate pointerY,UiPixelCoordinate pointerX,UiSingleLineTextControl *control)

{
  UiNodeBase *childControl;
  GraphicsCursorFrameIndex cursorFrame;
  
  cursorFrame = 0;
  childControl = control->focusChild;
  if (childControl != (UiNodeBase *)0x0) {
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
   Ownership: ui/controls/input.
   Purpose: Binary entry is anchored by g_UiNodeVtable_004B9530[11]@004B9530.
   Cross-module calls: UiContainer_HitTestChildren [ui/controls/layout].
*/
UiNodeBase * __thandor_eax_preserve_ecx_edx
UiSingleLineTextControl_HitTestChildProxy
          (UiPixelCoordinate pointerY,UiPixelCoordinate pointerX,UiSingleLineTextControl *control)

{
  UiNodeBase *hitNode;
  UiNodeBase *returnedNode;
  
  hitNode = UiContainer_HitTestChildren(pointerY,pointerX,&control->base);
  if (hitNode == control->focusChild) {
    returnedNode = &control->base;
    if ((hitNode->nodeFlags & UI_NODE_SUPPRESSED) != 0) {
      returnedNode = (UiNodeBase *)0xffffffff;
    }
  }
  else {
    returnedNode = hitNode;
    if (((hitNode == &control->base) && (control->focusChild != (UiNodeBase *)0x0)) &&
       (((control->focusChild)->nodeFlags & UI_NODE_SUPPRESSED) != 0)) {
      returnedNode = (UiNodeBase *)0xffffffff;
    }
  }
  return returnedNode;
}


/* Address: 0x004B9D40.
   Ownership: ui/controls/input.
   Purpose: Binary entry is anchored by g_UiNodeVtable_004B9530[16]@004B9530.
   Cross-module calls: UiNode_InvalidateRoot [ui/core/runtime].
*/
void __thandor_preserve_eax_edx UiSingleLineTextControl_ForwardTickToChild(UiSingleLineTextControl *control)

{
  UiNodeBase *childControl;
  
  childControl = control->focusChild;
  if (childControl != (UiNodeBase *)0x0) {
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
   Ownership: ui/controls/input.
   Purpose: Opaque-hit-tests pointer motion and forwards pointerMove to a matching child when child routing is
   enabled.
   Cross-module calls: UiContainer_HitTestChildren [ui/controls/layout].
*/
GraphicsCursorFrameIndex __thandor_eax_preserve_ecx_edx
UiImageControl_PointerMove
          (UiPixelCoordinate pointerY,UiPixelCoordinate pointerX,UiImageControl *control)

{
  UiImageControl *hitControl;
  GraphicsCursorFrameIndex cursorFrame;
  bool opaquePixelHit;
  bool opaqueTestResult;
  
  if (((control->selectable).base.nodeFlags & UI_NODE_SUPPRESSED) == 0) {
    if (((control->selectable).stateFlags & 0x40) == 0) {
      opaqueTestResult = g_GraphicsTextureSourceTestOpaquePixel
                        (pointerY,pointerX,(control->selectable).base.top,
                         (control->selectable).base.left,control->normalSubresource,
                         control->textureSource);
      if (opaqueTestResult) {
        return 0;
      }
    }
    else {
      opaqueTestResult = g_GraphicsTextureSourceTestOpaquePixel
                        (pointerY,pointerX,(control->selectable).base.top,
                         (control->selectable).base.left,control->alternateSubresource,
                         control->textureSource);
      if (opaqueTestResult) {
        return 0;
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
      if (g_UiImageControlHoverTarget == (UiImageControl *)0x0) {
        return 8;
      }
    }
  }
  return 0;
}


/* MMX lane helpers for the bilinear scaler below (lanes are little-endian 16-bit words). */

/* PUNPCKLBW mm,mm then PSRLW mm,shift: byte i of pixel becomes word lane i = (byte * 0x101) >> shift. */
static __inline uint64_t UiScaler_UnpackBytesToWordLanes(uint32_t pixel,int shift)
{
  uint64_t lanes;
  int lane;

  lanes = 0;
  for (lane = 0; lane < 4; lane = lane + 1) {
    lanes = lanes |
            (uint64_t)(uint16_t)((uint16_t)(((pixel >> (lane * 8)) & 0xff) * 0x101) >> shift) << (lane * 16);
  }
  return lanes;
}

/* PADDW: lane-wise wrapping 16-bit add. */
static __inline uint64_t UiScaler_AddWordLanes(uint64_t left,uint64_t right)
{
  uint64_t sum;
  int shift;

  sum = 0;
  for (shift = 0; shift < 64; shift = shift + 16) {
    sum = sum | (uint64_t)(uint16_t)((uint16_t)(left >> shift) + (uint16_t)(right >> shift)) << shift;
  }
  return sum;
}

/* PSRLW mm,shift then PACKUSWB (low dword): each lane shifted right, saturated to an unsigned
   byte. The logical shift leaves every lane non-negative, so only the 0xFF clamp applies. */
static __inline uint32_t UiScaler_ShiftAndPackWordLanes(uint64_t lanes,int shift)
{
  uint32_t packed;
  uint16_t laneValue;
  int lane;

  packed = 0;
  for (lane = 0; lane < 4; lane = lane + 1) {
    laneValue = (uint16_t)(lanes >> (lane * 16)) >> shift;
    packed = packed | (uint32_t)(0xff < laneValue ? 0xff : laneValue) << (lane * 8);
  }
  return packed;
}

/* Bilinear blend of a 2x2 texel quad with the scaler weight tables (256 steps per axis):
   rows blended across the column fraction, then across the row fraction, then >> 2 and packed. */
static __inline PackedArgb32 UiScaler_BlendBilinear
          (PackedArgb32 topLeft,PackedArgb32 topRight,PackedArgb32 bottomLeft,PackedArgb32 bottomRight,
          int columnWeight,int rowWeight)
{
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
   Ownership: ui/controls/input.
   Purpose: Handles ui selection geometry control draw clipped.
*/
void __thandor_void_preserve_eax_ecx_edx
UiSelectionGeometryControl_DrawClipped
          (int clipTop,int clipLeft,int clipBottom,int clipRight,UiSelectionGeometryControl *control
          )

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
     (control->textureSource != (GraphicsTextureSourceAsset *)0x0)) {
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
    sinTermOrSourceU = ((int)((uint64_t)rotationProductB >> 0x20) << 0xc | (uint32_t)rotationProductB >> 0x14) - stepTermOrRowStartV;
    cosTermOrRowStepV = stepTermOrRowStartV * 2;
    rowStepU = (uint64_t)sinTermOrSourceU;
    sourceStartU = (uint64_t)
             (control->sourceOriginYQ12 -
             (sinTermOrSourceU * (((control->base).top + (control->base).bottom >> 1) - clipBottom) +
             (((int)((uint64_t)rotationProductA >> 0x20) << 0xc | (uint32_t)rotationProductA >> 0x14) - stepTermOrRowStartU) *
             (((control->base).left + (control->base).right >> 1) - clipRight)));
    /* Per-pixel texture step (MM0 low/high in the original); the decompiler lost both. */
    pixelStepU = ((int)((uint64_t)rotationProductA >> 0x20) << 0xc | (uint32_t)rotationProductA >> 0x14) - stepTermOrRowStartU;
    pixelStepV = stepTermOrRowStartU * 2;
    texelIndexOrFraction = control->sourceOriginXQ12 -
             (cosTermOrRowStepV * (((control->base).top + (control->base).bottom >> 1) - clipBottom) +
             stepTermOrRowStartU * 2 * (((control->base).left + (control->base).right >> 1) - clipRight));
    sourceTexture = control->textureSource;
    subresourceTable = (sourceTexture->tableDescriptor).subresourceTableOffset;
    if (*(int *)((sourceTexture->common).buildMetadata.assetRelativeAddressAnchor28 + (subresourceTable - 0x20)) < 0)
    {
      sourceWidth = *(int *)((sourceTexture->common).buildMetadata.assetRelativeAddressAnchor28 + (subresourceTable - 0x10))
      ;
      sourceHeight = *(int *)((sourceTexture->common).buildMetadata.assetRelativeAddressAnchor28 + (subresourceTable - 0xc));
      pixelDataOffset = *(int *)((sourceTexture->common).buildMetadata.assetRelativeAddressAnchor28 + (subresourceTable - 0x1c))
      ;
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
              remainingColumns = remainingColumns + -1;
            } while (remainingColumns != 0);
            sinTermOrSourceU = stepTermOrRowStartU + (int)rowStepU;
            sourceV = stepTermOrRowStartV + rowStepUHigh + cosTermOrRowStepV;
            destPixel = destRowStart + g_FramebufferRowStrideBytes;
            clipTop = clipTop + -1;
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
              remainingColumns = remainingColumns + -1;
            } while (remainingColumns != 0);
            sinTermOrSourceU = stepTermOrRowStartU + (int)rowStepU;
            sourceV = stepTermOrRowStartV + rowStepUHigh + cosTermOrRowStepV;
            destPixel = destRowStart + g_FramebufferRowStrideBytes;
            clipTop = clipTop + -1;
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
   Ownership: ui/controls/input.
   Purpose: Binary entry is anchored by g_UiNodeVtable_00515C70[4]@00515C70.
   Cross-module calls: UiActionQueue_Enqueue [ui/core/runtime].
*/
void __thandor_void_preserve_eax_ecx_edx
UiSelectionGeometryControl_ConvertPointerAndEnqueueAction
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
   Ownership: ui/controls/input.
   Purpose: Hit-tests the root stack, captures the selected node as button LEFT, records the repeat/double-click
   marker in nodeFlags bit 0x80, and dispatches nonRightPress followed by nonRightDrag.
   Local calls: UiKeyboardFocus_Set.
   Cross-module calls: UiImageControl_HitTestOpaque [ui/controls/misc], UiRootStack_BringToFront
   [ui/controls/layout].
*/
void __thandor_void_preserve_eax_ecx_edx
UiPointer_DispatchLeftPress
          (GraphicsCursorButtonState buttonMask,UiPointerWheelDelta wheelDelta,
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
  opaqueHit = (UiNodeBase *)0xffffffff;
  if (g_UiImageControlHoverTarget != (UiImageControl *)0x0) {
    opaqueHit = UiImageControl_HitTestOpaque(pointerY,pointerX,g_UiImageControlHoverTarget);
    g_UiImageControlHoverTarget = (UiImageControl *)0x0;
    if (opaqueHit == (UiNodeBase *)0xffffffff) {
      stateFlagsField = &(node->selectable).stateFlags;
      *stateFlagsField = *stateFlagsField & 0xfffff9fc;
    }
  }
  if (opaqueHit == (UiNodeBase *)0xffffffff) {
    /* Otherwise hit-test the root stack from the top. */
    root = topRoot;
    while( true ) {
      if (root == (UiRootNode *)0xffffffff) {
        return;
      }
      if (((((root->rootFlags & UI_ROOT_DISABLE_POINTER_HIT_TEST) == 0) &&
           ((root->base).left <= pointerX)) && ((root->base).top <= pointerY)) &&
         ((pointerX < (root->base).right && (pointerY < (root->base).bottom)))) break;
      callbacksField = &root->callbacks;
      root = root->previousRoot;
      if (((*callbacksField)->method08 != (UiRootMethod08Callback *)0x0) &&
         (handled = (*(*callbacksField)->method08)(root), handled)) {
        return;
      }
    }
    node = (UiImageControl *)(*((root->base).vtable)->hitTest)(pointerY,pointerX,&root->base);
    if ((root != topRoot) && (handled = UiRootStack_BringToFront(root), handled)) {
      return;
    }
    if (node == (UiImageControl *)0xffffffff) {
      return;
    }
  }
  g_UiPointerCaptureButton = UI_POINTER_CAPTURE_LEFT;
  if ((buttonMask & 0x80000000) == CURSOR_BUTTON_NONE) {
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
  if (g_UiPointerCaptureTarget == (UiNodeBase *)0xffffffff) {
    return;
  }
  g_UiPointerCaptureTarget->vtable->nonRightDrag
            (wheelDelta,pointerY,pointerX,g_UiPointerCaptureTarget);
  return;
}


/* Address: 0x004AFBC0.
   Ownership: ui/controls/input.
   Purpose: Hit-tests the root stack, captures the selected node as button MIDDLE, records the repeat/double-click
   marker in nodeFlags bit 0x80, and dispatches nonRightPress followed by nonRightDrag. Kept distinct from player
   IDs, command opcodes, and resource identifiers. Typed parameters: p0 buttonMask→UiPointerButtonMask_V338.
   Calling convention, storage, body bytes, control flow, and executable data remain unchanged. Typed parameters:
   p2 pointerY→UiPixelCoordinate_V297, p3 pointerX→UiPixelCoordinate_V297.
   Local calls: UiKeyboardFocus_Set.
   Cross-module calls: UiImageControl_HitTestOpaque [ui/controls/misc], UiRootStack_BringToFront
   [ui/controls/layout].
*/
void __thandor_void_preserve_eax_ecx_edx
UiPointer_DispatchMiddlePress
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
  opaqueHit = (UiNodeBase *)0xffffffff;
  if (g_UiImageControlHoverTarget != (UiImageControl *)0x0) {
    opaqueHit = UiImageControl_HitTestOpaque(pointerY,pointerX,g_UiImageControlHoverTarget);
    g_UiImageControlHoverTarget = (UiImageControl *)0x0;
    if (opaqueHit == (UiNodeBase *)0xffffffff) {
      stateFlagsField = &(node->selectable).stateFlags;
      *stateFlagsField = *stateFlagsField & 0xfffff9fc;
    }
  }
  if (opaqueHit == (UiNodeBase *)0xffffffff) {
    /* Otherwise hit-test the root stack from the top. */
    root = topRoot;
    while( true ) {
      if (root == (UiRootNode *)0xffffffff) {
        return;
      }
      if (((((root->rootFlags & UI_ROOT_DISABLE_POINTER_HIT_TEST) == 0) &&
           ((root->base).left <= pointerX)) && ((root->base).top <= pointerY)) &&
         ((pointerX < (root->base).right && (pointerY < (root->base).bottom)))) break;
      callbacksField = &root->callbacks;
      root = root->previousRoot;
      if (((*callbacksField)->method08 != (UiRootMethod08Callback *)0x0) &&
         (handled = (*(*callbacksField)->method08)(root), handled)) {
        return;
      }
    }
    node = (UiImageControl *)(*((root->base).vtable)->hitTest)(pointerY,pointerX,&root->base);
    if ((root != topRoot) && (handled = UiRootStack_BringToFront(root), handled)) {
      return;
    }
    if (node == (UiImageControl *)0xffffffff) {
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
  if (g_UiPointerCaptureTarget == (UiNodeBase *)0xffffffff) {
    return;
  }
  g_UiPointerCaptureTarget->vtable->nonRightDrag
            (wheelDelta,pointerY,pointerX,g_UiPointerCaptureTarget);
  return;
}


/* Address: 0x004AFD10.
   Ownership: ui/controls/input.
   Purpose: Hit-tests the root stack, captures the selected node as button RIGHT, records the repeat/double-click
   marker in nodeFlags bit 0x80, and dispatches rightPress followed by rightDrag. Kept distinct from player IDs,
   command opcodes, and resource identifiers. Typed parameters: p0 buttonMask→UiPointerButtonMask_V338. Calling
   convention, storage, body bytes, control flow, and executable data remain unchanged. Typed parameters: p2
   pointerY→UiPixelCoordinate_V297, p3 pointerX→UiPixelCoordinate_V297.
   Local calls: UiKeyboardFocus_Set.
   Cross-module calls: UiRootStack_BringToFront [ui/controls/layout].
*/
void __thandor_void_preserve_eax_ecx_edx
UiPointer_DispatchRightPress
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
    if (g_UiImageControlHoverTarget != (UiImageControl *)0x0) {
      stateFlagsField = &(g_UiImageControlHoverTarget->selectable).stateFlags;
      *stateFlagsField = *stateFlagsField & 0xfffff9fc;
    }
    while (root != (UiRootNode *)0xffffffff) {
      if (((((root->rootFlags & UI_ROOT_DISABLE_POINTER_HIT_TEST) == 0) &&
           ((root->base).left <= pointerX)) && ((root->base).top <= pointerY)) &&
         ((pointerX < (root->base).right && (pointerY < (root->base).bottom)))) {
        node = (*((root->base).vtable)->hitTest)(pointerY,pointerX,&root->base);
        if ((root != topRoot) && (handled = UiRootStack_BringToFront(root), handled)) {
          return;
        }
        if (node == (UiNodeBase *)0xffffffff) {
          return;
        }
        g_UiPointerCaptureButton = UI_POINTER_CAPTURE_RIGHT;
        if ((buttonMask & 0x80000000) == 0) {
          node->nodeFlags = node->nodeFlags & ~UI_NODE_REPEAT_OR_DOUBLE_CLICK;
        }
        else {
          node->nodeFlags = node->nodeFlags | UI_NODE_REPEAT_OR_DOUBLE_CLICK;
        }
        nodeVtable = node->vtable;
        g_UiPointerCaptureTarget = node;
        if ((node->nodeFlags & (UI_NODE_FALLBACK_FOCUS_TARGET|UI_NODE_PREFERRED_FOCUS_TARGET)) != 0)
        {
          UiKeyboardFocus_Set(node);
        }
        nodeVtable->rightPress(wheelDelta,pointerY,pointerX,node);
        if (g_UiPointerCaptureTarget == (UiNodeBase *)0xffffffff) {
          return;
        }
        g_UiPointerCaptureTarget->vtable->rightDrag
                  (wheelDelta,pointerY,pointerX,g_UiPointerCaptureTarget);
        return;
      }
      callbacksField = &root->callbacks;
      root = root->previousRoot;
      if (((*callbacksField)->method08 != (UiRootMethod08Callback *)0x0) &&
         (handled = (*(*callbacksField)->method08)(root), handled)) {
        return;
      }
    }
  }
  return;
}


/* Address: 0x004AFFA0.
   Ownership: ui/controls/input.
   Purpose: Traverses the UI tree in depth-first order to the next non-suppressed node whose flags include 0x02 or
   0x20, then assigns keyboard focus.
   Local calls: UiKeyboardFocus_Set.
*/
void __thandor_void_preserve_eax_ecx_edx UiKeyboardFocus_MoveNext(void)

{
  UiNodeBase *node;
  UiNodeBase *nextNode;

  node = g_UiKeyboardFocusNode;
  if (g_UiKeyboardFocusNode == (UiNodeBase *)0xffffffff) {
    return;
  }
  /* Pre-order walk from the focus node, wrapping around through the topmost ancestor. */
  while( true ) {
    nextNode = node->firstChild;
    if (nextNode == (UiNodeBase *)0xffffffff) {
      while ((nextNode = node->nextSibling, nextNode == (UiNodeBase *)0xffffffff) &&
             (node->parent != (UiNodeBase *)0xffffffff)) {
        node = node->parent;
      }
      if (nextNode == (UiNodeBase *)0xffffffff) {
        nextNode = node;
      }
    }
    node = nextNode;
    if ((node->nodeFlags & (UI_NODE_FALLBACK_FOCUS_TARGET|UI_NODE_PREFERRED_FOCUS_TARGET)) == 0)
    continue;
    if (node == g_UiKeyboardFocusNode) {
      return;
    }
    if ((node->nodeFlags & UI_NODE_SUPPRESSED) == 0) break;
  }
  UiKeyboardFocus_Set(node);
  return;
}


/* Address: 0x004AFE40.
   Ownership: ui/controls/input.
   Purpose: Updates tooltip/hover tracking, routes motion to the active capture drag slot or to pointerMove on the
   hit-tested node, and sends nonzero wheel delta through pointerWheel.
   Cross-module calls: InGameSelectionDetailPanel_Rebuild [ui/ingame/runtime], UiTooltip_UpdateHoverTarget
   [ui/controls/text].
*/
void __thandor_void_preserve_eax_ecx_edx
UiPointer_DispatchMotionAndWheel
          (UiPointerWheelDelta wheelDelta,UiPixelCoordinate pointerY,UiPixelCoordinate pointerX)

{
  UiNodeVtable *nodeVtable;
  UiRootPointerMissPolicyCallback *missPolicy;
  UiNodeBase *targetNode;
  int missPolicyResult;
  UiRootNode *root;
  
  targetNode = g_UiPointerCaptureTarget;
  if (g_UiHoverSelectionRecord != (UiCommandRuntimeRecordPrefix *)0x0) {
    g_UiHoverSelectionRecord = (UiCommandRuntimeRecordPrefix *)0x0;
    InGameSelectionDetailPanel_Rebuild();
  }
  UiTooltip_UpdateHoverTarget(pointerY,pointerX);
  root = g_UiRootNode;
  if (targetNode != (UiNodeBase *)0xffffffff) {
    if (g_UiPointerCaptureButton == UI_POINTER_CAPTURE_RIGHT) {
      targetNode->vtable->rightDrag(wheelDelta,pointerY,pointerX,targetNode);
    }
    else {
      targetNode->vtable->nonRightDrag(wheelDelta,pointerY,pointerX,targetNode);
    }
    return;
  }
  while( true ) {
    if (root == (UiRootNode *)0xffffffff) {
      return;
    }
    if (((((root->base).left <= pointerX) && ((root->base).top <= pointerY)) &&
        (pointerX < (root->base).right)) && (pointerY < (root->base).bottom)) {
      targetNode = (*((root->base).vtable)->hitTest)(pointerY,pointerX,&root->base);
      if (targetNode == (UiNodeBase *)0xffffffff) {
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
    if (missPolicy == (UiRootPointerMissPolicyCallback *)0x0) {
      return;
    }
    missPolicyResult = missPolicy(root);
    if (-1 < missPolicyResult) break;
    root = root->previousRoot;
  }
  return;
}


/* Address: 0x004B09F0.
   Ownership: ui/controls/input.
   Purpose: Forwards pointerWheel to node->parent when one exists.
*/
void __thandor_preserve_eax_edx
UiNode_ForwardPointerWheelToParent
          (UiPointerWheelDelta wheelDelta,UiPixelCoordinate pointerY,UiPixelCoordinate pointerX,
          UiNodeBase *control)

{
  UiNodeBase *parentControl;
  
  parentControl = control->parent;
  if (parentControl != (UiNodeBase *)0xffffffff) {
    parentControl->vtable->pointerWheel(wheelDelta,pointerY,pointerX,parentControl);
  }
  return;
}


/* Address: 0x004B08C0.
   Ownership: ui/controls/input.
   Purpose: Shared three-argument keyboard fallback in vtable slot +0x30. It always returns CF set (event not
   handled): the original compares the event with 0x00010002 but then sets CF unconditionally (CMP; STC; RET 0xc),
   so the focus move its name suggests never happens.
*/
bool __thandor_cf_preserve_eax_ecx_edx
UiNode_DefaultKeyboardEventMoveFocusNext
          (UiKeyboardStateMask keyboardStateMask,UiKeyboardEventCode keyCode,UiNodeBase *control)

{
  return true;
}


/* Address: 0x004AFF60.
   Ownership: ui/controls/input.
   Purpose: Clears nodeFlags bit 0x04 on the old focus node, stores the new focus node, sets bit 0x04 on it, and
   invalidates the UI.
   Cross-module calls: UiRootStack_InvalidateAll [ui/controls/layout].
*/
void __thandor_void_preserve_eax_ecx_edx UiKeyboardFocus_Set(UiNodeBase *node)

{
  if (g_UiKeyboardFocusNode != node) {
    if (g_UiKeyboardFocusNode != (UiNodeBase *)0xffffffff) {
      g_UiKeyboardFocusNode->nodeFlags =
           g_UiKeyboardFocusNode->nodeFlags & ~UI_NODE_HAS_KEYBOARD_FOCUS;
    }
    g_UiKeyboardFocusNode = node;
    if (node != (UiNodeBase *)0xffffffff) {
      node->nodeFlags = node->nodeFlags | UI_NODE_HAS_KEYBOARD_FOCUS;
    }
  }
  UiRootStack_InvalidateAll();
  return;
}


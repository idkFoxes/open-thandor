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
  GraphicsCursorInputEventRegsCf21 pointerEvent;
  
  (*g_SpinLockAcquire)(g_UiRuntimeFrameLock);
  if (g_PointerSetPosition == DirectInputMouse_SetPosition) {
    DirectInputMouse_PollBufferedEvents();
  }
  while( true ) {
    control = g_UiPointerCaptureTarget;
    pointerEvent = (*g_GraphicsCursorConsumeEvent)();
    pointerX = pointerEvent.pointerX;
    wheelDelta = pointerEvent.wheelDelta;
    pointerY = pointerEvent.pointerY;
    buttonMask = pointerEvent.buttonState;
    if (pointerEvent.carry) break;
    eventKind = (char)pointerEvent.eventCode;
    if (eventKind < '\a') {
      if (eventKind == '\x06') {
        if ((control != (UiNodeBase *)0xffffffff) &&
           (g_UiPointerCaptureButton == UI_POINTER_CAPTURE_MIDDLE)) {
          (*control->vtable->nonRightRelease)(wheelDelta,pointerY,pointerX,control);
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
        (*control->vtable->nonRightRelease)(wheelDelta,pointerY,pointerX,control);
        g_UiPointerCaptureButton = UI_POINTER_CAPTURE_NONE;
        g_UiPointerCaptureTarget = (UiNodeBase *)0xffffffff;
        UiPointer_DispatchMotionAndWheel(wheelDelta,pointerY,pointerX);
      }
    }
    else if ((control != (UiNodeBase *)0xffffffff) &&
            (g_UiPointerCaptureButton == UI_POINTER_CAPTURE_RIGHT)) {
      (*control->vtable->rightRelease)(wheelDelta,pointerY,pointerX,control);
      g_UiPointerCaptureButton = UI_POINTER_CAPTURE_NONE;
      g_UiPointerCaptureTarget = (UiNodeBase *)0xffffffff;
      UiPointer_DispatchMotionAndWheel(wheelDelta,pointerY,pointerX);
    }
  }
  (*g_SpinLockReleaseAndInvoke)
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
   keyboardEventCf, traverses alternate focusable nodes when CF requests it, and falls back to the active root
   handler.
   Local calls: UiKeyboardFocus_Set.
*/
void __thandor_void_preserve_eax_ecx_edx UiKeyboard_DispatchPendingEvents(void)

{
  UiNodeFlags candidateFlags;
  bool wrappedOnce;
  UiKeyboardEventCode keyCode;
  UiKeyboardStateMask keyboardStateMask;
  UiNodeBase *control;
  UiNodeBase *walkNode;
  bool passToNext;
  KeyboardEventEaxEdxCf9 keyboardEvent;
  
  (*g_SpinLockAcquire)(g_UiRuntimeFrameLock);
  do {
    do {
      while( true ) {
        do {
          keyboardEvent = (*g_KeyboardReadEvent)();
          control = g_UiKeyboardFocusNode;
          keyboardStateMask = keyboardEvent.eventData;
          keyCode = keyboardEvent.eventCode;
          if (keyboardEvent.carry) {
            (*g_SpinLockReleaseAndInvoke)
                      ((SpinLockReleaseCallbackProc *)g_UiRuntimePostUnlockCallback,
                       g_UiRuntimeFrameLock);
            return;
          }
        } while (g_UiPointerCaptureTarget != (UiNodeBase *)0xffffffff);
        if (g_UiKeyboardFocusNode != (UiNodeBase *)0xffffffff) break;
UiKeyboard_DispatchEventToRootFallback:
        if ((g_UiRootNode != (UiRootNode *)0xffffffff) &&
           (g_UiRootNode->callbacks->keyboardFallbackCf != (UiRootKeyboardFallbackCf *)0x0)) {
          (*g_UiRootNode->callbacks->keyboardFallbackCf)(keyboardStateMask,keyCode,g_UiRootNode);
        }
      }
      wrappedOnce = false;
      passToNext = (*g_UiKeyboardFocusNode->vtable->keyboardEventCf)
                        (keyboardStateMask,keyCode,g_UiKeyboardFocusNode);
    } while (!passToNext);
    do {
      do {
        walkNode = UiKeyboard_CheckedLink(control,"firstChild",control->firstChild);
        if (walkNode == (UiNodeBase *)0xffffffff) {
          do {
            walkNode = control;
            control = UiKeyboard_CheckedLink(walkNode,"nextSibling",walkNode->nextSibling);
            if (control != (UiNodeBase *)0xffffffff) {
              candidateFlags = control->nodeFlags;
              goto joined_r0x004af45a;
            }
            control = UiKeyboard_CheckedLink(walkNode,"parent",walkNode->parent);
          } while (control != (UiNodeBase *)0xffffffff);
          if (wrappedOnce) goto UiKeyboard_DispatchEventToRootFallback;
          wrappedOnce = true;
          candidateFlags = walkNode->nodeFlags;
          control = walkNode;
        }
        else {
          candidateFlags = walkNode->nodeFlags;
          control = walkNode;
        }
joined_r0x004af45a:;
      } while ((candidateFlags & (UI_NODE_FALLBACK_FOCUS_TARGET|UI_NODE_PREFERRED_FOCUS_TARGET)) == 0);
      if (control == g_UiKeyboardFocusNode) goto UiKeyboard_DispatchEventToRootFallback;
    } while (((control->nodeFlags & UI_NODE_SUPPRESSED) != 0) ||
            (passToNext = (*control->vtable->keyboardEventCf)(keyboardStateMask,keyCode,control), passToNext))
    ;
    UiKeyboardFocus_Set(control);
  } while( true );
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
  UiNodeBase *traversalNode;
  
  fallbackFocusNode = (UiNodeBase *)0xffffffff;
  searchNodeCursor = root;
  if ((root->nodeFlags & UI_NODE_SUPPRESSED) == 0) {
    if ((root->nodeFlags & UI_NODE_PREFERRED_FOCUS_TARGET) != 0)
    goto UiKeyboardFocus_CommitInitialFocusableNode;
    if ((root->nodeFlags & UI_NODE_FALLBACK_FOCUS_TARGET) != 0) {
      fallbackFocusNode = root;
    }
  }
  while( true ) {
    do {
      while (root = searchNodeCursor->firstChild, root == (UiNodeBase *)0xffffffff) {
        while (root = searchNodeCursor->nextSibling, root == (UiNodeBase *)0xffffffff) {
          searchNodeCursor = searchNodeCursor->parent;
          if (searchNodeCursor == (UiNodeBase *)0xffffffff) {
            root = fallbackFocusNode;
            if (fallbackFocusNode == (UiNodeBase *)0xffffffff) {
              return;
            }
            goto UiKeyboardFocus_CommitInitialFocusableNode;
          }
        }
        searchNodeCursor = root;
        if ((root->nodeFlags & UI_NODE_SUPPRESSED) == 0) {
          if ((root->nodeFlags & UI_NODE_PREFERRED_FOCUS_TARGET) != 0)
          goto UiKeyboardFocus_CommitInitialFocusableNode;
          if ((root->nodeFlags & UI_NODE_FALLBACK_FOCUS_TARGET) != 0) {
            fallbackFocusNode = root;
          }
        }
      }
      searchNodeCursor = root;
    } while ((root->nodeFlags & UI_NODE_SUPPRESSED) != 0);
    if ((root->nodeFlags & UI_NODE_PREFERRED_FOCUS_TARGET) != 0) break;
    if ((root->nodeFlags & UI_NODE_FALLBACK_FOCUS_TARGET) != 0) {
      fallbackFocusNode = root;
    }
  }
UiKeyboardFocus_CommitInitialFocusableNode:
  if (root != g_UiKeyboardFocusNode) {
    UiKeyboardFocus_Set(root);
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
   Local calls: UiNode_DefaultKeyboardEventMoveFocusNextCf.
   Cross-module calls: UiActionQueue_Enqueue [ui/core/runtime], UiNode_InvalidateRoot [ui/core/runtime].
*/
bool __thandor_cf_preserve_eax_ecx_edx
UiRangeSliderControl_HandleKeyboardCf
          (UiKeyboardStateMask keyboardStateMask,UiKeyboardEventCode keyCode,UiRangeSliderControl *control)

{
  sdword increasedSliderValue;
  sdword adjustedSliderValue;
  bool delegatedResult;

  if ((control->base.nodeFlags & UI_NODE_SUPPRESSED) != 0) {
UiRangeSliderControl_DelegateUnhandledKeyboardEvent:
    delegatedResult = UiNode_DefaultKeyboardEventMoveFocusNextCf(keyboardStateMask,keyCode,&control->base);
    return delegatedResult;
  }
  if ((control->sliderFlags & 1) == 0) {
    if (keyCode == 0x10014) {
UiRangeSliderControl_DecreaseValueAndNotify:
      if (((keyboardStateMask & 0xc) != 0) ||
         (adjustedSliderValue = control->value - control->stepValue,
         adjustedSliderValue < control->minimumValue)) {
        adjustedSliderValue = control->minimumValue;
      }
      control->value = adjustedSliderValue;
      if (((control->sliderFlags & 4) != 0) && (control->clickSound != (DirectSoundVoiceSet *)0x0)) {
        (*g_SoundPlayOneShot)(g_UiSoundGainQ15,g_UiSoundGainQ15,control->clickSound);
      }
      UiActionQueue_Enqueue(control->actionId,&control->base);
      UiNode_InvalidateRoot(&control->base);
      return false;
    }
    if (keyCode != 0x10016) goto UiRangeSliderControl_DelegateUnhandledKeyboardEvent;
  }
  else if (keyCode != 0x10011) {
    if (keyCode == 0x10019) goto UiRangeSliderControl_DecreaseValueAndNotify;
    goto UiRangeSliderControl_DelegateUnhandledKeyboardEvent;
  }
  if (((keyboardStateMask & 0xc) != 0) ||
     (increasedSliderValue = control->value + control->stepValue,
     control->maximumValue < increasedSliderValue)) {
    increasedSliderValue = control->maximumValue;
  }
  control->value = increasedSliderValue;
  if (((control->sliderFlags & 4) != 0) && (control->clickSound != (DirectSoundVoiceSet *)0x0)) {
    (*g_SoundPlayOneShot)(g_UiSoundGainQ15,g_UiSoundGainQ15,control->clickSound);
  }
  UiActionQueue_Enqueue(control->actionId,&control->base);
  UiNode_InvalidateRoot(&control->base);
  return false;
}


/* Address: 0x004B9CB0.
   Ownership: ui/controls/input.
   Purpose: Binary entry is anchored by g_UiNodeVtable_004B9530[12]@004B9530.
   Local calls: UiNode_DefaultKeyboardEventMoveFocusNextCf.
   Cross-module calls: UiNode_InvalidateRoot [ui/core/runtime].
*/
bool __thandor_cf_preserve_eax_ecx_edx
UiFocusProxyControl_ForwardKeyboardEventToChildCf
          (UiKeyboardStateMask keyboardStateMask,UiKeyboardEventCode keyCode,UiFocusProxyControl *control)

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
    eventResult = UiNode_DefaultKeyboardEventMoveFocusNextCf(keyboardStateMask,0x10002,&control->base);
    return eventResult;
  }
  if (childControl != (UiNodeBase *)0x0) {
    eventResult = (*childControl->vtable->keyboardEventCf)(keyboardStateMask,keyCode,childControl);
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
UiFocusProxyControl_ForwardPointerWheelToChildOrParent
          (UiPointerWheelDelta wheelDelta,UiPixelCoordinate pointerY,UiPixelCoordinate pointerX,
          UiFocusProxyControl *control)

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
      (*childControl->vtable->pointerWheel)(wheelDelta,pointerY,pointerX,childControl);
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
  ulonglong scaledOffset;
  uint pointerOffset;
  sdword sliderValue;
  uint trackLength;
  GraphicsTextureSizeEaxEdxCf9 thumbSize;

  if ((control->sliderFlags & 2) != 0) {
    if ((control->sliderFlags & 1) == 0) {
      thumbSize = (*g_GraphicsTextureSourceGetLogicalSize)(0xaf,g_UiWindowTextureSource);
      trackLength = control->base.layoutWidth - thumbSize.logicalWidthPixels;
      if (trackLength == 0) {
        trackLength = 1;
      }
      pointerOffset = (pointerX - ((int)thumbSize.logicalWidthPixels >> 1)) - control->base.left;
      if ((int)pointerOffset < 0) {
        pointerOffset = 0;
      }
      scaledOffset = (ulonglong)pointerOffset *
              (ulonglong)(uint)(control->maximumValue - control->minimumValue);
      sliderValue = control->minimumValue +
               (uint)(trackLength < (uint)((int)(scaledOffset % (ulonglong)trackLength) * 2)) + (int)(scaledOffset / trackLength);
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
    thumbSize = (*g_GraphicsTextureSourceGetLogicalSize)(0xb7,g_UiWindowTextureSource);
    trackLength = control->base.layoutHeight - thumbSize.logicalHeightPixels;
    if (trackLength == 0) {
      trackLength = 1;
    }
    pointerOffset = (control->base.bottom - pointerY) - ((int)thumbSize.logicalHeightPixels >> 1);
    if ((int)pointerOffset < 0) {
      pointerOffset = 0;
    }
    scaledOffset = (ulonglong)pointerOffset *
            (ulonglong)(uint)(control->maximumValue - control->minimumValue);
    sliderValue = control->minimumValue +
             (uint)(trackLength < (uint)((int)(scaledOffset % (ulonglong)trackLength) * 2)) + (int)(scaledOffset / trackLength);
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
  sdword adjustedSliderValue;

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
UiFocusProxyControl_RelocateChild
          (UiSerializedRelocationDelta relocationDelta,UiFocusProxyControl *control)

{
  /* EBX is the control, the same node as the stack argument; the relocate vtable slot passes
     only (delta, control). */
  UiFocusProxyControl *controlReg = control;
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
    control->text = (word *)((int)control->text + relocationDelta);
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
UiFocusProxyControl_ForwardNonRightPressToChild
          (UiPointerWheelDelta wheelDelta,UiPixelCoordinate pointerY,UiPixelCoordinate pointerX,
          UiFocusProxyControl *control)

{
  UiNodeBase *childControl;
  
  childControl = control->focusChild;
  if (childControl != (UiNodeBase *)0x0) {
    if (&control->base == g_UiKeyboardFocusNode) {
      g_UiKeyboardFocusNode = childControl;
      childControl->nodeFlags = childControl->nodeFlags | UI_NODE_HAS_KEYBOARD_FOCUS;
    }
    (*childControl->vtable->nonRightPress)(wheelDelta,pointerY,pointerX,childControl);
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
UiFocusProxyControl_ForwardNonRightReleaseToChild
          (UiPointerWheelDelta wheelDelta,UiPixelCoordinate pointerY,UiPixelCoordinate pointerX,
          UiFocusProxyControl *control)

{
  UiNodeBase *childControl;
  
  childControl = control->focusChild;
  if (childControl != (UiNodeBase *)0x0) {
    if (&control->base == g_UiKeyboardFocusNode) {
      g_UiKeyboardFocusNode = childControl;
      childControl->nodeFlags = childControl->nodeFlags | UI_NODE_HAS_KEYBOARD_FOCUS;
    }
    (*childControl->vtable->nonRightRelease)(wheelDelta,pointerY,pointerX,childControl);
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
UiFocusProxyControl_ForwardRightPressToChild
          (UiPointerWheelDelta wheelDelta,UiPixelCoordinate pointerY,UiPixelCoordinate pointerX,
          UiFocusProxyControl *control)

{
  UiNodeBase *childControl;
  
  childControl = control->focusChild;
  if (childControl != (UiNodeBase *)0x0) {
    if (&control->base == g_UiKeyboardFocusNode) {
      g_UiKeyboardFocusNode = childControl;
      childControl->nodeFlags = childControl->nodeFlags | UI_NODE_HAS_KEYBOARD_FOCUS;
    }
    if (&control->base != childControl->parent) {
      (*childControl->vtable->rightPress)(wheelDelta,pointerY,pointerX,childControl);
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
UiFocusProxyControl_ForwardRightReleaseToChild
          (UiPointerWheelDelta wheelDelta,UiPixelCoordinate pointerY,UiPixelCoordinate pointerX,
          UiFocusProxyControl *control)

{
  UiNodeBase *childControl;
  
  childControl = control->focusChild;
  if (childControl != (UiNodeBase *)0x0) {
    if (&control->base == g_UiKeyboardFocusNode) {
      g_UiKeyboardFocusNode = childControl;
      childControl->nodeFlags = childControl->nodeFlags | UI_NODE_HAS_KEYBOARD_FOCUS;
    }
    (*childControl->vtable->rightRelease)(wheelDelta,pointerY,pointerX,childControl);
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
UiFocusProxyControl_ForwardNonRightDragToChild
          (UiPointerWheelDelta wheelDelta,UiPixelCoordinate pointerY,UiPixelCoordinate pointerX,
          UiFocusProxyControl *control)

{
  UiNodeBase *childControl;
  
  childControl = control->focusChild;
  if (childControl != (UiNodeBase *)0x0) {
    if (&control->base == g_UiKeyboardFocusNode) {
      g_UiKeyboardFocusNode = childControl;
      childControl->nodeFlags = childControl->nodeFlags | UI_NODE_HAS_KEYBOARD_FOCUS;
    }
    (*childControl->vtable->nonRightDrag)(wheelDelta,pointerY,pointerX,childControl);
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
UiFocusProxyControl_ForwardRightDragToChild
          (UiPointerWheelDelta wheelDelta,UiPixelCoordinate pointerY,UiPixelCoordinate pointerX,
          UiFocusProxyControl *control)

{
  UiNodeBase *childControl;
  
  childControl = control->focusChild;
  if (childControl != (UiNodeBase *)0x0) {
    if (&control->base == g_UiKeyboardFocusNode) {
      g_UiKeyboardFocusNode = childControl;
      childControl->nodeFlags = childControl->nodeFlags | UI_NODE_HAS_KEYBOARD_FOCUS;
    }
    (*childControl->vtable->rightDrag)(wheelDelta,pointerY,pointerX,childControl);
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
UiFocusProxyControl_ForwardPointerMoveToChild
          (UiPixelCoordinate pointerY,UiPixelCoordinate pointerX,UiFocusProxyControl *control)

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
    cursorFrame = (*childControl->vtable->pointerMove)(pointerY,pointerX,childControl);
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
UiFocusProxyControl_HitTestChildProxy
          (UiPixelCoordinate pointerY,UiPixelCoordinate pointerX,UiFocusProxyControl *control)

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
void __thandor_preserve_eax_edx UiFocusProxyControl_ForwardTickToChild(UiFocusProxyControl *control)

{
  UiNodeBase *childControl;
  
  childControl = control->focusChild;
  if (childControl != (UiNodeBase *)0x0) {
    if (&control->base == g_UiKeyboardFocusNode) {
      g_UiKeyboardFocusNode = childControl;
      childControl->nodeFlags = childControl->nodeFlags | UI_NODE_HAS_KEYBOARD_FOCUS;
    }
    (*childControl->vtable->tick)(childControl);
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
      opaqueTestResult = (*g_GraphicsTextureSourceTestOpaquePixel)
                        (pointerY,pointerX,(control->selectable).base.top,
                         (control->selectable).base.left,control->normalSubresource,
                         control->textureSource);
      if (opaqueTestResult) {
        return 0;
      }
    }
    else {
      opaqueTestResult = (*g_GraphicsTextureSourceTestOpaquePixel)
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
  longlong rotationProductA;
  longlong rotationProductB;
  longlong rotationProductC;
  char clampedLane0;
  int sourceColumn;
  uint sinTermOrSourceU;
  uint stepTermOrRowStartU;
  uint stepTermOrRowStartV;
  int sourceRow;
  int clipHeightOrColumnTerm;
  int clipWidth;
  int texelIndexOrFraction;
  byte *destPixel;
  bool framebufferUnavailable;
  uint pixelStepU;
  uint pixelStepV;
  undefined1 channelByte;
  undefined1 sample3Byte2;
  PackedArgb32 sourcePixelSample3;
  undefined4 sample3Word;
  undefined8 mm1PackedValue0;
  ulonglong rowStepU;
  int rowStepUHigh;
  undefined1 sample2Byte1;
  undefined1 sample2Byte2;
  int cosTermOrRowStepV;
  PackedArgb32 sourcePixelSample2;
  undefined4 sample2Word;
  undefined8 mm3PackedValue0;
  undefined8 row1BlendOrPacked;
  ulonglong sourceStartU;
  uint sourceV;
  undefined1 sample1Byte1;
  undefined1 sample1Byte2;
  PackedArgb32 sourcePixelSample1;
  undefined4 sample1Word;
  undefined8 mm5PackedValue0;
  undefined8 sample1WeightedOrRow0Blend;
  ushort lane0Word;
  undefined1 sample0Byte2;
  PackedArgb32 sourcePixelSample0;
  PackedArgb32 sourcePixelSample4;
  ushort lane1Word;
  undefined4 sample0Word;
  undefined4 sample0Load;
  undefined1 sample0Byte1;
  ushort lane2Word;
  undefined8 mm7PackedValue0;
  undefined8 row0BlendOrSample2;
  ushort lane3Word;
  undefined8 sample0Weighted;
  int remainingColumns;
  byte *destRowStart;
  char clampedLane1;
  char clampedLane2;
  char clampedLane3;
  undefined4 sample1Default;
  
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
    rotationProductA = (longlong)control->sampleScaleQ12 * (longlong)g_FixedCosQ28[control->rotationAngle];
    cosTermOrRowStepV = -((int)((ulonglong)rotationProductA >> 0x20) << 4 | (uint)rotationProductA >> 0x1c);
    rotationProductA = (longlong)control->sampleScaleQ12 * (longlong)g_FixedSinQ28[control->rotationAngle];
    sinTermOrSourceU = (int)((ulonglong)rotationProductA >> 0x20) << 4 | (uint)rotationProductA >> 0x1c;
    rotationProductA = (longlong)(int)sinTermOrSourceU * 0x1c6e9c;
    rotationProductB = (longlong)cosTermOrRowStepV * -0x20c8cc;
    stepTermOrRowStartU = (int)((ulonglong)rotationProductB >> 0x20) << 0xb | (uint)rotationProductB >> 0x15;
    rotationProductB = (longlong)cosTermOrRowStepV * 0x1c6e9c;
    rotationProductC = (longlong)(int)-sinTermOrSourceU * -0x20c8cc;
    stepTermOrRowStartV = (int)((ulonglong)rotationProductC >> 0x20) << 0xb | (uint)rotationProductC >> 0x15;
    sinTermOrSourceU = ((int)((ulonglong)rotationProductB >> 0x20) << 0xc | (uint)rotationProductB >> 0x14) - stepTermOrRowStartV;
    cosTermOrRowStepV = stepTermOrRowStartV * 2;
    rowStepU = (ulonglong)sinTermOrSourceU;
    sourceStartU = (ulonglong)
             (control->sourceOriginYQ12 -
             (sinTermOrSourceU * (((control->base).top + (control->base).bottom >> 1) - clipBottom) +
             (((int)((ulonglong)rotationProductA >> 0x20) << 0xc | (uint)rotationProductA >> 0x14) - stepTermOrRowStartU) *
             (((control->base).left + (control->base).right >> 1) - clipRight)));
    /* Per-pixel texture step (MM0 low/high in the original); the decompiler lost both. */
    pixelStepU = ((int)((ulonglong)rotationProductA >> 0x20) << 0xc | (uint)rotationProductA >> 0x14) - stepTermOrRowStartU;
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
      framebufferUnavailable = (*g_GraphicsFramebufferBeginAccess)();
      if (!framebufferUnavailable) {
        rowStepUHigh = (int)(rowStepU >> 0x20);
        sinTermOrSourceU = (uint)sourceStartU;
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
              sourcePixelSample0 = 0;
              sourcePixelSample1 = 0;
              sourcePixelSample1 = 0;
              sourcePixelSample2 = 0;
              sourcePixelSample2 = 0;
              sourcePixelSample3 = 0;
              sourceRow = (int)sourceV >> 0xc;
              sourceColumn = (int)sinTermOrSourceU >> 0xc;
              texelIndexOrFraction = sourceWidth * sourceRow + sourceColumn;
              clipHeightOrColumnTerm = sourceColumn + 1;
              if (sourceRow < sourceHeight) {
                sourcePixelSample0 = sourcePixelSample0;
                sourcePixelSample1 = sourcePixelSample1;
                if ((-1 < sourceRow) && (sourceColumn < sourceWidth)) {
                  sourcePixelSample4 = sourcePixelSample0;
                  if (-1 < sourceColumn) {
                    sourcePixelSample4 =
                         *(PackedArgb32 *)
                          ((sourceTexture->common).buildMetadata.assetRelativeAddressAnchor28 +
                          texelIndexOrFraction * 4 + pixelDataOffset + -0x28);
                  }
                  sourcePixelSample0 = sourcePixelSample4;
                  if ((-1 < clipHeightOrColumnTerm) && (clipHeightOrColumnTerm < sourceWidth)) {
                    sourcePixelSample1 =
                         *(PackedArgb32 *)
                          ((sourceTexture->common).buildMetadata.assetRelativeAddressAnchor28 +
                          texelIndexOrFraction * 4 + pixelDataOffset + -0x24);
                  }
                }
                if (((-1 < sourceRow + 1) && (sourceRow + 1 < sourceHeight)) && (sourceColumn < sourceWidth)) {
                  sourcePixelSample2 = sourcePixelSample2;
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
              channelByte = (undefined1)(sourcePixelSample0 >> 0x18);
              lane0Word = CONCAT11(channelByte,channelByte);
              sample0Byte2 = (undefined1)(sourcePixelSample0 >> 0x10);
              sample0Byte1 = (undefined1)(sourcePixelSample0 >> 8);
              channelByte = (undefined1)(sourcePixelSample1 >> 0x18);
              lane1Word = CONCAT11(channelByte,channelByte);
              sample1Byte2 = (undefined1)(sourcePixelSample1 >> 0x10);
              sample1Byte1 = (undefined1)(sourcePixelSample1 >> 8);
              channelByte = (undefined1)(sourcePixelSample2 >> 0x18);
              lane2Word = CONCAT11(channelByte,channelByte);
              sample2Byte2 = (undefined1)(sourcePixelSample2 >> 0x10);
              sample2Byte1 = (undefined1)(sourcePixelSample2 >> 8);
              channelByte = (undefined1)(sourcePixelSample3 >> 0x18);
              lane3Word = CONCAT11(channelByte,channelByte);
              sample3Byte2 = (undefined1)(sourcePixelSample3 >> 0x10);
              channelByte = (undefined1)(sourcePixelSample3 >> 8);
              texelIndexOrFraction = (int)(sourceV & 0xfff) >> 4;
              clipHeightOrColumnTerm = (int)(sinTermOrSourceU & 0xfff) >> 4;
              mm7PackedValue0 =
                   pmulhw(CONCAT26(lane0Word >> 2,
                                   CONCAT24((ushort)CONCAT31(CONCAT21(lane0Word,sample0Byte2),sample0Byte2) >> 2,
                                            CONCAT22(CONCAT11(sample0Byte1,sample0Byte1) >> 2,
                                                     CONCAT11((char)sourcePixelSample0,
                                                              (char)sourcePixelSample0) >> 2))),
                          *(undefined8 *)(clipHeightOrColumnTerm * 8 + THANDOR_ADDR(g_UiScalerFirstPixelWeights,0)));
              mm5PackedValue0 =
                   pmulhw(CONCAT26(lane1Word >> 2,
                                   CONCAT24((ushort)CONCAT31(CONCAT21(lane1Word,sample1Byte2),sample1Byte2) >> 2,
                                            CONCAT22(CONCAT11(sample1Byte1,sample1Byte1) >> 2,
                                                     CONCAT11((char)sourcePixelSample1,
                                                              (char)sourcePixelSample1) >> 2))),
                          *(undefined8 *)(clipHeightOrColumnTerm * 8 + THANDOR_ADDR(g_UiScalerSecondPixelWeights,0)));
              mm3PackedValue0 =
                   pmulhw(CONCAT26(lane2Word >> 2,
                                   CONCAT24((ushort)CONCAT31(CONCAT21(lane2Word,sample2Byte2),sample2Byte2) >> 2,
                                            CONCAT22(CONCAT11(sample2Byte1,sample2Byte1) >> 2,
                                                     CONCAT11((char)sourcePixelSample2,
                                                              (char)sourcePixelSample2) >> 2))),
                          *(undefined8 *)(clipHeightOrColumnTerm * 8 + THANDOR_ADDR(g_UiScalerFirstPixelWeights,0)));
              mm1PackedValue0 =
                   pmulhw(CONCAT26(lane3Word >> 2,
                                   CONCAT24((ushort)CONCAT31(CONCAT21(lane3Word,sample3Byte2),sample3Byte2) >> 2,
                                            CONCAT22(CONCAT11(channelByte,channelByte) >> 2,
                                                     CONCAT11((char)sourcePixelSample3,
                                                              (char)sourcePixelSample3) >> 2))),
                          *(undefined8 *)(clipHeightOrColumnTerm * 8 + THANDOR_ADDR(g_UiScalerSecondPixelWeights,0)));
              row0BlendOrSample2 = pmulhw(CONCAT26((short)((ulonglong)mm7PackedValue0 >> 0x30) +
                                       (short)((ulonglong)mm5PackedValue0 >> 0x30),
                                       CONCAT24((short)((ulonglong)mm7PackedValue0 >> 0x20) +
                                                (short)((ulonglong)mm5PackedValue0 >> 0x20),
                                                CONCAT22((short)((ulonglong)mm7PackedValue0 >> 0x10)
                                                         + (short)((ulonglong)mm5PackedValue0 >>
                                                                  0x10),
                                                         (short)mm7PackedValue0 +
                                                         (short)mm5PackedValue0))),
                              *(undefined8 *)(texelIndexOrFraction * 8 + THANDOR_ADDR(g_UiScalerFirstPixelWeights,0)));
              row1BlendOrPacked = pmulhw(CONCAT26((short)((ulonglong)mm3PackedValue0 >> 0x30) +
                                       (short)((ulonglong)mm1PackedValue0 >> 0x30),
                                       CONCAT24((short)((ulonglong)mm3PackedValue0 >> 0x20) +
                                                (short)((ulonglong)mm1PackedValue0 >> 0x20),
                                                CONCAT22((short)((ulonglong)mm3PackedValue0 >> 0x10)
                                                         + (short)((ulonglong)mm1PackedValue0 >>
                                                                  0x10),
                                                         (short)mm3PackedValue0 +
                                                         (short)mm1PackedValue0))),
                              *(undefined8 *)(texelIndexOrFraction * 8 + THANDOR_ADDR(g_UiScalerSecondPixelWeights,0)));
              lane0Word = (ushort)((short)row0BlendOrSample2 + (short)row1BlendOrPacked) >> 2;
              lane1Word = (ushort)((short)((ulonglong)row0BlendOrSample2 >> 0x10) +
                               (short)((ulonglong)row1BlendOrPacked >> 0x10)) >> 2;
              lane2Word = (ushort)((short)((ulonglong)row0BlendOrSample2 >> 0x20) +
                               (short)((ulonglong)row1BlendOrPacked >> 0x20)) >> 2;
              lane3Word = (ushort)((short)((ulonglong)row0BlendOrSample2 >> 0x30) +
                               (short)((ulonglong)row1BlendOrPacked >> 0x30)) >> 2;
              clampedLane0 = (lane0Word != 0) * (lane0Word < 0x100) * (char)lane0Word - (0xff < lane0Word);
              clampedLane1 = (lane1Word != 0) * (lane1Word < 0x100) * (char)lane1Word - (0xff < lane1Word);
              clampedLane2 = (lane2Word != 0) * (lane2Word < 0x100) * (char)lane2Word - (0xff < lane2Word);
              clampedLane3 = (lane3Word != 0) * (lane3Word < 0x100) * (char)lane3Word - (0xff < lane3Word);
              lane0Word = CONCAT11(clampedLane3,clampedLane3);
              row1BlendOrPacked = pmaddwd(CONCAT26(lane0Word >> 4,
                                        CONCAT24((ushort)CONCAT31(CONCAT21(lane0Word,clampedLane2),clampedLane2) >>
                                                 4,CONCAT22(CONCAT11(clampedLane1,clampedLane1) >> 4,
                                                            CONCAT11(clampedLane0,clampedLane0) >> 4))) &
                               THANDOR_BITCAST(SoftwareRgbWordLanes, ulonglong, g_SoftwarePixelMmxConstants.quantizeMasksQ12),
                               g_SoftwarePixelMmxConstants.packWeights);
              *(short *)destPixel =
                   (short)((ulonglong)row1BlendOrPacked >> 0x28) + (short)((ulonglong)row1BlendOrPacked >> 8);
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
              sample0Load = 0;
              sample0Word = 0;
              sample1Default = 0;
              sample1Word = 0;
              sample2Word = 0;
              sample3Word = 0;
              sourceRow = (int)sourceV >> 0xc;
              sourceColumn = (int)sinTermOrSourceU >> 0xc;
              texelIndexOrFraction = sourceWidth * sourceRow + sourceColumn;
              clipHeightOrColumnTerm = sourceColumn + 1;
              if (sourceRow < sourceHeight) {
                sample0Word = sample0Load;
                sample1Word = sample1Default;
                if ((-1 < sourceRow) && (sourceColumn < sourceWidth)) {
                  if (-1 < sourceColumn) {
                    sample0Load = *(undefined4 *)
                              ((sourceTexture->common).buildMetadata.assetRelativeAddressAnchor28 +
                              texelIndexOrFraction * 4 + pixelDataOffset + -0x28);
                  }
                  sample0Word = sample0Load;
                  if ((-1 < clipHeightOrColumnTerm) && (clipHeightOrColumnTerm < sourceWidth)) {
                    sample1Word = *(undefined4 *)
                              ((sourceTexture->common).buildMetadata.assetRelativeAddressAnchor28 +
                              texelIndexOrFraction * 4 + pixelDataOffset + -0x24);
                  }
                }
                if (((-1 < sourceRow + 1) && (sourceRow + 1 < sourceHeight)) && (sourceColumn < sourceWidth)) {
                  sample2Word = 0;
                  if (-1 < sourceColumn) {
                    sample2Word = *(undefined4 *)
                              ((sourceTexture->common).buildMetadata.assetRelativeAddressAnchor28 +
                              (texelIndexOrFraction + sourceWidth) * 4 + pixelDataOffset + -0x28);
                  }
                  if ((-1 < clipHeightOrColumnTerm) && (clipHeightOrColumnTerm < sourceWidth)) {
                    sample3Word = *(undefined4 *)
                              ((sourceTexture->common).buildMetadata.assetRelativeAddressAnchor28 +
                              (texelIndexOrFraction + sourceWidth) * 4 + pixelDataOffset + -0x24);
                  }
                }
              }
              channelByte = (undefined1)((uint)sample0Word >> 0x18);
              lane0Word = CONCAT11(channelByte,channelByte);
              sample0Byte2 = (undefined1)((uint)sample0Word >> 0x10);
              sample0Byte1 = (undefined1)((uint)sample0Word >> 8);
              channelByte = (undefined1)((uint)sample1Word >> 0x18);
              lane1Word = CONCAT11(channelByte,channelByte);
              sample1Byte2 = (undefined1)((uint)sample1Word >> 0x10);
              sample1Byte1 = (undefined1)((uint)sample1Word >> 8);
              channelByte = (undefined1)((uint)sample2Word >> 0x18);
              lane2Word = CONCAT11(channelByte,channelByte);
              sample2Byte2 = (undefined1)((uint)sample2Word >> 0x10);
              sample2Byte1 = (undefined1)((uint)sample2Word >> 8);
              channelByte = (undefined1)((uint)sample3Word >> 0x18);
              lane3Word = CONCAT11(channelByte,channelByte);
              sample3Byte2 = (undefined1)((uint)sample3Word >> 0x10);
              channelByte = (undefined1)((uint)sample3Word >> 8);
              texelIndexOrFraction = (int)(sourceV & 0xfff) >> 4;
              clipHeightOrColumnTerm = (int)(sinTermOrSourceU & 0xfff) >> 4;
              sample0Weighted = pmulhw(CONCAT26(lane0Word >> 2,
                                       CONCAT24((ushort)CONCAT31(CONCAT21(lane0Word,sample0Byte2),sample0Byte2) >>
                                                2,CONCAT22(CONCAT11(sample0Byte1,sample0Byte1) >> 2,
                                                           CONCAT11((char)sample0Word,(char)sample0Word) >> 2)
                                               )),*(undefined8 *)(clipHeightOrColumnTerm * 8 + THANDOR_ADDR(g_UiScalerFirstPixelWeights,0)));
              sample1WeightedOrRow0Blend = pmulhw(CONCAT26(lane1Word >> 2,
                                       CONCAT24((ushort)CONCAT31(CONCAT21(lane1Word,sample1Byte2),sample1Byte2) >>
                                                2,CONCAT22(CONCAT11(sample1Byte1,sample1Byte1) >> 2,
                                                           CONCAT11((char)sample1Word,(char)sample1Word) >> 2)
                                               )),*(undefined8 *)(clipHeightOrColumnTerm * 8 + THANDOR_ADDR(g_UiScalerSecondPixelWeights,0)));
              row0BlendOrSample2 = pmulhw(CONCAT26(lane2Word >> 2,
                                       CONCAT24((ushort)CONCAT31(CONCAT21(lane2Word,sample2Byte2),sample2Byte2) >>
                                                2,CONCAT22(CONCAT11(sample2Byte1,sample2Byte1) >> 2,
                                                           CONCAT11((char)sample2Word,(char)sample2Word) >> 2)
                                               )),*(undefined8 *)(clipHeightOrColumnTerm * 8 + THANDOR_ADDR(g_UiScalerFirstPixelWeights,0)));
              row1BlendOrPacked = pmulhw(CONCAT26(lane3Word >> 2,
                                       CONCAT24((ushort)CONCAT31(CONCAT21(lane3Word,sample3Byte2),sample3Byte2) >>
                                                2,CONCAT22(CONCAT11(channelByte,channelByte) >> 2,
                                                           CONCAT11((char)sample3Word,(char)sample3Word) >> 2)
                                               )),*(undefined8 *)(clipHeightOrColumnTerm * 8 + THANDOR_ADDR(g_UiScalerSecondPixelWeights,0)));
              sample1WeightedOrRow0Blend = pmulhw(CONCAT26((short)((ulonglong)sample0Weighted >> 0x30) +
                                       (short)((ulonglong)sample1WeightedOrRow0Blend >> 0x30),
                                       CONCAT24((short)((ulonglong)sample0Weighted >> 0x20) +
                                                (short)((ulonglong)sample1WeightedOrRow0Blend >> 0x20),
                                                CONCAT22((short)((ulonglong)sample0Weighted >> 0x10) +
                                                         (short)((ulonglong)sample1WeightedOrRow0Blend >> 0x10),
                                                         (short)sample0Weighted + (short)sample1WeightedOrRow0Blend))),
                              *(undefined8 *)(texelIndexOrFraction * 8 + THANDOR_ADDR(g_UiScalerFirstPixelWeights,0)));
              row1BlendOrPacked = pmulhw(CONCAT26((short)((ulonglong)row0BlendOrSample2 >> 0x30) +
                                       (short)((ulonglong)row1BlendOrPacked >> 0x30),
                                       CONCAT24((short)((ulonglong)row0BlendOrSample2 >> 0x20) +
                                                (short)((ulonglong)row1BlendOrPacked >> 0x20),
                                                CONCAT22((short)((ulonglong)row0BlendOrSample2 >> 0x10) +
                                                         (short)((ulonglong)row1BlendOrPacked >> 0x10),
                                                         (short)row0BlendOrSample2 + (short)row1BlendOrPacked))),
                              *(undefined8 *)(texelIndexOrFraction * 8 + THANDOR_ADDR(g_UiScalerSecondPixelWeights,0)));
              lane0Word = (ushort)((short)sample1WeightedOrRow0Blend + (short)row1BlendOrPacked) >> 2;
              lane1Word = (ushort)((short)((ulonglong)sample1WeightedOrRow0Blend >> 0x10) +
                               (short)((ulonglong)row1BlendOrPacked >> 0x10)) >> 2;
              lane2Word = (ushort)((short)((ulonglong)sample1WeightedOrRow0Blend >> 0x20) +
                               (short)((ulonglong)row1BlendOrPacked >> 0x20)) >> 2;
              lane3Word = (ushort)((short)((ulonglong)sample1WeightedOrRow0Blend >> 0x30) +
                               (short)((ulonglong)row1BlendOrPacked >> 0x30)) >> 2;
              *(uint *)destPixel =
                   CONCAT13((lane3Word != 0) * (lane3Word < 0x100) * (char)lane3Word - (0xff < lane3Word),
                            CONCAT12((lane2Word != 0) * (lane2Word < 0x100) * (char)lane2Word -
                                     (0xff < lane2Word),
                                     CONCAT11((lane1Word != 0) * (lane1Word < 0x100) * (char)lane1Word -
                                              (0xff < lane1Word),
                                              (lane0Word != 0) * (lane0Word < 0x100) * (char)lane0Word -
                                              (0xff < lane0Word))));
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
        (*g_GraphicsFramebufferEndAccess)();
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
  longlong rotationProductA;
  longlong rotationProductB;
  longlong rotationProductC;
  uint sinTermOrStepY;
  uint stepTermX;
  
  rotationProductA = (longlong)control->sampleScaleQ12 * (longlong)g_FixedCosQ28[control->rotationAngle];
  cosTermOrBoundsLeft = -((int)((ulonglong)rotationProductA >> 0x20) << 4 | (uint)rotationProductA >> 0x1c);
  rotationProductA = (longlong)control->sampleScaleQ12 * (longlong)g_FixedSinQ28[control->rotationAngle];
  sinTermOrStepY = (int)((ulonglong)rotationProductA >> 0x20) << 4 | (uint)rotationProductA >> 0x1c;
  rotationProductA = (longlong)(int)sinTermOrStepY * 0x1c6e9c;
  rotationProductB = (longlong)cosTermOrBoundsLeft * -0x20c8cc;
  stepTermX = (int)((ulonglong)rotationProductB >> 0x20) << 0xb | (uint)rotationProductB >> 0x15;
  rotationProductB = (longlong)cosTermOrBoundsLeft * 0x1c6e9c;
  rotationProductC = (longlong)(int)-sinTermOrStepY * -0x20c8cc;
  sinTermOrStepY = (int)((ulonglong)rotationProductC >> 0x20) << 0xb | (uint)rotationProductC >> 0x15;
  cosTermOrBoundsLeft = (control->base).left;
  boundsRight = (control->base).right;
  boundsTop = (control->base).top;
  boundsBottom = (control->base).bottom;
  control->selectedSourceYQ12 =
       control->sourceOriginYQ12 -
       ((((int)((ulonglong)rotationProductB >> 0x20) << 0xc | (uint)rotationProductB >> 0x14) - sinTermOrStepY) *
        (((control->base).top + (control->base).bottom >> 1) - pointerY) +
       (((int)((ulonglong)rotationProductA >> 0x20) << 0xc | (uint)rotationProductA >> 0x14) - stepTermX) *
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
  if (g_UiPointerCaptureButton == UI_POINTER_CAPTURE_NONE) {
    root = topRoot;
    if (g_UiImageControlHoverTarget != (UiImageControl *)0x0) {
      opaqueHit = UiImageControl_HitTestOpaque(pointerY,pointerX,g_UiImageControlHoverTarget);
      g_UiImageControlHoverTarget = (UiImageControl *)0x0;
      if (opaqueHit != (UiNodeBase *)0xffffffff) {
UiPointer_DispatchLeftPressToCapturedTarget:
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
        (*nodeVtable->nonRightPress)(wheelDelta,pointerY,pointerX,(UiNodeBase *)node);
        if (g_UiPointerCaptureTarget == (UiNodeBase *)0xffffffff) {
          return;
        }
        (*g_UiPointerCaptureTarget->vtable->nonRightDrag)
                  (wheelDelta,pointerY,pointerX,g_UiPointerCaptureTarget);
        return;
      }
      stateFlagsField = &(node->selectable).stateFlags;
      *stateFlagsField = *stateFlagsField & 0xfffff9fc;
    }
    while (root != (UiRootNode *)0xffffffff) {
      if (((((root->rootFlags & UI_ROOT_DISABLE_POINTER_HIT_TEST) == 0) &&
           ((root->base).left <= pointerX)) && ((root->base).top <= pointerY)) &&
         ((pointerX < (root->base).right && (pointerY < (root->base).bottom)))) {
        node = (UiImageControl *)(*((root->base).vtable)->hitTest)(pointerY,pointerX,&root->base);
        if ((root != topRoot) && (handled = UiRootStack_BringToFront(root), handled)) {
          return;
        }
        if (node == (UiImageControl *)0xffffffff) {
          return;
        }
        goto UiPointer_DispatchLeftPressToCapturedTarget;
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
  if (g_UiPointerCaptureButton == UI_POINTER_CAPTURE_NONE) {
    root = topRoot;
    if (g_UiImageControlHoverTarget != (UiImageControl *)0x0) {
      opaqueHit = UiImageControl_HitTestOpaque(pointerY,pointerX,g_UiImageControlHoverTarget);
      g_UiImageControlHoverTarget = (UiImageControl *)0x0;
      if (opaqueHit != (UiNodeBase *)0xffffffff) {
UiPointer_DispatchMiddlePressToCapturedTarget:
        g_UiPointerCaptureButton = UI_POINTER_CAPTURE_MIDDLE;
        nodeFlagsField = &(node->selectable).base.nodeFlags;
        *nodeFlagsField = *nodeFlagsField | UI_NODE_REPEAT_OR_DOUBLE_CLICK;
        nodeVtable = (node->selectable).base.vtable;
        g_UiPointerCaptureTarget = (UiNodeBase *)node;
        if (((node->selectable).base.nodeFlags &
            (UI_NODE_FALLBACK_FOCUS_TARGET|UI_NODE_PREFERRED_FOCUS_TARGET)) != 0) {
          UiKeyboardFocus_Set((UiNodeBase *)node);
        }
        (*nodeVtable->nonRightPress)(wheelDelta,pointerY,pointerX,(UiNodeBase *)node);
        if (g_UiPointerCaptureTarget == (UiNodeBase *)0xffffffff) {
          return;
        }
        (*g_UiPointerCaptureTarget->vtable->nonRightDrag)
                  (wheelDelta,pointerY,pointerX,g_UiPointerCaptureTarget);
        return;
      }
      stateFlagsField = &(node->selectable).stateFlags;
      *stateFlagsField = *stateFlagsField & 0xfffff9fc;
    }
    while (root != (UiRootNode *)0xffffffff) {
      if (((((root->rootFlags & UI_ROOT_DISABLE_POINTER_HIT_TEST) == 0) &&
           ((root->base).left <= pointerX)) && ((root->base).top <= pointerY)) &&
         ((pointerX < (root->base).right && (pointerY < (root->base).bottom)))) {
        node = (UiImageControl *)(*((root->base).vtable)->hitTest)(pointerY,pointerX,&root->base);
        if ((root != topRoot) && (handled = UiRootStack_BringToFront(root), handled)) {
          return;
        }
        if (node == (UiImageControl *)0xffffffff) {
          return;
        }
        goto UiPointer_DispatchMiddlePressToCapturedTarget;
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
        (*nodeVtable->rightPress)(wheelDelta,pointerY,pointerX,node);
        if (g_UiPointerCaptureTarget == (UiNodeBase *)0xffffffff) {
          return;
        }
        (*g_UiPointerCaptureTarget->vtable->rightDrag)
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
  UiNodeBase *traversalNode;
  UiNodeFlags candidateNodeFlags;
  UiNodeBase *firstChildNode;
  
  node = g_UiKeyboardFocusNode;
  if (g_UiKeyboardFocusNode != (UiNodeBase *)0xffffffff) {
    do {
      do {
        firstChildNode = node->firstChild;
        if (firstChildNode == (UiNodeBase *)0xffffffff) {
          do {
            traversalNode = node;
            node = traversalNode->nextSibling;
            if (node != (UiNodeBase *)0xffffffff) {
              candidateNodeFlags = node->nodeFlags;
              goto joined_r0x004affea;
            }
            node = traversalNode->parent;
          } while (traversalNode->parent != (UiNodeBase *)0xffffffff);
          candidateNodeFlags = traversalNode->nodeFlags;
          node = traversalNode;
        }
        else {
          candidateNodeFlags = firstChildNode->nodeFlags;
          node = firstChildNode;
        }
joined_r0x004affea:;
      } while ((candidateNodeFlags & (UI_NODE_FALLBACK_FOCUS_TARGET|UI_NODE_PREFERRED_FOCUS_TARGET))
               == 0);
      if (node == g_UiKeyboardFocusNode) {
        return;
      }
    } while ((node->nodeFlags & UI_NODE_SUPPRESSED) != 0);
    UiKeyboardFocus_Set(node);
  }
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
      (*targetNode->vtable->rightDrag)(wheelDelta,pointerY,pointerX,targetNode);
    }
    else {
      (*targetNode->vtable->nonRightDrag)(wheelDelta,pointerY,pointerX,targetNode);
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
      (*nodeVtable->pointerMove)(pointerY,pointerX,targetNode);
      if (wheelDelta == 0) {
        return;
      }
      (*nodeVtable->pointerWheel)(wheelDelta,pointerY,pointerX,targetNode);
      return;
    }
    missPolicy = root->callbacks->pointerMissPolicy;
    if (missPolicy == (UiRootPointerMissPolicyCallback *)0x0) {
      return;
    }
    missPolicyResult = (*missPolicy)(root);
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
    (*parentControl->vtable->pointerWheel)(wheelDelta,pointerY,pointerX,parentControl);
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
UiNode_DefaultKeyboardEventMoveFocusNextCf
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


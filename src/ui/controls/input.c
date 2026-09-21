/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/src/ui/controls/input.c
 * Reverse engineering by idkFoxes 2026
 */

#include <thandor/ui/controls/input.h>

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
  char cVar1;
  UiPixelCoordinate pointerX;
  UiPixelCoordinate pointerY;
  GraphicsCursorButtonState buttonMask;
  UiPointerWheelDelta wheelDelta;
  GraphicsCursorInputEventRegsCf21 GVar2;
  
  (*g_SpinLockAcquire)(g_UiRuntimeFrameLock);
  if (g_PointerSetPosition == DirectInputMouse_SetPosition) {
    DirectInputMouse_PollBufferedEvents();
  }
  while( true ) {
    control = g_UiPointerCaptureTarget;
    GVar2 = (*g_GraphicsCursorConsumeEvent)();
    pointerX = GVar2.pointerX;
    wheelDelta = GVar2.wheelDelta;
    pointerY = GVar2.pointerY;
    buttonMask = GVar2.buttonState;
    if (GVar2.carry) break;
    cVar1 = (char)GVar2.eventCode;
    if (cVar1 < '\a') {
      if (cVar1 == '\x06') {
        if ((control != (UiNodeBase *)0xffffffff) &&
           (g_UiPointerCaptureButton == UI_POINTER_CAPTURE_MIDDLE)) {
          (*control->vtable->nonRightRelease)(wheelDelta,pointerY,pointerX,control);
          g_UiPointerCaptureButton = UI_POINTER_CAPTURE_NONE;
          g_UiPointerCaptureTarget = (UiNodeBase *)0xffffffff;
          UiPointer_DispatchMotionAndWheel(wheelDelta,pointerY,pointerX);
          Random_NextPrimary();
        }
      }
      else if (cVar1 < '\x04') {
        if (cVar1 == '\x03') {
          UiPointer_DispatchRightPress(buttonMask,wheelDelta,pointerY,pointerX);
        }
        else if (cVar1 < '\x02') {
          if (cVar1 == '\x01') {
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
  UiNodeFlags UVar1;
  bool bVar2;
  UiKeyboardEventCode keyCode;
  UiKeyboardStateMask keyboardStateMask;
  UiNodeBase *control;
  UiNodeBase *pUVar3;
  bool bVar4;
  KeyboardEventEaxEdxCf9 KVar5;
  
  (*g_SpinLockAcquire)(g_UiRuntimeFrameLock);
  do {
    do {
      while( true ) {
        do {
          KVar5 = (*g_KeyboardReadEvent)();
          control = g_UiKeyboardFocusNode;
          keyboardStateMask = KVar5.eventData;
          keyCode = KVar5.eventCode;
          if (KVar5.carry) {
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
      bVar2 = false;
      bVar4 = (*g_UiKeyboardFocusNode->vtable->keyboardEventCf)
                        (keyboardStateMask,keyCode,g_UiKeyboardFocusNode);
    } while (!bVar4);
    do {
      do {
        pUVar3 = control->firstChild;
        if (pUVar3 == (UiNodeBase *)0xffffffff) {
          do {
            pUVar3 = control;
            control = pUVar3->nextSibling;
            if (control != (UiNodeBase *)0xffffffff) {
              UVar1 = control->nodeFlags;
              goto joined_r0x004af45a;
            }
            control = pUVar3->parent;
          } while (pUVar3->parent != (UiNodeBase *)0xffffffff);
          if (bVar2) goto UiKeyboard_DispatchEventToRootFallback;
          bVar2 = true;
          UVar1 = pUVar3->nodeFlags;
          control = pUVar3;
        }
        else {
          UVar1 = pUVar3->nodeFlags;
          control = pUVar3;
        }
joined_r0x004af45a:
      } while ((UVar1 & (UI_NODE_FALLBACK_FOCUS_TARGET|UI_NODE_PREFERRED_FOCUS_TARGET)) == 0);
      if (control == g_UiKeyboardFocusNode) goto UiKeyboard_DispatchEventToRootFallback;
    } while (((control->nodeFlags & UI_NODE_SUPPRESSED) != 0) ||
            (bVar4 = (*control->vtable->keyboardEventCf)(keyboardStateMask,keyCode,control), bVar4))
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
          (UiKeyboardStateMask keyboardStateMask,UiKeyboardEventCode keyCode,UiNodeBase *control)

{
  UiNodeBase *increasedSliderValue;
  UiNodeBase *adjustedSliderValue;
  bool bVar1;
  
  if ((control->nodeFlags & UI_NODE_SUPPRESSED) != 0) {
UiRangeSliderControl_DelegateUnhandledKeyboardEvent:
    bVar1 = UiNode_DefaultKeyboardEventMoveFocusNextCf(keyboardStateMask,keyCode,control);
    return bVar1;
  }
  if (((uint)control[1].nextSibling & 1) == 0) {
    if (keyCode == 0x10014) {
UiRangeSliderControl_DecreaseValueAndNotify:
      if (((keyboardStateMask & 0xc) != 0) ||
         (adjustedSliderValue = (UiNodeBase *)((int)control[1].vtable - control[1].left),
         (int)adjustedSliderValue < (int)control[1].firstChild)) {
        adjustedSliderValue = control[1].firstChild;
      }
      control[1].vtable = (UiNodeVtable *)adjustedSliderValue;
      if ((((uint)control[1].nextSibling & 4) != 0) && (control[1].right != 0)) {
        (*g_SoundPlayOneShot)
                  (g_UiSoundGainQ15,g_UiSoundGainQ15,(DirectSoundVoiceSet *)control[1].right);
      }
      UiActionQueue_Enqueue(control[1].top,control);
      UiNode_InvalidateRoot(control);
      return false;
    }
    if (keyCode != 0x10016) goto UiRangeSliderControl_DelegateUnhandledKeyboardEvent;
  }
  else if (keyCode != 0x10011) {
    if (keyCode == 0x10019) goto UiRangeSliderControl_DecreaseValueAndNotify;
    goto UiRangeSliderControl_DelegateUnhandledKeyboardEvent;
  }
  if (((keyboardStateMask & 0xc) != 0) ||
     (increasedSliderValue = (UiNodeBase *)((int)&(control[1].vtable)->relocate + control[1].left),
     (int)control[1].parent < (int)increasedSliderValue)) {
    increasedSliderValue = control[1].parent;
  }
  control[1].vtable = (UiNodeVtable *)increasedSliderValue;
  if ((((uint)control[1].nextSibling & 4) != 0) && (control[1].right != 0)) {
    (*g_SoundPlayOneShot)(g_UiSoundGainQ15,g_UiSoundGainQ15,(DirectSoundVoiceSet *)control[1].right)
    ;
  }
  UiActionQueue_Enqueue(control[1].top,control);
  UiNode_InvalidateRoot(control);
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
          (UiKeyboardStateMask keyboardStateMask,UiKeyboardEventCode keyCode,UiNodeBase *control)

{
  UiNodeBase *control_00;
  bool childHandledEvent;
  bool bVar1;
  
  control_00 = control[1].firstChild;
  if ((keyCode & 0xffff0000) == 0) {
    if ((((uint)control[1].nextSibling & 0x8000) != 0) && ((keyCode & 0x30) != 0)) {
      return false;
    }
  }
  else if (keyCode == 0x10002) {
    bVar1 = UiNode_DefaultKeyboardEventMoveFocusNextCf(keyboardStateMask,0x10002,control);
    return bVar1;
  }
  if (control_00 != (UiNodeBase *)0x0) {
    bVar1 = (*control_00->vtable->keyboardEventCf)(keyboardStateMask,keyCode,control_00);
    if (!bVar1) {
      UiNode_InvalidateRoot(control);
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
          UiNodeBase *control)

{
  UiNodeBase *control_00;
  
  if (((uint)control[1].nextSibling & 0x400) == 0) {
    control_00 = control[1].firstChild;
    control[1].nextSibling = (UiNodeBase *)((uint)control[1].nextSibling | 0x400);
    if (control_00 != (UiNodeBase *)0x0) {
      if (control == g_UiKeyboardFocusNode) {
        g_UiKeyboardFocusNode = control_00;
        control_00->nodeFlags = control_00->nodeFlags | UI_NODE_HAS_KEYBOARD_FOCUS;
      }
      (*control_00->vtable->pointerWheel)(wheelDelta,pointerY,pointerX,control_00);
      if (control_00 == g_UiKeyboardFocusNode) {
        g_UiKeyboardFocusNode = control;
        control_00->nodeFlags = control_00->nodeFlags & ~UI_NODE_HAS_KEYBOARD_FOCUS;
      }
      UiNode_InvalidateRoot(control);
    }
    control[1].nextSibling = (UiNodeBase *)((uint)control[1].nextSibling & 0xfffffbff);
    return;
  }
  UiNode_ForwardPointerWheelToParent(wheelDelta,pointerY,pointerX,control);
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
          UiNodeBase *control)

{
  ulonglong uVar1;
  uint uVar2;
  UiNodeBase *pUVar3;
  uint uVar4;
  GraphicsTextureSizeEaxEdxCf9 GVar5;
  
  if (((uint)control[1].nextSibling & 2) != 0) {
    if (((uint)control[1].nextSibling & 1) == 0) {
      GVar5 = (*g_GraphicsTextureSourceGetLogicalSize)(0xaf,g_UiWindowTextureSource);
      uVar4 = control->layoutWidth - GVar5.logicalWidthPixels;
      if (uVar4 == 0) {
        uVar4 = 1;
      }
      uVar2 = (pointerX - ((int)GVar5.logicalWidthPixels >> 1)) - control->left;
      if ((int)uVar2 < 0) {
        uVar2 = 0;
      }
      uVar1 = (ulonglong)uVar2 *
              (ulonglong)(uint)((int)control[1].parent - (int)control[1].firstChild);
      pUVar3 = (UiNodeBase *)
               ((int)&(control[1].firstChild)->nextSibling +
               (uint)(uVar4 < (uint)((int)(uVar1 % (ulonglong)uVar4) * 2)) + (int)(uVar1 / uVar4));
      if ((int)control[1].parent < (int)pUVar3) {
        pUVar3 = control[1].parent;
      }
      if (((uint)control[1].nextSibling & 8) != 0) {
        pUVar3 = (UiNodeBase *)((int)control[1].parent - ((int)pUVar3 - (int)control[1].firstChild))
        ;
      }
      control[1].vtable = (UiNodeVtable *)pUVar3;
      UiActionQueue_Enqueue(control[1].top,control);
      UiNode_InvalidateRoot(control);
      return;
    }
    GVar5 = (*g_GraphicsTextureSourceGetLogicalSize)(0xb7,g_UiWindowTextureSource);
    uVar4 = control->layoutHeight - GVar5.logicalHeightPixels;
    if (uVar4 == 0) {
      uVar4 = 1;
    }
    uVar2 = (control->bottom - pointerY) - ((int)GVar5.logicalHeightPixels >> 1);
    if ((int)uVar2 < 0) {
      uVar2 = 0;
    }
    uVar1 = (ulonglong)uVar2 *
            (ulonglong)(uint)((int)control[1].parent - (int)control[1].firstChild);
    pUVar3 = (UiNodeBase *)
             ((int)&(control[1].firstChild)->nextSibling +
             (uint)(uVar4 < (uint)((int)(uVar1 % (ulonglong)uVar4) * 2)) + (int)(uVar1 / uVar4));
    if ((int)control[1].parent < (int)pUVar3) {
      pUVar3 = control[1].parent;
    }
    if (((uint)control[1].nextSibling & 8) != 0) {
      pUVar3 = (UiNodeBase *)((int)control[1].parent - ((int)pUVar3 - (int)control[1].firstChild));
    }
    control[1].vtable = (UiNodeVtable *)pUVar3;
    UiActionQueue_Enqueue(control[1].top,control);
    UiNode_InvalidateRoot(control);
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
          UiNodeBase *control)

{
  UiNodeBase *adjustedSliderValue;
  
  if (((((uint)control[1].nextSibling & 2) == 0) && ((control->nodeFlags & UI_NODE_SUPPRESSED) == 0)
      ) && (wheelDelta != 0)) {
    adjustedSliderValue =
         (UiNodeBase *)
         ((int)&(control[1].vtable)->relocate +
         wheelDelta * g_UiRangeSliderDragScale * control[1].left);
    if ((int)adjustedSliderValue < (int)control[1].firstChild) {
      adjustedSliderValue = control[1].firstChild;
    }
    if ((int)control[1].parent < (int)adjustedSliderValue) {
      adjustedSliderValue = control[1].parent;
    }
    control[1].vtable = (UiNodeVtable *)adjustedSliderValue;
    UiActionQueue_Enqueue(control[1].top,control);
    UiNode_InvalidateRoot(control);
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
          (UiSerializedRelocationDelta relocationDelta,UiNodeBase *control,UiNodeBase *controlReg)

{
  UiNodeFlags *childNodeFlagsField;
  
  if (((controlReg->nodeFlags & (UI_NODE_FALLBACK_FOCUS_TARGET|UI_NODE_PREFERRED_FOCUS_TARGET)) == 0
      ) && (controlReg[1].firstChild != (UiNodeBase *)0x0)) {
    controlReg->nodeFlags = controlReg->nodeFlags | UI_NODE_FALLBACK_FOCUS_TARGET;
  }
  UiContainer_RelocateChildren(relocationDelta,controlReg);
  if (controlReg[1].firstChild != (UiNodeBase *)0x0) {
    controlReg[1].firstChild =
         (UiNodeBase *)((int)&(controlReg[1].firstChild)->nextSibling + relocationDelta);
    childNodeFlagsField = &(controlReg[1].firstChild)->nodeFlags;
    *childNodeFlagsField =
         *childNodeFlagsField & ~(UI_NODE_FALLBACK_FOCUS_TARGET|UI_NODE_PREFERRED_FOCUS_TARGET);
  }
  if (((uint)control[1].nextSibling & 0x20) != 0) {
    control[1].parent = (UiNodeBase *)((int)&(control[1].parent)->nextSibling + relocationDelta);
    control[1].nextSibling = (UiNodeBase *)((uint)control[1].nextSibling & 0xffffffdf);
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
          UiNodeBase *control)

{
  UiNodeBase *control_00;
  
  control_00 = control[1].firstChild;
  if (control_00 != (UiNodeBase *)0x0) {
    if (control == g_UiKeyboardFocusNode) {
      g_UiKeyboardFocusNode = control_00;
      control_00->nodeFlags = control_00->nodeFlags | UI_NODE_HAS_KEYBOARD_FOCUS;
    }
    (*control_00->vtable->nonRightPress)(wheelDelta,pointerY,pointerX,control_00);
    if (control_00 == g_UiKeyboardFocusNode) {
      g_UiKeyboardFocusNode = control;
      control_00->nodeFlags = control_00->nodeFlags & ~UI_NODE_HAS_KEYBOARD_FOCUS;
    }
    UiNode_InvalidateRoot(control);
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
          UiNodeBase *control)

{
  UiNodeBase *control_00;
  
  control_00 = control[1].firstChild;
  if (control_00 != (UiNodeBase *)0x0) {
    if (control == g_UiKeyboardFocusNode) {
      g_UiKeyboardFocusNode = control_00;
      control_00->nodeFlags = control_00->nodeFlags | UI_NODE_HAS_KEYBOARD_FOCUS;
    }
    (*control_00->vtable->nonRightRelease)(wheelDelta,pointerY,pointerX,control_00);
    if (control_00 == g_UiKeyboardFocusNode) {
      g_UiKeyboardFocusNode = control;
      control_00->nodeFlags = control_00->nodeFlags & ~UI_NODE_HAS_KEYBOARD_FOCUS;
    }
    UiNode_InvalidateRoot(control);
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
          UiNodeBase *control)

{
  UiNodeBase *control_00;
  
  control_00 = control[1].firstChild;
  if (control_00 != (UiNodeBase *)0x0) {
    if (control == g_UiKeyboardFocusNode) {
      g_UiKeyboardFocusNode = control_00;
      control_00->nodeFlags = control_00->nodeFlags | UI_NODE_HAS_KEYBOARD_FOCUS;
    }
    if (control != control_00->parent) {
      (*control_00->vtable->rightPress)(wheelDelta,pointerY,pointerX,control_00);
    }
    if (control_00 == g_UiKeyboardFocusNode) {
      g_UiKeyboardFocusNode = control;
      control_00->nodeFlags = control_00->nodeFlags & ~UI_NODE_HAS_KEYBOARD_FOCUS;
    }
    UiNode_InvalidateRoot(control);
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
          UiNodeBase *control)

{
  UiNodeBase *control_00;
  
  control_00 = control[1].firstChild;
  if (control_00 != (UiNodeBase *)0x0) {
    if (control == g_UiKeyboardFocusNode) {
      g_UiKeyboardFocusNode = control_00;
      control_00->nodeFlags = control_00->nodeFlags | UI_NODE_HAS_KEYBOARD_FOCUS;
    }
    (*control_00->vtable->rightRelease)(wheelDelta,pointerY,pointerX,control_00);
    if (control_00 == g_UiKeyboardFocusNode) {
      g_UiKeyboardFocusNode = control;
      control_00->nodeFlags = control_00->nodeFlags & ~UI_NODE_HAS_KEYBOARD_FOCUS;
    }
    UiNode_InvalidateRoot(control);
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
          UiNodeBase *control)

{
  UiNodeBase *control_00;
  
  control_00 = control[1].firstChild;
  if (control_00 != (UiNodeBase *)0x0) {
    if (control == g_UiKeyboardFocusNode) {
      g_UiKeyboardFocusNode = control_00;
      control_00->nodeFlags = control_00->nodeFlags | UI_NODE_HAS_KEYBOARD_FOCUS;
    }
    (*control_00->vtable->nonRightDrag)(wheelDelta,pointerY,pointerX,control_00);
    if (control_00 == g_UiKeyboardFocusNode) {
      g_UiKeyboardFocusNode = control;
      control_00->nodeFlags = control_00->nodeFlags & ~UI_NODE_HAS_KEYBOARD_FOCUS;
    }
    UiNode_InvalidateRoot(control);
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
          UiNodeBase *control)

{
  UiNodeBase *control_00;
  
  control_00 = control[1].firstChild;
  if (control_00 != (UiNodeBase *)0x0) {
    if (control == g_UiKeyboardFocusNode) {
      g_UiKeyboardFocusNode = control_00;
      control_00->nodeFlags = control_00->nodeFlags | UI_NODE_HAS_KEYBOARD_FOCUS;
    }
    (*control_00->vtable->rightDrag)(wheelDelta,pointerY,pointerX,control_00);
    if (control_00 == g_UiKeyboardFocusNode) {
      g_UiKeyboardFocusNode = control;
      control_00->nodeFlags = control_00->nodeFlags & ~UI_NODE_HAS_KEYBOARD_FOCUS;
    }
    UiNode_InvalidateRoot(control);
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
          (UiPixelCoordinate pointerY,UiPixelCoordinate pointerX,UiNodeBase *control)

{
  UiNodeBase *control_00;
  GraphicsCursorFrameIndex GVar1;
  
  GVar1 = 0;
  control_00 = control[1].firstChild;
  if (control_00 != (UiNodeBase *)0x0) {
    if (control == g_UiKeyboardFocusNode) {
      g_UiKeyboardFocusNode = control_00;
      control_00->nodeFlags = control_00->nodeFlags | UI_NODE_HAS_KEYBOARD_FOCUS;
    }
    GVar1 = (*control_00->vtable->pointerMove)(pointerY,pointerX,control_00);
    if (control_00 == g_UiKeyboardFocusNode) {
      g_UiKeyboardFocusNode = control;
      control_00->nodeFlags = control_00->nodeFlags & ~UI_NODE_HAS_KEYBOARD_FOCUS;
    }
    UiNode_InvalidateRoot(control);
  }
  return GVar1;
}


/* Address: 0x004B9C50.
   Ownership: ui/controls/input.
   Purpose: Binary entry is anchored by g_UiNodeVtable_004B9530[11]@004B9530.
   Cross-module calls: UiContainer_HitTestChildren [ui/controls/layout].
*/
UiNodeBase * __thandor_eax_preserve_ecx_edx
UiFocusProxyControl_HitTestChildProxy
          (UiPixelCoordinate pointerY,UiPixelCoordinate pointerX,UiNodeBase *control)

{
  UiNodeBase *hitNode;
  UiNodeBase *returnedNode;
  
  hitNode = UiContainer_HitTestChildren(pointerY,pointerX,control);
  if (hitNode == control[1].firstChild) {
    returnedNode = control;
    if ((hitNode->nodeFlags & UI_NODE_SUPPRESSED) != 0) {
      returnedNode = (UiNodeBase *)0xffffffff;
    }
  }
  else {
    returnedNode = hitNode;
    if (((hitNode == control) && (control[1].firstChild != (UiNodeBase *)0x0)) &&
       (((control[1].firstChild)->nodeFlags & UI_NODE_SUPPRESSED) != 0)) {
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
void __thandor_preserve_eax_edx UiFocusProxyControl_ForwardTickToChild(UiNodeBase *control)

{
  UiNodeBase *control_00;
  
  control_00 = control[1].firstChild;
  if (control_00 != (UiNodeBase *)0x0) {
    if (control == g_UiKeyboardFocusNode) {
      g_UiKeyboardFocusNode = control_00;
      control_00->nodeFlags = control_00->nodeFlags | UI_NODE_HAS_KEYBOARD_FOCUS;
    }
    (*control_00->vtable->tick)(control_00);
    if (control_00 == g_UiKeyboardFocusNode) {
      g_UiKeyboardFocusNode = control;
      control_00->nodeFlags = control_00->nodeFlags & ~UI_NODE_HAS_KEYBOARD_FOCUS;
    }
    UiNode_InvalidateRoot(control);
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
  UiImageControl *control_00;
  GraphicsCursorFrameIndex GVar1;
  bool opaquePixelHit;
  bool bVar2;
  
  if (((control->selectable).base.nodeFlags & UI_NODE_SUPPRESSED) == 0) {
    if (((control->selectable).stateFlags & 0x40) == 0) {
      bVar2 = (*g_GraphicsTextureSourceTestOpaquePixel)
                        (pointerY,pointerX,(control->selectable).base.top,
                         (control->selectable).base.left,control->normalSubresource,
                         control->textureSource);
      if (bVar2) {
        return 0;
      }
    }
    else {
      bVar2 = (*g_GraphicsTextureSourceTestOpaquePixel)
                        (pointerY,pointerX,(control->selectable).base.top,
                         (control->selectable).base.left,control->alternateSubresource,
                         control->textureSource);
      if (bVar2) {
        return 0;
      }
    }
    if (((control->selectable).stateFlags & UI_SELECTABLE_PERSISTENT_ACTIVATION_MODE) != 0) {
      control_00 = (UiImageControl *)
                   UiContainer_HitTestChildren(pointerY,pointerX,(UiNodeBase *)control);
      if (control_00 != control) {
        GVar1 = (*((control_00->selectable).base.vtable)->pointerMove)
                          (pointerY,pointerX,(UiNodeBase *)control_00);
        return GVar1;
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
  GraphicsTextureSourceAsset *pGVar1;
  AssetRelativeOffset AVar2;
  int iVar3;
  int iVar4;
  int iVar5;
  longlong lVar6;
  longlong lVar7;
  longlong lVar8;
  char cVar9;
  int iVar13;
  uint uVar14;
  uint uVar15;
  uint uVar16;
  int iVar17;
  int iVar18;
  int iVar19;
  int iVar20;
  byte *pbVar21;
  bool bVar22;
  undefined8 extraout_MM0;
  int iVar23;
  undefined1 uVar25;
  undefined1 uVar26;
  int extraout_MM1_Da;
  PackedArgb32 sourcePixelSample3;
  undefined4 uVar24;
  undefined8 mm1PackedValue0;
  ulonglong uVar27;
  int iVar28;
  undefined1 uVar32;
  undefined1 uVar33;
  int iVar29;
  PackedArgb32 sourcePixelSample2;
  undefined4 uVar30;
  undefined8 mm3PackedValue0;
  undefined8 uVar31;
  ulonglong uVar34;
  uint uVar35;
  undefined1 uVar39;
  undefined1 uVar40;
  PackedArgb32 sourcePixelSample1;
  undefined4 uVar36;
  undefined8 mm5PackedValue0;
  undefined8 uVar38;
  ushort uVar41;
  undefined1 uVar47;
  PackedArgb32 sourcePixelSample0;
  PackedArgb32 sourcePixelSample4;
  ushort uVar48;
  undefined4 uVar42;
  undefined4 uVar43;
  undefined1 uVar46;
  ushort uVar49;
  undefined8 mm7PackedValue0;
  undefined8 uVar44;
  ushort uVar50;
  undefined8 uVar45;
  int iStack_2c;
  byte *pbStack_28;
  char cVar10;
  char cVar11;
  char cVar12;
  undefined4 uVar37;
  
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
  iVar19 = clipLeft - clipRight;
  if (((iVar19 != 0 && clipRight <= clipLeft) &&
      (iVar18 = clipTop - clipBottom, iVar18 != 0 && clipBottom <= clipTop)) &&
     (control->textureSource != (GraphicsTextureSourceAsset *)0x0)) {
    lVar6 = (longlong)control->sampleScaleQ12 * (longlong)g_FixedCosQ28[control->rotationAngle];
    iVar29 = -((int)((ulonglong)lVar6 >> 0x20) << 4 | (uint)lVar6 >> 0x1c);
    lVar6 = (longlong)control->sampleScaleQ12 * (longlong)g_FixedSinQ28[control->rotationAngle];
    uVar14 = (int)((ulonglong)lVar6 >> 0x20) << 4 | (uint)lVar6 >> 0x1c;
    lVar6 = (longlong)(int)uVar14 * 0x1c6e9c;
    lVar7 = (longlong)iVar29 * -0x20c8cc;
    uVar15 = (int)((ulonglong)lVar7 >> 0x20) << 0xb | (uint)lVar7 >> 0x15;
    lVar7 = (longlong)iVar29 * 0x1c6e9c;
    lVar8 = (longlong)(int)-uVar14 * -0x20c8cc;
    uVar16 = (int)((ulonglong)lVar8 >> 0x20) << 0xb | (uint)lVar8 >> 0x15;
    uVar14 = ((int)((ulonglong)lVar7 >> 0x20) << 0xc | (uint)lVar7 >> 0x14) - uVar16;
    iVar29 = uVar16 * 2;
    uVar27 = (ulonglong)uVar14;
    uVar34 = (ulonglong)
             (control->sourceOriginYQ12 -
             (uVar14 * (((control->base).top + (control->base).bottom >> 1) - clipBottom) +
             (((int)((ulonglong)lVar6 >> 0x20) << 0xc | (uint)lVar6 >> 0x14) - uVar15) *
             (((control->base).left + (control->base).right >> 1) - clipRight)));
    iVar20 = control->sourceOriginXQ12 -
             (iVar29 * (((control->base).top + (control->base).bottom >> 1) - clipBottom) +
             uVar15 * 2 * (((control->base).left + (control->base).right >> 1) - clipRight));
    pGVar1 = control->textureSource;
    AVar2 = (pGVar1->tableDescriptor).subresourceTableOffset;
    if (*(int *)((pGVar1->common).buildMetadata.assetRelativeAddressAnchor28 + (AVar2 - 0x20)) < 0)
    {
      iVar3 = *(int *)((pGVar1->common).buildMetadata.assetRelativeAddressAnchor28 + (AVar2 - 0x10))
      ;
      iVar4 = *(int *)((pGVar1->common).buildMetadata.assetRelativeAddressAnchor28 + (AVar2 - 0xc));
      iVar5 = *(int *)((pGVar1->common).buildMetadata.assetRelativeAddressAnchor28 + (AVar2 - 0x1c))
      ;
      bVar22 = (*g_GraphicsFramebufferBeginAccess)();
      if (!bVar22) {
        iVar23 = (int)((ulonglong)extraout_MM0 >> 0x20);
        iVar28 = (int)(uVar27 >> 0x20);
        uVar14 = (uint)uVar34;
        iVar13 = (int)(uVar34 >> 0x20);
        clipTop = iVar18;
        iStack_2c = iVar19;
        if (g_FramebufferAccess->bytesPerPixel == SOFTWARE_FRAMEBUFFER_PIXEL_BYTES_16BIT) {
          pbVar21 = g_FramebufferAccess->pixels +
                    g_FramebufferRowStrideBytes * clipBottom + clipRight * 2;
          uVar35 = iVar13 + iVar20;
          uVar15 = uVar14;
          uVar16 = uVar35;
          pbStack_28 = pbVar21;
          do {
            do {
              sourcePixelSample0 = 0;
              sourcePixelSample0 = 0;
              sourcePixelSample1 = 0;
              sourcePixelSample1 = 0;
              sourcePixelSample2 = 0;
              sourcePixelSample2 = 0;
              sourcePixelSample3 = 0;
              iVar17 = (int)uVar35 >> 0xc;
              iVar13 = (int)uVar14 >> 0xc;
              iVar20 = iVar3 * iVar17 + iVar13;
              iVar18 = iVar13 + 1;
              if (iVar17 < iVar4) {
                sourcePixelSample0 = sourcePixelSample0;
                sourcePixelSample1 = sourcePixelSample1;
                if ((-1 < iVar17) && (iVar13 < iVar3)) {
                  sourcePixelSample4 = sourcePixelSample0;
                  if (-1 < iVar13) {
                    sourcePixelSample4 =
                         *(PackedArgb32 *)
                          ((pGVar1->common).buildMetadata.assetRelativeAddressAnchor28 +
                          iVar20 * 4 + iVar5 + -0x28);
                  }
                  sourcePixelSample0 = sourcePixelSample4;
                  if ((-1 < iVar18) && (iVar18 < iVar3)) {
                    sourcePixelSample1 =
                         *(PackedArgb32 *)
                          ((pGVar1->common).buildMetadata.assetRelativeAddressAnchor28 +
                          iVar20 * 4 + iVar5 + -0x24);
                  }
                }
                if (((-1 < iVar17 + 1) && (iVar17 + 1 < iVar4)) && (iVar13 < iVar3)) {
                  sourcePixelSample2 = sourcePixelSample2;
                  if (-1 < iVar13) {
                    sourcePixelSample2 =
                         *(PackedArgb32 *)
                          ((pGVar1->common).buildMetadata.assetRelativeAddressAnchor28 +
                          (iVar20 + iVar3) * 4 + iVar5 + -0x28);
                  }
                  if ((-1 < iVar18) && (iVar18 < iVar3)) {
                    sourcePixelSample3 =
                         *(PackedArgb32 *)
                          ((pGVar1->common).buildMetadata.assetRelativeAddressAnchor28 +
                          (iVar20 + iVar3) * 4 + iVar5 + -0x24);
                  }
                }
              }
              uVar25 = (undefined1)(sourcePixelSample0 >> 0x18);
              uVar41 = CONCAT11(uVar25,uVar25);
              uVar47 = (undefined1)(sourcePixelSample0 >> 0x10);
              uVar46 = (undefined1)(sourcePixelSample0 >> 8);
              uVar25 = (undefined1)(sourcePixelSample1 >> 0x18);
              uVar48 = CONCAT11(uVar25,uVar25);
              uVar40 = (undefined1)(sourcePixelSample1 >> 0x10);
              uVar39 = (undefined1)(sourcePixelSample1 >> 8);
              uVar25 = (undefined1)(sourcePixelSample2 >> 0x18);
              uVar49 = CONCAT11(uVar25,uVar25);
              uVar33 = (undefined1)(sourcePixelSample2 >> 0x10);
              uVar32 = (undefined1)(sourcePixelSample2 >> 8);
              uVar25 = (undefined1)(sourcePixelSample3 >> 0x18);
              uVar50 = CONCAT11(uVar25,uVar25);
              uVar26 = (undefined1)(sourcePixelSample3 >> 0x10);
              uVar25 = (undefined1)(sourcePixelSample3 >> 8);
              iVar20 = (int)(uVar35 & 0xfff) >> 4;
              iVar18 = (int)(uVar14 & 0xfff) >> 4;
              mm7PackedValue0 =
                   pmulhw(CONCAT26(uVar41 >> 2,
                                   CONCAT24((ushort)CONCAT31(CONCAT21(uVar41,uVar47),uVar47) >> 2,
                                            CONCAT22(CONCAT11(uVar46,uVar46) >> 2,
                                                     CONCAT11((char)sourcePixelSample0,
                                                              (char)sourcePixelSample0) >> 2))),
                          *(undefined8 *)(iVar18 * 8 + 0x420720));
              mm5PackedValue0 =
                   pmulhw(CONCAT26(uVar48 >> 2,
                                   CONCAT24((ushort)CONCAT31(CONCAT21(uVar48,uVar40),uVar40) >> 2,
                                            CONCAT22(CONCAT11(uVar39,uVar39) >> 2,
                                                     CONCAT11((char)sourcePixelSample1,
                                                              (char)sourcePixelSample1) >> 2))),
                          *(undefined8 *)(iVar18 * 8 + 0x41f720));
              mm3PackedValue0 =
                   pmulhw(CONCAT26(uVar49 >> 2,
                                   CONCAT24((ushort)CONCAT31(CONCAT21(uVar49,uVar33),uVar33) >> 2,
                                            CONCAT22(CONCAT11(uVar32,uVar32) >> 2,
                                                     CONCAT11((char)sourcePixelSample2,
                                                              (char)sourcePixelSample2) >> 2))),
                          *(undefined8 *)(iVar18 * 8 + 0x420720));
              mm1PackedValue0 =
                   pmulhw(CONCAT26(uVar50 >> 2,
                                   CONCAT24((ushort)CONCAT31(CONCAT21(uVar50,uVar26),uVar26) >> 2,
                                            CONCAT22(CONCAT11(uVar25,uVar25) >> 2,
                                                     CONCAT11((char)sourcePixelSample3,
                                                              (char)sourcePixelSample3) >> 2))),
                          *(undefined8 *)(iVar18 * 8 + 0x41f720));
              uVar44 = pmulhw(CONCAT26((short)((ulonglong)mm7PackedValue0 >> 0x30) +
                                       (short)((ulonglong)mm5PackedValue0 >> 0x30),
                                       CONCAT24((short)((ulonglong)mm7PackedValue0 >> 0x20) +
                                                (short)((ulonglong)mm5PackedValue0 >> 0x20),
                                                CONCAT22((short)((ulonglong)mm7PackedValue0 >> 0x10)
                                                         + (short)((ulonglong)mm5PackedValue0 >>
                                                                  0x10),
                                                         (short)mm7PackedValue0 +
                                                         (short)mm5PackedValue0))),
                              *(undefined8 *)(iVar20 * 8 + 0x420720));
              uVar31 = pmulhw(CONCAT26((short)((ulonglong)mm3PackedValue0 >> 0x30) +
                                       (short)((ulonglong)mm1PackedValue0 >> 0x30),
                                       CONCAT24((short)((ulonglong)mm3PackedValue0 >> 0x20) +
                                                (short)((ulonglong)mm1PackedValue0 >> 0x20),
                                                CONCAT22((short)((ulonglong)mm3PackedValue0 >> 0x10)
                                                         + (short)((ulonglong)mm1PackedValue0 >>
                                                                  0x10),
                                                         (short)mm3PackedValue0 +
                                                         (short)mm1PackedValue0))),
                              *(undefined8 *)(iVar20 * 8 + 0x41f720));
              uVar41 = (ushort)((short)uVar44 + (short)uVar31) >> 2;
              uVar48 = (ushort)((short)((ulonglong)uVar44 >> 0x10) +
                               (short)((ulonglong)uVar31 >> 0x10)) >> 2;
              uVar49 = (ushort)((short)((ulonglong)uVar44 >> 0x20) +
                               (short)((ulonglong)uVar31 >> 0x20)) >> 2;
              uVar50 = (ushort)((short)((ulonglong)uVar44 >> 0x30) +
                               (short)((ulonglong)uVar31 >> 0x30)) >> 2;
              cVar9 = (uVar41 != 0) * (uVar41 < 0x100) * (char)uVar41 - (0xff < uVar41);
              cVar10 = (uVar48 != 0) * (uVar48 < 0x100) * (char)uVar48 - (0xff < uVar48);
              cVar11 = (uVar49 != 0) * (uVar49 < 0x100) * (char)uVar49 - (0xff < uVar49);
              cVar12 = (uVar50 != 0) * (uVar50 < 0x100) * (char)uVar50 - (0xff < uVar50);
              uVar41 = CONCAT11(cVar12,cVar12);
              uVar31 = pmaddwd(CONCAT26(uVar41 >> 4,
                                        CONCAT24((ushort)CONCAT31(CONCAT21(uVar41,cVar11),cVar11) >>
                                                 4,CONCAT22(CONCAT11(cVar10,cVar10) >> 4,
                                                            CONCAT11(cVar9,cVar9) >> 4))) &
                               (ulonglong)g_SoftwarePixelMmxConstants.quantizeMasksQ12,
                               g_SoftwarePixelMmxConstants.packWeights);
              *(short *)pbVar21 =
                   (short)((ulonglong)uVar31 >> 0x28) + (short)((ulonglong)uVar31 >> 8);
              uVar14 = uVar14 + (int)extraout_MM0;
              uVar35 = uVar35 + iVar23 + extraout_MM1_Da;
              pbVar21 = pbVar21 + 2;
              iStack_2c = iStack_2c + -1;
            } while (iStack_2c != 0);
            uVar14 = uVar15 + (int)uVar27;
            uVar35 = uVar16 + iVar28 + iVar29;
            pbVar21 = pbStack_28 + g_FramebufferRowStrideBytes;
            clipTop = clipTop + -1;
            uVar15 = uVar14;
            uVar16 = uVar35;
            iStack_2c = iVar19;
            pbStack_28 = pbVar21;
          } while (clipTop != 0);
        }
        else {
          pbVar21 = g_FramebufferAccess->pixels +
                    g_FramebufferRowStrideBytes * clipBottom + clipRight * 4;
          uVar35 = iVar13 + iVar20;
          uVar15 = uVar14;
          uVar16 = uVar35;
          pbStack_28 = pbVar21;
          do {
            do {
              uVar43 = 0;
              uVar42 = 0;
              uVar37 = 0;
              uVar36 = 0;
              uVar30 = 0;
              uVar24 = 0;
              iVar17 = (int)uVar35 >> 0xc;
              iVar13 = (int)uVar14 >> 0xc;
              iVar20 = iVar3 * iVar17 + iVar13;
              iVar18 = iVar13 + 1;
              if (iVar17 < iVar4) {
                uVar42 = uVar43;
                uVar36 = uVar37;
                if ((-1 < iVar17) && (iVar13 < iVar3)) {
                  if (-1 < iVar13) {
                    uVar43 = *(undefined4 *)
                              ((pGVar1->common).buildMetadata.assetRelativeAddressAnchor28 +
                              iVar20 * 4 + iVar5 + -0x28);
                  }
                  uVar42 = uVar43;
                  if ((-1 < iVar18) && (iVar18 < iVar3)) {
                    uVar36 = *(undefined4 *)
                              ((pGVar1->common).buildMetadata.assetRelativeAddressAnchor28 +
                              iVar20 * 4 + iVar5 + -0x24);
                  }
                }
                if (((-1 < iVar17 + 1) && (iVar17 + 1 < iVar4)) && (iVar13 < iVar3)) {
                  uVar30 = 0;
                  if (-1 < iVar13) {
                    uVar30 = *(undefined4 *)
                              ((pGVar1->common).buildMetadata.assetRelativeAddressAnchor28 +
                              (iVar20 + iVar3) * 4 + iVar5 + -0x28);
                  }
                  if ((-1 < iVar18) && (iVar18 < iVar3)) {
                    uVar24 = *(undefined4 *)
                              ((pGVar1->common).buildMetadata.assetRelativeAddressAnchor28 +
                              (iVar20 + iVar3) * 4 + iVar5 + -0x24);
                  }
                }
              }
              uVar25 = (undefined1)((uint)uVar42 >> 0x18);
              uVar41 = CONCAT11(uVar25,uVar25);
              uVar47 = (undefined1)((uint)uVar42 >> 0x10);
              uVar46 = (undefined1)((uint)uVar42 >> 8);
              uVar25 = (undefined1)((uint)uVar36 >> 0x18);
              uVar48 = CONCAT11(uVar25,uVar25);
              uVar40 = (undefined1)((uint)uVar36 >> 0x10);
              uVar39 = (undefined1)((uint)uVar36 >> 8);
              uVar25 = (undefined1)((uint)uVar30 >> 0x18);
              uVar49 = CONCAT11(uVar25,uVar25);
              uVar33 = (undefined1)((uint)uVar30 >> 0x10);
              uVar32 = (undefined1)((uint)uVar30 >> 8);
              uVar25 = (undefined1)((uint)uVar24 >> 0x18);
              uVar50 = CONCAT11(uVar25,uVar25);
              uVar26 = (undefined1)((uint)uVar24 >> 0x10);
              uVar25 = (undefined1)((uint)uVar24 >> 8);
              iVar20 = (int)(uVar35 & 0xfff) >> 4;
              iVar18 = (int)(uVar14 & 0xfff) >> 4;
              uVar45 = pmulhw(CONCAT26(uVar41 >> 2,
                                       CONCAT24((ushort)CONCAT31(CONCAT21(uVar41,uVar47),uVar47) >>
                                                2,CONCAT22(CONCAT11(uVar46,uVar46) >> 2,
                                                           CONCAT11((char)uVar42,(char)uVar42) >> 2)
                                               )),*(undefined8 *)(iVar18 * 8 + 0x420720));
              uVar38 = pmulhw(CONCAT26(uVar48 >> 2,
                                       CONCAT24((ushort)CONCAT31(CONCAT21(uVar48,uVar40),uVar40) >>
                                                2,CONCAT22(CONCAT11(uVar39,uVar39) >> 2,
                                                           CONCAT11((char)uVar36,(char)uVar36) >> 2)
                                               )),*(undefined8 *)(iVar18 * 8 + 0x41f720));
              uVar44 = pmulhw(CONCAT26(uVar49 >> 2,
                                       CONCAT24((ushort)CONCAT31(CONCAT21(uVar49,uVar33),uVar33) >>
                                                2,CONCAT22(CONCAT11(uVar32,uVar32) >> 2,
                                                           CONCAT11((char)uVar30,(char)uVar30) >> 2)
                                               )),*(undefined8 *)(iVar18 * 8 + 0x420720));
              uVar31 = pmulhw(CONCAT26(uVar50 >> 2,
                                       CONCAT24((ushort)CONCAT31(CONCAT21(uVar50,uVar26),uVar26) >>
                                                2,CONCAT22(CONCAT11(uVar25,uVar25) >> 2,
                                                           CONCAT11((char)uVar24,(char)uVar24) >> 2)
                                               )),*(undefined8 *)(iVar18 * 8 + 0x41f720));
              uVar38 = pmulhw(CONCAT26((short)((ulonglong)uVar45 >> 0x30) +
                                       (short)((ulonglong)uVar38 >> 0x30),
                                       CONCAT24((short)((ulonglong)uVar45 >> 0x20) +
                                                (short)((ulonglong)uVar38 >> 0x20),
                                                CONCAT22((short)((ulonglong)uVar45 >> 0x10) +
                                                         (short)((ulonglong)uVar38 >> 0x10),
                                                         (short)uVar45 + (short)uVar38))),
                              *(undefined8 *)(iVar20 * 8 + 0x420720));
              uVar31 = pmulhw(CONCAT26((short)((ulonglong)uVar44 >> 0x30) +
                                       (short)((ulonglong)uVar31 >> 0x30),
                                       CONCAT24((short)((ulonglong)uVar44 >> 0x20) +
                                                (short)((ulonglong)uVar31 >> 0x20),
                                                CONCAT22((short)((ulonglong)uVar44 >> 0x10) +
                                                         (short)((ulonglong)uVar31 >> 0x10),
                                                         (short)uVar44 + (short)uVar31))),
                              *(undefined8 *)(iVar20 * 8 + 0x41f720));
              uVar41 = (ushort)((short)uVar38 + (short)uVar31) >> 2;
              uVar48 = (ushort)((short)((ulonglong)uVar38 >> 0x10) +
                               (short)((ulonglong)uVar31 >> 0x10)) >> 2;
              uVar49 = (ushort)((short)((ulonglong)uVar38 >> 0x20) +
                               (short)((ulonglong)uVar31 >> 0x20)) >> 2;
              uVar50 = (ushort)((short)((ulonglong)uVar38 >> 0x30) +
                               (short)((ulonglong)uVar31 >> 0x30)) >> 2;
              *(uint *)pbVar21 =
                   CONCAT13((uVar50 != 0) * (uVar50 < 0x100) * (char)uVar50 - (0xff < uVar50),
                            CONCAT12((uVar49 != 0) * (uVar49 < 0x100) * (char)uVar49 -
                                     (0xff < uVar49),
                                     CONCAT11((uVar48 != 0) * (uVar48 < 0x100) * (char)uVar48 -
                                              (0xff < uVar48),
                                              (uVar41 != 0) * (uVar41 < 0x100) * (char)uVar41 -
                                              (0xff < uVar41))));
              uVar14 = uVar14 + (int)extraout_MM0;
              uVar35 = uVar35 + iVar23 + extraout_MM1_Da;
              pbVar21 = pbVar21 + 4;
              iStack_2c = iStack_2c + -1;
            } while (iStack_2c != 0);
            uVar14 = uVar15 + (int)uVar27;
            uVar35 = uVar16 + iVar28 + iVar29;
            pbVar21 = pbStack_28 + g_FramebufferRowStrideBytes;
            clipTop = clipTop + -1;
            uVar15 = uVar14;
            uVar16 = uVar35;
            iStack_2c = iVar19;
            pbStack_28 = pbVar21;
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
  int iVar1;
  int iVar2;
  int iVar3;
  int iVar4;
  longlong lVar5;
  longlong lVar6;
  longlong lVar7;
  uint uVar8;
  uint uVar9;
  
  lVar5 = (longlong)control->sampleScaleQ12 * (longlong)g_FixedCosQ28[control->rotationAngle];
  iVar1 = -((int)((ulonglong)lVar5 >> 0x20) << 4 | (uint)lVar5 >> 0x1c);
  lVar5 = (longlong)control->sampleScaleQ12 * (longlong)g_FixedSinQ28[control->rotationAngle];
  uVar8 = (int)((ulonglong)lVar5 >> 0x20) << 4 | (uint)lVar5 >> 0x1c;
  lVar5 = (longlong)(int)uVar8 * 0x1c6e9c;
  lVar6 = (longlong)iVar1 * -0x20c8cc;
  uVar9 = (int)((ulonglong)lVar6 >> 0x20) << 0xb | (uint)lVar6 >> 0x15;
  lVar6 = (longlong)iVar1 * 0x1c6e9c;
  lVar7 = (longlong)(int)-uVar8 * -0x20c8cc;
  uVar8 = (int)((ulonglong)lVar7 >> 0x20) << 0xb | (uint)lVar7 >> 0x15;
  iVar1 = (control->base).left;
  iVar2 = (control->base).right;
  iVar3 = (control->base).top;
  iVar4 = (control->base).bottom;
  control->selectedSourceYQ12 =
       control->sourceOriginYQ12 -
       ((((int)((ulonglong)lVar6 >> 0x20) << 0xc | (uint)lVar6 >> 0x14) - uVar8) *
        (((control->base).top + (control->base).bottom >> 1) - pointerY) +
       (((int)((ulonglong)lVar5 >> 0x20) << 0xc | (uint)lVar5 >> 0x14) - uVar9) *
       (((control->base).left + (control->base).right >> 1) - pointerX));
  control->selectedSourceXQ12 =
       (control->sourceOriginXQ12 - uVar9 * 2 * ((iVar1 + iVar2 >> 1) - pointerX)) -
       uVar8 * 2 * ((iVar3 + iVar4 >> 1) - pointerY);
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
  UiSelectableStateFlags *pUVar1;
  UiNodeFlags *pUVar2;
  UiRootCallbacks **ppUVar3;
  UiNodeVtable *pUVar4;
  UiRootNode *pUVar5;
  UiNodeBase *pUVar6;
  UiImageControl *node;
  UiRootNode *root;
  bool bVar7;
  
  node = g_UiImageControlHoverTarget;
  pUVar5 = g_UiRootNode;
  if (g_UiPointerCaptureButton == UI_POINTER_CAPTURE_NONE) {
    root = pUVar5;
    if (g_UiImageControlHoverTarget != (UiImageControl *)0x0) {
      pUVar6 = UiImageControl_HitTestOpaque(pointerY,pointerX,g_UiImageControlHoverTarget);
      g_UiImageControlHoverTarget = (UiImageControl *)0x0;
      if (pUVar6 != (UiNodeBase *)0xffffffff) {
UiPointer_DispatchLeftPressToCapturedTarget:
        g_UiPointerCaptureButton = UI_POINTER_CAPTURE_LEFT;
        if ((buttonMask & 0x80000000) == NONE) {
          pUVar2 = &(node->selectable).base.nodeFlags;
          *pUVar2 = *pUVar2 & ~UI_NODE_REPEAT_OR_DOUBLE_CLICK;
        }
        else {
          pUVar2 = &(node->selectable).base.nodeFlags;
          *pUVar2 = *pUVar2 | UI_NODE_REPEAT_OR_DOUBLE_CLICK;
        }
        pUVar4 = (node->selectable).base.vtable;
        g_UiPointerCaptureTarget = (UiNodeBase *)node;
        if (((node->selectable).base.nodeFlags &
            (UI_NODE_FALLBACK_FOCUS_TARGET|UI_NODE_PREFERRED_FOCUS_TARGET)) != 0) {
          UiKeyboardFocus_Set((UiNodeBase *)node);
        }
        (*pUVar4->nonRightPress)(wheelDelta,pointerY,pointerX,(UiNodeBase *)node);
        if (g_UiPointerCaptureTarget == (UiNodeBase *)0xffffffff) {
          return;
        }
        (*g_UiPointerCaptureTarget->vtable->nonRightDrag)
                  (wheelDelta,pointerY,pointerX,g_UiPointerCaptureTarget);
        return;
      }
      pUVar1 = &(node->selectable).stateFlags;
      *pUVar1 = *pUVar1 & 0xfffff9fc;
    }
    while (root != (UiRootNode *)0xffffffff) {
      if (((((root->rootFlags & UI_ROOT_DISABLE_POINTER_HIT_TEST) == 0) &&
           ((root->base).left <= pointerX)) && ((root->base).top <= pointerY)) &&
         ((pointerX < (root->base).right && (pointerY < (root->base).bottom)))) {
        node = (UiImageControl *)(*((root->base).vtable)->hitTest)(pointerY,pointerX,&root->base);
        if ((root != pUVar5) && (bVar7 = UiRootStack_BringToFront(root), bVar7)) {
          return;
        }
        if (node == (UiImageControl *)0xffffffff) {
          return;
        }
        goto UiPointer_DispatchLeftPressToCapturedTarget;
      }
      ppUVar3 = &root->callbacks;
      root = root->previousRoot;
      if (((*ppUVar3)->method08 != (UiRootMethod08Callback *)0x0) &&
         (bVar7 = (*(*ppUVar3)->method08)(root), bVar7)) {
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
  UiSelectableStateFlags *pUVar1;
  UiNodeFlags *pUVar2;
  UiRootCallbacks **ppUVar3;
  UiNodeVtable *pUVar4;
  UiRootNode *pUVar5;
  UiNodeBase *pUVar6;
  UiImageControl *node;
  UiRootNode *root;
  bool bVar7;
  
  node = g_UiImageControlHoverTarget;
  pUVar5 = g_UiRootNode;
  if (g_UiPointerCaptureButton == UI_POINTER_CAPTURE_NONE) {
    root = pUVar5;
    if (g_UiImageControlHoverTarget != (UiImageControl *)0x0) {
      pUVar6 = UiImageControl_HitTestOpaque(pointerY,pointerX,g_UiImageControlHoverTarget);
      g_UiImageControlHoverTarget = (UiImageControl *)0x0;
      if (pUVar6 != (UiNodeBase *)0xffffffff) {
UiPointer_DispatchMiddlePressToCapturedTarget:
        g_UiPointerCaptureButton = UI_POINTER_CAPTURE_MIDDLE;
        pUVar2 = &(node->selectable).base.nodeFlags;
        *pUVar2 = *pUVar2 | UI_NODE_REPEAT_OR_DOUBLE_CLICK;
        pUVar4 = (node->selectable).base.vtable;
        g_UiPointerCaptureTarget = (UiNodeBase *)node;
        if (((node->selectable).base.nodeFlags &
            (UI_NODE_FALLBACK_FOCUS_TARGET|UI_NODE_PREFERRED_FOCUS_TARGET)) != 0) {
          UiKeyboardFocus_Set((UiNodeBase *)node);
        }
        (*pUVar4->nonRightPress)(wheelDelta,pointerY,pointerX,(UiNodeBase *)node);
        if (g_UiPointerCaptureTarget == (UiNodeBase *)0xffffffff) {
          return;
        }
        (*g_UiPointerCaptureTarget->vtable->nonRightDrag)
                  (wheelDelta,pointerY,pointerX,g_UiPointerCaptureTarget);
        return;
      }
      pUVar1 = &(node->selectable).stateFlags;
      *pUVar1 = *pUVar1 & 0xfffff9fc;
    }
    while (root != (UiRootNode *)0xffffffff) {
      if (((((root->rootFlags & UI_ROOT_DISABLE_POINTER_HIT_TEST) == 0) &&
           ((root->base).left <= pointerX)) && ((root->base).top <= pointerY)) &&
         ((pointerX < (root->base).right && (pointerY < (root->base).bottom)))) {
        node = (UiImageControl *)(*((root->base).vtable)->hitTest)(pointerY,pointerX,&root->base);
        if ((root != pUVar5) && (bVar7 = UiRootStack_BringToFront(root), bVar7)) {
          return;
        }
        if (node == (UiImageControl *)0xffffffff) {
          return;
        }
        goto UiPointer_DispatchMiddlePressToCapturedTarget;
      }
      ppUVar3 = &root->callbacks;
      root = root->previousRoot;
      if (((*ppUVar3)->method08 != (UiRootMethod08Callback *)0x0) &&
         (bVar7 = (*(*ppUVar3)->method08)(root), bVar7)) {
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
  UiSelectableStateFlags *pUVar1;
  UiRootCallbacks **ppUVar2;
  UiNodeVtable *pUVar3;
  UiRootNode *pUVar4;
  UiNodeBase *node;
  UiRootNode *root;
  bool bVar5;
  
  pUVar4 = g_UiRootNode;
  if (g_UiPointerCaptureButton == UI_POINTER_CAPTURE_NONE) {
    root = pUVar4;
    if (g_UiImageControlHoverTarget != (UiImageControl *)0x0) {
      pUVar1 = &(g_UiImageControlHoverTarget->selectable).stateFlags;
      *pUVar1 = *pUVar1 & 0xfffff9fc;
    }
    while (root != (UiRootNode *)0xffffffff) {
      if (((((root->rootFlags & UI_ROOT_DISABLE_POINTER_HIT_TEST) == 0) &&
           ((root->base).left <= pointerX)) && ((root->base).top <= pointerY)) &&
         ((pointerX < (root->base).right && (pointerY < (root->base).bottom)))) {
        node = (*((root->base).vtable)->hitTest)(pointerY,pointerX,&root->base);
        if ((root != pUVar4) && (bVar5 = UiRootStack_BringToFront(root), bVar5)) {
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
        pUVar3 = node->vtable;
        g_UiPointerCaptureTarget = node;
        if ((node->nodeFlags & (UI_NODE_FALLBACK_FOCUS_TARGET|UI_NODE_PREFERRED_FOCUS_TARGET)) != 0)
        {
          UiKeyboardFocus_Set(node);
        }
        (*pUVar3->rightPress)(wheelDelta,pointerY,pointerX,node);
        if (g_UiPointerCaptureTarget == (UiNodeBase *)0xffffffff) {
          return;
        }
        (*g_UiPointerCaptureTarget->vtable->rightDrag)
                  (wheelDelta,pointerY,pointerX,g_UiPointerCaptureTarget);
        return;
      }
      ppUVar2 = &root->callbacks;
      root = root->previousRoot;
      if (((*ppUVar2)->method08 != (UiRootMethod08Callback *)0x0) &&
         (bVar5 = (*(*ppUVar2)->method08)(root), bVar5)) {
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
joined_r0x004affea:
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
  UiNodeVtable *pUVar1;
  UiRootPointerMissPolicyCallback *pUVar2;
  UiNodeBase *pUVar3;
  int iVar4;
  UiRootNode *root;
  
  pUVar3 = g_UiPointerCaptureTarget;
  if (g_UiHoverSelectionRecord != (UiCommandRuntimeRecordPrefix *)0x0) {
    g_UiHoverSelectionRecord = (UiCommandRuntimeRecordPrefix *)0x0;
    InGameSelectionDetailPanel_Rebuild();
  }
  UiTooltip_UpdateHoverTarget(pointerY,pointerX);
  root = g_UiRootNode;
  if (pUVar3 != (UiNodeBase *)0xffffffff) {
    if (g_UiPointerCaptureButton == UI_POINTER_CAPTURE_RIGHT) {
      (*pUVar3->vtable->rightDrag)(wheelDelta,pointerY,pointerX,pUVar3);
    }
    else {
      (*pUVar3->vtable->nonRightDrag)(wheelDelta,pointerY,pointerX,pUVar3);
    }
    return;
  }
  while( true ) {
    if (root == (UiRootNode *)0xffffffff) {
      return;
    }
    if (((((root->base).left <= pointerX) && ((root->base).top <= pointerY)) &&
        (pointerX < (root->base).right)) && (pointerY < (root->base).bottom)) {
      pUVar3 = (*((root->base).vtable)->hitTest)(pointerY,pointerX,&root->base);
      if (pUVar3 == (UiNodeBase *)0xffffffff) {
        return;
      }
      pUVar1 = pUVar3->vtable;
      (*pUVar1->pointerMove)(pointerY,pointerX,pUVar3);
      if (wheelDelta == 0) {
        return;
      }
      (*pUVar1->pointerWheel)(wheelDelta,pointerY,pointerX,pUVar3);
      return;
    }
    pUVar2 = root->callbacks->pointerMissPolicy;
    if (pUVar2 == (UiRootPointerMissPolicyCallback *)0x0) {
      return;
    }
    iVar4 = (*pUVar2)(root);
    if (-1 < iVar4) break;
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
  UiNodeBase *control_00;
  
  control_00 = control->parent;
  if (control_00 != (UiNodeBase *)0xffffffff) {
    (*control_00->vtable->pointerWheel)(wheelDelta,pointerY,pointerX,control_00);
  }
  return;
}


/* Address: 0x004B08C0.
   Ownership: ui/controls/input.
   Purpose: Shared three-argument keyboard fallback in vtable slot +0x30. Event value 0x00010002 calls
   UiKeyboardFocus_MoveNext and returns CF clear; other values return CF set. EAX and companion registers remain
   untouched by the prototype.
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


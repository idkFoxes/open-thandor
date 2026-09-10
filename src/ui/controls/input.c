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
void __cdecl UiPointer_DispatchPendingEvents(void)

{
  UiNodeBase *control;
  byte bVar1;
  UiPixelCoordinate pointerX;
  UiPixelCoordinate pointerX_00;
  UiPixelCoordinate pointerX_01;
  UiPixelCoordinate pointerX_02;
  UiPixelCoordinate pointerY;
  UiPixelCoordinate pointerY_00;
  UiPixelCoordinate pointerY_01;
  UiPixelCoordinate pointerY_02;
  GraphicsCursorButtonState unaff_EBX;
  UiPointerWheelDelta unaff_ESI;
  undefined1 uVar2;
  
  (*g_SpinLockAcquire)(g_UiRuntimeFrameLock);
  uVar2 = g_PointerSetPosition < DirectInputMouse_SetPosition;
  if (g_PointerSetPosition == DirectInputMouse_SetPosition) {
    DirectInputMouse_PollBufferedEvents();
  }
  while( true ) {
    control = g_UiPointerCaptureTarget;
    bVar1 = (*g_GraphicsCursorConsumeEvent)();
    if ((bool)uVar2) break;
    if ((char)bVar1 < '\a') {
      if (bVar1 == 6) {
        uVar2 = control != (UiNodeBase *)0xffffffff;
        if ((control != (UiNodeBase *)0xffffffff) &&
           (uVar2 = g_UiPointerCaptureButton == UI_POINTER_CAPTURE_LEFT,
           g_UiPointerCaptureButton == UI_POINTER_CAPTURE_MIDDLE)) {
          (*control->vtable->nonRightRelease)(unaff_ESI,pointerY,pointerX,control);
          uVar2 = 0;
          g_UiPointerCaptureButton = UI_POINTER_CAPTURE_NONE;
          g_UiPointerCaptureTarget = (UiNodeBase *)0xffffffff;
          UiPointer_DispatchMotionAndWheel(unaff_ESI,pointerY_02,pointerX_02);
          Random_NextPrimary();
        }
      }
      else {
        uVar2 = bVar1 < 3;
        if ((char)bVar1 < '\x04') {
          if (bVar1 == 3) {
            UiPointer_DispatchRightPress(unaff_EBX,unaff_ESI,pointerY,pointerX);
          }
          else {
            uVar2 = bVar1 == 0;
            if ((char)bVar1 < '\x02') {
              if (bVar1 == 1) {
                UiPointer_DispatchLeftPress(unaff_EBX,unaff_ESI,pointerY,pointerX);
              }
              else {
                UiPointer_DispatchMotionAndWheel(unaff_ESI,pointerY,pointerX);
              }
            }
            else {
              UiPointer_DispatchMiddlePress(unaff_EBX,unaff_ESI,pointerY,pointerX);
            }
          }
        }
        else {
          uVar2 = control != (UiNodeBase *)0xffffffff;
          if ((control != (UiNodeBase *)0xffffffff) &&
             (uVar2 = 0, g_UiPointerCaptureButton == UI_POINTER_CAPTURE_LEFT)) {
            (*control->vtable->nonRightRelease)(unaff_ESI,pointerY,pointerX,control);
            uVar2 = 0;
            g_UiPointerCaptureButton = UI_POINTER_CAPTURE_NONE;
            g_UiPointerCaptureTarget = (UiNodeBase *)0xffffffff;
            UiPointer_DispatchMotionAndWheel(unaff_ESI,pointerY_01,pointerX_01);
          }
        }
      }
    }
    else {
      uVar2 = control != (UiNodeBase *)0xffffffff;
      if ((control != (UiNodeBase *)0xffffffff) &&
         (uVar2 = g_UiPointerCaptureButton < UI_POINTER_CAPTURE_RIGHT,
         g_UiPointerCaptureButton == UI_POINTER_CAPTURE_RIGHT)) {
        (*control->vtable->rightRelease)(unaff_ESI,pointerY,pointerX,control);
        uVar2 = 0;
        g_UiPointerCaptureButton = UI_POINTER_CAPTURE_NONE;
        g_UiPointerCaptureTarget = (UiNodeBase *)0xffffffff;
        UiPointer_DispatchMotionAndWheel(unaff_ESI,pointerY_00,pointerX_00);
      }
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
void UiKeyboardFocus_ReleaseNode(UiNodeBase *node)

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
void __cdecl UiKeyboard_DispatchPendingEvents(void)

{
  UiNodeFlags UVar1;
  bool bVar2;
  UiKeyboardEventCode extraout_ECX;
  UiKeyboardEventCode keyCode;
  UiKeyboardEventCode extraout_ECX_00;
  UiKeyboardStateMask extraout_EDX;
  UiKeyboardStateMask keyboardStateMask;
  UiKeyboardStateMask extraout_EDX_00;
  UiNodeBase *control;
  UiNodeBase *pUVar3;
  undefined1 in_CF;
  qword qVar4;
  
  (*g_SpinLockAcquire)(g_UiRuntimeFrameLock);
  do {
    do {
      while( true ) {
        do {
          qVar4 = (*g_KeyboardReadEvent)();
          control = g_UiKeyboardFocusNode;
          if ((bool)in_CF) {
            (*g_SpinLockReleaseAndInvoke)
                      ((SpinLockReleaseCallbackProc *)g_UiRuntimePostUnlockCallback,
                       g_UiRuntimeFrameLock);
            return;
          }
          in_CF = g_UiPointerCaptureTarget != (UiNodeBase *)0xffffffff;
        } while (g_UiPointerCaptureTarget != (UiNodeBase *)0xffffffff);
        if (g_UiKeyboardFocusNode != (UiNodeBase *)0xffffffff) break;
UiKeyboard_DispatchEventToRootFallback:
        in_CF = g_UiRootNode != (UiRootNode *)0xffffffff;
        if (g_UiRootNode != (UiRootNode *)0xffffffff) {
          in_CF = 0;
          if (g_UiRootNode->callbacks->keyboardFallbackCf != (UiRootKeyboardFallbackCf *)0x0) {
            (*g_UiRootNode->callbacks->keyboardFallbackCf)
                      ((dword)(qVar4 >> 0x20),(dword)qVar4,g_UiRootNode);
          }
        }
      }
      in_CF = false;
      bVar2 = false;
      (*g_UiKeyboardFocusNode->vtable->keyboardEventCf)
                ((UiKeyboardStateMask)(qVar4 >> 0x20),(UiKeyboardEventCode)qVar4,
                 g_UiKeyboardFocusNode);
      keyCode = extraout_ECX;
      keyboardStateMask = extraout_EDX;
    } while (!(bool)in_CF);
    do {
      do {
        qVar4 = CONCAT44(keyboardStateMask,keyCode);
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
      qVar4 = CONCAT44(keyboardStateMask,keyCode);
      if (control == g_UiKeyboardFocusNode) goto UiKeyboard_DispatchEventToRootFallback;
      in_CF = false;
    } while (((control->nodeFlags & UI_NODE_SUPPRESSED) != 0) ||
            ((*control->vtable->keyboardEventCf)(keyboardStateMask,keyCode,control),
            keyCode = extraout_ECX_00, keyboardStateMask = extraout_EDX_00, (bool)in_CF));
    UiKeyboardFocus_Set(control);
  } while( true );
}

/* Address: 0x004B0030.
   Ownership: ui/controls/input.
   Purpose: Traverses a subtree for an initial keyboard-focus target, preferring nodeFlags bit 0x02 and using bit
   0x20 as a fallback.
   Local calls: UiKeyboardFocus_Set.
*/
void UiKeyboardFocus_SelectInitial(UiNodeBase *root)

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
void UiKeyboardFocus_AcquireIfNone(UiNodeBase *node)

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
void UiRangeSliderControl_HandleKeyboardCf
               (UiKeyboardStateMask keyboardStateMask,UiKeyboardEventCode keyCode,
               UiNodeBase *control)

{
  UiNodeBase *increasedSliderValue;
  UiNodeBase *adjustedSliderValue;
  
  if ((control->nodeFlags & UI_NODE_SUPPRESSED) != 0) {
UiRangeSliderControl_DelegateUnhandledKeyboardEvent:
    UiNode_DefaultKeyboardEventMoveFocusNextCf(keyboardStateMask,keyCode,control);
    return;
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
      return;
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
  return;
}

/* Address: 0x004B9CB0.
   Ownership: ui/controls/input.
   Purpose: Binary entry is anchored by g_UiNodeVtable_004B9530[12]@004B9530.
   Local calls: UiNode_DefaultKeyboardEventMoveFocusNextCf.
   Cross-module calls: UiNode_InvalidateRoot [ui/core/runtime].
*/
void UiFocusProxyControl_ForwardKeyboardEventToChildCf
               (UiKeyboardStateMask keyboardStateMask,UiKeyboardEventCode keyCode,
               UiNodeBase *control)

{
  UiNodeBase *control_00;
  bool childHandledEvent;
  
  control_00 = control[1].firstChild;
  if ((keyCode & 0xffff0000) == 0) {
    if ((((uint)control[1].nextSibling & 0x8000) != 0) && ((keyCode & 0x30) != 0)) {
      return;
    }
  }
  else if (keyCode == 0x10002) {
    UiNode_DefaultKeyboardEventMoveFocusNextCf(keyboardStateMask,0x10002,control);
    return;
  }
  childHandledEvent = false;
  if (control_00 != (UiNodeBase *)0x0) {
    (*control_00->vtable->keyboardEventCf)(keyboardStateMask,keyCode,control_00);
    if (!childHandledEvent) {
      UiNode_InvalidateRoot(control);
      return;
    }
  }
  return;
}

/* Address: 0x004B9DA0.
   Ownership: ui/controls/input.
   Purpose: Binary entry is anchored by g_UiNodeVtable_004B9530[17]@004B9530.
   Local calls: UiNode_ForwardPointerWheelToParent.
   Cross-module calls: UiNode_InvalidateRoot [ui/core/runtime].
*/
void UiFocusProxyControl_ForwardPointerWheelToChildOrParent
               (UiPointerWheelDelta wheelDelta,UiPixelCoordinate pointerY,UiPixelCoordinate pointerX
               ,UiNodeBase *control)

{
  UiNodeBase *control_00;
  UiNodeBase *extraout_EDX;
  
  if (((uint)control[1].nextSibling & 0x400) == 0) {
    control_00 = control[1].firstChild;
    control[1].nextSibling = (UiNodeBase *)((uint)control[1].nextSibling | 0x400);
    if (control_00 != (UiNodeBase *)0x0) {
      if (control == g_UiKeyboardFocusNode) {
        g_UiKeyboardFocusNode = control_00;
        control_00->nodeFlags = control_00->nodeFlags | UI_NODE_HAS_KEYBOARD_FOCUS;
      }
      (*control_00->vtable->pointerWheel)(wheelDelta,pointerY,pointerX,control_00);
      if (extraout_EDX == g_UiKeyboardFocusNode) {
        g_UiKeyboardFocusNode = control;
        extraout_EDX->nodeFlags = extraout_EDX->nodeFlags & ~UI_NODE_HAS_KEYBOARD_FOCUS;
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
GraphicsCursorFrameIndex
UiNode_DefaultPointerMove(UiPixelCoordinate pointerY,UiPixelCoordinate pointerX,UiNodeBase *control)

{
  return 0;
}

/* Address: 0x004B42D0.
   Ownership: ui/controls/input.
   Purpose: Binary entry is anchored by g_UiNodeVtable_004B3EF0[8]@004B3EF0.
   Cross-module calls: UiActionQueue_Enqueue [ui/core/runtime], UiNode_InvalidateRoot [ui/core/runtime].
*/
void UiRangeSliderControl_UpdateValueFromPointer
               (UiPointerWheelDelta wheelDelta,UiPixelCoordinate pointerY,UiPixelCoordinate pointerX
               ,UiNodeBase *control)

{
  ulonglong uVar1;
  uint uVar2;
  UiNodeBase *pUVar3;
  qword qVar5;
  uint uVar4;
  int extraout_EDX;
  
  if (((uint)control[1].nextSibling & 2) != 0) {
    if (((uint)control[1].nextSibling & 1) == 0) {
      qVar5._0_4_ = (*g_GraphicsTextureSourceGetLogicalSize)(0xaf,g_UiWindowTextureSource);
      uVar4 = control->layoutWidth - (int)(qword)qVar5;
      if (uVar4 == 0) {
        uVar4 = 1;
      }
      uVar2 = (pointerX - ((int)(qword)qVar5 >> 1)) - control->left;
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
    (*g_GraphicsTextureSourceGetLogicalSize)(0xb7,g_UiWindowTextureSource);
    uVar4 = control->layoutHeight - extraout_EDX;
    if (uVar4 == 0) {
      uVar4 = 1;
    }
    uVar2 = (control->bottom - pointerY) - (extraout_EDX >> 1);
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
void UiRangeSliderControl_HandlePointerWheel
               (UiPointerWheelDelta wheelDelta,UiPixelCoordinate pointerY,UiPixelCoordinate pointerX
               ,UiNodeBase *control)

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
void UiFocusProxyControl_RelocateChild
               (UiSerializedRelocationDelta relocationDelta,UiNodeBase *control)

{
  int extraout_EAX;
  UiNodeBase *unaff_EBX;
  UiNodeFlags *childNodeFlagsField;
  
  if (((unaff_EBX->nodeFlags & (UI_NODE_FALLBACK_FOCUS_TARGET|UI_NODE_PREFERRED_FOCUS_TARGET)) == 0)
     && (unaff_EBX[1].firstChild != (UiNodeBase *)0x0)) {
    unaff_EBX->nodeFlags = unaff_EBX->nodeFlags | UI_NODE_FALLBACK_FOCUS_TARGET;
  }
  UiContainer_RelocateChildren(relocationDelta,unaff_EBX);
  if (unaff_EBX[1].firstChild != (UiNodeBase *)0x0) {
    unaff_EBX[1].firstChild =
         (UiNodeBase *)((int)&(unaff_EBX[1].firstChild)->nextSibling + extraout_EAX);
    childNodeFlagsField = &(unaff_EBX[1].firstChild)->nodeFlags;
    *childNodeFlagsField =
         *childNodeFlagsField & ~(UI_NODE_FALLBACK_FOCUS_TARGET|UI_NODE_PREFERRED_FOCUS_TARGET);
  }
  if (((uint)control[1].nextSibling & 0x20) != 0) {
    control[1].parent = (UiNodeBase *)((int)&(control[1].parent)->nextSibling + extraout_EAX);
    control[1].nextSibling = (UiNodeBase *)((uint)control[1].nextSibling & 0xffffffdf);
  }
  return;
}

/* Address: 0x004B99A0.
   Ownership: ui/controls/input.
   Purpose: Binary entry is anchored by g_UiNodeVtable_004B9530[4]@004B9530.
   Cross-module calls: UiNode_InvalidateRoot [ui/core/runtime].
*/
void UiFocusProxyControl_ForwardNonRightPressToChild
               (UiPointerWheelDelta wheelDelta,UiPixelCoordinate pointerY,UiPixelCoordinate pointerX
               ,UiNodeBase *control)

{
  UiNodeBase *control_00;
  UiNodeBase *extraout_EDX;
  
  control_00 = control[1].firstChild;
  if (control_00 != (UiNodeBase *)0x0) {
    if (control == g_UiKeyboardFocusNode) {
      g_UiKeyboardFocusNode = control_00;
      control_00->nodeFlags = control_00->nodeFlags | UI_NODE_HAS_KEYBOARD_FOCUS;
    }
    (*control_00->vtable->nonRightPress)(wheelDelta,pointerY,pointerX,control_00);
    if (extraout_EDX == g_UiKeyboardFocusNode) {
      g_UiKeyboardFocusNode = control;
      extraout_EDX->nodeFlags = extraout_EDX->nodeFlags & ~UI_NODE_HAS_KEYBOARD_FOCUS;
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
void UiFocusProxyControl_ForwardNonRightReleaseToChild
               (UiPointerWheelDelta wheelDelta,UiPixelCoordinate pointerY,UiPixelCoordinate pointerX
               ,UiNodeBase *control)

{
  UiNodeBase *control_00;
  UiNodeBase *extraout_EDX;
  
  control_00 = control[1].firstChild;
  if (control_00 != (UiNodeBase *)0x0) {
    if (control == g_UiKeyboardFocusNode) {
      g_UiKeyboardFocusNode = control_00;
      control_00->nodeFlags = control_00->nodeFlags | UI_NODE_HAS_KEYBOARD_FOCUS;
    }
    (*control_00->vtable->nonRightRelease)(wheelDelta,pointerY,pointerX,control_00);
    if (extraout_EDX == g_UiKeyboardFocusNode) {
      g_UiKeyboardFocusNode = control;
      extraout_EDX->nodeFlags = extraout_EDX->nodeFlags & ~UI_NODE_HAS_KEYBOARD_FOCUS;
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
void UiFocusProxyControl_ForwardRightPressToChild
               (UiPointerWheelDelta wheelDelta,UiPixelCoordinate pointerY,UiPixelCoordinate pointerX
               ,UiNodeBase *control)

{
  UiNodeBase *extraout_EDX;
  UiNodeBase *control_00;
  
  control_00 = control[1].firstChild;
  if (control_00 != (UiNodeBase *)0x0) {
    if (control == g_UiKeyboardFocusNode) {
      g_UiKeyboardFocusNode = control_00;
      control_00->nodeFlags = control_00->nodeFlags | UI_NODE_HAS_KEYBOARD_FOCUS;
    }
    if (control != control_00->parent) {
      (*control_00->vtable->rightPress)(wheelDelta,pointerY,pointerX,control_00);
      control_00 = extraout_EDX;
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
void UiFocusProxyControl_ForwardRightReleaseToChild
               (UiPointerWheelDelta wheelDelta,UiPixelCoordinate pointerY,UiPixelCoordinate pointerX
               ,UiNodeBase *control)

{
  UiNodeBase *control_00;
  UiNodeBase *extraout_EDX;
  
  control_00 = control[1].firstChild;
  if (control_00 != (UiNodeBase *)0x0) {
    if (control == g_UiKeyboardFocusNode) {
      g_UiKeyboardFocusNode = control_00;
      control_00->nodeFlags = control_00->nodeFlags | UI_NODE_HAS_KEYBOARD_FOCUS;
    }
    (*control_00->vtable->rightRelease)(wheelDelta,pointerY,pointerX,control_00);
    if (extraout_EDX == g_UiKeyboardFocusNode) {
      g_UiKeyboardFocusNode = control;
      extraout_EDX->nodeFlags = extraout_EDX->nodeFlags & ~UI_NODE_HAS_KEYBOARD_FOCUS;
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
void UiFocusProxyControl_ForwardNonRightDragToChild
               (UiPointerWheelDelta wheelDelta,UiPixelCoordinate pointerY,UiPixelCoordinate pointerX
               ,UiNodeBase *control)

{
  UiNodeBase *control_00;
  UiNodeBase *extraout_EDX;
  
  control_00 = control[1].firstChild;
  if (control_00 != (UiNodeBase *)0x0) {
    if (control == g_UiKeyboardFocusNode) {
      g_UiKeyboardFocusNode = control_00;
      control_00->nodeFlags = control_00->nodeFlags | UI_NODE_HAS_KEYBOARD_FOCUS;
    }
    (*control_00->vtable->nonRightDrag)(wheelDelta,pointerY,pointerX,control_00);
    if (extraout_EDX == g_UiKeyboardFocusNode) {
      g_UiKeyboardFocusNode = control;
      extraout_EDX->nodeFlags = extraout_EDX->nodeFlags & ~UI_NODE_HAS_KEYBOARD_FOCUS;
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
void UiFocusProxyControl_ForwardRightDragToChild
               (UiPointerWheelDelta wheelDelta,UiPixelCoordinate pointerY,UiPixelCoordinate pointerX
               ,UiNodeBase *control)

{
  UiNodeBase *control_00;
  UiNodeBase *extraout_EDX;
  
  control_00 = control[1].firstChild;
  if (control_00 != (UiNodeBase *)0x0) {
    if (control == g_UiKeyboardFocusNode) {
      g_UiKeyboardFocusNode = control_00;
      control_00->nodeFlags = control_00->nodeFlags | UI_NODE_HAS_KEYBOARD_FOCUS;
    }
    (*control_00->vtable->rightDrag)(wheelDelta,pointerY,pointerX,control_00);
    if (extraout_EDX == g_UiKeyboardFocusNode) {
      g_UiKeyboardFocusNode = control;
      extraout_EDX->nodeFlags = extraout_EDX->nodeFlags & ~UI_NODE_HAS_KEYBOARD_FOCUS;
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
GraphicsCursorFrameIndex
UiFocusProxyControl_ForwardPointerMoveToChild
          (UiPixelCoordinate pointerY,UiPixelCoordinate pointerX,UiNodeBase *control)

{
  UiNodeBase *control_00;
  GraphicsCursorFrameIndex GVar1;
  GraphicsCursorFrameIndex extraout_EAX;
  UiNodeBase *extraout_EDX;
  
  GVar1 = 0;
  control_00 = control[1].firstChild;
  if (control_00 != (UiNodeBase *)0x0) {
    if (control == g_UiKeyboardFocusNode) {
      g_UiKeyboardFocusNode = control_00;
      control_00->nodeFlags = control_00->nodeFlags | UI_NODE_HAS_KEYBOARD_FOCUS;
    }
    (*control_00->vtable->pointerMove)(pointerY,pointerX,control_00);
    if (extraout_EDX == g_UiKeyboardFocusNode) {
      g_UiKeyboardFocusNode = control;
      extraout_EDX->nodeFlags = extraout_EDX->nodeFlags & ~UI_NODE_HAS_KEYBOARD_FOCUS;
    }
    UiNode_InvalidateRoot(control);
    GVar1 = extraout_EAX;
  }
  return GVar1;
}

/* Address: 0x004B9C50.
   Ownership: ui/controls/input.
   Purpose: Binary entry is anchored by g_UiNodeVtable_004B9530[11]@004B9530.
   Cross-module calls: UiContainer_HitTestChildren [ui/controls/layout].
*/
UiNodeBase *
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
void UiFocusProxyControl_ForwardTickToChild(UiNodeBase *control)

{
  UiNodeBase *control_00;
  UiNodeBase *extraout_EDX;
  
  control_00 = control[1].firstChild;
  if (control_00 != (UiNodeBase *)0x0) {
    if (control == g_UiKeyboardFocusNode) {
      g_UiKeyboardFocusNode = control_00;
      control_00->nodeFlags = control_00->nodeFlags | UI_NODE_HAS_KEYBOARD_FOCUS;
    }
    (*control_00->vtable->tick)(control_00);
    if (extraout_EDX == g_UiKeyboardFocusNode) {
      g_UiKeyboardFocusNode = control;
      extraout_EDX->nodeFlags = extraout_EDX->nodeFlags & ~UI_NODE_HAS_KEYBOARD_FOCUS;
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
GraphicsCursorFrameIndex
UiImageControl_PointerMove
          (UiPixelCoordinate pointerY,UiPixelCoordinate pointerX,UiImageControl *control)

{
  UiImageControl *control_00;
  GraphicsCursorFrameIndex GVar1;
  bool opaquePixelHit;
  
  if (((control->selectable).base.nodeFlags & UI_NODE_SUPPRESSED) == 0) {
    opaquePixelHit = false;
    if (((control->selectable).stateFlags & 0x40) == 0) {
      (*g_GraphicsTextureSourceTestOpaquePixel)
                (pointerY,pointerX,(control->selectable).base.top,(control->selectable).base.left,
                 control->normalSubresource,control->textureSource);
      if (opaquePixelHit) {
        return 0;
      }
    }
    else {
      (*g_GraphicsTextureSourceTestOpaquePixel)
                (pointerY,pointerX,(control->selectable).base.top,(control->selectable).base.left,
                 control->alternateSubresource,control->textureSource);
      if (opaquePixelHit) {
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
void UiSelectionGeometryControl_DrawClipped
               (int clipTop,int clipLeft,int clipBottom,int clipRight,UiNodeBase *control)

{
  uint *puVar1;
  int iVar2;
  int iVar3;
  longlong lVar4;
  longlong lVar5;
  longlong lVar6;
  char cVar7;
  int iVar11;
  uint uVar12;
  uint uVar13;
  uint uVar14;
  int iVar15;
  int iVar16;
  int iVar17;
  int iVar18;
  int iVar19;
  byte *pbVar20;
  bool bVar21;
  undefined8 uVar22;
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
  char cVar8;
  char cVar9;
  char cVar10;
  undefined4 uVar37;
  
  if (clipRight < control->left) {
    clipRight = control->left;
  }
  if (clipBottom < control->top) {
    clipBottom = control->top;
  }
  if (control->right < clipLeft) {
    clipLeft = control->right;
  }
  if (control->bottom < clipTop) {
    clipTop = control->bottom;
  }
  iVar18 = clipLeft - clipRight;
  if (((iVar18 != 0 && clipRight <= clipLeft) &&
      (iVar16 = clipTop - clipBottom, iVar16 != 0 && clipBottom <= clipTop)) &&
     (control[1].top != 0)) {
    lVar4 = (longlong)(int)control[1].vtable * (longlong)g_FixedCosQ28[control[1].left];
    iVar17 = -((int)((ulonglong)lVar4 >> 0x20) << 4 | (uint)lVar4 >> 0x1c);
    lVar4 = (longlong)(int)control[1].vtable * (longlong)g_FixedSinQ28[control[1].left];
    uVar12 = (int)((ulonglong)lVar4 >> 0x20) << 4 | (uint)lVar4 >> 0x1c;
    lVar4 = (longlong)(int)uVar12 * 0x1c6e9c;
    lVar5 = (longlong)iVar17 * -0x20c8cc;
    uVar13 = (int)((ulonglong)lVar5 >> 0x20) << 0xb | (uint)lVar5 >> 0x15;
    lVar5 = (longlong)iVar17 * 0x1c6e9c;
    lVar6 = (longlong)(int)-uVar12 * -0x20c8cc;
    uVar14 = (int)((ulonglong)lVar6 >> 0x20) << 0xb | (uint)lVar6 >> 0x15;
    uVar12 = ((int)((ulonglong)lVar5 >> 0x20) << 0xc | (uint)lVar5 >> 0x14) - uVar14;
    iVar29 = uVar14 * 2;
    uVar27 = (ulonglong)uVar12;
    uVar34 = (ulonglong)
             ((int)control[1].firstChild -
             (uVar12 * ((control->top + control->bottom >> 1) - clipBottom) +
             (((int)((ulonglong)lVar4 >> 0x20) << 0xc | (uint)lVar4 >> 0x14) - uVar13) *
             ((control->left + control->right >> 1) - clipRight)));
    iVar19 = (int)control[1].parent -
             (iVar29 * ((control->top + control->bottom >> 1) - clipBottom) +
             uVar13 * 2 * ((control->left + control->right >> 1) - clipRight));
    uVar12 = control[1].top;
    iVar17 = *(int *)(uVar12 + 0xb8);
    if (*(int *)(iVar17 + 8 + uVar12) < 0) {
      iVar2 = *(int *)(iVar17 + 0x18 + uVar12);
      iVar3 = *(int *)(iVar17 + 0x1c + uVar12);
      puVar1 = (uint *)(iVar17 + 0xc + uVar12);
      bVar21 = CARRY4(uVar12,*puVar1);
      iVar17 = uVar12 + *puVar1;
      uVar22 = (*g_GraphicsFramebufferBeginAccess)();
      if (!bVar21) {
        iVar23 = (int)((ulonglong)uVar22 >> 0x20);
        iVar28 = (int)(uVar27 >> 0x20);
        uVar12 = (uint)uVar34;
        iVar11 = (int)(uVar34 >> 0x20);
        clipTop = iVar16;
        iStack_2c = iVar18;
        if (g_FramebufferAccess->bytesPerPixel == SOFTWARE_FRAMEBUFFER_PIXEL_BYTES_16BIT) {
          pbVar20 = g_FramebufferAccess->pixels +
                    g_FramebufferRowStrideBytes * clipBottom + clipRight * 2;
          uVar35 = iVar11 + iVar19;
          uVar13 = uVar12;
          uVar14 = uVar35;
          pbStack_28 = pbVar20;
          do {
            do {
              sourcePixelSample0 = 0;
              sourcePixelSample0 = 0;
              sourcePixelSample1 = 0;
              sourcePixelSample1 = 0;
              sourcePixelSample2 = 0;
              sourcePixelSample2 = 0;
              sourcePixelSample3 = 0;
              iVar15 = (int)uVar35 >> 0xc;
              iVar11 = (int)uVar12 >> 0xc;
              iVar19 = iVar2 * iVar15 + iVar11;
              iVar16 = iVar11 + 1;
              if (iVar15 < iVar3) {
                sourcePixelSample0 = sourcePixelSample0;
                sourcePixelSample1 = sourcePixelSample1;
                if ((-1 < iVar15) && (iVar11 < iVar2)) {
                  sourcePixelSample4 = sourcePixelSample0;
                  if (-1 < iVar11) {
                    sourcePixelSample4 = *(PackedArgb32 *)(iVar17 + iVar19 * 4);
                  }
                  sourcePixelSample0 = sourcePixelSample4;
                  if ((-1 < iVar16) && (iVar16 < iVar2)) {
                    sourcePixelSample1 = *(PackedArgb32 *)(iVar17 + 4 + iVar19 * 4);
                  }
                }
                if (((-1 < iVar15 + 1) && (iVar15 + 1 < iVar3)) && (iVar11 < iVar2)) {
                  sourcePixelSample2 = sourcePixelSample2;
                  if (-1 < iVar11) {
                    sourcePixelSample2 = *(PackedArgb32 *)(iVar17 + (iVar19 + iVar2) * 4);
                  }
                  if ((-1 < iVar16) && (iVar16 < iVar2)) {
                    sourcePixelSample3 = *(PackedArgb32 *)(iVar17 + 4 + (iVar19 + iVar2) * 4);
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
              iVar19 = (int)(uVar35 & 0xfff) >> 4;
              iVar16 = (int)(uVar12 & 0xfff) >> 4;
              mm7PackedValue0 =
                   pmulhw(CONCAT26(uVar41 >> 2,
                                   CONCAT24((ushort)CONCAT31(CONCAT21(uVar41,uVar47),uVar47) >> 2,
                                            CONCAT22(CONCAT11(uVar46,uVar46) >> 2,
                                                     CONCAT11((char)sourcePixelSample0,
                                                              (char)sourcePixelSample0) >> 2))),
                          *(undefined8 *)(iVar16 * 8 + 0x420720));
              mm5PackedValue0 =
                   pmulhw(CONCAT26(uVar48 >> 2,
                                   CONCAT24((ushort)CONCAT31(CONCAT21(uVar48,uVar40),uVar40) >> 2,
                                            CONCAT22(CONCAT11(uVar39,uVar39) >> 2,
                                                     CONCAT11((char)sourcePixelSample1,
                                                              (char)sourcePixelSample1) >> 2))),
                          *(undefined8 *)(iVar16 * 8 + 0x41f720));
              mm3PackedValue0 =
                   pmulhw(CONCAT26(uVar49 >> 2,
                                   CONCAT24((ushort)CONCAT31(CONCAT21(uVar49,uVar33),uVar33) >> 2,
                                            CONCAT22(CONCAT11(uVar32,uVar32) >> 2,
                                                     CONCAT11((char)sourcePixelSample2,
                                                              (char)sourcePixelSample2) >> 2))),
                          *(undefined8 *)(iVar16 * 8 + 0x420720));
              mm1PackedValue0 =
                   pmulhw(CONCAT26(uVar50 >> 2,
                                   CONCAT24((ushort)CONCAT31(CONCAT21(uVar50,uVar26),uVar26) >> 2,
                                            CONCAT22(CONCAT11(uVar25,uVar25) >> 2,
                                                     CONCAT11((char)sourcePixelSample3,
                                                              (char)sourcePixelSample3) >> 2))),
                          *(undefined8 *)(iVar16 * 8 + 0x41f720));
              uVar44 = pmulhw(CONCAT26((short)((ulonglong)mm7PackedValue0 >> 0x30) +
                                       (short)((ulonglong)mm5PackedValue0 >> 0x30),
                                       CONCAT24((short)((ulonglong)mm7PackedValue0 >> 0x20) +
                                                (short)((ulonglong)mm5PackedValue0 >> 0x20),
                                                CONCAT22((short)((ulonglong)mm7PackedValue0 >> 0x10)
                                                         + (short)((ulonglong)mm5PackedValue0 >>
                                                                  0x10),
                                                         (short)mm7PackedValue0 +
                                                         (short)mm5PackedValue0))),
                              *(undefined8 *)(iVar19 * 8 + 0x420720));
              uVar31 = pmulhw(CONCAT26((short)((ulonglong)mm3PackedValue0 >> 0x30) +
                                       (short)((ulonglong)mm1PackedValue0 >> 0x30),
                                       CONCAT24((short)((ulonglong)mm3PackedValue0 >> 0x20) +
                                                (short)((ulonglong)mm1PackedValue0 >> 0x20),
                                                CONCAT22((short)((ulonglong)mm3PackedValue0 >> 0x10)
                                                         + (short)((ulonglong)mm1PackedValue0 >>
                                                                  0x10),
                                                         (short)mm3PackedValue0 +
                                                         (short)mm1PackedValue0))),
                              *(undefined8 *)(iVar19 * 8 + 0x41f720));
              uVar41 = (ushort)((short)uVar44 + (short)uVar31) >> 2;
              uVar48 = (ushort)((short)((ulonglong)uVar44 >> 0x10) +
                               (short)((ulonglong)uVar31 >> 0x10)) >> 2;
              uVar49 = (ushort)((short)((ulonglong)uVar44 >> 0x20) +
                               (short)((ulonglong)uVar31 >> 0x20)) >> 2;
              uVar50 = (ushort)((short)((ulonglong)uVar44 >> 0x30) +
                               (short)((ulonglong)uVar31 >> 0x30)) >> 2;
              cVar7 = (uVar41 != 0) * (uVar41 < 0x100) * (char)uVar41 - (0xff < uVar41);
              cVar8 = (uVar48 != 0) * (uVar48 < 0x100) * (char)uVar48 - (0xff < uVar48);
              cVar9 = (uVar49 != 0) * (uVar49 < 0x100) * (char)uVar49 - (0xff < uVar49);
              cVar10 = (uVar50 != 0) * (uVar50 < 0x100) * (char)uVar50 - (0xff < uVar50);
              uVar41 = CONCAT11(cVar10,cVar10);
              uVar31 = pmaddwd(CONCAT26(uVar41 >> 4,
                                        CONCAT24((ushort)CONCAT31(CONCAT21(uVar41,cVar9),cVar9) >> 4
                                                 ,CONCAT22(CONCAT11(cVar8,cVar8) >> 4,
                                                           CONCAT11(cVar7,cVar7) >> 4))) &
                               (ulonglong)g_SoftwarePixelMmxConstants.quantizeMasksQ12,
                               g_SoftwarePixelMmxConstants.packWeights);
              *(short *)pbVar20 =
                   (short)((ulonglong)uVar31 >> 0x28) + (short)((ulonglong)uVar31 >> 8);
              uVar12 = uVar12 + (int)uVar22;
              uVar35 = uVar35 + iVar23 + extraout_MM1_Da;
              pbVar20 = pbVar20 + 2;
              iStack_2c = iStack_2c + -1;
            } while (iStack_2c != 0);
            uVar12 = uVar13 + (int)uVar27;
            uVar35 = uVar14 + iVar28 + iVar29;
            pbVar20 = pbStack_28 + g_FramebufferRowStrideBytes;
            clipTop = clipTop + -1;
            uVar13 = uVar12;
            uVar14 = uVar35;
            iStack_2c = iVar18;
            pbStack_28 = pbVar20;
          } while (clipTop != 0);
        }
        else {
          pbVar20 = g_FramebufferAccess->pixels +
                    g_FramebufferRowStrideBytes * clipBottom + clipRight * 4;
          uVar35 = iVar11 + iVar19;
          uVar13 = uVar12;
          uVar14 = uVar35;
          pbStack_28 = pbVar20;
          do {
            do {
              uVar43 = 0;
              uVar42 = 0;
              uVar37 = 0;
              uVar36 = 0;
              uVar30 = 0;
              uVar24 = 0;
              iVar15 = (int)uVar35 >> 0xc;
              iVar11 = (int)uVar12 >> 0xc;
              iVar19 = iVar2 * iVar15 + iVar11;
              iVar16 = iVar11 + 1;
              if (iVar15 < iVar3) {
                uVar42 = uVar43;
                uVar36 = uVar37;
                if ((-1 < iVar15) && (iVar11 < iVar2)) {
                  if (-1 < iVar11) {
                    uVar43 = *(undefined4 *)(iVar17 + iVar19 * 4);
                  }
                  uVar42 = uVar43;
                  if ((-1 < iVar16) && (iVar16 < iVar2)) {
                    uVar36 = *(undefined4 *)(iVar17 + 4 + iVar19 * 4);
                  }
                }
                if (((-1 < iVar15 + 1) && (iVar15 + 1 < iVar3)) && (iVar11 < iVar2)) {
                  uVar30 = 0;
                  if (-1 < iVar11) {
                    uVar30 = *(undefined4 *)(iVar17 + (iVar19 + iVar2) * 4);
                  }
                  if ((-1 < iVar16) && (iVar16 < iVar2)) {
                    uVar24 = *(undefined4 *)(iVar17 + 4 + (iVar19 + iVar2) * 4);
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
              iVar19 = (int)(uVar35 & 0xfff) >> 4;
              iVar16 = (int)(uVar12 & 0xfff) >> 4;
              uVar45 = pmulhw(CONCAT26(uVar41 >> 2,
                                       CONCAT24((ushort)CONCAT31(CONCAT21(uVar41,uVar47),uVar47) >>
                                                2,CONCAT22(CONCAT11(uVar46,uVar46) >> 2,
                                                           CONCAT11((char)uVar42,(char)uVar42) >> 2)
                                               )),*(undefined8 *)(iVar16 * 8 + 0x420720));
              uVar38 = pmulhw(CONCAT26(uVar48 >> 2,
                                       CONCAT24((ushort)CONCAT31(CONCAT21(uVar48,uVar40),uVar40) >>
                                                2,CONCAT22(CONCAT11(uVar39,uVar39) >> 2,
                                                           CONCAT11((char)uVar36,(char)uVar36) >> 2)
                                               )),*(undefined8 *)(iVar16 * 8 + 0x41f720));
              uVar44 = pmulhw(CONCAT26(uVar49 >> 2,
                                       CONCAT24((ushort)CONCAT31(CONCAT21(uVar49,uVar33),uVar33) >>
                                                2,CONCAT22(CONCAT11(uVar32,uVar32) >> 2,
                                                           CONCAT11((char)uVar30,(char)uVar30) >> 2)
                                               )),*(undefined8 *)(iVar16 * 8 + 0x420720));
              uVar31 = pmulhw(CONCAT26(uVar50 >> 2,
                                       CONCAT24((ushort)CONCAT31(CONCAT21(uVar50,uVar26),uVar26) >>
                                                2,CONCAT22(CONCAT11(uVar25,uVar25) >> 2,
                                                           CONCAT11((char)uVar24,(char)uVar24) >> 2)
                                               )),*(undefined8 *)(iVar16 * 8 + 0x41f720));
              uVar38 = pmulhw(CONCAT26((short)((ulonglong)uVar45 >> 0x30) +
                                       (short)((ulonglong)uVar38 >> 0x30),
                                       CONCAT24((short)((ulonglong)uVar45 >> 0x20) +
                                                (short)((ulonglong)uVar38 >> 0x20),
                                                CONCAT22((short)((ulonglong)uVar45 >> 0x10) +
                                                         (short)((ulonglong)uVar38 >> 0x10),
                                                         (short)uVar45 + (short)uVar38))),
                              *(undefined8 *)(iVar19 * 8 + 0x420720));
              uVar31 = pmulhw(CONCAT26((short)((ulonglong)uVar44 >> 0x30) +
                                       (short)((ulonglong)uVar31 >> 0x30),
                                       CONCAT24((short)((ulonglong)uVar44 >> 0x20) +
                                                (short)((ulonglong)uVar31 >> 0x20),
                                                CONCAT22((short)((ulonglong)uVar44 >> 0x10) +
                                                         (short)((ulonglong)uVar31 >> 0x10),
                                                         (short)uVar44 + (short)uVar31))),
                              *(undefined8 *)(iVar19 * 8 + 0x41f720));
              uVar41 = (ushort)((short)uVar38 + (short)uVar31) >> 2;
              uVar48 = (ushort)((short)((ulonglong)uVar38 >> 0x10) +
                               (short)((ulonglong)uVar31 >> 0x10)) >> 2;
              uVar49 = (ushort)((short)((ulonglong)uVar38 >> 0x20) +
                               (short)((ulonglong)uVar31 >> 0x20)) >> 2;
              uVar50 = (ushort)((short)((ulonglong)uVar38 >> 0x30) +
                               (short)((ulonglong)uVar31 >> 0x30)) >> 2;
              *(uint *)pbVar20 =
                   CONCAT13((uVar50 != 0) * (uVar50 < 0x100) * (char)uVar50 - (0xff < uVar50),
                            CONCAT12((uVar49 != 0) * (uVar49 < 0x100) * (char)uVar49 -
                                     (0xff < uVar49),
                                     CONCAT11((uVar48 != 0) * (uVar48 < 0x100) * (char)uVar48 -
                                              (0xff < uVar48),
                                              (uVar41 != 0) * (uVar41 < 0x100) * (char)uVar41 -
                                              (0xff < uVar41))));
              uVar12 = uVar12 + (int)uVar22;
              uVar35 = uVar35 + iVar23 + extraout_MM1_Da;
              pbVar20 = pbVar20 + 4;
              iStack_2c = iStack_2c + -1;
            } while (iStack_2c != 0);
            uVar12 = uVar13 + (int)uVar27;
            uVar35 = uVar14 + iVar28 + iVar29;
            pbVar20 = pbStack_28 + g_FramebufferRowStrideBytes;
            clipTop = clipTop + -1;
            uVar13 = uVar12;
            uVar14 = uVar35;
            iStack_2c = iVar18;
            pbStack_28 = pbVar20;
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
void UiSelectionGeometryControl_ConvertPointerAndEnqueueAction
               (UiPointerWheelDelta wheelDelta,UiPixelCoordinate pointerY,UiPixelCoordinate pointerX
               ,UiNodeBase *control)

{
  int iVar1;
  int iVar2;
  int iVar3;
  longlong lVar4;
  longlong lVar5;
  longlong lVar6;
  uint uVar7;
  uint uVar8;
  
  lVar4 = (longlong)(int)control[1].vtable * (longlong)g_FixedCosQ28[control[1].left];
  iVar1 = -((int)((ulonglong)lVar4 >> 0x20) << 4 | (uint)lVar4 >> 0x1c);
  lVar4 = (longlong)(int)control[1].vtable * (longlong)g_FixedSinQ28[control[1].left];
  uVar7 = (int)((ulonglong)lVar4 >> 0x20) << 4 | (uint)lVar4 >> 0x1c;
  lVar4 = (longlong)(int)uVar7 * 0x1c6e9c;
  lVar5 = (longlong)iVar1 * -0x20c8cc;
  uVar8 = (int)((ulonglong)lVar5 >> 0x20) << 0xb | (uint)lVar5 >> 0x15;
  lVar5 = (longlong)iVar1 * 0x1c6e9c;
  lVar6 = (longlong)(int)-uVar7 * -0x20c8cc;
  uVar7 = (int)((ulonglong)lVar6 >> 0x20) << 0xb | (uint)lVar6 >> 0x15;
  iVar1 = control->left;
  iVar2 = control->right;
  iVar3 = control->top;
  control[1].bottom =
       (int)control[1].firstChild -
       ((((int)((ulonglong)lVar5 >> 0x20) << 0xc | (uint)lVar5 >> 0x14) - uVar7) *
        ((control->top + control->bottom >> 1) - pointerY) +
       (((int)((ulonglong)lVar4 >> 0x20) << 0xc | (uint)lVar4 >> 0x14) - uVar8) *
       ((control->left + control->right >> 1) - pointerX));
  control[1].leftOffset =
       (int)control[1].parent +
       (-(uVar7 * 2 * ((iVar3 + control->bottom >> 1) - pointerY)) -
       uVar8 * 2 * ((iVar1 + iVar2 >> 1) - pointerX));
  UiActionQueue_Enqueue(control[1].right,control);
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
void UiPointer_DispatchLeftPress
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
  UiImageControl *extraout_EAX;
  UiImageControl *extraout_EAX_00;
  int iVar7;
  int extraout_ECX;
  int iVar8;
  int extraout_EDX;
  UiRootNode *root;
  bool bVar9;
  
  node = g_UiImageControlHoverTarget;
  pUVar5 = g_UiRootNode;
  if (g_UiPointerCaptureButton == UI_POINTER_CAPTURE_NONE) {
    iVar7 = pointerX;
    iVar8 = pointerY;
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
          node = extraout_EAX_00;
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
           ((root->base).left <= iVar7)) && ((root->base).top <= iVar8)) &&
         ((iVar7 < (root->base).right && (iVar8 < (root->base).bottom)))) {
        node = (UiImageControl *)(*((root->base).vtable)->hitTest)(pointerY,pointerX,&root->base);
        bVar9 = root < pUVar5;
        if ((root != pUVar5) && (UiRootStack_BringToFront(root), node = extraout_EAX, bVar9)) {
          return;
        }
        if (node == (UiImageControl *)0xffffffff) {
          return;
        }
        goto UiPointer_DispatchLeftPressToCapturedTarget;
      }
      ppUVar3 = &root->callbacks;
      root = root->previousRoot;
      bVar9 = false;
      if (((*ppUVar3)->method08 != (UiRootMethod08Callback *)0x0) &&
         ((*(*ppUVar3)->method08)(root), iVar7 = extraout_ECX, iVar8 = extraout_EDX, bVar9)) {
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
void UiPointer_DispatchMiddlePress
               (UiPointerButtonMask buttonMask,UiPointerWheelDelta wheelDelta,
               UiPixelCoordinate pointerY,UiPixelCoordinate pointerX)

{
  UiSelectableStateFlags *pUVar1;
  UiNodeFlags *pUVar2;
  UiRootCallbacks **ppUVar3;
  UiNodeVtable *pUVar4;
  UiRootNode *pUVar5;
  UiNodeBase *pUVar6;
  UiImageControl *node;
  UiImageControl *extraout_EAX;
  UiImageControl *extraout_EAX_00;
  int iVar7;
  int extraout_ECX;
  int iVar8;
  int extraout_EDX;
  UiRootNode *root;
  bool bVar9;
  
  node = g_UiImageControlHoverTarget;
  pUVar5 = g_UiRootNode;
  if (g_UiPointerCaptureButton == UI_POINTER_CAPTURE_NONE) {
    iVar7 = pointerX;
    iVar8 = pointerY;
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
          node = extraout_EAX_00;
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
           ((root->base).left <= iVar7)) && ((root->base).top <= iVar8)) &&
         ((iVar7 < (root->base).right && (iVar8 < (root->base).bottom)))) {
        node = (UiImageControl *)(*((root->base).vtable)->hitTest)(pointerY,pointerX,&root->base);
        bVar9 = root < pUVar5;
        if ((root != pUVar5) && (UiRootStack_BringToFront(root), node = extraout_EAX, bVar9)) {
          return;
        }
        if (node == (UiImageControl *)0xffffffff) {
          return;
        }
        goto UiPointer_DispatchMiddlePressToCapturedTarget;
      }
      ppUVar3 = &root->callbacks;
      root = root->previousRoot;
      bVar9 = false;
      if (((*ppUVar3)->method08 != (UiRootMethod08Callback *)0x0) &&
         ((*(*ppUVar3)->method08)(root), iVar7 = extraout_ECX, iVar8 = extraout_EDX, bVar9)) {
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
void UiPointer_DispatchRightPress
               (UiPointerButtonMask buttonMask,UiPointerWheelDelta wheelDelta,
               UiPixelCoordinate pointerY,UiPixelCoordinate pointerX)

{
  UiSelectableStateFlags *pUVar1;
  UiRootCallbacks **ppUVar2;
  UiNodeVtable *pUVar3;
  UiRootNode *pUVar4;
  UiNodeBase *node;
  UiNodeBase *extraout_EAX;
  UiNodeBase *extraout_EAX_00;
  int iVar5;
  int extraout_ECX;
  int iVar6;
  int extraout_EDX;
  UiRootNode *root;
  bool bVar7;
  
  pUVar4 = g_UiRootNode;
  if (g_UiPointerCaptureButton == UI_POINTER_CAPTURE_NONE) {
    iVar5 = pointerX;
    iVar6 = pointerY;
    root = pUVar4;
    if (g_UiImageControlHoverTarget != (UiImageControl *)0x0) {
      pUVar1 = &(g_UiImageControlHoverTarget->selectable).stateFlags;
      *pUVar1 = *pUVar1 & 0xfffff9fc;
    }
    while (root != (UiRootNode *)0xffffffff) {
      if (((((root->rootFlags & UI_ROOT_DISABLE_POINTER_HIT_TEST) == 0) &&
           ((root->base).left <= iVar5)) && ((root->base).top <= iVar6)) &&
         ((iVar5 < (root->base).right && (iVar6 < (root->base).bottom)))) {
        node = (*((root->base).vtable)->hitTest)(pointerY,pointerX,&root->base);
        bVar7 = root < pUVar4;
        if ((root != pUVar4) && (UiRootStack_BringToFront(root), node = extraout_EAX, bVar7)) {
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
          node = extraout_EAX_00;
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
      bVar7 = false;
      if (((*ppUVar2)->method08 != (UiRootMethod08Callback *)0x0) &&
         ((*(*ppUVar2)->method08)(root), iVar5 = extraout_ECX, iVar6 = extraout_EDX, bVar7)) {
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
void __cdecl UiKeyboardFocus_MoveNext(void)

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
void UiPointer_DispatchMotionAndWheel
               (UiPointerWheelDelta wheelDelta,UiPixelCoordinate pointerY,UiPixelCoordinate pointerX
               )

{
  UiNodeVtable *pUVar1;
  UiRootCloseCallbackCf *pUVar2;
  UiPixelCoordinate pointerX_00;
  UiPixelCoordinate pointerX_01;
  int extraout_ECX;
  UiPixelCoordinate pointerY_00;
  UiRootNode *arg0;
  UiNodeBase *pUVar4;
  undefined8 uVar3;
  
  pUVar4 = g_UiPointerCaptureTarget;
  if (g_UiHoverSelectionRecord != (UiCommandRuntimeRecordPrefix *)0x0) {
    g_UiHoverSelectionRecord = (UiCommandRuntimeRecordPrefix *)0x0;
    InGameSelectionDetailPanel_Rebuild();
  }
  UiTooltip_UpdateHoverTarget(pointerY,pointerX);
  arg0 = g_UiRootNode;
  if (pUVar4 != (UiNodeBase *)0xffffffff) {
    if (g_UiPointerCaptureButton == UI_POINTER_CAPTURE_RIGHT) {
      (*pUVar4->vtable->rightDrag)(wheelDelta,pointerY,pointerX,pUVar4);
    }
    else {
      (*pUVar4->vtable->nonRightDrag)(wheelDelta,pointerY,pointerX,pUVar4);
    }
    return;
  }
  while( true ) {
    if (arg0 == (UiRootNode *)0xffffffff) {
      return;
    }
    if (((((arg0->base).left <= pointerX) && ((arg0->base).top <= pointerY)) &&
        (pointerX < (arg0->base).right)) && (pointerY < (arg0->base).bottom)) {
      _pUVar4 = (*((arg0->base).vtable)->hitTest)(pointerY,pointerX,&arg0->base);
      pUVar4 = SUB84(_pUVar4,0);
      if (pUVar4 == (UiNodeBase *)0xffffffff) {
        return;
      }
      pUVar1 = pUVar4->vtable;
      (*pUVar1->pointerMove)((UiPixelCoordinate)((ulonglong)_pUVar4 >> 0x20),pointerX_00,pUVar4);
      if (wheelDelta == 0) {
        return;
      }
      (*pUVar1->pointerWheel)(wheelDelta,pointerY_00,pointerX_01,pUVar4);
      return;
    }
    pUVar2 = arg0->callbacks[1].closeCf;
    if (pUVar2 == (UiRootCloseCallbackCf *)0x0) {
      return;
    }
    uVar3 = (*pUVar2)(arg0);
    pointerY = (UiPixelCoordinate)((ulonglong)uVar3 >> 0x20);
    if (-1 < (int)uVar3) break;
    pointerX = extraout_ECX;
    arg0 = arg0->previousRoot;
  }
  return;
}

/* Address: 0x004B09F0.
   Ownership: ui/controls/input.
   Purpose: Forwards pointerWheel to node->parent when one exists.
*/
void UiNode_ForwardPointerWheelToParent
               (UiPointerWheelDelta wheelDelta,UiPixelCoordinate pointerY,UiPixelCoordinate pointerX
               ,UiNodeBase *control)

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
void UiNode_DefaultKeyboardEventMoveFocusNextCf
               (UiKeyboardStateMask keyboardStateMask,UiKeyboardEventCode keyCode,
               UiNodeBase *control)

{
  return;
}

/* Address: 0x004AFF60.
   Ownership: ui/controls/input.
   Purpose: Clears nodeFlags bit 0x04 on the old focus node, stores the new focus node, sets bit 0x04 on it, and
   invalidates the UI.
   Cross-module calls: UiRootStack_InvalidateAll [ui/controls/layout].
*/
void UiKeyboardFocus_Set(UiNodeBase *node)

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

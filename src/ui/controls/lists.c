#include <thandor/ui/controls/lists.h>

/* Implementation ownership: ui/controls/lists. */

/* Address: 0x004BBE60.
   Ownership: ui/controls/lists.
   Purpose: Handles ui timed list control handle keyboard navigation carry-flag result.
   Local calls: UiScrollableControl_QueryContentSizeRegs, UiTimedListTree_CountRecordArrayAndNestedChildren,
   UiScrollableControl_ClampOffsetsToViewport.
   Cross-module calls: UiNode_DefaultKeyboardEventMoveFocusNextCf [ui/controls/input].
*/
void UiTimedListControl_HandleKeyboardNavigationCf
               (dword keyCode,dword stateMask,UiNodeBase *control)

{
  uint *puVar1;
  uint *puVar2;
  UiFrameDelayFrames UVar3;
  UiNodeBase *pUVar4;
  int *piVar5;
  uint *puVar6;
  int iVar7;
  dword dVar8;
  UiNodeBase **ppUVar9;
  uint uVar10;
  int iVar11;
  int extraout_ECX;
  int iVar12;
  int iVar13;
  UiScrollableContentDimensionsEdxEax8 UVar14;
  
  iVar13 = 1;
  iVar7 = control[1].top;
  if ((stateMask & 0xffff0000) == 0) {
UiTimedListKeyboard_DelegateUnhandledEvent:
    UiNode_DefaultKeyboardEventMoveFocusNextCf(keyCode,stateMask,control);
    return;
  }
  if (stateMask == 0x10010) {
    control[1].top = (sdword)&(control[1].firstChild)->left;
    goto UiTimedListKeyboard_CommitSelectionAndScheduleAction;
  }
  if (stateMask == 0x10018) {
    pUVar4 = control[1].firstChild;
    do {
      ppUVar9 = &pUVar4->nextSibling + (int)pUVar4->nextSibling * 4;
      control[1].top = (sdword)ppUVar9;
      if ((((uint)ppUVar9[3] & 1) == 0) || (((uint)ppUVar9[3] & 2) == 0)) break;
      pUVar4 = ppUVar9[2];
    } while (pUVar4 != (UiNodeBase *)0x0);
    goto UiTimedListKeyboard_CommitSelectionAndScheduleAction;
  }
  if (stateMask == 0x10012) {
    UVar14 = UiScrollableControl_QueryContentSizeRegs((UiScrollableControl *)control->parent);
    iVar11 = (int)((UVar14 >> 0x20) / ZEXT48(control[1].vtable));
    iVar13 = iVar11 + -1;
    if (iVar11 < 1) {
      iVar13 = 1;
    }
UiTimedListKeyboard_MoveSelectionBackwardLoop:
    do {
      iVar11 = control[1].top;
      piVar5 = (int *)(iVar11 + -0x10);
      control[1].top = (sdword)piVar5;
      if ((*(uint *)(iVar11 + -4) & 0x80000000) == 0) {
        while ((((piVar5[3] & 1U) != 0 && ((piVar5[3] & 2U) != 0)) &&
               (piVar5 = (int *)piVar5[2], piVar5 != (int *)0x0))) {
          piVar5 = piVar5 + *piVar5 * 4;
          control[1].top = (sdword)piVar5;
        }
      }
      else {
        iVar12 = *(int *)(iVar11 + -8);
        control[1].top = iVar11;
        if (iVar12 != 0) {
          control[1].top = iVar12;
        }
      }
      iVar13 = iVar13 + -1;
    } while (iVar13 != 0);
  }
  else {
    if (stateMask == 0x1001a) {
      UVar14 = UiScrollableControl_QueryContentSizeRegs((UiScrollableControl *)control->parent);
      iVar11 = (int)((UVar14 >> 0x20) / ZEXT48(control[1].vtable));
      iVar13 = iVar11 + -1;
      if (iVar11 < 1) {
        iVar13 = 1;
      }
    }
    else {
      if (stateMask == 0x10011) goto UiTimedListKeyboard_MoveSelectionBackwardLoop;
      if (stateMask != 0x10019) {
        if (stateMask == 0x10014) {
          if ((*(uint *)(iVar7 + 0xc) & 1) == 0) {
            return;
          }
          if ((*(uint *)(iVar7 + 0xc) & 2) == 0) {
            return;
          }
          if (control[1].right == 0) {
            return;
          }
          (*(code *)control[1].right)(iVar7,control);
          return;
        }
        if (stateMask == 0x10016) {
          if ((*(uint *)(iVar7 + 0xc) & 1) == 0) {
            return;
          }
          if ((*(uint *)(iVar7 + 0xc) & 2) != 0) {
            return;
          }
          if (control[1].right == 0) {
            return;
          }
          (*(code *)control[1].right)(iVar7,control);
          return;
        }
        goto UiTimedListKeyboard_DelegateUnhandledEvent;
      }
    }
    do {
      puVar1 = (uint *)control[1].top;
      uVar10 = 0;
      puVar2 = puVar1;
      if ((((puVar1[3] & 1) == 0) || ((puVar1[3] & 2) == 0)) ||
         ((piVar5 = (int *)puVar1[2], piVar5 == (int *)0x0 || (*piVar5 == 0)))) {
        do {
          do {
            puVar6 = puVar2;
            uVar10 = uVar10 + 1;
            puVar2 = puVar6 + -4;
          } while ((puVar6[-1] & 0x80000000) == 0);
          control[1].top = control[1].top + 0x10;
          if (uVar10 < puVar6[-4]) goto UiTimedListKeyboard_ForwardTraversalStepComplete;
          puVar2 = (uint *)puVar6[-2];
          uVar10 = 0;
          control[1].top = (sdword)puVar2;
        } while (puVar2 != (uint *)0x0);
        control[1].top = (sdword)puVar1;
      }
      else {
        control[1].top = (sdword)(piVar5 + 4);
      }
UiTimedListKeyboard_ForwardTraversalStepComplete:
      iVar13 = iVar13 + -1;
    } while (iVar13 != 0);
  }
UiTimedListKeyboard_CommitSelectionAndScheduleAction:
  iVar13 = control[1].top;
  iVar11 = 0;
  if (iVar13 != iVar7) {
    do {
      iVar7 = iVar13 + -0x10;
      iVar12 = iVar11;
      if ((*(uint *)(iVar13 + -4) & 2) != 0) {
        dVar8 = UiTimedListTree_CountRecordArrayAndNestedChildren
                          (*(UiTimedListTreeRecord16 **)(iVar13 + -8));
        iVar12 = extraout_ECX + dVar8;
      }
      iVar13 = iVar7;
      iVar11 = iVar12 + 1;
    } while (((*(uint *)(iVar13 + 0xc) & 0x80000000) == 0) ||
            (iVar13 = *(int *)(iVar13 + 8), iVar13 != 0));
    iVar12 = iVar12 * (int)control[1].vtable;
    UiScrollableControl_ClampOffsetsToViewport
              ((int)&(control[1].vtable)->relocate + iVar12 + 1,control->rightOffset,iVar12,0,
               (UiScrollableControl *)control->parent);
    UVar3 = g_UiTimedListActionDelayFrames;
    control[1].nextSibling = (UiNodeBase *)((uint)control[1].nextSibling | 2);
    control[1].nextSibling = (UiNodeBase *)((uint)control[1].nextSibling & 0xffffff);
    control[1].nextSibling = (UiNodeBase *)((uint)control[1].nextSibling | UVar3 << 0x18);
  }
  return;
}

/* Address: 0x004BB100.
   Ownership: ui/controls/lists.
   Purpose: Binary entry is anchored by g_UiNodeVtable_004BA590[12]@004BA590.
   Local calls: UiScrollableControl_QueryContentSizeRegs, UiScrollableControl_ClampOffsetsToViewport.
   Cross-module calls: UiActionQueue_Enqueue [ui/core/runtime], UiNode_DefaultKeyboardEventMoveFocusNextCf
   [ui/controls/input].
*/
void UiListControl_HandleKeyboardNavigationCf
               (UiKeyboardStateMask keyboardStateMask,UiKeyboardEventCode keyCode,
               UiListControl *control)

{
  void **ppvVar1;
  void **ppvVar2;
  int iVar3;
  uint uVar4;
  UiScrollableContentDimensionsEdxEax8 UVar5;
  
  ppvVar1 = control->selectedRowSlot;
  if ((keyCode & 0xffff0000) != 0) {
    if (keyCode == 0x10001) {
      control->listStateFlags = control->listStateFlags | UI_LIST_SELECTION_CONFIRMED;
      UiActionQueue_Enqueue(control->actionId,control);
    }
    else if (keyCode == 0x10010) {
      control->selectedRowSlot = control->rowSlots;
    }
    else if (keyCode == 0x10018) {
      control->selectedRowSlot = control->rowSlots + (control->rowCount - 1);
    }
    else if (keyCode == 0x10012) {
      UVar5 = UiScrollableControl_QueryContentSizeRegs
                        ((UiScrollableControl *)(control->base).parent);
      iVar3 = ((uint)((int)control->selectedRowSlot - (int)control->rowSlots) >> 2) -
              ((int)((UVar5 >> 0x20) / (ulonglong)control->rowHeight) + -1);
      if (iVar3 < 0) {
        iVar3 = 0;
      }
      control->selectedRowSlot = control->rowSlots + iVar3;
    }
    else if (keyCode == 0x1001a) {
      UVar5 = UiScrollableControl_QueryContentSizeRegs
                        ((UiScrollableControl *)(control->base).parent);
      uVar4 = ((uint)((int)control->selectedRowSlot - (int)control->rowSlots) >> 2) +
              (int)((UVar5 >> 0x20) / (ulonglong)control->rowHeight) + -1;
      if (control->rowCount <= uVar4) {
        uVar4 = control->rowCount - 1;
      }
      control->selectedRowSlot = control->rowSlots + uVar4;
    }
    else if (keyCode == 0x10011) {
      if (control->rowSlots <= control->selectedRowSlot + -1) {
        control->selectedRowSlot = control->selectedRowSlot + -1;
      }
    }
    else {
      if (keyCode != 0x10019) goto UiListControl_DelegateUnhandledKeyboardEvent;
      if (((uint)((int)control->selectedRowSlot - (int)control->rowSlots) >> 2) + 1 <
          control->rowCount) {
        control->selectedRowSlot = control->selectedRowSlot + 1;
      }
    }
    ppvVar2 = control->selectedRowSlot;
    if (ppvVar2 != ppvVar1) {
      if (((control->listStateFlags & UI_LIST_PLAY_SELECTION_SOUND) != 0) &&
         (control->activationSound != (DirectSoundVoiceSet *)0x0)) {
        (*g_SoundPlayOneShot)(g_UiSoundGainQ15,g_UiSoundGainQ15,control->activationSound);
      }
      iVar3 = ((uint)((int)ppvVar2 - (int)control->rowSlots) >> 2) * control->rowHeight;
      UiScrollableControl_ClampOffsetsToViewport
                (iVar3 + control->rowHeight + 1,(control->base).rightOffset,iVar3,0,
                 (UiScrollableControl *)(control->base).parent);
      iVar3 = g_UiListActivationPulseFrames;
      control->listStateFlags = control->listStateFlags | UI_LIST_DEFERRED_ACTION_PENDING;
      control->listStateFlags = control->listStateFlags & 0xffffff;
      control->listStateFlags = control->listStateFlags | iVar3 << 0x18;
    }
    return;
  }
UiListControl_DelegateUnhandledKeyboardEvent:
  UiNode_DefaultKeyboardEventMoveFocusNextCf(keyboardStateMask,keyCode,&control->base);
  return;
}

/* Address: 0x004BB480.
   Ownership: ui/controls/lists.
   Purpose: Recomputes content height from count and row height, invokes the child layout callback, reselects the
   current pointer-derived index, then queues the action ID at +0x5C.
   Local calls: UiPointerList_GetSelectedIndexVariantBCf, UiPointerList_SelectIndexVariantB.
   Cross-module calls: UiActionQueue_Enqueue [ui/core/runtime].
*/
void UiPointerList_RefreshSelectionAndQueueAction(UiPointerListControl *control)

{
  UiNodeBase *control_00;
  UiListRowIndex index;
  UiNodeVtable *parentVtable;
  
  control_00 = (control->base).parent;
  parentVtable = control_00->vtable;
  (control->base).bottomOffset = control->rowHeight * control->rowCount + 1;
  (*parentVtable->layout)(control_00);
  index = UiPointerList_GetSelectedIndexVariantBCf(control);
  UiPointerList_SelectIndexVariantB(index,control);
  UiActionQueue_Enqueue(control->actionId,control);
  return;
}

/* Address: 0x004B87A0.
   Ownership: ui/controls/lists.
   Purpose: Binary entry is anchored by g_UiNodeVtable_004B7920[4]@004B7920.
   Local calls: UiScrollableControl_RefreshChildAndScrollThumbs.
   Cross-module calls: UiNode_InvalidateRoot [ui/core/runtime].
*/
void UiScrollableControl_BeginPrimaryScrollInteraction
               (UiPointerWheelDelta wheelDelta,UiPixelCoordinate pointerY,UiPixelCoordinate pointerX
               ,UiScrollableControl *control)

{
  qword qVar4;
  int iVar1;
  int extraout_ECX;
  int extraout_ECX_00;
  int extraout_ECX_01;
  int extraout_EDX;
  int extraout_EDX_00;
  int extraout_EDX_01;
  int extraout_EDX_02;
  qword qVar5;
  int iVar2;
  int iVar3;
  
  iVar3 = pointerX - (control->base).left;
  iVar2 = pointerY - (control->base).top;
  if ((control->scrollStateFlags &
      (UI_SCROLL_HORIZONTAL_BAR_AT_BOTTOM|UI_SCROLL_HORIZONTAL_BAR_AT_TOP)) == 0) {
UiScrollableControl_TryVerticalScrollbarInteraction:
    if ((control->scrollStateFlags &
        (UI_SCROLL_VERTICAL_BAR_AT_RIGHT|UI_SCROLL_VERTICAL_BAR_AT_LEFT)) == 0) {
      return;
    }
    qVar4._0_4_ = (*g_GraphicsTextureSourceGetLogicalSize)(0x5e,g_UiWindowTextureSource);
    if ((control->scrollStateFlags & UI_SCROLL_VERTICAL_BAR_AT_RIGHT) == 0) {
      if (iVar3 < 0) {
        return;
      }
      if ((int)(qword)qVar4 <= iVar3) {
        return;
      }
    }
    else {
      if ((control->base).layoutWidth <= iVar3) {
        return;
      }
      if (iVar3 < (control->base).layoutWidth - (int)(qword)qVar4) {
        return;
      }
    }
    (*g_GraphicsTextureSourceGetLogicalSize)(0x5a,g_UiWindowTextureSource);
    iVar3 = extraout_ECX_00;
    if ((control->scrollStateFlags & UI_SCROLL_HORIZONTAL_BAR_AT_BOTTOM) != 0) {
      iVar3 = extraout_ECX_00 - extraout_EDX_00;
    }
    iVar1 = 0;
    if ((control->scrollStateFlags & UI_SCROLL_HORIZONTAL_BAR_AT_TOP) != 0) {
      iVar1 = extraout_EDX_00;
    }
    if (iVar2 < iVar1) {
      return;
    }
    if (iVar3 <= iVar2) {
      return;
    }
    if (iVar2 < control->verticalThumbTop) {
      (*g_GraphicsTextureSourceGetLogicalSize)(0x5e,g_UiWindowTextureSource);
      if (iVar2 - iVar1 < extraout_EDX_01) {
        control->scrollStateFlags =
             control->scrollStateFlags |
             (UI_SCROLL_VERTICAL_DECREMENT_ACTIVE|UI_SCROLL_PRIMARY_INTERACTION_ACTIVE);
        goto UiScrollableControl_InvalidateAfterPrimaryScrollInteraction;
      }
      control->scrollOffsetY = control->scrollOffsetY + ((int)control->viewportHeight >> 1);
      control->scrollStateFlags =
           control->scrollStateFlags | UI_SCROLL_VERTICAL_TRACK_BEFORE_THUMB_ACTIVE;
    }
    else {
      if (iVar2 < control->verticalThumbBottom) {
        control->pointerAnchorY = iVar2 - control->verticalThumbTop;
        control->scrollStateFlags = control->scrollStateFlags | UI_SCROLL_VERTICAL_THUMB_ACTIVE;
        goto UiScrollableControl_InvalidateAfterPrimaryScrollInteraction;
      }
      (*g_GraphicsTextureSourceGetLogicalSize)(0x5e,g_UiWindowTextureSource);
      if (extraout_ECX_01 <= extraout_EDX_02) {
        control->scrollStateFlags =
             control->scrollStateFlags |
             (UI_SCROLL_VERTICAL_INCREMENT_ACTIVE|UI_SCROLL_PRIMARY_INTERACTION_ACTIVE);
        goto UiScrollableControl_InvalidateAfterPrimaryScrollInteraction;
      }
      control->scrollOffsetY = control->scrollOffsetY - ((int)control->viewportHeight >> 1);
      control->scrollStateFlags =
           control->scrollStateFlags | UI_SCROLL_VERTICAL_TRACK_AFTER_THUMB_ACTIVE;
    }
  }
  else {
    (*g_GraphicsTextureSourceGetLogicalSize)(0x5a,g_UiWindowTextureSource);
    if ((control->scrollStateFlags & UI_SCROLL_HORIZONTAL_BAR_AT_BOTTOM) == 0) {
      if ((-1 < iVar2) && (iVar2 < extraout_EDX))
      goto UiScrollableControl_BeginHorizontalScrollbarInteraction;
      goto UiScrollableControl_TryVerticalScrollbarInteraction;
    }
    if (((control->base).layoutHeight <= iVar2) ||
       (iVar2 < (control->base).layoutHeight - extraout_EDX))
    goto UiScrollableControl_TryVerticalScrollbarInteraction;
UiScrollableControl_BeginHorizontalScrollbarInteraction:
    qVar4._0_4_ = (*g_GraphicsTextureSourceGetLogicalSize)(0x5e,g_UiWindowTextureSource);
    iVar1 = qVar4._4_4_;
    if ((control->scrollStateFlags & UI_SCROLL_VERTICAL_BAR_AT_RIGHT) != 0) {
      iVar1 = qVar4._4_4_ - (int)(qword)qVar4;
    }
    qVar5._0_4_ = 0;
    if ((control->scrollStateFlags & UI_SCROLL_VERTICAL_BAR_AT_LEFT) != 0) {
      qVar5._0_4_ = (qword)qVar4;
    }
    if ((iVar3 < (int)(qword)qVar5) || (iVar1 <= iVar3))
    goto UiScrollableControl_TryVerticalScrollbarInteraction;
    if (iVar3 < control->horizontalThumbLeft) {
      qVar4._0_4_ = (*g_GraphicsTextureSourceGetLogicalSize)(0x5a,g_UiWindowTextureSource);
      if (iVar3 - (int)(qword)qVar5 < (int)(qword)qVar4) {
        control->scrollStateFlags =
             control->scrollStateFlags |
             (UI_SCROLL_HORIZONTAL_DECREMENT_ACTIVE|UI_SCROLL_PRIMARY_INTERACTION_ACTIVE);
        goto UiScrollableControl_InvalidateAfterPrimaryScrollInteraction;
      }
      control->scrollOffsetX = control->scrollOffsetX + ((int)control->viewportWidth >> 1);
      control->scrollStateFlags =
           control->scrollStateFlags | UI_SCROLL_HORIZONTAL_TRACK_BEFORE_THUMB_ACTIVE;
    }
    else {
      if (iVar3 < control->horizontalThumbRight) {
        control->pointerAnchorX = iVar3 - control->horizontalThumbLeft;
        control->scrollStateFlags = control->scrollStateFlags | UI_SCROLL_HORIZONTAL_THUMB_ACTIVE;
        goto UiScrollableControl_InvalidateAfterPrimaryScrollInteraction;
      }
      qVar4._0_4_ = (*g_GraphicsTextureSourceGetLogicalSize)(0x5a,g_UiWindowTextureSource);
      if (extraout_ECX <= (int)(qword)qVar4) {
        control->scrollStateFlags =
             control->scrollStateFlags |
             (UI_SCROLL_HORIZONTAL_INCREMENT_ACTIVE|UI_SCROLL_PRIMARY_INTERACTION_ACTIVE);
        goto UiScrollableControl_InvalidateAfterPrimaryScrollInteraction;
      }
      control->scrollOffsetX = control->scrollOffsetX - ((int)control->viewportWidth >> 1);
      control->scrollStateFlags =
           control->scrollStateFlags | UI_SCROLL_HORIZONTAL_TRACK_AFTER_THUMB_ACTIVE;
    }
  }
  UiScrollableControl_RefreshChildAndScrollThumbs(control);
UiScrollableControl_InvalidateAfterPrimaryScrollInteraction:
  UiNode_InvalidateRoot(&control->base);
  return;
}

/* Address: 0x004B8A20.
   Ownership: ui/controls/lists.
   Purpose: Binary entry is anchored by g_UiNodeVtable_004B7920[5]@004B7920.
   Local calls: UiScrollableControl_RefreshChildAndScrollThumbs.
   Cross-module calls: UiNode_InvalidateRoot [ui/core/runtime].
*/
void __fastcall
UiScrollableControl_EndPrimaryScrollInteraction(undefined4 param_1,undefined4 param_2)

{
  UiScrollableControl *in_stack_00000010;
  
  if ((in_stack_00000010->scrollStateFlags & UI_SCROLL_HORIZONTAL_TRACK_BEFORE_THUMB_ACTIVE) != 0) {
    in_stack_00000010->scrollOffsetX =
         in_stack_00000010->scrollOffsetX + ((int)in_stack_00000010->viewportWidth >> 1);
    UiScrollableControl_RefreshChildAndScrollThumbs(in_stack_00000010);
  }
  if ((in_stack_00000010->scrollStateFlags & UI_SCROLL_HORIZONTAL_TRACK_AFTER_THUMB_ACTIVE) != 0) {
    in_stack_00000010->scrollOffsetX =
         in_stack_00000010->scrollOffsetX - ((int)in_stack_00000010->viewportWidth >> 1);
    UiScrollableControl_RefreshChildAndScrollThumbs(in_stack_00000010);
  }
  if ((in_stack_00000010->scrollStateFlags & UI_SCROLL_VERTICAL_TRACK_BEFORE_THUMB_ACTIVE) != 0) {
    in_stack_00000010->scrollOffsetY =
         in_stack_00000010->scrollOffsetY + ((int)in_stack_00000010->viewportHeight >> 1);
    UiScrollableControl_RefreshChildAndScrollThumbs(in_stack_00000010);
  }
  if ((in_stack_00000010->scrollStateFlags & UI_SCROLL_VERTICAL_TRACK_AFTER_THUMB_ACTIVE) != 0) {
    in_stack_00000010->scrollOffsetY =
         in_stack_00000010->scrollOffsetY - ((int)in_stack_00000010->viewportHeight >> 1);
    UiScrollableControl_RefreshChildAndScrollThumbs(in_stack_00000010);
  }
  in_stack_00000010->scrollStateFlags =
       in_stack_00000010->scrollStateFlags &
       ~(UI_SCROLL_VERTICAL_INCREMENT_ACTIVE|UI_SCROLL_VERTICAL_TRACK_AFTER_THUMB_ACTIVE|
         UI_SCROLL_VERTICAL_THUMB_ACTIVE|UI_SCROLL_VERTICAL_TRACK_BEFORE_THUMB_ACTIVE|
         UI_SCROLL_VERTICAL_DECREMENT_ACTIVE|UI_SCROLL_HORIZONTAL_INCREMENT_ACTIVE|
         UI_SCROLL_HORIZONTAL_TRACK_AFTER_THUMB_ACTIVE|UI_SCROLL_HORIZONTAL_THUMB_ACTIVE|
         UI_SCROLL_HORIZONTAL_TRACK_BEFORE_THUMB_ACTIVE|UI_SCROLL_HORIZONTAL_DECREMENT_ACTIVE|
        UI_SCROLL_PRIMARY_INTERACTION_ACTIVE);
  UiNode_InvalidateRoot(&in_stack_00000010->base);
  return;
}

/* Address: 0x004B8BF0.
   Ownership: ui/controls/lists.
   Purpose: Binary entry is anchored by g_UiNodeVtable_004B7920[8]@004B7920.
   Local calls: UiScrollableControl_RefreshChildAndScrollThumbs.
   Cross-module calls: UiNode_InvalidateRoot [ui/core/runtime].
*/
void UiScrollableControl_UpdatePrimaryScrollDrag
               (UiPointerWheelDelta wheelDelta,UiPixelCoordinate pointerY,UiPixelCoordinate pointerX
               ,UiScrollableControl *control)

{
  int extraout_ECX;
  int extraout_ECX_00;
  int extraout_ECX_01;
  int extraout_ECX_02;
  int extraout_ECX_03;
  int extraout_ECX_04;
  int extraout_ECX_05;
  int extraout_ECX_06;
  int extraout_ECX_07;
  int extraout_ECX_08;
  int iVar1;
  int extraout_EDX;
  int iVar2;
  int extraout_EDX_00;
  int extraout_EDX_01;
  int extraout_EDX_02;
  int iVar3;
  qword qVar4;
  
  if ((control->scrollStateFlags &
      (UI_SCROLL_VERTICAL_TRACK_AFTER_THUMB_ACTIVE|UI_SCROLL_VERTICAL_TRACK_BEFORE_THUMB_ACTIVE|
       UI_SCROLL_HORIZONTAL_TRACK_AFTER_THUMB_ACTIVE|UI_SCROLL_HORIZONTAL_TRACK_BEFORE_THUMB_ACTIVE)
      ) != 0) {
    return;
  }
  if ((control->scrollStateFlags & UI_SCROLL_HORIZONTAL_THUMB_ACTIVE) != 0) {
    iVar1 = (control->base).layoutWidth;
    qVar4 = (*g_GraphicsTextureSourceGetLogicalSize)(0x5a,g_UiWindowTextureSource);
    iVar2 = extraout_ECX_04 - (int)qVar4;
    iVar1 = iVar1 + (int)qVar4 * -2;
    if ((control->scrollStateFlags &
        (UI_SCROLL_VERTICAL_BAR_AT_RIGHT|UI_SCROLL_VERTICAL_BAR_AT_LEFT)) != 0) {
      qVar4 = (*g_GraphicsTextureSourceGetLogicalSize)(0x5e,g_UiWindowTextureSource);
      iVar2 = extraout_ECX_05;
      if ((control->scrollStateFlags & UI_SCROLL_VERTICAL_BAR_AT_LEFT) != 0) {
        iVar2 = extraout_ECX_05 - (int)qVar4;
      }
      iVar1 = iVar1 - (int)qVar4;
    }
    control->scrollOffsetX =
         (UiPixelOffset)
         (((longlong)(int)(control->contentWidth - control->viewportWidth) * (longlong)iVar2) /
         (longlong)-((iVar1 - control->horizontalThumbRight) + control->horizontalThumbLeft));
    goto UiScrollableControl_RefreshAfterPrimaryDragUpdate;
  }
  if ((control->scrollStateFlags & UI_SCROLL_VERTICAL_THUMB_ACTIVE) != 0) {
    iVar1 = (control->base).layoutHeight;
    (*g_GraphicsTextureSourceGetLogicalSize)(0x5e,g_UiWindowTextureSource);
    iVar2 = extraout_ECX_06 - extraout_EDX_00;
    iVar1 = iVar1 + extraout_EDX_00 * -2;
    if ((control->scrollStateFlags &
        (UI_SCROLL_HORIZONTAL_BAR_AT_BOTTOM|UI_SCROLL_HORIZONTAL_BAR_AT_TOP)) != 0) {
      (*g_GraphicsTextureSourceGetLogicalSize)(0x5a,g_UiWindowTextureSource);
      iVar2 = extraout_ECX_07;
      if ((control->scrollStateFlags & UI_SCROLL_HORIZONTAL_BAR_AT_TOP) != 0) {
        iVar2 = extraout_ECX_07 - extraout_EDX_01;
      }
      iVar1 = iVar1 - extraout_EDX_01;
    }
    control->scrollOffsetY =
         (UiPixelOffset)
         (((longlong)(int)(control->contentHeight - control->viewportHeight) * (longlong)iVar2) /
         (longlong)-((iVar1 - control->verticalThumbBottom) + control->verticalThumbTop));
    goto UiScrollableControl_RefreshAfterPrimaryDragUpdate;
  }
  if ((control->scrollStateFlags & UI_SCROLL_HORIZONTAL_DECREMENT_ACTIVE) == 0) {
    if ((control->scrollStateFlags & UI_SCROLL_HORIZONTAL_INCREMENT_ACTIVE) == 0) {
      if ((control->scrollStateFlags & UI_SCROLL_VERTICAL_DECREMENT_ACTIVE) == 0) {
        if ((control->scrollStateFlags & UI_SCROLL_VERTICAL_INCREMENT_ACTIVE) == 0) {
          return;
        }
        iVar1 = pointerY - (control->base).top;
        qVar4 = (*g_GraphicsTextureSourceGetLogicalSize)(0x5e,g_UiWindowTextureSource);
        iVar2 = (int)(qVar4 >> 0x20);
        if ((control->scrollStateFlags & UI_SCROLL_VERTICAL_BAR_AT_LEFT) == 0) {
          if ((extraout_ECX < (control->base).layoutWidth) &&
             ((control->base).layoutWidth - (int)qVar4 <= extraout_ECX))
          goto UiScrollableControl_ValidateBottomArrowHover;
        }
        else if ((-1 < extraout_ECX) && (extraout_ECX < (int)qVar4)) {
UiScrollableControl_ValidateBottomArrowHover:
          iVar3 = (control->base).layoutHeight;
          if ((control->scrollStateFlags & UI_SCROLL_HORIZONTAL_BAR_AT_BOTTOM) != 0) {
            (*g_GraphicsTextureSourceGetLogicalSize)(0x5a,g_UiWindowTextureSource);
            iVar3 = iVar3 - extraout_EDX;
          }
          if ((iVar1 < iVar3) && (iVar3 - iVar2 <= iVar1))
          goto UiScrollableControl_SetArrowHoverActive;
        }
      }
      else {
        iVar1 = pointerY - (control->base).top;
        qVar4 = (*g_GraphicsTextureSourceGetLogicalSize)(0x5e,g_UiWindowTextureSource);
        iVar2 = (int)(qVar4 >> 0x20);
        if ((control->scrollStateFlags & UI_SCROLL_VERTICAL_BAR_AT_LEFT) == 0) {
          if ((extraout_ECX_08 < (control->base).layoutWidth) &&
             ((control->base).layoutWidth - (int)qVar4 <= extraout_ECX_08))
          goto UiScrollableControl_ValidateTopArrowHover;
        }
        else if ((-1 < extraout_ECX_08) && (extraout_ECX_08 < (int)qVar4)) {
UiScrollableControl_ValidateTopArrowHover:
          iVar3 = 0;
          if ((control->scrollStateFlags & UI_SCROLL_HORIZONTAL_BAR_AT_TOP) != 0) {
            (*g_GraphicsTextureSourceGetLogicalSize)(0x5a,g_UiWindowTextureSource);
            iVar3 = extraout_EDX_02;
          }
          if ((iVar3 <= iVar1) && (iVar1 < iVar3 + iVar2))
          goto UiScrollableControl_SetArrowHoverActive;
        }
      }
    }
    else {
      iVar2 = pointerY - (control->base).top;
      qVar4 = (*g_GraphicsTextureSourceGetLogicalSize)(0x5a,g_UiWindowTextureSource);
      iVar3 = (int)(qVar4 >> 0x20);
      iVar1 = (int)qVar4;
      if ((control->scrollStateFlags & UI_SCROLL_HORIZONTAL_BAR_AT_TOP) == 0) {
        if ((iVar2 < (control->base).layoutHeight) &&
           ((control->base).layoutHeight - iVar3 <= iVar2))
        goto UiScrollableControl_ValidateRightArrowHover;
      }
      else if ((-1 < iVar2) && (iVar2 < iVar3)) {
UiScrollableControl_ValidateRightArrowHover:
        iVar2 = (control->base).layoutWidth;
        iVar3 = extraout_ECX_00;
        if ((control->scrollStateFlags & UI_SCROLL_VERTICAL_BAR_AT_RIGHT) != 0) {
          qVar4 = (*g_GraphicsTextureSourceGetLogicalSize)(0x5e,g_UiWindowTextureSource);
          iVar2 = iVar2 - (int)qVar4;
          iVar3 = extraout_ECX_01;
        }
        if ((iVar3 < iVar2) && (iVar2 - iVar1 <= iVar3))
        goto UiScrollableControl_SetArrowHoverActive;
      }
    }
  }
  else {
    iVar2 = pointerY - (control->base).top;
    qVar4 = (*g_GraphicsTextureSourceGetLogicalSize)(0x5a,g_UiWindowTextureSource);
    iVar3 = (int)(qVar4 >> 0x20);
    iVar1 = (int)qVar4;
    if ((control->scrollStateFlags & UI_SCROLL_HORIZONTAL_BAR_AT_TOP) == 0) {
      if ((iVar2 < (control->base).layoutHeight) && ((control->base).layoutHeight - iVar3 <= iVar2))
      goto UiScrollableControl_ValidateLeftArrowHover;
    }
    else if ((-1 < iVar2) && (iVar2 < iVar3)) {
UiScrollableControl_ValidateLeftArrowHover:
      qVar4 = 0;
      iVar2 = extraout_ECX_02;
      if ((control->scrollStateFlags & UI_SCROLL_VERTICAL_BAR_AT_LEFT) != 0) {
        qVar4 = (*g_GraphicsTextureSourceGetLogicalSize)(0x5e,g_UiWindowTextureSource);
        iVar2 = extraout_ECX_03;
      }
      if (((int)qVar4 <= iVar2) && (iVar2 < (int)qVar4 + iVar1)) {
UiScrollableControl_SetArrowHoverActive:
        if ((control->scrollStateFlags & UI_SCROLL_PRIMARY_INTERACTION_ACTIVE) != 0) {
          return;
        }
        control->scrollStateFlags = control->scrollStateFlags | UI_SCROLL_PRIMARY_INTERACTION_ACTIVE
        ;
        goto UiScrollableControl_RefreshAfterPrimaryDragUpdate;
      }
    }
  }
  if ((control->scrollStateFlags & UI_SCROLL_PRIMARY_INTERACTION_ACTIVE) == 0) {
    return;
  }
  control->scrollStateFlags = control->scrollStateFlags & ~UI_SCROLL_PRIMARY_INTERACTION_ACTIVE;
UiScrollableControl_RefreshAfterPrimaryDragUpdate:
  UiScrollableControl_RefreshChildAndScrollThumbs(control);
  UiNode_InvalidateRoot(&control->base);
  return;
}

/* Address: 0x004B8F90.
   Ownership: ui/controls/lists.
   Purpose: Binary entry is anchored by g_UiNodeVtable_004B7920[9]@004B7920.
   Local calls: UiScrollableControl_RefreshChildAndScrollThumbs.
   Cross-module calls: UiNode_InvalidateRoot [ui/core/runtime].
*/
void UiScrollableControl_UpdateSecondaryScrollDrag
               (UiPointerWheelDelta wheelDelta,UiPixelCoordinate pointerY,UiPixelCoordinate pointerX
               ,UiScrollableControl *control)

{
  int pointerDeltaX;
  int pointerDeltaY;
  
  pointerDeltaX = pointerX - control->pointerAnchorX;
  pointerDeltaY = pointerY - control->pointerAnchorY;
  if ((control->scrollStateFlags &
      (UI_SCROLL_HORIZONTAL_BAR_AT_BOTTOM|UI_SCROLL_HORIZONTAL_BAR_AT_TOP)) != 0) {
    control->scrollOffsetX = control->scrollOffsetX - pointerDeltaX;
  }
  if ((control->scrollStateFlags & (UI_SCROLL_VERTICAL_BAR_AT_RIGHT|UI_SCROLL_VERTICAL_BAR_AT_LEFT))
      != 0) {
    control->scrollOffsetY = control->scrollOffsetY - pointerDeltaY;
  }
  UiScrollableControl_RefreshChildAndScrollThumbs(control);
  UiNode_InvalidateRoot(&control->base);
  (*g_PointerSetPosition)(control->pointerAnchorY,control->pointerAnchorX);
  return;
}

/* Address: 0x004B9070.
   Ownership: ui/controls/lists.
   Purpose: Binary entry is anchored by g_UiNodeVtable_004B7920[16]@004B7920.
   Local calls: UiScrollableControl_RefreshChildAndScrollThumbs.
   Cross-module calls: UiNode_InvalidateRoot [ui/core/runtime].
*/
void UiScrollableControl_TickAutoScroll(UiScrollableControl *control)

{
  UiPixelOffset horizontalScrollStep;
  UiPixelOffset verticalScrollStep;
  
  if ((control->scrollStateFlags & UI_SCROLL_PRIMARY_INTERACTION_ACTIVE) != 0) {
    if ((control->scrollStateFlags & UI_SCROLL_HORIZONTAL_DECREMENT_ACTIVE) != 0) {
      control->scrollOffsetX = control->scrollOffsetX + control->autoScrollStepX;
    }
    if ((control->scrollStateFlags & UI_SCROLL_HORIZONTAL_INCREMENT_ACTIVE) != 0) {
      control->scrollOffsetX = control->scrollOffsetX - control->autoScrollStepX;
    }
    if ((control->scrollStateFlags & UI_SCROLL_VERTICAL_DECREMENT_ACTIVE) != 0) {
      control->scrollOffsetY = control->scrollOffsetY + control->autoScrollStepY;
    }
    if ((control->scrollStateFlags & UI_SCROLL_VERTICAL_INCREMENT_ACTIVE) != 0) {
      control->scrollOffsetY = control->scrollOffsetY - control->autoScrollStepY;
    }
    UiScrollableControl_RefreshChildAndScrollThumbs(control);
    UiNode_InvalidateRoot(&control->base);
  }
  return;
}

/* Address: 0x004B90E0.
   Ownership: ui/controls/lists.
   Purpose: Binary entry is anchored by g_UiNodeVtable_004B7920[17]@004B7920.
   Local calls: UiScrollableControl_RefreshChildAndScrollThumbs.
   Cross-module calls: UiNode_InvalidateRoot [ui/core/runtime].
*/
void UiScrollableControl_HandlePointerWheel
               (UiPointerWheelDelta wheelDelta,UiPixelCoordinate pointerY,UiPixelCoordinate pointerX
               ,UiScrollableControl *control)

{
  UiNodeBase *pUVar1;
  int scrollStep;
  UiNodeBase *contentChild;
  
  if (((((control->scrollStateFlags &
         (UI_SCROLL_VERTICAL_BAR_AT_RIGHT|UI_SCROLL_VERTICAL_BAR_AT_LEFT)) != 0) &&
       ((control->scrollStateFlags & 0x3000) == 0)) &&
      (((control->base).nodeFlags & UI_NODE_SUPPRESSED) == 0)) &&
     (pUVar1 = (control->base).firstChild, wheelDelta != 0)) {
    scrollStep = g_UiScrollWheelDefaultStep;
    if ((pUVar1 != (UiNodeBase *)0xffffffff) &&
       (((pUVar1->vtable == &g_UiTimedListControlVtable ||
         (pUVar1->vtable == &g_UiTextListControlVtable)) ||
        (pUVar1->vtable == &g_UiListControlVtable)))) {
      scrollStep = g_UiScrollWheelListStep;
    }
    control->scrollOffsetY = control->scrollOffsetY + wheelDelta * scrollStep;
    UiScrollableControl_RefreshChildAndScrollThumbs(control);
    UiNode_InvalidateRoot(&control->base);
  }
  return;
}

/* Address: 0x004BA500.
   Ownership: ui/controls/lists.
   Purpose: Selects an in-range pointer-list entry by storing base + index*4 at +0x60 and invalidates the
   corresponding fixed-height row. EAX, EDX, and flags remain governed by the original code.
   Local calls: UiScrollableControl_ClampOffsetsToViewport.
*/
void UiPointerList_SelectIndexVariantA(UiListRowIndex index,UiPointerListControl *control)

{
  int clipBottom;
  
  if (index < control->rowCount) {
    control->selectedRowSlot = control->rowSlots + index;
    clipBottom = control->rowHeight * index;
    UiScrollableControl_ClampOffsetsToViewport
              (clipBottom + 1 + control->rowHeight,(control->base).rightOffset,clipBottom,0,
               (UiScrollableControl *)(control->base).parent);
  }
  return;
}

/* Address: 0x004BB020.
   Ownership: ui/controls/lists.
   Purpose: Binary entry is anchored by g_UiNodeVtable_004BA590[4]@004BA590.
   Local calls: UiScrollableControl_ClampOffsetsToViewport.
   Cross-module calls: UiActionQueue_Enqueue [ui/core/runtime].
*/
void UiListControl_SelectRowFromPointer
               (UiPointerWheelDelta wheelDelta,UiPixelCoordinate pointerY,UiPixelCoordinate pointerX
               ,UiListControl *control)

{
  uint uVar1;
  int clipBottom;
  sdword *topEdgeField;
  
  topEdgeField = &(control->base).top;
  if ((((*topEdgeField <= pointerY) && (pointerY - *topEdgeField < (control->base).layoutHeight)) &&
      ((control->base).left <= pointerX)) && (pointerX < (control->base).right)) {
    uVar1 = (uint)(pointerY - *topEdgeField) / control->rowHeight;
    if (uVar1 < control->rowCount) {
      control->listStateFlags = control->listStateFlags | UI_LIST_SELECTION_CONFIRMED;
      if ((((control->base).nodeFlags & UI_NODE_REPEAT_OR_DOUBLE_CLICK) != 0) ||
         (control->listStateFlags = control->listStateFlags & ~UI_LIST_SELECTION_CONFIRMED,
         control->rowSlots + uVar1 != control->selectedRowSlot)) {
        control->selectedRowSlot = control->rowSlots + uVar1;
        clipBottom = uVar1 * control->rowHeight;
        UiScrollableControl_ClampOffsetsToViewport
                  (clipBottom + 1 + control->rowHeight,(control->base).rightOffset,clipBottom,0,
                   (UiScrollableControl *)(control->base).parent);
        UiActionQueue_Enqueue(control->actionId,control);
        if (((control->listStateFlags & UI_LIST_PLAY_SELECTION_SOUND) != 0) &&
           (control->activationSound != (DirectSoundVoiceSet *)0x0)) {
          (*g_SoundPlayOneShot)(g_UiSoundGainQ15,g_UiSoundGainQ15,control->activationSound);
        }
      }
    }
  }
  return;
}

/* Address: 0x004BB7A0.
   Ownership: ui/controls/lists.
   Purpose: Bubble-sorts in descending unsigned lexicographic order by two consecutive dwords at fieldOffset, then
   restores selection and invalidates its row. Typed parameters: p0 fieldOffset→UiPointerListFieldByteOffset_V342.
   Calling convention, exact VariableStorage serialization, function body bytes, control flow, globals, locals, and
   executable data remain unchanged.
   Local calls: UiScrollableControl_ClampOffsetsToViewport.
*/
undefined8
UiPointerList_SortByDwordPairFieldDescending
          (UiPointerListFieldByteOffset fieldOffset,UiPointerListControl *control)

{
  void *pvVar1;
  void *pvVar2;
  uint uVar3;
  uint uVar4;
  undefined4 in_EAX;
  int iVar5;
  int iVar6;
  UiListRowCount UVar7;
  undefined4 in_EDX;
  void **ppvVar8;
  void **ppvVar9;
  
  ppvVar9 = control->rowSlots;
  if (ppvVar9 != (void **)0x0) {
    iVar5 = control->rowCount - 1;
    if ((iVar5 != 0) && (-1 < iVar5)) {
      pvVar2 = *control->selectedRowSlot;
      ppvVar8 = ppvVar9;
      iVar6 = iVar5;
      do {
        do {
          ppvVar9 = ppvVar9 + 1;
          uVar3 = *(uint *)((int)*ppvVar8 + fieldOffset);
          uVar4 = *(uint *)((int)*ppvVar9 + fieldOffset);
          if ((uVar3 <= uVar4) &&
             ((uVar3 < uVar4 ||
              (((uint *)((int)*ppvVar8 + fieldOffset))[1] <=
               ((uint *)((int)*ppvVar9 + fieldOffset))[1])))) {
            LOCK();
            pvVar1 = *ppvVar9;
            *ppvVar9 = *ppvVar8;
            UNLOCK();
            *ppvVar8 = pvVar1;
          }
          iVar5 = iVar5 + -1;
        } while (iVar5 != 0);
        ppvVar9 = ppvVar8 + 1;
        iVar5 = iVar6 + -1;
        ppvVar8 = ppvVar9;
        iVar6 = iVar5;
      } while (iVar5 != 0);
      ppvVar9 = control->rowSlots;
      UVar7 = control->rowCount;
      iVar5 = 0;
      do {
        if (pvVar2 == *ppvVar9)
        goto 
        UiPointerList_SortByDwordPairFieldDescending_CommitResolvedSelectedRowSlotAndClampViewport;
        iVar5 = iVar5 + control->rowHeight;
        ppvVar9 = ppvVar9 + 1;
        UVar7 = UVar7 - 1;
      } while (UVar7 != 0);
      ppvVar9 = control->rowSlots;
UiPointerList_SortByDwordPairFieldDescending_CommitResolvedSelectedRowSlotAndClampViewport:
      control->selectedRowSlot = ppvVar9;
      UiScrollableControl_ClampOffsetsToViewport
                (iVar5 + 1 + control->rowHeight,(control->base).rightOffset,iVar5,0,
                 (UiScrollableControl *)(control->base).parent);
    }
  }
  return CONCAT44(in_EDX,in_EAX);
}

/* Address: 0x004BB8A0.
   Ownership: ui/controls/lists.
   Purpose: Bubble-sorts in ascending unsigned order by one dword at fieldOffset, then restores selection and
   invalidates its row. Typed parameters: p0 fieldOffset→UiPointerListFieldByteOffset_V342. Calling convention,
   exact VariableStorage serialization, function body bytes, control flow, globals, locals, and executable data
   remain unchanged.
   Local calls: UiScrollableControl_ClampOffsetsToViewport.
*/
undefined8
UiPointerList_SortByDwordFieldAscending
          (UiPointerListFieldByteOffset fieldOffset,UiPointerListControl *control)

{
  void *pvVar1;
  void *pvVar2;
  undefined4 in_EAX;
  int iVar3;
  int iVar4;
  UiListRowCount UVar5;
  undefined4 in_EDX;
  void **ppvVar6;
  void **ppvVar7;
  
  ppvVar7 = control->rowSlots;
  if (ppvVar7 != (void **)0x0) {
    iVar3 = control->rowCount - 1;
    if ((iVar3 != 0) && (-1 < iVar3)) {
      pvVar2 = *control->selectedRowSlot;
      ppvVar6 = ppvVar7;
      iVar4 = iVar3;
      do {
        do {
          ppvVar7 = ppvVar7 + 1;
          if (*(uint *)((int)*ppvVar7 + fieldOffset) <= *(uint *)((int)*ppvVar6 + fieldOffset)) {
            LOCK();
            pvVar1 = *ppvVar7;
            *ppvVar7 = *ppvVar6;
            UNLOCK();
            *ppvVar6 = pvVar1;
          }
          iVar3 = iVar3 + -1;
        } while (iVar3 != 0);
        ppvVar7 = ppvVar6 + 1;
        iVar3 = iVar4 + -1;
        ppvVar6 = ppvVar7;
        iVar4 = iVar3;
      } while (iVar3 != 0);
      ppvVar7 = control->rowSlots;
      UVar5 = control->rowCount;
      iVar3 = 0;
      do {
        if (pvVar2 == *ppvVar7)
        goto UiPointerList_SortByDwordFieldAscending_CommitResolvedSelectedRowSlotAndClampViewport;
        iVar3 = iVar3 + control->rowHeight;
        ppvVar7 = ppvVar7 + 1;
        UVar5 = UVar5 - 1;
      } while (UVar5 != 0);
      ppvVar7 = control->rowSlots;
UiPointerList_SortByDwordFieldAscending_CommitResolvedSelectedRowSlotAndClampViewport:
      control->selectedRowSlot = ppvVar7;
      UiScrollableControl_ClampOffsetsToViewport
                (iVar3 + 1 + control->rowHeight,(control->base).rightOffset,iVar3,0,
                 (UiScrollableControl *)(control->base).parent);
    }
  }
  return CONCAT44(in_EDX,in_EAX);
}

/* Address: 0x004BBCD0.
   Ownership: ui/controls/lists.
   Purpose: Handles ui timed list control select row from pointer.
   Local calls: UiTimedListControl_SelectRecordAndScrollIntoView.
   Cross-module calls: RichTextCommandStream_MeasureRegs [assets/text/richtext], UiActionQueue_Enqueue
   [ui/core/runtime].
*/
void UiTimedListControl_SelectRowFromPointer
               (int pointerButton,int pointerY,int pointerX,UiNodeBase *control)

{
  UiNodeBase *pUVar1;
  code *pcVar2;
  UiAnchorFractionQ31 UVar3;
  UiNodeVtable *pUVar4;
  int iVar5;
  int iVar6;
  UiAnchorFractionQ31 UVar7;
  int extraout_ECX;
  uint uVar8;
  sdword *unaff_EBP;
  sdword *psVar10;
  sdword *psVar11;
  UiNodeBase *pUVar12;
  bool bVar13;
  RichTextExtentRegs RVar14;
  UiPackedTextStyle aUStack_3c [5];
  UiNodeBase *pUStack_28;
  sdword *psStack_24;
  UiNodeBase *pUStack_20;
  uint uVar9;
  
  pUStack_20 = (UiNodeBase *)0x0;
  pUVar1 = control[1].firstChild;
  if (pUVar1 != (UiNodeBase *)0x0) {
    pUVar12 = pUVar1->nextSibling;
    iVar6 = pointerX - control->left;
    if (((control->left <= pointerX) && (control->top <= pointerY)) &&
       (pUVar12 != (UiNodeBase *)0x0)) {
      uVar9 = pointerY - control->top;
      psVar10 = &pUVar1->left;
      while (pUVar1 = pUStack_20, uVar8 = uVar9 - (int)control[1].vtable,
            uVar8 != 0 && (int)control[1].vtable <= (int)uVar9) {
        psVar11 = psVar10 + 4;
        pUVar12 = (UiNodeBase *)((int)&pUVar12[-1].nodeFlags + 3);
        if ((((psVar10[3] & 1U) != 0) && ((psVar10[3] & 2U) != 0)) && (psVar10[2] != 0)) {
          pUStack_20 = (UiNodeBase *)((int)&pUStack_20->nextSibling + 1);
          psStack_24 = psVar11;
          pUStack_28 = pUVar12;
          psVar11 = (int *)psVar10[2] + 4;
          pUVar12 = *(UiNodeBase **)psVar10[2];
        }
        while (uVar9 = uVar8, psVar10 = psVar11, pUVar12 == (UiNodeBase *)0x0) {
          if (pUStack_20 == (UiNodeBase *)0x0) {
            return;
          }
          pUStack_20 = (UiNodeBase *)((int)&pUStack_20[-1].nodeFlags + 3);
          psVar11 = unaff_EBP;
          pUVar12 = pUStack_20;
        }
      }
      iVar5 = (int)pUStack_20 * control[1].topAnchorQ31;
      UVar7 = iVar6 - iVar5;
      if (iVar6 < iVar5) {
        if (((psVar10[3] & 1U) != 0) && (iVar5 != 0)) {
          UVar3 = control[1].topAnchorQ31;
          bVar13 = CARRY4(uVar8,(uint)control[1].vtable);
          pUVar4 = control[1].vtable;
          (&psStack_24)[(int)pUStack_20 * 2] = (sdword *)control[1].bottom;
          aUStack_3c[(int)pUVar1 * 2 + 5] = control[1].leftOffset;
          aUStack_3c[(int)pUVar1 * 2 + 4] = 0;
          aUStack_3c[(int)pUVar1 * 2 + 3] = 0;
          aUStack_3c[(int)pUVar1 * 2 + 2] = UVar7 + UVar3;
          aUStack_3c[(int)pUVar1 * 2 + 1] = uVar8 + (int)pUVar4;
          aUStack_3c[(int)pUVar1 * 2] = 0x4bbe3a;
          (*g_GraphicsTextureSourceTestOpaquePixel)
                    (aUStack_3c[(int)pUVar1 * 2 + 1],aUStack_3c[(int)pUVar1 * 2 + 2],
                     aUStack_3c[(int)pUVar1 * 2 + 3],aUStack_3c[(int)pUVar1 * 2 + 4],
                     aUStack_3c[(int)pUVar1 * 2 + 5],
                     (GraphicsTextureSourceAsset *)(&psStack_24)[(int)pUVar1 * 2]);
          if ((bVar13) && (control[1].right != 0)) {
            (&psStack_24)[(int)pUVar1 * 2] = (sdword *)control;
            aUStack_3c[(int)pUVar1 * 2 + 5] = (UiPackedTextStyle)psVar10;
            pcVar2 = (code *)control[1].right;
            aUStack_3c[(int)pUVar1 * 2 + 4] = 0x4bbe4d;
            (*pcVar2)();
          }
        }
      }
      else {
        if (UVar7 != control[1].rightAnchorQ31 && (int)control[1].rightAnchorQ31 <= (int)UVar7) {
          (&psStack_24)[(int)pUStack_20 * 2] = (sdword *)*psVar10;
          aUStack_3c[(int)pUVar1 * 2 + 5] = g_UiListTextStyle;
          aUStack_3c[(int)pUVar1 * 2 + 4] = 0x4bbdb4;
          RVar14 = RichTextCommandStream_MeasureRegs
                             (aUStack_3c[(int)pUVar1 * 2 + 5],(word *)(&psStack_24)[(int)pUVar1 * 2]
                             );
          if (((int)RVar14.widthPixels < extraout_ECX) &&
             (6 < (int)(extraout_ECX - RVar14.widthPixels))) {
            return;
          }
        }
        if (psVar10 != (int *)control[1].top) {
          (&psStack_24)[(int)pUVar1 * 2] = (sdword *)control;
          aUStack_3c[(int)pUVar1 * 2 + 5] = (UiPackedTextStyle)psVar10;
          aUStack_3c[(int)pUVar1 * 2 + 4] = 0x4bbdd0;
          UiTimedListControl_SelectRecordAndScrollIntoView
                    ((UiTimedListTreeRecord16 *)aUStack_3c[(int)pUVar1 * 2 + 5],
                     (UiTimedListRuntimeExtendedView88 *)(&psStack_24)[(int)pUVar1 * 2]);
          (&psStack_24)[(int)pUVar1 * 2] = (sdword *)control;
          aUStack_3c[(int)pUVar1 * 2 + 5] = control[1].left;
          aUStack_3c[(int)pUVar1 * 2 + 4] = 0x4bbddc;
          UiActionQueue_Enqueue(aUStack_3c[(int)pUVar1 * 2 + 5],(&psStack_24)[(int)pUVar1 * 2]);
        }
        if ((((control->nodeFlags & UI_NODE_REPEAT_OR_DOUBLE_CLICK) != 0) &&
            ((psVar10[3] & 1U) != 0)) && (control[1].right != 0)) {
          (&psStack_24)[(int)pUVar1 * 2] = (sdword *)control;
          aUStack_3c[(int)pUVar1 * 2 + 5] = (UiPackedTextStyle)psVar10;
          pcVar2 = (code *)control[1].right;
          aUStack_3c[(int)pUVar1 * 2 + 4] = 0x4bbe07;
          (*pcVar2)();
        }
      }
    }
  }
  return;
}

/* Address: 0x004B11C0.
   Ownership: ui/controls/lists.
   Purpose: Traverses a sibling list and forwards the action ID through vtable slot +0x3C, restoring matching
   controls. Kept distinct from player IDs, command opcodes, and resource identifiers. Typed parameters: p0
   actionId→UiActionId_V338. Calling convention, storage, body bytes, control flow, and executable data remain
   unchanged.
*/
void UiNodeList_UnsuppressActionId(UiActionId actionId,UiNodeBase *firstNode)

{
  for (; firstNode != (UiNodeBase *)0xffffffff; firstNode = firstNode->nextSibling) {
    (*firstNode->vtable->unsuppressActionId)(actionId,firstNode);
  }
  return;
}

/* Address: 0x004B1200.
   Ownership: ui/controls/lists.
   Purpose: Traverses a sibling list and forwards the action ID through vtable slot +0x38, suppressing matching
   controls. Kept distinct from player IDs, command opcodes, and resource identifiers. Typed parameters: p0
   actionId→UiActionId_V338. Calling convention, storage, body bytes, control flow, and executable data remain
   unchanged.
*/
void UiNodeList_SuppressActionId(UiActionId actionId,UiNodeBase *firstNode)

{
  for (; firstNode != (UiNodeBase *)0xffffffff; firstNode = firstNode->nextSibling) {
    (*firstNode->vtable->suppressActionId)(actionId,firstNode);
  }
  return;
}

/* Address: 0x004B2550.
   Ownership: ui/controls/lists.
   Purpose: Handles keyboard activation for selectable controls, optionally plays the derived control's keyboard
   sound, toggles or sets selected state, queues actionId, invalidates the root, and returns consumption through
   CF.
   Cross-module calls: UiActionQueue_Enqueue [ui/core/runtime], UiNode_InvalidateRoot [ui/core/runtime],
   UiNode_DefaultKeyboardEventMoveFocusNextCf [ui/controls/input].
*/
void UiSelectableControl_KeyboardEventCf
               (UiKeyboardStateMask keyboardStateMask,UiKeyboardEventCode keyCode,
               UiSelectableControl *control)

{
  UiSelectableStateFlags activationKeyBindingFlag;
  
  if (((control->base).nodeFlags & UI_NODE_SUPPRESSED) != 0)
  goto UiSelectableControl_DelegateUnhandledKeyboardEvent;
  if (keyCode == 0x20) {
    if ((control != (UiSelectableControl *)g_UiKeyboardFocusNode) ||
       ((control->stateFlags & UI_SELECTABLE_IGNORE_FOCUSED_SPACE_ACTIVATION) != 0))
    goto UiSelectableControl_DelegateUnhandledKeyboardEvent;
  }
  else {
    if ((keyCode & 0xffff0000) == 0) goto UiSelectableControl_DelegateUnhandledKeyboardEvent;
    if (keyCode == 0x10001) {
      activationKeyBindingFlag = control->stateFlags & 4;
    }
    else {
      if (keyCode != 0x10000) goto UiSelectableControl_DelegateUnhandledKeyboardEvent;
      activationKeyBindingFlag = control->stateFlags & 8;
    }
    if (activationKeyBindingFlag == 0) goto UiSelectableControl_DelegateUnhandledKeyboardEvent;
  }
  if ((control->stateFlags & UI_SELECTABLE_PERSISTENT_ACTIVATION_MODE) == 0) {
    if (((control->stateFlags & 0x80) != 0) && (control[1].base.parent != (UiNodeBase *)0x0)) {
      (*g_SoundPlayOneShot)
                (g_UiSoundGainQ15,g_UiSoundGainQ15,(DirectSoundVoiceSet *)control[1].base.parent);
    }
    UiActionQueue_Enqueue(control->actionId,control);
    UiNode_InvalidateRoot(&control->base);
    return;
  }
  if ((control->stateFlags & UI_SELECTABLE_TOGGLE_ON_ACTIVATION) != 0) {
    if (((control->stateFlags & 0x80) != 0) && (control[1].base.parent != (UiNodeBase *)0x0)) {
      (*g_SoundPlayOneShot)
                (g_UiSoundGainQ15,g_UiSoundGainQ15,(DirectSoundVoiceSet *)control[1].base.parent);
    }
    control->stateFlags = control->stateFlags ^ UI_SELECTABLE_SELECTED_OR_CHECKED;
    UiActionQueue_Enqueue(control->actionId,control);
    UiNode_InvalidateRoot(&control->base);
    return;
  }
  if ((control->stateFlags & UI_SELECTABLE_SELECTED_OR_CHECKED) == 0) {
    if (((control->stateFlags & 0x80) != 0) && (control[1].base.parent != (UiNodeBase *)0x0)) {
      (*g_SoundPlayOneShot)
                (g_UiSoundGainQ15,g_UiSoundGainQ15,(DirectSoundVoiceSet *)control[1].base.parent);
    }
    control->stateFlags = control->stateFlags | UI_SELECTABLE_SELECTED_OR_CHECKED;
    UiActionQueue_Enqueue(control->actionId,control);
    UiNode_InvalidateRoot(&control->base);
    return;
  }
UiSelectableControl_DelegateUnhandledKeyboardEvent:
  UiNode_DefaultKeyboardEventMoveFocusNextCf(keyboardStateMask,keyCode,&control->base);
  return;
}

/* Address: 0x004B26E0.
   Ownership: ui/controls/lists.
   Purpose: When actionId matches control->actionId, sets nodeFlags bit 0x08 and updates focus/activation state for
   the newly suppressed control.
   Cross-module calls: UiKeyboardFocus_ReleaseNode [ui/controls/input].
*/
void UiSelectableControl_SuppressIfActionId(UiActionId actionId,UiSelectableControl *control)

{
  UiNodeFlags *controlNodeFlags;
  
  if (actionId == control->actionId) {
    controlNodeFlags = &(control->base).nodeFlags;
    *controlNodeFlags = *controlNodeFlags | UI_NODE_SUPPRESSED;
    UiKeyboardFocus_ReleaseNode(&control->base);
  }
  return;
}

/* Address: 0x004B2710.
   Ownership: ui/controls/lists.
   Purpose: When actionId matches control->actionId, clears nodeFlags bit 0x08 and updates focus/activation state
   for the restored control.
   Cross-module calls: UiKeyboardFocus_AcquireIfNone [ui/controls/input].
*/
void UiSelectableControl_UnsuppressIfActionId(UiActionId actionId,UiSelectableControl *control)

{
  UiNodeFlags *controlNodeFlags;
  
  if (actionId == control->actionId) {
    controlNodeFlags = &(control->base).nodeFlags;
    *controlNodeFlags = *controlNodeFlags & ~UI_NODE_SUPPRESSED;
    UiKeyboardFocus_AcquireIfNone(&control->base);
  }
  return;
}

/* Address: 0x004B2D30.
   Ownership: ui/controls/lists.
   Purpose: Variadic group test. CF=0 when at least one non-suppressed control has selected bit 0x02; CF=1 when
   none does. Kept distinct from player IDs, command opcodes, and resource identifiers. Typed parameters: p0
   controlCount→UiControlCount_V338. Calling convention, storage, body bytes, control flow, and executable data
   remain unchanged.
*/
void UiSelectableGroup_NoneVisibleSelectedCf(UiControlCount controlCount)

{
  uint controlIndex;
  int controlPointerByteOffset;
  
  controlPointerByteOffset = 0;
  controlIndex = 0;
  while (((*(uint *)(*(int *)(&stack0x00000008 + controlPointerByteOffset) + 0x48) & 8) != 0 ||
         ((*(uint *)(*(int *)(&stack0x00000008 + controlPointerByteOffset) + 0x4c) & 2) == 0))) {
    controlIndex = controlIndex + 1;
    controlPointerByteOffset = controlPointerByteOffset + 4;
    if (controlCount <= controlIndex) {
      return;
    }
  }
  return;
}

/* Address: 0x004B2D70.
   Ownership: ui/controls/lists.
   Purpose: Variadic group test that ignores node visibility. CF=0 when any listed control has selected bit 0x02;
   CF=1 when none does. Kept distinct from player IDs, command opcodes, and resource identifiers. Typed parameters:
   p0 controlCount→UiControlCount_V338. Calling convention, storage, body bytes, control flow, and executable data
   remain unchanged.
*/
void UiSelectableGroup_NoneSelectedCf(UiControlCount controlCount)

{
  uint controlIndex;
  int controlPointerByteOffset;
  
  controlPointerByteOffset = 0;
  controlIndex = 0;
  do {
    if ((*(uint *)(*(int *)(&stack0x00000008 + controlPointerByteOffset) + 0x4c) & 2) != 0) {
      return;
    }
    controlIndex = controlIndex + 1;
    controlPointerByteOffset = controlPointerByteOffset + 4;
  } while (controlIndex < controlCount);
  return;
}

/* Address: 0x004B2DA0.
   Ownership: ui/controls/lists.
   Purpose: Variadic exclusive-selection helper. Sets selected bit 0x02 only on the chosen control, clears it on
   the remaining controls, and invalidates every listed control. Kept distinct from player IDs, command opcodes,
   and resource identifiers. Typed parameters: p0 controlCount→UiControlCount_V338. Calling convention, storage,
   body bytes, control flow, and executable data remain unchanged.
   Cross-module calls: UiNode_InvalidateRoot [ui/core/runtime].
*/
undefined4
UiSelectableGroup_SelectExclusive(UiControlCount controlCount,UiNodeBase *selectedControl)

{
  UiNodeBase *node;
  undefined4 in_EAX;
  int extraout_ECX;
  int controlPointerByteOffset;
  
  controlPointerByteOffset = 0;
  do {
    node = *(UiNodeBase **)(&stack0x0000000c + controlPointerByteOffset);
    if (node == selectedControl) {
      node[1].nextSibling = (UiNodeBase *)((uint)node[1].nextSibling | 2);
    }
    else {
      node[1].nextSibling = (UiNodeBase *)((uint)node[1].nextSibling & 0xfffffffd);
    }
    UiNode_InvalidateRoot(node);
    controlPointerByteOffset = controlPointerByteOffset + 4;
  } while (extraout_ECX + 1U < controlCount);
  return in_EAX;
}

/* Address: 0x004B2DE0.
   Ownership: ui/controls/lists.
   Purpose: Tests one selectable control. CF=1 only when nodeFlags bit 0x08 is clear and stateFlags bit 0x02 is
   set; otherwise CF=0.
*/
void UiSelectableControl_IsSelectedCf(UiSelectableControl *control)

{
  if ((((control->base).nodeFlags & UI_NODE_SUPPRESSED) == 0) &&
     ((control->stateFlags & UI_SELECTABLE_SELECTED_OR_CHECKED) != 0)) {
    return;
  }
  return;
}

/* Address: 0x004B2E10.
   Ownership: ui/controls/lists.
   Purpose: Clears selected bit 0x02, sets it when the boolean argument is nonzero, then invalidates the control
   root. Typed parameters: p0 selected→UiBooleanState32_V342. Calling convention, exact VariableStorage
   serialization, function body bytes, control flow, globals, locals, and executable data remain unchanged.
   Cross-module calls: UiNode_InvalidateRoot [ui/core/runtime].
*/
void UiSelectableControl_SetSelected(UiBooleanState32 selected,UiSelectableControl *control)

{
  control->stateFlags = control->stateFlags & ~UI_SELECTABLE_SELECTED_OR_CHECKED;
  if (selected != 0) {
    control->stateFlags = control->stateFlags | UI_SELECTABLE_SELECTED_OR_CHECKED;
  }
  UiNode_InvalidateRoot(&control->base);
  return;
}

/* Address: 0x004B4920.
   Ownership: ui/controls/lists.
   Purpose: CF=0 when base.firstChild matches an entry in the page array; CF=1 when the active child is absent.
*/
void UiPageStack_ActivePageNotInListCf(UiPageStackControl *stack)

{
  uint pageIndex;
  
  pageIndex = 0;
  do {
    if ((stack->base).firstChild == (&stack->pages)[pageIndex]) {
      return;
    }
    pageIndex = pageIndex + 1;
  } while (pageIndex < stack->pageCount);
  return;
}

/* Address: 0x004B7970.
   Ownership: ui/controls/lists.
   Purpose: Binary entry is anchored by g_UiNodeVtable_004B7920[0]@004B7920.
   Cross-module calls: UiContainer_RelocateChildren [ui/controls/layout].
*/
void UiScrollableControl_RelocateChildren
               (UiSerializedRelocationDelta relocationDelta,UiScrollableControl *control)

{
  UiNodeBase *contentChild;
  UiPixelExtent contentHeight;
  
  UiContainer_RelocateChildren(relocationDelta,&control->base);
  contentChild = (control->base).firstChild;
  if (contentChild != (UiNodeBase *)0xffffffff) {
    contentHeight = contentChild->bottomOffset;
    control->contentWidth = contentChild->rightOffset;
    control->contentHeight = contentHeight;
    control->scrollOffsetX = 0;
    control->scrollOffsetY = 0;
  }
  return;
}

/* Address: 0x004B79D0.
   Ownership: ui/controls/lists.
   Purpose: Binary entry is anchored by g_UiNodeVtable_004B7920[2]@004B7920.
   Cross-module calls: UiWindow_BlitTiledHorizontalEdge [ui/controls/layout], UiWindow_BlitTiledVerticalEdge
   [ui/controls/layout], UiWindow_BlitTiledInterior [ui/controls/layout], UiContainer_DrawIntersectingChildren
   [ui/controls/layout].
*/
void UiScrollableControl_DrawFrameContentAndScrollbars
               (UiPixelCoordinate clipTop,UiPixelCoordinate clipLeft,UiPixelCoordinate clipBottom,
               UiPixelCoordinate clipRight,UiScrollableControl *control)

{
  GraphicsSubresourceIndex subresource;
  UiPixelCoordinate tileEnd;
  int extraout_ECX;
  int iVar1;
  UiPixelCoordinate extraout_ECX_00;
  UiPixelCoordinate extraout_ECX_01;
  UiPixelCoordinate extraout_ECX_02;
  UiPixelCoordinate extraout_ECX_03;
  UiPixelCoordinate UVar2;
  UiPixelCoordinate tileEnd_00;
  UiPixelCoordinate tileEnd_01;
  UiPixelCoordinate extraout_ECX_04;
  UiPixelCoordinate extraout_ECX_05;
  UiPixelCoordinate extraout_ECX_06;
  UiPixelCoordinate extraout_ECX_07;
  UiPixelCoordinate tileStart;
  UiPixelCoordinate tileStart_00;
  int extraout_ECX_08;
  int extraout_ECX_09;
  int extraout_ECX_10;
  int extraout_ECX_11;
  int extraout_ECX_12;
  int extraout_ECX_13;
  int extraout_ECX_14;
  int extraout_ECX_15;
  int extraout_ECX_16;
  int extraout_ECX_17;
  int extraout_ECX_18;
  UiPixelCoordinate extraout_ECX_19;
  int extraout_ECX_20;
  int iVar3;
  int extraout_EDX;
  UiPixelCoordinate tileStart_01;
  int extraout_EDX_00;
  int extraout_EDX_01;
  UiPixelCoordinate tileStart_02;
  UiPixelCoordinate extraout_EDX_02;
  int extraout_EDX_03;
  int iVar4;
  int iVar5;
  int iVar6;
  int iVar7;
  int iVar8;
  bool bVar9;
  qword qVar10;
  
  bVar9 = &stack0xffffffe4 < (undefined1 *)0x8;
  (*g_GraphicsFramebufferBeginAccess)();
  if (!bVar9) {
    tileEnd = 0;
    iVar3 = 0;
    iVar7 = (control->base).layoutWidth;
    iVar6 = (control->base).layoutHeight;
    if ((control->scrollStateFlags &
        (UI_SCROLL_HORIZONTAL_BAR_AT_BOTTOM|UI_SCROLL_HORIZONTAL_BAR_AT_TOP)) != 0) {
      tileEnd = 0;
      iVar3 = 0;
      iVar8 = iVar7;
      if ((control->scrollStateFlags & UI_SCROLL_HORIZONTAL_BAR_AT_BOTTOM) == 0) {
        iVar4 = 0;
        qVar10 = (*g_GraphicsTextureSourceGetLogicalSize)(0x5a,g_UiWindowTextureSource);
        iVar3 = iVar3 + (int)(qVar10 >> 0x20);
      }
      else {
        iVar1 = iVar6;
        qVar10 = (*g_GraphicsTextureSourceGetLogicalSize)(0x5a,g_UiWindowTextureSource);
        iVar5 = (int)(qVar10 >> 0x20);
        iVar4 = iVar6 - iVar5;
        iVar6 = iVar1 - iVar5;
      }
      qVar10 = (*g_GraphicsTextureSourceGetLogicalSize)(0x5e,g_UiWindowTextureSource);
      iVar1 = extraout_ECX;
      if ((control->scrollStateFlags & UI_SCROLL_VERTICAL_BAR_AT_LEFT) != 0) {
        iVar1 = extraout_ECX + (int)qVar10;
      }
      if ((control->scrollStateFlags & UI_SCROLL_VERTICAL_BAR_AT_RIGHT) != 0) {
        iVar7 = iVar7 - (int)qVar10;
      }
      if (((control->scrollStateFlags & UI_SCROLL_HORIZONTAL_DECREMENT_ACTIVE) == 0) ||
         ((control->scrollStateFlags & UI_SCROLL_PRIMARY_INTERACTION_ACTIVE) == 0)) {
        qVar10 = (*g_GraphicsTextureSourceBlitSourceAlpha)
                           (clipTop,clipLeft,clipBottom,clipRight,iVar4 + (control->base).top,
                            iVar1 + (control->base).left,0x5a,g_UiWindowTextureSource,
                            g_FramebufferAccess);
        iVar1 = (int)(qVar10 >> 0x20);
      }
      else {
        qVar10 = (*g_GraphicsTextureSourceBlitSourceAlpha)
                           (clipTop,clipLeft,clipBottom,clipRight,iVar4 + (control->base).top,
                            iVar1 + (control->base).left,0x62,g_UiWindowTextureSource,
                            g_FramebufferAccess);
        iVar1 = (int)(qVar10 >> 0x20);
      }
      iVar1 = iVar7 - iVar1;
      if (((control->scrollStateFlags & UI_SCROLL_HORIZONTAL_INCREMENT_ACTIVE) == 0) ||
         ((control->scrollStateFlags & UI_SCROLL_PRIMARY_INTERACTION_ACTIVE) == 0)) {
        (*g_GraphicsTextureSourceBlitSourceAlpha)
                  (clipTop,clipLeft,clipBottom,clipRight,iVar4 + (control->base).top,
                   iVar1 + (control->base).left,0x5b,g_UiWindowTextureSource,g_FramebufferAccess);
        UVar2 = extraout_ECX_01;
        iVar7 = iVar8;
      }
      else {
        (*g_GraphicsTextureSourceBlitSourceAlpha)
                  (clipTop,clipLeft,clipBottom,clipRight,iVar4 + (control->base).top,
                   iVar1 + (control->base).left,99,g_UiWindowTextureSource,g_FramebufferAccess);
        UVar2 = extraout_ECX_00;
        iVar7 = iVar8;
      }
      if ((control->scrollStateFlags & UI_SCROLL_HORIZONTAL_TRACK_BEFORE_THUMB_ACTIVE) == 0) {
        UiWindow_BlitTiledHorizontalEdge
                  (clipTop,clipLeft,clipBottom,clipRight,0x5d,control->horizontalThumbLeft,iVar4,
                   UVar2,control);
        UVar2 = extraout_ECX_03;
      }
      else {
        UiWindow_BlitTiledHorizontalEdge
                  (clipTop,clipLeft,clipBottom,clipRight,0x65,control->horizontalThumbLeft,iVar4,
                   UVar2,control);
        UVar2 = extraout_ECX_02;
      }
      iVar8 = control->horizontalThumbRight;
      if ((control->scrollStateFlags & UI_SCROLL_HORIZONTAL_TRACK_AFTER_THUMB_ACTIVE) == 0) {
        if (iVar8 < clipRight) {
          iVar8 = clipRight;
        }
        UiWindow_BlitTiledHorizontalEdge
                  (clipTop,clipLeft,clipBottom,iVar8,0x5d,iVar1,iVar4,UVar2,control);
      }
      else {
        if (iVar8 < clipRight) {
          iVar8 = clipRight;
        }
        UiWindow_BlitTiledHorizontalEdge
                  (clipTop,clipLeft,clipBottom,iVar8,0x65,iVar1,iVar4,UVar2,control);
      }
      (*g_GraphicsTextureSourceGetLogicalSize)(0xc0,g_UiWindowTextureSource);
      iVar8 = control->horizontalThumbRight;
      if ((control->scrollStateFlags & UI_SCROLL_HORIZONTAL_THUMB_ACTIVE) == 0) {
        qVar10 = (*g_GraphicsTextureSourceBlitSourceAlpha)
                           (clipTop,clipLeft,clipBottom,clipRight,iVar4 + (control->base).top,
                            control->horizontalThumbLeft + (control->base).left,0xc0,
                            g_UiWindowTextureSource,g_FramebufferAccess);
        iVar8 = iVar8 - (int)(qVar10 >> 0x20);
        (*g_GraphicsTextureSourceBlitSourceAlpha)
                  (clipTop,clipLeft,clipBottom,clipRight,iVar4 + (control->base).top,
                   iVar8 + (control->base).left,0xc1,g_UiWindowTextureSource,g_FramebufferAccess);
        UiWindow_BlitTiledHorizontalEdge
                  (clipTop,clipLeft,clipBottom,clipRight,0x5c,iVar8,iVar4,tileEnd_01,control);
      }
      else {
        qVar10 = (*g_GraphicsTextureSourceBlitSourceAlpha)
                           (clipTop,clipLeft,clipBottom,clipRight,iVar4 + (control->base).top,
                            control->horizontalThumbLeft + (control->base).left,0xc4,
                            g_UiWindowTextureSource,g_FramebufferAccess);
        iVar8 = iVar8 - (int)(qVar10 >> 0x20);
        (*g_GraphicsTextureSourceBlitSourceAlpha)
                  (clipTop,clipLeft,clipBottom,clipRight,iVar4 + (control->base).top,
                   iVar8 + (control->base).left,0xc5,g_UiWindowTextureSource,g_FramebufferAccess);
        UiWindow_BlitTiledHorizontalEdge
                  (clipTop,clipLeft,clipBottom,clipRight,100,iVar8,iVar4,tileEnd_00,control);
      }
    }
    if ((control->scrollStateFlags &
        (UI_SCROLL_VERTICAL_BAR_AT_RIGHT|UI_SCROLL_VERTICAL_BAR_AT_LEFT)) != 0) {
      iVar8 = iVar6;
      iVar4 = iVar3;
      if ((control->scrollStateFlags & UI_SCROLL_VERTICAL_BAR_AT_RIGHT) == 0) {
        iVar1 = tileEnd;
        qVar10 = (*g_GraphicsTextureSourceGetLogicalSize)(0x5e,g_UiWindowTextureSource);
        iVar1 = iVar1 + (int)qVar10;
      }
      else {
        iVar5 = iVar7;
        iVar1 = tileEnd;
        qVar10 = (*g_GraphicsTextureSourceGetLogicalSize)(0x5e,g_UiWindowTextureSource);
        tileEnd = iVar7 - (int)qVar10;
        iVar7 = iVar5 - (int)qVar10;
      }
      if (((control->scrollStateFlags & UI_SCROLL_VERTICAL_DECREMENT_ACTIVE) == 0) ||
         ((control->scrollStateFlags & UI_SCROLL_PRIMARY_INTERACTION_ACTIVE) == 0)) {
        qVar10 = (*g_GraphicsTextureSourceBlitSourceAlpha)
                           (clipTop,clipLeft,clipBottom,clipRight,iVar3 + (control->base).top,
                            tileEnd + (control->base).left,0x5e,g_UiWindowTextureSource,
                            g_FramebufferAccess);
        iVar5 = (int)(qVar10 >> 0x20);
        iVar3 = iVar4;
      }
      else {
        qVar10 = (*g_GraphicsTextureSourceBlitSourceAlpha)
                           (clipTop,clipLeft,clipBottom,clipRight,iVar3 + (control->base).top,
                            tileEnd + (control->base).left,0x66,g_UiWindowTextureSource,
                            g_FramebufferAccess);
        iVar5 = (int)(qVar10 >> 0x20);
        iVar3 = iVar4;
      }
      iVar5 = iVar6 - iVar5;
      if (((control->scrollStateFlags & UI_SCROLL_VERTICAL_INCREMENT_ACTIVE) == 0) ||
         ((control->scrollStateFlags & UI_SCROLL_PRIMARY_INTERACTION_ACTIVE) == 0)) {
        (*g_GraphicsTextureSourceBlitSourceAlpha)
                  (clipTop,clipLeft,clipBottom,clipRight,iVar5 + (control->base).top,
                   tileEnd + (control->base).left,0x5f,g_UiWindowTextureSource,g_FramebufferAccess);
        UVar2 = extraout_ECX_05;
        iVar6 = iVar8;
      }
      else {
        (*g_GraphicsTextureSourceBlitSourceAlpha)
                  (clipTop,clipLeft,clipBottom,clipRight,iVar5 + (control->base).top,
                   tileEnd + (control->base).left,0x67,g_UiWindowTextureSource,g_FramebufferAccess);
        UVar2 = extraout_ECX_04;
        iVar6 = iVar8;
      }
      if ((control->scrollStateFlags & UI_SCROLL_VERTICAL_TRACK_BEFORE_THUMB_ACTIVE) == 0) {
        UiWindow_BlitTiledVerticalEdge
                  (clipTop,clipLeft,clipBottom,clipRight,0x61,control->verticalThumbTop,UVar2,
                   tileEnd,control);
        UVar2 = extraout_ECX_07;
      }
      else {
        UiWindow_BlitTiledVerticalEdge
                  (clipTop,clipLeft,clipBottom,clipRight,0x69,control->verticalThumbTop,UVar2,
                   tileEnd,control);
        UVar2 = extraout_ECX_06;
      }
      iVar8 = control->verticalThumbBottom;
      if ((control->scrollStateFlags & UI_SCROLL_VERTICAL_TRACK_AFTER_THUMB_ACTIVE) == 0) {
        if (iVar8 < clipBottom) {
          iVar8 = clipBottom;
        }
        UiWindow_BlitTiledVerticalEdge
                  (clipTop,clipLeft,iVar8,clipRight,0x61,iVar5,UVar2,tileEnd,control);
      }
      else {
        if (iVar8 < clipBottom) {
          iVar8 = clipBottom;
        }
        UiWindow_BlitTiledVerticalEdge
                  (clipTop,clipLeft,iVar8,clipRight,0x69,iVar5,UVar2,tileEnd,control);
      }
      (*g_GraphicsTextureSourceGetLogicalSize)(0xc2,g_UiWindowTextureSource);
      iVar8 = control->verticalThumbBottom;
      if ((control->scrollStateFlags & UI_SCROLL_VERTICAL_THUMB_ACTIVE) == 0) {
        qVar10 = (*g_GraphicsTextureSourceBlitSourceAlpha)
                           (clipTop,clipLeft,clipBottom,clipRight,
                            control->verticalThumbTop + (control->base).top,
                            tileEnd + (control->base).left,0xc2,g_UiWindowTextureSource,
                            g_FramebufferAccess);
        iVar8 = iVar8 - (int)(qVar10 >> 0x20);
        (*g_GraphicsTextureSourceBlitSourceAlpha)
                  (clipTop,clipLeft,clipBottom,clipRight,iVar8 + (control->base).top,
                   tileEnd + (control->base).left,0xc3,g_UiWindowTextureSource,g_FramebufferAccess);
        UiWindow_BlitTiledVerticalEdge
                  (clipTop,clipLeft,clipBottom,clipRight,0x60,iVar8,tileStart_00,tileEnd,control);
        tileEnd = iVar1;
      }
      else {
        qVar10 = (*g_GraphicsTextureSourceBlitSourceAlpha)
                           (clipTop,clipLeft,clipBottom,clipRight,
                            control->verticalThumbTop + (control->base).top,
                            tileEnd + (control->base).left,0xc6,g_UiWindowTextureSource,
                            g_FramebufferAccess);
        iVar8 = iVar8 - (int)(qVar10 >> 0x20);
        (*g_GraphicsTextureSourceBlitSourceAlpha)
                  (clipTop,clipLeft,clipBottom,clipRight,iVar8 + (control->base).top,
                   tileEnd + (control->base).left,199,g_UiWindowTextureSource,g_FramebufferAccess);
        UiWindow_BlitTiledVerticalEdge
                  (clipTop,clipLeft,clipBottom,clipRight,0x68,iVar8,tileStart,tileEnd,control);
        tileEnd = iVar1;
      }
    }
    if ((control->scrollStateFlags & 0x400) != 0) {
      qVar10 = (*g_GraphicsTextureSourceGetLogicalSize)(0x6a,g_UiWindowTextureSource);
      iVar4 = (int)(qVar10 >> 0x20);
      iVar8 = (int)qVar10;
      iVar6 = iVar6 - iVar4;
      iVar7 = iVar7 - iVar8;
      qVar10 = (*g_GraphicsTextureSourceBlitSourceAlpha)
                         (clipTop,clipLeft,clipBottom,clipRight,iVar3 + (control->base).top,
                          extraout_ECX_08 + (control->base).left,0x6a,g_UiWindowTextureSource,
                          g_FramebufferAccess);
      (*g_GraphicsTextureSourceBlitSourceAlpha)
                (clipTop,clipLeft,clipBottom,clipRight,(int)(qVar10 >> 0x20) + (control->base).top,
                 iVar7 + (control->base).left,0x6b,g_UiWindowTextureSource,g_FramebufferAccess);
      (*g_GraphicsTextureSourceBlitSourceAlpha)
                (clipTop,clipLeft,clipBottom,clipRight,iVar6 + (control->base).top,
                 extraout_ECX_09 + (control->base).left,0x6c,g_UiWindowTextureSource,
                 g_FramebufferAccess);
      qVar10 = (*g_GraphicsTextureSourceBlitSourceAlpha)
                         (clipTop,clipLeft,clipBottom,clipRight,iVar6 + (control->base).top,
                          iVar7 + (control->base).left,0x6d,g_UiWindowTextureSource,
                          g_FramebufferAccess);
      UiWindow_BlitTiledHorizontalEdge
                (clipTop,clipLeft,clipBottom,clipRight,0x6e,iVar7,
                 (UiPixelCoordinate)(qVar10 >> 0x20),extraout_ECX_10 + iVar8,control);
      UiWindow_BlitTiledVerticalEdge
                (clipTop,clipLeft,clipBottom,clipRight,0x6f,iVar6,extraout_EDX + iVar4,
                 extraout_ECX_11 - iVar8,control);
      UiWindow_BlitTiledVerticalEdge
                (clipTop,clipLeft,clipBottom,clipRight,0x70,iVar6,tileStart_01,iVar7,control);
      UiWindow_BlitTiledHorizontalEdge
                (clipTop,clipLeft,clipBottom,clipRight,0x71,iVar7,iVar6,extraout_ECX_12 + iVar8,
                 control);
      tileEnd = extraout_ECX_13;
      iVar3 = extraout_EDX_00;
    }
    if ((control->scrollStateFlags & 0x800) != 0) {
      qVar10 = (*g_GraphicsTextureSourceGetLogicalSize)(0x72,g_UiWindowTextureSource);
      iVar4 = (int)(qVar10 >> 0x20);
      iVar8 = (int)qVar10;
      iVar6 = iVar6 - iVar4;
      iVar7 = iVar7 - iVar8;
      qVar10 = (*g_GraphicsTextureSourceBlitSourceAlpha)
                         (clipTop,clipLeft,clipBottom,clipRight,iVar3 + (control->base).top,
                          extraout_ECX_14 + (control->base).left,0x72,g_UiWindowTextureSource,
                          g_FramebufferAccess);
      (*g_GraphicsTextureSourceBlitSourceAlpha)
                (clipTop,clipLeft,clipBottom,clipRight,(int)(qVar10 >> 0x20) + (control->base).top,
                 iVar7 + (control->base).left,0x73,g_UiWindowTextureSource,g_FramebufferAccess);
      (*g_GraphicsTextureSourceBlitSourceAlpha)
                (clipTop,clipLeft,clipBottom,clipRight,iVar6 + (control->base).top,
                 extraout_ECX_15 + (control->base).left,0x74,g_UiWindowTextureSource,
                 g_FramebufferAccess);
      qVar10 = (*g_GraphicsTextureSourceBlitSourceAlpha)
                         (clipTop,clipLeft,clipBottom,clipRight,iVar6 + (control->base).top,
                          iVar7 + (control->base).left,0x75,g_UiWindowTextureSource,
                          g_FramebufferAccess);
      UiWindow_BlitTiledHorizontalEdge
                (clipTop,clipLeft,clipBottom,clipRight,0x76,iVar7,
                 (UiPixelCoordinate)(qVar10 >> 0x20),extraout_ECX_16 + iVar8,control);
      UiWindow_BlitTiledVerticalEdge
                (clipTop,clipLeft,clipBottom,clipRight,0x77,iVar6,extraout_EDX_01 + iVar4,
                 extraout_ECX_17 - iVar8,control);
      UiWindow_BlitTiledVerticalEdge
                (clipTop,clipLeft,clipBottom,clipRight,0x78,iVar6,tileStart_02,iVar7,control);
      UiWindow_BlitTiledHorizontalEdge
                (clipTop,clipLeft,clipBottom,clipRight,0x79,iVar7,iVar6,extraout_ECX_18 + iVar8,
                 control);
      tileEnd = extraout_ECX_19;
      iVar3 = extraout_EDX_02;
    }
    if ((control->scrollStateFlags & 0x300) != 0) {
      subresource = 0;
      if ((control->scrollStateFlags & 0x100) != 0) {
        subresource = 0x7a;
      }
      UiWindow_BlitTiledInterior
                (clipTop,clipLeft,clipBottom,clipRight,subresource,iVar6,iVar7,iVar3,tileEnd,control
                );
    }
    (*g_GraphicsFramebufferEndAccess)();
    iVar3 = extraout_ECX_20 + (control->base).left;
    iVar8 = extraout_EDX_03 + (control->base).top;
    iVar7 = iVar7 + (control->base).left;
    iVar6 = iVar6 + (control->base).top;
    if (iVar3 < clipRight) {
      iVar3 = clipRight;
    }
    if (iVar8 < clipBottom) {
      iVar8 = clipBottom;
    }
    if (clipLeft < iVar7) {
      iVar7 = clipLeft;
    }
    if (clipTop < iVar6) {
      iVar6 = clipTop;
    }
    UiContainer_DrawIntersectingChildren(iVar6,iVar7,iVar8,iVar3,&control->base);
  }
  return;
}

/* Address: 0x004B8310.
   Ownership: ui/controls/lists.
   Purpose: Rebuilds the scrollable child rectangle, content extents, horizontal and vertical scrollbar visibility,
   track bounds, thumb geometry, and clamped content offsets from the control rectangle and active child
   dimensions.
*/
undefined8 UiScrollableControl_RebuildViewportAndScrollbars(UiScrollableControl *control)

{
  UiNodeBase *pUVar1;
  UiPixelExtent UVar2;
  UiPixelExtent UVar3;
  undefined4 in_EAX;
  int iVar4;
  UiPixelOffset UVar5;
  uint extraout_ECX;
  uint extraout_ECX_00;
  uint uVar6;
  undefined4 in_EDX;
  int extraout_EDX;
  int extraout_EDX_00;
  UiPixelOffset UVar7;
  int extraout_EDX_01;
  int extraout_EDX_02;
  UiPixelExtent UVar8;
  int iVar9;
  UiPixelExtent UVar10;
  int iVar11;
  qword qVar12;
  
  pUVar1 = (control->base).firstChild;
  UVar10 = (control->base).right - (control->base).left;
  UVar8 = (control->base).bottom - (control->base).top;
  control->scrollStateFlags =
       control->scrollStateFlags &
       ~(UI_SCROLL_VERTICAL_BAR_AT_RIGHT|UI_SCROLL_VERTICAL_BAR_AT_LEFT|
         UI_SCROLL_HORIZONTAL_BAR_AT_BOTTOM|UI_SCROLL_HORIZONTAL_BAR_AT_TOP);
  (control->base).layoutWidth = UVar10;
  (control->base).layoutHeight = UVar8;
  if (pUVar1 != (UiNodeBase *)0xffffffff) {
    UVar5 = control->scrollOffsetY;
    iVar11 = (control->base).top;
    pUVar1->left = pUVar1->leftOffset + control->scrollOffsetX + (control->base).left;
    pUVar1->top = pUVar1->topOffset + UVar5 + iVar11;
    UVar2 = pUVar1->rightOffset;
    UVar3 = pUVar1->bottomOffset;
    control->contentWidth = UVar2;
    control->contentHeight = UVar3;
    UVar5 = control->scrollOffsetY;
    iVar11 = (control->base).top;
    pUVar1->right = UVar2 + control->scrollOffsetX + (control->base).left;
    pUVar1->bottom = UVar3 + UVar5 + iVar11;
    control->contentOriginX = 0;
    control->contentOriginY = 0;
    if ((control->scrollStateFlags & 0x400) != 0) {
      qVar12 = (*g_GraphicsTextureSourceGetLogicalSize)(0x6a,g_UiWindowTextureSource);
      iVar11 = (int)(qVar12 >> 0x20);
      control->contentOriginX = control->contentOriginX + (int)qVar12;
      control->contentOriginY = control->contentOriginY + iVar11;
      UVar10 = UVar10 + (int)qVar12 * -2;
      UVar8 = UVar8 + iVar11 * -2;
    }
    if ((control->scrollStateFlags & 0x800) != 0) {
      qVar12 = (*g_GraphicsTextureSourceGetLogicalSize)(0x72,g_UiWindowTextureSource);
      iVar11 = (int)(qVar12 >> 0x20);
      control->contentOriginX = control->contentOriginX + (int)qVar12;
      control->contentOriginY = control->contentOriginY + iVar11;
      UVar10 = UVar10 + (int)qVar12 * -2;
      UVar8 = UVar8 + iVar11 * -2;
    }
    control->viewportWidth = UVar10;
    control->viewportHeight = UVar8;
    iVar11 = UVar10 - control->contentWidth;
    if (iVar11 < 0) {
      control->scrollStateFlags =
           control->scrollStateFlags |
           (UI_SCROLL_HORIZONTAL_BAR_AT_BOTTOM|UI_SCROLL_HORIZONTAL_BAR_AT_TOP);
    }
    iVar9 = UVar8 - control->contentHeight;
    if (iVar9 < 0) {
      control->scrollStateFlags =
           control->scrollStateFlags |
           (UI_SCROLL_VERTICAL_BAR_AT_RIGHT|UI_SCROLL_VERTICAL_BAR_AT_LEFT);
    }
    control->scrollStateFlags =
         control->scrollStateFlags &
         (control->scrollStateFlags >> 4 |
         ~(UI_SCROLL_VERTICAL_BAR_AT_RIGHT|UI_SCROLL_VERTICAL_BAR_AT_LEFT|
           UI_SCROLL_HORIZONTAL_BAR_AT_BOTTOM|UI_SCROLL_HORIZONTAL_BAR_AT_TOP));
    if (((control->scrollStateFlags & 0xffffffcf) != 0) &&
       ((control->scrollStateFlags &
        (UI_SCROLL_VERTICAL_BAR_AT_RIGHT|UI_SCROLL_VERTICAL_BAR_AT_LEFT)) != 0)) {
      qVar12 = (*g_GraphicsTextureSourceGetLogicalSize)(0x5e,g_UiWindowTextureSource);
      iVar11 = iVar11 - (int)qVar12;
      if (iVar11 < 0) {
        control->scrollStateFlags =
             control->scrollStateFlags |
             (UI_SCROLL_HORIZONTAL_BAR_AT_BOTTOM|UI_SCROLL_HORIZONTAL_BAR_AT_TOP);
      }
    }
    if (((control->scrollStateFlags & 0xffffff3f) != 0) &&
       ((control->scrollStateFlags &
        (UI_SCROLL_HORIZONTAL_BAR_AT_BOTTOM|UI_SCROLL_HORIZONTAL_BAR_AT_TOP)) != 0)) {
      (*g_GraphicsTextureSourceGetLogicalSize)(0x5a,g_UiWindowTextureSource);
      if (iVar9 - extraout_EDX < 0) {
        if (((control->scrollStateFlags &
             (UI_SCROLL_HORIZONTAL_BAR_AT_BOTTOM|UI_SCROLL_HORIZONTAL_BAR_AT_TOP)) == 0) &&
           ((control->scrollStateFlags & 0xffffffcf) != 0)) {
          qVar12 = (*g_GraphicsTextureSourceGetLogicalSize)(0x5e,g_UiWindowTextureSource);
          if (iVar11 - (int)qVar12 < 0) {
            control->scrollStateFlags =
                 control->scrollStateFlags |
                 (UI_SCROLL_HORIZONTAL_BAR_AT_BOTTOM|UI_SCROLL_HORIZONTAL_BAR_AT_TOP);
          }
        }
        control->scrollStateFlags =
             control->scrollStateFlags |
             (UI_SCROLL_VERTICAL_BAR_AT_RIGHT|UI_SCROLL_VERTICAL_BAR_AT_LEFT);
      }
    }
    control->scrollStateFlags =
         control->scrollStateFlags &
         (control->scrollStateFlags >> 4 |
         ~(UI_SCROLL_VERTICAL_BAR_AT_RIGHT|UI_SCROLL_VERTICAL_BAR_AT_LEFT|
           UI_SCROLL_HORIZONTAL_BAR_AT_BOTTOM|UI_SCROLL_HORIZONTAL_BAR_AT_TOP));
    if ((control->scrollStateFlags &
        (UI_SCROLL_HORIZONTAL_BAR_AT_BOTTOM|UI_SCROLL_HORIZONTAL_BAR_AT_TOP)) != 0) {
      (*g_GraphicsTextureSourceGetLogicalSize)(0x5a,g_UiWindowTextureSource);
      control->viewportHeight = control->viewportHeight - extraout_EDX_00;
      if ((control->scrollStateFlags & UI_SCROLL_HORIZONTAL_BAR_AT_TOP) != 0) {
        control->contentOriginY = control->contentOriginY + extraout_EDX_00;
      }
    }
    if ((control->scrollStateFlags &
        (UI_SCROLL_VERTICAL_BAR_AT_RIGHT|UI_SCROLL_VERTICAL_BAR_AT_LEFT)) != 0) {
      qVar12 = (*g_GraphicsTextureSourceGetLogicalSize)(0x5e,g_UiWindowTextureSource);
      control->viewportWidth = control->viewportWidth - (int)qVar12;
      if ((control->scrollStateFlags & UI_SCROLL_VERTICAL_BAR_AT_LEFT) != 0) {
        control->contentOriginX = control->contentOriginX + (int)qVar12;
      }
    }
    UVar5 = control->scrollOffsetX;
    UVar7 = control->scrollOffsetY;
    pUVar1 = (control->base).firstChild;
    iVar9 = (control->contentWidth - control->viewportWidth) + UVar5;
    iVar11 = (control->contentHeight - control->viewportHeight) + UVar7;
    if (iVar9 < 0) {
      control->scrollOffsetX = control->scrollOffsetX - iVar9;
      pUVar1->left = pUVar1->left - iVar9;
      pUVar1->right = pUVar1->right - iVar9;
      UVar5 = UVar5 - iVar9;
    }
    if (iVar11 < 0) {
      control->scrollOffsetY = control->scrollOffsetY - iVar11;
      pUVar1->top = pUVar1->top - iVar11;
      pUVar1->bottom = pUVar1->bottom - iVar11;
      UVar7 = UVar7 - iVar11;
    }
    if (-1 < (int)UVar5) {
      control->scrollOffsetX = 0;
      pUVar1->left = pUVar1->left - UVar5;
      pUVar1->right = pUVar1->right - UVar5;
    }
    if (-1 < (int)UVar7) {
      control->scrollOffsetY = 0;
      pUVar1->top = pUVar1->top - UVar7;
      pUVar1->bottom = pUVar1->bottom - UVar7;
    }
    UVar5 = control->contentOriginX;
    UVar7 = control->contentOriginY;
    pUVar1->left = pUVar1->left + UVar5;
    pUVar1->top = pUVar1->top + UVar7;
    pUVar1->right = pUVar1->right + UVar5;
    pUVar1->bottom = pUVar1->bottom + UVar7;
    (*pUVar1->vtable->layout)(pUVar1);
    control->horizontalThumbLeft = 0;
    control->verticalThumbTop = 0;
    control->horizontalThumbRight = 0;
    control->verticalThumbBottom = 0;
    if ((control->scrollStateFlags & UI_SCROLL_HORIZONTAL_BAR_AT_TOP) != 0) {
      (*g_GraphicsTextureSourceGetLogicalSize)(0x5a,g_UiWindowTextureSource);
      control->verticalThumbTop = control->verticalThumbTop + extraout_EDX_01;
      control->verticalThumbBottom = control->verticalThumbBottom + extraout_EDX_01;
    }
    if ((control->scrollStateFlags & UI_SCROLL_VERTICAL_BAR_AT_LEFT) != 0) {
      qVar12 = (*g_GraphicsTextureSourceGetLogicalSize)(0x5e,g_UiWindowTextureSource);
      control->horizontalThumbLeft = control->horizontalThumbLeft + (int)qVar12;
      control->horizontalThumbRight = control->horizontalThumbRight + (int)qVar12;
    }
    iVar11 = (control->base).layoutWidth;
    iVar9 = (control->base).layoutHeight;
    if ((control->scrollStateFlags &
        (UI_SCROLL_HORIZONTAL_BAR_AT_BOTTOM|UI_SCROLL_HORIZONTAL_BAR_AT_TOP)) != 0) {
      qVar12 = (*g_GraphicsTextureSourceGetLogicalSize)(0x5a,g_UiWindowTextureSource);
      iVar4 = (int)qVar12;
      control->horizontalThumbLeft = control->horizontalThumbLeft + iVar4;
      control->horizontalThumbRight = control->horizontalThumbRight + iVar4;
      iVar11 = iVar11 + iVar4 * -2;
      iVar9 = iVar9 - (int)(qVar12 >> 0x20);
    }
    if ((control->scrollStateFlags &
        (UI_SCROLL_VERTICAL_BAR_AT_RIGHT|UI_SCROLL_VERTICAL_BAR_AT_LEFT)) != 0) {
      qVar12 = (*g_GraphicsTextureSourceGetLogicalSize)(0x5e,g_UiWindowTextureSource);
      iVar4 = (int)(qVar12 >> 0x20);
      control->verticalThumbTop = control->verticalThumbTop + iVar4;
      control->verticalThumbBottom = control->verticalThumbBottom + iVar4;
      iVar9 = iVar9 + iVar4 * -2;
      iVar11 = iVar11 - (int)qVar12;
    }
    if ((control->scrollStateFlags &
        (UI_SCROLL_HORIZONTAL_BAR_AT_BOTTOM|UI_SCROLL_HORIZONTAL_BAR_AT_TOP)) != 0) {
      qVar12 = (*g_GraphicsTextureSourceGetLogicalSize)(0xc0,g_UiWindowTextureSource);
      uVar6 = extraout_ECX;
      if (extraout_ECX < (uint)((int)qVar12 * 2)) {
        uVar6 = (int)qVar12 * 2;
      }
      control->horizontalThumbRight = control->horizontalThumbRight + uVar6;
      iVar11 = (int)(((longlong)(int)-control->scrollOffsetX * (longlong)(int)(iVar11 - uVar6)) /
                    (longlong)(int)(control->contentWidth - control->viewportWidth));
      control->horizontalThumbLeft = control->horizontalThumbLeft + iVar11;
      control->horizontalThumbRight = control->horizontalThumbRight + iVar11;
    }
    if ((control->scrollStateFlags &
        (UI_SCROLL_VERTICAL_BAR_AT_RIGHT|UI_SCROLL_VERTICAL_BAR_AT_LEFT)) != 0) {
      (*g_GraphicsTextureSourceGetLogicalSize)(0xc2,g_UiWindowTextureSource);
      uVar6 = extraout_ECX_00;
      if (extraout_ECX_00 < (uint)(extraout_EDX_02 * 2)) {
        uVar6 = extraout_EDX_02 * 2;
      }
      control->verticalThumbBottom = control->verticalThumbBottom + uVar6;
      iVar11 = (int)(((longlong)(int)-control->scrollOffsetY * (longlong)(int)(iVar9 - uVar6)) /
                    (longlong)(int)(control->contentHeight - control->viewportHeight));
      control->verticalThumbTop = control->verticalThumbTop + iVar11;
      control->verticalThumbBottom = control->verticalThumbBottom + iVar11;
    }
  }
  return CONCAT44(in_EDX,in_EAX);
}

/* Address: 0x004B8AC0.
   Ownership: ui/controls/lists.
   Purpose: Binary entry is anchored by g_UiNodeVtable_004B7920[10]@004B7920.
*/
GraphicsCursorFrameIndex
UiScrollableControl_QueryPointerRegion
          (UiPixelCoordinate pointerY,UiPixelCoordinate pointerX,UiScrollableControl *control)

{
  GraphicsCursorFrameIndex GVar1;
  
  GVar1 = 0;
  if (((control->scrollStateFlags & 0x4000) != 0) &&
     ((control->scrollStateFlags &
      (UI_SCROLL_VERTICAL_BAR_AT_RIGHT|UI_SCROLL_VERTICAL_BAR_AT_LEFT|
       UI_SCROLL_HORIZONTAL_BAR_AT_BOTTOM|UI_SCROLL_HORIZONTAL_BAR_AT_TOP)) != 0)) {
    GVar1 = 1;
    if ((control->scrollStateFlags &
        (UI_SCROLL_HORIZONTAL_BAR_AT_BOTTOM|UI_SCROLL_HORIZONTAL_BAR_AT_TOP)) == 0) {
      GVar1 = 4;
    }
    if ((control->scrollStateFlags &
        (UI_SCROLL_VERTICAL_BAR_AT_RIGHT|UI_SCROLL_VERTICAL_BAR_AT_LEFT)) == 0) {
      GVar1 = 5;
    }
  }
  return GVar1;
}

/* Address: 0x004B8B10.
   Ownership: ui/controls/lists.
   Purpose: Binary entry is anchored by g_UiNodeVtable_004B7920[6]@004B7920.
*/
void UiScrollableControl_BeginSecondaryScrollInteraction
               (UiPointerWheelDelta wheelDelta,UiPixelCoordinate pointerY,UiPixelCoordinate pointerX
               ,UiScrollableControl *control)

{
  int localPointerX;
  int localPointerY;
  dword cursorFrame;
  
  g_CursorUseOverridePosition = g_CursorUseOverridePosition + 1;
  control->scrollStateFlags = control->scrollStateFlags | 0x1000;
  control->pointerAnchorX = pointerX;
  control->pointerAnchorY = pointerY;
  localPointerX = pointerX - (control->base).left;
  localPointerY = pointerY - (control->base).top;
  control->scrollStateFlags = control->scrollStateFlags & 0xe0e0cfff;
  if (((((int)control->contentOriginX <= localPointerX) &&
       ((int)control->contentOriginY <= localPointerY)) &&
      ((int)(localPointerX - control->viewportWidth) < (int)control->contentOriginX)) &&
     (((int)(localPointerY - control->viewportHeight) < (int)control->contentOriginY &&
      (control->scrollStateFlags = control->scrollStateFlags | 0x4000,
      (control->scrollStateFlags &
      (UI_SCROLL_VERTICAL_BAR_AT_RIGHT|UI_SCROLL_VERTICAL_BAR_AT_LEFT|
       UI_SCROLL_HORIZONTAL_BAR_AT_BOTTOM|UI_SCROLL_HORIZONTAL_BAR_AT_TOP)) != 0)))) {
    cursorFrame = 1;
    if ((control->scrollStateFlags &
        (UI_SCROLL_HORIZONTAL_BAR_AT_BOTTOM|UI_SCROLL_HORIZONTAL_BAR_AT_TOP)) == 0) {
      cursorFrame = 4;
    }
    if ((control->scrollStateFlags &
        (UI_SCROLL_VERTICAL_BAR_AT_RIGHT|UI_SCROLL_VERTICAL_BAR_AT_LEFT)) == 0) {
      cursorFrame = 5;
    }
    (*g_GraphicsCursorSetFrame)(cursorFrame);
  }
  return;
}

/* Address: 0x004B8BC0.
   Ownership: ui/controls/lists.
   Purpose: Binary entry is anchored by g_UiNodeVtable_004B7920[7]@004B7920.
*/
void UiScrollableControl_EndSecondaryScrollInteraction(void)

{
  int in_stack_00000010;
  
  g_CursorUseOverridePosition = 0;
  *(uint *)(in_stack_00000010 + 0x4c) = *(uint *)(in_stack_00000010 + 0x4c) & 0xffffafff;
  (*g_GraphicsCursorSetFrame)(0);
  return;
}

/* Address: 0x004B9000.
   Ownership: ui/controls/lists.
   Purpose: Binary entry is anchored by g_UiNodeVtable_004B7920[11]@004B7920.
   Cross-module calls: UiContainer_HitTestChildren [ui/controls/layout].
*/
UiNodeBase *
UiScrollableControl_HitTestContentAndScrollbars
          (UiPixelCoordinate pointerY,UiPixelCoordinate pointerX,UiScrollableControl *control)

{
  int localPointerX;
  int localPointerY;
  
  localPointerX = pointerX - (control->base).left;
  localPointerY = pointerY - (control->base).top;
  if (((((control->scrollStateFlags & 0x4000) == 0) &&
       ((int)control->contentOriginX <= localPointerX)) &&
      ((int)control->contentOriginY <= localPointerY)) &&
     (((int)(localPointerX - control->viewportWidth) < (int)control->contentOriginX &&
      ((int)(localPointerY - control->viewportHeight) < (int)control->contentOriginY)))) {
    control = (UiScrollableControl *)UiContainer_HitTestChildren(pointerY,pointerX,&control->base);
  }
  return &control->base;
}

/* Address: 0x004BA4E0.
   Ownership: ui/controls/lists.
   Purpose: Handles ui pointer list get row slots variant a.
*/
undefined4 UiPointerList_GetRowSlotsVariantA(int param_1)

{
  return *(undefined4 *)(param_1 + 0x50);
}

/* Address: 0x004BA560.
   Ownership: ui/controls/lists.
   Purpose: Returns (+0x60 - +0x50)/4 in EAX. CF mirrors control flag 0x04: clear when absent and set when present.
*/
UiListRowIndex UiPointerList_GetSelectedIndexVariantACf(UiPointerListControl *control)

{
  UiListRowIndex selectedRowIndex;
  
  selectedRowIndex = (int)control->selectedRowSlot - (int)control->rowSlots >> 2;
  if ((control->listStateFlags & UI_LIST_SELECTION_CONFIRMED) == 0) {
    return selectedRowIndex;
  }
  return selectedRowIndex;
}

/* Address: 0x004BADE0.
   Ownership: ui/controls/lists.
   Purpose: Handles ui list control draw rows and selection.
   Cross-module calls: UiWindow_BlitTiledHorizontalEdge [ui/controls/layout], RichTextCommandStream_MeasureRegs
   [assets/text/richtext], RichTextCommandStream_DrawSingleLine [assets/text/richtext].
*/
void UiListControl_DrawRowsAndSelection
               (int clipTop,int clipLeft,int clipBottom,int clipRight,UiNodeBase *control)

{
  int iVar1;
  UiNodeBase *pUVar2;
  int iVar3;
  UiPixelCoordinate extraout_ECX;
  UiPixelCoordinate edgeY;
  int extraout_ECX_00;
  int extraout_ECX_01;
  UiPixelCoordinate extraout_ECX_02;
  UiPixelCoordinate extraout_ECX_03;
  int extraout_ECX_04;
  UiPixelCoordinate edgeY_00;
  int extraout_ECX_05;
  int extraout_ECX_06;
  int iVar4;
  UiNodeBase **ppUVar5;
  UiNodeBase *pUVar6;
  UiNodeBase **ppUVar7;
  word *commandStream;
  bool bVar8;
  qword qVar9;
  RichTextExtentRegs RVar10;
  
  if (control[1].parent != (UiNodeBase *)0x0) {
    iVar1 = (clipBottom - control->top) / (int)control[1].vtable;
    if (iVar1 < 0) {
      iVar1 = 0;
    }
    ppUVar7 = &(control[1].firstChild)->nextSibling + iVar1;
    pUVar2 = (UiNodeBase *)
             (((clipTop - control->top) + (int)control[1].vtable) / (int)control[1].vtable);
    if (control[1].parent <= pUVar2) {
      pUVar2 = (UiNodeBase *)((int)&control[1].parent[-1].nodeFlags + 3);
    }
    ppUVar5 = &(control[1].firstChild)->nextSibling + (int)pUVar2;
    bVar8 = ppUVar7 < ppUVar5;
    if (ppUVar7 <= ppUVar5) {
      (*g_GraphicsFramebufferBeginAccess)();
      edgeY_00 = extraout_ECX;
      if (!bVar8) {
        do {
          if (ppUVar7 == (UiNodeBase **)control[1].top) {
            iVar1 = control->layoutWidth;
            if ((control->nodeFlags & UI_NODE_HAS_KEYBOARD_FOCUS) == 0) {
              UiWindow_BlitTiledHorizontalEdge
                        (clipTop,clipLeft,clipBottom,clipRight,0x82,iVar1,edgeY_00,0,control);
              edgeY_00 = extraout_ECX_03;
            }
            else {
              qVar9 = (*g_GraphicsTextureSourceGetLogicalSize)(0x83,g_UiWindowTextureSource);
              iVar1 = iVar1 - (int)qVar9;
              UiWindow_BlitTiledHorizontalEdge
                        (clipTop,clipLeft,clipBottom,clipRight,0x84,iVar1,edgeY,(int)qVar9,control);
              (*g_GraphicsTextureSourceBlitSourceAlpha)
                        (clipTop,clipLeft,clipBottom,clipRight,extraout_ECX_00 + control->top,
                         control->left,0x83,g_UiWindowTextureSource,g_FramebufferAccess);
              (*g_GraphicsTextureSourceBlitSourceAlpha)
                        (clipTop,clipLeft,clipBottom,clipRight,extraout_ECX_01 + control->top,
                         iVar1 + control->left,0x85,g_UiWindowTextureSource,g_FramebufferAccess);
              edgeY_00 = extraout_ECX_02;
            }
          }
          iVar1 = control[1].right;
          pUVar2 = *ppUVar7;
          if (iVar1 != 0) {
            iVar3 = 3;
            pUVar6 = control;
            do {
              iVar4 = pUVar6[1].leftOffset;
              if (iVar4 < 0) {
                iVar3 = iVar3 - iVar4;
                commandStream = (word *)((int)&pUVar2->nextSibling + pUVar6[1].topOffset);
                RVar10 = RichTextCommandStream_MeasureRegs(g_UiListTextStyle,commandStream);
                RichTextCommandStream_DrawSingleLine
                          (clipTop,clipLeft,clipBottom,clipRight,g_UiListTextStyle,commandStream,
                           extraout_ECX_05 + 1 + control->top,
                           (iVar3 - (RVar10.widthPixels + 6)) + control->left);
                iVar4 = extraout_ECX_06;
              }
              else {
                iVar3 = iVar3 + iVar4;
                RichTextCommandStream_DrawSingleLine
                          (clipTop,clipLeft,clipBottom,clipRight,g_UiListTextStyle,
                           (word *)((int)&pUVar2->nextSibling + pUVar6[1].topOffset),
                           edgeY_00 + 1 + control->top,(iVar3 - iVar4) + control->left);
                iVar4 = extraout_ECX_04;
              }
              edgeY_00 = iVar4 + -1;
              pUVar6 = (UiNodeBase *)&pUVar6->parent;
              iVar1 = iVar1 + -1;
            } while (iVar1 != 0);
          }
          ppUVar7 = ppUVar7 + 1;
          edgeY_00 = (int)&(control[1].vtable)->relocate + edgeY_00;
        } while (ppUVar7 <= ppUVar5);
        (*g_GraphicsFramebufferEndAccess)();
      }
    }
  }
  return;
}

/* Address: 0x004BB310.
   Ownership: ui/controls/lists.
   Purpose: Binary entry is anchored by g_UiNodeVtable_004BA590[16]@004BA590.
   Cross-module calls: UiActionQueue_Enqueue [ui/core/runtime].
*/
void UiListControl_TickActivationPulse(UiListControl *control)

{
  if (((control->listStateFlags & UI_LIST_DEFERRED_ACTION_PENDING) != 0) &&
     (control->listStateFlags = control->listStateFlags - 0x1000000,
     (control->listStateFlags & 0xff000000) == 0)) {
    control->listStateFlags = control->listStateFlags & 0xfffff9;
    UiActionQueue_Enqueue(control->actionId,control);
  }
  return;
}

/* Address: 0x004BB350.
   Ownership: ui/controls/lists.
   Purpose: Binary entry is anchored by g_UiNodeVtable_004BA590[15]@004BA590.
   Cross-module calls: UiContainer_UnsuppressActionId [ui/controls/layout].
*/
void UiListControl_UnsuppressIfActionId(UiActionId actionId,UiListControl *control)

{
  UiNodeFlags *controlNodeFlags;
  
  if (actionId == control->actionId) {
    controlNodeFlags = &(control->base).nodeFlags;
    *controlNodeFlags = *controlNodeFlags & ~UI_NODE_SUPPRESSED;
  }
  UiContainer_UnsuppressActionId(actionId,&control->base);
  return;
}

/* Address: 0x004BB380.
   Ownership: ui/controls/lists.
   Purpose: Binary entry is anchored by g_UiNodeVtable_004BA590[14]@004BA590.
   Cross-module calls: UiContainer_SuppressActionId [ui/controls/layout].
*/
void UiListControl_SuppressIfActionId(UiActionId actionId,UiListControl *control)

{
  UiNodeFlags *controlNodeFlags;
  
  if (actionId == control->actionId) {
    controlNodeFlags = &(control->base).nodeFlags;
    *controlNodeFlags = *controlNodeFlags | UI_NODE_SUPPRESSED;
  }
  UiContainer_SuppressActionId(actionId,&control->base);
  return;
}

/* Address: 0x004BB3B0.
   Ownership: ui/controls/lists.
   Purpose: Initializes a pointer-list control and derives content width from the absolute values of its fixed
   column offsets before clearing scroll state and requesting parent layout.
   Cross-module calls: FontGlyph_GetLogicalSizeForStyleRegs [assets/text/resources].
*/
void UiPointerList_InitializeColumnLayout
               (UiListRowCount rowCount,void **rowPointers,UiPointerListControl *control)

{
  UiNodeBase *pUVar1;
  UiNodeVtable *pUVar2;
  UiNodeBase *pUVar3;
  int extraout_EDX;
  int iVar4;
  UiPointerListControl *pUVar5;
  
  FontGlyph_GetLogicalSizeForStyleRegs(g_UiListTextStyle,0);
  control->rowHeight = extraout_EDX + 1U;
  control->rowCount = rowCount;
  control->rowSlots = rowPointers;
  control->selectedRowSlot = rowPointers;
  iVar4 = 6;
  pUVar3 = control[1].base.nextSibling;
  (control->base).bottomOffset = (extraout_EDX + 1U) * rowCount + 1;
  pUVar5 = control;
  for (; pUVar3 != (UiNodeBase *)0x0; pUVar3 = (UiNodeBase *)((int)&pUVar3[-1].nodeFlags + 3)) {
    pUVar1 = pUVar5[1].base.parent;
    if ((int)pUVar1 < 0) {
      pUVar1 = (UiNodeBase *)-(int)pUVar1;
    }
    iVar4 = (int)&pUVar1->nextSibling + iVar4;
    pUVar5 = (UiPointerListControl *)&(pUVar5->base).parent;
  }
  pUVar3 = (control->base).parent;
  pUVar2 = pUVar3->vtable;
  (control->base).rightOffset = iVar4;
  (control->base).leftOffset = 0;
  (control->base).topOffset = 0;
  (*pUVar2->layout)(pUVar3);
  return;
}

/* Address: 0x004BB460.
   Ownership: ui/controls/lists.
   Purpose: EXACT_DUPLICATE_UI_POINTER_LIST_ROW_SLOT_GETTER_VARIANT_B.
*/
undefined4 UiPointerList_GetRowSlotsVariantB(int param_1)

{
  return *(undefined4 *)(param_1 + 0x50);
}

/* Address: 0x004BB9E0.
   Ownership: ui/controls/lists.
   Purpose: Binary entry is anchored by g_UiNodeVtable_004BB990[0]@004BB990.
   Cross-module calls: UiContainer_RelocateChildren [ui/controls/layout].
*/
void UiTimedListControl_RelocateChildren
               (UiSerializedRelocationDelta relocationDelta,UiTimedListControl *control)

{
  UiContainer_RelocateChildren(relocationDelta,&control->base);
  return;
}

/* Address: 0x004BBA00.
   Ownership: ui/controls/lists.
   Purpose: Handles ui timed list control draw rows and selection.
   Cross-module calls: RichTextCommandStream_MeasureRegs [assets/text/richtext], UiWindow_BlitTiledHorizontalEdge
   [ui/controls/layout], RichTextCommandStream_DrawSingleLine [assets/text/richtext].
*/
void UiTimedListControl_DrawRowsAndSelection
               (int clipTop,int clipLeft,int clipBottom,int clipRight,UiNodeBase *control)

{
  byte *pbVar1;
  dword dVar2;
  int iVar3;
  sdword arg4;
  sdword extraout_ECX;
  sdword arg4_00;
  sdword extraout_ECX_00;
  int extraout_ECX_01;
  UiPixelCoordinate edgeY;
  UiPixelCoordinate edgeY_00;
  int extraout_ECX_02;
  int extraout_ECX_03;
  int extraout_ECX_04;
  int extraout_ECX_05;
  int extraout_ECX_06;
  sdword arg5;
  int tileEnd;
  int extraout_EDX;
  int extraout_EDX_00;
  sdword *unaff_EBP;
  UiNodeBase *pUVar4;
  UiNodeBase *pUVar5;
  sdword *psVar6;
  sdword *psVar7;
  bool bVar8;
  qword qVar9;
  RichTextExtentRegs RVar10;
  UiNodeBase *pUStack_20;
  
  bVar8 = &stack0xffffffe4 < (undefined1 *)0x4;
  (*g_GraphicsFramebufferBeginAccess)();
  if (!bVar8) {
    pUStack_20 = (UiNodeBase *)0x0;
    pUVar4 = control[1].firstChild;
    iVar3 = 0;
    if (pUVar4 != (UiNodeBase *)0x0) {
      pUVar5 = pUVar4->nextSibling;
      psVar6 = &pUVar4->left;
      if (pUVar5 != (UiNodeBase *)0x0) {
        do {
          arg5 = control->left;
          arg4 = iVar3 + control->top;
          pUVar4 = pUStack_20;
          if (pUStack_20 != (UiNodeBase *)0x0) {
            while (pbVar1 = (byte *)((int)&pUVar4[-1].nodeFlags + 3), pbVar1 != (byte *)0x0) {
              qVar9 = CONCAT44(arg5,pbVar1);
              if (*(int *)(&stack0xffffffd8 + (int)pbVar1 * 8) != 0) {
                qVar9 = (*g_GraphicsTextureSourceBlitSourceAlpha)
                                  (clipTop,clipLeft,clipBottom,clipRight,arg4,arg5,
                                   control[1].rightOffset,
                                   (GraphicsTextureSourceAsset *)control[1].bottom,
                                   g_FramebufferAccess);
                arg4 = extraout_ECX;
              }
              arg5 = (int)(qVar9 >> 0x20) + control[1].topAnchorQ31;
              pUVar4 = (UiNodeBase *)qVar9;
            }
            if (pUVar5 < (UiNodeBase *)0x2) {
              dVar2 = control[1].leftAnchorQ31;
            }
            else {
              dVar2 = control[1].bottomOffset;
            }
            qVar9 = (*g_GraphicsTextureSourceBlitSourceAlpha)
                              (clipTop,clipLeft,clipBottom,clipRight,arg4,arg5,dVar2,
                               (GraphicsTextureSourceAsset *)control[1].bottom,g_FramebufferAccess);
            iVar3 = (int)(qVar9 >> 0x20);
            arg4 = arg4_00;
            if ((psVar6[3] & 1U) != 0) {
              if ((psVar6[3] & 2U) == 0) {
                dVar2 = control[1].leftOffset;
              }
              else {
                dVar2 = control[1].topOffset;
              }
              qVar9 = (*g_GraphicsTextureSourceBlitSourceAlpha)
                                (clipTop,clipLeft,clipBottom,clipRight,arg4_00,iVar3,dVar2,
                                 (GraphicsTextureSourceAsset *)control[1].bottom,g_FramebufferAccess
                                );
              iVar3 = (int)(qVar9 >> 0x20);
              arg4 = extraout_ECX_00;
            }
            arg5 = iVar3 + control[1].topAnchorQ31;
          }
          qVar9 = (*g_GraphicsTextureSourceBlitSourceAlpha)
                            (clipTop,clipLeft,clipBottom,clipRight,arg4,arg5,psVar6[1],
                             (GraphicsTextureSourceAsset *)control[1].bottom,g_FramebufferAccess);
          iVar3 = extraout_ECX_01 - control->top;
          tileEnd = ((int)(qVar9 >> 0x20) + control[1].rightAnchorQ31) - control->left;
          if ((uint *)psVar6 == (uint *)control[1].top) {
            RVar10 = RichTextCommandStream_MeasureRegs(g_UiListTextStyle,(word *)*psVar6);
            iVar3 = RVar10.widthPixels + 6;
            if ((control->nodeFlags & UI_NODE_HAS_KEYBOARD_FOCUS) == 0) {
              UiWindow_BlitTiledHorizontalEdge
                        (clipTop,clipLeft,clipBottom,clipRight,0x82,iVar3 + tileEnd,edgeY,tileEnd,
                         control);
              iVar3 = extraout_ECX_05;
              tileEnd = extraout_EDX_00;
            }
            else {
              qVar9 = (*g_GraphicsTextureSourceGetLogicalSize)(0x83,g_UiWindowTextureSource);
              iVar3 = (iVar3 - (int)qVar9) + tileEnd;
              UiWindow_BlitTiledHorizontalEdge
                        (clipTop,clipLeft,clipBottom,clipRight,0x84,iVar3,edgeY_00,
                         (int)qVar9 + tileEnd,control);
              (*g_GraphicsTextureSourceBlitSourceAlpha)
                        (clipTop,clipLeft,clipBottom,clipRight,extraout_ECX_02 + control->top,
                         extraout_EDX + control->left,0x83,g_UiWindowTextureSource,
                         g_FramebufferAccess);
              qVar9 = (*g_GraphicsTextureSourceBlitSourceAlpha)
                                (clipTop,clipLeft,clipBottom,clipRight,
                                 extraout_ECX_03 + control->top,iVar3 + control->left,0x85,
                                 g_UiWindowTextureSource,g_FramebufferAccess);
              tileEnd = (int)(qVar9 >> 0x20);
              iVar3 = extraout_ECX_04;
            }
          }
          RichTextCommandStream_DrawSingleLine
                    (clipTop,clipLeft,clipBottom,clipRight,g_UiListTextStyle,(word *)*psVar6,
                     iVar3 + 1 + control->top,tileEnd + 3 + control->left);
          psVar7 = psVar6 + 4;
          iVar3 = extraout_ECX_06 + -1 + (int)control[1].vtable;
          pUVar4 = (UiNodeBase *)((int)&pUVar5[-1].nodeFlags + 3);
          pUVar5 = pUVar4;
          if ((((psVar6[3] & 1U) != 0) && ((psVar6[3] & 2U) != 0)) && (psVar6[2] != 0)) {
            pUVar5 = *(UiNodeBase **)psVar6[2];
            psVar7 = (sdword *)((uint *)psVar6[2] + 4);
            pUStack_20 = pUVar4;
          }
          while (psVar6 = psVar7, pUVar5 == (UiNodeBase *)0x0) {
            if (pUStack_20 == (UiNodeBase *)0x0) goto UiTimedListDraw_EndFramebufferAccess;
            pUVar5 = (UiNodeBase *)((int)&pUStack_20[-1].nodeFlags + 3);
            psVar7 = unaff_EBP;
            pUStack_20 = pUVar5;
          }
        } while( true );
      }
    }
UiTimedListDraw_EndFramebufferAccess:
    (*g_GraphicsFramebufferEndAccess)();
  }
  return;
}

/* Address: 0x004BC180.
   Ownership: ui/controls/lists.
   Purpose: Binary entry is anchored by g_UiNodeVtable_004BB990[16]@004BB990.
   Cross-module calls: UiActionQueue_Enqueue [ui/core/runtime].
*/
void UiTimedListControl_TickActionDelay(UiTimedListControl *control)

{
  if (((control->listStateAndDelay & UI_TIMED_LIST_ACTION_DELAY_PENDING) != 0) &&
     (control->listStateAndDelay = control->listStateAndDelay - 0x1000000,
     (control->listStateAndDelay & 0xff000000) == 0)) {
    control->listStateAndDelay = control->listStateAndDelay & 0xfffffd;
    UiActionQueue_Enqueue(control->actionId,control);
  }
  return;
}

/* Address: 0x004BC460.
   Ownership: ui/controls/lists.
   Purpose: Binary entry is anchored by g_UiNodeVtable_004BC410[0]@004BC410; g_UiNodeVtable_00517F10[0]@00517F10.
   Cross-module calls: UiContainer_RelocateChildren [ui/controls/layout].
*/
void UiListOffsetControl_RelocateAndApplyDeferredOffset
               (UiSerializedRelocationDelta relocationDelta,UiNodeBase *control)

{
  int extraout_EAX;
  
  UiContainer_RelocateChildren(relocationDelta,control);
  if (((uint)control[1].nextSibling & 0x20) != 0) {
    control[1].parent = (UiNodeBase *)((int)&(control[1].parent)->nextSibling + extraout_EAX);
    control[1].nextSibling = (UiNodeBase *)((uint)control[1].nextSibling & 0xffffffdf);
  }
  return;
}

/* Address: 0x00516580.
   Ownership: ui/controls/lists.
   Purpose: Draws the inherited sprite state and catalog-specific numeric/status overlays derived from
   runtimeDisplayValueQ4 and active game-state tables.
   Cross-module calls: RichTextCommandStream_MeasureRegs [assets/text/richtext],
   RichTextCommandStream_DrawSingleLine [assets/text/richtext].
*/
void UiCatalogEntryControl_DrawClipped
               (UiPixelCoordinate clipTop,UiPixelCoordinate clipLeft,UiPixelCoordinate clipBottom,
               UiPixelCoordinate clipRight,UiCatalogEntryControl *control)

{
  WorldRuntimeNode *pWVar1;
  int *piVar2;
  dword dVar3;
  int iVar4;
  int iVar5;
  FactionArmyAssetCount FVar6;
  int iVar7;
  bool bVar8;
  RichTextExtentRegs RVar9;
  dword arg6;
  GraphicsTextureSourceAsset *arg7;
  SoftwareFramebufferAccess *pSVar10;
  PckArmyAssetIdCatalog PVar11;
  UiPackedTextStyle UStack_20;
  
  if ((((control->command).sprite.selectable.base.nodeFlags & UI_NODE_SUPPRESSED) == 0) &&
     ((((control->command).sprite.selectable.stateFlags & UI_SELECTABLE_SELECTED_OR_CHECKED) != 0 ||
      (((control->command).sprite.selectable.stateFlags & 0x400) == 0)))) {
    bVar8 = false;
    (*g_GraphicsFramebufferBeginAccess)();
    if (!bVar8) {
      arg7 = (control->command).sprite.primaryTextureSource;
      pSVar10 = g_FramebufferAccess;
      if (((control->command).sprite.selectable.stateFlags & UI_SELECTABLE_SELECTED_OR_CHECKED) == 0
         ) {
        dVar3 = (control->command).sprite.normalSubresourceStartOrDescriptor;
      }
      else {
        if ((((control->command).sprite.selectable.stateFlags & 0x80) == 0) &&
           (((control->command).sprite.selectable.stateFlags & 0x800) != 0)) {
          arg7 = (control->command).sprite.alternateTextureSource;
        }
        dVar3 = (control->command).sprite.selectedSubresourceStart;
        if (((control->command).sprite.selectable.stateFlags & 0x40) != 0) {
          arg6 = (control->command).sprite.normalSubresourceStartOrDescriptor;
          if (((control->command).sprite.selectable.stateFlags & 0x80) != 0) {
            arg6 = arg6 + (control->command).sprite.animationFrameOffset;
          }
          (*g_GraphicsTextureSourceBlitSourceAlpha)
                    (clipTop,clipLeft,clipBottom,clipRight,
                     (control->command).sprite.selectable.base.top,
                     (control->command).sprite.selectable.base.left,arg6,
                     (control->command).sprite.primaryTextureSource,g_FramebufferAccess);
        }
      }
      if (((control->command).sprite.selectable.stateFlags & 0x80) != 0) {
        dVar3 = dVar3 + (control->command).sprite.animationFrameOffset;
      }
      (*g_GraphicsTextureSourceBlitSourceAlpha)
                (clipTop,clipLeft,clipBottom,clipRight,(control->command).sprite.selectable.base.top
                 ,(control->command).sprite.selectable.base.left,dVar3,arg7,pSVar10);
      iVar7 = (g_InGameRuntimeRoot->worldRuntime0A30).activeFactionRuntimeIndex;
      if ((int)g_GameFactionRuntimeImage.records[iVar7].xeniteCurrentQ4 <
          (int)control->runtimeDisplayValueQ4) {
        UStack_20 = 0x1050000;
      }
      else {
        UStack_20 = 0x1040000;
      }
      g_UiCatalogEntryRichTextScratchUtf16[0] = 0x20;
      g_UiCatalogEntryRichTextScratchUtf16[1] = 0;
      dVar3 = (*g_WideNumberFormatUtf16)
                        (WIDE_FORMAT_WRITE_TERMINATOR,0,10,1,control->runtimeDisplayValueQ4 >> 4,
                         g_UiCatalogEntryRichTextScratchUtf16 + 1);
      *(undefined4 *)((int)g_UiCatalogEntryRichTextScratchUtf16 + dVar3 + 2) = 0x20;
      RVar9 = RichTextCommandStream_MeasureRegs(0x1000000,g_UiCatalogEntryRichTextScratchUtf16);
      RichTextCommandStream_DrawSingleLine
                (clipTop,clipLeft,clipBottom,clipRight,UStack_20,
                 g_UiCatalogEntryRichTextScratchUtf16,
                 ((control->command).sprite.selectable.base.bottom - RVar9.heightPixels) + -2,
                 ((int)((control->command).sprite.selectable.base.layoutWidth - RVar9.widthPixels)
                 >> 1) + (control->command).sprite.selectable.base.left);
      iVar4 = 0x29;
      do {
        if ((int)control - (int)g_InGameRuntimeRoot ==
            g_UiCatalogGroup42OffsetTables[g_UiCatalogGroup42ColumnCount][iVar4]) {
          iVar5 = 0;
          PVar11 = g_UiCatalogGroup42Records[iVar4]->armyAssetId;
          for (FVar6 = g_GameFactionRuntimeImage.records[iVar7].secondaryArmyAssetCount; FVar6 != 0;
              FVar6 = FVar6 - 1) {
            if (g_UiCatalogGroup42Records[iVar4] ==
                *(UiCommandRuntimeRecordPrefix **)(iVar7 * 0x740 + 0x50f41c + FVar6 * 4)) {
              iVar5 = iVar5 + 1;
            }
          }
          if (iVar5 != 0) {
            g_UiCatalogEntryRichTextScratchUtf16[0] = 0x20;
            dVar3 = (*g_WideNumberFormatUtf16)
                              (WIDE_FORMAT_WRITE_TERMINATOR,0,10,1,iVar5,
                               g_UiCatalogEntryRichTextScratchUtf16 + 1);
            iVar7 = (control->command).sprite.selectable.base.top;
            *(undefined4 *)((int)g_UiCatalogEntryRichTextScratchUtf16 + dVar3 + 2) = 0x20;
            RichTextCommandStream_DrawSingleLine
                      (clipTop,clipLeft,clipBottom,clipRight,UStack_20,
                       g_UiCatalogEntryRichTextScratchUtf16,iVar7 + 2,
                       (control->command).sprite.selectable.base.left);
          }
          iVar7 = -1;
          for (pWVar1 = (g_InGameRuntimeRoot->worldRuntime0A30).ownerListHead;
              pWVar1 != (WorldRuntimeNode *)0x0; pWVar1 = (pWVar1->common).nextNode) {
            if ((((pWVar1[2].common.nextNode == (WorldRuntimeNode *)0x0) &&
                 (piVar2 = pWVar1->runtimePayload, *(int *)(*piVar2 + 0x4c) == 0xb)) &&
                (((piVar2[0x2e] == 1 &&
                  (((g_InGameRuntimeRoot->worldRuntime0A30).activeFactionRuntimeIndex ==
                    *(int *)(piVar2[2] + 0xc) && (PVar11 == piVar2[0x18])))) &&
                 (iVar4 = (int)(((longlong)piVar2[0x19] * 100) / (longlong)piVar2[0x1a]),
                 iVar7 <= iVar4)))) &&
               (UStack_20 = 0x1040000, iVar7 = iVar4, (piVar2[0x3b] & 1U) != 0)) {
              UStack_20 = 0x1050000;
            }
          }
          if (-1 < iVar7) {
            g_UiCatalogEntryRichTextScratchUtf16[0] = 0x20;
            dVar3 = (*g_WideNumberFormatUtf16)
                              (WIDE_FORMAT_WRITE_TERMINATOR,0,10,1,iVar7,
                               g_UiCatalogEntryRichTextScratchUtf16 + 1);
            *(undefined2 *)((int)g_UiCatalogEntryRichTextScratchUtf16 + dVar3 + 2) = 0x25;
            *(undefined4 *)((int)g_UiCatalogEntryRichTextScratchUtf16 + dVar3 + 4) = 0x20;
            RVar9 = RichTextCommandStream_MeasureRegs
                              (0x1000000,g_UiCatalogEntryRichTextScratchUtf16);
            RichTextCommandStream_DrawSingleLine
                      (clipTop,clipLeft,clipBottom,clipRight,UStack_20,
                       g_UiCatalogEntryRichTextScratchUtf16,
                       (control->command).sprite.selectable.base.top + 2,
                       (control->command).sprite.selectable.base.right - RVar9.widthPixels);
          }
          goto UiCatalogEntryControl_DrawClipped_EndFramebufferAccessAndReturn;
        }
        iVar4 = iVar4 + -1;
      } while (-1 < iVar4);
      iVar4 = 0x2f;
      while ((int)control - (int)g_InGameRuntimeRoot !=
             g_UiCatalogGroup48OffsetTables[g_UiCatalogGroup48ColumnCount][iVar4]) {
        iVar4 = iVar4 + -1;
        if (iVar4 < 0) goto UiCatalogEntryControl_DrawClipped_EndFramebufferAccessAndReturn;
      }
      iVar5 = 0;
      PVar11 = g_UiCatalogGroup48Records[iVar4]->armyAssetId;
      for (FVar6 = g_GameFactionRuntimeImage.records[iVar7].secondaryArmyAssetCount; FVar6 != 0;
          FVar6 = FVar6 - 1) {
        if (g_UiCatalogGroup48Records[iVar4] ==
            *(UiCommandRuntimeRecordPrefix **)(iVar7 * 0x740 + 0x50f41c + FVar6 * 4)) {
          iVar5 = iVar5 + 1;
        }
      }
      if (iVar5 != 0) {
        g_UiCatalogEntryRichTextScratchUtf16[0] = 0x20;
        dVar3 = (*g_WideNumberFormatUtf16)
                          (WIDE_FORMAT_WRITE_TERMINATOR,0,10,1,iVar5,
                           g_UiCatalogEntryRichTextScratchUtf16 + 1);
        iVar7 = (control->command).sprite.selectable.base.top;
        *(undefined4 *)((int)g_UiCatalogEntryRichTextScratchUtf16 + dVar3 + 2) = 0x20;
        RichTextCommandStream_DrawSingleLine
                  (clipTop,clipLeft,clipBottom,clipRight,UStack_20,
                   g_UiCatalogEntryRichTextScratchUtf16,iVar7 + 2,
                   (control->command).sprite.selectable.base.left);
      }
      iVar7 = (g_InGameRuntimeRoot->worldRuntime0A30).activeFactionRuntimeIndex;
      iVar4 = -1;
      for (pWVar1 = (g_InGameRuntimeRoot->worldRuntime0A30).ownerListHead;
          pWVar1 != (WorldRuntimeNode *)0x0; pWVar1 = (pWVar1->common).nextNode) {
        if (pWVar1[2].common.nextNode == (WorldRuntimeNode *)0x0) {
          piVar2 = pWVar1->runtimePayload;
          if (*(int *)(*piVar2 + 0x4c) == 0x16) {
            if ((((piVar2[0x2b] == 1) && (iVar7 == *(int *)(piVar2[2] + 0xc))) &&
                (PVar11 == piVar2[0x18])) &&
               ((iVar5 = (int)(((longlong)piVar2[0x19] * 100) / (longlong)piVar2[0x1a]),
                iVar4 <= iVar5 && (UStack_20 = 0x1040000, iVar4 = iVar5, (piVar2[0x3b] & 1U) != 0)))
               ) {
              UStack_20 = 0x1050000;
            }
          }
          else if (((*(int *)(*piVar2 + 0x4c) == 0xd) && (piVar2[0x2e] == 1)) &&
                  ((iVar7 == *(int *)(piVar2[2] + 0xc) &&
                   (((PVar11 == piVar2[0x18] &&
                     (iVar5 = (int)(((longlong)piVar2[0x19] * 100) / (longlong)piVar2[0x1a]),
                     iVar4 <= iVar5)) &&
                    (UStack_20 = 0x1040000, iVar4 = iVar5, (piVar2[0x3b] & 1U) != 0)))))) {
            UStack_20 = 0x1050000;
          }
        }
      }
      if (-1 < iVar4) {
        g_UiCatalogEntryRichTextScratchUtf16[0] = 0x20;
        dVar3 = (*g_WideNumberFormatUtf16)
                          (WIDE_FORMAT_WRITE_TERMINATOR,0,10,1,iVar4,
                           g_UiCatalogEntryRichTextScratchUtf16 + 1);
        *(undefined2 *)((int)g_UiCatalogEntryRichTextScratchUtf16 + dVar3 + 2) = 0x25;
        *(undefined4 *)((int)g_UiCatalogEntryRichTextScratchUtf16 + dVar3 + 4) = 0x20;
        RVar9 = RichTextCommandStream_MeasureRegs(0x1000000,g_UiCatalogEntryRichTextScratchUtf16);
        RichTextCommandStream_DrawSingleLine
                  (clipTop,clipLeft,clipBottom,clipRight,UStack_20,
                   g_UiCatalogEntryRichTextScratchUtf16,
                   (control->command).sprite.selectable.base.top + 2,
                   (control->command).sprite.selectable.base.right - RVar9.widthPixels);
      }
UiCatalogEntryControl_DrawClipped_EndFramebufferAccessAndReturn:
      (*g_GraphicsFramebufferEndAccess)();
      return;
    }
  }
  return;
}

/* Address: 0x00516B90.
   Ownership: ui/controls/lists.
   Purpose: Maps a hovered catalog control to one of two verified static catalog tables, updates the active catalog
   selection, and returns the pointer cursor identifier.
   Cross-module calls: InGameSelectionDetailPanel_Rebuild [ui/ingame/runtime].
*/
GraphicsCursorFrameIndex
UiCatalogEntryControl_PointerMove
          (UiPixelCoordinate pointerY,UiPixelCoordinate pointerX,UiCatalogEntryControl *control)

{
  GraphicsCursorFrameIndex GVar1;
  int recordIndex;
  int iVar2;
  
  if (((control->command).sprite.selectable.base.nodeFlags & UI_NODE_SUPPRESSED) == 0) {
    recordIndex = 0x29;
    do {
      if ((int)control - (int)g_InGameRuntimeRoot ==
          g_UiCatalogGroup42OffsetTables[g_UiCatalogGroup42ColumnCount][recordIndex]) {
        g_UiHoverSelectionRecord = g_UiCatalogGroup42Records[recordIndex];
        InGameSelectionDetailPanel_Rebuild();
        goto UiCatalogEntryControl_PointerMove_ReturnCursorCodeAfterHoverResolution;
      }
      recordIndex = recordIndex + -1;
    } while (-1 < recordIndex);
    iVar2 = 0x2f;
    do {
      if ((int)control - (int)g_InGameRuntimeRoot ==
          g_UiCatalogGroup48OffsetTables[g_UiCatalogGroup48ColumnCount][iVar2]) {
        g_UiHoverSelectionRecord = g_UiCatalogGroup48Records[iVar2];
        InGameSelectionDetailPanel_Rebuild();
        break;
      }
      iVar2 = iVar2 + -1;
    } while (-1 < iVar2);
  }
UiCatalogEntryControl_PointerMove_ReturnCursorCodeAfterHoverResolution:
  GVar1 = 10;
  if ((g_KeyboardStateMask & 0xc) != 0) {
    GVar1 = 0xc;
  }
  return GVar1;
}

/* Address: 0x00516C50.
   Ownership: ui/controls/lists.
   Purpose: Handles ui catalog entry control non right release.
   Cross-module calls: UiActionQueue_Enqueue [ui/core/runtime], UiNode_InvalidateRoot [ui/core/runtime].
*/
void UiCatalogEntryControl_NonRightRelease
               (UiPointerWheelDelta wheelDelta,UiPixelCoordinate pointerY,UiPixelCoordinate pointerX
               ,UiCatalogEntryControl *control)

{
  UiSelectableStateFlags *stateFlagsField;
  dword activationInputState;
  
  activationInputState = g_KeyboardStateMask;
  if ((((control->command).sprite.selectable.base.nodeFlags & UI_NODE_SUPPRESSED) == 0) &&
     (((control->command).sprite.selectable.stateFlags & UI_SELECTABLE_SELECTED_OR_CHECKED) != 0)) {
    stateFlagsField = &(control->command).sprite.selectable.stateFlags;
    *stateFlagsField = *stateFlagsField & ~UI_SELECTABLE_SELECTED_OR_CHECKED;
    (control->command).activationInputState = activationInputState;
    if ((((control->command).sprite.selectable.stateFlags & 0x200) != 0) &&
       ((control->command).sprite.activationSoundId != 0)) {
      (*g_SoundPlayOneShot)
                (g_UiSoundGainQ15,g_UiSoundGainQ15,
                 (DirectSoundVoiceSet *)(control->command).sprite.activationSoundId);
    }
    UiActionQueue_Enqueue((control->command).sprite.selectable.actionId,control);
    UiNode_InvalidateRoot((UiNodeBase *)control);
  }
  return;
}

/* Address: 0x004BC360.
   Ownership: ui/controls/lists.
   Purpose: Handles ui timed list control select record and scroll into view.
   Local calls: UiTimedListTree_CountRecordArrayAndNestedChildren, UiScrollableControl_ClampOffsetsToViewport.
*/
void UiTimedListControl_SelectRecordAndScrollIntoView
               (UiTimedListTreeRecord16 *selectedRecord,UiTimedListRuntimeExtendedView88 *control)

{
  dword dVar1;
  int iVar2;
  int extraout_ECX;
  int iVar3;
  UiTimedListTreeRecord16 *pUVar4;
  
  iVar2 = 0;
  (control->base).selectedRecord = selectedRecord;
  do {
    pUVar4 = selectedRecord + -1;
    iVar3 = iVar2;
    if ((selectedRecord[-1].recordFlags0C & UI_TIMED_LIST_RECORD_ENABLES_NESTED_CHILD_TRAVERSAL) !=
        0) {
      dVar1 = UiTimedListTree_CountRecordArrayAndNestedChildren
                        (selectedRecord[-1].nestedRecordBlockOrParentLink08);
      iVar3 = extraout_ECX + dVar1;
    }
    selectedRecord = pUVar4;
    iVar2 = iVar3 + 1;
  } while (((selectedRecord->recordFlags0C & UI_TIMED_LIST_RECORD_ANCESTOR_BOUNDARY) == 0) ||
          (selectedRecord = selectedRecord->nestedRecordBlockOrParentLink08,
          selectedRecord != (UiTimedListTreeRecord16 *)0x0));
  iVar3 = iVar3 * (control->base).rowHeight;
  UiScrollableControl_ClampOffsetsToViewport
            (iVar3 + 1 + (control->base).rowHeight,(control->base).base.rightOffset,iVar3,0,
             (UiScrollableControl *)(control->base).base.parent);
  return;
}

/* Address: 0x004BB4E0.
   Ownership: ui/controls/lists.
   Purpose: Selects an in-range pointer-list entry by storing base + index*4 at +0x60 and invalidates the
   corresponding row using the variant-B geometry calculation.
   Local calls: UiScrollableControl_ClampOffsetsToViewport.
*/
undefined8 UiPointerList_SelectIndexVariantB(UiListRowIndex index,UiPointerListControl *control)

{
  undefined4 in_EAX;
  int clipBottom;
  undefined4 in_EDX;
  
  if (index < control->rowCount) {
    control->selectedRowSlot = control->rowSlots + index;
    clipBottom = control->rowHeight * index;
    UiScrollableControl_ClampOffsetsToViewport
              (clipBottom + 1 + control->rowHeight,(control->base).rightOffset,clipBottom,0,
               (UiScrollableControl *)(control->base).parent);
  }
  return CONCAT44(in_EDX,in_EAX);
}

/* Address: 0x004BB540.
   Ownership: ui/controls/lists.
   Purpose: Returns (+0x60 - +0x50)/4 in EAX. CF mirrors control flag 0x04 exactly.
*/
UiListRowIndex UiPointerList_GetSelectedIndexVariantBCf(UiPointerListControl *control)

{
  UiListRowIndex selectedRowIndex;
  
  selectedRowIndex = (int)control->selectedRowSlot - (int)control->rowSlots >> 2;
  if ((control->listStateFlags & UI_LIST_SELECTION_CONFIRMED) == 0) {
    return selectedRowIndex;
  }
  return selectedRowIndex;
}

/* Address: 0x004B9460.
   Ownership: ui/controls/lists.
   Purpose: Returns the verified scrollable-content width and height fields through EAX and EDX when the control
   uses the expected scrollable vtable, otherwise returns zero for both dimensions.
*/
UiScrollableContentDimensionsEdxEax8
UiScrollableControl_QueryContentSizeRegs(UiScrollableControl *control)

{
  UiPixelExtent contentWidth;
  UiPixelExtent contentHeight;
  
  contentWidth = 0;
  contentHeight = 0;
  if ((control->base).vtable == &g_UiScrollableControlVtable) {
    contentWidth = control->viewportWidth;
    contentHeight = control->viewportHeight;
  }
  return CONCAT44(contentHeight,contentWidth);
}

/* Address: 0x004BC310.
   Ownership: ui/controls/lists.
   Purpose: Handles ui timed list tree count record array and nested children.
*/
dword UiTimedListTree_CountRecordArrayAndNestedChildren(UiTimedListTreeRecord16 *recordBlock)

{
  dword dVar1;
  int extraout_ECX;
  dword extraout_EDX;
  dword dVar2;
  
  dVar1 = 0;
  if (recordBlock != (UiTimedListTreeRecord16 *)0x0) {
    dVar1 = recordBlock->recordCountOrRowPayload00;
    dVar2 = dVar1;
    do {
      if ((recordBlock[1].recordFlags0C & UI_TIMED_LIST_RECORD_ENABLES_NESTED_CHILD_TRAVERSAL) != 0)
      {
        dVar1 = UiTimedListTree_CountRecordArrayAndNestedChildren
                          (recordBlock[1].nestedRecordBlockOrParentLink08);
        dVar1 = extraout_ECX + dVar1;
        dVar2 = extraout_EDX;
      }
      dVar2 = dVar2 - 1;
      recordBlock = recordBlock + 1;
    } while (dVar2 != 0);
  }
  return dVar1;
}

/* Address: 0x004B9170.
   Ownership: ui/controls/lists.
   Purpose: Refreshes the active child rectangle from the current scroll offsets and recomputes the enabled
   scrollbar tracks and thumb positions without repeating the complete viewport-selection pass.
*/
undefined8 UiScrollableControl_RefreshChildAndScrollThumbs(UiScrollableControl *control)

{
  UiNodeBase *pUVar1;
  UiPixelExtent UVar2;
  UiPixelExtent UVar3;
  undefined4 in_EAX;
  int iVar4;
  UiPixelOffset UVar5;
  qword qVar10;
  uint extraout_ECX;
  uint extraout_ECX_00;
  uint uVar6;
  undefined4 in_EDX;
  UiPixelOffset UVar7;
  int extraout_EDX;
  int extraout_EDX_00;
  int iVar8;
  int iVar9;
  
  pUVar1 = (control->base).firstChild;
  if (pUVar1 != (UiNodeBase *)0xffffffff) {
    UVar5 = control->scrollOffsetY;
    iVar8 = (control->base).top;
    pUVar1->left = pUVar1->leftOffset + control->scrollOffsetX + (control->base).left;
    pUVar1->top = pUVar1->topOffset + UVar5 + iVar8;
    UVar2 = pUVar1->rightOffset;
    UVar3 = pUVar1->bottomOffset;
    control->contentWidth = UVar2;
    control->contentHeight = UVar3;
    UVar5 = control->scrollOffsetY;
    iVar8 = (control->base).top;
    pUVar1->right = UVar2 + control->scrollOffsetX + (control->base).left;
    pUVar1->bottom = UVar3 + UVar5 + iVar8;
    UVar5 = control->scrollOffsetX;
    UVar7 = control->scrollOffsetY;
    pUVar1 = (control->base).firstChild;
    iVar9 = (control->contentWidth - control->viewportWidth) + UVar5;
    iVar8 = (control->contentHeight - control->viewportHeight) + UVar7;
    if (iVar9 < 0) {
      control->scrollOffsetX = control->scrollOffsetX - iVar9;
      pUVar1->left = pUVar1->left - iVar9;
      pUVar1->right = pUVar1->right - iVar9;
      UVar5 = UVar5 - iVar9;
    }
    if (iVar8 < 0) {
      control->scrollOffsetY = control->scrollOffsetY - iVar8;
      pUVar1->top = pUVar1->top - iVar8;
      pUVar1->bottom = pUVar1->bottom - iVar8;
      UVar7 = UVar7 - iVar8;
    }
    if (-1 < (int)UVar5) {
      control->scrollOffsetX = 0;
      pUVar1->left = pUVar1->left - UVar5;
      pUVar1->right = pUVar1->right - UVar5;
    }
    if (-1 < (int)UVar7) {
      control->scrollOffsetY = 0;
      pUVar1->top = pUVar1->top - UVar7;
      pUVar1->bottom = pUVar1->bottom - UVar7;
    }
    UVar5 = control->contentOriginX;
    UVar7 = control->contentOriginY;
    pUVar1->left = pUVar1->left + UVar5;
    pUVar1->top = pUVar1->top + UVar7;
    pUVar1->right = pUVar1->right + UVar5;
    pUVar1->bottom = pUVar1->bottom + UVar7;
    (*pUVar1->vtable->layout)(pUVar1);
    control->horizontalThumbLeft = 0;
    control->verticalThumbTop = 0;
    control->horizontalThumbRight = 0;
    control->verticalThumbBottom = 0;
    if ((control->scrollStateFlags & UI_SCROLL_HORIZONTAL_BAR_AT_TOP) != 0) {
      (*g_GraphicsTextureSourceGetLogicalSize)(0x5a,g_UiWindowTextureSource);
      control->verticalThumbTop = control->verticalThumbTop + extraout_EDX;
      control->verticalThumbBottom = control->verticalThumbBottom + extraout_EDX;
    }
    if ((control->scrollStateFlags & UI_SCROLL_VERTICAL_BAR_AT_LEFT) != 0) {
      qVar10._0_4_ = (*g_GraphicsTextureSourceGetLogicalSize)(0x5e,g_UiWindowTextureSource);
      control->horizontalThumbLeft = control->horizontalThumbLeft + (int)(qword)qVar10;
      control->horizontalThumbRight = control->horizontalThumbRight + (int)(qword)qVar10;
    }
    iVar8 = (control->base).layoutWidth;
    iVar9 = (control->base).layoutHeight;
    if ((control->scrollStateFlags &
        (UI_SCROLL_HORIZONTAL_BAR_AT_BOTTOM|UI_SCROLL_HORIZONTAL_BAR_AT_TOP)) != 0) {
      qVar10 = (*g_GraphicsTextureSourceGetLogicalSize)(0x5a,g_UiWindowTextureSource);
      iVar4 = (int)qVar10;
      control->horizontalThumbLeft = control->horizontalThumbLeft + iVar4;
      control->horizontalThumbRight = control->horizontalThumbRight + iVar4;
      iVar8 = iVar8 + iVar4 * -2;
      iVar9 = iVar9 - (int)(qVar10 >> 0x20);
    }
    if ((control->scrollStateFlags &
        (UI_SCROLL_VERTICAL_BAR_AT_RIGHT|UI_SCROLL_VERTICAL_BAR_AT_LEFT)) != 0) {
      qVar10 = (*g_GraphicsTextureSourceGetLogicalSize)(0x5e,g_UiWindowTextureSource);
      iVar4 = (int)(qVar10 >> 0x20);
      control->verticalThumbTop = control->verticalThumbTop + iVar4;
      control->verticalThumbBottom = control->verticalThumbBottom + iVar4;
      iVar9 = iVar9 + iVar4 * -2;
      iVar8 = iVar8 - (int)qVar10;
    }
    if ((control->scrollStateFlags &
        (UI_SCROLL_HORIZONTAL_BAR_AT_BOTTOM|UI_SCROLL_HORIZONTAL_BAR_AT_TOP)) != 0) {
      qVar10._0_4_ = (*g_GraphicsTextureSourceGetLogicalSize)(0xc0,g_UiWindowTextureSource);
      uVar6 = extraout_ECX;
      if (extraout_ECX < (uint)((int)(qword)qVar10 * 2)) {
        uVar6 = (int)(qword)qVar10 * 2;
      }
      control->horizontalThumbRight = control->horizontalThumbRight + uVar6;
      iVar8 = (int)(((longlong)(int)-control->scrollOffsetX * (longlong)(int)(iVar8 - uVar6)) /
                   (longlong)(int)(control->contentWidth - control->viewportWidth));
      control->horizontalThumbLeft = control->horizontalThumbLeft + iVar8;
      control->horizontalThumbRight = control->horizontalThumbRight + iVar8;
    }
    if ((control->scrollStateFlags &
        (UI_SCROLL_VERTICAL_BAR_AT_RIGHT|UI_SCROLL_VERTICAL_BAR_AT_LEFT)) != 0) {
      (*g_GraphicsTextureSourceGetLogicalSize)(0xc2,g_UiWindowTextureSource);
      uVar6 = extraout_ECX_00;
      if (extraout_ECX_00 < (uint)(extraout_EDX_00 * 2)) {
        uVar6 = extraout_EDX_00 * 2;
      }
      control->verticalThumbBottom = control->verticalThumbBottom + uVar6;
      iVar8 = (int)(((longlong)(int)-control->scrollOffsetY * (longlong)(int)(iVar9 - uVar6)) /
                   (longlong)(int)(control->contentHeight - control->viewportHeight));
      control->verticalThumbTop = control->verticalThumbTop + iVar8;
      control->verticalThumbBottom = control->verticalThumbBottom + iVar8;
    }
  }
  return CONCAT44(in_EDX,in_EAX);
}

/* Address: 0x004B9490.
   Ownership: ui/controls/lists.
   Purpose: Clamps enabled horizontal and vertical scroll offsets to the visible viewport, relayouts when an offset
   changes, and invalidates the owning UI root. Typed parameters: p0 clipTop→UiPixelCoordinate_V297, p1
   clipLeft→UiPixelCoordinate_V297, p2 clipBottom→UiPixelCoordinate_V297, p3 clipRight→UiPixelCoordinate_V297.
   Calling convention, parameter storage, body bytes, control flow, globals, locals, and executable data remain
   unchanged.
   Local calls: UiScrollableControl_RefreshChildAndScrollThumbs.
   Cross-module calls: UiNode_InvalidateRoot [ui/core/runtime].
*/
void UiScrollableControl_ClampOffsetsToViewport
               (UiPixelCoordinate clipTop,UiPixelCoordinate clipLeft,UiPixelCoordinate clipBottom,
               UiPixelCoordinate clipRight,UiScrollableControl *control)

{
  char cVar1;
  int iVar2;
  int iVar3;
  int iVar4;
  int iVar5;
  int iVar6;
  
  if ((control->base).vtable == &g_UiScrollableControlVtable) {
    iVar2 = -control->scrollOffsetX;
    iVar3 = -control->scrollOffsetY;
    cVar1 = '\0';
    iVar5 = control->viewportWidth + iVar2;
    iVar4 = control->viewportHeight + iVar3;
    if ((control->scrollStateFlags &
        (UI_SCROLL_HORIZONTAL_BAR_AT_BOTTOM|UI_SCROLL_HORIZONTAL_BAR_AT_TOP)) != 0) {
      iVar6 = iVar5 - clipLeft;
      cVar1 = iVar5 < clipLeft;
      if ((bool)cVar1) {
        control->scrollOffsetX = control->scrollOffsetX + iVar6;
        iVar2 = iVar2 - iVar6;
      }
      if (iVar2 - clipRight != 0 && clipRight <= iVar2) {
        cVar1 = cVar1 + '\x01';
        control->scrollOffsetX = control->scrollOffsetX + (iVar2 - clipRight);
      }
    }
    if ((control->scrollStateFlags &
        (UI_SCROLL_VERTICAL_BAR_AT_RIGHT|UI_SCROLL_VERTICAL_BAR_AT_LEFT)) != 0) {
      iVar2 = iVar4 - clipTop;
      if (iVar4 < clipTop) {
        control->scrollOffsetY = control->scrollOffsetY + iVar2;
        iVar3 = iVar3 - iVar2;
        cVar1 = cVar1 + '\x01';
      }
      if (iVar3 - clipBottom != 0 && clipBottom <= iVar3) {
        cVar1 = cVar1 + '\x01';
        control->scrollOffsetY = control->scrollOffsetY + (iVar3 - clipBottom);
      }
    }
    if (cVar1 != '\0') {
      UiScrollableControl_RefreshChildAndScrollThumbs(control);
    }
    UiNode_InvalidateRoot(&control->base);
  }
  return;
}

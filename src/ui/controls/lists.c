/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/src/ui/controls/lists.c
 * Reverse engineering by idkFoxes 2026
 */

#include <thandor/ui/controls/lists.h>

/* Implementation ownership: ui/controls/lists. */

/* Address: 0x004BBE60.
   Ownership: ui/controls/lists.
   Purpose: Handles ui timed list control handle keyboard navigation carry-flag result.
   Local calls: UiScrollableControl_QueryContentSizeRegs, UiTimedListTree_CountRecordArrayAndNestedChildren,
   UiScrollableControl_ClampOffsetsToViewport.
   Cross-module calls: UiNode_DefaultKeyboardEventMoveFocusNextCf [ui/controls/input].
*/
bool __thandor_cf_preserve_eax_ecx_edx
UiTimedListControl_HandleKeyboardNavigationCf(dword keyCode,dword stateMask,UiNodeBase *control)

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
  int iVar12;
  int iVar13;
  bool bVar14;
  UiScrollableContentDimensionsEdxEax8 UVar15;
  
  iVar13 = 1;
  iVar7 = control[1].top;
  if ((stateMask & 0xffff0000) == 0) {
UiTimedListKeyboard_DelegateUnhandledEvent:
    bVar14 = UiNode_DefaultKeyboardEventMoveFocusNextCf(keyCode,stateMask,control);
    return bVar14;
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
    UVar15 = UiScrollableControl_QueryContentSizeRegs((UiScrollableControl *)control->parent);
    iVar11 = (int)((UVar15 >> 0x20) / ZEXT48(control[1].vtable));
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
      UVar15 = UiScrollableControl_QueryContentSizeRegs((UiScrollableControl *)control->parent);
      iVar11 = (int)((UVar15 >> 0x20) / ZEXT48(control[1].vtable));
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
            return false;
          }
          if ((*(uint *)(iVar7 + 0xc) & 2) == 0) {
            return false;
          }
          if (control[1].right == 0) {
            return false;
          }
          (*(code *)control[1].right)(iVar7,control);
          return false;
        }
        if (stateMask == 0x10016) {
          if ((*(uint *)(iVar7 + 0xc) & 1) == 0) {
            return false;
          }
          if ((*(uint *)(iVar7 + 0xc) & 2) != 0) {
            return false;
          }
          if (control[1].right == 0) {
            return false;
          }
          (*(code *)control[1].right)(iVar7,control);
          return false;
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
        iVar12 = iVar11 + dVar8;
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
  return false;
}


/* Address: 0x004BB100.
   Ownership: ui/controls/lists.
   Purpose: Binary entry is anchored by g_UiNodeVtable_004BA590[12]@004BA590.
   Local calls: UiScrollableControl_QueryContentSizeRegs, UiScrollableControl_ClampOffsetsToViewport.
   Cross-module calls: UiActionQueue_Enqueue [ui/core/runtime], UiNode_DefaultKeyboardEventMoveFocusNextCf
   [ui/controls/input].
*/
bool __thandor_cf_preserve_eax_ecx_edx
UiListControl_HandleKeyboardNavigationCf
          (UiKeyboardStateMask keyboardStateMask,UiKeyboardEventCode keyCode,UiListControl *control)

{
  void **ppvVar1;
  void **ppvVar2;
  int iVar3;
  uint uVar4;
  bool bVar5;
  UiScrollableContentDimensionsEdxEax8 UVar6;
  
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
      UVar6 = UiScrollableControl_QueryContentSizeRegs
                        ((UiScrollableControl *)(control->base).parent);
      iVar3 = ((uint)((int)control->selectedRowSlot - (int)control->rowSlots) >> 2) -
              ((int)((UVar6 >> 0x20) / (ulonglong)control->rowHeight) + -1);
      if (iVar3 < 0) {
        iVar3 = 0;
      }
      control->selectedRowSlot = control->rowSlots + iVar3;
    }
    else if (keyCode == 0x1001a) {
      UVar6 = UiScrollableControl_QueryContentSizeRegs
                        ((UiScrollableControl *)(control->base).parent);
      uVar4 = ((uint)((int)control->selectedRowSlot - (int)control->rowSlots) >> 2) +
              (int)((UVar6 >> 0x20) / (ulonglong)control->rowHeight) + -1;
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
    return false;
  }
UiListControl_DelegateUnhandledKeyboardEvent:
  bVar5 = UiNode_DefaultKeyboardEventMoveFocusNextCf(keyboardStateMask,keyCode,&control->base);
  return bVar5;
}


/* Address: 0x004BB480.
   Ownership: ui/controls/lists.
   Purpose: Recomputes content height from count and row height, invokes the child layout callback, reselects the
   current pointer-derived index, then queues the action ID at +0x5C.
   Local calls: UiPointerList_GetSelectedIndexVariantBCf, UiPointerList_SelectIndexVariantB.
   Cross-module calls: UiActionQueue_Enqueue [ui/core/runtime].
*/
void __thandor_preserve_eax_edx
UiPointerList_RefreshSelectionAndQueueAction(UiPointerListControl *control)

{
  UiNodeBase *control_00;
  UiListRowIndexEaxCf5 UVar1;
  UiNodeVtable *parentVtable;
  
  control_00 = (control->base).parent;
  parentVtable = control_00->vtable;
  (control->base).bottomOffset = control->rowHeight * control->rowCount + 1;
  (*parentVtable->layout)(control_00);
  UVar1 = UiPointerList_GetSelectedIndexVariantBCf(control);
  UiPointerList_SelectIndexVariantB(UVar1.rowIndex,control);
  UiActionQueue_Enqueue(control->actionId,control);
  return;
}


/* Address: 0x004B87A0.
   Ownership: ui/controls/lists.
   Purpose: Binary entry is anchored by g_UiNodeVtable_004B7920[4]@004B7920.
   Local calls: UiScrollableControl_RefreshChildAndScrollThumbs.
   Cross-module calls: UiNode_InvalidateRoot [ui/core/runtime].
*/
void __thandor_void_preserve_eax_ecx_edx
UiScrollableControl_BeginPrimaryScrollInteraction
          (UiPointerWheelDelta wheelDelta,UiPixelCoordinate pointerY,UiPixelCoordinate pointerX,
          UiScrollableControl *control)

{
  dword dVar1;
  int iVar2;
  int iVar3;
  int iVar4;
  GraphicsTextureSizeEaxEdxCf9 GVar5;
  
  iVar4 = pointerX - (control->base).left;
  iVar3 = pointerY - (control->base).top;
  if ((control->scrollStateFlags &
      (UI_SCROLL_HORIZONTAL_BAR_AT_BOTTOM|UI_SCROLL_HORIZONTAL_BAR_AT_TOP)) == 0) {
UiScrollableControl_TryVerticalScrollbarInteraction:
    if ((control->scrollStateFlags &
        (UI_SCROLL_VERTICAL_BAR_AT_RIGHT|UI_SCROLL_VERTICAL_BAR_AT_LEFT)) == 0) {
      return;
    }
    GVar5 = (*g_GraphicsTextureSourceGetLogicalSize)(0x5e,g_UiWindowTextureSource);
    if ((control->scrollStateFlags & UI_SCROLL_VERTICAL_BAR_AT_RIGHT) == 0) {
      if (iVar4 < 0) {
        return;
      }
      if ((int)GVar5.logicalWidthPixels <= iVar4) {
        return;
      }
    }
    else {
      if ((control->base).layoutWidth <= iVar4) {
        return;
      }
      if (iVar4 < (int)((control->base).layoutWidth - GVar5.logicalWidthPixels)) {
        return;
      }
    }
    iVar4 = (control->base).layoutHeight;
    GVar5 = (*g_GraphicsTextureSourceGetLogicalSize)(0x5a,g_UiWindowTextureSource);
    if ((control->scrollStateFlags & UI_SCROLL_HORIZONTAL_BAR_AT_BOTTOM) != 0) {
      iVar4 = iVar4 - GVar5.logicalHeightPixels;
    }
    dVar1 = 0;
    if ((control->scrollStateFlags & UI_SCROLL_HORIZONTAL_BAR_AT_TOP) != 0) {
      dVar1 = GVar5.logicalHeightPixels;
    }
    if (iVar3 < (int)dVar1) {
      return;
    }
    if (iVar4 <= iVar3) {
      return;
    }
    if (iVar3 < control->verticalThumbTop) {
      GVar5 = (*g_GraphicsTextureSourceGetLogicalSize)(0x5e,g_UiWindowTextureSource);
      if ((int)(iVar3 - dVar1) < (int)GVar5.logicalHeightPixels) {
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
      if (iVar3 < control->verticalThumbBottom) {
        control->pointerAnchorY = iVar3 - control->verticalThumbTop;
        control->scrollStateFlags = control->scrollStateFlags | UI_SCROLL_VERTICAL_THUMB_ACTIVE;
        goto UiScrollableControl_InvalidateAfterPrimaryScrollInteraction;
      }
      GVar5 = (*g_GraphicsTextureSourceGetLogicalSize)(0x5e,g_UiWindowTextureSource);
      if (iVar4 - iVar3 <= (int)GVar5.logicalHeightPixels) {
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
    GVar5 = (*g_GraphicsTextureSourceGetLogicalSize)(0x5a,g_UiWindowTextureSource);
    if ((control->scrollStateFlags & UI_SCROLL_HORIZONTAL_BAR_AT_BOTTOM) == 0) {
      if ((-1 < iVar3) && (iVar3 < (int)GVar5.logicalHeightPixels))
      goto UiScrollableControl_BeginHorizontalScrollbarInteraction;
      goto UiScrollableControl_TryVerticalScrollbarInteraction;
    }
    if (((control->base).layoutHeight <= iVar3) ||
       (iVar3 < (int)((control->base).layoutHeight - GVar5.logicalHeightPixels)))
    goto UiScrollableControl_TryVerticalScrollbarInteraction;
UiScrollableControl_BeginHorizontalScrollbarInteraction:
    iVar2 = (control->base).layoutWidth;
    GVar5 = (*g_GraphicsTextureSourceGetLogicalSize)(0x5e,g_UiWindowTextureSource);
    if ((control->scrollStateFlags & UI_SCROLL_VERTICAL_BAR_AT_RIGHT) != 0) {
      iVar2 = iVar2 - GVar5.logicalWidthPixels;
    }
    dVar1 = 0;
    if ((control->scrollStateFlags & UI_SCROLL_VERTICAL_BAR_AT_LEFT) != 0) {
      dVar1 = GVar5.logicalWidthPixels;
    }
    if ((iVar4 < (int)dVar1) || (iVar2 <= iVar4))
    goto UiScrollableControl_TryVerticalScrollbarInteraction;
    if (iVar4 < control->horizontalThumbLeft) {
      GVar5 = (*g_GraphicsTextureSourceGetLogicalSize)(0x5a,g_UiWindowTextureSource);
      if ((int)(iVar4 - dVar1) < (int)GVar5.logicalWidthPixels) {
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
      if (iVar4 < control->horizontalThumbRight) {
        control->pointerAnchorX = iVar4 - control->horizontalThumbLeft;
        control->scrollStateFlags = control->scrollStateFlags | UI_SCROLL_HORIZONTAL_THUMB_ACTIVE;
        goto UiScrollableControl_InvalidateAfterPrimaryScrollInteraction;
      }
      GVar5 = (*g_GraphicsTextureSourceGetLogicalSize)(0x5a,g_UiWindowTextureSource);
      if (iVar2 - iVar4 <= (int)GVar5.logicalWidthPixels) {
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
void __thandor_preserve_eax
UiScrollableControl_EndPrimaryScrollInteraction
          (UiPointerWheelDelta wheelDelta,UiPixelCoordinate pointerY,UiPixelCoordinate pointerX,
          UiNodeBase *control)

{
  if (((uint)control[1].nextSibling & 0x20000) != 0) {
    control[1].bottom = control[1].bottom + ((int)control[1].firstChild >> 1);
    UiScrollableControl_RefreshChildAndScrollThumbs((UiScrollableControl *)control);
  }
  if (((uint)control[1].nextSibling & 0x80000) != 0) {
    control[1].bottom = control[1].bottom - ((int)control[1].firstChild >> 1);
    UiScrollableControl_RefreshChildAndScrollThumbs((UiScrollableControl *)control);
  }
  if (((uint)control[1].nextSibling & 0x2000000) != 0) {
    control[1].leftOffset = control[1].leftOffset + ((int)control[1].parent >> 1);
    UiScrollableControl_RefreshChildAndScrollThumbs((UiScrollableControl *)control);
  }
  if (((uint)control[1].nextSibling & 0x8000000) != 0) {
    control[1].leftOffset = control[1].leftOffset - ((int)control[1].parent >> 1);
    UiScrollableControl_RefreshChildAndScrollThumbs((UiScrollableControl *)control);
  }
  control[1].nextSibling = (UiNodeBase *)((uint)control[1].nextSibling & 0xe0e0dfff);
  UiNode_InvalidateRoot(control);
  return;
}


/* Address: 0x004B8BF0.
   Ownership: ui/controls/lists.
   Purpose: Binary entry is anchored by g_UiNodeVtable_004B7920[8]@004B7920.
   Local calls: UiScrollableControl_RefreshChildAndScrollThumbs.
   Cross-module calls: UiNode_InvalidateRoot [ui/core/runtime].
*/
void __thandor_void_preserve_eax_ecx_edx
UiScrollableControl_UpdatePrimaryScrollDrag
          (UiPointerWheelDelta wheelDelta,UiPixelCoordinate pointerY,UiPixelCoordinate pointerX,
          UiScrollableControl *control)

{
  int iVar1;
  int iVar2;
  dword dVar3;
  int iVar4;
  dword dVar5;
  GraphicsTextureSizeEaxEdxCf9 GVar6;
  
  if ((control->scrollStateFlags &
      (UI_SCROLL_VERTICAL_TRACK_AFTER_THUMB_ACTIVE|UI_SCROLL_VERTICAL_TRACK_BEFORE_THUMB_ACTIVE|
       UI_SCROLL_HORIZONTAL_TRACK_AFTER_THUMB_ACTIVE|UI_SCROLL_HORIZONTAL_TRACK_BEFORE_THUMB_ACTIVE)
      ) != 0) {
    return;
  }
  if ((control->scrollStateFlags & UI_SCROLL_HORIZONTAL_THUMB_ACTIVE) != 0) {
    iVar1 = (control->base).left;
    iVar2 = control->pointerAnchorX;
    iVar4 = (control->base).layoutWidth;
    GVar6 = (*g_GraphicsTextureSourceGetLogicalSize)(0x5a,g_UiWindowTextureSource);
    iVar1 = ((pointerX - iVar1) - iVar2) - GVar6.logicalWidthPixels;
    iVar4 = iVar4 + GVar6.logicalWidthPixels * -2;
    if ((control->scrollStateFlags &
        (UI_SCROLL_VERTICAL_BAR_AT_RIGHT|UI_SCROLL_VERTICAL_BAR_AT_LEFT)) != 0) {
      GVar6 = (*g_GraphicsTextureSourceGetLogicalSize)(0x5e,g_UiWindowTextureSource);
      if ((control->scrollStateFlags & UI_SCROLL_VERTICAL_BAR_AT_LEFT) != 0) {
        iVar1 = iVar1 - GVar6.logicalWidthPixels;
      }
      iVar4 = iVar4 - GVar6.logicalWidthPixels;
    }
    control->scrollOffsetX =
         (UiPixelOffset)
         (((longlong)(int)(control->contentWidth - control->viewportWidth) * (longlong)iVar1) /
         (longlong)-((iVar4 - control->horizontalThumbRight) + control->horizontalThumbLeft));
    goto UiScrollableControl_RefreshAfterPrimaryDragUpdate;
  }
  if ((control->scrollStateFlags & UI_SCROLL_VERTICAL_THUMB_ACTIVE) != 0) {
    iVar1 = (control->base).top;
    iVar2 = control->pointerAnchorY;
    iVar4 = (control->base).layoutHeight;
    GVar6 = (*g_GraphicsTextureSourceGetLogicalSize)(0x5e,g_UiWindowTextureSource);
    iVar1 = ((pointerY - iVar1) - iVar2) - GVar6.logicalHeightPixels;
    iVar4 = iVar4 + GVar6.logicalHeightPixels * -2;
    if ((control->scrollStateFlags &
        (UI_SCROLL_HORIZONTAL_BAR_AT_BOTTOM|UI_SCROLL_HORIZONTAL_BAR_AT_TOP)) != 0) {
      GVar6 = (*g_GraphicsTextureSourceGetLogicalSize)(0x5a,g_UiWindowTextureSource);
      if ((control->scrollStateFlags & UI_SCROLL_HORIZONTAL_BAR_AT_TOP) != 0) {
        iVar1 = iVar1 - GVar6.logicalHeightPixels;
      }
      iVar4 = iVar4 - GVar6.logicalHeightPixels;
    }
    control->scrollOffsetY =
         (UiPixelOffset)
         (((longlong)(int)(control->contentHeight - control->viewportHeight) * (longlong)iVar1) /
         (longlong)-((iVar4 - control->verticalThumbBottom) + control->verticalThumbTop));
    goto UiScrollableControl_RefreshAfterPrimaryDragUpdate;
  }
  if ((control->scrollStateFlags & UI_SCROLL_HORIZONTAL_DECREMENT_ACTIVE) == 0) {
    if ((control->scrollStateFlags & UI_SCROLL_HORIZONTAL_INCREMENT_ACTIVE) == 0) {
      if ((control->scrollStateFlags & UI_SCROLL_VERTICAL_DECREMENT_ACTIVE) == 0) {
        if ((control->scrollStateFlags & UI_SCROLL_VERTICAL_INCREMENT_ACTIVE) == 0) {
          return;
        }
        iVar2 = pointerY - (control->base).top;
        iVar1 = pointerX - (control->base).left;
        GVar6 = (*g_GraphicsTextureSourceGetLogicalSize)(0x5e,g_UiWindowTextureSource);
        dVar3 = GVar6.logicalHeightPixels;
        if ((control->scrollStateFlags & UI_SCROLL_VERTICAL_BAR_AT_LEFT) == 0) {
          if ((iVar1 < (control->base).layoutWidth) &&
             ((int)((control->base).layoutWidth - GVar6.logicalWidthPixels) <= iVar1))
          goto UiScrollableControl_ValidateBottomArrowHover;
        }
        else if ((-1 < iVar1) && (iVar1 < (int)GVar6.logicalWidthPixels)) {
UiScrollableControl_ValidateBottomArrowHover:
          iVar1 = (control->base).layoutHeight;
          if ((control->scrollStateFlags & UI_SCROLL_HORIZONTAL_BAR_AT_BOTTOM) != 0) {
            GVar6 = (*g_GraphicsTextureSourceGetLogicalSize)(0x5a,g_UiWindowTextureSource);
            iVar1 = iVar1 - GVar6.logicalHeightPixels;
          }
          if ((iVar2 < iVar1) && ((int)(iVar1 - dVar3) <= iVar2))
          goto UiScrollableControl_SetArrowHoverActive;
        }
      }
      else {
        iVar2 = pointerY - (control->base).top;
        iVar1 = pointerX - (control->base).left;
        GVar6 = (*g_GraphicsTextureSourceGetLogicalSize)(0x5e,g_UiWindowTextureSource);
        dVar3 = GVar6.logicalHeightPixels;
        if ((control->scrollStateFlags & UI_SCROLL_VERTICAL_BAR_AT_LEFT) == 0) {
          if ((iVar1 < (control->base).layoutWidth) &&
             ((int)((control->base).layoutWidth - GVar6.logicalWidthPixels) <= iVar1))
          goto UiScrollableControl_ValidateTopArrowHover;
        }
        else if ((-1 < iVar1) && (iVar1 < (int)GVar6.logicalWidthPixels)) {
UiScrollableControl_ValidateTopArrowHover:
          dVar5 = 0;
          if ((control->scrollStateFlags & UI_SCROLL_HORIZONTAL_BAR_AT_TOP) != 0) {
            GVar6 = (*g_GraphicsTextureSourceGetLogicalSize)(0x5a,g_UiWindowTextureSource);
            dVar5 = GVar6.logicalHeightPixels;
          }
          if (((int)dVar5 <= iVar2) && (iVar2 < (int)(dVar5 + dVar3)))
          goto UiScrollableControl_SetArrowHoverActive;
        }
      }
    }
    else {
      iVar2 = pointerY - (control->base).top;
      iVar1 = pointerX - (control->base).left;
      GVar6 = (*g_GraphicsTextureSourceGetLogicalSize)(0x5a,g_UiWindowTextureSource);
      dVar3 = GVar6.logicalWidthPixels;
      if ((control->scrollStateFlags & UI_SCROLL_HORIZONTAL_BAR_AT_TOP) == 0) {
        if ((iVar2 < (control->base).layoutHeight) &&
           ((int)((control->base).layoutHeight - GVar6.logicalHeightPixels) <= iVar2))
        goto UiScrollableControl_ValidateRightArrowHover;
      }
      else if ((-1 < iVar2) && (iVar2 < (int)GVar6.logicalHeightPixels)) {
UiScrollableControl_ValidateRightArrowHover:
        iVar2 = (control->base).layoutWidth;
        if ((control->scrollStateFlags & UI_SCROLL_VERTICAL_BAR_AT_RIGHT) != 0) {
          GVar6 = (*g_GraphicsTextureSourceGetLogicalSize)(0x5e,g_UiWindowTextureSource);
          iVar2 = iVar2 - GVar6.logicalWidthPixels;
        }
        if ((iVar1 < iVar2) && ((int)(iVar2 - dVar3) <= iVar1))
        goto UiScrollableControl_SetArrowHoverActive;
      }
    }
  }
  else {
    iVar2 = pointerY - (control->base).top;
    iVar1 = pointerX - (control->base).left;
    GVar6 = (*g_GraphicsTextureSourceGetLogicalSize)(0x5a,g_UiWindowTextureSource);
    dVar3 = GVar6.logicalWidthPixels;
    if ((control->scrollStateFlags & UI_SCROLL_HORIZONTAL_BAR_AT_TOP) == 0) {
      if ((iVar2 < (control->base).layoutHeight) &&
         ((int)((control->base).layoutHeight - GVar6.logicalHeightPixels) <= iVar2))
      goto UiScrollableControl_ValidateLeftArrowHover;
    }
    else if ((-1 < iVar2) && (iVar2 < (int)GVar6.logicalHeightPixels)) {
UiScrollableControl_ValidateLeftArrowHover:
      dVar5 = 0;
      if ((control->scrollStateFlags & UI_SCROLL_VERTICAL_BAR_AT_LEFT) != 0) {
        GVar6 = (*g_GraphicsTextureSourceGetLogicalSize)(0x5e,g_UiWindowTextureSource);
        dVar5 = GVar6.logicalWidthPixels;
      }
      if (((int)dVar5 <= iVar1) && (iVar1 < (int)(dVar5 + dVar3))) {
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
void __thandor_void_preserve_eax_ecx_edx
UiScrollableControl_UpdateSecondaryScrollDrag
          (UiPointerWheelDelta wheelDelta,UiPixelCoordinate pointerY,UiPixelCoordinate pointerX,
          UiScrollableControl *control)

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
void __thandor_void_preserve_ecx_edx
UiScrollableControl_TickAutoScroll(UiScrollableControl *control)

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
void __thandor_void_preserve_eax_ecx_edx
UiScrollableControl_HandlePointerWheel
          (UiPointerWheelDelta wheelDelta,UiPixelCoordinate pointerY,UiPixelCoordinate pointerX,
          UiScrollableControl *control)

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
void __thandor_void_preserve_eax_ecx_edx
UiPointerList_SelectIndexVariantA(UiListRowIndex index,UiPointerListControl *control)

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
void __thandor_void_preserve_eax_ecx_edx
UiListControl_SelectRowFromPointer
          (UiPointerWheelDelta wheelDelta,UiPixelCoordinate pointerY,UiPixelCoordinate pointerX,
          UiListControl *control)

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
void __thandor_void_preserve_eax_ecx_edx
UiPointerList_SortByDwordPairFieldDescending
          (UiPointerListFieldByteOffset fieldOffset,UiPointerListControl *control)

{
  void *pvVar1;
  void *pvVar2;
  uint uVar3;
  uint uVar4;
  int iVar5;
  int iVar6;
  UiListRowCount UVar7;
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
  return;
}


/* Address: 0x004BB8A0.
   Ownership: ui/controls/lists.
   Purpose: Bubble-sorts in ascending unsigned order by one dword at fieldOffset, then restores selection and
   invalidates its row. Typed parameters: p0 fieldOffset→UiPointerListFieldByteOffset_V342. Calling convention,
   exact VariableStorage serialization, function body bytes, control flow, globals, locals, and executable data
   remain unchanged.
   Local calls: UiScrollableControl_ClampOffsetsToViewport.
*/
void __thandor_void_preserve_eax_ecx_edx
UiPointerList_SortByDwordFieldAscending
          (UiPointerListFieldByteOffset fieldOffset,UiPointerListControl *control)

{
  void *pvVar1;
  void *pvVar2;
  int iVar3;
  int iVar4;
  UiListRowCount UVar5;
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
  return;
}


/* Address: 0x004BBCD0.
   Ownership: ui/controls/lists.
   Purpose: Handles ui timed list control select row from pointer.
   Local calls: UiTimedListControl_SelectRecordAndScrollIntoView.
   Cross-module calls: RichTextCommandStream_MeasureRegs [assets/text/richtext], UiActionQueue_Enqueue
   [ui/core/runtime].
*/
void __thandor_void_preserve_eax_ecx_edx
UiTimedListControl_SelectRowFromPointer
          (int pointerButton,int pointerY,int pointerX,UiNodeBase *control)

{
  UiNodeBase *pUVar1;
  code *pcVar2;
  UiAnchorFractionQ31 UVar3;
  UiNodeVtable *pUVar4;
  int iVar5;
  int iVar6;
  int iVar7;
  int iVar8;
  sdword *unaff_EBP;
  sdword *psVar9;
  sdword *psVar10;
  UiNodeBase *pUVar11;
  bool bVar12;
  RichTextExtentRegs RVar13;
  UiPackedTextStyle aUStack_3c [5];
  UiNodeBase *pUStack_28;
  sdword *psStack_24;
  UiNodeBase *pUStack_20;
  
  pUStack_20 = (UiNodeBase *)0x0;
  pUVar1 = control[1].firstChild;
  if (pUVar1 != (UiNodeBase *)0x0) {
    pUVar11 = pUVar1->nextSibling;
    iVar6 = pointerX - control->left;
    if (((control->left <= pointerX) && (control->top <= pointerY)) &&
       (pUVar11 != (UiNodeBase *)0x0)) {
      iVar5 = pointerY - control->top;
      psVar9 = &pUVar1->left;
      while (pUVar1 = pUStack_20, iVar8 = iVar5 - (int)control[1].vtable,
            iVar8 != 0 && (int)control[1].vtable <= iVar5) {
        psVar10 = psVar9 + 4;
        pUVar11 = (UiNodeBase *)((int)&pUVar11[-1].nodeFlags + 3);
        if ((((psVar9[3] & 1U) != 0) && ((psVar9[3] & 2U) != 0)) && (psVar9[2] != 0)) {
          pUStack_20 = (UiNodeBase *)((int)&pUStack_20->nextSibling + 1);
          psStack_24 = psVar10;
          pUStack_28 = pUVar11;
          psVar10 = (int *)psVar9[2] + 4;
          pUVar11 = *(UiNodeBase **)psVar9[2];
        }
        while (iVar5 = iVar8, psVar9 = psVar10, pUVar11 == (UiNodeBase *)0x0) {
          if (pUStack_20 == (UiNodeBase *)0x0) {
            return;
          }
          pUStack_20 = (UiNodeBase *)((int)&pUStack_20[-1].nodeFlags + 3);
          psVar10 = unaff_EBP;
          pUVar11 = pUStack_20;
        }
      }
      iVar5 = (int)pUStack_20 * control[1].topAnchorQ31;
      iVar7 = iVar6 - iVar5;
      if (iVar6 < iVar5) {
        if (((psVar9[3] & 1U) != 0) && (iVar5 != 0)) {
          UVar3 = control[1].topAnchorQ31;
          pUVar4 = control[1].vtable;
          (&psStack_24)[(int)pUStack_20 * 2] = (sdword *)control[1].bottom;
          aUStack_3c[(int)pUVar1 * 2 + 5] = control[1].leftOffset;
          aUStack_3c[(int)pUVar1 * 2 + 4] = 0;
          aUStack_3c[(int)pUVar1 * 2 + 3] = 0;
          aUStack_3c[(int)pUVar1 * 2 + 2] = iVar7 + UVar3;
          aUStack_3c[(int)pUVar1 * 2 + 1] = iVar8 + (int)pUVar4;
          aUStack_3c[(int)pUVar1 * 2] = 0x4bbe3a;
          bVar12 = (*g_GraphicsTextureSourceTestOpaquePixel)
                             (aUStack_3c[(int)pUVar1 * 2 + 1],aUStack_3c[(int)pUVar1 * 2 + 2],
                              aUStack_3c[(int)pUVar1 * 2 + 3],aUStack_3c[(int)pUVar1 * 2 + 4],
                              aUStack_3c[(int)pUVar1 * 2 + 5],
                              (GraphicsTextureSourceAsset *)(&psStack_24)[(int)pUVar1 * 2]);
          if ((bVar12) && (control[1].right != 0)) {
            (&psStack_24)[(int)pUVar1 * 2] = (sdword *)control;
            aUStack_3c[(int)pUVar1 * 2 + 5] = (UiPackedTextStyle)psVar9;
            pcVar2 = (code *)control[1].right;
            aUStack_3c[(int)pUVar1 * 2 + 4] = 0x4bbe4d;
            (*pcVar2)();
          }
        }
      }
      else {
        iVar6 = iVar7 - control[1].rightAnchorQ31;
        if (iVar6 != 0 && (int)control[1].rightAnchorQ31 <= iVar7) {
          (&psStack_24)[(int)pUStack_20 * 2] = (sdword *)*psVar9;
          aUStack_3c[(int)pUVar1 * 2 + 5] = g_UiListTextStyle;
          aUStack_3c[(int)pUVar1 * 2 + 4] = 0x4bbdb4;
          RVar13 = RichTextCommandStream_MeasureRegs
                             (aUStack_3c[(int)pUVar1 * 2 + 5],(word *)(&psStack_24)[(int)pUVar1 * 2]
                             );
          if (((int)RVar13.widthPixels < iVar6) && (6 < (int)(iVar6 - RVar13.widthPixels))) {
            return;
          }
        }
        if (psVar9 != (int *)control[1].top) {
          (&psStack_24)[(int)pUVar1 * 2] = (sdword *)control;
          aUStack_3c[(int)pUVar1 * 2 + 5] = (UiPackedTextStyle)psVar9;
          aUStack_3c[(int)pUVar1 * 2 + 4] = 0x4bbdd0;
          UiTimedListControl_SelectRecordAndScrollIntoView
                    ((UiTimedListTreeRecord16 *)aUStack_3c[(int)pUVar1 * 2 + 5],
                     (UiTimedListRuntimeExtendedView88 *)(&psStack_24)[(int)pUVar1 * 2]);
          (&psStack_24)[(int)pUVar1 * 2] = (sdword *)control;
          aUStack_3c[(int)pUVar1 * 2 + 5] = control[1].left;
          aUStack_3c[(int)pUVar1 * 2 + 4] = 0x4bbddc;
          UiActionQueue_Enqueue(aUStack_3c[(int)pUVar1 * 2 + 5],(&psStack_24)[(int)pUVar1 * 2]);
        }
        if ((((control->nodeFlags & UI_NODE_REPEAT_OR_DOUBLE_CLICK) != 0) && ((psVar9[3] & 1U) != 0)
            ) && (control[1].right != 0)) {
          (&psStack_24)[(int)pUVar1 * 2] = (sdword *)control;
          aUStack_3c[(int)pUVar1 * 2 + 5] = (UiPackedTextStyle)psVar9;
          pcVar2 = (code *)control[1].right;
          aUStack_3c[(int)pUVar1 * 2 + 4] = 0x4bbe07;
          (*pcVar2)();
        }
      }
    }
  }
  return;
}


/* Address: 0x0040FF70.
   Ownership: ui/controls/lists.
   Purpose: Finds a timed-list tree record by label.
*/
UiTimedListTreeRecord16 * __thandor_eax_preserve_ecx_edx
UiTimedListTree_FindRecordByLabel(word *labelUtf16,UiTimedListTreeRecord16 *recordBlock)

{
  word wVar1;
  int iVar2;
  int iVar3;
  dword dVar4;
  word *pwVar5;
  word *pwVar6;
  bool bVar7;
  
  iVar2 = 0x100;
  pwVar5 = labelUtf16;
  do {
    if (iVar2 == 0) break;
    iVar2 = iVar2 + -1;
    wVar1 = *pwVar5;
    pwVar5 = pwVar5 + 1;
  } while (wVar1 != 0);
  dVar4 = recordBlock->recordCountOrRowPayload00;
  do {
    if (dVar4 == 0) {
      return (UiTimedListTreeRecord16 *)0x0;
    }
    recordBlock = recordBlock + 1;
    bVar7 = false;
    iVar3 = 0x100 - iVar2;
    pwVar5 = (word *)recordBlock->recordCountOrRowPayload00;
    pwVar6 = labelUtf16;
    do {
      if (iVar3 == 0) break;
      iVar3 = iVar3 + -1;
      bVar7 = *pwVar5 == *pwVar6;
      pwVar5 = pwVar5 + 1;
      pwVar6 = pwVar6 + 1;
    } while (bVar7);
    if (bVar7) {
      return recordBlock;
    }
    dVar4 = dVar4 - 1;
  } while( true );
}

/* Address: 0x0040FFE0.
   Ownership: ui/controls/lists.
   Purpose: Builds a timed-list directory record block and exposes the recovered carry/error contract.
*/
Recovered0040FFE0EaxCf5 __thandor_eax_cf_preserve_ecx_edx
UiTimedListTree_BuildDirectoryRecordBlockCf(word *pathUtf16)

{
  dword *pdVar1;
  dword dVar2;
  dword *pdVar3;
  dword *pdVar4;
  dword *outputRecords;
  uint driveLetter;
  EngineDriveTypeCode EVar5;
  dword *pdVar6;
  int iVar7;
  dword *pdVar8;
  uint uVar9;
  dword dVar10;
  byte *pbVar11;
  dword *leaf;
  dword *pdVar12;
  uint *puVar13;
  uint *puVar14;
  uint *puVar15;
  bool bVar16;
  ArenaShrinkEaxCf5 AVar17;
  Recovered0040FFE0EaxCf5 RVar18;
  Recovered0040FFE0EaxCf5 RVar19;
  ArenaAllocEaxCf5 AVar20;
  ArenaLargestAllocationEaxEcxCf9 AVar21;
  FileSystemEnumerationEaxEcxCf9 FVar22;
  DriveLetterEnumerationEaxEcx8 DVar23;
  
  if (*pathUtf16 == 0) {
    AVar20 = (*g_MemoryApi.alloc)(0x220);
    outputRecords = (dword *)AVar20.eax;
    if (!AVar20.carry) {
      *outputRecords = 1;
      outputRecords[1] = 0;
      outputRecords[2] = 0;
      outputRecords[3] = 0x80000000;
      outputRecords[4] = (dword)(outputRecords + 8);
      outputRecords[5] = 0x27;
      outputRecords[6] = 0;
      outputRecords[7] = 1;
      (*g_LocaleCopyDefaultComputerLabelUtf16)((word *)(outputRecords + 8));
      return (Recovered0040FFE0EaxCf5)((uint5)AVar20 & 0xffffffff);
    }
  }
  else if ((pathUtf16[3] == 0) || (pathUtf16[2] == 0)) {
    DVar23 = (*g_FileSystemEnumerateDriveLetters)((byte *)0x40f530);
    dVar10 = DVar23.driveCount;
    AVar20 = (*g_MemoryApi.alloc)(DVar23.driveCountMirror * 0x210 + 0x10);
    outputRecords = (dword *)AVar20.eax;
    if (!AVar20.carry) {
      RVar18.recordBlockOrError = outputRecords + 4;
      *outputRecords = dVar10;
      outputRecords[1] = 0;
      outputRecords[2] = 0;
      outputRecords[3] = 0x80000000;
      puVar13 = RVar18.recordBlockOrError + dVar10 * 4;
      pbVar11 = (byte *)0x40f530;
      do {
        driveLetter = (uint)*pbVar11;
        *RVar18.recordBlockOrError = (dword)puVar13;
        uVar9 = driveLetter;
        EVar5 = (*g_FileSystemGetDriveTypeCode)(driveLetter);
        RVar18.recordBlockOrError[1] = EVar5;
        RVar18.recordBlockOrError[2] = 0;
        RVar18.recordBlockOrError[3] = 0;
        u________0040ff58[0] = (wchar_t)driveLetter;
        *puVar13 = driveLetter;
        ((word *)((int)puVar13 + 2))[0] = 0x3a;
        ((word *)((int)puVar13 + 2))[1] = 0x5b;
        ((word *)((int)puVar13 + 6))[0] = 0x5d;
        ((word *)((int)puVar13 + 6))[1] = 0;
        bVar16 = (*g_FileSystemCheckDriveMediaReady)(uVar9);
        if (bVar16) {
LAB_004102e2:
          ((word *)((int)puVar13 + 6))[0] = 0x5d;
          ((word *)((int)puVar13 + 6))[1] = 0;
        }
        else {
          FVar22 = (*g_FileSystemEnumerateDirectoryOrVolumeEntriesCf)
                             (FILESYSTEM_ENUMERATE_VOLUME_LABEL,0xffffffff,0x1f8,
                              (byte *)((int)puVar13 + 6),(byte *)u________0040ff58);
          if (FVar22.carry) goto LAB_004102e2;
          if (FVar22.entryCount == 0) {
            ((word *)((int)puVar13 + 6))[0] = 0x5d;
            ((word *)((int)puVar13 + 6))[1] = 0;
          }
          else {
            iVar7 = 0x100;
            puVar14 = puVar13;
            do {
              puVar15 = puVar14;
              if (iVar7 == 0) break;
              iVar7 = iVar7 + -1;
              puVar15 = (uint *)((int)puVar14 + 2);
              uVar9 = *puVar14;
              puVar14 = puVar15;
            } while ((word)uVar9 != 0);
            ((word *)((int)puVar15 + -2))[0] = 0x5d;
            ((word *)((int)puVar15 + -2))[1] = 0;
          }
          FVar22 = (*g_FileSystemEnumerateDirectoryOrVolumeEntriesCf)
                             (FILESYSTEM_ENUMERATE_DIRECTORIES,0xffffffff,0x200,
                              (byte *)g_UiTimedListRecordPathScratch.codeUnits,
                              (byte *)u________0040ff58);
          if (FVar22.carry) goto LAB_004102e2;
          if (FVar22.entryCount != 0) {
            RVar18.recordBlockOrError[3] = RVar18.recordBlockOrError[3] | 1;
          }
        }
        puVar13 = puVar13 + 0x80;
        RVar18.recordBlockOrError = RVar18.recordBlockOrError + 4;
        pbVar11 = pbVar11 + 1;
        dVar10 = dVar10 - 1;
        if (dVar10 == 0) {
          return (Recovered0040FFE0EaxCf5)((uint5)AVar20 & 0xffffffff);
        }
      } while( true );
    }
  }
  else {
    WidePath_SplitParentAndLeaf
              (g_UiTimedListRecordPathScratch.codeUnits,g_UiTimedListCombinedPathScratch.codeUnits,
               pathUtf16);
    WidePath_CombineDirectoryAndLeaf
              (g_UiTimedListRecordPathScratch.codeUnits,(word *)0x40ff50,
               g_UiTimedListCombinedPathScratch.codeUnits);
    AVar21 = (*g_MemoryApi.allocLargestFreeBlock)();
    outputRecords = (dword *)AVar21.allocationOrError;
    if (!AVar21.carry) {
      FVar22 = (*g_FileSystemEnumerateDirectoryOrVolumeEntriesCf)
                         (FILESYSTEM_ENUMERATE_DIRECTORIES,0xffffffff,AVar21.blockSizeOrSentinel,
                          (byte *)outputRecords,(byte *)g_UiTimedListRecordPathScratch.codeUnits);
      dVar10 = FVar22.entryCount;
      pdVar3 = (dword *)FVar22.recordSizeBytes;
      RVar18.recordBlockOrError = pdVar3;
      if (!FVar22.carry) {
        AVar17 = (*g_MemoryApi.shrinkInPlace)((int)pdVar3 * dVar10,outputRecords);
        RVar18.recordBlockOrError = (dword *)AVar17.scratchOrError;
        if (!AVar17.carry) {
          AVar21 = (*g_MemoryApi.allocLargestFreeBlock)();
          RVar18.recordBlockOrError = (dword *)AVar21.allocationOrError;
          if (!AVar21.carry) {
            iVar7 = dVar10 + 1;
            pdVar4 = (dword *)0x14;
            pdVar6 = (dword *)(AVar21.blockSizeOrSentinel + iVar7 * -0x10);
            if ((uint)(iVar7 * 0x10) <= AVar21.blockSizeOrSentinel && pdVar6 != (dword *)0x0) {
              *RVar18.recordBlockOrError = dVar10;
              RVar18.recordBlockOrError[1] = 0;
              RVar18.recordBlockOrError[2] = 0;
              RVar18.recordBlockOrError[3] = 0x80000000;
              pdVar12 = RVar18.recordBlockOrError + iVar7 * 4;
              leaf = outputRecords;
              pdVar1 = RVar18.recordBlockOrError;
              for (; dVar10 != 0; dVar10 = dVar10 - 1) {
                pdVar1[4] = (dword)pdVar12;
                pdVar1[5] = 0x26;
                pdVar1[6] = 0;
                pdVar1[7] = 0;
                WidePath_CombineDirectoryAndLeaf
                          (g_UiTimedListRecordPathScratch.codeUnits,(word *)leaf,
                           g_UiTimedListCombinedPathScratch.codeUnits);
                WidePath_CombineDirectoryAndLeaf
                          (g_UiTimedListSecondaryPathScratch.codeUnits,(word *)0x40ff50,
                           g_UiTimedListRecordPathScratch.codeUnits);
                FVar22 = (*g_FileSystemEnumerateDirectoryOrVolumeEntriesCf)
                                   (FILESYSTEM_ENUMERATE_DIRECTORIES,0xffffffff,0x200,
                                    (byte *)g_UiTimedListRecordPathScratch.codeUnits,
                                    (byte *)g_UiTimedListSecondaryPathScratch.codeUnits);
                if ((!FVar22.carry) && (FVar22.entryCount != 0)) {
                  pdVar1[7] = pdVar1[7] | 1;
                }
                iVar7 = 0x100;
                pdVar4 = leaf;
                do {
                  if (iVar7 == 0) break;
                  iVar7 = iVar7 + -1;
                  dVar2 = *pdVar4;
                  pdVar4 = (dword *)((int)pdVar4 + 2);
                } while ((word)dVar2 != 0);
                pdVar4 = (dword *)(0x102U - iVar7 & 0xfffffffe);
                pdVar8 = (dword *)((int)pdVar6 - (int)pdVar4);
                if ((pdVar6 < pdVar4 || pdVar8 == (dword *)0x0) ||
                   (pdVar6 = (dword *)((int)pdVar8 - (int)pdVar4),
                   pdVar8 < pdVar4 || pdVar6 == (dword *)0x0)) goto LAB_004101a4;
                pdVar4 = leaf;
                for (uVar9 = 0x102U - iVar7 >> 1; uVar9 != 0; uVar9 = uVar9 - 1) {
                  *pdVar12 = *pdVar4;
                  pdVar4 = pdVar4 + 1;
                  pdVar12 = pdVar12 + 1;
                }
                leaf = (dword *)((int)leaf + (int)pdVar3);
                pdVar1 = pdVar1 + 4;
              }
              AVar17 = (*g_MemoryApi.shrinkInPlace)
                                 ((int)pdVar12 + (0x200 - (int)RVar18.recordBlockOrError),
                                  RVar18.recordBlockOrError);
              pdVar4 = (dword *)AVar17.scratchOrError;
              if (!AVar17.carry) {
                (*g_MemoryApi.free)(outputRecords);
                RVar18.carry = false;
                return RVar18;
              }
            }
LAB_004101a4:
            (*g_MemoryApi.free)(RVar18.recordBlockOrError);
            RVar18.recordBlockOrError = pdVar4;
          }
        }
      }
      (*g_MemoryApi.free)(outputRecords);
      outputRecords = RVar18.recordBlockOrError;
    }
  }
  RVar19.carry = true;
  RVar19.recordBlockOrError = outputRecords;
  return RVar19;
}

/* Address: 0x00410380.
   Ownership: ui/controls/lists.
   Purpose: Builds the timed-list directory hierarchy.
*/
UiTimedListDirectoryHierarchyEaxEdxCf9 __thandor_eax_edx_cf_preserve_ecx
UiTimedListTree_BuildDirectoryHierarchyCf(word *selectedPathUtf16)

{
  ushort uVar1;
  undefined2 uVar2;
  UiTimedListTreeRecord16 *pUVar3;
  int iVar4;
  dword dVar5;
  int iVar6;
  UiTimedListTreeRecord16 *unaff_EBP;
  UiTimedListTreeRecord16 *pUVar7;
  WidePathBuffer256 *pWVar8;
  UiTimedListTreeRecord16 *pUVar9;
  Recovered0040FFE0EaxCf5 RVar10;
  UiTimedListDirectoryHierarchyEaxEdxCf9 UVar11;
  UiTimedListDirectoryHierarchyEaxEdxCf9 UVar12;
  UiTimedListTreeRecord16 *pUVar13;
  
  iVar6 = 0;
  while( true ) {
    pWVar8 = &g_UiTimedListHierarchyPathScratch;
    for (iVar4 = 0x80; iVar4 != 0; iVar4 = iVar4 + -1) {
      pWVar8->firstTwoCodeUnits = *(dword *)selectedPathUtf16;
      selectedPathUtf16 = (word *)(selectedPathUtf16 + 2);
      pWVar8 = (WidePathBuffer256 *)(&pWVar8->firstTwoCodeUnits + 1);
    }
    if ((g_UiTimedListHierarchyPathScratch.codeUnits[3] == 0) ||
       (g_UiTimedListHierarchyPathScratch.codeUnits[2] == 0)) break;
    RVar10 = UiTimedListTree_BuildDirectoryRecordBlockCf
                       (g_UiTimedListHierarchyPathScratch.codeUnits);
    UVar11.rootRecordBlockOrError = RVar10.recordBlockOrError;
    if (RVar10.carry) goto joined_r0x00410484;
    WidePath_SplitParentAndLeaf
              (g_UiTimedListRecordPathScratch.codeUnits,
               g_UiTimedListHierarchyParentPathScratch.codeUnits,
               g_UiTimedListHierarchyPathScratch.codeUnits);
    UiTimedListTree_FindRecordByLabel
              (g_UiTimedListRecordPathScratch.codeUnits,UVar11.rootRecordBlockOrError);
    iVar6 = iVar6 + 1;
    selectedPathUtf16 = (word *)&g_UiTimedListHierarchyParentPathScratch;
    unaff_EBP = UVar11.rootRecordBlockOrError;
  }
  RVar10 = UiTimedListTree_BuildDirectoryRecordBlockCf(g_UiTimedListHierarchyPathScratch.codeUnits);
  pUVar3 = RVar10.recordBlockOrError;
  UVar11.rootRecordBlockOrError = pUVar3;
  if (!RVar10.carry) {
    dVar5 = pUVar3->recordCountOrRowPayload00;
    uVar2 = g_UiTimedListHierarchyPathScratch.codeUnits[0];
    pUVar13 = pUVar3;
    goto LAB_0041040e;
  }
  goto joined_r0x00410484;
  while( true ) {
    uVar2 = uVar1 ^ *(ushort *)pUVar13->recordCountOrRowPayload00;
    dVar5 = dVar5 - 1;
    if (dVar5 == 0) break;
LAB_0041040e:
    pUVar13 = pUVar13 + 1;
    uVar1 = uVar2 ^ *(ushort *)pUVar13->recordCountOrRowPayload00;
    if ((uVar1 & 0xdf) == 0) {
      iVar6 = iVar6 + 1;
      g_UiTimedListHierarchyPathScratch.codeUnits[0] = 0;
      RVar10 = UiTimedListTree_BuildDirectoryRecordBlockCf
                         (g_UiTimedListHierarchyPathScratch.codeUnits);
      UVar11.rootRecordBlockOrError = RVar10.recordBlockOrError;
      if (!RVar10.carry) {
        pUVar7 = UVar11.rootRecordBlockOrError + 1;
        pUVar9 = UVar11.rootRecordBlockOrError;
        do {
          if (pUVar3 != (UiTimedListTreeRecord16 *)0x0) {
            pUVar3->rowPayload04 = (dword)pUVar9;
            pUVar3->nestedRecordBlockOrParentLink08 = pUVar7;
          }
          if (pUVar7 != (UiTimedListTreeRecord16 *)0x0) {
            pUVar7->recordFlags0C =
                 pUVar7->recordFlags0C | UI_TIMED_LIST_RECORD_ENABLES_NESTED_CHILD_TRAVERSAL;
            pUVar7->nestedRecordBlockOrParentLink08 = pUVar3;
          }
          iVar6 = iVar6 + -1;
          pUVar7 = pUVar13;
          pUVar9 = pUVar3;
        } while (iVar6 != 0);
        UVar11.selectedRecordOrNull = pUVar13;
        UVar11.carry = false;
        return UVar11;
      }
      goto joined_r0x00410484;
    }
  }
  UVar11.rootRecordBlockOrError = (UiTimedListTreeRecord16 *)0x0;
joined_r0x00410484:
  for (; iVar6 != 0; iVar6 = iVar6 + -1) {
    (*g_MemoryApi.free)(unaff_EBP);
  }
  UVar12.selectedRecordOrNull = (UiTimedListTreeRecord16 *)0x0;
  UVar12.rootRecordBlockOrError = UVar11.rootRecordBlockOrError;
  UVar12.carry = true;
  return UVar12;
}

/* Address: 0x004104B0.
   Ownership: ui/controls/lists.
   Purpose: Recursively frees a timed-list record block while testing containment.
*/
bool __thandor_cf_preserve_eax_ecx_edx
UiTimedListTree_FreeRecordBlockRecursiveAndTestContainsCf
          (UiTimedListTreeRecord16 *targetRecord,UiTimedListTreeRecord16 *recordBlock)

{
  UiTimedListTreeRecord16 *pUVar1;
  dword dVar2;
  int iVar3;
  bool bVar4;
  
  iVar3 = 0;
  if (recordBlock != (UiTimedListTreeRecord16 *)0x0) {
    pUVar1 = recordBlock;
    for (dVar2 = recordBlock->recordCountOrRowPayload00; dVar2 != 0; dVar2 = dVar2 - 1) {
      if (pUVar1 + 1 == targetRecord) {
        iVar3 = iVar3 + 1;
      }
      if ((((pUVar1[1].recordFlags0C & UI_TIMED_LIST_RECORD_OBSERVED_BIT0) != 0) &&
          ((pUVar1[1].recordFlags0C & UI_TIMED_LIST_RECORD_ENABLES_NESTED_CHILD_TRAVERSAL) != 0)) &&
         (bVar4 = UiTimedListTree_FreeRecordBlockRecursiveAndTestContainsCf
                            (targetRecord,pUVar1[1].nestedRecordBlockOrParentLink08), bVar4)) {
        iVar3 = iVar3 + 1;
      }
      pUVar1 = pUVar1 + 1;
    }
  }
  (*g_MemoryApi.free)(recordBlock);
  return iVar3 != 0;
}

/* Address: 0x00410520.
   Ownership: ui/controls/lists.
   Purpose: Attaches a directory record block to the timed-list hierarchy.
*/
bool __thandor_cf_preserve_ecx_edx
UiTimedListTree_AttachDirectoryRecordBlockCf(UiTimedListTreeRecord16 *record)

{
  dword *directory;
  UiTimedListTreeRecord16 *pUVar1;
  int iVar2;
  UiTimedListTreeRecord16 *pUVar3;
  UiTimedListTreeRecord16 *pUVar4;
  dword *recordPathSourceDwords;
  dword *combinedPathSourceDwords;
  dword *recordPathScratchDestDwords;
  dword *combinedPathScratchDestDwords;
  Recovered0040FFE0EaxCf5 RVar5;
  
  recordPathSourceDwords = (dword *)record->recordCountOrRowPayload00;
  recordPathScratchDestDwords = (dword *)&g_UiTimedListRecordPathScratch;
  for (iVar2 = 0x80; iVar2 != 0; iVar2 = iVar2 + -1) {
    *recordPathScratchDestDwords = *recordPathSourceDwords;
    recordPathSourceDwords = recordPathSourceDwords + 1;
    recordPathScratchDestDwords = recordPathScratchDestDwords + 1;
  }
  if (((record[-1].recordFlags0C & UI_TIMED_LIST_RECORD_ANCESTOR_BOUNDARY) == 0) ||
     (record[-1].rowPayload04 != 0)) {
    pUVar1 = record;
    if (g_UiTimedListRecordPathScratch.codeUnits[1] == 0x3a) {
      g_UiTimedListRecordPathScratch.codeUnits[2] = 0;
      WidePath_CombineDirectoryAndLeaf
                (g_UiTimedListHierarchyParentPathScratch.codeUnits,(word *)0x40ff50,
                 g_UiTimedListRecordPathScratch.codeUnits);
    }
    else {
      while( true ) {
        do {
          pUVar3 = pUVar1;
          pUVar1 = pUVar3 + -1;
        } while ((pUVar3[-1].recordFlags0C & UI_TIMED_LIST_RECORD_ANCESTOR_BOUNDARY) == 0);
        pUVar1 = pUVar3[-1].nestedRecordBlockOrParentLink08;
        if (pUVar1 == (UiTimedListTreeRecord16 *)0x0) {
          return true;
        }
        directory = (dword *)pUVar1->recordCountOrRowPayload00;
        if (*(word *)((int)directory + 2) == 0x3a) break;
        WidePath_CombineDirectoryAndLeaf
                  (g_UiTimedListCombinedPathScratch.codeUnits,
                   g_UiTimedListRecordPathScratch.codeUnits,(word *)directory);
        combinedPathSourceDwords = (dword *)&g_UiTimedListCombinedPathScratch;
        combinedPathScratchDestDwords = (dword *)&g_UiTimedListRecordPathScratch;
        for (iVar2 = 0x80; iVar2 != 0; iVar2 = iVar2 + -1) {
          *combinedPathScratchDestDwords = *combinedPathSourceDwords;
          combinedPathSourceDwords = combinedPathSourceDwords + 1;
          combinedPathScratchDestDwords = combinedPathScratchDestDwords + 1;
        }
      }
      g_UiTimedListCombinedPathScratch.firstTwoCodeUnits = *directory;
      g_UiTimedListCombinedPathScratch._4_4_ = 0;
      WidePath_CombineDirectoryAndLeaf
                (g_UiTimedListSecondaryPathScratch.codeUnits,
                 g_UiTimedListRecordPathScratch.codeUnits,g_UiTimedListCombinedPathScratch.codeUnits
                );
      WidePath_CombineDirectoryAndLeaf
                (g_UiTimedListHierarchyParentPathScratch.codeUnits,(word *)0x40ff50,
                 g_UiTimedListSecondaryPathScratch.codeUnits);
    }
  }
  else {
    g_UiTimedListHierarchyParentPathScratch.firstTwoCodeUnits = 0x3a0061;
    g_UiTimedListHierarchyParentPathScratch._4_4_ = 0;
  }
  RVar5 = UiTimedListTree_BuildDirectoryRecordBlockCf
                    (g_UiTimedListHierarchyParentPathScratch.codeUnits);
  pUVar1 = RVar5.recordBlockOrError;
  if (RVar5.carry) {
    return true;
  }
  record->nestedRecordBlockOrParentLink08 = pUVar1;
  pUVar1->nestedRecordBlockOrParentLink08 = record;
  do {
    pUVar4 = record + -1;
    pUVar3 = record + -1;
    record = pUVar4;
  } while ((pUVar3->recordFlags0C & UI_TIMED_LIST_RECORD_ANCESTOR_BOUNDARY) == 0);
  pUVar1->rowPayload04 = (dword)pUVar4;
  return false;
}

/* Address: 0x00410670.
   Ownership: ui/controls/lists.
   Purpose: Toggles expansion for a timed-list directory record.
*/
void __thandor_void_preserve_eax_ecx
UiTimedListControl_ToggleDirectoryRecordExpansion
          (UiTimedListTreeRecord16 *record,UiTimedListRuntimeExtendedView88 *control)

{
  UiTimedListTreeRecord16 *targetRecord;
  bool bVar1;
  
  targetRecord = UiTimedListControl_GetSelectedRecord(control);
  if ((record->recordFlags0C & UI_TIMED_LIST_RECORD_OBSERVED_BIT0) != 0) {
    if ((record->recordFlags0C & UI_TIMED_LIST_RECORD_ENABLES_NESTED_CHILD_TRAVERSAL) != 0) {
      bVar1 = UiTimedListTree_FreeRecordBlockRecursiveAndTestContainsCf
                        (targetRecord,record->nestedRecordBlockOrParentLink08);
      if (bVar1) {
        UiActionQueue_Enqueue((control->base).actionId,control);
        targetRecord = record;
      }
      record->recordFlags0C =
           record->recordFlags0C & ~UI_TIMED_LIST_RECORD_ENABLES_NESTED_CHILD_TRAVERSAL;
      UiTimedListControl_SetRecordTreeAndRecomputeLayout((control->base).recordTree,control);
      UiTimedListControl_SelectRecordAndScrollIntoView(targetRecord,control);
      return;
    }
    bVar1 = UiTimedListTree_AttachDirectoryRecordBlockCf(record);
    if (!bVar1) {
      record->recordFlags0C =
           record->recordFlags0C | UI_TIMED_LIST_RECORD_ENABLES_NESTED_CHILD_TRAVERSAL;
      UiTimedListControl_SetRecordTreeAndRecomputeLayout((control->base).recordTree,control);
      UiTimedListControl_SelectRecordAndScrollIntoView(targetRecord,control);
    }
  }
  return;
}

/* Address: 0x00410700.
   Ownership: ui/controls/lists.
   Purpose: Builds the recovered path for a timed-list tree record.
*/
bool __thandor_cf_preserve_ecx_edx
UiTimedListTree_BuildRecordPathCf(dword *outputPathDwords,UiTimedListTreeRecord16 *record)

{
  dword *directory;
  int iVar1;
  UiTimedListTreeRecord16 *pUVar2;
  dword *recordPathSourceDwords;
  dword *combinedPathSourceDwords;
  dword *recordPathScratchDestDwords;
  dword *combinedPathScratchDestDwords;
  
  recordPathScratchDestDwords = (dword *)&g_UiTimedListRecordPathScratch;
  recordPathSourceDwords = (dword *)record->recordCountOrRowPayload00;
  iVar1 = 0x80;
  if (((record[-1].recordFlags0C & UI_TIMED_LIST_RECORD_ANCESTOR_BOUNDARY) == 0) ||
     (record[-1].rowPayload04 != 0)) {
    for (; iVar1 != 0; iVar1 = iVar1 + -1) {
      *recordPathScratchDestDwords = *recordPathSourceDwords;
      recordPathSourceDwords = (dword *)(recordPathSourceDwords + 1);
      recordPathScratchDestDwords = recordPathScratchDestDwords + 1;
    }
    if (g_UiTimedListRecordPathScratch.codeUnits[1] == 0x3a) {
      g_UiTimedListRecordPathScratch.codeUnits[2] = 0;
      recordPathSourceDwords = (dword *)&g_UiTimedListRecordPathScratch;
    }
    else {
      while( true ) {
        do {
          pUVar2 = record;
          record = pUVar2 + -1;
        } while ((pUVar2[-1].recordFlags0C & UI_TIMED_LIST_RECORD_ANCESTOR_BOUNDARY) == 0);
        record = pUVar2[-1].nestedRecordBlockOrParentLink08;
        if (record == (UiTimedListTreeRecord16 *)0x0) {
          return true;
        }
        directory = (dword *)record->recordCountOrRowPayload00;
        if (*(word *)((int)directory + 2) == 0x3a) break;
        WidePath_CombineDirectoryAndLeaf
                  (g_UiTimedListCombinedPathScratch.codeUnits,
                   g_UiTimedListRecordPathScratch.codeUnits,(word *)directory);
        combinedPathSourceDwords = (dword *)&g_UiTimedListCombinedPathScratch;
        combinedPathScratchDestDwords = (dword *)&g_UiTimedListRecordPathScratch;
        for (iVar1 = 0x80; iVar1 != 0; iVar1 = iVar1 + -1) {
          *combinedPathScratchDestDwords = *combinedPathSourceDwords;
          combinedPathSourceDwords = combinedPathSourceDwords + 1;
          combinedPathScratchDestDwords = combinedPathScratchDestDwords + 1;
        }
      }
      g_UiTimedListCombinedPathScratch.firstTwoCodeUnits = *directory;
      g_UiTimedListCombinedPathScratch._4_4_ = 0;
      WidePath_CombineDirectoryAndLeaf
                (g_UiTimedListSecondaryPathScratch.codeUnits,
                 g_UiTimedListRecordPathScratch.codeUnits,g_UiTimedListCombinedPathScratch.codeUnits
                );
      recordPathSourceDwords = (dword *)&g_UiTimedListSecondaryPathScratch;
    }
  }
  for (iVar1 = 0x80; iVar1 != 0; iVar1 = iVar1 + -1) {
    *outputPathDwords = *recordPathSourceDwords;
    recordPathSourceDwords = (dword *)(recordPathSourceDwords + 1);
    outputPathDwords = outputPathDwords + 1;
  }
  return false;
}

/* Address: 0x004B11C0.
   Ownership: ui/controls/lists.
   Purpose: Traverses a sibling list and forwards the action ID through vtable slot +0x3C, restoring matching
   controls. Kept distinct from player IDs, command opcodes, and resource identifiers. Typed parameters: p0
   actionId→UiActionId_V338. Calling convention, storage, body bytes, control flow, and executable data remain
   unchanged.
*/
void __thandor_void_preserve_eax_ecx_edx
UiNodeList_UnsuppressActionId(UiActionId actionId,UiNodeBase *firstNode)

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
void __thandor_void_preserve_eax_ecx_edx
UiNodeList_SuppressActionId(UiActionId actionId,UiNodeBase *firstNode)

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
bool __thandor_cf_preserve_eax_ecx_edx
UiSelectableControl_KeyboardEventCf
          (UiKeyboardStateMask keyboardStateMask,UiKeyboardEventCode keyCode,
          UiSelectableControl *control)

{
  bool bVar1;
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
    return false;
  }
  if ((control->stateFlags & UI_SELECTABLE_TOGGLE_ON_ACTIVATION) != 0) {
    if (((control->stateFlags & 0x80) != 0) && (control[1].base.parent != (UiNodeBase *)0x0)) {
      (*g_SoundPlayOneShot)
                (g_UiSoundGainQ15,g_UiSoundGainQ15,(DirectSoundVoiceSet *)control[1].base.parent);
    }
    control->stateFlags = control->stateFlags ^ UI_SELECTABLE_SELECTED_OR_CHECKED;
    UiActionQueue_Enqueue(control->actionId,control);
    UiNode_InvalidateRoot(&control->base);
    return false;
  }
  if ((control->stateFlags & UI_SELECTABLE_SELECTED_OR_CHECKED) == 0) {
    if (((control->stateFlags & 0x80) != 0) && (control[1].base.parent != (UiNodeBase *)0x0)) {
      (*g_SoundPlayOneShot)
                (g_UiSoundGainQ15,g_UiSoundGainQ15,(DirectSoundVoiceSet *)control[1].base.parent);
    }
    control->stateFlags = control->stateFlags | UI_SELECTABLE_SELECTED_OR_CHECKED;
    UiActionQueue_Enqueue(control->actionId,control);
    UiNode_InvalidateRoot(&control->base);
    return false;
  }
UiSelectableControl_DelegateUnhandledKeyboardEvent:
  bVar1 = UiNode_DefaultKeyboardEventMoveFocusNextCf(keyboardStateMask,keyCode,&control->base);
  return bVar1;
}


/* Address: 0x004B26E0.
   Ownership: ui/controls/lists.
   Purpose: When actionId matches control->actionId, sets nodeFlags bit 0x08 and updates focus/activation state for
   the newly suppressed control.
   Cross-module calls: UiKeyboardFocus_ReleaseNode [ui/controls/input].
*/
void __thandor_void_preserve_eax_ecx_edx
UiSelectableControl_SuppressIfActionId(UiActionId actionId,UiSelectableControl *control)

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
void __thandor_void_preserve_eax_ecx_edx
UiSelectableControl_UnsuppressIfActionId(UiActionId actionId,UiSelectableControl *control)

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
UiSelectableNodeEaxEcxCf9 __thandor_eax_ecx_cf_preserve_edx
UiSelectableGroup_NoneVisibleSelectedCf(UiControlCount controlCount)

{
  int iVar1;
  uint controlIndex;
  int controlPointerByteOffset;
  UiSelectableNodeEaxEcxCf9 UVar2;
  UiSelectableNodeEaxEcxCf9 UVar3;
  
  controlPointerByteOffset = 0;
  controlIndex = 0;
  while ((iVar1 = *(int *)(&stack0x00000008 + controlPointerByteOffset),
         (*(uint *)(iVar1 + 0x48) & 8) != 0 || ((*(uint *)(iVar1 + 0x4c) & 2) == 0))) {
    controlIndex = controlIndex + 1;
    controlPointerByteOffset = controlPointerByteOffset + 4;
    if (controlCount <= controlIndex) {
      UVar2.node = (UiNodeBase *)iVar1;
      UVar2.carry = true;
      return UVar2;
    }
  }
  UVar3.controlIndexOrCount = controlIndex;
  UVar3.node = (UiNodeBase *)iVar1;
  UVar3.carry = false;
  return UVar3;
}


/* Address: 0x004B2D70.
   Ownership: ui/controls/lists.
   Purpose: Variadic group test that ignores node visibility. CF=0 when any listed control has selected bit 0x02;
   CF=1 when none does. Kept distinct from player IDs, command opcodes, and resource identifiers. Typed parameters:
   p0 controlCount→UiControlCount_V338. Calling convention, storage, body bytes, control flow, and executable data
   remain unchanged.
*/
UiSelectableGroupIndexEcxCf5 __thandor_eax_ecx_cf_preserve_edx
UiSelectableGroup_NoneSelectedCf(UiControlCount controlCount)

{
  uint controlIndex;
  int controlPointerByteOffset;
  UiSelectableGroupIndexEcxCf5 UVar1;
  UiSelectableGroupIndexEcxCf5 UVar2;
  
  controlPointerByteOffset = 0;
  controlIndex = 0;
  do {
    if ((*(uint *)(*(int *)(&stack0x00000008 + controlPointerByteOffset) + 0x4c) & 2) != 0) {
      UVar2.carryNoneSelected = false;
      UVar2.selectedIndexOrCount = controlIndex;
      return UVar2;
    }
    controlIndex = controlIndex + 1;
    controlPointerByteOffset = controlPointerByteOffset + 4;
  } while (controlIndex < controlCount);
  UVar1.carryNoneSelected = true;
  return UVar1;
}


/* Address: 0x004B2DA0.
   Ownership: ui/controls/lists.
   Purpose: Variadic exclusive-selection helper. Sets selected bit 0x02 only on the chosen control, clears it on
   the remaining controls, and invalidates every listed control. Kept distinct from player IDs, command opcodes,
   and resource identifiers. Typed parameters: p0 controlCount→UiControlCount_V338. Calling convention, storage,
   body bytes, control flow, and executable data remain unchanged.
   Cross-module calls: UiNode_InvalidateRoot [ui/core/runtime].
*/
void __thandor_void_preserve_eax_ecx_edx
UiSelectableGroup_SelectExclusive(UiControlCount controlCount,UiNodeBase *selectedControl)

{
  UiNodeBase *node;
  uint uVar1;
  int controlPointerByteOffset;
  
  controlPointerByteOffset = 0;
  uVar1 = 0;
  do {
    node = *(UiNodeBase **)(&stack0x0000000c + controlPointerByteOffset);
    if (node == selectedControl) {
      node[1].nextSibling = (UiNodeBase *)((uint)node[1].nextSibling | 2);
    }
    else {
      node[1].nextSibling = (UiNodeBase *)((uint)node[1].nextSibling & 0xfffffffd);
    }
    UiNode_InvalidateRoot(node);
    uVar1 = uVar1 + 1;
    controlPointerByteOffset = controlPointerByteOffset + 4;
  } while (uVar1 < controlCount);
  return;
}


/* Address: 0x004B2DE0.
   Ownership: ui/controls/lists.
   Purpose: Tests one selectable control. CF=1 only when nodeFlags bit 0x08 is clear and stateFlags bit 0x02 is
   set; otherwise CF=0.
*/
byte __thandor_cf_preserve_eax_ecx_edx
UiSelectableControl_IsSelectedCf(UiSelectableControl *control)

{
  if ((((control->base).nodeFlags & UI_NODE_SUPPRESSED) == 0) &&
     ((control->stateFlags & UI_SELECTABLE_SELECTED_OR_CHECKED) != 0)) {
    return 1;
  }
  return 0;
}


/* Address: 0x004B2E10.
   Ownership: ui/controls/lists.
   Purpose: Clears selected bit 0x02, sets it when the boolean argument is nonzero, then invalidates the control
   root. Typed parameters: p0 selected→UiBooleanState32_V342. Calling convention, exact VariableStorage
   serialization, function body bytes, control flow, globals, locals, and executable data remain unchanged.
   Cross-module calls: UiNode_InvalidateRoot [ui/core/runtime].
*/
void __thandor_void_preserve_eax_ecx_edx
UiSelectableControl_SetSelected(UiBooleanState32 selected,UiSelectableControl *control)

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
StatusValueEaxCf5 __thandor_eax_cf_preserve_ecx_edx
UiPageStack_ActivePageNotInListCf(UiPageStackControl *stack)

{
  uint pageIndex;
  bool bVar1;
  StatusValueEaxCf5 SVar2;
  
  pageIndex = 0;
  do {
    bVar1 = false;
    if ((stack->base).firstChild == (&stack->pages)[pageIndex]) goto LAB_004b4944;
    pageIndex = pageIndex + 1;
  } while (pageIndex < stack->pageCount);
  bVar1 = true;
LAB_004b4944:
  SVar2.carry = bVar1;
  SVar2.valueOrError = pageIndex;
  return SVar2;
}


/* Address: 0x004B7970.
   Ownership: ui/controls/lists.
   Purpose: Binary entry is anchored by g_UiNodeVtable_004B7920[0]@004B7920.
   Cross-module calls: UiContainer_RelocateChildren [ui/controls/layout].
*/
void __thandor_void_preserve_eax_ecx_edx
UiScrollableControl_RelocateChildren
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
void __thandor_void_preserve_eax_ecx_edx
UiScrollableControl_DrawFrameContentAndScrollbars
          (UiPixelCoordinate clipTop,UiPixelCoordinate clipLeft,UiPixelCoordinate clipBottom,
          UiPixelCoordinate clipRight,UiScrollableControl *control)

{
  dword dVar1;
  GraphicsSubresourceIndex subresource;
  dword tileEnd;
  dword dVar2;
  int iVar3;
  int iVar4;
  int iVar5;
  int iVar6;
  int iVar7;
  int iVar8;
  int iVar9;
  bool bVar10;
  GraphicsTextureSizeEaxEdxCf9 GVar11;
  dword dVar12;
  
  bVar10 = (*g_GraphicsFramebufferBeginAccess)();
  if (!bVar10) {
    tileEnd = 0;
    iVar5 = 0;
    iVar8 = (control->base).layoutWidth;
    iVar7 = (control->base).layoutHeight;
    if ((control->scrollStateFlags &
        (UI_SCROLL_HORIZONTAL_BAR_AT_BOTTOM|UI_SCROLL_HORIZONTAL_BAR_AT_TOP)) != 0) {
      dVar12 = 0;
      iVar5 = 0;
      iVar4 = iVar8;
      if ((control->scrollStateFlags & UI_SCROLL_HORIZONTAL_BAR_AT_BOTTOM) == 0) {
        iVar6 = 0;
        GVar11 = (*g_GraphicsTextureSourceGetLogicalSize)(0x5a,g_UiWindowTextureSource);
        dVar1 = GVar11.logicalWidthPixels;
        iVar5 = iVar5 + GVar11.logicalHeightPixels;
      }
      else {
        iVar3 = iVar7;
        GVar11 = (*g_GraphicsTextureSourceGetLogicalSize)(0x5a,g_UiWindowTextureSource);
        dVar1 = GVar11.logicalWidthPixels;
        iVar6 = iVar7 - GVar11.logicalHeightPixels;
        iVar7 = iVar3 - GVar11.logicalHeightPixels;
      }
      GVar11 = (*g_GraphicsTextureSourceGetLogicalSize)(0x5e,g_UiWindowTextureSource);
      dVar2 = tileEnd;
      if ((control->scrollStateFlags & UI_SCROLL_VERTICAL_BAR_AT_LEFT) != 0) {
        dVar2 = GVar11.logicalWidthPixels;
      }
      if ((control->scrollStateFlags & UI_SCROLL_VERTICAL_BAR_AT_RIGHT) != 0) {
        iVar8 = iVar8 - GVar11.logicalWidthPixels;
      }
      if (((control->scrollStateFlags & UI_SCROLL_HORIZONTAL_DECREMENT_ACTIVE) == 0) ||
         ((control->scrollStateFlags & UI_SCROLL_PRIMARY_INTERACTION_ACTIVE) == 0)) {
        (*g_GraphicsTextureSourceBlitSourceAlpha)
                  (clipTop,clipLeft,clipBottom,clipRight,iVar6 + (control->base).top,
                   dVar2 + (control->base).left,0x5a,g_UiWindowTextureSource,g_FramebufferAccess);
        tileEnd = dVar12;
      }
      else {
        (*g_GraphicsTextureSourceBlitSourceAlpha)
                  (clipTop,clipLeft,clipBottom,clipRight,iVar6 + (control->base).top,
                   dVar2 + (control->base).left,0x62,g_UiWindowTextureSource,g_FramebufferAccess);
        tileEnd = dVar12;
      }
      iVar3 = dVar2 + dVar1;
      iVar9 = iVar8 - dVar1;
      if (((control->scrollStateFlags & UI_SCROLL_HORIZONTAL_INCREMENT_ACTIVE) == 0) ||
         ((control->scrollStateFlags & UI_SCROLL_PRIMARY_INTERACTION_ACTIVE) == 0)) {
        (*g_GraphicsTextureSourceBlitSourceAlpha)
                  (clipTop,clipLeft,clipBottom,clipRight,iVar6 + (control->base).top,
                   iVar9 + (control->base).left,0x5b,g_UiWindowTextureSource,g_FramebufferAccess);
        iVar8 = iVar4;
      }
      else {
        (*g_GraphicsTextureSourceBlitSourceAlpha)
                  (clipTop,clipLeft,clipBottom,clipRight,iVar6 + (control->base).top,
                   iVar9 + (control->base).left,99,g_UiWindowTextureSource,g_FramebufferAccess);
        iVar8 = iVar4;
      }
      if ((control->scrollStateFlags & UI_SCROLL_HORIZONTAL_TRACK_BEFORE_THUMB_ACTIVE) == 0) {
        UiWindow_BlitTiledHorizontalEdge
                  (clipTop,clipLeft,clipBottom,clipRight,0x5d,control->horizontalThumbLeft,iVar6,
                   iVar3,control);
      }
      else {
        UiWindow_BlitTiledHorizontalEdge
                  (clipTop,clipLeft,clipBottom,clipRight,0x65,control->horizontalThumbLeft,iVar6,
                   iVar3,control);
      }
      iVar4 = control->horizontalThumbRight;
      if ((control->scrollStateFlags & UI_SCROLL_HORIZONTAL_TRACK_AFTER_THUMB_ACTIVE) == 0) {
        if (iVar4 < clipRight) {
          iVar4 = clipRight;
        }
        UiWindow_BlitTiledHorizontalEdge
                  (clipTop,clipLeft,clipBottom,iVar4,0x5d,iVar9,iVar6,iVar3,control);
      }
      else {
        if (iVar4 < clipRight) {
          iVar4 = clipRight;
        }
        UiWindow_BlitTiledHorizontalEdge
                  (clipTop,clipLeft,clipBottom,iVar4,0x65,iVar9,iVar6,iVar3,control);
      }
      GVar11 = (*g_GraphicsTextureSourceGetLogicalSize)(0xc0,g_UiWindowTextureSource);
      dVar12 = GVar11.logicalWidthPixels;
      iVar4 = control->horizontalThumbLeft;
      iVar3 = control->horizontalThumbRight;
      if ((control->scrollStateFlags & UI_SCROLL_HORIZONTAL_THUMB_ACTIVE) == 0) {
        (*g_GraphicsTextureSourceBlitSourceAlpha)
                  (clipTop,clipLeft,clipBottom,clipRight,iVar6 + (control->base).top,
                   iVar4 + (control->base).left,0xc0,g_UiWindowTextureSource,g_FramebufferAccess);
        iVar3 = iVar3 - dVar12;
        (*g_GraphicsTextureSourceBlitSourceAlpha)
                  (clipTop,clipLeft,clipBottom,clipRight,iVar6 + (control->base).top,
                   iVar3 + (control->base).left,0xc1,g_UiWindowTextureSource,g_FramebufferAccess);
        UiWindow_BlitTiledHorizontalEdge
                  (clipTop,clipLeft,clipBottom,clipRight,0x5c,iVar3,iVar6,iVar4 + dVar12,control);
      }
      else {
        (*g_GraphicsTextureSourceBlitSourceAlpha)
                  (clipTop,clipLeft,clipBottom,clipRight,iVar6 + (control->base).top,
                   iVar4 + (control->base).left,0xc4,g_UiWindowTextureSource,g_FramebufferAccess);
        iVar3 = iVar3 - dVar12;
        (*g_GraphicsTextureSourceBlitSourceAlpha)
                  (clipTop,clipLeft,clipBottom,clipRight,iVar6 + (control->base).top,
                   iVar3 + (control->base).left,0xc5,g_UiWindowTextureSource,g_FramebufferAccess);
        UiWindow_BlitTiledHorizontalEdge
                  (clipTop,clipLeft,clipBottom,clipRight,100,iVar3,iVar6,iVar4 + dVar12,control);
      }
    }
    if ((control->scrollStateFlags &
        (UI_SCROLL_VERTICAL_BAR_AT_RIGHT|UI_SCROLL_VERTICAL_BAR_AT_LEFT)) != 0) {
      iVar4 = iVar7;
      iVar6 = iVar5;
      if ((control->scrollStateFlags & UI_SCROLL_VERTICAL_BAR_AT_RIGHT) == 0) {
        dVar12 = tileEnd;
        GVar11 = (*g_GraphicsTextureSourceGetLogicalSize)(0x5e,g_UiWindowTextureSource);
        dVar1 = GVar11.logicalHeightPixels;
        dVar12 = dVar12 + GVar11.logicalWidthPixels;
      }
      else {
        iVar3 = iVar8;
        dVar12 = tileEnd;
        GVar11 = (*g_GraphicsTextureSourceGetLogicalSize)(0x5e,g_UiWindowTextureSource);
        dVar1 = GVar11.logicalHeightPixels;
        tileEnd = iVar8 - GVar11.logicalWidthPixels;
        iVar8 = iVar3 - GVar11.logicalWidthPixels;
      }
      if (((control->scrollStateFlags & UI_SCROLL_VERTICAL_DECREMENT_ACTIVE) == 0) ||
         ((control->scrollStateFlags & UI_SCROLL_PRIMARY_INTERACTION_ACTIVE) == 0)) {
        (*g_GraphicsTextureSourceBlitSourceAlpha)
                  (clipTop,clipLeft,clipBottom,clipRight,iVar5 + (control->base).top,
                   tileEnd + (control->base).left,0x5e,g_UiWindowTextureSource,g_FramebufferAccess);
      }
      else {
        (*g_GraphicsTextureSourceBlitSourceAlpha)
                  (clipTop,clipLeft,clipBottom,clipRight,iVar5 + (control->base).top,
                   tileEnd + (control->base).left,0x66,g_UiWindowTextureSource,g_FramebufferAccess);
      }
      iVar3 = iVar5 + dVar1;
      iVar9 = iVar7 - dVar1;
      if (((control->scrollStateFlags & UI_SCROLL_VERTICAL_INCREMENT_ACTIVE) == 0) ||
         ((control->scrollStateFlags & UI_SCROLL_PRIMARY_INTERACTION_ACTIVE) == 0)) {
        (*g_GraphicsTextureSourceBlitSourceAlpha)
                  (clipTop,clipLeft,clipBottom,clipRight,iVar9 + (control->base).top,
                   tileEnd + (control->base).left,0x5f,g_UiWindowTextureSource,g_FramebufferAccess);
        iVar7 = iVar4;
        iVar5 = iVar6;
      }
      else {
        (*g_GraphicsTextureSourceBlitSourceAlpha)
                  (clipTop,clipLeft,clipBottom,clipRight,iVar9 + (control->base).top,
                   tileEnd + (control->base).left,0x67,g_UiWindowTextureSource,g_FramebufferAccess);
        iVar7 = iVar4;
        iVar5 = iVar6;
      }
      if ((control->scrollStateFlags & UI_SCROLL_VERTICAL_TRACK_BEFORE_THUMB_ACTIVE) == 0) {
        UiWindow_BlitTiledVerticalEdge
                  (clipTop,clipLeft,clipBottom,clipRight,0x61,control->verticalThumbTop,iVar3,
                   tileEnd,control);
      }
      else {
        UiWindow_BlitTiledVerticalEdge
                  (clipTop,clipLeft,clipBottom,clipRight,0x69,control->verticalThumbTop,iVar3,
                   tileEnd,control);
      }
      iVar4 = control->verticalThumbBottom;
      if ((control->scrollStateFlags & UI_SCROLL_VERTICAL_TRACK_AFTER_THUMB_ACTIVE) == 0) {
        if (iVar4 < clipBottom) {
          iVar4 = clipBottom;
        }
        UiWindow_BlitTiledVerticalEdge
                  (clipTop,clipLeft,iVar4,clipRight,0x61,iVar9,iVar3,tileEnd,control);
      }
      else {
        if (iVar4 < clipBottom) {
          iVar4 = clipBottom;
        }
        UiWindow_BlitTiledVerticalEdge
                  (clipTop,clipLeft,iVar4,clipRight,0x69,iVar9,iVar3,tileEnd,control);
      }
      GVar11 = (*g_GraphicsTextureSourceGetLogicalSize)(0xc2,g_UiWindowTextureSource);
      dVar1 = GVar11.logicalHeightPixels;
      iVar4 = control->verticalThumbTop;
      iVar6 = control->verticalThumbBottom;
      if ((control->scrollStateFlags & UI_SCROLL_VERTICAL_THUMB_ACTIVE) == 0) {
        (*g_GraphicsTextureSourceBlitSourceAlpha)
                  (clipTop,clipLeft,clipBottom,clipRight,iVar4 + (control->base).top,
                   tileEnd + (control->base).left,0xc2,g_UiWindowTextureSource,g_FramebufferAccess);
        iVar6 = iVar6 - dVar1;
        (*g_GraphicsTextureSourceBlitSourceAlpha)
                  (clipTop,clipLeft,clipBottom,clipRight,iVar6 + (control->base).top,
                   tileEnd + (control->base).left,0xc3,g_UiWindowTextureSource,g_FramebufferAccess);
        UiWindow_BlitTiledVerticalEdge
                  (clipTop,clipLeft,clipBottom,clipRight,0x60,iVar6,iVar4 + dVar1,tileEnd,control);
        tileEnd = dVar12;
      }
      else {
        (*g_GraphicsTextureSourceBlitSourceAlpha)
                  (clipTop,clipLeft,clipBottom,clipRight,iVar4 + (control->base).top,
                   tileEnd + (control->base).left,0xc6,g_UiWindowTextureSource,g_FramebufferAccess);
        iVar6 = iVar6 - dVar1;
        (*g_GraphicsTextureSourceBlitSourceAlpha)
                  (clipTop,clipLeft,clipBottom,clipRight,iVar6 + (control->base).top,
                   tileEnd + (control->base).left,199,g_UiWindowTextureSource,g_FramebufferAccess);
        UiWindow_BlitTiledVerticalEdge
                  (clipTop,clipLeft,clipBottom,clipRight,0x68,iVar6,iVar4 + dVar1,tileEnd,control);
        tileEnd = dVar12;
      }
    }
    if ((control->scrollStateFlags & 0x400) != 0) {
      GVar11 = (*g_GraphicsTextureSourceGetLogicalSize)(0x6a,g_UiWindowTextureSource);
      dVar12 = GVar11.logicalWidthPixels;
      iVar7 = iVar7 - GVar11.logicalHeightPixels;
      iVar8 = iVar8 - dVar12;
      (*g_GraphicsTextureSourceBlitSourceAlpha)
                (clipTop,clipLeft,clipBottom,clipRight,iVar5 + (control->base).top,
                 tileEnd + (control->base).left,0x6a,g_UiWindowTextureSource,g_FramebufferAccess);
      (*g_GraphicsTextureSourceBlitSourceAlpha)
                (clipTop,clipLeft,clipBottom,clipRight,iVar5 + (control->base).top,
                 iVar8 + (control->base).left,0x6b,g_UiWindowTextureSource,g_FramebufferAccess);
      (*g_GraphicsTextureSourceBlitSourceAlpha)
                (clipTop,clipLeft,clipBottom,clipRight,iVar7 + (control->base).top,
                 tileEnd + (control->base).left,0x6c,g_UiWindowTextureSource,g_FramebufferAccess);
      (*g_GraphicsTextureSourceBlitSourceAlpha)
                (clipTop,clipLeft,clipBottom,clipRight,iVar7 + (control->base).top,
                 iVar8 + (control->base).left,0x6d,g_UiWindowTextureSource,g_FramebufferAccess);
      UiWindow_BlitTiledHorizontalEdge
                (clipTop,clipLeft,clipBottom,clipRight,0x6e,iVar8,iVar5,tileEnd + dVar12,control);
      iVar5 = iVar5 + GVar11.logicalHeightPixels;
      iVar4 = (tileEnd + dVar12) - dVar12;
      UiWindow_BlitTiledVerticalEdge
                (clipTop,clipLeft,clipBottom,clipRight,0x6f,iVar7,iVar5,iVar4,control);
      UiWindow_BlitTiledVerticalEdge
                (clipTop,clipLeft,clipBottom,clipRight,0x70,iVar7,iVar5,iVar8,control);
      tileEnd = iVar4 + dVar12;
      UiWindow_BlitTiledHorizontalEdge
                (clipTop,clipLeft,clipBottom,clipRight,0x71,iVar8,iVar7,tileEnd,control);
    }
    if ((control->scrollStateFlags & 0x800) != 0) {
      GVar11 = (*g_GraphicsTextureSourceGetLogicalSize)(0x72,g_UiWindowTextureSource);
      dVar12 = GVar11.logicalWidthPixels;
      iVar7 = iVar7 - GVar11.logicalHeightPixels;
      iVar8 = iVar8 - dVar12;
      (*g_GraphicsTextureSourceBlitSourceAlpha)
                (clipTop,clipLeft,clipBottom,clipRight,iVar5 + (control->base).top,
                 tileEnd + (control->base).left,0x72,g_UiWindowTextureSource,g_FramebufferAccess);
      (*g_GraphicsTextureSourceBlitSourceAlpha)
                (clipTop,clipLeft,clipBottom,clipRight,iVar5 + (control->base).top,
                 iVar8 + (control->base).left,0x73,g_UiWindowTextureSource,g_FramebufferAccess);
      (*g_GraphicsTextureSourceBlitSourceAlpha)
                (clipTop,clipLeft,clipBottom,clipRight,iVar7 + (control->base).top,
                 tileEnd + (control->base).left,0x74,g_UiWindowTextureSource,g_FramebufferAccess);
      (*g_GraphicsTextureSourceBlitSourceAlpha)
                (clipTop,clipLeft,clipBottom,clipRight,iVar7 + (control->base).top,
                 iVar8 + (control->base).left,0x75,g_UiWindowTextureSource,g_FramebufferAccess);
      UiWindow_BlitTiledHorizontalEdge
                (clipTop,clipLeft,clipBottom,clipRight,0x76,iVar8,iVar5,tileEnd + dVar12,control);
      iVar5 = iVar5 + GVar11.logicalHeightPixels;
      iVar4 = (tileEnd + dVar12) - dVar12;
      UiWindow_BlitTiledVerticalEdge
                (clipTop,clipLeft,clipBottom,clipRight,0x77,iVar7,iVar5,iVar4,control);
      UiWindow_BlitTiledVerticalEdge
                (clipTop,clipLeft,clipBottom,clipRight,0x78,iVar7,iVar5,iVar8,control);
      tileEnd = iVar4 + dVar12;
      UiWindow_BlitTiledHorizontalEdge
                (clipTop,clipLeft,clipBottom,clipRight,0x79,iVar8,iVar7,tileEnd,control);
    }
    if ((control->scrollStateFlags & 0x300) != 0) {
      subresource = 0;
      if ((control->scrollStateFlags & 0x100) != 0) {
        subresource = 0x7a;
      }
      UiWindow_BlitTiledInterior
                (clipTop,clipLeft,clipBottom,clipRight,subresource,iVar7,iVar8,iVar5,tileEnd,control
                );
    }
    (*g_GraphicsFramebufferEndAccess)();
    iVar4 = tileEnd + (control->base).left;
    iVar5 = iVar5 + (control->base).top;
    iVar8 = iVar8 + (control->base).left;
    iVar7 = iVar7 + (control->base).top;
    if (iVar4 < clipRight) {
      iVar4 = clipRight;
    }
    if (iVar5 < clipBottom) {
      iVar5 = clipBottom;
    }
    if (clipLeft < iVar8) {
      iVar8 = clipLeft;
    }
    if (clipTop < iVar7) {
      iVar7 = clipTop;
    }
    UiContainer_DrawIntersectingChildren(iVar7,iVar8,iVar5,iVar4,&control->base);
  }
  return;
}


/* Address: 0x004B8310.
   Ownership: ui/controls/lists.
   Purpose: Rebuilds the scrollable child rectangle, content extents, horizontal and vertical scrollbar visibility,
   track bounds, thumb geometry, and clamped content offsets from the control rectangle and active child
   dimensions.
*/
void __thandor_void_preserve_eax_ecx_edx
UiScrollableControl_RebuildViewportAndScrollbars(UiScrollableControl *control)

{
  UiNodeBase *pUVar1;
  UiPixelExtent UVar2;
  UiPixelExtent UVar3;
  uint uVar4;
  dword dVar5;
  uint uVar6;
  UiPixelOffset UVar7;
  UiPixelOffset UVar8;
  UiPixelExtent UVar9;
  int iVar10;
  UiPixelExtent UVar11;
  int iVar12;
  GraphicsTextureSizeEaxEdxCf9 GVar13;
  
  pUVar1 = (control->base).firstChild;
  UVar11 = (control->base).right - (control->base).left;
  UVar9 = (control->base).bottom - (control->base).top;
  control->scrollStateFlags =
       control->scrollStateFlags &
       ~(UI_SCROLL_VERTICAL_BAR_AT_RIGHT|UI_SCROLL_VERTICAL_BAR_AT_LEFT|
         UI_SCROLL_HORIZONTAL_BAR_AT_BOTTOM|UI_SCROLL_HORIZONTAL_BAR_AT_TOP);
  (control->base).layoutWidth = UVar11;
  (control->base).layoutHeight = UVar9;
  if (pUVar1 != (UiNodeBase *)0xffffffff) {
    UVar7 = control->scrollOffsetY;
    iVar12 = (control->base).top;
    pUVar1->left = pUVar1->leftOffset + control->scrollOffsetX + (control->base).left;
    pUVar1->top = pUVar1->topOffset + UVar7 + iVar12;
    UVar2 = pUVar1->rightOffset;
    UVar3 = pUVar1->bottomOffset;
    control->contentWidth = UVar2;
    control->contentHeight = UVar3;
    UVar7 = control->scrollOffsetY;
    iVar12 = (control->base).top;
    pUVar1->right = UVar2 + control->scrollOffsetX + (control->base).left;
    pUVar1->bottom = UVar3 + UVar7 + iVar12;
    control->contentOriginX = 0;
    control->contentOriginY = 0;
    if ((control->scrollStateFlags & 0x400) != 0) {
      GVar13 = (*g_GraphicsTextureSourceGetLogicalSize)(0x6a,g_UiWindowTextureSource);
      control->contentOriginX = control->contentOriginX + GVar13.logicalWidthPixels;
      control->contentOriginY = control->contentOriginY + GVar13.logicalHeightPixels;
      UVar11 = UVar11 + GVar13.logicalWidthPixels * -2;
      UVar9 = UVar9 + GVar13.logicalHeightPixels * -2;
    }
    if ((control->scrollStateFlags & 0x800) != 0) {
      GVar13 = (*g_GraphicsTextureSourceGetLogicalSize)(0x72,g_UiWindowTextureSource);
      control->contentOriginX = control->contentOriginX + GVar13.logicalWidthPixels;
      control->contentOriginY = control->contentOriginY + GVar13.logicalHeightPixels;
      UVar11 = UVar11 + GVar13.logicalWidthPixels * -2;
      UVar9 = UVar9 + GVar13.logicalHeightPixels * -2;
    }
    control->viewportWidth = UVar11;
    control->viewportHeight = UVar9;
    iVar12 = UVar11 - control->contentWidth;
    if (iVar12 < 0) {
      control->scrollStateFlags =
           control->scrollStateFlags |
           (UI_SCROLL_HORIZONTAL_BAR_AT_BOTTOM|UI_SCROLL_HORIZONTAL_BAR_AT_TOP);
    }
    iVar10 = UVar9 - control->contentHeight;
    if (iVar10 < 0) {
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
      GVar13 = (*g_GraphicsTextureSourceGetLogicalSize)(0x5e,g_UiWindowTextureSource);
      iVar12 = iVar12 - GVar13.logicalWidthPixels;
      if (iVar12 < 0) {
        control->scrollStateFlags =
             control->scrollStateFlags |
             (UI_SCROLL_HORIZONTAL_BAR_AT_BOTTOM|UI_SCROLL_HORIZONTAL_BAR_AT_TOP);
      }
    }
    if (((control->scrollStateFlags & 0xffffff3f) != 0) &&
       ((control->scrollStateFlags &
        (UI_SCROLL_HORIZONTAL_BAR_AT_BOTTOM|UI_SCROLL_HORIZONTAL_BAR_AT_TOP)) != 0)) {
      GVar13 = (*g_GraphicsTextureSourceGetLogicalSize)(0x5a,g_UiWindowTextureSource);
      if ((int)(iVar10 - GVar13.logicalHeightPixels) < 0) {
        if (((control->scrollStateFlags &
             (UI_SCROLL_HORIZONTAL_BAR_AT_BOTTOM|UI_SCROLL_HORIZONTAL_BAR_AT_TOP)) == 0) &&
           ((control->scrollStateFlags & 0xffffffcf) != 0)) {
          GVar13 = (*g_GraphicsTextureSourceGetLogicalSize)(0x5e,g_UiWindowTextureSource);
          if ((int)(iVar12 - GVar13.logicalWidthPixels) < 0) {
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
      GVar13 = (*g_GraphicsTextureSourceGetLogicalSize)(0x5a,g_UiWindowTextureSource);
      control->viewportHeight = control->viewportHeight - GVar13.logicalHeightPixels;
      if ((control->scrollStateFlags & UI_SCROLL_HORIZONTAL_BAR_AT_TOP) != 0) {
        control->contentOriginY = control->contentOriginY + GVar13.logicalHeightPixels;
      }
    }
    if ((control->scrollStateFlags &
        (UI_SCROLL_VERTICAL_BAR_AT_RIGHT|UI_SCROLL_VERTICAL_BAR_AT_LEFT)) != 0) {
      GVar13 = (*g_GraphicsTextureSourceGetLogicalSize)(0x5e,g_UiWindowTextureSource);
      control->viewportWidth = control->viewportWidth - GVar13.logicalWidthPixels;
      if ((control->scrollStateFlags & UI_SCROLL_VERTICAL_BAR_AT_LEFT) != 0) {
        control->contentOriginX = control->contentOriginX + GVar13.logicalWidthPixels;
      }
    }
    UVar7 = control->scrollOffsetX;
    UVar8 = control->scrollOffsetY;
    pUVar1 = (control->base).firstChild;
    iVar10 = (control->contentWidth - control->viewportWidth) + UVar7;
    iVar12 = (control->contentHeight - control->viewportHeight) + UVar8;
    if (iVar10 < 0) {
      control->scrollOffsetX = control->scrollOffsetX - iVar10;
      pUVar1->left = pUVar1->left - iVar10;
      pUVar1->right = pUVar1->right - iVar10;
      UVar7 = UVar7 - iVar10;
    }
    if (iVar12 < 0) {
      control->scrollOffsetY = control->scrollOffsetY - iVar12;
      pUVar1->top = pUVar1->top - iVar12;
      pUVar1->bottom = pUVar1->bottom - iVar12;
      UVar8 = UVar8 - iVar12;
    }
    if (-1 < (int)UVar7) {
      control->scrollOffsetX = 0;
      pUVar1->left = pUVar1->left - UVar7;
      pUVar1->right = pUVar1->right - UVar7;
    }
    if (-1 < (int)UVar8) {
      control->scrollOffsetY = 0;
      pUVar1->top = pUVar1->top - UVar8;
      pUVar1->bottom = pUVar1->bottom - UVar8;
    }
    UVar7 = control->contentOriginX;
    UVar8 = control->contentOriginY;
    pUVar1->left = pUVar1->left + UVar7;
    pUVar1->top = pUVar1->top + UVar8;
    pUVar1->right = pUVar1->right + UVar7;
    pUVar1->bottom = pUVar1->bottom + UVar8;
    (*pUVar1->vtable->layout)(pUVar1);
    control->horizontalThumbLeft = 0;
    control->verticalThumbTop = 0;
    control->horizontalThumbRight = 0;
    control->verticalThumbBottom = 0;
    if ((control->scrollStateFlags & UI_SCROLL_HORIZONTAL_BAR_AT_TOP) != 0) {
      GVar13 = (*g_GraphicsTextureSourceGetLogicalSize)(0x5a,g_UiWindowTextureSource);
      control->verticalThumbTop = control->verticalThumbTop + GVar13.logicalHeightPixels;
      control->verticalThumbBottom = control->verticalThumbBottom + GVar13.logicalHeightPixels;
    }
    if ((control->scrollStateFlags & UI_SCROLL_VERTICAL_BAR_AT_LEFT) != 0) {
      GVar13 = (*g_GraphicsTextureSourceGetLogicalSize)(0x5e,g_UiWindowTextureSource);
      control->horizontalThumbLeft = control->horizontalThumbLeft + GVar13.logicalWidthPixels;
      control->horizontalThumbRight = control->horizontalThumbRight + GVar13.logicalWidthPixels;
    }
    iVar12 = (control->base).layoutWidth;
    iVar10 = (control->base).layoutHeight;
    if ((control->scrollStateFlags &
        (UI_SCROLL_HORIZONTAL_BAR_AT_BOTTOM|UI_SCROLL_HORIZONTAL_BAR_AT_TOP)) != 0) {
      GVar13 = (*g_GraphicsTextureSourceGetLogicalSize)(0x5a,g_UiWindowTextureSource);
      dVar5 = GVar13.logicalWidthPixels;
      control->horizontalThumbLeft = control->horizontalThumbLeft + dVar5;
      control->horizontalThumbRight = control->horizontalThumbRight + dVar5;
      iVar12 = iVar12 + dVar5 * -2;
      iVar10 = iVar10 - GVar13.logicalHeightPixels;
    }
    if ((control->scrollStateFlags &
        (UI_SCROLL_VERTICAL_BAR_AT_RIGHT|UI_SCROLL_VERTICAL_BAR_AT_LEFT)) != 0) {
      GVar13 = (*g_GraphicsTextureSourceGetLogicalSize)(0x5e,g_UiWindowTextureSource);
      dVar5 = GVar13.logicalHeightPixels;
      control->verticalThumbTop = control->verticalThumbTop + dVar5;
      control->verticalThumbBottom = control->verticalThumbBottom + dVar5;
      iVar10 = iVar10 + dVar5 * -2;
      iVar12 = iVar12 - GVar13.logicalWidthPixels;
    }
    if ((control->scrollStateFlags &
        (UI_SCROLL_HORIZONTAL_BAR_AT_BOTTOM|UI_SCROLL_HORIZONTAL_BAR_AT_TOP)) != 0) {
      uVar6 = (uint)(((longlong)(int)control->viewportWidth * (longlong)iVar12) /
                    (longlong)(int)control->contentWidth);
      GVar13 = (*g_GraphicsTextureSourceGetLogicalSize)(0xc0,g_UiWindowTextureSource);
      uVar4 = GVar13.logicalWidthPixels * 2;
      if (uVar6 < uVar4) {
        uVar6 = uVar4;
      }
      control->horizontalThumbRight = control->horizontalThumbRight + uVar6;
      iVar12 = (int)(((longlong)(int)-control->scrollOffsetX * (longlong)(int)(iVar12 - uVar6)) /
                    (longlong)(int)(control->contentWidth - control->viewportWidth));
      control->horizontalThumbLeft = control->horizontalThumbLeft + iVar12;
      control->horizontalThumbRight = control->horizontalThumbRight + iVar12;
    }
    if ((control->scrollStateFlags &
        (UI_SCROLL_VERTICAL_BAR_AT_RIGHT|UI_SCROLL_VERTICAL_BAR_AT_LEFT)) != 0) {
      uVar6 = (uint)(((longlong)(int)control->viewportHeight * (longlong)iVar10) /
                    (longlong)(int)control->contentHeight);
      GVar13 = (*g_GraphicsTextureSourceGetLogicalSize)(0xc2,g_UiWindowTextureSource);
      uVar4 = GVar13.logicalHeightPixels * 2;
      if (uVar6 < uVar4) {
        uVar6 = uVar4;
      }
      control->verticalThumbBottom = control->verticalThumbBottom + uVar6;
      iVar12 = (int)(((longlong)(int)-control->scrollOffsetY * (longlong)(int)(iVar10 - uVar6)) /
                    (longlong)(int)(control->contentHeight - control->viewportHeight));
      control->verticalThumbTop = control->verticalThumbTop + iVar12;
      control->verticalThumbBottom = control->verticalThumbBottom + iVar12;
    }
  }
  return;
}


/* Address: 0x004B8AC0.
   Ownership: ui/controls/lists.
   Purpose: Binary entry is anchored by g_UiNodeVtable_004B7920[10]@004B7920.
*/
GraphicsCursorFrameIndex __thandor_eax_preserve_ecx_edx
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
void __thandor_void_preserve_ecx_edx
UiScrollableControl_BeginSecondaryScrollInteraction
          (UiPointerWheelDelta wheelDelta,UiPixelCoordinate pointerY,UiPixelCoordinate pointerX,
          UiScrollableControl *control)

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
void UiScrollableControl_EndSecondaryScrollInteraction
               (UiPointerWheelDelta wheelDelta,UiPixelCoordinate pointerY,UiPixelCoordinate pointerX
               ,UiNodeBase *control)

{
  g_CursorUseOverridePosition = 0;
  control[1].nextSibling = (UiNodeBase *)((uint)control[1].nextSibling & 0xffffafff);
  (*g_GraphicsCursorSetFrame)(0);
  return;
}


/* Address: 0x004B9000.
   Ownership: ui/controls/lists.
   Purpose: Binary entry is anchored by g_UiNodeVtable_004B7920[11]@004B7920.
   Cross-module calls: UiContainer_HitTestChildren [ui/controls/layout].
*/
UiNodeBase * __thandor_eax_preserve_ecx_edx
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
void ** UiPointerList_GetRowSlotsVariantA(UiPointerListControl *control)

{
  return control->rowSlots;
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
void __thandor_void_preserve_eax_ecx_edx
UiListControl_DrawRowsAndSelection
          (int clipTop,int clipLeft,int clipBottom,int clipRight,UiNodeBase *control)

{
  int iVar1;
  int iVar2;
  UiNodeBase *pUVar3;
  int iVar4;
  UiNodeBase **ppUVar5;
  int iVar6;
  UiNodeBase *pUVar7;
  UiNodeBase **ppUVar8;
  word *commandStream;
  bool bVar9;
  RichTextExtentRegs RVar10;
  GraphicsTextureSizeEaxEdxCf9 GVar11;
  
  if (control[1].parent != (UiNodeBase *)0x0) {
    iVar2 = (clipBottom - control->top) / (int)control[1].vtable;
    if (iVar2 < 0) {
      iVar2 = 0;
    }
    ppUVar8 = &(control[1].firstChild)->nextSibling + iVar2;
    pUVar3 = (UiNodeBase *)
             (((clipTop - control->top) + (int)control[1].vtable) / (int)control[1].vtable);
    iVar2 = iVar2 * (int)control[1].vtable;
    if (control[1].parent <= pUVar3) {
      pUVar3 = (UiNodeBase *)((int)&control[1].parent[-1].nodeFlags + 3);
    }
    ppUVar5 = &(control[1].firstChild)->nextSibling + (int)pUVar3;
    if (ppUVar8 <= ppUVar5) {
      bVar9 = (*g_GraphicsFramebufferBeginAccess)();
      if (!bVar9) {
        do {
          if (ppUVar8 == (UiNodeBase **)control[1].top) {
            iVar6 = control->layoutWidth;
            if ((control->nodeFlags & UI_NODE_HAS_KEYBOARD_FOCUS) == 0) {
              UiWindow_BlitTiledHorizontalEdge
                        (clipTop,clipLeft,clipBottom,clipRight,0x82,iVar6,iVar2,0,control);
            }
            else {
              GVar11 = (*g_GraphicsTextureSourceGetLogicalSize)(0x83,g_UiWindowTextureSource);
              iVar6 = iVar6 - GVar11.logicalWidthPixels;
              UiWindow_BlitTiledHorizontalEdge
                        (clipTop,clipLeft,clipBottom,clipRight,0x84,iVar6,iVar2,
                         GVar11.logicalWidthPixels,control);
              (*g_GraphicsTextureSourceBlitSourceAlpha)
                        (clipTop,clipLeft,clipBottom,clipRight,iVar2 + control->top,control->left,
                         0x83,g_UiWindowTextureSource,g_FramebufferAccess);
              (*g_GraphicsTextureSourceBlitSourceAlpha)
                        (clipTop,clipLeft,clipBottom,clipRight,iVar2 + control->top,
                         iVar6 + control->left,0x85,g_UiWindowTextureSource,g_FramebufferAccess);
            }
          }
          iVar6 = control[1].right;
          pUVar3 = *ppUVar8;
          if (iVar6 != 0) {
            iVar4 = 3;
            pUVar7 = control;
            do {
              iVar1 = pUVar7[1].leftOffset;
              if (iVar1 < 0) {
                iVar4 = iVar4 - iVar1;
                commandStream = (word *)((int)&pUVar3->nextSibling + pUVar7[1].topOffset);
                RVar10 = RichTextCommandStream_MeasureRegs(g_UiListTextStyle,commandStream);
                RichTextCommandStream_DrawSingleLine
                          (clipTop,clipLeft,clipBottom,clipRight,g_UiListTextStyle,commandStream,
                           iVar2 + 1 + control->top,
                           (iVar4 - (RVar10.widthPixels + 6)) + control->left);
              }
              else {
                iVar4 = iVar4 + iVar1;
                RichTextCommandStream_DrawSingleLine
                          (clipTop,clipLeft,clipBottom,clipRight,g_UiListTextStyle,
                           (word *)((int)&pUVar3->nextSibling + pUVar7[1].topOffset),
                           iVar2 + 1 + control->top,(iVar4 - iVar1) + control->left);
              }
              pUVar7 = (UiNodeBase *)&pUVar7->parent;
              iVar6 = iVar6 + -1;
            } while (iVar6 != 0);
          }
          ppUVar8 = ppUVar8 + 1;
          iVar2 = (int)&(control[1].vtable)->relocate + iVar2;
        } while (ppUVar8 <= ppUVar5);
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
void __thandor_preserve_eax UiListControl_TickActivationPulse(UiListControl *control)

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
void __thandor_void_preserve_eax_ecx_edx
UiListControl_UnsuppressIfActionId(UiActionId actionId,UiListControl *control)

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
void __thandor_void_preserve_eax_ecx_edx
UiListControl_SuppressIfActionId(UiActionId actionId,UiListControl *control)

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
void __thandor_void_preserve_eax_ecx_edx
UiPointerList_InitializeColumnLayout
          (UiListRowCount rowCount,void **rowPointers,UiPointerListControl *control)

{
  UiNodeBase *pUVar1;
  UiNodeVtable *pUVar2;
  UiNodeBase *pUVar3;
  UiPixelExtent UVar4;
  int iVar5;
  UiPointerListControl *pUVar6;
  FontGlyphSizeEaxEdxCf9 FVar7;
  
  FVar7 = FontGlyph_GetLogicalSizeForStyleRegs(g_UiListTextStyle,0);
  UVar4 = FVar7.lineHeight + 1;
  control->rowHeight = UVar4;
  control->rowCount = rowCount;
  control->rowSlots = rowPointers;
  control->selectedRowSlot = rowPointers;
  iVar5 = 6;
  pUVar3 = control[1].base.nextSibling;
  (control->base).bottomOffset = UVar4 * rowCount + 1;
  pUVar6 = control;
  for (; pUVar3 != (UiNodeBase *)0x0; pUVar3 = (UiNodeBase *)((int)&pUVar3[-1].nodeFlags + 3)) {
    pUVar1 = pUVar6[1].base.parent;
    if ((int)pUVar1 < 0) {
      pUVar1 = (UiNodeBase *)-(int)pUVar1;
    }
    iVar5 = (int)&pUVar1->nextSibling + iVar5;
    pUVar6 = (UiPointerListControl *)&(pUVar6->base).parent;
  }
  pUVar3 = (control->base).parent;
  pUVar2 = pUVar3->vtable;
  (control->base).rightOffset = iVar5;
  (control->base).leftOffset = 0;
  (control->base).topOffset = 0;
  (*pUVar2->layout)(pUVar3);
  return;
}


/* Address: 0x004BB460.
   Ownership: ui/controls/lists.
   Purpose: EXACT_DUPLICATE_UI_POINTER_LIST_ROW_SLOT_GETTER_VARIANT_B.
*/
void ** UiPointerList_GetRowSlotsVariantB(UiPointerListControl *control)

{
  return control->rowSlots;
}


/* Address: 0x004BB9E0.
   Ownership: ui/controls/lists.
   Purpose: Binary entry is anchored by g_UiNodeVtable_004BB990[0]@004BB990.
   Cross-module calls: UiContainer_RelocateChildren [ui/controls/layout].
*/
void __thandor_void_preserve_eax_ecx_edx
UiTimedListControl_RelocateChildren
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
void __thandor_void_preserve_eax_ecx_edx
UiTimedListControl_DrawRowsAndSelection
          (int clipTop,int clipLeft,int clipBottom,int clipRight,UiNodeBase *control)

{
  dword dVar1;
  int iVar2;
  int iVar3;
  int iVar4;
  sdword *unaff_EBP;
  UiNodeBase *pUVar5;
  UiNodeBase *pUVar6;
  sdword *psVar7;
  sdword *psVar8;
  bool bVar9;
  RichTextExtentRegs RVar10;
  GraphicsTextureSizeEaxEdxCf9 GVar11;
  UiNodeBase *pUStack_20;
  
  bVar9 = (*g_GraphicsFramebufferBeginAccess)();
  if (!bVar9) {
    pUStack_20 = (UiNodeBase *)0x0;
    pUVar5 = control[1].firstChild;
    iVar3 = 0;
    if (pUVar5 != (UiNodeBase *)0x0) {
      pUVar6 = pUVar5->nextSibling;
      psVar7 = &pUVar5->left;
      if (pUVar6 != (UiNodeBase *)0x0) {
        do {
          iVar4 = control->left;
          iVar3 = iVar3 + control->top;
          pUVar5 = pUStack_20;
          if (pUStack_20 != (UiNodeBase *)0x0) {
            while (pUVar5 = (UiNodeBase *)((int)&pUVar5[-1].nodeFlags + 3),
                  pUVar5 != (UiNodeBase *)0x0) {
              if (*(int *)(&stack0xffffffd8 + (int)pUVar5 * 8) != 0) {
                (*g_GraphicsTextureSourceBlitSourceAlpha)
                          (clipTop,clipLeft,clipBottom,clipRight,iVar3,iVar4,control[1].rightOffset,
                           (GraphicsTextureSourceAsset *)control[1].bottom,g_FramebufferAccess);
              }
              iVar4 = iVar4 + control[1].topAnchorQ31;
            }
            if (pUVar6 < (UiNodeBase *)0x2) {
              dVar1 = control[1].leftAnchorQ31;
            }
            else {
              dVar1 = control[1].bottomOffset;
            }
            (*g_GraphicsTextureSourceBlitSourceAlpha)
                      (clipTop,clipLeft,clipBottom,clipRight,iVar3,iVar4,dVar1,
                       (GraphicsTextureSourceAsset *)control[1].bottom,g_FramebufferAccess);
            if ((psVar7[3] & 1U) != 0) {
              if ((psVar7[3] & 2U) == 0) {
                dVar1 = control[1].leftOffset;
              }
              else {
                dVar1 = control[1].topOffset;
              }
              (*g_GraphicsTextureSourceBlitSourceAlpha)
                        (clipTop,clipLeft,clipBottom,clipRight,iVar3,iVar4,dVar1,
                         (GraphicsTextureSourceAsset *)control[1].bottom,g_FramebufferAccess);
            }
            iVar4 = iVar4 + control[1].topAnchorQ31;
          }
          (*g_GraphicsTextureSourceBlitSourceAlpha)
                    (clipTop,clipLeft,clipBottom,clipRight,iVar3,iVar4,psVar7[1],
                     (GraphicsTextureSourceAsset *)control[1].bottom,g_FramebufferAccess);
          iVar3 = iVar3 - control->top;
          iVar4 = (iVar4 + control[1].rightAnchorQ31) - control->left;
          if ((uint *)psVar7 == (uint *)control[1].top) {
            RVar10 = RichTextCommandStream_MeasureRegs(g_UiListTextStyle,(word *)*psVar7);
            iVar2 = RVar10.widthPixels + 6;
            if ((control->nodeFlags & UI_NODE_HAS_KEYBOARD_FOCUS) == 0) {
              UiWindow_BlitTiledHorizontalEdge
                        (clipTop,clipLeft,clipBottom,clipRight,0x82,iVar2 + iVar4,iVar3,iVar4,
                         control);
            }
            else {
              GVar11 = (*g_GraphicsTextureSourceGetLogicalSize)(0x83,g_UiWindowTextureSource);
              iVar2 = (iVar2 - GVar11.logicalWidthPixels) + iVar4;
              UiWindow_BlitTiledHorizontalEdge
                        (clipTop,clipLeft,clipBottom,clipRight,0x84,iVar2,iVar3,
                         GVar11.logicalWidthPixels + iVar4,control);
              (*g_GraphicsTextureSourceBlitSourceAlpha)
                        (clipTop,clipLeft,clipBottom,clipRight,iVar3 + control->top,
                         iVar4 + control->left,0x83,g_UiWindowTextureSource,g_FramebufferAccess);
              (*g_GraphicsTextureSourceBlitSourceAlpha)
                        (clipTop,clipLeft,clipBottom,clipRight,iVar3 + control->top,
                         iVar2 + control->left,0x85,g_UiWindowTextureSource,g_FramebufferAccess);
            }
          }
          RichTextCommandStream_DrawSingleLine
                    (clipTop,clipLeft,clipBottom,clipRight,g_UiListTextStyle,(word *)*psVar7,
                     iVar3 + 1 + control->top,iVar4 + 3 + control->left);
          psVar8 = psVar7 + 4;
          iVar3 = iVar3 + (int)control[1].vtable;
          pUVar5 = (UiNodeBase *)((int)&pUVar6[-1].nodeFlags + 3);
          pUVar6 = pUVar5;
          if ((((psVar7[3] & 1U) != 0) && ((psVar7[3] & 2U) != 0)) && (psVar7[2] != 0)) {
            pUVar6 = *(UiNodeBase **)psVar7[2];
            psVar8 = (sdword *)((uint *)psVar7[2] + 4);
            pUStack_20 = pUVar5;
          }
          while (psVar7 = psVar8, pUVar6 == (UiNodeBase *)0x0) {
            if (pUStack_20 == (UiNodeBase *)0x0) goto UiTimedListDraw_EndFramebufferAccess;
            pUVar6 = (UiNodeBase *)((int)&pUStack_20[-1].nodeFlags + 3);
            psVar8 = unaff_EBP;
            pUStack_20 = pUVar6;
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
void __thandor_preserve_eax UiTimedListControl_TickActionDelay(UiTimedListControl *control)

{
  if (((control->listStateAndDelay & UI_TIMED_LIST_ACTION_DELAY_PENDING) != 0) &&
     (control->listStateAndDelay = control->listStateAndDelay - 0x1000000,
     (control->listStateAndDelay & 0xff000000) == 0)) {
    control->listStateAndDelay = control->listStateAndDelay & 0xfffffd;
    UiActionQueue_Enqueue(control->actionId,control);
  }
  return;
}


/* Address: 0x004BC3F0.
   Ownership: ui/controls/lists.
   Purpose: Returns the selected timed-list record.
*/
UiTimedListTreeRecord16 *
UiTimedListControl_GetSelectedRecord(UiTimedListRuntimeExtendedView88 *control)

{
  return (control->base).selectedRecord;
}

/* Address: 0x004BC460.
   Ownership: ui/controls/lists.
   Purpose: Binary entry is anchored by g_UiNodeVtable_004BC410[0]@004BC410; g_UiNodeVtable_00517F10[0]@00517F10.
   Cross-module calls: UiContainer_RelocateChildren [ui/controls/layout].
*/
void __thandor_void_preserve_eax_ecx_edx
UiListOffsetControl_RelocateAndApplyDeferredOffset
          (UiSerializedRelocationDelta relocationDelta,UiNodeBase *control)

{
  UiContainer_RelocateChildren(relocationDelta,control);
  if (((uint)control[1].nextSibling & 0x20) != 0) {
    control[1].parent = (UiNodeBase *)((int)&(control[1].parent)->nextSibling + relocationDelta);
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
void __thandor_void_preserve_eax_ecx_edx
UiCatalogEntryControl_DrawClipped
          (UiPixelCoordinate clipTop,UiPixelCoordinate clipLeft,UiPixelCoordinate clipBottom,
          UiPixelCoordinate clipRight,UiCatalogEntryControl *control)

{
  ArmyRuntimeSlot *pAVar1;
  dword dVar2;
  int iVar3;
  int iVar4;
  FactionArmyAssetCount FVar5;
  int iVar6;
  bool bVar7;
  RichTextExtentRegs RVar8;
  dword arg6;
  GraphicsTextureSourceAsset *arg7;
  SoftwareFramebufferAccess *pSVar9;
  PckArmyAssetIdCatalog PVar10;
  UiPackedTextStyle UStack_20;
  ModelRuntimeNode *modelNode;
  ArmyRuntimeSlot *armyRuntime;
  ModelRuntimeNode *modelNodePrimary;
  
  if ((((control->command).sprite.selectable.base.nodeFlags & UI_NODE_SUPPRESSED) != 0) ||
     (((((control->command).sprite.selectable.stateFlags & UI_SELECTABLE_SELECTED_OR_CHECKED) == 0
       && (((control->command).sprite.selectable.stateFlags & 0x400) != 0)) ||
      (bVar7 = (*g_GraphicsFramebufferBeginAccess)(), bVar7)))) {
    return;
  }
  arg7 = (control->command).sprite.primaryTextureSource;
  pSVar9 = g_FramebufferAccess;
  if (((control->command).sprite.selectable.stateFlags & UI_SELECTABLE_SELECTED_OR_CHECKED) == 0) {
    dVar2 = (control->command).sprite.normalSubresourceStartOrDescriptor;
  }
  else {
    if ((((control->command).sprite.selectable.stateFlags & 0x80) == 0) &&
       (((control->command).sprite.selectable.stateFlags & 0x800) != 0)) {
      arg7 = (control->command).sprite.alternateTextureSource;
    }
    dVar2 = (control->command).sprite.selectedSubresourceStart;
    if (((control->command).sprite.selectable.stateFlags & 0x40) != 0) {
      arg6 = (control->command).sprite.normalSubresourceStartOrDescriptor;
      if (((control->command).sprite.selectable.stateFlags & 0x80) != 0) {
        arg6 = arg6 + (control->command).sprite.animationFrameOffset;
      }
      (*g_GraphicsTextureSourceBlitSourceAlpha)
                (clipTop,clipLeft,clipBottom,clipRight,(control->command).sprite.selectable.base.top
                 ,(control->command).sprite.selectable.base.left,arg6,
                 (control->command).sprite.primaryTextureSource,g_FramebufferAccess);
    }
  }
  if (((control->command).sprite.selectable.stateFlags & 0x80) != 0) {
    dVar2 = dVar2 + (control->command).sprite.animationFrameOffset;
  }
  (*g_GraphicsTextureSourceBlitSourceAlpha)
            (clipTop,clipLeft,clipBottom,clipRight,(control->command).sprite.selectable.base.top,
             (control->command).sprite.selectable.base.left,dVar2,arg7,pSVar9);
  iVar6 = (g_InGameRuntimeRoot->worldRuntime0A30).activeFactionRuntimeIndex;
  if ((int)g_GameFactionRuntimeImage.records[iVar6].xeniteCurrentQ4 <
      (int)control->runtimeDisplayValueQ4) {
    UStack_20 = 0x1050000;
  }
  else {
    UStack_20 = 0x1040000;
  }
  g_UiCatalogEntryRichTextScratchUtf16[0] = 0x20;
  g_UiCatalogEntryRichTextScratchUtf16[1] = 0;
  dVar2 = (*g_WideNumberFormatUtf16)
                    (WIDE_FORMAT_WRITE_TERMINATOR,0,10,1,control->runtimeDisplayValueQ4 >> 4,
                     g_UiCatalogEntryRichTextScratchUtf16 + 1);
  *(undefined4 *)((int)g_UiCatalogEntryRichTextScratchUtf16 + dVar2 + 2) = 0x20;
  RVar8 = RichTextCommandStream_MeasureRegs(0x1000000,g_UiCatalogEntryRichTextScratchUtf16);
  RichTextCommandStream_DrawSingleLine
            (clipTop,clipLeft,clipBottom,clipRight,UStack_20,g_UiCatalogEntryRichTextScratchUtf16,
             ((control->command).sprite.selectable.base.bottom - RVar8.heightPixels) + -2,
             ((int)((control->command).sprite.selectable.base.layoutWidth - RVar8.widthPixels) >> 1)
             + (control->command).sprite.selectable.base.left);
  iVar3 = 0x29;
  do {
    if ((int)control - (int)g_InGameRuntimeRoot ==
        g_UiCatalogGroup42OffsetTables[g_UiCatalogGroup42ColumnCount][iVar3]) {
      iVar4 = 0;
      PVar10 = g_UiCatalogGroup42Records[iVar3]->armyAssetId;
      for (FVar5 = g_GameFactionRuntimeImage.records[iVar6].secondaryArmyAssetCount; FVar5 != 0;
          FVar5 = FVar5 - 1) {
        if (g_UiCatalogGroup42Records[iVar3] ==
            *(UiCommandRuntimeRecordPrefix **)(iVar6 * 0x740 + 0x50f41c + FVar5 * 4)) {
          iVar4 = iVar4 + 1;
        }
      }
      if (iVar4 != 0) {
        g_UiCatalogEntryRichTextScratchUtf16[0] = 0x20;
        dVar2 = (*g_WideNumberFormatUtf16)
                          (WIDE_FORMAT_WRITE_TERMINATOR,0,10,1,iVar4,
                           g_UiCatalogEntryRichTextScratchUtf16 + 1);
        iVar6 = (control->command).sprite.selectable.base.top;
        *(undefined4 *)((int)g_UiCatalogEntryRichTextScratchUtf16 + dVar2 + 2) = 0x20;
        RichTextCommandStream_DrawSingleLine
                  (clipTop,clipLeft,clipBottom,clipRight,UStack_20,
                   g_UiCatalogEntryRichTextScratchUtf16,iVar6 + 2,
                   (control->command).sprite.selectable.base.left);
      }
      iVar6 = -1;
      for (modelNodePrimary =
                (ModelRuntimeNode *)(g_InGameRuntimeRoot->worldRuntime0A30).ownerListHead;
          modelNodePrimary != (ModelRuntimeNode *)0x0;
          modelNodePrimary = (ModelRuntimeNode *)(modelNodePrimary->common).nextNode) {
        if (((modelNodePrimary->ownerClassId == MODEL_RUNTIME_CLASS_00) &&
            (armyRuntime = (modelNodePrimary->runtimePayload).armyRuntime,
            ((armyRuntime->modelRuntimeOrSavedOffset).modelRuntime)->definitionValue9C_4C == 0xb))
           && (((armyRuntime->articulatedContact).fallbackPosition0Q12 == 1 &&
               (((((g_InGameRuntimeRoot->worldRuntime0A30).activeFactionRuntimeIndex ==
                   (armyRuntime->linkedEntityRuntime->common).ownership.ownerIndex &&
                  (PVar10 == armyRuntime->classState60)) &&
                 (iVar3 = (int)(((longlong)(int)armyRuntime->ownerValue64 * 100) /
                               (longlong)(int)armyRuntime->ownerValue68), iVar6 <= iVar3)) &&
                (UStack_20 = 0x1040000, iVar6 = iVar3, (armyRuntime->runtimeFlags & 1) != 0)))))) {
          UStack_20 = 0x1050000;
        }
      }
      if (-1 < iVar6) {
        g_UiCatalogEntryRichTextScratchUtf16[0] = 0x20;
        dVar2 = (*g_WideNumberFormatUtf16)
                          (WIDE_FORMAT_WRITE_TERMINATOR,0,10,1,iVar6,
                           g_UiCatalogEntryRichTextScratchUtf16 + 1);
        *(undefined2 *)((int)g_UiCatalogEntryRichTextScratchUtf16 + dVar2 + 2) = 0x25;
        *(undefined4 *)((int)g_UiCatalogEntryRichTextScratchUtf16 + dVar2 + 4) = 0x20;
        RVar8 = RichTextCommandStream_MeasureRegs(0x1000000,g_UiCatalogEntryRichTextScratchUtf16);
        RichTextCommandStream_DrawSingleLine
                  (clipTop,clipLeft,clipBottom,clipRight,UStack_20,
                   g_UiCatalogEntryRichTextScratchUtf16,
                   (control->command).sprite.selectable.base.top + 2,
                   (control->command).sprite.selectable.base.right - RVar8.widthPixels);
      }
      goto UiCatalogEntryControl_DrawClipped_EndFramebufferAccessAndReturn;
    }
    iVar3 = iVar3 + -1;
  } while (-1 < iVar3);
  iVar3 = 0x2f;
  while ((int)control - (int)g_InGameRuntimeRoot !=
         g_UiCatalogGroup48OffsetTables[g_UiCatalogGroup48ColumnCount][iVar3]) {
    iVar3 = iVar3 + -1;
    if (iVar3 < 0) goto UiCatalogEntryControl_DrawClipped_EndFramebufferAccessAndReturn;
  }
  iVar4 = 0;
  PVar10 = g_UiCatalogGroup48Records[iVar3]->armyAssetId;
  for (FVar5 = g_GameFactionRuntimeImage.records[iVar6].secondaryArmyAssetCount; FVar5 != 0;
      FVar5 = FVar5 - 1) {
    if (g_UiCatalogGroup48Records[iVar3] ==
        *(UiCommandRuntimeRecordPrefix **)(iVar6 * 0x740 + 0x50f41c + FVar5 * 4)) {
      iVar4 = iVar4 + 1;
    }
  }
  if (iVar4 != 0) {
    g_UiCatalogEntryRichTextScratchUtf16[0] = 0x20;
    dVar2 = (*g_WideNumberFormatUtf16)
                      (WIDE_FORMAT_WRITE_TERMINATOR,0,10,1,iVar4,
                       g_UiCatalogEntryRichTextScratchUtf16 + 1);
    iVar6 = (control->command).sprite.selectable.base.top;
    *(undefined4 *)((int)g_UiCatalogEntryRichTextScratchUtf16 + dVar2 + 2) = 0x20;
    RichTextCommandStream_DrawSingleLine
              (clipTop,clipLeft,clipBottom,clipRight,UStack_20,g_UiCatalogEntryRichTextScratchUtf16,
               iVar6 + 2,(control->command).sprite.selectable.base.left);
  }
  iVar6 = (g_InGameRuntimeRoot->worldRuntime0A30).activeFactionRuntimeIndex;
  iVar3 = -1;
  for (modelNode = (ModelRuntimeNode *)(g_InGameRuntimeRoot->worldRuntime0A30).ownerListHead;
      modelNode != (ModelRuntimeNode *)0x0;
      modelNode = (ModelRuntimeNode *)(modelNode->common).nextNode) {
    if (modelNode->ownerClassId == MODEL_RUNTIME_CLASS_00) {
      pAVar1 = (modelNode->runtimePayload).armyRuntime;
      if (((pAVar1->modelRuntimeOrSavedOffset).modelRuntime)->definitionValue9C_4C == 0x16) {
        if ((((pAVar1->articulatedContact).terrainContactMode ==
              ARMY_TERRAIN_CONTACT_ADVANCE_ACTIVE_CONTACT_AND_RELEASE) &&
            (iVar6 == (pAVar1->linkedEntityRuntime->common).ownership.ownerIndex)) &&
           ((PVar10 == pAVar1->classState60 &&
            ((iVar4 = (int)(((longlong)(int)pAVar1->ownerValue64 * 100) /
                           (longlong)(int)pAVar1->ownerValue68), iVar3 <= iVar4 &&
             (UStack_20 = 0x1040000, iVar3 = iVar4, (pAVar1->runtimeFlags & 1) != 0)))))) {
          UStack_20 = 0x1050000;
        }
      }
      else if ((((((pAVar1->modelRuntimeOrSavedOffset).modelRuntime)->definitionValue9C_4C == 0xd)
                && ((pAVar1->articulatedContact).fallbackPosition0Q12 == 1)) &&
               (iVar6 == (pAVar1->linkedEntityRuntime->common).ownership.ownerIndex)) &&
              (((PVar10 == pAVar1->classState60 &&
                (iVar4 = (int)(((longlong)(int)pAVar1->ownerValue64 * 100) /
                              (longlong)(int)pAVar1->ownerValue68), iVar3 <= iVar4)) &&
               (UStack_20 = 0x1040000, iVar3 = iVar4, (pAVar1->runtimeFlags & 1) != 0)))) {
        UStack_20 = 0x1050000;
      }
    }
  }
  if (-1 < iVar3) {
    g_UiCatalogEntryRichTextScratchUtf16[0] = 0x20;
    dVar2 = (*g_WideNumberFormatUtf16)
                      (WIDE_FORMAT_WRITE_TERMINATOR,0,10,1,iVar3,
                       g_UiCatalogEntryRichTextScratchUtf16 + 1);
    *(undefined2 *)((int)g_UiCatalogEntryRichTextScratchUtf16 + dVar2 + 2) = 0x25;
    *(undefined4 *)((int)g_UiCatalogEntryRichTextScratchUtf16 + dVar2 + 4) = 0x20;
    RVar8 = RichTextCommandStream_MeasureRegs(0x1000000,g_UiCatalogEntryRichTextScratchUtf16);
    RichTextCommandStream_DrawSingleLine
              (clipTop,clipLeft,clipBottom,clipRight,UStack_20,g_UiCatalogEntryRichTextScratchUtf16,
               (control->command).sprite.selectable.base.top + 2,
               (control->command).sprite.selectable.base.right - RVar8.widthPixels);
  }
UiCatalogEntryControl_DrawClipped_EndFramebufferAccessAndReturn:
  (*g_GraphicsFramebufferEndAccess)();
  return;
}


/* Address: 0x00516B90.
   Ownership: ui/controls/lists.
   Purpose: Maps a hovered catalog control to one of two verified static catalog tables, updates the active catalog
   selection, and returns the pointer cursor identifier.
   Cross-module calls: InGameSelectionDetailPanel_Rebuild [ui/ingame/runtime].
*/
GraphicsCursorFrameIndex __thandor_eax_preserve_ecx_edx
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
void __thandor_void_preserve_eax_ecx_edx
UiCatalogEntryControl_NonRightRelease
          (UiPointerWheelDelta wheelDelta,UiPixelCoordinate pointerY,UiPixelCoordinate pointerX,
          UiCatalogEntryControl *control)

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
void __thandor_void_preserve_eax_ecx_edx
UiTimedListControl_SelectRecordAndScrollIntoView
          (UiTimedListTreeRecord16 *selectedRecord,UiTimedListRuntimeExtendedView88 *control)

{
  dword dVar1;
  int iVar2;
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
      iVar3 = iVar2 + dVar1;
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
void __thandor_void_preserve_eax_ecx_edx
UiPointerList_SelectIndexVariantB(UiListRowIndex index,UiPointerListControl *control)

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


/* Address: 0x004BB540.
   Ownership: ui/controls/lists.
   Purpose: Returns (+0x60 - +0x50)/4 in EAX. CF mirrors control flag 0x04 exactly.
*/
UiListRowIndexEaxCf5 __thandor_eax_cf_preserve_ecx_edx
UiPointerList_GetSelectedIndexVariantBCf(UiPointerListControl *control)

{
  UiListRowIndex selectedRowIndex;
  UiListRowIndexEaxCf5 UVar1;
  UiListRowIndexEaxCf5 UVar2;
  
  selectedRowIndex = (int)control->selectedRowSlot - (int)control->rowSlots >> 2;
  if ((control->listStateFlags & UI_LIST_SELECTION_CONFIRMED) == 0) {
    UVar1.carry = false;
    UVar1.rowIndex = selectedRowIndex;
    return UVar1;
  }
  UVar2.carry = true;
  UVar2.rowIndex = selectedRowIndex;
  return UVar2;
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

/* Address: 0x004BC1C0.
   Ownership: ui/controls/lists.
   Purpose: Sets the timed-list record tree and recomputes layout.
*/
void __thandor_void_preserve_eax_ecx_edx
UiTimedListControl_SetRecordTreeAndRecomputeLayout
          (UiTimedListTreeRecord16 *recordTree,UiTimedListRuntimeExtendedView88 *control)

{
  UiListRowCount *pUVar1;
  UiNodeBase *control_00;
  UiListRowCount UVar2;
  UiNodeVtable *pUVar3;
  UiPixelExtent UVar4;
  uint uVar5;
  dword dVar6;
  dword dVar7;
  UiTimedListTreeRecord16 *unaff_EBP;
  uint uVar8;
  UiTimedListTreeRecord16 *pUVar9;
  UiTimedListTreeRecord16 *pUVar10;
  RichTextExtentRegs RVar11;
  FontGlyphSizeEaxEdxCf9 FVar12;
  dword dStack_20;
  
  FVar12 = FontGlyph_GetLogicalSizeActiveRegs(0);
  if (recordTree == (UiTimedListTreeRecord16 *)0x0) {
    dVar6 = 0;
  }
  else {
    dVar6 = recordTree->recordCountOrRowPayload00;
  }
  (control->base).rowHeight = FVar12.lineHeight + 1;
  (control->base).rowCount = dVar6;
  (control->base).recordTree = recordTree;
  (control->base).selectedRecord = recordTree + 1;
  uVar8 = 0;
  if (dVar6 != 0) {
    dStack_20 = 0;
    pUVar9 = recordTree + 1;
    do {
      RVar11 = RichTextCommandStream_MeasureRegs
                         (g_UiListTextStyle,(word *)pUVar9->recordCountOrRowPayload00);
      uVar5 = RVar11.widthPixels + control->observedDrawParameter84 +
              control->observedDrawParameter80 * dStack_20;
      pUVar10 = pUVar9 + 1;
      dVar7 = dVar6 - 1;
      if (uVar8 < uVar5) {
        uVar8 = uVar5;
      }
      dVar6 = dVar7;
      if ((((pUVar9->recordFlags0C & UI_TIMED_LIST_RECORD_OBSERVED_BIT0) != 0) &&
          ((pUVar9->recordFlags0C & UI_TIMED_LIST_RECORD_ENABLES_NESTED_CHILD_TRAVERSAL) != 0)) &&
         (pUVar9->nestedRecordBlockOrParentLink08 != (UiTimedListTreeRecord16 *)0x0)) {
        pUVar9 = pUVar9->nestedRecordBlockOrParentLink08;
        dVar6 = pUVar9->recordCountOrRowPayload00;
        pUVar1 = &(control->base).rowCount;
        *pUVar1 = *pUVar1 + dVar6;
        pUVar10 = pUVar9 + 1;
        dStack_20 = dVar7;
      }
      while (pUVar9 = pUVar10, dVar6 == 0) {
        if (dStack_20 == 0) goto LAB_004bc299;
        dVar6 = dStack_20 - 1;
        pUVar10 = unaff_EBP;
        dStack_20 = dVar6;
      }
    } while( true );
  }
LAB_004bc299:
  control_00 = (control->base).base.parent;
  UVar2 = (control->base).rowCount;
  pUVar3 = control_00->vtable;
  UVar4 = (control->base).rowHeight;
  (control->base).base.rightOffset = uVar8 + 6;
  (control->base).base.leftOffset = 0;
  (control->base).base.topOffset = 0;
  (control->base).base.bottomOffset = UVar2 * UVar4 + 1;
  (*pUVar3->layout)(control_00);
  return;
}

/* Address: 0x004BC2F0.
   Ownership: ui/controls/lists.
   Purpose: Returns the timed-list record tree.
*/
UiTimedListTreeRecord16 * UiTimedListControl_GetRecordTree(UiTimedListControl *control)

{
  return control->recordTree;
}

/* Address: 0x004BC310.
   Ownership: ui/controls/lists.
   Purpose: Handles ui timed list tree count record array and nested children.
*/
dword __thandor_eax_preserve_ecx_edx
UiTimedListTree_CountRecordArrayAndNestedChildren(UiTimedListTreeRecord16 *recordBlock)

{
  dword dVar1;
  dword dVar2;
  dword dVar3;
  
  dVar2 = 0;
  if (recordBlock != (UiTimedListTreeRecord16 *)0x0) {
    dVar2 = recordBlock->recordCountOrRowPayload00;
    dVar3 = dVar2;
    do {
      if ((recordBlock[1].recordFlags0C & UI_TIMED_LIST_RECORD_ENABLES_NESTED_CHILD_TRAVERSAL) != 0)
      {
        dVar1 = UiTimedListTree_CountRecordArrayAndNestedChildren
                          (recordBlock[1].nestedRecordBlockOrParentLink08);
        dVar2 = dVar2 + dVar1;
      }
      dVar3 = dVar3 - 1;
      recordBlock = recordBlock + 1;
    } while (dVar3 != 0);
  }
  return dVar2;
}


/* Address: 0x004B9170.
   Ownership: ui/controls/lists.
   Purpose: Refreshes the active child rectangle from the current scroll offsets and recomputes the enabled
   scrollbar tracks and thumb positions without repeating the complete viewport-selection pass.
*/
void __thandor_void_preserve_eax_ecx_edx
UiScrollableControl_RefreshChildAndScrollThumbs(UiScrollableControl *control)

{
  UiNodeBase *pUVar1;
  UiPixelExtent UVar2;
  UiPixelExtent UVar3;
  uint uVar4;
  dword dVar5;
  uint uVar6;
  UiPixelOffset UVar7;
  UiPixelOffset UVar8;
  int iVar9;
  int iVar10;
  GraphicsTextureSizeEaxEdxCf9 GVar11;
  
  pUVar1 = (control->base).firstChild;
  if (pUVar1 != (UiNodeBase *)0xffffffff) {
    UVar7 = control->scrollOffsetY;
    iVar9 = (control->base).top;
    pUVar1->left = pUVar1->leftOffset + control->scrollOffsetX + (control->base).left;
    pUVar1->top = pUVar1->topOffset + UVar7 + iVar9;
    UVar2 = pUVar1->rightOffset;
    UVar3 = pUVar1->bottomOffset;
    control->contentWidth = UVar2;
    control->contentHeight = UVar3;
    UVar7 = control->scrollOffsetY;
    iVar9 = (control->base).top;
    pUVar1->right = UVar2 + control->scrollOffsetX + (control->base).left;
    pUVar1->bottom = UVar3 + UVar7 + iVar9;
    UVar7 = control->scrollOffsetX;
    UVar8 = control->scrollOffsetY;
    pUVar1 = (control->base).firstChild;
    iVar10 = (control->contentWidth - control->viewportWidth) + UVar7;
    iVar9 = (control->contentHeight - control->viewportHeight) + UVar8;
    if (iVar10 < 0) {
      control->scrollOffsetX = control->scrollOffsetX - iVar10;
      pUVar1->left = pUVar1->left - iVar10;
      pUVar1->right = pUVar1->right - iVar10;
      UVar7 = UVar7 - iVar10;
    }
    if (iVar9 < 0) {
      control->scrollOffsetY = control->scrollOffsetY - iVar9;
      pUVar1->top = pUVar1->top - iVar9;
      pUVar1->bottom = pUVar1->bottom - iVar9;
      UVar8 = UVar8 - iVar9;
    }
    if (-1 < (int)UVar7) {
      control->scrollOffsetX = 0;
      pUVar1->left = pUVar1->left - UVar7;
      pUVar1->right = pUVar1->right - UVar7;
    }
    if (-1 < (int)UVar8) {
      control->scrollOffsetY = 0;
      pUVar1->top = pUVar1->top - UVar8;
      pUVar1->bottom = pUVar1->bottom - UVar8;
    }
    UVar7 = control->contentOriginX;
    UVar8 = control->contentOriginY;
    pUVar1->left = pUVar1->left + UVar7;
    pUVar1->top = pUVar1->top + UVar8;
    pUVar1->right = pUVar1->right + UVar7;
    pUVar1->bottom = pUVar1->bottom + UVar8;
    (*pUVar1->vtable->layout)(pUVar1);
    control->horizontalThumbLeft = 0;
    control->verticalThumbTop = 0;
    control->horizontalThumbRight = 0;
    control->verticalThumbBottom = 0;
    if ((control->scrollStateFlags & UI_SCROLL_HORIZONTAL_BAR_AT_TOP) != 0) {
      GVar11 = (*g_GraphicsTextureSourceGetLogicalSize)(0x5a,g_UiWindowTextureSource);
      control->verticalThumbTop = control->verticalThumbTop + GVar11.logicalHeightPixels;
      control->verticalThumbBottom = control->verticalThumbBottom + GVar11.logicalHeightPixels;
    }
    if ((control->scrollStateFlags & UI_SCROLL_VERTICAL_BAR_AT_LEFT) != 0) {
      GVar11 = (*g_GraphicsTextureSourceGetLogicalSize)(0x5e,g_UiWindowTextureSource);
      control->horizontalThumbLeft = control->horizontalThumbLeft + GVar11.logicalWidthPixels;
      control->horizontalThumbRight = control->horizontalThumbRight + GVar11.logicalWidthPixels;
    }
    iVar9 = (control->base).layoutWidth;
    iVar10 = (control->base).layoutHeight;
    if ((control->scrollStateFlags &
        (UI_SCROLL_HORIZONTAL_BAR_AT_BOTTOM|UI_SCROLL_HORIZONTAL_BAR_AT_TOP)) != 0) {
      GVar11 = (*g_GraphicsTextureSourceGetLogicalSize)(0x5a,g_UiWindowTextureSource);
      dVar5 = GVar11.logicalWidthPixels;
      control->horizontalThumbLeft = control->horizontalThumbLeft + dVar5;
      control->horizontalThumbRight = control->horizontalThumbRight + dVar5;
      iVar9 = iVar9 + dVar5 * -2;
      iVar10 = iVar10 - GVar11.logicalHeightPixels;
    }
    if ((control->scrollStateFlags &
        (UI_SCROLL_VERTICAL_BAR_AT_RIGHT|UI_SCROLL_VERTICAL_BAR_AT_LEFT)) != 0) {
      GVar11 = (*g_GraphicsTextureSourceGetLogicalSize)(0x5e,g_UiWindowTextureSource);
      dVar5 = GVar11.logicalHeightPixels;
      control->verticalThumbTop = control->verticalThumbTop + dVar5;
      control->verticalThumbBottom = control->verticalThumbBottom + dVar5;
      iVar10 = iVar10 + dVar5 * -2;
      iVar9 = iVar9 - GVar11.logicalWidthPixels;
    }
    if ((control->scrollStateFlags &
        (UI_SCROLL_HORIZONTAL_BAR_AT_BOTTOM|UI_SCROLL_HORIZONTAL_BAR_AT_TOP)) != 0) {
      uVar6 = (uint)(((longlong)(int)control->viewportWidth * (longlong)iVar9) /
                    (longlong)(int)control->contentWidth);
      GVar11 = (*g_GraphicsTextureSourceGetLogicalSize)(0xc0,g_UiWindowTextureSource);
      uVar4 = GVar11.logicalWidthPixels * 2;
      if (uVar6 < uVar4) {
        uVar6 = uVar4;
      }
      control->horizontalThumbRight = control->horizontalThumbRight + uVar6;
      iVar9 = (int)(((longlong)(int)-control->scrollOffsetX * (longlong)(int)(iVar9 - uVar6)) /
                   (longlong)(int)(control->contentWidth - control->viewportWidth));
      control->horizontalThumbLeft = control->horizontalThumbLeft + iVar9;
      control->horizontalThumbRight = control->horizontalThumbRight + iVar9;
    }
    if ((control->scrollStateFlags &
        (UI_SCROLL_VERTICAL_BAR_AT_RIGHT|UI_SCROLL_VERTICAL_BAR_AT_LEFT)) != 0) {
      uVar6 = (uint)(((longlong)(int)control->viewportHeight * (longlong)iVar10) /
                    (longlong)(int)control->contentHeight);
      GVar11 = (*g_GraphicsTextureSourceGetLogicalSize)(0xc2,g_UiWindowTextureSource);
      uVar4 = GVar11.logicalHeightPixels * 2;
      if (uVar6 < uVar4) {
        uVar6 = uVar4;
      }
      control->verticalThumbBottom = control->verticalThumbBottom + uVar6;
      iVar9 = (int)(((longlong)(int)-control->scrollOffsetY * (longlong)(int)(iVar10 - uVar6)) /
                   (longlong)(int)(control->contentHeight - control->viewportHeight));
      control->verticalThumbTop = control->verticalThumbTop + iVar9;
      control->verticalThumbBottom = control->verticalThumbBottom + iVar9;
    }
  }
  return;
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
void __thandor_void_preserve_eax_ecx_edx
UiScrollableControl_ClampOffsetsToViewport
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


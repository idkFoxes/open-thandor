#include <thandor/ui/controls/buttons.h>

/* Implementation ownership: ui/controls/buttons. */

/* Address: 0x004B1D20.
   Ownership: ui/controls/buttons.
   Purpose: Depth-first traverses a UI subtree and advances nodes whose vtable is exactly
   g_UiSpriteButtonControlVtable.
   Local calls: UiSpriteButtonControl_AdvanceAnimation.
*/
void UiTree_AdvanceSpriteButtonAnimations(UiNodeBase *root)

{
  int iVar1;
  int extraout_ECX;
  UiSpriteButtonControl *control;
  UiSpriteButtonControl *unaff_EBP;
  
  iVar1 = 1;
  control = (UiSpriteButtonControl *)root->firstChild;
  do {
    for (; control != (UiSpriteButtonControl *)0xffffffff;
        control = (UiSpriteButtonControl *)(control->selectable).base.firstChild) {
      if ((control->selectable).base.vtable == &g_UiSpriteButtonControlVtable) {
        UiSpriteButtonControl_AdvanceAnimation(control);
        iVar1 = extraout_ECX;
      }
      unaff_EBP = (UiSpriteButtonControl *)(control->selectable).base.nextSibling;
      iVar1 = iVar1 + 1;
    }
    iVar1 = iVar1 + -1;
    control = unaff_EBP;
  } while (iVar1 != 0);
  return;
}

/* Address: 0x004B1620.
   Ownership: ui/controls/buttons.
   Purpose: Initializes animation state, optionally expands a 0x20-byte sequence descriptor into layout and frame
   ranges, randomizes the initial normal frame, and relocates child pointers.
   Cross-module calls: Random_NextPrimary [core/math/random], UiContainer_RelocateChildren [ui/controls/layout].
*/
void UiSpriteButtonControl_Relocate
               (UiSerializedRelocationDelta relocationDelta,UiSpriteButtonControl *control)

{
  UiSelectableStateFlags *pUVar1;
  sdword *psVar2;
  sdword sVar3;
  GraphicsSubresourceEndIndex GVar4;
  dword dVar5;
  UiSerializedRelocationDelta extraout_ECX;
  uint uVar6;
  
  if (((control->selectable).stateFlags & 0x80) != 0) {
    control->animationFrameOffset = 0;
    if (((control->selectable).stateFlags & 0x100) != 0) {
      psVar2 = (sdword *)control->normalSubresourceStartOrDescriptor;
      sVar3 = psVar2[1];
      (control->selectable).base.leftOffset = *psVar2;
      (control->selectable).base.topOffset = sVar3;
      sVar3 = psVar2[3];
      (control->selectable).base.rightOffset = psVar2[2];
      (control->selectable).base.bottomOffset = sVar3;
      GVar4 = psVar2[5];
      control->normalSubresourceStartOrDescriptor = psVar2[4];
      control->normalSubresourceEndExclusive = GVar4;
      GVar4 = psVar2[7];
      control->selectedSubresourceStart = psVar2[6];
      control->selectedSubresourceEndExclusive = GVar4;
      pUVar1 = &(control->selectable).stateFlags;
      *pUVar1 = *pUVar1 & 0xfffffeff;
    }
    dVar5 = Random_NextPrimary();
    uVar6 = control->normalSubresourceEndExclusive - control->normalSubresourceStartOrDescriptor;
    relocationDelta = extraout_ECX;
    if (uVar6 != 0) {
      control->animationFrameOffset = dVar5 % uVar6;
    }
  }
  UiContainer_RelocateChildren(relocationDelta,(UiNodeBase *)control);
  return;
}

/* Address: 0x004B16E0.
   Ownership: ui/controls/buttons.
   Purpose: Draws the active normal or selected sprite frame with signed state-specific offsets, optional
   animation-frame offset, and optional alternate texture source.
*/
void UiSpriteButtonControl_DrawClipped
               (UiPixelCoordinate clipTop,UiPixelCoordinate clipLeft,UiPixelCoordinate clipBottom,
               UiPixelCoordinate clipRight,UiSpriteButtonControl *control)

{
  sbyte sVar1;
  sbyte sVar2;
  bool bVar3;
  dword arg6;
  dword dVar4;
  GraphicsTextureSourceAsset *pGVar5;
  SoftwareFramebufferAccess *pSVar6;
  
  if ((((control->selectable).base.nodeFlags & UI_NODE_SUPPRESSED) == 0) &&
     (((((control->selectable).stateFlags & UI_SELECTABLE_SELECTED_OR_CHECKED) != 0 ||
       (((control->selectable).stateFlags & 0x400) == 0)) &&
      (bVar3 = false, control->primaryTextureSource != (GraphicsTextureSourceAsset *)0x0)))) {
    (*g_GraphicsFramebufferBeginAccess)();
    if (!bVar3) {
      if (((control->selectable).stateFlags & UI_SELECTABLE_SELECTED_OR_CHECKED) == 0) {
        sVar1 = (control->drawOffsets).normalX;
        sVar2 = (control->drawOffsets).normalY;
      }
      else {
        sVar1 = (control->drawOffsets).selectedX;
        sVar2 = (control->drawOffsets).selectedY;
      }
      pGVar5 = control->primaryTextureSource;
      if (((control->selectable).stateFlags & UI_SELECTABLE_SELECTED_OR_CHECKED) == 0) {
        dVar4 = control->normalSubresourceStartOrDescriptor;
      }
      else {
        if ((((control->selectable).stateFlags & 0x80) == 0) &&
           (((control->selectable).stateFlags & 0x800) != 0)) {
          pGVar5 = control->alternateTextureSource;
        }
        dVar4 = control->selectedSubresourceStart;
      }
      if (((control->selectable).stateFlags & 0x80) != 0) {
        dVar4 = dVar4 + control->animationFrameOffset;
      }
      (*g_GraphicsTextureSourceBlitModulatedSourceAlpha)
                (clipTop,clipLeft,clipBottom,clipRight,(int)sVar2 + (control->selectable).base.top,
                 (int)sVar1 + (control->selectable).base.left,0x7f000000,dVar4,pGVar5,
                 g_FramebufferAccess);
      pGVar5 = control->primaryTextureSource;
      pSVar6 = g_FramebufferAccess;
      if (((control->selectable).stateFlags & UI_SELECTABLE_SELECTED_OR_CHECKED) == 0) {
        dVar4 = control->normalSubresourceStartOrDescriptor;
      }
      else {
        if ((((control->selectable).stateFlags & 0x80) == 0) &&
           (((control->selectable).stateFlags & 0x800) != 0)) {
          pGVar5 = control->alternateTextureSource;
        }
        dVar4 = control->selectedSubresourceStart;
        if (((control->selectable).stateFlags & 0x40) != 0) {
          arg6 = control->normalSubresourceStartOrDescriptor;
          if (((control->selectable).stateFlags & 0x80) != 0) {
            arg6 = arg6 + control->animationFrameOffset;
          }
          (*g_GraphicsTextureSourceBlitSourceAlpha)
                    (clipTop,clipLeft,clipBottom,clipRight,(control->selectable).base.top,
                     (control->selectable).base.left,arg6,control->primaryTextureSource,
                     g_FramebufferAccess);
        }
      }
      if (((control->selectable).stateFlags & 0x80) != 0) {
        dVar4 = dVar4 + control->animationFrameOffset;
      }
      (*g_GraphicsTextureSourceBlitSourceAlpha)
                (clipTop,clipLeft,clipBottom,clipRight,(control->selectable).base.top,
                 (control->selectable).base.left,dVar4,pGVar5,pSVar6);
      (*g_GraphicsFramebufferEndAccess)();
    }
  }
  return;
}

/* Address: 0x004B1890.
   Ownership: ui/controls/buttons.
   Purpose: Updates selected/toggle and animation state for a non-right pointer press, optionally plays
   activationSoundId, queues actionId, and invalidates the root.
   Cross-module calls: UiNode_InvalidateRoot [ui/core/runtime], UiActionQueue_Enqueue [ui/core/runtime].
*/
void UiSpriteButtonControl_NonRightPress
               (UiPointerWheelDelta wheelDelta,UiPixelCoordinate pointerY,UiPixelCoordinate pointerX
               ,UiSpriteButtonControl *control)

{
  UiSelectableStateFlags *pUVar1;
  UiSelectableStateFlags *selectionStateFlagsField;
  UiSelectableStateFlags *stateFlagsField;
  
  if (((control->selectable).base.nodeFlags & UI_NODE_SUPPRESSED) == 0) {
    if (((control->selectable).stateFlags & UI_SELECTABLE_PERSISTENT_ACTIVATION_MODE) == 0) {
      stateFlagsField = &(control->selectable).stateFlags;
      *stateFlagsField = *stateFlagsField | UI_SELECTABLE_SELECTED_OR_CHECKED;
      if ((((control->selectable).stateFlags & 0x80) != 0) &&
         ((((control->selectable).stateFlags & 0x800) != 0 ||
          (control->selectedSubresourceEndExclusive <=
           control->animationFrameOffset + control->selectedSubresourceStart)))) {
        control->animationFrameOffset = 0;
      }
UiSpriteButtonControl_NonRightPress_InvalidateAfterStateOrAnimationUpdateAndReturn:
      UiNode_InvalidateRoot((UiNodeBase *)control);
      return;
    }
    if (((control->selectable).stateFlags & UI_SELECTABLE_TOGGLE_ON_ACTIVATION) != 0) {
      if ((((control->selectable).stateFlags & 0x200) != 0) && (control->activationSoundId != 0)) {
        (*g_SoundPlayOneShot)
                  (g_UiSoundGainQ15,g_UiSoundGainQ15,
                   (DirectSoundVoiceSet *)control->activationSoundId);
      }
      selectionStateFlagsField = &(control->selectable).stateFlags;
      *selectionStateFlagsField = *selectionStateFlagsField ^ UI_SELECTABLE_SELECTED_OR_CHECKED;
      if (((control->selectable).stateFlags & 0x80) != 0) {
        if ((((control->selectable).stateFlags & 0x800) != 0) ||
           (control->selectedSubresourceEndExclusive <=
            control->animationFrameOffset + control->selectedSubresourceStart)) {
          control->animationFrameOffset = 0;
        }
        pUVar1 = &(control->selectable).stateFlags;
        *pUVar1 = *pUVar1 | 0x1000;
        if (((control->selectable).stateFlags & 0x800) != 0)
        goto UiSpriteButtonControl_NonRightPress_InvalidateAfterStateOrAnimationUpdateAndReturn;
      }
      UiActionQueue_Enqueue((control->selectable).actionId,control);
      UiNode_InvalidateRoot((UiNodeBase *)control);
      return;
    }
    if (((control->selectable).stateFlags & UI_SELECTABLE_SELECTED_OR_CHECKED) == 0) {
      if ((((control->selectable).stateFlags & 0x200) != 0) && (control->activationSoundId != 0)) {
        (*g_SoundPlayOneShot)
                  (g_UiSoundGainQ15,g_UiSoundGainQ15,
                   (DirectSoundVoiceSet *)control->activationSoundId);
      }
      pUVar1 = &(control->selectable).stateFlags;
      *pUVar1 = *pUVar1 | UI_SELECTABLE_SELECTED_OR_CHECKED;
      if (((control->selectable).stateFlags & 0x80) != 0) {
        if ((((control->selectable).stateFlags & 0x800) != 0) ||
           (control->selectedSubresourceEndExclusive <=
            control->animationFrameOffset + control->selectedSubresourceStart)) {
          control->animationFrameOffset = 0;
        }
        pUVar1 = &(control->selectable).stateFlags;
        *pUVar1 = *pUVar1 | 0x1000;
        if (((control->selectable).stateFlags & 0x800) != 0)
        goto UiSpriteButtonControl_NonRightPress_InvalidateAfterStateOrAnimationUpdateAndReturn;
      }
      UiActionQueue_Enqueue((control->selectable).actionId,control);
      UiNode_InvalidateRoot((UiNodeBase *)control);
    }
  }
  return;
}

/* Address: 0x004B1A30.
   Ownership: ui/controls/buttons.
   Purpose: Clears momentary selected state on release, resets completed animation state when required, optionally
   plays activationSoundId, queues actionId, and invalidates the root.
   Cross-module calls: UiActionQueue_Enqueue [ui/core/runtime], UiNode_InvalidateRoot [ui/core/runtime].
*/
void UiSpriteButtonControl_NonRightRelease
               (UiPointerWheelDelta wheelDelta,UiPixelCoordinate pointerY,UiPixelCoordinate pointerX
               ,UiSpriteButtonControl *control)

{
  UiSelectableStateFlags *stateFlagsField;
  
  if ((((control->selectable).base.nodeFlags & UI_NODE_SUPPRESSED) == 0) &&
     (((control->selectable).stateFlags & UI_SELECTABLE_PERSISTENT_ACTIVATION_MODE) == 0)) {
    if ((((control->selectable).stateFlags & 0x80) != 0) &&
       ((((control->selectable).stateFlags & 0x800) != 0 ||
        (control->normalSubresourceEndExclusive <=
         control->animationFrameOffset + control->normalSubresourceStartOrDescriptor)))) {
      control->animationFrameOffset = 0;
    }
    if (((control->selectable).stateFlags & UI_SELECTABLE_SELECTED_OR_CHECKED) != 0) {
      if ((((control->selectable).stateFlags & 0x200) != 0) && (control->activationSoundId != 0)) {
        (*g_SoundPlayOneShot)
                  (g_UiSoundGainQ15,g_UiSoundGainQ15,
                   (DirectSoundVoiceSet *)control->activationSoundId);
      }
      stateFlagsField = &(control->selectable).stateFlags;
      *stateFlagsField = *stateFlagsField & ~UI_SELECTABLE_SELECTED_OR_CHECKED;
      UiActionQueue_Enqueue((control->selectable).actionId,control);
      UiNode_InvalidateRoot((UiNodeBase *)control);
    }
  }
  return;
}

/* Address: 0x004B1AE0.
   Ownership: ui/controls/buttons.
   Purpose: Uses opaque-sprite or rectangular hit testing during capture to update the selected hover/pressed state
   and invalidate changes.
   Cross-module calls: UiNode_InvalidateRoot [ui/core/runtime].
*/
void UiSpriteButtonControl_NonRightDrag
               (UiPointerWheelDelta wheelDelta,UiPixelCoordinate pointerY,UiPixelCoordinate pointerX
               ,UiSpriteButtonControl *control)

{
  bool opaquePixelHit;
  UiSelectableStateFlags *selectedStateFlagsField;
  UiSelectableStateFlags *stateFlagsField;
  
  if (((control->selectable).base.nodeFlags & UI_NODE_SUPPRESSED) != 0) {
    return;
  }
  if (((control->selectable).stateFlags & UI_SELECTABLE_PERSISTENT_ACTIVATION_MODE) != 0) {
    return;
  }
  if (((control->selectable).stateFlags & 0x80) != 0) {
    return;
  }
  if (((control->selectable).stateFlags & 0x20) == 0) {
    if (control->primaryTextureSource != (GraphicsTextureSourceAsset *)0x0) {
      opaquePixelHit = false;
      if (((control->selectable).stateFlags & 0x400) == 0) {
        (*g_GraphicsTextureSourceTestOpaquePixel)
                  (pointerY,pointerX,(control->selectable).base.top,(control->selectable).base.left,
                   control->normalSubresourceStartOrDescriptor,control->primaryTextureSource);
        if (opaquePixelHit) {
UiSpriteButtonControl_NonRightDrag_SetPointerInsideStateAndInvalidate:
          if (((control->selectable).stateFlags & UI_SELECTABLE_SELECTED_OR_CHECKED) != 0) {
            return;
          }
          stateFlagsField = &(control->selectable).stateFlags;
          *stateFlagsField = *stateFlagsField | UI_SELECTABLE_SELECTED_OR_CHECKED;
          UiNode_InvalidateRoot((UiNodeBase *)control);
          return;
        }
      }
      else {
        (*g_GraphicsTextureSourceTestOpaquePixel)
                  (pointerY,pointerX,(control->selectable).base.top,(control->selectable).base.left,
                   control->selectedSubresourceStart,control->primaryTextureSource);
        if (opaquePixelHit)
        goto UiSpriteButtonControl_NonRightDrag_SetPointerInsideStateAndInvalidate;
      }
    }
  }
  else if (((((control->selectable).base.left <= pointerX) &&
            ((control->selectable).base.top <= pointerY)) &&
           (pointerX < (control->selectable).base.right)) &&
          (pointerY < (control->selectable).base.bottom))
  goto UiSpriteButtonControl_NonRightDrag_SetPointerInsideStateAndInvalidate;
  if (((control->selectable).stateFlags & UI_SELECTABLE_SELECTED_OR_CHECKED) != 0) {
    selectedStateFlagsField = &(control->selectable).stateFlags;
    *selectedStateFlagsField = *selectedStateFlagsField & ~UI_SELECTABLE_SELECTED_OR_CHECKED;
    UiNode_InvalidateRoot((UiNodeBase *)control);
  }
  return;
}

/* Address: 0x004B1BF0.
   Ownership: ui/controls/buttons.
   Purpose: Returns the control when its active sprite pixel or configured rectangular region contains the point;
   otherwise returns the 0xFFFFFFFF sentinel.
*/
UiNodeBase *
UiSpriteButtonControl_HitTestOpaque
          (UiPixelCoordinate pointerY,UiPixelCoordinate pointerX,UiSpriteButtonControl *control)

{
  UiNodeBase *opaqueHitNode;
  UiSpriteButtonControl *hitNode;
  bool opaquePixelHit;
  
  hitNode = (UiSpriteButtonControl *)0xffffffff;
  if ((((control->selectable).base.nodeFlags & UI_NODE_SUPPRESSED) == 0) &&
     (hitNode = control, ((control->selectable).stateFlags & 0x20) == 0)) {
    opaquePixelHit = false;
    if (((control->selectable).stateFlags & 0x400) == 0) {
      opaquePixelHit = false;
      if (control->primaryTextureSource == (GraphicsTextureSourceAsset *)0x0) {
        hitNode = (UiSpriteButtonControl *)0xffffffff;
        return (UiNodeBase *)hitNode;
      }
      opaqueHitNode =
           (UiNodeBase *)
           (*g_GraphicsTextureSourceTestOpaquePixel)
                     (pointerY,pointerX,(control->selectable).base.top,
                      (control->selectable).base.left,control->normalSubresourceStartOrDescriptor,
                      control->primaryTextureSource);
    }
    else {
      opaqueHitNode =
           (UiNodeBase *)
           (*g_GraphicsTextureSourceTestOpaquePixel)
                     (pointerY,pointerX,(control->selectable).base.top,
                      (control->selectable).base.left,control->selectedSubresourceStart,
                      control->primaryTextureSource);
    }
    if (!opaquePixelHit) {
      hitNode = (UiSpriteButtonControl *)opaqueHitNode;
      return (UiNodeBase *)hitNode;
    }
  }
  return (UiNodeBase *)hitNode;
}

/* Address: 0x00515010.
   Ownership: ui/controls/buttons.
   Purpose: Binary entry is anchored by g_UiNodeVtable_00514FC0[2]@00514FC0.
   Cross-module calls: UiContainer_DrawIntersectingChildren [ui/controls/layout].
*/
void UiImageActionControl_DrawImageAndChildren
               (UiPixelCoordinate clipTop,UiPixelCoordinate clipLeft,UiPixelCoordinate clipBottom,
               UiPixelCoordinate clipRight,UiNodeBase *control)

{
  uint uVar1;
  int iVar2;
  int iVar3;
  int iVar4;
  uint uVar5;
  sdword arg5;
  sdword arg3;
  int arg4;
  sdword arg6;
  sdword arg6_00;
  bool bVar6;
  undefined8 uVar7;
  
  if (((control->nodeFlags & UI_NODE_SUPPRESSED) == 0) &&
     (bVar6 = false, control[1].parent != (UiNodeBase *)0x0)) {
    (*g_GraphicsFramebufferBeginAccess)();
    if (!bVar6) {
      if (((uint)control[1].nextSibling & 1) == 0) {
        (*g_GraphicsTextureSourceBlitSourceAlpha)
                  (clipTop,clipLeft,clipBottom,clipRight,control->top,control->left,
                   (dword)control[1].vtable,(GraphicsTextureSourceAsset *)control[1].parent,
                   g_FramebufferAccess);
        (*g_GraphicsFramebufferEndAccess)();
      }
      else {
        uVar1 = control[1].right;
        uVar5 = control->layoutWidth - uVar1;
        if (((uint)control->layoutWidth < uVar1 || uVar5 == 0) ||
           (((uint)control[1].nextSibling & 4) == 0)) {
          (*g_GraphicsTextureSourceStretchDirectColorBilinear)
                    (control->layoutHeight,control->layoutWidth,control->top,control->left,
                     (dword)control[1].vtable,(GraphicsTextureSourceAsset *)control[1].parent,
                     g_FramebufferAccess);
          (*g_GraphicsFramebufferEndAccess)();
        }
        else {
          iVar4 = (int)(((longlong)(int)uVar1 * (longlong)control->layoutHeight) /
                       (longlong)control->layoutWidth);
          iVar2 = control->left;
          arg4 = ((uint)(control->layoutHeight - iVar4) >> 1) + control->top;
          iVar3 = control[1].right;
          iVar4 = iVar4 + arg4;
          (*g_GraphicsFramebufferFillRectArgb)
                    (clipTop,clipLeft,clipBottom,clipRight,arg4,control->right,control->top,
                     control->left,0xff000000,g_FramebufferAccess);
          (*g_GraphicsFramebufferFillRectArgb)
                    (clipTop,clipLeft,clipBottom,clipRight,control->bottom,control->right,iVar4,
                     control->left,0xff000000,g_FramebufferAccess);
          (*g_GraphicsFramebufferFillRectArgb)
                    (clipTop,clipLeft,clipBottom,clipRight,iVar4,arg5,arg6,control->left,0xff000000,
                     g_FramebufferAccess);
          uVar7 = (*g_GraphicsFramebufferFillRectArgb)
                            (clipTop,clipLeft,clipBottom,clipRight,iVar4,control->right,arg6_00,
                             (uVar5 >> 1) + iVar2 + iVar3,0xff000000,g_FramebufferAccess);
          (*g_GraphicsTextureSourceStretchDirectColorBilinear)
                    ((dword)uVar7,control[1].right,(sdword)((ulonglong)uVar7 >> 0x20),arg3,
                     (dword)control[1].vtable,(GraphicsTextureSourceAsset *)control[1].parent,
                     g_FramebufferAccess);
          (*g_GraphicsFramebufferEndAccess)();
        }
      }
    }
  }
  UiContainer_DrawIntersectingChildren(clipTop,clipLeft,clipBottom,clipRight,control);
  return;
}

/* Address: 0x005151F0.
   Ownership: ui/controls/buttons.
   Purpose: Binary entry is anchored by g_UiNodeVtable_00514FC0[10]@00514FC0.
*/
GraphicsCursorFrameIndex
UiImageActionControl_QueryPointerCode
          (UiPixelCoordinate pointerY,UiPixelCoordinate pointerX,UiNodeBase *control)

{
  return (GraphicsCursorFrameIndex)control[1].firstChild;
}

/* Address: 0x00515210.
   Ownership: ui/controls/buttons.
   Purpose: Binary entry is anchored by g_UiNodeVtable_00514FC0[4]@00514FC0.
   Cross-module calls: UiActionQueue_Enqueue [ui/core/runtime].
*/
void UiImageActionControl_EnqueuePrimaryAction(void)

{
  void *in_stack_00000010;
  
  UiActionQueue_Enqueue(*(UiActionId *)((int)in_stack_00000010 + 0x5c),in_stack_00000010);
  return;
}

/* Address: 0x00515230.
   Ownership: ui/controls/buttons.
   Purpose: Binary entry is anchored by g_UiNodeVtable_00514FC0[6]@00514FC0.
   Cross-module calls: UiActionQueue_Enqueue [ui/core/runtime].
*/
void UiImageActionControl_EnqueueSecondaryAction(void)

{
  void *in_stack_00000010;
  
  UiActionQueue_Enqueue(*(UiActionId *)((int)in_stack_00000010 + 0x60),in_stack_00000010);
  return;
}

/* Address: 0x00515250.
   Ownership: ui/controls/buttons.
   Purpose: Binary entry is anchored by g_UiNodeVtable_00514FC0[12]@00514FC0.
   Cross-module calls: UiKeyboardFocus_MoveNext [ui/controls/input], UiActionQueue_Enqueue [ui/core/runtime].
*/
void UiImageActionControl_HandleKeyboardActivationCf
               (UiKeyboardStateMask keyboardStateMask,UiKeyboardEventCode keyCode,
               UiNodeBase *control)

{
  if (keyCode == 0x10002) {
    UiKeyboardFocus_MoveNext();
    return;
  }
  if (((uint)control[1].nextSibling & 2) != 0) {
    UiActionQueue_Enqueue(control[1].left,control);
    return;
  }
  return;
}

/* Address: 0x005152E0.
   Ownership: ui/controls/buttons.
   Purpose: Handles ui conditional action control draw clipped.
   Cross-module calls: UiWindow_BlitTiledHorizontalEdge [ui/controls/layout], UiWindow_BlitTiledVerticalEdge
   [ui/controls/layout], UiWindow_BlitTiledInterior [ui/controls/layout], RichTextCommandStream_MeasureRegs
   [assets/text/richtext], RichTextCommandStream_DrawSingleLine [assets/text/richtext].
*/
void UiConditionalActionControl_DrawClipped
               (int clipTop,int clipLeft,int clipBottom,int clipRight,UiNodeBase *control)

{
  int iVar1;
  int extraout_ECX;
  int extraout_ECX_00;
  int extraout_ECX_01;
  int extraout_ECX_02;
  UiPixelCoordinate tileRight;
  int extraout_ECX_03;
  int extraout_ECX_04;
  int iVar2;
  UiPixelCoordinate extraout_ECX_05;
  UiPixelCoordinate UVar3;
  UiPixelCoordinate extraout_ECX_06;
  UiPixelCoordinate extraout_ECX_07;
  int iVar4;
  int extraout_EDX;
  UiPixelCoordinate tileStart;
  UiPixelCoordinate tileBottom;
  int extraout_EDX_00;
  int extraout_EDX_01;
  int iVar5;
  int iVar6;
  UiNodeVtable *pUVar7;
  int iVar8;
  bool bVar9;
  qword qVar10;
  undefined8 uVar11;
  GraphicsSubresourceIndex GStack_28;
  
  bVar9 = false;
  if ((control[1].vtable != (UiNodeVtable *)0x0) && ((*g_GraphicsFramebufferBeginAccess)(), !bVar9))
  {
    GStack_28 = 0x7b;
    if ((control->layoutWidth == 0x1a0) && (control->layoutHeight == 0x3a)) {
      GStack_28 = 200;
    }
    qVar10 = (*g_GraphicsTextureSourceGetLogicalSize)(0x72,g_UiWindowTextureSource);
    iVar4 = (int)(qVar10 >> 0x20);
    iVar1 = (int)qVar10;
    iVar8 = control->layoutWidth;
    iVar6 = control->layoutHeight;
    if (GStack_28 == 0x7b) {
      iVar8 = iVar8 - iVar1;
      iVar6 = iVar6 - iVar4;
      qVar10 = (*g_GraphicsTextureSourceBlitSourceAlpha)
                         (clipTop,clipLeft,clipBottom,clipRight,control->top,control->left,0x72,
                          g_UiWindowTextureSource,g_FramebufferAccess);
      (*g_GraphicsTextureSourceBlitSourceAlpha)
                (clipTop,clipLeft,clipBottom,clipRight,(int)(qVar10 >> 0x20) + control->top,
                 iVar8 + control->left,0x73,g_UiWindowTextureSource,g_FramebufferAccess);
      (*g_GraphicsTextureSourceBlitSourceAlpha)
                (clipTop,clipLeft,clipBottom,clipRight,iVar6 + control->top,
                 extraout_ECX + control->left,0x74,g_UiWindowTextureSource,g_FramebufferAccess);
      qVar10 = (*g_GraphicsTextureSourceBlitSourceAlpha)
                         (clipTop,clipLeft,clipBottom,clipRight,iVar6 + control->top,
                          iVar8 + control->left,0x75,g_UiWindowTextureSource,g_FramebufferAccess);
      UiWindow_BlitTiledHorizontalEdge
                (clipTop,clipLeft,clipBottom,clipRight,0x76,iVar8,
                 (UiPixelCoordinate)(qVar10 >> 0x20),extraout_ECX_00 + iVar1,control);
      UiWindow_BlitTiledVerticalEdge
                (clipTop,clipLeft,clipBottom,clipRight,0x77,iVar6,extraout_EDX + iVar4,
                 extraout_ECX_01 - iVar1,control);
      UiWindow_BlitTiledVerticalEdge
                (clipTop,clipLeft,clipBottom,clipRight,0x78,iVar6,tileStart,iVar8,control);
      UiWindow_BlitTiledHorizontalEdge
                (clipTop,clipLeft,clipBottom,clipRight,0x79,iVar8,iVar6,extraout_ECX_02 + iVar1,
                 control);
      UiWindow_BlitTiledInterior
                (clipTop,clipLeft,clipBottom,clipRight,0x7b,iVar6,iVar8,tileBottom,tileRight,control
                );
      iVar2 = extraout_ECX_03;
      iVar5 = extraout_EDX_00;
    }
    else {
      UiWindow_BlitTiledInterior
                (clipTop,clipLeft,clipBottom,clipRight,GStack_28,iVar6,iVar8,0,0,control);
      iVar2 = extraout_ECX_04 + iVar1;
      iVar5 = extraout_EDX_01 + iVar4;
      iVar8 = iVar8 - iVar1;
      iVar6 = iVar6 - iVar4;
    }
    iVar2 = iVar2 + control->left;
    iVar5 = iVar5 + control->top;
    iVar8 = iVar8 + control->left;
    iVar6 = iVar6 + control->top;
    if (clipRight < iVar2) {
      clipRight = iVar2;
    }
    if (clipBottom < iVar5) {
      clipBottom = iVar5;
    }
    if (iVar8 < clipLeft) {
      clipLeft = iVar8;
    }
    if (iVar6 < clipTop) {
      clipTop = iVar6;
    }
    RichTextCommandStream_MeasureRegs(g_UiTextStyleNormal,(word *)control[1].left);
    if (GStack_28 == 0x7b) {
      pUVar7 = (UiNodeVtable *)0x0;
      UVar3 = extraout_ECX_05;
      do {
        uVar11 = RichTextCommandStream_DrawSingleLine
                           (clipTop,clipLeft,clipBottom,clipRight,g_UiTextStyleNormal,
                            (word *)(&control[1].left)[(int)pUVar7],iVar5,UVar3);
        pUVar7 = (UiNodeVtable *)((int)&pUVar7->relocate + 1);
        iVar5 = iVar5 + (int)((ulonglong)uVar11 >> 0x20);
        UVar3 = extraout_ECX_06;
      } while (pUVar7 < control[1].vtable);
    }
    else {
      pUVar7 = control[1].vtable;
      UVar3 = extraout_ECX_05;
      if ((UiNodeVtable *)0x4 < pUVar7) {
        pUVar7 = (UiNodeVtable *)0x4;
      }
      do {
        uVar11 = RichTextCommandStream_DrawSingleLine
                           (clipTop,clipLeft,clipBottom,clipRight,g_UiTextStyleNormal,
                            (word *)(&control[1].vtable)[(int)pUVar7],iVar5,UVar3);
        iVar5 = iVar5 + (int)((ulonglong)uVar11 >> 0x20);
        pUVar7 = (UiNodeVtable *)((int)&pUVar7[-1].pointerWheel + 3);
        UVar3 = extraout_ECX_07;
      } while (pUVar7 != (UiNodeVtable *)0x0);
    }
    (*g_GraphicsFramebufferEndAccess)();
  }
  return;
}

/* Address: 0x005155A0.
   Ownership: ui/controls/buttons.
   Purpose: Binary entry is anchored by g_UiNodeVtable_00515290[10]@00515290.
*/
GraphicsCursorFrameIndex
UiConditionalActionControl_QueryPointerCode
          (UiPixelCoordinate pointerY,UiPixelCoordinate pointerX,UiNodeBase *control)

{
  return (GraphicsCursorFrameIndex)control[1].firstChild;
}

/* Address: 0x005155C0.
   Ownership: ui/controls/buttons.
   Purpose: Binary entry is anchored by g_UiNodeVtable_00515290[11]@00515290.
   Cross-module calls: UiContainer_HitTestChildren [ui/controls/layout].
*/
UiNodeBase *
UiConditionalActionControl_HitTestWhenEnabled
          (UiPixelCoordinate pointerY,UiPixelCoordinate pointerX,UiNodeBase *control)

{
  UiNodeBase *hitNode;
  
  hitNode = (UiNodeBase *)0xffffffff;
  if (control[1].vtable != (UiNodeVtable *)0x0) {
    hitNode = UiContainer_HitTestChildren(pointerY,pointerX,control);
  }
  return hitNode;
}

/* Address: 0x005155F0.
   Ownership: ui/controls/buttons.
   Purpose: Binary entry is anchored by g_UiNodeVtable_00515290[4]@00515290.
   Cross-module calls: UiActionQueue_Enqueue [ui/core/runtime].
*/
void UiConditionalActionControl_EnqueuePrimaryActionIfEnabled(void)

{
  void *in_stack_00000010;
  
  if (*(int *)((int)in_stack_00000010 + 0x58) != 0) {
    UiActionQueue_Enqueue(*(UiActionId *)((int)in_stack_00000010 + 0x54),in_stack_00000010);
  }
  return;
}

/* Address: 0x004B1C80.
   Ownership: ui/controls/buttons.
   Purpose: Advances the active normal or selected frame range, loops at the exclusive end, optionally queues
   actionId on terminal animation state, and invalidates the root.
   Cross-module calls: UiActionQueue_Enqueue [ui/core/runtime], UiNode_InvalidateRoot [ui/core/runtime].
*/
void UiSpriteButtonControl_AdvanceAnimation(UiSpriteButtonControl *control)

{
  dword subresourceStart;
  uint subresourceEndExclusive;
  UiSelectableStateFlags *stateFlagsField;
  
  if (((control->selectable).stateFlags & 0x80) != 0) {
    if (((control->selectable).stateFlags & UI_SELECTABLE_SELECTED_OR_CHECKED) == 0) {
      subresourceStart = control->normalSubresourceStartOrDescriptor;
      subresourceEndExclusive = control->normalSubresourceEndExclusive;
    }
    else {
      subresourceStart = control->selectedSubresourceStart;
      subresourceEndExclusive = control->selectedSubresourceEndExclusive;
    }
    if (subresourceStart + 1 + control->animationFrameOffset < subresourceEndExclusive) {
      control->animationFrameOffset = control->animationFrameOffset + 1;
    }
    else {
      control->animationFrameOffset = 0;
    }
    if (subresourceEndExclusive - 1 <= subresourceStart + control->animationFrameOffset) {
      if ((((control->selectable).stateFlags & 0x800) != 0) &&
         (((control->selectable).stateFlags & 0x1000) != 0)) {
        UiActionQueue_Enqueue((control->selectable).actionId,control);
      }
      stateFlagsField = &(control->selectable).stateFlags;
      *stateFlagsField = *stateFlagsField & 0xffffefff;
    }
    UiNode_InvalidateRoot((UiNodeBase *)control);
  }
  return;
}

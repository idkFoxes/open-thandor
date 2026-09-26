/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/src/ui/controls/buttons.c
 * Reverse engineering by idkFoxes 2026
 */

#include <thandor/ui/controls/buttons.h>
#include <thandor/thandor.h>

/* Implementation ownership: ui/controls/buttons. */

/* Address: 0x004B1D20.
   Ownership: ui/controls/buttons.
   Purpose: Depth-first traverses a UI subtree and advances nodes whose vtable is exactly
   g_UiSpriteButtonControlVtable.
   Local calls: UiSpriteButtonControl_AdvanceAnimation.
*/
/* Rewritten from the assembly (0x004B1D20): a depth-first walk that pushes each node's next
   sibling on the machine stack before descending; the decompiler kept only one level. */
static void UiTree_AdvanceSpriteButtonAnimationsFrom(UiNodeBase *node)
{
  while (node != (UiNodeBase *)0xffffffff) {
    if (node->vtable == (UiNodeVtable *)&g_UiSpriteButtonControlVtable) {
      UiSpriteButtonControl_AdvanceAnimation((UiSpriteButtonControl *)node);
    }
    UiTree_AdvanceSpriteButtonAnimationsFrom(node->firstChild);
    node = node->nextSibling;
  }
}

void __thandor_void_preserve_eax_ecx_edx UiTree_AdvanceSpriteButtonAnimations(UiNodeBase *root)

{
  UiTree_AdvanceSpriteButtonAnimationsFrom(root->firstChild);
  return;
}


/* Address: 0x004B1620.
   Ownership: ui/controls/buttons.
   Purpose: Initializes animation state, optionally expands a 0x20-byte sequence descriptor into layout and frame
   ranges, randomizes the initial normal frame, and relocates child pointers.
   Cross-module calls: Random_NextPrimary [core/math/random], UiContainer_RelocateChildren [ui/controls/layout].
*/
void __thandor_void_preserve_eax_ecx_edx
UiSpriteButtonControl_Relocate
          (UiSerializedRelocationDelta relocationDelta,UiSpriteButtonControl *control)

{
  UiSelectableStateFlags *stateFlagsField;
  sdword *sequenceDescriptor;
  sdword descriptorOffset;
  GraphicsSubresourceEndIndex subresourceEnd;
  dword randomValue;
  uint normalFrameCount;
  
  if (((control->selectable).stateFlags & 0x80) != 0) {
    control->animationFrameOffset = 0;
    if (((control->selectable).stateFlags & 0x100) != 0) {
      sequenceDescriptor = (sdword *)control->normalSubresourceStartOrDescriptor;
      descriptorOffset = sequenceDescriptor[1];
      (control->selectable).base.leftOffset = *sequenceDescriptor;
      (control->selectable).base.topOffset = descriptorOffset;
      descriptorOffset = sequenceDescriptor[3];
      (control->selectable).base.rightOffset = sequenceDescriptor[2];
      (control->selectable).base.bottomOffset = descriptorOffset;
      subresourceEnd = sequenceDescriptor[5];
      control->normalSubresourceStartOrDescriptor = sequenceDescriptor[4];
      control->normalSubresourceEndExclusive = subresourceEnd;
      subresourceEnd = sequenceDescriptor[7];
      control->selectedSubresourceStart = sequenceDescriptor[6];
      control->selectedSubresourceEndExclusive = subresourceEnd;
      stateFlagsField = &(control->selectable).stateFlags;
      *stateFlagsField = *stateFlagsField & 0xfffffeff;
    }
    randomValue = Random_NextPrimary();
    normalFrameCount = control->normalSubresourceEndExclusive - control->normalSubresourceStartOrDescriptor;
    if (normalFrameCount != 0) {
      control->animationFrameOffset = randomValue % normalFrameCount;
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
void __thandor_void_preserve_eax_ecx_edx
UiSpriteButtonControl_DrawClipped
          (UiPixelCoordinate clipTop,UiPixelCoordinate clipLeft,UiPixelCoordinate clipBottom,
          UiPixelCoordinate clipRight,UiSpriteButtonControl *control)

{
  sbyte drawOffsetX;
  sbyte drawOffsetY;
  bool accessFailed;
  dword underlaySubresource;
  dword subresourceIndex;
  GraphicsTextureSourceAsset *textureSource;
  SoftwareFramebufferAccess *framebufferAccess;
  
  if ((((control->selectable).base.nodeFlags & UI_NODE_SUPPRESSED) == 0) &&
     (((((control->selectable).stateFlags & UI_SELECTABLE_SELECTED_OR_CHECKED) != 0 ||
       (((control->selectable).stateFlags & 0x400) == 0)) &&
      (control->primaryTextureSource != (GraphicsTextureSourceAsset *)0x0)))) {
    accessFailed = (*g_GraphicsFramebufferBeginAccess)();
    if (!accessFailed) {
      if (((control->selectable).stateFlags & UI_SELECTABLE_SELECTED_OR_CHECKED) == 0) {
        drawOffsetX = (control->drawOffsets).normalX;
        drawOffsetY = (control->drawOffsets).normalY;
      }
      else {
        drawOffsetX = (control->drawOffsets).selectedX;
        drawOffsetY = (control->drawOffsets).selectedY;
      }
      textureSource = control->primaryTextureSource;
      if (((control->selectable).stateFlags & UI_SELECTABLE_SELECTED_OR_CHECKED) == 0) {
        subresourceIndex = control->normalSubresourceStartOrDescriptor;
      }
      else {
        if ((((control->selectable).stateFlags & 0x80) == 0) &&
           (((control->selectable).stateFlags & 0x800) != 0)) {
          textureSource = control->alternateTextureSource;
        }
        subresourceIndex = control->selectedSubresourceStart;
      }
      if (((control->selectable).stateFlags & 0x80) != 0) {
        subresourceIndex = subresourceIndex + control->animationFrameOffset;
      }
      (*g_GraphicsTextureSourceBlitModulatedSourceAlpha)
                (clipTop,clipLeft,clipBottom,clipRight,(int)drawOffsetY + (control->selectable).base.top,
                 (int)drawOffsetX + (control->selectable).base.left,0x7f000000,subresourceIndex,textureSource,
                 g_FramebufferAccess);
      textureSource = control->primaryTextureSource;
      framebufferAccess = g_FramebufferAccess;
      if (((control->selectable).stateFlags & UI_SELECTABLE_SELECTED_OR_CHECKED) == 0) {
        subresourceIndex = control->normalSubresourceStartOrDescriptor;
      }
      else {
        if ((((control->selectable).stateFlags & 0x80) == 0) &&
           (((control->selectable).stateFlags & 0x800) != 0)) {
          textureSource = control->alternateTextureSource;
        }
        subresourceIndex = control->selectedSubresourceStart;
        if (((control->selectable).stateFlags & 0x40) != 0) {
          underlaySubresource = control->normalSubresourceStartOrDescriptor;
          if (((control->selectable).stateFlags & 0x80) != 0) {
            underlaySubresource = underlaySubresource + control->animationFrameOffset;
          }
          (*g_GraphicsTextureSourceBlitSourceAlpha)
                    (clipTop,clipLeft,clipBottom,clipRight,(control->selectable).base.top,
                     (control->selectable).base.left,underlaySubresource,control->primaryTextureSource,
                     g_FramebufferAccess);
        }
      }
      if (((control->selectable).stateFlags & 0x80) != 0) {
        subresourceIndex = subresourceIndex + control->animationFrameOffset;
      }
      (*g_GraphicsTextureSourceBlitSourceAlpha)
                (clipTop,clipLeft,clipBottom,clipRight,(control->selectable).base.top,
                 (control->selectable).base.left,subresourceIndex,textureSource,framebufferAccess);
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
void __thandor_void_preserve_eax_ecx_edx
UiSpriteButtonControl_NonRightPress
          (UiPointerWheelDelta wheelDelta,UiPixelCoordinate pointerY,UiPixelCoordinate pointerX,
          UiSpriteButtonControl *control)

{
  UiSelectableStateFlags *pressStateFlagsField;
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
        pressStateFlagsField = &(control->selectable).stateFlags;
        *pressStateFlagsField = *pressStateFlagsField | 0x1000;
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
      pressStateFlagsField = &(control->selectable).stateFlags;
      *pressStateFlagsField = *pressStateFlagsField | UI_SELECTABLE_SELECTED_OR_CHECKED;
      if (((control->selectable).stateFlags & 0x80) != 0) {
        if ((((control->selectable).stateFlags & 0x800) != 0) ||
           (control->selectedSubresourceEndExclusive <=
            control->animationFrameOffset + control->selectedSubresourceStart)) {
          control->animationFrameOffset = 0;
        }
        pressStateFlagsField = &(control->selectable).stateFlags;
        *pressStateFlagsField = *pressStateFlagsField | 0x1000;
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
void __thandor_preserve_eax
UiSpriteButtonControl_NonRightRelease
          (UiPointerWheelDelta wheelDelta,UiPixelCoordinate pointerY,UiPixelCoordinate pointerX,
          UiSpriteButtonControl *control)

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
void __thandor_preserve_eax_edx
UiSpriteButtonControl_NonRightDrag
          (UiPointerWheelDelta wheelDelta,UiPixelCoordinate pointerY,UiPixelCoordinate pointerX,
          UiSpriteButtonControl *control)

{
  bool opaquePixelHit;
  bool spritePixelHit;
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
      if (((control->selectable).stateFlags & 0x400) == 0) {
        spritePixelHit = (*g_GraphicsTextureSourceTestOpaquePixel)
                          (pointerY,pointerX,(control->selectable).base.top,
                           (control->selectable).base.left,
                           control->normalSubresourceStartOrDescriptor,control->primaryTextureSource
                          );
        if (spritePixelHit) {
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
        spritePixelHit = (*g_GraphicsTextureSourceTestOpaquePixel)
                          (pointerY,pointerX,(control->selectable).base.top,
                           (control->selectable).base.left,control->selectedSubresourceStart,
                           control->primaryTextureSource);
        if (spritePixelHit) goto UiSpriteButtonControl_NonRightDrag_SetPointerInsideStateAndInvalidate;
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
UiNodeBase * __thandor_eax_preserve_ecx_edx
UiSpriteButtonControl_HitTestOpaque
          (UiPixelCoordinate pointerY,UiPixelCoordinate pointerX,UiSpriteButtonControl *control)

{
  UiNodeBase *opaqueHitNode;
  UiSpriteButtonControl *hitNode;
  bool opaquePixelHit;
  bool spritePixelHit;
  
  hitNode = (UiSpriteButtonControl *)0xffffffff;
  if ((((control->selectable).base.nodeFlags & UI_NODE_SUPPRESSED) == 0) &&
     (hitNode = control, ((control->selectable).stateFlags & 0x20) == 0)) {
    if (((control->selectable).stateFlags & 0x400) == 0) {
      if (control->primaryTextureSource == (GraphicsTextureSourceAsset *)0x0) {
        hitNode = (UiSpriteButtonControl *)0xffffffff;
        return (UiNodeBase *)hitNode;
      }
      spritePixelHit = (*g_GraphicsTextureSourceTestOpaquePixel)
                        (pointerY,pointerX,(control->selectable).base.top,
                         (control->selectable).base.left,control->normalSubresourceStartOrDescriptor
                         ,control->primaryTextureSource);
    }
    else {
      spritePixelHit = (*g_GraphicsTextureSourceTestOpaquePixel)
                        (pointerY,pointerX,(control->selectable).base.top,
                         (control->selectable).base.left,control->selectedSubresourceStart,
                         control->primaryTextureSource);
    }
    if (!spritePixelHit) {
      hitNode = (UiSpriteButtonControl *)0xffffffff;
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
void __thandor_void_preserve_eax_ecx_edx
UiImageActionControl_DrawImageAndChildren
          (UiPixelCoordinate clipTop,UiPixelCoordinate clipLeft,UiPixelCoordinate clipBottom,
          UiPixelCoordinate clipRight,UiNodeBase *control)

{
  int imageBottom;
  uint sourceWidth;
  int drawWidth;
  dword scaledHeight;
  uint horizontalMargin;
  int imageLeft;
  int imageTop;
  bool accessFailed;
  
  if (((control->nodeFlags & UI_NODE_SUPPRESSED) == 0) && (control[1].parent != (UiNodeBase *)0x0))
  {
    accessFailed = (*g_GraphicsFramebufferBeginAccess)();
    if (!accessFailed) {
      if (((uint)control[1].nextSibling & 1) == 0) {
        (*g_GraphicsTextureSourceBlitSourceAlpha)
                  (clipTop,clipLeft,clipBottom,clipRight,control->top,control->left,
                   (dword)control[1].vtable,(GraphicsTextureSourceAsset *)control[1].parent,
                   g_FramebufferAccess);
        (*g_GraphicsFramebufferEndAccess)();
      }
      else {
        sourceWidth = control[1].right;
        horizontalMargin = control->layoutWidth - sourceWidth;
        if (((uint)control->layoutWidth < sourceWidth || horizontalMargin == 0) ||
           (((uint)control[1].nextSibling & 4) == 0)) {
          (*g_GraphicsTextureSourceStretchDirectColorBilinear)
                    (control->layoutHeight,control->layoutWidth,control->top,control->left,
                     (dword)control[1].vtable,(GraphicsTextureSourceAsset *)control[1].parent,
                     g_FramebufferAccess);
          (*g_GraphicsFramebufferEndAccess)();
        }
        else {
          scaledHeight = (dword)(((longlong)(int)sourceWidth * (longlong)control->layoutHeight) /
                        (longlong)control->layoutWidth);
          imageLeft = (horizontalMargin >> 1) + control->left;
          imageTop = (control->layoutHeight - scaledHeight >> 1) + control->top;
          drawWidth = control[1].right;
          imageBottom = scaledHeight + imageTop;
          (*g_GraphicsFramebufferFillRectArgb)
                    (clipTop,clipLeft,clipBottom,clipRight,imageTop,control->right,control->top,
                     control->left,0xff000000,g_FramebufferAccess);
          (*g_GraphicsFramebufferFillRectArgb)
                    (clipTop,clipLeft,clipBottom,clipRight,control->bottom,control->right,imageBottom,
                     control->left,0xff000000,g_FramebufferAccess);
          (*g_GraphicsFramebufferFillRectArgb)
                    (clipTop,clipLeft,clipBottom,clipRight,imageBottom,imageLeft,imageTop,control->left,0xff000000,
                     g_FramebufferAccess);
          (*g_GraphicsFramebufferFillRectArgb)
                    (clipTop,clipLeft,clipBottom,clipRight,imageBottom,control->right,imageTop,imageLeft + drawWidth,
                     0xff000000,g_FramebufferAccess);
          (*g_GraphicsTextureSourceStretchDirectColorBilinear)
                    (scaledHeight,control[1].right,imageTop,imageLeft,(dword)control[1].vtable,
                     (GraphicsTextureSourceAsset *)control[1].parent,g_FramebufferAccess);
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
GraphicsCursorFrameIndex __thandor_eax_preserve_ecx_edx
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
void __thandor_preserve_eax
UiImageActionControl_EnqueuePrimaryAction
          (UiPointerWheelDelta wheelDelta,UiPixelCoordinate pointerY,UiPixelCoordinate pointerX,
          UiNodeBase *control)

{
  UiActionQueue_Enqueue(control[1].left,control);
  return;
}


/* Address: 0x00515230.
   Ownership: ui/controls/buttons.
   Purpose: Binary entry is anchored by g_UiNodeVtable_00514FC0[6]@00514FC0.
   Cross-module calls: UiActionQueue_Enqueue [ui/core/runtime].
*/
void __thandor_preserve_eax
UiImageActionControl_EnqueueSecondaryAction
          (UiPointerWheelDelta wheelDelta,UiPixelCoordinate pointerY,UiPixelCoordinate pointerX,
          UiNodeBase *control)

{
  UiActionQueue_Enqueue(control[1].top,control);
  return;
}


/* Address: 0x00515250.
   Ownership: ui/controls/buttons.
   Purpose: Binary entry is anchored by g_UiNodeVtable_00514FC0[12]@00514FC0.
   Cross-module calls: UiKeyboardFocus_MoveNext [ui/controls/input], UiActionQueue_Enqueue [ui/core/runtime].
*/
bool __thandor_cf_preserve_eax_ecx_edx
UiImageActionControl_HandleKeyboardActivationCf
          (UiKeyboardStateMask keyboardStateMask,UiKeyboardEventCode keyCode,UiNodeBase *control)

{
  if (keyCode == 0x10002) {
    UiKeyboardFocus_MoveNext();
    return false;
  }
  if (((uint)control[1].nextSibling & 2) != 0) {
    UiActionQueue_Enqueue(control[1].left,control);
    return false;
  }
  return true;
}


/* Address: 0x005152E0.
   Ownership: ui/controls/buttons.
   Purpose: Handles ui conditional action control draw clipped.
   Cross-module calls: UiWindow_BlitTiledHorizontalEdge [ui/controls/layout], UiWindow_BlitTiledVerticalEdge
   [ui/controls/layout], UiWindow_BlitTiledInterior [ui/controls/layout], RichTextCommandStream_MeasureRegs
   [assets/text/richtext], RichTextCommandStream_DrawSingleLine [assets/text/richtext].
*/
void __thandor_void_preserve_eax_ecx_edx
UiConditionalActionControl_DrawClipped
          (int clipTop,int clipLeft,int clipBottom,int clipRight,UiNodeBase *control)

{
  dword tileEnd;
  int textLeft;
  dword tileStart;
  int drawX;
  int innerHeightOrBottom;
  UiNodeVtable *lineIndexOrCount;
  int innerWidthOrRight;
  bool accessFailed;
  RichTextExtentRegs textExtent;
  GraphicsTextureSizeEaxEdxCf9 cornerSize;
  GraphicsSubresourceIndex backgroundSubresource;
  
  if ((control[1].vtable != (UiNodeVtable *)0x0) &&
     (accessFailed = (*g_GraphicsFramebufferBeginAccess)(), !accessFailed)) {
    backgroundSubresource = 0x7b;
    if ((control->layoutWidth == 0x1a0) && (control->layoutHeight == 0x3a)) {
      backgroundSubresource = 200;
    }
    cornerSize = (*g_GraphicsTextureSourceGetLogicalSize)(0x72,g_UiWindowTextureSource);
    tileStart = cornerSize.logicalHeightPixels;
    tileEnd = cornerSize.logicalWidthPixels;
    innerWidthOrRight = control->layoutWidth;
    innerHeightOrBottom = control->layoutHeight;
    if (backgroundSubresource == 0x7b) {
      innerWidthOrRight = innerWidthOrRight - tileEnd;
      innerHeightOrBottom = innerHeightOrBottom - tileStart;
      (*g_GraphicsTextureSourceBlitSourceAlpha)
                (clipTop,clipLeft,clipBottom,clipRight,control->top,control->left,0x72,
                 g_UiWindowTextureSource,g_FramebufferAccess);
      (*g_GraphicsTextureSourceBlitSourceAlpha)
                (clipTop,clipLeft,clipBottom,clipRight,control->top,innerWidthOrRight + control->left,0x73,
                 g_UiWindowTextureSource,g_FramebufferAccess);
      (*g_GraphicsTextureSourceBlitSourceAlpha)
                (clipTop,clipLeft,clipBottom,clipRight,innerHeightOrBottom + control->top,control->left,0x74,
                 g_UiWindowTextureSource,g_FramebufferAccess);
      (*g_GraphicsTextureSourceBlitSourceAlpha)
                (clipTop,clipLeft,clipBottom,clipRight,innerHeightOrBottom + control->top,innerWidthOrRight + control->left,
                 0x75,g_UiWindowTextureSource,g_FramebufferAccess);
      UiWindow_BlitTiledHorizontalEdge
                (clipTop,clipLeft,clipBottom,clipRight,0x76,innerWidthOrRight,0,tileEnd,control);
      UiWindow_BlitTiledVerticalEdge
                (clipTop,clipLeft,clipBottom,clipRight,0x77,innerHeightOrBottom,tileStart,0,control);
      UiWindow_BlitTiledVerticalEdge
                (clipTop,clipLeft,clipBottom,clipRight,0x78,innerHeightOrBottom,tileStart,innerWidthOrRight,control);
      UiWindow_BlitTiledHorizontalEdge
                (clipTop,clipLeft,clipBottom,clipRight,0x79,innerWidthOrRight,innerHeightOrBottom,tileEnd,control);
      UiWindow_BlitTiledInterior
                (clipTop,clipLeft,clipBottom,clipRight,0x7b,innerHeightOrBottom,innerWidthOrRight,tileStart,tileEnd,control);
    }
    else {
      UiWindow_BlitTiledInterior
                (clipTop,clipLeft,clipBottom,clipRight,backgroundSubresource,innerHeightOrBottom,innerWidthOrRight,0,0,control);
      innerWidthOrRight = innerWidthOrRight - tileEnd;
      innerHeightOrBottom = innerHeightOrBottom - tileStart;
    }
    textLeft = tileEnd + control->left;
    drawX = tileStart + control->top;
    innerWidthOrRight = innerWidthOrRight + control->left;
    innerHeightOrBottom = innerHeightOrBottom + control->top;
    if (clipRight < textLeft) {
      clipRight = textLeft;
    }
    if (clipBottom < drawX) {
      clipBottom = drawX;
    }
    if (innerWidthOrRight < clipLeft) {
      clipLeft = innerWidthOrRight;
    }
    if (innerHeightOrBottom < clipTop) {
      clipTop = innerHeightOrBottom;
    }
    textExtent = RichTextCommandStream_MeasureRegs(g_UiTextStyleNormal,(word *)control[1].left);
    if (backgroundSubresource == 0x7b) {
      lineIndexOrCount = (UiNodeVtable *)0x0;
      do {
        RichTextCommandStream_DrawSingleLine
                  (clipTop,clipLeft,clipBottom,clipRight,g_UiTextStyleNormal,
                   (word *)(&control[1].left)[(int)lineIndexOrCount],drawX,textLeft + 3);
        lineIndexOrCount = (UiNodeVtable *)((int)&lineIndexOrCount->relocate + 1);
        drawX = drawX + textExtent.heightPixels;
      } while (lineIndexOrCount < control[1].vtable);
    }
    else {
      lineIndexOrCount = control[1].vtable;
      if ((UiNodeVtable *)0x4 < lineIndexOrCount) {
        lineIndexOrCount = (UiNodeVtable *)0x4;
      }
      do {
        RichTextCommandStream_DrawSingleLine
                  (clipTop,clipLeft,clipBottom,clipRight,g_UiTextStyleNormal,
                   (word *)(&control[1].vtable)[(int)lineIndexOrCount],drawX,textLeft + 3);
        drawX = drawX + textExtent.heightPixels;
        lineIndexOrCount = (UiNodeVtable *)((int)&lineIndexOrCount[-1].pointerWheel + 3);
      } while (lineIndexOrCount != (UiNodeVtable *)0x0);
    }
    (*g_GraphicsFramebufferEndAccess)();
  }
  return;
}


/* Address: 0x005155A0.
   Ownership: ui/controls/buttons.
   Purpose: Binary entry is anchored by g_UiNodeVtable_00515290[10]@00515290.
*/
GraphicsCursorFrameIndex __thandor_eax_preserve_ecx_edx
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
UiNodeBase * __thandor_eax_preserve_ecx_edx
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
void __thandor_preserve_eax
UiConditionalActionControl_EnqueuePrimaryActionIfEnabled
          (UiPointerWheelDelta wheelDelta,UiPixelCoordinate pointerY,UiPixelCoordinate pointerX,
          UiNodeBase *control)

{
  if (control[1].vtable != (UiNodeVtable *)0x0) {
    UiActionQueue_Enqueue((UiActionId)control[1].parent,control);
  }
  return;
}


/* Address: 0x004B1C80.
   Ownership: ui/controls/buttons.
   Purpose: Advances the active normal or selected frame range, loops at the exclusive end, optionally queues
   actionId on terminal animation state, and invalidates the root.
   Cross-module calls: UiActionQueue_Enqueue [ui/core/runtime], UiNode_InvalidateRoot [ui/core/runtime].
*/
void __thandor_void_preserve_eax_ecx_edx
UiSpriteButtonControl_AdvanceAnimation(UiSpriteButtonControl *control)

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


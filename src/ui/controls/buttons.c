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
   Advances the frame animation of every sprite button below root (depth first), so animated buttons
   keep cycling their frames. Only nodes whose vtable is exactly g_UiSpriteButtonControlVtable count.
*/
/* Rewritten from the assembly (0x004B1D20): a depth-first walk that pushes each node's next
   sibling on the machine stack before descending; the decompiler kept only one level. */
static void UiTree_AdvanceSpriteButtonAnimationsFrom(UiNodeBase *node)
{
  while (node != UI_NODE_NONE) {
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
   Relocate slot of g_UiSpriteButtonControlVtable and the sprite-button vtables at 0x005162C0, 0x00516310
   and 0x00516530. For an animated button it first expands a serialized 8-int descriptor (node rectangle,
   normal and selected frame ranges) and starts the animation on a random normal frame, so buttons of the
   same kind do not animate in lockstep; then the children are relocated.
*/
void __thandor_void_preserve_eax_ecx_edx
UiSpriteButtonControl_Relocate
          (UiSerializedRelocationDelta relocationDelta,UiSpriteButtonControl *control)

{
  UiSelectableStateFlags *stateFlagsField;
  int32_t *sequenceDescriptor;
  int32_t descriptorValue;
  GraphicsSubresourceEndIndex subresourceEnd;
  uint32_t randomValue;
  uint32_t normalFrameCount;

  if (((control->selectable).stateFlags & UI_SPRITE_BUTTON_ANIMATED) != 0) {
    control->animationFrameOffset = 0;
    if (((control->selectable).stateFlags & UI_SPRITE_BUTTON_SERIALIZED_DESCRIPTOR) != 0) {
      sequenceDescriptor = (int32_t *)control->normalSubresourceStartOrDescriptor;
      descriptorValue = sequenceDescriptor[1];
      (control->selectable).base.leftOffset = *sequenceDescriptor;
      (control->selectable).base.topOffset = descriptorValue;
      descriptorValue = sequenceDescriptor[3];
      (control->selectable).base.rightOffset = sequenceDescriptor[2];
      (control->selectable).base.bottomOffset = descriptorValue;
      subresourceEnd = sequenceDescriptor[5];
      control->normalSubresourceStartOrDescriptor = sequenceDescriptor[4];
      control->normalSubresourceEndExclusive = subresourceEnd;
      subresourceEnd = sequenceDescriptor[7];
      control->selectedSubresourceStart = sequenceDescriptor[6];
      control->selectedSubresourceEndExclusive = subresourceEnd;
      stateFlagsField = &(control->selectable).stateFlags;
      *stateFlagsField = *stateFlagsField & ~UI_SPRITE_BUTTON_SERIALIZED_DESCRIPTOR;
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
   drawClipped slot of g_UiSpriteButtonControlVtable and of the sprite-button vtables at 0x005162C0 and
   0x00516310. Draws the current frame (normal or selected, plus the animation offset) twice: first as a
   half-transparent black shadow shifted by the state's drawOffsets, then the sprite itself, optionally
   over the normal frame (NORMAL_UNDER_SELECTED).
*/
void __thandor_void_preserve_eax_ecx_edx
UiSpriteButtonControl_DrawClipped
          (UiPixelCoordinate clipTop,UiPixelCoordinate clipLeft,UiPixelCoordinate clipBottom,
          UiPixelCoordinate clipRight,UiSpriteButtonControl *control)

{
  int8_t shadowOffsetX;
  int8_t shadowOffsetY;
  bool accessFailed;
  uint32_t underlaySubresource;
  uint32_t subresourceIndex;
  GraphicsTextureSourceAsset *textureSource;
  SoftwareFramebufferAccess *framebufferAccess;

  if ((((control->selectable).base.nodeFlags & UI_NODE_SUPPRESSED) == 0) &&
     (((((control->selectable).stateFlags & UI_SELECTABLE_SELECTED_OR_CHECKED) != 0 ||
       (((control->selectable).stateFlags & UI_SPRITE_BUTTON_SELECTED_ONLY) == 0)) &&
      (control->primaryTextureSource != NULL)))) {
    accessFailed = g_GraphicsFramebufferBeginAccess();
    if (!accessFailed) {
      if (((control->selectable).stateFlags & UI_SELECTABLE_SELECTED_OR_CHECKED) == 0) {
        shadowOffsetX = (control->drawOffsets).normalX;
        shadowOffsetY = (control->drawOffsets).normalY;
      }
      else {
        shadowOffsetX = (control->drawOffsets).selectedX;
        shadowOffsetY = (control->drawOffsets).selectedY;
      }
      textureSource = control->primaryTextureSource;
      if (((control->selectable).stateFlags & UI_SELECTABLE_SELECTED_OR_CHECKED) == 0) {
        subresourceIndex = control->normalSubresourceStartOrDescriptor;
      }
      else {
        if ((((control->selectable).stateFlags & UI_SPRITE_BUTTON_ANIMATED) == 0) &&
           (((control->selectable).stateFlags & UI_SPRITE_BUTTON_ALTERNATE_SELECTED_TEXTURE) != 0)) {
          textureSource = control->alternateTextureSource;
        }
        subresourceIndex = control->selectedSubresourceStart;
      }
      if (((control->selectable).stateFlags & UI_SPRITE_BUTTON_ANIMATED) != 0) {
        subresourceIndex = subresourceIndex + control->animationFrameOffset;
      }
      g_GraphicsTextureSourceBlitModulatedSourceAlpha
                (clipTop,clipLeft,clipBottom,clipRight,(int)shadowOffsetY + (control->selectable).base.top,
                 (int)shadowOffsetX + (control->selectable).base.left,UI_SPRITE_BUTTON_SHADOW_ARGB,
                 subresourceIndex,textureSource,g_FramebufferAccess);
      textureSource = control->primaryTextureSource;
      framebufferAccess = g_FramebufferAccess;
      if (((control->selectable).stateFlags & UI_SELECTABLE_SELECTED_OR_CHECKED) == 0) {
        subresourceIndex = control->normalSubresourceStartOrDescriptor;
      }
      else {
        if ((((control->selectable).stateFlags & UI_SPRITE_BUTTON_ANIMATED) == 0) &&
           (((control->selectable).stateFlags & UI_SPRITE_BUTTON_ALTERNATE_SELECTED_TEXTURE) != 0)) {
          textureSource = control->alternateTextureSource;
        }
        subresourceIndex = control->selectedSubresourceStart;
        if (((control->selectable).stateFlags & UI_SPRITE_BUTTON_NORMAL_UNDER_SELECTED) != 0) {
          underlaySubresource = control->normalSubresourceStartOrDescriptor;
          if (((control->selectable).stateFlags & UI_SPRITE_BUTTON_ANIMATED) != 0) {
            underlaySubresource = underlaySubresource + control->animationFrameOffset;
          }
          g_GraphicsTextureSourceBlitSourceAlpha
                    (clipTop,clipLeft,clipBottom,clipRight,(control->selectable).base.top,
                     (control->selectable).base.left,underlaySubresource,control->primaryTextureSource,
                     g_FramebufferAccess);
        }
      }
      if (((control->selectable).stateFlags & UI_SPRITE_BUTTON_ANIMATED) != 0) {
        subresourceIndex = subresourceIndex + control->animationFrameOffset;
      }
      g_GraphicsTextureSourceBlitSourceAlpha
                (clipTop,clipLeft,clipBottom,clipRight,(control->selectable).base.top,
                 (control->selectable).base.left,subresourceIndex,textureSource,framebufferAccess);
      g_GraphicsFramebufferEndAccess();
    }
  }
  return;
}


/* Address: 0x004B1890.
   nonRightPress slot of g_UiSpriteButtonControlVtable. A momentary button only shows its pressed frame
   (the action follows on release); a persistent one toggles (TOGGLE_ON_ACTIVATION) or latches selected,
   plays its activation sound and queues actionId, deferred to the animation end for ACTION_AFTER_ANIMATION.
*/
void __thandor_void_preserve_eax_ecx_edx
UiSpriteButtonControl_NonRightPress
          (UiPointerWheelDelta wheelDelta,UiPixelCoordinate pointerY,UiPixelCoordinate pointerX,
          UiSpriteButtonControl *control)

{
  UiSelectableStateFlags *pressStateFlagsField;
  UiSelectableStateFlags *selectionStateFlagsField;
  UiSelectableStateFlags *stateFlagsField;
  bool queueAction;

  if (((control->selectable).base.nodeFlags & UI_NODE_SUPPRESSED) == 0) {
    if (((control->selectable).stateFlags & UI_SELECTABLE_PERSISTENT_ACTIVATION_MODE) == 0) {
      stateFlagsField = &(control->selectable).stateFlags;
      *stateFlagsField = *stateFlagsField | UI_SELECTABLE_SELECTED_OR_CHECKED;
      if ((((control->selectable).stateFlags & UI_SPRITE_BUTTON_ANIMATED) != 0) &&
         ((((control->selectable).stateFlags & UI_SPRITE_BUTTON_ACTION_AFTER_ANIMATION) != 0 ||
          (control->selectedSubresourceEndExclusive <=
           control->animationFrameOffset + control->selectedSubresourceStart)))) {
        control->animationFrameOffset = 0;
      }
      UiNode_InvalidateRoot((UiNodeBase *)control);
      return;
    }
    if (((control->selectable).stateFlags & UI_SELECTABLE_TOGGLE_ON_ACTIVATION) != 0) {
      if ((((control->selectable).stateFlags & UI_SPRITE_BUTTON_ACTIVATION_SOUND) != 0) && (control->activationSoundId != 0)) {
        g_SoundPlayOneShot
                  (g_UiSoundGainQ15,g_UiSoundGainQ15,
                   (DirectSoundVoiceSet *)control->activationSoundId);
      }
      selectionStateFlagsField = &(control->selectable).stateFlags;
      *selectionStateFlagsField = *selectionStateFlagsField ^ UI_SELECTABLE_SELECTED_OR_CHECKED;
      queueAction = true;
      if (((control->selectable).stateFlags & UI_SPRITE_BUTTON_ANIMATED) != 0) {
        if ((((control->selectable).stateFlags & UI_SPRITE_BUTTON_ACTION_AFTER_ANIMATION) != 0) ||
           (control->selectedSubresourceEndExclusive <=
            control->animationFrameOffset + control->selectedSubresourceStart)) {
          control->animationFrameOffset = 0;
        }
        pressStateFlagsField = &(control->selectable).stateFlags;
        *pressStateFlagsField = *pressStateFlagsField | UI_SPRITE_BUTTON_ACTION_PENDING;
        /* ACTION_AFTER_ANIMATION: UiSpriteButtonControl_AdvanceAnimation queues it on the last frame. */
        queueAction = ((control->selectable).stateFlags & UI_SPRITE_BUTTON_ACTION_AFTER_ANIMATION) == 0;
      }
      if (queueAction) {
        UiActionQueue_Enqueue((control->selectable).actionId,control);
      }
      UiNode_InvalidateRoot((UiNodeBase *)control);
      return;
    }
    if (((control->selectable).stateFlags & UI_SELECTABLE_SELECTED_OR_CHECKED) == 0) {
      if ((((control->selectable).stateFlags & UI_SPRITE_BUTTON_ACTIVATION_SOUND) != 0) && (control->activationSoundId != 0)) {
        g_SoundPlayOneShot
                  (g_UiSoundGainQ15,g_UiSoundGainQ15,
                   (DirectSoundVoiceSet *)control->activationSoundId);
      }
      pressStateFlagsField = &(control->selectable).stateFlags;
      *pressStateFlagsField = *pressStateFlagsField | UI_SELECTABLE_SELECTED_OR_CHECKED;
      queueAction = true;
      if (((control->selectable).stateFlags & UI_SPRITE_BUTTON_ANIMATED) != 0) {
        if ((((control->selectable).stateFlags & UI_SPRITE_BUTTON_ACTION_AFTER_ANIMATION) != 0) ||
           (control->selectedSubresourceEndExclusive <=
            control->animationFrameOffset + control->selectedSubresourceStart)) {
          control->animationFrameOffset = 0;
        }
        pressStateFlagsField = &(control->selectable).stateFlags;
        *pressStateFlagsField = *pressStateFlagsField | UI_SPRITE_BUTTON_ACTION_PENDING;
        queueAction = ((control->selectable).stateFlags & UI_SPRITE_BUTTON_ACTION_AFTER_ANIMATION) == 0;
      }
      if (queueAction) {
        UiActionQueue_Enqueue((control->selectable).actionId,control);
      }
      UiNode_InvalidateRoot((UiNodeBase *)control);
    }
  }
  return;
}


/* Address: 0x004B1A30.
   nonRightRelease slot of g_UiSpriteButtonControlVtable. Completes the click of a momentary button: if it
   is still shown pressed (the pointer was released over it), it plays the activation sound, drops the
   pressed state and queues actionId. Persistent buttons act on press instead.
*/
void __thandor_preserve_eax
UiSpriteButtonControl_NonRightRelease
          (UiPointerWheelDelta wheelDelta,UiPixelCoordinate pointerY,UiPixelCoordinate pointerX,
          UiSpriteButtonControl *control)

{
  UiSelectableStateFlags *stateFlagsField;
  
  if ((((control->selectable).base.nodeFlags & UI_NODE_SUPPRESSED) == 0) &&
     (((control->selectable).stateFlags & UI_SELECTABLE_PERSISTENT_ACTIVATION_MODE) == 0)) {
    if ((((control->selectable).stateFlags & UI_SPRITE_BUTTON_ANIMATED) != 0) &&
       ((((control->selectable).stateFlags & UI_SPRITE_BUTTON_ACTION_AFTER_ANIMATION) != 0 ||
        (control->normalSubresourceEndExclusive <=
         control->animationFrameOffset + control->normalSubresourceStartOrDescriptor)))) {
      control->animationFrameOffset = 0;
    }
    if (((control->selectable).stateFlags & UI_SELECTABLE_SELECTED_OR_CHECKED) != 0) {
      if ((((control->selectable).stateFlags & UI_SPRITE_BUTTON_ACTIVATION_SOUND) != 0) && (control->activationSoundId != 0)) {
        g_SoundPlayOneShot
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
   nonRightDrag slot of g_UiSpriteButtonControlVtable, and both drag slots of the sprite-button vtables at
   0x005162C0, 0x00516310 and 0x00516530. While a momentary, non-animated button holds the
   pointer, it shows the pressed state only while the pointer is over the button (opaque sprite pixel or
   node rectangle), so dragging off cancels the click.
*/
void __thandor_preserve_eax_edx
UiSpriteButtonControl_NonRightDrag
          (UiPointerWheelDelta wheelDelta,UiPixelCoordinate pointerY,UiPixelCoordinate pointerX,
          UiSpriteButtonControl *control)

{
  bool pointerInside;
  UiSelectableStateFlags *selectedStateFlagsField;
  UiSelectableStateFlags *stateFlagsField;
  
  if (((control->selectable).base.nodeFlags & UI_NODE_SUPPRESSED) != 0) {
    return;
  }
  if (((control->selectable).stateFlags & UI_SELECTABLE_PERSISTENT_ACTIVATION_MODE) != 0) {
    return;
  }
  if (((control->selectable).stateFlags & UI_SPRITE_BUTTON_ANIMATED) != 0) {
    return;
  }
  if (((control->selectable).stateFlags & UI_SPRITE_BUTTON_RECT_HIT_TEST) == 0) {
    pointerInside = false;
    if (control->primaryTextureSource != NULL) {
      if (((control->selectable).stateFlags & UI_SPRITE_BUTTON_SELECTED_ONLY) == 0) {
        pointerInside = g_GraphicsTextureSourceTestOpaquePixel
                          (pointerY,pointerX,(control->selectable).base.top,
                           (control->selectable).base.left,
                           control->normalSubresourceStartOrDescriptor,control->primaryTextureSource
                          );
      }
      else {
        pointerInside = g_GraphicsTextureSourceTestOpaquePixel
                          (pointerY,pointerX,(control->selectable).base.top,
                           (control->selectable).base.left,control->selectedSubresourceStart,
                           control->primaryTextureSource);
      }
    }
  }
  else {
    pointerInside =
         (((control->selectable).base.left <= pointerX) &&
          ((control->selectable).base.top <= pointerY)) &&
         (pointerX < (control->selectable).base.right) &&
         (pointerY < (control->selectable).base.bottom);
  }
  if (pointerInside) {
    /* Pointer inside: show the pressed state. */
    if (((control->selectable).stateFlags & UI_SELECTABLE_SELECTED_OR_CHECKED) != 0) {
      return;
    }
    stateFlagsField = &(control->selectable).stateFlags;
    *stateFlagsField = *stateFlagsField | UI_SELECTABLE_SELECTED_OR_CHECKED;
    UiNode_InvalidateRoot((UiNodeBase *)control);
    return;
  }
  if (((control->selectable).stateFlags & UI_SELECTABLE_SELECTED_OR_CHECKED) != 0) {
    selectedStateFlagsField = &(control->selectable).stateFlags;
    *selectedStateFlagsField = *selectedStateFlagsField & ~UI_SELECTABLE_SELECTED_OR_CHECKED;
    UiNode_InvalidateRoot((UiNodeBase *)control);
  }
  return;
}


/* Address: 0x004B1BF0.
   hitTest slot of g_UiSpriteButtonControlVtable and of the sprite-button vtables at 0x005162C0, 0x00516310
   and 0x00516530. Returns the button when the point lies on an opaque pixel of its normal frame (selected
   frame for SELECTED_ONLY buttons); RECT_HIT_TEST buttons accept the whole node (the caller has already
   checked the rectangle). Otherwise UI_NODE_NONE.
*/
UiNodeBase * __thandor_eax_preserve_ecx_edx
UiSpriteButtonControl_HitTestOpaque
          (UiPixelCoordinate pointerY,UiPixelCoordinate pointerX,UiSpriteButtonControl *control)

{
  UiSpriteButtonControl *hitNode;
  bool spritePixelHit;
  
  hitNode = (UiSpriteButtonControl *)UI_NODE_NONE;
  if ((((control->selectable).base.nodeFlags & UI_NODE_SUPPRESSED) == 0) &&
     (hitNode = control, ((control->selectable).stateFlags & UI_SPRITE_BUTTON_RECT_HIT_TEST) == 0)) {
    if (((control->selectable).stateFlags & UI_SPRITE_BUTTON_SELECTED_ONLY) == 0) {
      if (control->primaryTextureSource == NULL) {
        hitNode = (UiSpriteButtonControl *)UI_NODE_NONE;
        return (UiNodeBase *)hitNode;
      }
      spritePixelHit = g_GraphicsTextureSourceTestOpaquePixel
                        (pointerY,pointerX,(control->selectable).base.top,
                         (control->selectable).base.left,control->normalSubresourceStartOrDescriptor
                         ,control->primaryTextureSource);
    }
    else {
      spritePixelHit = g_GraphicsTextureSourceTestOpaquePixel
                        (pointerY,pointerX,(control->selectable).base.top,
                         (control->selectable).base.left,control->selectedSubresourceStart,
                         control->primaryTextureSource);
    }
    if (!spritePixelHit) {
      hitNode = (UiSpriteButtonControl *)UI_NODE_NONE;
      return (UiNodeBase *)hitNode;
    }
  }
  return (UiNodeBase *)hitNode;
}


/* Address: 0x00515010.
   drawClipped slot of g_UiImageActionControlVtable (briefing image, movie views). Draws the image 1:1, or
   with UI_IMAGE_ACTION_STRETCH bilinearly stretched over the node; with UI_IMAGE_ACTION_LETTERBOX and a
   letterboxWidth narrower than the node it is scaled to that width, centred and framed by black bars.
   Children are drawn on top.
*/
void __thandor_void_preserve_eax_ecx_edx
UiImageActionControl_DrawImageAndChildren
          (UiPixelCoordinate clipTop,UiPixelCoordinate clipLeft,UiPixelCoordinate clipBottom,
          UiPixelCoordinate clipRight,UiImageActionControl *control)

{
  int imageBottom;
  uint32_t letterboxWidth;
  int drawWidth;
  uint32_t scaledHeight;
  uint32_t horizontalMargin;
  int imageLeft;
  int imageTop;
  bool accessFailed;

  if (((control->base.nodeFlags & UI_NODE_SUPPRESSED) == 0) && (control->textureSource != NULL))
  {
    accessFailed = g_GraphicsFramebufferBeginAccess();
    if (!accessFailed) {
      if ((control->displayFlags & UI_IMAGE_ACTION_STRETCH) == 0) {
        g_GraphicsTextureSourceBlitSourceAlpha
                  (clipTop,clipLeft,clipBottom,clipRight,control->base.top,control->base.left,
                   control->subresource,control->textureSource,
                   g_FramebufferAccess);
        g_GraphicsFramebufferEndAccess();
      }
      else {
        letterboxWidth = control->letterboxWidth;
        horizontalMargin = control->base.layoutWidth - letterboxWidth;
        if (((uint32_t)control->base.layoutWidth < letterboxWidth || horizontalMargin == 0) ||
           ((control->displayFlags & UI_IMAGE_ACTION_LETTERBOX) == 0)) {
          g_GraphicsTextureSourceStretchDirectColorBilinear
                    (control->base.layoutHeight,control->base.layoutWidth,control->base.top,control->base.left,
                     control->subresource,control->textureSource,
                     g_FramebufferAccess);
          g_GraphicsFramebufferEndAccess();
        }
        else {
          /* keep the node's aspect ratio at letterboxWidth */
          scaledHeight = (uint32_t)(((int64_t)(int)letterboxWidth * (int64_t)control->base.layoutHeight) /
                        (int64_t)control->base.layoutWidth);
          imageLeft = (horizontalMargin >> 1) + control->base.left;
          imageTop = ((control->base.layoutHeight - scaledHeight) >> 1) + control->base.top;
          drawWidth = control->letterboxWidth;
          imageBottom = scaledHeight + imageTop;
          /* black bars above, below, left and right of the image */
          g_GraphicsFramebufferFillRectArgb
                    (clipTop,clipLeft,clipBottom,clipRight,imageTop,control->base.right,control->base.top,
                     control->base.left,UI_IMAGE_ACTION_LETTERBOX_BAR_ARGB,g_FramebufferAccess);
          g_GraphicsFramebufferFillRectArgb
                    (clipTop,clipLeft,clipBottom,clipRight,control->base.bottom,control->base.right,imageBottom,
                     control->base.left,UI_IMAGE_ACTION_LETTERBOX_BAR_ARGB,g_FramebufferAccess);
          g_GraphicsFramebufferFillRectArgb
                    (clipTop,clipLeft,clipBottom,clipRight,imageBottom,imageLeft,imageTop,control->base.left,
                     UI_IMAGE_ACTION_LETTERBOX_BAR_ARGB,g_FramebufferAccess);
          g_GraphicsFramebufferFillRectArgb
                    (clipTop,clipLeft,clipBottom,clipRight,imageBottom,control->base.right,imageTop,imageLeft + drawWidth,
                     UI_IMAGE_ACTION_LETTERBOX_BAR_ARGB,g_FramebufferAccess);
          g_GraphicsTextureSourceStretchDirectColorBilinear
                    (scaledHeight,control->letterboxWidth,imageTop,imageLeft,control->subresource,
                     control->textureSource,g_FramebufferAccess);
          g_GraphicsFramebufferEndAccess();
        }
      }
    }
  }
  UiContainer_DrawIntersectingChildren(clipTop,clipLeft,clipBottom,clipRight,&control->base);
  return;
}


/* Address: 0x005151F0.
   pointerMove slot of g_UiImageActionControlVtable: returns the control's cursor frame for the pointer.
*/
GraphicsCursorFrameIndex __thandor_eax_preserve_ecx_edx
UiImageActionControl_QueryPointerCode
          (UiPixelCoordinate pointerY,UiPixelCoordinate pointerX,UiImageActionControl *control)

{
  return control->cursorFrame;
}


/* Address: 0x00515210.
   nonRightPress slot of g_UiImageActionControlVtable: a left click queues primaryActionId.
*/
void __thandor_preserve_eax
UiImageActionControl_EnqueuePrimaryAction
          (UiPointerWheelDelta wheelDelta,UiPixelCoordinate pointerY,UiPixelCoordinate pointerX,
          UiImageActionControl *control)

{
  UiActionQueue_Enqueue(control->primaryActionId,&control->base);
  return;
}


/* Address: 0x00515230.
   rightPress slot of g_UiImageActionControlVtable: a right click queues secondaryActionId.
*/
void __thandor_preserve_eax
UiImageActionControl_EnqueueSecondaryAction
          (UiPointerWheelDelta wheelDelta,UiPixelCoordinate pointerY,UiPixelCoordinate pointerX,
          UiImageActionControl *control)

{
  UiActionQueue_Enqueue(control->secondaryActionId,&control->base);
  return;
}


/* Address: 0x00515250.
   keyboardEvent slot of g_UiImageActionControlVtable. Tab moves the keyboard focus on; with
   UI_IMAGE_ACTION_KEY_ACTIVATES any other key queues primaryActionId, like a left click.
   CF clear when the key was consumed, set to pass it on.
*/
bool __thandor_cf_preserve_eax_ecx_edx
UiImageActionControl_HandleKeyboardActivation
          (UiKeyboardStateMask keyboardStateMask,UiKeyboardEventCode keyCode,UiImageActionControl *control)

{
  if (keyCode == KEYBOARD_KEY_CODE_TAB) {
    UiKeyboardFocus_MoveNext();
    return false;
  }
  if ((control->displayFlags & UI_IMAGE_ACTION_KEY_ACTIVATES) != 0) {
    UiActionQueue_Enqueue(control->primaryActionId,&control->base);
    return false;
  }
  return true;
}


/* Address: 0x005152E0.
   drawClipped slot of g_UiConditionalActionControlVtable. Draws nothing while the box has no text lines.
   Otherwise it draws a tiled window frame (or, for a 416x58 box, one unframed background image) and the
   rich-text lines inside the frame, clipped to the inner area. The 416x58 variant shows at most 4 lines,
   last line first.
*/
void __thandor_void_preserve_eax_ecx_edx
UiConditionalActionControl_DrawClipped
          (int clipTop,int clipLeft,int clipBottom,int clipRight,UiConditionalActionControl *control)

{
  uint32_t cornerWidth;
  int textLeft;
  uint32_t cornerHeight;
  int lineTop;
  int innerHeightOrBottom;
  uint32_t lineIndexOrCount;
  int innerWidthOrRight;
  bool accessFailed;
  RichTextExtentRegs textExtent;
  TextureSizeResult cornerSize;
  GraphicsSubresourceIndex backgroundSubresource;

  if ((control->lineCount != 0) &&
     (accessFailed = g_GraphicsFramebufferBeginAccess(), !accessFailed)) {
    backgroundSubresource = UI_TEXT_BOX_SUBRESOURCE_INTERIOR;
    if ((control->base.layoutWidth == 416) && (control->base.layoutHeight == 58)) {
      backgroundSubresource = UI_TEXT_BOX_SUBRESOURCE_WIDE_BACKGROUND;
    }
    cornerSize = g_GraphicsTextureSourceGetLogicalSize(UI_TEXT_BOX_SUBRESOURCE_TOP_LEFT,g_UiWindowTextureSource);
    cornerHeight = cornerSize.logicalHeightPixels;
    cornerWidth = cornerSize.logicalWidthPixels;
    innerWidthOrRight = control->base.layoutWidth;
    innerHeightOrBottom = control->base.layoutHeight;
    if (backgroundSubresource == UI_TEXT_BOX_SUBRESOURCE_INTERIOR) {
      innerWidthOrRight = innerWidthOrRight - cornerWidth;
      innerHeightOrBottom = innerHeightOrBottom - cornerHeight;
      g_GraphicsTextureSourceBlitSourceAlpha
                (clipTop,clipLeft,clipBottom,clipRight,control->base.top,control->base.left,
                 UI_TEXT_BOX_SUBRESOURCE_TOP_LEFT,g_UiWindowTextureSource,g_FramebufferAccess);
      g_GraphicsTextureSourceBlitSourceAlpha
                (clipTop,clipLeft,clipBottom,clipRight,control->base.top,innerWidthOrRight + control->base.left,
                 UI_TEXT_BOX_SUBRESOURCE_TOP_RIGHT,g_UiWindowTextureSource,g_FramebufferAccess);
      g_GraphicsTextureSourceBlitSourceAlpha
                (clipTop,clipLeft,clipBottom,clipRight,innerHeightOrBottom + control->base.top,control->base.left,
                 UI_TEXT_BOX_SUBRESOURCE_BOTTOM_LEFT,g_UiWindowTextureSource,g_FramebufferAccess);
      g_GraphicsTextureSourceBlitSourceAlpha
                (clipTop,clipLeft,clipBottom,clipRight,innerHeightOrBottom + control->base.top,innerWidthOrRight + control->base.left,
                 UI_TEXT_BOX_SUBRESOURCE_BOTTOM_RIGHT,g_UiWindowTextureSource,g_FramebufferAccess);
      UiWindow_BlitTiledHorizontalEdge
                (clipTop,clipLeft,clipBottom,clipRight,UI_TEXT_BOX_SUBRESOURCE_TOP,innerWidthOrRight,0,cornerWidth,
                 &control->base);
      UiWindow_BlitTiledVerticalEdge
                (clipTop,clipLeft,clipBottom,clipRight,UI_TEXT_BOX_SUBRESOURCE_LEFT,innerHeightOrBottom,cornerHeight,0,
                 &control->base);
      UiWindow_BlitTiledVerticalEdge
                (clipTop,clipLeft,clipBottom,clipRight,UI_TEXT_BOX_SUBRESOURCE_RIGHT,innerHeightOrBottom,cornerHeight,
                 innerWidthOrRight,&control->base);
      UiWindow_BlitTiledHorizontalEdge
                (clipTop,clipLeft,clipBottom,clipRight,UI_TEXT_BOX_SUBRESOURCE_BOTTOM,innerWidthOrRight,
                 innerHeightOrBottom,cornerWidth,&control->base);
      UiWindow_BlitTiledInterior
                (clipTop,clipLeft,clipBottom,clipRight,UI_TEXT_BOX_SUBRESOURCE_INTERIOR,innerHeightOrBottom,
                 innerWidthOrRight,cornerHeight,cornerWidth,&control->base);
    }
    else {
      UiWindow_BlitTiledInterior
                (clipTop,clipLeft,clipBottom,clipRight,backgroundSubresource,innerHeightOrBottom,innerWidthOrRight,0,0,&control->base);
      innerWidthOrRight = innerWidthOrRight - cornerWidth;
      innerHeightOrBottom = innerHeightOrBottom - cornerHeight;
    }
    textLeft = cornerWidth + control->base.left;
    lineTop = cornerHeight + control->base.top;
    innerWidthOrRight = innerWidthOrRight + control->base.left;
    innerHeightOrBottom = innerHeightOrBottom + control->base.top;
    /* narrow the clip rectangle to the area inside the frame (the clip parameters are named in reverse
       order: clipRight/clipBottom act as the left/top bound here) */
    if (clipRight < textLeft) {
      clipRight = textLeft;
    }
    if (clipBottom < lineTop) {
      clipBottom = lineTop;
    }
    if (innerWidthOrRight < clipLeft) {
      clipLeft = innerWidthOrRight;
    }
    if (innerHeightOrBottom < clipTop) {
      clipTop = innerHeightOrBottom;
    }
    textExtent = RichTextCommandStream_MeasureRegs(g_UiTextStyleNormal,control->textLines[0]);
    if (backgroundSubresource == UI_TEXT_BOX_SUBRESOURCE_INTERIOR) {
      lineIndexOrCount = 0;
      do {
        RichTextCommandStream_DrawSingleLine
                  (clipTop,clipLeft,clipBottom,clipRight,g_UiTextStyleNormal,
                   control->textLines[lineIndexOrCount],lineTop,textLeft + 3);
        lineIndexOrCount = lineIndexOrCount + 1;
        lineTop = lineTop + textExtent.heightPixels;
      } while (lineIndexOrCount < control->lineCount);
    }
    else {
      lineIndexOrCount = control->lineCount;
      if (4 < lineIndexOrCount) {
        lineIndexOrCount = 4;
      }
      do {
        RichTextCommandStream_DrawSingleLine
                  (clipTop,clipLeft,clipBottom,clipRight,g_UiTextStyleNormal,
                   control->textLines[lineIndexOrCount - 1],lineTop,textLeft + 3);
        lineTop = lineTop + textExtent.heightPixels;
        lineIndexOrCount = lineIndexOrCount - 1;
      } while (lineIndexOrCount != 0);
    }
    g_GraphicsFramebufferEndAccess();
  }
  return;
}


/* Address: 0x005155A0.
   pointerMove slot of g_UiConditionalActionControlVtable: returns the control's cursor frame.
*/
GraphicsCursorFrameIndex __thandor_eax_preserve_ecx_edx
UiConditionalActionControl_QueryPointerCode
          (UiPixelCoordinate pointerY,UiPixelCoordinate pointerX,UiConditionalActionControl *control)

{
  return control->cursorFrame;
}


/* Address: 0x005155C0.
   hitTest slot of g_UiConditionalActionControlVtable. An empty box (no text lines) is invisible and
   returns UI_NODE_NONE; otherwise the normal child hit test applies.
*/
UiNodeBase * __thandor_eax_preserve_ecx_edx
UiConditionalActionControl_HitTestWhenEnabled
          (UiPixelCoordinate pointerY,UiPixelCoordinate pointerX,UiConditionalActionControl *control)

{
  UiNodeBase *hitNode;

  hitNode = UI_NODE_NONE;
  if (control->lineCount != 0) {
    hitNode = UiContainer_HitTestChildren(pointerY,pointerX,&control->base);
  }
  return hitNode;
}


/* Address: 0x005155F0.
   nonRightPress slot of g_UiConditionalActionControlVtable: a left click queues actionId, but only while
   the box shows text.
*/
void __thandor_preserve_eax
UiConditionalActionControl_EnqueuePrimaryActionIfEnabled
          (UiPointerWheelDelta wheelDelta,UiPixelCoordinate pointerY,UiPixelCoordinate pointerX,
          UiConditionalActionControl *control)

{
  if (control->lineCount != 0) {
    UiActionQueue_Enqueue(control->actionId,&control->base);
  }
  return;
}


/* Address: 0x004B1C80.
   Advances an animated sprite button by one frame within its normal or selected frame range, wrapping to
   the first frame. On the last frame a deferred activation action is queued (and cleared), then the UI is
   redrawn.
*/
void __thandor_void_preserve_eax_ecx_edx
UiSpriteButtonControl_AdvanceAnimation(UiSpriteButtonControl *control)

{
  uint32_t subresourceStart;
  uint32_t subresourceEndExclusive;
  UiSelectableStateFlags *stateFlagsField;

  if (((control->selectable).stateFlags & UI_SPRITE_BUTTON_ANIMATED) != 0) {
    if (((control->selectable).stateFlags & UI_SELECTABLE_SELECTED_OR_CHECKED) == 0) {
      subresourceStart = control->normalSubresourceStartOrDescriptor;
      subresourceEndExclusive = control->normalSubresourceEndExclusive;
    }
    else {
      subresourceStart = control->selectedSubresourceStart;
      subresourceEndExclusive = control->selectedSubresourceEndExclusive;
    }
    if (subresourceStart + 1 + control->animationFrameOffset < subresourceEndExclusive) {
      control->animationFrameOffset++;
    }
    else {
      control->animationFrameOffset = 0;
    }
    if (subresourceEndExclusive - 1 <= subresourceStart + control->animationFrameOffset) {
      if ((((control->selectable).stateFlags & UI_SPRITE_BUTTON_ACTION_AFTER_ANIMATION) != 0) &&
         (((control->selectable).stateFlags & UI_SPRITE_BUTTON_ACTION_PENDING) != 0)) {
        UiActionQueue_Enqueue((control->selectable).actionId,control);
      }
      stateFlagsField = &(control->selectable).stateFlags;
      *stateFlagsField = *stateFlagsField & ~UI_SPRITE_BUTTON_ACTION_PENDING;
    }
    UiNode_InvalidateRoot((UiNodeBase *)control);
  }
  return;
}


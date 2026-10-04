/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/src/ui/controls/buttons.cpp
 * Reverse engineering by idkFoxes 2026
 */

#include <thandor/ui/controls/buttons.h>
#include <thandor/thandor.h>

/* Implementation ownership: ui/controls/buttons. */

/* Depth-first walk over node, its descendants and its following siblings: each node's subtree is
   visited before its next sibling, at every depth. */
static void UiTree_AdvanceSpriteButtonAnimationsFrom(UiNodeBase *node) {
  while (node != UI_NODE_NONE) {
    if (node->vtable == (UiNodeVtable *)&g_UiSpriteButtonControlVtable) {
      UiSpriteButtonControl_AdvanceAnimation((UiSpriteButtonControl *)node);
    }
    UiTree_AdvanceSpriteButtonAnimationsFrom(node->firstChild);
    node = node->nextSibling;
  }
}

/* Advances the frame animation of every sprite button below root (depth first), so animated buttons
   keep cycling their frames. Only nodes whose vtable is exactly g_UiSpriteButtonControlVtable count.
*/
void UiTree_AdvanceSpriteButtonAnimations(UiNodeBase *root)

{
  UiTree_AdvanceSpriteButtonAnimationsFrom(root->firstChild);
  return;
}

/* Relocate slot of g_UiSpriteButtonControlVtable and the sprite-button vtables g_UiCommandSpriteButtonWithDetailsVtable,
   g_UiCommandSpriteButtonControlVtable and g_UiCatalogEntryControlVtable. For an animated button it first expands a serialized 8-int descriptor (node rectangle,
   normal and selected frame ranges) and starts the animation on a random normal frame, so buttons of the
   same kind do not animate in lockstep; then the children are relocated.
*/
void UiSpriteButtonControl_Relocate(UiSerializedRelocationDelta relocationDelta,UiSpriteButtonControl *control)

{
  int32_t *sequenceDescriptor;
  uint32_t randomValue;
  uint32_t normalFrameCount;

  if (((control->selectable).stateFlags & UI_SPRITE_BUTTON_ANIMATED) != 0) {
    control->animationFrameOffset = 0;
    if (((control->selectable).stateFlags & UI_SPRITE_BUTTON_SERIALIZED_DESCRIPTOR) != 0) {
      sequenceDescriptor = (int32_t *)control->normalSubresourceStartOrDescriptor; /* 5f-format: UiSpriteButtonControl.normalSubresourceStartOrDescriptor (UI template) */
      (control->selectable).base.leftOffset = sequenceDescriptor[0];
      (control->selectable).base.topOffset = sequenceDescriptor[1];
      (control->selectable).base.rightOffset = sequenceDescriptor[2];
      (control->selectable).base.bottomOffset = sequenceDescriptor[3];
      control->normalSubresourceStartOrDescriptor = sequenceDescriptor[4];
      control->normalSubresourceEndExclusive = sequenceDescriptor[5];
      control->selectedSubresourceStart = sequenceDescriptor[6];
      control->selectedSubresourceEndExclusive = sequenceDescriptor[7];
      (control->selectable).stateFlags &= ~UI_SPRITE_BUTTON_SERIALIZED_DESCRIPTOR;
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

/* drawClipped slot of g_UiSpriteButtonControlVtable and of the sprite-button vtables g_UiCommandSpriteButtonWithDetailsVtable
   and g_UiCommandSpriteButtonControlVtable. Draws the current frame (normal or selected, plus the animation offset) twice: first as a
   half-transparent black shadow shifted by the state's drawOffsets, then the sprite itself, optionally
   over the normal frame (NORMAL_UNDER_SELECTED).
*/
void UiSpriteButtonControl_DrawClipped
          (UiPixelCoordinate clipBottom,UiPixelCoordinate clipRight,UiPixelCoordinate clipTop,
          UiPixelCoordinate clipLeft,UiSpriteButtonControl *control)

{
  int8_t shadowOffsetX;
  int8_t shadowOffsetY;
  uint32_t underlaySubresource;
  uint32_t subresourceIndex;
  GraphicsTextureSourceAsset *textureSource;
  SoftwareFramebufferAccess *framebufferAccess;

  if (((control->selectable).base.nodeFlags & UI_NODE_SUPPRESSED) != 0) {
    return;
  }
  /* SELECTED_ONLY buttons are invisible while not selected */
  if (((control->selectable).stateFlags & UI_SELECTABLE_SELECTED_OR_CHECKED) == 0 &&
      ((control->selectable).stateFlags & UI_SPRITE_BUTTON_SELECTED_ONLY) != 0) {
    return;
  }
  if (control->primaryTextureSource == NULL) {
    return;
  }
  if (g_GraphicsFramebufferBeginAccess()) {
    return;
  }

  /* shadow pass */
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
            (clipBottom,clipRight,clipTop,clipLeft,(int)shadowOffsetY + (control->selectable).base.top,
             (int)shadowOffsetX + (control->selectable).base.left,UI_SPRITE_BUTTON_SHADOW_ARGB,
             subresourceIndex,textureSource,g_FramebufferAccess);

  /* sprite pass (the framebuffer access is read before the optional underlay blit) */
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
                (clipBottom,clipRight,clipTop,clipLeft,(control->selectable).base.top,
                 (control->selectable).base.left,underlaySubresource,control->primaryTextureSource,
                 g_FramebufferAccess);
    }
  }
  if (((control->selectable).stateFlags & UI_SPRITE_BUTTON_ANIMATED) != 0) {
    subresourceIndex = subresourceIndex + control->animationFrameOffset;
  }
  g_GraphicsTextureSourceBlitSourceAlpha
            (clipBottom,clipRight,clipTop,clipLeft,(control->selectable).base.top,
             (control->selectable).base.left,subresourceIndex,textureSource,framebufferAccess);
  g_GraphicsFramebufferEndAccess();
  return;
}

/* nonRightPress slot of g_UiSpriteButtonControlVtable. A momentary button only shows its pressed frame
   (the action follows on release); a persistent one toggles (TOGGLE_ON_ACTIVATION) or latches selected,
   plays its activation sound and queues actionId, deferred to the animation end for ACTION_AFTER_ANIMATION.
*/
void UiSpriteButtonControl_NonRightPress
          (UiPointerWheelDelta wheelDelta,UiPixelCoordinate pointerY,UiPixelCoordinate pointerX,
          UiSpriteButtonControl *control)

{
  UiSelectableStateFlags *pressStateFlagsField;
  UiSelectableStateFlags *selectionStateFlagsField;
  UiSelectableStateFlags *stateFlagsField;
  Bool8 queueAction;

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
      if (((control->selectable).stateFlags & UI_SPRITE_BUTTON_ACTIVATION_SOUND) != 0 &&
          control->activationSound != NULL) {
        g_SoundPlayOneShot
                  (g_UiSoundGainQ15,g_UiSoundGainQ15,
                   control->activationSound,NULL);
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
      if (((control->selectable).stateFlags & UI_SPRITE_BUTTON_ACTIVATION_SOUND) != 0 &&
          control->activationSound != NULL) {
        g_SoundPlayOneShot
                  (g_UiSoundGainQ15,g_UiSoundGainQ15,
                   control->activationSound,NULL);
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

/* nonRightRelease slot of g_UiSpriteButtonControlVtable. Completes the click of a momentary button: if it
   is still shown pressed (the pointer was released over it), it plays the activation sound, drops the
   pressed state and queues actionId. Persistent buttons act on press instead.
*/
void UiSpriteButtonControl_NonRightRelease
          (UiPointerWheelDelta wheelDelta,UiPixelCoordinate pointerY,UiPixelCoordinate pointerX,
          UiSpriteButtonControl *control)

{
  if (((control->selectable).base.nodeFlags & UI_NODE_SUPPRESSED) != 0) {
    return;
  }
  if (((control->selectable).stateFlags & UI_SELECTABLE_PERSISTENT_ACTIVATION_MODE) != 0) {
    return;
  }
  if ((((control->selectable).stateFlags & UI_SPRITE_BUTTON_ANIMATED) != 0) &&
     ((((control->selectable).stateFlags & UI_SPRITE_BUTTON_ACTION_AFTER_ANIMATION) != 0 ||
      (control->normalSubresourceEndExclusive <=
       control->animationFrameOffset + control->normalSubresourceStartOrDescriptor)))) {
    control->animationFrameOffset = 0;
  }
  if (((control->selectable).stateFlags & UI_SELECTABLE_SELECTED_OR_CHECKED) != 0) {
    if (((control->selectable).stateFlags & UI_SPRITE_BUTTON_ACTIVATION_SOUND) != 0 &&
        control->activationSound != NULL) {
      g_SoundPlayOneShot
                (g_UiSoundGainQ15,g_UiSoundGainQ15,
                 control->activationSound,NULL);
    }
    (control->selectable).stateFlags &= ~UI_SELECTABLE_SELECTED_OR_CHECKED;
    UiActionQueue_Enqueue((control->selectable).actionId,control);
    UiNode_InvalidateRoot((UiNodeBase *)control);
  }
  return;
}

/* nonRightDrag slot of g_UiSpriteButtonControlVtable, and both drag slots of the sprite-button vtables
   g_UiCommandSpriteButtonWithDetailsVtable, g_UiCommandSpriteButtonControlVtable and g_UiCatalogEntryControlVtable. While a momentary, non-animated button holds the
   pointer, it shows the pressed state only while the pointer is over the button (opaque sprite pixel or
   node rectangle), so dragging off cancels the click.
*/
void UiSpriteButtonControl_NonRightDrag
          (UiPointerWheelDelta wheelDelta,UiPixelCoordinate pointerY,UiPixelCoordinate pointerX,
          UiSpriteButtonControl *control)

{
  Bool8 pointerInside;

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
                           control->normalSubresourceStartOrDescriptor,control->primaryTextureSource);
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
    (control->selectable).stateFlags |= UI_SELECTABLE_SELECTED_OR_CHECKED;
    UiNode_InvalidateRoot((UiNodeBase *)control);
    return;
  }
  /* Pointer outside: drop the pressed state. */
  if (((control->selectable).stateFlags & UI_SELECTABLE_SELECTED_OR_CHECKED) != 0) {
    (control->selectable).stateFlags &= ~UI_SELECTABLE_SELECTED_OR_CHECKED;
    UiNode_InvalidateRoot((UiNodeBase *)control);
  }
  return;
}

/* hitTest slot of g_UiSpriteButtonControlVtable and of the sprite-button vtables g_UiCommandSpriteButtonWithDetailsVtable,
   g_UiCommandSpriteButtonControlVtable and g_UiCatalogEntryControlVtable. Returns the button when the point lies on an opaque pixel of its normal frame (selected
   frame for SELECTED_ONLY buttons); RECT_HIT_TEST buttons accept the whole node (the caller has already
   checked the rectangle). Otherwise UI_NODE_NONE.
*/
UiNodeBase * UiSpriteButtonControl_HitTestOpaque
          (UiPixelCoordinate pointerY,UiPixelCoordinate pointerX,UiSpriteButtonControl *control)

{
  Bool8 spritePixelHit;

  if (((control->selectable).base.nodeFlags & UI_NODE_SUPPRESSED) != 0) {
    return UI_NODE_NONE;
  }
  if (((control->selectable).stateFlags & UI_SPRITE_BUTTON_RECT_HIT_TEST) != 0) {
    return (UiNodeBase *)control;
  }
  if (((control->selectable).stateFlags & UI_SPRITE_BUTTON_SELECTED_ONLY) == 0) {
    if (control->primaryTextureSource == NULL) {
      return UI_NODE_NONE;
    }
    spritePixelHit = g_GraphicsTextureSourceTestOpaquePixel
                      (pointerY,pointerX,(control->selectable).base.top,
                       (control->selectable).base.left,control->normalSubresourceStartOrDescriptor,
                       control->primaryTextureSource);
  }
  else {
    /* Original quirk: no NULL check of primaryTextureSource on this path (NonRightDrag has one). */
    spritePixelHit = g_GraphicsTextureSourceTestOpaquePixel
                      (pointerY,pointerX,(control->selectable).base.top,
                       (control->selectable).base.left,control->selectedSubresourceStart,
                       control->primaryTextureSource);
  }
  if (!spritePixelHit) {
    return UI_NODE_NONE;
  }
  return (UiNodeBase *)control;
}

/* drawClipped slot of g_UiImageActionControlVtable (briefing image, movie views). Draws the image 1:1, or
   with UI_IMAGE_ACTION_STRETCH bilinearly stretched over the node; with UI_IMAGE_ACTION_LETTERBOX and a
   letterboxWidth narrower than the node it is scaled to that width, centred and framed by black bars.
   Children are drawn on top.
*/
void UiImageActionControl_DrawImageAndChildren
          (UiPixelCoordinate clipBottom,UiPixelCoordinate clipRight,UiPixelCoordinate clipTop,
          UiPixelCoordinate clipLeft,UiImageActionControl *control)

{
  int imageBottom;
  uint32_t letterboxWidth;
  int drawWidth;
  uint32_t scaledHeight;
  uint32_t horizontalMargin;
  int imageLeft;
  int imageTop;
  Bool8 accessFailed;

  if ((control->base.nodeFlags & UI_NODE_SUPPRESSED) == 0 && control->textureSource != NULL) {
    accessFailed = g_GraphicsFramebufferBeginAccess();
    if (!accessFailed) {
      if ((control->displayFlags & UI_IMAGE_ACTION_STRETCH) == 0) {
        g_GraphicsTextureSourceBlitSourceAlpha
                  (clipBottom,clipRight,clipTop,clipLeft,control->base.top,control->base.left,
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
                    (clipBottom,clipRight,clipTop,clipLeft,imageTop,control->base.right,control->base.top,
                     control->base.left,UI_IMAGE_ACTION_LETTERBOX_BAR_ARGB,g_FramebufferAccess);
          g_GraphicsFramebufferFillRectArgb
                    (clipBottom,clipRight,clipTop,clipLeft,control->base.bottom,control->base.right,imageBottom,
                     control->base.left,UI_IMAGE_ACTION_LETTERBOX_BAR_ARGB,g_FramebufferAccess);
          g_GraphicsFramebufferFillRectArgb
                    (clipBottom,clipRight,clipTop,clipLeft,imageBottom,imageLeft,imageTop,control->base.left,
                     UI_IMAGE_ACTION_LETTERBOX_BAR_ARGB,g_FramebufferAccess);
          g_GraphicsFramebufferFillRectArgb
                    (clipBottom,clipRight,clipTop,clipLeft,imageBottom,control->base.right,imageTop,
                     imageLeft + drawWidth,UI_IMAGE_ACTION_LETTERBOX_BAR_ARGB,g_FramebufferAccess);
          g_GraphicsTextureSourceStretchDirectColorBilinear
                    (scaledHeight,control->letterboxWidth,imageTop,imageLeft,control->subresource,
                     control->textureSource,g_FramebufferAccess);
          g_GraphicsFramebufferEndAccess();
        }
      }
    }
  }
  UiContainer_DrawIntersectingChildren(clipBottom,clipRight,clipTop,clipLeft,&control->base);
  return;
}

/* pointerMove slot of g_UiImageActionControlVtable: returns the control's cursor frame for the pointer.
*/
GraphicsCursorFrameIndex UiImageActionControl_QueryPointerCode
          (UiPixelCoordinate pointerY,UiPixelCoordinate pointerX,UiImageActionControl *control)

{
  return control->cursorFrame;
}

/* nonRightPress slot of g_UiImageActionControlVtable: a left click queues primaryActionId.
*/
void UiImageActionControl_EnqueuePrimaryAction
          (UiPointerWheelDelta wheelDelta,UiPixelCoordinate pointerY,UiPixelCoordinate pointerX,
          UiImageActionControl *control)

{
  UiActionQueue_Enqueue(control->primaryActionId,&control->base);
  return;
}

/* rightPress slot of g_UiImageActionControlVtable: a right click queues secondaryActionId.
*/
void UiImageActionControl_EnqueueSecondaryAction
          (UiPointerWheelDelta wheelDelta,UiPixelCoordinate pointerY,UiPixelCoordinate pointerX,
          UiImageActionControl *control)

{
  UiActionQueue_Enqueue(control->secondaryActionId,&control->base);
  return;
}

/* keyboardEvent slot of g_UiImageActionControlVtable. Tab moves the keyboard focus on; with
   UI_IMAGE_ACTION_KEY_ACTIVATES any other key queues primaryActionId, like a left click.
   Returns false when the key was consumed, true to pass it on.
*/
Bool8 UiImageActionControl_HandleKeyboardActivation
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

/* drawClipped slot of g_UiConditionalActionControlVtable. Draws nothing while the box has no text lines.
   Otherwise it draws a tiled window frame (or, for a 416x58 box, one unframed background image) and the
   rich-text lines inside the frame, clipped to the inner area. The 416x58 variant shows at most 4 lines,
   last line first.
*/
void UiConditionalActionControl_DrawClipped
          (int clipBottom,int clipRight,int clipTop,int clipLeft,UiConditionalActionControl *control)

{
  uint32_t cornerWidth;
  int textLeft;
  uint32_t cornerHeight;
  int lineTop;
  int innerWidth;
  int innerHeight;
  int innerRight;
  int innerBottom;
  uint32_t lineIndex;
  uint32_t remainingLines;
  RichTextExtent textExtent;
  GraphicsTextureLogicalSize cornerSize;
  GraphicsSubresourceIndex backgroundSubresource;

  if (control->lineCount == 0) {
    return;
  }
  if (g_GraphicsFramebufferBeginAccess()) {
    return;
  }
  backgroundSubresource = UI_TEXT_BOX_SUBRESOURCE_INTERIOR;
  if (control->base.layoutWidth == UI_TEXT_BOX_WIDE_WIDTH && control->base.layoutHeight == UI_TEXT_BOX_WIDE_HEIGHT) {
    backgroundSubresource = UI_TEXT_BOX_SUBRESOURCE_WIDE_BACKGROUND;
  }
  cornerSize = g_GraphicsTextureSourceGetLogicalSize(UI_TEXT_BOX_SUBRESOURCE_TOP_LEFT,g_UiWindowTextureSource);
  cornerHeight = cornerSize.logicalHeightPixels;
  cornerWidth = cornerSize.logicalWidthPixels;
  innerWidth = control->base.layoutWidth;
  innerHeight = control->base.layoutHeight;
  if (backgroundSubresource == UI_TEXT_BOX_SUBRESOURCE_INTERIOR) {
    innerWidth = innerWidth - cornerWidth;
    innerHeight = innerHeight - cornerHeight;
    g_GraphicsTextureSourceBlitSourceAlpha
              (clipBottom,clipRight,clipTop,clipLeft,control->base.top,control->base.left,
               UI_TEXT_BOX_SUBRESOURCE_TOP_LEFT,g_UiWindowTextureSource,g_FramebufferAccess);
    g_GraphicsTextureSourceBlitSourceAlpha
              (clipBottom,clipRight,clipTop,clipLeft,control->base.top,innerWidth + control->base.left,
               UI_TEXT_BOX_SUBRESOURCE_TOP_RIGHT,g_UiWindowTextureSource,g_FramebufferAccess);
    g_GraphicsTextureSourceBlitSourceAlpha
              (clipBottom,clipRight,clipTop,clipLeft,innerHeight + control->base.top,control->base.left,
               UI_TEXT_BOX_SUBRESOURCE_BOTTOM_LEFT,g_UiWindowTextureSource,g_FramebufferAccess);
    g_GraphicsTextureSourceBlitSourceAlpha
              (clipBottom,clipRight,clipTop,clipLeft,innerHeight + control->base.top,
               innerWidth + control->base.left,UI_TEXT_BOX_SUBRESOURCE_BOTTOM_RIGHT,g_UiWindowTextureSource,
               g_FramebufferAccess);
    UiWindow_BlitTiledHorizontalEdge
              (clipBottom,clipRight,clipTop,clipLeft,UI_TEXT_BOX_SUBRESOURCE_TOP,innerWidth,0,cornerWidth,
               &control->base);
    UiWindow_BlitTiledVerticalEdge
              (clipBottom,clipRight,clipTop,clipLeft,UI_TEXT_BOX_SUBRESOURCE_LEFT,innerHeight,cornerHeight,0,
               &control->base);
    UiWindow_BlitTiledVerticalEdge
              (clipBottom,clipRight,clipTop,clipLeft,UI_TEXT_BOX_SUBRESOURCE_RIGHT,innerHeight,cornerHeight,
               innerWidth,&control->base);
    UiWindow_BlitTiledHorizontalEdge
              (clipBottom,clipRight,clipTop,clipLeft,UI_TEXT_BOX_SUBRESOURCE_BOTTOM,innerWidth,
               innerHeight,cornerWidth,&control->base);
    UiWindow_BlitTiledInterior
              (clipBottom,clipRight,clipTop,clipLeft,UI_TEXT_BOX_SUBRESOURCE_INTERIOR,innerHeight,
               innerWidth,cornerHeight,cornerWidth,&control->base);
  }
  else {
    UiWindow_BlitTiledInterior
              (clipBottom,clipRight,clipTop,clipLeft,backgroundSubresource,innerHeight,innerWidth,0,0,
               &control->base);
    innerWidth = innerWidth - cornerWidth;
    innerHeight = innerHeight - cornerHeight;
  }
  textLeft = cornerWidth + control->base.left;
  lineTop = cornerHeight + control->base.top;
  innerRight = innerWidth + control->base.left;
  innerBottom = innerHeight + control->base.top;
  /* narrow the clip rectangle to the area inside the frame */
  if (clipLeft < textLeft) {
    clipLeft = textLeft;
  }
  if (clipTop < lineTop) {
    clipTop = lineTop;
  }
  if (innerRight < clipRight) {
    clipRight = innerRight;
  }
  if (innerBottom < clipBottom) {
    clipBottom = innerBottom;
  }
  textExtent = RichTextCommandStream_MeasureLine(g_UiTextStyleNormal,control->textLines[0]);
  if (backgroundSubresource == UI_TEXT_BOX_SUBRESOURCE_INTERIOR) {
    /* lineCount != 0 here, so the first line is always drawn */
    lineIndex = 0;
    do {
      RichTextCommandStream_DrawSingleLine
                (clipBottom,clipRight,clipTop,clipLeft,g_UiTextStyleNormal,
                 control->textLines[lineIndex],lineTop,textLeft + 3);
      lineIndex = lineIndex + 1;
      lineTop = lineTop + textExtent.heightPixels;
    } while (lineIndex < control->lineCount);
  }
  else {
    /* wide box: at most UI_TEXT_BOX_WIDE_MAX_LINES lines, last line first */
    remainingLines = control->lineCount;
    if (UI_TEXT_BOX_WIDE_MAX_LINES < remainingLines) {
      remainingLines = UI_TEXT_BOX_WIDE_MAX_LINES;
    }
    do {
      RichTextCommandStream_DrawSingleLine
                (clipBottom,clipRight,clipTop,clipLeft,g_UiTextStyleNormal,
                 control->textLines[remainingLines - 1],lineTop,textLeft + 3);
      lineTop = lineTop + textExtent.heightPixels;
      remainingLines = remainingLines - 1;
    } while (remainingLines != 0);
  }
  g_GraphicsFramebufferEndAccess();
  return;
}

/* pointerMove slot of g_UiConditionalActionControlVtable: returns the control's cursor frame.
*/
GraphicsCursorFrameIndex UiConditionalActionControl_QueryPointerCode
          (UiPixelCoordinate pointerY,UiPixelCoordinate pointerX,UiConditionalActionControl *control)

{
  return control->cursorFrame;
}

/* hitTest slot of g_UiConditionalActionControlVtable. An empty box (no text lines) is invisible and
   returns UI_NODE_NONE; otherwise the normal child hit test applies.
*/
UiNodeBase * UiConditionalActionControl_HitTestWhenEnabled
          (UiPixelCoordinate pointerY,UiPixelCoordinate pointerX,UiConditionalActionControl *control)

{
  UiNodeBase *hitNode;

  hitNode = UI_NODE_NONE;
  if (control->lineCount != 0) {
    hitNode = UiContainer_HitTestChildren(pointerY,pointerX,&control->base);
  }
  return hitNode;
}

/* nonRightPress slot of g_UiConditionalActionControlVtable: a left click queues actionId, but only while
   the box shows text.
*/
void UiConditionalActionControl_EnqueuePrimaryActionIfEnabled
          (UiPointerWheelDelta wheelDelta,UiPixelCoordinate pointerY,UiPixelCoordinate pointerX,
          UiConditionalActionControl *control)

{
  if (control->lineCount != 0) {
    UiActionQueue_Enqueue(control->actionId,&control->base);
  }
  return;
}

/* Advances an animated sprite button by one frame within its normal or selected frame range, wrapping to
   the first frame. On the last frame a deferred activation action is queued (and cleared), then the UI is
   redrawn.
*/
void UiSpriteButtonControl_AdvanceAnimation(UiSpriteButtonControl *control)

{
  uint32_t subresourceStart;
  uint32_t subresourceEndExclusive;

  if (((control->selectable).stateFlags & UI_SPRITE_BUTTON_ANIMATED) == 0) {
    return;
  }
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
  /* last frame reached: fire a deferred (ACTION_AFTER_ANIMATION) action once */
  if (subresourceEndExclusive - 1 <= subresourceStart + control->animationFrameOffset) {
    if ((((control->selectable).stateFlags & UI_SPRITE_BUTTON_ACTION_AFTER_ANIMATION) != 0) &&
       (((control->selectable).stateFlags & UI_SPRITE_BUTTON_ACTION_PENDING) != 0)) {
      UiActionQueue_Enqueue((control->selectable).actionId,control);
    }
    (control->selectable).stateFlags &= ~UI_SPRITE_BUTTON_ACTION_PENDING;
  }
  UiNode_InvalidateRoot((UiNodeBase *)control);
  return;
}

UiNodeVtable g_UiSpriteButtonControlVtable = {
    .relocate = THANDOR_FN(UiSpriteButtonControl_Relocate),
    .method04 = THANDOR_FN(UiNode_DefaultMethod04_NoOp),
    .drawClipped = THANDOR_FN(UiSpriteButtonControl_DrawClipped),
    .layout = THANDOR_FN(UiContainer_LayoutChildren),
    .nonRightPress = THANDOR_FN(UiSpriteButtonControl_NonRightPress),
    .nonRightRelease = THANDOR_FN(UiSpriteButtonControl_NonRightRelease),
    .rightPress = THANDOR_FN(UiNode_ForwardRightPressToParent),
    .rightRelease = THANDOR_FN(UiNode_DefaultRightRelease),
    .nonRightDrag = THANDOR_FN(UiSpriteButtonControl_NonRightDrag),
    .rightDrag = THANDOR_FN(UiNode_DefaultRightDrag),
    .pointerMove = THANDOR_FN(UiNode_DefaultPointerMove),
    .hitTest = THANDOR_FN(UiSpriteButtonControl_HitTestOpaque),
    .keyboardEvent = THANDOR_FN(UiSelectableControl_KeyboardEvent),
    .applyFlags = THANDOR_FN(UiNode_ApplyFlagsRecursive),
    .suppressActionId = THANDOR_FN(UiSelectableControl_SuppressIfActionId),
    .unsuppressActionId = THANDOR_FN(UiSelectableControl_UnsuppressIfActionId),
    .tick = THANDOR_FN(UiNode_DefaultTick),
    .pointerWheel = THANDOR_FN(UiNode_ForwardPointerWheelToParent)};

UiNodeVtable g_UiImageActionControlVtable = {
        .relocate = THANDOR_FN(UiContainer_RelocateChildren),
        .method04 = THANDOR_FN(UiNode_DefaultMethod04_NoOp),
        .drawClipped = THANDOR_FN(UiImageActionControl_DrawImageAndChildren),
        .layout = THANDOR_FN(UiContainer_LayoutChildren),
        .nonRightPress = THANDOR_FN(UiImageActionControl_EnqueuePrimaryAction),
        .nonRightRelease = THANDOR_FN(UiNode_DefaultNonRightRelease),
        .rightPress = THANDOR_FN(UiImageActionControl_EnqueueSecondaryAction),
        .rightRelease = THANDOR_FN(UiNode_DefaultRightRelease),
        .nonRightDrag = THANDOR_FN(UiNode_DefaultNonRightDrag),
        .rightDrag = THANDOR_FN(UiNode_DefaultRightDrag),
        .pointerMove = THANDOR_FN(UiImageActionControl_QueryPointerCode),
        .hitTest = THANDOR_FN(UiContainer_HitTestChildren),
        .keyboardEvent = THANDOR_FN(UiImageActionControl_HandleKeyboardActivation),
        .applyFlags = THANDOR_FN(UiNode_ApplyFlagsRecursive),
        .suppressActionId = THANDOR_FN(UiContainer_SuppressActionId),
        .unsuppressActionId = THANDOR_FN(UiContainer_UnsuppressActionId),
        .tick = THANDOR_FN(UiNode_DefaultTick),
        .pointerWheel = THANDOR_FN(UiNode_ForwardPointerWheelToParent),
};

UiNodeVtable g_UiConditionalActionControlVtable = {
        .relocate = THANDOR_FN(UiContainer_RelocateChildren),
        .method04 = THANDOR_FN(UiNode_DefaultMethod04_NoOp),
        .drawClipped = THANDOR_FN(UiConditionalActionControl_DrawClipped),
        .layout = THANDOR_FN(UiContainer_LayoutChildren),
        .nonRightPress = THANDOR_FN(UiConditionalActionControl_EnqueuePrimaryActionIfEnabled),
        .nonRightRelease = THANDOR_FN(UiNode_DefaultNonRightRelease),
        .rightPress = THANDOR_FN(UiNode_ForwardRightPressToParent),
        .rightRelease = THANDOR_FN(UiNode_DefaultRightRelease),
        .nonRightDrag = THANDOR_FN(UiNode_DefaultNonRightDrag),
        .rightDrag = THANDOR_FN(UiNode_DefaultRightDrag),
        .pointerMove = THANDOR_FN(UiConditionalActionControl_QueryPointerCode),
        .hitTest = THANDOR_FN(UiConditionalActionControl_HitTestWhenEnabled),
        .keyboardEvent = THANDOR_FN(UiNode_DefaultKeyboardEventMoveFocusNext),
        .applyFlags = THANDOR_FN(UiNode_ApplyFlagsRecursive),
        .suppressActionId = THANDOR_FN(UiContainer_SuppressActionId),
        .unsuppressActionId = THANDOR_FN(UiContainer_UnsuppressActionId),
        .tick = THANDOR_FN(UiNode_DefaultTick),
        .pointerWheel = THANDOR_FN(UiNode_ForwardPointerWheelToParent),
};

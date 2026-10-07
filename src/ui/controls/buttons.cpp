/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/src/ui/controls/buttons.cpp
 * Reverse engineering by idkFoxes 2026
 */

#include <thandor/ui/controls/buttons.h>
#include <thandor/thandor.h>

/* Depth-first walk over node, its descendants and its following siblings: each node's subtree is
   visited before its next sibling, at every depth. */
static void UiTree_AdvanceSpriteButtonAnimationsFrom(UiNodeBase *node) {
  while (node != UI_NODE_NONE) {
    if (node->vtable == &g_UiSpriteButtonControlVtable) {
      UiSpriteButtonControl_AdvanceAnimation(UiNode_As<UiSpriteButtonControl>(node));
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

  if (Any((control->selectable).stateFlags & UI_SPRITE_BUTTON_ANIMATED)) {
    control->animationFrameOffset = 0;
    if (Any((control->selectable).stateFlags & UI_SPRITE_BUTTON_SERIALIZED_DESCRIPTOR)) {
      sequenceDescriptor = Thandor_U32ToPointer<int32_t>(control->normalSubresourceStartOrDescriptor); /* 5f-format: UiSpriteButtonControl.normalSubresourceStartOrDescriptor (UI template) */
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
  UiContainer_RelocateChildren(relocationDelta,&control->selectable.base);
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

  if (Any((control->selectable).base.nodeFlags & UI_NODE_SUPPRESSED)) {
    return;
  }
  /* SELECTED_ONLY buttons are invisible while not selected */
  if (!Any((control->selectable).stateFlags & UI_SELECTABLE_SELECTED_OR_CHECKED) &&
      Any((control->selectable).stateFlags & UI_SPRITE_BUTTON_SELECTED_ONLY)) {
    return;
  }
  if (control->primaryTextureSource == nullptr) {
    return;
  }
  if (g_GraphicsFramebufferBeginAccess()) {
    return;
  }

  /* shadow pass */
  if (!Any((control->selectable).stateFlags & UI_SELECTABLE_SELECTED_OR_CHECKED)) {
    shadowOffsetX = (control->drawOffsets).normalX;
    shadowOffsetY = (control->drawOffsets).normalY;
  }
  else {
    shadowOffsetX = (control->drawOffsets).selectedX;
    shadowOffsetY = (control->drawOffsets).selectedY;
  }
  textureSource = control->primaryTextureSource;
  if (!Any((control->selectable).stateFlags & UI_SELECTABLE_SELECTED_OR_CHECKED)) {
    subresourceIndex = control->normalSubresourceStartOrDescriptor;
  }
  else {
    if (!Any((control->selectable).stateFlags & UI_SPRITE_BUTTON_ANIMATED) &&
       (Any((control->selectable).stateFlags & UI_SPRITE_BUTTON_ALTERNATE_SELECTED_TEXTURE))) {
      textureSource = control->alternateTextureSource;
    }
    subresourceIndex = control->selectedSubresourceStart;
  }
  if (Any((control->selectable).stateFlags & UI_SPRITE_BUTTON_ANIMATED)) {
    subresourceIndex = subresourceIndex + control->animationFrameOffset;
  }
  g_GraphicsTextureSourceBlitModulatedSourceAlpha
            (clipBottom,clipRight,clipTop,clipLeft,(int)shadowOffsetY + (control->selectable).base.top,
             (int)shadowOffsetX + (control->selectable).base.left,UI_SPRITE_BUTTON_SHADOW_ARGB,
             subresourceIndex,textureSource,g_FramebufferAccess);

  /* sprite pass (the framebuffer access is read before the optional underlay blit) */
  textureSource = control->primaryTextureSource;
  framebufferAccess = g_FramebufferAccess;
  if (!Any((control->selectable).stateFlags & UI_SELECTABLE_SELECTED_OR_CHECKED)) {
    subresourceIndex = control->normalSubresourceStartOrDescriptor;
  }
  else {
    if (!Any((control->selectable).stateFlags & UI_SPRITE_BUTTON_ANIMATED) &&
       (Any((control->selectable).stateFlags & UI_SPRITE_BUTTON_ALTERNATE_SELECTED_TEXTURE))) {
      textureSource = control->alternateTextureSource;
    }
    subresourceIndex = control->selectedSubresourceStart;
    if (Any((control->selectable).stateFlags & UI_SPRITE_BUTTON_NORMAL_UNDER_SELECTED)) {
      underlaySubresource = control->normalSubresourceStartOrDescriptor;
      if (Any((control->selectable).stateFlags & UI_SPRITE_BUTTON_ANIMATED)) {
        underlaySubresource = underlaySubresource + control->animationFrameOffset;
      }
      g_GraphicsTextureSourceBlitSourceAlpha
                (clipBottom,clipRight,clipTop,clipLeft,(control->selectable).base.top,
                 (control->selectable).base.left,underlaySubresource,control->primaryTextureSource,
                 g_FramebufferAccess);
    }
  }
  if (Any((control->selectable).stateFlags & UI_SPRITE_BUTTON_ANIMATED)) {
    subresourceIndex = subresourceIndex + control->animationFrameOffset;
  }
  g_GraphicsTextureSourceBlitSourceAlpha
            (clipBottom,clipRight,clipTop,clipLeft,(control->selectable).base.top,
             (control->selectable).base.left,subresourceIndex,textureSource,framebufferAccess);
  g_GraphicsFramebufferEndAccess();
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

  if (!Any((control->selectable).base.nodeFlags & UI_NODE_SUPPRESSED)) {
    if (!Any((control->selectable).stateFlags & UI_SELECTABLE_PERSISTENT_ACTIVATION_MODE)) {
      stateFlagsField = &(control->selectable).stateFlags;
      *stateFlagsField = *stateFlagsField | UI_SELECTABLE_SELECTED_OR_CHECKED;
      if (Any((control->selectable).stateFlags & UI_SPRITE_BUTTON_ANIMATED) &&
         ((Any((control->selectable).stateFlags & UI_SPRITE_BUTTON_ACTION_AFTER_ANIMATION) ||
          (control->selectedSubresourceEndExclusive <=
           control->animationFrameOffset + control->selectedSubresourceStart)))) {
        control->animationFrameOffset = 0;
      }
      UiNode_InvalidateRoot(&control->selectable.base);
      return;
    }
    if (Any((control->selectable).stateFlags & UI_SELECTABLE_TOGGLE_ON_ACTIVATION)) {
      if (Any((control->selectable).stateFlags & UI_SPRITE_BUTTON_ACTIVATION_SOUND) &&
          control->activationSound != nullptr) {
        g_SoundPlayOneShot
                  (g_UiSoundGainQ15,g_UiSoundGainQ15,
                   control->activationSound,nullptr);
      }
      selectionStateFlagsField = &(control->selectable).stateFlags;
      *selectionStateFlagsField = *selectionStateFlagsField ^ UI_SELECTABLE_SELECTED_OR_CHECKED;
      queueAction = true;
      if (Any((control->selectable).stateFlags & UI_SPRITE_BUTTON_ANIMATED)) {
        if (Any((control->selectable).stateFlags & UI_SPRITE_BUTTON_ACTION_AFTER_ANIMATION) ||
           (control->selectedSubresourceEndExclusive <=
            control->animationFrameOffset + control->selectedSubresourceStart)) {
          control->animationFrameOffset = 0;
        }
        pressStateFlagsField = &(control->selectable).stateFlags;
        *pressStateFlagsField = *pressStateFlagsField | UI_SPRITE_BUTTON_ACTION_PENDING;
        /* ACTION_AFTER_ANIMATION: UiSpriteButtonControl_AdvanceAnimation queues it on the last frame. */
        queueAction = !Any((control->selectable).stateFlags & UI_SPRITE_BUTTON_ACTION_AFTER_ANIMATION);
      }
      if (queueAction) {
        UiActionQueue_Enqueue((control->selectable).actionId,control);
      }
      UiNode_InvalidateRoot(&control->selectable.base);
      return;
    }
    if (!Any((control->selectable).stateFlags & UI_SELECTABLE_SELECTED_OR_CHECKED)) {
      if (Any((control->selectable).stateFlags & UI_SPRITE_BUTTON_ACTIVATION_SOUND) &&
          control->activationSound != nullptr) {
        g_SoundPlayOneShot
                  (g_UiSoundGainQ15,g_UiSoundGainQ15,
                   control->activationSound,nullptr);
      }
      pressStateFlagsField = &(control->selectable).stateFlags;
      *pressStateFlagsField = *pressStateFlagsField | UI_SELECTABLE_SELECTED_OR_CHECKED;
      queueAction = true;
      if (Any((control->selectable).stateFlags & UI_SPRITE_BUTTON_ANIMATED)) {
        if (Any((control->selectable).stateFlags & UI_SPRITE_BUTTON_ACTION_AFTER_ANIMATION) ||
           (control->selectedSubresourceEndExclusive <=
            control->animationFrameOffset + control->selectedSubresourceStart)) {
          control->animationFrameOffset = 0;
        }
        pressStateFlagsField = &(control->selectable).stateFlags;
        *pressStateFlagsField = *pressStateFlagsField | UI_SPRITE_BUTTON_ACTION_PENDING;
        queueAction = !Any((control->selectable).stateFlags & UI_SPRITE_BUTTON_ACTION_AFTER_ANIMATION);
      }
      if (queueAction) {
        UiActionQueue_Enqueue((control->selectable).actionId,control);
      }
      UiNode_InvalidateRoot(&control->selectable.base);
    }
  }
}

/* nonRightRelease slot of g_UiSpriteButtonControlVtable. Completes the click of a momentary button: if it
   is still shown pressed (the pointer was released over it), it plays the activation sound, drops the
   pressed state and queues actionId. Persistent buttons act on press instead.
*/
void UiSpriteButtonControl_NonRightRelease
          (UiPointerWheelDelta wheelDelta,UiPixelCoordinate pointerY,UiPixelCoordinate pointerX,
          UiSpriteButtonControl *control)

{
  if (Any((control->selectable).base.nodeFlags & UI_NODE_SUPPRESSED)) {
    return;
  }
  if (Any((control->selectable).stateFlags & UI_SELECTABLE_PERSISTENT_ACTIVATION_MODE)) {
    return;
  }
  if (Any((control->selectable).stateFlags & UI_SPRITE_BUTTON_ANIMATED) &&
     ((Any((control->selectable).stateFlags & UI_SPRITE_BUTTON_ACTION_AFTER_ANIMATION) ||
      (control->normalSubresourceEndExclusive <=
       control->animationFrameOffset + control->normalSubresourceStartOrDescriptor)))) {
    control->animationFrameOffset = 0;
  }
  if (Any((control->selectable).stateFlags & UI_SELECTABLE_SELECTED_OR_CHECKED)) {
    if (Any((control->selectable).stateFlags & UI_SPRITE_BUTTON_ACTIVATION_SOUND) &&
        control->activationSound != nullptr) {
      g_SoundPlayOneShot
                (g_UiSoundGainQ15,g_UiSoundGainQ15,
                 control->activationSound,nullptr);
    }
    (control->selectable).stateFlags &= ~UI_SELECTABLE_SELECTED_OR_CHECKED;
    UiActionQueue_Enqueue((control->selectable).actionId,control);
    UiNode_InvalidateRoot(&control->selectable.base);
  }
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

  if (Any((control->selectable).base.nodeFlags & UI_NODE_SUPPRESSED)) {
    return;
  }
  if (Any((control->selectable).stateFlags & UI_SELECTABLE_PERSISTENT_ACTIVATION_MODE)) {
    return;
  }
  if (Any((control->selectable).stateFlags & UI_SPRITE_BUTTON_ANIMATED)) {
    return;
  }
  if (!Any((control->selectable).stateFlags & UI_SPRITE_BUTTON_RECT_HIT_TEST)) {
    pointerInside = false;
    if (control->primaryTextureSource != nullptr) {
      if (!Any((control->selectable).stateFlags & UI_SPRITE_BUTTON_SELECTED_ONLY)) {
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
    if (Any((control->selectable).stateFlags & UI_SELECTABLE_SELECTED_OR_CHECKED)) {
      return;
    }
    (control->selectable).stateFlags |= UI_SELECTABLE_SELECTED_OR_CHECKED;
    UiNode_InvalidateRoot(&control->selectable.base);
    return;
  }
  /* Pointer outside: drop the pressed state. */
  if (Any((control->selectable).stateFlags & UI_SELECTABLE_SELECTED_OR_CHECKED)) {
    (control->selectable).stateFlags &= ~UI_SELECTABLE_SELECTED_OR_CHECKED;
    UiNode_InvalidateRoot(&control->selectable.base);
  }
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

  if (Any((control->selectable).base.nodeFlags & UI_NODE_SUPPRESSED)) {
    return UI_NODE_NONE;
  }
  if (Any((control->selectable).stateFlags & UI_SPRITE_BUTTON_RECT_HIT_TEST)) {
    return &control->selectable.base;
  }
  if (!Any((control->selectable).stateFlags & UI_SPRITE_BUTTON_SELECTED_ONLY)) {
    if (control->primaryTextureSource == nullptr) {
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
  return &control->selectable.base;
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

  if (!Any(control->base.nodeFlags & UI_NODE_SUPPRESSED) && control->textureSource != nullptr) {
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
}

/* rightPress slot of g_UiImageActionControlVtable: a right click queues secondaryActionId.
*/
void UiImageActionControl_EnqueueSecondaryAction
          (UiPointerWheelDelta wheelDelta,UiPixelCoordinate pointerY,UiPixelCoordinate pointerX,
          UiImageActionControl *control)

{
  UiActionQueue_Enqueue(control->secondaryActionId,&control->base);
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
    while (lineIndex < control->lineCount) {
      RichTextCommandStream_DrawSingleLine
                (clipBottom,clipRight,clipTop,clipLeft,g_UiTextStyleNormal,
                 control->textLines[lineIndex],lineTop,textLeft + 3);
      lineIndex = lineIndex + 1;
      lineTop = lineTop + textExtent.heightPixels;
    }
  }
  else {
    /* wide box: at most UI_TEXT_BOX_WIDE_MAX_LINES lines, last line first */
    remainingLines = control->lineCount;
    if (UI_TEXT_BOX_WIDE_MAX_LINES < remainingLines) {
      remainingLines = UI_TEXT_BOX_WIDE_MAX_LINES;
    }
    while (remainingLines != 0) {
      RichTextCommandStream_DrawSingleLine
                (clipBottom,clipRight,clipTop,clipLeft,g_UiTextStyleNormal,
                 control->textLines[remainingLines - 1],lineTop,textLeft + 3);
      lineTop = lineTop + textExtent.heightPixels;
      remainingLines = remainingLines - 1;
    }
  }
  g_GraphicsFramebufferEndAccess();
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
}

/* Advances an animated sprite button by one frame within its normal or selected frame range, wrapping to
   the first frame. On the last frame a deferred activation action is queued (and cleared), then the UI is
   redrawn.
*/
void UiSpriteButtonControl_AdvanceAnimation(UiSpriteButtonControl *control)

{
  uint32_t subresourceStart;
  uint32_t subresourceEndExclusive;

  if (!Any((control->selectable).stateFlags & UI_SPRITE_BUTTON_ANIMATED)) {
    return;
  }
  if (!Any((control->selectable).stateFlags & UI_SELECTABLE_SELECTED_OR_CHECKED)) {
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
    if (Any((control->selectable).stateFlags & UI_SPRITE_BUTTON_ACTION_AFTER_ANIMATION) &&
       (Any((control->selectable).stateFlags & UI_SPRITE_BUTTON_ACTION_PENDING))) {
      UiActionQueue_Enqueue((control->selectable).actionId,control);
    }
    (control->selectable).stateFlags &= ~UI_SPRITE_BUTTON_ACTION_PENDING;
  }
  UiNode_InvalidateRoot(&control->selectable.base);
}

UiNodeVtable g_UiSpriteButtonControlVtable = {
    .relocate = UI_SLOT(UiSpriteButtonControl_Relocate),
    .method04 = UI_SLOT(UiNode_DefaultMethod04_NoOp),
    .drawClipped = UI_SLOT(UiSpriteButtonControl_DrawClipped),
    .layout = UI_SLOT(UiContainer_LayoutChildren),
    .nonRightPress = UI_SLOT(UiSpriteButtonControl_NonRightPress),
    .nonRightRelease = UI_SLOT(UiSpriteButtonControl_NonRightRelease),
    .rightPress = UI_SLOT(UiNode_ForwardRightPressToParent),
    .rightRelease = UI_SLOT(UiNode_DefaultRightRelease),
    .nonRightDrag = UI_SLOT(UiSpriteButtonControl_NonRightDrag),
    .rightDrag = UI_SLOT(UiNode_DefaultRightDrag),
    .pointerMove = UI_SLOT(UiNode_DefaultPointerMove),
    .hitTest = UI_SLOT(UiSpriteButtonControl_HitTestOpaque),
    .keyboardEvent = UI_SLOT(UiSelectableControl_KeyboardEvent),
    .applyFlags = UI_SLOT(UiNode_ApplyFlagsRecursive),
    .suppressActionId = UI_SLOT(UiSelectableControl_SuppressIfActionId),
    .unsuppressActionId = UI_SLOT(UiSelectableControl_UnsuppressIfActionId),
    .tick = UI_SLOT(UiNode_DefaultTick),
    .pointerWheel = UI_SLOT(UiNode_ForwardPointerWheelToParent)};

UiNodeVtable g_UiImageActionControlVtable = {
        .relocate = UI_SLOT(UiContainer_RelocateChildren),
        .method04 = UI_SLOT(UiNode_DefaultMethod04_NoOp),
        .drawClipped = UI_SLOT(UiImageActionControl_DrawImageAndChildren),
        .layout = UI_SLOT(UiContainer_LayoutChildren),
        .nonRightPress = UI_SLOT(UiImageActionControl_EnqueuePrimaryAction),
        .nonRightRelease = UI_SLOT(UiNode_DefaultNonRightRelease),
        .rightPress = UI_SLOT(UiImageActionControl_EnqueueSecondaryAction),
        .rightRelease = UI_SLOT(UiNode_DefaultRightRelease),
        .nonRightDrag = UI_SLOT(UiNode_DefaultNonRightDrag),
        .rightDrag = UI_SLOT(UiNode_DefaultRightDrag),
        .pointerMove = UI_SLOT(UiImageActionControl_QueryPointerCode),
        .hitTest = UI_SLOT(UiContainer_HitTestChildren),
        .keyboardEvent = UI_SLOT(UiImageActionControl_HandleKeyboardActivation),
        .applyFlags = UI_SLOT(UiNode_ApplyFlagsRecursive),
        .suppressActionId = UI_SLOT(UiContainer_SuppressActionId),
        .unsuppressActionId = UI_SLOT(UiContainer_UnsuppressActionId),
        .tick = UI_SLOT(UiNode_DefaultTick),
        .pointerWheel = UI_SLOT(UiNode_ForwardPointerWheelToParent),
};

UiNodeVtable g_UiConditionalActionControlVtable = {
        .relocate = UI_SLOT(UiContainer_RelocateChildren),
        .method04 = UI_SLOT(UiNode_DefaultMethod04_NoOp),
        .drawClipped = UI_SLOT(UiConditionalActionControl_DrawClipped),
        .layout = UI_SLOT(UiContainer_LayoutChildren),
        .nonRightPress = UI_SLOT(UiConditionalActionControl_EnqueuePrimaryActionIfEnabled),
        .nonRightRelease = UI_SLOT(UiNode_DefaultNonRightRelease),
        .rightPress = UI_SLOT(UiNode_ForwardRightPressToParent),
        .rightRelease = UI_SLOT(UiNode_DefaultRightRelease),
        .nonRightDrag = UI_SLOT(UiNode_DefaultNonRightDrag),
        .rightDrag = UI_SLOT(UiNode_DefaultRightDrag),
        .pointerMove = UI_SLOT(UiConditionalActionControl_QueryPointerCode),
        .hitTest = UI_SLOT(UiConditionalActionControl_HitTestWhenEnabled),
        .keyboardEvent = UI_SLOT(UiNode_DefaultKeyboardEventMoveFocusNext),
        .applyFlags = UI_SLOT(UiNode_ApplyFlagsRecursive),
        .suppressActionId = UI_SLOT(UiContainer_SuppressActionId),
        .unsuppressActionId = UI_SLOT(UiContainer_UnsuppressActionId),
        .tick = UI_SLOT(UiNode_DefaultTick),
        .pointerWheel = UI_SLOT(UiNode_ForwardPointerWheelToParent),
};

/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/src/ui/controls/image.cpp
 * Reverse engineering by idkFoxes 2026
 */

#include <thandor/ui/controls/image.h>
#include <thandor/thandor.h>

/* Module data. */

UiImageControl * g_UiImageControlHoverTarget = nullptr;

/* Implementation ownership: ui/controls/image. */

/* layout of g_UiImageControlVtable (image toggles of the in-game resource panel): lays out the children
   relative to the parent's rectangle instead of the control's own by swapping the parent's edges in for
   the call; afterwards the own rectangle is restored and its size stored as layoutWidth/layoutHeight.
*/
void UiImageControl_LayoutChildrenToParent(UiImageControl *control)

{
  int32_t *edgeField;
  int savedLeft;
  int savedTop;
  int savedRight;
  int savedBottom;
  UiNodeBase *parentNode;
  int32_t parentTop;
  int32_t parentRight;
  int32_t parentBottom;
  
  parentNode = (control->selectable).base.parent;
  parentTop = parentNode->top;
  parentRight = parentNode->right;
  parentBottom = parentNode->bottom;
  LOCK();
  edgeField = &(control->selectable).base.left;
  savedLeft = *edgeField;
  *edgeField = parentNode->left;
  UNLOCK();
  LOCK();
  edgeField = &(control->selectable).base.top;
  savedTop = *edgeField;
  *edgeField = parentTop;
  UNLOCK();
  LOCK();
  edgeField = &(control->selectable).base.right;
  savedRight = *edgeField;
  *edgeField = parentRight;
  UNLOCK();
  LOCK();
  edgeField = &(control->selectable).base.bottom;
  savedBottom = *edgeField;
  *edgeField = parentBottom;
  UNLOCK();
  UiContainer_LayoutChildren((UiNodeBase *)control);
  (control->selectable).base.left = savedLeft;
  (control->selectable).base.top = savedTop;
  (control->selectable).base.right = savedRight;
  (control->selectable).base.bottom = savedBottom;
  (control->selectable).base.layoutWidth = savedRight - savedLeft;
  (control->selectable).base.layoutHeight = savedBottom - savedTop;
  return;
}

/* nonRightDrag of the image control (g_UiImageControlVtable): only for an image in persistent activation
   mode, whose children act like a menu. Moving onto another child hands the pointer over: the new child
   gets a synthetic press and the drag, becomes activeChild, and the previous one gets a synthetic drag and
   release far outside (UI_POINTER_FAR_OUTSIDE). A drag over the current child is simply forwarded.
*/
void UiImageControl_NonRightDrag(UiPointerWheelDelta wheelDelta,UiPixelCoordinate pointerY,UiPixelCoordinate pointerX,
          UiImageControl *control)

{
  UiNodeVtable *hitVtable;
  UiImageControl *hitControl;
  UiNodeBase *previousActiveChild;

  if (((control->selectable).stateFlags & UI_SELECTABLE_PERSISTENT_ACTIVATION_MODE) == 0) {
    return;
  }
  hitControl = (UiImageControl *)UiImageControl_HitTestOpaque(pointerY,pointerX,control);
  /* Off the image's own pixels PRESSED_ON_IMAGE is cleared. Over the image itself or over nothing the
     current child only gets the synthetic drag and release; it stays activeChild (as in the original). */
  if (hitControl != control) {
    (control->selectable).stateFlags &= ~UI_IMAGE_CONTROL_PRESSED_ON_IMAGE;
  }
  if (hitControl == control || hitControl == (UiImageControl *)UI_NODE_NONE) {
    previousActiveChild = control->activeChild;
  }
  else {
    if (hitControl == (UiImageControl *)control->activeChild) {
      (*((hitControl->selectable).base.vtable)->nonRightDrag)
                (wheelDelta,pointerY,pointerX,(UiNodeBase *)hitControl);
      UiRootStack_InvalidateAll();
      return;
    }
    hitVtable = (hitControl->selectable).base.vtable;
    /* All calls go to the hit child through its own vtable, and that child becomes the new
       activeChild. */
    hitVtable->nonRightPress(0,UI_POINTER_FAR_OUTSIDE,UI_POINTER_FAR_OUTSIDE,(UiNodeBase *)hitControl);
    hitVtable->nonRightDrag(wheelDelta,pointerY,pointerX,(UiNodeBase *)hitControl);
    /* swap in the new active child */
    previousActiveChild = control->activeChild;
    control->activeChild = (UiNodeBase *)hitControl;
  }
  if (previousActiveChild != nullptr) {
    previousActiveChild->vtable->nonRightDrag
              (0,UI_POINTER_FAR_OUTSIDE,UI_POINTER_FAR_OUTSIDE,previousActiveChild);
    previousActiveChild->vtable->nonRightRelease
              (0,UI_POINTER_FAR_OUTSIDE,UI_POINTER_FAR_OUTSIDE,previousActiveChild);
  }
  UiRootStack_InvalidateAll();
  return;
}

/* tick of the image control (g_UiImageControlVtable): when the right mouse button goes down (latched in
   UI_IMAGE_CONTROL_RIGHT_BUTTON_LATCHED until it is released), an opaque child under the cursor gets a
   release, press and drag at the current cursor position, so that it re-evaluates the pointer;
   UI_IMAGE_CONTROL_PRESS_STARTED is cleared then.
*/
void UiImageControl_TickHover(UiImageControl *control)

{
  UiSelectableStateFlags *clearStateFlagsField;
  UiNodeVtable *hitChildVtable;
  UiImageControl *hitControl;
  UiSelectableStateFlags *stateFlagsField;
  UiSelectableStateFlags *hoverStateFlagsField;

  if (((control->selectable).base.nodeFlags & UI_NODE_SUPPRESSED) == 0) {
    if (((control->selectable).stateFlags & UI_IMAGE_CONTROL_RIGHT_BUTTON_LATCHED) == 0) {
      if ((g_CursorButtonState & RIGHT) != 0) {
        hitControl = (UiImageControl *)
                     UiImageControl_HitTestOpaque(g_CursorOverrideY,g_CursorOverrideX,control);
        stateFlagsField = &(control->selectable).stateFlags;
        *stateFlagsField = *stateFlagsField | UI_IMAGE_CONTROL_RIGHT_BUTTON_LATCHED;
        if ((hitControl != control) && (hitControl != (UiImageControl *)UI_NODE_NONE)) {
          hitChildVtable = (hitControl->selectable).base.vtable;
          hoverStateFlagsField = &(control->selectable).stateFlags;
          *hoverStateFlagsField = *hoverStateFlagsField & ~UI_IMAGE_CONTROL_PRESS_STARTED;
          hitChildVtable->nonRightRelease
                    (g_CursorWheelDelta,g_CursorOverrideY,g_CursorOverrideX,(UiNodeBase *)hitControl);
          /* all three calls go to the hovered child through its own vtable */
          hitChildVtable->nonRightPress
                    (g_CursorWheelDelta,g_CursorOverrideY,g_CursorOverrideX,(UiNodeBase *)hitControl);
          hitChildVtable->nonRightDrag
                    (g_CursorWheelDelta,g_CursorOverrideY,g_CursorOverrideX,(UiNodeBase *)hitControl);
        }
      }
    }
    else if ((g_CursorButtonState & RIGHT) == 0) {
      clearStateFlagsField = &(control->selectable).stateFlags;
      *clearStateFlagsField = *clearStateFlagsField & ~UI_IMAGE_CONTROL_RIGHT_BUTTON_LATCHED;
    }
  }
  return;
}

/* drawClipped of the image control (g_UiImageControlVtable): in persistent activation mode the children
   are drawn first, then the image itself: alternateSubresource while selected, else normalSubresource. An
   image with UI_IMAGE_CONTROL_ALTERNATE_HIT_SHAPE is only drawn while selected.
*/
void UiImageControl_DrawClipped(UiPixelCoordinate clipBottom,UiPixelCoordinate clipRight,UiPixelCoordinate clipTop,
          UiPixelCoordinate clipLeft,UiImageControl *control)

{
  Bool8 accessFailed;
  GraphicsSubresourceIndex subresource;

  if (((control->selectable).base.nodeFlags & UI_NODE_SUPPRESSED) == 0) {
    if (((control->selectable).stateFlags & UI_SELECTABLE_PERSISTENT_ACTIVATION_MODE) != 0) {
      UiContainer_DrawIntersectingChildren
                (clipBottom,clipRight,clipTop,clipLeft,(UiNodeBase *)control);
    }
    if ((((control->selectable).stateFlags & UI_SELECTABLE_SELECTED_OR_CHECKED) != 0) ||
       (((control->selectable).stateFlags & UI_IMAGE_CONTROL_ALTERNATE_HIT_SHAPE) == 0)) {
      accessFailed = g_GraphicsFramebufferBeginAccess();
      if (!accessFailed) {
        if (((control->selectable).stateFlags & UI_SELECTABLE_SELECTED_OR_CHECKED) == 0) {
          subresource = control->normalSubresource;
        }
        else {
          subresource = control->alternateSubresource;
        }
        g_GraphicsTextureSourceBlitSourceAlpha
                  (clipBottom,clipRight,clipTop,clipLeft,(control->selectable).base.top,
                   (control->selectable).base.left,subresource,control->textureSource,g_FramebufferAccess);
        g_GraphicsFramebufferEndAccess();
      }
    }
  }
  return;
}

/* nonRightPress of the image control (g_UiImageControlVtable): plays the pointer sound
   (UI_IMAGE_CONTROL_POINTER_SOUND, unless the image is already OPEN), drops the active child and the hover
   target, then toggles: a press on an opaque pixel of an already selected image clears
   UI_IMAGE_CONTROL_PRESS_STATE_BITS, any other press sets them.
*/
void UiImageControl_NonRightPress(UiPointerWheelDelta wheelDelta,UiPixelCoordinate pointerY,UiPixelCoordinate pointerX,
          UiImageControl *control)

{
  UiSelectableStateFlags *pressStateFlagsField;
  Bool8 opaqueHit;
  UiSelectableStateFlags *stateFlagsField;

  if (((control->selectable).base.nodeFlags & UI_NODE_SUPPRESSED) != 0) {
    return;
  }
  if (((control->selectable).stateFlags & UI_IMAGE_CONTROL_OPEN) == 0 &&
      ((control->selectable).stateFlags & UI_IMAGE_CONTROL_POINTER_SOUND) != 0 &&
      control->pointerActivationSound != nullptr) {
    g_SoundPlayOneShot
              (g_UiSoundGainQ15,g_UiSoundGainQ15,
               control->pointerActivationSound,nullptr);
  }
  opaqueHit = false;
  if (((control->selectable).stateFlags & UI_SELECTABLE_SELECTED_OR_CHECKED) != 0) {
    if (((control->selectable).stateFlags & UI_IMAGE_CONTROL_ALTERNATE_HIT_SHAPE) == 0) {
      opaqueHit = g_GraphicsTextureSourceTestOpaquePixel
                        (pointerY,pointerX,(control->selectable).base.top,
                         (control->selectable).base.left,control->normalSubresource,
                         control->textureSource);
    }
    else {
      opaqueHit = g_GraphicsTextureSourceTestOpaquePixel
                        (pointerY,pointerX,(control->selectable).base.top,
                         (control->selectable).base.left,control->alternateSubresource,
                         control->textureSource);
    }
  }
  control->activeChild = nullptr;
  g_UiImageControlHoverTarget = nullptr;
  if (opaqueHit) {
    /* Pressing an already selected image on an opaque pixel clears its selected/armed state. */
    stateFlagsField = &(control->selectable).stateFlags;
    *stateFlagsField = *stateFlagsField & ~UI_IMAGE_CONTROL_PRESS_STATE_BITS;
  }
  else {
    pressStateFlagsField = &(control->selectable).stateFlags;
    *pressStateFlagsField = *pressStateFlagsField | UI_IMAGE_CONTROL_PRESS_STATE_BITS;
  }
  UiNode_InvalidateRoot((UiNodeBase *)control);
  return;
}

/* nonRightRelease of the image control (g_UiImageControlVtable). A release while
   UI_IMAGE_CONTROL_PRESSED_ON_IMAGE is set keeps it open: UI_IMAGE_CONTROL_OPEN, and the image becomes
   g_UiImageControlHoverTarget. Otherwise an active child gets the release first, and the image stays open
   only if it had one and OPEN was not yet set or PRESS_STARTED is set; else it closes: hover target
   cleared, UI_IMAGE_CONTROL_HOVER_STATE_BITS cleared and the pointer sound played (POINTER_SOUND).
*/
void UiImageControl_NonRightRelease
          (UiPointerWheelDelta wheelDelta,UiPixelCoordinate pointerY,UiPixelCoordinate pointerX,
          UiImageControl *control)

{
  UiSelectableStateFlags *hoverStateFlagsField;
  UiNodeBase *previousActiveChild;
  UiSelectableStateFlags *stateFlagsField;
  UiNodeVtable *activeChildVtable;
  Bool8 preserveHover;

  previousActiveChild = control->activeChild;
  if (((control->selectable).base.nodeFlags & UI_NODE_SUPPRESSED) == 0) {
    preserveHover = ((control->selectable).stateFlags & UI_IMAGE_CONTROL_PRESSED_ON_IMAGE) != 0;
    if (!preserveHover) {
      if (previousActiveChild != nullptr) {
        activeChildVtable = previousActiveChild->vtable;
        control->activeChild = nullptr;
        activeChildVtable->nonRightRelease(wheelDelta,pointerY,pointerX,previousActiveChild);
        preserveHover = ((control->selectable).stateFlags & UI_IMAGE_CONTROL_OPEN) == 0 ||
                        ((control->selectable).stateFlags & UI_IMAGE_CONTROL_PRESS_STARTED) != 0;
      }
      if (!preserveHover) {
        g_UiImageControlHoverTarget = nullptr;
        stateFlagsField = &(control->selectable).stateFlags;
        *stateFlagsField = *stateFlagsField & ~UI_IMAGE_CONTROL_HOVER_STATE_BITS;
        if (((control->selectable).stateFlags & UI_IMAGE_CONTROL_POINTER_SOUND) != 0 &&
            control->pointerActivationSound != nullptr) {
          g_SoundPlayOneShot
                    (g_UiSoundGainQ15,g_UiSoundGainQ15,
                     control->pointerActivationSound,nullptr);
        }
      }
    }
    if (preserveHover) {
      hoverStateFlagsField = &(control->selectable).stateFlags;
      *hoverStateFlagsField = *hoverStateFlagsField | UI_IMAGE_CONTROL_OPEN;
      g_UiImageControlHoverTarget = control;
      hoverStateFlagsField = &(control->selectable).stateFlags;
      *hoverStateFlagsField = *hoverStateFlagsField & ~UI_IMAGE_CONTROL_PRESSED_ON_IMAGE;
    }
  }
  UiNode_InvalidateRoot((UiNodeBase *)control);
  return;
}

/* Hit test of an image control: only opaque pixels of its current image count, so irregular shapes react
   precisely. A miss clears UI_IMAGE_CONTROL_PRESSED_ON_IMAGE and, in persistent activation mode, passes the
   test on to the children. Returns the hit node or UI_NODE_NONE.
*/
UiNodeBase * UiImageControl_HitTestOpaque(UiPixelCoordinate pointerY,UiPixelCoordinate pointerX,UiImageControl *control)

{
  UiNodeBase *hitNode;
  Bool8 opaqueHit;

  if ((control->selectable.base.nodeFlags & UI_NODE_SUPPRESSED) != 0) {
    return UI_NODE_NONE;
  }
  if ((control->selectable.stateFlags & UI_IMAGE_CONTROL_ALTERNATE_HIT_SHAPE) == 0) {
    opaqueHit = g_GraphicsTextureSourceTestOpaquePixel
                      (pointerY,pointerX,control->selectable.base.top,
                       control->selectable.base.left,control->normalSubresource,
                       control->textureSource);
  }
  else {
    opaqueHit = g_GraphicsTextureSourceTestOpaquePixel
                      (pointerY,pointerX,control->selectable.base.top,
                       control->selectable.base.left,control->alternateSubresource,
                       control->textureSource);
  }
  if (opaqueHit) {
    return (UiNodeBase *)control;
  }
  control->selectable.stateFlags &= ~UI_IMAGE_CONTROL_PRESSED_ON_IMAGE;
  if ((control->selectable.stateFlags & UI_SELECTABLE_PERSISTENT_ACTIVATION_MODE) == 0) {
    return UI_NODE_NONE;
  }
  /* UiContainer_HitTestChildren returns the control itself when no child is hit; that only counts
     while g_UiImageControlHoverTarget is NULL */
  hitNode = UiContainer_HitTestChildren(pointerY,pointerX,(UiNodeBase *)control);
  if (hitNode == (UiNodeBase *)control && g_UiImageControlHoverTarget != nullptr) {
    return UI_NODE_NONE;
  }
  return hitNode;
}

UiNodeVtable g_UiImageControlVtable = {
        .relocate = THANDOR_FN(UiContainer_RelocateChildren),
        .method04 = THANDOR_FN(UiNode_DefaultMethod04_NoOp),
        .drawClipped = THANDOR_FN(UiImageControl_DrawClipped),
        .layout = THANDOR_FN(UiImageControl_LayoutChildrenToParent),
        .nonRightPress = THANDOR_FN(UiImageControl_NonRightPress),
        .nonRightRelease = THANDOR_FN(UiImageControl_NonRightRelease),
        .rightPress = THANDOR_FN(UiNode_ForwardRightPressToParent),
        .rightRelease = THANDOR_FN(UiNode_DefaultRightRelease),
        .nonRightDrag = THANDOR_FN(UiImageControl_NonRightDrag),
        .rightDrag = THANDOR_FN(UiNode_DefaultRightDrag),
        .pointerMove = THANDOR_FN(UiImageControl_PointerMove),
        .hitTest = THANDOR_FN(UiImageControl_HitTestOpaque),
        .keyboardEvent = THANDOR_FN(UiSelectableControl_KeyboardEvent),
        .applyFlags = THANDOR_FN(UiNode_ApplyFlagsRecursive),
        .suppressActionId = THANDOR_FN(UiSelectableControl_SuppressIfActionId),
        .unsuppressActionId = THANDOR_FN(UiSelectableControl_UnsuppressIfActionId),
        .tick = THANDOR_FN(UiImageControl_TickHover),
        .pointerWheel = THANDOR_FN(UiNode_ForwardPointerWheelToParent)};

/* pointerMove slot of g_UiImageControlVtable. Over an opaque pixel of the image the arrow
   is shown. Over a transparent pixel of a persistent-activation image, a child under the pointer supplies
   the cursor; without one, UI_IMAGE_CONTROL_CURSOR_FRAME_IDLE while no image control is hovered.
*/
GraphicsCursorFrameIndex UiImageControl_PointerMove
          (UiPixelCoordinate pointerY,UiPixelCoordinate pointerX,UiImageControl *control)

{
  UiImageControl *hitControl;
  GraphicsCursorFrameIndex cursorFrame;
  Bool8 overOpaquePixel;

  if (((control->selectable).base.nodeFlags & UI_NODE_SUPPRESSED) == 0) {
    if (((control->selectable).stateFlags & UI_IMAGE_CONTROL_ALTERNATE_HIT_SHAPE) == 0) {
      overOpaquePixel = g_GraphicsTextureSourceTestOpaquePixel
                        (pointerY,pointerX,(control->selectable).base.top,
                         (control->selectable).base.left,control->normalSubresource,
                         control->textureSource);
      if (overOpaquePixel) {
        return GRAPHICS_CURSOR_FRAME_ARROW;
      }
    }
    else {
      overOpaquePixel = g_GraphicsTextureSourceTestOpaquePixel
                        (pointerY,pointerX,(control->selectable).base.top,
                         (control->selectable).base.left,control->alternateSubresource,
                         control->textureSource);
      if (overOpaquePixel) {
        return GRAPHICS_CURSOR_FRAME_ARROW;
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
      if (g_UiImageControlHoverTarget == nullptr) {
        return UI_IMAGE_CONTROL_CURSOR_FRAME_IDLE;
      }
    }
  }
  return GRAPHICS_CURSOR_FRAME_ARROW;
}

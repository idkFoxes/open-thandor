/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/src/ui/controls/slider.cpp
 * Reverse engineering by idkFoxes 2026
 */

#include <thandor/ui/controls/slider.h>
#include <thandor/thandor.h>

/* Module data. */

/* int32_t, 1: multiplier of wheelDelta * stepValue when the mouse wheel moves a range slider */
static const int32_t g_UiRangeSliderDragScale = 1;

/* Implementation ownership: ui/controls/slider. */

/* Thumb offset along the track for UiRangeSliderControl_DrawTrackAndThumb: value (clamped to
   minimumValue..maximumValue) scaled from the range onto freeTrackLength, rounded to the nearest pixel;
   measured from the other end when invert is set. */
static uint32_t UiRangeSliderControl_ThumbOffset(const UiRangeSliderControl *control,uint32_t freeTrackLength,
                                                 Bool8 invert)
{
  int32_t rangeMax;
  int32_t clampedValue;
  uint32_t range;
  uint32_t valueOffset;
  uint64_t scaledOffset;

  rangeMax = control->maximumValue;
  clampedValue = control->value;
  if (rangeMax < control->value) {
    clampedValue = rangeMax;
  }
  range = rangeMax - control->minimumValue;
  if (range == 0) {
    range = 1;
  }
  valueOffset = clampedValue - control->minimumValue;
  if ((int)valueOffset < 0) {
    valueOffset = 0;
  }
  if (invert) {
    valueOffset = range - valueOffset;
  }
  /* offset * free track length / range, plus one when twice the remainder exceeds the range */
  scaledOffset = (uint64_t)valueOffset * (uint64_t)freeTrackLength;
  return (uint32_t)(int)(scaledOffset / range) +
         (uint32_t)(range < (uint32_t)((int)(scaledOffset % (uint64_t)range) * 2));
}

/* drawClipped of the range slider (g_UiRangeSliderControlVtable): draws the track from three
   g_UiWindowTextureSource pieces (start cap, tiled middle, end cap) and the thumb at the position of value
   within minimumValue..maximumValue, rounded to the nearest pixel. Horizontal or vertical after
   UI_RANGE_SLIDER_VERTICAL; a suppressed slider uses the greyed pieces.
*/
void UiRangeSliderControl_DrawTrackAndThumb
          (UiPixelCoordinate clipBottom,UiPixelCoordinate clipRight,UiPixelCoordinate clipTop,
          UiPixelCoordinate clipLeft,UiRangeSliderControl *control)

{
  uint32_t subresourceBase;
  int edgeLength;
  uint32_t thumbOffset;
  GraphicsTextureLogicalSize textureSize;

  if (g_GraphicsFramebufferBeginAccess()) {
    return; /* framebuffer access failed */
  }
  subresourceBase = UI_RANGE_SLIDER_SUBRESOURCE_BASE;
  if ((control->base.nodeFlags & UI_NODE_SUPPRESSED) != 0) {
    subresourceBase = UI_RANGE_SLIDER_SUBRESOURCE_BASE_SUPPRESSED;
  }
  if ((control->sliderFlags & UI_RANGE_SLIDER_VERTICAL) != 0) {
    g_GraphicsTextureSourceBlitSourceAlpha
              (clipBottom,clipRight,clipTop,clipLeft,control->base.top,control->base.left,
               subresourceBase + UI_RANGE_SLIDER_SUBRESOURCE_VERTICAL_OFFSET,
               g_UiWindowTextureSource,g_FramebufferAccess);
    textureSize = g_GraphicsTextureSourceGetLogicalSize(subresourceBase + UI_RANGE_SLIDER_SUBRESOURCE_VERTICAL_OFFSET,
                                                        g_UiWindowTextureSource);
    edgeLength = control->base.layoutHeight - textureSize.logicalHeightPixels;
    UiWindow_BlitTiledVerticalEdge
              (clipBottom,clipRight,clipTop,clipLeft,
               subresourceBase + UI_RANGE_SLIDER_SUBRESOURCE_VERTICAL_OFFSET + UI_RANGE_SLIDER_PIECE_TRACK,
               edgeLength,textureSize.logicalHeightPixels,0,&control->base);
    g_GraphicsTextureSourceBlitSourceAlpha
              (clipBottom,clipRight,clipTop,clipLeft,edgeLength + control->base.top,control->base.left,
               subresourceBase + UI_RANGE_SLIDER_SUBRESOURCE_VERTICAL_OFFSET + UI_RANGE_SLIDER_PIECE_END_CAP,
               g_UiWindowTextureSource,g_FramebufferAccess);
    /* the thumb; vertical sliders have their maximum at the top unless reversed */
    textureSize = g_GraphicsTextureSourceGetLogicalSize
              (subresourceBase + UI_RANGE_SLIDER_SUBRESOURCE_VERTICAL_OFFSET + UI_RANGE_SLIDER_PIECE_THUMB,
               g_UiWindowTextureSource);
    thumbOffset = UiRangeSliderControl_ThumbOffset
              (control,control->base.layoutHeight - textureSize.logicalHeightPixels,
               (control->sliderFlags & UI_RANGE_SLIDER_REVERSED) == 0);
    g_GraphicsTextureSourceBlitSourceAlpha
              (clipBottom,clipRight,clipTop,clipLeft,thumbOffset + control->base.top,control->base.left,
               subresourceBase + UI_RANGE_SLIDER_SUBRESOURCE_VERTICAL_OFFSET + UI_RANGE_SLIDER_PIECE_THUMB,
               g_UiWindowTextureSource,g_FramebufferAccess);
  }
  else {
    g_GraphicsTextureSourceBlitSourceAlpha
              (clipBottom,clipRight,clipTop,clipLeft,control->base.top,control->base.left,subresourceBase,
               g_UiWindowTextureSource,g_FramebufferAccess);
    textureSize = g_GraphicsTextureSourceGetLogicalSize(subresourceBase,g_UiWindowTextureSource);
    edgeLength = control->base.layoutWidth - textureSize.logicalWidthPixels;
    UiWindow_BlitTiledHorizontalEdge
              (clipBottom,clipRight,clipTop,clipLeft,subresourceBase + UI_RANGE_SLIDER_PIECE_TRACK,edgeLength,0,
               textureSize.logicalWidthPixels,&control->base);
    g_GraphicsTextureSourceBlitSourceAlpha
              (clipBottom,clipRight,clipTop,clipLeft,control->base.top,edgeLength + control->base.left,
               subresourceBase + UI_RANGE_SLIDER_PIECE_END_CAP,
               g_UiWindowTextureSource,g_FramebufferAccess);
    /* the thumb */
    textureSize = g_GraphicsTextureSourceGetLogicalSize(subresourceBase + UI_RANGE_SLIDER_PIECE_THUMB,
                                                        g_UiWindowTextureSource);
    thumbOffset = UiRangeSliderControl_ThumbOffset
              (control,control->base.layoutWidth - textureSize.logicalWidthPixels,
               (control->sliderFlags & UI_RANGE_SLIDER_REVERSED) != 0);
    g_GraphicsTextureSourceBlitSourceAlpha
              (clipBottom,clipRight,clipTop,clipLeft,control->base.top,thumbOffset + control->base.left,
               subresourceBase + UI_RANGE_SLIDER_PIECE_THUMB,g_UiWindowTextureSource,g_FramebufferAccess);
  }
  g_GraphicsFramebufferEndAccess();
}

/* nonRightPress of the range slider (g_UiRangeSliderControlVtable): a press inside the slider, within the
   thumb's cross size (its height for a horizontal slider, its width for a vertical one), starts a thumb
   drag and plays the click sound when UI_RANGE_SLIDER_CLICK_SOUND is set.
*/
void UiRangeSliderControl_BeginThumbDrag
          (UiPointerWheelDelta wheelDelta,UiPixelCoordinate pointerY,UiPixelCoordinate pointerX,
          UiRangeSliderControl *control)

{
  int localX;
  int localY;
  GraphicsTextureLogicalSize thumbSize;

  if ((control->base.nodeFlags & UI_NODE_SUPPRESSED) != 0) {
    return;
  }
  if (pointerX < control->base.left || pointerY < control->base.top) {
    return;
  }
  localX = pointerX - control->base.left;
  localY = pointerY - control->base.top;
  if (localX >= control->base.layoutWidth || localY >= control->base.layoutHeight) {
    return;
  }
  if ((control->sliderFlags & UI_RANGE_SLIDER_VERTICAL) == 0) {
    thumbSize = g_GraphicsTextureSourceGetLogicalSize(UI_RANGE_SLIDER_SUBRESOURCE_HORIZONTAL_THUMB,
                                                      g_UiWindowTextureSource);
    if ((int)thumbSize.logicalHeightPixels <= localY) {
      return;
    }
  }
  else {
    thumbSize = g_GraphicsTextureSourceGetLogicalSize(UI_RANGE_SLIDER_SUBRESOURCE_VERTICAL_THUMB,
                                                      g_UiWindowTextureSource);
    if ((int)thumbSize.logicalWidthPixels <= localX) {
      return;
    }
  }
  control->sliderFlags = control->sliderFlags | UI_RANGE_SLIDER_DRAGGING;
  if (((control->sliderFlags & UI_RANGE_SLIDER_CLICK_SOUND) != 0) && (control->clickSound != nullptr)) {
    g_SoundPlayOneShot(g_UiSoundGainQ15,g_UiSoundGainQ15,control->clickSound,nullptr);
  }
}

/* nonRightRelease of the range slider (g_UiRangeSliderControlVtable): ends a thumb drag and plays the click
   sound when UI_RANGE_SLIDER_CLICK_SOUND is set and the slider is not suppressed.
*/
void UiRangeSliderControl_EndThumbDrag
               (UiPointerWheelDelta wheelDelta,UiPixelCoordinate pointerY,UiPixelCoordinate pointerX,
               UiRangeSliderControl *control)

{
  control->sliderFlags = control->sliderFlags & ~UI_RANGE_SLIDER_DRAGGING;
  if ((((control->base.nodeFlags & UI_NODE_SUPPRESSED) == 0) &&
       ((control->sliderFlags & UI_RANGE_SLIDER_CLICK_SOUND) != 0)) && (control->clickSound != nullptr)) {
    g_SoundPlayOneShot(g_UiSoundGainQ15,g_UiSoundGainQ15,control->clickSound,nullptr);
  }
}

/* suppressActionId of the range slider (g_UiRangeSliderControlVtable): a slider with this action id is
   greyed out (UI_NODE_SUPPRESSED), gives up the keyboard focus and is redrawn.
*/
void UiRangeSliderControl_SuppressIfActionId(UiActionId actionId,UiRangeSliderControl *control)

{
  if (actionId == control->actionId) {
    control->base.nodeFlags = control->base.nodeFlags | UI_NODE_SUPPRESSED;
    UiKeyboardFocus_ReleaseNode(&control->base);
    UiNode_InvalidateRoot(&control->base);
  }
}

/* unsuppressActionId of the range slider (g_UiRangeSliderControlVtable): a slider with this action id is
   enabled again, takes the keyboard focus if nobody has it and is redrawn.
*/
void UiRangeSliderControl_UnsuppressIfActionId(UiActionId actionId,UiRangeSliderControl *control)

{
  if (actionId == control->actionId) {
    control->base.nodeFlags = control->base.nodeFlags & ~UI_NODE_SUPPRESSED;
    UiKeyboardFocus_AcquireIfNone(&control->base);
    UiNode_InvalidateRoot(&control->base);
  }
}

UiNodeVtable g_UiRangeSliderControlVtable = {
        .relocate = UI_SLOT(UiContainer_RelocateChildren),
        .method04 = UI_SLOT(UiNode_DefaultMethod04_NoOp),
        .drawClipped = UI_SLOT(UiRangeSliderControl_DrawTrackAndThumb),
        .layout = UI_SLOT(UiContainer_LayoutChildren),
        .nonRightPress = UI_SLOT(UiRangeSliderControl_BeginThumbDrag),
        .nonRightRelease = UI_SLOT(UiRangeSliderControl_EndThumbDrag),
        .rightPress = UI_SLOT(UiNode_ForwardRightPressToParent),
        .rightRelease = UI_SLOT(UiNode_DefaultRightRelease),
        .nonRightDrag = UI_SLOT(UiRangeSliderControl_UpdateValueFromPointer),
        .rightDrag = UI_SLOT(UiNode_DefaultRightDrag),
        .pointerMove = UI_SLOT(UiNode_DefaultPointerMove),
        .hitTest = UI_SLOT(UiContainer_HitTestChildren),
        .keyboardEvent = UI_SLOT(UiRangeSliderControl_HandleKeyboard),
        .applyFlags = UI_SLOT(UiNode_ApplyFlagsRecursive),
        .suppressActionId = UI_SLOT(UiRangeSliderControl_SuppressIfActionId),
        .unsuppressActionId = UI_SLOT(UiRangeSliderControl_UnsuppressIfActionId),
        .tick = UI_SLOT(UiNode_DefaultTick),
        .pointerWheel = UI_SLOT(UiRangeSliderControl_HandlePointerWheel)};

/* keyboardEvent slot of g_UiRangeSliderControlVtable. Left/Right (Down/Up for a vertical slider) move the
   value by stepValue, with Ctrl straight to the minimum/maximum; each step plays the click sound, queues
   actionId and redraws. Other keys, and all keys while suppressed, go to the default handler, which passes
   them on. Returns false when the key was consumed.
*/
Bool8 UiRangeSliderControl_HandleKeyboard
          (UiKeyboardStateMask keyboardStateMask,UiKeyboardEventCode keyCode,UiRangeSliderControl *control)

{
  int32_t adjustedSliderValue;
  UiKeyboardEventCode decreaseKey;
  UiKeyboardEventCode increaseKey;

  if ((control->base.nodeFlags & UI_NODE_SUPPRESSED) != 0) {
    return UiNode_DefaultKeyboardEventMoveFocusNext(keyboardStateMask,keyCode,&control->base);
  }
  if ((control->sliderFlags & UI_RANGE_SLIDER_VERTICAL) == 0) {
    decreaseKey = KEYBOARD_KEY_CODE_LEFT;
    increaseKey = KEYBOARD_KEY_CODE_RIGHT;
  }
  else {
    decreaseKey = KEYBOARD_KEY_CODE_DOWN;
    increaseKey = KEYBOARD_KEY_CODE_UP;
  }
  if (keyCode == decreaseKey) {
    if ((keyboardStateMask & KEYBOARD_STATE_CTRL) != 0) {
      adjustedSliderValue = control->minimumValue;
    }
    else {
      adjustedSliderValue = control->value - control->stepValue;
      if (adjustedSliderValue < control->minimumValue) {
        adjustedSliderValue = control->minimumValue;
      }
    }
  }
  else if (keyCode == increaseKey) {
    if ((keyboardStateMask & KEYBOARD_STATE_CTRL) != 0) {
      adjustedSliderValue = control->maximumValue;
    }
    else {
      adjustedSliderValue = control->value + control->stepValue;
      if (control->maximumValue < adjustedSliderValue) {
        adjustedSliderValue = control->maximumValue;
      }
    }
  }
  else {
    return UiNode_DefaultKeyboardEventMoveFocusNext(keyboardStateMask,keyCode,&control->base);
  }
  control->value = adjustedSliderValue;
  if (((control->sliderFlags & UI_RANGE_SLIDER_CLICK_SOUND) != 0) && (control->clickSound != nullptr)) {
    g_SoundPlayOneShot(g_UiSoundGainQ15,g_UiSoundGainQ15,control->clickSound,nullptr);
  }
  UiActionQueue_Enqueue(control->actionId,&control->base);
  UiNode_InvalidateRoot(&control->base);
  return false;
}

/* nonRightDrag slot of g_UiRangeSliderControlVtable. While the thumb is dragged, maps the pointer position
   (thumb centre) along the track onto minimumValue..maximumValue, rounded to nearest and mirrored for
   reversed sliders, then queues actionId and redraws.
*/
void UiRangeSliderControl_UpdateValueFromPointer
          (UiPointerWheelDelta wheelDelta,UiPixelCoordinate pointerY,UiPixelCoordinate pointerX,
          UiRangeSliderControl *control)

{
  uint64_t scaledOffset;
  uint32_t pointerOffset;
  int32_t sliderValue;
  uint32_t trackLength;
  GraphicsTextureLogicalSize thumbSize;

  if ((control->sliderFlags & UI_RANGE_SLIDER_DRAGGING) != 0) {
    if ((control->sliderFlags & UI_RANGE_SLIDER_VERTICAL) == 0) {
      thumbSize = g_GraphicsTextureSourceGetLogicalSize(UI_RANGE_SLIDER_SUBRESOURCE_HORIZONTAL_THUMB,
                                                        g_UiWindowTextureSource);
      trackLength = control->base.layoutWidth - thumbSize.logicalWidthPixels;
      if (trackLength == 0) {
        trackLength = 1;
      }
      pointerOffset = (pointerX - ((int)thumbSize.logicalWidthPixels >> 1)) - control->base.left;
      if ((int)pointerOffset < 0) {
        pointerOffset = 0;
      }
      scaledOffset = (uint64_t)pointerOffset *
              (uint64_t)(uint32_t)(control->maximumValue - control->minimumValue);
      /* offset * range / trackLength, plus one when the remainder is more than half the track */
      sliderValue = control->minimumValue +
               (uint32_t)(trackLength < (uint32_t)((int)(scaledOffset % (uint64_t)trackLength) * 2)) +
               (int)(scaledOffset / trackLength);
      if (control->maximumValue < sliderValue) {
        sliderValue = control->maximumValue;
      }
      if ((control->sliderFlags & UI_RANGE_SLIDER_REVERSED) != 0) {
        sliderValue = control->maximumValue - (sliderValue - control->minimumValue);
      }
      control->value = sliderValue;
      UiActionQueue_Enqueue(control->actionId,&control->base);
      UiNode_InvalidateRoot(&control->base);
      return;
    }
    thumbSize = g_GraphicsTextureSourceGetLogicalSize(UI_RANGE_SLIDER_SUBRESOURCE_VERTICAL_THUMB,
                                                      g_UiWindowTextureSource);
    trackLength = control->base.layoutHeight - thumbSize.logicalHeightPixels;
    if (trackLength == 0) {
      trackLength = 1;
    }
    pointerOffset = (control->base.bottom - pointerY) - ((int)thumbSize.logicalHeightPixels >> 1);
    if ((int)pointerOffset < 0) {
      pointerOffset = 0;
    }
    scaledOffset = (uint64_t)pointerOffset *
            (uint64_t)(uint32_t)(control->maximumValue - control->minimumValue);
    sliderValue = control->minimumValue +
             (uint32_t)(trackLength < (uint32_t)((int)(scaledOffset % (uint64_t)trackLength) * 2)) +
             (int)(scaledOffset / trackLength);
    if (control->maximumValue < sliderValue) {
      sliderValue = control->maximumValue;
    }
    if ((control->sliderFlags & UI_RANGE_SLIDER_REVERSED) != 0) {
      sliderValue = control->maximumValue - (sliderValue - control->minimumValue);
    }
    control->value = sliderValue;
    UiActionQueue_Enqueue(control->actionId,&control->base);
    UiNode_InvalidateRoot(&control->base);
  }
}

/* pointerWheel slot of g_UiRangeSliderControlVtable. Unless the thumb is being dragged, each wheel notch
   moves the value by stepValue * g_UiRangeSliderDragScale, clamped to the range; then actionId is queued
   and the slider redrawn.
*/
void UiRangeSliderControl_HandlePointerWheel
          (UiPointerWheelDelta wheelDelta,UiPixelCoordinate pointerY,UiPixelCoordinate pointerX,
          UiRangeSliderControl *control)

{
  int32_t adjustedSliderValue;

  if ((control->sliderFlags & UI_RANGE_SLIDER_DRAGGING) == 0 && (control->base.nodeFlags & UI_NODE_SUPPRESSED) == 0 &&
      wheelDelta != 0) {
    adjustedSliderValue =
         control->value + wheelDelta * g_UiRangeSliderDragScale * control->stepValue;
    if (adjustedSliderValue < control->minimumValue) {
      adjustedSliderValue = control->minimumValue;
    }
    if (control->maximumValue < adjustedSliderValue) {
      adjustedSliderValue = control->maximumValue;
    }
    control->value = adjustedSliderValue;
    UiActionQueue_Enqueue(control->actionId,&control->base);
    UiNode_InvalidateRoot(&control->base);
  }
}

/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/src/ui/controls/slider.cpp
 * Reverse engineering by idkFoxes 2026
 */

#include <thandor/ui/controls/slider.h>
#include <thandor/thandor.h>

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
  return;
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
  if (((control->sliderFlags & UI_RANGE_SLIDER_CLICK_SOUND) != 0) && (control->clickSound != NULL)) {
    g_SoundPlayOneShot(g_UiSoundGainQ15,g_UiSoundGainQ15,control->clickSound,NULL);
  }
  return;
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
       ((control->sliderFlags & UI_RANGE_SLIDER_CLICK_SOUND) != 0)) && (control->clickSound != NULL)) {
    g_SoundPlayOneShot(g_UiSoundGainQ15,g_UiSoundGainQ15,control->clickSound,NULL);
  }
  return;
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
  return;
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
  return;
}

UiNodeVtable g_UiRangeSliderControlVtable = {
        .relocate = THANDOR_FN(UiContainer_RelocateChildren),
        .method04 = THANDOR_FN(UiNode_DefaultMethod04_NoOp),
        .drawClipped = THANDOR_FN(UiRangeSliderControl_DrawTrackAndThumb),
        .layout = THANDOR_FN(UiContainer_LayoutChildren),
        .nonRightPress = THANDOR_FN(UiRangeSliderControl_BeginThumbDrag),
        .nonRightRelease = THANDOR_FN(UiRangeSliderControl_EndThumbDrag),
        .rightPress = THANDOR_FN(UiNode_ForwardRightPressToParent),
        .rightRelease = THANDOR_FN(UiNode_DefaultRightRelease),
        .nonRightDrag = THANDOR_FN(UiRangeSliderControl_UpdateValueFromPointer),
        .rightDrag = THANDOR_FN(UiNode_DefaultRightDrag),
        .pointerMove = THANDOR_FN(UiNode_DefaultPointerMove),
        .hitTest = THANDOR_FN(UiContainer_HitTestChildren),
        .keyboardEvent = THANDOR_FN(UiRangeSliderControl_HandleKeyboard),
        .applyFlags = THANDOR_FN(UiNode_ApplyFlagsRecursive),
        .suppressActionId = THANDOR_FN(UiRangeSliderControl_SuppressIfActionId),
        .unsuppressActionId = THANDOR_FN(UiRangeSliderControl_UnsuppressIfActionId),
        .tick = THANDOR_FN(UiNode_DefaultTick),
        .pointerWheel = THANDOR_FN(UiRangeSliderControl_HandlePointerWheel)};

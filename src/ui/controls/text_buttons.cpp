/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/src/ui/controls/text_buttons.cpp
 * Reverse engineering by idkFoxes 2026
 */

#include <thandor/ui/controls/text_buttons.h>
#include <thandor/thandor.h>

/* Module data. */

const uint32_t g_UiTextStyleNormal = 0;

const int32_t g_UiWindowFrameInset = 2;

uint16_t g_GraphicsAdapterFormatScratch0Utf16[16] = {0};

uint16_t g_GraphicsAdapterFormatScratch1Utf16[16] = {0};

/* UiPackedTextStyle, 0x10000 (palette byte 1): text style of the selected/highlighted row or item (src/ui/controls/text.c). */
const UiPackedTextStyle g_UiTextStyleSelected = 0x10000;

/* UiPackedTextStyle, 0x20000 (palette byte 2): text style of disabled items (src/ui/controls/text.c). */
const UiPackedTextStyle g_UiTextStyleDisabled = 0x20000;

static const uint32_t g_UiTextStyleAlternate = 0;

static uint16_t g_UiNumericPairFirstValueScratchUtf16[16] = {0};

static uint16_t g_UiNumericPairSecondValueScratchUtf16[16] = {0};

/* Implementation ownership: ui/controls/text_buttons. */

/* Draws a graphics-adapter option button (drawClipped slot of g_UiGraphicsAdapterTextButtonVtable): patches
   rich-text payloads 0 and 1 of its text and draws it as a text button. The values are the two dwords stored
   just before the node (control[-1].packedTextStyle at -8 is the adapter index or first number,
   control[-1].textResourceId at -0xC the second number). State bit 0x80: only the first number; bit 0x800:
   the adapter's driver description and as device name text 0x111 (every adapter is a software renderer
   device; the original showed a hardware renderer device's own name instead); neither: both numbers.
*/
void UiGraphicsAdapterTextButton_DrawFormattedAdapterText
          (UiPixelCoordinate clipBottom,UiPixelCoordinate clipRight,UiPixelCoordinate clipTop,
          UiPixelCoordinate clipLeft,UiTextButtonControl *control)

{
  UiPackedTextStyle adapterIndex;
  uint16_t *stream;
  uint16_t *resolvedText;

  if (((control->selectable).base.nodeFlags & UI_NODE_SUPPRESSED) == 0) {
    resolvedText = TextResource_Resolve(control->textResourceId);
    stream = resolvedText;
    if (((control->selectable).stateFlags & UI_ADAPTER_TEXT_BUTTON_SINGLE_NUMBER) != 0) {
      g_WideNumberFormatUtf16
                (WIDE_FORMAT_WRITE_TERMINATOR,0,10,1,control[-1].packedTextStyle,
                 g_GraphicsAdapterFormatScratch0Utf16);
      RichTextCommandStream_PatchPayloadBySelector(0,g_GraphicsAdapterFormatScratch0Utf16,stream);
      UiTextButtonControl_DrawClipped(clipBottom,clipRight,clipTop,clipLeft,control);
      return;
    }
    if (((control->selectable).stateFlags & UI_ADAPTER_TEXT_BUTTON_ADAPTER_NAME) == 0) {
      g_WideNumberFormatUtf16
                (WIDE_FORMAT_WRITE_TERMINATOR,0,10,1,control[-1].packedTextStyle,
                 g_GraphicsAdapterFormatScratch0Utf16);
      g_WideNumberFormatUtf16
                (WIDE_FORMAT_WRITE_TERMINATOR,0,10,1,control[-1].textResourceId,
                 g_GraphicsAdapterFormatScratch1Utf16);
      RichTextCommandStream_PatchPayloadBySelector(0,g_GraphicsAdapterFormatScratch0Utf16,stream);
      RichTextCommandStream_PatchPayloadBySelector(1,g_GraphicsAdapterFormatScratch1Utf16,stream);
      UiTextButtonControl_DrawClipped(clipBottom,clipRight,clipTop,clipLeft,control);
      return;
    }
    adapterIndex = control[-1].packedTextStyle;
    RichTextCommandStream_PatchPayloadBySelector
              (0,g_GraphicsAdapters[adapterIndex].driverDescriptionUtf16,stream);
    resolvedText = TextResource_Resolve(TEXT_ID_PRIMARY_DISPLAY_ADAPTER);
    RichTextCommandStream_PatchPayloadBySelector(1,resolvedText,stream);
    UiTextButtonControl_DrawClipped(clipBottom,clipRight,clipTop,clipLeft,control);
  }
  return;
}

/* Draws a text button showing two numbers (drawClipped slot of g_UiNumericPairTextButtonVtable, e.g. a
   display resolution): formats firstValue and secondValue as decimal into rich-text payloads 0 and 1 of its
   text, then draws it as a text button.
*/
void UiNumericPairTextButton_DrawFormattedValues
          (UiPixelCoordinate clipBottom,UiPixelCoordinate clipRight,UiPixelCoordinate clipTop,
          UiPixelCoordinate clipLeft,UiNumericPairTextButton *control)

{
  uint16_t *resolvedText;

  if (((control->base).selectable.base.nodeFlags & UI_NODE_SUPPRESSED) == 0) {
    resolvedText = TextResource_Resolve((control->base).textResourceId);
    g_WideNumberFormatUtf16
              (WIDE_FORMAT_WRITE_TERMINATOR,0,10,1,control->firstValue,
               g_UiNumericPairFirstValueScratchUtf16);
    g_WideNumberFormatUtf16
              (WIDE_FORMAT_WRITE_TERMINATOR,0,10,1,control->secondValue,
               g_UiNumericPairSecondValueScratchUtf16);
    RichTextCommandStream_PatchPayloadBySelector(0,g_UiNumericPairFirstValueScratchUtf16,resolvedText);
    RichTextCommandStream_PatchPayloadBySelector(1,g_UiNumericPairSecondValueScratchUtf16,resolvedText);
    UiTextButtonControl_DrawClipped(clipBottom,clipRight,clipTop,clipLeft,&control->base);
  }
  return;
}

/* Draws a text button with two text payloads (drawClipped slot of g_UiPayloadPairTextButtonVtable): patches
   firstPayload and secondPayload into rich-text payloads 0 and 1 of its text, then draws it as a text
   button.
*/
void UiPayloadPairTextButton_DrawFormattedPayloads
          (UiPixelCoordinate clipBottom,UiPixelCoordinate clipRight,UiPixelCoordinate clipTop,
          UiPixelCoordinate clipLeft,UiPayloadPairTextButton *control)

{
  uint16_t *resolvedText;

  if (((control->base).selectable.base.nodeFlags & UI_NODE_SUPPRESSED) == 0) {
    resolvedText = TextResource_Resolve((control->base).textResourceId);
    RichTextCommandStream_PatchPayloadBySelector(0,control->firstPayload,resolvedText);
    RichTextCommandStream_PatchPayloadBySelector(1,control->secondPayload,resolvedText);
    UiTextButtonControl_DrawClipped(clipBottom,clipRight,clipTop,clipLeft,&control->base);
  }
  return;
}

/* Relocation of a loaded framed text button (relocate slot of g_UiFramedTextButtonControlVtable): an inset-framed
   button (UI_BUTTON_FRAME_INSET) grows its layout offsets by g_UiWindowFrameInset on every side, so the frame
   lies outside the authored box; then the children are relocated.
*/
void UiFramedTextButtonControl_Relocate(UiSerializedRelocationDelta relocationDelta,UiFramedTextButtonControl *control)

{
  int32_t *bottomOffsetField;
  int32_t *frameEdgeOffsetField;
  int32_t *edgeOffsetField;
  int32_t *trailingEdgeOffsetField;
  int frameInset;
  
  frameInset = g_UiWindowFrameInset;
  if (((control->selectable).stateFlags & UI_BUTTON_FRAME_INSET) != 0) {
    edgeOffsetField = &(control->selectable).base.leftOffset;
    *edgeOffsetField = *edgeOffsetField - g_UiWindowFrameInset;
    frameEdgeOffsetField = &(control->selectable).base.topOffset;
    *frameEdgeOffsetField = *frameEdgeOffsetField - frameInset;
    trailingEdgeOffsetField = &(control->selectable).base.rightOffset;
    *trailingEdgeOffsetField = *trailingEdgeOffsetField + frameInset;
    bottomOffsetField = &(control->selectable).base.bottomOffset;
    *bottomOffsetField = *bottomOffsetField + frameInset;
  }
  UiContainer_RelocateChildren(relocationDelta,(UiNodeBase *)control);
  return;
}

/* Draws a framed text button (drawClipped slot of g_UiFramedTextButtonControlVtable): the normal, selected or disabled
   win.gfx frame (plain or inset), its text centred in the state's style, with the focus mark and its shadow
   behind the text while it has keyboard focus, then the children unless the button is suppressed.
*/
void UiFramedTextButtonControl_DrawClipped
          (UiPixelCoordinate clipBottom,UiPixelCoordinate clipRight,UiPixelCoordinate clipTop,
          UiPixelCoordinate clipLeft,UiFramedTextButtonControl *control)

{
  uint32_t cornerWidth;
  uint16_t *commandStream;
  uint32_t framePiece;
  uint32_t textWidth;
  uint32_t cornerHeight;
  int rightCornerX;
  int bottomCornerY;
  int focusMarkTop;
  int focusTileX;
  int focusMarkRightX;
  uint32_t textStyle;
  Bool8 framebufferUnavailable;
  Bool8 drawFrame;
  RichTextExtent textExtent;
  GraphicsTextureLogicalSize cornerTileSize;
  GraphicsTextureLogicalSize rightCapSize;
  GraphicsTextureLogicalSize leftCapSize;
  GraphicsTextureLogicalSize middleTileSize;
  int textX;
  int textY;

  framebufferUnavailable = g_GraphicsFramebufferBeginAccess();
  if (framebufferUnavailable) {
    drawFrame = false;
  }
  else if (((control->selectable).base.nodeFlags & UI_NODE_SUPPRESSED) == 0) {
    drawFrame = true;
    if (((control->selectable).stateFlags & UI_BUTTON_FRAME_INSET) == 0) {
      framePiece = UI_WINDOW_SUBRESOURCE_BUTTON_FRAME;
    }
    else {
      framePiece = UI_WINDOW_SUBRESOURCE_INSET_BUTTON_FRAME;
    }
    if (((control->selectable).stateFlags & UI_SELECTABLE_SELECTED_OR_CHECKED) != 0) {
      framePiece = framePiece + UI_WINDOW_FRAME_PIECE_COUNT;
    }
  }
  else {
    /* Suppressed: the disabled frame, unless UI_BUTTON_HIDDEN_WHILE_SUPPRESSED hides it. */
    drawFrame = ((control->selectable).stateFlags & UI_BUTTON_HIDDEN_WHILE_SUPPRESSED) == 0;
    if (((control->selectable).stateFlags & UI_BUTTON_FRAME_INSET) == 0) {
      framePiece = UI_WINDOW_SUBRESOURCE_BUTTON_FRAME_DISABLED;
    }
    else {
      framePiece = UI_WINDOW_SUBRESOURCE_INSET_BUTTON_FRAME_DISABLED;
    }
  }
  if (drawFrame) {
    cornerTileSize = g_GraphicsTextureSourceGetLogicalSize(framePiece,g_UiWindowTextureSource);
    cornerHeight = cornerTileSize.logicalHeightPixels;
    cornerWidth = cornerTileSize.logicalWidthPixels;
    rightCornerX = (control->selectable).base.layoutWidth - cornerWidth;
    bottomCornerY = (control->selectable).base.layoutHeight - cornerHeight;
    if ((((control->selectable).base.nodeFlags & UI_NODE_SUPPRESSED) == 0) ||
       (((control->selectable).stateFlags & UI_BUTTON_NO_FRAME_WHILE_SUPPRESSED) == 0)) {
      g_GraphicsTextureSourceBlitSourceAlpha
                (clipBottom,clipRight,clipTop,clipLeft,(control->selectable).base.top,
                 (control->selectable).base.left,framePiece,g_UiWindowTextureSource,g_FramebufferAccess);
      g_GraphicsTextureSourceBlitSourceAlpha
                (clipBottom,clipRight,clipTop,clipLeft,(control->selectable).base.top,
                 rightCornerX + (control->selectable).base.left,
                 framePiece + UI_WINDOW_FRAME_TOP_RIGHT,g_UiWindowTextureSource,
                 g_FramebufferAccess);
      g_GraphicsTextureSourceBlitSourceAlpha
                (clipBottom,clipRight,clipTop,clipLeft,bottomCornerY + (control->selectable).base.top,
                 (control->selectable).base.left,framePiece + UI_WINDOW_FRAME_BOTTOM_LEFT,
                 g_UiWindowTextureSource,
                 g_FramebufferAccess);
      g_GraphicsTextureSourceBlitSourceAlpha
                (clipBottom,clipRight,clipTop,clipLeft,bottomCornerY + (control->selectable).base.top,
                 rightCornerX + (control->selectable).base.left,
                 framePiece + UI_WINDOW_FRAME_BOTTOM_RIGHT,g_UiWindowTextureSource,
                 g_FramebufferAccess);
      UiWindow_BlitTiledHorizontalEdge
                (clipBottom,clipRight,clipTop,clipLeft,framePiece + UI_WINDOW_FRAME_TOP,rightCornerX,
                 0,cornerWidth,control);
      UiWindow_BlitTiledVerticalEdge
                (clipBottom,clipRight,clipTop,clipLeft,framePiece + UI_WINDOW_FRAME_LEFT,
                 bottomCornerY,cornerHeight,0,control);
      UiWindow_BlitTiledVerticalEdge
                (clipBottom,clipRight,clipTop,clipLeft,framePiece + UI_WINDOW_FRAME_RIGHT,
                 bottomCornerY,cornerHeight,rightCornerX,control);
      UiWindow_BlitTiledHorizontalEdge
                (clipBottom,clipRight,clipTop,clipLeft,framePiece + UI_WINDOW_FRAME_BOTTOM,
                 rightCornerX,bottomCornerY,cornerWidth,control);
    }
    commandStream = TextResource_Resolve(control->textResourceId);
    textExtent = RichTextCommandStream_MeasureLine(control->packedTextStyle,commandStream);
    textWidth = textExtent.widthPixels;
    /* centred offsets inside the button */
    textX = (int)((control->selectable).base.layoutWidth - textWidth) >> 1;
    textY = (int)((control->selectable).base.layoutHeight - textExtent.heightPixels) >> 1;
    textStyle = g_UiTextStyleDisabled;
    if (((control->selectable).base.nodeFlags & UI_NODE_SUPPRESSED) == 0) {
      textStyle = g_UiTextStyleNormal;
      if (((control->selectable).stateFlags & UI_SELECTABLE_SELECTED_OR_CHECKED) != 0) {
        textStyle = g_UiTextStyleSelected;
      }
    }
    /* packedTextStyle may override the font byte (bits 24-31) and the palette byte (bits 16-23) */
    if (((control->selectable).stateFlags & UI_BUTTON_OWN_STYLE_FONT) == 0) {
      control->packedTextStyle = control->packedTextStyle & UI_TEXT_STYLE_PALETTE_BYTE;
    }
    else {
      textStyle = textStyle & ~UI_TEXT_STYLE_FONT_BYTE;
    }
    if (((control->selectable).stateFlags & UI_BUTTON_OWN_STYLE_PALETTE) == 0) {
      control->packedTextStyle = control->packedTextStyle & UI_TEXT_STYLE_FONT_BYTE;
    }
    else {
      textStyle = textStyle & ~UI_TEXT_STYLE_PALETTE_BYTE;
    }
    textStyle = textStyle | control->packedTextStyle;
    if ((((control->selectable).base.nodeFlags & UI_NODE_HAS_KEYBOARD_FOCUS) == 0) ||
       (((control->selectable).stateFlags & UI_BUTTON_NO_FOCUS_MARK) != 0)) {
      RichTextCommandStream_DrawSingleLine
                (clipBottom,clipRight,clipTop,clipLeft,textStyle,commandStream,
                 textY + (control->selectable).base.top,
                 textX + (control->selectable).base.left);
    }
    else {
      textX = textX + (control->selectable).base.left;
      textY = textY + (control->selectable).base.top;
      /* focus mark shadow: left cap, right cap, then the middle tiles up to the right cap */
      g_GraphicsTextureSourceBlitModulatedSourceAlpha
                (clipBottom,clipRight,clipTop,clipLeft,textY,textX - 2,
                 TEXT_SHADOW_COLOR_ARGB,UI_WINDOW_SUBRESOURCE_FOCUS_MARK_LEFT,
                 g_UiWindowTextureSource,g_FramebufferAccess);
      rightCapSize = g_GraphicsTextureSourceGetLogicalSize(UI_WINDOW_SUBRESOURCE_FOCUS_MARK_RIGHT,
                                                           g_UiWindowTextureSource);
      leftCapSize = g_GraphicsTextureSourceGetLogicalSize(UI_WINDOW_SUBRESOURCE_FOCUS_MARK_LEFT,
                                                          g_UiWindowTextureSource);
      focusTileX = textX - 2 + leftCapSize.logicalWidthPixels;
      middleTileSize = g_GraphicsTextureSourceGetLogicalSize(UI_WINDOW_SUBRESOURCE_FOCUS_MARK_MIDDLE,
                                                             g_UiWindowTextureSource);
      focusMarkRightX = ((textX + 4) - rightCapSize.logicalWidthPixels) + textWidth;
      g_GraphicsTextureSourceBlitModulatedSourceAlpha
                (clipBottom,clipRight,clipTop,clipLeft,textY,focusMarkRightX,TEXT_SHADOW_COLOR_ARGB,
                 UI_WINDOW_SUBRESOURCE_FOCUS_MARK_RIGHT,
                 g_UiWindowTextureSource,g_FramebufferAccess);
      if (clipRight < focusMarkRightX) {
        focusMarkRightX = clipRight;
      }
      do {
        g_GraphicsTextureSourceBlitModulatedSourceAlpha
                  (clipBottom,focusMarkRightX,clipTop,clipLeft,textY,focusTileX,
                   TEXT_SHADOW_COLOR_ARGB,UI_WINDOW_SUBRESOURCE_FOCUS_MARK_MIDDLE,
                   g_UiWindowTextureSource,g_FramebufferAccess);
        focusTileX = focusTileX + middleTileSize.logicalWidthPixels;
      } while (focusTileX < focusMarkRightX);
      focusMarkTop = textY - 1;
      RichTextCommandStream_DrawSingleLine
                (clipBottom,clipRight,clipTop,clipLeft,textStyle,commandStream,textY,textX);
      /* the focus mark itself, one pixel up and to the left of its shadow */
      g_GraphicsTextureSourceBlitSourceAlpha
                (clipBottom,clipRight,clipTop,clipLeft,focusMarkTop,textX - 3,UI_WINDOW_SUBRESOURCE_FOCUS_MARK_LEFT,
                 g_UiWindowTextureSource,g_FramebufferAccess);
      rightCapSize = g_GraphicsTextureSourceGetLogicalSize(UI_WINDOW_SUBRESOURCE_FOCUS_MARK_RIGHT,
                                                           g_UiWindowTextureSource);
      leftCapSize = g_GraphicsTextureSourceGetLogicalSize(UI_WINDOW_SUBRESOURCE_FOCUS_MARK_LEFT,
                                                          g_UiWindowTextureSource);
      focusTileX = textX - 3 + leftCapSize.logicalWidthPixels;
      middleTileSize = g_GraphicsTextureSourceGetLogicalSize(UI_WINDOW_SUBRESOURCE_FOCUS_MARK_MIDDLE,
                                                             g_UiWindowTextureSource);
      focusMarkRightX = ((textX + 3) - rightCapSize.logicalWidthPixels) + textWidth;
      g_GraphicsTextureSourceBlitSourceAlpha
                (clipBottom,clipRight,clipTop,clipLeft,focusMarkTop,focusMarkRightX,
                 UI_WINDOW_SUBRESOURCE_FOCUS_MARK_RIGHT,g_UiWindowTextureSource,
                 g_FramebufferAccess);
      if (clipRight < focusMarkRightX) {
        focusMarkRightX = clipRight;
      }
      do {
        g_GraphicsTextureSourceBlitSourceAlpha
                  (clipBottom,focusMarkRightX,clipTop,clipLeft,focusMarkTop,focusTileX,
                   UI_WINDOW_SUBRESOURCE_FOCUS_MARK_MIDDLE,g_UiWindowTextureSource,
                   g_FramebufferAccess);
        focusTileX = focusTileX + middleTileSize.logicalWidthPixels;
      } while (focusTileX < focusMarkRightX);
    }
  }
  if (!framebufferUnavailable) {
    g_GraphicsFramebufferEndAccess();
  }
  if (((control->selectable).base.nodeFlags & UI_NODE_SUPPRESSED) == 0) {
    UiContainer_DrawIntersectingChildren
              (clipBottom,clipRight,clipTop,clipLeft,(UiNodeBase *)control);
  }
  return;
}

/* Primary button press on a framed button (nonRightPress slot of g_UiFramedTextButtonControlVtable and
   g_UiWindowControlVtable). A momentary button only shows itself pressed (the action follows on release); a
   persistent toggle button flips its selected state, a persistent radio-style button becomes selected unless
   it already is. Both of those play the activation sound when enabled and queue the action.
*/
void UiFramedTextButtonControl_NonRightPress
          (UiPointerWheelDelta wheelDelta,UiPixelCoordinate pointerY,UiPixelCoordinate pointerX,
          UiFramedTextButtonControl *control)

{
  UiSelectableStateFlags *selectedStateFlagsField;
  UiSelectableStateFlags *stateFlagsField;
  UiSelectableStateFlags *toggleStateFlagsField;
  
  if (((control->selectable).base.nodeFlags & UI_NODE_SUPPRESSED) == 0) {
    if (((control->selectable).stateFlags & UI_SELECTABLE_PERSISTENT_ACTIVATION_MODE) == 0) {
      stateFlagsField = &(control->selectable).stateFlags;
      *stateFlagsField = *stateFlagsField | UI_SELECTABLE_SELECTED_OR_CHECKED;
      UiNode_InvalidateRoot((UiNodeBase *)control);
      return;
    }
    if (((control->selectable).stateFlags & UI_SELECTABLE_TOGGLE_ON_ACTIVATION) != 0) {
      if ((((control->selectable).stateFlags & UI_BUTTON_PLAY_ACTIVATION_SOUND) != 0) &&
          (control->activationSound != NULL)) {
        g_SoundPlayOneShot
                  (g_UiSoundGainQ15,g_UiSoundGainQ15,
                   control->activationSound,NULL);
      }
      toggleStateFlagsField = &(control->selectable).stateFlags;
      *toggleStateFlagsField = *toggleStateFlagsField ^ UI_SELECTABLE_SELECTED_OR_CHECKED;
      UiActionQueue_Enqueue((control->selectable).actionId,control);
      UiNode_InvalidateRoot((UiNodeBase *)control);
      return;
    }
    if (((control->selectable).stateFlags & UI_SELECTABLE_SELECTED_OR_CHECKED) == 0) {
      if ((((control->selectable).stateFlags & UI_BUTTON_PLAY_ACTIVATION_SOUND) != 0) &&
          (control->activationSound != NULL)) {
        g_SoundPlayOneShot
                  (g_UiSoundGainQ15,g_UiSoundGainQ15,
                   control->activationSound,NULL);
      }
      selectedStateFlagsField = &(control->selectable).stateFlags;
      *selectedStateFlagsField = *selectedStateFlagsField | UI_SELECTABLE_SELECTED_OR_CHECKED;
      UiActionQueue_Enqueue((control->selectable).actionId,control);
      UiNode_InvalidateRoot((UiNodeBase *)control);
    }
  }
  return;
}

/* Primary button release on a framed button (nonRightRelease slot of g_UiFramedTextButtonControlVtable and
   g_UiWindowControlVtable): a momentary button that is still pressed (the pointer stayed on it) plays the
   activation sound when enabled, pops back up and queues its action.
*/
void UiFramedTextButtonControl_NonRightRelease
          (UiPointerWheelDelta wheelDelta,UiPixelCoordinate pointerY,UiPixelCoordinate pointerX,
          UiFramedTextButtonControl *control)

{
  UiSelectableStateFlags *stateFlagsField;
  
  if (((((control->selectable).base.nodeFlags & UI_NODE_SUPPRESSED) == 0) &&
      (((control->selectable).stateFlags & UI_SELECTABLE_PERSISTENT_ACTIVATION_MODE) == 0)) &&
     (((control->selectable).stateFlags & UI_SELECTABLE_SELECTED_OR_CHECKED) != 0)) {
    if ((((control->selectable).stateFlags & UI_BUTTON_PLAY_ACTIVATION_SOUND) != 0) &&
        (control->activationSound != NULL)) {
      g_SoundPlayOneShot
                (g_UiSoundGainQ15,g_UiSoundGainQ15,control->activationSound,NULL);
    }
    stateFlagsField = &(control->selectable).stateFlags;
    *stateFlagsField = *stateFlagsField & ~UI_SELECTABLE_SELECTED_OR_CHECKED;
    UiActionQueue_Enqueue((control->selectable).actionId,control);
    UiNode_InvalidateRoot((UiNodeBase *)control);
  }
  return;
}

/* True when the point lies inside the framed button's box (with UI_BUTTON_FRAME_INSET: inside its frame,
   g_UiWindowFrameInset pixels in from every edge). */
static Bool8 UiFramedTextButtonControl_ContainsPoint
          (UiPixelCoordinate pointerY,UiPixelCoordinate pointerX,UiFramedTextButtonControl *control)

{
  int relativeX;
  int relativeY;

  relativeX = pointerX - (control->selectable).base.left;
  relativeY = pointerY - (control->selectable).base.top;
  if ((pointerX < (control->selectable).base.left) || (pointerY < (control->selectable).base.top) ||
      ((control->selectable).base.layoutWidth <= relativeX) ||
      ((control->selectable).base.layoutHeight <= relativeY)) {
    return false;
  }
  if (((control->selectable).stateFlags & UI_BUTTON_FRAME_INSET) == 0) {
    return true;
  }
  return (g_UiWindowFrameInset <= relativeX) && (g_UiWindowFrameInset <= relativeY) &&
         (relativeX + g_UiWindowFrameInset < (control->selectable).base.layoutWidth) &&
         (relativeY + g_UiWindowFrameInset < (control->selectable).base.layoutHeight);
}

/* Primary-button drag with a framed button captured (nonRightDrag slot of g_UiFramedTextButtonControlVtable and
   g_UiWindowControlVtable): a momentary button shows itself pressed while the pointer is inside its box
   (inside the frame for UI_BUTTON_FRAME_INSET) and released while it is outside.
*/
void UiFramedTextButtonControl_NonRightDrag
          (UiPointerWheelDelta wheelDelta,UiPixelCoordinate pointerY,UiPixelCoordinate pointerX,
          UiFramedTextButtonControl *control)

{
  if ((((control->selectable).base.nodeFlags & UI_NODE_SUPPRESSED) == 0) &&
     (((control->selectable).stateFlags & UI_SELECTABLE_PERSISTENT_ACTIVATION_MODE) == 0)) {
    if (!UiFramedTextButtonControl_ContainsPoint(pointerY,pointerX,control)) {
      if (((control->selectable).stateFlags & UI_SELECTABLE_SELECTED_OR_CHECKED) != 0) {
        (control->selectable).stateFlags = (control->selectable).stateFlags & ~UI_SELECTABLE_SELECTED_OR_CHECKED;
        UiNode_InvalidateRoot((UiNodeBase *)control);
      }
    }
    else if (((control->selectable).stateFlags & UI_SELECTABLE_SELECTED_OR_CHECKED) == 0) {
      (control->selectable).stateFlags = (control->selectable).stateFlags | UI_SELECTABLE_SELECTED_OR_CHECKED;
      UiNode_InvalidateRoot((UiNodeBase *)control);
    }
  }
}

/* Hit test of a framed button (hitTest slot of g_UiFramedTextButtonControlVtable and g_UiWindowControlVtable): the
   control itself when the point lies inside its box (for UI_BUTTON_FRAME_INSET: inside the frame), else
   UI_NODE_NONE. Suppressed buttons are never hit; children are not tested.
*/
UiNodeBase * UiFramedTextButtonControl_HitTestRect
          (UiPixelCoordinate pointerY,UiPixelCoordinate pointerX,UiFramedTextButtonControl *control)

{
  if ((((control->selectable).base.nodeFlags & UI_NODE_SUPPRESSED) == 0) &&
     (UiFramedTextButtonControl_ContainsPoint(pointerY,pointerX,control))) {
    return (UiNodeBase *)control;
  }
  return UI_NODE_NONE;
}

/* Relocation of a loaded text button (relocate slot of g_UiTextButtonControlVtable,
   g_UiGraphicsAdapterTextButtonVtable, g_UiNumericPairTextButtonVtable and g_UiPayloadPairTextButtonVtable):
   only the children need relocating; the text resource id and style are plain values.
*/
void UiTextButtonControl_Relocate(UiSerializedRelocationDelta relocationDelta,UiTextButtonControl *control)

{
  UiContainer_RelocateChildren(relocationDelta,(UiNodeBase *)control);
  return;
}

/* Primary button press on a text button (nonRightPress slot of g_UiTextButtonControlVtable,
   g_UiGraphicsAdapterTextButtonVtable, g_UiNumericPairTextButtonVtable and g_UiPayloadPairTextButtonVtable).
   Only opaque pixels of the button graphic count. A checkbox (toggle) flips its checked state and leaves the
   alternate state; a radio-style button becomes selected unless it already is. Either way the activation
   sound plays when enabled and the action is queued.
*/
void UiTextButtonControl_NonRightPress
          (UiPointerWheelDelta wheelDelta,UiPixelCoordinate pointerY,UiPixelCoordinate pointerX,
          UiTextButtonControl *control)

{
  UiSelectableStateFlags *clearedStateFlagsField;
  Bool8 pixelHit;
  UiSelectableStateFlags *toggleStateFlagsField;
  UiSelectableStateFlags *stateFlagsField;
  
  if (((control->selectable).base.nodeFlags & UI_NODE_SUPPRESSED) == 0) {
    if (((control->selectable).stateFlags & UI_SELECTABLE_TOGGLE_ON_ACTIVATION) == 0) {
      pixelHit = g_GraphicsTextureSourceTestOpaquePixel
                        (pointerY,pointerX,(control->selectable).base.top,
                         (control->selectable).base.left,UI_WINDOW_SUBRESOURCE_PUSH_BUTTON,g_UiWindowTextureSource);
      if (pixelHit) {
        if ((((control->selectable).stateFlags & UI_BUTTON_PLAY_ACTIVATION_SOUND) != 0) &&
            (control->activationSound != NULL)) {
          g_SoundPlayOneShot
                    (g_UiSoundGainQ15,g_UiSoundGainQ15,
                     control->activationSound,NULL);
        }
        if (((control->selectable).stateFlags & UI_SELECTABLE_SELECTED_OR_CHECKED) == 0) {
          stateFlagsField = &(control->selectable).stateFlags;
          *stateFlagsField = *stateFlagsField | UI_SELECTABLE_SELECTED_OR_CHECKED;
          UiActionQueue_Enqueue((control->selectable).actionId,control);
          UiNode_InvalidateRoot((UiNodeBase *)control);
          return;
        }
      }
    }
    else {
      pixelHit = g_GraphicsTextureSourceTestOpaquePixel
                        (pointerY,pointerX,(control->selectable).base.top,
                         (control->selectable).base.left,UI_WINDOW_SUBRESOURCE_CHECKBOX,g_UiWindowTextureSource);
      if (pixelHit) {
        if ((((control->selectable).stateFlags & UI_BUTTON_PLAY_ACTIVATION_SOUND) != 0) &&
            (control->activationSound != NULL)) {
          g_SoundPlayOneShot
                    (g_UiSoundGainQ15,g_UiSoundGainQ15,
                     control->activationSound,NULL);
        }
        toggleStateFlagsField = &(control->selectable).stateFlags;
        *toggleStateFlagsField = *toggleStateFlagsField ^ UI_SELECTABLE_SELECTED_OR_CHECKED;
        clearedStateFlagsField = &(control->selectable).stateFlags;
        *clearedStateFlagsField = *clearedStateFlagsField & ~UI_BUTTON_ALTERNATE_STATE;
        UiActionQueue_Enqueue((control->selectable).actionId,control);
        UiNode_InvalidateRoot((UiNodeBase *)control);
      }
    }
  }
  return;
}

/* Keyboard handler of a text button (keyboardEvent slot of g_UiTextButtonControlVtable,
   g_UiGraphicsAdapterTextButtonVtable, g_UiNumericPairTextButtonVtable and g_UiPayloadPairTextButtonVtable):
   Space on the focused button activates it like a pointer press (checkbox toggles, radio-style button gets
   selected) unless UI_SELECTABLE_IGNORE_FOCUSED_SPACE_ACTIVATION is set. Everything else, including Space on
   an already selected radio-style button, goes to UiNode_DefaultKeyboardEventMoveFocusNext. Returns
   false: consumed.
*/
Bool8 UiTextButtonControl_KeyboardEvent(UiKeyboardStateMask keyboardStateMask,UiKeyboardEventCode keyCode,
          UiTextButtonControl *control)

{
  UiSelectableStateFlags *selectedStateFlagsField;
  Bool8 delegatedResult;
  UiSelectableStateFlags *stateFlagsField;
  UiSelectableStateFlags *selectionStateFlagsField;
  
  if ((((((control->selectable).base.nodeFlags & UI_NODE_SUPPRESSED) == 0) && (keyCode == KEYBOARD_KEY_CODE_SPACE)) &&
      (control == (UiTextButtonControl *)g_UiKeyboardFocusNode)) &&
     (((control->selectable).stateFlags & UI_SELECTABLE_IGNORE_FOCUSED_SPACE_ACTIVATION) == 0)) {
    if (((control->selectable).stateFlags & UI_SELECTABLE_TOGGLE_ON_ACTIVATION) != 0) {
      if ((((control->selectable).stateFlags & UI_BUTTON_PLAY_ACTIVATION_SOUND) != 0) &&
          (control->activationSound != NULL)) {
        g_SoundPlayOneShot
                  (g_UiSoundGainQ15,g_UiSoundGainQ15,
                   control->activationSound,NULL);
      }
      stateFlagsField = &(control->selectable).stateFlags;
      *stateFlagsField = *stateFlagsField ^ UI_SELECTABLE_SELECTED_OR_CHECKED;
      selectionStateFlagsField = &(control->selectable).stateFlags;
      *selectionStateFlagsField = *selectionStateFlagsField & ~UI_BUTTON_ALTERNATE_STATE;
      UiActionQueue_Enqueue((control->selectable).actionId,control);
      UiNode_InvalidateRoot((UiNodeBase *)control);
      return false;
    }
    if (((control->selectable).stateFlags & UI_SELECTABLE_SELECTED_OR_CHECKED) == 0) {
      if ((((control->selectable).stateFlags & UI_BUTTON_PLAY_ACTIVATION_SOUND) != 0) &&
          (control->activationSound != NULL)) {
        g_SoundPlayOneShot
                  (g_UiSoundGainQ15,g_UiSoundGainQ15,
                   control->activationSound,NULL);
      }
      selectedStateFlagsField = &(control->selectable).stateFlags;
      *selectedStateFlagsField = *selectedStateFlagsField | UI_SELECTABLE_SELECTED_OR_CHECKED;
      UiActionQueue_Enqueue((control->selectable).actionId,control);
      UiNode_InvalidateRoot((UiNodeBase *)control);
      return false;
    }
  }
  delegatedResult = UiNode_DefaultKeyboardEventMoveFocusNext
                    (keyboardStateMask,keyCode,(UiNodeBase *)control);
  return delegatedResult;
}

/* Draws a text button (drawClipped slot of g_UiTextButtonControlVtable; also called by the adapter, numeric-pair
   and payload-pair buttons after patching their text): the push-button or checkbox graphic for its state
   (pressed/checked, alternate, disabled), then its text 6 pixels right of the graphic, vertically centred,
   with the focus mark and its shadow while it has keyboard focus. Children are not drawn.
*/
void UiTextButtonControl_DrawClipped(UiPixelCoordinate clipBottom,UiPixelCoordinate clipRight,UiPixelCoordinate clipTop,
          UiPixelCoordinate clipLeft,UiTextButtonControl *control)

{
  uint16_t *commandStream;
  uint32_t buttonFrame;
  int focusMarkTop;
  int focusTileX;
  int focusMarkRightX;
  uint32_t textStyle;
  RichTextExtent textExtent;
  GraphicsTextureLogicalSize buttonSize;
  GraphicsTextureLogicalSize rightCapSize;
  GraphicsTextureLogicalSize leftCapSize;
  GraphicsTextureLogicalSize middleTileSize;
  int textX;
  int textY;

  if (g_GraphicsFramebufferBeginAccess()) {
    return;
  }
  buttonFrame = UI_WINDOW_SUBRESOURCE_PUSH_BUTTON;
  if (((control->selectable).stateFlags & UI_SELECTABLE_SELECTED_OR_CHECKED) != 0) {
    buttonFrame = UI_WINDOW_SUBRESOURCE_PUSH_BUTTON + 2;
  }
  if (((control->selectable).stateFlags & UI_SELECTABLE_TOGGLE_ON_ACTIVATION) != 0) {
    buttonFrame = buttonFrame - (UI_WINDOW_SUBRESOURCE_PUSH_BUTTON - UI_WINDOW_SUBRESOURCE_CHECKBOX);
    if (((control->selectable).stateFlags & UI_BUTTON_ALTERNATE_STATE) != 0) {
      buttonFrame = UI_WINDOW_SUBRESOURCE_CHECKBOX + 4;
    }
  }
  if (((control->selectable).base.nodeFlags & UI_NODE_SUPPRESSED) != 0) {
    if (((control->selectable).stateFlags & UI_BUTTON_HIDDEN_WHILE_SUPPRESSED) != 0) {
      g_GraphicsFramebufferEndAccess();
      return;
    }
    buttonFrame++;
  }
  g_GraphicsTextureSourceBlitSourceAlpha
            (clipBottom,clipRight,clipTop,clipLeft,(control->selectable).base.top,
             (control->selectable).base.left,buttonFrame,g_UiWindowTextureSource,g_FramebufferAccess);
  buttonSize = g_GraphicsTextureSourceGetLogicalSize(buttonFrame,g_UiWindowTextureSource);
  /* text offsets inside the control: 6 pixels right of the graphic, vertically centred on it */
  textX = buttonSize.logicalWidthPixels + 6;
  commandStream = TextResource_Resolve(control->textResourceId);
  textExtent = RichTextCommandStream_MeasureLine(control->packedTextStyle,commandStream);
  textY = (int)(buttonSize.logicalHeightPixels - textExtent.heightPixels) >> 1;
  textStyle = g_UiTextStyleDisabled;
  if (((control->selectable).base.nodeFlags & UI_NODE_SUPPRESSED) == 0) {
    textStyle = g_UiTextStyleNormal;
    if (((control->selectable).stateFlags & UI_BUTTON_ALTERNATE_STATE) != 0) {
      textStyle = g_UiTextStyleAlternate;
    }
  }
  /* packedTextStyle may override the font byte (bits 24-31) and the palette byte (bits 16-23) */
  if (((control->selectable).stateFlags & UI_BUTTON_OWN_STYLE_FONT) == 0) {
    control->packedTextStyle = control->packedTextStyle & UI_TEXT_STYLE_PALETTE_BYTE;
  }
  else {
    textStyle = textStyle & ~UI_TEXT_STYLE_FONT_BYTE;
  }
  if (((control->selectable).stateFlags & UI_BUTTON_OWN_STYLE_PALETTE) == 0) {
    control->packedTextStyle = control->packedTextStyle & UI_TEXT_STYLE_FONT_BYTE;
  }
  else {
    textStyle = textStyle & ~UI_TEXT_STYLE_PALETTE_BYTE;
  }
  textStyle = textStyle | control->packedTextStyle;
  if ((((control->selectable).base.nodeFlags & UI_NODE_HAS_KEYBOARD_FOCUS) == 0) ||
     (((control->selectable).stateFlags & UI_BUTTON_NO_FOCUS_MARK) != 0)) {
    RichTextCommandStream_DrawSingleLine
              (clipBottom,clipRight,clipTop,clipLeft,textStyle,commandStream,
               textY + (control->selectable).base.top,textX + (control->selectable).base.left);
  }
  else {
    textX = textX + (control->selectable).base.left;
    textY = textY + (control->selectable).base.top;
    /* focus mark shadow: left cap, right cap, then the middle tiles up to the right cap */
    g_GraphicsTextureSourceBlitModulatedSourceAlpha
              (clipBottom,clipRight,clipTop,clipLeft,textY,textX - 2,TEXT_SHADOW_COLOR_ARGB,
               UI_WINDOW_SUBRESOURCE_FOCUS_MARK_LEFT,
               g_UiWindowTextureSource,g_FramebufferAccess);
    rightCapSize = g_GraphicsTextureSourceGetLogicalSize(UI_WINDOW_SUBRESOURCE_FOCUS_MARK_RIGHT,
                                                         g_UiWindowTextureSource);
    leftCapSize = g_GraphicsTextureSourceGetLogicalSize(UI_WINDOW_SUBRESOURCE_FOCUS_MARK_LEFT,
                                                        g_UiWindowTextureSource);
    focusTileX = textX - 2 + leftCapSize.logicalWidthPixels;
    middleTileSize = g_GraphicsTextureSourceGetLogicalSize(UI_WINDOW_SUBRESOURCE_FOCUS_MARK_MIDDLE,
                                                           g_UiWindowTextureSource);
    focusMarkRightX = ((textX + 4) - rightCapSize.logicalWidthPixels) + textExtent.widthPixels;
    g_GraphicsTextureSourceBlitModulatedSourceAlpha
              (clipBottom,clipRight,clipTop,clipLeft,textY,focusMarkRightX,TEXT_SHADOW_COLOR_ARGB,
               UI_WINDOW_SUBRESOURCE_FOCUS_MARK_RIGHT,
               g_UiWindowTextureSource,g_FramebufferAccess);
    if (clipRight < focusMarkRightX) {
      focusMarkRightX = clipRight;
    }
    do {
      g_GraphicsTextureSourceBlitModulatedSourceAlpha
                (clipBottom,focusMarkRightX,clipTop,clipLeft,textY,focusTileX,TEXT_SHADOW_COLOR_ARGB,
                 UI_WINDOW_SUBRESOURCE_FOCUS_MARK_MIDDLE,
                 g_UiWindowTextureSource,g_FramebufferAccess);
      focusTileX = focusTileX + middleTileSize.logicalWidthPixels;
    } while (focusTileX < focusMarkRightX);
    focusMarkTop = textY - 1;
    RichTextCommandStream_DrawSingleLine
              (clipBottom,clipRight,clipTop,clipLeft,textStyle,commandStream,textY,textX);
    /* the focus mark itself, one pixel up and to the left of its shadow */
    g_GraphicsTextureSourceBlitSourceAlpha
              (clipBottom,clipRight,clipTop,clipLeft,focusMarkTop,textX - 3,UI_WINDOW_SUBRESOURCE_FOCUS_MARK_LEFT,
               g_UiWindowTextureSource,g_FramebufferAccess);
    rightCapSize = g_GraphicsTextureSourceGetLogicalSize(UI_WINDOW_SUBRESOURCE_FOCUS_MARK_RIGHT,
                                                         g_UiWindowTextureSource);
    leftCapSize = g_GraphicsTextureSourceGetLogicalSize(UI_WINDOW_SUBRESOURCE_FOCUS_MARK_LEFT,
                                                        g_UiWindowTextureSource);
    focusTileX = textX - 3 + leftCapSize.logicalWidthPixels;
    middleTileSize = g_GraphicsTextureSourceGetLogicalSize(UI_WINDOW_SUBRESOURCE_FOCUS_MARK_MIDDLE,
                                                           g_UiWindowTextureSource);
    focusMarkRightX = ((textX + 3) - rightCapSize.logicalWidthPixels) + textExtent.widthPixels;
    g_GraphicsTextureSourceBlitSourceAlpha
              (clipBottom,clipRight,clipTop,clipLeft,focusMarkTop,focusMarkRightX,
               UI_WINDOW_SUBRESOURCE_FOCUS_MARK_RIGHT,g_UiWindowTextureSource,
               g_FramebufferAccess);
    if (clipRight < focusMarkRightX) {
      focusMarkRightX = clipRight;
    }
    do {
      g_GraphicsTextureSourceBlitSourceAlpha
                (clipBottom,focusMarkRightX,clipTop,clipLeft,focusMarkTop,focusTileX,
                 UI_WINDOW_SUBRESOURCE_FOCUS_MARK_MIDDLE,g_UiWindowTextureSource,
                 g_FramebufferAccess);
      focusTileX = focusTileX + middleTileSize.logicalWidthPixels;
    } while (focusTileX < focusMarkRightX);
  }
  g_GraphicsFramebufferEndAccess();
}

UiNodeVtable g_UiGraphicsAdapterTextButtonVtable = {
        .relocate = THANDOR_FN(UiTextButtonControl_Relocate),
        .method04 = THANDOR_FN(UiNode_DefaultMethod04_NoOp),
        .drawClipped = THANDOR_FN(UiGraphicsAdapterTextButton_DrawFormattedAdapterText),
        .layout = THANDOR_FN(UiContainer_LayoutChildren),
        .nonRightPress = THANDOR_FN(UiTextButtonControl_NonRightPress),
        .nonRightRelease = THANDOR_FN(UiNode_DefaultNonRightRelease),
        .rightPress = THANDOR_FN(UiNode_ForwardRightPressToParent),
        .rightRelease = THANDOR_FN(UiNode_DefaultRightRelease),
        .nonRightDrag = THANDOR_FN(UiNode_DefaultNonRightDrag),
        .rightDrag = THANDOR_FN(UiNode_DefaultRightDrag),
        .pointerMove = THANDOR_FN(UiNode_DefaultPointerMove),
        .hitTest = THANDOR_FN(UiContainer_HitTestChildren),
        .keyboardEvent = THANDOR_FN(UiTextButtonControl_KeyboardEvent),
        .applyFlags = THANDOR_FN(UiNode_ApplyFlagsRecursive),
        .suppressActionId = THANDOR_FN(UiSelectableControl_SuppressIfActionId),
        .unsuppressActionId = THANDOR_FN(UiSelectableControl_UnsuppressIfActionId),
        .tick = THANDOR_FN(UiNode_DefaultTick),
        .pointerWheel = THANDOR_FN(UiNode_ForwardPointerWheelToParent),
};

UiNodeVtable g_UiFramedTextButtonControlVtable = {
        .relocate = THANDOR_FN(UiFramedTextButtonControl_Relocate),
        .method04 = THANDOR_FN(UiNode_DefaultMethod04_NoOp),
        .drawClipped = THANDOR_FN(UiFramedTextButtonControl_DrawClipped),
        .layout = THANDOR_FN(UiContainer_LayoutChildren),
        .nonRightPress = THANDOR_FN(UiFramedTextButtonControl_NonRightPress),
        .nonRightRelease = THANDOR_FN(UiFramedTextButtonControl_NonRightRelease),
        .rightPress = THANDOR_FN(UiNode_ForwardRightPressToParent),
        .rightRelease = THANDOR_FN(UiNode_DefaultRightRelease),
        .nonRightDrag = THANDOR_FN(UiFramedTextButtonControl_NonRightDrag),
        .rightDrag = THANDOR_FN(UiNode_DefaultRightDrag),
        .pointerMove = THANDOR_FN(UiNode_DefaultPointerMove),
        .hitTest = THANDOR_FN(UiFramedTextButtonControl_HitTestRect),
        .keyboardEvent = THANDOR_FN(UiSelectableControl_KeyboardEvent),
        .applyFlags = THANDOR_FN(UiNode_ApplyFlagsRecursive),
        .suppressActionId = THANDOR_FN(UiSelectableControl_SuppressIfActionId),
        .unsuppressActionId = THANDOR_FN(UiSelectableControl_UnsuppressIfActionId),
        .tick = THANDOR_FN(UiNode_DefaultTick),
        .pointerWheel = THANDOR_FN(UiNode_ForwardPointerWheelToParent)};

UiNodeVtable g_UiTextButtonControlVtable = {
        .relocate = THANDOR_FN(UiTextButtonControl_Relocate),
        .method04 = THANDOR_FN(UiNode_DefaultMethod04_NoOp),
        .drawClipped = THANDOR_FN(UiTextButtonControl_DrawClipped),
        .layout = THANDOR_FN(UiContainer_LayoutChildren),
        .nonRightPress = THANDOR_FN(UiTextButtonControl_NonRightPress),
        .nonRightRelease = THANDOR_FN(UiNode_DefaultNonRightRelease),
        .rightPress = THANDOR_FN(UiNode_ForwardRightPressToParent),
        .rightRelease = THANDOR_FN(UiNode_DefaultRightRelease),
        .nonRightDrag = THANDOR_FN(UiNode_DefaultNonRightDrag),
        .rightDrag = THANDOR_FN(UiNode_DefaultRightDrag),
        .pointerMove = THANDOR_FN(UiNode_DefaultPointerMove),
        .hitTest = THANDOR_FN(UiContainer_HitTestChildren),
        .keyboardEvent = THANDOR_FN(UiTextButtonControl_KeyboardEvent),
        .applyFlags = THANDOR_FN(UiNode_ApplyFlagsRecursive),
        .suppressActionId = THANDOR_FN(UiSelectableControl_SuppressIfActionId),
        .unsuppressActionId = THANDOR_FN(UiSelectableControl_UnsuppressIfActionId),
        .tick = THANDOR_FN(UiNode_DefaultTick),
        .pointerWheel = THANDOR_FN(UiNode_ForwardPointerWheelToParent)};

UiNodeVtable g_UiNumericPairTextButtonVtable = {
        .relocate = THANDOR_FN(UiTextButtonControl_Relocate),
        .method04 = THANDOR_FN(UiNode_DefaultMethod04_NoOp),
        .drawClipped = THANDOR_FN(UiNumericPairTextButton_DrawFormattedValues),
        .layout = THANDOR_FN(UiContainer_LayoutChildren),
        .nonRightPress = THANDOR_FN(UiTextButtonControl_NonRightPress),
        .nonRightRelease = THANDOR_FN(UiNode_DefaultNonRightRelease),
        .rightPress = THANDOR_FN(UiNode_ForwardRightPressToParent),
        .rightRelease = THANDOR_FN(UiNode_DefaultRightRelease),
        .nonRightDrag = THANDOR_FN(UiNode_DefaultNonRightDrag),
        .rightDrag = THANDOR_FN(UiNode_DefaultRightDrag),
        .pointerMove = THANDOR_FN(UiNode_DefaultPointerMove),
        .hitTest = THANDOR_FN(UiContainer_HitTestChildren),
        .keyboardEvent = THANDOR_FN(UiTextButtonControl_KeyboardEvent),
        .applyFlags = THANDOR_FN(UiNode_ApplyFlagsRecursive),
        .suppressActionId = THANDOR_FN(UiSelectableControl_SuppressIfActionId),
        .unsuppressActionId = THANDOR_FN(UiSelectableControl_UnsuppressIfActionId),
        .tick = THANDOR_FN(UiNode_DefaultTick),
        .pointerWheel = THANDOR_FN(UiNode_ForwardPointerWheelToParent),
};

UiNodeVtable g_UiPayloadPairTextButtonVtable = {
        .relocate = THANDOR_FN(UiTextButtonControl_Relocate),
        .method04 = THANDOR_FN(UiNode_DefaultMethod04_NoOp),
        .drawClipped = THANDOR_FN(UiPayloadPairTextButton_DrawFormattedPayloads),
        .layout = THANDOR_FN(UiContainer_LayoutChildren),
        .nonRightPress = THANDOR_FN(UiTextButtonControl_NonRightPress),
        .nonRightRelease = THANDOR_FN(UiNode_DefaultNonRightRelease),
        .rightPress = THANDOR_FN(UiNode_ForwardRightPressToParent),
        .rightRelease = THANDOR_FN(UiNode_DefaultRightRelease),
        .nonRightDrag = THANDOR_FN(UiNode_DefaultNonRightDrag),
        .rightDrag = THANDOR_FN(UiNode_DefaultRightDrag),
        .pointerMove = THANDOR_FN(UiNode_DefaultPointerMove),
        .hitTest = THANDOR_FN(UiContainer_HitTestChildren),
        .keyboardEvent = THANDOR_FN(UiTextButtonControl_KeyboardEvent),
        .applyFlags = THANDOR_FN(UiNode_ApplyFlagsRecursive),
        .suppressActionId = THANDOR_FN(UiSelectableControl_SuppressIfActionId),
        .unsuppressActionId = THANDOR_FN(UiSelectableControl_UnsuppressIfActionId),
        .tick = THANDOR_FN(UiNode_DefaultTick),
        .pointerWheel = THANDOR_FN(UiNode_ForwardPointerWheelToParent),
};

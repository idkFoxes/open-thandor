/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/src/ui/controls/window.cpp
 * Reverse engineering by idkFoxes 2026
 */

#include <thandor/ui/controls/window.h>
#include <thandor/thandor.h>

/* Implementation ownership: ui/controls/window. */

/* Draws a framed icon-and-text button (drawClipped slot of g_UiWindowControlVtable): the normal, selected or
   disabled win.gfx frame, the text centred in the right three quarters (with the focus mark while focused),
   and the icon, vertically centred and ending at the quarter line, over its shadow copy shifted by the normal
   or selected iconDrawOffsets (no shift while suppressed). Children are not drawn.
*/
void UiWindowControl_DrawFramedTextAndChrome
          (UiPixelCoordinate clipBottom,UiPixelCoordinate clipRight,UiPixelCoordinate clipTop,
          UiPixelCoordinate clipLeft,UiWindowControl *control)

{
  char iconOffsetX;
  char iconOffsetY;
  uint32_t cornerWidth;
  uint32_t cornerHeight;
  uint16_t *commandStream;
  uint32_t framePiece;
  uint32_t textWidth;
  int rightCornerX;
  int bottomCornerY;
  int buttonWidth;
  int textX;
  int textY;
  int styleVerticalOffset;
  int shadowMarkLeftX;
  int shadowMarkTop;
  int focusMarkTop;
  int focusTileX;
  int focusMarkRightX;
  int iconX;
  int iconY;
  int iconShadowX;
  int iconShadowY;
  uint32_t textStyle;
  RichTextExtent textExtent;
  GraphicsTextureLogicalSize cornerTileSize;
  GraphicsTextureLogicalSize rightCapSize;
  GraphicsTextureLogicalSize leftCapSize;
  GraphicsTextureLogicalSize middleTileSize;
  GraphicsTextureLogicalSize iconSize;

  if (g_GraphicsFramebufferBeginAccess()) {
    return;
  }
  if (((control->selectable).base.nodeFlags & UI_NODE_SUPPRESSED) == 0) {
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
    if (((control->selectable).stateFlags & UI_BUTTON_HIDDEN_WHILE_SUPPRESSED) != 0) {
      g_GraphicsFramebufferEndAccess();
      return;
    }
    if (((control->selectable).stateFlags & UI_BUTTON_FRAME_INSET) == 0) {
      framePiece = UI_WINDOW_SUBRESOURCE_BUTTON_FRAME_DISABLED;
    }
    else {
      framePiece = UI_WINDOW_SUBRESOURCE_INSET_BUTTON_FRAME_DISABLED;
    }
  }
  cornerTileSize = g_GraphicsTextureSourceGetLogicalSize(framePiece,g_UiWindowTextureSource);
  cornerHeight = cornerTileSize.logicalHeightPixels;
  cornerWidth = cornerTileSize.logicalWidthPixels;
  rightCornerX = (control->selectable).base.layoutWidth - cornerWidth;
  bottomCornerY = (control->selectable).base.layoutHeight - cornerHeight;
  g_GraphicsTextureSourceBlitSourceAlpha
            (clipBottom,clipRight,clipTop,clipLeft,(control->selectable).base.top,(control->selectable).base.left,
             framePiece,
             g_UiWindowTextureSource,g_FramebufferAccess);
  g_GraphicsTextureSourceBlitSourceAlpha
            (clipBottom,clipRight,clipTop,clipLeft,(control->selectable).base.top,
             rightCornerX + (control->selectable).base.left,framePiece + UI_WINDOW_FRAME_TOP_RIGHT,
             g_UiWindowTextureSource,g_FramebufferAccess);
  g_GraphicsTextureSourceBlitSourceAlpha
            (clipBottom,clipRight,clipTop,clipLeft,bottomCornerY + (control->selectable).base.top,
             (control->selectable).base.left,framePiece + UI_WINDOW_FRAME_BOTTOM_LEFT,
             g_UiWindowTextureSource,g_FramebufferAccess);
  g_GraphicsTextureSourceBlitSourceAlpha
            (clipBottom,clipRight,clipTop,clipLeft,bottomCornerY + (control->selectable).base.top,
             rightCornerX + (control->selectable).base.left,
             framePiece + UI_WINDOW_FRAME_BOTTOM_RIGHT,g_UiWindowTextureSource,g_FramebufferAccess);
  UiWindow_BlitTiledHorizontalEdge
            (clipBottom,clipRight,clipTop,clipLeft,framePiece + UI_WINDOW_FRAME_TOP,rightCornerX,0,
             cornerWidth,control);
  UiWindow_BlitTiledVerticalEdge
            (clipBottom,clipRight,clipTop,clipLeft,framePiece + UI_WINDOW_FRAME_LEFT,bottomCornerY,
             cornerHeight,0,control);
  UiWindow_BlitTiledVerticalEdge
            (clipBottom,clipRight,clipTop,clipLeft,framePiece + UI_WINDOW_FRAME_RIGHT,bottomCornerY,
             cornerHeight,rightCornerX,control);
  UiWindow_BlitTiledHorizontalEdge
            (clipBottom,clipRight,clipTop,clipLeft,framePiece + UI_WINDOW_FRAME_BOTTOM,rightCornerX,
             bottomCornerY,cornerWidth,control);
  commandStream = TextResource_Resolve(control->textResourceId);
  buttonWidth = (control->selectable).base.layoutWidth;
  textExtent = RichTextCommandStream_MeasureLine(control->packedTextStyle,commandStream);
  textWidth = textExtent.widthPixels;
  /* offsets inside the button: centred vertically and in the right three quarters */
  textY = (int)((control->selectable).base.layoutHeight - textExtent.heightPixels) >> 1;
  textX = ((int)(((uint32_t)(buttonWidth * 3) >> 2) - textWidth) >> 1) +
          ((uint32_t)(control->selectable).base.layoutWidth >> 2);
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
  if (((control->selectable).base.nodeFlags & UI_NODE_HAS_KEYBOARD_FOCUS) == 0) {
    RichTextCommandStream_DrawSingleLine
              (clipBottom,clipRight,clipTop,clipLeft,textStyle,commandStream,
               textY + (control->selectable).base.top,
               textX + (control->selectable).base.left);
  }
  else {
    textX = textX + (control->selectable).base.left;
    textY = textY + (control->selectable).base.top;
    /* focus mark shadow, shifted by the style's vertical offset: left cap, right cap, then the middle tiles */
    styleVerticalOffset = (int)(textStyle << 16) >> 24; /* signed byte 1 of the packed style */
    shadowMarkLeftX = styleVerticalOffset - 3 + textX;
    shadowMarkTop = styleVerticalOffset - 1 + textY;
    g_GraphicsTextureSourceBlitModulatedSourceAlpha
              (clipBottom,clipRight,clipTop,clipLeft,shadowMarkTop,shadowMarkLeftX,TEXT_SHADOW_COLOR_ARGB,
               UI_WINDOW_SUBRESOURCE_FOCUS_MARK_LEFT,
               g_UiWindowTextureSource,g_FramebufferAccess);
    rightCapSize = g_GraphicsTextureSourceGetLogicalSize(UI_WINDOW_SUBRESOURCE_FOCUS_MARK_RIGHT,
                                                         g_UiWindowTextureSource);
    leftCapSize = g_GraphicsTextureSourceGetLogicalSize(UI_WINDOW_SUBRESOURCE_FOCUS_MARK_LEFT,
                                                        g_UiWindowTextureSource);
    focusTileX = shadowMarkLeftX + leftCapSize.logicalWidthPixels;
    middleTileSize = g_GraphicsTextureSourceGetLogicalSize(UI_WINDOW_SUBRESOURCE_FOCUS_MARK_MIDDLE,
                                                           g_UiWindowTextureSource);
    focusMarkRightX = ((shadowMarkLeftX + 6) - rightCapSize.logicalWidthPixels) + textWidth;
    g_GraphicsTextureSourceBlitModulatedSourceAlpha
              (clipBottom,clipRight,clipTop,clipLeft,shadowMarkTop,focusMarkRightX,TEXT_SHADOW_COLOR_ARGB,
               UI_WINDOW_SUBRESOURCE_FOCUS_MARK_RIGHT,
               g_UiWindowTextureSource,g_FramebufferAccess);
    if (clipRight < focusMarkRightX) {
      focusMarkRightX = clipRight;
    }
    do {
      g_GraphicsTextureSourceBlitModulatedSourceAlpha
                (clipBottom,focusMarkRightX,clipTop,clipLeft,shadowMarkTop,focusTileX,TEXT_SHADOW_COLOR_ARGB,
                 UI_WINDOW_SUBRESOURCE_FOCUS_MARK_MIDDLE,
                 g_UiWindowTextureSource,g_FramebufferAccess);
      focusTileX = focusTileX + middleTileSize.logicalWidthPixels;
    } while (focusTileX < focusMarkRightX);
    focusMarkTop = textY - 1;
    RichTextCommandStream_DrawSingleLine
              (clipBottom,clipRight,clipTop,clipLeft,textStyle,commandStream,textY,textX);
    /* the focus mark itself */
    g_GraphicsTextureSourceBlitSourceAlpha
              (clipBottom,clipRight,clipTop,clipLeft,focusMarkTop,textX - 3,
               UI_WINDOW_SUBRESOURCE_FOCUS_MARK_LEFT,g_UiWindowTextureSource,
               g_FramebufferAccess);
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
  iconSize = g_GraphicsTextureSourceGetLogicalSize(control->iconSubresource,control->iconTextureSource);
  iconX = (((uint32_t)(control->selectable).base.layoutWidth >> 2) - iconSize.logicalWidthPixels) +
          (control->selectable).base.left;
  iconY = ((int)((control->selectable).base.layoutHeight - iconSize.logicalHeightPixels) >> 1) +
          (control->selectable).base.top;
  iconShadowX = iconX;
  iconShadowY = iconY;
  if (((control->selectable).base.nodeFlags & UI_NODE_SUPPRESSED) == 0) {
    if (((control->selectable).stateFlags & UI_SELECTABLE_SELECTED_OR_CHECKED) == 0) {
      iconOffsetX = control->iconDrawOffsets.normalX;
      iconOffsetY = control->iconDrawOffsets.normalY;
    }
    else {
      iconOffsetX = control->iconDrawOffsets.selectedX;
      iconOffsetY = control->iconDrawOffsets.selectedY;
    }
    iconShadowX = iconX + iconOffsetX;
    iconShadowY = iconY + iconOffsetY;
  }
  g_GraphicsTextureSourceBlitModulatedSourceAlpha
            (clipBottom,clipRight,clipTop,clipLeft,iconShadowY,iconShadowX,TEXT_SHADOW_COLOR_ARGB,
             control->iconSubresource,
             control->iconTextureSource,g_FramebufferAccess);
  g_GraphicsTextureSourceBlitSourceAlpha
            (clipBottom,clipRight,clipTop,clipLeft,iconY,iconX,control->iconSubresource,
             control->iconTextureSource,g_FramebufferAccess);
  g_GraphicsFramebufferEndAccess();
}

UiNodeVtable g_UiWindowControlVtable = {
        .relocate = THANDOR_FN(UiWindowControl_RelocateWithFrameInset),
        .method04 = THANDOR_FN(UiNode_DefaultMethod04_NoOp),
        .drawClipped = THANDOR_FN(UiWindowControl_DrawFramedTextAndChrome),
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

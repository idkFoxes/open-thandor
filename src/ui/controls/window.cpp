/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/src/ui/controls/window.cpp
 * Reverse engineering by idkFoxes 2026
 */

#include <thandor/ui/controls/window.h>
#include <thandor/thandor.h>

/* Module data. */

GraphicsTextureSourceAsset *g_UiWindowTextureSource = (GraphicsTextureSourceAsset *)(intptr_t)-1; /* 0xFFFFFFFF in the original */

GraphicsTextureSourceAsset *g_UiWindowClassTextureSource = nullptr;

/* int32_t, 5: pixels from the window top to the title text line of a resizable window */
static const int32_t g_UiResizableWindowTitleTextTopOffset = 5;

/* UiPackedTextStyle, 2: packed rich-text style of the resizable window title */
static const UiPackedTextStyle g_UiResizableWindowTitleTextStyle = 2;

/* int32_t, 19 (0x13): height in pixels of the top strip that drags a movable root window */
static const int32_t g_UiWindowMoveHandleWidth = 19;

static const int32_t g_UiWindowResizeBorderThickness = 19;

static const uint32_t g_UiWindowTitleTextStyle = 0;

static uint16_t g_UiWindowClassTexturePathUtf16[20] = {'e', 'n', 'g', 'i', 'n', 'e', '\\', 'w', 'i', 'n', 'c', 'l', 'a', 's', 's', '.', 'g', 'f', 'x', 0}; /* L"engine\\winclass.gfx" */

static uint16_t g_UiWindowClassTextPathUtf16[19] = {'t', 'e', 'x', 't', 'e', '\\', 'w', 'i', 'n', 'c', 'l', 'a', 's', 's', '.', 's', 't', 'r', 0}; /* L"texte\\winclass.str" */

static uint16_t g_UiWindowTexturePathUtf16[15] = {'e', 'n', 'g', 'i', 'n', 'e', '\\', 'w', 'i', 'n', '.', 'g', 'f', 'x', 0}; /* L"engine\\win.gfx" */

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
        .relocate = UI_SLOT(UiWindowControl_RelocateWithFrameInset),
        .method04 = UI_SLOT(UiNode_DefaultMethod04_NoOp),
        .drawClipped = UI_SLOT(UiWindowControl_DrawFramedTextAndChrome),
        .layout = UI_SLOT(UiContainer_LayoutChildren),
        .nonRightPress = UI_SLOT(UiFramedTextButtonControl_NonRightPress),
        .nonRightRelease = UI_SLOT(UiFramedTextButtonControl_NonRightRelease),
        .rightPress = UI_SLOT(UiNode_ForwardRightPressToParent),
        .rightRelease = UI_SLOT(UiNode_DefaultRightRelease),
        .nonRightDrag = UI_SLOT(UiFramedTextButtonControl_NonRightDrag),
        .rightDrag = UI_SLOT(UiNode_DefaultRightDrag),
        .pointerMove = UI_SLOT(UiNode_DefaultPointerMove),
        .hitTest = UI_SLOT(UiFramedTextButtonControl_HitTestRect),
        .keyboardEvent = UI_SLOT(UiSelectableControl_KeyboardEvent),
        .applyFlags = UI_SLOT(UiNode_ApplyFlagsRecursive),
        .suppressActionId = UI_SLOT(UiSelectableControl_SuppressIfActionId),
        .unsuppressActionId = UI_SLOT(UiSelectableControl_UnsuppressIfActionId),
        .tick = UI_SLOT(UiNode_DefaultTick),
        .pointerWheel = UI_SLOT(UiNode_ForwardPointerWheelToParent)};

/* drawClipped of g_UiResizableWindowControlVtable: draws the window chrome selected by rootFlags (tiled
   background, frame, title bar with the centred title text, close button top left, maximize/restore button
   top right) and then the children. Title bar and buttons use their inactive pieces while the window is not
   in the front root, the buttons their armed pieces while pressed under the pointer.
*/
void UiResizableWindowControl_DrawFrameTitleAndChildren
          (UiPixelCoordinate clipBottom,UiPixelCoordinate clipRight,UiPixelCoordinate clipTop,
          UiPixelCoordinate clipLeft,UiResizableWindowControl *control)

{
  uint32_t cornerWidth;
  uint32_t cornerHeight;
  GraphicsSubresourceIndex titleBarSubresource;
  GraphicsSubresourceIndex buttonSubresource;
  int bottomEdgeY;
  int rightEdgeX;
  int rightCapX;
  Bool8 beginAccessFailed;
  uint16_t *titleText;
  GraphicsTextureLogicalSize textureSize;
  GraphicsTextureLogicalSize rightCapSize;

  beginAccessFailed = g_GraphicsFramebufferBeginAccess();
  if (!beginAccessFailed) {
    if ((control->root.rootFlags & UI_ROOT_TILED_BACKGROUND) != 0) {
      UiWindow_BlitTiledInterior
                (clipBottom,clipRight,clipTop,clipLeft,UI_WINDOW_SUBRESOURCE_WINDOW_INTERIOR,
                 control->root.base.layoutHeight,control->root.base.layoutWidth,0,0,control);
    }
    if ((control->root.rootFlags & UI_ROOT_FRAME) != 0) {
      textureSize = g_GraphicsTextureSourceGetLogicalSize
                              (UI_WINDOW_SUBRESOURCE_WINDOW_FRAME + UI_WINDOW_FRAME_BOTTOM_RIGHT,
                               g_UiWindowTextureSource);
      cornerHeight = textureSize.logicalHeightPixels;
      cornerWidth = textureSize.logicalWidthPixels;
      rightEdgeX = control->root.base.layoutWidth - cornerWidth;
      bottomEdgeY = control->root.base.layoutHeight - cornerHeight; /* bottom corner row */
      g_GraphicsTextureSourceBlitSourceAlpha
                (clipBottom,clipRight,clipTop,clipLeft,control->root.base.top,control->root.base.left,
                 UI_WINDOW_SUBRESOURCE_WINDOW_FRAME + UI_WINDOW_FRAME_TOP_LEFT,
                 g_UiWindowTextureSource,g_FramebufferAccess);
      g_GraphicsTextureSourceBlitSourceAlpha
                (clipBottom,clipRight,clipTop,clipLeft,control->root.base.top,rightEdgeX + control->root.base.left,
                 UI_WINDOW_SUBRESOURCE_WINDOW_FRAME + UI_WINDOW_FRAME_TOP_RIGHT,
                 g_UiWindowTextureSource,g_FramebufferAccess);
      g_GraphicsTextureSourceBlitSourceAlpha
                (clipBottom,clipRight,clipTop,clipLeft,bottomEdgeY + control->root.base.top,control->root.base.left,
                 UI_WINDOW_SUBRESOURCE_WINDOW_FRAME + UI_WINDOW_FRAME_BOTTOM_LEFT,
                 g_UiWindowTextureSource,g_FramebufferAccess);
      g_GraphicsTextureSourceBlitSourceAlpha
                (clipBottom,clipRight,clipTop,clipLeft,bottomEdgeY + control->root.base.top,
                 rightEdgeX + control->root.base.left,UI_WINDOW_SUBRESOURCE_WINDOW_FRAME + UI_WINDOW_FRAME_BOTTOM_RIGHT,
                 g_UiWindowTextureSource,g_FramebufferAccess);
      UiWindow_BlitTiledHorizontalEdge
                (clipBottom,clipRight,clipTop,clipLeft,UI_WINDOW_SUBRESOURCE_WINDOW_FRAME + UI_WINDOW_FRAME_TOP,
                 rightEdgeX,0,cornerWidth,control);
      UiWindow_BlitTiledVerticalEdge
                (clipBottom,clipRight,clipTop,clipLeft,UI_WINDOW_SUBRESOURCE_WINDOW_FRAME + UI_WINDOW_FRAME_LEFT,
                 bottomEdgeY,cornerHeight,0,control);
      UiWindow_BlitTiledVerticalEdge
                (clipBottom,clipRight,clipTop,clipLeft,UI_WINDOW_SUBRESOURCE_WINDOW_FRAME + UI_WINDOW_FRAME_RIGHT,
                 bottomEdgeY,cornerHeight,rightEdgeX,control);
      UiWindow_BlitTiledHorizontalEdge
                (clipBottom,clipRight,clipTop,clipLeft,UI_WINDOW_SUBRESOURCE_WINDOW_FRAME + UI_WINDOW_FRAME_BOTTOM,
                 rightEdgeX,bottomEdgeY,cornerWidth,control);
    }
    if ((control->root.rootFlags & UI_ROOT_TITLE_BAR) != 0) {
      titleBarSubresource = UI_WINDOW_SUBRESOURCE_TITLE_BAR;
      if ((control->root.base.nodeFlags & UI_NODE_IN_FRONT_ROOT) == 0) {
        titleBarSubresource = UI_WINDOW_SUBRESOURCE_TITLE_BAR_INACTIVE;
      }
      g_GraphicsTextureSourceBlitSourceAlpha
                (clipBottom,clipRight,clipTop,clipLeft,control->root.base.top,control->root.base.left,
                 titleBarSubresource,
                 g_UiWindowTextureSource,g_FramebufferAccess);
      textureSize = g_GraphicsTextureSourceGetLogicalSize(titleBarSubresource,g_UiWindowTextureSource);
      rightCapSize = g_GraphicsTextureSourceGetLogicalSize
                               (titleBarSubresource + UI_WINDOW_TITLE_BAR_RIGHT,g_UiWindowTextureSource);
      rightCapX = control->root.base.layoutWidth - rightCapSize.logicalWidthPixels;
      g_GraphicsTextureSourceBlitSourceAlpha
                (clipBottom,clipRight,clipTop,clipLeft,control->root.base.top,rightCapX + control->root.base.left,
                 titleBarSubresource + UI_WINDOW_TITLE_BAR_RIGHT,g_UiWindowTextureSource,g_FramebufferAccess);
      UiWindow_BlitTiledHorizontalEdge
                (clipBottom,clipRight,clipTop,clipLeft,titleBarSubresource + UI_WINDOW_TITLE_BAR_MIDDLE,
                 rightCapX,0,textureSize.logicalWidthPixels,control);
      titleText = TextResource_Resolve(control->titleTextResourceId);
      RichTextCommandStream_DrawSingleLine
                (clipBottom,clipRight,clipTop,clipLeft,g_UiResizableWindowTitleTextStyle,titleText,
                 g_UiResizableWindowTitleTextTopOffset + control->root.base.top,
                 (control->root.base.layoutWidth >> 1) + control->root.base.left);
    }
    if ((control->root.rootFlags & UI_ROOT_CLOSE_BUTTON) != 0) {
      buttonSubresource = UI_WINDOW_SUBRESOURCE_CLOSE_BUTTON;
      if ((control->root.rootFlags & UI_ROOT_CLOSE_ARMED) != 0) {
        buttonSubresource = UI_WINDOW_SUBRESOURCE_CLOSE_BUTTON_ARMED;
      }
      if ((control->root.base.nodeFlags & UI_NODE_IN_FRONT_ROOT) == 0) {
        buttonSubresource = UI_WINDOW_SUBRESOURCE_CLOSE_BUTTON_INACTIVE;
      }
      g_GraphicsTextureSourceBlitSourceAlpha
                (clipBottom,clipRight,clipTop,clipLeft,control->root.base.top,control->root.base.left,
                 buttonSubresource,
                 g_UiWindowTextureSource,g_FramebufferAccess);
    }
    if ((control->root.rootFlags & UI_ROOT_MAXIMIZE_BUTTON) != 0) {
      buttonSubresource = UI_WINDOW_SUBRESOURCE_MAXIMIZE_BUTTON;
      if ((control->root.rootFlags & UI_ROOT_MAXIMIZE_ARMED) != 0) {
        buttonSubresource = UI_WINDOW_SUBRESOURCE_MAXIMIZE_BUTTON_ARMED;
      }
      if ((control->root.base.nodeFlags & UI_NODE_IN_FRONT_ROOT) == 0) {
        buttonSubresource = UI_WINDOW_SUBRESOURCE_MAXIMIZE_BUTTON_INACTIVE;
      }
      if ((control->root.rootFlags & UI_ROOT_MAXIMIZED) != 0) {
        buttonSubresource = buttonSubresource + UI_WINDOW_RESTORE_BUTTON_OFFSET;
      }
      textureSize = g_GraphicsTextureSourceGetLogicalSize(buttonSubresource,g_UiWindowTextureSource);
      g_GraphicsTextureSourceBlitSourceAlpha
                (clipBottom,clipRight,clipTop,clipLeft,control->root.base.top,
                 (control->root.base.layoutWidth - textureSize.logicalWidthPixels) + control->root.base.left,
                 buttonSubresource,
                 g_UiWindowTextureSource,
                 g_FramebufferAccess);
    }
    g_GraphicsFramebufferEndAccess();
  }
  UiContainer_DrawIntersectingChildren(clipBottom,clipRight,clipTop,clipLeft,(UiNodeBase *)control);
  return;
}

/* drawClipped of g_UiTitledWindowControlVtable: draws the group-box frame with the title text set into its
   top edge (between two caps, centred when titleFlags bit 0 is set, otherwise after the top-left corner),
   then the children. The top edge is tiled left of the title only when it is centred.
*/
void UiTitledWindowControl_DrawFrameTitleAndChildren
          (UiPixelCoordinate clipBottom,UiPixelCoordinate clipRight,UiPixelCoordinate clipTop,
          UiPixelCoordinate clipLeft,UiTitledWindowControl *control)

{
  uint32_t cornerWidth;
  uint16_t *commandStream;
  int bottomEdgeY;
  uint32_t titleCapX;
  int rightEdgeX;
  int titleTextX;
  int rightCapX;
  Bool8 beginAccessFailed;
  RichTextExtent titleExtent;
  uint16_t *titleText;
  GraphicsTextureLogicalSize textureSize;

  beginAccessFailed = g_GraphicsFramebufferBeginAccess();
  if (!beginAccessFailed) {
    g_GraphicsTextureSourceBlitSourceAlpha
              (clipBottom,clipRight,clipTop,clipLeft,control->base.top,control->base.left,
               UI_WINDOW_SUBRESOURCE_TITLED_BOX_FRAME + UI_WINDOW_FRAME_TOP_LEFT,
               g_UiWindowTextureSource,g_FramebufferAccess);
    textureSize = g_GraphicsTextureSourceGetLogicalSize
                            (UI_WINDOW_SUBRESOURCE_TITLED_BOX_FRAME + UI_WINDOW_FRAME_BOTTOM_RIGHT,
                             g_UiWindowTextureSource);
    cornerWidth = textureSize.logicalWidthPixels;
    rightEdgeX = control->base.layoutWidth - cornerWidth;
    bottomEdgeY = control->base.layoutHeight - textureSize.logicalHeightPixels;
    g_GraphicsTextureSourceBlitSourceAlpha
              (clipBottom,clipRight,clipTop,clipLeft,control->base.top,rightEdgeX + control->base.left,
               UI_WINDOW_SUBRESOURCE_TITLED_BOX_FRAME + UI_WINDOW_FRAME_TOP_RIGHT,
               g_UiWindowTextureSource,g_FramebufferAccess);
    g_GraphicsTextureSourceBlitSourceAlpha
              (clipBottom,clipRight,clipTop,clipLeft,bottomEdgeY + control->base.top,control->base.left,
               UI_WINDOW_SUBRESOURCE_TITLED_BOX_FRAME + UI_WINDOW_FRAME_BOTTOM_LEFT,
               g_UiWindowTextureSource,g_FramebufferAccess);
    g_GraphicsTextureSourceBlitSourceAlpha
              (clipBottom,clipRight,clipTop,clipLeft,bottomEdgeY + control->base.top,
               rightEdgeX + control->base.left,
               UI_WINDOW_SUBRESOURCE_TITLED_BOX_FRAME + UI_WINDOW_FRAME_BOTTOM_RIGHT,
               g_UiWindowTextureSource,g_FramebufferAccess);
    textureSize = g_GraphicsTextureSourceGetLogicalSize
                            (UI_WINDOW_SUBRESOURCE_TITLED_BOX_FRAME + UI_WINDOW_FRAME_TOP_LEFT,
                             g_UiWindowTextureSource);
    UiWindow_BlitTiledVerticalEdge
              (clipBottom,clipRight,clipTop,clipLeft,UI_WINDOW_SUBRESOURCE_TITLED_BOX_FRAME + UI_WINDOW_FRAME_LEFT,
               bottomEdgeY,textureSize.logicalHeightPixels,0,control);
    UiWindow_BlitTiledVerticalEdge
              (clipBottom,clipRight,clipTop,clipLeft,UI_WINDOW_SUBRESOURCE_TITLED_BOX_FRAME + UI_WINDOW_FRAME_RIGHT,
               bottomEdgeY,textureSize.logicalHeightPixels,rightEdgeX,control);
    UiWindow_BlitTiledHorizontalEdge
              (clipBottom,clipRight,clipTop,clipLeft,UI_WINDOW_SUBRESOURCE_TITLED_BOX_FRAME + UI_WINDOW_FRAME_BOTTOM,
               rightEdgeX,bottomEdgeY,cornerWidth,control);
    titleText = TextResource_Resolve(control->titleTextResourceId);
    commandStream = titleText;
    titleExtent = RichTextCommandStream_MeasureLine(g_UiWindowTitleTextStyle,commandStream);
    titleCapX = cornerWidth;
    if ((control->titleFlags & UI_TITLED_WINDOW_CENTERED_TITLE) != 0) {
      /* centred title: the top edge runs from the corner to the left cap */
      textureSize = g_GraphicsTextureSourceGetLogicalSize
                              (UI_WINDOW_SUBRESOURCE_TITLED_BOX_TITLE_LEFT,g_UiWindowTextureSource);
      titleCapX = ((int)(control->base.layoutWidth - titleExtent.widthPixels) >> 1) - textureSize.logicalWidthPixels;
      UiWindow_BlitTiledHorizontalEdge
                (clipBottom,clipRight,clipTop,clipLeft,UI_WINDOW_SUBRESOURCE_TITLED_BOX_FRAME + UI_WINDOW_FRAME_TOP,
                 titleCapX,0,cornerWidth,control);
    }
    g_GraphicsTextureSourceBlitSourceAlpha
              (clipBottom,clipRight,clipTop,clipLeft,control->base.top,titleCapX + control->base.left,
               UI_WINDOW_SUBRESOURCE_TITLED_BOX_TITLE_LEFT,g_UiWindowTextureSource,g_FramebufferAccess);
    textureSize = g_GraphicsTextureSourceGetLogicalSize
                            (UI_WINDOW_SUBRESOURCE_TITLED_BOX_TITLE_LEFT,g_UiWindowTextureSource);
    titleTextX = titleCapX + textureSize.logicalWidthPixels;
    RichTextCommandStream_DrawSingleLine
              (clipBottom,clipRight,clipTop,clipLeft,g_UiWindowTitleTextStyle,commandStream,
               control->base.top,titleTextX + control->base.left);
    titleExtent = RichTextCommandStream_MeasureLine(g_UiWindowTitleTextStyle,commandStream);
    rightCapX = titleTextX + titleExtent.widthPixels;
    g_GraphicsTextureSourceBlitSourceAlpha
              (clipBottom,clipRight,clipTop,clipLeft,control->base.top,rightCapX + control->base.left,
               UI_WINDOW_SUBRESOURCE_TITLED_BOX_TITLE_RIGHT,g_UiWindowTextureSource,g_FramebufferAccess);
    textureSize = g_GraphicsTextureSourceGetLogicalSize
                            (UI_WINDOW_SUBRESOURCE_TITLED_BOX_TITLE_RIGHT,g_UiWindowTextureSource);
    /* the top edge from the right cap to the top-right corner */
    UiWindow_BlitTiledHorizontalEdge
              (clipBottom,clipRight,clipTop,clipLeft,UI_WINDOW_SUBRESOURCE_TITLED_BOX_FRAME + UI_WINDOW_FRAME_TOP,
               rightEdgeX,0,rightCapX + textureSize.logicalWidthPixels,control);
    g_GraphicsFramebufferEndAccess();
  }
  UiContainer_DrawIntersectingChildren(clipBottom,clipRight,clipTop,clipLeft,(UiNodeBase *)control);
  return;
}

/* Restores the rectangle saved when the window was maximized and lays the window out again. */
static void UiResizableWindowControl_RestoreSavedRectangle(UiResizableWindowControl *control)

{
  control->root.base.left = control->restoredLeft;
  control->root.base.top = control->restoredTop;
  control->root.base.bottom = control->restoredBottom;
  control->root.base.right = control->restoredRight;
  UiContainer_LayoutWithOptionalWindowHeaderOffset(control);
  return;
}

/* Maximizes the window: saves its rectangle for the restore, makes it cover the whole framebuffer and lays it
   out again. */
static void UiResizableWindowControl_SaveRectangleAndCoverFramebuffer(UiResizableWindowControl *control)

{
  control->restoredLeft = control->root.base.left;
  control->restoredTop = control->root.base.top;
  control->restoredBottom = control->root.base.bottom;
  control->restoredRight = control->root.base.right;
  control->root.base.right = g_FramebufferWidth;
  control->root.base.left = 0;
  control->root.base.top = 0;
  control->root.base.bottom = g_FramebufferHeight;
  UiContainer_LayoutWithOptionalWindowHeaderOffset(control);
  return;
}

/* nonRightRelease of g_UiResizableWindowControlVtable: ends a move or resize and completes a button press.
   Releasing over the armed close button closes the window, but only when the press was a double click
   (or a middle-button press), like the control-menu box of old Windows versions. Releasing over the
   armed maximize button toggles between the full framebuffer and the saved rectangle.
*/
void UiResizableWindowControl_EndMoveResizeAndHandleWindowActions
          (UiPointerWheelDelta wheelDelta,UiPixelCoordinate pointerY,UiPixelCoordinate pointerX,
          UiResizableWindowControl *control)

{
  if ((control->root.rootFlags & UI_ROOT_MOVING) != 0) {
    g_GraphicsCursorSetFrame(GRAPHICS_CURSOR_FRAME_ARROW);
  }
  if (((control->root.rootFlags & UI_ROOT_CLOSE_ARMED) != 0) &&
     ((control->root.base.nodeFlags & UI_NODE_REPEAT_OR_DOUBLE_CLICK) != 0)) {
    UiActionQueue_Enqueue(UI_ACTION_CLOSE_ROOT,control);
  }
  if ((control->root.rootFlags & UI_ROOT_MAXIMIZE_ARMED) != 0) {
    control->root.rootFlags = control->root.rootFlags ^ UI_ROOT_MAXIMIZED;
    if ((control->root.rootFlags & UI_ROOT_MAXIMIZED) == 0) {
      UiResizableWindowControl_RestoreSavedRectangle(control);
      UiRootStack_InvalidateAll();
      control->root.rootFlags = control->root.rootFlags & ~UI_ROOT_POINTER_STATE;
      return;
    }
    UiResizableWindowControl_SaveRectangleAndCoverFramebuffer(control);
  }
  UiNode_InvalidateRoot((UiNodeBase *)control);
  control->root.rootFlags = control->root.rootFlags & ~UI_ROOT_POINTER_STATE;
  return;
}

/* keyboardEvent of g_UiResizableWindowControlVtable: Alt+C closes the window (with a close button), Alt+Z
   toggles maximize (with a maximize button), both returning false; every other key goes to the default
   focus-moving handler. The key events of Keyboard_OnKeyDown carry letters as KEYBOARD_KEY_CODE_CHAR
   (0x30000 + code), so the plain 'c' / 'z' compared here never arrive and the hotkeys do not fire.
*/
Bool8 UiResizableWindowControl_HandleWindowHotkeys
          (UiKeyboardStateMask keyboardStateMask,UiKeyboardEventCode keyCode,UiResizableWindowControl *control)

{
  Bool8 delegateResult;

  if ((keyboardStateMask & KEYBOARD_STATE_ALT) != 0) {
    if (((control->root.rootFlags & UI_ROOT_CLOSE_BUTTON) != 0) && (keyCode == 'c')) {
      UiActionQueue_Enqueue(UI_ACTION_CLOSE_ROOT,control);
      return false;
    }
    if (((control->root.rootFlags & UI_ROOT_MAXIMIZE_BUTTON) != 0) && (keyCode == 'z')) {
      control->root.rootFlags = control->root.rootFlags ^ UI_ROOT_MAXIMIZED;
      if ((control->root.rootFlags & UI_ROOT_MAXIMIZED) == 0) {
        UiResizableWindowControl_RestoreSavedRectangle(control);
        UiRootStack_InvalidateAll();
      }
      else {
        UiResizableWindowControl_SaveRectangleAndCoverFramebuffer(control);
        UiNode_InvalidateRoot((UiNodeBase *)control);
      }
      return false;
    }
  }
  delegateResult = UiNode_DefaultKeyboardEventMoveFocusNext(keyboardStateMask,keyCode,(UiNodeBase *)control);
  return delegateResult;
}

/* relocate of g_UiWindowControlVtable, the same as UiFramedTextButtonControl_Relocate: an inset-framed
   control (UI_BUTTON_FRAME_INSET) grows its layout offsets by g_UiWindowFrameInset on every side, so the
   frame lies outside the authored box; then the children are relocated.
*/
void UiWindowControl_RelocateWithFrameInset(UiSerializedRelocationDelta relocationDelta,UiWindowControl *control)

{
  int frameInset;

  frameInset = g_UiWindowFrameInset;
  if ((control->selectable.stateFlags & UI_BUTTON_FRAME_INSET) != 0) {
    control->selectable.base.leftOffset = control->selectable.base.leftOffset - g_UiWindowFrameInset;
    control->selectable.base.topOffset = control->selectable.base.topOffset - frameInset;
    control->selectable.base.rightOffset = control->selectable.base.rightOffset + frameInset;
    control->selectable.base.bottomOffset = control->selectable.base.bottomOffset + frameInset;
  }
  UiContainer_RelocateChildren(relocationDelta,(UiNodeBase *)control);
  return;
}

/* layout of g_UiTitledWindowControlVtable: lays out the children inside the frame, i.e. with the rectangle
   shrunk by the top-left corner (or the title height when that is taller) and the bottom-right corner,
   then restores the rectangle and grows layoutWidth/layoutHeight back to the full box.
*/
void UiTitledWindowControl_LayoutFrameTitleAndChildren(UiTitledWindowControl *control)

{
  uint32_t leftInset;
  uint32_t titleHeight;
  uint32_t rightInset;
  uint32_t topInset;
  uint32_t bottomInset;
  RichTextExtent titleExtent;
  uint16_t *titleText;
  GraphicsTextureLogicalSize cornerSize;

  titleText = TextResource_Resolve(control->titleTextResourceId);
  titleExtent = RichTextCommandStream_MeasureLine(g_UiWindowTitleTextStyle,titleText);
  titleHeight = titleExtent.heightPixels;
  cornerSize = g_GraphicsTextureSourceGetLogicalSize
                         (UI_WINDOW_SUBRESOURCE_TITLED_BOX_FRAME + UI_WINDOW_FRAME_TOP_LEFT,g_UiWindowTextureSource);
  leftInset = cornerSize.logicalWidthPixels;
  topInset = cornerSize.logicalHeightPixels;
  if ((int)cornerSize.logicalHeightPixels < (int)titleHeight) {
    topInset = titleHeight;
  }
  control->base.left = control->base.left + leftInset;
  control->base.top = control->base.top + topInset;
  cornerSize = g_GraphicsTextureSourceGetLogicalSize
                         (UI_WINDOW_SUBRESOURCE_TITLED_BOX_FRAME + UI_WINDOW_FRAME_BOTTOM_RIGHT,
                          g_UiWindowTextureSource);
  bottomInset = cornerSize.logicalHeightPixels;
  rightInset = cornerSize.logicalWidthPixels;
  control->base.right = control->base.right - rightInset;
  control->base.bottom = control->base.bottom - bottomInset;
  UiContainer_LayoutChildren((UiNodeBase *)control);
  control->base.right = control->base.right + rightInset;
  control->base.bottom = control->base.bottom + bottomInset;
  control->base.layoutWidth = control->base.layoutWidth + rightInset;
  control->base.layoutHeight = control->base.layoutHeight + bottomInset;
  control->base.left = control->base.left - leftInset;
  control->base.top = control->base.top - topInset;
  control->base.layoutWidth = control->base.layoutWidth + leftInset;
  control->base.layoutHeight = control->base.layoutHeight + topInset;
  return;
}

/* relocate of g_UiResizableWindowControlVtable: relocates the children, then makes the window a keyboard
   focus target only when it has a close or maximize button, i.e. hotkeys for
   UiResizableWindowControl_HandleWindowHotkeys.
*/
void UiResizableWindowControl_RelocateAndRefreshInteractionState
          (UiSerializedRelocationDelta relocationDelta,UiResizableWindowControl *control)

{
  UiContainer_RelocateChildren(relocationDelta,(UiNodeBase *)control);
  if ((control->root.rootFlags & (UI_ROOT_CLOSE_BUTTON | UI_ROOT_MAXIMIZE_BUTTON)) == 0) {
    control->root.base.nodeFlags =
         control->root.base.nodeFlags & ~(UI_NODE_FALLBACK_FOCUS_TARGET|UI_NODE_PREFERRED_FOCUS_TARGET);
  }
  else {
    control->root.base.nodeFlags = control->root.base.nodeFlags | UI_NODE_FALLBACK_FOCUS_TARGET;
  }
  return;
}

/* Sets or clears armedFlag (UI_ROOT_CLOSE_ARMED / UI_ROOT_MAXIMIZE_ARMED) of a pressed window button to match
   overButton. Returns true when the flag changed (the button needs a redraw). */
static Bool8 UiResizableWindowControl_UpdateArmedFlag
          (Bool8 overButton,UiRootFlags armedFlag,UiResizableWindowControl *control)

{
  if (overButton) {
    if ((control->root.rootFlags & armedFlag) != 0) {
      return false;
    }
    control->root.rootFlags = control->root.rootFlags | armedFlag;
  }
  else {
    if ((control->root.rootFlags & armedFlag) == 0) {
      return false;
    }
    control->root.rootFlags = control->root.rootFlags & ~armedFlag;
  }
  return true;
}

/* Resize step of UiResizableWindowControl_UpdateMoveOrResize: the grabbed edges (UI_ROOT_RESIZE_MOVES_*) follow
   the pointer, the others stay; a window narrower or lower than UI_WINDOW_MINIMUM_SIZE gets its grabbed edge(s)
   pushed back by the shortfall. The new right/bottom edges pass through dragAnchorXOrPendingRight /
   dragAnchorYOrPendingBottom. Relays the window out only when an edge changed. */
static void UiResizableWindowControl_ResizeToPointer
          (UiPixelCoordinate pointerY,UiPixelCoordinate pointerX,UiResizableWindowControl *control)

{
  uint32_t interactionFlags;
  int newLeft;
  int newTop;
  int pendingRight;
  int pendingBottom;
  int widthShortfall;
  int heightShortfall;
  int32_t newRight;
  int32_t newBottom;
  int oldLeft;
  int oldTop;
  int oldRight;
  int oldBottom;

  interactionFlags = control->root.rootFlags;
  newTop = pointerY;
  if ((interactionFlags & UI_ROOT_RESIZE_MOVES_TOP) == 0) {
    newTop = control->root.base.top;
  }
  pendingBottom = pointerY;
  if ((interactionFlags & UI_ROOT_RESIZE_MOVES_BOTTOM) == 0) {
    pendingBottom = control->root.base.bottom;
  }
  newLeft = pointerX;
  if ((interactionFlags & UI_ROOT_RESIZE_MOVES_LEFT) == 0) {
    newLeft = control->root.base.left;
  }
  pendingRight = pointerX;
  if ((interactionFlags & UI_ROOT_RESIZE_MOVES_RIGHT) == 0) {
    pendingRight = control->root.base.right;
  }
  control->dragAnchorXOrPendingRight = pendingRight;
  control->dragAnchorYOrPendingBottom = pendingBottom;
  /* too narrow or too low: push the grabbed edge(s) back by the shortfall */
  widthShortfall = (pendingRight - newLeft) - UI_WINDOW_MINIMUM_SIZE;
  if (widthShortfall < 0) {
    if ((interactionFlags & UI_ROOT_RESIZE_MOVES_RIGHT) != 0) {
      control->dragAnchorXOrPendingRight = control->dragAnchorXOrPendingRight - widthShortfall;
    }
    if ((interactionFlags & UI_ROOT_RESIZE_MOVES_LEFT) != 0) {
      newLeft = newLeft + widthShortfall;
    }
  }
  heightShortfall = (pendingBottom - newTop) - UI_WINDOW_MINIMUM_SIZE;
  if (heightShortfall < 0) {
    if ((interactionFlags & UI_ROOT_RESIZE_MOVES_BOTTOM) != 0) {
      control->dragAnchorYOrPendingBottom = control->dragAnchorYOrPendingBottom - heightShortfall;
    }
    if ((interactionFlags & UI_ROOT_RESIZE_MOVES_TOP) != 0) {
      newTop = newTop + heightShortfall;
    }
  }
  newRight = control->dragAnchorXOrPendingRight;
  newBottom = control->dragAnchorYOrPendingBottom;
  /* exchange old and new edges; relayout only when one changed */
  oldLeft = control->root.base.left;
  control->root.base.left = newLeft;
  oldTop = control->root.base.top;
  control->root.base.top = newTop;
  oldRight = control->root.base.right;
  control->root.base.right = newRight;
  oldBottom = control->root.base.bottom;
  control->root.base.bottom = newBottom;
  if (oldLeft != control->root.base.left || oldTop != control->root.base.top ||
      oldRight != control->root.base.right || oldBottom != control->root.base.bottom) {
    UiRootStack_InvalidateAll();
    control->root.base.vtable->layout((UiNodeBase *)control);
    UiNode_InvalidateRoot((UiNodeBase *)control);
  }
  return;
}

/* nonRightDrag of g_UiResizableWindowControlVtable: while moving, shifts the window by the pointer's
   movement since the grab; while resizing, moves the grabbed edges to the pointer, keeping the window at
   least UI_WINDOW_MINIMUM_SIZE wide and high, and relays it out when the rectangle changed. While the close
   or maximize button is pressed it only tracks whether the pointer is still over it (armed).
*/
void UiResizableWindowControl_UpdateMoveOrResize
          (UiPointerWheelDelta wheelDelta,UiPixelCoordinate pointerY,UiPixelCoordinate pointerX,
          UiResizableWindowControl *control)

{
  int localX;
  int localY;
  int moveX;
  int moveY;
  Bool8 overButton;
  GraphicsTextureLogicalSize buttonSize;

  localX = pointerX - control->root.base.left;
  localY = pointerY - control->root.base.top;
  if ((control->root.rootFlags & UI_ROOT_MOVING) != 0) {
    /* moving: dragAnchor* hold the grab point relative to the window */
    moveX = localX - control->dragAnchorXOrPendingRight;
    moveY = localY - control->dragAnchorYOrPendingBottom;
    if ((moveY != 0) || (moveX != 0)) {
      UiRootStack_InvalidateAll();
      control->root.base.left = control->root.base.left + moveX;
      control->root.base.top = control->root.base.top + moveY;
      control->root.base.right = control->root.base.right + moveX;
      control->root.base.bottom = control->root.base.bottom + moveY;
      control->root.base.vtable->layout((UiNodeBase *)control);
      UiNode_InvalidateRoot((UiNodeBase *)control);
    }
  }
  else if ((control->root.rootFlags & UI_ROOT_CLOSE_PRESSED) != 0) {
    /* close button pressed: armed while the pointer is on its opaque pixels (top left) */
    overButton = g_GraphicsTextureSourceTestOpaquePixel
                      (localY,localX,0,0,UI_WINDOW_SUBRESOURCE_CLOSE_BUTTON,g_UiWindowTextureSource);
    if (UiResizableWindowControl_UpdateArmedFlag(overButton,UI_ROOT_CLOSE_ARMED,control)) {
      /* result unused */
      g_GraphicsTextureSourceGetLogicalSize(UI_WINDOW_SUBRESOURCE_CLOSE_BUTTON,g_UiWindowTextureSource);
      UiNode_InvalidateRoot((UiNodeBase *)control);
    }
  }
  else if ((control->root.rootFlags & UI_ROOT_MAXIMIZE_PRESSED) != 0) {
    /* maximize button pressed: armed while the pointer is on its opaque pixels (top right) */
    buttonSize = g_GraphicsTextureSourceGetLogicalSize(UI_WINDOW_SUBRESOURCE_MAXIMIZE_BUTTON,
                                                       g_UiWindowTextureSource);
    overButton = g_GraphicsTextureSourceTestOpaquePixel
                      (localY,localX,0,control->root.base.layoutWidth - buttonSize.logicalWidthPixels,
                       UI_WINDOW_SUBRESOURCE_MAXIMIZE_BUTTON,g_UiWindowTextureSource);
    if (UiResizableWindowControl_UpdateArmedFlag(overButton,UI_ROOT_MAXIMIZE_ARMED,control)) {
      UiNode_InvalidateRoot((UiNodeBase *)control);
    }
  }
  else if ((control->root.rootFlags & UI_ROOT_RESIZING) != 0) {
    UiResizableWindowControl_ResizeToPointer(pointerY,pointerX,control);
  }
  return;
}

/* Loads what every window needs: the frame graphics (engine\win.gfx, engine\winclass.gfx) and their texts
   (texte\winclass.str as text page 1), installs the root-stack actions as action-handler page 0 and starts
   with an empty root stack. A missing file is fatal.
*/
void UiWindowResources_Init()

{
  GraphicsTextureSourceAsset *loadedTexture;
  uint32_t textureLoadError;
  uintptr_t checkedValue;
  uintptr_t pageLoadError;
  Bool8 pageLoaded;

  loadedTexture = g_GraphicsTextureSourceLoadPackageAsset((uint16_t *)g_UiWindowTexturePathUtf16,&textureLoadError);
  checkedValue = FatalError_ExitIfFailed(loadedTexture != nullptr ? (uintptr_t)loadedTexture : textureLoadError,
                                          loadedTexture == nullptr);
  g_UiWindowTextureSource = (GraphicsTextureSourceAsset *)checkedValue;
  loadedTexture = g_GraphicsTextureSourceLoadPackageAsset(g_UiWindowClassTexturePathUtf16,&textureLoadError);
  checkedValue = FatalError_ExitIfFailed(loadedTexture != nullptr ? (uintptr_t)loadedTexture : textureLoadError,
                                          loadedTexture == nullptr);
  g_UiWindowClassTextureSource = (GraphicsTextureSourceAsset *)checkedValue;
  /* the out value is the error code on failure; on success it is only passed through unused */
  pageLoaded = TextResourcePage_Load(1,(uint16_t *)g_UiWindowClassTextPathUtf16,&pageLoadError);
  FatalError_ExitIfFailed(pageLoadError,!pageLoaded);
  UiActionHandlers_SetPage(0,(UiActionHandlerPage *)&g_UiRootStackActionHandlerPage);
  g_UiRootNode = UI_ROOT_STACK_END;
  return;
}

/* nonRightPress of g_UiResizableWindowControlVtable: a press on the close or maximize button (its opaque
   pixels) marks that button pressed. Otherwise, unless maximized, a press on the border of a resizable
   window (outside the inner area left by the frame corners) starts a resize of the grabbed edge or corner,
   and a press in the top UiWindowMoveHandleWidth rows of a movable window starts a move with the move cursor.
*/
void UiResizableWindowControl_BeginMoveResizeOrWindowAction
          (UiPointerWheelDelta wheelDelta,UiPixelCoordinate pointerY,UiPixelCoordinate pointerX,
          UiResizableWindowControl *control)

{
  int localX;
  uint32_t localY;
  uint32_t resizeFlags;
  int bottomBorderY;
  Bool8 hitOpaque;
  GraphicsTextureLogicalSize textureSize;

  localX = pointerX - control->root.base.left;
  localY = pointerY - control->root.base.top;
  if ((control->root.rootFlags & UI_ROOT_CLOSE_BUTTON) != 0) {
    hitOpaque = g_GraphicsTextureSourceTestOpaquePixel
                      (localY,localX,0,0,UI_WINDOW_SUBRESOURCE_CLOSE_BUTTON,g_UiWindowTextureSource);
    if (hitOpaque) {
      control->root.rootFlags = control->root.rootFlags | UI_ROOT_CLOSE_PRESSED;
      return;
    }
  }
  if ((control->root.rootFlags & UI_ROOT_MAXIMIZE_BUTTON) != 0) {
    textureSize = g_GraphicsTextureSourceGetLogicalSize(UI_WINDOW_SUBRESOURCE_MAXIMIZE_BUTTON,g_UiWindowTextureSource);
    hitOpaque = g_GraphicsTextureSourceTestOpaquePixel
                      (localY,localX,0,control->root.base.layoutWidth - textureSize.logicalWidthPixels,
                       UI_WINDOW_SUBRESOURCE_MAXIMIZE_BUTTON,g_UiWindowTextureSource);
    if (hitOpaque) {
      control->root.rootFlags = control->root.rootFlags | UI_ROOT_MAXIMIZE_PRESSED;
      return;
    }
  }
  if ((control->root.rootFlags & UI_ROOT_MAXIMIZED) == 0) {
    if ((control->root.rootFlags & UI_ROOT_RESIZABLE) != 0) {
      textureSize = g_GraphicsTextureSourceGetLogicalSize
                              (UI_WINDOW_SUBRESOURCE_WINDOW_FRAME + UI_WINDOW_FRAME_TOP_LEFT,g_UiWindowTextureSource);
      if (localX < (int)textureSize.logicalWidthPixels || (int)localY < (int)textureSize.logicalHeightPixels ||
          (int)(control->root.base.layoutWidth - textureSize.logicalWidthPixels) <= localX ||
          (int)(control->root.base.layoutHeight - textureSize.logicalHeightPixels) <= (int)localY) {
        /* which border: within g_UiWindowResizeBorderThickness of a side (corners take precedence) */
        bottomBorderY = control->root.base.layoutHeight - g_UiWindowResizeBorderThickness;
        if (localX < g_UiWindowResizeBorderThickness) {
          if ((int)localY < g_UiWindowResizeBorderThickness) {
            resizeFlags = UI_ROOT_RESIZE_TOP_LEFT | UI_ROOT_RESIZING;
          }
          else if ((int)localY < bottomBorderY) {
            resizeFlags = UI_ROOT_RESIZE_LEFT | UI_ROOT_RESIZING;
          }
          else {
            resizeFlags = UI_ROOT_RESIZE_BOTTOM_LEFT | UI_ROOT_RESIZING;
          }
        }
        else if (localX < control->root.base.layoutWidth - g_UiWindowResizeBorderThickness) {
          resizeFlags = UI_ROOT_RESIZE_TOP | UI_ROOT_RESIZING;
          if (g_UiWindowResizeBorderThickness <= (int)localY) {
            resizeFlags = UI_ROOT_RESIZE_BOTTOM | UI_ROOT_RESIZING;
          }
        }
        else {
          if ((int)localY < g_UiWindowResizeBorderThickness) {
            resizeFlags = UI_ROOT_RESIZE_TOP_RIGHT | UI_ROOT_RESIZING;
          }
          else if ((int)localY < bottomBorderY) {
            resizeFlags = UI_ROOT_RESIZE_RIGHT | UI_ROOT_RESIZING;
          }
          else {
            resizeFlags = UI_ROOT_RESIZE_BOTTOM_RIGHT | UI_ROOT_RESIZING;
          }
        }
        /* clears the UI_ROOT_RESIZE_EDGES byte; signed, because the unsigned ~UI_ROOT_RESIZE_EDGES changes the
           generated code of this build */
        control->root.rootFlags = control->root.rootFlags & (int)~UI_ROOT_RESIZE_EDGES;
        control->root.rootFlags = control->root.rootFlags | resizeFlags;
        return;
      }
    }
    if (((control->root.rootFlags & UI_ROOT_MOVABLE) != 0) && (localY < g_UiWindowMoveHandleWidth)) {
      control->root.rootFlags = control->root.rootFlags | UI_ROOT_MOVING;
      control->dragAnchorXOrPendingRight = localX;
      control->dragAnchorYOrPendingBottom = localY;
      g_GraphicsCursorSetFrame(GRAPHICS_CURSOR_FRAME_MOVE);
    }
  }
  return;
}

/* pointerMove of g_UiResizableWindowControlVtable: returns the cursor frame for the pointer position, a
   resize cursor over the border of a resizable, non-maximized window (the same border zones as
   UiResizableWindowControl_BeginMoveResizeOrWindowAction), otherwise the arrow.
*/
GraphicsCursorFrameIndex UiResizableWindowControl_QueryResizeCursorCode
          (UiPixelCoordinate pointerY,UiPixelCoordinate pointerX,UiResizableWindowControl *control)

{
  int bottomBorderY;
  int localY;
  int localX;
  GraphicsTextureLogicalSize cornerSize;

  if ((control->root.rootFlags & UI_ROOT_RESIZABLE) == 0 || (control->root.rootFlags & UI_ROOT_MAXIMIZED) != 0) {
    return GRAPHICS_CURSOR_FRAME_ARROW;
  }
  localX = pointerX - control->root.base.left;
  localY = pointerY - control->root.base.top;
  if (pointerX < control->root.base.left || pointerY < control->root.base.top ||
      control->root.base.layoutWidth <= localX || control->root.base.layoutHeight <= localY) {
    return GRAPHICS_CURSOR_FRAME_ARROW;
  }
  cornerSize = g_GraphicsTextureSourceGetLogicalSize
                         (UI_WINDOW_SUBRESOURCE_WINDOW_FRAME + UI_WINDOW_FRAME_TOP_LEFT,g_UiWindowTextureSource);
  if (localX >= (int)cornerSize.logicalWidthPixels && localY >= (int)cornerSize.logicalHeightPixels &&
      (int)(control->root.base.layoutWidth - cornerSize.logicalWidthPixels) > localX &&
      (int)(control->root.base.layoutHeight - cornerSize.logicalHeightPixels) > localY) {
    /* inside the area left by the frame corners */
    return GRAPHICS_CURSOR_FRAME_ARROW;
  }
  /* the same border zones as UiResizableWindowControl_BeginMoveResizeOrWindowAction */
  bottomBorderY = control->root.base.layoutHeight - g_UiWindowResizeBorderThickness;
  if (localX < g_UiWindowResizeBorderThickness) {
    if (localY < g_UiWindowResizeBorderThickness) {
      return GRAPHICS_CURSOR_FRAME_SIZE_NWSE;
    }
    if (localY < bottomBorderY) {
      return GRAPHICS_CURSOR_FRAME_SIZE_WE;
    }
    return GRAPHICS_CURSOR_FRAME_SIZE_NESW;
  }
  if (localX < control->root.base.layoutWidth - g_UiWindowResizeBorderThickness) {
    return GRAPHICS_CURSOR_FRAME_SIZE_NS;
  }
  if (localY < g_UiWindowResizeBorderThickness) {
    return GRAPHICS_CURSOR_FRAME_SIZE_NESW;
  }
  if (localY < bottomBorderY) {
    return GRAPHICS_CURSOR_FRAME_SIZE_WE;
  }
  return GRAPHICS_CURSOR_FRAME_SIZE_NWSE;
}

/* Tiles a piece of g_UiWindowTextureSource over the rectangle (tileLeft, tileTop)..(tileRight, tileBottom),
   given relative to node (a UiNodeBase), clipped to the clip rectangle. Like
   GraphicsTextureSource_BlitTiledSourceAlpha, which it forwards to, it takes the bottom/right values first.
   Called by the window, panel and button draw methods.
*/
void UiWindow_BlitTiledInterior(UiPixelCoordinate clipBottom,UiPixelCoordinate clipRight,UiPixelCoordinate clipTop,
          UiPixelCoordinate clipLeft,GraphicsSubresourceIndex subresource,UiPixelCoordinate tileBottom,
          UiPixelCoordinate tileRight,UiPixelCoordinate tileTop,UiPixelCoordinate tileLeft,
          void *node)

{
  g_GraphicsTextureSourceBlitTiledSourceAlpha
            (clipBottom,clipRight,clipTop,clipLeft,tileBottom + ((UiNodeBase *)node)->top,
             tileRight + ((UiNodeBase *)node)->left,tileTop + ((UiNodeBase *)node)->top,
             tileLeft + ((UiNodeBase *)node)->left,subresource,g_UiWindowTextureSource,
             g_FramebufferAccess);
  return;
}

/* Tiles a piece of g_UiWindowTextureSource downwards from tileTop to tileBottom in one column at tileLeft
   (relative to node, a UiNodeBase), one piece wide (INT32_MIN as right edge), clipped to the clip rectangle:
   the vertical edges of window and button frames. Bottom/right values come first, as in
   GraphicsTextureSource_BlitTiledSourceAlpha.
*/
void UiWindow_BlitTiledVerticalEdge(UiPixelCoordinate clipBottom,UiPixelCoordinate clipRight,UiPixelCoordinate clipTop,
          UiPixelCoordinate clipLeft,GraphicsSubresourceIndex subresource,UiPixelCoordinate tileBottom,
          UiPixelCoordinate tileTop,UiPixelCoordinate tileLeft,void *node)

{
  g_GraphicsTextureSourceBlitTiledSourceAlpha
            (clipBottom,clipRight,clipTop,clipLeft,tileBottom + ((UiNodeBase *)node)->top,INT32_MIN,
             tileTop + ((UiNodeBase *)node)->top,tileLeft + ((UiNodeBase *)node)->left,subresource,
             g_UiWindowTextureSource,g_FramebufferAccess);
  return;
}

/* Tiles a piece of g_UiWindowTextureSource rightwards from tileLeft to tileRight in one row at tileTop
   (relative to node, a UiNodeBase), one piece high (INT32_MIN as bottom edge), clipped to the clip
   rectangle: horizontal frame edges, title bars and gauge tracks. Bottom/right values come first, as in
   GraphicsTextureSource_BlitTiledSourceAlpha.
*/
void UiWindow_BlitTiledHorizontalEdge
          (UiPixelCoordinate clipBottom,UiPixelCoordinate clipRight,UiPixelCoordinate clipTop,
          UiPixelCoordinate clipLeft,GraphicsSubresourceIndex subresource,
          UiPixelCoordinate tileRight,UiPixelCoordinate tileTop,UiPixelCoordinate tileLeft,void *node)

{
  g_GraphicsTextureSourceBlitTiledSourceAlpha
            (clipBottom,clipRight,clipTop,clipLeft,INT32_MIN,
             tileRight + ((UiNodeBase *)node)->left,tileTop + ((UiNodeBase *)node)->top,
             tileLeft + ((UiNodeBase *)node)->left,subresource,g_UiWindowTextureSource,
             g_FramebufferAccess);
  return;
}

UiNodeVtable g_UiTitledWindowControlVtable = {
        .relocate = UI_SLOT(UiContainer_RelocateChildren),
        .method04 = UI_SLOT(UiNode_DefaultMethod04_NoOp),
        .drawClipped = UI_SLOT(UiTitledWindowControl_DrawFrameTitleAndChildren),
        .layout = UI_SLOT(UiTitledWindowControl_LayoutFrameTitleAndChildren),
        .nonRightPress = UI_SLOT(UiNode_DefaultNonRightPress),
        .nonRightRelease = UI_SLOT(UiNode_DefaultNonRightRelease),
        .rightPress = UI_SLOT(UiNode_ForwardRightPressToParent),
        .rightRelease = UI_SLOT(UiNode_DefaultRightRelease),
        .nonRightDrag = UI_SLOT(UiNode_DefaultNonRightDrag),
        .rightDrag = UI_SLOT(UiNode_DefaultRightDrag),
        .pointerMove = UI_SLOT(UiNode_DefaultPointerMove),
        .hitTest = UI_SLOT(UiContainer_HitTestChildren),
        .keyboardEvent = UI_SLOT(UiNode_DefaultKeyboardEventMoveFocusNext),
        .applyFlags = UI_SLOT(UiNode_ApplyFlagsRecursive),
        .suppressActionId = UI_SLOT(UiContainer_SuppressActionId),
        .unsuppressActionId = UI_SLOT(UiContainer_UnsuppressActionId),
        .tick = UI_SLOT(UiNode_DefaultTick),
        .pointerWheel = UI_SLOT(UiNode_ForwardPointerWheelToParent)};

UiNodeVtable g_UiResizableWindowControlVtable = {
        .relocate = UI_SLOT(UiResizableWindowControl_RelocateAndRefreshInteractionState),
        .method04 = UI_SLOT(UiNode_DefaultMethod04_NoOp),
        .drawClipped = UI_SLOT(UiResizableWindowControl_DrawFrameTitleAndChildren),
        .layout = UI_SLOT(UiContainer_LayoutWithOptionalWindowHeaderOffset),
        .nonRightPress = UI_SLOT(UiResizableWindowControl_BeginMoveResizeOrWindowAction),
        .nonRightRelease = UI_SLOT(UiResizableWindowControl_EndMoveResizeAndHandleWindowActions),
        .rightPress = UI_SLOT(UiNode_ForwardRightPressToParent),
        .rightRelease = UI_SLOT(UiNode_DefaultRightRelease),
        .nonRightDrag = UI_SLOT(UiResizableWindowControl_UpdateMoveOrResize),
        .rightDrag = UI_SLOT(UiNode_DefaultRightDrag),
        .pointerMove = UI_SLOT(UiResizableWindowControl_QueryResizeCursorCode),
        .hitTest = UI_SLOT(UiContainer_HitTestChildren),
        .keyboardEvent = UI_SLOT(UiResizableWindowControl_HandleWindowHotkeys),
        .applyFlags = UI_SLOT(UiNode_ApplyFlagsRecursive),
        .suppressActionId = UI_SLOT(UiContainer_SuppressActionId),
        .unsuppressActionId = UI_SLOT(UiContainer_UnsuppressActionId),
        .tick = UI_SLOT(UiNode_DefaultTick),
        .pointerWheel = UI_SLOT(UiNode_ForwardPointerWheelToParent)};

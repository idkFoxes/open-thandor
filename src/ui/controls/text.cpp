/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/src/ui/controls/text.cpp
 * Reverse engineering by idkFoxes 2026
 */

#include <thandor/ui/controls/text.h>
#include <thandor/thandor.h>
#include <thandor/platform/bootstrap/image.h>

/* The label's rich-text command stream: the text itself, or the resolved text resource. */
static uint16_t *UiSingleLineTextControl_GetCommandStream(UiSingleLineTextControl *control)

{
  if ((control->labelFlags & UI_LABEL_TEXT_IS_STREAM) == 0) {
    return TextResource_Resolve((TextResourceId)(uintptr_t)control->text);
  }
  return control->text;
}

/* One piece of the keyboard-focus mark at (markTop, x): drawn as its shadow (TEXT_SHADOW_COLOR_ARGB) or as
   itself. */
static void UiSingleLineTextControl_BlitFocusMarkPiece
          (UiPixelCoordinate clipBottom,UiPixelCoordinate clipRight,UiPixelCoordinate clipTop,
          UiPixelCoordinate clipLeft,int markTop,int x,uint32_t subresource,Bool8 shadow)

{
  if (shadow) {
    g_GraphicsTextureSourceBlitModulatedSourceAlpha
              (clipBottom,clipRight,clipTop,clipLeft,markTop,x,TEXT_SHADOW_COLOR_ARGB,subresource,
               g_UiWindowTextureSource,g_FramebufferAccess);
  }
  else {
    g_GraphicsTextureSourceBlitSourceAlpha
              (clipBottom,clipRight,clipTop,clipLeft,markTop,x,subresource,g_UiWindowTextureSource,
               g_FramebufferAccess);
  }
}

/* Draws the keyboard-focus mark (or its shadow) behind a label line: the left cap at markLeft, the right cap
   at markLeft + lineWidth - capWidth, and the middle tiled from the end of the left cap up to the right cap
   (or clipRight). */
static void UiSingleLineTextControl_DrawFocusMark
          (UiPixelCoordinate clipBottom,UiPixelCoordinate clipRight,UiPixelCoordinate clipTop,
          UiPixelCoordinate clipLeft,int markTop,int markLeft,int lineWidth,int capWidth,Bool8 shadow)

{
  int rightCapX;
  int tileX;
  GraphicsTextureLogicalSize tileSize;

  rightCapX = (lineWidth + markLeft) - capWidth;
  UiSingleLineTextControl_BlitFocusMarkPiece
            (clipBottom,clipRight,clipTop,clipLeft,markTop,markLeft,UI_WINDOW_SUBRESOURCE_FOCUS_MARK_LEFT,shadow);
  UiSingleLineTextControl_BlitFocusMarkPiece
            (clipBottom,clipRight,clipTop,clipLeft,markTop,rightCapX,UI_WINDOW_SUBRESOURCE_FOCUS_MARK_RIGHT,shadow);
  tileX = capWidth + markLeft;
  if (rightCapX <= clipRight) {
    clipRight = rightCapX;
  }
  tileSize = g_GraphicsTextureSourceGetLogicalSize(UI_WINDOW_SUBRESOURCE_FOCUS_MARK_MIDDLE,g_UiWindowTextureSource);
  do {
    UiSingleLineTextControl_BlitFocusMarkPiece
              (clipBottom,clipRight,clipTop,clipLeft,markTop,tileX,UI_WINDOW_SUBRESOURCE_FOCUS_MARK_MIDDLE,shadow);
    tileX = tileX + tileSize.logicalWidthPixels;
  } while (tileX < clipRight);
}

/* Draws a single-line label that forwards its focus to a child (drawClipped slot of
   g_UiFocusProxyControlVtable): measures the line, aligns it by labelFlags (room for the focus-mark caps
   when there is a focusChild), draws the focus mark and its shadow while the label has keyboard focus, then
   the line (disabled style when the focus child is suppressed). While the label holds the keyboard focus,
   the focus is lent to focusChild for drawing the children, so the child draws itself focused.
*/
void UiSingleLineTextControl_DrawClipped
          (UiPixelCoordinate clipBottom,UiPixelCoordinate clipRight,UiPixelCoordinate clipTop,
          UiPixelCoordinate clipLeft,UiSingleLineTextControl *control)

{
  UiPackedTextStyle packedStyleOverride;
  uint16_t *commandStream;
  UiNodeBase *focusLendTarget;
  int capWidth;
  uint32_t textStyle;
  int lineWidth;
  int alignOffsetY;
  int alignOffsetX;
  Bool8 framebufferUnavailable;
  RichTextExtent textExtent;
  GraphicsTextureLogicalSize tileSize;

  textStyle = g_UiTextStyleNormal;
  alignOffsetX = 0;
  alignOffsetY = 0;
  if ((((control->base).nodeFlags & UI_NODE_SUPPRESSED) == 0) ||
     ((control->labelFlags & UI_LABEL_HIDE_WHILE_SUPPRESSED) == 0)) {
    if ((control->labelFlags & UI_LABEL_OWN_STYLE_FONT) == 0) {
      control->styleOverride = control->styleOverride & ~UI_TEXT_STYLE_FONT_BYTE;
    }
    else {
      textStyle = g_UiTextStyleNormal & ~UI_TEXT_STYLE_FONT_BYTE;
    }
    if ((control->labelFlags & UI_LABEL_OWN_STYLE_PALETTE) == 0) {
      control->styleOverride = control->styleOverride & ~UI_TEXT_STYLE_PALETTE_BYTE;
    }
    else {
      textStyle = textStyle & ~UI_TEXT_STYLE_PALETTE_BYTE;
    }
    packedStyleOverride = control->styleOverride;
    commandStream = UiSingleLineTextControl_GetCommandStream(control);
    textExtent = RichTextCommandStream_MeasureLine((textStyle | packedStyleOverride) & (UI_TEXT_STYLE_FONT_BYTE|UI_TEXT_STYLE_PALETTE_BYTE),
                                                   commandStream);
    lineWidth = (int)(g_UiTextStyleNormal << 16) >> 24; /* signed byte 1 of the packed style */
    if (lineWidth < 0) {
      lineWidth = -lineWidth;
    }
    lineWidth = textExtent.widthPixels + lineWidth;
    tileSize = g_GraphicsTextureSourceGetLogicalSize(UI_WINDOW_SUBRESOURCE_FOCUS_MARK_LEFT,g_UiWindowTextureSource);
    capWidth = (int)tileSize.logicalWidthPixels;
    /* The original leaves the cap width here, reinterpreted as a pointer, as the node the focus is lent to
       below when the framebuffer cannot be accessed; NULL here because that pointer is written through. */
    focusLendTarget = nullptr;
    if (control->focusChild != nullptr) {
      lineWidth = lineWidth + capWidth * 2;
    }
    if ((control->labelFlags & UI_LABEL_ALIGN_RIGHT) != 0) {
      alignOffsetX = (control->base).layoutWidth - lineWidth;
    }
    if ((control->labelFlags & UI_LABEL_ALIGN_BOTTOM) != 0) {
      alignOffsetY = (control->base).layoutHeight - tileSize.logicalHeightPixels;
    }
    if ((control->labelFlags & UI_LABEL_CENTER_Y) != 0) {
      alignOffsetY = (int)((control->base).layoutHeight - tileSize.logicalHeightPixels) >> 1;
    }
    if ((control->labelFlags & UI_LABEL_CENTER_X) != 0) {
      alignOffsetX = ((control->base).layoutWidth - lineWidth) >> 1;
    }
    lineWidth--;
    framebufferUnavailable = g_GraphicsFramebufferBeginAccess();
    if (!framebufferUnavailable) {
      if (((control->base).nodeFlags & UI_NODE_HAS_KEYBOARD_FOCUS) != 0) {
        /* The focus mark: first its shadow one pixel down and right, then the mark itself. */
        UiSingleLineTextControl_DrawFocusMark
                  (clipBottom,clipRight,clipTop,clipLeft,alignOffsetY + 1 + (control->base).top,
                   alignOffsetX + 1 + (control->base).left,lineWidth,capWidth,true);
        UiSingleLineTextControl_DrawFocusMark
                  (clipBottom,clipRight,clipTop,clipLeft,alignOffsetY + (control->base).top,
                   alignOffsetX + (control->base).left,lineWidth,capWidth,false);
      }
      if (control->focusChild != nullptr) {
        alignOffsetX = capWidth + alignOffsetX;
        alignOffsetY++;
      }
      commandStream = UiSingleLineTextControl_GetCommandStream(control);
      focusLendTarget = control->focusChild;
      textStyle = g_UiTextStyleNormal;
      if ((focusLendTarget != nullptr) && ((focusLendTarget->nodeFlags & UI_NODE_SUPPRESSED) != 0)) {
        textStyle = g_UiTextStyleDisabled;
      }
      if ((control->labelFlags & UI_LABEL_OWN_STYLE_FONT) == 0) {
        control->styleOverride = control->styleOverride & ~UI_TEXT_STYLE_FONT_BYTE;
      }
      else {
        textStyle = textStyle & ~UI_TEXT_STYLE_FONT_BYTE;
      }
      if ((control->labelFlags & UI_LABEL_OWN_STYLE_PALETTE) == 0) {
        control->styleOverride = control->styleOverride & ~UI_TEXT_STYLE_PALETTE_BYTE;
      }
      else {
        textStyle = textStyle & ~UI_TEXT_STYLE_PALETTE_BYTE;
      }
      control->styleOverride = control->styleOverride & (UI_TEXT_STYLE_FONT_BYTE|UI_TEXT_STYLE_PALETTE_BYTE);
      RichTextCommandStream_DrawSingleLine
                (clipBottom,clipRight,clipTop,clipLeft,textStyle | control->styleOverride,
                 commandStream,alignOffsetY + (control->base).top,alignOffsetX + (control->base).left);
      g_GraphicsFramebufferEndAccess();
    }
    /* The original lends the focus whenever the label is focused, writing through NULL without a focus
       child (or through the cap width when the framebuffer could not be accessed); bounded here because
       that crashes: then the children are drawn without lending the focus. */
    if ((&control->base == g_UiKeyboardFocusNode) && (focusLendTarget == nullptr)) {
      static int s_loggedMissingFocusChild;
      if (s_loggedMissingFocusChild == 0) {
        s_loggedMissingFocusChild = 1;
        Thandor_Log("focused label %p: focus not lent (focus child %p, framebuffer %s)",
                    (void *)control,(void *)control->focusChild,
                    framebufferUnavailable ? "unavailable" : "accessed");
      }
    }
    if ((&control->base == g_UiKeyboardFocusNode) && (focusLendTarget != nullptr)) {
      g_UiKeyboardFocusNode = focusLendTarget;
      focusLendTarget->nodeFlags = focusLendTarget->nodeFlags | UI_NODE_HAS_KEYBOARD_FOCUS;
      UiContainer_DrawIntersectingChildren(clipBottom,clipRight,clipTop,clipLeft,&control->base);
      focusLendTarget->nodeFlags = focusLendTarget->nodeFlags & ~UI_NODE_HAS_KEYBOARD_FOCUS;
      g_UiKeyboardFocusNode = &control->base;
    }
    else {
      UiContainer_DrawIntersectingChildren(clipBottom,clipRight,clipTop,clipLeft,&control->base);
    }
  }
}

/* Draws a wrapped multi-line label (drawClipped slot of g_UiListOffsetControlVtable): its text (a text
   resource or a command stream) wrapped at wrapWidth, which follows the layout width unless
   UI_LABEL_KEEP_WRAP_WIDTH, in g_UiTextStyleNormal with the label's font/palette overrides; then the
   children.
*/
void UiWrappedTextControl_DrawClipped(UiPixelCoordinate clipBottom,UiPixelCoordinate clipRight,UiPixelCoordinate clipTop,
          UiPixelCoordinate clipLeft,UiWrappedTextControl *control)

{
  UiPackedTextStyle packedStyleOverride;
  uint16_t *commandStream;
  uint32_t textStyle;
  Bool8 framebufferUnavailable;
  uint16_t *resolvedText;
  
  framebufferUnavailable = g_GraphicsFramebufferBeginAccess();
  if (!framebufferUnavailable) {
    if ((control->labelFlags & UI_LABEL_KEEP_WRAP_WIDTH) == 0) {
      control->wrapWidth = (control->base).layoutWidth;
    }
    textStyle = g_UiTextStyleNormal;
    if ((control->labelFlags & UI_LABEL_OWN_STYLE_FONT) == 0) {
      control->styleOverride = control->styleOverride & ~UI_TEXT_STYLE_FONT_BYTE;
    }
    else {
      textStyle = g_UiTextStyleNormal & ~UI_TEXT_STYLE_FONT_BYTE;
    }
    if ((control->labelFlags & UI_LABEL_OWN_STYLE_PALETTE) == 0) {
      control->styleOverride = control->styleOverride & ~UI_TEXT_STYLE_PALETTE_BYTE;
    }
    else {
      textStyle = textStyle & ~UI_TEXT_STYLE_PALETTE_BYTE;
    }
    packedStyleOverride = control->styleOverride;
    commandStream = control->text;
    if ((control->labelFlags & UI_LABEL_TEXT_IS_STREAM) == 0) {
      resolvedText = TextResource_Resolve((TextResourceId)(uintptr_t)commandStream);
      commandStream = resolvedText;
    }
    RichTextCommandStream_DrawWrappedBlock
              (clipBottom,clipRight,clipTop,clipLeft,(textStyle | packedStyleOverride) & (UI_TEXT_STYLE_FONT_BYTE|UI_TEXT_STYLE_PALETTE_BYTE),
               commandStream,control->wrapWidth,(control->base).top,(control->base).left);
    g_GraphicsFramebufferEndAccess();
  }
  UiContainer_DrawIntersectingChildren(clipBottom,clipRight,clipTop,clipLeft,&control->base);
}

/* Relocation of a wrapped text control loaded from a serialized UI tree (relocate of
   g_UiListOffsetControlVtable and g_UiCommandVisibilityWrappedTextVtable): relocates the children and, when
   UI_LABEL_TEXT_NEEDS_RELOCATION marks the text pointer as a serialized offset, turns it into a pointer once.
*/
void UiWrappedTextControl_RelocateAndApplyDeferredOffset
          (UiSerializedRelocationDelta relocationDelta,UiWrappedTextControl *control)

{
  UiContainer_RelocateChildren(relocationDelta,&control->base);
  if ((control->labelFlags & UI_LABEL_TEXT_NEEDS_RELOCATION) != 0) {
    control->text = (uint16_t *)((uint8_t *)control->text + relocationDelta);
    control->labelFlags = control->labelFlags & ~UI_LABEL_TEXT_NEEDS_RELOCATION;
  }
}

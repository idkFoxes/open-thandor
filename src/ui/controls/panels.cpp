/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/src/ui/controls/panels.cpp
 * Reverse engineering by idkFoxes 2026
 */

#include <thandor/ui/controls/panels.h>
#include <thandor/thandor.h>
#include <thandor/platform/bootstrap/image.h>

/* Draws an image panel (drawClipped slot of g_UiImagePanelControlVtable): its texture aligned in the layout
   box by panelFlags (centre/right/bottom), optionally over a drop shadow, or stretched over the whole box,
   clipped to the panel; then the children. A panel without a texture or a suppressed one draws nothing, not
   even its children.
*/
void UiImagePanelControl_DrawAlignedTextureAndChildren
          (UiPixelCoordinate clipBottom,UiPixelCoordinate clipRight,UiPixelCoordinate clipTop,
          UiPixelCoordinate clipLeft,UiImagePanelControl *control)

{
  int clippedRight;
  int slackWidth;
  int clippedLeft;
  int drawX;
  int clippedBottom;
  int slackHeight;
  int clippedTop;
  int drawY;
  bool framebufferUnavailable;
  GraphicsTextureLogicalSize textureSize;
  
  if (!Any((control->base).nodeFlags & UI_NODE_SUPPRESSED)) {
    clippedLeft = (control->base).left;
    if ((control->base).left < clipLeft) {
      clippedLeft = clipLeft;
    }
    clippedTop = (control->base).top;
    if ((control->base).top < clipTop) {
      clippedTop = clipTop;
    }
    clippedRight = (control->base).right;
    if (clipRight < (control->base).right) {
      clippedRight = clipRight;
    }
    clippedBottom = (control->base).bottom;
    if (clipBottom < (control->base).bottom) {
      clippedBottom = clipBottom;
    }
    drawX = (control->base).left;
    drawY = (control->base).top;
    if (control->textureSource != nullptr) {
      textureSize = g_GraphicsTextureSourceGetLogicalSize
                        (control->subresource,control->textureSource);
      slackWidth = (control->base).layoutWidth - textureSize.logicalWidthPixels;
      slackHeight = (control->base).layoutHeight - textureSize.logicalHeightPixels;
      if ((control->panelFlags & UI_IMAGE_PANEL_ALIGN_RIGHT) != 0) {
        drawX = drawX + slackWidth;
      }
      if ((control->panelFlags & UI_IMAGE_PANEL_ALIGN_BOTTOM) != 0) {
        drawY = drawY + slackHeight;
      }
      if ((control->panelFlags & UI_IMAGE_PANEL_CENTER_X) != 0) {
        drawX = drawX + (slackWidth >> 1);
      }
      if ((control->panelFlags & UI_IMAGE_PANEL_CENTER_Y) != 0) {
        drawY = drawY + (slackHeight >> 1);
      }
      framebufferUnavailable = g_GraphicsFramebufferBeginAccess();
      if (!framebufferUnavailable) {
        if ((control->panelFlags & UI_IMAGE_PANEL_DROP_SHADOW) != 0) {
          g_GraphicsTextureSourceBlitModulatedSourceAlpha
                    (clippedBottom,clippedRight,clippedTop,clippedLeft,control->shadowOffsetY + drawY,
                     control->shadowOffsetX + drawX,TEXT_SHADOW_COLOR_ARGB,control->subresource,
                     control->textureSource,g_FramebufferAccess);
        }
        if ((control->panelFlags & UI_IMAGE_PANEL_STRETCH) == 0) {
          g_GraphicsTextureSourceBlitSourceAlpha
                    (clippedBottom,clippedRight,clippedTop,clippedLeft,drawY,drawX,control->subresource,
                     control->textureSource,g_FramebufferAccess);
        }
        else {
          g_GraphicsTextureSourceStretchDirectColorBilinear
                    ((control->base).layoutHeight,(control->base).layoutWidth,(control->base).top,(control->base).left,
                     control->subresource,control->textureSource,
                     g_FramebufferAccess);
        }
        g_GraphicsFramebufferEndAccess();
      }
      UiContainer_DrawIntersectingChildren(clipBottom,clipRight,clipTop,clipLeft,&control->base);
    }
  }
}

/* Hit test of an image panel (hitTest slot of g_UiImagePanelControlVtable and g_UiArmyMetricsPanelVtable):
   the point must lie on an opaque pixel of the aligned texture (unless UI_IMAGE_PANEL_HIT_WHOLE_BOX), then
   the children are tested. UI_NODE_NONE for UI_IMAGE_PANEL_NEVER_HIT or a miss.
*/
UiNodeBase * UiImagePanelControl_HitTestAlignedTextureAndChildren(int pointerY,int pointerX,
                                                                  UiImagePanelControl *control)

{
  bool childrenAlreadyRetried;
  bool skipTextureTest;
  UiNodeBase *hitNode;
  int slackWidth;
  int drawX;
  int slackHeight;
  int drawY;
  bool opaqueHit;
  GraphicsTextureLogicalSize textureSize;
  
  hitNode = UI_NODE_NONE;
  childrenAlreadyRetried = false;
  if ((control->panelFlags & UI_IMAGE_PANEL_NEVER_HIT) != 0) {
    return UI_NODE_NONE;
  }
  /* The first pass skips the opaque-texture test when children may be hit outside the bounds;
     a retry (the children hit test returned the panel itself) always runs it. */
  skipTextureTest = Any((control->base).nodeFlags & UI_NODE_ALLOW_CHILD_HIT_TEST_OUTSIDE_BOUNDS);
  do {
    if ((!skipTextureTest) && ((control->panelFlags & UI_IMAGE_PANEL_HIT_WHOLE_BOX) == 0)) {
      drawX = (control->base).left;
      drawY = (control->base).top;
      if (control->textureSource == nullptr) {
        return hitNode;
      }
      textureSize = g_GraphicsTextureSourceGetLogicalSize
                        (control->subresource,control->textureSource);
      slackWidth = (control->base).layoutWidth - textureSize.logicalWidthPixels;
      slackHeight = (control->base).layoutHeight - textureSize.logicalHeightPixels;
      if ((control->panelFlags & UI_IMAGE_PANEL_ALIGN_RIGHT) != 0) {
        drawX = drawX + slackWidth;
      }
      if ((control->panelFlags & UI_IMAGE_PANEL_ALIGN_BOTTOM) != 0) {
        drawY = drawY + slackHeight;
      }
      if ((control->panelFlags & UI_IMAGE_PANEL_CENTER_X) != 0) {
        drawX = drawX + (slackWidth >> 1);
      }
      if ((control->panelFlags & UI_IMAGE_PANEL_CENTER_Y) != 0) {
        drawY = drawY + (slackHeight >> 1);
      }
      opaqueHit = g_GraphicsTextureSourceTestOpaquePixel
                        (pointerY,pointerX,drawY,drawX,control->subresource,
                         control->textureSource);
      if (!opaqueHit) {
        return UI_NODE_NONE;
      }
    }
    skipTextureTest = false;
    hitNode = UiContainer_HitTestChildren(pointerY,pointerX,&control->base);
    if (childrenAlreadyRetried) {
      return hitNode;
    }
    childrenAlreadyRetried = true;
  } while (hitNode == &control->base);
  return hitNode;
}

/* Draws one row of fill panel tiles at tileTop, starting at the panel's left edge: a single tile, or with
   UI_FILL_PANEL_TILE_X tiles every tileWidth pixels while they start left of clipRight. Each tile is drawn
   over its drop shadow when UI_FILL_PANEL_DROP_SHADOW is set. */
static void UiFillPanelControl_DrawTileRow
          (UiPixelCoordinate clipBottom,UiPixelCoordinate clipRight,UiPixelCoordinate clipTop,
          UiPixelCoordinate clipLeft,int32_t tileTop,uint32_t tileWidth,UiFillPanelControl *control)

{
  int tileLeft;

  tileLeft = (control->base).left;
  do {
    if ((control->fillFlags & UI_FILL_PANEL_DROP_SHADOW) != 0) {
      g_GraphicsTextureSourceBlitModulatedSourceAlpha
                (clipBottom,clipRight,clipTop,clipLeft,
                 control->shadowOffsetY + tileTop,
                 control->shadowOffsetX + tileLeft,TEXT_SHADOW_COLOR_ARGB,control->subresourceOrFillArgb,
                 control->textureSource,g_FramebufferAccess);
    }
    g_GraphicsTextureSourceBlitSourceAlpha
              (clipBottom,clipRight,clipTop,clipLeft,tileTop,tileLeft,control->subresourceOrFillArgb,
               control->textureSource,g_FramebufferAccess);
    if ((control->fillFlags & UI_FILL_PANEL_TILE_X) == 0) {
      return;
    }
    tileLeft = tileLeft + tileWidth;
  } while (tileLeft < clipRight);
}

/* Draws a fill panel (drawClipped slot of g_UiFillPanelControlVtable): without a texture, a rectangle in the
   ARGB colour subresourceOrFillArgb; with one, its subresource once or tiled across and/or down the box
   (fillFlags), each tile optionally over a drop shadow. Then the children.
*/
void UiFillPanelControl_DrawColorOrTiledTextureAndChildren
          (UiPixelCoordinate clipBottom,UiPixelCoordinate clipRight,UiPixelCoordinate clipTop,
          UiPixelCoordinate clipLeft,UiFillPanelControl *control)

{
  int controlRight;
  int controlBottom;
  uint32_t tileWidth;
  int controlLeft;
  uint32_t tileHeight;
  int32_t tileTop;
  bool framebufferUnavailable;
  GraphicsTextureLogicalSize tileSize;

  controlRight = (control->base).right;
  controlBottom = (control->base).bottom;
  controlLeft = (control->base).left;
  tileTop = (control->base).top;
  if (control->textureSource == nullptr) {
    framebufferUnavailable = g_GraphicsFramebufferBeginAccess();
    if (!framebufferUnavailable) {
      g_GraphicsFramebufferFillRectArgb
                (clipBottom,clipRight,clipTop,clipLeft,controlBottom,controlRight,tileTop,controlLeft,
                 control->subresourceOrFillArgb,
                 g_FramebufferAccess);
      g_GraphicsFramebufferEndAccess();
    }
  }
  else {
    if (controlRight < clipRight) {
      clipRight = controlRight;
    }
    if (controlBottom < clipBottom) {
      clipBottom = controlBottom;
    }
    tileSize = g_GraphicsTextureSourceGetLogicalSize
                      (control->subresourceOrFillArgb,control->textureSource);
    tileHeight = tileSize.logicalHeightPixels;
    tileWidth = tileSize.logicalWidthPixels;
    framebufferUnavailable = g_GraphicsFramebufferBeginAccess();
    if (!framebufferUnavailable) {
      /* Tile rows left to right (UI_FILL_PANEL_TILE_X), then top to bottom (UI_FILL_PANEL_TILE_Y). */
      do {
        UiFillPanelControl_DrawTileRow(clipBottom,clipRight,clipTop,clipLeft,tileTop,tileWidth,control);
        if ((control->fillFlags & UI_FILL_PANEL_TILE_Y) == 0) break;
        tileTop = tileTop + tileHeight;
      } while (tileTop < clipBottom);
      g_GraphicsFramebufferEndAccess();
    }
  }
  UiContainer_DrawIntersectingChildren(clipBottom,clipRight,clipTop,clipLeft,&control->base);
}

/* Draws a nine-slice panel (drawClipped slot of g_UiNineSlicePanelControlVtable) from eight consecutive
   frames starting at firstFrameSubresource: 0 top-left and 1 top-right corner, 2 top edge, 3 left edge,
   4 right edge, 5 bottom-left and 6 bottom-right corner, 7 bottom edge (edges tiled), and centerSubresource
   tiled over the interior; then the children. GRAPHICS_TILED_BLIT_ONE_TILE keeps an edge one tile thick.
*/
void UiNineSlicePanelControl_DrawTextureFrameAndChildren
          (UiPixelCoordinate clipBottom,UiPixelCoordinate clipRight,UiPixelCoordinate clipTop,
          UiPixelCoordinate clipLeft,UiNineSlicePanelControl *control)

{
  GraphicsSubresourceIndex baseTextureFrame;
  uint32_t slice4Width;
  int leftEdgeX;
  int topEdgeY;
  int bottomEdgeY;
  int rightEdgeX;
  bool framebufferUnavailable;
  GraphicsTextureLogicalSize slice0Size;
  GraphicsTextureLogicalSize slice1Size;
  GraphicsTextureLogicalSize slice2Size;
  GraphicsTextureLogicalSize slice3Size;
  GraphicsTextureLogicalSize slice4Or5Size;
  GraphicsTextureLogicalSize slice6Size;
  GraphicsTextureLogicalSize slice7Size;
  
  if (!Any((control->base).nodeFlags & UI_NODE_SUPPRESSED)) {
    framebufferUnavailable = g_GraphicsFramebufferBeginAccess();
    if (!framebufferUnavailable) {
      baseTextureFrame = control->firstFrameSubresource;
      slice0Size = g_GraphicsTextureSourceGetLogicalSize
                        (baseTextureFrame,control->textureSource);
      slice1Size = g_GraphicsTextureSourceGetLogicalSize
                        (baseTextureFrame + 1,
                         control->textureSource);
      slice2Size = g_GraphicsTextureSourceGetLogicalSize
                        (baseTextureFrame + 2,
                         control->textureSource);
      slice3Size = g_GraphicsTextureSourceGetLogicalSize
                         (baseTextureFrame + 3,
                          control->textureSource);
      slice4Or5Size = g_GraphicsTextureSourceGetLogicalSize
                         (baseTextureFrame + 4,
                          control->textureSource);
      slice4Width = slice4Or5Size.logicalWidthPixels;
      slice4Or5Size = g_GraphicsTextureSourceGetLogicalSize
                         (baseTextureFrame + 5,
                          control->textureSource);
      slice6Size = g_GraphicsTextureSourceGetLogicalSize
                         (baseTextureFrame + 6,
                          control->textureSource);
      slice7Size = g_GraphicsTextureSourceGetLogicalSize
                         (baseTextureFrame + 7,
                          control->textureSource);
      leftEdgeX = (control->base).left;
      topEdgeY = (control->base).top;
      bottomEdgeY = (control->base).bottom;
      rightEdgeX = (control->base).right - slice1Size.logicalWidthPixels;
      g_GraphicsTextureSourceBlitSourceAlpha
                (clipBottom,clipRight,clipTop,clipLeft,topEdgeY,leftEdgeX,baseTextureFrame,
                 control->textureSource,g_FramebufferAccess);
      g_GraphicsTextureSourceBlitSourceAlpha
                (clipBottom,clipRight,clipTop,clipLeft,topEdgeY,rightEdgeX,
                 baseTextureFrame + 1,
                 control->textureSource,g_FramebufferAccess);
      leftEdgeX = leftEdgeX + slice0Size.logicalWidthPixels;
      g_GraphicsTextureSourceBlitTiledSourceAlpha
                (clipBottom,clipRight,clipTop,clipLeft,GRAPHICS_TILED_BLIT_ONE_TILE,rightEdgeX,topEdgeY,leftEdgeX,
                 baseTextureFrame + 2,
                 control->textureSource,g_FramebufferAccess);
      leftEdgeX = leftEdgeX - slice0Size.logicalWidthPixels;
      topEdgeY = topEdgeY + slice0Size.logicalHeightPixels;
      bottomEdgeY = bottomEdgeY - slice4Or5Size.logicalHeightPixels;
      g_GraphicsTextureSourceBlitTiledSourceAlpha
                (clipBottom,clipRight,clipTop,clipLeft,bottomEdgeY,GRAPHICS_TILED_BLIT_ONE_TILE,topEdgeY,leftEdgeX,
                 baseTextureFrame + 3,
                 control->textureSource,g_FramebufferAccess);
      g_GraphicsTextureSourceBlitSourceAlpha
                (clipBottom,clipRight,clipTop,clipLeft,bottomEdgeY,leftEdgeX,
                 baseTextureFrame + 5,
                 control->textureSource,g_FramebufferAccess);
      rightEdgeX = (rightEdgeX + slice1Size.logicalWidthPixels) - slice4Width;
      topEdgeY = (topEdgeY - slice0Size.logicalHeightPixels) + slice1Size.logicalHeightPixels;
      bottomEdgeY = (bottomEdgeY + slice4Or5Size.logicalHeightPixels) - slice6Size.logicalHeightPixels;
      g_GraphicsTextureSourceBlitTiledSourceAlpha
                (clipBottom,clipRight,clipTop,clipLeft,bottomEdgeY,GRAPHICS_TILED_BLIT_ONE_TILE,topEdgeY,rightEdgeX,
                 baseTextureFrame + 4,control->textureSource,
                 g_FramebufferAccess);
      rightEdgeX = (rightEdgeX + slice4Width) - slice6Size.logicalWidthPixels;
      g_GraphicsTextureSourceBlitSourceAlpha
                (clipBottom,clipRight,clipTop,clipLeft,bottomEdgeY,rightEdgeX,
                 baseTextureFrame + 6,
                 control->textureSource,g_FramebufferAccess);
      leftEdgeX = leftEdgeX + slice4Or5Size.logicalWidthPixels;
      bottomEdgeY = (bottomEdgeY + slice6Size.logicalHeightPixels) - slice7Size.logicalHeightPixels;
      g_GraphicsTextureSourceBlitTiledSourceAlpha
                (clipBottom,clipRight,clipTop,clipLeft,GRAPHICS_TILED_BLIT_ONE_TILE,rightEdgeX,bottomEdgeY,leftEdgeX,
                 baseTextureFrame + 7,
                 control->textureSource,g_FramebufferAccess);
      g_GraphicsTextureSourceBlitTiledSourceAlpha
                (clipBottom,clipRight,clipTop,clipLeft,bottomEdgeY,
                 (rightEdgeX + slice6Size.logicalWidthPixels) - slice4Width,
                 (topEdgeY - slice1Size.logicalHeightPixels) + slice2Size.logicalHeightPixels,
                 (leftEdgeX - slice4Or5Size.logicalWidthPixels) + slice3Size.logicalWidthPixels,
                 control->centerSubresource,control->textureSource,
                 g_FramebufferAccess);
      g_GraphicsFramebufferEndAccess();
    }
  }
  UiContainer_DrawIntersectingChildren(clipBottom,clipRight,clipTop,clipLeft,&control->base);
}

/* Relocation of a loaded resource gauge (relocate slot of g_UiFormattedContainerVtable): when it has a
   tooltip, points payloads 0, 1 (and 2 with UI_GAUGE_HAS_MARKER) of the tooltip text (the resource id stored
   just before the node) at the gauge's own value strings, so the tooltip always shows the current numbers;
   the value strings start out empty. Then the children are relocated.
*/
void UiFormattedContainer_RelocateWithPatchedTextPayloads
          (UiSerializedRelocationDelta relocationDelta,UiFormattedContainer *control)

{
  uint16_t *stream;
  uint16_t *resolvedText;
  
  if (Any((control->base).nodeFlags & UI_NODE_TOOLTIP_ELIGIBLE)) {
    /* the template stores the tooltip's text id in the dword just before the node */
    resolvedText = TextResource_Resolve(reinterpret_cast<TextResourceId *>(control)[-1]);
    stream = resolvedText;
    RichTextCommandStream_PatchPayloadBySelector(0,control->currentValueTextUtf16,stream);
    RichTextCommandStream_PatchPayloadBySelector(1,control->limitValueTextUtf16,stream);
    if ((control->gaugeFlags & UI_GAUGE_HAS_MARKER) != 0) {
      RichTextCommandStream_PatchPayloadBySelector(2,UiNode_As<UiFormattedContainerWithMarker>(control)->markerValueTextUtf16,
                                                   stream);
      Thandor_StoreU32(UiNode_As<UiFormattedContainerWithMarker>(control)->markerValueTextUtf16,0);
    }
  }
  Thandor_StoreU32(control->currentValueTextUtf16,0); /* the first two code units */
  Thandor_StoreU32(control->limitValueTextUtf16,0);
  UiContainer_RelocateChildren(relocationDelta,&control->base);
}

/* Fill colour variant of a gauge (frame offset 3..18, three frames each) by the fill percentage: rising from
   80% on, or for the two-sided scale also rising the further it falls below 40%. */
static int UiFormattedContainer_FillVariantOffset(uint32_t fillPercent,bool twoSidedScale)

{
  if (!twoSidedScale) {
    if (fillPercent <= 79) {
      return 3;
    }
    if (fillPercent <= 83) {
      return 6;
    }
    if (fillPercent <= 87) {
      return 9;
    }
    if (fillPercent <= 91) {
      return 12;
    }
    if (fillPercent <= 95) {
      return 15;
    }
    return 18;
  }
  if (fillPercent <= 7) {
    return 18;
  }
  if (fillPercent <= 15) {
    return 15;
  }
  if (fillPercent <= 23) {
    return 12;
  }
  if (fillPercent <= 31) {
    return 9;
  }
  if (fillPercent <= 39) {
    return 6;
  }
  if (fillPercent <= 85) {
    return 3;
  }
  if (fillPercent <= 87) {
    return 6;
  }
  if (fillPercent <= 89) {
    return 9;
  }
  if (fillPercent <= 91) {
    return 12;
  }
  if (fillPercent <= 93) {
    return 15;
  }
  return 18;
}

/* Draws a resource gauge (drawClipped slot of g_UiFormattedContainerVtable): the empty bar (frames
   firstFrameSubresource + 0/1/2: left cap, tiled middle, right cap), the fill up to currentValue in one of six
   colour variants (+3, +6, ... +18, chosen by the fill percentage of the limit or marker), and marker frame
   +21 at limitValue and, with UI_GAUGE_HAS_MARKER, at markerValue. The scale starts at
   initialScaleRange and grows by factors of 4 until every value fits. Afterwards (also when nothing is
   drawn) the value strings for the tooltip are refreshed.
*/
void UiFormattedContainer_DrawClipped
          (int clipBottom,int clipRight,int clipTop,int clipLeft,UiFormattedContainer *control)

{
  int currentValue;
  int limitValue;
  uint32_t fillPercent;
  int scaleRange;
  int scaleLimit;
  uint32_t textureFrame;
  int barEndX;
  int barSpan;
  int limitMarkerX;
  int fillEndX;
  int barStartX;
  GraphicsTextureLogicalSize frameSize;
  int markerOffsetX = 0; /* set and used only with UI_GAUGE_HAS_MARKER */

  if (control->limitValue != 0 && !g_GraphicsFramebufferBeginAccess()) {
    if (clipLeft < (control->base).left) {
      clipLeft = (control->base).left;
    }
    if (clipTop < (control->base).top) {
      clipTop = (control->base).top;
    }
    if ((control->base).right < clipRight) {
      clipRight = (control->base).right;
    }
    if ((control->base).bottom < clipBottom) {
      clipBottom = (control->base).bottom;
    }
    textureFrame = control->firstFrameSubresource;
    barStartX = (control->base).left;
    barEndX = (control->base).right;
    g_GraphicsTextureSourceBlitSourceAlpha
              (clipBottom,clipRight,clipTop,clipLeft,(control->base).top,barStartX,textureFrame,
               control->textureSource,g_FramebufferAccess);
    frameSize = g_GraphicsTextureSourceGetLogicalSize
                       (textureFrame,control->textureSource);
    barStartX = barStartX + frameSize.logicalWidthPixels;
    frameSize = g_GraphicsTextureSourceGetLogicalSize
                       (textureFrame + UI_GAUGE_FRAME_END_CAP,control->textureSource);
    barEndX = barEndX - frameSize.logicalWidthPixels;
    g_GraphicsTextureSourceBlitSourceAlpha
              (clipBottom,clipRight,clipTop,clipLeft,(control->base).top,barEndX,
               textureFrame + UI_GAUGE_FRAME_END_CAP,
               control->textureSource,g_FramebufferAccess);
    if (barStartX < barEndX) {
      GraphicsTextureSource_BlitTiledSourceAlpha
                (clipBottom,clipRight,clipTop,clipLeft,GRAPHICS_TILED_BLIT_ONE_TILE,barEndX,
                 (control->base).top,barStartX,textureFrame + UI_GAUGE_FRAME_TRACK,
                 control->textureSource,g_FramebufferAccess);
      /* the marker fields, read only with UI_GAUGE_HAS_MARKER set */
      const UiFormattedContainerWithMarker *marked = UiNode_As<UiFormattedContainerWithMarker>(control);
      currentValue = control->currentValue;
      scaleRange = control->initialScaleRange;
      barSpan = barEndX - barStartX;
      /* Grow the scale by factors of 4 until it covers the value, the limit and the marker. The original
         loops forever when the initial range is <= 0 or the growth overflows; bounded here because of
         that: a range <= 0 starts at 1 and the growth stops before it would overflow. */
      if (scaleRange <= 0) {
        static int s_loggedScaleRange;
        if (s_loggedScaleRange == 0) {
          s_loggedScaleRange = 1;
          Thandor_Log("gauge %p: initial scale range %d, started at 1",static_cast<void *>(control),scaleRange);
        }
        scaleRange = 1;
      }
      while (((scaleRange < currentValue) || (scaleRange < control->limitValue) ||
              (((control->gaugeFlags & UI_GAUGE_HAS_MARKER) != 0) &&
               (scaleRange < marked->markerValue))) &&
             (scaleRange <= (INT32_MAX >> 2))) {
        scaleRange = scaleRange << 2;
      }
      if ((control->gaugeFlags & UI_GAUGE_HAS_MARKER) != 0) {
        markerOffsetX =
             (int)(((int64_t)marked->markerValue * (int64_t)barSpan) /
                   (int64_t)scaleRange);
      }
      limitValue = control->limitValue;
      scaleLimit = control->limitValue;
      if (((control->gaugeFlags & UI_GAUGE_HAS_MARKER) != 0) &&
          (marked->markerValue < scaleLimit)) {
        scaleLimit = marked->markerValue;
      }
      if (scaleLimit == 0) {
        /* The original divides by zero here (marker value 0); bounded here because that crashes. */
        static int s_loggedScaleLimit;
        if (s_loggedScaleLimit == 0) {
          s_loggedScaleLimit = 1;
          Thandor_Log("gauge %p: marker value 0, fill percentage taken against 1",static_cast<void *>(control));
        }
        scaleLimit = 1;
      }
      fillPercent = (uint32_t)(((int64_t)currentValue * 100) / (int64_t)scaleLimit);
      textureFrame = UiFormattedContainer_FillVariantOffset
                       (fillPercent,(control->gaugeFlags & UI_GAUGE_TWO_SIDED_SCALE) != 0) +
                     control->firstFrameSubresource;
      if (control->currentValue != 0) {
        frameSize = g_GraphicsTextureSourceGetLogicalSize
                           (textureFrame,control->textureSource);
        barStartX = barStartX - frameSize.logicalWidthPixels;
        g_GraphicsTextureSourceBlitSourceAlpha
                  (clipBottom,clipRight,clipTop,clipLeft,(control->base).top,barStartX,textureFrame,
                   control->textureSource,g_FramebufferAccess);
        barStartX = barStartX + frameSize.logicalWidthPixels;
        fillEndX = (int)(((int64_t)currentValue * (int64_t)barSpan) / (int64_t)scaleRange) + barStartX;
        GraphicsTextureSource_BlitTiledSourceAlpha
                  (clipBottom,clipRight,clipTop,clipLeft,GRAPHICS_TILED_BLIT_ONE_TILE,fillEndX,
                   (control->base).top,barStartX,
                   textureFrame + UI_GAUGE_FRAME_TRACK,control->textureSource,g_FramebufferAccess);
        g_GraphicsTextureSourceBlitSourceAlpha
                  (clipBottom,clipRight,clipTop,clipLeft,(control->base).top,fillEndX,
                   textureFrame + UI_GAUGE_FRAME_END_CAP,
                   control->textureSource,g_FramebufferAccess);
      }
      limitMarkerX = barStartX + (int)(((int64_t)limitValue * (int64_t)barSpan) / (int64_t)scaleRange);
      textureFrame = control->firstFrameSubresource + UI_GAUGE_FRAME_MARKER;
      frameSize = g_GraphicsTextureSourceGetLogicalSize
                         (textureFrame,control->textureSource);
      g_GraphicsTextureSourceBlitSourceAlpha
                (clipBottom,clipRight,clipTop,clipLeft,(control->base).top,
                 limitMarkerX - ((int)frameSize.logicalWidthPixels >> 1),textureFrame,
                 control->textureSource,g_FramebufferAccess);
      if ((control->gaugeFlags & UI_GAUGE_HAS_MARKER) != 0) {
        textureFrame = control->firstFrameSubresource + UI_GAUGE_FRAME_MARKER;
        frameSize = g_GraphicsTextureSourceGetLogicalSize
                           (textureFrame,control->textureSource);
        g_GraphicsTextureSourceBlitSourceAlpha
                  (clipBottom,clipRight,clipTop,clipLeft,(control->base).top,
                   (barStartX + markerOffsetX) - ((int)frameSize.logicalWidthPixels >> 1),textureFrame,
                   control->textureSource,g_FramebufferAccess);
      }
    }
    g_GraphicsFramebufferEndAccess();
  }
  /* the tooltip value strings, also when nothing was drawn */
  g_WideNumberFormatUtf16
            (WIDE_FORMAT_WRITE_TERMINATOR,0,10,1,control->currentValue,
             control->currentValueTextUtf16);
  g_WideNumberFormatUtf16
            (WIDE_FORMAT_WRITE_TERMINATOR,0,10,1,control->limitValue,
             control->limitValueTextUtf16);
  if ((control->gaugeFlags & UI_GAUGE_HAS_MARKER) != 0) {
    g_WideNumberFormatUtf16
              (WIDE_FORMAT_WRITE_TERMINATOR,0,10,1,UiNode_As<UiFormattedContainerWithMarker>(control)->markerValue,
               UiNode_As<UiFormattedContainerWithMarker>(control)->markerValueTextUtf16);
  }
}

/* Draws the army metrics panel (drawClipped slot of g_UiArmyMetricsPanelVtable): the aligned panel texture
   like an image panel, then, when an entity is attached, its runtime metrics via
   SelectionPanel_RenderArmyRuntimeMetrics with the info-panel graphics temporarily installed as the
   selection-panel graphics; then the children.
*/
void UiArmyMetricsPanel_DrawTextureMetricsAndChildren
          (UiPixelCoordinate clipBottom,UiPixelCoordinate clipRight,UiPixelCoordinate clipTop,
          UiPixelCoordinate clipLeft,UiArmyMetricsPanel *control)

{
  GraphicsTextureSourceAsset *savedTextureSource;
  void *savedPanelData;
  int clippedRight;
  int slackWidth;
  int clippedLeft;
  int drawX;
  int clippedBottom;
  int slackHeight;
  int clippedTop;
  int drawY;
  bool framebufferUnavailable;
  GraphicsTextureLogicalSize textureSize;
  
  if (!Any((control->base).base.nodeFlags & UI_NODE_SUPPRESSED)) {
    clippedLeft = (control->base).base.left;
    if ((control->base).base.left < clipLeft) {
      clippedLeft = clipLeft;
    }
    clippedTop = (control->base).base.top;
    if ((control->base).base.top < clipTop) {
      clippedTop = clipTop;
    }
    clippedRight = (control->base).base.right;
    if (clipRight < (control->base).base.right) {
      clippedRight = clipRight;
    }
    clippedBottom = (control->base).base.bottom;
    if (clipBottom < (control->base).base.bottom) {
      clippedBottom = clipBottom;
    }
    drawX = (control->base).base.left;
    drawY = (control->base).base.top;
    if ((control->base).textureSource != nullptr) {
      textureSize = g_GraphicsTextureSourceGetLogicalSize
                        ((control->base).subresource,(control->base).textureSource);
      slackWidth = (control->base).base.layoutWidth - textureSize.logicalWidthPixels;
      slackHeight = (control->base).base.layoutHeight - textureSize.logicalHeightPixels;
      if (((control->base).panelFlags & UI_IMAGE_PANEL_ALIGN_RIGHT) != 0) {
        drawX = drawX + slackWidth;
      }
      if (((control->base).panelFlags & UI_IMAGE_PANEL_ALIGN_BOTTOM) != 0) {
        drawY = drawY + slackHeight;
      }
      if (((control->base).panelFlags & UI_IMAGE_PANEL_CENTER_X) != 0) {
        drawX = drawX + (slackWidth >> 1);
      }
      if (((control->base).panelFlags & UI_IMAGE_PANEL_CENTER_Y) != 0) {
        drawY = drawY + (slackHeight >> 1);
      }
      framebufferUnavailable = g_GraphicsFramebufferBeginAccess();
      if (!framebufferUnavailable) {
        g_GraphicsTextureSourceBlitSourceAlpha
                  (clippedBottom,clippedRight,clippedTop,clippedLeft,drawY,drawX,(control->base).subresource,
                   (control->base).textureSource,g_FramebufferAccess);
        g_GraphicsFramebufferEndAccess();
        savedPanelData = g_SelectionPanelData;
        savedTextureSource = g_SelectionPanelTextureSource;
        /* the original writes both straight back here */
        g_SelectionPanelTextureSource = savedTextureSource;
        g_SelectionPanelData = savedPanelData;
        if (control->entity != nullptr) {
          g_SelectionPanelTextureSource = g_InfoPanelTextureSource;
          g_SelectionPanelData = g_InfoPanelData;
          SelectionPanel_RenderArmyRuntimeMetrics
                    (clipBottom,clipRight,clipTop,clipLeft,(control->base).base.bottom,(control->base).base.right,
                     (control->base).base.top,(control->base).base.left,control->entity);
          g_SelectionPanelTextureSource = savedTextureSource;
          g_SelectionPanelData = savedPanelData;
        }
      }
      UiContainer_DrawIntersectingChildren(clipBottom,clipRight,clipTop,clipLeft,&control->base.base);
    }
  }
}

/* Draws a software texture preview (drawClipped slot of g_UiSoftwareTexturePreviewControlVtable): the
   outgoing and incoming subresources blended bilinearly and scaled over the layout box (a crossfade driven by
   the control's blend buffers), then the children.
*/
void UiSoftwareTexturePreviewControl_DrawScaledTextureAndChildren
          (UiPixelCoordinate clipBottom,UiPixelCoordinate clipRight,UiPixelCoordinate clipTop,
          UiPixelCoordinate clipLeft,UiSoftwareTexturePreviewControl *control)

{
  bool framebufferUnavailable;
  
  if (!Any((control->base).nodeFlags & UI_NODE_SUPPRESSED) &&
     (control->textureSource != nullptr)) {
    framebufferUnavailable = g_GraphicsFramebufferBeginAccess();
    if (!framebufferUnavailable) {
      g_GraphicsGreyScaleImage
                ((control->base).layoutHeight,(control->base).layoutWidth,(control->base).top,(control->base).left,
                 control->blendedSourcePixels,control->blendFactorPixels,
                 control->incomingSubresource,
                 control->outgoingSubresource,control->textureSource,
                 g_FramebufferAccess);
      g_GraphicsFramebufferEndAccess();
    }
  }
  UiContainer_DrawIntersectingChildren(clipBottom,clipRight,clipTop,clipLeft,&control->base);
}

/* Primary button press on a software texture preview (nonRightPress slot of
   g_UiSoftwareTexturePreviewControlVtable): queues the control's action.
*/
void UiSoftwareTexturePreviewControl_EnqueueActionOnPrimaryPress
          (UiPointerWheelDelta wheelDelta,UiPixelCoordinate pointerY,UiPixelCoordinate pointerX,
          UiSoftwareTexturePreviewControl *control)

{
  UiActionQueue_Enqueue(control->actionId,control);
}

/* Secondary button press on a software texture preview (rightPress slot of
   g_UiSoftwareTexturePreviewControlVtable): queues the control's action, like the primary button.
*/
void UiSoftwareTexturePreviewControl_EnqueueActionOnSecondaryPress
          (UiPointerWheelDelta wheelDelta,UiPixelCoordinate pointerY,UiPixelCoordinate pointerX,
          UiSoftwareTexturePreviewControl *control)

{
  UiActionQueue_Enqueue(control->actionId,control);
}

/* Keyboard handler of a software texture preview (keyboardEvent slot of
   g_UiSoftwareTexturePreviewControlVtable): Tab moves the focus on, any other key queues the control's action.
   Always consumed (returns false).
*/
bool UiSoftwareTexturePreviewControl_HandleKeyboardActivation
          (UiKeyboardStateMask keyboardStateMask,UiKeyboardEventCode keyCode,UiSoftwareTexturePreviewControl *control)

{
  if (keyCode != KEYBOARD_KEY_CODE_TAB) {
    UiActionQueue_Enqueue(control->actionId,control);
    return false;
  }
  UiKeyboardFocus_MoveNext();
  return false;
}

UiNodeVtable g_UiImagePanelControlVtable = {
        .relocate = UI_SLOT(UiContainer_RelocateChildren),
        .method04 = UI_SLOT(UiNode_DefaultMethod04_NoOp),
        .drawClipped = UI_SLOT(UiImagePanelControl_DrawAlignedTextureAndChildren),
        .layout = UI_SLOT(UiContainer_LayoutChildren),
        .nonRightPress = UI_SLOT(UiNode_DefaultNonRightPress),
        .nonRightRelease = UI_SLOT(UiNode_DefaultNonRightRelease),
        .rightPress = UI_SLOT(UiNode_ForwardRightPressToParent),
        .rightRelease = UI_SLOT(UiNode_DefaultRightRelease),
        .nonRightDrag = UI_SLOT(UiNode_DefaultNonRightDrag),
        .rightDrag = UI_SLOT(UiNode_DefaultRightDrag),
        .pointerMove = UI_SLOT(UiNode_DefaultPointerMove),
        .hitTest = UI_SLOT(UiImagePanelControl_HitTestAlignedTextureAndChildren),
        .keyboardEvent = UI_SLOT(UiNode_DefaultKeyboardEventMoveFocusNext),
        .applyFlags = UI_SLOT(UiNode_ApplyFlagsRecursive),
        .suppressActionId = UI_SLOT(UiContainer_SuppressActionId),
        .unsuppressActionId = UI_SLOT(UiContainer_UnsuppressActionId),
        .tick = UI_SLOT(UiNode_DefaultTick),
        .pointerWheel = UI_SLOT(UiNode_ForwardPointerWheelToParent)};

UiNodeVtable g_UiNineSlicePanelControlVtable = {
        .relocate = UI_SLOT(UiContainer_RelocateChildren),
        .method04 = UI_SLOT(UiNode_DefaultMethod04_NoOp),
        .drawClipped = UI_SLOT(UiNineSlicePanelControl_DrawTextureFrameAndChildren),
        .layout = UI_SLOT(UiContainer_LayoutChildren),
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

UiNodeVtable g_UiFormattedContainerVtable = {
        .relocate = UI_SLOT(UiFormattedContainer_RelocateWithPatchedTextPayloads),
        .method04 = UI_SLOT(UiNode_DefaultMethod04_NoOp),
        .drawClipped = UI_SLOT(UiFormattedContainer_DrawClipped),
        .layout = UI_SLOT(UiContainer_LayoutChildren),
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
        .pointerWheel = UI_SLOT(UiNode_ForwardPointerWheelToParent),
};

UiNodeVtable g_UiArmyMetricsPanelVtable = {
        .relocate = UI_SLOT(UiContainer_RelocateChildren),
        .method04 = UI_SLOT(UiNode_DefaultMethod04_NoOp),
        .drawClipped = UI_SLOT(UiArmyMetricsPanel_DrawTextureMetricsAndChildren),
        .layout = UI_SLOT(UiContainer_LayoutChildren),
        .nonRightPress = UI_SLOT(UiNode_DefaultNonRightPress),
        .nonRightRelease = UI_SLOT(UiNode_DefaultNonRightRelease),
        .rightPress = UI_SLOT(UiNode_ForwardRightPressToParent),
        .rightRelease = UI_SLOT(UiNode_DefaultRightRelease),
        .nonRightDrag = UI_SLOT(UiNode_DefaultNonRightDrag),
        .rightDrag = UI_SLOT(UiNode_DefaultRightDrag),
        .pointerMove = UI_SLOT(UiNode_DefaultPointerMove),
        .hitTest = UI_SLOT(UiImagePanelControl_HitTestAlignedTextureAndChildren),
        .keyboardEvent = UI_SLOT(UiNode_DefaultKeyboardEventMoveFocusNext),
        .applyFlags = UI_SLOT(UiNode_ApplyFlagsRecursive),
        .suppressActionId = UI_SLOT(UiContainer_SuppressActionId),
        .unsuppressActionId = UI_SLOT(UiContainer_UnsuppressActionId),
        .tick = UI_SLOT(UiNode_DefaultTick),
        .pointerWheel = UI_SLOT(UiNode_ForwardPointerWheelToParent),
};

UiNodeVtable g_UiSoftwareTexturePreviewControlVtable = {
    .relocate = UI_SLOT(UiContainer_RelocateChildren),
    .method04 = UI_SLOT(UiNode_DefaultMethod04_NoOp),
    .drawClipped = UI_SLOT(UiSoftwareTexturePreviewControl_DrawScaledTextureAndChildren),
    .layout = UI_SLOT(UiContainer_LayoutChildren),
    .nonRightPress = UI_SLOT(UiSoftwareTexturePreviewControl_EnqueueActionOnPrimaryPress),
    .nonRightRelease = UI_SLOT(UiNode_DefaultNonRightRelease),
    .rightPress = UI_SLOT(UiSoftwareTexturePreviewControl_EnqueueActionOnSecondaryPress),
    .rightRelease = UI_SLOT(UiNode_DefaultRightRelease),
    .nonRightDrag = UI_SLOT(UiNode_DefaultNonRightDrag),
    .rightDrag = UI_SLOT(UiNode_DefaultRightDrag),
    .pointerMove = UI_SLOT(UiNode_DefaultPointerMove),
    .hitTest = UI_SLOT(UiContainer_HitTestChildren),
    .keyboardEvent = UI_SLOT(UiSoftwareTexturePreviewControl_HandleKeyboardActivation),
    .applyFlags = UI_SLOT(UiNode_ApplyFlagsRecursive),
    .suppressActionId = UI_SLOT(UiContainer_SuppressActionId),
    .unsuppressActionId = UI_SLOT(UiContainer_UnsuppressActionId),
    .tick = UI_SLOT(UiNode_DefaultTick),
    .pointerWheel = UI_SLOT(UiNode_ForwardPointerWheelToParent),
};

/* drawClipped of g_UiPanelControlVtable: draws the panel's optional tiled background (UI_ROOT_TILED_BACKGROUND)
   and frame (UI_ROOT_FRAME); UI_ROOT_ALTERNATE_BACKGROUND switches to the second background and adds the
   second frame on top. Then draws the children. A frame is four corners and four tiled edges between them.
*/
void UiPanelControl_DrawOptionalTiledBackgroundFrameAndChildren
          (UiPixelCoordinate clipBottom,UiPixelCoordinate clipRight,UiPixelCoordinate clipTop,
          UiPixelCoordinate clipLeft,UiPanelControl *control)

{
  uint32_t cornerWidth;
  GraphicsSubresourceIndex subresource;
  uint32_t cornerHeight;
  int bottomEdgeY;
  int rightEdgeX;
  bool beginAccessFailed;
  GraphicsTextureLogicalSize cornerSize;

  if (Any(control->root.rootFlags & (UI_ROOT_TILED_BACKGROUND | UI_ROOT_FRAME))) {
    beginAccessFailed = g_GraphicsFramebufferBeginAccess();
    if (!beginAccessFailed) {
      if (Any(control->root.rootFlags & UI_ROOT_TILED_BACKGROUND)) {
        subresource = UI_WINDOW_SUBRESOURCE_WINDOW_INTERIOR;
        if (Any(control->root.rootFlags & UI_ROOT_ALTERNATE_BACKGROUND)) {
          subresource = UI_WINDOW_SUBRESOURCE_ALTERNATE_INTERIOR;
        }
        UiWindow_BlitTiledInterior
                  (clipBottom,clipRight,clipTop,clipLeft,subresource,control->root.base.layoutHeight,
                   control->root.base.layoutWidth,0,0,control);
      }
      if (Any(control->root.rootFlags & UI_ROOT_FRAME)) {
        /* the bottom-right corner gives the corner size */
        cornerSize = g_GraphicsTextureSourceGetLogicalSize
                               (UI_WINDOW_SUBRESOURCE_WINDOW_FRAME + UI_WINDOW_FRAME_BOTTOM_RIGHT,
                                g_UiWindowTextureSource);
        cornerHeight = cornerSize.logicalHeightPixels;
        cornerWidth = cornerSize.logicalWidthPixels;
        rightEdgeX = control->root.base.layoutWidth - cornerWidth;
        bottomEdgeY = control->root.base.layoutHeight - cornerHeight;
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
                   rightEdgeX + control->root.base.left,
                   UI_WINDOW_SUBRESOURCE_WINDOW_FRAME + UI_WINDOW_FRAME_BOTTOM_RIGHT,
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
      if (Any(control->root.rootFlags & UI_ROOT_ALTERNATE_BACKGROUND)) {
        cornerSize = g_GraphicsTextureSourceGetLogicalSize
                               (UI_WINDOW_SUBRESOURCE_ALTERNATE_FRAME + UI_WINDOW_FRAME_BOTTOM_RIGHT,
                                g_UiWindowTextureSource);
        cornerHeight = cornerSize.logicalHeightPixels;
        cornerWidth = cornerSize.logicalWidthPixels;
        rightEdgeX = control->root.base.layoutWidth - cornerWidth;
        bottomEdgeY = control->root.base.layoutHeight - cornerHeight;
        g_GraphicsTextureSourceBlitSourceAlpha
                  (clipBottom,clipRight,clipTop,clipLeft,control->root.base.top,control->root.base.left,
                   UI_WINDOW_SUBRESOURCE_ALTERNATE_FRAME + UI_WINDOW_FRAME_TOP_LEFT,
                   g_UiWindowTextureSource,g_FramebufferAccess);
        g_GraphicsTextureSourceBlitSourceAlpha
                  (clipBottom,clipRight,clipTop,clipLeft,control->root.base.top,rightEdgeX + control->root.base.left,
                   UI_WINDOW_SUBRESOURCE_ALTERNATE_FRAME + UI_WINDOW_FRAME_TOP_RIGHT,
                   g_UiWindowTextureSource,g_FramebufferAccess);
        g_GraphicsTextureSourceBlitSourceAlpha
                  (clipBottom,clipRight,clipTop,clipLeft,bottomEdgeY + control->root.base.top,control->root.base.left,
                   UI_WINDOW_SUBRESOURCE_ALTERNATE_FRAME + UI_WINDOW_FRAME_BOTTOM_LEFT,
                   g_UiWindowTextureSource,g_FramebufferAccess);
        g_GraphicsTextureSourceBlitSourceAlpha
                  (clipBottom,clipRight,clipTop,clipLeft,bottomEdgeY + control->root.base.top,
                   rightEdgeX + control->root.base.left,
                   UI_WINDOW_SUBRESOURCE_ALTERNATE_FRAME + UI_WINDOW_FRAME_BOTTOM_RIGHT,
                   g_UiWindowTextureSource,g_FramebufferAccess);
        UiWindow_BlitTiledHorizontalEdge
                  (clipBottom,clipRight,clipTop,clipLeft,UI_WINDOW_SUBRESOURCE_ALTERNATE_FRAME + UI_WINDOW_FRAME_TOP,
                   rightEdgeX,0,cornerWidth,control);
        UiWindow_BlitTiledVerticalEdge
                  (clipBottom,clipRight,clipTop,clipLeft,UI_WINDOW_SUBRESOURCE_ALTERNATE_FRAME + UI_WINDOW_FRAME_LEFT,
                   bottomEdgeY,cornerHeight,0,control);
        UiWindow_BlitTiledVerticalEdge
                  (clipBottom,clipRight,clipTop,clipLeft,UI_WINDOW_SUBRESOURCE_ALTERNATE_FRAME + UI_WINDOW_FRAME_RIGHT,
                   bottomEdgeY,cornerHeight,rightEdgeX,control);
        UiWindow_BlitTiledHorizontalEdge
                  (clipBottom,clipRight,clipTop,clipLeft,UI_WINDOW_SUBRESOURCE_ALTERNATE_FRAME + UI_WINDOW_FRAME_BOTTOM,
                   rightEdgeX,bottomEdgeY,cornerWidth,control);
      }
      g_GraphicsFramebufferEndAccess();
    }
  }
  UiContainer_DrawIntersectingChildren(clipBottom,clipRight,clipTop,clipLeft,&control->root.base);
}

/* hitTest of g_UiFillPanelControlVtable: like UiContainer_HitTestChildren, but the container itself is never
   hit (UI_NODE_NONE instead), so the pointer passes through the panel to what lies below it.
*/
UiNodeBase * UiFillPanelControl_HitTestChildrenOnly
          (UiPixelCoordinate pointerY,UiPixelCoordinate pointerX,UiNodeBase *control)

{
  UiNodeBase *hitNode;

  hitNode = UiContainer_HitTestChildren(pointerY,pointerX,control);
  if (hitNode == control) {
    hitNode = UI_NODE_NONE;
  }
  return hitNode;
}

UiNodeVtable g_UiFillPanelControlVtable = {
        .relocate = UI_SLOT(UiContainer_RelocateChildren),
        .method04 = UI_SLOT(UiNode_DefaultMethod04_NoOp),
        .drawClipped = UI_SLOT(UiFillPanelControl_DrawColorOrTiledTextureAndChildren),
        .layout = UI_SLOT(UiContainer_LayoutChildren),
        .nonRightPress = UI_SLOT(UiNode_DefaultNonRightPress),
        .nonRightRelease = UI_SLOT(UiNode_DefaultNonRightRelease),
        .rightPress = UI_SLOT(UiNode_ForwardRightPressToParent),
        .rightRelease = UI_SLOT(UiNode_DefaultRightRelease),
        .nonRightDrag = UI_SLOT(UiNode_DefaultNonRightDrag),
        .rightDrag = UI_SLOT(UiNode_DefaultRightDrag),
        .pointerMove = UI_SLOT(UiNode_DefaultPointerMove),
        .hitTest = UI_SLOT(UiFillPanelControl_HitTestChildrenOnly),
        .keyboardEvent = UI_SLOT(UiNode_DefaultKeyboardEventMoveFocusNext),
        .applyFlags = UI_SLOT(UiNode_ApplyFlagsRecursive),
        .suppressActionId = UI_SLOT(UiContainer_SuppressActionId),
        .unsuppressActionId = UI_SLOT(UiContainer_UnsuppressActionId),
        .tick = UI_SLOT(UiNode_DefaultTick),
        .pointerWheel = UI_SLOT(UiNode_ForwardPointerWheelToParent)};

UiNodeVtable g_UiPanelControlVtable = {
        .relocate = UI_SLOT(UiContainer_RelocateChildren),
        .method04 = UI_SLOT(UiNode_DefaultMethod04_NoOp),
        .drawClipped = UI_SLOT(UiPanelControl_DrawOptionalTiledBackgroundFrameAndChildren),
        .layout = UI_SLOT(UiContainer_LayoutChildren),
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

/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/src/ui/controls/gauge.cpp
 * Reverse engineering by idkFoxes 2026
 */

#include <thandor/ui/controls/gauge.h>
#include <thandor/thandor.h>
#include <thandor/platform/bootstrap/image.h>

/* Module data. */

/* int32_t, 4: pixels from the gauge top to its label line (src/ui/controls/layout.c). */
static const int32_t g_UiHorizontalGaugeLabelTopInset = 4;

static const uint32_t g_UiHorizontalGaugeLabelTextStyle = 0;

static uint16_t g_UiWindowPercentTextUtf16[5] = {0};

/* Implementation ownership: ui/controls/gauge. */

/* drawClipped of g_UiHorizontalGaugeControlVtable (progress bar): draws the track, a fill proportional to
   (value - minimumValue) / (maximumValue - minimumValue) with value clamped to maximumValue, and with
   gaugeFlags bit 0 the percentage centred on top. The fill is left out while it would be narrower than
   its two caps. Children are not drawn.
*/
void UiHorizontalGaugeControl_DrawFrameFillAndLabel
          (UiPixelCoordinate clipBottom,UiPixelCoordinate clipRight,UiPixelCoordinate clipTop,
          UiPixelCoordinate clipLeft,UiHorizontalGaugeControl *control)

{
  uint64_t scaledFillProduct;
  uint32_t leftCapWidth;
  uint32_t clampedValue;
  uint32_t progress;
  uint32_t fillRange;
  uint32_t percentProgress;
  uint32_t percentRange;
  uint32_t percent;
  int rightCapX;
  int fillMinEndX;
  int fillEndX;
  uint32_t divisionRemainder;
  uint16_t *commandStream;
  Bool8 beginAccessFailed;
  GraphicsTextureLogicalSize textureSize;

  beginAccessFailed = g_GraphicsFramebufferBeginAccess();
  if (!beginAccessFailed) {
    g_GraphicsTextureSourceBlitSourceAlpha
              (clipBottom,clipRight,clipTop,clipLeft,control->base.top,control->base.left,
               UI_WINDOW_SUBRESOURCE_GAUGE_LEFT,g_UiWindowTextureSource,g_FramebufferAccess);
    textureSize = g_GraphicsTextureSourceGetLogicalSize(UI_WINDOW_SUBRESOURCE_GAUGE_LEFT,g_UiWindowTextureSource);
    leftCapWidth = textureSize.logicalWidthPixels;
    /* the right cap is assumed to be as wide as the left one */
    rightCapX = control->base.layoutWidth - leftCapWidth;
    UiWindow_BlitTiledHorizontalEdge
              (clipBottom,clipRight,clipTop,clipLeft,UI_WINDOW_SUBRESOURCE_GAUGE_TRACK,rightCapX,0,
               leftCapWidth,&control->base);
    g_GraphicsTextureSourceBlitSourceAlpha
              (clipBottom,clipRight,clipTop,clipLeft,control->base.top,rightCapX + control->base.left,
               UI_WINDOW_SUBRESOURCE_GAUGE_RIGHT,g_UiWindowTextureSource,g_FramebufferAccess);
    clampedValue = control->value;
    if (control->maximumValue < clampedValue) {
      clampedValue = control->maximumValue;
    }
    progress = clampedValue - control->minimumValue;
    percentProgress = 0;
    if (progress != 0 && (int)control->minimumValue <= (int)clampedValue) {
      scaledFillProduct = (uint64_t)progress * (uint64_t)(rightCapX - leftCapWidth);
      fillRange = control->maximumValue - control->minimumValue;
      if (fillRange == 0) {
        fillRange = 1;
      }
      /* Original quirk: meant as rounding (add 1 when doubling the remainder overflows 32 bits), but it rounds
         up only when the remainder has bit 31 set */
      divisionRemainder = (uint32_t)(scaledFillProduct % (uint64_t)fillRange);
      textureSize = g_GraphicsTextureSourceGetLogicalSize(UI_WINDOW_SUBRESOURCE_GAUGE_FILL_LEFT,
                                                          g_UiWindowTextureSource);
      fillEndX = ((int)(scaledFillProduct / fillRange) +
                  (divisionRemainder >> 31) + leftCapWidth) - textureSize.logicalWidthPixels;
      /* the fill is drawn only when there is room for both of its caps */
      fillMinEndX = textureSize.logicalWidthPixels + leftCapWidth;
      percentProgress = progress;
      if (fillMinEndX <= fillEndX) {
        UiWindow_BlitTiledHorizontalEdge
                  (clipBottom,clipRight,clipTop,clipLeft,UI_WINDOW_SUBRESOURCE_GAUGE_FILL,fillEndX,0,
                   fillMinEndX,&control->base);
        g_GraphicsTextureSourceBlitSourceAlpha
                  (clipBottom,clipRight,clipTop,clipLeft,control->base.top,leftCapWidth + control->base.left,
                   UI_WINDOW_SUBRESOURCE_GAUGE_FILL_LEFT,g_UiWindowTextureSource,g_FramebufferAccess);
        g_GraphicsTextureSourceBlitSourceAlpha
                  (clipBottom,clipRight,clipTop,clipLeft,control->base.top,fillEndX + control->base.left,
                   UI_WINDOW_SUBRESOURCE_GAUGE_FILL_RIGHT,g_UiWindowTextureSource,g_FramebufferAccess);
      }
    }
    if ((control->gaugeFlags & UI_HORIZONTAL_GAUGE_SHOW_PERCENT) != 0) {
      /* the percentage of the clamped progress (0 when no fill was computed), as "100%" or two digits
         without a leading zero */
      percentRange = control->maximumValue - control->minimumValue;
      if (percentRange == 0) {
        percentRange = 1;
      }
      divisionRemainder = (uint32_t)(((uint64_t)percentProgress * 100) % (uint64_t)percentRange);
      percent = (int)(((uint64_t)percentProgress * 100) / (uint64_t)percentRange) +
                (divisionRemainder >> 31); /* same rounding quirk as the fill above */
      if (percent == 100) {
        g_UiWindowPercentTextUtf16[0] = '1';
        g_UiWindowPercentTextUtf16[1] = '0';
        g_UiWindowPercentTextUtf16[2] = '0';
        g_UiWindowPercentTextUtf16[3] = '%';
        g_UiWindowPercentTextUtf16[4] = 0;
      }
      else {
        g_UiWindowPercentTextUtf16[1] = (short)((uint64_t)percent % 10) + '0';
        g_UiWindowPercentTextUtf16[0] = (short)((uint64_t)percent / 10) + '0';
        g_UiWindowPercentTextUtf16[2] = '%';
        g_UiWindowPercentTextUtf16[3] = 0;
      }
      commandStream = g_UiWindowPercentTextUtf16;
      if (g_UiWindowPercentTextUtf16[0] == '0') {
        commandStream = g_UiWindowPercentTextUtf16 + 1;
      }
      RichTextCommandStream_DrawSingleLine
                (clipBottom,clipRight,clipTop,clipLeft,g_UiHorizontalGaugeLabelTextStyle,
                 commandStream,g_UiHorizontalGaugeLabelTopInset + control->base.top,
                 ((uint32_t)control->base.layoutWidth >> 1) + control->base.left);
    }
    g_GraphicsFramebufferEndAccess();
  }
  return;
}

/* pointerMove of g_UiHorizontalGaugeControlVtable: returns the busy cursor as cursor frame, so a progress
   bar under the pointer shows it where the caller applies the frame (the in-game and scenario hover code
   pass it to g_GraphicsCursorSetFrame; the generic pointer-move dispatch ignores it).
*/
GraphicsCursorFrameIndex UiHorizontalGaugeControl_PointerMoveBusyCursor
          (UiPixelCoordinate pointerY,UiPixelCoordinate pointerX,UiNodeBase *control)

{
  return GRAPHICS_CURSOR_FRAME_BUSY;
}

UiNodeVtable g_UiHorizontalGaugeControlVtable = {
        .relocate = UI_SLOT(UiContainer_RelocateChildren),
        .method04 = UI_SLOT(UiNode_DefaultMethod04_NoOp),
        .drawClipped = UI_SLOT(UiHorizontalGaugeControl_DrawFrameFillAndLabel),
        .layout = UI_SLOT(UiContainer_LayoutChildren),
        .nonRightPress = UI_SLOT(UiNode_DefaultNonRightPress),
        .nonRightRelease = UI_SLOT(UiNode_DefaultNonRightRelease),
        .rightPress = UI_SLOT(UiNode_ForwardRightPressToParent),
        .rightRelease = UI_SLOT(UiNode_DefaultRightRelease),
        .nonRightDrag = UI_SLOT(UiNode_DefaultNonRightDrag),
        .rightDrag = UI_SLOT(UiNode_DefaultRightDrag),
        .pointerMove = UI_SLOT(UiHorizontalGaugeControl_PointerMoveBusyCursor),
        .hitTest = UI_SLOT(UiContainer_HitTestChildren),
        .keyboardEvent = UI_SLOT(UiNode_DefaultKeyboardEventMoveFocusNext),
        .applyFlags = UI_SLOT(UiNode_ApplyFlagsRecursive),
        .suppressActionId = UI_SLOT(UiContainer_SuppressActionId),
        .unsuppressActionId = UI_SLOT(UiContainer_UnsuppressActionId),
        .tick = UI_SLOT(UiNode_DefaultTick),
        .pointerWheel = UI_SLOT(UiNode_ForwardPointerWheelToParent)};

/* drawClipped of the transfer progress gauge (g_UiTransferProgressGaugeVtable) shown while the player snapshots
   are exchanged at session start: on the host (or in a local game) the range is the outgoing byte count
   and the value the smallest progress any client has reported (transferProgressBytes of player blocks 1..n); on a
   client it is the received byte count and the bytes received so far. Draws nothing unless a transfer is
   running and not yet complete.
*/
void UiHorizontalGaugeControl_UpdateRuntimeRangeAndDraw
          (int clipBottom,int clipRight,int clipTop,int clipLeft,UiHorizontalGaugeControl *control)

{
  UiTransferPayloadByteCount receivedTotal;
  uint32_t minimumProgress;
  uint32_t receivedDone;
  int remainingPlayers;
  FrontendPlayerRuntimeRecord *playerRecord;

  playerRecord = g_FrontendPlayerRuntimeBlocks;
  receivedTotal = g_UiTransferMailbox.receivedByteCount;
  minimumProgress = g_UiTransferMailbox.outgoingByteCount;
  if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_CLIENT) == SESSION_NETWORK_ROLE_LOCAL) {
    if (g_UiTransferMailbox.outgoingByteCount == 0) {
      return;
    }
    /* The original stops only on exactly one block; a count of 0 (-1 remaining) and counts above the 8
       allocated blocks are bounded here because they would scan ~2^32 records or past the blocks. */
    remainingPlayers = (int)g_FrontendPlayerRuntimeBlockCount - 1;
    if (remainingPlayers > FRONTEND_PLAYER_RUNTIME_RECORD_ALLOC_COUNT - 1) {
      static int s_loggedBlockCount;
      if (s_loggedBlockCount == 0) {
        s_loggedBlockCount = 1;
        Thandor_Log("transfer gauge: block count %d out of range, scan bounded",
                    (int)g_FrontendPlayerRuntimeBlockCount);
      }
      remainingPlayers = FRONTEND_PLAYER_RUNTIME_RECORD_ALLOC_COUNT - 1;
    }
    if (remainingPlayers <= 0) {
      return;
    }
    control->minimumValue = 0;
    control->maximumValue = minimumProgress;
    /* the clients follow the host's own block 0 */
    do {
      if ((int)playerRecord[1].transferProgressBytes < (int)minimumProgress) {
        minimumProgress = playerRecord[1].transferProgressBytes;
      }
      remainingPlayers--;
      playerRecord = playerRecord + 1;
    } while (remainingPlayers != 0);
    control->value = minimumProgress;
    if (control->maximumValue <= minimumProgress) {
      return;
    }
  }
  else {
    if ((g_UiTransferMailbox.receivedByteCount == 0) &&
       (g_UiTransferMailbox.receivedRemainingBytes == 0)) {
      return;
    }
    receivedDone = g_UiTransferMailbox.receivedByteCount - g_UiTransferMailbox.receivedRemainingBytes;
    control->minimumValue = 0;
    control->maximumValue = receivedTotal;
    control->value = receivedDone;
    if (receivedTotal <= receivedDone) {
      return;
    }
  }
  UiHorizontalGaugeControl_DrawFrameFillAndLabel(clipBottom,clipRight,clipTop,clipLeft,control);
  return;
}

UiNodeVtable g_UiTransferProgressGaugeVtable = {
    .relocate = UI_SLOT(UiContainer_RelocateChildren),
    .method04 = UI_SLOT(UiNode_DefaultMethod04_NoOp),
    .drawClipped = UI_SLOT(UiHorizontalGaugeControl_UpdateRuntimeRangeAndDraw),
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

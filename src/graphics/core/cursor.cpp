/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/src/graphics/core/cursor.cpp
 * Reverse engineering by idkFoxes 2026
 */

/* The mouse cursor: frame animation and the primary-surface refresh timer, the input event ring with double
   clicks, and saving/composing/restoring the cursor background around a present. */

#include <thandor/graphics/core/cursor.h>
#include <thandor/thandor.h>
#include <thandor/platform/bootstrap/image.h>
#include <thandor/platform/debug/hooks.h>

/* Module data. */

__declspec(align(8)) GraphicsCursorSetFrameProc *g_GraphicsCursorSetFrame = THANDOR_FN(GraphicsCursor_SetFrameIndex);

int32_t g_CursorCurrentVisibilityToken = 0;

SoftwareFramebufferAccess *g_CursorAlternateSavedBackground = 0;

/* uint32_t ticks until the next cursor animation frame (initial 2, reloaded with 2 when it reaches 0 in graphics/core/runtime.c). */
static uint32_t g_GraphicsCursorAnimationCountdown = 2;

static uint32_t g_CursorButtonReleaseClock[3] = {0};

static GraphicsCursorFrameIndex g_CursorFrameIndex = 0;

static UiPixelCoordinate g_CursorLastClickX = 0;

static UiPixelCoordinate g_CursorLastClickY = 0;

/* scratch descriptor the cursor save/restore Lock fills (lPitch, lpSurface) */
static DDSURFACEDESC_DX6 g_GraphicsCursorSurfaceDesc = {0};

static int32_t g_CursorCurrentDrawX = 0;

static int32_t g_CursorCurrentDrawY = 0;

GraphicsCursorInputEvent18 g_CursorInputEvents[256] = {0};

uint32_t g_CursorInputReadIndex = 0;

uint32_t g_CursorInputClockValue = 0;

GraphicsTextureSourceAsset *g_CursorSourceAsset = 0;

GraphicsCursorFrameRecord *g_CursorFrameRecords = 0;

GraphicsCursorFrameCount g_CursorFrameCount = 0;

GraphicsCursorConsumeEventProc *g_GraphicsCursorConsumeEvent = THANDOR_FN(GraphicsCursor_ConsumeNextInputEvent);

SoftwareFramebufferAccess *g_CursorSavedBackground = 0;

SoftwareFramebufferAccess *g_CursorCompositeBuffer = 0;

uint32_t g_MouseEventsProcessed = 0;

/* Periodic cursor timer callback: keeps the software mouse cursor animated and in place independently of the
   game's frame rate. Every second tick it steps the idle and active animation subresources of the current cursor
   frame (wrapping to the first one); when the frame changed or mouse events moved the cursor, and the graphics
   backend is not in use, the cursor is redrawn directly on the primary surface.
*/
void GraphicsCursor_AdvanceAnimationAndRefreshPrimaryTimer(void)

{
  GraphicsCursorFrameRecord *frameRecords;
  GraphicsCursorFrameIndex activeFrameIndex;
  int32_t previousAccessState;
  GraphicsSubresourceIndex nextIdleSubresource;
  GraphicsSubresourceIndex nextActiveSubresource;
  Bool8 frameAdvanced;

  frameAdvanced = false;
  activeFrameIndex = g_CursorFrameIndex;
  frameRecords = g_CursorFrameRecords;
  if (g_CursorVisibilityToken < 0) {
    return;
  }
  g_CursorInputClockValue++;
  g_GraphicsCursorAnimationCountdown--;
  if (g_GraphicsCursorAnimationCountdown == 0) {
    g_GraphicsCursorAnimationCountdown = 2;
    nextIdleSubresource = g_CursorFrameRecords[g_CursorFrameIndex].idleSubresourceIndex + 1;
    nextActiveSubresource = g_CursorFrameRecords[g_CursorFrameIndex].activeSubresourceIndex + 1;
    if (g_CursorFrameRecords[g_CursorFrameIndex].idleAnimationLastSubresourceIndex < nextIdleSubresource) {
      nextIdleSubresource = g_CursorFrameRecords[g_CursorFrameIndex].idleAnimationFirstSubresourceIndex;
    }
    if (g_CursorFrameRecords[g_CursorFrameIndex].activeAnimationLastSubresourceIndex < nextActiveSubresource) {
      nextActiveSubresource = g_CursorFrameRecords[g_CursorFrameIndex].activeAnimationFirstSubresourceIndex;
    }
    if (nextIdleSubresource != g_CursorFrameRecords[g_CursorFrameIndex].idleSubresourceIndex) {
      g_CursorFrameRecords[g_CursorFrameIndex].idleSubresourceIndex = nextIdleSubresource;
      frameRecords[activeFrameIndex].activeSubresourceIndex = nextActiveSubresource;
      frameAdvanced = true;
    }
  }
  /* Recompose the cursor when its animation frame changed or the mouse moved. */
  if ((!frameAdvanced) && (g_MouseEventsProcessed == 0)) {
    return;
  }
#ifdef THANDOR_PLATFORM_SDL3
  /* SDL3 backend: there is no primary surface to draw on from this (timer) thread; SdlVideo_Present composes
     the cursor into every presented frame. */
  return;
#endif
  /* Windowed mode (developer tools, not in the original): the primary surface is the whole desktop, so drawing
     the cursor at framebuffer coordinates would paint over the desktop's top left corner.
     GraphicsFramebuffer_Present composes the cursor into the back surface every frame, so the timer refresh is
     skipped. */
  if (DebugHook_Windowed()) {
    return;
  }
  /* try-lock: the original atomically swaps 1 into the access state and only draws when it was 0 */
  if (g_CursorSourceAsset != NULL) {
    previousAccessState = (int32_t)THANDOR_ATOMIC_EXCHANGE(&g_GraphicsBackendAccessState,1);
    if (previousAccessState == 0) {
      g_MouseEventsProcessed = 0;
      GraphicsCursor_RestoreAfterPresent(g_PrimarySurface3);
      GraphicsCursor_ComposeBeforePresent(g_PrimarySurface3);
      g_GraphicsBackendAccessState--;
    }
  }
  return;
}

/* Selects the software cursor frame (GRAPHICS_CURSOR_FRAME_*) that the cursor timer animates and draws.
   Returns true when the frame was selected, false (frame unchanged) for an index at or above g_CursorFrameCount;
   the error to report for that is FATAL_ERROR_CURSOR_FRAME_OUT_OF_RANGE. Installed in g_GraphicsCursorSetFrame.
*/
Bool8 GraphicsCursor_SetFrameIndex(UiNumericCursorFrameIndex frameIndex)

{
  if (frameIndex < g_CursorFrameCount) {
    g_CursorFrameIndex = frameIndex;
    return true;
  }
  return false;
}

/* The software cursor frame selected by GraphicsCursor_SetFrameIndex (for backends that compose the cursor
   themselves, such as the SDL3 backend). */
GraphicsCursorFrameIndex GraphicsCursor_GetFrameIndex(void)

{
  return g_CursorFrameIndex;
}

/* Takes the next mouse event from the 256-entry ring the mouse input code fills (returns false and leaves
   *outEvent untouched when it is empty; true with the event in *outEvent otherwise) and publishes it: button state, cursor position and wheel delta go to the g_Cursor* globals, a release stores
   its clock per button, a press its position as the last click. Installed in g_GraphicsCursorConsumeEvent.
   A press less than 16 clock ticks after the release of the same button and within
   +-4 pixels of the last click also sets bit 31 (double click) in the returned button state (outEvent->buttonState;
   g_CursorButtonState keeps the raw value), which UiPointer_DispatchPendingEvents passes on to the press
   dispatchers as UI_POINTER_BUTTON_REPEAT_CLICK.
*/
Bool8 GraphicsCursor_ConsumeNextInputEvent(CursorPointerEvent *outEvent)

{
  GraphicsCursorEventType consumedEventType;
  uint32_t nextReadIndex;
  GraphicsCursorClockValue eventClock;
  uint32_t eventIndex;
  uint32_t rawButtonState;
  uint32_t ticksSinceRelease;
  UiPixelCoordinate clickDeltaX;
  UiPixelCoordinate clickDeltaY;

  eventIndex = g_CursorInputReadIndex;
  nextReadIndex = g_CursorInputReadIndex + 1;
  if (g_CursorInputReadIndex == g_CursorInputWriteIndex) {
    return false;
  }
  if (GRAPHICS_CURSOR_INPUT_EVENT_CAPACITY - 1 < nextReadIndex) { /* wrap around the ring */
    nextReadIndex = 0;
  }
  g_CursorInputReadIndex = nextReadIndex;
  consumedEventType = g_CursorInputEvents[eventIndex].eventType;
  eventClock = g_CursorInputEvents[eventIndex].clockValue;
  rawButtonState = g_CursorInputEvents[eventIndex].buttonState;
  g_CursorButtonState = rawButtonState;
  /* ticksSinceRelease: ticks since the release of the pressed button; releases (and motion) leave the limit
     itself, which never counts as a double click. The compare is unsigned. */
  ticksSinceRelease = GRAPHICS_CURSOR_DOUBLE_CLICK_TICKS;
  if (consumedEventType == LEFT_PRESS) {
    ticksSinceRelease = eventClock - g_CursorButtonReleaseClock[0];
  }
  else if (consumedEventType == MIDDLE_PRESS) {
    ticksSinceRelease = eventClock - g_CursorButtonReleaseClock[1];
  }
  else if (consumedEventType == RIGHT_PRESS) {
    ticksSinceRelease = eventClock - g_CursorButtonReleaseClock[2];
  }
  else if (consumedEventType == LEFT_RELEASE) {
    g_CursorButtonReleaseClock[0] = eventClock;
  }
  else if (consumedEventType == MIDDLE_RELEASE) {
    g_CursorButtonReleaseClock[1] = eventClock;
  }
  else if (consumedEventType == RIGHT_RELEASE) {
    g_CursorButtonReleaseClock[2] = eventClock;
  }
  /* The returned state gets bit 31 for a double click; g_CursorButtonState above stays raw. The distance
     is measured to the previous press, before this press becomes the last click below. */
  rawButtonState = rawButtonState & ~GRAPHICS_CURSOR_BUTTON_DOUBLE_CLICK;
  if (ticksSinceRelease < GRAPHICS_CURSOR_DOUBLE_CLICK_TICKS) {
    clickDeltaX = g_CursorInputEvents[eventIndex].pointerX - g_CursorLastClickX;
    clickDeltaY = g_CursorInputEvents[eventIndex].pointerY - g_CursorLastClickY;
    if ((-GRAPHICS_CURSOR_DOUBLE_CLICK_DISTANCE <= clickDeltaX) &&
        (clickDeltaX <= GRAPHICS_CURSOR_DOUBLE_CLICK_DISTANCE) &&
        (-GRAPHICS_CURSOR_DOUBLE_CLICK_DISTANCE <= clickDeltaY) &&
        (clickDeltaY <= GRAPHICS_CURSOR_DOUBLE_CLICK_DISTANCE)) {
      rawButtonState = rawButtonState | GRAPHICS_CURSOR_BUTTON_DOUBLE_CLICK;
    }
  }
  g_CursorOverrideX = g_CursorInputEvents[eventIndex].pointerX;
  g_CursorOverrideY = g_CursorInputEvents[eventIndex].pointerY;
  g_CursorWheelDelta = g_CursorInputEvents[eventIndex].wheelDelta;
  if ((consumedEventType != MOTION_OR_WHEEL) && (consumedEventType < 4)) { /* a press: LEFT/MIDDLE/RIGHT_PRESS */
    g_CursorLastClickX = g_CursorInputEvents[eventIndex].pointerX;
    g_CursorLastClickY = g_CursorInputEvents[eventIndex].pointerY;
  }
  outEvent->eventType = consumedEventType;
  outEvent->buttonState = (GraphicsCursorButtonState)rawButtonState;
  outEvent->pointerX = g_CursorInputEvents[eventIndex].pointerX;
  outEvent->pointerY = g_CursorInputEvents[eventIndex].pointerY;
  outEvent->wheelDelta = g_CursorInputEvents[eventIndex].wheelDelta;
  return true;
}

/* Draws the software mouse cursor into backSurface before it is presented (the animation timer also redraws it
   on the primary surface when it moved): the background under the cursor is saved twice (once to draw on, once
   for GraphicsCursor_RestoreAfterPresent), the cursor frame is blended onto the first copy (the pressed image
   while a mouse button is down) and that copy is written back.
   The visibility token is latched so the restore matches what was drawn.
*/
void GraphicsCursor_ComposeBeforePresent(IDirectDrawSurface3 *backSurface)

{
  UiPixelCoordinate cursorX;
  uint32_t cursorSubresourceIndex;
  GraphicsCursorFrameRecord *cursorFrame;
  UiPixelCoordinate cursorY;
  int drawY;

  g_CursorCurrentVisibilityToken = g_CursorVisibilityToken;
  if (-1 < g_CursorVisibilityToken) { /* a negative token hides the cursor */
    cursorX = g_MouseX;
    cursorY = g_MouseY;
    if (g_CursorUseOverridePosition != 0) {
      cursorX = g_CursorOverrideX;
      cursorY = g_CursorOverrideY;
    }
    cursorFrame = g_CursorFrameRecords + g_CursorFrameIndex;
    cursorX = cursorX - cursorFrame->hotspotX;
    drawY = cursorY - cursorFrame->hotspotY;
    g_CursorCurrentDrawX = cursorX;
    g_CursorCurrentDrawY = drawY;
    GraphicsCursor_SaveSurfaceBackground(g_CursorCompositeBuffer,drawY,cursorX,backSurface);
    GraphicsCursor_SaveSurfaceBackground(g_CursorSavedBackground,drawY,cursorX,backSurface);
    cursorSubresourceIndex = cursorFrame->activeSubresourceIndex;
    if ((g_CursorButtonState & LEFT_MIDDLE_RIGHT) == 0) { /* none of the three mouse buttons is down */
      cursorSubresourceIndex = cursorFrame->idleSubresourceIndex;
    }
    /* the composite buffer holds the saved rectangle at its origin, so the cursor is drawn at (0,0) */
    g_GraphicsTextureSourceBlitSourceAlpha
              (g_FramebufferHeight,g_FramebufferWidth,0,0,0,0,cursorSubresourceIndex,g_CursorSourceAsset,
               g_CursorCompositeBuffer);
    GraphicsCursor_RestoreSurfaceBackground(g_CursorCompositeBuffer,drawY,cursorX,backSurface);
  }
  return;
}

/* Removes the software cursor from backSurface again by writing back the background that
   GraphicsCursor_ComposeBeforePresent saved, so the surface is clean again (for the next frame or for drawing
   the cursor at its new position). Skipped when the cursor was hidden at compose time.
*/
void GraphicsCursor_RestoreAfterPresent(IDirectDrawSurface3 *backSurface)

{
  if (-1 < g_CursorCurrentVisibilityToken) {
    GraphicsCursor_RestoreSurfaceBackground
              (g_CursorSavedBackground,g_CursorCurrentDrawY,g_CursorCurrentDrawX,backSurface);
  }
  return;
}

/* Saves the screen rectangle under the software cursor: copies the part of sourceSurface at (drawX, drawY)
   that lies on screen into destinationBuffer (same layout, 16 or 32 bits per pixel), so the cursor can later
   be removed again with GraphicsCursor_RestoreSurfaceBackground. The surface is restored first if it was lost.
*/
void GraphicsCursor_SaveSurfaceBackground(SoftwareFramebufferAccess *destinationBuffer,GraphicsScreenCoordinate drawY,
          GraphicsScreenCoordinate drawX,IDirectDrawSurface3 *sourceSurface)

{
  /* Copies a clipped rectangle of the locked surface into the buffer origin. */
  int bytesPerPixel;
  int rowPixels;
  int copyWidth;
  int copyHeight;
  uint8_t *destination;
  uint8_t *source;
  uint8_t *surfacePixels;
  TH_LEGACY_HRESULT result;

  bytesPerPixel = destinationBuffer->bytesPerPixel == 2 ? 2 : 4;
  rowPixels = (int)destinationBuffer->width;
  copyWidth = (int)destinationBuffer->width;
  copyHeight = (int)destinationBuffer->height;
  destination = destinationBuffer->pixels;
  if (drawX < 0) {
    destination = destination + -drawX * bytesPerPixel;
    copyWidth = copyWidth + drawX;
    drawX = 0;
  }
  if (drawY < 0) {
    copyHeight = copyHeight + drawY;
    destination = destination + -drawY * rowPixels * bytesPerPixel;
    drawY = 0;
  }
  if (copyWidth + drawX - (int)g_FramebufferWidth > 0) {
    copyWidth = copyWidth - (copyWidth + drawX - (int)g_FramebufferWidth);
  }
  if (copyHeight + drawY - (int)g_FramebufferHeight > 0) {
    copyHeight = copyHeight - (copyHeight + drawY - (int)g_FramebufferHeight);
  }
  if (copyWidth <= 0 || copyHeight <= 0) {
    return;
  }
  result = sourceSurface->lpVtbl->IsLost(sourceSurface);
  if (result != 0) {
    result = sourceSurface->lpVtbl->Restore(sourceSurface);
  }
  if (result == 0) {
    /* the scratch DDSURFACEDESC: cleared, then dwSize set */
    Memory_ZeroDwords(sizeof(DDSURFACEDESC_DX6),&g_GraphicsCursorSurfaceDesc);
    g_GraphicsCursorSurfaceDesc.dwSize = sizeof(DDSURFACEDESC_DX6);
    result = sourceSurface->lpVtbl->Lock
                       (sourceSurface,NULL,&g_GraphicsCursorSurfaceDesc,DDLOCK_WAIT | DDLOCK_READONLY,NULL);
  }
  if (result != 0) {
    return;
  }
  surfacePixels = (uint8_t *)g_GraphicsCursorSurfaceDesc.lpSurface;
  source = surfacePixels + drawY * (int)g_GraphicsCursorSurfaceDesc.lPitch + drawX * bytesPerPixel;
  for (; copyHeight != 0; copyHeight--) {
    memcpy(destination,source,(size_t)(copyWidth * bytesPerPixel));
    destination = destination + rowPixels * bytesPerPixel;
    source = source + (int)g_GraphicsCursorSurfaceDesc.lPitch;
  }
  sourceSurface->lpVtbl->Unlock(sourceSurface,surfacePixels);
}

/* Writes a buffer filled by GraphicsCursor_SaveSurfaceBackground (or the composed cursor image) back into
   destinationSurface at (drawX, drawY), clipped to the screen exactly like the save. Used to draw the
   composed cursor and to remove it again after the present.
*/
void GraphicsCursor_RestoreSurfaceBackground(SoftwareFramebufferAccess *sourceBuffer,GraphicsScreenCoordinate drawY,
          GraphicsScreenCoordinate drawX,IDirectDrawSurface3 *destinationSurface)

{
  /* Mirror of GraphicsCursor_SaveSurfaceBackground: buffer rows back into the locked surface. */
  int bytesPerPixel;
  int rowPixels;
  int copyWidth;
  int copyHeight;
  uint8_t *source;
  uint8_t *destination;
  uint8_t *surfacePixels;
  TH_LEGACY_HRESULT result;

  bytesPerPixel = sourceBuffer->bytesPerPixel == 2 ? 2 : 4;
  rowPixels = (int)sourceBuffer->width;
  copyWidth = (int)sourceBuffer->width;
  copyHeight = (int)sourceBuffer->height;
  source = sourceBuffer->pixels;
  if (drawX < 0) {
    source = source + -drawX * bytesPerPixel;
    copyWidth = copyWidth + drawX;
    drawX = 0;
  }
  if (drawY < 0) {
    copyHeight = copyHeight + drawY;
    source = source + -drawY * rowPixels * bytesPerPixel;
    drawY = 0;
  }
  if (copyWidth + drawX - (int)g_FramebufferWidth > 0) {
    copyWidth = copyWidth - (copyWidth + drawX - (int)g_FramebufferWidth);
  }
  if (copyHeight + drawY - (int)g_FramebufferHeight > 0) {
    copyHeight = copyHeight - (copyHeight + drawY - (int)g_FramebufferHeight);
  }
  if (copyWidth <= 0 || copyHeight <= 0) {
    return;
  }
  result = destinationSurface->lpVtbl->IsLost(destinationSurface);
  if (result != 0) {
    result = destinationSurface->lpVtbl->Restore(destinationSurface);
  }
  if (result == 0) {
    /* the scratch DDSURFACEDESC: cleared, then dwSize set */
    Memory_ZeroDwords(sizeof(DDSURFACEDESC_DX6),&g_GraphicsCursorSurfaceDesc);
    g_GraphicsCursorSurfaceDesc.dwSize = sizeof(DDSURFACEDESC_DX6);
    /* write-only lock (DDLOCK_WAIT | DDLOCK_WRITEONLY), as in the original */
    result = destinationSurface->lpVtbl->Lock
                       (destinationSurface,NULL,&g_GraphicsCursorSurfaceDesc,DDLOCK_WAIT | DDLOCK_WRITEONLY,NULL);
  }
  if (result != 0) {
    return;
  }
  surfacePixels = (uint8_t *)g_GraphicsCursorSurfaceDesc.lpSurface;
  destination = surfacePixels + drawY * (int)g_GraphicsCursorSurfaceDesc.lPitch + drawX * bytesPerPixel;
  for (; copyHeight != 0; copyHeight--) {
    memcpy(destination,source,(size_t)(copyWidth * bytesPerPixel));
    destination = destination + (int)g_GraphicsCursorSurfaceDesc.lPitch;
    source = source + rowPixels * bytesPerPixel;
  }
  destinationSurface->lpVtbl->Unlock(destinationSurface,surfacePixels);
}

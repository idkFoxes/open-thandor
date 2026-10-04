/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/src/graphics/core/cursor.cpp
 * Reverse engineering by idkFoxes 2026
 */

/* The mouse cursor: frame animation (the cursor timer) and the input event ring with double clicks. The cursor
   itself is composed into every presented frame by the SDL3 backend (SdlVideo_Present). */

#include <thandor/graphics/core/cursor.h>
#include <thandor/thandor.h>
#include <thandor/platform/bootstrap/image.h>
#include <thandor/platform/debug/hooks.h>

/* Module data. */

THANDOR_ALIGN(8) GraphicsCursorSetFrameProc *g_GraphicsCursorSetFrame = THANDOR_FN(GraphicsCursor_SetFrameIndex);

int32_t g_CursorCurrentVisibilityToken = 0;

SoftwareFramebufferAccess *g_CursorAlternateSavedBackground = nullptr;

/* uint32_t ticks until the next cursor animation frame (initial 2, reloaded with 2 when it reaches 0). */
static uint32_t g_GraphicsCursorAnimationCountdown = 2;

static uint32_t g_CursorButtonReleaseClock[3] = {0};

static GraphicsCursorFrameIndex g_CursorFrameIndex = 0;

static UiPixelCoordinate g_CursorLastClickX = 0;

static UiPixelCoordinate g_CursorLastClickY = 0;

GraphicsCursorInputEvent18 g_CursorInputEvents[256] = {0};

uint32_t g_CursorInputReadIndex = 0;

uint32_t g_CursorInputClockValue = 0;

GraphicsTextureSourceAsset *g_CursorSourceAsset = nullptr;

GraphicsCursorFrameRecord *g_CursorFrameRecords = nullptr;

GraphicsCursorFrameCount g_CursorFrameCount = 0;

GraphicsCursorConsumeEventProc *g_GraphicsCursorConsumeEvent = THANDOR_FN(GraphicsCursor_ConsumeNextInputEvent);

SoftwareFramebufferAccess *g_CursorSavedBackground = nullptr;

SoftwareFramebufferAccess *g_CursorCompositeBuffer = nullptr;

/* Periodic cursor timer callback: keeps the software mouse cursor animated and in place independently of the
   game's frame rate. Every second tick it steps the idle and active animation subresources of the current cursor
   frame (wrapping to the first one). The cursor itself is drawn by the present (SdlVideo_Present).
*/
void GraphicsCursor_AdvanceAnimationAndRefreshPrimaryTimer(void)

{
  GraphicsCursorFrameRecord *frameRecords;
  GraphicsCursorFrameIndex activeFrameIndex;
  GraphicsSubresourceIndex nextIdleSubresource;
  GraphicsSubresourceIndex nextActiveSubresource;

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
    }
  }
  /* The original redraws the cursor on the primary surface here when its frame changed or the mouse moved. The
     SDL3 backend has no primary surface to draw on from this (timer) thread: SdlVideo_Present composes the cursor
     into every presented frame. */
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


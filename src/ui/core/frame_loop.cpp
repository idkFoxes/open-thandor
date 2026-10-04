/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/src/ui/core/frame_loop.cpp
 * Reverse engineering by idkFoxes 2026
 */

#include <thandor/ui/core/frame_loop.h>
#include <thandor/thandor.h>

/* Module data. */

std::atomic<uint32_t> g_UiPendingFrameTicks{0};

/* Implementation ownership: ui/core/frame_loop. */

/* Runs one complete UI frame (events, frame ticks, queued actions, draw, present) from code that may or may
   not hold the UI frame lock, e.g. modal loops and the fatal-error box: the lock is released for the frame
   and taken again afterwards only when it was held on entry.
*/
void __cdecl UiFrame_ProcessAndPresentWithLockTransition(void)

{
  Bool8 lockWasHeld;
  
  /* the try-acquire takes a free lock, so both paths release it before the frame */
  lockWasHeld = g_SpinLockTryAcquire(g_UiRuntimeFrameLock);
  if (!lockWasHeld) {
    g_SpinLockRelease(g_UiRuntimeFrameLock);
    UiKeyboard_DispatchPendingEvents();
    UiPointer_DispatchPendingEvents();
    UiFrame_Update(0);
    UiActionQueue_DispatchPending();
    UiFrame_Draw();
    g_GraphicsFramebufferPresent(g_FramebufferAccess);
    return;
  }
  g_SpinLockRelease(g_UiRuntimeFrameLock);
  UiKeyboard_DispatchPendingEvents();
  UiPointer_DispatchPendingEvents();
  UiFrame_Update(0);
  UiActionQueue_DispatchPending();
  UiFrame_Draw();
  g_GraphicsFramebufferPresent(g_FramebufferAccess);
  g_SpinLockAcquire(g_UiRuntimeFrameLock);
  return;
}

/* Runs one complete UI frame: dispatches pending keyboard and pointer events, runs the pending frame ticks,
   dispatches queued UI actions, draws and presents. Unlike UiFrame_ProcessAndPresentWithLockTransition it does
   not touch the UI frame lock itself.
*/
void UiFrame_ProcessAndPresent(void)

{
  UiKeyboard_DispatchPendingEvents();
  UiPointer_DispatchPendingEvents();
  UiFrame_Update(0); /* 0: pump messages once, do not wait for a frame tick */
  UiActionQueue_DispatchPending();
  UiFrame_Draw();
  g_GraphicsFramebufferPresent(g_FramebufferAccess);
  return;
}

/* Discards all buffered keyboard and pointer input and the frame ticks that piled up, so a UI loop that starts
   (or resumes after a movie, session or error box) neither reacts to stale input nor catches up on old ticks.
*/
void UiFrame_FlushInputAndResetPendingTicks(void)

{
  g_KeyboardFlushEvents();
  g_PointerFlushEvents();
  g_UiPendingFrameTicks = 0;
  return;
}

/* One UI frame step under the UI frame lock: pumps Win32 messages, then runs every pending frame tick
   (sprite-button animations and frame callback of the front root, tick of the pointer-capture and
   keyboard-focus nodes, tooltip countdown).
*/
void UiFrame_Update(UiStopMessageCode stopMessageCode)

{
  uint32_t ticksToRun;
  UiRootNode *frontRoot;
  UiRootCallbacks *rootCallbacks;

  g_SpinLockAcquire(g_UiRuntimeFrameLock);
  /* In the original the value compared here is always 0 (it is cleared before the loop and the
     message pump does not change it): pump until a frame tick is pending, or once when stopMessageCode
     is 0 (every caller passes 0). The pending tick count is then consumed (reset to 0). */
  do {
    g_Win32PumpMessages();
  } while ((stopMessageCode != 0) && (g_UiPendingFrameTicks == 0));
  ticksToRun = g_UiPendingFrameTicks.exchange(0); /* read and clear in one step: no timer tick is lost */
  for (; ticksToRun != 0; ticksToRun--) {
    frontRoot = g_UiRootNode;
    if (frontRoot != UI_ROOT_STACK_END) {
      UiTree_AdvanceSpriteButtonAnimations(&frontRoot->base);
      rootCallbacks = frontRoot->callbacks;
      if (rootCallbacks->frameUpdate != nullptr) {
        rootCallbacks->frameUpdate(frontRoot);
      }
    }
    if (g_UiPointerCaptureTarget != UI_NODE_NONE) {
      g_UiPointerCaptureTarget->vtable->tick(g_UiPointerCaptureTarget);
    }
    /* the focus node ticks only once when it also holds the pointer capture */
    if ((g_UiKeyboardFocusNode != UI_NODE_NONE) &&
       (g_UiKeyboardFocusNode != g_UiPointerCaptureTarget)) {
      g_UiKeyboardFocusNode->vtable->tick(g_UiKeyboardFocusNode);
    }
    UiTooltip_TickCountdown();
  }
  /* the original refreshes its DirectInput mouse here every 48th call; the SDL3 backend has no device to refresh */
  g_SpinLockReleaseAndInvoke
            ((SpinLockReleaseCallbackProc *)g_UiRuntimePostUnlockCallback,g_UiRuntimeFrameLock);
  return;
}

/* Draws the UI root stack from the bottom root up to the front root, each clipped to its rectangle within
   the framebuffer, and the tooltip on top.
*/
void UiFrame_Draw(void)

{
  /* Collects every root while walking previousRoot down from the front root, then draws them
     bottom to top, so the roots stacked above the bottom one (e.g. the end movie) are drawn too. */
  enum { ROOT_LIMIT = 64 };
  UiRootNode *roots[ROOT_LIMIT];
  UiRootNode *root;
  int count;
  int clipLeft;
  int clipTop;
  int clipRight;
  int clipBottom;

  if (g_UiRootNode == UI_ROOT_STACK_END) {
    return;
  }
  count = 0;
  for (root = g_UiRootNode; (root != UI_ROOT_STACK_END) && (count < ROOT_LIMIT);
       root = root->previousRoot) {
    roots[count] = root;
    count++;
  }
  while (count != 0) {
    count--;
    root = roots[count];
    clipLeft = (root->base).left;
    clipTop = (root->base).top;
    clipRight = (root->base).right;
    clipBottom = (root->base).bottom;
    if (clipLeft < 0) {
      clipLeft = 0;
    }
    if (clipTop < 0) {
      clipTop = 0;
    }
    if ((int)g_FramebufferWidth < clipRight) {
      clipRight = g_FramebufferWidth;
    }
    if ((int)g_FramebufferHeight < clipBottom) {
      clipBottom = g_FramebufferHeight;
    }
    if ((clipLeft < clipRight) && (clipTop < clipBottom)) {
      /* drawClipped takes (bottom, right, top, left, node) */
      (*((root->base).vtable)->drawClipped)(clipBottom,clipRight,clipTop,clipLeft,&root->base);
    }
  }
  UiTooltip_Draw(g_FramebufferHeight,g_FramebufferWidth,0,0);
  return;
}

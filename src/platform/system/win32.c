/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/src/platform/system/win32.c
 * Reverse engineering by idkFoxes 2026
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <thandor/platform/system/win32.h>
#include <thandor/thandor.h>
#include <thandor/platform/bootstrap/image.h>

/* Implementation ownership: platform/system/win32. */

/* Test aid: OPEN_THANDOR_AUTOSHOT=<milliseconds> saves the game's own framebuffer to
   shots\shot_NNNN.bmp at that interval (checked from the message pump), so automated runs can be
   looked at without capturing the desktop. */
static void Win32_AutoShotTick(void)
{
  static int interval = -1;
  static unsigned last;
  static unsigned number;
  unsigned now;
  FramebufferCaptureResult capture;
  if (interval < 0) {
    const char *value = getenv("OPEN_THANDOR_AUTOSHOT");
    interval = (value != NULL) ? atoi(value) : 0;
    if (interval > 0) {
      CreateDirectoryA("shots", NULL);
    }
    last = Thandor_TickCount();
  }
  if (interval <= 0 || g_GraphicsFramebufferCaptureRegion == NULL || g_FramebufferWidth == 0) {
    return;
  }
  now = Thandor_TickCount();
  if (now - last < (unsigned)interval) {
    return;
  }
  last = now;
  capture = g_GraphicsFramebufferCaptureRegion(g_FramebufferHeight,g_FramebufferWidth,0,0);
#ifdef THANDOR_TEST_AIDS
  if (capture.failed || capture.capture == NULL) {
    Thandor_Log("autoshot failed: error %08x, backend access state %d, frame heartbeat %u",
                (unsigned)(uintptr_t)capture.capture, (int)g_GraphicsBackendAccessState, g_ThandorFrameHeartbeat);
  }
#endif
  if (!capture.failed && capture.capture != NULL) {
    GraphicsTextureSourceEntry *entry = &capture.capture->sourceEntry;
    const uint32_t *pixels = (const uint32_t *)((uint8_t *)capture.capture + entry->dataOffset);
    uint32_t width = entry->pixelWidth;
    uint32_t height = entry->pixelHeight;
    char name[64];
    FILE *file;
    sprintf(name, "shots\\shot_%04u.bmp", number++);
    file = fopen(name, "wb");
    if (file != NULL) {
      /* "BM" + the rest of BITMAPFILEHEADER (14 bytes) and a BITMAPINFOHEADER (40 bytes): 32-bit top-down */
      uint32_t header[13];
      uint32_t imageBytes = width * height * 4;
      memset(header, 0, sizeof header);
      fwrite("BM", 1, 2, file);
      header[0] = 54 + imageBytes; /* bfSize */
      header[2] = 54;              /* bfOffBits */
      header[3] = 40;              /* biSize */
      header[4] = width;
      header[5] = (uint32_t)-(int)height; /* negative height: rows top to bottom */
      header[6] = 1 | (32 << 16);  /* biPlanes 1, biBitCount 32 */
      header[8] = imageBytes;      /* biSizeImage */
      fwrite(header, 4, 13, file);
      fwrite(pixels, 4, width * height, file);
      fclose(file);
      Thandor_Log("autoshot %s (%ux%u)", name, width, height);
    }
    g_MemoryApi.free(capture.capture);
  }
}

/* Test aid: OPEN_THANDOR_SCRIPT=<file> replays timed input from a text file, one command per line:
     <ms> click <x> <y> [hold]  left press at framebuffer pixel x,y, release after hold ms (120)
     <ms> rclick <x> <y>    the same with the right button
     <ms> move <x> <y>      pointer motion
     <ms> drag <x> <y> <x2> <y2>  left press at x,y, motion to x2,y2 with the button held, release there
     <ms> key <vk>          key press and release (Windows virtual-key code, decimal)
     <ms> shot              save the framebuffer now (shots\script_NNNN.bmp, needs AUTOSHOT's folder)
     <ms> quit              end the process
     <ms> layout <w> <h>    the following coordinates are for a w x h screen; they are moved by half
                            the difference to the current resolution (dialogs and the view are centred)
     <ms> clickuntilingame <x> <y> [interval]  left click x,y every interval ms (3000) until the level runs
     <ms> clickuntilnextlevel <x> <y> [interval]  the same until the next in-game session starts (test build:
                            campaign level change); the times of the following lines count from then
     <ms> ingame            wait until the level has loaded and the game runs; the times of the
                            following lines count from that moment
   <ms> counts from the first message pump. Pointer events go into the same ring DirectInput fills. */
volatile unsigned g_TestAidInGameFrames;
volatile unsigned g_TestAidSessionCount;

/* Port-only test aid (no original address). Moves the pointer to framebuffer pixel x,y with button mask buttons
   (LEFT/RIGHT of GraphicsCursorButtonState) and appends a pointer event of that type to the 256-entry ring
   g_CursorInputEvents, as DirectInputMouse_PollBufferedEvents does for real mouse input, so scripted clicks
   reach the UI through the normal event path. */
static void Win32_PushCursorEvent(GraphicsCursorEventType type, uint32_t buttons, int x, int y)
{
  uint32_t index = g_CursorInputWriteIndex;
  uint32_t next = index + 1;
  if (255 < next) { /* wrap around the ring */
    next = 0;
  }
  g_MouseX = x;
  g_MouseY = y;
  g_MouseButtonMask = buttons;
  g_CursorInputEvents[index].eventType = type;
  g_CursorInputEvents[index].buttonState = buttons;
  g_CursorInputEvents[index].pointerX = x;
  g_CursorInputEvents[index].pointerY = y;
  g_CursorInputEvents[index].wheelDelta = 0;
  g_CursorInputEvents[index].clockValue = g_CursorInputClockValue;
  g_CursorInputWriteIndex = next;
}

static void Win32_ScriptTick(void)
{
  static FILE *script;
  static int state = -1;
  static unsigned start;
  static unsigned due;
  static char command[32];
  static int x;
  static int y;
  static int hold;
  static int extra;
  static int pendingRelease;
  static uint32_t releaseButton;
  static unsigned releaseAt;
  static int releaseX;
  static int releaseY;
  static int layoutWidth;
  static int layoutHeight;
  char line[128];
  unsigned now;
  /* state: -1 not started, 0 no (more) script, 1 read the next line, 2 a line waits for its time */
  if (state < 0) {
    const char *path = getenv("OPEN_THANDOR_SCRIPT");
    state = 0;
    if (path != NULL && (script = fopen(path, "r")) != NULL) {
      state = 1;
      start = Thandor_TickCount();
      Thandor_Log("script: %s", path);
    }
  }
  now = Thandor_TickCount() - start;
  {
    /* Report when the in-game session stops producing frames (the level ended and the frontend runs again). */
    static unsigned lastFrames;
    static unsigned framesChangedAt;
    static int sessionEndReported;
    unsigned tick = Thandor_TickCount();
    if (g_TestAidInGameFrames != lastFrames) {
      lastFrames = g_TestAidInGameFrames;
      framesChangedAt = tick;
      sessionEndReported = 0;
    }
    else if (lastFrames != 0 && !sessionEndReported && tick - framesChangedAt > 3000) {
      sessionEndReported = 1;
      Thandor_Log("script: in-game session ended after %u frames", lastFrames);
    }
  }
  if (pendingRelease && now >= releaseAt) {
    Win32_PushCursorEvent(releaseButton == 1 ? LEFT_RELEASE : RIGHT_RELEASE, 0, releaseX, releaseY);
    pendingRelease = 0;
  }
  if (state == 0) {
    return;
  }
  for (;;) {
    if (state == 1) {
      if (fgets(line, sizeof line, script) == NULL) {
        state = 0;
        fclose(script);
        return;
      }
      command[0] = 0;
      x = y = hold = extra = 0;
      if (sscanf(line, "%u %31s %d %d %d %d", &due, command, &x, &y, &hold, &extra) < 2) {
        continue;
      }
      state = 2;
    }
    if (now < due || pendingRelease) {
      return;
    }
    if (strcmp(command, "ingame") == 0) {
      if (g_TestAidInGameFrames == 0) {
        return;
      }
      start = Thandor_TickCount();
      now = 0;
      state = 1;
      Thandor_Log("script: in game, times restart at 0");
      continue;
    }
    if (strcmp(command, "clickuntilingame") == 0 || strcmp(command, "clickuntilnextlevel") == 0) {
      /* click x,y every `hold` ms (default 3000) until the level runs (clickuntilingame) or until the next
         in-game session has started (clickuntilnextlevel, test build: end movie, results, briefing of the next
         campaign level); this line stays pending until then */
      static unsigned targetSession;
      static int armed;
      int nextLevel = strcmp(command, "clickuntilnextlevel") == 0;
      int clickX = x;
      int clickY = y;
      int reached;
      if (nextLevel && !armed) {
        targetSession = g_TestAidSessionCount + 1;
        armed = 1;
      }
      reached = nextLevel ? g_TestAidSessionCount >= targetSession : g_TestAidInGameFrames != 0;
      if (reached) {
        armed = 0;
        state = 1;
        if (nextLevel) {
          start = Thandor_TickCount();
          now = 0;
          Thandor_Log("script: next level started (session %u), times restart at 0", g_TestAidSessionCount);
        }
        continue;
      }
      if (layoutWidth > 0) {
        clickX += ((int)g_FramebufferWidth - layoutWidth) / 2;
        clickY += ((int)g_FramebufferHeight - layoutHeight) / 2;
      }
      Thandor_Log("script: %u ms click %d %d (until in game)", now, clickX, clickY);
      Win32_PushCursorEvent(MOTION_OR_WHEEL, 0, clickX, clickY);
      Win32_PushCursorEvent(LEFT_PRESS, 1, clickX, clickY);
      pendingRelease = 1;
      releaseButton = 1;
      releaseX = clickX;
      releaseY = clickY;
      releaseAt = now + 120;
      due = now + (hold > 0 ? (unsigned)hold : 3000);
      return;
    }
    state = 1;
    if (strcmp(command, "layout") == 0) {
      layoutWidth = x;
      layoutHeight = y;
      continue;
    }
    if (layoutWidth > 0) {
      x += ((int)g_FramebufferWidth - layoutWidth) / 2;
      y += ((int)g_FramebufferHeight - layoutHeight) / 2;
    }
    Thandor_Log("script: %u ms %s %d %d", now, command, x, y);
    if (strcmp(command, "click") == 0 || strcmp(command, "rclick") == 0) {
      int right = command[0] == 'r';
      /* g_MouseButtonMask bits: 1 left, 4 right */
      Win32_PushCursorEvent(MOTION_OR_WHEEL, 0, x, y);
      Win32_PushCursorEvent(right ? RIGHT_PRESS : LEFT_PRESS, right ? 4 : 1, x, y);
      pendingRelease = 1;
      releaseButton = right ? 4 : 1;
      /* the next line is read before the release is due and overwrites x, y */
      releaseX = x;
      releaseY = y;
      releaseAt = now + (hold > 0 ? (unsigned)hold : 120);
    }
    else if (strcmp(command, "move") == 0) {
      Win32_PushCursorEvent(MOTION_OR_WHEEL, 0, x, y);
    }
    else if (strcmp(command, "drag") == 0) {
      /* drag x y x2 y2: press the left button at x,y, move with it held to x2,y2 and release there */
      int toX = hold;
      int toY = extra;
      if (layoutWidth > 0) {
        toX += ((int)g_FramebufferWidth - layoutWidth) / 2;
        toY += ((int)g_FramebufferHeight - layoutHeight) / 2;
      }
      Win32_PushCursorEvent(MOTION_OR_WHEEL, 0, x, y);
      Win32_PushCursorEvent(LEFT_PRESS, 1, x, y);
      Win32_PushCursorEvent(MOTION_OR_WHEEL, 1, (x + toX) / 2, (y + toY) / 2);
      Win32_PushCursorEvent(MOTION_OR_WHEEL, 1, toX, toY);
      pendingRelease = 1;
      releaseButton = 1;
      releaseX = toX;
      releaseY = toY;
      releaseAt = now + 200;
    }
    else if (strcmp(command, "key") == 0) {
      Keyboard_OnKeyDown(x);
      Keyboard_OnKeyUp(x);
      if (g_TestAidInGameFrames != 0) {
        Thandor_Log("script: simulation step ticks now %u", (unsigned)g_InGameSimulationStepTicks);
      }
    }
    else if (strcmp(command, "quit") == 0) {
      ExitProcess(0);
    }
  }
}

/* Address: 0x005868D0.
   The game's non-blocking message pump (g_Win32PumpMessages): handles every pending message of the main
   window (TranslateMessage only where Win32_ShouldTranslateMessageFlags allows it) and returns once the
   queue is empty. WM_QUIT or a window being destroyed (g_WindowDestroyDepth) shuts the game down and
   ends the process instead. open-thandor first runs its test aids (autoshot, input script).
*/
void Win32_PumpMessages(void)

{
  Win32_AutoShotTick();
  Win32_ScriptTick();
  bool shouldTranslate;

  while (PeekMessageA((LPMSG)&g_MainMessage,g_MainWindow,0,0,PM_REMOVE) != 0) {
    if (g_WindowDestroyDepth != 0 || g_MainMessage.message == WM_QUIT)
      goto shutdown;
    shouldTranslate = Win32_ShouldTranslateMessageFlags(&g_MainMessage);
    if (shouldTranslate) {
      TranslateMessage((MSG *)&g_MainMessage);
    }
    DispatchMessageA((MSG *)&g_MainMessage);
  }
  if (g_WindowDestroyDepth == 0) {
    return;
  }
shutdown:
  Runtime_Shutdown();
  DestroyWindow(g_MainWindow);
  ExitProcess(0);
}


/* Address: 0x00577B90.
   Decides whether the message pump calls TranslateMessage (CF set): only for key-down/up messages
   (WM_KEYDOWN..WM_SYSKEYUP, not WM_CHAR/WM_DEADCHAR) of keys that produce text. The keys the engine
   handles itself (Backspace, Tab, Enter, Pause, Escape, Space through Delete, numpad and F1-F12) get no
   WM_CHAR, so text fields do not see them twice.
*/
bool Win32_ShouldTranslateMessageFlags(Win32Message32 *message)

{
  uint32_t messageCode;
  uint32_t virtualKeyCode;

  messageCode = message->message;
  virtualKeyCode = message->wParam;
  if (WM_KEYDOWN - 1 < messageCode && messageCode < WM_SYSKEYUP + 1 && messageCode != WM_DEADCHAR &&
      messageCode != WM_CHAR && virtualKeyCode != VK_BACK && virtualKeyCode != VK_TAB &&
      virtualKeyCode != VK_RETURN && virtualKeyCode != VK_PAUSE && virtualKeyCode != VK_ESCAPE &&
      (virtualKeyCode < VK_SPACE ||
       (VK_DELETE < virtualKeyCode && (virtualKeyCode < VK_NUMPAD0 || VK_F12 < virtualKeyCode)))) {
    return true;
  }
  return false;
}


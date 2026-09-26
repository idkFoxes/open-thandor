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

/* Address: 0x005868D0.
   Ownership: platform/system/win32.
   Purpose: Nonblocking central message pump. Repeatedly calls PeekMessageA for g_MainWindow with PM_REMOVE,
   conditionally calls TranslateMessage according to Win32_ShouldTranslateMessageFlags, and always dispatches the
   message. It returns when the filtered queue is empty and no destroy is pending. WM_QUIT or a nonzero
   g_WindowDestroyDepth triggers Runtime_Shutdown, DestroyWindow, and ExitProcess(0).
   Local calls: Win32_ShouldTranslateMessageFlags.
   Cross-module calls: Runtime_Shutdown [core/memory/synchronization].
*/
/* Test aid: OPEN_THANDOR_AUTOSHOT=<milliseconds> saves the game's own framebuffer to
   shots\shot_NNNN.bmp at that interval (checked from the message pump), so automated runs can be
   looked at without capturing the desktop. */
static void Win32_AutoShotTick(void)
{
  static int interval = -1;
  static unsigned last;
  static unsigned number;
  unsigned now;
  GraphicsFramebufferCaptureEaxCf5 capture;
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
  capture = (*g_GraphicsFramebufferCaptureRegion)(g_FramebufferHeight,g_FramebufferWidth,0,0);
  if (!capture.carry && capture.eax != NULL) {
    GraphicsTextureSourceEntry *entry = &capture.eax->sourceEntry;
    const dword *pixels = (const dword *)((byte *)capture.eax + entry->dataOffset);
    dword width = entry->pixelWidth;
    dword height = entry->pixelHeight;
    char name[64];
    FILE *file;
    sprintf(name, "shots\\shot_%04u.bmp", number++);
    file = fopen(name, "wb");
    if (file != NULL) {
      dword header[13];
      dword imageBytes = width * height * 4;
      memset(header, 0, sizeof header);
      fwrite("BM", 1, 2, file);
      header[0] = 54 + imageBytes;
      header[2] = 54;
      header[3] = 40;
      header[4] = width;
      header[5] = (dword)-(int)height;
      header[6] = 1 | (32 << 16);
      header[8] = imageBytes;
      fwrite(header, 4, 13, file);
      fwrite(pixels, 4, width * height, file);
      fclose(file);
      Thandor_Log("autoshot %s (%ux%u)", name, width, height);
    }
    (*g_MemoryApi.free)(capture.eax);
  }
}

/* Test aid: OPEN_THANDOR_SCRIPT=<file> replays timed input from a text file, one command per line:
     <ms> click <x> <y> [hold]  left press at framebuffer pixel x,y, release after hold ms (120)
     <ms> rclick <x> <y>    the same with the right button
     <ms> move <x> <y>      pointer motion
     <ms> key <vk>          key press and release (Windows virtual-key code, decimal)
     <ms> shot              save the framebuffer now (shots\script_NNNN.bmp, needs AUTOSHOT's folder)
     <ms> quit              end the process
     <ms> layout <w> <h>    the following coordinates are for a w x h screen; they are moved by half
                            the difference to the current resolution (dialogs and the view are centred)
     <ms> ingame            wait until the level has loaded and the game runs; the times of the
                            following lines count from that moment
   <ms> counts from the first message pump. Pointer events go into the same ring DirectInput fills. */
volatile unsigned g_TestAidInGameFrames;

static void Win32_PushCursorEvent(GraphicsCursorEventType type, dword buttons, int x, int y)
{
  dword index = g_CursorInputWriteIndex;
  dword next = index + 1;
  if (0xff < next) {
    next = 0;
  }
  g_MouseX = x;
  g_MouseY = y;
  g_MouseButtonMask = buttons;
  g_CursorInputEvents[index].eventType00 = type;
  g_CursorInputEvents[index].buttonState04 = buttons;
  g_CursorInputEvents[index].pointerX08 = x;
  g_CursorInputEvents[index].pointerY0C = y;
  g_CursorInputEvents[index].wheelDelta10 = 0;
  g_CursorInputEvents[index].clockValue14 = g_CursorInputClockValue;
  g_CursorInputWriteIndex = next;
}

static void Win32_ScriptTick(void)
{
  static FILE *script;
  static int state = -1;
  static unsigned start;
  static unsigned due;
  static char command[16];
  static int x;
  static int y;
  static int hold;
  static int pendingRelease;
  static dword releaseButton;
  static unsigned releaseAt;
  static int releaseX;
  static int releaseY;
  static int layoutWidth;
  static int layoutHeight;
  char line[128];
  unsigned now;
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
      x = y = hold = 0;
      if (sscanf(line, "%u %15s %d %d %d", &due, command, &x, &y, &hold) < 2) {
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
    else if (strcmp(command, "key") == 0) {
      Keyboard_OnKeyDown(x);
      Keyboard_OnKeyUp(x);
    }
    else if (strcmp(command, "quit") == 0) {
      ExitProcess(0);
    }
  }
}

void __thandor_void_preserve_eax_ecx_edx Win32_PumpMessages(void)

{
  Win32_AutoShotTick();
  Win32_ScriptTick();
  BOOL messageAvailable;
  bool shouldTranslateMessage;
  bool shouldTranslate;
  
  while( true ) {
    messageAvailable = PeekMessageA((LPMSG)&g_MainMessage,g_MainWindow,0,0,1);
    if (messageAvailable == 0) break;
    if ((g_WindowDestroyDepth != 0) || (g_MainMessage.message == 0x12))
    goto Win32_PumpMessages_ShutdownDestroyWindowAndExitAfterQuitOrDestroyRequest;
    shouldTranslate = Win32_ShouldTranslateMessageFlags(&g_MainMessage);
    if (shouldTranslate) {
      TranslateMessage((MSG *)&g_MainMessage);
    }
    DispatchMessageA((MSG *)&g_MainMessage);
  }
  if (g_WindowDestroyDepth == 0) {
    return;
  }
Win32_PumpMessages_ShutdownDestroyWindowAndExitAfterQuitOrDestroyRequest:
  Runtime_Shutdown();
  DestroyWindow(g_MainWindow);
                    // WARNING: Subroutine does not return
  ExitProcess(0);
}


/* Address: 0x00577B90.
   Ownership: platform/system/win32.
   Purpose: Examines a Win32 MSG before TranslateMessage. CF set means the pump should call TranslateMessage. CF
   clear means translation is suppressed. WM_CHAR/WM_DEADCHAR and the engine's directly handled editing,
   navigation, digit, letter, and function-key ranges are suppressed to avoid duplicate character messages.
*/
bool __thandor_void_preserve_eax_ecx Win32_ShouldTranslateMessageFlags(Win32Message32 *message)

{
  uint messageCode;
  uint virtualKeyCode;
  
  messageCode = message->message;
  virtualKeyCode = message->wParam;
  if (((((((0xff < messageCode) && (messageCode < 0x106)) && (messageCode != 0x103)) &&
        ((messageCode != 0x102 && (virtualKeyCode != 8)))) &&
       ((virtualKeyCode != 9 && ((virtualKeyCode != 0xd && (virtualKeyCode != 0x13)))))) &&
      (virtualKeyCode != 0x1b)) &&
     ((virtualKeyCode < 0x20 ||
      ((0x2e < virtualKeyCode && ((virtualKeyCode < 0x60 || (0x7b < virtualKeyCode)))))))) {
    return true;
  }
  return false;
}


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

void __thandor_void_preserve_eax_ecx_edx Win32_PumpMessages(void)

{
  Win32_AutoShotTick();
  BOOL messageAvailable;
  bool shouldTranslateMessage;
  bool bVar1;
  
  while( true ) {
    messageAvailable = PeekMessageA((LPMSG)&g_MainMessage,g_MainWindow,0,0,1);
    if (messageAvailable == 0) break;
    if ((g_WindowDestroyDepth != 0) || (g_MainMessage.message == 0x12))
    goto Win32_PumpMessages_ShutdownDestroyWindowAndExitAfterQuitOrDestroyRequest;
    bVar1 = Win32_ShouldTranslateMessageFlags(&g_MainMessage);
    if (bVar1) {
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


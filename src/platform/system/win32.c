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
#include <thandor/platform/debug/hooks.h>

/* Module data. */

Win32PumpMessagesProc *g_Win32PumpMessages = 0;

uint32_t g_WindowDestroyDepth = 0;

/* Implementation ownership: platform/system/win32. */

/* Shuts the game down, destroys the main window and ends the process (does not return). */
static void Win32_ShutdownAndExit(void)
{
  Runtime_Shutdown();
  DestroyWindow(g_MainWindow);
  ExitProcess(0);
}

/* The game's non-blocking message pump (g_Win32PumpMessages): handles every pending message of the main
   window (TranslateMessage only where Win32_ShouldTranslateMessageFlags allows it) and returns once the
   queue is empty. WM_QUIT or a window being destroyed (g_WindowDestroyDepth) shuts the game down and
   ends the process instead. With the developer tools open-thandor first runs its automatic screenshots and the
   input script.
*/
void Win32_PumpMessages(void)

{
  DebugHook_MessagePump();

  while (PeekMessageA((LPMSG)&g_MainMessageStorage.message,g_MainWindow,0,0,PM_REMOVE) != 0) {
    if (g_WindowDestroyDepth != 0 || g_MainMessageStorage.message.message == WM_QUIT) {
      Win32_ShutdownAndExit();
      return;
    }
    if (Win32_ShouldTranslateMessageFlags(&g_MainMessageStorage.message)) {
      TranslateMessage((MSG *)&g_MainMessageStorage.message);
    }
    DispatchMessageA((MSG *)&g_MainMessageStorage.message);
  }
  if (g_WindowDestroyDepth != 0) {
    Win32_ShutdownAndExit();
  }
}


/* Decides whether the message pump calls TranslateMessage (returns true): only for key-down/up messages
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


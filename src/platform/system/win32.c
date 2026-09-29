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
#include <thandor/platform/debug/autoshot.h>
#include <thandor/platform/debug/script.h>

/* Implementation ownership: platform/system/win32. */

/* Address: 0x005868D0.
   The game's non-blocking message pump (g_Win32PumpMessages): handles every pending message of the main
   window (TranslateMessage only where Win32_ShouldTranslateMessageFlags allows it) and returns once the
   queue is empty. WM_QUIT or a window being destroyed (g_WindowDestroyDepth) shuts the game down and
   ends the process instead. open-thandor first runs its test aids (autoshot, input script).
*/
void Win32_PumpMessages(void)

{
  DebugAutoShot_Tick();
  DebugScript_Tick();
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


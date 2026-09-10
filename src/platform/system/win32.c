#include <thandor/platform/system/win32.h>

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
void __cdecl Win32_PumpMessages(void)

{
  BOOL messageAvailable;
  bool shouldTranslateMessage;
  
  while( true ) {
    messageAvailable = PeekMessageA((LPMSG)&g_MainMessage,g_MainWindow,0,0,1);
    if (messageAvailable == 0) break;
    if ((g_WindowDestroyDepth != 0) ||
       (shouldTranslateMessage = g_MainMessage.message < 0x12, g_MainMessage.message == 0x12))
    goto Win32_PumpMessages_ShutdownDestroyWindowAndExitAfterQuitOrDestroyRequest;
    Win32_ShouldTranslateMessageFlags(&g_MainMessage);
    if (shouldTranslateMessage) {
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
                    
  ExitProcess(0);
}

/* Address: 0x00577B90.
   Ownership: platform/system/win32.
   Purpose: Examines a Win32 MSG before TranslateMessage. CF set means the pump should call TranslateMessage. CF
   clear means translation is suppressed. WM_CHAR/WM_DEADCHAR and the engine's directly handled editing,
   navigation, digit, letter, and function-key ranges are suppressed to avoid duplicate character messages.
*/
void Win32_ShouldTranslateMessageFlags(Win32Message32 *message)

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
    return;
  }
  return;
}

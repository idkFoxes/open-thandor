/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/src/core/error/runtime.cpp
 * Reverse engineering by idkFoxes 2026
 */

#include <thandor/core/error/runtime.h>
#include <thandor/thandor.h>
#include <thandor/platform/bootstrap/image.h>

/* Module data. */

THANDOR_ALIGN(4) uint16_t g_FatalErrorDetail1Utf16[256] = {0};

uint16_t g_FatalErrorDetail2Utf16[256] = {0};

uint16_t g_FatalErrorDetail3Utf16[256] = {0};

/* "texte\\error.str" */
static uint16_t g_TexteErrorStrPathUtf16[16] = {'t', 'e', 'x', 't', 'e', '\\', 'e', 'r', 'r', 'o', 'r', '.', 's', 't', 'r'};

static uint8_t g_FatalErrorNarrowBuffer[1024] = {0};

FatalErrorPassThroughProc *g_FatalErrorExitHandler = nullptr;

FatalErrorPassThroughProc *g_FatalErrorReportHandler = nullptr;

/* "error: IO: initialization failed!" */
uint16_t g_ErrorTextIoInitializationFailed[34] = {'e', 'r', 'r', 'o', 'r', ':', ' ', 'I', 'O', ':', ' ', 'i', 'n', 'i', 't', 'i', 'a',
                                                  'l', 'i', 'z', 'a', 't', 'i', 'o', 'n', ' ', 'f', 'a', 'i', 'l', 'e', 'd', '!'};

/* Points both fatal-error handlers at FatalError_Exit (the UI dialog handler is installed later) and
   loads the error texts (texte\error.str) as text page 0. If they cannot be loaded the game exits with
   the built-in I/O error message; otherwise FatalError_Exit returns at once because failed is false.
*/
void ErrorSystem_Init()

{
  Bool8 errorTextsLoaded;

  g_FatalErrorExitHandler = FatalError_Exit;
  g_FatalErrorReportHandler = FatalError_Exit;
  errorTextsLoaded = TextResourcePage_Load(0,g_TexteErrorStrPathUtf16,nullptr);
  FatalError_Exit(THANDOR_ADDR(g_ErrorTextIoInitializationFailed,0),!errorTextsLoaded);
}

/* The fatal-error handler: without failed it returns valueOrError unchanged; with failed it is
   FatalError_ShowAndExit(valueOrError) and does not return.
*/
uintptr_t FatalError_Exit(uintptr_t valueOrError,Bool8 failed)

{
  if (!failed) {
    return valueOrError;
  }
  /* from here on valueOrError (see FatalErrorPassThroughProc) is the error code or rich-text stream */
  FatalError_ShowAndExit(valueOrError);
}

/* The failing half of FatalError_Exit: builds the error message (a code below 0x100 selects a text of the
   error page, anything else is a rich-text stream), fills in the last path and the three detail strings,
   shuts everything down, shows the text in a message box and exits the process.
*/
void FatalError_ShowAndExit(uintptr_t error)

{
  uint16_t *messageText;

  /* open-thandor diagnostics: fatal error code, last package path and the calling stack */
  Thandor_Log("fatal error 0x%08IX, last path \"%ls\"", error, (wchar_t *)g_PackageLastErrorPath);
  Thandor_LogStack("fatal error stack", (unsigned)error);
  if (FATAL_ERROR_IS_CODE(error)) {
    messageText = TextResource_Resolve((uint32_t)error);
  }
  else {
    messageText = (uint16_t *)error;
  }
  /* payload selectors 0..3 of the message text */
  RichTextCommandStream_PatchPayloadBySelector(0,g_PackageLastErrorPath,messageText);
  RichTextCommandStream_PatchPayloadBySelector(1,g_FatalErrorDetail1Utf16,messageText);
  RichTextCommandStream_PatchPayloadBySelector(2,g_FatalErrorDetail2Utf16,messageText);
  RichTextCommandStream_PatchPayloadBySelector(3,g_FatalErrorDetail3Utf16,messageText);
  FatalError_CopyRichTextToNarrow(sizeof g_FatalErrorNarrowBuffer,g_FatalErrorNarrowBuffer,messageText);
  Runtime_Shutdown();
  DestroyWindow(g_MainWindow);
  MessageBoxA(nullptr,(LPCSTR)g_FatalErrorNarrowBuffer,nullptr,MB_ICONEXCLAMATION);
  ExitProcess(0);
}

/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/src/core/error/runtime.c
 * Reverse engineering by idkFoxes 2026
 */

#include <thandor/core/error/runtime.h>
#include <thandor/thandor.h>
#include <thandor/platform/bootstrap/image.h>

/* Implementation ownership: core/error/runtime. */

/* Address: 0x00575890.
   Points all three fatal-error handlers at FatalError_Exit (the UI dialog handler is installed later) and
   loads the error texts (texte\error.str) as text page 0. If they cannot be loaded the game exits with
   the built-in I/O error message; otherwise FatalError_Exit returns at once because failed is false.
*/
void __cdecl ErrorSystem_Init(void)

{
  bool errorTextsLoaded;

  g_FatalErrorExitHandler = FatalError_Exit;
  g_FatalErrorReportHandler = FatalError_Exit;
  g_FatalErrorFallbackHandler = FatalError_Exit;
  errorTextsLoaded = TextResourcePage_Load(0,u_texte_error_str_00407d20,NULL);
  FatalError_Exit(THANDOR_ADDR(g_ErrorTextIoInitializationFailed,0),!errorTextsLoaded);
}


/* Address: 0x00407F50.
   method08 of g_UiRootCallbacks_00407E28, the callbacks of the fatal-error dialog root: always sets CF, so
   a pointer event that misses the dialog ends the root-stack hit test there instead of reaching the roots
   below (the dialog is modal).
*/
bool FatalErrorDialog_BlockMissedPointerPress(UiRootNode *root)

{
  return true;
}


/* Address: 0x00407F60.
   pointerMissPolicy of g_UiRootCallbacks_00407E28 (the fatal-error dialog root): the non-negative result
   stops the pointer traversal at the dialog, so the roots below it get no pointer input. The value 8 itself
   carries no meaning beyond being non-negative.
*/
int FatalErrorDialog_BlockMissedPointerMotion(UiRootNode *root)

{
  return 8;
}


/* Address: 0x00407F70.
   Handler of UI action 1 (slot 1 of g_UiRootStackActionHandlerPage, installed as action page 0 by the UI
   setup in ui/controls/layout.c): closes the fatal-error dialog root and counts the dismissal, which ends
   the modal frame loop in FatalErrorRuntime_DispatchPendingError.
*/
void FatalErrorDialog_DismissAndPopRoot(UiRootNode *rootNode)

{
  UiRootStack_Pop(rootNode);
  g_FatalErrorDialogDismissed++;
  return;
}


/* Address: 0x00407F90.
   The in-game fatal-error handler behind FatalError_ReportIfFailed (installed by
   ErrorRuntime_InstallUiHandlerAndAllocateState): returns valueOrError unchanged. When failed is set it first
   builds the message like FatalError_Exit, opens it as a modal dialog sized to the text and runs UI frames
   until the dialog is dismissed, so the caller can carry on (the caller knows the failure from its own flag).
*/
uint32_t FatalErrorRuntime_DispatchPendingError(uint32_t valueOrError,bool failed)

{
  UiRootNode *dialogRoot;
  uint16_t *stream;
  int remainingDwords;
  const uint32_t *templateImageCursor;
  uint32_t *templateCopyCursor;
  RichTextExtent wrappedExtent;
  uint32_t error;

  if (!failed) {
    return valueOrError;
  }
  error = valueOrError;
  if (g_FatalErrorUiRootTemplate == NULL) {
    /* no dialog state allocated yet: FatalError_Exit, which does not return */
    error = FatalError_ExitIfFailed(error,true);
  }
  stream = (uint16_t *)error;
  if (FATAL_ERROR_IS_CODE(error)) {
    /* only resolved error-page texts get the payload selectors 0..3 patched (FatalError_Exit patches
       every stream) */
    stream = TextResource_Resolve(error);
    RichTextCommandStream_PatchPayloadBySelector(0,g_PackageLastErrorPath,stream);
    RichTextCommandStream_PatchPayloadBySelector(1,g_FatalErrorDetail1Utf16,stream);
    RichTextCommandStream_PatchPayloadBySelector(2,&g_FatalErrorDetail2Utf16,stream);
    RichTextCommandStream_PatchPayloadBySelector(3,&g_FatalErrorDetail3Utf16,stream);
  }
  g_FatalErrorRichTextStream = (uint32_t)stream;
  /* copy the dialog template image into the allocated root node, one dword per step (REP MOVSD) */
  templateImageCursor = g_FatalErrorUiRootTemplateImage;
  templateCopyCursor = (uint32_t *)g_FatalErrorUiRootTemplate;
  for (remainingDwords = sizeof g_FatalErrorUiRootTemplateImage / sizeof(uint32_t); remainingDwords != 0;
       remainingDwords--) {
    *templateCopyCursor = *templateImageCursor;
    templateImageCursor++;
    templateCopyCursor++;
  }
  dialogRoot = g_FatalErrorUiRootTemplate;
  /* the text height is subtracted from the dialog's top offset */
  wrappedExtent = RichTextCommandStream_MeasureWrappedBlock
                    (g_UiTextStyleNormal,(uint16_t *)g_FatalErrorRichTextStream,
                     ((g_FatalErrorRichTextRight - g_FatalErrorRichTextLeft) +
                     g_FatalErrorRichTextBottom) - g_FatalErrorRichTextTop);
  dialogRoot->base.topOffset = dialogRoot->base.topOffset - wrappedExtent.heightPixels;
  UiRootStack_Push(&g_UiRootCallbacks_00407E28,g_FatalErrorUiRootTemplate);
  g_UiPointerCaptureTarget = UI_NODE_NONE;
  g_UiPointerCaptureButton = UI_POINTER_CAPTURE_NONE;
  UiFrame_FlushInputAndResetPendingTicks();
  g_GraphicsCursorSetFrame(0);
  g_FatalErrorDialogDismissed = 0;
  do {
    UiRootStack_InvalidateAll();
    UiFrame_ProcessAndPresentWithLockTransition();
  } while (g_FatalErrorDialogDismissed == 0);
  return error;
}


/* Address: 0x00408090.
   Allocates the 0x110-byte root node of the fatal-error dialog (FatalErrorRuntime_DispatchPendingError
   fills it from g_FatalErrorUiRootTemplateImage) and, if that worked, switches FatalError_ReportIfFailed
   from FatalError_Exit to the in-game dialog. Without the allocation errors keep ending the process.
*/
void ErrorRuntime_InstallUiHandlerAndAllocateState(void)

{
  void *allocPayload;

  if (g_MemoryApi.alloc(sizeof g_FatalErrorUiRootTemplateImage,&allocPayload) == 0) {
    g_FatalErrorReportHandler = FatalErrorRuntime_DispatchPendingError;
    g_FatalErrorUiRootTemplate = (UiRootNode *)allocPayload;
  }
  return;
}


/* Address: 0x0041BC50.
   Byte-for-byte twin of Text_CopyNarrowToUtf16: widens a NUL-terminated 8-bit string to UTF-16 into a
   buffer of capacityBytes bytes and returns the bytes written including the terminator (always at least 2);
   a string that does not fit is cut off and terminated, and 0 is returned (the original reported
   FATAL_ERROR_GENERAL_FAILURE with CF set). Nothing in this code base calls it and no callback-table slot
   references it.
*/
uint32_t FatalError_CopyNarrowToUtf16(TextOutputCapacityBytes capacityBytes,uint16_t *destination,uint8_t *source)

{
  uint8_t sourceByte;
  TextOutputCapacityBytes remainingCapacityBytes;
  bool capacityExhausted;

  remainingCapacityBytes = capacityBytes;
  do {
    sourceByte = *source;
    /* SUB ECX,2 / JBE: fails once the capacity would reach zero, so one unit always stays unused */
    capacityExhausted = remainingCapacityBytes < 2;
    remainingCapacityBytes = remainingCapacityBytes - 2;
    if (capacityExhausted || remainingCapacityBytes == 0) {
      destination[-1] = 0;
      return 0;
    }
    *destination = (uint16_t)sourceByte;
    source++;
    destination++;
  } while (sourceByte != 0);
  return capacityBytes - remainingCapacityBytes;
}


/* Address: 0x005758D0.
   The fatal-error handler: without failed it returns valueOrError unchanged; with failed it builds the error
   message (a code below 0x100 selects a text of the error page, anything else is a rich-text stream), fills
   in the last path and the three detail strings, shuts everything down, shows the text in a message box and
   exits the process (it does not return then).
*/
uint32_t FatalError_Exit(uint32_t valueOrError,bool failed)

{
  uint32_t error;
  uint16_t *messageText;

  if (!failed) {
    return valueOrError;
  }
  error = valueOrError;
  /* open-thandor diagnostics: fatal error code, last package path and the calling stack */
  Thandor_Log("fatal error 0x%08X, last path \"%ls\"", error, (wchar_t *)g_PackageLastErrorPath);
  Thandor_LogStack("fatal error stack", error);
  if (FATAL_ERROR_IS_CODE(error)) {
    messageText = TextResource_Resolve(error);
  }
  else {
    messageText = (uint16_t *)error;
  }
  /* payload selectors 0..3 of the message text */
  RichTextCommandStream_PatchPayloadBySelector(0,g_PackageLastErrorPath,messageText);
  RichTextCommandStream_PatchPayloadBySelector(1,g_FatalErrorDetail1Utf16,messageText);
  RichTextCommandStream_PatchPayloadBySelector(2,&g_FatalErrorDetail2Utf16,messageText);
  RichTextCommandStream_PatchPayloadBySelector(3,&g_FatalErrorDetail3Utf16,messageText);
  FatalError_CopyRichTextToNarrow(sizeof g_FatalErrorNarrowBuffer,g_FatalErrorNarrowBuffer,messageText);
  Runtime_Shutdown();
  DestroyWindow(g_MainWindow);
  MessageBoxA(NULL,(LPCSTR)g_FatalErrorNarrowBuffer,NULL,MB_ICONEXCLAMATION);
  ExitProcess(0);
}


/* Failure tail of FatalError_CopyRichTextToNarrow: terminates the cut output on the last byte written
   (destination points behind it) and reports the failure. */
static int FatalError_TerminateCutNarrowText(uint8_t *destination)
{
  destination[-1] = 0;
  return FATAL_ERROR_GENERAL_FAILURE;
}

/* Address: 0x0041BB00.
   Converts a rich-text command stream into plain narrow text for the fatal-error MessageBoxA: glyphs below
   0x100 are copied as bytes, fixed spaces become ' ', line breaks CR LF, nested streams are followed and
   every other command is skipped. Returns the bytes written including the terminator, or
   FATAL_ERROR_GENERAL_FAILURE (output cut and terminated) when capacityBytes runs out.
*/
int FatalError_CopyRichTextToNarrow
              (TextOutputCapacityBytes capacityBytes,uint8_t *destination,uint16_t *source)

{
  uint16_t *command;
  uint16_t *record;
  uint16_t *operand;
  uint16_t codeUnit;
  uint32_t remainingCapacityBytes;
  bool newlineCapacityUnderflow;
  uint16_t *nestedReturnStack[FATAL_ERROR_RICHTEXT_NESTING_MAX]; /* the original's machine-stack chain */
  int nestedDepth;

  nestedDepth = 0;
  remainingCapacityBytes = capacityBytes;
  command = source;
  /* a 0 code unit ends the current stream; the outermost one ends the text */
  while (*command != 0 || nestedDepth != 0) {
    codeUnit = *command;
    if (codeUnit == 0) {
      /* end of a nested stream: continue behind the payload of its call record */
      command = (uint16_t *)((uint8_t *)nestedReturnStack[--nestedDepth] + RICHTEXT_NESTED_PAYLOAD_BYTES);
      continue;
    }
    record = command;
    operand = record + 1;
    /* by default a code unit (glyph or unknown command) is one unit long */
    command = operand;
    if ((short)codeUnit < 0) {
      switch(codeUnit & RICHTEXT_OPCODE_MASK) {
      case RICHTEXT_OP_LITERAL_COLOR:
        command = record + RICHTEXT_RECORD_UNITS_LITERAL_COLOR;
        break;
      case RICHTEXT_OP_FIXED_SPACE:
        remainingCapacityBytes--;
        if (remainingCapacityBytes == 0) {
          return FatalError_TerminateCutNarrowText(destination);
        }
        *destination = ' ';
        destination++;
        break;
      case RICHTEXT_OP_LINE_BREAK:
        newlineCapacityUnderflow = remainingCapacityBytes < 2;
        remainingCapacityBytes = remainingCapacityBytes - 2;
        if (newlineCapacityUnderflow || remainingCapacityBytes == 0) {
          return FatalError_TerminateCutNarrowText(destination);
        }
        destination[0] = '\r';
        destination[1] = '\n';
        destination = destination + 2;
        break;
      case RICHTEXT_OP_INLINE_VALUE_0:
      case RICHTEXT_OP_INLINE_VALUE_1:
      case RICHTEXT_OP_INLINE_VALUE_2:
        command = record + RICHTEXT_RECORD_UNITS_INLINE_VALUE;
        break;
      case RICHTEXT_OP_CALL_NESTED:
        if (nestedDepth == FATAL_ERROR_RICHTEXT_NESTING_MAX) {
          return FatalError_TerminateCutNarrowText(destination);
        }
        nestedReturnStack[nestedDepth++] = operand;
        command = *(uint16_t **)operand;
        break;
      case RICHTEXT_OP_JUMP_NESTED:
        command = *(uint16_t **)operand;
        break;
      case RICHTEXT_OP_INLINE_IMAGE:
        command = record + RICHTEXT_RECORD_UNITS_INLINE_IMAGE;
        break;
      }
    }
    else if ((codeUnit & 0xff00) == 0) { /* only glyphs that fit a narrow character */
      remainingCapacityBytes--;
      if (remainingCapacityBytes == 0) {
        return FatalError_TerminateCutNarrowText(destination);
      }
      *destination = (uint8_t)codeUnit;
      destination++;
    }
  }
  if (0 < (int)remainingCapacityBytes) {
    *destination = 0;
    return capacityBytes - (remainingCapacityBytes - 1);
  }
  return FatalError_TerminateCutNarrowText(destination);
}

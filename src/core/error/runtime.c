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
   the built-in I/O error message; otherwise FatalError_Exit returns at once because the failure flag is clear.
*/
void __cdecl ErrorSystem_Init(void)

{
  TextPageLoadResult loadResult;

  g_FatalErrorExitHandler = FatalError_Exit;
  g_FatalErrorReportHandler = FatalError_Exit;
  g_FatalErrorFallbackHandler = FatalError_Exit;
  loadResult = TextResourcePage_Load(0,u_texte_error_str_00407d20);
  FatalError_Exit(THANDOR_ADDR(g_ErrorTextIoInitializationFailed,0),loadResult.failed);
}


/* Address: 0x00407F50.
   method08 of g_UiRootCallbacks_00407E28, the callbacks of the fatal-error dialog root: always sets CF, so
   a pointer event that misses the dialog ends the root-stack hit test there instead of reaching the roots
   below (the dialog is modal).
*/
bool ErrorRuntime_CallbackAlwaysFail(UiRootNode *root)

{
  return true;
}


/* Address: 0x00407F60.
   pointerMissPolicy of g_UiRootCallbacks_00407E28 (the fatal-error dialog root): the non-negative result
   stops the pointer traversal at the dialog, so the roots below it get no pointer input. The value 8 itself
   carries no meaning beyond being non-negative.
*/
int ErrorRuntime_CallbackReturnCode8(UiRootNode *root)

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
   ErrorRuntime_InstallUiHandlerAndAllocateState): with CF clear it passes EAX through; with CF set it builds
   the message like FatalError_Exit, opens it as a modal dialog sized to the text and runs UI frames until the
   dialog is dismissed, then returns the error with CF set so the caller can carry on.
   Original register convention: EAX and CF are passed in and returned (CF set = failure); ECX and EDX preserved.
*/
FatalErrorCheckResult FatalErrorRuntime_DispatchPendingError(uint32_t errorOrValue,bool carryIn)

{
  int32_t *topOffsetField;
  UiRootNode *dialogRoot;
  uint16_t *stream;
  int remainingDwords;
  uint32_t *templateImageCursor;
  UiRootNode *templateCopyCursor;
  RichTextExtentRegs wrappedExtent;
  FatalErrorCheckResult passThroughResult;
  FatalErrorCheckResult dispatchResult;
  TextResolveResult resolvedText;

  if (!carryIn) {
    passThroughResult.failed = false;
    passThroughResult.valueOrError = errorOrValue;
    return passThroughResult;
  }
  if (g_FatalErrorUiRootTemplate == NULL) {
    /* no dialog state allocated yet: FatalError_Exit, which does not return */
    dispatchResult = FatalError_ExitIfFailed(errorOrValue,true);
    errorOrValue = dispatchResult.valueOrError;
  }
  stream = (uint16_t *)errorOrValue;
  if ((errorOrValue & 0xffffff00) == 0) { /* an error code, not a text pointer */
    resolvedText = TextResource_Resolve(errorOrValue);
    stream = resolvedText.text;
    RichTextCommandStream_PatchPayloadBySelector(0,g_PackageLastErrorPath,stream);
    RichTextCommandStream_PatchPayloadBySelector(1,g_FatalErrorDetail1Utf16,stream);
    RichTextCommandStream_PatchPayloadBySelector(2,&g_FatalErrorDetail2Utf16,stream);
    RichTextCommandStream_PatchPayloadBySelector(3,&g_FatalErrorDetail3Utf16,stream);
  }
  templateImageCursor = g_FatalErrorUiRootTemplateImage;
  templateCopyCursor = g_FatalErrorUiRootTemplate;
  g_FatalErrorRichTextStream = stream;
  /* copy the dialog template image into the allocated root node, one dword per step (REP MOVSD) */
  for (remainingDwords = sizeof g_FatalErrorUiRootTemplateImage / sizeof(uint32_t);
       dialogRoot = g_FatalErrorUiRootTemplate, remainingDwords != 0; remainingDwords--) {
    (templateCopyCursor->base).nextSibling = (UiNodeBase *)*templateImageCursor;
    templateImageCursor = templateImageCursor + 1;
    templateCopyCursor = (UiRootNode *)&(templateCopyCursor->base).firstChild;
  }
  /* the text height is subtracted from the dialog's top offset */
  wrappedExtent = RichTextCommandStream_MeasureWrappedBlockRegs
                    (g_UiTextStyleNormal,g_FatalErrorRichTextStream,
                     ((g_FatalErrorRichTextRight - g_FatalErrorRichTextLeft) +
                     g_FatalErrorRichTextBottom) - g_FatalErrorRichTextTop);
  topOffsetField = &(dialogRoot->base).topOffset;
  *topOffsetField = *topOffsetField - wrappedExtent.heightPixels;
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
  dispatchResult.failed = true;
  dispatchResult.valueOrError = errorOrValue;
  return dispatchResult;
}


/* Address: 0x00408090.
   Allocates the 0x110-byte root node of the fatal-error dialog (FatalErrorRuntime_DispatchPendingError
   fills it from g_FatalErrorUiRootTemplateImage) and, if that worked, switches FatalError_ReportIfFailed
   from FatalError_Exit to the in-game dialog. Without the allocation errors keep ending the process.
*/
void __fastcall ErrorRuntime_InstallUiHandlerAndAllocateState(void)

{
  ArenaAllocResult allocResult;

  allocResult = g_MemoryApi.alloc(sizeof g_FatalErrorUiRootTemplateImage);
  if (!allocResult.failed) {
    g_FatalErrorReportHandler = FatalErrorRuntime_DispatchPendingError;
    g_FatalErrorUiRootTemplate = (UiRootNode *)allocResult.payloadOrError;
  }
  return;
}


/* Address: 0x0041BC50.
   Byte-for-byte twin of Text_CopyNarrowToUtf16: widens a NUL-terminated 8-bit string to UTF-16 into a
   buffer of capacityBytes bytes and returns the bytes written including the terminator; a string that does
   not fit is cut off and terminated, and CF is set with FATAL_ERROR_GENERAL_FAILURE. Nothing in this code
   base calls it and no callback-table slot references it.
*/
StatusResult FatalError_CopyNarrowToUtf16(TextOutputCapacityBytes capacityBytes,uint16_t *destination,uint8_t *source)

{
  uint8_t sourceByte;
  TextOutputCapacityBytes remainingCapacityBytes;
  bool capacityExhausted;
  StatusResult successResult;
  StatusResult overflowResult;
  
  remainingCapacityBytes = capacityBytes;
  do {
    sourceByte = *source;
    /* SUB ECX,2 / JBE: fails once the capacity would reach zero, so one unit always stays unused */
    capacityExhausted = remainingCapacityBytes < 2;
    remainingCapacityBytes = remainingCapacityBytes - 2;
    if (capacityExhausted || remainingCapacityBytes == 0) {
      destination[-1] = 0;
      overflowResult.failed = true;
      overflowResult.valueOrError = FATAL_ERROR_GENERAL_FAILURE;
      return overflowResult;
    }
    *destination = (uint16_t)sourceByte;
    source++;
    destination++;
  } while (sourceByte != 0);
  successResult.valueOrError = capacityBytes - remainingCapacityBytes;
  successResult.failed = false;
  return successResult;
}


/* Address: 0x005758D0.
   The fatal-error handler: with CF clear it passes EAX through; with CF set it builds the error message
   (a code below 0x100 selects a text of the error page, anything else is a rich-text stream), fills in the
   last path and the three detail strings, shuts everything down, shows the text in a message box and
   exits the process.
   Original register convention: EAX and CF are passed in and returned (CF set = failure); ECX and EDX preserved.
*/
FatalErrorCheckResult FatalError_Exit(uint32_t errorOrValue,bool carryIn)

{
  FatalErrorCheckResult passThroughResult;
  TextResolveResult resolvedText;
  
  if (!carryIn) {
    passThroughResult.failed = false;
    passThroughResult.valueOrError = errorOrValue;
    return passThroughResult;
  }
  /* open-thandor diagnostics: fatal error code, last package path and the calling stack */
  Thandor_Log("fatal error 0x%08X, last path \"%ls\"", errorOrValue, (wchar_t *)g_PackageLastErrorPath);
  Thandor_LogStack("fatal error stack", errorOrValue);
  if ((errorOrValue & 0xffffff00) == 0) { /* an error code, not a text pointer */
    resolvedText = TextResource_Resolve(errorOrValue);
    errorOrValue = (uint32_t)resolvedText.text;
  }
  /* payload selectors 0..3 of the message text */
  RichTextCommandStream_PatchPayloadBySelector(0,g_PackageLastErrorPath,(uint16_t *)errorOrValue);
  RichTextCommandStream_PatchPayloadBySelector(1,g_FatalErrorDetail1Utf16,(uint16_t *)errorOrValue);
  RichTextCommandStream_PatchPayloadBySelector(2,&g_FatalErrorDetail2Utf16,(uint16_t *)errorOrValue);
  RichTextCommandStream_PatchPayloadBySelector(3,&g_FatalErrorDetail3Utf16,(uint16_t *)errorOrValue);
  FatalError_CopyRichTextToNarrow(sizeof g_FatalErrorNarrowBuffer,g_FatalErrorNarrowBuffer,(uint16_t *)errorOrValue);
  Runtime_Shutdown();
  DestroyWindow(g_MainWindow);
  MessageBoxA(NULL,(LPCSTR)g_FatalErrorNarrowBuffer,NULL,MB_ICONEXCLAMATION);
  ExitProcess(0);
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
  uint16_t *nextCommand;
  uint32_t remainingCapacityBytes;
  uint16_t *commandCursor;
  uint16_t *streamCursor;
  bool newlineCapacityUnderflow;
  uint16_t *nestedReturnStack[64]; /* the original's machine-stack chain */
  int nestedDepth;
  uint16_t commandOrCodeUnit;
  
  nestedDepth = 0;
  remainingCapacityBytes = capacityBytes;
  nextCommand = source;
  while( true ) {
    while( true ) {
      commandCursor = nextCommand;
      commandOrCodeUnit = *commandCursor;
      streamCursor = commandCursor + 1;
      if (commandOrCodeUnit == 0) break;
      if ((short)commandOrCodeUnit < 0) {
        nextCommand = streamCursor;
        switch(commandOrCodeUnit & RICHTEXT_OPCODE_MASK) {
        case RICHTEXT_OP_LITERAL_COLOR:
          nextCommand = commandCursor + RICHTEXT_RECORD_UNITS_LITERAL_COLOR;
          break;
        case RICHTEXT_OP_FIXED_SPACE:
          remainingCapacityBytes--;
          if (remainingCapacityBytes == 0)
          goto FatalError_CopyRichTextToNarrow_TerminateOutputAndReturnCapacityError;
          *destination = ' ';
          destination++;
          nextCommand = streamCursor;
          break;
        case RICHTEXT_OP_LINE_BREAK:
          newlineCapacityUnderflow = remainingCapacityBytes < 2;
          remainingCapacityBytes = remainingCapacityBytes - 2;
          if (newlineCapacityUnderflow || remainingCapacityBytes == 0)
          goto FatalError_CopyRichTextToNarrow_TerminateOutputAndReturnCapacityError;
          destination[0] = '\r';
          destination[1] = '\n';
          destination = destination + 2;
          nextCommand = streamCursor;
          break;
        case RICHTEXT_OP_INLINE_VALUE_0:
        case RICHTEXT_OP_INLINE_VALUE_1:
        case RICHTEXT_OP_INLINE_VALUE_2:
          nextCommand = commandCursor + RICHTEXT_RECORD_UNITS_INLINE_VALUE;
          break;
        case RICHTEXT_OP_CALL_NESTED:
          if (nestedDepth == 64)
          goto FatalError_CopyRichTextToNarrow_TerminateOutputAndReturnCapacityError;
          nestedReturnStack[nestedDepth++] = streamCursor;
          nextCommand = *(uint16_t **)streamCursor;
          break;
        case RICHTEXT_OP_JUMP_NESTED:
          nextCommand = *(uint16_t **)streamCursor;
          break;
        case RICHTEXT_OP_INLINE_IMAGE:
          nextCommand = commandCursor + RICHTEXT_RECORD_UNITS_INLINE_IMAGE;
        }
      }
      else {
        nextCommand = streamCursor;
        if ((commandOrCodeUnit & 0xff00) == 0) { /* only glyphs that fit a narrow character */
          remainingCapacityBytes--;
          if (remainingCapacityBytes == 0)
          goto FatalError_CopyRichTextToNarrow_TerminateOutputAndReturnCapacityError;
          *destination = (uint8_t)commandOrCodeUnit;
          destination++;
          nextCommand = streamCursor;
        }
      }
    }
    if (nestedDepth == 0) break;
    nextCommand = (uint16_t *)((uint8_t *)nestedReturnStack[--nestedDepth] + RICHTEXT_NESTED_PAYLOAD_BYTES);
  }
  if (0 < (int)remainingCapacityBytes) {
    *destination = 0;
    return capacityBytes - (remainingCapacityBytes - 1);
  }
FatalError_CopyRichTextToNarrow_TerminateOutputAndReturnCapacityError:
  destination[-1] = 0;
  return FATAL_ERROR_GENERAL_FAILURE;
}

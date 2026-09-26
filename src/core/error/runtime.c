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
   Ownership: core/error/runtime.
   Purpose: Handles error system init.
   Local calls: FatalError_Exit.
   Cross-module calls: TextResourcePage_Load [assets/text/resources].
*/
void __cdecl ErrorSystem_Init(void)

{
  TextResourceLoadEaxCf5 loadResult;
  
  g_FatalErrorPrimaryDispatchCf = FatalError_Exit;
  g_FatalErrorRuntimeDispatchCf = FatalError_Exit;
  g_FatalErrorExitFallbackDispatchCf = FatalError_Exit;
  loadResult = TextResourcePage_Load(0,(word *)u_texte_error_str_00407d20);
                    // WARNING: Subroutine does not return
  FatalError_Exit(THANDOR_ADDR(g_ErrorTextIoInitializationFailed,0),loadResult.carry);
}


/* Address: 0x00407F50.
   Ownership: core/error/runtime.
   Purpose: Fallback error callback that sets carry and returns after consuming one stack argument.
*/
bool __thandor_cf_preserve_eax_ecx_edx ErrorRuntime_CallbackAlwaysFailCf(UiRootNode *root)

{
  return true;
}


/* Address: 0x00407F60.
   Ownership: core/error/runtime.
   Purpose: Fallback error callback that returns code 8 in EAX after consuming one stack argument.
*/
int __thandor_eax_preserve_ecx_edx ErrorRuntime_CallbackReturnCode8(UiRootNode *root)

{
  return 8;
}


/* Address: 0x00407F70.
   Ownership: core/error/runtime.
   Purpose: Handles fatal error dialog dismiss and pop root.
   Cross-module calls: UiRootStack_PopCf [ui/controls/layout].
*/
void __thandor_preserve_eax FatalErrorDialog_DismissAndPopRoot(UiRootNode *rootNode)

{
  UiRootStack_PopCf(rootNode);
  g_FatalErrorDialogDismissed = g_FatalErrorDialogDismissed + 1;
  return;
}


/* Address: 0x00407F90.
   Ownership: core/error/runtime.
   Purpose: Registered as the fatal-error dispatch callback after the fixed 0x110-byte error state is allocated. It
   publishes the pending error state, presents the modal runtime UI, pumps frames until dismissal, and returns
   status through CF.
   Cross-module calls: TextResource_Resolve [assets/text/resources], RichTextCommandStream_PatchPayloadBySelector
   [assets/text/richtext], RichTextCommandStream_MeasureWrappedBlockRegs [assets/text/richtext], UiRootStack_Push
   [ui/controls/layout], UiFrame_FlushInputAndResetPendingTicks [ui/controls/layout], UiRootStack_InvalidateAll
   [ui/controls/layout].
*/
FatalErrorEaxCf5 __thandor_eax_cf_io_preserve_ecx_edx
FatalErrorRuntime_DispatchPendingErrorCf(dword errorOrValue,bool carryIn)

{
  sdword *topOffsetField;
  UiRootNode *uiRootTemplate;
  word *stream;
  int remainingDwords;
  undefined4 *templateImageCursor;
  UiRootNode *templateCopyCursor;
  RichTextExtentRegs wrappedExtent;
  FatalErrorEaxCf5 passThroughResult;
  FatalErrorEaxCf5 dispatchResult;
  TextResourceResolveEaxCf5 resolvedText;
  
  if (!carryIn) {
    passThroughResult.carry = false;
    passThroughResult.eax = errorOrValue;
    return passThroughResult;
  }
  if (g_FatalErrorUiRootTemplate == (UiRootNode *)0x0) {
    dispatchResult = (*g_FatalErrorPrimaryDispatchCf)(errorOrValue,true);
    errorOrValue = dispatchResult.eax;
  }
  stream = (word *)errorOrValue;
  if ((errorOrValue & 0xffffff00) == 0) {
    resolvedText = TextResource_Resolve(errorOrValue);
    stream = resolvedText.eax;
    RichTextCommandStream_PatchPayloadBySelector(0,g_PackageLastErrorPath,stream);
    RichTextCommandStream_PatchPayloadBySelector(1,g_FatalErrorDetail1Utf16,stream);
    RichTextCommandStream_PatchPayloadBySelector(2,&g_FatalErrorDetail2Utf16,stream);
    RichTextCommandStream_PatchPayloadBySelector(3,&g_FatalErrorDetail3Utf16,stream);
  }
  templateImageCursor = &g_FatalErrorUiRootTemplateImage;
  templateCopyCursor = g_FatalErrorUiRootTemplate;
  g_FatalErrorRichTextStream = stream;
  for (remainingDwords = 0x44; uiRootTemplate = g_FatalErrorUiRootTemplate, remainingDwords != 0; remainingDwords = remainingDwords + -1) {
    (templateCopyCursor->base).nextSibling = (UiNodeBase *)*templateImageCursor;
    templateImageCursor = templateImageCursor + 1;
    templateCopyCursor = (UiRootNode *)&(templateCopyCursor->base).firstChild;
  }
  wrappedExtent = RichTextCommandStream_MeasureWrappedBlockRegs
                    (g_UiTextStyleNormal,g_FatalErrorRichTextStream,
                     ((g_FatalErrorRichTextRight - g_FatalErrorRichTextLeft) +
                     g_FatalErrorRichTextBottom) - g_FatalErrorRichTextTop);
  topOffsetField = &(uiRootTemplate->base).topOffset;
  *topOffsetField = *topOffsetField - wrappedExtent.heightPixels;
  UiRootStack_Push(&g_UiRootCallbacks_00407E28,g_FatalErrorUiRootTemplate);
  g_UiPointerCaptureTarget = (UiNodeBase *)0xffffffff;
  g_UiPointerCaptureButton = UI_POINTER_CAPTURE_NONE;
  UiFrame_FlushInputAndResetPendingTicks();
  (*g_GraphicsCursorSetFrame)(0);
  g_FatalErrorDialogDismissed = 0;
  do {
    UiRootStack_InvalidateAll();
    UiFrame_ProcessAndPresentWithLockTransition();
  } while (g_FatalErrorDialogDismissed == 0);
  dispatchResult.carry = true;
  dispatchResult.eax = errorOrValue;
  return dispatchResult;
}


/* Address: 0x00408090.
   Ownership: core/error/runtime.
   Purpose: Allocates the 0x110-byte error-runtime state, publishes it globally, and replaces the fatal fallback
   callback with the UI-capable error handler when allocation succeeds.
*/
void __fastcall ErrorRuntime_InstallUiHandlerAndAllocateState(void)

{
  void *allocatedFatalErrorUiRootTemplate;
  ArenaAllocEaxCf5 allocResult;
  
  allocResult = (*g_MemoryApi.alloc)(0x110);
  allocatedFatalErrorUiRootTemplate = (void *)allocResult.eax;
  if (!allocResult.carry) {
    g_FatalErrorRuntimeDispatchCf = FatalErrorRuntime_DispatchPendingErrorCf;
    g_FatalErrorUiRootTemplate = allocatedFatalErrorUiRootTemplate;
  }
  return;
}


/* Address: 0x0041BC50.
   Ownership: core/error/runtime.
   Purpose: EXACT_DUPLICATE_FATAL_DIALOG_NARROW_TO_UTF16_TWIN.
*/
StatusValueEaxCf5 __thandor_eax_cf_preserve_ecx_edx
FatalError_CopyNarrowToUtf16Cf(TextOutputCapacityBytes capacityBytes,word *destination,byte *source)

{
  byte sourceByte;
  TextOutputCapacityBytes remainingCapacityBytes;
  bool capacityExhausted;
  StatusValueEaxCf5 successResult;
  StatusValueEaxCf5 overflowResult;
  
  remainingCapacityBytes = capacityBytes;
  do {
    sourceByte = *source;
    capacityExhausted = remainingCapacityBytes < 2;
    remainingCapacityBytes = remainingCapacityBytes - 2;
    if (capacityExhausted || remainingCapacityBytes == 0) {
      destination[-1] = 0;
      overflowResult.carry = true;
      overflowResult.valueOrError = 0x14;
      return overflowResult;
    }
    *destination = (ushort)sourceByte;
    source = source + 1;
    destination = destination + 1;
  } while (sourceByte != 0);
  successResult.valueOrError = capacityBytes - remainingCapacityBytes;
  successResult.carry = false;
  return successResult;
}


/* Address: 0x005758D0.
   Ownership: core/error/runtime.
   Purpose: Consumes EAX/CF engine error state, shuts down, displays a message, and exits.
   Local calls: FatalError_CopyRichTextToNarrowCf.
   Cross-module calls: TextResource_Resolve [assets/text/resources], RichTextCommandStream_PatchPayloadBySelector
   [assets/text/richtext], Runtime_Shutdown [core/memory/synchronization].
*/
FatalErrorEaxCf5 __thandor_eax_cf_io_preserve_ecx_edx
FatalError_Exit(dword errorOrValue,bool carryIn)

{
  FatalErrorEaxCf5 passThroughResult;
  TextResourceResolveEaxCf5 resolvedText;
  
  if (!carryIn) {
    passThroughResult.carry = false;
    passThroughResult.eax = errorOrValue;
    return passThroughResult;
  }
  /* open-thandor diagnostics: fatal error code, last package path and the calling stack */
  Thandor_Log("fatal error 0x%08X, last path \"%ls\"", errorOrValue, (wchar_t *)g_PackageLastErrorPath);
  Thandor_LogStack("fatal error stack", errorOrValue);
  if ((errorOrValue & 0xffffff00) == 0) {
    resolvedText = TextResource_Resolve(errorOrValue);
    errorOrValue = (dword)resolvedText.eax;
  }
  RichTextCommandStream_PatchPayloadBySelector(0,g_PackageLastErrorPath,(word *)errorOrValue);
  RichTextCommandStream_PatchPayloadBySelector(1,g_FatalErrorDetail1Utf16,(word *)errorOrValue);
  RichTextCommandStream_PatchPayloadBySelector(2,&g_FatalErrorDetail2Utf16,(word *)errorOrValue);
  RichTextCommandStream_PatchPayloadBySelector(3,&g_FatalErrorDetail3Utf16,(word *)errorOrValue);
  FatalError_CopyRichTextToNarrowCf(0x400,g_FatalErrorNarrowBuffer,(word *)errorOrValue);
  Runtime_Shutdown();
  DestroyWindow(g_MainWindow);
  MessageBoxA((HWND)0x0,(LPCSTR)g_FatalErrorNarrowBuffer,(LPCSTR)0x0,0x30);
                    // WARNING: Subroutine does not return
  ExitProcess(0);
}


/* Address: 0x0041BB00.
   Ownership: core/error/runtime.
   Purpose: Fatal-dialog copy of the same bounded rich-text-to-narrow conversion used immediately before
   MessageBoxA. It follows nested command streams and preserves the original CF/EAX error contract. Typed
   parameters: p0 capacityBytes→TextOutputCapacityBytes_V342. Calling convention, exact VariableStorage
   serialization, function body bytes, control flow, globals, locals, and executable data remain unchanged.
*/
int FatalError_CopyRichTextToNarrowCf
              (TextOutputCapacityBytes capacityBytes,byte *destination,word *source)

{
  word *nextCommand;
  dword remainingCapacityBytes;
  ushort *commandCursor;
  word *streamCursor;
  bool newlineCapacityUnderflow;
  word *nestedReturnStack[64]; /* the original's machine-stack chain */
  int nestedDepth;
  ushort commandOrCodeUnit;
  
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
        switch(commandOrCodeUnit & 0x1f) {
        case 6:
          nextCommand = commandCursor + 9;
          break;
        case 0x10:
          remainingCapacityBytes = remainingCapacityBytes - 1;
          if (remainingCapacityBytes == 0)
          goto FatalError_CopyRichTextToNarrow_TerminateOutputAndReturnCapacityError;
          *destination = 0x20;
          destination = destination + 1;
          nextCommand = streamCursor;
          break;
        case 0x12:
          newlineCapacityUnderflow = remainingCapacityBytes < 2;
          remainingCapacityBytes = remainingCapacityBytes - 2;
          if (newlineCapacityUnderflow || remainingCapacityBytes == 0)
          goto FatalError_CopyRichTextToNarrow_TerminateOutputAndReturnCapacityError;
          destination[0] = 0xd;
          destination[1] = 10;
          destination = destination + 2;
          nextCommand = streamCursor;
          break;
        case 0x14:
        case 0x15:
        case 0x16:
          nextCommand = commandCursor + 3;
          break;
        case 0x18:
          if (nestedDepth == 64)
          goto FatalError_CopyRichTextToNarrow_TerminateOutputAndReturnCapacityError;
          nestedReturnStack[nestedDepth++] = streamCursor;
          nextCommand = *(ushort **)streamCursor;
          break;
        case 0x19:
          nextCommand = *(ushort **)streamCursor;
          break;
        case 0x1a:
          nextCommand = commandCursor + 5;
        }
      }
      else {
        nextCommand = streamCursor;
        if ((commandOrCodeUnit & 0xff00) == 0) {
          remainingCapacityBytes = remainingCapacityBytes - 1;
          if (remainingCapacityBytes == 0)
          goto FatalError_CopyRichTextToNarrow_TerminateOutputAndReturnCapacityError;
          *destination = (byte)commandOrCodeUnit;
          destination = destination + 1;
          nextCommand = streamCursor;
        }
      }
    }
    if (nestedDepth == 0) break;
    nextCommand = (ushort *)((byte *)nestedReturnStack[--nestedDepth] + 8);
  }
  if (0 < (int)remainingCapacityBytes) {
    *destination = 0;
    return capacityBytes - (remainingCapacityBytes - 1);
  }
FatalError_CopyRichTextToNarrow_TerminateOutputAndReturnCapacityError:
  destination[-1] = 0;
  return 0x14;
}

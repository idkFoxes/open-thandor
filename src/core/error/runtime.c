/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/src/core/error/runtime.c
 * Reverse engineering by idkFoxes 2026
 */

#include <thandor/core/error/runtime.h>

/* Implementation ownership: core/error/runtime. */

/* Address: 0x00575890.
   Ownership: core/error/runtime.
   Purpose: Handles error system init.
   Local calls: FatalError_Exit.
   Cross-module calls: TextResourcePage_Load [assets/text/resources].
*/
void __cdecl ErrorSystem_Init(void)

{
  TextResourceLoadEaxCf5 TVar1;
  
  g_FatalErrorPrimaryDispatchCf = FatalError_Exit;
  g_FatalErrorRuntimeDispatchCf = FatalError_Exit;
  g_FatalErrorExitFallbackDispatchCf = FatalError_Exit;
  TVar1 = TextResourcePage_Load(0,(word *)u_texte_error_str_00407d20);
                    // WARNING: Subroutine does not return
  FatalError_Exit(0x407d40,TVar1.carry);
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
  sdword *psVar1;
  UiRootNode *pUVar2;
  word *stream;
  int iVar3;
  undefined4 *puVar4;
  UiRootNode *pUVar5;
  RichTextExtentRegs RVar6;
  FatalErrorEaxCf5 FVar7;
  FatalErrorEaxCf5 FVar8;
  TextResourceResolveEaxCf5 TVar9;
  
  if (!carryIn) {
    FVar7.carry = false;
    FVar7.eax = errorOrValue;
    return FVar7;
  }
  if (g_FatalErrorUiRootTemplate == (UiRootNode *)0x0) {
    FVar8 = (*g_FatalErrorPrimaryDispatchCf)(errorOrValue,true);
    errorOrValue = FVar8.eax;
  }
  stream = (word *)errorOrValue;
  if ((errorOrValue & 0xffffff00) == 0) {
    TVar9 = TextResource_Resolve(errorOrValue);
    stream = TVar9.eax;
    RichTextCommandStream_PatchPayloadBySelector(0,g_PackageLastErrorPath,stream);
    RichTextCommandStream_PatchPayloadBySelector(1,g_FatalErrorDetail1Utf16,stream);
    RichTextCommandStream_PatchPayloadBySelector(2,&g_FatalErrorDetail2Utf16,stream);
    RichTextCommandStream_PatchPayloadBySelector(3,&g_FatalErrorDetail3Utf16,stream);
  }
  puVar4 = &g_FatalErrorUiRootTemplateImage;
  pUVar5 = g_FatalErrorUiRootTemplate;
  g_FatalErrorRichTextStream = stream;
  for (iVar3 = 0x44; pUVar2 = g_FatalErrorUiRootTemplate, iVar3 != 0; iVar3 = iVar3 + -1) {
    (pUVar5->base).nextSibling = (UiNodeBase *)*puVar4;
    puVar4 = puVar4 + 1;
    pUVar5 = (UiRootNode *)&(pUVar5->base).firstChild;
  }
  RVar6 = RichTextCommandStream_MeasureWrappedBlockRegs
                    (g_UiTextStyleNormal,g_FatalErrorRichTextStream,
                     ((g_FatalErrorRichTextRight - g_FatalErrorRichTextLeft) +
                     g_FatalErrorRichTextBottom) - g_FatalErrorRichTextTop);
  psVar1 = &(pUVar2->base).topOffset;
  *psVar1 = *psVar1 - RVar6.heightPixels;
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
  FVar8.carry = true;
  FVar8.eax = errorOrValue;
  return FVar8;
}


/* Address: 0x00408090.
   Ownership: core/error/runtime.
   Purpose: Allocates the 0x110-byte error-runtime state, publishes it globally, and replaces the fatal fallback
   callback with the UI-capable error handler when allocation succeeds.
*/
void __fastcall ErrorRuntime_InstallUiHandlerAndAllocateState(void)

{
  void *allocatedFatalErrorUiRootTemplate;
  ArenaAllocEaxCf5 AVar1;
  
  AVar1 = (*g_MemoryApi.alloc)(0x110);
  allocatedFatalErrorUiRootTemplate = (void *)AVar1.eax;
  if (!AVar1.carry) {
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
  byte bVar1;
  TextOutputCapacityBytes TVar2;
  bool bVar3;
  StatusValueEaxCf5 SVar4;
  StatusValueEaxCf5 SVar5;
  
  TVar2 = capacityBytes;
  do {
    bVar1 = *source;
    bVar3 = TVar2 < 2;
    TVar2 = TVar2 - 2;
    if (bVar3 || TVar2 == 0) {
      destination[-1] = 0;
      SVar5.carry = true;
      SVar5.valueOrError = 0x14;
      return SVar5;
    }
    *destination = (ushort)bVar1;
    source = source + 1;
    destination = destination + 1;
  } while (bVar1 != 0);
  SVar4.valueOrError = capacityBytes - TVar2;
  SVar4.carry = false;
  return SVar4;
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
  FatalErrorEaxCf5 FVar1;
  TextResourceResolveEaxCf5 TVar2;
  
  if (!carryIn) {
    FVar1.carry = false;
    FVar1.eax = errorOrValue;
    return FVar1;
  }
  if ((errorOrValue & 0xffffff00) == 0) {
    TVar2 = TextResource_Resolve(errorOrValue);
    errorOrValue = (dword)TVar2.eax;
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
  word *pwVar1;
  dword remainingCapacityBytes;
  ushort *commandCursor;
  word *streamCursor;
  bool newlineCapacityUnderflow;
  int nestedReturnCursor;
  ushort commandOrCodeUnit;
  
  nestedReturnCursor = 0;
  remainingCapacityBytes = capacityBytes;
  pwVar1 = source;
  while( true ) {
    while( true ) {
      commandCursor = pwVar1;
      commandOrCodeUnit = *commandCursor;
      streamCursor = commandCursor + 1;
      if (commandOrCodeUnit == 0) break;
      if ((short)commandOrCodeUnit < 0) {
        pwVar1 = streamCursor;
        switch(commandOrCodeUnit & 0x1f) {
        case 6:
          pwVar1 = commandCursor + 9;
          break;
        case 0x10:
          remainingCapacityBytes = remainingCapacityBytes - 1;
          if (remainingCapacityBytes == 0)
          goto FatalError_CopyRichTextToNarrow_TerminateOutputAndReturnCapacityError;
          *destination = 0x20;
          destination = destination + 1;
          pwVar1 = streamCursor;
          break;
        case 0x12:
          newlineCapacityUnderflow = remainingCapacityBytes < 2;
          remainingCapacityBytes = remainingCapacityBytes - 2;
          if (newlineCapacityUnderflow || remainingCapacityBytes == 0)
          goto FatalError_CopyRichTextToNarrow_TerminateOutputAndReturnCapacityError;
          destination[0] = 0xd;
          destination[1] = 10;
          destination = destination + 2;
          pwVar1 = streamCursor;
          break;
        case 0x14:
        case 0x15:
        case 0x16:
          pwVar1 = commandCursor + 3;
          break;
        case 0x18:
          nestedReturnCursor = (int)commandCursor + 3;
          pwVar1 = *(ushort **)streamCursor;
          break;
        case 0x19:
          pwVar1 = *(ushort **)streamCursor;
          break;
        case 0x1a:
          pwVar1 = commandCursor + 5;
        }
      }
      else {
        pwVar1 = streamCursor;
        if ((commandOrCodeUnit & 0xff00) == 0) {
          remainingCapacityBytes = remainingCapacityBytes - 1;
          if (remainingCapacityBytes == 0)
          goto FatalError_CopyRichTextToNarrow_TerminateOutputAndReturnCapacityError;
          *destination = (byte)commandOrCodeUnit;
          destination = destination + 1;
          pwVar1 = streamCursor;
        }
      }
    }
    if (nestedReturnCursor == 0) break;
    pwVar1 = (ushort *)(nestedReturnCursor + 8);
    nestedReturnCursor = nestedReturnCursor + -1;
  }
  if (0 < (int)remainingCapacityBytes) {
    *destination = 0;
    return capacityBytes - (remainingCapacityBytes - 1);
  }
FatalError_CopyRichTextToNarrow_TerminateOutputAndReturnCapacityError:
  destination[-1] = 0;
  return 0x14;
}

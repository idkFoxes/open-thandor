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
  g_FatalErrorPrimaryDispatchCf = FatalError_Exit;
  g_FatalErrorRuntimeDispatchCf = FatalError_Exit;
  g_FatalErrorExitFallbackDispatchCf = FatalError_Exit;
  TextResourcePage_Load(0,(word *)u_texte_error_str_00407d20);
                    
  FatalError_Exit();
}

/* Address: 0x00407F50.
   Ownership: core/error/runtime.
   Purpose: Fallback error callback that sets carry and returns after consuming one stack argument.
*/
void ErrorRuntime_CallbackAlwaysFailCf(void)

{
  return;
}

/* Address: 0x00407F60.
   Ownership: core/error/runtime.
   Purpose: Fallback error callback that returns code 8 in EAX after consuming one stack argument.
*/
undefined4 ErrorRuntime_CallbackReturnCode8(void)

{
  return 8;
}

/* Address: 0x00407F70.
   Ownership: core/error/runtime.
   Purpose: Handles fatal error dialog dismiss and pop root.
   Cross-module calls: UiRootStack_PopCf [ui/controls/layout].
*/
undefined4 FatalErrorDialog_DismissAndPopRoot(UiRootNode *param_1)

{
  undefined4 in_EAX;
  
  UiRootStack_PopCf(param_1);
  g_FatalErrorDialogDismissed = g_FatalErrorDialogDismissed + 1;
  return in_EAX;
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
void __cdecl FatalErrorRuntime_DispatchPendingErrorCf(void)

{
  sdword *psVar1;
  UiRootNode *pUVar2;
  word *in_EAX;
  word *stream;
  int iVar3;
  undefined4 *puVar4;
  UiRootNode *pUVar5;
  bool in_CF;
  undefined8 uVar6;
  RichTextExtentRegs RVar7;
  
  if (!in_CF) {
    return;
  }
  if (g_FatalErrorUiRootTemplate == (UiRootNode *)0x0) {
    in_EAX = (word *)(*g_FatalErrorPrimaryDispatchCf)();
  }
  if (((uint)in_EAX & 0xffffff00) == 0) {
    stream = TextResource_Resolve((TextResourceId)in_EAX);
    uVar6 = RichTextCommandStream_PatchPayloadBySelector(0,g_PackageLastErrorPath,stream);
    uVar6 = RichTextCommandStream_PatchPayloadBySelector(1,g_FatalErrorDetail1Utf16,(word *)uVar6);
    uVar6 = RichTextCommandStream_PatchPayloadBySelector(2,&g_FatalErrorDetail2Utf16,(word *)uVar6);
    uVar6 = RichTextCommandStream_PatchPayloadBySelector(3,&g_FatalErrorDetail3Utf16,(word *)uVar6);
    in_EAX = (word *)uVar6;
  }
  puVar4 = &g_FatalErrorUiRootTemplateImage;
  pUVar5 = g_FatalErrorUiRootTemplate;
  g_FatalErrorRichTextStream = in_EAX;
  for (iVar3 = 0x44; pUVar2 = g_FatalErrorUiRootTemplate, iVar3 != 0; iVar3 = iVar3 + -1) {
    (pUVar5->base).nextSibling = (UiNodeBase *)*puVar4;
    puVar4 = puVar4 + 1;
    pUVar5 = (UiRootNode *)&(pUVar5->base).firstChild;
  }
  RVar7 = RichTextCommandStream_MeasureWrappedBlockRegs
                    (g_UiTextStyleNormal,g_FatalErrorRichTextStream,
                     ((g_FatalErrorRichTextRight - g_FatalErrorRichTextLeft) +
                     g_FatalErrorRichTextBottom) - g_FatalErrorRichTextTop);
  psVar1 = &(pUVar2->base).topOffset;
  *psVar1 = *psVar1 - RVar7.heightPixels;
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
  return;
}

/* Address: 0x00408090.
   Ownership: core/error/runtime.
   Purpose: Allocates the 0x110-byte error-runtime state, publishes it globally, and replaces the fatal fallback
   callback with the UI-capable error handler when allocation succeeds.
*/
void __fastcall ErrorRuntime_InstallUiHandlerAndAllocateState(void)

{
  void *allocatedFatalErrorUiRootTemplate;
  undefined1 in_CF;
  
  allocatedFatalErrorUiRootTemplate = (*g_MemoryApi.alloc)(0x110);
  if (!(bool)in_CF) {
    g_FatalErrorRuntimeDispatchCf = FatalErrorRuntime_DispatchPendingErrorCf;
    g_FatalErrorUiRootTemplate = allocatedFatalErrorUiRootTemplate;
  }
  return;
}

/* Address: 0x0041BC50.
   Ownership: core/error/runtime.
   Purpose: EXACT_DUPLICATE_FATAL_DIALOG_NARROW_TO_UTF16_TWIN.
*/
int FatalError_CopyNarrowToUtf16Cf(uint param_1,ushort *param_2,byte *param_3)

{
  byte bVar1;
  uint uVar2;
  bool bVar3;
  
  uVar2 = param_1;
  do {
    bVar1 = *param_3;
    bVar3 = uVar2 < 2;
    uVar2 = uVar2 - 2;
    if (bVar3 || uVar2 == 0) {
      param_2[-1] = 0;
      return 0x14;
    }
    *param_2 = (ushort)bVar1;
    param_3 = param_3 + 1;
    param_2 = param_2 + 1;
  } while (bVar1 != 0);
  return param_1 - uVar2;
}

/* Address: 0x005758D0.
   Ownership: core/error/runtime.
   Purpose: Consumes EAX/CF engine error state, shuts down, displays a message, and exits.
   Local calls: FatalError_CopyRichTextToNarrowCf.
   Cross-module calls: TextResource_Resolve [assets/text/resources], RichTextCommandStream_PatchPayloadBySelector
   [assets/text/richtext], Runtime_Shutdown [core/memory/synchronization].
*/
void __cdecl FatalError_Exit(void)

{
  word *in_EAX;
  bool in_CF;
  undefined8 uVar1;
  
  if (!in_CF) {
    return;
  }
  if (((uint)in_EAX & 0xffffff00) == 0) {
    in_EAX = TextResource_Resolve((TextResourceId)in_EAX);
  }
  uVar1 = RichTextCommandStream_PatchPayloadBySelector(0,g_PackageLastErrorPath,in_EAX);
  uVar1 = RichTextCommandStream_PatchPayloadBySelector(1,g_FatalErrorDetail1Utf16,(word *)uVar1);
  uVar1 = RichTextCommandStream_PatchPayloadBySelector(2,&g_FatalErrorDetail2Utf16,(word *)uVar1);
  uVar1 = RichTextCommandStream_PatchPayloadBySelector(3,&g_FatalErrorDetail3Utf16,(word *)uVar1);
  FatalError_CopyRichTextToNarrowCf(0x400,g_FatalErrorNarrowBuffer,(word *)uVar1);
  Runtime_Shutdown();
  DestroyWindow(g_MainWindow);
  MessageBoxA((HWND)0x0,(LPCSTR)g_FatalErrorNarrowBuffer,(LPCSTR)0x0,0x30);
                    
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

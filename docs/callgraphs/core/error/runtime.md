# Call graph: `core/error/runtime`

9 functions. Addresses are current V523 executable VAs.

## Functions

- `0x00575890` **ErrorSystem_Init** — local: `FatalError_Exit`; cross: `TextResourcePage_Load` → `assets/text/resources`
- `0x00407F50` **ErrorRuntime_CallbackAlwaysFailCf**
- `0x00407F60` **ErrorRuntime_CallbackReturnCode8**
- `0x00407F70` **FatalErrorDialog_DismissAndPopRoot** — cross: `UiRootStack_PopCf` → `ui/controls/layout`
- `0x00407F90` **FatalErrorRuntime_DispatchPendingErrorCf** — cross: `TextResource_Resolve` → `assets/text/resources`, `RichTextCommandStream_PatchPayloadBySelector` → `assets/text/richtext`, `RichTextCommandStream_MeasureWrappedBlockRegs` → `assets/text/richtext`, `UiRootStack_Push` → `ui/controls/layout`, `UiFrame_FlushInputAndResetPendingTicks` → `ui/controls/layout`, `UiRootStack_InvalidateAll` → `ui/controls/layout`, `UiFrame_ProcessAndPresentWithLockTransition` → `ui/controls/layout`
- `0x00408090` **ErrorRuntime_InstallUiHandlerAndAllocateState**
- `0x0041BC50` **FatalError_CopyNarrowToUtf16Cf**
- `0x005758D0` **FatalError_Exit** — local: `FatalError_CopyRichTextToNarrowCf`; cross: `TextResource_Resolve` → `assets/text/resources`, `RichTextCommandStream_PatchPayloadBySelector` → `assets/text/richtext`, `Runtime_Shutdown` → `core/memory/synchronization`
- `0x0041BB00` **FatalError_CopyRichTextToNarrowCf**

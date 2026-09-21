# Call graph: `core/error/runtime`

[Source](../../../../src/core/error/runtime.c) · [Header](../../../../include/thandor/core/error/runtime.h) · [Graphviz](runtime.dot) · [Module tree](../../../MODULE_TREE.md) · [Source guide](../../../SOURCE_FILE_GUIDE.md#module-core-error-runtime) · [Developer notes](../../../../CHANGELOG.md#module-core-error-runtime) · [Full changelog](../../../../CHANGELOG_FULL.md#module-core-error-runtime)

9 functions. Addresses are current V523 executable VAs.

## Functions

- `0x00575890` **[`ErrorSystem_Init`](../../../../src/core/error/runtime.c#L5)** — local: [`FatalError_Exit`](../../../../src/core/error/runtime.c#L168); cross: [`TextResourcePage_Load`](../../../../src/assets/text/resources.c#L255) → [`assets/text/resources`](../../assets/text/resources.md)
- `0x00407F50` **[`ErrorRuntime_CallbackAlwaysFailCf`](../../../../src/core/error/runtime.c#L22)**
- `0x00407F60` **[`ErrorRuntime_CallbackReturnCode8`](../../../../src/core/error/runtime.c#L32)**
- `0x00407F70` **[`FatalErrorDialog_DismissAndPopRoot`](../../../../src/core/error/runtime.c#L42)** — cross: [`UiRootStack_PopCf`](../../../../src/ui/controls/layout.c#L556) → [`ui/controls/layout`](../../ui/controls/layout.md)
- `0x00407F90` **[`FatalErrorRuntime_DispatchPendingErrorCf`](../../../../src/core/error/runtime.c#L57)** — cross: [`TextResource_Resolve`](../../../../src/assets/text/resources.c#L400) → [`assets/text/resources`](../../assets/text/resources.md), [`RichTextCommandStream_PatchPayloadBySelector`](../../../../src/assets/text/richtext.c#L222) → [`assets/text/richtext`](../../assets/text/richtext.md), [`RichTextCommandStream_MeasureWrappedBlockRegs`](../../../../src/assets/text/richtext.c#L5) → [`assets/text/richtext`](../../assets/text/richtext.md), [`UiRootStack_Push`](../../../../src/ui/controls/layout.c#L504) → [`ui/controls/layout`](../../ui/controls/layout.md), [`UiFrame_FlushInputAndResetPendingTicks`](../../../../src/ui/controls/layout.c#L1123) → [`ui/controls/layout`](../../ui/controls/layout.md), [`UiRootStack_InvalidateAll`](../../../../src/ui/controls/layout.c#L1918) → [`ui/controls/layout`](../../ui/controls/layout.md), [`UiFrame_ProcessAndPresentWithLockTransition`](../../../../src/ui/controls/layout.c#L311) → [`ui/controls/layout`](../../ui/controls/layout.md)
- `0x00408090` **[`ErrorRuntime_InstallUiHandlerAndAllocateState`](../../../../src/core/error/runtime.c#L122)**
- `0x0041BC50` **[`FatalError_CopyNarrowToUtf16Cf`](../../../../src/core/error/runtime.c#L141)**
- `0x005758D0` **[`FatalError_Exit`](../../../../src/core/error/runtime.c#L168)** — local: [`FatalError_CopyRichTextToNarrowCf`](../../../../src/core/error/runtime.c#L200); cross: [`TextResource_Resolve`](../../../../src/assets/text/resources.c#L400) → [`assets/text/resources`](../../assets/text/resources.md), [`RichTextCommandStream_PatchPayloadBySelector`](../../../../src/assets/text/richtext.c#L222) → [`assets/text/richtext`](../../assets/text/richtext.md), [`Runtime_Shutdown`](../../../../src/core/memory/synchronization.c#L79) → [`core/memory/synchronization`](../memory/synchronization.md)
- `0x0041BB00` **[`FatalError_CopyRichTextToNarrowCf`](../../../../src/core/error/runtime.c#L200)**

## Called by

- [`core/memory/allocator`](../memory/allocator.md): [`ArenaHeap_Init`](../../../../src/core/memory/allocator.c#L134) → [`FatalError_Exit`](../../../../src/core/error/runtime.c#L168)
- [`platform/bootstrap/runtime`](../../platform/bootstrap/runtime.md): [`ProcessEntry`](../../../../src/platform/bootstrap/runtime.c#L5) → [`ErrorSystem_Init`](../../../../src/core/error/runtime.c#L5)
- [`platform/filesystem/win32`](../../platform/filesystem/win32.md): [`FileSystem_Init`](../../../../src/platform/filesystem/win32.c#L5) → [`FatalError_Exit`](../../../../src/core/error/runtime.c#L168)
- [`ui/core/runtime`](../../ui/core/runtime.md): [`UiRuntime_Initialize`](../../../../src/ui/core/runtime.c#L194) → [`ErrorRuntime_InstallUiHandlerAndAllocateState`](../../../../src/core/error/runtime.c#L122)

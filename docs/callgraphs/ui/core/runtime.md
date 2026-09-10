# Call graph: `ui/core/runtime`

27 functions. Addresses are current V523 executable VAs.

## Functions

- `0x004228F0` **UiRootCallbacks_FreeCf**
- `0x00422980` **UiRootCallbacks_NoOpMethod08**
- `0x00424270` **UiRuntime_FormatSignedValues140And144**
- `0x004244E0` **UiRuntime_OpenFourValueDialogCf** — cross: `TextResource_Resolve` → `assets/text/resources`, `RichTextCommandStream_PatchPayloadBySelector` → `assets/text/richtext`, `UiRootStack_Push` → `ui/controls/layout`, `UiRootStack_InvalidateAll` → `ui/controls/layout`
- `0x004AEF00` **UiRuntimeRecordRing_DiscardOldestCf**
- `0x004AF020` **UiRuntimeRecordRing_Clear**
- `0x004AF030` **UiRuntimeRecordRing_HasPendingCf**
- `0x004AF050` **UiRuntimeRecordRing_ContainsIdCf**
- `0x004AF0F0` **UiRuntime_SetSynchronizationHooks**
- `0x004AF210` **UiRuntime_Initialize** — cross: `FontRuntime_Init` → `assets/text/resources`, `UiWindowResources_Init` → `ui/controls/layout`, `ErrorRuntime_InstallUiHandlerAndAllocateState` → `core/error/runtime`
- `0x004AF2F0` **UiRuntime_Shutdown**
- `0x004AF3A0` **UiRuntime_IncrementPeriodicTickCounter**
- `0x004AF760` **UiActionQueue_DispatchPending**
- `0x004B05A0` **UiNode_DefaultMethod04_NoOp**
- `0x004B0750` **UiNode_DefaultNonRightPress**
- `0x004B0760` **UiNode_DefaultNonRightRelease**
- `0x004B0770` **UiNode_ForwardRightPressToParent**
- `0x004B07C0` **UiNode_DefaultRightRelease**
- `0x004B07D0` **UiNode_DefaultNonRightDrag**
- `0x004B07E0` **UiNode_DefaultRightDrag**
- `0x004B08E0` **UiNode_ApplyFlagsRecursive**
- `0x004B09E0` **UiNode_DefaultTick**
- `0x004B0FD0` **UiActionHandlers_SetPageCf**
- `0x004B14B0` **UiNode_GetRoot**
- `0x004B1510` **UiNode_InvalidateRoot**
- `0x004B1590` **UiActionQueue_Enqueue**
- `0x004BD160` **UiNode_GetStateTintArgb**

# Call graph: `ui/controls/misc`

23 functions. Addresses are current V523 executable VAs.

## Functions

- `0x00422910` **UiDisplaySettingsRoot_RefreshModeSelection** — local: `UiDisplayModeSelection_RefreshEnumeratedOptions`; cross: `UiRuntime_FormatSignedValues140And144` → `ui/core/runtime`
- `0x00423C40` **UiDisplayModeAction_UpdateAdapterSelection** — local: `UiDisplayModeSelection_RefreshEnumeratedOptions`; cross: `UiNode_GetRoot` → `ui/core/runtime`
- `0x00423C80` **UiDisplayModeAction_UpdateResolutionSelection** — local: `UiDisplayModeSelection_RefreshEnumeratedOptions`; cross: `UiNode_GetRoot` → `ui/core/runtime`
- `0x00423CB0` **UiDisplayModeAction_UpdateColorDepthSelection** — local: `UiDisplayModeSelection_RefreshEnumeratedOptions`; cross: `UiNode_GetRoot` → `ui/core/runtime`
- `0x00424590` **UiDisplayModeAction_ApplyFourValueDialogAndReopenSettings** — local: `UiDisplaySettings_OpenAndPopulateModeSelectionCf`; cross: `UiNode_GetRoot` → `ui/core/runtime`, `UiRootStack_PopCf` → `ui/controls/layout`, `UiFrame_ProcessAndPresentWithLockTransition` → `ui/controls/layout`
- `0x004BC8B0` **UiImageControl_NonRightDrag** — local: `UiImageControl_HitTestOpaque`; cross: `UiRootStack_InvalidateAll` → `ui/controls/layout`
- `0x004BCB50` **UiImageControl_TickHover** — local: `UiImageControl_HitTestOpaque`
- `0x00423B30` **UiDisplayModeAction_ApplyPendingMode** — cross: `UiNode_GetRoot` → `ui/core/runtime`, `UiRootStack_PopCf` → `ui/controls/layout`, `UiFrame_ProcessAndPresentWithLockTransition` → `ui/controls/layout`, `UiRootStack_Relayout` → `ui/controls/layout`, `UiRuntime_OpenFourValueDialogCf` → `ui/core/runtime`
- `0x00423C00` **UiDisplayModeAction_CancelAndRebuildPixelPacking** — cross: `UiNode_GetRoot` → `ui/core/runtime`, `UiRootStack_PopCf` → `ui/controls/layout`
- `0x004242D0` **UiFourValueDialog_TickCountdownAndRequestClose** — cross: `UiActionQueue_Enqueue` → `ui/core/runtime`
- `0x004B3F40` **UiRangeSliderControl_DrawTrackAndThumb** — cross: `UiWindow_BlitTiledVerticalEdge` → `ui/controls/layout`, `UiWindow_BlitTiledHorizontalEdge` → `ui/controls/layout`
- `0x004B41C0` **UiRangeSliderControl_BeginThumbDrag**
- `0x004B4280` **UiRangeSliderControl_EndThumbDrag**
- `0x004B45F0` **UiRangeSliderControl_SuppressIfActionId** — cross: `UiKeyboardFocus_ReleaseNode` → `ui/controls/input`, `UiNode_InvalidateRoot` → `ui/core/runtime`
- `0x004B4620` **UiRangeSliderControl_UnsuppressIfActionId** — cross: `UiKeyboardFocus_AcquireIfNone` → `ui/controls/input`, `UiNode_InvalidateRoot` → `ui/core/runtime`
- `0x004BC5C0` **UiImageControl_DrawClipped** — cross: `UiContainer_DrawIntersectingChildren` → `ui/controls/layout`
- `0x004BC6E0` **UiImageControl_NonRightPress** — cross: `UiNode_InvalidateRoot` → `ui/core/runtime`
- `0x004BC7E0` **UiImageControl_NonRightRelease** — cross: `UiNode_InvalidateRoot` → `ui/core/runtime`
- `0x004BD2A0` **UiModelControl_RefreshStateTint** — cross: `UiNode_GetStateTintArgb` → `ui/core/runtime`, `ModelNodeRuntime_ApplyTintRecursive` → `world/model/hierarchy`
- `0x00517E30` **UiHorizontalGaugeControl_UpdateRuntimeRangeAndDraw** — cross: `UiHorizontalGaugeControl_DrawFrameFillAndLabel` → `ui/controls/layout`
- `0x00423600` **UiDisplaySettings_OpenAndPopulateModeSelectionCf** — local: `UiDisplayModeSelection_RefreshEnumeratedOptions`; cross: `UiRuntime_FormatSignedValues140And144` → `ui/core/runtime`, `UiActionHandlers_SetPageCf` → `ui/core/runtime`, `UiRootStack_Push` → `ui/controls/layout`, `UiRootStack_InvalidateAll` → `ui/controls/layout`
- `0x004BC9B0` **UiImageControl_HitTestOpaque** — cross: `UiContainer_HitTestChildren` → `ui/controls/layout`
- `0x00423D70` **UiDisplayModeSelection_RefreshEnumeratedOptions** — cross: `GraphicsDisplayMode_IsEnumeratedCf` → `graphics/backend/directdraw`, `UiNodeList_SuppressActionId` → `ui/controls/lists`, `UiNodeList_UnsuppressActionId` → `ui/controls/lists`, `UiSelectableGroup_SelectExclusive` → `ui/controls/lists`

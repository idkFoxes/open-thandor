# Call graph: `ui/controls/layout`

48 functions. Addresses are current V523 executable VAs.

## Functions

- `0x004B49A0` **UiPanelControl_DrawOptionalTiledBackgroundFrameAndChildren** — local: `UiWindow_BlitTiledInterior`, `UiWindow_BlitTiledHorizontalEdge`, `UiWindow_BlitTiledVerticalEdge`, `UiContainer_DrawIntersectingChildren`
- `0x004B4D40` **UiResizableWindowControl_DrawFrameTitleAndChildren** — local: `UiWindow_BlitTiledInterior`, `UiWindow_BlitTiledHorizontalEdge`, `UiWindow_BlitTiledVerticalEdge`, `UiContainer_DrawIntersectingChildren`; cross: `TextResource_Resolve` → `assets/text/resources`, `RichTextCommandStream_DrawSingleLine` → `assets/text/richtext`
- `0x004B3420` **UiTitledWindowControl_DrawFrameTitleAndChildren** — local: `UiWindow_BlitTiledVerticalEdge`, `UiWindow_BlitTiledHorizontalEdge`, `UiContainer_DrawIntersectingChildren`; cross: `TextResource_Resolve` → `assets/text/resources`, `RichTextCommandStream_MeasureRegs` → `assets/text/richtext`, `RichTextCommandStream_DrawSingleLine` → `assets/text/richtext`
- `0x004AF890` **UiFrame_ProcessAndPresentWithLockTransition** — local: `UiFrame_Update`, `UiFrame_Draw`; cross: `UiKeyboard_DispatchPendingEvents` → `ui/controls/input`, `UiPointer_DispatchPendingEvents` → `ui/controls/input`, `UiActionQueue_DispatchPending` → `ui/core/runtime`
- `0x004AF920` **UiFrame_ProcessAndPresent** — local: `UiFrame_Update`, `UiFrame_Draw`; cross: `UiKeyboard_DispatchPendingEvents` → `ui/controls/input`, `UiPointer_DispatchPendingEvents` → `ui/controls/input`, `UiActionQueue_DispatchPending` → `ui/core/runtime`
- `0x004B48D0` **UiPageStack_SetActiveIndex** — local: `UiNodeSubtree_ReleaseKeyboardFocus`, `UiNodeSubtree_AcquireKeyboardFocusDefaults`; cross: `UiNode_InvalidateRoot` → `ui/core/runtime`
- `0x004B52D0` **UiResizableWindowControl_EndMoveResizeAndHandleWindowActions** — local: `UiContainer_LayoutWithOptionalWindowHeaderOffset`, `UiRootStack_InvalidateAll`; cross: `UiActionQueue_Enqueue` → `ui/core/runtime`, `UiNode_InvalidateRoot` → `ui/core/runtime`
- `0x004B5770` **UiResizableWindowControl_HandleWindowHotkeysCf** — local: `UiContainer_LayoutWithOptionalWindowHeaderOffset`, `UiRootStack_InvalidateAll`; cross: `UiNode_InvalidateRoot` → `ui/core/runtime`, `UiActionQueue_Enqueue` → `ui/core/runtime`, `UiNode_DefaultKeyboardEventMoveFocusNextCf` → `ui/controls/input`
- `0x004B1000` **UiRootStack_Push** — local: `UiSerializedTree_Relocate`; cross: `UiKeyboardFocus_SelectInitial` → `ui/controls/input`
- `0x004B1110` **UiRootStack_PopCf** — local: `UiRootStack_InvalidateAll`; cross: `UiNode_InvalidateRoot` → `ui/core/runtime`, `UiKeyboardFocus_SelectInitial` → `ui/controls/input`
- `0x004B2790` **UiWindowControl_RelocateWithFrameInset** — local: `UiContainer_RelocateChildren`
- `0x004B36C0` **UiTitledWindowControl_LayoutFrameTitleAndChildren** — local: `UiContainer_LayoutChildren`; cross: `TextResource_Resolve` → `assets/text/resources`, `RichTextCommandStream_MeasureRegs` → `assets/text/richtext`
- `0x004B3C00` **UiContainer_HitTestChildrenOrNoneA** — local: `UiContainer_HitTestChildren`
- `0x004B3C80` **UiHorizontalGaugeControl_DrawFrameFillAndLabel** — local: `UiWindow_BlitTiledHorizontalEdge`; cross: `RichTextCommandStream_DrawSingleLine` → `assets/text/richtext`
- `0x004B46A0` **UiLayoutContainerControl_RelocateChildren** — local: `UiContainer_RelocateChildren`
- `0x004B4700` **UiLayoutContainerControl_LayoutChildren** — local: `UiContainer_LayoutChildren`
- `0x004B4790` **UiContainer_HitTestChildrenOrNoneB** — local: `UiContainer_HitTestChildren`
- `0x004B47B0` **UiLayoutContainerControl_SuppressActionIdRecursive** — local: `UiContainer_SuppressActionId`
- `0x004B4800` **UiLayoutContainerControl_UnsuppressActionIdRecursive** — local: `UiContainer_UnsuppressActionId`
- `0x004B4D10` **UiResizableWindowControl_RelocateAndRefreshInteractionState** — local: `UiContainer_RelocateChildren`
- `0x004B53E0` **UiResizableWindowControl_UpdateMoveOrResize** — local: `UiRootStack_InvalidateAll`; cross: `UiNode_InvalidateRoot` → `ui/core/runtime`
- `0x004BC660` **UiImageControl_LayoutChildrenToParent** — local: `UiContainer_LayoutChildren`
- `0x004AF3B0` **UiFrame_FlushInputAndResetPendingTicks**
- `0x004AF9D0` **UiRootStack_BringToFront** — cross: `UiKeyboardFocus_SelectInitial` → `ui/controls/input`, `UiNode_InvalidateRoot` → `ui/core/runtime`
- `0x004B0F30` **UiWindowResources_Init** — cross: `TextResourcePage_Load` → `assets/text/resources`, `UiActionHandlers_SetPageCf` → `ui/core/runtime`
- `0x004B1240` **UiRootStack_Relayout**
- `0x004B3EE0` **UiContainer_PointerMoveReturnCode6**
- `0x004B4740` **UiLayoutContainerControl_ApplyFlagsRecursive** — cross: `UiNode_ApplyFlagsRecursive` → `ui/core/runtime`
- `0x004B5120` **UiResizableWindowControl_BeginMoveResizeOrWindowAction**
- `0x004B5660` **UiResizableWindowControl_QueryResizeCursorCode**
- `0x00569A80` **UiGrid_ComputeDimensionsPacked**
- `0x00569AE0` **UiGrid_OneColumnDimensionsPacked**
- `0x004B0940` **UiContainer_SuppressActionId**
- `0x004B0990` **UiContainer_UnsuppressActionId**
- `0x004B1420` **UiSerializedTree_Relocate**
- `0x004B4850` **UiNodeSubtree_AcquireKeyboardFocusDefaults** — cross: `UiKeyboardFocus_AcquireIfNone` → `ui/controls/input`
- `0x004B4890` **UiNodeSubtree_ReleaseKeyboardFocus** — cross: `UiKeyboardFocus_ReleaseNode` → `ui/controls/input`
- `0x004B50D0` **UiContainer_LayoutWithOptionalWindowHeaderOffset** — local: `UiContainer_LayoutChildren`
- `0x004AF680` **UiFrame_Update** — cross: `UiTree_AdvanceSpriteButtonAnimations` → `ui/controls/buttons`, `UiTooltip_TickCountdown` → `ui/controls/text`, `DirectInputMouse_RefreshDeviceIfIdle` → `platform/input/devices`
- `0x004AF7E0` **UiFrame_Draw** — cross: `UiTooltip_Draw` → `ui/controls/text`
- `0x004B0800` **UiContainer_HitTestChildren**
- `0x004B13B0` **UiWindow_BlitTiledInterior**
- `0x004B0510` **UiContainer_RelocateChildren**
- `0x004B05B0` **UiContainer_DrawIntersectingChildren**
- `0x004B1350` **UiWindow_BlitTiledVerticalEdge**
- `0x004B0640` **UiContainer_LayoutChildren**
- `0x004B12F0` **UiWindow_BlitTiledHorizontalEdge**
- `0x004B14D0` **UiRootStack_InvalidateAll** — cross: `UiNode_InvalidateRoot` → `ui/core/runtime`

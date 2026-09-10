# Call graph: `ui/controls/input`

32 functions. Addresses are current V523 executable VAs.

## Functions

- `0x004AF500` **UiPointer_DispatchPendingEvents** — local: `UiPointer_DispatchMotionAndWheel`, `UiPointer_DispatchRightPress`, `UiPointer_DispatchLeftPress`, `UiPointer_DispatchMiddlePress`; cross: `DirectInputMouse_PollBufferedEvents` → `platform/input/devices`, `Random_NextPrimary` → `core/math/random`
- `0x004B00F0` **UiKeyboardFocus_ReleaseNode** — local: `UiKeyboardFocus_MoveNext`, `UiKeyboardFocus_Set`
- `0x004AF3D0` **UiKeyboard_DispatchPendingEvents** — local: `UiKeyboardFocus_Set`
- `0x004B0030` **UiKeyboardFocus_SelectInitial** — local: `UiKeyboardFocus_Set`
- `0x004B0120` **UiKeyboardFocus_AcquireIfNone** — local: `UiKeyboardFocus_Set`
- `0x004B4420` **UiRangeSliderControl_HandleKeyboardCf** — local: `UiNode_DefaultKeyboardEventMoveFocusNextCf`; cross: `UiActionQueue_Enqueue` → `ui/core/runtime`, `UiNode_InvalidateRoot` → `ui/core/runtime`
- `0x004B9CB0` **UiFocusProxyControl_ForwardKeyboardEventToChildCf** — local: `UiNode_DefaultKeyboardEventMoveFocusNextCf`; cross: `UiNode_InvalidateRoot` → `ui/core/runtime`
- `0x004B9DA0` **UiFocusProxyControl_ForwardPointerWheelToChildOrParent** — local: `UiNode_ForwardPointerWheelToParent`; cross: `UiNode_InvalidateRoot` → `ui/core/runtime`
- `0x004B07F0` **UiNode_DefaultPointerMove**
- `0x004B42D0` **UiRangeSliderControl_UpdateValueFromPointer** — cross: `UiActionQueue_Enqueue` → `ui/core/runtime`, `UiNode_InvalidateRoot` → `ui/core/runtime`
- `0x004B4570` **UiRangeSliderControl_HandlePointerWheel** — cross: `UiActionQueue_Enqueue` → `ui/core/runtime`, `UiNode_InvalidateRoot` → `ui/core/runtime`
- `0x004B9580` **UiFocusProxyControl_RelocateChild** — cross: `UiContainer_RelocateChildren` → `ui/controls/layout`
- `0x004B99A0` **UiFocusProxyControl_ForwardNonRightPressToChild** — cross: `UiNode_InvalidateRoot` → `ui/core/runtime`
- `0x004B9A00` **UiFocusProxyControl_ForwardNonRightReleaseToChild** — cross: `UiNode_InvalidateRoot` → `ui/core/runtime`
- `0x004B9A60` **UiFocusProxyControl_ForwardRightPressToChild** — cross: `UiNode_InvalidateRoot` → `ui/core/runtime`
- `0x004B9AD0` **UiFocusProxyControl_ForwardRightReleaseToChild** — cross: `UiNode_InvalidateRoot` → `ui/core/runtime`
- `0x004B9B30` **UiFocusProxyControl_ForwardNonRightDragToChild** — cross: `UiNode_InvalidateRoot` → `ui/core/runtime`
- `0x004B9B90` **UiFocusProxyControl_ForwardRightDragToChild** — cross: `UiNode_InvalidateRoot` → `ui/core/runtime`
- `0x004B9BF0` **UiFocusProxyControl_ForwardPointerMoveToChild** — cross: `UiNode_InvalidateRoot` → `ui/core/runtime`
- `0x004B9C50` **UiFocusProxyControl_HitTestChildProxy** — cross: `UiContainer_HitTestChildren` → `ui/controls/layout`
- `0x004B9D40` **UiFocusProxyControl_ForwardTickToChild** — cross: `UiNode_InvalidateRoot` → `ui/core/runtime`
- `0x004BCA70` **UiImageControl_PointerMove** — cross: `UiContainer_HitTestChildren` → `ui/controls/layout`
- `0x00515CC0` **UiSelectionGeometryControl_DrawClipped**
- `0x005161A0` **UiSelectionGeometryControl_ConvertPointerAndEnqueueAction** — cross: `UiActionQueue_Enqueue` → `ui/core/runtime`
- `0x004AFA60` **UiPointer_DispatchLeftPress** — local: `UiKeyboardFocus_Set`; cross: `UiImageControl_HitTestOpaque` → `ui/controls/misc`, `UiRootStack_BringToFront` → `ui/controls/layout`
- `0x004AFBC0` **UiPointer_DispatchMiddlePress** — local: `UiKeyboardFocus_Set`; cross: `UiImageControl_HitTestOpaque` → `ui/controls/misc`, `UiRootStack_BringToFront` → `ui/controls/layout`
- `0x004AFD10` **UiPointer_DispatchRightPress** — local: `UiKeyboardFocus_Set`; cross: `UiRootStack_BringToFront` → `ui/controls/layout`
- `0x004AFFA0` **UiKeyboardFocus_MoveNext** — local: `UiKeyboardFocus_Set`
- `0x004AFE40` **UiPointer_DispatchMotionAndWheel** — cross: `InGameSelectionDetailPanel_Rebuild` → `ui/ingame/runtime`, `UiTooltip_UpdateHoverTarget` → `ui/controls/text`
- `0x004B09F0` **UiNode_ForwardPointerWheelToParent**
- `0x004B08C0` **UiNode_DefaultKeyboardEventMoveFocusNextCf**
- `0x004AFF60` **UiKeyboardFocus_Set** — cross: `UiRootStack_InvalidateAll` → `ui/controls/layout`

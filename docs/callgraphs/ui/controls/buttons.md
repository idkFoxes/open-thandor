# Call graph: `ui/controls/buttons`

17 functions. Addresses are current V523 executable VAs.

## Functions

- `0x004B1D20` **UiTree_AdvanceSpriteButtonAnimations** — local: `UiSpriteButtonControl_AdvanceAnimation`
- `0x004B1620` **UiSpriteButtonControl_Relocate** — cross: `Random_NextPrimary` → `core/math/random`, `UiContainer_RelocateChildren` → `ui/controls/layout`
- `0x004B16E0` **UiSpriteButtonControl_DrawClipped**
- `0x004B1890` **UiSpriteButtonControl_NonRightPress** — cross: `UiNode_InvalidateRoot` → `ui/core/runtime`, `UiActionQueue_Enqueue` → `ui/core/runtime`
- `0x004B1A30` **UiSpriteButtonControl_NonRightRelease** — cross: `UiActionQueue_Enqueue` → `ui/core/runtime`, `UiNode_InvalidateRoot` → `ui/core/runtime`
- `0x004B1AE0` **UiSpriteButtonControl_NonRightDrag** — cross: `UiNode_InvalidateRoot` → `ui/core/runtime`
- `0x004B1BF0` **UiSpriteButtonControl_HitTestOpaque**
- `0x00515010` **UiImageActionControl_DrawImageAndChildren** — cross: `UiContainer_DrawIntersectingChildren` → `ui/controls/layout`
- `0x005151F0` **UiImageActionControl_QueryPointerCode**
- `0x00515210` **UiImageActionControl_EnqueuePrimaryAction** — cross: `UiActionQueue_Enqueue` → `ui/core/runtime`
- `0x00515230` **UiImageActionControl_EnqueueSecondaryAction** — cross: `UiActionQueue_Enqueue` → `ui/core/runtime`
- `0x00515250` **UiImageActionControl_HandleKeyboardActivationCf** — cross: `UiKeyboardFocus_MoveNext` → `ui/controls/input`, `UiActionQueue_Enqueue` → `ui/core/runtime`
- `0x005152E0` **UiConditionalActionControl_DrawClipped** — cross: `UiWindow_BlitTiledHorizontalEdge` → `ui/controls/layout`, `UiWindow_BlitTiledVerticalEdge` → `ui/controls/layout`, `UiWindow_BlitTiledInterior` → `ui/controls/layout`, `RichTextCommandStream_MeasureRegs` → `assets/text/richtext`, `RichTextCommandStream_DrawSingleLine` → `assets/text/richtext`
- `0x005155A0` **UiConditionalActionControl_QueryPointerCode**
- `0x005155C0` **UiConditionalActionControl_HitTestWhenEnabled** — cross: `UiContainer_HitTestChildren` → `ui/controls/layout`
- `0x005155F0` **UiConditionalActionControl_EnqueuePrimaryActionIfEnabled** — cross: `UiActionQueue_Enqueue` → `ui/core/runtime`
- `0x004B1C80` **UiSpriteButtonControl_AdvanceAnimation** — cross: `UiActionQueue_Enqueue` → `ui/core/runtime`, `UiNode_InvalidateRoot` → `ui/core/runtime`

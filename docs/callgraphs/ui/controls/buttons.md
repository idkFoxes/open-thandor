# Call graph: `ui/controls/buttons`

[Source](../../../../src/ui/controls/buttons.c) · [Header](../../../../include/thandor/ui/controls/buttons.h) · [Graphviz](buttons.dot) · [Module tree](../../../MODULE_TREE.md) · [Source guide](../../../SOURCE_FILE_GUIDE.md#module-ui-controls-buttons)

17 functions. Addresses are current V523 executable VAs.

## Functions

- `0x004B1D20` **[`UiTree_AdvanceSpriteButtonAnimations`](../../../../src/ui/controls/buttons.c#L5)** — local: [`UiSpriteButtonControl_AdvanceAnimation`](../../../../src/ui/controls/buttons.c#L719)
- `0x004B1620` **[`UiSpriteButtonControl_Relocate`](../../../../src/ui/controls/buttons.c#L37)** — cross: [`Random_NextPrimary`](../../../../src/core/math/random.c#L5) → [`core/math/random`](../../core/math/random.md), [`UiContainer_RelocateChildren`](../../../../src/ui/controls/layout.c#L1765) → [`ui/controls/layout`](layout.md)
- `0x004B16E0` **[`UiSpriteButtonControl_DrawClipped`](../../../../src/ui/controls/buttons.c#L85)**
- `0x004B1890` **[`UiSpriteButtonControl_NonRightPress`](../../../../src/ui/controls/buttons.c#L169)** — cross: [`UiNode_InvalidateRoot`](../../../../src/ui/core/runtime.c#L469) → [`ui/core/runtime`](../core/runtime.md), [`UiActionQueue_Enqueue`](../../../../src/ui/core/runtime.c#L504) → [`ui/core/runtime`](../core/runtime.md)
- `0x004B1A30` **[`UiSpriteButtonControl_NonRightRelease`](../../../../src/ui/controls/buttons.c#L247)** — cross: [`UiActionQueue_Enqueue`](../../../../src/ui/core/runtime.c#L504) → [`ui/core/runtime`](../core/runtime.md), [`UiNode_InvalidateRoot`](../../../../src/ui/core/runtime.c#L469) → [`ui/core/runtime`](../core/runtime.md)
- `0x004B1AE0` **[`UiSpriteButtonControl_NonRightDrag`](../../../../src/ui/controls/buttons.c#L283)** — cross: [`UiNode_InvalidateRoot`](../../../../src/ui/core/runtime.c#L469) → [`ui/core/runtime`](../core/runtime.md)
- `0x004B1BF0` **[`UiSpriteButtonControl_HitTestOpaque`](../../../../src/ui/controls/buttons.c#L347)**
- `0x00515010` **[`UiImageActionControl_DrawImageAndChildren`](../../../../src/ui/controls/buttons.c#L394)** — cross: [`UiContainer_DrawIntersectingChildren`](../../../../src/ui/controls/layout.c#L1799) → [`ui/controls/layout`](layout.md)
- `0x005151F0` **[`UiImageActionControl_QueryPointerCode`](../../../../src/ui/controls/buttons.c#L471)**
- `0x00515210` **[`UiImageActionControl_EnqueuePrimaryAction`](../../../../src/ui/controls/buttons.c#L483)** — cross: [`UiActionQueue_Enqueue`](../../../../src/ui/core/runtime.c#L504) → [`ui/core/runtime`](../core/runtime.md)
- `0x00515230` **[`UiImageActionControl_EnqueueSecondaryAction`](../../../../src/ui/controls/buttons.c#L497)** — cross: [`UiActionQueue_Enqueue`](../../../../src/ui/core/runtime.c#L504) → [`ui/core/runtime`](../core/runtime.md)
- `0x00515250` **[`UiImageActionControl_HandleKeyboardActivationCf`](../../../../src/ui/controls/buttons.c#L511)** — cross: [`UiKeyboardFocus_MoveNext`](../../../../src/ui/controls/input.c#L1591) → [`ui/controls/input`](input.md), [`UiActionQueue_Enqueue`](../../../../src/ui/core/runtime.c#L504) → [`ui/core/runtime`](../core/runtime.md)
- `0x005152E0` **[`UiConditionalActionControl_DrawClipped`](../../../../src/ui/controls/buttons.c#L532)** — cross: [`UiWindow_BlitTiledHorizontalEdge`](../../../../src/ui/controls/layout.c#L1895) → [`ui/controls/layout`](layout.md), [`UiWindow_BlitTiledVerticalEdge`](../../../../src/ui/controls/layout.c#L1824) → [`ui/controls/layout`](layout.md), [`UiWindow_BlitTiledInterior`](../../../../src/ui/controls/layout.c#L1741) → [`ui/controls/layout`](layout.md), [`RichTextCommandStream_MeasureRegs`](../../../../src/assets/text/richtext.c#L1167) → [`assets/text/richtext`](../../assets/text/richtext.md), [`RichTextCommandStream_DrawSingleLine`](../../../../src/assets/text/richtext.c#L81) → [`assets/text/richtext`](../../assets/text/richtext.md)
- `0x005155A0` **[`UiConditionalActionControl_QueryPointerCode`](../../../../src/ui/controls/buttons.c#L672)**
- `0x005155C0` **[`UiConditionalActionControl_HitTestWhenEnabled`](../../../../src/ui/controls/buttons.c#L684)** — cross: [`UiContainer_HitTestChildren`](../../../../src/ui/controls/layout.c#L1702) → [`ui/controls/layout`](layout.md)
- `0x005155F0` **[`UiConditionalActionControl_EnqueuePrimaryActionIfEnabled`](../../../../src/ui/controls/buttons.c#L703)** — cross: [`UiActionQueue_Enqueue`](../../../../src/ui/core/runtime.c#L504) → [`ui/core/runtime`](../core/runtime.md)
- `0x004B1C80` **[`UiSpriteButtonControl_AdvanceAnimation`](../../../../src/ui/controls/buttons.c#L719)** — cross: [`UiActionQueue_Enqueue`](../../../../src/ui/core/runtime.c#L504) → [`ui/core/runtime`](../core/runtime.md), [`UiNode_InvalidateRoot`](../../../../src/ui/core/runtime.c#L469) → [`ui/core/runtime`](../core/runtime.md)

## Called by

- [`ui/controls/layout`](layout.md): [`UiFrame_Update`](../../../../src/ui/controls/layout.c#L1592) → [`UiTree_AdvanceSpriteButtonAnimations`](../../../../src/ui/controls/buttons.c#L5)

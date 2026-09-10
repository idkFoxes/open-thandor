# Call graph: `graphics/render/projection`

[Source](../../../../src/graphics/render/projection.c) · [Header](../../../../include/thandor/graphics/render/projection.h) · [Graphviz](projection.dot) · [Module tree](../../../MODULE_TREE.md) · [Source guide](../../../SOURCE_FILE_GUIDE.md#module-graphics-render-projection)

2 functions. Addresses are current V523 executable VAs.

## Functions

- `0x00486B00` **[`GraphicsOffscreen_RenderModelListToTextureSourceCf`](../../../../src/graphics/render/projection.c#L5)** — cross: [`Graphics_SetProjectionClipRect`](../../../../src/graphics/core/runtime.c#L151) → [`graphics/core/runtime`](../core/runtime.md), [`Graphics_SetViewProjectionParameters`](../../../../src/graphics/core/runtime.c#L171) → [`graphics/core/runtime`](../core/runtime.md), [`Graphics_SetProjectionViewport`](../../../../src/graphics/core/runtime.c#L222) → [`graphics/core/runtime`](../core/runtime.md), [`Graphics_SetAuxiliaryOrientation`](../../../../src/graphics/core/runtime.c#L260) → [`graphics/core/runtime`](../core/runtime.md), [`Graphics_SetSceneBounds`](../../../../src/graphics/core/runtime.c#L283) → [`graphics/core/runtime`](../core/runtime.md), [`Graphics_RebuildFrustumPlanes`](../../../../src/graphics/core/runtime.c#L316) → [`graphics/core/runtime`](../core/runtime.md), [`GraphicsPrimitiveQueue_ResetGlobal`](../../../../src/graphics/render/primitives.c#L280) → [`graphics/render/primitives`](primitives.md), [`Graphics_SetActivePrimitiveQueue`](../../../../src/graphics/core/runtime.c#L305) → [`graphics/core/runtime`](../core/runtime.md), [`ModelRuntime_CullAndRenderHierarchyRecursive`](../../../../src/world/model/runtime.c#L66) → [`world/model/runtime`](../../world/model/runtime.md), [`GraphicsPrimitiveQueue_RadixSortForRendering`](../../../../src/graphics/render/primitives.c#L5) → [`graphics/render/primitives`](primitives.md), [`SoftwareRenderer_DrawQueueAuxiliary`](../../../../src/graphics/backend/software.c#L192) → [`graphics/backend/software`](../backend/software.md)
- `0x0050A6A0` **[`GraphicsProjectedPoint_IsInsideTriangleCf`](../../../../src/graphics/render/projection.c#L128)**

## Called by

- [`world/model/hierarchy`](../../world/model/hierarchy.md): [`ModelRuntimeNode_HitTestProjectedBoundsAndChildrenCf`](../../../../src/world/model/hierarchy.c#L344) → [`GraphicsProjectedPoint_IsInsideTriangleCf`](../../../../src/graphics/render/projection.c#L128)

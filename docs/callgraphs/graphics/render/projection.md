# Call graph: `graphics/render/projection`

2 functions. Addresses are current V523 executable VAs.

## Functions

- `0x00486B00` **GraphicsOffscreen_RenderModelListToTextureSourceCf** — cross: `Graphics_SetProjectionClipRect` → `graphics/core/runtime`, `Graphics_SetViewProjectionParameters` → `graphics/core/runtime`, `Graphics_SetProjectionViewport` → `graphics/core/runtime`, `Graphics_SetAuxiliaryOrientation` → `graphics/core/runtime`, `Graphics_SetSceneBounds` → `graphics/core/runtime`, `Graphics_RebuildFrustumPlanes` → `graphics/core/runtime`, `GraphicsPrimitiveQueue_ResetGlobal` → `graphics/render/primitives`, `Graphics_SetActivePrimitiveQueue` → `graphics/core/runtime`, `ModelRuntime_CullAndRenderHierarchyRecursive` → `world/model/runtime`, `GraphicsPrimitiveQueue_RadixSortForRendering` → `graphics/render/primitives`, `SoftwareRenderer_DrawQueueAuxiliary` → `graphics/backend/software`
- `0x0050A6A0` **GraphicsProjectedPoint_IsInsideTriangleCf**

# Call graph: `graphics/resources/framebuffer`

[Source](../../../../src/graphics/resources/framebuffer.c) · [Header](../../../../include/thandor/graphics/resources/framebuffer.h) · [Graphviz](framebuffer.dot) · [Module tree](../../../MODULE_TREE.md) · [Source guide](../../../SOURCE_FILE_GUIDE.md#module-graphics-resources-framebuffer)

7 functions. Addresses are current V523 executable VAs.

## Functions

- `0x004A9250` **[`GraphicsFramebuffer_BeginAccessStub`](../../../../src/graphics/resources/framebuffer.c#L5)**
- `0x004A9260` **[`GraphicsFramebuffer_EndAccessStub`](../../../../src/graphics/resources/framebuffer.c#L16)**
- `0x005796E0` **[`GraphicsFramebuffer_Present`](../../../../src/graphics/resources/framebuffer.c#L26)** — cross: [`Glide3_Framebuffer_Present`](../../../../src/graphics/backend/glide.c#L625) → [`graphics/backend/glide`](../backend/glide.md), [`GraphicsCursor_ComposeBeforePresent`](../../../../src/graphics/core/runtime.c#L930) → [`graphics/core/runtime`](../core/runtime.md), [`GraphicsCursor_RestoreAfterPresent`](../../../../src/graphics/core/runtime.c#L978) → [`graphics/core/runtime`](../core/runtime.md)
- `0x005798A0` **[`GraphicsFramebuffer_CaptureRegion16Bit`](../../../../src/graphics/resources/framebuffer.c#L133)** — cross: [`Glide3_Framebuffer_CaptureRegion`](../../../../src/graphics/backend/glide.c#L902) → [`graphics/backend/glide`](../backend/glide.md), [`Memory_ZeroDwords`](../../../../src/core/memory/allocator.c#L406) → [`core/memory/allocator`](../../core/memory/allocator.md)
- `0x00579B50` **[`GraphicsFramebuffer_CaptureRegion32Bit`](../../../../src/graphics/resources/framebuffer.c#L255)** — cross: [`Memory_ZeroDwords`](../../../../src/core/memory/allocator.c#L406) → [`core/memory/allocator`](../../core/memory/allocator.md)
- `0x00579D90` **[`GraphicsFramebuffer_BeginAccess`](../../../../src/graphics/resources/framebuffer.c#L367)** — cross: [`Glide3_Framebuffer_BeginAccess`](../../../../src/graphics/backend/glide.c#L4233) → [`graphics/backend/glide`](../backend/glide.md), [`Memory_ZeroDwords`](../../../../src/core/memory/allocator.c#L406) → [`core/memory/allocator`](../../core/memory/allocator.md)
- `0x00579E60` **[`GraphicsFramebuffer_EndAccess`](../../../../src/graphics/resources/framebuffer.c#L414)** — cross: [`Glide3_Framebuffer_EndAccess`](../../../../src/graphics/backend/glide.c#L4270) → [`graphics/backend/glide`](../backend/glide.md)

## Called by

No cross-module callers are recorded.

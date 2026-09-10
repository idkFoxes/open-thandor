# Call graph: `graphics/resources/framebuffer`

7 functions. Addresses are current V523 executable VAs.

## Functions

- `0x004A9250` **GraphicsFramebuffer_BeginAccessStub**
- `0x004A9260` **GraphicsFramebuffer_EndAccessStub**
- `0x005796E0` **GraphicsFramebuffer_Present** — cross: `Glide3_Framebuffer_Present` → `graphics/backend/glide`, `GraphicsCursor_ComposeBeforePresent` → `graphics/core/runtime`, `GraphicsCursor_RestoreAfterPresent` → `graphics/core/runtime`
- `0x005798A0` **GraphicsFramebuffer_CaptureRegion16Bit** — cross: `Glide3_Framebuffer_CaptureRegion` → `graphics/backend/glide`, `Memory_ZeroDwords` → `core/memory/allocator`
- `0x00579B50` **GraphicsFramebuffer_CaptureRegion32Bit** — cross: `Memory_ZeroDwords` → `core/memory/allocator`
- `0x00579D90` **GraphicsFramebuffer_BeginAccess** — cross: `Glide3_Framebuffer_BeginAccess` → `graphics/backend/glide`, `Memory_ZeroDwords` → `core/memory/allocator`
- `0x00579E60` **GraphicsFramebuffer_EndAccess** — cross: `Glide3_Framebuffer_EndAccess` → `graphics/backend/glide`

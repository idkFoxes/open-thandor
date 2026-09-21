# Call graph: `core/memory/allocator`

[Source](../../../../src/core/memory/allocator.c) · [Header](../../../../include/thandor/core/memory/allocator.h) · [Graphviz](allocator.dot) · [Module tree](../../../MODULE_TREE.md) · [Source guide](../../../SOURCE_FILE_GUIDE.md#module-core-memory-allocator) · [Developer notes](../../../../CHANGELOG.md#module-core-memory-allocator) · [Full changelog](../../../../CHANGELOG_FULL.md#module-core-memory-allocator)

12 functions. Addresses are current V523 executable VAs.

## Functions

- `0x005368E0` **[`PriorityPairHeap_SiftUp`](../../../../src/core/memory/allocator.c#L5)**
- `0x00536930` **[`PriorityPairHeap_SiftDown`](../../../../src/core/memory/allocator.c#L44)**
- `0x00547D20` **[`DwordBlock64Array_ContainsExactRecordCf`](../../../../src/core/memory/allocator.c#L94)**
- `0x005863C0` **[`ArenaHeap_Init`](../../../../src/core/memory/allocator.c#L134)** — cross: [`FatalError_Exit`](../../../../src/core/error/runtime.c#L168) → [`core/error/runtime`](../error/runtime.md)
- `0x00586470` **[`ArenaHeap_Shutdown`](../../../../src/core/memory/allocator.c#L171)**
- `0x005864A0` **[`ArenaHeap_Alloc`](../../../../src/core/memory/allocator.c#L183)**
- `0x00586570` **[`ArenaHeap_QueryFreeBytes`](../../../../src/core/memory/allocator.c#L239)**
- `0x005865B0` **[`ArenaHeap_Free`](../../../../src/core/memory/allocator.c#L264)**
- `0x00586640` **[`ArenaHeap_AllocLargestFreeBlock`](../../../../src/core/memory/allocator.c#L306)**
- `0x005866B0` **[`ArenaHeap_ShrinkInPlace`](../../../../src/core/memory/allocator.c#L340)**
- `0x00586750` **[`ArenaHeap_ReserveLinear`](../../../../src/core/memory/allocator.c#L386)**
- `0x005873A0` **[`Memory_ZeroDwords`](../../../../src/core/memory/allocator.c#L406)**

## Called by

- [`assets/scenario/catalog`](../../assets/scenario/catalog.md): [`FrontendScenarioTransfer_ProcessReceivedAsset`](../../../../src/assets/scenario/catalog.c#L626) → [`DwordBlock64Array_ContainsExactRecordCf`](../../../../src/core/memory/allocator.c#L94)
- [`audio/backend/runtime`](../../audio/backend/runtime.md): [`DirectSound_Init`](../../../../src/audio/backend/runtime.c#L147) → [`Memory_ZeroDwords`](../../../../src/core/memory/allocator.c#L406); [`DirectSound_CreateSampleVoiceSet`](../../../../src/audio/backend/runtime.c#L251) → [`Memory_ZeroDwords`](../../../../src/core/memory/allocator.c#L406); [`DirectSound_CreatePcmVoiceSet`](../../../../src/audio/backend/runtime.c#L393) → [`Memory_ZeroDwords`](../../../../src/core/memory/allocator.c#L406)
- [`core/memory/synchronization`](synchronization.md): [`Runtime_Shutdown`](../../../../src/core/memory/synchronization.c#L79) → [`ArenaHeap_Shutdown`](../../../../src/core/memory/allocator.c#L171)
- [`graphics/backend/directdraw`](../../graphics/backend/directdraw.md): [`GraphicsDirectDraw_ApplyDisplayModeAndCreateResourcesCf`](../../../../src/graphics/backend/directdraw.c#L180) → [`Memory_ZeroDwords`](../../../../src/core/memory/allocator.c#L406)
- [`graphics/core/runtime`](../../graphics/core/runtime.md): [`Graphics_SetViewportAndClearDepth`](../../../../src/graphics/core/runtime.c#L751) → [`Memory_ZeroDwords`](../../../../src/core/memory/allocator.c#L406); [`GraphicsCursor_SaveSurfaceBackground`](../../../../src/graphics/core/runtime.c#L1000) → [`Memory_ZeroDwords`](../../../../src/core/memory/allocator.c#L406); [`GraphicsCursor_RestoreSurfaceBackground`](../../../../src/graphics/core/runtime.c#L1144) → [`Memory_ZeroDwords`](../../../../src/core/memory/allocator.c#L406)
- [`graphics/resources/framebuffer`](../../graphics/resources/framebuffer.md): [`GraphicsFramebuffer_CaptureRegion16Bit`](../../../../src/graphics/resources/framebuffer.c#L133) → [`Memory_ZeroDwords`](../../../../src/core/memory/allocator.c#L406); [`GraphicsFramebuffer_CaptureRegion32Bit`](../../../../src/graphics/resources/framebuffer.c#L255) → [`Memory_ZeroDwords`](../../../../src/core/memory/allocator.c#L406); [`GraphicsFramebuffer_BeginAccess`](../../../../src/graphics/resources/framebuffer.c#L367) → [`Memory_ZeroDwords`](../../../../src/core/memory/allocator.c#L406)
- [`graphics/resources/texture`](../../graphics/resources/texture.md): [`GraphicsTexture_UploadColor_1x`](../../../../src/graphics/resources/texture.c#L805) → [`Memory_ZeroDwords`](../../../../src/core/memory/allocator.c#L406); [`GraphicsTexture_UploadColor_2x`](../../../../src/graphics/resources/texture.c#L1221) → [`Memory_ZeroDwords`](../../../../src/core/memory/allocator.c#L406); [`GraphicsTexture_UploadColor_4x`](../../../../src/graphics/resources/texture.c#L1840) → [`Memory_ZeroDwords`](../../../../src/core/memory/allocator.c#L406); [`GraphicsTexture_UploadAlpha_1x`](../../../../src/graphics/resources/texture.c#L2910) → [`Memory_ZeroDwords`](../../../../src/core/memory/allocator.c#L406); [`GraphicsTexture_UploadAlpha_2x`](../../../../src/graphics/resources/texture.c#L3024) → [`Memory_ZeroDwords`](../../../../src/core/memory/allocator.c#L406); [`GraphicsTexture_UploadAlpha_4x`](../../../../src/graphics/resources/texture.c#L3160) → [`Memory_ZeroDwords`](../../../../src/core/memory/allocator.c#L406); [`GraphicsTexture_CreateDeviceTexture`](../../../../src/graphics/resources/texture.c#L3352) → [`Memory_ZeroDwords`](../../../../src/core/memory/allocator.c#L406); [`GraphicsTexture_CreateStagingTexture`](../../../../src/graphics/resources/texture.c#L3700) → [`Memory_ZeroDwords`](../../../../src/core/memory/allocator.c#L406)
- [`platform/bootstrap/runtime`](../../platform/bootstrap/runtime.md): [`ProcessEntry`](../../../../src/platform/bootstrap/runtime.c#L5) → [`ArenaHeap_Init`](../../../../src/core/memory/allocator.c#L134)
- [`platform/filesystem/win32`](../../platform/filesystem/win32.md): [`FileSystem_Init`](../../../../src/platform/filesystem/win32.c#L5) → [`ArenaHeap_Alloc`](../../../../src/core/memory/allocator.c#L183); [`FileSystem_Init`](../../../../src/platform/filesystem/win32.c#L5) → [`ArenaHeap_Free`](../../../../src/core/memory/allocator.c#L264)
- [`world/pathing/grid`](../../world/pathing/grid.md): [`EntityPathing_RebuildOverlappingGroupRoutes`](../../../../src/world/pathing/grid.c#L844) → [`PriorityPairHeap_SiftUp`](../../../../src/core/memory/allocator.c#L5); [`EntityPathing_RebuildOverlappingGroupRoutes`](../../../../src/world/pathing/grid.c#L844) → [`PriorityPairHeap_SiftDown`](../../../../src/core/memory/allocator.c#L44)

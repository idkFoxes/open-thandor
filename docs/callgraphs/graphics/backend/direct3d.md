# Call graph: `graphics/backend/direct3d`

[Source](../../../../src/graphics/backend/direct3d.c) · [Header](../../../../include/thandor/graphics/backend/direct3d.h) · [Graphviz](direct3d.dot) · [Module tree](../../../MODULE_TREE.md) · [Source guide](../../../SOURCE_FILE_GUIDE.md#module-graphics-backend-direct3d)

14 functions. Addresses are current V523 executable VAs.

## Functions

- `0x00578270` **[`Direct3D_EnumDeviceCallback`](../../../../src/graphics/backend/direct3d.c#L5)** — cross: [`Text_CopyNarrowToUtf16Cf`](../../../../src/core/text/string.c#L185) → [`core/text/string`](../../core/text/string.md)
- `0x00578820` **[`GraphicsDirect3D_SelectPreferredTextureFormatEnumCallback`](../../../../src/graphics/backend/direct3d.c#L88)**
- `0x0057A450` **[`Direct3DRenderer_SetAntialiasMode`](../../../../src/graphics/backend/direct3d.c#L166)**
- `0x0057A4C0` **[`Direct3DRenderer_SetTextureFilterMode`](../../../../src/graphics/backend/direct3d.c#L189)**
- `0x0057A550` **[`Direct3DRenderer_SetTexturePerspectiveEnabled`](../../../../src/graphics/backend/direct3d.c#L217)**
- `0x0057CCB0` **[`Direct3D_PrimitiveHandler_UntexturedPreset0`](../../../../src/graphics/backend/direct3d.c#L240)**
- `0x0057CF20` **[`Direct3D_PrimitiveHandler_UntexturedPreset2`](../../../../src/graphics/backend/direct3d.c#L498)**
- `0x0057D1D0` **[`Direct3D_PrimitiveHandler_UntexturedPreset3`](../../../../src/graphics/backend/direct3d.c#L774)**
- `0x0057D480` **[`Direct3D_PrimitiveHandler_UntexturedPreset4`](../../../../src/graphics/backend/direct3d.c#L1051)**
- `0x0057D730` **[`Direct3D_PrimitiveHandler_TexturedPreset0`](../../../../src/graphics/backend/direct3d.c#L1327)** — cross: [`GraphicsTexture_CreateDeviceTexture`](../../../../src/graphics/resources/texture.c#L3352) → [`graphics/resources/texture`](../resources/texture.md)
- `0x0057DA50` **[`Direct3D_PrimitiveHandler_TexturedPreset1`](../../../../src/graphics/backend/direct3d.c#L1530)** — cross: [`GraphicsTexture_CreateDeviceTexture`](../../../../src/graphics/resources/texture.c#L3352) → [`graphics/resources/texture`](../resources/texture.md)
- `0x0057DD70` **[`Direct3D_PrimitiveHandler_TexturedPreset2`](../../../../src/graphics/backend/direct3d.c#L1733)** — cross: [`GraphicsTexture_CreateDeviceTexture`](../../../../src/graphics/resources/texture.c#L3352) → [`graphics/resources/texture`](../resources/texture.md)
- `0x0057E090` **[`Direct3D_PrimitiveHandler_TexturedPreset3`](../../../../src/graphics/backend/direct3d.c#L1936)** — cross: [`GraphicsTexture_CreateDeviceTexture`](../../../../src/graphics/resources/texture.c#L3352) → [`graphics/resources/texture`](../resources/texture.md)
- `0x0057E3B0` **[`Direct3D_PrimitiveHandler_TexturedPreset4`](../../../../src/graphics/backend/direct3d.c#L2140)** — cross: [`GraphicsTexture_CreateDeviceTexture`](../../../../src/graphics/resources/texture.c#L3352) → [`graphics/resources/texture`](../resources/texture.md)

## Called by

No cross-module callers are recorded.

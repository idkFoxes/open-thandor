# Call graph: `graphics/backend/direct3d`

14 functions. Addresses are current V523 executable VAs.

## Functions

- `0x00578270` **Direct3D_EnumDeviceCallback** — cross: `Text_CopyNarrowToUtf16Cf` → `core/text/string`
- `0x00578820` **GraphicsDirect3D_SelectPreferredTextureFormatEnumCallback**
- `0x0057A450` **Direct3DRenderer_SetAntialiasMode**
- `0x0057A4C0` **Direct3DRenderer_SetTextureFilterMode**
- `0x0057A550` **Direct3DRenderer_SetTexturePerspectiveEnabled**
- `0x0057CCB0` **Direct3D_PrimitiveHandler_UntexturedPreset0**
- `0x0057CF20` **Direct3D_PrimitiveHandler_UntexturedPreset2**
- `0x0057D1D0` **Direct3D_PrimitiveHandler_UntexturedPreset3**
- `0x0057D480` **Direct3D_PrimitiveHandler_UntexturedPreset4**
- `0x0057D730` **Direct3D_PrimitiveHandler_TexturedPreset0** — cross: `GraphicsTexture_CreateDeviceTexture` → `graphics/resources/texture`
- `0x0057DA50` **Direct3D_PrimitiveHandler_TexturedPreset1** — cross: `GraphicsTexture_CreateDeviceTexture` → `graphics/resources/texture`
- `0x0057DD70` **Direct3D_PrimitiveHandler_TexturedPreset2** — cross: `GraphicsTexture_CreateDeviceTexture` → `graphics/resources/texture`
- `0x0057E090` **Direct3D_PrimitiveHandler_TexturedPreset3** — cross: `GraphicsTexture_CreateDeviceTexture` → `graphics/resources/texture`
- `0x0057E3B0` **Direct3D_PrimitiveHandler_TexturedPreset4** — cross: `GraphicsTexture_CreateDeviceTexture` → `graphics/resources/texture`

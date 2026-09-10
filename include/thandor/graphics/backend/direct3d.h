#ifndef THANDOR_GRAPHICS_BACKEND_DIRECT3D_H
#define THANDOR_GRAPHICS_BACKEND_DIRECT3D_H

#include <thandor/generated/v523_types.h>
#include <thandor/core/contracts.h>

/* Submodule: graphics/backend/direct3d. */
/* Functions are grouped by semantic ownership; address comments are executable virtual addresses. */

/* 0x00578270 */
sdword Direct3D_EnumDeviceCallback (TH_LEGACY_GUID *deviceGuid,char *description,char *deviceName, D3DDEVICEDESC_DX6 *hardwareDesc,D3DDEVICEDESC_DX6 *softwareDesc, GraphicsAdapterRecord *adapterContext);

/* 0x00578820 */
undefined8 __fastcall GraphicsDirect3D_SelectPreferredTextureFormatEnumCallback (undefined4 param_1,undefined4 param_2,int param_3);

/* 0x0057A450 */
undefined8 __fastcall Direct3DRenderer_SetAntialiasMode(undefined4 param_1,undefined4 param_2,dword param_3);

/* 0x0057A4C0 */
undefined8 __fastcall Direct3DRenderer_SetTextureFilterMode(undefined4 param_1,undefined4 param_2,dword param_3);

/* 0x0057A550 */
undefined8 __fastcall Direct3DRenderer_SetTexturePerspectiveEnabled(undefined4 param_1,undefined4 param_2,dword param_3);

/* 0x0057CCB0 */
void Direct3D_PrimitiveHandler_UntexturedPreset0(GraphicsPrimitivePacket *packet);

/* 0x0057CF20 */
void Direct3D_PrimitiveHandler_UntexturedPreset2(GraphicsPrimitivePacket *packet);

/* 0x0057D1D0 */
void Direct3D_PrimitiveHandler_UntexturedPreset3(GraphicsPrimitivePacket *packet);

/* 0x0057D480 */
void Direct3D_PrimitiveHandler_UntexturedPreset4(GraphicsPrimitivePacket *packet);

/* 0x0057D730 */
void Direct3D_PrimitiveHandler_TexturedPreset0(GraphicsPrimitivePacket *packet);

/* 0x0057DA50 */
void Direct3D_PrimitiveHandler_TexturedPreset1(GraphicsPrimitivePacket *packet);

/* 0x0057DD70 */
void Direct3D_PrimitiveHandler_TexturedPreset2(GraphicsPrimitivePacket *packet);

/* 0x0057E090 */
void Direct3D_PrimitiveHandler_TexturedPreset3(GraphicsPrimitivePacket *packet);

/* 0x0057E3B0 */
void Direct3D_PrimitiveHandler_TexturedPreset4(GraphicsPrimitivePacket *packet);

#endif /* THANDOR_GRAPHICS_BACKEND_DIRECT3D_H */

/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/include/thandor/graphics/backend/direct3d.h
 * Reverse engineering by idkFoxes 2026
 */

#ifndef THANDOR_GRAPHICS_BACKEND_DIRECT3D_H
#define THANDOR_GRAPHICS_BACKEND_DIRECT3D_H

#include <thandor/generated/types.h>
#include <thandor/core/contracts.h>

/* Submodule: graphics/backend/direct3d. */
/* Functions are grouped by semantic ownership; address comments are executable virtual addresses. */

/* 0x00578270 */
sdword Direct3D_EnumDeviceCallback (TH_LEGACY_GUID *deviceGuid,char *description,char *deviceName, D3DDEVICEDESC_DX6 *hardwareDesc,D3DDEVICEDESC_DX6 *softwareDesc, GraphicsAdapterRecord *adapterContext);

/* 0x00578820 */
sdword __thandor_eax_preserve_ecx_edx
GraphicsDirect3D_SelectPreferredTextureFormatEnumCallback
          (DDSURFACEDESC_DX6 *surfaceDesc,TH_LEGACY_LPVOID context);

/* 0x0057A450 */
Direct3DRenderStateApplyEaxCf5 __thandor_eax_cf_preserve_ecx_edx
Direct3DRenderer_SetAntialiasMode(dword antialiasMode);

/* 0x0057A4C0 */
Direct3DRenderStateApplyEaxCf5 __thandor_eax_cf_preserve_ecx_edx
Direct3DRenderer_SetTextureFilterMode(dword textureFilterMode);

/* 0x0057A550 */
Direct3DRenderStateApplyEaxCf5 __thandor_eax_cf_preserve_ecx_edx
Direct3DRenderer_SetTexturePerspectiveEnabled(dword texturePerspectiveEnabled);

/* 0x0057CCB0 */
void __thandor_void_preserve_eax_ecx_edx
Direct3D_PrimitiveHandler_UntexturedPreset0(GraphicsPrimitivePacket *packet);

/* 0x0057CF20 */
void __thandor_void_preserve_eax_ecx_edx
Direct3D_PrimitiveHandler_UntexturedPreset2(GraphicsPrimitivePacket *packet);

/* 0x0057D1D0 */
void __thandor_void_preserve_eax_ecx_edx
Direct3D_PrimitiveHandler_UntexturedPreset3(GraphicsPrimitivePacket *packet);

/* 0x0057D480 */
void __thandor_void_preserve_eax_ecx_edx
Direct3D_PrimitiveHandler_UntexturedPreset4(GraphicsPrimitivePacket *packet);

/* 0x0057D730 */
void __thandor_void_preserve_eax_ecx_edx
Direct3D_PrimitiveHandler_TexturedPreset0(GraphicsPrimitivePacket *packet);

/* 0x0057DA50 */
void __thandor_void_preserve_eax_ecx_edx
Direct3D_PrimitiveHandler_TexturedPreset1(GraphicsPrimitivePacket *packet);

/* 0x0057DD70 */
void __thandor_void_preserve_eax_ecx_edx
Direct3D_PrimitiveHandler_TexturedPreset2(GraphicsPrimitivePacket *packet);

/* 0x0057E090 */
void __thandor_void_preserve_eax_ecx_edx
Direct3D_PrimitiveHandler_TexturedPreset3(GraphicsPrimitivePacket *packet);

/* 0x0057E3B0 */
void __thandor_void_preserve_eax_ecx_edx
Direct3D_PrimitiveHandler_TexturedPreset4(GraphicsPrimitivePacket *packet);

#endif /* THANDOR_GRAPHICS_BACKEND_DIRECT3D_H */

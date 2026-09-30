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

/* Stage numbers the Direct3DRenderer_Set* functions leave in g_PackageLastErrorPath (as decimal text) together
   with FATAL_ERROR_DIRECT3D_SETUP when a SetRenderState call fails */
#define DIRECT3D_RENDER_STATE_STAGE_ANTIALIAS 100
#define DIRECT3D_RENDER_STATE_STAGE_TEXTURE_MAG 110
#define DIRECT3D_RENDER_STATE_STAGE_TEXTURE_MIN 111
#define DIRECT3D_RENDER_STATE_STAGE_TEXTURE_PERSPECTIVE 120
/* The primitive handlers rescale converted fixed-point values by adding n * this to the bits of a nonzero
   float (THANDOR_FLOAT_ADD_EXPONENT_BITS), which multiplies it by 2^n: -12 for the screen coordinates, -20 for
   depth and texture coordinates, +20 (untextured) or +18 (textured) for rhw */
#define DIRECT3D_FLOAT_EXPONENT_STEP 0x800000
/* Functions are grouped by semantic ownership; address comments are executable virtual addresses. */

/* 0x00578270 */
int32_t __stdcall Direct3D_EnumDeviceCallback (TH_LEGACY_GUID *deviceGuid,char *description,char *deviceName, D3DDEVICEDESC_DX6 *hardwareDesc,D3DDEVICEDESC_DX6 *softwareDesc, GraphicsAdapterRecord *adapterContext);

/* 0x00578820 */
/* Called by IDirect3DDevice2::EnumTextureFormats: __stdcall (the original returns with RET 8). */
int32_t __stdcall
GraphicsDirect3D_SelectPreferredTextureFormatEnumCallback
          (DDSURFACEDESC_DX6 *surfaceDesc,TH_LEGACY_LPVOID context);

/* 0x0057A450 */
uint32_t Direct3DRenderer_SetAntialiasMode(uint32_t antialiasMode);

/* 0x0057A4C0 */
uint32_t Direct3DRenderer_SetTextureFilterMode(uint32_t textureFilterMode);

/* 0x0057A550 */
uint32_t Direct3DRenderer_SetTexturePerspectiveEnabled(uint32_t texturePerspectiveEnabled);

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

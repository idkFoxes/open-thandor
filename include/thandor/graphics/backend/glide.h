/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/include/thandor/graphics/backend/glide.h
 * Reverse engineering by idkFoxes 2026
 */

#ifndef THANDOR_GRAPHICS_BACKEND_GLIDE_H
#define THANDOR_GRAPHICS_BACKEND_GLIDE_H

#include <thandor/generated/types.h>
#include <thandor/core/contracts.h>

/* Submodule: graphics/backend/glide. */

/* Glide has no DirectDraw surfaces: Glide3_Framebuffer_Present passes this value as the "back surface" to
   Glide3_Cursor_ComposeBeforePresent, which draws the cursor only for it (the generic DirectDraw callers pass
   real surface pointers and are ignored). */
#define GLIDE_CURSOR_PRESENT_SENTINEL ((IDirectDrawSurface3 *)0x1)
/* Bit pattern of the float 1.0f (guGammaCorrectionRGB takes floats) */
#define GLIDE_FLOAT_BITS_ONE 0x3f800000

/* glide.h GrResolution: one 16-byte entry of the list grQueryResolutions writes (Glide3_InitAndEnumerate,
   GraphicsGlide3_ApplyDisplayModeAndInitializeResources). */
typedef struct GrResolution {
  int32_t resolution;      /* GR_RESOLUTION_* */
  uint32_t refresh;        /* GR_REFRESH_*: 0 = 60 Hz .. 8 = 120 Hz (g_GlideRefreshRatesHz) */
  int32_t numColorBuffers;
  int32_t numAuxBuffers;
} GrResolution;

/* Functions are grouped by semantic ownership; address comments are executable virtual addresses. */

/* 0x005801B0 */
bool Glide3_TextureSet_CreateBackend(GraphicsTextureSet *textureSet,GraphicsTextureSourceAsset *sourceAsset);

/* 0x00580430 */
void Glide3_TextureResource_ReinitializeAll(void);

/* 0x0057F0C0 */
void __cdecl GlideBackend_ShutdownWrapper(void);

/* 0x0057F0F0 */
bool GraphicsGlide3_ApplyDisplayModeAndInitializeResources
          (FrontendDisplayAdapterIndex adapterIndex,GraphicsBitsPerPixel bitsPerPixel,
          GraphicsPixelDimension height,GraphicsPixelDimension width);

/* 0x0057F7B0 */
void Glide3_DrawPrimitiveQueue(int32_t clipMaxY,int32_t clipMaxX,int32_t clipMinY,int32_t clipMinX,
          GraphicsPrimitiveQueue *queue);

/* 0x005802F0 */
GraphicsTextureSourceAsset * Glide3_TextureSet_DestroyBackend(GraphicsTextureSet *setRegisterMirror,GraphicsTextureSet *set);

/* 0x00580470 */
void Glide3_Framebuffer_Present(SoftwareFramebufferAccess *framebuffer);

/* 0x0057EE90 */
StatusResult Glide3_InitAndEnumerate(void);

/* 0x0057F0D0 */
void GlideBackend_BeginSceneNoOp(void);

/* 0x0057F0E0 */
void GlideBackend_EndSceneNoOp(void);

/* 0x0057F740 */
void Glide3_ClearViewport(GraphicsScreenCoordinate clipMaxY,GraphicsScreenCoordinate clipMaxX,
          GraphicsScreenCoordinate clipMinY,GraphicsScreenCoordinate clipMinX);

/* 0x00580370 */
void Glide3_TextureSet_RefreshColor(GraphicsSubresourceIndex subresourceIndex,GraphicsTextureSet *set);

/* 0x005803D0 */
void Glide3_TextureSet_RefreshAlpha(GraphicsSubresourceIndex subresourceIndex,GraphicsTextureSet *set);

/* 0x00580540 */
FramebufferCaptureResult Glide3_Framebuffer_CaptureRegion
          (GraphicsPixelDimension captureHeight,GraphicsPixelDimension captureWidth,
          GraphicsScreenCoordinate sourceY,GraphicsScreenCoordinate sourceX);

/* 0x005806E0 */
void Glide3_Cursor_RestoreAfterPresentNoOp(IDirectDrawSurface3 *backSurfaceSentinel);

/* 0x005807B0 */
void Glide3_TextureUpload_1x(GraphicsTextureResource *texture);

/* 0x00580960 */
void Glide3_TextureUpload_2x(GraphicsTextureResource *texture);

/* 0x00580C60 */
void Glide3_TextureUpload_4x(GraphicsTextureResource *texture);

/* 0x00581150 */
bool Glide3_TextureSource_BlitSourceAlpha(GraphicsScreenCoordinate clipMaxY,GraphicsScreenCoordinate clipMaxX,
          GraphicsScreenCoordinate clipMinY,GraphicsScreenCoordinate clipMinX,
          GraphicsScreenCoordinate drawY,GraphicsScreenCoordinate drawX,
          GraphicsSubresourceIndex subresourceIndex,GraphicsTextureSourceAsset *sourceAsset,
          SoftwareFramebufferAccess *framebuffer);

/* 0x005814D0 */
bool Glide3_TextureSource_BlitHalfSourceRgb(GraphicsScreenCoordinate clipMaxY,GraphicsScreenCoordinate clipMaxX,
          GraphicsScreenCoordinate clipMinY,GraphicsScreenCoordinate clipMinX,
          GraphicsScreenCoordinate drawY,GraphicsScreenCoordinate drawX,
          GraphicsSubresourceIndex subresourceIndex,GraphicsTextureSourceAsset *sourceAsset,
          SoftwareFramebufferAccess *framebuffer);

/* 0x005817F0 */
void Glide3_TextureSource_StretchDirectColorBilinear
          (GraphicsPixelDimension destinationHeight,GraphicsPixelDimension destinationWidth,
          GraphicsScreenCoordinate destinationY,GraphicsScreenCoordinate destinationX,
          GraphicsSubresourceIndex subresourceIndex,GraphicsTextureSourceAsset *sourceAsset,
          SoftwareFramebufferAccess *framebuffer);

/* 0x00581A90 */
void Glide3_TextureSource_BlitIntegerScaledSourceAlpha
          (GraphicsScreenCoordinate clipMaxY,GraphicsScreenCoordinate clipMaxX,
          GraphicsScreenCoordinate clipMinY,GraphicsScreenCoordinate clipMinX,
          GraphicsScreenCoordinate drawY,GraphicsScreenCoordinate drawX,
          GraphicsIntegerScale integerScale,GraphicsSubresourceIndex subresourceIndex,
          GraphicsTextureSourceAsset *sourceAsset,SoftwareFramebufferAccess *framebuffer);

/* 0x00581EF0 */
void Glide3_TextureSource_BlitSourceAlphaPaletteBank
          (GraphicsScreenCoordinate clipMaxY,GraphicsScreenCoordinate clipMaxX,
          GraphicsScreenCoordinate clipMinY,GraphicsScreenCoordinate clipMinX,
          GraphicsScreenCoordinate drawY,GraphicsScreenCoordinate drawX,
          PaletteBankIndex paletteBankIndex,GraphicsSubresourceIndex subresourceIndex,
          GraphicsTextureSourceAsset *sourceAsset,SoftwareFramebufferAccess *framebuffer);

/* 0x00582290 */
bool Glide3_TextureSource_BlitSaturatedAddRgb(GraphicsScreenCoordinate clipMaxY,GraphicsScreenCoordinate clipMaxX,
          GraphicsScreenCoordinate clipMinY,GraphicsScreenCoordinate clipMinX,
          GraphicsScreenCoordinate drawY,GraphicsScreenCoordinate drawX,
          GraphicsSubresourceIndex subresourceIndex,GraphicsTextureSourceAsset *sourceAsset,
          SoftwareFramebufferAccess *framebuffer);

/* 0x00582580 */
bool Glide3_TextureSource_BlitHalfRgbSaturatedAdd(GraphicsScreenCoordinate clipMaxY,GraphicsScreenCoordinate clipMaxX,
          GraphicsScreenCoordinate clipMinY,GraphicsScreenCoordinate clipMinX,
          GraphicsScreenCoordinate drawY,GraphicsScreenCoordinate drawX,
          GraphicsSubresourceIndex subresourceIndex,GraphicsTextureSourceAsset *sourceAsset,
          SoftwareFramebufferAccess *framebuffer);

/* 0x00582870 */
bool Glide3_TextureSource_BlitModulatedSourceAlpha(GraphicsScreenCoordinate clipMaxY,GraphicsScreenCoordinate clipMaxX,
          GraphicsScreenCoordinate clipMinY,GraphicsScreenCoordinate clipMinX,
          GraphicsScreenCoordinate drawY,GraphicsScreenCoordinate drawX,
          PackedArgb32 modulationArgb8888,GraphicsSubresourceIndex subresourceIndex,
          GraphicsTextureSourceAsset *sourceAsset,SoftwareFramebufferAccess *framebuffer);

/* 0x00582D30 */
void Glide3_Framebuffer_FillRectArgb(GraphicsScreenCoordinate clipMaxY,GraphicsScreenCoordinate clipMaxX,
          GraphicsScreenCoordinate clipMinY,GraphicsScreenCoordinate clipMinX,
          GraphicsScreenCoordinate rectMaxY,GraphicsScreenCoordinate rectMaxX,
          GraphicsScreenCoordinate rectMinY,GraphicsScreenCoordinate rectMinX,PackedArgb32 argb8888,
          SoftwareFramebufferAccess *framebuffer);

/* 0x005806F0 */
void Glide3_Cursor_ComposeBeforePresent(IDirectDrawSurface3 *backSurfaceSentinel);

/* 0x0057F5C0 */
void Glide3_Shutdown(void);

/* 0x0057F630 */
bool Glide3_Framebuffer_BeginAccess(void);

/* 0x0057F6E0 */
void Glide3_Framebuffer_EndAccess(void);

/* 0x0057FF50 */
void Glide3_TextureResource_EnsureResident(GraphicsTextureResource *texture);

/* 0x0057FD40 */
void Glide3_TextureResource_Initialize(GraphicsTextureResource *texture);

/* 0x0057FE80 */
void Glide3_TextureResource_Release(GraphicsTextureResource *texture);


/* 0x00580F60 */
void GraphicsGlide3_FillTextureDataConstant0FFF(GraphicsTextureResource *texture);

/* 0x00580FF0 */
void GraphicsGlide3_DownsampleAlpha8ToWhiteArgb4444(GraphicsTextureResource *texture);

/* 0x005810A0 */
void GraphicsGlide3_DownsampleAlternateAlphaSamplesToWhiteArgb4444(GraphicsTextureResource *texture);

#endif /* THANDOR_GRAPHICS_BACKEND_GLIDE_H */

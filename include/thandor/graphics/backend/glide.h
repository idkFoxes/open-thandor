#ifndef THANDOR_GRAPHICS_BACKEND_GLIDE_H
#define THANDOR_GRAPHICS_BACKEND_GLIDE_H

#include <thandor/generated/v523_types.h>
#include <thandor/core/contracts.h>

/* Submodule: graphics/backend/glide. */
/* Functions are grouped by semantic ownership; address comments are executable virtual addresses. */

/* 0x005801B0 */
GraphicsTextureSet * Glide3_TextureSet_CreateBackend(GraphicsTextureSourceAsset *sourceAsset);

/* 0x00580430 */
void __cdecl Glide3_TextureResource_ReinitializeAll(void);

/* 0x0057F0C0 */
void __cdecl GlideBackend_ShutdownWrapper(void);

/* 0x0057F0F0 */
void GraphicsGlide3_ApplyDisplayModeAndInitializeResourcesCf (FrontendDisplayAdapterIndex adapterIndex,GraphicsBitsPerPixel bitsPerPixel, GraphicsPixelDimension height,GraphicsPixelDimension width);

/* 0x0057F7B0 */
void Glide3_DrawPrimitiveQueue (sdword coordinate0,sdword coordinate1,sdword coordinate2,sdword coordinate3, GraphicsPrimitiveQueue *queue);

/* 0x005802F0 */
GraphicsTextureSourceAsset * Glide3_TextureSet_DestroyBackend(GraphicsTextureSet *set);

/* 0x00580470 */
void Glide3_Framebuffer_Present(SoftwareFramebufferAccess *framebuffer);

/* 0x0057EE90 */
dword __cdecl Glide3_InitAndEnumerate(void);

/* 0x0057F0D0 */
void __cdecl GlideBackend_BeginSceneNoOp(void);

/* 0x0057F0E0 */
void __cdecl GlideBackend_EndSceneNoOp(void);

/* 0x0057F740 */
void Glide3_ClearViewport (GraphicsScreenCoordinate coordinate0,GraphicsScreenCoordinate coordinate1, GraphicsScreenCoordinate coordinate2,GraphicsScreenCoordinate coordinate3);

/* 0x00580370 */
void Glide3_TextureSet_RefreshColor (GraphicsSubresourceIndex subresourceIndex,GraphicsTextureSet *set);

/* 0x005803D0 */
void Glide3_TextureSet_RefreshAlpha (GraphicsSubresourceIndex subresourceIndex,GraphicsTextureSet *set);

/* 0x00580540 */
GraphicsCapturedTextureSourceAsset * Glide3_Framebuffer_CaptureRegion (GraphicsPixelDimension captureHeight,GraphicsPixelDimension captureWidth, GraphicsScreenCoordinate sourceY,GraphicsScreenCoordinate sourceX);

/* 0x005806E0 */
void Glide3_Cursor_RestoreAfterPresentNoOp(IDirectDrawSurface3 *backSurfaceSentinel);

/* 0x005807B0 */
void Glide3_TextureUpload_1x(GraphicsTextureResource *texture);

/* 0x00580960 */
void Glide3_TextureUpload_2x(GraphicsTextureResource *texture);

/* 0x00580C60 */
void Glide3_TextureUpload_4x(GraphicsTextureResource *texture);

/* 0x00581150 */
qword Glide3_TextureSource_BlitSourceAlpha (GraphicsScreenCoordinate clipMaxY,GraphicsScreenCoordinate clipMaxX, GraphicsScreenCoordinate clipMinY,GraphicsScreenCoordinate clipMinX, GraphicsScreenCoordinate drawY,GraphicsScreenCoordinate drawX, GraphicsSubresourceIndex subresourceIndex,GraphicsTextureSourceAsset *sourceAsset, SoftwareFramebufferAccess *framebuffer);

/* 0x005814D0 */
qword Glide3_TextureSource_BlitHalfSourceRgb (GraphicsScreenCoordinate clipMaxY,GraphicsScreenCoordinate clipMaxX, GraphicsScreenCoordinate clipMinY,GraphicsScreenCoordinate clipMinX, GraphicsScreenCoordinate drawY,GraphicsScreenCoordinate drawX, GraphicsSubresourceIndex subresourceIndex,GraphicsTextureSourceAsset *sourceAsset, SoftwareFramebufferAccess *framebuffer);

/* 0x005817F0 */
void Glide3_TextureSource_StretchDirectColorBilinear (GraphicsPixelDimension destinationHeight,GraphicsPixelDimension destinationWidth, GraphicsScreenCoordinate destinationY,GraphicsScreenCoordinate destinationX, GraphicsSubresourceIndex subresourceIndex,GraphicsTextureSourceAsset *sourceAsset, SoftwareFramebufferAccess *framebuffer);

/* 0x00581A90 */
void Glide3_TextureSource_BlitIntegerScaledSourceAlpha (GraphicsScreenCoordinate clipMaxY,GraphicsScreenCoordinate clipMaxX, GraphicsScreenCoordinate clipMinY,GraphicsScreenCoordinate clipMinX, GraphicsScreenCoordinate drawY,GraphicsScreenCoordinate drawX, GraphicsIntegerScale integerScale,GraphicsSubresourceIndex subresourceIndex, GraphicsTextureSourceAsset *sourceAsset,SoftwareFramebufferAccess *framebuffer);

/* 0x00581EF0 */
void Glide3_TextureSource_BlitSourceAlphaPaletteBank (GraphicsScreenCoordinate clipMaxY,GraphicsScreenCoordinate clipMaxX, GraphicsScreenCoordinate clipMinY,GraphicsScreenCoordinate clipMinX, GraphicsScreenCoordinate drawY,GraphicsScreenCoordinate drawX, PaletteBankIndex paletteBankIndex,GraphicsSubresourceIndex subresourceIndex, GraphicsTextureSourceAsset *sourceAsset,SoftwareFramebufferAccess *framebuffer);

/* 0x00582290 */
qword Glide3_TextureSource_BlitSaturatedAddRgb (GraphicsScreenCoordinate clipMaxY,GraphicsScreenCoordinate clipMaxX, GraphicsScreenCoordinate clipMinY,GraphicsScreenCoordinate clipMinX, GraphicsScreenCoordinate drawY,GraphicsScreenCoordinate drawX, GraphicsSubresourceIndex subresourceIndex,GraphicsTextureSourceAsset *sourceAsset, SoftwareFramebufferAccess *framebuffer);

/* 0x00582580 */
qword Glide3_TextureSource_BlitHalfRgbSaturatedAdd (GraphicsScreenCoordinate clipMaxY,GraphicsScreenCoordinate clipMaxX, GraphicsScreenCoordinate clipMinY,GraphicsScreenCoordinate clipMinX, GraphicsScreenCoordinate drawY,GraphicsScreenCoordinate drawX, GraphicsSubresourceIndex subresourceIndex,GraphicsTextureSourceAsset *sourceAsset, SoftwareFramebufferAccess *framebuffer);

/* 0x00582870 */
qword Glide3_TextureSource_BlitModulatedSourceAlpha (GraphicsScreenCoordinate clipMaxY,GraphicsScreenCoordinate clipMaxX, GraphicsScreenCoordinate clipMinY,GraphicsScreenCoordinate clipMinX, GraphicsScreenCoordinate drawY,GraphicsScreenCoordinate drawX, PackedArgb32 modulationArgb8888,GraphicsSubresourceIndex subresourceIndex, GraphicsTextureSourceAsset *sourceAsset,SoftwareFramebufferAccess *framebuffer);

/* 0x00582D30 */
void Glide3_Framebuffer_FillRectArgb (GraphicsScreenCoordinate clipMaxY,GraphicsScreenCoordinate clipMaxX, GraphicsScreenCoordinate clipMinY,GraphicsScreenCoordinate clipMinX, GraphicsScreenCoordinate rectMaxY,GraphicsScreenCoordinate rectMaxX, GraphicsScreenCoordinate rectMinY,GraphicsScreenCoordinate rectMinX, PackedArgb32 argb8888,SoftwareFramebufferAccess *framebuffer);

/* 0x005806F0 */
void Glide3_Cursor_ComposeBeforePresent(IDirectDrawSurface3 *backSurfaceSentinel);

/* 0x0057F5C0 */
void __cdecl Glide3_Shutdown(void);

/* 0x0057F630 */
void __cdecl Glide3_Framebuffer_BeginAccess(void);

/* 0x0057F6E0 */
void __cdecl Glide3_Framebuffer_EndAccess(void);

/* 0x0057FF50 */
void Glide3_TextureResource_EnsureResident(GraphicsTextureResource *texture);

/* 0x0057FD40 */
GraphicsTextureResource * Glide3_TextureResource_Initialize(GraphicsTextureResource *texture);

/* 0x0057FE80 */
GraphicsTextureResource * Glide3_TextureResource_Release(GraphicsTextureResource *texture);

#endif /* THANDOR_GRAPHICS_BACKEND_GLIDE_H */

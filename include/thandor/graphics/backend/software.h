#ifndef THANDOR_GRAPHICS_BACKEND_SOFTWARE_H
#define THANDOR_GRAPHICS_BACKEND_SOFTWARE_H

#include <thandor/generated/v523_types.h>
#include <thandor/core/contracts.h>

/* Submodule: graphics/backend/software. */
/* Functions are grouped by semantic ownership; address comments are executable virtual addresses. */

/* 0x00519320 */
void SoftwareMaskBuffer_AdvancePatternByPercentTick(SoftwareMaskRuntimeView *maskRuntime);

/* 0x00485FD0 */
void SoftwareRenderer_ClearViewport (GraphicsScreenCoordinate clipMaxY,GraphicsScreenCoordinate clipMaxX, GraphicsScreenCoordinate clipMinY,GraphicsScreenCoordinate clipMinX);

/* 0x004D1560 */
void SoftwareRenderer_DrawQueue16Bit (GraphicsScreenCoordinate clipMaxY,GraphicsScreenCoordinate clipMaxX, GraphicsScreenCoordinate clipMinY,GraphicsScreenCoordinate clipMinX, GraphicsPrimitiveQueue *queue);

/* 0x004D15D0 */
void SoftwareRenderer_DrawQueueNon16Bit (GraphicsScreenCoordinate clipMaxY,GraphicsScreenCoordinate clipMaxX, GraphicsScreenCoordinate clipMinY,GraphicsScreenCoordinate clipMinX, GraphicsPrimitiveQueue *queue);

/* 0x004D1640 */
void SoftwareRenderer_DrawQueueAuxiliary (GraphicsScreenCoordinate clipMaxY,GraphicsScreenCoordinate clipMaxX,void *targetBase ,GraphicsPrimitiveQueue *queue);

/* 0x00486020 */
void SoftwareRenderer_DrawPrimitiveQueueBridge (GraphicsScreenCoordinate clipMaxY,GraphicsScreenCoordinate clipMaxX, GraphicsScreenCoordinate clipMinY,GraphicsScreenCoordinate clipMinX, GraphicsPrimitiveQueue *queue);

/* 0x00486050 */
void SoftwareGraphicsDispatch_SuccessNoOp(void);

/* 0x00486060 */
void SoftwareGraphicsDispatch_NoOp(void);

/* 0x004A8F80 */
void SoftwarePixelFormat_BaseDisplayModeHook (dword modeArg0,dword modeArg1,FrontendDisplayDimensionPixels height, FrontendDisplayDimensionPixels width);

/* 0x004A9110 */
SoftwareFramebufferAccess * SoftwareFramebuffer_Create (SoftwareFramebufferPixelSize bytesPerPixel,GraphicsPixelDimension height, GraphicsPixelDimension width);

/* 0x004A9160 */
void SoftwareFramebuffer_Destroy(SoftwareFramebufferAccess *framebuffer);

/* 0x004A9180 */
void SoftwarePixelFormat_BuildChannelPackTables (SoftwareColorTransformQ16 colorScaleQ16,SoftwareColorTransformQ16 colorBiasQ16);

/* 0x004A93C0 */
qword SoftwareTextureSource_BlitSourceAlpha16 (GraphicsScreenCoordinate clipMaxY,GraphicsScreenCoordinate clipMaxX, GraphicsScreenCoordinate clipMinY,GraphicsScreenCoordinate clipMinX, GraphicsScreenCoordinate drawY,GraphicsScreenCoordinate drawX, GraphicsSubresourceIndex subresourceIndex,GraphicsTextureSourceAsset *sourceAsset, SoftwareFramebufferAccess *framebuffer);

/* 0x004A9710 */
qword SoftwareTextureSource_BlitSourceAlpha32 (GraphicsScreenCoordinate clipMaxY,GraphicsScreenCoordinate clipMaxX, GraphicsScreenCoordinate clipMinY,GraphicsScreenCoordinate clipMinX, GraphicsScreenCoordinate drawY,GraphicsScreenCoordinate drawX, GraphicsSubresourceIndex subresourceIndex,GraphicsTextureSourceAsset *sourceAsset, SoftwareFramebufferAccess *framebuffer);

/* 0x004A9B20 */
qword SoftwareTextureSource_BlitHalfSourceRgb16 (GraphicsScreenCoordinate clipMaxY,GraphicsScreenCoordinate clipMaxX, GraphicsScreenCoordinate clipMinY,GraphicsScreenCoordinate clipMinX, GraphicsScreenCoordinate drawY,GraphicsScreenCoordinate drawX, GraphicsSubresourceIndex subresourceIndex,GraphicsTextureSourceAsset *sourceAsset, SoftwareFramebufferAccess *framebuffer);

/* 0x004A9E10 */
qword SoftwareTextureSource_BlitHalfSourceRgb32 (GraphicsScreenCoordinate clipMaxY,GraphicsScreenCoordinate clipMaxX, GraphicsScreenCoordinate clipMinY,GraphicsScreenCoordinate clipMinX, GraphicsScreenCoordinate drawY,GraphicsScreenCoordinate drawX, GraphicsSubresourceIndex subresourceIndex,GraphicsTextureSourceAsset *sourceAsset, SoftwareFramebufferAccess *framebuffer);

/* 0x004AA170 */
void SoftwareTextureSource_StretchDirectColorBilinear16 (GraphicsPixelDimension destinationHeight,GraphicsPixelDimension destinationWidth, GraphicsScreenCoordinate destinationY,GraphicsScreenCoordinate destinationX, GraphicsSubresourceIndex subresourceIndex,GraphicsTextureSourceAsset *sourceAsset, SoftwareFramebufferAccess *framebuffer);

/* 0x004AA3F0 */
void SoftwareTextureSource_StretchDirectColorBilinear32 (GraphicsPixelDimension destinationHeight,GraphicsPixelDimension destinationWidth, GraphicsScreenCoordinate destinationY,GraphicsScreenCoordinate destinationX, GraphicsSubresourceIndex subresourceIndex,GraphicsTextureSourceAsset *sourceAsset, SoftwareFramebufferAccess *framebuffer);

/* 0x004AA630 */
void SoftwareTextureSource_BlitIntegerScaledSourceAlpha16 (GraphicsScreenCoordinate clipMaxY,GraphicsScreenCoordinate clipMaxX, GraphicsScreenCoordinate clipMinY,GraphicsScreenCoordinate clipMinX, GraphicsScreenCoordinate drawY,GraphicsScreenCoordinate drawX, GraphicsIntegerScale integerScale,GraphicsSubresourceIndex subresourceIndex, GraphicsTextureSourceAsset *sourceAsset,SoftwareFramebufferAccess *framebuffer);

/* 0x004AAA40 */
void SoftwareTextureSource_BlitIntegerScaledSourceAlpha32 (GraphicsScreenCoordinate clipMaxY,GraphicsScreenCoordinate clipMaxX, GraphicsScreenCoordinate clipMinY,GraphicsScreenCoordinate clipMinX, GraphicsScreenCoordinate drawY,GraphicsScreenCoordinate drawX, GraphicsIntegerScale integerScale,GraphicsSubresourceIndex subresourceIndex, GraphicsTextureSourceAsset *sourceAsset,SoftwareFramebufferAccess *framebuffer);

/* 0x004AADE0 */
void SoftwareTextureSource_BlitSourceAlphaPaletteBank16 (GraphicsScreenCoordinate clipMaxY,GraphicsScreenCoordinate clipMaxX, GraphicsScreenCoordinate clipMinY,GraphicsScreenCoordinate clipMinX, GraphicsScreenCoordinate drawY,GraphicsScreenCoordinate drawX, PaletteBankIndex paletteBankIndex,GraphicsSubresourceIndex subresourceIndex, GraphicsTextureSourceAsset *sourceAsset,SoftwareFramebufferAccess *framebuffer);

/* 0x004AB150 */
void SoftwareTextureSource_BlitSourceAlphaPaletteBank32 (GraphicsScreenCoordinate clipMaxY,GraphicsScreenCoordinate clipMaxX, GraphicsScreenCoordinate clipMinY,GraphicsScreenCoordinate clipMinX, GraphicsScreenCoordinate drawY,GraphicsScreenCoordinate drawX, PaletteBankIndex paletteBankIndex,GraphicsSubresourceIndex subresourceIndex, GraphicsTextureSourceAsset *sourceAsset,SoftwareFramebufferAccess *framebuffer);

/* 0x004AB4A0 */
qword SoftwareTextureSource_BlitSaturatedAddRgb16 (GraphicsScreenCoordinate clipMaxY,GraphicsScreenCoordinate clipMaxX, GraphicsScreenCoordinate clipMinY,GraphicsScreenCoordinate clipMinX, GraphicsScreenCoordinate drawY,GraphicsScreenCoordinate drawX, GraphicsSubresourceIndex subresourceIndex,GraphicsTextureSourceAsset *sourceAsset, SoftwareFramebufferAccess *framebuffer);

/* 0x004AB750 */
qword SoftwareTextureSource_BlitSaturatedAddRgb32 (GraphicsScreenCoordinate clipMaxY,GraphicsScreenCoordinate clipMaxX, GraphicsScreenCoordinate clipMinY,GraphicsScreenCoordinate clipMinX, GraphicsScreenCoordinate drawY,GraphicsScreenCoordinate drawX, GraphicsSubresourceIndex subresourceIndex,GraphicsTextureSourceAsset *sourceAsset, SoftwareFramebufferAccess *framebuffer);

/* 0x004ABA70 */
qword SoftwareTextureSource_BlitHalfRgbSaturatedAdd16 (GraphicsScreenCoordinate clipMaxY,GraphicsScreenCoordinate clipMaxX, GraphicsScreenCoordinate clipMinY,GraphicsScreenCoordinate clipMinX, GraphicsScreenCoordinate drawY,GraphicsScreenCoordinate drawX, GraphicsSubresourceIndex subresourceIndex,GraphicsTextureSourceAsset *sourceAsset, SoftwareFramebufferAccess *framebuffer);

/* 0x004ABD20 */
qword SoftwareTextureSource_BlitHalfRgbSaturatedAdd32 (GraphicsScreenCoordinate clipMaxY,GraphicsScreenCoordinate clipMaxX, GraphicsScreenCoordinate clipMinY,GraphicsScreenCoordinate clipMinX, GraphicsScreenCoordinate drawY,GraphicsScreenCoordinate drawX, GraphicsSubresourceIndex subresourceIndex,GraphicsTextureSourceAsset *sourceAsset, SoftwareFramebufferAccess *framebuffer);

/* 0x004AC040 */
qword SoftwareTextureSource_BlitModulatedSourceAlpha16 (GraphicsScreenCoordinate clipMaxY,GraphicsScreenCoordinate clipMaxX, GraphicsScreenCoordinate clipMinY,GraphicsScreenCoordinate clipMinX, GraphicsScreenCoordinate drawY,GraphicsScreenCoordinate drawX, PackedArgb32 modulationArgb8888,GraphicsSubresourceIndex subresourceIndex, GraphicsTextureSourceAsset *sourceAsset,SoftwareFramebufferAccess *framebuffer);

/* 0x004AC4C0 */
qword SoftwareTextureSource_BlitModulatedSourceAlpha32 (GraphicsScreenCoordinate clipMaxY,GraphicsScreenCoordinate clipMaxX, GraphicsScreenCoordinate clipMinY,GraphicsScreenCoordinate clipMinX, GraphicsScreenCoordinate drawY,GraphicsScreenCoordinate drawX, PackedArgb32 modulationArgb8888,GraphicsSubresourceIndex subresourceIndex, GraphicsTextureSourceAsset *sourceAsset,SoftwareFramebufferAccess *framebuffer);

/* 0x004AD110 */
void SoftwareFramebuffer_FillRectArgb16 (GraphicsScreenCoordinate clipMaxY,GraphicsScreenCoordinate clipMaxX, GraphicsScreenCoordinate clipMinY,GraphicsScreenCoordinate clipMinX, GraphicsScreenCoordinate rectMaxY,GraphicsScreenCoordinate rectMaxX, GraphicsScreenCoordinate rectMinY,GraphicsScreenCoordinate rectMinX, PackedArgb32 argb8888,SoftwareFramebufferAccess *framebuffer);

/* 0x004AD2A0 */
void SoftwareFramebuffer_FillRectArgb32 (GraphicsScreenCoordinate clipMaxY,GraphicsScreenCoordinate clipMaxX, GraphicsScreenCoordinate clipMinY,GraphicsScreenCoordinate clipMinX, GraphicsScreenCoordinate rectMaxY,GraphicsScreenCoordinate rectMaxX, GraphicsScreenCoordinate rectMinY,GraphicsScreenCoordinate rectMinX, PackedArgb32 argb8888,SoftwareFramebufferAccess *framebuffer);

/* 0x004AD410 */
void SoftwareFramebuffer_CopyRegionToOrigin (GraphicsPixelDimension copyHeight,GraphicsPixelDimension copyWidth, GraphicsScreenCoordinate sourceY,GraphicsScreenCoordinate sourceX, SoftwareFramebufferAccess *destination,SoftwareFramebufferAccess *source);

/* 0x004AD520 */
void SoftwareFramebuffer_CopyOriginToRegion (GraphicsPixelDimension copyHeight,GraphicsPixelDimension copyWidth, GraphicsScreenCoordinate destinationY,GraphicsScreenCoordinate destinationX, SoftwareFramebufferAccess *source,SoftwareFramebufferAccess *destination);

/* 0x004D1710 */
void SoftwareRaster16_Mode16 (GraphicsScreenCoordinate clipMaxY,GraphicsScreenCoordinate clipMaxX, GraphicsScreenCoordinate clipMinY,GraphicsScreenCoordinate clipMinX, GraphicsPrimitivePacket *packet);

/* 0x004D2990 */
void SoftwareRaster16_Mode22 (GraphicsScreenCoordinate clipMaxY,GraphicsScreenCoordinate clipMaxX, GraphicsScreenCoordinate clipMinY,GraphicsScreenCoordinate clipMinX, GraphicsPrimitivePacket *packet);

/* 0x004D3E90 */
void SoftwareRaster16_Mode17 (GraphicsScreenCoordinate clipMaxY,GraphicsScreenCoordinate clipMaxX, GraphicsScreenCoordinate clipMinY,GraphicsScreenCoordinate clipMinX, GraphicsPrimitivePacket *packet);

/* 0x004D5310 */
void SoftwareRaster16_Mode18 (GraphicsScreenCoordinate clipMaxY,GraphicsScreenCoordinate clipMaxX, GraphicsScreenCoordinate clipMinY,GraphicsScreenCoordinate clipMinX, GraphicsPrimitivePacket *packet);

/* 0x004D6690 */
void SoftwareRaster16_Mode20 (GraphicsScreenCoordinate clipMaxY,GraphicsScreenCoordinate clipMaxX, GraphicsScreenCoordinate clipMinY,GraphicsScreenCoordinate clipMinX, GraphicsPrimitivePacket *packet);

/* 0x004D7B90 */
void SoftwareRaster16_Mode24 (GraphicsScreenCoordinate clipMaxY,GraphicsScreenCoordinate clipMaxX, GraphicsScreenCoordinate clipMinY,GraphicsScreenCoordinate clipMinX, GraphicsPrimitivePacket *packet);

/* 0x004D8A90 */
void SoftwareRaster16_Mode30 (GraphicsScreenCoordinate clipMaxY,GraphicsScreenCoordinate clipMaxX, GraphicsScreenCoordinate clipMinY,GraphicsScreenCoordinate clipMinX, GraphicsPrimitivePacket *packet);

/* 0x004D9C10 */
void SoftwareRaster16_Mode25 (GraphicsScreenCoordinate clipMaxY,GraphicsScreenCoordinate clipMaxX, GraphicsScreenCoordinate clipMinY,GraphicsScreenCoordinate clipMinX, GraphicsPrimitivePacket *packet);

/* 0x004DAD10 */
void SoftwareRaster16_Mode26 (GraphicsScreenCoordinate clipMaxY,GraphicsScreenCoordinate clipMaxX, GraphicsScreenCoordinate clipMinY,GraphicsScreenCoordinate clipMinX, GraphicsPrimitivePacket *packet);

/* 0x004DBD10 */
void SoftwareRaster16_Mode28 (GraphicsScreenCoordinate clipMaxY,GraphicsScreenCoordinate clipMaxX, GraphicsScreenCoordinate clipMinY,GraphicsScreenCoordinate clipMinX, GraphicsPrimitivePacket *packet);

/* 0x004DCE90 */
void SoftwareRaster16_Mode00 (GraphicsScreenCoordinate clipMaxY,GraphicsScreenCoordinate clipMaxX, GraphicsScreenCoordinate clipMinY,GraphicsScreenCoordinate clipMinX, GraphicsPrimitivePacket *packet);

/* 0x004DD700 */
void SoftwareRaster16_Mode06 (GraphicsScreenCoordinate clipMaxY,GraphicsScreenCoordinate clipMaxX, GraphicsScreenCoordinate clipMinY,GraphicsScreenCoordinate clipMinX, GraphicsPrimitivePacket *packet);

/* 0x004DE0C0 */
void SoftwareRaster16_Mode01 (GraphicsScreenCoordinate clipMaxY,GraphicsScreenCoordinate clipMaxX, GraphicsScreenCoordinate clipMinY,GraphicsScreenCoordinate clipMinX, GraphicsPrimitivePacket *packet);

/* 0x004DEA40 */
void SoftwareRaster16_Mode02 (GraphicsScreenCoordinate clipMaxY,GraphicsScreenCoordinate clipMaxX, GraphicsScreenCoordinate clipMinY,GraphicsScreenCoordinate clipMinX, GraphicsPrimitivePacket *packet);

/* 0x004DF2F0 */
void SoftwareRaster16_Mode04 (GraphicsScreenCoordinate clipMaxY,GraphicsScreenCoordinate clipMaxX, GraphicsScreenCoordinate clipMinY,GraphicsScreenCoordinate clipMinX, GraphicsPrimitivePacket *packet);

/* 0x004DFCB0 */
void SoftwareRaster16_Mode08 (GraphicsScreenCoordinate clipMaxY,GraphicsScreenCoordinate clipMaxX, GraphicsScreenCoordinate clipMinY,GraphicsScreenCoordinate clipMinX, GraphicsPrimitivePacket *packet);

/* 0x004E0250 */
void SoftwareRaster16_Mode14 (GraphicsScreenCoordinate clipMaxY,GraphicsScreenCoordinate clipMaxX, GraphicsScreenCoordinate clipMinY,GraphicsScreenCoordinate clipMinX, GraphicsPrimitivePacket *packet);

/* 0x004E0940 */
void SoftwareRaster16_Mode09 (GraphicsScreenCoordinate clipMaxY,GraphicsScreenCoordinate clipMaxX, GraphicsScreenCoordinate clipMinY,GraphicsScreenCoordinate clipMinX, GraphicsPrimitivePacket *packet);

/* 0x004E0FF0 */
void SoftwareRaster16_Mode10 (GraphicsScreenCoordinate clipMaxY,GraphicsScreenCoordinate clipMaxX, GraphicsScreenCoordinate clipMinY,GraphicsScreenCoordinate clipMinX, GraphicsPrimitivePacket *packet);

/* 0x004E15D0 */
void SoftwareRaster16_Mode12 (GraphicsScreenCoordinate clipMaxY,GraphicsScreenCoordinate clipMaxX, GraphicsScreenCoordinate clipMinY,GraphicsScreenCoordinate clipMinX, GraphicsPrimitivePacket *packet);

/* 0x004E1CC0 */
void SoftwareRasterNon16_Mode16 (GraphicsScreenCoordinate clipMaxY,GraphicsScreenCoordinate clipMaxX, GraphicsScreenCoordinate clipMinY,GraphicsScreenCoordinate clipMinX, GraphicsPrimitivePacket *packet);

/* 0x004E2E00 */
void SoftwareRasterNon16_Mode22 (GraphicsScreenCoordinate clipMaxY,GraphicsScreenCoordinate clipMaxX, GraphicsScreenCoordinate clipMinY,GraphicsScreenCoordinate clipMinX, GraphicsPrimitivePacket *packet);

/* 0x004E4180 */
void SoftwareRasterNon16_Mode17 (GraphicsScreenCoordinate clipMaxY,GraphicsScreenCoordinate clipMaxX, GraphicsScreenCoordinate clipMinY,GraphicsScreenCoordinate clipMinX, GraphicsPrimitivePacket *packet);

/* 0x004E5440 */
void SoftwareRasterNon16_Mode18 (GraphicsScreenCoordinate clipMaxY,GraphicsScreenCoordinate clipMaxX, GraphicsScreenCoordinate clipMinY,GraphicsScreenCoordinate clipMinX, GraphicsPrimitivePacket *packet);

/* 0x004E65C0 */
void SoftwareRasterNon16_Mode20 (GraphicsScreenCoordinate clipMaxY,GraphicsScreenCoordinate clipMaxX, GraphicsScreenCoordinate clipMinY,GraphicsScreenCoordinate clipMinX, GraphicsPrimitivePacket *packet);

/* 0x004E7940 */
void SoftwareRasterNon16_Mode24 (GraphicsScreenCoordinate clipMaxY,GraphicsScreenCoordinate clipMaxX, GraphicsScreenCoordinate clipMinY,GraphicsScreenCoordinate clipMinX, GraphicsPrimitivePacket *packet);

/* 0x004E86D0 */
void SoftwareRasterNon16_Mode30 (GraphicsScreenCoordinate clipMaxY,GraphicsScreenCoordinate clipMaxX, GraphicsScreenCoordinate clipMinY,GraphicsScreenCoordinate clipMinX, GraphicsPrimitivePacket *packet);

/* 0x004E96D0 */
void SoftwareRasterNon16_Mode25 (GraphicsScreenCoordinate clipMaxY,GraphicsScreenCoordinate clipMaxX, GraphicsScreenCoordinate clipMinY,GraphicsScreenCoordinate clipMinX, GraphicsPrimitivePacket *packet);

/* 0x004EA610 */
void SoftwareRasterNon16_Mode26 (GraphicsScreenCoordinate clipMaxY,GraphicsScreenCoordinate clipMaxX, GraphicsScreenCoordinate clipMinY,GraphicsScreenCoordinate clipMinX, GraphicsPrimitivePacket *packet);

/* 0x004EB3E0 */
void SoftwareRasterNon16_Mode28 (GraphicsScreenCoordinate clipMaxY,GraphicsScreenCoordinate clipMaxX, GraphicsScreenCoordinate clipMinY,GraphicsScreenCoordinate clipMinX, GraphicsPrimitivePacket *packet);

/* 0x004EC3E0 */
void SoftwareRasterNon16_Mode00 (GraphicsScreenCoordinate clipMaxY,GraphicsScreenCoordinate clipMaxX, GraphicsScreenCoordinate clipMinY,GraphicsScreenCoordinate clipMinX, GraphicsPrimitivePacket *packet);

/* 0x004ECB90 */
void SoftwareRasterNon16_Mode06 (GraphicsScreenCoordinate clipMaxY,GraphicsScreenCoordinate clipMaxX, GraphicsScreenCoordinate clipMinY,GraphicsScreenCoordinate clipMinX, GraphicsPrimitivePacket *packet);

/* 0x004ED440 */
void SoftwareRasterNon16_Mode01 (GraphicsScreenCoordinate clipMaxY,GraphicsScreenCoordinate clipMaxX, GraphicsScreenCoordinate clipMinY,GraphicsScreenCoordinate clipMinX, GraphicsPrimitivePacket *packet);

/* 0x004EDCB0 */
void SoftwareRasterNon16_Mode02 (GraphicsScreenCoordinate clipMaxY,GraphicsScreenCoordinate clipMaxX, GraphicsScreenCoordinate clipMinY,GraphicsScreenCoordinate clipMinX, GraphicsPrimitivePacket *packet);

/* 0x004EE4A0 */
void SoftwareRasterNon16_Mode04 (GraphicsScreenCoordinate clipMaxY,GraphicsScreenCoordinate clipMaxX, GraphicsScreenCoordinate clipMinY,GraphicsScreenCoordinate clipMinX, GraphicsPrimitivePacket *packet);

/* 0x004EED50 */
void SoftwareRasterNon16_Mode08 (GraphicsScreenCoordinate clipMaxY,GraphicsScreenCoordinate clipMaxX, GraphicsScreenCoordinate clipMinY,GraphicsScreenCoordinate clipMinX, GraphicsPrimitivePacket *packet);

/* 0x004EF230 */
void SoftwareRasterNon16_Mode14 (GraphicsScreenCoordinate clipMaxY,GraphicsScreenCoordinate clipMaxX, GraphicsScreenCoordinate clipMinY,GraphicsScreenCoordinate clipMinX, GraphicsPrimitivePacket *packet);

/* 0x004EF810 */
void SoftwareRasterNon16_Mode09 (GraphicsScreenCoordinate clipMaxY,GraphicsScreenCoordinate clipMaxX, GraphicsScreenCoordinate clipMinY,GraphicsScreenCoordinate clipMinX, GraphicsPrimitivePacket *packet);

/* 0x004EFDB0 */
void SoftwareRasterNon16_Mode10 (GraphicsScreenCoordinate clipMaxY,GraphicsScreenCoordinate clipMaxX, GraphicsScreenCoordinate clipMinY,GraphicsScreenCoordinate clipMinX, GraphicsPrimitivePacket *packet);

/* 0x004F02D0 */
void SoftwareRasterNon16_Mode12 (GraphicsScreenCoordinate clipMaxY,GraphicsScreenCoordinate clipMaxX, GraphicsScreenCoordinate clipMinY,GraphicsScreenCoordinate clipMinX, GraphicsPrimitivePacket *packet);

/* 0x004F08B0 */
void SoftwareRasterAux_Mode16 (GraphicsScreenCoordinate clipMaxY,GraphicsScreenCoordinate clipMaxX, GraphicsScreenCoordinate clipMinY,GraphicsScreenCoordinate clipMinX, GraphicsPrimitivePacket *packet);

/* 0x004F19E0 */
void SoftwareRasterAux_Mode22 (GraphicsScreenCoordinate clipMaxY,GraphicsScreenCoordinate clipMaxX, GraphicsScreenCoordinate clipMinY,GraphicsScreenCoordinate clipMinX, GraphicsPrimitivePacket *packet);

/* 0x004F2BD0 */
void SoftwareRasterAux_Mode17 (GraphicsScreenCoordinate clipMaxY,GraphicsScreenCoordinate clipMaxX, GraphicsScreenCoordinate clipMinY,GraphicsScreenCoordinate clipMinX, GraphicsPrimitivePacket *packet);

/* 0x004F3D40 */
void SoftwareRasterAux_Mode18 (GraphicsScreenCoordinate clipMaxY,GraphicsScreenCoordinate clipMaxX, GraphicsScreenCoordinate clipMinY,GraphicsScreenCoordinate clipMinX, GraphicsPrimitivePacket *packet);

/* 0x004F4EB0 */
void SoftwareRasterAux_Mode20 (GraphicsScreenCoordinate clipMaxY,GraphicsScreenCoordinate clipMaxX, GraphicsScreenCoordinate clipMinY,GraphicsScreenCoordinate clipMinX, GraphicsPrimitivePacket *packet);

/* 0x004F60A0 */
void SoftwareRasterAux_Mode24 (GraphicsScreenCoordinate clipMaxY,GraphicsScreenCoordinate clipMaxX, GraphicsScreenCoordinate clipMinY,GraphicsScreenCoordinate clipMinX, GraphicsPrimitivePacket *packet);

/* 0x004F6E20 */
void SoftwareRasterAux_Mode30 (GraphicsScreenCoordinate clipMaxY,GraphicsScreenCoordinate clipMaxX, GraphicsScreenCoordinate clipMinY,GraphicsScreenCoordinate clipMinX, GraphicsPrimitivePacket *packet);

/* 0x004F7C70 */
void SoftwareRasterAux_Mode25 (GraphicsScreenCoordinate clipMaxY,GraphicsScreenCoordinate clipMaxX, GraphicsScreenCoordinate clipMinY,GraphicsScreenCoordinate clipMinX, GraphicsPrimitivePacket *packet);

/* 0x004F8A30 */
void SoftwareRasterAux_Mode26 (GraphicsScreenCoordinate clipMaxY,GraphicsScreenCoordinate clipMaxX, GraphicsScreenCoordinate clipMinY,GraphicsScreenCoordinate clipMinX, GraphicsPrimitivePacket *packet);

/* 0x004F97F0 */
void SoftwareRasterAux_Mode28 (GraphicsScreenCoordinate clipMaxY,GraphicsScreenCoordinate clipMaxX, GraphicsScreenCoordinate clipMinY,GraphicsScreenCoordinate clipMinX, GraphicsPrimitivePacket *packet);

/* 0x004FA640 */
void SoftwareRasterAux_Mode00 (GraphicsScreenCoordinate clipMaxY,GraphicsScreenCoordinate clipMaxX, GraphicsScreenCoordinate clipMinY,GraphicsScreenCoordinate clipMinX, GraphicsPrimitivePacket *packet);

/* 0x004FADD0 */
void SoftwareRasterAux_Mode06 (GraphicsScreenCoordinate clipMaxY,GraphicsScreenCoordinate clipMaxX, GraphicsScreenCoordinate clipMinY,GraphicsScreenCoordinate clipMinX, GraphicsPrimitivePacket *packet);

/* 0x004FB5A0 */
void SoftwareRasterAux_Mode01 (GraphicsScreenCoordinate clipMaxY,GraphicsScreenCoordinate clipMaxX, GraphicsScreenCoordinate clipMinY,GraphicsScreenCoordinate clipMinX, GraphicsPrimitivePacket *packet);

/* 0x004FBD70 */
void SoftwareRasterAux_Mode02 (GraphicsScreenCoordinate clipMaxY,GraphicsScreenCoordinate clipMaxX, GraphicsScreenCoordinate clipMinY,GraphicsScreenCoordinate clipMinX, GraphicsPrimitivePacket *packet);

/* 0x004FC540 */
void SoftwareRasterAux_Mode04 (GraphicsScreenCoordinate clipMaxY,GraphicsScreenCoordinate clipMaxX, GraphicsScreenCoordinate clipMinY,GraphicsScreenCoordinate clipMinX, GraphicsPrimitivePacket *packet);

/* 0x004FCD10 */
void SoftwareRasterAux_Mode08 (GraphicsScreenCoordinate clipMaxY,GraphicsScreenCoordinate clipMaxX, GraphicsScreenCoordinate clipMinY,GraphicsScreenCoordinate clipMinX, GraphicsPrimitivePacket *packet);

/* 0x004FD1E0 */
void SoftwareRasterAux_Mode14 (GraphicsScreenCoordinate clipMaxY,GraphicsScreenCoordinate clipMaxX, GraphicsScreenCoordinate clipMinY,GraphicsScreenCoordinate clipMinX, GraphicsPrimitivePacket *packet);

/* 0x004FD6F0 */
void SoftwareRasterAux_Mode09 (GraphicsScreenCoordinate clipMaxY,GraphicsScreenCoordinate clipMaxX, GraphicsScreenCoordinate clipMinY,GraphicsScreenCoordinate clipMinX, GraphicsPrimitivePacket *packet);

/* 0x004FDC00 */
void SoftwareRasterAux_Mode10 (GraphicsScreenCoordinate clipMaxY,GraphicsScreenCoordinate clipMaxX, GraphicsScreenCoordinate clipMinY,GraphicsScreenCoordinate clipMinX, GraphicsPrimitivePacket *packet);

/* 0x004FE110 */
void SoftwareRasterAux_Mode12 (GraphicsScreenCoordinate clipMaxY,GraphicsScreenCoordinate clipMaxX, GraphicsScreenCoordinate clipMinY,GraphicsScreenCoordinate clipMinX, GraphicsPrimitivePacket *packet);

/* 0x004FE620 */
void SoftwareRenderer_DisplayModeHook (DisplayModeHookArgument0 modeArg0,DisplayModeHookArgument1 modeArg1, FrontendDisplayDimensionPixels height,FrontendDisplayDimensionPixels width);

/* 0x004FE7D0 */
void __cdecl SoftwareRenderer_InstallDisplayModeHook(void);

/* 0x00518CE0 */
void SoftwareTexture_BilinearBlendScaleSubresources (GraphicsPixelDimension destinationHeight,GraphicsPixelDimension destinationWidth, GraphicsScreenCoordinate destinationTop,GraphicsScreenCoordinate destinationLeft, qword *blendedSourcePixels,qword *blendFactorPixels, GraphicsSubresourceIndex sourceSubresourceIndexA, GraphicsSubresourceIndex sourceSubresourceIndexB,int *graphicsTextureAsset, int *framebufferAccess);

/* 0x004D16D0 */
void __cdecl SoftwareRenderer_AdvanceDepthEpoch(void);

/* 0x00519210 */
void SoftwareMaskBuffer_Clear(SoftwareMaskRuntimeView *maskControl);

/* 0x00519270 */
void SoftwareMaskBuffer_AdvanceNonzeroPixelsSaturating31(SoftwareMaskRuntimeView *maskRuntime);

/* 0x00519500 */
void __fastcall SoftwareMaskBuffer_ApplyCircularRegionBit (undefined4 param_1,undefined4 param_2,UiBooleanState32 invertSelection, GraphicsScreenCoordinate centerY,GraphicsScreenCoordinate centerX, SoftwareMaskRadiusStep radiusStep,SoftwareMaskRuntimeView *maskRuntime);

/* 0x005195D0 */
void __fastcall SoftwareMaskBuffer_ApplyDiagonalHalfPlaneBit (undefined4 param_1,undefined4 param_2,UiBooleanState32 invertSelection, SoftwareMaskThresholdStep thresholdStep,SoftwareMaskRuntimeView *maskRuntime);

/* 0x00519670 */
void SoftwareMaskBuffer_SetAllPixelsBit(SoftwareMaskRuntimeView *maskControl);

/* 0x005196C0 */
void __fastcall SoftwareMaskBuffer_ApplyHorizontalBandBit (undefined4 param_1,undefined4 param_2,UiBooleanState32 reverseRows, TerrainGridMaskIndex bandIndex,SoftwareMaskRuntimeView *maskRuntime);

/* 0x004FE840 */
GraphicsPrimitivePacket * SoftwareRenderer_PrepareTrianglePacket(GraphicsPrimitivePacket *packet);

#endif /* THANDOR_GRAPHICS_BACKEND_SOFTWARE_H */

/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/include/thandor/graphics/backend/software.h
 * Reverse engineering by idkFoxes 2026
 */

#ifndef THANDOR_GRAPHICS_BACKEND_SOFTWARE_H
#define THANDOR_GRAPHICS_BACKEND_SOFTWARE_H

#include <thandor/generated/types.h>
#include <thandor/core/contracts.h>

/* Submodule: graphics/backend/software. */

/* g_SoftwareDepthEpoch drops by one step (the top byte) per frame (SoftwareRenderer_AdvanceDepthEpoch) */
#define SOFTWARE_DEPTH_EPOCH_STEP 0x1000000
/* SoftwareMaskBuffer: a revealed mask byte brightens by this much per tick, saturating at 0xFF */
#define SOFTWARE_MASK_BRIGHTEN_STEP 0x1f
/* Functions are grouped by semantic ownership. */

/* Builds the bilinear and alpha blend factor tables (the original carried them precomputed); once at startup. */
void SoftwareRenderer_BuildFactorTables(void);

void SoftwareMaskBuffer_AdvancePatternByPercentTick(SoftwareMaskRuntimeView *maskRuntime);

void SoftwareRenderer_ClearViewport(GraphicsScreenCoordinate clipMaxY,GraphicsScreenCoordinate clipMaxX,
          GraphicsScreenCoordinate clipMinY,GraphicsScreenCoordinate clipMinX);

void SoftwareRenderer_DrawQueue16Bit(GraphicsScreenCoordinate clipMaxY,GraphicsScreenCoordinate clipMaxX,
          GraphicsScreenCoordinate clipMinY,GraphicsScreenCoordinate clipMinX,
          GraphicsPrimitiveQueue *queue);

void SoftwareRenderer_DrawQueueNon16Bit(GraphicsScreenCoordinate clipMaxY,GraphicsScreenCoordinate clipMaxX,
          GraphicsScreenCoordinate clipMinY,GraphicsScreenCoordinate clipMinX,
          GraphicsPrimitiveQueue *queue);

void SoftwareRenderer_DrawQueueAuxiliary
          (GraphicsScreenCoordinate clipMaxY,GraphicsScreenCoordinate clipMaxX,void *targetBase,
          GraphicsPrimitiveQueue *queue);

void SoftwareRenderer_DrawPrimitiveQueueBridge(GraphicsScreenCoordinate clipMaxY,GraphicsScreenCoordinate clipMaxX,
          GraphicsScreenCoordinate clipMinY,GraphicsScreenCoordinate clipMinX,
          GraphicsPrimitiveQueue *queue);

void SoftwareGraphicsDispatch_SuccessNoOp(void);

void SoftwareGraphicsDispatch_NoOp(void);

bool SoftwarePixelFormat_BaseDisplayModeHook
          (uint32_t adapterIndex,uint32_t bitsPerPixel,FrontendDisplayDimensionPixels height,
          FrontendDisplayDimensionPixels width,uint32_t *errorCode);

SoftwareFramebufferAccess *SoftwareFramebuffer_Create
          (SoftwareFramebufferPixelSize bytesPerPixel,GraphicsPixelDimension height,
          GraphicsPixelDimension width,uint32_t *outError);

void SoftwareFramebuffer_Destroy(SoftwareFramebufferAccess *framebuffer);

void SoftwarePixelFormat_BuildChannelPackTables
          (SoftwareColorTransformQ16 colorScaleQ16,SoftwareColorTransformQ16 colorBiasQ16);

bool SoftwareTextureSource_BlitSourceAlpha16(GraphicsScreenCoordinate clipMaxY,GraphicsScreenCoordinate clipMaxX,
          GraphicsScreenCoordinate clipMinY,GraphicsScreenCoordinate clipMinX,
          GraphicsScreenCoordinate drawY,GraphicsScreenCoordinate drawX,
          GraphicsSubresourceIndex subresourceIndex,GraphicsTextureSourceAsset *sourceAsset,
          SoftwareFramebufferAccess *framebuffer);

bool SoftwareTextureSource_BlitSourceAlpha32(GraphicsScreenCoordinate clipMaxY,GraphicsScreenCoordinate clipMaxX,
          GraphicsScreenCoordinate clipMinY,GraphicsScreenCoordinate clipMinX,
          GraphicsScreenCoordinate drawY,GraphicsScreenCoordinate drawX,
          GraphicsSubresourceIndex subresourceIndex,GraphicsTextureSourceAsset *sourceAsset,
          SoftwareFramebufferAccess *framebuffer);

bool SoftwareTextureSource_BlitHalfSourceRgb16(GraphicsScreenCoordinate clipMaxY,GraphicsScreenCoordinate clipMaxX,
          GraphicsScreenCoordinate clipMinY,GraphicsScreenCoordinate clipMinX,
          GraphicsScreenCoordinate drawY,GraphicsScreenCoordinate drawX,
          GraphicsSubresourceIndex subresourceIndex,GraphicsTextureSourceAsset *sourceAsset,
          SoftwareFramebufferAccess *framebuffer);

bool SoftwareTextureSource_BlitHalfSourceRgb32(GraphicsScreenCoordinate clipMaxY,GraphicsScreenCoordinate clipMaxX,
          GraphicsScreenCoordinate clipMinY,GraphicsScreenCoordinate clipMinX,
          GraphicsScreenCoordinate drawY,GraphicsScreenCoordinate drawX,
          GraphicsSubresourceIndex subresourceIndex,GraphicsTextureSourceAsset *sourceAsset,
          SoftwareFramebufferAccess *framebuffer);

void SoftwareTextureSource_StretchDirectColorBilinear16
          (GraphicsPixelDimension destinationHeight,GraphicsPixelDimension destinationWidth,
          GraphicsScreenCoordinate destinationY,GraphicsScreenCoordinate destinationX,
          GraphicsSubresourceIndex subresourceIndex,GraphicsTextureSourceAsset *sourceAsset,
          SoftwareFramebufferAccess *framebuffer);

void SoftwareTextureSource_StretchDirectColorBilinear32
          (GraphicsPixelDimension destinationHeight,GraphicsPixelDimension destinationWidth,
          GraphicsScreenCoordinate destinationY,GraphicsScreenCoordinate destinationX,
          GraphicsSubresourceIndex subresourceIndex,GraphicsTextureSourceAsset *sourceAsset,
          SoftwareFramebufferAccess *framebuffer);

void SoftwareTextureSource_BlitIntegerScaledSourceAlpha16
          (GraphicsScreenCoordinate clipMaxY,GraphicsScreenCoordinate clipMaxX,
          GraphicsScreenCoordinate clipMinY,GraphicsScreenCoordinate clipMinX,
          GraphicsScreenCoordinate drawY,GraphicsScreenCoordinate drawX,
          GraphicsIntegerScale integerScale,GraphicsSubresourceIndex subresourceIndex,
          GraphicsTextureSourceAsset *sourceAsset,SoftwareFramebufferAccess *framebuffer);

void SoftwareTextureSource_BlitIntegerScaledSourceAlpha32
          (GraphicsScreenCoordinate clipMaxY,GraphicsScreenCoordinate clipMaxX,
          GraphicsScreenCoordinate clipMinY,GraphicsScreenCoordinate clipMinX,
          GraphicsScreenCoordinate drawY,GraphicsScreenCoordinate drawX,
          GraphicsIntegerScale integerScale,GraphicsSubresourceIndex subresourceIndex,
          GraphicsTextureSourceAsset *sourceAsset,SoftwareFramebufferAccess *framebuffer);

void SoftwareTextureSource_BlitSourceAlphaPaletteBank16
          (GraphicsScreenCoordinate clipMaxY,GraphicsScreenCoordinate clipMaxX,
          GraphicsScreenCoordinate clipMinY,GraphicsScreenCoordinate clipMinX,
          GraphicsScreenCoordinate drawY,GraphicsScreenCoordinate drawX,
          PaletteBankIndex paletteBankIndex,GraphicsSubresourceIndex subresourceIndex,
          GraphicsTextureSourceAsset *sourceAsset,SoftwareFramebufferAccess *framebuffer);

void SoftwareTextureSource_BlitSourceAlphaPaletteBank32
          (GraphicsScreenCoordinate clipMaxY,GraphicsScreenCoordinate clipMaxX,
          GraphicsScreenCoordinate clipMinY,GraphicsScreenCoordinate clipMinX,
          GraphicsScreenCoordinate drawY,GraphicsScreenCoordinate drawX,
          PaletteBankIndex paletteBankIndex,GraphicsSubresourceIndex subresourceIndex,
          GraphicsTextureSourceAsset *sourceAsset,SoftwareFramebufferAccess *framebuffer);

bool SoftwareTextureSource_BlitSaturatedAddRgb16(GraphicsScreenCoordinate clipMaxY,GraphicsScreenCoordinate clipMaxX,
          GraphicsScreenCoordinate clipMinY,GraphicsScreenCoordinate clipMinX,
          GraphicsScreenCoordinate drawY,GraphicsScreenCoordinate drawX,
          GraphicsSubresourceIndex subresourceIndex,GraphicsTextureSourceAsset *sourceAsset,
          SoftwareFramebufferAccess *framebuffer);

bool SoftwareTextureSource_BlitSaturatedAddRgb32(GraphicsScreenCoordinate clipMaxY,GraphicsScreenCoordinate clipMaxX,
          GraphicsScreenCoordinate clipMinY,GraphicsScreenCoordinate clipMinX,
          GraphicsScreenCoordinate drawY,GraphicsScreenCoordinate drawX,
          GraphicsSubresourceIndex subresourceIndex,GraphicsTextureSourceAsset *sourceAsset,
          SoftwareFramebufferAccess *framebuffer);

bool SoftwareTextureSource_BlitHalfRgbSaturatedAdd16
          (GraphicsScreenCoordinate clipMaxY,GraphicsScreenCoordinate clipMaxX,
          GraphicsScreenCoordinate clipMinY,GraphicsScreenCoordinate clipMinX,
          GraphicsScreenCoordinate drawY,GraphicsScreenCoordinate drawX,
          GraphicsSubresourceIndex subresourceIndex,GraphicsTextureSourceAsset *sourceAsset,
          SoftwareFramebufferAccess *framebuffer);

bool SoftwareTextureSource_BlitHalfRgbSaturatedAdd32
          (GraphicsScreenCoordinate clipMaxY,GraphicsScreenCoordinate clipMaxX,
          GraphicsScreenCoordinate clipMinY,GraphicsScreenCoordinate clipMinX,
          GraphicsScreenCoordinate drawY,GraphicsScreenCoordinate drawX,
          GraphicsSubresourceIndex subresourceIndex,GraphicsTextureSourceAsset *sourceAsset,
          SoftwareFramebufferAccess *framebuffer);

bool SoftwareTextureSource_BlitModulatedSourceAlpha16
          (GraphicsScreenCoordinate clipMaxY,GraphicsScreenCoordinate clipMaxX,
          GraphicsScreenCoordinate clipMinY,GraphicsScreenCoordinate clipMinX,
          GraphicsScreenCoordinate drawY,GraphicsScreenCoordinate drawX,
          PackedArgb32 modulationArgb8888,GraphicsSubresourceIndex subresourceIndex,
          GraphicsTextureSourceAsset *sourceAsset,SoftwareFramebufferAccess *framebuffer);

bool SoftwareTextureSource_BlitModulatedSourceAlpha32
          (GraphicsScreenCoordinate clipMaxY,GraphicsScreenCoordinate clipMaxX,
          GraphicsScreenCoordinate clipMinY,GraphicsScreenCoordinate clipMinX,
          GraphicsScreenCoordinate drawY,GraphicsScreenCoordinate drawX,
          PackedArgb32 modulationArgb8888,GraphicsSubresourceIndex subresourceIndex,
          GraphicsTextureSourceAsset *sourceAsset,SoftwareFramebufferAccess *framebuffer);

void SoftwareFramebuffer_FillRectArgb16(GraphicsScreenCoordinate clipMaxY,GraphicsScreenCoordinate clipMaxX,
          GraphicsScreenCoordinate clipMinY,GraphicsScreenCoordinate clipMinX,
          GraphicsScreenCoordinate rectMaxY,GraphicsScreenCoordinate rectMaxX,
          GraphicsScreenCoordinate rectMinY,GraphicsScreenCoordinate rectMinX,PackedArgb32 argb8888,
          SoftwareFramebufferAccess *framebuffer);

void SoftwareFramebuffer_FillRectArgb32(GraphicsScreenCoordinate clipMaxY,GraphicsScreenCoordinate clipMaxX,
          GraphicsScreenCoordinate clipMinY,GraphicsScreenCoordinate clipMinX,
          GraphicsScreenCoordinate rectMaxY,GraphicsScreenCoordinate rectMaxX,
          GraphicsScreenCoordinate rectMinY,GraphicsScreenCoordinate rectMinX,PackedArgb32 argb8888,
          SoftwareFramebufferAccess *framebuffer);

void SoftwareFramebuffer_CopyRegionToOrigin(GraphicsPixelDimension copyHeight,GraphicsPixelDimension copyWidth,
          GraphicsScreenCoordinate sourceY,GraphicsScreenCoordinate sourceX,
          SoftwareFramebufferAccess *destination,SoftwareFramebufferAccess *source);

void SoftwareFramebuffer_CopyOriginToRegion(GraphicsPixelDimension copyHeight,GraphicsPixelDimension copyWidth,
          GraphicsScreenCoordinate destinationY,GraphicsScreenCoordinate destinationX,
          SoftwareFramebufferAccess *source,SoftwareFramebufferAccess *destination);

void SoftwareRaster16_Mode16 (GraphicsScreenCoordinate clipMaxY,GraphicsScreenCoordinate clipMaxX, GraphicsScreenCoordinate clipMinY,GraphicsScreenCoordinate clipMinX, GraphicsPrimitivePacket *packet);

void SoftwareRaster16_Mode22 (GraphicsScreenCoordinate clipMaxY,GraphicsScreenCoordinate clipMaxX, GraphicsScreenCoordinate clipMinY,GraphicsScreenCoordinate clipMinX, GraphicsPrimitivePacket *packet);

void SoftwareRaster16_Mode17 (GraphicsScreenCoordinate clipMaxY,GraphicsScreenCoordinate clipMaxX, GraphicsScreenCoordinate clipMinY,GraphicsScreenCoordinate clipMinX, GraphicsPrimitivePacket *packet);

void SoftwareRaster16_Mode18 (GraphicsScreenCoordinate clipMaxY,GraphicsScreenCoordinate clipMaxX, GraphicsScreenCoordinate clipMinY,GraphicsScreenCoordinate clipMinX, GraphicsPrimitivePacket *packet);

void SoftwareRaster16_Mode20 (GraphicsScreenCoordinate clipMaxY,GraphicsScreenCoordinate clipMaxX, GraphicsScreenCoordinate clipMinY,GraphicsScreenCoordinate clipMinX, GraphicsPrimitivePacket *packet);

void SoftwareRaster16_Mode24 (GraphicsScreenCoordinate clipMaxY,GraphicsScreenCoordinate clipMaxX, GraphicsScreenCoordinate clipMinY,GraphicsScreenCoordinate clipMinX, GraphicsPrimitivePacket *packet);

void SoftwareRaster16_Mode30 (GraphicsScreenCoordinate clipMaxY,GraphicsScreenCoordinate clipMaxX, GraphicsScreenCoordinate clipMinY,GraphicsScreenCoordinate clipMinX, GraphicsPrimitivePacket *packet);

void SoftwareRaster16_Mode25 (GraphicsScreenCoordinate clipMaxY,GraphicsScreenCoordinate clipMaxX, GraphicsScreenCoordinate clipMinY,GraphicsScreenCoordinate clipMinX, GraphicsPrimitivePacket *packet);

void SoftwareRaster16_Mode26 (GraphicsScreenCoordinate clipMaxY,GraphicsScreenCoordinate clipMaxX, GraphicsScreenCoordinate clipMinY,GraphicsScreenCoordinate clipMinX, GraphicsPrimitivePacket *packet);

void SoftwareRaster16_Mode28 (GraphicsScreenCoordinate clipMaxY,GraphicsScreenCoordinate clipMaxX, GraphicsScreenCoordinate clipMinY,GraphicsScreenCoordinate clipMinX, GraphicsPrimitivePacket *packet);

void SoftwareRaster16_Mode00 (GraphicsScreenCoordinate clipMaxY,GraphicsScreenCoordinate clipMaxX, GraphicsScreenCoordinate clipMinY,GraphicsScreenCoordinate clipMinX, GraphicsPrimitivePacket *packet);

void SoftwareRaster16_Mode06 (GraphicsScreenCoordinate clipMaxY,GraphicsScreenCoordinate clipMaxX, GraphicsScreenCoordinate clipMinY,GraphicsScreenCoordinate clipMinX, GraphicsPrimitivePacket *packet);

void SoftwareRaster16_Mode01 (GraphicsScreenCoordinate clipMaxY,GraphicsScreenCoordinate clipMaxX, GraphicsScreenCoordinate clipMinY,GraphicsScreenCoordinate clipMinX, GraphicsPrimitivePacket *packet);

void SoftwareRaster16_Mode02 (GraphicsScreenCoordinate clipMaxY,GraphicsScreenCoordinate clipMaxX, GraphicsScreenCoordinate clipMinY,GraphicsScreenCoordinate clipMinX, GraphicsPrimitivePacket *packet);

void SoftwareRaster16_Mode04 (GraphicsScreenCoordinate clipMaxY,GraphicsScreenCoordinate clipMaxX, GraphicsScreenCoordinate clipMinY,GraphicsScreenCoordinate clipMinX, GraphicsPrimitivePacket *packet);

void SoftwareRaster16_Mode08 (GraphicsScreenCoordinate clipMaxY,GraphicsScreenCoordinate clipMaxX, GraphicsScreenCoordinate clipMinY,GraphicsScreenCoordinate clipMinX, GraphicsPrimitivePacket *packet);

void SoftwareRaster16_Mode14 (GraphicsScreenCoordinate clipMaxY,GraphicsScreenCoordinate clipMaxX, GraphicsScreenCoordinate clipMinY,GraphicsScreenCoordinate clipMinX, GraphicsPrimitivePacket *packet);

void SoftwareRaster16_Mode09 (GraphicsScreenCoordinate clipMaxY,GraphicsScreenCoordinate clipMaxX, GraphicsScreenCoordinate clipMinY,GraphicsScreenCoordinate clipMinX, GraphicsPrimitivePacket *packet);

void SoftwareRaster16_Mode10 (GraphicsScreenCoordinate clipMaxY,GraphicsScreenCoordinate clipMaxX, GraphicsScreenCoordinate clipMinY,GraphicsScreenCoordinate clipMinX, GraphicsPrimitivePacket *packet);

void SoftwareRaster16_Mode12 (GraphicsScreenCoordinate clipMaxY,GraphicsScreenCoordinate clipMaxX, GraphicsScreenCoordinate clipMinY,GraphicsScreenCoordinate clipMinX, GraphicsPrimitivePacket *packet);

void SoftwareRasterNon16_Mode16 (GraphicsScreenCoordinate clipMaxY,GraphicsScreenCoordinate clipMaxX, GraphicsScreenCoordinate clipMinY,GraphicsScreenCoordinate clipMinX, GraphicsPrimitivePacket *packet);

void SoftwareRasterNon16_Mode22 (GraphicsScreenCoordinate clipMaxY,GraphicsScreenCoordinate clipMaxX, GraphicsScreenCoordinate clipMinY,GraphicsScreenCoordinate clipMinX, GraphicsPrimitivePacket *packet);

void SoftwareRasterNon16_Mode17 (GraphicsScreenCoordinate clipMaxY,GraphicsScreenCoordinate clipMaxX, GraphicsScreenCoordinate clipMinY,GraphicsScreenCoordinate clipMinX, GraphicsPrimitivePacket *packet);

void SoftwareRasterNon16_Mode18 (GraphicsScreenCoordinate clipMaxY,GraphicsScreenCoordinate clipMaxX, GraphicsScreenCoordinate clipMinY,GraphicsScreenCoordinate clipMinX, GraphicsPrimitivePacket *packet);

void SoftwareRasterNon16_Mode20 (GraphicsScreenCoordinate clipMaxY,GraphicsScreenCoordinate clipMaxX, GraphicsScreenCoordinate clipMinY,GraphicsScreenCoordinate clipMinX, GraphicsPrimitivePacket *packet);

void SoftwareRasterNon16_Mode24 (GraphicsScreenCoordinate clipMaxY,GraphicsScreenCoordinate clipMaxX, GraphicsScreenCoordinate clipMinY,GraphicsScreenCoordinate clipMinX, GraphicsPrimitivePacket *packet);

void SoftwareRasterNon16_Mode30 (GraphicsScreenCoordinate clipMaxY,GraphicsScreenCoordinate clipMaxX, GraphicsScreenCoordinate clipMinY,GraphicsScreenCoordinate clipMinX, GraphicsPrimitivePacket *packet);

void SoftwareRasterNon16_Mode25 (GraphicsScreenCoordinate clipMaxY,GraphicsScreenCoordinate clipMaxX, GraphicsScreenCoordinate clipMinY,GraphicsScreenCoordinate clipMinX, GraphicsPrimitivePacket *packet);

void SoftwareRasterNon16_Mode26 (GraphicsScreenCoordinate clipMaxY,GraphicsScreenCoordinate clipMaxX, GraphicsScreenCoordinate clipMinY,GraphicsScreenCoordinate clipMinX, GraphicsPrimitivePacket *packet);

void SoftwareRasterNon16_Mode28 (GraphicsScreenCoordinate clipMaxY,GraphicsScreenCoordinate clipMaxX, GraphicsScreenCoordinate clipMinY,GraphicsScreenCoordinate clipMinX, GraphicsPrimitivePacket *packet);

void SoftwareRasterNon16_Mode00 (GraphicsScreenCoordinate clipMaxY,GraphicsScreenCoordinate clipMaxX, GraphicsScreenCoordinate clipMinY,GraphicsScreenCoordinate clipMinX, GraphicsPrimitivePacket *packet);

void SoftwareRasterNon16_Mode06 (GraphicsScreenCoordinate clipMaxY,GraphicsScreenCoordinate clipMaxX, GraphicsScreenCoordinate clipMinY,GraphicsScreenCoordinate clipMinX, GraphicsPrimitivePacket *packet);

void SoftwareRasterNon16_Mode01 (GraphicsScreenCoordinate clipMaxY,GraphicsScreenCoordinate clipMaxX, GraphicsScreenCoordinate clipMinY,GraphicsScreenCoordinate clipMinX, GraphicsPrimitivePacket *packet);

void SoftwareRasterNon16_Mode02 (GraphicsScreenCoordinate clipMaxY,GraphicsScreenCoordinate clipMaxX, GraphicsScreenCoordinate clipMinY,GraphicsScreenCoordinate clipMinX, GraphicsPrimitivePacket *packet);

void SoftwareRasterNon16_Mode04 (GraphicsScreenCoordinate clipMaxY,GraphicsScreenCoordinate clipMaxX, GraphicsScreenCoordinate clipMinY,GraphicsScreenCoordinate clipMinX, GraphicsPrimitivePacket *packet);

void SoftwareRasterNon16_Mode08 (GraphicsScreenCoordinate clipMaxY,GraphicsScreenCoordinate clipMaxX, GraphicsScreenCoordinate clipMinY,GraphicsScreenCoordinate clipMinX, GraphicsPrimitivePacket *packet);

void SoftwareRasterNon16_Mode14 (GraphicsScreenCoordinate clipMaxY,GraphicsScreenCoordinate clipMaxX, GraphicsScreenCoordinate clipMinY,GraphicsScreenCoordinate clipMinX, GraphicsPrimitivePacket *packet);

void SoftwareRasterNon16_Mode09 (GraphicsScreenCoordinate clipMaxY,GraphicsScreenCoordinate clipMaxX, GraphicsScreenCoordinate clipMinY,GraphicsScreenCoordinate clipMinX, GraphicsPrimitivePacket *packet);

void SoftwareRasterNon16_Mode10 (GraphicsScreenCoordinate clipMaxY,GraphicsScreenCoordinate clipMaxX, GraphicsScreenCoordinate clipMinY,GraphicsScreenCoordinate clipMinX, GraphicsPrimitivePacket *packet);

void SoftwareRasterNon16_Mode12 (GraphicsScreenCoordinate clipMaxY,GraphicsScreenCoordinate clipMaxX, GraphicsScreenCoordinate clipMinY,GraphicsScreenCoordinate clipMinX, GraphicsPrimitivePacket *packet);

void SoftwareRasterAux_Mode16 (GraphicsScreenCoordinate clipMaxY,GraphicsScreenCoordinate clipMaxX, GraphicsScreenCoordinate clipMinY,GraphicsScreenCoordinate clipMinX, GraphicsPrimitivePacket *packet);

void SoftwareRasterAux_Mode22 (GraphicsScreenCoordinate clipMaxY,GraphicsScreenCoordinate clipMaxX, GraphicsScreenCoordinate clipMinY,GraphicsScreenCoordinate clipMinX, GraphicsPrimitivePacket *packet);

void SoftwareRasterAux_Mode17 (GraphicsScreenCoordinate clipMaxY,GraphicsScreenCoordinate clipMaxX, GraphicsScreenCoordinate clipMinY,GraphicsScreenCoordinate clipMinX, GraphicsPrimitivePacket *packet);

void SoftwareRasterAux_Mode18 (GraphicsScreenCoordinate clipMaxY,GraphicsScreenCoordinate clipMaxX, GraphicsScreenCoordinate clipMinY,GraphicsScreenCoordinate clipMinX, GraphicsPrimitivePacket *packet);

void SoftwareRasterAux_Mode20 (GraphicsScreenCoordinate clipMaxY,GraphicsScreenCoordinate clipMaxX, GraphicsScreenCoordinate clipMinY,GraphicsScreenCoordinate clipMinX, GraphicsPrimitivePacket *packet);

void SoftwareRasterAux_Mode24 (GraphicsScreenCoordinate clipMaxY,GraphicsScreenCoordinate clipMaxX, GraphicsScreenCoordinate clipMinY,GraphicsScreenCoordinate clipMinX, GraphicsPrimitivePacket *packet);

void SoftwareRasterAux_Mode30 (GraphicsScreenCoordinate clipMaxY,GraphicsScreenCoordinate clipMaxX, GraphicsScreenCoordinate clipMinY,GraphicsScreenCoordinate clipMinX, GraphicsPrimitivePacket *packet);

void SoftwareRasterAux_Mode25 (GraphicsScreenCoordinate clipMaxY,GraphicsScreenCoordinate clipMaxX, GraphicsScreenCoordinate clipMinY,GraphicsScreenCoordinate clipMinX, GraphicsPrimitivePacket *packet);

void SoftwareRasterAux_Mode26 (GraphicsScreenCoordinate clipMaxY,GraphicsScreenCoordinate clipMaxX, GraphicsScreenCoordinate clipMinY,GraphicsScreenCoordinate clipMinX, GraphicsPrimitivePacket *packet);

void SoftwareRasterAux_Mode28 (GraphicsScreenCoordinate clipMaxY,GraphicsScreenCoordinate clipMaxX, GraphicsScreenCoordinate clipMinY,GraphicsScreenCoordinate clipMinX, GraphicsPrimitivePacket *packet);

void SoftwareRasterAux_Mode00 (GraphicsScreenCoordinate clipMaxY,GraphicsScreenCoordinate clipMaxX, GraphicsScreenCoordinate clipMinY,GraphicsScreenCoordinate clipMinX, GraphicsPrimitivePacket *packet);

void SoftwareRasterAux_Mode06 (GraphicsScreenCoordinate clipMaxY,GraphicsScreenCoordinate clipMaxX, GraphicsScreenCoordinate clipMinY,GraphicsScreenCoordinate clipMinX, GraphicsPrimitivePacket *packet);

void SoftwareRasterAux_Mode01 (GraphicsScreenCoordinate clipMaxY,GraphicsScreenCoordinate clipMaxX, GraphicsScreenCoordinate clipMinY,GraphicsScreenCoordinate clipMinX, GraphicsPrimitivePacket *packet);

void SoftwareRasterAux_Mode02 (GraphicsScreenCoordinate clipMaxY,GraphicsScreenCoordinate clipMaxX, GraphicsScreenCoordinate clipMinY,GraphicsScreenCoordinate clipMinX, GraphicsPrimitivePacket *packet);

void SoftwareRasterAux_Mode04 (GraphicsScreenCoordinate clipMaxY,GraphicsScreenCoordinate clipMaxX, GraphicsScreenCoordinate clipMinY,GraphicsScreenCoordinate clipMinX, GraphicsPrimitivePacket *packet);

void SoftwareRasterAux_Mode08 (GraphicsScreenCoordinate clipMaxY,GraphicsScreenCoordinate clipMaxX, GraphicsScreenCoordinate clipMinY,GraphicsScreenCoordinate clipMinX, GraphicsPrimitivePacket *packet);

void SoftwareRasterAux_Mode14 (GraphicsScreenCoordinate clipMaxY,GraphicsScreenCoordinate clipMaxX, GraphicsScreenCoordinate clipMinY,GraphicsScreenCoordinate clipMinX, GraphicsPrimitivePacket *packet);

void SoftwareRasterAux_Mode09 (GraphicsScreenCoordinate clipMaxY,GraphicsScreenCoordinate clipMaxX, GraphicsScreenCoordinate clipMinY,GraphicsScreenCoordinate clipMinX, GraphicsPrimitivePacket *packet);

void SoftwareRasterAux_Mode10 (GraphicsScreenCoordinate clipMaxY,GraphicsScreenCoordinate clipMaxX, GraphicsScreenCoordinate clipMinY,GraphicsScreenCoordinate clipMinX, GraphicsPrimitivePacket *packet);

void SoftwareRasterAux_Mode12 (GraphicsScreenCoordinate clipMaxY,GraphicsScreenCoordinate clipMaxX, GraphicsScreenCoordinate clipMinY,GraphicsScreenCoordinate clipMinX, GraphicsPrimitivePacket *packet);

bool SoftwareRenderer_SetDisplayMode
          (DisplayModeHookArgument0 adapterIndex,DisplayModeHookArgument1 bitsPerPixel,
          FrontendDisplayDimensionPixels height,FrontendDisplayDimensionPixels width,uint32_t *errorCode);

uint32_t __cdecl SoftwareRenderer_InstallDisplayModeHook(void);

void SoftwareTexture_BilinearBlendScaleSubresources
          (GraphicsPixelDimension destinationHeight,GraphicsPixelDimension destinationWidth,
          GraphicsScreenCoordinate destinationTop,GraphicsScreenCoordinate destinationLeft,
          uint64_t *blendedSourcePixels,uint64_t *blendFactorPixels,
          GraphicsSubresourceIndex sourceSubresourceIndexA,
          GraphicsSubresourceIndex sourceSubresourceIndexB,int *graphicsTextureAsset,
          int *framebufferAccess);

void SoftwareRenderer_AdvanceDepthEpoch(void);

void SoftwareMaskBuffer_Clear(SoftwareMaskRuntimeView *maskControl);

void SoftwareMaskBuffer_AdvanceNonzeroPixelsSaturating31(SoftwareMaskRuntimeView *maskRuntime);

void SoftwareMaskBuffer_ApplyCircularRegionBit(UiBooleanState32 invertSelection,GraphicsScreenCoordinate centerY,
          GraphicsScreenCoordinate centerX,SoftwareMaskRadiusStep radiusStep,
          SoftwareMaskRuntimeView *maskRuntime);

void SoftwareMaskBuffer_ApplyDiagonalHalfPlaneBit
          (UiBooleanState32 invertSelection,SoftwareMaskThresholdStep thresholdStep,
          SoftwareMaskRuntimeView *maskRuntime);

void SoftwareMaskBuffer_SetAllPixelsBit(SoftwareMaskRuntimeView *maskControl);

void SoftwareMaskBuffer_ApplyHorizontalBandBit(UiBooleanState32 reverseRows,TerrainGridMaskIndex bandIndex,
          SoftwareMaskRuntimeView *maskRuntime);

void SoftwareRenderer_PrepareTrianglePacket(GraphicsPrimitivePacket *packet);

extern GraphicsSetViewportProc *g_GraphicsSetViewportAndClearDepth;
extern GraphicsDrawPrimitiveQueueProc *g_GraphicsDrawPrimitiveQueue;
extern GraphicsBeginSceneProc *g_GraphicsBeginScene;

extern SoftwarePixelMmxConstants g_SoftwarePixelMmxConstants;
extern SoftwareBgraWordLanes g_SoftwareBilinearForwardFactors[257];
extern SoftwareBgraWordLanes g_SoftwareBilinearInverseFactors[257];
extern SoftwareRgbWordLanes g_SoftwareBlendAlphaFactors[256];
extern SoftwareRgbWordLanes g_SoftwareBlendInverseAlphaFactors[256];
extern GraphicsEndSceneProc *g_GraphicsEndScene;
extern GraphicsDiagnosticCounter g_PrimitiveDrawCallCount;
extern GraphicsDiagnosticCounter g_TextureBindStateChangeCount;
extern GraphicsDiagnosticCounter g_TextureDeviceReloadCount;
extern SoftwarePixelPackTables *g_SoftwarePixelPackTables;
extern int32_t g_SoftwareColorScaleQ16;
extern int32_t g_SoftwareColorBiasQ16;
extern SoftwarePixelFormatConfig g_SoftwarePixelFormatConfig;
extern SoftwareDisplayModeHookProc *g_GraphicsSetDisplayMode;
extern SoftwareBuildPixelPackTablesProc *g_SoftwareBuildPixelPackTables;
extern uint32_t g_SoftwareDepthRowStrideBytes;
extern void *g_SoftwareAuxiliaryTargetBase;
extern int32_t g_SoftwareDepthEpoch;

extern SoftwareRasterHandler *g_SoftwareRasterHandlers16Bit[64];
extern SoftwareRasterHandler *g_SoftwareRasterHandlersNon16Bit[64];
extern SoftwareRasterHandler *g_SoftwareRasterHandlersAuxiliary[64];

extern int32_t *g_SoftwareDepthBuffer;

extern SoftwareRasterScanState g_SoftwareRasterScanState;

extern SoftwareFramebufferCreateProc *g_SoftwareFramebufferCreate;

#endif /* THANDOR_GRAPHICS_BACKEND_SOFTWARE_H */

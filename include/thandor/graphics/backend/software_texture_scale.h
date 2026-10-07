/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/include/thandor/graphics/backend/software_texture_scale.h
 * Reverse engineering by idkFoxes 2026
 */

#ifndef THANDOR_GRAPHICS_BACKEND_SOFTWARE_TEXTURE_SCALE_H
#define THANDOR_GRAPHICS_BACKEND_SOFTWARE_TEXTURE_SCALE_H

#include <thandor/graphics/backend/types.h>
#include <thandor/graphics/render/types.h>
#include <thandor/graphics/resources/types.h>
#include <thandor/ui/controls/types.h>
#include <thandor/core/contracts.h>

void SoftwareTexture_BilinearBlendScaleSubresources
          (GraphicsPixelDimension destinationHeight,GraphicsPixelDimension destinationWidth,
          GraphicsScreenCoordinate destinationTop,GraphicsScreenCoordinate destinationLeft,
          uint64_t *blendedSourcePixels,uint64_t *blendFactorPixels,
          GraphicsSubresourceIndex sourceSubresourceIndexA,
          GraphicsSubresourceIndex sourceSubresourceIndexB,const GraphicsTextureSourceAsset *graphicsTextureAsset,
          const SoftwareFramebufferAccess *framebufferAccess);

/* Step 1 of SoftwareTexture_BilinearBlendScaleSubresources alone (the cross-fade into blendedSourcePixels; the GPU
   draw list scales the result on the GPU). The caller checks the asset and that both entries are paletted. */
void SoftwareTexture_CrossFadeSubresources
          (uint64_t *blendedSourcePixels,uint64_t *blendFactorPixels,
          GraphicsSubresourceIndex sourceSubresourceIndexA,GraphicsSubresourceIndex sourceSubresourceIndexB,
          const GraphicsTextureSourceAsset *asset);

/* Not in the original: builds the minimap scaler weight tables (called once at startup). */
void SoftwareMinimap_BuildPixelWeightTables();

/* Software implementation of g_GraphicsMinimapDraw (graphics/core/draw2d.h). */
void SoftwareTexture_DrawMinimapBilinear32
          (int32_t destY,int32_t destX,int32_t height,int32_t width,uint32_t startU,uint32_t startV,
          uint32_t pixelStepU,uint32_t pixelStepV,uint32_t rowStepU,uint32_t rowStepV,
          GraphicsTextureSourceAsset *texture,SoftwareFramebufferAccess *framebuffer);

#endif /* THANDOR_GRAPHICS_BACKEND_SOFTWARE_TEXTURE_SCALE_H */

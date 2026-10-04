/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/include/thandor/graphics/backend/software_texture_scale.h
 * Reverse engineering by idkFoxes 2026
 */

#ifndef THANDOR_GRAPHICS_BACKEND_SOFTWARE_TEXTURE_SCALE_H
#define THANDOR_GRAPHICS_BACKEND_SOFTWARE_TEXTURE_SCALE_H

#include <thandor/graphics/render/types.h>
#include <thandor/ui/controls/types.h>
#include <thandor/core/contracts.h>

void SoftwareTexture_BilinearBlendScaleSubresources
          (GraphicsPixelDimension destinationHeight,GraphicsPixelDimension destinationWidth,
          GraphicsScreenCoordinate destinationTop,GraphicsScreenCoordinate destinationLeft,
          uint64_t *blendedSourcePixels,uint64_t *blendFactorPixels,
          GraphicsSubresourceIndex sourceSubresourceIndexA,
          GraphicsSubresourceIndex sourceSubresourceIndexB,int *graphicsTextureAsset,
          int *framebufferAccess);

#endif /* THANDOR_GRAPHICS_BACKEND_SOFTWARE_TEXTURE_SCALE_H */

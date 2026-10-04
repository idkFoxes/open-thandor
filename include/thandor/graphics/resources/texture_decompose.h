/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/include/thandor/graphics/resources/texture_decompose.h
 * Reverse engineering by idkFoxes 2026
 */

#ifndef THANDOR_GRAPHICS_RESOURCES_TEXTURE_DECOMPOSE_H
#define THANDOR_GRAPHICS_RESOURCES_TEXTURE_DECOMPOSE_H

#include <thandor/generated/types.h>
#include <thandor/core/contracts.h>

Bool8 GraphicsTextureSource_DecomposeSubresourceRegions
          (GraphicsSubresourceIndex entryIndex,GraphicsTextureSourceAsset *sourceAsset,
          GraphicsTextureSourceAsset **outAsset,uint32_t *outError);

#endif /* THANDOR_GRAPHICS_RESOURCES_TEXTURE_DECOMPOSE_H */

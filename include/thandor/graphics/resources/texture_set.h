/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/include/thandor/graphics/resources/texture_set.h
 * Reverse engineering by idkFoxes 2026
 */

#ifndef THANDOR_GRAPHICS_RESOURCES_TEXTURE_SET_H
#define THANDOR_GRAPHICS_RESOURCES_TEXTURE_SET_H

#include <thandor/generated/types.h>
#include <thandor/core/contracts.h>

/* Number of entries in g_GraphicsTextureSlots, the registry of live texture resources
   (GraphicsTexture_RegisterSlot, GraphicsTextureSet_Destroy). */
#define GRAPHICS_TEXTURE_SLOT_CAPACITY 4096

extern GraphicsTextureSetLoadPackageProc *g_GraphicsTextureSetLoadPackage;

extern GraphicsTextureSetReleasePackageProc *g_GraphicsTextureSetReleasePackage;

extern GraphicsTextureSetRefreshProc *g_GraphicsRefreshTextureAlpha;

extern GraphicsTextureResource **g_GraphicsTextureSlots;

extern GraphicsTextureSetCreateProc *g_GraphicsCreateTextureSet;

extern GraphicsTextureSetDestroyProc *g_GraphicsDestroyTextureSet;

extern GraphicsTextureRebuildAllProc *g_GraphicsRebuildAllStagingTextures;

GraphicsTextureSet * GraphicsTextureSet_Create(GraphicsTextureSourceAsset *sourceAsset,uint32_t *outErrorCode);

GraphicsTextureSourceAsset * GraphicsTextureSet_Destroy(GraphicsTextureSet *set);

GraphicsTextureSet * GraphicsTextureSet_LoadPackage(uint16_t *pathUtf16,uint32_t *outErrorCode);

void GraphicsTextureSet_ReleasePackage(GraphicsTextureSet *set);

void GraphicsTextureSet_RefreshNoOp(GraphicsSubresourceIndex subresourceIndex,GraphicsTextureSet *set);

void __cdecl GraphicsTexture_RebuildNoOp(void);

GraphicsTextureSet * GraphicsTextureSet_AllocateMetadata(GraphicsTextureSourceAsset *sourceAsset,uint32_t *outErrorCode);

GraphicsTextureSourceAsset * GraphicsTextureSet_FreeMetadata(GraphicsTextureSet *set);

Bool8 GraphicsTexture_RegisterSlot(GraphicsTextureResource *texture);

#endif /* THANDOR_GRAPHICS_RESOURCES_TEXTURE_SET_H */

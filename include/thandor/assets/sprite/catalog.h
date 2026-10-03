/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/include/thandor/assets/sprite/catalog.h
 * Reverse engineering by idkFoxes 2026
 */

#ifndef THANDOR_ASSETS_SPRITE_CATALOG_H
#define THANDOR_ASSETS_SPRITE_CATALOG_H

#include <thandor/generated/types.h>
#include <thandor/core/contracts.h>

/* Submodule: assets/sprite/catalog. */
/* Functions are grouped by semantic ownership. */

void SpriteAssetRegistry_Reset(void);

SpriteAssetHeader * SpriteAssetRegistry_FindById(SpriteAssetId registryId);

uint32_t SpriteAsset_RegisterAndRelocatePointers(SpriteAssetHeader *asset);


void SpriteAsset_CopyAndDerelocateImage(void *serializedDestination,SpriteAssetHeader *relocatedSourceImage);

#endif /* THANDOR_ASSETS_SPRITE_CATALOG_H */

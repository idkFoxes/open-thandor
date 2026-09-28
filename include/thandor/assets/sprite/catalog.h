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
/* Functions are grouped by semantic ownership; address comments are executable virtual addresses. */

/* 0x00486D60 */
void SpriteAsset_ValidateGroupCount(void);

/* 0x004BE480 */
void SpriteAssetRegistry_Reset(void);

/* 0x004BE490 */
SpriteAssetHeader * SpriteAssetRegistry_FindById(SpriteAssetId registryId);

/* 0x004BE4D0 */
SpriteRegisterResult SpriteAsset_RegisterAndRelocatePointers(SpriteAssetHeader *asset);


/* 0x004BE5A0 */
void SpriteAsset_CopyAndDerelocateImage(void *serializedDestination,SpriteAssetHeader *relocatedSourceImage);

#endif /* THANDOR_ASSETS_SPRITE_CATALOG_H */

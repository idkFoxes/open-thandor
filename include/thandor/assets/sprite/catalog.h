#ifndef THANDOR_ASSETS_SPRITE_CATALOG_H
#define THANDOR_ASSETS_SPRITE_CATALOG_H

#include <thandor/generated/v523_types.h>
#include <thandor/core/contracts.h>

/* Submodule: assets/sprite/catalog. */
/* Functions are grouped by semantic ownership; address comments are executable virtual addresses. */

/* 0x00486D60 */
void SpriteAsset_ValidateGroupCountCf(void);

/* 0x004BE480 */
void SpriteAssetRegistry_Reset(void);

/* 0x004BE490 */
SpriteAssetHeader * SpriteAssetRegistry_FindById(SpriteAssetId registryId);

/* 0x004BE4D0 */
SpriteAssetHeader * SpriteAsset_RegisterAndRelocatePointers(SpriteAssetHeader *asset);

#endif /* THANDOR_ASSETS_SPRITE_CATALOG_H */

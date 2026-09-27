/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/src/assets/sprite/catalog.c
 * Reverse engineering by idkFoxes 2026
 */

#include <thandor/assets/sprite/catalog.h>
#include <thandor/thandor.h>

/* Implementation ownership: assets/sprite/catalog. */

/* Address: 0x00486D60.
   Ownership: assets/sprite/catalog.
   Purpose: Validates the sprite asset magic and requires a group count from 1 through 0xFFF. Carry is clear for a
   valid asset and set for invalid input. Role: Validates the serialized SPR LOD-group count before relocation or
   rendering. Inputs: SPR header, including group count at header +0xB0. Outputs: Carry/status result used by the
   caller to reject malformed images.
*/
void SpriteAsset_ValidateGroupCount(void)

{
  return;
}

/* Address: 0x004BE480.
   Empties the registry of already relocated sprite assets (the list SpriteAssetRegistry_FindById walks),
   so the next load of any sprite registers and relocates it afresh.
*/
void SpriteAssetRegistry_Reset(void)

{
  g_SpriteAssetRegistryHead = NULL;
}

/* Address: 0x004BE490.
   Finds an already registered sprite asset by its id (+0xB8), walking the registry list from the most
   recently registered one; NULL when no asset has that id. Lets model loading reuse a sprite that another
   model already loaded.
*/
SpriteAssetHeader * __thandor_eax_preserve_ecx_edx
SpriteAssetRegistry_FindById(SpriteAssetId registryId)

{
  SpriteAssetHeader *spriteAssetCursor;

  for (spriteAssetCursor = g_SpriteAssetRegistryHead;
      (spriteAssetCursor != NULL &&
      (registryId != (spriteAssetCursor->registryHeader).registryId));
      spriteAssetCursor = (spriteAssetCursor->registryHeader).previousRegistryAsset) {
  }
  return spriteAssetCursor;
}


/* Address: 0x004BE4D0.
   Checks that the asset is an 'spr' of converter version 0x20007, prepends it to the sprite registry and
   turns the three serialized offsets of every 0x40-byte pointer record (in every block of every group) into
   absolute pointers. Must run exactly once per loaded image. Returns the asset with CF clear, or
   FATAL_ERROR_SPRITE_ASSET_INVALID with CF set.
*/
SpriteRegisterResult __thandor_eax_cf_preserve_ecx_edx
SpriteAsset_RegisterAndRelocatePointers(SpriteAssetHeader *asset)

{
  AssetAllocationSizeBytes relocationBlocksRemaining;
  int pointerRecordsRemaining;
  SprGroupRelocationHeader20 *groupRelocationCursor;
  SprRelocationBlockHeader20 *relocationBlockCursor;
  SprPointerRelocationRecord40 *pointerRelocationCursor;
  SpriteRegisterResult successResult;
  SpriteRegisterResult errorResult;
  AssetRecordCount groupsRemaining;
  SpriteAssetHeader *previousRegistryHead;

  previousRegistryHead = g_SpriteAssetRegistryHead;
  if (((asset->registryHeader).common.magic == ASSET_MAGIC_SPR) &&
     ((asset->registryHeader).common.converterVersion == PCK_CONVERTER_SPR_00020007)) {
    g_SpriteAssetRegistryHead = asset;
    (asset->registryHeader).previousRegistryAsset = previousRegistryHead;
    groupRelocationCursor = (SprGroupRelocationHeader20 *)(asset + 1);
    if ((asset->registryHeader).groupCount != 0) {
      groupsRemaining = (asset->registryHeader).groupCount;
      do {
        relocationBlocksRemaining = groupRelocationCursor->relocationBlockCount;
        if (relocationBlocksRemaining != 0) {
          relocationBlockCursor = (SprRelocationBlockHeader20 *)(groupRelocationCursor + 1);
          do {
            /* the pointer records follow the block header and its fixed 0x40-byte records */
            pointerRelocationCursor =
                 (SprPointerRelocationRecord40 *)
                 (relocationBlockCursor + relocationBlockCursor->fixedRecordCount40 * 2 + 1);
            for (pointerRecordsRemaining = relocationBlockCursor->pointerRelocationCount;
                pointerRecordsRemaining != 0; pointerRecordsRemaining--)
            {
              pointerRelocationCursor->pointerOrSerializedOffset00 =
                   (uint32_t)((asset->registryHeader).common.buildMetadata.assetRelativeAddressAnchor28
                          + (pointerRelocationCursor->pointerOrSerializedOffset00 - 0x28));
              pointerRelocationCursor->pointerOrSerializedOffset0C =
                   (uint32_t)((asset->registryHeader).common.buildMetadata.assetRelativeAddressAnchor28
                          + (pointerRelocationCursor->pointerOrSerializedOffset0C - 0x28));
              pointerRelocationCursor->pointerOrSerializedOffset18 =
                   (uint32_t)((asset->registryHeader).common.buildMetadata.assetRelativeAddressAnchor28
                          + (pointerRelocationCursor->pointerOrSerializedOffset18 - 0x28));
              pointerRelocationCursor++;
            }
            /* advance by the block's leading byte size (reserved10_1F lies at +0x10) */
            relocationBlockCursor =
                 (SprRelocationBlockHeader20 *)
                 (relocationBlockCursor->reserved10_1F +
                 (relocationBlockCursor->blockByteSize - 0x10));
            relocationBlocksRemaining--;
          } while (relocationBlocksRemaining != 0);
        }
        /* advance by the group's leading byte size (reserved08_1F lies at +0x08) */
        groupRelocationCursor =
             (SprGroupRelocationHeader20 *)
             (groupRelocationCursor->reserved08_1F +
             (groupRelocationCursor->nextGroupByteOffset - 8));
        groupsRemaining--;
      } while (groupsRemaining != 0);
    }
    successResult.failed = false;
    successResult.assetOrError = asset;
    return successResult;
  }
  errorResult.failed = true;
  errorResult.assetOrError = (SpriteAssetHeader *)FATAL_ERROR_SPRITE_ASSET_INVALID;
  return errorResult;
}

/* Address: 0x004BE5A0.
   Ownership: assets/sprite/catalog.
   Purpose: Copies and derelocates a sprite image asset.
*/
void __thandor_void_preserve_eax_ecx_edx
SpriteAsset_CopyAndDerelocateImage
          (void *serializedDestination,SpriteAssetHeader *relocatedSourceImage)

{
  uint32_t copyDwordsRemaining;
  int blocksRemaining;
  int recordsRemaining;
  SpriteAssetHeader *sourceCursor;
  int *blockCursor;
  int *groupCursor;
  AssetMagic *destinationCursor;
  int *recordCursor;
  int groupsRemaining;
  
  sourceCursor = relocatedSourceImage;
  destinationCursor = serializedDestination;
  for (copyDwordsRemaining = (relocatedSourceImage->registryHeader).common.allocationSizeBytes >> 2; copyDwordsRemaining != 0;
      copyDwordsRemaining = copyDwordsRemaining - 1) {
    *destinationCursor = (sourceCursor->registryHeader).common.magic;
    sourceCursor = (SpriteAssetHeader *)&(sourceCursor->registryHeader).common.allocationSizeBytes;
    destinationCursor = destinationCursor + 1;
  }
  groupCursor = (int *)((int)serializedDestination + 0x200);
  groupsRemaining = *(int *)((int)serializedDestination + 0xb0);
  do {
    blocksRemaining = groupCursor[1];
    blockCursor = groupCursor + 8;
    do {
      recordsRemaining = blockCursor[2];
      recordCursor = blockCursor + 8;
      do {
        recordCursor[0xc] = 0;
        recordCursor[0xd] = 0;
        recordCursor[8] = 0;
        recordCursor[9] = 0;
        recordCursor[10] = 0;
        recordCursor = recordCursor + 0x10;
        recordsRemaining = recordsRemaining + -1;
      } while (recordsRemaining != 0);
      recordsRemaining = blockCursor[3];
      do {
        *recordCursor = *recordCursor - (int)relocatedSourceImage;
        recordCursor[3] = recordCursor[3] - (int)relocatedSourceImage;
        recordCursor[6] = recordCursor[6] - (int)relocatedSourceImage;
        recordCursor = recordCursor + 0x10;
        recordsRemaining = recordsRemaining + -1;
      } while (recordsRemaining != 0);
      blockCursor = (int *)((int)blockCursor + *blockCursor);
      blocksRemaining = blocksRemaining + -1;
    } while (blocksRemaining != 0);
    groupCursor = (int *)((int)groupCursor + *groupCursor);
    groupsRemaining = groupsRemaining + -1;
  } while (groupsRemaining != 0);
  return;
}


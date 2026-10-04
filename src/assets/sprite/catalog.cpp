/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/src/assets/sprite/catalog.cpp
 * Reverse engineering by idkFoxes 2026
 */

#include <thandor/assets/sprite/catalog.h>
#include <thandor/thandor.h>

/* Module data. */

static SpriteAssetHeader *g_SpriteAssetRegistryHead = nullptr;

/* Empties the registry of already relocated sprite assets (the list SpriteAssetRegistry_FindById walks),
   so the next load of any sprite registers and relocates it afresh.
*/
void SpriteAssetRegistry_Reset()

{
  g_SpriteAssetRegistryHead = nullptr;
}

/* Finds an already registered sprite asset by its id (registryHeader.registryId), walking the registry list
   from the most recently registered one; NULL when no asset has that id. Lets model loading reuse a sprite that another
   model already loaded.
*/
SpriteAssetHeader * SpriteAssetRegistry_FindById(SpriteAssetId registryId)

{
  SpriteAssetHeader *spriteAssetCursor;

  spriteAssetCursor = g_SpriteAssetRegistryHead;
  while (spriteAssetCursor != nullptr && registryId != spriteAssetCursor->registryHeader.registryId) {
    spriteAssetCursor = spriteAssetCursor->registryHeader.previousRegistryAsset;
  }
  return spriteAssetCursor;
}


/* Checks that the asset is an 'spr' of converter version 0x20007, prepends it to the sprite registry and
   turns the three serialized offsets of every 0x40-byte pointer record (in every block of every group) into
   absolute pointers. Must run exactly once per loaded image. Returns 0 on success or
   FATAL_ERROR_SPRITE_ASSET_INVALID (the original's success return value, the asset itself, was read by no
   caller).
*/
uint32_t SpriteAsset_RegisterAndRelocatePointers(SpriteAssetHeader *asset)

{
  SprRelocationCount relocationBlocksRemaining;
  SprRelocationCount pointerRecordsRemaining;
  SprGroupRelocationHeader *groupRelocationCursor;
  SprRelocationBlockHeader *relocationBlockCursor;
  SprPointerRelocationRecord *pointerRelocationCursor;
  AssetRecordCount groupsRemaining;

  if ((asset->registryHeader.common.magic != ASSET_MAGIC_SPR) ||
      (asset->registryHeader.common.converterVersion != PCK_CONVERTER_SPR_00020007)) {
    return FATAL_ERROR_SPRITE_ASSET_INVALID;
  }
  asset->registryHeader.previousRegistryAsset = g_SpriteAssetRegistryHead;
  g_SpriteAssetRegistryHead = asset;

  groupRelocationCursor = (SprGroupRelocationHeader *)(asset + 1);
  for (groupsRemaining = asset->registryHeader.groupCount; groupsRemaining != 0; groupsRemaining--) {
    relocationBlockCursor = (SprRelocationBlockHeader *)(groupRelocationCursor + 1);
    for (relocationBlocksRemaining = groupRelocationCursor->relocationBlockCount;
         relocationBlocksRemaining != 0; relocationBlocksRemaining--) {
      /* the pointer records follow the block header and its fixed records (both 0x40 bytes) */
      pointerRelocationCursor = (SprPointerRelocationRecord *)(relocationBlockCursor + 1) +
                                relocationBlockCursor->fixedRecordCount;
      for (pointerRecordsRemaining = relocationBlockCursor->pointerRelocationCount;
           pointerRecordsRemaining != 0; pointerRecordsRemaining--) {
        /* asset start + serialized offset */
        pointerRelocationCursor->pointerOrSerializedOffset00 =
             Thandor_PointerToU32((uint8_t *)asset + pointerRelocationCursor->pointerOrSerializedOffset00); /* 5f-format: SprPointerRelocationRecord.pointerOrSerializedOffset00 */
        pointerRelocationCursor->pointerOrSerializedOffset0C =
             Thandor_PointerToU32((uint8_t *)asset + pointerRelocationCursor->pointerOrSerializedOffset0C); /* 5f-format: SprPointerRelocationRecord.pointerOrSerializedOffset0C */
        pointerRelocationCursor->pointerOrSerializedOffset18 =
             Thandor_PointerToU32((uint8_t *)asset + pointerRelocationCursor->pointerOrSerializedOffset18); /* 5f-format: SprPointerRelocationRecord.pointerOrSerializedOffset18 */
        pointerRelocationCursor++;
      }
      /* advance by the block's leading byte size */
      relocationBlockCursor = (SprRelocationBlockHeader *)
           ((uint8_t *)relocationBlockCursor + relocationBlockCursor->blockByteSize);
    }
    /* advance by the group's leading byte size */
    groupRelocationCursor = (SprGroupRelocationHeader *)
         ((uint8_t *)groupRelocationCursor + groupRelocationCursor->nextGroupByteOffset);
  }
  return 0;
}

/* Inverse of SpriteAsset_RegisterAndRelocatePointers: copies the relocated sprite asset (allocationSizeBytes
   long) to serializedDestination and turns the copy back into its serialized form, so it can be written out
   again: the runtime fields of every fixed 0x40-byte record are cleared and the three pointers of every
   pointer record become offsets from the asset start again. No caller or callback-table slot references it
   in this code base.
*/
void SpriteAsset_CopyAndDerelocateImage(void *serializedDestination,SpriteAssetHeader *relocatedSourceImage)

{
  uint32_t copyDwordsRemaining;
  int blocksRemaining;
  int recordsRemaining;
  SpriteAssetHeader *sourceCursor;
  SprRelocationBlockHeader *blockCursor;
  SprGroupRelocationHeader *groupCursor;
  AssetMagic *destinationCursor;
  int *recordCursor; /* 0x40-byte records (0x10 dwords) behind the block header */
  int groupsRemaining;

  sourceCursor = relocatedSourceImage;
  destinationCursor = (AssetMagic *)serializedDestination;
  /* dword copy of the whole asset; each step advances sourceCursor by one dword */
  for (copyDwordsRemaining = relocatedSourceImage->registryHeader.common.allocationSizeBytes >> 2;
       copyDwordsRemaining != 0; copyDwordsRemaining--) {
    *destinationCursor = sourceCursor->registryHeader.common.magic;
    sourceCursor = (SpriteAssetHeader *)&sourceCursor->registryHeader.common.allocationSizeBytes;
    destinationCursor++;
  }
  /* the groups follow the SpriteAssetHeader */
  groupCursor = (SprGroupRelocationHeader *)((SpriteAssetHeader *)serializedDestination + 1);
  groupsRemaining = ((SpriteAssetHeader *)serializedDestination)->registryHeader.groupCount;
  /* unlike the relocation, every count is assumed to be non-zero (a zero count would wrap around) */
  do {
    blocksRemaining = groupCursor->relocationBlockCount;
    blockCursor = (SprRelocationBlockHeader *)(groupCursor + 1);
    do {
      recordsRemaining = blockCursor->fixedRecordCount;
      recordCursor = (int *)(blockCursor + 1);
      do {
        recordCursor[12] = 0;
        recordCursor[13] = 0;
        recordCursor[8] = 0;
        recordCursor[9] = 0;
        recordCursor[10] = 0;
        recordCursor = recordCursor + 16;
        recordsRemaining--;
      } while (recordsRemaining != 0);
      recordsRemaining = blockCursor->pointerRelocationCount;
      do {
        /* pointerOrSerializedOffset00/0C/18 */
        *recordCursor = *recordCursor - Thandor_PointerToI32(relocatedSourceImage); /* 5f-format: SprPointerRelocationRecord.pointerOrSerializedOffset00 */
        recordCursor[3] = recordCursor[3] - Thandor_PointerToI32(relocatedSourceImage); /* 5f-format: SprPointerRelocationRecord.pointerOrSerializedOffset0C */
        recordCursor[6] = recordCursor[6] - Thandor_PointerToI32(relocatedSourceImage); /* 5f-format: SprPointerRelocationRecord.pointerOrSerializedOffset18 */
        recordCursor = recordCursor + 16;
        recordsRemaining--;
      } while (recordsRemaining != 0);
      blockCursor = (SprRelocationBlockHeader *)((uint8_t *)blockCursor + blockCursor->blockByteSize);
      blocksRemaining--;
    } while (blocksRemaining != 0);
    groupCursor = (SprGroupRelocationHeader *)((uint8_t *)groupCursor + groupCursor->nextGroupByteOffset);
    groupsRemaining--;
  } while (groupsRemaining != 0);
  return;
}


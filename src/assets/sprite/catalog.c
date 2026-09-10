#include <thandor/assets/sprite/catalog.h>

/* Implementation ownership: assets/sprite/catalog. */

/* Address: 0x00486D60.
   Ownership: assets/sprite/catalog.
   Purpose: Validates the sprite asset magic and requires a group count from 1 through 0xFFF. Carry is clear for a
   valid asset and set for invalid input. Role: Validates the serialized SPR LOD-group count before relocation or
   rendering. Inputs: SPR header, including group count at header +0xB0. Outputs: Carry/status result used by the
   caller to reject malformed images.
*/
void SpriteAsset_ValidateGroupCountCf(void)

{
  return;
}

/* Address: 0x004BE480.
   Ownership: assets/sprite/catalog.
   Purpose: Clears the global linked-list head used to reuse already prepared sprite assets by registryId.
*/
void SpriteAssetRegistry_Reset(void)

{
  g_SpriteAssetRegistryHead = (SpriteAssetHeader *)0x0;
  return;
}

/* Address: 0x004BE490.
   Ownership: assets/sprite/catalog.
   Purpose: Traverses g_SpriteAssetRegistryHead through previousRegistryAsset and returns the first asset whose
   registryId at +0xB8 matches the requested value, or null when absent. Sprite registry ids and relocated
   SpriteAssetHeader pointers remain distinct; no renderer flag or attachment-kind meaning is inferred from path
   names alone. It remains separate from relocated SpriteAssetHeader pointers, mesh flags, and attachment-kind
   semantics.
*/
SpriteAssetHeader * SpriteAssetRegistry_FindById(SpriteAssetId registryId)

{
  SpriteAssetHeader *spriteAssetCursor;
  
  for (spriteAssetCursor = g_SpriteAssetRegistryHead;
      (spriteAssetCursor != (SpriteAssetHeader *)0x0 &&
      (registryId != (spriteAssetCursor->registryHeader).registryId));
      spriteAssetCursor = (spriteAssetCursor->registryHeader).previousRegistryAsset) {
  }
  return spriteAssetCursor;
}

/* Address: 0x004BE4D0.
   Ownership: assets/sprite/catalog.
   Purpose: Validates the 'spr' magic and converter version 0x00020007, prepends the image to the sprite registry,
   then walks every variable-size group and record. Three stored offsets in each nested 0x40-byte reference record
   are converted to absolute pointers by adding the asset base. The operation is one-way and must run once per
   serialized image. CF clear returns the asset; CF set returns error 0x36 in EAX. Role: Registers one SPR image
   and converts serialized offsets into runtime pointers.
*/
SpriteAssetHeader * SpriteAsset_RegisterAndRelocatePointers(SpriteAssetHeader *asset)

{
  AssetAllocationSizeBytes relocationBlocksRemaining;
  int pointerRecordsRemaining;
  SprGroupRelocationHeader20 *groupRelocationCursor;
  SprRelocationBlockHeader20 *relocationBlockCursor;
  SprPointerRelocationRecord40 *pointerRelocationCursor;
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
            pointerRelocationCursor =
                 (SprPointerRelocationRecord40 *)
                 (relocationBlockCursor + relocationBlockCursor->fixedRecordCount40 * 2 + 1);
            for (pointerRecordsRemaining = relocationBlockCursor->pointerRelocationCount;
                pointerRecordsRemaining != 0; pointerRecordsRemaining = pointerRecordsRemaining - 1)
            {
              pointerRelocationCursor->pointerOrSerializedOffset00 =
                   (dword)((asset->registryHeader).common.buildMetadata.assetRelativeAddressAnchor28
                          + (pointerRelocationCursor->pointerOrSerializedOffset00 - 0x28));
              pointerRelocationCursor->pointerOrSerializedOffset0C =
                   (dword)((asset->registryHeader).common.buildMetadata.assetRelativeAddressAnchor28
                          + (pointerRelocationCursor->pointerOrSerializedOffset0C - 0x28));
              pointerRelocationCursor->pointerOrSerializedOffset18 =
                   (dword)((asset->registryHeader).common.buildMetadata.assetRelativeAddressAnchor28
                          + (pointerRelocationCursor->pointerOrSerializedOffset18 - 0x28));
              pointerRelocationCursor = pointerRelocationCursor + 1;
            }
            relocationBlockCursor =
                 (SprRelocationBlockHeader20 *)
                 (relocationBlockCursor->reserved10_1F +
                 (relocationBlockCursor->blockByteSize - 0x10));
            relocationBlocksRemaining = relocationBlocksRemaining - 1;
          } while (relocationBlocksRemaining != 0);
        }
        groupRelocationCursor =
             (SprGroupRelocationHeader20 *)
             (groupRelocationCursor->reserved08_1F +
             (groupRelocationCursor->nextGroupByteOffset - 8));
        groupsRemaining = groupsRemaining - 1;
      } while (groupsRemaining != 0);
    }
    return asset;
  }
  return (SpriteAssetHeader *)0x36;
}

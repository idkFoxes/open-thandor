/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/src/assets/sprite/catalog.c
 * Reverse engineering by idkFoxes 2026
 */

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
SpriteAssetHeader * __thandor_eax_preserve_ecx_edx
SpriteAssetRegistry_FindById(SpriteAssetId registryId)

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
SpriteRegisterRelocateEaxCf5 __thandor_eax_cf_preserve_ecx_edx
SpriteAsset_RegisterAndRelocatePointers(SpriteAssetHeader *asset)

{
  AssetAllocationSizeBytes relocationBlocksRemaining;
  int pointerRecordsRemaining;
  SprGroupRelocationHeader20 *groupRelocationCursor;
  SprRelocationBlockHeader20 *relocationBlockCursor;
  SprPointerRelocationRecord40 *pointerRelocationCursor;
  SpriteRegisterRelocateEaxCf5 SVar1;
  SpriteRegisterRelocateEaxCf5 SVar2;
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
    SVar1.carry = false;
    SVar1.assetOrError = asset;
    return SVar1;
  }
  SVar2.carry = true;
  SVar2.assetOrError = (SpriteAssetHeader *)0x36;
  return SVar2;
}

/* Address: 0x004BE5A0.
   Ownership: assets/sprite/catalog.
   Purpose: Copies and derelocates a sprite image asset.
*/
void __thandor_void_preserve_eax_ecx_edx
SpriteAsset_CopyAndDerelocateImage
          (void *serializedDestination,SpriteAssetHeader *relocatedSourceImage)

{
  uint uVar1;
  int iVar2;
  int iVar3;
  SpriteAssetHeader *pSVar4;
  int *piVar5;
  int *piVar6;
  AssetMagic *pAVar7;
  int *piVar8;
  int iStack_20;
  
  pSVar4 = relocatedSourceImage;
  pAVar7 = serializedDestination;
  for (uVar1 = (relocatedSourceImage->registryHeader).common.allocationSizeBytes >> 2; uVar1 != 0;
      uVar1 = uVar1 - 1) {
    *pAVar7 = (pSVar4->registryHeader).common.magic;
    pSVar4 = (SpriteAssetHeader *)&(pSVar4->registryHeader).common.allocationSizeBytes;
    pAVar7 = pAVar7 + 1;
  }
  piVar6 = (int *)((int)serializedDestination + 0x200);
  iStack_20 = *(int *)((int)serializedDestination + 0xb0);
  do {
    iVar2 = piVar6[1];
    piVar5 = piVar6 + 8;
    do {
      iVar3 = piVar5[2];
      piVar8 = piVar5 + 8;
      do {
        piVar8[0xc] = 0;
        piVar8[0xd] = 0;
        piVar8[8] = 0;
        piVar8[9] = 0;
        piVar8[10] = 0;
        piVar8 = piVar8 + 0x10;
        iVar3 = iVar3 + -1;
      } while (iVar3 != 0);
      iVar3 = piVar5[3];
      do {
        *piVar8 = *piVar8 - (int)relocatedSourceImage;
        piVar8[3] = piVar8[3] - (int)relocatedSourceImage;
        piVar8[6] = piVar8[6] - (int)relocatedSourceImage;
        piVar8 = piVar8 + 0x10;
        iVar3 = iVar3 + -1;
      } while (iVar3 != 0);
      piVar5 = (int *)((int)piVar5 + *piVar5);
      iVar2 = iVar2 + -1;
    } while (iVar2 != 0);
    piVar6 = (int *)((int)piVar6 + *piVar6);
    iStack_20 = iStack_20 + -1;
  } while (iStack_20 != 0);
  return;
}


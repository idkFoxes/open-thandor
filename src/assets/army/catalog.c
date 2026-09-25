/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/src/assets/army/catalog.c
 * Reverse engineering by idkFoxes 2026
 */

#include <thandor/assets/army/catalog.h>
#include <thandor/thandor.h>

/* Implementation ownership: assets/army/catalog. */

/* Address: 0x005719F0.
   Ownership: assets/army/catalog.
   Purpose: Tests recordId against the exact flag-0x0100-set and flag-0x0200-clear predicate. When absent, it
   forwards the current EAX ID into the wrapped-next search. EAX and CF remain the result channel. Stock ARM
   ledgers contain 675 records and 326 unique ids; flag-filtered stepping preserves the 32-bit registry key and
   does not imply gameplay class, tier, faction, or direction.
   Local calls: ArmyAssetRegistry_HasIdWithFlag0100Without0200Cf,
   ArmyAssetRegistry_FindNextFlag0100Without0200WrappedCf.
*/
ArmyRegistryIdEaxCf5_5719f0 __thandor_eax_cf_preserve_ecx_edx
ArmyAssetRegistry_NormalizeIdForFlag0100Without0200Cf(PckArmyAssetIdCatalog recordId)

{
  bool bVar1;
  ArmyRegistryIdEaxCf5_571ab0 AVar2;
  ArmyRegistryIdEaxCf5_5719f0 AVar3;
  
  bVar1 = (bool)ArmyAssetRegistry_HasIdWithFlag0100Without0200Cf(recordId);
  AVar2.carry = bVar1;
  AVar2.eax = recordId;
  if (bVar1) {
    AVar2 = ArmyAssetRegistry_FindNextFlag0100Without0200WrappedCf(recordId);
  }
  AVar3.eax = AVar2.eax;
  AVar3.carry = AVar2.carry;
  return AVar3;
}


/* Address: 0x00571A10.
   Ownership: assets/army/catalog.
   Purpose: Steps forward through IDs using the exact desired predicate and the broader flag-0x0200-clear
   predicate, with the verified reverse-gap fallback. EAX and CF remain intact. Stock ARM ledgers contain 675
   records and 326 unique ids; flag-filtered stepping preserves the 32-bit registry key and does not imply gameplay
   class, tier, faction, or direction.
   Local calls: ArmyAssetRegistry_HasIdWithFlag0100Without0200Cf, ArmyAssetRegistry_HasIdWithoutFlag0200Cf.
*/
ArmyRegistryIdEaxCf5_571a10 __thandor_eax_cf_preserve_ecx_edx
ArmyAssetRegistry_StepForwardFlag0100Without0200Cf(ArmyAssetId recordId)

{
  ArmyAssetId recordId_00;
  bool bVar1;
  ArmyRegistryIdEaxCf5_571a10 AVar2;
  
  while( true ) {
    recordId_00 = recordId;
    AVar2.eax = recordId_00 + 1;
    AVar2.carry = (bool)ArmyAssetRegistry_HasIdWithFlag0100Without0200Cf(AVar2.eax);
    if (!AVar2.carry) break;
    bVar1 = (bool)ArmyAssetRegistry_HasIdWithoutFlag0200Cf(AVar2.eax);
    recordId = AVar2.eax;
    if (bVar1) {
      do {
        recordId_00 = recordId_00 - 1;
        bVar1 = (bool)ArmyAssetRegistry_HasIdWithoutFlag0200Cf(recordId_00);
        recordId = recordId_00;
      } while (!bVar1);
    }
  }
  return AVar2;
}


/* Address: 0x00571A60.
   Ownership: assets/army/catalog.
   Purpose: Steps backward through IDs using the exact desired predicate and the broader flag-0x0200-clear
   predicate, with the verified forward-gap fallback. EAX and CF remain intact. Stock ARM ledgers contain 675
   records and 326 unique ids; flag-filtered stepping preserves the 32-bit registry key and does not imply gameplay
   class, tier, faction, or direction.
   Local calls: ArmyAssetRegistry_HasIdWithFlag0100Without0200Cf, ArmyAssetRegistry_HasIdWithoutFlag0200Cf.
*/
ArmyRegistryIdEaxCf5_571a60 __thandor_eax_cf_preserve_ecx_edx
ArmyAssetRegistry_StepBackwardFlag0100Without0200Cf(ArmyAssetId recordId)

{
  ArmyAssetId recordId_00;
  bool bVar1;
  ArmyRegistryIdEaxCf5_571a60 AVar2;
  
  while( true ) {
    recordId_00 = recordId;
    AVar2.eax = recordId_00 - 1;
    AVar2.carry = (bool)ArmyAssetRegistry_HasIdWithFlag0100Without0200Cf(AVar2.eax);
    if (!AVar2.carry) break;
    bVar1 = (bool)ArmyAssetRegistry_HasIdWithoutFlag0200Cf(AVar2.eax);
    recordId = AVar2.eax;
    if (bVar1) {
      do {
        recordId_00 = recordId_00 + 1;
        bVar1 = (bool)ArmyAssetRegistry_HasIdWithoutFlag0200Cf(recordId_00);
        recordId = recordId_00;
      } while (!bVar1);
    }
  }
  return AVar2;
}


/* Address: 0x00571B00.
   Ownership: assets/army/catalog.
   Purpose: Finds the previous ID in the flag-0x0100-set and flag-0x0200-clear class, wrapping signed underflow to
   0x1000. EAX and CF remain intact. Stock ARM ledgers contain 675 records and 326 unique ids; flag-filtered
   stepping preserves the 32-bit registry key and does not imply gameplay class, tier, faction, or direction.
   Local calls: ArmyAssetRegistry_HasIdWithoutFlag0200Cf, ArmyAssetRegistry_HasIdWithFlag0100Without0200Cf.
*/
ArmyRegistryIdEaxCf5_571b00 __thandor_eax_cf_preserve_ecx_edx
ArmyAssetRegistry_FindPreviousFlag0100Without0200WrappedCf(ArmyAssetId recordId)

{
  bool bVar1;
  ArmyRegistryIdEaxCf5_571b00 AVar2;
  
  do {
    bVar1 = (bool)ArmyAssetRegistry_HasIdWithoutFlag0200Cf(recordId);
    if (bVar1)
    goto 
    ArmyAssetRegistry_FindPreviousFlag0100Without0200WrappedCf_ScanPreviousQualifiedCandidateWithWrap
    ;
    recordId = recordId - 1;
  } while (-1 < (int)recordId);
  recordId = 0x1000;
ArmyAssetRegistry_FindPreviousFlag0100Without0200WrappedCf_ScanPreviousQualifiedCandidateWithWrap:
  while( true ) {
    AVar2.carry = (bool)ArmyAssetRegistry_HasIdWithFlag0100Without0200Cf(recordId);
    if (!AVar2.carry) break;
    recordId = recordId - 1;
    if ((int)recordId < 0) {
      recordId = 0x1000;
    }
  }
  AVar2.eax = recordId;
  return AVar2;
}


/* Address: 0x00571C30.
   Ownership: assets/army/catalog.
   Purpose: Tests recordId against the exact flags-0x0100-and-0x0200-set predicate. When absent, it forwards the
   current EAX ID into the wrapped-next search. EAX and CF remain the result channel. Stock ARM ledgers contain 675
   records and 326 unique ids; flag-filtered stepping preserves the 32-bit registry key and does not imply gameplay
   class, tier, faction, or direction.
   Local calls: ArmyAssetRegistry_HasIdWithFlags0100And0200Cf, ArmyAssetRegistry_FindNextFlags0100And0200WrappedCf.
*/
ArmyRegistryIdEaxCf5_571c30 __thandor_eax_cf_preserve_ecx_edx
ArmyAssetRegistry_NormalizeIdForFlags0100And0200Cf(PckArmyAssetIdCatalog recordId)

{
  bool bVar1;
  ArmyRegistryIdEaxCf5_571cf0 AVar2;
  ArmyRegistryIdEaxCf5_571c30 AVar3;
  
  bVar1 = (bool)ArmyAssetRegistry_HasIdWithFlags0100And0200Cf(recordId);
  AVar2.carry = bVar1;
  AVar2.eax = recordId;
  if (bVar1) {
    AVar2 = ArmyAssetRegistry_FindNextFlags0100And0200WrappedCf(recordId);
  }
  AVar3.eax = AVar2.eax;
  AVar3.carry = AVar2.carry;
  return AVar3;
}


/* Address: 0x00571C50.
   Ownership: assets/army/catalog.
   Purpose: Steps forward through IDs using the exact combined-flags predicate and the broader flag-0x0200-set
   predicate, with the verified reverse-gap fallback. EAX and CF remain intact. Stock ARM ledgers contain 675
   records and 326 unique ids; flag-filtered stepping preserves the 32-bit registry key and does not imply gameplay
   class, tier, faction, or direction.
   Local calls: ArmyAssetRegistry_HasIdWithFlags0100And0200Cf, ArmyAssetRegistry_HasIdWithFlag0200Cf.
*/
ArmyRegistryIdEaxCf5_571c50 __thandor_eax_cf_preserve_ecx_edx
ArmyAssetRegistry_StepForwardFlags0100And0200Cf(ArmyAssetId recordId)

{
  ArmyAssetId recordId_00;
  bool bVar1;
  ArmyRegistryIdEaxCf5_571c50 AVar2;
  
  while( true ) {
    recordId_00 = recordId;
    AVar2.eax = recordId_00 + 1;
    AVar2.carry = (bool)ArmyAssetRegistry_HasIdWithFlags0100And0200Cf(AVar2.eax);
    if (!AVar2.carry) break;
    bVar1 = (bool)ArmyAssetRegistry_HasIdWithFlag0200Cf(AVar2.eax);
    recordId = AVar2.eax;
    if (bVar1) {
      do {
        recordId_00 = recordId_00 - 1;
        bVar1 = (bool)ArmyAssetRegistry_HasIdWithFlag0200Cf(recordId_00);
        recordId = recordId_00;
      } while (!bVar1);
    }
  }
  return AVar2;
}


/* Address: 0x00571CA0.
   Ownership: assets/army/catalog.
   Purpose: Steps backward through IDs using the exact combined-flags predicate and the broader flag-0x0200-set
   predicate, with the verified forward-gap fallback. EAX and CF remain intact. Stock ARM ledgers contain 675
   records and 326 unique ids; flag-filtered stepping preserves the 32-bit registry key and does not imply gameplay
   class, tier, faction, or direction.
   Local calls: ArmyAssetRegistry_HasIdWithFlags0100And0200Cf, ArmyAssetRegistry_HasIdWithFlag0200Cf.
*/
ArmyRegistryIdEaxCf5_571ca0 __thandor_eax_cf_preserve_ecx_edx
ArmyAssetRegistry_StepBackwardFlags0100And0200Cf(ArmyAssetId recordId)

{
  ArmyAssetId recordId_00;
  bool bVar1;
  ArmyRegistryIdEaxCf5_571ca0 AVar2;
  
  while( true ) {
    recordId_00 = recordId;
    AVar2.eax = recordId_00 - 1;
    AVar2.carry = (bool)ArmyAssetRegistry_HasIdWithFlags0100And0200Cf(AVar2.eax);
    if (!AVar2.carry) break;
    bVar1 = (bool)ArmyAssetRegistry_HasIdWithFlag0200Cf(AVar2.eax);
    recordId = AVar2.eax;
    if (bVar1) {
      do {
        recordId_00 = recordId_00 + 1;
        bVar1 = (bool)ArmyAssetRegistry_HasIdWithFlag0200Cf(recordId_00);
        recordId = recordId_00;
      } while (!bVar1);
    }
  }
  return AVar2;
}


/* Address: 0x00571D40.
   Ownership: assets/army/catalog.
   Purpose: Finds the previous ID with flags 0x0100 and 0x0200 set, wrapping signed underflow to 0x1000. EAX and CF
   remain intact. Stock ARM ledgers contain 675 records and 326 unique ids; flag-filtered stepping preserves the
   32-bit registry key and does not imply gameplay class, tier, faction, or direction.
   Local calls: ArmyAssetRegistry_HasIdWithFlag0200Cf, ArmyAssetRegistry_HasIdWithFlags0100And0200Cf.
*/
ArmyRegistryIdEaxCf5_571d40 __thandor_eax_cf_preserve_ecx_edx
ArmyAssetRegistry_FindPreviousFlags0100And0200WrappedCf(ArmyAssetId recordId)

{
  bool bVar1;
  ArmyRegistryIdEaxCf5_571d40 AVar2;
  
  do {
    bVar1 = (bool)ArmyAssetRegistry_HasIdWithFlag0200Cf(recordId);
    if (bVar1)
    goto 
    ArmyAssetRegistry_FindPreviousFlags0100And0200WrappedCf_ScanPreviousQualifiedCandidateWithWrap;
    recordId = recordId - 1;
  } while (-1 < (int)recordId);
  recordId = 0x1000;
ArmyAssetRegistry_FindPreviousFlags0100And0200WrappedCf_ScanPreviousQualifiedCandidateWithWrap:
  while( true ) {
    AVar2.carry = (bool)ArmyAssetRegistry_HasIdWithFlags0100And0200Cf(recordId);
    if (!AVar2.carry) break;
    recordId = recordId - 1;
    if ((int)recordId < 0) {
      recordId = 0x1000;
    }
  }
  AVar2.eax = recordId;
  return AVar2;
}


/* Address: 0x0051B5E0.
   Ownership: assets/army/catalog.
   Purpose: Validates the 'arm' magic and converter version 0x00020008, then prepares recordCount variable-size
   records beginning at +0x200. Each successful record advances by its leading byteSize. Invalid headers update the
   package last-error path. CF and EAX status are preserved.
   Local calls: ArmyAssetRecord_RegisterAndRelocate.
   Cross-module calls: Package_SetLastErrorPath [assets/package/runtime].
*/
StatusValueEaxCf5 __thandor_void_preserve_ecx_edx ArmyAsset_PrepareRecords(ArmyAssetHeader *asset)

{
  dword registrationStatusCode;
  AssetRecordCount AVar1;
  ArmyAssetHeader *record;
  StatusValueEaxCf5 SVar2;
  StatusValueEaxCf5 SVar3;
  
  registrationStatusCode = 0x40;
  if (((asset->recordCountHeader).common.magic == ASSET_MAGIC_ARM) &&
     ((asset->recordCountHeader).common.converterVersion == PCK_CONVERTER_ARM_00020008)) {
    AVar1 = (asset->recordCountHeader).recordCount;
    record = asset + 1;
    while( true ) {
      if (AVar1 == 0) {
        SVar2.carry = false;
        SVar2.valueOrError = registrationStatusCode;
        return SVar2;
      }
      SVar2 = ArmyAssetRecord_RegisterAndRelocate((ArmyAssetRuntimeSemanticView80 *)record,asset);
      registrationStatusCode = SVar2.valueOrError;
      if (SVar2.carry) break;
      record = (ArmyAssetHeader *)
               ((int)(record->recordCountHeader).common.buildMetadata.assetRelativeAddressAnchor28 +
               ((record->recordCountHeader).common.magic - 0x28));
      AVar1 = AVar1 - 1;
    }
  }
  else {
    Package_SetLastErrorPath((word *)asset);
  }
  SVar3.carry = true;
  SVar3.valueOrError = registrationStatusCode;
  return SVar3;
}


/* Address: 0x0051B740.
   Ownership: assets/army/catalog.
   Purpose: Resolves an army asset by registry identifier and returns success only when the asset is present and
   its verified enabled flag is set.
   Local calls: ArmyAssetRegistry_FindByIdCf.
*/
bool __thandor_cf_preserve_eax_ecx_edx
ArmyAssetRegistry_FindEnabledByIdCf(PckArmyAssetIdCatalog recordId)

{
  bool bVar1;
  ArmyRegistryEaxCf5_51b6d0 AVar2;
  
  AVar2 = ArmyAssetRegistry_FindByIdCf(recordId);
  bVar1 = AVar2.carry;
  if (!bVar1) {
    bVar1 = (AVar2.eax[1].selectionDetailTemplateVariantIndex & 1) == 0;
  }
  return bVar1;
}


/* Address: 0x0051B770.
   Ownership: assets/army/catalog.
   Purpose: Scans the sixteen linked army-asset identifiers and succeeds when an enabled asset has a faction-
   unlocked linked definition with the required nonzero state and flag overlap. It is distinct from
   FrontendPlayerIndex_V306, PlayerRuntimeId, active-faction masks or codes, and PCK-backed ArmyAssetId,
   ModelDefinitionId, and TechnologyId domains.
   Local calls: ArmyAssetRegistry_FindByIdCf.
   Cross-module calls: ModelDefinitionHierarchy_AllTechnologyUnlockedForFactionCf [assets/model/definitions].
*/
bool __thandor_cf_preserve_eax_ecx_edx
ArmyAssetRecord_HasFactionUnlockedLinkedDefinitionCf
          (FactionRuntimeIndex factionIndex,dword requiredDefinitionFlags,
          ArmyAssetRecordPrefix *armyAssetRecord)

{
  ArmyAssetRecordPrefix *definitionNode;
  int iVar1;
  bool bVar2;
  ArmyRegistryEaxCf5_51b6d0 AVar3;
  
  iVar1 = 0x10;
  do {
    if (armyAssetRecord[3].byteSize != 0) {
      AVar3 = ArmyAssetRegistry_FindByIdCf(armyAssetRecord[3].byteSize);
      definitionNode = AVar3.eax;
      if ((!AVar3.carry) && ((definitionNode[1].selectionDetailTemplateVariantIndex & 1) != 0)) {
        bVar2 = ModelDefinitionHierarchy_AllTechnologyUnlockedForFactionCf
                          (factionIndex,(ModelDefinitionHierarchyNodeAddress32)definitionNode);
        if ((!bVar2) &&
           ((definitionNode[1].rootNodeOffsetOrPointer != 0 &&
            ((definitionNode[1].selectionDetailTemplateVariantIndex & requiredDefinitionFlags) != 0)
            ))) {
          return true;
        }
      }
    }
    armyAssetRecord = (ArmyAssetRecordPrefix *)&armyAssetRecord->selectionDetailTemplateVariantIndex
    ;
    iVar1 = iVar1 + -1;
    if (iVar1 == 0) {
      return false;
    }
  } while( true );
}


/* Address: 0x00571E40.
   Ownership: assets/army/catalog.
   Purpose: Clears cached ArmyAsset preview textures, frees existing cache entries, and refreshes the two selected
   preview resources.
   Local calls: ArmyAssetRegistry_ResolveOrCreatePreviewTextureCf.
*/
void __thandor_void_preserve_eax_ecx
ArmyAssetRegistry_ClearPreviewTextureCacheAndRefreshSelected(dword selectedArmyAssetRegistryId)

{
  dword dVar1;
  int iVar2;
  ArmyAssetRecordPrefix **ppAVar3;
  ArmyAssetRecordPrefix *armyDefinition1;
  
  ppAVar3 = g_ArmyAssetRecordRegistry;
  iVar2 = 0x300;
  do {
    armyDefinition1 = *ppAVar3;
    if (armyDefinition1 != (ArmyAssetRecordPrefix *)0x0) {
      (*g_MemoryApi.free)((void *)armyDefinition1[2].byteSize);
      armyDefinition1[2].byteSize = 0;
    }
    ppAVar3 = ppAVar3 + 1;
    iVar2 = iVar2 + -1;
  } while (iVar2 != 0);
  dVar1 = ArmyAssetRegistry_ResolveOrCreatePreviewTextureCf(g_UiCommandModeGArmyAssetId);
  *(dword *)(selectedArmyAssetRegistryId + 0x9d24) = dVar1;
  dVar1 = ArmyAssetRegistry_ResolveOrCreatePreviewTextureCf(g_UiCommandMode4ArmyAssetId);
  *(dword *)(selectedArmyAssetRegistryId + 0x9ddc) = dVar1;
  return;
}


static dword ArmyAssetHierarchy_SumArmourFrom(FactionRuntimeIndex factionIndex,byte *node)
{
  ModelDefinitionLookupEaxCf5 selected;
  dword sum;
  dword i;
  selected = ModelDefinition_SelectFactionUnlockedLinkedDefinitionCf
                       (factionIndex,(ModelLinkedDefinitionListAddress32)(uintptr_t)node);
  sum = *(dword *)((byte *)selected.modelDefinition + 0x60);
  for (i = 0; i < *(dword *)(node + 8); i++) {
    byte *child = *(byte **)(node + 0xc + i * 4);
    if (child != (byte *)0x0) {
      sum = sum + ArmyAssetHierarchy_SumArmourFrom(factionIndex,child);
    }
  }
  return sum;
}

/* Address: 0x0051C170.
   Ownership: assets/army/catalog.
   Purpose: Traverses the linked model-definition hierarchy, resolves the faction-unlocked definition at each node,
   and sums the dword at definition offset 0x60. It is distinct from FrontendPlayerIndex_V306, PlayerRuntimeId,
   active-faction masks or codes, and PCK-backed ArmyAssetId, ModelDefinitionId, and TechnologyId domains. Typed
   parameters: p3 definitionNode→ModelDefinitionHierarchyNodeAddress32_V345. Calling convention, complete
   VariableStorage serialization, function bytes, control flow, globals, locals, and executable data remain
   unchanged.
   Cross-module calls: ModelDefinition_SelectFactionUnlockedLinkedDefinitionCf [assets/model/definitions].
*/
dword __thandor_eax_preserve_ecx_edx
ArmyAssetHierarchy_SumFactionUnlockedArmour
          (FactionRuntimeIndex factionIndex,ModelDefinitionHierarchyNodeAddress32 definitionNode)

{
  /* Rewritten from the assembly: the original walks the definition tree (child count at +0x08,
     children at +0x0C + 4*i) depth-first with frames on the machine stack. */
  return ArmyAssetHierarchy_SumArmourFrom(factionIndex,*(byte **)(uintptr_t)(definitionNode + 0xc));
}


static EnergyDemandQ4 ArmyAssetHierarchy_SumEnergyFrom(FactionRuntimeIndex factionIndex,byte *node)
{
  ModelDefinitionLookupEaxCf5 selected;
  EnergyDemandQ4 sum;
  dword childCount;
  dword i;
  selected = ModelDefinition_SelectFactionUnlockedLinkedDefinitionCf
                       (factionIndex,(ModelLinkedDefinitionListAddress32)(uintptr_t)node);
  sum = *(EnergyDemandQ4 *)((byte *)selected.modelDefinition + 0x18c);
  childCount = *(dword *)(node + 8);
  if ((*(dword *)((byte *)selected.modelDefinition + 0x68) & 0x80) == 0) {
    childCount = 0; /* only definitions with flag 0x80 contribute their children */
  }
  for (i = 0; i < childCount; i++) {
    sum = sum + ArmyAssetHierarchy_SumEnergyFrom(factionIndex,*(byte **)(node + 0xc + i * 4));
  }
  return sum;
}

/* Address: 0x0051C2C0.
   Ownership: assets/army/catalog.
   Purpose: Traverses the linked model-definition hierarchy, resolves the faction-unlocked definition at each node,
   and sums the dword at definition offset 0x18C. It is distinct from FrontendPlayerIndex_V306, PlayerRuntimeId,
   active-faction masks or codes, and PCK-backed ArmyAssetId, ModelDefinitionId, and TechnologyId domains. Typed
   parameters: p3 definitionNode→ModelDefinitionHierarchyNodeAddress32_V345. Calling convention, complete
   VariableStorage serialization, function bytes, control flow, globals, locals, and executable data remain
   unchanged. [RESOURCE_FUEL_ENERGY_CAPACITY_SEPARATION_CLOSURE] Sums faction-unlocked MDL +0x18C values across the
   model hierarchy.
   Cross-module calls: ModelDefinition_SelectFactionUnlockedLinkedDefinitionCf [assets/model/definitions].
*/
EnergyDemandQ4 __thandor_eax_preserve_ecx_edx
ArmyAssetHierarchy_SumFactionUnlockedDisplayedEnergyQ4
          (FactionRuntimeIndex factionIndex,ModelDefinitionHierarchyNodeAddress32 definitionNode)

{
  /* Rewritten from the assembly: the original walks the definition tree (child count at +0x08,
     children at +0x0C + 4*i) depth-first with frames on the machine stack. */
  return ArmyAssetHierarchy_SumEnergyFrom(factionIndex,*(byte **)(uintptr_t)(definitionNode + 0xc));
}


/* Address: 0x00571AB0.
   Ownership: assets/army/catalog.
   Purpose: Finds the next ID in the flag-0x0100-set and flag-0x0200-clear class, first advancing to a broader
   valid record and then wrapping the desired search at 0x1000. EAX and CF remain intact. Stock ARM ledgers contain
   675 records and 326 unique ids; flag-filtered stepping preserves the 32-bit registry key and does not imply
   gameplay class, tier, faction, or direction.
   Local calls: ArmyAssetRegistry_HasIdWithoutFlag0200Cf, ArmyAssetRegistry_HasIdWithFlag0100Without0200Cf.
*/
ArmyRegistryIdEaxCf5_571ab0 __thandor_eax_cf_preserve_ecx_edx
ArmyAssetRegistry_FindNextFlag0100Without0200WrappedCf(ArmyAssetId recordId)

{
  bool bVar1;
  ArmyRegistryIdEaxCf5_571ab0 AVar2;
  
  while( true ) {
    bVar1 = (bool)ArmyAssetRegistry_HasIdWithoutFlag0200Cf(recordId);
    if (bVar1) break;
    recordId = recordId + 1;
  }
  while( true ) {
    AVar2.carry = (bool)ArmyAssetRegistry_HasIdWithFlag0100Without0200Cf(recordId);
    if (!AVar2.carry) break;
    recordId = recordId + 1;
    if (0xfff < recordId) {
      recordId = 0;
    }
  }
  AVar2.eax = recordId;
  return AVar2;
}


/* Address: 0x00571CF0.
   Ownership: assets/army/catalog.
   Purpose: Finds the next ID with flags 0x0100 and 0x0200 set, first advancing to a broader flag-0x0200 record and
   then wrapping the desired search at 0x1000. EAX and CF remain intact. Stock ARM ledgers contain 675 records and
   326 unique ids; flag-filtered stepping preserves the 32-bit registry key and does not imply gameplay class,
   tier, faction, or direction.
   Local calls: ArmyAssetRegistry_HasIdWithFlag0200Cf, ArmyAssetRegistry_HasIdWithFlags0100And0200Cf.
*/
ArmyRegistryIdEaxCf5_571cf0 __thandor_eax_cf_preserve_ecx_edx
ArmyAssetRegistry_FindNextFlags0100And0200WrappedCf(ArmyAssetId recordId)

{
  bool bVar1;
  ArmyRegistryIdEaxCf5_571cf0 AVar2;
  
  while( true ) {
    bVar1 = (bool)ArmyAssetRegistry_HasIdWithFlag0200Cf(recordId);
    if (bVar1) break;
    recordId = recordId + 1;
  }
  while( true ) {
    AVar2.carry = (bool)ArmyAssetRegistry_HasIdWithFlags0100And0200Cf(recordId);
    if (!AVar2.carry) break;
    recordId = recordId + 1;
    if (0xfff < recordId) {
      recordId = 0;
    }
  }
  AVar2.eax = recordId;
  return AVar2;
}


/* Address: 0x0051B4A0.
   Ownership: assets/army/catalog.
   Purpose: Rejects duplicate registryId values, inserts the record into the first free entry of the fixed
   768-pointer registry, converts rootNodeOffsetOrPointer to an absolute pointer using the asset base, and
   recursively prepares nested model references and child offsets. CF status and EAX errors are preserved. Role:
   Registers an army/placeable record and resolves its root model definition. Inputs: Serialized army asset record
   with model-definition ID/offset. Outputs: Army/placeable definition linked to a ModelDefinition.
   Local calls: ArmyAssetRegistry_FindByIdCf.
   Cross-module calls: ModelDefinitionRegistry_FindBuildMetricTupleByIdCf [assets/model/definitions].
*/
/* Relocates one node of an army record's model tree and all of its children (offsets 0x0C + 4*i,
   count at +0x08) against assetBase, adding each node's model-definition build metrics (looked up by
   the id at +0x20) to the record. Returns the last lookup error, or 0. */
static dword ArmyAssetRecord_RelocateModelTree
          (ArmyAssetRuntimeSemanticView80 *record,byte *assetBase,byte *node)
{
  ModelBuildMetricEaxEcxEdxCf13 metrics;
  dword childCount;
  dword childIndex;
  dword error = 0;
  dword childError;

  metrics = ModelDefinitionRegistry_FindBuildMetricTupleByIdCf(*(PckModelDefinitionIdCatalog *)(node + 0x20));
  if (metrics.carry) {
    error = (dword)metrics.metric0;
  }
  else {
    record->relocationPointerOrOffset2C = record->relocationPointerOrOffset2C + (dword)metrics.metric0;
    record->relocationValue24 = record->relocationValue24 + metrics.metric1;
    record->relocationValue28 = record->relocationValue28 + metrics.metric2;
  }
  childCount = *(dword *)(node + 8);
  for (childIndex = 0; childIndex < childCount; childIndex = childIndex + 1) {
    byte **child = (byte **)(node + 0xc + childIndex * 4);
    *child = *child + (uintptr_t)assetBase;
    childError = ArmyAssetRecord_RelocateModelTree(record,assetBase,*child);
    if (childError != 0) {
      error = childError;
    }
  }
  return error;
}

StatusValueEaxCf5 __thandor_void_preserve_ecx_edx
ArmyAssetRecord_RegisterAndRelocate
          (ArmyAssetRuntimeSemanticView80 *record,ArmyAssetHeader *assetBase)

{
  /* Rewritten from the assembly (0x0051B4A0-0x0051B5D8): the model tree walk kept its recursion on
     the machine stack, which the decompiler could not express. */
  ArmyAssetRecordPrefix **slot;
  int slotsRemaining;
  ArmyRegistryEaxCf5_51b6d0 existing;
  StatusValueEaxCf5 status;

  existing = ArmyAssetRegistry_FindByIdCf(record->registryId);
  if (!existing.carry) {
    (*g_WideNumberFormatUtf16)
              (WIDE_FORMAT_WRITE_TERMINATOR,0,10,1,record->registryId,g_PackageLastErrorPath);
    status.carry = true;
    status.valueOrError = 0x4c;
    return status;
  }
  slot = g_ArmyAssetRecordRegistry;
  for (slotsRemaining = 0x300; slotsRemaining != 0; slotsRemaining = slotsRemaining + -1) {
    if (*slot == (ArmyAssetRecordPrefix *)0x0) {
      dword error = 0;
      *slot = (ArmyAssetRecordPrefix *)record;
      if (record->rootNodeOffsetOrPointer != 0) {
        record->rootNodeOffsetOrPointer = record->rootNodeOffsetOrPointer + (dword)(uintptr_t)assetBase;
        error = ArmyAssetRecord_RelocateModelTree
                          (record,(byte *)assetBase,(byte *)(uintptr_t)record->rootNodeOffsetOrPointer);
      }
      status.carry = error != 0;
      status.valueOrError = error;
      return status;
    }
    slot = slot + 1;
  }
  (*g_WideNumberFormatUtf16)(WIDE_FORMAT_WRITE_TERMINATOR,0,10,1,0x300,g_PackageLastErrorPath);
  status.carry = true;
  status.valueOrError = 0x42;
  return status;
}


/* Address: 0x00571D90.
   Ownership: assets/army/catalog.
   Purpose: Looks up an ArmyAsset registry record by id and returns its cached preview texture; if absent it
   creates and caches the preview through ArmyRuntime_RenderPreviewTextureCf.
   Cross-module calls: ArmyRuntime_RenderPreviewTextureCf [gameplay/army/runtime].
*/
dword ArmyAssetRegistry_ResolveOrCreatePreviewTextureCf(dword armyAssetRegistryId)

{
  ArmyAssetRecordPrefix *pAVar1;
  int iVar2;
  ArmyAssetRecordPrefix **ppAVar3;
  ArmyPreviewTextureEaxCf5 AVar4;
  FactionRuntimeIndex factionIndex;
  
  ppAVar3 = g_ArmyAssetRecordRegistry;
  iVar2 = 0x300;
  while ((pAVar1 = *ppAVar3, pAVar1 == (ArmyAssetRecordPrefix *)0x0 ||
         (armyAssetRegistryId != pAVar1->registryId))) {
    ppAVar3 = ppAVar3 + 1;
    iVar2 = iVar2 + -1;
    if (iVar2 == 0) {
      return 0;
    }
  }
  if (pAVar1[2].byteSize == 0) {
    factionIndex = g_UiCommandModeGOwnerFactionIndex;
    if (399 < armyAssetRegistryId) {
      factionIndex = 0;
    }
    AVar4 = ArmyRuntime_RenderPreviewTextureCf
                      (*(GraphicsPixelDimension *)(g_InGameRuntimeRoot->opaque9A74_9B4B + 0x5c),
                       *(GraphicsPixelDimension *)(g_InGameRuntimeRoot->opaque9A74_9B4B + 0x5c),
                       factionIndex,armyAssetRegistryId,&g_InGameRuntimeRoot->worldRuntime0A30);
    if (AVar4.carry) {
      return 0;
    }
    pAVar1[2].byteSize = (AssetRecordByteCount)AVar4.previewTexture;
    return (dword)AVar4.previewTexture;
  }
  return pAVar1[2].byteSize;
}


/* Address: 0x0051B6D0.
   Ownership: assets/army/catalog.
   Purpose: Scans the fixed 768-pointer army registry for registryId. A match returns the record with CF clear.
   Failure formats the requested identifier into the package error buffer and returns error 0x41 with CF set. Stock
   ARM ledgers contain 675 records and 326 unique ids; flag-filtered stepping preserves the 32-bit registry key and
   does not imply gameplay class, tier, faction, or direction.
*/
ArmyRegistryEaxCf5_51b6d0 __thandor_eax_cf_preserve_ecx_edx
ArmyAssetRegistry_FindByIdCf(PckArmyAssetIdCatalog registryId)

{
  ArmyAssetRecordPrefix *pAVar1;
  int registrySlotsRemaining;
  ArmyAssetRecordPrefix **registryCursor;
  ArmyRegistryEaxCf5_51b6d0 AVar2;
  ArmyRegistryEaxCf5_51b6d0 AVar3;
  ArmyAssetRecordPrefix *candidateAsset;
  
  registryCursor = g_ArmyAssetRecordRegistry;
  registrySlotsRemaining = 0x300;
  while ((pAVar1 = *registryCursor, pAVar1 == (ArmyAssetRecordPrefix *)0x0 ||
         (pAVar1->registryId != registryId))) {
    registryCursor = registryCursor + 1;
    registrySlotsRemaining = registrySlotsRemaining + -1;
    if (registrySlotsRemaining == 0) {
      (*g_WideNumberFormatUtf16)
                (WIDE_FORMAT_WRITE_TERMINATOR,0,10,1,registryId,g_PackageLastErrorPath);
      AVar2.carry = true;
      AVar2.eax = (ArmyAssetRecordPrefix *)0x41;
      return AVar2;
    }
  }
  AVar3.carry = false;
  AVar3.eax = pAVar1;
  return AVar3;
}


/* Address: 0x00571910.
   Ownership: assets/army/catalog.
   Purpose: Scans all 768 ArmyAsset registry pointers for recordId. CF clear means a matching record was found with
   flags dword +0x14 bit 0x0200 clear; CF set means absent. EAX preserves recordId. Stock ARM ledgers contain 675
   records and 326 unique ids; flag-filtered stepping preserves the 32-bit registry key and does not imply gameplay
   class, tier, faction, or direction.
*/
byte __thandor_cf_preserve_eax_ecx_edx
ArmyAssetRegistry_HasIdWithoutFlag0200Cf(ArmyAssetId recordId)

{
  int registrySlotsRemaining;
  ArmyAssetRecordPrefix **registryCursor;
  ArmyAssetRecordPrefix *candidateAsset;
  
  registryCursor = g_ArmyAssetRecordRegistry;
  registrySlotsRemaining = 0x300;
  while (((candidateAsset = *registryCursor, candidateAsset == (ArmyAssetRecordPrefix *)0x0 ||
          (recordId != candidateAsset->registryId)) ||
         ((candidateAsset[1].selectionDetailTemplateVariantIndex & 0x200) != 0))) {
    registryCursor = registryCursor + 1;
    registrySlotsRemaining = registrySlotsRemaining + -1;
    if (registrySlotsRemaining == 0) {
      return 1;
    }
  }
  return 0;
}


/* Address: 0x00571B50.
   Ownership: assets/army/catalog.
   Purpose: Scans all 768 ArmyAsset registry pointers for recordId. CF clear requires flag 0x0200 set; CF set means
   absent. EAX preserves recordId. Stock ARM ledgers contain 675 records and 326 unique ids; flag-filtered stepping
   preserves the 32-bit registry key and does not imply gameplay class, tier, faction, or direction.
*/
byte __thandor_cf_preserve_eax_ecx_edx ArmyAssetRegistry_HasIdWithFlag0200Cf(ArmyAssetId recordId)

{
  int registrySlotsRemaining;
  ArmyAssetRecordPrefix **registryCursor;
  ArmyAssetRecordPrefix *candidateAsset;
  
  registryCursor = g_ArmyAssetRecordRegistry;
  registrySlotsRemaining = 0x300;
  while (((candidateAsset = *registryCursor, candidateAsset == (ArmyAssetRecordPrefix *)0x0 ||
          (recordId != candidateAsset->registryId)) ||
         ((candidateAsset[1].selectionDetailTemplateVariantIndex & 0x200) == 0))) {
    registryCursor = registryCursor + 1;
    registrySlotsRemaining = registrySlotsRemaining + -1;
    if (registrySlotsRemaining == 0) {
      return 1;
    }
  }
  return 0;
}


/* Address: 0x00571980.
   Ownership: assets/army/catalog.
   Purpose: Scans all 768 ArmyAsset registry pointers for recordId. CF clear requires flag 0x0100 set and flag
   0x0200 clear; CF set means absent. EAX preserves recordId. Stock ARM ledgers contain 675 records and 326 unique
   ids; flag-filtered stepping preserves the 32-bit registry key and does not imply gameplay class, tier, faction,
   or direction.
*/
byte __thandor_cf_preserve_eax_ecx_edx
ArmyAssetRegistry_HasIdWithFlag0100Without0200Cf(ArmyAssetId recordId)

{
  int registrySlotsRemaining;
  ArmyAssetRecordPrefix **registryCursor;
  ArmyAssetRecordPrefix *candidateAsset;
  
  registryCursor = g_ArmyAssetRecordRegistry;
  registrySlotsRemaining = 0x300;
  while ((((candidateAsset = *registryCursor, candidateAsset == (ArmyAssetRecordPrefix *)0x0 ||
           (recordId != candidateAsset->registryId)) ||
          ((candidateAsset[1].selectionDetailTemplateVariantIndex & 0x100) == 0)) ||
         ((candidateAsset[1].selectionDetailTemplateVariantIndex & 0x200) != 0))) {
    registryCursor = registryCursor + 1;
    registrySlotsRemaining = registrySlotsRemaining + -1;
    if (registrySlotsRemaining == 0) {
      return 1;
    }
  }
  return 0;
}


/* Address: 0x00571BC0.
   Ownership: assets/army/catalog.
   Purpose: Scans all 768 ArmyAsset registry pointers for recordId. CF clear requires both flags 0x0100 and 0x0200
   set; CF set means absent. EAX preserves recordId. Stock ARM ledgers contain 675 records and 326 unique ids;
   flag-filtered stepping preserves the 32-bit registry key and does not imply gameplay class, tier, faction, or
   direction.
*/
byte __thandor_cf_preserve_eax_ecx_edx
ArmyAssetRegistry_HasIdWithFlags0100And0200Cf(ArmyAssetId recordId)

{
  int registrySlotsRemaining;
  ArmyAssetRecordPrefix **registryCursor;
  ArmyAssetRecordPrefix *candidateAsset;
  
  registryCursor = g_ArmyAssetRecordRegistry;
  registrySlotsRemaining = 0x300;
  while ((((candidateAsset = *registryCursor, candidateAsset == (ArmyAssetRecordPrefix *)0x0 ||
           (recordId != candidateAsset->registryId)) ||
          ((candidateAsset[1].selectionDetailTemplateVariantIndex & 0x100) == 0)) ||
         ((candidateAsset[1].selectionDetailTemplateVariantIndex & 0x200) == 0))) {
    registryCursor = registryCursor + 1;
    registrySlotsRemaining = registrySlotsRemaining + -1;
    if (registrySlotsRemaining == 0) {
      return 1;
    }
  }
  return 0;
}


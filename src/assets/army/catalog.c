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
  bool idAbsent;
  ArmyRegistryIdEaxCf5_571ab0 searchResult;
  ArmyRegistryIdEaxCf5_5719f0 normalizedResult;
  
  idAbsent = (bool)ArmyAssetRegistry_HasIdWithFlag0100Without0200Cf(recordId);
  searchResult.carry = idAbsent;
  searchResult.eax = recordId;
  if (idAbsent) {
    searchResult = ArmyAssetRegistry_FindNextFlag0100Without0200WrappedCf(recordId);
  }
  normalizedResult.eax = searchResult.eax;
  normalizedResult.carry = searchResult.carry;
  return normalizedResult;
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
  ArmyAssetId baseId;
  bool broaderAbsent;
  ArmyRegistryIdEaxCf5_571a10 stepResult;
  
  while( true ) {
    baseId = recordId;
    stepResult.eax = baseId + 1;
    stepResult.carry = (bool)ArmyAssetRegistry_HasIdWithFlag0100Without0200Cf(stepResult.eax);
    if (!stepResult.carry) break;
    broaderAbsent = (bool)ArmyAssetRegistry_HasIdWithoutFlag0200Cf(stepResult.eax);
    recordId = stepResult.eax;
    if (broaderAbsent) {
      do {
        baseId = baseId - 1;
        broaderAbsent = (bool)ArmyAssetRegistry_HasIdWithoutFlag0200Cf(baseId);
        recordId = baseId;
      } while (!broaderAbsent);
    }
  }
  return stepResult;
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
  ArmyAssetId baseId;
  bool broaderAbsent;
  ArmyRegistryIdEaxCf5_571a60 stepResult;
  
  while( true ) {
    baseId = recordId;
    stepResult.eax = baseId - 1;
    stepResult.carry = (bool)ArmyAssetRegistry_HasIdWithFlag0100Without0200Cf(stepResult.eax);
    if (!stepResult.carry) break;
    broaderAbsent = (bool)ArmyAssetRegistry_HasIdWithoutFlag0200Cf(stepResult.eax);
    recordId = stepResult.eax;
    if (broaderAbsent) {
      do {
        baseId = baseId + 1;
        broaderAbsent = (bool)ArmyAssetRegistry_HasIdWithoutFlag0200Cf(baseId);
        recordId = baseId;
      } while (!broaderAbsent);
    }
  }
  return stepResult;
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
  bool broaderAbsent;
  ArmyRegistryIdEaxCf5_571b00 searchResult;
  
  for (;;) {
    broaderAbsent = (bool)ArmyAssetRegistry_HasIdWithoutFlag0200Cf(recordId);
    if (broaderAbsent) break;
    recordId = recordId - 1;
    if ((int)recordId < 0) {
      recordId = 0x1000;
      break;
    }
  }
  /* Scan backward for a qualified candidate, wrapping below zero to 0x1000. */
  while( true ) {
    searchResult.carry = (bool)ArmyAssetRegistry_HasIdWithFlag0100Without0200Cf(recordId);
    if (!searchResult.carry) break;
    recordId = recordId - 1;
    if ((int)recordId < 0) {
      recordId = 0x1000;
    }
  }
  searchResult.eax = recordId;
  return searchResult;
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
  bool idAbsent;
  ArmyRegistryIdEaxCf5_571cf0 searchResult;
  ArmyRegistryIdEaxCf5_571c30 normalizedResult;
  
  idAbsent = (bool)ArmyAssetRegistry_HasIdWithFlags0100And0200Cf(recordId);
  searchResult.carry = idAbsent;
  searchResult.eax = recordId;
  if (idAbsent) {
    searchResult = ArmyAssetRegistry_FindNextFlags0100And0200WrappedCf(recordId);
  }
  normalizedResult.eax = searchResult.eax;
  normalizedResult.carry = searchResult.carry;
  return normalizedResult;
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
  ArmyAssetId baseId;
  bool broaderAbsent;
  ArmyRegistryIdEaxCf5_571c50 stepResult;
  
  while( true ) {
    baseId = recordId;
    stepResult.eax = baseId + 1;
    stepResult.carry = (bool)ArmyAssetRegistry_HasIdWithFlags0100And0200Cf(stepResult.eax);
    if (!stepResult.carry) break;
    broaderAbsent = (bool)ArmyAssetRegistry_HasIdWithFlag0200Cf(stepResult.eax);
    recordId = stepResult.eax;
    if (broaderAbsent) {
      do {
        baseId = baseId - 1;
        broaderAbsent = (bool)ArmyAssetRegistry_HasIdWithFlag0200Cf(baseId);
        recordId = baseId;
      } while (!broaderAbsent);
    }
  }
  return stepResult;
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
  ArmyAssetId baseId;
  bool broaderAbsent;
  ArmyRegistryIdEaxCf5_571ca0 stepResult;
  
  while( true ) {
    baseId = recordId;
    stepResult.eax = baseId - 1;
    stepResult.carry = (bool)ArmyAssetRegistry_HasIdWithFlags0100And0200Cf(stepResult.eax);
    if (!stepResult.carry) break;
    broaderAbsent = (bool)ArmyAssetRegistry_HasIdWithFlag0200Cf(stepResult.eax);
    recordId = stepResult.eax;
    if (broaderAbsent) {
      do {
        baseId = baseId + 1;
        broaderAbsent = (bool)ArmyAssetRegistry_HasIdWithFlag0200Cf(baseId);
        recordId = baseId;
      } while (!broaderAbsent);
    }
  }
  return stepResult;
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
  bool broaderAbsent;
  ArmyRegistryIdEaxCf5_571d40 searchResult;
  
  for (;;) {
    broaderAbsent = (bool)ArmyAssetRegistry_HasIdWithFlag0200Cf(recordId);
    if (broaderAbsent) break;
    recordId = recordId - 1;
    if ((int)recordId < 0) {
      recordId = 0x1000;
      break;
    }
  }
  /* Scan backward for a qualified candidate, wrapping below zero to 0x1000. */
  while( true ) {
    searchResult.carry = (bool)ArmyAssetRegistry_HasIdWithFlags0100And0200Cf(recordId);
    if (!searchResult.carry) break;
    recordId = recordId - 1;
    if ((int)recordId < 0) {
      recordId = 0x1000;
    }
  }
  searchResult.eax = recordId;
  return searchResult;
}


/* Address: 0x0051B5E0.
   Ownership: assets/army/catalog.
   Purpose: Validates the 'arm' magic and converter version 0x00020008, then prepares recordCount variable-size
   records beginning at +0x200. Each successful record advances by its leading byteSize. Invalid headers update the
   package last-error path. CF and EAX status are preserved.
   Local calls: ArmyAssetRecord_RegisterAndRelocate.
   Cross-module calls: Package_SetLastErrorPath [assets/package/runtime].
*/
StatusResult __thandor_void_preserve_ecx_edx ArmyAsset_PrepareRecords(ArmyAssetHeader *asset)

{
  uint32_t registrationStatusCode;
  AssetRecordCount recordsRemaining;
  ArmyAssetHeader *record;
  StatusResult registrationStatus;
  StatusResult failureStatus;
  
  registrationStatusCode = 0x40;
  if (((asset->recordCountHeader).common.magic == ASSET_MAGIC_ARM) &&
     ((asset->recordCountHeader).common.converterVersion == PCK_CONVERTER_ARM_00020008)) {
    recordsRemaining = (asset->recordCountHeader).recordCount;
    record = asset + 1;
    while( true ) {
      if (recordsRemaining == 0) {
        registrationStatus.failed = false;
        registrationStatus.valueOrError = registrationStatusCode;
        return registrationStatus;
      }
      registrationStatus = ArmyAssetRecord_RegisterAndRelocate((ArmyAssetRuntimeSemanticView80 *)record,asset);
      registrationStatusCode = registrationStatus.valueOrError;
      if (registrationStatus.failed) break;
      record = (ArmyAssetHeader *)
               ((int)(record->recordCountHeader).common.buildMetadata.assetRelativeAddressAnchor28 +
               ((record->recordCountHeader).common.magic - 0x28));
      recordsRemaining = recordsRemaining - 1;
    }
  }
  else {
    Package_SetLastErrorPath((uint16_t *)asset);
  }
  failureStatus.failed = true;
  failureStatus.valueOrError = registrationStatusCode;
  return failureStatus;
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
  bool missingOrDisabled;
  ArmyRegistryEaxCf5_51b6d0 registryLookup;
  
  registryLookup = ArmyAssetRegistry_FindByIdCf(recordId);
  missingOrDisabled = registryLookup.carry;
  if (!missingOrDisabled) {
    missingOrDisabled = (registryLookup.eax[1].selectionDetailTemplateVariantIndex & 1) == 0;
  }
  return missingOrDisabled;
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
          (FactionRuntimeIndex factionIndex,uint32_t requiredDefinitionFlags,
          ArmyAssetRecordPrefix *armyAssetRecord)

{
  ArmyAssetRecordPrefix *definitionNode;
  int linksRemaining;
  bool technologyLocked;
  ArmyRegistryEaxCf5_51b6d0 registryLookup;
  
  linksRemaining = 0x10;
  do {
    if (armyAssetRecord[3].byteSize != 0) {
      registryLookup = ArmyAssetRegistry_FindByIdCf(armyAssetRecord[3].byteSize);
      definitionNode = registryLookup.eax;
      if ((!registryLookup.carry) && ((definitionNode[1].selectionDetailTemplateVariantIndex & 1) != 0)) {
        technologyLocked = ModelDefinitionHierarchy_AllTechnologyUnlockedForFactionCf
                          (factionIndex,(ModelDefinitionHierarchyNodeAddress32)definitionNode);
        if ((!technologyLocked) &&
           ((definitionNode[1].rootNodeOffsetOrPointer != 0 &&
            ((definitionNode[1].selectionDetailTemplateVariantIndex & requiredDefinitionFlags) != 0)
            ))) {
          return true;
        }
      }
    }
    armyAssetRecord = (ArmyAssetRecordPrefix *)&armyAssetRecord->selectionDetailTemplateVariantIndex
    ;
    linksRemaining = linksRemaining + -1;
    if (linksRemaining == 0) {
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
ArmyAssetRegistry_ClearPreviewTextureCacheAndRefreshSelected(uint32_t selectedArmyAssetRegistryId)

{
  uint32_t resolvedTexture;
  int registrySlotsRemaining;
  ArmyAssetRecordPrefix **registryCursor;
  ArmyAssetRecordPrefix *registeredRecord;
  
  registryCursor = g_ArmyAssetRecordRegistry;
  registrySlotsRemaining = 0x300;
  do {
    registeredRecord = *registryCursor;
    if (registeredRecord != (ArmyAssetRecordPrefix *)0x0) {
      (*g_MemoryApi.free)((void *)registeredRecord[2].byteSize);
      registeredRecord[2].byteSize = 0;
    }
    registryCursor = registryCursor + 1;
    registrySlotsRemaining = registrySlotsRemaining + -1;
  } while (registrySlotsRemaining != 0);
  resolvedTexture = ArmyAssetRegistry_ResolveOrCreatePreviewTextureCf(g_UiCommandModeGArmyAssetId);
  *(uint32_t *)(selectedArmyAssetRegistryId + 0x9d24) = resolvedTexture;
  resolvedTexture = ArmyAssetRegistry_ResolveOrCreatePreviewTextureCf(g_UiCommandMode4ArmyAssetId);
  *(uint32_t *)(selectedArmyAssetRegistryId + 0x9ddc) = resolvedTexture;
  return;
}


static uint32_t ArmyAssetHierarchy_SumArmourFrom(FactionRuntimeIndex factionIndex,uint8_t *node)
{
  ModelDefinitionResult selected;
  uint32_t sum;
  uint32_t i;
  selected = ModelDefinition_SelectFactionUnlockedLinkedDefinitionCf
                       (factionIndex,(ModelLinkedDefinitionListAddress32)(uintptr_t)node);
  sum = *(uint32_t *)((uint8_t *)selected.modelDefinition + 0x60);
  for (i = 0; i < *(uint32_t *)(node + 8); i++) {
    uint8_t *child = *(uint8_t **)(node + 0xc + i * 4);
    if (child != (uint8_t *)0x0) {
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
uint32_t __thandor_eax_preserve_ecx_edx
ArmyAssetHierarchy_SumFactionUnlockedArmour
          (FactionRuntimeIndex factionIndex,ModelDefinitionHierarchyNodeAddress32 definitionNode)

{
  /* Rewritten from the assembly: the original walks the definition tree (child count at +0x08,
     children at +0x0C + 4*i) depth-first with frames on the machine stack. */
  return ArmyAssetHierarchy_SumArmourFrom(factionIndex,*(uint8_t **)(uintptr_t)(definitionNode + 0xc));
}


static EnergyDemandQ4 ArmyAssetHierarchy_SumEnergyFrom(FactionRuntimeIndex factionIndex,uint8_t *node)
{
  ModelDefinitionResult selected;
  EnergyDemandQ4 sum;
  uint32_t childCount;
  uint32_t i;
  selected = ModelDefinition_SelectFactionUnlockedLinkedDefinitionCf
                       (factionIndex,(ModelLinkedDefinitionListAddress32)(uintptr_t)node);
  sum = *(EnergyDemandQ4 *)((uint8_t *)selected.modelDefinition + 0x18c);
  childCount = *(uint32_t *)(node + 8);
  if ((*(uint32_t *)((uint8_t *)selected.modelDefinition + 0x68) & 0x80) == 0) {
    childCount = 0; /* only definitions with flag 0x80 contribute their children */
  }
  for (i = 0; i < childCount; i++) {
    sum = sum + ArmyAssetHierarchy_SumEnergyFrom(factionIndex,*(uint8_t **)(node + 0xc + i * 4));
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
  return ArmyAssetHierarchy_SumEnergyFrom(factionIndex,*(uint8_t **)(uintptr_t)(definitionNode + 0xc));
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
  bool broaderAbsent;
  ArmyRegistryIdEaxCf5_571ab0 searchResult;
  
  while( true ) {
    broaderAbsent = (bool)ArmyAssetRegistry_HasIdWithoutFlag0200Cf(recordId);
    if (broaderAbsent) break;
    recordId = recordId + 1;
  }
  while( true ) {
    searchResult.carry = (bool)ArmyAssetRegistry_HasIdWithFlag0100Without0200Cf(recordId);
    if (!searchResult.carry) break;
    recordId = recordId + 1;
    if (0xfff < recordId) {
      recordId = 0;
    }
  }
  searchResult.eax = recordId;
  return searchResult;
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
  bool broaderAbsent;
  ArmyRegistryIdEaxCf5_571cf0 searchResult;
  
  while( true ) {
    broaderAbsent = (bool)ArmyAssetRegistry_HasIdWithFlag0200Cf(recordId);
    if (broaderAbsent) break;
    recordId = recordId + 1;
  }
  while( true ) {
    searchResult.carry = (bool)ArmyAssetRegistry_HasIdWithFlags0100And0200Cf(recordId);
    if (!searchResult.carry) break;
    recordId = recordId + 1;
    if (0xfff < recordId) {
      recordId = 0;
    }
  }
  searchResult.eax = recordId;
  return searchResult;
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
static uint32_t ArmyAssetRecord_RelocateModelTree
          (ArmyAssetRuntimeSemanticView80 *record,uint8_t *assetBase,uint8_t *node)
{
  BuildMetricResult metrics;
  uint32_t childCount;
  uint32_t childIndex;
  uint32_t error = 0;
  uint32_t childError;

  metrics = ModelDefinitionRegistry_FindBuildMetricTupleByIdCf(*(PckModelDefinitionIdCatalog *)(node + 0x20));
  if (metrics.notFound) {
    error = (uint32_t)metrics.metric0;
  }
  else {
    record->relocationPointerOrOffset2C = record->relocationPointerOrOffset2C + (uint32_t)metrics.metric0;
    record->relocationValue24 = record->relocationValue24 + metrics.metric1;
    record->relocationValue28 = record->relocationValue28 + metrics.metric2;
  }
  childCount = *(uint32_t *)(node + 8);
  for (childIndex = 0; childIndex < childCount; childIndex = childIndex + 1) {
    uint8_t **child = (uint8_t **)(node + 0xc + childIndex * 4);
    *child = *child + (uintptr_t)assetBase;
    childError = ArmyAssetRecord_RelocateModelTree(record,assetBase,*child);
    if (childError != 0) {
      error = childError;
    }
  }
  return error;
}

StatusResult __thandor_void_preserve_ecx_edx
ArmyAssetRecord_RegisterAndRelocate
          (ArmyAssetRuntimeSemanticView80 *record,ArmyAssetHeader *assetBase)

{
  /* Rewritten from the assembly (0x0051B4A0-0x0051B5D8): the model tree walk kept its recursion on
     the machine stack, which the decompiler could not express. */
  ArmyAssetRecordPrefix **slot;
  int slotsRemaining;
  ArmyRegistryEaxCf5_51b6d0 existing;
  StatusResult status;

  existing = ArmyAssetRegistry_FindByIdCf(record->registryId);
  if (!existing.carry) {
    (*g_WideNumberFormatUtf16)
              (WIDE_FORMAT_WRITE_TERMINATOR,0,10,1,record->registryId,g_PackageLastErrorPath);
    status.failed = true;
    status.valueOrError = 0x4c;
    return status;
  }
  slot = g_ArmyAssetRecordRegistry;
  for (slotsRemaining = 0x300; slotsRemaining != 0; slotsRemaining = slotsRemaining + -1) {
    if (*slot == (ArmyAssetRecordPrefix *)0x0) {
      uint32_t error = 0;
      *slot = (ArmyAssetRecordPrefix *)record;
      if (record->rootNodeOffsetOrPointer != 0) {
        record->rootNodeOffsetOrPointer = record->rootNodeOffsetOrPointer + (uint32_t)(uintptr_t)assetBase;
        error = ArmyAssetRecord_RelocateModelTree
                          (record,(uint8_t *)assetBase,(uint8_t *)(uintptr_t)record->rootNodeOffsetOrPointer);
      }
      status.failed = error != 0;
      status.valueOrError = error;
      return status;
    }
    slot = slot + 1;
  }
  (*g_WideNumberFormatUtf16)(WIDE_FORMAT_WRITE_TERMINATOR,0,10,1,0x300,g_PackageLastErrorPath);
  status.failed = true;
  status.valueOrError = 0x42;
  return status;
}


/* Address: 0x00571D90.
   Ownership: assets/army/catalog.
   Purpose: Looks up an ArmyAsset registry record by id and returns its cached preview texture; if absent it
   creates and caches the preview through ArmyRuntime_RenderPreviewTextureCf.
   Cross-module calls: ArmyRuntime_RenderPreviewTextureCf [gameplay/army/runtime].
*/
uint32_t ArmyAssetRegistry_ResolveOrCreatePreviewTextureCf(uint32_t armyAssetRegistryId)

{
  ArmyAssetRecordPrefix *registeredRecord;
  int registrySlotsRemaining;
  ArmyAssetRecordPrefix **registryCursor;
  ArmyPreviewTextureResult renderResult;
  FactionRuntimeIndex factionIndex;
  
  registryCursor = g_ArmyAssetRecordRegistry;
  registrySlotsRemaining = 0x300;
  while ((registeredRecord = *registryCursor, registeredRecord == (ArmyAssetRecordPrefix *)0x0 ||
         (armyAssetRegistryId != registeredRecord->registryId))) {
    registryCursor = registryCursor + 1;
    registrySlotsRemaining = registrySlotsRemaining + -1;
    if (registrySlotsRemaining == 0) {
      return 0;
    }
  }
  if (registeredRecord[2].byteSize == 0) {
    factionIndex = g_UiCommandModeGOwnerFactionIndex;
    if (399 < armyAssetRegistryId) {
      factionIndex = 0;
    }
    renderResult = ArmyRuntime_RenderPreviewTextureCf
                      (*(GraphicsPixelDimension *)(g_InGameRuntimeRoot->opaque9A74_9B4B + 0x5c),
                       *(GraphicsPixelDimension *)(g_InGameRuntimeRoot->opaque9A74_9B4B + 0x5c),
                       factionIndex,armyAssetRegistryId,&g_InGameRuntimeRoot->worldRuntime0A30);
    if (renderResult.failed) {
      return 0;
    }
    registeredRecord[2].byteSize = (AssetRecordByteCount)renderResult.previewTexture;
    return (uint32_t)renderResult.previewTexture;
  }
  return registeredRecord[2].byteSize;
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
  ArmyAssetRecordPrefix *matchedRecord;
  int registrySlotsRemaining;
  ArmyAssetRecordPrefix **registryCursor;
  ArmyRegistryEaxCf5_51b6d0 failureResult;
  ArmyRegistryEaxCf5_51b6d0 successResult;
  ArmyAssetRecordPrefix *candidateAsset;
  
  registryCursor = g_ArmyAssetRecordRegistry;
  registrySlotsRemaining = 0x300;
  while ((matchedRecord = *registryCursor, matchedRecord == (ArmyAssetRecordPrefix *)0x0 ||
         (matchedRecord->registryId != registryId))) {
    registryCursor = registryCursor + 1;
    registrySlotsRemaining = registrySlotsRemaining + -1;
    if (registrySlotsRemaining == 0) {
      (*g_WideNumberFormatUtf16)
                (WIDE_FORMAT_WRITE_TERMINATOR,0,10,1,registryId,g_PackageLastErrorPath);
      failureResult.carry = true;
      failureResult.eax = (ArmyAssetRecordPrefix *)0x41;
      return failureResult;
    }
  }
  successResult.carry = false;
  successResult.eax = matchedRecord;
  return successResult;
}


/* Address: 0x00571910.
   Ownership: assets/army/catalog.
   Purpose: Scans all 768 ArmyAsset registry pointers for recordId. CF clear means a matching record was found with
   flags dword +0x14 bit 0x0200 clear; CF set means absent. EAX preserves recordId. Stock ARM ledgers contain 675
   records and 326 unique ids; flag-filtered stepping preserves the 32-bit registry key and does not imply gameplay
   class, tier, faction, or direction.
*/
uint8_t __thandor_cf_preserve_eax_ecx_edx
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
uint8_t __thandor_cf_preserve_eax_ecx_edx ArmyAssetRegistry_HasIdWithFlag0200Cf(ArmyAssetId recordId)

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
uint8_t __thandor_cf_preserve_eax_ecx_edx
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
uint8_t __thandor_cf_preserve_eax_ecx_edx
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


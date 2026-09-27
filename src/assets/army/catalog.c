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
   Local calls: ArmyAssetRegistry_HasIdWithFlag0100Without0200,
   ArmyAssetRegistry_FindNextFlag0100Without0200Wrapped.
*/
ArmyAssetIdSearchResult __thandor_eax_cf_preserve_ecx_edx
ArmyAssetRegistry_NormalizeIdForFlag0100Without0200(PckArmyAssetIdCatalog recordId)

{
  bool idAbsent;
  ArmyAssetIdSearchResult searchResult;
  ArmyAssetIdSearchResult normalizedResult;
  
  idAbsent = (bool)ArmyAssetRegistry_HasIdWithFlag0100Without0200(recordId);
  searchResult.notFound = idAbsent;
  searchResult.armyAssetId = recordId;
  if (idAbsent) {
    searchResult = ArmyAssetRegistry_FindNextFlag0100Without0200Wrapped(recordId);
  }
  normalizedResult.armyAssetId = searchResult.armyAssetId;
  normalizedResult.notFound = searchResult.notFound;
  return normalizedResult;
}


/* Address: 0x00571A10.
   Ownership: assets/army/catalog.
   Purpose: Steps forward through IDs using the exact desired predicate and the broader flag-0x0200-clear
   predicate, with the verified reverse-gap fallback. EAX and CF remain intact. Stock ARM ledgers contain 675
   records and 326 unique ids; flag-filtered stepping preserves the 32-bit registry key and does not imply gameplay
   class, tier, faction, or direction.
   Local calls: ArmyAssetRegistry_HasIdWithFlag0100Without0200, ArmyAssetRegistry_HasIdWithoutFlag0200.
*/
ArmyAssetIdSearchResult __thandor_eax_cf_preserve_ecx_edx
ArmyAssetRegistry_StepForwardFlag0100Without0200(ArmyAssetId recordId)

{
  ArmyAssetId baseId;
  bool broaderAbsent;
  ArmyAssetIdSearchResult stepResult;
  
  while( true ) {
    baseId = recordId;
    stepResult.armyAssetId = baseId + 1;
    stepResult.notFound = (bool)ArmyAssetRegistry_HasIdWithFlag0100Without0200(stepResult.armyAssetId);
    if (!stepResult.notFound) break;
    broaderAbsent = (bool)ArmyAssetRegistry_HasIdWithoutFlag0200(stepResult.armyAssetId);
    recordId = stepResult.armyAssetId;
    if (broaderAbsent) {
      do {
        baseId = baseId - 1;
        broaderAbsent = (bool)ArmyAssetRegistry_HasIdWithoutFlag0200(baseId);
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
   Local calls: ArmyAssetRegistry_HasIdWithFlag0100Without0200, ArmyAssetRegistry_HasIdWithoutFlag0200.
*/
ArmyAssetIdSearchResult __thandor_eax_cf_preserve_ecx_edx
ArmyAssetRegistry_StepBackwardFlag0100Without0200(ArmyAssetId recordId)

{
  ArmyAssetId baseId;
  bool broaderAbsent;
  ArmyAssetIdSearchResult stepResult;
  
  while( true ) {
    baseId = recordId;
    stepResult.armyAssetId = baseId - 1;
    stepResult.notFound = (bool)ArmyAssetRegistry_HasIdWithFlag0100Without0200(stepResult.armyAssetId);
    if (!stepResult.notFound) break;
    broaderAbsent = (bool)ArmyAssetRegistry_HasIdWithoutFlag0200(stepResult.armyAssetId);
    recordId = stepResult.armyAssetId;
    if (broaderAbsent) {
      do {
        baseId = baseId + 1;
        broaderAbsent = (bool)ArmyAssetRegistry_HasIdWithoutFlag0200(baseId);
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
   Local calls: ArmyAssetRegistry_HasIdWithoutFlag0200, ArmyAssetRegistry_HasIdWithFlag0100Without0200.
*/
ArmyAssetIdSearchResult __thandor_eax_cf_preserve_ecx_edx
ArmyAssetRegistry_FindPreviousFlag0100Without0200Wrapped(ArmyAssetId recordId)

{
  bool broaderAbsent;
  ArmyAssetIdSearchResult searchResult;
  
  for (;;) {
    broaderAbsent = (bool)ArmyAssetRegistry_HasIdWithoutFlag0200(recordId);
    if (broaderAbsent) break;
    recordId = recordId - 1;
    if ((int)recordId < 0) {
      recordId = 0x1000;
      break;
    }
  }
  /* Scan backward for a qualified candidate, wrapping below zero to 0x1000. */
  while( true ) {
    searchResult.notFound = (bool)ArmyAssetRegistry_HasIdWithFlag0100Without0200(recordId);
    if (!searchResult.notFound) break;
    recordId = recordId - 1;
    if ((int)recordId < 0) {
      recordId = 0x1000;
    }
  }
  searchResult.armyAssetId = recordId;
  return searchResult;
}


/* Address: 0x00571C30.
   Ownership: assets/army/catalog.
   Purpose: Tests recordId against the exact flags-0x0100-and-0x0200-set predicate. When absent, it forwards the
   current EAX ID into the wrapped-next search. EAX and CF remain the result channel. Stock ARM ledgers contain 675
   records and 326 unique ids; flag-filtered stepping preserves the 32-bit registry key and does not imply gameplay
   class, tier, faction, or direction.
   Local calls: ArmyAssetRegistry_HasIdWithFlags0100And0200, ArmyAssetRegistry_FindNextFlags0100And0200Wrapped.
*/
ArmyAssetIdSearchResult __thandor_eax_cf_preserve_ecx_edx
ArmyAssetRegistry_NormalizeIdForFlags0100And0200(PckArmyAssetIdCatalog recordId)

{
  bool idAbsent;
  ArmyAssetIdSearchResult searchResult;
  ArmyAssetIdSearchResult normalizedResult;
  
  idAbsent = (bool)ArmyAssetRegistry_HasIdWithFlags0100And0200(recordId);
  searchResult.notFound = idAbsent;
  searchResult.armyAssetId = recordId;
  if (idAbsent) {
    searchResult = ArmyAssetRegistry_FindNextFlags0100And0200Wrapped(recordId);
  }
  normalizedResult.armyAssetId = searchResult.armyAssetId;
  normalizedResult.notFound = searchResult.notFound;
  return normalizedResult;
}


/* Address: 0x00571C50.
   Ownership: assets/army/catalog.
   Purpose: Steps forward through IDs using the exact combined-flags predicate and the broader flag-0x0200-set
   predicate, with the verified reverse-gap fallback. EAX and CF remain intact. Stock ARM ledgers contain 675
   records and 326 unique ids; flag-filtered stepping preserves the 32-bit registry key and does not imply gameplay
   class, tier, faction, or direction.
   Local calls: ArmyAssetRegistry_HasIdWithFlags0100And0200, ArmyAssetRegistry_HasIdWithFlag0200.
*/
ArmyAssetIdSearchResult __thandor_eax_cf_preserve_ecx_edx
ArmyAssetRegistry_StepForwardFlags0100And0200(ArmyAssetId recordId)

{
  ArmyAssetId baseId;
  bool broaderAbsent;
  ArmyAssetIdSearchResult stepResult;
  
  while( true ) {
    baseId = recordId;
    stepResult.armyAssetId = baseId + 1;
    stepResult.notFound = (bool)ArmyAssetRegistry_HasIdWithFlags0100And0200(stepResult.armyAssetId);
    if (!stepResult.notFound) break;
    broaderAbsent = (bool)ArmyAssetRegistry_HasIdWithFlag0200(stepResult.armyAssetId);
    recordId = stepResult.armyAssetId;
    if (broaderAbsent) {
      do {
        baseId = baseId - 1;
        broaderAbsent = (bool)ArmyAssetRegistry_HasIdWithFlag0200(baseId);
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
   Local calls: ArmyAssetRegistry_HasIdWithFlags0100And0200, ArmyAssetRegistry_HasIdWithFlag0200.
*/
ArmyAssetIdSearchResult __thandor_eax_cf_preserve_ecx_edx
ArmyAssetRegistry_StepBackwardFlags0100And0200(ArmyAssetId recordId)

{
  ArmyAssetId baseId;
  bool broaderAbsent;
  ArmyAssetIdSearchResult stepResult;
  
  while( true ) {
    baseId = recordId;
    stepResult.armyAssetId = baseId - 1;
    stepResult.notFound = (bool)ArmyAssetRegistry_HasIdWithFlags0100And0200(stepResult.armyAssetId);
    if (!stepResult.notFound) break;
    broaderAbsent = (bool)ArmyAssetRegistry_HasIdWithFlag0200(stepResult.armyAssetId);
    recordId = stepResult.armyAssetId;
    if (broaderAbsent) {
      do {
        baseId = baseId + 1;
        broaderAbsent = (bool)ArmyAssetRegistry_HasIdWithFlag0200(baseId);
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
   Local calls: ArmyAssetRegistry_HasIdWithFlag0200, ArmyAssetRegistry_HasIdWithFlags0100And0200.
*/
ArmyAssetIdSearchResult __thandor_eax_cf_preserve_ecx_edx
ArmyAssetRegistry_FindPreviousFlags0100And0200Wrapped(ArmyAssetId recordId)

{
  bool broaderAbsent;
  ArmyAssetIdSearchResult searchResult;
  
  for (;;) {
    broaderAbsent = (bool)ArmyAssetRegistry_HasIdWithFlag0200(recordId);
    if (broaderAbsent) break;
    recordId = recordId - 1;
    if ((int)recordId < 0) {
      recordId = 0x1000;
      break;
    }
  }
  /* Scan backward for a qualified candidate, wrapping below zero to 0x1000. */
  while( true ) {
    searchResult.notFound = (bool)ArmyAssetRegistry_HasIdWithFlags0100And0200(recordId);
    if (!searchResult.notFound) break;
    recordId = recordId - 1;
    if ((int)recordId < 0) {
      recordId = 0x1000;
    }
  }
  searchResult.armyAssetId = recordId;
  return searchResult;
}


/* Address: 0x0051B5E0.
   Checks that a loaded asset is an 'arm' file of converter version 0x20008 and registers every army record
   in it (the variable-size records follow the 0x200-byte header, each starting with its byte size). A wrong
   header stores the asset path as the error detail and fails with FATAL_ERROR_ARMY_ASSET_INVALID; a failed
   registration fails with that step's error code.
*/
StatusResult __thandor_void_preserve_ecx_edx ArmyAsset_PrepareRecords(ArmyAssetHeader *asset)

{
  uint32_t registrationStatusCode;
  AssetRecordCount recordsRemaining;
  ArmyAssetHeader *record;
  StatusResult registrationStatus;
  StatusResult failureStatus;

  registrationStatusCode = FATAL_ERROR_ARMY_ASSET_INVALID;
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
      /* record += its byte size: the anchor array at +0x28 decays to record + 0x28, and the dword at +0
         (typed as the magic here) is the record's byte size */
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
   Checks whether an army asset id is registered and enabled: returns false (CF clear) only when the record
   exists and bit 0 of its flags dword (+0x14) is set, true when it is missing or disabled.
*/
bool __thandor_cf_preserve_eax_ecx_edx
ArmyAssetRegistry_FindEnabledById(PckArmyAssetIdCatalog recordId)

{
  bool missingOrDisabled;
  ArmyAssetLookupResult registryLookup;

  registryLookup = ArmyAssetRegistry_FindById(recordId);
  missingOrDisabled = registryLookup.notFound;
  if (!missingOrDisabled) {
    /* [1].selectionDetailTemplateVariantIndex is the flags dword +0x14; bit 0 = enabled */
    missingOrDisabled = (registryLookup.recordOrError[1].selectionDetailTemplateVariantIndex & 1) == 0;
  }
  return missingOrDisabled;
}


/* Address: 0x0051B770.
   Checks the 16 army-asset ids linked from an army record (dwords at +0x30) and returns true (CF set) as soon as
   one names a registered, enabled asset whose technology is fully unlocked for the faction, that has a model
   tree (+0x1C) and whose flags (+0x14) share a bit with requiredDefinitionFlags.
*/
bool __thandor_cf_preserve_eax_ecx_edx
ArmyAssetRecord_HasFactionUnlockedLinkedDefinition
          (FactionRuntimeIndex factionIndex,uint32_t requiredDefinitionFlags,
          ArmyAssetRecordPrefix *armyAssetRecord)

{
  ArmyAssetRecordPrefix *linkedAsset;
  int linksRemaining;
  bool technologyLocked;
  ArmyAssetLookupResult registryLookup;

  /* armyAssetRecord walks the link list in 4-byte steps, so [3].byteSize is the current link at +0x30;
     on the linked asset [1].selectionDetailTemplateVariantIndex is the flags dword +0x14 (bit 0 = enabled)
     and [1].rootNodeOffsetOrPointer the model-tree pointer +0x1C */
  linksRemaining = ARMY_ASSET_LINKED_ID_COUNT;
  do {
    if (armyAssetRecord[3].byteSize != 0) {
      registryLookup = ArmyAssetRegistry_FindById(armyAssetRecord[3].byteSize);
      linkedAsset = registryLookup.recordOrError;
      if ((!registryLookup.notFound) && ((linkedAsset[1].selectionDetailTemplateVariantIndex & 1) != 0)) {
        technologyLocked = ModelDefinitionHierarchy_AllTechnologyUnlockedForFaction
                          (factionIndex,(ModelDefinitionHierarchyNodeAddress32)linkedAsset);
        if ((!technologyLocked) &&
           ((linkedAsset[1].rootNodeOffsetOrPointer != 0 &&
            ((linkedAsset[1].selectionDetailTemplateVariantIndex & requiredDefinitionFlags) != 0)
            ))) {
          return true;
        }
      }
    }
    armyAssetRecord = (ArmyAssetRecordPrefix *)&armyAssetRecord->selectionDetailTemplateVariantIndex;
    linksRemaining--;
    if (linksRemaining == 0) {
      return false;
    }
  } while( true );
}


/* Address: 0x00571E40.
   Ownership: assets/army/catalog.
   Purpose: Clears cached ArmyAsset preview textures, frees existing cache entries, and refreshes the two selected
   preview resources.
   Local calls: ArmyAssetRegistry_ResolveOrCreatePreviewTexture.
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
      g_MemoryApi.free((void *)registeredRecord[2].byteSize);
      registeredRecord[2].byteSize = 0;
    }
    registryCursor = registryCursor + 1;
    registrySlotsRemaining = registrySlotsRemaining + -1;
  } while (registrySlotsRemaining != 0);
  resolvedTexture = ArmyAssetRegistry_ResolveOrCreatePreviewTexture(g_UiCommandModeGArmyAssetId);
  *(uint32_t *)(selectedArmyAssetRegistryId + 0x9d24) = resolvedTexture;
  resolvedTexture = ArmyAssetRegistry_ResolveOrCreatePreviewTexture(g_UiCommandMode4ArmyAssetId);
  *(uint32_t *)(selectedArmyAssetRegistryId + 0x9ddc) = resolvedTexture;
  return;
}


/* Armour of one model-tree node (dword +0x60 of the definition the faction has unlocked for it) plus that of
   all its children (count at +0x08, pointers from +0x0C). */
static uint32_t ArmyAssetHierarchy_SumArmourFrom(FactionRuntimeIndex factionIndex,uint8_t *node)
{
  ModelDefinitionResult selected;
  uint32_t armourSum;
  uint32_t childIndex;
  selected = ModelDefinition_SelectFactionUnlockedLinkedDefinition
                       (factionIndex,(ModelLinkedDefinitionListAddress32)(uintptr_t)node);
  armourSum = *(uint32_t *)((uint8_t *)selected.modelDefinition + 0x60);
  for (childIndex = 0; childIndex < *(uint32_t *)(node + 8); childIndex++) {
    uint8_t *child = *(uint8_t **)(node + 0xc + childIndex * 4);
    if (child != NULL) {
      armourSum = armourSum + ArmyAssetHierarchy_SumArmourFrom(factionIndex,child);
    }
  }
  return armourSum;
}

/* Address: 0x0051C170.
   Sums the armour (model-definition dword +0x60) over an army record's whole model tree, taking at each node
   the linked definition the faction has unlocked, so the in-game detail display shows the armour of the
   current upgrades.
*/
uint32_t __thandor_eax_preserve_ecx_edx
ArmyAssetHierarchy_SumFactionUnlockedArmour
          (FactionRuntimeIndex factionIndex,ModelDefinitionHierarchyNodeAddress32 definitionNode)

{
  /* Rewritten from the assembly: the original walks the definition tree (child count at +0x08,
     children at +0x0C + 4*i) depth-first with frames on the machine stack. */
  return ArmyAssetHierarchy_SumArmourFrom(factionIndex,*(uint8_t **)(uintptr_t)(definitionNode + 0xc));
}


/* Displayed energy (Q4 dword +0x18C) of the definition the faction has unlocked for one model-tree node, plus
   that of its children when the definition's flags (+0x68) have bit 0x80 set. */
static EnergyDemandQ4 ArmyAssetHierarchy_SumEnergyFrom(FactionRuntimeIndex factionIndex,uint8_t *node)
{
  ModelDefinitionResult selected;
  EnergyDemandQ4 energySum;
  uint32_t childCount;
  uint32_t childIndex;
  selected = ModelDefinition_SelectFactionUnlockedLinkedDefinition
                       (factionIndex,(ModelLinkedDefinitionListAddress32)(uintptr_t)node);
  energySum = *(EnergyDemandQ4 *)((uint8_t *)selected.modelDefinition + 0x18c);
  childCount = *(uint32_t *)(node + 8);
  if ((*(uint32_t *)((uint8_t *)selected.modelDefinition + 0x68) & 0x80) == 0) {
    childCount = 0; /* only definitions with flag 0x80 contribute their children */
  }
  for (childIndex = 0; childIndex < childCount; childIndex++) {
    energySum = energySum + ArmyAssetHierarchy_SumEnergyFrom(factionIndex,*(uint8_t **)(node + 0xc + childIndex * 4));
  }
  return energySum;
}

/* Address: 0x0051C2C0.
   Sums the displayed energy value (model-definition Q4 dword +0x18C) over an army record's model tree, taking
   at each node the linked definition the faction has unlocked; children only count below definitions with
   flag 0x80. Used by the in-game detail display.
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
   Local calls: ArmyAssetRegistry_HasIdWithoutFlag0200, ArmyAssetRegistry_HasIdWithFlag0100Without0200.
*/
ArmyAssetIdSearchResult __thandor_eax_cf_preserve_ecx_edx
ArmyAssetRegistry_FindNextFlag0100Without0200Wrapped(ArmyAssetId recordId)

{
  bool broaderAbsent;
  ArmyAssetIdSearchResult searchResult;
  
  while( true ) {
    broaderAbsent = (bool)ArmyAssetRegistry_HasIdWithoutFlag0200(recordId);
    if (broaderAbsent) break;
    recordId = recordId + 1;
  }
  while( true ) {
    searchResult.notFound = (bool)ArmyAssetRegistry_HasIdWithFlag0100Without0200(recordId);
    if (!searchResult.notFound) break;
    recordId = recordId + 1;
    if (0xfff < recordId) {
      recordId = 0;
    }
  }
  searchResult.armyAssetId = recordId;
  return searchResult;
}


/* Address: 0x00571CF0.
   Ownership: assets/army/catalog.
   Purpose: Finds the next ID with flags 0x0100 and 0x0200 set, first advancing to a broader flag-0x0200 record and
   then wrapping the desired search at 0x1000. EAX and CF remain intact. Stock ARM ledgers contain 675 records and
   326 unique ids; flag-filtered stepping preserves the 32-bit registry key and does not imply gameplay class,
   tier, faction, or direction.
   Local calls: ArmyAssetRegistry_HasIdWithFlag0200, ArmyAssetRegistry_HasIdWithFlags0100And0200.
*/
ArmyAssetIdSearchResult __thandor_eax_cf_preserve_ecx_edx
ArmyAssetRegistry_FindNextFlags0100And0200Wrapped(ArmyAssetId recordId)

{
  bool broaderAbsent;
  ArmyAssetIdSearchResult searchResult;
  
  while( true ) {
    broaderAbsent = (bool)ArmyAssetRegistry_HasIdWithFlag0200(recordId);
    if (broaderAbsent) break;
    recordId = recordId + 1;
  }
  while( true ) {
    searchResult.notFound = (bool)ArmyAssetRegistry_HasIdWithFlags0100And0200(recordId);
    if (!searchResult.notFound) break;
    recordId = recordId + 1;
    if (0xfff < recordId) {
      recordId = 0;
    }
  }
  searchResult.armyAssetId = recordId;
  return searchResult;
}


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

  metrics = ModelDefinitionRegistry_FindBuildMetricTupleById(*(PckModelDefinitionIdCatalog *)(node + 0x20));
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

/* Address: 0x0051B4A0.
   Registers a loaded army record: rejects an id that is already registered (FATAL_ERROR_ARMY_ID_DUPLICATE), puts
   the record into the first free slot of the 768-slot army registry (FATAL_ERROR_ARMY_REGISTRY_FULL when none is
   left), turns its model-tree offsets into pointers against assetBase and adds the build metrics of every node's
   model definition to the record. CF set on failure with the error code in EAX.
*/
StatusResult __thandor_void_preserve_ecx_edx
ArmyAssetRecord_RegisterAndRelocate
          (ArmyAssetRuntimeSemanticView80 *record,ArmyAssetHeader *assetBase)

{
  /* Rewritten from the assembly (0x0051B4A0-0x0051B5D8): the model tree walk kept its recursion on
     the machine stack, which the decompiler could not express. */
  ArmyAssetRecordPrefix **slot;
  int slotsRemaining;
  ArmyAssetLookupResult existing;
  StatusResult status;

  existing = ArmyAssetRegistry_FindById(record->registryId);
  if (!existing.notFound) {
    g_WideNumberFormatUtf16
              (WIDE_FORMAT_WRITE_TERMINATOR,0,10,1,record->registryId,g_PackageLastErrorPath);
    status.failed = true;
    status.valueOrError = FATAL_ERROR_ARMY_ID_DUPLICATE;
    return status;
  }
  slot = g_ArmyAssetRecordRegistry;
  for (slotsRemaining = ARMY_ASSET_REGISTRY_SLOT_COUNT; slotsRemaining != 0; slotsRemaining--) {
    if (*slot == NULL) {
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
  g_WideNumberFormatUtf16(WIDE_FORMAT_WRITE_TERMINATOR,0,10,1,ARMY_ASSET_REGISTRY_SLOT_COUNT,g_PackageLastErrorPath);
  status.failed = true;
  status.valueOrError = FATAL_ERROR_ARMY_REGISTRY_FULL;
  return status;
}


/* Address: 0x00571D90.
   Ownership: assets/army/catalog.
   Purpose: Looks up an ArmyAsset registry record by id and returns its cached preview texture; if absent it
   creates and caches the preview through ArmyRuntime_RenderPreviewTexture.
   Cross-module calls: ArmyRuntime_RenderPreviewTexture [gameplay/army/runtime].
*/
uint32_t ArmyAssetRegistry_ResolveOrCreatePreviewTexture(uint32_t armyAssetRegistryId)

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
    renderResult = ArmyRuntime_RenderPreviewTexture
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
   Looks an army asset up by its registry id in the 768-slot army registry and returns the record (CF clear).
   An unknown id is written as decimal text to g_PackageLastErrorPath and fails with FATAL_ERROR_ARMY_ID_NOT_FOUND.
*/
ArmyAssetLookupResult __thandor_eax_cf_preserve_ecx_edx
ArmyAssetRegistry_FindById(PckArmyAssetIdCatalog registryId)

{
  ArmyAssetRecordPrefix *matchedRecord;
  int registrySlotsRemaining;
  ArmyAssetRecordPrefix **registryCursor;
  ArmyAssetLookupResult failureResult;
  ArmyAssetLookupResult successResult;

  registryCursor = g_ArmyAssetRecordRegistry;
  registrySlotsRemaining = ARMY_ASSET_REGISTRY_SLOT_COUNT;
  while ((matchedRecord = *registryCursor, matchedRecord == NULL ||
         (matchedRecord->registryId != registryId))) {
    registryCursor = registryCursor + 1;
    registrySlotsRemaining--;
    if (registrySlotsRemaining == 0) {
      g_WideNumberFormatUtf16
                (WIDE_FORMAT_WRITE_TERMINATOR,0,10,1,registryId,g_PackageLastErrorPath);
      failureResult.notFound = true;
      failureResult.recordOrError = (ArmyAssetRecordPrefix *)FATAL_ERROR_ARMY_ID_NOT_FOUND;
      return failureResult;
    }
  }
  successResult.notFound = false;
  successResult.recordOrError = matchedRecord;
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
ArmyAssetRegistry_HasIdWithoutFlag0200(ArmyAssetId recordId)

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
uint8_t __thandor_cf_preserve_eax_ecx_edx ArmyAssetRegistry_HasIdWithFlag0200(ArmyAssetId recordId)

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
ArmyAssetRegistry_HasIdWithFlag0100Without0200(ArmyAssetId recordId)

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
ArmyAssetRegistry_HasIdWithFlags0100And0200(ArmyAssetId recordId)

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


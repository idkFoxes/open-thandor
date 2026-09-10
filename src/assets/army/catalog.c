#include <thandor/assets/army/catalog.h>

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
void ArmyAssetRegistry_NormalizeIdForFlag0100Without0200Cf(PckArmyAssetIdCatalog recordId)

{
  ArmyAssetId recordId_00;
  undefined1 in_CF;
  
  ArmyAssetRegistry_HasIdWithFlag0100Without0200Cf(recordId);
  if ((bool)in_CF) {
    ArmyAssetRegistry_FindNextFlag0100Without0200WrappedCf(recordId_00);
  }
  return;
}

/* Address: 0x00571A10.
   Ownership: assets/army/catalog.
   Purpose: Steps forward through IDs using the exact desired predicate and the broader flag-0x0200-clear
   predicate, with the verified reverse-gap fallback. EAX and CF remain intact. Stock ARM ledgers contain 675
   records and 326 unique ids; flag-filtered stepping preserves the 32-bit registry key and does not imply gameplay
   class, tier, faction, or direction.
   Local calls: ArmyAssetRegistry_HasIdWithFlag0100Without0200Cf, ArmyAssetRegistry_HasIdWithoutFlag0200Cf.
*/
void ArmyAssetRegistry_StepForwardFlag0100Without0200Cf(ArmyAssetId recordId)

{
  ArmyAssetId recordId_00;
  uint extraout_EAX;
  ArmyAssetId recordId_01;
  uint extraout_EAX_00;
  undefined1 in_CF;
  undefined1 uVar1;
  bool bVar2;
  
  while( true ) {
    uVar1 = in_CF;
    ArmyAssetRegistry_HasIdWithFlag0100Without0200Cf(recordId + 1);
    if (!(bool)uVar1) break;
    ArmyAssetRegistry_HasIdWithoutFlag0200Cf(recordId_00);
    recordId = extraout_EAX;
    in_CF = 0;
    if ((bool)uVar1) {
      bVar2 = extraout_EAX < 2;
      recordId_01 = extraout_EAX - 2;
      while( true ) {
        ArmyAssetRegistry_HasIdWithoutFlag0200Cf(recordId_01);
        recordId = extraout_EAX_00;
        in_CF = 1;
        if (bVar2) break;
        recordId_01 = extraout_EAX_00 - 1;
        bVar2 = false;
      }
    }
  }
  return;
}

/* Address: 0x00571A60.
   Ownership: assets/army/catalog.
   Purpose: Steps backward through IDs using the exact desired predicate and the broader flag-0x0200-clear
   predicate, with the verified forward-gap fallback. EAX and CF remain intact. Stock ARM ledgers contain 675
   records and 326 unique ids; flag-filtered stepping preserves the 32-bit registry key and does not imply gameplay
   class, tier, faction, or direction.
   Local calls: ArmyAssetRegistry_HasIdWithFlag0100Without0200Cf, ArmyAssetRegistry_HasIdWithoutFlag0200Cf.
*/
void ArmyAssetRegistry_StepBackwardFlag0100Without0200Cf(ArmyAssetId recordId)

{
  ArmyAssetId recordId_00;
  uint extraout_EAX;
  ArmyAssetId recordId_01;
  uint extraout_EAX_00;
  undefined1 in_CF;
  undefined1 uVar1;
  bool bVar2;
  
  while( true ) {
    uVar1 = in_CF;
    ArmyAssetRegistry_HasIdWithFlag0100Without0200Cf(recordId - 1);
    if (!(bool)uVar1) break;
    ArmyAssetRegistry_HasIdWithoutFlag0200Cf(recordId_00);
    recordId = extraout_EAX;
    in_CF = 0;
    if ((bool)uVar1) {
      bVar2 = 0xfffffffd < extraout_EAX;
      recordId_01 = extraout_EAX + 2;
      while( true ) {
        ArmyAssetRegistry_HasIdWithoutFlag0200Cf(recordId_01);
        recordId = extraout_EAX_00;
        in_CF = 1;
        if (bVar2) break;
        recordId_01 = extraout_EAX_00 + 1;
        bVar2 = false;
      }
    }
  }
  return;
}

/* Address: 0x00571B00.
   Ownership: assets/army/catalog.
   Purpose: Finds the previous ID in the flag-0x0100-set and flag-0x0200-clear class, wrapping signed underflow to
   0x1000. EAX and CF remain intact. Stock ARM ledgers contain 675 records and 326 unique ids; flag-filtered
   stepping preserves the 32-bit registry key and does not imply gameplay class, tier, faction, or direction.
   Local calls: ArmyAssetRegistry_HasIdWithoutFlag0200Cf, ArmyAssetRegistry_HasIdWithFlag0100Without0200Cf.
*/
void ArmyAssetRegistry_FindPreviousFlag0100Without0200WrappedCf(ArmyAssetId recordId)

{
  ArmyAssetId extraout_EAX;
  ArmyAssetId recordId_00;
  int extraout_EAX_00;
  undefined1 in_CF;
  
  do {
    ArmyAssetRegistry_HasIdWithoutFlag0200Cf(recordId);
    recordId_00 = extraout_EAX;
    if ((bool)in_CF)
    goto 
    ArmyAssetRegistry_FindPreviousFlag0100Without0200WrappedCf_ScanPreviousQualifiedCandidateWithWrap
    ;
    recordId = extraout_EAX - 1;
  } while (-1 < (int)recordId);
  recordId_00 = 0x1000;
ArmyAssetRegistry_FindPreviousFlag0100Without0200WrappedCf_ScanPreviousQualifiedCandidateWithWrap:
  while( true ) {
    ArmyAssetRegistry_HasIdWithFlag0100Without0200Cf(recordId_00);
    if (!(bool)in_CF) break;
    recordId_00 = extraout_EAX_00 - 1;
    if ((int)recordId_00 < 0) {
      recordId_00 = 0x1000;
    }
  }
  return;
}

/* Address: 0x00571C30.
   Ownership: assets/army/catalog.
   Purpose: Tests recordId against the exact flags-0x0100-and-0x0200-set predicate. When absent, it forwards the
   current EAX ID into the wrapped-next search. EAX and CF remain the result channel. Stock ARM ledgers contain 675
   records and 326 unique ids; flag-filtered stepping preserves the 32-bit registry key and does not imply gameplay
   class, tier, faction, or direction.
   Local calls: ArmyAssetRegistry_HasIdWithFlags0100And0200Cf, ArmyAssetRegistry_FindNextFlags0100And0200WrappedCf.
*/
void ArmyAssetRegistry_NormalizeIdForFlags0100And0200Cf(PckArmyAssetIdCatalog recordId)

{
  ArmyAssetId recordId_00;
  undefined1 in_CF;
  
  ArmyAssetRegistry_HasIdWithFlags0100And0200Cf(recordId);
  if ((bool)in_CF) {
    ArmyAssetRegistry_FindNextFlags0100And0200WrappedCf(recordId_00);
  }
  return;
}

/* Address: 0x00571C50.
   Ownership: assets/army/catalog.
   Purpose: Steps forward through IDs using the exact combined-flags predicate and the broader flag-0x0200-set
   predicate, with the verified reverse-gap fallback. EAX and CF remain intact. Stock ARM ledgers contain 675
   records and 326 unique ids; flag-filtered stepping preserves the 32-bit registry key and does not imply gameplay
   class, tier, faction, or direction.
   Local calls: ArmyAssetRegistry_HasIdWithFlags0100And0200Cf, ArmyAssetRegistry_HasIdWithFlag0200Cf.
*/
void ArmyAssetRegistry_StepForwardFlags0100And0200Cf(ArmyAssetId recordId)

{
  ArmyAssetId recordId_00;
  uint extraout_EAX;
  ArmyAssetId recordId_01;
  uint extraout_EAX_00;
  undefined1 in_CF;
  undefined1 uVar1;
  bool bVar2;
  
  while( true ) {
    uVar1 = in_CF;
    ArmyAssetRegistry_HasIdWithFlags0100And0200Cf(recordId + 1);
    if (!(bool)uVar1) break;
    ArmyAssetRegistry_HasIdWithFlag0200Cf(recordId_00);
    recordId = extraout_EAX;
    in_CF = 0;
    if ((bool)uVar1) {
      bVar2 = extraout_EAX < 2;
      recordId_01 = extraout_EAX - 2;
      while( true ) {
        ArmyAssetRegistry_HasIdWithFlag0200Cf(recordId_01);
        recordId = extraout_EAX_00;
        in_CF = 1;
        if (bVar2) break;
        recordId_01 = extraout_EAX_00 - 1;
        bVar2 = false;
      }
    }
  }
  return;
}

/* Address: 0x00571CA0.
   Ownership: assets/army/catalog.
   Purpose: Steps backward through IDs using the exact combined-flags predicate and the broader flag-0x0200-set
   predicate, with the verified forward-gap fallback. EAX and CF remain intact. Stock ARM ledgers contain 675
   records and 326 unique ids; flag-filtered stepping preserves the 32-bit registry key and does not imply gameplay
   class, tier, faction, or direction.
   Local calls: ArmyAssetRegistry_HasIdWithFlags0100And0200Cf, ArmyAssetRegistry_HasIdWithFlag0200Cf.
*/
void ArmyAssetRegistry_StepBackwardFlags0100And0200Cf(ArmyAssetId recordId)

{
  ArmyAssetId recordId_00;
  uint extraout_EAX;
  ArmyAssetId recordId_01;
  uint extraout_EAX_00;
  undefined1 in_CF;
  undefined1 uVar1;
  bool bVar2;
  
  while( true ) {
    uVar1 = in_CF;
    ArmyAssetRegistry_HasIdWithFlags0100And0200Cf(recordId - 1);
    if (!(bool)uVar1) break;
    ArmyAssetRegistry_HasIdWithFlag0200Cf(recordId_00);
    recordId = extraout_EAX;
    in_CF = 0;
    if ((bool)uVar1) {
      bVar2 = 0xfffffffd < extraout_EAX;
      recordId_01 = extraout_EAX + 2;
      while( true ) {
        ArmyAssetRegistry_HasIdWithFlag0200Cf(recordId_01);
        recordId = extraout_EAX_00;
        in_CF = 1;
        if (bVar2) break;
        recordId_01 = extraout_EAX_00 + 1;
        bVar2 = false;
      }
    }
  }
  return;
}

/* Address: 0x00571D40.
   Ownership: assets/army/catalog.
   Purpose: Finds the previous ID with flags 0x0100 and 0x0200 set, wrapping signed underflow to 0x1000. EAX and CF
   remain intact. Stock ARM ledgers contain 675 records and 326 unique ids; flag-filtered stepping preserves the
   32-bit registry key and does not imply gameplay class, tier, faction, or direction.
   Local calls: ArmyAssetRegistry_HasIdWithFlag0200Cf, ArmyAssetRegistry_HasIdWithFlags0100And0200Cf.
*/
void ArmyAssetRegistry_FindPreviousFlags0100And0200WrappedCf(ArmyAssetId recordId)

{
  ArmyAssetId extraout_EAX;
  ArmyAssetId recordId_00;
  int extraout_EAX_00;
  undefined1 in_CF;
  
  do {
    ArmyAssetRegistry_HasIdWithFlag0200Cf(recordId);
    recordId_00 = extraout_EAX;
    if ((bool)in_CF)
    goto 
    ArmyAssetRegistry_FindPreviousFlags0100And0200WrappedCf_ScanPreviousQualifiedCandidateWithWrap;
    recordId = extraout_EAX - 1;
  } while (-1 < (int)recordId);
  recordId_00 = 0x1000;
ArmyAssetRegistry_FindPreviousFlags0100And0200WrappedCf_ScanPreviousQualifiedCandidateWithWrap:
  while( true ) {
    ArmyAssetRegistry_HasIdWithFlags0100And0200Cf(recordId_00);
    if (!(bool)in_CF) break;
    recordId_00 = extraout_EAX_00 - 1;
    if ((int)recordId_00 < 0) {
      recordId_00 = 0x1000;
    }
  }
  return;
}

/* Address: 0x0051B5E0.
   Ownership: assets/army/catalog.
   Purpose: Validates the 'arm' magic and converter version 0x00020008, then prepares recordCount variable-size
   records beginning at +0x200. Each successful record advances by its leading byteSize. Invalid headers update the
   package last-error path. CF and EAX status are preserved.
   Local calls: ArmyAssetRecord_RegisterAndRelocate.
   Cross-module calls: Package_SetLastErrorPath [assets/package/runtime].
*/
dword ArmyAsset_PrepareRecords(ArmyAssetHeader *asset)

{
  dword registrationStatusCode;
  undefined4 extraout_EAX;
  int extraout_ECX;
  ArmyAssetHeader *record;
  bool bVar1;
  longlong lVar2;
  
  registrationStatusCode = 0x40;
  if (((asset->recordCountHeader).common.magic == ASSET_MAGIC_ARM) &&
     ((asset->recordCountHeader).common.converterVersion == PCK_CONVERTER_ARM_00020008)) {
    record = asset + 1;
    bVar1 = false;
    if ((asset->recordCountHeader).recordCount != 0) {
      do {
        lVar2 = ArmyAssetRecord_RegisterAndRelocate((ArmyAssetRuntimeSemanticView80 *)record,asset);
        registrationStatusCode = (dword)lVar2;
        if (bVar1) {
          return registrationStatusCode;
        }
        bVar1 = CARRY4((uint)record,(record->recordCountHeader).common.magic);
        record = (ArmyAssetHeader *)
                 ((int)(record->recordCountHeader).common.buildMetadata.assetRelativeAddressAnchor28
                 + ((record->recordCountHeader).common.magic - 0x28));
      } while (extraout_ECX != 1);
    }
    return registrationStatusCode;
  }
  Package_SetLastErrorPath((word *)asset);
  return extraout_EAX;
}

/* Address: 0x0051B740.
   Ownership: assets/army/catalog.
   Purpose: Resolves an army asset by registry identifier and returns success only when the asset is present and
   its verified enabled flag is set.
   Local calls: ArmyAssetRegistry_FindByIdCf.
*/
undefined4 ArmyAssetRegistry_FindEnabledByIdCf(PckArmyAssetIdCatalog recordId)

{
  undefined4 in_EAX;
  
  ArmyAssetRegistry_FindByIdCf(recordId);
  return in_EAX;
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
void ArmyAssetRecord_HasFactionUnlockedLinkedDefinitionCf
               (FactionRuntimeIndex factionIndex,undefined4 param_2,
               ArmyAssetRecordPrefix *armyAssetRecord)

{
  ArmyAssetRecordPrefix *definitionNode;
  dword dVar1;
  dword extraout_ECX;
  bool bVar2;
  ModelTechnologyHierarchyCfVolatileContinuityResult MVar3;
  
  dVar1 = 0x10;
  do {
    bVar2 = false;
    if (armyAssetRecord[3].byteSize != 0) {
      definitionNode = ArmyAssetRegistry_FindByIdCf(armyAssetRecord[3].byteSize);
      dVar1 = extraout_ECX;
      if ((!bVar2) &&
         (bVar2 = false, (definitionNode[1].selectionDetailTemplateVariantIndex & 1) != 0)) {
        MVar3 = ModelDefinitionHierarchy_AllTechnologyUnlockedForFactionCf
                          (factionIndex,(ModelDefinitionHierarchyNodeAddress32)definitionNode);
        dVar1 = MVar3.preservedEcxCallerValue;
        if ((!bVar2) &&
           ((*(int *)(MVar3.preservedEaxDefinitionNodeAddress + 0x1c) != 0 &&
            ((*(uint *)(MVar3.preservedEaxDefinitionNodeAddress + 0x14) &
             MVar3.preservedEdxCallerValue) != 0)))) {
          return;
        }
      }
    }
    armyAssetRecord = (ArmyAssetRecordPrefix *)&armyAssetRecord->selectionDetailTemplateVariantIndex
    ;
    dVar1 = dVar1 - 1;
    if (dVar1 == 0) {
      return;
    }
  } while( true );
}

/* Address: 0x00571E40.
   Ownership: assets/army/catalog.
   Purpose: Clears cached ArmyAsset preview textures, frees existing cache entries, and refreshes the two selected
   preview resources.
   Local calls: ArmyAssetRegistry_ResolveOrCreatePreviewTextureCf.
*/
void ArmyAssetRegistry_ClearPreviewTextureCacheAndRefreshSelected(dword selectedArmyAssetRegistryId)

{
  dword dVar1;
  int iVar2;
  int extraout_ECX;
  ArmyAssetRecordPrefix **ppAVar3;
  ArmyAssetRecordPrefix *armyDefinition1;
  
  ppAVar3 = g_ArmyAssetRecordRegistry;
  iVar2 = 0x300;
  do {
    armyDefinition1 = *ppAVar3;
    if (armyDefinition1 != (ArmyAssetRecordPrefix *)0x0) {
      (*g_MemoryApi.free)((void *)armyDefinition1[2].byteSize);
      armyDefinition1[2].byteSize = 0;
      iVar2 = extraout_ECX;
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
ArmyUnlockedArmourAggregateEaxPreservedEdxCarrier64
ArmyAssetHierarchy_SumFactionUnlockedArmour
          (FactionRuntimeIndex factionIndex,ModelDefinitionHierarchyNodeAddress32 definitionNode)

{
  ModelDefinitionRecordPrefix *pMVar1;
  undefined4 in_EDX;
  int extraout_EDX;
  int iVar2;
  int unaff_EBP;
  int unaff_EDI;
  ModelLinkedDefinitionListAddress32 linkedDefinitionList;
  int iVar3;
  
  iVar2 = 0;
  linkedDefinitionList = *(ModelLinkedDefinitionListAddress32 *)(definitionNode + 0xc);
  do {
    pMVar1 = ModelDefinition_SelectFactionUnlockedLinkedDefinitionCf
                       (factionIndex,linkedDefinitionList);
    iVar2 = iVar2 + 1;
    iVar3 = unaff_EDI;
    do {
      while (unaff_EBP == 0) {
        iVar2 = iVar2 + -1;
        if (iVar2 == 0) {
          return CONCAT44(in_EDX,extraout_EDX + pMVar1[8].byteSize);
        }
      }
      unaff_EBP = unaff_EBP + -1;
      unaff_EDI = iVar3 + 4;
      linkedDefinitionList = *(int *)(iVar3 + 0xc);
      iVar3 = unaff_EDI;
    } while (linkedDefinitionList == 0);
  } while( true );
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
EnergyQ4AggregateEaxPreservedEdxCarrier64
ArmyAssetHierarchy_SumFactionUnlockedDisplayedEnergyQ4
          (FactionRuntimeIndex factionIndex,ModelDefinitionHierarchyNodeAddress32 definitionNode)

{
  ModelDefinitionRecordPrefix *pMVar1;
  undefined4 in_EDX;
  int extraout_EDX;
  int iVar2;
  int unaff_EBP;
  int unaff_EDI;
  ModelLinkedDefinitionListAddress32 linkedDefinitionList;
  
  iVar2 = 0;
  linkedDefinitionList = *(ModelLinkedDefinitionListAddress32 *)(definitionNode + 0xc);
  do {
    pMVar1 = ModelDefinition_SelectFactionUnlockedLinkedDefinitionCf
                       (factionIndex,linkedDefinitionList);
    iVar2 = iVar2 + 1;
    while (unaff_EBP == 0) {
      iVar2 = iVar2 + -1;
      if (iVar2 == 0) {
        return CONCAT44(in_EDX,extraout_EDX + pMVar1[0x21].byteSize);
      }
    }
    linkedDefinitionList = *(ModelLinkedDefinitionListAddress32 *)(unaff_EDI + 0xc);
    unaff_EBP = unaff_EBP + -1;
    unaff_EDI = unaff_EDI + 4;
  } while( true );
}

/* Address: 0x00571AB0.
   Ownership: assets/army/catalog.
   Purpose: Finds the next ID in the flag-0x0100-set and flag-0x0200-clear class, first advancing to a broader
   valid record and then wrapping the desired search at 0x1000. EAX and CF remain intact. Stock ARM ledgers contain
   675 records and 326 unique ids; flag-filtered stepping preserves the 32-bit registry key and does not imply
   gameplay class, tier, faction, or direction.
   Local calls: ArmyAssetRegistry_HasIdWithoutFlag0200Cf, ArmyAssetRegistry_HasIdWithFlag0100Without0200Cf.
*/
void ArmyAssetRegistry_FindNextFlag0100Without0200WrappedCf(ArmyAssetId recordId)

{
  ArmyAssetId extraout_EAX;
  int extraout_EAX_00;
  ArmyAssetId recordId_00;
  undefined1 in_CF;
  bool bVar1;
  
  while( true ) {
    ArmyAssetRegistry_HasIdWithoutFlag0200Cf(recordId);
    bVar1 = true;
    recordId_00 = extraout_EAX;
    if ((bool)in_CF) break;
    recordId = extraout_EAX + 1;
    in_CF = 0;
  }
  while( true ) {
    ArmyAssetRegistry_HasIdWithFlag0100Without0200Cf(recordId_00);
    if (!bVar1) break;
    recordId_00 = extraout_EAX_00 + 1;
    bVar1 = recordId_00 < 0x1000;
    if (!bVar1) {
      bVar1 = false;
      recordId_00 = 0;
    }
  }
  return;
}

/* Address: 0x00571CF0.
   Ownership: assets/army/catalog.
   Purpose: Finds the next ID with flags 0x0100 and 0x0200 set, first advancing to a broader flag-0x0200 record and
   then wrapping the desired search at 0x1000. EAX and CF remain intact. Stock ARM ledgers contain 675 records and
   326 unique ids; flag-filtered stepping preserves the 32-bit registry key and does not imply gameplay class,
   tier, faction, or direction.
   Local calls: ArmyAssetRegistry_HasIdWithFlag0200Cf, ArmyAssetRegistry_HasIdWithFlags0100And0200Cf.
*/
void ArmyAssetRegistry_FindNextFlags0100And0200WrappedCf(ArmyAssetId recordId)

{
  ArmyAssetId extraout_EAX;
  int extraout_EAX_00;
  ArmyAssetId recordId_00;
  undefined1 in_CF;
  bool bVar1;
  
  while( true ) {
    ArmyAssetRegistry_HasIdWithFlag0200Cf(recordId);
    bVar1 = true;
    recordId_00 = extraout_EAX;
    if ((bool)in_CF) break;
    recordId = extraout_EAX + 1;
    in_CF = 0;
  }
  while( true ) {
    ArmyAssetRegistry_HasIdWithFlags0100And0200Cf(recordId_00);
    if (!bVar1) break;
    recordId_00 = extraout_EAX_00 + 1;
    bVar1 = recordId_00 < 0x1000;
    if (!bVar1) {
      bVar1 = false;
      recordId_00 = 0;
    }
  }
  return;
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
longlong ArmyAssetRecord_RegisterAndRelocate
                   (ArmyAssetRuntimeSemanticView80 *record,ArmyAssetHeader *assetBase)

{
  byte *pbVar1;
  dword dVar2;
  ArmyAssetRuntimeSemanticView80 *pAVar3;
  ArmyAssetRuntimeSemanticView80 *pAVar4;
  int extraout_ECX;
  int iVar5;
  uint in_EDX;
  int extraout_EDX;
  int iVar6;
  int extraout_EDX_00;
  int iVar7;
  ArmyAssetRecordPrefix **ppAVar8;
  int unaff_EBP;
  int unaff_ESI;
  byte *unaff_EDI;
  byte *pbVar9;
  undefined1 in_CF;
  bool bVar10;
  
  pAVar3 = record;
  ppAVar8 = g_ArmyAssetRecordRegistry;
  ArmyAssetRegistry_FindByIdCf(record->registryId);
  iVar6 = extraout_EDX;
  if ((bool)in_CF) {
    do {
      if (*ppAVar8 == (ArmyAssetRecordPrefix *)0x0) {
        dVar2 = record->rootNodeOffsetOrPointer;
        *ppAVar8 = (ArmyAssetRecordPrefix *)record;
        record = (ArmyAssetRuntimeSemanticView80 *)0x0;
        if (dVar2 != 0) {
          pAVar3->rootNodeOffsetOrPointer =
               (dword)((assetBase->recordCountHeader).common.buildMetadata.
                       assetRelativeAddressAnchor28 + (pAVar3->rootNodeOffsetOrPointer - 0x28));
          pbVar9 = (assetBase->recordCountHeader).common.buildMetadata.assetRelativeAddressAnchor28
                   + (dVar2 - 0x28);
          bVar10 = false;
          iVar6 = 0;
          do {
            pAVar4 = ModelDefinitionRegistry_FindBuildMetricTupleByIdCf
                               (*(PckModelDefinitionIdCatalog *)(pbVar9 + 0x20));
            if (!bVar10) {
              pAVar3->relocationPointerOrOffset2C =
                   (dword)(pAVar4->reserved018_023 + (pAVar3->relocationPointerOrOffset2C - 0x18));
              pAVar3->relocationValue24 = pAVar3->relocationValue24 + extraout_ECX;
              pAVar3->relocationValue28 = pAVar3->relocationValue28 + extraout_EDX_00;
              pAVar4 = record;
            }
            record = pAVar4;
            iVar7 = 0;
            iVar5 = *(int *)((AssetProducerSourceNames *)(pbVar9 + 8))->producerName;
            while (iVar5 == 0) {
              iVar6 = iVar6 + -1;
              if (iVar6 < 0) goto ArmyAssetRecord_RegisterAndRelocate_FinalizeRelocationResult;
              iVar7 = unaff_ESI + 1;
              pbVar9 = unaff_EDI;
              iVar5 = unaff_EBP + -1;
            }
            iVar6 = iVar6 + 1;
            pbVar1 = pbVar9 + iVar7 * 4 + 0xc;
            bVar10 = CARRY4(*(uint *)pbVar1,(uint)assetBase);
            *(byte **)pbVar1 =
                 (assetBase->recordCountHeader).common.buildMetadata.assetRelativeAddressAnchor28 +
                 (*(uint *)pbVar1 - 0x28);
            pbVar9 = *(byte **)(pbVar9 + iVar7 * 4 + 0xc);
            unaff_EBP = iVar5;
          } while( true );
        }
ArmyAssetRecord_RegisterAndRelocate_FinalizeRelocationResult:
        if (record == (ArmyAssetRuntimeSemanticView80 *)0x0) {
          return (ulonglong)in_EDX << 0x20;
        }
        goto ArmyAssetRecord_RegisterAndRelocate_ReturnRegistrationStatus;
      }
      ppAVar8 = ppAVar8 + 1;
      iVar6 = iVar6 + -1;
    } while (iVar6 != 0);
    (*g_WideNumberFormatUtf16)(WIDE_FORMAT_WRITE_TERMINATOR,0,10,1,0x300,g_PackageLastErrorPath);
    record = (ArmyAssetRuntimeSemanticView80 *)0x42;
  }
  else {
    (*g_WideNumberFormatUtf16)
              (WIDE_FORMAT_WRITE_TERMINATOR,0,10,1,record->registryId,g_PackageLastErrorPath);
    record = (ArmyAssetRuntimeSemanticView80 *)0x4c;
  }
ArmyAssetRecord_RegisterAndRelocate_ReturnRegistrationStatus:
  return CONCAT44(in_EDX,record);
}

/* Address: 0x00571D90.
   Ownership: assets/army/catalog.
   Purpose: Looks up an ArmyAsset registry record by id and returns its cached preview texture; if absent it
   creates and caches the preview through ArmyRuntime_RenderPreviewTextureCf.
   Cross-module calls: ArmyRuntime_RenderPreviewTextureCf [gameplay/army/runtime].
*/
dword ArmyAssetRegistry_ResolveOrCreatePreviewTextureCf(dword armyAssetRegistryId)

{
  int iVar1;
  ArmyAssetRecordPrefix **ppAVar2;
  bool bVar3;
  undefined8 uVar4;
  FactionRuntimeIndex factionIndex;
  ArmyAssetRecordPrefix *armyDefinition1;
  
  ppAVar2 = g_ArmyAssetRecordRegistry;
  iVar1 = 0x300;
  while ((armyDefinition1 = *ppAVar2, armyDefinition1 == (ArmyAssetRecordPrefix *)0x0 ||
         (armyAssetRegistryId != armyDefinition1->registryId))) {
    ppAVar2 = ppAVar2 + 1;
    iVar1 = iVar1 + -1;
    if (iVar1 == 0) {
      return 0;
    }
  }
  if (armyDefinition1[2].byteSize == 0) {
    bVar3 = armyAssetRegistryId < 400;
    factionIndex = g_UiCommandModeGOwnerFactionIndex;
    if (!bVar3) {
      factionIndex = 0;
    }
    uVar4 = ArmyRuntime_RenderPreviewTextureCf
                      (iVar1,armyDefinition1,
                       *(GraphicsPixelDimension *)(g_InGameRuntimeRoot->opaque9A74_9B4B + 0x5c),
                       *(GraphicsPixelDimension *)(g_InGameRuntimeRoot->opaque9A74_9B4B + 0x5c),
                       factionIndex,armyAssetRegistryId,&g_InGameRuntimeRoot->worldRuntime0A30);
    if (bVar3) {
      return 0;
    }
    *(dword *)((int)((ulonglong)uVar4 >> 0x20) + 0x20) = (dword)uVar4;
    return (dword)uVar4;
  }
  return armyDefinition1[2].byteSize;
}

/* Address: 0x0051B6D0.
   Ownership: assets/army/catalog.
   Purpose: Scans the fixed 768-pointer army registry for registryId. A match returns the record with CF clear.
   Failure formats the requested identifier into the package error buffer and returns error 0x41 with CF set. Stock
   ARM ledgers contain 675 records and 326 unique ids; flag-filtered stepping preserves the 32-bit registry key and
   does not imply gameplay class, tier, faction, or direction.
*/
ArmyAssetRecordPrefix * ArmyAssetRegistry_FindByIdCf(PckArmyAssetIdCatalog registryId)

{
  int registrySlotsRemaining;
  ArmyAssetRecordPrefix **registryCursor;
  ArmyAssetRecordPrefix *candidateAsset;
  ArmyAssetRecordPrefix *armyDefinition1;
  
  registryCursor = g_ArmyAssetRecordRegistry;
  registrySlotsRemaining = 0x300;
  while( true ) {
    armyDefinition1 = *registryCursor;
    if ((armyDefinition1 != (ArmyAssetRecordPrefix *)0x0) &&
       (armyDefinition1->registryId == registryId)) break;
    registryCursor = registryCursor + 1;
    registrySlotsRemaining = registrySlotsRemaining + -1;
    if (registrySlotsRemaining == 0) {
      (*g_WideNumberFormatUtf16)
                (WIDE_FORMAT_WRITE_TERMINATOR,0,10,1,registryId,g_PackageLastErrorPath);
      return (ArmyAssetRecordPrefix *)0x41;
    }
  }
  return armyDefinition1;
}

/* Address: 0x00571910.
   Ownership: assets/army/catalog.
   Purpose: Scans all 768 ArmyAsset registry pointers for recordId. CF clear means a matching record was found with
   flags dword +0x14 bit 0x0200 clear; CF set means absent. EAX preserves recordId. Stock ARM ledgers contain 675
   records and 326 unique ids; flag-filtered stepping preserves the 32-bit registry key and does not imply gameplay
   class, tier, faction, or direction.
*/
void ArmyAssetRegistry_HasIdWithoutFlag0200Cf(ArmyAssetId recordId)

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
      return;
    }
  }
  return;
}

/* Address: 0x00571B50.
   Ownership: assets/army/catalog.
   Purpose: Scans all 768 ArmyAsset registry pointers for recordId. CF clear requires flag 0x0200 set; CF set means
   absent. EAX preserves recordId. Stock ARM ledgers contain 675 records and 326 unique ids; flag-filtered stepping
   preserves the 32-bit registry key and does not imply gameplay class, tier, faction, or direction.
*/
void ArmyAssetRegistry_HasIdWithFlag0200Cf(ArmyAssetId recordId)

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
      return;
    }
  }
  return;
}

/* Address: 0x00571980.
   Ownership: assets/army/catalog.
   Purpose: Scans all 768 ArmyAsset registry pointers for recordId. CF clear requires flag 0x0100 set and flag
   0x0200 clear; CF set means absent. EAX preserves recordId. Stock ARM ledgers contain 675 records and 326 unique
   ids; flag-filtered stepping preserves the 32-bit registry key and does not imply gameplay class, tier, faction,
   or direction.
*/
void ArmyAssetRegistry_HasIdWithFlag0100Without0200Cf(ArmyAssetId recordId)

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
      return;
    }
  }
  return;
}

/* Address: 0x00571BC0.
   Ownership: assets/army/catalog.
   Purpose: Scans all 768 ArmyAsset registry pointers for recordId. CF clear requires both flags 0x0100 and 0x0200
   set; CF set means absent. EAX preserves recordId. Stock ARM ledgers contain 675 records and 326 unique ids;
   flag-filtered stepping preserves the 32-bit registry key and does not imply gameplay class, tier, faction, or
   direction.
*/
void ArmyAssetRegistry_HasIdWithFlags0100And0200Cf(ArmyAssetId recordId)

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
      return;
    }
  }
  return;
}

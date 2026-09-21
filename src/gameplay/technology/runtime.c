/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/src/gameplay/technology/runtime.c
 * Reverse engineering by idkFoxes 2026
 */

#include <thandor/gameplay/technology/runtime.h>

/* Implementation ownership: gameplay/technology/runtime. */

/* Address: 0x005139C0.
   Ownership: gameplay/technology/runtime.
   Purpose: Sets one technology bit in the selected faction's 256-bit unlock mask when not already present, emits
   the verified local-player notification, recursively processes dependencyTechnologyIndex, refreshes matching
   active build records, and rebuilds the two affected catalog grids. The function preserves its nonstandard
   register/flag result contract. Sets the unlock bit, recursively unlocks dependencyTechnologyIndex, queues the
   completionMessageResourceId notification (the +0x30 field docs mislabeled "techRef"), applies model variants and
   rebuilds the build-grid UI groups.
   Cross-module calls: InGameNotificationQueue_InsertPriorityRecord [ui/ingame/runtime],
   ModelRuntimeHierarchy_ApplyFactionTechnologyVariants [world/model/hierarchy], UiCatalogGroup48_RebuildGrid
   [ui/ingame/technology], UiCatalogGroup42_RebuildGrid [ui/ingame/technology].
*/
void __thandor_void_preserve_eax_ecx_edx
Technology_UnlockForFaction
          (GraphicsWorldCoordinateQ12 notificationXQ12,GraphicsWorldCoordinateQ12 notificationYQ12,
          TechnologyId technologyIndex,FactionRuntimeIndex factionIndex)

{
  WorldOwnerListNode100 *pWVar1;
  ArmyRuntimeSlot *modelRuntimeHolder;
  InGameRuntimeRootImageC3E4 *node;
  uint technologyBitMask;
  uint *factionTechnologyMaskWord;
  TechnologyAsset *technologyAsset;
  
  node = g_InGameRuntimeRoot;
  technologyAsset = g_TechnologyAsset;
  technologyBitMask = 1 << ((byte)technologyIndex & 0x1f);
  factionTechnologyMaskWord = (uint *)(factionIndex * 0x740 + 0x50fa20 + (technologyIndex >> 5) * 4)
  ;
  if ((*factionTechnologyMaskWord & technologyBitMask) == 0) {
    *factionTechnologyMaskWord = *factionTechnologyMaskWord | technologyBitMask;
    if (((g_UiCommandRuntimeFlags & 0x10) == 0) &&
       (factionIndex == (node->worldRuntime0A30).activeFactionRuntimeIndex)) {
      if ((notificationYQ12 == 0) && (notificationXQ12 == 0)) {
        InGameNotificationQueue_InsertPriorityRecord
                  (NONE,0,0,0,0,0,5,
                   g_TechnologyAsset->records[technologyIndex].completionMessageResourceId);
      }
      else {
        InGameNotificationQueue_InsertPriorityRecord
                  (TECHNOLOGY_UNLOCK_POSITION,0,0,0,notificationXQ12,notificationYQ12,5,
                   g_TechnologyAsset->records[technologyIndex].completionMessageResourceId);
      }
    }
    Technology_UnlockForFaction
              (0,0,technologyAsset->records[technologyIndex].dependencyTechnologyIndex,factionIndex)
    ;
    for (pWVar1 = (node->worldRuntime0A30).ownerListHead; pWVar1 != (WorldOwnerListNode100 *)0x0;
        pWVar1 = pWVar1->nextNode) {
      if ((pWVar1->ownerClassId == WORLD_OWNER_RUNTIME_MODEL) &&
         (modelRuntimeHolder = *(ArmyRuntimeSlot **)((int)pWVar1->runtimePayload + 8),
         factionIndex == modelRuntimeHolder->factionIndex)) {
        ModelRuntimeHierarchy_ApplyFactionTechnologyVariants(factionIndex,modelRuntimeHolder);
      }
    }
    if (factionIndex == (node->worldRuntime0A30).activeFactionRuntimeIndex) {
      UiCatalogGroup48_RebuildGrid((UiNodeBase *)node);
      UiCatalogGroup42_RebuildGrid((UiNodeBase *)node);
    }
  }
  return;
}


/* Address: 0x00513AE0.
   Ownership: gameplay/technology/runtime.
   Purpose: Tests one bit in the selected faction's 256-bit technology unlock mask. The original function reports
   the result through CF while preserving EAX. Bit test against the faction's eight 32-bit unlock words; CF-style
   result.
*/
bool __thandor_cf_preserve_eax_ecx_edx
Technology_IsUnlockedForFactionCf
          (PckTechnologyIdCatalog technologyIndex,FactionRuntimeIndex factionIndex)

{
  if ((*(uint *)(factionIndex * 0x740 + 0x50fa20 + (technologyIndex >> 5) * 4) &
      1 << ((byte)technologyIndex & 0x1f)) != 0) {
    return false;
  }
  return true;
}


/* Address: 0x00513B20.
   Ownership: gameplay/technology/runtime.
   Purpose: Tests that the technology is still locked, all eight prerequisite masks are satisfied by the selected
   faction, and no active entity of that faction is already executing the same technology. CF set means available;
   CF clear means unavailable. EAX is preserved by the original ABI. prerequisiteMasks[8] AND-test against the
   faction unlock words (usually one set bit; value 1 reused for roots/gates — tech-tree.md). Stock tech.tec has
   512 records over canonical ids 0..255; localized titles do not prove source-building, tier, direction, or effect
   mappings.
*/
bool __thandor_cf_preserve_eax_ecx_edx
Technology_IsAvailableForFactionCf
          (PckTechnologyIdCatalog technologyIndex,FactionRuntimeIndex factionIndex)

{
  WorldRuntimeNode *worldNodeCursor;
  ArmyRuntimeSlot *activeResearchArmyRuntime;
  
  if (((((*(uint *)(factionIndex * 0x740 + 0x50fa20 + (technologyIndex >> 5) * 4) &
         1 << ((byte)technologyIndex & 0x1f)) == 0) &&
       ((g_TechnologyAsset->records[technologyIndex].prerequisiteMasks[0] &
        g_GameFactionRuntimeImage.records[factionIndex].technologyMasks256Bits[0]) ==
        g_TechnologyAsset->records[technologyIndex].prerequisiteMasks[0])) &&
      ((g_TechnologyAsset->records[technologyIndex].prerequisiteMasks[1] &
       g_GameFactionRuntimeImage.records[factionIndex].technologyMasks256Bits[1]) ==
       g_TechnologyAsset->records[technologyIndex].prerequisiteMasks[1])) &&
     (((((g_TechnologyAsset->records[technologyIndex].prerequisiteMasks[2] &
         g_GameFactionRuntimeImage.records[factionIndex].technologyMasks256Bits[2]) ==
         g_TechnologyAsset->records[technologyIndex].prerequisiteMasks[2] &&
        ((g_TechnologyAsset->records[technologyIndex].prerequisiteMasks[3] &
         g_GameFactionRuntimeImage.records[factionIndex].technologyMasks256Bits[3]) ==
         g_TechnologyAsset->records[technologyIndex].prerequisiteMasks[3])) &&
       (((g_TechnologyAsset->records[technologyIndex].prerequisiteMasks[4] &
         g_GameFactionRuntimeImage.records[factionIndex].technologyMasks256Bits[4]) ==
         g_TechnologyAsset->records[technologyIndex].prerequisiteMasks[4] &&
        (((g_TechnologyAsset->records[technologyIndex].prerequisiteMasks[5] &
          g_GameFactionRuntimeImage.records[factionIndex].technologyMasks256Bits[5]) ==
          g_TechnologyAsset->records[technologyIndex].prerequisiteMasks[5] &&
         ((g_TechnologyAsset->records[technologyIndex].prerequisiteMasks[6] &
          g_GameFactionRuntimeImage.records[factionIndex].technologyMasks256Bits[6]) ==
          g_TechnologyAsset->records[technologyIndex].prerequisiteMasks[6])))))) &&
      ((g_TechnologyAsset->records[technologyIndex].prerequisiteMasks[7] &
       g_GameFactionRuntimeImage.records[factionIndex].technologyMasks256Bits[7]) ==
       g_TechnologyAsset->records[technologyIndex].prerequisiteMasks[7])))) {
    worldNodeCursor = (WorldRuntimeNode *)(g_InGameRuntimeRoot->worldRuntime0A30).ownerListHead;
    do {
      if (worldNodeCursor == (WorldRuntimeNode *)0x0) {
        return true;
      }
      if (worldNodeCursor[2].common.nextNode == (WorldRuntimeNode *)0x0) {
        activeResearchArmyRuntime = worldNodeCursor->runtimePayload;
        if ((((activeResearchArmyRuntime->runtimeFlags & 0x40) != 0) &&
            (factionIndex ==
             (activeResearchArmyRuntime->linkedEntityRuntime->common).ownership.ownerIndex)) &&
           (technologyIndex == activeResearchArmyRuntime->stateOrTechnologyId)) {
          return false;
        }
      }
      worldNodeCursor = (worldNodeCursor->common).nextNode;
    } while( true );
  }
  return false;
}


/* Address: 0x0052AE10.
   Ownership: gameplay/technology/runtime.
   Purpose: When the entity is not already in either blocked state bit, writes the technology index and the three
   verified record values into its runtime technology state, applies the high-speed scaling path to entityValue28,
   and sets state bit 0x80. Copies entityValue20/24/28 into entity runtime state; a runtime flag scales
   entityValue28 before storage. The HUD reads these raw as xenit/energy/time costs (tech-tree.md). Stock tech.tec
   has 512 records over canonical ids 0..255; localized titles do not prove source-building, tier, direction, or
   effect mappings. [RESOURCE_FUEL_ENERGY_CAPACITY_SEPARATION_CLOSURE] Copies TEC +0x20 Xenite requirement, +0x24
   Energy requirement, and +0x28 duration into the entity technology payload.
*/
void __thandor_void_preserve_eax_ecx_edx
Technology_ApplyRecordToEntity(PckTechnologyIdCatalog technologyIndex,GameEntityRuntime *entity)

{
  uint appliedEntityValue28;
  dword recordEntityValue20;
  dword recordEntityValue24;
  GameEntityRuntimeFlags *entityRuntimeFlags;
  
  if (((entity->common).runtimeFlags & 0xc0) == 0) {
    recordEntityValue20 = g_TechnologyAsset->records[technologyIndex].xeniteCostQ4;
    recordEntityValue24 = g_TechnologyAsset->records[technologyIndex].energyCostQ4;
    appliedEntityValue28 = g_TechnologyAsset->records[technologyIndex].researchDurationQ5;
    (entity->classPayload).technology.entityValue24 = 0;
    (entity->common).commandState = technologyIndex;
    if ((g_UiCommandRuntimeFlags & 0x100000) != 0) {
      appliedEntityValue28 = (appliedEntityValue28 >> 4) + 1;
    }
    (entity->classPayload).technology.xeniteCostQ4 = recordEntityValue20;
    (entity->classPayload).technology.energyCostQ4 = recordEntityValue24;
    (entity->classPayload).technology.appliedResearchDurationQ5 = appliedEntityValue28;
    entityRuntimeFlags = &(entity->common).runtimeFlags;
    *entityRuntimeFlags = *entityRuntimeFlags | 0x80;
  }
  return;
}


/* Address: 0x00539BB0.
   Ownership: gameplay/technology/runtime.
   Purpose: Rebuilds derived per-class limits from the fixed army registry, computes eight reciprocal scale values,
   clears the two 256-bit technology category masks, and scans all 256 TechnologyRecord entries to collect
   categories 2 and 3. The original EDX faction input and preserved EAX contract remain explicit. Builds the
   256-bit category masks for categories 2 and 3 (category field @+0x34, domain counts {0:29, 1:54, 2:48, 3:24} on
   disk) plus derived per-faction limits. Stock tech.tec has 512 records over canonical ids 0..255; localized
   titles do not prove source-building, tier, direction, or effect mappings.
   Cross-module calls: ModelDefinitionRegistry_FindByIdWithErrorCf [assets/model/definitions].
*/
void __thandor_void_preserve_eax_ecx_edx
TechnologyRuntime_RebuildDerivedLimitsAndCategoryMasks(void)

{
  ModelDefinitionRecordPrefix *pMVar1;
  uint technologyBitMask;
  int iVar2;
  int iVar3;
  ArmyAssetRecordPrefix **armyAssetRegistryCursor;
  undefined4 *categoryReciprocalCursor;
  TechnologyCategoryMasks *categoryMaskClearCursor;
  TechnologyRecord *technologyRecordCursor;
  ModelDefinitionLookupEaxCf5 MVar4;
  ArmyAssetRecordPrefix *armyAssetRecord;
  
  g_TechnologyCategoryMaximum0 = 1;
  g_TechnologyCategoryMaximum1 = 1;
  g_TechnologyCategoryMaximum2 = 1;
  g_TechnologyCategoryMaximum3 = 1;
  g_TechnologyCategoryMaximum4 = 1;
  g_TechnologyCategoryMaximum5 = 1;
  g_TechnologyCategoryMaximum6 = 1;
  g_TechnologyCategoryMaximum7 = 1;
  g_AiArmyCandidateFlaggedDefinitionValueMaximum = 0;
  armyAssetRegistryCursor = g_ArmyAssetRecordRegistry;
  iVar2 = 0x300;
  do {
    armyAssetRecord = *armyAssetRegistryCursor;
    if ((armyAssetRecord != (ArmyAssetRecordPrefix *)0x0) &&
       ((armyAssetRecord[1].selectionDetailTemplateVariantIndex & 1) != 0)) {
      MVar4 = ModelDefinitionRegistry_FindByIdWithErrorCf
                        (*(PckModelDefinitionIdCatalog *)
                          (armyAssetRecord->rootNodeOffsetOrPointer + 0x20));
      pMVar1 = MVar4.modelDefinition;
      if (!MVar4.carry) {
        if ((int)(&g_TechnologyCategoryMaximum0)[pMVar1[7].definitionId] < (int)pMVar1[8].byteSize)
        {
          (&g_TechnologyCategoryMaximum0)[pMVar1[7].definitionId] = pMVar1[8].byteSize;
        }
        if ((pMVar1[2].byteSize != 0) &&
           ((int)g_AiArmyCandidateFlaggedDefinitionValueMaximum < (int)pMVar1[1].byteSize)) {
          g_AiArmyCandidateFlaggedDefinitionValueMaximum = pMVar1[1].byteSize;
        }
      }
    }
    armyAssetRegistryCursor = armyAssetRegistryCursor + 1;
    iVar2 = iVar2 + -1;
  } while (iVar2 != 0);
  categoryReciprocalCursor = &g_TechnologyCategoryMaximumReciprocalQ24Table8;
  iVar2 = 8;
  do {
    *categoryReciprocalCursor = (int)(0x1000000 / (ulonglong)(uint)categoryReciprocalCursor[8]);
    categoryReciprocalCursor = categoryReciprocalCursor + 1;
    iVar2 = iVar2 + -1;
  } while (iVar2 != 0);
  categoryMaskClearCursor = &g_TechnologyCategoryMasks;
  for (iVar2 = 0x10; iVar2 != 0; iVar2 = iVar2 + -1) {
    categoryMaskClearCursor->category2[0] = 0;
    categoryMaskClearCursor = (TechnologyCategoryMasks *)(categoryMaskClearCursor->category2 + 1);
  }
  iVar2 = 0x100;
  technologyRecordCursor = g_TechnologyAsset->records;
  technologyBitMask = 1;
  iVar3 = 0;
  do {
    if (technologyRecordCursor->category == TECHNOLOGY_CATEGORY_C) {
      g_TechnologyCategoryMasks.category2[iVar3] =
           g_TechnologyCategoryMasks.category2[iVar3] | technologyBitMask;
    }
    if (technologyRecordCursor->category == TECHNOLOGY_CATEGORY_D) {
      g_TechnologyCategoryMasks.category3[iVar3] =
           g_TechnologyCategoryMasks.category3[iVar3] | technologyBitMask;
    }
    technologyRecordCursor = technologyRecordCursor + 1;
    technologyBitMask = technologyBitMask * 2;
    if (technologyBitMask == 0) {
      iVar3 = iVar3 + 1;
      technologyBitMask = 1;
    }
    iVar2 = iVar2 + -1;
  } while (iVar2 != 0);
  return;
}


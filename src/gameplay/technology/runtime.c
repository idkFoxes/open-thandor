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
void Technology_UnlockForFaction
               (GraphicsWorldCoordinateQ12 notificationXQ12,
               GraphicsWorldCoordinateQ12 notificationYQ12,TechnologyId technologyIndex,
               FactionRuntimeIndex factionIndex)

{
  UiNodeVtable *pUVar1;
  InGameRuntimeRootImageC3E4 *pIVar2;
  uint technologyBitMask;
  FactionRuntimeIndex extraout_ECX;
  FactionRuntimeIndex extraout_ECX_00;
  int extraout_ECX_01;
  int extraout_ECX_02;
  int factionIndex_00;
  UiNodeBase *extraout_EDX;
  UiNodeBase *node;
  UiNodeBase *node_00;
  undefined8 uVar3;
  uint *factionTechnologyMaskWord;
  TechnologyAsset *technologyAsset;
  
  pIVar2 = g_InGameRuntimeRoot;
  technologyAsset = g_TechnologyAsset;
  technologyBitMask = 1 << ((byte)technologyIndex & 0x1f);
  factionTechnologyMaskWord = (uint *)(factionIndex * 0x740 + 0x50fa20 + (technologyIndex >> 5) * 4)
  ;
  if ((*factionTechnologyMaskWord & technologyBitMask) == 0) {
    *factionTechnologyMaskWord = *factionTechnologyMaskWord | technologyBitMask;
    if (((g_UiCommandRuntimeFlags & 0x10) == 0) &&
       (factionIndex == (pIVar2->worldRuntime0A30).activeFactionRuntimeIndex)) {
      if ((notificationYQ12 == 0) && (notificationXQ12 == 0)) {
        InGameNotificationQueue_InsertPriorityRecord
                  (NONE,0,0,0,0,0,5,
                   g_TechnologyAsset->records[technologyIndex].completionMessageResourceId);
        factionIndex = extraout_ECX;
      }
      else {
        InGameNotificationQueue_InsertPriorityRecord
                  (TECHNOLOGY_UNLOCK_POSITION,0,0,0,notificationXQ12,notificationYQ12,5,
                   g_TechnologyAsset->records[technologyIndex].completionMessageResourceId);
        factionIndex = extraout_ECX_00;
      }
    }
    Technology_UnlockForFaction
              (0,0,technologyAsset->records[technologyIndex].dependencyTechnologyIndex,factionIndex)
    ;
    factionIndex_00 = extraout_ECX_01;
    node = extraout_EDX;
    for (pUVar1 = extraout_EDX[0x25].vtable; pUVar1 != (UiNodeVtable *)0x0;
        pUVar1 = pUVar1->method04) {
      if ((pUVar1[2].nonRightRelease == (UiNodeNonRightReleaseCallbackProc *)0x0) &&
         (factionIndex_00 == (*(ArmyRuntimeSlot **)(pUVar1[1].relocate + 8))->factionIndex)) {
        uVar3 = ModelRuntimeHierarchy_ApplyFactionTechnologyVariants
                          (factionIndex_00,node,factionIndex_00,
                           *(ArmyRuntimeSlot **)(pUVar1[1].relocate + 8));
        node = (UiNodeBase *)((ulonglong)uVar3 >> 0x20);
        factionIndex_00 = extraout_ECX_02;
      }
    }
    if (factionIndex_00 == node[0x23].bottom) {
      UiCatalogGroup48_RebuildGrid(node);
      UiCatalogGroup42_RebuildGrid(node_00);
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
undefined4
Technology_IsUnlockedForFactionCf
          (PckTechnologyIdCatalog technologyIndex,FactionRuntimeIndex factionIndex)

{
  undefined4 in_EAX;
  
  if ((*(uint *)(factionIndex * 0x740 + 0x50fa20 + (technologyIndex >> 5) * 4) &
      1 << ((byte)technologyIndex & 0x1f)) != 0) {
    return in_EAX;
  }
  return in_EAX;
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
void Technology_IsAvailableForFactionCf
               (PckTechnologyIdCatalog technologyIndex,FactionRuntimeIndex factionIndex)

{
  WorldRuntimeNode *worldNodeCursor;
  void *candidateEntityRuntime;
  
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
    worldNodeCursor = (g_InGameRuntimeRoot->worldRuntime0A30).ownerListHead;
    do {
      if (worldNodeCursor == (WorldRuntimeNode *)0x0) {
        return;
      }
      if (worldNodeCursor[2].common.nextNode == (WorldRuntimeNode *)0x0) {
        candidateEntityRuntime = worldNodeCursor->runtimePayload;
        if ((((*(uint *)((int)candidateEntityRuntime + 0xec) & 0x40) != 0) &&
            (factionIndex == *(int *)(*(int *)((int)candidateEntityRuntime + 8) + 0xc))) &&
           (technologyIndex == *(PckTechnologyIdCatalog *)((int)candidateEntityRuntime + 0x100))) {
          return;
        }
      }
      worldNodeCursor = (worldNodeCursor->common).nextNode;
    } while( true );
  }
  return;
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
void Technology_ApplyRecordToEntity
               (PckTechnologyIdCatalog technologyIndex,GameEntityRuntime *entity)

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
undefined8 __fastcall
TechnologyRuntime_RebuildDerivedLimitsAndCategoryMasks
          (dword registerContext,FactionRuntimeIndex factionIndex)

{
  undefined4 in_EAX;
  ModelDefinitionRecordPrefix *pMVar1;
  uint technologyBitMask;
  int iVar2;
  int extraout_ECX;
  int iVar3;
  ArmyAssetRecordPrefix **armyAssetRegistryCursor;
  undefined4 *categoryReciprocalCursor;
  TechnologyCategoryMasks *categoryMaskClearCursor;
  TechnologyRecord *technologyRecordCursor;
  bool bVar4;
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
       (bVar4 = false, (armyAssetRecord[1].selectionDetailTemplateVariantIndex & 1) != 0)) {
      pMVar1 = ModelDefinitionRegistry_FindByIdWithErrorCf
                         (*(PckModelDefinitionIdCatalog *)
                           (armyAssetRecord->rootNodeOffsetOrPointer + 0x20));
      iVar2 = extraout_ECX;
      if (!bVar4) {
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
  return CONCAT44(factionIndex,in_EAX);
}

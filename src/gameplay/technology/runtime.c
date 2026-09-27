/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/src/gameplay/technology/runtime.c
 * Reverse engineering by idkFoxes 2026
 */

#include <thandor/gameplay/technology/runtime.h>
#include <thandor/thandor.h>

/* Implementation ownership: gameplay/technology/runtime. */

/* Address: 0x005139C0.
   Unlocks a technology for a faction (once): sets its bit in the faction's 256-bit technology mask, announces it
   to the local player (at the given map position, if any), recursively unlocks the technology it depends on,
   applies the technology's model variants to the faction's models on the map and, for the local faction, rebuilds
   the two build-catalog grids.
*/
void __thandor_void_preserve_eax_ecx_edx
Technology_UnlockForFaction
          (GraphicsWorldCoordinateQ12 notificationXQ12,GraphicsWorldCoordinateQ12 notificationYQ12,
          TechnologyId technologyIndex,FactionRuntimeIndex factionIndex)

{
  WorldOwnerListNode100 *ownerNode;
  ArmyRuntimeSlot *modelRuntimeHolder;
  InGameRuntimeRootImageC3E4 *node;
  uint32_t technologyBitMask;
  uint32_t *factionTechnologyMaskWord;
  TechnologyAsset *technologyAsset;
  
  node = g_InGameRuntimeRoot;
  technologyAsset = g_TechnologyAsset;
  technologyBitMask = 1 << ((uint8_t)technologyIndex & 0x1f);
  /* the word of faction record +0x6E0 (technologyMasks256Bits) that holds the bit */
  factionTechnologyMaskWord = (uint32_t *)(factionIndex * 0x740 + THANDOR_ADDR(g_GameFactionRuntimeImage,0x6e0) + (technologyIndex >> 5) * 4)
  ;
  if ((*factionTechnologyMaskWord & technologyBitMask) == 0) {
    *factionTechnologyMaskWord = *factionTechnologyMaskWord | technologyBitMask;
    /* no announcement while the session still waits for its players */
    if (((g_UiCommandRuntimeFlags & UI_COMMAND_RUNTIME_FLAG_WAITING_FOR_PLAYERS) == 0) &&
       (factionIndex == (node->worldRuntime0A30).activeFactionRuntimeIndex)) {
      if ((notificationYQ12 == 0) && (notificationXQ12 == 0)) {
        InGameNotificationQueue_InsertPriorityRecord
                  (NOTIFICATION_PAYLOAD_NONE,0,0,0,0,0,5,
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
    for (ownerNode = (node->worldRuntime0A30).ownerListHead; ownerNode != NULL;
        ownerNode = ownerNode->nextNode) {
      if ((ownerNode->ownerClassId == WORLD_OWNER_RUNTIME_MODEL) &&
         (modelRuntimeHolder = *(ArmyRuntimeSlot **)((int)ownerNode->runtimePayload + 8),
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
Technology_IsUnlockedForFaction
          (PckTechnologyIdCatalog technologyIndex,FactionRuntimeIndex factionIndex)

{
  if ((*(uint32_t *)(factionIndex * 0x740 + THANDOR_ADDR(g_GameFactionRuntimeImage,0x6e0) + (technologyIndex >> 5) * 4) &
      1 << ((uint8_t)technologyIndex & 0x1f)) != 0) {
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
Technology_IsAvailableForFaction
          (PckTechnologyIdCatalog technologyIndex,FactionRuntimeIndex factionIndex)

{
  WorldRuntimeNode *worldNodeCursor;
  ArmyRuntimeSlot *activeResearchArmyRuntime;
  
  if (((((*(uint32_t *)(factionIndex * 0x740 + THANDOR_ADDR(g_GameFactionRuntimeImage,0x6e0) + (technologyIndex >> 5) * 4) &
         1 << ((uint8_t)technologyIndex & 0x1f)) == 0) &&
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
  uint32_t appliedEntityValue28;
  uint32_t recordEntityValue20;
  uint32_t recordEntityValue24;
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
   Recomputes values derived from the loaded army and technology files: per model category (0..7) the largest
   model value +0x60 and its Q24 reciprocal, the largest value +0x0C of models with a non-zero +0x18 (used by the
   AI army candidates), and the 256-bit masks of the technologies in categories 2 and 3.
*/
void __thandor_void_preserve_eax_ecx_edx
TechnologyRuntime_RebuildDerivedLimitsAndCategoryMasks(void)

{
  ModelDefinitionRecordPrefix *definitionRecord;
  uint32_t technologyBitMask;
  int remainingCount;
  int maskWordIndex;
  ArmyAssetRecordPrefix **armyAssetRegistryCursor;
  uint32_t *categoryReciprocalCursor;
  TechnologyCategoryMasks *categoryMaskClearCursor;
  TechnologyRecord *technologyRecordCursor;
  ModelDefinitionResult modelLookup;
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
  /* every registered army asset with flag +0x14 bit 0: look at its root model definition */
  armyAssetRegistryCursor = g_ArmyAssetRecordRegistry;
  remainingCount = 0x300;
  do {
    armyAssetRecord = *armyAssetRegistryCursor;
    if ((armyAssetRecord != NULL) &&
       ((armyAssetRecord[1].selectionDetailTemplateVariantIndex & 1) != 0)) {
      modelLookup = ModelDefinitionRegistry_FindByIdWithError
                        (*(PckModelDefinitionIdCatalog *)
                          (armyAssetRecord->rootNodeOffsetOrPointer + 0x20));
      definitionRecord = modelLookup.modelDefinition;
      if (!modelLookup.notFound) {
        /* model definition +0x5C category, +0x60 value; +0x18 flag, +0x0C value */
        if ((int)(&g_TechnologyCategoryMaximum0)[definitionRecord[7].definitionId] < (int)definitionRecord[8].byteSize)
        {
          (&g_TechnologyCategoryMaximum0)[definitionRecord[7].definitionId] = definitionRecord[8].byteSize;
        }
        if ((definitionRecord[2].byteSize != 0) &&
           ((int)g_AiArmyCandidateFlaggedDefinitionValueMaximum < (int)definitionRecord[1].byteSize)) {
          g_AiArmyCandidateFlaggedDefinitionValueMaximum = definitionRecord[1].byteSize;
        }
      }
    }
    armyAssetRegistryCursor++;
    remainingCount--;
  } while (remainingCount != 0);
  categoryReciprocalCursor = g_TechnologyCategoryMaximumReciprocalQ24Table8;
  remainingCount = 8;
  do {
    /* the eight category maxima follow the reciprocal table directly */
    *categoryReciprocalCursor = 0x1000000u / categoryReciprocalCursor[8];
    categoryReciprocalCursor++;
    remainingCount--;
  } while (remainingCount != 0);
  categoryMaskClearCursor = &g_TechnologyCategoryMasks;
  for (remainingCount = 0x10; remainingCount != 0; remainingCount--) {
    categoryMaskClearCursor->category2[0] = 0;
    categoryMaskClearCursor = (TechnologyCategoryMasks *)(categoryMaskClearCursor->category2 + 1);
  }
  /* one bit per technology record (256), category field at record +0x34 */
  remainingCount = 0x100;
  technologyRecordCursor = g_TechnologyAsset->records;
  technologyBitMask = 1;
  maskWordIndex = 0;
  do {
    if (technologyRecordCursor->category == TECHNOLOGY_CATEGORY_C) {
      g_TechnologyCategoryMasks.category2[maskWordIndex] =
           g_TechnologyCategoryMasks.category2[maskWordIndex] | technologyBitMask;
    }
    if (technologyRecordCursor->category == TECHNOLOGY_CATEGORY_D) {
      g_TechnologyCategoryMasks.category3[maskWordIndex] =
           g_TechnologyCategoryMasks.category3[maskWordIndex] | technologyBitMask;
    }
    technologyRecordCursor++;
    technologyBitMask = technologyBitMask * 2;
    if (technologyBitMask == 0) {
      maskWordIndex++;
      technologyBitMask = 1;
    }
    remainingCount--;
  } while (remainingCount != 0);
  return;
}


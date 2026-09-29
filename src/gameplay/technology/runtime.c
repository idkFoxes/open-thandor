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
void Technology_UnlockForFaction
          (GraphicsWorldCoordinateQ12 notificationXQ12,GraphicsWorldCoordinateQ12 notificationYQ12,
          TechnologyId technologyIndex,FactionRuntimeIndex factionIndex)

{
  WorldOwnerListNode *ownerNode;
  ArmyRuntimeSlot *modelRuntimeHolder;
  InGameRuntimeRoot *node;
  uint32_t technologyBitMask;
  uint32_t *factionTechnologyMaskWord;
  TechnologyAsset *technologyAsset;
  
  node = g_InGameRuntimeRoot;
  technologyAsset = g_TechnologyAsset;
  technologyBitMask = 1 << ((uint8_t)technologyIndex & 0x1f);
  /* the word of the faction's 256-bit technology mask that holds the bit */
  factionTechnologyMaskWord =
       &g_GameFactionRuntimeImage.records[factionIndex].technologyMasks256Bits[technologyIndex >> 5];
  if ((*factionTechnologyMaskWord & technologyBitMask) == 0) {
    *factionTechnologyMaskWord = *factionTechnologyMaskWord | technologyBitMask;
    /* no announcement while the session still waits for its players */
    if (((g_UiCommandRuntimeFlags & UI_COMMAND_RUNTIME_FLAG_WAITING_FOR_PLAYERS) == 0) &&
       (factionIndex == (node->worldRuntime).activeFactionRuntimeIndex)) {
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
    for (ownerNode = (node->worldRuntime).ownerListHead; ownerNode != NULL;
        ownerNode = ownerNode->nextNode) {
      if ((ownerNode->ownerClassId == WORLD_OWNER_RUNTIME_MODEL) &&
         (modelRuntimeHolder =
               ((ModelRuntimeSlot *)ownerNode->runtimePayload)->ownerArmyRuntimeOrSavedOffset.armyRuntime,
         factionIndex == modelRuntimeHolder->factionIndex)) {
        ModelRuntimeHierarchy_ApplyFactionTechnologyVariants(factionIndex,modelRuntimeHolder);
      }
    }
    if (factionIndex == (node->worldRuntime).activeFactionRuntimeIndex) {
      InGameBuildCatalog_RebuildGrid((UiNodeBase *)node);
      InGameSpecialBuildCatalog_RebuildGrid((UiNodeBase *)node);
    }
  }
  return;
}


/* Address: 0x00513AE0.
   Tests the technology's bit in the faction's 256-bit unlock mask (records[factionIndex].technologyMasks256Bits
   at +0x6E0). Note the inverted CF result: false (CF clear) when the technology is unlocked, true (CF set)
   when it is still locked.
*/
bool Technology_IsUnlockedForFaction(PckTechnologyIdCatalog technologyIndex,FactionRuntimeIndex factionIndex)

{
  if ((*(uint32_t *)(factionIndex * (int)sizeof(GameFactionRuntimeRecord) +
                    THANDOR_ADDR(g_GameFactionRuntimeImage,offsetof(GameFactionRuntimeRecord,technologyMasks256Bits)) +
                    (technologyIndex >> 5) * 4) &
      1 << ((uint8_t)technologyIndex & 0x1f)) != 0) {
    return false;
  }
  return true;
}


/* Address: 0x00513B20.
   Decides whether the faction may start researching a technology: it must still be locked, every bit of
   its eight prerequisite mask words must be unlocked for the faction, and no army
   of that faction may already be researching it. True (CF set) means available.
*/
bool Technology_IsAvailableForFaction(PckTechnologyIdCatalog technologyIndex,FactionRuntimeIndex factionIndex)

{
  WorldOwnerListNode *ownerNode;
  ModelRuntimeSlot *researchingModel;

  /* the first test reads the faction's unlock bit, as Technology_IsUnlockedForFaction does */
  if ((*(uint32_t *)(factionIndex * (int)sizeof(GameFactionRuntimeRecord) +
                    THANDOR_ADDR(g_GameFactionRuntimeImage,offsetof(GameFactionRuntimeRecord,technologyMasks256Bits)) +
                    (technologyIndex >> 5) * 4) &
       1 << ((uint8_t)technologyIndex & 0x1f)) == 0 &&
      (g_TechnologyAsset->records[technologyIndex].prerequisiteMasks[0] &
       g_GameFactionRuntimeImage.records[factionIndex].technologyMasks256Bits[0]) ==
       g_TechnologyAsset->records[technologyIndex].prerequisiteMasks[0] &&
      (g_TechnologyAsset->records[technologyIndex].prerequisiteMasks[1] &
       g_GameFactionRuntimeImage.records[factionIndex].technologyMasks256Bits[1]) ==
       g_TechnologyAsset->records[technologyIndex].prerequisiteMasks[1] &&
      (g_TechnologyAsset->records[technologyIndex].prerequisiteMasks[2] &
       g_GameFactionRuntimeImage.records[factionIndex].technologyMasks256Bits[2]) ==
       g_TechnologyAsset->records[technologyIndex].prerequisiteMasks[2] &&
      (g_TechnologyAsset->records[technologyIndex].prerequisiteMasks[3] &
       g_GameFactionRuntimeImage.records[factionIndex].technologyMasks256Bits[3]) ==
       g_TechnologyAsset->records[technologyIndex].prerequisiteMasks[3] &&
      (g_TechnologyAsset->records[technologyIndex].prerequisiteMasks[4] &
       g_GameFactionRuntimeImage.records[factionIndex].technologyMasks256Bits[4]) ==
       g_TechnologyAsset->records[technologyIndex].prerequisiteMasks[4] &&
      (g_TechnologyAsset->records[technologyIndex].prerequisiteMasks[5] &
       g_GameFactionRuntimeImage.records[factionIndex].technologyMasks256Bits[5]) ==
       g_TechnologyAsset->records[technologyIndex].prerequisiteMasks[5] &&
      (g_TechnologyAsset->records[technologyIndex].prerequisiteMasks[6] &
       g_GameFactionRuntimeImage.records[factionIndex].technologyMasks256Bits[6]) ==
       g_TechnologyAsset->records[technologyIndex].prerequisiteMasks[6] &&
      (g_TechnologyAsset->records[technologyIndex].prerequisiteMasks[7] &
       g_GameFactionRuntimeImage.records[factionIndex].technologyMasks256Bits[7]) ==
       g_TechnologyAsset->records[technologyIndex].prerequisiteMasks[7]) {
    /* scan the world's model owner nodes for an army of the faction that is already researching (runtime flag
       0x40) this technology (+0x100) */
    for (ownerNode = (g_InGameRuntimeRoot->worldRuntime).ownerListHead; ownerNode != NULL;
        ownerNode = ownerNode->nextNode) {
      if (ownerNode->ownerClassId == WORLD_OWNER_RUNTIME_MODEL) {
        researchingModel = ownerNode->runtimePayload;
        if ((researchingModel->classState.stateFlags & ENTITY_RUNTIME_FLAG_RESEARCH_RUNNING) != 0 &&
            factionIndex == researchingModel->ownerArmyRuntimeOrSavedOffset.armyRuntime->factionIndex &&
            technologyIndex == researchingModel->researchTechnologyId) {
          return false;
        }
      }
    }
    return true;
  }
  return false;
}


/* Address: 0x0052AE10.
   Starts researching a technology in a building: unless a research is already assigned or running, stores the
   technology index and copies the tech.tec record's Xenite cost (+0x20), energy cost (+0x24) and duration
   (+0x28) into the entity; the HUD shows these as the research costs and time. The fast-build cheat divides the
   duration by 16.
*/
void Technology_ApplyRecordToEntity(PckTechnologyIdCatalog technologyIndex,GameEntityRuntime *entity)

{
  uint32_t researchDurationQ5;
  uint32_t xeniteCostQ4;
  uint32_t energyCostQ4;
  GameEntityRuntimeFlags *entityRuntimeFlags;

  if (((entity->common).runtimeFlags &
      (ENTITY_RUNTIME_FLAG_RESEARCH_RUNNING | ENTITY_RUNTIME_FLAG_RESEARCH_ASSIGNED)) == 0) {
    xeniteCostQ4 = g_TechnologyAsset->records[technologyIndex].xeniteCostQ4;
    energyCostQ4 = g_TechnologyAsset->records[technologyIndex].energyCostQ4;
    researchDurationQ5 = g_TechnologyAsset->records[technologyIndex].researchDurationQ5;
    (entity->classPayload).technology.entityValue24 = 0;
    (entity->common).commandState = technologyIndex;
    if ((g_UiCommandRuntimeFlags & UI_COMMAND_RUNTIME_FLAG_CHEAT_FAST_BUILD) != 0) {
      researchDurationQ5 = (researchDurationQ5 >> 4) + 1;
    }
    (entity->classPayload).technology.xeniteCostQ4 = xeniteCostQ4;
    (entity->classPayload).technology.energyCostQ4 = energyCostQ4;
    (entity->classPayload).technology.appliedResearchDurationQ5 = researchDurationQ5;
    entityRuntimeFlags = &(entity->common).runtimeFlags;
    *entityRuntimeFlags = *entityRuntimeFlags | ENTITY_RUNTIME_FLAG_RESEARCH_ASSIGNED;
  }
  return;
}


/* Address: 0x00539BB0.
   Recomputes values derived from the loaded army and technology files: per model category (0..7) the largest
   model value +0x60 and its Q24 reciprocal, the largest value +0x0C of models with a non-zero +0x18 (used by the
   AI army candidates), and the 256-bit masks of the technologies in categories 2 and 3.
*/
void TechnologyRuntime_RebuildDerivedLimitsAndCategoryMasks(void)

{
  ModelDefinition *definitionRecord;
  uint32_t technologyBitMask;
  int remainingCount;
  int maskWordIndex;
  ArmyAssetRecordPrefix **armyAssetRegistryCursor;
  uint32_t *categoryReciprocalCursor;
  uint32_t *categoryMaskClearCursor;
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
  for (remainingCount = ARMY_ASSET_REGISTRY_SLOT_COUNT; remainingCount != 0; remainingCount--) {
    armyAssetRecord = *armyAssetRegistryCursor;
    if ((armyAssetRecord != NULL) &&
       ((((ArmyAssetRecord *)armyAssetRecord)->flags & 1) != 0)) {
      /* the root node's model definition id (+0x20) */
      modelLookup = ModelDefinitionRegistry_FindByIdWithError
                        (*(PckModelDefinitionIdCatalog *)
                          (armyAssetRecord->rootNodeOffsetOrPointer + 0x20));
      definitionRecord = (ModelDefinition *)modelLookup.modelDefinition;
      if (!modelLookup.notFound) {
        /* per target class (+0x5C) the largest armour (+0x60); for mobile models (+0x18) the top speed (+0x0C) */
        if ((int)(&g_TechnologyCategoryMaximum0)[definitionRecord->targetClassIndex] <
            (int)definitionRecord->maximumHealth) {
          (&g_TechnologyCategoryMaximum0)[definitionRecord->targetClassIndex] = definitionRecord->maximumHealth;
        }
        if ((definitionRecord->accelerationPerTick != 0) &&
           ((int)g_AiArmyCandidateFlaggedDefinitionValueMaximum < definitionRecord->movementSpeed)) {
          g_AiArmyCandidateFlaggedDefinitionValueMaximum = definitionRecord->movementSpeed;
        }
      }
    }
    armyAssetRegistryCursor++;
  }
  /* the eight category maxima follow the reciprocal table directly */
  categoryReciprocalCursor = g_TechnologyCategoryMaximumReciprocalQ24Table8;
  for (remainingCount = 8; remainingCount != 0; remainingCount--) {
    *categoryReciprocalCursor = 0x1000000u / categoryReciprocalCursor[8];
    categoryReciprocalCursor++;
  }
  /* clear both category masks (2 x 8 dwords) */
  categoryMaskClearCursor = g_TechnologyCategoryMasks.category2;
  for (remainingCount = 0x10; remainingCount != 0; remainingCount--) {
    *categoryMaskClearCursor = 0;
    categoryMaskClearCursor++;
  }
  /* one bit per technology record, category field at record +0x34 */
  remainingCount = TECHNOLOGY_RECORD_COUNT;
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


/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/src/gameplay/technology/runtime.cpp
 * Reverse engineering by idkFoxes 2026
 */

#include <thandor/gameplay/technology/runtime.h>
#include <thandor/thandor.h>

/* Module data. */

int32_t g_TechnologyCategoryMaximums[8] = {0, 0, 0, 0, 0, 0, 0, 0};

int32_t g_AiArmyCandidateFlaggedDefinitionValueMaximum = 0;

TechnologyCategoryMasks g_TechnologyCategoryMasks = {0};

static uint32_t g_TechnologyCategoryMaximumReciprocalQ24Table8[8] = {0};

TechnologyAsset *g_TechnologyAsset = 0;

/* Implementation ownership: gameplay/technology/runtime. */

/* Unlocks a technology for a faction (once): sets its bit in the faction's 256-bit technology mask, announces it
   to the local player (at the given map position, if any), recursively unlocks the technology it depends on,
   applies the technology's model variants to the faction's models on the map and, for the local faction, rebuilds
   the two build-catalog grids.
*/
void Technology_UnlockForFaction
          (GraphicsWorldCoordinateQ12 notificationXQ12,GraphicsWorldCoordinateQ12 notificationYQ12,
          TechnologyId technologyIndex,FactionRuntimeIndex factionIndex)

{
  WorldOwnerListNode *ownerNode;
  ArmyRuntimeSlot *ownerArmy;
  InGameRuntimeRoot *root;
  uint32_t technologyBitMask;
  uint32_t *factionTechnologyMaskWord;
  TechnologyAsset *technologyAsset;

  root = g_InGameRuntimeRoot;
  technologyAsset = g_TechnologyAsset;
  technologyBitMask = 1 << ((uint8_t)technologyIndex & 31);
  /* the word of the faction's 256-bit technology mask that holds the bit */
  factionTechnologyMaskWord =
       &g_GameFactionRuntimeImage.records[factionIndex].technologyMasks256Bits[technologyIndex >> 5];
  if ((*factionTechnologyMaskWord & technologyBitMask) != 0) {
    return;
  }
  *factionTechnologyMaskWord = *factionTechnologyMaskWord | technologyBitMask;
  /* no announcement while the session still waits for its players */
  if (((g_UiCommandRuntimeFlags & UI_COMMAND_RUNTIME_FLAG_WAITING_FOR_PLAYERS) == 0) &&
     (factionIndex == (root->worldRuntime).activeFactionRuntimeIndex)) {
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
  /* the dependency is unlocked without a position; the recursion ends at an already unlocked technology */
  Technology_UnlockForFaction
            (0,0,technologyAsset->records[technologyIndex].dependencyTechnologyIndex,factionIndex);
  for (ownerNode = (root->worldRuntime).ownerListHead; ownerNode != NULL;
      ownerNode = ownerNode->nextNode) {
    if (ownerNode->ownerClassId == WORLD_OWNER_RUNTIME_MODEL) {
      ownerArmy = ((ModelRuntimeSlot *)ownerNode->runtimePayload)->ownerArmyRuntimeOrSavedOffset.armyRuntime;
      if (factionIndex == ownerArmy->factionIndex) {
        ModelRuntimeHierarchy_ApplyFactionTechnologyVariants(factionIndex,ownerArmy);
      }
    }
  }
  if (factionIndex == (root->worldRuntime).activeFactionRuntimeIndex) {
    InGameBuildCatalog_RebuildGrid((UiNodeBase *)root);
    InGameSpecialBuildCatalog_RebuildGrid((UiNodeBase *)root);
  }
}


/* Tests the technology's bit in the faction's 256-bit unlock mask (records[factionIndex].technologyMasks256Bits).
   Note the inverted result: false when the technology is unlocked, true when it is still locked.
*/
Bool8 Technology_IsUnlockedForFaction(PckTechnologyIdCatalog technologyIndex,FactionRuntimeIndex factionIndex)

{
  if ((*(uint32_t *)(factionIndex * (int)sizeof(GameFactionRuntimeRecord) +
                    THANDOR_ADDR(g_GameFactionRuntimeImage,offsetof(GameFactionRuntimeRecord,technologyMasks256Bits)) +
                    (technologyIndex >> 5) * 4) &
      1 << ((uint8_t)technologyIndex & 31)) != 0) {
    return false;
  }
  return true;
}


/* Decides whether the faction may start researching a technology: it must still be locked, every bit of
   its eight prerequisite mask words must be unlocked for the faction, and no army
   of that faction may already be researching it. True means available.
*/
Bool8 Technology_IsAvailableForFaction(PckTechnologyIdCatalog technologyIndex,FactionRuntimeIndex factionIndex)

{
  WorldOwnerListNode *ownerNode;
  ModelRuntimeSlot *researchingModel;
  const uint32_t *factionTechnologyMasks;
  const uint32_t *prerequisiteMasks;
  int maskWordIndex;

  factionTechnologyMasks = g_GameFactionRuntimeImage.records[factionIndex].technologyMasks256Bits;
  /* already unlocked: not available */
  if ((factionTechnologyMasks[technologyIndex >> 5] & 1 << ((uint8_t)technologyIndex & 31)) != 0) {
    return false;
  }
  /* every prerequisite bit must be unlocked for the faction */
  prerequisiteMasks = g_TechnologyAsset->records[technologyIndex].prerequisiteMasks;
  for (maskWordIndex = 0; maskWordIndex < 8; maskWordIndex++) {
    if ((prerequisiteMasks[maskWordIndex] & factionTechnologyMasks[maskWordIndex]) !=
        prerequisiteMasks[maskWordIndex]) {
      return false;
    }
  }
  /* scan the world's model owner nodes for an army of the faction that is already researching (runtime flag
     0x40) this technology (researchTechnologyId) */
  for (ownerNode = (g_InGameRuntimeRoot->worldRuntime).ownerListHead; ownerNode != NULL;
      ownerNode = ownerNode->nextNode) {
    if (ownerNode->ownerClassId == WORLD_OWNER_RUNTIME_MODEL) {
      researchingModel = (ModelRuntimeSlot *)ownerNode->runtimePayload;
      if ((researchingModel->classState.stateFlags & ENTITY_RUNTIME_FLAG_RESEARCH_RUNNING) != 0 &&
          factionIndex == researchingModel->ownerArmyRuntimeOrSavedOffset.armyRuntime->factionIndex &&
          technologyIndex == researchingModel->researchTechnologyId) {
        return false;
      }
    }
  }
  return true;
}


/* Starts researching a technology in a building: unless a research is already assigned or running, stores the
   technology index and copies the tech.tec record's Xenite cost, energy cost and research duration into the
   entity; the HUD shows these as the research costs and time. The fast-build cheat divides the
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


/* Recomputes values derived from the loaded army and technology files: per model category (0..7) the largest
   maximumHealth and its Q24 reciprocal, the largest movementSpeed of models with a non-zero accelerationPerTick
   (used by the AI army candidates), and the 256-bit masks of the technologies in categories 2 and 3.
*/
void TechnologyRuntime_RebuildDerivedLimitsAndCategoryMasks(void)

{
  ModelDefinition *definitionRecord;
  int registrySlotIndex;
  int categoryIndex;
  int maskWordIndex;
  int technologyIndex;
  uint32_t technologyBitMask;
  const TechnologyRecord *technologyRecord;
  ArmyAssetRecordPrefix *armyAssetRecord;

  g_TechnologyCategoryMaximums[0] = 1;
  g_TechnologyCategoryMaximums[1] = 1;
  g_TechnologyCategoryMaximums[2] = 1;
  g_TechnologyCategoryMaximums[3] = 1;
  g_TechnologyCategoryMaximums[4] = 1;
  g_TechnologyCategoryMaximums[5] = 1;
  g_TechnologyCategoryMaximums[6] = 1;
  g_TechnologyCategoryMaximums[7] = 1;
  g_AiArmyCandidateFlaggedDefinitionValueMaximum = 0;
  /* every registered army asset with flags bit 0: look at its root model definition */
  for (registrySlotIndex = 0; registrySlotIndex < ARMY_ASSET_REGISTRY_SLOT_COUNT; registrySlotIndex++) {
    armyAssetRecord = g_ArmyAssetRecordRegistry[registrySlotIndex];
    if ((armyAssetRecord != NULL) &&
       ((((ArmyAssetRecord *)armyAssetRecord)->flags & 1) != 0)) {
      /* the root node's model definition id (ArmyModelTreeNode.linkedDefinitionIds[0]) */
      definitionRecord = (ModelDefinition *)ModelDefinitionRegistry_FindById
                        (*(PckModelDefinitionIdCatalog *)
                          (armyAssetRecord->rootNodeOffsetOrPointer + 32)); /* 5f-format: ArmyAssetRecord.rootNodeOffsetOrPointer */
      if (definitionRecord != NULL) {
        /* per target class (targetClassIndex) the largest armour (maximumHealth); for mobile models
           (accelerationPerTick) the top speed (movementSpeed) */
        if ((int)g_TechnologyCategoryMaximums[definitionRecord->targetClassIndex] <
            (int)definitionRecord->maximumHealth) {
          g_TechnologyCategoryMaximums[definitionRecord->targetClassIndex] = definitionRecord->maximumHealth;
        }
        if ((definitionRecord->accelerationPerTick != 0) &&
           ((int)g_AiArmyCandidateFlaggedDefinitionValueMaximum < definitionRecord->movementSpeed)) {
          g_AiArmyCandidateFlaggedDefinitionValueMaximum = definitionRecord->movementSpeed;
        }
      }
    }
  }
  /* Q24 reciprocal of each category maximum (the maxima start at 1, so no division by zero) */
  for (categoryIndex = 0; categoryIndex < 8; categoryIndex++) {
    g_TechnologyCategoryMaximumReciprocalQ24Table8[categoryIndex] =
         TECHNOLOGY_RECIPROCAL_Q24_ONE / (uint32_t)g_TechnologyCategoryMaximums[categoryIndex];
  }
  /* clear both category masks */
  for (maskWordIndex = 0; maskWordIndex < 8; maskWordIndex++) {
    g_TechnologyCategoryMasks.category2[maskWordIndex] = 0;
  }
  for (maskWordIndex = 0; maskWordIndex < 8; maskWordIndex++) {
    g_TechnologyCategoryMasks.category3[maskWordIndex] = 0;
  }
  /* one bit per technology record, by its category field */
  for (technologyIndex = 0; technologyIndex < TECHNOLOGY_RECORD_COUNT; technologyIndex++) {
    technologyRecord = &g_TechnologyAsset->records[technologyIndex];
    maskWordIndex = technologyIndex >> 5;
    technologyBitMask = 1u << (technologyIndex & 31);
    if (technologyRecord->category == TECHNOLOGY_CATEGORY_C) {
      g_TechnologyCategoryMasks.category2[maskWordIndex] =
           g_TechnologyCategoryMasks.category2[maskWordIndex] | technologyBitMask;
    }
    if (technologyRecord->category == TECHNOLOGY_CATEGORY_D) {
      g_TechnologyCategoryMasks.category3[maskWordIndex] =
           g_TechnologyCategoryMasks.category3[maskWordIndex] | technologyBitMask;
    }
  }
}


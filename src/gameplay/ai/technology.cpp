/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/src/gameplay/ai/technology.cpp
 * Reverse engineering by idkFoxes 2026
 */

#include <thandor/gameplay/ai/technology.h>
#include <thandor/thandor.h>

/* Implementation ownership: gameplay/ai/technology. */

/* Technology score callback for score kind 3 (g_AiTechnologyCandidateScoreCallbackTable[3]; technologies
   researched in class 13/22 structures). Looks up the model definition that
   ModelDefinitionRegistry_FindByRuntimeClassId finds for the technology id and scores the technology by
   how many own units share that definition's family (AiTechnologyCompatibility_ComputeAverageRuntimeRelationScaleQ8).
   Definitions of class 1 only score while the faction has none of ARM 302/303/304, at half weight, and at
   3/8 when it has ARM 305 or 306.
*/
AiTechnologyCandidateScore AiTechnologyScore_ComputeRuntimeClassCompatibleCandidateValue
          (FactionRuntimeIndex factionIndex,PckTechnologyIdCatalog technologyId,
          WorldRuntimeContext *worldRuntime)

{
  uint32_t runtimeClassId;
  TechnologyAsset *technologyAsset;
  ModelDefinitionRecordPrefix *candidateDefinition;
  UQ8 relationScaleQ8;
  uint32_t candidateScore;
  Bool8 rejected;

  candidateDefinition = ModelDefinitionRegistry_FindByRuntimeClassId(technologyId);
  technologyAsset = g_TechnologyAsset;
  if (candidateDefinition == nullptr) {
    return 0;
  }
  runtimeClassId = ((ModelDefinition *)candidateDefinition)->runtimeClassId;
  if (runtimeClassId != MODEL_RUNTIME_CLASS_01_GROUND) {
    if (runtimeClassId != MODEL_RUNTIME_CLASS_21_AIRCRAFT && runtimeClassId != MODEL_RUNTIME_CLASS_02_TRACKED &&
        runtimeClassId != MODEL_RUNTIME_CLASS_03_ARTICULATED_WALKER &&
        runtimeClassId != MODEL_RUNTIME_CLASS_17_DEPLOYING_GLIDER &&
        runtimeClassId != MODEL_RUNTIME_CLASS_19_WATER_SURFACE) {
      /* Original quirk: the original returns the definition pointer here, so every other class "scores"
         with its record address (on x64 its low 32 bits). */
      return (AiTechnologyCandidateScore)(intptr_t)candidateDefinition;
    }
    rejected = AiTechnologyCompatibility_AcceptRuntimeClassCandidate(factionIndex,candidateDefinition);
    if (rejected) {
      return 0;
    }
    relationScaleQ8 = AiTechnologyCompatibility_ComputeAverageRuntimeRelationScaleQ8(candidateDefinition);
    return (relationScaleQ8 * AI_TECHNOLOGY_RELATION_SCORE_FACTOR >> 8) *
           technologyAsset->records[technologyId].baseCandidateScore >> 8;
  }
  /* class 1 (ground) definitions: nothing once the faction has any of ARM 302/303/304 */
  if (AiPrimaryWorkspace_HasEntryById(ARM_0302_BUILDING_MDL0300) ||
      AiPrimaryWorkspace_HasEntryById(ARM_0303_BUILDING_MDL0316) ||
      AiPrimaryWorkspace_HasEntryById(ARM_0304_BUILDING_MDL0324)) {
    return 0;
  }
  rejected = AiTechnologyCompatibility_AcceptRuntimeClassCandidate(factionIndex,candidateDefinition);
  if (rejected) {
    return 0;
  }
  relationScaleQ8 = AiTechnologyCompatibility_ComputeAverageRuntimeRelationScaleQ8(candidateDefinition);
  candidateScore = (relationScaleQ8 * AI_TECHNOLOGY_RELATION_SCORE_FACTOR >> 8) *
                   technologyAsset->records[technologyId].baseCandidateScore >> 8;
  if (AiPrimaryWorkspace_HasEntryById(ARM_0305_BUILDING_MDL0317) ||
      AiPrimaryWorkspace_HasEntryById(ARM_0306_BUILDING_MDL0310)) {
    candidateScore = candidateScore * 3 >> 2;
  }
  return candidateScore >> 1;
}


/* Returns false when the AI may plan this technology: no workspace-00 structure is already working
   on it (runtimeFlags & 0xC0 with the technology id in researchTechnologyId), the faction
   (factionRecordOffset = faction * 0x740) has not unlocked it yet, and all eight prerequisite mask words are
   covered by its unlocked technologies.
   Note the inverted sense despite the name: true means NOT available (callers rely on it).
*/
Bool8 AiTechnologyCandidate_IsCurrentlyAvailable
          (PckTechnologyIdCatalog technologyIndex,FactionRuntimeRecordByteOffset factionRecordOffset
          )

{
  int remainingCount;
  int maskIndex;
  AiStructureWorkspaceEntry *workspaceEntry;
  ModelRuntimeSlot *structureRuntime;
  const GameFactionRuntimeRecord *factionRecord;
  const TechnologyRecord *technologyRecord;

  workspaceEntry = g_AiWorkspace00Structures;
  for (remainingCount = g_AiWorkspace00Count; remainingCount != 0; remainingCount--) {
    structureRuntime = (ModelRuntimeSlot *)workspaceEntry->runtimeSlotAddressOrZero;
    if (structureRuntime != nullptr &&
        (structureRuntime->classState.stateFlags &
         (ENTITY_RUNTIME_FLAG_RESEARCH_RUNNING | ENTITY_RUNTIME_FLAG_RESEARCH_ASSIGNED)) != 0 &&
        technologyIndex == structureRuntime->researchTechnologyId) {
      return true;
    }
    workspaceEntry++;
  }
  factionRecord = (const GameFactionRuntimeRecord *)((uint8_t *)g_GameFactionRuntimeImage.records +
                                                     factionRecordOffset);
  /* already unlocked (bit in the faction record's technologyMasks256Bits) */
  if ((factionRecord->technologyMasks256Bits[technologyIndex >> 5] & 1 << ((uint8_t)technologyIndex & 31)) != 0) {
    return true;
  }
  technologyRecord = &g_TechnologyAsset->records[technologyIndex];
  for (maskIndex = 0; maskIndex < 8; maskIndex++) {
    if ((technologyRecord->prerequisiteMasks[maskIndex] & factionRecord->technologyMasks256Bits[maskIndex]) !=
        technologyRecord->prerequisiteMasks[maskIndex]) {
      return true;
    }
  }
  return false;
}


/* Score kind of a new technology candidate (see AiTechnologyPlanning_AddCandidateRecord); the first match
   decides. */
static AiTechnologyCandidateScoreKind AiTechnologyPlanning_SelectScoreKind
          (PckTechnologyIdCatalog technologyId,const MdlDefinitionSemanticPrefix *sourceArmyModelDefinition)
{
  if (technologyId == TEC_216_WALL || technologyId == TEC_217_HIGH_WALL) {
    return AI_TECHNOLOGY_SCORE_DEFAULT_ZERO;
  }
  if (technologyId == TEC_210_IMPROVE_XENITE_MINE_1 || technologyId == TEC_211_IMPROVE_XENITE_MINE_2 ||
      technologyId == TEC_213_IMPROVE_TRITIUM_PUMP_1 || technologyId == TEC_214_IMPROVE_TRITIUM_PUMP_2) {
    return AI_TECHNOLOGY_SCORE_FACTION_SCALED;
  }
  if (sourceArmyModelDefinition->runtimeClassId == MODEL_RUNTIME_CLASS_11) {
    return AI_TECHNOLOGY_SCORE_BASE_VALUE_KIND2;
  }
  if (sourceArmyModelDefinition->runtimeClassId == MODEL_RUNTIME_CLASS_13 ||
      sourceArmyModelDefinition->runtimeClassId == MODEL_RUNTIME_CLASS_22) {
    return AI_TECHNOLOGY_SCORE_RUNTIME_CLASS_COMPATIBLE;
  }
  if (technologyId == TEC_183_RADAR || technologyId == TEC_184_RADAR_RANGE_PLUS_10_PERCENT ||
      technologyId == TEC_185_RADAR_RANGE_PLUS_10_PERCENT || technologyId == TEC_189_AR_MINUS_M_SILO ||
      technologyId == TEC_190_AR_MINUS_M_SILO_RANGE_PLUS_10_PERCENT ||
      technologyId == TEC_191_AR_MINUS_M_SILO_RANGE_PLUS_10_PERCENT) {
    return AI_TECHNOLOGY_SCORE_BASE_VALUE_KIND4;
  }
  return AI_TECHNOLOGY_SCORE_CATEGORY_COMPATIBLE;
}


/* Appends a technology that the source structure can research to the technology candidate list (workspace 12,
   at most 32; the pioneer vehicle is never added) and picks its score function: walls 0, mine and pump
   improvements 1, source of runtime class 11 2, of class 13 or 22 3, radar and AR-M silo technologies 4,
   everything else 5.
*/
void AiTechnologyPlanning_AddCandidateRecord(ModelRuntimeSlot *sourceModelRuntime,PckTechnologyIdCatalog technologyId)

{
  AiTechnologyPlanningCandidate *candidate;

  if (g_AiWorkspace12Count >= AI_WORKSPACE12_CAPACITY || technologyId == TEC_011_PIONEER_VEHICLE) {
    return;
  }
  candidate = &g_AiWorkspace12TechnologyCandidates[g_AiWorkspace12Count];
  candidate->technologyId00 = technologyId;
  candidate->sourceModelRuntime04 = sourceModelRuntime;
  g_AiWorkspace12Count++;
  candidate->scoreKind08 = AiTechnologyPlanning_SelectScoreKind
                             (technologyId,
                              (MdlDefinitionSemanticPrefix *)
                              sourceModelRuntime->definitionOrSavedId.runtimeDefinition);
}


/* Technology score callback for score kind 1 (g_AiTechnologyCandidateScoreCallbackTable[1]; xenite-mine and
   tritium-pump improvements). Nothing while the faction's xenite is below the
   ki.dat minimum; mine improvements then score their base value, pump improvements their base value
   scaled by energy demand / energy supply (Q8), but only once demand reaches 0xF0/0x100 (about 94%)
   of supply.
*/
AiTechnologyCandidateScore AiTechnologyScore_ComputeFactionScaledCandidateValue
          (FactionRuntimeIndex factionIndex,PckTechnologyIdCatalog technologyId,
          WorldRuntimeContext *worldRuntime)

{
  /* the original receives g_AiKnowledgeData from its caller outside the argument list; it is read directly */
  AiKnowledgeDataImage *knowledgeData = g_AiKnowledgeData;
  EnergyDemandQ4 totalEnergyDemandQ4;
  UQ8 energyDemandPressureRatioQ8;
  
  if ((int)(knowledgeData->parameters).factionScaledTechnologyMinimumXeniteQ4 <=
      (int)g_GameFactionRuntimeImage.records[factionIndex].xeniteCurrentQ4) {
    if ((technologyId == TEC_210_IMPROVE_XENITE_MINE_1) ||
       (technologyId == TEC_211_IMPROVE_XENITE_MINE_2)) {
      return g_TechnologyAsset->records[technologyId].baseCandidateScore;
    }
    totalEnergyDemandQ4 =
         g_GameFactionRuntimeImage.records[factionIndex].suppliedEnergyDemandQ4 +
         g_GameFactionRuntimeImage.records[factionIndex].unpoweredEnergyDemandQ4;
    energyDemandPressureRatioQ8 =
         (UQ8)(((uint64_t)(totalEnergyDemandQ4 >> 24) << 32 |
               (uint64_t)totalEnergyDemandQ4 * Q8_ONE & UINT32_MAX) /
              (uint64_t)
              (g_GameFactionRuntimeImage.records[factionIndex].tritiumExtractionRateQ4PerTick * 16
              + g_GameFactionRuntimeImage.records[factionIndex].baselineEnergySupplyQ4));
    /* the original divides without a zero check: a faction with no energy supply at all faults here */
    if (239 < (int)energyDemandPressureRatioQ8) {
      return energyDemandPressureRatioQ8 *
             g_TechnologyAsset->records[technologyId].baseCandidateScore >> 8;
    }
  }
  return 0;
}


/* Technology score callback for score kind 2 (g_AiTechnologyCandidateScoreCallbackTable[2]; technologies
   researched in class 11 structures): the technology's base candidate score
   from the technology asset, unconditionally.
*/
AiTechnologyCandidateScore AiTechnologyScore_ReturnBaseCandidateValueForKind2
          (FactionRuntimeIndex factionIndex,PckTechnologyIdCatalog technologyId,
          WorldRuntimeContext *worldRuntime)

{
  return g_TechnologyAsset->records[technologyId].baseCandidateScore;
}


/* Technology score callback for score kind 4 (g_AiTechnologyCandidateScoreCallbackTable[4]; radar and AR-M silo
   technologies): the technology's base candidate score, unconditionally.
*/
AiTechnologyCandidateScore AiTechnologyScore_ReturnBaseCandidateValueForKind4
          (FactionRuntimeIndex factionIndex,PckTechnologyIdCatalog technologyId,
          WorldRuntimeContext *worldRuntime)

{
  return g_TechnologyAsset->records[technologyId].baseCandidateScore;
}


/* The category mask for the score callback below; AiTechnologyCandidate_AddBestResearch stores it
   here before its scoring loop. */
AiTechnologyCategoryMask g_AiTechnologyScoreCategoryMask;

AiTechnologyCandidateScore AiTechnologyScore_ComputeCategoryCompatibleCandidateValue_Body
          (AiTechnologyCategoryMask categoryMask,FactionRuntimeIndex factionIndex,
          PckTechnologyIdCatalog technologyId,WorldRuntimeContext *worldRuntime);

/* Technology score callback for score kind 5 (g_AiTechnologyCandidateScoreCallbackTable[5]; every technology
   not caught by kinds 0-4). Hands the category mask (a global set by the caller) to the body below, which holds
   the original logic.
*/
AiTechnologyCandidateScore AiTechnologyScore_ComputeCategoryCompatibleCandidateValue
          (FactionRuntimeIndex factionIndex,PckTechnologyIdCatalog technologyId,
          WorldRuntimeContext *worldRuntime)

{
  return AiTechnologyScore_ComputeCategoryCompatibleCandidateValue_Body
                   (g_AiTechnologyScoreCategoryMask,factionIndex,technologyId,worldRuntime);
}

/* Body of AiTechnologyScore_ComputeCategoryCompatibleCandidateValue (a C-only split; only called by the wrapper
   above).
   Category C and D technologies score nothing unless the faction already owns a technology of that
   category (bit 2 / bit 4 of categoryMask). Otherwise the score is the average faction-weighted score
   of the army assets the technology leads to (weights g_AiArmyCandidateScoreWeightsVariantC15) times the
   base candidate score, >> 8.
*/
AiTechnologyCandidateScore AiTechnologyScore_ComputeCategoryCompatibleCandidateValue_Body
          (AiTechnologyCategoryMask categoryMask,FactionRuntimeIndex factionIndex,
          PckTechnologyIdCatalog technologyId,WorldRuntimeContext *worldRuntime)

{
  TechnologyCategory technologyCategory;
  uint32_t categoryMaskBit;
  TechnologyAsset *technologyAsset;
  AiCandidateScore32 averageAssetScore;
  
  technologyAsset = g_TechnologyAsset;
  technologyCategory = g_TechnologyAsset->records[technologyId].category;
  /* Categories C and D need their bit in the category mask; every other category is always compatible. */
  if (technologyCategory == TECHNOLOGY_CATEGORY_C) {
    categoryMaskBit = categoryMask & 2;
    if (categoryMaskBit == 0) {
      return 0;
    }
  }
  else if (technologyCategory == TECHNOLOGY_CATEGORY_D) {
    categoryMaskBit = categoryMask & 4;
    if (categoryMaskBit == 0) {
      return 0;
    }
  }
  averageAssetScore = AiArmyCandidate_ComputeAverageCompatibleAssetScore
                    (&g_AiArmyCandidateScoreWeightsVariantC15,factionIndex,technologyId);
  return (uint32_t)(averageAssetScore * technologyAsset->records[technologyId].baseCandidateScore) >> 8;
}


/* Veto hook of AiTechnologyScore_ComputeRuntimeClassCompatibleCandidateValue (its only caller, called
   directly): would return true to reject the candidate definition, but always accepts (returns false).
*/
Bool8 AiTechnologyCompatibility_AcceptRuntimeClassCandidate
          (FactionRuntimeIndex factionIndex,ModelDefinitionRecordPrefix *candidateDefinition)

{
  return false;
}


/* Relation scale (Q8) of a candidate definition to the faction's units in the secondary workspace
   (workspace 01): (1.0 + 2.0 per assigned unit whose definition id equals the candidate's or differs by
   1000 or 2000, i.e. the same unit in another id block) / number of assigned units; 1.0 when there is
   none. Called directly by AiTechnologyScore_ComputeRuntimeClassCompatibleCandidateValue.
*/
UQ8 AiTechnologyCompatibility_ComputeAverageRuntimeRelationScaleQ8
              (ModelDefinitionRecordPrefix *candidateDefinition)

{
  /* definition id (+8) of the unit's model definition */
  PckModelDefinitionIdCatalog unitDefinitionId;
  UQ8 averageScaleQ8;
  int remainingCount;
  int definitionIdDelta;
  uint32_t occupiedEntryCount;
  AiRuntimeWorkspaceEntry *runtimeWorkspaceEntry;
  
  averageScaleQ8 = Q8_ONE;
  occupiedEntryCount = 0;
  runtimeWorkspaceEntry = g_AiWorkspace01Units;
  for (remainingCount = g_AiWorkspace01Count; remainingCount != 0; remainingCount--) {
    if (runtimeWorkspaceEntry->modelRuntime != nullptr) {
      occupiedEntryCount++;
      unitDefinitionId = runtimeWorkspaceEntry->modelRuntime->definitionOrSavedId.definition->definitionId;
      definitionIdDelta = (int)unitDefinitionId - (int)candidateDefinition->definitionId;
      if (unitDefinitionId == candidateDefinition->definitionId ||
          definitionIdDelta == -1000 || definitionIdDelta == -2000 ||
          definitionIdDelta == 1000 || definitionIdDelta == 2000) {
        averageScaleQ8 = averageScaleQ8 + 2 * Q8_ONE;
      }
    }
    runtimeWorkspaceEntry++;
  }
  if (occupiedEntryCount != 0) {
    averageScaleQ8 = averageScaleQ8 / occupiedEntryCount;
  }
  return averageScaleQ8;
}


/* Class vtables. */

AiTechnologyCandidateScoreCallback *g_AiTechnologyCandidateScoreCallbackTable[6] = {
    /* 0 */ THANDOR_SLOT(AiTechnologyScore_AlwaysZero),
    /* 1 */ THANDOR_SLOT(AiTechnologyScore_ComputeFactionScaledCandidateValue),
    /* 2 */ THANDOR_SLOT(AiTechnologyScore_ReturnBaseCandidateValueForKind2),
    /* 3 */ THANDOR_SLOT(AiTechnologyScore_ComputeRuntimeClassCompatibleCandidateValue),
    /* 4 */ THANDOR_SLOT(AiTechnologyScore_ReturnBaseCandidateValueForKind4),
    /* 5 */ THANDOR_SLOT(AiTechnologyScore_ComputeCategoryCompatibleCandidateValue)};

/* Research planning: once the faction has an ARM 330 (0x14A) structure, scores every available technology of
   workspace 12 with the score callback of its kind and proposes the best one (entry kind 2) with
   workspace12BestCandidateBaseWeight, halved while the faction's primary anchor cooldown runs.
*/
void AiTechnologyCandidate_AddBestResearch(FactionRuntimeIndex factionIndex,WorldRuntimeContext *worldRuntime)

{
  AiTechnologyCandidateScore candidateScore;
  int wordIndex;
  AiTechnologyCandidateScore bestScore;
  AiTechnologyPlanningCandidateCount candidatesRemaining;
  AiTechnologyPlanningCandidate *candidateCursor;
  uint32_t weightRange;
  RuntimeToken entityId;

  if (!AiPrimaryWorkspace_HasEntryById(ARM_0330_BUILDING_MDL0303)) {
    return;
  }
  /* Category mask for the category score callback: bit 2 / bit 4 when the faction owns any
     technology of category 2 / 3. */
  g_AiTechnologyScoreCategoryMask = 0;
  for (wordIndex = 0; wordIndex < 8; wordIndex++) {
    uint32_t owned = g_GameFactionRuntimeImage.records[factionIndex].technologyMasks256Bits[wordIndex];
    if ((g_TechnologyCategoryMasks.category2[wordIndex] & owned) != 0) {
      g_AiTechnologyScoreCategoryMask = g_AiTechnologyScoreCategoryMask | 2;
    }
    if ((g_TechnologyCategoryMasks.category3[wordIndex] & owned) != 0) {
      g_AiTechnologyScoreCategoryMask = g_AiTechnologyScoreCategoryMask | 4;
    }
  }
  if (g_AiWorkspace12Count == 0) {
    return;
  }
  bestScore = 0;
  weightRange = g_AiKnowledgeData->parameters.workspace12BestCandidateBaseWeight;
  entityId = 0;
  candidateCursor = g_AiWorkspace12TechnologyCandidates;
  for (candidatesRemaining = g_AiWorkspace12Count; candidatesRemaining != 0; candidatesRemaining--) {
    candidateScore = g_AiTechnologyCandidateScoreCallbackTable[candidateCursor->scoreKind08]
                      (factionIndex,candidateCursor->technologyId00,worldRuntime);
    if (bestScore < candidateScore) {
      entityId = candidateCursor->technologyId00;
      bestScore = candidateScore;
    }
    candidateCursor++;
  }
  if (bestScore != 0) {
    if (g_GameFactionRuntimeImage.records[factionIndex].primaryAnchorCooldown != 0) {
      weightRange = weightRange >> 1;
    }
    AiCandidateWorkspace_AddOrAccumulateWeightedEntry(entityId,weightRange,2 /* technology */);
  }
}

/* Technology score callback for score kind 0 (g_AiTechnologyCandidateScoreCallbackTable[0],
   called by AiTechnologyCandidate_AddBestResearch): a technology of this kind always scores 0, so it
   is never chosen for research.
*/
AiTechnologyCandidateScore AiTechnologyScore_AlwaysZero
          (FactionRuntimeIndex factionIndex,PckTechnologyIdCatalog technologyId,
          WorldRuntimeContext *worldRuntime)

{
  return 0;
}

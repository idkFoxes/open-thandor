/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/src/gameplay/ai/technology.c
 * Reverse engineering by idkFoxes 2026
 */

#include <thandor/gameplay/ai/technology.h>
#include <thandor/thandor.h>

/* Implementation ownership: gameplay/ai/technology. */

/* Address: 0x0053BD80.
   Technology score callback for score kind 3 (g_AiTechnologyCandidateScoreCallbackTable[3], image
   0x0053B9EC; technologies researched in class 13/22 structures). Looks up the model definition that
   ModelDefinitionRegistry_FindByRuntimeClassId finds for the technology id and scores the technology by
   how many own units share that definition's family (AiTechnologyCompatibility_ComputeAverageRuntimeRelationScaleQ8).
   Definitions of class 1 only score while the faction has none of ARM 302/303/304, at half weight, and at
   3/8 when it has ARM 305 or 306.
*/
AiTechnologyCandidateScore __thandor_eax_preserve_ecx_edx
AiTechnologyScore_ComputeRuntimeClassCompatibleCandidateValue
          (FactionRuntimeIndex factionIndex,PckTechnologyIdCatalog technologyId,
          WorldRuntimeContext *worldRuntime)

{
  uint32_t runtimeClassId;
  TechnologyAsset *technologyAsset;
  ModelDefinitionRecordPrefix *candidateDefinition;
  UQ8 relationScaleQ8;
  uint32_t candidateScore;
  bool rejected;
  bool hasAsset;

  candidateDefinition = ModelDefinitionRegistry_FindByRuntimeClassId(technologyId);
  technologyAsset = g_TechnologyAsset;
  if (candidateDefinition == NULL) {
    return 0;
  }
  runtimeClassId = candidateDefinition[6].flags; /* +0x4C of the definition */
  if (runtimeClassId != 1) {
    if ((((runtimeClassId != 0x15) && (runtimeClassId != 2)) && (runtimeClassId != 3)) && ((runtimeClassId != 0x11 && (runtimeClassId != 0x13))))
    {
      /* The original leaves the definition pointer in EAX here (JNZ 0x0053BE79), so every other class
         "scores" with its record address. */
      return (AiTechnologyCandidateScore)candidateDefinition;
    }
    rejected = AiTechnologyCompatibility_AcceptRuntimeClassCandidate(factionIndex,candidateDefinition);
    if (!rejected) {
      relationScaleQ8 = AiTechnologyCompatibility_ComputeAverageRuntimeRelationScaleQ8(candidateDefinition);
      return (relationScaleQ8 * AI_TECHNOLOGY_RELATION_SCORE_FACTOR >> 8) *
             technologyAsset->records[technologyId].baseCandidateScore >> 8;
    }
    return 0;
  }
  hasAsset = AiPrimaryWorkspace_HasEntryById(ARM_0302_BUILDING_MDL0300);
  if (hasAsset) {
    return 0;
  }
  hasAsset = AiPrimaryWorkspace_HasEntryById(ARM_0303_BUILDING_MDL0316);
  if (!hasAsset) {
    hasAsset = AiPrimaryWorkspace_HasEntryById(ARM_0304_BUILDING_MDL0324);
    if (hasAsset) {
      return 0;
    }
    rejected = AiTechnologyCompatibility_AcceptRuntimeClassCandidate(factionIndex,candidateDefinition);
    if (!rejected) {
      relationScaleQ8 = AiTechnologyCompatibility_ComputeAverageRuntimeRelationScaleQ8(candidateDefinition);
      candidateScore = (relationScaleQ8 * AI_TECHNOLOGY_RELATION_SCORE_FACTOR >> 8) *
                       technologyAsset->records[technologyId].baseCandidateScore >> 8;
      hasAsset = AiPrimaryWorkspace_HasEntryById(ARM_0305_BUILDING_MDL0317);
      if ((hasAsset) || (hasAsset = AiPrimaryWorkspace_HasEntryById(ARM_0306_BUILDING_MDL0310), hasAsset))
      {
        candidateScore = candidateScore * 3 >> 2;
      }
      return candidateScore >> 1;
    }
    return 0;
  }
  return 0;
}


/* Address: 0x00538000.
   Returns false (CF clear) when the AI may plan this technology: no workspace-00 structure is already working
   on it (runtimeFlags & 0xC0 with the technology id at +0x100), the faction (factionRecordOffset = faction *
   0x740) has not unlocked it yet, and all eight prerequisite mask words are covered by its unlocked technologies.
*/
bool __thandor_cf_preserve_eax_ecx_edx
AiTechnologyCandidate_IsCurrentlyAvailable
          (PckTechnologyIdCatalog technologyIndex,FactionRuntimeRecordByteOffset factionRecordOffset
          )

{
  AiWorkspaceRuntimeSlotAddress32 runtimeSlotAddress;
  int remainingCount;
  AiWorkspace00EntryView8 *workspaceEntry;

  workspaceEntry = g_AiWorkspaceBuffer00_Size0400;
  for (remainingCount = g_AiWorkspace00Count; remainingCount != 0; remainingCount--) {
    runtimeSlotAddress = workspaceEntry->runtimeSlotAddressOrZero;
    if (((runtimeSlotAddress != 0) && ((*(uint32_t *)(runtimeSlotAddress + 0xec) & 0xc0) != 0)) &&
       (technologyIndex == *(PckTechnologyIdCatalog *)(runtimeSlotAddress + 0x100))) {
      return true;
    }
    workspaceEntry++;
  }
  /* the first test reads the unlock bit in technologyMasks256Bits (faction record +0x6E0) */
  if ((((((*(uint32_t *)(factionRecordOffset + THANDOR_ADDR(g_GameFactionRuntimeImage,0x6e0) + (technologyIndex >> 5) * 4) &
          1 << ((uint8_t)technologyIndex & 0x1f)) == 0) &&
        ((g_TechnologyAsset->records[technologyIndex].prerequisiteMasks[0] &
         *(uint32_t *)((int)g_GameFactionRuntimeImage.records[0].technologyMasks256Bits +
                  factionRecordOffset)) ==
         g_TechnologyAsset->records[technologyIndex].prerequisiteMasks[0])) &&
       (((g_TechnologyAsset->records[technologyIndex].prerequisiteMasks[1] &
         *(uint32_t *)((int)g_GameFactionRuntimeImage.records[0].technologyMasks256Bits +
                  factionRecordOffset + 4)) ==
         g_TechnologyAsset->records[technologyIndex].prerequisiteMasks[1] &&
        (((g_TechnologyAsset->records[technologyIndex].prerequisiteMasks[2] &
          *(uint32_t *)((int)g_GameFactionRuntimeImage.records[0].technologyMasks256Bits +
                   factionRecordOffset + 8)) ==
          g_TechnologyAsset->records[technologyIndex].prerequisiteMasks[2] &&
         ((g_TechnologyAsset->records[technologyIndex].prerequisiteMasks[3] &
          *(uint32_t *)((int)g_GameFactionRuntimeImage.records[0].technologyMasks256Bits +
                   factionRecordOffset + 0xc)) ==
          g_TechnologyAsset->records[technologyIndex].prerequisiteMasks[3])))))) &&
      ((g_TechnologyAsset->records[technologyIndex].prerequisiteMasks[4] &
       *(uint32_t *)((int)g_GameFactionRuntimeImage.records[0].technologyMasks256Bits +
                factionRecordOffset + 0x10)) ==
       g_TechnologyAsset->records[technologyIndex].prerequisiteMasks[4])) &&
     ((((g_TechnologyAsset->records[technologyIndex].prerequisiteMasks[5] &
        *(uint32_t *)((int)g_GameFactionRuntimeImage.records[0].technologyMasks256Bits +
                 factionRecordOffset + 0x14)) ==
        g_TechnologyAsset->records[technologyIndex].prerequisiteMasks[5] &&
       ((g_TechnologyAsset->records[technologyIndex].prerequisiteMasks[6] &
        *(uint32_t *)((int)g_GameFactionRuntimeImage.records[0].technologyMasks256Bits +
                 factionRecordOffset + 0x18)) ==
        g_TechnologyAsset->records[technologyIndex].prerequisiteMasks[6])) &&
      ((g_TechnologyAsset->records[technologyIndex].prerequisiteMasks[7] &
       *(uint32_t *)((int)g_GameFactionRuntimeImage.records[0].technologyMasks256Bits +
                factionRecordOffset + 0x1c)) ==
       g_TechnologyAsset->records[technologyIndex].prerequisiteMasks[7])))) {
    return false;
  }
  return true;
}


/* Address: 0x00538140.
   Appends a technology that the source structure can research to the technology candidate list (workspace 12,
   at most 32; the pioneer vehicle is never added) and picks its score function: walls 0, mine and pump
   improvements 1, source of runtime class 11 2, of class 13 or 22 3, radar and AR-M silo technologies 4,
   everything else 5. The register values of the caller's loop pass through unchanged.
*/
AiTechnologyPlanningLoopRegisterContinuityResult
AiTechnologyPlanning_AddCandidateRecord
          (uint32_t technologyPanelIndex,uint32_t sourceArmyEntriesRemaining,uint32_t factionRecordOffset,
          ArmyRuntimeSlot *sourceArmyRuntime,PckTechnologyIdCatalog technologyId)

{
  AiTechnologyPlanningCandidate *candidateBuffer;
  AiTechnologyPlanningCandidateCount candidateIndex;
  AiTechnologyPlanningLoopRegisterContinuityResult continuityResult;
  MdlDefinitionSemanticPrefix80 *sourceArmyModelDefinition;
  
  candidateIndex = g_AiWorkspace12Count;
  candidateBuffer = g_AiWorkspaceBuffer12_Size0200;
  if ((g_AiWorkspace12Count < AI_WORKSPACE12_CAPACITY) && (technologyId != TEC_011_PIONEER_VEHICLE)) {
    g_AiWorkspaceBuffer12_Size0200[g_AiWorkspace12Count].technologyId00 = technologyId;
    candidateBuffer[candidateIndex].sourceArmyRuntime04 = sourceArmyRuntime;
    candidateBuffer[candidateIndex].scoreKind08 = AI_TECHNOLOGY_SCORE_DEFAULT_ZERO;
    g_AiWorkspace12Count++;
    /* each test passed raises the score kind by one; the first match stops the chain */
    sourceArmyModelDefinition =
         (MdlDefinitionSemanticPrefix80 *)
         (sourceArmyRuntime->modelRuntimeOrSavedOffset).modelRuntime;
    if ((((technologyId != TEC_216_WALL) &&
         (((technologyId != TEC_217_HIGH_WALL &&
           (candidateBuffer[candidateIndex].scoreKind08 =
                 candidateBuffer[candidateIndex].scoreKind08 + AI_TECHNOLOGY_SCORE_FACTION_SCALED,
           technologyId != TEC_210_IMPROVE_XENITE_MINE_1)) &&
          (technologyId != TEC_211_IMPROVE_XENITE_MINE_2)))) &&
        ((((technologyId != TEC_213_IMPROVE_TRITIUM_PUMP_1 &&
           (technologyId != TEC_214_IMPROVE_TRITIUM_PUMP_2)) &&
          (candidateBuffer[candidateIndex].scoreKind08 =
                candidateBuffer[candidateIndex].scoreKind08 + AI_TECHNOLOGY_SCORE_FACTION_SCALED,
          sourceArmyModelDefinition->runtimeClassId != MODEL_RUNTIME_CLASS_11)) &&
         ((candidateBuffer[candidateIndex].scoreKind08 =
                candidateBuffer[candidateIndex].scoreKind08 + AI_TECHNOLOGY_SCORE_FACTION_SCALED,
          sourceArmyModelDefinition->runtimeClassId != MODEL_RUNTIME_CLASS_13 &&
          (sourceArmyModelDefinition->runtimeClassId != MODEL_RUNTIME_CLASS_22)))))) &&
       ((candidateBuffer[candidateIndex].scoreKind08 = candidateBuffer[candidateIndex].scoreKind08 + AI_TECHNOLOGY_SCORE_FACTION_SCALED,
        technologyId != TEC_183_RADAR &&
        ((((technologyId != TEC_184_RADAR_RANGE_PLUS_10_PERCENT &&
           (technologyId != TEC_185_RADAR_RANGE_PLUS_10_PERCENT)) &&
          (technologyId != TEC_189_AR_MINUS_M_SILO)) &&
         ((technologyId != TEC_190_AR_MINUS_M_SILO_RANGE_PLUS_10_PERCENT &&
          (technologyId != TEC_191_AR_MINUS_M_SILO_RANGE_PLUS_10_PERCENT)))))))) {
      candidateBuffer[candidateIndex].scoreKind08 = candidateBuffer[candidateIndex].scoreKind08 + AI_TECHNOLOGY_SCORE_FACTION_SCALED;
    }
  }
  continuityResult.preservedEcxSourceArmyEntriesRemaining = sourceArmyEntriesRemaining;
  continuityResult.preservedEaxTechnologyPanelIndex = technologyPanelIndex;
  continuityResult.preservedEdxFactionRecordOffset = factionRecordOffset;
  return continuityResult;
}


/* Address: 0x0053BCC0.
   Technology score callback for score kind 1 (g_AiTechnologyCandidateScoreCallbackTable[1], image
   0x0053B9E4; xenite-mine and tritium-pump improvements). Nothing while the faction's xenite is below the
   ki.dat minimum; mine improvements then score their base value, pump improvements their base value
   scaled by energy demand / energy supply (Q8), but only once demand reaches 0xF0/0x100 (about 94%)
   of supply.
*/
AiTechnologyCandidateScore __thandor_eax_preserve_ecx_edx
AiTechnologyScore_ComputeFactionScaledCandidateValue
          (FactionRuntimeIndex factionIndex,PckTechnologyIdCatalog technologyId,
          WorldRuntimeContext *worldRuntime)

{
  /* EDI side channel: the caller loads g_AiKnowledgeData. It was a leading pseudo-parameter,
     which shifted every argument of the three-argument table call. */
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
         (UQ8)(((uint64_t)(totalEnergyDemandQ4 >> 0x18) << 0x20 |
               (uint64_t)totalEnergyDemandQ4 * 0x100 & 0xffffffff) /
              (uint64_t)
              (g_GameFactionRuntimeImage.records[factionIndex].tritiumExtractionRateQ4PerTick * 0x10
              + g_GameFactionRuntimeImage.records[factionIndex].baselineEnergySupplyQ4));
    /* the original divides without a zero check: a faction with no energy supply at all faults here */
    if (0xef < (int)energyDemandPressureRatioQ8) {
      return energyDemandPressureRatioQ8 *
             g_TechnologyAsset->records[technologyId].baseCandidateScore >> 8;
    }
  }
  return 0;
}


/* Address: 0x0053BD60.
   Technology score callback for score kind 2 (g_AiTechnologyCandidateScoreCallbackTable[2], image
   0x0053B9E8; technologies researched in class 11 structures): the technology's base candidate score
   from the technology asset, unconditionally.
*/
AiTechnologyCandidateScore __thandor_eax_preserve_ecx_edx
AiTechnologyScore_ReturnBaseCandidateValueForKind2
          (FactionRuntimeIndex factionIndex,PckTechnologyIdCatalog technologyId,
          WorldRuntimeContext *worldRuntime)

{
  return g_TechnologyAsset->records[technologyId].baseCandidateScore;
}


/* Address: 0x0053BEA0.
   Technology score callback for score kind 4 (g_AiTechnologyCandidateScoreCallbackTable[4], image
   0x0053B9F0; radar and AR-M silo technologies): the technology's base candidate score, unconditionally.
*/
AiTechnologyCandidateScore __thandor_eax_preserve_ecx_edx
AiTechnologyScore_ReturnBaseCandidateValueForKind4
          (FactionRuntimeIndex factionIndex,PckTechnologyIdCatalog technologyId,
          WorldRuntimeContext *worldRuntime)

{
  return g_TechnologyAsset->records[technologyId].baseCandidateScore;
}


/* The original passes the category mask in EDX; AiStrategicCandidate_AddBestWorkspace12Entry stores it
   here before its scoring loop. */
AiTechnologyCategoryMask g_AiTechnologyScoreCategoryMaskEdx;

AiTechnologyCandidateScore __thandor_eax_preserve_ecx_edx
AiTechnologyScore_ComputeCategoryCompatibleCandidateValue_Body
          (AiTechnologyCategoryMask categoryMaskEdx,FactionRuntimeIndex factionIndex,
          PckTechnologyIdCatalog technologyId,WorldRuntimeContext *worldRuntime);

/* Address: 0x0053BEC0.
   Technology score callback for score kind 5 (g_AiTechnologyCandidateScoreCallbackTable[5], image
   0x0053B9F4; every technology not caught by kinds 0-4). Hands the EDX category mask (here a global) to
   the body below, which holds the original code.
*/
AiTechnologyCandidateScore __thandor_eax_preserve_ecx_edx
AiTechnologyScore_ComputeCategoryCompatibleCandidateValue
          (FactionRuntimeIndex factionIndex,PckTechnologyIdCatalog technologyId,
          WorldRuntimeContext *worldRuntime)

{
  return AiTechnologyScore_ComputeCategoryCompatibleCandidateValue_Body
                   (g_AiTechnologyScoreCategoryMaskEdx,factionIndex,technologyId,worldRuntime);
}

/* Body of 0x0053BEC0 (C-only split, no address of its own; only called by the wrapper above).
   Category C and D technologies score nothing unless the faction already owns a technology of that
   category (bit 2 / bit 4 of categoryMaskEdx). Otherwise the score is the average faction-weighted score
   of the army assets the technology leads to (weights g_AiArmyCandidateScoreWeightsVariantC15) times the
   base candidate score, >> 8.
*/
AiTechnologyCandidateScore __thandor_eax_preserve_ecx_edx
AiTechnologyScore_ComputeCategoryCompatibleCandidateValue_Body
          (AiTechnologyCategoryMask categoryMaskEdx,FactionRuntimeIndex factionIndex,
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
    categoryMaskBit = categoryMaskEdx & 2;
    if (categoryMaskBit == 0) {
      return 0;
    }
  }
  else if (technologyCategory == TECHNOLOGY_CATEGORY_D) {
    categoryMaskBit = categoryMaskEdx & 4;
    if (categoryMaskBit == 0) {
      return 0;
    }
  }
  averageAssetScore = AiArmyCandidate_ComputeAverageCompatibleAssetScore
                    (&g_AiArmyCandidateScoreWeightsVariantC15,factionIndex,technologyId);
  return (uint32_t)(averageAssetScore * technologyAsset->records[technologyId].baseCandidateScore) >> 8;
}


/* Address: 0x0053BC00.
   Veto hook of AiTechnologyScore_ComputeRuntimeClassCompatibleCandidateValue (its only caller, called
   directly): would return true (CF set) to reject the candidate definition, but always accepts (CF clear).
   The caller passes candidateDefinition in EAX as well as on the stack; EAX is preserved.
*/
bool __thandor_cf_preserve_eax_ecx_edx
AiTechnologyCompatibility_AcceptRuntimeClassCandidate
          (FactionRuntimeIndex factionIndex,ModelDefinitionRecordPrefix *candidateDefinition)

{
  return false;
}


/* Address: 0x0053BC20.
   Relation scale (Q8) of a candidate definition to the faction's units in the secondary workspace
   (workspace 01): (1.0 + 2.0 per assigned unit whose definition id equals the candidate's or differs by
   1000 or 2000, i.e. the same unit in another id block) / number of assigned units; 1.0 when there is
   none. Called directly by AiTechnologyScore_ComputeRuntimeClassCompatibleCandidateValue.
*/
UQ8 AiTechnologyCompatibility_ComputeAverageRuntimeRelationScaleQ8
              (ModelDefinitionRecordPrefix *candidateDefinition)

{
  /* The typed path reads the dword at +8 of the record at the unit's slot +0 (MOV EDX,[EDX]; MOV
     EDX,[EDX+8]); the current struct view calls it ownerArmyRuntime, but it is compared as a definition id. */
  ArmyRuntimeSlot *unitDefinitionId;
  UQ8 averageScaleQ8;
  int remainingCount;
  int definitionIdDelta;
  uint32_t occupiedEntryCount;
  AiRuntimeWorkspaceEntry *runtimeWorkspaceEntry;
  
  averageScaleQ8 = 0x100; /* 1.0 */
  if (g_AiWorkspace01Count != 0) {
    occupiedEntryCount = 0;
    remainingCount = g_AiWorkspace01Count;
    runtimeWorkspaceEntry = g_AiWorkspaceBuffer01_Size0200;
    do {
      if (runtimeWorkspaceEntry->armyRuntime != NULL) {
        occupiedEntryCount++;
        unitDefinitionId = (((runtimeWorkspaceEntry->armyRuntime->modelRuntimeOrSavedOffset).modelRuntime)->
                 ownerArmyRuntimeOrSavedOffset).armyRuntime;
        definitionIdDelta = (int)unitDefinitionId - (int)candidateDefinition->definitionId;
        if ((((unitDefinitionId == (ArmyRuntimeSlot *)candidateDefinition->definitionId) || (definitionIdDelta == -1000))
            || (definitionIdDelta == -2000)) || ((definitionIdDelta == 1000 || (definitionIdDelta == 2000)))) {
          averageScaleQ8 = averageScaleQ8 + 0x200; /* 2.0 */
        }
      }
      runtimeWorkspaceEntry++;
      remainingCount--;
    } while (remainingCount != 0);
    if (occupiedEntryCount != 0) {
      averageScaleQ8 = averageScaleQ8 / occupiedEntryCount;
    }
  }
  return averageScaleQ8;
}


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
   Ownership: gameplay/ai/technology.
   Purpose: Common stdcall stack ABI: FactionRuntimeIndex, TechnologyId, WorldRuntimeContext*. Signed score returns
   in EAX. Target group: stack-only callback target. Exact binary and live ownership are preflight locked.
   Local calls: AiTechnologyCompatibility_AcceptRuntimeClassCandidateCf,
   AiTechnologyCompatibility_ComputeAverageRuntimeRelationScaleQ8.
   Cross-module calls: ModelDefinitionRegistry_FindByRuntimeClassId [assets/model/definitions],
   AiPrimaryWorkspace_HasEntryByIdCf [gameplay/ai/workspaces].
*/
AiTechnologyCandidateScore __thandor_eax_preserve_ecx_edx
AiTechnologyScore_ComputeRuntimeClassCompatibleCandidateValue
          (FactionRuntimeIndex factionIndex,PckTechnologyIdCatalog technologyId,
          WorldRuntimeContext *worldRuntime)

{
  dword definitionClassValue;
  TechnologyAsset *technologyAsset;
  ModelDefinitionRecordPrefix *candidateDefinition;
  UQ8 relationScaleQ8;
  uint candidateScore;
  bool cfResult;
  
  candidateDefinition = ModelDefinitionRegistry_FindByRuntimeClassId(technologyId);
  technologyAsset = g_TechnologyAsset;
  if (candidateDefinition == (ModelDefinitionRecordPrefix *)0x0) {
    return 0;
  }
  definitionClassValue = candidateDefinition[6].flags;
  if (definitionClassValue != 1) {
    if ((((definitionClassValue != 0x15) && (definitionClassValue != 2)) && (definitionClassValue != 3)) && ((definitionClassValue != 0x11 && (definitionClassValue != 0x13))))
    {
      return (AiTechnologyCandidateScore)candidateDefinition;
    }
    cfResult = AiTechnologyCompatibility_AcceptRuntimeClassCandidateCf(factionIndex,candidateDefinition);
    if (!cfResult) {
      relationScaleQ8 = AiTechnologyCompatibility_ComputeAverageRuntimeRelationScaleQ8(candidateDefinition);
      return (relationScaleQ8 * 40000 >> 8) * technologyAsset->records[technologyId].baseCandidateScore >> 8;
    }
    return 0;
  }
  cfResult = AiPrimaryWorkspace_HasEntryByIdCf(ARM_0302_BUILDING_MDL0300);
  if (cfResult) {
    return 0;
  }
  cfResult = AiPrimaryWorkspace_HasEntryByIdCf(ARM_0303_BUILDING_MDL0316);
  if (!cfResult) {
    cfResult = AiPrimaryWorkspace_HasEntryByIdCf(ARM_0304_BUILDING_MDL0324);
    if (cfResult) {
      return 0;
    }
    cfResult = AiTechnologyCompatibility_AcceptRuntimeClassCandidateCf(factionIndex,candidateDefinition);
    if (!cfResult) {
      relationScaleQ8 = AiTechnologyCompatibility_ComputeAverageRuntimeRelationScaleQ8(candidateDefinition);
      candidateScore = (relationScaleQ8 * 40000 >> 8) * technologyAsset->records[technologyId].baseCandidateScore >> 8;
      cfResult = AiPrimaryWorkspace_HasEntryByIdCf(ARM_0305_BUILDING_MDL0317);
      if ((cfResult) || (cfResult = AiPrimaryWorkspace_HasEntryByIdCf(ARM_0306_BUILDING_MDL0310), cfResult))
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
   Ownership: gameplay/ai/technology.
   Purpose: Rejects a technology already represented by an active workspace entity or already unlocked for the
   faction, then verifies all eight prerequisite masks. CF is clear only when the technology is currently available
   for planning. Stock tech.tec has 512 records over canonical ids 0..255; localized titles do not prove source-
   building, tier, direction, or effect mappings. Typed parameters: p3
   factionRecordOffset→FactionRuntimeRecordByteOffset_V344. Calling convention, complete VariableStorage
   serialization, function bytes, control flow, globals, locals, and executable data remain unchanged.
*/
bool __thandor_cf_preserve_eax_ecx_edx
AiTechnologyCandidate_IsCurrentlyAvailableCf
          (PckTechnologyIdCatalog technologyIndex,FactionRuntimeRecordByteOffset factionRecordOffset
          )

{
  AiWorkspaceRuntimeSlotAddress32 runtimeSlotAddress;
  int remainingCount;
  AiWorkspace00EntryView8 *workspaceEntry;
  
  workspaceEntry = g_AiWorkspaceBuffer00_Size0400;
  for (remainingCount = g_AiWorkspace00Count; remainingCount != 0; remainingCount = remainingCount + -1) {
    runtimeSlotAddress = workspaceEntry->runtimeSlotAddressOrZero;
    if (((runtimeSlotAddress != 0) && ((*(uint *)(runtimeSlotAddress + 0xec) & 0xc0) != 0)) &&
       (technologyIndex == *(PckTechnologyIdCatalog *)(runtimeSlotAddress + 0x100))) {
      return true;
    }
    workspaceEntry = workspaceEntry + 1;
  }
  if ((((((*(uint *)(factionRecordOffset + THANDOR_ADDR(g_GameFactionRuntimeImage,0x6e0) + (technologyIndex >> 5) * 4) &
          1 << ((byte)technologyIndex & 0x1f)) == 0) &&
        ((g_TechnologyAsset->records[technologyIndex].prerequisiteMasks[0] &
         *(uint *)((int)g_GameFactionRuntimeImage.records[0].technologyMasks256Bits +
                  factionRecordOffset)) ==
         g_TechnologyAsset->records[technologyIndex].prerequisiteMasks[0])) &&
       (((g_TechnologyAsset->records[technologyIndex].prerequisiteMasks[1] &
         *(uint *)((int)g_GameFactionRuntimeImage.records[0].technologyMasks256Bits +
                  factionRecordOffset + 4)) ==
         g_TechnologyAsset->records[technologyIndex].prerequisiteMasks[1] &&
        (((g_TechnologyAsset->records[technologyIndex].prerequisiteMasks[2] &
          *(uint *)((int)g_GameFactionRuntimeImage.records[0].technologyMasks256Bits +
                   factionRecordOffset + 8)) ==
          g_TechnologyAsset->records[technologyIndex].prerequisiteMasks[2] &&
         ((g_TechnologyAsset->records[technologyIndex].prerequisiteMasks[3] &
          *(uint *)((int)g_GameFactionRuntimeImage.records[0].technologyMasks256Bits +
                   factionRecordOffset + 0xc)) ==
          g_TechnologyAsset->records[technologyIndex].prerequisiteMasks[3])))))) &&
      ((g_TechnologyAsset->records[technologyIndex].prerequisiteMasks[4] &
       *(uint *)((int)g_GameFactionRuntimeImage.records[0].technologyMasks256Bits +
                factionRecordOffset + 0x10)) ==
       g_TechnologyAsset->records[technologyIndex].prerequisiteMasks[4])) &&
     ((((g_TechnologyAsset->records[technologyIndex].prerequisiteMasks[5] &
        *(uint *)((int)g_GameFactionRuntimeImage.records[0].technologyMasks256Bits +
                 factionRecordOffset + 0x14)) ==
        g_TechnologyAsset->records[technologyIndex].prerequisiteMasks[5] &&
       ((g_TechnologyAsset->records[technologyIndex].prerequisiteMasks[6] &
        *(uint *)((int)g_GameFactionRuntimeImage.records[0].technologyMasks256Bits +
                 factionRecordOffset + 0x18)) ==
        g_TechnologyAsset->records[technologyIndex].prerequisiteMasks[6])) &&
      ((g_TechnologyAsset->records[technologyIndex].prerequisiteMasks[7] &
       *(uint *)((int)g_GameFactionRuntimeImage.records[0].technologyMasks256Bits +
                factionRecordOffset + 0x1c)) ==
       g_TechnologyAsset->records[technologyIndex].prerequisiteMasks[7])))) {
    return false;
  }
  return true;
}


/* Address: 0x00538140.
   Ownership: gameplay/ai/technology.
   Purpose: Appends one technology candidate and source entity to the 32-entry planning workspace and derives its
   category count from verified technology IDs and source entity classes. Stock tech.tec has 512 records over
   canonical ids 0..255; localized titles do not prove source-building, tier, direction, or effect mappings.
*/
AiTechnologyPlanningLoopRegisterContinuityResult
AiTechnologyPlanning_AddCandidateRecord
          (dword technologyPanelIndex,dword sourceArmyEntriesRemaining,dword factionRecordOffset,
          ArmyRuntimeSlot *sourceArmyRuntime,PckTechnologyIdCatalog technologyId)

{
  AiTechnologyPlanningCandidate *candidateBuffer;
  AiTechnologyPlanningCandidateCount candidateIndex;
  AiTechnologyPlanningLoopRegisterContinuityResult continuityResult;
  MdlDefinitionSemanticPrefix80 *sourceArmyModelDefinition;
  
  candidateIndex = g_AiWorkspace12Count;
  candidateBuffer = g_AiWorkspaceBuffer12_Size0200;
  if ((g_AiWorkspace12Count < 0x20) && (technologyId != TEC_011_PIONEER_VEHICLE)) {
    g_AiWorkspaceBuffer12_Size0200[g_AiWorkspace12Count].technologyId00 = technologyId;
    candidateBuffer[candidateIndex].sourceArmyRuntime04 = sourceArmyRuntime;
    candidateBuffer[candidateIndex].scoreKind08 = AI_TECHNOLOGY_SCORE_DEFAULT_ZERO;
    g_AiWorkspace12Count = g_AiWorkspace12Count + 1;
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
   Ownership: gameplay/ai/technology.
   Purpose: Common stdcall stack ABI: FactionRuntimeIndex, TechnologyId, WorldRuntimeContext*. Signed score returns
   in EAX. Target group: EDI inherited knowledge-context side channel. Exact binary and live ownership are
   preflight locked.
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
         (UQ8)(((ulonglong)(totalEnergyDemandQ4 >> 0x18) << 0x20 |
               (ulonglong)totalEnergyDemandQ4 * 0x100 & 0xffffffff) /
              (ulonglong)
              (g_GameFactionRuntimeImage.records[factionIndex].tritiumExtractionRateQ4PerTick * 0x10
              + g_GameFactionRuntimeImage.records[factionIndex].baselineEnergySupplyQ4));
    if (0xef < (int)energyDemandPressureRatioQ8) {
      return energyDemandPressureRatioQ8 *
             g_TechnologyAsset->records[technologyId].baseCandidateScore >> 8;
    }
  }
  return 0;
}


/* Address: 0x0053BD60.
   Ownership: gameplay/ai/technology.
   Purpose: Common stdcall stack ABI: FactionRuntimeIndex, TechnologyId, WorldRuntimeContext*. Signed score returns
   in EAX. Target group: stack-only callback target. Exact binary and live ownership are preflight locked.
*/
AiTechnologyCandidateScore __thandor_eax_preserve_ecx_edx
AiTechnologyScore_ReturnBaseCandidateValueForKind2
          (FactionRuntimeIndex factionIndex,PckTechnologyIdCatalog technologyId,
          WorldRuntimeContext *worldRuntime)

{
  return g_TechnologyAsset->records[technologyId].baseCandidateScore;
}


/* Address: 0x0053BEA0.
   Ownership: gameplay/ai/technology.
   Purpose: Common stdcall stack ABI: FactionRuntimeIndex, TechnologyId, WorldRuntimeContext*. Signed score returns
   in EAX. Target group: stack-only callback target. Exact binary and live ownership are preflight locked.
*/
AiTechnologyCandidateScore __thandor_eax_preserve_ecx_edx
AiTechnologyScore_ReturnBaseCandidateValueForKind4
          (FactionRuntimeIndex factionIndex,PckTechnologyIdCatalog technologyId,
          WorldRuntimeContext *worldRuntime)

{
  return g_TechnologyAsset->records[technologyId].baseCandidateScore;
}


/* Address: 0x0053BEC0.
   Ownership: gameplay/ai/technology.
   Purpose: Common stdcall stack ABI: FactionRuntimeIndex, TechnologyId, WorldRuntimeContext*. Signed score returns
   in EAX. Target group: EDX inherited category-mask side channel. Exact binary and live ownership are preflight
   locked.
   Cross-module calls: AiArmyCandidate_ComputeAverageCompatibleAssetScore [gameplay/ai/planning].
*/
AiTechnologyCategoryMask g_AiTechnologyScoreCategoryMaskEdx;

AiTechnologyCandidateScore __thandor_eax_preserve_ecx_edx
AiTechnologyScore_ComputeCategoryCompatibleCandidateValue_Body
          (AiTechnologyCategoryMask categoryMaskEdx,FactionRuntimeIndex factionIndex,
          PckTechnologyIdCatalog technologyId,WorldRuntimeContext *worldRuntime);

AiTechnologyCandidateScore __thandor_eax_preserve_ecx_edx
AiTechnologyScore_ComputeCategoryCompatibleCandidateValue
          (FactionRuntimeIndex factionIndex,PckTechnologyIdCatalog technologyId,
          WorldRuntimeContext *worldRuntime)

{
  return AiTechnologyScore_ComputeCategoryCompatibleCandidateValue_Body
                   (g_AiTechnologyScoreCategoryMaskEdx,factionIndex,technologyId,worldRuntime);
}

AiTechnologyCandidateScore __thandor_eax_preserve_ecx_edx
AiTechnologyScore_ComputeCategoryCompatibleCandidateValue_Body
          (AiTechnologyCategoryMask categoryMaskEdx,FactionRuntimeIndex factionIndex,
          PckTechnologyIdCatalog technologyId,WorldRuntimeContext *worldRuntime)

{
  TechnologyCategory technologyCategory;
  uint categoryMaskBit;
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
  return (uint)(averageAssetScore * technologyAsset->records[technologyId].baseCandidateScore) >> 8;
}


/* Address: 0x0053BC00.
   Ownership: gameplay/ai/technology.
   Purpose: Exact CF-clear pass-through helper. The two callers pass candidateDefinition both in EAX and as the
   second stack argument; the helper preserves EAX and clears CF.
*/
bool __thandor_cf_preserve_eax_ecx_edx
AiTechnologyCompatibility_AcceptRuntimeClassCandidateCf
          (FactionRuntimeIndex factionIndex,ModelDefinitionRecordPrefix *candidateDefinition)

{
  return false;
}


/* Address: 0x0053BC20.
   Ownership: gameplay/ai/technology.
   Purpose: Handles ai technology compatibility compute average runtime relation scale q8.
*/
UQ8 AiTechnologyCompatibility_ComputeAverageRuntimeRelationScaleQ8
              (ModelDefinitionRecordPrefix *candidateDefinition)

{
  ArmyRuntimeSlot *ownerArmyRuntime;
  UQ8 averageScaleQ8;
  int remainingCount;
  int definitionIdDelta;
  uint occupiedEntryCount;
  AiRuntimeWorkspaceEntry *runtimeWorkspaceEntry;
  
  averageScaleQ8 = 0x100;
  if (g_AiWorkspace01Count != 0) {
    occupiedEntryCount = 0;
    remainingCount = g_AiWorkspace01Count;
    runtimeWorkspaceEntry = g_AiWorkspaceBuffer01_Size0200;
    do {
      if (runtimeWorkspaceEntry->armyRuntime != (ArmyRuntimeSlot *)0x0) {
        occupiedEntryCount = occupiedEntryCount + 1;
        ownerArmyRuntime = (((runtimeWorkspaceEntry->armyRuntime->modelRuntimeOrSavedOffset).modelRuntime)->
                 ownerArmyRuntimeOrSavedOffset).armyRuntime;
        definitionIdDelta = (int)ownerArmyRuntime - (int)candidateDefinition->definitionId;
        if ((((ownerArmyRuntime == (ArmyRuntimeSlot *)candidateDefinition->definitionId) || (definitionIdDelta == -1000))
            || (definitionIdDelta == -2000)) || ((definitionIdDelta == 1000 || (definitionIdDelta == 2000)))) {
          averageScaleQ8 = averageScaleQ8 + 0x200;
        }
      }
      runtimeWorkspaceEntry = runtimeWorkspaceEntry + 1;
      remainingCount = remainingCount + -1;
    } while (remainingCount != 0);
    if (occupiedEntryCount != 0) {
      averageScaleQ8 = averageScaleQ8 / occupiedEntryCount;
    }
  }
  return averageScaleQ8;
}


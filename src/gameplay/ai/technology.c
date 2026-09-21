/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/src/gameplay/ai/technology.c
 * Reverse engineering by idkFoxes 2026
 */

#include <thandor/gameplay/ai/technology.h>

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
  dword dVar1;
  TechnologyAsset *pTVar2;
  ModelDefinitionRecordPrefix *modelDefinition1;
  UQ8 UVar3;
  uint uVar4;
  bool bVar5;
  
  modelDefinition1 = ModelDefinitionRegistry_FindByRuntimeClassId(technologyId);
  pTVar2 = g_TechnologyAsset;
  if (modelDefinition1 == (ModelDefinitionRecordPrefix *)0x0) {
    return 0;
  }
  dVar1 = modelDefinition1[6].flags;
  if (dVar1 != 1) {
    if ((((dVar1 != 0x15) && (dVar1 != 2)) && (dVar1 != 3)) && ((dVar1 != 0x11 && (dVar1 != 0x13))))
    {
      return (AiTechnologyCandidateScore)modelDefinition1;
    }
    bVar5 = AiTechnologyCompatibility_AcceptRuntimeClassCandidateCf(factionIndex,modelDefinition1);
    if (!bVar5) {
      UVar3 = AiTechnologyCompatibility_ComputeAverageRuntimeRelationScaleQ8(modelDefinition1);
      return (UVar3 * 40000 >> 8) * pTVar2->records[technologyId].baseCandidateScore >> 8;
    }
    return 0;
  }
  bVar5 = AiPrimaryWorkspace_HasEntryByIdCf(ARM_0302_BUILDING_MDL0300);
  if (bVar5) {
    return 0;
  }
  bVar5 = AiPrimaryWorkspace_HasEntryByIdCf(ARM_0303_BUILDING_MDL0316);
  if (!bVar5) {
    bVar5 = AiPrimaryWorkspace_HasEntryByIdCf(ARM_0304_BUILDING_MDL0324);
    if (bVar5) {
      return 0;
    }
    bVar5 = AiTechnologyCompatibility_AcceptRuntimeClassCandidateCf(factionIndex,modelDefinition1);
    if (!bVar5) {
      UVar3 = AiTechnologyCompatibility_ComputeAverageRuntimeRelationScaleQ8(modelDefinition1);
      uVar4 = (UVar3 * 40000 >> 8) * pTVar2->records[technologyId].baseCandidateScore >> 8;
      bVar5 = AiPrimaryWorkspace_HasEntryByIdCf(ARM_0305_BUILDING_MDL0317);
      if ((bVar5) || (bVar5 = AiPrimaryWorkspace_HasEntryByIdCf(ARM_0306_BUILDING_MDL0310), bVar5))
      {
        uVar4 = uVar4 * 3 >> 2;
      }
      return uVar4 >> 1;
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
  AiWorkspaceRuntimeSlotAddress32 AVar1;
  int iVar2;
  AiWorkspace00EntryView8 *pAVar3;
  
  pAVar3 = g_AiWorkspaceBuffer00_Size0400;
  for (iVar2 = g_AiWorkspace00Count; iVar2 != 0; iVar2 = iVar2 + -1) {
    AVar1 = pAVar3->runtimeSlotAddressOrZero;
    if (((AVar1 != 0) && ((*(uint *)(AVar1 + 0xec) & 0xc0) != 0)) &&
       (technologyIndex == *(PckTechnologyIdCatalog *)(AVar1 + 0x100))) {
      return true;
    }
    pAVar3 = pAVar3 + 1;
  }
  if ((((((*(uint *)(factionRecordOffset + 0x50fa20 + (technologyIndex >> 5) * 4) &
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
  AiTechnologyPlanningCandidate *pAVar1;
  AiTechnologyPlanningCandidateCount AVar2;
  AiTechnologyPlanningLoopRegisterContinuityResult AVar3;
  MdlDefinitionSemanticPrefix80 *sourceArmyModelDefinition;
  
  AVar2 = g_AiWorkspace12Count;
  pAVar1 = g_AiWorkspaceBuffer12_Size0200;
  if ((g_AiWorkspace12Count < 0x20) && (technologyId != TEC_011_PIONEER_VEHICLE)) {
    g_AiWorkspaceBuffer12_Size0200[g_AiWorkspace12Count].technologyId00 = technologyId;
    pAVar1[AVar2].sourceArmyRuntime04 = sourceArmyRuntime;
    pAVar1[AVar2].scoreKind08 = AI_TECHNOLOGY_SCORE_DEFAULT_ZERO;
    g_AiWorkspace12Count = g_AiWorkspace12Count + 1;
    sourceArmyModelDefinition =
         (MdlDefinitionSemanticPrefix80 *)
         (sourceArmyRuntime->modelRuntimeOrSavedOffset).modelRuntime;
    if ((((technologyId != TEC_216_WALL) &&
         (((technologyId != TEC_217_HIGH_WALL &&
           (pAVar1[AVar2].scoreKind08 =
                 pAVar1[AVar2].scoreKind08 + AI_TECHNOLOGY_SCORE_FACTION_SCALED,
           technologyId != TEC_210_IMPROVE_XENITE_MINE_1)) &&
          (technologyId != TEC_211_IMPROVE_XENITE_MINE_2)))) &&
        ((((technologyId != TEC_213_IMPROVE_TRITIUM_PUMP_1 &&
           (technologyId != TEC_214_IMPROVE_TRITIUM_PUMP_2)) &&
          (pAVar1[AVar2].scoreKind08 =
                pAVar1[AVar2].scoreKind08 + AI_TECHNOLOGY_SCORE_FACTION_SCALED,
          sourceArmyModelDefinition->runtimeClassId != MODEL_RUNTIME_CLASS_11)) &&
         ((pAVar1[AVar2].scoreKind08 =
                pAVar1[AVar2].scoreKind08 + AI_TECHNOLOGY_SCORE_FACTION_SCALED,
          sourceArmyModelDefinition->runtimeClassId != MODEL_RUNTIME_CLASS_13 &&
          (sourceArmyModelDefinition->runtimeClassId != MODEL_RUNTIME_CLASS_22)))))) &&
       ((pAVar1[AVar2].scoreKind08 = pAVar1[AVar2].scoreKind08 + AI_TECHNOLOGY_SCORE_FACTION_SCALED,
        technologyId != TEC_183_RADAR &&
        ((((technologyId != TEC_184_RADAR_RANGE_PLUS_10_PERCENT &&
           (technologyId != TEC_185_RADAR_RANGE_PLUS_10_PERCENT)) &&
          (technologyId != TEC_189_AR_MINUS_M_SILO)) &&
         ((technologyId != TEC_190_AR_MINUS_M_SILO_RANGE_PLUS_10_PERCENT &&
          (technologyId != TEC_191_AR_MINUS_M_SILO_RANGE_PLUS_10_PERCENT)))))))) {
      pAVar1[AVar2].scoreKind08 = pAVar1[AVar2].scoreKind08 + AI_TECHNOLOGY_SCORE_FACTION_SCALED;
    }
  }
  AVar3.preservedEcxSourceArmyEntriesRemaining = sourceArmyEntriesRemaining;
  AVar3.preservedEaxTechnologyPanelIndex = technologyPanelIndex;
  AVar3.preservedEdxFactionRecordOffset = factionRecordOffset;
  return AVar3;
}


/* Address: 0x0053BCC0.
   Ownership: gameplay/ai/technology.
   Purpose: Common stdcall stack ABI: FactionRuntimeIndex, TechnologyId, WorldRuntimeContext*. Signed score returns
   in EAX. Target group: EDI inherited knowledge-context side channel. Exact binary and live ownership are
   preflight locked.
*/
AiTechnologyCandidateScore __thandor_eax_preserve_ecx_edx
AiTechnologyScore_ComputeFactionScaledCandidateValue
          (AiKnowledgeDataImage *knowledgeData,FactionRuntimeIndex factionIndex,
          PckTechnologyIdCatalog technologyId,WorldRuntimeContext *worldRuntime)

{
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
AiTechnologyCandidateScore __thandor_eax_preserve_ecx_edx
AiTechnologyScore_ComputeCategoryCompatibleCandidateValue
          (AiTechnologyCategoryMask categoryMaskEdx,FactionRuntimeIndex factionIndex,
          PckTechnologyIdCatalog technologyId,WorldRuntimeContext *worldRuntime)

{
  TechnologyCategory TVar1;
  uint uVar2;
  TechnologyAsset *pTVar3;
  AiCandidateScore32 AVar4;
  
  pTVar3 = g_TechnologyAsset;
  TVar1 = g_TechnologyAsset->records[technologyId].category;
  if (TVar1 == TECHNOLOGY_CATEGORY_C) {
    uVar2 = categoryMaskEdx & 2;
  }
  else {
    if (TVar1 != TECHNOLOGY_CATEGORY_D)
    goto 
    AiTechnologyScore_ComputeCategoryCompatibleCandidateValue_ComputeWeightedArmyCompatibilityScore;
    uVar2 = categoryMaskEdx & 4;
  }
  if (uVar2 == 0) {
    return 0;
  }
AiTechnologyScore_ComputeCategoryCompatibleCandidateValue_ComputeWeightedArmyCompatibilityScore:
  AVar4 = AiArmyCandidate_ComputeAverageCompatibleAssetScore
                    (&g_AiArmyCandidateScoreWeightsVariantC15,factionIndex,technologyId);
  return (uint)(AVar4 * pTVar3->records[technologyId].baseCandidateScore) >> 8;
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
  ArmyRuntimeSlot *pAVar1;
  UQ8 UVar2;
  int iVar3;
  int iVar4;
  uint uVar5;
  AiRuntimeWorkspaceEntry *runtimeWorkspaceEntry;
  
  UVar2 = 0x100;
  if (g_AiWorkspace01Count != 0) {
    uVar5 = 0;
    iVar3 = g_AiWorkspace01Count;
    runtimeWorkspaceEntry = g_AiWorkspaceBuffer01_Size0200;
    do {
      if (runtimeWorkspaceEntry->armyRuntime != (ArmyRuntimeSlot *)0x0) {
        uVar5 = uVar5 + 1;
        pAVar1 = (((runtimeWorkspaceEntry->armyRuntime->modelRuntimeOrSavedOffset).modelRuntime)->
                 ownerArmyRuntimeOrSavedOffset).armyRuntime;
        iVar4 = (int)pAVar1 - (int)candidateDefinition->definitionId;
        if ((((pAVar1 == (ArmyRuntimeSlot *)candidateDefinition->definitionId) || (iVar4 == -1000))
            || (iVar4 == -2000)) || ((iVar4 == 1000 || (iVar4 == 2000)))) {
          UVar2 = UVar2 + 0x200;
        }
      }
      runtimeWorkspaceEntry = runtimeWorkspaceEntry + 1;
      iVar3 = iVar3 + -1;
    } while (iVar3 != 0);
    if (uVar5 != 0) {
      UVar2 = UVar2 / uVar5;
    }
  }
  return UVar2;
}


/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/include/thandor/gameplay/ai/data.h
 */

#ifndef THANDOR_GAMEPLAY_AI_DATA_H
#define THANDOR_GAMEPLAY_AI_DATA_H

#include <thandor/generated/types.h>
#include <thandor/generated/ui_templates.h>

extern TechnologyAsset *g_TechnologyAsset; /* 0050D930 g_TechnologyAsset */

extern GameFactionRuntimeImage g_GameFactionRuntimeImage; /* 0050F340 g_GameFactionRuntimeImage */

extern ArmyCommandGeneration g_AiCommandGenerationCandidateBase; /* 0051B38C g_AiCommandGenerationCandidateBase */

extern ArmyCommandGeneration g_AiCommandGenerationRetainedTarget; /* 0051B390 g_AiCommandGenerationRetainedTarget: ArmyCommandGeneration (uint32_t) 0x200 assigned to an army's commandGeneration by the AI combat code (gameplay/ai/combat.c) */

extern uint32_t g_AiCombatTargetClassBaseScores[24]; /* 0051FB78 g_AiCombatTargetClassBaseScores: uint32_t[24] AI combat target base score per target runtime class (gameplay/ai/combat.c) */

extern int32_t g_AiCombatTargetRadialClearanceWeight; /* 00536FA0 g_AiCombatTargetRadialClearanceWeight: int32_t weight 0x600 multiplying the radial clearance term of the AI combat target score (gameplay/ai/combat.c) */

extern int32_t g_AiCombatTargetCandidateCounterCountWeight; /* 00536FA4 g_AiCombatTargetCandidateCounterCountWeight: int32_t weight 0x12000 for the candidate class counter term of the AI combat target score, divided by the Q12 hierarchy scale unity (gameplay/ai/combat.c) */

extern int32_t g_AiCombatTargetSourceCounterCountWeight; /* 00536FA8 g_AiCombatTargetSourceCounterCountWeight: int32_t weight 0x20000 for the source class counter term of the AI combat target score (gameplay/ai/combat.c) */

extern int32_t g_AiCombatTargetScaleDeficitWeight; /* 00536FAC g_AiCombatTargetScaleDeficitWeight: int32_t weight 0x1100 for the hierarchy scale deficit (1.0 - condition ratio) term of the AI combat target score (gameplay/ai/combat.c) */

extern int32_t g_AiCombatTargetClassBaseScoreMultiplier; /* 00536FB0 g_AiCombatTargetClassBaseScoreMultiplier: int32_t multiplier 0x800 applied to g_AiCombatTargetClassBaseScores[runtimeClassId] in the AI combat target score (gameplay/ai/combat.c) */

extern AiCommandGenerationRightShiftBits g_AiCombatTargetSelectedCommandGenerationRightShiftBits; /* 00536FB4 g_AiCombatTargetSelectedCommandGenerationRightShiftBits */

extern AiCommandGenerationRightShiftBits g_AiCombatTargetCurrentCommandGenerationRightShiftBits; /* 00536FB8 g_AiCombatTargetCurrentCommandGenerationRightShiftBits */

extern AiCandidateWorkspaceEntry *g_AiWorkspace13Candidates; /* 00537410 g_AiWorkspace13Candidates */

extern uint32_t g_AiCandidateWorkspaceEntryCount; /* 00537414 g_AiCandidateWorkspaceEntryCount */

extern uint32_t g_AiPurchaseAppliedArmyClassMask; /* 00537418 g_AiPurchaseAppliedArmyClassMask */

extern AiStructureWorkspaceEntry *g_AiWorkspace00Structures; /* 00537960 g_AiWorkspace00Structures */

extern uint32_t g_AiWorkspace00Count; /* 00537964 g_AiWorkspace00Count */

extern AiRuntimeWorkspaceEntry *g_AiWorkspace01Units; /* 00537968 g_AiWorkspace01Units */

extern uint32_t g_AiWorkspace01Count; /* 0053796C g_AiWorkspace01Count */

extern AiRuntimeWorkspaceEntry *g_AiWorkspace02VisibleHostiles; /* 00537970 g_AiWorkspace02VisibleHostiles */

extern uint32_t g_AiWorkspace02Count; /* 00537974 g_AiWorkspace02Count */

extern AiRuntimeWorkspaceEntry *g_AiWorkspace03UnseenHostiles; /* 00537978 g_AiWorkspace03UnseenHostiles */

extern uint32_t g_AiWorkspace03Count; /* 0053797C g_AiWorkspace03Count */

extern AiRuntimeWorkspaceEntry *g_AiWorkspace04RequestedAssets; /* 00537980 g_AiWorkspace04RequestedAssets */

extern uint32_t g_AiWorkspace04Count; /* 00537984 g_AiWorkspace04Count */

extern AiScoredSiteWorkspaceEntry *g_AiWorkspace05GeneralSites; /* 00537988 g_AiWorkspace05GeneralSites */

extern uint32_t g_AiWorkspace05Count; /* 0053798C g_AiWorkspace05Count */

extern uint8_t *g_AiWorkspace06FlaggedSites; /* 00537990 g_AiWorkspace06FlaggedSites */

extern uint32_t g_AiWorkspace06Count; /* 00537994 g_AiWorkspace06Count */

extern AiTargetWorkspaceEntry *g_AiWorkspace07Targets; /* 00537998 g_AiWorkspace07Targets */

extern uint32_t g_AiWorkspace07Count; /* 0053799C g_AiWorkspace07Count */

extern AiTerrainFeatureWorkspaceEntry *g_AiWorkspace08TerrainFeatureSites; /* 005379A0 g_AiWorkspace08TerrainFeatureSites */

extern uint32_t g_AiWorkspace08Count; /* 005379A4 g_AiWorkspace08Count */

extern FieldGridCell **g_AiWorkspace09Cells; /* 005379A8 g_AiWorkspace09Cells */

extern uint32_t g_AiWorkspace09Count; /* 005379AC g_AiWorkspace09Count */

extern FieldGridCell **g_AiWorkspace10Cells; /* 005379B0 g_AiWorkspace10Cells */

extern uint32_t g_AiWorkspace10Count; /* 005379B4 g_AiWorkspace10Count */

extern ArmyAssetRecordPrefix **g_AiWorkspace11ProducibleAssets; /* 005379B8 g_AiWorkspace11ProducibleAssets */

extern uint32_t g_AiWorkspace11Count; /* 005379BC g_AiWorkspace11Count */

extern AiTechnologyPlanningCandidate *g_AiWorkspace12TechnologyCandidates; /* 005379C0 g_AiWorkspace12TechnologyCandidates */

extern AiTechnologyPlanningCandidateCount g_AiWorkspace12Count; /* 005379C4 g_AiWorkspace12Count */

extern uint32_t g_AiActiveGridMaskClasses[4]; /* 005379C8 g_AiActiveGridMaskClasses */

extern ModelRuntimeSlot *g_AiWorkspaceOwnedAsset300Runtime; /* 005379D8 g_AiWorkspaceOwnedAsset300Runtime */

extern uint32_t g_AiConstructionPendingAssetConsumedCount; /* 00539060 g_AiConstructionPendingAssetConsumedCount */

extern int32_t g_AiStrategicClassTerrainWeights[5][3]; /* 005399C4 g_AiStrategicClassTerrainWeights */

extern ArmyRuntimeSlot **g_AiWorkspace14CollectedArmies; /* 0053B0D0 g_AiWorkspace14CollectedArmies */

extern uint32_t g_AiCollectedEntityCount; /* 0053B0D4 g_AiCollectedEntityCount */

extern AiTechnologyCandidateScoreCallback *g_AiTechnologyCandidateScoreCallbackTable[6]; /* 0053B9E0 g_AiTechnologyCandidateScoreCallbackTable */

extern AiKnowledgeDataImage *g_AiKnowledgeData; /* 0053C5E0 g_AiKnowledgeData */

extern uint16_t u_engine_ki_dat_0053c5e4[14]; /* 0053C5E4 u_engine_ki_dat_0053c5e4 */

extern AiArmyScoreWeights g_AiArmyCandidateScoreWeightsVariantB15; /* 0053C600 g_AiArmyCandidateScoreWeightsVariantB15 */

extern AiArmyScoreWeights g_AiArmyCandidateScoreWeightsVariantA15; /* 0053C63C g_AiArmyCandidateScoreWeightsVariantA15 */

extern AiArmyScoreWeights g_AiArmyCandidateScoreWeightsVariantC15; /* 0053C678 g_AiArmyCandidateScoreWeightsVariantC15 */

#endif

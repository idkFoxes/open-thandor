/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/include/thandor/gameplay/ai/data.h
 */

#ifndef THANDOR_GAMEPLAY_AI_DATA_H
#define THANDOR_GAMEPLAY_AI_DATA_H

#include <thandor/generated/types.h>
#include <thandor/generated/ui_templates.h>

extern TechnologyAsset *g_TechnologyAsset;

extern GameFactionRuntimeImage g_GameFactionRuntimeImage;

extern ArmyCommandGeneration g_AiCommandGenerationCandidateBase;

extern ArmyCommandGeneration g_AiCommandGenerationRetainedTarget; /* ArmyCommandGeneration (uint32_t) 0x200 assigned to an army's commandGeneration by the AI combat code (gameplay/ai/combat.c) */

extern uint32_t g_AiCombatTargetClassBaseScores[24]; /* uint32_t[24] AI combat target base score per target runtime class (gameplay/ai/combat.c) */

extern int32_t g_AiCombatTargetRadialClearanceWeight; /* int32_t weight 0x600 multiplying the radial clearance term of the AI combat target score (gameplay/ai/combat.c) */

extern int32_t g_AiCombatTargetCandidateCounterCountWeight; /* int32_t weight 0x12000 for the candidate class counter term of the AI combat target score, divided by the Q12 hierarchy scale unity (gameplay/ai/combat.c) */

extern int32_t g_AiCombatTargetSourceCounterCountWeight; /* int32_t weight 0x20000 for the source class counter term of the AI combat target score (gameplay/ai/combat.c) */

extern int32_t g_AiCombatTargetScaleDeficitWeight; /* int32_t weight 0x1100 for the hierarchy scale deficit (1.0 - condition ratio) term of the AI combat target score (gameplay/ai/combat.c) */

extern int32_t g_AiCombatTargetClassBaseScoreMultiplier; /* int32_t multiplier 0x800 applied to g_AiCombatTargetClassBaseScores[runtimeClassId] in the AI combat target score (gameplay/ai/combat.c) */

extern AiCommandGenerationRightShiftBits g_AiCombatTargetSelectedCommandGenerationRightShiftBits;

extern AiCommandGenerationRightShiftBits g_AiCombatTargetCurrentCommandGenerationRightShiftBits;

extern AiCandidateWorkspaceEntry *g_AiWorkspace13Candidates;

extern uint32_t g_AiCandidateWorkspaceEntryCount;

extern uint32_t g_AiPurchaseAppliedArmyClassMask;

extern AiStructureWorkspaceEntry *g_AiWorkspace00Structures;

extern uint32_t g_AiWorkspace00Count;

extern AiRuntimeWorkspaceEntry *g_AiWorkspace01Units;

extern uint32_t g_AiWorkspace01Count;

extern AiRuntimeWorkspaceEntry *g_AiWorkspace02VisibleHostiles;

extern uint32_t g_AiWorkspace02Count;

extern AiRuntimeWorkspaceEntry *g_AiWorkspace03UnseenHostiles;

extern uint32_t g_AiWorkspace03Count;

extern AiRuntimeWorkspaceEntry *g_AiWorkspace04RequestedAssets;

extern uint32_t g_AiWorkspace04Count;

extern AiScoredSiteWorkspaceEntry *g_AiWorkspace05GeneralSites;

extern uint32_t g_AiWorkspace05Count;

extern uint8_t *g_AiWorkspace06FlaggedSites;

extern uint32_t g_AiWorkspace06Count;

extern AiTargetWorkspaceEntry *g_AiWorkspace07Targets;

extern uint32_t g_AiWorkspace07Count;

extern AiTerrainFeatureWorkspaceEntry *g_AiWorkspace08TerrainFeatureSites;

extern uint32_t g_AiWorkspace08Count;

extern FieldGridCell **g_AiWorkspace09Cells;

extern uint32_t g_AiWorkspace09Count;

extern FieldGridCell **g_AiWorkspace10Cells;

extern uint32_t g_AiWorkspace10Count;

extern ArmyAssetRecordPrefix **g_AiWorkspace11ProducibleAssets;

extern uint32_t g_AiWorkspace11Count;

extern AiTechnologyPlanningCandidate *g_AiWorkspace12TechnologyCandidates;

extern AiTechnologyPlanningCandidateCount g_AiWorkspace12Count;

extern uint32_t g_AiActiveGridMaskClasses[4];

extern ModelRuntimeSlot *g_AiWorkspaceOwnedAsset300Runtime;

extern uint32_t g_AiConstructionPendingAssetConsumedCount;

extern int32_t g_AiStrategicClassTerrainWeights[5][3];

extern ArmyRuntimeSlot **g_AiWorkspace14CollectedArmies;

extern uint32_t g_AiCollectedEntityCount;

extern AiTechnologyCandidateScoreCallback *g_AiTechnologyCandidateScoreCallbackTable[6];

extern AiKnowledgeDataImage *g_AiKnowledgeData;

extern uint16_t u_engine_ki_dat_0053c5e4[14];

extern AiArmyScoreWeights g_AiArmyCandidateScoreWeightsVariantB15;

extern AiArmyScoreWeights g_AiArmyCandidateScoreWeightsVariantA15;

extern AiArmyScoreWeights g_AiArmyCandidateScoreWeightsVariantC15;

#endif

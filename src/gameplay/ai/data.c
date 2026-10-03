/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/src/gameplay/ai/data.c
 */

/* Data of the original image that this module uses (moved here from the generated image data in
   step 4c); declared in <thandor/gameplay/ai/data.h>. Original addresses in the comments. */

#include <thandor/thandor.h>

#pragma warning(disable : 4152) /* function pointer fields initialized through (void *) */

/* 0050D930 g_TechnologyAsset */
__declspec(align(16)) TechnologyAsset *g_TechnologyAsset = 0;

/* 0050F340 g_GameFactionRuntimeImage */
__declspec(align(16)) GameFactionRuntimeImage g_GameFactionRuntimeImage = {.tail = {.factionLifecycleStates = {0x1, 0x1, 0x1, 0x1, 0x1, 0x1, 0x1, 0x1}}};

/* 0051B38C g_AiCommandGenerationCandidateBase */
__declspec(align(4)) ArmyCommandGeneration g_AiCommandGenerationCandidateBase = 32;

/* 0051B390 g_AiCommandGenerationRetainedTarget: ArmyCommandGeneration (uint32_t) 0x200 assigned to an army's commandGeneration by the AI combat code (gameplay/ai/combat.c) */
__declspec(align(16)) ArmyCommandGeneration g_AiCommandGenerationRetainedTarget = 512;

/* 0051FB78 g_AiCombatTargetClassBaseScores: uint32_t[24] AI combat target base score per target runtime class (gameplay/ai/combat.c) */
__declspec(align(8)) uint32_t g_AiCombatTargetClassBaseScores[24] = {
    /*  0 */ 0, 2048, 4096, 4096, 512, 0, 0, 0, 0, 0, 384, 256, 0, 128, 1024, 896,
    /* 16 */ 768, 4096, 6144, 4096, 256, 4096, 192, 64};

/* 00536FA0 g_AiCombatTargetRadialClearanceWeight: int32_t weight 0x600 multiplying the radial clearance term of the AI combat target score (gameplay/ai/combat.c) */
__declspec(align(16)) int32_t g_AiCombatTargetRadialClearanceWeight = 1536;

/* 00536FA4 g_AiCombatTargetCandidateCounterCountWeight: int32_t weight 0x12000 for the candidate class counter term of the AI combat target score, divided by the Q12 hierarchy scale unity (gameplay/ai/combat.c) */
__declspec(align(4)) int32_t g_AiCombatTargetCandidateCounterCountWeight = 73728;

/* 00536FA8 g_AiCombatTargetSourceCounterCountWeight: int32_t weight 0x20000 for the source class counter term of the AI combat target score (gameplay/ai/combat.c) */
__declspec(align(8)) int32_t g_AiCombatTargetSourceCounterCountWeight = 131072;

/* 00536FAC g_AiCombatTargetScaleDeficitWeight: int32_t weight 0x1100 for the hierarchy scale deficit (1.0 - condition ratio) term of the AI combat target score (gameplay/ai/combat.c) */
__declspec(align(4)) int32_t g_AiCombatTargetScaleDeficitWeight = 4352;

/* 00536FB0 g_AiCombatTargetClassBaseScoreMultiplier: int32_t multiplier 0x800 applied to g_AiCombatTargetClassBaseScores[runtimeClassId] in the AI combat target score (gameplay/ai/combat.c) */
__declspec(align(16)) int32_t g_AiCombatTargetClassBaseScoreMultiplier = 2048;

/* 00536FB4 g_AiCombatTargetSelectedCommandGenerationRightShiftBits */
__declspec(align(4)) AiCommandGenerationRightShiftBits g_AiCombatTargetSelectedCommandGenerationRightShiftBits = 0;

/* 00536FB8 g_AiCombatTargetCurrentCommandGenerationRightShiftBits */
__declspec(align(8)) AiCommandGenerationRightShiftBits g_AiCombatTargetCurrentCommandGenerationRightShiftBits = 0;

/* 00537410 g_AiWorkspace13Candidates */
__declspec(align(16)) AiCandidateWorkspaceEntry *g_AiWorkspace13Candidates = 0;

/* 00537414 g_AiCandidateWorkspaceEntryCount */
__declspec(align(4)) uint32_t g_AiCandidateWorkspaceEntryCount = 0;

/* 00537418 g_AiPurchaseAppliedArmyClassMask */
__declspec(align(8)) uint32_t g_AiPurchaseAppliedArmyClassMask = 0;

/* 00537960 g_AiWorkspace00Structures */
__declspec(align(16)) AiStructureWorkspaceEntry *g_AiWorkspace00Structures = 0;

/* 00537964 g_AiWorkspace00Count */
__declspec(align(4)) uint32_t g_AiWorkspace00Count = 0;

/* 00537968 g_AiWorkspace01Units */
__declspec(align(8)) AiRuntimeWorkspaceEntry *g_AiWorkspace01Units = 0;

/* 0053796C g_AiWorkspace01Count */
__declspec(align(4)) uint32_t g_AiWorkspace01Count = 0;

/* 00537970 g_AiWorkspace02VisibleHostiles */
__declspec(align(16)) AiRuntimeWorkspaceEntry *g_AiWorkspace02VisibleHostiles = 0;

/* 00537974 g_AiWorkspace02Count */
__declspec(align(4)) uint32_t g_AiWorkspace02Count = 0;

/* 00537978 g_AiWorkspace03UnseenHostiles */
__declspec(align(8)) AiRuntimeWorkspaceEntry *g_AiWorkspace03UnseenHostiles = 0;

/* 0053797C g_AiWorkspace03Count */
__declspec(align(4)) uint32_t g_AiWorkspace03Count = 0;

/* 00537980 g_AiWorkspace04RequestedAssets */
__declspec(align(16)) AiRuntimeWorkspaceEntry *g_AiWorkspace04RequestedAssets = 0;

/* 00537984 g_AiWorkspace04Count */
__declspec(align(4)) uint32_t g_AiWorkspace04Count = 0;

/* 00537988 g_AiWorkspace05GeneralSites */
__declspec(align(8)) AiScoredSiteWorkspaceEntry *g_AiWorkspace05GeneralSites = 0;

/* 0053798C g_AiWorkspace05Count */
__declspec(align(4)) uint32_t g_AiWorkspace05Count = 0;

/* 00537990 g_AiWorkspace06FlaggedSites */
__declspec(align(16)) uint8_t *g_AiWorkspace06FlaggedSites = 0;

/* 00537994 g_AiWorkspace06Count */
__declspec(align(4)) uint32_t g_AiWorkspace06Count = 0;

/* 00537998 g_AiWorkspace07Targets */
__declspec(align(8)) AiTargetWorkspaceEntry *g_AiWorkspace07Targets = 0;

/* 0053799C g_AiWorkspace07Count */
__declspec(align(4)) uint32_t g_AiWorkspace07Count = 0;

/* 005379A0 g_AiWorkspace08TerrainFeatureSites */
__declspec(align(16)) AiTerrainFeatureWorkspaceEntry *g_AiWorkspace08TerrainFeatureSites = 0;

/* 005379A4 g_AiWorkspace08Count */
__declspec(align(4)) uint32_t g_AiWorkspace08Count = 0;

/* 005379A8 g_AiWorkspace09Cells */
__declspec(align(8)) FieldGridCell **g_AiWorkspace09Cells = 0;

/* 005379AC g_AiWorkspace09Count */
__declspec(align(4)) uint32_t g_AiWorkspace09Count = 0;

/* 005379B0 g_AiWorkspace10Cells */
__declspec(align(16)) FieldGridCell **g_AiWorkspace10Cells = 0;

/* 005379B4 g_AiWorkspace10Count */
__declspec(align(4)) uint32_t g_AiWorkspace10Count = 0;

/* 005379B8 g_AiWorkspace11ProducibleAssets */
__declspec(align(8)) ArmyAssetRecordPrefix **g_AiWorkspace11ProducibleAssets = 0;

/* 005379BC g_AiWorkspace11Count */
__declspec(align(4)) uint32_t g_AiWorkspace11Count = 0;

/* 005379C0 g_AiWorkspace12TechnologyCandidates */
__declspec(align(16)) AiTechnologyPlanningCandidate *g_AiWorkspace12TechnologyCandidates = 0;

/* 005379C4 g_AiWorkspace12Count */
__declspec(align(4)) AiTechnologyPlanningCandidateCount g_AiWorkspace12Count = 0;

/* 005379C8 g_AiActiveGridMaskClasses */
__declspec(align(8)) uint32_t g_AiActiveGridMaskClasses[4] = {0, 0, 0, 0};

/* 005379D8 g_AiWorkspaceOwnedAsset300Runtime */
__declspec(align(8)) ModelRuntimeSlot *g_AiWorkspaceOwnedAsset300Runtime = 0;

/* 00539060 g_AiConstructionPendingAssetConsumedCount */
__declspec(align(16)) uint32_t g_AiConstructionPendingAssetConsumedCount = 0;

/* 005399C4 g_AiStrategicClassTerrainWeights */
__declspec(align(4)) int32_t g_AiStrategicClassTerrainWeights[5][3] = {
    {256, 0, 0},
    {240, 16, 0},
    {0, 176, 80},
    {0, 0, 256},
    {0, 0, 0}};

/* 0053B0D0 g_AiWorkspace14CollectedArmies */
__declspec(align(16)) ArmyRuntimeSlot **g_AiWorkspace14CollectedArmies = 0;

/* 0053B0D4 g_AiCollectedEntityCount */
__declspec(align(4)) uint32_t g_AiCollectedEntityCount = 0;

/* 0053B9E0 g_AiTechnologyCandidateScoreCallbackTable */
__declspec(align(16)) AiTechnologyCandidateScoreCallback *g_AiTechnologyCandidateScoreCallbackTable[6] = {
    /* 0 */ (void *)AiTechnologyScore_AlwaysZero,
    /* 1 */ (void *)AiTechnologyScore_ComputeFactionScaledCandidateValue,
    /* 2 */ (void *)AiTechnologyScore_ReturnBaseCandidateValueForKind2,
    /* 3 */ (void *)AiTechnologyScore_ComputeRuntimeClassCompatibleCandidateValue,
    /* 4 */ (void *)AiTechnologyScore_ReturnBaseCandidateValueForKind4,
    /* 5 */ (void *)AiTechnologyScore_ComputeCategoryCompatibleCandidateValue};

/* 0053C5E0 g_AiKnowledgeData */
__declspec(align(16)) AiKnowledgeDataImage *g_AiKnowledgeData = 0;

/* 0053C5E4 u_engine_ki_dat_0053c5e4 */
__declspec(align(4)) uint16_t u_engine_ki_dat_0053c5e4[14] = L"engine\\ki.dat";

/* 0053C600 g_AiArmyCandidateScoreWeightsVariantB15 */
__declspec(align(16)) AiArmyScoreWeights g_AiArmyCandidateScoreWeightsVariantB15 = {
    .pressureCategoryWeights = {0, 128, 128, 128, 128, 128, 128, 128},
    .definitionValue0CWeight = 512,
    .armyRecord74Weight = 128,
    .armyRecord70Weight = 512,
    .nonzeroDefinition18Bonus = 4096,
    .baseScore = -4096};

/* 0053C63C g_AiArmyCandidateScoreWeightsVariantA15 */
__declspec(align(4)) AiArmyScoreWeights g_AiArmyCandidateScoreWeightsVariantA15 = {
    .pressureCategoryWeights = {0, 512, 512, 512, 512, 640, 512},
    .definitionValue60Weight = 512,
    .definitionValue0CWeight = -64,
    .armyRecord74Weight = 512,
    .armyRecord78Weight = 256,
    .nonzeroDefinition18Bonus = -256};

/* 0053C678 g_AiArmyCandidateScoreWeightsVariantC15 */
__declspec(align(8)) AiArmyScoreWeights g_AiArmyCandidateScoreWeightsVariantC15 = {
    .pressureCategoryWeights = {0, 384, 384, 384, 384, 448, 384, 512},
    .definitionValue60Weight = 384,
    .definitionValue0CWeight = 192,
    .armyRecord74Weight = 256,
    .armyRecord78Weight = 512,
    .nonzeroDefinition18Bonus = 4096,
    .baseScore = -4096};

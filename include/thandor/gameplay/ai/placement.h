#ifndef THANDOR_GAMEPLAY_AI_PLACEMENT_H
#define THANDOR_GAMEPLAY_AI_PLACEMENT_H

#include <thandor/generated/v523_types.h>
#include <thandor/core/contracts.h>

/* Submodule: gameplay/ai/placement. */
/* Functions are grouped by semantic ownership; address comments are executable virtual addresses. */

/* 0x0053A110 */
void AiPlacement_ReserveAdditionalSpecialSite (PckArmyAssetIdCatalog armyAssetId,FieldGridCell *workspaceRecord, FactionRuntimeIndex factionIndex,WorldRuntimeContext *worldRuntime);

/* 0x0053AE60 */
AiPreservedFactionIndexEdxResult AiCandidatePlanning_AddSpecialSiteCandidate (FactionRuntimeIndex factionIndex,WorldRuntimeContext *worldRuntime);

/* 0x00537B20 */
AiPlanningGridScratchEdxContinuityResult AiSiteCandidate_AddGeneralCellIfSeparated(void);

/* 0x00537C10 */
AiPlanningGridScratchEdxContinuityResult AiSiteCandidate_AddFlaggedCellIfSeparated(void);

/* 0x00537CF0 */
AiPlanningGridScratchEdxContinuityResult AiSiteCandidate_AddTerrainFeatureCellIfSeparated(void);

/* 0x00539200 */
undefined4 AiPlacement_TestWorkspaceRecordAtPoint (PckArmyAssetIdCatalog armyAssetId,FieldGridCell *workspaceRecord, ArmyPlacementContext placementContext,UiRootNode *inGameRoot);

/* 0x0053A1B0 */
undefined4 AiPlacement_TestMode4AtWorkspaceRecord (PckArmyAssetIdCatalog armyAssetId,FieldGridCell *workspaceRecord, FactionRuntimeIndex factionIndex,WorldRuntimeContext *worldRuntime);

/* 0x0053B570 */
dword AiPlacement_QueryReachableSiteBucketCount (PckArmyAssetIdCatalog armyAssetId,FieldGridCell *workspaceRecord, FactionRuntimeIndex factionIndex,WorldRuntimeContext *worldRuntime);

/* 0x0053ACD0 */
void AiPlacement_ReserveMode3SiteCluster (PckArmyAssetIdCatalog armyAssetId,FieldGridCell *workspaceRecord, FactionRuntimeIndex factionIndex,WorldRuntimeContext *worldRuntime);

/* 0x0053AD50 */
AiCandidateScoreCfEaxPreservedEdxCarrier64 AiCandidatePlanning_ComputeSpecialSiteWeight (FactionRuntimeIndex factionIndex,WorldRuntimeContext *worldRuntime);

/* 0x00539330 */
AiWorkspace09AnchorCfRegisterResult AiPlacement_FindNearestValidWorkspace09Anchor (Q12 referenceWorldXQ12,Q12 referenceWorldYQ12,PckArmyAssetIdCatalog armyAssetId, FactionRuntimeIndex factionIndex,WorldRuntimeContext *worldRuntime);

/* 0x00539EF0 */
void __fastcall AiPlacement_ReserveSeparatedSpecialSiteChain (undefined4 param_1,uint param_2,undefined4 param_3,FieldGridCell *workspaceRecord, FactionRuntimeIndex factionIndex,WorldRuntimeContext *worldRuntime);

#endif /* THANDOR_GAMEPLAY_AI_PLACEMENT_H */

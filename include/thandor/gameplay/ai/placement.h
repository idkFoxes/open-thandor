/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/include/thandor/gameplay/ai/placement.h
 * Reverse engineering by idkFoxes 2026
 */

#ifndef THANDOR_GAMEPLAY_AI_PLACEMENT_H
#define THANDOR_GAMEPLAY_AI_PLACEMENT_H

#include <thandor/generated/types.h>
#include <thandor/core/contracts.h>

/* Submodule: gameplay/ai/placement. */
/* Functions are grouped by semantic ownership; address comments are executable virtual addresses. */

/* 0x0053A110 */
bool AiPlacement_ReserveAdditionalSpecialSite(PckArmyAssetIdCatalog armyAssetId,FieldGridCell *workspaceRecord,
          FactionRuntimeIndex factionIndex,WorldRuntimeContext *worldRuntime);

/* 0x0053AE60 */
void AiCandidatePlanning_AddSpecialSiteCandidate(FactionRuntimeIndex factionIndex,WorldRuntimeContext *worldRuntime);

/* 0x00537B20 */
void AiSiteCandidate_AddGeneralCellIfSeparated(FieldGridCell *currentCell);

/* 0x00537C10 */
void AiSiteCandidate_AddFlaggedCellIfSeparated(FieldGridCell *currentCell);

/* 0x00537CF0 */
void AiSiteCandidate_AddTerrainFeatureCellIfSeparated
          (FieldGridCell *terrainFeatureCell,uint32_t gridScratchRowStrideBytes);

/* 0x00539200 */
bool AiPlacement_TestWorkspaceRecordAtPoint(PckArmyAssetIdCatalog armyAssetId,FieldGridCell *workspaceRecord,
          ArmyPlacementContext placementContext,UiRootNode *inGameRoot);

/* 0x0053A1B0 */
bool AiPlacement_TestMode4AtWorkspaceRecord(PckArmyAssetIdCatalog armyAssetId,FieldGridCell *workspaceRecord,
          FactionRuntimeIndex factionIndex,WorldRuntimeContext *worldRuntime);

/* 0x0053B570 */
StatusResult AiPlacement_QueryReachableSiteBucketCount(PckArmyAssetIdCatalog armyAssetId,FieldGridCell *workspaceRecord,
          FactionRuntimeIndex factionIndex,WorldRuntimeContext *worldRuntime);

/* 0x0053ACD0 */
bool AiPlacement_ReserveMode3SiteCluster(PckArmyAssetIdCatalog armyAssetId,FieldGridCell *workspaceRecord,
          FactionRuntimeIndex factionIndex,WorldRuntimeContext *worldRuntime);

/* 0x0053AD50 */
SiteWeightResult AiCandidatePlanning_ComputeSpecialSiteWeight
          (FactionRuntimeIndex factionIndex,WorldRuntimeContext *worldRuntime);

/* 0x00539330 */
AiAnchorResult AiPlacement_FindNearestPlaceableBaseSite
          (Q12 referenceWorldYQ12,Q12 referenceWorldXQ12,PckArmyAssetIdCatalog armyAssetId,
          FactionRuntimeIndex factionIndex,WorldRuntimeContext *worldRuntime);

/* 0x00539EF0 */
bool AiPlacement_ReserveSeparatedSpecialSiteChain(PckArmyAssetIdCatalog armyAssetId,FieldGridCell *workspaceRecord,
          FactionRuntimeIndex factionIndex,WorldRuntimeContext *worldRuntime);

#endif /* THANDOR_GAMEPLAY_AI_PLACEMENT_H */

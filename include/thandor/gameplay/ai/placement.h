/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/include/thandor/gameplay/ai/placement.h
 * Reverse engineering by idkFoxes 2026
 */

#ifndef THANDOR_GAMEPLAY_AI_PLACEMENT_H
#define THANDOR_GAMEPLAY_AI_PLACEMENT_H

#include <thandor/core/types.h>
#include <thandor/gameplay/ai/types.h>
#include <thandor/ui/controls/types.h>
#include <thandor/ui/ingame/types.h>
#include <thandor/world/terrain/types.h>
#include <thandor/core/contracts.h>

bool AiPlacement_ReserveAdditionalSpecialSite(PckArmyAssetIdCatalog armyAssetId,FieldGridCell *workspaceRecord,
          FactionRuntimeIndex factionIndex,WorldRuntimeContext *worldRuntime);

void AiCandidatePlanning_AddSpecialSiteCandidate(FactionRuntimeIndex factionIndex,WorldRuntimeContext *worldRuntime);

void AiSiteCandidate_AddGeneralCellIfSeparated(FieldGridCell *currentCell);

void AiSiteCandidate_AddFlaggedCellIfSeparated(FieldGridCell *currentCell);

void AiSiteCandidate_AddTerrainFeatureCellIfSeparated
          (FieldGridCell *terrainFeatureCell,uint32_t gridScratchRowStrideBytes);

bool AiPlacement_TestWorkspaceRecordAtPoint(PckArmyAssetIdCatalog armyAssetId,FieldGridCell *workspaceRecord,
          ArmyPlacementContext placementContext,WorldRuntimeContext *worldRuntime);

bool AiPlacement_TestMode4AtWorkspaceRecord(PckArmyAssetIdCatalog armyAssetId,FieldGridCell *workspaceRecord,
          FactionRuntimeIndex factionIndex,WorldRuntimeContext *worldRuntime);

bool AiPlacement_QueryReachableSiteBucketCount(PckArmyAssetIdCatalog armyAssetId,FieldGridCell *workspaceRecord,
          FactionRuntimeIndex factionIndex,WorldRuntimeContext *worldRuntime,uint32_t *outBucketCount);

bool AiPlacement_ReserveMode3SiteCluster(PckArmyAssetIdCatalog armyAssetId,FieldGridCell *workspaceRecord,
          FactionRuntimeIndex factionIndex,WorldRuntimeContext *worldRuntime);

bool AiCandidatePlanning_ComputeSpecialSiteWeight
          (FactionRuntimeIndex factionIndex,WorldRuntimeContext *worldRuntime,uint32_t *outWeight);

bool AiPlacement_FindNearestPlaceableBaseSite
          (Q12 referenceWorldYQ12,Q12 referenceWorldXQ12,PckArmyAssetIdCatalog armyAssetId,
          FactionRuntimeIndex factionIndex,WorldRuntimeContext *worldRuntime,Q12 *outWorldYQ12,
          Q12 *outWorldXQ12);

bool AiPlacement_ReserveSeparatedSpecialSiteChain(PckArmyAssetIdCatalog armyAssetId,FieldGridCell *workspaceRecord,
          FactionRuntimeIndex factionIndex,WorldRuntimeContext *worldRuntime);

extern AiKnowledgeDataImage *g_AiKnowledgeData;

#endif /* THANDOR_GAMEPLAY_AI_PLACEMENT_H */

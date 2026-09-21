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
bool __thandor_cf_preserve_eax_ecx_edx
AiPlacement_ReserveAdditionalSpecialSite
          (PckArmyAssetIdCatalog armyAssetId,FieldGridCell *workspaceRecord,
          FactionRuntimeIndex factionIndex,WorldRuntimeContext *worldRuntime);

/* 0x0053AE60 */
void __thandor_void_preserve_eax_ecx_edx
AiCandidatePlanning_AddSpecialSiteCandidate
          (FactionRuntimeIndex factionIndex,WorldRuntimeContext *worldRuntime);

/* 0x00537B20 */
void __thandor_void_preserve_ecx_edx
AiSiteCandidate_AddGeneralCellIfSeparated(FieldGridCell *currentCell);

/* 0x00537C10 */
void __thandor_void_preserve_ecx_edx
AiSiteCandidate_AddFlaggedCellIfSeparated(FieldGridCell *currentCell);

/* 0x00537CF0 */
void __thandor_void_preserve_ecx_edx
AiSiteCandidate_AddTerrainFeatureCellIfSeparated
          (FieldGridCell *terrainFeatureCell,dword gridScratchRowStrideBytes);

/* 0x00539200 */
bool __thandor_cf_preserve_eax_ecx_edx
AiPlacement_TestWorkspaceRecordAtPoint
          (PckArmyAssetIdCatalog armyAssetId,FieldGridCell *workspaceRecord,
          ArmyPlacementContext placementContext,UiRootNode *inGameRoot);

/* 0x0053A1B0 */
bool __thandor_cf_preserve_eax_ecx_edx
AiPlacement_TestMode4AtWorkspaceRecord
          (PckArmyAssetIdCatalog armyAssetId,FieldGridCell *workspaceRecord,
          FactionRuntimeIndex factionIndex,WorldRuntimeContext *worldRuntime);

/* 0x0053B570 */
StatusValueEaxCf5 __thandor_eax_cf_preserve_ecx_edx
AiPlacement_QueryReachableSiteBucketCount
          (PckArmyAssetIdCatalog armyAssetId,FieldGridCell *workspaceRecord,
          FactionRuntimeIndex factionIndex,WorldRuntimeContext *worldRuntime);

/* 0x0053ACD0 */
bool __thandor_cf_preserve_eax_ecx_edx
AiPlacement_ReserveMode3SiteCluster
          (PckArmyAssetIdCatalog armyAssetId,FieldGridCell *workspaceRecord,
          FactionRuntimeIndex factionIndex,WorldRuntimeContext *worldRuntime);

/* 0x0053AD50 */
AiCandidateScoreEaxCf5 __thandor_eax_cf_preserve_ecx_edx
AiCandidatePlanning_ComputeSpecialSiteWeight
          (FactionRuntimeIndex factionIndex,WorldRuntimeContext *worldRuntime);

/* 0x00539330 */
AiWorkspace09AnchorEcxEdxCf9 __thandor_preserve_eax
AiPlacement_FindNearestValidWorkspace09Anchor
          (Q12 referenceWorldXQ12,Q12 referenceWorldYQ12,PckArmyAssetIdCatalog armyAssetId,
          FactionRuntimeIndex factionIndex,WorldRuntimeContext *worldRuntime);

/* 0x00539EF0 */
bool __thandor_cf_preserve_eax_ecx_edx
AiPlacement_ReserveSeparatedSpecialSiteChain
          (PckArmyAssetIdCatalog armyAssetId,FieldGridCell *workspaceRecord,
          FactionRuntimeIndex factionIndex,WorldRuntimeContext *worldRuntime);

#endif /* THANDOR_GAMEPLAY_AI_PLACEMENT_H */

/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/include/thandor/gameplay/ai/workspaces.h
 * Reverse engineering by idkFoxes 2026
 */

#ifndef THANDOR_GAMEPLAY_AI_WORKSPACES_H
#define THANDOR_GAMEPLAY_AI_WORKSPACES_H

#include <thandor/generated/types.h>
#include <thandor/core/contracts.h>

/* Submodule: gameplay/ai/workspaces. */

/* Entry capacities of the AI workspace buffers allocated by AiRuntime_InitWorkspace (buffer size / entry size). */
#define AI_WORKSPACE00_CAPACITY 128 /* own structures: 0x400 bytes of 8-byte entries */
#define AI_WORKSPACE01_CAPACITY 64 /* own units: 0x200 bytes of 8-byte AiRuntimeWorkspaceEntry */
#define AI_WORKSPACE02_CAPACITY 128 /* visible hostiles: 0x400 bytes */
#define AI_WORKSPACE03_CAPACITY 512 /* unseen hostiles: 0x1000 bytes */
#define AI_WORKSPACE04_CAPACITY 8 /* requested assets: 0x40 bytes */
#define AI_WORKSPACE07_CAPACITY 64 /* targets: 0x400 bytes of 16-byte AiTargetWorkspaceEntry */
#define AI_WORKSPACE11_CAPACITY 1024 /* producible assets: 0x1000 bytes of ArmyAssetRecordPrefix pointers */
#define AI_WORKSPACE05_CAPACITY 32 /* general sites: 0x200 bytes of 16-byte AiScoredSiteWorkspaceEntry */
#define AI_WORKSPACE06_CAPACITY 64 /* flagged sites: 0x400 bytes of 16-byte entries */
#define AI_WORKSPACE08_CAPACITY 32 /* terrain-feature sites: 0x200 bytes of 16-byte AiTerrainFeatureWorkspaceEntry */
#define AI_WORKSPACE09_CAPACITY 1024 /* 0x1000 bytes of FieldGridCell pointers */
#define AI_WORKSPACE10_CAPACITY 256 /* 0x400 bytes of FieldGridCell pointers */
#define AI_WORKSPACE12_CAPACITY 32 /* technology candidates: 0x200 bytes of 16-byte AiTechnologyPlanningCandidate */
#define AI_CANDIDATE_WORKSPACE_CAPACITY 128 /* workspace 13: 0x400 bytes of 8-byte AiCandidateWorkspaceEntry */
#define AI_WORKSPACE14_CAPACITY 64 /* collected idle units: 0x100 bytes of ArmyRuntimeSlot pointers */
/* AiCandidateWorkspaceEntry: weightedScoreAndKind = score << 4 | kind, entityIdAndMultiplicity = count << 16 | id */
#define AI_CANDIDATE_KIND_MASK 0xF
#define AI_CANDIDATE_KIND_TECHNOLOGY 2 /* the id is a technology id; the other kinds carry an army asset id */
#define AI_CANDIDATE_ID_MASK 0xFFFF
#define AI_CANDIDATE_MULTIPLICITY_ONE 0x10000
#define AI_CANDIDATE_SCORE_ONE 16 /* weightedScoreAndKind: one score unit above the 4 kind bits */
/* AiPlanning_RebuildFactionWorkspaces site scan: scratch-grid state bits (world/pathing/grid.h) that rule a
   site out - map edge and terrain classes 28..30 - and the low distance bands of grid classes 0..5 */
#define AI_SITE_SCRATCH_OBSTACLE_BITS \
  (GRID_SCRATCH_BLOCKED | GRID_SCRATCH_TERRAIN_CLASS_BIT30 | GRID_SCRATCH_TERRAIN_CLASS_BIT29 | \
   GRID_SCRATCH_TERRAIN_CLASS_BIT28) /* 0xf0000000 */
#define AI_SITE_SCRATCH_BANDS_CLASSES_0_TO_5 (0x3f * GRID_SCRATCH_LOW_BAND0) /* 0x3f00 */
/* Functions are grouped by semantic ownership; address comments are executable virtual addresses. */

void AiWorkspaceAssetCandidate_AddWeightedEntry(AiCandidateScore32 baseWeight,PckArmyAssetIdCatalog armyAssetId,
          FactionRuntimeIndex factionIndex,WorldRuntimeContext *worldRuntime);

void AiPlanning_RebuildFactionWorkspaces(AiPlanningPhaseIndex planningPhaseDispatchIndex,
          FactionRuntimeIndex factionRuntimeIndexRegisterCopy,FactionRuntimeIndex factionIndex,
          WorldRuntimeContext *worldRuntime);

void AiTechnologyCandidate_AddBestResearch(FactionRuntimeIndex factionIndex,WorldRuntimeContext *worldRuntime);

void AiCandidateWorkspace_Clear(void);

void AiCandidateWorkspace_SaveToFactionImage(FactionImageByteOffset factionImageByteOffset);

void AiCandidateWorkspace_LoadFromFactionImage(FactionImageByteOffset factionImageByteOffset);

void AiCandidateWorkspace_SortDescending(void);

int AiCandidateWorkspace_GetEntryXeniteCost(AiCandidateWorkspaceEntry *entry);

bool AiSecondaryWorkspace_HasEntryById(PckArmyAssetIdCatalog entryId);

int AiSecondaryWorkspace_GetMinimumManhattanDistanceToPoint(Q12 worldY,Q12 worldX);

int AiPrimaryWorkspace_GetMinimumActiveManhattanDistanceToPoint(Q12 worldY,Q12 worldX);

int AiHostileWorkspace_GetNearestVisibleHostileDistance(Q12 worldY,Q12 worldX);

int AiHostileWorkspace_GetNearestUnseenHostileDistance(Q12 worldY,Q12 worldX);

int AiPrimaryWorkspace_GetMinimumManhattanDistanceToPoint(Q12 worldY,Q12 worldX);

void AiConstructionPlanner_PlaceSpecialAssetFromWorkspace
          (PckArmyAssetIdCatalog armyAssetId,FactionRuntimeIndex factionIndex,
          WorldRuntimeContext *worldRuntime);

AiTechnologyCandidateScore AiTechnologyScore_AlwaysZero
          (FactionRuntimeIndex factionIndex,PckTechnologyIdCatalog technologyId,
          WorldRuntimeContext *worldRuntime);

bool AiRuntime_InitWorkspace(uint32_t *outErrorCode);

void AiBaseSiteWorkspace_AddCellInsideBase(FieldGridCell *currentCell);

void AiBaseSiteWorkspace_AddLargeCellInsideBase(FieldGridCell *currentCell);

bool AiPrimaryWorkspace_HasUnassignedEntryById(PckArmyAssetIdCatalog entryId);

bool AiPrimaryWorkspace_HasEntryById(PckArmyAssetIdCatalog entryId);

int AiPrimaryWorkspace_CountAssignedEntriesById(PckArmyAssetIdCatalog entryId);

void AiCandidateWorkspace_AddOrAccumulateWeightedEntry
          (RuntimeToken entityId,uint32_t weightRange,AiCandidateEntryKind entryKind);

bool AiPrimaryWorkspace_IsPointOutsideAllEntryExtents(Q12 worldY,Q12 worldX);

#endif /* THANDOR_GAMEPLAY_AI_WORKSPACES_H */

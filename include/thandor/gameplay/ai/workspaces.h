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
/* Functions are grouped by semantic ownership; address comments are executable virtual addresses. */

/* 0x0053A1E0 */
void AiWorkspaceAssetCandidate_AddWeightedEntry(AiCandidateScore32 baseWeight,PckArmyAssetIdCatalog armyAssetId,
          FactionRuntimeIndex factionIndex,WorldRuntimeContext *worldRuntime);

/* 0x00538230 */
void AiPlanning_RebuildFactionWorkspaces(AiPlanningPhaseIndex planningPhaseDispatchIndex,
          FactionRuntimeIndex factionRuntimeIndexRegisterCopy,FactionRuntimeIndex factionIndex,
          WorldRuntimeContext *worldRuntime);

/* 0x0053BF30 */
void AiTechnologyCandidate_AddBestResearch(FactionRuntimeIndex factionIndex,WorldRuntimeContext *worldRuntime);

/* 0x00537420 */
void AiCandidateWorkspace_Clear(void);

/* 0x00537430 */
void AiCandidateWorkspace_SaveToFactionImage(FactionImageByteOffset factionImageByteOffset);

/* 0x00537470 */
void AiCandidateWorkspace_LoadFromFactionImage(FactionImageByteOffset factionImageByteOffset);

/* 0x00537570 */
void AiCandidateWorkspace_SortDescending(void);

/* 0x005375D0 */
int AiCandidateWorkspace_GetEntryXeniteCost(AiCandidateWorkspaceEntry *entry);

/* 0x00538C90 */
bool AiSecondaryWorkspace_HasUnassignedEntryById(PckArmyAssetIdCatalog entryId);

/* 0x00538CF0 */
bool AiSecondaryWorkspace_HasEntryById(PckArmyAssetIdCatalog entryId);

/* 0x00538D40 */
int AiPrimaryWorkspace_CountAssignedEntriesByIdDuplicate(PckArmyAssetIdCatalog entryId);

/* 0x00538D90 */
int AiSecondaryWorkspace_GetMinimumManhattanDistanceToPoint(Q12 worldY,Q12 worldX);

/* 0x00538E00 */
int AiPrimaryWorkspace_GetMinimumActiveManhattanDistanceToPoint(Q12 worldY,Q12 worldX);

/* 0x00538E80 */
int AiHostileWorkspace_GetNearestVisibleHostileDistance(Q12 worldY,Q12 worldX);

/* 0x00538EF0 */
int AiHostileWorkspace_GetNearestUnseenHostileDistance(Q12 worldY,Q12 worldX);

/* 0x00538F60 */
int AiPrimaryWorkspace_GetMinimumManhattanDistanceToPoint(Q12 worldY,Q12 worldX);

/* 0x00539240 */
void AiConstructionPlanner_PlaceSpecialAssetFromWorkspace
          (PckArmyAssetIdCatalog armyAssetId,FactionRuntimeIndex factionIndex,
          WorldRuntimeContext *worldRuntime);

/* 0x0053BCB0 */
AiTechnologyCandidateScore AiTechnologyScore_AlwaysZero
          (FactionRuntimeIndex factionIndex,PckTechnologyIdCatalog technologyId,
          WorldRuntimeContext *worldRuntime);

/* 0x0053C6C0 */
StatusResult AiRuntime_InitWorkspace(void);

/* 0x00537F80 */
void AiBaseSiteWorkspace_AddCellInsideBase(FieldGridCell *currentCell);

/* 0x00537FC0 */
void AiBaseSiteWorkspace_AddLargeCellInsideBase(FieldGridCell *currentCell);

/* 0x00538B90 */
bool AiPrimaryWorkspace_HasUnassignedEntryById(PckArmyAssetIdCatalog entryId);

/* 0x00538BF0 */
bool AiPrimaryWorkspace_HasEntryById(PckArmyAssetIdCatalog entryId);

/* 0x00538C40 */
int AiPrimaryWorkspace_CountAssignedEntriesById(PckArmyAssetIdCatalog entryId);

/* 0x005374B0 */
void AiCandidateWorkspace_AddOrAccumulateWeightedEntry
          (RuntimeToken entityId,uint32_t weightRange,AiCandidateEntryKind entryKind);

/* 0x00538FD0 */
bool AiPrimaryWorkspace_IsPointOutsideAllEntryExtents(Q12 worldY,Q12 worldX);

#endif /* THANDOR_GAMEPLAY_AI_WORKSPACES_H */

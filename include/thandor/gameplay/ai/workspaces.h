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
/* Functions are grouped by semantic ownership; address comments are executable virtual addresses. */

/* 0x0053A1E0 */
void __thandor_void_preserve_eax_ecx_edx
AiWorkspaceAssetCandidate_AddWeightedEntry
          (AiCandidateScore32 baseWeight,PckArmyAssetIdCatalog armyAssetId,
          FactionRuntimeIndex factionIndex,WorldRuntimeContext *worldRuntime);

/* 0x00538230 */
void __thandor_void_preserve_eax_ecx_edx
AiPlanning_RebuildFactionWorkspaces
          (AiPlanningPhaseIndex planningPhaseDispatchIndex,
          FactionRuntimeIndex factionRuntimeIndexRegisterCopy,FactionRuntimeIndex factionIndex,
          WorldRuntimeContext *worldRuntime);

/* 0x0053BF30 */
void __thandor_void_preserve_eax_ecx_edx
AiStrategicCandidate_AddBestWorkspace12Entry
          (FactionRuntimeIndex factionIndex,WorldRuntimeContext *worldRuntime);

/* 0x00537420 */
void __thandor_void_preserve_eax_ecx_edx AiCandidateWorkspace_Clear(void);

/* 0x00537430 */
void AiCandidateWorkspace_SaveToFactionImage(FactionImageByteOffset factionImageByteOffset);

/* 0x00537470 */
void __thandor_void_preserve_eax_ecx_edx
AiCandidateWorkspace_LoadFromFactionImage(FactionImageByteOffset factionImageByteOffset);

/* 0x00537570 */
void __thandor_void_preserve_eax_ecx_edx AiCandidateWorkspace_SortDescending(void);

/* 0x005375D0 */
int __thandor_eax_preserve_ecx_edx
AiCandidateWorkspace_GetEntryEntityValue(AiCandidateWorkspaceEntry *entry);

/* 0x00538C90 */
bool __thandor_cf_preserve_eax_ecx_edx
AiSecondaryWorkspace_HasUnassignedEntryByIdCf(PckArmyAssetIdCatalog entryId);

/* 0x00538CF0 */
bool __thandor_cf_preserve_eax_ecx_edx
AiSecondaryWorkspace_HasEntryByIdCf(PckArmyAssetIdCatalog entryId);

/* 0x00538D40 */
int __thandor_eax_preserve_ecx_edx
AiPrimaryWorkspace_CountAssignedEntriesByIdDuplicate(PckArmyAssetIdCatalog entryId);

/* 0x00538D90 */
int __thandor_eax_preserve_ecx_edx
AiSecondaryWorkspace_GetMinimumManhattanDistanceToPoint(Q12 worldX,Q12 worldY);

/* 0x00538E00 */
int __thandor_eax_preserve_ecx_edx
AiPrimaryWorkspace_GetMinimumActiveManhattanDistanceToPoint(Q12 worldX,Q12 worldY);

/* 0x00538E80 */
int __thandor_eax_preserve_ecx_edx
AiWorkspace02_GetMinimumManhattanDistanceToPoint(Q12 worldX,Q12 worldY);

/* 0x00538EF0 */
int __thandor_eax_preserve_ecx_edx
AiWorkspace03_GetMinimumManhattanDistanceToPoint(Q12 worldX,Q12 worldY);

/* 0x00538F60 */
int __thandor_eax_preserve_ecx_edx
AiPrimaryWorkspace_GetMinimumManhattanDistanceToPoint(Q12 worldX,Q12 worldY);

/* 0x00539240 */
void __thandor_void_preserve_eax_ecx_edx
AiConstructionPlanner_PlaceSpecialAssetFromWorkspace
          (PckArmyAssetIdCatalog armyAssetId,FactionRuntimeIndex factionIndex,
          WorldRuntimeContext *worldRuntime);

/* 0x0053BCB0 */
AiTechnologyCandidateScore __thandor_eax_preserve_ecx_edx
AiWorkspace12Score_DefaultZero
          (FactionRuntimeIndex factionIndex,PckTechnologyIdCatalog technologyId,
          WorldRuntimeContext *worldRuntime);

/* 0x0053C6C0 */
StatusValueEaxCf5 __thandor_eax_cf_preserve_ecx_edx AiRuntime_InitWorkspace(void);

/* 0x00537F80 */
void __thandor_void_preserve_ecx_edx
AiEntityCandidateWorkspace09_AddOutsidePrimaryExtents(FieldGridCell *currentCell);

/* 0x00537FC0 */
void __thandor_void_preserve_ecx_edx
AiEntityCandidateWorkspace10_AddOutsidePrimaryExtents(FieldGridCell *currentCell);

/* 0x00538B90 */
bool __thandor_cf_preserve_eax_ecx_edx
AiPrimaryWorkspace_HasUnassignedEntryByIdCf(PckArmyAssetIdCatalog entryId);

/* 0x00538BF0 */
bool __thandor_cf_preserve_eax_ecx_edx
AiPrimaryWorkspace_HasEntryByIdCf(PckArmyAssetIdCatalog entryId);

/* 0x00538C40 */
int __thandor_eax_preserve_ecx_edx
AiPrimaryWorkspace_CountAssignedEntriesById(PckArmyAssetIdCatalog entryId);

/* 0x005374B0 */
void __thandor_void_preserve_eax_ecx_edx
AiCandidateWorkspace_AddOrAccumulateWeightedEntry
          (RuntimeToken entityId,dword weightRange,AiCandidateEntryKind entryKind);

/* 0x00538FD0 */
bool __thandor_void_preserve_ecx_edx
AiPrimaryWorkspace_IsPointOutsideAllEntryExtentsCf(Q12 worldX,Q12 worldY);

#endif /* THANDOR_GAMEPLAY_AI_WORKSPACES_H */

#ifndef THANDOR_GAMEPLAY_AI_WORKSPACES_H
#define THANDOR_GAMEPLAY_AI_WORKSPACES_H

#include <thandor/generated/v523_types.h>
#include <thandor/core/contracts.h>

/* Submodule: gameplay/ai/workspaces. */
/* Functions are grouped by semantic ownership; address comments are executable virtual addresses. */

/* 0x0053A1E0 */
AiPreservedFactionIndexEdxResult AiWorkspaceAssetCandidate_AddWeightedEntry (AiCandidateScore32 baseWeight,PckArmyAssetIdCatalog armyAssetId, FactionRuntimeIndex factionIndex,WorldRuntimeContext *worldRuntime);

/* 0x00538230 */
AiPlanningDispatchRegisterContinuityResult AiPlanning_RebuildFactionWorkspaces (FactionRuntimeIndex factionIndex,WorldRuntimeContext *worldRuntime);

/* 0x0053BF30 */
AiPreservedFactionIndexEdxResult AiStrategicCandidate_AddBestWorkspace12Entry (FactionRuntimeIndex factionIndex,WorldRuntimeContext *worldRuntime);

/* 0x00537420 */
AiPreservedFactionIndexEdxResult __cdecl AiCandidateWorkspace_Clear(void);

/* 0x00537430 */
void AiCandidateWorkspace_SaveToFactionImage(FactionImageByteOffset factionImageByteOffset);

/* 0x00537470 */
AiPreservedFactionIndexEdxResult AiCandidateWorkspace_LoadFromFactionImage(FactionImageByteOffset factionImageByteOffset);

/* 0x00537570 */
void __cdecl AiCandidateWorkspace_SortDescending(void);

/* 0x005375D0 */
AiWorkspaceEntryValueEaxPreservedEdxCarrier64 AiCandidateWorkspace_GetEntryEntityValue(AiCandidateWorkspaceEntry *entry);

/* 0x00538C90 */
undefined4 AiSecondaryWorkspace_HasUnassignedEntryByIdCf(PckArmyAssetIdCatalog entryId);

/* 0x00538CF0 */
undefined4 AiSecondaryWorkspace_HasEntryByIdCf(PckArmyAssetIdCatalog entryId);

/* 0x00538D40 */
AiWorkspaceCountEaxPreservedEdxCarrier64 AiPrimaryWorkspace_CountAssignedEntriesByIdDuplicate(PckArmyAssetIdCatalog entryId);

/* 0x00538D90 */
int AiSecondaryWorkspace_GetMinimumManhattanDistanceToPoint(Q12 worldX,Q12 worldY);

/* 0x00538E00 */
int AiPrimaryWorkspace_GetMinimumActiveManhattanDistanceToPoint(Q12 worldX,Q12 worldY);

/* 0x00538E80 */
int AiWorkspace02_GetMinimumManhattanDistanceToPoint(Q12 worldX,Q12 worldY);

/* 0x00538EF0 */
int AiWorkspace03_GetMinimumManhattanDistanceToPoint(Q12 worldX,Q12 worldY);

/* 0x00538F60 */
int AiPrimaryWorkspace_GetMinimumManhattanDistanceToPoint(Q12 worldX,Q12 worldY);

/* 0x00539240 */
void AiConstructionPlanner_PlaceSpecialAssetFromWorkspace (PckArmyAssetIdCatalog armyAssetId,FactionRuntimeIndex factionIndex, WorldRuntimeContext *worldRuntime);

/* 0x0053BCB0 */
AiTechnologyCandidateScore AiWorkspace12Score_DefaultZero (FactionRuntimeIndex factionIndex,PckTechnologyIdCatalog technologyId, WorldRuntimeContext *worldRuntime);

/* 0x0053C6C0 */
void AiRuntime_InitWorkspace(void);

/* 0x00537F80 */
AiPlanningGridScratchEdxContinuityResult AiEntityCandidateWorkspace09_AddOutsidePrimaryExtents(void);

/* 0x00537FC0 */
AiPlanningGridScratchEdxContinuityResult AiEntityCandidateWorkspace10_AddOutsidePrimaryExtents(void);

/* 0x00538B90 */
undefined4 AiPrimaryWorkspace_HasUnassignedEntryByIdCf(PckArmyAssetIdCatalog entryId);

/* 0x00538BF0 */
undefined4 AiPrimaryWorkspace_HasEntryByIdCf(PckArmyAssetIdCatalog entryId);

/* 0x00538C40 */
AiWorkspaceCountEaxPreservedEdxCarrier64 AiPrimaryWorkspace_CountAssignedEntriesById(PckArmyAssetIdCatalog entryId);

/* 0x005374B0 */
void AiCandidateWorkspace_AddOrAccumulateWeightedEntry (RuntimeToken entityId,dword weightRange,AiCandidateEntryKind entryKind);

/* 0x00538FD0 */
void AiPrimaryWorkspace_IsPointOutsideAllEntryExtentsCf(Q12 worldX,Q12 worldY);

#endif /* THANDOR_GAMEPLAY_AI_WORKSPACES_H */

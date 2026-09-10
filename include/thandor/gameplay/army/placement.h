#ifndef THANDOR_GAMEPLAY_ARMY_PLACEMENT_H
#define THANDOR_GAMEPLAY_ARMY_PLACEMENT_H

#include <thandor/generated/v523_types.h>
#include <thandor/core/contracts.h>

/* Submodule: gameplay/army/placement. */
/* Functions are grouped by semantic ownership; address comments are executable virtual addresses. */

/* 0x005244B0 */
int ArmyPlacementCandidate_TestOffsetClearanceCf (ArmyPlacementDispatchArg0 dispatchArg0, ArmyPlacementClearancePaddingQ12 placementClearancePaddingQ12, ArmyPlacementDispatchArg2 dispatchArg2,ArmyPlacementDispatchArg3 dispatchArg3, Q12 worldXQ12,Q12 worldYQ12,ModelDefinitionRuntimeSemanticView280 *modelDefinition, ArmyPlacementDispatchArg7 dispatchArg7,WorldRuntimeContext *worldRuntime);

/* 0x00524570 */
void ArmyPlacement_TestModelTerrainAndRuntimeClearance (WorldRuntimeContext *worldRuntime,ArmyRuntimeSlot *armyRuntime);

/* 0x0051D380 */
void ArmyPlacement_ValidateAssetAtPointAndCellCornersCf (ArmyPlacementMode placementMode,PckArmyAssetIdCatalog armyAssetId,Q12 worldYQ12, Q12 worldXQ12,ArmyPlacementContext placementContext, FactionRuntimeIndex ownerFactionId,void *inGameRuntime);

/* 0x00524EB0 */
int ArmyPlacementCandidate_TestFieldOccupancyCf (ArmyPlacementDispatchArg0 dispatchArg0, ArmyPlacementClearancePaddingQ12 placementClearancePaddingQ12, ArmyPlacementDispatchArg2 dispatchArg2,ArmyPlacementDispatchArg3 dispatchArg3, Q12 worldYQ12,Q12 worldXQ12,ModelDefinitionRecordPrefix *modelDefinition, ArmyPlacementDispatchArg7 dispatchArg7,WorldRuntimeContext *worldRuntime);

/* 0x00524F70 */
void ArmyPlacement_TestGridOccupancyMask (WorldRuntimeContext *worldRuntime,ArmyRuntimeSlot *armyRuntime);

/* 0x00528110 */
void ArmyPlacement_TestGridRuntimeAndFieldBlocking (WorldRuntimeContext *worldRuntime,ArmyRuntimeSlot *armyRuntime);

/* 0x005281A0 */
void ArmyRuntimeCollision_TestShotSpawnPointCf (dword dispatchArg0,dword dispatchArg1,dword dispatchArg2,dword dispatchArg3, Q12 worldXQ12,Q12 worldYQ12,ModelDefinitionRuntimeSemanticView280 *modelDefinition, ArmyPlacementDispatchArg7 dispatchArg7,WorldRuntimeContext *worldRuntime);

/* 0x004BE7F0 */
void ArmyPlacementContact_ApplyTerrainHeight (Q12 heightOffsetQ12,Q12 worldXQ12,Q12 worldYQ12,ModelRuntimeNode *modelNode, WorldRuntimeContext *worldRuntime);

/* 0x004BE860 */
void ArmyPlacementContact_ApplyWaterSurfaceHeight (Q12 heightOffsetQ12,Q12 worldXQ12,Q12 worldYQ12,ModelRuntimeNode *modelNode, WorldRuntimeContext *worldRuntime);

/* 0x004BE8C0 */
void ArmyPlacementContact_ApplyTerrainHeightAndNormal (Q12 heightOffsetQ12,Q12 worldXQ12,Q12 worldYQ12,ModelRuntimeNode *modelNode, WorldRuntimeContext *worldRuntime);

/* 0x004BE930 */
void ArmyPlacementContact_ApplyTopSurfaceHeight (Q12 heightOffsetQ12,Q12 worldXQ12,Q12 worldYQ12,ModelRuntimeNode *modelNode, WorldRuntimeContext *worldRuntime);

/* 0x004BE990 */
void ArmyPlacementContact_InitializeArticulatedSuspension (Q12 heightOffsetQ12,Q12 worldXQ12,Q12 worldYQ12,ModelRuntimeNode *modelNode, WorldRuntimeContext *worldRuntime);

/* 0x00525320 */
void ArmyPlacement_ReleaseFactionCapacityAndClearGridReservation (ModelDefinitionRecordPrefix *modelDefinition,ModelRuntimeSlot *modelRuntime);

/* 0x00525420 */
void ArmyPlacement_ReleaseFactionCapacity (ModelDefinitionRecordPrefix *modelDefinition,ModelRuntimeSlot *modelRuntime);

/* 0x005263E0 */
void ArmyPlacement_ReleaseClassStateReservation (ModelDefinitionRecordPrefix *modelDefinition,ModelRuntimeSlot *modelRuntime);

/* 0x00527BD0 */
void ArmyPlacementAssetClassDispatch_AlwaysSuccessCf (dword dispatchArg0,dword dispatchArg1,dword dispatchArg2,dword dispatchArg3, Q12 worldXQ12,Q12 worldYQ12,ModelDefinitionRecordPrefix *modelDefinition, dword dispatchArg7,WorldRuntimeContext *worldRuntime);

/* 0x00529CB0 */
void __fastcall ArmyCollision_TestPointAgainstRuntimeListCf (undefined4 incomingEcxValue,undefined4 preservedEdxValue,Q12 worldXQ12,Q12 worldYQ12, byte *modelDefinition,WorldRuntimeContext *worldRuntime);

/* 0x00529E60 */
ArmyRuntimeSlot * ArmyCollision_FindBlockingRuntimeForCurrentUnitCf (Q12 worldXQ12,Q12 worldYQ12,ArmyRuntimeSlot *currentArmyRuntime, WorldRuntimeContext *worldRuntime);

/* 0x0051D450 */
void ArmyPlacement_DispatchAssetAtFieldPoint (ArmyPlacementMode placementMode, ArmyPlacementClearancePaddingQ12 placementClearancePaddingQ12, FactionRuntimeIndex ownerFactionIndex,Q12 worldYQ12,Q12 worldXQ12, PckArmyAssetIdCatalog armyAssetId,ArmyPlacementContext placementContext, UiRootNode *inGameRoot);

/* 0x00529D70 */
undefined8 ArmyPlacementCollision_TestPointAgainstRuntimeListCf (ArmyPlacementCollisionFilterFlags placementFilterFlags,Q12 queryRadiusQ12,Q12 worldXQ12, Q12 worldYQ12,WorldRuntimeContext *worldRuntime);

/* 0x00529F30 */
void ArmyPlacementCollision_TestCandidateAgainstRuntimeListCf (WorldRuntimeNode *excludedWorldObject,Q12 worldXQ12,Q12 worldYQ12, IMAGE_DOS_HEADER *candidateRuntimeOrRadiusQ12,WorldRuntimeContext *worldRuntime);

/* 0x00527740 */
void ArmyPlacementCollision_TestCurrentRuntimeCf (WorldRuntimeContext *worldRuntime,ArmyRuntimeSlot *armyRuntime);

/* 0x005278D0 */
int ArmyPlacementCollision_TestCandidateAndClearanceCf (ArmyPlacementDispatchArg0 dispatchArg0, ArmyPlacementClearancePaddingQ12 placementClearancePaddingQ12,dword dispatchArg2, ArmyPlacementDispatchArg3 dispatchArg3,Q12 worldXQ12,Q12 worldYQ12, ModelDefinitionRuntimeSemanticView280 *modelDefinition, ArmyPlacementDispatchArg7 dispatchArg7,WorldRuntimeContext *worldRuntime);

/* 0x00524650 */
undefined8 ArmyPlacementCandidate_TestModelAnchorDistanceCf (undefined4 param_1,Q12 targetWorldXQ12,Q12 targetWorldYQ12,ArmyRuntimeSlot *armyRuntime);

/* 0x00529C40 */
undefined8 ArmyCollision_TestPointWithinExpandedRuntimeRadiusCf (Q12 queryRadiusQ12,Q12 worldXQ12,Q12 worldYQ12,ArmyRuntimeSlot *armyRuntime);

#endif /* THANDOR_GAMEPLAY_ARMY_PLACEMENT_H */

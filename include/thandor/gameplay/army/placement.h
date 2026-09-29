/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/include/thandor/gameplay/army/placement.h
 * Reverse engineering by idkFoxes 2026
 */

#ifndef THANDOR_GAMEPLAY_ARMY_PLACEMENT_H
#define THANDOR_GAMEPLAY_ARMY_PLACEMENT_H

#include <thandor/generated/types.h>
#include <thandor/core/contracts.h>

/* Submodule: gameplay/army/placement. */

/* placementMode bits of the placement asset-class dispatch (ArmyPlacement_DispatchAssetAtFieldPoint passes
   the mode as the first argument of every g_ArmyRuntimeOrderHandlerMatrix11Columns24Classes.
   placementAssetClassDispatch handler; the player uses 0/1, the AI 0, 3, 4 and 7). */
#define ARMY_PLACEMENT_MODE_SKIP_CLASS18_SUPPORT 0x1   /* runtime-class-18 neighbours do not count as the
                                                          same-faction support that a candidate must be near */
#define ARMY_PLACEMENT_MODE_MEASURE_SUPPORT_DISTANCE 0x2 /* do not accept at the first supporting neighbour:
                                                            return the free distance to the nearest one */
#define ARMY_PLACEMENT_MODE_DEPTH_CLASS_90_ONLY 0x4    /* the runtime-list collision only considers armies whose
                                                          g_ArmyRuntimeDepthBinClassByModelClass entry is 0x90 */
/* Clearance (3.0 in Q12) kept around a model's (1,5) anchor point, the point placement offsets by the model's
   heading (ArmyPlacementCandidate_TestOffsetClearance, ArmyPlacement_TestModelTerrainAndRuntimeClearance,
   ArmyPlacementCandidate_TestModelAnchorDistance). */
#define ARMY_PLACEMENT_ANCHOR_CLEARANCE_Q12 0xc00
/* Placement contact kinds (model definition +0x278): index into g_ArmyPlacementContactKindDispatchTable and
   g_TerrainClassPlacementAndOverlayCallbacks10.placementTests. 0 terrain height, 1 water surface, 2 terrain
   height and normal, 3 articulated suspension, 4 top surface. */
#define ARMY_PLACEMENT_CONTACT_KIND_WATER_SURFACE 1
#define ARMY_PLACEMENT_CONTACT_KIND_ARTICULATED_SUSPENSION 3
/* Byte size of one GameFactionRuntimeRecord (8 of them in g_GameFactionRuntimeImage). */
#define GAME_FACTION_RUNTIME_RECORD_BYTES 0x740
/* Functions are grouped by semantic ownership; address comments are executable virtual addresses. */

/* 0x005244B0 */
PlacementCandidateResult ArmyPlacementCandidate_TestOffsetClearance (ArmyPlacementDispatchArg0 placementMode, ArmyPlacementClearancePaddingQ12 placementClearancePaddingQ12, ArmyPlacementDispatchArg2 placementHeading,ArmyPlacementDispatchArg3 terrainHeightQ12, Q12 worldXQ12,Q12 worldYQ12,ModelDefinition *modelDefinition, ArmyPlacementDispatchArg7 ownerFactionIndex,WorldRuntimeContext *worldRuntime);

/* 0x00524570 */
bool ArmyPlacement_TestModelTerrainAndRuntimeClearance
          (WorldRuntimeContext *worldRuntime,ModelRuntimePlacementValidationView *modelRuntime);

/* ECX/EDX results of ArmyPlacement_ValidateAssetAtPointAndCellCorners: the accepted point. */
extern Q12 g_ArmyPlacementValidatedWorldXQ12;
extern Q12 g_ArmyPlacementValidatedWorldYQ12;

/* 0x0051D380 */
bool ArmyPlacement_ValidateAssetAtPointAndCellCorners
          (ArmyPlacementMode placementMode,uint32_t placementHeading,Q12 worldYQ12,
          Q12 worldXQ12,PckArmyAssetIdCatalog armyAssetId,FactionRuntimeIndex ownerFactionId,
          void *inGameRuntime);

/* 0x00524EB0 */
PlacementCandidateResult ArmyPlacementCandidate_TestFieldOccupancy (ArmyPlacementDispatchArg0 placementMode, ArmyPlacementClearancePaddingQ12 placementClearancePaddingQ12, ArmyPlacementDispatchArg2 placementHeading,ArmyPlacementDispatchArg3 terrainHeightQ12, Q12 worldYQ12,Q12 worldXQ12,ModelDefinitionRecordPrefix *modelDefinition, ArmyPlacementDispatchArg7 ownerFactionIndex,WorldRuntimeContext *worldRuntime);

/* 0x00524F70 */
bool ArmyPlacement_TestGridOccupancyMask
          (WorldRuntimeContext *worldRuntime,ModelRuntimePlacementClass14View *modelRuntime);

/* 0x00528110 */
bool ArmyPlacement_TestGridRuntimeAndFieldBlocking
          (WorldRuntimeContext *worldRuntime,ModelRuntimePlacementValidationView *modelRuntime);

/* 0x005281A0 */
PlacementCandidateResult ArmyPlacement_TestMobileUnitPoint
               (uint32_t placementMode,uint32_t placementClearancePaddingQ12,uint32_t placementHeading,uint32_t terrainHeightQ12,
               Q12 worldXQ12,Q12 worldYQ12,ModelDefinition *modelDefinition,
               ArmyPlacementDispatchArg7 ownerFactionIndex,WorldRuntimeContext *worldRuntime);

/* 0x004BE7F0 */
void ArmyPlacementContact_ApplyTerrainHeight
          (Q12 heightOffsetQ12,Q12 worldYQ12,Q12 worldXQ12,ModelRuntimeNode *modelNode,
          WorldRuntimeContext *worldRuntime);

/* 0x004BE860 */
void ArmyPlacementContact_ApplyWaterSurfaceHeight
          (Q12 heightOffsetQ12,Q12 worldYQ12,Q12 worldXQ12,ModelRuntimeNode *modelNode,
          WorldRuntimeContext *worldRuntime);

/* 0x004BE8C0 */
void ArmyPlacementContact_ApplyTerrainHeightAndNormal
          (Q12 heightOffsetQ12,Q12 worldYQ12,Q12 worldXQ12,ModelRuntimeNode *modelNode,
          WorldRuntimeContext *worldRuntime);

/* 0x004BE930 */
void ArmyPlacementContact_ApplyTopSurfaceHeight
          (Q12 heightOffsetQ12,Q12 worldYQ12,Q12 worldXQ12,ModelRuntimeNode *modelNode,
          WorldRuntimeContext *worldRuntime);

/* 0x004BE990 */
void ArmyPlacementContact_InitializeArticulatedSuspension
          (Q12 heightOffsetQ12,Q12 worldYQ12,Q12 worldXQ12,ModelRuntimeNode *modelNode,
          WorldRuntimeContext *worldRuntime);

/* 0x00525320 */
void ArmyPlacement_ReleaseFactionCapacityAndClearGridReservation
          (ModelDefinitionRecordPrefix *modelDefinition,ModelRuntimeSlot *modelRuntime);

/* 0x00525420 */
void ArmyPlacement_ReleaseFactionCapacity(ModelDefinitionRecordPrefix *modelDefinition,ModelRuntimeSlot *modelRuntime);

/* 0x005263E0 */
void ArmyPlacement_ReleaseClassStateReservation
          (ModelDefinitionRecordPrefix *modelDefinition,ModelRuntimeSlot *modelRuntime);

/* 0x00527BD0 */
PlacementCandidateResult ArmyPlacementAssetClassDispatch_AlwaysSuccess (uint32_t placementMode,uint32_t placementClearancePaddingQ12,uint32_t placementHeading,uint32_t terrainHeightQ12, Q12 worldXQ12,Q12 worldYQ12,ModelDefinitionRecordPrefix *modelDefinition, uint32_t ownerFactionIndex,WorldRuntimeContext *worldRuntime);

/* 0x00529CB0 */
bool ArmyCollision_TestPointAgainstRuntimeList
          (Q12 worldXQ12,Q12 worldYQ12,uint8_t *modelDefinition,WorldRuntimeContext *worldRuntime);

/* 0x00529E60 */
ArmyCollisionResult ArmyCollision_FindBlockingRuntimeForCurrentUnit
          (Q12 worldXQ12,Q12 worldYQ12,RuntimeCollisionQueryView *currentRuntime,
          WorldRuntimeContext *worldRuntime);

/* 0x0051D450 */
PlacementDispatchResult ArmyPlacement_DispatchAssetAtFieldPoint(ArmyPlacementMode placementMode,
          ArmyPlacementClearancePaddingQ12 placementClearancePaddingQ12,
          uint32_t placementHeading,Q12 worldYQ12,Q12 worldXQ12,
          PckArmyAssetIdCatalog armyAssetId,FactionRuntimeIndex ownerFactionIndex,
          UiRootNode *inGameRoot);

/* 0x00529D70 */
bool ArmyPlacementCollision_TestPointAgainstRuntimeList
          (ArmyPlacementCollisionFilterFlags placementFilterFlags,Q12 queryRadiusQ12,Q12 worldXQ12,
          Q12 worldYQ12,WorldRuntimeContext *worldRuntime);

/* 0x00529F30 */
bool ArmyPlacementCollision_TestCandidateAgainstRuntimeList
          (WorldOwnerListNode *excludedWorldObject,Q12 worldXQ12,Q12 worldYQ12,
          IMAGE_DOS_HEADER *candidateRuntimeOrRadiusQ12,WorldRuntimeContext *worldRuntime);

/* 0x00527740 */
bool ArmyPlacementCollision_TestCurrentRuntime
          (WorldRuntimeContext *worldRuntime,ModelRuntimePlacementValidationView *modelRuntime);

/* 0x005278D0 */
PlacementCandidateResult
ArmyPlacementCollision_TestCandidateAndClearance
          (ArmyPlacementDispatchArg0 placementMode,
          ArmyPlacementClearancePaddingQ12 placementClearancePaddingQ12,uint32_t placementHeading,
          ArmyPlacementDispatchArg3 terrainHeightQ12,Q12 worldXQ12,Q12 worldYQ12,
          ModelDefinition *modelDefinition,
          ArmyPlacementDispatchArg7 ownerFactionIndex,WorldRuntimeContext *worldRuntime);

/* 0x00524650 */
bool ArmyPlacementCandidate_TestModelAnchorDistance
          (Q12 queryRadiusQ12,Q12 targetWorldXQ12,Q12 targetWorldYQ12,ArmyRuntimeSlot *armyRuntime);

/* 0x00529C40 */
bool ArmyCollision_TestPointWithinExpandedRuntimeRadius
          (Q12 queryRadiusQ12,Q12 worldXQ12,Q12 worldYQ12,ArmyRuntimeSlot *armyRuntime);

#endif /* THANDOR_GAMEPLAY_ARMY_PLACEMENT_H */

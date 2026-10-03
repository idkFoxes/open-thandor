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

/* placementMode bits of the placement asset-class dispatch (ArmyPlacement_CanPlaceAssetAtFieldPoint passes
   the mode as the first argument of every g_ArmyRuntimeOrderHandlerMatrix11Columns24Classes.
   placementAssetClassDispatch handler; the player uses 0/1, the AI 0, 3, 4 and 7). */
#define ARMY_PLACEMENT_MODE_SKIP_CLASS18_SUPPORT 0x1   /* runtime-class-18 neighbours do not count as the
                                                          same-faction support that a candidate must be near */
#define ARMY_PLACEMENT_MODE_MEASURE_SUPPORT_DISTANCE 0x2 /* do not accept at the first supporting neighbour:
                                                            return the free distance to the nearest one */
#define ARMY_PLACEMENT_MODE_STRUCTURES_ONLY 0x4    /* the runtime-list collision only considers armies whose
                                                          g_ArmyRuntimeDepthBinClassByModelClass entry is 0x90 */
/* Clearance (3.0 in Q12) kept around a model's (1,5) anchor point, the point placement offsets by the model's
   heading (ArmyPlacement_CanPlaceAnchoredModel,ArmyPlacement_TestModelTerrainAndRuntimeClearance,
   ArmyPlacementCandidate_TestModelAnchorDistance). */
#define ARMY_PLACEMENT_ANCHOR_CLEARANCE_Q12 0xc00
/* ArmyPlacement_ValidateAssetAtPointAndCellCorners: offset (Q12, each axis) of the retry corners from the point
   rounded down to a multiple of 0x100 */
#define ARMY_PLACEMENT_CORNER_OFFSET_Q12 0x240
/* nearest-support search: a squared distance above this still has the INT64_MAX start value's high dword
   (no supporting model found) */
#define ARMY_PLACEMENT_NO_SUPPORT_DISTANCE_SQUARED (INT64_MAX - ((int64_t)1 << 32))
/* Placement contact kinds (ModelDefinition.placementContactKindIndex): index into
   g_ArmyPlacementContactKindDispatchTable and g_TerrainClassPlacementAndOverlayCallbacks10.placementTests.
   0 terrain height, 1 water surface, 2 terrain height and normal, 3 articulated suspension, 4 top surface. */
#define ARMY_PLACEMENT_CONTACT_KIND_WATER_SURFACE 1
#define ARMY_PLACEMENT_CONTACT_KIND_ARTICULATED_SUSPENSION 3
/* Byte size of one GameFactionRuntimeRecord (8 of them in g_GameFactionRuntimeImage). */
#define GAME_FACTION_RUNTIME_RECORD_BYTES 0x740
/* Functions are grouped by semantic ownership. */

bool ArmyPlacement_CanPlaceAnchoredModel (ArmyPlacementDispatchArg0 placementMode, ArmyPlacementClearancePaddingQ12 placementClearancePaddingQ12, ArmyPlacementDispatchArg2 placementHeading,ArmyPlacementDispatchArg3 terrainHeightQ12, Q12 worldXQ12,Q12 worldYQ12,ModelDefinition *modelDefinition, ArmyPlacementDispatchArg7 ownerFactionIndex,WorldRuntimeContext *worldRuntime, uint32_t *outPlacementValue);

bool ArmyPlacement_TestModelTerrainAndRuntimeClearance
          (WorldRuntimeContext *worldRuntime,ModelRuntimePlacementValidationView *modelRuntime);

/* Extra results of ArmyPlacement_ValidateAssetAtPointAndCellCorners: the accepted point. */
extern Q12 g_ArmyPlacementValidatedWorldXQ12;
extern Q12 g_ArmyPlacementValidatedWorldYQ12;

bool ArmyPlacement_ValidateAssetAtPointAndCellCorners
          (ArmyPlacementMode placementMode,uint32_t placementHeading,Q12 worldYQ12,
          Q12 worldXQ12,PckArmyAssetIdCatalog armyAssetId,FactionRuntimeIndex ownerFactionId,
          void *inGameRuntime);

bool ArmyPlacement_CanPlaceResourceExtractor (ArmyPlacementDispatchArg0 placementMode, ArmyPlacementClearancePaddingQ12 placementClearancePaddingQ12, ArmyPlacementDispatchArg2 placementHeading,ArmyPlacementDispatchArg3 terrainHeightQ12, Q12 worldYQ12,Q12 worldXQ12,ModelDefinitionRecordPrefix *modelDefinition, ArmyPlacementDispatchArg7 ownerFactionIndex,WorldRuntimeContext *worldRuntime, uint32_t *outPlacementValue);

bool ArmyPlacement_TestGridOccupancyMask
          (WorldRuntimeContext *worldRuntime,ModelRuntimePlacementClass14View *modelRuntime);

bool ArmyPlacement_TestGridRuntimeAndFieldBlocking
          (WorldRuntimeContext *worldRuntime,ModelRuntimePlacementValidationView *modelRuntime);

bool ArmyPlacement_CanPlaceMobileUnit
               (uint32_t placementMode,uint32_t placementClearancePaddingQ12,uint32_t placementHeading,uint32_t terrainHeightQ12,
               Q12 worldXQ12,Q12 worldYQ12,ModelDefinition *modelDefinition,
               ArmyPlacementDispatchArg7 ownerFactionIndex,WorldRuntimeContext *worldRuntime,
               uint32_t *outPlacementValue);

void ArmyPlacementContact_ApplyTerrainHeight
          (Q12 heightOffsetQ12,Q12 worldYQ12,Q12 worldXQ12,ModelRuntimeNode *modelNode,
          WorldRuntimeContext *worldRuntime);

void ArmyPlacementContact_ApplyWaterSurfaceHeight
          (Q12 heightOffsetQ12,Q12 worldYQ12,Q12 worldXQ12,ModelRuntimeNode *modelNode,
          WorldRuntimeContext *worldRuntime);

void ArmyPlacementContact_ApplyTerrainHeightAndNormal
          (Q12 heightOffsetQ12,Q12 worldYQ12,Q12 worldXQ12,ModelRuntimeNode *modelNode,
          WorldRuntimeContext *worldRuntime);

void ArmyPlacementContact_ApplyTopSurfaceHeight
          (Q12 heightOffsetQ12,Q12 worldYQ12,Q12 worldXQ12,ModelRuntimeNode *modelNode,
          WorldRuntimeContext *worldRuntime);

void ArmyPlacementContact_InitializeArticulatedSuspension
          (Q12 heightOffsetQ12,Q12 worldYQ12,Q12 worldXQ12,ModelRuntimeNode *modelNode,
          WorldRuntimeContext *worldRuntime);

void ArmyPlacement_ReleaseFactionCapacityAndClearGridReservation
          (ModelDefinitionRecordPrefix *modelDefinition,ModelRuntimeSlot *modelRuntime);

void ArmyPlacement_ReleaseFactionCapacity(ModelDefinitionRecordPrefix *modelDefinition,ModelRuntimeSlot *modelRuntime);

void ArmyPlacement_ReleaseClassStateReservation
          (ModelDefinitionRecordPrefix *modelDefinition,ModelRuntimeSlot *modelRuntime);

bool ArmyPlacement_CanPlaceAnywhere (uint32_t placementMode,uint32_t placementClearancePaddingQ12,uint32_t placementHeading,uint32_t terrainHeightQ12, Q12 worldXQ12,Q12 worldYQ12,ModelDefinitionRecordPrefix *modelDefinition, uint32_t ownerFactionIndex,WorldRuntimeContext *worldRuntime, uint32_t *outPlacementValue);

bool ArmyCollision_TestPointAgainstRuntimeList
          (Q12 worldXQ12,Q12 worldYQ12,uint8_t *modelDefinition,WorldRuntimeContext *worldRuntime);

ModelRuntimeSlot *ArmyCollision_FindBlockingRuntimeForCurrentUnit
          (Q12 worldXQ12,Q12 worldYQ12,RuntimeCollisionQueryView *currentRuntime,
          WorldRuntimeContext *worldRuntime);

bool ArmyPlacement_CanPlaceAssetAtFieldPoint(ArmyPlacementMode placementMode,
          ArmyPlacementClearancePaddingQ12 placementClearancePaddingQ12,
          uint32_t placementHeading,Q12 worldYQ12,Q12 worldXQ12,
          PckArmyAssetIdCatalog armyAssetId,FactionRuntimeIndex ownerFactionIndex,
          UiRootNode *inGameRoot,uint32_t *outPlacementValue);

bool ArmyPlacementCollision_TestPointAgainstRuntimeList
          (ArmyPlacementCollisionFilterFlags placementFilterFlags,Q12 queryRadiusQ12,Q12 worldXQ12,
          Q12 worldYQ12,WorldRuntimeContext *worldRuntime);

bool ArmyPlacementCollision_TestCandidateAgainstRuntimeList
          (WorldOwnerListNode *excludedWorldObject,Q12 worldXQ12,Q12 worldYQ12,
          IMAGE_DOS_HEADER *candidateRuntimeOrRadiusQ12,WorldRuntimeContext *worldRuntime);

bool ArmyPlacementCollision_TestCurrentRuntime
          (WorldRuntimeContext *worldRuntime,ModelRuntimePlacementValidationView *modelRuntime);

bool ArmyPlacement_CanPlaceBuilding
          (ArmyPlacementDispatchArg0 placementMode,
          ArmyPlacementClearancePaddingQ12 placementClearancePaddingQ12,uint32_t placementHeading,
          ArmyPlacementDispatchArg3 terrainHeightQ12,Q12 worldXQ12,Q12 worldYQ12,
          ModelDefinition *modelDefinition,
          ArmyPlacementDispatchArg7 ownerFactionIndex,WorldRuntimeContext *worldRuntime,
          uint32_t *outPlacementValue);

bool ArmyPlacementCandidate_TestModelAnchorDistance
          (Q12 queryRadiusQ12,Q12 targetWorldXQ12,Q12 targetWorldYQ12,ModelRuntimeSlot *modelRuntime);

bool ArmyCollision_TestPointWithinExpandedRuntimeRadius
          (Q12 queryRadiusQ12,Q12 worldXQ12,Q12 worldYQ12,ModelRuntimeSlot *modelRuntime);

#endif /* THANDOR_GAMEPLAY_ARMY_PLACEMENT_H */

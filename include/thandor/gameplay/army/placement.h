/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/include/thandor/gameplay/army/placement.h
 * Reverse engineering by idkFoxes 2026
 */

#ifndef THANDOR_GAMEPLAY_ARMY_PLACEMENT_H
#define THANDOR_GAMEPLAY_ARMY_PLACEMENT_H

#include <thandor/assets/model/types.h>
#include <thandor/core/types.h>
#include <thandor/gameplay/army/types.h>
#include <thandor/ui/controls/types.h>
#include <thandor/ui/ingame/types.h>
#include <thandor/core/contracts.h>
#include <stdint.h> /* INT64_MAX */

/* placementMode bits of the placement asset-class dispatch (ArmyPlacement_CanPlaceAssetAtFieldPoint passes
   the mode as the first argument of every g_ArmyRuntimeOrderHandlerMatrix11Columns24Classes.
   placementAssetClassDispatch handler; the player uses 0/1, the AI 0, 3, 4 and 7). */
inline constexpr int ARMY_PLACEMENT_MODE_SKIP_CLASS18_SUPPORT = 0x1; /* runtime-class-18 neighbours do not count as the
                                                          same-faction support that a candidate must be near */
inline constexpr int ARMY_PLACEMENT_MODE_MEASURE_SUPPORT_DISTANCE = 0x2; /* do not accept at the first supporting neighbour:
                                                            return the free distance to the nearest one */
inline constexpr int ARMY_PLACEMENT_MODE_STRUCTURES_ONLY = 0x4; /* the runtime-list collision only considers armies whose
                                                          g_ArmyRuntimeDepthBinClassByModelClass entry is 0x90 */
/* Clearance (3.0 in Q12) kept around a model's (1,5) anchor point, the point placement offsets by the model's
   heading (ArmyPlacement_CanPlaceAnchoredModel,ArmyPlacement_TestModelTerrainAndRuntimeClearance,
   ArmyPlacementCandidate_TestModelAnchorDistance). */
inline constexpr int ARMY_PLACEMENT_ANCHOR_CLEARANCE_Q12 = 0xc00;
/* ArmyPlacement_ValidateAssetAtPointAndCellCorners: offset (Q12, each axis) of the retry corners from the point
   rounded down to a multiple of 0x100 */
inline constexpr int ARMY_PLACEMENT_CORNER_OFFSET_Q12 = 0x240;
/* nearest-support search: a squared distance above this still has the INT64_MAX start value's high dword
   (no supporting model found) */
inline constexpr auto ARMY_PLACEMENT_NO_SUPPORT_DISTANCE_SQUARED = INT64_MAX - ((int64_t)1 << 32);
/* Placement contact kinds (ModelDefinition.placementContactKindIndex): index into
   g_ArmyPlacementContactKindDispatchTable and g_TerrainClassPlacementAndOverlayCallbacks10.placementTests.
   0 terrain height, 1 water surface, 2 terrain height and normal, 3 articulated suspension, 4 top surface. */
inline constexpr int ARMY_PLACEMENT_CONTACT_KIND_WATER_SURFACE = 1;
inline constexpr int ARMY_PLACEMENT_CONTACT_KIND_ARTICULATED_SUSPENSION = 3;

Bool8 ArmyPlacement_CanPlaceAnchoredModel (ArmyPlacementDispatchArg0 placementMode, ArmyPlacementClearancePaddingQ12 placementClearancePaddingQ12, ArmyPlacementDispatchArg2 placementHeading,ArmyPlacementDispatchArg3 terrainHeightQ12, Q12 worldXQ12,Q12 worldYQ12,ModelDefinition *modelDefinition, ArmyPlacementDispatchArg7 ownerFactionIndex,WorldRuntimeContext *worldRuntime, uint32_t *outPlacementValue);

Bool8 ArmyPlacement_TestModelTerrainAndRuntimeClearance
          (WorldRuntimeContext *worldRuntime,ModelRuntimePlacementValidationView *modelRuntime);

Bool8 ArmyPlacement_ValidateAssetAtPointAndCellCorners
          (ArmyPlacementMode placementMode,uint32_t placementHeading,Q12 worldYQ12,
          Q12 worldXQ12,PckArmyAssetIdCatalog armyAssetId,FactionRuntimeIndex ownerFactionId,
          void *inGameRuntime);

Bool8 ArmyPlacement_CanPlaceResourceExtractor (ArmyPlacementDispatchArg0 placementMode, ArmyPlacementClearancePaddingQ12 placementClearancePaddingQ12, ArmyPlacementDispatchArg2 placementHeading,ArmyPlacementDispatchArg3 terrainHeightQ12, Q12 worldYQ12,Q12 worldXQ12,ModelDefinitionRecordPrefix *modelDefinition, ArmyPlacementDispatchArg7 ownerFactionIndex,WorldRuntimeContext *worldRuntime, uint32_t *outPlacementValue);

Bool8 ArmyPlacement_TestGridOccupancyMask
          (WorldRuntimeContext *worldRuntime,ModelRuntimePlacementClass14View *modelRuntime);

Bool8 ArmyPlacement_TestGridRuntimeAndFieldBlocking
          (WorldRuntimeContext *worldRuntime,ModelRuntimePlacementValidationView *modelRuntime);

Bool8 ArmyPlacement_CanPlaceMobileUnit
               (uint32_t placementMode,uint32_t placementClearancePaddingQ12,uint32_t placementHeading,uint32_t terrainHeightQ12,
               Q12 worldXQ12,Q12 worldYQ12,ModelDefinition *modelDefinition,
               ArmyPlacementDispatchArg7 ownerFactionIndex,WorldRuntimeContext *worldRuntime,
               uint32_t *outPlacementValue);

Bool8 ArmyPlacement_CanPlaceAnywhere (uint32_t placementMode,uint32_t placementClearancePaddingQ12,uint32_t placementHeading,uint32_t terrainHeightQ12, Q12 worldXQ12,Q12 worldYQ12,ModelDefinitionRecordPrefix *modelDefinition, uint32_t ownerFactionIndex,WorldRuntimeContext *worldRuntime, uint32_t *outPlacementValue);

Bool8 ArmyPlacement_CanPlaceAssetAtFieldPoint(ArmyPlacementMode placementMode,
          ArmyPlacementClearancePaddingQ12 placementClearancePaddingQ12,
          uint32_t placementHeading,Q12 worldYQ12,Q12 worldXQ12,
          PckArmyAssetIdCatalog armyAssetId,FactionRuntimeIndex ownerFactionIndex,
          WorldRuntimeContext *worldRuntime,uint32_t *outPlacementValue);

Bool8 ArmyPlacement_CanPlaceBuilding
          (ArmyPlacementDispatchArg0 placementMode,
          ArmyPlacementClearancePaddingQ12 placementClearancePaddingQ12,uint32_t placementHeading,
          ArmyPlacementDispatchArg3 terrainHeightQ12,Q12 worldXQ12,Q12 worldYQ12,
          ModelDefinition *modelDefinition,
          ArmyPlacementDispatchArg7 ownerFactionIndex,WorldRuntimeContext *worldRuntime,
          uint32_t *outPlacementValue);

Bool8 ArmyPlacementCandidate_TestModelAnchorDistance
          (Q12 queryRadiusQ12,Q12 targetWorldXQ12,Q12 targetWorldYQ12,ModelRuntimeSlot *modelRuntime);

void PlayerRuntime_CreatePlacementArmy(PlayerRuntimeId playerRuntimeId,PlayerStateLookupValue0 worldXQ12,
          PlayerStateLookupValue1 worldYQ12,RuntimeToken armyAssetId);

void PlayerRuntime_SetPlacementFaction(PlayerRuntimeId playerRuntimeId,uint32_t unusedZero0,uint32_t unusedZero1,
          PlacementFactionIndex placementFactionIndex);

void PlayerRuntime_SetPlacementArmy(PlayerRuntimeId playerRuntimeId,uint32_t unusedZero0,uint32_t unusedZero1,
          PlacedArmyToken armyToken);

void PlayerRuntime_ClearPlacementArmy(PlayerRuntimeId playerRuntimeId,uint32_t unusedZero0,uint32_t unusedZero1,
          uint32_t unusedZero2);

/* Extra results of ArmyPlacement_ValidateAssetAtPointAndCellCorners: the accepted point. */
extern Q12 g_ArmyPlacementValidatedWorldXQ12;
extern Q12 g_ArmyPlacementValidatedWorldYQ12;

extern ArmyPlacementCandidateCount g_ArmyPlacementLateRejectionCount;

void WorldRuntime_EmitModelDefinitionOverlayForMatchingEntries(void *sourceRuntime,WorldRuntimeContext *worldRuntime);

#endif /* THANDOR_GAMEPLAY_ARMY_PLACEMENT_H */

/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/include/thandor/gameplay/selection/commands.h
 * Reverse engineering by idkFoxes 2026
 */

#ifndef THANDOR_GAMEPLAY_SELECTION_COMMANDS_H
#define THANDOR_GAMEPLAY_SELECTION_COMMANDS_H

#include <thandor/core/types.h>
#include <thandor/gameplay/army/types.h>
#include <thandor/gameplay/selection/types.h>
#include <thandor/network/protocol/types.h>
#include <thandor/core/contracts.h>

void InGameSelection_SelectAllOwnAircraftPads
          (PlayerRuntimeId playerRuntimeId,uint32_t callbackArg1,uint32_t callbackArg2,uint32_t callbackArg3);

void InGamePlayerSelection_ReplaceWithArmyRuntimeIndex
          (PlayerRuntimeId playerId,uint32_t unusedPayload1,uint32_t unusedPayload2,
          RuntimeToken armyRuntimeIndex);

void InGamePlayerSelection_ApplyMoveCommand
          (PlayerRuntimeId playerId,uint32_t unusedPayload1,CommandPayload worldXQ12,
          CommandPayload worldYQ12);

void InGamePlayerSelection_ApplyPositionCommand
          (PlayerRuntimeId playerId,uint32_t unusedPayload1,CommandPayload worldXQ12,
          CommandPayload worldYQ12);

void InGamePlayerSelection_SelectArmyRuntimeIndex
          (PlayerRuntimeId playerId,uint32_t unusedPayload1,uint32_t unusedPayload2,
          RuntimeToken armyRuntimeIndex);

void InGamePlayerSelection_ApplyTargetPositionCommand(PlayerRuntimeId playerId,CommandPayload surfaceHeightQ12,
          CommandPayload worldXQ12,CommandPayload worldYQ12);

void PlayerSelection_ResetMovementPruneAndRecenterEntries(PlayerRuntimeId playerId,CommandPayload unusedPayload1,
          CommandPayload unusedPayload2,CommandPayload unusedPayload3);

void PlayerSelection_StopMovement
          (PlayerRuntimeId playerId,CommandPayload unusedPayload1,
          CommandPayload unusedPayload2,CommandPayload unusedPayload3);

void PlayerSelection_CancelTargets
          (PlayerRuntimeId playerId,CommandPayload unusedPayload1,
          CommandPayload unusedPayload2,CommandPayload unusedPayload3);

void PlayerSelection_SelfDestruct
          (PlayerRuntimeId playerId,CommandPayload unusedPayload1,
          CommandPayload unusedPayload2,CommandPayload unusedPayload3);

void InGameSelection_SetAircraftPadTargetLane1
          (SelectionMarkerIndex playerRuntimeId,SelectionMarkerCoordinateValue32 heading16,
          SelectionMarkerCoordinateValue32 worldXQ12,SelectionMarkerCoordinateValue32 worldYQ12);

void InGameSelection_SetAircraftPadTargetLane2
          (SelectionMarkerIndex playerRuntimeId,SelectionMarkerCoordinateValue32 heading16,
          SelectionMarkerCoordinateValue32 worldXQ12,SelectionMarkerCoordinateValue32 worldYQ12);

void SelectionPointerArray_ApplyMoveCommand
          (Q12 targetWorldY,Q12 targetWorldX,SelectionPointerArray32 *selection);

void SelectionRuntime_ResetMovementPruneAndRecenterEntries(Ptr32<GameEntityRuntime> *selectionEntries);

void SelectionPointerArray_ApplyPositionCommand(Q12 targetWorldY,Q12 targetWorldX,SelectionPointerArray32 *selection);

void SelectionPointerArray_ApplyArmyRuntimeTarget
          (ArmyRuntimeSlot *targetArmyRuntime,SelectionPointerArray32 *selection);

void SelectionPointerArray_ApplyTargetPositionCommand
          (Q12 coordinateA,uint32_t coordinateB,Q12 coordinateC,SelectionPointerArray32 *selection);

void SelectionRuntime_StopMovement(Ptr32<GameEntityRuntime> *selectionEntries);

void SelectionRuntime_CancelTargets(Ptr32<GameEntityRuntime> *selectionEntries);

void SelectionRuntime_SelfDestruct(Ptr32<GameEntityRuntime> *selectionEntries);

void SelectionPointerArray_SetAircraftPadTargets
          (SelectionMarkerLaneMask laneMask,SelectionMarkerCoordinateValue32 heading16,
          SelectionMarkerCoordinateValue32 worldXQ12,SelectionMarkerCoordinateValue32 worldYQ12,
          SelectionPointerArray32 *selection);

extern SelectionPlayerRuntimeBlock *g_SelectionPlayerRuntimeBlockPointers[256]; /* indexed by player runtime id (0..254, FrontendTransfer_FindLowestFreePlayerRuntimeId), 0x400 bytes in the original */

/* Entries of g_InGamePointerModeHandlers (InGameSelection_SetAircraftPadTargetLane1/2,
   SelectionMarkerCoordinates_ApplyType3..7): four arguments. */
using InGamePointerModeHandler = void
          (SelectionMarkerIndex selectionIndex,SelectionMarkerCoordinateValue32 valueC,
          SelectionMarkerCoordinateValue32 valueB,SelectionMarkerCoordinateValue32 valueA);

void SelectionMarkerCoordinates_ApplyType3(SelectionMarkerIndex playerId,SelectionMarkerCoordinateValue32 heading,
          SelectionMarkerCoordinateValue32 worldXQ12,SelectionMarkerCoordinateValue32 worldYQ12);

void SelectionMarkerCoordinates_ApplyType4(SelectionMarkerIndex playerId,SelectionMarkerCoordinateValue32 heading,
          SelectionMarkerCoordinateValue32 worldXQ12,SelectionMarkerCoordinateValue32 worldYQ12);

void SelectionMarkerCoordinates_ApplyType5(SelectionMarkerIndex playerId,SelectionMarkerCoordinateValue32 heading,
          SelectionMarkerCoordinateValue32 worldXQ12,SelectionMarkerCoordinateValue32 worldYQ12);

void SelectionMarkerCoordinates_ApplyType6(SelectionMarkerIndex playerId,SelectionMarkerCoordinateValue32 heading,
          SelectionMarkerCoordinateValue32 worldXQ12,SelectionMarkerCoordinateValue32 worldYQ12);

void SelectionMarkerCoordinates_ApplyType7(SelectionMarkerIndex playerId,SelectionMarkerCoordinateValue32 heading,
          SelectionMarkerCoordinateValue32 worldXQ12,SelectionMarkerCoordinateValue32 worldYQ12);

extern InGamePointerModeHandler *g_InGamePointerModeHandlers[8];

#endif /* THANDOR_GAMEPLAY_SELECTION_COMMANDS_H */

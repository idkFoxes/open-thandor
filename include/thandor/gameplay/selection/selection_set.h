/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/include/thandor/gameplay/selection/selection_set.h
 * Reverse engineering by idkFoxes 2026
 */

#ifndef THANDOR_GAMEPLAY_SELECTION_SELECTION_SET_H
#define THANDOR_GAMEPLAY_SELECTION_SELECTION_SET_H

#include <thandor/generated/types.h>
#include <thandor/core/contracts.h>

/* Submodule: gameplay/selection/selection_set. */

/* Entries of a SelectionPointerArray32 / SelectionInfoEntitySlots (empty entries are NULL). */
#define SELECTION_ENTRY_CAPACITY 32
/* One count in byte lane laneByte of a packed per-byte counter (SelectionPointerArray_SetAircraftPadTargets: lane 0
   bits 0-7, lane 1 bits 8-15, lane 2 bits 16-23) */
#define SELECTION_PACKED_LANE_ONE(laneByte) (1 << ((laneByte) * 8))

/* Functions are grouped by semantic ownership. */

void SelectionPlayerRuntime_MovePrimarySelectionBy
          (PlayerRuntimeId playerRuntimeId,uint32_t reserved,Q12 deltaYQ12,Q12 deltaXQ12);

void SelectionPlayerRuntime_RotatePrimarySelectionBy
          (PlayerRuntimeId playerRuntimeId,uint32_t reserved0,uint32_t reserved1,AngleTurn32 angleDelta);

void SelectionPlayerBlocks_RemovePointer(GameEntityRuntime *target);

void SelectionPointerArray_RemoveFirstMatch(GameEntityRuntime *target,SelectionPointerArray32 *array);

void SelectionPlayerRuntime_ClearTerrainEditSelectionState
          (PlayerRuntimeId playerRuntimeId,uint32_t reservedZero0,uint32_t reservedZero1,
          uint32_t reservedZero2);

Bool8 SelectionPlayerPairList_ContainsPair(SelectionPlayerPairValue worldYQ12,SelectionPlayerPairKey worldXQ12,
          PlayerRuntimeId playerRuntimeId);

void SelectionPointerArray_AddWorldEntriesMatchingRuntimeIdentity
          (ArmyRuntimeSlot *sourceArmyRuntime,SelectionPointerArray32 *selection);

void SelectionPointerArray_InsertUniqueAndRecenter(GameEntityRuntime *entityRuntime,SelectionPointerArray32 *selection);

void SelectionPointerArray_RecenterOffsetsAroundAveragePosition(SelectionPointerArray32 *selection);

Bool8 SelectionPointerArray_Contains(GameEntityRuntime *target,SelectionPointerArray32 *array);

Bool8 SelectionPointerArray_IsSpatialSpreadTooLarge(SelectionPointerArray32 *selection);

void SelectionPointerArray_Clear32(SelectionPointerArray32 *array);

#endif /* THANDOR_GAMEPLAY_SELECTION_SELECTION_SET_H */

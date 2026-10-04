/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/include/thandor/world/terrain/field_edit_commands.h
 * Reverse engineering by idkFoxes 2026
 */

#ifndef THANDOR_WORLD_TERRAIN_FIELD_EDIT_COMMANDS_H
#define THANDOR_WORLD_TERRAIN_FIELD_EDIT_COMMANDS_H

#include <thandor/core/types.h>
#include <thandor/gameplay/army/types.h>
#include <thandor/world/terrain/types.h>
#include <thandor/core/contracts.h>

/* The terrain height scratch plane / material edit plane of the player a map-editor command names, or NULL
   (logged once) when the player has no selection block or the plane is not set; the commands then do nothing. */
int *FieldGridEdit_PlayerHeightPlane(PlayerRuntimeId playerRuntimeId);

uint32_t *FieldGridEdit_PlayerMaterialPlane(PlayerRuntimeId playerRuntimeId);

void FieldGrid_ApplyPositiveCellDeltas(PlayerRuntimeId playerRuntimeId,Q12 anchorRowQ12,Q12 anchorColumnQ12,
          PackedFieldGridDeltaXY16 packedDragDeltaXY16);

void FieldGrid_ApplyNegativeCellDeltas(PlayerRuntimeId playerRuntimeId,Q12 anchorRowQ12,Q12 anchorColumnQ12,
          PackedFieldGridDeltaXY16 packedDragDeltaXY16);

void FieldGrid_RebuildLocalInfluenceState
          (PlayerRuntimeId playerRuntimeId,FieldGridCommandReservedValue reservedCommandValue,
          Q12 gridRowQ12,Q12 gridColumnQ12);

void FieldGrid_ApplyLocalCellUpdate
          (PlayerRuntimeId playerRuntimeId,FieldGridTransitionValue transitionValue,Q12 gridRowQ12,
          Q12 gridColumnQ12);

void FieldGrid_ApplyEncodedCellUpdate(PlayerRuntimeId playerRuntimeId,Q12 gridRowQ12,Q12 gridColumnQ12,
          PackedFieldGridDeltaXY16 packedDragDeltaXY16);

void FieldGrid_SetCellFluidReceiverExcluded
          (PlayerRuntimeId playerRuntimeId,FieldGridRegionMask setMask,Q12 gridRowQ12,Q12 gridColumnQ12);

void FieldGrid_SetCellFluidSourceExcluded
          (PlayerRuntimeId playerRuntimeId,FieldGridRegionMask setMask,Q12 gridRowQ12,Q12 gridColumnQ12);

void FieldGrid_SetCellResourceSupportFlag
          (PlayerRuntimeId playerRuntimeId,FieldGridMaterialBitIndex materialBitIndex,Q12 gridRowQ12,
          Q12 gridColumnQ12);

void FieldGrid_ClearPlayerScratchPlane
          (PlayerRuntimeId playerRuntimeId,FieldGridCommandReservedValue reservedCommandValue,
          Q12 reservedWorldYQ12,Q12 reservedWorldXQ12);

void FieldGrid_ResetLocalInfluenceState
          (PlayerRuntimeId playerRuntimeId,FieldGridCommandReservedValue reservedCommandValue,
          Q12 reservedWorldYQ12,Q12 reservedWorldXQ12);

void FieldGrid_ApplyEncodedUpdateCore(FieldGridHeightDeltaUnits heightDeltaUnits,Q12 gridRowQ12,Q12 gridColumnQ12,
          FieldGridAsset *fieldGrid);

void FieldGrid_ProcessHorizontalSpan(Q12 sourceRowQ12,Q12 sourceColumnQ12,FieldGridHeightDeltaUnits heightDeltaUnits,
          FieldGridRadiusUnits radiusUnits,Q12 centerRowQ12,Q12 centerColumnQ12,
          FieldGridAccumulatorValue *accumulatorPlane,FieldGridAsset *fieldGrid);

void FieldGrid_ProcessVerticalSpan(FieldGridHeightDeltaUnits heightDeltaUnits,FieldGridRadiusUnits radiusUnits,
          Q12 centerRowQ12,Q12 centerColumnQ12,FieldGridAccumulatorValue *accumulatorPlane,
          FieldGridAsset *fieldGrid);

void FieldGrid_ApplySingleCellTransition(FieldGridTransitionValue transitionValue,Q12 gridRowQ12,Q12 gridColumnQ12,
          FieldGridAsset *fieldGrid);

void FieldGrid_ApplyRectangularTransition(Q12 gridRowQ12,Q12 gridColumnQ12,FieldGridAsset *fieldGrid);

void FieldGrid_ApplyMaskedRegionCore
          (FieldGridRegionMask preserveMask,FieldGridRegionMask setMask,Q12 gridRowQ12,Q12 gridColumnQ12,
          FieldGridAsset *fieldGrid);

#endif /* THANDOR_WORLD_TERRAIN_FIELD_EDIT_COMMANDS_H */

/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/include/thandor/gameplay/selection/runtime.h
 * Reverse engineering by idkFoxes 2026
 */

#ifndef THANDOR_GAMEPLAY_SELECTION_RUNTIME_H
#define THANDOR_GAMEPLAY_SELECTION_RUNTIME_H

#include <thandor/generated/types.h>
#include <thandor/core/contracts.h>

/* Submodule: gameplay/selection/runtime. */

/* Entries of a SelectionPointerArray32 / SelectionInfoEntitySlots (empty entries are NULL). */
#define SELECTION_ENTRY_CAPACITY 32
/* One count in byte lane laneByte of a packed per-byte counter (SelectionPointerArray_SetAircraftPadTargets: lane 0
   bits 0-7, lane 1 bits 8-15, lane 2 bits 16-23) */
#define SELECTION_PACKED_LANE_ONE(laneByte) (1 << ((laneByte) * 8))

/* Selection/info panel layout (g_SelectionPanelData, loaded from select.dat or info.dat by
   SelectionInfoPanel_InitResources): a dword header followed by 16-byte cell records. The offsets below are
   relative to g_SelectionPanelData + cellIndex * SELECTION_PANEL_CELL_SIZE. */
#define SELECTION_PANEL_CELL_SIZE 0x10
#define SELECTION_PANEL_CELL_FLAGS 0x4             /* SELECTION_PANEL_CELL_FLAG_* */
#define SELECTION_PANEL_CELL_BASE_SUBRESOURCE 0x8  /* first sprite of the cell in the panel texture */
#define SELECTION_PANEL_CELL_OFFSET_X 0xc          /* added to the horizontal draw coordinate */
#define SELECTION_PANEL_CELL_OFFSET_Y 0x10         /* added to the vertical draw coordinate */
#define SELECTION_PANEL_CELL_FLAG_NO_ADVANCE_X 0x4 /* the next horizontal coordinate ignores the sprite width */
#define SELECTION_PANEL_CELL_FLAG_NO_ADVANCE_Y 0x8 /* the next vertical coordinate ignores the sprite height */
#define SELECTION_PANEL_CELL_FLAG_ALIGN_START 0x100 /* bar content starts at the start cap (else centred) */
#define SELECTION_PANEL_CELL_FLAG_ALIGN_END 0x200   /* bar content ends at the end cap (else centred) */
#define SELECTION_PANEL_CELL_FLAG_SHOW_EMPTY_SEGMENTS 0x400 /* segmented bars also draw the unfilled segments */
/* Cells used by SelectionPanel_RenderArmyRuntimeMetrics around the projected bounds of a selected model. */
#define SELECTION_PANEL_CELL_CORNER_TOP_LEFT 0
#define SELECTION_PANEL_CELL_CORNER_TOP_RIGHT 1
#define SELECTION_PANEL_CELL_CORNER_BOTTOM_LEFT 2
#define SELECTION_PANEL_CELL_CORNER_BOTTOM_RIGHT 3
#define SELECTION_PANEL_CELL_TOP_BAR_EMPTY 4        /* top edge without a value */
#define SELECTION_PANEL_CELL_LEFT_BAR 5
#define SELECTION_PANEL_CELL_RIGHT_BAR 6
#define SELECTION_PANEL_CELL_GROUP_NUMBER 0xb       /* top right: runtime group index */
#define SELECTION_PANEL_CELL_LEFT_SEGMENTS 0xf
#define SELECTION_PANEL_CELL_RIGHT_SEGMENTS 0x10
#define SELECTION_PANEL_CELL_HIERARCHY_METER 0x12   /* top left: active/total hierarchy metric */
#define SELECTION_PANEL_CELL_TOP_BAR 0x16
#define SELECTION_PANEL_CELL_BOTTOM_BAR 0x19        /* hierarchy scale ratio */
/* Functions are grouped by semantic ownership. */

void SelectionPanel_RenderArmyRuntimeMetrics
          (UiPixelCoordinate clipBottom,UiPixelCoordinate clipRight,UiPixelCoordinate clipTop,
          UiPixelCoordinate clipLeft,UiPixelCoordinate panelBottom,UiPixelCoordinate panelRight,
          UiPixelCoordinate panelTop,UiPixelCoordinate panelLeft,
          RuntimeModelFactionPrefix *runtimeEntry);

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

void SelectionPlayerRuntime_MovePrimarySelectionBy
          (PlayerRuntimeId playerRuntimeId,uint32_t reserved,Q12 deltaYQ12,Q12 deltaXQ12);

void SelectionPlayerRuntime_RotatePrimarySelectionBy
          (PlayerRuntimeId playerRuntimeId,uint32_t reserved0,uint32_t reserved1,AngleTurn32 angleDelta);

bool SelectionInfoPanel_InitResources(SelectionInfoEntitySlots *entitySlots,uint32_t *outError);

void SelectionInfoPanel_ShutdownResources(void);

void SelectionPlayerBlocks_RemovePointer(GameEntityRuntime *target);

bool SelectionInfoEntitySlots_ComputeAverageWorldPosition(FixedVectorQ12 *outPosition);

void SelectionPointerArray_RemoveFirstMatch(GameEntityRuntime *target,SelectionPointerArray32 *array);

bool SelectionInfo_HasAnyEntry(void);

bool SelectionInfo_AllEntriesEmptyOrMatchOwner(FactionRuntimeIndex ownerIndex);

bool SelectionInfo_TestNotOwnAircraftPadsWithAircraft(FactionRuntimeIndex ownerIndex);

bool SelectionInfo_TestAnyActiveOrSingleClass13(void);

bool SelectionInfo_TestPositionCommandAtWorldPoint(Q12 worldXQ12,Q12 worldYQ12,WorldRuntimeContext *inGameRuntime);

bool SelectionInfo_TestNoEntryHasWeaponDamage(void);

bool SelectionInfo_TestAnyEntryWeaponDamageNonnegative(void);

GameEntityRuntime * __cdecl SelectionInfo_GetFirstEntry(void);

bool SelectionInfo_IsEntryAbsent(GameEntityRuntime *entry);

uint32_t SelectionInfo_CollectAttachmentEffectVariantMask(void);

uint32_t __cdecl SelectionInfo_CollectCapabilityFlags(void);

void SelectionPlayerRuntime_ClearTerrainEditSelectionState
          (PlayerRuntimeId playerRuntimeId,uint32_t reservedZero0,uint32_t reservedZero1,
          uint32_t reservedZero2);

bool SelectionPlayerPairList_ContainsPair(SelectionPlayerPairValue worldYQ12,SelectionPlayerPairKey worldXQ12,
          PlayerRuntimeId playerRuntimeId);

void SelectionPointerArray_ApplyMoveCommand
          (Q12 targetWorldY,Q12 targetWorldX,SelectionPointerArray32 *selection);

void SelectionRuntime_ResetMovementPruneAndRecenterEntries(GameEntityRuntime **selectionEntries);

void SelectionPointerArray_AddWorldEntriesMatchingRuntimeIdentity
          (ArmyRuntimeSlot *sourceArmyRuntime,SelectionPointerArray32 *selection);

void SelectionPointerArray_ApplyPositionCommand(Q12 targetWorldY,Q12 targetWorldX,SelectionPointerArray32 *selection);

SelectionPanelCellAdvance SelectionPanel_DrawNumberCellAndAdvance
          (UiPixelCoordinate clipBottom,UiPixelCoordinate clipRight,UiPixelCoordinate clipTop,
          UiPixelCoordinate clipLeft,UiPixelCoordinate originY,UiPixelCoordinate originX,
          SelectionPanelNumericValue32 value,SelectionPanelCellIndex cellIndex);

SelectionPanelCellAdvance SelectionPanel_DrawIconCellAndAdvance
          (UiPixelCoordinate clipBottom,UiPixelCoordinate clipRight,UiPixelCoordinate clipTop,
          UiPixelCoordinate clipLeft,UiPixelCoordinate originY,UiPixelCoordinate originX,
          SelectionPanelCellIndex cellIndex);

SelectionPanelCellAdvance SelectionPanel_DrawSteppedMeterCellAndAdvance
          (UiPixelCoordinate clipBottom,UiPixelCoordinate clipRight,UiPixelCoordinate clipTop,
          UiPixelCoordinate clipLeft,UiPixelCoordinate originY,UiPixelCoordinate originX,
          UiNumericValue32 maximumValue,UiNumericValue32 currentValue,
          SelectionPanelCellIndex cellIndex);

void SelectionPanel_DrawProportionalCappedBar
          (UiPixelCoordinate clipBottom,UiPixelCoordinate clipRight,UiPixelCoordinate clipTop,
          UiPixelCoordinate clipLeft,UiPixelCoordinate fixedCoordinate,
          UiPixelCoordinate barEndCoordinate,UiPixelCoordinate barStartCoordinate,
          UiNumericValue32 maximumValue,UiNumericValue32 currentValue,
          SelectionPanelCellIndex cellIndex);

void SelectionPanel_DrawForwardCappedBar
          (UiPixelCoordinate clipBottom,UiPixelCoordinate clipRight,UiPixelCoordinate clipTop,
          UiPixelCoordinate clipLeft,UiPixelCoordinate fixedCoordinate,
          UiPixelCoordinate barEndCoordinate,UiPixelCoordinate barStartCoordinate,
          SelectionPanelCellIndex cellIndex);

void SelectionPanel_DrawSolidCappedBar
          (UiPixelCoordinate clipBottom,UiPixelCoordinate clipRight,UiPixelCoordinate clipTop,
          UiPixelCoordinate clipLeft,UiPixelCoordinate barEndCoordinate,
          UiPixelCoordinate barStartCoordinate,UiPixelCoordinate fixedCoordinate,
          SelectionPanelCellIndex cellIndex);

void SelectionPanel_DrawSegmentedCappedBar
          (UiPixelCoordinate clipBottom,UiPixelCoordinate clipRight,UiPixelCoordinate clipTop,
          UiPixelCoordinate clipLeft,UiPixelCoordinate barEndCoordinate,
          UiPixelCoordinate barStartCoordinate,UiPixelCoordinate fixedCoordinate,
          SelectionPanelSegmentCount totalSegmentCount,SelectionPanelSegmentCount filledSegmentCount
          ,SelectionPanelCellIndex cellIndex);

void SelectionPointerArray_ApplyArmyRuntimeTarget
          (ArmyRuntimeSlot *targetArmyRuntime,SelectionPointerArray32 *selection);

void SelectionPointerArray_ApplyTargetPositionCommand
          (Q12 coordinateA,uint32_t coordinateB,Q12 coordinateC,SelectionPointerArray32 *selection);

void SelectionRuntime_StopMovement(GameEntityRuntime **selectionEntries);

void SelectionRuntime_CancelTargets(GameEntityRuntime **selectionEntries);

void SelectionRuntime_SelfDestruct(GameEntityRuntime **selectionEntries);

void SelectionPointerArray_InsertUniqueAndRecenter(GameEntityRuntime *entityRuntime,SelectionPointerArray32 *selection);

void SelectionPointerArray_RecenterOffsetsAroundAveragePosition(SelectionPointerArray32 *selection);

bool SelectionPointerArray_Contains(GameEntityRuntime *target,SelectionPointerArray32 *array);

bool SelectionPointerArray_IsSpatialSpreadTooLarge(SelectionPointerArray32 *selection);

void SelectionPointerArray_SetAircraftPadTargets
          (SelectionMarkerLaneMask laneMask,SelectionMarkerCoordinateValue32 heading16,
          SelectionMarkerCoordinateValue32 worldXQ12,SelectionMarkerCoordinateValue32 worldYQ12,
          SelectionPointerArray32 *selection);

void SelectionPointerArray_Clear32(SelectionPointerArray32 *array);

#endif /* THANDOR_GAMEPLAY_SELECTION_RUNTIME_H */

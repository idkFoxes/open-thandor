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
/* Functions are grouped by semantic ownership; address comments are executable virtual addresses. */

/* 0x0052E350 */
void __thandor_void_preserve_eax_ecx_edx
SelectionPanel_RenderArmyRuntimeMetrics
          (UiPixelCoordinate clipTop,UiPixelCoordinate clipLeft,UiPixelCoordinate clipBottom,
          UiPixelCoordinate clipRight,UiPixelCoordinate panelBottom,UiPixelCoordinate panelRight,
          UiPixelCoordinate panelTop,UiPixelCoordinate panelLeft,
          RuntimeModelFactionPrefix10 *runtimeEntry);

/* 0x0055FA20 */
void __thandor_void_preserve_eax_ecx_edx
InGameSelection_RebuildOwnedClass16Selection
          (PlayerRuntimeId playerRuntimeId,uint32_t callbackArg1,uint32_t callbackArg2,uint32_t callbackArg3);

/* 0x0055FB30 */
void __thandor_preserve_eax
InGamePlayerSelection_ReplaceWithArmyRuntimeIndex
          (PlayerRuntimeId playerId,uint32_t payloadDword04,uint32_t payloadDword08,
          RuntimeToken armyRuntimeIndex);

/* 0x0055FE70 */
void __thandor_preserve_eax_edx
InGamePlayerSelection_ApplyPositionCommandVariantB
          (PlayerRuntimeId playerId,uint32_t payloadDword04,CommandPayloadDword08 worldXQ12,
          CommandPayloadDword0C worldYQ12);

/* 0x0055FEA0 */
void __thandor_void_preserve_eax_ecx_edx
InGamePlayerSelection_ApplyPositionCommand
          (PlayerRuntimeId playerId,uint32_t payloadDword04,CommandPayloadDword08 worldXQ12,
          CommandPayloadDword0C worldYQ12);

/* 0x0055FED0 */
void __thandor_preserve_eax
InGamePlayerSelection_SelectArmyRuntimeIndex
          (PlayerRuntimeId playerId,uint32_t payloadDword04,uint32_t payloadDword08,
          RuntimeToken armyRuntimeIndex);

/* 0x0055FF10 */
void __thandor_void_preserve_eax_ecx_edx
InGamePlayerSelection_ApplyTargetPositionCommand
          (PlayerRuntimeId playerId,CommandPayloadDword04 surfaceHeightQ12,
          CommandPayloadDword08 worldXQ12,CommandPayloadDword0C worldYQ12);

/* 0x0055FF40 */
void __thandor_preserve_eax
PlayerSelection_ResetMovementPruneAndRecenterEntries
          (PlayerRuntimeId playerId,CommandPayloadDword04 payloadDword04,
          CommandPayloadDword08 payloadDword08,CommandPayloadDword0C payloadDword0C);

/* 0x0055FF60 */
void __thandor_preserve_eax
PlayerSelection_ResetMovementAnchorsAndClearFlag200ForEligibleEntries
          (PlayerRuntimeId playerId,CommandPayloadDword04 payloadDword04,
          CommandPayloadDword08 payloadDword08,CommandPayloadDword0C payloadDword0C);

/* 0x0055FF80 */
void __thandor_preserve_eax
PlayerSelection_InterruptTargetsAndClearFlag10ForEligibleEntries
          (PlayerRuntimeId playerId,CommandPayloadDword04 payloadDword04,
          CommandPayloadDword08 payloadDword08,CommandPayloadDword0C payloadDword0C);

/* 0x0055FFA0 */
void __thandor_preserve_eax
PlayerSelection_ApplyFlags418UnlessBit8ToEligibleEntries
          (PlayerRuntimeId playerId,CommandPayloadDword04 payloadDword04,
          CommandPayloadDword08 payloadDword08,CommandPayloadDword0C payloadDword0C);

/* 0x0055FFC0 */
void __thandor_void_preserve_eax_ecx_edx
InGameSelection_ApplyType16MarkerCoordinatesVariant1
          (SelectionMarkerIndex playerRuntimeId,SelectionMarkerCoordinateValue32 heading16,
          SelectionMarkerCoordinateValue32 worldXQ12,SelectionMarkerCoordinateValue32 worldYQ12);

/* 0x0055FFF0 */
void __thandor_void_preserve_eax_ecx_edx
InGameSelection_ApplyType16MarkerCoordinatesVariant2
          (SelectionMarkerIndex playerRuntimeId,SelectionMarkerCoordinateValue32 heading16,
          SelectionMarkerCoordinateValue32 worldXQ12,SelectionMarkerCoordinateValue32 worldYQ12);

/* 0x00562050 */
void __thandor_void_preserve_eax_ecx_edx
SelectionPlayerRuntime_MovePrimarySelectionBy
          (PlayerRuntimeId playerRuntimeId,uint32_t reserved,Q12 deltaYQ12,Q12 deltaXQ12);

/* 0x00562220 */
void __thandor_void_preserve_eax_ecx_edx
SelectionPlayerRuntime_RotatePrimarySelectionBy
          (PlayerRuntimeId playerRuntimeId,uint32_t reserved0,uint32_t reserved1,AngleTurn32 angleDelta);

/* 0x0052CEE0 */
StatusResult __thandor_eax_cf_preserve_ecx_edx
SelectionInfoPanel_InitResources(SelectionInfoEntitySlots *entitySlots);

/* 0x0052D0F0 */
void __thandor_preserve_eax SelectionInfoPanel_ShutdownResources(void);

/* 0x0052FB20 */
void __thandor_void_preserve_eax_ecx_edx
SelectionPlayerBlocks_RemovePointer(GameEntityRuntime *target);

/* 0x0052FB70 */
WorldPositionResult __cdecl SelectionInfoEntitySlots_ComputeAverageWorldPositionRegs(void);

/* 0x0052FD60 */
void __thandor_void_preserve_eax_ecx_edx
SelectionPointerArray_RemoveFirstMatch(GameEntityRuntime *target,SelectionPointerArray32 *array);

/* 0x0052FDC0 */
bool __thandor_cf_preserve_eax_ecx_edx SelectionInfo_HasAnyEntry(void);

/* 0x0052FDE0 */
bool __thandor_cf_preserve_eax_ecx_edx
SelectionInfo_AllEntriesEmptyOrMatchOwner(FactionRuntimeIndex ownerIndex);

/* 0x0052FE30 */
bool __thandor_cf_preserve_eax_ecx_edx
SelectionInfo_ValidateOwnerType16AndAnyActive(FactionRuntimeIndex ownerIndex);

/* 0x0052FEB0 */
bool __thandor_cf_preserve_eax_ecx_edx SelectionInfo_TestAnyActiveOrSingleClass13(void);

/* 0x0052FF30 */
bool __thandor_cf_preserve_eax_ecx_edx
SelectionInfo_TestPositionCommandAtWorldPoint
          (Q12 worldXQ12,Q12 worldYQ12,WorldRuntimeContext *inGameRuntime);

/* 0x00530050 */
bool __thandor_cf_preserve_eax_ecx_edx SelectionInfo_TestAllStateField100Nonpositive(void);

/* 0x005300A0 */
bool __thandor_cf_preserve_eax_ecx_edx SelectionInfo_TestAnyStateField100Nonnegative(void);

/* 0x005300E0 */
GameEntityRuntime * __cdecl SelectionInfo_GetFirstEntry(void);

/* 0x00530100 */
bool __thandor_cf_preserve_eax_ecx_edx SelectionInfo_FindEntry(GameEntityRuntime *entry);

/* 0x00530770 */
uint32_t __thandor_eax_preserve_ecx_edx SelectionInfo_CollectAttachmentEffectVariantMask(void);

/* 0x005307C0 */
uint32_t __cdecl SelectionInfo_CollectCapabilityFlags(void);

/* 0x00561000 */
void __thandor_void_preserve_eax_ecx_edx
SelectionPlayerRuntime_ClearTerrainEditSelectionState
          (PlayerRuntimeId playerRuntimeId,uint32_t reservedZero0,uint32_t reservedZero1,
          uint32_t reservedZero2);

/* 0x00571020 */
bool __thandor_cf_preserve_eax_ecx_edx
SelectionPlayerPairList_ContainsPair
          (SelectionPlayerPairValue worldYQ12,SelectionPlayerPairKey worldXQ12,
          PlayerRuntimeId playerRuntimeId);

/* 0x005302B0 */
void __thandor_void_preserve_eax_ecx_edx
SelectionPointerArray_ApplyPositionCommandVariantB
          (Q12 targetWorldY,Q12 targetWorldX,SelectionPointerArray32 *selection);

/* 0x00530420 */
void __thandor_void_preserve_eax_ecx_edx
SelectionRuntime_ResetMovementPruneAndRecenterEntries(GameEntityRuntime **selectionEntries);

/* 0x0052FCE0 */
void __thandor_void_preserve_ecx_edx
SelectionPointerArray_AddWorldEntriesMatchingRuntimeIdentity
          (ArmyRuntimeSlot *sourceArmyRuntime,SelectionPointerArray32 *selection);

/* 0x005303A0 */
void __thandor_void_preserve_eax_ecx_edx
SelectionPointerArray_ApplyPositionCommand
          (Q12 targetWorldY,Q12 targetWorldX,SelectionPointerArray32 *selection);

/* 0x0052D600 */
SelectionPanelAdvanceEaxEdx8 __thandor_eax_edx_cf_preserve_ecx
SelectionPanel_DrawNumberCellAndAdvanceRegs
          (UiPixelCoordinate clipTop,UiPixelCoordinate clipLeft,UiPixelCoordinate clipBottom,
          UiPixelCoordinate clipRight,UiPixelCoordinate originY,UiPixelCoordinate originX,
          SelectionPanelNumericValue32 value,SelectionPanelCellIndex cellIndex);

/* 0x0052D6F0 */
SelectionPanelAdvanceEaxEdx8 __thandor_eax_edx_cf_preserve_ecx
SelectionPanel_DrawIconCellAndAdvanceRegs
          (UiPixelCoordinate clipTop,UiPixelCoordinate clipLeft,UiPixelCoordinate clipBottom,
          UiPixelCoordinate clipRight,UiPixelCoordinate originY,UiPixelCoordinate originX,
          SelectionPanelCellIndex cellIndex);

/* 0x0052D770 */
SelectionPanelAdvanceEaxEdx8 __thandor_eax_edx_cf_preserve_ecx
SelectionPanel_DrawSteppedMeterCellAndAdvanceRegs
          (UiPixelCoordinate clipTop,UiPixelCoordinate clipLeft,UiPixelCoordinate clipBottom,
          UiPixelCoordinate clipRight,UiPixelCoordinate originY,UiPixelCoordinate originX,
          UiNumericValue32 maximumValue,UiNumericValue32 currentValue,
          SelectionPanelCellIndex cellIndex);

/* 0x0052D850 */
void __thandor_void_preserve_eax_ecx_edx
SelectionPanel_DrawProportionalCappedBar
          (UiPixelCoordinate clipTop,UiPixelCoordinate clipLeft,UiPixelCoordinate clipBottom,
          UiPixelCoordinate clipRight,UiPixelCoordinate fixedCoordinate,
          UiPixelCoordinate barEndCoordinate,UiPixelCoordinate barStartCoordinate,
          UiNumericValue32 maximumValue,UiNumericValue32 currentValue,
          SelectionPanelCellIndex cellIndex);

/* 0x0052DAF0 */
void __thandor_void_preserve_eax_ecx_edx
SelectionPanel_DrawForwardCappedBar
          (UiPixelCoordinate clipTop,UiPixelCoordinate clipLeft,UiPixelCoordinate clipBottom,
          UiPixelCoordinate clipRight,UiPixelCoordinate fixedCoordinate,
          UiPixelCoordinate barEndCoordinate,UiPixelCoordinate barStartCoordinate,
          SelectionPanelCellIndex cellIndex);

/* 0x0052DBC0 */
void __thandor_void_preserve_eax_ecx_edx
SelectionPanel_DrawSolidCappedBar
          (UiPixelCoordinate clipTop,UiPixelCoordinate clipLeft,UiPixelCoordinate clipBottom,
          UiPixelCoordinate clipRight,UiPixelCoordinate barEndCoordinate,
          UiPixelCoordinate barStartCoordinate,UiPixelCoordinate fixedCoordinate,
          SelectionPanelCellIndex cellIndex);

/* 0x0052DFF0 */
void __thandor_void_preserve_eax_ecx_edx
SelectionPanel_DrawSegmentedCappedBar
          (UiPixelCoordinate clipTop,UiPixelCoordinate clipLeft,UiPixelCoordinate clipBottom,
          UiPixelCoordinate clipRight,UiPixelCoordinate barEndCoordinate,
          UiPixelCoordinate barStartCoordinate,UiPixelCoordinate fixedCoordinate,
          SelectionPanelSegmentCount totalSegmentCount,SelectionPanelSegmentCount filledSegmentCount
          ,SelectionPanelCellIndex cellIndex);

/* 0x00530130 */
void __thandor_void_preserve_eax_ecx_edx
SelectionPointerArray_ApplyArmyRuntimeTarget
          (ArmyRuntimeSlot *targetArmyRuntime,SelectionPointerArray32 *selection);

/* 0x00530190 */
void __thandor_void_preserve_eax_ecx_edx
SelectionPointerArray_ApplyTargetPositionCommand
          (Q12 coordinateA,uint32_t coordinateB,Q12 coordinateC,SelectionPointerArray32 *selection);

/* 0x00530540 */
void __thandor_void_preserve_eax_ecx_edx
SelectionRuntime_ResetMovementAnchorsAndClearFlag200ForEligibleEntries
          (GameEntityRuntime **selectionEntries);

/* 0x005305A0 */
void __thandor_void_preserve_eax_ecx_edx
SelectionRuntime_InterruptTargetsAndClearFlag10ForEligibleEntries
          (GameEntityRuntime **selectionEntries);

/* 0x00530600 */
void __thandor_void_preserve_eax_ecx
SelectionRuntime_ApplyFlags418UnlessBit8ToEligibleEntries(GameEntityRuntime **selectionEntries);

/* 0x0052FCA0 */
void __thandor_void_preserve_eax_ecx_edx
SelectionPointerArray_InsertUniqueAndRecenter
          (GameEntityRuntime *entityRuntime,SelectionPointerArray32 *selection);

/* 0x0052FBF0 */
void __thandor_void_preserve_eax_ecx_edx
SelectionPointerArray_RecenterOffsetsAroundAveragePosition(SelectionPointerArray32 *selection);

/* 0x0052FD90 */
bool __thandor_cf_preserve_eax_ecx_edx
SelectionPointerArray_Contains(GameEntityRuntime *target,SelectionPointerArray32 *array);

/* 0x005301F0 */
bool __thandor_cf_preserve_eax_ecx_edx
SelectionPointerArray_IsSpatialSpreadTooLarge(SelectionPointerArray32 *selection);

/* 0x00530650 */
void __thandor_void_preserve_eax_ecx_edx
SelectionPointerArray_ApplyType16MarkerCoordinates
          (SelectionMarkerLaneMask laneMask,SelectionMarkerCoordinateValue32 heading16,
          SelectionMarkerCoordinateValue32 worldXQ12,SelectionMarkerCoordinateValue32 worldYQ12,
          SelectionPointerArray32 *selection);

/* 0x0052FB00 */
void __thandor_void_preserve_eax_ecx_edx
SelectionPointerArray_Clear32(SelectionPointerArray32 *array);


/* 0x0052D150 */
void __thandor_void_preserve_eax_ecx_edx
SelectionPanel_DrawHorizontalNumberTextCappedBar
          (UiPixelCoordinate clipTop,UiPixelCoordinate clipLeft,UiPixelCoordinate clipBottom,
          UiPixelCoordinate clipRight,UiPixelCoordinate fixedCoordinate,
          UiPixelCoordinate spanEndCoordinate,UiPixelCoordinate spanStartCoordinate,
          uint16_t *commandStream,SelectionPanelCellIndex cellIndex);

/* 0x0052D9A0 */
void __thandor_void_preserve_eax_ecx_edx
SelectionPanel_DrawVerticalProportionalCappedBar
          (UiPixelCoordinate clipTop,UiPixelCoordinate clipLeft,UiPixelCoordinate clipBottom,
          UiPixelCoordinate clipRight,UiPixelCoordinate barEndCoordinate,
          UiPixelCoordinate barStartCoordinate,UiPixelCoordinate fixedCoordinate,
          UiNumericValue32 maximumValue,UiNumericValue32 currentValue,
          SelectionPanelCellIndex cellIndex);

/* 0x0052DC90 */
void __thandor_void_preserve_eax_ecx_edx
SelectionPanel_DrawHorizontalSegmentedCappedBar
          (UiPixelCoordinate clipTop,UiPixelCoordinate clipLeft,UiPixelCoordinate clipBottom,
          UiPixelCoordinate clipRight,UiPixelCoordinate fixedCoordinate,
          UiPixelCoordinate spanEndCoordinate,UiPixelCoordinate spanStartCoordinate,
          SelectionPanelSegmentCount totalSegmentCount,SelectionPanelSegmentCount filledSegmentCount
          ,SelectionPanelCellIndex cellIndex);

#endif /* THANDOR_GAMEPLAY_SELECTION_RUNTIME_H */

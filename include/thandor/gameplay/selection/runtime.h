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
          (PlayerRuntimeId playerRuntimeId,dword callbackArg1,dword callbackArg2,dword callbackArg3);

/* 0x0055FB30 */
void __thandor_preserve_eax
InGamePlayerSelection_ReplaceWithArmyRuntimeIndex
          (PlayerRuntimeId playerId,dword payloadDword04,dword payloadDword08,
          RuntimeToken armyRuntimeIndex);

/* 0x0055FE70 */
void __thandor_preserve_eax_edx
InGamePlayerSelection_ApplyPositionCommandVariantB
          (PlayerRuntimeId playerId,dword payloadDword04,CommandPayloadDword08 payloadDword08,
          CommandPayloadDword0C payloadDword0C);

/* 0x0055FEA0 */
void __thandor_void_preserve_eax_ecx_edx
InGamePlayerSelection_ApplyPositionCommand
          (PlayerRuntimeId playerId,dword payloadDword04,CommandPayloadDword08 payloadDword08,
          CommandPayloadDword0C payloadDword0C);

/* 0x0055FED0 */
void __thandor_preserve_eax
InGamePlayerSelection_SelectArmyRuntimeIndex
          (PlayerRuntimeId playerId,dword payloadDword04,dword payloadDword08,
          RuntimeToken armyRuntimeIndex);

/* 0x0055FF10 */
void __thandor_void_preserve_eax_ecx_edx
InGamePlayerSelection_ApplyTargetPositionCommand
          (PlayerRuntimeId playerId,CommandPayloadDword04 payloadDword04,
          CommandPayloadDword08 payloadDword08,CommandPayloadDword0C payloadDword0C);

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
          (SelectionMarkerIndex selectionIndex,SelectionMarkerCoordinateValue32 valueC,
          SelectionMarkerCoordinateValue32 valueB,SelectionMarkerCoordinateValue32 valueA);

/* 0x0055FFF0 */
void __thandor_void_preserve_eax_ecx_edx
InGameSelection_ApplyType16MarkerCoordinatesVariant2
          (SelectionMarkerIndex selectionIndex,SelectionMarkerCoordinateValue32 valueC,
          SelectionMarkerCoordinateValue32 valueB,SelectionMarkerCoordinateValue32 valueA);

/* 0x00562050 */
void __thandor_void_preserve_eax_ecx_edx
SelectionPlayerRuntime_ReissuePrimarySelectionPosition
          (PlayerRuntimeId playerRuntimeId,dword reserved,Q12 deltaYQ12,Q12 deltaXQ12);

/* 0x00562220 */
void __thandor_void_preserve_eax_ecx_edx
SelectionPlayerRuntime_AdvancePrimarySelectionCycle
          (PlayerRuntimeId playerRuntimeId,dword reserved0,dword reserved1,AngleTurn32 angleDelta);

/* 0x0052CEE0 */
StatusValueEaxCf5 __thandor_eax_cf_preserve_ecx_edx
SelectionInfoPanel_InitResources(SelectionInfoEntitySlots *entitySlots);

/* 0x0052D0F0 */
void __thandor_preserve_eax SelectionInfoPanel_ShutdownResources(void);

/* 0x0052FB20 */
void __thandor_void_preserve_eax_ecx_edx
SelectionPlayerBlocks_RemovePointer(GameEntityRuntime *target);

/* 0x0052FB70 */
WorldPositionEaxEcxEdxCf13 __cdecl SelectionInfoEntitySlots_ComputeAverageWorldPositionRegsCf(void);

/* 0x0052FD60 */
void __thandor_void_preserve_eax_ecx_edx
SelectionPointerArray_RemoveFirstMatch(GameEntityRuntime *target,SelectionPointerArray32 *array);

/* 0x0052FDC0 */
bool __thandor_cf_preserve_eax_ecx_edx SelectionInfo_HasAnyEntryCf(void);

/* 0x0052FDE0 */
bool __thandor_cf_preserve_eax_ecx_edx
SelectionInfo_AllEntriesEmptyOrMatchOwnerCf(FactionRuntimeIndex ownerIndex);

/* 0x0052FE30 */
bool __thandor_cf_preserve_eax_ecx_edx
SelectionInfo_ValidateOwnerType16AndAnyActiveCf(FactionRuntimeIndex ownerIndex);

/* 0x0052FEB0 */
bool __thandor_cf_preserve_eax_ecx_edx SelectionInfo_TestAnyActiveOrSingleClass13Cf(void);

/* 0x0052FF30 */
bool __thandor_cf_preserve_eax_ecx_edx
SelectionInfo_TestPositionCommandAtWorldPointCf
          (Q12 worldXQ12,Q12 worldYQ12,WorldRuntimeContext *inGameRuntime);

/* 0x00530050 */
bool __thandor_cf_preserve_eax_ecx_edx SelectionInfo_TestAllStateField100NonpositiveCf(void);

/* 0x005300A0 */
bool __thandor_cf_preserve_eax_ecx_edx SelectionInfo_TestAnyStateField100NonnegativeCf(void);

/* 0x005300E0 */
GameEntityRuntime * __cdecl SelectionInfo_GetFirstEntry(void);

/* 0x00530100 */
bool __thandor_cf_preserve_eax_ecx_edx SelectionInfo_FindEntryCf(GameEntityRuntime *entry);

/* 0x00530770 */
dword __thandor_eax_preserve_ecx_edx SelectionInfo_CollectAttachmentEffectVariantMask(void);

/* 0x005307C0 */
dword __cdecl SelectionInfo_CollectCapabilityFlags(void);

/* 0x00561000 */
void __thandor_void_preserve_eax_ecx_edx
SelectionPlayerRuntime_ClearTerrainEditSelectionState
          (PlayerRuntimeId playerRuntimeId,dword reservedZero0,dword reservedZero1,
          dword reservedZero2);

/* 0x00571020 */
bool __thandor_cf_preserve_eax_ecx_edx
SelectionPlayerPairList_ContainsPairCf
          (SelectionPlayerPairValue pairValue,SelectionPlayerPairKey pairKey,
          PlayerRuntimeId playerRuntimeId);

/* 0x005302B0 */
void __thandor_void_preserve_eax_ecx_edx
SelectionPointerArray_ApplyPositionCommandVariantB
          (Q12 coordinateA,Q12 coordinateB,SelectionPointerArray32 *selection);

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
          (Q12 coordinateA,Q12 coordinateB,SelectionPointerArray32 *selection);

/* 0x0052D600 */
SelectionPanelAdvanceEaxEdx8 __thandor_eax_edx_cf_preserve_ecx
SelectionPanel_DrawNumberCellAndAdvanceRegs
          (UiPixelCoordinate clipTop,UiPixelCoordinate clipLeft,UiPixelCoordinate clipBottom,
          UiPixelCoordinate clipRight,UiPixelCoordinate drawX,UiPixelCoordinate drawY,
          SelectionPanelNumericValue32 value,SelectionPanelCellIndex cellIndex);

/* 0x0052D6F0 */
SelectionPanelAdvanceEaxEdx8 __thandor_eax_edx_cf_preserve_ecx
SelectionPanel_DrawIconCellAndAdvanceRegs
          (UiPixelCoordinate clipTop,UiPixelCoordinate clipLeft,UiPixelCoordinate clipBottom,
          UiPixelCoordinate clipRight,UiPixelCoordinate drawX,UiPixelCoordinate drawY,
          SelectionPanelCellIndex cellIndex);

/* 0x0052D770 */
SelectionPanelAdvanceEaxEdx8 __thandor_eax_edx_cf_preserve_ecx
SelectionPanel_DrawSteppedMeterCellAndAdvanceRegs
          (UiPixelCoordinate clipTop,UiPixelCoordinate clipLeft,UiPixelCoordinate clipBottom,
          UiPixelCoordinate clipRight,UiPixelCoordinate drawX,UiPixelCoordinate drawY,
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
          (Q12 coordinateA,dword coordinateB,Q12 coordinateC,SelectionPointerArray32 *selection);

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
SelectionPointerArray_ContainsCf(GameEntityRuntime *target,SelectionPointerArray32 *array);

/* 0x005301F0 */
bool __thandor_cf_preserve_eax_ecx_edx
SelectionPointerArray_IsSpatialSpreadTooLargeCf(SelectionPointerArray32 *selection);

/* 0x00530650 */
void __thandor_void_preserve_eax_ecx_edx
SelectionPointerArray_ApplyType16MarkerCoordinates
          (SelectionMarkerLaneMask laneMask,SelectionMarkerCoordinateValue32 valueC,
          SelectionMarkerCoordinateValue32 valueB,SelectionMarkerCoordinateValue32 valueA,
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
          word *commandStream,SelectionPanelCellIndex cellIndex);

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

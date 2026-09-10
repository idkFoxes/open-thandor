#ifndef THANDOR_GAMEPLAY_SELECTION_RUNTIME_H
#define THANDOR_GAMEPLAY_SELECTION_RUNTIME_H

#include <thandor/generated/v523_types.h>
#include <thandor/core/contracts.h>

/* Submodule: gameplay/selection/runtime. */
/* Functions are grouped by semantic ownership; address comments are executable virtual addresses. */

/* 0x0052E350 */
void SelectionPanel_RenderArmyRuntimeMetrics (UiPixelCoordinate clipTop,UiPixelCoordinate clipLeft,UiPixelCoordinate clipBottom, UiPixelCoordinate clipRight,UiPixelCoordinate panelBottom, UiPixelCoordinate panelRight,UiPixelCoordinate panelTop,UiPixelCoordinate panelLeft, ArmyRuntimeSlot *armyRuntime);

/* 0x0055FA20 */
undefined8 __fastcall InGameSelection_RebuildOwnedClass16Selection(undefined4 param_1,undefined4 param_2,int param_3);

/* 0x0055FB30 */
void InGamePlayerSelection_ReplaceWithArmyRuntimeIndex (PlayerRuntimeId playerId,dword payloadDword04,dword payloadDword08, RuntimeToken armyRuntimeIndex);

/* 0x0055FE70 */
void InGamePlayerSelection_ApplyPositionCommandVariantB (PlayerRuntimeId playerId,dword payloadDword04,CommandPayloadDword08 payloadDword08, CommandPayloadDword0C payloadDword0C);

/* 0x0055FEA0 */
void InGamePlayerSelection_ApplyPositionCommand (PlayerRuntimeId playerId,dword payloadDword04,CommandPayloadDword08 payloadDword08, CommandPayloadDword0C payloadDword0C);

/* 0x0055FED0 */
void InGamePlayerSelection_SelectArmyRuntimeIndex (PlayerRuntimeId playerId,dword payloadDword04,dword payloadDword08, RuntimeToken armyRuntimeIndex);

/* 0x0055FF10 */
void InGamePlayerSelection_ApplyTargetPositionCommand (PlayerRuntimeId playerId,CommandPayloadDword04 payloadDword04, CommandPayloadDword08 payloadDword08,CommandPayloadDword0C payloadDword0C);

/* 0x0055FF40 */
void PlayerSelection_ResetMovementPruneAndRecenterEntries (PlayerRuntimeId playerId,CommandPayloadDword04 payloadDword04, CommandPayloadDword08 payloadDword08,CommandPayloadDword0C payloadDword0C);

/* 0x0055FF60 */
void PlayerSelection_ResetMovementAnchorsAndClearFlag200ForEligibleEntries (PlayerRuntimeId playerId,CommandPayloadDword04 payloadDword04, CommandPayloadDword08 payloadDword08,CommandPayloadDword0C payloadDword0C);

/* 0x0055FF80 */
void PlayerSelection_InterruptTargetsAndClearFlag10ForEligibleEntries (PlayerRuntimeId playerId,CommandPayloadDword04 payloadDword04, CommandPayloadDword08 payloadDword08,CommandPayloadDword0C payloadDword0C);

/* 0x0055FFA0 */
void PlayerSelection_ApplyFlags418UnlessBit8ToEligibleEntries (PlayerRuntimeId playerId,CommandPayloadDword04 payloadDword04, CommandPayloadDword08 payloadDword08,CommandPayloadDword0C payloadDword0C);

/* 0x0055FFC0 */
undefined4 InGameSelection_ApplyType16MarkerCoordinatesVariant1 (int param_1,SelectionMarkerCoordinateValue32 param_2, SelectionMarkerCoordinateValue32 param_3,SelectionMarkerCoordinateValue32 param_4);

/* 0x0055FFF0 */
undefined4 InGameSelection_ApplyType16MarkerCoordinatesVariant2 (int param_1,SelectionMarkerCoordinateValue32 param_2, SelectionMarkerCoordinateValue32 param_3,SelectionMarkerCoordinateValue32 param_4);

/* 0x00562050 */
void __fastcall SelectionPlayerRuntime_ReissuePrimarySelectionPosition (undefined4 param_1,undefined4 param_2,PlayerRuntimeId playerRuntimeId);

/* 0x00562220 */
void __fastcall SelectionPlayerRuntime_AdvancePrimarySelectionCycle (undefined4 param_1,undefined4 param_2,PlayerRuntimeId playerRuntimeId);

/* 0x0052CEE0 */
void SelectionInfoPanel_InitResources(SelectionInfoEntitySlots *entitySlots);

/* 0x0052D0F0 */
void SelectionInfoPanel_ShutdownResources(void);

/* 0x0052FB20 */
void SelectionPlayerBlocks_RemovePointer(GameEntityRuntime *target);

/* 0x0052FB70 */
undefined8 __cdecl SelectionInfoEntitySlots_ComputeAverageWorldPositionRegsCf(void);

/* 0x0052FD60 */
undefined4 SelectionPointerArray_RemoveFirstMatch(GameEntityRuntime *target,SelectionPointerArray32 *array);

/* 0x0052FDC0 */
void __cdecl SelectionInfo_HasAnyEntryCf(void);

/* 0x0052FDE0 */
undefined8 SelectionInfo_AllEntriesEmptyOrMatchOwnerCf(FactionRuntimeIndex ownerIndex);

/* 0x0052FE30 */
undefined8 SelectionInfo_ValidateOwnerType16AndAnyActiveCf(FactionRuntimeIndex ownerIndex);

/* 0x0052FEB0 */
void SelectionInfo_TestAnyActiveOrSingleClass13Cf(void);

/* 0x0052FF30 */
void SelectionInfo_TestPositionCommandAtWorldPointCf (Q12 worldXQ12,Q12 worldYQ12,WorldRuntimeContext *inGameRuntime);

/* 0x00530050 */
void SelectionInfo_TestAllStateField100NonpositiveCf(void);

/* 0x005300A0 */
void SelectionInfo_TestAnyStateField100NonnegativeCf(void);

/* 0x005300E0 */
GameEntityRuntime * __cdecl SelectionInfo_GetFirstEntry(void);

/* 0x00530100 */
undefined4 SelectionInfo_FindEntryCf(GameEntityRuntime *entry);

/* 0x00530770 */
dword SelectionInfo_CollectAttachmentEffectVariantMask(void);

/* 0x005307C0 */
dword __cdecl SelectionInfo_CollectCapabilityFlags(void);

/* 0x00561000 */
void __fastcall SelectionPlayerRuntime_ClearTerrainEditSelectionState (undefined4 param_1,undefined4 param_2,PlayerRuntimeId playerRuntimeId);

/* 0x00571020 */
void SelectionPlayerPairList_ContainsPairCf (SelectionPlayerPairValue pairValue,SelectionPlayerPairKey pairKey, PlayerRuntimeId playerRuntimeId);

/* 0x005302B0 */
void SelectionPointerArray_ApplyPositionCommandVariantB (Q12 coordinateA,Q12 coordinateB,SelectionPointerArray32 *selection);

/* 0x00530420 */
void SelectionRuntime_ResetMovementPruneAndRecenterEntries(GameEntityRuntime **selectionEntries);

/* 0x0052FCE0 */
void SelectionPointerArray_AddWorldEntriesMatchingRuntimeIdentity (ArmyRuntimeSlot *sourceArmyRuntime,SelectionPointerArray32 *selection);

/* 0x005303A0 */
void SelectionPointerArray_ApplyPositionCommand (Q12 coordinateA,Q12 coordinateB,SelectionPointerArray32 *selection);

/* 0x0052D600 */
undefined8 SelectionPanel_DrawNumberCellAndAdvanceRegs (UiPixelCoordinate clipTop,UiPixelCoordinate clipLeft,UiPixelCoordinate clipBottom, UiPixelCoordinate clipRight,UiPixelCoordinate drawX,UiPixelCoordinate drawY, SelectionPanelNumericValue32 value,SelectionPanelCellIndex cellIndex);

/* 0x0052D6F0 */
undefined8 SelectionPanel_DrawIconCellAndAdvanceRegs (UiPixelCoordinate clipTop,UiPixelCoordinate clipLeft,UiPixelCoordinate clipBottom, UiPixelCoordinate clipRight,UiPixelCoordinate drawX,UiPixelCoordinate drawY, SelectionPanelCellIndex cellIndex);

/* 0x0052D770 */
undefined8 SelectionPanel_DrawSteppedMeterCellAndAdvanceRegs (UiPixelCoordinate clipTop,UiPixelCoordinate clipLeft,UiPixelCoordinate clipBottom, UiPixelCoordinate clipRight,UiPixelCoordinate drawX,UiPixelCoordinate drawY, UiNumericValue32 maximumValue,UiNumericValue32 currentValue, SelectionPanelCellIndex cellIndex);

/* 0x0052D850 */
void SelectionPanel_DrawProportionalCappedBar (UiPixelCoordinate clipTop,UiPixelCoordinate clipLeft,UiPixelCoordinate clipBottom, UiPixelCoordinate clipRight,undefined4 param_5,UiPixelCoordinate barEndCoordinate, UiPixelCoordinate barStartCoordinate,UiNumericValue32 maximumValue, UiNumericValue32 currentValue,SelectionPanelCellIndex cellIndex);

/* 0x0052DAF0 */
void SelectionPanel_DrawForwardCappedBar (UiPixelCoordinate clipTop,UiPixelCoordinate clipLeft,UiPixelCoordinate clipBottom, UiPixelCoordinate clipRight,undefined4 param_5,UiPixelCoordinate barEndCoordinate, UiPixelCoordinate barStartCoordinate,SelectionPanelCellIndex cellIndex);

/* 0x0052DBC0 */
void SelectionPanel_DrawSolidCappedBar (UiPixelCoordinate clipTop,UiPixelCoordinate clipLeft,UiPixelCoordinate clipBottom, UiPixelCoordinate clipRight,UiPixelCoordinate barEndCoordinate, UiPixelCoordinate barStartCoordinate,undefined4 param_7, SelectionPanelCellIndex cellIndex);

/* 0x0052DFF0 */
void SelectionPanel_DrawSegmentedCappedBar (UiPixelCoordinate clipTop,UiPixelCoordinate clipLeft,UiPixelCoordinate clipBottom, UiPixelCoordinate clipRight,UiPixelCoordinate barEndCoordinate, UiPixelCoordinate barStartCoordinate,undefined4 param_7, SelectionPanelSegmentCount totalSegmentCount, SelectionPanelSegmentCount filledSegmentCount,SelectionPanelCellIndex cellIndex);

/* 0x00530130 */
void SelectionPointerArray_ApplyArmyRuntimeTarget (ArmyRuntimeSlot *targetArmyRuntime,SelectionPointerArray32 *selection);

/* 0x00530190 */
void SelectionPointerArray_ApplyTargetPositionCommand (Q12 coordinateA,dword coordinateB,Q12 coordinateC,SelectionPointerArray32 *selection );

/* 0x00530540 */
void SelectionRuntime_ResetMovementAnchorsAndClearFlag200ForEligibleEntries(int *selectionEntries);

/* 0x005305A0 */
void SelectionRuntime_InterruptTargetsAndClearFlag10ForEligibleEntries(undefined4 *selectionEntries);

/* 0x00530600 */
void SelectionRuntime_ApplyFlags418UnlessBit8ToEligibleEntries(undefined4 *selectionEntries);

/* 0x0052FCA0 */
undefined4 __fastcall SelectionPointerArray_InsertUniqueAndRecenter (undefined4 param_1,undefined4 param_2,GameEntityRuntime *entityRuntime, SelectionPointerArray32 *selection);

/* 0x0052FBF0 */
void SelectionPointerArray_RecenterOffsetsAroundAveragePosition(SelectionPointerArray32 *selection);

/* 0x0052FD90 */
undefined4 SelectionPointerArray_ContainsCf(GameEntityRuntime *target,SelectionPointerArray32 *array);

/* 0x005301F0 */
void SelectionPointerArray_IsSpatialSpreadTooLargeCf(SelectionPointerArray32 *selection);

/* 0x00530650 */
void SelectionPointerArray_ApplyType16MarkerCoordinates (SelectionMarkerLaneMask laneMask,SelectionMarkerCoordinateValue32 valueC, SelectionMarkerCoordinateValue32 valueB,SelectionMarkerCoordinateValue32 valueA, SelectionPointerArray32 *selection);

/* 0x0052FB00 */
undefined4 SelectionPointerArray_Clear32(SelectionPointerArray32 *array);

#endif /* THANDOR_GAMEPLAY_SELECTION_RUNTIME_H */

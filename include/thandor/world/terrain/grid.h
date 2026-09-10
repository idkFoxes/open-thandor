#ifndef THANDOR_WORLD_TERRAIN_GRID_H
#define THANDOR_WORLD_TERRAIN_GRID_H

#include <thandor/generated/v523_types.h>
#include <thandor/core/contracts.h>

/* Submodule: world/terrain/grid. */
/* Functions are grouped by semantic ownership; address comments are executable virtual addresses. */

/* 0x00505930 */
void FieldGrid_ApplyRadialTerrainHeightDeltaAndRefreshSurfaceCf (TerrainMaterialIndex terrainMaterialIndexOrNegativeSentinel, FieldGridRadiusUnits radiusWorldUnits,Q12 terrainHeightDeltaAmplitudeQ12, Q12 centerWorldYQ12,Q12 centerWorldXQ12,FieldGridAsset *fieldGrid);

/* 0x00562330 */
void __fastcall TerrainGrid_RunDirectionalRelaxationPasses (undefined4 param_1,undefined4 param_2,undefined4 param_3,undefined4 param_4, TerrainRelaxationPassCount passCount,TerrainRelaxationMode mode);

/* 0x005610A0 */
void FieldGrid_ApplyPositiveCellDeltas (PlayerRuntimeId playerRuntimeId,Q12 anchorWorldYQ12,Q12 anchorWorldXQ12, PackedFieldGridDeltaXY16 packedDragDeltaXY16);

/* 0x005613C0 */
void FieldGrid_ApplyNegativeCellDeltas (PlayerRuntimeId playerRuntimeId,Q12 anchorWorldYQ12,Q12 anchorWorldXQ12, PackedFieldGridDeltaXY16 packedDragDeltaXY16);

/* 0x00561C10 */
void FieldGrid_RebuildLocalInfluenceState (PlayerRuntimeId playerRuntimeId,FieldGridCommandReservedValue reservedCommandValue, Q12 worldYQ12,Q12 worldXQ12);

/* 0x00505620 */
void FieldGrid_RecomputeInteriorTriangleNormalAngles(FieldGridAsset *fieldGrid);

/* 0x00505700 */
void FieldGrid_RecomputeInteriorDirectionalLighting (AngleTurn32 lightElevationAngle,AngleTurn32 lightAzimuthAngle, FieldGridAsset *fieldGrid);

/* 0x005090E0 */
void FieldGrid_ApplyHeightAtWorldPointAndRefreshNeighborsCf (TerrainHeightBrushDeltaSource heightDeltaSourceValue,Q12 worldZQ12,Q12 worldYQ12, Q12 worldXQ12,FieldGridAsset *fieldGrid);

/* 0x005618A0 */
void FieldGrid_ApplyLocalCellUpdate (PlayerRuntimeId playerRuntimeId,FieldGridTransitionValue transitionValue, Q12 worldYQ12,Q12 worldXQ12);

/* 0x00562390 */
void FieldGrid_ApplyEncodedCellUpdate (PlayerRuntimeId playerRuntimeId,Q12 worldYQ12,Q12 worldXQ12, PackedFieldGridDeltaXY16 packedDragDeltaXY16);

/* 0x005623D0 */
void FieldGrid_ApplyMaskDFFFFFFF (PlayerRuntimeId playerRuntimeId,FieldGridRegionMask setMask,Q12 worldYQ12, Q12 worldXQ12);

/* 0x00562410 */
void FieldGrid_ApplyMaskBFFFFFFF (PlayerRuntimeId playerRuntimeId,FieldGridRegionMask setMask,Q12 worldYQ12, Q12 worldXQ12);

/* 0x00562450 */
void FieldGrid_ApplyCallerMask (PlayerRuntimeId playerRuntimeId,FieldGridMaterialBitIndex materialBitIndex, Q12 worldYQ12,Q12 worldXQ12);

/* 0x004FEA80 */
qword FieldGrid_GetNearestTerrainPoint(Q12 worldY,Q12 worldX,FieldGridAsset *field);

/* 0x004FEB10 */
qword FieldGrid_GetNearestTopSurfacePoint(Q12 worldY,Q12 worldX,FieldGridAsset *field);

/* 0x004FEBA0 */
sdword FieldGrid_GetNearestWaterDelta(Q12 worldY,Q12 worldX,FieldGridAsset *field);

/* 0x004FEC10 */
Q12 FieldGrid_InterpolateTerrainHeight(Q12 worldYQ12,Q12 worldXQ12,FieldGridAsset *fieldGrid);

/* 0x004FED50 */
sdword FieldGrid_InterpolateWaterDelta(Q12 worldY,Q12 worldX,FieldGridAsset *field);

/* 0x004FEE90 */
Q12 FieldGrid_InterpolateWaterSurfaceHeight(Q12 worldYQ12,Q12 worldXQ12,FieldGridAsset *fieldGrid);

/* 0x004FEFF0 */
Q12 FieldGrid_InterpolateTopSurfaceHeight(Q12 worldYQ12,Q12 worldXQ12,FieldGridAsset *fieldGrid);

/* 0x004FF1A0 */
qword FieldGrid_InterpolateTerrainHeightAndNormal(Q12 worldY,Q12 worldX,FieldGridAsset *field);

/* 0x004FF3D0 */
undefined8 FieldGrid_InterpolateTerrainHeightAndTriangle0Normal (Q12 worldYQ12,Q12 worldXQ12,FieldGridAsset *fieldGrid);

/* 0x004FF600 */
undefined8 FieldGrid_InterpolateTerrainHeightAndTriangle1Normal (Q12 worldYQ12,Q12 worldXQ12,FieldGridAsset *fieldGrid);

/* 0x004FF830 */
undefined8 FieldGrid_SampleInterpolatedTerrainHeightAndNormalAnglesCfRegs (GraphicsWorldCoordinateQ12 worldYQ12,GraphicsWorldCoordinateQ12 worldXQ12, FieldGridAsset *fieldGrid);

/* 0x004FFB80 */
void FieldGrid_TestWorldPointBlockedCf (FieldGridByteOffset stateByteOffset,Q12 worldYQ12,Q12 worldXQ12, FieldGridAsset *fieldGrid);

/* 0x00503C90 */
void FieldGrid_InitializeRuntimeCellsAndBoundaryFlags(FieldGridAsset *fieldGrid);

/* 0x00503DB0 */
void FieldGrid_RebuildCellLookupPointers(FieldGridAsset *fieldGrid);

/* 0x00503E20 */
void FieldGrid_ApplyByteClampLookupToCells (FieldGridByteOffset sourceChannelOffset,FieldGridAsset *fieldGrid);

/* 0x00503E80 */
void FieldGrid_ClassifyCellFlagsToRuntimeByte (FieldGridByteOffset cellByteOffset,FieldGridAsset *fieldGrid);

/* 0x00503EE0 */
void __fastcall TerrainDirectionTable_AdvanceAndRebuildVectors(undefined4 param_1,undefined4 param_2);

/* 0x00504B10 */
ulonglong FieldGrid_RaycastTerrainSurfaceDistanceCf (AngleTurn32 elevationAngle,AngleTurn32 azimuthAngle,Q12 rayScaleQ12, Q12 rayOriginZQ12,Q12 rayOriginXQ12,Q12 rayOriginYQ12,FieldGridAsset *fieldGrid);

/* 0x00504CA0 */
ulonglong FieldGrid_RaycastSecondarySurfaceDistanceCf (AngleTurn32 elevationAngle,AngleTurn32 azimuthAngle,Q12 rayScaleQ12, Q12 rayOriginZQ12,Q12 rayOriginXQ12,Q12 rayOriginYQ12,FieldGridAsset *fieldGrid);

/* 0x00504E60 */
ulonglong FieldGrid_RaycastTerrainTrianglesAlongDirection (AngleTurn32 param_1,AngleTurn32 param_2,FixedMathScale32 param_3,Q12 param_4, int param_5,int param_6,int param_7);

/* 0x00505120 */
void FieldGrid_ClearOccupancyMaskBits0To6AllCells(FieldGridAsset *fieldGrid);

/* 0x00505240 */
void FieldGrid_SetOccupancyMaskByteBit0AllCells (FieldGridOccupancyByteIndex occupancyMaskByteIndex,FieldGridAsset *fieldGrid);

/* 0x00505290 */
void FieldGrid_ClearOccupancyMaskByteBit0AllCells (FieldGridOccupancyByteIndex occupancyMaskByteIndex,FieldGridAsset *fieldGrid);

/* 0x00507580 */
undefined8 TerrainGrid_TestProjectedCellMaskBits01Cf (Q12 worldYQ12,Q12 worldXQ12,WorldRuntimeContext *worldRuntime);

/* 0x005092A0 */
void FieldGrid_ClearCellFlag8000AcrossGrid(int param_1);

/* 0x005092E0 */
void FieldGrid_SetAllCellOverlayColors(PackedArgb32 argbColor,FieldGridAsset *fieldGrid);

/* 0x00532B60 */
dword FieldGrid_SaveAssetImageFromRuntimeStateCf(dword fieldGridRuntimeImageCarrier);

/* 0x00561050 */
void FieldGrid_ClearPlayerScratchPlane (PlayerRuntimeId playerRuntimeId,FieldGridCommandReservedValue reservedCommandValue, Q12 reservedWorldYQ12,Q12 reservedWorldXQ12);

/* 0x00561BB0 */
void FieldGrid_ResetLocalInfluenceState (PlayerRuntimeId playerRuntimeId,FieldGridCommandReservedValue reservedCommandValue, Q12 reservedWorldYQ12,Q12 reservedWorldXQ12);

/* 0x00571EC0 */
void FieldGrid_ApplyEncodedUpdateCore (FieldGridHeightDeltaUnits heightDeltaUnits,Q12 worldYQ12,Q12 worldXQ12, FieldGridAsset *fieldGrid);

/* 0x005058A0 */
void FieldGridCell_ApplyRadialTerrainHeightDeltaAndMaterial (TerrainMaterialIndex terrainMaterialIndexOrNegativeSentinel, FieldGridRadiusUnits radiusWorldUnits,Q12 terrainHeightDeltaAmplitudeQ12, Q12 centerWorldYQ12,Q12 centerWorldXQ12,FieldGridCell *cell);

/* 0x00505AA0 */
undefined8 TerrainGrid_RelaxNeighborHeightsForwardWithSignGate(FieldGridAsset *fieldGrid);

/* 0x00505BE0 */
undefined8 TerrainGrid_RelaxNeighborHeightsReverseWithSignGate(FieldGridAsset *fieldGrid);

/* 0x00505D30 */
void TerrainGrid_RelaxNeighborHeightsForward(FieldGridAsset *fieldGrid);

/* 0x00505E60 */
void TerrainGrid_RelaxNeighborHeightsReverse(FieldGridAsset *fieldGrid);

/* 0x00571090 */
void FieldGrid_ProcessHorizontalSpan (Q12 sourceWorldYQ12,Q12 sourceWorldXQ12,FieldGridHeightDeltaUnits heightDeltaUnits, FieldGridRadiusUnits radiusUnits,Q12 centerWorldYQ12,Q12 centerWorldXQ12, FieldGridAccumulatorValue *accumulatorPlane,FieldGridAsset *fieldGrid);

/* 0x00571250 */
void FieldGrid_ProcessVerticalSpan (FieldGridHeightDeltaUnits heightDeltaUnits,FieldGridRadiusUnits radiusUnits, Q12 centerWorldYQ12,Q12 centerWorldXQ12,FieldGridAccumulatorValue *accumulatorPlane, FieldGridAsset *fieldGrid);

/* 0x005713E0 */
void FieldGrid_ApplySingleCellTransition (FieldGridTransitionValue transitionValue,Q12 worldYQ12,Q12 worldXQ12, FieldGridAsset *fieldGrid);

/* 0x00571860 */
void FieldGrid_ApplyRectangularTransition(Q12 worldYQ12,Q12 worldXQ12,FieldGridAsset *fieldGrid);

/* 0x004FEA50 */
qword FieldGrid_WorldToGridQ12(Q12 worldY,Q12 worldX);

/* 0x00571FE0 */
void FieldGrid_ApplyMaskedRegionCore (FieldGridRegionMask preserveMask,FieldGridRegionMask setMask,Q12 worldYQ12, Q12 worldXQ12,FieldGridAsset *fieldGrid);

/* 0x005052E0 */
void FieldGridCell_RecomputeTriangleNormalAngles (FieldGridRowStrideBytes rowStrideBytes,FieldGridCell *cell);

/* 0x00505690 */
void FieldGridCell_ComputeDirectionalLightColor(FieldGridCell *cell);

#endif /* THANDOR_WORLD_TERRAIN_GRID_H */

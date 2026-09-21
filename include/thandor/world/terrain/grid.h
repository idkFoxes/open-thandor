/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/include/thandor/world/terrain/grid.h
 * Reverse engineering by idkFoxes 2026
 */

#ifndef THANDOR_WORLD_TERRAIN_GRID_H
#define THANDOR_WORLD_TERRAIN_GRID_H

#include <thandor/generated/types.h>
#include <thandor/core/contracts.h>

/* Submodule: world/terrain/grid. */
/* Functions are grouped by semantic ownership; address comments are executable virtual addresses. */

/* 0x00505930 */
void __thandor_void_preserve_eax_ecx_edx
FieldGrid_ApplyRadialTerrainHeightDeltaAndRefreshSurfaceCf
          (TerrainMaterialIndex terrainMaterialIndexOrNegativeSentinel,
          FieldGridRadiusUnits radiusWorldUnits,Q12 terrainHeightDeltaAmplitudeQ12,
          Q12 centerWorldYQ12,Q12 centerWorldXQ12,FieldGridAsset *fieldGrid);

/* 0x00562330 */
void __thandor_void_preserve_eax_ecx_edx
TerrainGrid_RunDirectionalRelaxationPasses
          (FrontendPlayerRuntimeId playerRuntimeId,dword reservedZero,
          TerrainRelaxationPassCount passCount,TerrainRelaxationMode mode);

/* 0x005610A0 */
void __thandor_void_preserve_eax_ecx_edx
FieldGrid_ApplyPositiveCellDeltas
          (PlayerRuntimeId playerRuntimeId,Q12 anchorWorldYQ12,Q12 anchorWorldXQ12,
          PackedFieldGridDeltaXY16 packedDragDeltaXY16);

/* 0x005613C0 */
void __thandor_void_preserve_eax_ecx_edx
FieldGrid_ApplyNegativeCellDeltas
          (PlayerRuntimeId playerRuntimeId,Q12 anchorWorldYQ12,Q12 anchorWorldXQ12,
          PackedFieldGridDeltaXY16 packedDragDeltaXY16);

/* 0x00561C10 */
void __thandor_void_preserve_eax_ecx_edx
FieldGrid_RebuildLocalInfluenceState
          (PlayerRuntimeId playerRuntimeId,FieldGridCommandReservedValue reservedCommandValue,
          Q12 worldYQ12,Q12 worldXQ12);

/* 0x00505620 */
void __thandor_void_preserve_eax_ecx_edx
FieldGrid_RecomputeInteriorTriangleNormalAngles(FieldGridAsset *fieldGrid);

/* 0x00505700 */
void __thandor_void_preserve_eax_ecx_edx
FieldGrid_RecomputeInteriorDirectionalLighting
          (AngleTurn32 lightElevationAngle,AngleTurn32 lightAzimuthAngle,FieldGridAsset *fieldGrid);

/* 0x005090E0 */
void __thandor_void_preserve_eax_ecx_edx
FieldGrid_ApplyHeightAtWorldPointAndRefreshNeighborsCf
          (TerrainHeightBrushDeltaSource heightDeltaSourceValue,Q12 worldZQ12,Q12 worldYQ12,
          Q12 worldXQ12,FieldGridAsset *fieldGrid);

/* 0x005618A0 */
void __thandor_void_preserve_eax_ecx_edx
FieldGrid_ApplyLocalCellUpdate
          (PlayerRuntimeId playerRuntimeId,FieldGridTransitionValue transitionValue,Q12 worldYQ12,
          Q12 worldXQ12);

/* 0x00562390 */
void __thandor_void_preserve_eax_ecx_edx
FieldGrid_ApplyEncodedCellUpdate
          (PlayerRuntimeId playerRuntimeId,Q12 worldYQ12,Q12 worldXQ12,
          PackedFieldGridDeltaXY16 packedDragDeltaXY16);

/* 0x005623D0 */
void __thandor_preserve_eax
FieldGrid_ApplyMaskDFFFFFFF
          (PlayerRuntimeId playerRuntimeId,FieldGridRegionMask setMask,Q12 worldYQ12,Q12 worldXQ12);

/* 0x00562410 */
void __thandor_preserve_eax
FieldGrid_ApplyMaskBFFFFFFF
          (PlayerRuntimeId playerRuntimeId,FieldGridRegionMask setMask,Q12 worldYQ12,Q12 worldXQ12);

/* 0x00562450 */
void __thandor_void_preserve_eax_ecx_edx
FieldGrid_ApplyCallerMask
          (PlayerRuntimeId playerRuntimeId,FieldGridMaterialBitIndex materialBitIndex,Q12 worldYQ12,
          Q12 worldXQ12);

/* 0x004FEA80 */
FieldGridNearestPointRegsCf13
FieldGrid_GetNearestTerrainPoint(Q12 worldY,Q12 worldX,FieldGridAsset *field);

/* 0x004FEB10 */
FieldGridSurfacePointEaxEcxEdxCf13
FieldGrid_GetNearestTopSurfacePoint(Q12 worldY,Q12 worldX,FieldGridAsset *field);

/* 0x004FEBA0 */
sdword __thandor_eax_preserve_ecx_edx
FieldGrid_GetNearestWaterDelta(Q12 worldY,Q12 worldX,FieldGridAsset *field);

/* 0x004FEC10 */
FieldGridHeightEaxCf5 __thandor_eax_cf_preserve_ecx_edx
FieldGrid_InterpolateTerrainHeight(Q12 worldYQ12,Q12 worldXQ12,FieldGridAsset *fieldGrid);

/* 0x004FED50 */
sdword __thandor_eax_preserve_ecx_edx
FieldGrid_InterpolateWaterDelta(Q12 worldY,Q12 worldX,FieldGridAsset *field);

/* 0x004FEE90 */
FieldGridHeightEaxCf5 __thandor_eax_cf_preserve_ecx_edx
FieldGrid_InterpolateWaterSurfaceHeight(Q12 worldYQ12,Q12 worldXQ12,FieldGridAsset *fieldGrid);

/* 0x004FEFF0 */
FieldGridHeightEaxCf5 __thandor_eax_cf_preserve_ecx_edx
FieldGrid_InterpolateTopSurfaceHeight(Q12 worldYQ12,Q12 worldXQ12,FieldGridAsset *fieldGrid);

/* 0x004FF1A0 */
FieldGridHeightNormalEaxEdxCf9 __thandor_eax_edx_cf_preserve_ecx
FieldGrid_InterpolateTerrainHeightAndNormal(Q12 worldY,Q12 worldX,FieldGridAsset *field);

/* 0x004FF3D0 */
FieldGridHeightNormalEaxEdxCf9 __thandor_eax_edx_cf_preserve_ecx
FieldGrid_InterpolateTerrainHeightAndTriangle0Normal
          (Q12 worldYQ12,Q12 worldXQ12,FieldGridAsset *fieldGrid);

/* 0x004FF600 */
FieldGridHeightNormalEaxEdxCf9 __thandor_eax_edx_cf_preserve_ecx
FieldGrid_InterpolateTerrainHeightAndTriangle1Normal
          (Q12 worldYQ12,Q12 worldXQ12,FieldGridAsset *fieldGrid);

/* 0x004FF830 */
FieldGridHeightNormalEaxEdxCf9 __thandor_eax_edx_cf_preserve_ecx
FieldGrid_SampleInterpolatedTerrainHeightAndNormalAnglesCfRegs
          (GraphicsWorldCoordinateQ12 worldYQ12,GraphicsWorldCoordinateQ12 worldXQ12,
          FieldGridAsset *fieldGrid);

/* 0x004FFB80 */
bool __thandor_cf_preserve_eax_ecx_edx
FieldGrid_TestWorldPointBlockedCf
          (FieldGridByteOffset stateByteOffset,Q12 worldYQ12,Q12 worldXQ12,FieldGridAsset *fieldGrid
          );

/* 0x00503C90 */
void __thandor_void_preserve_eax_ecx_edx
FieldGrid_InitializeRuntimeCellsAndBoundaryFlags(FieldGridAsset *fieldGrid);

/* 0x00503DB0 */
void __thandor_void_preserve_eax_ecx_edx
FieldGrid_RebuildCellLookupPointers(FieldGridAsset *fieldGrid);

/* 0x00503E20 */
void __thandor_void_preserve_eax_ecx_edx
FieldGrid_ApplyByteClampLookupToCells
          (FieldGridByteOffset sourceChannelOffset,FieldGridAsset *fieldGrid);

/* 0x00503E80 */
void __thandor_void_preserve_eax_ecx_edx
FieldGrid_ClassifyCellFlagsToRuntimeByte
          (FieldGridByteOffset cellByteOffset,FieldGridAsset *fieldGrid);

/* 0x00503EE0 */
void __thandor_void_preserve_eax_ecx_edx TerrainDirectionTable_AdvanceAndRebuildVectors(void);

/* 0x00504B10 */
FieldGridRaycastEaxEdxCf9 __thandor_eax_edx_cf_preserve_ecx
FieldGrid_RaycastTerrainSurfaceDistanceCf
          (AngleTurn32 elevationAngle,AngleTurn32 azimuthAngle,Q12 rayScaleQ12,Q12 rayOriginZQ12,
          Q12 rayOriginXQ12,Q12 rayOriginYQ12,FieldGridAsset *fieldGrid);

/* 0x00504CA0 */
FieldGridRaycastEaxEdxCf9 __thandor_eax_edx_cf_preserve_ecx
FieldGrid_RaycastSecondarySurfaceDistanceCf
          (AngleTurn32 elevationAngle,AngleTurn32 azimuthAngle,Q12 rayScaleQ12,Q12 rayOriginZQ12,
          Q12 rayOriginXQ12,Q12 rayOriginYQ12,FieldGridAsset *fieldGrid);

/* 0x00504E60 */
FieldGridRaycastEaxEdxCf9 __thandor_eax_edx_cf_preserve_ecx
FieldGrid_RaycastTerrainTrianglesAlongDirection
          (AngleTurn32 elevationAngle,AngleTurn32 azimuthAngle,FixedMathScale32 rayScaleQ12,
          Q12 rayOriginZQ12,Q12 rayOriginXQ12,Q12 rayOriginYQ12,FieldGridAsset *fieldGrid);

/* 0x00505120 */
void FieldGrid_ClearOccupancyMaskBits0To6AllCells(FieldGridAsset *fieldGrid);

/* 0x00505240 */
void __thandor_void_preserve_eax_ecx_edx
FieldGrid_SetOccupancyMaskByteBit0AllCells
          (FieldGridOccupancyByteIndex occupancyMaskByteIndex,FieldGridAsset *fieldGrid);

/* 0x00505290 */
void __thandor_void_preserve_eax_ecx_edx
FieldGrid_ClearOccupancyMaskByteBit0AllCells
          (FieldGridOccupancyByteIndex occupancyMaskByteIndex,FieldGridAsset *fieldGrid);

/* 0x00507580 */
bool __thandor_cf_preserve_eax_ecx_edx
TerrainGrid_TestProjectedCellMaskBits01Cf
          (Q12 worldYQ12,Q12 worldXQ12,WorldRuntimeContext *worldRuntime);

/* 0x005092A0 */
void FieldGrid_ClearCellFlag8000AcrossGrid(FieldGridAsset *fieldGrid);

/* 0x005092E0 */
void __thandor_void_preserve_eax_ecx
FieldGrid_SetAllCellOverlayColors(PackedArgb32 argbColor,FieldGridAsset *fieldGrid);

/* 0x00532B60 */
StatusValueEaxCf5 __thandor_eax_cf_preserve_ecx_edx
FieldGrid_SaveAssetImageFromRuntimeStateCf(dword *sourceImageDwords);

/* 0x00561050 */
void __thandor_void_preserve_eax_ecx_edx
FieldGrid_ClearPlayerScratchPlane
          (PlayerRuntimeId playerRuntimeId,FieldGridCommandReservedValue reservedCommandValue,
          Q12 reservedWorldYQ12,Q12 reservedWorldXQ12);

/* 0x00561BB0 */
void __thandor_void_preserve_eax_ecx_edx
FieldGrid_ResetLocalInfluenceState
          (PlayerRuntimeId playerRuntimeId,FieldGridCommandReservedValue reservedCommandValue,
          Q12 reservedWorldYQ12,Q12 reservedWorldXQ12);

/* 0x00571EC0 */
void __thandor_void_preserve_eax_ecx_edx
FieldGrid_ApplyEncodedUpdateCore
          (FieldGridHeightDeltaUnits heightDeltaUnits,Q12 worldYQ12,Q12 worldXQ12,
          FieldGridAsset *fieldGrid);

/* 0x005058A0 */
void __thandor_void_preserve_eax_ecx_edx
FieldGridCell_ApplyRadialTerrainHeightDeltaAndMaterial
          (TerrainMaterialIndex terrainMaterialIndexOrNegativeSentinel,
          FieldGridRadiusUnits radiusWorldUnits,Q12 terrainHeightDeltaAmplitudeQ12,
          Q12 centerWorldYQ12,Q12 centerWorldXQ12,FieldGridCell *cell);

/* 0x00505AA0 */
void __thandor_void_preserve_eax_ecx_edx
TerrainGrid_RelaxNeighborHeightsForwardWithSignGate(FieldGridAsset *fieldGrid);

/* 0x00505BE0 */
void __thandor_void_preserve_eax_ecx_edx
TerrainGrid_RelaxNeighborHeightsReverseWithSignGate(FieldGridAsset *fieldGrid);

/* 0x00505D30 */
void __thandor_void_preserve_eax_ecx_edx
TerrainGrid_RelaxNeighborHeightsForward(FieldGridAsset *fieldGrid);

/* 0x00505E60 */
void __thandor_void_preserve_eax_ecx_edx
TerrainGrid_RelaxNeighborHeightsReverse(FieldGridAsset *fieldGrid);

/* 0x00571090 */
void __thandor_void_preserve_eax_ecx_edx
FieldGrid_ProcessHorizontalSpan
          (Q12 sourceWorldYQ12,Q12 sourceWorldXQ12,FieldGridHeightDeltaUnits heightDeltaUnits,
          FieldGridRadiusUnits radiusUnits,Q12 centerWorldYQ12,Q12 centerWorldXQ12,
          FieldGridAccumulatorValue *accumulatorPlane,FieldGridAsset *fieldGrid);

/* 0x00571250 */
void __thandor_void_preserve_eax_ecx_edx
FieldGrid_ProcessVerticalSpan
          (FieldGridHeightDeltaUnits heightDeltaUnits,FieldGridRadiusUnits radiusUnits,
          Q12 centerWorldYQ12,Q12 centerWorldXQ12,FieldGridAccumulatorValue *accumulatorPlane,
          FieldGridAsset *fieldGrid);

/* 0x005713E0 */
void __thandor_void_preserve_eax_ecx_edx
FieldGrid_ApplySingleCellTransition
          (FieldGridTransitionValue transitionValue,Q12 worldYQ12,Q12 worldXQ12,
          FieldGridAsset *fieldGrid);

/* 0x00571860 */
void __thandor_void_preserve_eax_ecx_edx
FieldGrid_ApplyRectangularTransition(Q12 worldYQ12,Q12 worldXQ12,FieldGridAsset *fieldGrid);

/* 0x004FEA50 */
FieldGridCoordinatesEaxEdx8 __thandor_eax_edx_cf_preserve_ecx_mm0
FieldGrid_WorldToGridQ12(Q12 worldY,Q12 worldX);

/* 0x00571FE0 */
void __thandor_void_preserve_eax_ecx_edx
FieldGrid_ApplyMaskedRegionCore
          (FieldGridRegionMask preserveMask,FieldGridRegionMask setMask,Q12 worldYQ12,Q12 worldXQ12,
          FieldGridAsset *fieldGrid);

/* 0x005052E0 */
void __thandor_void_preserve_eax_ecx_edx
FieldGridCell_RecomputeTriangleNormalAngles
          (FieldGridRowStrideBytes rowStrideBytes,FieldGridCell *cell);

/* 0x00505690 */
void __thandor_void_preserve_eax_ecx_edx
FieldGridCell_ComputeDirectionalLightColor(FieldGridCell *cell);

#endif /* THANDOR_WORLD_TERRAIN_GRID_H */

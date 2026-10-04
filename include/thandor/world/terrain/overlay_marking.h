/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/include/thandor/world/terrain/overlay_marking.h
 * Reverse engineering by idkFoxes 2026
 */

#ifndef THANDOR_WORLD_TERRAIN_OVERLAY_MARKING_H
#define THANDOR_WORLD_TERRAIN_OVERLAY_MARKING_H

#include <thandor/generated/types.h>
#include <thandor/core/contracts.h>

Bool8 FieldGridTerrainOverlayVariantA_ApplyAroundWorldPoint
          (FieldCellFlagMask cellFlagMask,TerrainOverlayCellRuntimeValue cellValue,
          FieldGridRadiusUnits radiusWorldUnits,Q12 worldYQ12,Q12 worldXQ12,
          FieldGridAsset *fieldGrid);

Bool8 FieldGridTerrainOverlayVariantB_ApplyAroundWorldPoint
          (FieldCellFlagMask cellFlagMask,TerrainOverlayCellRuntimeValue cellValue,
          FieldGridRadiusUnits radiusWorldUnits,Q12 worldYQ12,Q12 worldXQ12,
          FieldGridAsset *fieldGrid);

void FieldGridTerrainOverlayVariantA_ApplyWedge0(TerrainDirectionalScanStep scanStep,FieldGridCell *fieldCell);

void FieldGridTerrainOverlayVariantA_ApplyWedge1(TerrainDirectionalScanStep scanStep,FieldGridCell *fieldCell);

void FieldGridTerrainOverlayVariantA_ApplyWedge2(TerrainDirectionalScanStep scanStep,FieldGridCell *fieldCell);

void FieldGridTerrainOverlayVariantA_ApplyWedge3(TerrainDirectionalScanStep scanStep,FieldGridCell *fieldCell);

void FieldGridTerrainOverlayVariantA_ApplyWedge4(TerrainDirectionalScanStep scanStep,FieldGridCell *fieldCell);

void FieldGridTerrainOverlayVariantA_ApplyWedge5(TerrainDirectionalScanStep scanStep,FieldGridCell *fieldCell);

void FieldGridTerrainOverlayVariantB_ApplyWedge0(TerrainDirectionalScanStep scanStep,FieldGridCell *fieldCell);

void FieldGridTerrainOverlayVariantB_ApplyWedge1(TerrainDirectionalScanStep scanStep,FieldGridCell *fieldCell);

void FieldGridTerrainOverlayVariantB_ApplyWedge2(TerrainDirectionalScanStep scanStep,FieldGridCell *fieldCell);

void FieldGridTerrainOverlayVariantB_ApplyWedge3(TerrainDirectionalScanStep scanStep,FieldGridCell *fieldCell);

void FieldGridTerrainOverlayVariantB_ApplyWedge4(TerrainDirectionalScanStep scanStep,FieldGridCell *fieldCell);

void FieldGridTerrainOverlayVariantB_ApplyWedge5(TerrainDirectionalScanStep scanStep,FieldGridCell *fieldCell);

void FieldGridTerrainOverlayVariantA_ApplyDirection0(TerrainDirectionalScanStep scanStep,FieldGridCell *fieldCell);

void FieldGridTerrainOverlayVariantA_ApplyDirection1(TerrainDirectionalScanStep scanStep,FieldGridCell *fieldCell);

void FieldGridTerrainOverlayVariantA_ApplyDirection2(TerrainDirectionalScanStep scanStep,FieldGridCell *fieldCell);

void FieldGridTerrainOverlayVariantA_ApplyDirection3(TerrainDirectionalScanStep scanStep,FieldGridCell *fieldCell);

void FieldGridTerrainOverlayVariantA_ApplyDirection4(TerrainDirectionalScanStep scanStep,FieldGridCell *fieldCell);

void FieldGridTerrainOverlayVariantA_ApplyDirection5(TerrainDirectionalScanStep scanStep,FieldGridCell *fieldCell);

void FieldGridTerrainOverlayVariantB_ApplyDirection0(TerrainDirectionalScanStep scanStep,FieldGridCell *fieldCell);

void FieldGridTerrainOverlayVariantB_ApplyDirection1(TerrainDirectionalScanStep scanStep,FieldGridCell *fieldCell);

void FieldGridTerrainOverlayVariantB_ApplyDirection2(TerrainDirectionalScanStep scanStep,FieldGridCell *fieldCell);

void FieldGridTerrainOverlayVariantB_ApplyDirection3(TerrainDirectionalScanStep scanStep,FieldGridCell *fieldCell);

void FieldGridTerrainOverlayVariantB_ApplyDirection4(TerrainDirectionalScanStep scanStep,FieldGridCell *fieldCell);

void FieldGridTerrainOverlayVariantB_ApplyDirection5(TerrainDirectionalScanStep scanStep,FieldGridCell *fieldCell);

#endif /* THANDOR_WORLD_TERRAIN_OVERLAY_MARKING_H */

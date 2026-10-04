/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/include/thandor/world/terrain/field_lighting.h
 * Reverse engineering by idkFoxes 2026
 */

#ifndef THANDOR_WORLD_TERRAIN_FIELD_LIGHTING_H
#define THANDOR_WORLD_TERRAIN_FIELD_LIGHTING_H

#include <thandor/generated/types.h>
#include <thandor/core/contracts.h>

extern TerrainDirectionRecord g_TerrainDirectionRecordTable256[256];

void FieldGrid_RecomputeInteriorTriangleNormalAngles(FieldGridAsset *fieldGrid);

void FieldGrid_RecomputeInteriorDirectionalLighting
          (AngleTurn32 lightElevationAngle,AngleTurn32 lightAzimuthAngle,FieldGridAsset *fieldGrid);

void TerrainDirectionTable_AdvanceAndRebuildVectors(void);

void FieldGridCell_RecomputeTriangleNormalAngles(FieldGridRowStrideBytes rowStrideBytes,FieldGridCell *cell);

void FieldGridCell_ComputeDirectionalLightColor(FieldGridCell *cell);

#endif /* THANDOR_WORLD_TERRAIN_FIELD_LIGHTING_H */

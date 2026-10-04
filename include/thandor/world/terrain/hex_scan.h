/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/include/thandor/world/terrain/hex_scan.h
 * Reverse engineering by idkFoxes 2026
 */

#ifndef THANDOR_WORLD_TERRAIN_HEX_SCAN_H
#define THANDOR_WORLD_TERRAIN_HEX_SCAN_H

#include <thandor/generated/types.h>
#include <thandor/core/contracts.h>

/* Hexagonal radius scans (TerrainHeightDelta_*, TerrainHeightBand_*, TerrainAuxHeightThreshold_*): the scan step
   counter grows by 4 per cell along a straight direction and by 7 (about 4 * sqrt(3)) per step along a wedge's
   diagonal, and a scan stops at g_TerrainScanStepLimit (radius / 0x240, clamped to 1..255). */
#define TERRAIN_SCAN_STEP_STRAIGHT 4
#define TERRAIN_SCAN_STEP_DIAGONAL 7
#define TERRAIN_SCAN_RADIUS_PER_STEP 0x240 /* world units per step: one cell (0x900) is 4 steps */
#define TERRAIN_SCAN_STEP_LIMIT_MAX 255

extern uint32_t g_TerrainScanRowStrideBytes;

extern uint32_t g_TerrainScanStepLimit;

extern TerrainScanSelectorUnion g_TerrainScanSharedSelectorValue;

extern uint32_t g_TerrainScanReferenceHeight;

void TerrainProjectedScan_SetStepLimitFromRadius(FieldGridRadiusUnits radiusWorldUnits);

void TerrainProjectedScan_SelectNearestGridVertex
          (FieldGridCoordinates gridCoordinates,uint32_t *gridRowOut,uint32_t *gridColumnOut);

#endif /* THANDOR_WORLD_TERRAIN_HEX_SCAN_H */

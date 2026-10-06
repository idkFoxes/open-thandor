/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/include/thandor/world/terrain/hex_scan.h
 * Reverse engineering by idkFoxes 2026
 */

#ifndef THANDOR_WORLD_TERRAIN_HEX_SCAN_H
#define THANDOR_WORLD_TERRAIN_HEX_SCAN_H

#include <thandor/world/terrain/types.h>
#include <thandor/world/terrain/grid.h>
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

/* The hexagon walk shared by the radius scans (one template instead of a function per direction and sector).
   Directions around a cell C (W = one grid row, g_TerrainScanRowStrideBytes bytes):
     0: C+1   1: C+1-W   2: C-W   3: C-1   4: C-1+W   5: C+W
   A cell policy says what happens to each visited cell (TerrainHexScan_MarkPolicy: visit and go on;
   TerrainHexScan_TestPolicy: the walk fails at the first failing cell). Every walk ends at a map-edge cell
   (a failure for a test policy); there are no other bounds checks, the edge ring stops the walk. */

/* The neighbour of cell in direction Direction (0..5). The row moves are signed 32-bit byte offsets. */
template <int Direction>
inline FieldGridCell *TerrainHexScan_Neighbor(FieldGridCell *cell)
{
  static_assert(0 <= Direction && Direction < 6, "hexagon direction 0..5");
  if constexpr (Direction == 0) {
    return cell + 1;
  } else if constexpr (Direction == 1) {
    return FieldGridCell_AtByteOffset(cell + 1,-g_TerrainScanRowStrideBytes);
  } else if constexpr (Direction == 2) {
    return FieldGridCell_AtByteOffset(cell,-g_TerrainScanRowStrideBytes);
  } else if constexpr (Direction == 3) {
    return cell - 1;
  } else if constexpr (Direction == 4) {
    return FieldGridCell_AtByteOffset(cell - 1,g_TerrainScanRowStrideBytes);
  } else {
    return FieldGridCell_AtByteOffset(cell,g_TerrainScanRowStrideBytes);
  }
}

/* What a walk does with each cell: IsTest false: visit(cell) is called for every non-edge cell and an edge
   cell only ends the current walk; IsTest true: visit(cell) returns true when the cell fails, and an edge cell
   or a failing cell ends the whole scan with true. The policy is passed by value, so a leg works on a copy. */
template <bool IsTestPolicy, class VisitFn>
struct TerrainHexScan_CellPolicy {
  static constexpr bool IsTest = IsTestPolicy;
  VisitFn visit;
};

template <class VisitFn>
inline TerrainHexScan_CellPolicy<false,VisitFn> TerrainHexScan_MarkPolicy(VisitFn visit)
{
  return TerrainHexScan_CellPolicy<false,VisitFn>{visit};
}

template <class VisitFn>
inline TerrainHexScan_CellPolicy<true,VisitFn> TerrainHexScan_TestPolicy(VisitFn test)
{
  return TerrainHexScan_CellPolicy<true,VisitFn>{test};
}

/* One cell of a walk: true when the walk ends here (an edge cell, or a failing cell of a test policy).
   A policy whose visit also takes the cell's scan step (visit(cell, scanStep), e.g. the line-of-sight policy
   that scales heights by distance) gets it: the leg cell's own step, s for a spine cell, s+4 for the cell
   between two spine cells. */
template <class Policy>
inline bool TerrainHexScan_EndsAt(FieldGridCell *cell,TerrainDirectionalScanStep scanStep,Policy &policy)
{
  if ((cell->flagsAndMaterial & FIELD_CELL_GRID_EDGE_MASK) != 0) {
    return true;
  }
  if constexpr (requires { policy.visit(cell,scanStep); }) {
    if constexpr (Policy::IsTest) {
      return policy.visit(cell,scanStep);
    } else {
      policy.visit(cell,scanStep);
      return false;
    }
  } else if constexpr (Policy::IsTest) {
    return policy.visit(cell);
  } else {
    policy.visit(cell);
    return false;
  }
}

/* Straight leg along Direction from cell: TERRAIN_SCAN_STEP_STRAIGHT steps per cell until the step limit.
   Returns true only for a test policy that failed. */
template <int Direction,class Policy>
inline Bool8 TerrainHexScan_Leg(TerrainDirectionalScanStep scanStep,FieldGridCell *cell,Policy policy)
{
  while (scanStep < g_TerrainScanStepLimit) {
    if (TerrainHexScan_EndsAt(cell,scanStep,policy)) {
      return Policy::IsTest;
    }
    scanStep = scanStep + TERRAIN_SCAN_STEP_STRAIGHT;
    cell = TerrainHexScan_Neighbor<Direction>(cell);
  }
  return false;
}

/* Sector Sector (0..5) between directions a = Sector and b = Sector + 1 (mod 6), from its first cell C+Da of
   the centre C: each spine cell S is visited, a leg runs along a from S+Da (step s+4); unless the limit is
   reached at s+4, the cell between S+Db is visited, s grows by TERRAIN_SCAN_STEP_DIAGONAL (7, about
   4 * sqrt(3)), a leg runs along b from S+2Db and the spine continues at S+Db+Da. The radius is therefore only
   approximate (4 per straight cell, 7 per spine cell). Returns true only for a test policy that failed. */
template <int Sector,class Policy>
inline Bool8 TerrainHexScan_Sector(TerrainDirectionalScanStep scanStep,FieldGridCell *cell,Policy policy)
{
  constexpr int DirectionA = Sector;
  constexpr int DirectionB = (Sector + 1) % 6;
  FieldGridCell *betweenCell;

  while (scanStep < g_TerrainScanStepLimit) {
    if (TerrainHexScan_EndsAt(cell,scanStep,policy)) {
      return Policy::IsTest;
    }
    if (TerrainHexScan_Leg<DirectionA>(scanStep + TERRAIN_SCAN_STEP_STRAIGHT,
                                       TerrainHexScan_Neighbor<DirectionA>(cell),policy)) {
      return true;
    }
    if (g_TerrainScanStepLimit <= scanStep + TERRAIN_SCAN_STEP_STRAIGHT) {
      return false;
    }
    betweenCell = TerrainHexScan_Neighbor<DirectionB>(cell);
    if (TerrainHexScan_EndsAt(betweenCell,scanStep + TERRAIN_SCAN_STEP_STRAIGHT,policy)) {
      return Policy::IsTest;
    }
    scanStep = scanStep + TERRAIN_SCAN_STEP_DIAGONAL;
    if (TerrainHexScan_Leg<DirectionB>(scanStep,TerrainHexScan_Neighbor<DirectionB>(betweenCell),policy)) {
      return true;
    }
    cell = TerrainHexScan_Neighbor<DirectionA>(betweenCell);
  }
  return false;
}

/* The six sectors around centerCell (not visited itself), in the order 0..5, each from scan step 0; for a test
   policy the first failing sector ends the scan (left-to-right ||). */
template <class Policy>
inline Bool8 TerrainHexScan_AllSectors(FieldGridCell *centerCell,Policy policy)
{
  return TerrainHexScan_Sector<0>(0,TerrainHexScan_Neighbor<0>(centerCell),policy) ||
         TerrainHexScan_Sector<1>(0,TerrainHexScan_Neighbor<1>(centerCell),policy) ||
         TerrainHexScan_Sector<2>(0,TerrainHexScan_Neighbor<2>(centerCell),policy) ||
         TerrainHexScan_Sector<3>(0,TerrainHexScan_Neighbor<3>(centerCell),policy) ||
         TerrainHexScan_Sector<4>(0,TerrainHexScan_Neighbor<4>(centerCell),policy) ||
         TerrainHexScan_Sector<5>(0,TerrainHexScan_Neighbor<5>(centerCell),policy);
}

#endif /* THANDOR_WORLD_TERRAIN_HEX_SCAN_H */

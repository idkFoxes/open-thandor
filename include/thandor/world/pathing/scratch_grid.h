/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/include/thandor/world/pathing/scratch_grid.h
 * Reverse engineering by idkFoxes 2026
 */

#ifndef THANDOR_WORLD_PATHING_SCRATCH_GRID_H
#define THANDOR_WORLD_PATHING_SCRATCH_GRID_H

#include <thandor/core/types.h>
#include <thandor/ui/ingame/types.h>
#include <thandor/world/pathing/types.h>
#include <thandor/world/terrain/types.h>
#include <thandor/core/contracts.h>
#include <thandor/world/pathing/grid.h>

extern GridScratchCell *g_GridScratchPrimary;

extern uint32_t g_GridScratchWidth;

extern int32_t g_GridScratchHeight;

extern const int32_t g_GridTerrainClassThresholds[GRID_TERRAIN_THRESHOLD_COUNT]; /* int32_t[17] terrain-class thresholds of the grid classification and the model definition terrain-class values, one table (ModelDefinition_CopyTerrainClassValues indexes across entries); followed by 12 bytes of 0x90 padding */

void GridScratch_RebuildTerrainAndRuntimeClassificationMasks(WorldRuntimeContext *worldRuntime);

Bool8 GridScratch_AllocateForFieldGrid(FieldGridAsset *fieldGrid,uint32_t *outError);

void GridScratch_ReleaseBuffers();

void GridScratch_PropagateFieldOccupancyMaskNeighborhood(FieldGridAsset *fieldGrid);

Bool8 GridScratch_TestProjectedCellMaskBands(Q12 worldYQ12,Q12 worldXQ12,uint8_t lowBandIndex,uint8_t highBandIndex);

void __cdecl GridScratch_CopyPrimaryToSecondary();

void GridScratch_SwapPrimarySecondary();

void GridScratch_FloodFillConnectedCells
          (GridScratchStateMask traversalMask,uint32_t rowStrideBytes,GridScratchCell *currentCell);

void GridScratch_ResetTraversalFlagsAndCosts();

#endif /* THANDOR_WORLD_PATHING_SCRATCH_GRID_H */

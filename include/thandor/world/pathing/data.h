/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/include/thandor/world/pathing/data.h
 */

#ifndef THANDOR_WORLD_PATHING_DATA_H
#define THANDOR_WORLD_PATHING_DATA_H

#include <thandor/generated/types.h>
#include <thandor/generated/ui_templates.h>

extern GridScratchCell *g_GridScratchPrimary;

extern GridScratchCell *g_GridScratchSecondary;

extern GridScratchCell **g_GridPathCostQueueBegin;

extern GridScratchCell **g_GridPathCostQueueEnd;

extern GridScratchCell **g_GridPathCostQueuePassBoundary;

extern uint32_t g_GridScratchWidth;

extern int32_t g_GridScratchHeight;

extern uint32_t g_GridPathEntityClassMask;

extern uint32_t g_GridPathBlockingMask;

extern uint32_t g_GridPathHighCostMask;

extern uint32_t g_GridPathUnreachableRegionReferenceColumn;

extern uint32_t g_GridPathUnreachableRegionReferenceRow;

extern EntityPathingPriorityPair *g_EntityPathingPriorityPairs;

/* The 32 pairs g_EntityPathingPriorityPairs points at (EntityPathing_RebuildOverlappingGroupRoutes
   fills at most ENTITY_PATHING_PRIORITY_PAIR_CAPACITY of them and heap-sorts them in place) */
#define ENTITY_PATHING_PRIORITY_PAIR_CAPACITY 32
extern EntityPathingPriorityPair g_EntityPathingPriorityPairStorage[ENTITY_PATHING_PRIORITY_PAIR_CAPACITY];

extern uint32_t g_EntityPathingPriorityPairCount;

/* Indices into g_GridTerrainClassThresholds (one table in the original). Water surface
   deltas are Q12, normal angles are the high 16 bits of the cell's packed normal angles. */
enum {
    GRID_TERRAIN_THRESHOLD_BIT24_MAX_WATER_SURFACE_DELTA = 0,
    GRID_TERRAIN_THRESHOLD_BIT24_MAX_TRIANGLE1_NORMAL_ANGLE = 1,
    GRID_TERRAIN_THRESHOLD_BIT25_MAX_SELECTED_NORMAL_ANGLE = 2,
    GRID_TERRAIN_THRESHOLD_BIT26_MAX_SELECTED_NORMAL_ANGLE = 3,
    GRID_TERRAIN_THRESHOLD_BIT27_MAX_SELECTED_NORMAL_ANGLE = 4,
    GRID_TERRAIN_THRESHOLD_CLASS4_SECONDARY = 5, /* [3] 14000/15000/15500, contact kind 4, class 1..3 */
    GRID_TERRAIN_THRESHOLD_BIT28_MIN_WATER_SURFACE_DELTA = 8,
    GRID_TERRAIN_THRESHOLD_BIT29_MIN_WATER_SURFACE_DELTA = 9,
    GRID_TERRAIN_THRESHOLD_BIT30_MIN_WATER_SURFACE_DELTA = 10,
    GRID_TERRAIN_THRESHOLD_BIT28_MAX_TRIANGLE0_NORMAL_ANGLE = 11,
    GRID_TERRAIN_THRESHOLD_BIT29_MAX_TRIANGLE0_NORMAL_ANGLE = 12,
    GRID_TERRAIN_THRESHOLD_BIT30_MAX_TRIANGLE0_NORMAL_ANGLE = 13,
    GRID_TERRAIN_THRESHOLD_FALLBACK_SECONDARY = 14, /* [3] 12500/13500/14500, other contact kinds, class 4..6 */
    GRID_TERRAIN_THRESHOLD_COUNT = 17
};
extern int32_t g_GridTerrainClassThresholds[GRID_TERRAIN_THRESHOLD_COUNT]; /* int32_t[17] terrain-class thresholds of the grid classification and the model definition terrain-class values, one table (ModelDefinition_CopyTerrainClassValues indexes across entries); followed by 12 bytes of 0x90 padding */

extern uint32_t g_GridInfluenceRadiusOffset[8]; /* uint32_t[8] grid influence ring radius offsets 1000..4100, indexed by ring / footprint radius class (world/pathing/influence.c, assets/model/definitions.c) */

extern uint32_t g_GridInfluenceSquaredThreshold[8]; /* uint32_t[8] squared influence ring radii, ring n = (g_GridInfluenceRadiusOffset[n] + radius + margin)^2; [6] is also reused as the footprint clearance disc (world/pathing/influence.c, world/pathing/grid.c) */

#endif

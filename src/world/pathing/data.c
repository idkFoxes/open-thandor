/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/src/world/pathing/data.c
 */

/* Data of the original image that this module uses (moved here from the generated image data in
   step 4c); declared in <thandor/world/pathing/data.h>. */

#include <thandor/thandor.h>

#pragma warning(disable : 4152) /* function pointer fields initialized through (void *) */

__declspec(align(16)) GridScratchCell *g_GridScratchPrimary = 0;

__declspec(align(4)) GridScratchCell *g_GridScratchSecondary = 0;

__declspec(align(8)) GridScratchCell **g_GridPathCostQueueBegin = 0;

__declspec(align(4)) GridScratchCell **g_GridPathCostQueueEnd = 0;

__declspec(align(16)) GridScratchCell **g_GridPathCostQueuePassBoundary = 0;

__declspec(align(4)) uint32_t g_GridScratchWidth = 0;

__declspec(align(8)) int32_t g_GridScratchHeight = 0;

__declspec(align(16)) uint32_t g_GridPathEntityClassMask = 0;

__declspec(align(4)) uint32_t g_GridPathBlockingMask = 0;

__declspec(align(8)) uint32_t g_GridPathHighCostMask = 0;

__declspec(align(4)) uint32_t g_GridPathUnreachableRegionReferenceColumn = 0;

__declspec(align(16)) uint32_t g_GridPathUnreachableRegionReferenceRow = 0;

__declspec(align(16)) EntityPathingPriorityPair *g_EntityPathingPriorityPairs = g_EntityPathingPriorityPairStorage;

__declspec(align(4)) EntityPathingPriorityPair g_EntityPathingPriorityPairStorage[ENTITY_PATHING_PRIORITY_PAIR_CAPACITY] = {0};

__declspec(align(4)) uint32_t g_EntityPathingPriorityPairCount = 0;

/* int32_t[17] terrain-class thresholds, one table in the original
   (indexed by GRID_TERRAIN_THRESHOLD_*). GridScratch classification reads each entry by name;
   ModelDefinition_CopyTerrainClassValues indexes from several entries into their neighbours by the model's
   terrainTraversalClass (assets/model/definitions.c). Followed by 12 bytes of 0x90 padding in the original. */
__declspec(align(16)) int32_t g_GridTerrainClassThresholds[GRID_TERRAIN_THRESHOLD_COUNT] = {
    0, /* [0] bit 24 max water surface delta (Q12) */
    11500, /* [1] bit 24 max triangle 1 normal angle (high 16) */
    10500, /* [2] bit 25 max selected normal angle (high 16) */
    10500, /* [3] bit 26 max selected normal angle (high 16) */
    10500, /* [4] bit 27 max selected normal angle (high 16) */
    14000, /* [5] contact kind 4 traversal secondary threshold, class 1 */
    15000, /* [6] contact kind 4 traversal secondary threshold, class 2 */
    15500, /* [7] contact kind 4 traversal secondary threshold, class 3 */
    500 /* 0.12207 */, /* [8] bit 28 min water surface delta (Q12) */
    500 /* 0.12207 */, /* [9] bit 29 min water surface delta (Q12) */
    500 /* 0.12207 */, /* [10] bit 30 min water surface delta (Q12) */
    10500, /* [11] bit 28 max triangle 0 normal angle (high 16) */
    10500, /* [12] bit 29 max triangle 0 normal angle (high 16) */
    10500, /* [13] bit 30 max triangle 0 normal angle (high 16) */
    12500, /* [14] fallback traversal secondary threshold, class 4 */
    13500, /* [15] fallback traversal secondary threshold, class 5 */
    14500, /* [16] fallback traversal secondary threshold, class 6 */
};

/* uint32_t[8] grid influence ring radius offsets */
__declspec(align(16)) uint32_t g_GridInfluenceRadiusOffset[8] = {
    1000, /* [0] */
    1250, /* [1] */
    1500, /* [2] */
    1600, /* [3] */
    1920, /* [4] */
    2240, /* [5] */
    2600, /* [6] */
    4100, /* [7] */
};

/* uint32_t[8] squared ring radii (g_GridInfluenceRadiusOffset[n] + radius + margin)^2 */
__declspec(align(16)) uint32_t g_GridInfluenceSquaredThreshold[8] = {
    0, /* [0] */
    0, /* [1] */
    0, /* [2] */
    0, /* [3] */
    0, /* [4] */
    0, /* [5] */
    0, /* [6] */
    0, /* [7] */
};

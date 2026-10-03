/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/src/world/pathing/data.c
 */

/* Data of the original image that this module uses (moved here from the generated image data in
   step 4c); declared in <thandor/world/pathing/data.h>. Original addresses in the comments. */

#include <thandor/thandor.h>

#pragma warning(disable : 4152) /* function pointer fields initialized through (void *) */

/* 005332A0 g_GridScratchPrimary */
__declspec(align(16)) GridScratchCell *g_GridScratchPrimary = 0;

/* 005332A4 g_GridScratchSecondary */
__declspec(align(4)) GridScratchCell *g_GridScratchSecondary = 0;

/* 005332A8 g_GridPathCostQueueBegin */
__declspec(align(8)) GridScratchCell **g_GridPathCostQueueBegin = 0;

/* 005332AC g_GridPathCostQueueEnd */
__declspec(align(4)) GridScratchCell **g_GridPathCostQueueEnd = 0;

/* 005332B0 g_GridPathCostQueuePassBoundary */
__declspec(align(16)) GridScratchCell **g_GridPathCostQueuePassBoundary = 0;

/* 005332B4 g_GridScratchWidth */
__declspec(align(4)) uint32_t g_GridScratchWidth = 0;

/* 005332B8 g_GridScratchHeight */
__declspec(align(8)) int32_t g_GridScratchHeight = 0;

/* 00533ED0 g_GridPathEntityClassMask */
__declspec(align(16)) uint32_t g_GridPathEntityClassMask = 0;

/* 00533ED4 g_GridPathBlockingMask */
__declspec(align(4)) uint32_t g_GridPathBlockingMask = 0;

/* 00533ED8 g_GridPathHighCostMask */
__declspec(align(8)) uint32_t g_GridPathHighCostMask = 0;

/* 00533EDC g_GridPathUnreachableRegionReferenceColumn */
__declspec(align(4)) uint32_t g_GridPathUnreachableRegionReferenceColumn = 0;

/* 00533EE0 g_GridPathUnreachableRegionReferenceRow */
__declspec(align(16)) uint32_t g_GridPathUnreachableRegionReferenceRow = 0;

/* 005367D0 g_EntityPathingPriorityPairs */
__declspec(align(16)) EntityPathingPriorityPair *g_EntityPathingPriorityPairs = g_EntityPathingPriorityPairStorage;

/* 005367D4 g_EntityPathingPriorityPairStorage (Ghidra had split it at [0].priority, [1].entity, [1].priority) */
__declspec(align(4)) EntityPathingPriorityPair g_EntityPathingPriorityPairStorage[ENTITY_PATHING_PRIORITY_PAIR_CAPACITY] = {0};

/* 005368D4 g_EntityPathingPriorityPairCount */
__declspec(align(4)) uint32_t g_EntityPathingPriorityPairCount = 0;

/* 00536F10 g_GridTerrainClassThresholds: int32_t[17] terrain-class thresholds, one table in the original
   (indexed by GRID_TERRAIN_THRESHOLD_*). GridScratch classification reads each entry by name;
   ModelDefinition_CopyTerrainClassValues indexes from several entries into their neighbours by the model's
   terrainTraversalClass (assets/model/definitions.c). Followed by 12 bytes of 0x90 padding (0x536F54). */
__declspec(align(16)) int32_t g_GridTerrainClassThresholds[GRID_TERRAIN_THRESHOLD_COUNT] = {
    0, /* 00536F10 [0] bit 24 max water surface delta (Q12) */
    11500, /* 00536F14 [1] bit 24 max triangle 1 normal angle (high 16) */
    10500, /* 00536F18 [2] bit 25 max selected normal angle (high 16) */
    10500, /* 00536F1C [3] bit 26 max selected normal angle (high 16) */
    10500, /* 00536F20 [4] bit 27 max selected normal angle (high 16) */
    14000, /* 00536F24 [5] contact kind 4 traversal secondary threshold, class 1 */
    15000, /* 00536F28 [6] contact kind 4 traversal secondary threshold, class 2 */
    15500, /* 00536F2C [7] contact kind 4 traversal secondary threshold, class 3 */
    500 /* 0.12207 */, /* 00536F30 [8] bit 28 min water surface delta (Q12) */
    500 /* 0.12207 */, /* 00536F34 [9] bit 29 min water surface delta (Q12) */
    500 /* 0.12207 */, /* 00536F38 [10] bit 30 min water surface delta (Q12) */
    10500, /* 00536F3C [11] bit 28 max triangle 0 normal angle (high 16) */
    10500, /* 00536F40 [12] bit 29 max triangle 0 normal angle (high 16) */
    10500, /* 00536F44 [13] bit 30 max triangle 0 normal angle (high 16) */
    12500, /* 00536F48 [14] fallback traversal secondary threshold, class 4 */
    13500, /* 00536F4C [15] fallback traversal secondary threshold, class 5 */
    14500, /* 00536F50 [16] fallback traversal secondary threshold, class 6 */
};

/* 00536F60 g_GridInfluenceRadiusOffset: uint32_t[8] grid influence ring radius offsets */
__declspec(align(16)) uint32_t g_GridInfluenceRadiusOffset[8] = {
    1000, /* 00536F60 [0] */
    1250, /* 00536F64 [1] */
    1500, /* 00536F68 [2] */
    1600, /* 00536F6C [3] */
    1920, /* 00536F70 [4] */
    2240, /* 00536F74 [5] */
    2600, /* 00536F78 [6] */
    4100, /* 00536F7C [7] */
};

/* 00536F80 g_GridInfluenceSquaredThreshold: uint32_t[8] squared ring radii (g_GridInfluenceRadiusOffset[n] + radius + margin)^2 */
__declspec(align(16)) uint32_t g_GridInfluenceSquaredThreshold[8] = {
    0, /* 00536F80 [0] */
    0, /* 00536F84 [1] */
    0, /* 00536F88 [2] */
    0, /* 00536F8C [3] */
    0, /* 00536F90 [4] */
    0, /* 00536F94 [5] */
    0, /* 00536F98 [6] */
    0, /* 00536F9C [7] */
};

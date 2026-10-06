/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/include/thandor/world/pathing/types.h
 * Reverse engineering by idkFoxes 2026
 */

#ifndef THANDOR_WORLD_PATHING_TYPES_H
#define THANDOR_WORLD_PATHING_TYPES_H

#include <stdint.h>
#include <thandor/core/ptr32.h> /* Ptr32: the pointer fields of these 32-bit layouts */
#include <thandor/core/types.h>

typedef struct EntityPathingPriorityPair EntityPathingPriorityPair, *PEntityPathingPriorityPair;
typedef struct GridScratchCell GridScratchCell, *PGridScratchCell;
typedef struct GridPathBestUnreachableCell GridPathBestUnreachableCell, *PGridPathBestUnreachableCell;
typedef struct EntityPathingRouteEntityRuntimeView EntityPathingRouteEntityRuntimeView, *PEntityPathingRouteEntityRuntimeView;
typedef struct WorldPositionXY WorldPositionXY, *PWorldPositionXY;
typedef struct ArmyMovementRuntime ArmyMovementRuntime;
typedef struct GameEntityRuntime GameEntityRuntime;
typedef struct ModelDefinition ModelDefinition;
typedef struct ModelRuntimeNode ModelRuntimeNode;

using PriorityPairHeapCount = int;

using GridPathUnreachableReferenceColumn32 = uint32_t;

using GridPathPassCount = int;

using GridPathUnreachableReferenceRow32 = uint32_t;

struct EntityPathingPriorityPair {
    Ptr32<struct GameEntityRuntime> entity;
    int32_t priority;
};

using GridPathCost = uint32_t;

enum { 
    GRID_SCRATCH_TRAVERSAL_VISITED=1,
    GRID_SCRATCH_TERRAIN_CLASS_BIT24=16777216,
    GRID_SCRATCH_TERRAIN_CLASS_BIT25=33554432,
    GRID_SCRATCH_TERRAIN_CLASS_BIT26=67108864,
    GRID_SCRATCH_TERRAIN_CLASS_BIT27=134217728,
    GRID_SCRATCH_TERRAIN_CLASS_BIT28=268435456,
    GRID_SCRATCH_TERRAIN_CLASS_BIT29=536870912,
    GRID_SCRATCH_TERRAIN_CLASS_BIT30=1073741824
};
using GridScratchStateMask = int;

struct GridScratchCell {
    GridScratchStateMask stateMask; 
    GridPathCost pathCost; 
};
/* Step 13 X7b: the scratch grid's byte-address views, each one named reinterpret_cast.
   GridScratchCell_RowBelow/RowAbove: the cell one row (rowStrideBytes bytes) below/above; the stride keeps the
   caller's integer type, as the byte-pointer arithmetic cell +- rowStrideBytes they replace did.
   GridScratchCell_StateMaskBits: stateMask (an int) as the uint32_t the hex-line walkers and the dword loops over the
   grid read and write. */
template <class Stride> inline GridScratchCell *GridScratchCell_RowBelow(GridScratchCell *cell, Stride rowStrideBytes)
{
    return reinterpret_cast<GridScratchCell *>(reinterpret_cast<uint8_t *>(cell) + rowStrideBytes);
}
template <class Stride> inline GridScratchCell *GridScratchCell_RowAbove(GridScratchCell *cell, Stride rowStrideBytes)
{
    return reinterpret_cast<GridScratchCell *>(reinterpret_cast<uint8_t *>(cell) - rowStrideBytes);
}
inline uint32_t *GridScratchCell_StateMaskBits(GridScratchCell *cell)
{
    return reinterpret_cast<uint32_t *>(&cell->stateMask);
}

struct GridPathBestUnreachableCell {
    uint32_t bestCellByteOffset; /* in/out: byte offset from g_GridScratchPrimary. */
    GridPathCost bestCost;    /* in/out: best path metric; outer caller starts at 0x7fffffff. */
};

struct EntityPathingRouteEntityRuntimeView {
    Ptr32<struct ModelDefinition> modelDefinition; // Entity ownership definition pointer; EntityPathing_UpdateRouteSegment reads runtimeClassId, footprintRadiusClass, terrainTraversalClass and footprintRadius from this object.
    Ptr32<struct ModelRuntimeNode> modelNode; // Entity ownership model-node pointer; EntityPathing_UpdateRouteSegment reads worldTransform.translation.x/y.
    Ptr32<struct ArmyMovementRuntime> movementRuntime; // Entity ownership runtime-link specialized for this pathing routine; EntityPathing_UpdateRouteSegment reads factionIndex and movementWorldX/Y and passes it to ArmyRuntime_SetPendingMoveTarget.
    FactionRuntimeIndex ownerIndex; // Ownership index retained at canonical +0C; not reinterpreted by this shard.
};

struct WorldPositionXY {
    Q12 worldXQ12; // world X
    Q12 worldYQ12; // world Y
};

#endif /* THANDOR_WORLD_PATHING_TYPES_H */

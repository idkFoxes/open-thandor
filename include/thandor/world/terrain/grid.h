/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/include/thandor/world/terrain/grid.h
 * Reverse engineering by idkFoxes 2026
 */

#ifndef THANDOR_WORLD_TERRAIN_GRID_H
#define THANDOR_WORLD_TERRAIN_GRID_H

#include <thandor/core/contracts.h>
#include <thandor/world/terrain/types.h> /* FieldGridCell for the cell helpers */

/* Field-grid cell flag bits (FieldGridCell.flagsAndMaterial, +0x50): the flag enum class
   FieldCellPackedFlagsAndMaterial in core/types.h (FIELD_CELL_LAST_ROW_BOUNDARY and FIELD_CELL_GRID_EDGE_MASK
   included). */
static_assert(FIELD_CELL_GRID_EDGE_MASK == (FIELD_CELL_LAST_ROW_BOUNDARY | FIELD_CELL_LAST_COLUMN_BOUNDARY |
                                            FIELD_CELL_FIRST_ROW_BOUNDARY | FIELD_CELL_FIRST_COLUMN_BOUNDARY),
              "FIELD_CELL_GRID_EDGE_MASK is the four map-edge bits");

/* World plane to field-grid coordinates (FieldGrid_WorldToGridQ12 and the samplers that inline it): the grid
   is a triangular lattice, grid columns per world unit in Q20 (about 1 / 0.5625) and grid rows per world unit in
   Q20, negative because rows grow towards -Y (about -2.05). The column is then skewed by half the row. */
inline constexpr int FIELD_GRID_WORLD_X_TO_COLUMN_Q20 = 0x1c6e9c;
inline constexpr int FIELD_GRID_WORLD_Y_TO_ROW_Q20 = -0x20c8cc;

/* FieldGridCell.occupancyMask (+0x70) holds one occupancy byte per faction slot 0..7 (the tick wheel
   indexes it with WorldRuntimeContext.activeFactionRuntimeIndex). Bit meanings inside a byte as far as
   the tick-wheel code shows them: */
inline constexpr int FIELD_CELL_OCCUPANCY_BIT0 = 0x01; /* set/cleared grid-wide for one faction by the Bit0 helpers */
inline constexpr int FIELD_CELL_OCCUPANCY_REBUILT_BITS = 0x7f; /* bits 0..6: cleared before every occupancy rebuild */
inline constexpr int FIELD_CELL_OCCUPANCY_PERSISTENT_BIT = 0x80; /* bit 7: survives the rebuild clear */
inline constexpr int FIELD_CELL_OCCUPANCY_PRESENCE_BITS = 0xf9; /* bits that count as "faction present" (1 and 2 excluded) */
inline constexpr int FIELD_CELL_OCCUPANCY_CURRENT_PRESENCE_BITS = 0x79; /* the presence bits without the persistent bit 7
                                                           (FieldGrid_ClassifyCellFlagsToRuntimeByte, minimap) */
inline constexpr int FIELD_CELL_OCCUPANCY_EXPLORED_BITS = 0xf8; /* bits 3..7: the faction has explored the cell (exploration score) */
/* a byte mask moved into the faction slot's byte of the 64-bit occupancyMask */
#define FIELD_CELL_OCCUPANCY_SLOT_MASK(bits,factionSlot) ((uint64_t)(bits) << ((factionSlot) * 8))
/* The faction slot's occupancy byte of a cell (an lvalue): byte factionSlot of the 64-bit occupancyMask, one
   byte per faction slot (step 13 X7: was the macro FIELD_CELL_OCCUPANCY_BYTE). The index keeps the caller's type,
   as the macro's subscript did. */
template <class SlotIndex> inline uint8_t &FieldGridCell_OccupancyByte(FieldGridCell *cell, SlotIndex factionSlot)
{
    return reinterpret_cast<uint8_t *>(&cell->occupancyMask)[factionSlot];
}
/* The cell byteOffset bytes away from cell; byteOffset is usually +-the row stride (one grid row). It is a
   signed 32-bit offset, also when computed in unsigned arithmetic (-stride of a uint32_t stride), so it moves
   backwards on x64 too (step 13 X7: was the macro FIELD_GRID_CELL_AT_BYTE_OFFSET). */
inline FieldGridCell *FieldGridCell_AtByteOffset(FieldGridCell *cell, int32_t byteOffset)
{
    return reinterpret_cast<FieldGridCell *>(reinterpret_cast<uint8_t *>(cell) + byteOffset);
}

/* FieldGridCell.visibilityLightingIndex (+0x68) as FieldGrid_ClassifyCellFlagsToRuntimeByte sets it; the
   projection pass indexes g_PackedLightingLookupTable with it, FIELD_CELL_LIGHTING_VISIBLE selects the dynamic
   lights instead. */
inline constexpr int FIELD_CELL_LIGHTING_VISIBLE = 0xff; /* a current presence bit of the faction is set */
inline constexpr int FIELD_CELL_LIGHTING_EXPLORED = 0x87; /* only the persistent occupancy bit 7 is set */
inline constexpr int FIELD_CELL_LIGHTING_UNEXPLORED = 0x00;

/* FieldGridAsset.runtimeStateFlags: FIELD_GRID_RUNTIME_SURFACE_DIRTY is an enumerator of FieldGridRuntimeFlags
   (world/terrain/types.h). */
/* FieldGrid_RaycastTerrainSurfaceDistance / ..SecondarySurfaceDistance: at most this many cell steps per ray
   (the counter is decremented before the first step, so 1023 cells are visited), and the miss distance. */
inline constexpr int FIELD_GRID_RAYCAST_MAX_STEPS = 1024;
inline constexpr int FIELD_GRID_RAYCAST_MISS_DISTANCE = 0x7fffffff;
/* One grid cell in Q12 grid coordinates; masking with ~(FIELD_GRID_CELL_Q12 - 1) keeps the cell origin. */
inline constexpr int FIELD_GRID_CELL_Q12 = 0x1000;
/* Lattice cell of a Q12 grid position (the samplers after FieldGrid_WorldToGridQ12): with the Q12 fractions f
   (column) and g (row), f + 2g and 2f + g compared with one and two cells (FIELD_GRID_CELL_Q12,
   FIELD_GRID_TWO_CELLS_Q12) pick the cell of the triangle the point lies in. */
inline constexpr int FIELD_GRID_TWO_CELLS_Q12 = 0x2000;
/* gridWidth as the original recovers it from the row stride (gridWidth * sizeof(FieldGridCell) divided by the
   128-byte cell size in 32 bits): the 25 width bits that survive the stride multiply */
inline constexpr int FIELD_GRID_ROW_STRIDE_WIDTH_MASK = 0x1ffffff;
/* sqrt(3) in Q12 (7094): FieldGrid_ApplyRadialTerrainHeightDeltaAndRefreshSurface scales the radius by it for the X
   half-extent of the box around the circle */
inline constexpr int FIELD_GRID_SQRT3_Q12 = 0x1bb6;
/* Editor drag brushes (FieldGrid_ApplyEncodedUpdateCore, FieldGrid_ProcessHorizontalSpan/VerticalSpan): one drag
   unit moves a height or water level by 64 (Q12) and widens a brush radius by 64 world units; the radius is
   clamped to FIELD_GRID_EDIT_BRUSH_RADIUS_MAX. */
inline constexpr int FIELD_GRID_EDIT_DRAG_UNIT_Q12 = 0x40;
inline constexpr int FIELD_GRID_EDIT_BRUSH_RADIUS_MAX = 0x5000;
/* occupancy bits 0 and 1 of a faction byte: positioned sounds only play in cells where one of them is set
   (TerrainGrid_TestProjectedCellMaskBits01) */
inline constexpr int FIELD_CELL_OCCUPANCY_BITS01 = 0x03;
/* occupancy bit 1, set within an army's radius by TerrainOccupancyBit2_MarkAroundWorldPoint (the "Bit2" in the
   TerrainOccupancyBit2_* names is the mask value 2) */
inline constexpr int FIELD_CELL_OCCUPANCY_BIT1 = 0x02;
/* Two FieldGridCells in bytes: the byte-addressed reverse water relaxation passes step two rows (upper
   neighbour row to lower neighbour row) as rowLength * this, and back over the two border cells at a row end. */
#define FIELD_GRID_TWO_CELLS_BYTES (2 * sizeof(FieldGridCell))
/* Z of the unnormalised cell normal (FieldGridCell_RecomputeTriangleNormalAngles), 3 * 2048^2: the sum of the
   squared X offsets of the six lattice neighbours for a cell spacing of 2048 world units, so a plane's X/Y tilt
   sums come out against it roughly to scale (the real spacing is 2305, FIELD_GRID_WORLD_COLUMN_STEP_X). */
inline constexpr int FIELD_GRID_NORMAL_Z_COMPONENT = 0xc00000;
/* Height-drag brush falloff (FieldGrid_ProcessHorizontalSpan/VerticalSpan): the original 64-bit
   distance * FIXED_ANGLE16_HALF_TURN keeps bits 0..48 of the sign-extended distance and shifts them right by 17
   to get the product's high dword (a plain 64-bit multiply compiles differently). */
inline constexpr uint64_t FIELD_GRID_ANGLE_PRODUCT_HIGH_BITS_MASK = 0x1ffffffffffffU;

#endif /* THANDOR_WORLD_TERRAIN_GRID_H */

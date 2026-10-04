/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/include/thandor/world/terrain/types.h
 * Reverse engineering by idkFoxes 2026
 */

#ifndef THANDOR_WORLD_TERRAIN_TYPES_H
#define THANDOR_WORLD_TERRAIN_TYPES_H

#include <stdint.h>
#include <thandor/core/ptr32.h> /* Ptr32: the pointer fields of these 32-bit layouts */
#include <thandor/core/types.h>
#include <thandor/graphics/render/types.h>
#include <thandor/graphics/resources/types.h>

/* Types (split out by tools/dev/split_types.py). */

typedef struct FieldGridAsset FieldGridAsset, *PFieldGridAsset;
typedef struct FieldGridCell FieldGridCell, *PFieldGridCell;
typedef struct TerrainDirectionRecord TerrainDirectionRecord, *PTerrainDirectionRecord;
typedef union TerrainScanSelectorUnion TerrainScanSelectorUnion, *PTerrainScanSelectorUnion;
typedef struct TerrainClassPlacementAndOverlayCallbackTable10 TerrainClassPlacementAndOverlayCallbackTable10, *PTerrainClassPlacementAndOverlayCallbackTable10;
typedef struct FieldGridInterpolationCallbackTable5 FieldGridInterpolationCallbackTable5, *PFieldGridInterpolationCallbackTable5;
typedef struct FieldGridCoordinates FieldGridCoordinates, *PFieldGridCoordinates;
typedef struct TerrainOccupancyResolvedMasks TerrainOccupancyResolvedMasks, *PTerrainOccupancyResolvedMasks;
typedef struct FieldGridCellSaveImageView FieldGridCellSaveImageView, *PFieldGridCellSaveImageView;

/* Recovered semantic scalar types used by canonical records. */
using ResourceExtractionDescriptor32 = uint32_t;

using FieldGridRegionMask = uint32_t;

using TerrainMaterialIndex = int;

using FieldGridFlags = uint32_t;

using FieldGridRuntimeFlags = uint32_t;

using PackedTerrainNormalAngles = uint32_t;

using FieldCellPersistedAux = uint32_t;

using TerrainOccupancyMask = uint64_t;

using FieldGridCellCoordinate = int;

enum {
    TERRAIN_RELAXATION_SIGN_GATED=0,
    TERRAIN_RELAXATION_UNGATED_LAND_TOOL=1
};
using TerrainRelaxationMode = int;

using TerrainRelaxationPassCount = uint32_t;

using FieldCellFlagMask = uint32_t;

struct FieldGridCoordinates {
    Q12 columnQ12;
    Q12 rowQ12;
};

using TerrainOverlayCellRuntimeValue = int;

using FieldGridDimensionCells = int;

using TerrainHeightBrushDeltaSource = uint32_t;

using TerrainDirectionalScanStep = uint32_t;

using TerrainRegionCollectionCount = uint32_t;

using TerrainDirectionRecordCount = uint32_t;

using TerrainMaterialByteValue = uint32_t;

using TerrainProjectedHeightThresholdQ20 = uint32_t;

using ArmyRuntimeSavedOffset = uint32_t;

/* +0x0C..+0x3F and +0x60..+0x6B are the terrain projection pass's per-vertex work area (the same layout as
   TerrainProjectedVertexWorkRecord, which graphics/terrain/terrain_render.cpp uses as its view of a cell). */
struct FieldGridCell {
    uint32_t surfacePacketIndex; // Terrain surface packet (animation phase) of the cell: random at load (FieldGrid_InitializeRuntimeCellsAndBoundaryFlags), TerrainProjectedVertexWorkRecord.surfacePacketIndex.
    PackedArgb32 overlayColor; // ARGB tint multiplied into the terrain shading (TerrainProjectedVertexWorkRecord.basePackedColor): opaque white at load, set by the terrain-class overlay callbacks and FieldGrid_SetAllCellOverlayColors.
    PackedTerrainNormalAngles triangle0NormalAngles; // Packed normal-angle pair.
    struct GraphicsProjectedPointPair groundScreenPoint; // +0x0C projected screen point of the ground vertex (Q12 pixels; projectedPointA).
    struct GraphicsFixedVec3 groundViewPoint; // +0x14 ground vertex in view space (viewPointA).
    uint8_t runtime20_2B[12]; // Unresolved.
    struct GraphicsProjectedPointPair secondarySurfaceScreenPoint; // +0x2C projected screen point of the secondary (water) surface vertex (projectedPointB).
    struct GraphicsFixedVec3 secondarySurfaceViewPoint; // +0x34 secondary surface vertex in view space (viewPointB).
    Q12 worldX; // Generated world X coordinate.
    Q12 worldY; // Generated world Y coordinate.
    Q12 terrainHeight; // Terrain height.
    Q12 waterSurfaceDelta; // Water-surface delta.
    FieldCellPackedFlagsAndMaterial flagsAndMaterial; // [FIELD_GRID_STORAGE_NAMESPACE_DB_CLOSURE] FLD +0x50 namespace: low byte material; 0x700 runtime-random variant; 0x0800 Xenite support; 0x1000 Tritium support; 0x88006000 hard edges; 0x10000 transient region-visited; 0x20000000 fluid receiver exclusion; 0x40000000 fluid source exclusion; 0x10000000 terrain-visual-clearable but semantic unresolved; 0x8000 init-cleared unresolved. Numeric GridScratch class bits are a different allocation and must not be written here.
    FieldCellPersistedAux persistedAux54; // Persisted field-cell auxiliary value.
    PackedArgb32 groundDirectionalLightColor; // Packed ground directional-light color written by FieldGridCell_ComputeDirectionalLightColor and consumed by terrain shading.
    PackedArgb32 secondarySurfaceDirectionalLightColor; // Packed secondary-surface directional-light color written beside the ground color and consumed by the secondary terrain shading path.
    PackedArgb32 shadedGroundColor; // +0x60 ground vertex color after shading (projection pass; shadedColorA).
    PackedArgb32 shadedSecondarySurfaceColor; // +0x64 secondary surface vertex color after shading (shadedColorB).
    uint8_t visibilityLightingIndex; // +0x68 fog-of-war lighting index (lightingLookupIndexOrSentinel): 0xFF visible now (dynamic lights), 0x87 seen before, 0 never seen (FieldGrid_ClassifyCellFlagsToRuntimeByte), then remapped by FieldGrid_ApplyByteClampLookupToCells.
    uint8_t runtime69_6B[3]; // Upper bytes of the 32-bit lighting index read by the projection pass.
    ArmyRuntimeSavedOffset armyRuntimeSavedOffset; // Army runtime pool-relative token. Producer stores armyRuntime - g_ModelRuntimeRebaseDelta; connected-region collection copies it; consumer adds the same delta before dereference.
    TerrainOccupancyMask occupancyMask; // [FIELD_GRID_STORAGE_NAMESPACE_DB_CLOSURE] Separate 64-bit runtime occupancy namespace. Do not conflate with FLD +0x50 flagsAndMaterial or GridScratchCell_V419.stateMask.
    PackedTerrainNormalAngles triangle1NormalAngles; // Packed normal-angle pair.
    ResourceExtractionDescriptor32 resourceExtractionDescriptor; // Packed class-14 extraction reservation: resource-support bit, faction index, and extraction weight; cleared when the reservation is released/collected.
};

struct FieldGridAsset {
    struct GeneratedAssetCommonPrefix common; 
    FieldGridFlags fieldFlags; 
    FieldGridRuntimeFlags runtimeStateFlags; 
    FieldGridDimension gridWidth; 
    FieldGridDimension gridHeight; 
    uint8_t reservedC0_FF[64]; 
    uint16_t sourcePath[128]; 
    struct FieldGridCell cells[1]; 
};

using FieldGridOccupancyBlockCount = uint32_t;

struct TerrainDirectionRecord {
    uint32_t angleAComponent0ScaledQ28;
    uint32_t angleAComponent1ScaledQ28;
    uint32_t angleBComponent0ScaledQ28;
    uint32_t packedAngles; // angle A in the low word, angle B in the high word (advanced together by rateA | rateB << 16)
    uint32_t scaleA;
    uint32_t scaleB;
    uint16_t rateA;
    uint16_t rateB;
    uint32_t reserved1C;
};

using FieldGridOccupancyByteIndex = int;

union TerrainScanSelectorUnion {
    FieldGridOccupancyByteIndex occupancyMaskByteIndex; 
    FieldCellFlagMask fieldCellFlagMask; 
    uint32_t raw; 
};

using FieldGridRadiusUnits = int;

using FieldGridAccumulatorValue = int;

using FieldGridTransitionValue = uint32_t;

using FieldGridRowStrideBytes = int;

using FieldGridCommandReservedValue = uint32_t;

using FieldGridMaterialBitIndex = int;

using FieldGridByteOffset = int;

using FieldGridHeightDeltaUnits = int;

using PackedFieldGridDeltaXY16 = uint32_t;

struct FieldGridInterpolationCallbackTable5 {
    Ptr32<Bool8 (Q12, Q12, struct FieldGridAsset *, Q12 *)> callbacks[5]; // Exact immutable callback partition: height samplers (y, x, grid, out height Q12) returning false off the grid.
};
struct TerrainOccupancyResolvedMasks {
    uint32_t secondaryOccupancyMask;
    uint32_t primaryOccupancyMask;
    uint32_t runtimeFlags;
};

struct FieldGridCellSaveImageView { // Function-local physical serialization view for FieldGrid_SaveAssetImageFromRuntimeState: the FieldGridCell layout with every runtime field as a plain dword, because this routine clears them with 32-bit stores (visibilityLightingIndex included).
    uint32_t surfacePacketIndex;
    uint32_t overlayColor;
    PackedTerrainNormalAngles triangle0NormalAngles;
    uint32_t groundScreenX;
    uint32_t groundScreenY;
    uint32_t groundViewX;
    uint32_t groundViewY;
    uint32_t groundViewZ;
    uint32_t runtime20;
    uint32_t runtime24;
    uint32_t runtime28;
    uint32_t secondarySurfaceScreenX;
    uint32_t secondarySurfaceScreenY;
    uint32_t secondarySurfaceViewX;
    uint32_t secondarySurfaceViewY;
    uint32_t secondarySurfaceViewZ;
    Q12 worldX;
    Q12 worldY;
    Q12 terrainHeight;
    Q12 waterSurfaceDelta;
    FieldCellPackedFlagsAndMaterial flagsAndMaterial;
    FieldCellPersistedAux persistedAux54;
    uint32_t groundDirectionalLightColor;
    uint32_t secondarySurfaceDirectionalLightColor;
    uint32_t shadedGroundColor;
    uint32_t shadedSecondarySurfaceColor;
    uint32_t visibilityLightingIndex;
    uint32_t armyRuntimeSavedOffset;
    TerrainOccupancyMask occupancyMask;
    PackedTerrainNormalAngles triangle1NormalAngles;
    uint32_t resourceExtractionDescriptor;
};

struct TerrainClassPlacementAndOverlayCallbackTable10 {
    Ptr32<Bool8 (FieldGridRadiusUnits, Q12, Q12, Q12, struct FieldGridAsset *)> placementTests[5]; // Exact immutable callback partition; the bool result is true on reject.
    Ptr32<Bool8 (FieldCellFlagMask, TerrainOverlayCellRuntimeValue, FieldGridRadiusUnits, Q12, Q12, struct FieldGridAsset *)> overlayCallbacks[5]; // Exact immutable callback partition.
};

#endif /* THANDOR_WORLD_TERRAIN_TYPES_H */

/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/src/platform/selftest/hexscan_selftest.cpp
 * Project code (not in the original game)
 */

/* OPEN_THANDOR_SELFTEST=hexscan: golden hashes of the hexagonal radius scans in src/world/terrain (the six sector
   walkers and six straight legs of overlay marking A and B, occupancy marking, the flatten brush, the two placement
   tests and the line-of-sight accumulation), a safety net for merging those walkers. No game data is needed: the
   field grid is synthetic (67x53 cells, the map-edge ring flagged like FieldGrid_InitializeRuntimeCellsAndBoundaryFlags
   does) and filled from a local seeded LCG (not the game's Random_* streams). Each driver runs at
   HEXSCAN_TEST_POINTS seeded world points, inside and outside the grid and on the edge ring, with radii from 0 to
   past the 255-step clamp, starting from a grid rebuilt from its own seed.
   - The drivers that write cells are reset every HEXSCAN_TEST_CHUNK calls; the hash folds in the whole cell array
     after each chunk, so a cell that a later call would also mark still counts. Each call writes a different value
     (overlay value, occupancy byte, flatten height, sight bit).
   - The two placement tests do not write: they run on a grid where every cell passes, with up to two failing
     probe cells planted near the centre per call (and restored after it), so each result says whether the walk
     reached a probe (or the edge ring). The hash covers the results and the cell array.
   One line per driver: "hexscan <driver>: <FNV-1a hash> (<n> cells changed)" or "(<n> true)", then a summary.
   Run it with two builds and compare the lines. */

#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <vector>
#include <thandor/thandor.h>
#include <thandor/platform/bootstrap/image.h>
#include <thandor/platform/selftest/selftest.h>
#include <thandor/world/terrain/field_deformation.h>
#include <thandor/world/terrain/grid.h>
#include <thandor/world/terrain/hex_scan.h>
#include <thandor/world/terrain/occupancy.h>
#include <thandor/world/terrain/overlay_marking.h>
#include <thandor/world/terrain/placement_tests.h>
#include <thandor/world/terrain/sight.h>

#define HEXSCAN_TEST_WIDTH 67
#define HEXSCAN_TEST_HEIGHT 53
#define HEXSCAN_TEST_POINTS 2000
#define HEXSCAN_TEST_CHUNK 8
#define HEXSCAN_TEST_MARGIN_CELLS 3 /* points reach this many cells past every grid side */
#define HEXSCAN_TEST_RADIUS_MAX (TERRAIN_SCAN_RADIUS_PER_STEP * 300)
/* the placement tests' height band is +-1024 around the reference: clean heights and references stay within
   +-HEXSCAN_TEST_CLEAN_HEIGHT, so every difference is inside it */
#define HEXSCAN_TEST_CLEAN_HEIGHT 400
#define HEXSCAN_TEST_AUX_MINIMUM 0x3000 /* g_TerrainAuxHeightMinimum (placement_tests.cpp) */

enum HexscanTestProfile {
    HEXSCAN_PROFILE_NOISE,      /* the writing drivers: random heights, water above/below/at zero, flags */
    HEXSCAN_PROFILE_HEIGHTBAND, /* every cell passes TerrainHeightBand (water <= 0, heights in the band) */
    HEXSCAN_PROFILE_AUX         /* every cell passes TerrainAuxHeightThreshold (water >= 0, normals high) */
};

enum HexscanTestDriver {
    HEXSCAN_OVERLAY_A,
    HEXSCAN_OVERLAY_B,
    HEXSCAN_OCCUPANCY,
    HEXSCAN_FLATTEN,
    HEXSCAN_HEIGHTBAND,
    HEXSCAN_AUX,
    HEXSCAN_SIGHT,
    HEXSCAN_DRIVER_COUNT
};

static const char *const g_HexscanTestDriverNames[HEXSCAN_DRIVER_COUNT] = {
    "overlayA", "overlayB", "occupancy", "flatten", "heightband", "aux", "sight",
};

/* 64-bit LCG (Knuth's MMIX constants); the high half is the result. */
static uint32_t HexscanTest_Random(uint64_t *seed)
{
    *seed = *seed * 6364136223846793005ull + 1442695040888963407ull;
    return (uint32_t)(*seed >> 32);
}

/* Random in [low, high). */
static int HexscanTest_Range(uint64_t *seed, int low, int high)
{
    return low + (int)(HexscanTest_Random(seed) % (uint32_t)(high - low));
}

static uint64_t HexscanTest_Random64(uint64_t *seed)
{
    uint64_t high = HexscanTest_Random(seed);
    return (high << 32) | HexscanTest_Random(seed);
}

static uint32_t HexscanTest_Hash(uint32_t hash, const void *bytes, size_t count)
{
    size_t i;
    for (i = 0; i < count; i++) {
        hash = (hash ^ static_cast<const uint8_t *>(bytes)[i]) * 16777619u;
    }
    return hash;
}

/* A packed normal-angle pair whose high word passes (or, with low set, fails) the auxiliary threshold. */
static uint32_t HexscanTest_NormalAngles(uint64_t *seed, int low)
{
    uint32_t highWord = low ? (uint32_t)HexscanTest_Range(seed, 0, HEXSCAN_TEST_AUX_MINIMUM)
                            : (uint32_t)HexscanTest_Range(seed, HEXSCAN_TEST_AUX_MINIMUM, 0x8000);
    return (highWord << 16) | (HexscanTest_Random(seed) & 0xffffu);
}

/* Fills every cell from seed for the profile and flags the map-edge ring. */
static void HexscanTest_FillGrid(FieldGridAsset *grid, uint64_t seed, int profile)
{
    uint32_t row;
    uint32_t column;

    grid->runtimeStateFlags = 0;
    for (row = 0; row < grid->gridHeight; row++) {
        for (column = 0; column < grid->gridWidth; column++) {
            FieldGridCell *cell = &grid->cells[row * grid->gridWidth + column];
            uint32_t pick;
            memset(cell, 0, sizeof *cell);
            cell->surfacePacketIndex = HexscanTest_Random(&seed);
            cell->overlayColor = 0xffffffffu;
            cell->worldX = (Q12)(column * 0x900);
            cell->worldY = (Q12)(row * -0x7d0);
            cell->flagsAndMaterial = HexscanTest_Random(&seed) & ~FIELD_CELL_GRID_EDGE_MASK;
            cell->triangle0NormalAngles = HexscanTest_NormalAngles(&seed, 0);
            cell->triangle1NormalAngles = HexscanTest_NormalAngles(&seed, 0);
            cell->occupancyMask = HexscanTest_Random64(&seed) & 0x8484848484848484ull;
            pick = HexscanTest_Random(&seed) % 3;
            if (profile == HEXSCAN_PROFILE_NOISE) {
                cell->terrainHeight = HexscanTest_Range(&seed, -0x6000, 0x6000);
                cell->waterSurfaceDelta = pick == 0 ? HexscanTest_Range(&seed, -0x3000, 0)
                                                    : (pick == 1 ? 0 : HexscanTest_Range(&seed, 1, 0x3000));
                if (HexscanTest_Random(&seed) % 4 == 0) {
                    cell->triangle1NormalAngles = HexscanTest_NormalAngles(&seed, 1);
                }
            }
            else {
                cell->terrainHeight = HexscanTest_Range(&seed, -HEXSCAN_TEST_CLEAN_HEIGHT, HEXSCAN_TEST_CLEAN_HEIGHT + 1);
                if (pick == 0) {
                    cell->waterSurfaceDelta = 0;
                }
                else if (profile == HEXSCAN_PROFILE_HEIGHTBAND) {
                    cell->waterSurfaceDelta = HexscanTest_Range(&seed, -0x3000, 0);
                }
                else {
                    cell->waterSurfaceDelta = HexscanTest_Range(&seed, 1, 0x3000);
                }
            }
        }
    }
    for (column = 0; column < grid->gridWidth; column++) {
        grid->cells[column].flagsAndMaterial |= FIELD_CELL_FIRST_ROW_BOUNDARY;
        grid->cells[(grid->gridHeight - 1) * grid->gridWidth + column].flagsAndMaterial |= FIELD_CELL_LAST_ROW_BOUNDARY;
    }
    for (row = 0; row < grid->gridHeight; row++) {
        grid->cells[row * grid->gridWidth].flagsAndMaterial |= FIELD_CELL_FIRST_COLUMN_BOUNDARY;
        grid->cells[row * grid->gridWidth + grid->gridWidth - 1].flagsAndMaterial |= FIELD_CELL_LAST_COLUMN_BOUNDARY;
    }
}

/* A radius that covers the clamp to 1 (below one step), the clamp to 255 and everything between, biased to the
   radii that fit inside the grid. */
static FieldGridRadiusUnits HexscanTest_Radius(uint64_t *seed)
{
    switch (HexscanTest_Random(seed) % 8) {
    case 0: return HexscanTest_Range(seed, 0, TERRAIN_SCAN_RADIUS_PER_STEP * 2);
    case 1: return HexscanTest_Range(seed, TERRAIN_SCAN_RADIUS_PER_STEP * 254, HEXSCAN_TEST_RADIUS_MAX + 1);
    case 2:
    case 3: return HexscanTest_Range(seed, 0, TERRAIN_SCAN_RADIUS_PER_STEP * 16);
    case 4:
    case 5: return HexscanTest_Range(seed, 0, TERRAIN_SCAN_RADIUS_PER_STEP * 64);
    default: return HexscanTest_Range(seed, 0, HEXSCAN_TEST_RADIUS_MAX + 1);
    }
}

/* A world point (worldY, worldX) whose grid position (inverse of FieldGrid_WorldToGridQ12) lies anywhere within
   HEXSCAN_TEST_MARGIN_CELLS of the grid; *rowOut and *columnOut get the approximate cell. */
static void HexscanTest_WorldPoint(uint64_t *seed, const FieldGridAsset *grid, Q12 *worldY, Q12 *worldX, int *rowOut,
                                   int *columnOut)
{
    int64_t rowQ12 = HexscanTest_Range(seed, -HEXSCAN_TEST_MARGIN_CELLS * FIELD_GRID_CELL_Q12,
                                       ((int)grid->gridHeight + HEXSCAN_TEST_MARGIN_CELLS) * FIELD_GRID_CELL_Q12);
    int64_t columnQ12 = HexscanTest_Range(seed, -HEXSCAN_TEST_MARGIN_CELLS * FIELD_GRID_CELL_Q12,
                                          ((int)grid->gridWidth + HEXSCAN_TEST_MARGIN_CELLS) * FIELD_GRID_CELL_Q12);
    *worldY = (Q12)(rowQ12 * (1 << 20) / FIELD_GRID_WORLD_Y_TO_ROW_Q20);
    *worldX = (Q12)((columnQ12 + rowQ12 / 2) * (1 << 20) / FIELD_GRID_WORLD_X_TO_COLUMN_Q20);
    *rowOut = (int)(rowQ12 >> 12);
    *columnOut = (int)(columnQ12 >> 12);
}

/* Plants one failing probe cell for the placement test of the profile near (row, column), within 1.5 times the
   walk's reach plus two cells (so about half of them lie outside the hexagon), unless that lands outside the
   interior. Returns the cell (NULL for none). */
static FieldGridCell *HexscanTest_PlantProbe(uint64_t *seed, FieldGridAsset *grid, int profile, int row, int column,
                                             FieldGridRadiusUnits radius, FieldGridCell *saved)
{
    int reach = (int)((uint32_t)radius / TERRAIN_SCAN_RADIUS_PER_STEP / TERRAIN_SCAN_STEP_STRAIGHT) * 3 / 2 + 2;
    int probeRow;
    int probeColumn;
    FieldGridCell *cell;

    if (reach > 60) {
        reach = 60;
    }
    probeRow = row + HexscanTest_Range(seed, -reach, reach + 1);
    probeColumn = column + HexscanTest_Range(seed, -reach, reach + 1);
    if (probeRow < 1 || probeColumn < 1 || probeRow >= (int)grid->gridHeight - 1 ||
        probeColumn >= (int)grid->gridWidth - 1) {
        return nullptr;
    }
    cell = &grid->cells[probeRow * (int)grid->gridWidth + probeColumn];
    *saved = *cell;
    if (profile == HEXSCAN_PROFILE_HEIGHTBAND) {
        switch (HexscanTest_Random(seed) % 3) {
        case 0: cell->waterSurfaceDelta = HexscanTest_Range(seed, 1, 0x3000); break;
        case 1: cell->terrainHeight = HexscanTest_Range(seed, 1500, 0x6000); break;
        default: cell->terrainHeight = HexscanTest_Range(seed, -0x6000, -1500); break;
        }
    }
    else {
        switch (HexscanTest_Random(seed) % 3) {
        case 0: cell->waterSurfaceDelta = HexscanTest_Range(seed, -0x3000, 0); break;
        case 1: cell->triangle1NormalAngles = HexscanTest_NormalAngles(seed, 1); break;
        default: cell->triangle0NormalAngles = HexscanTest_NormalAngles(seed, 1); break; /* only the centre reads it */
        }
    }
    return cell;
}

/* Counts the cells that differ from the reset grid. */
static unsigned HexscanTest_ChangedCells(const FieldGridAsset *grid, const FieldGridAsset *reset)
{
    unsigned changed = 0;
    uint32_t i;
    for (i = 0; i < grid->gridWidth * grid->gridHeight; i++) {
        if (memcmp(&grid->cells[i], &reset->cells[i], sizeof(FieldGridCell)) != 0) {
            changed++;
        }
    }
    return changed;
}

/* Runs one driver at HEXSCAN_TEST_POINTS points and logs its line; returns its hash. */
static uint32_t HexscanTest_RunDriver(int driver, FieldGridAsset *grid, FieldGridAsset *reset, size_t gridBytes)
{
    uint64_t seed = 0x6865787363616e00ull + (uint64_t)driver * 0x9e3779b97f4a7c15ull;
    int profile = driver == HEXSCAN_HEIGHTBAND ? HEXSCAN_PROFILE_HEIGHTBAND
                                               : (driver == HEXSCAN_AUX ? HEXSCAN_PROFILE_AUX : HEXSCAN_PROFILE_NOISE);
    int isTest = profile != HEXSCAN_PROFILE_NOISE;
    size_t cellBytes = grid->gridWidth * grid->gridHeight * sizeof(FieldGridCell);
    uint32_t hash = 2166136261u;
    unsigned changed = 0;
    unsigned trueCount = 0;
    int point;

    HexscanTest_FillGrid(reset, HexscanTest_Random64(&seed), profile);
    memcpy(grid, reset, gridBytes);
    for (point = 0; point < HEXSCAN_TEST_POINTS; point++) {
        FieldGridRadiusUnits radius = HexscanTest_Radius(&seed);
        Q12 worldY;
        Q12 worldX;
        int row;
        int column;
        HexscanTest_WorldPoint(&seed, grid, &worldY, &worldX, &row, &column);
        switch (driver) {
        case HEXSCAN_OVERLAY_A:
        case HEXSCAN_OVERLAY_B: {
            FieldCellFlagMask mask = 1u << HexscanTest_Range(&seed, 0, 24);
            TerrainOverlayCellRuntimeValue value = (TerrainOverlayCellRuntimeValue)HexscanTest_Random(&seed);
            Bool8 rejected = driver == HEXSCAN_OVERLAY_A
                                 ? FieldGridTerrainOverlayVariantA_ApplyAroundWorldPoint(mask, value, radius, worldY,
                                                                                         worldX, grid)
                                 : FieldGridTerrainOverlayVariantB_ApplyAroundWorldPoint(mask, value, radius, worldY,
                                                                                         worldX, grid);
            uint8_t result = rejected ? 1 : 0;
            hash = HexscanTest_Hash(hash, &result, 1);
            break;
        }
        case HEXSCAN_OCCUPANCY:
            TerrainOccupancyBit2_MarkAroundWorldPoint(radius, worldY, worldX, point % 8, grid);
            break;
        case HEXSCAN_FLATTEN:
            FieldGrid_ApplyHeightAtWorldPointAndRefreshNeighbors((TerrainHeightBrushDeltaSource)radius,
                                                                 HexscanTest_Range(&seed, -0x6000, 0x6000), worldY,
                                                                 worldX, grid);
            break;
        case HEXSCAN_SIGHT:
            TerrainProjectedOcclusion_AccumulateMaskAroundWorldPoint(
                (1ull << (point % 64)) | (HexscanTest_Random64(&seed) & 0x0101010101010101ull), radius,
                HexscanTest_Range(&seed, -0x4000, 0x8000), worldY, worldX, grid);
            break;
        default: {
            FieldGridCell saved[3];
            FieldGridCell *probes[3];
            int probeCount = HexscanTest_Range(&seed, 0, 3);
            int i;
            Q12 reference = HexscanTest_Range(&seed, -HEXSCAN_TEST_CLEAN_HEIGHT, HEXSCAN_TEST_CLEAN_HEIGHT + 1);
            int edgeDistance = row < column ? row : column;
            Bool8 rejected;
            uint8_t result;
            if ((int)grid->gridHeight - 1 - row < edgeDistance) {
                edgeDistance = (int)grid->gridHeight - 1 - row;
            }
            if ((int)grid->gridWidth - 1 - column < edgeDistance) {
                edgeDistance = (int)grid->gridWidth - 1 - column;
            }
            /* three in four calls with a radius that about reaches the edge ring, so most results depend on the
               probes and not on the ring */
            if (edgeDistance > 0 && HexscanTest_Random(&seed) % 4 != 0) {
                radius = HexscanTest_Range(&seed, 0, (edgeDistance * TERRAIN_SCAN_STEP_STRAIGHT + 2) *
                                                         TERRAIN_SCAN_RADIUS_PER_STEP);
            }
            for (i = 0; i < probeCount; i++) {
                probes[i] = HexscanTest_PlantProbe(&seed, grid, profile, row, column, radius, &saved[i]);
            }
            /* the placement tests take (x, y) but hand them to FieldGrid_WorldToGridQ12 as (y, x) */
            rejected = driver == HEXSCAN_HEIGHTBAND
                           ? TerrainHeightBand_TestAroundWorldPoint(radius, reference, worldY, worldX, grid)
                           : TerrainAuxHeightThreshold_TestAroundWorldPoint(radius, reference, worldY, worldX, grid);
            for (i = probeCount - 1; i >= 0; i--) {
                if (probes[i] != nullptr) {
                    *probes[i] = saved[i];
                }
            }
            result = rejected ? 1 : 0;
            trueCount += result;
            hash = HexscanTest_Hash(hash, &result, 1);
            break;
        }
        }
        if (!isTest && (point % HEXSCAN_TEST_CHUNK == HEXSCAN_TEST_CHUNK - 1 || point == HEXSCAN_TEST_POINTS - 1)) {
            hash = HexscanTest_Hash(hash, &grid->runtimeStateFlags, sizeof grid->runtimeStateFlags);
            hash = HexscanTest_Hash(hash, grid->cells, cellBytes);
            changed += HexscanTest_ChangedCells(grid, reset);
            memcpy(grid, reset, gridBytes);
        }
    }
    if (isTest) {
        /* the tests must not write: the planted probes are restored, so the grid equals its reset copy */
        hash = HexscanTest_Hash(hash, grid->cells, cellBytes);
        Thandor_Log("hexscan %s: %08X (%u true)", g_HexscanTestDriverNames[driver], hash, trueCount);
        if (memcmp(grid, reset, gridBytes) != 0) {
            Thandor_Log("hexscan %s: FAILED, the test changed the grid", g_HexscanTestDriverNames[driver]);
        }
    }
    else {
        Thandor_Log("hexscan %s: %08X (%u cells changed)", g_HexscanTestDriverNames[driver], hash, changed);
    }
    return hash;
}

void Thandor_SelfTestHexScan(void)
{
    /* the scan globals the drivers set, restored at the end */
    uint32_t savedRowStride = g_TerrainScanRowStrideBytes;
    uint32_t savedStepLimit = g_TerrainScanStepLimit;
    TerrainScanSelectorUnion savedSelector = g_TerrainScanSharedSelectorValue;
    uint32_t savedReferenceHeight = g_TerrainScanReferenceHeight;
    size_t gridBytes =
        offsetof(FieldGridAsset, cells) + (size_t)HEXSCAN_TEST_WIDTH * HEXSCAN_TEST_HEIGHT * sizeof(FieldGridCell);
    /* two zeroed grid images: the asset header followed by the cells (a variable-length record) */
    std::vector<uint32_t> gridStorage((gridBytes + 3) / 4);
    std::vector<uint32_t> resetStorage((gridBytes + 3) / 4);
    auto *grid = reinterpret_cast<FieldGridAsset *>(gridStorage.data());
    auto *reset = reinterpret_cast<FieldGridAsset *>(resetStorage.data());
    uint32_t summary = 2166136261u;
    int driver;

    grid->gridWidth = reset->gridWidth = HEXSCAN_TEST_WIDTH;
    grid->gridHeight = reset->gridHeight = HEXSCAN_TEST_HEIGHT;
    for (driver = 0; driver < HEXSCAN_DRIVER_COUNT; driver++) {
        uint32_t hash = HexscanTest_RunDriver(driver, grid, reset, gridBytes);
        summary = HexscanTest_Hash(summary, &hash, sizeof hash);
    }
    Thandor_Log("hexscan: %d drivers, %d points each, %dx%d grid, summary %08X", HEXSCAN_DRIVER_COUNT,
                HEXSCAN_TEST_POINTS, HEXSCAN_TEST_WIDTH, HEXSCAN_TEST_HEIGHT, summary);
    g_TerrainScanRowStrideBytes = savedRowStride;
    g_TerrainScanStepLimit = savedStepLimit;
    g_TerrainScanSharedSelectorValue = savedSelector;
    g_TerrainScanReferenceHeight = savedReferenceHeight;
}

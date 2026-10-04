/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/src/world/terrain/field_lighting.cpp
 * Reverse engineering by idkFoxes 2026
 */

/* Field-grid lighting: cell triangle normals, the directional light colour per cell and the animated
   direction record table. */

#include <thandor/world/terrain/field_lighting.h>
#include <thandor/thandor.h>
#include <thandor/platform/bootstrap/image.h>

/* Module data. */

/* Q28 unit vector */
static GraphicsFixedVec3 g_TerrainLightDirection = {0};

TerrainDirectionRecord g_TerrainDirectionRecordTable256[256] = {0};

/* entries 0..255 the shaded colour ramp (originally
   g_TerrainLightingColorRampArgb256), entries 256..512 the lit half; indexed by the signed dot
   product -256..256 from entry 256 */
PackedArgb32 g_TerrainDirectionalLightColorLut[513] = {0};

uint32_t g_TerrainDirectionalLightSecondaryColor = 0;

/* Recomputes the packed normal angles of both terrain triangles of every interior cell (the one-cell
   border ring is skipped) after the heights changed, and marks the field grid dirty (runtimeStateFlags
   bit 0).
*/
void FieldGrid_RecomputeInteriorTriangleNormalAngles(FieldGridAsset *fieldGrid)

{
  FieldGridDimension rowLength;
  int columnsLeft;
  int rowsLeft;
  FieldGridCell *cell;
  FieldGridCell *cellCursor;

  if (fieldGrid != NULL) {
    fieldGrid->runtimeStateFlags = fieldGrid->runtimeStateFlags | 1;
    rowLength = fieldGrid->gridWidth;
    rowsLeft = fieldGrid->gridHeight - 2;
    columnsLeft = rowLength - 2;
    /* cell (row 1, column 1) */
    cellCursor = &fieldGrid->cells[rowLength + 1];
    do {
      do {
        cell = cellCursor;
        FieldGridCell_RecomputeTriangleNormalAngles(rowLength * sizeof(FieldGridCell),cell); /* row stride in bytes */
        columnsLeft--;
        cellCursor = cell + 1;
      } while (columnsLeft != 0);
      columnsLeft = rowLength - 2;
      rowsLeft--;
      cellCursor = cell + 3; /* skip the right border cell of this row and the left one of the next */
    } while (rowsLeft != 0);
  }
}

/* Sets the terrain light direction (g_TerrainLightDirection, Q28) from an elevation and azimuth
   and relights every interior cell with it (border ring skipped); marks the field grid dirty.
*/
void FieldGrid_RecomputeInteriorDirectionalLighting
          (AngleTurn32 lightElevationAngle,AngleTurn32 lightAzimuthAngle,FieldGridAsset *fieldGrid)

{
  FieldGridDimension rowLength;
  int columnsLeft;
  int rowsLeft;
  FieldGridCell *cell;
  FieldGridCell *cellCursor;

  FixedMath_WriteDirectionQ28
            (&g_TerrainLightDirection,lightElevationAngle,lightAzimuthAngle);
  if (fieldGrid != NULL) {
    fieldGrid->runtimeStateFlags = fieldGrid->runtimeStateFlags | 1;
    rowLength = fieldGrid->gridWidth;
    rowsLeft = fieldGrid->gridHeight - 2;
    columnsLeft = rowLength - 2;
    /* cell (row 1, column 1), see FieldGrid_RecomputeInteriorTriangleNormalAngles */
    cellCursor = &fieldGrid->cells[rowLength + 1];
    do {
      do {
        cell = cellCursor;
        FieldGridCell_ComputeDirectionalLightColor(cell);
        columnsLeft--;
        cellCursor = cell + 1;
      } while (columnsLeft != 0);
      columnsLeft = rowLength - 2;
      rowsLeft--;
      cellCursor = cell + 3;
    } while (rowsLeft != 0);
  }
}

/* Animates the 256 terrain direction records (random rates and scales from TerrainVisualResources_LoadPrimary):
   both 16-bit angles advance by their rates, and the scaled sine/cosine of angle A and one component of
   angle B are stored, computed from the angles before this step.
*/
void TerrainDirectionTable_AdvanceAndRebuildVectors(void)

{
  uint32_t previousPackedAngles;
  TerrainDirectionRecordCount recordsRemaining;
  TerrainDirectionRecord *currentDirectionRecord;
  FixedSinCos scaledSinCosPair;
  FixedSinCos angleBScaledSinCosPair;

  currentDirectionRecord = g_TerrainDirectionRecordTable256;
  recordsRemaining = 256;
  do {
    previousPackedAngles = currentDirectionRecord->packedAngles;
    /* one 32-bit add advances both packed angles by rateA (low word) and rateB (high word); a carry out
       of angle A moves angle B by one more */
    currentDirectionRecord->packedAngles = currentDirectionRecord->packedAngles + *(int *)&currentDirectionRecord->rateA;
    scaledSinCosPair = FixedMath_SinCosScaled(previousPackedAngles & FIXED_ANGLE16_MASK,currentDirectionRecord->scaleA);
    currentDirectionRecord->angleAComponent0ScaledQ28 = scaledSinCosPair.cosValue;
    currentDirectionRecord->angleAComponent1ScaledQ28 = scaledSinCosPair.sinValue;
    angleBScaledSinCosPair =
         FixedMath_SinCosScaled((int)previousPackedAngles >> 16,currentDirectionRecord->scaleB);
    currentDirectionRecord->angleBComponent0ScaledQ28 = (uint32_t)angleBScaledSinCosPair.cosValue;
    currentDirectionRecord++;
    recordsRemaining--;
  } while (recordsRemaining != 0);
}

/* Recomputes a cell's two vertex normals from its six lattice neighbours and stores them as packed
   (azimuth | elevation << 16) angle pairs: triangle0NormalAngles for the terrain surface and
   triangle1NormalAngles for the secondary surface (terrainHeight + waterSurfaceDelta). The lighting in
   FieldGridCell_ComputeDirectionalLightColor reads the first one.
*/
void FieldGridCell_RecomputeTriangleNormalAngles(FieldGridRowStrideBytes rowStrideBytes,FieldGridCell *cell)

{
  int rightDelta;
  int neighborDeltaA;
  int neighborDeltaB;
  int neighborDeltaC;
  int neighborDeltaD;
  int neighborDeltaE;
  int neighborDeltaF;
  FixedVectorAngles normalAngles;

  /* The six neighbours of the triangular lattice, addressed as raw byte offsets from the cell (0x80 bytes per
     cell, rowStrideBytes per row; worldX +0x40, worldY +0x44, terrainHeight +0x48, waterSurfaceDelta +0x4C):
       cell[1] right, cell[-1] left, +rowStrideBytes below, -rowStrideBytes above,
       +rowStrideBytes - 0x80 below-left, -rowStrideBytes + 0x80 above-right.
     Each normal is (-sum(dX * dH), -sum(dY * dH), 0xC00000) over the neighbours. First pass (terrain):
     rightDelta = right, A = below, B = above, C = left, D = below-left, E = above-right. Second pass (surface):
     A = right, B = below, C = above, D = left, E = below-left, F = above-right. */
  rightDelta = cell[1].terrainHeight - cell->terrainHeight;
  neighborDeltaA = FIELD_GRID_CELL_AT_BYTE_OFFSET(cell,rowStrideBytes)->terrainHeight - cell->terrainHeight;
  neighborDeltaB = FIELD_GRID_CELL_AT_BYTE_OFFSET(cell,-rowStrideBytes)->terrainHeight - cell->terrainHeight;
  neighborDeltaC = cell[-1].terrainHeight - cell->terrainHeight;
  neighborDeltaD = FIELD_GRID_CELL_AT_BYTE_OFFSET(cell,rowStrideBytes)[-1].terrainHeight - cell->terrainHeight;
  neighborDeltaE = FIELD_GRID_CELL_AT_BYTE_OFFSET(cell,-rowStrideBytes)[1].terrainHeight - cell->terrainHeight;
  normalAngles = FixedMath_VectorToAngles
                    (FIELD_GRID_NORMAL_Z_COMPONENT,((((-((FIELD_GRID_CELL_AT_BYTE_OFFSET(cell,rowStrideBytes)->worldY -
                                    cell->worldY) * neighborDeltaA) - (cell[1].worldY - cell->worldY) * rightDelta
                                 ) - (FIELD_GRID_CELL_AT_BYTE_OFFSET(cell,-rowStrideBytes)->worldY - cell->worldY)
                                     * neighborDeltaB) - (cell[-1].worldY - cell->worldY) * neighborDeltaC) -
                              (FIELD_GRID_CELL_AT_BYTE_OFFSET(cell,rowStrideBytes)[-1].worldY - cell->worldY)
                              * neighborDeltaD) -
                              (FIELD_GRID_CELL_AT_BYTE_OFFSET(cell,-rowStrideBytes)[1].worldY - cell->worldY) * neighborDeltaE
                     ,((((-((FIELD_GRID_CELL_AT_BYTE_OFFSET(cell,rowStrideBytes)->worldX - cell->worldX) *
                           neighborDeltaA) - (cell[1].worldX - cell->worldX) * rightDelta) -
                        (FIELD_GRID_CELL_AT_BYTE_OFFSET(cell,-rowStrideBytes)->worldX - cell->worldX) * neighborDeltaB) -
                       (cell[-1].worldX - cell->worldX) * neighborDeltaC) -
                      (FIELD_GRID_CELL_AT_BYTE_OFFSET(cell,rowStrideBytes)[-1].worldX - cell->worldX) * neighborDeltaD
                      ) - (FIELD_GRID_CELL_AT_BYTE_OFFSET(cell,-rowStrideBytes)[1].worldX - cell->worldX) * neighborDeltaE);
  cell->triangle0NormalAngles = normalAngles.azimuthAngle | normalAngles.elevationAngle << 16;
  neighborDeltaA = ((cell[1].terrainHeight + cell[1].waterSurfaceDelta) - cell->terrainHeight) -
          cell->waterSurfaceDelta;
  neighborDeltaB = ((FIELD_GRID_CELL_AT_BYTE_OFFSET(cell,rowStrideBytes)->terrainHeight +
           FIELD_GRID_CELL_AT_BYTE_OFFSET(cell,rowStrideBytes)->waterSurfaceDelta) - cell->terrainHeight) -
          cell->waterSurfaceDelta;
  neighborDeltaC = ((FIELD_GRID_CELL_AT_BYTE_OFFSET(cell,-rowStrideBytes)->terrainHeight +
           FIELD_GRID_CELL_AT_BYTE_OFFSET(cell,-rowStrideBytes)->waterSurfaceDelta) -
          cell->terrainHeight) - cell->waterSurfaceDelta;
  neighborDeltaD = ((cell[-1].terrainHeight + cell[-1].waterSurfaceDelta) - cell->terrainHeight) -
          cell->waterSurfaceDelta;
  neighborDeltaE = ((FIELD_GRID_CELL_AT_BYTE_OFFSET(cell,rowStrideBytes)[-1].terrainHeight +
           FIELD_GRID_CELL_AT_BYTE_OFFSET(cell,rowStrideBytes)[-1].waterSurfaceDelta) - cell->terrainHeight) -
          cell->waterSurfaceDelta;
  neighborDeltaF = ((FIELD_GRID_CELL_AT_BYTE_OFFSET(cell,-rowStrideBytes)[1].terrainHeight +
           FIELD_GRID_CELL_AT_BYTE_OFFSET(cell,-rowStrideBytes)[1].waterSurfaceDelta) -
          cell->terrainHeight) - cell->waterSurfaceDelta;
  normalAngles = FixedMath_VectorToAngles
                    (FIELD_GRID_NORMAL_Z_COMPONENT,((((-((FIELD_GRID_CELL_AT_BYTE_OFFSET(cell,rowStrideBytes)->worldY -
                                    cell->worldY) * neighborDeltaB) - (cell[1].worldY - cell->worldY) * neighborDeltaA
                                 ) - (FIELD_GRID_CELL_AT_BYTE_OFFSET(cell,-rowStrideBytes)->worldY - cell->worldY) * neighborDeltaC) -
                               (cell[-1].worldY - cell->worldY) * neighborDeltaD) -
                              (FIELD_GRID_CELL_AT_BYTE_OFFSET(cell,rowStrideBytes)[-1].worldY - cell->worldY)
                              * neighborDeltaE) - (FIELD_GRID_CELL_AT_BYTE_OFFSET(cell,-rowStrideBytes)[1].worldY - cell->worldY) * neighborDeltaF
                     ,((((-((FIELD_GRID_CELL_AT_BYTE_OFFSET(cell,rowStrideBytes)->worldX - cell->worldX) *
                           neighborDeltaB) - (cell[1].worldX - cell->worldX) * neighborDeltaA) -
                        (FIELD_GRID_CELL_AT_BYTE_OFFSET(cell,-rowStrideBytes)->worldX - cell->worldX) * neighborDeltaC) -
                       (cell[-1].worldX - cell->worldX) * neighborDeltaD) -
                      (FIELD_GRID_CELL_AT_BYTE_OFFSET(cell,rowStrideBytes)[-1].worldX - cell->worldX) * neighborDeltaE
                      ) - (FIELD_GRID_CELL_AT_BYTE_OFFSET(cell,-rowStrideBytes)[1].worldX - cell->worldX) * neighborDeltaF);
  cell->triangle1NormalAngles = normalAngles.azimuthAngle | normalAngles.elevationAngle << 16;
}

/* Diffuse terrain lighting for one cell: turns the terrain normal (triangle0NormalAngles) back into a Q28
   direction, dots it with the global light direction and looks the result up in the directional light colour
   table (groundDirectionalLightColor). The secondary surface always gets the one fixed secondary colour
   (secondarySurfaceDirectionalLightColor).
*/
void FieldGridCell_ComputeDirectionalLightColor(FieldGridCell *cell)

{
  FixedDirection normalDirection;
  PackedArgb32 directionalLightColor;

  /* packed as azimuth (low word) | elevation (high word) */
  normalDirection = FixedMath_DirectionFromAnglesQ28
                    ((int)cell->triangle0NormalAngles >> 16,cell->triangle0NormalAngles & FIXED_ANGLE16_MASK);
  /* the signed Q8 dot product (-256..256) indexes the table from its middle entry; the shaded ramp is the
     lower half */
  directionalLightColor =
       g_TerrainDirectionalLightColorLut
       [TERRAIN_DIRECTIONAL_LIGHT_LUT_ZERO_INDEX +
        (((int)((uint64_t)((int64_t)(int)normalDirection.x * (int64_t)g_TerrainLightDirection.x) >> 32) +
          (int)((uint64_t)((int64_t)(int)normalDirection.y * (int64_t)g_TerrainLightDirection.y) >> 32) +
          (int)((uint64_t)((int64_t)(int)normalDirection.z * (int64_t)g_TerrainLightDirection.z) >> 32)) >>
         16)];
  cell->secondarySurfaceDirectionalLightColor = g_TerrainDirectionalLightSecondaryColor;
  cell->groundDirectionalLightColor = directionalLightColor;
}

/* Sets up the terrain lighting colours: the shaded half of g_TerrainDirectionalLightColorLut gets
   [i] = base + ramp * (256 - i) / 256 per colour channel (saturated at 0xFF, alpha taken from base), the lit
   half (from TERRAIN_DIRECTIONAL_LIGHT_LUT_ZERO_INDEX) is filled with the base colour and the secondary colour
   is stored in g_TerrainDirectionalLightSecondaryColor.
*/
void TerrainLighting_BuildColorRampAndSetBaseColor
          (PackedArgb32 secondaryColorArgb,PackedArgb32 baseColorArgb,PackedArgb32 rampStepColorArgb
          )

{
  uint32_t channelValue;
  int rampStepsRemaining;
  uint32_t *rampEntryCursor;
  PackedArgb32 *lightLutCursor;
  
  rampEntryCursor = g_TerrainDirectionalLightColorLut;
  rampStepsRemaining = TERRAIN_LIGHTING_RAMP_ENTRY_COUNT;
  do {
    channelValue = ((rampStepColorArgb & ARGB8888_BLUE_MASK) * rampStepsRemaining >> 8) + (baseColorArgb & ARGB8888_BLUE_MASK);
    if (ARGB8888_BLUE_MASK < channelValue) {
      channelValue = ARGB8888_BLUE_MASK;
    }
    *rampEntryCursor = channelValue;
    rampEntryCursor++;
    rampStepsRemaining--;
  } while (rampStepsRemaining != 0);
  rampEntryCursor = g_TerrainDirectionalLightColorLut;
  rampStepsRemaining = TERRAIN_LIGHTING_RAMP_ENTRY_COUNT;
  do {
    channelValue = ((rampStepColorArgb & ARGB8888_GREEN_MASK) * rampStepsRemaining >> 8) + (baseColorArgb & ARGB8888_GREEN_MASK);
    if (0xffff < channelValue) {
      channelValue = ARGB8888_GREEN_MASK;
    }
    *rampEntryCursor = *rampEntryCursor | channelValue & ARGB8888_GREEN_MASK;
    rampEntryCursor++;
    rampStepsRemaining--;
  } while (rampStepsRemaining != 0);
  rampEntryCursor = g_TerrainDirectionalLightColorLut;
  rampStepsRemaining = TERRAIN_LIGHTING_RAMP_ENTRY_COUNT;
  do {
    channelValue = ((rampStepColorArgb & ARGB8888_RED_MASK) * rampStepsRemaining >> 8) + (baseColorArgb & ARGB8888_RED_MASK);
    if (0xffffff < channelValue) {
      channelValue = ARGB8888_RED_MASK;
    }
    *rampEntryCursor = *rampEntryCursor | channelValue & ARGB8888_RED_MASK;
    rampEntryCursor++;
    rampStepsRemaining--;
  } while (rampStepsRemaining != 0);
  rampEntryCursor = g_TerrainDirectionalLightColorLut;
  rampStepsRemaining = TERRAIN_LIGHTING_RAMP_ENTRY_COUNT;
  do {
    *rampEntryCursor = *rampEntryCursor | baseColorArgb & ARGB8888_ALPHA_MASK;
    rampEntryCursor++;
    rampStepsRemaining--;
  } while (rampStepsRemaining != 0);
  g_TerrainDirectionalLightSecondaryColor = secondaryColorArgb;
  lightLutCursor = &g_TerrainDirectionalLightColorLut[TERRAIN_DIRECTIONAL_LIGHT_LUT_ZERO_INDEX];
  for (rampStepsRemaining = TERRAIN_DIRECTIONAL_LIGHT_LUT_LIT_ENTRY_COUNT; rampStepsRemaining != 0;
       rampStepsRemaining--) {
    *lightLutCursor = baseColorArgb;
    lightLutCursor++;
  }
}

/* In-game command 0x2D70 (INGAME_COMMAND_EDITOR_TURN_LIGHT; issued by Ctrl editor hotkeys in
   ui/ingame/runtime.c with steps of +-0x400): turns the terrain light and relights the field region. The
   elevation (the root's lightElevationAngle) is kept between -0x4000 (straight down) and -0x1000, the
   azimuth (lightAzimuthAngle) wraps around.
*/
void TerrainLighting_AdjustDirectionAndRecomputeField
          (uint32_t playerRuntimeId,uint32_t reservedZero,uint32_t deltaElevationAngle,
          uint32_t deltaAzimuthAngle)

{
  Q12 lightElevationAngle;

  lightElevationAngle = deltaElevationAngle + g_InGameRuntimeRoot->lightElevationAngle;
  if (-TERRAIN_LIGHT_ELEVATION_MIN_TILT_ANGLE16 < lightElevationAngle) {
    lightElevationAngle = -TERRAIN_LIGHT_ELEVATION_MIN_TILT_ANGLE16;
  }
  if (lightElevationAngle < -FIXED_ANGLE16_QUARTER_TURN) {
    lightElevationAngle = -FIXED_ANGLE16_QUARTER_TURN;
  }
  WorldRuntime_RecomputeFieldRegionNormalsAndLighting
            ((g_InGameRuntimeRoot->worldRuntime).fieldRegion.auxiliaryElevationAngle,
             (g_InGameRuntimeRoot->worldRuntime).fieldRegion.auxiliaryAzimuthAngle,lightElevationAngle,
             deltaAzimuthAngle + g_InGameRuntimeRoot->lightAzimuthAngle & FIXED_ANGLE16_MASK,
             &g_InGameRuntimeRoot->worldRuntime);
  return;
}

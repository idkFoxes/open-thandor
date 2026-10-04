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
#include <thandor/core/color_lanes.h>
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

  if (fieldGrid != nullptr) {
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
  if (fieldGrid != nullptr) {
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

/* Not a function of its own in the original: the inlined MMX sequence that blends one colour pair of
   WorldLightingRuntime_UpdateInterpolatedTerrainLighting per byte, color * forward + alternateColor * inverse
   with unsigned saturation (PUNPCKLBW/PSRLW 6 of both colours, PMULHW by the factors, PADDW, PACKUSWB). */
static __inline uint32_t WorldLighting_BlendColors
          (uint32_t color,uint32_t alternateColor,SoftwareBgraWordLanes forwardFactors,
          SoftwareBgraWordLanes inverseFactors)

{
  ThandorMmx forwardTerm;
  ThandorMmx inverseTerm;
  uint32_t packed;
  short sum;
  int lane;

  forwardTerm.q = pmulhw(ColorLanes_UnpackBytesShiftRight(color,6),forwardFactors);
  inverseTerm.q = pmulhw(ColorLanes_UnpackBytesShiftRight(alternateColor,6),inverseFactors);
  packed = 0;
  for (lane = 0; lane < 4; lane++) {
    sum = (short)(forwardTerm.sw[lane] + inverseTerm.sw[lane]);
    packed = packed | (uint32_t)(sum < 0 ? 0 : (0xff < sum ? 0xff : sum)) << (lane * 8);
  }
  return packed;
}

/* Triangular blend of two 16-bit values over the phase byte (0 = primary, 0x80 = alternate, back towards
   primary at 0xff); the value that would lie below the other one gets WORLD_LIGHTING_PACKED_HALF_WRAP
   added, so the blend runs forward through the 16-bit wrap. Returns the low 16 bits of the result. */
static uint32_t WorldLighting_BlendPackedLow16(uint32_t primaryValue,uint32_t alternateValue,uint32_t phaseByte)

{
  int primaryWeighted;
  int alternateWeighted;

  if (phaseByte < 128) {
    if (alternateValue < primaryValue) {
      alternateValue = alternateValue + WORLD_LIGHTING_PACKED_HALF_WRAP;
    }
    alternateWeighted = alternateValue * phaseByte;
    primaryWeighted = primaryValue * (128 - phaseByte);
  }
  else {
    if (primaryValue < alternateValue) {
      primaryValue = primaryValue + WORLD_LIGHTING_PACKED_HALF_WRAP;
    }
    primaryWeighted = primaryValue * (phaseByte - 128);
    alternateWeighted = alternateValue * (128 - (phaseByte - 128));
  }
  return (uint32_t)(primaryWeighted + alternateWeighted) >> 7 & 0xffff;
}

/* Periodic terrain lighting cycle (tick-wheel case 0, plus two session setup paths): when the level
   defines a cycle duration, the simulation tick's phase in the cycle picks a cosine blend between the
   level's primary and alternate terrain colour sets and between two packed 16-bit parameter pairs,
   installs the blended colours and recomputes the terrain normals and lighting with the blended pairs.
   At phase 0 the blend index is 256 if the cosine table holds exactly 1.0 there: one past the declared
   256-entry factor tables (as in the original).
*/
void WorldLightingRuntime_UpdateInterpolatedTerrainLighting(void)

{
  SoftwareBgraWordLanes forwardFactors;
  SoftwareBgraWordLanes inverseFactors;
  struct LevelWorldSettings *settings;
  uint32_t mixedColor0A;
  uint32_t mixedColor0B;
  uint32_t mixedColor1A;
  uint32_t mixedColor1B;
  uint32_t mixedColor2A;
  uint32_t mixedColor2B;
  uint32_t mixedColor3A;
  uint32_t mixedColor3B;
  uint32_t cycleDuration;
  uint32_t phase;
  uint32_t phaseByte;
  uint32_t blendIndex;
  uint32_t blendWeight;
  int inverseBlendWeight;
  uint32_t blendedLightAzimuth;
  uint32_t blendedAuxiliaryAzimuth;
  WorldRuntimeContext *worldRuntime;

  settings = &g_InGameLevelRuntimeGlobalBlock.conditionStorage->levelImage.worldSettings;
  cycleDuration = settings->terrainLightingCycleDurationTicks;
  worldRuntime = &g_InGameRuntimeRoot->worldRuntime;
  if (cycleDuration != 0) {
    /* phase in the cycle as a 16-bit angle; its cosine (Q28, -1..1) becomes a blend index 0..256 */
    phase = (g_GameFactionRuntimeImage.tail.simulationTick % cycleDuration << 16) / cycleDuration;
    blendIndex = (g_FixedSineQ28[FIXED_SINE_TABLE_COS + phase] + (uint32_t)Q28_ONE) >> 21;
    forwardFactors = g_SoftwareBilinearForwardFactors[blendIndex];
    inverseFactors = g_SoftwareBilinearInverseFactors[blendIndex];
    mixedColor0A = WorldLighting_BlendColors(settings->terrainRampStepColorArgb,
                                             settings->alternateTerrainRampStepColorArgb,
                                             forwardFactors,inverseFactors);
    mixedColor0B = WorldLighting_BlendColors(settings->terrainBaseColorArgb,
                                             settings->alternateTerrainBaseColorArgb,
                                             forwardFactors,inverseFactors);
    mixedColor1A = WorldLighting_BlendColors(settings->terrainLightingColor128Argb,
                                             settings->alternateTerrainLightingColor128Argb,
                                             forwardFactors,inverseFactors);
    mixedColor1B = WorldLighting_BlendColors(settings->terrainSecondaryColorArgb,
                                             settings->alternateTerrainSecondaryColorArgb,
                                             forwardFactors,inverseFactors);
    mixedColor2A = WorldLighting_BlendColors(settings->terrainLightingColor130Argb,
                                             settings->alternateTerrainLightingColor130Argb,
                                             forwardFactors,inverseFactors);
    mixedColor2B = WorldLighting_BlendColors(settings->terrainLightingColor134Argb,
                                             settings->alternateTerrainLightingColor134Argb,
                                             forwardFactors,inverseFactors);
    mixedColor3A = WorldLighting_BlendColors(settings->terrainLightingColor138Argb,
                                             settings->alternateTerrainLightingColor138Argb,
                                             forwardFactors,inverseFactors);
    mixedColor3B = WorldLighting_BlendColors(settings->terrainLightingColor13CArgb,
                                             settings->alternateTerrainLightingColor13CArgb,
                                             forwardFactors,inverseFactors);
    /* A colors without alpha; B colors opaque, except the secondary colour keeps its alpha */
    WorldRuntime_SetTerrainLightingConfiguration
              (mixedColor3B | 0xff000000,mixedColor3A & 0xffffff,mixedColor2B | 0xff000000,
               mixedColor2A & 0xffffff,mixedColor1B | settings->terrainSecondaryColorArgb & 0xff000000,
               mixedColor1A & 0xffffff,mixedColor0B | 0xff000000,mixedColor0A & 0xffffff,worldRuntime);
    /* The packed pairs hold the light direction (origin pair: elevation high, azimuth low) and the auxiliary
       angles (height/width pair). Low 16 bits: triangular blend over the phase byte (see
       WorldLighting_BlendPackedLow16). High 16 bits: the same cosine weight as the colours. */
    phaseByte = phase >> 8;
    blendedLightAzimuth =
         WorldLighting_BlendPackedLow16((uint16_t)settings->packedFieldRegionOriginYHigh16XLow16,
                                        (uint16_t)settings->alternatePackedFieldRegionOriginYHigh16XLow16,
                                        phaseByte);
    blendWeight = (g_FixedSineQ28[FIXED_SINE_TABLE_COS + phase] + (uint32_t)Q28_ONE) >> 21;
    inverseBlendWeight = 256 - blendWeight;
    blendedAuxiliaryAzimuth =
         WorldLighting_BlendPackedLow16((uint16_t)settings->packedFieldRegionHeightHigh16WidthLow16,
                                        (uint16_t)settings->alternatePackedFieldRegionHeightHigh16WidthLow16,
                                        phaseByte);
    /* ((uint16_t *)&pair)[1]: the high 16 bits of a packed pair */
    WorldRuntime_RecomputeFieldRegionNormalsAndLighting
              ((int)((uint32_t)((uint16_t *)&settings->alternatePackedFieldRegionHeightHigh16WidthLow16)[1] *
                     inverseBlendWeight +
                     (uint32_t)((uint16_t *)&settings->packedFieldRegionHeightHigh16WidthLow16)[1] *
                     (256 - inverseBlendWeight)) >> 8,
               blendedAuxiliaryAzimuth,
               (int)((uint32_t)((uint16_t *)&settings->alternatePackedFieldRegionOriginYHigh16XLow16)[1] *
                     inverseBlendWeight +
                     ((uint16_t *)&settings->packedFieldRegionOriginYHigh16XLow16)[1] * blendWeight) >> 8,
               blendedLightAzimuth,worldRuntime);
  }
}

/* Keyboard command of the in-game root (called directly in a local game, in a networked one queued as
   command 0x2D00 from InGameUiRootKeyboardFallback_DispatchCommandByCodeAndModifierFlags): turns the
   auxiliary angle pair (stored in fieldRegion.auxiliaryElevationAngle/auxiliaryAzimuthAngle, see
   WorldRuntime_RecomputeFieldRegionNormalsAndLighting) by the given deltas, the elevation clamped to
   -0x4000..-0x1000 and the azimuth wrapped to 16 bits, and relights the field with the unchanged light
   direction. The name is historical: nothing here is a field origin.
*/
void WorldRuntime_TurnAuxiliaryAnglesClamped
          (PlayerRuntimeId playerRuntimeId,uint32_t reservedZero,Q12 deltaElevationAngle,Q12 deltaAzimuthAngle)

{
  FieldGridDimensionCells auxiliaryElevationAngle;

  auxiliaryElevationAngle =
       deltaElevationAngle + g_InGameRuntimeRoot->worldRuntime.fieldRegion.auxiliaryElevationAngle;
  if (WORLD_AUXILIARY_ELEVATION_MAXIMUM < auxiliaryElevationAngle) {
    auxiliaryElevationAngle = WORLD_AUXILIARY_ELEVATION_MAXIMUM;
  }
  if (auxiliaryElevationAngle < WORLD_AUXILIARY_ELEVATION_MINIMUM) {
    auxiliaryElevationAngle = WORLD_AUXILIARY_ELEVATION_MINIMUM;
  }
  WorldRuntime_RecomputeFieldRegionNormalsAndLighting
            (auxiliaryElevationAngle,
             deltaAzimuthAngle + g_InGameRuntimeRoot->worldRuntime.fieldRegion.auxiliaryAzimuthAngle & FIXED_ANGLE16_MASK,
             g_InGameRuntimeRoot->lightElevationAngle,
             g_InGameRuntimeRoot->lightAzimuthAngle,
             &g_InGameRuntimeRoot->worldRuntime);
  return;
}

/* Stores the eight terrain lighting colours of the level (or of the current lighting-cycle blend) in the world
   runtime and rebuilds the terrain colour ramp from the base colour, the ramp-step colour and the secondary colour.
*/
void WorldRuntime_SetTerrainLightingConfiguration(PackedArgb32 lightingColor13CArgb,PackedArgb32 lightingColor138Argb,
          PackedArgb32 lightingColor134Argb,PackedArgb32 lightingColor130Argb,
          PackedArgb32 secondaryColorArgb,PackedArgb32 lightingColor128Argb,
          PackedArgb32 baseColorArgb,PackedArgb32 rampStepColorArgb,WorldRuntimeContext *worldRuntime
          )

{
  worldRuntime->lighting.color130Argb = lightingColor130Argb;
  worldRuntime->lighting.color134Argb = lightingColor134Argb;
  worldRuntime->lighting.color128Argb = lightingColor128Argb;
  worldRuntime->lighting.color138Argb = lightingColor138Argb;
  worldRuntime->lighting.color13CArgb = lightingColor13CArgb;
  worldRuntime->lighting.rampStepColorArgb = rampStepColorArgb;
  worldRuntime->lighting.baseColorArgb = baseColorArgb;
  worldRuntime->lighting.secondaryColorArgb = secondaryColorArgb;
  TerrainLighting_BuildColorRampAndSetBaseColor(secondaryColorArgb,baseColorArgb,rampStepColorArgb);
  return;
}

/* Sets the terrain light direction (elevation, azimuth) and relights the field: recomputes the triangle normals
   and the directional lighting of the field grid. The auxiliary angle pair is only stored (in
   fieldRegion.auxiliaryElevationAngle/auxiliaryAzimuthAngle; callers clamp and wrap it like the light direction, elevation
   -0x4000..-0x1000, azimuth & 0xFFFF). Callers: level load, the periodic lighting cycle and the light-direction
   commands.
*/
void WorldRuntime_RecomputeFieldRegionNormalsAndLighting
          (FieldGridDimensionCells auxiliaryElevationAngle,FieldGridDimensionCells auxiliaryAzimuthAngle,
          Q12 lightElevationAngle,Q12 lightAzimuthAngle,WorldRuntimeContext *worldRuntime)

{
  /* worldRuntime is the world embedded in the in-game root; the light angles are stored in the root */
  THANDOR_CONTAINER_OF(worldRuntime, InGameRuntimeRoot, worldRuntime)->
       lightAzimuthAngle = lightAzimuthAngle;
  THANDOR_CONTAINER_OF(worldRuntime, InGameRuntimeRoot, worldRuntime)->
       lightElevationAngle = lightElevationAngle;
  FieldGrid_RecomputeInteriorTriangleNormalAngles(worldRuntime->fieldGrid);
  FieldGrid_RecomputeInteriorDirectionalLighting
            (lightElevationAngle,lightAzimuthAngle,worldRuntime->fieldGrid);
  worldRuntime->fieldRegion.auxiliaryAzimuthAngle = auxiliaryAzimuthAngle;
  worldRuntime->fieldRegion.auxiliaryElevationAngle = auxiliaryElevationAngle;
  return;
}

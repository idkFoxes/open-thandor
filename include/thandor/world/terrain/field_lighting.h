/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/include/thandor/world/terrain/field_lighting.h
 * Reverse engineering by idkFoxes 2026
 */

#ifndef THANDOR_WORLD_TERRAIN_FIELD_LIGHTING_H
#define THANDOR_WORLD_TERRAIN_FIELD_LIGHTING_H

#include <thandor/generated/types.h>
#include <thandor/core/contracts.h>

/* g_TerrainDirectionalLightColorLut (TerrainLighting_BuildColorRampAndSetBaseColor,
   FieldGridCell_ComputeDirectionalLightColor) is indexed by the signed Q8 dot product of the cell normal and
   the light direction (-256..256) from its middle entry: the first RAMP_ENTRY_COUNT entries (dot -256..-1) hold
   the shaded colour ramp, the following LIT_ENTRY_COUNT entries (dot 0..256) the base colour. */
#define TERRAIN_LIGHTING_RAMP_ENTRY_COUNT 256
#define TERRAIN_DIRECTIONAL_LIGHT_LUT_LIT_ENTRY_COUNT 257
#define TERRAIN_DIRECTIONAL_LIGHT_LUT_ZERO_INDEX TERRAIN_LIGHTING_RAMP_ENTRY_COUNT /* entry of dot 0 */
#define TERRAIN_DIRECTIONAL_LIGHT_LUT_ENTRY_COUNT \
          (TERRAIN_LIGHTING_RAMP_ENTRY_COUNT + TERRAIN_DIRECTIONAL_LIGHT_LUT_LIT_ENTRY_COUNT) /* 513 */
/* TerrainLighting_AdjustDirectionAndRecomputeField: the light elevation stays at least this far (1/16 turn)
   below the horizon; the other limit is -FIXED_ANGLE16_QUARTER_TURN (straight down). */
#define TERRAIN_LIGHT_ELEVATION_MIN_TILT_ANGLE16 0x1000

extern TerrainDirectionRecord g_TerrainDirectionRecordTable256[256];

void FieldGrid_RecomputeInteriorTriangleNormalAngles(FieldGridAsset *fieldGrid);

void FieldGrid_RecomputeInteriorDirectionalLighting
          (AngleTurn32 lightElevationAngle,AngleTurn32 lightAzimuthAngle,FieldGridAsset *fieldGrid);

void TerrainDirectionTable_AdvanceAndRebuildVectors(void);

void FieldGridCell_RecomputeTriangleNormalAngles(FieldGridRowStrideBytes rowStrideBytes,FieldGridCell *cell);

void FieldGridCell_ComputeDirectionalLightColor(FieldGridCell *cell);

extern PackedArgb32 g_TerrainDirectionalLightColorLut[513]; /* shaded ramp (256) + lit half (257), indexed from the middle entry */

extern uint32_t g_TerrainDirectionalLightSecondaryColor;

void TerrainLighting_BuildColorRampAndSetBaseColor
          (PackedArgb32 secondaryColorArgb,PackedArgb32 baseColorArgb,PackedArgb32 rampStepColorArgb
          );

void TerrainLighting_AdjustDirectionAndRecomputeField
          (uint32_t playerRuntimeId,uint32_t reservedZero,uint32_t deltaElevationAngle,
          uint32_t deltaAzimuthAngle);

#endif /* THANDOR_WORLD_TERRAIN_FIELD_LIGHTING_H */

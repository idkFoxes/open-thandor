/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/include/thandor/world/terrain/field_lighting.h
 * Reverse engineering by idkFoxes 2026
 */

#ifndef THANDOR_WORLD_TERRAIN_FIELD_LIGHTING_H
#define THANDOR_WORLD_TERRAIN_FIELD_LIGHTING_H

#include <thandor/core/types.h>
#include <thandor/gameplay/army/types.h>
#include <thandor/ui/ingame/types.h>
#include <thandor/world/terrain/types.h>
#include <thandor/core/contracts.h>

/* Range of the auxiliary elevation angle (fieldRegion.auxiliaryElevationAngle) set by WorldRuntime_TurnAuxiliaryAnglesClamped */
#define WORLD_AUXILIARY_ELEVATION_MINIMUM (-0x4000) /* a quarter turn down */
#define WORLD_AUXILIARY_ELEVATION_MAXIMUM (-0x1000)
/* WorldLightingRuntime_UpdateInterpolatedTerrainLighting: one wrap of a 16-bit half of a packed field-region
   pair, added to the lower endpoint so the blend runs forward through the wrap */
#define WORLD_LIGHTING_PACKED_HALF_WRAP 0x10000

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

void TerrainDirectionTable_AdvanceAndRebuildVectors();

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

void WorldLightingRuntime_UpdateInterpolatedTerrainLighting();

void WorldRuntime_TurnAuxiliaryAnglesClamped
          (PlayerRuntimeId playerRuntimeId,uint32_t reservedZero,Q12 deltaElevationAngle,Q12 deltaAzimuthAngle);

void WorldRuntime_SetTerrainLightingConfiguration(PackedArgb32 lightingColor13CArgb,PackedArgb32 lightingColor138Argb,
          PackedArgb32 lightingColor134Argb,PackedArgb32 lightingColor130Argb,
          PackedArgb32 secondaryColorArgb,PackedArgb32 lightingColor128Argb,
          PackedArgb32 baseColorArgb,PackedArgb32 rampStepColorArgb,WorldRuntimeContext *worldRuntime
          );

void WorldRuntime_RecomputeFieldRegionNormalsAndLighting
          (FieldGridDimensionCells auxiliaryElevationAngle,FieldGridDimensionCells auxiliaryAzimuthAngle,
          Q12 lightElevationAngle,Q12 lightAzimuthAngle,WorldRuntimeContext *worldRuntime);

#endif /* THANDOR_WORLD_TERRAIN_FIELD_LIGHTING_H */

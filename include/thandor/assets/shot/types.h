/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/include/thandor/assets/shot/types.h
 * Reverse engineering by idkFoxes 2026
 */

#ifndef THANDOR_ASSETS_SHOT_TYPES_H
#define THANDOR_ASSETS_SHOT_TYPES_H

#include <stdint.h>
#include <thandor/core/ptr32.h> /* Ptr32: the pointer fields of these 32-bit layouts */
#include <thandor/assets/effect/types.h>
#include <thandor/audio/spatial/types.h>
#include <thandor/core/types.h>
#include <thandor/gameplay/army/types.h>
#include <thandor/graphics/render/types.h>
#include <thandor/ui/frontend/types.h>
#include <thandor/world/effects/types.h>
#include <thandor/world/terrain/types.h>

/* Types (split out by tools/dev/split_types.py). */

typedef struct ShotDefinition ShotDefinition, *PShotDefinition;
typedef struct ShotAssetHeader ShotAssetHeader, *PShotAssetHeader;

enum {
    SHOT_TRAJECTORY_DIRECT_LINE=0,
    SHOT_TRAJECTORY_BALLISTIC=1,
    SHOT_TRAJECTORY_FIXED_RANGE=2,
    SHOT_TRAJECTORY_LEAD_ADJUSTED=3
};
typedef int ShotTrajectoryMode;

typedef uint32_t ShotProjectileLifetimeTicks;

typedef AngleTurn16Stored32 ShotModelSpinStepTurn16;

typedef uint32_t ShotFrameAdvanceThresholdQ4;

typedef uint32_t ShotAnimationFrameCount;

typedef uint32_t ShotSecondaryEffectIntervalTicks;

typedef uint32_t ShotTrajectoryRampDurationTicks;

typedef uint32_t ShotFixedRangeTransitionAgeTicks;

struct ShotDefinition {
    ShotTrajectoryMode trajectoryMode; // Shot trajectory selector; enum is partial.
    uint32_t reservedDword04; // Exact fixed dword with semantics deferred.
    PckShotDefinitionIdCatalog definitionId; // Shot-definition registry identifier.
    Q12 launchSpeedQ12; // Launch speed in Q12.
    Ptr32<struct EffectDefinition> primaryEffectDefinition; // Primary effect identifier resolved in place to EffectDefinition pointer.
    Ptr32<struct EffectDefinition> terrainImpactEffectDefinitions31[31]; // SHT +0x14 exact 31-entry terrain-impact effect array.
    Ptr32<struct EffectDefinition> targetClassImpactEffectDefinitions8[8]; // SHT +0x90 exact 8-entry target-class impact effect array.
    Q12 targetClassImpactDamageQ12[8]; // SHT +0xB0 exact 8-entry target-class damage Q12 array.
    ShotProjectileLifetimeTicks projectileLifetimeTicks; // V414 Shot definition timing
    Ptr32<struct EffectDefinition> launchEffectDefinition; // Launch-effect identifier resolved in place and emitted by projectile creation.
    ShotModelSpinStepTurn16 modelSpinStepTurn16; // SHT +0xD8 added to model rotation each projectile update and masked to 16 bits.
    Q12 ballisticDivisorQ12; // Ballistic divisor in Q12.
    ShotTerrainImpactHeightDeltaQ12 terrainImpactHeightDeltasQ12[31]; // Per hit terrain material (index as terrainImpactEffectDefinitions31): terrain height delta of the crater. The address of the entry is passed as the impact effect's owner; when the effect completes (EFFECT_RUNTIME_COMPLETION_INVOKE_LINKED_HANDLER) it reads this column and the two parallel ones below through ShotTerrainImpactDeformationColumns.
    ShotFrameAdvanceThresholdQ4 animationFrameAdvanceThresholdQ4; // SHT +0x15C compared against a +0x10 animation accumulator.
    uint32_t terrainImpactRadiusWorldUnits[31]; // Per hit terrain material: crater radius (FieldGrid_ApplyRadialTerrainHeightDeltaAndRefreshSurface radiusWorldUnits; no crater unless positive).
    ShotAnimationFrameCount animationFrameCount; // SHT +0x1DC wraps projectile texture subresource animation.
    TerrainMaterialIndex terrainMaterialIndices31[31]; // Thirty-one signed terrain-material indices; negative values are sentinels, nonnegative values are bounded to loaded material texture sets.
    PackedArgb32 shadingColorArgb; // Packed shading ARGB color.
    GraphicsTransitionTickCount shadingTransitionDurationTicks; // Shading transition duration in ticks.
    GraphicsTransitionTickCount shadingReleaseTransitionDurationTicks; // SHT +0x264 is passed by three projectile release/impact paths to InterpolationState_SetNegatedTargetAndRescaleProgress.
    Ptr32<struct EffectDefinition> secondaryEffectDefinition; // Secondary effect identifier resolved in place to EffectDefinition pointer.
    ShotSecondaryEffectIntervalTicks secondaryEffectIntervalTicks; // SHT +0x26C reloads projectile secondary-effect countdown.
    ShotTrajectoryRampDurationTicks trajectoryRampDurationTicks; // V414 Shot definition timing
    ShotFixedRangeTransitionAgeTicks fixedRangeTransitionAgeThresholdTicks; // SHT +0x274 is the trajectory-mode-2 projectile age threshold compared after one age increment per processed simulation tick.
    AngleTurn16Stored32 elevationOffsetAngle16; // Elevation offset angle.
    Q12 mode2SelectionRangeQ12; // Mode-2 selection range in Q12.
    TerrainGridMaskIndex soundSlotIndex; // SHT +0x280 is bounds checked and indexes the world's sound slot table (worldRuntime->dwordArray); the positioned sound is gated against terrain cells.
    PackedArgb32 stateTintArgb; // Packed projectile tint.
    SpatialSoundGainQ15 positionedSoundGainQ15; // SHT +0x288 is the Q15 gain passed to SpatialSound_UpdateDesiredPositionedGains after terrain-mask gating.
    SpatialSoundMaximumDistanceQ12 positionedSoundMaximumDistanceQ12; // SHT +0x28C is the Q12 maximum distance passed to SpatialSound_UpdateDesiredPositionedGains after terrain-mask gating.
    AngleTurn16Stored32 guidanceTurnLimitAngle16; // Nonzero signed turn-limit magnitude used as a symmetric +/- clamp for in-flight guidance angle deltas; zero disables that runtime guidance path and selects the unguided/pre-lead range checks.
    Ptr32<void> ownedNestedResource; // Loaded or reused nested sprite resource.
    OwnedNestedResourceFlag ownedNestedResourcePresent; // Nested-resource ownership marker.
    uint16_t resourcePathUtf16[34]; // Fixed 34-word resource path changed to .spr during registration.
};

struct ShotAssetHeader {
    struct GeneratedAssetEntryCountHeader entryCountHeader;
    uint8_t reservedB4_1FF[332];
};

#endif /* THANDOR_ASSETS_SHOT_TYPES_H */

/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/include/thandor/assets/effect/types.h
 * Reverse engineering by idkFoxes 2026
 */

#ifndef THANDOR_ASSETS_EFFECT_TYPES_H
#define THANDOR_ASSETS_EFFECT_TYPES_H

#include <stdint.h>
#include <thandor/core/ptr32.h> /* Ptr32: the pointer fields of these 32-bit layouts */
#include <thandor/assets/army/types.h>
#include <thandor/audio/spatial/types.h>
#include <thandor/core/types.h>
#include <thandor/gameplay/army/types.h>
#include <thandor/graphics/render/types.h>
#include <thandor/graphics/resources/types.h>
#include <thandor/ui/frontend/types.h>
#include <thandor/world/effects/types.h>

/* Types (split out by tools/dev/split_types.py). */

typedef struct EffectDefinition EffectDefinition, *PEffectDefinition;
typedef struct EffectDefinitionTransitionPrefix EffectDefinitionTransitionPrefix, *PEffectDefinitionTransitionPrefix;
typedef struct GeneratedAssetEntryCountHeader GeneratedAssetEntryCountHeader, *PGeneratedAssetEntryCountHeader;
typedef struct EffectAssetHeader EffectAssetHeader, *PEffectAssetHeader;
typedef struct ShotDefinition ShotDefinition;

typedef Q12 EffectMovementSpeedQ12;

typedef uint32_t EffectFrameAdvanceThresholdQ4;

enum {
    EFFECT_CREATION_RANDOMIZE_ORIENTATION=1,
    EFFECT_CREATION_USE_ARMY_PALETTE_AND_TEXTURE_SET=2
};
typedef int EffectCreationFlagBits;

typedef uint32_t EffectAlphaFadeTicks;

struct EffectDefinitionTransitionPrefix {
    EffectLifecycleTransitionKind transitionKind; 
    uint32_t reserved04; 
};

struct EffectDefinition {
    struct EffectDefinitionTransitionPrefix transitionPrefix; 
    PckEffectDefinitionIdCatalog definitionId; 
    EffectAnimationFrameCount animationFrameCount; 
    DefinitionReferencePresentFlag linkedEffectPresent; 
    Ptr32<struct EffectDefinition> linkedEffectDefinition; 
    DefinitionReferencePresentFlag linkedShotPresent; 
    Ptr32<struct ShotDefinition> linkedShotDefinition; 
    EffectMovementSpeedQ12 movementSpeedQ12; 
    uint32_t completionCountdownTicks; /* +0x24 copied to the effect slot */
    EffectFrameAdvanceThresholdQ4 frameAdvanceThresholdQ4; 
    TerrainGridMaskIndex soundSlotIndex; 
    EffectCreationFlagBits creationFlags; 
    PackedArgb32 shadingColorArgb; 
    GraphicsTransitionTickCount shadingTransitionDurationTicks; 
    GraphicsTransitionTickCount shadingReleaseTransitionDurationTicks; 
    EffectShadingCountdownTicks shadingStartCountdownTicks; 
    EffectShadingCountdownTicks shadingStopCountdownTicks; 
    Ptr32<struct EffectDefinition> periodicEffectDefinition; 
    EffectPeriodicIntervalTicks periodicEffectIntervalTicks; 
    EffectAlphaFadeTicks alphaFadeInTicks; 
    EffectAlphaFadeTicks alphaFadeOutTicks;
    uint32_t pitchDropPerAgeSquared; // Terrain-relative motion: pitch (worldRotationAngle1) sinks by age * age * this per tick.
    uint32_t groundContactAlphaFadeStep; // Terrain-relative motion: alpha lost per tick while moving into the ground.
    PackedArgb32 stateTintArgb;
    Q12 modelScaleStartQ12; 
    Q12 modelScaleEndQ12; 
    SpatialSoundGainQ15 positionedSoundGainQ15; 
    SpatialSoundMaximumDistanceQ12 positionedSoundMaximumDistanceQ12; 
    Ptr32<void> ownedNestedResource; 
    OwnedNestedResourceFlag ownedNestedResourcePresent; 
    uint16_t resourcePathUtf16[34]; 
};

struct GeneratedAssetEntryCountHeader {
    struct GeneratedAssetCommonPrefix common;
    AssetRecordCount entryCount;
};

struct EffectAssetHeader {
    struct GeneratedAssetEntryCountHeader entryCountHeader;
    uint8_t reservedB4_1FF[332];
};

#endif /* THANDOR_ASSETS_EFFECT_TYPES_H */

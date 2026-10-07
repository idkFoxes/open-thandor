/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/include/thandor/assets/model/types.h
 * Reverse engineering by idkFoxes 2026
 */

#ifndef THANDOR_ASSETS_MODEL_TYPES_H
#define THANDOR_ASSETS_MODEL_TYPES_H

#include <stdint.h>
#include <thandor/core/ptr32.h> /* Ptr32: the pointer fields of these 32-bit layouts */
#include <thandor/assets/army/types.h>
#include <thandor/audio/spatial/types.h>
#include <thandor/core/types.h>
#include <thandor/gameplay/army/types.h>
#include <thandor/ui/ingame/types.h>

struct ModelDefinitionRecordPrefix;
struct ModelDefinition;
struct ModelDefinitionResolveView;
struct ModelAssetHeader;
struct EffectDefinition;
struct ShotDefinition;

struct ModelDefinitionRecordPrefix {
    AssetRecordByteCount byteSize;
    uint32_t nameTextIndex; // Model name text: TEXT_ID_MODEL_NAME_BASE (0x18004F) + nameTextIndex.
    PckModelDefinitionIdCatalog definitionId;
};

using ModelLookupKeyIndex = int;

using ModelLookupKeyClass = uint32_t;

struct ModelDefinition {
    AssetRecordByteCount byteSize;
    uint32_t nameTextIndex; /* +0x04 model name text: TEXT_ID_MODEL_NAME_BASE (0x18004F) + nameTextIndex */
    PckModelDefinitionIdCatalog definitionId;
    int movementSpeed; /* +0x0C movement speed; door/animation step per tick */
    int animatedChild0RotationStep; /* +0x10 per-tick rotation of animated child node 0 */
    int animatedChild2BobStep; /* +0x14 per-tick up/down step of animated child node 2 */
    int accelerationPerTick; /* +0x18 movement advance change per tick (acceleration); nonzero = mobile (shots lead it) */
    int animatedChild1RotationStep; /* +0x1C per-tick rotation of animated child node 1 */
    uint8_t reserved020_023[4];
    uint32_t runtimeValue24;
    uint32_t runtimeValue28;
    union ShotDefinitionReferenceOrSavedId shotDefinitionReference;
    int reloadTicks; /* +0x30 weapon reload ticks; nonzero = armed (the shot in shotDefinitionReference counts for selection range and damage) */
    uint8_t reserved034_047[20];
    uint32_t visibilityRadius; /* +0x48 terrain visibility (occlusion) radius; the army keeps the maximum in ArmyRuntimeSlot.visibilityRadius */
    ModelRuntimeClassId runtimeClassId; // 24-way model/army runtime callback class selector; consumed by placement, grid-influence, maintenance, and class-method dispatch tables.
    Q12 aimHeightOffsetQ12; /* +0x50 height above the model origin that shots aim at */
    Q12 placementHeightOffsetQ12; // Q12 height offset passed as the first argument to the five ArmyPlacementContact callbacks.
    union EffectDefinitionReferenceOrSavedId waterEmitterEffectDefinitionReference; /* +0x58 timed/step effect used instead of emitterEffectDefinitionReference over water */
    int targetClassIndex; /* +0x5C target class: indexes per-class impact effects and AI class tables */
    uint32_t maximumHealth; /* +0x60 maximum health */
    uint32_t rootNodeOffsetOrPointer; /* +0x64 MDL root node: asset-relative offset on disk, MdlSerializedNodeHeader * after registration */
    uint32_t modelFlags; /* +0x68 flag bits (0x10/0x20/0x40 become model node flags, 0x20 also used by army runtime, 0x80 by the session) */
    uint8_t field20_0x6c;
    uint8_t field21_0x6d;
    uint8_t field22_0x6e;
    uint8_t field23_0x6f;
    int visibilityHeightOffset; /* +0x70 eye height above the model; the army keeps the maximum in ArmyRuntimeSlot.visibilityHeightOffset */
    PckArmyAssetIdCatalog destroyedReplacementArmyAssetId; /* +0x74 army spawned in place of a destroyed root model, -1 = none */
    SpatialSoundGainQ15 positionedSoundGainQ15; // Q15 gain passed with positionedSoundMaximumDistanceQ12 to positioned-sound playback/update helpers.
    SpatialSoundMaximumDistanceQ12 positionedSoundMaximumDistanceQ12; // Q12 maximum positioned-sound distance paired with positionedSoundGainQ15.
    union EffectDefinitionReferenceOrSavedId destructionEffect0; /* +0x80 + 8 * i: destruction effect channel i (ModelRuntimeSlot.destructionEffectTimers) */
    uint32_t destructionEffectDelayTicks0; /* +0x84 + 8 * i: initial timer of channel i */
    union EffectDefinitionReferenceOrSavedId destructionEffect1;
    uint32_t destructionEffectDelayTicks1;
    union EffectDefinitionReferenceOrSavedId destructionEffect2;
    uint32_t destructionEffectDelayTicks2;
    union EffectDefinitionReferenceOrSavedId destructionEffect3;
    uint32_t destructionEffectDelayTicks3;
    union EffectDefinitionReferenceOrSavedId destructionEffect4;
    uint32_t destructionEffectDelayTicks4;
    union EffectDefinitionReferenceOrSavedId destructionEffect5;
    uint32_t destructionEffectDelayTicks5;
    union EffectDefinitionReferenceOrSavedId destructionEffect6;
    uint32_t destructionEffectDelayTicks6;
    union EffectDefinitionReferenceOrSavedId destructionEffect7;
    uint32_t destructionEffectDelayTicks7;
    union {
        uint8_t reserved0C0_0DB[28];
        struct {
            int classParameterC0; /* +0xC0 class specific (class 0x12: army asset it turns into; walkers: stride) */
            int classParameterC4; /* +0xC4 class specific (linked-child slot count, buildable asset flag mask, step lift) */
            int classParameterC8; /* +0xC8 class specific */
            int classParameterCC; /* +0xCC class specific (placement kind 1: maximum water surface delta) */
            uint32_t movingLoopSoundSlotIndex; /* +0xD0 looping sound while moving (index into worldRuntime->dwordArray) */
            uint32_t moveStartSoundSlotIndex; /* +0xD4 one-shot sound when the unit starts moving */
            uint32_t turningLoopSoundSlotIndex; /* +0xD8 looping sound while turning */
        };
    };
    uint32_t footprintRadius; /* +0xDC radius for grid influence, placement clearance, depth bins and selection markers (from footprintRadiusClass) */
    uint8_t reserved0E0_15F[128];
    InGameNotificationMovieId firstBuiltNotificationMovieId; /* +0x160 notification when the first one is built */
    InGameNotificationMovieId nextBuiltNotificationMovieId; /* +0x164 notification for every later one */
    union ShotDefinitionReferenceOrSavedId emitterShotDefinitionReference; /* +0x168 timed shot emitter shot, -1 = none */
    int shotEmitterIntervalTicks; /* +0x16C timed shot emitter interval */
    uint32_t shotEmitterRandomTicks; /* +0x170 random extra interval, 0 = none */
    union EffectDefinitionReferenceOrSavedId emitterEffectDefinitionReference; /* +0x174 timed effect emitter effect (on dry ground) */
    int effectEmitterIntervalTicks; /* +0x178 timed effect emitter interval */
    uint32_t effectEmitterRandomTicks; /* +0x17C random extra interval, 0 = none */
    uint32_t buildTicks; /* +0x180 build time (ModelDefinitionRegistry_FindBuildCostsById) */
    uint32_t xeniteValueQ4; /* +0x184 Xenite value; dismantling refunds 1/32 of it every 12 ticks */
    uint32_t buildEnergyLoadQ4; /* +0x188 Energy load while it is being built (ArmyAssetRecord.energyLoadQ4) */
    uint32_t energyLoadQ4; /* +0x18C Energy demand (copied to the model runtime's energyLoadQ4) */
    union EffectDefinitionReferenceOrSavedId removalEffectDefinitionReference; /* +0x190 effect that destroys the model hierarchy when it is dismantled */
    int waterDamageMultiplier; /* +0x194 water damage = water delta * this >> 7 (ground movement views) */
    uint32_t waterDamageThreshold; /* +0x198 water delta above which ground units take damage (from terrainTraversalClass) */
    int supportRadius; /* +0x19C support / proximity radius (0 = none) */
    uint32_t footprintRadiusCopy; /* +0x1A0 same grid table value as footprintRadius; ArmyRuntime_TestArmyNearFactoryExit uses it */
    uint32_t switchedOffVisibilityRadius; /* +0x1A4 visibilityRadius while the model is switched off */
    uint32_t placementFlags;
    uint32_t loopingSoundSlotIndex; /* +0x1AC looping positioned sound (index into worldRuntime->dwordArray) */
    int builtCount; /* +0x1B0 how many of this definition were built (selects the notification) */
    int healthRegenerationPerStep; /* +0x1B4 health change every 4 ticks */
    ModelTextureSubresourceIndex primaryAnimatedSubresourceIndex;
    ModelTextureSubresourceIndex secondaryAnimatedSubresourceIndex;
    uint32_t requiredTechnologyBit; /* +0x1C0 technology bit the faction needs; also matched against a runtime class id by ModelDefinitionRegistry_FindByRuntimeClassId */
    uint32_t researchTechnologyIds[29]; /* +0x1C4 [1..28] (+0x1C8..+0x234) the technologies researchable here */
    PckModelDefinitionIdCatalog variantModelDefinitionIds[6]; /* +0x238 technology variants, 0 = none */
    int damageEffectHealthPercent; /* +0x250 damage effect below this health percentage */
    union EffectDefinitionReferenceOrSavedId damageEffectDefinitionReference;
    int damageEffectIntervalTicks; /* +0x258 */
    uint32_t damageEffectRandomTicks; /* +0x25C random extra interval, 0 = none */
    uint32_t footprintRadiusClass; /* +0x260 index into g_GridInfluenceRadiusOffset (sets footprintRadius), negative = keep */
    uint32_t terrainTraversalClass; /* +0x264 selects slope/water thresholds by placementContactKindIndex, negative = keep */
    uint32_t traversalSecondaryThreshold; /* +0x268 pitch below which ground movement slows (from terrainTraversalClass) */
    uint32_t primarySoundIndex; /* +0x26C one-shot sound (index into worldRuntime->dwordArray) */
    uint32_t secondarySoundIndex; /* +0x270 second one-shot sound (index into worldRuntime->dwordArray): creation, hatch lift */
    uint32_t positionedSoundSlotIndex; /* +0x274 positioned sound slot (index into worldRuntime->dwordArray) */
    ArmyPlacementContactKindIndex32 placementContactKindIndex; // Placement-contact callback dispatch index. Observed vocabulary: 0 terrain height; 1 water-surface height; 2 terrain height+normal; 3 articulated suspension; 4 top-surface height. Kept as a 32-bit index typedef rather than enum storage.
    uint32_t occupancyMarkRadius; /* +0x27C radius of occupancy bit 1; the army keeps the maximum in ArmyRuntimeSlot.occupancyMarkRadius */
};

struct ModelDefinitionResolveView {
    AssetRecordByteCount byteSize;
    uint32_t flags;
    PckModelDefinitionIdCatalog definitionId;
    uint32_t movementSpeed; // Movement speed; also the AI coefficient at +24.
    uint8_t reserved010_017[8]; // Unresolved intervening fields.
    uint32_t accelerationPerTick; // Movement acceleration; nonzero (mobile) enables the AI score bonus.
    uint8_t reserved01C_023[8]; // Unresolved remaining fields.
    uint32_t runtimeValue24;
    uint32_t runtimeValue28;
    Ptr32<struct ShotDefinition> shotDefinitionReference; // Resolver-phase slot: serialized Shot id on entry, live ShotDefinition pointer after successful lookup.
    uint32_t reloadTicks; // ModelDefinition.reloadTicks; the AI divides shot damage by it (damage per tick), nonzero = armed.
    uint8_t reserved034_047[20]; // Unresolved remaining fields.
    uint32_t visibilityRadius;
    ModelRuntimeClassId runtimeClassId; // 24-way model/army runtime callback class selector; consumed by placement, grid-influence, maintenance, and class-method dispatch tables.
    Q12 aimHeightOffsetQ12; /* +0x50 height above the model origin that shots aim at */
    Q12 placementHeightOffsetQ12; // Q12 height offset passed as the first argument to the five ArmyPlacementContact callbacks.
    Ptr32<struct EffectDefinition> waterEmitterEffectDefinitionReference; // Resolver-phase slot: serialized Effect id on entry, live EffectDefinition pointer after successful lookup.
    uint32_t targetClassIndex; // ModelDefinition.targetClassIndex; also indexes the AI per-class maximum array during scoring.
    uint32_t maximumHealth; /* +0x60 maximum health */
    uint32_t rootNodeOffsetOrPointer;
    uint32_t modelFlags;
    uint8_t field24_0x6c;
    uint8_t field25_0x6d;
    uint8_t field26_0x6e;
    uint8_t field27_0x6f;
    int visibilityHeightOffset; /* +0x70 */
    PckArmyAssetIdCatalog destroyedReplacementArmyAssetId; /* +0x74 */
    SpatialSoundGainQ15 positionedSoundGainQ15; // Q15 gain passed with positionedSoundMaximumDistanceQ12 to positioned-sound playback/update helpers.
    SpatialSoundMaximumDistanceQ12 positionedSoundMaximumDistanceQ12; // Q12 maximum positioned-sound distance paired with positionedSoundGainQ15.
    Ptr32<struct EffectDefinition> destructionEffect0; // Resolver-phase slot: serialized Effect id on entry, live EffectDefinition pointer after successful lookup.
    uint32_t destructionEffectDelayTicks0;
    Ptr32<struct EffectDefinition> destructionEffect1; // Resolver-phase slot: serialized Effect id on entry, live EffectDefinition pointer after successful lookup.
    uint32_t destructionEffectDelayTicks1;
    Ptr32<struct EffectDefinition> destructionEffect2; // Resolver-phase slot: serialized Effect id on entry, live EffectDefinition pointer after successful lookup.
    uint32_t destructionEffectDelayTicks2;
    Ptr32<struct EffectDefinition> destructionEffect3; // Resolver-phase slot: serialized Effect id on entry, live EffectDefinition pointer after successful lookup.
    uint32_t destructionEffectDelayTicks3;
    Ptr32<struct EffectDefinition> destructionEffect4; // Resolver-phase slot: serialized Effect id on entry, live EffectDefinition pointer after successful lookup.
    uint32_t destructionEffectDelayTicks4;
    Ptr32<struct EffectDefinition> destructionEffect5; // Resolver-phase slot: serialized Effect id on entry, live EffectDefinition pointer after successful lookup.
    uint32_t destructionEffectDelayTicks5;
    Ptr32<struct EffectDefinition> destructionEffect6; // Resolver-phase slot: serialized Effect id on entry, live EffectDefinition pointer after successful lookup.
    uint32_t destructionEffectDelayTicks6;
    Ptr32<struct EffectDefinition> destructionEffect7; // Resolver-phase slot: serialized Effect id on entry, live EffectDefinition pointer after successful lookup.
    uint32_t destructionEffectDelayTicks7;
    uint8_t reserved0C0_0DB[28]; // Unresolved byte span retained explicitly to avoid autogenerated undefined-byte components.
    uint32_t footprintRadius;
    uint8_t reserved0E0_167[136]; // Unresolved byte span retained explicitly to avoid autogenerated undefined-byte components.
    Ptr32<struct ShotDefinition> emitterShotDefinitionReference; // Resolver-phase slot: serialized Shot id on entry, live ShotDefinition pointer after successful lookup.
    uint8_t reserved16C_173[8]; // Unresolved byte span retained explicitly to avoid autogenerated undefined-byte components.
    Ptr32<struct EffectDefinition> emitterEffectDefinitionReference; // Resolver-phase slot: serialized Effect id on entry, live EffectDefinition pointer after successful lookup.
    uint8_t reserved178_187[16]; // Unresolved byte span retained explicitly to avoid autogenerated undefined-byte components.
    uint32_t buildEnergyLoadQ4;
    uint32_t energyLoadQ4; /* +0x18C Energy demand (copied to the model runtime's energyLoadQ4) */
    Ptr32<struct EffectDefinition> removalEffectDefinitionReference; // Resolver-phase slot: serialized Effect id on entry, live EffectDefinition pointer after successful lookup.
    uint8_t reserved194_197[4]; // Unresolved byte span retained explicitly to avoid autogenerated undefined-byte components.
    uint32_t waterDamageThreshold;
    uint8_t reserved19C_19F[4]; // Unresolved byte span retained explicitly to avoid autogenerated undefined-byte components.
    uint32_t footprintRadiusCopy; // Grid-derived runtime value; deliberately not an Effect reference.
    uint8_t reserved1A4_1A7[4]; // Unresolved byte span retained explicitly to avoid autogenerated undefined-byte components.
    uint32_t placementFlags;
    uint8_t reserved1AC_1B7[12]; // Unresolved byte span retained explicitly to avoid autogenerated undefined-byte components.
    ModelTextureSubresourceIndex primaryAnimatedSubresourceIndex;
    ModelTextureSubresourceIndex secondaryAnimatedSubresourceIndex;
    uint8_t reserved1C0_253[148]; // Unresolved byte span retained explicitly to avoid autogenerated undefined-byte components.
    Ptr32<struct EffectDefinition> damageEffectDefinitionReference; // Resolver-phase slot: serialized Effect id on entry, live EffectDefinition pointer after successful lookup.
    uint8_t reserved258_25F[8]; // Unresolved byte span retained explicitly to avoid autogenerated undefined-byte components.
    uint32_t footprintRadiusClass;
    uint32_t terrainTraversalClass;
    uint32_t traversalSecondaryThreshold;
    uint8_t reserved26C_277[12]; // Unresolved byte span retained explicitly to avoid autogenerated undefined-byte components.
    ArmyPlacementContactKindIndex32 placementContactKindIndex; // Placement-contact callback dispatch index. Observed vocabulary: 0 terrain height; 1 water-surface height; 2 terrain height+normal; 3 articulated suspension; 4 top-surface height. Kept as a 32-bit index typedef rather than enum storage.
    uint32_t occupancyMarkRadius;
};

struct ModelAssetHeader {
    struct GeneratedAssetRecordCountHeader recordCountHeader;
    uint8_t reservedB4_1FF[332];
};

/* One MDL definition record, three views: the registry (g_ModelDefinitionRegistry) keeps the
   ModelDefinitionRecordPrefix (its first 0xC bytes), registration works on ModelDefinitionResolveView, the
   runtime reads ModelDefinition. These convert between the views of the same record bytes. */
static_assert(offsetof(ModelDefinition, byteSize) == offsetof(ModelDefinitionRecordPrefix, byteSize));
static_assert(offsetof(ModelDefinition, definitionId) == offsetof(ModelDefinitionRecordPrefix, definitionId));
static_assert(offsetof(ModelDefinitionResolveView, definitionId) == offsetof(ModelDefinition, definitionId));
static_assert(sizeof(ModelDefinitionResolveView) <= sizeof(ModelDefinition));
static inline ModelDefinition *ModelDefinition_FromPrefix(ModelDefinitionRecordPrefix *prefix)
{
  return reinterpret_cast<ModelDefinition *>(prefix);
}
static inline ModelDefinition *ModelDefinition_FromResolveView(ModelDefinitionResolveView *view)
{
  return reinterpret_cast<ModelDefinition *>(view);
}
static inline ModelDefinitionRecordPrefix *ModelDefinitionResolveView_Prefix(ModelDefinitionResolveView *view)
{
  return reinterpret_cast<ModelDefinitionRecordPrefix *>(view);
}

#endif /* THANDOR_ASSETS_MODEL_TYPES_H */

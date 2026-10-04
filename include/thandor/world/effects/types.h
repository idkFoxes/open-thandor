/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/include/thandor/world/effects/types.h
 * Reverse engineering by idkFoxes 2026
 */

#ifndef THANDOR_WORLD_EFFECTS_TYPES_H
#define THANDOR_WORLD_EFFECTS_TYPES_H

#include <stdint.h>
#include <thandor/core/ptr32.h> /* Ptr32: the pointer fields of these 32-bit layouts */
#include <thandor/assets/rom/types.h>
#include <thandor/core/types.h>
#include <thandor/gameplay/army/types.h>
#include <thandor/ui/ingame/types.h>
#include <thandor/world/terrain/types.h>

/* Types (split from generated/types.h by tools/dev/split_types.py). */

typedef struct EffectRuntimeSlot EffectRuntimeSlot, *PEffectRuntimeSlot;
typedef union EffectModelNodeReferenceOrSavedOffset EffectModelNodeReferenceOrSavedOffset, *PEffectModelNodeReferenceOrSavedOffset;
typedef struct EffectRuntimeLifecycleState EffectRuntimeLifecycleState, *PEffectRuntimeLifecycleState;
typedef struct EffectRuntimeOwnerAndDefinitionState EffectRuntimeOwnerAndDefinitionState, *PEffectRuntimeOwnerAndDefinitionState;
typedef union EffectRuntimeOwnerReference EffectRuntimeOwnerReference, *PEffectRuntimeOwnerReference;
typedef struct ShotTerrainImpactDeformationColumns ShotTerrainImpactDeformationColumns, *PShotTerrainImpactDeformationColumns;
typedef struct EffectModelRuntimeNode EffectModelRuntimeNode, *PEffectModelRuntimeNode;
typedef struct GraphicsShadingRuntimeRecord GraphicsShadingRuntimeRecord;
typedef struct ModelRuntimeNode ModelRuntimeNode;

typedef uint32_t EffectAnimationFrameCount;

typedef uint32_t DefinitionReferencePresentFlag;

enum {
    EFFECT_RUNTIME_COMPLETION_NONE=0,
    EFFECT_RUNTIME_COMPLETION_DESTROY_MODEL_HIERARCHY=1,
    EFFECT_RUNTIME_COMPLETION_INVOKE_LINKED_HANDLER=2,
    EFFECT_RUNTIME_COMPLETION_SPAWN_ARMY_FROM_MODEL=3
};
typedef int EffectRuntimeCompletionAction;

typedef uint32_t EffectShadingCountdownTicks;

typedef uint32_t EffectPeriodicIntervalTicks;

typedef uint32_t EffectAgeTicks;

typedef uint32_t EffectAnimationFrameAccumulatorQ4;

typedef uint32_t ShotTerrainImpactHeightDeltaQ12;

union EffectRuntimeOwnerReference {
    Ptr32<struct ModelRuntimeNode> modelNode;
    Ptr32<struct ModelRuntimeSlot> modelRuntime; /* owner of EFFECT_RUNTIME_COMPLETION_DESTROY_MODEL_HIERARCHY */
    Ptr32<struct ArmyRuntimeSlot> armyRuntime;
    Ptr32<struct ShotTerrainImpactDeformationColumns> terrainImpactColumns; /* owner of a shot's terrain impact effect (EFFECT_RUNTIME_COMPLETION_INVOKE_LINKED_HANDLER) */
    uint32_t serializedOffset;
};

struct EffectRuntimeOwnerAndDefinitionState {
    union EffectRuntimeOwnerReference owner;
    uint32_t completionCountdownTicks; /* from EffectDefinition.completionCountdownTicks; 0 runs the completion action */
};

struct EffectRuntimeLifecycleState {
    uint32_t nextShotPointIndex; /* key index of the model point (class MODEL_POINT_CLASS_SHOT) of the next shot */ 
    EffectAnimationFrameAccumulatorQ4 animationFrameAccumulatorQ4; 
    struct EffectRuntimeOwnerAndDefinitionState ownerAndDefinition; 
};

union EffectModelNodeReferenceOrSavedOffset {
    Ptr32<struct ModelRuntimeNode> modelNode; 
    uint32_t savedIdOrOffset; 
    uint32_t raw; 
};

struct EffectRuntimeSlot {
    union EffectDefinitionReferenceOrSavedId definitionOrSavedId; 
    union EffectModelNodeReferenceOrSavedOffset modelNodeOrSavedOffset; 
    EffectAnimationFrameCount animationFramesRemaining; 
    DefinitionReferencePresentFlag linkedEffectPresent; 
    DefinitionReferencePresentFlag linkedShotPresent; 
    struct EffectRuntimeLifecycleState lifecycleOwnerAndDefinition; 
    EffectRuntimeCompletionAction completionAction; 
    EffectShadingCountdownTicks shadingStartCountdownTicksRemaining; 
    EffectShadingCountdownTicks shadingStopCountdownTicksRemaining; 
    EffectPeriodicIntervalTicks periodicEffectCountdownTicks; 
    uint32_t terrainRuntimeClassState; 
    PackedArgb32 stateTintArgb; 
    EffectAgeTicks effectAgeTicks; 
};

/* The three parallel per-terrain-material columns of a ShotDefinition seen from &terrainImpactHeightDeltasQ12[i]
   (the owner a shot's terrain impact effect is created with): entry i of each column. */
struct ShotTerrainImpactDeformationColumns {
    ShotTerrainImpactHeightDeltaQ12 heightDeltaQ12; // terrainImpactHeightDeltasQ12[i]
    uint8_t otherHeightDeltas[124];
    uint32_t radiusWorldUnits; // terrainImpactRadiusWorldUnits[i] (+0x80)
    uint8_t otherRadii[124];
    TerrainMaterialIndex terrainMaterialIndex; // terrainMaterialIndices31[i] (+0x100): material painted into the crater, negative for none.
};

struct EffectModelRuntimeNode {
    struct WorldRuntimeNodeCommon common; 
    struct WorldRuntimeNodeModelPayload modelPayload; 
    Ptr32<struct EffectRuntimeSlot> effectRuntime; 
    ModelRuntimeFlags runtimeFlags; 
    ModelTextureSubresourceIndex textureSubresourceBaseIndex; 
    Q12 subtreeBoundingRadiusQ12; 
    PackedArgb32 tintArgb; 
    Ptr32<struct GraphicsShadingRuntimeRecord> shadingRecord; 
    Ptr32<void> modelRuntimeLinkOrSavedOffset; 
    ModelTextureSubresourceIndex primaryAnimatedSubresourceIndex; 
    ModelTextureOffsetTexel primaryTextureOffsetU; 
    ModelTextureOffsetTexel primaryTextureOffsetV; 
    struct GraphicsFixedMatrix3x4 worldTransform; 
    uint32_t runtimeStateA0; 
    WorldOwnerRuntimeClassId ownerClassId; 
    ModelTextureSubresourceIndex secondaryAnimatedSubresourceIndex; 
    ModelTextureOffsetTexel secondaryTextureOffsetU; 
    ModelTextureOffsetTexel secondaryTextureOffsetV; 
    ModelDepthBinMask depthBinMaskNear; 
    ModelDepthBinMask depthBinMaskFar; 
    int renderDepthBiasOrState; 
    Q12 modelScaleQ12; 
    Ptr32<struct ModelRuntimeNode> parentNode; 
    uint32_t childCount; 
    Ptr32<struct ModelRuntimeNode> childNodes[13]; 
};

#endif /* THANDOR_WORLD_EFFECTS_TYPES_H */

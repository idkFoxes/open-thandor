/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/include/thandor/world/shots/types.h
 * Reverse engineering by idkFoxes 2026
 */

#ifndef THANDOR_WORLD_SHOTS_TYPES_H
#define THANDOR_WORLD_SHOTS_TYPES_H

#include <stdint.h>
#include <thandor/core/ptr32.h> /* Ptr32: the pointer fields of these 32-bit layouts */
#include <thandor/assets/rom/types.h>
#include <thandor/core/types.h>
#include <thandor/gameplay/army/types.h>
#include <thandor/ui/ingame/types.h>

/* Types (split out by tools/dev/split_types.py). */

typedef struct ShotRuntimeSlot ShotRuntimeSlot, *PShotRuntimeSlot;
typedef union ShotModelNodeReferenceOrSavedOffset ShotModelNodeReferenceOrSavedOffset, *PShotModelNodeReferenceOrSavedOffset;
typedef union ShotModelRuntimeStateOrSavedOffset ShotModelRuntimeStateOrSavedOffset, *PShotModelRuntimeStateOrSavedOffset;
typedef struct ShotRuntimeOwnerAndTrajectoryState ShotRuntimeOwnerAndTrajectoryState, *PShotRuntimeOwnerAndTrajectoryState;
typedef struct ShotLaunchAngles ShotLaunchAngles, *PShotLaunchAngles;
typedef struct ShotModelRuntimeNode ShotModelRuntimeNode, *PShotModelRuntimeNode;
typedef struct GraphicsShadingRuntimeRecord GraphicsShadingRuntimeRecord;
typedef struct ModelRuntimeNode ModelRuntimeNode;

using ShotAnimationFrameAccumulatorQ4 = uint32_t;

using ShotImpactEffectEmissionFlags = uint32_t;

using ShotProjectileAgeTicks = uint32_t;

using ShotLifetimeRemainingTicks = uint32_t;

using ShotAnimationFrameIndex = uint32_t;

using ShotSecondaryEffectCountdownTicks = uint32_t;

struct ShotRuntimeOwnerAndTrajectoryState {
    Ptr32<struct ArmyRuntimeSlot> ownerArmyRuntime; 
    Q12 directionComponent0Q12; 
    Q12 directionComponent1Q12; 
    Q12 directionComponent2Q12; 
    ShotAnimationFrameIndex animationFrameIndex; 
    ShotSecondaryEffectCountdownTicks secondaryEffectCountdownTicks; 
};

union ShotModelNodeReferenceOrSavedOffset {
    Ptr32<struct ModelRuntimeNode> modelNode; 
    uint32_t savedIdOrOffset; 
    uint32_t raw; 
};

union ShotModelRuntimeStateOrSavedOffset {
    uint32_t runtimeState; 
    uint32_t savedIdOrOffset; 
    uint32_t raw; 
    Ptr32<void> runtimeStatePointer; 
};

struct ShotRuntimeSlot {
    union ShotDefinitionReferenceOrSavedId definitionOrSavedId; 
    Q12 launchSpeedQ12; 
    uint32_t terrainRuntimeClassState; 
    ShotAnimationFrameAccumulatorQ4 animationFrameAccumulatorQ4; 
    union ShotModelNodeReferenceOrSavedOffset modelNodeOrSavedOffset; 
    union ShotModelRuntimeStateOrSavedOffset runtimeStateOrSavedOffset; 
    AngleTurn16Stored32 elevationOffsetAngle16; 
    ShotImpactEffectEmissionFlags impactEffectEmissionFlags; 
    ShotProjectileAgeTicks projectileAgeTicks; 
    ShotLifetimeRemainingTicks lifetimeTicksRemaining; 
    struct ShotRuntimeOwnerAndTrajectoryState ownerAndTrajectory; 
};

struct ShotLaunchAngles {
    AngleTurn32 headingAngle; // launch heading/azimuth
    AngleTurn32 elevationAngle; // launch elevation
};

struct ShotModelRuntimeNode {
    struct WorldRuntimeNodeCommon common; // Shared world-node prefix.
    struct WorldRuntimeNodeModelPayload modelPayload; // Model-specific world-node payload.
    Ptr32<struct ShotRuntimeSlot> shotRuntime; // Function-local live ShotRuntimeSlot pointer for maintenance-table object kind shot; global ModelRuntimeNode payload remains polymorphic.
    ModelRuntimeFlags runtimeFlags; // Model transform, animation, and render flags.
    ModelTextureSubresourceIndex textureSubresourceBaseIndex; // Base added to model triangle subresource indices.
    Q12 subtreeBoundingRadiusQ12; // Computed subtree bounding radius.
    PackedArgb32 tintArgb; // Current packed state tint.
    Ptr32<struct GraphicsShadingRuntimeRecord> shadingRecord; // Optional runtime shading record.
    Ptr32<void> modelRuntimeLinkOrSavedOffset; // Runtime link or serialized pool offset.
    ModelTextureSubresourceIndex primaryAnimatedSubresourceIndex; // Primary animated texture subresource index.
    ModelTextureOffsetTexel primaryTextureOffsetU; // Primary animated texture U offset.
    ModelTextureOffsetTexel primaryTextureOffsetV; // Primary animated texture V offset.
    struct GraphicsFixedMatrix3x4 worldTransform; // Composed world transform.
    uint32_t runtimeStateA0; // Class-specific runtime state.
    WorldOwnerRuntimeClassId ownerClassId; // Runtime owner/class discriminator.
    ModelTextureSubresourceIndex secondaryAnimatedSubresourceIndex; // Secondary animated texture subresource index.
    ModelTextureOffsetTexel secondaryTextureOffsetU; // Secondary animated texture U offset.
    ModelTextureOffsetTexel secondaryTextureOffsetV; // Secondary animated texture V offset.
    ModelDepthBinMask depthBinMaskNear; // Near depth-bin visibility mask.
    ModelDepthBinMask depthBinMaskFar; // Far depth-bin visibility mask.
    int renderDepthBiasOrState; // Render depth bias or class state.
    Q12 modelScaleQ12; // Model scale.
    Ptr32<struct ModelRuntimeNode> parentNode; // Parent model node.
    uint32_t childCount; // Valid child pointer count.
    Ptr32<struct ModelRuntimeNode> childNodes[13]; // Fixed child-node pointer array.
};

#endif /* THANDOR_WORLD_SHOTS_TYPES_H */

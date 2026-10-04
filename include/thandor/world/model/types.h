/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/include/thandor/world/model/types.h
 * Reverse engineering by idkFoxes 2026
 */

#ifndef THANDOR_WORLD_MODEL_TYPES_H
#define THANDOR_WORLD_MODEL_TYPES_H

#include <stdint.h>
#include <thandor/core/ptr32.h> /* Ptr32: the pointer fields of these 32-bit layouts */
#include <thandor/assets/army/types.h>
#include <thandor/assets/rom/types.h>
#include <thandor/core/types.h>
#include <thandor/gameplay/army/types.h>
#include <thandor/graphics/render/types.h>
#include <thandor/ui/ingame/types.h>

typedef struct ModelRuntimeNode ModelRuntimeNode, *PModelRuntimeNode;
typedef union ModelRuntimePayloadReference4 ModelRuntimePayloadReference4, *PModelRuntimePayloadReference4;
typedef struct ModelAttachmentTransformRecord ModelAttachmentTransformRecord, *PModelAttachmentTransformRecord;
typedef struct ModelRaycastTriangleDescriptor ModelRaycastTriangleDescriptor, *PModelRaycastTriangleDescriptor;
typedef struct ModelRuntimeSlotClassStateSerializedScalar ModelRuntimeSlotClassStateSerializedScalar, *PModelRuntimeSlotClassStateSerializedScalar;
typedef struct ModelRuntimeAttachmentSavedDescriptor ModelRuntimeAttachmentSavedDescriptor, *PModelRuntimeAttachmentSavedDescriptor;
typedef struct ModelRuntimeSlotUnrebaseView ModelRuntimeSlotUnrebaseView, *PModelRuntimeSlotUnrebaseView;
typedef union ModelRaycastNearestNodeOrScratch4 ModelRaycastNearestNodeOrScratch4, *PModelRaycastNearestNodeOrScratch4;
typedef struct ModelRelativeDirectionAngles ModelRelativeDirectionAngles, *PModelRelativeDirectionAngles;
typedef struct EffectRuntimeSlot EffectRuntimeSlot;
typedef struct ShotRuntimeSlot ShotRuntimeSlot;

using SprAttachmentPackedKey = uint32_t;

struct ModelAttachmentTransformRecord {
    SprAttachmentPackedKey packedKindAndSelector; 
    Q12 localTranslationXQ12; 
    Q12 localTranslationYQ12; 
    Q12 localTranslationZQ12; 
};

union ModelRuntimePayloadReference4 {
    Ptr32<struct ModelRuntimeSlot> modelRuntime; 
    Ptr32<struct ArmyRuntimeSlot> armyRuntime; 
    Ptr32<struct EffectRuntimeSlot> effectRuntime; 
    Ptr32<struct ShotRuntimeSlot> shotRuntime; 
    Ptr32<void> opaqueRuntime; 
    uint32_t savedOffsetOrRaw; 
};

struct ModelRuntimeNode {
    struct WorldRuntimeNodeCommon common; 
    struct WorldRuntimeNodeModelPayload modelPayload; 
    union ModelRuntimePayloadReference4 runtimePayload; 
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

using ModelRuntimePoolRelativeOffset = uint32_t;

using ModelRuntimeAttachmentIndex = uint32_t;

using ModelNodePoolRelativeOffset = uint32_t;

struct ModelRaycastTriangleDescriptor {
    Ptr32<struct GraphicsFixedVec3> vertex0; 
    uint8_t reservedVertex0Metadata04_0B[8]; 
    Ptr32<struct GraphicsFixedVec3> vertex1; 
    uint8_t reservedVertex1Metadata10_17[8]; 
    Ptr32<struct GraphicsFixedVec3> vertex2; 
    uint8_t reservedVertex2Metadata1C_23[8]; 
    GraphicsPlaneNormalFixed planeNormalX; 
    GraphicsPlaneNormalFixed planeNormalY; 
    GraphicsPlaneNormalFixed planeNormalZ; 
    uint8_t reserved30_3F[16]; 
};

struct ModelRuntimeAttachmentSavedDescriptor {
    ModelRuntimePoolRelativeOffset childModelRuntimeSavedOffset; 
    Ptr32<struct ModelAttachmentTransformRecord> sourceTransform; 
    ModelNodePoolRelativeOffset parentModelNodeSavedOffset; 
    ModelChildNodeIndex childNodeIndex; 
    AngleTurn32 childLocalRotationAngle0; 
    AngleTurn32 childLocalRotationAngle1; 
    AngleTurn32 childLocalRotationAngle2; 
    uint32_t reserved1C; 
};

struct ModelRuntimeSlotClassStateSerializedScalar {
    uint8_t reserved84_A7[36]; 
    uint32_t classStateA8; 
    uint32_t classStateAC; 
    int32_t classStateB0; 
    int32_t classStateB4; 
    uint32_t behaviorState; 
    uint8_t reservedBC_CF[20]; 
    int32_t classStateD0; 
    uint8_t reservedD4_DB[8]; 
    uint32_t classStateDC; 
    uint32_t effectEmitterPointIndex; 
    uint32_t shotEmitterTimerTicks; 
    uint32_t effectEmitterTimerTicks; 
    uint32_t stateFlags; 
    uint32_t linkedArmyRuntimeSavedOffset; 
    EnergyDemandQ4 energyDemandQ4; 
    uint32_t healthRegenerationDelayTicks; 
    uint32_t dismantleTickCountdown; 
};

struct ModelRuntimeSlotUnrebaseView {
    union ModelDefinitionReferenceOrSavedId definitionReferenceOrSavedId; 
    uint32_t rootModelNodeSavedOffset; 
    uint32_t ownerArmyRuntimeSavedOffset; 
    uint32_t attachmentCount; 
    uint8_t classPrefixState[40]; 
    uint32_t linkedModelRuntimeSavedOffset; 
    uint32_t health; /* +0x3C current health; starts at ModelDefinition.maximumHealth */
    uint32_t destructionEffectTimers[8]; // +0x40 destruction effect channel timers, copied from ModelDefinition.destructionEffectDelayTicks0..7
    struct ModelRuntimeClassLinkState classLinkState; 
    struct ModelRuntimeSlotClassStateSerializedScalar classState; 
    uint32_t researchTechnologyId; /* +0x100 see ModelRuntimeSlot */
    int researchDurationTicks;
    int researchElapsedTicks;
    int researchEnergyLoadQ4;
    int researchXeniteCostQ4;
    uint8_t reserved114_117[4];
    uint32_t classState118; 
    uint32_t classState11C; 
    uint8_t reserved120_13F[32]; 
    struct ModelRuntimeAttachmentSavedDescriptor attachments[6]; 
};

union ModelRaycastNearestNodeOrScratch4 {
    Ptr32<struct ModelRuntimeNode> nearestModelNode; // nearest node, valid when the raycast reports a hit
    int scratchSigned; // arithmetic/scratch value when there is no hit or before the hit is stored
};

struct ModelRelativeDirectionAngles {
    AngleTurn32 relativeYawAngle; // Wrapped yaw/azimuth relative to model local rotation.
    AngleTurn32 relativePitchAngle; // Transformed elevation/pitch angle retained from FixedMath_VectorToAngles.
};

#endif /* THANDOR_WORLD_MODEL_TYPES_H */

/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/include/thandor/gameplay/army/runtime.h
 * Reverse engineering by idkFoxes 2026
 */

#ifndef THANDOR_GAMEPLAY_ARMY_RUNTIME_H
#define THANDOR_GAMEPLAY_ARMY_RUNTIME_H

#include <thandor/generated/types.h>
#include <thandor/core/contracts.h>

/* Submodule: gameplay/army/runtime. */

/* ArmyRuntimeSlot.runtimeFlags bit set when the army's health (actionVector2Q12) drops to zero
   (ArmyRuntime_ApplyImpactDamageAndFinalizeState and the other damage helpers). */
#define ARMY_RUNTIME_FLAG_DESTROYED 0x8
/* g_ArmyRuntimeSlots: a 0x48000-byte pool of 0x120-byte ArmyRuntimeSlot entries (ArmyRuntime_InitializePoolAndGraphics) */
#define ARMY_RUNTIME_SLOT_COUNT 0x400
/* g_ArmyGraphicsBindings: texture set and palette per faction slot 0-7 */
#define ARMY_GRAPHICS_BINDING_COUNT 8
/* ArmyRuntime_CreateInstanceFromAsset creationFlags */
#define ARMY_CREATE_COUNT_FOR_ACTIVE_FACTION 0x2 /* owned by the active faction: +1 on the counter at +0x1B0 of
                                                     the faction's selected model definition */
#define ARMY_CREATE_UNLOCK_TECHNOLOGY 0x4 /* ModelDefinitionHierarchy_UnlockSelectedLinkedTechnology */
/* ArmyRuntimeSlot.commandModeFlags: what the current command targets (ArmyRuntime_ResolveCommandTarget,
   ArmyRuntime_ApplyTargetPositionCommand, ArmyRuntimeCommand_InterruptActiveTargetAndStampGeneration) */
#define ARMY_COMMAND_MODE_TARGET_ARMY 0x1     /* commandTargetArmyRuntime */
#define ARMY_COMMAND_MODE_TARGET_POSITION 0x2 /* commandCoordinate0-2Q12 */
#define ARMY_COMMAND_MODE_INTERRUPTED 0x4     /* a target command was cancelled; commandGeneration re-stamped */
#define ARMY_COMMAND_MODE_AI_COMBAT_TARGET 0x8 /* target picked by the AI combat target selection: target-following
                                                  moves are clamped (ArmyRuntime_StartClampedMoveCommand) */
/* Army model runtime stateFlags (+0xEC) bits set and tested by the class update callbacks
   (ArmyRuntimeHierarchy_UpdateProgressAndClassCallbacksRecursive, the production slots 11/13/22) */
#define ARMY_MODEL_STATE_SWITCHED_OFF 0x1         /* powered down: no Energy demand, health decays to 3/4 */
#define ARMY_MODEL_STATE_DISMANTLING 0x10         /* being recycled: health drains, Xenite (+0x184 >> 5) is refunded */
#define ARMY_MODEL_STATE_DESTRUCTION_STARTED 0x20 /* destruction effect spawned; skips the attachment channel ticks */
#define ARMY_MODEL_STATE_RESEARCHING 0x40         /* technology research in progress (+0x100 record) */
#define ARMY_MODEL_STATE_RESEARCH_UNPAID 0x80     /* research queued, Xenite not yet paid */
#define ARMY_MODEL_STATE_PRODUCING 0x100          /* a queued secondary army asset is being built */
#define ARMY_MODEL_STATE_DISMANTLED 0x200         /* dismantling finished (toggled together with DISMANTLING) */
#define ARMY_MODEL_STATE_NO_REGENERATION 0x400    /* health does not regenerate */
#define ARMY_MODEL_STATE_RALLY_POINT_SET 0x800    /* class 13: the exit point (+0x78/+0x7C) was set by the player */
/* SWITCHED_OFF | DESTROYED: the model does nothing this tick */
#define ARMY_MODEL_STATE_INACTIVE_MASK (ARMY_MODEL_STATE_SWITCHED_OFF | ARMY_RUNTIME_FLAG_DESTROYED)
/* INACTIVE_MASK | RESEARCHING | RESEARCH_UNPAID: a production class may start a new build */
#define ARMY_MODEL_STATE_BUILD_BLOCKING_MASK \
          (ARMY_MODEL_STATE_INACTIVE_MASK | ARMY_MODEL_STATE_RESEARCHING | ARMY_MODEL_STATE_RESEARCH_UNPAID)
/* Aircraft (class 21) state at model runtime +0xB8 (ArmyRuntimeClass_UpdateAircraft) */
#define ARMY_AIRCRAFT_STATE_NO_PAD 0       /* no home pad left: stays where it is */
#define ARMY_AIRCRAFT_STATE_PARKED 1       /* parked on the home pad */
#define ARMY_AIRCRAFT_STATE_LANDING 2      /* landing arc back onto the pad */
#define ARMY_AIRCRAFT_STATE_TAKING_OFF 3   /* take-off arc, then off the map */
#define ARMY_AIRCRAFT_STATE_APPROACH 4     /* off the map: lines up behind the attack point */
#define ARMY_AIRCRAFT_STATE_ATTACK_RUN 5   /* flies over the attack point, dropping its effects */
#define ARMY_AIRCRAFT_STATE_RETURNING 6    /* off the map again: lines up for the landing arc */
/* Aircraft pad (class 22) hangar state at model runtime +0xB0
   (ArmyRuntimeClass_UpdateLinkedModelFlagsAndDispatchTerrainContactMode) */
#define ARMY_PAD_HANGAR_IDLE 0
#define ARMY_PAD_HANGAR_OPENING 1          /* hatch texture scrolls open */
#define ARMY_PAD_HANGAR_LIFTING 2          /* platform (child 0) moves up */
#define ARMY_PAD_HANGAR_READY 3            /* platform up: a parked aircraft takes off */
#define ARMY_PAD_HANGAR_LOWERING 4
#define ARMY_PAD_HANGAR_CLOSING 5
#define ARMY_PAD_HANGAR_CLOSED 6           /* back to idle on the next update */
/* Production state at model runtime +0xB8 of the unit factory (class 13) and production class 11 (0/1 only) */
#define ARMY_FACTORY_STATE_IDLE 0
#define ARMY_FACTORY_STATE_BUILDING 1
#define ARMY_FACTORY_STATE_OPENING 2       /* door opens, then the new army is sent to the exit point */
#define ARMY_FACTORY_STATE_WAITING_EXIT 3  /* until the new army no longer links back to the factory */
#define ARMY_FACTORY_STATE_CLOSING 4
/* primaryTextureOffsetV of a fully open door or hatch (texture V offset, 0 = closed) */
#define ARMY_DOOR_TEXTURE_OPEN_V 0x80000
/* Parking position of an aircraft that has flown off the map (ArmyRuntimeClass_UpdateAircraft) */
#define ARMY_AIRCRAFT_OFF_MAP_X_Q12 (-0x100000)
#define ARMY_AIRCRAFT_OFF_MAP_Y_Q12 0x100000
/* ArmyAssetRecord.flags bits that select the production class able to build a queued secondary asset */
#define ARMY_ASSET_FLAG_BUILT_AT_AIRCRAFT_PAD 0x8 /* class-22 aircraft pad */
#define ARMY_ASSET_FLAG_BUILT_BY_CLASS11 0x10     /* production class 11 */
/* impact damage of a model crushed by a structure placed over it or run over by a vehicle
   (ArmyRuntime_ClassCommandHandlerGroupA, ArmyRuntime_HandleCollisionPartner) */
#define ARMY_CRUSH_IMPACT_DAMAGE 0x100000
/* Functions are grouped by semantic ownership; address comments are executable virtual addresses. */

/* 0x00525A60 */
void ArmyRuntimeClass_UpdateAircraft (WorldRuntimeContext *worldRuntime,ModelRuntimeClass21UpdateView *modelRuntime);

/* 0x00526620 */
void ArmyRuntimeClass_UpdateLinkedModelFlagsAndDispatchTerrainContactMode (WorldRuntimeContext *worldRuntime, ModelRuntimeLinkedChildSpawnAndBuildView *modelRuntime);

/* 0x00524740 */
void ArmyRuntimeClass_UpdateUnitFactory
          (WorldRuntimeContext *worldRuntime,ModelRuntimeUpdateView *modelRuntime);

/* 0x005240F0 */
void ArmyRuntimeClass_UpdateStructureFactory
          (WorldRuntimeContext *worldRuntime,ModelRuntimeUpdateView *modelRuntime);

/* 0x00525020 */
void ArmyRuntimeClass_UpdateGridBoundEffectsAndModels
          (WorldRuntimeContext *worldRuntime,ModelRuntimeClass14UpdateView *modelRuntime);

/* 0x005274D0 */
void ArmyRuntime_ClassCommandHandlerGroupA(WorldRuntimeContext *worldRuntime,ArmyRuntimeSlot *armyRuntime);

/* 0x0051D140 */
void ArmyRuntimeMaintenance_InitializeOccupancyAndStateTint
          (WorldRuntimeContext *worldRuntime,ModelRuntimeNode *modelNodeRuntime);

/* 0x0051D280 */
void ArmyRuntimeMaintenance_DispatchClassMethodDRecursive
          (WorldRuntimeContext *worldRuntime,WorldOwnerListNode *ownerNode);

/* 0x0051D2A0 */
void ArmyRuntimeMaintenance_UpdateHierarchyAiAndTimers
          (WorldRuntimeContext *worldRuntime,WorldOwnerListNode *ownerNode);

/* 0x0051D6B0 */
ArmyRuntimeInitResult ArmyRuntime_InitializePoolAndGraphics(void *ownerContext,uint16_t *graphicsBasePath);

/* 0x00528330 */
void ArmyRuntimeClass_UpdateEffectsAndDestroyModelHierarchy (WorldRuntimeContext *worldRuntime,ModelRuntimeDestroyEffectsView *modelRuntime);

/* 0x00531130 */
void ArmyRuntimeNode_RebuildTerrainOccupancyAndVisualStateCallback
          (WorldRuntimeContext *armyContext,WorldOwnerListNode *node);

/* 0x0051C3B0 */
void ArmyRuntime_SetNonzeroActionVector
          (Q12 actionVector0,Q12 actionVector2,Q12 actionVector1,ArmyRuntimeSlot *armyRuntime);

/* 0x0051C540 */
void ArmyRuntime_ResolveCommandTarget(ArmyRuntimeSlot *targetArmyRuntime,ArmyRuntimeSlot *armyRuntime);

/* 0x0051C620 */
void ArmyRuntime_ApplyTargetPositionCommand
          (Q12 coordinate2Q12,Q12 coordinate1Q12,Q12 coordinate0Q12,ArmyRuntimeSlot *armyRuntime);

/* 0x0051C720 */
WorldPositionResult
ArmyRuntime_ResolveShotAimPoint
          (Q12 sourceWorldZQ12,Q12 sourceWorldYQ12,Q12 sourceWorldXQ12,
          ShotDefinition *shotDefinition,GameEntityRuntime *targetState);

/* 0x0051D170 */
void ArmyRuntimeNode_AccumulateTerrainOcclusionAndOccupancyCallback
          (WorldRuntimeContext *worldRuntime,WorldOwnerListNode *node);

/* 0x0051D310 */
bool ArmyRuntime_TestHasNoWeaponDamage(ArmyRuntimeSlot *armyRuntime);

/* 0x0051D330 */
bool ArmyRuntime_TestWeaponDamageNonnegative(ArmyRuntimeSlot *armyRuntime);

/* 0x0051D350 */
bool ArmyRuntimeNode_DispatchTypedCallback(ArmyRuntimeSlot **armyRuntimeHolder,WorldRuntimeContext *worldRuntime);

/* 0x0051D4D0 */
void ArmyRuntime_DispatchClassCommand(ArmyRuntimeSlot **armyRuntimeHolder,WorldRuntimeContext *worldRuntime);

/* 0x0051D8C0 */
void ArmyRuntime_ShutdownPoolAndGraphics(void);

/* 0x0051D960 */
RuntimeImagePointerByteSizeEdxEax8 __cdecl ArmyRuntimePool_ConvertPointersToOffsetsForSaveRegs(void);

/* 0x0051D9F0 */
void ArmyRuntimePool_RebaseAfterLoad(void);

/* 0x00520CF0 */
void ArmyRuntimeClass_UpdateGroundPositionedSounds (WorldRuntimeContext *worldRuntime, ArmyRuntimeGroundMovementPositionedSoundView *armyRuntime);

/* 0x00522B70 */
void ArmyRuntimeClass_NoOpUpdate(WorldRuntimeContext *worldRuntime,ArmyRuntimeSlot *armyRuntime);

/* 0x00523E70 */
void ArmyRuntimeClass_SelectProjectileTargetNode (ModelRuntimeTimedTargetProjectileView *modelRuntime, WorldOwnerListNode *candidateNode);

/* 0x00523FC0 */
void ArmyRuntimeClass_UpdateTimedTargetProjectilesAndEffects (WorldRuntimeContext *worldRuntime,ModelRuntimeTimedTargetProjectileView *modelRuntime);

/* 0x00525960 */
void ArmyRuntimeClass_UpdateWaterPositionedSounds (WorldRuntimeContext *worldRuntime, ArmyRuntimeGroundMovementPositionedSoundView *armyRuntime);

/* 0x00526FE0 */
ArmySegmentMeter ArmyRuntime_GetLinkedChildSlotMeterRegs(ArmyRuntimeSlot *armyRuntime);

/* 0x00527150 */
int ArmyRuntime_AccumulateAttachmentEffectVariantMaskRegs(ArmyRuntimeSlot *armyRuntime);

/* 0x00527FE0 */
void ArmyRuntime_UpdateLoopingPositionedSound(WorldRuntimeContext *worldRuntime,ArmyRuntimeSlot *armyRuntime);

/* 0x005283D0 */
void ArmyRuntimeClass_UpdateVerticalDeploymentAndCollisionState (WorldRuntimeContext *worldRuntime,ModelRuntimeVerticalDeploymentView *modelRuntime);

/* 0x00529720 */
bool ArmyRuntime_ResolveShotLaunchFromModelAttachment
          (ShotTargetModelReference targetModelReference,Q12 targetWorldXQ12,Q12 targetWorldYQ12,
          Q12 targetWorldZQ12,SprAttachmentSelectorOrdinal attachmentSelectorOrdinal,
          ShotDefinition *shotDefinition,ModelRuntimeNode *modelNode,
          MdlSerializedNodeHeader *definitionNode,WorldRuntimeContext *worldRuntime);

/* 0x00529B50 */
void ArmyRuntime_UpdateActivationMetricAndPlayStartSound
          (WorldRuntimeContext *worldRuntime,ArmyRuntimeSlot *armyRuntime);

/* 0x0052A040 */
void ArmyRuntime_HandleCollisionPartner(ArmyRuntimeSlot *currentArmyRuntime,Q12 currentWorldYQ12,Q12 currentWorldXQ12,
          ArmyRuntimeSlot *collisionPartnerArmyRuntime,WorldRuntimeContext *worldRuntime);

/* 0x0051BC00 */
ArmyPreviewTextureResult ArmyRuntime_RenderPreviewTexture
          (GraphicsPixelDimension previewHeight,GraphicsPixelDimension previewWidth,
          FactionRuntimeIndex factionIndex,PckArmyAssetIdCatalog armyAssetId,
          WorldRuntimeContext *worldRuntime);

/* 0x0052A7C0 */
void ArmyRuntimeHierarchy_UpdateProgressAndClassCallbacksRecursive
          (WorldRuntimeContext *worldRuntime,ModelRuntimeSlot *modelRuntime);

/* 0x00527010 */
bool ArmyRuntimeSpawner_CreateLinkedChildInstance
          (WorldMotionValue78 inheritedValue78,WorldMotionValue74 inheritedValue74,
          WorldMotionValue70 inheritedValue70,PckArmyAssetIdCatalog linkedArmyAssetId,
          WorldRuntimeContext *worldRuntime,ArmyRuntimeLinkedChildMaskSlotView *armyRuntime);

/* 0x00527430 */
bool ArmyRuntime_TestModelAttachmentProximity(ArmyRuntimeSlot *candidateArmyRuntime,ArmyRuntimeSlot *sourceArmyRuntime);

/* 0x0051C040 */
void ArmyRuntime_DestroyInstanceAndRefreshUi(WorldRuntimeContext *worldRuntime,GameEntityRuntime *entityRuntime);

/* 0x005246B0 */
bool ArmyRuntime_TestArmyNearFactoryExit
          (ArmyRuntimeSlot *candidateArmyRuntime,ArmyRuntimeSlot *sourceArmyRuntime);

/* 0x00526510 */
void ArmyRuntime_TryPlayMappedTerrainSoundAtWorldPoint(FactionRuntimeIndex factionIndex,Q12 worldYQ12,Q12 worldXQ12,
          SoundAssetIndex soundAssetIndex,WorldRuntimeContext *worldContext);

/* 0x005271A0 */
uint32_t ArmyRuntimeSpawner_ComputeRemainingLinkedAssetMetric(ArmyRuntimeLinkedChildMaskSlotView *armyRuntime);

/* 0x00527230 */
void ArmyRuntime_PlayDefinitionOneShotSound(ArmyRuntimeSlot *armyRuntime,WorldRuntimeContext *worldRuntime);

/* 0x005272B0 */
void ArmyRuntime_TrySpawnDefinitionEffectAtWorldPoint(ArmyRuntimeSlot *armyRuntime,WorldRuntimeContext *worldContext);

/* 0x005273D0 */
bool ArmyRuntime_TestPositionDistanceWithinCombinedRadius
          (UQ12 candidateRadiusQ12,UQ12 sourceRadiusQ12,void *candidateModelNode,
          void *sourceModelNode);

/* 0x005297D0 */
void ArmyRuntime_SpawnIndexedModelPointEffectNearCandidate
          (EffectCreationFlagBits effectFlags,Q12 worldZQ12,Q12 worldYQ12,Q12 worldXQ12,
          ModelAttachmentOrdinal modelPointOrdinal,PckEffectDefinitionIdCatalog effectDefinitionId,
          void *sourceRuntime,void *modelPointTable,WorldRuntimeContext *worldContext);

/* 0x00529980 */
void ArmyRuntime_ProcessReadyAttachmentChannels(WorldRuntimeContext *worldRuntime,ModelRuntimeSlot *modelRuntime);

/* 0x0052A760 */
void ArmyRuntimeHierarchy_DispatchClassMethodDRecursive(WorldRuntimeContext *worldRuntime,ArmyRuntimeSlot *armyRuntime);

/* 0x0051C350 */
void ArmyRuntime_RebuildDerivedSelectionMetrics(ArmyRuntimeSlot *armyRuntime);

/* 0x0051DBA0 */
bool ArmyRuntime_TestWorldPointAllowedDefault(uint32_t allowedContext,uint32_t worldYQ12,uint32_t worldXQ12);

/* 0x0051B8F0 */
ArmyRuntimeCreateResult ArmyRuntime_CreateInstanceFromAsset
          (WorldObjectAllocationFlags creationFlags,AngleTurn32 orientationAngle,Q12 worldXQ12,
          Q12 worldYQ12,FactionRuntimeIndex factionIndex,PckArmyAssetIdCatalog armyAssetId,
          WorldRuntimeContext *worldRuntime);

/* 0x0051D0B0 */
void ArmyRuntime_InitializeTerrainOccupancyFlags (WorldRuntimeContext *worldRuntime,ArmyRuntimeSlot *armyRuntime);

/* 0x00527E70 */
void ArmyRuntime_UpdateAnimatedModelSubnodes(WorldRuntimeContext *worldRuntime,ModelRuntimeUpdateView *modelRuntime);

/* 0x00527C00 */
void ArmyRuntime_UpdateTimedShotAndEffectEmitters
          (WorldRuntimeContext *worldRuntime,ModelRuntimeUpdateView *modelRuntime);

#endif /* THANDOR_GAMEPLAY_ARMY_RUNTIME_H */

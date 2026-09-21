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
/* Functions are grouped by semantic ownership; address comments are executable virtual addresses. */

/* 0x00525A60 */
void __thandor_void_preserve_eax_ecx_edx ArmyRuntimeClassUpdateSlot21_DispatchByClassId (WorldRuntimeContext *worldRuntime,ModelRuntimeClass21UpdateView200 *modelRuntime);

/* 0x00526620 */
void __thandor_void_preserve_eax_ecx_edx ArmyRuntimeClass_UpdateLinkedModelFlagsAndDispatchTerrainContactMode (WorldRuntimeContext *worldRuntime, ModelRuntimeLinkedChildSpawnAndBuildView200 *modelRuntime);

/* 0x00524740 */
void __thandor_void_preserve_eax_ecx_edx
ArmyRuntimeClassUpdateSlot13_PrepareModelAndDispatchByClassId
          (WorldRuntimeContext *worldRuntime,ModelRuntimeUpdateView200 *modelRuntime);

/* 0x005240F0 */
void __thandor_void_preserve_eax_ecx_edx
ArmyRuntimeClassUpdateSlot11_DispatchByClassId
          (WorldRuntimeContext *worldRuntime,ModelRuntimeUpdateView200 *modelRuntime);

/* 0x00525020 */
void __thandor_void_preserve_eax_ecx_edx
ArmyRuntimeClass_UpdateGridBoundEffectsAndModels
          (WorldRuntimeContext *worldRuntime,ModelRuntimeClass14UpdateView200 *modelRuntime);

/* 0x005274D0 */
void __thandor_void_preserve_eax_ecx_edx
ArmyRuntime_ClassCommandHandlerGroupACf
          (WorldRuntimeContext *worldRuntime,ArmyRuntimeSlot *armyRuntime);

/* 0x0051D140 */
void __thandor_void_preserve_eax_ecx_edx
ArmyRuntimeMaintenance_InitializeOccupancyAndStateTint
          (WorldRuntimeContext *worldRuntime,ModelRuntimeNode *modelNodeRuntime);

/* 0x0051D280 */
void __thandor_void_preserve_eax_ecx_edx
ArmyRuntimeMaintenance_DispatchClassMethodDRecursive
          (WorldRuntimeContext *worldRuntime,ArmyRuntimeSlot *armyRuntime);

/* 0x0051D2A0 */
void __thandor_void_preserve_eax_ecx_edx
ArmyRuntimeMaintenance_UpdateHierarchyAiAndTimers
          (WorldRuntimeContext *worldRuntime,WorldOwnerListNode100 *ownerNode);

/* 0x0051D6B0 */
ArmyRuntimeInitEaxCf5 __thandor_eax_cf_preserve_ecx_edx
ArmyRuntime_InitializePoolAndGraphicsCf(void *ownerContext,word *graphicsBasePath);

/* 0x00528330 */
void __thandor_void_preserve_eax_ecx_edx ArmyRuntimeClass_UpdateEffectsAndDestroyModelHierarchy (WorldRuntimeContext *worldRuntime,ModelRuntimeDestroyEffectsView200 *modelRuntime);

/* 0x00531130 */
void __thandor_preserve_eax_edx
ArmyRuntimeNode_RebuildTerrainOccupancyAndVisualStateCallback
          (WorldRuntimeContext *armyContext,WorldOwnerListNode100 *node);

/* 0x0051C3B0 */
void __thandor_void_preserve_eax_ecx_edx
ArmyRuntime_SetNonzeroActionVector
          (Q12 actionVector0,Q12 actionVector2,Q12 actionVector1,ArmyRuntimeSlot *armyRuntime);

/* 0x0051C540 */
void __thandor_void_preserve_eax_ecx_edx
ArmyRuntime_ResolveCommandTarget(ArmyRuntimeSlot *targetArmyRuntime,ArmyRuntimeSlot *armyRuntime);

/* 0x0051C620 */
void __thandor_void_preserve_eax_ecx_edx
ArmyRuntime_ApplyTargetPositionCommand
          (Q12 coordinateA,Q12 coordinateB,Q12 coordinateC,ArmyRuntimeSlot *armyRuntime);

/* 0x0051C720 */
WorldPositionEaxEcxEdxCf13
ArmyRuntime_ResolveShotAimPointCf
          (Q12 sourceWorldZQ12,Q12 sourceWorldYQ12,Q12 sourceWorldXQ12,
          ShotDefinition *shotDefinition,GameEntityRuntime *targetState);

/* 0x0051D170 */
void __thandor_void_preserve_eax_ecx_edx
ArmyRuntimeNode_AccumulateTerrainOcclusionAndOccupancyCallback
          (WorldRuntimeContext *worldRuntime,WorldOwnerListNode100 *node);

/* 0x0051D310 */
bool __thandor_cf_preserve_eax_ecx_edx
ArmyRuntime_TestStateField100ZeroCf(ArmyRuntimeSlot *runtimeState);

/* 0x0051D330 */
bool __thandor_cf_preserve_eax_ecx_edx
ArmyRuntime_TestStateField100NonnegativeCf(ArmyRuntimeSlot *armyRuntime);

/* 0x0051D350 */
bool __thandor_cf_preserve_eax_edx
ArmyRuntimeNode_DispatchTypedCallback
          (ArmyRuntimeSlot **modelRuntimeHolder,WorldRuntimeContext *worldRuntime);

/* 0x0051D4D0 */
void __thandor_void_preserve_eax_ecx_edx
ArmyRuntime_DispatchClassCommand
          (ArmyRuntimeSlot **armyRuntimeHolder,WorldRuntimeContext *worldRuntime);

/* 0x0051D8C0 */
void __thandor_void_preserve_eax_ecx_edx ArmyRuntime_ShutdownPoolAndGraphics(void);

/* 0x0051D960 */
RuntimeImagePointerByteSizeEdxEax8 __cdecl ArmyRuntimePool_ConvertPointersToOffsetsForSaveRegs(void);

/* 0x0051D9F0 */
void __thandor_void_preserve_eax_ecx_edx ArmyRuntimePool_RebaseAfterLoad(void);

/* 0x00520CF0 */
void __thandor_void_preserve_eax_ecx_edx ArmyRuntimeClass_UpdatePositionedSoundsVariantA (WorldRuntimeContext *worldRuntime, ArmyRuntimeGroundMovementPositionedSoundView120 *armyRuntime);

/* 0x00522B70 */
void __thandor_void_preserve_eax_ecx_edx
ArmyRuntimeClass_NoOpUpdate(WorldRuntimeContext *worldRuntime,ArmyRuntimeSlot *armyRuntime);

/* 0x00523E70 */
void __thandor_void_preserve_eax_ecx_edx ArmyRuntimeClass_SelectProjectileTargetNode (ModelRuntimeTimedTargetProjectileView200 *modelRuntime, WorldOwnerListNode100 *candidateNode);

/* 0x00523FC0 */
void __thandor_void_preserve_eax_ecx_edx ArmyRuntimeClass_UpdateTimedTargetProjectilesAndEffects (WorldRuntimeContext *worldRuntime,ModelRuntimeTimedTargetProjectileView200 *modelRuntime);

/* 0x00525960 */
void __thandor_void_preserve_eax_ecx_edx ArmyRuntimeClass_UpdatePositionedSoundsVariantB (WorldRuntimeContext *worldRuntime, ArmyRuntimeGroundMovementPositionedSoundView120 *armyRuntime);

/* 0x00526FE0 */
ArmyMetric6CDefinitionC4Regs8 __thandor_regs_ebx_ecx_preserve_eax_edx
ArmyRuntime_QueryMetric6CAndDefinitionC4Regs(ArmyRuntimeSlot *armyRuntime);

/* 0x00527150 */
int __thandor_void_preserve_eax_ecx_edx
ArmyRuntime_AccumulateAttachmentEffectVariantMaskRegs(ArmyRuntimeSlot *armyRuntime);

/* 0x00527FE0 */
void __thandor_void_preserve_eax_ecx_edx
ArmyRuntime_UpdateLoopingPositionedSound
          (WorldRuntimeContext *worldRuntime,ArmyRuntimeSlot *armyRuntime);

/* 0x005283D0 */
void __thandor_void_preserve_eax_ecx_edx ArmyRuntimeClass_UpdateVerticalDeploymentAndCollisionState (WorldRuntimeContext *worldRuntime,ModelRuntimeVerticalDeploymentView200 *modelRuntime);

/* 0x00529720 */
bool __thandor_cf_preserve_eax_ecx_edx
ArmyRuntime_ResolveShotLaunchFromModelAttachmentCf
          (ShotRuntimeState14 shotRuntimeState14,Q12 targetWorldXQ12,Q12 targetWorldYQ12,
          Q12 targetWorldZQ12,SprAttachmentSelectorOrdinal attachmentSelectorOrdinal,
          ShotDefinition *shotDefinition,ModelRuntimeNode *modelNode,
          MdlSerializedNodeHeader38 *definitionNode,WorldRuntimeContext *worldRuntime);

/* 0x00529B50 */
void __thandor_void_preserve_eax_ecx_edx
ArmyRuntime_UpdateActivationMetricAndPlayStartSound
          (WorldRuntimeContext *worldRuntime,ArmyRuntimeSlot *armyRuntime);

/* 0x0052A040 */
void __thandor_void_preserve_eax_ecx_edx
ArmyRuntime_HandleCollisionPartner
          (ArmyRuntimeSlot *currentArmyRuntime,Q12 currentWorldXQ12,Q12 currentWorldYQ12,
          ArmyRuntimeSlot *collisionPartnerArmyRuntime,WorldRuntimeContext *worldRuntime);

/* 0x0051BC00 */
ArmyPreviewTextureEaxCf5 __thandor_eax_cf_preserve_ecx_edx
ArmyRuntime_RenderPreviewTextureCf
          (GraphicsPixelDimension previewHeight,GraphicsPixelDimension previewWidth,
          FactionRuntimeIndex factionIndex,PckArmyAssetIdCatalog armyAssetId,
          WorldRuntimeContext *worldRuntime);

/* 0x0052A7C0 */
void __thandor_void_preserve_eax_ecx_edx
ArmyRuntimeHierarchy_UpdateProgressAndClassCallbacksRecursive
          (WorldRuntimeContext *worldRuntime,ModelRuntimeSlot *modelRuntime);

/* 0x00527010 */
bool __thandor_cf_preserve_eax_ecx_edx
ArmyRuntimeSpawner_CreateLinkedChildInstanceCf
          (WorldMotionValue78 inheritedValue78,WorldMotionValue74 inheritedValue74,
          WorldMotionValue70 inheritedValue70,PckArmyAssetIdCatalog linkedArmyAssetId,
          WorldRuntimeContext *worldRuntime,ArmyRuntimeLinkedChildMaskSlotView *armyRuntime);

/* 0x00527430 */
bool __thandor_cf_preserve_eax_ecx_edx
ArmyRuntime_TestModelAttachmentProximityCf
          (ArmyRuntimeSlot *candidateArmyRuntime,ArmyRuntimeSlot *sourceArmyRuntime);

/* 0x0051C040 */
void __thandor_void_preserve_eax_ecx_edx
ArmyRuntime_DestroyInstanceAndRefreshUi
          (WorldRuntimeContext *worldRuntime,GameEntityRuntime *entityRuntime);

/* 0x005246B0 */
bool __thandor_cf_preserve_eax_ecx_edx
ArmyRuntime_TestClass13ProximityCandidateCf
          (ArmyRuntimeSlot *candidateArmyRuntime,ArmyRuntimeSlot *sourceArmyRuntime);

/* 0x00526510 */
void __thandor_void_preserve_eax_ecx_edx
ArmyRuntime_TryPlayMappedTerrainSoundAtWorldPoint
          (FactionRuntimeIndex factionIndex,Q12 worldYQ12,Q12 worldXQ12,
          SoundAssetIndex soundAssetIndex,WorldRuntimeContext *worldContext);

/* 0x005271A0 */
uint __thandor_void_preserve_eax_ecx
ArmyRuntimeSpawner_ComputeRemainingLinkedAssetMetric
          (ArmyRuntimeLinkedChildMaskSlotView *armyRuntime);

/* 0x00527230 */
void __thandor_void_preserve_eax_ecx_edx
ArmyRuntimeSpawner_PlayCreationSound(ArmyRuntimeSlot *armyRuntime,WorldRuntimeContext *worldRuntime);

/* 0x005272B0 */
void __thandor_void_preserve_eax_ecx_edx
ArmyRuntime_TrySpawnDefinitionEffectAtWorldPoint
          (ArmyRuntimeSlot *armyRuntime,WorldRuntimeContext *worldContext);

/* 0x005273D0 */
bool __thandor_cf_preserve_eax_ecx_edx
ArmyRuntime_TestPositionDistanceWithinCombinedRadiusCf
          (UQ12 candidateRadiusQ12,UQ12 sourceRadiusQ12,void *candidatePositionRuntime,
          void *sourcePositionRuntime);

/* 0x005297D0 */
void __thandor_void_preserve_eax_ecx_edx
ArmyRuntime_SpawnIndexedModelPointEffectNearCandidate
          (EffectCreationFlagBits effectFlags,Q12 worldZQ12,Q12 worldYQ12,Q12 worldXQ12,
          ModelAttachmentOrdinal modelPointOrdinal,PckEffectDefinitionIdCatalog effectDefinitionId,
          void *sourceRuntime,void *modelPointTable,WorldRuntimeContext *worldContext);

/* 0x00529980 */
void __thandor_void_preserve_eax_ecx_edx
ArmyRuntime_ProcessReadyAttachmentChannels
          (WorldRuntimeContext *worldRuntime,ModelRuntimeSlot *modelRuntime);

/* 0x0052A760 */
void __thandor_void_preserve_eax_ecx_edx
ArmyRuntimeHierarchy_DispatchClassMethodDRecursive
          (WorldRuntimeContext *worldRuntime,ArmyRuntimeSlot *armyRuntime);

/* 0x0051C350 */
void __thandor_void_preserve_eax_ecx
ArmyRuntime_RebuildDerivedSelectionMetrics(ArmyRuntimeSlot *armyRuntime);

/* 0x0051DBA0 */
bool __thandor_cf_preserve_eax_ecx_edx
ArmyRuntime_TestWorldPointAllowedDefaultCf(dword arg0,dword arg1,dword arg2);

/* 0x0051B8F0 */
ArmyRuntimeCreateEaxCf5 __thandor_eax_cf_preserve_ecx_edx
ArmyRuntime_CreateInstanceFromAssetCf
          (WorldObjectAllocationFlags creationFlags,AngleTurn32 orientationAngle,Q12 worldXQ12,
          Q12 worldYQ12,FactionRuntimeIndex factionIndex,PckArmyAssetIdCatalog armyAssetId,
          WorldRuntimeContext *worldRuntime);

/* 0x0051D0B0 */
void ArmyRuntime_InitializeTerrainOccupancyFlags (WorldRuntimeContext *worldRuntime,ArmyRuntimeSlot *armyRuntime);

/* 0x00527E70 */
void __thandor_void_preserve_eax_ecx_edx
ArmyRuntime_UpdateAnimatedModelSubnodes
          (WorldRuntimeContext *worldRuntime,ModelRuntimeUpdateView200 *modelRuntime);

/* 0x00527C00 */
void __thandor_void_preserve_eax_ecx_edx
ArmyRuntime_UpdateTimedShotAndEffectEmitters
          (WorldRuntimeContext *worldRuntime,ModelRuntimeUpdateView200 *modelRuntime);

#endif /* THANDOR_GAMEPLAY_ARMY_RUNTIME_H */

#ifndef THANDOR_GAMEPLAY_ARMY_RUNTIME_H
#define THANDOR_GAMEPLAY_ARMY_RUNTIME_H

#include <thandor/generated/v523_types.h>
#include <thandor/core/contracts.h>

/* Submodule: gameplay/army/runtime. */
/* Functions are grouped by semantic ownership; address comments are executable virtual addresses. */

/* 0x00525A60 */
void ArmyRuntimeClassUpdateSlot21_DispatchByClassId (WorldRuntimeContext *worldRuntime,ArmyRuntimeSlot *armyRuntime);

/* 0x00526620 */
void ArmyRuntimeClass_UpdateLinkedModelFlagsAndDispatchTerrainContactMode (WorldRuntimeContext *worldRuntime, ArmyRuntimeTerrainContactLinkedChildSlotView120 *armyRuntime);

/* 0x00524740 */
void ArmyRuntimeClassUpdateSlot13_PrepareModelAndDispatchByClassId (WorldRuntimeContext *worldRuntime,ArmyRuntimeSlot *armyRuntime);

/* 0x005240F0 */
void ArmyRuntimeClassUpdateSlot11_DispatchByClassId (WorldRuntimeContext *worldRuntime,ArmyRuntimeSlot *armyRuntime);

/* 0x00525020 */
void ArmyRuntimeClass_UpdateGridBoundEffectsAndModels (WorldRuntimeContext *worldRuntime,ArmyRuntimeSlot *armyRuntime);

/* 0x005274D0 */
void ArmyRuntime_ClassCommandHandlerGroupACf (WorldRuntimeContext *worldRuntime,ArmyRuntimeSlot *armyRuntime);

/* 0x0051D140 */
void ArmyRuntimeMaintenance_InitializeOccupancyAndStateTint (WorldRuntimeContext *worldRuntime,ModelRuntimeNode *modelNodeRuntime);

/* 0x0051D280 */
void ArmyRuntimeMaintenance_DispatchClassMethodDRecursive (WorldRuntimeContext *worldRuntime,ArmyRuntimeSlot *armyRuntime);

/* 0x0051D2A0 */
void ArmyRuntimeMaintenance_UpdateHierarchyAiAndTimers (WorldRuntimeContext *worldRuntime,WorldRuntimeNode *ownerNode);

/* 0x0051D6B0 */
dword ArmyRuntime_InitializePoolAndGraphicsCf(void *ownerContext,word *graphicsBasePath);

/* 0x00528330 */
void ArmyRuntimeClass_UpdateEffectsAndDestroyModelHierarchy (WorldRuntimeContext *worldRuntime,ArmyRuntimeSlot *armyRuntime);

/* 0x00531130 */
void ArmyRuntimeNode_RebuildTerrainOccupancyAndVisualStateCallback (WorldRuntimeContext *armyContext,WorldRuntimeNode *node);

/* 0x0051C3B0 */
void ArmyRuntime_SetNonzeroActionVector (Q12 actionVector0,Q12 actionVector2,Q12 actionVector1,ArmyRuntimeSlot *armyRuntime);

/* 0x0051C540 */
void ArmyRuntime_ResolveCommandTarget (GameEntityRuntime *resolvedTarget,ArmyRuntimeSlot *armyRuntime);

/* 0x0051C620 */
void ArmyRuntime_ApplyTargetPositionCommand (Q12 coordinateA,Q12 coordinateB,Q12 coordinateC,ArmyRuntimeSlot *armyRuntime);

/* 0x0051C720 */
ShotAimXZRegisterPairQ12 ArmyRuntime_ResolveShotAimPointCf (Q12 sourceWorldZQ12,Q12 sourceWorldYQ12,Q12 sourceWorldXQ12, ShotDefinition *shotDefinition,GameEntityRuntime *targetState);

/* 0x0051D170 */
void ArmyRuntimeNode_AccumulateTerrainOcclusionAndOccupancyCallback (WorldRuntimeContext *worldRuntime,WorldRuntimeNode *node);

/* 0x0051D310 */
undefined4 ArmyRuntime_TestStateField100ZeroCf(void);

/* 0x0051D330 */
void ArmyRuntime_TestStateField100NonnegativeCf(ArmyRuntimeSlot *armyRuntime);

/* 0x0051D350 */
void ArmyRuntimeNode_DispatchTypedCallback (ArmyRuntimeSlot **modelRuntimeHolder,WorldRuntimeContext *worldRuntime);

/* 0x0051D4D0 */
void ArmyRuntime_DispatchClassCommand (ArmyRuntimeSlot **armyRuntimeHolder,WorldRuntimeContext *worldRuntime);

/* 0x0051D8C0 */
undefined8 __cdecl ArmyRuntime_ShutdownPoolAndGraphics(void);

/* 0x0051D960 */
RuntimeImagePointerByteSizeEdxEax8 __cdecl ArmyRuntimePool_ConvertPointersToOffsetsForSaveRegs(void);

/* 0x0051D9F0 */
void __cdecl ArmyRuntimePool_RebaseAfterLoad(void);

/* 0x00520CF0 */
void ArmyRuntimeClass_UpdatePositionedSoundsVariantA (WorldRuntimeContext *worldRuntime,ArmyRuntimeSlot *armyRuntime);

/* 0x00522B70 */
void ArmyRuntimeClass_NoOpUpdate(WorldRuntimeContext *worldRuntime,ArmyRuntimeSlot *armyRuntime);

/* 0x00523E70 */
void ArmyRuntimeClass_SelectProjectileTargetNodeCf (ArmyRuntimeSlot *armyRuntime,WorldRuntimeNode *candidateNode);

/* 0x00523FC0 */
void ArmyRuntimeClass_UpdateTimedTargetProjectilesAndEffects (WorldRuntimeContext *worldRuntime,ArmyRuntimeSlot *armyRuntime);

/* 0x00525960 */
void ArmyRuntimeClass_UpdatePositionedSoundsVariantB (WorldRuntimeContext *worldRuntime,ArmyRuntimeSlot *armyRuntime);

/* 0x00526FE0 */
void ArmyRuntime_QueryMetric6CAndDefinitionC4Regs(void);

/* 0x00527150 */
void ArmyRuntime_AccumulateAttachmentEffectVariantMaskRegs(ArmyRuntimeSlot *armyRuntime);

/* 0x00527FE0 */
void ArmyRuntime_UpdateLoopingPositionedSound (WorldRuntimeContext *worldRuntime,ArmyRuntimeSlot *armyRuntime);

/* 0x005283D0 */
void ArmyRuntimeClass_UpdateVerticalDeploymentAndCollisionState (WorldRuntimeContext *worldRuntime,ArmyRuntimeSlot *armyRuntime);

/* 0x00529720 */
void ArmyRuntime_ResolveShotLaunchFromModelAttachmentCf (ShotRuntimeState14 shotRuntimeState14,Q12 targetWorldXQ12,Q12 targetWorldYQ12, Q12 targetWorldZQ12,SprAttachmentSelectorOrdinal attachmentSelectorOrdinal, ShotDefinition *shotDefinition,ModelRuntimeNode *modelNode, MdlSerializedNodeHeader38 *definitionNode,WorldRuntimeContext *worldRuntime);

/* 0x00529B50 */
void ArmyRuntime_UpdateActivationMetricAndPlayStartSound (WorldRuntimeContext *worldRuntime,ArmyRuntimeSlot *armyRuntime);

/* 0x0052A040 */
void ArmyRuntime_HandleCollisionPartner (ArmyRuntimeSlot *currentArmyRuntime,Q12 currentWorldXQ12,Q12 currentWorldYQ12, ArmyRuntimeSlot *collisionPartnerArmyRuntime,WorldRuntimeContext *worldRuntime);

/* 0x0051BC00 */
undefined8 __fastcall ArmyRuntime_RenderPreviewTextureCf (undefined4 param_1,undefined4 param_2,GraphicsPixelDimension previewHeight, GraphicsPixelDimension previewWidth,FactionRuntimeIndex factionIndex, PckArmyAssetIdCatalog armyAssetId,WorldRuntimeContext *worldRuntime);

/* 0x0052A7C0 */
undefined8 __fastcall ArmyRuntimeHierarchy_UpdateProgressAndClassCallbacksRecursive (undefined4 incomingEcxValue,undefined4 preservedEdxValue, WorldRuntimeContext *worldRuntime,ArmyRuntimeSlot *armyRuntime);

/* 0x00527010 */
undefined8 ArmyRuntimeSpawner_CreateLinkedChildInstanceCf (WorldMotionValue78 inheritedValue78,WorldMotionValue74 inheritedValue74, WorldMotionValue70 inheritedValue70,PckArmyAssetIdCatalog linkedArmyAssetId, WorldRuntimeContext *worldRuntime,ArmyRuntimeLinkedChildMaskSlotView *armyRuntime);

/* 0x00527430 */
void ArmyRuntime_TestModelAttachmentProximityCf (ArmyRuntimeSlot *candidateArmyRuntime,ArmyRuntimeSlot *sourceArmyRuntime);

/* 0x0051C040 */
void ArmyRuntime_DestroyInstanceAndRefreshUi (WorldRuntimeContext *worldRuntime,GameEntityRuntime *entityRuntime);

/* 0x005246B0 */
void ArmyRuntime_TestClass13ProximityCandidateCf (ArmyRuntimeSlot *candidateArmyRuntime,ArmyRuntimeSlot *sourceArmyRuntime);

/* 0x00526510 */
void ArmyRuntime_TryPlayMappedTerrainSoundAtWorldPoint (FactionRuntimeIndex factionIndex,Q12 worldYQ12,Q12 worldXQ12, SoundAssetIndex soundAssetIndex,WorldRuntimeContext *worldContext);

/* 0x005271A0 */
undefined8 ArmyRuntimeSpawner_ComputeRemainingLinkedAssetMetric (ArmyRuntimeLinkedChildMaskSlotView *armyRuntime);

/* 0x00527230 */
void ArmyRuntimeSpawner_PlayCreationSound (ArmyRuntimeSlot *armyRuntime,WorldRuntimeContext *worldRuntime);

/* 0x005272B0 */
void ArmyRuntime_TrySpawnDefinitionEffectAtWorldPoint (ArmyRuntimeSlot *armyRuntime,WorldRuntimeContext *worldContext);

/* 0x005273D0 */
void ArmyRuntime_TestPositionDistanceWithinCombinedRadiusCf (UQ12 candidateRadiusQ12,UQ12 sourceRadiusQ12,void *candidatePositionRuntime, void *sourcePositionRuntime);

/* 0x005297D0 */
void ArmyRuntime_SpawnIndexedModelPointEffectNearCandidate (EffectCreationFlagBits effectFlags,Q12 worldZQ12,Q12 worldYQ12,Q12 worldXQ12, ModelAttachmentOrdinal modelPointOrdinal, PckEffectDefinitionIdCatalog effectDefinitionId,void *sourceRuntime, void *modelPointTable,WorldRuntimeContext *worldContext);

/* 0x00529980 */
void ArmyRuntime_ProcessReadyAttachmentChannels (WorldRuntimeContext *worldRuntime,ArmyRuntimeSlot *armyRuntime);

/* 0x0052A760 */
void ArmyRuntimeHierarchy_DispatchClassMethodDRecursive (WorldRuntimeContext *worldRuntime,ArmyRuntimeSlot *armyRuntime);

/* 0x0051C350 */
void ArmyRuntime_RebuildDerivedSelectionMetrics(ArmyRuntimeSlot *armyRuntime);

/* 0x0051DBA0 */
undefined8 __fastcall ArmyRuntime_TestWorldPointAllowedDefaultCf(undefined4 param_1,undefined4 param_2);

/* 0x0051B8F0 */
ArmyRuntimeSlot * ArmyRuntime_CreateInstanceFromAssetCf (WorldObjectAllocationFlags creationFlags,AngleTurn32 orientationAngle,Q12 worldXQ12, Q12 worldYQ12,FactionRuntimeIndex factionIndex,PckArmyAssetIdCatalog armyAssetId, WorldRuntimeContext *worldRuntime);

/* 0x0051D0B0 */
void ArmyRuntime_InitializeTerrainOccupancyFlags (WorldRuntimeContext *worldRuntime,ArmyRuntimeSlot *armyRuntime);

/* 0x00527E70 */
void ArmyRuntime_UpdateAnimatedModelSubnodes (WorldRuntimeContext *worldRuntime,ArmyRuntimeSlot *armyRuntime);

/* 0x00527C00 */
void ArmyRuntime_UpdateTimedShotAndEffectEmitters (WorldRuntimeContext *worldRuntime,ArmyRuntimeSlot *armyRuntime);

#endif /* THANDOR_GAMEPLAY_ARMY_RUNTIME_H */

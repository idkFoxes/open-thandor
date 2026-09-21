/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/include/thandor/world/model/hierarchy.h
 * Reverse engineering by idkFoxes 2026
 */

#ifndef THANDOR_WORLD_MODEL_HIERARCHY_H
#define THANDOR_WORLD_MODEL_HIERARCHY_H

#include <thandor/generated/types.h>
#include <thandor/core/contracts.h>

/* Submodule: world/model/hierarchy. */
/* Functions are grouped by semantic ownership; address comments are executable virtual addresses. */

/* 0x004BD1F0 */
void __thandor_void_preserve_eax_ecx_edx
ModelNodeRuntime_UpdateStateTintRecursive(ModelRuntimeNode *modelNodeRuntime);

/* 0x004BE360 */
void __thandor_void_preserve_eax_ecx_edx
ModelNodeRuntime_RebuildTransformsFromRoot(ModelRuntimeNode *modelNodeRuntime);

/* 0x0051DB80 */
void __thandor_void_preserve_eax_ecx_edx
ModelRuntimeHierarchy_ApplyFactionTechnologyVariants
          (FactionRuntimeIndex factionIndex,ArmyRuntimeSlot *modelRuntimeHolder);

/* 0x004BD310 */
void __thandor_void_preserve_eax_ecx_edx
ModelNodeRuntime_AccumulateTransformedBoundsRecursive(ModelRuntimeNode *modelNode);

/* 0x004BD8D0 */
void __thandor_void_preserve_eax_ecx_edx
ModelNodeRuntime_BuildViewFacingRotation(ModelRuntimeNode *modelNodeRuntime);

/* 0x004BD950 */
void __thandor_void_preserve_eax_ecx_edx
ModelNodeRuntime_BuildBillboardRotation(ModelRuntimeNode *modelNodeRuntime);

/* 0x004BE9D0 */
void __thandor_void_preserve_eax_ecx_edx
ModelNodeRuntime_RecomputeSubtreeBoundingRadius(ModelRuntimeNode *modelNodeRuntime);

/* 0x004BEA30 */
void __thandor_preserve_eax
ModelNodeRuntime_UpdateDepthBinMasks
          (DepthIntervalRadius32 intervalRadiusQ14,ModelRuntimeNode *modelNodeRuntime);

/* 0x004BEB80 */
ModelLocalPointRegs12
ModelNodeRuntime_TransformLocalPointRegs
          (ModelPackedPointRecord *localPointRecord,ModelRuntimeNode *modelNodeRuntime);

/* 0x004BEBC0 */
ModelRelativeDirectionAnglesEaxEdx8 __thandor_eax_edx_cf_preserve_ecx ModelNodeRuntime_ComputeRelativeDirectionAngle (ModelRuntimeNode *modelNodeRuntime,AngleTurn32 elevationAngle,AngleTurn32 azimuthAngle);

/* 0x0050A7A0 */
StatusValueEaxCf5 __thandor_eax_cf_preserve_ecx_edx
ModelRuntimeNode_HitTestProjectedBoundsAndChildrenCf
          (int pointerY,int pointerX,ModelRuntimeNode *modelNode,
          FrontendModelPointerContextRuntimeState118 *context);

/* 0x0050B1D0 */
ModelRaycastNearestHitEaxEdxCf9 __thandor_eax_edx_cf_preserve_ecx
ModelNodeRuntime_RaycastHierarchyNearestCf(ModelRuntimeNode *modelNodeRuntime);

/* 0x0051B650 */
bool __thandor_cf_preserve_eax_ecx_edx
ModelNodeRuntime_InstantiateLinkedChildrenRecursiveCf
          (FactionRuntimeIndex factionIndex,GraphicsPaletteAsset *paletteAsset,
          GraphicsTextureSet *textureSet,ModelRuntimeSlot *modelRuntimeSlot,
          ModelDefinitionHierarchyNodeAddress32 definitionNode,WorldRuntimeContext *worldRuntime);

/* 0x0051BEC0 */
void __thandor_void_preserve_eax_ecx_edx
ModelRuntimeHierarchy_SetCommandTargetRecursive
          (GraphicsPaletteAsset *paletteAsset,GraphicsTextureSet *textureSet,
          ModelRuntimeNode *modelNode);

/* 0x0051BF30 */
void __thandor_void_preserve_eax_ecx_edx
ModelRuntimeHierarchy_ClearMatchingTargetRecursive(RuntimeToken targetRuntimeId,int *modelRuntime);

/* 0x0051C100 */
void __thandor_void_preserve_eax_ecx_edx
ModelRuntimeHierarchy_ApplyFlags418UnlessBit8Recursive
          (WorldRuntimeContext *contextArg,int *modelRuntime);

/* 0x0051C1F0 */
int __thandor_eax_preserve_ecx_edx ModelRuntimeHierarchy_SumMetric3C(int *modelRuntimeRoot);

/* 0x00528C20 */
ModelRuntimeSlot * __thandor_eax_preserve_ecx_edx
ModelRuntimeHierarchy_CollectAttachmentDescriptorsRecursive
          (ModelRuntimeSlot *modelRuntimeContinuityEdi,ModelRuntimeSlot *modelRuntime,
          MdlSerializedNodeHeader38 *definitionNode);

/* 0x00528E90 */
ModelNodeCreateEaxCf5 __thandor_eax_cf_preserve_ecx_edx
ModelNodeRuntime_CreateHierarchyRecursiveCf
          (GraphicsPaletteAsset *paletteAsset,GraphicsTextureSet *textureSet,
          ModelRuntimeSlot *modelRuntime,MdlSerializedNodeHeader38 *definitionNode,
          WorldRuntimeContext *worldRuntime);

/* 0x005294E0 */
void __thandor_void_preserve_eax_ecx_edx
ModelRuntimeNode_ReleaseRecursiveAndDetachParent(ModelRuntimeNode *node);

/* 0x0052A100 */
void __thandor_void_preserve_eax_ecx_edx
ModelRuntimeHierarchy_AccumulateDerivedSelectionMetrics(int *modelRuntime);

/* 0x0052A690 */
ModelRuntimeScaleRatioRegisterPairQ12 __thandor_eax_edx_cf_preserve_ecx
ModelRuntimeHierarchy_ComputeScaleRatioQ12Regs(ModelRuntimeSlot *modelRuntime);

/* 0x0052A6F0 */
ModelRuntimeActiveTotalMetricRegisterPair ModelRuntimeHierarchy_ComputeActiveAndTotalMetricsRegs(ModelRuntimeSlot *modelRuntime);

/* 0x0052AAC0 */
ModelSmoothEaxCf5 __thandor_eax_cf_preserve_ecx_edx ModelNodeRuntime_SmoothYawTowardTarget (ModelRuntimeNode *modelNodeRuntime,ModelRuntimeWeaponAimStateView200 *smoothingState, AngleTurn32 targetYawAngle16);

/* 0x0052AC00 */
ModelSmoothEaxCf5 __thandor_eax_cf_preserve_ecx_edx ModelNodeRuntime_SmoothPitchTowardTarget (ModelRuntimeNode *modelNodeRuntime,ModelRuntimeWeaponAimStateView200 *smoothingState, AngleTurn32 targetPitchAngle16);

/* 0x004BD1A0 */
void __thandor_void_preserve_eax_ecx_edx
ModelNodeRuntime_ApplyTintRecursive(PackedArgb32 tintArgb,ModelRuntimeNode *modelNode);

/* 0x004BE390 */
void __thandor_void_preserve_eax_ecx_edx
ModelNodeRuntime_ComposeChildTransformsRecursive(ModelRuntimeNode *modelNodeRuntime);

/* 0x0052AEA0 */
void __thandor_void_preserve_eax_ecx_edx
ModelRuntimeHierarchy_ApplyFactionTechnologyVariantsRecursive
          (FactionRuntimeIndex factionIndex,int *modelRuntime);


/* 0x0051D870 */
void __thandor_void_preserve_eax_ecx_edx
ModelRuntimeHierarchy_SetPaletteAndTextureSetRecursive
          (GraphicsPaletteAsset *paletteAsset,GraphicsTextureSet *textureSet,ModelRuntimeNode *node);

#endif /* THANDOR_WORLD_MODEL_HIERARCHY_H */

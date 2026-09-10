#ifndef THANDOR_WORLD_MODEL_HIERARCHY_H
#define THANDOR_WORLD_MODEL_HIERARCHY_H

#include <thandor/generated/v523_types.h>
#include <thandor/core/contracts.h>

/* Submodule: world/model/hierarchy. */
/* Functions are grouped by semantic ownership; address comments are executable virtual addresses. */

/* 0x004BD1F0 */
void ModelNodeRuntime_UpdateStateTintRecursive(ModelRuntimeNode *modelNodeRuntime);

/* 0x004BE360 */
undefined8 __fastcall ModelNodeRuntime_RebuildTransformsFromRoot (undefined4 param_1,undefined4 param_2,ModelRuntimeNode *modelNodeRuntime);

/* 0x0051DB80 */
undefined8 __fastcall ModelRuntimeHierarchy_ApplyFactionTechnologyVariants (undefined4 param_1,undefined4 param_2,FactionRuntimeIndex factionIndex, ArmyRuntimeSlot *modelRuntimeHolder);

/* 0x004BD310 */
void ModelNodeRuntime_AccumulateTransformedBoundsRecursive(ModelRuntimeNode *modelNode);

/* 0x004BD8D0 */
void ModelNodeRuntime_BuildViewFacingRotation(ModelRuntimeNode *modelNodeRuntime);

/* 0x004BD950 */
void ModelNodeRuntime_BuildBillboardRotation(ModelRuntimeNode *modelNodeRuntime);

/* 0x004BE9D0 */
undefined8 ModelNodeRuntime_RecomputeSubtreeBoundingRadius(ModelRuntimeNode *modelNodeRuntime);

/* 0x004BEA30 */
undefined4 __fastcall ModelNodeRuntime_UpdateDepthBinMasks (undefined4 param_1,undefined4 param_2,DepthIntervalRadius32 intervalRadiusQ14, ModelRuntimeNode *modelNodeRuntime);

/* 0x004BEB80 */
undefined8 __thiscall ModelNodeRuntime_TransformLocalPointRegs (undefined4 param_1_00,ModelPackedPointRecord *localPointRecord, ModelRuntimeNode *modelNodeRuntime);

/* 0x004BEBC0 */
uint ModelNodeRuntime_ComputeRelativeDirectionAngle (ModelRuntimeNode *modelNodeRuntime,AngleTurn32 param_2,AngleTurn32 param_3);

/* 0x0050A7A0 */
dword ModelRuntimeNode_HitTestProjectedBoundsAndChildrenCf (int pointerY,int pointerX,ModelRuntimeNode *modelNode, FrontendModelPointerContextRuntimeState118 *context);

/* 0x0050B1D0 */
ModelRaycastNearestHitCfRegisterResult ModelNodeRuntime_RaycastHierarchyNearestCf(ModelRuntimeNode *modelNodeRuntime);

/* 0x0051B650 */
undefined8 ModelNodeRuntime_InstantiateLinkedChildrenRecursiveCf (FactionRuntimeIndex factionIndex,GraphicsPaletteAsset *paletteAsset, GraphicsTextureSet *textureSet,ModelRuntimeSlot *modelRuntimeSlot, ModelDefinitionHierarchyNodeAddress32 definitionNode,WorldRuntimeContext *worldRuntime);

/* 0x0051BEC0 */
void ModelRuntimeHierarchy_SetCommandTargetRecursive (ModelCommandTarget0 commandTarget0,ModelCommandTarget1 commandTarget1, ModelRuntimeNode *modelNode);

/* 0x0051BF30 */
undefined8 ModelRuntimeHierarchy_ClearMatchingTargetRecursive(RuntimeToken targetRuntimeId,int *modelRuntime);

/* 0x0051C100 */
void ModelRuntimeHierarchy_ApplyFlags418UnlessBit8Recursive(undefined4 contextArg,int *modelRuntime);

/* 0x0051C1F0 */
int ModelRuntimeHierarchy_SumMetric3C(int *modelRuntimeRoot);

/* 0x00528C20 */
ModelRuntimeAttachmentCollectionRegisterPair ModelRuntimeHierarchy_CollectAttachmentDescriptorsRecursive (ModelRuntimeSlot *modelRuntime,MdlSerializedNodeHeader38 *definitionNode);

/* 0x00528E90 */
longlong __fastcall ModelNodeRuntime_CreateHierarchyRecursiveCf (undefined4 param_1,uint param_2,GraphicsPaletteAsset *paletteAsset, GraphicsTextureSet *textureSet,ModelRuntimeSlot *modelRuntime, MdlSerializedNodeHeader38 *definitionNode,WorldRuntimeContext *worldRuntime);

/* 0x005294E0 */
undefined8 ModelRuntimeNode_ReleaseRecursiveAndDetachParent(ModelRuntimeNode *node);

/* 0x0052A100 */
undefined8 ModelRuntimeHierarchy_AccumulateDerivedSelectionMetrics(int *modelRuntime);

/* 0x0052A690 */
ModelRuntimeScaleRatioRegisterPairQ12 ModelRuntimeHierarchy_ComputeScaleRatioQ12Regs(ModelRuntimeSlot *modelRuntime);

/* 0x0052A6F0 */
ModelRuntimeActiveTotalMetricRegisterPair ModelRuntimeHierarchy_ComputeActiveAndTotalMetricsRegs(ModelRuntimeSlot *modelRuntime);

/* 0x0052AAC0 */
void ModelNodeRuntime_SmoothYawTowardTarget (ModelRuntimeNode *modelNodeRuntime,ArmyRuntimeSlot *smoothingState, AngleTurn32 targetYawAngle16);

/* 0x0052AC00 */
void ModelNodeRuntime_SmoothPitchTowardTarget (ModelRuntimeNode *modelNodeRuntime,ArmyRuntimeSlot *smoothingState, AngleTurn32 targetPitchAngle16);

/* 0x004BD1A0 */
void ModelNodeRuntime_ApplyTintRecursive(PackedArgb32 tintArgb,ModelRuntimeNode *modelNode);

/* 0x004BE390 */
void ModelNodeRuntime_ComposeChildTransformsRecursive(ModelRuntimeNode *modelNodeRuntime);

/* 0x0052AEA0 */
undefined8 ModelRuntimeHierarchy_ApplyFactionTechnologyVariantsRecursive (FactionRuntimeIndex factionIndex,int *modelRuntime);

#endif /* THANDOR_WORLD_MODEL_HIERARCHY_H */

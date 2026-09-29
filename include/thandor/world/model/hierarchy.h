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

/* ModelRuntimeSlot.attachments[]: the attachment points one model runtime can record
   (ModelNodeRuntime_CreateHierarchyRecursive drops further ones). */
#define MODEL_RUNTIME_ATTACHMENT_CAPACITY 6
/* ModelRuntimeNode_HitTestProjectedBoundsAndChildren: bit i of the clipped-corner mask stands for bounds corner i
   (x from bit 0, y from bit 1, z from bit 2 of i); a face triangle is tested only when none of its corners is set. */
#define MODEL_BOUNDS_CORNER_BIT(corner) (1 << (corner))
#define MODEL_BOUNDS_TRIANGLE_CORNERS(a,b,c) \
  (MODEL_BOUNDS_CORNER_BIT(a) | MODEL_BOUNDS_CORNER_BIT(b) | MODEL_BOUNDS_CORNER_BIT(c))
/* Functions are grouped by semantic ownership; address comments are executable virtual addresses. */

/* 0x004BD1F0 */
void ModelNodeRuntime_UpdateStateTintRecursive(ModelRuntimeNode *modelNodeRuntime);

/* 0x004BE360 */
void ModelNodeRuntime_RebuildTransformsFromRoot(ModelRuntimeNode *modelNodeRuntime);

/* 0x0051DB80 */
void ModelRuntimeHierarchy_ApplyFactionTechnologyVariants
          (FactionRuntimeIndex factionIndex,ArmyRuntimeSlot *armyRuntime);

/* 0x004BD310 */
void ModelNodeRuntime_AccumulateTransformedBoundsRecursive(ModelRuntimeNode *modelNode);

/* 0x004BD8D0 */
void ModelNodeRuntime_BuildViewFacingRotation(ModelRuntimeNode *modelNodeRuntime);

/* 0x004BD950 */
void ModelNodeRuntime_BuildBillboardRotation(ModelRuntimeNode *modelNodeRuntime);

/* 0x004BE9D0 */
void ModelNodeRuntime_RecomputeSubtreeBoundingRadius(ModelRuntimeNode *modelNodeRuntime);

/* 0x004BEA30 */
void ModelNodeRuntime_UpdateDepthBinMasks(DepthIntervalRadius32 minimumRadius,ModelRuntimeNode *modelNodeRuntime);

/* 0x004BEB80 */
ModelWorldPoint
ModelNodeRuntime_TransformLocalPointRegs
          (ModelPackedPointRecord *localPointRecord,ModelRuntimeNode *modelNodeRuntime);

/* 0x004BEBC0 */
ModelRelativeDirectionAngles ModelNodeRuntime_ComputeRelativeDirectionAngle (ModelRuntimeNode *modelNodeRuntime,AngleTurn32 elevationAngle,AngleTurn32 azimuthAngle);

/* 0x0050A7A0 */
ModelHitTestResult ModelRuntimeNode_HitTestProjectedBoundsAndChildren
          (int pointerY,int pointerX,ModelRuntimeNode *modelNode,
          FrontendModelPointerHitContext *context);

/* 0x0050B1D0 */
ModelRaycastResult ModelNodeRuntime_RaycastHierarchyNearest(ModelRuntimeNode *modelNodeRuntime);

/* 0x0051B650 */
bool ModelNodeRuntime_InstantiateLinkedChildrenRecursive
          (FactionRuntimeIndex factionIndex,GraphicsPaletteAsset *paletteAsset,
          GraphicsTextureSet *textureSet,ModelRuntimeSlot *modelRuntimeSlot,
          ModelDefinitionHierarchyNodeAddress32 definitionNode,WorldRuntimeContext *worldRuntime);

/* 0x0051BEC0 */
void ModelRuntimeHierarchy_SetPaletteAndTextureSetNonNullRecursive
          (GraphicsPaletteAsset *paletteAsset,GraphicsTextureSet *textureSet,
          ModelRuntimeNode *modelNode);

/* 0x0051BF30 */
void ModelRuntimeHierarchy_ClearMatchingTargetRecursive(RuntimeToken targetRuntimeId,int *modelRuntime);

/* 0x0051C100 */
void ModelRuntimeHierarchy_MarkDestroyedRecursive(WorldRuntimeContext *contextArg,ArmyRuntimeSlot *armyRuntime);

/* 0x0051C1F0 */
int ModelRuntimeHierarchy_SumArmour(int *modelRuntimeRoot);

/* 0x00528C20 */
ModelRuntimeSlot * ModelRuntimeHierarchy_CollectAttachmentDescriptorsRecursive
          (ModelRuntimeSlot *modelRuntimeContinuityEdi,ModelRuntimeSlot *modelRuntime,
          MdlSerializedNodeHeader *definitionNode);

/* 0x00528E90 */
ModelNodeCreateResult ModelNodeRuntime_CreateHierarchyRecursive
          (GraphicsPaletteAsset *paletteAsset,GraphicsTextureSet *textureSet,
          ModelRuntimeSlot *modelRuntime,MdlSerializedNodeHeader *definitionNode,
          WorldRuntimeContext *worldRuntime);

/* 0x005294E0 */
void ModelRuntimeNode_ReleaseRecursiveAndDetachParent(ModelRuntimeNode *node);

/* 0x0052A100 */
void ModelRuntimeHierarchy_AccumulateDerivedSelectionMetrics(int *modelRuntime);

/* 0x0052A690 */
ModelRuntimeScaleRatioRegisterPairQ12 ModelRuntimeHierarchy_ComputeScaleRatioQ12Regs(ModelRuntimeSlot *modelRuntime);

/* 0x0052A6F0 */
ModelRuntimeActiveTotalMetricRegisterPair ModelRuntimeHierarchy_ComputeActiveAndTotalMetricsRegs(ModelRuntimeSlot *modelRuntime);

/* 0x0052AAC0 */
AimSmoothResult ModelNodeRuntime_SmoothYawTowardTarget (ModelRuntimeNode *modelNodeRuntime,ModelRuntimeWeaponAimStateView *smoothingState, AngleTurn32 targetYawAngle16);

/* 0x0052AC00 */
AimSmoothResult ModelNodeRuntime_SmoothPitchTowardTarget (ModelRuntimeNode *modelNodeRuntime,ModelRuntimeWeaponAimStateView *smoothingState, AngleTurn32 targetPitchAngle16);

/* 0x004BD1A0 */
void ModelNodeRuntime_ApplyTintRecursive(PackedArgb32 tintArgb,ModelRuntimeNode *modelNode);

/* 0x004BE390 */
void ModelNodeRuntime_ComposeChildTransformsRecursive(ModelRuntimeNode *modelNodeRuntime);

/* 0x0052AEA0 */
void ModelRuntimeHierarchy_ApplyFactionTechnologyVariantsRecursive(FactionRuntimeIndex factionIndex,int *modelRuntime);


/* 0x0051D870 */
void ModelRuntimeHierarchy_SetPaletteAndTextureSetRecursive
          (GraphicsPaletteAsset *paletteAsset,GraphicsTextureSet *textureSet,ModelRuntimeNode *node);

#endif /* THANDOR_WORLD_MODEL_HIERARCHY_H */

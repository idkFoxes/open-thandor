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
/* Functions are grouped by semantic ownership. */

void ModelNodeRuntime_UpdateStateTintRecursive(ModelRuntimeNode *modelNodeRuntime);

void ModelNodeRuntime_RebuildTransformsFromRoot(ModelRuntimeNode *modelNodeRuntime);

void ModelRuntimeHierarchy_ApplyFactionTechnologyVariants
          (FactionRuntimeIndex factionIndex,ArmyRuntimeSlot *armyRuntime);

void ModelNodeRuntime_AccumulateTransformedBoundsRecursive(ModelRuntimeNode *modelNode);

void ModelNodeRuntime_BuildViewFacingRotation(ModelRuntimeNode *modelNodeRuntime);

void ModelNodeRuntime_BuildBillboardRotation(ModelRuntimeNode *modelNodeRuntime);

void ModelNodeRuntime_RecomputeSubtreeBoundingRadius(ModelRuntimeNode *modelNodeRuntime);

void ModelNodeRuntime_UpdateDepthBinMasks(DepthIntervalRadius32 minimumRadius,ModelRuntimeNode *modelNodeRuntime);

ModelWorldPoint
ModelNodeRuntime_TransformLocalPoint
          (ModelPackedPointRecord *localPointRecord,ModelRuntimeNode *modelNodeRuntime);

ModelRelativeDirectionAngles ModelNodeRuntime_ComputeRelativeDirectionAngle (ModelRuntimeNode *modelNodeRuntime,AngleTurn32 elevationAngle,AngleTurn32 azimuthAngle);

bool ModelRuntimeNode_HitTestProjectedBoundsAndChildren
          (int pointerY,int pointerX,ModelRuntimeNode *modelNode,
          FrontendModelPointerHitContext *context,uint32_t *outDistanceQ12);

Q12 ModelNodeRuntime_RaycastHierarchyNearest
          (ModelRuntimeNode *modelNodeRuntime,ModelRuntimeNode **outNearestModelNode);

bool ModelNodeRuntime_InstantiateLinkedChildrenRecursive
          (FactionRuntimeIndex factionIndex,GraphicsPaletteAsset *paletteAsset,
          GraphicsTextureSet *textureSet,ModelRuntimeSlot *modelRuntimeSlot,
          ModelDefinitionHierarchyNodeAddress32 definitionNode,WorldRuntimeContext *worldRuntime);

void ModelRuntimeHierarchy_SetPaletteAndTextureSetNonNullRecursive
          (GraphicsPaletteAsset *paletteAsset,GraphicsTextureSet *textureSet,
          ModelRuntimeNode *modelNode);

void ModelRuntimeHierarchy_ClearMatchingTargetRecursive(RuntimeToken targetRuntimeId,int *modelRuntime);

void ModelRuntimeHierarchy_MarkDestroyedRecursive(WorldRuntimeContext *contextArg,ArmyRuntimeSlot *armyRuntime);

int ModelRuntimeHierarchy_SumArmour(int *modelRuntimeRoot);

bool ModelRuntimeHierarchy_CollectAttachmentDescriptorsRecursive
          (ModelRuntimeSlot *modelRuntime,MdlSerializedNodeHeader *definitionNode);

bool ModelNodeRuntime_CreateHierarchyRecursive
          (GraphicsPaletteAsset *paletteAsset,GraphicsTextureSet *textureSet,
          ModelRuntimeSlot *modelRuntime,MdlSerializedNodeHeader *definitionNode,
          WorldRuntimeContext *worldRuntime,ModelRuntimeNode **outNode);

void ModelRuntimeNode_ReleaseRecursiveAndDetachParent(ModelRuntimeNode *node);

void ModelRuntimeHierarchy_AccumulateDerivedSelectionMetrics(int *modelRuntime);

Q12 ModelRuntimeHierarchy_ComputeConditionRatioQ12(ModelRuntimeSlot *modelRuntime);

ModelHierarchyEnergyDemand ModelRuntimeHierarchy_ComputeEnergyDemand(ModelRuntimeSlot *modelRuntime);

bool ModelNodeRuntime_SmoothYawTowardTarget (ModelRuntimeNode *modelNodeRuntime,ModelRuntimeWeaponAimStateView *smoothingState, AngleTurn32 targetYawAngle16);

uint32_t ModelNodeRuntime_SmoothPitchTowardTarget (ModelRuntimeNode *modelNodeRuntime,ModelRuntimeWeaponAimStateView *smoothingState, AngleTurn32 targetPitchAngle16);

void ModelNodeRuntime_ApplyTintRecursive(PackedArgb32 tintArgb,ModelRuntimeNode *modelNode);

void ModelNodeRuntime_ComposeChildTransformsRecursive(ModelRuntimeNode *modelNodeRuntime);

void ModelRuntimeHierarchy_ApplyFactionTechnologyVariantsRecursive(FactionRuntimeIndex factionIndex,int *modelRuntime);


void ModelRuntimeHierarchy_SetPaletteAndTextureSetRecursive
          (GraphicsPaletteAsset *paletteAsset,GraphicsTextureSet *textureSet,ModelRuntimeNode *node);

#endif /* THANDOR_WORLD_MODEL_HIERARCHY_H */

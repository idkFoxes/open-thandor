/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/include/thandor/world/model/hierarchy.h
 * Reverse engineering by idkFoxes 2026
 */

#ifndef THANDOR_WORLD_MODEL_HIERARCHY_H
#define THANDOR_WORLD_MODEL_HIERARCHY_H

#include <thandor/core/types.h>
#include <thandor/gameplay/army/types.h>
#include <thandor/graphics/render/types.h>
#include <thandor/graphics/resources/types.h>
#include <thandor/ui/ingame/types.h>
#include <thandor/world/model/types.h>
#include <thandor/core/contracts.h>

/* ModelRuntimeSlot.attachments[]: the attachment points one model runtime can record
   (ModelNodeRuntime_CreateHierarchyRecursive drops further ones). */
inline constexpr int MODEL_RUNTIME_ATTACHMENT_CAPACITY = 6;
/* ModelRuntimeNode_HitTestProjectedBoundsAndChildren: bit i of the clipped-corner mask stands for bounds corner i
   (x from bit 0, y from bit 1, z from bit 2 of i); a face triangle is tested only when none of its corners is set. */
#define MODEL_BOUNDS_CORNER_BIT(corner) (1 << (corner))
#define MODEL_BOUNDS_TRIANGLE_CORNERS(a,b,c) \
  (MODEL_BOUNDS_CORNER_BIT(a) | MODEL_BOUNDS_CORNER_BIT(b) | MODEL_BOUNDS_CORNER_BIT(c))

void ModelNodeRuntime_UpdateStateTintRecursive(ModelRuntimeNode *modelNodeRuntime);

void ModelNodeRuntime_RebuildTransformsFromRoot(ModelRuntimeNode *modelNodeRuntime);

void ModelNodeRuntime_AccumulateTransformedBoundsRecursive(ModelRuntimeNode *modelNode);

void ModelNodeRuntime_BuildViewFacingRotation(ModelRuntimeNode *modelNodeRuntime);

void ModelNodeRuntime_BuildBillboardRotation(ModelRuntimeNode *modelNodeRuntime);

void ModelNodeRuntime_RecomputeSubtreeBoundingRadius(ModelRuntimeNode *modelNodeRuntime);

void ModelNodeRuntime_UpdateDepthBinMasks(DepthIntervalRadius32 minimumRadius,ModelRuntimeNode *modelNodeRuntime);

ModelWorldPoint
ModelNodeRuntime_TransformLocalPoint
          (ModelPackedPointRecord *localPointRecord,ModelRuntimeNode *modelNodeRuntime);

ModelRelativeDirectionAngles ModelNodeRuntime_ComputeRelativeDirectionAngle (ModelRuntimeNode *modelNodeRuntime,AngleTurn32 elevationAngle,AngleTurn32 azimuthAngle);

Bool8 ModelNodeRuntime_InstantiateLinkedChildrenRecursive
          (FactionRuntimeIndex factionIndex,GraphicsPaletteAsset *paletteAsset,
          GraphicsTextureSet *textureSet,ModelRuntimeSlot *modelRuntimeSlot,
          ModelDefinitionHierarchyNodeAddress32 definitionNode,WorldRuntimeContext *worldRuntime);

void ModelRuntimeHierarchy_SetPaletteAndTextureSetNonNullRecursive
          (GraphicsPaletteAsset *paletteAsset,GraphicsTextureSet *textureSet,
          ModelRuntimeNode *modelNode);

void ModelRuntimeHierarchy_ClearMatchingTargetRecursive(const void *targetRuntimeId,ModelRuntimeSlot *modelRuntime);

Bool8 ModelRuntimeHierarchy_CollectAttachmentDescriptorsRecursive
          (ModelRuntimeSlot *modelRuntime,MdlSerializedNodeHeader *definitionNode);

Bool8 ModelNodeRuntime_CreateHierarchyRecursive
          (GraphicsPaletteAsset *paletteAsset,GraphicsTextureSet *textureSet,
          ModelRuntimeSlot *modelRuntime,MdlSerializedNodeHeader *definitionNode,
          WorldRuntimeContext *worldRuntime,ModelRuntimeNode **outNode);

void ModelRuntimeNode_ReleaseRecursiveAndDetachParent(ModelRuntimeNode *node);

void ModelNodeRuntime_ApplyTintRecursive(PackedArgb32 tintArgb,ModelRuntimeNode *modelNode);

void ModelNodeRuntime_ComposeChildTransformsRecursive(ModelRuntimeNode *modelNodeRuntime);


void ModelRuntimeHierarchy_SetPaletteAndTextureSetRecursive
          (GraphicsPaletteAsset *paletteAsset,GraphicsTextureSet *textureSet,ModelRuntimeNode *node);

extern GraphicsFixedMatrix3x4 g_ModelTransformScratchMatrix;

extern int32_t g_ModelBoundsMinimumX;
extern int32_t g_ModelBoundsMaximumX;
extern int32_t g_ModelBoundsMinimumY;
extern int32_t g_ModelBoundsMaximumY;
extern int32_t g_ModelBoundsMinimumZ;
extern int32_t g_ModelBoundsMaximumZ;

#endif /* THANDOR_WORLD_MODEL_HIERARCHY_H */

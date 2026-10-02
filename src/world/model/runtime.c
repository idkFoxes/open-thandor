/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/src/world/model/runtime.c
 * Reverse engineering by idkFoxes 2026
 */

#include <thandor/world/model/runtime.h>
#include <thandor/thandor.h>
#include <thandor/platform/bootstrap/image.h>

/* Diagnostics: set by the offscreen preview renderer while it submits models. */

/* Implementation ownership: world/model/runtime. */

/* Address: 0x00529360.
   Attaches a new model to one of the attachment points of modelRuntime (recorded by
   ModelNodeRuntime_CreateHierarchyRecursive): creates the model childDefinitionId for the same army, stores it in
   the attachment entry, hangs its root node into the parent node's child slot and gives it the saved local
   rotation and the attachment translation. Returns true and stores the child's model runtime in
   *outChildModelRuntime, or false (leaving it unchanged) when the model could not be created.
*/
bool ModelRuntimePool_RepairDeferredChild
          (GraphicsPaletteAsset *paletteAsset,GraphicsTextureSet *textureSet,
          ModelRuntimeAttachmentIndex attachmentIndex,PckModelDefinitionIdCatalog childDefinitionId,
          ModelRuntimeSlot *modelRuntime,WorldRuntimeContext *worldRuntime,
          ModelRuntimeSlot **outChildModelRuntime)

{
  ModelAttachmentTransformRecord *sourceTransform;
  AngleTurn32 rotationAngle0;
  AngleTurn32 rotationAngle1;
  AngleTurn32 rotationAngle2;
  Q12 translationX;
  Q12 translationY;
  ModelRuntimeSlot *childModelRuntime;
  ModelRuntimeNode *parentModelNode;
  ModelRuntimeNode *childRootNode;

  if (attachmentIndex < modelRuntime->attachmentCount) {
    if (ModelRuntimePool_CreateInstanceByDefinitionId
                      (paletteAsset,textureSet,
                       modelRuntime->ownerArmyRuntimeOrSavedOffset.armyRuntime,childDefinitionId,
                       worldRuntime,&childModelRuntime) != 0) {
      return false;
    }
    modelRuntime->attachments[attachmentIndex].childModelRuntimeOrSavedOffset = childModelRuntime;
    parentModelNode = modelRuntime->attachments[attachmentIndex].parentModelNodeOrSavedOffset;
    sourceTransform = modelRuntime->attachments[attachmentIndex].sourceTransform;
    childRootNode = (childModelRuntime->rootModelNodeOrSavedOffset).modelNode;
    rotationAngle0 = modelRuntime->attachments[attachmentIndex].childLocalRotationAngle0;
    rotationAngle1 = modelRuntime->attachments[attachmentIndex].childLocalRotationAngle1;
    rotationAngle2 = modelRuntime->attachments[attachmentIndex].childLocalRotationAngle2;
    parentModelNode->childNodes[modelRuntime->attachments[attachmentIndex].childNodeIndex] =
         childRootNode;
    childRootNode->modelPayload.localRotationAngle2 = rotationAngle2;
    childRootNode->modelPayload.localRotationAngle1 = rotationAngle1;
    childRootNode->modelPayload.localRotationAngle0 = rotationAngle0;
    childRootNode->parentNode = parentModelNode;
    translationX = sourceTransform->localTranslationXQ12;
    translationY = sourceTransform->localTranslationYQ12;
    childRootNode->modelPayload.localTranslationZQ12 =
         sourceTransform->localTranslationZQ12;
    childRootNode->modelPayload.localTranslationYQ12 = translationY;
    childRootNode->modelPayload.localTranslationXQ12 = translationX;
    *outChildModelRuntime = childModelRuntime;
    return true;
  }
  /* Original quirk: index past the attachment count still succeeds with EAX untouched, and in its only caller
     (ModelNodeRuntime_InstantiateLinkedChildrenRecursive) EAX holds childDefinitionId at the call. */
  *outChildModelRuntime = (ModelRuntimeSlot *)(uintptr_t)childDefinitionId;
  return true;
}


/* Address: 0x004BDDB0.
   Renders a model node and its children for the main view: clears the node's MODEL_NODE_FLAG_RENDERED, culls it
   against the four side planes of the view frustum and the near plane, and draws it when it lies fully in front
   of the near plane, picking the level of detail by depth. A node outside a plane by more than its subtree radius
   ends the walk; otherwise the children are visited even when the node itself was culled. Called for every
   model by the offscreen preview renderer (src/graphics/render/projection.c) and by the frontend/in-game world
   view (src/ui/frontend/runtime.c).
*/
void ModelRuntime_CullAndRenderHierarchyRecursive(ModelRuntimeNode *modelNodeRuntime)

{
  Q12 *boundingRadiusField;
  ModelResource *renderView;
  uint32_t radiusOrMeshGroupCount;
  int32_t planeDistance;
  uint32_t distanceOrChildrenRemaining;
  int subtreeRadiusOrChildIndex;
  ModelMeshGroupRelativeOffset *meshGroup;
  int nodeRadius;
  Q12 projectedRadiusScale;

  if (modelNodeRuntime != NULL) {
    modelNodeRuntime->runtimeFlags = modelNodeRuntime->runtimeFlags & ~MODEL_NODE_FLAG_RENDERED;
    g_ModelCullViewRelativeX =
         modelNodeRuntime->worldTransform.translation.x - g_ViewOriginFixed.x;
    g_ModelCullViewRelativeY =
         modelNodeRuntime->worldTransform.translation.y - g_ViewOriginFixed.y;
    g_ModelCullViewRelativeZ =
         modelNodeRuntime->worldTransform.translation.z - g_ViewOriginFixed.z;
    renderView = modelNodeRuntime->modelPayload.modelResource;
    subtreeRadiusOrChildIndex = modelNodeRuntime->subtreeBoundingRadiusQ12 + modelNodeRuntime->renderDepthBiasOrState;
    nodeRadius = renderView->boundingRadiusQ12 + modelNodeRuntime->renderDepthBiasOrState;
    /* plane distance above the subtree radius: the whole subtree is outside; above the node radius: only the
       node is culled */
    planeDistance = FixedVec3_DotQ28(g_FrustumPlaneNormalFixed_0,
                             (GraphicsFixedVec3 *)&g_ModelCullViewRelativeX);
    if (planeDistance <= subtreeRadiusOrChildIndex) {
      if (planeDistance <= nodeRadius) {
        planeDistance = FixedVec3_DotQ28(g_FrustumPlaneNormalFixed_0 + 1,
                                 (GraphicsFixedVec3 *)&g_ModelCullViewRelativeX);
        if (subtreeRadiusOrChildIndex < planeDistance) {
          return;
        }
        if (planeDistance <= nodeRadius) {
          planeDistance = FixedVec3_DotQ28(g_FrustumPlaneNormalFixed_0 + 2,
                                   (GraphicsFixedVec3 *)&g_ModelCullViewRelativeX);
          if (subtreeRadiusOrChildIndex < planeDistance) {
            return;
          }
          if (planeDistance <= nodeRadius) {
            planeDistance = FixedVec3_DotQ28(g_FrustumPlaneNormalFixed_0 + 3,
                                     (GraphicsFixedVec3 *)&g_ModelCullViewRelativeX);
            if (subtreeRadiusOrChildIndex < planeDistance) {
              return;
            }
            if (planeDistance <= nodeRadius) {
              distanceOrChildrenRemaining = FixedMath_Length3(g_ModelCullViewRelativeZ,g_ModelCullViewRelativeY,
                                        g_ModelCullViewRelativeX);
              radiusOrMeshGroupCount = renderView->boundingRadiusQ12;
              /* radius / distance in Q28, 1.0 when the view origin is inside the bounding sphere */
              if ((int)distanceOrChildrenRemaining < (int)radiusOrMeshGroupCount) {
                projectedRadiusScale = Q28_ONE;
              }
              else {
                /* unsigned DIV */
                projectedRadiusScale =
                     (Q12)(((uint64_t)radiusOrMeshGroupCount << 28) / (uint64_t)distanceOrChildrenRemaining);
              }
              FixedTransform_ApplyPoint
                        ((GraphicsFixedVec3 *)&g_ModelCullViewRelativeX,
                         &modelNodeRuntime->worldTransform.translation,
                         &g_ViewProjectionMatrixFixed);
              renderView = modelNodeRuntime->modelPayload.modelResource;
              /* g_ModelCullViewRelativeZ is now the view depth; g_ProjectionScaleFixed is the near plane */
              if ((int)g_ModelCullViewRelativeZ <= (int)g_ProjectionScaleFixed) {
                return;
              }
              boundingRadiusField = &renderView->boundingRadiusQ12;
              if (g_ModelCullViewRelativeZ - g_ProjectionScaleFixed != *boundingRadiusField &&
                  *boundingRadiusField <= (int)(g_ModelCullViewRelativeZ - g_ProjectionScaleFixed)) {
                modelNodeRuntime->runtimeFlags = modelNodeRuntime->runtimeFlags | MODEL_NODE_FLAG_RENDERED;
                g_GraphicsShadingNearbyRecordCount = 0;
                GraphicsShadingRuntime_CollectNearbyRecords
                          (renderView->boundingRadiusQ12,g_ModelCullViewRelativeZ,
                           g_ModelCullViewRelativeY,g_ModelCullViewRelativeX);
                renderView = modelNodeRuntime->modelPayload.modelResource;
                radiusOrMeshGroupCount = renderView->meshGroupCount;
                meshGroup = &renderView->firstMeshGroupRelativeOffset;
                /* level of detail: the next mesh group beyond g_ModelLodDepthThresholdQ8, the third beyond twice
                   that depth (each group starts with the offset to the next) */
                if ((uint32_t)g_ModelLodDepthThresholdQ8 < (int)g_ModelCullViewRelativeZ &&
                    1 < radiusOrMeshGroupCount &&
                    (meshGroup = (ModelMeshGroupRelativeOffset *)((int)meshGroup + *meshGroup),
                     (uint32_t)g_ModelLodDepthThresholdQ8 < (uint32_t)((int)g_ModelCullViewRelativeZ >> 1)) &&
                    2 < radiusOrMeshGroupCount) {
                  meshGroup = (ModelMeshGroupRelativeOffset *)((int)meshGroup + *meshGroup);
                }
                ModelRender_DrawMeshGroupsWithTemporaryTransform
                          (projectedRadiusScale,(ModelMeshGroupAddress32)meshGroup,modelNodeRuntime);
              }
            }
          }
        }
      }
      distanceOrChildrenRemaining = modelNodeRuntime->childCount;
      if (distanceOrChildrenRemaining != 0) {
        subtreeRadiusOrChildIndex = 0;
        do {
          if (modelNodeRuntime->childNodes[subtreeRadiusOrChildIndex] != NULL) {
            ModelRuntime_CullAndRenderHierarchyRecursive(modelNodeRuntime->childNodes[subtreeRadiusOrChildIndex]);
          }
          subtreeRadiusOrChildIndex++;
          distanceOrChildrenRemaining--;
        } while (distanceOrChildrenRemaining != 0);
      }
    }
  }
}


/* Address: 0x004BE270.
   Alternate model renderer of the frontend/in-game world view (src/ui/frontend/runtime.c, chosen when the
   pointer context compares hits by metric only): draws a node and all its children without culling. The centre
   of the node's local bounds is transformed into g_ModelCullViewRelativeX/Y/Z to collect the nearby shading
   records; every drawn node gets MODEL_NODE_FLAG_RENDERED.
*/
void ModelRuntime_RenderHierarchyRecursiveAlternatePath(ModelRuntimeNode *modelNode)

{
  ModelResource *modelResourceView;
  uint32_t boundsLengthOrChildrenRemaining;
  int childIndex;

  modelResourceView = modelNode->modelPayload.modelResource; /* read before the NULL test, as in the original */
  if (modelNode != NULL) {
    modelNode->runtimeFlags = modelNode->runtimeFlags | MODEL_NODE_FLAG_RENDERED;
    /* g_GraphicsDirectionWorld only serves as scratch vector here */
    g_GraphicsDirectionWorld.x = (modelResourceView->localBoundsX0Q12 + modelResourceView->localBoundsX1Q12) >> 1;
    g_GraphicsDirectionWorld.y = (modelResourceView->localBoundsY0Q12 + modelResourceView->localBoundsY1Q12) >> 1;
    g_GraphicsDirectionWorld.z = (modelResourceView->localBoundsZ0Q12 + modelResourceView->localBoundsZ1Q12) >> 1;
    FixedTransform_ApplyPoint
              ((GraphicsFixedVec3 *)&g_ModelCullViewRelativeX,&g_GraphicsDirectionWorld,
               &g_ViewProjectionMatrixFixed);
    boundsLengthOrChildrenRemaining =
         FixedMath_Length3(modelResourceView->localBoundsZ1Q12 - modelResourceView->localBoundsZ0Q12,
                           modelResourceView->localBoundsY1Q12 - modelResourceView->localBoundsY0Q12,
                           modelResourceView->localBoundsX1Q12 - modelResourceView->localBoundsX0Q12);
    GraphicsShadingRuntime_CollectNearbyRecords
              ((int)boundsLengthOrChildrenRemaining >> 1,g_ModelCullViewRelativeZ,g_ModelCullViewRelativeY,
               g_ModelCullViewRelativeX);
    ModelRender_DrawMeshGroupsAlternatePath(modelNode->runtimeStateA0,modelNode);
    childIndex = 0;
    for (boundsLengthOrChildrenRemaining = modelNode->childCount; boundsLengthOrChildrenRemaining != 0;
        boundsLengthOrChildrenRemaining--) {
      if (modelNode->childNodes[childIndex] != NULL) {
        ModelRuntime_RenderHierarchyRecursiveAlternatePath(modelNode->childNodes[childIndex]);
      }
      childIndex++;
    }
  }
}


/* Address: 0x0050B440.
   Casts a ray from the origin in the direction (elevationAngle, azimuthAngle), at most maximumDistanceQ12 long,
   against the models in worldRuntime's owner list whose owner class is requiredOwnerId, skipping excludedNode and
   ray-transparent models (MODEL_NODE_FLAG_RAY_TRANSPARENT) and pre-filtering by the depth bin masks of the X and Y
   ranges the ray can reach. Returns true when a model was hit. *outNearestDistanceQ12 always receives the
   nearest distance (MODEL_RAYCAST_NO_HIT_DISTANCE on a miss) and *outNearestModelNode the nearest hit node.
   Original quirk: on a miss *outNearestModelNode is NULL or the scratch EDX value of the last missing hierarchy
   test; callers only use it after a hit. Used by the army combat code (src/gameplay/army/combat.c) and the shot
   updates (src/world/shots/maintenance.c).
*/
bool ModelRuntime_RaycastCandidateListNearest
          (AngleTurn32 elevationAngle,AngleTurn32 azimuthAngle,Q12 maximumDistanceQ12,Q12 originZQ12
          ,Q12 originYQ12,Q12 originXQ12,WorldOwnerRuntimeClassId requiredOwnerId,
          ModelRuntimeNode *excludedNode,WorldRuntimeContext *worldRuntime,Q12 *outNearestDistanceQ12,
          ModelRuntimeNode **outNearestModelNode)

{
  ModelRuntimeNode *modelNodeRuntime;
  DepthBinMask32 rayXBinMask;
  DepthBinMask32 rayYBinMask;
  int bestDistanceQ12;
  ModelRuntimeNode *nearestModelNode;
  bool masksOverlap;
  Q12 hierarchyDistanceQ12;
  ModelRuntimeNode *hierarchyNearestNode;

  g_ModelRaycastOriginX = originXQ12;
  g_ModelRaycastOriginY = originYQ12;
  g_ModelRaycastOriginZ = originZQ12;
  g_ModelRaycastMaximumDistance = maximumDistanceQ12;
  rayXBinMask = DepthInterval_BuildBinMask(maximumDistanceQ12,originXQ12);
  rayYBinMask = DepthInterval_BuildBinMask(maximumDistanceQ12,originYQ12);
  FixedMath_WriteDirectionQ28
            ((GraphicsFixedVec3 *)&g_ModelRaycastWorldDirectionXQ28,elevationAngle,azimuthAngle);
  nearestModelNode = NULL;
  bestDistanceQ12 = MODEL_RAYCAST_NO_HIT_DISTANCE;
  for (modelNodeRuntime = (ModelRuntimeNode *)worldRuntime->ownerListHead;
      modelNodeRuntime != NULL;
      modelNodeRuntime = (ModelRuntimeNode *)(modelNodeRuntime->common).nextNode) {
    if (modelNodeRuntime != excludedNode && modelNodeRuntime->ownerClassId == requiredOwnerId &&
        (modelNodeRuntime->runtimeFlags & MODEL_NODE_FLAG_RAY_TRANSPARENT) == 0 &&
        (masksOverlap = DepthBinMasks_Overlap
                           (modelNodeRuntime->depthBinMaskFar,modelNodeRuntime->depthBinMaskNear,
                            rayYBinMask,rayXBinMask), masksOverlap)) {
      hierarchyDistanceQ12 = ModelNodeRuntime_RaycastHierarchyNearest(modelNodeRuntime,&hierarchyNearestNode);
      if (hierarchyDistanceQ12 <= bestDistanceQ12) {
        bestDistanceQ12 = hierarchyDistanceQ12;
        nearestModelNode = hierarchyNearestNode;
      }
    }
  }
  *outNearestModelNode = nearestModelNode;
  *outNearestDistanceQ12 = bestDistanceQ12;
  return bestDistanceQ12 != MODEL_RAYCAST_NO_HIT_DISTANCE;
}


/* Address: 0x0051C240.
   Returns the condition ratio (Q12) of an army's model hierarchy: the EAX half of
   ModelRuntimeHierarchy_ComputeScaleRatioQ12Regs; EDX is preserved. No caller in the C code (function map only).
*/
Q12 ModelRuntime_QueryHierarchyScaleRatioQ12(RuntimeModelFactionPrefix *runtimeEntry)

{
  ModelRuntimeScaleRatioRegisterPairQ12 scaleRatioPairQ12;

  scaleRatioPairQ12 = ModelRuntimeHierarchy_ComputeScaleRatioQ12Regs(runtimeEntry->modelRuntime);
  return (Q12)scaleRatioPairQ12;
}


/* Address: 0x0051C260.
   Returns ModelRuntimeHierarchy_ComputeScaleRatioQ12Regs (EDX:EAX) for the model runtime hierarchy of a
   runtime entry (an army).
*/
ModelRuntimeScaleRatioRegisterPairQ12
ModelRuntime_QueryHierarchyScaleRatioQ12Regs(RuntimeModelFactionPrefix *runtimeEntry)

{
  return ModelRuntimeHierarchy_ComputeScaleRatioQ12Regs(runtimeEntry->modelRuntime);
}


/* Address: 0x0051C280.
   Returns the active metric of an army's model hierarchy (the EAX half of
   ModelRuntimeHierarchy_ComputeActiveAndTotalMetricsRegs); the in-game selection detail shows it divided by 16
   as the energy value. EDX is preserved.
*/
int ModelRuntime_QueryActiveHierarchyMetric(ArmyRuntimeSlot *armyRuntime)

{
  ModelRuntimeActiveTotalMetricRegisterPair activeHierarchyMetricPair;

  activeHierarchyMetricPair =
       ModelRuntimeHierarchy_ComputeActiveAndTotalMetricsRegs
                 (armyRuntime->modelRuntimeOrSavedOffset.modelRuntime);
  return (int)activeHierarchyMetricPair;
}


/* Address: 0x0051C2A0.
   Returns the energy demand of an army's model hierarchy as ModelRuntimeHierarchy_ComputeActiveAndTotalMetricsRegs
   does: EAX the active part, EDX the total. The selection panel (src/gameplay/selection/runtime.c) draws it as a
   stepped meter.
*/
ModelRuntimeActiveTotalMetricRegisterPair
ModelRuntime_QueryActiveAndTotalHierarchyMetricsRegs(RuntimeModelFactionPrefix *runtimeEntry)

{
  ModelRuntimeActiveTotalMetricRegisterPair activeTotalMetrics;

  activeTotalMetrics =
       ModelRuntimeHierarchy_ComputeActiveAndTotalMetricsRegs(runtimeEntry->modelRuntime);
  return activeTotalMetrics;
}


/* Address: 0x00528A40.
   Allocates and zeroes the model runtime pool (MODEL_RUNTIME_SLOT_COUNT 0x200-byte slots, 4 MiB) and records
   its rebase delta (pool base - 1) for savegames. Returns 0, or the allocation error
   (FATAL_ERROR_ARENA_EXHAUSTED / ARENA_HEAP_CORRUPT, never 0 from the arena).
*/
uint32_t __cdecl ModelRuntimePool_Init(void)

{
  ModelRuntimeSlot *modelRuntimeStorageCursor;
  int allocationDwordsRemaining;
  uint32_t allocError;

  allocError = g_MemoryApi.alloc(MODEL_RUNTIME_POOL_BYTES,(void **)&modelRuntimeStorageCursor);
  if (allocError != 0) {
    return allocError;
  }
  /* pool base - 1 */
  g_ModelRuntimeRebaseDelta = (int)modelRuntimeStorageCursor - 1;
  g_ModelRuntimeSlots = modelRuntimeStorageCursor;
  /* zero the pool dword by dword */
  for (allocationDwordsRemaining = MODEL_RUNTIME_POOL_BYTES / 4; allocationDwordsRemaining != 0;
       allocationDwordsRemaining--) {
    modelRuntimeStorageCursor->definitionOrSavedId.definition = NULL;
    modelRuntimeStorageCursor = (ModelRuntimeSlot *)((uint32_t *)modelRuntimeStorageCursor + 1);
  }
  return 0;
}


/* Part of ModelRuntimePool_ShutdownAndReleaseDefinitions (0x00528A70): releases the resource of one serialized
   MDL definition node and then, depth first in index order, of all its children (child count at +0x14, child
   pointers from +0x18). Only plain nodes (nodeFlags +4, low nibble 0) whose flag at +0x34 is set own a
   resource (the sprite asset at +0x30). The original walks the tree with an explicit {count, index, node}
   frame stack on the machine stack; the decompile only followed the first child. */
static void ModelRuntimePool_ReleaseDefinitionNodeResources(MdlSerializedNodeHeader *node)

{
  uint32_t childrenRemaining;
  int childIndex;

  childrenRemaining = node->childCount;
  if ((node->nodeFlags & 0xf) == 0 && node->ownedNestedResourcePresent != 0) {
    Resource_Release(node->spriteAssetReference.spriteAsset);
  }
  for (childIndex = 0; childrenRemaining != 0; childIndex++) {
    /* the child offsets were relocated into pointers when the definition was registered */
    ModelRuntimePool_ReleaseDefinitionNodeResources
              ((MdlSerializedNodeHeader *)(uintptr_t)node->childSerializedOffsets[childIndex]);
    childrenRemaining--;
  }
  return;
}


/* Address: 0x00528A70.
   Counterpart of ModelRuntimePool_Init: frees the model runtime pool, releases the resources of every
   registered model definition's node tree (see ModelRuntimePool_ReleaseDefinitionNodeResources) and clears
   the definition registry.
*/
void ModelRuntimePool_ShutdownAndReleaseDefinitions(void)

{
  int registryRemaining;
  ModelDefinitionRecordPrefix **registryEntry;
  MdlSerializedNodeHeader *rootNode;

  g_MemoryApi.free(g_ModelRuntimeSlots);
  g_ModelRuntimeSlots = NULL;
  registryEntry = g_ModelDefinitionRegistry;
  for (registryRemaining = MODEL_DEFINITION_REGISTRY_SLOT_COUNT; registryRemaining != 0; registryRemaining--) {
    /* the root of the definition's node tree */
    if (*registryEntry != NULL &&
        (rootNode = (MdlSerializedNodeHeader *)((ModelDefinition *)*registryEntry)->rootNodeOffsetOrPointer,
         rootNode != NULL)) {
      ModelRuntimePool_ReleaseDefinitionNodeResources(rootNode);
    }
    *registryEntry = NULL;
    registryEntry++;
  }
}



/* Address: 0x00528B30.
   Before the model runtime pool is written to a savegame (in-game save, src/ui/ingame/runtime.c): turns the
   pointers of every used slot into offsets (owner and linked army against g_ArmyRuntimeRebaseBaseMinusOne, root
   and attachment parent nodes against g_RuntimeObjectRebaseBaseMinusOne, linked model runtime and attachment
   children against g_ModelRuntimeRebaseDelta; NULL stays 0), replaces the definition pointer by its id and runs
   the class's modelUnrebase handler. Unused slots are zeroed. The original returns the pool (EAX) and its size
   0x400000 (EDX) for the save. Counterpart of ModelRuntimePool_RebaseAfterLoad.
*/
void __cdecl ModelRuntimePool_UnrebaseBeforeSave(void)

{
  ModelRuntimeSlotSavedView *linkedModelOffset;
  ModelRuntimePoolRelativeOffset childRuntimeOffset;
  uint32_t offsetClassOrCount;
  int dwordsRemaining;
  int slotsRemaining;
  ModelNodePoolRelativeOffset parentNodeOffset;
  ModelRuntimeSlotUnrebaseView *attachmentCursor;
  ModelRuntimeSlotUnrebaseView *modelRuntime;

  slotsRemaining = MODEL_RUNTIME_SLOT_COUNT;
  modelRuntime = (ModelRuntimeSlotUnrebaseView *)g_ModelRuntimeSlots;
  do {
    while (modelRuntime->rootModelNodeSavedOffset == 0) {
      /* unused slot: zero its 0x80 dwords, one dword step at a time (REP STOSD in the original) */
      for (dwordsRemaining = sizeof(ModelRuntimeSlot) / 4; dwordsRemaining != 0; dwordsRemaining--) {
        modelRuntime->definitionReferenceOrSavedId.definition = NULL;
        modelRuntime = (ModelRuntimeSlotUnrebaseView *)((uint32_t *)modelRuntime + 1);
      }
      slotsRemaining--;
      if (slotsRemaining == 0) {
        return;
      }
    }
    offsetClassOrCount = modelRuntime->ownerArmyRuntimeSavedOffset - (int)g_ArmyRuntimeRebaseBaseMinusOne;
    modelRuntime->rootModelNodeSavedOffset =
         modelRuntime->rootModelNodeSavedOffset - (int)g_RuntimeObjectRebaseBaseMinusOne;
    modelRuntime->ownerArmyRuntimeSavedOffset = offsetClassOrCount;
    linkedModelOffset = (ModelRuntimeSlotSavedView *)modelRuntime->linkedModelRuntimeSavedOffset;
    offsetClassOrCount = modelRuntime->classState.linkedArmyRuntimeSavedOffset;
    if (linkedModelOffset != NULL) {
      linkedModelOffset = (ModelRuntimeSlotSavedView *)((int)linkedModelOffset - g_ModelRuntimeRebaseDelta);
    }
    if (offsetClassOrCount != 0) {
      offsetClassOrCount = offsetClassOrCount - (int)g_ArmyRuntimeRebaseBaseMinusOne;
    }
    modelRuntime->linkedModelRuntimeSavedOffset = (uint32_t)linkedModelOffset;
    modelRuntime->classState.linkedArmyRuntimeSavedOffset = offsetClassOrCount;
    offsetClassOrCount = modelRuntime->definitionReferenceOrSavedId.runtimeDefinition->runtimeClassId;
    modelRuntime->definitionReferenceOrSavedId =
         THANDOR_BITCAST(PckModelDefinitionIdCatalog, ModelDefinitionReferenceOrSavedId,
                         modelRuntime->definitionReferenceOrSavedId.definition->definitionId);
    g_ArmyRuntimeOrderHandlerMatrix11Columns24Classes.modelUnrebase[offsetClassOrCount]
              ((ModelRuntimeSlot *)modelRuntime);
    attachmentCursor = modelRuntime;
    for (offsetClassOrCount = modelRuntime->attachmentCount; offsetClassOrCount != 0; offsetClassOrCount--) {
      childRuntimeOffset = attachmentCursor->attachments[0].childModelRuntimeSavedOffset;
      parentNodeOffset = attachmentCursor->attachments[0].parentModelNodeSavedOffset;
      if (childRuntimeOffset != 0) {
        childRuntimeOffset = childRuntimeOffset - g_ModelRuntimeRebaseDelta;
      }
      if (parentNodeOffset != 0) {
        parentNodeOffset = parentNodeOffset - (int)g_RuntimeObjectRebaseBaseMinusOne;
      }
      attachmentCursor->attachments[0].childModelRuntimeSavedOffset = childRuntimeOffset;
      attachmentCursor->attachments[0].parentModelNodeSavedOffset = parentNodeOffset;
      /* next attachment descriptor: 0x20 bytes on */
      attachmentCursor = (ModelRuntimeSlotUnrebaseView *)
                         ((uint8_t *)attachmentCursor + sizeof(ModelRuntimeAttachmentDescriptor));
    }
    modelRuntime++;
    slotsRemaining--;
  } while (slotsRemaining != 0);
}


/* Address: 0x00528CF0.
   After a savegame load, counterpart of ModelRuntimePool_UnrebaseBeforeSave: turns the saved offsets of every
   used model runtime slot back into pointers, replaces the saved definition id by the registered definition,
   runs the class's load-repair callback and rebuilds the attachment descriptors from the definition. A slot
   whose definition is no longer registered is dropped.
*/
void ModelRuntimePool_RebaseAfterLoad(void)

{
  ModelRuntimeSlot *linkedRuntimeOrCursor;
  ModelRuntimeSlot *rebasedChildRuntime;
  ArmyRuntimeSlot *ownerOrLinkedArmy;
  ArmyRuntimeSlot *rebasedLinkedArmy;
  int registryRemaining;
  uint32_t classIndexOrCount;
  int slotsRemaining;
  ModelRuntimeNode *rebasedParentNode;
  ModelDefinitionRecordPrefix **registryEntry;
  ModelRuntimeSlot *modelRuntime;
  ModelDefinitionRecordPrefix *registeredDefinition;
  ModelRuntimeNode *savedParentNode;
  ModelRuntimeSlot *savedChildRuntime;
  
  slotsRemaining = MODEL_RUNTIME_SLOT_COUNT;
  modelRuntime = g_ModelRuntimeSlots;
  do {
    if (modelRuntime->rootModelNodeOrSavedOffset.modelNode != NULL) {
      /* saved offsets + pool deltas: the owner army (+0x08, always rebased) and the linked army (+0xF0) get
         g_ArmyRuntimeRebaseBaseMinusOne, the root node (+0x04) and attachment parent nodes
         g_RuntimeObjectRebaseBaseMinusOne, the linked model runtime (+0x38) and attachment children
         g_ModelRuntimeRebaseDelta; zero offsets other than the owner stay NULL. */
      ownerOrLinkedArmy = (ArmyRuntimeSlot *)((int)modelRuntime->ownerArmyRuntimeOrSavedOffset.armyRuntime +
                                              (int)g_ArmyRuntimeRebaseBaseMinusOne);
      modelRuntime->rootModelNodeOrSavedOffset.modelNode =
           (ModelRuntimeNode *)
           (g_RuntimeObjectRebaseBaseMinusOne +
           (int)(modelRuntime->rootModelNodeOrSavedOffset).modelNode);
      modelRuntime->ownerArmyRuntimeOrSavedOffset.armyRuntime = ownerOrLinkedArmy;
      ownerOrLinkedArmy = modelRuntime->classState.linkedArmyRuntimeOrSavedOffset.armyRuntime;
      linkedRuntimeOrCursor = NULL;
      if (modelRuntime->linkedModelRuntimeOrSavedOffset.modelRuntime != NULL) {
        linkedRuntimeOrCursor = (ModelRuntimeSlot *)
                     ((uint8_t *)(modelRuntime->linkedModelRuntimeOrSavedOffset).modelRuntime +
                     g_ModelRuntimeRebaseDelta);
      }
      rebasedLinkedArmy = NULL;
      if (ownerOrLinkedArmy != NULL) {
        rebasedLinkedArmy = (ArmyRuntimeSlot *)((int)ownerOrLinkedArmy + (int)g_ArmyRuntimeRebaseBaseMinusOne);
      }
      modelRuntime->linkedModelRuntimeOrSavedOffset.modelRuntime = linkedRuntimeOrCursor;
      modelRuntime->classState.linkedArmyRuntimeOrSavedOffset.armyRuntime = rebasedLinkedArmy;
      registryEntry = g_ModelDefinitionRegistry;
      registryRemaining = MODEL_DEFINITION_REGISTRY_SLOT_COUNT;
      do {
        registeredDefinition = *registryEntry;
        if ((registeredDefinition != NULL) &&
           (modelRuntime->definitionOrSavedId.definition ==
            (ModelDefinitionRecordPrefix *)registeredDefinition->definitionId)) {
          classIndexOrCount = ((ModelDefinition *)registeredDefinition)->runtimeClassId;
          modelRuntime->definitionOrSavedId.definition = registeredDefinition;
          g_ArmyRuntimeOrderHandlerMatrix11Columns24Classes.modelRebaseOrLoadRepair[classIndexOrCount]
                    (modelRuntime);
          linkedRuntimeOrCursor = modelRuntime;
          if (modelRuntime->attachmentCount != 0) {
            for (classIndexOrCount = modelRuntime->attachmentCount; classIndexOrCount != 0; classIndexOrCount--) {
              savedChildRuntime = linkedRuntimeOrCursor->attachments[0].childModelRuntimeOrSavedOffset;
              savedParentNode = linkedRuntimeOrCursor->attachments[0].parentModelNodeOrSavedOffset;
              rebasedChildRuntime = NULL;
              if (savedChildRuntime != NULL) {
                rebasedChildRuntime = (ModelRuntimeSlot *)
                             ((uint8_t *)savedChildRuntime + g_ModelRuntimeRebaseDelta);
              }
              rebasedParentNode = NULL;
              if (savedParentNode != NULL) {
                rebasedParentNode = (ModelRuntimeNode *)
                             (g_RuntimeObjectRebaseBaseMinusOne +
                             (int)savedParentNode);
              }
              linkedRuntimeOrCursor->attachments[0].childModelRuntimeOrSavedOffset = rebasedChildRuntime;
              linkedRuntimeOrCursor->attachments[0].parentModelNodeOrSavedOffset = rebasedParentNode;
              /* next attachment descriptor: 0x20 bytes on */
              linkedRuntimeOrCursor = (ModelRuntimeSlot *)
                                      ((uint8_t *)linkedRuntimeOrCursor + sizeof(ModelRuntimeAttachmentDescriptor));
            }
            modelRuntime->attachmentCount = 0;
            ModelRuntimeHierarchy_CollectAttachmentDescriptorsRecursive
                      (modelRuntime,modelRuntime,
                       (MdlSerializedNodeHeader *)
                       modelRuntime->definitionOrSavedId.runtimeDefinition->rootNodeOffsetOrPointer);
          }
          break;
        }
        registryEntry++;
        registryRemaining--;
        if (registryRemaining == 0) {
          /* definition no longer registered: drop the instance */
          modelRuntime->rootModelNodeOrSavedOffset.modelNode = NULL;
        }
      } while (registryRemaining != 0);
    }
    modelRuntime++;
    slotsRemaining--;
  } while (slotsRemaining != 0);
}


/* Address: 0x00529560.
   Destroys a model runtime: drops player references to it, runs its class release handler, destroys the
   attached model runtimes, clears world nodes that still point to it and releases its node tree.
   An attached part is then removed from its parent's attachment list and the army's derived metrics are
   rebuilt; a root model destroys its army instead, first spawning the army asset its definition names at
   +0x74 at the same place, unless that id is -1 or flag 0x20 is set at +0xEC of the owner record.
*/
void ModelRuntimePool_DestroyHierarchyAndDetach(WorldRuntimeContext *worldRuntime,ModelRuntimeSlot *modelRuntime)

{
  int *ownerRecord;
  ModelDefinitionRecordPrefix *modelDefinition;
  ModelRuntimeSlot *childRuntime;
  ModelRuntimeNode *rootModelNode;
  GameEntityRuntime *entityRuntime;
  Q12 translationX;
  Q12 translationY;
  AngleTurn32 orientationAngle;
  int ownerDefinition;
  uint32_t classIndexOrCount;
  ModelRuntimeSlot *attachmentCursor;
  ModelRuntimeNode *parentModelNode;

  modelDefinition =
       (ModelDefinitionRecordPrefix *)modelRuntime->definitionOrSavedId.savedIdOrOffset;
  classIndexOrCount = ((ModelDefinition *)modelDefinition)->runtimeClassId;
  FrontendPlayerRuntime_ClearAssignmentTokenFromAll((RuntimeToken)modelRuntime);
  g_ArmyRuntimeOrderHandlerMatrix11Columns24Classes.modelReleaseOrCommit[classIndexOrCount]
            (modelDefinition,modelRuntime);
  attachmentCursor = modelRuntime;
  for (classIndexOrCount = modelRuntime->attachmentCount; classIndexOrCount != 0; classIndexOrCount--) {
    childRuntime = attachmentCursor->attachments[0].childModelRuntimeOrSavedOffset;
    if (childRuntime != NULL) {
      ModelRuntimePool_DestroyHierarchyAndDetach(worldRuntime,childRuntime);
    }
    /* steps the cursor by one 0x20-byte attachments[] entry */
    attachmentCursor = (ModelRuntimeSlot *)((uint8_t *)attachmentCursor + sizeof(ModelRuntimeAttachmentDescriptor));
  }
  rootModelNode = modelRuntime->rootModelNodeOrSavedOffset.modelNode;
  entityRuntime = (GameEntityRuntime *)modelRuntime->ownerArmyRuntimeOrSavedOffset.armyRuntime;
  /* position and heading for the replacement army, read before the node tree is released */
  translationX = rootModelNode->worldTransform.translation.x;
  translationY = rootModelNode->worldTransform.translation.y;
  orientationAngle = rootModelNode->modelPayload.worldRotationAngle2;
  parentModelNode = rootModelNode->parentNode;
  WorldRuntime_ForEachOwnerListNode
            (modelRuntime,WorldRuntimeNode_ClearDetachedEntityReferencesCallback,worldRuntime);
  ModelRuntimeNode_ReleaseRecursiveAndDetachParent(rootModelNode);
  modelRuntime->rootModelNodeOrSavedOffset.modelNode = NULL;
  if (parentModelNode == NULL) {
    if (entityRuntime->common.ownership.definitionOrClassRecord != NULL) {
      LOCK(); /* XCHG in the original */
      ownerRecord = entityRuntime->common.ownership.definitionOrClassRecord;
      entityRuntime->common.ownership.definitionOrClassRecord = NULL;
      UNLOCK();
      ownerDefinition = *ownerRecord;
      /* ownerRecord is the owner's root ModelRuntimeSlot */
      if ((((ModelRuntimeSlot *)ownerRecord)->classState.stateFlags & ARMY_MODEL_STATE_DESTRUCTION_STARTED) == 0 &&
         (((ModelDefinition *)ownerDefinition)->destroyedReplacementArmyAssetId != -1)) {
        /* the third parameter of ArmyRuntime_CreateInstanceFromAsset takes y, as at its other callers */
        ArmyRuntime_CreateInstanceFromAsset
                  (0,orientationAngle,translationY,translationX,0,
                   ((ModelDefinition *)ownerDefinition)->destroyedReplacementArmyAssetId,
                   worldRuntime,NULL);
      }
      ArmyRuntime_DestroyInstanceAndRefreshUi(worldRuntime,entityRuntime);
    }
  }
  else {
    attachmentCursor = parentModelNode->runtimePayload.modelRuntime;
    for (classIndexOrCount = attachmentCursor->attachmentCount; classIndexOrCount != 0; classIndexOrCount--) {
      if (attachmentCursor->attachments[0].childModelRuntimeOrSavedOffset == modelRuntime) {
        attachmentCursor->attachments[0].childModelRuntimeOrSavedOffset = NULL;
      }
      attachmentCursor = (ModelRuntimeSlot *)((uint8_t *)attachmentCursor + sizeof(ModelRuntimeAttachmentDescriptor));
    }
    ArmyRuntime_RebuildDerivedSelectionMetrics((ArmyRuntimeSlot *)entityRuntime);
  }
  return;
}


/* Address: 0x00529690.
   Fires a shot from every launch point of a model node: rebuilds the node transforms, then for each point record
   of the node's sprite asset with kind 2 (low nibble of packedLookupKey) creates a projectile from shotDefinition
   at the point's world position, aimed at the target shifted by the point's X/Y offset from the node, so that
   side-by-side launchers fire parallel shots. Called by the army weapon code (src/gameplay/army/movement.c,
   src/gameplay/army/runtime.c).
*/
void ModelRuntime_EmitProjectilesFromAttachmentPoints
          (ShotTargetModelReference targetModelReference,Q12 targetWorldZQ12,Q12 targetWorldYQ12,
          Q12 targetWorldXQ12,ShotDefinition *shotDefinition,ModelRuntimeNode *modelNodeRuntime,
          MdlSerializedNodeHeader *definitionNode,WorldRuntimeContext *worldRuntime)

{
  int modelPointRecordsRemaining;
  ModelPackedPointRecord *localPointRecord;
  ModelWorldPoint launchPointWorld;
  AssetRecordByteCount modelPointTableBase;

  ModelNodeRuntime_RebuildTransformsFromRoot(modelNodeRuntime);
  /* sprite asset: +0xE4 offset of the point records, +0xE8 their count */
  modelPointTableBase = definitionNode->spriteAssetReference.savedId;
  localPointRecord =
       (ModelPackedPointRecord *)
       (modelPointTableBase +
       ((ModelResource *)modelPointTableBase)->packedLookupTableRelativeOffset);
  for (modelPointRecordsRemaining =
           ((ModelResource *)modelPointTableBase)->packedLookupTableEntryCount;
      modelPointRecordsRemaining != 0; modelPointRecordsRemaining--)
  {
    if ((localPointRecord->packedLookupKey & 0xf) == MODEL_POINT_CLASS_SHOT) {
      launchPointWorld = ModelNodeRuntime_TransformLocalPointRegs(localPointRecord,modelNodeRuntime);
      ShotRuntimePool_CreateProjectileFromDefinition
                (targetModelReference,
                 (ArmyRuntimeSlot *)
                 modelNodeRuntime->runtimePayload.armyRuntime->linkedEntityRuntime,
                 targetWorldZQ12,
                 (launchPointWorld.yQ12 - modelNodeRuntime->worldTransform.translation.y) + targetWorldYQ12,
                 (launchPointWorld.xQ12 - modelNodeRuntime->worldTransform.translation.x) + targetWorldXQ12,
                 launchPointWorld.zQ12,launchPointWorld.yQ12,launchPointWorld.xQ12,shotDefinition,worldRuntime);
    }
    localPointRecord++;
  }
}


/* Address: 0x00529140.
   Creates a model runtime for an army from a model definition id: takes the first free pool slot, copies the
   definition's starting values (armour points at +0x3C and the values at +0x40..+0x5C), raises two of the
   army's values to the definition's, builds the model node tree, its bounding radius and transforms, and runs
   the definition class's initialize handler. Returns 0 and stores the slot in *outModelRuntime, or returns
   FATAL_ERROR_GENERAL_FAILURE (no pool or no free slot), the node tree's error, or
   FATAL_ERROR_MODEL_DEFINITION_MISSING (never 0) and leaves *outModelRuntime unchanged.
*/
uint32_t ModelRuntimePool_CreateInstanceByDefinitionId
          (GraphicsPaletteAsset *paletteAsset,GraphicsTextureSet *textureSet,
          ArmyRuntimeSlot *armyRuntime,PckModelDefinitionIdCatalog modelDefinitionId,
          WorldRuntimeContext *worldRuntime,ModelRuntimeSlot **outModelRuntime)

{
  uint32_t copiedValueA;
  uint32_t state44CandidateOrFlags;
  uint32_t state90Candidate;
  uint32_t copiedValueB;
  uint32_t copiedValueC;
  ModelRuntimeNode *modelNodeRuntime;
  int slotsRemaining;
  int registryRemaining;
  ModelDefinitionRecordPrefix **registryEntry;
  ModelRuntimeSlot *modelRuntime;
  ModelDefinition *definitionView;

  /* first free slot (no root node) */
  modelRuntime = g_ModelRuntimeSlots;
  if (modelRuntime == NULL) {
    return FATAL_ERROR_GENERAL_FAILURE;
  }
  slotsRemaining = MODEL_RUNTIME_SLOT_COUNT;
  while (modelRuntime->rootModelNodeOrSavedOffset.modelNode != NULL) {
    modelRuntime++;
    slotsRemaining--;
    if (slotsRemaining == 0) {
      return FATAL_ERROR_GENERAL_FAILURE;
    }
  }
  for (registryEntry = g_ModelDefinitionRegistry, registryRemaining = MODEL_DEFINITION_REGISTRY_SLOT_COUNT;
      registryRemaining != 0; registryEntry++, registryRemaining--) {
    definitionView = (ModelDefinition *)*registryEntry;
    if ((definitionView != NULL) && (definitionView->definitionId == modelDefinitionId)) {
      modelRuntime->definitionOrSavedId.definition = (ModelDefinitionRecordPrefix *)definitionView;
      copiedValueA = definitionView->maximumHealth;
      state44CandidateOrFlags = definitionView->visibilityRadius;
      state90Candidate = definitionView->occupancyMarkRadius;
      modelRuntime->rootModelNodeOrSavedOffset.modelNode = NULL;
      modelRuntime->ownerArmyRuntimeOrSavedOffset.armyRuntime = armyRuntime;
      modelRuntime->attachmentCount = 0;
      modelRuntime->health = copiedValueA;
      if (armyRuntime->visibilityRadius < state44CandidateOrFlags) {
        armyRuntime->visibilityRadius = state44CandidateOrFlags;
      }
      if (armyRuntime->occupancyMarkRadius < state90Candidate) {
        armyRuntime->occupancyMarkRadius = state90Candidate;
      }
      modelRuntime->classPrefixState[0] = 0;
      modelRuntime->classPrefixState[1] = 0;
      modelRuntime->classPrefixState[2] = 0;
      modelRuntime->classPrefixState[3] = 0;
      modelRuntime->classPrefixState[4] = 0;
      modelRuntime->classPrefixState[5] = 0;
      modelRuntime->classPrefixState[6] = 0;
      modelRuntime->classPrefixState[7] = 0;
      modelRuntime->classPrefixState[8] = 0;
      modelRuntime->classPrefixState[9] = 0;
      modelRuntime->classPrefixState[10] = 0;
      modelRuntime->classPrefixState[11] = 0;
      modelRuntime->classPrefixState[12] = 0;
      modelRuntime->classPrefixState[13] = 0;
      modelRuntime->classPrefixState[14] = 0;
      modelRuntime->classPrefixState[15] = 0;
      modelRuntime->classPrefixState[16] = 0;
      modelRuntime->classPrefixState[17] = 0;
      modelRuntime->classPrefixState[18] = 0;
      modelRuntime->classPrefixState[19] = 0;
      modelRuntime->classPrefixState[20] = 0;
      modelRuntime->classPrefixState[21] = 0;
      modelRuntime->classPrefixState[22] = 0;
      modelRuntime->classPrefixState[23] = 0;
      modelRuntime->classPrefixState[24] = 0;
      modelRuntime->classPrefixState[25] = 0;
      modelRuntime->classPrefixState[26] = 0;
      modelRuntime->classPrefixState[27] = 0;
      modelRuntime->classPrefixState[28] = 0;
      modelRuntime->classPrefixState[29] = 0;
      modelRuntime->classPrefixState[30] = 0;
      modelRuntime->classPrefixState[31] = 0;
      modelRuntime->linkedModelRuntimeOrSavedOffset.modelRuntime = NULL;
      copiedValueA = definitionView->destructionEffectDelayTicks1;
      copiedValueB = definitionView->destructionEffectDelayTicks2;
      copiedValueC = definitionView->destructionEffectDelayTicks3;
      modelRuntime->destructionEffectTimers[0] = definitionView->destructionEffectDelayTicks0;
      modelRuntime->destructionEffectTimers[1] = copiedValueA;
      modelRuntime->destructionEffectTimers[2] = copiedValueB;
      modelRuntime->destructionEffectTimers[3] = copiedValueC;
      copiedValueA = definitionView->destructionEffectDelayTicks5;
      copiedValueB = definitionView->destructionEffectDelayTicks6;
      copiedValueC = definitionView->destructionEffectDelayTicks7;
      modelRuntime->destructionEffectTimers[4] = definitionView->destructionEffectDelayTicks4;
      modelRuntime->destructionEffectTimers[5] = copiedValueA;
      modelRuntime->destructionEffectTimers[6] = copiedValueB;
      modelRuntime->destructionEffectTimers[7] = copiedValueC;
      copiedValueA = definitionView->energyLoadQ4;
      modelRuntime->classState.shotEmitterTimerTicks = 1;
      modelRuntime->classState.effectEmitterTimerTicks = 1;
      modelRuntime->classState.linkedArmyRuntimeOrSavedOffset.modelRuntime = NULL;
      modelRuntime->classState.energyLoadQ4 = copiedValueA;
      modelRuntime->classState.stateFlags = 0;
      modelRuntime->classState.healthRegenerationDelayTicks = 0;
      modelRuntime->classState.dismantleTickCountdown = 0;
      modelRuntime->damageEffectPointIndex = 0;
      state44CandidateOrFlags = definitionView->modelFlags;
      if ((MdlSerializedNodeHeader *)definitionView->rootNodeOffsetOrPointer != NULL) {
        if (!ModelNodeRuntime_CreateHierarchyRecursive
                (paletteAsset,textureSet,modelRuntime,
                 (MdlSerializedNodeHeader *)definitionView->rootNodeOffsetOrPointer,worldRuntime,
                 &modelNodeRuntime)) {
          /* the hierarchy's error code: no free world object record */
          return FATAL_ERROR_GENERAL_FAILURE;
        }
        modelRuntime->rootModelNodeOrSavedOffset.modelNode = modelNodeRuntime;
        ModelNodeRuntime_RecomputeSubtreeBoundingRadius(modelNodeRuntime);
        ModelNodeRuntime_RebuildTransformsFromRoot(modelNodeRuntime);
        if ((state44CandidateOrFlags & MODEL_DEFINITION_FLAG_RAY_TRANSPARENT) != 0) {
          modelNodeRuntime->runtimeFlags = modelNodeRuntime->runtimeFlags | MODEL_NODE_FLAG_RAY_TRANSPARENT;
        }
      }
      (*g_ArmyRuntimeOrderHandlerMatrix11Columns24Classes.modelClassInitialize
        [modelRuntime->definitionOrSavedId.runtimeDefinition->runtimeClassId])
                (modelRuntime->definitionOrSavedId.definition,modelRuntime);
      *outModelRuntime = modelRuntime;
      return 0;
    }
  }
  g_WideNumberFormatUtf16
            (WIDE_FORMAT_WRITE_TERMINATOR,0,10,1,modelDefinitionId,g_PackageLastErrorPath);
  return FATAL_ERROR_MODEL_DEFINITION_MISSING;
}


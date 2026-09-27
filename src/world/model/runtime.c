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
   Ownership: world/model/runtime.
   Purpose: Creates one deferred child model runtime, installs it into its saved parent slot, and restores the
   serialized local transform and parent link. Role: Repairs a child model link that could not be instantiated
   during the first recursive pass. Inputs: Deferred child descriptor and parent/runtime context. Outputs: Created
   child ModelRuntimeSlot/Node linked into the parent graph. Edges: Re-enters
   ModelRuntimePool_CreateInstanceByDefinitionIdCf.
   Local calls: ModelRuntimePool_CreateInstanceByDefinitionIdCf.
*/
ModelNodeCreateResult __thandor_eax_cf_preserve_ecx_edx
ModelRuntimePool_RepairDeferredChild
          (GraphicsPaletteAsset *paletteAsset,GraphicsTextureSet *textureSet,
          ModelRuntimeAttachmentIndex attachmentIndex,PckModelDefinitionIdCatalog childDefinitionId,
          ModelRuntimeSlot *modelRuntime,WorldRuntimeContext *worldRuntime)

{
  ModelAttachmentTransformRecord *sourceTransform;
  AngleTurn32 rotationAngle0;
  AngleTurn32 rotationAngle1;
  AngleTurn32 rotationAngle2;
  Q12 translationX;
  Q12 translationY;
  ModelNodeCreateResult createResult;
  ModelNodeCreateResult repairResult;
  ModelRuntimeNode *parentModelNode;
  ModelRuntimeNode *childRootNode;
  
  if (attachmentIndex < modelRuntime->attachmentCount0C) {
    createResult = ModelRuntimePool_CreateInstanceByDefinitionIdCf
                      (paletteAsset,textureSet,
                       (modelRuntime->ownerArmyRuntimeOrSavedOffset).armyRuntime,childDefinitionId,
                       worldRuntime);
    if (createResult.failed) {
      createResult.failed = true;
      return createResult;
    }
    modelRuntime->attachments140[attachmentIndex].childModelRuntimeOrSavedOffset00 = createResult.modelNode
    ;
    parentModelNode = modelRuntime->attachments140[attachmentIndex].parentModelNodeOrSavedOffset08;
    sourceTransform = modelRuntime->attachments140[attachmentIndex].sourceTransform04;
    childRootNode = (((ModelRuntimeSlot *)createResult.modelNode)->rootModelNodeOrSavedOffset).modelNode;
    rotationAngle0 = modelRuntime->attachments140[attachmentIndex].childLocalRotationAngle0;
    rotationAngle1 = modelRuntime->attachments140[attachmentIndex].childLocalRotationAngle1;
    rotationAngle2 = modelRuntime->attachments140[attachmentIndex].childLocalRotationAngle2;
    parentModelNode->childNodes[modelRuntime->attachments140[attachmentIndex].childNodeIndex0C] =
         childRootNode;
    (((WorldRuntimeNodePayload *)&childRootNode->modelPayload)->model).localRotationAngle2 = rotationAngle2;
    (((WorldRuntimeNodePayload *)&childRootNode->modelPayload)->model).localRotationAngle1 = rotationAngle1;
    (((WorldRuntimeNodePayload *)&childRootNode->modelPayload)->model).localRotationAngle0 = rotationAngle0;
    childRootNode->parentNode = parentModelNode;
    translationX = sourceTransform->localTranslationXQ12;
    translationY = sourceTransform->localTranslationYQ12;
    (((WorldRuntimeNodePayload *)&childRootNode->modelPayload)->model).localTranslationZQ12 =
         sourceTransform->localTranslationZQ12;
    (((WorldRuntimeNodePayload *)&childRootNode->modelPayload)->model).localTranslationYQ12 = translationY;
    (((WorldRuntimeNodePayload *)&childRootNode->modelPayload)->model).localTranslationXQ12 = translationX;
    repairResult.failed = false;
    repairResult.modelNode = createResult.modelNode;
    return repairResult;
  }
  /* Index past the attachment count: the original leaves EAX untouched, and in its only caller
     (ModelNodeRuntime_InstantiateLinkedChildrenRecursiveCf) EAX holds childDefinitionId at the call. */
  repairResult.failed = false;
  repairResult.modelNode = (ModelRuntimeNode *)(uintptr_t)childDefinitionId;
  return repairResult;
}


/* Address: 0x004BDDB0.
   Ownership: world/model/runtime.
   Purpose: Performs frustum and depth tests, chooses the model LOD, renders the accepted node, and recursively
   traverses its child hierarchy.
   Cross-module calls: FixedVec3_DotQ28 [core/math/fixed], FixedMath_Length3 [core/math/fixed],
   FixedTransform_ApplyPoint [core/math/fixed], GraphicsShadingRuntime_CollectNearbyRecords
   [graphics/render/shading], ModelRender_DrawMeshGroupsWithTemporaryTransform [graphics/render/model].
*/
void __thandor_void_preserve_eax_ecx_edx
ModelRuntime_CullAndRenderHierarchyRecursive(ModelRuntimeNode *modelNodeRuntime)

{
  Q12 *boundingRadiusField;
  ModelResourceHitTestAndRenderView210 *renderView;
  uint32_t radiusOrMeshGroupCount;
  int32_t planeDistance;
  uint32_t distanceOrChildrenRemaining;
  int subtreeRadiusOrChildIndex;
  ModelMeshGroupRelativeOffset *meshGroup;
  int nodeRadius;
  Q12 projectedRadiusScale;
  
  if (modelNodeRuntime != (ModelRuntimeNode *)0x0) {
    modelNodeRuntime->runtimeFlags = modelNodeRuntime->runtimeFlags & 0xfffffffd;
    g_ModelCullViewRelativeX =
         (modelNodeRuntime->worldTransform).translation.x - g_ViewOriginFixed.x;
    g_ModelCullViewRelativeY =
         (modelNodeRuntime->worldTransform).translation.y - g_ViewOriginFixed.y;
    g_ModelCullViewRelativeZ =
         (modelNodeRuntime->worldTransform).translation.z - g_ViewOriginFixed.z;
    renderView = (modelNodeRuntime->modelPayload).modelResource;
    subtreeRadiusOrChildIndex = modelNodeRuntime->subtreeBoundingRadiusQ12 + modelNodeRuntime->renderDepthBiasOrState;
    nodeRadius = renderView->boundingRadiusQ12 + modelNodeRuntime->renderDepthBiasOrState;
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
              if ((int)distanceOrChildrenRemaining < (int)radiusOrMeshGroupCount) {
                projectedRadiusScale = 0x10000000;
              }
              else {
                projectedRadiusScale = (Q12)(((uint64_t)radiusOrMeshGroupCount << 0x1c) / (uint64_t)distanceOrChildrenRemaining); /* unsigned DIV */
              }
              FixedTransform_ApplyPoint
                        ((GraphicsFixedVec3 *)&g_ModelCullViewRelativeX,
                         &(modelNodeRuntime->worldTransform).translation,
                         &g_ViewProjectionMatrixFixed);
              renderView = (modelNodeRuntime->modelPayload).modelResource;
              if ((int)g_ModelCullViewRelativeZ <= (int)g_ProjectionScaleFixed) {
                return;
              }
              boundingRadiusField = &renderView->boundingRadiusQ12;
              if (g_ModelCullViewRelativeZ - g_ProjectionScaleFixed != *boundingRadiusField &&
                  *boundingRadiusField <= (int)(g_ModelCullViewRelativeZ - g_ProjectionScaleFixed)) {
                modelNodeRuntime->runtimeFlags = modelNodeRuntime->runtimeFlags | 2;
                g_GraphicsShadingNearbyRecordCount = 0;
                GraphicsShadingRuntime_CollectNearbyRecords
                          (renderView->boundingRadiusQ12,g_ModelCullViewRelativeZ,
                           g_ModelCullViewRelativeY,g_ModelCullViewRelativeX);
                renderView = (modelNodeRuntime->modelPayload).modelResource;
                radiusOrMeshGroupCount = renderView->meshGroupCount;
                meshGroup = &renderView->firstMeshGroupRelativeOffset;
                if (((((uint32_t)g_ModelLodDepthThresholdQ8 < (int)g_ModelCullViewRelativeZ) && (1 < radiusOrMeshGroupCount))
                    && (meshGroup = (ModelMeshGroupRelativeOffset *)((int)meshGroup + *meshGroup),
                       (uint32_t)g_ModelLodDepthThresholdQ8 < (uint32_t)((int)g_ModelCullViewRelativeZ >> 1)
                       )) && (2 < radiusOrMeshGroupCount)) {
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
          if (modelNodeRuntime->childNodes[subtreeRadiusOrChildIndex] != (ModelRuntimeNode *)0x0) {
            ModelRuntime_CullAndRenderHierarchyRecursive(modelNodeRuntime->childNodes[subtreeRadiusOrChildIndex]);
          }
          subtreeRadiusOrChildIndex = subtreeRadiusOrChildIndex + 1;
          distanceOrChildrenRemaining = distanceOrChildrenRemaining - 1;
        } while (distanceOrChildrenRemaining != 0);
      }
    }
  }
  return;
}


/* Address: 0x004BE270.
   Ownership: world/model/runtime.
   Purpose: Handles model runtime render hierarchy recursive alternate path.
   Cross-module calls: FixedTransform_ApplyPoint [core/math/fixed], FixedMath_Length3 [core/math/fixed],
   GraphicsShadingRuntime_CollectNearbyRecords [graphics/render/shading], ModelRender_DrawMeshGroupsAlternatePath
   [graphics/render/model].
*/
void __thandor_void_preserve_eax_ecx_edx
ModelRuntime_RenderHierarchyRecursiveAlternatePath(ModelRuntimeNode *modelNode)

{
  ModelResourceHitTestAndRenderView210 *modelResourceView;
  uint32_t boundsLengthOrChildrenRemaining;
  int childIndex;
  
  modelResourceView = (modelNode->modelPayload).modelResource;
  if (modelNode != (ModelRuntimeNode *)0x0) {
    modelNode->runtimeFlags = modelNode->runtimeFlags | 2;
    iRam004bcf50 = modelResourceView->localBoundsX0Q12 + modelResourceView->localBoundsX1Q12 >> 1;
    iRam004bcf54 = modelResourceView->localBoundsY0Q12 + modelResourceView->localBoundsY1Q12 >> 1;
    iRam004bcf58 = modelResourceView->localBoundsZ0Q12 + modelResourceView->localBoundsZ1Q12 >> 1;
    FixedTransform_ApplyPoint
              ((GraphicsFixedVec3 *)&g_ModelCullViewRelativeX,(GraphicsFixedVec3 *)THANDOR_ADDR(g_GraphicsDirectionWorld,0),
               &g_ViewProjectionMatrixFixed);
    boundsLengthOrChildrenRemaining = FixedMath_Length3(modelResourceView->localBoundsZ1Q12 - modelResourceView->localBoundsZ0Q12,
                              modelResourceView->localBoundsY1Q12 - modelResourceView->localBoundsY0Q12,
                              modelResourceView->localBoundsX1Q12 - modelResourceView->localBoundsX0Q12);
    GraphicsShadingRuntime_CollectNearbyRecords
              ((int)boundsLengthOrChildrenRemaining >> 1,g_ModelCullViewRelativeZ,g_ModelCullViewRelativeY,
               g_ModelCullViewRelativeX);
    ModelRender_DrawMeshGroupsAlternatePath(modelNode->runtimeStateA0,modelNode);
    childIndex = 0;
    for (boundsLengthOrChildrenRemaining = modelNode->childCount; boundsLengthOrChildrenRemaining != 0; boundsLengthOrChildrenRemaining = boundsLengthOrChildrenRemaining - 1) {
      if (modelNode->childNodes[childIndex] != (ModelRuntimeNode *)0x0) {
        ModelRuntime_RenderHierarchyRecursiveAlternatePath(modelNode->childNodes[childIndex]);
      }
      childIndex = childIndex + 1;
    }
  }
  return;
}


/* Address: 0x0050B440.
   Ownership: world/model/runtime.
   Purpose: Kept distinct from Q12 coordinates, Q4/Q5 resource scales, attachment ordinals, and raw renderer flags.
   Explicit Q12 fixed-point value proved by the accepted parameter name and fixed-math/geometry consumer. Storage
   remains one signed 32-bit word. Typed parameters: p0 elevationAngle→AngleTurn32, p1 azimuthAngle→AngleTurn32, p2
   maximumDistanceQ12→Q12. Calling convention, storage, body bytes, control flow, and executable data remain
   unchanged.
   Cross-module calls: DepthInterval_BuildBinMask [graphics/render/primitives], FixedMath_WriteDirectionQ28
   [core/math/fixed], DepthBinMasks_OverlapCf [graphics/render/primitives],
   ModelNodeRuntime_RaycastHierarchyNearestCf [world/model/hierarchy].
*/
ModelRaycastResult __thandor_eax_edx_cf_preserve_ecx
ModelRuntime_RaycastCandidateListNearestCf
          (AngleTurn32 elevationAngle,AngleTurn32 azimuthAngle,Q12 maximumDistanceQ12,Q12 originZQ12
          ,Q12 originYQ12,Q12 originXQ12,WorldOwnerRuntimeClassId requiredOwnerId,
          ModelRuntimeNode *excludedNode,WorldRuntimeContext *worldRuntime)

{
  ModelRuntimeNode *modelNodeRuntime;
  DepthBinMask32 secondMaskHigh;
  DepthBinMask32 secondMaskLow;
  DepthIntervalCenter32 centerDepth;
  int bestDistanceQ12;
  ModelRuntimeNode *nearestModelNode;
  bool masksOverlap;
  ModelRaycastResult raycastHit;
  
  g_ModelRaycastOriginX = originXQ12;
  g_ModelRaycastOriginY = originYQ12;
  g_ModelRaycastOriginZ = originZQ12;
  g_ModelRaycastMaximumDistance = maximumDistanceQ12;
  secondMaskHigh = DepthInterval_BuildBinMask(maximumDistanceQ12,originXQ12);
  secondMaskLow = DepthInterval_BuildBinMask(maximumDistanceQ12,originYQ12);
  FixedMath_WriteDirectionQ28
            ((GraphicsFixedVec3 *)&g_ModelRaycastWorldDirectionXQ28,elevationAngle,azimuthAngle);
  nearestModelNode = (ModelRuntimeNode *)0x0;
  bestDistanceQ12 = 0x7fffffff;
  for (modelNodeRuntime = (ModelRuntimeNode *)worldRuntime->ownerListHead;
      modelNodeRuntime != (ModelRuntimeNode *)0x0;
      modelNodeRuntime = (ModelRuntimeNode *)(modelNodeRuntime->common).nextNode) {
    if ((((modelNodeRuntime != excludedNode) && (modelNodeRuntime->ownerClassId == requiredOwnerId))
        && ((modelNodeRuntime->runtimeFlags & 0x2000) == 0)) &&
       (masksOverlap = DepthBinMasks_OverlapCf
                          (modelNodeRuntime->depthBinMaskFar,modelNodeRuntime->depthBinMaskNear,
                           secondMaskLow,secondMaskHigh), masksOverlap)) {
      raycastHit = ModelNodeRuntime_RaycastHierarchyNearestCf(modelNodeRuntime);
      if (raycastHit.nearestDistanceQ12 <= bestDistanceQ12) {
        bestDistanceQ12 = raycastHit.nearestDistanceQ12;
        nearestModelNode = raycastHit.nearestNodeOrScratch.nearestModelNode;
      }
    }
  }
  raycastHit.nearestNodeOrScratch.nearestModelNode = nearestModelNode;
  raycastHit.nearestDistanceQ12 = bestDistanceQ12;
  raycastHit.hit = bestDistanceQ12 != 0x7fffffff;
  return raycastHit;
}


/* Address: 0x0051C240.
   Ownership: world/model/runtime.
   Purpose: EXACT_SCALAR_TWIN_OF_MODEL_SCALE_RATIO_REGISTER_WRAPPER.
   Cross-module calls: ModelRuntimeHierarchy_ComputeScaleRatioQ12Regs [world/model/hierarchy].
*/
Q12 __thandor_eax_preserve_ecx_edx
ModelRuntime_QueryHierarchyScaleRatioQ12(RuntimeModelFactionPrefix10 *runtimeEntry)

{
  ModelRuntimeScaleRatioRegisterPairQ12 scaleRatioPairQ12;
  
  scaleRatioPairQ12 = ModelRuntimeHierarchy_ComputeScaleRatioQ12Regs(runtimeEntry->modelRuntime);
  return (Q12)scaleRatioPairQ12;
}


/* Address: 0x0051C260.
   Ownership: world/model/runtime.
   Purpose: Queries the root model runtime hierarchy for the recursive Q12 scale-ratio result returned by the
   shared hierarchy metric routine. Queries the linked ModelRuntimeSlot and preserves the verified scale-ratio
   register pair returned in EDX:EAX.
   Cross-module calls: ModelRuntimeHierarchy_ComputeScaleRatioQ12Regs [world/model/hierarchy].
*/
ModelRuntimeScaleRatioRegisterPairQ12 __thandor_eax_edx_cf_preserve_ecx
ModelRuntime_QueryHierarchyScaleRatioQ12Regs(RuntimeModelFactionPrefix10 *runtimeEntry)

{
  ModelRuntimeScaleRatioRegisterPairQ12 hierarchyScaleRatioPairQ12;
  
  hierarchyScaleRatioPairQ12 =
       ModelRuntimeHierarchy_ComputeScaleRatioQ12Regs(runtimeEntry->modelRuntime);
  return hierarchyScaleRatioPairQ12;
}


/* Address: 0x0051C280.
   Ownership: world/model/runtime.
   Purpose: Queries the root model runtime hierarchy and returns the low active-metric result from the shared
   active-and-total hierarchy metric routine. Returns only the active hierarchy metric in EAX. EDX is explicitly
   saved and restored by the wrapper, so the previous fastcall register parameters and undefined8 return were
   synthetic.
   Cross-module calls: ModelRuntimeHierarchy_ComputeActiveAndTotalMetricsRegs [world/model/hierarchy].
*/
int __thandor_eax_preserve_ecx_edx
ModelRuntime_QueryActiveHierarchyMetric(ArmyRuntimeSlot *modelRuntimeHolder)

{
  ModelRuntimeActiveTotalMetricRegisterPair activeHierarchyMetricPair;
  
  activeHierarchyMetricPair =
       ModelRuntimeHierarchy_ComputeActiveAndTotalMetricsRegs
                 ((modelRuntimeHolder->modelRuntimeOrSavedOffset).modelRuntime);
  return (int)activeHierarchyMetricPair;
}


/* Address: 0x0051C2A0.
   Ownership: world/model/runtime.
   Purpose: Queries the root model runtime hierarchy and preserves both active and total metric results returned in
   the verified register pair. Queries the linked ModelRuntimeSlot and preserves both active and total hierarchy
   metrics in the verified EDX:EAX pair.
   Cross-module calls: ModelRuntimeHierarchy_ComputeActiveAndTotalMetricsRegs [world/model/hierarchy].
*/
ModelRuntimeActiveTotalMetricRegisterPair
ModelRuntime_QueryActiveAndTotalHierarchyMetricsRegs(RuntimeModelFactionPrefix10 *runtimeEntry)

{
  ModelRuntimeActiveTotalMetricRegisterPair activeTotalMetrics;
  
  activeTotalMetrics =
       ModelRuntimeHierarchy_ComputeActiveAndTotalMetricsRegs(runtimeEntry->modelRuntime);
  return activeTotalMetrics;
}


/* Address: 0x00528A40.
   Ownership: world/model/runtime.
   Purpose: Allocates and zeroes the exact 0x400000-byte model runtime pool, equal to 8192 ModelRuntimeSlot
   records.
*/
StatusResult __cdecl ModelRuntimePool_Init(void)

{
  ModelRuntimeSlot *modelRuntimeStorageCursor;
  int allocationDwordsRemaining;
  ArenaAllocResult allocResult;
  StatusResult statusResult;
  
  allocResult = (*g_MemoryApi.alloc)(0x400000);
  modelRuntimeStorageCursor = (ModelRuntimeSlot *)allocResult.payloadOrError;
  if (!allocResult.failed) {
    g_ModelRuntimeRebaseDelta = (int)&modelRuntimeStorageCursor[-1].attachments140[5].reserved1C + 3
    ;
    g_ModelRuntimeSlots = modelRuntimeStorageCursor;
    for (allocationDwordsRemaining = 0x100000; allocationDwordsRemaining != 0;
        allocationDwordsRemaining = allocationDwordsRemaining + -1) {
      (modelRuntimeStorageCursor->definitionOrSavedId).definition =
           (ModelDefinitionRecordPrefix *)0x0;
      modelRuntimeStorageCursor =
           (ModelRuntimeSlot *)&modelRuntimeStorageCursor->rootModelNodeOrSavedOffset;
    }
    allocResult.payloadOrError = 0;
    allocResult.failed = false;
  }
  statusResult.valueOrError = allocResult.payloadOrError;
  statusResult.failed = allocResult.failed;
  return statusResult;
}


/* Releases the resource of one definition node and then, depth first in index order, of all its
   children (child count at +0x14, child pointers from +0x18). The original walks the tree with an
   explicit {count, index, node} frame stack on the machine stack; the decompile only followed the
   first child. */
static void ModelRuntimePool_ReleaseDefinitionNodeResources(uint32_t resourceRecord)

{
  uint32_t childrenRemaining;
  int childIndex;
  
  childrenRemaining = *(uint32_t *)(resourceRecord + 0x14);
  if (((*(uint32_t *)(resourceRecord + 4) & 0xf) == 0) && (*(int *)(resourceRecord + 0x34) != 0)) {
    Resource_Release(*(void **)(resourceRecord + 0x30));
  }
  for (childIndex = 0; childrenRemaining != 0; childIndex = childIndex + 1) {
    ModelRuntimePool_ReleaseDefinitionNodeResources(*(uint32_t *)(resourceRecord + 0x18 + childIndex * 4));
    childrenRemaining = childrenRemaining - 1;
  }
  return;
}


/* Address: 0x00528A70.
   Ownership: world/model/runtime.
   Purpose: Frees the model runtime pool and releases registered definition resources. The function returns no
   semantic value.
   Cross-module calls: Resource_Release [assets/resource/runtime].
*/
void __thandor_void_preserve_eax_ecx_edx ModelRuntimePool_ShutdownAndReleaseDefinitions(void)

{
  int registryRemaining;
  ModelDefinitionRecordPrefix **registryEntry;
  uint32_t resourceRecord;
  
  (*g_MemoryApi.free)(g_ModelRuntimeSlots);
  g_ModelRuntimeSlots = (ModelRuntimeSlot *)0x0;
  registryEntry = g_ModelDefinitionRegistry;
  for (registryRemaining = 0x300; registryRemaining != 0; registryRemaining = registryRemaining + -1) {
    if ((*registryEntry != (ModelDefinitionRecordPrefix *)0x0) &&
       (resourceRecord = (*registryEntry)[8].flags, resourceRecord != 0)) {
      ModelRuntimePool_ReleaseDefinitionNodeResources(resourceRecord);
    }
    *registryEntry = (ModelDefinitionRecordPrefix *)0x0;
    registryEntry = registryEntry + 1;
  }
  return;
}



/* Address: 0x00528B30.
   Ownership: world/model/runtime.
   Purpose: Converts the live model-runtime pool back to serialized offsets and dispatches the 24-entry per-class
   unrebase callback partition before save. Function-specific scalar serialized model-slot and attachment views
   expose direct saved-id/offset dwords and eliminate union-member selection from save/unrebase writes; live
   ModelRuntimeSlot remains unchanged. Saved ids, relocated pointers, attachment selectors, and runtime class ids
   remain separate.
*/
void __cdecl ModelRuntimePool_UnrebaseBeforeSave(void)

{
  ModelRuntimeSlotSerializedScalarView200 *linkedModelOffset;
  ModelRuntimePoolRelativeOffset childRuntimeOffset;
  uint32_t offsetClassOrCount;
  int dwordsRemaining;
  int slotsRemaining;
  ModelNodePoolRelativeOffset parentNodeOffset;
  ModelRuntimeSlotUnrebaseSemanticView200 *attachmentCursor;
  ModelRuntimeSlotUnrebaseSemanticView200 *modelRuntime;
  
  slotsRemaining = 0x2000;
  modelRuntime = (ModelRuntimeSlotUnrebaseSemanticView200 *)g_ModelRuntimeSlots;
  do {
    while( true ) {
      if (modelRuntime->rootModelNodeSavedOffset != 0) break;
      for (dwordsRemaining = 0x80; dwordsRemaining != 0; dwordsRemaining = dwordsRemaining + -1) {
        (modelRuntime->definitionReferenceOrSavedId).definition = (ModelDefinitionRecordPrefix *)0x0
        ;
        modelRuntime = (ModelRuntimeSlotUnrebaseSemanticView200 *)
                       &modelRuntime->rootModelNodeSavedOffset;
      }
      slotsRemaining = slotsRemaining + -1;
      if (slotsRemaining == 0) {
        return;
      }
    }
    offsetClassOrCount = modelRuntime->ownerArmyRuntimeSavedOffset - (int)g_ArmyRuntimeRebaseBaseMinusOne;
    modelRuntime->rootModelNodeSavedOffset =
         modelRuntime->rootModelNodeSavedOffset - (int)g_RuntimeObjectRebaseBaseMinusOne;
    modelRuntime->ownerArmyRuntimeSavedOffset = offsetClassOrCount;
    linkedModelOffset = (ModelRuntimeSlotSerializedScalarView200 *)modelRuntime->linkedModelRuntimeSavedOffset;
    offsetClassOrCount = (modelRuntime->classState).linkedArmyRuntimeSavedOffset;
    if (linkedModelOffset != (ModelRuntimeSlotSerializedScalarView200 *)0x0) {
      linkedModelOffset = (ModelRuntimeSlotSerializedScalarView200 *)((int)linkedModelOffset - g_ModelRuntimeRebaseDelta);
    }
    if (offsetClassOrCount != 0) {
      offsetClassOrCount = offsetClassOrCount - (int)g_ArmyRuntimeRebaseBaseMinusOne;
    }
    modelRuntime->linkedModelRuntimeSavedOffset = (uint32_t)linkedModelOffset;
    (modelRuntime->classState).linkedArmyRuntimeSavedOffset = offsetClassOrCount;
    offsetClassOrCount = (modelRuntime->definitionReferenceOrSavedId).definition[6].flags;
    modelRuntime->definitionReferenceOrSavedId =
         THANDOR_BITCAST(PckModelDefinitionIdCatalog, ModelDefinitionReferenceOrSavedId4, ((modelRuntime->definitionReferenceOrSavedId).definition)->definitionId);
    (*g_ArmyRuntimeOrderHandlerMatrix11Columns24Classes.modelUnrebase[offsetClassOrCount])
              ((ModelRuntimeSlot *)modelRuntime);
    attachmentCursor = modelRuntime;
    for (offsetClassOrCount = modelRuntime->attachmentCount0C; offsetClassOrCount != 0; offsetClassOrCount = offsetClassOrCount - 1) {
      childRuntimeOffset = attachmentCursor->attachments140[0].childModelRuntimeSavedOffset00;
      parentNodeOffset = attachmentCursor->attachments140[0].parentModelNodeSavedOffset08;
      if (childRuntimeOffset != 0) {
        childRuntimeOffset = childRuntimeOffset - g_ModelRuntimeRebaseDelta;
      }
      if (parentNodeOffset != 0) {
        parentNodeOffset = parentNodeOffset - (int)g_RuntimeObjectRebaseBaseMinusOne;
      }
      attachmentCursor->attachments140[0].childModelRuntimeSavedOffset00 = childRuntimeOffset;
      attachmentCursor->attachments140[0].parentModelNodeSavedOffset08 = parentNodeOffset;
      attachmentCursor = (ModelRuntimeSlotUnrebaseSemanticView200 *)(attachmentCursor->reserved10_37 + 0x10);
    }
    modelRuntime = modelRuntime + 1;
    slotsRemaining = slotsRemaining + -1;
  } while (slotsRemaining != 0);
  return;
}


/* Address: 0x00528CF0.
   Ownership: world/model/runtime.
   Purpose: Rebases 8192 exact 0x200-byte model runtime slots and rebuilds their six attachment descriptors. The
   function returns no semantic value. Saved ids, relocated pointers, attachment selectors, and runtime class ids
   remain separate.
   Cross-module calls: ModelRuntimeHierarchy_CollectAttachmentDescriptorsRecursive [world/model/hierarchy].
*/
void __thandor_void_preserve_eax_ecx_edx ModelRuntimePool_RebaseAfterLoad(void)

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
  
  slotsRemaining = 0x2000;
  modelRuntime = g_ModelRuntimeSlots;
  do {
    if ((modelRuntime->rootModelNodeOrSavedOffset).modelNode != (ModelRuntimeNode *)0x0) {
      ownerOrLinkedArmy = (ArmyRuntimeSlot *)
                  ((int)&((modelRuntime->ownerArmyRuntimeOrSavedOffset).armyRuntime)->
                         modelRuntimeOrSavedOffset + (int)g_ArmyRuntimeRebaseBaseMinusOne);
      (modelRuntime->rootModelNodeOrSavedOffset).modelNode =
           (ModelRuntimeNode *)
           (g_RuntimeObjectRebaseBaseMinusOne +
           (int)(&((modelRuntime->rootModelNodeOrSavedOffset).modelNode)->modelPayload + -1) + 0x30)
      ;
      (modelRuntime->ownerArmyRuntimeOrSavedOffset).armyRuntime = ownerOrLinkedArmy;
      ownerOrLinkedArmy = (modelRuntime->classState).linkedArmyRuntimeOrSavedOffset.armyRuntime;
      linkedRuntimeOrCursor = (ModelRuntimeSlot *)0x0;
      if ((modelRuntime->linkedModelRuntimeOrSavedOffset).modelRuntime != (ModelRuntimeSlot *)0x0) {
        linkedRuntimeOrCursor = (ModelRuntimeSlot *)
                     (((modelRuntime->linkedModelRuntimeOrSavedOffset).modelRuntime)->reserved10_37
                     + g_ModelRuntimeRebaseDelta + -0x10);
      }
      rebasedLinkedArmy = (ArmyRuntimeSlot *)0x0;
      if (ownerOrLinkedArmy != (ArmyRuntimeSlot *)0x0) {
        rebasedLinkedArmy = (ArmyRuntimeSlot *)
                    ((int)&ownerOrLinkedArmy->modelRuntimeOrSavedOffset +
                    (int)g_ArmyRuntimeRebaseBaseMinusOne);
      }
      (modelRuntime->linkedModelRuntimeOrSavedOffset).modelRuntime = linkedRuntimeOrCursor;
      (modelRuntime->classState).linkedArmyRuntimeOrSavedOffset.armyRuntime = rebasedLinkedArmy;
      registryEntry = g_ModelDefinitionRegistry;
      registryRemaining = 0x300;
      for (;;) {
        registeredDefinition = *registryEntry;
        if ((registeredDefinition != (ModelDefinitionRecordPrefix *)0x0) &&
           ((modelRuntime->definitionOrSavedId).definition ==
            (ModelDefinitionRecordPrefix *)registeredDefinition->definitionId)) {
          classIndexOrCount = registeredDefinition[6].flags;
          (modelRuntime->definitionOrSavedId).definition = registeredDefinition;
          (*g_ArmyRuntimeOrderHandlerMatrix11Columns24Classes.modelRebaseOrLoadRepair[classIndexOrCount])
                    (modelRuntime);
          classIndexOrCount = modelRuntime->attachmentCount0C;
          linkedRuntimeOrCursor = modelRuntime;
          if (classIndexOrCount != 0) {
            do {
              savedChildRuntime = linkedRuntimeOrCursor->attachments140[0].childModelRuntimeOrSavedOffset00;
              savedParentNode = linkedRuntimeOrCursor->attachments140[0].parentModelNodeOrSavedOffset08;
              rebasedChildRuntime = (ModelRuntimeSlot *)0x0;
              if (savedChildRuntime != (ModelRuntimeSlot *)0x0) {
                rebasedChildRuntime = (ModelRuntimeSlot *)
                             (savedChildRuntime->reserved10_37 + g_ModelRuntimeRebaseDelta + -0x10);
              }
              rebasedParentNode = (ModelRuntimeNode *)0x0;
              if (savedParentNode != (ModelRuntimeNode *)0x0) {
                rebasedParentNode = (ModelRuntimeNode *)
                             (g_RuntimeObjectRebaseBaseMinusOne +
                             (int)(&savedParentNode->modelPayload + -1) + 0x30);
              }
              linkedRuntimeOrCursor->attachments140[0].childModelRuntimeOrSavedOffset00 = rebasedChildRuntime;
              linkedRuntimeOrCursor->attachments140[0].parentModelNodeOrSavedOffset08 = rebasedParentNode;
              linkedRuntimeOrCursor = (ModelRuntimeSlot *)(linkedRuntimeOrCursor->reserved10_37 + 0x10);
              classIndexOrCount = classIndexOrCount - 1;
            } while (classIndexOrCount != 0);
            modelRuntime->attachmentCount0C = 0;
            ModelRuntimeHierarchy_CollectAttachmentDescriptorsRecursive
                      (modelRuntime,modelRuntime,
                       *(MdlSerializedNodeHeader38 **)
                        ((modelRuntime->definitionOrSavedId).savedIdOrOffset + 100));
          }
          break;
        }
        registryEntry = registryEntry + 1;
        registryRemaining = registryRemaining + -1;
        if (registryRemaining == 0) {
          /* definition no longer registered: drop the instance */
          (modelRuntime->rootModelNodeOrSavedOffset).modelNode = (ModelRuntimeNode *)0x0;
          break;
        }
      }
    }
    modelRuntime = modelRuntime + 1;
    slotsRemaining = slotsRemaining + -1;
    if (slotsRemaining == 0) {
      return;
    }
  } while( true );
}


/* Address: 0x00529560.
   Ownership: world/model/runtime.
   Purpose: Recursively destroys child model runtimes, releases owned world nodes, detaches the hierarchy from its
   parent or owner runtime, and refreshes derived army metrics. Model release partition slots 0-23 receive
   (modelDefinition, modelRuntime).
   Cross-module calls: FrontendPlayerRuntime_ClearAssignmentTokenFromAll [ui/frontend/player],
   WorldRuntime_ForEachNodeInOwnerListD8 [world/runtime/core], ModelRuntimeNode_ReleaseRecursiveAndDetachParent
   [world/model/hierarchy], ArmyRuntime_CreateInstanceFromAssetCf [gameplay/army/runtime],
   ArmyRuntime_DestroyInstanceAndRefreshUi [gameplay/army/runtime], ArmyRuntime_RebuildDerivedSelectionMetrics
   [gameplay/army/runtime].
*/
void __thandor_void_preserve_eax_ecx_edx
ModelRuntimePool_DestroyHierarchyAndDetach
          (WorldRuntimeContext *worldRuntime,ModelRuntimeSlot *modelRuntime)

{
  int *ownerRecord;
  ModelDefinitionRecordPrefix *modelDefinition;
  ModelRuntimeSlot *childRuntime;
  ModelRuntimeNode *rootModelNode;
  GameEntityRuntime *entityRuntime;
  Q12 worldYQ12;
  Q12 worldXQ12;
  AngleTurn32 orientationAngle;
  int ownerDefinition;
  uint32_t classIndexOrCount;
  ModelRuntimeSlot *attachmentCursor;
  ModelRuntimeNode *parentModelNode;
  
  modelDefinition =
       (ModelDefinitionRecordPrefix *)(modelRuntime->definitionOrSavedId).savedIdOrOffset;
  classIndexOrCount = modelDefinition[6].flags;
  FrontendPlayerRuntime_ClearAssignmentTokenFromAll((RuntimeToken)modelRuntime);
  (*g_ArmyRuntimeOrderHandlerMatrix11Columns24Classes.modelReleaseOrCommit[classIndexOrCount])
            (modelDefinition,modelRuntime);
  attachmentCursor = modelRuntime;
  for (classIndexOrCount = modelRuntime->attachmentCount0C; classIndexOrCount != 0; classIndexOrCount = classIndexOrCount - 1) {
    childRuntime = attachmentCursor->attachments140[0].childModelRuntimeOrSavedOffset00;
    if (childRuntime != (ModelRuntimeSlot *)0x0) {
      ModelRuntimePool_DestroyHierarchyAndDetach(worldRuntime,childRuntime);
    }
    attachmentCursor = (ModelRuntimeSlot *)(attachmentCursor->reserved10_37 + 0x10);
  }
  rootModelNode = (modelRuntime->rootModelNodeOrSavedOffset).modelNode;
  entityRuntime = (GameEntityRuntime *)(modelRuntime->ownerArmyRuntimeOrSavedOffset).armyRuntime;
  worldYQ12 = (rootModelNode->worldTransform).translation.x;
  worldXQ12 = (rootModelNode->worldTransform).translation.y;
  orientationAngle = (rootModelNode->modelPayload).worldRotationAngle2;
  parentModelNode = rootModelNode->parentNode;
  WorldRuntime_ForEachNodeInOwnerListD8
            (modelRuntime,WorldRuntimeNode_ClearDetachedEntityReferencesCallback,worldRuntime);
  ModelRuntimeNode_ReleaseRecursiveAndDetachParent(rootModelNode);
  (modelRuntime->rootModelNodeOrSavedOffset).modelNode = (ModelRuntimeNode *)0x0;
  if (parentModelNode == (ModelRuntimeNode *)0x0) {
    if ((entityRuntime->common).ownership.definitionOrClassRecord != (void *)0x0) {
      LOCK();
      ownerRecord = (entityRuntime->common).ownership.definitionOrClassRecord;
      (entityRuntime->common).ownership.definitionOrClassRecord = (void *)0x0;
      UNLOCK();
      ownerDefinition = *ownerRecord;
      if (((ownerRecord[0x3b] & 0x20U) == 0) && (*(int *)(ownerDefinition + 0x74) != -1)) {
        ArmyRuntime_CreateInstanceFromAssetCf
                  (0,orientationAngle,worldXQ12,worldYQ12,0,*(PckArmyAssetIdCatalog *)(ownerDefinition + 0x74)
                   ,worldRuntime);
      }
      ArmyRuntime_DestroyInstanceAndRefreshUi(worldRuntime,entityRuntime);
    }
  }
  else {
    attachmentCursor = (parentModelNode->runtimePayload).modelRuntime;
    for (classIndexOrCount = attachmentCursor->attachmentCount0C; classIndexOrCount != 0; classIndexOrCount = classIndexOrCount - 1) {
      if (attachmentCursor->attachments140[0].childModelRuntimeOrSavedOffset00 == modelRuntime) {
        attachmentCursor->attachments140[0].childModelRuntimeOrSavedOffset00 = (ModelRuntimeSlot *)0x0;
      }
      attachmentCursor = (ModelRuntimeSlot *)(attachmentCursor->reserved10_37 + 0x10);
    }
    ArmyRuntime_RebuildDerivedSelectionMetrics((ArmyRuntimeSlot *)entityRuntime);
  }
  return;
}


/* Address: 0x00529690.
   Ownership: world/model/runtime.
   Purpose: Eight stack arguments are authoritative from RET 0x20; prior EAX/EDX synthetic parameters and return
   were preserved-register noise. Scans catalog kind-2 records matched as (launchMode << 4 | 2), transforms each
   launch point to world, and creates projectiles. Role: Emits projectiles from every SPR attachment record whose
   low nibble is kind 2. Inputs: Current model hierarchy, SpriteAsset attachment table, ShotDefinition and target
   coordinates. Outputs: One ShotRuntime per matching launch attachment.
   Cross-module calls: ModelNodeRuntime_RebuildTransformsFromRoot [world/model/hierarchy],
   ModelNodeRuntime_TransformLocalPointRegs [world/model/hierarchy], ShotRuntimePool_CreateProjectileFromDefinition
   [world/shots/runtime].
*/
void __thandor_void_preserve_eax_ecx_edx
ModelRuntime_EmitProjectilesFromAttachmentPoints
          (ShotRuntimeState14 shotRuntimeState14,Q12 targetWorldZQ12,Q12 targetWorldYQ12,
          Q12 targetWorldXQ12,ShotDefinition *shotDefinition,ModelRuntimeNode *modelNodeRuntime,
          MdlSerializedNodeHeader38 *definitionNode,WorldRuntimeContext *worldRuntime)

{
  int modelPointRecordsRemaining;
  ModelPackedPointRecord *localPointRecord;
  WorldPositionXYRegisterPairQ12 attachmentWorldPointPairQ12;
  ModelLocalPointRegs12 launchPointWorld;
  AssetRecordByteCount modelPointTableBase;
  
  ModelNodeRuntime_RebuildTransformsFromRoot(modelNodeRuntime);
  modelPointTableBase = (definitionNode->spriteAssetReference).savedId;
  localPointRecord =
       (ModelPackedPointRecord *)(modelPointTableBase + *(int *)(modelPointTableBase + 0xe4));
  for (modelPointRecordsRemaining = *(int *)(modelPointTableBase + 0xe8);
      modelPointRecordsRemaining != 0; modelPointRecordsRemaining = modelPointRecordsRemaining + -1)
  {
    if ((localPointRecord->packedLookupKey & 0xf) == 2) {
      launchPointWorld = ModelNodeRuntime_TransformLocalPointRegs(localPointRecord,modelNodeRuntime);
      ShotRuntimePool_CreateProjectileFromDefinition
                (shotRuntimeState14,
                 (ArmyRuntimeSlot *)
                 ((modelNodeRuntime->runtimePayload).armyRuntime)->linkedEntityRuntime,
                 targetWorldZQ12,
                 (launchPointWorld.ecx - (modelNodeRuntime->worldTransform).translation.y) + targetWorldYQ12,
                 (launchPointWorld.eax - (modelNodeRuntime->worldTransform).translation.x) + targetWorldXQ12,
                 launchPointWorld.edx,launchPointWorld.ecx,launchPointWorld.eax,shotDefinition,worldRuntime);
    }
    localPointRecord = localPointRecord + 1;
  }
  return;
}


/* Address: 0x00529140.
   Ownership: world/model/runtime.
   Purpose: Allocates a model runtime slot, resolves its definition by identifier, initializes the exact runtime
   image, creates the root hierarchy, and runs the class-specific initialization handler. Role: Allocates a
   ModelRuntimeSlot, creates its node hierarchy and initializes transforms/radius. Inputs: Model definition ID,
   world context and initial orientation/position. Outputs: ModelRuntimeSlot with root ModelRuntimeNode and class-
   selected initialization. Edges: ModelNodeRuntime_CreateHierarchyRecursiveCf -> radius recompute -> transform
   rebuild.
   Cross-module calls: ModelNodeRuntime_CreateHierarchyRecursiveCf [world/model/hierarchy],
   ModelNodeRuntime_RecomputeSubtreeBoundingRadius [world/model/hierarchy],
   ModelNodeRuntime_RebuildTransformsFromRoot [world/model/hierarchy].
*/
ModelNodeCreateResult __thandor_eax_cf_preserve_ecx_edx
ModelRuntimePool_CreateInstanceByDefinitionIdCf
          (GraphicsPaletteAsset *paletteAsset,GraphicsTextureSet *textureSet,
          ArmyRuntimeSlot *armyRuntime,PckModelDefinitionIdCatalog modelDefinitionId,
          WorldRuntimeContext *worldRuntime)

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
  ModelNodeCreateResult failureResult;
  ModelNodeCreateResult createResult;
  ModelDefinitionRuntimeSemanticView280 *definitionView;

  /* first free slot (no root node); error 0x14 when the pool is missing or full */
  failureResult.failed = true;
  failureResult.modelNode = (ModelRuntimeNode *)0x14;
  modelRuntime = g_ModelRuntimeSlots;
  if (modelRuntime == (ModelRuntimeSlot *)0x0) {
    return failureResult;
  }
  slotsRemaining = 0x2000;
  while ((modelRuntime->rootModelNodeOrSavedOffset).modelNode != (ModelRuntimeNode *)0x0) {
    modelRuntime = modelRuntime + 1;
    slotsRemaining = slotsRemaining + -1;
    if (slotsRemaining == 0) {
      return failureResult;
    }
  }
  for (registryEntry = g_ModelDefinitionRegistry, registryRemaining = 0x300; registryRemaining != 0;
      registryEntry = registryEntry + 1, registryRemaining = registryRemaining + -1) {
    definitionView = (ModelDefinitionRuntimeSemanticView280 *)*registryEntry;
    if ((definitionView != (ModelDefinitionRuntimeSemanticView280 *)0x0) &&
       (definitionView->definitionId == modelDefinitionId)) {
      (modelRuntime->definitionOrSavedId).definition = (ModelDefinitionRecordPrefix *)definitionView;
      copiedValueA = definitionView->runtimeValue60;
      state44CandidateOrFlags = definitionView->runtimeValue48;
      state90Candidate = definitionView->runtimeValue27C;
      (modelRuntime->rootModelNodeOrSavedOffset).modelNode = (ModelRuntimeNode *)0x0;
      (modelRuntime->ownerArmyRuntimeOrSavedOffset).armyRuntime = armyRuntime;
      modelRuntime->attachmentCount0C = 0;
      modelRuntime->definitionValue60_3C = copiedValueA;
      if (armyRuntime->runtimeState44 < state44CandidateOrFlags) {
        armyRuntime->runtimeState44 = state44CandidateOrFlags;
      }
      if (armyRuntime->runtimeState90 < state90Candidate) {
        armyRuntime->runtimeState90 = state90Candidate;
      }
      modelRuntime->reserved10_37[0] = 0;
      modelRuntime->reserved10_37[1] = 0;
      modelRuntime->reserved10_37[2] = 0;
      modelRuntime->reserved10_37[3] = 0;
      modelRuntime->reserved10_37[4] = 0;
      modelRuntime->reserved10_37[5] = 0;
      modelRuntime->reserved10_37[6] = 0;
      modelRuntime->reserved10_37[7] = 0;
      modelRuntime->reserved10_37[8] = 0;
      modelRuntime->reserved10_37[9] = 0;
      modelRuntime->reserved10_37[10] = 0;
      modelRuntime->reserved10_37[0xb] = 0;
      modelRuntime->reserved10_37[0xc] = 0;
      modelRuntime->reserved10_37[0xd] = 0;
      modelRuntime->reserved10_37[0xe] = 0;
      modelRuntime->reserved10_37[0xf] = 0;
      modelRuntime->reserved10_37[0x10] = 0;
      modelRuntime->reserved10_37[0x11] = 0;
      modelRuntime->reserved10_37[0x12] = 0;
      modelRuntime->reserved10_37[0x13] = 0;
      modelRuntime->reserved10_37[0x14] = 0;
      modelRuntime->reserved10_37[0x15] = 0;
      modelRuntime->reserved10_37[0x16] = 0;
      modelRuntime->reserved10_37[0x17] = 0;
      modelRuntime->reserved10_37[0x18] = 0;
      modelRuntime->reserved10_37[0x19] = 0;
      modelRuntime->reserved10_37[0x1a] = 0;
      modelRuntime->reserved10_37[0x1b] = 0;
      modelRuntime->reserved10_37[0x1c] = 0;
      modelRuntime->reserved10_37[0x1d] = 0;
      modelRuntime->reserved10_37[0x1e] = 0;
      modelRuntime->reserved10_37[0x1f] = 0;
      (modelRuntime->linkedModelRuntimeOrSavedOffset).modelRuntime = (ModelRuntimeSlot *)0x0;
      copiedValueA = definitionView->runtimeValue8C;
      copiedValueB = definitionView->runtimeValue94;
      copiedValueC = definitionView->runtimeValue9C;
      modelRuntime->definitionValue84_40 = definitionView->runtimeValue84;
      modelRuntime->definitionValue88_44 = copiedValueA;
      modelRuntime->definitionValue94_48 = copiedValueB;
      modelRuntime->definitionValue9C_4C = copiedValueC;
      copiedValueA = definitionView->runtimeValueAC;
      copiedValueB = definitionView->runtimeValueB4;
      copiedValueC = definitionView->runtimeValueBC;
      modelRuntime->definitionValueA4_50 = definitionView->runtimeValueA4;
      modelRuntime->definitionValueAC_54 = copiedValueA;
      modelRuntime->definitionValueB4_58 = copiedValueB;
      modelRuntime->definitionValueBC_5C = copiedValueC;
      copiedValueA = definitionView->runtimeValue18C;
      (modelRuntime->classState).enabledStateE4 = 1;
      (modelRuntime->classState).enabledStateE8 = 1;
      (modelRuntime->classState).linkedArmyRuntimeOrSavedOffset.armyRuntime = (ArmyRuntimeSlot *)0x0
      ;
      (modelRuntime->classState).definitionDerivedValueF4 = copiedValueA;
      (modelRuntime->classState).classStateEC = 0;
      (modelRuntime->classState).classStateF8 = 0;
      (modelRuntime->classState).classStateFC = 0;
      modelRuntime->classState118 = 0;
      state44CandidateOrFlags = definitionView->runtimeValue68;
      if ((MdlSerializedNodeHeader38 *)definitionView->serializedNodeOffsetOrPointer64 !=
          (MdlSerializedNodeHeader38 *)0x0) {
        createResult = ModelNodeRuntime_CreateHierarchyRecursiveCf
                          (paletteAsset,textureSet,modelRuntime,
                           (MdlSerializedNodeHeader38 *)definitionView->serializedNodeOffsetOrPointer64,
                           worldRuntime);
        modelNodeRuntime = createResult.modelNode;
        if (createResult.failed) {
          failureResult.modelNode = modelNodeRuntime; /* the hierarchy's error code */
          return failureResult;
        }
        (modelRuntime->rootModelNodeOrSavedOffset).modelNode = modelNodeRuntime;
        ModelNodeRuntime_RecomputeSubtreeBoundingRadius(modelNodeRuntime);
        ModelNodeRuntime_RebuildTransformsFromRoot(modelNodeRuntime);
        if ((state44CandidateOrFlags & 0x100) != 0) {
          modelNodeRuntime->runtimeFlags = modelNodeRuntime->runtimeFlags | 0x2000;
        }
      }
      (*g_ArmyRuntimeOrderHandlerMatrix11Columns24Classes.modelClassInitialize
        [(modelRuntime->definitionOrSavedId).definition[6].flags])
                ((modelRuntime->definitionOrSavedId).definition,modelRuntime);
      createResult.failed = false;
      createResult.modelNode = (ModelRuntimeNode *)modelRuntime;
      return createResult;
    }
  }
  (*g_WideNumberFormatUtf16)
            (WIDE_FORMAT_WRITE_TERMINATOR,0,10,1,modelDefinitionId,g_PackageLastErrorPath);
  failureResult.modelNode = (ModelRuntimeNode *)0x3e; /* definition not registered */
  return failureResult;
}


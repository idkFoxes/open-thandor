/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/src/world/model/runtime.c
 * Reverse engineering by idkFoxes 2026
 */

#include <thandor/world/model/runtime.h>

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
ModelNodeCreateEaxCf5 __thandor_eax_cf_preserve_ecx_edx
ModelRuntimePool_RepairDeferredChild
          (GraphicsPaletteAsset *paletteAsset,GraphicsTextureSet *textureSet,
          ModelRuntimeAttachmentIndex attachmentIndex,PckModelDefinitionIdCatalog childDefinitionId,
          ModelRuntimeSlot *modelRuntime,WorldRuntimeContext *worldRuntime)

{
  ModelAttachmentTransformRecord *pMVar1;
  AngleTurn32 AVar2;
  AngleTurn32 AVar3;
  AngleTurn32 AVar4;
  Q12 QVar5;
  Q12 QVar6;
  ModelRuntimeSlot *in_EAX;
  ModelNodeCreateEaxCf5 MVar7;
  ModelNodeCreateEaxCf5 MVar8;
  ModelRuntimeNode *modelNode1;
  ModelRuntimeNode *modelNode2;
  
  if (attachmentIndex < modelRuntime->attachmentCount0C) {
    MVar7 = ModelRuntimePool_CreateInstanceByDefinitionIdCf
                      (paletteAsset,textureSet,
                       (modelRuntime->ownerArmyRuntimeOrSavedOffset).armyRuntime,childDefinitionId,
                       worldRuntime);
    if (MVar7.carry) {
      MVar7.carry = true;
      return MVar7;
    }
    modelRuntime->attachments140[attachmentIndex].childModelRuntimeOrSavedOffset00 = MVar7.modelNode
    ;
    modelNode1 = modelRuntime->attachments140[attachmentIndex].parentModelNodeOrSavedOffset08;
    pMVar1 = modelRuntime->attachments140[attachmentIndex].sourceTransform04;
    modelNode2 = ((MVar7.modelNode)->rootModelNodeOrSavedOffset).modelNode;
    AVar2 = modelRuntime->attachments140[attachmentIndex].childLocalRotationAngle0;
    AVar3 = modelRuntime->attachments140[attachmentIndex].childLocalRotationAngle1;
    AVar4 = modelRuntime->attachments140[attachmentIndex].childLocalRotationAngle2;
    modelNode1->childNodes[modelRuntime->attachments140[attachmentIndex].childNodeIndex0C] =
         modelNode2;
    (((WorldRuntimeNodePayload *)&modelNode2->modelPayload)->model).localRotationAngle2 = AVar4;
    (((WorldRuntimeNodePayload *)&modelNode2->modelPayload)->model).localRotationAngle1 = AVar3;
    (((WorldRuntimeNodePayload *)&modelNode2->modelPayload)->model).localRotationAngle0 = AVar2;
    modelNode2->parentNode = modelNode1;
    QVar5 = pMVar1->localTranslationXQ12;
    QVar6 = pMVar1->localTranslationYQ12;
    (((WorldRuntimeNodePayload *)&modelNode2->modelPayload)->model).localTranslationZQ12 =
         pMVar1->localTranslationZQ12;
    (((WorldRuntimeNodePayload *)&modelNode2->modelPayload)->model).localTranslationYQ12 = QVar6;
    (((WorldRuntimeNodePayload *)&modelNode2->modelPayload)->model).localTranslationXQ12 = QVar5;
    in_EAX = MVar7.modelNode;
  }
  MVar8.carry = false;
  MVar8.modelNode = (ModelRuntimeNode *)in_EAX;
  return MVar8;
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
  Q12 *pQVar1;
  ModelResourceHitTestAndRenderView210 *pMVar2;
  uint uVar3;
  sdword sVar4;
  dword dVar5;
  int iVar6;
  ModelMeshGroupRelativeOffset *meshGroup;
  int iVar7;
  Q12 local_20;
  
  if (modelNodeRuntime != (ModelRuntimeNode *)0x0) {
    modelNodeRuntime->runtimeFlags = modelNodeRuntime->runtimeFlags & 0xfffffffd;
    g_ModelCullViewRelativeX =
         (modelNodeRuntime->worldTransform).translation.x - g_ViewOriginFixed.x;
    g_ModelCullViewRelativeY =
         (modelNodeRuntime->worldTransform).translation.y - g_ViewOriginFixed.y;
    g_ModelCullViewRelativeZ =
         (modelNodeRuntime->worldTransform).translation.z - g_ViewOriginFixed.z;
    pMVar2 = (modelNodeRuntime->modelPayload).modelResource;
    iVar6 = modelNodeRuntime->subtreeBoundingRadiusQ12 + modelNodeRuntime->renderDepthBiasOrState;
    iVar7 = pMVar2->boundingRadiusQ12 + modelNodeRuntime->renderDepthBiasOrState;
    sVar4 = FixedVec3_DotQ28(g_FrustumPlaneNormalFixed_0,
                             (GraphicsFixedVec3 *)&g_ModelCullViewRelativeX);
    if (sVar4 <= iVar6) {
      if (sVar4 <= iVar7) {
        sVar4 = FixedVec3_DotQ28(g_FrustumPlaneNormalFixed_0 + 1,
                                 (GraphicsFixedVec3 *)&g_ModelCullViewRelativeX);
        if (iVar6 < sVar4) {
          return;
        }
        if (sVar4 <= iVar7) {
          sVar4 = FixedVec3_DotQ28(g_FrustumPlaneNormalFixed_0 + 2,
                                   (GraphicsFixedVec3 *)&g_ModelCullViewRelativeX);
          if (iVar6 < sVar4) {
            return;
          }
          if (sVar4 <= iVar7) {
            sVar4 = FixedVec3_DotQ28(g_FrustumPlaneNormalFixed_0 + 3,
                                     (GraphicsFixedVec3 *)&g_ModelCullViewRelativeX);
            if (iVar6 < sVar4) {
              return;
            }
            if (sVar4 <= iVar7) {
              dVar5 = FixedMath_Length3(g_ModelCullViewRelativeZ,g_ModelCullViewRelativeY,
                                        g_ModelCullViewRelativeX);
              uVar3 = pMVar2->boundingRadiusQ12;
              if ((int)dVar5 < (int)uVar3) {
                local_20 = 0x10000000;
              }
              else {
                local_20 = (Q12)(CONCAT44(uVar3 >> 4,uVar3 << 0x1c) / (ulonglong)dVar5);
              }
              FixedTransform_ApplyPoint
                        ((GraphicsFixedVec3 *)&g_ModelCullViewRelativeX,
                         &(modelNodeRuntime->worldTransform).translation,
                         &g_ViewProjectionMatrixFixed);
              pMVar2 = (modelNodeRuntime->modelPayload).modelResource;
              if ((int)g_ModelCullViewRelativeZ <= (int)g_ProjectionScaleFixed) {
                return;
              }
              pQVar1 = &pMVar2->boundingRadiusQ12;
              if (g_ModelCullViewRelativeZ - g_ProjectionScaleFixed != *pQVar1 &&
                  *pQVar1 <= (int)(g_ModelCullViewRelativeZ - g_ProjectionScaleFixed)) {
                modelNodeRuntime->runtimeFlags = modelNodeRuntime->runtimeFlags | 2;
                g_GraphicsShadingNearbyRecordCount = 0;
                GraphicsShadingRuntime_CollectNearbyRecords
                          (pMVar2->boundingRadiusQ12,g_ModelCullViewRelativeZ,
                           g_ModelCullViewRelativeY,g_ModelCullViewRelativeX);
                pMVar2 = (modelNodeRuntime->modelPayload).modelResource;
                uVar3 = pMVar2->meshGroupCount;
                meshGroup = &pMVar2->firstMeshGroupRelativeOffset;
                if (((((uint)g_ModelLodDepthThresholdQ8 < g_ModelCullViewRelativeZ) && (1 < uVar3))
                    && (meshGroup = (ModelMeshGroupRelativeOffset *)((int)meshGroup + *meshGroup),
                       (uint)g_ModelLodDepthThresholdQ8 < (uint)((int)g_ModelCullViewRelativeZ >> 1)
                       )) && (2 < uVar3)) {
                  meshGroup = (ModelMeshGroupRelativeOffset *)((int)meshGroup + *meshGroup);
                }
                ModelRender_DrawMeshGroupsWithTemporaryTransform
                          (local_20,(ModelMeshGroupAddress32)meshGroup,modelNodeRuntime);
              }
            }
          }
        }
      }
      dVar5 = modelNodeRuntime->childCount;
      if (dVar5 != 0) {
        iVar6 = 0;
        do {
          if (modelNodeRuntime->childNodes[iVar6] != (ModelRuntimeNode *)0x0) {
            ModelRuntime_CullAndRenderHierarchyRecursive(modelNodeRuntime->childNodes[iVar6]);
          }
          iVar6 = iVar6 + 1;
          dVar5 = dVar5 - 1;
        } while (dVar5 != 0);
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
  ModelResourceHitTestAndRenderView210 *pMVar1;
  dword dVar2;
  int iVar3;
  
  pMVar1 = (modelNode->modelPayload).modelResource;
  if (modelNode != (ModelRuntimeNode *)0x0) {
    modelNode->runtimeFlags = modelNode->runtimeFlags | 2;
    iRam004bcf50 = pMVar1->localBoundsX0Q12 + pMVar1->localBoundsX1Q12 >> 1;
    iRam004bcf54 = pMVar1->localBoundsY0Q12 + pMVar1->localBoundsY1Q12 >> 1;
    iRam004bcf58 = pMVar1->localBoundsZ0Q12 + pMVar1->localBoundsZ1Q12 >> 1;
    FixedTransform_ApplyPoint
              ((GraphicsFixedVec3 *)&g_ModelCullViewRelativeX,(GraphicsFixedVec3 *)0x4bcf50,
               &g_ViewProjectionMatrixFixed);
    dVar2 = FixedMath_Length3(pMVar1->localBoundsZ1Q12 - pMVar1->localBoundsZ0Q12,
                              pMVar1->localBoundsY1Q12 - pMVar1->localBoundsY0Q12,
                              pMVar1->localBoundsX1Q12 - pMVar1->localBoundsX0Q12);
    GraphicsShadingRuntime_CollectNearbyRecords
              ((int)dVar2 >> 1,g_ModelCullViewRelativeZ,g_ModelCullViewRelativeY,
               g_ModelCullViewRelativeX);
    ModelRender_DrawMeshGroupsAlternatePath(modelNode->runtimeStateA0,modelNode);
    iVar3 = 0;
    for (dVar2 = modelNode->childCount; dVar2 != 0; dVar2 = dVar2 - 1) {
      if (modelNode->childNodes[iVar3] != (ModelRuntimeNode *)0x0) {
        ModelRuntime_RenderHierarchyRecursiveAlternatePath(modelNode->childNodes[iVar3]);
      }
      iVar3 = iVar3 + 1;
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
ModelRaycastNearestHitEaxEdxCf9 __thandor_eax_edx_cf_preserve_ecx
ModelRuntime_RaycastCandidateListNearestCf
          (AngleTurn32 elevationAngle,AngleTurn32 azimuthAngle,Q12 maximumDistanceQ12,Q12 originZQ12
          ,Q12 originYQ12,Q12 originXQ12,ModelRuntimeClassId requiredOwnerId,
          ModelRuntimeNode *excludedNode,WorldRuntimeContext *worldRuntime)

{
  ModelRuntimeNode *modelNodeRuntime;
  DepthBinMask32 secondMaskHigh;
  DepthBinMask32 secondMaskLow;
  DepthIntervalCenter32 centerDepth;
  int iVar1;
  ModelRuntimeNode *nearestModelNode;
  bool bVar2;
  ModelRaycastNearestHitEaxEdxCf9 MVar3;
  
  g_ModelRaycastOriginX = originXQ12;
  g_ModelRaycastOriginY = originYQ12;
  g_ModelRaycastOriginZ = originZQ12;
  g_ModelRaycastMaximumDistance = maximumDistanceQ12;
  secondMaskHigh = DepthInterval_BuildBinMask(maximumDistanceQ12,originXQ12);
  secondMaskLow = DepthInterval_BuildBinMask(maximumDistanceQ12,originYQ12);
  FixedMath_WriteDirectionQ28
            ((GraphicsFixedVec3 *)&g_ModelRaycastWorldDirectionXQ28,elevationAngle,azimuthAngle);
  nearestModelNode = (ModelRuntimeNode *)0x0;
  iVar1 = 0x7fffffff;
  for (modelNodeRuntime = (ModelRuntimeNode *)worldRuntime->ownerListHead;
      modelNodeRuntime != (ModelRuntimeNode *)0x0;
      modelNodeRuntime = (ModelRuntimeNode *)(modelNodeRuntime->common).nextNode) {
    if ((((modelNodeRuntime != excludedNode) && (modelNodeRuntime->ownerClassId == requiredOwnerId))
        && ((modelNodeRuntime->runtimeFlags & 0x2000) == 0)) &&
       (bVar2 = DepthBinMasks_OverlapCf
                          (modelNodeRuntime->depthBinMaskFar,modelNodeRuntime->depthBinMaskNear,
                           secondMaskLow,secondMaskHigh), bVar2)) {
      MVar3 = ModelNodeRuntime_RaycastHierarchyNearestCf(modelNodeRuntime);
      if (MVar3.nearestDistanceQ12 <= iVar1) {
        iVar1 = MVar3.nearestDistanceQ12;
        nearestModelNode = MVar3.edxCarrier.nearestModelNode;
      }
    }
  }
  MVar3.edxCarrier.nearestModelNode = nearestModelNode;
  MVar3.nearestDistanceQ12 = iVar1;
  MVar3.carry = iVar1 != 0x7fffffff;
  return MVar3;
}


/* Address: 0x0051C240.
   Ownership: world/model/runtime.
   Purpose: EXACT_SCALAR_TWIN_OF_MODEL_SCALE_RATIO_REGISTER_WRAPPER.
   Cross-module calls: ModelRuntimeHierarchy_ComputeScaleRatioQ12Regs [world/model/hierarchy].
*/
Q12 __thandor_eax_preserve_ecx_edx
ModelRuntime_QueryHierarchyScaleRatioQ12(RuntimeModelFactionPrefix10 *runtimeEntry)

{
  ModelRuntimeScaleRatioRegisterPairQ12 MVar1;
  
  MVar1 = ModelRuntimeHierarchy_ComputeScaleRatioQ12Regs(runtimeEntry->modelRuntime);
  return (Q12)MVar1;
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
StatusValueEaxCf5 __cdecl ModelRuntimePool_Init(void)

{
  ModelRuntimeSlot *modelRuntimeStorageCursor;
  int allocationDwordsRemaining;
  ArenaAllocEaxCf5 AVar1;
  StatusValueEaxCf5 SVar2;
  
  AVar1 = (*g_MemoryApi.alloc)(0x400000);
  modelRuntimeStorageCursor = (ModelRuntimeSlot *)AVar1.eax;
  if (!AVar1.carry) {
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
    AVar1.eax = 0;
    AVar1.carry = false;
  }
  SVar2.valueOrError = AVar1.eax;
  SVar2.carry = AVar1.carry;
  return SVar2;
}


/* Address: 0x00528A70.
   Ownership: world/model/runtime.
   Purpose: Frees the model runtime pool and releases registered definition resources. The function returns no
   semantic value.
   Cross-module calls: Resource_Release [assets/resource/runtime].
*/
void __thandor_void_preserve_eax_ecx_edx ModelRuntimePool_ShutdownAndReleaseDefinitions(void)

{
  int iVar1;
  int iVar2;
  int iVar3;
  ModelDefinitionRecordPrefix **ppMVar4;
  dword dVar5;
  
  (*g_MemoryApi.free)(g_ModelRuntimeSlots);
  g_ModelRuntimeSlots = (ModelRuntimeSlot *)0x0;
  ppMVar4 = g_ModelDefinitionRegistry;
  iVar2 = 0x300;
  while( true ) {
    if ((*ppMVar4 != (ModelDefinitionRecordPrefix *)0x0) &&
       (dVar5 = (*ppMVar4)[8].flags, dVar5 != 0)) break;
ModelRuntimePool_Shutdown_ClearDefinitionEntryAndAdvance:
    *ppMVar4 = (ModelDefinitionRecordPrefix *)0x0;
    ppMVar4 = ppMVar4 + 1;
    iVar2 = iVar2 + -1;
    if (iVar2 == 0) {
      return;
    }
  }
  iVar3 = 0;
  do {
    iVar1 = *(int *)(dVar5 + 0x14);
    if (((*(uint *)(dVar5 + 4) & 0xf) == 0) && (*(int *)(dVar5 + 0x34) != 0)) {
      Resource_Release(*(void **)(dVar5 + 0x30));
    }
    iVar3 = iVar3 + 1;
    while( true ) {
      if (iVar1 != 0) break;
      iVar3 = iVar3 + -1;
      if (iVar3 == 0) goto ModelRuntimePool_Shutdown_ClearDefinitionEntryAndAdvance;
    }
    dVar5 = *(dword *)(dVar5 + 0x18);
  } while( true );
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
  ModelRuntimeSlotSerializedScalarView200 *pMVar1;
  ModelRuntimePoolRelativeOffset MVar1;
  dword dVar2;
  int iVar3;
  int iVar4;
  ModelNodePoolRelativeOffset MVar5;
  ModelRuntimeSlotUnrebaseSemanticView200 *pMVar6;
  ModelRuntimeSlotUnrebaseSemanticView200 *modelRuntime;
  
  iVar4 = 0x2000;
  modelRuntime = (ModelRuntimeSlotUnrebaseSemanticView200 *)g_ModelRuntimeSlots;
  do {
    while( true ) {
      if (modelRuntime->rootModelNodeSavedOffset != 0) break;
      for (iVar3 = 0x80; iVar3 != 0; iVar3 = iVar3 + -1) {
        (modelRuntime->definitionReferenceOrSavedId).definition = (ModelDefinitionRecordPrefix *)0x0
        ;
        modelRuntime = (ModelRuntimeSlotUnrebaseSemanticView200 *)
                       &modelRuntime->rootModelNodeSavedOffset;
      }
      iVar4 = iVar4 + -1;
      if (iVar4 == 0) {
        return;
      }
    }
    dVar2 = modelRuntime->ownerArmyRuntimeSavedOffset - (int)g_ArmyRuntimeRebaseBaseMinusOne;
    modelRuntime->rootModelNodeSavedOffset =
         modelRuntime->rootModelNodeSavedOffset - (int)g_RuntimeObjectRebaseBaseMinusOne;
    modelRuntime->ownerArmyRuntimeSavedOffset = dVar2;
    pMVar1 = (ModelRuntimeSlotSerializedScalarView200 *)modelRuntime->linkedModelRuntimeSavedOffset;
    dVar2 = (modelRuntime->classState).linkedArmyRuntimeSavedOffset;
    if (pMVar1 != (ModelRuntimeSlotSerializedScalarView200 *)0x0) {
      pMVar1 = (ModelRuntimeSlotSerializedScalarView200 *)((int)pMVar1 - g_ModelRuntimeRebaseDelta);
    }
    if (dVar2 != 0) {
      dVar2 = dVar2 - (int)g_ArmyRuntimeRebaseBaseMinusOne;
    }
    modelRuntime->linkedModelRuntimeSavedOffset = (dword)pMVar1;
    (modelRuntime->classState).linkedArmyRuntimeSavedOffset = dVar2;
    dVar2 = (modelRuntime->definitionReferenceOrSavedId).definition[6].flags;
    modelRuntime->definitionReferenceOrSavedId =
         (ModelDefinitionReferenceOrSavedId4)
         ((modelRuntime->definitionReferenceOrSavedId).definition)->definitionId;
    (*g_ArmyRuntimeOrderHandlerMatrix11Columns24Classes.modelUnrebase[dVar2])
              ((ModelRuntimeSlot *)modelRuntime);
    pMVar6 = modelRuntime;
    for (dVar2 = modelRuntime->attachmentCount0C; dVar2 != 0; dVar2 = dVar2 - 1) {
      MVar1 = pMVar6->attachments140[0].childModelRuntimeSavedOffset00;
      MVar5 = pMVar6->attachments140[0].parentModelNodeSavedOffset08;
      if (MVar1 != 0) {
        MVar1 = MVar1 - g_ModelRuntimeRebaseDelta;
      }
      if (MVar5 != 0) {
        MVar5 = MVar5 - (int)g_RuntimeObjectRebaseBaseMinusOne;
      }
      pMVar6->attachments140[0].childModelRuntimeSavedOffset00 = MVar1;
      pMVar6->attachments140[0].parentModelNodeSavedOffset08 = MVar5;
      pMVar6 = (ModelRuntimeSlotUnrebaseSemanticView200 *)(pMVar6->reserved10_37 + 0x10);
    }
    modelRuntime = modelRuntime + 1;
    iVar4 = iVar4 + -1;
  } while (iVar4 != 0);
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
  ModelRuntimeSlot *modelSlot3;
  ModelRuntimeSlot *modelSlot2;
  ArmyRuntimeSlot *armySlot2;
  ArmyRuntimeSlot *armySlot1;
  int iVar1;
  dword dVar2;
  int iVar3;
  ModelRuntimeNode *modelNode2;
  ModelDefinitionRecordPrefix **ppMVar4;
  ModelRuntimeSlot *modelRuntime;
  ModelDefinitionRecordPrefix *modelDefinition1;
  ModelRuntimeNode *modelNode1;
  ModelRuntimeSlot *modelSlot1;
  
  iVar3 = 0x2000;
  modelRuntime = g_ModelRuntimeSlots;
  do {
    if ((modelRuntime->rootModelNodeOrSavedOffset).modelNode != (ModelRuntimeNode *)0x0) {
      armySlot2 = (ArmyRuntimeSlot *)
                  ((int)&((modelRuntime->ownerArmyRuntimeOrSavedOffset).armyRuntime)->
                         modelRuntimeOrSavedOffset + (int)g_ArmyRuntimeRebaseBaseMinusOne);
      (modelRuntime->rootModelNodeOrSavedOffset).modelNode =
           (ModelRuntimeNode *)
           (g_RuntimeObjectRebaseBaseMinusOne +
           (int)(&((modelRuntime->rootModelNodeOrSavedOffset).modelNode)->modelPayload + -1) + 0x30)
      ;
      (modelRuntime->ownerArmyRuntimeOrSavedOffset).armyRuntime = armySlot2;
      armySlot2 = (modelRuntime->classState).linkedArmyRuntimeOrSavedOffset.armyRuntime;
      modelSlot3 = (ModelRuntimeSlot *)0x0;
      if ((modelRuntime->linkedModelRuntimeOrSavedOffset).modelRuntime != (ModelRuntimeSlot *)0x0) {
        modelSlot3 = (ModelRuntimeSlot *)
                     (((modelRuntime->linkedModelRuntimeOrSavedOffset).modelRuntime)->reserved10_37
                     + g_ModelRuntimeRebaseDelta + -0x10);
      }
      armySlot1 = (ArmyRuntimeSlot *)0x0;
      if (armySlot2 != (ArmyRuntimeSlot *)0x0) {
        armySlot1 = (ArmyRuntimeSlot *)
                    ((int)&armySlot2->modelRuntimeOrSavedOffset +
                    (int)g_ArmyRuntimeRebaseBaseMinusOne);
      }
      (modelRuntime->linkedModelRuntimeOrSavedOffset).modelRuntime = modelSlot3;
      (modelRuntime->classState).linkedArmyRuntimeOrSavedOffset.armyRuntime = armySlot1;
      ppMVar4 = g_ModelDefinitionRegistry;
      iVar1 = 0x300;
      do {
        modelDefinition1 = *ppMVar4;
        if ((modelDefinition1 != (ModelDefinitionRecordPrefix *)0x0) &&
           ((modelRuntime->definitionOrSavedId).definition ==
            (ModelDefinitionRecordPrefix *)modelDefinition1->definitionId)) {
          dVar2 = modelDefinition1[6].flags;
          (modelRuntime->definitionOrSavedId).definition = modelDefinition1;
          (*g_ArmyRuntimeOrderHandlerMatrix11Columns24Classes.modelRebaseOrLoadRepair[dVar2])
                    (modelRuntime);
          dVar2 = modelRuntime->attachmentCount0C;
          modelSlot3 = modelRuntime;
          if (dVar2 != 0) {
            do {
              modelSlot1 = modelSlot3->attachments140[0].childModelRuntimeOrSavedOffset00;
              modelNode1 = modelSlot3->attachments140[0].parentModelNodeOrSavedOffset08;
              modelSlot2 = (ModelRuntimeSlot *)0x0;
              if (modelSlot1 != (ModelRuntimeSlot *)0x0) {
                modelSlot2 = (ModelRuntimeSlot *)
                             (modelSlot1->reserved10_37 + g_ModelRuntimeRebaseDelta + -0x10);
              }
              modelNode2 = (ModelRuntimeNode *)0x0;
              if (modelNode1 != (ModelRuntimeNode *)0x0) {
                modelNode2 = (ModelRuntimeNode *)
                             (g_RuntimeObjectRebaseBaseMinusOne +
                             (int)(&modelNode1->modelPayload + -1) + 0x30);
              }
              modelSlot3->attachments140[0].childModelRuntimeOrSavedOffset00 = modelSlot2;
              modelSlot3->attachments140[0].parentModelNodeOrSavedOffset08 = modelNode2;
              modelSlot3 = (ModelRuntimeSlot *)(modelSlot3->reserved10_37 + 0x10);
              dVar2 = dVar2 - 1;
            } while (dVar2 != 0);
            modelRuntime->attachmentCount0C = 0;
            ModelRuntimeHierarchy_CollectAttachmentDescriptorsRecursive
                      (modelRuntime,modelRuntime,
                       *(MdlSerializedNodeHeader38 **)
                        ((modelRuntime->definitionOrSavedId).savedIdOrOffset + 100));
          }
          goto ModelRuntimePool_RebaseAfterLoad_AdvanceAfterDefinitionResolution;
        }
        ppMVar4 = ppMVar4 + 1;
        iVar1 = iVar1 + -1;
      } while (iVar1 != 0);
      (modelRuntime->rootModelNodeOrSavedOffset).modelNode = (ModelRuntimeNode *)0x0;
    }
ModelRuntimePool_RebaseAfterLoad_AdvanceAfterDefinitionResolution:
    modelRuntime = modelRuntime + 1;
    iVar3 = iVar3 + -1;
    if (iVar3 == 0) {
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
  int *piVar1;
  ModelDefinitionRecordPrefix *modelDefinition;
  ModelRuntimeSlot *modelRuntime_00;
  ModelRuntimeNode *node;
  GameEntityRuntime *entityRuntime;
  Q12 worldYQ12;
  Q12 worldXQ12;
  AngleTurn32 orientationAngle;
  int iVar2;
  dword dVar3;
  ModelRuntimeSlot *modelSlot1;
  ModelRuntimeNode *modelNode1;
  
  modelDefinition =
       (ModelDefinitionRecordPrefix *)(modelRuntime->definitionOrSavedId).savedIdOrOffset;
  dVar3 = modelDefinition[6].flags;
  FrontendPlayerRuntime_ClearAssignmentTokenFromAll((RuntimeToken)modelRuntime);
  (*g_ArmyRuntimeOrderHandlerMatrix11Columns24Classes.modelReleaseOrCommit[dVar3])
            (modelDefinition,modelRuntime);
  modelSlot1 = modelRuntime;
  for (dVar3 = modelRuntime->attachmentCount0C; dVar3 != 0; dVar3 = dVar3 - 1) {
    modelRuntime_00 = modelSlot1->attachments140[0].childModelRuntimeOrSavedOffset00;
    if (modelRuntime_00 != (ModelRuntimeSlot *)0x0) {
      ModelRuntimePool_DestroyHierarchyAndDetach(worldRuntime,modelRuntime_00);
    }
    modelSlot1 = (ModelRuntimeSlot *)(modelSlot1->reserved10_37 + 0x10);
  }
  node = (modelRuntime->rootModelNodeOrSavedOffset).modelNode;
  entityRuntime = (GameEntityRuntime *)(modelRuntime->ownerArmyRuntimeOrSavedOffset).armyRuntime;
  worldYQ12 = (node->worldTransform).translation.x;
  worldXQ12 = (node->worldTransform).translation.y;
  orientationAngle = (node->modelPayload).worldRotationAngle2;
  modelNode1 = node->parentNode;
  WorldRuntime_ForEachNodeInOwnerListD8
            (modelRuntime,WorldRuntimeNode_ClearDetachedEntityReferencesCallback,worldRuntime);
  ModelRuntimeNode_ReleaseRecursiveAndDetachParent(node);
  (modelRuntime->rootModelNodeOrSavedOffset).modelNode = (ModelRuntimeNode *)0x0;
  if (modelNode1 == (ModelRuntimeNode *)0x0) {
    if ((entityRuntime->common).ownership.definitionOrClassRecord != (void *)0x0) {
      LOCK();
      piVar1 = (entityRuntime->common).ownership.definitionOrClassRecord;
      (entityRuntime->common).ownership.definitionOrClassRecord = (void *)0x0;
      UNLOCK();
      iVar2 = *piVar1;
      if (((piVar1[0x3b] & 0x20U) == 0) && (*(int *)(iVar2 + 0x74) != -1)) {
        ArmyRuntime_CreateInstanceFromAssetCf
                  (0,orientationAngle,worldXQ12,worldYQ12,0,*(PckArmyAssetIdCatalog *)(iVar2 + 0x74)
                   ,worldRuntime);
      }
      ArmyRuntime_DestroyInstanceAndRefreshUi(worldRuntime,entityRuntime);
    }
  }
  else {
    modelSlot1 = (modelNode1->runtimePayload).modelRuntime;
    for (dVar3 = modelSlot1->attachmentCount0C; dVar3 != 0; dVar3 = dVar3 - 1) {
      if (modelSlot1->attachments140[0].childModelRuntimeOrSavedOffset00 == modelRuntime) {
        modelSlot1->attachments140[0].childModelRuntimeOrSavedOffset00 = (ModelRuntimeSlot *)0x0;
      }
      modelSlot1 = (ModelRuntimeSlot *)(modelSlot1->reserved10_37 + 0x10);
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
  ModelLocalPointRegs12 MVar1;
  AssetRecordByteCount modelPointTableBase;
  
  ModelNodeRuntime_RebuildTransformsFromRoot(modelNodeRuntime);
  modelPointTableBase = (definitionNode->spriteAssetReference).savedId;
  localPointRecord =
       (ModelPackedPointRecord *)(modelPointTableBase + *(int *)(modelPointTableBase + 0xe4));
  for (modelPointRecordsRemaining = *(int *)(modelPointTableBase + 0xe8);
      modelPointRecordsRemaining != 0; modelPointRecordsRemaining = modelPointRecordsRemaining + -1)
  {
    if ((localPointRecord->packedLookupKey & 0xf) == 2) {
      MVar1 = ModelNodeRuntime_TransformLocalPointRegs(localPointRecord,modelNodeRuntime);
      ShotRuntimePool_CreateProjectileFromDefinition
                (shotRuntimeState14,
                 (ArmyRuntimeSlot *)
                 ((modelNodeRuntime->runtimePayload).armyRuntime)->linkedEntityRuntime,
                 targetWorldZQ12,
                 (MVar1.ecx - (modelNodeRuntime->worldTransform).translation.y) + targetWorldYQ12,
                 (MVar1.eax - (modelNodeRuntime->worldTransform).translation.x) + targetWorldXQ12,
                 MVar1.edx,MVar1.ecx,MVar1.eax,shotDefinition,worldRuntime);
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
ModelNodeCreateEaxCf5 __thandor_eax_cf_preserve_ecx_edx
ModelRuntimePool_CreateInstanceByDefinitionIdCf
          (GraphicsPaletteAsset *paletteAsset,GraphicsTextureSet *textureSet,
          ArmyRuntimeSlot *armyRuntime,PckModelDefinitionIdCatalog modelDefinitionId,
          WorldRuntimeContext *worldRuntime)

{
  dword dVar1;
  uint uVar2;
  uint uVar3;
  dword dVar4;
  dword dVar5;
  ModelRuntimeNode *modelNodeRuntime;
  ModelRuntimeSlot *modelSlot2;
  int iVar6;
  ModelDefinitionRecordPrefix **ppMVar7;
  ModelRuntimeSlot *modelRuntime;
  ModelNodeCreateEaxCf5 MVar8;
  ModelNodeCreateEaxCf5 MVar9;
  ModelDefinitionRuntimeSemanticView280 *pMVar2;
  ModelRuntimeSlot *modelSlot1;
  
  modelNodeRuntime = (ModelRuntimeNode *)0x14;
  modelSlot2 = (ModelRuntimeSlot *)0x2000;
  modelRuntime = g_ModelRuntimeSlots;
  modelSlot1 = g_ModelRuntimeSlots;
  while (modelSlot1 != (ModelRuntimeSlot *)0x0) {
    if ((modelRuntime->rootModelNodeOrSavedOffset).modelNode == (ModelRuntimeNode *)0x0) {
      ppMVar7 = g_ModelDefinitionRegistry;
      iVar6 = 0x300;
      goto ModelRuntimePool_CreateInstanceByDefinitionId_ScanDefinitionRegistry;
    }
    modelRuntime = modelRuntime + 1;
    modelSlot2 = (ModelRuntimeSlot *)((int)&modelSlot2[-1].attachments140[5].reserved1C + 3);
    modelSlot1 = modelSlot2;
  }
  goto ModelRuntimePool_CreateInstanceByDefinitionId_ReturnCreationFailure;
  while( true ) {
    ppMVar7 = ppMVar7 + 1;
    iVar6 = iVar6 + -1;
    if (iVar6 == 0) break;
ModelRuntimePool_CreateInstanceByDefinitionId_ScanDefinitionRegistry:
    pMVar2 = (ModelDefinitionRuntimeSemanticView280 *)*ppMVar7;
    if ((pMVar2 != (ModelDefinitionRuntimeSemanticView280 *)0x0) &&
       (pMVar2->definitionId == modelDefinitionId)) {
      (modelRuntime->definitionOrSavedId).definition = (ModelDefinitionRecordPrefix *)pMVar2;
      dVar1 = pMVar2->runtimeValue60;
      uVar2 = pMVar2->runtimeValue48;
      uVar3 = pMVar2->runtimeValue27C;
      (modelRuntime->rootModelNodeOrSavedOffset).modelNode = (ModelRuntimeNode *)0x0;
      (modelRuntime->ownerArmyRuntimeOrSavedOffset).armyRuntime = armyRuntime;
      modelRuntime->attachmentCount0C = 0;
      modelRuntime->definitionValue60_3C = dVar1;
      if (armyRuntime->runtimeState44 < uVar2) {
        armyRuntime->runtimeState44 = uVar2;
      }
      if (armyRuntime->runtimeState90 < uVar3) {
        armyRuntime->runtimeState90 = uVar3;
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
      dVar1 = pMVar2->runtimeValue8C;
      dVar4 = pMVar2->runtimeValue94;
      dVar5 = pMVar2->runtimeValue9C;
      modelRuntime->definitionValue84_40 = pMVar2->runtimeValue84;
      modelRuntime->definitionValue88_44 = dVar1;
      modelRuntime->definitionValue94_48 = dVar4;
      modelRuntime->definitionValue9C_4C = dVar5;
      dVar1 = pMVar2->runtimeValueAC;
      dVar4 = pMVar2->runtimeValueB4;
      dVar5 = pMVar2->runtimeValueBC;
      modelRuntime->definitionValueA4_50 = pMVar2->runtimeValueA4;
      modelRuntime->definitionValueAC_54 = dVar1;
      modelRuntime->definitionValueB4_58 = dVar4;
      modelRuntime->definitionValueBC_5C = dVar5;
      dVar1 = pMVar2->runtimeValue18C;
      (modelRuntime->classState).enabledStateE4 = 1;
      (modelRuntime->classState).enabledStateE8 = 1;
      (modelRuntime->classState).linkedArmyRuntimeOrSavedOffset.armyRuntime = (ArmyRuntimeSlot *)0x0
      ;
      (modelRuntime->classState).definitionDerivedValueF4 = dVar1;
      (modelRuntime->classState).classStateEC = 0;
      (modelRuntime->classState).classStateF8 = 0;
      (modelRuntime->classState).classStateFC = 0;
      modelRuntime->classState118 = 0;
      uVar2 = pMVar2->runtimeValue68;
      if ((MdlSerializedNodeHeader38 *)pMVar2->serializedNodeOffsetOrPointer64 !=
          (MdlSerializedNodeHeader38 *)0x0) {
        MVar9 = ModelNodeRuntime_CreateHierarchyRecursiveCf
                          (paletteAsset,textureSet,modelRuntime,
                           (MdlSerializedNodeHeader38 *)pMVar2->serializedNodeOffsetOrPointer64,
                           worldRuntime);
        modelNodeRuntime = MVar9.modelNode;
        if (MVar9.carry) goto ModelRuntimePool_CreateInstanceByDefinitionId_ReturnCreationFailure;
        (modelRuntime->rootModelNodeOrSavedOffset).modelNode = modelNodeRuntime;
        ModelNodeRuntime_RecomputeSubtreeBoundingRadius(modelNodeRuntime);
        ModelNodeRuntime_RebuildTransformsFromRoot(modelNodeRuntime);
        if ((uVar2 & 0x100) != 0) {
          modelNodeRuntime->runtimeFlags = modelNodeRuntime->runtimeFlags | 0x2000;
        }
      }
      (*g_ArmyRuntimeOrderHandlerMatrix11Columns24Classes.modelClassInitialize
        [(modelRuntime->definitionOrSavedId).definition[6].flags])
                ((modelRuntime->definitionOrSavedId).definition,modelRuntime);
      MVar9.carry = false;
      MVar9.modelNode = (ModelRuntimeNode *)modelRuntime;
      return MVar9;
    }
  }
  (*g_WideNumberFormatUtf16)
            (WIDE_FORMAT_WRITE_TERMINATOR,0,10,1,modelDefinitionId,g_PackageLastErrorPath);
  modelNodeRuntime = (ModelRuntimeNode *)0x3e;
ModelRuntimePool_CreateInstanceByDefinitionId_ReturnCreationFailure:
  MVar8.carry = true;
  MVar8.modelNode = modelNodeRuntime;
  return MVar8;
}


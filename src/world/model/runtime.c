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
void __fastcall
ModelRuntimePool_RepairDeferredChild
          (undefined4 param_1,undefined4 param_2,GraphicsPaletteAsset *paletteAsset,
          GraphicsTextureSet *textureSet,ModelRuntimeAttachmentIndex attachmentIndex,
          PckModelDefinitionIdCatalog childDefinitionId,ModelRuntimeSlot *modelRuntime,
          WorldRuntimeContext *worldRuntime)

{
  ModelAttachmentTransformRecord *pMVar1;
  AngleTurn32 AVar2;
  AngleTurn32 AVar3;
  AngleTurn32 AVar4;
  Q12 QVar5;
  Q12 QVar6;
  int extraout_ECX;
  bool bVar7;
  undefined8 uVar8;
  ModelRuntimeNode *modelNode1;
  ModelRuntimeNode *modelNode2;
  
  bVar7 = attachmentIndex < modelRuntime->attachmentCount0C;
  if (bVar7) {
    uVar8 = ModelRuntimePool_CreateInstanceByDefinitionIdCf
                      (paletteAsset,textureSet,
                       (modelRuntime->ownerArmyRuntimeOrSavedOffset).armyRuntime,childDefinitionId,
                       worldRuntime);
    if (bVar7) {
      return;
    }
    modelRuntime->attachments140[extraout_ECX].childModelRuntimeOrSavedOffset00 =
         (ModelRuntimeSlot *)uVar8;
    modelNode1 = modelRuntime->attachments140[extraout_ECX].parentModelNodeOrSavedOffset08;
    pMVar1 = modelRuntime->attachments140[extraout_ECX].sourceTransform04;
    modelNode2 = (((ModelRuntimeSlot *)uVar8)->rootModelNodeOrSavedOffset).modelNode;
    AVar2 = modelRuntime->attachments140[extraout_ECX].childLocalRotationAngle0;
    AVar3 = modelRuntime->attachments140[extraout_ECX].childLocalRotationAngle1;
    AVar4 = modelRuntime->attachments140[extraout_ECX].childLocalRotationAngle2;
    modelNode1->childNodes[modelRuntime->attachments140[extraout_ECX].childNodeIndex0C] = modelNode2
    ;
    (modelNode2->modelPayload).localRotationAngle2 = AVar4;
    (modelNode2->modelPayload).localRotationAngle1 = AVar3;
    (modelNode2->modelPayload).localRotationAngle0 = AVar2;
    modelNode2->parentNode = modelNode1;
    QVar5 = pMVar1->localTranslationXQ12;
    QVar6 = pMVar1->localTranslationYQ12;
    (modelNode2->modelPayload).localTranslationZQ12 = pMVar1->localTranslationZQ12;
    (modelNode2->modelPayload).localTranslationYQ12 = QVar6;
    (modelNode2->modelPayload).localTranslationXQ12 = QVar5;
  }
  return;
}

/* Address: 0x004BDDB0.
   Ownership: world/model/runtime.
   Purpose: Performs frustum and depth tests, chooses the model LOD, renders the accepted node, and recursively
   traverses its child hierarchy.
   Cross-module calls: FixedVec3_DotQ28 [core/math/fixed], FixedMath_Length3 [core/math/fixed],
   FixedTransform_ApplyPoint [core/math/fixed], GraphicsShadingRuntime_CollectNearbyRecords
   [graphics/render/shading], ModelRender_DrawMeshGroupsWithTemporaryTransform [graphics/render/model].
*/
undefined8 ModelRuntime_CullAndRenderHierarchyRecursive(ModelRuntimeNode *modelNodeRuntime)

{
  ModelResourceHitTestAndRenderView210 *pMVar1;
  uint uVar2;
  ModelRuntimeNode *modelNodeRuntime_00;
  undefined4 in_EAX;
  sdword sVar3;
  dword dVar4;
  int extraout_ECX;
  int extraout_ECX_00;
  int extraout_ECX_01;
  int extraout_ECX_02;
  int extraout_ECX_03;
  int modelNode;
  int extraout_ECX_04;
  int iVar5;
  int extraout_ECX_05;
  undefined4 in_EDX;
  int extraout_EDX;
  int extraout_EDX_00;
  int extraout_EDX_01;
  int extraout_EDX_02;
  int *piVar6;
  int iVar7;
  int iVar8;
  undefined8 uVar9;
  Q12 local_20;
  
  if (modelNodeRuntime != (ModelRuntimeNode *)0x0) {
    modelNodeRuntime->runtimeFlags = modelNodeRuntime->runtimeFlags & 0xfffffffd;
    g_ModelCullViewRelativeX =
         (modelNodeRuntime->worldTransform).translation.x - g_ViewOriginFixed.x;
    g_ModelCullViewRelativeY =
         (modelNodeRuntime->worldTransform).translation.y - g_ViewOriginFixed.y;
    g_ModelCullViewRelativeZ =
         (modelNodeRuntime->worldTransform).translation.z - g_ViewOriginFixed.z;
    pMVar1 = (modelNodeRuntime->modelPayload).modelResource;
    iVar7 = pMVar1->boundingRadiusQ12 + modelNodeRuntime->renderDepthBiasOrState;
    sVar3 = FixedVec3_DotQ28(g_FrustumPlaneNormalFixed_0,
                             (GraphicsFixedVec3 *)&g_ModelCullViewRelativeX);
    if (sVar3 <= extraout_EDX) {
      iVar5 = extraout_ECX;
      if (sVar3 <= iVar7) {
        sVar3 = FixedVec3_DotQ28(g_FrustumPlaneNormalFixed_0 + 1,
                                 (GraphicsFixedVec3 *)&g_ModelCullViewRelativeX);
        if (extraout_EDX_00 < sVar3)
        goto 
        ModelRuntime_CullAndRenderHierarchyRecursive_ReturnAfterCullRejectRenderOrChildTraversal;
        iVar5 = extraout_ECX_00;
        if (sVar3 <= iVar7) {
          sVar3 = FixedVec3_DotQ28(g_FrustumPlaneNormalFixed_0 + 2,
                                   (GraphicsFixedVec3 *)&g_ModelCullViewRelativeX);
          if (extraout_EDX_01 < sVar3)
          goto 
          ModelRuntime_CullAndRenderHierarchyRecursive_ReturnAfterCullRejectRenderOrChildTraversal;
          iVar5 = extraout_ECX_01;
          if (sVar3 <= iVar7) {
            sVar3 = FixedVec3_DotQ28(g_FrustumPlaneNormalFixed_0 + 3,
                                     (GraphicsFixedVec3 *)&g_ModelCullViewRelativeX);
            if (extraout_EDX_02 < sVar3)
            goto 
            ModelRuntime_CullAndRenderHierarchyRecursive_ReturnAfterCullRejectRenderOrChildTraversal
            ;
            iVar5 = extraout_ECX_02;
            if (sVar3 <= iVar7) {
              dVar4 = FixedMath_Length3(g_ModelCullViewRelativeZ,g_ModelCullViewRelativeY,
                                        g_ModelCullViewRelativeX);
              uVar2 = pMVar1->boundingRadiusQ12;
              if ((int)dVar4 < (int)uVar2) {
                local_20 = 0x10000000;
              }
              else {
                local_20 = (Q12)(CONCAT44(uVar2 >> 4,uVar2 << 0x1c) / (ulonglong)dVar4);
              }
              FixedTransform_ApplyPoint
                        ((GraphicsFixedVec3 *)&g_ModelCullViewRelativeX,
                         &(modelNodeRuntime->worldTransform).translation,
                         &g_ViewProjectionMatrixFixed);
              if ((int)g_ModelCullViewRelativeZ <= (int)g_ProjectionScaleFixed)
              goto 
              ModelRuntime_CullAndRenderHierarchyRecursive_ReturnAfterCullRejectRenderOrChildTraversal
              ;
              piVar6 = (int *)(*(int *)(extraout_ECX_03 + 0x40) + 0xd8);
              iVar5 = extraout_ECX_03;
              if (g_ModelCullViewRelativeZ - g_ProjectionScaleFixed != *piVar6 &&
                  *piVar6 <= (int)(g_ModelCullViewRelativeZ - g_ProjectionScaleFixed)) {
                *(uint *)(extraout_ECX_03 + 0x4c) = *(uint *)(extraout_ECX_03 + 0x4c) | 2;
                g_GraphicsShadingNearbyRecordCount = 0;
                GraphicsShadingRuntime_CollectNearbyRecords
                          (*(GraphicsRadiusQ12 *)(*(int *)(extraout_ECX_03 + 0x40) + 0xd8),
                           g_ModelCullViewRelativeZ,g_ModelCullViewRelativeY,
                           g_ModelCullViewRelativeX);
                uVar2 = *(uint *)(*(int *)(modelNode + 0x40) + 0xb0);
                piVar6 = (int *)(*(int *)(modelNode + 0x40) + 0x200);
                if (((((uint)g_ModelLodDepthThresholdQ8 < g_ModelCullViewRelativeZ) && (1 < uVar2))
                    && (piVar6 = (int *)((int)piVar6 + *piVar6),
                       (uint)g_ModelLodDepthThresholdQ8 < (uint)((int)g_ModelCullViewRelativeZ >> 1)
                       )) && (2 < uVar2)) {
                  piVar6 = (int *)((int)piVar6 + *piVar6);
                }
                ModelRender_DrawMeshGroupsWithTemporaryTransform
                          (modelNode,piVar6,local_20,(ModelMeshGroupAddress32)piVar6,
                           (ModelRuntimeNode *)modelNode);
                iVar5 = extraout_ECX_04;
              }
            }
          }
        }
      }
      iVar7 = *(int *)(iVar5 + 200);
      if (iVar7 != 0) {
        iVar8 = 0;
        do {
          modelNodeRuntime_00 = *(ModelRuntimeNode **)(iVar5 + 0xcc + iVar8 * 4);
          if (modelNodeRuntime_00 != (ModelRuntimeNode *)0x0) {
            uVar9 = ModelRuntime_CullAndRenderHierarchyRecursive(modelNodeRuntime_00);
            iVar7 = (int)((ulonglong)uVar9 >> 0x20);
            iVar5 = extraout_ECX_05;
          }
          iVar8 = iVar8 + 1;
          iVar7 = iVar7 + -1;
        } while (iVar7 != 0);
      }
    }
  }
ModelRuntime_CullAndRenderHierarchyRecursive_ReturnAfterCullRejectRenderOrChildTraversal:
  return CONCAT44(in_EDX,in_EAX);
}

/* Address: 0x004BE270.
   Ownership: world/model/runtime.
   Purpose: Handles model runtime render hierarchy recursive alternate path.
   Cross-module calls: FixedTransform_ApplyPoint [core/math/fixed], FixedMath_Length3 [core/math/fixed],
   GraphicsShadingRuntime_CollectNearbyRecords [graphics/render/shading], ModelRender_DrawMeshGroupsAlternatePath
   [graphics/render/model].
*/
undefined8 ModelRuntime_RenderHierarchyRecursiveAlternatePath(ModelRuntimeNode *param_1)

{
  ModelResourceHitTestAndRenderView210 *pMVar1;
  undefined4 in_EAX;
  dword dVar2;
  undefined4 extraout_ECX;
  int iVar3;
  int extraout_ECX_00;
  undefined4 in_EDX;
  undefined4 extraout_EDX;
  dword extraout_EDX_00;
  
  pMVar1 = (param_1->modelPayload).modelResource;
  if (param_1 != (ModelRuntimeNode *)0x0) {
    param_1->runtimeFlags = param_1->runtimeFlags | 2;
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
    ModelRender_DrawMeshGroupsAlternatePath
              (extraout_ECX,extraout_EDX,param_1->runtimeStateA0,param_1);
    iVar3 = 0;
    for (dVar2 = param_1->childCount; dVar2 != 0; dVar2 = dVar2 - 1) {
      if (param_1->childNodes[iVar3] != (ModelRuntimeNode *)0x0) {
        ModelRuntime_RenderHierarchyRecursiveAlternatePath(param_1->childNodes[iVar3]);
        iVar3 = extraout_ECX_00;
        dVar2 = extraout_EDX_00;
      }
      iVar3 = iVar3 + 1;
    }
  }
  return CONCAT44(in_EDX,in_EAX);
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
ModelRaycastNearestHitCfRegisterResult
ModelRuntime_RaycastCandidateListNearestCf
          (AngleTurn32 elevationAngle,AngleTurn32 azimuthAngle,Q12 maximumDistanceQ12,Q12 originZQ12
          ,Q12 originYQ12,Q12 originXQ12,ModelRuntimeClassId requiredOwnerId,
          ModelRuntimeNode *excludedNode,WorldRuntimeContext *worldRuntime)

{
  ModelRuntimeNode *modelNodeRuntime;
  ModelRaycastNearestHitCfRegisterResult MVar1;
  DepthIntervalCenter32 centerDepth;
  bool bVar2;
  DepthBinMaskEaxPreservedEdxCarrier64 DVar3;
  DepthBinMaskEaxPreservedEdxCarrier64 DVar4;
  ModelRaycastNearestHitCfRegisterResult MVar5;
  
  g_ModelRaycastOriginX = originXQ12;
  g_ModelRaycastOriginY = originYQ12;
  g_ModelRaycastOriginZ = originZQ12;
  g_ModelRaycastMaximumDistance = maximumDistanceQ12;
  DVar3 = DepthInterval_BuildBinMask(maximumDistanceQ12,originXQ12);
  DVar4 = DepthInterval_BuildBinMask(maximumDistanceQ12,centerDepth);
  FixedMath_WriteDirectionQ28
            ((GraphicsFixedVec3 *)&g_ModelRaycastWorldDirectionXQ28,elevationAngle,azimuthAngle);
  MVar1.nearestDistanceQ12 = 0x7fffffff;
  MVar1.nearestModelNode = (ModelRuntimeNode *)0x0;
  for (modelNodeRuntime = (ModelRuntimeNode *)worldRuntime->ownerListHead;
      modelNodeRuntime != (ModelRuntimeNode *)0x0;
      modelNodeRuntime = (ModelRuntimeNode *)(modelNodeRuntime->common).nextNode) {
    if ((((modelNodeRuntime != excludedNode) && (modelNodeRuntime->ownerClassId == requiredOwnerId))
        && (bVar2 = false, (modelNodeRuntime->runtimeFlags & 0x2000) == 0)) &&
       ((DepthBinMasks_OverlapCf
                   (modelNodeRuntime->depthBinMaskFar,modelNodeRuntime->depthBinMaskNear,
                    (DepthBinMask32)DVar4,(DepthBinMask32)DVar3), bVar2 &&
        (MVar5 = ModelNodeRuntime_RaycastHierarchyNearestCf(modelNodeRuntime),
        MVar5.nearestDistanceQ12 <= MVar1.nearestDistanceQ12)))) {
      MVar1 = MVar5;
    }
  }
  return MVar1;
}

/* Address: 0x0051C240.
   Ownership: world/model/runtime.
   Purpose: EXACT_SCALAR_TWIN_OF_MODEL_SCALE_RATIO_REGISTER_WRAPPER.
   Cross-module calls: ModelRuntimeHierarchy_ComputeScaleRatioQ12Regs [world/model/hierarchy].
*/
undefined8 __fastcall
ModelRuntime_QueryHierarchyScaleRatioQ12(undefined4 param_1,undefined4 param_2,undefined4 *param_3)

{
  ModelRuntimeScaleRatioRegisterPairQ12 MVar1;
  
  MVar1 = ModelRuntimeHierarchy_ComputeScaleRatioQ12Regs((ModelRuntimeSlot *)*param_3);
  return CONCAT44(param_2,(int)MVar1);
}

/* Address: 0x0051C260.
   Ownership: world/model/runtime.
   Purpose: Queries the root model runtime hierarchy for the recursive Q12 scale-ratio result returned by the
   shared hierarchy metric routine. Queries the linked ModelRuntimeSlot and preserves the verified scale-ratio
   register pair returned in EDX:EAX.
   Cross-module calls: ModelRuntimeHierarchy_ComputeScaleRatioQ12Regs [world/model/hierarchy].
*/
ModelRuntimeScaleRatioRegisterPairQ12
ModelRuntime_QueryHierarchyScaleRatioQ12Regs(ArmyRuntimeSlot *modelRuntimeHolder)

{
  ModelRuntimeScaleRatioRegisterPairQ12 hierarchyScaleRatioPairQ12;
  
  hierarchyScaleRatioPairQ12 =
       ModelRuntimeHierarchy_ComputeScaleRatioQ12Regs(modelRuntimeHolder->definitionOrAsset);
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
int ModelRuntime_QueryActiveHierarchyMetric(ArmyRuntimeSlot *modelRuntimeHolder)

{
  ModelRuntimeActiveTotalMetricRegisterPair activeHierarchyMetricPair;
  
  activeHierarchyMetricPair =
       ModelRuntimeHierarchy_ComputeActiveAndTotalMetricsRegs(modelRuntimeHolder->definitionOrAsset)
  ;
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
ModelRuntime_QueryActiveAndTotalHierarchyMetricsRegs(ArmyRuntimeSlot *modelRuntimeHolder)

{
  ModelRuntimeActiveTotalMetricRegisterPair activeTotalMetrics;
  
  activeTotalMetrics =
       ModelRuntimeHierarchy_ComputeActiveAndTotalMetricsRegs(modelRuntimeHolder->definitionOrAsset)
  ;
  return activeTotalMetrics;
}

/* Address: 0x00528A40.
   Ownership: world/model/runtime.
   Purpose: Allocates and zeroes the exact 0x400000-byte model runtime pool, equal to 8192 ModelRuntimeSlot
   records.
*/
void __cdecl ModelRuntimePool_Init(void)

{
  ModelRuntimeSlot *modelRuntimeStorageCursor;
  int allocationDwordsRemaining;
  undefined1 in_CF;
  
  modelRuntimeStorageCursor = (*g_MemoryApi.alloc)(0x400000);
  if (!(bool)in_CF) {
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
  }
  return;
}

/* Address: 0x00528A70.
   Ownership: world/model/runtime.
   Purpose: Frees the model runtime pool and releases registered definition resources. The function returns no
   semantic value.
   Cross-module calls: Resource_Release [assets/resource/runtime].
*/
void __cdecl ModelRuntimePool_ShutdownAndReleaseDefinitions(void)

{
  int iVar1;
  int extraout_ECX;
  int iVar2;
  int iVar3;
  int extraout_EDX;
  int iVar4;
  ModelDefinitionRecordPrefix **ppMVar5;
  dword dVar6;
  
  (*g_MemoryApi.free)(g_ModelRuntimeSlots);
  g_ModelRuntimeSlots = (ModelRuntimeSlot *)0x0;
  ppMVar5 = g_ModelDefinitionRegistry;
  iVar1 = 0x300;
  while( true ) {
    if ((*ppMVar5 != (ModelDefinitionRecordPrefix *)0x0) &&
       (dVar6 = (*ppMVar5)[8].flags, dVar6 != 0)) break;
ModelRuntimePool_Shutdown_ClearDefinitionEntryAndAdvance:
    *ppMVar5 = (ModelDefinitionRecordPrefix *)0x0;
    ppMVar5 = ppMVar5 + 1;
    iVar1 = iVar1 + -1;
    if (iVar1 == 0) {
      return;
    }
  }
  iVar4 = 0;
  do {
    iVar2 = *(int *)(dVar6 + 0x14);
    iVar3 = 0;
    if (((*(uint *)(dVar6 + 4) & 0xf) == 0) && (*(int *)(dVar6 + 0x34) != 0)) {
      Resource_Release(*(void **)(dVar6 + 0x30));
      iVar2 = extraout_ECX;
      iVar3 = extraout_EDX;
    }
    iVar4 = iVar4 + 1;
    while( true ) {
      if (iVar2 != 0) break;
      iVar4 = iVar4 + -1;
      if (iVar4 == 0) goto ModelRuntimePool_Shutdown_ClearDefinitionEntryAndAdvance;
    }
    dVar6 = *(dword *)(dVar6 + 0x18 + iVar3 * 4);
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
  int extraout_EDX;
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
    iVar4 = extraout_EDX + -1;
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
void __cdecl ModelRuntimePool_RebaseAfterLoad(void)

{
  ModelRuntimeSlot *modelSlot3;
  ModelRuntimeSlot *modelSlot2;
  ArmyRuntimeSlot *armySlot2;
  ArmyRuntimeSlot *armySlot1;
  int iVar1;
  dword dVar2;
  int iVar3;
  int extraout_EDX;
  ModelRuntimeNode *modelNode2;
  ModelDefinitionRecordPrefix **ppMVar4;
  ModelRuntimeSlot *modelRuntime;
  ModelRuntimeAttachmentCollectionRegisterPair MVar5;
  ModelDefinitionRecordPrefix *modelDefinition1;
  ModelRuntimeNode *modelNode1;
  ModelRuntimeSlot *modelSlot1;
  
  iVar3 = 0x2000;
  modelRuntime = g_ModelRuntimeSlots;
  do {
    if ((modelRuntime->rootModelNodeOrSavedOffset).modelNode != (ModelRuntimeNode *)0x0) {
      armySlot2 = (ArmyRuntimeSlot *)
                  ((int)&((modelRuntime->ownerArmyRuntimeOrSavedOffset).armyRuntime)->
                         definitionOrAsset + (int)g_ArmyRuntimeRebaseBaseMinusOne);
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
                    ((int)&armySlot2->definitionOrAsset + (int)g_ArmyRuntimeRebaseBaseMinusOne);
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
          iVar3 = extraout_EDX;
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
            MVar5 = ModelRuntimeHierarchy_CollectAttachmentDescriptorsRecursive
                              (modelRuntime,
                               *(MdlSerializedNodeHeader38 **)
                                ((modelRuntime->definitionOrSavedId).savedIdOrOffset + 100));
            iVar3 = (int)(MVar5 >> 0x20);
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
void ModelRuntimePool_DestroyHierarchyAndDetach
               (WorldRuntimeContext *worldRuntime,ModelRuntimeSlot *modelRuntime)

{
  int *piVar1;
  ModelRuntimeSlot *modelRuntime_00;
  ModelRuntimeNode *node;
  Q12 worldYQ12;
  Q12 worldXQ12;
  AngleTurn32 orientationAngle;
  int iVar2;
  dword extraout_ECX;
  dword dVar3;
  GameEntityRuntime *entityRuntime;
  GameEntityRuntime *extraout_EDX;
  ModelRuntimeSlot *modelSlot1;
  undefined8 uVar4;
  ModelRuntimeNode *modelNode1;
  
  uVar4 = FrontendPlayerRuntime_ClearAssignmentTokenFromAll((RuntimeToken)modelRuntime);
  (*g_ArmyRuntimeOrderHandlerMatrix11Columns24Classes.modelReleaseOrCommit[(int)uVar4])
            ((ModelDefinitionRecordPrefix *)((ulonglong)uVar4 >> 0x20),modelRuntime);
  modelSlot1 = modelRuntime;
  for (dVar3 = modelRuntime->attachmentCount0C; dVar3 != 0; dVar3 = dVar3 - 1) {
    modelRuntime_00 = modelSlot1->attachments140[0].childModelRuntimeOrSavedOffset00;
    if (modelRuntime_00 != (ModelRuntimeSlot *)0x0) {
      ModelRuntimePool_DestroyHierarchyAndDetach(worldRuntime,modelRuntime_00);
      dVar3 = extraout_ECX;
    }
    modelSlot1 = (ModelRuntimeSlot *)(modelSlot1->reserved10_37 + 0x10);
  }
  node = (modelRuntime->rootModelNodeOrSavedOffset).modelNode;
  worldYQ12 = (node->worldTransform).translation.x;
  worldXQ12 = (node->worldTransform).translation.y;
  orientationAngle = (node->modelPayload).worldRotationAngle2;
  modelNode1 = node->parentNode;
  WorldRuntime_ForEachNodeInOwnerListD8
            (modelRuntime,WorldRuntimeNode_ClearDetachedEntityReferencesCallback,worldRuntime);
  uVar4 = ModelRuntimeNode_ReleaseRecursiveAndDetachParent(node);
  entityRuntime = (GameEntityRuntime *)((ulonglong)uVar4 >> 0x20);
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
        entityRuntime = extraout_EDX;
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
void ModelRuntime_EmitProjectilesFromAttachmentPoints
               (ShotRuntimeState14 shotRuntimeState14,Q12 targetWorldZQ12,Q12 targetWorldYQ12,
               Q12 targetWorldXQ12,ShotDefinition *shotDefinition,ModelRuntimeNode *modelNodeRuntime
               ,MdlSerializedNodeHeader38 *definitionNode,WorldRuntimeContext *worldRuntime)

{
  undefined4 in_ECX;
  undefined4 extraout_ECX;
  int launchWorldYQ12;
  undefined4 extraout_ECX_00;
  undefined4 uVar1;
  undefined4 in_EDX;
  int modelPointRecordsRemaining;
  ModelPackedPointRecord *localPointRecord;
  WorldPositionXYRegisterPairQ12 attachmentWorldPointPairQ12;
  AssetRecordByteCount modelPointTableBase;
  
  ModelNodeRuntime_RebuildTransformsFromRoot(in_ECX,in_EDX,modelNodeRuntime);
  modelPointTableBase = (definitionNode->spriteAssetReference).savedId;
  localPointRecord =
       (ModelPackedPointRecord *)(modelPointTableBase + *(int *)(modelPointTableBase + 0xe4));
  uVar1 = extraout_ECX;
  for (modelPointRecordsRemaining = *(int *)(modelPointTableBase + 0xe8);
      modelPointRecordsRemaining != 0; modelPointRecordsRemaining = modelPointRecordsRemaining + -1)
  {
    if ((localPointRecord->packedLookupKey & 0xf) == 2) {
      attachmentWorldPointPairQ12 =
           ModelNodeRuntime_TransformLocalPointRegs(uVar1,localPointRecord,modelNodeRuntime);
      ShotRuntimePool_CreateProjectileFromDefinition
                (shotRuntimeState14,
                 (ArmyRuntimeSlot *)
                 ((modelNodeRuntime->runtimePayload).armyRuntime)->linkedEntityRuntime,
                 targetWorldZQ12,
                 (launchWorldYQ12 - (modelNodeRuntime->worldTransform).translation.y) +
                 targetWorldYQ12,
                 ((int)attachmentWorldPointPairQ12 -
                 (modelNodeRuntime->worldTransform).translation.x) + targetWorldXQ12,
                 (Q12)(attachmentWorldPointPairQ12 >> 0x20),launchWorldYQ12,
                 (int)attachmentWorldPointPairQ12,shotDefinition,worldRuntime);
      uVar1 = extraout_ECX_00;
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
undefined8
ModelRuntimePool_CreateInstanceByDefinitionIdCf
          (GraphicsPaletteAsset *paletteAsset,GraphicsTextureSet *textureSet,
          ArmyRuntimeSlot *armyRuntime,PckModelDefinitionIdCatalog modelDefinitionId,
          WorldRuntimeContext *worldRuntime)

{
  uint *puVar1;
  dword dVar2;
  uint uVar3;
  uint uVar4;
  dword dVar5;
  dword dVar6;
  ArmyRuntimeReferenceOrSavedOffset4 AVar7;
  ModelRuntimeNode *modelNodeRuntime;
  ModelRuntimeSlot *modelSlot2;
  undefined4 extraout_ECX;
  undefined4 in_EDX;
  int iVar8;
  ModelDefinitionRecordPrefix **ppMVar9;
  ModelRuntimeSlot *modelRuntime;
  bool bVar10;
  longlong lVar11;
  undefined8 uVar12;
  ulonglong uVar13;
  ModelDefinitionRuntimeSemanticView280 *pMVar2;
  ModelRuntimeSlot *modelSlot1;
  
  modelNodeRuntime = (ModelRuntimeNode *)0x14;
  modelSlot2 = (ModelRuntimeSlot *)0x2000;
  modelRuntime = g_ModelRuntimeSlots;
  modelSlot1 = g_ModelRuntimeSlots;
  while (modelSlot1 != (ModelRuntimeSlot *)0x0) {
    if ((modelRuntime->rootModelNodeOrSavedOffset).modelNode == (ModelRuntimeNode *)0x0) {
      ppMVar9 = g_ModelDefinitionRegistry;
      iVar8 = 0x300;
      goto ModelRuntimePool_CreateInstanceByDefinitionId_ScanDefinitionRegistry;
    }
    modelRuntime = modelRuntime + 1;
    modelSlot2 = (ModelRuntimeSlot *)((int)&modelSlot2[-1].attachments140[5].reserved1C + 3);
    modelSlot1 = modelSlot2;
  }
  goto ModelRuntimePool_CreateInstanceByDefinitionId_ReturnCreationFailure;
  while( true ) {
    ppMVar9 = ppMVar9 + 1;
    iVar8 = iVar8 + -1;
    if (iVar8 == 0) break;
ModelRuntimePool_CreateInstanceByDefinitionId_ScanDefinitionRegistry:
    pMVar2 = (ModelDefinitionRuntimeSemanticView280 *)*ppMVar9;
    if ((pMVar2 != (ModelDefinitionRuntimeSemanticView280 *)0x0) &&
       (pMVar2->definitionId == modelDefinitionId)) {
      (modelRuntime->definitionOrSavedId).definition = (ModelDefinitionRecordPrefix *)pMVar2;
      dVar2 = pMVar2->runtimeValue60;
      uVar3 = pMVar2->runtimeValue48;
      uVar4 = pMVar2->runtimeValue27C;
      (modelRuntime->rootModelNodeOrSavedOffset).modelNode = (ModelRuntimeNode *)0x0;
      (modelRuntime->ownerArmyRuntimeOrSavedOffset).armyRuntime = armyRuntime;
      modelRuntime->attachmentCount0C = 0;
      modelRuntime->definitionValue60_3C = dVar2;
      if (armyRuntime->runtimeState44 < uVar3) {
        armyRuntime->runtimeState44 = uVar3;
      }
      if (armyRuntime->runtimeState90 < uVar4) {
        armyRuntime->runtimeState90 = uVar4;
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
      dVar2 = pMVar2->runtimeValue8C;
      dVar5 = pMVar2->runtimeValue94;
      dVar6 = pMVar2->runtimeValue9C;
      modelRuntime->definitionValue84_40 = pMVar2->runtimeValue84;
      modelRuntime->definitionValue88_44 = dVar2;
      modelRuntime->definitionValue94_48 = dVar5;
      modelRuntime->definitionValue9C_4C = dVar6;
      dVar2 = pMVar2->runtimeValueAC;
      dVar5 = pMVar2->runtimeValueB4;
      dVar6 = pMVar2->runtimeValueBC;
      modelRuntime->definitionValueA4_50 = pMVar2->runtimeValueA4;
      modelRuntime->definitionValueAC_54 = dVar2;
      modelRuntime->definitionValueB4_58 = dVar5;
      modelRuntime->definitionValueBC_5C = dVar6;
      AVar7 = modelRuntime->ownerArmyRuntimeOrSavedOffset;
      dVar2 = pMVar2->runtimeValue18C;
      (modelRuntime->classState).enabledStateE4 = 1;
      (modelRuntime->classState).enabledStateE8 = 1;
      (modelRuntime->classState).linkedArmyRuntimeOrSavedOffset.armyRuntime = (ArmyRuntimeSlot *)0x0
      ;
      (modelRuntime->classState).definitionDerivedValueF4 = dVar2;
      (modelRuntime->classState).classStateEC = 0;
      (modelRuntime->classState).classStateF8 = 0;
      (modelRuntime->classState).classStateFC = 0;
      modelRuntime->classState118 = 0;
      bVar10 = false;
      if ((MdlSerializedNodeHeader38 *)pMVar2->serializedNodeOffsetOrPointer64 !=
          (MdlSerializedNodeHeader38 *)0x0) {
        lVar11 = ModelNodeRuntime_CreateHierarchyRecursiveCf
                           (AVar7,pMVar2->runtimeValue68,paletteAsset,textureSet,modelRuntime,
                            (MdlSerializedNodeHeader38 *)pMVar2->serializedNodeOffsetOrPointer64,
                            worldRuntime);
        modelNodeRuntime = (ModelRuntimeNode *)lVar11;
        if (bVar10) goto ModelRuntimePool_CreateInstanceByDefinitionId_ReturnCreationFailure;
        (modelRuntime->rootModelNodeOrSavedOffset).modelNode = modelNodeRuntime;
        uVar12 = ModelNodeRuntime_RecomputeSubtreeBoundingRadius(modelNodeRuntime);
        uVar13 = ModelNodeRuntime_RebuildTransformsFromRoot
                           (extraout_ECX,(int)((ulonglong)uVar12 >> 0x20),(ModelRuntimeNode *)uVar12
                           );
        if ((uVar13 & 0x10000000000) != 0) {
          puVar1 = (uint *)((int)uVar13 + 0x4c);
          *puVar1 = *puVar1 | 0x2000;
        }
      }
      (*g_ArmyRuntimeOrderHandlerMatrix11Columns24Classes.modelClassInitialize
        [((ModelDefinitionRecordPrefix *)(modelRuntime->definitionOrSavedId).savedIdOrOffset)[6].
         flags])((ModelDefinitionRecordPrefix *)(modelRuntime->definitionOrSavedId).savedIdOrOffset,
                 modelRuntime);
      return CONCAT44(in_EDX,modelRuntime);
    }
  }
  (*g_WideNumberFormatUtf16)
            (WIDE_FORMAT_WRITE_TERMINATOR,0,10,1,modelDefinitionId,g_PackageLastErrorPath);
  modelNodeRuntime = (ModelRuntimeNode *)0x3e;
ModelRuntimePool_CreateInstanceByDefinitionId_ReturnCreationFailure:
  return CONCAT44(in_EDX,modelNodeRuntime);
}

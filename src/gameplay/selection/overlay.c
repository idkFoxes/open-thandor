/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/src/gameplay/selection/overlay.c
 * Reverse engineering by idkFoxes 2026
 */

#include <thandor/gameplay/selection/overlay.h>

/* Implementation ownership: gameplay/selection/overlay. */

/* Address: 0x00568300.
   Ownership: gameplay/selection/overlay.
   Purpose: Builds or releases transient world-interaction marker objects according to current in-game UI and world
   state. Typed parameters: p0 releaseMode→GraphicsBooleanState_V307. Nearby but non-identical semantic domains
   were explicitly deferred. Calling convention, parameter storage, body bytes, control flow, globals, locals, and
   executable data remain unchanged.
   Local calls: InGameWorldOverlay_EnsureTransientEffectMarkerAtPoint.
   Cross-module calls: SelectionInfo_ValidateOwnerType16AndAnyActiveCf [gameplay/selection/runtime],
   ArmyRuntime_CreateInstanceFromAssetCf [gameplay/army/runtime], ModelNodeRuntime_RebuildTransformsFromRoot
   [world/model/hierarchy], ArmyRuntime_DestroyInstanceAndRefreshUi [gameplay/army/runtime],
   ArmyPlacement_ValidateAssetAtPointAndCellCornersCf [gameplay/army/placement],
   ArmyPlacement_DispatchAssetAtFieldPoint [gameplay/army/placement].
*/
void __thandor_void_preserve_eax_ecx_edx
InGameWorldOverlay_RebuildOrReleaseTransientMarkersCf
          (GraphicsBooleanState releaseMode,WorldRuntimeContext *worldRuntime)

{
  PackedArgb32 *pPVar1;
  ModelRuntimeNode *pMVar2;
  int *piVar3;
  ArmyRuntimeSlot *pAVar4;
  EffectRuntimeSlot *pEVar5;
  sdword sVar6;
  ArmyPlacementCandidateCount AVar7;
  GameEntityRuntime *pGVar8;
  Q12 worldXQ12;
  Q12 worldYQ12;
  uint uVar9;
  int iVar10;
  Q12 worldYQ12_00;
  Q12 worldXQ12_00;
  EffectDefinition *effectDefinition;
  PackedArgb32 PVar11;
  GameEntityRuntime **ppGVar12;
  ModelRuntimeNode *modelNodeCursor;
  EffectRuntimeSlot **ownedEffectCursor;
  EffectRuntimeSlot **commandTargetEffectCursor;
  bool bVar13;
  ArmyRuntimeCreateEaxCf5 AVar14;
  ArmyPlacementDispatchEaxCf5 AVar15;
  EffectDefinitionLookupEaxCf5 EVar16;
  FieldGridHeightEaxCf5 FVar17;
  EffectRuntimeCreateEaxCf5 EVar18;
  EffectDefinitionLookupEaxCf5 EVar19;
  PckArmyAssetIdCatalog armyAssetId;
  WorldRuntimeContext *worldRuntime_00;
  GameEntityRuntime *entityRuntime2;
  
  sVar6 = g_InGamePendingPlacementArmyAsset;
  if (((worldRuntime->interaction).interactionFlags48 & 8) != 0) {
    return;
  }
  if ((worldRuntime->runtimeFlags & 0x10) != 0) {
    return;
  }
  if ((g_UiCommandRuntimeFlags & 0x100) != 0) {
    return;
  }
  if ((g_UiCommandRuntimeFlags & 0x20) == 0) {
    bVar13 = SelectionInfo_ValidateOwnerType16AndAnyActiveCf
                       (worldRuntime->activeFactionRuntimeIndex);
    if (!bVar13) {
      if (releaseMode == GRAPHICS_STATE_DISABLED) {
        g_InGameCommandPreviewArmyRuntime = (GameEntityRuntime *)0x0;
        if ((((g_InGamePointerInteractionStateFlags & 3) == 0) &&
            (g_InGameCommandPreviewArmyAssetId != 0)) &&
           (g_InGameCommandPreviewSurfaceHeightQ12OrSentinel != 0x7fffffff)) {
          AVar14 = ArmyRuntime_CreateInstanceFromAssetCf
                             (1,g_InGameCommandPreviewHeading16,g_InGameCommandPreviewWorldXQ12,
                              g_InGameCommandPreviewWorldYQ12,
                              worldRuntime->activeFactionRuntimeIndex,
                              g_InGameCommandPreviewArmyAssetId,worldRuntime);
          if (!AVar14.carry) {
            pMVar2 = (((GameEntityRuntime *)AVar14.eax)->common).ownership.modelNode;
            g_InGameCommandPreviewArmyRuntime = (GameEntityRuntime *)AVar14.eax;
            pMVar2->tintArgb = 0xcfffffff;
            ModelNodeRuntime_RebuildTransformsFromRoot(pMVar2);
          }
        }
      }
      else if (g_InGameCommandPreviewArmyRuntime != (GameEntityRuntime *)0x0) {
        ArmyRuntime_DestroyInstanceAndRefreshUi(worldRuntime,g_InGameCommandPreviewArmyRuntime);
        g_InGameCommandPreviewArmyRuntime = (GameEntityRuntime *)0x0;
      }
    }
  }
  else if (releaseMode == GRAPHICS_STATE_DISABLED) {
    g_InGamePlacementPreviewArmyRuntime = (GameEntityRuntime *)0x0;
    if ((g_InGamePendingPlacementArmyAsset != 0) &&
       (g_InGamePlacementSurfaceHeightQ12OrSentinel != 0x7fffffff)) {
      PVar11 = 0xcfffffff;
      g_ArmyPlacementAcceptedCandidateCount = 1;
      bVar13 = ArmyPlacement_ValidateAssetAtPointAndCellCornersCf
                         (0,g_InGamePlacementHeading16,g_InGamePlacementWorldXQ12,
                          g_InGamePlacementWorldYQ12,
                          *(ArmyPlacementContext *)(g_InGamePendingPlacementArmyAsset + 8),
                          worldRuntime->activeFactionRuntimeIndex,worldRuntime);
      AVar7 = g_ArmyPlacementAcceptedCandidateCount;
      if (bVar13) {
        g_ArmyPlacementAcceptedCandidateCount = 0;
        if (AVar7 < 2) goto InGameWorldOverlay_RefreshTransientEffectMarkers;
        PVar11 = 0x4fffffff;
      }
      armyAssetId = *(PckArmyAssetIdCatalog *)(sVar6 + 8);
      g_ArmyPlacementAcceptedCandidateCount = 1;
      worldRuntime_00 = worldRuntime;
      AVar15 = ArmyPlacement_DispatchAssetAtFieldPoint
                         (1,0,g_InGamePlacementHeading16,worldYQ12_00,worldXQ12,
                          *(PckArmyAssetIdCatalog *)(sVar6 + 8),
                          worldRuntime->activeFactionRuntimeIndex,(UiRootNode *)worldRuntime);
      if ((AVar15.carry) && (g_ArmyPlacementAcceptedCandidateCount < 2)) {
        PVar11 = PVar11 & 0xff707070;
      }
      g_ArmyPlacementAcceptedCandidateCount = 0;
      AVar14 = ArmyRuntime_CreateInstanceFromAssetCf
                         (1,g_InGamePlacementHeading16,worldYQ12_00,worldXQ12,
                          worldRuntime->activeFactionRuntimeIndex,armyAssetId,worldRuntime_00);
      pGVar8 = (GameEntityRuntime *)AVar14.eax;
      if (!AVar14.carry) {
        pMVar2 = (pGVar8->common).ownership.modelNode;
        piVar3 = (pGVar8->common).ownership.definitionOrClassRecord;
        g_InGamePlacementPreviewArmyRuntime = pGVar8;
        pMVar2->tintArgb = PVar11;
        iVar10 = *piVar3;
        ModelNodeRuntime_RebuildTransformsFromRoot(pMVar2);
        if (((*(int *)(iVar10 + 0x4c) == 0xd) && (3 < pMVar2->childCount)) &&
           (pMVar2->childNodes[3] != (ModelRuntimeNode *)0x0)) {
          pPVar1 = &pMVar2->childNodes[3]->tintArgb;
          *pPVar1 = *pPVar1 | 0xff000000;
        }
      }
    }
  }
  else if (g_InGamePlacementPreviewArmyRuntime != (GameEntityRuntime *)0x0) {
    ArmyRuntime_DestroyInstanceAndRefreshUi(worldRuntime,g_InGamePlacementPreviewArmyRuntime);
    g_InGamePlacementPreviewArmyRuntime = (GameEntityRuntime *)0x0;
  }
InGameWorldOverlay_RefreshTransientEffectMarkers:
  if (releaseMode == GRAPHICS_STATE_DISABLED) {
    EVar16 = EffectDefinitionRegistry_FindByIdWithErrorCf(EFF_0143_EGATH0);
    if (!EVar16.carry) {
      modelNodeCursor = (ModelRuntimeNode *)worldRuntime->ownerListHead;
      uVar9 = 0;
      if (modelNodeCursor == (ModelRuntimeNode *)0x0) {
        return;
      }
      do {
        if (((modelNodeCursor->ownerClassId == MODEL_RUNTIME_CLASS_00) &&
            (pAVar4 = (modelNodeCursor->runtimePayload).armyRuntime,
            ((pAVar4->modelRuntimeOrSavedOffset).modelRuntime)->definitionValue9C_4C == 0xd)) &&
           (((pAVar4->runtimeFlags & 0x800) != 0 &&
            ((pAVar4->linkedEntityRuntime->common).ownership.ownerIndex ==
             worldRuntime->activeFactionRuntimeIndex)))) {
          FVar17 = FieldGrid_InterpolateTopSurfaceHeight
                             (pAVar4->movementTarget1Q12,pAVar4->movementTarget0Q12,
                              worldRuntime->fieldGrid);
          EVar18 = EffectRuntimePool_CreateInstanceFromDefinitionCf
                             (EFFECT_RUNTIME_COMPLETION_NONE,(EffectRuntimeOwnerReference4)0x0,0,
                              0x4000,0,FVar17.heightQ12,pAVar4->movementTarget1Q12,
                              pAVar4->movementTarget0Q12,EVar16.definitionOrError,worldRuntime);
          if (!EVar18.carry) {
            g_UiAction1012SubresourceByState[uVar9 + 0xb] = (dword)EVar18.effectRuntime;
            uVar9 = uVar9 + 1;
            (((EVar18.effectRuntime)->modelNodeOrSavedOffset).modelNode)->tintArgb = 0xffffffff;
            g_InGameOwnedEntityTransientEffectMarkerCount =
                 g_InGameOwnedEntityTransientEffectMarkerCount + 1;
            if (0x1f < uVar9) break;
          }
        }
        modelNodeCursor = (ModelRuntimeNode *)(modelNodeCursor->common).nextNode;
      } while (modelNodeCursor != (ModelRuntimeNode *)0x0);
    }
    EVar16 = EffectDefinitionRegistry_FindByIdWithErrorCf(EFF_0148_EWAYP0);
    if (!EVar16.carry) {
      EVar19 = EffectDefinitionRegistry_FindByIdWithErrorCf(EFF_0149_ETARG0);
      if (!EVar19.carry) {
        iVar10 = 0x20;
        ppGVar12 = g_SelectionInfoEntitySlots->entries;
        do {
          pGVar8 = *ppGVar12;
          if ((pGVar8 != (GameEntityRuntime *)0x0) &&
             (worldRuntime->activeFactionRuntimeIndex == (pGVar8->common).ownership.ownerIndex)) {
            if ((*(int *)(*(int *)(pGVar8->common).ownership.definitionOrClassRecord + 0x18) != 0)
               && (((pGVar8->common).commandFlags & 1) != 0)) {
              InGameWorldOverlay_EnsureTransientEffectMarkerAtPoint
                        (0x1000,(pGVar8->common).ownership.modelNode,
                         (pGVar8->common).pathCoordinate1Q12,(pGVar8->common).pathCoordinate0Q12,
                         EVar16.definitionOrError,worldRuntime);
              if (0x7f < g_InGameCommandTargetTransientEffectMarkerCount) {
                return;
              }
              if (((pGVar8->common).commandFlags & 8) != 0) {
                uVar9 = 0;
                do {
                  InGameWorldOverlay_EnsureTransientEffectMarkerAtPoint
                            (0x1000,(pGVar8->common).ownership.modelNode,
                             *(Q12 *)((pGVar8->common).reservedC0_EB + uVar9 * 8 + 4),
                             *(Q12 *)((pGVar8->common).reservedC0_EB + uVar9 * 8),
                             EVar16.definitionOrError,worldRuntime);
                  uVar9 = uVar9 + 1;
                  if (0x7f < g_InGameCommandTargetTransientEffectMarkerCount) {
                    return;
                  }
                } while (uVar9 < *(uint *)((pGVar8->common).reservedA4_B7 + 4));
              }
            }
            if ((((pGVar8->common).commandTarget.targetFlags & 2) != 0) &&
               (InGameWorldOverlay_EnsureTransientEffectMarkerAtPoint
                          (0x1000,(pGVar8->common).ownership.modelNode,
                           (pGVar8->common).commandTarget.targetWorldYQ12,
                           (pGVar8->common).commandTarget.targetWorldXQ12,EVar19.definitionOrError,
                           worldRuntime), 0x7f < g_InGameCommandTargetTransientEffectMarkerCount)) {
              return;
            }
            entityRuntime2 = (pGVar8->common).commandTarget.targetEntity;
            if (((((pGVar8->common).commandTarget.targetFlags & 1) != 0) &&
                (entityRuntime2 != (GameEntityRuntime *)0x0)) &&
               (pMVar2 = (entityRuntime2->common).ownership.modelNode,
               InGameWorldOverlay_EnsureTransientEffectMarkerAtPoint
                         (*(Q12 *)(*(int *)(entityRuntime2->common).ownership.
                                           definitionOrClassRecord + 0xdc),
                          (pGVar8->common).ownership.modelNode,
                          (pMVar2->worldTransform).translation.y,
                          (pMVar2->worldTransform).translation.x,EVar19.definitionOrError,
                          worldRuntime), 0x7f < g_InGameCommandTargetTransientEffectMarkerCount)) {
              return;
            }
          }
          ppGVar12 = ppGVar12 + 1;
          iVar10 = iVar10 + -1;
        } while (iVar10 != 0);
      }
    }
  }
  else {
    ownedEffectCursor = (EffectRuntimeSlot **)(g_UiAction1012SubresourceByState + 0xb);
    iVar10 = g_InGameOwnedEntityTransientEffectMarkerCount;
    if (g_InGameOwnedEntityTransientEffectMarkerCount != 0) {
      do {
        pEVar5 = *ownedEffectCursor;
        pMVar2 = (pEVar5->modelNodeOrSavedOffset).modelNode;
        InterpolationState_SetNegatedTargetAndRescaleProgress(0,pMVar2->shadingRecord);
        WorldRuntime_UnlinkNodeFromOwnerListD8((WorldOwnerListNode100 *)pMVar2);
        (pEVar5->modelNodeOrSavedOffset).modelNode = (ModelRuntimeNode *)0x0;
        ownedEffectCursor = ownedEffectCursor + 1;
        iVar10 = iVar10 + -1;
      } while (iVar10 != 0);
      g_InGameOwnedEntityTransientEffectMarkerCount = 0;
    }
    commandTargetEffectCursor = (EffectRuntimeSlot **)0x562ecc;
    uVar9 = g_InGameCommandTargetTransientEffectMarkerCount;
    if (g_InGameCommandTargetTransientEffectMarkerCount != 0) {
      do {
        pEVar5 = *commandTargetEffectCursor;
        pMVar2 = (pEVar5->modelNodeOrSavedOffset).modelNode;
        InterpolationState_SetNegatedTargetAndRescaleProgress(0,pMVar2->shadingRecord);
        WorldRuntime_UnlinkNodeFromOwnerListD8((WorldOwnerListNode100 *)pMVar2);
        (pEVar5->modelNodeOrSavedOffset).modelNode = (ModelRuntimeNode *)0x0;
        commandTargetEffectCursor = commandTargetEffectCursor + 1;
        uVar9 = uVar9 - 1;
      } while (uVar9 != 0);
      g_InGameCommandTargetTransientEffectMarkerCount = 0;
    }
  }
  return;
}


/* Address: 0x0052F0C0.
   Ownership: gameplay/selection/overlay.
   Purpose: Handles selection overlay render selected army metrics.
   Cross-module calls: ModelProjectedBounds_AccumulateHierarchyRecursive [graphics/render/model],
   SelectionPanel_RenderArmyRuntimeMetrics [gameplay/selection/runtime].
*/
void __thandor_void_preserve_eax_ecx_edx
SelectionOverlay_RenderSelectedArmyMetrics
          (UiPixelCoordinate clipTop,UiPixelCoordinate clipLeft,UiPixelCoordinate clipBottom,
          UiPixelCoordinate clipRight)

{
  ModelRuntimeNode *modelNode;
  int iVar1;
  RuntimeModelFactionPrefix10 *runtimeEntry;
  GameEntityRuntime **ppGVar2;
  
  iVar1 = 0x20;
  ppGVar2 = g_SelectionInfoEntitySlots->entries;
  do {
    if ((*ppGVar2 != (GameEntityRuntime *)0x0) &&
       ((modelNode = ((*ppGVar2)->common).ownership.modelNode, (modelNode->runtimeFlags & 4) != 0 ||
        (((modelNode->runtimeFlags & 0x10) == 0 && ((modelNode->runtimeFlags & 8) != 0)))))) {
      g_ModelProjectedBoundsPixels.minX = 0x10000;
      g_ModelProjectedBoundsPixels.minY = 0x10000;
      g_ModelProjectedBoundsPixels.maxX = -0x10000;
      g_ModelProjectedBoundsPixels.maxY = -0x10000;
      ModelProjectedBounds_AccumulateHierarchyRecursive(&g_ModelProjectedBoundsPixels,modelNode);
      if ((g_ModelProjectedBoundsPixels.minX < g_ModelProjectedBoundsPixels.maxX) &&
         (g_ModelProjectedBoundsPixels.minY < g_ModelProjectedBoundsPixels.maxY)) {
        SelectionPanel_RenderArmyRuntimeMetrics
                  (clipTop,clipLeft,clipBottom,clipRight,g_ModelProjectedBoundsPixels.maxY,
                   g_ModelProjectedBoundsPixels.maxX,g_ModelProjectedBoundsPixels.minY,
                   g_ModelProjectedBoundsPixels.minX,runtimeEntry);
      }
    }
    ppGVar2 = ppGVar2 + 1;
    iVar1 = iVar1 + -1;
  } while (iVar1 != 0);
  return;
}


/* Address: 0x0052F1B0.
   Ownership: gameplay/selection/overlay.
   Purpose: Handles selection overlay render army metrics for entity.
   Cross-module calls: ModelProjectedBounds_AccumulateHierarchyRecursive [graphics/render/model],
   SelectionPanel_RenderArmyRuntimeMetrics [gameplay/selection/runtime].
*/
void __thandor_void_preserve_eax_ecx_edx
SelectionOverlay_RenderArmyMetricsForEntity
          (UiPixelCoordinate clipTop,UiPixelCoordinate clipLeft,UiPixelCoordinate clipBottom,
          UiPixelCoordinate clipRight,GameEntityRuntime *entity)

{
  ModelRuntimeNode *modelNode;
  GraphicsTextureSourceAsset *pGVar1;
  void *pvVar2;
  RuntimeModelFactionPrefix10 *runtimeEntry;
  
  modelNode = (entity->common).ownership.modelNode;
  if (((modelNode->runtimeFlags & 4) != 0) ||
     (((modelNode->runtimeFlags & 0x10) == 0 && ((modelNode->runtimeFlags & 8) != 0)))) {
    g_ModelProjectedBoundsPixels.minX = 0x10000;
    g_ModelProjectedBoundsPixels.minY = 0x10000;
    g_ModelProjectedBoundsPixels.maxX = -0x10000;
    g_ModelProjectedBoundsPixels.maxY = -0x10000;
    ModelProjectedBounds_AccumulateHierarchyRecursive(&g_ModelProjectedBoundsPixels,modelNode);
    pvVar2 = g_SelectionPanelData;
    pGVar1 = g_SelectionPanelTextureSource;
    g_SelectionPanelTextureSource = pGVar1;
    g_SelectionPanelData = pvVar2;
    if ((g_ModelProjectedBoundsPixels.minX < g_ModelProjectedBoundsPixels.maxX) &&
       (g_ModelProjectedBoundsPixels.minY < g_ModelProjectedBoundsPixels.maxY)) {
      g_SelectionPanelTextureSource = g_InfoPanelTextureSource;
      g_SelectionPanelData = g_InfoPanelData;
      SelectionPanel_RenderArmyRuntimeMetrics
                (clipTop,clipLeft,clipBottom,clipRight,g_ModelProjectedBoundsPixels.maxY,
                 g_ModelProjectedBoundsPixels.maxX,g_ModelProjectedBoundsPixels.minY,
                 g_ModelProjectedBoundsPixels.minX,runtimeEntry);
      g_SelectionPanelTextureSource = pGVar1;
      g_SelectionPanelData = pvVar2;
    }
  }
  return;
}


/* Address: 0x0052F2A0.
   Ownership: gameplay/selection/overlay.
   Purpose: Handles selection overlay draw bounds frame.
*/
void __thandor_void_preserve_eax_ecx_edx
SelectionOverlay_DrawBoundsFrame
          (UiPixelCoordinate clipTop,UiPixelCoordinate clipLeft,UiPixelCoordinate clipBottom,
          UiPixelCoordinate clipRight,UiPixelCoordinate frameCoordinate0A,
          UiPixelCoordinate frameCoordinate1A,UiPixelCoordinate frameCoordinate0B,
          UiPixelCoordinate frameCoordinate1B)

{
  int iVar1;
  int iVar2;
  UiPixelCoordinate UVar3;
  UiPixelCoordinate UVar4;
  dword dVar5;
  bool bVar6;
  GraphicsTextureSizeEaxEdxCf9 GVar7;
  
  UVar4 = frameCoordinate1B;
  UVar3 = frameCoordinate0B;
  if (frameCoordinate1A <= frameCoordinate1B) {
    if (frameCoordinate1B == frameCoordinate1A) {
      return;
    }
    frameCoordinate1B = frameCoordinate1A;
    frameCoordinate1A = UVar4;
  }
  if (frameCoordinate0A <= frameCoordinate0B) {
    if (frameCoordinate0B == frameCoordinate0A) {
      return;
    }
    frameCoordinate0B = frameCoordinate0A;
    frameCoordinate0A = UVar3;
  }
  bVar6 = (*g_GraphicsFramebufferBeginAccess)();
  if (!bVar6) {
    GVar7 = (*g_GraphicsTextureSourceGetLogicalSize)(0xa4,g_SelectionPanelTextureSource);
    dVar5 = GVar7.logicalWidthPixels;
    iVar1 = frameCoordinate1B - dVar5;
    iVar2 = frameCoordinate0B - GVar7.logicalHeightPixels;
    (*g_SelectionPanelBlitOpaque)
              (clipTop,clipLeft,clipBottom,clipRight,iVar2,iVar1,0xa4,g_SelectionPanelTextureSource,
               g_FramebufferAccess);
    (*g_SelectionPanelBlitOpaque)
              (clipTop,clipLeft,clipBottom,clipRight,iVar2,frameCoordinate1A,0xa6,
               g_SelectionPanelTextureSource,g_FramebufferAccess);
    (*g_SelectionPanelBlitOpaque)
              (clipTop,clipLeft,clipBottom,clipRight,frameCoordinate0A,iVar1,0xa9,
               g_SelectionPanelTextureSource,g_FramebufferAccess);
    (*g_SelectionPanelBlitOpaque)
              (clipTop,clipLeft,clipBottom,clipRight,frameCoordinate0A,frameCoordinate1A,0xab,
               g_SelectionPanelTextureSource,g_FramebufferAccess);
    iVar1 = iVar1 + dVar5;
    (*g_SelectionPanelBlitClipped)
              (clipTop,clipLeft,clipBottom,clipRight,-0x80000000,frameCoordinate1A,iVar2,iVar1,0xa5,
               g_SelectionPanelTextureSource,g_FramebufferAccess);
    (*g_SelectionPanelBlitClipped)
              (clipTop,clipLeft,clipBottom,clipRight,-0x80000000,frameCoordinate1A,frameCoordinate0A
               ,iVar1,0xaa,g_SelectionPanelTextureSource,g_FramebufferAccess);
    iVar2 = iVar2 + GVar7.logicalHeightPixels;
    (*g_SelectionPanelBlitClipped)
              (clipTop,clipLeft,clipBottom,clipRight,frameCoordinate0A,-0x80000000,iVar2,
               iVar1 - dVar5,0xa7,g_SelectionPanelTextureSource,g_FramebufferAccess);
    (*g_SelectionPanelBlitClipped)
              (clipTop,clipLeft,clipBottom,clipRight,frameCoordinate0A,-0x80000000,iVar2,
               frameCoordinate1A,0xa8,g_SelectionPanelTextureSource,g_FramebufferAccess);
    (*g_GraphicsFramebufferEndAccess)();
  }
  return;
}


/* Address: 0x0052F490.
   Ownership: gameplay/selection/overlay.
   Purpose: Handles selection overlay draw marker adfor field grid terrain points.
   Cross-module calls: FieldGrid_GetNearestTerrainPoint [world/terrain/grid], FixedTransform_ApplyPoint
   [core/math/fixed], Graphics_ProjectViewPoint [graphics/core/runtime].
*/
void __thandor_void_preserve_eax_ecx_edx
SelectionOverlay_DrawMarkerADForFieldGridTerrainPoints
          (UiPixelCoordinate clipTop,UiPixelCoordinate clipLeft,UiPixelCoordinate clipBottom,
          UiPixelCoordinate clipRight,int markerPointCount,int *gridCoordinatePairs,
          FieldGridAsset *fieldGrid)

{
  longlong lVar1;
  longlong lVar2;
  int iVar3;
  int iVar4;
  bool bVar5;
  GraphicsProjectedPointPair GVar6;
  GraphicsTextureSizeEaxEdxCf9 GVar7;
  FieldGridNearestPointRegsCf13 FVar8;
  dword arg6;
  GraphicsTextureSourceAsset *arg7;
  SoftwareFramebufferAccess *arg8;
  
  if (markerPointCount != 0) {
    bVar5 = (*g_GraphicsFramebufferBeginAccess)();
    if (!bVar5) {
      do {
        lVar1 = (longlong)(gridCoordinatePairs[1] + *gridCoordinatePairs * 2) * 0x901;
        lVar2 = (longlong)gridCoordinatePairs[1] * -1999;
        FVar8 = FieldGrid_GetNearestTerrainPoint
                          ((int)((ulonglong)lVar2 >> 0x20) << 0x14 | (uint)lVar2 >> 0xc,
                           (int)((ulonglong)lVar1 >> 0x20) << 0x13 | (uint)lVar1 >> 0xd,fieldGrid);
        if (!FVar8.carry) {
          g_GraphicsTransformScratchMatrix3x4.basisRow0[0] = FVar8.eax;
          g_GraphicsTransformScratchMatrix3x4.basisRow0[1] = FVar8.ecx;
          g_GraphicsTransformScratchMatrix3x4.basisRow0[2] = FVar8.edx;
          FixedTransform_ApplyPoint
                    (&g_GraphicsTransformInputScratchVec3,
                     (GraphicsFixedVec3 *)&g_GraphicsTransformScratchMatrix3x4,
                     &g_ViewProjectionMatrixFixed);
          if (0x10 < g_GraphicsTransformInputScratchVec3.z) {
            GVar6 = Graphics_ProjectViewPoint(&g_GraphicsTransformInputScratchVec3);
            iVar3 = GVar6.projectedX >> 0xc;
            iVar4 = GVar6.projectedY >> 0xc;
            arg6 = 0xad;
            arg7 = g_SelectionPanelTextureSource;
            arg8 = g_FramebufferAccess;
            GVar7 = (*g_GraphicsTextureSourceGetLogicalSize)(0xad,g_SelectionPanelTextureSource);
            (*g_SelectionPanelBlitOpaque)
                      (clipTop,clipLeft,clipBottom,clipRight,
                       iVar4 - ((int)GVar7.logicalHeightPixels >> 1),
                       iVar3 - ((int)GVar7.logicalWidthPixels >> 1),arg6,arg7,arg8);
          }
        }
        gridCoordinatePairs = gridCoordinatePairs + 2;
        markerPointCount = markerPointCount + -1;
      } while (markerPointCount != 0);
      (*g_GraphicsFramebufferEndAccess)();
    }
  }
  return;
}


/* Address: 0x0052F5A0.
   Ownership: gameplay/selection/overlay.
   Purpose: Handles selection overlay draw marker acfor world surface point.
   Cross-module calls: FieldGrid_GetNearestTerrainPoint [world/terrain/grid], FieldGrid_GetNearestTopSurfacePoint
   [world/terrain/grid], FixedTransform_ApplyPoint [core/math/fixed], Graphics_ProjectViewPoint
   [graphics/core/runtime].
*/
void __thandor_void_preserve_eax_ecx_edx
SelectionOverlay_DrawMarkerACForWorldSurfacePoint
          (UiPixelCoordinate clipTop,UiPixelCoordinate clipLeft,UiPixelCoordinate clipBottom,
          UiPixelCoordinate clipRight,int useTopSurface,Q12 worldCoordinate0Q12,
          Q12 worldCoordinate1Q12,FieldGridAsset *fieldGrid)

{
  dword dVar1;
  dword dVar2;
  dword dVar3;
  bool bVar4;
  GraphicsProjectedPointPair GVar5;
  GraphicsTextureSizeEaxEdxCf9 GVar6;
  FieldGridSurfacePointEaxEcxEdxCf13 FVar7;
  FieldGridNearestPointRegsCf13 FVar8;
  
  if (useTopSurface == 0) {
    FVar8 = FieldGrid_GetNearestTerrainPoint(worldCoordinate0Q12,worldCoordinate1Q12,fieldGrid);
    dVar3 = FVar8.edx;
    dVar2 = FVar8.ecx;
    dVar1 = FVar8.eax;
    if (FVar8.carry) {
      return;
    }
  }
  else {
    FVar7 = FieldGrid_GetNearestTopSurfacePoint(worldCoordinate0Q12,worldCoordinate1Q12,fieldGrid);
    dVar3 = FVar7.worldZQ12;
    dVar2 = FVar7.worldYQ12;
    dVar1 = FVar7.worldXQ12;
    if (FVar7.carry) {
      return;
    }
  }
  g_GraphicsTransformScratchMatrix3x4.basisRow0[0] = dVar1;
  g_GraphicsTransformScratchMatrix3x4.basisRow0[1] = dVar2;
  g_GraphicsTransformScratchMatrix3x4.basisRow0[2] = dVar3;
  FixedTransform_ApplyPoint
            (&g_GraphicsTransformInputScratchVec3,
             (GraphicsFixedVec3 *)&g_GraphicsTransformScratchMatrix3x4,&g_ViewProjectionMatrixFixed)
  ;
  GVar5 = Graphics_ProjectViewPoint(&g_GraphicsTransformInputScratchVec3);
  bVar4 = (*g_GraphicsFramebufferBeginAccess)(worldCoordinate0Q12,worldCoordinate1Q12,fieldGrid);
  if (!bVar4) {
    GVar6 = (*g_GraphicsTextureSourceGetLogicalSize)(0xac,g_SelectionPanelTextureSource);
    (*g_SelectionPanelBlitOpaque)
              (clipTop,clipLeft,clipBottom,clipRight,
               (GVar5.projectedY >> 0xc) - ((int)GVar6.logicalHeightPixels >> 1),
               (GVar5.projectedX >> 0xc) - ((int)GVar6.logicalWidthPixels >> 1),0xac,
               g_SelectionPanelTextureSource,g_FramebufferAccess);
    (*g_GraphicsFramebufferEndAccess)();
  }
  return;
}


/* Address: 0x0052F680.
   Ownership: gameplay/selection/overlay.
   Purpose: Handles selection overlay draw marker aefor visible projected grid vertices.
*/
void __thandor_void_preserve_eax_ecx_edx
SelectionOverlay_DrawMarkerAEForVisibleProjectedGridVertices
          (UiPixelCoordinate clipTop,UiPixelCoordinate clipLeft,UiPixelCoordinate clipBottom,
          UiPixelCoordinate clipRight,FieldGridAsset *fieldGrid)

{
  uint uVar1;
  int iVar2;
  uint uVar3;
  uint uVar4;
  uint uVar5;
  int iVar6;
  byte *pbVar7;
  int iVar8;
  bool bVar9;
  GraphicsTextureSizeEaxEdxCf9 GVar10;
  dword arg6;
  GraphicsTextureSourceAsset *arg7;
  SoftwareFramebufferAccess *arg8;
  byte *pbStack_20;
  
  bVar9 = (*g_GraphicsFramebufferBeginAccess)();
  if (!bVar9) {
    iVar8 = 0;
    uVar1 = fieldGrid->gridWidth;
    uVar3 = uVar1 >> 2;
    uVar5 = fieldGrid->gridHeight >> 2;
    pbVar7 = fieldGrid[1].common.buildMetadata.assetRelativeAddressAnchor28 + uVar1 * 0x80 + -0x28;
    uVar4 = uVar3;
    pbStack_20 = pbVar7;
    if ((g_UiCommandModeGColorVariantLimit & 0xff000000) != 0) {
      iVar8 = 0x20;
    }
    do {
      do {
        if ((*(uint *)(pbVar7 + 0x50) & 0x200000) == 0) {
          iVar2 = *(int *)(pbVar7 + iVar8 + 0xc) >> 0xc;
          iVar6 = *(int *)(pbVar7 + iVar8 + 0x10) >> 0xc;
          arg6 = 0xae;
          arg7 = g_SelectionPanelTextureSource;
          arg8 = g_FramebufferAccess;
          GVar10 = (*g_GraphicsTextureSourceGetLogicalSize)(0xae,g_SelectionPanelTextureSource);
          (*g_SelectionPanelBlitOpaque)
                    (clipTop,clipLeft,clipBottom,clipRight,
                     iVar6 - ((int)GVar10.logicalHeightPixels >> 1),
                     iVar2 - ((int)GVar10.logicalWidthPixels >> 1),arg6,arg7,arg8);
        }
        pbVar7 = pbVar7 + 0x200;
        uVar4 = uVar4 - 1;
      } while (-1 < (int)uVar4);
      pbVar7 = pbStack_20 + uVar1 * 0x200;
      uVar5 = uVar5 - 1;
      uVar4 = uVar3;
      pbStack_20 = pbVar7;
    } while (-1 < (int)uVar5);
    (*g_GraphicsFramebufferEndAccess)();
  }
  return;
}


/* Address: 0x0052F780.
   Ownership: gameplay/selection/overlay.
   Purpose: Handles selection overlay draw marker afb0 for projected vertex state flags.
*/
void __thandor_void_preserve_eax_ecx_edx
SelectionOverlay_DrawMarkerAFB0ForProjectedVertexStateFlags
          (UiPixelCoordinate clipTop,UiPixelCoordinate clipLeft,UiPixelCoordinate clipBottom,
          UiPixelCoordinate clipRight,FieldGridAsset *fieldGrid)

{
  FieldGridDimension FVar1;
  int iVar2;
  FieldGridDimension FVar3;
  int iVar4;
  FieldGridDimension FVar5;
  FieldGridCell *pFVar6;
  bool bVar7;
  GraphicsTextureSizeEaxEdxCf9 GVar8;
  dword arg6;
  GraphicsTextureSourceAsset *arg7;
  SoftwareFramebufferAccess *arg8;
  int iVar9;
  int iVar10;
  dword arg6_00;
  GraphicsTextureSourceAsset *arg7_00;
  SoftwareFramebufferAccess *arg8_00;
  FieldGridCell *pFStack_1c;
  
  bVar7 = (*g_GraphicsFramebufferBeginAccess)();
  if (!bVar7) {
    FVar1 = fieldGrid->gridWidth;
    FVar5 = fieldGrid->gridHeight;
    pFVar6 = fieldGrid->cells;
    FVar3 = FVar1;
    pFStack_1c = pFVar6;
    do {
      do {
        if (((pFVar6->flagsAndMaterial & 0x4000000) == 0) &&
           ((pFVar6->flagsAndMaterial &
            (FIELD_CELL_FLUID_SOURCE_EXCLUDED|FIELD_CELL_FLUID_RECEIVER_EXCLUDED)) != 0)) {
          iVar2 = *(int *)(pFVar6->runtime0C_3F + 0x20) >> 0xc;
          iVar4 = *(int *)(pFVar6->runtime0C_3F + 0x24) >> 0xc;
          arg6_00 = 0xb0;
          arg6 = 0xaf;
          arg7_00 = g_SelectionPanelTextureSource;
          arg8_00 = g_FramebufferAccess;
          if ((pFVar6->flagsAndMaterial & FIELD_CELL_FLUID_RECEIVER_EXCLUDED) != 0) {
            arg7 = g_SelectionPanelTextureSource;
            arg8 = g_FramebufferAccess;
            iVar9 = iVar4;
            iVar10 = iVar2;
            GVar8 = (*g_GraphicsTextureSourceGetLogicalSize)(0xaf,g_SelectionPanelTextureSource);
            (*g_SelectionPanelBlitOpaque)
                      (clipTop,clipLeft,clipBottom,clipRight,
                       iVar4 - ((int)GVar8.logicalHeightPixels >> 1),
                       iVar2 - ((int)GVar8.logicalWidthPixels >> 1),arg6,arg7,arg8);
            iVar4 = iVar9;
            iVar2 = iVar10;
          }
          if ((pFVar6->flagsAndMaterial & FIELD_CELL_FLUID_SOURCE_EXCLUDED) != 0) {
            GVar8 = (*g_GraphicsTextureSourceGetLogicalSize)(0xb0,g_SelectionPanelTextureSource);
            (*g_SelectionPanelBlitOpaque)
                      (clipTop,clipLeft,clipBottom,clipRight,
                       iVar4 - ((int)GVar8.logicalHeightPixels >> 1),
                       iVar2 - ((int)GVar8.logicalWidthPixels >> 1),arg6_00,arg7_00,arg8_00);
          }
        }
        pFVar6 = pFVar6 + 1;
        FVar3 = FVar3 - 1;
      } while (FVar3 != 0);
      pFVar6 = pFStack_1c + FVar1;
      FVar5 = FVar5 - 1;
      FVar3 = FVar1;
      pFStack_1c = pFVar6;
    } while (FVar5 != 0);
    (*g_GraphicsFramebufferEndAccess)();
  }
  return;
}


/* Address: 0x0052F8C0.
   Ownership: gameplay/selection/overlay.
   Purpose: Handles selection overlay draw marker b1 b2 for projected vertex mask1800.
*/
void __thandor_void_preserve_eax_ecx_edx
SelectionOverlay_DrawMarkerB1B2ForProjectedVertexMask1800
          (UiPixelCoordinate clipTop,UiPixelCoordinate clipLeft,UiPixelCoordinate clipBottom,
          UiPixelCoordinate clipRight,byte markerBitIndex,FieldGridAsset *fieldGrid)

{
  FieldGridDimension FVar1;
  int iVar2;
  FieldGridDimension FVar3;
  int iVar4;
  FieldGridDimension FVar5;
  FieldGridCell *pFVar6;
  FieldCellPackedFlagsAndMaterial FVar7;
  bool bVar8;
  GraphicsTextureSizeEaxEdxCf9 GVar9;
  dword arg6;
  GraphicsTextureSourceAsset *arg7;
  SoftwareFramebufferAccess *arg8;
  int iVar10;
  int iVar11;
  dword arg6_00;
  GraphicsTextureSourceAsset *arg7_00;
  SoftwareFramebufferAccess *arg8_00;
  FieldGridCell *pFStack_20;
  
  FVar7 = 0x800 << (markerBitIndex & 0x1f);
  bVar8 = (*g_GraphicsFramebufferBeginAccess)();
  if (!bVar8) {
    FVar1 = fieldGrid->gridWidth;
    FVar5 = fieldGrid->gridHeight;
    pFVar6 = fieldGrid->cells;
    FVar3 = FVar1;
    pFStack_20 = pFVar6;
    do {
      do {
        if (((pFVar6->flagsAndMaterial & 0x200000) == 0) &&
           ((pFVar6->flagsAndMaterial & FIELD_CELL_XENITE_OR_TRITIUM_SUPPORT_MASK) != 0)) {
          iVar2 = *(int *)pFVar6->runtime0C_3F >> 0xc;
          iVar4 = *(int *)(pFVar6->runtime0C_3F + 4) >> 0xc;
          arg6_00 = 0xb2;
          arg6 = 0xb1;
          arg7_00 = g_SelectionPanelTextureSource;
          arg8_00 = g_FramebufferAccess;
          if ((pFVar6->flagsAndMaterial & FVar7) != 0) {
            arg7 = g_SelectionPanelTextureSource;
            arg8 = g_FramebufferAccess;
            iVar10 = iVar4;
            iVar11 = iVar2;
            GVar9 = (*g_GraphicsTextureSourceGetLogicalSize)(0xb1,g_SelectionPanelTextureSource);
            (*g_SelectionPanelBlitOpaque)
                      (clipTop,clipLeft,clipBottom,clipRight,
                       iVar4 - ((int)GVar9.logicalHeightPixels >> 1),
                       iVar2 - ((int)GVar9.logicalWidthPixels >> 1),arg6,arg7,arg8);
            iVar4 = iVar10;
            iVar2 = iVar11;
          }
          if ((pFVar6->flagsAndMaterial & (FVar7 ^ FIELD_CELL_XENITE_OR_TRITIUM_SUPPORT_MASK)) != 0)
          {
            GVar9 = (*g_GraphicsTextureSourceGetLogicalSize)(0xb2,g_SelectionPanelTextureSource);
            (*g_SelectionPanelBlitOpaque)
                      (clipTop,clipLeft,clipBottom,clipRight,
                       iVar4 - ((int)GVar9.logicalHeightPixels >> 1),
                       iVar2 - ((int)GVar9.logicalWidthPixels >> 1),arg6_00,arg7_00,arg8_00);
          }
        }
        pFVar6 = pFVar6 + 1;
        FVar3 = FVar3 - 1;
      } while (FVar3 != 0);
      pFVar6 = pFStack_20 + FVar1;
      FVar5 = FVar5 - 1;
      FVar3 = FVar1;
      pFStack_20 = pFVar6;
    } while (FVar5 != 0);
    (*g_GraphicsFramebufferEndAccess)();
  }
  return;
}


/* Address: 0x0052FA20.
   Ownership: gameplay/selection/overlay.
   Purpose: Handles selection overlay draw marker affor projected vertex flag8000.
*/
void __thandor_void_preserve_eax_ecx_edx
SelectionOverlay_DrawMarkerAFForProjectedVertexFlag8000
          (UiPixelCoordinate clipTop,UiPixelCoordinate clipLeft,UiPixelCoordinate clipBottom,
          UiPixelCoordinate clipRight,FieldGridAsset *fieldGrid)

{
  FieldGridDimension FVar1;
  int iVar2;
  FieldGridDimension FVar3;
  int iVar4;
  FieldGridDimension FVar5;
  FieldGridCell *pFVar6;
  bool bVar7;
  GraphicsTextureSizeEaxEdxCf9 GVar8;
  dword arg6;
  GraphicsTextureSourceAsset *arg7;
  SoftwareFramebufferAccess *arg8;
  FieldGridCell *pFStack_1c;
  
  bVar7 = (*g_GraphicsFramebufferBeginAccess)();
  if (!bVar7) {
    FVar1 = fieldGrid->gridWidth;
    FVar5 = fieldGrid->gridHeight;
    pFVar6 = fieldGrid->cells;
    FVar3 = FVar1;
    pFStack_1c = pFVar6;
    do {
      do {
        if (((pFVar6->flagsAndMaterial & FIELD_CELL_INIT_CLEARED_UNRESOLVED_BIT15) != 0) &&
           ((pFVar6->flagsAndMaterial & 0x200000) == 0)) {
          iVar2 = *(int *)pFVar6->runtime0C_3F >> 0xc;
          iVar4 = *(int *)(pFVar6->runtime0C_3F + 4) >> 0xc;
          arg6 = 0xaf;
          arg7 = g_SelectionPanelTextureSource;
          arg8 = g_FramebufferAccess;
          GVar8 = (*g_GraphicsTextureSourceGetLogicalSize)(0xaf,g_SelectionPanelTextureSource);
          (*g_SelectionPanelBlitOpaque)
                    (clipTop,clipLeft,clipBottom,clipRight,
                     iVar4 - ((int)GVar8.logicalHeightPixels >> 1),
                     iVar2 - ((int)GVar8.logicalWidthPixels >> 1),arg6,arg7,arg8);
        }
        pFVar6 = pFVar6 + 1;
        FVar3 = FVar3 - 1;
      } while (FVar3 != 0);
      pFVar6 = pFStack_1c + FVar1;
      FVar5 = FVar5 - 1;
      FVar3 = FVar1;
      pFStack_1c = pFVar6;
    } while (FVar5 != 0);
    (*g_GraphicsFramebufferEndAccess)();
  }
  return;
}


/* Address: 0x00560020.
   Ownership: gameplay/selection/overlay.
   Purpose: Binary entry is anchored by g_RuntimeRebaseCallbackTable5[0]@00563754. Typed parameters: p0
   selectionIndex→SelectionMarkerIndex_V342, p1 valueC→SelectionMarkerCoordinateValue32_V342, p2
   valueB→SelectionMarkerCoordinateValue32_V342, p3 valueA→SelectionMarkerCoordinateValue32_V342. Calling
   convention, exact VariableStorage serialization, function body bytes, control flow, globals, locals, and
   executable data remain unchanged.
   Cross-module calls: SelectionPointerArray_ApplyType16MarkerCoordinates [gameplay/selection/runtime].
*/
void __thandor_void_preserve_eax_ecx_edx
SelectionMarkerCoordinates_ApplyType3
          (SelectionMarkerIndex selectionIndex,SelectionMarkerCoordinateValue32 valueC,
          SelectionMarkerCoordinateValue32 valueB,SelectionMarkerCoordinateValue32 valueA)

{
  SelectionPointerArray_ApplyType16MarkerCoordinates
            (3,valueC,valueB,valueA,
             &g_SelectionPlayerRuntimeBlockPointers[selectionIndex]->selection);
  return;
}


/* Address: 0x00560050.
   Ownership: gameplay/selection/overlay.
   Purpose: Binary entry is anchored by g_RuntimeRebaseCallbackTable5[1]@00563754. Typed parameters: p0
   selectionIndex→SelectionMarkerIndex_V342, p1 valueC→SelectionMarkerCoordinateValue32_V342, p2
   valueB→SelectionMarkerCoordinateValue32_V342, p3 valueA→SelectionMarkerCoordinateValue32_V342. Calling
   convention, exact VariableStorage serialization, function body bytes, control flow, globals, locals, and
   executable data remain unchanged.
   Cross-module calls: SelectionPointerArray_ApplyType16MarkerCoordinates [gameplay/selection/runtime].
*/
void __thandor_void_preserve_eax_ecx_edx
SelectionMarkerCoordinates_ApplyType4
          (SelectionMarkerIndex selectionIndex,SelectionMarkerCoordinateValue32 valueC,
          SelectionMarkerCoordinateValue32 valueB,SelectionMarkerCoordinateValue32 valueA)

{
  SelectionPointerArray_ApplyType16MarkerCoordinates
            (4,valueC,valueB,valueA,
             &g_SelectionPlayerRuntimeBlockPointers[selectionIndex]->selection);
  return;
}


/* Address: 0x00560080.
   Ownership: gameplay/selection/overlay.
   Purpose: Binary entry is anchored by g_RuntimeRebaseCallbackTable5[2]@00563754. Typed parameters: p0
   selectionIndex→SelectionMarkerIndex_V342, p1 valueC→SelectionMarkerCoordinateValue32_V342, p2
   valueB→SelectionMarkerCoordinateValue32_V342, p3 valueA→SelectionMarkerCoordinateValue32_V342. Calling
   convention, exact VariableStorage serialization, function body bytes, control flow, globals, locals, and
   executable data remain unchanged.
   Cross-module calls: SelectionPointerArray_ApplyType16MarkerCoordinates [gameplay/selection/runtime].
*/
void __thandor_void_preserve_eax_ecx_edx
SelectionMarkerCoordinates_ApplyType5
          (SelectionMarkerIndex selectionIndex,SelectionMarkerCoordinateValue32 valueC,
          SelectionMarkerCoordinateValue32 valueB,SelectionMarkerCoordinateValue32 valueA)

{
  SelectionPointerArray_ApplyType16MarkerCoordinates
            (5,valueC,valueB,valueA,
             &g_SelectionPlayerRuntimeBlockPointers[selectionIndex]->selection);
  return;
}


/* Address: 0x005600B0.
   Ownership: gameplay/selection/overlay.
   Purpose: Binary entry is anchored by g_RuntimeRebaseCallbackTable5[3]@00563754. Typed parameters: p0
   selectionIndex→SelectionMarkerIndex_V342, p1 valueC→SelectionMarkerCoordinateValue32_V342, p2
   valueB→SelectionMarkerCoordinateValue32_V342, p3 valueA→SelectionMarkerCoordinateValue32_V342. Calling
   convention, exact VariableStorage serialization, function body bytes, control flow, globals, locals, and
   executable data remain unchanged.
   Cross-module calls: SelectionPointerArray_ApplyType16MarkerCoordinates [gameplay/selection/runtime].
*/
void __thandor_void_preserve_eax_ecx_edx
SelectionMarkerCoordinates_ApplyType6
          (SelectionMarkerIndex selectionIndex,SelectionMarkerCoordinateValue32 valueC,
          SelectionMarkerCoordinateValue32 valueB,SelectionMarkerCoordinateValue32 valueA)

{
  SelectionPointerArray_ApplyType16MarkerCoordinates
            (6,valueC,valueB,valueA,
             &g_SelectionPlayerRuntimeBlockPointers[selectionIndex]->selection);
  return;
}


/* Address: 0x005600E0.
   Ownership: gameplay/selection/overlay.
   Purpose: Binary entry is anchored by g_RuntimeRebaseCallbackTable5[4]@00563754. Typed parameters: p0
   selectionIndex→SelectionMarkerIndex_V342, p1 valueC→SelectionMarkerCoordinateValue32_V342, p2
   valueB→SelectionMarkerCoordinateValue32_V342, p3 valueA→SelectionMarkerCoordinateValue32_V342. Calling
   convention, exact VariableStorage serialization, function body bytes, control flow, globals, locals, and
   executable data remain unchanged.
   Cross-module calls: SelectionPointerArray_ApplyType16MarkerCoordinates [gameplay/selection/runtime].
*/
void __thandor_void_preserve_eax_ecx_edx
SelectionMarkerCoordinates_ApplyType7
          (SelectionMarkerIndex selectionIndex,SelectionMarkerCoordinateValue32 valueC,
          SelectionMarkerCoordinateValue32 valueB,SelectionMarkerCoordinateValue32 valueA)

{
  SelectionPointerArray_ApplyType16MarkerCoordinates
            (7,valueC,valueB,valueA,
             &g_SelectionPlayerRuntimeBlockPointers[selectionIndex]->selection);
  return;
}


/* Address: 0x00568210.
   Ownership: gameplay/selection/overlay.
   Purpose: Avoids duplicate transient markers, interpolates terrain height, creates the effect, and records it in
   the in-game overlay marker array. Storage remains one signed 32-bit word. Typed parameters: p0 scaleQ12→Q12.
   Calling convention, storage, body bytes, control flow, and executable data remain unchanged. Typed parameters:
   p2 worldYQ12→Q12, p3 worldXQ12→Q12.
   Cross-module calls: FieldGrid_InterpolateTopSurfaceHeight [world/terrain/grid],
   EffectRuntimePool_CreateInstanceFromDefinitionCf [world/effects/runtime].
*/
void __thandor_void_preserve_eax_ecx_edx
InGameWorldOverlay_EnsureTransientEffectMarkerAtPoint
          (Q12 scaleQ12,void *sourceWorldNode,Q12 worldYQ12,Q12 worldXQ12,void *effectDefinition,
          void *inGameRuntime)

{
  GraphicsWorldCoordinateQ12 *pGVar1;
  ModelRuntimeNode *pMVar2;
  uint uVar3;
  int iVar4;
  int iVar5;
  int *piVar6;
  FieldGridHeightEaxCf5 FVar7;
  EffectRuntimeCreateEaxCf5 EVar8;
  
  iVar4 = g_InGameCommandTargetTransientEffectMarkerCount;
  if ((worldXQ12 != *(int *)((int)sourceWorldNode + 0x94)) ||
     (worldYQ12 != *(int *)((int)sourceWorldNode + 0x98))) {
    piVar6 = (int *)0x562ecc;
    for (iVar5 = g_InGameCommandTargetTransientEffectMarkerCount; iVar5 != 0; iVar5 = iVar5 + -1) {
      if ((worldXQ12 == *(int *)(*(int *)(*piVar6 + 4) + 0x94)) &&
         (worldYQ12 == *(int *)(*(int *)(*piVar6 + 4) + 0x98))) {
        return;
      }
      piVar6 = piVar6 + 1;
    }
    FVar7 = FieldGrid_InterpolateTopSurfaceHeight
                      (worldYQ12,worldXQ12,*(FieldGridAsset **)((int)inGameRuntime + 0x54));
    EVar8 = EffectRuntimePool_CreateInstanceFromDefinitionCf
                      (EFFECT_RUNTIME_COMPLETION_NONE,(EffectRuntimeOwnerReference4)0x0,0,0x4000,0,
                       FVar7.heightQ12,worldYQ12,worldXQ12,effectDefinition,inGameRuntime);
    *(EffectRuntimeSlot **)(iVar4 * 4 + 0x562ecc) = EVar8.effectRuntime;
    pMVar2 = ((EVar8.effectRuntime)->modelNodeOrSavedOffset).modelNode;
    g_InGameCommandTargetTransientEffectMarkerCount =
         g_InGameCommandTargetTransientEffectMarkerCount + 1;
    uVar3 = pMVar2->subtreeBoundingRadiusQ12;
    pMVar2->tintArgb = 0xffffffff;
    if ((scaleQ12 != 0x1000) && (uVar3 != 0)) {
      pGVar1 = &(pMVar2->worldTransform).translation.z;
      *pGVar1 = *pGVar1 + 0x144;
      pMVar2->runtimeFlags = pMVar2->runtimeFlags | 0x800;
      pMVar2->modelScaleQ12 = (Q12)(((ulonglong)(uint)scaleQ12 * 0x1a00) / (ulonglong)uVar3);
    }
  }
  return;
}


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
void InGameWorldOverlay_RebuildOrReleaseTransientMarkersCf
               (GraphicsBooleanState releaseMode,void *inGameRuntime)

{
  PackedArgb32 *pPVar1;
  dword dVar2;
  ArmyPlacementCandidateCount AVar3;
  GameEntityRuntime *entityRuntime1;
  int extraout_EAX;
  Q12 worldZQ12;
  EffectDefinition *effectDefinition_00;
  uint uVar4;
  int extraout_EAX_00;
  undefined4 extraout_ECX;
  Q12 worldXQ12;
  Q12 worldYQ12;
  undefined4 extraout_ECX_00;
  undefined4 extraout_ECX_01;
  int extraout_ECX_02;
  int iVar5;
  int extraout_ECX_03;
  int extraout_ECX_04;
  int extraout_ECX_05;
  int extraout_ECX_06;
  int extraout_ECX_07;
  int extraout_ECX_08;
  undefined4 extraout_EDX;
  Q12 worldYQ12_00;
  Q12 worldXQ12_00;
  EffectDefinition *effectDefinition;
  void *extraout_EDX_00;
  void *extraout_EDX_01;
  void *extraout_EDX_02;
  void *pvVar6;
  void *extraout_EDX_03;
  void *extraout_EDX_04;
  WorldRuntimeNode *node;
  WorldRuntimeNode *node_00;
  PackedArgb32 PVar7;
  GameEntityRuntime **ppGVar8;
  dword *pdVar9;
  int *piVar10;
  bool bVar11;
  bool bVar12;
  undefined1 uVar13;
  undefined8 uVar14;
  PckArmyAssetIdCatalog armyAssetId;
  WorldRuntimeContext *worldRuntime;
  GameEntityRuntime *entityRuntime2;
  ModelRuntimeNode *modelNode1;
  
  if ((*(uint *)((int)inGameRuntime + 0x48) & 8) != 0) {
    return;
  }
  if ((*(uint *)((int)inGameRuntime + 0x4c) & 0x10) != 0) {
    return;
  }
  if ((g_UiCommandRuntimeFlags & 0x100) != 0) {
    return;
  }
  bVar11 = false;
  if ((g_UiCommandRuntimeFlags & 0x20) == 0) {
    SelectionInfo_ValidateOwnerType16AndAnyActiveCf
              (*(FactionRuntimeIndex *)((int)inGameRuntime + 0x50));
    if (!bVar11) {
      if (releaseMode == GRAPHICS_STATE_DISABLED) {
        g_InGameCommandPreviewArmyRuntime = (GameEntityRuntime *)0x0;
        if ((((g_InGamePointerInteractionStateFlags & 3) == 0) &&
            (g_InGameCommandPreviewArmyAssetId != 0)) &&
           ((bVar11 = g_InGameCommandPreviewSurfaceHeightQ12OrSentinel < 0x7fffffff,
            g_InGameCommandPreviewSurfaceHeightQ12OrSentinel != 0x7fffffff &&
            (entityRuntime1 =
                  (GameEntityRuntime *)
                  ArmyRuntime_CreateInstanceFromAssetCf
                            (1,g_InGameCommandPreviewHeading16,g_InGameCommandPreviewWorldXQ12,
                             g_InGameCommandPreviewWorldYQ12,
                             *(FactionRuntimeIndex *)((int)inGameRuntime + 0x50),
                             g_InGameCommandPreviewArmyAssetId,inGameRuntime), !bVar11)))) {
          modelNode1 = (entityRuntime1->common).ownership.modelNode;
          g_InGameCommandPreviewArmyRuntime = entityRuntime1;
          modelNode1->tintArgb = 0xcfffffff;
          ModelNodeRuntime_RebuildTransformsFromRoot(extraout_ECX,extraout_EDX,modelNode1);
        }
      }
      else if (g_InGameCommandPreviewArmyRuntime != (GameEntityRuntime *)0x0) {
        ArmyRuntime_DestroyInstanceAndRefreshUi(inGameRuntime,g_InGameCommandPreviewArmyRuntime);
        g_InGameCommandPreviewArmyRuntime = (GameEntityRuntime *)0x0;
      }
    }
  }
  else if (releaseMode == GRAPHICS_STATE_DISABLED) {
    g_InGamePlacementPreviewArmyRuntime = (GameEntityRuntime *)0x0;
    if ((g_InGamePendingPlacementArmyAsset != 0) &&
       (bVar11 = (uint)g_InGamePlacementSurfaceHeightQ12OrSentinel < 0x7fffffff,
       g_InGamePlacementSurfaceHeightQ12OrSentinel != 0x7fffffff)) {
      PVar7 = 0xcfffffff;
      g_ArmyPlacementAcceptedCandidateCount = 1;
      ArmyPlacement_ValidateAssetAtPointAndCellCornersCf
                (0,g_InGamePlacementHeading16,g_InGamePlacementWorldXQ12,g_InGamePlacementWorldYQ12,
                 *(ArmyPlacementContext *)(g_InGamePendingPlacementArmyAsset + 8),
                 *(FactionRuntimeIndex *)((int)inGameRuntime + 0x50),inGameRuntime);
      AVar3 = g_ArmyPlacementAcceptedCandidateCount;
      bVar12 = false;
      if (bVar11) {
        bVar12 = g_ArmyPlacementAcceptedCandidateCount == 0;
        g_ArmyPlacementAcceptedCandidateCount = 0;
        if (AVar3 < 2) goto InGameWorldOverlay_RefreshTransientEffectMarkers;
        PVar7 = 0x4fffffff;
      }
      armyAssetId = *(PckArmyAssetIdCatalog *)(extraout_EAX + 8);
      g_ArmyPlacementAcceptedCandidateCount = 1;
      worldRuntime = inGameRuntime;
      ArmyPlacement_DispatchAssetAtFieldPoint
                (1,0,g_InGamePlacementHeading16,worldYQ12_00,worldXQ12,
                 *(PckArmyAssetIdCatalog *)(extraout_EAX + 8),
                 *(ArmyPlacementContext *)((int)inGameRuntime + 0x50),inGameRuntime);
      bVar11 = false;
      if ((bVar12) &&
         (bVar11 = g_ArmyPlacementAcceptedCandidateCount == 0,
         g_ArmyPlacementAcceptedCandidateCount < 2)) {
        bVar11 = false;
        PVar7 = PVar7 & 0xff707070;
      }
      g_ArmyPlacementAcceptedCandidateCount = 0;
      entityRuntime1 =
           (GameEntityRuntime *)
           ArmyRuntime_CreateInstanceFromAssetCf
                     (1,g_InGamePlacementHeading16,worldXQ12_00,worldYQ12,
                      *(FactionRuntimeIndex *)((int)inGameRuntime + 0x50),armyAssetId,worldRuntime);
      if (!bVar11) {
        modelNode1 = (entityRuntime1->common).ownership.modelNode;
        piVar10 = (entityRuntime1->common).ownership.definitionOrClassRecord;
        g_InGamePlacementPreviewArmyRuntime = entityRuntime1;
        modelNode1->tintArgb = PVar7;
        iVar5 = *piVar10;
        ModelNodeRuntime_RebuildTransformsFromRoot(extraout_ECX_00,piVar10,modelNode1);
        if (((*(int *)(iVar5 + 0x4c) == 0xd) && (3 < modelNode1->childCount)) &&
           (modelNode1->childNodes[3] != (ModelRuntimeNode *)0x0)) {
          pPVar1 = &modelNode1->childNodes[3]->tintArgb;
          *pPVar1 = *pPVar1 | 0xff000000;
        }
      }
    }
  }
  else if (g_InGamePlacementPreviewArmyRuntime != (GameEntityRuntime *)0x0) {
    ArmyRuntime_DestroyInstanceAndRefreshUi(inGameRuntime,g_InGamePlacementPreviewArmyRuntime);
    g_InGamePlacementPreviewArmyRuntime = (GameEntityRuntime *)0x0;
  }
InGameWorldOverlay_RefreshTransientEffectMarkers:
  uVar13 = false;
  if (releaseMode == GRAPHICS_STATE_DISABLED) {
    EffectDefinitionRegistry_FindByIdWithErrorCf(EFF_0143_EGATH0);
    if (!(bool)uVar13) {
      iVar5 = *(int *)((int)inGameRuntime + 0xd8);
      if (iVar5 == 0) {
        return;
      }
      do {
        if (((*(int *)(iVar5 + 0xa4) == 0) &&
            (piVar10 = *(int **)(iVar5 + 0x48), *(int *)(*piVar10 + 0x4c) == 0xd)) &&
           (((piVar10[0x3b] & 0x800U) != 0 &&
            (uVar13 = *(uint *)(piVar10[2] + 0xc) < *(uint *)((int)inGameRuntime + 0x50),
            *(uint *)(piVar10[2] + 0xc) == *(uint *)((int)inGameRuntime + 0x50))))) {
          worldZQ12 = FieldGrid_InterpolateTopSurfaceHeight
                                (piVar10[0x1f],piVar10[0x1e],
                                 *(FieldGridAsset **)((int)inGameRuntime + 0x54));
          uVar14 = EffectRuntimePool_CreateInstanceFromDefinitionCf
                             (extraout_ECX_01,effectDefinition,EFFECT_RUNTIME_COMPLETION_NONE,0,0,
                              0x4000,0,worldZQ12,piVar10[0x1f],piVar10[0x1e],effectDefinition,
                              inGameRuntime);
          if (!(bool)uVar13) {
            g_UiAction1012SubresourceByState[extraout_ECX_02 + 0xb] = (dword)uVar14;
            *(undefined4 *)(*(int *)((dword)uVar14 + 4) + 0x58) = 0xffffffff;
            g_InGameOwnedEntityTransientEffectMarkerCount =
                 g_InGameOwnedEntityTransientEffectMarkerCount + 1;
            uVar13 = extraout_ECX_02 + 1U < 0x20;
            if (!(bool)uVar13) break;
          }
        }
        iVar5 = *(int *)(iVar5 + 4);
        uVar13 = 0;
      } while (iVar5 != 0);
    }
    EffectDefinitionRegistry_FindByIdWithErrorCf(EFF_0148_EWAYP0);
    if ((!(bool)uVar13) &&
       (effectDefinition_00 = EffectDefinitionRegistry_FindByIdWithErrorCf(EFF_0149_ETARG0),
       !(bool)uVar13)) {
      iVar5 = 0x20;
      pvVar6 = extraout_EDX_00;
      ppGVar8 = g_SelectionInfoEntitySlots->entries;
      do {
        entityRuntime1 = *ppGVar8;
        if ((entityRuntime1 != (GameEntityRuntime *)0x0) &&
           (*(int *)((int)inGameRuntime + 0x50) == (entityRuntime1->common).ownership.ownerIndex)) {
          if ((*(int *)(*(int *)(entityRuntime1->common).ownership.definitionOrClassRecord + 0x18)
               != 0) && (((entityRuntime1->common).commandFlags & 1) != 0)) {
            InGameWorldOverlay_EnsureTransientEffectMarkerAtPoint
                      (0x1000,(entityRuntime1->common).ownership.modelNode,
                       (entityRuntime1->common).pathCoordinate1Q12,
                       (entityRuntime1->common).pathCoordinate0Q12,pvVar6,inGameRuntime);
            if (0x7f < g_InGameCommandTargetTransientEffectMarkerCount) {
              return;
            }
            iVar5 = extraout_ECX_03;
            pvVar6 = extraout_EDX_01;
            if (((entityRuntime1->common).commandFlags & 8) != 0) {
              uVar4 = 0;
              do {
                InGameWorldOverlay_EnsureTransientEffectMarkerAtPoint
                          (0x1000,(entityRuntime1->common).ownership.modelNode,
                           *(Q12 *)((entityRuntime1->common).reservedC0_EB + uVar4 * 8 + 4),
                           *(Q12 *)((entityRuntime1->common).reservedC0_EB + uVar4 * 8),pvVar6,
                           inGameRuntime);
                uVar4 = extraout_EAX_00 + 1;
                if (0x7f < g_InGameCommandTargetTransientEffectMarkerCount) {
                  return;
                }
                iVar5 = extraout_ECX_04;
                pvVar6 = extraout_EDX_02;
              } while (uVar4 < *(uint *)((entityRuntime1->common).reservedA4_B7 + 4));
            }
          }
          if ((((entityRuntime1->common).commandTarget.targetFlags & 2) != 0) &&
             (InGameWorldOverlay_EnsureTransientEffectMarkerAtPoint
                        (0x1000,(entityRuntime1->common).ownership.modelNode,
                         (entityRuntime1->common).commandTarget.targetWorldYQ12,
                         (entityRuntime1->common).commandTarget.targetWorldXQ12,effectDefinition_00,
                         inGameRuntime), iVar5 = extraout_ECX_05, pvVar6 = extraout_EDX_03,
             0x7f < g_InGameCommandTargetTransientEffectMarkerCount)) {
            return;
          }
          entityRuntime2 = (entityRuntime1->common).commandTarget.targetEntity;
          if (((((entityRuntime1->common).commandTarget.targetFlags & 1) != 0) &&
              (entityRuntime2 != (GameEntityRuntime *)0x0)) &&
             (modelNode1 = (entityRuntime2->common).ownership.modelNode,
             InGameWorldOverlay_EnsureTransientEffectMarkerAtPoint
                       (*(Q12 *)(*(int *)(entityRuntime2->common).ownership.definitionOrClassRecord
                                + 0xdc),(entityRuntime1->common).ownership.modelNode,
                        (modelNode1->worldTransform).translation.y,
                        (modelNode1->worldTransform).translation.x,effectDefinition_00,inGameRuntime
                       ), iVar5 = extraout_ECX_06, pvVar6 = extraout_EDX_04,
             0x7f < g_InGameCommandTargetTransientEffectMarkerCount)) {
            return;
          }
        }
        ppGVar8 = ppGVar8 + 1;
        iVar5 = iVar5 + -1;
      } while (iVar5 != 0);
    }
  }
  else {
    pdVar9 = g_UiAction1012SubresourceByState + 0xb;
    if (g_InGameOwnedEntityTransientEffectMarkerCount != 0) {
      do {
        dVar2 = *pdVar9;
        InterpolationState_SetNegatedTargetAndRescaleProgress
                  (0,*(GraphicsShadingRuntimeRecord **)(*(int *)(dVar2 + 4) + 0x5c));
        WorldRuntime_UnlinkNodeFromOwnerListD8(node);
        *(undefined4 *)(dVar2 + 4) = 0;
        pdVar9 = pdVar9 + 1;
      } while (extraout_ECX_07 != 1);
      g_InGameOwnedEntityTransientEffectMarkerCount = 0;
    }
    piVar10 = (int *)0x562ecc;
    if (g_InGameCommandTargetTransientEffectMarkerCount != 0) {
      do {
        iVar5 = *piVar10;
        InterpolationState_SetNegatedTargetAndRescaleProgress
                  (0,*(GraphicsShadingRuntimeRecord **)(*(int *)(iVar5 + 4) + 0x5c));
        WorldRuntime_UnlinkNodeFromOwnerListD8(node_00);
        *(undefined4 *)(iVar5 + 4) = 0;
        piVar10 = piVar10 + 1;
      } while (extraout_ECX_08 != 1);
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
undefined8
SelectionOverlay_RenderSelectedArmyMetrics
          (UiPixelCoordinate param_1,UiPixelCoordinate param_2,UiPixelCoordinate param_3,
          UiPixelCoordinate param_4)

{
  GameEntityRuntime *pGVar1;
  ModelRuntimeNode *pMVar2;
  undefined4 in_EAX;
  int iVar3;
  int extraout_ECX;
  int extraout_ECX_00;
  undefined4 in_EDX;
  ArmyRuntimeSlot *armyRuntime;
  GameEntityRuntime **ppGVar4;
  
  iVar3 = 0x20;
  ppGVar4 = g_SelectionInfoEntitySlots->entries;
  do {
    pGVar1 = *ppGVar4;
    if ((pGVar1 != (GameEntityRuntime *)0x0) &&
       ((pMVar2 = (pGVar1->common).ownership.modelNode, (pMVar2->runtimeFlags & 4) != 0 ||
        (((pMVar2->runtimeFlags & 0x10) == 0 && ((pMVar2->runtimeFlags & 8) != 0)))))) {
      DAT_0052ce04 = 0x10000;
      DAT_0052ce08 = 0x10000;
      DAT_0052ce0c = -0x10000;
      DAT_0052ce10 = -0x10000;
      ModelProjectedBounds_AccumulateHierarchyRecursive(iVar3,pGVar1,&DAT_0052ce04,(int)pMVar2);
      iVar3 = extraout_ECX;
      if ((DAT_0052ce04 < DAT_0052ce0c) && (DAT_0052ce08 < DAT_0052ce10)) {
        SelectionPanel_RenderArmyRuntimeMetrics
                  (param_1,param_2,param_3,param_4,DAT_0052ce10,DAT_0052ce0c,DAT_0052ce08,
                   DAT_0052ce04,armyRuntime);
        iVar3 = extraout_ECX_00;
      }
    }
    ppGVar4 = ppGVar4 + 1;
    iVar3 = iVar3 + -1;
  } while (iVar3 != 0);
  return CONCAT44(in_EDX,in_EAX);
}

/* Address: 0x0052F1B0.
   Ownership: gameplay/selection/overlay.
   Purpose: Handles selection overlay render army metrics for entity.
   Cross-module calls: ModelProjectedBounds_AccumulateHierarchyRecursive [graphics/render/model],
   SelectionPanel_RenderArmyRuntimeMetrics [gameplay/selection/runtime].
*/
void __fastcall
SelectionOverlay_RenderArmyMetricsForEntity
          (undefined4 param_1,undefined4 param_2,UiPixelCoordinate param_3,UiPixelCoordinate param_4
          ,UiPixelCoordinate param_5,UiPixelCoordinate param_6,int param_7)

{
  int iVar1;
  void *pvVar2;
  GraphicsTextureSourceAsset *extraout_ECX;
  ArmyRuntimeSlot *armyRuntime;
  
  iVar1 = *(int *)(param_7 + 4);
  if (((*(uint *)(iVar1 + 0x4c) & 4) != 0) ||
     (((*(uint *)(iVar1 + 0x4c) & 0x10) == 0 && ((*(uint *)(iVar1 + 0x4c) & 8) != 0)))) {
    DAT_0052ce04 = 0x10000;
    DAT_0052ce08 = 0x10000;
    DAT_0052ce0c = -0x10000;
    DAT_0052ce10 = -0x10000;
    ModelProjectedBounds_AccumulateHierarchyRecursive(param_1,param_7,&DAT_0052ce04,iVar1);
    pvVar2 = g_SelectionPanelData;
    g_SelectionPanelData = pvVar2;
    if ((DAT_0052ce04 < DAT_0052ce0c) && (DAT_0052ce08 < DAT_0052ce10)) {
      g_SelectionPanelTextureSource = g_InfoPanelTextureSource;
      g_SelectionPanelData = g_InfoPanelData;
      SelectionPanel_RenderArmyRuntimeMetrics
                (param_3,param_4,param_5,param_6,DAT_0052ce10,DAT_0052ce0c,DAT_0052ce08,DAT_0052ce04
                 ,armyRuntime);
      g_SelectionPanelTextureSource = extraout_ECX;
      g_SelectionPanelData = pvVar2;
    }
  }
  return;
}

/* Address: 0x0052F2A0.
   Ownership: gameplay/selection/overlay.
   Purpose: Handles selection overlay draw bounds frame.
*/
undefined8
SelectionOverlay_DrawBoundsFrame
          (sdword param_1,sdword param_2,sdword param_3,sdword param_4,uint param_5,int param_6,
          uint param_7,int param_8)

{
  int iVar1;
  int iVar2;
  uint uVar3;
  undefined4 in_EAX;
  undefined4 in_EDX;
  bool bVar4;
  qword qVar5;
  undefined8 uVar6;
  
  iVar1 = param_8;
  uVar3 = param_7;
  if (param_6 <= param_8) {
    if (param_8 == param_6) goto LAB_0052f47a;
    param_8 = param_6;
    param_6 = iVar1;
  }
  bVar4 = param_7 < param_5;
  if ((int)param_5 <= (int)param_7) {
    if (param_7 == param_5) goto LAB_0052f47a;
    param_7 = param_5;
    param_5 = uVar3;
  }
  (*g_GraphicsFramebufferBeginAccess)();
  if (!bVar4) {
    qVar5 = (*g_GraphicsTextureSourceGetLogicalSize)(0xa4,g_SelectionPanelTextureSource);
    iVar1 = param_8 - (int)qVar5;
    iVar2 = param_7 - (int)(qVar5 >> 0x20);
    (*g_SelectionPanelBlitOpaque)
              (param_1,param_2,param_3,param_4,iVar2,iVar1,0xa4,g_SelectionPanelTextureSource,
               g_FramebufferAccess);
    (*g_SelectionPanelBlitOpaque)
              (param_1,param_2,param_3,param_4,iVar2,param_6,0xa6,g_SelectionPanelTextureSource,
               g_FramebufferAccess);
    (*g_SelectionPanelBlitOpaque)
              (param_1,param_2,param_3,param_4,param_5,iVar1,0xa9,g_SelectionPanelTextureSource,
               g_FramebufferAccess);
    qVar5 = (*g_SelectionPanelBlitOpaque)
                      (param_1,param_2,param_3,param_4,param_5,param_6,0xab,
                       g_SelectionPanelTextureSource,g_FramebufferAccess);
    iVar1 = iVar1 + (int)qVar5;
    (*g_SelectionPanelBlitClipped)
              (param_1,param_2,param_3,param_4,-0x80000000,param_6,iVar2,iVar1,0xa5,
               g_SelectionPanelTextureSource,g_FramebufferAccess);
    uVar6 = (*g_SelectionPanelBlitClipped)
                      (param_1,param_2,param_3,param_4,-0x80000000,param_6,param_5,iVar1,0xaa,
                       g_SelectionPanelTextureSource,g_FramebufferAccess);
    iVar2 = iVar2 + (int)((ulonglong)uVar6 >> 0x20);
    (*g_SelectionPanelBlitClipped)
              (param_1,param_2,param_3,param_4,param_5,-0x80000000,iVar2,iVar1 - (int)uVar6,0xa7,
               g_SelectionPanelTextureSource,g_FramebufferAccess);
    (*g_SelectionPanelBlitClipped)
              (param_1,param_2,param_3,param_4,param_5,-0x80000000,iVar2,param_6,0xa8,
               g_SelectionPanelTextureSource,g_FramebufferAccess);
    (*g_GraphicsFramebufferEndAccess)();
  }
LAB_0052f47a:
  return CONCAT44(in_EDX,in_EAX);
}

/* Address: 0x0052F490.
   Ownership: gameplay/selection/overlay.
   Purpose: Handles selection overlay draw marker adfor field grid terrain points.
   Cross-module calls: FieldGrid_GetNearestTerrainPoint [world/terrain/grid], FixedTransform_ApplyPoint
   [core/math/fixed], Graphics_ProjectViewPoint [graphics/core/runtime].
*/
undefined8
SelectionOverlay_DrawMarkerADForFieldGridTerrainPoints
          (sdword param_1,sdword param_2,sdword param_3,sdword param_4,int param_5,int *param_6,
          FieldGridAsset *param_7)

{
  longlong lVar1;
  longlong lVar2;
  undefined4 in_EAX;
  sdword extraout_ECX;
  undefined4 in_EDX;
  int iVar3;
  int iVar4;
  bool bVar5;
  qword qVar6;
  GraphicsProjectedPointEdxEax8 GVar7;
  dword arg6;
  GraphicsTextureSourceAsset *arg7;
  SoftwareFramebufferAccess *arg8;
  
  bVar5 = false;
  if (param_5 != 0) {
    (*g_GraphicsFramebufferBeginAccess)();
    if (!bVar5) {
      do {
        lVar1 = (longlong)(param_6[1] + *param_6 * 2) * 0x901;
        lVar2 = (longlong)param_6[1] * -1999;
        iVar3 = (int)((ulonglong)lVar2 >> 0x20);
        bVar5 = iVar3 << 0x13 < 0;
        qVar6 = FieldGrid_GetNearestTerrainPoint
                          (iVar3 << 0x14 | (uint)lVar2 >> 0xc,
                           (int)((ulonglong)lVar1 >> 0x20) << 0x13 | (uint)lVar1 >> 0xd,param_7);
        if (!bVar5) {
          g_GraphicsTransformScratchMatrix3x4.basisRow0[0] = (sdword)qVar6;
          g_GraphicsTransformScratchMatrix3x4.basisRow0[1] = extraout_ECX;
          g_GraphicsTransformScratchMatrix3x4.basisRow0[2] = (sdword)(qVar6 >> 0x20);
          FixedTransform_ApplyPoint
                    (&g_GraphicsTransformInputScratchVec3,
                     (GraphicsFixedVec3 *)&g_GraphicsTransformScratchMatrix3x4,
                     &g_ViewProjectionMatrixFixed);
          if (0x10 < g_GraphicsTransformInputScratchVec3.z) {
            GVar7 = Graphics_ProjectViewPoint(&g_GraphicsTransformInputScratchVec3);
            iVar3 = (int)GVar7 >> 0xc;
            iVar4 = (int)((longlong)GVar7 >> 0x2c);
            arg6 = 0xad;
            arg7 = g_SelectionPanelTextureSource;
            arg8 = g_FramebufferAccess;
            qVar6 = (*g_GraphicsTextureSourceGetLogicalSize)(0xad,g_SelectionPanelTextureSource);
            (*g_SelectionPanelBlitOpaque)
                      (param_1,param_2,param_3,param_4,iVar4 - (int)((longlong)qVar6 >> 0x21),
                       iVar3 - ((int)qVar6 >> 1),arg6,arg7,arg8);
          }
        }
        param_6 = param_6 + 2;
        param_5 = param_5 + -1;
      } while (param_5 != 0);
      (*g_GraphicsFramebufferEndAccess)();
    }
  }
  return CONCAT44(in_EDX,in_EAX);
}

/* Address: 0x0052F5A0.
   Ownership: gameplay/selection/overlay.
   Purpose: Handles selection overlay draw marker acfor world surface point.
   Cross-module calls: FieldGrid_GetNearestTerrainPoint [world/terrain/grid], FieldGrid_GetNearestTopSurfacePoint
   [world/terrain/grid], FixedTransform_ApplyPoint [core/math/fixed], Graphics_ProjectViewPoint
   [graphics/core/runtime].
*/
void __fastcall
SelectionOverlay_DrawMarkerACForWorldSurfacePoint
          (undefined4 param_1,undefined4 param_2,sdword param_3,sdword param_4,sdword param_5,
          sdword param_6,int param_7,Q12 param_8,Q12 param_9,FieldGridAsset *param_10)

{
  sdword extraout_ECX;
  sdword extraout_ECX_00;
  sdword sVar1;
  int iVar2;
  bool bVar3;
  qword qVar4;
  GraphicsProjectedPointEdxEax8 GVar5;
  
  bVar3 = false;
  if (param_7 == 0) {
    qVar4 = FieldGrid_GetNearestTerrainPoint(param_8,param_9,param_10);
    sVar1 = extraout_ECX_00;
    if (bVar3) {
      return;
    }
  }
  else {
    qVar4 = FieldGrid_GetNearestTopSurfacePoint(param_8,param_9,param_10);
    sVar1 = extraout_ECX;
    if (bVar3) {
      return;
    }
  }
  g_GraphicsTransformScratchMatrix3x4.basisRow0[2] = (sdword)(qVar4 >> 0x20);
  g_GraphicsTransformScratchMatrix3x4.basisRow0[0] = (sdword)qVar4;
  g_GraphicsTransformScratchMatrix3x4.basisRow0[1] = sVar1;
  FixedTransform_ApplyPoint
            (&g_GraphicsTransformInputScratchVec3,
             (GraphicsFixedVec3 *)&g_GraphicsTransformScratchMatrix3x4,&g_ViewProjectionMatrixFixed)
  ;
  GVar5 = Graphics_ProjectViewPoint(&g_GraphicsTransformInputScratchVec3);
  iVar2 = (int)(GVar5 >> 0x20);
  bVar3 = (iVar2 >> 0xb & 1U) != 0;
  (*g_GraphicsFramebufferBeginAccess)(param_8,param_9,param_10);
  if (!bVar3) {
    qVar4 = (*g_GraphicsTextureSourceGetLogicalSize)(0xac,g_SelectionPanelTextureSource);
    (*g_SelectionPanelBlitOpaque)
              (param_3,param_4,param_5,param_6,(iVar2 >> 0xc) - (int)((longlong)qVar4 >> 0x21),
               ((int)GVar5 >> 0xc) - ((int)qVar4 >> 1),0xac,g_SelectionPanelTextureSource,
               g_FramebufferAccess);
    (*g_GraphicsFramebufferEndAccess)();
  }
  return;
}

/* Address: 0x0052F680.
   Ownership: gameplay/selection/overlay.
   Purpose: Handles selection overlay draw marker aefor visible projected grid vertices.
*/
undefined8
SelectionOverlay_DrawMarkerAEForVisibleProjectedGridVertices
          (sdword param_1,sdword param_2,sdword param_3,sdword param_4,int param_5)

{
  uint uVar1;
  undefined4 in_EAX;
  int iVar2;
  uint uVar3;
  uint extraout_ECX;
  uint uVar4;
  undefined4 in_EDX;
  uint uVar5;
  int iVar6;
  int iVar7;
  int iVar8;
  undefined1 in_CF;
  qword qVar9;
  dword arg6;
  GraphicsTextureSourceAsset *arg7;
  SoftwareFramebufferAccess *arg8;
  undefined4 uStack_20;
  
  (*g_GraphicsFramebufferBeginAccess)();
  if (!(bool)in_CF) {
    iVar8 = 0;
    uVar1 = *(uint *)(param_5 + 0xb8);
    uVar3 = uVar1 >> 2;
    uVar5 = *(uint *)(param_5 + 0xbc) >> 2;
    iVar7 = param_5 + 0x280 + uVar1 * 0x80;
    uVar4 = uVar3;
    uStack_20 = iVar7;
    if ((g_UiCommandModeGColorVariantLimit & 0xff000000) != 0) {
      iVar8 = 0x20;
    }
    do {
      do {
        if ((*(uint *)(iVar7 + 0x50) & 0x200000) == 0) {
          iVar2 = *(int *)(iVar7 + 0xc + iVar8) >> 0xc;
          iVar6 = *(int *)(iVar7 + 0x10 + iVar8) >> 0xc;
          arg6 = 0xae;
          arg7 = g_SelectionPanelTextureSource;
          arg8 = g_FramebufferAccess;
          qVar9 = (*g_GraphicsTextureSourceGetLogicalSize)(0xae,g_SelectionPanelTextureSource);
          (*g_SelectionPanelBlitOpaque)
                    (param_1,param_2,param_3,param_4,iVar6 - (int)((longlong)qVar9 >> 0x21),
                     iVar2 - ((int)qVar9 >> 1),arg6,arg7,arg8);
          uVar4 = extraout_ECX;
        }
        iVar7 = iVar7 + 0x200;
        uVar4 = uVar4 - 1;
      } while (-1 < (int)uVar4);
      iVar7 = uStack_20 + uVar1 * 0x200;
      uVar5 = uVar5 - 1;
      uVar4 = uVar3;
      uStack_20 = iVar7;
    } while (-1 < (int)uVar5);
    (*g_GraphicsFramebufferEndAccess)();
  }
  return CONCAT44(in_EDX,in_EAX);
}

/* Address: 0x0052F780.
   Ownership: gameplay/selection/overlay.
   Purpose: Handles selection overlay draw marker afb0 for projected vertex state flags.
*/
undefined8
SelectionOverlay_DrawMarkerAFB0ForProjectedVertexStateFlags
          (sdword param_1,sdword param_2,sdword param_3,sdword param_4,int param_5)

{
  int iVar1;
  undefined4 in_EAX;
  int iVar2;
  int extraout_ECX;
  int extraout_ECX_00;
  int iVar3;
  undefined4 in_EDX;
  int iVar4;
  int iVar5;
  int iVar6;
  undefined1 in_CF;
  qword qVar7;
  dword arg6;
  GraphicsTextureSourceAsset *arg7;
  SoftwareFramebufferAccess *arg8;
  int iVar8;
  int iVar9;
  dword arg6_00;
  GraphicsTextureSourceAsset *arg7_00;
  SoftwareFramebufferAccess *arg8_00;
  int iStack_1c;
  
  (*g_GraphicsFramebufferBeginAccess)();
  if (!(bool)in_CF) {
    iVar1 = *(int *)(param_5 + 0xb8);
    iVar5 = *(int *)(param_5 + 0xbc);
    iVar6 = param_5 + 0x200;
    iVar3 = iVar1;
    iStack_1c = iVar6;
    do {
      do {
        if (((*(uint *)(iVar6 + 0x50) & 0x4000000) == 0) &&
           ((*(uint *)(iVar6 + 0x50) & 0x60000000) != 0)) {
          iVar2 = *(int *)(iVar6 + 0x2c) >> 0xc;
          iVar4 = *(int *)(iVar6 + 0x30) >> 0xc;
          arg6_00 = 0xb0;
          arg6 = 0xaf;
          arg7_00 = g_SelectionPanelTextureSource;
          arg8_00 = g_FramebufferAccess;
          if ((*(uint *)(iVar6 + 0x50) & 0x20000000) != 0) {
            arg7 = g_SelectionPanelTextureSource;
            arg8 = g_FramebufferAccess;
            iVar8 = iVar4;
            iVar9 = iVar2;
            qVar7 = (*g_GraphicsTextureSourceGetLogicalSize)(0xaf,g_SelectionPanelTextureSource);
            (*g_SelectionPanelBlitOpaque)
                      (param_1,param_2,param_3,param_4,iVar4 - (int)((longlong)qVar7 >> 0x21),
                       iVar2 - ((int)qVar7 >> 1),arg6,arg7,arg8);
            iVar3 = extraout_ECX;
            iVar4 = iVar8;
            iVar2 = iVar9;
          }
          if ((*(uint *)(iVar6 + 0x50) & 0x40000000) != 0) {
            qVar7 = (*g_GraphicsTextureSourceGetLogicalSize)(0xb0,g_SelectionPanelTextureSource);
            (*g_SelectionPanelBlitOpaque)
                      (param_1,param_2,param_3,param_4,iVar4 - (int)((longlong)qVar7 >> 0x21),
                       iVar2 - ((int)qVar7 >> 1),arg6_00,arg7_00,arg8_00);
            iVar3 = extraout_ECX_00;
          }
        }
        iVar6 = iVar6 + 0x80;
        iVar3 = iVar3 + -1;
      } while (iVar3 != 0);
      iVar6 = iStack_1c + iVar1 * 0x80;
      iVar5 = iVar5 + -1;
      iVar3 = iVar1;
      iStack_1c = iVar6;
    } while (iVar5 != 0);
    (*g_GraphicsFramebufferEndAccess)();
  }
  return CONCAT44(in_EDX,in_EAX);
}

/* Address: 0x0052F8C0.
   Ownership: gameplay/selection/overlay.
   Purpose: Handles selection overlay draw marker b1 b2 for projected vertex mask1800.
*/
undefined8
SelectionOverlay_DrawMarkerB1B2ForProjectedVertexMask1800
          (sdword param_1,sdword param_2,sdword param_3,sdword param_4,byte param_5,int param_6)

{
  int iVar1;
  undefined4 in_EAX;
  int iVar2;
  int extraout_ECX;
  int extraout_ECX_00;
  int iVar3;
  undefined4 in_EDX;
  int iVar4;
  int iVar5;
  int iVar6;
  uint uVar7;
  byte in_CF;
  bool bVar8;
  qword qVar9;
  dword arg6;
  GraphicsTextureSourceAsset *arg7;
  SoftwareFramebufferAccess *arg8;
  int iVar10;
  int iVar11;
  dword arg6_00;
  GraphicsTextureSourceAsset *arg7_00;
  SoftwareFramebufferAccess *arg8_00;
  int iStack_20;
  
  uVar7 = 0x800 << (param_5 & 0x1f);
  bVar8 = (param_5 & 0x1f) != 0;
  bVar8 = (bool)(!bVar8 & in_CF | (bVar8 && 0x800 << (param_5 & 0x1f) - 1 < 0));
  (*g_GraphicsFramebufferBeginAccess)();
  if (!bVar8) {
    iVar1 = *(int *)(param_6 + 0xb8);
    iVar5 = *(int *)(param_6 + 0xbc);
    iVar6 = param_6 + 0x200;
    iVar3 = iVar1;
    iStack_20 = iVar6;
    do {
      do {
        if (((*(uint *)(iVar6 + 0x50) & 0x200000) == 0) && ((*(uint *)(iVar6 + 0x50) & 0x1800) != 0)
           ) {
          iVar2 = *(int *)(iVar6 + 0xc) >> 0xc;
          iVar4 = *(int *)(iVar6 + 0x10) >> 0xc;
          arg6_00 = 0xb2;
          arg6 = 0xb1;
          arg7_00 = g_SelectionPanelTextureSource;
          arg8_00 = g_FramebufferAccess;
          if ((*(uint *)(iVar6 + 0x50) & uVar7) != 0) {
            arg7 = g_SelectionPanelTextureSource;
            arg8 = g_FramebufferAccess;
            iVar10 = iVar4;
            iVar11 = iVar2;
            qVar9 = (*g_GraphicsTextureSourceGetLogicalSize)(0xb1,g_SelectionPanelTextureSource);
            (*g_SelectionPanelBlitOpaque)
                      (param_1,param_2,param_3,param_4,iVar4 - (int)((longlong)qVar9 >> 0x21),
                       iVar2 - ((int)qVar9 >> 1),arg6,arg7,arg8);
            iVar3 = extraout_ECX;
            iVar4 = iVar10;
            iVar2 = iVar11;
          }
          if ((*(uint *)(iVar6 + 0x50) & (uVar7 ^ 0x1800)) != 0) {
            qVar9 = (*g_GraphicsTextureSourceGetLogicalSize)(0xb2,g_SelectionPanelTextureSource);
            (*g_SelectionPanelBlitOpaque)
                      (param_1,param_2,param_3,param_4,iVar4 - (int)((longlong)qVar9 >> 0x21),
                       iVar2 - ((int)qVar9 >> 1),arg6_00,arg7_00,arg8_00);
            iVar3 = extraout_ECX_00;
          }
        }
        iVar6 = iVar6 + 0x80;
        iVar3 = iVar3 + -1;
      } while (iVar3 != 0);
      iVar6 = iStack_20 + iVar1 * 0x80;
      iVar5 = iVar5 + -1;
      iVar3 = iVar1;
      iStack_20 = iVar6;
    } while (iVar5 != 0);
    (*g_GraphicsFramebufferEndAccess)();
  }
  return CONCAT44(in_EDX,in_EAX);
}

/* Address: 0x0052FA20.
   Ownership: gameplay/selection/overlay.
   Purpose: Handles selection overlay draw marker affor projected vertex flag8000.
*/
void SelectionOverlay_DrawMarkerAFForProjectedVertexFlag8000
               (sdword param_1,sdword param_2,sdword param_3,sdword param_4,int param_5)

{
  int iVar1;
  int iVar2;
  int extraout_ECX;
  int iVar3;
  int iVar4;
  int iVar5;
  undefined1 in_CF;
  qword qVar6;
  dword arg6;
  GraphicsTextureSourceAsset *arg7;
  SoftwareFramebufferAccess *arg8;
  undefined4 uStack_1c;
  
  (*g_GraphicsFramebufferBeginAccess)();
  if (!(bool)in_CF) {
    iVar1 = *(int *)(param_5 + 0xb8);
    iVar4 = *(int *)(param_5 + 0xbc);
    iVar5 = param_5 + 0x200;
    iVar2 = iVar1;
    uStack_1c = iVar5;
    do {
      do {
        if (((*(uint *)(iVar5 + 0x50) & 0x8000) != 0) && ((*(uint *)(iVar5 + 0x50) & 0x200000) == 0)
           ) {
          iVar2 = *(int *)(iVar5 + 0xc) >> 0xc;
          iVar3 = *(int *)(iVar5 + 0x10) >> 0xc;
          arg6 = 0xaf;
          arg7 = g_SelectionPanelTextureSource;
          arg8 = g_FramebufferAccess;
          qVar6 = (*g_GraphicsTextureSourceGetLogicalSize)(0xaf,g_SelectionPanelTextureSource);
          (*g_SelectionPanelBlitOpaque)
                    (param_1,param_2,param_3,param_4,iVar3 - (int)((longlong)qVar6 >> 0x21),
                     iVar2 - ((int)qVar6 >> 1),arg6,arg7,arg8);
          iVar2 = extraout_ECX;
        }
        iVar5 = iVar5 + 0x80;
        iVar2 = iVar2 + -1;
      } while (iVar2 != 0);
      iVar5 = uStack_1c + iVar1 * 0x80;
      iVar4 = iVar4 + -1;
      iVar2 = iVar1;
      uStack_1c = iVar5;
    } while (iVar4 != 0);
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
undefined4
SelectionMarkerCoordinates_ApplyType3
          (SelectionMarkerIndex selectionIndex,SelectionMarkerCoordinateValue32 valueC,
          SelectionMarkerCoordinateValue32 valueB,SelectionMarkerCoordinateValue32 valueA)

{
  undefined4 in_EAX;
  
  SelectionPointerArray_ApplyType16MarkerCoordinates
            (3,valueC,valueB,valueA,
             &g_SelectionPlayerRuntimeBlockPointers[selectionIndex]->selection);
  return in_EAX;
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
undefined4
SelectionMarkerCoordinates_ApplyType4
          (SelectionMarkerIndex selectionIndex,SelectionMarkerCoordinateValue32 valueC,
          SelectionMarkerCoordinateValue32 valueB,SelectionMarkerCoordinateValue32 valueA)

{
  undefined4 in_EAX;
  
  SelectionPointerArray_ApplyType16MarkerCoordinates
            (4,valueC,valueB,valueA,
             &g_SelectionPlayerRuntimeBlockPointers[selectionIndex]->selection);
  return in_EAX;
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
undefined4
SelectionMarkerCoordinates_ApplyType5
          (SelectionMarkerIndex selectionIndex,SelectionMarkerCoordinateValue32 valueC,
          SelectionMarkerCoordinateValue32 valueB,SelectionMarkerCoordinateValue32 valueA)

{
  undefined4 in_EAX;
  
  SelectionPointerArray_ApplyType16MarkerCoordinates
            (5,valueC,valueB,valueA,
             &g_SelectionPlayerRuntimeBlockPointers[selectionIndex]->selection);
  return in_EAX;
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
undefined4
SelectionMarkerCoordinates_ApplyType6
          (SelectionMarkerIndex selectionIndex,SelectionMarkerCoordinateValue32 valueC,
          SelectionMarkerCoordinateValue32 valueB,SelectionMarkerCoordinateValue32 valueA)

{
  undefined4 in_EAX;
  
  SelectionPointerArray_ApplyType16MarkerCoordinates
            (6,valueC,valueB,valueA,
             &g_SelectionPlayerRuntimeBlockPointers[selectionIndex]->selection);
  return in_EAX;
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
undefined4
SelectionMarkerCoordinates_ApplyType7
          (SelectionMarkerIndex selectionIndex,SelectionMarkerCoordinateValue32 valueC,
          SelectionMarkerCoordinateValue32 valueB,SelectionMarkerCoordinateValue32 valueA)

{
  undefined4 in_EAX;
  
  SelectionPointerArray_ApplyType16MarkerCoordinates
            (7,valueC,valueB,valueA,
             &g_SelectionPlayerRuntimeBlockPointers[selectionIndex]->selection);
  return in_EAX;
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
void InGameWorldOverlay_EnsureTransientEffectMarkerAtPoint
               (Q12 scaleQ12,void *sourceWorldNode,Q12 worldYQ12,Q12 worldXQ12,
               void *effectDefinition,void *inGameRuntime)

{
  int iVar1;
  Q12 worldZQ12;
  int iVar2;
  undefined4 extraout_ECX;
  undefined4 extraout_EDX;
  int *piVar3;
  undefined8 uVar4;
  
  iVar1 = g_InGameCommandTargetTransientEffectMarkerCount;
  if ((worldXQ12 != *(int *)((int)sourceWorldNode + 0x94)) ||
     (worldYQ12 != *(int *)((int)sourceWorldNode + 0x98))) {
    piVar3 = (int *)0x562ecc;
    for (iVar2 = g_InGameCommandTargetTransientEffectMarkerCount; iVar2 != 0; iVar2 = iVar2 + -1) {
      if ((worldXQ12 == *(int *)(*(int *)(*piVar3 + 4) + 0x94)) &&
         (worldYQ12 == *(int *)(*(int *)(*piVar3 + 4) + 0x98))) {
        return;
      }
      piVar3 = piVar3 + 1;
    }
    worldZQ12 = FieldGrid_InterpolateTopSurfaceHeight
                          (worldYQ12,worldXQ12,*(FieldGridAsset **)((int)inGameRuntime + 0x54));
    uVar4 = EffectRuntimePool_CreateInstanceFromDefinitionCf
                      (extraout_ECX,extraout_EDX,EFFECT_RUNTIME_COMPLETION_NONE,0,0,0x4000,0,
                       worldZQ12,worldYQ12,worldXQ12,effectDefinition,inGameRuntime);
    *(int *)(iVar1 * 4 + 0x562ecc) = (int)uVar4;
    iVar1 = *(int *)((int)uVar4 + 4);
    g_InGameCommandTargetTransientEffectMarkerCount =
         g_InGameCommandTargetTransientEffectMarkerCount + 1;
    *(undefined4 *)(iVar1 + 0x58) = 0xffffffff;
    if ((scaleQ12 != 0x1000) && (*(uint *)(iVar1 + 0x54) != 0)) {
      *(int *)(iVar1 + 0x9c) = *(int *)(iVar1 + 0x9c) + 0x144;
      *(uint *)(iVar1 + 0x4c) = *(uint *)(iVar1 + 0x4c) | 0x800;
      *(int *)(iVar1 + 0xc0) =
           (int)(((ulonglong)(uint)scaleQ12 * 0x1a00) / (ulonglong)*(uint *)(iVar1 + 0x54));
    }
  }
  return;
}

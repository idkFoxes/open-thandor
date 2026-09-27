/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/src/gameplay/selection/overlay.c
 * Reverse engineering by idkFoxes 2026
 */

#include <thandor/gameplay/selection/overlay.h>
#include <thandor/thandor.h>

/* Implementation ownership: gameplay/selection/overlay. */

/* Address: 0x00568300.
   World overlay callback: builds (releaseMode == GRAPHICS_STATE_DISABLED) or releases the transient objects
   drawn over the map - the ghost army previewing a placement or command-mode command, EGATH0 markers at the
   movement target of own class-13 armies (runtime flag 0x800), and the waypoint (EWAYP0) and target (ETARG0)
   markers of the selected own armies.
*/
void __thandor_void_preserve_eax_ecx_edx
InGameWorldOverlay_RebuildOrReleaseTransientMarkers
          (GraphicsBooleanState releaseMode,WorldRuntimeContext *worldRuntime)

{
  PackedArgb32 *childTint;
  ModelRuntimeNode *currentModelNode;
  int *classRecord;
  ArmyRuntimeSlot *armySlot;
  EffectRuntimeSlot *effectSlot;
  int32_t pendingPlacementAsset;
  ArmyPlacementCandidateCount acceptedCandidateCount;
  GameEntityRuntime *entityRuntime;
  Q12 worldXQ12;
  uint32_t indexOrCount;
  int recordOrCount;
  Q12 validatedWorldYQ12;
  PackedArgb32 previewTint;
  GameEntityRuntime **selectionSlotCursor;
  ModelRuntimeNode *modelNodeCursor;
  EffectRuntimeSlot **ownedEffectCursor;
  EffectRuntimeSlot **commandTargetEffectCursor;
  bool checkResult;
  ArmyRuntimeCreateResult createdArmy;
  PlacementDispatchResult dispatchResult;
  EffectDefinitionResult markerDefinition;
  HeightSampleResult surfaceHeight;
  EffectCreateResult createdEffect;
  EffectDefinitionResult targetDefinition;
  PckArmyAssetIdCatalog armyAssetId;
  WorldRuntimeContext *worldRuntimeCopy;
  GameEntityRuntime *commandTargetEntity;
  
  pendingPlacementAsset = g_InGamePendingPlacementArmyAsset;
  if (((worldRuntime->interaction).interactionFlags48 & 8) != 0) {
    return;
  }
  if ((worldRuntime->runtimeFlags & 0x10) != 0) {
    return;
  }
  if ((g_UiCommandRuntimeFlags & UI_COMMAND_RUNTIME_FLAG_WORLD_INPUT_DISABLED) != 0) {
    return;
  }
  if ((g_UiCommandRuntimeFlags & UI_COMMAND_RUNTIME_FLAG_PLACEMENT_PENDING) == 0) {
    checkResult = SelectionInfo_ValidateOwnerType16AndAnyActive
                       (worldRuntime->activeFactionRuntimeIndex);
    if (!checkResult) {
      /* command mode: ghost of the army the previewed pointer-mode command would create */
      if (releaseMode == GRAPHICS_STATE_DISABLED) {
        g_InGameCommandPreviewArmyRuntime = NULL;
        if ((((g_InGamePointerInteractionStateFlags &
               (WORLD_POINTER_STATE_OVER_OWN_ARMY | WORLD_POINTER_STATE_SELECTION_CAPTURE)) == 0) &&
            (g_InGameCommandPreviewArmyAssetId != 0)) &&
           (g_InGameCommandPreviewSurfaceHeightQ12OrSentinel != WORLD_POINTER_NO_HIT)) {
          createdArmy = ArmyRuntime_CreateInstanceFromAsset
                             (1,g_InGameCommandPreviewHeading16,g_InGameCommandPreviewWorldXQ12,
                              g_InGameCommandPreviewWorldYQ12,
                              worldRuntime->activeFactionRuntimeIndex,
                              g_InGameCommandPreviewArmyAssetId,worldRuntime);
          if (!createdArmy.failed) {
            currentModelNode = (((GameEntityRuntime *)createdArmy.armyRuntimeOrError)->common).ownership.modelNode;
            g_InGameCommandPreviewArmyRuntime = (GameEntityRuntime *)createdArmy.armyRuntimeOrError;
            currentModelNode->tintArgb = OVERLAY_PREVIEW_TINT_ARGB;
            ModelNodeRuntime_RebuildTransformsFromRoot(currentModelNode);
          }
        }
      }
      else if (g_InGameCommandPreviewArmyRuntime != NULL) {
        ArmyRuntime_DestroyInstanceAndRefreshUi(worldRuntime,g_InGameCommandPreviewArmyRuntime);
        g_InGameCommandPreviewArmyRuntime = NULL;
      }
    }
  }
  else if (releaseMode == GRAPHICS_STATE_DISABLED) {
    /* placement mode: ghost of the pending army at the (possibly snapped) placement point */
    g_InGamePlacementPreviewArmyRuntime = NULL;
    if ((g_InGamePendingPlacementArmyAsset != 0) &&
       (g_InGamePlacementSurfaceHeightQ12OrSentinel != WORLD_POINTER_NO_HIT)) {
      previewTint = OVERLAY_PREVIEW_TINT_ARGB;
      g_ArmyPlacementAcceptedCandidateCount = 1;
      checkResult = ArmyPlacement_ValidateAssetAtPointAndCellCorners
                         (0,g_InGamePlacementHeading16,g_InGamePlacementWorldXQ12,
                          g_InGamePlacementWorldYQ12,
                          *(ArmyPlacementContext *)(g_InGamePendingPlacementArmyAsset + 8),
                          worldRuntime->activeFactionRuntimeIndex,worldRuntime);
      acceptedCandidateCount = g_ArmyPlacementAcceptedCandidateCount;
      if (checkResult) {
        g_ArmyPlacementAcceptedCandidateCount = 0;
        if (acceptedCandidateCount < 2) goto InGameWorldOverlay_RefreshTransientEffectMarkers;
        previewTint = OVERLAY_PREVIEW_TINT_MULTI_CANDIDATE_ARGB;
      }
      armyAssetId = *(PckArmyAssetIdCatalog *)(pendingPlacementAsset + 8);
      /* ECX/EDX of the validator: the accepted (possibly snapped) point. */
      worldXQ12 = g_ArmyPlacementValidatedWorldXQ12;
      validatedWorldYQ12 = g_ArmyPlacementValidatedWorldYQ12;
      g_ArmyPlacementAcceptedCandidateCount = 1;
      worldRuntimeCopy = worldRuntime;
      dispatchResult = ArmyPlacement_DispatchAssetAtFieldPoint
                         (1,0,g_InGamePlacementHeading16,validatedWorldYQ12,worldXQ12,
                          *(PckArmyAssetIdCatalog *)(pendingPlacementAsset + 8),
                          worldRuntime->activeFactionRuntimeIndex,(UiRootNode *)worldRuntime);
      if ((dispatchResult.failed) && (g_ArmyPlacementAcceptedCandidateCount < 2)) {
        previewTint = previewTint & OVERLAY_PREVIEW_TINT_BLOCKED_MASK;
      }
      g_ArmyPlacementAcceptedCandidateCount = 0;
      createdArmy = ArmyRuntime_CreateInstanceFromAsset
                         (1,g_InGamePlacementHeading16,validatedWorldYQ12,worldXQ12,
                          worldRuntime->activeFactionRuntimeIndex,armyAssetId,worldRuntimeCopy);
      entityRuntime = (GameEntityRuntime *)createdArmy.armyRuntimeOrError;
      if (!createdArmy.failed) {
        currentModelNode = (entityRuntime->common).ownership.modelNode;
        classRecord = (entityRuntime->common).ownership.definitionOrClassRecord;
        g_InGamePlacementPreviewArmyRuntime = entityRuntime;
        currentModelNode->tintArgb = previewTint;
        recordOrCount = *classRecord;
        ModelNodeRuntime_RebuildTransformsFromRoot(currentModelNode);
        if (((*(int *)(recordOrCount + 0x4c) == 0xd) && (3 < currentModelNode->childCount)) &&
           (currentModelNode->childNodes[3] != NULL)) {
          /* class-13 armies keep child node 3 opaque */
          childTint = &currentModelNode->childNodes[3]->tintArgb;
          *childTint = *childTint | 0xff000000;
        }
      }
    }
  }
  else if (g_InGamePlacementPreviewArmyRuntime != NULL) {
    ArmyRuntime_DestroyInstanceAndRefreshUi(worldRuntime,g_InGamePlacementPreviewArmyRuntime);
    g_InGamePlacementPreviewArmyRuntime = NULL;
  }
InGameWorldOverlay_RefreshTransientEffectMarkers:
  if (releaseMode == GRAPHICS_STATE_DISABLED) {
    markerDefinition = EffectDefinitionRegistry_FindByIdWithError(EFF_0143_EGATH0);
    if (!markerDefinition.notFound) {
      modelNodeCursor = (ModelRuntimeNode *)worldRuntime->ownerListHead;
      indexOrCount = 0;
      if (modelNodeCursor == NULL) {
        return;
      }
      do {
        if (((modelNodeCursor->ownerClassId == WORLD_OWNER_RUNTIME_MODEL) &&
            (armySlot = (modelNodeCursor->runtimePayload).armyRuntime,
            ((armySlot->modelRuntimeOrSavedOffset).modelRuntime)->definitionValue9C_4C == 0xd)) &&
           (((armySlot->runtimeFlags & 0x800) != 0 &&
            ((armySlot->linkedEntityRuntime->common).ownership.ownerIndex ==
             worldRuntime->activeFactionRuntimeIndex)))) {
          surfaceHeight = FieldGrid_InterpolateTopSurfaceHeight
                             (armySlot->movementTarget1Q12,armySlot->movementTarget0Q12,
                              worldRuntime->fieldGrid);
          createdEffect = EffectRuntimePool_CreateInstanceFromDefinition
                             (EFFECT_RUNTIME_COMPLETION_NONE,THANDOR_BITCAST(int, EffectRuntimeOwnerReference4, 0x0),0,
                              0x4000,0,surfaceHeight.heightQ12,armySlot->movementTarget1Q12,
                              armySlot->movementTarget0Q12,markerDefinition.definitionOrError,worldRuntime);
          if (!createdEffect.failed) {
            g_InGameOwnedEntityTransientEffectMarkers[indexOrCount] = createdEffect.effectRuntime;
            indexOrCount = indexOrCount + 1;
            (((createdEffect.effectRuntime)->modelNodeOrSavedOffset).modelNode)->tintArgb = 0xffffffff;
            g_InGameOwnedEntityTransientEffectMarkerCount =
                 g_InGameOwnedEntityTransientEffectMarkerCount + 1;
            if (OVERLAY_OWNED_MARKER_CAPACITY - 1 < indexOrCount) break;
          }
        }
        modelNodeCursor = (ModelRuntimeNode *)(modelNodeCursor->common).nextNode;
      } while (modelNodeCursor != NULL);
    }
    markerDefinition = EffectDefinitionRegistry_FindByIdWithError(EFF_0148_EWAYP0);
    if (!markerDefinition.notFound) {
      targetDefinition = EffectDefinitionRegistry_FindByIdWithError(EFF_0149_ETARG0);
      if (!targetDefinition.notFound) {
        /* the markers use scale 0x1000 (1.0 in Q12) except at an army target, which uses the target's own
           marker scale (class record +0xDC); at most OVERLAY_COMMAND_TARGET_MARKER_CAPACITY markers */
        recordOrCount = 0x20; /* selection-info slots */
        selectionSlotCursor = g_SelectionInfoEntitySlots->entries;
        do {
          entityRuntime = *selectionSlotCursor;
          if ((entityRuntime != NULL) &&
             (worldRuntime->activeFactionRuntimeIndex == (entityRuntime->common).ownership.ownerIndex)) {
            if ((*(int *)(*(int *)(entityRuntime->common).ownership.definitionOrClassRecord + 0x18) != 0)
               && (((entityRuntime->common).commandFlags & 1) != 0)) {
              InGameWorldOverlay_EnsureTransientEffectMarkerAtPoint
                        (0x1000,(entityRuntime->common).ownership.modelNode,
                         (entityRuntime->common).pathCoordinate1Q12,(entityRuntime->common).pathCoordinate0Q12,
                         markerDefinition.definitionOrError,worldRuntime);
              if (OVERLAY_COMMAND_TARGET_MARKER_CAPACITY - 1 < g_InGameCommandTargetTransientEffectMarkerCount) {
                return;
              }
              if (((entityRuntime->common).commandFlags & 8) != 0) {
                /* further waypoints: (x, y) Q12 pairs at +0xC0, their count at +0xA8 */
                indexOrCount = 0;
                do {
                  InGameWorldOverlay_EnsureTransientEffectMarkerAtPoint
                            (0x1000,(entityRuntime->common).ownership.modelNode,
                             *(Q12 *)((entityRuntime->common).reservedC0_EB + indexOrCount * 8 + 4),
                             *(Q12 *)((entityRuntime->common).reservedC0_EB + indexOrCount * 8),
                             markerDefinition.definitionOrError,worldRuntime);
                  indexOrCount++;
                  if (OVERLAY_COMMAND_TARGET_MARKER_CAPACITY - 1 < g_InGameCommandTargetTransientEffectMarkerCount) {
                    return;
                  }
                } while (indexOrCount < *(uint32_t *)((entityRuntime->common).reservedA4_B7 + 4));
              }
            }
            if ((((entityRuntime->common).commandTarget.targetFlags & 2) != 0) &&
               (InGameWorldOverlay_EnsureTransientEffectMarkerAtPoint
                          (0x1000,(entityRuntime->common).ownership.modelNode,
                           (entityRuntime->common).commandTarget.targetWorldYQ12,
                           (entityRuntime->common).commandTarget.targetWorldXQ12,targetDefinition.definitionOrError,
                           worldRuntime),
                OVERLAY_COMMAND_TARGET_MARKER_CAPACITY - 1 < g_InGameCommandTargetTransientEffectMarkerCount)) {
              return;
            }
            commandTargetEntity = (entityRuntime->common).commandTarget.targetEntity;
            if (((((entityRuntime->common).commandTarget.targetFlags & 1) != 0) &&
                (commandTargetEntity != NULL)) &&
               (currentModelNode = (commandTargetEntity->common).ownership.modelNode,
               InGameWorldOverlay_EnsureTransientEffectMarkerAtPoint
                         (*(Q12 *)(*(int *)(commandTargetEntity->common).ownership.
                                           definitionOrClassRecord + 0xdc),
                          (entityRuntime->common).ownership.modelNode,
                          (currentModelNode->worldTransform).translation.y,
                          (currentModelNode->worldTransform).translation.x,targetDefinition.definitionOrError,
                          worldRuntime),
               OVERLAY_COMMAND_TARGET_MARKER_CAPACITY - 1 < g_InGameCommandTargetTransientEffectMarkerCount)) {
              return;
            }
          }
          selectionSlotCursor = selectionSlotCursor + 1;
          recordOrCount--;
        } while (recordOrCount != 0);
      }
    }
  }
  else {
    /* release: detach every marker's model node from the world and forget it */
    ownedEffectCursor = g_InGameOwnedEntityTransientEffectMarkers;
    recordOrCount = g_InGameOwnedEntityTransientEffectMarkerCount;
    if (g_InGameOwnedEntityTransientEffectMarkerCount != 0) {
      do {
        effectSlot = *ownedEffectCursor;
        currentModelNode = (effectSlot->modelNodeOrSavedOffset).modelNode;
        InterpolationState_SetNegatedTargetAndRescaleProgress(0,currentModelNode->shadingRecord);
        WorldRuntime_UnlinkNodeFromOwnerListD8((WorldOwnerListNode100 *)currentModelNode);
        (effectSlot->modelNodeOrSavedOffset).modelNode = NULL;
        ownedEffectCursor = ownedEffectCursor + 1;
        recordOrCount--;
      } while (recordOrCount != 0);
      g_InGameOwnedEntityTransientEffectMarkerCount = 0;
    }
    commandTargetEffectCursor = (EffectRuntimeSlot **)THANDOR_ADDR(g_InGameCommandTargetTransientEffectMarkers,0);
    indexOrCount = g_InGameCommandTargetTransientEffectMarkerCount;
    if (g_InGameCommandTargetTransientEffectMarkerCount != 0) {
      do {
        effectSlot = *commandTargetEffectCursor;
        currentModelNode = (effectSlot->modelNodeOrSavedOffset).modelNode;
        InterpolationState_SetNegatedTargetAndRescaleProgress(0,currentModelNode->shadingRecord);
        WorldRuntime_UnlinkNodeFromOwnerListD8((WorldOwnerListNode100 *)currentModelNode);
        (effectSlot->modelNodeOrSavedOffset).modelNode = NULL;
        commandTargetEffectCursor = commandTargetEffectCursor + 1;
        indexOrCount = indexOrCount - 1;
      } while (indexOrCount != 0);
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
  int remainingSlots;
  GameEntityRuntime **selectionSlotCursor;
  
  remainingSlots = 0x20;
  selectionSlotCursor = g_SelectionInfoEntitySlots->entries;
  do {
    if ((*selectionSlotCursor != (GameEntityRuntime *)0x0) &&
       ((modelNode = ((*selectionSlotCursor)->common).ownership.modelNode, (modelNode->runtimeFlags & 4) != 0 ||
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
                   g_ModelProjectedBoundsPixels.minX,
                   (RuntimeModelFactionPrefix10 *)*selectionSlotCursor); /* EDX: the entity (lost local) */
      }
    }
    selectionSlotCursor = selectionSlotCursor + 1;
    remainingSlots = remainingSlots + -1;
  } while (remainingSlots != 0);
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
  GraphicsTextureSourceAsset *savedTextureSource;
  void *savedPanelData;
  
  modelNode = (entity->common).ownership.modelNode;
  if (((modelNode->runtimeFlags & 4) != 0) ||
     (((modelNode->runtimeFlags & 0x10) == 0 && ((modelNode->runtimeFlags & 8) != 0)))) {
    g_ModelProjectedBoundsPixels.minX = 0x10000;
    g_ModelProjectedBoundsPixels.minY = 0x10000;
    g_ModelProjectedBoundsPixels.maxX = -0x10000;
    g_ModelProjectedBoundsPixels.maxY = -0x10000;
    ModelProjectedBounds_AccumulateHierarchyRecursive(&g_ModelProjectedBoundsPixels,modelNode);
    savedPanelData = g_SelectionPanelData;
    savedTextureSource = g_SelectionPanelTextureSource;
    g_SelectionPanelTextureSource = savedTextureSource;
    g_SelectionPanelData = savedPanelData;
    if ((g_ModelProjectedBoundsPixels.minX < g_ModelProjectedBoundsPixels.maxX) &&
       (g_ModelProjectedBoundsPixels.minY < g_ModelProjectedBoundsPixels.maxY)) {
      g_SelectionPanelTextureSource = g_InfoPanelTextureSource;
      g_SelectionPanelData = g_InfoPanelData;
      SelectionPanel_RenderArmyRuntimeMetrics
                (clipTop,clipLeft,clipBottom,clipRight,g_ModelProjectedBoundsPixels.maxY,
                 g_ModelProjectedBoundsPixels.maxX,g_ModelProjectedBoundsPixels.minY,
                 g_ModelProjectedBoundsPixels.minX,
                 (RuntimeModelFactionPrefix10 *)entity); /* EDX: the entity (lost local) */
      g_SelectionPanelTextureSource = savedTextureSource;
      g_SelectionPanelData = savedPanelData;
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
  int edgeX;
  int edgeY;
  UiPixelCoordinate originalCoordinate0B;
  UiPixelCoordinate originalCoordinate1B;
  uint32_t cornerWidth;
  bool accessFailed;
  TextureSizeResult cornerSize;
  
  originalCoordinate1B = frameCoordinate1B;
  originalCoordinate0B = frameCoordinate0B;
  if (frameCoordinate1A <= frameCoordinate1B) {
    if (frameCoordinate1B == frameCoordinate1A) {
      return;
    }
    frameCoordinate1B = frameCoordinate1A;
    frameCoordinate1A = originalCoordinate1B;
  }
  if (frameCoordinate0A <= frameCoordinate0B) {
    if (frameCoordinate0B == frameCoordinate0A) {
      return;
    }
    frameCoordinate0B = frameCoordinate0A;
    frameCoordinate0A = originalCoordinate0B;
  }
  accessFailed = g_GraphicsFramebufferBeginAccess();
  if (!accessFailed) {
    cornerSize = g_GraphicsTextureSourceGetLogicalSize(0xa4,g_SelectionPanelTextureSource);
    cornerWidth = cornerSize.logicalWidthPixels;
    edgeX = frameCoordinate1B - cornerWidth;
    edgeY = frameCoordinate0B - cornerSize.logicalHeightPixels;
    g_SelectionPanelBlitOpaque
              (clipTop,clipLeft,clipBottom,clipRight,edgeY,edgeX,0xa4,g_SelectionPanelTextureSource,
               g_FramebufferAccess);
    g_SelectionPanelBlitOpaque
              (clipTop,clipLeft,clipBottom,clipRight,edgeY,frameCoordinate1A,0xa6,
               g_SelectionPanelTextureSource,g_FramebufferAccess);
    g_SelectionPanelBlitOpaque
              (clipTop,clipLeft,clipBottom,clipRight,frameCoordinate0A,edgeX,0xa9,
               g_SelectionPanelTextureSource,g_FramebufferAccess);
    g_SelectionPanelBlitOpaque
              (clipTop,clipLeft,clipBottom,clipRight,frameCoordinate0A,frameCoordinate1A,0xab,
               g_SelectionPanelTextureSource,g_FramebufferAccess);
    edgeX = edgeX + cornerWidth;
    g_SelectionPanelBlitClipped
              (clipTop,clipLeft,clipBottom,clipRight,-0x80000000,frameCoordinate1A,edgeY,edgeX,0xa5,
               g_SelectionPanelTextureSource,g_FramebufferAccess);
    g_SelectionPanelBlitClipped
              (clipTop,clipLeft,clipBottom,clipRight,-0x80000000,frameCoordinate1A,frameCoordinate0A
               ,edgeX,0xaa,g_SelectionPanelTextureSource,g_FramebufferAccess);
    edgeY = edgeY + cornerSize.logicalHeightPixels;
    g_SelectionPanelBlitClipped
              (clipTop,clipLeft,clipBottom,clipRight,frameCoordinate0A,-0x80000000,edgeY,
               edgeX - cornerWidth,0xa7,g_SelectionPanelTextureSource,g_FramebufferAccess);
    g_SelectionPanelBlitClipped
              (clipTop,clipLeft,clipBottom,clipRight,frameCoordinate0A,-0x80000000,edgeY,
               frameCoordinate1A,0xa8,g_SelectionPanelTextureSource,g_FramebufferAccess);
    g_GraphicsFramebufferEndAccess();
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
  int64_t packedCoordinate1;
  int64_t packedCoordinate0;
  int screenX;
  int screenY;
  bool accessFailed;
  GraphicsProjectedPointPair projectedPoint;
  TextureSizeResult markerSize;
  TerrainPointResult terrainPoint;
  uint32_t blitTextureId;
  GraphicsTextureSourceAsset *blitTextureSource;
  SoftwareFramebufferAccess *blitFramebuffer;
  
  if (markerPointCount != 0) {
    accessFailed = g_GraphicsFramebufferBeginAccess();
    if (!accessFailed) {
      do {
        packedCoordinate1 = (int64_t)(gridCoordinatePairs[1] + *gridCoordinatePairs * 2) * 0x901;
        packedCoordinate0 = (int64_t)gridCoordinatePairs[1] * -1999;
        terrainPoint = FieldGrid_GetNearestTerrainPoint
                          ((int)((uint64_t)packedCoordinate0 >> 0x20) << 0x14 | (uint32_t)packedCoordinate0 >> 0xc,
                           (int)((uint64_t)packedCoordinate1 >> 0x20) << 0x13 | (uint32_t)packedCoordinate1 >> 0xd,fieldGrid);
        if (!terrainPoint.outOfBounds) {
          g_GraphicsTransformScratchMatrix3x4.basisRow0[0] = terrainPoint.worldXQ12;
          g_GraphicsTransformScratchMatrix3x4.basisRow0[1] = terrainPoint.worldYQ12;
          g_GraphicsTransformScratchMatrix3x4.basisRow0[2] = terrainPoint.terrainHeightQ12;
          FixedTransform_ApplyPoint
                    (&g_GraphicsTransformInputScratchVec3,
                     (GraphicsFixedVec3 *)&g_GraphicsTransformScratchMatrix3x4,
                     &g_ViewProjectionMatrixFixed);
          if (0x10 < g_GraphicsTransformInputScratchVec3.z) {
            projectedPoint = Graphics_ProjectViewPoint(&g_GraphicsTransformInputScratchVec3);
            screenX = projectedPoint.projectedX >> 0xc;
            screenY = projectedPoint.projectedY >> 0xc;
            blitTextureId = 0xad;
            blitTextureSource = g_SelectionPanelTextureSource;
            blitFramebuffer = g_FramebufferAccess;
            markerSize = g_GraphicsTextureSourceGetLogicalSize(0xad,g_SelectionPanelTextureSource);
            g_SelectionPanelBlitOpaque
                      (clipTop,clipLeft,clipBottom,clipRight,
                       screenY - ((int)markerSize.logicalHeightPixels >> 1),
                       screenX - ((int)markerSize.logicalWidthPixels >> 1),blitTextureId,blitTextureSource,blitFramebuffer);
          }
        }
        gridCoordinatePairs = gridCoordinatePairs + 2;
        markerPointCount = markerPointCount + -1;
      } while (markerPointCount != 0);
      g_GraphicsFramebufferEndAccess();
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
  uint32_t pointX;
  uint32_t pointY;
  uint32_t pointZ;
  bool accessFailed;
  GraphicsProjectedPointPair projectedPoint;
  TextureSizeResult markerSize;
  SurfacePointResult topSurfacePoint;
  TerrainPointResult terrainPoint;
  
  if (useTopSurface == 0) {
    terrainPoint = FieldGrid_GetNearestTerrainPoint(worldCoordinate0Q12,worldCoordinate1Q12,fieldGrid);
    pointZ = terrainPoint.terrainHeightQ12;
    pointY = terrainPoint.worldYQ12;
    pointX = terrainPoint.worldXQ12;
    if (terrainPoint.outOfBounds) {
      return;
    }
  }
  else {
    topSurfacePoint = FieldGrid_GetNearestTopSurfacePoint(worldCoordinate0Q12,worldCoordinate1Q12,fieldGrid);
    pointZ = topSurfacePoint.worldZQ12;
    pointY = topSurfacePoint.worldYQ12;
    pointX = topSurfacePoint.worldXQ12;
    if (topSurfacePoint.outOfBounds) {
      return;
    }
  }
  g_GraphicsTransformScratchMatrix3x4.basisRow0[0] = pointX;
  g_GraphicsTransformScratchMatrix3x4.basisRow0[1] = pointY;
  g_GraphicsTransformScratchMatrix3x4.basisRow0[2] = pointZ;
  FixedTransform_ApplyPoint
            (&g_GraphicsTransformInputScratchVec3,
             (GraphicsFixedVec3 *)&g_GraphicsTransformScratchMatrix3x4,&g_ViewProjectionMatrixFixed)
  ;
  projectedPoint = Graphics_ProjectViewPoint(&g_GraphicsTransformInputScratchVec3);
  accessFailed = g_GraphicsFramebufferBeginAccess(); /* Ghidra passed stale register values (worldCoordinate0Q12, worldCoordinate1Q12, fieldGrid); the callee takes none */
  if (!accessFailed) {
    markerSize = g_GraphicsTextureSourceGetLogicalSize(0xac,g_SelectionPanelTextureSource);
    g_SelectionPanelBlitOpaque
              (clipTop,clipLeft,clipBottom,clipRight,
               (projectedPoint.projectedY >> 0xc) - ((int)markerSize.logicalHeightPixels >> 1),
               (projectedPoint.projectedX >> 0xc) - ((int)markerSize.logicalWidthPixels >> 1),0xac,
               g_SelectionPanelTextureSource,g_FramebufferAccess);
    g_GraphicsFramebufferEndAccess();
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
  uint32_t gridColumns;
  int screenX;
  uint32_t columnCount;
  uint32_t columnsRemaining;
  uint32_t rowsRemaining;
  int screenY;
  uint8_t *vertexCursor;
  int coordinateOffset;
  bool accessFailed;
  TextureSizeResult markerSize;
  uint32_t blitTextureId;
  GraphicsTextureSourceAsset *blitTextureSource;
  SoftwareFramebufferAccess *blitFramebuffer;
  uint8_t *rowStart;
  
  accessFailed = g_GraphicsFramebufferBeginAccess();
  if (!accessFailed) {
    coordinateOffset = 0;
    gridColumns = fieldGrid->gridWidth;
    columnCount = gridColumns >> 2;
    rowsRemaining = fieldGrid->gridHeight >> 2;
    vertexCursor = fieldGrid[1].common.buildMetadata.assetRelativeAddressAnchor28 + gridColumns * 0x80 + -0x28;
    columnsRemaining = columnCount;
    rowStart = vertexCursor;
    if ((g_UiCommandModeGColorVariantLimit & 0xff000000) != 0) {
      coordinateOffset = 0x20;
    }
    do {
      do {
        if ((*(uint32_t *)(vertexCursor + 0x50) & 0x200000) == 0) {
          screenX = *(int *)(vertexCursor + coordinateOffset + 0xc) >> 0xc;
          screenY = *(int *)(vertexCursor + coordinateOffset + 0x10) >> 0xc;
          blitTextureId = 0xae;
          blitTextureSource = g_SelectionPanelTextureSource;
          blitFramebuffer = g_FramebufferAccess;
          markerSize = g_GraphicsTextureSourceGetLogicalSize(0xae,g_SelectionPanelTextureSource);
          g_SelectionPanelBlitOpaque
                    (clipTop,clipLeft,clipBottom,clipRight,
                     screenY - ((int)markerSize.logicalHeightPixels >> 1),
                     screenX - ((int)markerSize.logicalWidthPixels >> 1),blitTextureId,blitTextureSource,blitFramebuffer);
        }
        vertexCursor = vertexCursor + 0x200;
        columnsRemaining = columnsRemaining - 1;
      } while (-1 < (int)columnsRemaining);
      vertexCursor = rowStart + gridColumns * 0x200;
      rowsRemaining = rowsRemaining - 1;
      columnsRemaining = columnCount;
      rowStart = vertexCursor;
    } while (-1 < (int)rowsRemaining);
    g_GraphicsFramebufferEndAccess();
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
  FieldGridDimension gridColumns;
  int screenX;
  FieldGridDimension columnsRemaining;
  int screenY;
  FieldGridDimension rowsRemaining;
  FieldGridCell *cellCursor;
  bool accessFailed;
  TextureSizeResult markerSize;
  uint32_t receiverTextureId;
  GraphicsTextureSourceAsset *receiverTextureSource;
  SoftwareFramebufferAccess *receiverFramebuffer;
  int savedScreenY;
  int savedScreenX;
  uint32_t sourceTextureId;
  GraphicsTextureSourceAsset *sourceTextureSource;
  SoftwareFramebufferAccess *sourceFramebuffer;
  FieldGridCell *rowStartCell;
  
  accessFailed = g_GraphicsFramebufferBeginAccess();
  if (!accessFailed) {
    gridColumns = fieldGrid->gridWidth;
    rowsRemaining = fieldGrid->gridHeight;
    cellCursor = fieldGrid->cells;
    columnsRemaining = gridColumns;
    rowStartCell = cellCursor;
    do {
      do {
        if (((cellCursor->flagsAndMaterial & 0x4000000) == 0) &&
           ((cellCursor->flagsAndMaterial &
            (FIELD_CELL_FLUID_SOURCE_EXCLUDED|FIELD_CELL_FLUID_RECEIVER_EXCLUDED)) != 0)) {
          screenX = *(int *)(cellCursor->runtime0C_3F + 0x20) >> 0xc;
          screenY = *(int *)(cellCursor->runtime0C_3F + 0x24) >> 0xc;
          sourceTextureId = 0xb0;
          receiverTextureId = 0xaf;
          sourceTextureSource = g_SelectionPanelTextureSource;
          sourceFramebuffer = g_FramebufferAccess;
          if ((cellCursor->flagsAndMaterial & FIELD_CELL_FLUID_RECEIVER_EXCLUDED) != 0) {
            receiverTextureSource = g_SelectionPanelTextureSource;
            receiverFramebuffer = g_FramebufferAccess;
            savedScreenY = screenY;
            savedScreenX = screenX;
            markerSize = g_GraphicsTextureSourceGetLogicalSize(0xaf,g_SelectionPanelTextureSource);
            g_SelectionPanelBlitOpaque
                      (clipTop,clipLeft,clipBottom,clipRight,
                       screenY - ((int)markerSize.logicalHeightPixels >> 1),
                       screenX - ((int)markerSize.logicalWidthPixels >> 1),receiverTextureId,receiverTextureSource,receiverFramebuffer);
            screenY = savedScreenY;
            screenX = savedScreenX;
          }
          if ((cellCursor->flagsAndMaterial & FIELD_CELL_FLUID_SOURCE_EXCLUDED) != 0) {
            markerSize = g_GraphicsTextureSourceGetLogicalSize(0xb0,g_SelectionPanelTextureSource);
            g_SelectionPanelBlitOpaque
                      (clipTop,clipLeft,clipBottom,clipRight,
                       screenY - ((int)markerSize.logicalHeightPixels >> 1),
                       screenX - ((int)markerSize.logicalWidthPixels >> 1),sourceTextureId,sourceTextureSource,sourceFramebuffer);
          }
        }
        cellCursor = cellCursor + 1;
        columnsRemaining = columnsRemaining - 1;
      } while (columnsRemaining != 0);
      cellCursor = rowStartCell + gridColumns;
      rowsRemaining = rowsRemaining - 1;
      columnsRemaining = gridColumns;
      rowStartCell = cellCursor;
    } while (rowsRemaining != 0);
    g_GraphicsFramebufferEndAccess();
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
          UiPixelCoordinate clipRight,uint8_t markerBitIndex,FieldGridAsset *fieldGrid)

{
  FieldGridDimension gridColumns;
  int screenX;
  FieldGridDimension columnsRemaining;
  int screenY;
  FieldGridDimension rowsRemaining;
  FieldGridCell *cellCursor;
  FieldCellPackedFlagsAndMaterial markerFlagMask;
  bool accessFailed;
  TextureSizeResult markerSize;
  uint32_t flaggedTextureId;
  GraphicsTextureSourceAsset *flaggedTextureSource;
  SoftwareFramebufferAccess *flaggedFramebuffer;
  int savedScreenY;
  int savedScreenX;
  uint32_t otherTextureId;
  GraphicsTextureSourceAsset *otherTextureSource;
  SoftwareFramebufferAccess *otherFramebuffer;
  FieldGridCell *rowStartCell;
  
  markerFlagMask = 0x800 << (markerBitIndex & 0x1f);
  accessFailed = g_GraphicsFramebufferBeginAccess();
  if (!accessFailed) {
    gridColumns = fieldGrid->gridWidth;
    rowsRemaining = fieldGrid->gridHeight;
    cellCursor = fieldGrid->cells;
    columnsRemaining = gridColumns;
    rowStartCell = cellCursor;
    do {
      do {
        if (((cellCursor->flagsAndMaterial & 0x200000) == 0) &&
           ((cellCursor->flagsAndMaterial & FIELD_CELL_XENITE_OR_TRITIUM_SUPPORT_MASK) != 0)) {
          screenX = *(int *)cellCursor->runtime0C_3F >> 0xc;
          screenY = *(int *)(cellCursor->runtime0C_3F + 4) >> 0xc;
          otherTextureId = 0xb2;
          flaggedTextureId = 0xb1;
          otherTextureSource = g_SelectionPanelTextureSource;
          otherFramebuffer = g_FramebufferAccess;
          if ((cellCursor->flagsAndMaterial & markerFlagMask) != 0) {
            flaggedTextureSource = g_SelectionPanelTextureSource;
            flaggedFramebuffer = g_FramebufferAccess;
            savedScreenY = screenY;
            savedScreenX = screenX;
            markerSize = g_GraphicsTextureSourceGetLogicalSize(0xb1,g_SelectionPanelTextureSource);
            g_SelectionPanelBlitOpaque
                      (clipTop,clipLeft,clipBottom,clipRight,
                       screenY - ((int)markerSize.logicalHeightPixels >> 1),
                       screenX - ((int)markerSize.logicalWidthPixels >> 1),flaggedTextureId,flaggedTextureSource,flaggedFramebuffer);
            screenY = savedScreenY;
            screenX = savedScreenX;
          }
          if ((cellCursor->flagsAndMaterial & (markerFlagMask ^ FIELD_CELL_XENITE_OR_TRITIUM_SUPPORT_MASK)) != 0)
          {
            markerSize = g_GraphicsTextureSourceGetLogicalSize(0xb2,g_SelectionPanelTextureSource);
            g_SelectionPanelBlitOpaque
                      (clipTop,clipLeft,clipBottom,clipRight,
                       screenY - ((int)markerSize.logicalHeightPixels >> 1),
                       screenX - ((int)markerSize.logicalWidthPixels >> 1),otherTextureId,otherTextureSource,otherFramebuffer);
          }
        }
        cellCursor = cellCursor + 1;
        columnsRemaining = columnsRemaining - 1;
      } while (columnsRemaining != 0);
      cellCursor = rowStartCell + gridColumns;
      rowsRemaining = rowsRemaining - 1;
      columnsRemaining = gridColumns;
      rowStartCell = cellCursor;
    } while (rowsRemaining != 0);
    g_GraphicsFramebufferEndAccess();
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
  FieldGridDimension gridColumns;
  int screenX;
  FieldGridDimension columnsRemaining;
  int screenY;
  FieldGridDimension rowsRemaining;
  FieldGridCell *cellCursor;
  bool accessFailed;
  TextureSizeResult markerSize;
  uint32_t blitTextureId;
  GraphicsTextureSourceAsset *blitTextureSource;
  SoftwareFramebufferAccess *blitFramebuffer;
  FieldGridCell *rowStartCell;
  
  accessFailed = g_GraphicsFramebufferBeginAccess();
  if (!accessFailed) {
    gridColumns = fieldGrid->gridWidth;
    rowsRemaining = fieldGrid->gridHeight;
    cellCursor = fieldGrid->cells;
    columnsRemaining = gridColumns;
    rowStartCell = cellCursor;
    do {
      do {
        if (((cellCursor->flagsAndMaterial & FIELD_CELL_INIT_CLEARED_UNRESOLVED_BIT15) != 0) &&
           ((cellCursor->flagsAndMaterial & 0x200000) == 0)) {
          screenX = *(int *)cellCursor->runtime0C_3F >> 0xc;
          screenY = *(int *)(cellCursor->runtime0C_3F + 4) >> 0xc;
          blitTextureId = 0xaf;
          blitTextureSource = g_SelectionPanelTextureSource;
          blitFramebuffer = g_FramebufferAccess;
          markerSize = g_GraphicsTextureSourceGetLogicalSize(0xaf,g_SelectionPanelTextureSource);
          g_SelectionPanelBlitOpaque
                    (clipTop,clipLeft,clipBottom,clipRight,
                     screenY - ((int)markerSize.logicalHeightPixels >> 1),
                     screenX - ((int)markerSize.logicalWidthPixels >> 1),blitTextureId,blitTextureSource,blitFramebuffer);
        }
        cellCursor = cellCursor + 1;
        columnsRemaining = columnsRemaining - 1;
      } while (columnsRemaining != 0);
      cellCursor = rowStartCell + gridColumns;
      rowsRemaining = rowsRemaining - 1;
      columnsRemaining = gridColumns;
      rowStartCell = cellCursor;
    } while (rowsRemaining != 0);
    g_GraphicsFramebufferEndAccess();
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
   EffectRuntimePool_CreateInstanceFromDefinition [world/effects/runtime].
*/
void __thandor_void_preserve_eax_ecx_edx
InGameWorldOverlay_EnsureTransientEffectMarkerAtPoint
          (Q12 scaleQ12,void *sourceWorldNode,Q12 worldYQ12,Q12 worldXQ12,void *effectDefinition,
          void *inGameRuntime)

{
  GraphicsWorldCoordinateQ12 *translationZ;
  ModelRuntimeNode *markerModelNode;
  uint32_t boundingRadius;
  int markerSlotIndex;
  int remainingMarkers;
  int *markerCursor;
  HeightSampleResult surfaceHeight;
  EffectCreateResult createdEffect;
  
  markerSlotIndex = g_InGameCommandTargetTransientEffectMarkerCount;
  if ((worldXQ12 != *(int *)((int)sourceWorldNode + 0x94)) ||
     (worldYQ12 != *(int *)((int)sourceWorldNode + 0x98))) {
    markerCursor = (int *)THANDOR_ADDR(g_InGameCommandTargetTransientEffectMarkers,0);
    for (remainingMarkers = g_InGameCommandTargetTransientEffectMarkerCount; remainingMarkers != 0; remainingMarkers = remainingMarkers + -1) {
      if ((worldXQ12 == *(int *)(*(int *)(*markerCursor + 4) + 0x94)) &&
         (worldYQ12 == *(int *)(*(int *)(*markerCursor + 4) + 0x98))) {
        return;
      }
      markerCursor = markerCursor + 1;
    }
    surfaceHeight = FieldGrid_InterpolateTopSurfaceHeight
                      (worldYQ12,worldXQ12,*(FieldGridAsset **)((int)inGameRuntime + 0x54));
    createdEffect = EffectRuntimePool_CreateInstanceFromDefinition
                      (EFFECT_RUNTIME_COMPLETION_NONE,THANDOR_BITCAST(int, EffectRuntimeOwnerReference4, 0x0),0,0x4000,0,
                       surfaceHeight.heightQ12,worldYQ12,worldXQ12,effectDefinition,inGameRuntime);
    *(EffectRuntimeSlot **)(markerSlotIndex * 4 + THANDOR_ADDR(g_InGameCommandTargetTransientEffectMarkers,0)) = createdEffect.effectRuntime;
    markerModelNode = ((createdEffect.effectRuntime)->modelNodeOrSavedOffset).modelNode;
    g_InGameCommandTargetTransientEffectMarkerCount =
         g_InGameCommandTargetTransientEffectMarkerCount + 1;
    boundingRadius = markerModelNode->subtreeBoundingRadiusQ12;
    markerModelNode->tintArgb = 0xffffffff;
    if ((scaleQ12 != 0x1000) && (boundingRadius != 0)) {
      translationZ = &(markerModelNode->worldTransform).translation.z;
      *translationZ = *translationZ + 0x144;
      markerModelNode->runtimeFlags = markerModelNode->runtimeFlags | 0x800;
      markerModelNode->modelScaleQ12 = (Q12)(((uint64_t)(uint32_t)scaleQ12 * 0x1a00) / (uint64_t)boundingRadius);
    }
  }
  return;
}


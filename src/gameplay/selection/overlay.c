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
void InGameWorldOverlay_RebuildOrReleaseTransientMarkers
          (GraphicsBooleanState releaseMode,WorldRuntimeContext *worldRuntime)

{
  PackedArgb32 *childTint;
  ModelRuntimeNode *currentModelNode;
  int *classRecord;
  ModelRuntimeSlot *factoryModelRuntime;
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
  if (((worldRuntime->interaction).nodeFlags & 8) != 0) {
    return;
  }
  if ((worldRuntime->runtimeFlags & WORLD_RUNTIME_FLAG_NOTIFICATION_GOTO) != 0) {
    return;
  }
  if ((g_UiCommandRuntimeFlags & UI_COMMAND_RUNTIME_FLAG_WORLD_INPUT_DISABLED) != 0) {
    return;
  }
  if ((g_UiCommandRuntimeFlags & UI_COMMAND_RUNTIME_FLAG_PLACEMENT_PENDING) == 0) {
    checkResult = SelectionInfo_TestNotOwnAircraftPadsWithAircraft
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
      g_ArmyPlacementLateRejectionCount = 1;
      checkResult = ArmyPlacement_ValidateAssetAtPointAndCellCorners
                         (0,g_InGamePlacementHeading16,g_InGamePlacementWorldXQ12,
                          g_InGamePlacementWorldYQ12,
                          *(ArmyPlacementContext *)(g_InGamePendingPlacementArmyAsset + 8),
                          worldRuntime->activeFactionRuntimeIndex,worldRuntime);
      acceptedCandidateCount = g_ArmyPlacementLateRejectionCount;
      if (checkResult) {
        g_ArmyPlacementLateRejectionCount = 0;
        if (acceptedCandidateCount < 2) goto RefreshMarkers;
        previewTint = OVERLAY_PREVIEW_TINT_MULTI_CANDIDATE_ARGB;
      }
      armyAssetId = *(PckArmyAssetIdCatalog *)(pendingPlacementAsset + 8);
      /* ECX/EDX of the validator: the accepted (possibly snapped) point. */
      worldXQ12 = g_ArmyPlacementValidatedWorldXQ12;
      validatedWorldYQ12 = g_ArmyPlacementValidatedWorldYQ12;
      g_ArmyPlacementLateRejectionCount = 1;
      worldRuntimeCopy = worldRuntime;
      dispatchResult = ArmyPlacement_DispatchAssetAtFieldPoint
                         (1,0,g_InGamePlacementHeading16,validatedWorldYQ12,worldXQ12,
                          *(PckArmyAssetIdCatalog *)(pendingPlacementAsset + 8),
                          worldRuntime->activeFactionRuntimeIndex,(UiRootNode *)worldRuntime);
      if ((dispatchResult.failed) && (g_ArmyPlacementLateRejectionCount < 2)) {
        previewTint = previewTint & OVERLAY_PREVIEW_TINT_BLOCKED_MASK;
      }
      g_ArmyPlacementLateRejectionCount = 0;
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
        if (((((ModelDefinition *)recordOrCount)->runtimeClassId == MODEL_RUNTIME_CLASS_13) &&
            (3 < currentModelNode->childCount)) &&
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
RefreshMarkers:
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
            (factoryModelRuntime = (modelNodeCursor->runtimePayload).modelRuntime,
            factoryModelRuntime->definitionOrSavedId.runtimeDefinition->runtimeClassId == MODEL_RUNTIME_CLASS_13)) &&
           ((((factoryModelRuntime->classState).stateFlags & ARMY_MODEL_STATE_RALLY_POINT_SET) != 0 &&
            (factoryModelRuntime->ownerArmyRuntimeOrSavedOffset.armyRuntime->factionIndex ==
             worldRuntime->activeFactionRuntimeIndex)))) {
          /* the unit factory's exit point: X at +0x78, Y at +0x7C (classLinkState.classState78 / 7C) */
          surfaceHeight = FieldGrid_InterpolateTopSurfaceHeight
                             ((factoryModelRuntime->classLinkState).classState7C,
                              (factoryModelRuntime->classLinkState).classState78,worldRuntime->fieldGrid);
          createdEffect = EffectRuntimePool_CreateInstanceFromDefinition
                             (EFFECT_RUNTIME_COMPLETION_NONE,THANDOR_BITCAST(int, EffectRuntimeOwnerReference, 0),0,
                              FIXED_ANGLE16_QUARTER_TURN,0,surfaceHeight.heightQ12,(factoryModelRuntime->classLinkState).classState7C,
                              (factoryModelRuntime->classLinkState).classState78,markerDefinition.definitionOrError,
                              worldRuntime);
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
           marker scale (its definition's footprintRadius); at most OVERLAY_COMMAND_TARGET_MARKER_CAPACITY markers */
        recordOrCount = SELECTION_ENTRY_CAPACITY;
        selectionSlotCursor = g_SelectionInfoEntitySlots->entries;
        do {
          entityRuntime = *selectionSlotCursor;
          if ((entityRuntime != NULL) &&
             (worldRuntime->activeFactionRuntimeIndex == (entityRuntime->common).ownership.ownerIndex)) {
            if ((((ModelRuntimeSlot *)(entityRuntime->common).ownership.definitionOrClassRecord)->definitionOrSavedId.
                 runtimeDefinition->accelerationPerTick != 0)
               && (((entityRuntime->common).commandFlags & ARMY_MOVEMENT_ACTIVE) != 0)) {
              InGameWorldOverlay_EnsureTransientEffectMarkerAtPoint
                        (Q12_ONE,(entityRuntime->common).ownership.modelNode,
                         (entityRuntime->common).pathCoordinate1Q12,(entityRuntime->common).pathCoordinate0Q12,
                         markerDefinition.definitionOrError,worldRuntime);
              if (OVERLAY_COMMAND_TARGET_MARKER_CAPACITY - 1 < g_InGameCommandTargetTransientEffectMarkerCount) {
                return;
              }
              if (((entityRuntime->common).commandFlags & ARMY_MOVEMENT_WAYPOINTS_QUEUED) != 0) {
                /* further waypoints: the army's queued waypoints */
                indexOrCount = 0;
                do {
                  InGameWorldOverlay_EnsureTransientEffectMarkerAtPoint
                            (Q12_ONE,(entityRuntime->common).ownership.modelNode,
                             ((ArmyMovementRuntime *)entityRuntime)->queuedWaypoints[indexOrCount].worldYQ12,
                             ((ArmyMovementRuntime *)entityRuntime)->queuedWaypoints[indexOrCount].worldXQ12,
                             markerDefinition.definitionOrError,worldRuntime);
                  indexOrCount++;
                  if (OVERLAY_COMMAND_TARGET_MARKER_CAPACITY - 1 < g_InGameCommandTargetTransientEffectMarkerCount) {
                    return;
                  }
                } while (indexOrCount < ((ArmyMovementRuntime *)entityRuntime)->queuedWaypointCount);
              }
            }
            if ((((entityRuntime->common).commandTarget.targetFlags & 2) != 0) &&
               (InGameWorldOverlay_EnsureTransientEffectMarkerAtPoint
                          (Q12_ONE,(entityRuntime->common).ownership.modelNode,
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
                         (((ModelRuntimeSlot *)(commandTargetEntity->common).ownership.definitionOrClassRecord)->
                          definitionOrSavedId.runtimeDefinition->footprintRadius,
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
        WorldRuntime_UnlinkOwnerListNode((WorldOwnerListNode *)currentModelNode);
        (effectSlot->modelNodeOrSavedOffset).modelNode = NULL;
        ownedEffectCursor = ownedEffectCursor + 1;
        recordOrCount--;
      } while (recordOrCount != 0);
      g_InGameOwnedEntityTransientEffectMarkerCount = 0;
    }
    commandTargetEffectCursor = g_InGameCommandTargetTransientEffectMarkers;
    indexOrCount = g_InGameCommandTargetTransientEffectMarkerCount;
    if (g_InGameCommandTargetTransientEffectMarkerCount != 0) {
      do {
        effectSlot = *commandTargetEffectCursor;
        currentModelNode = (effectSlot->modelNodeOrSavedOffset).modelNode;
        InterpolationState_SetNegatedTargetAndRescaleProgress(0,currentModelNode->shadingRecord);
        WorldRuntime_UnlinkOwnerListNode((WorldOwnerListNode *)currentModelNode);
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
   Draws the metric bars (SelectionPanel_RenderArmyRuntimeMetrics) over every selected entity whose model node
   carries flag 4, or flag 8 without 0x10, at the screen bounds of its projected model hierarchy; entities whose
   bounds come out empty are skipped. Called by FrontendModelPointerContext_RenderWorldViewQueuesClipped when
   context flag 0x400 is set.
*/
void SelectionOverlay_RenderSelectedArmyMetrics
          (UiPixelCoordinate clipBottom,UiPixelCoordinate clipRight,UiPixelCoordinate clipTop,
          UiPixelCoordinate clipLeft)

{
  ModelRuntimeNode *modelNode;
  int remainingSlots;
  GameEntityRuntime **selectionSlotCursor;
  
  remainingSlots = SELECTION_ENTRY_CAPACITY;
  selectionSlotCursor = g_SelectionInfoEntitySlots->entries;
  do {
    if ((*selectionSlotCursor != NULL) &&
       ((modelNode = ((*selectionSlotCursor)->common).ownership.modelNode, (modelNode->runtimeFlags & 4) != 0 ||
        (((modelNode->runtimeFlags & TERRAIN_OCCUPANCY_FLAG_NOT_REMEMBERED) == 0 && ((modelNode->runtimeFlags & TERRAIN_OCCUPANCY_FLAG_SEEN_BEFORE) != 0)))))) {
      g_ModelProjectedBoundsPixels.minX = SELECTION_OVERLAY_EMPTY_BOUNDS_MIN;
      g_ModelProjectedBoundsPixels.minY = SELECTION_OVERLAY_EMPTY_BOUNDS_MIN;
      g_ModelProjectedBoundsPixels.maxX = SELECTION_OVERLAY_EMPTY_BOUNDS_MAX;
      g_ModelProjectedBoundsPixels.maxY = SELECTION_OVERLAY_EMPTY_BOUNDS_MAX;
      ModelProjectedBounds_AccumulateHierarchyRecursive(&g_ModelProjectedBoundsPixels,modelNode);
      if ((g_ModelProjectedBoundsPixels.minX < g_ModelProjectedBoundsPixels.maxX) &&
         (g_ModelProjectedBoundsPixels.minY < g_ModelProjectedBoundsPixels.maxY)) {
        SelectionPanel_RenderArmyRuntimeMetrics
                  (clipBottom,clipRight,clipTop,clipLeft,g_ModelProjectedBoundsPixels.maxY,
                   g_ModelProjectedBoundsPixels.maxX,g_ModelProjectedBoundsPixels.minY,
                   g_ModelProjectedBoundsPixels.minX,
                   (RuntimeModelFactionPrefix *)*selectionSlotCursor); /* EDX: the entity (lost local) */
      }
    }
    selectionSlotCursor++;
    remainingSlots--;
  } while (remainingSlots != 0);
  return;
}


/* Address: 0x0052F1B0.
   Draws the metric bars of one entity like SelectionOverlay_RenderSelectedArmyMetrics, but with the info-panel
   texture and data (g_InfoPanelTextureSource/g_InfoPanelData) swapped in for the call. Called by
   FrontendModelPointerContext_RenderWorldViewQueuesClipped for its selectedOverlayEntity when that entity is in
   the selection info.
*/
void SelectionOverlay_RenderArmyMetricsForEntity
          (UiPixelCoordinate clipBottom,UiPixelCoordinate clipRight,UiPixelCoordinate clipTop,
          UiPixelCoordinate clipLeft,GameEntityRuntime *entity)

{
  ModelRuntimeNode *modelNode;
  GraphicsTextureSourceAsset *savedTextureSource;
  void *savedPanelData;
  
  modelNode = (entity->common).ownership.modelNode;
  if (((modelNode->runtimeFlags & 4) != 0) ||
     (((modelNode->runtimeFlags & TERRAIN_OCCUPANCY_FLAG_NOT_REMEMBERED) == 0 && ((modelNode->runtimeFlags & TERRAIN_OCCUPANCY_FLAG_SEEN_BEFORE) != 0)))) {
    g_ModelProjectedBoundsPixels.minX = SELECTION_OVERLAY_EMPTY_BOUNDS_MIN;
    g_ModelProjectedBoundsPixels.minY = SELECTION_OVERLAY_EMPTY_BOUNDS_MIN;
    g_ModelProjectedBoundsPixels.maxX = SELECTION_OVERLAY_EMPTY_BOUNDS_MAX;
    g_ModelProjectedBoundsPixels.maxY = SELECTION_OVERLAY_EMPTY_BOUNDS_MAX;
    ModelProjectedBounds_AccumulateHierarchyRecursive(&g_ModelProjectedBoundsPixels,modelNode);
    savedPanelData = g_SelectionPanelData;
    savedTextureSource = g_SelectionPanelTextureSource;
    /* no-op write-back from the decompilation; the original saves both only inside the if below */
    g_SelectionPanelTextureSource = savedTextureSource;
    g_SelectionPanelData = savedPanelData;
    if ((g_ModelProjectedBoundsPixels.minX < g_ModelProjectedBoundsPixels.maxX) &&
       (g_ModelProjectedBoundsPixels.minY < g_ModelProjectedBoundsPixels.maxY)) {
      g_SelectionPanelTextureSource = g_InfoPanelTextureSource;
      g_SelectionPanelData = g_InfoPanelData;
      SelectionPanel_RenderArmyRuntimeMetrics
                (clipBottom,clipRight,clipTop,clipLeft,g_ModelProjectedBoundsPixels.maxY,
                 g_ModelProjectedBoundsPixels.maxX,g_ModelProjectedBoundsPixels.minY,
                 g_ModelProjectedBoundsPixels.minX,
                 (RuntimeModelFactionPrefix *)entity); /* EDX: the entity (lost local) */
      g_SelectionPanelTextureSource = savedTextureSource;
      g_SelectionPanelData = savedPanelData;
    }
  }
  return;
}


/* Address: 0x0052F2A0.
   Draws a frame around the screen rectangle spanned by corners A and B (either order; nothing when it is empty in
   either direction): four corner pieces outside the rectangle and the four edges tiled between them. Called by
   FrontendModelPointerContext_RenderWorldViewQueuesClipped when context flag 0x80 is set (the drag-selection
   rectangle, WORLD_RUNTIME_FLAG_DRAG_SELECTING in the in-game world view).
*/
void SelectionOverlay_DrawBoundsFrame(UiPixelCoordinate clipBottom,UiPixelCoordinate clipRight,UiPixelCoordinate clipTop,
          UiPixelCoordinate clipLeft,UiPixelCoordinate cornerAY,UiPixelCoordinate cornerAX,
          UiPixelCoordinate cornerBY,UiPixelCoordinate cornerBX)

{
  int edgeX;
  int edgeY;
  UiPixelCoordinate originalCornerBY;
  UiPixelCoordinate originalCornerBX;
  uint32_t cornerWidth;
  bool accessFailed;
  TextureSizeResult cornerSize;
  
  /* order the corners: A becomes bottom-right (maximum), B top-left (minimum) */
  originalCornerBX = cornerBX;
  originalCornerBY = cornerBY;
  if (cornerAX <= cornerBX) {
    if (cornerBX == cornerAX) {
      return;
    }
    cornerBX = cornerAX;
    cornerAX = originalCornerBX;
  }
  if (cornerAY <= cornerBY) {
    if (cornerBY == cornerAY) {
      return;
    }
    cornerBY = cornerAY;
    cornerAY = originalCornerBY;
  }
  accessFailed = g_GraphicsFramebufferBeginAccess();
  if (!accessFailed) {
    cornerSize = g_GraphicsTextureSourceGetLogicalSize(SELECTION_OVERLAY_FRAME_TOP_LEFT,g_SelectionPanelTextureSource);
    cornerWidth = cornerSize.logicalWidthPixels;
    edgeX = cornerBX - cornerWidth;
    edgeY = cornerBY - cornerSize.logicalHeightPixels;
    g_SelectionPanelBlitOpaque
              (clipBottom,clipRight,clipTop,clipLeft,edgeY,edgeX,SELECTION_OVERLAY_FRAME_TOP_LEFT,
               g_SelectionPanelTextureSource,g_FramebufferAccess);
    g_SelectionPanelBlitOpaque
              (clipBottom,clipRight,clipTop,clipLeft,edgeY,cornerAX,SELECTION_OVERLAY_FRAME_TOP_RIGHT,
               g_SelectionPanelTextureSource,g_FramebufferAccess);
    g_SelectionPanelBlitOpaque
              (clipBottom,clipRight,clipTop,clipLeft,cornerAY,edgeX,SELECTION_OVERLAY_FRAME_BOTTOM_LEFT,
               g_SelectionPanelTextureSource,g_FramebufferAccess);
    g_SelectionPanelBlitOpaque
              (clipBottom,clipRight,clipTop,clipLeft,cornerAY,cornerAX,SELECTION_OVERLAY_FRAME_BOTTOM_RIGHT,
               g_SelectionPanelTextureSource,g_FramebufferAccess);
    edgeX = edgeX + cornerWidth;
    g_SelectionPanelBlitClipped
              (clipBottom,clipRight,clipTop,clipLeft,GRAPHICS_TILED_BLIT_ONE_TILE,cornerAX,edgeY,edgeX,
               SELECTION_OVERLAY_FRAME_TOP,g_SelectionPanelTextureSource,g_FramebufferAccess);
    g_SelectionPanelBlitClipped
              (clipBottom,clipRight,clipTop,clipLeft,GRAPHICS_TILED_BLIT_ONE_TILE,cornerAX,cornerAY,edgeX,
               SELECTION_OVERLAY_FRAME_BOTTOM,g_SelectionPanelTextureSource,g_FramebufferAccess);
    edgeY = edgeY + cornerSize.logicalHeightPixels;
    g_SelectionPanelBlitClipped
              (clipBottom,clipRight,clipTop,clipLeft,cornerAY,GRAPHICS_TILED_BLIT_ONE_TILE,edgeY,
               edgeX - cornerWidth,SELECTION_OVERLAY_FRAME_LEFT,g_SelectionPanelTextureSource,g_FramebufferAccess);
    g_SelectionPanelBlitClipped
              (clipBottom,clipRight,clipTop,clipLeft,cornerAY,GRAPHICS_TILED_BLIT_ONE_TILE,edgeY,
               cornerAX,SELECTION_OVERLAY_FRAME_RIGHT,g_SelectionPanelTextureSource,g_FramebufferAccess);
    g_GraphicsFramebufferEndAccess();
  }
  return;
}


/* Address: 0x0052F490.
   Draws the SELECTION_OVERLAY_MARKER_GRID_POINT marker centred on the screen position of each of markerPointCount
   grid coordinate pairs: each pair is converted to a world point, snapped to the nearest terrain point and
   projected; points off the field or not beyond the near plane are skipped. Called by
   FrontendModelPointerContext_RenderWorldViewQueuesClipped with its terrainMarkerCoordinatePairs when context
   flag 0x200000 is set.
*/
void SelectionOverlay_DrawTerrainPointMarkers
          (UiPixelCoordinate clipBottom,UiPixelCoordinate clipRight,UiPixelCoordinate clipTop,
          UiPixelCoordinate clipLeft,int markerPointCount,int *gridCoordinatePairs,
          FieldGridAsset *fieldGrid)

{
  int64_t worldXProduct;
  int64_t worldYProduct;
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
        /* pair (a, b) -> world point: x = (2a + b) * 0x901 / 2^13, y = -b * 1999 / 2^12 (SHRD of the 64-bit
           products); 0x901 and 1999 are about one cell width 0x900 and 0x900 * sqrt(3) / 2, so the pairs look
           like triangular-lattice coordinates in Q12 */
        worldXProduct = (int64_t)(gridCoordinatePairs[1] + *gridCoordinatePairs * 2) * FIELD_GRID_WORLD_COLUMN_STEP_X;
        worldYProduct = (int64_t)gridCoordinatePairs[1] * -1999;
        terrainPoint = FieldGrid_GetNearestTerrainPoint
                          (FIXED_PRODUCT_SHR(worldYProduct,Q12_SHIFT),
                           FIXED_PRODUCT_SHR(worldXProduct,13),fieldGrid);
        if (!terrainPoint.outOfBounds) {
          g_GraphicsTransformScratchMatrix3x4.basisRow0[0] = terrainPoint.worldXQ12;
          g_GraphicsTransformScratchMatrix3x4.basisRow0[1] = terrainPoint.worldYQ12;
          g_GraphicsTransformScratchMatrix3x4.basisRow0[2] = terrainPoint.terrainHeightQ12;
          FixedTransform_ApplyPoint
                    (&g_GraphicsTransformInputScratchVec3,
                     (GraphicsFixedVec3 *)&g_GraphicsTransformScratchMatrix3x4,
                     &g_ViewProjectionMatrixFixed);
          if (16 < g_GraphicsTransformInputScratchVec3.z) { /* in front of the near plane */
            projectedPoint = Graphics_ProjectViewPoint(&g_GraphicsTransformInputScratchVec3);
            screenX = projectedPoint.projectedX >> 12;
            screenY = projectedPoint.projectedY >> 12;
            blitTextureId = SELECTION_OVERLAY_MARKER_GRID_POINT;
            blitTextureSource = g_SelectionPanelTextureSource;
            blitFramebuffer = g_FramebufferAccess;
            markerSize = g_GraphicsTextureSourceGetLogicalSize(SELECTION_OVERLAY_MARKER_GRID_POINT,
                                                               g_SelectionPanelTextureSource);
            g_SelectionPanelBlitOpaque
                      (clipBottom,clipRight,clipTop,clipLeft,
                       screenY - ((int)markerSize.logicalHeightPixels >> 1),
                       screenX - ((int)markerSize.logicalWidthPixels >> 1),blitTextureId,blitTextureSource,blitFramebuffer);
          }
        }
        gridCoordinatePairs = gridCoordinatePairs + 2;
        markerPointCount--;
      } while (markerPointCount != 0);
      g_GraphicsFramebufferEndAccess();
    }
  }
  return;
}


/* Address: 0x0052F5A0.
   Draws the SELECTION_OVERLAY_MARKER_WORLD_POINT marker centred on the projected nearest terrain point (or top
   surface point when useTopSurface is nonzero) of a world position; nothing when it is off the field. Called by
   FrontendModelPointerContext_RenderWorldViewQueuesClipped with the point in its surfaceHitWorldY/X when
   context flag 0x100000 is set and surfaceHitDepth is not WORLD_POINTER_NO_HIT; useTopSurface is set when the
   high byte of g_UiCommandModeGColorVariantLimit is nonzero.
*/
void SelectionOverlay_DrawWorldPointMarker
          (UiPixelCoordinate clipBottom,UiPixelCoordinate clipRight,UiPixelCoordinate clipTop,
          UiPixelCoordinate clipLeft,int useTopSurface,Q12 worldYQ12,Q12 worldXQ12,
          FieldGridAsset *fieldGrid)

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
    terrainPoint = FieldGrid_GetNearestTerrainPoint(worldYQ12,worldXQ12,fieldGrid);
    pointZ = terrainPoint.terrainHeightQ12;
    pointY = terrainPoint.worldYQ12;
    pointX = terrainPoint.worldXQ12;
    if (terrainPoint.outOfBounds) {
      return;
    }
  }
  else {
    topSurfacePoint = FieldGrid_GetNearestTopSurfacePoint(worldYQ12,worldXQ12,fieldGrid);
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
  accessFailed = g_GraphicsFramebufferBeginAccess(); /* Ghidra passed stale register values (worldYQ12, worldXQ12, fieldGrid); the callee takes none */
  if (!accessFailed) {
    markerSize = g_GraphicsTextureSourceGetLogicalSize(SELECTION_OVERLAY_MARKER_WORLD_POINT,
                                                       g_SelectionPanelTextureSource);
    g_SelectionPanelBlitOpaque
              (clipBottom,clipRight,clipTop,clipLeft,
               (projectedPoint.projectedY >> 12) - ((int)markerSize.logicalHeightPixels >> 1),
               (projectedPoint.projectedX >> 12) - ((int)markerSize.logicalWidthPixels >> 1),
               SELECTION_OVERLAY_MARKER_WORLD_POINT,g_SelectionPanelTextureSource,g_FramebufferAccess);
    g_GraphicsFramebufferEndAccess();
  }
  return;
}


/* Address: 0x0052F680.
   Draws the SELECTION_OVERLAY_MARKER_GRID_VERTEX marker at the projected position of every fourth field cell in
   both directions (rows and columns 1, 5, 9, ...), using the cells' projected point B instead of A
   when the high byte of g_UiCommandModeGColorVariantLimit is nonzero; cells whose point A was not projected are
   skipped. Called by FrontendModelPointerContext_RenderWorldViewQueuesClipped when context flag 0x800000 is set.
*/
void SelectionOverlay_DrawGridVertexMarkers
          (UiPixelCoordinate clipBottom,UiPixelCoordinate clipRight,UiPixelCoordinate clipTop,
          UiPixelCoordinate clipLeft,FieldGridAsset *fieldGrid)

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
    /* &fieldGrid->cells[gridColumns + 1]: row 1, column 1 (the FieldGridAsset header, then the cells) */
    vertexCursor = fieldGrid[1].common.buildMetadata.reserved28_2F + gridColumns * sizeof(FieldGridCell) + -0x28;
    columnsRemaining = columnCount;
    rowStart = vertexCursor;
    if ((g_UiCommandModeGColorVariantLimit & 0xff000000) != 0) {
      coordinateOffset = 32; /* projectedPointB (+0x2C) instead of projectedPointA (+0x0C) */
    }
    do {
      do {
        if ((*(uint32_t *)(vertexCursor + 80) & TERRAIN_VERTEX_POINT_A_NOT_PROJECTED) == 0) {
          screenX = *(int *)(vertexCursor + coordinateOffset + 12) >> Q12_SHIFT;
          screenY = *(int *)(vertexCursor + coordinateOffset + 16) >> Q12_SHIFT;
          blitTextureId = SELECTION_OVERLAY_MARKER_GRID_VERTEX;
          blitTextureSource = g_SelectionPanelTextureSource;
          blitFramebuffer = g_FramebufferAccess;
          markerSize = g_GraphicsTextureSourceGetLogicalSize(SELECTION_OVERLAY_MARKER_GRID_VERTEX,
                                                             g_SelectionPanelTextureSource);
          g_SelectionPanelBlitOpaque
                    (clipBottom,clipRight,clipTop,clipLeft,
                     screenY - ((int)markerSize.logicalHeightPixels >> 1),
                     screenX - ((int)markerSize.logicalWidthPixels >> 1),blitTextureId,blitTextureSource,blitFramebuffer);
        }
        vertexCursor = vertexCursor + 4 * sizeof(FieldGridCell); /* four cells on */
        columnsRemaining = columnsRemaining - 1;
      } while (-1 < (int)columnsRemaining); /* gridWidth / 4 + 1 cells per row */
      vertexCursor = rowStart + gridColumns * 4 * sizeof(FieldGridCell); /* four rows on */
      rowsRemaining = rowsRemaining - 1;
      columnsRemaining = columnCount;
      rowStart = vertexCursor;
    } while (-1 < (int)rowsRemaining);
    g_GraphicsFramebufferEndAccess();
  }
  return;
}


/* Address: 0x0052F780.
   Marks the field cells excluded from the fluid simulation: SELECTION_OVERLAY_MARKER_FLUID_RECEIVER_EXCLUDED and/or
   SELECTION_OVERLAY_MARKER_FLUID_SOURCE_EXCLUDED at the cell's projected point B, skipping cells whose point B was
   not projected. Called by FrontendModelPointerContext_RenderWorldViewQueuesClipped when context flag 0x1000000
   is set.
*/
void SelectionOverlay_DrawFluidExclusionMarkers
          (UiPixelCoordinate clipBottom,UiPixelCoordinate clipRight,UiPixelCoordinate clipTop,
          UiPixelCoordinate clipLeft,FieldGridAsset *fieldGrid)

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
        if (((cellCursor->flagsAndMaterial & TERRAIN_VERTEX_POINT_B_NOT_PROJECTED) == 0) &&
           ((cellCursor->flagsAndMaterial &
            (FIELD_CELL_FLUID_SOURCE_EXCLUDED|FIELD_CELL_FLUID_RECEIVER_EXCLUDED)) != 0)) {
          screenX = cellCursor->secondarySurfaceScreenPoint.projectedX >> 12;
          screenY = cellCursor->secondarySurfaceScreenPoint.projectedY >> 12;
          sourceTextureId = SELECTION_OVERLAY_MARKER_FLUID_SOURCE_EXCLUDED;
          receiverTextureId = SELECTION_OVERLAY_MARKER_FLUID_RECEIVER_EXCLUDED;
          sourceTextureSource = g_SelectionPanelTextureSource;
          sourceFramebuffer = g_FramebufferAccess;
          if ((cellCursor->flagsAndMaterial & FIELD_CELL_FLUID_RECEIVER_EXCLUDED) != 0) {
            receiverTextureSource = g_SelectionPanelTextureSource;
            receiverFramebuffer = g_FramebufferAccess;
            savedScreenY = screenY;
            savedScreenX = screenX;
            markerSize = g_GraphicsTextureSourceGetLogicalSize(SELECTION_OVERLAY_MARKER_FLUID_RECEIVER_EXCLUDED,
                                                               g_SelectionPanelTextureSource);
            g_SelectionPanelBlitOpaque
                      (clipBottom,clipRight,clipTop,clipLeft,
                       screenY - ((int)markerSize.logicalHeightPixels >> 1),
                       screenX - ((int)markerSize.logicalWidthPixels >> 1),receiverTextureId,receiverTextureSource,receiverFramebuffer);
            screenY = savedScreenY;
            screenX = savedScreenX;
          }
          if ((cellCursor->flagsAndMaterial & FIELD_CELL_FLUID_SOURCE_EXCLUDED) != 0) {
            markerSize = g_GraphicsTextureSourceGetLogicalSize(SELECTION_OVERLAY_MARKER_FLUID_SOURCE_EXCLUDED,
                                                               g_SelectionPanelTextureSource);
            g_SelectionPanelBlitOpaque
                      (clipBottom,clipRight,clipTop,clipLeft,
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
   Marks the field cells that support Xenite or Tritium: SELECTION_OVERLAY_MARKER_SELECTED_RESOURCE when the cell
   supports the resource selectedResourceIndex picks (0 Xenite, 1 Tritium), SELECTION_OVERLAY_MARKER_OTHER_RESOURCE
   when it supports the other one, at the cell's projected point A. Called by
   FrontendModelPointerContext_RenderWorldViewQueuesClipped with the low byte of its selectedResourceMarkerIndex when
   context flag 0x2000000 is set.
*/
void SelectionOverlay_DrawResourceCellMarkers
          (UiPixelCoordinate clipBottom,UiPixelCoordinate clipRight,UiPixelCoordinate clipTop,
          UiPixelCoordinate clipLeft,uint8_t selectedResourceIndex,FieldGridAsset *fieldGrid)

{
  FieldGridDimension gridColumns;
  int screenX;
  FieldGridDimension columnsRemaining;
  int screenY;
  FieldGridDimension rowsRemaining;
  FieldGridCell *cellCursor;
  FieldCellPackedFlagsAndMaterial selectedResourceFlag;
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
  
  selectedResourceFlag = FIELD_CELL_XENITE_SUPPORT << (selectedResourceIndex & 31);
  accessFailed = g_GraphicsFramebufferBeginAccess();
  if (!accessFailed) {
    gridColumns = fieldGrid->gridWidth;
    rowsRemaining = fieldGrid->gridHeight;
    cellCursor = fieldGrid->cells;
    columnsRemaining = gridColumns;
    rowStartCell = cellCursor;
    do {
      do {
        if (((cellCursor->flagsAndMaterial & TERRAIN_VERTEX_POINT_A_NOT_PROJECTED) == 0) &&
           ((cellCursor->flagsAndMaterial & FIELD_CELL_XENITE_OR_TRITIUM_SUPPORT_MASK) != 0)) {
          screenX = cellCursor->groundScreenPoint.projectedX >> 12;
          screenY = cellCursor->groundScreenPoint.projectedY >> 12;
          otherTextureId = SELECTION_OVERLAY_MARKER_OTHER_RESOURCE;
          flaggedTextureId = SELECTION_OVERLAY_MARKER_SELECTED_RESOURCE;
          otherTextureSource = g_SelectionPanelTextureSource;
          otherFramebuffer = g_FramebufferAccess;
          if ((cellCursor->flagsAndMaterial & selectedResourceFlag) != 0) {
            flaggedTextureSource = g_SelectionPanelTextureSource;
            flaggedFramebuffer = g_FramebufferAccess;
            savedScreenY = screenY;
            savedScreenX = screenX;
            markerSize = g_GraphicsTextureSourceGetLogicalSize(SELECTION_OVERLAY_MARKER_SELECTED_RESOURCE,
                                                               g_SelectionPanelTextureSource);
            g_SelectionPanelBlitOpaque
                      (clipBottom,clipRight,clipTop,clipLeft,
                       screenY - ((int)markerSize.logicalHeightPixels >> 1),
                       screenX - ((int)markerSize.logicalWidthPixels >> 1),flaggedTextureId,flaggedTextureSource,flaggedFramebuffer);
            screenY = savedScreenY;
            screenX = savedScreenX;
          }
          if ((cellCursor->flagsAndMaterial & (selectedResourceFlag ^ FIELD_CELL_XENITE_OR_TRITIUM_SUPPORT_MASK)) != 0)
          {
            markerSize = g_GraphicsTextureSourceGetLogicalSize(SELECTION_OVERLAY_MARKER_OTHER_RESOURCE,
                                                               g_SelectionPanelTextureSource);
            g_SelectionPanelBlitOpaque
                      (clipBottom,clipRight,clipTop,clipLeft,
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
   Marks the field cells with FIELD_CELL_INIT_CLEARED_UNRESOLVED_BIT15 with the
   SELECTION_OVERLAY_MARKER_FLUID_RECEIVER_EXCLUDED marker at their projected point A. Called by
   FrontendModelPointerContext_RenderWorldViewQueuesClipped when context flag 0x4000 and g_UiCommandRuntimeFlags
   bit 0x40 are set and a field grid is attached.
*/
void SelectionOverlay_DrawUnresolvedCellMarkers
          (UiPixelCoordinate clipBottom,UiPixelCoordinate clipRight,UiPixelCoordinate clipTop,
          UiPixelCoordinate clipLeft,FieldGridAsset *fieldGrid)

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
           ((cellCursor->flagsAndMaterial & TERRAIN_VERTEX_POINT_A_NOT_PROJECTED) == 0)) {
          screenX = cellCursor->groundScreenPoint.projectedX >> 12;
          screenY = cellCursor->groundScreenPoint.projectedY >> 12;
          blitTextureId = SELECTION_OVERLAY_MARKER_FLUID_RECEIVER_EXCLUDED;
          blitTextureSource = g_SelectionPanelTextureSource;
          blitFramebuffer = g_FramebufferAccess;
          markerSize = g_GraphicsTextureSourceGetLogicalSize(SELECTION_OVERLAY_MARKER_FLUID_RECEIVER_EXCLUDED,
                                                             g_SelectionPanelTextureSource);
          g_SelectionPanelBlitOpaque
                    (clipBottom,clipRight,clipTop,clipLeft,
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
   Pointer-mode handler 3 (g_InGamePointerModeHandlers[3]; networked games queue it as a command
   instead): SelectionPointerArray_SetAircraftPadTargets with lane mask 3 (lanes 1 and 2) on the player's
   selection, which stores the pointer point and heading as the marker target of those linked-child lanes
   in every selected class-0x16 army.
*/
void SelectionMarkerCoordinates_ApplyType3(SelectionMarkerIndex playerId,SelectionMarkerCoordinateValue32 heading,
          SelectionMarkerCoordinateValue32 worldXQ12,SelectionMarkerCoordinateValue32 worldYQ12)

{
  SelectionPointerArray_SetAircraftPadTargets
            (3,heading,worldXQ12,worldYQ12,
             &g_SelectionPlayerRuntimeBlockPointers[playerId]->selection);
  return;
}


/* Address: 0x00560050.
   Pointer-mode handler 4 (g_InGamePointerModeHandlers[4]; networked games queue it as a command
   instead): SelectionPointerArray_SetAircraftPadTargets with lane mask 4 (lane 4) on the player's
   selection, which stores the pointer point and heading as the marker target of those linked-child lanes
   in every selected class-0x16 army.
*/
void SelectionMarkerCoordinates_ApplyType4(SelectionMarkerIndex playerId,SelectionMarkerCoordinateValue32 heading,
          SelectionMarkerCoordinateValue32 worldXQ12,SelectionMarkerCoordinateValue32 worldYQ12)

{
  SelectionPointerArray_SetAircraftPadTargets
            (4,heading,worldXQ12,worldYQ12,
             &g_SelectionPlayerRuntimeBlockPointers[playerId]->selection);
  return;
}


/* Address: 0x00560080.
   Pointer-mode handler 5 (g_InGamePointerModeHandlers[5]; networked games queue it as a command
   instead): SelectionPointerArray_SetAircraftPadTargets with lane mask 5 (lanes 1 and 4) on the player's
   selection, which stores the pointer point and heading as the marker target of those linked-child lanes
   in every selected class-0x16 army.
*/
void SelectionMarkerCoordinates_ApplyType5(SelectionMarkerIndex playerId,SelectionMarkerCoordinateValue32 heading,
          SelectionMarkerCoordinateValue32 worldXQ12,SelectionMarkerCoordinateValue32 worldYQ12)

{
  SelectionPointerArray_SetAircraftPadTargets
            (5,heading,worldXQ12,worldYQ12,
             &g_SelectionPlayerRuntimeBlockPointers[playerId]->selection);
  return;
}


/* Address: 0x005600B0.
   Pointer-mode handler 6 (g_InGamePointerModeHandlers[6]; networked games queue it as a command
   instead): SelectionPointerArray_SetAircraftPadTargets with lane mask 6 (lanes 2 and 4) on the player's
   selection, which stores the pointer point and heading as the marker target of those linked-child lanes
   in every selected class-0x16 army.
*/
void SelectionMarkerCoordinates_ApplyType6(SelectionMarkerIndex playerId,SelectionMarkerCoordinateValue32 heading,
          SelectionMarkerCoordinateValue32 worldXQ12,SelectionMarkerCoordinateValue32 worldYQ12)

{
  SelectionPointerArray_SetAircraftPadTargets
            (6,heading,worldXQ12,worldYQ12,
             &g_SelectionPlayerRuntimeBlockPointers[playerId]->selection);
  return;
}


/* Address: 0x005600E0.
   Pointer-mode handler 7 (g_InGamePointerModeHandlers[7]; networked games queue it as a command
   instead): SelectionPointerArray_SetAircraftPadTargets with lane mask 7 (lanes 1, 2 and 4) on the player's
   selection, which stores the pointer point and heading as the marker target of those linked-child lanes
   in every selected class-0x16 army.
*/
void SelectionMarkerCoordinates_ApplyType7(SelectionMarkerIndex playerId,SelectionMarkerCoordinateValue32 heading,
          SelectionMarkerCoordinateValue32 worldXQ12,SelectionMarkerCoordinateValue32 worldYQ12)

{
  SelectionPointerArray_SetAircraftPadTargets
            (7,heading,worldXQ12,worldYQ12,
             &g_SelectionPlayerRuntimeBlockPointers[playerId]->selection);
  return;
}


/* Address: 0x00568210.
   Places a command-target marker effect on the terrain at a world point, unless the point is the source node's
   own position or already carries a marker. Markers for a scaleQ12 other than 1.0 are lifted and scaled so the
   model's bounding radius becomes 6.5 * scaleQ12 (Q12).
*/
void InGameWorldOverlay_EnsureTransientEffectMarkerAtPoint
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
  if ((worldXQ12 != ((ModelRuntimeNode *)sourceWorldNode)->worldTransform.translation.x) ||
     (worldYQ12 != ((ModelRuntimeNode *)sourceWorldNode)->worldTransform.translation.y)) {
    /* each marker is an EffectRuntimeSlot * */
    markerCursor = (int *)g_InGameCommandTargetTransientEffectMarkers;
    for (remainingMarkers = g_InGameCommandTargetTransientEffectMarkerCount; remainingMarkers != 0; remainingMarkers--) {
      if ((worldXQ12 ==
           ((EffectRuntimeSlot *)*markerCursor)->modelNodeOrSavedOffset.modelNode->worldTransform.translation.x) &&
         (worldYQ12 ==
          ((EffectRuntimeSlot *)*markerCursor)->modelNodeOrSavedOffset.modelNode->worldTransform.translation.y)) {
        return;
      }
      markerCursor++;
    }
    surfaceHeight = FieldGrid_InterpolateTopSurfaceHeight
                      (worldYQ12,worldXQ12,((WorldRuntimeContext *)inGameRuntime)->fieldGrid);
    createdEffect = EffectRuntimePool_CreateInstanceFromDefinition
                      (EFFECT_RUNTIME_COMPLETION_NONE,THANDOR_BITCAST(int, EffectRuntimeOwnerReference, 0),0,FIXED_ANGLE16_QUARTER_TURN,0,
                       surfaceHeight.heightQ12,worldYQ12,worldXQ12,effectDefinition,inGameRuntime);
    g_InGameCommandTargetTransientEffectMarkers[markerSlotIndex] = createdEffect.effectRuntime;
    markerModelNode = ((createdEffect.effectRuntime)->modelNodeOrSavedOffset).modelNode;
    g_InGameCommandTargetTransientEffectMarkerCount++;
    boundingRadius = markerModelNode->subtreeBoundingRadiusQ12;
    markerModelNode->tintArgb = 0xffffffff;
    if ((scaleQ12 != Q12_ONE) && (boundingRadius != 0)) {
      translationZ = &(markerModelNode->worldTransform).translation.z;
      *translationZ = *translationZ + 324;
      markerModelNode->runtimeFlags = markerModelNode->runtimeFlags | MODEL_RUNTIME_FLAG_APPLY_SCALE;
      /* unsigned 32x32->64 MUL / DIV, as in the original; 6656 is 6.5 in Q12 */
      markerModelNode->modelScaleQ12 = (Q12)(((uint64_t)(uint32_t)scaleQ12 * 6656) / (uint64_t)boundingRadius);
    }
  }
  return;
}


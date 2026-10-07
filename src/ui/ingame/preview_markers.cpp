/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/src/ui/ingame/preview_markers.cpp
 * Reverse engineering by idkFoxes 2026
 */

#include <thandor/ui/ingame/preview_markers.h>
#include <thandor/thandor.h>
#include <thandor/platform/bootstrap/image.h>
#include <thandor/platform/debug/hooks.h>

/* Module data. */

intptr_t g_InGamePendingPlacementArmyAsset = 0;

uint32_t g_InGameCommandPreviewArmyAssetId = 0;

static EffectRuntimeSlot *g_InGameOwnedEntityTransientEffectMarkers[32] = {};

static uint32_t g_InGameOwnedEntityTransientEffectMarkerCount = 0;

static EffectRuntimeSlot *g_InGameCommandTargetTransientEffectMarkers[128] = {};

static uint32_t g_InGameCommandTargetTransientEffectMarkerCount = 0;

static GameEntityRuntime *g_InGamePlacementPreviewArmyRuntime = nullptr;

static GameEntityRuntime *g_InGameCommandPreviewArmyRuntime = nullptr;

/* The army asset record staged in g_InGamePendingPlacementArmyAsset (kept as an integer, 0 when none). */
static inline ArmyAssetRecordPrefix *InGameWorldOverlay_PendingPlacementRecord(intptr_t pendingPlacementAsset)
{
  return reinterpret_cast<ArmyAssetRecordPrefix *>(pendingPlacementAsset); /* the address stored as an integer */
}

/* Command mode: builds (releaseMode == GRAPHICS_STATE_DISABLED) or releases the ghost of the army the previewed
   pointer-mode command would create. */
static void InGameWorldOverlay_UpdateCommandPreviewArmy
          (GraphicsBooleanState releaseMode,WorldRuntimeContext *worldRuntime)

{
  ArmyRuntimeSlot *createdArmy;
  ModelRuntimeNode *previewModelNode;

  if (releaseMode != GRAPHICS_STATE_DISABLED) {
    if (g_InGameCommandPreviewArmyRuntime != nullptr) {
      ArmyRuntime_DestroyInstanceAndRefreshUi(worldRuntime,g_InGameCommandPreviewArmyRuntime);
      g_InGameCommandPreviewArmyRuntime = nullptr;
    }
    return;
  }
  g_InGameCommandPreviewArmyRuntime = nullptr;
  if ((((g_InGamePointerInteractionStateFlags &
         (WORLD_POINTER_STATE_OVER_OWN_ARMY | WORLD_POINTER_STATE_SELECTION_CAPTURE)) == 0) &&
      (g_InGameCommandPreviewArmyAssetId != 0)) &&
     (g_InGameCommandPreviewSurfaceHeightQ12OrSentinel != WORLD_POINTER_NO_HIT)) {
    createdArmy = ArmyRuntime_CreateInstanceFromAsset
                       (1,g_InGameCommandPreviewHeading16,g_InGameCommandPreviewWorldXQ12,
                        g_InGameCommandPreviewWorldYQ12,
                        worldRuntime->activeFactionRuntimeIndex,
                        g_InGameCommandPreviewArmyAssetId,worldRuntime,nullptr);
    if (createdArmy != nullptr) {
      previewModelNode = (ModelView_Cast<GameEntityRuntime>(createdArmy)->common).ownership.modelNode;
      g_InGameCommandPreviewArmyRuntime = ModelView_Cast<GameEntityRuntime>(createdArmy);
      previewModelNode->tintArgb = OVERLAY_PREVIEW_TINT_ARGB;
      ModelNodeRuntime_RebuildTransformsFromRoot(previewModelNode);
    }
  }
}

/* Placement mode: builds the ghost of the pending army at the (possibly snapped) placement point.
   pendingPlacementAsset is g_InGamePendingPlacementArmyAsset as read on entry to the overlay callback. */
static void InGameWorldOverlay_BuildPlacementPreviewArmy
          (intptr_t pendingPlacementAsset,WorldRuntimeContext *worldRuntime)

{
  PackedArgb32 *childTint;
  ModelRuntimeNode *previewModelNode;
  ModelDefinition *armyDefinition;
  ArmyPlacementCandidateCount acceptedCandidateCount;
  GameEntityRuntime *previewArmy;
  Q12 validatedWorldXQ12;
  Q12 validatedWorldYQ12;
  PackedArgb32 previewTint;
  Bool8 validated;
  Bool8 placeable;
  PckArmyAssetIdCatalog armyAssetId;

  g_InGamePlacementPreviewArmyRuntime = nullptr;
  if ((g_InGamePendingPlacementArmyAsset == 0) ||
     (g_InGamePlacementSurfaceHeightQ12OrSentinel == WORLD_POINTER_NO_HIT)) {
    return;
  }
  previewTint = OVERLAY_PREVIEW_TINT_ARGB;
  g_ArmyPlacementLateRejectionCount = 1;
  validated = ArmyPlacement_ValidateAssetAtPointAndCellCorners
                   (0,g_InGamePlacementHeading16,g_InGamePlacementWorldXQ12,
                    g_InGamePlacementWorldYQ12,
                    static_cast<ArmyPlacementContext>(InGameWorldOverlay_PendingPlacementRecord(g_InGamePendingPlacementArmyAsset)->registryId),
                    worldRuntime->activeFactionRuntimeIndex,worldRuntime);
  acceptedCandidateCount = g_ArmyPlacementLateRejectionCount;
  if (validated) {
    g_ArmyPlacementLateRejectionCount = 0;
    if (acceptedCandidateCount < 2) {
      /* Original quirk: a validated point with a single accepted candidate gets no ghost at all */
      return;
    }
    previewTint = OVERLAY_PREVIEW_TINT_MULTI_CANDIDATE_ARGB;
  }
  armyAssetId = InGameWorldOverlay_PendingPlacementRecord(pendingPlacementAsset)->registryId;
  /* the point the validator accepted (possibly snapped) */
  validatedWorldXQ12 = g_ArmyPlacementValidatedWorldXQ12;
  validatedWorldYQ12 = g_ArmyPlacementValidatedWorldYQ12;
  g_ArmyPlacementLateRejectionCount = 1;
  placeable = ArmyPlacement_CanPlaceAssetAtFieldPoint
                   (1,0,g_InGamePlacementHeading16,validatedWorldYQ12,validatedWorldXQ12,armyAssetId,
                    worldRuntime->activeFactionRuntimeIndex,worldRuntime,nullptr);
  if ((!placeable) && (g_ArmyPlacementLateRejectionCount < 2)) {
    previewTint = previewTint & OVERLAY_PREVIEW_TINT_BLOCKED_MASK;
  }
  g_ArmyPlacementLateRejectionCount = 0;
  previewArmy = ModelView_Cast<GameEntityRuntime>(ArmyRuntime_CreateInstanceFromAsset
                     (1,g_InGamePlacementHeading16,validatedWorldYQ12,validatedWorldXQ12,
                      worldRuntime->activeFactionRuntimeIndex,armyAssetId,worldRuntime,nullptr));
  if (previewArmy == nullptr) {
    return;
  }
  previewModelNode = (previewArmy->common).ownership.modelNode;
  g_InGamePlacementPreviewArmyRuntime = previewArmy;
  previewModelNode->tintArgb = previewTint;
  armyDefinition = THANDOR_PTR32_AT(ModelDefinition, (previewArmy->common).ownership.definitionOrClassRecord);
  ModelNodeRuntime_RebuildTransformsFromRoot(previewModelNode);
  if (((armyDefinition->runtimeClassId == MODEL_RUNTIME_CLASS_13) &&
      (3 < previewModelNode->childCount)) &&
     (previewModelNode->childNodes[3] != nullptr)) {
    /* class-13 armies keep child node 3 opaque */
    childTint = &previewModelNode->childNodes[3]->tintArgb;
    *childTint = *childTint | 0xff000000;
  }
}

/* Places an EGATH0 marker at the exit point of every own class-13 army with a rally point set (at most
   OVERLAY_OWNED_MARKER_CAPACITY). Returns false when the world owner list is empty. */
static Bool8 InGameWorldOverlay_BuildOwnedEntityMarkers(WorldRuntimeContext *worldRuntime)

{
  ModelRuntimeNode *ownerNode;
  ModelRuntimeSlot *factoryModelRuntime;
  EffectDefinition *markerDefinition;
  EffectRuntimeSlot *createdEffect;
  Q12 surfaceHeightQ12;
  uint32_t markerIndex;

  if (EffectDefinitionRegistry_FindById(EFF_0143_EGATH0,&markerDefinition) != 0) {
    return true;
  }
  ownerNode = WorldNode_View<ModelRuntimeNode>(worldRuntime->ownerListHead.get());
  if (ownerNode == nullptr) {
    return false;
  }
  markerIndex = 0;
  for (; ownerNode != nullptr; ownerNode = WorldNode_View<ModelRuntimeNode>((ownerNode->common).nextNode.get())) {
    if (ownerNode->ownerClassId != WORLD_OWNER_RUNTIME_MODEL) {
      continue;
    }
    factoryModelRuntime = (ownerNode->runtimePayload).modelRuntime;
    if (factoryModelRuntime->definitionOrSavedId.runtimeDefinition->runtimeClassId != MODEL_RUNTIME_CLASS_13) {
      continue;
    }
    if (!Any((factoryModelRuntime->classState).stateFlags & ARMY_MODEL_STATE_RALLY_POINT_SET) ||
        (factoryModelRuntime->ownerArmyRuntimeOrSavedOffset.armyRuntime->factionIndex !=
         worldRuntime->activeFactionRuntimeIndex)) {
      continue;
    }
    /* the unit factory's exit point: X in classLinkState.classState78, Y in classState7C */
    FieldGrid_InterpolateTopSurfaceHeight
              ((factoryModelRuntime->classLinkState).classState7C,
               (factoryModelRuntime->classLinkState).classState78,worldRuntime->fieldGrid,&surfaceHeightQ12);
    createdEffect = EffectRuntimePool_CreateInstanceFromDefinition
                       (EFFECT_RUNTIME_COMPLETION_NONE,THANDOR_COMPOUND(EffectRuntimeOwnerReference){ .modelNode = nullptr },0,
                        FIXED_ANGLE16_QUARTER_TURN,0,surfaceHeightQ12,(factoryModelRuntime->classLinkState).classState7C,
                        (factoryModelRuntime->classLinkState).classState78,markerDefinition,
                        worldRuntime);
    if (createdEffect != nullptr) {
      g_InGameOwnedEntityTransientEffectMarkers[markerIndex] = createdEffect;
      markerIndex = markerIndex + 1;
      createdEffect->modelNodeOrSavedOffset.modelNode->tintArgb = 0xffffffff;
      g_InGameOwnedEntityTransientEffectMarkerCount =
           g_InGameOwnedEntityTransientEffectMarkerCount + 1;
      if (OVERLAY_OWNED_MARKER_CAPACITY - 1 < markerIndex) break;
    }
  }
  return true;
}

static Bool8 InGameWorldOverlay_CommandTargetMarkersFull()

{
  return static_cast<Bool8>(OVERLAY_COMMAND_TARGET_MARKER_CAPACITY - 1 < g_InGameCommandTargetTransientEffectMarkerCount);
}

/* Places the waypoint (EWAYP0) and target (ETARG0) markers of the selected own armies. The markers use scale
   0x1000 (1.0 in Q12) except at an army target, which uses the target's own marker scale (its definition's
   footprintRadius); stops once OVERLAY_COMMAND_TARGET_MARKER_CAPACITY markers exist. */
static void InGameWorldOverlay_BuildCommandTargetMarkers(WorldRuntimeContext *worldRuntime)

{
  EffectDefinition *waypointDefinition;
  EffectDefinition *targetDefinition;
  Ptr32<GameEntityRuntime> *selectionSlots;
  GameEntityRuntime *entityRuntime;
  GameEntityRuntime *commandTargetEntity;
  ModelRuntimeNode *targetModelNode;
  int slotIndex;
  uint32_t waypointIndex;

  if (EffectDefinitionRegistry_FindById(EFF_0148_EWAYP0,&waypointDefinition) != 0) {
    return;
  }
  if (EffectDefinitionRegistry_FindById(EFF_0149_ETARG0,&targetDefinition) != 0) {
    return;
  }
  selectionSlots = g_SelectionInfoEntitySlots->entries;
  for (slotIndex = 0; slotIndex < SELECTION_ENTRY_CAPACITY; slotIndex++) {
    entityRuntime = selectionSlots[slotIndex];
    if ((entityRuntime == nullptr) ||
       (worldRuntime->activeFactionRuntimeIndex != (entityRuntime->common).ownership.ownerIndex)) {
      continue;
    }
    if (((entityRuntime->common).ownership.modelRuntime()->definitionOrSavedId.
         runtimeDefinition->accelerationPerTick != 0)
       && (((entityRuntime->common).commandFlags & ToBits(ARMY_MOVEMENT_ACTIVE)) != 0)) {
      InGameWorldOverlay_EnsureTransientEffectMarkerAtPoint
                (Q12_ONE,(entityRuntime->common).ownership.modelNode,
                 (entityRuntime->common).pathCoordinate1Q12,(entityRuntime->common).pathCoordinate0Q12,
                 waypointDefinition,worldRuntime);
      if (InGameWorldOverlay_CommandTargetMarkersFull()) {
        return;
      }
      if (((entityRuntime->common).commandFlags & ToBits(ARMY_MOVEMENT_WAYPOINTS_QUEUED)) != 0) {
        /* further waypoints: the army's queued waypoints (at least one is visited) */
        waypointIndex = 0;
        /* Original quirk: a do/while, so it runs once even with a count of 0 (kept as in the original; step 11). */
        do {
          InGameWorldOverlay_EnsureTransientEffectMarkerAtPoint
                    (Q12_ONE,(entityRuntime->common).ownership.modelNode,
                     ModelView_Cast<ArmyMovementRuntime>(entityRuntime)->queuedWaypoints[waypointIndex].worldYQ12,
                     ModelView_Cast<ArmyMovementRuntime>(entityRuntime)->queuedWaypoints[waypointIndex].worldXQ12,
                     waypointDefinition,worldRuntime);
          waypointIndex++;
          if (InGameWorldOverlay_CommandTargetMarkersFull()) {
            return;
          }
        } while (waypointIndex < ModelView_Cast<ArmyMovementRuntime>(entityRuntime)->queuedWaypointCount);
      }
    }
    if (((entityRuntime->common).commandTarget.targetFlags & 2) != 0) {
      InGameWorldOverlay_EnsureTransientEffectMarkerAtPoint
                (Q12_ONE,(entityRuntime->common).ownership.modelNode,
                 (entityRuntime->common).commandTarget.targetWorldYQ12,
                 (entityRuntime->common).commandTarget.targetWorldXQ12,targetDefinition,
                 worldRuntime);
      if (InGameWorldOverlay_CommandTargetMarkersFull()) {
        return;
      }
    }
    commandTargetEntity = (entityRuntime->common).commandTarget.targetEntity;
    if ((((entityRuntime->common).commandTarget.targetFlags & 1) != 0) && (commandTargetEntity != nullptr)) {
      targetModelNode = (commandTargetEntity->common).ownership.modelNode;
      InGameWorldOverlay_EnsureTransientEffectMarkerAtPoint
                ((commandTargetEntity->common).ownership.modelRuntime()->
                 definitionOrSavedId.runtimeDefinition->footprintRadius,
                 (entityRuntime->common).ownership.modelNode,
                 (targetModelNode->worldTransform).translation.y,
                 (targetModelNode->worldTransform).translation.x,targetDefinition,
                 worldRuntime);
      if (InGameWorldOverlay_CommandTargetMarkersFull()) {
        return;
      }
    }
  }
}

/* Detaches the model node of each of the markerCount markers from the world, forgets it and resets the count. */
static void InGameWorldOverlay_ReleaseMarkers(EffectRuntimeSlot **markers,uint32_t *markerCount)

{
  EffectRuntimeSlot *effectSlot;
  ModelRuntimeNode *markerModelNode;
  uint32_t markerTotal;
  uint32_t markerIndex;

  markerTotal = *markerCount;
  if (markerTotal == 0) {
    return;
  }
  for (markerIndex = 0; markerIndex < markerTotal; markerIndex++) {
    effectSlot = markers[markerIndex];
    markerModelNode = (effectSlot->modelNodeOrSavedOffset).modelNode;
    InterpolationState_SetNegatedTargetAndRescaleProgress(0,markerModelNode->shadingRecord);
    WorldRuntime_UnlinkOwnerListNode(WorldNode_View<WorldOwnerListNode>(markerModelNode));
    (effectSlot->modelNodeOrSavedOffset).modelNode = nullptr;
  }
  *markerCount = 0;
}

/* World overlay callback: builds (releaseMode == GRAPHICS_STATE_DISABLED) or releases the transient objects
   drawn over the map - the ghost army previewing a placement or command-mode command, EGATH0 markers at the
   movement target of own class-13 armies (runtime flag 0x800), and the waypoint (EWAYP0) and target (ETARG0)
   markers of the selected own armies.
*/
void InGameWorldOverlay_RebuildOrReleaseTransientMarkers
          (GraphicsBooleanState releaseMode,WorldRuntimeContext *worldRuntime)

{
  intptr_t pendingPlacementAsset;

  /* developer tools (not in the original): no render-path objects while the determinism test hashes the
     simulation */
  if (DebugHook_SuppressTransientMarkers()) {
    return;
  }
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
    if (!SelectionInfo_TestNotOwnAircraftPadsWithAircraft(worldRuntime->activeFactionRuntimeIndex)) {
      InGameWorldOverlay_UpdateCommandPreviewArmy(releaseMode,worldRuntime);
    }
  }
  else if (releaseMode == GRAPHICS_STATE_DISABLED) {
    InGameWorldOverlay_BuildPlacementPreviewArmy(pendingPlacementAsset,worldRuntime);
  }
  else if (g_InGamePlacementPreviewArmyRuntime != nullptr) {
    ArmyRuntime_DestroyInstanceAndRefreshUi(worldRuntime,g_InGamePlacementPreviewArmyRuntime);
    g_InGamePlacementPreviewArmyRuntime = nullptr;
  }
  if (releaseMode == GRAPHICS_STATE_DISABLED) {
    if (!InGameWorldOverlay_BuildOwnedEntityMarkers(worldRuntime)) {
      /* Original quirk: with an empty world owner list no waypoint or target markers are built either */
      return;
    }
    InGameWorldOverlay_BuildCommandTargetMarkers(worldRuntime);
  }
  else {
    /* release: detach every marker's model node from the world and forget it */
    InGameWorldOverlay_ReleaseMarkers
              (g_InGameOwnedEntityTransientEffectMarkers,&g_InGameOwnedEntityTransientEffectMarkerCount);
    InGameWorldOverlay_ReleaseMarkers
              (g_InGameCommandTargetTransientEffectMarkers,&g_InGameCommandTargetTransientEffectMarkerCount);
  }
}

/* Places a command-target marker effect on the terrain at a world point, unless the point is the source node's
   own position or already carries a marker. Markers for a scaleQ12 other than 1.0 are lifted and scaled so the
   model's bounding radius becomes 6.5 * scaleQ12 (Q12).
*/
void InGameWorldOverlay_EnsureTransientEffectMarkerAtPoint
          (Q12 scaleQ12,ModelRuntimeNode *sourceWorldNode,Q12 worldYQ12,Q12 worldXQ12,
          EffectDefinition *effectDefinition,WorldRuntimeContext *inGameRuntime)

{
  GraphicsWorldCoordinateQ12 *translationZ;
  ModelRuntimeNode *markerModelNode;
  uint32_t boundingRadius;
  int markerSlotIndex;
  int remainingMarkers;
  EffectRuntimeSlot **markerCursor;
  ModelRuntimeNode *existingMarkerNode;
  Q12 surfaceHeightQ12;
  EffectRuntimeSlot *createdEffect;

  markerSlotIndex = g_InGameCommandTargetTransientEffectMarkerCount;
  if ((worldXQ12 == sourceWorldNode->worldTransform.translation.x) &&
     (worldYQ12 == sourceWorldNode->worldTransform.translation.y)) {
    return;
  }
  markerCursor = g_InGameCommandTargetTransientEffectMarkers;
  for (remainingMarkers = g_InGameCommandTargetTransientEffectMarkerCount; remainingMarkers != 0; remainingMarkers--) {
    existingMarkerNode = (*markerCursor)->modelNodeOrSavedOffset.modelNode;
    if ((worldXQ12 == existingMarkerNode->worldTransform.translation.x) &&
       (worldYQ12 == existingMarkerNode->worldTransform.translation.y)) {
      return;
    }
    markerCursor++;
  }
  FieldGrid_InterpolateTopSurfaceHeight
            (worldYQ12,worldXQ12,inGameRuntime->fieldGrid,&surfaceHeightQ12);
  createdEffect = EffectRuntimePool_CreateInstanceFromDefinition
                    (EFFECT_RUNTIME_COMPLETION_NONE,THANDOR_COMPOUND(EffectRuntimeOwnerReference){ .modelNode = nullptr },0,FIXED_ANGLE16_QUARTER_TURN,0,
                     surfaceHeightQ12,worldYQ12,worldXQ12,effectDefinition,
                     inGameRuntime);
  if (createdEffect == nullptr) {
    /* The original has no failure check and stores and dereferences its failure value
       (FATAL_ERROR_GENERAL_FAILURE); bounded here because a full effect or world object pool would crash:
       no marker is placed. */
    static bool s_markerCreationFailureLogged = false;
    if (!s_markerCreationFailureLogged) {
      s_markerCreationFailureLogged = true;
      Thandor_Log("preview markers: effect or object pool full, command-target marker skipped");
    }
    return;
  }
  g_InGameCommandTargetTransientEffectMarkers[markerSlotIndex] = createdEffect;
  markerModelNode = createdEffect->modelNodeOrSavedOffset.modelNode;
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

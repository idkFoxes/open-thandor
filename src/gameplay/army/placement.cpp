/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/src/gameplay/army/placement.cpp
 * Reverse engineering by idkFoxes 2026
 */

#include <thandor/gameplay/army/placement.h>
#include <thandor/thandor.h>
#include <thandor/platform/bootstrap/image.h>

/* Module data. */

ArmyPlacementCandidateCount g_ArmyPlacementLateRejectionCount = 0;

/* Placement test for models with a second footprint: runs the common candidate test
   (ArmyPlacement_CanPlaceBuilding), then rotates the model's (1,5) anchor point by the
   placement heading and requires ARMY_PLACEMENT_ANCHOR_CLEARANCE_Q12 of free room there, both from other
   armies and from the terrain limits of the definition's contact kind. Returns true when the point is
   accepted and stores the common test's value in *outPlacementValue; returns false (*outPlacementValue
   untouched) when it is rejected.
   Reached through g_ArmyRuntimeOrderHandlerMatrix11Columns24Classes.placementAssetClassDispatch[13],
   called by ArmyPlacement_CanPlaceAssetAtFieldPoint.
*/

Bool8 ArmyPlacement_CanPlaceAnchoredModel
              (ArmyPlacementDispatchArg0 placementMode,
              ArmyPlacementClearancePaddingQ12 placementClearancePaddingQ12,
              ArmyPlacementDispatchArg2 placementHeading,ArmyPlacementDispatchArg3 terrainHeightQ12,
              Q12 worldXQ12,Q12 worldYQ12,ModelDefinition *modelDefinition,
              ArmyPlacementDispatchArg7 ownerFactionIndex,WorldRuntimeContext *worldRuntime,
              uint32_t *outPlacementValue)

{
  int offsetWorldXQ12;
  int offsetWorldYQ12;
  Bool8 blocked;
  FixedLengthAngle offsetLengthAngle;
  FixedSinCos rotatedOffset;
  uint32_t clearanceValue;
  ModelPackedPointRecord *anchorRecord;
  TerrainPlacementResult terrainTest;

  if (!ArmyPlacement_CanPlaceBuilding
                    (placementMode,placementClearancePaddingQ12,placementHeading,terrainHeightQ12,
                     worldXQ12,worldYQ12,modelDefinition,ownerFactionIndex,worldRuntime,
                     &clearanceValue)) {
    return false;
  }
  /* the model resource of the definition's root node.
     Original quirk: the found flag is not checked; without a (1,5) point the record just past the
     point table is read. */
  ModelLookupTable_FindPackedPoint
            (1,5,Thandor_U32ToPointer<MdlSerializedNodeHeader>(modelDefinition->rootNodeOffsetOrPointer)-> /* 32-bit format field: ModelDefinition.rootNodeOffsetOrPointer */
                 spriteAssetReference.modelResource,&anchorRecord);
  offsetLengthAngle = FixedMath_Vector2AngleAndLength
                    ((anchorRecord->localPosition).y,(anchorRecord->localPosition).x);
  rotatedOffset = FixedMath_SinCosScaled(offsetLengthAngle.angle + placementHeading & FIXED_ANGLE16_MASK,offsetLengthAngle.length);
  offsetWorldXQ12 = worldXQ12 + rotatedOffset.sinValue;
  offsetWorldYQ12 = worldYQ12 + rotatedOffset.cosValue;
  blocked = ArmyPlacementCollision_TestPointAgainstRuntimeList
                    (placementMode,ARMY_PLACEMENT_ANCHOR_CLEARANCE_Q12,offsetWorldXQ12,offsetWorldYQ12,
                     worldRuntime);
  if (!blocked) {
    terrainTest.rejected = (*g_TerrainClassPlacementAndOverlayCallbacks10.placementTests
              [modelDefinition->placementContactKindIndex])
                      (ARMY_PLACEMENT_ANCHOR_CLEARANCE_Q12,terrainHeightQ12,offsetWorldXQ12,
                       offsetWorldYQ12,worldRuntime->fieldGrid);
    if (!terrainTest.rejected) {
      *outPlacementValue = clearanceValue;
      return true;
    }
  }
  /* the counter counts these late rejections; the placement preview
     (InGameWorldOverlay) tints the ghost by it */
  g_ArmyPlacementLateRejectionCount++;
  return false;
}

/* Validates a placed model with a second footprint (the live counterpart of
   ArmyPlacement_CanPlaceAnchoredModel): the common test ArmyPlacementCollision_TestCurrentRuntime,
   then ARMY_PLACEMENT_ANCHOR_CLEARANCE_Q12 of room around the model's (1,5) anchor point in world space,
   free of other armies and within the terrain limits (TerrainAuxHeightThreshold_TestAroundWorldPoint for
   contact kind 1 = water surface, TerrainHeightBand_TestAroundWorldPoint otherwise). Returns true when rejected
   (also when the model has no anchor point).
   Reached through g_ArmyRuntimeOrderHandlerMatrix11Columns24Classes.placementValidation[13],
   called by ArmyRuntimeNode_DispatchTypedCallback.
*/

Bool8 ArmyPlacement_TestModelTerrainAndRuntimeClearance
          (WorldRuntimeContext *worldRuntime,ModelRuntimePlacementValidationView *modelRuntime)

{
  ModelRuntimeNode *modelNodeRuntime;
  ModelDefinition *placementDefinition;
  uint32_t worldYQ12;
  Q12 worldXQ12;
  int referenceHeightQ12;
  Bool8 blocked;
  ModelPackedPointRecord *anchorRecord;
  ModelWorldPoint anchorWorldPoint;

  modelNodeRuntime = modelRuntime->rootModelNode;
  blocked = ArmyPlacementCollision_TestCurrentRuntime(worldRuntime,modelRuntime);
  if (!blocked) {
    placementDefinition = modelRuntime->modelDefinition;
    if (ModelLookupTable_FindPackedPoint(1,5,(modelNodeRuntime->modelPayload).modelResource,&anchorRecord)) {
      anchorWorldPoint = ModelNodeRuntime_TransformLocalPoint(anchorRecord,modelNodeRuntime);
      worldXQ12 = anchorWorldPoint.yQ12;
      worldYQ12 = anchorWorldPoint.xQ12;
      /* the anchor's height with the definition's and the resource's height offsets taken off */
      referenceHeightQ12 =
           (anchorWorldPoint.zQ12 - placementDefinition->placementHeightOffsetQ12) -
           ((modelNodeRuntime->modelPayload).modelResource)->placementHeightOffsetQ12;
      blocked = ArmyPlacementCollision_TestCandidateAgainstRuntimeList
                        (ModelView_Cast<WorldOwnerListNode>(modelNodeRuntime),worldXQ12,worldYQ12,
                         nullptr,ARMY_PLACEMENT_ANCHOR_CLEARANCE_Q12,worldRuntime);
      if (!blocked) {
        if (modelRuntime->modelDefinition->placementContactKindIndex == ARMY_PLACEMENT_CONTACT_KIND_WATER_SURFACE) {
          blocked = TerrainAuxHeightThreshold_TestAroundWorldPoint
                            (ARMY_PLACEMENT_ANCHOR_CLEARANCE_Q12,referenceHeightQ12,worldXQ12,worldYQ12,
                             worldRuntime->fieldGrid);
          if (!blocked) {
            return false;
          }
        }
        else {
          blocked = TerrainHeightBand_TestAroundWorldPoint
                            (ARMY_PLACEMENT_ANCHOR_CLEARANCE_Q12,referenceHeightQ12,worldXQ12,worldYQ12,
                             worldRuntime->fieldGrid);
          if (!blocked) {
            return false;
          }
        }
      }
    }
    g_ArmyPlacementLateRejectionCount++;
  }
  return true;
}

Q12 g_ArmyPlacementValidatedWorldXQ12;
Q12 g_ArmyPlacementValidatedWorldYQ12;

/* Tests whether an army asset can be placed at a point (the placement cursor, a build command): first at
   the point itself, then at four points around it (the point rounded down to a multiple of 0x100, plus 0 or
   0x240 on each axis). Returns false when one fits and leaves the accepted point in
   g_ArmyPlacementValidatedWorldXQ12/YQ12; true when none fits.
*/
Bool8 ArmyPlacement_ValidateAssetAtPointAndCellCorners
          (ArmyPlacementMode placementMode,uint32_t placementHeading,Q12 worldYQ12,
          Q12 worldXQ12,PckArmyAssetIdCatalog armyAssetId,FactionRuntimeIndex ownerFactionId,
          void *inGameRuntime)

{
  /* Besides the result, the function hands back the point it accepted - the input point, or the first free
     snapped cell corner. Callers read it from g_ArmyPlacementValidatedWorldX/YQ12. */
  /* corner order as in the original: (0,0), (+0x240,0), (+0x240,+0x240), (0,+0x240) */
  static const int cornerDx[4] = {0,ARMY_PLACEMENT_CORNER_OFFSET_Q12,ARMY_PLACEMENT_CORNER_OFFSET_Q12,0};
  static const int cornerDy[4] = {0,0,ARMY_PLACEMENT_CORNER_OFFSET_Q12,ARMY_PLACEMENT_CORNER_OFFSET_Q12};
  int corner;

  g_ArmyPlacementValidatedWorldXQ12 = worldXQ12;
  g_ArmyPlacementValidatedWorldYQ12 = worldYQ12;
  if (ArmyPlacement_CanPlaceAssetAtFieldPoint
                         (placementMode,0,placementHeading,worldYQ12,worldXQ12,armyAssetId,
                          ownerFactionId,static_cast<WorldRuntimeContext *>(inGameRuntime),nullptr)) {
    return false;
  }
  for (corner = 0; corner < 4; corner++) {
    Q12 x = (Q12)(((uint32_t)worldXQ12 & 0xffffff00) + cornerDx[corner]);
    Q12 y = (Q12)(((uint32_t)worldYQ12 & 0xffffff00) + cornerDy[corner]);
    if (ArmyPlacement_CanPlaceAssetAtFieldPoint
                           (placementMode,0,placementHeading,y,x,armyAssetId,ownerFactionId,
                            static_cast<WorldRuntimeContext *>(inGameRuntime),nullptr)) {
      g_ArmyPlacementValidatedWorldXQ12 = x;
      g_ArmyPlacementValidatedWorldYQ12 = y;
      return false;
    }
  }
  g_ArmyPlacementValidatedWorldXQ12 = worldXQ12;
  g_ArmyPlacementValidatedWorldYQ12 = worldYQ12;
  return true;
}

/* Placement test for resource extractors: after the common candidate test
   (ArmyPlacement_CanPlaceBuilding) the field-grid cell under the point must carry the
   deposit bit the definition asks for (FIELD_CELL_XENITE_SUPPORT << selector classParameterC0: 0 Xenite,
   1 Tritium).
   Returns true when the point is accepted and stores the common test's value in *outPlacementValue;
   returns false (*outPlacementValue untouched) when it is rejected.
   Reached through g_ArmyRuntimeOrderHandlerMatrix11Columns24Classes.placementAssetClassDispatch[14],
   called by ArmyPlacement_CanPlaceAssetAtFieldPoint.
*/
Bool8 ArmyPlacement_CanPlaceResourceExtractor
              (ArmyPlacementDispatchArg0 placementMode,
              ArmyPlacementClearancePaddingQ12 placementClearancePaddingQ12,
              ArmyPlacementDispatchArg2 placementHeading,ArmyPlacementDispatchArg3 terrainHeightQ12,
              Q12 worldYQ12,Q12 worldXQ12,ModelDefinitionRecordPrefix *modelDefinition,
              ArmyPlacementDispatchArg7 ownerFactionIndex,WorldRuntimeContext *worldRuntime,
              uint32_t *outPlacementValue)

{
  uint32_t clearanceValue;
  int cellColumn;
  uint32_t projectedRowTerm;
  int cellRow;
  FieldGridAsset *activeFieldGrid;

  if (!ArmyPlacement_CanPlaceBuilding
                    (placementMode,placementClearancePaddingQ12,placementHeading,terrainHeightQ12,
                     worldYQ12,worldXQ12,ModelView_Cast<ModelDefinition>(modelDefinition),
                     ownerFactionIndex,worldRuntime,&clearanceValue)) {
    return false;
  }
  activeFieldGrid = worldRuntime->fieldGrid;
  /* FieldGrid_WorldToGridQ12 inlined (skewed grid: the column is shifted by half the row), rounded to the
     nearest cell */
  projectedRowTerm = FIXED_MUL_SHR(worldYQ12,FIELD_GRID_WORLD_Y_TO_ROW_Q20,Q20_SHIFT + 1);
  cellColumn = (int)((FIXED_MUL_SHR(worldXQ12,FIELD_GRID_WORLD_X_TO_COLUMN_Q20,Q20_SHIFT) - projectedRowTerm) + FIELD_GRID_CELL_Q12 / 2) >> Q12_SHIFT;
  cellRow = (int)(projectedRowTerm * 2 + FIELD_GRID_CELL_Q12 / 2) >> Q12_SHIFT;
  if ((cellColumn >= 0) && (cellRow >= 0) &&
      (cellColumn < (int)activeFieldGrid->gridWidth) && (cellRow < (int)activeFieldGrid->gridHeight)) {
    /* the resource field selector of an extractor is its class parameter classParameterC0 */
    if ((activeFieldGrid->cells[(int32_t)(activeFieldGrid->gridWidth * cellRow + cellColumn)].flagsAndMaterial &
        FIELD_CELL_XENITE_SUPPORT << ((uint8_t)ModelView_Cast<ModelDefinition>(modelDefinition)->classParameterC0 & 31)) != 0) {
      *outPlacementValue = clearanceValue;
      return true;
    }
  }
  g_ArmyPlacementLateRejectionCount++;
  return false;
}

/* Validates a placed resource extractor (the live counterpart of ArmyPlacement_CanPlaceResourceExtractor):
   the common test ArmyPlacementCollision_TestCurrentRuntime, then the field-grid cell under the model (not on
   the grid border) must carry the deposit bit FIELD_CELL_XENITE_SUPPORT << selector
   (resourceFieldSupportSelector: 0 Xenite, 1 Tritium). Returns true when rejected.
   Reached through g_ArmyRuntimeOrderHandlerMatrix11Columns24Classes.placementValidation[14],
   called by ArmyRuntimeNode_DispatchTypedCallback.
*/
Bool8 ArmyPlacement_TestGridOccupancyMask
          (WorldRuntimeContext *worldRuntime,ModelRuntimePlacementClass14View *modelRuntime)

{
  int cellColumn;
  int cellRow;
  Bool8 blocked;
  FieldGridCoordinates gridCoordinates;
  FieldGridAsset *activeFieldGrid;
  ModelRuntimeNode *rootNode;
  
  rootNode = modelRuntime->rootModelNode;
  blocked = ArmyPlacementCollision_TestCurrentRuntime
                    (worldRuntime,ModelView_Cast<ModelRuntimePlacementValidationView>(modelRuntime));
  if (!blocked) {
    gridCoordinates = FieldGrid_WorldToGridQ12
                      ((rootNode->worldTransform).translation.y,
                       (rootNode->worldTransform).translation.x);
    /* Q12 grid coordinates rounded to the nearest cell */
    cellColumn = ((gridCoordinates.columnQ12 >> (Q12_SHIFT - 1)) + 1) >> 1;
    cellRow = ((gridCoordinates.rowQ12 >> (Q12_SHIFT - 1)) + 1) >> 1;
    activeFieldGrid = worldRuntime->fieldGrid;
    if ((0 < cellColumn) && (0 < cellRow)) {
      if ((cellColumn + 1 < (int)activeFieldGrid->gridWidth) &&
         ((cellRow + 1 < (int)activeFieldGrid->gridHeight &&
          ((activeFieldGrid->cells[(int32_t)(cellRow * activeFieldGrid->gridWidth + cellColumn)].flagsAndMaterial &
           FIELD_CELL_XENITE_SUPPORT <<
           ((uint8_t)modelRuntime->modelDefinition->resourceFieldSupportSelector & 31)) != 0)))) {
        return false;
      }
    }
    g_ArmyPlacementLateRejectionCount++;
  }
  return true;
}

/* Validates the position of a mobile unit (ground, tracked, walker, glider and water classes): its grid cell
   must pass the definition's cell-mask bands (footprintRadiusClass/terrainTraversalClass), no other army may
   overlap it (ArmyCollision_FindBlockingRuntimeForCurrentUnit), and, unless
   UI_COMMAND_RUNTIME_FLAG_INTERACTION_SUBSYSTEM_ACTIVE is set, the field-grid point must not be blocked for
   the owner's faction. Returns true when blocked.
   Reached through g_ArmyRuntimeOrderHandlerMatrix11Columns24Classes.placementValidation[1..3, 17..19],
   called by ArmyRuntimeNode_DispatchTypedCallback.
*/
Bool8 ArmyPlacement_TestGridRuntimeAndFieldBlocking
          (WorldRuntimeContext *worldRuntime,ModelRuntimePlacementValidationView *modelRuntime)

{
  Bool8 blocked;
  ModelRuntimeNode *modelNode;
  
  modelNode = modelRuntime->rootModelNode;
  blocked = GridScratch_TestProjectedCellMaskBands
                    ((modelNode->worldTransform).translation.y,
                     (modelNode->worldTransform).translation.x,
                     (uint8_t)modelRuntime->modelDefinition->footprintRadiusClass,
                     (uint8_t)modelRuntime->modelDefinition->terrainTraversalClass);
  if (blocked) {
    return true;
  }
  if (ArmyCollision_FindBlockingRuntimeForCurrentUnit
                    ((modelNode->worldTransform).translation.y,
                     (modelNode->worldTransform).translation.x,
                     ModelView_Cast<RuntimeCollisionQueryView>(modelRuntime),worldRuntime) != nullptr) {
    return true;
  }
  if ((g_UiCommandRuntimeFlags & UI_COMMAND_RUNTIME_FLAG_INTERACTION_SUBSYSTEM_ACTIVE) != 0) {
    return false;
  }
  return FieldGrid_TestWorldPointBlocked
                    (modelRuntime->ownerArmyRuntime->factionIndex,
                     (modelNode->worldTransform).translation.y,
                     (modelNode->worldTransform).translation.x,worldRuntime->fieldGrid);
}

/* Placement test for a mobile unit (ground, tracked, walker, glider and water classes): the point must pass the
   definition's cell-mask bands (footprintRadiusClass/terrainTraversalClass), be free of other armies
   (ArmyCollision_TestPointAgainstRuntimeList) and, unless UI_COMMAND_RUNTIME_FLAG_INTERACTION_SUBSYSTEM_ACTIVE
   is set, not be blocked on the field grid for the owner's faction. Returns true when the point is accepted and
   stores 0 in *outPlacementValue; returns false (*outPlacementValue untouched) when it is blocked.
   Reached through g_ArmyRuntimeOrderHandlerMatrix11Columns24Classes.placementAssetClassDispatch[1..3, 17..19],
   called by ArmyPlacement_CanPlaceAssetAtFieldPoint.
*/
Bool8 ArmyPlacement_CanPlaceMobileUnit
               (uint32_t placementMode,uint32_t placementClearancePaddingQ12,uint32_t placementHeading,
               uint32_t terrainHeightQ12,
               Q12 worldXQ12,Q12 worldYQ12,ModelDefinition *modelDefinition,
               ArmyPlacementDispatchArg7 ownerFactionIndex,WorldRuntimeContext *worldRuntime,
               uint32_t *outPlacementValue)

{
  Bool8 blocked;
  
  blocked = GridScratch_TestProjectedCellMaskBands
                    (worldXQ12,worldYQ12,(uint8_t)modelDefinition->footprintRadiusClass,
                     (uint8_t)modelDefinition->terrainTraversalClass);
  if (blocked) {
    return false;
  }
  blocked = ArmyCollision_TestPointAgainstRuntimeList
                    (worldXQ12,worldYQ12,modelDefinition,worldRuntime);
  if (blocked) {
    return false;
  }
  if ((g_UiCommandRuntimeFlags & UI_COMMAND_RUNTIME_FLAG_INTERACTION_SUBSYSTEM_ACTIVE) == 0) {
    blocked = FieldGrid_TestWorldPointBlocked
                      (ownerFactionIndex,worldXQ12,worldYQ12,worldRuntime->fieldGrid);
    if (blocked) {
      return false;
    }
  }
  *outPlacementValue = 0;
  return true;
}

/* Placement test of the classes that can be placed anywhere: accepts at once (returns true and stores 0 in
   *outPlacementValue).
   Reached through g_ArmyRuntimeOrderHandlerMatrix11Columns24Classes.placementAssetClassDispatch[0, 5..9,
   12, 21], called by ArmyPlacement_CanPlaceAssetAtFieldPoint.
*/
Bool8 ArmyPlacement_CanPlaceAnywhere
               (uint32_t placementMode,uint32_t placementClearancePaddingQ12,uint32_t placementHeading,
               uint32_t terrainHeightQ12,
               Q12 worldXQ12,Q12 worldYQ12,ModelDefinitionRecordPrefix *modelDefinition,
               uint32_t ownerFactionIndex,WorldRuntimeContext *worldRuntime,
               uint32_t *outPlacementValue)

{
  *outPlacementValue = 0;
  return true;
}

/* Looks up the army asset and its model definition, samples the terrain height at the point with the
   definition's interpolation callback (index placementContactKindIndex), and hands the placement test to the
   handler of the definition's class (runtimeClassId) in the placement dispatch table. Returns true when that
   handler accepts the point and stores the handler's value (a placement count for the counting modes 3 and 7) in
   *outPlacementValue (may be NULL); returns false (*outPlacementValue untouched) when the handler rejects
   the point or the asset or its model definition is missing. No caller used the lookup error code the
   original returned on failure.
*/
Bool8 ArmyPlacement_CanPlaceAssetAtFieldPoint(ArmyPlacementMode placementMode,
          ArmyPlacementClearancePaddingQ12 placementClearancePaddingQ12,
          uint32_t placementHeading,Q12 worldYQ12,Q12 worldXQ12,
          PckArmyAssetIdCatalog armyAssetId,FactionRuntimeIndex ownerFactionIndex,
          WorldRuntimeContext *worldRuntime,uint32_t *outPlacementValue)

{
  uint32_t assetClassIndex;
  ModelDefinitionRecordPrefix *modelDefinition;
  ArmyAssetRecordPrefix *armyAsset;
  Q12 terrainHeightQ12;
  uint32_t placementValue;

  if (ArmyAssetRegistry_FindById(armyAssetId,&armyAsset) != 0) {
    return false;
  }
  /* the model definition of the army asset's root node */
  modelDefinition = ModelDefinitionRegistry_FindById
                    (Thandor_U32ToPointer<AiLinkedDefinitionListView>(armyAsset->rootNodeOffsetOrPointer)->definitionIds[0]); /* 32-bit format field: ArmyAssetRecord.rootNodeOffsetOrPointer */
  if (modelDefinition == nullptr) {
    return false;
  }
  assetClassIndex = ModelView_Cast<ModelDefinition>(modelDefinition)->runtimeClassId;
  /* placementContactKindIndex selects the height interpolation mode on the world's field grid */
  (*g_FieldGridInterpolationCallbacks5.callbacks[ModelView_Cast<ModelDefinition>(modelDefinition)->placementContactKindIndex])
            (worldYQ12,worldXQ12,worldRuntime->fieldGrid,&terrainHeightQ12);
  if (!(*g_ArmyRuntimeOrderHandlerMatrix11Columns24Classes.placementAssetClassDispatch[assetClassIndex])
            (placementMode,placementClearancePaddingQ12,placementHeading,terrainHeightQ12,
             worldYQ12,worldXQ12,modelDefinition,ownerFactionIndex,worldRuntime,
             &placementValue)) {
    return false;
  }
  if (outPlacementValue != nullptr) {
    *outPlacementValue = placementValue;
  }
  return true;
}

/* Common placement test for buildings: the candidate point must be free of other armies (placement radius
   footprintRadius), pass the terrain test of the definition's contact kind against terrainHeightQ12, and -
   unless UI_COMMAND_RUNTIME_FLAG_INTERACTION_SUBSYSTEM_ACTIVE is set - not be blocked on the field grid for the
   owner's faction and lie within reach of a same-faction model with a support radius (supportRadius of its
   definition, plus placementClearancePaddingQ12 and our own margin placementFlags; class-18 models only count when
   ARMY_PLACEMENT_MODE_SKIP_CLASS18_SUPPORT is clear and their army flags 0x18 are clear). Without
   ARMY_PLACEMENT_MODE_MEASURE_SUPPORT_DISTANCE the first such model accepts; with it the placement value is
   the free distance to the nearest one. Returns true when the point is accepted and stores the placement
   value in *outPlacementValue; returns false (*outPlacementValue untouched) when it is rejected.
   Reached through g_ArmyRuntimeOrderHandlerMatrix11Columns24Classes.placementAssetClassDispatch[4, 10, 11,
   15, 16, 20, 22, 23], called by ArmyPlacement_CanPlaceAssetAtFieldPoint; also called directly
   by ArmyPlacement_CanPlaceAnchoredModel and ArmyPlacement_CanPlaceResourceExtractor.
*/

Bool8 ArmyPlacement_CanPlaceBuilding
          (ArmyPlacementDispatchArg0 placementMode,
          ArmyPlacementClearancePaddingQ12 placementClearancePaddingQ12,uint32_t placementHeading,
          ArmyPlacementDispatchArg3 terrainHeightQ12,Q12 worldXQ12,Q12 worldYQ12,
          ModelDefinition *modelDefinition,
          ArmyPlacementDispatchArg7 ownerFactionIndex,WorldRuntimeContext *worldRuntime,
          uint32_t *outPlacementValue)

{
  ArmyPlacementContactKindIndex32 contactKindIndex;
  ModelRuntimeSlot *supporter;
  ModelDefinition *supporterDefinition;
  int supportReachQ12;
  int reachQ12;
  int deltaQ12;
  int64_t reachSquared;
  int64_t remainingSquared;
  int64_t deltaYSquared;
  int64_t distanceSquared;
  int64_t nearestDistanceSquared;
  uint32_t nearestDistanceQ12;
  int freeDistanceQ12;
  int placementValue;
  WorldOwnerListNode *ownerNode;
  Bool8 blocked;
  TerrainPlacementResult terrainTest;
  int nearestClearanceQ12 = 0; /* set together with nearestDistanceSquared */

  nearestDistanceSquared = INT64_MAX;
  contactKindIndex = modelDefinition->placementContactKindIndex;
  blocked = ArmyPlacementCollision_TestPointAgainstRuntimeList
                    (placementMode,modelDefinition->footprintRadius,worldXQ12,worldYQ12,
                     worldRuntime);
  if (blocked) {
    return false;
  }
  terrainTest.value = 0;
  terrainTest.rejected = g_TerrainClassPlacementAndOverlayCallbacks10.placementTests[contactKindIndex]
                    (modelDefinition->footprintRadius,terrainHeightQ12,worldXQ12,
                     worldYQ12,worldRuntime->fieldGrid);
  if (terrainTest.rejected) {
    return false;
  }
  if ((g_UiCommandRuntimeFlags & UI_COMMAND_RUNTIME_FLAG_INTERACTION_SUBSYSTEM_ACTIVE) != 0) {
    *outPlacementValue = (uint32_t)terrainTest.value;
    return true;
  }
  ownerNode = worldRuntime->ownerListHead;
  blocked = FieldGrid_TestWorldPointBlocked
                    (ownerFactionIndex,worldXQ12,worldYQ12,worldRuntime->fieldGrid);
  if (blocked) {
    return false;
  }
  if (ownerNode == nullptr) {
    *outPlacementValue = (uint32_t)terrainTest.value;
    return true;
  }
  for (; ownerNode != nullptr; ownerNode = ownerNode->nextNode) {
    if (ownerNode->ownerClassId != WORLD_OWNER_RUNTIME_MODEL) {
      continue;
    }
    /* the model's definition: support radius (supportRadius) and class id; the owner army holds the owner
       faction */
    supporter = WorldOwnerNode_ModelRuntime(ownerNode);
    supporterDefinition = supporter->definitionOrSavedId.runtimeDefinition;
    supportReachQ12 = supporterDefinition->supportRadius;
    if ((supportReachQ12 == 0) ||
        (supporter->ownerArmyRuntimeOrSavedOffset.armyRuntime->factionIndex != ownerFactionIndex)) {
      continue;
    }
    supportReachQ12 = supportReachQ12 + placementClearancePaddingQ12;
    if ((supporterDefinition->runtimeClassId == MODEL_RUNTIME_CLASS_18) &&
        (((placementMode & ARMY_PLACEMENT_MODE_SKIP_CLASS18_SUPPORT) != 0) ||
         (Any(supporter->classState.stateFlags &
           (ARMY_RUNTIME_FLAG_DESTROYED | ARMY_MODEL_STATE_DISMANTLING))))) {
      continue;
    }
    reachQ12 = supportReachQ12 + modelDefinition->placementFlags;
    reachSquared = (int64_t)reachQ12 * (int64_t)reachQ12;
    deltaQ12 = ownerNode->worldXQ12 - worldYQ12;
    remainingSquared = reachSquared - (int64_t)deltaQ12 * (int64_t)deltaQ12;
    if (remainingSquared < 0) {
      continue;
    }
    deltaQ12 = ownerNode->worldYQ12 - worldXQ12;
    deltaYSquared = (int64_t)deltaQ12 * (int64_t)deltaQ12;
    remainingSquared = remainingSquared - deltaYSquared;
    if (remainingSquared < 0) {
      continue;
    }
    if ((placementMode & ARMY_PLACEMENT_MODE_MEASURE_SUPPORT_DISTANCE) == 0) {
      /* Original quirk: the accepted placement value is the low dword of the squared y delta (a left-over
         intermediate value). */
      *outPlacementValue = (uint32_t)(int)deltaYSquared;
      return true;
    }
    /* keep the nearest supporting model (smallest squared distance) */
    distanceSquared = reachSquared - remainingSquared;
    if (distanceSquared < nearestDistanceSquared) {
      nearestClearanceQ12 = supporterDefinition->supportRadius;
      nearestDistanceSquared = distanceSquared;
    }
  }
  /* no supporting model in reach (the distance is still the INT64_MAX start value) */
  if (((placementMode & ARMY_PLACEMENT_MODE_MEASURE_SUPPORT_DISTANCE) == 0) ||
      (ARMY_PLACEMENT_NO_SUPPORT_DISTANCE_SQUARED < nearestDistanceSquared)) {
    return false;
  }
  /* placement value: free distance between the candidate's margin and the nearest supporter's radius */
  nearestDistanceQ12 = FixedMath_UInt64Sqrt((UInt64Half32)((uint64_t)nearestDistanceSquared >> 32),
                                            (UInt64Half32)nearestDistanceSquared);
  freeDistanceQ12 = nearestDistanceQ12 - nearestClearanceQ12;
  if (freeDistanceQ12 < 0) {
    freeDistanceQ12 = 0;
  }
  placementValue = freeDistanceQ12 - modelDefinition->placementFlags;
  if (placementValue < 0) {
    placementValue = 0;
  }
  *outPlacementValue = (uint32_t)placementValue;
  return true;
}

/* Tests whether a point comes within queryRadiusQ12 + ARMY_PLACEMENT_ANCHOR_CLEARANCE_Q12 of the model's
   (1,5) anchor point in world space - the second footprint of class-13 models. Returns true when too close
   (and counts it in g_ArmyPlacementLateRejectionCount); false also when the model has no anchor point.
   Called directly by ArmyPlacementCollision_TestPointAgainstRuntimeList and
   ArmyPlacementCollision_TestCandidateAgainstRuntimeList.
*/
Bool8 ArmyPlacementCandidate_TestModelAnchorDistance
          (Q12 queryRadiusQ12,Q12 targetWorldXQ12,Q12 targetWorldYQ12,ModelRuntimeSlot *modelRuntime)

{
  ModelRuntimeNode *modelNodeRuntime;
  uint32_t anchorDistanceQ12;
  ModelPackedPointRecord *anchorRecord;
  ModelWorldPoint anchorWorldPoint;

  modelNodeRuntime = modelRuntime->rootModelNodeOrSavedOffset.modelNode;
  if (ModelLookupTable_FindPackedPoint(1,5,(modelNodeRuntime->modelPayload).modelResource,&anchorRecord)) {
    anchorWorldPoint = ModelNodeRuntime_TransformLocalPoint(anchorRecord,modelNodeRuntime);
    anchorDistanceQ12 = FixedMath_Length2(anchorWorldPoint.yQ12 - targetWorldXQ12,
                                          anchorWorldPoint.xQ12 - targetWorldYQ12);
    if ((int)anchorDistanceQ12 <= queryRadiusQ12 + ARMY_PLACEMENT_ANCHOR_CLEARANCE_Q12) {
      g_ArmyPlacementLateRejectionCount++;
      return true;
    }
  }
  return false;
}

/* In-game command INGAME_COMMAND_PLACEMENT_CREATE_ARMY (map click while placing an army in command mode 3/4, from
   InGameUiCommand_BeginInteractionByMode): creates army armyAssetId at the clicked position for the faction set
   by PlayerRuntime_SetPlacementFaction and keeps it as the player's placed army (placedArmyToken, as an offset from
   g_ArmyRuntimeRebaseBaseMinusOne), or 0 when it could not be created.
*/
void PlayerRuntime_CreatePlacementArmy(PlayerRuntimeId playerRuntimeId,PlayerStateLookupValue0 worldXQ12,
          PlayerStateLookupValue1 worldYQ12,RuntimeToken armyAssetId)

{
  SelectionPlayerRuntimeBlock *playerBlock;
  ArmyRuntimeSlot *createdRuntime;

  playerBlock = g_SelectionPlayerRuntimeBlockPointers[playerRuntimeId];
  createdRuntime = ArmyRuntime_CreateInstanceFromAsset
                    (ARMY_CREATE_UNLOCK_TECHNOLOGY,0,worldXQ12,worldYQ12,playerBlock->placementFactionIndex,
                     armyAssetId,
                     &g_InGameRuntimeRoot->worldRuntime,nullptr);
  if (createdRuntime != nullptr) {
    playerBlock->placedArmyToken =
         (uint32_t)(reinterpret_cast<uintptr_t>(createdRuntime) - reinterpret_cast<uintptr_t>(g_ArmyRuntimeRebaseBaseMinusOne));
    return;
  }
  playerBlock->placedArmyToken = 0;
}

/* In-game command INGAME_COMMAND_PLACEMENT_SET_FACTION (from InGameUiCommand_BeginInteractionByMode, before
   INGAME_COMMAND_PLACEMENT_CREATE_ARMY): sets the faction (placementFactionIndex) that the player's next placed
   army belongs to.
*/
void PlayerRuntime_SetPlacementFaction(PlayerRuntimeId playerRuntimeId,uint32_t unusedZero0,uint32_t unusedZero1,
          PlacementFactionIndex placementFactionIndex)

{
  g_SelectionPlayerRuntimeBlockPointers[playerRuntimeId]->placementFactionIndex = placementFactionIndex;
}

/* In-game command INGAME_COMMAND_PLACEMENT_SET_ARMY (clicking an existing army in placement sub-mode 2, from
   InGameUiCommand_BeginInteractionByMode): makes it the player's placed army (placedArmyToken); armyToken is its
   offset from g_ArmyRuntimeRebaseBaseMinusOne.
*/
void PlayerRuntime_SetPlacementArmy(PlayerRuntimeId playerRuntimeId,uint32_t unusedZero0,uint32_t unusedZero1,
          PlacedArmyToken armyToken)

{
  g_SelectionPlayerRuntimeBlockPointers[playerRuntimeId]->placedArmyToken = armyToken;
}

/* In-game command INGAME_COMMAND_PLACEMENT_CLEAR_ARMY (end of a placement interaction, from
   InGameUiCommand_EndInteractionByMode): forgets the player's placed army (placedArmyToken).
*/
void PlayerRuntime_ClearPlacementArmy(PlayerRuntimeId playerRuntimeId,uint32_t unusedZero0,uint32_t unusedZero1,
          uint32_t unusedZero2)

{
  g_SelectionPlayerRuntimeBlockPointers[playerRuntimeId]->placedArmyToken = 0;
}

/* Applies the terrain-class overlay of sourceRuntime's model definition at every model of the world's active
   faction: for each such owner-list node whose model has an overlay base (supportRadius of its
   definition), the overlay callback of the definition's terrain class runs at the node's position on the field
   grid. The extent is 0x800 << n for definitions of kind 0xE, else unlimited (-1).
*/
void WorldRuntime_EmitModelDefinitionOverlayForMatchingEntries(void *sourceRuntime,WorldRuntimeContext *worldRuntime)

{
  uint32_t overlayBaseOffset;
  TerrainClassOverlayCallback *overlayCallback;
  int modelOverlayBase;
  ModelDefinitionRecordPrefix *definitionRecord;
  WorldOwnerListNode *ownerNode;
  uint32_t overlayExtent;
  ModelRuntimeSlot *modelRuntime;

  if (sourceRuntime == nullptr) {
    return;
  }
  /* 32-bit format field: ArmyAssetRecordPrefix.rootNodeOffsetOrPointer */
  definitionRecord = ModelDefinitionRegistry_FindById
                    (Thandor_U32ToPointer<AiLinkedDefinitionListView>(
                      static_cast<ArmyAssetRecordPrefix *>(sourceRuntime)->rootNodeOffsetOrPointer)->definitionIds[0]);
  if (definitionRecord == nullptr) {
    return;
  }
  overlayExtent = UINT32_MAX;
  ownerNode = worldRuntime->ownerListHead;
  overlayBaseOffset = ModelView_Cast<ModelDefinition>(definitionRecord)->placementFlags;
  if (ownerNode == nullptr) {
    return;
  }
  if (ModelView_Cast<ModelDefinition>(definitionRecord)->runtimeClassId == MODEL_RUNTIME_CLASS_14) {
    overlayExtent =
         FIELD_CELL_XENITE_SUPPORT << ((uint8_t)ModelView_Cast<ModelDefinition>(definitionRecord)->classParameterC0 & 31);
  }
  overlayCallback = g_TerrainClassPlacementAndOverlayCallbacks10.overlayCallbacks
           [ModelView_Cast<ModelDefinition>(definitionRecord)->placementContactKindIndex];
  for (; ownerNode != nullptr; ownerNode = ownerNode->nextNode) {
    if (ownerNode->ownerClassId != WORLD_OWNER_RUNTIME_MODEL) {
      continue;
    }
    modelRuntime = WorldOwnerNode_ModelRuntime(ownerNode);
    if (worldRuntime->activeFactionRuntimeIndex !=
        modelRuntime->ownerArmyRuntimeOrSavedOffset.armyRuntime->factionIndex) {
      continue;
    }
    modelOverlayBase = modelRuntime->definitionOrSavedId.runtimeDefinition->supportRadius;
    if (modelOverlayBase != 0) {
      overlayCallback(overlayExtent,-1,modelOverlayBase + overlayBaseOffset,ownerNode->worldYQ12,
                      ownerNode->worldXQ12,worldRuntime->fieldGrid);
    }
  }
}

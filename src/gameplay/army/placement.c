/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/src/gameplay/army/placement.c
 * Reverse engineering by idkFoxes 2026
 */

#include <thandor/gameplay/army/placement.h>
#include <thandor/thandor.h>
#include <thandor/platform/bootstrap/image.h>

/* Implementation ownership: gameplay/army/placement. */

/* Address: 0x005244B0.
   Placement test for models with a second footprint: runs the common candidate test
   (ArmyPlacementCollision_TestCandidateAndClearance), then rotates the model's (1,5) anchor point by the
   placement heading and requires ARMY_PLACEMENT_ANCHOR_CLEARANCE_Q12 of free room there, both from other
   armies and from the terrain limits of the definition's contact kind. Returns the common test's value with
   CF (rejected) as the result.
   Reached through g_ArmyRuntimeOrderHandlerMatrix11Columns24Classes.placementAssetClassDispatch[13]
   (0x0051FF38), called by ArmyPlacement_DispatchAssetAtFieldPoint.
*/

PlacementCandidateResult ArmyPlacementCandidate_TestOffsetClearance
              (ArmyPlacementDispatchArg0 placementMode,
              ArmyPlacementClearancePaddingQ12 placementClearancePaddingQ12,
              ArmyPlacementDispatchArg2 placementHeading,ArmyPlacementDispatchArg3 terrainHeightQ12,
              Q12 worldXQ12,Q12 worldYQ12,ModelDefinition *modelDefinition,
              ArmyPlacementDispatchArg7 ownerFactionIndex,WorldRuntimeContext *worldRuntime)

{
  int offsetWorldXQ12;
  int eaxOrOffsetYQ12;
  bool blocked;
  FixedLengthAngle offsetLengthAngle;
  FixedSinCosEdxEax8 rotatedOffset;
  PlacementCandidateResult clearanceResult;
  ModelLookupEntryResult anchorLookup;
  TerrainPlacementResult terrainTest;
  
  PlacementCandidateResult result;

  /* The table dispatch reads EAX and CF; the decompiled int return lost CF. */
  clearanceResult = ArmyPlacementCollision_TestCandidateAndClearance
                    (placementMode,placementClearancePaddingQ12,placementHeading,terrainHeightQ12,
                     worldXQ12,worldYQ12,modelDefinition,ownerFactionIndex,worldRuntime);
  eaxOrOffsetYQ12 = clearanceResult.value;
  if (!clearanceResult.rejected) {
    /* the model resource of the definition's root node */
    anchorLookup = ModelLookupTable_ContainsPackedKey
                      (1,5,((MdlSerializedNodeHeader *)modelDefinition->rootNodeOffsetOrPointer)->
                           spriteAssetReference.modelResource);
    offsetLengthAngle = FixedMath_Vector2AngleAndLengthRegs
                      (((anchorLookup.entry)->localPosition).y,((anchorLookup.entry)->localPosition).x);
    rotatedOffset = FixedMath_SinCosScaled(offsetLengthAngle.angle + placementHeading & FIXED_ANGLE16_MASK,offsetLengthAngle.length);
    eaxOrOffsetYQ12 = (int)rotatedOffset;
    offsetWorldXQ12 = worldXQ12 + (int)(rotatedOffset >> 32);
    blocked = ArmyPlacementCollision_TestPointAgainstRuntimeList
                      (placementMode,ARMY_PLACEMENT_ANCHOR_CLEARANCE_Q12,offsetWorldXQ12,
                       worldYQ12 + eaxOrOffsetYQ12,worldRuntime);
    if (!blocked) {
      terrainTest.rejected = (*g_TerrainClassPlacementAndOverlayCallbacks10.placementTests
                [modelDefinition->placementContactKindIndex])
                        (ARMY_PLACEMENT_ANCHOR_CLEARANCE_Q12,terrainHeightQ12,offsetWorldXQ12,
                         worldYQ12 + eaxOrOffsetYQ12,worldRuntime->fieldGrid);
      if (!terrainTest.rejected) {
        result.value = clearanceResult.value;
        result.rejected = false;
        return result;
      }
    }
    /* the counter counts these late rejections; the placement preview
       (InGameWorldOverlay) tints the ghost by it */
    g_ArmyPlacementLateRejectionCount++;
  }
  result.value = eaxOrOffsetYQ12;
  result.rejected = true;
  return result;
}


/* Address: 0x00524570.
   Validates a placed model with a second footprint (the live counterpart of
   ArmyPlacementCandidate_TestOffsetClearance): the common test ArmyPlacementCollision_TestCurrentRuntime,
   then ARMY_PLACEMENT_ANCHOR_CLEARANCE_Q12 of room around the model's (1,5) anchor point in world space,
   free of other armies and within the terrain limits (TerrainAuxHeightThreshold_TestAroundWorldPoint for
   contact kind 1 = water surface, TerrainHeightBand_TestAroundWorldPoint otherwise). Returns CF: true = rejected (also when the model has no anchor point).
   Reached through g_ArmyRuntimeOrderHandlerMatrix11Columns24Classes.placementValidation[13] (0x0051FED8),
   called by ArmyRuntimeNode_DispatchTypedCallback.
*/

bool ArmyPlacement_TestModelTerrainAndRuntimeClearance
          (WorldRuntimeContext *worldRuntime,ModelRuntimePlacementValidationView *modelRuntime)

{
  ModelRuntimeNode *modelNodeRuntime;
  ModelDefinition *placementDefinition;
  uint32_t worldYQ12;
  Q12 worldXQ12;
  int referenceHeightQ12;
  bool blocked;
  ModelLookupEntryResult anchorLookup;
  ModelWorldPoint anchorWorldPoint;
  
  modelNodeRuntime = modelRuntime->rootModelNode;
  blocked = ArmyPlacementCollision_TestCurrentRuntime(worldRuntime,modelRuntime);
  if (!blocked) {
    placementDefinition = modelRuntime->modelDefinition;
    anchorLookup = ModelLookupTable_ContainsPackedKey(1,5,(modelNodeRuntime->modelPayload).modelResource)
    ;
    if (!anchorLookup.notFound) {
      anchorWorldPoint = ModelNodeRuntime_TransformLocalPointRegs(anchorLookup.entry,modelNodeRuntime);
      worldXQ12 = anchorWorldPoint.yQ12;
      worldYQ12 = anchorWorldPoint.xQ12;
      /* the anchor's height with the definition's and the resource's height offsets taken off */
      referenceHeightQ12 =
           (anchorWorldPoint.zQ12 - placementDefinition->placementHeightOffsetQ12) -
           ((modelNodeRuntime->modelPayload).modelResource)->placementHeightOffsetQ12;
      blocked = ArmyPlacementCollision_TestCandidateAgainstRuntimeList
                        ((WorldOwnerListNode *)modelNodeRuntime,worldXQ12,worldYQ12,
                         (IMAGE_DOS_HEADER *)ARMY_PLACEMENT_ANCHOR_CLEARANCE_Q12,worldRuntime);
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

/* Address: 0x0051D380.
   Tests whether an army asset can be placed at a point (the placement cursor, a build command): first at
   the point itself, then at four points around it (the point rounded down to a multiple of 0x100, plus 0 or
   0x240 on each axis). Returns false (CF clear) when one fits and leaves the accepted point in
   g_ArmyPlacementValidatedWorldXQ12/YQ12 (the original's ECX/EDX); true when none fits.
*/
bool ArmyPlacement_ValidateAssetAtPointAndCellCorners
          (ArmyPlacementMode placementMode,uint32_t placementHeading,Q12 worldYQ12,
          Q12 worldXQ12,PckArmyAssetIdCatalog armyAssetId,FactionRuntimeIndex ownerFactionId,
          void *inGameRuntime)

{
  /* Rewritten from the assembly (0x0051D380-0x0051D447): the original also returns the point it
     accepted in ECX (x) / EDX (y) - the input point, or the first free snapped cell corner - which the
     decompiler dropped. Callers read it from g_ArmyPlacementValidatedWorldX/YQ12. */
  /* corner order as in the original: (0,0), (+0x240,0), (+0x240,+0x240), (0,+0x240) */
  static const int cornerDx[4] = {0,ARMY_PLACEMENT_CORNER_OFFSET_Q12,ARMY_PLACEMENT_CORNER_OFFSET_Q12,0};
  static const int cornerDy[4] = {0,0,ARMY_PLACEMENT_CORNER_OFFSET_Q12,ARMY_PLACEMENT_CORNER_OFFSET_Q12};
  PlacementDispatchResult dispatched;
  int corner;

  g_ArmyPlacementValidatedWorldXQ12 = worldXQ12;
  g_ArmyPlacementValidatedWorldYQ12 = worldYQ12;
  dispatched = ArmyPlacement_DispatchAssetAtFieldPoint
                         (placementMode,0,placementHeading,worldYQ12,worldXQ12,armyAssetId,
                          ownerFactionId,inGameRuntime);
  if (!dispatched.failed) {
    return false;
  }
  for (corner = 0; corner < 4; corner++) {
    Q12 x = (Q12)(((uint32_t)worldXQ12 & 0xffffff00) + cornerDx[corner]);
    Q12 y = (Q12)(((uint32_t)worldYQ12 & 0xffffff00) + cornerDy[corner]);
    dispatched = ArmyPlacement_DispatchAssetAtFieldPoint
                           (placementMode,0,placementHeading,y,x,armyAssetId,ownerFactionId,
                            inGameRuntime);
    if (!dispatched.failed) {
      g_ArmyPlacementValidatedWorldXQ12 = x;
      g_ArmyPlacementValidatedWorldYQ12 = y;
      return false;
    }
  }
  g_ArmyPlacementValidatedWorldXQ12 = worldXQ12;
  g_ArmyPlacementValidatedWorldYQ12 = worldYQ12;
  return true;
}


/* Address: 0x00524EB0.
   Placement test for resource extractors: after the common candidate test
   (ArmyPlacementCollision_TestCandidateAndClearance) the field-grid cell under the point must carry the
   deposit bit the definition asks for (FIELD_CELL_XENITE_SUPPORT << selector at +0xC0: 0 Xenite, 1 Tritium).
   Returns the common test's value with CF (rejected) as the result.
   Reached through g_ArmyRuntimeOrderHandlerMatrix11Columns24Classes.placementAssetClassDispatch[14]
   (0x0051FF38), called by ArmyPlacement_DispatchAssetAtFieldPoint.
*/
PlacementCandidateResult ArmyPlacementCandidate_TestFieldOccupancy
              (ArmyPlacementDispatchArg0 placementMode,
              ArmyPlacementClearancePaddingQ12 placementClearancePaddingQ12,
              ArmyPlacementDispatchArg2 placementHeading,ArmyPlacementDispatchArg3 terrainHeightQ12,
              Q12 worldYQ12,Q12 worldXQ12,ModelDefinitionRecordPrefix *modelDefinition,
              ArmyPlacementDispatchArg7 ownerFactionIndex,WorldRuntimeContext *worldRuntime)

{
  int clearanceEax;
  int cellColumn;
  int eaxOrCellColumn;
  PlacementCandidateResult result;
  uint32_t projectedRowTerm;
  int cellRow;
  PlacementCandidateResult clearanceResult;
  FieldGridAsset *activeFieldGrid;
  
  clearanceResult = ArmyPlacementCollision_TestCandidateAndClearance
                    (placementMode,placementClearancePaddingQ12,placementHeading,terrainHeightQ12,
                     worldYQ12,worldXQ12,(ModelDefinition *)modelDefinition,
                     ownerFactionIndex,worldRuntime);
  clearanceEax = clearanceResult.value;
  eaxOrCellColumn = clearanceEax;
  if (!clearanceResult.rejected) {
    activeFieldGrid = worldRuntime->fieldGrid;
    /* FieldGrid_WorldToGridQ12 inlined (skewed grid: the column is shifted by half the row), rounded to the
       nearest cell */
    projectedRowTerm = FIXED_MUL_SHR(worldYQ12,FIELD_GRID_WORLD_Y_TO_ROW_Q20,Q20_SHIFT + 1);
    cellColumn = (int)((FIXED_MUL_SHR(worldXQ12,FIELD_GRID_WORLD_X_TO_COLUMN_Q20,Q20_SHIFT) - projectedRowTerm) + FIELD_GRID_CELL_Q12 / 2) >> Q12_SHIFT;
    eaxOrCellColumn = cellColumn;
    if ((((-1 < cellColumn) && (cellRow = (int)(projectedRowTerm * 2 + FIELD_GRID_CELL_Q12 / 2) >> Q12_SHIFT, -1 < cellRow)) &&
        (cellColumn < (int)activeFieldGrid->gridWidth)) && (cellRow < (int)activeFieldGrid->gridHeight)) {
      eaxOrCellColumn = clearanceEax;
      /* the resource field selector of an extractor is its class parameter at +0xC0 */
      if ((activeFieldGrid->cells[activeFieldGrid->gridWidth * cellRow + cellColumn].flagsAndMaterial &
          FIELD_CELL_XENITE_SUPPORT << ((uint8_t)((ModelDefinition *)modelDefinition)->classParameterC0 & 31)) != 0) {
        result.value = clearanceEax;
        result.rejected = false;
        return result;
      }
    }
    g_ArmyPlacementLateRejectionCount++;
  }
  result.value = eaxOrCellColumn;
  result.rejected = true;
  return result;
}


/* Address: 0x00524F70.
   Validates a placed resource extractor (the live counterpart of ArmyPlacementCandidate_TestFieldOccupancy):
   the common test ArmyPlacementCollision_TestCurrentRuntime, then the field-grid cell under the model (not on
   the grid border) must carry the deposit bit FIELD_CELL_XENITE_SUPPORT << selector (+0xC0: 0 Xenite,
   1 Tritium). Returns CF: true = rejected.
   Reached through g_ArmyRuntimeOrderHandlerMatrix11Columns24Classes.placementValidation[14] (0x0051FED8),
   called by ArmyRuntimeNode_DispatchTypedCallback.
*/
bool ArmyPlacement_TestGridOccupancyMask
          (WorldRuntimeContext *worldRuntime,ModelRuntimePlacementClass14View *modelRuntime)

{
  int cellColumn;
  int cellRow;
  bool blocked;
  FieldGridCoordinates gridCoordinates;
  FieldGridAsset *activeFieldGrid;
  ModelRuntimeNode *rootNode;
  
  rootNode = modelRuntime->rootModelNode;
  blocked = ArmyPlacementCollision_TestCurrentRuntime
                    (worldRuntime,(ModelRuntimePlacementValidationView *)modelRuntime);
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
          ((activeFieldGrid->cells[cellRow * activeFieldGrid->gridWidth + cellColumn].flagsAndMaterial &
           FIELD_CELL_XENITE_SUPPORT <<
           ((uint8_t)modelRuntime->modelDefinition->resourceFieldSupportSelector & 31)) != 0)))) {
        return false;
      }
    }
    g_ArmyPlacementLateRejectionCount++;
  }
  return true;
}


/* Address: 0x00528110.
   Validates the position of a mobile unit (ground, tracked, walker, glider and water classes): its grid cell
   must pass the definition's cell-mask bands (+0x260/+0x264), no other army may overlap it
   (ArmyCollision_FindBlockingRuntimeForCurrentUnit), and, unless
   UI_COMMAND_RUNTIME_FLAG_INTERACTION_SUBSYSTEM_ACTIVE is set, the field-grid point must not be blocked for
   the owner's faction. Returns CF: true = blocked.
   Reached through g_ArmyRuntimeOrderHandlerMatrix11Columns24Classes.placementValidation[1..3, 17..19]
   (0x0051FED8), called by ArmyRuntimeNode_DispatchTypedCallback.
*/
bool ArmyPlacement_TestGridRuntimeAndFieldBlocking
          (WorldRuntimeContext *worldRuntime,ModelRuntimePlacementValidationView *modelRuntime)

{
  bool blocked;
  ArmyCollisionResult blockingRuntime;
  ModelRuntimeNode *modelNode;
  
  modelNode = modelRuntime->rootModelNode;
  blocked = GridScratch_TestProjectedCellMaskBands
                    ((modelNode->worldTransform).translation.y,
                     (modelNode->worldTransform).translation.x,
                     (uint8_t)modelRuntime->modelDefinition->footprintRadiusClass,
                     (uint8_t)modelRuntime->modelDefinition->terrainTraversalClass);
  if (!blocked) {
    blockingRuntime = ArmyCollision_FindBlockingRuntimeForCurrentUnit
                      ((modelNode->worldTransform).translation.y,
                       (modelNode->worldTransform).translation.x,
                       (RuntimeCollisionQueryView *)modelRuntime,worldRuntime);
    blocked = blockingRuntime.blocked;
    if ((!blocked) &&
       (blocked = false,
       (g_UiCommandRuntimeFlags & UI_COMMAND_RUNTIME_FLAG_INTERACTION_SUBSYSTEM_ACTIVE) == 0)) {
      blocked = FieldGrid_TestWorldPointBlocked
                        (modelRuntime->ownerArmyRuntime->factionIndex,
                         (modelNode->worldTransform).translation.y,
                         (modelNode->worldTransform).translation.x,worldRuntime->fieldGrid);
    }
  }
  return blocked;
}


/* Address: 0x005281A0.
   Placement test for a mobile unit (ground, tracked, walker, glider and water classes): the point must pass the definition's cell-mask bands (+0x260/+0x264), be
   free of other armies (ArmyCollision_TestPointAgainstRuntimeList) and, unless
   UI_COMMAND_RUNTIME_FLAG_INTERACTION_SUBSYSTEM_ACTIVE is set, not be blocked on the field grid for the
   owner's faction. Returns value 0 with CF (rejected) as the result.
   Reached through g_ArmyRuntimeOrderHandlerMatrix11Columns24Classes.placementAssetClassDispatch[1..3, 17..19]
   (0x0051FF38), called by ArmyPlacement_DispatchAssetAtFieldPoint.
*/
PlacementCandidateResult ArmyPlacement_TestMobileUnitPoint
               (uint32_t placementMode,uint32_t placementClearancePaddingQ12,uint32_t placementHeading,
               uint32_t terrainHeightQ12,
               Q12 worldXQ12,Q12 worldYQ12,ModelDefinition *modelDefinition,
               ArmyPlacementDispatchArg7 ownerFactionIndex,WorldRuntimeContext *worldRuntime)

{
  bool blocked;
  
  blocked = GridScratch_TestProjectedCellMaskBands
                    (worldXQ12,worldYQ12,(uint8_t)modelDefinition->footprintRadiusClass,
                     (uint8_t)modelDefinition->terrainTraversalClass);
  if (!blocked) {
    blocked = ArmyCollision_TestPointAgainstRuntimeList
                      (worldXQ12,worldYQ12,(uint8_t *)modelDefinition,worldRuntime);
    if ((!blocked) &&
       (blocked = false,
       (g_UiCommandRuntimeFlags & UI_COMMAND_RUNTIME_FLAG_INTERACTION_SUBSYSTEM_ACTIVE) == 0)) {
      blocked = FieldGrid_TestWorldPointBlocked
                        (ownerFactionIndex,worldXQ12,worldYQ12,worldRuntime->fieldGrid);
    }
  }
  {
    PlacementCandidateResult result;

    result.value = 0;
    result.rejected = blocked;
    return result;
  }
}


/* Address: 0x004BE7F0.
   Contact kind 0 (terrain): sets the model node onto the interpolated terrain height at the point, plus
   heightOffsetQ12 and the model resource's own height offset, stands it upright (angle 1 = quarter turn)
   and sets node flag 0x1. Nothing changes without a field grid or when the point is off the grid.
   Reached through g_ArmyPlacementContactKindDispatchTable.callbacks[0] (0x004BD890), indexed by the model
   definition's contact kind (+0x278) from the movement, creation and session code.
*/
void ArmyPlacementContact_ApplyTerrainHeight
          (Q12 heightOffsetQ12,Q12 worldYQ12,Q12 worldXQ12,ModelRuntimeNode *modelNode,
          WorldRuntimeContext *worldRuntime)

{
  Q12 surfaceHeightQ12;
  HeightSampleResult surfaceHeight;
  
  if (worldRuntime->fieldGrid != NULL) {
    surfaceHeight = FieldGrid_InterpolateTerrainHeight(worldYQ12,worldXQ12,worldRuntime->fieldGrid);
    surfaceHeightQ12 = surfaceHeight.heightQ12;
    if (!surfaceHeight.failed) {
      (modelNode->worldTransform).translation.z =
           surfaceHeightQ12 + heightOffsetQ12 +
           ((modelNode->modelPayload).modelResource)->placementHeightOffsetQ12;
      (modelNode->worldTransform).translation.x = worldXQ12;
      (modelNode->worldTransform).translation.y = worldYQ12;
      (modelNode->modelPayload).worldRotationAngle1 = FIXED_ANGLE16_QUARTER_TURN;
      modelNode->runtimeFlags = modelNode->runtimeFlags | 1;
    }
  }
  return;
}


/* Address: 0x004BE860.
   Contact kind 1 (water surface): sets the model node onto the interpolated water surface at the point
   plus heightOffsetQ12 (no resource offset), stands it upright (angle 1 = quarter turn) and sets node flag
   0x1. Nothing changes without a field grid or when the point is off the grid.
   Reached through g_ArmyPlacementContactKindDispatchTable.callbacks[1] (0x004BD890), indexed by the model
   definition's contact kind (+0x278).
*/
void ArmyPlacementContact_ApplyWaterSurfaceHeight
          (Q12 heightOffsetQ12,Q12 worldYQ12,Q12 worldXQ12,ModelRuntimeNode *modelNode,
          WorldRuntimeContext *worldRuntime)

{
  Q12 surfaceHeightQ12;
  HeightSampleResult surfaceHeight;
  
  if (worldRuntime->fieldGrid != NULL) {
    surfaceHeight = FieldGrid_InterpolateWaterSurfaceHeight(worldYQ12,worldXQ12,worldRuntime->fieldGrid);
    surfaceHeightQ12 = surfaceHeight.heightQ12;
    if (!surfaceHeight.failed) {
      (modelNode->worldTransform).translation.z = surfaceHeightQ12 + heightOffsetQ12;
      (modelNode->worldTransform).translation.x = worldXQ12;
      (modelNode->worldTransform).translation.y = worldYQ12;
      (modelNode->modelPayload).worldRotationAngle1 = FIXED_ANGLE16_QUARTER_TURN;
      modelNode->runtimeFlags = modelNode->runtimeFlags | 1;
    }
  }
  return;
}


/* Address: 0x004BE8C0.
   Contact kind 2 (terrain with slope): like kind 0, but tilts the model node to the terrain normal (the two
   packed 16-bit normal angles become rotation angles 0 and 1) instead of standing it upright.
   Reached through g_ArmyPlacementContactKindDispatchTable.callbacks[2] (0x004BD890), indexed by the model
   definition's contact kind (+0x278).
*/
void ArmyPlacementContact_ApplyTerrainHeightAndNormal
          (Q12 heightOffsetQ12,Q12 worldYQ12,Q12 worldXQ12,ModelRuntimeNode *modelNode,
          WorldRuntimeContext *worldRuntime)

{
  int resourceHeightOffsetQ12;
  HeightNormalSampleResult surfaceHeightNormal;
  
  if (worldRuntime->fieldGrid != NULL) {
    surfaceHeightNormal = FieldGrid_InterpolateTerrainHeightAndNormal(worldYQ12,worldXQ12,worldRuntime->fieldGrid)
    ;
    if (!surfaceHeightNormal.failed) {
      resourceHeightOffsetQ12 = ((modelNode->modelPayload).modelResource)->placementHeightOffsetQ12;
      (modelNode->modelPayload).worldRotationAngle0 = surfaceHeightNormal.packedNormalAngles & FIXED_ANGLE16_MASK;
      (modelNode->modelPayload).worldRotationAngle1 = (int)surfaceHeightNormal.packedNormalAngles >> 16;
      (modelNode->worldTransform).translation.z =
           surfaceHeightNormal.heightQ12 + resourceHeightOffsetQ12 + heightOffsetQ12;
      (modelNode->worldTransform).translation.x = worldXQ12;
      (modelNode->worldTransform).translation.y = worldYQ12;
      modelNode->runtimeFlags = modelNode->runtimeFlags | 1;
    }
  }
  return;
}


/* Address: 0x004BE930.
   Contact kind 4 (top surface): sets the model node onto the top surface - the terrain, or the water above
   it (FieldGrid_InterpolateTopSurfaceHeight) - plus heightOffsetQ12, stands it upright (angle 1 = quarter turn)
   and sets node flag 0x1. Nothing changes without a field grid or when the point is off the grid.
   Reached through g_ArmyPlacementContactKindDispatchTable.callbacks[4] (0x004BD890), indexed by the model
   definition's contact kind (+0x278).
*/
void ArmyPlacementContact_ApplyTopSurfaceHeight
          (Q12 heightOffsetQ12,Q12 worldYQ12,Q12 worldXQ12,ModelRuntimeNode *modelNode,
          WorldRuntimeContext *worldRuntime)

{
  Q12 surfaceHeightQ12;
  HeightSampleResult surfaceHeight;
  
  if (worldRuntime->fieldGrid != NULL) {
    surfaceHeight = FieldGrid_InterpolateTopSurfaceHeight(worldYQ12,worldXQ12,worldRuntime->fieldGrid);
    surfaceHeightQ12 = surfaceHeight.heightQ12;
    if (!surfaceHeight.failed) {
      (modelNode->worldTransform).translation.z = surfaceHeightQ12 + heightOffsetQ12;
      (modelNode->worldTransform).translation.x = worldXQ12;
      (modelNode->worldTransform).translation.y = worldYQ12;
      (modelNode->modelPayload).worldRotationAngle1 = FIXED_ANGLE16_QUARTER_TURN;
      modelNode->runtimeFlags = modelNode->runtimeFlags | 1;
    }
  }
  return;
}


/* Address: 0x004BE990.
   Contact kind 3 (articulated walker): moves the model node to the point, sets node flag 0x1 and lets the
   articulated code seat its legs/suspension on the terrain (height and tilt come from there, so
   heightOffsetQ12 is unused).
   Reached through g_ArmyPlacementContactKindDispatchTable.callbacks[3] (0x004BD890), indexed by the model
   definition's contact kind (+0x278).
*/
void ArmyPlacementContact_InitializeArticulatedSuspension
          (Q12 heightOffsetQ12,Q12 worldYQ12,Q12 worldXQ12,ModelRuntimeNode *modelNode,
          WorldRuntimeContext *worldRuntime)

{
  (modelNode->worldTransform).translation.x = worldXQ12;
  (modelNode->worldTransform).translation.y = worldYQ12;
  modelNode->runtimeFlags = modelNode->runtimeFlags | 1;
  ArmyArticulatedRuntime_InitializeTerrainContactGeometry(modelNode,worldRuntime);
  ArmyArticulatedRuntime_UpdateSuspensionHierarchy(modelNode,worldRuntime);
  return;
}


/* Address: 0x00525320.
   Release handler of a resource extractor (class 14): gives back the storage it added to its faction (see
   ArmyPlacement_ReleaseFactionCapacity) and clears the extractor markers (army at +0x6C, extraction
   descriptor at +0x7C) of the field-grid cell it stood on, so the deposit can be built on again.
   Reached through g_ArmyRuntimeOrderHandlerMatrix11Columns24Classes.modelReleaseOrCommit[14] (0x0051FE78),
   called by ModelRuntimePool_DestroyHierarchyAndDetach.
*/
void ArmyPlacement_ReleaseFactionCapacityAndClearGridReservation
          (ModelDefinitionRecordPrefix *modelDefinition,ModelRuntimeSlot *modelRuntime)

{
  uint8_t *storageLimit;
  uint32_t storageContribution;
  InGameRuntimeRoot *inGameRoot;
  int factionOffsetOrLimitOrCellColumn;
  int limitOffsetOrCellRow;
  FieldGridCoordinates gridCoordinates;
  FieldGridAsset *activeFieldGrid;

  /* classParameterC0: the resource selector (0 Xenite, 1 Tritium), classParameterC4: the storage the model adds */
  storageContribution = ((ModelDefinition *)modelDefinition)->classParameterC4;
  factionOffsetOrLimitOrCellColumn =
       ((modelRuntime->ownerArmyRuntimeOrSavedOffset).armyRuntime)->factionIndex *
       GAME_FACTION_RUNTIME_RECORD_BYTES;
  limitOffsetOrCellRow = factionOffsetOrLimitOrCellColumn + 4; /* xeniteStorageLimitQ4 */
  if (((ModelDefinition *)modelDefinition)->classParameterC0 != 0) {
    limitOffsetOrCellRow = factionOffsetOrLimitOrCellColumn + 20; /* tritiumStorageLimitQ4 */
  }
  factionOffsetOrLimitOrCellColumn = *(int *)((uint8_t *)g_GameFactionRuntimeImage.records + limitOffsetOrCellRow);
  /* the stock (the dword before the limit) loses the share this storage held */
  if ((((int)modelRuntime->health < 2) && (factionOffsetOrLimitOrCellColumn != 0)) &&
     (((modelRuntime->classState).stateFlags & ARMY_MODEL_STATE_DESTRUCTION_STARTED) == 0)) {
    *(int *)(limitOffsetOrCellRow + THANDOR_ADDR(g_GameFactionRuntimeImage,-4)) =
         *(int *)(limitOffsetOrCellRow + THANDOR_ADDR(g_GameFactionRuntimeImage,-4)) -
         (int)(((int64_t)(int)storageContribution * (int64_t)*(int *)(limitOffsetOrCellRow + THANDOR_ADDR(g_GameFactionRuntimeImage,-4))) / (int64_t)factionOffsetOrLimitOrCellColumn);
  }
  storageLimit = (uint8_t *)g_GameFactionRuntimeImage.records + limitOffsetOrCellRow;
  *(uint32_t *)storageLimit = *(int *)storageLimit - storageContribution;
  inGameRoot = g_InGameRuntimeRoot;
  gridCoordinates = FieldGrid_WorldToGridQ12
                    ((((modelRuntime->rootModelNodeOrSavedOffset).modelNode)->worldTransform).
                     translation.y,
                     (((modelRuntime->rootModelNodeOrSavedOffset).modelNode)->worldTransform).
                     translation.x);
  factionOffsetOrLimitOrCellColumn = ((gridCoordinates.columnQ12 >> (Q12_SHIFT - 1)) + 1) >> 1;
  limitOffsetOrCellRow = ((gridCoordinates.rowQ12 >> (Q12_SHIFT - 1)) + 1) >> 1;
  activeFieldGrid = (inGameRoot->worldRuntime).fieldGrid;
  if (((0 < factionOffsetOrLimitOrCellColumn) && (0 < limitOffsetOrCellRow)) && (activeFieldGrid != NULL)) {
    if ((factionOffsetOrLimitOrCellColumn + 1 < (int)activeFieldGrid->gridWidth) &&
        (limitOffsetOrCellRow + 1 < (int)activeFieldGrid->gridHeight)) {
      factionOffsetOrLimitOrCellColumn =
           limitOffsetOrCellRow * activeFieldGrid->gridWidth + factionOffsetOrLimitOrCellColumn;
      activeFieldGrid->cells[factionOffsetOrLimitOrCellColumn].armyRuntimeSavedOffset = 0;
      activeFieldGrid->cells[factionOffsetOrLimitOrCellColumn].resourceExtractionDescriptor = 0;
    }
  }
  return;
}


/* Address: 0x00525420.
   Release handler of a resource storage (class 15): the model's storage (+0xC4 of the definition) is taken
   off its faction's Xenite or Tritium storage limit (selector +0xC0), and - unless the model's state
   (+0x3C) is 2 or more, the limit is zero or class-state bit 0x20 is set - the faction's stock of that
   resource loses the proportional share (stock * storage / limit) that was kept in it.
   Reached through g_ArmyRuntimeOrderHandlerMatrix11Columns24Classes.modelReleaseOrCommit[15] (0x0051FE78),
   called by ModelRuntimePool_DestroyHierarchyAndDetach.
*/
void ArmyPlacement_ReleaseFactionCapacity(ModelDefinitionRecordPrefix *modelDefinition,ModelRuntimeSlot *modelRuntime)

{
  uint8_t *storageLimit;
  uint32_t storageContribution;
  int factionOffsetOrStorageLimit;
  int storageLimitOffset;

  /* classParameterC0: the resource selector (0 Xenite, 1 Tritium), classParameterC4: the storage the model adds */
  storageContribution = ((ModelDefinition *)modelDefinition)->classParameterC4;
  factionOffsetOrStorageLimit =
       ((modelRuntime->ownerArmyRuntimeOrSavedOffset).armyRuntime)->factionIndex *
       GAME_FACTION_RUNTIME_RECORD_BYTES;
  storageLimitOffset = factionOffsetOrStorageLimit + 4; /* xeniteStorageLimitQ4 */
  if (((ModelDefinition *)modelDefinition)->classParameterC0 != 0) {
    storageLimitOffset = factionOffsetOrStorageLimit + 20; /* tritiumStorageLimitQ4 */
  }
  factionOffsetOrStorageLimit = *(int *)((uint8_t *)g_GameFactionRuntimeImage.records + storageLimitOffset);
  /* the stock (the dword before the limit) loses the share this storage held */
  if ((((int)modelRuntime->health < 2) && (factionOffsetOrStorageLimit != 0)) &&
     (((modelRuntime->classState).stateFlags & ARMY_MODEL_STATE_DESTRUCTION_STARTED) == 0)) {
    *(int *)(storageLimitOffset + THANDOR_ADDR(g_GameFactionRuntimeImage,-4)) =
         *(int *)(storageLimitOffset + THANDOR_ADDR(g_GameFactionRuntimeImage,-4)) -
         (int)(((int64_t)(int)storageContribution * (int64_t)*(int *)(storageLimitOffset + THANDOR_ADDR(g_GameFactionRuntimeImage,-4))) / (int64_t)factionOffsetOrStorageLimit);
  }
  storageLimit = (uint8_t *)g_GameFactionRuntimeImage.records + storageLimitOffset;
  *(uint32_t *)storageLimit = *(int *)storageLimit - storageContribution;
  return;
}


/* Address: 0x005263E0.
   Release handler of class 21 (aircraft): the model linked at +0x60 (presumably its home base) keeps 13 slots of army
   asset ids (+0x78..) with a reservation bit each (+0xB4). The first slot holding this army's asset id with
   its bit set gets the bit cleared and the counter at +0x70 incremented; unless class-state bit 0x20 of the
   released model is set, the slot is also emptied and the counters at +0x6C and +0x70 are decremented.
   Reached through g_ArmyRuntimeOrderHandlerMatrix11Columns24Classes.modelReleaseOrCommit[21] (0x0051FE78),
   called by ModelRuntimePool_DestroyHierarchyAndDetach.
*/
void ArmyPlacement_ReleaseClassStateReservation
          (ModelDefinitionRecordPrefix *modelDefinition,ModelRuntimeSlot *modelRuntime)

{
  uint32_t *classCounter;
  int32_t *reservationBits;
  ModelRuntimeArmyLinkOrState *armyLinkState;
  int remainingCount;
  ModelRuntimeSlot *slotCursor;
  uint32_t reservationBit;
  ModelRuntimeSlot *linkedModelSlot;
  
  linkedModelSlot = (modelRuntime->classLinkState).modelLinkOrState.modelRuntime;
  if (linkedModelSlot != NULL) {
    remainingCount = 13;
    reservationBit = 1;
    slotCursor = linkedModelSlot;
    do {
      if ((((modelRuntime->ownerArmyRuntimeOrSavedOffset).armyRuntime)->armyAssetId ==
           (slotCursor->classLinkState).classState78) &&
         (((linkedModelSlot->classState).classStateB4 & reservationBit) != 0)) {
        classCounter = &(linkedModelSlot->classLinkState).classState70;
        *classCounter = *classCounter + 1;
        reservationBits = &(linkedModelSlot->classState).classStateB4;
        *reservationBits = *reservationBits & (reservationBit ^ 0xffffffff);
        if (((modelRuntime->classState).stateFlags & ARMY_MODEL_STATE_DESTRUCTION_STARTED) != 0) {
          return;
        }
        (slotCursor->classLinkState).classState78 = 0;
        armyLinkState = &(linkedModelSlot->classLinkState).armyLinkOrState;
        armyLinkState->armyRuntime = (ArmyRuntimeSlot *)(armyLinkState->classState - 1);
        classCounter = &(linkedModelSlot->classLinkState).classState70;
        *classCounter = *classCounter - 1;
        return;
      }
      /* next asset-id slot: 4 bytes on (rootModelNodeOrSavedOffset is the second dword) */
      slotCursor = (ModelRuntimeSlot *)&slotCursor->rootModelNodeOrSavedOffset;
      reservationBit = reservationBit * 2;
      remainingCount--;
    } while (remainingCount != 0);
  }
  return;
}


/* Address: 0x00527BD0.
   Placement test of the classes that can be placed anywhere: accepts at once (CF clear, value 0; the
   original preserves EAX and pops the nine arguments).
   Reached through g_ArmyRuntimeOrderHandlerMatrix11Columns24Classes.placementAssetClassDispatch[0, 5..9,
   12, 21] (0x0051FF38), called by ArmyPlacement_DispatchAssetAtFieldPoint.
*/
PlacementCandidateResult ArmyPlacementAssetClassDispatch_AlwaysSuccess
               (uint32_t placementMode,uint32_t placementClearancePaddingQ12,uint32_t placementHeading,
               uint32_t terrainHeightQ12,
               Q12 worldXQ12,Q12 worldYQ12,ModelDefinitionRecordPrefix *modelDefinition,
               uint32_t ownerFactionIndex,WorldRuntimeContext *worldRuntime)

{
  PlacementCandidateResult result;

  result.value = 0;
  result.rejected = false;
  return result;
}

/* Address: 0x00529CB0.
   Tests whether a model of this definition, with its placement radius (+0xDC), would overlap any army in
   the world's owner list: a cheap depth-bin mask overlap first, then the exact circle test
   ArmyCollision_TestPointWithinExpandedRuntimeRadius. Returns CF: true = an army is in the way (a
   definition without radius never collides).
   Called directly by ArmyPlacement_TestMobileUnitPoint.
*/

bool ArmyCollision_TestPointAgainstRuntimeList
          (Q12 worldXQ12,Q12 worldYQ12,uint8_t *modelDefinition,WorldRuntimeContext *worldRuntime)

{
  int placementRadiusQ12;
  DepthBinMask32 firstMaskHigh;
  DepthBinMask32 firstMaskLow;
  WorldOwnerListNode *ownerNode;
  bool hit;

  placementRadiusQ12 = ((ModelDefinition *)modelDefinition)->footprintRadius;
  ownerNode = worldRuntime->ownerListHead;
  if ((placementRadiusQ12 != 0) && (ownerNode != NULL)) {
    firstMaskHigh = DepthInterval_BuildBinMask(placementRadiusQ12,worldYQ12);
    firstMaskLow = DepthInterval_BuildBinMask(placementRadiusQ12,worldXQ12);
    do {
      if (ownerNode->ownerClassId == WORLD_OWNER_RUNTIME_MODEL) {
        hit = DepthBinMasks_Overlap
                          (firstMaskLow,firstMaskHigh,ownerNode->modelDepthBinMaskFar,
                           ownerNode->modelDepthBinMaskNear);
        if (hit) {
          hit = ArmyCollision_TestPointWithinExpandedRuntimeRadius
                            (placementRadiusQ12,worldXQ12,worldYQ12,ownerNode->runtimePayload);
          if (hit) {
            return true;
          }
        }
      }
      ownerNode = ownerNode->nextNode;
    } while (ownerNode != NULL);
  }
  return false;
}


/* Address: 0x00529E60.
   Finds the first model that a moving unit would run into at the given point: every model in the world's
   owner list whose depth bins overlap the unit's own, except the unit itself, the model it is linked to
   (+0xF0) and models linked to it, and whose collision circle reaches the point within the unit's placement
   radius (+0xDC of its definition). Returns that model runtime in EAX with CF set, or 0 with CF clear.
   Called directly by the ground-movement code (gameplay/army/movement.c) and by
   ArmyPlacement_TestGridRuntimeAndFieldBlocking.
*/
ArmyCollisionResult ArmyCollision_FindBlockingRuntimeForCurrentUnit
          (Q12 worldXQ12,Q12 worldYQ12,RuntimeCollisionQueryView *currentRuntime,
          WorldRuntimeContext *worldRuntime)

{
  ModelRuntimeNode *currentModelNode;
  uint32_t clearanceRadiusQ12;
  ModelRuntimeSlot *candidateModelRuntime;
  bool hit;
  ArmyCollisionResult notFound;
  ArmyCollisionResult found;
  ModelRuntimeNode *candidateModelNode;
  
  currentModelNode = currentRuntime->modelNodeRuntime;
  clearanceRadiusQ12 = currentRuntime->modelDefinition->footprintRadius;
  candidateModelNode = (ModelRuntimeNode *)worldRuntime->ownerListHead;
  if (clearanceRadiusQ12 != 0) {
    for (; candidateModelNode != NULL;
        candidateModelNode = (ModelRuntimeNode *)(candidateModelNode->common).nextNode) {
      if (((((candidateModelNode->ownerClassId == WORLD_OWNER_RUNTIME_MODEL) &&
            (hit = DepthBinMasks_Overlap
                               (currentRuntime->modelNodeRuntime->depthBinMaskFar,
                                currentRuntime->modelNodeRuntime->depthBinMaskNear,
                                candidateModelNode->depthBinMaskFar,
                                candidateModelNode->depthBinMaskNear), hit)) &&
           (candidateModelRuntime = (candidateModelNode->runtimePayload).modelRuntime,
           currentModelNode != candidateModelNode)) &&
          /* the null test comes after currentRuntime was already dereferenced, so it never fires */
          ((currentRuntime == NULL ||
           ((candidateModelRuntime != currentRuntime->linkedRuntime &&
            ((ModelRuntimeSlot *)currentRuntime !=
             (candidateModelRuntime->classState).linkedArmyRuntimeOrSavedOffset.modelRuntime)))))) &&
         (hit = ArmyCollision_TestPointWithinExpandedRuntimeRadius
                            (clearanceRadiusQ12,worldXQ12,worldYQ12,candidateModelRuntime), hit)) {
        found.blocked = true;
        found.blockingModelRuntime = (uint32_t)candidateModelRuntime;
        return found;
      }
    }
  }
  notFound.blockingModelRuntime = 0;
  notFound.blocked = false;
  return notFound;
}


/* Address: 0x0051D450.
   Looks up the army asset and its model definition, samples the terrain height at the point with the
   definition's interpolation callback (index at +0x278), and hands the placement test to the handler of the
   definition's class (+0x4C) in the placement dispatch table. Returns that handler's result, or the lookup
   error with CF set.
*/
PlacementDispatchResult ArmyPlacement_DispatchAssetAtFieldPoint(ArmyPlacementMode placementMode,
          ArmyPlacementClearancePaddingQ12 placementClearancePaddingQ12,
          uint32_t placementHeading,Q12 worldYQ12,Q12 worldXQ12,
          PckArmyAssetIdCatalog armyAssetId,FactionRuntimeIndex ownerFactionIndex,
          UiRootNode *inGameRoot)

{
  uint32_t assetClassIndex;
  ArmyAssetRecordPrefix *modelDefinition;
  ArmyAssetLookupResult lookupResult;
  HeightSampleResult terrainHeight;
  PlacementDispatchResult dispatchResult;
  
  lookupResult = ArmyAssetRegistry_FindById(armyAssetId);
  if (!lookupResult.notFound) {
    /* the model definition of the army asset's root node */
    lookupResult = THANDOR_BITCAST(ModelDefinitionResult, ArmyAssetLookupResult, ModelDefinitionRegistry_FindByIdWithError
                      (((AiLinkedDefinitionListView *)(lookupResult.recordOrError)->rootNodeOffsetOrPointer)->
                       definitionIds[0]));
    modelDefinition = lookupResult.recordOrError;
    if (!lookupResult.notFound) {
      /* modelDefinition is the model definition (typed as the army record by the shared lookup result) */
      assetClassIndex = ((ModelDefinition *)modelDefinition)->runtimeClassId;
      /* placementContactKindIndex selects the height interpolation mode; the field grid is at
         +0x54 of the in-game runtime */
      terrainHeight = (*g_FieldGridInterpolationCallbacks5.callbacks[((ModelDefinition *)modelDefinition)->placementContactKindIndex])
                        (worldYQ12,worldXQ12,(FieldGridAsset *)inGameRoot->previousRoot);
      lookupResult = THANDOR_BITCAST(PlacementDispatchResult, ArmyAssetLookupResult, (*g_ArmyRuntimeOrderHandlerMatrix11Columns24Classes.placementAssetClassDispatch[assetClassIndex]
              )(placementMode,placementClearancePaddingQ12,placementHeading,terrainHeight.heightQ12,
                worldYQ12,worldXQ12,(ModelDefinitionRecordPrefix *)modelDefinition,ownerFactionIndex,
                (WorldRuntimeContext *)inGameRoot));
    }
  }
  dispatchResult.value = (uint32_t)lookupResult.recordOrError;
  dispatchResult.failed = lookupResult.notFound;
  return dispatchResult;
}


/* Address: 0x00529D70.
   Tests whether a circle of queryRadiusQ12 at a candidate point hits any army in the world's owner list:
   depth-bin overlap first, then the exact circle test; armies of runtime class 0 and 12 never block, class-13
   armies also block when the point comes near their (1,5) anchor point. With
   ARMY_PLACEMENT_MODE_STRUCTURES_ONLY in the mode only armies of depth-bin class 0x90 count. Returns CF:
   true = hit.
   Called directly by ArmyPlacementCollision_TestCandidateAndClearance and
   ArmyPlacementCandidate_TestOffsetClearance.
*/

bool ArmyPlacementCollision_TestPointAgainstRuntimeList
          (ArmyPlacementCollisionFilterFlags placementFilterFlags,Q12 queryRadiusQ12,Q12 worldXQ12,
          Q12 worldYQ12,WorldRuntimeContext *worldRuntime)

{
  int modelClassId;
  DepthBinMask32 firstMaskHigh;
  DepthBinMask32 firstMaskLow;
  WorldOwnerListNode *ownerNode;
  bool hit;

  ownerNode = worldRuntime->ownerListHead;
  if ((queryRadiusQ12 != 0) && (ownerNode != NULL)) {
    firstMaskHigh = DepthInterval_BuildBinMask(queryRadiusQ12,worldYQ12);
    firstMaskLow = DepthInterval_BuildBinMask(queryRadiusQ12,worldXQ12);
    do {
      if (ownerNode->ownerClassId == WORLD_OWNER_RUNTIME_MODEL) {
        modelClassId = ((ModelRuntimeSlot *)ownerNode->runtimePayload)->definitionOrSavedId.runtimeDefinition->runtimeClassId;
        hit = DepthBinMasks_Overlap
                          (firstMaskLow,firstMaskHigh,ownerNode->modelDepthBinMaskFar,
                           ownerNode->modelDepthBinMaskNear);
        if (((hit) &&
            ((((placementFilterFlags & ARMY_PLACEMENT_MODE_STRUCTURES_ONLY) == 0 ||
              (*(int *)(&g_ArmyRuntimeDepthBinClassByModelClass + modelClassId * 4) == ARMY_DEPTH_BIN_CLASS_STRUCTURE)) &&
             (modelClassId != MODEL_RUNTIME_CLASS_00)))) && (modelClassId != MODEL_RUNTIME_CLASS_12)) {
          hit = ArmyCollision_TestPointWithinExpandedRuntimeRadius
                            (queryRadiusQ12,worldXQ12,worldYQ12,ownerNode->runtimePayload);
          if (hit) {
            return true;
          }
          if ((modelClassId == MODEL_RUNTIME_CLASS_13) &&
             (hit = ArmyPlacementCandidate_TestModelAnchorDistance
                                (queryRadiusQ12,worldXQ12,worldYQ12,ownerNode->runtimePayload),
             hit)) {
            return true;
          }
        }
      }
      ownerNode = ownerNode->nextNode;
    } while (ownerNode != NULL);
  }
  return false;
}


/* Address: 0x00529F30.
   Tests whether a placed model collides with another army at a point. candidateRuntimeOrRadiusQ12 is either
   the model runtime itself (a value at or above the image base 0x400000: its placement radius +0xDC is used,
   the depth bins of its node must overlap, and it, its linked model (+0xF0) and models linked to it are
   skipped) or a bare radius below 0x400000 (used as radius + 1, no depth-bin pre-test). excludedWorldObject
   is skipped as well; armies of class 0 and 12 never block, class-13 armies also block near their (1,5)
   anchor. Returns CF: true = collision.
   Called directly by ArmyPlacementCollision_TestCurrentRuntime and
   ArmyPlacement_TestModelTerrainAndRuntimeClearance.
*/

bool ArmyPlacementCollision_TestCandidateAgainstRuntimeList
          (WorldOwnerListNode *excludedWorldObject,Q12 worldXQ12,Q12 worldYQ12,
          IMAGE_DOS_HEADER *candidateRuntimeOrRadiusQ12,WorldRuntimeContext *worldRuntime)

{
  ModelRuntimeSlot *ownerModelRuntime;
  uint32_t modelClassId;
  WorldOwnerListNode *candidateNode;
  char *queryRadiusQ12;
  bool hit;
  WorldOwnerListNode *ownerNode;
  
  /* the IMAGE_DOS_HEADER type only serves the compare against the image base: below it the "pointer" is
     a radius (e_magic + 1 is that value plus one), above it a model runtime whose +4 (e_cp) is the root
     node and whose first dword is the definition */
  if (candidateRuntimeOrRadiusQ12 < (IMAGE_DOS_HEADER *)0x400000) {
    queryRadiusQ12 = candidateRuntimeOrRadiusQ12->e_magic + 1;
    candidateNode = NULL;
  }
  else {
    candidateNode = *(WorldOwnerListNode **)&candidateRuntimeOrRadiusQ12->e_cp;
    queryRadiusQ12 = (char *)((ModelRuntimeSlot *)candidateRuntimeOrRadiusQ12)->definitionOrSavedId.runtimeDefinition->footprintRadius;
  }
  ownerNode = worldRuntime->ownerListHead;
  if (queryRadiusQ12 != NULL) {
    for (; ownerNode != NULL; ownerNode = ownerNode->nextNode) {
      if (((((ownerNode->ownerClassId == WORLD_OWNER_RUNTIME_MODEL) &&
            ((((candidateRuntimeOrRadiusQ12 < &IMAGE_DOS_HEADER_00400000 ||
               (hit = DepthBinMasks_Overlap
                                  (((ModelRuntimeSlot *)candidateRuntimeOrRadiusQ12)->rootModelNodeOrSavedOffset.modelNode->depthBinMaskFar,
                                   ((ModelRuntimeSlot *)candidateRuntimeOrRadiusQ12)->rootModelNodeOrSavedOffset.modelNode->depthBinMaskNear,
                                   ownerNode->modelDepthBinMaskFar,
                                   ownerNode->modelDepthBinMaskNear), hit)) &&
              (ownerModelRuntime = ownerNode->runtimePayload, candidateNode != ownerNode)) &&
             (ownerNode != excludedWorldObject)))) &&
           ((candidateRuntimeOrRadiusQ12 < &IMAGE_DOS_HEADER_00400000 ||
            ((ownerModelRuntime !=
              ((ModelRuntimeSlot *)candidateRuntimeOrRadiusQ12)->classState.linkedArmyRuntimeOrSavedOffset.modelRuntime
             && ((ModelRuntimeSlot *)candidateRuntimeOrRadiusQ12 !=
                 ownerModelRuntime->classState.linkedArmyRuntimeOrSavedOffset.modelRuntime)))
            ))) && ((modelClassId = ownerModelRuntime->definitionOrSavedId.runtimeDefinition->runtimeClassId,
                     modelClassId != MODEL_RUNTIME_CLASS_00 &&
                             (modelClassId != MODEL_RUNTIME_CLASS_12)))) &&
         ((hit = ArmyCollision_TestPointWithinExpandedRuntimeRadius
                             ((Q12)queryRadiusQ12,worldXQ12,worldYQ12,ownerModelRuntime), hit ||
          ((modelClassId == MODEL_RUNTIME_CLASS_13 &&
           (hit = ArmyPlacementCandidate_TestModelAnchorDistance
                              ((Q12)queryRadiusQ12,worldXQ12,worldYQ12,ownerModelRuntime), hit)))))) {
        return true;
      }
    }
  }
  return false;
}


/* Address: 0x00527740.
   Validates a placed building (the live counterpart of ArmyPlacementCollision_TestCandidateAndClearance): no
   other army may overlap it, the terrain around it must suit its contact kind, and - unless
   UI_COMMAND_RUNTIME_FLAG_INTERACTION_SUBSYSTEM_ACTIVE is set - its field-grid point must not be blocked for
   its faction and it must stand within the support radius (+0x19C) plus its own margin (+0x1A8) of another
   model of the same faction. Returns CF: true = rejected.
   Reached through g_ArmyRuntimeOrderHandlerMatrix11Columns24Classes.placementValidation[4, 10, 11, 15, 16,
   20, 22, 23] (0x0051FED8), called by ArmyRuntimeNode_DispatchTypedCallback; also called directly by
   ArmyPlacement_TestModelTerrainAndRuntimeClearance and ArmyPlacement_TestGridOccupancyMask.
*/

bool ArmyPlacementCollision_TestCurrentRuntime
          (WorldRuntimeContext *worldRuntime,ModelRuntimePlacementValidationView *modelRuntime)

{
  ModelDefinition *placementDefinition;
  uint32_t ownClearanceQ12;
  ArmyRuntimeSlot *ownerArmy;
  uint32_t neighborSupportRadiusQ12;
  int64_t deltaYSquared;
  int64_t remainingSquared;
  int heightCopyOrDeltaX;
  int heightRadiusOrDeltaY;
  ModelRuntimeNode *ownerNode;
  bool blocked;
  bool (*terrainTest)(FieldGridRadiusUnits,Q12,Q12,Q12,FieldGridAsset *);
  ModelRuntimeNode *rootNode;
  
  rootNode = modelRuntime->rootModelNode;
  placementDefinition = modelRuntime->modelDefinition;
  ownClearanceQ12 = placementDefinition->placementFlags;
  /* reference height for the terrain test: the node's height without the definition's and the resource's
     height offsets */
  heightRadiusOrDeltaY = ((rootNode->worldTransform).translation.z - placementDefinition->placementHeightOffsetQ12) -
          ((rootNode->modelPayload).modelResource)->placementHeightOffsetQ12;
  terrainTest = TerrainHeightBand_TestAroundWorldPoint;
  if (placementDefinition->placementContactKindIndex == ARMY_PLACEMENT_CONTACT_KIND_WATER_SURFACE) {
    terrainTest = TerrainAuxHeightThreshold_TestAroundWorldPoint;
  }
  heightCopyOrDeltaX = heightRadiusOrDeltaY;
  blocked = ArmyPlacementCollision_TestCandidateAgainstRuntimeList
                    ((WorldOwnerListNode *)rootNode,(rootNode->worldTransform).translation.y,
                     (rootNode->worldTransform).translation.x,(IMAGE_DOS_HEADER *)modelRuntime,
                     worldRuntime);
  /* The original rejects on the terrain test's CF (JC after the indirect call); the decompile
     dropped that result and re-tested the runtime-list flag. */
  if ((!blocked) &&
     (blocked = terrainTest(placementDefinition->footprintRadius,heightRadiusOrDeltaY,
                 (rootNode->worldTransform).translation.y,
                 (rootNode->worldTransform).translation.x,worldRuntime->fieldGrid),
     !blocked)) {
    rootNode = modelRuntime->rootModelNode;
    ownerArmy = modelRuntime->ownerArmyRuntime;
    if ((g_UiCommandRuntimeFlags & UI_COMMAND_RUNTIME_FLAG_INTERACTION_SUBSYSTEM_ACTIVE) != 0) {
      return false;
    }
    blocked = FieldGrid_TestWorldPointBlocked
                      (ownerArmy->factionIndex,(rootNode->worldTransform).translation.y,
                       (rootNode->worldTransform).translation.x,worldRuntime->fieldGrid);
    if (!blocked) {
      ownerNode = (ModelRuntimeNode *)worldRuntime->ownerListHead;
      if (ownerNode == NULL) {
        return false;
      }
      /* support check: some same-faction model with a support radius (supportRadius of its definition)
         must lie within that radius plus our own margin */
      do {
        if ((((ownerNode->ownerClassId == WORLD_OWNER_RUNTIME_MODEL) && (ownerNode != rootNode))
            && (neighborSupportRadiusQ12 =
                     ((ownerNode->runtimePayload).modelRuntime)->definitionOrSavedId.runtimeDefinition->
                     supportRadius,
               ownerArmy->factionIndex ==
               ((ownerNode->runtimePayload).modelRuntime)->ownerArmyRuntimeOrSavedOffset.armyRuntime->factionIndex)) &&
           (neighborSupportRadiusQ12 != 0)) {
          heightRadiusOrDeltaY = neighborSupportRadiusQ12 + ownClearanceQ12;
          heightCopyOrDeltaX = (rootNode->worldTransform).translation.x -
                  (ownerNode->worldTransform).translation.x;
          remainingSquared = (int64_t)heightRadiusOrDeltaY * (int64_t)heightRadiusOrDeltaY - (int64_t)heightCopyOrDeltaX * (int64_t)heightCopyOrDeltaX;
          if ((-1 < remainingSquared) &&
             (heightRadiusOrDeltaY = (rootNode->worldTransform).translation.y -
                      (ownerNode->worldTransform).translation.y,
             deltaYSquared = (int64_t)heightRadiusOrDeltaY * (int64_t)heightRadiusOrDeltaY,
             -1 < (int)(((int)((uint64_t)remainingSquared >> 32) - (int)((uint64_t)deltaYSquared >> 32)) -
                       (uint32_t)((uint32_t)remainingSquared < (uint32_t)deltaYSquared)))) {
            return false;
          }
        }
        ownerNode = (ModelRuntimeNode *)(ownerNode->common).nextNode;
      } while (ownerNode != NULL);
    }
  }
  return true;
}


/* Address: 0x005278D0.
   Common placement test for buildings: the candidate point must be free of other armies (placement radius
   +0xDC), pass the terrain test of the definition's contact kind against terrainHeightQ12, and - unless
   UI_COMMAND_RUNTIME_FLAG_INTERACTION_SUBSYSTEM_ACTIVE is set - not be blocked on the field grid for the
   owner's faction and lie within reach of a same-faction model with a support radius (+0x19C of its record,
   plus placementClearancePaddingQ12 and our own margin +0x1A8; class-18 models only count when
   ARMY_PLACEMENT_MODE_SKIP_CLASS18_SUPPORT is clear and their army flags 0x18 are clear). Without
   ARMY_PLACEMENT_MODE_MEASURE_SUPPORT_DISTANCE the first such model accepts; with it the value returned in EAX
   is the free distance to the nearest one. CF (rejected) is the result.
   Reached through g_ArmyRuntimeOrderHandlerMatrix11Columns24Classes.placementAssetClassDispatch[4, 10, 11,
   15, 16, 20, 22, 23] (0x0051FF38), called by ArmyPlacement_DispatchAssetAtFieldPoint; also called directly
   by ArmyPlacementCandidate_TestOffsetClearance and ArmyPlacementCandidate_TestFieldOccupancy.
*/

PlacementCandidateResult
ArmyPlacementCollision_TestCandidateAndClearance
          (ArmyPlacementDispatchArg0 placementMode,
          ArmyPlacementClearancePaddingQ12 placementClearancePaddingQ12,uint32_t placementHeading,
          ArmyPlacementDispatchArg3 terrainHeightQ12,Q12 worldXQ12,Q12 worldYQ12,
          ModelDefinition *modelDefinition,
          ArmyPlacementDispatchArg7 ownerFactionIndex,WorldRuntimeContext *worldRuntime)

{
  ArmyPlacementContactKindIndex32 contactKindIndex;
  int64_t radiusSquaredOrDelta;
  int64_t remainingSquared;
  int64_t deltaYSquared;
  int64_t nearestDistanceSquared;
  int recordOrDistanceTerm;
  uint32_t nearestDistanceQ12;
  WorldOwnerListNode *ownerNode;
  bool blocked;
  TerrainPlacementResult terrainTest;
  PlacementCandidateResult accepted;
  PlacementCandidateResult rejected;
  int nearestClearanceQ12;
  UInt64Half32 nearestDistanceLow;
  UInt64Half32 nearestDistanceHigh;
  /* Was a leading pseudo-parameter for the incoming EAX, which shifted every argument of the
     nine-argument table dispatch. */
  int eaxContinuity = 0;
  
  nearestDistanceSquared = INT64_MAX;
  contactKindIndex = modelDefinition->placementContactKindIndex;
  blocked = ArmyPlacementCollision_TestPointAgainstRuntimeList
                    (placementMode,modelDefinition->footprintRadius,worldXQ12,worldYQ12,
                     worldRuntime);
  if (!blocked) {
    terrainTest.value = 0;
    terrainTest.rejected = g_TerrainClassPlacementAndOverlayCallbacks10.placementTests[contactKindIndex]
                      (modelDefinition->footprintRadius,terrainHeightQ12,worldXQ12,
                       worldYQ12,worldRuntime->fieldGrid);
    eaxContinuity = terrainTest.value;
    if (!terrainTest.rejected) {
      if ((g_UiCommandRuntimeFlags & UI_COMMAND_RUNTIME_FLAG_INTERACTION_SUBSYSTEM_ACTIVE) == 0) {
        ownerNode = worldRuntime->ownerListHead;
        blocked = FieldGrid_TestWorldPointBlocked
                          (ownerFactionIndex,worldXQ12,worldYQ12,worldRuntime->fieldGrid);
        if (blocked) goto Reject;
        if (ownerNode != NULL) {
          do {
            if (ownerNode->ownerClassId == WORLD_OWNER_RUNTIME_MODEL) {
              /* the model's definition: support radius (supportRadius) and class id; the owner army
                 holds the owner faction */
              recordOrDistanceTerm = (int)((ModelRuntimeSlot *)ownerNode->runtimePayload)->definitionOrSavedId.runtimeDefinition;
              eaxContinuity = ((ModelDefinition *)recordOrDistanceTerm)->supportRadius;
              if (((eaxContinuity != 0) &&
                  (((ModelRuntimeSlot *)ownerNode->runtimePayload)->ownerArmyRuntimeOrSavedOffset.armyRuntime->factionIndex == ownerFactionIndex)) &&
                 ((eaxContinuity = eaxContinuity + placementClearancePaddingQ12,
                  ((ModelDefinition *)recordOrDistanceTerm)->runtimeClassId !=
                  MODEL_RUNTIME_CLASS_18 ||
                  (((placementMode & ARMY_PLACEMENT_MODE_SKIP_CLASS18_SUPPORT) == 0 &&
                   ((((ModelRuntimeSlot *)ownerNode->runtimePayload)->classState.stateFlags &
                     (ARMY_RUNTIME_FLAG_DESTROYED | ARMY_MODEL_STATE_DISMANTLING)) == 0)))))) {
                recordOrDistanceTerm = eaxContinuity + modelDefinition->placementFlags;
                radiusSquaredOrDelta = (int64_t)recordOrDistanceTerm * (int64_t)recordOrDistanceTerm;
                recordOrDistanceTerm = ownerNode->worldXQ12 - worldYQ12;
                remainingSquared = (int64_t)recordOrDistanceTerm * (int64_t)recordOrDistanceTerm;
                eaxContinuity = (int)remainingSquared;
                remainingSquared = radiusSquaredOrDelta - remainingSquared;
                if (-1 < remainingSquared) {
                  recordOrDistanceTerm = ownerNode->worldYQ12 - worldXQ12;
                  deltaYSquared = (int64_t)recordOrDistanceTerm * (int64_t)recordOrDistanceTerm;
                  eaxContinuity = (int)deltaYSquared;
                  remainingSquared = remainingSquared - deltaYSquared;
                  if (-1 < remainingSquared) {
                    if ((placementMode & ARMY_PLACEMENT_MODE_MEASURE_SUPPORT_DISTANCE) == 0)
                      goto Accept;
                    /* keep the nearest supporting model (smallest squared distance) */
                    radiusSquaredOrDelta = (radiusSquaredOrDelta - remainingSquared) - nearestDistanceSquared;
                    eaxContinuity = (int)radiusSquaredOrDelta;
                    if (radiusSquaredOrDelta < 0) {
                      nearestClearanceQ12 = ((ModelRuntimeSlot *)ownerNode->runtimePayload)->definitionOrSavedId.runtimeDefinition->supportRadius;
                      nearestDistanceSquared = radiusSquaredOrDelta + nearestDistanceSquared;
                    }
                  }
                }
              }
            }
            nearestDistanceHigh = (UInt64Half32)((uint64_t)nearestDistanceSquared >> 32);
            nearestDistanceLow = (UInt64Half32)nearestDistanceSquared;
            ownerNode = ownerNode->nextNode;
          } while (ownerNode != NULL);
          /* no supporting model in reach (the distance is still the INT64_MAX start value) */
          if (((placementMode & ARMY_PLACEMENT_MODE_MEASURE_SUPPORT_DISTANCE) == 0) ||
              (ARMY_PLACEMENT_NO_SUPPORT_DISTANCE_SQUARED < nearestDistanceSquared))
          goto Reject;
          /* EAX: free distance between the candidate's margin and the nearest supporter's radius */
          nearestDistanceQ12 = FixedMath_UInt64Sqrt(nearestDistanceHigh,nearestDistanceLow);
          recordOrDistanceTerm = nearestDistanceQ12 - nearestClearanceQ12;
          if (recordOrDistanceTerm < 0) {
            recordOrDistanceTerm = 0;
          }
          eaxContinuity = recordOrDistanceTerm - modelDefinition->placementFlags;
          if (eaxContinuity < 0) {
            eaxContinuity = 0;
          }
        }
      }
Accept:
      accepted.rejected = false;
      accepted.value = eaxContinuity;
      return accepted;
    }
  }
Reject:
  rejected.rejected = true;
  rejected.value = eaxContinuity;
  return rejected;
}


/* Address: 0x00524650.
   Tests whether a point comes within queryRadiusQ12 + ARMY_PLACEMENT_ANCHOR_CLEARANCE_Q12 of the model's
   (1,5) anchor point in world space - the second footprint of class-13 models. Returns CF: true = too close
   (and counts it in g_ArmyPlacementLateRejectionCount); false also when the model has no anchor point.
   Called directly by ArmyPlacementCollision_TestPointAgainstRuntimeList and
   ArmyPlacementCollision_TestCandidateAgainstRuntimeList.
*/
bool ArmyPlacementCandidate_TestModelAnchorDistance
          (Q12 queryRadiusQ12,Q12 targetWorldXQ12,Q12 targetWorldYQ12,ModelRuntimeSlot *modelRuntime)

{
  ModelRuntimeNode *modelNodeRuntime;
  uint32_t anchorDistanceQ12;
  ModelLookupEntryResult anchorLookup;
  ModelWorldPoint anchorWorldPoint;
  
  modelNodeRuntime = modelRuntime->rootModelNodeOrSavedOffset.modelNode;
  anchorLookup = ModelLookupTable_ContainsPackedKey(1,5,(modelNodeRuntime->modelPayload).modelResource);
  if (!anchorLookup.notFound) {
    anchorWorldPoint = ModelNodeRuntime_TransformLocalPointRegs(anchorLookup.entry,modelNodeRuntime);
    anchorDistanceQ12 = FixedMath_Length2(anchorWorldPoint.yQ12 - targetWorldXQ12,
                                          anchorWorldPoint.xQ12 - targetWorldYQ12);
    if ((int)anchorDistanceQ12 <= queryRadiusQ12 + ARMY_PLACEMENT_ANCHOR_CLEARANCE_Q12) {
      g_ArmyPlacementLateRejectionCount++;
      return true;
    }
  }
  return false;
}


/* Address: 0x00529C40.
   Exact circle test between a point and a model: true (CF set) when the point lies within the model's
   placement radius (definition footprintRadius, +0xDC) plus queryRadiusQ12 of the model's position, compared
   on the 64-bit squares. Models without radius and a zero query radius never hit.
   Called directly by the runtime-list collision scans in this file and by the movement code
   (gameplay/army/movement.c).
*/
bool ArmyCollision_TestPointWithinExpandedRuntimeRadius
          (Q12 queryRadiusQ12,Q12 worldXQ12,Q12 worldYQ12,ModelRuntimeSlot *modelRuntime)

{
  int64_t radiusSquared;
  int64_t distanceSquared;
  int deltaXOrRadiusQ12;
  int deltaYQ12;
  
  if (((modelRuntime->definitionOrSavedId.runtimeDefinition->footprintRadius != 0) &&
      (queryRadiusQ12 != 0)) &&
     (deltaXOrRadiusQ12 = (modelRuntime->rootModelNodeOrSavedOffset.modelNode->worldTransform).translation.x - worldYQ12,
     deltaYQ12 = (modelRuntime->rootModelNodeOrSavedOffset.modelNode->worldTransform).translation.y - worldXQ12,
     distanceSquared = (int64_t)deltaYQ12 * (int64_t)deltaYQ12 + (int64_t)deltaXOrRadiusQ12 * (int64_t)deltaXOrRadiusQ12,
     deltaXOrRadiusQ12 = modelRuntime->definitionOrSavedId.runtimeDefinition->footprintRadius + queryRadiusQ12, radiusSquared = (int64_t)deltaXOrRadiusQ12 * (int64_t)deltaXOrRadiusQ12,
     -1 < (int)(((int)((uint64_t)radiusSquared >> 32) - (int)((uint64_t)distanceSquared >> 32)) -
               (uint32_t)((uint32_t)radiusSquared < (uint32_t)distanceSquared)))) {
    return true;
  }
  return false;
}


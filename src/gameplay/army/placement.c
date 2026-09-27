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
   Ownership: gameplay/army/placement.
   Purpose: Tests a placement candidate, transforms the configured model offset, and rejects the candidate when
   clearance, terrain-band, or occupancy constraints fail. Twenty-four-entry army placement asset-class dispatch
   contract; CF carries acceptance and the first four class-specific dwords remain deliberately generic. Typed
   parameters: p0 dispatchArg0→ArmyPlacementDispatchArg0_V344, p1 dispatchArg1→ArmyPlacementDispatchArg1_V344, p2
   dispatchArg2→ArmyPlacementDispatchArg2_V344, p3 dispatchArg3→ArmyPlacementDispatchArg3_V344, p7
   dispatchArg7→ArmyPlacementDispatchArg7_V344. Calling convention, complete VariableStorage serialization,
   function bytes, control flow, globals, locals, and executable data remain unchanged.
   Local calls: ArmyPlacementCollision_TestCandidateAndClearanceCf,
   ArmyPlacementCollision_TestPointAgainstRuntimeListCf.
   Cross-module calls: ModelLookupTable_ContainsPackedKeyCf [assets/model/definitions],
   FixedMath_Vector2AngleAndLengthRegs [core/math/fixed], FixedMath_SinCosScaled [core/math/fixed].
*/

PlacementCandidateResult ArmyPlacementCandidate_TestOffsetClearanceCf
              (ArmyPlacementDispatchArg0 dispatchArg0,
              ArmyPlacementClearancePaddingQ12 placementClearancePaddingQ12,
              ArmyPlacementDispatchArg2 dispatchArg2,ArmyPlacementDispatchArg3 dispatchArg3,
              Q12 worldXQ12,Q12 worldYQ12,ModelDefinitionRuntimeSemanticView280 *modelDefinition,
              ArmyPlacementDispatchArg7 dispatchArg7,WorldRuntimeContext *worldRuntime)

{
  int offsetWorldXQ12;
  int eaxOrOffsetYQ12;
  bool blocked;
  FixedLengthAngleEaxEdx8 offsetLengthAngle;
  FixedSinCosEdxEax8 rotatedOffset;
  PlacementCandidateResult clearanceResult;
  ModelLookupEntryResult anchorLookup;
  TerrainPlacementResult terrainTest;
  
  PlacementCandidateResult result;

  /* The table dispatch reads EAX and CF; the decompiled int return lost CF. */
  clearanceResult = ArmyPlacementCollision_TestCandidateAndClearanceCf
                    (dispatchArg0,placementClearancePaddingQ12,dispatchArg2,dispatchArg3,
                     worldXQ12,worldYQ12,modelDefinition,dispatchArg7,worldRuntime);
  eaxOrOffsetYQ12 = clearanceResult.value;
  if (!clearanceResult.rejected) {
    anchorLookup = ModelLookupTable_ContainsPackedKeyCf
                      (1,5,*(ModelResourceHitTestAndRenderView210 **)
                            (modelDefinition->serializedNodeOffsetOrPointer64 + 0x30));
    offsetLengthAngle = FixedMath_Vector2AngleAndLengthRegs
                      (((anchorLookup.entry)->localPosition).y,((anchorLookup.entry)->localPosition).x);
    rotatedOffset = FixedMath_SinCosScaled(offsetLengthAngle.angle + dispatchArg2 & 0xffff,offsetLengthAngle.length);
    eaxOrOffsetYQ12 = (int)rotatedOffset;
    offsetWorldXQ12 = worldXQ12 + (int)(rotatedOffset >> 0x20);
    blocked = ArmyPlacementCollision_TestPointAgainstRuntimeListCf
                      (dispatchArg0,0xc00,offsetWorldXQ12,worldYQ12 + eaxOrOffsetYQ12,worldRuntime);
    if (!blocked) {
      terrainTest.rejected = (*g_TerrainClassPlacementAndOverlayCallbacks10.placementTests
                [modelDefinition->placementContactKindIndex278])
                        (0xc00,dispatchArg3,offsetWorldXQ12,worldYQ12 + eaxOrOffsetYQ12,worldRuntime->fieldGrid);
      if (!terrainTest.rejected) {
        result.value = clearanceResult.value;
        result.rejected = false;
        return result;
      }
    }
    g_ArmyPlacementAcceptedCandidateCount = g_ArmyPlacementAcceptedCandidateCount + 1;
  }
  result.value = eaxOrOffsetYQ12;
  result.rejected = true;
  return result;
}


/* Address: 0x00524570.
   Ownership: gameplay/army/placement.
   Purpose: Binary entry is anchored by g_CodePointerTable_0051FE78[37]@0051FE78. Placement-validation partition
   slots 24-47 receive (worldRuntime, armyRuntime), with CF carrying acceptance.
   Local calls: ArmyPlacementCollision_TestCurrentRuntimeCf,
   ArmyPlacementCollision_TestCandidateAgainstRuntimeListCf.
   Cross-module calls: ModelLookupTable_ContainsPackedKeyCf [assets/model/definitions],
   ModelNodeRuntime_TransformLocalPointRegs [world/model/hierarchy], TerrainAuxHeightThreshold_TestAroundWorldPoint
   [world/terrain/height], TerrainHeightBand_TestAroundWorldPoint [world/terrain/height].
*/

bool __thandor_cf_preserve_eax_ecx_edx
ArmyPlacement_TestModelTerrainAndRuntimeClearance
          (WorldRuntimeContext *worldRuntime,ModelRuntimePlacementValidationView200 *modelRuntime)

{
  ModelRuntimeNode *modelNodeRuntime;
  ModelDefinitionRuntimeSemanticView280 *placementDefinition;
  uint32_t worldYQ12;
  Q12 worldXQ12;
  int referenceHeightQ12;
  bool blocked;
  ModelLookupEntryResult anchorLookup;
  ModelWorldPoint anchorWorldPoint;
  
  modelNodeRuntime = modelRuntime->rootModelNode;
  blocked = ArmyPlacementCollision_TestCurrentRuntimeCf(worldRuntime,modelRuntime);
  if (!blocked) {
    placementDefinition = modelRuntime->modelDefinition;
    anchorLookup = ModelLookupTable_ContainsPackedKeyCf(1,5,(modelNodeRuntime->modelPayload).modelResource)
    ;
    if (!anchorLookup.notFound) {
      anchorWorldPoint = ModelNodeRuntime_TransformLocalPointRegs(anchorLookup.entry,modelNodeRuntime);
      worldXQ12 = anchorWorldPoint.yQ12;
      worldYQ12 = anchorWorldPoint.xQ12;
      referenceHeightQ12 =
           (anchorWorldPoint.zQ12 - placementDefinition->placementHeightOffsetQ12) -
           ((modelNodeRuntime->modelPayload).modelResource)->placementHeightOffsetQ12;
      blocked = ArmyPlacementCollision_TestCandidateAgainstRuntimeListCf
                        ((WorldOwnerListNode100 *)modelNodeRuntime,worldXQ12,worldYQ12,
                         (IMAGE_DOS_HEADER *)0xc00,worldRuntime);
      if (!blocked) {
        if (modelRuntime->modelDefinition->placementContactKindIndex278 == 1) {
          blocked = TerrainAuxHeightThreshold_TestAroundWorldPoint
                            (0xc00,referenceHeightQ12,worldXQ12,worldYQ12,worldRuntime->fieldGrid);
          if (!blocked) {
            return false;
          }
        }
        else {
          blocked = TerrainHeightBand_TestAroundWorldPoint
                            (0xc00,referenceHeightQ12,worldXQ12,worldYQ12,worldRuntime->fieldGrid);
          if (!blocked) {
            return false;
          }
        }
      }
    }
    g_ArmyPlacementAcceptedCandidateCount = g_ArmyPlacementAcceptedCandidateCount + 1;
  }
  return true;
}


Q12 g_ArmyPlacementValidatedWorldXQ12;
Q12 g_ArmyPlacementValidatedWorldYQ12;

/* Address: 0x0051D380.
   Ownership: gameplay/army/placement.
   Purpose: Validates one army asset placement at the requested point and aligned cell corners; CF carries
   acceptance. Typed parameters: p0 placementMode→ArmyPlacementMode_V344, p4
   placementContext→ArmyPlacementContext_V344. Calling convention, complete VariableStorage serialization, function
   bytes, control flow, globals, locals, and executable data remain unchanged.
   Local calls: ArmyPlacement_DispatchAssetAtFieldPoint.
*/
bool __thandor_preserve_eax
ArmyPlacement_ValidateAssetAtPointAndCellCornersCf
          (ArmyPlacementMode placementMode,PckArmyAssetIdCatalog armyAssetId,Q12 worldYQ12,
          Q12 worldXQ12,ArmyPlacementContext placementContext,FactionRuntimeIndex ownerFactionId,
          void *inGameRuntime)

{
  /* Rewritten from the assembly (0x0051D380-0x0051D447): the original also returns the point it
     accepted in ECX (x) / EDX (y) - the input point, or the first free snapped cell corner - which the
     decompiler dropped. Callers read it from g_ArmyPlacementValidatedWorldX/YQ12. */
  static const int cornerDx[4] = {0,0x240,0x240,0};
  static const int cornerDy[4] = {0,0,0x240,0x240};
  PlacementDispatchResult dispatched;
  int corner;

  g_ArmyPlacementValidatedWorldXQ12 = worldXQ12;
  g_ArmyPlacementValidatedWorldYQ12 = worldYQ12;
  dispatched = ArmyPlacement_DispatchAssetAtFieldPoint
                         (placementMode,0,armyAssetId,worldYQ12,worldXQ12,placementContext,
                          ownerFactionId,inGameRuntime);
  if (!dispatched.failed) {
    return false;
  }
  for (corner = 0; corner < 4; corner++) {
    Q12 x = (Q12)(((uint32_t)worldXQ12 & 0xffffff00) + cornerDx[corner]);
    Q12 y = (Q12)(((uint32_t)worldYQ12 & 0xffffff00) + cornerDy[corner]);
    dispatched = ArmyPlacement_DispatchAssetAtFieldPoint
                           (placementMode,0,armyAssetId,y,x,placementContext,ownerFactionId,
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
   Ownership: gameplay/army/placement.
   Purpose: Tests a placement candidate and rejects it when the projected field cell contains the configured
   occupancy bit for the selected runtime class. Twenty-four-entry army placement asset-class dispatch contract; CF
   carries acceptance and the first four class-specific dwords remain deliberately generic. Typed parameters: p0
   dispatchArg0→ArmyPlacementDispatchArg0_V344, p1 dispatchArg1→ArmyPlacementDispatchArg1_V344, p2
   dispatchArg2→ArmyPlacementDispatchArg2_V344, p3 dispatchArg3→ArmyPlacementDispatchArg3_V344, p7
   dispatchArg7→ArmyPlacementDispatchArg7_V344. Calling convention, complete VariableStorage serialization,
   function bytes, control flow, globals, locals, and executable data remain unchanged.
   Local calls: ArmyPlacementCollision_TestCandidateAndClearanceCf.
*/
PlacementCandidateResult ArmyPlacementCandidate_TestFieldOccupancyCf
              (ArmyPlacementDispatchArg0 dispatchArg0,
              ArmyPlacementClearancePaddingQ12 placementClearancePaddingQ12,
              ArmyPlacementDispatchArg2 dispatchArg2,ArmyPlacementDispatchArg3 dispatchArg3,
              Q12 worldYQ12,Q12 worldXQ12,ModelDefinitionRecordPrefix *modelDefinition,
              ArmyPlacementDispatchArg7 dispatchArg7,WorldRuntimeContext *worldRuntime)

{
  int clearanceEax;
  int cellColumn;
  int eaxOrCellColumn;
  PlacementCandidateResult result;
  uint32_t projectedRowTerm;
  int cellRow;
  PlacementCandidateResult clearanceResult;
  FieldGridAsset *activeFieldGrid;
  
  clearanceResult = ArmyPlacementCollision_TestCandidateAndClearanceCf
                    (dispatchArg0,placementClearancePaddingQ12,dispatchArg2,dispatchArg3,
                     worldYQ12,worldXQ12,(ModelDefinitionRuntimeSemanticView280 *)modelDefinition,
                     dispatchArg7,worldRuntime);
  clearanceEax = clearanceResult.value;
  eaxOrCellColumn = clearanceEax;
  if (!clearanceResult.rejected) {
    activeFieldGrid = worldRuntime->fieldGrid;
    projectedRowTerm = (int)((uint64_t)((int64_t)worldYQ12 * -0x20c8cc) >> 0x20) << 0xb |
            (uint32_t)((int64_t)worldYQ12 * -0x20c8cc) >> 0x15;
    cellColumn = (int)((((int)((uint64_t)((int64_t)worldXQ12 * 0x1c6e9c) >> 0x20) << 0xc |
                   (uint32_t)((int64_t)worldXQ12 * 0x1c6e9c) >> 0x14) - projectedRowTerm) + 0x800) >> 0xc;
    eaxOrCellColumn = cellColumn;
    if ((((-1 < cellColumn) && (cellRow = (int)(projectedRowTerm * 2 + 0x800) >> 0xc, -1 < cellRow)) &&
        (cellColumn < (int)activeFieldGrid->gridWidth)) && (cellRow < (int)activeFieldGrid->gridHeight)) {
      eaxOrCellColumn = clearanceEax;
      if ((activeFieldGrid->cells[activeFieldGrid->gridWidth * cellRow + cellColumn].flagsAndMaterial &
          0x800 << ((uint8_t)modelDefinition[0x10].byteSize & 0x1f)) != 0) {
        result.value = clearanceEax;
        result.rejected = false;
        return result;
      }
    }
    g_ArmyPlacementAcceptedCandidateCount = g_ArmyPlacementAcceptedCandidateCount + 1;
  }
  result.value = eaxOrCellColumn;
  result.rejected = true;
  return result;
}


/* Address: 0x00524F70.
   Ownership: gameplay/army/placement.
   Purpose: Binary entry is anchored by g_CodePointerTable_0051FE78[38]@0051FE78. Placement-validation partition
   slots 24-47 receive (worldRuntime, armyRuntime), with CF carrying acceptance.
   [FIELD_GRID_STORAGE_NAMESPACE_DB_CLOSURE] Positive support predicate: required mask is 0x00000800 << model
   occupancy class. Missing bit rejects, present bit accepts subject to other placement tests.
   Local calls: ArmyPlacementCollision_TestCurrentRuntimeCf.
   Cross-module calls: FieldGrid_WorldToGridQ12 [world/terrain/grid].
*/
bool __thandor_cf_preserve_eax_ecx_edx
ArmyPlacement_TestGridOccupancyMask
          (WorldRuntimeContext *worldRuntime,ModelRuntimePlacementClass14View200 *modelRuntime)

{
  int cellColumn;
  int cellRow;
  bool blocked;
  FieldGridCoordinatesEaxEdx8 gridCoordinates;
  FieldGridAsset *activeFieldGrid;
  ModelRuntimeNode *rootNode;
  
  rootNode = modelRuntime->rootModelNode;
  blocked = ArmyPlacementCollision_TestCurrentRuntimeCf
                    (worldRuntime,(ModelRuntimePlacementValidationView200 *)modelRuntime);
  if (!blocked) {
    gridCoordinates = FieldGrid_WorldToGridQ12
                      ((rootNode->worldTransform).translation.y,
                       (rootNode->worldTransform).translation.x);
    cellColumn = (gridCoordinates.columnQ12 >> 0xb) + 1 >> 1;
    cellRow = (gridCoordinates.rowQ12 >> 0xb) + 1 >> 1;
    activeFieldGrid = worldRuntime->fieldGrid;
    if ((0 < cellColumn) && (0 < cellRow)) {
      if ((cellColumn + 1 < (int)activeFieldGrid->gridWidth) &&
         ((cellRow + 1 < (int)activeFieldGrid->gridHeight &&
          ((activeFieldGrid->cells[cellRow * activeFieldGrid->gridWidth + cellColumn].flagsAndMaterial &
           0x800 << ((uint8_t)modelRuntime->modelDefinition->resourceFieldSupportSelectorC0 & 0x1f)) !=
           0)))) {
        return false;
      }
    }
    g_ArmyPlacementAcceptedCandidateCount = g_ArmyPlacementAcceptedCandidateCount + 1;
  }
  return true;
}


/* Address: 0x00528110.
   Ownership: gameplay/army/placement.
   Purpose: Binary entry is anchored by g_CodePointerTable_0051FE78[25]@0051FE78;
   g_CodePointerTable_0051FE78[26]@0051FE78; g_CodePointerTable_0051FE78[27]@0051FE78;
   g_CodePointerTable_0051FE78[41]@0051FE78; g_CodePointerTable_0051FE78[42]@0051FE78;
   g_CodePointerTable_0051FE78[43]@0051FE78. Placement-validation partition slots 24-47 receive (worldRuntime,
   armyRuntime), with CF carrying acceptance.
   Local calls: ArmyCollision_FindBlockingRuntimeForCurrentUnitCf.
   Cross-module calls: GridScratch_TestProjectedCellMaskBandsCf [world/pathing/grid],
   FieldGrid_TestWorldPointBlockedCf [world/terrain/grid].
*/
bool __thandor_cf_preserve_eax
ArmyPlacement_TestGridRuntimeAndFieldBlocking
          (WorldRuntimeContext *worldRuntime,ModelRuntimePlacementValidationView200 *modelRuntime)

{
  bool blocked;
  ArmyCollisionResult blockingRuntime;
  ModelRuntimeNode *modelNode;
  
  modelNode = modelRuntime->rootModelNode;
  blocked = GridScratch_TestProjectedCellMaskBandsCf
                    ((modelNode->worldTransform).translation.y,
                     (modelNode->worldTransform).translation.x,
                     (uint8_t)modelRuntime->modelDefinition->gridClassification260,
                     (uint8_t)modelRuntime->modelDefinition->gridClassification264);
  if (!blocked) {
    blockingRuntime = ArmyCollision_FindBlockingRuntimeForCurrentUnitCf
                      ((modelNode->worldTransform).translation.y,
                       (modelNode->worldTransform).translation.x,
                       (RuntimeCollisionQueryViewF4 *)modelRuntime,worldRuntime);
    blocked = blockingRuntime.blocked;
    if ((!blocked) && (blocked = false, (g_UiCommandRuntimeFlags & 4) == 0)) {
      blocked = FieldGrid_TestWorldPointBlockedCf
                        (modelRuntime->ownerArmyRuntime->factionIndex,
                         (modelNode->worldTransform).translation.y,
                         (modelNode->worldTransform).translation.x,worldRuntime->fieldGrid);
    }
  }
  return blocked;
}


/* Address: 0x005281A0.
   Ownership: gameplay/army/placement.
   Purpose: Tests a shot spawn point against the definition bounds, depth-bin collision state, and field occupancy
   before permitting the shot path. Twenty-four-entry army placement asset-class dispatch contract; CF carries
   acceptance and the first four class-specific dwords remain deliberately generic. Typed parameters: p7
   dispatchArg7→ArmyPlacementDispatchArg7_V344. Calling convention, complete VariableStorage serialization,
   function bytes, control flow, globals, locals, and executable data remain unchanged.
   Local calls: ArmyCollision_TestPointAgainstRuntimeListCf.
   Cross-module calls: GridScratch_TestProjectedCellMaskBandsCf [world/pathing/grid],
   FieldGrid_TestWorldPointBlockedCf [world/terrain/grid].
*/
PlacementCandidateResult ArmyRuntimeCollision_TestShotSpawnPointCf
               (uint32_t dispatchArg0,uint32_t dispatchArg1,uint32_t dispatchArg2,uint32_t dispatchArg3,
               Q12 worldXQ12,Q12 worldYQ12,ModelDefinitionRuntimeSemanticView280 *modelDefinition,
               ArmyPlacementDispatchArg7 dispatchArg7,WorldRuntimeContext *worldRuntime)

{
  bool blocked;
  
  blocked = GridScratch_TestProjectedCellMaskBandsCf
                    (worldXQ12,worldYQ12,(uint8_t)modelDefinition->gridClassification260,
                     (uint8_t)modelDefinition->gridClassification264);
  if (!blocked) {
    blocked = ArmyCollision_TestPointAgainstRuntimeListCf
                      (worldXQ12,worldYQ12,(uint8_t *)modelDefinition,worldRuntime);
    if ((!blocked) && (blocked = false, (g_UiCommandRuntimeFlags & 4) == 0)) {
      blocked = FieldGrid_TestWorldPointBlockedCf
                        (dispatchArg7,worldXQ12,worldYQ12,worldRuntime->fieldGrid);
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
   Ownership: gameplay/army/placement.
   Purpose: Preserved EAX/ECX/EDX values are incidental caller state, not a normal return contract.
   Cross-module calls: FieldGrid_InterpolateTerrainHeight [world/terrain/grid].
*/
void __thandor_void_preserve_eax_ecx_edx
ArmyPlacementContact_ApplyTerrainHeight
          (Q12 heightOffsetQ12,Q12 worldXQ12,Q12 worldYQ12,ModelRuntimeNode *modelNode,
          WorldRuntimeContext *worldRuntime)

{
  Q12 surfaceHeightQ12;
  HeightSampleResult surfaceHeight;
  
  if (worldRuntime->fieldGrid != (FieldGridAsset *)0x0) {
    surfaceHeight = FieldGrid_InterpolateTerrainHeight(worldXQ12,worldYQ12,worldRuntime->fieldGrid);
    surfaceHeightQ12 = surfaceHeight.heightQ12;
    if (!surfaceHeight.failed) {
      (modelNode->worldTransform).translation.z =
           surfaceHeightQ12 + heightOffsetQ12 +
           ((modelNode->modelPayload).modelResource)->placementHeightOffsetQ12;
      (modelNode->worldTransform).translation.x = worldYQ12;
      (modelNode->worldTransform).translation.y = worldXQ12;
      (modelNode->modelPayload).worldRotationAngle1 = 0x4000;
      modelNode->runtimeFlags = modelNode->runtimeFlags | 1;
    }
  }
  return;
}


/* Address: 0x004BE860.
   Ownership: gameplay/army/placement.
   Purpose: Preserved EAX/ECX/EDX values are incidental caller state, not a normal return contract.
   Cross-module calls: FieldGrid_InterpolateWaterSurfaceHeight [world/terrain/grid].
*/
void __thandor_void_preserve_eax_ecx_edx
ArmyPlacementContact_ApplyWaterSurfaceHeight
          (Q12 heightOffsetQ12,Q12 worldXQ12,Q12 worldYQ12,ModelRuntimeNode *modelNode,
          WorldRuntimeContext *worldRuntime)

{
  Q12 surfaceHeightQ12;
  HeightSampleResult surfaceHeight;
  
  if (worldRuntime->fieldGrid != (FieldGridAsset *)0x0) {
    surfaceHeight = FieldGrid_InterpolateWaterSurfaceHeight(worldXQ12,worldYQ12,worldRuntime->fieldGrid);
    surfaceHeightQ12 = surfaceHeight.heightQ12;
    if (!surfaceHeight.failed) {
      (modelNode->worldTransform).translation.z = surfaceHeightQ12 + heightOffsetQ12;
      (modelNode->worldTransform).translation.x = worldYQ12;
      (modelNode->worldTransform).translation.y = worldXQ12;
      (modelNode->modelPayload).worldRotationAngle1 = 0x4000;
      modelNode->runtimeFlags = modelNode->runtimeFlags | 1;
    }
  }
  return;
}


/* Address: 0x004BE8C0.
   Ownership: gameplay/army/placement.
   Purpose: Preserved EAX/ECX/EDX values are incidental caller state, not a normal return contract.
   Cross-module calls: FieldGrid_InterpolateTerrainHeightAndNormal [world/terrain/grid].
*/
void __thandor_void_preserve_eax_ecx_edx
ArmyPlacementContact_ApplyTerrainHeightAndNormal
          (Q12 heightOffsetQ12,Q12 worldXQ12,Q12 worldYQ12,ModelRuntimeNode *modelNode,
          WorldRuntimeContext *worldRuntime)

{
  int resourceHeightOffsetQ12;
  HeightNormalSampleResult surfaceHeightNormal;
  
  if (worldRuntime->fieldGrid != (FieldGridAsset *)0x0) {
    surfaceHeightNormal = FieldGrid_InterpolateTerrainHeightAndNormal(worldXQ12,worldYQ12,worldRuntime->fieldGrid)
    ;
    if (!surfaceHeightNormal.failed) {
      resourceHeightOffsetQ12 = ((modelNode->modelPayload).modelResource)->placementHeightOffsetQ12;
      (modelNode->modelPayload).worldRotationAngle0 = surfaceHeightNormal.packedNormalAngles & 0xffff;
      (modelNode->modelPayload).worldRotationAngle1 = (int)surfaceHeightNormal.packedNormalAngles >> 0x10;
      (modelNode->worldTransform).translation.z = surfaceHeightNormal.heightQ12 + resourceHeightOffsetQ12 + heightOffsetQ12;
      (modelNode->worldTransform).translation.x = worldYQ12;
      (modelNode->worldTransform).translation.y = worldXQ12;
      modelNode->runtimeFlags = modelNode->runtimeFlags | 1;
    }
  }
  return;
}


/* Address: 0x004BE930.
   Ownership: gameplay/army/placement.
   Purpose: Preserved EAX/ECX/EDX values are incidental caller state, not a normal return contract.
   Cross-module calls: FieldGrid_InterpolateTopSurfaceHeight [world/terrain/grid].
*/
void __thandor_void_preserve_eax_ecx_edx
ArmyPlacementContact_ApplyTopSurfaceHeight
          (Q12 heightOffsetQ12,Q12 worldXQ12,Q12 worldYQ12,ModelRuntimeNode *modelNode,
          WorldRuntimeContext *worldRuntime)

{
  Q12 surfaceHeightQ12;
  HeightSampleResult surfaceHeight;
  
  if (worldRuntime->fieldGrid != (FieldGridAsset *)0x0) {
    surfaceHeight = FieldGrid_InterpolateTopSurfaceHeight(worldXQ12,worldYQ12,worldRuntime->fieldGrid);
    surfaceHeightQ12 = surfaceHeight.heightQ12;
    if (!surfaceHeight.failed) {
      (modelNode->worldTransform).translation.z = surfaceHeightQ12 + heightOffsetQ12;
      (modelNode->worldTransform).translation.x = worldYQ12;
      (modelNode->worldTransform).translation.y = worldXQ12;
      (modelNode->modelPayload).worldRotationAngle1 = 0x4000;
      modelNode->runtimeFlags = modelNode->runtimeFlags | 1;
    }
  }
  return;
}


/* Address: 0x004BE990.
   Ownership: gameplay/army/placement.
   Purpose: Preserved EAX/ECX/EDX values are incidental caller state, not a normal return contract.
   Cross-module calls: ArmyArticulatedRuntime_InitializeTerrainContactGeometry [gameplay/army/movement],
   ArmyArticulatedRuntime_UpdateSuspensionHierarchy [gameplay/army/movement].
*/
void __thandor_void_preserve_eax_ecx_edx
ArmyPlacementContact_InitializeArticulatedSuspension
          (Q12 heightOffsetQ12,Q12 worldXQ12,Q12 worldYQ12,ModelRuntimeNode *modelNode,
          WorldRuntimeContext *worldRuntime)

{
  (modelNode->worldTransform).translation.x = worldYQ12;
  (modelNode->worldTransform).translation.y = worldXQ12;
  modelNode->runtimeFlags = modelNode->runtimeFlags | 1;
  ArmyArticulatedRuntime_InitializeTerrainContactGeometry(modelNode,worldRuntime);
  ArmyArticulatedRuntime_UpdateSuspensionHierarchy(modelNode,worldRuntime);
  return;
}


/* Address: 0x00525320.
   Ownership: gameplay/army/placement.
   Purpose: Table membership PLACEMENT_VALIDATION[14]. Releases the faction runtime-capacity contribution and
   clears the two reserved field-grid runtime markers at the placed unit cell. Model release partition slots 0-23
   receive (modelDefinition, modelRuntime).
   Cross-module calls: FieldGrid_WorldToGridQ12 [world/terrain/grid].
*/
void __thandor_void_preserve_eax_ecx_edx
ArmyPlacement_ReleaseFactionCapacityAndClearGridReservation
          (ModelDefinitionRecordPrefix *modelDefinition,ModelRuntimeSlot *modelRuntime)

{
  uint8_t *capacityCounter;
  uint32_t capacityContribution;
  InGameRuntimeRootImageC3E4 *inGameRoot;
  int factionValueOrCellIndex;
  int counterOffsetOrCellRow;
  FieldGridCoordinatesEaxEdx8 gridCoordinates;
  FieldGridAsset *activeFieldGrid;
  
  capacityContribution = modelDefinition[0x10].flags;
  factionValueOrCellIndex = ((modelRuntime->ownerArmyRuntimeOrSavedOffset).armyRuntime)->factionIndex * 0x740;
  counterOffsetOrCellRow = factionValueOrCellIndex + 4;
  if (modelDefinition[0x10].byteSize != 0) {
    counterOffsetOrCellRow = factionValueOrCellIndex + 0x14;
  }
  factionValueOrCellIndex = *(int *)(g_GameFactionRuntimeImage.records[0].reserved78_87 + counterOffsetOrCellRow + -0x78);
  if ((((int)modelRuntime->definitionValue60_3C < 2) && (factionValueOrCellIndex != 0)) &&
     (((modelRuntime->classState).classStateEC & 0x20) == 0)) {
    *(int *)(counterOffsetOrCellRow + THANDOR_ADDR(g_GameFactionRuntimeImage,-4)) =
         *(int *)(counterOffsetOrCellRow + THANDOR_ADDR(g_GameFactionRuntimeImage,-4)) -
         (int)(((int64_t)(int)capacityContribution * (int64_t)*(int *)(counterOffsetOrCellRow + THANDOR_ADDR(g_GameFactionRuntimeImage,-4))) / (int64_t)factionValueOrCellIndex);
  }
  capacityCounter = g_GameFactionRuntimeImage.records[0].reserved78_87 + counterOffsetOrCellRow + -0x78;
  *(uint32_t *)capacityCounter = *(int *)capacityCounter - capacityContribution;
  inGameRoot = g_InGameRuntimeRoot;
  gridCoordinates = FieldGrid_WorldToGridQ12
                    ((((modelRuntime->rootModelNodeOrSavedOffset).modelNode)->worldTransform).
                     translation.y,
                     (((modelRuntime->rootModelNodeOrSavedOffset).modelNode)->worldTransform).
                     translation.x);
  factionValueOrCellIndex = (gridCoordinates.columnQ12 >> 0xb) + 1 >> 1;
  counterOffsetOrCellRow = (gridCoordinates.rowQ12 >> 0xb) + 1 >> 1;
  activeFieldGrid = (inGameRoot->worldRuntime0A30).fieldGrid;
  if (((0 < factionValueOrCellIndex) && (0 < counterOffsetOrCellRow)) && (activeFieldGrid != (FieldGridAsset *)0x0)) {
    if ((factionValueOrCellIndex + 1 < (int)activeFieldGrid->gridWidth) && (counterOffsetOrCellRow + 1 < (int)activeFieldGrid->gridHeight)) {
      factionValueOrCellIndex = counterOffsetOrCellRow * activeFieldGrid->gridWidth + factionValueOrCellIndex;
      activeFieldGrid->cells[factionValueOrCellIndex].armyRuntimeSavedOffset6C = 0;
      activeFieldGrid->cells[factionValueOrCellIndex].resourceExtractionDescriptor7C = 0;
    }
  }
  return;
}


/* Address: 0x00525420.
   Ownership: gameplay/army/placement.
   Purpose: Table membership PLACEMENT_VALIDATION[15]. Releases the faction runtime-capacity contribution without
   changing field-grid reservation markers. Model release partition slots 0-23 receive (modelDefinition,
   modelRuntime).
*/
void __thandor_void_preserve_eax_ecx_edx
ArmyPlacement_ReleaseFactionCapacity
          (ModelDefinitionRecordPrefix *modelDefinition,ModelRuntimeSlot *modelRuntime)

{
  uint8_t *capacityCounter;
  uint32_t capacityContribution;
  int factionOffsetOrCapacityTotal;
  int counterOffset;
  
  capacityContribution = modelDefinition[0x10].flags;
  factionOffsetOrCapacityTotal = ((modelRuntime->ownerArmyRuntimeOrSavedOffset).armyRuntime)->factionIndex * 0x740;
  counterOffset = factionOffsetOrCapacityTotal + 4;
  if (modelDefinition[0x10].byteSize != 0) {
    counterOffset = factionOffsetOrCapacityTotal + 0x14;
  }
  factionOffsetOrCapacityTotal = *(int *)(g_GameFactionRuntimeImage.records[0].reserved78_87 + counterOffset + -0x78);
  if ((((int)modelRuntime->definitionValue60_3C < 2) && (factionOffsetOrCapacityTotal != 0)) &&
     (((modelRuntime->classState).classStateEC & 0x20) == 0)) {
    *(int *)(counterOffset + THANDOR_ADDR(g_GameFactionRuntimeImage,-4)) =
         *(int *)(counterOffset + THANDOR_ADDR(g_GameFactionRuntimeImage,-4)) -
         (int)(((int64_t)(int)capacityContribution * (int64_t)*(int *)(counterOffset + THANDOR_ADDR(g_GameFactionRuntimeImage,-4))) / (int64_t)factionOffsetOrCapacityTotal);
  }
  capacityCounter = g_GameFactionRuntimeImage.records[0].reserved78_87 + counterOffset + -0x78;
  *(uint32_t *)capacityCounter = *(int *)capacityCounter - capacityContribution;
  return;
}


/* Address: 0x005263E0.
   Ownership: gameplay/army/placement.
   Purpose: Table membership PLACEMENT_VALIDATION[21]. Releases the matching class-state reservation bit and count
   for the runtime model-definition class. Model release partition slots 0-23 receive (modelDefinition,
   modelRuntime).
*/
void __thandor_void_preserve_eax_ecx_edx
ArmyPlacement_ReleaseClassStateReservation
          (ModelDefinitionRecordPrefix *modelDefinition,ModelRuntimeSlot *modelRuntime)

{
  uint32_t *classCounter;
  int32_t *reservationBits;
  ModelRuntimeArmyLinkOrState4 *armyLinkState;
  int remainingCount;
  ModelRuntimeSlot *slotCursor;
  uint32_t reservationBit;
  ModelRuntimeSlot *linkedModelSlot;
  
  linkedModelSlot = (modelRuntime->classLinkState).modelLinkOrState60.modelRuntime;
  if (linkedModelSlot != (ModelRuntimeSlot *)0x0) {
    remainingCount = 0xd;
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
        if (((modelRuntime->classState).classStateEC & 0x20) != 0) {
          return;
        }
        (slotCursor->classLinkState).classState78 = 0;
        armyLinkState = &(linkedModelSlot->classLinkState).armyLinkOrState6C;
        armyLinkState->armyRuntime = (ArmyRuntimeSlot *)(armyLinkState->classState - 1);
        classCounter = &(linkedModelSlot->classLinkState).classState70;
        *classCounter = *classCounter - 1;
        return;
      }
      slotCursor = (ModelRuntimeSlot *)&slotCursor->rootModelNodeOrSavedOffset;
      reservationBit = reservationBit * 2;
      remainingCount = remainingCount + -1;
    } while (remainingCount != 0);
  }
  return;
}


/* Address: 0x00527BD0.
   Ownership: gameplay/army/placement.
   Purpose: Exact nine-argument default that returns CF clear with ret 0x24 while preserving EAX. Twenty-four-entry
   army placement asset-class dispatch contract; CF carries acceptance and the first four class-specific dwords
   remain deliberately generic.
*/
PlacementCandidateResult ArmyPlacementAssetClassDispatch_AlwaysSuccessCf
               (uint32_t dispatchArg0,uint32_t dispatchArg1,uint32_t dispatchArg2,uint32_t dispatchArg3,
               Q12 worldXQ12,Q12 worldYQ12,ModelDefinitionRecordPrefix *modelDefinition,
               uint32_t dispatchArg7,WorldRuntimeContext *worldRuntime)

{
  PlacementCandidateResult result;

  result.value = 0;
  result.rejected = false;
  return result;
}

/* Address: 0x00529CB0.
   Ownership: gameplay/army/placement.
   Purpose: Saved ids, relocated pointers, attachment selectors, and runtime class ids remain separate. It is
   separate from world-unit radii, angles, grid indices, and serialized PCK identities.
   Local calls: ArmyCollision_TestPointWithinExpandedRuntimeRadiusCf.
   Cross-module calls: DepthInterval_BuildBinMask [graphics/render/primitives], DepthBinMasks_OverlapCf
   [graphics/render/primitives].
*/

bool __thandor_cf_preserve_eax_ecx_edx
ArmyCollision_TestPointAgainstRuntimeListCf
          (Q12 worldXQ12,Q12 worldYQ12,uint8_t *modelDefinition,WorldRuntimeContext *worldRuntime)

{
  int intervalRadius;
  DepthBinMask32 firstMaskHigh;
  DepthBinMask32 firstMaskLow;
  Q12 queryRadiusQ12;
  WorldOwnerListNode100 *ownerNode;
  bool hit;
  
  intervalRadius = *(int *)(modelDefinition + 0xdc);
  ownerNode = worldRuntime->ownerListHead;
  if ((intervalRadius != 0) && (ownerNode != (WorldOwnerListNode100 *)0x0)) {
    firstMaskHigh = DepthInterval_BuildBinMask(intervalRadius,worldYQ12);
    firstMaskLow = DepthInterval_BuildBinMask(intervalRadius,worldXQ12);
    do {
      if (ownerNode->ownerClassId == WORLD_OWNER_RUNTIME_MODEL) {
        hit = DepthBinMasks_OverlapCf
                          (firstMaskLow,firstMaskHigh,ownerNode->modelDepthBinMaskFar,
                           ownerNode->modelDepthBinMaskNear);
        if (hit) {
          hit = ArmyCollision_TestPointWithinExpandedRuntimeRadiusCf
                            (intervalRadius,worldXQ12,worldYQ12,ownerNode->runtimePayload);
          if (hit) {
            return true;
          }
        }
      }
      ownerNode = ownerNode->nextNode;
    } while (ownerNode != (WorldOwnerListNode100 *)0x0);
  }
  return false;
}


/* Address: 0x00529E60.
   Ownership: gameplay/army/placement.
   Purpose: Scans the active runtime list for a blocking object that overlaps the current runtime depth bins,
   excluding self and the verified owner and target relationships, and returns the matching runtime pointer. Typed
   parameters: p2 worldXQ12→Q12, p3 worldYQ12→Q12. Nearby but non-identical semantic domains were explicitly
   deferred. Calling convention, parameter storage, body bytes, control flow, globals, locals, and executable data
   remain unchanged.
   Local calls: ArmyCollision_TestPointWithinExpandedRuntimeRadiusCf.
   Cross-module calls: DepthBinMasks_OverlapCf [graphics/render/primitives].
*/
ArmyCollisionResult __thandor_eax_cf_preserve_ecx_edx
ArmyCollision_FindBlockingRuntimeForCurrentUnitCf
          (Q12 worldXQ12,Q12 worldYQ12,RuntimeCollisionQueryViewF4 *currentRuntime,
          WorldRuntimeContext *worldRuntime)

{
  ModelRuntimeNode *currentModelNode;
  uint32_t clearanceRadiusQ12;
  ArmyRuntimeSlot *armyRuntime;
  Q12 queryRadiusQ12;
  bool hit;
  ArmyCollisionResult notFound;
  ArmyCollisionResult found;
  ModelRuntimeNode *candidateModelNode;
  
  currentModelNode = currentRuntime->modelNodeRuntime;
  clearanceRadiusQ12 = currentRuntime->modelDefinition->placementRadiusOrClearanceDC;
  candidateModelNode = (ModelRuntimeNode *)worldRuntime->ownerListHead;
  if (clearanceRadiusQ12 != 0) {
    for (; candidateModelNode != (ModelRuntimeNode *)0x0;
        candidateModelNode = (ModelRuntimeNode *)(candidateModelNode->common).nextNode) {
      if (((((candidateModelNode->ownerClassId == WORLD_OWNER_RUNTIME_MODEL) &&
            (hit = DepthBinMasks_OverlapCf
                               (currentRuntime->modelNodeRuntime->depthBinMaskFar,
                                currentRuntime->modelNodeRuntime->depthBinMaskNear,
                                candidateModelNode->depthBinMaskFar,
                                candidateModelNode->depthBinMaskNear), hit)) &&
           (armyRuntime = (candidateModelNode->runtimePayload).armyRuntime,
           currentModelNode != candidateModelNode)) &&
          ((currentRuntime == (RuntimeCollisionQueryViewF4 *)0x0 ||
           ((armyRuntime != currentRuntime->linkedRuntimeF0 &&
            ((ArmyRuntimeSlot *)currentRuntime != armyRuntime->linkedArmyRuntime)))))) &&
         (hit = ArmyCollision_TestPointWithinExpandedRuntimeRadiusCf
                            (clearanceRadiusQ12,worldXQ12,worldYQ12,armyRuntime), hit)) {
        found.blocked = true;
        found.blockingArmy = (uint32_t)armyRuntime;
        return found;
      }
    }
  }
  notFound.blockingArmy = 0;
  notFound.blocked = false;
  return notFound;
}


/* Address: 0x0051D450.
   Ownership: gameplay/army/placement.
   Purpose: Typed parameters: p2 placementMode→ArmyPlacementMode_V344, p3
   placementAuxiliaryValue→ArmyPlacementAuxiliaryValue_V344, p8 placementContext→ArmyPlacementContext_V344. Calling
   convention, complete VariableStorage serialization, function bytes, control flow, globals, locals, and
   executable data remain unchanged.
   Cross-module calls: ArmyAssetRegistry_FindByIdCf [assets/army/catalog],
   ModelDefinitionRegistry_FindByIdWithErrorCf [assets/model/definitions].
*/
PlacementDispatchResult __thandor_eax_cf_preserve_ecx_edx
ArmyPlacement_DispatchAssetAtFieldPoint
          (ArmyPlacementMode placementMode,
          ArmyPlacementClearancePaddingQ12 placementClearancePaddingQ12,
          FactionRuntimeIndex ownerFactionIndex,Q12 worldYQ12,Q12 worldXQ12,
          PckArmyAssetIdCatalog armyAssetId,ArmyPlacementContext placementContext,
          UiRootNode *inGameRoot)

{
  uint32_t assetClassIndex;
  ArmyAssetRecordPrefix *modelDefinition;
  ArmyAssetLookupResult lookupResult;
  HeightSampleResult terrainHeight;
  PlacementDispatchResult dispatchResult;
  
  lookupResult = ArmyAssetRegistry_FindByIdCf(armyAssetId);
  if (!lookupResult.notFound) {
    lookupResult = THANDOR_BITCAST(ModelDefinitionResult, ArmyAssetLookupResult, ModelDefinitionRegistry_FindByIdWithErrorCf
                      (*(PckModelDefinitionIdCatalog *)((lookupResult.recordOrError)->rootNodeOffsetOrPointer + 0x20)
                      ));
    modelDefinition = lookupResult.recordOrError;
    if (!lookupResult.notFound) {
      assetClassIndex = modelDefinition[4].rootNodeOffsetOrPointer;
      terrainHeight = (*g_FieldGridInterpolationCallbacks5.callbacks[modelDefinition[0x27].registryId])
                        (worldYQ12,worldXQ12,(FieldGridAsset *)inGameRoot->previousRoot);
      lookupResult = THANDOR_BITCAST(PlacementDispatchResult, ArmyAssetLookupResult, (*g_ArmyRuntimeOrderHandlerMatrix11Columns24Classes.placementAssetClassDispatch[assetClassIndex]
              )(placementMode,placementClearancePaddingQ12,ownerFactionIndex,terrainHeight.heightQ12,
                worldYQ12,worldXQ12,(ModelDefinitionRecordPrefix *)modelDefinition,placementContext,
                (WorldRuntimeContext *)inGameRoot));
    }
  }
  dispatchResult.value = (uint32_t)lookupResult.recordOrError;
  dispatchResult.failed = lookupResult.notFound;
  return dispatchResult;
}


/* Address: 0x00529D70.
   Ownership: gameplay/army/placement.
   Purpose: Scans the active runtime list with the verified class filters and uses the class-0x0D model-anchor
   distance fallback before accepting a placement collision. It is separate from world-unit radii, angles, grid
   indices, and serialized PCK identities. Typed parameters: p2
   placementFilterFlags→ArmyPlacementCollisionFilterFlags_V344. Calling convention, complete VariableStorage
   serialization, function bytes, control flow, globals, locals, and executable data remain unchanged.
   Local calls: ArmyCollision_TestPointWithinExpandedRuntimeRadiusCf,
   ArmyPlacementCandidate_TestModelAnchorDistanceCf.
   Cross-module calls: DepthInterval_BuildBinMask [graphics/render/primitives], DepthBinMasks_OverlapCf
   [graphics/render/primitives].
*/

bool __thandor_cf_preserve_eax_ecx_edx
ArmyPlacementCollision_TestPointAgainstRuntimeListCf
          (ArmyPlacementCollisionFilterFlags placementFilterFlags,Q12 queryRadiusQ12,Q12 worldXQ12,
          Q12 worldYQ12,WorldRuntimeContext *worldRuntime)

{
  int modelClassId;
  DepthBinMask32 firstMaskHigh;
  DepthBinMask32 firstMaskLow;
  Q12 unusedRadiusQ12;
  WorldOwnerListNode100 *ownerNode;
  bool hit;
  
  ownerNode = worldRuntime->ownerListHead;
  if ((queryRadiusQ12 != 0) && (ownerNode != (WorldOwnerListNode100 *)0x0)) {
    firstMaskHigh = DepthInterval_BuildBinMask(queryRadiusQ12,worldYQ12);
    firstMaskLow = DepthInterval_BuildBinMask(queryRadiusQ12,worldXQ12);
    do {
      if (ownerNode->ownerClassId == WORLD_OWNER_RUNTIME_MODEL) {
        modelClassId = *(int *)(*(int *)ownerNode->runtimePayload + 0x4c);
        hit = DepthBinMasks_OverlapCf
                          (firstMaskLow,firstMaskHigh,ownerNode->modelDepthBinMaskFar,
                           ownerNode->modelDepthBinMaskNear);
        if (((hit) &&
            ((((placementFilterFlags & 4) == 0 ||
              (*(int *)(&g_ArmyRuntimeDepthBinClassByModelClass + modelClassId * 4) == 0x90)) &&
             (modelClassId != 0)))) && (modelClassId != 0xc)) {
          hit = ArmyCollision_TestPointWithinExpandedRuntimeRadiusCf
                            (queryRadiusQ12,worldXQ12,worldYQ12,ownerNode->runtimePayload);
          if (hit) {
            return true;
          }
          if ((modelClassId == 0xd) &&
             (hit = ArmyPlacementCandidate_TestModelAnchorDistanceCf
                                (queryRadiusQ12,worldXQ12,worldYQ12,ownerNode->runtimePayload),
             hit)) {
            return true;
          }
        }
      }
      ownerNode = ownerNode->nextNode;
    } while (ownerNode != (WorldOwnerListNode100 *)0x0);
  }
  return false;
}


/* Address: 0x00529F30.
   Ownership: gameplay/army/placement.
   Purpose: Tests a candidate runtime or class handle against the active list using optional depth-bin overlap,
   exclusion relationships, expanded collision radius, and the class-0x0D model-anchor distance path. Typed
   parameters: p2 excludedWorldObject→WorldRuntimeNode *, p6 worldRuntime→WorldRuntimeContext *. Calling
   convention, complete VariableStorage serialization, function bytes, control flow, globals, locals, and
   executable data remain unchanged.
   Local calls: ArmyCollision_TestPointWithinExpandedRuntimeRadiusCf,
   ArmyPlacementCandidate_TestModelAnchorDistanceCf.
   Cross-module calls: DepthBinMasks_OverlapCf [graphics/render/primitives].
*/

bool __thandor_cf_preserve_eax_ecx_edx
ArmyPlacementCollision_TestCandidateAgainstRuntimeListCf
          (WorldOwnerListNode100 *excludedWorldObject,Q12 worldXQ12,Q12 worldYQ12,
          IMAGE_DOS_HEADER *candidateRuntimeOrRadiusQ12,WorldRuntimeContext *worldRuntime)

{
  ArmyRuntimeSlot *armyRuntime;
  uint32_t modelClassId;
  WorldOwnerListNode100 *candidateNode;
  char *queryRadiusQ12;
  bool hit;
  WorldOwnerListNode100 *ownerNode;
  
  if (candidateRuntimeOrRadiusQ12 < (IMAGE_DOS_HEADER *)0x400000) {
    queryRadiusQ12 = candidateRuntimeOrRadiusQ12->e_magic + 1;
    candidateNode = (WorldOwnerListNode100 *)0x0;
  }
  else {
    candidateNode = *(WorldOwnerListNode100 **)&candidateRuntimeOrRadiusQ12->e_cp;
    queryRadiusQ12 = *(char **)(*(int *)candidateRuntimeOrRadiusQ12 + 0xdc);
  }
  ownerNode = worldRuntime->ownerListHead;
  if (queryRadiusQ12 != (char *)0x0) {
    for (; ownerNode != (WorldOwnerListNode100 *)0x0; ownerNode = ownerNode->nextNode) {
      if (((((ownerNode->ownerClassId == WORLD_OWNER_RUNTIME_MODEL) &&
            ((((candidateRuntimeOrRadiusQ12 < &IMAGE_DOS_HEADER_00400000 ||
               (hit = DepthBinMasks_OverlapCf
                                  (*(DepthBinMask32 *)
                                    (*(int *)&candidateRuntimeOrRadiusQ12->e_cp + 0xb8),
                                   *(DepthBinMask32 *)
                                    (*(int *)&candidateRuntimeOrRadiusQ12->e_cp + 0xb4),
                                   ownerNode->modelDepthBinMaskFar,
                                   ownerNode->modelDepthBinMaskNear), hit)) &&
              (armyRuntime = ownerNode->runtimePayload, candidateNode != ownerNode)) &&
             (ownerNode != excludedWorldObject)))) &&
           ((candidateRuntimeOrRadiusQ12 < &IMAGE_DOS_HEADER_00400000 ||
            ((armyRuntime != *(ArmyRuntimeSlot **)(candidateRuntimeOrRadiusQ12[1].e_program + 0x30)
             && ((ArmyRuntimeSlot *)candidateRuntimeOrRadiusQ12 != armyRuntime->linkedArmyRuntime)))
            ))) && ((modelClassId = ((armyRuntime->modelRuntimeOrSavedOffset).modelRuntime)->
                             definitionValue9C_4C, modelClassId != 0 && (modelClassId != 0xc)))) &&
         ((hit = ArmyCollision_TestPointWithinExpandedRuntimeRadiusCf
                             ((Q12)queryRadiusQ12,worldXQ12,worldYQ12,armyRuntime), hit ||
          ((modelClassId == 0xd &&
           (hit = ArmyPlacementCandidate_TestModelAnchorDistanceCf
                              ((Q12)queryRadiusQ12,worldXQ12,worldYQ12,armyRuntime), hit)))))) {
        return true;
      }
    }
  }
  return false;
}


/* Address: 0x00527740.
   Ownership: gameplay/army/placement.
   Purpose: Tests the current runtime against terrain height, field occupancy, and nearby compatible active objects
   before accepting its placement state. Placement-validation partition slots 24-47 receive (worldRuntime,
   armyRuntime), with CF carrying acceptance.
   Local calls: ArmyPlacementCollision_TestCandidateAgainstRuntimeListCf.
   Cross-module calls: FieldGrid_TestWorldPointBlockedCf [world/terrain/grid].
*/

bool __thandor_cf_preserve_eax_ecx_edx
ArmyPlacementCollision_TestCurrentRuntimeCf
          (WorldRuntimeContext *worldRuntime,ModelRuntimePlacementValidationView200 *modelRuntime)

{
  ModelDefinitionRuntimeSemanticView280 *placementDefinition;
  uint32_t ownClearanceQ12;
  ArmyRuntimeSlot *ownerArmy;
  uint32_t neighborClearanceQ12;
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
  ownClearanceQ12 = placementDefinition->placementFlags1A8;
  heightRadiusOrDeltaY = ((rootNode->worldTransform).translation.z - placementDefinition->placementHeightOffsetQ12) -
          ((rootNode->modelPayload).modelResource)->placementHeightOffsetQ12;
  terrainTest = TerrainHeightBand_TestAroundWorldPoint;
  if (placementDefinition->placementContactKindIndex278 == 1) {
    terrainTest = TerrainAuxHeightThreshold_TestAroundWorldPoint;
  }
  heightCopyOrDeltaX = heightRadiusOrDeltaY;
  blocked = ArmyPlacementCollision_TestCandidateAgainstRuntimeListCf
                    ((WorldOwnerListNode100 *)rootNode,(rootNode->worldTransform).translation.y,
                     (rootNode->worldTransform).translation.x,(IMAGE_DOS_HEADER *)modelRuntime,
                     worldRuntime);
  /* The original rejects on the terrain test's CF (JC after the indirect call); the decompile
     dropped that result and re-tested the runtime-list flag. */
  if ((!blocked) &&
     (blocked = terrainTest(placementDefinition->placementRadiusOrClearanceDC,heightRadiusOrDeltaY,
                 (rootNode->worldTransform).translation.y,
                 (rootNode->worldTransform).translation.x,worldRuntime->fieldGrid),
     !blocked)) {
    rootNode = modelRuntime->rootModelNode;
    ownerArmy = modelRuntime->ownerArmyRuntime;
    if ((g_UiCommandRuntimeFlags & 4) != 0) {
      return false;
    }
    blocked = FieldGrid_TestWorldPointBlockedCf
                      (ownerArmy->factionIndex,(rootNode->worldTransform).translation.y,
                       (rootNode->worldTransform).translation.x,worldRuntime->fieldGrid);
    if (!blocked) {
      ownerNode = (ModelRuntimeNode *)worldRuntime->ownerListHead;
      if (ownerNode == (ModelRuntimeNode *)0x0) {
        return false;
      }
      do {
        if ((((ownerNode->ownerClassId == WORLD_OWNER_RUNTIME_MODEL) && (ownerNode != rootNode))
            && (neighborClearanceQ12 = ((((ownerNode->runtimePayload).armyRuntime)->modelRuntimeOrSavedOffset).
                        modelRuntime)->attachments140[2].reserved1C,
               ownerArmy->factionIndex ==
               (((ownerNode->runtimePayload).armyRuntime)->linkedEntityRuntime->common).ownership.
               ownerIndex)) && (neighborClearanceQ12 != 0)) {
          heightRadiusOrDeltaY = neighborClearanceQ12 + ownClearanceQ12;
          heightCopyOrDeltaX = (rootNode->worldTransform).translation.x -
                  (ownerNode->worldTransform).translation.x;
          remainingSquared = (int64_t)heightRadiusOrDeltaY * (int64_t)heightRadiusOrDeltaY - (int64_t)heightCopyOrDeltaX * (int64_t)heightCopyOrDeltaX;
          if ((-1 < remainingSquared) &&
             (heightRadiusOrDeltaY = (rootNode->worldTransform).translation.y -
                      (ownerNode->worldTransform).translation.y,
             deltaYSquared = (int64_t)heightRadiusOrDeltaY * (int64_t)heightRadiusOrDeltaY,
             -1 < (int)(((int)((uint64_t)remainingSquared >> 0x20) - (int)((uint64_t)deltaYSquared >> 0x20)) -
                       (uint32_t)((uint32_t)remainingSquared < (uint32_t)deltaYSquared)))) {
            return false;
          }
        }
        ownerNode = (ModelRuntimeNode *)(ownerNode->common).nextNode;
      } while (ownerNode != (ModelRuntimeNode *)0x0);
    }
  }
  return true;
}


/* Address: 0x005278D0.
   Ownership: gameplay/army/placement.
   Purpose: Tests a candidate point against model collision, terrain constraints, field occupancy, and neighboring
   compatible objects, optionally returning the nearest remaining clearance. Twenty-four-entry army placement
   asset-class dispatch contract; CF carries acceptance and the first four class-specific dwords remain
   deliberately generic. Typed parameters: p0 dispatchArg0→ArmyPlacementDispatchArg0_V344, p1
   dispatchArg1→ArmyPlacementDispatchArg1_V344, p3 dispatchArg3→ArmyPlacementDispatchArg3_V344, p7
   dispatchArg7→ArmyPlacementDispatchArg7_V344. Calling convention, complete VariableStorage serialization,
   function bytes, control flow, globals, locals, and executable data remain unchanged.
   Local calls: ArmyPlacementCollision_TestPointAgainstRuntimeListCf.
   Cross-module calls: FieldGrid_TestWorldPointBlockedCf [world/terrain/grid], FixedMath_UInt64Sqrt
   [core/math/fixed].
*/

PlacementCandidateResult
ArmyPlacementCollision_TestCandidateAndClearanceCf
          (ArmyPlacementDispatchArg0 dispatchArg0,
          ArmyPlacementClearancePaddingQ12 placementClearancePaddingQ12,uint32_t dispatchArg2,
          ArmyPlacementDispatchArg3 dispatchArg3,Q12 worldXQ12,Q12 worldYQ12,
          ModelDefinitionRuntimeSemanticView280 *modelDefinition,
          ArmyPlacementDispatchArg7 dispatchArg7,WorldRuntimeContext *worldRuntime)

{
  ArmyPlacementContactKindIndex32 contactKindIndex;
  int64_t radiusSquaredOrDelta;
  int64_t remainingSquared;
  int64_t deltaYSquared;
  int64_t nearestDistanceSquared;
  int recordOrDistanceTerm;
  uint32_t nearestDistanceQ12;
  WorldOwnerListNode100 *ownerNode;
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
  
  nearestDistanceSquared = 0x7fffffffffffffff;
  contactKindIndex = modelDefinition->placementContactKindIndex278;
  blocked = ArmyPlacementCollision_TestPointAgainstRuntimeListCf
                    (dispatchArg0,modelDefinition->placementRadiusOrClearanceDC,worldXQ12,worldYQ12,
                     worldRuntime);
  if (!blocked) {
    terrainTest.value = 0;
    terrainTest.rejected = g_TerrainClassPlacementAndOverlayCallbacks10.placementTests[contactKindIndex]
                      (modelDefinition->placementRadiusOrClearanceDC,dispatchArg3,worldXQ12,
                       worldYQ12,worldRuntime->fieldGrid);
    eaxContinuity = terrainTest.value;
    if (!terrainTest.rejected) {
      if ((g_UiCommandRuntimeFlags & 4) == 0) {
        ownerNode = worldRuntime->ownerListHead;
        blocked = FieldGrid_TestWorldPointBlockedCf
                          (dispatchArg7,worldXQ12,worldYQ12,worldRuntime->fieldGrid);
        if (blocked) goto ArmyPlacementCollision_TestCandidateAndClearance_Reject;
        if (ownerNode != (WorldOwnerListNode100 *)0x0) {
          do {
            if (ownerNode->ownerClassId == WORLD_OWNER_RUNTIME_MODEL) {
              recordOrDistanceTerm = *(int *)ownerNode->runtimePayload;
              eaxContinuity = *(uint32_t *)(recordOrDistanceTerm + 0x19c);
              if (((eaxContinuity != 0) &&
                  (*(ArmyPlacementDispatchArg7 *)
                    (*(int *)((int)ownerNode->runtimePayload + 8) + 0xc) == dispatchArg7)) &&
                 ((eaxContinuity = eaxContinuity + placementClearancePaddingQ12,
                  *(int *)(recordOrDistanceTerm + 0x4c) != 0x12 ||
                  (((dispatchArg0 & 1) == 0 &&
                   ((*(uint32_t *)((int)ownerNode->runtimePayload + 0xec) & 0x18) == 0)))))) {
                recordOrDistanceTerm = eaxContinuity + modelDefinition->placementFlags1A8;
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
                    if ((dispatchArg0 & 2) == 0) goto ArmyPlacementCollision_TestCandidateAndClearance_Accept;
                    radiusSquaredOrDelta = (radiusSquaredOrDelta - remainingSquared) - nearestDistanceSquared;
                    eaxContinuity = (int)radiusSquaredOrDelta;
                    if (radiusSquaredOrDelta < 0) {
                      nearestClearanceQ12 = *(int *)(*(int *)ownerNode->runtimePayload + 0x19c);
                      nearestDistanceSquared = radiusSquaredOrDelta + nearestDistanceSquared;
                    }
                  }
                }
              }
            }
            nearestDistanceHigh = (UInt64Half32)((uint64_t)nearestDistanceSquared >> 0x20);
            nearestDistanceLow = (UInt64Half32)nearestDistanceSquared;
            ownerNode = ownerNode->nextNode;
          } while (ownerNode != (WorldOwnerListNode100 *)0x0);
          if (((dispatchArg0 & 2) == 0) || (0x7ffffffeffffffff < nearestDistanceSquared))
          goto ArmyPlacementCollision_TestCandidateAndClearance_Reject;
          nearestDistanceQ12 = FixedMath_UInt64Sqrt(nearestDistanceHigh,nearestDistanceLow);
          recordOrDistanceTerm = nearestDistanceQ12 - nearestClearanceQ12;
          if (recordOrDistanceTerm < 0) {
            recordOrDistanceTerm = 0;
          }
          eaxContinuity = recordOrDistanceTerm - modelDefinition->placementFlags1A8;
          if (eaxContinuity < 0) {
            eaxContinuity = 0;
          }
        }
      }
ArmyPlacementCollision_TestCandidateAndClearance_Accept:
      accepted.rejected = false;
      accepted.value = eaxContinuity;
      return accepted;
    }
  }
ArmyPlacementCollision_TestCandidateAndClearance_Reject:
  rejected.rejected = true;
  rejected.value = eaxContinuity;
  return rejected;
}


/* Address: 0x00524650.
   Ownership: gameplay/army/placement.
   Purpose: It is separate from world-unit radii, angles, grid indices, and serialized PCK identities.
   Cross-module calls: ModelLookupTable_ContainsPackedKeyCf [assets/model/definitions],
   ModelNodeRuntime_TransformLocalPointRegs [world/model/hierarchy], FixedMath_Length2 [core/math/fixed].
*/
bool __thandor_cf_preserve_eax_ecx_edx
ArmyPlacementCandidate_TestModelAnchorDistanceCf
          (Q12 queryRadiusQ12,Q12 targetWorldXQ12,Q12 targetWorldYQ12,ArmyRuntimeSlot *armyRuntime)

{
  ModelRuntimeNode *modelNodeRuntime;
  uint32_t anchorDistanceQ12;
  ModelLookupEntryResult anchorLookup;
  ModelWorldPoint anchorWorldPoint;
  
  modelNodeRuntime = armyRuntime->modelNodeRuntime;
  anchorLookup = ModelLookupTable_ContainsPackedKeyCf(1,5,(modelNodeRuntime->modelPayload).modelResource);
  if (!anchorLookup.notFound) {
    anchorWorldPoint = ModelNodeRuntime_TransformLocalPointRegs(anchorLookup.entry,modelNodeRuntime);
    anchorDistanceQ12 = FixedMath_Length2(anchorWorldPoint.yQ12 - targetWorldXQ12,anchorWorldPoint.xQ12 - targetWorldYQ12);
    if ((int)anchorDistanceQ12 <= queryRadiusQ12 + 0xc00) {
      g_ArmyPlacementAcceptedCandidateCount = g_ArmyPlacementAcceptedCandidateCount + 1;
      return true;
    }
  }
  return false;
}


/* Address: 0x00529C40.
   Ownership: gameplay/army/placement.
   Purpose: The carry flag preserves the acceptance result. It is separate from world-unit radii, angles, grid
   indices, and serialized PCK identities.
*/
bool __thandor_cf_preserve_eax_ecx_edx
ArmyCollision_TestPointWithinExpandedRuntimeRadiusCf
          (Q12 queryRadiusQ12,Q12 worldXQ12,Q12 worldYQ12,ArmyRuntimeSlot *armyRuntime)

{
  int64_t radiusSquared;
  int64_t distanceSquared;
  int deltaXOrRadiusQ12;
  int deltaYQ12;
  
  if ((((((armyRuntime->modelRuntimeOrSavedOffset).modelRuntime)->classState).classStateDC != 0) &&
      (queryRadiusQ12 != 0)) &&
     (deltaXOrRadiusQ12 = (armyRuntime->modelNodeRuntime->worldTransform).translation.x - worldYQ12,
     deltaYQ12 = (armyRuntime->modelNodeRuntime->worldTransform).translation.y - worldXQ12,
     distanceSquared = (int64_t)deltaYQ12 * (int64_t)deltaYQ12 + (int64_t)deltaXOrRadiusQ12 * (int64_t)deltaXOrRadiusQ12,
     deltaXOrRadiusQ12 = (((armyRuntime->modelRuntimeOrSavedOffset).modelRuntime)->classState).classStateDC +
             queryRadiusQ12, radiusSquared = (int64_t)deltaXOrRadiusQ12 * (int64_t)deltaXOrRadiusQ12,
     -1 < (int)(((int)((uint64_t)radiusSquared >> 0x20) - (int)((uint64_t)distanceSquared >> 0x20)) -
               (uint32_t)((uint32_t)radiusSquared < (uint32_t)distanceSquared)))) {
    return true;
  }
  return false;
}


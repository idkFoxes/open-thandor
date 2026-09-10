#include <thandor/gameplay/army/placement.h>

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
int ArmyPlacementCandidate_TestOffsetClearanceCf
              (ArmyPlacementDispatchArg0 dispatchArg0,
              ArmyPlacementClearancePaddingQ12 placementClearancePaddingQ12,
              ArmyPlacementDispatchArg2 dispatchArg2,ArmyPlacementDispatchArg3 dispatchArg3,
              Q12 worldXQ12,Q12 worldYQ12,ModelDefinitionRuntimeSemanticView280 *modelDefinition,
              ArmyPlacementDispatchArg7 dispatchArg7,WorldRuntimeContext *worldRuntime)

{
  int worldYQ12_00;
  int worldXQ12_00;
  int iVar1;
  int iVar2;
  uint uVar3;
  undefined1 in_CF;
  bool bVar4;
  FixedLengthAngleEdxEax8 FVar5;
  FixedSinCosEdxEax8 FVar6;
  
  iVar1 = ArmyPlacementCollision_TestCandidateAndClearanceCf
                    (dispatchArg0,placementClearancePaddingQ12,dispatchArg2,dispatchArg3,worldXQ12,
                     worldYQ12,modelDefinition,dispatchArg7,worldRuntime);
  iVar2 = iVar1;
  if (!(bool)in_CF) {
    iVar2 = ModelLookupTable_ContainsPackedKeyCf
                      (1,5,*(ModelResourceHitTestAndRenderView210 **)
                            (modelDefinition->serializedNodeOffsetOrPointer64 + 0x30));
    FVar5 = FixedMath_Vector2AngleAndLengthRegs
                      (*(FixedMathVectorComponent32 *)(iVar2 + 8),
                       *(FixedMathVectorComponent32 *)(iVar2 + 4));
    FVar6 = FixedMath_SinCosScaled
                      ((int)(FVar5 >> 0x20) + dispatchArg2 & 0xffff,(FixedMathScale32)FVar5);
    uVar3 = (uint)(FVar6 >> 0x20);
    worldYQ12_00 = worldYQ12 + (int)FVar6;
    bVar4 = CARRY4(worldXQ12,uVar3);
    worldXQ12_00 = worldXQ12 + uVar3;
    iVar2 = ArmyPlacementCollision_TestPointAgainstRuntimeListCf
                      (dispatchArg0,0xc00,worldXQ12_00,worldYQ12_00,worldRuntime);
    if (!bVar4) {
      iVar2 = (*g_TerrainClassPlacementAndOverlayCallbacks10.placementTests
                [modelDefinition->placementCallbackSelector278])
                        (0xc00,dispatchArg3,worldXQ12_00,worldYQ12_00,worldRuntime->fieldGrid);
      if (!bVar4) {
        return iVar1;
      }
    }
    g_ArmyPlacementAcceptedCandidateCount = g_ArmyPlacementAcceptedCandidateCount + 1;
  }
  return iVar2;
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
void ArmyPlacement_TestModelTerrainAndRuntimeClearance
               (WorldRuntimeContext *worldRuntime,ArmyRuntimeSlot *armyRuntime)

{
  ModelRuntimeNode *modelNodeRuntime;
  void *pvVar1;
  int iVar2;
  ModelPackedPointRecord *localPointRecord;
  Q12 worldYQ12;
  undefined4 extraout_ECX;
  Q12 worldXQ12;
  Q12 worldXQ12_00;
  Q12 referenceHeightQ12;
  undefined1 in_CF;
  bool bVar3;
  undefined8 uVar4;
  
  modelNodeRuntime = armyRuntime->modelNodeRuntime;
  ArmyPlacementCollision_TestCurrentRuntimeCf(worldRuntime,armyRuntime);
  if (!(bool)in_CF) {
    pvVar1 = armyRuntime->definitionOrAsset;
    localPointRecord =
         (ModelPackedPointRecord *)
         ModelLookupTable_ContainsPackedKeyCf(1,5,(modelNodeRuntime->modelPayload).modelResource);
    if (!(bool)in_CF) {
      uVar4 = ModelNodeRuntime_TransformLocalPointRegs
                        (extraout_ECX,localPointRecord,modelNodeRuntime);
      bVar3 = (uint)((int)((ulonglong)uVar4 >> 0x20) - *(int *)((int)pvVar1 + 0x54)) <
              (uint)((modelNodeRuntime->modelPayload).modelResource)->placementHeightOffsetQ12;
      ArmyPlacementCollision_TestCandidateAgainstRuntimeListCf
                ((WorldRuntimeNode *)modelNodeRuntime,worldXQ12,(Q12)uVar4,(IMAGE_DOS_HEADER *)0xc00
                 ,worldRuntime);
      if (!bVar3) {
        iVar2 = *(int *)((int)armyRuntime->definitionOrAsset + 0x278);
        bVar3 = iVar2 == 0;
        if (iVar2 == 1) {
          TerrainAuxHeightThreshold_TestAroundWorldPoint
                    (0xc00,referenceHeightQ12,worldXQ12_00,worldYQ12,worldRuntime->fieldGrid);
          if (!bVar3) {
            return;
          }
        }
        else {
          TerrainHeightBand_TestAroundWorldPoint
                    (0xc00,referenceHeightQ12,worldXQ12_00,worldYQ12,worldRuntime->fieldGrid);
          if (!bVar3) {
            return;
          }
        }
      }
    }
    g_ArmyPlacementAcceptedCandidateCount = g_ArmyPlacementAcceptedCandidateCount + 1;
  }
  return;
}

/* Address: 0x0051D380.
   Ownership: gameplay/army/placement.
   Purpose: Validates one army asset placement at the requested point and aligned cell corners; CF carries
   acceptance. Typed parameters: p0 placementMode→ArmyPlacementMode_V344, p4
   placementContext→ArmyPlacementContext_V344. Calling convention, complete VariableStorage serialization, function
   bytes, control flow, globals, locals, and executable data remain unchanged.
   Local calls: ArmyPlacement_DispatchAssetAtFieldPoint.
*/
void ArmyPlacement_ValidateAssetAtPointAndCellCornersCf
               (ArmyPlacementMode placementMode,PckArmyAssetIdCatalog armyAssetId,Q12 worldYQ12,
               Q12 worldXQ12,ArmyPlacementContext placementContext,
               FactionRuntimeIndex ownerFactionId,void *inGameRuntime)

{
  uint extraout_ECX;
  uint extraout_ECX_00;
  Q12 worldXQ12_00;
  uint extraout_ECX_01;
  uint extraout_EDX;
  Q12 worldYQ12_00;
  uint extraout_EDX_00;
  Q12 worldYQ12_01;
  undefined1 in_CF;
  bool bVar1;
  
  ArmyPlacement_DispatchAssetAtFieldPoint
            (placementMode,0,armyAssetId,worldYQ12,worldXQ12,placementContext,ownerFactionId,
             inGameRuntime);
  if ((bool)in_CF) {
    bVar1 = false;
    ArmyPlacement_DispatchAssetAtFieldPoint
              (placementMode,0,armyAssetId,extraout_EDX & 0xffffff00,extraout_ECX & 0xffffff00,
               placementContext,ownerFactionId,inGameRuntime);
    if (bVar1) {
      bVar1 = 0xfffffdbf < extraout_ECX_00;
      ArmyPlacement_DispatchAssetAtFieldPoint
                (placementMode,0,armyAssetId,worldYQ12_00,extraout_ECX_00 + 0x240,placementContext,
                 ownerFactionId,inGameRuntime);
      if (bVar1) {
        bVar1 = 0xfffffdbf < extraout_EDX_00;
        ArmyPlacement_DispatchAssetAtFieldPoint
                  (placementMode,0,armyAssetId,extraout_EDX_00 + 0x240,worldXQ12_00,placementContext
                   ,ownerFactionId,inGameRuntime);
        if (bVar1) {
          bVar1 = extraout_ECX_01 < 0x240;
          ArmyPlacement_DispatchAssetAtFieldPoint
                    (placementMode,0,armyAssetId,worldYQ12_01,extraout_ECX_01 - 0x240,
                     placementContext,ownerFactionId,inGameRuntime);
          if (bVar1) {
            return;
          }
        }
      }
    }
  }
  return;
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
int ArmyPlacementCandidate_TestFieldOccupancyCf
              (ArmyPlacementDispatchArg0 dispatchArg0,
              ArmyPlacementClearancePaddingQ12 placementClearancePaddingQ12,
              ArmyPlacementDispatchArg2 dispatchArg2,ArmyPlacementDispatchArg3 dispatchArg3,
              Q12 worldYQ12,Q12 worldXQ12,ModelDefinitionRecordPrefix *modelDefinition,
              ArmyPlacementDispatchArg7 dispatchArg7,WorldRuntimeContext *worldRuntime)

{
  int iVar1;
  int iVar2;
  int iVar3;
  uint uVar4;
  int iVar5;
  undefined1 in_CF;
  FieldGridAsset *fieldGrid1;
  
  iVar1 = ArmyPlacementCollision_TestCandidateAndClearanceCf
                    (dispatchArg0,placementClearancePaddingQ12,dispatchArg2,dispatchArg3,worldYQ12,
                     worldXQ12,(ModelDefinitionRuntimeSemanticView280 *)modelDefinition,dispatchArg7
                     ,worldRuntime);
  iVar3 = iVar1;
  if (!(bool)in_CF) {
    fieldGrid1 = worldRuntime->fieldGrid;
    uVar4 = (int)((ulonglong)((longlong)worldYQ12 * -0x20c8cc) >> 0x20) << 0xb |
            (uint)((longlong)worldYQ12 * -0x20c8cc) >> 0x15;
    iVar2 = (int)((((int)((ulonglong)((longlong)worldXQ12 * 0x1c6e9c) >> 0x20) << 0xc |
                   (uint)((longlong)worldXQ12 * 0x1c6e9c) >> 0x14) - uVar4) + 0x800) >> 0xc;
    iVar3 = iVar2;
    if ((((-1 < iVar2) && (iVar5 = (int)(uVar4 * 2 + 0x800) >> 0xc, -1 < iVar5)) &&
        (iVar2 < (int)fieldGrid1->gridWidth)) && (iVar5 < (int)fieldGrid1->gridHeight)) {
      iVar3 = iVar1;
      if ((fieldGrid1->cells[fieldGrid1->gridWidth * iVar5 + iVar2].flagsAndMaterial &
          0x800 << ((byte)modelDefinition[0x10].byteSize & 0x1f)) != 0) {
        return iVar1;
      }
    }
    g_ArmyPlacementAcceptedCandidateCount = g_ArmyPlacementAcceptedCandidateCount + 1;
  }
  return iVar3;
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
void ArmyPlacement_TestGridOccupancyMask
               (WorldRuntimeContext *worldRuntime,ArmyRuntimeSlot *armyRuntime)

{
  int iVar1;
  int iVar2;
  undefined1 in_CF;
  qword qVar3;
  FieldGridAsset *fieldGrid1;
  ModelRuntimeNode *modelNode1;
  
  modelNode1 = armyRuntime->modelNodeRuntime;
  ArmyPlacementCollision_TestCurrentRuntimeCf(worldRuntime,armyRuntime);
  if (!(bool)in_CF) {
    qVar3 = FieldGrid_WorldToGridQ12
                      ((modelNode1->worldTransform).translation.y,
                       (modelNode1->worldTransform).translation.x);
    iVar1 = ((int)qVar3 >> 0xb) + 1 >> 1;
    iVar2 = (int)((longlong)qVar3 >> 0x2b) + 1 >> 1;
    fieldGrid1 = worldRuntime->fieldGrid;
    if ((0 < iVar1) && (0 < iVar2)) {
      if ((iVar1 + 1 < (int)fieldGrid1->gridWidth) &&
         ((iVar2 + 1 < (int)fieldGrid1->gridHeight &&
          ((fieldGrid1->cells[iVar2 * fieldGrid1->gridWidth + iVar1].flagsAndMaterial &
           0x800 << ((byte)*(undefined4 *)((int)armyRuntime->definitionOrAsset + 0xc0) & 0x1f)) != 0
          )))) {
        return;
      }
    }
    g_ArmyPlacementAcceptedCandidateCount = g_ArmyPlacementAcceptedCandidateCount + 1;
  }
  return;
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
void ArmyPlacement_TestGridRuntimeAndFieldBlocking
               (WorldRuntimeContext *worldRuntime,ArmyRuntimeSlot *armyRuntime)

{
  undefined1 in_CF;
  ModelRuntimeNode *modelNode;
  
  modelNode = armyRuntime->modelNodeRuntime;
  GridScratch_TestProjectedCellMaskBandsCf
            ((modelNode->worldTransform).translation.y,(modelNode->worldTransform).translation.x,
             (byte)*(undefined4 *)((int)armyRuntime->definitionOrAsset + 0x260),
             (byte)*(undefined4 *)((int)armyRuntime->definitionOrAsset + 0x264));
  if (!(bool)in_CF) {
    ArmyCollision_FindBlockingRuntimeForCurrentUnitCf
              ((modelNode->worldTransform).translation.y,(modelNode->worldTransform).translation.x,
               armyRuntime,worldRuntime);
    if ((!(bool)in_CF) && ((g_UiCommandRuntimeFlags & 4) == 0)) {
      FieldGrid_TestWorldPointBlockedCf
                ((armyRuntime->linkedEntityRuntime->common).ownership.ownerIndex,
                 (modelNode->worldTransform).translation.y,(modelNode->worldTransform).translation.x
                 ,worldRuntime->fieldGrid);
    }
  }
  return;
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
void ArmyRuntimeCollision_TestShotSpawnPointCf
               (dword dispatchArg0,dword dispatchArg1,dword dispatchArg2,dword dispatchArg3,
               Q12 worldXQ12,Q12 worldYQ12,ModelDefinitionRuntimeSemanticView280 *modelDefinition,
               ArmyPlacementDispatchArg7 dispatchArg7,WorldRuntimeContext *worldRuntime)

{
  undefined4 incomingEcxValue;
  undefined4 preservedEdxValue;
  undefined1 in_CF;
  
  GridScratch_TestProjectedCellMaskBandsCf
            (worldXQ12,worldYQ12,(byte)modelDefinition->gridClassification260,
             (byte)modelDefinition->gridClassification264);
  if (!(bool)in_CF) {
    ArmyCollision_TestPointAgainstRuntimeListCf
              (incomingEcxValue,preservedEdxValue,worldXQ12,worldYQ12,(byte *)modelDefinition,
               worldRuntime);
    if ((!(bool)in_CF) && ((g_UiCommandRuntimeFlags & 4) == 0)) {
      FieldGrid_TestWorldPointBlockedCf(dispatchArg7,worldXQ12,worldYQ12,worldRuntime->fieldGrid);
    }
  }
  return;
}

/* Address: 0x004BE7F0.
   Ownership: gameplay/army/placement.
   Purpose: Preserved EAX/ECX/EDX values are incidental caller state, not a normal return contract.
   Cross-module calls: FieldGrid_InterpolateTerrainHeight [world/terrain/grid].
*/
void ArmyPlacementContact_ApplyTerrainHeight
               (Q12 heightOffsetQ12,Q12 worldXQ12,Q12 worldYQ12,ModelRuntimeNode *modelNode,
               WorldRuntimeContext *worldRuntime)

{
  Q12 surfaceHeightQ12;
  bool bVar1;
  
  bVar1 = false;
  if (worldRuntime->fieldGrid != (FieldGridAsset *)0x0) {
    surfaceHeightQ12 =
         FieldGrid_InterpolateTerrainHeight(worldXQ12,worldYQ12,worldRuntime->fieldGrid);
    if (!bVar1) {
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
void ArmyPlacementContact_ApplyWaterSurfaceHeight
               (Q12 heightOffsetQ12,Q12 worldXQ12,Q12 worldYQ12,ModelRuntimeNode *modelNode,
               WorldRuntimeContext *worldRuntime)

{
  Q12 surfaceHeightQ12;
  bool bVar1;
  
  bVar1 = false;
  if (worldRuntime->fieldGrid != (FieldGridAsset *)0x0) {
    surfaceHeightQ12 =
         FieldGrid_InterpolateWaterSurfaceHeight(worldXQ12,worldYQ12,worldRuntime->fieldGrid);
    if (!bVar1) {
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
void ArmyPlacementContact_ApplyTerrainHeightAndNormal
               (Q12 heightOffsetQ12,Q12 worldXQ12,Q12 worldYQ12,ModelRuntimeNode *modelNode,
               WorldRuntimeContext *worldRuntime)

{
  int iVar1;
  uint uVar2;
  bool bVar3;
  qword qVar4;
  
  bVar3 = false;
  if (worldRuntime->fieldGrid != (FieldGridAsset *)0x0) {
    qVar4 = FieldGrid_InterpolateTerrainHeightAndNormal(worldXQ12,worldYQ12,worldRuntime->fieldGrid)
    ;
    uVar2 = (uint)(qVar4 >> 0x20);
    if (!bVar3) {
      iVar1 = ((modelNode->modelPayload).modelResource)->placementHeightOffsetQ12;
      (modelNode->modelPayload).worldRotationAngle0 = uVar2 & 0xffff;
      (modelNode->modelPayload).worldRotationAngle1 = (int)uVar2 >> 0x10;
      (modelNode->worldTransform).translation.z = (int)qVar4 + iVar1 + heightOffsetQ12;
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
void ArmyPlacementContact_ApplyTopSurfaceHeight
               (Q12 heightOffsetQ12,Q12 worldXQ12,Q12 worldYQ12,ModelRuntimeNode *modelNode,
               WorldRuntimeContext *worldRuntime)

{
  Q12 surfaceHeightQ12;
  bool bVar1;
  
  bVar1 = false;
  if (worldRuntime->fieldGrid != (FieldGridAsset *)0x0) {
    surfaceHeightQ12 =
         FieldGrid_InterpolateTopSurfaceHeight(worldXQ12,worldYQ12,worldRuntime->fieldGrid);
    if (!bVar1) {
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
void ArmyPlacementContact_InitializeArticulatedSuspension
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
void ArmyPlacement_ReleaseFactionCapacityAndClearGridReservation
               (ModelDefinitionRecordPrefix *modelDefinition,ModelRuntimeSlot *modelRuntime)

{
  byte *pbVar1;
  dword dVar2;
  InGameRuntimeRootImageC3E4 *pIVar3;
  int iVar4;
  int iVar5;
  qword qVar6;
  FieldGridAsset *fieldGrid1;
  FieldGridCell *gridCell1;
  
  dVar2 = modelDefinition[0x10].flags;
  iVar4 = ((modelRuntime->ownerArmyRuntimeOrSavedOffset).armyRuntime)->factionIndex * 0x740;
  iVar5 = iVar4 + 4;
  if (modelDefinition[0x10].byteSize != 0) {
    iVar5 = iVar4 + 0x14;
  }
  iVar4 = *(int *)(g_GameFactionRuntimeImage.records[0].reserved78_87 + iVar5 + -0x78);
  if ((((int)modelRuntime->definitionValue60_3C < 2) && (iVar4 != 0)) &&
     (((modelRuntime->classState).classStateEC & 0x20) == 0)) {
    *(int *)(iVar5 + 0x50f33c) =
         *(int *)(iVar5 + 0x50f33c) -
         (int)(((longlong)(int)dVar2 * (longlong)*(int *)(iVar5 + 0x50f33c)) / (longlong)iVar4);
  }
  pbVar1 = g_GameFactionRuntimeImage.records[0].reserved78_87 + iVar5 + -0x78;
  *(dword *)pbVar1 = *(int *)pbVar1 - dVar2;
  pIVar3 = g_InGameRuntimeRoot;
  qVar6 = FieldGrid_WorldToGridQ12
                    ((((modelRuntime->rootModelNodeOrSavedOffset).modelNode)->worldTransform).
                     translation.y,
                     (((modelRuntime->rootModelNodeOrSavedOffset).modelNode)->worldTransform).
                     translation.x);
  iVar4 = ((int)qVar6 >> 0xb) + 1 >> 1;
  iVar5 = (int)((longlong)qVar6 >> 0x2b) + 1 >> 1;
  fieldGrid1 = (pIVar3->worldRuntime0A30).fieldGrid;
  if (((0 < iVar4) && (0 < iVar5)) && (fieldGrid1 != (FieldGridAsset *)0x0)) {
    if ((iVar4 + 1 < (int)fieldGrid1->gridWidth) && (iVar5 + 1 < (int)fieldGrid1->gridHeight)) {
      iVar4 = iVar5 * fieldGrid1->gridWidth + iVar4;
      gridCell1 = fieldGrid1->cells + iVar4;
      gridCell1->runtime58_6F[0x14] = 0;
      gridCell1->runtime58_6F[0x15] = 0;
      gridCell1->runtime58_6F[0x16] = 0;
      gridCell1->runtime58_6F[0x17] = 0;
      fieldGrid1->cells[iVar4].runtime7C = 0;
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
void ArmyPlacement_ReleaseFactionCapacity
               (ModelDefinitionRecordPrefix *modelDefinition,ModelRuntimeSlot *modelRuntime)

{
  byte *pbVar1;
  dword dVar2;
  int iVar3;
  int iVar4;
  
  dVar2 = modelDefinition[0x10].flags;
  iVar3 = ((modelRuntime->ownerArmyRuntimeOrSavedOffset).armyRuntime)->factionIndex * 0x740;
  iVar4 = iVar3 + 4;
  if (modelDefinition[0x10].byteSize != 0) {
    iVar4 = iVar3 + 0x14;
  }
  iVar3 = *(int *)(g_GameFactionRuntimeImage.records[0].reserved78_87 + iVar4 + -0x78);
  if ((((int)modelRuntime->definitionValue60_3C < 2) && (iVar3 != 0)) &&
     (((modelRuntime->classState).classStateEC & 0x20) == 0)) {
    *(int *)(iVar4 + 0x50f33c) =
         *(int *)(iVar4 + 0x50f33c) -
         (int)(((longlong)(int)dVar2 * (longlong)*(int *)(iVar4 + 0x50f33c)) / (longlong)iVar3);
  }
  pbVar1 = g_GameFactionRuntimeImage.records[0].reserved78_87 + iVar4 + -0x78;
  *(dword *)pbVar1 = *(int *)pbVar1 - dVar2;
  return;
}

/* Address: 0x005263E0.
   Ownership: gameplay/army/placement.
   Purpose: Table membership PLACEMENT_VALIDATION[21]. Releases the matching class-state reservation bit and count
   for the runtime model-definition class. Model release partition slots 0-23 receive (modelDefinition,
   modelRuntime).
*/
void ArmyPlacement_ReleaseClassStateReservation
               (ModelDefinitionRecordPrefix *modelDefinition,ModelRuntimeSlot *modelRuntime)

{
  dword *pdVar1;
  sdword *psVar2;
  ModelRuntimeArmyLinkOrState4 *pMVar3;
  int iVar4;
  ModelRuntimeSlot *modelSlot1;
  uint uVar5;
  ModelRuntimeSlot *modelSlot2;
  
  modelSlot2 = (modelRuntime->classLinkState).modelLinkOrState60.modelRuntime;
  if (modelSlot2 != (ModelRuntimeSlot *)0x0) {
    iVar4 = 0xd;
    uVar5 = 1;
    modelSlot1 = modelSlot2;
    do {
      if ((((modelRuntime->ownerArmyRuntimeOrSavedOffset).armyRuntime)->armyAssetId ==
           (modelSlot1->classLinkState).classState78) &&
         (((modelSlot2->classState).classStateB4 & uVar5) != 0)) {
        pdVar1 = &(modelSlot2->classLinkState).classState70;
        *pdVar1 = *pdVar1 + 1;
        psVar2 = &(modelSlot2->classState).classStateB4;
        *psVar2 = *psVar2 & (uVar5 ^ 0xffffffff);
        if (((modelRuntime->classState).classStateEC & 0x20) != 0) {
          return;
        }
        (modelSlot1->classLinkState).classState78 = 0;
        pMVar3 = &(modelSlot2->classLinkState).armyLinkOrState6C;
        pMVar3->armyRuntime = (ArmyRuntimeSlot *)(pMVar3->classState - 1);
        pdVar1 = &(modelSlot2->classLinkState).classState70;
        *pdVar1 = *pdVar1 - 1;
        return;
      }
      modelSlot1 = (ModelRuntimeSlot *)&modelSlot1->rootModelNodeOrSavedOffset;
      uVar5 = uVar5 * 2;
      iVar4 = iVar4 + -1;
    } while (iVar4 != 0);
  }
  return;
}

/* Address: 0x00527BD0.
   Ownership: gameplay/army/placement.
   Purpose: Exact nine-argument default that returns CF clear with ret 0x24 while preserving EAX. Twenty-four-entry
   army placement asset-class dispatch contract; CF carries acceptance and the first four class-specific dwords
   remain deliberately generic.
*/
void ArmyPlacementAssetClassDispatch_AlwaysSuccessCf
               (dword dispatchArg0,dword dispatchArg1,dword dispatchArg2,dword dispatchArg3,
               Q12 worldXQ12,Q12 worldYQ12,ModelDefinitionRecordPrefix *modelDefinition,
               dword dispatchArg7,WorldRuntimeContext *worldRuntime)

{
  return;
}

/* Address: 0x00529CB0.
   Ownership: gameplay/army/placement.
   Purpose: Saved ids, relocated pointers, attachment selectors, and runtime class ids remain separate. It is
   separate from world-unit radii, angles, grid indices, and serialized PCK identities.
   Local calls: ArmyCollision_TestPointWithinExpandedRuntimeRadiusCf.
   Cross-module calls: DepthInterval_BuildBinMask [graphics/render/primitives], DepthBinMasks_OverlapCf
   [graphics/render/primitives].
*/
void __fastcall
ArmyCollision_TestPointAgainstRuntimeListCf
          (undefined4 incomingEcxValue,undefined4 preservedEdxValue,Q12 worldXQ12,Q12 worldYQ12,
          byte *modelDefinition,WorldRuntimeContext *worldRuntime)

{
  DepthBinMask32 extraout_ECX;
  DepthBinMask32 extraout_ECX_00;
  DepthBinMask32 extraout_ECX_01;
  DepthBinMask32 firstMaskHigh;
  Q12 queryRadiusQ12;
  Q12 queryRadiusQ12_00;
  WorldRuntimeNode *worldNode1;
  bool bVar1;
  DepthBinMaskEaxPreservedEdxCarrier64 DVar2;
  
  worldNode1 = worldRuntime->ownerListHead;
  if ((*(int *)(modelDefinition + 0xdc) != 0) && (worldNode1 != (WorldRuntimeNode *)0x0)) {
    DVar2 = DepthInterval_BuildBinMask(*(int *)(modelDefinition + 0xdc),worldYQ12);
    DVar2 = DepthInterval_BuildBinMask((DepthIntervalRadius32)(DVar2 >> 0x20),worldXQ12);
    firstMaskHigh = extraout_ECX;
    do {
      bVar1 = false;
      if (worldNode1[2].common.nextNode == (WorldRuntimeNode *)0x0) {
        DepthBinMasks_OverlapCf
                  ((DepthBinMask32)DVar2,firstMaskHigh,
                   worldNode1[2].classPayload.model.localTranslationXQ12,
                   worldNode1[2].classPayload.model.worldRotationAngle2);
        firstMaskHigh = extraout_ECX_00;
        if (bVar1) {
          ArmyCollision_TestPointWithinExpandedRuntimeRadiusCf
                    (queryRadiusQ12_00,worldXQ12,worldYQ12,worldNode1->runtimePayload);
          firstMaskHigh = extraout_ECX_01;
          if (bVar1) {
            return;
          }
        }
      }
      worldNode1 = (worldNode1->common).nextNode;
    } while (worldNode1 != (WorldRuntimeNode *)0x0);
  }
  return;
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
ArmyRuntimeSlot *
ArmyCollision_FindBlockingRuntimeForCurrentUnitCf
          (Q12 worldXQ12,Q12 worldYQ12,ArmyRuntimeSlot *currentArmyRuntime,
          WorldRuntimeContext *worldRuntime)

{
  ArmyRuntimeSlot *armyRuntime;
  ArmyRuntimeSlot *extraout_EAX;
  WorldRuntimeNode *extraout_ECX;
  Q12 queryRadiusQ12;
  bool bVar1;
  WorldRuntimeNode *worldNode1;
  
  worldNode1 = worldRuntime->ownerListHead;
  if (*(int *)((int)currentArmyRuntime->definitionOrAsset + 0xdc) != 0) {
    for (; worldNode1 != (WorldRuntimeNode *)0x0; worldNode1 = (worldNode1->common).nextNode) {
      bVar1 = false;
      if ((((worldNode1[2].common.nextNode == (WorldRuntimeNode *)0x0) &&
           (DepthBinMasks_OverlapCf
                      (currentArmyRuntime->modelNodeRuntime->depthBinMaskFar,
                       currentArmyRuntime->modelNodeRuntime->depthBinMaskNear,
                       worldNode1[2].classPayload.model.localTranslationXQ12,
                       worldNode1[2].classPayload.model.worldRotationAngle2), bVar1)) &&
          (armyRuntime = worldNode1->runtimePayload, extraout_ECX != worldNode1)) &&
         ((bVar1 = false, extraout_EAX == (ArmyRuntimeSlot *)0x0 ||
          ((armyRuntime != extraout_EAX->linkedArmyRuntime &&
           (bVar1 = extraout_EAX < armyRuntime->linkedArmyRuntime,
           extraout_EAX != armyRuntime->linkedArmyRuntime)))))) {
        ArmyCollision_TestPointWithinExpandedRuntimeRadiusCf
                  (queryRadiusQ12,worldXQ12,worldYQ12,armyRuntime);
        if (bVar1) {
          return armyRuntime;
        }
      }
    }
  }
  return (ArmyRuntimeSlot *)0x0;
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
void ArmyPlacement_DispatchAssetAtFieldPoint
               (ArmyPlacementMode placementMode,
               ArmyPlacementClearancePaddingQ12 placementClearancePaddingQ12,
               FactionRuntimeIndex ownerFactionIndex,Q12 worldYQ12,Q12 worldXQ12,
               PckArmyAssetIdCatalog armyAssetId,ArmyPlacementContext placementContext,
               UiRootNode *inGameRoot)

{
  ArmyAssetRecordPrefix *pAVar1;
  ModelDefinitionRecordPrefix *modelDefinition;
  undefined1 in_CF;
  Q12 QVar2;
  
  pAVar1 = ArmyAssetRegistry_FindByIdCf(armyAssetId);
  if (!(bool)in_CF) {
    modelDefinition =
         ModelDefinitionRegistry_FindByIdWithErrorCf
                   (*(PckModelDefinitionIdCatalog *)(pAVar1->rootNodeOffsetOrPointer + 0x20));
    if (!(bool)in_CF) {
      _QVar2 = (*g_FieldGridInterpolationCallbacks5.callbacks[modelDefinition[0x34].definitionId])
                         (worldYQ12,worldXQ12,(FieldGridAsset *)inGameRoot->previousRoot);
      (*g_ArmyRuntimeOrderHandlerMatrix11Columns24Classes.placementAssetClassDispatch
        [(int)((ulonglong)_QVar2 >> 0x20)])
                (placementMode,placementClearancePaddingQ12,ownerFactionIndex,(dword)_QVar2,
                 worldYQ12,worldXQ12,modelDefinition,placementContext,
                 (WorldRuntimeContext *)inGameRoot);
    }
  }
  return;
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
undefined8
ArmyPlacementCollision_TestPointAgainstRuntimeListCf
          (ArmyPlacementCollisionFilterFlags placementFilterFlags,Q12 queryRadiusQ12,Q12 worldXQ12,
          Q12 worldYQ12,WorldRuntimeContext *worldRuntime)

{
  undefined4 in_EAX;
  uint extraout_EAX;
  DepthBinMask32 extraout_ECX;
  DepthBinMask32 extraout_ECX_00;
  DepthBinMask32 extraout_ECX_01;
  DepthBinMask32 extraout_ECX_02;
  DepthBinMask32 firstMaskHigh;
  undefined4 in_EDX;
  Q12 queryRadiusQ12_00;
  Q12 queryRadiusQ12_01;
  WorldRuntimeNode *worldNode1;
  bool bVar1;
  DepthBinMaskEaxPreservedEdxCarrier64 DVar2;
  undefined8 uVar3;
  
  worldNode1 = worldRuntime->ownerListHead;
  if ((queryRadiusQ12 != 0) && (worldNode1 != (WorldRuntimeNode *)0x0)) {
    DVar2 = DepthInterval_BuildBinMask(queryRadiusQ12,worldYQ12);
    DVar2 = DepthInterval_BuildBinMask((DepthIntervalRadius32)(DVar2 >> 0x20),worldXQ12);
    firstMaskHigh = extraout_ECX;
    do {
      bVar1 = false;
      if (worldNode1[2].common.nextNode == (WorldRuntimeNode *)0x0) {
        DepthBinMasks_OverlapCf
                  ((DepthBinMask32)DVar2,firstMaskHigh,
                   worldNode1[2].classPayload.model.localTranslationXQ12,
                   worldNode1[2].classPayload.model.worldRotationAngle2);
        firstMaskHigh = extraout_ECX_00;
        if (((bVar1) &&
            ((((placementFilterFlags & 4) == 0 ||
              (*(int *)(&g_ArmyRuntimeDepthBinClassByModelClass + extraout_EAX * 4) == 0x90)) &&
             (extraout_EAX != 0)))) && (bVar1 = extraout_EAX < 0xc, extraout_EAX != 0xc)) {
          uVar3 = ArmyCollision_TestPointWithinExpandedRuntimeRadiusCf
                            (queryRadiusQ12_01,worldXQ12,worldYQ12,worldNode1->runtimePayload);
          if (bVar1) {
ArmyPlacementCollision_TestPointAgainstRuntimeListCf_ReturnCollisionWithCarrySet:
            return CONCAT44(in_EDX,in_EAX);
          }
          bVar1 = (uint)uVar3 < 0xd;
          firstMaskHigh = extraout_ECX_01;
          if ((uint)uVar3 == 0xd) {
            ArmyPlacementCandidate_TestModelAnchorDistanceCf
                      ((int)((ulonglong)uVar3 >> 0x20),worldXQ12,worldYQ12,
                       worldNode1->runtimePayload);
            firstMaskHigh = extraout_ECX_02;
            if (bVar1)
            goto ArmyPlacementCollision_TestPointAgainstRuntimeListCf_ReturnCollisionWithCarrySet;
          }
        }
      }
      worldNode1 = (worldNode1->common).nextNode;
    } while (worldNode1 != (WorldRuntimeNode *)0x0);
  }
  return CONCAT44(in_EDX,in_EAX);
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
void ArmyPlacementCollision_TestCandidateAgainstRuntimeListCf
               (WorldRuntimeNode *excludedWorldObject,Q12 worldXQ12,Q12 worldYQ12,
               IMAGE_DOS_HEADER *candidateRuntimeOrRadiusQ12,WorldRuntimeContext *worldRuntime)

{
  ArmyRuntimeSlot *armyRuntime;
  uint uVar1;
  ArmyRuntimeSlot *extraout_EAX;
  ArmyRuntimeSlot *armySlot1;
  WorldRuntimeNode *worldNode2;
  WorldRuntimeNode *extraout_ECX;
  WorldRuntimeNode *extraout_ECX_00;
  WorldRuntimeNode *extraout_ECX_01;
  char *extraout_EDX;
  char *extraout_EDX_00;
  char *queryRadiusQ12;
  bool bVar2;
  undefined8 uVar3;
  WorldRuntimeNode *worldNode1;
  
  if (candidateRuntimeOrRadiusQ12 < (IMAGE_DOS_HEADER *)0x400000) {
    queryRadiusQ12 = candidateRuntimeOrRadiusQ12->e_magic + 1;
    worldNode2 = (WorldRuntimeNode *)0x0;
  }
  else {
    worldNode2 = *(WorldRuntimeNode **)&candidateRuntimeOrRadiusQ12->e_cp;
    queryRadiusQ12 = *(char **)(*(int *)candidateRuntimeOrRadiusQ12 + 0xdc);
  }
  worldNode1 = worldRuntime->ownerListHead;
  if (queryRadiusQ12 != (char *)0x0) {
    for (; worldNode1 != (WorldRuntimeNode *)0x0; worldNode1 = (worldNode1->common).nextNode) {
      if ((((worldNode1[2].common.nextNode == (WorldRuntimeNode *)0x0) &&
           ((((bVar2 = candidateRuntimeOrRadiusQ12 < &IMAGE_DOS_HEADER_00400000,
              armySlot1 = (ArmyRuntimeSlot *)candidateRuntimeOrRadiusQ12, bVar2 ||
              (DepthBinMasks_OverlapCf
                         (*(DepthBinMask32 *)(*(int *)&candidateRuntimeOrRadiusQ12->e_cp + 0xb8),
                          *(DepthBinMask32 *)(*(int *)&candidateRuntimeOrRadiusQ12->e_cp + 0xb4),
                          worldNode1[2].classPayload.model.localTranslationXQ12,
                          worldNode1[2].classPayload.model.worldRotationAngle2),
              armySlot1 = extraout_EAX, worldNode2 = extraout_ECX, queryRadiusQ12 = extraout_EDX,
              bVar2)) && (armyRuntime = worldNode1->runtimePayload, worldNode2 != worldNode1)) &&
            (worldNode1 != excludedWorldObject)))) &&
          ((armySlot1 < &IMAGE_DOS_HEADER_00400000 ||
           ((armyRuntime != armySlot1->linkedArmyRuntime &&
            (armySlot1 != armyRuntime->linkedArmyRuntime)))))) &&
         ((uVar1 = *(uint *)((int)armyRuntime->definitionOrAsset + 0x4c), uVar1 != 0 &&
          (bVar2 = uVar1 < 0xc, uVar1 != 0xc)))) {
        uVar3 = ArmyCollision_TestPointWithinExpandedRuntimeRadiusCf
                          ((Q12)queryRadiusQ12,worldXQ12,worldYQ12,armyRuntime);
        queryRadiusQ12 = (char *)((ulonglong)uVar3 >> 0x20);
        if ((bVar2) ||
           ((bVar2 = (uint)uVar3 < 0xd, worldNode2 = extraout_ECX_00, (uint)uVar3 == 0xd &&
            (ArmyPlacementCandidate_TestModelAnchorDistanceCf
                       (queryRadiusQ12,worldXQ12,worldYQ12,armyRuntime),
            worldNode2 = extraout_ECX_01, queryRadiusQ12 = extraout_EDX_00, bVar2)))) {
          return;
        }
      }
    }
  }
  return;
}

/* Address: 0x00527740.
   Ownership: gameplay/army/placement.
   Purpose: Tests the current runtime against terrain height, field occupancy, and nearby compatible active objects
   before accepting its placement state. Placement-validation partition slots 24-47 receive (worldRuntime,
   armyRuntime), with CF carrying acceptance.
   Local calls: ArmyPlacementCollision_TestCandidateAgainstRuntimeListCf.
   Cross-module calls: FieldGrid_TestWorldPointBlockedCf [world/terrain/grid].
*/
void ArmyPlacementCollision_TestCurrentRuntimeCf
               (WorldRuntimeContext *worldRuntime,ArmyRuntimeSlot *armyRuntime)

{
  void *pvVar1;
  int iVar2;
  longlong lVar3;
  longlong lVar4;
  int extraout_EAX;
  int iVar5;
  int extraout_ECX;
  int iVar6;
  undefined4 extraout_EDX;
  ModelRuntimeNode *modelNode1;
  bool bVar7;
  code *pcVar8;
  ModelRuntimeNode *modelNode2;
  
  modelNode2 = armyRuntime->modelNodeRuntime;
  pvVar1 = armyRuntime->definitionOrAsset;
  iVar2 = *(int *)((int)pvVar1 + 0x1a8);
  iVar6 = ((modelNode2->worldTransform).translation.z - *(int *)((int)pvVar1 + 0x54)) -
          ((((WorldRuntimeNodePayload *)&modelNode2->modelPayload)->model).modelResource)->
          placementHeightOffsetQ12;
  pcVar8 = TerrainHeightBand_TestAroundWorldPoint;
  bVar7 = *(int *)((int)pvVar1 + 0x278) == 0;
  if (*(int *)((int)pvVar1 + 0x278) == 1) {
    pcVar8 = TerrainAuxHeightThreshold_TestAroundWorldPoint;
  }
  ArmyPlacementCollision_TestCandidateAgainstRuntimeListCf
            ((WorldRuntimeNode *)modelNode2,(modelNode2->worldTransform).translation.y,
             (modelNode2->worldTransform).translation.x,(IMAGE_DOS_HEADER *)armyRuntime,worldRuntime
            );
  if ((!bVar7) &&
     ((*pcVar8)(*(undefined4 *)(extraout_ECX + 0xdc),extraout_EDX,
                (modelNode2->worldTransform).translation.y,
                (modelNode2->worldTransform).translation.x,worldRuntime->fieldGrid,pcVar8,iVar6),
     !bVar7)) {
    modelNode2 = armyRuntime->modelNodeRuntime;
    bVar7 = false;
    if ((g_UiCommandRuntimeFlags & 4) != 0) {
      return;
    }
    FieldGrid_TestWorldPointBlockedCf
              ((armyRuntime->linkedEntityRuntime->common).ownership.ownerIndex,
               (modelNode2->worldTransform).translation.y,(modelNode2->worldTransform).translation.x
               ,worldRuntime->fieldGrid);
    if (!bVar7) {
      modelNode1 = (ModelRuntimeNode *)worldRuntime->ownerListHead;
      if (modelNode1 == (ModelRuntimeNode *)0x0) {
        return;
      }
      do {
        if ((((modelNode1->ownerClassId == MODEL_RUNTIME_CLASS_00) && (modelNode1 != modelNode2)) &&
            (iVar6 = *(int *)((int)((modelNode1->runtimePayload).armyRuntime)->definitionOrAsset +
                             0x19c),
            *(int *)(extraout_EAX + 0xc) ==
            (((modelNode1->runtimePayload).armyRuntime)->linkedEntityRuntime->common).ownership.
            ownerIndex)) && (iVar6 != 0)) {
          iVar6 = iVar6 + iVar2;
          iVar5 = (modelNode2->worldTransform).translation.x -
                  (modelNode1->worldTransform).translation.x;
          lVar4 = (longlong)iVar6 * (longlong)iVar6 - (longlong)iVar5 * (longlong)iVar5;
          if ((-1 < lVar4) &&
             (iVar6 = (modelNode2->worldTransform).translation.y -
                      (modelNode1->worldTransform).translation.y,
             lVar3 = (longlong)iVar6 * (longlong)iVar6,
             -1 < (int)(((int)((ulonglong)lVar4 >> 0x20) - (int)((ulonglong)lVar3 >> 0x20)) -
                       (uint)((uint)lVar4 < (uint)lVar3)))) {
            return;
          }
        }
        modelNode1 = (ModelRuntimeNode *)(modelNode1->common).nextNode;
      } while (modelNode1 != (ModelRuntimeNode *)0x0);
    }
  }
  return;
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
int ArmyPlacementCollision_TestCandidateAndClearanceCf
              (ArmyPlacementDispatchArg0 dispatchArg0,
              ArmyPlacementClearancePaddingQ12 placementClearancePaddingQ12,dword dispatchArg2,
              ArmyPlacementDispatchArg3 dispatchArg3,Q12 worldXQ12,Q12 worldYQ12,
              ModelDefinitionRuntimeSemanticView280 *modelDefinition,
              ArmyPlacementDispatchArg7 dispatchArg7,WorldRuntimeContext *worldRuntime)

{
  int iVar1;
  longlong lVar2;
  longlong lVar3;
  longlong lVar4;
  longlong lVar5;
  int iVar6;
  int extraout_EAX;
  dword dVar7;
  WorldRuntimeNode *worldNode1;
  bool bVar8;
  undefined8 uVar9;
  int local_18;
  UInt64Half32 local_c;
  UInt64Half32 local_8;
  
  bVar8 = &stack0xfffffffc < (undefined1 *)0x14;
  lVar5 = 0x7fffffffffffffff;
  uVar9 = ArmyPlacementCollision_TestPointAgainstRuntimeListCf
                    (dispatchArg0,modelDefinition->placementRadiusOrClearanceDC,worldXQ12,worldYQ12,
                     worldRuntime);
  iVar6 = (int)uVar9;
  if ((bVar8) ||
     (iVar6 = (*g_TerrainClassPlacementAndOverlayCallbacks10.placementTests
                [(int)((ulonglong)uVar9 >> 0x20)])
                        (modelDefinition->placementRadiusOrClearanceDC,dispatchArg3,worldXQ12,
                         worldYQ12,worldRuntime->fieldGrid), bVar8)) {
    return iVar6;
  }
  bVar8 = false;
  if ((g_UiCommandRuntimeFlags & 4) == 0) {
    worldNode1 = worldRuntime->ownerListHead;
    FieldGrid_TestWorldPointBlockedCf(dispatchArg7,worldXQ12,worldYQ12,worldRuntime->fieldGrid);
    if (bVar8) {
      return extraout_EAX;
    }
    iVar6 = extraout_EAX;
    if (worldNode1 != (WorldRuntimeNode *)0x0) {
      do {
        if (worldNode1[2].common.nextNode == (WorldRuntimeNode *)0x0) {
          iVar1 = *(int *)worldNode1->runtimePayload;
          iVar6 = *(int *)(iVar1 + 0x19c);
          if (((iVar6 != 0) &&
              (*(ArmyPlacementDispatchArg7 *)(*(int *)((int)worldNode1->runtimePayload + 8) + 0xc)
               == dispatchArg7)) &&
             ((iVar6 = iVar6 + placementClearancePaddingQ12, *(int *)(iVar1 + 0x4c) != 0x12 ||
              (((dispatchArg0 & 1) == 0 &&
               ((*(uint *)((int)worldNode1->runtimePayload + 0xec) & 0x18) == 0)))))) {
            iVar6 = iVar6 + modelDefinition->placementFlags1A8;
            lVar2 = (longlong)iVar6 * (longlong)iVar6;
            iVar6 = worldNode1[1].classPayload.model.meshGroupMask - worldYQ12;
            lVar3 = (longlong)iVar6 * (longlong)iVar6;
            iVar6 = (int)lVar3;
            lVar3 = lVar2 - lVar3;
            if (-1 < lVar3) {
              iVar6 = (int)worldNode1[1].runtimePayload - worldXQ12;
              lVar4 = (longlong)iVar6 * (longlong)iVar6;
              iVar6 = (int)lVar4;
              lVar3 = lVar3 - lVar4;
              if (-1 < lVar3) {
                if ((dispatchArg0 & 2) == 0) {
                  return iVar6;
                }
                lVar2 = (lVar2 - lVar3) - lVar5;
                iVar6 = (int)lVar2;
                if (lVar2 < 0) {
                  local_18 = *(int *)(*(int *)worldNode1->runtimePayload + 0x19c);
                  lVar5 = lVar2 + lVar5;
                }
              }
            }
          }
        }
        local_8 = (UInt64Half32)((ulonglong)lVar5 >> 0x20);
        local_c = (UInt64Half32)lVar5;
        worldNode1 = (worldNode1->common).nextNode;
      } while (worldNode1 != (WorldRuntimeNode *)0x0);
      if ((dispatchArg0 & 2) == 0) {
        return iVar6;
      }
      if (0x7ffffffeffffffff < lVar5) {
        return iVar6;
      }
      dVar7 = FixedMath_UInt64Sqrt(local_8,local_c);
      iVar6 = dVar7 - local_18;
      if (iVar6 < 0) {
        iVar6 = 0;
      }
      iVar6 = iVar6 - modelDefinition->placementFlags1A8;
      if (iVar6 < 0) {
        iVar6 = 0;
      }
    }
  }
  return iVar6;
}

/* Address: 0x00524650.
   Ownership: gameplay/army/placement.
   Purpose: It is separate from world-unit radii, angles, grid indices, and serialized PCK identities.
   Cross-module calls: ModelLookupTable_ContainsPackedKeyCf [assets/model/definitions],
   ModelNodeRuntime_TransformLocalPointRegs [world/model/hierarchy], FixedMath_Length2 [core/math/fixed].
*/
undefined8
ArmyPlacementCandidate_TestModelAnchorDistanceCf
          (undefined4 param_1,Q12 targetWorldXQ12,Q12 targetWorldYQ12,ArmyRuntimeSlot *armyRuntime)

{
  ModelRuntimeNode *modelNodeRuntime;
  undefined4 in_EAX;
  ModelPackedPointRecord *localPointRecord;
  dword dVar1;
  undefined4 extraout_ECX;
  int extraout_ECX_00;
  undefined4 in_EDX;
  int extraout_EDX;
  undefined1 in_CF;
  undefined8 uVar2;
  
  modelNodeRuntime = armyRuntime->modelNodeRuntime;
  localPointRecord =
       (ModelPackedPointRecord *)
       ModelLookupTable_ContainsPackedKeyCf(1,5,(modelNodeRuntime->modelPayload).modelResource);
  if (!(bool)in_CF) {
    uVar2 = ModelNodeRuntime_TransformLocalPointRegs(extraout_ECX,localPointRecord,modelNodeRuntime)
    ;
    dVar1 = FixedMath_Length2(extraout_ECX_00 - targetWorldXQ12,(int)uVar2 - targetWorldYQ12);
    if ((int)dVar1 <= extraout_EDX) {
      g_ArmyPlacementAcceptedCandidateCount = g_ArmyPlacementAcceptedCandidateCount + 1;
      return CONCAT44(in_EDX,in_EAX);
    }
  }
  return CONCAT44(in_EDX,in_EAX);
}

/* Address: 0x00529C40.
   Ownership: gameplay/army/placement.
   Purpose: The carry flag preserves the acceptance result. It is separate from world-unit radii, angles, grid
   indices, and serialized PCK identities.
*/
undefined8
ArmyCollision_TestPointWithinExpandedRuntimeRadiusCf
          (Q12 queryRadiusQ12,Q12 worldXQ12,Q12 worldYQ12,ArmyRuntimeSlot *armyRuntime)

{
  longlong lVar1;
  longlong lVar2;
  undefined4 in_EAX;
  int iVar3;
  int iVar4;
  undefined4 in_EDX;
  
  if (((*(int *)((int)armyRuntime->definitionOrAsset + 0xdc) != 0) && (queryRadiusQ12 != 0)) &&
     (iVar3 = (armyRuntime->modelNodeRuntime->worldTransform).translation.x - worldYQ12,
     iVar4 = (armyRuntime->modelNodeRuntime->worldTransform).translation.y - worldXQ12,
     lVar2 = (longlong)iVar4 * (longlong)iVar4 + (longlong)iVar3 * (longlong)iVar3,
     iVar3 = *(int *)((int)armyRuntime->definitionOrAsset + 0xdc) + queryRadiusQ12,
     lVar1 = (longlong)iVar3 * (longlong)iVar3,
     -1 < (int)(((int)((ulonglong)lVar1 >> 0x20) - (int)((ulonglong)lVar2 >> 0x20)) -
               (uint)((uint)lVar1 < (uint)lVar2)))) {
    return CONCAT44(in_EDX,in_EAX);
  }
  return CONCAT44(in_EDX,in_EAX);
}

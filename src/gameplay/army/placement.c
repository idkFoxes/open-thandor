/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/src/gameplay/army/placement.c
 * Reverse engineering by idkFoxes 2026
 */

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
  int worldXQ12_00;
  int in_EAX;
  int iVar1;
  bool bVar2;
  FixedLengthAngleEaxEdx8 FVar3;
  FixedSinCosEdxEax8 FVar4;
  ArmyPlacementCandidateEaxCf5 AVar5;
  ModelLookupEntryEaxCf5 MVar6;
  TerrainPlacementTestEaxCf5 TVar7;
  
  AVar5 = ArmyPlacementCollision_TestCandidateAndClearanceCf
                    (in_EAX,dispatchArg0,placementClearancePaddingQ12,dispatchArg2,dispatchArg3,
                     worldXQ12,worldYQ12,modelDefinition,dispatchArg7,worldRuntime);
  iVar1 = AVar5.eax;
  if (!AVar5.carry) {
    MVar6 = ModelLookupTable_ContainsPackedKeyCf
                      (1,5,*(ModelResourceHitTestAndRenderView210 **)
                            (modelDefinition->serializedNodeOffsetOrPointer64 + 0x30));
    FVar3 = FixedMath_Vector2AngleAndLengthRegs
                      (((MVar6.entry)->localPosition).y,((MVar6.entry)->localPosition).x);
    FVar4 = FixedMath_SinCosScaled(FVar3.angle + dispatchArg2 & 0xffff,FVar3.length);
    iVar1 = (int)FVar4;
    worldXQ12_00 = worldXQ12 + (int)(FVar4 >> 0x20);
    bVar2 = ArmyPlacementCollision_TestPointAgainstRuntimeListCf
                      (dispatchArg0,0xc00,worldXQ12_00,worldYQ12 + iVar1,worldRuntime);
    if (!bVar2) {
      TVar7 = (*g_TerrainClassPlacementAndOverlayCallbacks10.placementTests
                [modelDefinition->placementContactKindIndex278])
                        (0xc00,dispatchArg3,worldXQ12_00,worldYQ12 + iVar1,worldRuntime->fieldGrid);
      iVar1 = TVar7.eax;
      if (!TVar7.carry) {
        return AVar5.eax;
      }
    }
    g_ArmyPlacementAcceptedCandidateCount = g_ArmyPlacementAcceptedCandidateCount + 1;
  }
  return iVar1;
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
  ModelDefinitionRuntimeSemanticView280 *pMVar1;
  dword worldYQ12;
  Q12 worldXQ12;
  int referenceHeightQ12;
  bool bVar2;
  ModelLookupEntryEaxCf5 MVar3;
  ModelLocalPointRegs12 MVar4;
  
  modelNodeRuntime = modelRuntime->rootModelNode;
  bVar2 = ArmyPlacementCollision_TestCurrentRuntimeCf(worldRuntime,modelRuntime);
  if (!bVar2) {
    pMVar1 = modelRuntime->modelDefinition;
    MVar3 = ModelLookupTable_ContainsPackedKeyCf(1,5,(modelNodeRuntime->modelPayload).modelResource)
    ;
    if (!MVar3.carry) {
      MVar4 = ModelNodeRuntime_TransformLocalPointRegs(MVar3.entry,modelNodeRuntime);
      worldXQ12 = MVar4.ecx;
      worldYQ12 = MVar4.eax;
      referenceHeightQ12 =
           (MVar4.edx - pMVar1->placementHeightOffsetQ12) -
           ((modelNodeRuntime->modelPayload).modelResource)->placementHeightOffsetQ12;
      bVar2 = ArmyPlacementCollision_TestCandidateAgainstRuntimeListCf
                        ((WorldOwnerListNode100 *)modelNodeRuntime,worldXQ12,worldYQ12,
                         (IMAGE_DOS_HEADER *)0xc00,worldRuntime);
      if (!bVar2) {
        if (modelRuntime->modelDefinition->placementContactKindIndex278 == 1) {
          bVar2 = TerrainAuxHeightThreshold_TestAroundWorldPoint
                            (0xc00,referenceHeightQ12,worldXQ12,worldYQ12,worldRuntime->fieldGrid);
          if (!bVar2) {
            return false;
          }
        }
        else {
          bVar2 = TerrainHeightBand_TestAroundWorldPoint
                            (0xc00,referenceHeightQ12,worldXQ12,worldYQ12,worldRuntime->fieldGrid);
          if (!bVar2) {
            return false;
          }
        }
      }
    }
    g_ArmyPlacementAcceptedCandidateCount = g_ArmyPlacementAcceptedCandidateCount + 1;
  }
  return true;
}


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
  uint worldXQ12_01;
  Q12 worldXQ12_00;
  uint worldYQ12_02;
  Q12 worldYQ12_00;
  Q12 worldYQ12_01;
  ArmyPlacementDispatchEaxCf5 AVar1;
  
  AVar1 = ArmyPlacement_DispatchAssetAtFieldPoint
                    (placementMode,0,armyAssetId,worldYQ12,worldXQ12,placementContext,ownerFactionId
                     ,inGameRuntime);
  if (AVar1.carry) {
    worldXQ12_01 = worldXQ12 & 0xffffff00;
    worldYQ12_02 = worldYQ12 & 0xffffff00;
    AVar1 = ArmyPlacement_DispatchAssetAtFieldPoint
                      (placementMode,0,armyAssetId,worldYQ12_02,worldXQ12_01,placementContext,
                       ownerFactionId,inGameRuntime);
    if (AVar1.carry) {
      AVar1 = ArmyPlacement_DispatchAssetAtFieldPoint
                        (placementMode,0,armyAssetId,worldYQ12_02,worldXQ12_01 + 0x240,
                         placementContext,ownerFactionId,inGameRuntime);
      if (AVar1.carry) {
        AVar1 = ArmyPlacement_DispatchAssetAtFieldPoint
                          (placementMode,0,armyAssetId,worldYQ12_02 + 0x240,worldXQ12_01 + 0x240,
                           placementContext,ownerFactionId,inGameRuntime);
        if (AVar1.carry) {
          AVar1 = ArmyPlacement_DispatchAssetAtFieldPoint
                            (placementMode,0,armyAssetId,worldYQ12_02 + 0x240,worldXQ12_01,
                             placementContext,ownerFactionId,inGameRuntime);
          if (AVar1.carry) {
            return true;
          }
        }
      }
    }
  }
  return false;
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
  int in_EAX;
  int iVar1;
  int iVar2;
  int iVar3;
  uint uVar4;
  int iVar5;
  ArmyPlacementCandidateEaxCf5 AVar6;
  FieldGridAsset *fieldGrid1;
  
  AVar6 = ArmyPlacementCollision_TestCandidateAndClearanceCf
                    (in_EAX,dispatchArg0,placementClearancePaddingQ12,dispatchArg2,dispatchArg3,
                     worldYQ12,worldXQ12,(ModelDefinitionRuntimeSemanticView280 *)modelDefinition,
                     dispatchArg7,worldRuntime);
  iVar1 = AVar6.eax;
  iVar3 = iVar1;
  if (!AVar6.carry) {
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
bool __thandor_cf_preserve_eax_ecx_edx
ArmyPlacement_TestGridOccupancyMask
          (WorldRuntimeContext *worldRuntime,ModelRuntimePlacementClass14View200 *modelRuntime)

{
  int iVar1;
  int iVar2;
  bool bVar3;
  FieldGridCoordinatesEaxEdx8 FVar4;
  FieldGridAsset *fieldGrid1;
  ModelRuntimeNode *modelNode1;
  
  modelNode1 = modelRuntime->rootModelNode;
  bVar3 = ArmyPlacementCollision_TestCurrentRuntimeCf
                    (worldRuntime,(ModelRuntimePlacementValidationView200 *)modelRuntime);
  if (!bVar3) {
    FVar4 = FieldGrid_WorldToGridQ12
                      ((modelNode1->worldTransform).translation.y,
                       (modelNode1->worldTransform).translation.x);
    iVar1 = (FVar4.columnQ12 >> 0xb) + 1 >> 1;
    iVar2 = (FVar4.rowQ12 >> 0xb) + 1 >> 1;
    fieldGrid1 = worldRuntime->fieldGrid;
    if ((0 < iVar1) && (0 < iVar2)) {
      if ((iVar1 + 1 < (int)fieldGrid1->gridWidth) &&
         ((iVar2 + 1 < (int)fieldGrid1->gridHeight &&
          ((fieldGrid1->cells[iVar2 * fieldGrid1->gridWidth + iVar1].flagsAndMaterial &
           0x800 << ((byte)modelRuntime->modelDefinition->resourceFieldSupportSelectorC0 & 0x1f)) !=
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
  bool bVar1;
  ArmyCollisionFindEaxCf5 AVar2;
  ModelRuntimeNode *modelNode;
  
  modelNode = modelRuntime->rootModelNode;
  bVar1 = GridScratch_TestProjectedCellMaskBandsCf
                    ((modelNode->worldTransform).translation.y,
                     (modelNode->worldTransform).translation.x,
                     (byte)modelRuntime->modelDefinition->gridClassification260,
                     (byte)modelRuntime->modelDefinition->gridClassification264);
  if (!bVar1) {
    AVar2 = ArmyCollision_FindBlockingRuntimeForCurrentUnitCf
                      ((modelNode->worldTransform).translation.y,
                       (modelNode->worldTransform).translation.x,
                       (RuntimeCollisionQueryViewF4 *)modelRuntime,worldRuntime);
    bVar1 = AVar2.carry;
    if ((!bVar1) && (bVar1 = false, (g_UiCommandRuntimeFlags & 4) == 0)) {
      bVar1 = FieldGrid_TestWorldPointBlockedCf
                        (modelRuntime->ownerArmyRuntime->factionIndex,
                         (modelNode->worldTransform).translation.y,
                         (modelNode->worldTransform).translation.x,worldRuntime->fieldGrid);
    }
  }
  return bVar1;
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
bool ArmyRuntimeCollision_TestShotSpawnPointCf
               (dword dispatchArg0,dword dispatchArg1,dword dispatchArg2,dword dispatchArg3,
               Q12 worldXQ12,Q12 worldYQ12,ModelDefinitionRuntimeSemanticView280 *modelDefinition,
               ArmyPlacementDispatchArg7 dispatchArg7,WorldRuntimeContext *worldRuntime)

{
  bool bVar1;
  
  bVar1 = GridScratch_TestProjectedCellMaskBandsCf
                    (worldXQ12,worldYQ12,(byte)modelDefinition->gridClassification260,
                     (byte)modelDefinition->gridClassification264);
  if (!bVar1) {
    bVar1 = ArmyCollision_TestPointAgainstRuntimeListCf
                      (worldXQ12,worldYQ12,(byte *)modelDefinition,worldRuntime);
    if ((!bVar1) && (bVar1 = false, (g_UiCommandRuntimeFlags & 4) == 0)) {
      bVar1 = FieldGrid_TestWorldPointBlockedCf
                        (dispatchArg7,worldXQ12,worldYQ12,worldRuntime->fieldGrid);
    }
  }
  return bVar1;
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
  FieldGridHeightEaxCf5 FVar1;
  
  if (worldRuntime->fieldGrid != (FieldGridAsset *)0x0) {
    FVar1 = FieldGrid_InterpolateTerrainHeight(worldXQ12,worldYQ12,worldRuntime->fieldGrid);
    surfaceHeightQ12 = FVar1.heightQ12;
    if (!FVar1.carry) {
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
  FieldGridHeightEaxCf5 FVar1;
  
  if (worldRuntime->fieldGrid != (FieldGridAsset *)0x0) {
    FVar1 = FieldGrid_InterpolateWaterSurfaceHeight(worldXQ12,worldYQ12,worldRuntime->fieldGrid);
    surfaceHeightQ12 = FVar1.heightQ12;
    if (!FVar1.carry) {
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
  int iVar1;
  FieldGridHeightNormalEaxEdxCf9 FVar2;
  
  if (worldRuntime->fieldGrid != (FieldGridAsset *)0x0) {
    FVar2 = FieldGrid_InterpolateTerrainHeightAndNormal(worldXQ12,worldYQ12,worldRuntime->fieldGrid)
    ;
    if (!FVar2.carry) {
      iVar1 = ((modelNode->modelPayload).modelResource)->placementHeightOffsetQ12;
      (modelNode->modelPayload).worldRotationAngle0 = FVar2.packedNormalAngles & 0xffff;
      (modelNode->modelPayload).worldRotationAngle1 = (int)FVar2.packedNormalAngles >> 0x10;
      (modelNode->worldTransform).translation.z = FVar2.heightQ12 + iVar1 + heightOffsetQ12;
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
  FieldGridHeightEaxCf5 FVar1;
  
  if (worldRuntime->fieldGrid != (FieldGridAsset *)0x0) {
    FVar1 = FieldGrid_InterpolateTopSurfaceHeight(worldXQ12,worldYQ12,worldRuntime->fieldGrid);
    surfaceHeightQ12 = FVar1.heightQ12;
    if (!FVar1.carry) {
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
  byte *pbVar1;
  dword dVar2;
  InGameRuntimeRootImageC3E4 *pIVar3;
  int iVar4;
  int iVar5;
  FieldGridCoordinatesEaxEdx8 FVar6;
  FieldGridAsset *fieldGrid1;
  
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
  FVar6 = FieldGrid_WorldToGridQ12
                    ((((modelRuntime->rootModelNodeOrSavedOffset).modelNode)->worldTransform).
                     translation.y,
                     (((modelRuntime->rootModelNodeOrSavedOffset).modelNode)->worldTransform).
                     translation.x);
  iVar4 = (FVar6.columnQ12 >> 0xb) + 1 >> 1;
  iVar5 = (FVar6.rowQ12 >> 0xb) + 1 >> 1;
  fieldGrid1 = (pIVar3->worldRuntime0A30).fieldGrid;
  if (((0 < iVar4) && (0 < iVar5)) && (fieldGrid1 != (FieldGridAsset *)0x0)) {
    if ((iVar4 + 1 < (int)fieldGrid1->gridWidth) && (iVar5 + 1 < (int)fieldGrid1->gridHeight)) {
      iVar4 = iVar5 * fieldGrid1->gridWidth + iVar4;
      fieldGrid1->cells[iVar4].armyRuntimeSavedOffset6C = 0;
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
void __thandor_void_preserve_eax_ecx_edx
ArmyPlacement_ReleaseFactionCapacity
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
void __thandor_void_preserve_eax_ecx_edx
ArmyPlacement_ReleaseClassStateReservation
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

bool __thandor_cf_preserve_eax_ecx_edx
ArmyCollision_TestPointAgainstRuntimeListCf
          (Q12 worldXQ12,Q12 worldYQ12,byte *modelDefinition,WorldRuntimeContext *worldRuntime)

{
  int intervalRadius;
  DepthBinMask32 firstMaskHigh;
  DepthBinMask32 firstMaskLow;
  Q12 queryRadiusQ12;
  WorldOwnerListNode100 *worldNode1;
  bool bVar1;
  
  intervalRadius = *(int *)(modelDefinition + 0xdc);
  worldNode1 = worldRuntime->ownerListHead;
  if ((intervalRadius != 0) && (worldNode1 != (WorldOwnerListNode100 *)0x0)) {
    firstMaskHigh = DepthInterval_BuildBinMask(intervalRadius,worldYQ12);
    firstMaskLow = DepthInterval_BuildBinMask(intervalRadius,worldXQ12);
    do {
      if (worldNode1->ownerClassId == WORLD_OWNER_RUNTIME_MODEL) {
        bVar1 = DepthBinMasks_OverlapCf
                          (firstMaskLow,firstMaskHigh,worldNode1->modelDepthBinMaskFar,
                           worldNode1->modelDepthBinMaskNear);
        if (bVar1) {
          bVar1 = ArmyCollision_TestPointWithinExpandedRuntimeRadiusCf
                            (intervalRadius,worldXQ12,worldYQ12,worldNode1->runtimePayload);
          if (bVar1) {
            return true;
          }
        }
      }
      worldNode1 = worldNode1->nextNode;
    } while (worldNode1 != (WorldOwnerListNode100 *)0x0);
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
ArmyCollisionFindEaxCf5 __thandor_eax_cf_preserve_ecx_edx
ArmyCollision_FindBlockingRuntimeForCurrentUnitCf
          (Q12 worldXQ12,Q12 worldYQ12,RuntimeCollisionQueryViewF4 *currentRuntime,
          WorldRuntimeContext *worldRuntime)

{
  ModelRuntimeNode *pMVar1;
  dword queryRadiusQ12_00;
  ArmyRuntimeSlot *armyRuntime;
  Q12 queryRadiusQ12;
  bool bVar2;
  ArmyCollisionFindEaxCf5 AVar3;
  ArmyCollisionFindEaxCf5 AVar4;
  ModelRuntimeNode *candidateModelNode;
  
  pMVar1 = currentRuntime->modelNodeRuntime;
  queryRadiusQ12_00 = currentRuntime->modelDefinition->placementRadiusOrClearanceDC;
  candidateModelNode = (ModelRuntimeNode *)worldRuntime->ownerListHead;
  if (queryRadiusQ12_00 != 0) {
    for (; candidateModelNode != (ModelRuntimeNode *)0x0;
        candidateModelNode = (ModelRuntimeNode *)(candidateModelNode->common).nextNode) {
      if (((((candidateModelNode->ownerClassId == MODEL_RUNTIME_CLASS_00) &&
            (bVar2 = DepthBinMasks_OverlapCf
                               (currentRuntime->modelNodeRuntime->depthBinMaskFar,
                                currentRuntime->modelNodeRuntime->depthBinMaskNear,
                                candidateModelNode->depthBinMaskFar,
                                candidateModelNode->depthBinMaskNear), bVar2)) &&
           (armyRuntime = (candidateModelNode->runtimePayload).armyRuntime,
           pMVar1 != candidateModelNode)) &&
          ((currentRuntime == (RuntimeCollisionQueryViewF4 *)0x0 ||
           ((armyRuntime != currentRuntime->linkedRuntimeF0 &&
            ((ArmyRuntimeSlot *)currentRuntime != armyRuntime->linkedArmyRuntime)))))) &&
         (bVar2 = ArmyCollision_TestPointWithinExpandedRuntimeRadiusCf
                            (queryRadiusQ12_00,worldXQ12,worldYQ12,armyRuntime), bVar2)) {
        AVar4.carry = true;
        AVar4.eax = (dword)armyRuntime;
        return AVar4;
      }
    }
  }
  AVar3.eax = 0;
  AVar3.carry = false;
  return AVar3;
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
ArmyPlacementDispatchEaxCf5 __thandor_eax_cf_preserve_ecx_edx
ArmyPlacement_DispatchAssetAtFieldPoint
          (ArmyPlacementMode placementMode,
          ArmyPlacementClearancePaddingQ12 placementClearancePaddingQ12,
          FactionRuntimeIndex ownerFactionIndex,Q12 worldYQ12,Q12 worldXQ12,
          PckArmyAssetIdCatalog armyAssetId,ArmyPlacementContext placementContext,
          UiRootNode *inGameRoot)

{
  dword dVar1;
  ArmyAssetRecordPrefix *modelDefinition;
  ArmyRegistryEaxCf5_51b6d0 AVar2;
  FieldGridHeightEaxCf5 FVar3;
  ArmyPlacementDispatchEaxCf5 AVar4;
  
  AVar2 = ArmyAssetRegistry_FindByIdCf(armyAssetId);
  if (!AVar2.carry) {
    AVar2 = (ArmyRegistryEaxCf5_51b6d0)
            ModelDefinitionRegistry_FindByIdWithErrorCf
                      (*(PckModelDefinitionIdCatalog *)((AVar2.eax)->rootNodeOffsetOrPointer + 0x20)
                      );
    modelDefinition = AVar2.eax;
    if (!AVar2.carry) {
      dVar1 = modelDefinition[4].rootNodeOffsetOrPointer;
      FVar3 = (*g_FieldGridInterpolationCallbacks5.callbacks[modelDefinition[0x27].registryId])
                        (worldYQ12,worldXQ12,(FieldGridAsset *)inGameRoot->previousRoot);
      AVar2 = (ArmyRegistryEaxCf5_51b6d0)
              (*g_ArmyRuntimeOrderHandlerMatrix11Columns24Classes.placementAssetClassDispatch[dVar1]
              )(placementMode,placementClearancePaddingQ12,ownerFactionIndex,FVar3.heightQ12,
                worldYQ12,worldXQ12,(ModelDefinitionRecordPrefix *)modelDefinition,placementContext,
                (WorldRuntimeContext *)inGameRoot);
    }
  }
  AVar4.eax = (dword)AVar2.eax;
  AVar4.carry = AVar2.carry;
  return AVar4;
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
  int iVar1;
  DepthBinMask32 firstMaskHigh;
  DepthBinMask32 firstMaskLow;
  Q12 queryRadiusQ12_00;
  WorldOwnerListNode100 *worldNode1;
  bool bVar2;
  
  worldNode1 = worldRuntime->ownerListHead;
  if ((queryRadiusQ12 != 0) && (worldNode1 != (WorldOwnerListNode100 *)0x0)) {
    firstMaskHigh = DepthInterval_BuildBinMask(queryRadiusQ12,worldYQ12);
    firstMaskLow = DepthInterval_BuildBinMask(queryRadiusQ12,worldXQ12);
    do {
      if (worldNode1->ownerClassId == WORLD_OWNER_RUNTIME_MODEL) {
        iVar1 = *(int *)(*(int *)worldNode1->runtimePayload + 0x4c);
        bVar2 = DepthBinMasks_OverlapCf
                          (firstMaskLow,firstMaskHigh,worldNode1->modelDepthBinMaskFar,
                           worldNode1->modelDepthBinMaskNear);
        if (((bVar2) &&
            ((((placementFilterFlags & 4) == 0 ||
              (*(int *)(&g_ArmyRuntimeDepthBinClassByModelClass + iVar1 * 4) == 0x90)) &&
             (iVar1 != 0)))) && (iVar1 != 0xc)) {
          bVar2 = ArmyCollision_TestPointWithinExpandedRuntimeRadiusCf
                            (queryRadiusQ12,worldXQ12,worldYQ12,worldNode1->runtimePayload);
          if (bVar2) {
            return true;
          }
          if ((iVar1 == 0xd) &&
             (bVar2 = ArmyPlacementCandidate_TestModelAnchorDistanceCf
                                (queryRadiusQ12,worldXQ12,worldYQ12,worldNode1->runtimePayload),
             bVar2)) {
            return true;
          }
        }
      }
      worldNode1 = worldNode1->nextNode;
    } while (worldNode1 != (WorldOwnerListNode100 *)0x0);
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
  dword dVar1;
  WorldOwnerListNode100 *worldNode2;
  char *queryRadiusQ12;
  bool bVar2;
  WorldOwnerListNode100 *worldNode1;
  
  if (candidateRuntimeOrRadiusQ12 < (IMAGE_DOS_HEADER *)0x400000) {
    queryRadiusQ12 = candidateRuntimeOrRadiusQ12->e_magic + 1;
    worldNode2 = (WorldOwnerListNode100 *)0x0;
  }
  else {
    worldNode2 = *(WorldOwnerListNode100 **)&candidateRuntimeOrRadiusQ12->e_cp;
    queryRadiusQ12 = *(char **)(*(int *)candidateRuntimeOrRadiusQ12 + 0xdc);
  }
  worldNode1 = worldRuntime->ownerListHead;
  if (queryRadiusQ12 != (char *)0x0) {
    for (; worldNode1 != (WorldOwnerListNode100 *)0x0; worldNode1 = worldNode1->nextNode) {
      if (((((worldNode1->ownerClassId == WORLD_OWNER_RUNTIME_MODEL) &&
            ((((candidateRuntimeOrRadiusQ12 < &IMAGE_DOS_HEADER_00400000 ||
               (bVar2 = DepthBinMasks_OverlapCf
                                  (*(DepthBinMask32 *)
                                    (*(int *)&candidateRuntimeOrRadiusQ12->e_cp + 0xb8),
                                   *(DepthBinMask32 *)
                                    (*(int *)&candidateRuntimeOrRadiusQ12->e_cp + 0xb4),
                                   worldNode1->modelDepthBinMaskFar,
                                   worldNode1->modelDepthBinMaskNear), bVar2)) &&
              (armyRuntime = worldNode1->runtimePayload, worldNode2 != worldNode1)) &&
             (worldNode1 != excludedWorldObject)))) &&
           ((candidateRuntimeOrRadiusQ12 < &IMAGE_DOS_HEADER_00400000 ||
            ((armyRuntime != *(ArmyRuntimeSlot **)(candidateRuntimeOrRadiusQ12[1].e_program + 0x30)
             && ((ArmyRuntimeSlot *)candidateRuntimeOrRadiusQ12 != armyRuntime->linkedArmyRuntime)))
            ))) && ((dVar1 = ((armyRuntime->modelRuntimeOrSavedOffset).modelRuntime)->
                             definitionValue9C_4C, dVar1 != 0 && (dVar1 != 0xc)))) &&
         ((bVar2 = ArmyCollision_TestPointWithinExpandedRuntimeRadiusCf
                             ((Q12)queryRadiusQ12,worldXQ12,worldYQ12,armyRuntime), bVar2 ||
          ((dVar1 == 0xd &&
           (bVar2 = ArmyPlacementCandidate_TestModelAnchorDistanceCf
                              ((Q12)queryRadiusQ12,worldXQ12,worldYQ12,armyRuntime), bVar2)))))) {
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
  ModelDefinitionRuntimeSemanticView280 *pMVar1;
  dword dVar2;
  ArmyRuntimeSlot *pAVar3;
  dword dVar4;
  longlong lVar5;
  longlong lVar6;
  int iVar7;
  int iVar8;
  ModelRuntimeNode *modelNode1;
  bool bVar9;
  code *pcVar10;
  ModelRuntimeNode *modelNode2;
  
  modelNode2 = modelRuntime->rootModelNode;
  pMVar1 = modelRuntime->modelDefinition;
  dVar2 = pMVar1->placementFlags1A8;
  iVar8 = ((modelNode2->worldTransform).translation.z - pMVar1->placementHeightOffsetQ12) -
          ((modelNode2->modelPayload).modelResource)->placementHeightOffsetQ12;
  pcVar10 = TerrainHeightBand_TestAroundWorldPoint;
  if (pMVar1->placementContactKindIndex278 == 1) {
    pcVar10 = TerrainAuxHeightThreshold_TestAroundWorldPoint;
  }
  iVar7 = iVar8;
  bVar9 = ArmyPlacementCollision_TestCandidateAgainstRuntimeListCf
                    ((WorldOwnerListNode100 *)modelNode2,(modelNode2->worldTransform).translation.y,
                     (modelNode2->worldTransform).translation.x,(IMAGE_DOS_HEADER *)modelRuntime,
                     worldRuntime);
  if ((!bVar9) &&
     ((*pcVar10)(pMVar1->placementRadiusOrClearanceDC,iVar8,
                 (modelNode2->worldTransform).translation.y,
                 (modelNode2->worldTransform).translation.x,worldRuntime->fieldGrid,pcVar10,iVar7),
     !bVar9)) {
    modelNode2 = modelRuntime->rootModelNode;
    pAVar3 = modelRuntime->ownerArmyRuntime;
    if ((g_UiCommandRuntimeFlags & 4) != 0) {
      return false;
    }
    bVar9 = FieldGrid_TestWorldPointBlockedCf
                      (pAVar3->factionIndex,(modelNode2->worldTransform).translation.y,
                       (modelNode2->worldTransform).translation.x,worldRuntime->fieldGrid);
    if (!bVar9) {
      modelNode1 = (ModelRuntimeNode *)worldRuntime->ownerListHead;
      if (modelNode1 == (ModelRuntimeNode *)0x0) {
        return false;
      }
      do {
        if ((((modelNode1->ownerClassId == WORLD_OWNER_RUNTIME_MODEL) && (modelNode1 != modelNode2))
            && (dVar4 = ((((modelNode1->runtimePayload).armyRuntime)->modelRuntimeOrSavedOffset).
                        modelRuntime)->attachments140[2].reserved1C,
               pAVar3->factionIndex ==
               (((modelNode1->runtimePayload).armyRuntime)->linkedEntityRuntime->common).ownership.
               ownerIndex)) && (dVar4 != 0)) {
          iVar8 = dVar4 + dVar2;
          iVar7 = (modelNode2->worldTransform).translation.x -
                  (modelNode1->worldTransform).translation.x;
          lVar6 = (longlong)iVar8 * (longlong)iVar8 - (longlong)iVar7 * (longlong)iVar7;
          if ((-1 < lVar6) &&
             (iVar8 = (modelNode2->worldTransform).translation.y -
                      (modelNode1->worldTransform).translation.y,
             lVar5 = (longlong)iVar8 * (longlong)iVar8,
             -1 < (int)(((int)((ulonglong)lVar6 >> 0x20) - (int)((ulonglong)lVar5 >> 0x20)) -
                       (uint)((uint)lVar6 < (uint)lVar5)))) {
            return false;
          }
        }
        modelNode1 = (ModelRuntimeNode *)(modelNode1->common).nextNode;
      } while (modelNode1 != (ModelRuntimeNode *)0x0);
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

ArmyPlacementCandidateEaxCf5
ArmyPlacementCollision_TestCandidateAndClearanceCf
          (int eaxContinuity,ArmyPlacementDispatchArg0 dispatchArg0,
          ArmyPlacementClearancePaddingQ12 placementClearancePaddingQ12,dword dispatchArg2,
          ArmyPlacementDispatchArg3 dispatchArg3,Q12 worldXQ12,Q12 worldYQ12,
          ModelDefinitionRuntimeSemanticView280 *modelDefinition,
          ArmyPlacementDispatchArg7 dispatchArg7,WorldRuntimeContext *worldRuntime)

{
  ArmyPlacementContactKindIndex32 AVar1;
  longlong lVar2;
  longlong lVar3;
  longlong lVar4;
  longlong lVar5;
  int iVar6;
  dword dVar7;
  WorldOwnerListNode100 *worldNode1;
  bool bVar8;
  TerrainPlacementTestEaxCf5 TVar9;
  ArmyPlacementCandidateEaxCf5 AVar10;
  ArmyPlacementCandidateEaxCf5 AVar11;
  int local_18;
  UInt64Half32 local_c;
  UInt64Half32 local_8;
  
  lVar5 = 0x7fffffffffffffff;
  AVar1 = modelDefinition->placementContactKindIndex278;
  bVar8 = ArmyPlacementCollision_TestPointAgainstRuntimeListCf
                    (dispatchArg0,modelDefinition->placementRadiusOrClearanceDC,worldXQ12,worldYQ12,
                     worldRuntime);
  if (!bVar8) {
    TVar9 = (*g_TerrainClassPlacementAndOverlayCallbacks10.placementTests[AVar1])
                      (modelDefinition->placementRadiusOrClearanceDC,dispatchArg3,worldXQ12,
                       worldYQ12,worldRuntime->fieldGrid);
    eaxContinuity = TVar9.eax;
    if (!TVar9.carry) {
      if ((g_UiCommandRuntimeFlags & 4) == 0) {
        worldNode1 = worldRuntime->ownerListHead;
        bVar8 = FieldGrid_TestWorldPointBlockedCf
                          (dispatchArg7,worldXQ12,worldYQ12,worldRuntime->fieldGrid);
        if (bVar8) goto LAB_00527ab0;
        if (worldNode1 != (WorldOwnerListNode100 *)0x0) {
          do {
            if (worldNode1->ownerClassId == WORLD_OWNER_RUNTIME_MODEL) {
              iVar6 = *(int *)worldNode1->runtimePayload;
              eaxContinuity = *(uint *)(iVar6 + 0x19c);
              if (((eaxContinuity != 0) &&
                  (*(ArmyPlacementDispatchArg7 *)
                    (*(int *)((int)worldNode1->runtimePayload + 8) + 0xc) == dispatchArg7)) &&
                 ((eaxContinuity = eaxContinuity + placementClearancePaddingQ12,
                  *(int *)(iVar6 + 0x4c) != 0x12 ||
                  (((dispatchArg0 & 1) == 0 &&
                   ((*(uint *)((int)worldNode1->runtimePayload + 0xec) & 0x18) == 0)))))) {
                iVar6 = eaxContinuity + modelDefinition->placementFlags1A8;
                lVar2 = (longlong)iVar6 * (longlong)iVar6;
                iVar6 = worldNode1->worldXQ12 - worldYQ12;
                lVar3 = (longlong)iVar6 * (longlong)iVar6;
                eaxContinuity = (int)lVar3;
                lVar3 = lVar2 - lVar3;
                if (-1 < lVar3) {
                  iVar6 = worldNode1->worldYQ12 - worldXQ12;
                  lVar4 = (longlong)iVar6 * (longlong)iVar6;
                  eaxContinuity = (int)lVar4;
                  lVar3 = lVar3 - lVar4;
                  if (-1 < lVar3) {
                    if ((dispatchArg0 & 2) == 0) goto LAB_00527aa0;
                    lVar2 = (lVar2 - lVar3) - lVar5;
                    eaxContinuity = (int)lVar2;
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
            worldNode1 = worldNode1->nextNode;
          } while (worldNode1 != (WorldOwnerListNode100 *)0x0);
          if (((dispatchArg0 & 2) == 0) || (0x7ffffffeffffffff < lVar5)) goto LAB_00527ab0;
          dVar7 = FixedMath_UInt64Sqrt(local_8,local_c);
          iVar6 = dVar7 - local_18;
          if (iVar6 < 0) {
            iVar6 = 0;
          }
          eaxContinuity = iVar6 - modelDefinition->placementFlags1A8;
          if (eaxContinuity < 0) {
            eaxContinuity = 0;
          }
        }
      }
LAB_00527aa0:
      AVar10.carry = false;
      AVar10.eax = eaxContinuity;
      return AVar10;
    }
  }
LAB_00527ab0:
  AVar11.carry = true;
  AVar11.eax = eaxContinuity;
  return AVar11;
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
  dword dVar1;
  ModelLookupEntryEaxCf5 MVar2;
  ModelLocalPointRegs12 MVar3;
  
  modelNodeRuntime = armyRuntime->modelNodeRuntime;
  MVar2 = ModelLookupTable_ContainsPackedKeyCf(1,5,(modelNodeRuntime->modelPayload).modelResource);
  if (!MVar2.carry) {
    MVar3 = ModelNodeRuntime_TransformLocalPointRegs(MVar2.entry,modelNodeRuntime);
    dVar1 = FixedMath_Length2(MVar3.ecx - targetWorldXQ12,MVar3.eax - targetWorldYQ12);
    if ((int)dVar1 <= queryRadiusQ12 + 0xc00) {
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
  longlong lVar1;
  longlong lVar2;
  int iVar3;
  int iVar4;
  
  if ((((((armyRuntime->modelRuntimeOrSavedOffset).modelRuntime)->classState).classStateDC != 0) &&
      (queryRadiusQ12 != 0)) &&
     (iVar3 = (armyRuntime->modelNodeRuntime->worldTransform).translation.x - worldYQ12,
     iVar4 = (armyRuntime->modelNodeRuntime->worldTransform).translation.y - worldXQ12,
     lVar2 = (longlong)iVar4 * (longlong)iVar4 + (longlong)iVar3 * (longlong)iVar3,
     iVar3 = (((armyRuntime->modelRuntimeOrSavedOffset).modelRuntime)->classState).classStateDC +
             queryRadiusQ12, lVar1 = (longlong)iVar3 * (longlong)iVar3,
     -1 < (int)(((int)((ulonglong)lVar1 >> 0x20) - (int)((ulonglong)lVar2 >> 0x20)) -
               (uint)((uint)lVar1 < (uint)lVar2)))) {
    return true;
  }
  return false;
}


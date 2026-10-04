/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/src/gameplay/army/placement_contact.cpp
 * Reverse engineering by idkFoxes 2026
 */

#include <thandor/gameplay/army/placement_contact.h>
#include <thandor/thandor.h>
#include <thandor/platform/bootstrap/image.h>

/* Implementation ownership: gameplay/army/placement_contact. */

/* Contact kind 0 (terrain): sets the model node onto the interpolated terrain height at the point, plus
   heightOffsetQ12 and the model resource's own height offset, stands it upright (angle 1 = quarter turn)
   and sets node flag 0x1. Nothing changes without a field grid or when the point is off the grid.
   Reached through g_ArmyPlacementContactKindDispatchTable.callbacks[0], indexed by the model
   definition's contact kind (placementContactKindIndex) from the movement, creation and session code.
*/
void ArmyPlacementContact_ApplyTerrainHeight
          (Q12 heightOffsetQ12,Q12 worldYQ12,Q12 worldXQ12,ModelRuntimeNode *modelNode,
          WorldRuntimeContext *worldRuntime)

{
  Q12 surfaceHeightQ12;
  
  if (worldRuntime->fieldGrid != nullptr) {
    if (FieldGrid_InterpolateTerrainHeight(worldYQ12,worldXQ12,worldRuntime->fieldGrid,&surfaceHeightQ12)) {
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

/* Contact kind 1 (water surface): sets the model node onto the interpolated water surface at the point
   plus heightOffsetQ12 (no resource offset), stands it upright (angle 1 = quarter turn) and sets node flag
   0x1. Nothing changes without a field grid or when the point is off the grid.
   Reached through g_ArmyPlacementContactKindDispatchTable.callbacks[1], indexed by the model
   definition's contact kind (placementContactKindIndex).
*/
void ArmyPlacementContact_ApplyWaterSurfaceHeight
          (Q12 heightOffsetQ12,Q12 worldYQ12,Q12 worldXQ12,ModelRuntimeNode *modelNode,
          WorldRuntimeContext *worldRuntime)

{
  Q12 surfaceHeightQ12;
  
  if (worldRuntime->fieldGrid != nullptr) {
    if (FieldGrid_InterpolateWaterSurfaceHeight(worldYQ12,worldXQ12,worldRuntime->fieldGrid,&surfaceHeightQ12)) {
      (modelNode->worldTransform).translation.z = surfaceHeightQ12 + heightOffsetQ12;
      (modelNode->worldTransform).translation.x = worldXQ12;
      (modelNode->worldTransform).translation.y = worldYQ12;
      (modelNode->modelPayload).worldRotationAngle1 = FIXED_ANGLE16_QUARTER_TURN;
      modelNode->runtimeFlags = modelNode->runtimeFlags | 1;
    }
  }
  return;
}

/* Contact kind 2 (terrain with slope): like kind 0, but tilts the model node to the terrain normal (the two
   packed 16-bit normal angles become rotation angles 0 and 1) instead of standing it upright.
   Reached through g_ArmyPlacementContactKindDispatchTable.callbacks[2], indexed by the model
   definition's contact kind (placementContactKindIndex).
*/
void ArmyPlacementContact_ApplyTerrainHeightAndNormal
          (Q12 heightOffsetQ12,Q12 worldYQ12,Q12 worldXQ12,ModelRuntimeNode *modelNode,
          WorldRuntimeContext *worldRuntime)

{
  int resourceHeightOffsetQ12;
  Q12 surfaceHeightQ12;
  uint32_t surfaceNormalAngles;

  if (worldRuntime->fieldGrid != nullptr) {
    if (FieldGrid_InterpolateTerrainHeightAndNormal
          (worldYQ12,worldXQ12,worldRuntime->fieldGrid,&surfaceHeightQ12,&surfaceNormalAngles)) {
      resourceHeightOffsetQ12 = ((modelNode->modelPayload).modelResource)->placementHeightOffsetQ12;
      (modelNode->modelPayload).worldRotationAngle0 = surfaceNormalAngles & FIXED_ANGLE16_MASK;
      (modelNode->modelPayload).worldRotationAngle1 = (int)surfaceNormalAngles >> 16;
      (modelNode->worldTransform).translation.z =
           surfaceHeightQ12 + resourceHeightOffsetQ12 + heightOffsetQ12;
      (modelNode->worldTransform).translation.x = worldXQ12;
      (modelNode->worldTransform).translation.y = worldYQ12;
      modelNode->runtimeFlags = modelNode->runtimeFlags | 1;
    }
  }
  return;
}

/* Contact kind 4 (top surface): sets the model node onto the top surface - the terrain, or the water above
   it (FieldGrid_InterpolateTopSurfaceHeight) - plus heightOffsetQ12, stands it upright (angle 1 = quarter turn)
   and sets node flag 0x1. Nothing changes without a field grid or when the point is off the grid.
   Reached through g_ArmyPlacementContactKindDispatchTable.callbacks[4], indexed by the model
   definition's contact kind (placementContactKindIndex).
*/
void ArmyPlacementContact_ApplyTopSurfaceHeight
          (Q12 heightOffsetQ12,Q12 worldYQ12,Q12 worldXQ12,ModelRuntimeNode *modelNode,
          WorldRuntimeContext *worldRuntime)

{
  Q12 surfaceHeightQ12;
  
  if (worldRuntime->fieldGrid != nullptr) {
    if (FieldGrid_InterpolateTopSurfaceHeight(worldYQ12,worldXQ12,worldRuntime->fieldGrid,&surfaceHeightQ12)) {
      (modelNode->worldTransform).translation.z = surfaceHeightQ12 + heightOffsetQ12;
      (modelNode->worldTransform).translation.x = worldXQ12;
      (modelNode->worldTransform).translation.y = worldYQ12;
      (modelNode->modelPayload).worldRotationAngle1 = FIXED_ANGLE16_QUARTER_TURN;
      modelNode->runtimeFlags = modelNode->runtimeFlags | 1;
    }
  }
  return;
}

/* Contact kind 3 (articulated walker): moves the model node to the point, sets node flag 0x1 and lets the
   articulated code seat its legs/suspension on the terrain (height and tilt come from there, so
   heightOffsetQ12 is unused).
   Reached through g_ArmyPlacementContactKindDispatchTable.callbacks[3], indexed by the model
   definition's contact kind (placementContactKindIndex).
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

ArmyPlacementContactCallbackTable5 g_ArmyPlacementContactKindDispatchTable = {
    .callbacks = {
        /* 0 */ THANDOR_FN(ArmyPlacementContact_ApplyTerrainHeight),
        /* 1 */ THANDOR_FN(ArmyPlacementContact_ApplyWaterSurfaceHeight),
        /* 2 */ THANDOR_FN(ArmyPlacementContact_ApplyTerrainHeightAndNormal),
        /* 3 */ THANDOR_FN(ArmyPlacementContact_InitializeArticulatedSuspension),
        /* 4 */ THANDOR_FN(ArmyPlacementContact_ApplyTopSurfaceHeight)
    }};

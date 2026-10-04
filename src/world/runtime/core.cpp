/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/src/world/runtime/core.cpp
 * Reverse engineering by idkFoxes 2026
 */

#include <thandor/world/runtime/core.h>
#include <thandor/thandor.h>

/* Module data. */

static GraphicsFixedVec3 g_GraphicsProjectionScratchVec3 = {0};

/* Implementation ownership: world/runtime/core. */

/* Attaches a field grid ('fld' asset) to the world and computes its triangle normals; any other asset is
   ignored.
*/
void WorldRuntime_AttachFieldGridAsset(FieldGridAsset *asset,WorldRuntimeContext *world)

{
  if (asset->common.magic == ASSET_MAGIC_FLD) {
    world->fieldGrid = asset;
    FieldGrid_RecomputeInteriorTriangleNormalAngles(asset);
    WorldRuntime_ClearFieldGridDirtyFlag(world);
  }
  return;
}

/* Returns the field grid's top surface height (terrain plus the water above it) at a world point, or
   WORLD_HEIGHT_NO_FIELD_GRID when the world has no field grid.
*/
uint32_t WorldRuntime_InterpolateTopSurfaceHeightOrSentinel
                (Q12 worldYQ12,Q12 worldXQ12,WorldRuntimeContext *worldRuntime)

{
  Q12 topSurfaceHeightQ12;

  topSurfaceHeightQ12 = WORLD_HEIGHT_NO_FIELD_GRID;
  if (worldRuntime->fieldGrid != NULL) {
    FieldGrid_InterpolateTopSurfaceHeight(worldYQ12,worldXQ12,worldRuntime->fieldGrid,&topSurfaceHeightQ12);
  }
  return topSurfaceHeightQ12;
}

/* Drag selection test: projects the node's world position to the screen and returns true when that pixel
   lies inside the rectangle spanned by the pointer press position and the current pointer position of
   boundsControl (inclusive, in either corner order).
*/
Bool8 WorldRuntimeNode_IsPositionInsideBounds
          (WorldOwnerListNode *runtimeNode,WorldRuntimeExtendedMapControlView *boundsControl)

{
  int boundsSecondX;
  int boundsSecondY;
  int projectedScreenX;
  int boundsMaxX;
  int projectedScreenY;
  int boundsMinX;
  int boundsMaxY;
  int boundsMinY;
  GraphicsProjectedPointPair projectedPosition;
  
  FixedTransform_ApplyPoint
            (&g_GraphicsProjectionScratchVec3,(GraphicsFixedVec3 *)&runtimeNode->worldXQ12,
             &g_ViewProjectionMatrixFixed);
  projectedPosition = Graphics_ProjectViewPoint(&g_GraphicsProjectionScratchVec3);
  boundsMinX = boundsControl->pointerPressX;
  boundsSecondX = boundsControl->pointerX;
  boundsMinY = boundsControl->pointerPressY;
  boundsSecondY = boundsControl->pointerY;
  /* the projection is in Q12 screen pixels */
  projectedScreenX = projectedPosition.projectedX >> 12;
  projectedScreenY = projectedPosition.projectedY >> 12;
  boundsMaxX = boundsSecondX;
  if (boundsSecondX < boundsMinX) {
    boundsMaxX = boundsMinX;
    boundsMinX = boundsSecondX;
  }
  boundsMaxY = boundsSecondY;
  if (boundsSecondY < boundsMinY) {
    boundsMaxY = boundsMinY;
    boundsMinY = boundsSecondY;
  }
  if (boundsMinX <= projectedScreenX && projectedScreenX <= boundsMaxX && boundsMinY <= projectedScreenY &&
      projectedScreenY <= boundsMaxY) {
    return true;
  }
  return false;
}

/* Attaches a caller-owned workspace of count pointer-sized words to the world runtime and zeroes it.
*/
void WorldRuntime_AttachAndClearDwordArray(WorldWorkspaceElementCount count,uintptr_t *array,WorldRuntimeContext *world)

{
  world->dwordArray = array;
  world->dwordArrayCount = count;
  for (; count != 0; count--) {
    *array = 0;
    array = array + 1;
  }
  return;
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

  if (sourceRuntime == NULL) {
    return;
  }
  /* 5f-format: ArmyAssetRecordPrefix.rootNodeOffsetOrPointer */
  definitionRecord = ModelDefinitionRegistry_FindById
                    (((AiLinkedDefinitionListView *)
                      ((ArmyAssetRecordPrefix *)sourceRuntime)->rootNodeOffsetOrPointer)->definitionIds[0]);
  if (definitionRecord == NULL) {
    return;
  }
  overlayExtent = UINT32_MAX;
  ownerNode = worldRuntime->ownerListHead;
  overlayBaseOffset = ((ModelDefinition *)definitionRecord)->placementFlags;
  if (ownerNode == NULL) {
    return;
  }
  if (((ModelDefinition *)definitionRecord)->runtimeClassId == MODEL_RUNTIME_CLASS_14) {
    overlayExtent =
         FIELD_CELL_XENITE_SUPPORT << ((uint8_t)((ModelDefinition *)definitionRecord)->classParameterC0 & 31);
  }
  overlayCallback = g_TerrainClassPlacementAndOverlayCallbacks10.overlayCallbacks
           [((ModelDefinition *)definitionRecord)->placementContactKindIndex];
  for (; ownerNode != NULL; ownerNode = ownerNode->nextNode) {
    if (ownerNode->ownerClassId != WORLD_OWNER_RUNTIME_MODEL) {
      continue;
    }
    modelRuntime = (ModelRuntimeSlot *)ownerNode->runtimePayload;
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

/* Per-tick update of army class 5 (g_ArmyRuntimeOrderHandlerMatrix11Columns24Classes.runtimeUpdate[5]):
   that class has nothing to update, so this does nothing.
*/
void ArmyRuntimeClass_NoOpTickUpdateForClass5
               (WorldRuntimeContext *worldRuntime,ModelRuntimeUpdateView *modelRuntime)

{
  return;
}

/* Per-tick update of army class 6 (g_ArmyRuntimeOrderHandlerMatrix11Columns24Classes.runtimeUpdate[6]):
   that class has nothing to update, so this does nothing.
*/
void ArmyRuntimeClass_NoOpTickUpdateForClass6
               (WorldRuntimeContext *worldRuntime,ModelRuntimeUpdateView *modelRuntime)

{
  return;
}

/* Default model-unrebase handler (g_ArmyRuntimeOrderHandlerMatrix11Columns24Classes.modelUnrebase, every class
   except 13 and 21): those classes keep no pointers that need unrebasing, so this does nothing.
*/
void UnifiedRuntimeDefault_OneArgNoOpC(ModelRuntimeSlot *modelRuntime)

{
  return;
}

/* Default model release/commit handler (g_ArmyRuntimeOrderHandlerMatrix11Columns24Classes.modelReleaseOrCommit,
   every class except 14-16 and 21): those classes hold no faction capacity or placement reservation to release,
   so this does nothing.
*/
void UnifiedRuntimeDefault_TwoArgNoOpB
               (ModelDefinitionRecordPrefix *modelDefinition,ModelRuntimeSlot *modelRuntime)

{
  return;
}

/* Default placement validation (g_ArmyRuntimeOrderHandlerMatrix11Columns24Classes.placementValidation, classes
   0, 5-9, 12 and 21): accepts every placement.
*/
Bool8 UnifiedRuntimeDefault_TwoArgSuccess
          (WorldRuntimeContext *worldRuntime,ModelRuntimePlacementValidationView *modelRuntime)

{
  return false;
}

/* Default class method D (g_ArmyRuntimeOrderHandlerMatrix11Columns24Classes.classMethodD, classes 0, 9, 12,
   15, 20 and 23), the slot where the other classes update their looping and positioned sounds: these classes
   have none, so this does nothing.
*/
void UnifiedRuntimeDefault_TwoArgNoOpD(WorldRuntimeContext *worldRuntime,ModelRuntimeSlot *modelRuntime)

{
  return;
}

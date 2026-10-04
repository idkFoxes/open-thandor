/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/src/gameplay/ai/construction.cpp
 * Reverse engineering by idkFoxes 2026
 */

#include <thandor/gameplay/ai/construction.h>
#include <thandor/thandor.h>

/* Implementation ownership: gameplay/ai/construction. */

/* Builds a pending resource structure (ARM_0330/ARM_0332) of the AI faction: at the first workspace-08 site of
   this asset where the mode-0 placement test passes it creates the structure with the site's heading, rebuilds
   its model transforms, dispatches its class command, starts the effect referenced by its model runtime and
   removes the asset from the faction's pending list. Nothing happens when no site passes.
*/
void AiConstructionPlanner_PlaceSpecialAssetFromWorkspace
          (PckArmyAssetIdCatalog armyAssetId,FactionRuntimeIndex factionIndex,
          WorldRuntimeContext *worldRuntime)

{
  FieldGridCell *workspaceRecord;
  ArmyRuntimeSlot *modelNodeRuntime;
  ArmyRuntimeSlot *primarySlot;
  ModelRuntimeSlot *createdModelRuntime;
  Ptr32<ArmyRuntimeSlot> *createdSlotPair;
  int recordsRemaining;
  AiTerrainFeatureWorkspaceEntry *terrainFeatureEntry;

  terrainFeatureEntry = g_AiWorkspace08TerrainFeatureSites;
  for (recordsRemaining = g_AiWorkspace08Count; recordsRemaining != 0; recordsRemaining--, terrainFeatureEntry++) {
    if (armyAssetId != terrainFeatureEntry->armyAssetId) {
      continue;
    }
    workspaceRecord = terrainFeatureEntry->cell;
    if (AiPlacement_TestWorkspaceRecordAtPoint(armyAssetId,workspaceRecord,factionIndex,(UiRootNode *)worldRuntime)) {
      continue; /* placement rejected */
    }
    createdSlotPair = (Ptr32<ArmyRuntimeSlot> *)ArmyRuntime_CreateInstanceFromAsset
                      (ARMY_CREATE_UNLOCK_TECHNOLOGY,(uint32_t)(uint16_t)workspaceRecord->triangle0NormalAngles,
                       workspaceRecord->worldY,workspaceRecord->worldX,factionIndex,armyAssetId,
                       worldRuntime,NULL);
    if (createdSlotPair == NULL) {
      return;
    }
    /* the create result points at the pair {army slot, model node}; the node is typed as a slot here, so
       the effect arguments below are its fields under ArmyRuntimeSlot names */
    modelNodeRuntime = createdSlotPair[1];
    primarySlot = *createdSlotPair;
    modelNodeRuntime->movementPosition0Q12 = 0;
    createdModelRuntime = (primarySlot->modelRuntimeOrSavedOffset).modelRuntime;
    ModelNodeRuntime_RebuildTransformsFromRoot((ModelRuntimeNode *)modelNodeRuntime);
    ArmyRuntime_DispatchClassCommand((ArmyRuntimeSlot *)createdSlotPair,worldRuntime); /* the created army */
    EffectRuntimePool_CreateInstanceFromDefinition
              (EFFECT_RUNTIME_COMPLETION_NONE,THANDOR_COMPOUND(EffectRuntimeOwnerReference){NULL},
               ((ModelRuntimeNode *)modelNodeRuntime)->modelPayload.worldRotationAngle2,
               ((ModelRuntimeNode *)modelNodeRuntime)->modelPayload.worldRotationAngle1,
               ((ModelRuntimeNode *)modelNodeRuntime)->modelPayload.worldRotationAngle0,
               ((ModelRuntimeNode *)modelNodeRuntime)->worldTransform.translation.z,
               ((ModelRuntimeNode *)modelNodeRuntime)->worldTransform.translation.y,
               ((ModelRuntimeNode *)modelNodeRuntime)->worldTransform.translation.x,
               (EffectDefinition *)createdModelRuntime->attachments[2].childLocalRotationAngle0, /* 5f-format: ModelRuntimeSlot.attachments[2].childLocalRotationAngle0 */
               worldRuntime);
    AiConstructionPlanner_ConsumeFactionPendingArmyAsset(armyAssetId,factionIndex);
    return;
  }
}

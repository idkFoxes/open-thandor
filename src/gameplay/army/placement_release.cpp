/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/src/gameplay/army/placement_release.cpp
 * Reverse engineering by idkFoxes 2026
 */

#include <thandor/gameplay/army/placement_release.h>
#include <thandor/thandor.h>
#include <thandor/platform/bootstrap/image.h>

/* Release handler of a resource extractor (class 14): gives back the storage it added to its faction (see
   ArmyPlacement_ReleaseFactionCapacity) and clears the extractor markers (armyRuntimeSavedOffset,
   resourceExtractionDescriptor) of the field-grid cell it stood on, so the deposit can be built on again.
   Reached through g_ArmyRuntimeOrderHandlerMatrix11Columns24Classes.modelReleaseOrCommit[14],
   called by ModelRuntimePool_DestroyHierarchyAndDetach.
*/
void ArmyPlacement_ReleaseFactionCapacityAndClearGridReservation
          (ModelDefinitionRecordPrefix *modelDefinition,ModelRuntimeSlot *modelRuntime)

{
  uint8_t *storageLimit;
  int storageStock;
  int storageLimitValue;
  uint32_t storageContribution;
  InGameRuntimeRoot *inGameRoot;
  int factionOffset;
  int limitOffset;
  int cellColumn;
  int cellRow;
  int cellIndex;
  FieldGridCoordinates gridCoordinates;
  FieldGridAsset *activeFieldGrid;

  /* classParameterC0: the resource selector (0 Xenite, 1 Tritium), classParameterC4: the storage the model adds */
  storageContribution = ModelView_Cast<ModelDefinition>(modelDefinition)->classParameterC4;
  factionOffset =
       ((modelRuntime->ownerArmyRuntimeOrSavedOffset).armyRuntime)->factionIndex *
       GAME_FACTION_RUNTIME_RECORD_BYTES;
  limitOffset = factionOffset + 4; /* xeniteStorageLimitQ4 */
  if (ModelView_Cast<ModelDefinition>(modelDefinition)->classParameterC0 != 0) {
    limitOffset = factionOffset + 20; /* tritiumStorageLimitQ4 */
  }
  /* the faction records read as bytes: the limit dword at the selected byte offset */
  storageLimit = reinterpret_cast<uint8_t *>(g_GameFactionRuntimeImage.records) + limitOffset;
  storageLimitValue = (int)Thandor_LoadU32(storageLimit);
  /* the stock loses the share this storage held */
  if ((((int)modelRuntime->health < 2) && (storageLimitValue != 0)) &&
     (((modelRuntime->classState).stateFlags & ARMY_MODEL_STATE_DESTRUCTION_STARTED) == 0)) {
    storageStock = (int)Thandor_LoadU32(storageLimit - 4); /* the stock is the dword before the limit */
    Thandor_StoreU32(storageLimit - 4,(uint32_t)(storageStock -
         (int)(((int64_t)(int)storageContribution * (int64_t)storageStock) / (int64_t)storageLimitValue)));
  }
  Thandor_StoreU32(storageLimit,Thandor_LoadU32(storageLimit) - storageContribution);
  inGameRoot = g_InGameRuntimeRoot;
  gridCoordinates = FieldGrid_WorldToGridQ12
                    ((((modelRuntime->rootModelNodeOrSavedOffset).modelNode)->worldTransform).
                     translation.y,
                     (((modelRuntime->rootModelNodeOrSavedOffset).modelNode)->worldTransform).
                     translation.x);
  cellColumn = ((gridCoordinates.columnQ12 >> (Q12_SHIFT - 1)) + 1) >> 1;
  cellRow = ((gridCoordinates.rowQ12 >> (Q12_SHIFT - 1)) + 1) >> 1;
  activeFieldGrid = (inGameRoot->worldRuntime).fieldGrid;
  if (((0 < cellColumn) && (0 < cellRow)) && (activeFieldGrid != nullptr)) {
    if ((cellColumn + 1 < (int)activeFieldGrid->gridWidth) &&
        (cellRow + 1 < (int)activeFieldGrid->gridHeight)) {
      cellIndex = cellRow * activeFieldGrid->gridWidth + cellColumn;
      activeFieldGrid->cells[cellIndex].armyRuntimeSavedOffset = 0;
      activeFieldGrid->cells[cellIndex].resourceExtractionDescriptor = 0;
    }
  }
}

/* Release handler of a resource storage (class 15): the model's storage (the definition's classParameterC4) is
   taken off its faction's Xenite or Tritium storage limit (selector classParameterC0), and - unless the model's
   health is 2 or more, the limit is zero or class-state bit 0x20 is set - the faction's stock of that
   resource loses the proportional share (stock * storage / limit) that was kept in it.
   Reached through g_ArmyRuntimeOrderHandlerMatrix11Columns24Classes.modelReleaseOrCommit[15],
   called by ModelRuntimePool_DestroyHierarchyAndDetach.
*/
void ArmyPlacement_ReleaseFactionCapacity(ModelDefinitionRecordPrefix *modelDefinition,ModelRuntimeSlot *modelRuntime)

{
  uint8_t *storageLimit;
  int storageStock;
  int storageLimitValue;
  uint32_t storageContribution;
  int factionOffset;
  int storageLimitOffset;

  /* classParameterC0: the resource selector (0 Xenite, 1 Tritium), classParameterC4: the storage the model adds */
  storageContribution = ModelView_Cast<ModelDefinition>(modelDefinition)->classParameterC4;
  factionOffset =
       ((modelRuntime->ownerArmyRuntimeOrSavedOffset).armyRuntime)->factionIndex *
       GAME_FACTION_RUNTIME_RECORD_BYTES;
  storageLimitOffset = factionOffset + 4; /* xeniteStorageLimitQ4 */
  if (ModelView_Cast<ModelDefinition>(modelDefinition)->classParameterC0 != 0) {
    storageLimitOffset = factionOffset + 20; /* tritiumStorageLimitQ4 */
  }
  /* the faction records read as bytes: the limit dword at the selected byte offset */
  storageLimit = reinterpret_cast<uint8_t *>(g_GameFactionRuntimeImage.records) + storageLimitOffset;
  storageLimitValue = (int)Thandor_LoadU32(storageLimit);
  /* the stock loses the share this storage held */
  if ((((int)modelRuntime->health < 2) && (storageLimitValue != 0)) &&
     (((modelRuntime->classState).stateFlags & ARMY_MODEL_STATE_DESTRUCTION_STARTED) == 0)) {
    storageStock = (int)Thandor_LoadU32(storageLimit - 4); /* the stock is the dword before the limit */
    Thandor_StoreU32(storageLimit - 4,(uint32_t)(storageStock -
         (int)(((int64_t)(int)storageContribution * (int64_t)storageStock) / (int64_t)storageLimitValue)));
  }
  Thandor_StoreU32(storageLimit,Thandor_LoadU32(storageLimit) - storageContribution);
}

/* Release handler of class 21 (aircraft): the model linked in classLinkState.modelLinkOrState (its home
   pad) keeps 13 slots of army asset ids (from classLinkState.classState78 on) with a reservation bit each
   (classState.classStateB4). The first slot holding this army's asset id with its bit set gets the bit cleared
   and the counter classState70 incremented; unless class-state bit 0x20 of the released model is set, the slot
   is also emptied and the counters armyLinkOrState and classState70 are decremented.
   Reached through g_ArmyRuntimeOrderHandlerMatrix11Columns24Classes.modelReleaseOrCommit[21],
   called by ModelRuntimePool_DestroyHierarchyAndDetach.
*/
void ArmyPlacement_ReleaseClassStateReservation
          (ModelDefinitionRecordPrefix *modelDefinition,ModelRuntimeSlot *modelRuntime)

{
  uint32_t *classCounter;
  int32_t *reservationBits;
  ModelRuntimeArmyLinkOrState *armyLinkState;
  uint32_t *slotAssetIds;
  int slotIndex;
  uint32_t reservationBit;
  ModelRuntimeSlot *linkedModelSlot;

  linkedModelSlot = (modelRuntime->classLinkState).modelLinkOrState.modelRuntime;
  if (linkedModelSlot == nullptr) {
    return;
  }
  /* the 13 asset-id dwords start at classState78 and run on past it */
  slotAssetIds = &(linkedModelSlot->classLinkState).classState78;
  reservationBit = 1;
  for (slotIndex = 0; slotIndex < 13; slotIndex++) {
    if ((((modelRuntime->ownerArmyRuntimeOrSavedOffset).armyRuntime)->armyAssetId ==
         slotAssetIds[slotIndex]) &&
       (((linkedModelSlot->classState).classStateB4 & reservationBit) != 0)) {
      classCounter = &(linkedModelSlot->classLinkState).classState70;
      *classCounter = *classCounter + 1;
      reservationBits = &(linkedModelSlot->classState).classStateB4;
      *reservationBits = *reservationBits & (reservationBit ^ 0xffffffff);
      if (((modelRuntime->classState).stateFlags & ARMY_MODEL_STATE_DESTRUCTION_STARTED) != 0) {
        return;
      }
      slotAssetIds[slotIndex] = 0;
      armyLinkState = &(linkedModelSlot->classLinkState).armyLinkOrState;
      armyLinkState->armyRuntime = Thandor_U32ToPointer<ArmyRuntimeSlot>(armyLinkState->classState - 1); /* 32-bit format field: ModelRuntimeSlot.classLinkState.armyLinkOrState */
      classCounter = &(linkedModelSlot->classLinkState).classState70;
      *classCounter = *classCounter - 1;
      return;
    }
    reservationBit = reservationBit * 2;
  }
}

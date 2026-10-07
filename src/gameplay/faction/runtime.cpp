/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/src/gameplay/faction/runtime.cpp
 * Reverse engineering by idkFoxes 2026
 */

#include <thandor/gameplay/faction/runtime.h>
#include <thandor/thandor.h>

/* Returns true when bit otherFactionIndex is clear in factionIndex's record capabilityFlags.
   The mask holds one bit per faction: GameData_ResetDefaults sets the faction's own bit and bit 0, and
   GameFactionRuntime_ApplyPairwiseRelationTransition sets or clears the others, so a clear bit marks a faction
   this one is not friendly with (the AI treats its entities as foreign/hostile).
*/
bool GameFactionRuntime_TestCapabilityBitClear(uint32_t otherFactionIndex,FactionRuntimeIndex factionIndex)

{
  return (g_GameFactionRuntimeImage.records[factionIndex].capabilityFlags &
         1 << ((uint8_t)otherFactionIndex & 31)) == 0;
}

/* Returns the diplomatic relation state (0..11) of factionIndex towards otherFactionIndex: nibble
   otherFactionIndex of the faction record's packedRelationStates. States from 4 on count as friendly.
*/
FactionRelationState GameFactionRuntime_GetPackedStateNibble
          (FactionRuntimeIndex otherFactionIndex,FactionRuntimeIndex factionIndex)

{
  return g_GameFactionRuntimeImage.records[factionIndex].packedRelationStates >>
         ((uint8_t)(otherFactionIndex << 2) & 31) & 0xf;
}

/* One alert anchor of GameFactionRuntime_UpdateImpactAlertAnchorAndNotify: restarts *cooldown at 150, moves the
   anchor (*anchorY = hit model x, *anchorX = hit model y) and queues alert movie notificationMovieId when the old
   cooldown was below 50 and the hit is far from the old anchor or the cooldown had run out, and the shown faction
   owns the hit army (ownershipRecord[1] = root model node, [2] = owning army). */
static void GameFactionRuntime_MoveImpactAlertAnchor(FactionAnchorCooldownTicks *cooldown,
          GraphicsWorldCoordinateQ12 *anchorY,GraphicsWorldCoordinateQ12 *anchorX,const ModelRuntimeSlot *ownershipRecord,
          WorldRuntimeContext *worldRuntime,InGameNotificationPriority priority,
          InGameNotificationMovieId notificationMovieId)
{
  ModelRuntimeNode *hitModelNode;
  ArmyRuntimeSlot *owningArmy;
  FactionAnchorCooldownTicks previousCooldown;
  GraphicsWorldCoordinateQ12 hitX;
  GraphicsWorldCoordinateQ12 hitY;
  int distanceY;
  int distanceX;
  bool notify;

  hitModelNode = ownershipRecord->rootModelNodeOrSavedOffset.modelNode;
  previousCooldown = *cooldown;
  distanceY = *anchorY - hitModelNode->worldTransform.translation.x;
  if (distanceY < 0) {
    distanceY = -distanceY;
  }
  distanceX = *anchorX - hitModelNode->worldTransform.translation.y;
  if (distanceX < 0) {
    distanceX = -distanceX;
  }
  *cooldown = 150;
  /* 0xC000 = 12.0 world units (Q12), measured as |dy| + |dx| */
  notify = ((int)previousCooldown < 50) && ((12 * Q12_ONE < distanceY + distanceX) || (previousCooldown == 0));
  hitY = hitModelNode->worldTransform.translation.y;
  hitX = hitModelNode->worldTransform.translation.x;
  *anchorY = hitX;
  *anchorX = hitY;
  if (notify) {
    owningArmy = ownershipRecord->ownerArmyRuntimeOrSavedOffset.armyRuntime;
    if (worldRuntime->activeFactionRuntimeIndex == owningArmy->factionIndex) {
      InGameNotificationQueue_InsertPriorityRecord
                (FACTION_IMPACT_ANCHOR,0,(worldRuntime->motion).pitchAngle,(worldRuntime->motion).headingAngle,
                 hitModelNode->worldTransform.translation.y,hitModelNode->worldTransform.translation.x,priority,
                 notificationMovieId);
    }
  }
}

/* "Under attack" alert for the faction owning a hit army (called by ShotRuntime_ApplyArmyHitRelationAndNotifications
   when a shot opens hostilities): moves the faction's primary or secondary alert anchor (chosen by the definition's
   accelerationPerTick: zero = primary) to the hit model and restarts its 150-tick cooldown. The alert movie
   300 / 301 is only queued when the cooldown had fallen below 50 and the hit is more than 12 world units from
   the old anchor or the cooldown had run out, and only when the shown faction owns the hit model, so a sustained
   attack does not repeat the alert.
*/
void GameFactionRuntime_UpdateImpactAlertAnchorAndNotify
          (ModelRuntimeSlot *hitModelRuntime,WorldRuntimeContext *worldRuntime)

{
  /* the root model runtime of the hit model's army: [0] definition, [1] root model node, [2] owning army
     (with its factionIndex) */
  ModelRuntimeSlot *ownershipRecord;
  int ownerFactionIndex;
  GameFactionRuntimeRecord *ownerRecord;

  ownershipRecord =
       hitModelRuntime->ownerArmyRuntimeOrSavedOffset.armyRuntime->modelRuntimeOrSavedOffset.modelRuntime;
  ownerFactionIndex = hitModelRuntime->ownerArmyRuntimeOrSavedOffset.armyRuntime->factionIndex;
  ownerRecord = &g_GameFactionRuntimeImage.records[ownerFactionIndex];
  if (ownershipRecord->definitionOrSavedId.runtimeDefinition->accelerationPerTick == 0) {
    GameFactionRuntime_MoveImpactAlertAnchor(&ownerRecord->primaryAnchorCooldown,&ownerRecord->primaryAnchorYQ12,
                                             &ownerRecord->primaryAnchorXQ12,ownershipRecord,worldRuntime,8,300);
  }
  else {
    GameFactionRuntime_MoveImpactAlertAnchor(&ownerRecord->anchorCooldown0,&ownerRecord->secondaryAnchorYQ12,
                                             &ownerRecord->secondaryAnchorXQ12,ownershipRecord,worldRuntime,7,301);
  }
}

/* Recomputes one faction's statistics for the score / results screens: explored terrain percent, unlocked
   technologies beyond the five starting ones, extracted resource components, the economy and relation scores,
   the summed value of its army assets on the map, and the combined progress score.
*/
void GameFactionRuntime_RecomputeProgressAndScoreMetrics
          (FactionRuntimeIndex factionIndex,WorldRuntimeContext *worldRuntime)

{
  TerrainExploredPercent exploredPercent;
  ResourceExtractionRateQ4PerTick xeniteRate;
  ResourceExtractionRateQ4PerTick tritiumRate;
  int exploredCellCount;
  uint32_t cellCount;
  uint32_t cellsRemaining;
  uint32_t technologyCount;
  uint32_t technologyBit;
  FactionTechnologyCount technologyCountBeyondBaseline;
  int xeniteComponent;
  int tritiumComponent;
  int armyAssetValueSum;
  ArmyRuntimeSlot *ownerArmy;
  uint8_t *cellVisibilityCursor;
  uint32_t maskWordIndex;
  ArmyAssetRecordPrefix *resolvedAsset;
  FieldGridAsset *terrainGrid;
  WorldOwnerListNode *ownerNode;

  terrainGrid = worldRuntime->fieldGrid;
  cellCount = terrainGrid->gridWidth * terrainGrid->gridHeight;
  exploredCellCount = 0;
  /* one byte per faction in the cell's occupancyMask; bits 3-7 set = the faction has explored the cell (0x80-byte cells).
     Original quirk: a grid with zero cells would count down from 0 (never happens). */
  cellVisibilityCursor = reinterpret_cast<uint8_t *>(&terrainGrid->cells[0].occupancyMask) + factionIndex;
  cellsRemaining = cellCount;
  /* Original quirk: a do-while, a count of 0 runs it 2^32 times (D8: kept for step 11) */
  do {
    if ((*cellVisibilityCursor & FIELD_CELL_OCCUPANCY_EXPLORED_BITS) != 0) {
      exploredCellCount++;
    }
    cellVisibilityCursor = cellVisibilityCursor + sizeof(FieldGridCell);
    cellsRemaining--;
  } while (cellsRemaining != 0);
  g_GameFactionRuntimeImage.records[factionIndex].exploredTerrainPercent =
       (uint32_t)(exploredCellCount * 100) / cellCount;
  /* count the set bits of the faction record's 256-bit technology mask (technologyMasks256Bits) */
  technologyCount = 0;
  for (maskWordIndex = 0; maskWordIndex < 8; maskWordIndex++) {
    for (technologyBit = 1; technologyBit != 0; technologyBit = technologyBit * 2) {
      if ((g_GameFactionRuntimeImage.records[factionIndex].technologyMasks256Bits[maskWordIndex] & technologyBit) != 0) {
        technologyCount++;
      }
    }
  }
  technologyCountBeyondBaseline = technologyCount - 5;
  if (technologyCount < 5) {
    technologyCountBeyondBaseline = 0;
  }
  exploredPercent = g_GameFactionRuntimeImage.records[factionIndex].exploredTerrainPercent;
  xeniteComponent = (int)g_GameFactionRuntimeImage.records[factionIndex].xeniteExtractedTotalQ4 >> 4;
  tritiumComponent = (int)g_GameFactionRuntimeImage.records[factionIndex].tritiumExtractedTotalQ4 >> 4;
  g_GameFactionRuntimeImage.records[factionIndex].unlockedTechnologyCountBeyondBaseline = technologyCountBeyondBaseline;
  g_GameFactionRuntimeImage.records[factionIndex].primaryResourceComponent = xeniteComponent;
  g_GameFactionRuntimeImage.records[factionIndex].secondaryResourceComponent = tritiumComponent;
  g_GameFactionRuntimeImage.records[factionIndex].economyProgressScore =
       (int)(xeniteComponent * 16 + tritiumComponent * 8 + technologyCountBeyondBaseline * (10 * Q12_ONE) +
             exploredPercent * Q12_ONE) >> Q12_SHIFT;
  g_GameFactionRuntimeImage.records[factionIndex].relationScore =
       (g_GameFactionRuntimeImage.records[factionIndex].relationCounterB * (40 * Q12_ONE) +
       g_GameFactionRuntimeImage.records[factionIndex].relationCounterD * (-20 * Q12_ONE) +
       g_GameFactionRuntimeImage.records[factionIndex].relationCounterA * (20 * Q12_ONE) +
       g_GameFactionRuntimeImage.records[factionIndex].relationCounterC * (-10 * Q12_ONE) +
       g_GameFactionRuntimeImage.records[factionIndex].relationCounterE * (20 * Q12_ONE) +
       g_GameFactionRuntimeImage.records[factionIndex].relationCounterF * (40 * Q12_ONE)) >> Q12_SHIFT;
  /* sum the army asset's xeniteCostQ4 (read as resolvedAsset[2].registryId) over every model of this faction
     on the map */
  armyAssetValueSum = 0;
  for (ownerNode = worldRuntime->ownerListHead; ownerNode != nullptr;
      ownerNode = ownerNode->nextNode) {
    if (ownerNode->ownerClassId != WORLD_OWNER_RUNTIME_MODEL) {
      continue;
    }
    ownerArmy = WorldOwnerNode_ModelRuntime(ownerNode)->ownerArmyRuntimeOrSavedOffset.armyRuntime;
    if ((factionIndex == ownerArmy->factionIndex) &&
        (ArmyAssetRegistry_FindById(ownerArmy->armyAssetId,&resolvedAsset) == 0)) {
      armyAssetValueSum = armyAssetValueSum + resolvedAsset[2].registryId;
    }
  }
  tritiumRate = g_GameFactionRuntimeImage.records[factionIndex].tritiumExtractionRateQ4PerTick;
  xeniteRate = g_GameFactionRuntimeImage.records[factionIndex].xeniteExtractionRateQ4PerTick;
  exploredPercent = g_GameFactionRuntimeImage.records[factionIndex].exploredTerrainPercent;
  technologyCountBeyondBaseline = g_GameFactionRuntimeImage.records[factionIndex].unlockedTechnologyCountBeyondBaseline;
  g_GameFactionRuntimeImage.records[factionIndex].activeArmyContribution = armyAssetValueSum;
  g_GameFactionRuntimeImage.records[factionIndex].combinedProgressScore =
       ((tritiumRate >> 1) + xeniteRate) * exploredPercent * technologyCountBeyondBaseline >> 3;
}

/* Finds which of its faction's eight runtime groups (32 member slots each) holds runtimeEntry, for the group
   selection commands in gameplay/selection/runtime. Returns the one-based group number (1..8), or 0 when it is
   in no group.
*/
uint32_t GameFactionRuntime_FindRuntimeGroupNumber(RuntimeModelFactionPrefix *runtimeEntry)

{
  int slotsRemaining;
  uint32_t groupNumber;
  Ptr32<ArmyRuntimeSlot> *slotCursor;
  Ptr32<ArmyRuntimeSlot> *nextSlotCursor;
  bool found;

  groupNumber = 0;
  nextSlotCursor = g_GameFactionRuntimeImage.records[runtimeEntry->factionIndex].runtimeGroupMembers8x32;
  do {
    slotsRemaining = 32;
    groupNumber++;
    found = groupNumber == 0; /* the original's zero test of the increment; never true */
    slotCursor = nextSlotCursor;
    do {
      nextSlotCursor = slotCursor;
      if (slotsRemaining == 0) break;
      slotsRemaining--;
      nextSlotCursor = slotCursor + 1;
      found = reinterpret_cast<ArmyRuntimeSlot *>(runtimeEntry) == *slotCursor; /* prefix view of the army */
      slotCursor = nextSlotCursor;
    } while (!found);
    if (found) {
      return groupNumber;
    }
  } while (groupNumber <= 7);
  return 0;
}

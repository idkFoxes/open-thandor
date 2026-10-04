/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/src/ui/ingame/editor_army_cycling.cpp
 * Reverse engineering by idkFoxes 2026
 */

#include <thandor/ui/ingame/editor_army_cycling.h>
#include <thandor/thandor.h>

/* Keeps the editor's unit-placement army id when it names a placeable unit (flag 0x0100 set, 0x0200 clear),
   otherwise moves on to the next such id with wrap-around. Called by
   InGameUiCommandRuntime_ApplyInteractionSubsystemActiveState when the in-game command UI is activated.
   Returns the placeable unit id.
*/
ArmyAssetId ArmyAssetRegistry_NormalizeIdToPlaceableUnit(PckArmyAssetIdCatalog recordId)

{
  if (ArmyAssetRegistry_HasNoPlaceableUnitWithId(recordId)) {
    return ArmyAssetRegistry_FindNextPlaceableUnitWrapped(recordId);
  }
  return recordId;
}

/* Steps the editor's unit-placement army id to the next placeable unit (flag 0x0100 set, 0x0200 clear). Ids of
   other units (0x0200 clear) are skipped; at the end of the run of unit ids it goes back to the first id of that
   run, so the step cycles within one contiguous id block. Called from the in-game keyboard dispatch table
   g_InGameKeyboardDispatchRecords (command 0x10011) in unit-placement mode.
   Returns the new id.
*/
ArmyAssetId ArmyAssetRegistry_StepForwardPlaceableUnit(ArmyAssetId recordId)

{
  ArmyAssetId baseId;
  Bool8 broaderAbsent;
  ArmyAssetId candidateId;

  baseId = recordId;
  candidateId = baseId + 1;
  while (ArmyAssetRegistry_HasNoPlaceableUnitWithId(candidateId)) {
    broaderAbsent = (Bool8)ArmyAssetRegistry_HasNoUnitWithId(candidateId);
    recordId = candidateId;
    if (broaderAbsent) {
      /* Left the run: walk back to its other end. */
      do {
        baseId--;
        broaderAbsent = (Bool8)ArmyAssetRegistry_HasNoUnitWithId(baseId);
        recordId = baseId;
      } while (!broaderAbsent);
    }
    baseId = recordId;
    candidateId = baseId + 1;
  }
  return candidateId;
}

/* Steps the editor's unit-placement army id to the previous placeable unit (flag 0x0100 set, 0x0200 clear).
   Ids of other units are skipped; at the start of the run of unit ids it goes forward to the last id of that
   run, so the step cycles within one contiguous id block. Called from the in-game keyboard dispatch table
   g_InGameKeyboardDispatchRecords (command 0x10019) in unit-placement mode.
   Returns the new id.
*/
ArmyAssetId ArmyAssetRegistry_StepBackwardPlaceableUnit(ArmyAssetId recordId)

{
  ArmyAssetId baseId;
  Bool8 broaderAbsent;
  ArmyAssetId candidateId;

  baseId = recordId;
  candidateId = baseId - 1;
  while (ArmyAssetRegistry_HasNoPlaceableUnitWithId(candidateId)) {
    broaderAbsent = (Bool8)ArmyAssetRegistry_HasNoUnitWithId(candidateId);
    recordId = candidateId;
    if (broaderAbsent) {
      /* Left the run: walk back to its other end. */
      do {
        baseId++;
        broaderAbsent = (Bool8)ArmyAssetRegistry_HasNoUnitWithId(baseId);
        recordId = baseId;
      } while (!broaderAbsent);
    }
    baseId = recordId;
    candidateId = baseId - 1;
  }
  return candidateId;
}

/* Moves the editor's unit-placement army id back out of its current run of unit ids (flag 0x0200 clear) and
   returns the nearest placeable unit (0x0100 set, 0x0200 clear) below it, wrapping from below 0 to 0x1000. Unlike
   the step functions this jumps between id blocks. Called from the in-game keyboard dispatch table
   g_InGameKeyboardDispatchRecords (command 0x10014) in unit-placement mode.
*/
ArmyAssetId ArmyAssetRegistry_FindPreviousPlaceableUnitWrapped(ArmyAssetId recordId)

{
  /* Leave the current run of unit ids first. */
  while (!ArmyAssetRegistry_HasNoUnitWithId(recordId)) {
    recordId--;
    if ((int)recordId < 0) {
      recordId = ARMY_ASSET_EDITOR_ID_LIMIT;
      break;
    }
  }
  /* Scan backward for a qualified candidate, wrapping below zero to 0x1000. */
  while (ArmyAssetRegistry_HasNoPlaceableUnitWithId(recordId)) {
    recordId--;
    if ((int)recordId < 0) {
      recordId = ARMY_ASSET_EDITOR_ID_LIMIT;
    }
  }
  return recordId;
}

/* Keeps the editor's object-placement army id when it names a placeable object (flags 0x0100 and 0x0200 set),
   otherwise moves on to the next such id with wrap-around. Called by
   InGameUiCommandRuntime_ApplyInteractionSubsystemActiveState when the in-game command UI is activated.
   Returns the placeable object id.
*/
ArmyAssetId ArmyAssetRegistry_NormalizeIdToPlaceableObject(PckArmyAssetIdCatalog recordId)

{
  if (ArmyAssetRegistry_HasNoPlaceableObjectWithId(recordId)) {
    return ArmyAssetRegistry_FindNextPlaceableObjectWrapped(recordId);
  }
  return recordId;
}

/* Steps the editor's object-placement army id to the next placeable object (flags 0x0100 and 0x0200 set). Other
   objects (0x0200 set) are skipped; at the end of the run of object ids it goes back to the first id of that run,
   so the step cycles within one contiguous id block. Called from the in-game keyboard dispatch table
   g_InGameKeyboardDispatchRecords (command 0x10011) in object-placement mode.
   Returns the new id.
*/
ArmyAssetId ArmyAssetRegistry_StepForwardPlaceableObject(ArmyAssetId recordId)

{
  ArmyAssetId baseId;
  Bool8 broaderAbsent;
  ArmyAssetId candidateId;

  baseId = recordId;
  candidateId = baseId + 1;
  while (ArmyAssetRegistry_HasNoPlaceableObjectWithId(candidateId)) {
    broaderAbsent = (Bool8)ArmyAssetRegistry_HasNoObjectWithId(candidateId);
    recordId = candidateId;
    if (broaderAbsent) {
      /* Left the run: walk back to its other end. */
      do {
        baseId--;
        broaderAbsent = (Bool8)ArmyAssetRegistry_HasNoObjectWithId(baseId);
        recordId = baseId;
      } while (!broaderAbsent);
    }
    baseId = recordId;
    candidateId = baseId + 1;
  }
  return candidateId;
}

/* Steps the editor's object-placement army id to the previous placeable object (flags 0x0100 and 0x0200 set).
   Other objects are skipped; at the start of the run of object ids it goes forward to the last id of that run,
   so the step cycles within one contiguous id block. Called from the in-game keyboard dispatch table
   g_InGameKeyboardDispatchRecords (command 0x10019) in object-placement mode.
   Returns the new id.
*/
ArmyAssetId ArmyAssetRegistry_StepBackwardPlaceableObject(ArmyAssetId recordId)

{
  ArmyAssetId baseId;
  Bool8 broaderAbsent;
  ArmyAssetId candidateId;

  baseId = recordId;
  candidateId = baseId - 1;
  while (ArmyAssetRegistry_HasNoPlaceableObjectWithId(candidateId)) {
    broaderAbsent = (Bool8)ArmyAssetRegistry_HasNoObjectWithId(candidateId);
    recordId = candidateId;
    if (broaderAbsent) {
      /* Left the run: walk back to its other end. */
      do {
        baseId++;
        broaderAbsent = (Bool8)ArmyAssetRegistry_HasNoObjectWithId(baseId);
        recordId = baseId;
      } while (!broaderAbsent);
    }
    baseId = recordId;
    candidateId = baseId - 1;
  }
  return candidateId;
}

/* Moves the editor's object-placement army id back out of its current run of object ids (flag 0x0200 set) and
   returns the nearest placeable object (0x0100 and 0x0200 set) below it, wrapping from below 0 to 0x1000. Called
   from the in-game keyboard dispatch table g_InGameKeyboardDispatchRecords (command 0x10014)
   in object-placement mode.
*/
ArmyAssetId ArmyAssetRegistry_FindPreviousPlaceableObjectWrapped(ArmyAssetId recordId)

{
  /* Leave the current run of object ids first. */
  while (!ArmyAssetRegistry_HasNoObjectWithId(recordId)) {
    recordId--;
    if ((int)recordId < 0) {
      recordId = ARMY_ASSET_EDITOR_ID_LIMIT;
      break;
    }
  }
  /* Scan backward for a qualified candidate, wrapping below zero to 0x1000. */
  while (ArmyAssetRegistry_HasNoPlaceableObjectWithId(recordId)) {
    recordId--;
    if ((int)recordId < 0) {
      recordId = ARMY_ASSET_EDITOR_ID_LIMIT;
    }
  }
  return recordId;
}

/* Moves the editor's unit-placement army id forward out of its current run of unit ids (flag 0x0200 clear) and
   returns the next placeable unit (0x0100 set, 0x0200 clear), wrapping from 0x1000 to 0. Unlike the step
   functions this jumps between id blocks. Called from the in-game keyboard dispatch table
   g_InGameKeyboardDispatchRecords (command 0x10016) in unit-placement mode, and by
   ArmyAssetRegistry_NormalizeIdToPlaceableUnit. Returns the found id; the scan only ends on a match (it loops
   forever when no placeable unit is registered).
*/
ArmyAssetId ArmyAssetRegistry_FindNextPlaceableUnitWrapped(ArmyAssetId recordId)

{
  /* Leave the current run of unit ids first. */
  while (!ArmyAssetRegistry_HasNoUnitWithId(recordId)) {
    recordId++;
  }
  while (ArmyAssetRegistry_HasNoPlaceableUnitWithId(recordId)) {
    recordId++;
    if (ARMY_ASSET_EDITOR_ID_LIMIT - 1 < recordId) {
      recordId = 0;
    }
  }
  return recordId;
}

/* Moves the editor's object-placement army id forward out of its current run of object ids (flag 0x0200 set) and
   returns the next placeable object (0x0100 and 0x0200 set), wrapping from 0x1000 to 0. Called from the in-game
   keyboard dispatch table g_InGameKeyboardDispatchRecords (command 0x10016) in
   object-placement mode, and by ArmyAssetRegistry_NormalizeIdToPlaceableObject. Returns the found id; the scan
   only ends on a match (it loops forever when no placeable object is registered).
*/
ArmyAssetId ArmyAssetRegistry_FindNextPlaceableObjectWrapped(ArmyAssetId recordId)

{
  /* Leave the current run of object ids first. */
  while (!ArmyAssetRegistry_HasNoObjectWithId(recordId)) {
    recordId++;
  }
  while (ArmyAssetRegistry_HasNoPlaceableObjectWithId(recordId)) {
    recordId++;
    if (ARMY_ASSET_EDITOR_ID_LIMIT - 1 < recordId) {
      recordId = 0;
    }
  }
  return recordId;
}

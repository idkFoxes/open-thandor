/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/src/assets/army/catalog.c
 * Reverse engineering by idkFoxes 2026
 */

#include <thandor/assets/army/catalog.h>
#include <thandor/thandor.h>

/* Implementation ownership: assets/army/catalog. */

/* Address: 0x005719F0.
   Keeps the editor's unit-placement army id when it names a placeable unit (flag 0x0100 set, 0x0200 clear),
   otherwise moves on to the next such id with wrap-around. Called by
   InGameUiCommandRuntime_ApplyInteractionSubsystemActiveState when the in-game command UI is activated.
*/
ArmyAssetIdSearchResult ArmyAssetRegistry_NormalizeIdForFlag0100Without0200(PckArmyAssetIdCatalog recordId)

{
  bool idAbsent;
  ArmyAssetIdSearchResult searchResult;
  ArmyAssetIdSearchResult normalizedResult;
  
  idAbsent = (bool)ArmyAssetRegistry_HasIdWithFlag0100Without0200(recordId);
  searchResult.notFound = idAbsent;
  searchResult.armyAssetId = recordId;
  if (idAbsent) {
    searchResult = ArmyAssetRegistry_FindNextFlag0100Without0200Wrapped(recordId);
  }
  normalizedResult.armyAssetId = searchResult.armyAssetId;
  normalizedResult.notFound = searchResult.notFound;
  return normalizedResult;
}


/* Address: 0x00571A10.
   Steps the editor's unit-placement army id to the next placeable unit (flag 0x0100 set, 0x0200 clear). Ids of
   other units (0x0200 clear) are skipped; at the end of the run of unit ids it goes back to the first id of that
   run, so the step cycles within one contiguous id block. Called from the in-game keyboard dispatch table
   g_InGameKeyboardDispatchRecords (handler 0x0056EAA0, command 0x10011) in unit-placement mode.
*/
ArmyAssetIdSearchResult ArmyAssetRegistry_StepForwardFlag0100Without0200(ArmyAssetId recordId)

{
  ArmyAssetId baseId;
  bool broaderAbsent;
  ArmyAssetIdSearchResult stepResult;
  
  while( true ) {
    baseId = recordId;
    stepResult.armyAssetId = baseId + 1;
    stepResult.notFound = (bool)ArmyAssetRegistry_HasIdWithFlag0100Without0200(stepResult.armyAssetId);
    if (!stepResult.notFound) break;
    broaderAbsent = (bool)ArmyAssetRegistry_HasIdWithoutFlag0200(stepResult.armyAssetId);
    recordId = stepResult.armyAssetId;
    if (broaderAbsent) {
      do {
        baseId--;
        broaderAbsent = (bool)ArmyAssetRegistry_HasIdWithoutFlag0200(baseId);
        recordId = baseId;
      } while (!broaderAbsent);
    }
  }
  return stepResult;
}


/* Address: 0x00571A60.
   Steps the editor's unit-placement army id to the previous placeable unit (flag 0x0100 set, 0x0200 clear).
   Ids of other units are skipped; at the start of the run of unit ids it goes forward to the last id of that
   run, so the step cycles within one contiguous id block. Called from the in-game keyboard dispatch table
   g_InGameKeyboardDispatchRecords (handler 0x0056EC10, command 0x10019) in unit-placement mode.
*/
ArmyAssetIdSearchResult ArmyAssetRegistry_StepBackwardFlag0100Without0200(ArmyAssetId recordId)

{
  ArmyAssetId baseId;
  bool broaderAbsent;
  ArmyAssetIdSearchResult stepResult;
  
  while( true ) {
    baseId = recordId;
    stepResult.armyAssetId = baseId - 1;
    stepResult.notFound = (bool)ArmyAssetRegistry_HasIdWithFlag0100Without0200(stepResult.armyAssetId);
    if (!stepResult.notFound) break;
    broaderAbsent = (bool)ArmyAssetRegistry_HasIdWithoutFlag0200(stepResult.armyAssetId);
    recordId = stepResult.armyAssetId;
    if (broaderAbsent) {
      do {
        baseId++;
        broaderAbsent = (bool)ArmyAssetRegistry_HasIdWithoutFlag0200(baseId);
        recordId = baseId;
      } while (!broaderAbsent);
    }
  }
  return stepResult;
}


/* Address: 0x00571B00.
   Moves the editor's unit-placement army id back out of its current run of unit ids (flag 0x0200 clear) and
   returns the nearest placeable unit (0x0100 set, 0x0200 clear) below it, wrapping from below 0 to 0x1000. Unlike
   the step functions this jumps between id blocks. Called from the in-game keyboard dispatch table
   g_InGameKeyboardDispatchRecords (handler 0x0056E7C0, command 0x10014) in unit-placement mode.
*/
ArmyAssetIdSearchResult ArmyAssetRegistry_FindPreviousFlag0100Without0200Wrapped(ArmyAssetId recordId)

{
  bool broaderAbsent;
  ArmyAssetIdSearchResult searchResult;
  
  for (;;) {
    broaderAbsent = (bool)ArmyAssetRegistry_HasIdWithoutFlag0200(recordId);
    if (broaderAbsent) break;
    recordId--;
    if ((int)recordId < 0) {
      recordId = ARMY_ASSET_EDITOR_ID_LIMIT;
      break;
    }
  }
  /* Scan backward for a qualified candidate, wrapping below zero to 0x1000. */
  while( true ) {
    searchResult.notFound = (bool)ArmyAssetRegistry_HasIdWithFlag0100Without0200(recordId);
    if (!searchResult.notFound) break;
    recordId--;
    if ((int)recordId < 0) {
      recordId = ARMY_ASSET_EDITOR_ID_LIMIT;
    }
  }
  searchResult.armyAssetId = recordId;
  return searchResult;
}


/* Address: 0x00571C30.
   Keeps the editor's object-placement army id when it names a placeable object (flags 0x0100 and 0x0200 set),
   otherwise moves on to the next such id with wrap-around. Called by
   InGameUiCommandRuntime_ApplyInteractionSubsystemActiveState when the in-game command UI is activated.
*/
ArmyAssetIdSearchResult ArmyAssetRegistry_NormalizeIdForFlags0100And0200(PckArmyAssetIdCatalog recordId)

{
  bool idAbsent;
  ArmyAssetIdSearchResult searchResult;
  ArmyAssetIdSearchResult normalizedResult;
  
  idAbsent = (bool)ArmyAssetRegistry_HasIdWithFlags0100And0200(recordId);
  searchResult.notFound = idAbsent;
  searchResult.armyAssetId = recordId;
  if (idAbsent) {
    searchResult = ArmyAssetRegistry_FindNextFlags0100And0200Wrapped(recordId);
  }
  normalizedResult.armyAssetId = searchResult.armyAssetId;
  normalizedResult.notFound = searchResult.notFound;
  return normalizedResult;
}


/* Address: 0x00571C50.
   Steps the editor's object-placement army id to the next placeable object (flags 0x0100 and 0x0200 set). Other
   objects (0x0200 set) are skipped; at the end of the run of object ids it goes back to the first id of that run,
   so the step cycles within one contiguous id block. Called from the in-game keyboard dispatch table
   g_InGameKeyboardDispatchRecords (handler 0x0056EAA0, command 0x10011) in object-placement mode.
*/
ArmyAssetIdSearchResult ArmyAssetRegistry_StepForwardFlags0100And0200(ArmyAssetId recordId)

{
  ArmyAssetId baseId;
  bool broaderAbsent;
  ArmyAssetIdSearchResult stepResult;
  
  while( true ) {
    baseId = recordId;
    stepResult.armyAssetId = baseId + 1;
    stepResult.notFound = (bool)ArmyAssetRegistry_HasIdWithFlags0100And0200(stepResult.armyAssetId);
    if (!stepResult.notFound) break;
    broaderAbsent = (bool)ArmyAssetRegistry_HasIdWithFlag0200(stepResult.armyAssetId);
    recordId = stepResult.armyAssetId;
    if (broaderAbsent) {
      do {
        baseId--;
        broaderAbsent = (bool)ArmyAssetRegistry_HasIdWithFlag0200(baseId);
        recordId = baseId;
      } while (!broaderAbsent);
    }
  }
  return stepResult;
}


/* Address: 0x00571CA0.
   Steps the editor's object-placement army id to the previous placeable object (flags 0x0100 and 0x0200 set).
   Other objects are skipped; at the start of the run of object ids it goes forward to the last id of that run,
   so the step cycles within one contiguous id block. Called from the in-game keyboard dispatch table
   g_InGameKeyboardDispatchRecords (handler 0x0056EC10, command 0x10019) in object-placement mode.
*/
ArmyAssetIdSearchResult ArmyAssetRegistry_StepBackwardFlags0100And0200(ArmyAssetId recordId)

{
  ArmyAssetId baseId;
  bool broaderAbsent;
  ArmyAssetIdSearchResult stepResult;
  
  while( true ) {
    baseId = recordId;
    stepResult.armyAssetId = baseId - 1;
    stepResult.notFound = (bool)ArmyAssetRegistry_HasIdWithFlags0100And0200(stepResult.armyAssetId);
    if (!stepResult.notFound) break;
    broaderAbsent = (bool)ArmyAssetRegistry_HasIdWithFlag0200(stepResult.armyAssetId);
    recordId = stepResult.armyAssetId;
    if (broaderAbsent) {
      do {
        baseId++;
        broaderAbsent = (bool)ArmyAssetRegistry_HasIdWithFlag0200(baseId);
        recordId = baseId;
      } while (!broaderAbsent);
    }
  }
  return stepResult;
}


/* Address: 0x00571D40.
   Moves the editor's object-placement army id back out of its current run of object ids (flag 0x0200 set) and
   returns the nearest placeable object (0x0100 and 0x0200 set) below it, wrapping from below 0 to 0x1000. Called
   from the in-game keyboard dispatch table g_InGameKeyboardDispatchRecords (handler 0x0056E7C0, command 0x10014)
   in object-placement mode.
*/
ArmyAssetIdSearchResult ArmyAssetRegistry_FindPreviousFlags0100And0200Wrapped(ArmyAssetId recordId)

{
  bool broaderAbsent;
  ArmyAssetIdSearchResult searchResult;
  
  for (;;) {
    broaderAbsent = (bool)ArmyAssetRegistry_HasIdWithFlag0200(recordId);
    if (broaderAbsent) break;
    recordId--;
    if ((int)recordId < 0) {
      recordId = ARMY_ASSET_EDITOR_ID_LIMIT;
      break;
    }
  }
  /* Scan backward for a qualified candidate, wrapping below zero to 0x1000. */
  while( true ) {
    searchResult.notFound = (bool)ArmyAssetRegistry_HasIdWithFlags0100And0200(recordId);
    if (!searchResult.notFound) break;
    recordId--;
    if ((int)recordId < 0) {
      recordId = ARMY_ASSET_EDITOR_ID_LIMIT;
    }
  }
  searchResult.armyAssetId = recordId;
  return searchResult;
}


/* Address: 0x0051B5E0.
   Checks that a loaded asset is an 'arm' file of converter version 0x20008 and registers every army record
   in it (the variable-size records follow the 0x200-byte header, each starting with its byte size). A wrong
   header stores the asset path as the error detail and fails with FATAL_ERROR_ARMY_ASSET_INVALID; a failed
   registration fails with that step's error code.
*/
StatusResult ArmyAsset_PrepareRecords(ArmyAssetHeader *asset)

{
  uint32_t registrationStatusCode;
  AssetRecordCount recordsRemaining;
  ArmyAssetHeader *record;
  StatusResult registrationStatus;
  StatusResult failureStatus;

  registrationStatusCode = FATAL_ERROR_ARMY_ASSET_INVALID;
  if (((asset->recordCountHeader).common.magic == ASSET_MAGIC_ARM) &&
     ((asset->recordCountHeader).common.converterVersion == PCK_CONVERTER_ARM_00020008)) {
    recordsRemaining = (asset->recordCountHeader).recordCount;
    record = asset + 1;
    while( true ) {
      if (recordsRemaining == 0) {
        registrationStatus.failed = false;
        registrationStatus.valueOrError = registrationStatusCode;
        return registrationStatus;
      }
      registrationStatus = ArmyAssetRecord_RegisterAndRelocate((ArmyAssetRuntimeSemanticView80 *)record,asset);
      registrationStatusCode = registrationStatus.valueOrError;
      if (registrationStatus.failed) break;
      /* record += its byte size: the anchor array at +0x28 decays to record + 0x28, and the dword at +0
         (typed as the magic here) is the record's byte size */
      record = (ArmyAssetHeader *)
               ((int)(record->recordCountHeader).common.buildMetadata.assetRelativeAddressAnchor28 +
               ((record->recordCountHeader).common.magic - 0x28));
      recordsRemaining = recordsRemaining - 1;
    }
  }
  else {
    Package_SetLastErrorPath((uint16_t *)asset);
  }
  failureStatus.failed = true;
  failureStatus.valueOrError = registrationStatusCode;
  return failureStatus;
}


/* Address: 0x0051B740.
   Checks whether an army asset id is registered and enabled: returns false (CF clear) only when the record
   exists and bit 0 of its flags dword (+0x14) is set, true when it is missing or disabled.
*/
bool ArmyAssetRegistry_FindEnabledById(PckArmyAssetIdCatalog recordId)

{
  bool missingOrDisabled;
  ArmyAssetLookupResult registryLookup;

  registryLookup = ArmyAssetRegistry_FindById(recordId);
  missingOrDisabled = registryLookup.notFound;
  if (!missingOrDisabled) {
    /* flags bit 0 = enabled */
    missingOrDisabled = (((ArmyAssetRuntimeSemanticView80 *)registryLookup.recordOrError)->flags14 & 1) == 0;
  }
  return missingOrDisabled;
}


/* Address: 0x0051B770.
   Checks the 16 army-asset ids linked from an army record (dwords at +0x30) and returns true (CF set) as soon as
   one names a registered, enabled asset whose technology is fully unlocked for the faction, that has a model
   tree (+0x1C) and whose flags (+0x14) share a bit with requiredDefinitionFlags.
*/
bool ArmyAssetRecord_HasFactionUnlockedLinkedDefinition
          (FactionRuntimeIndex factionIndex,uint32_t requiredDefinitionFlags,
          ArmyAssetRecordPrefix *armyAssetRecord)

{
  ArmyAssetRecordPrefix *linkedAsset;
  int linksRemaining;
  bool technologyLocked;
  ArmyAssetLookupResult registryLookup;

  /* armyAssetRecord walks the link list in 4-byte steps, so its linkedArmyAssetIds30[0] is the current
     link; on the linked asset flags14 bit 0 = enabled */
  linksRemaining = ARMY_ASSET_LINKED_ID_COUNT;
  do {
    if (((ArmyAssetRuntimeSemanticView80 *)armyAssetRecord)->linkedArmyAssetIds30[0] != 0) {
      registryLookup = ArmyAssetRegistry_FindById(((ArmyAssetRuntimeSemanticView80 *)armyAssetRecord)->linkedArmyAssetIds30[0]);
      linkedAsset = registryLookup.recordOrError;
      if ((!registryLookup.notFound) && ((((ArmyAssetRuntimeSemanticView80 *)linkedAsset)->flags14 & 1) != 0)) {
        technologyLocked = ModelDefinitionHierarchy_AllTechnologyUnlockedForFaction
                          (factionIndex,(ModelDefinitionHierarchyNodeAddress32)linkedAsset);
        if ((!technologyLocked) &&
           ((((ArmyAssetRuntimeSemanticView80 *)linkedAsset)->selectionDetailValue1C != 0 &&
            ((((ArmyAssetRuntimeSemanticView80 *)linkedAsset)->flags14 & requiredDefinitionFlags) != 0)
            ))) {
          return true;
        }
      }
    }
    armyAssetRecord = (ArmyAssetRecordPrefix *)((uint32_t *)armyAssetRecord + 1); /* next link id */
    linksRemaining--;
    if (linksRemaining == 0) {
      return false;
    }
  } while( true );
}


/* Address: 0x00571E40.
   Frees the cached preview texture of every registered army record and re-renders the previews of the editor's
   unit- and object-placement selections into their image panels on the in-game UI root. Needed because the
   previews are drawn in the unit-placement owner faction's colours: called from the in-game keyboard dispatch
   table g_InGameKeyboardDispatchRecords (handlers 0x0056ED80 / 0x0056EDC0, commands 0x10012 / 0x1001A) after
   g_UiCommandModeGOwnerFactionIndex was cycled.
*/
void ArmyAssetRegistry_ClearPreviewTextureCacheAndRefreshSelected(uint32_t uiRootAddress)

{
  uint32_t resolvedTexture;
  int registrySlotsRemaining;
  ArmyAssetRecordPrefix **registryCursor;
  ArmyAssetRecordPrefix *registeredRecord;

  registryCursor = g_ArmyAssetRecordRegistry;
  registrySlotsRemaining = ARMY_ASSET_REGISTRY_SLOT_COUNT;
  do {
    registeredRecord = *registryCursor;
    if (registeredRecord != NULL) {
      g_MemoryApi.free((void *)((ArmyAssetRuntimeSemanticView80 *)registeredRecord)->previewTexture20);
      ((ArmyAssetRuntimeSemanticView80 *)registeredRecord)->previewTexture20 = 0;
    }
    registryCursor++;
    registrySlotsRemaining--;
  } while (registrySlotsRemaining != 0);
  resolvedTexture = ArmyAssetRegistry_ResolveOrCreatePreviewTexture(g_UiCommandModeGArmyAssetId);
  ((UiImagePanelControl *)INGAME_UI(uiRootAddress,unitPlacementPreviewImage))->textureSource =
       (GraphicsTextureSourceAsset *)resolvedTexture;
  resolvedTexture = ArmyAssetRegistry_ResolveOrCreatePreviewTexture(g_UiCommandMode4ArmyAssetId);
  ((UiImagePanelControl *)INGAME_UI(uiRootAddress,objectPlacementPreviewImage))->textureSource =
       (GraphicsTextureSourceAsset *)resolvedTexture;
  return;
}


/* Armour of one model-tree node (dword +0x60 of the definition the faction has unlocked for it) plus that of
   all its children (count at +0x08, pointers from +0x0C). */
static uint32_t ArmyAssetHierarchy_SumArmourFrom(FactionRuntimeIndex factionIndex,uint8_t *node)
{
  ModelDefinitionResult selected;
  uint32_t armourSum;
  uint32_t childIndex;
  selected = ModelDefinition_SelectFactionUnlockedLinkedDefinition
                       (factionIndex,(ModelLinkedDefinitionListAddress32)(uintptr_t)node);
  armourSum = ((ModelDefinitionRuntimeSemanticView280 *)selected.modelDefinition)->runtimeValue60;
  for (childIndex = 0; childIndex < ((ArmyModelTreeNode *)node)->childCount; childIndex++) {
    uint8_t *child = (uint8_t *)((ArmyModelTreeNode *)node)->children[childIndex];
    if (child != NULL) {
      armourSum = armourSum + ArmyAssetHierarchy_SumArmourFrom(factionIndex,child);
    }
  }
  return armourSum;
}

/* Address: 0x0051C170.
   Sums the armour (model-definition dword +0x60) over an army record's whole model tree, taking at each node
   the linked definition the faction has unlocked, so the in-game detail display shows the armour of the
   current upgrades.
*/
uint32_t ArmyAssetHierarchy_SumFactionUnlockedArmour
          (FactionRuntimeIndex factionIndex,ModelDefinitionHierarchyNodeAddress32 definitionNode)

{
  /* Rewritten from the assembly: the original walks the definition tree (child count at +0x08,
     children at +0x0C + 4*i) depth-first with frames on the machine stack. */
  return ArmyAssetHierarchy_SumArmourFrom(factionIndex,(uint8_t *)((ArmyAssetRecordPrefix *)(uintptr_t)definitionNode)->rootNodeOffsetOrPointer);
}


/* Displayed energy (Q4 dword +0x18C) of the definition the faction has unlocked for one model-tree node, plus
   that of its children when the definition's flags (+0x68) have bit 0x80 set. */
static EnergyDemandQ4 ArmyAssetHierarchy_SumEnergyFrom(FactionRuntimeIndex factionIndex,uint8_t *node)
{
  ModelDefinitionResult selected;
  EnergyDemandQ4 energySum;
  uint32_t childCount;
  uint32_t childIndex;
  selected = ModelDefinition_SelectFactionUnlockedLinkedDefinition
                       (factionIndex,(ModelLinkedDefinitionListAddress32)(uintptr_t)node);
  energySum = ((ModelDefinitionRuntimeSemanticView280 *)selected.modelDefinition)->runtimeValue18C;
  childCount = ((ArmyModelTreeNode *)node)->childCount;
  if ((((ModelDefinitionRuntimeSemanticView280 *)selected.modelDefinition)->runtimeValue68 & 0x80) == 0) {
    childCount = 0; /* only definitions with flag 0x80 contribute their children */
  }
  for (childIndex = 0; childIndex < childCount; childIndex++) {
    energySum = energySum + ArmyAssetHierarchy_SumEnergyFrom(factionIndex,(uint8_t *)((ArmyModelTreeNode *)node)->children[childIndex]);
  }
  return energySum;
}

/* Address: 0x0051C2C0.
   Sums the displayed energy value (model-definition Q4 dword +0x18C) over an army record's model tree, taking
   at each node the linked definition the faction has unlocked; children only count below definitions with
   flag 0x80. Used by the in-game detail display.
*/
EnergyDemandQ4 ArmyAssetHierarchy_SumFactionUnlockedDisplayedEnergyQ4
          (FactionRuntimeIndex factionIndex,ModelDefinitionHierarchyNodeAddress32 definitionNode)

{
  /* Rewritten from the assembly: the original walks the definition tree (child count at +0x08,
     children at +0x0C + 4*i) depth-first with frames on the machine stack. */
  return ArmyAssetHierarchy_SumEnergyFrom(factionIndex,(uint8_t *)((ArmyAssetRecordPrefix *)(uintptr_t)definitionNode)->rootNodeOffsetOrPointer);
}


/* Address: 0x00571AB0.
   Moves the editor's unit-placement army id forward out of its current run of unit ids (flag 0x0200 clear) and
   returns the next placeable unit (0x0100 set, 0x0200 clear), wrapping from 0x1000 to 0. Unlike the step
   functions this jumps between id blocks. Called from the in-game keyboard dispatch table
   g_InGameKeyboardDispatchRecords (handler 0x0056E930, command 0x10016) in unit-placement mode, and by
   ArmyAssetRegistry_NormalizeIdForFlag0100Without0200.
*/
ArmyAssetIdSearchResult ArmyAssetRegistry_FindNextFlag0100Without0200Wrapped(ArmyAssetId recordId)

{
  bool broaderAbsent;
  ArmyAssetIdSearchResult searchResult;
  
  while( true ) {
    broaderAbsent = (bool)ArmyAssetRegistry_HasIdWithoutFlag0200(recordId);
    if (broaderAbsent) break;
    recordId++;
  }
  while( true ) {
    searchResult.notFound = (bool)ArmyAssetRegistry_HasIdWithFlag0100Without0200(recordId);
    if (!searchResult.notFound) break;
    recordId++;
    if (ARMY_ASSET_EDITOR_ID_LIMIT - 1 < recordId) {
      recordId = 0;
    }
  }
  searchResult.armyAssetId = recordId;
  return searchResult;
}


/* Address: 0x00571CF0.
   Moves the editor's object-placement army id forward out of its current run of object ids (flag 0x0200 set) and
   returns the next placeable object (0x0100 and 0x0200 set), wrapping from 0x1000 to 0. Called from the in-game
   keyboard dispatch table g_InGameKeyboardDispatchRecords (handler 0x0056E930, command 0x10016) in
   object-placement mode, and by ArmyAssetRegistry_NormalizeIdForFlags0100And0200.
*/
ArmyAssetIdSearchResult ArmyAssetRegistry_FindNextFlags0100And0200Wrapped(ArmyAssetId recordId)

{
  bool broaderAbsent;
  ArmyAssetIdSearchResult searchResult;
  
  while( true ) {
    broaderAbsent = (bool)ArmyAssetRegistry_HasIdWithFlag0200(recordId);
    if (broaderAbsent) break;
    recordId++;
  }
  while( true ) {
    searchResult.notFound = (bool)ArmyAssetRegistry_HasIdWithFlags0100And0200(recordId);
    if (!searchResult.notFound) break;
    recordId++;
    if (ARMY_ASSET_EDITOR_ID_LIMIT - 1 < recordId) {
      recordId = 0;
    }
  }
  searchResult.armyAssetId = recordId;
  return searchResult;
}


/* Relocates one node of an army record's model tree and all of its children (offsets 0x0C + 4*i,
   count at +0x08) against assetBase, adding each node's model-definition build metrics (looked up by
   the id at +0x20) to the record. Returns the last lookup error, or 0. */
static uint32_t ArmyAssetRecord_RelocateModelTree
          (ArmyAssetRuntimeSemanticView80 *record,uint8_t *assetBase,uint8_t *node)
{
  BuildMetricResult metrics;
  uint32_t childCount;
  uint32_t childIndex;
  uint32_t error = 0;
  uint32_t childError;

  metrics = ModelDefinitionRegistry_FindBuildMetricTupleById(((ArmyModelTreeNode *)node)->linkedDefinitionIds[0]);
  if (metrics.notFound) {
    error = (uint32_t)metrics.metric0;
  }
  else {
    record->relocationPointerOrOffset2C = record->relocationPointerOrOffset2C + (uint32_t)metrics.metric0;
    record->relocationValue24 = record->relocationValue24 + metrics.metric1;
    record->relocationValue28 = record->relocationValue28 + metrics.metric2;
  }
  childCount = ((ArmyModelTreeNode *)node)->childCount;
  for (childIndex = 0; childIndex < childCount; childIndex++) {
    uint8_t **child = (uint8_t **)&((ArmyModelTreeNode *)node)->children[childIndex];
    *child = *child + (uintptr_t)assetBase;
    childError = ArmyAssetRecord_RelocateModelTree(record,assetBase,*child);
    if (childError != 0) {
      error = childError;
    }
  }
  return error;
}

/* Address: 0x0051B4A0.
   Registers a loaded army record: rejects an id that is already registered (FATAL_ERROR_ARMY_ID_DUPLICATE), puts
   the record into the first free slot of the 768-slot army registry (FATAL_ERROR_ARMY_REGISTRY_FULL when none is
   left), turns its model-tree offsets into pointers against assetBase and adds the build metrics of every node's
   model definition to the record. CF set on failure with the error code in EAX.
*/
StatusResult ArmyAssetRecord_RegisterAndRelocate(ArmyAssetRuntimeSemanticView80 *record,ArmyAssetHeader *assetBase)

{
  /* Rewritten from the assembly (0x0051B4A0-0x0051B5D8): the model tree walk kept its recursion on
     the machine stack, which the decompiler could not express. */
  ArmyAssetRecordPrefix **slot;
  int slotsRemaining;
  ArmyAssetLookupResult existing;
  StatusResult status;

  existing = ArmyAssetRegistry_FindById(record->registryId);
  if (!existing.notFound) {
    g_WideNumberFormatUtf16
              (WIDE_FORMAT_WRITE_TERMINATOR,0,10,1,record->registryId,g_PackageLastErrorPath);
    status.failed = true;
    status.valueOrError = FATAL_ERROR_ARMY_ID_DUPLICATE;
    return status;
  }
  slot = g_ArmyAssetRecordRegistry;
  for (slotsRemaining = ARMY_ASSET_REGISTRY_SLOT_COUNT; slotsRemaining != 0; slotsRemaining--) {
    if (*slot == NULL) {
      uint32_t error = 0;
      *slot = (ArmyAssetRecordPrefix *)record;
      if (record->rootNodeOffsetOrPointer != 0) {
        record->rootNodeOffsetOrPointer = record->rootNodeOffsetOrPointer + (uint32_t)(uintptr_t)assetBase;
        error = ArmyAssetRecord_RelocateModelTree
                          (record,(uint8_t *)assetBase,(uint8_t *)(uintptr_t)record->rootNodeOffsetOrPointer);
      }
      status.failed = error != 0;
      status.valueOrError = error;
      return status;
    }
    slot = slot + 1;
  }
  g_WideNumberFormatUtf16(WIDE_FORMAT_WRITE_TERMINATOR,0,10,1,ARMY_ASSET_REGISTRY_SLOT_COUNT,g_PackageLastErrorPath);
  status.failed = true;
  status.valueOrError = FATAL_ERROR_ARMY_REGISTRY_FULL;
  return status;
}


/* Address: 0x00571D90.
   Returns the preview texture of a registered army asset for the editor's placement panels. The texture is
   cached in the record (previewTexture20); on the first request it is rendered in the unit-placement
   owner faction's colours (faction 0 for ids from 400 up). Returns 0 for an unknown id or a failed render.
   Called directly by the in-game keyboard dispatch handlers (g_InGameKeyboardDispatchRecords),
   InGameUiCommandRuntime_ApplyInteractionSubsystemActiveState and
   ArmyAssetRegistry_ClearPreviewTextureCacheAndRefreshSelected.
*/
uint32_t ArmyAssetRegistry_ResolveOrCreatePreviewTexture(uint32_t armyAssetRegistryId)

{
  ArmyAssetRecordPrefix *registeredRecord;
  int registrySlotsRemaining;
  ArmyAssetRecordPrefix **registryCursor;
  ArmyPreviewTextureResult renderResult;
  FactionRuntimeIndex factionIndex;

  registryCursor = g_ArmyAssetRecordRegistry;
  registrySlotsRemaining = ARMY_ASSET_REGISTRY_SLOT_COUNT;
  while ((registeredRecord = *registryCursor, registeredRecord == NULL ||
         (armyAssetRegistryId != registeredRecord->registryId))) {
    registryCursor++;
    registrySlotsRemaining--;
    if (registrySlotsRemaining == 0) {
      return 0;
    }
  }
  if (((ArmyAssetRuntimeSemanticView80 *)registeredRecord)->previewTexture20 == 0) {
    factionIndex = g_UiCommandModeGOwnerFactionIndex;
    if (ARMY_ASSET_NEUTRAL_PREVIEW_FIRST_ID - 1 < armyAssetRegistryId) {
      factionIndex = 0;
    }
    renderResult = ArmyRuntime_RenderPreviewTexture
                      (INGAME_UI(g_InGameRuntimeRoot,modePreviewPageStack)->layoutHeight,
                       INGAME_UI(g_InGameRuntimeRoot,modePreviewPageStack)->layoutHeight,
                       factionIndex,armyAssetRegistryId,&g_InGameRuntimeRoot->worldRuntime0A30);
    if (renderResult.failed) {
      return 0;
    }
    ((ArmyAssetRuntimeSemanticView80 *)registeredRecord)->previewTexture20 = (uint32_t)renderResult.previewTexture;
    return (uint32_t)renderResult.previewTexture;
  }
  return ((ArmyAssetRuntimeSemanticView80 *)registeredRecord)->previewTexture20;
}


/* Address: 0x0051B6D0.
   Looks an army asset up by its registry id in the 768-slot army registry and returns the record (CF clear).
   An unknown id is written as decimal text to g_PackageLastErrorPath and fails with FATAL_ERROR_ARMY_ID_NOT_FOUND.
*/
ArmyAssetLookupResult ArmyAssetRegistry_FindById(PckArmyAssetIdCatalog registryId)

{
  ArmyAssetRecordPrefix *matchedRecord;
  int registrySlotsRemaining;
  ArmyAssetRecordPrefix **registryCursor;
  ArmyAssetLookupResult failureResult;
  ArmyAssetLookupResult successResult;

  registryCursor = g_ArmyAssetRecordRegistry;
  registrySlotsRemaining = ARMY_ASSET_REGISTRY_SLOT_COUNT;
  while ((matchedRecord = *registryCursor, matchedRecord == NULL ||
         (matchedRecord->registryId != registryId))) {
    registryCursor = registryCursor + 1;
    registrySlotsRemaining--;
    if (registrySlotsRemaining == 0) {
      g_WideNumberFormatUtf16
                (WIDE_FORMAT_WRITE_TERMINATOR,0,10,1,registryId,g_PackageLastErrorPath);
      failureResult.notFound = true;
      failureResult.recordOrError = (ArmyAssetRecordPrefix *)FATAL_ERROR_ARMY_ID_NOT_FOUND;
      return failureResult;
    }
  }
  successResult.notFound = false;
  successResult.recordOrError = matchedRecord;
  return successResult;
}


/* Address: 0x00571910.
   Returns 0 (CF clear) when some registered army record with this id is a unit, i.e. has flag 0x0200 of its flags
   dword (+0x14) clear; 1 (CF set) otherwise. Several records may share an id, so the scan goes on past a match
   with the wrong flags. Predicate of the editor's unit-placement id searches
   (FindNext/FindPrevious/Step*Flag0100Without0200).
*/
uint8_t ArmyAssetRegistry_HasIdWithoutFlag0200(ArmyAssetId recordId)

{
  int registrySlotsRemaining;
  ArmyAssetRecordPrefix **registryCursor;
  ArmyAssetRecordPrefix *candidateAsset;
  
  registryCursor = g_ArmyAssetRecordRegistry;
  registrySlotsRemaining = ARMY_ASSET_REGISTRY_SLOT_COUNT;
  while (((candidateAsset = *registryCursor, candidateAsset == NULL ||
          (recordId != candidateAsset->registryId)) ||
         ((((ArmyAssetRuntimeSemanticView80 *)candidateAsset)->flags14 & ARMY_ASSET_FLAG_EDITOR_OBJECT) != 0))) {
    registryCursor++;
    registrySlotsRemaining--;
    if (registrySlotsRemaining == 0) {
      return 1;
    }
  }
  return 0;
}


/* Address: 0x00571B50.
   Returns 0 (CF clear) when some registered army record with this id is an object, i.e. has flag 0x0200 of its
   flags dword (+0x14) set; 1 (CF set) otherwise. Predicate of the editor's object-placement id searches
   (FindNext/FindPrevious/Step*Flags0100And0200).
*/
uint8_t ArmyAssetRegistry_HasIdWithFlag0200(ArmyAssetId recordId)

{
  int registrySlotsRemaining;
  ArmyAssetRecordPrefix **registryCursor;
  ArmyAssetRecordPrefix *candidateAsset;
  
  registryCursor = g_ArmyAssetRecordRegistry;
  registrySlotsRemaining = ARMY_ASSET_REGISTRY_SLOT_COUNT;
  while (((candidateAsset = *registryCursor, candidateAsset == NULL ||
          (recordId != candidateAsset->registryId)) ||
         ((((ArmyAssetRuntimeSemanticView80 *)candidateAsset)->flags14 & ARMY_ASSET_FLAG_EDITOR_OBJECT) == 0))) {
    registryCursor++;
    registrySlotsRemaining--;
    if (registrySlotsRemaining == 0) {
      return 1;
    }
  }
  return 0;
}


/* Address: 0x00571980.
   Returns 0 (CF clear) when some registered army record with this id is a placeable unit (flags dword +0x14 with
   0x0100 set and 0x0200 clear); 1 (CF set) otherwise. The id test of the editor's unit-placement list
   (NormalizeIdFor/FindNext/FindPrevious/Step*Flag0100Without0200).
*/
uint8_t ArmyAssetRegistry_HasIdWithFlag0100Without0200(ArmyAssetId recordId)

{
  int registrySlotsRemaining;
  ArmyAssetRecordPrefix **registryCursor;
  ArmyAssetRecordPrefix *candidateAsset;
  
  registryCursor = g_ArmyAssetRecordRegistry;
  registrySlotsRemaining = ARMY_ASSET_REGISTRY_SLOT_COUNT;
  while ((((candidateAsset = *registryCursor, candidateAsset == NULL ||
           (recordId != candidateAsset->registryId)) ||
          ((((ArmyAssetRuntimeSemanticView80 *)candidateAsset)->flags14 & ARMY_ASSET_FLAG_EDITOR_PLACEABLE) == 0)) ||
         ((((ArmyAssetRuntimeSemanticView80 *)candidateAsset)->flags14 & ARMY_ASSET_FLAG_EDITOR_OBJECT) != 0))) {
    registryCursor++;
    registrySlotsRemaining--;
    if (registrySlotsRemaining == 0) {
      return 1;
    }
  }
  return 0;
}


/* Address: 0x00571BC0.
   Returns 0 (CF clear) when some registered army record with this id is a placeable object (flags dword +0x14
   with 0x0100 and 0x0200 set); 1 (CF set) otherwise. The id test of the editor's object-placement list
   (NormalizeIdFor/FindNext/FindPrevious/Step*Flags0100And0200).
*/
uint8_t ArmyAssetRegistry_HasIdWithFlags0100And0200(ArmyAssetId recordId)

{
  int registrySlotsRemaining;
  ArmyAssetRecordPrefix **registryCursor;
  ArmyAssetRecordPrefix *candidateAsset;
  
  registryCursor = g_ArmyAssetRecordRegistry;
  registrySlotsRemaining = ARMY_ASSET_REGISTRY_SLOT_COUNT;
  while ((((candidateAsset = *registryCursor, candidateAsset == NULL ||
           (recordId != candidateAsset->registryId)) ||
          ((((ArmyAssetRuntimeSemanticView80 *)candidateAsset)->flags14 & ARMY_ASSET_FLAG_EDITOR_PLACEABLE) == 0)) ||
         ((((ArmyAssetRuntimeSemanticView80 *)candidateAsset)->flags14 & ARMY_ASSET_FLAG_EDITOR_OBJECT) == 0))) {
    registryCursor++;
    registrySlotsRemaining--;
    if (registrySlotsRemaining == 0) {
      return 1;
    }
  }
  return 0;
}


/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/src/assets/army/catalog.cpp
 * Reverse engineering by idkFoxes 2026
 */

#include <thandor/assets/army/catalog.h>
#include <thandor/thandor.h>

/* Module data. */

/* UTF-16 L"army0000.gfx" after g_AiCommandGenerationRetainedTarget; no code reference found */
THANDOR_ALIGN(4) uint16_t g_UnreferencedArmyTexturePathUtf16[13] = {'a', 'r', 'm', 'y', '0', '0', '0', '0', '.', 'g', 'f', 'x', 0};

/* char "ARMY" after the army0000.gfx string; no code reference found; followed by 0x90 fill */
THANDOR_ALIGN(4) char g_UnreferencedArmyTag[5] = "ARMY";

ArmyAssetRecordPrefix *g_ArmyAssetRecordRegistry[768] = {0};

/* Implementation ownership: assets/army/catalog. */

/* Checks that a loaded asset is an 'arm' file of converter version 0x20008 and registers every army record
   in it (the variable-size records follow the 0x200-byte header, each starting with its byte size). A wrong
   header stores the asset path as the error detail and fails with FATAL_ERROR_ARMY_ASSET_INVALID; a failed
   registration fails with that step's error code. Returns 0 on success, otherwise that (non-zero) error code.
   (The original's success return value, the preset error code or the last registration result, was read by no
   caller.)
*/
uint32_t ArmyAsset_PrepareRecords(ArmyAssetHeader *asset)

{
  uint32_t registrationStatusCode;
  AssetRecordCount recordsRemaining;
  ArmyAssetRecord *record;

  if (asset->recordCountHeader.common.magic == ASSET_MAGIC_ARM &&
      asset->recordCountHeader.common.converterVersion == PCK_CONVERTER_ARM_00020008) {
    record = (ArmyAssetRecord *)(asset + 1);
    for (recordsRemaining = asset->recordCountHeader.recordCount; recordsRemaining != 0; recordsRemaining--) {
      registrationStatusCode = ArmyAssetRecord_RegisterAndRelocate(record,asset);
      if (registrationStatusCode != 0) return registrationStatusCode;
      record = (ArmyAssetRecord *)((uint8_t *)record + record->byteSize);
    }
    return 0;
  }
  Package_SetLastErrorPath((uint16_t *)asset);
  return FATAL_ERROR_ARMY_ASSET_INVALID;
}

/* Checks whether an army asset id is registered and enabled: returns false only when the record exists
   and bit 0 (ARMY_ASSET_FLAG_ENABLED) of its flags is set, true when it is missing or disabled.
*/
Bool8 ArmyAssetRegistry_FindEnabledById(PckArmyAssetIdCatalog recordId)

{
  ArmyAssetRecordPrefix *registeredRecord;

  if (ArmyAssetRegistry_FindById(recordId,&registeredRecord) != 0) {
    return true; /* missing */
  }
  return (((ArmyAssetRecord *)registeredRecord)->flags & ARMY_ASSET_FLAG_ENABLED) == 0; /* disabled */
}

/* Checks the 16 army-asset ids linked from an army record (linkedArmyAssetIds) and returns true as soon as one
   names a registered, enabled asset whose technology is fully unlocked for the faction (every definition of its
   model tree, rootNodeOffsetOrPointer), whose selectionDetailValue is non-zero and whose flags share a bit with
   requiredDefinitionFlags.
*/
Bool8 ArmyAssetRecord_HasFactionUnlockedLinkedDefinition
          (FactionRuntimeIndex factionIndex,uint32_t requiredDefinitionFlags,
          ArmyAssetRecordPrefix *armyAssetRecord)

{
  ArmyAssetRecord *linkedAsset;
  int linksRemaining;
  Bool8 technologyLocked;
  uint32_t lookupError;
  ArmyAssetRecordPrefix *linkedRecord;

  /* armyAssetRecord walks the link list in 4-byte steps, so its linkedArmyAssetIds[0] is the current link */
  for (linksRemaining = ARMY_ASSET_LINKED_ID_COUNT; linksRemaining != 0; linksRemaining--) {
    if (((ArmyAssetRecord *)armyAssetRecord)->linkedArmyAssetIds[0] != 0) {
      lookupError = ArmyAssetRegistry_FindById(((ArmyAssetRecord *)armyAssetRecord)->linkedArmyAssetIds[0],
                                               &linkedRecord);
      linkedAsset = (ArmyAssetRecord *)linkedRecord;
      if (lookupError == 0 && (linkedAsset->flags & ARMY_ASSET_FLAG_ENABLED) != 0) {
        technologyLocked = ModelDefinitionHierarchy_AllTechnologyUnlockedForFaction
                          (factionIndex,(ModelDefinitionHierarchyNodeAddress32)linkedAsset);
        if (!technologyLocked && linkedAsset->selectionDetailValue != 0 &&
            (linkedAsset->flags & requiredDefinitionFlags) != 0) {
          return true;
        }
      }
    }
    armyAssetRecord = (ArmyAssetRecordPrefix *)((uint32_t *)armyAssetRecord + 1); /* next link id */
  }
  return false;
}

/* Armour of one model-tree node (maximumHealth of the definition the faction has unlocked for it) plus that of
   all its children (childCount, children). */
static uint32_t ArmyAssetHierarchy_SumArmourFrom(FactionRuntimeIndex factionIndex,ArmyModelTreeNode *node)
{
  ModelDefinitionRecordPrefix *selected;
  uint32_t armourSum;
  uint32_t childIndex;
  selected = ModelDefinition_SelectFactionUnlockedLinkedDefinition
                       (factionIndex,(uintptr_t)node);
  armourSum = ((ModelDefinition *)selected)->maximumHealth;
  for (childIndex = 0; childIndex < node->childCount; childIndex++) {
    ArmyModelTreeNode *child = node->children[childIndex];
    if (child != NULL) {
      armourSum = armourSum + ArmyAssetHierarchy_SumArmourFrom(factionIndex,child);
    }
  }
  return armourSum;
}

/* Sums the armour (ModelDefinition.maximumHealth) over an army record's whole model tree, taking at each node
   the linked definition the faction has unlocked, so the in-game detail display shows the armour of the
   current upgrades.
*/
uint32_t ArmyAssetHierarchy_SumFactionUnlockedArmour
          (FactionRuntimeIndex factionIndex,ModelDefinitionHierarchyNodeAddress32 definitionNode)

{
  /* Depth-first walk of the model tree (childCount, children[]), written as a recursion. */
  return ArmyAssetHierarchy_SumArmourFrom(
       factionIndex,(ArmyModelTreeNode *)((ArmyAssetRecordPrefix *)(uintptr_t)definitionNode)->rootNodeOffsetOrPointer); /* 5f-format: ArmyAssetRecordPrefix.rootNodeOffsetOrPointer */
}

/* Displayed energy (Q4 energyLoadQ4) of the definition the faction has unlocked for one model-tree node, plus
   that of its children when the definition's modelFlags have bit 0x80 set. */
static EnergyDemandQ4 ArmyAssetHierarchy_SumEnergyFrom(FactionRuntimeIndex factionIndex,ArmyModelTreeNode *node)
{
  ModelDefinitionRecordPrefix *selected;
  EnergyDemandQ4 energySum;
  uint32_t childCount;
  uint32_t childIndex;
  selected = ModelDefinition_SelectFactionUnlockedLinkedDefinition
                       (factionIndex,(uintptr_t)node);
  energySum = ((ModelDefinition *)selected)->energyLoadQ4;
  childCount = node->childCount;
  if ((((ModelDefinition *)selected)->modelFlags &
       MODEL_DEFINITION_FLAG_COUNT_ATTACHED_ENERGY) == 0) {
    childCount = 0; /* only definitions with this flag contribute their children */
  }
  for (childIndex = 0; childIndex < childCount; childIndex++) {
    energySum = energySum + ArmyAssetHierarchy_SumEnergyFrom(factionIndex,node->children[childIndex]);
  }
  return energySum;
}

/* Sums the displayed energy value (ModelDefinition.energyLoadQ4, Q4) over an army record's model tree, taking
   at each node the linked definition the faction has unlocked; children only count below definitions with
   flag 0x80. Used by the in-game detail display.
*/
EnergyDemandQ4 ArmyAssetHierarchy_SumFactionUnlockedDisplayedEnergyQ4
          (FactionRuntimeIndex factionIndex,ModelDefinitionHierarchyNodeAddress32 definitionNode)

{
  /* Depth-first walk of the model tree (childCount, children[]), written as a recursion. */
  return ArmyAssetHierarchy_SumEnergyFrom(
       factionIndex,(ArmyModelTreeNode *)((ArmyAssetRecordPrefix *)(uintptr_t)definitionNode)->rootNodeOffsetOrPointer); /* 5f-format: ArmyAssetRecordPrefix.rootNodeOffsetOrPointer */
}

/* Relocates one node of an army record's model tree and all of its children (children[], childCount)
   against assetBase, adding each node's model-definition build costs (looked up by linkedDefinitionIds[0])
   to the record. Returns the last lookup error, or 0. */
static uint32_t ArmyAssetRecord_RelocateModelTree
          (ArmyAssetRecord *record,uint8_t *assetBase,ArmyModelTreeNode *node)
{
  uint32_t energyLoadQ4;
  uint32_t buildTicks;
  uint32_t xeniteCostQ4;
  uint32_t childCount;
  uint32_t childIndex;
  uint32_t error;
  uint32_t childError;

  error = ModelDefinitionRegistry_FindBuildCostsById
                    (node->linkedDefinitionIds[0],&energyLoadQ4,&buildTicks,&xeniteCostQ4);
  if (error == 0) {
    record->energyLoadQ4 = record->energyLoadQ4 + energyLoadQ4;
    record->buildTicks = record->buildTicks + buildTicks;
    record->xeniteCostQ4 = record->xeniteCostQ4 + xeniteCostQ4;
  }
  childCount = node->childCount;
  for (childIndex = 0; childIndex < childCount; childIndex++) {
    /* serialized offset from assetBase -> pointer */
    node->children[childIndex] = (ArmyModelTreeNode *)((uint8_t *)node->children[childIndex] + (uintptr_t)assetBase);
    childError = ArmyAssetRecord_RelocateModelTree(record,assetBase,node->children[childIndex]);
    if (childError != 0) {
      error = childError;
    }
  }
  return error;
}

/* Registers a loaded army record: rejects an id that is already registered (FATAL_ERROR_ARMY_ID_DUPLICATE), puts
   the record into the first free slot of the 768-slot army registry (FATAL_ERROR_ARMY_REGISTRY_FULL when none is
   left), turns its model-tree offsets into pointers against assetBase and adds the build costs of every node's
   model definition to the record. Returns 0 on success, otherwise the FATAL_ERROR_* code (a failed
   model-definition lookup returns that lookup's error, after the record was already put into its slot).
*/
uint32_t ArmyAssetRecord_RegisterAndRelocate(ArmyAssetRecord *record,ArmyAssetHeader *assetBase)

{
  /* The model tree walk is a recursion (ArmyAssetRecord_RelocateModelTree). */
  int slotIndex;
  ArmyAssetRecordPrefix *existing;
  uint32_t error;

  if (ArmyAssetRegistry_FindById(record->registryId,&existing) == 0) {
    g_WideNumberFormatUtf16
              (WIDE_FORMAT_WRITE_TERMINATOR,0,10,1,record->registryId,g_PackageLastErrorPath);
    return FATAL_ERROR_ARMY_ID_DUPLICATE;
  }
  for (slotIndex = 0; slotIndex < ARMY_ASSET_REGISTRY_SLOT_COUNT; slotIndex++) {
    if (g_ArmyAssetRecordRegistry[slotIndex] != NULL) {
      continue;
    }
    g_ArmyAssetRecordRegistry[slotIndex] = (ArmyAssetRecordPrefix *)record;
    error = 0;
    if (record->rootNodeOffsetOrPointer != 0) {
      /* serialized offset from assetBase -> pointer */
      record->rootNodeOffsetOrPointer = record->rootNodeOffsetOrPointer + (uint32_t)(uintptr_t)assetBase;
      error = ArmyAssetRecord_RelocateModelTree
                        (record,(uint8_t *)assetBase,(ArmyModelTreeNode *)(uintptr_t)record->rootNodeOffsetOrPointer);
    }
    return error;
  }
  g_WideNumberFormatUtf16(WIDE_FORMAT_WRITE_TERMINATOR,0,10,1,ARMY_ASSET_REGISTRY_SLOT_COUNT,g_PackageLastErrorPath);
  return FATAL_ERROR_ARMY_REGISTRY_FULL;
}

/* The first registered army asset record with this id in the 768-slot army registry, or NULL when there is
   none. No side effects (ArmyAssetRegistry_FindById also reports a miss). */
ArmyAssetRecordPrefix *ArmyAssetRegistry_FindRecordById(PckArmyAssetIdCatalog registryId)

{
  ArmyAssetRecordPrefix *candidateRecord;
  int slotIndex;

  for (slotIndex = 0; slotIndex < ARMY_ASSET_REGISTRY_SLOT_COUNT; slotIndex++) {
    candidateRecord = g_ArmyAssetRecordRegistry[slotIndex];
    if (candidateRecord != NULL && candidateRecord->registryId == registryId) {
      return candidateRecord;
    }
  }
  return NULL;
}

/* Looks an army asset up by its registry id in the 768-slot army registry. Returns 0 and stores the record in
   *outRecord; an unknown id is written as decimal text to g_PackageLastErrorPath and returns
   FATAL_ERROR_ARMY_ID_NOT_FOUND.
   Original quirk: on failure *outRecord is set to FATAL_ERROR_ARMY_ID_NOT_FOUND cast to a pointer (the original
   returned the error code where the record goes). Several callers read the record without checking the status,
   so this value is kept.
*/
uint32_t ArmyAssetRegistry_FindById(PckArmyAssetIdCatalog registryId,ArmyAssetRecordPrefix **outRecord)

{
  ArmyAssetRecordPrefix *candidateRecord;

  candidateRecord = ArmyAssetRegistry_FindRecordById(registryId);
  if (candidateRecord != NULL) {
    *outRecord = candidateRecord;
    return 0;
  }
  g_WideNumberFormatUtf16(WIDE_FORMAT_WRITE_TERMINATOR,0,10,1,registryId,g_PackageLastErrorPath);
  *outRecord = (ArmyAssetRecordPrefix *)FATAL_ERROR_ARMY_ID_NOT_FOUND;
  return FATAL_ERROR_ARMY_ID_NOT_FOUND;
}

/* Returns 0 when some registered army record with this id is a unit, i.e. has flag 0x0200 of its flags clear;
   1 otherwise. Several records may share an id, so the scan goes on past a match
   with the wrong flags. Predicate of the editor's unit-placement id searches
   (FindNext/FindPrevious/Step*Flag0100Without0200).
*/
uint8_t ArmyAssetRegistry_HasNoUnitWithId(ArmyAssetId recordId)

{
  int slotIndex;
  ArmyAssetRecord *candidateAsset;

  for (slotIndex = 0; slotIndex < ARMY_ASSET_REGISTRY_SLOT_COUNT; slotIndex++) {
    candidateAsset = (ArmyAssetRecord *)g_ArmyAssetRecordRegistry[slotIndex];
    if (candidateAsset != NULL && recordId == candidateAsset->registryId &&
        (candidateAsset->flags & ARMY_ASSET_FLAG_EDITOR_OBJECT) == 0) {
      return 0; /* found a unit */
    }
  }
  return 1;
}

/* Returns 0 when some registered army record with this id is an object, i.e. has flag 0x0200 of its flags set;
   1 otherwise. Predicate of the editor's object-placement id searches
   (FindNext/FindPrevious/Step*Flags0100And0200).
*/
uint8_t ArmyAssetRegistry_HasNoObjectWithId(ArmyAssetId recordId)

{
  int slotIndex;
  ArmyAssetRecord *candidateAsset;

  for (slotIndex = 0; slotIndex < ARMY_ASSET_REGISTRY_SLOT_COUNT; slotIndex++) {
    candidateAsset = (ArmyAssetRecord *)g_ArmyAssetRecordRegistry[slotIndex];
    if (candidateAsset != NULL && recordId == candidateAsset->registryId &&
        (candidateAsset->flags & ARMY_ASSET_FLAG_EDITOR_OBJECT) != 0) {
      return 0; /* found an object */
    }
  }
  return 1;
}

/* Returns 0 when some registered army record with this id is a placeable unit (flags with 0x0100 set and 0x0200
   clear); 1 otherwise. The id test of the editor's unit-placement list
   (NormalizeIdFor/FindNext/FindPrevious/Step*Flag0100Without0200).
*/
uint8_t ArmyAssetRegistry_HasNoPlaceableUnitWithId(ArmyAssetId recordId)

{
  int slotIndex;
  ArmyAssetRecord *candidateAsset;

  for (slotIndex = 0; slotIndex < ARMY_ASSET_REGISTRY_SLOT_COUNT; slotIndex++) {
    candidateAsset = (ArmyAssetRecord *)g_ArmyAssetRecordRegistry[slotIndex];
    if (candidateAsset != NULL && recordId == candidateAsset->registryId &&
        (candidateAsset->flags & ARMY_ASSET_FLAG_EDITOR_PLACEABLE) != 0 &&
        (candidateAsset->flags & ARMY_ASSET_FLAG_EDITOR_OBJECT) == 0) {
      return 0; /* found a placeable unit */
    }
  }
  return 1;
}

/* Returns 0 when some registered army record with this id is a placeable object (flags with 0x0100 and 0x0200
   set); 1 otherwise. The id test of the editor's object-placement list
   (NormalizeIdFor/FindNext/FindPrevious/Step*Flags0100And0200).
*/
uint8_t ArmyAssetRegistry_HasNoPlaceableObjectWithId(ArmyAssetId recordId)

{
  int slotIndex;
  ArmyAssetRecord *candidateAsset;

  for (slotIndex = 0; slotIndex < ARMY_ASSET_REGISTRY_SLOT_COUNT; slotIndex++) {
    candidateAsset = (ArmyAssetRecord *)g_ArmyAssetRecordRegistry[slotIndex];
    if (candidateAsset != NULL && recordId == candidateAsset->registryId &&
        (candidateAsset->flags & ARMY_ASSET_FLAG_EDITOR_PLACEABLE) != 0 &&
        (candidateAsset->flags & ARMY_ASSET_FLAG_EDITOR_OBJECT) != 0) {
      return 0; /* found a placeable object */
    }
  }
  return 1;
}

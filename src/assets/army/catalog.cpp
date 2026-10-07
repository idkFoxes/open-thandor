/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/src/assets/army/catalog.cpp
 * Reverse engineering by idkFoxes 2026
 */

#include <thandor/assets/army/catalog.h>
#include <thandor/thandor.h>
#include <thandor/platform/bootstrap/image.h>
#include <thandor/assets/record_bytes.h>

/* Module data. */

ArmyAssetRecordPrefix *g_ArmyAssetRecordRegistry[768] = {};

/* Fixed name for the fatal-error box when an ARM asset is invalid (the asset's own path is not known here). */
static uint16_t s_ArmyAssetErrorName[] = {'*', '.', 'a', 'r', 'm', 0}; /* L"*.arm" */

/* Checks that a loaded asset (assetByteCount bytes) is an 'arm' file of converter version 0x20008 and registers
   every army record in it (the variable-size records follow the 0x200-byte header, each starting with its byte
   size). A wrong header, or a record that is shorter than its prefix or does not fit into the asset, stores
   "*.arm" as the error detail and fails with FATAL_ERROR_ARMY_ASSET_INVALID; a failed registration fails with
   that step's error code. Returns 0 on success, otherwise that (non-zero) error code.
   (The original's success return value, the preset error code or the last registration result, was read by no
   caller.)
*/
uint32_t ArmyAsset_PrepareRecords(ArmyAssetHeader *asset,uint32_t assetByteCount)

{
  uint32_t registrationStatusCode;
  AssetRecordCount recordsRemaining;
  ArmyAssetRecord *record;
  uint32_t bytesLeft;

  /* The original trusts the asset size and every record's byteSize; bounded here because the walk follows file
     data (a byteSize of 0 loops on one record, a large one walks past the asset). */
  if (assetByteCount >= sizeof(ArmyAssetHeader) &&
      asset->recordCountHeader.common.magic == ASSET_MAGIC_ARM &&
      asset->recordCountHeader.common.converterVersion == PCK_CONVERTER_ARM_00020008) {
    record = Asset_RecordAfter<ArmyAssetRecord>(asset);
    bytesLeft = assetByteCount - (uint32_t)sizeof(ArmyAssetHeader);
    for (recordsRemaining = asset->recordCountHeader.recordCount; recordsRemaining != 0; recordsRemaining--) {
      if (bytesLeft < sizeof(ArmyAssetRecord) || record->byteSize < sizeof(ArmyAssetRecordPrefix) ||
          record->byteSize > bytesLeft) {
        Thandor_Log("ArmyAsset_PrepareRecords: record at offset 0x%X (byteSize 0x%X) does not fit the asset of "
                    "0x%X bytes, rejected",(uint32_t)Asset_ByteDistance(record,asset),
                    bytesLeft < sizeof(ArmyAssetRecordPrefix) ? 0u : record->byteSize,assetByteCount);
        Package_SetLastErrorPath(s_ArmyAssetErrorName);
        return FATAL_ERROR_ARMY_ASSET_INVALID;
      }
      registrationStatusCode = ArmyAssetRecord_RegisterAndRelocate(record,asset);
      if (registrationStatusCode != 0) return registrationStatusCode;
      bytesLeft = bytesLeft - record->byteSize;
      record = Asset_RecordAt<ArmyAssetRecord>(record,record->byteSize);
    }
    return 0;
  }
  /* The original passes the asset header itself as the error path; replaced by a fixed name here because the
     header words are no text (units >= 0x8000 become rich-text pointer codes in the fatal-error box). */
  Package_SetLastErrorPath(s_ArmyAssetErrorName);
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
  return (ArmyAssetRecord_FromPrefix(registeredRecord)->flags & ARMY_ASSET_FLAG_ENABLED) == 0; /* disabled */
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
    if (ArmyAssetRecord_FromPrefix(armyAssetRecord)->linkedArmyAssetIds[0] != 0) {
      lookupError = ArmyAssetRegistry_FindById(ArmyAssetRecord_FromPrefix(armyAssetRecord)->linkedArmyAssetIds[0],
                                               &linkedRecord);
      linkedAsset = ArmyAssetRecord_FromPrefix(linkedRecord);
      if (lookupError == 0 && (linkedAsset->flags & ARMY_ASSET_FLAG_ENABLED) != 0) {
        technologyLocked = ModelDefinitionHierarchy_AllTechnologyUnlockedForFaction
                          (factionIndex,(ModelDefinitionHierarchyNodeAddress32)linkedAsset);
        if (!technologyLocked && linkedAsset->selectionDetailValue != 0 &&
            (linkedAsset->flags & requiredDefinitionFlags) != 0) {
          return true;
        }
      }
    }
    armyAssetRecord = Asset_RecordAt<ArmyAssetRecordPrefix>(armyAssetRecord,sizeof(uint32_t)); /* next link id */
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
  armourSum = ModelDefinition_FromPrefix(selected)->maximumHealth;
  for (childIndex = 0; childIndex < node->childCount; childIndex++) {
    /* children are always relocated pointers (ArmyAssetRecord_RelocateModelTree), never NULL */
    armourSum = armourSum + ArmyAssetHierarchy_SumArmourFrom(factionIndex,node->children[childIndex]);
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
       factionIndex,Thandor_U32ToPointer<ArmyModelTreeNode>(reinterpret_cast<ArmyAssetRecordPrefix *>(definitionNode)->rootNodeOffsetOrPointer)); /* 5f-format: ArmyAssetRecordPrefix.rootNodeOffsetOrPointer; definitionNode is the record's address */
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
  energySum = ModelDefinition_FromPrefix(selected)->energyLoadQ4;
  childCount = node->childCount;
  if (!Any(ModelDefinition_FromPrefix(selected)->modelFlags &
       MODEL_DEFINITION_FLAG_COUNT_ATTACHED_ENERGY)) {
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
       factionIndex,Thandor_U32ToPointer<ArmyModelTreeNode>(reinterpret_cast<ArmyAssetRecordPrefix *>(definitionNode)->rootNodeOffsetOrPointer)); /* 5f-format: ArmyAssetRecordPrefix.rootNodeOffsetOrPointer; definitionNode is the record's address */
}

/* Relocates one node of an army record's model tree and all of its children (children[], childCount)
   against assetBase, adding each node's model-definition build costs (looked up by linkedDefinitionIds[0])
   to the record. Returns the last lookup error, or 0. The original trusts childCount and the nesting depth;
   bounded here because the walk follows file data: a node with more children than children[] holds or a tree
   deeper than ARMY_MODEL_TREE_MAX_DEPTH (a cyclic offset) fails with FATAL_ERROR_ARMY_ASSET_INVALID. */
static constexpr int ARMY_MODEL_TREE_MAX_DEPTH = 64;
static uint32_t ArmyAssetRecord_RelocateModelTree
          (ArmyAssetRecord *record,ArmyAssetHeader *assetBase,ArmyModelTreeNode *node,uint32_t depth)
{
  uint32_t energyLoadQ4;
  uint32_t buildTicks;
  uint32_t xeniteCostQ4;
  uint32_t childCount;
  uint32_t childIndex;
  uint32_t error;
  uint32_t childError;

  if (depth >= ARMY_MODEL_TREE_MAX_DEPTH ||
      node->childCount > sizeof(node->children) / sizeof(node->children[0])) {
    Thandor_Log("ArmyAssetRecord_RegisterAndRelocate: army %u: model tree node with %u children at depth %u, "
                "rejected",record->registryId,node->childCount,depth);
    return FATAL_ERROR_ARMY_ASSET_INVALID;
  }
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
    node->children[childIndex] =
         Asset_RecordAt<ArmyModelTreeNode>(assetBase,static_cast<uintptr_t>(node->children[childIndex]));
    childError = ArmyAssetRecord_RelocateModelTree(record,assetBase,node->children[childIndex],depth + 1);
    if (childError == FATAL_ERROR_ARMY_ASSET_INVALID) {
      return childError;
    }
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
    if (g_ArmyAssetRecordRegistry[slotIndex] != nullptr) {
      continue;
    }
    g_ArmyAssetRecordRegistry[slotIndex] = ArmyAssetRecord_Prefix(record);
    error = 0;
    if (record->rootNodeOffsetOrPointer != 0) {
      /* serialized offset from assetBase -> pointer */
      record->rootNodeOffsetOrPointer = record->rootNodeOffsetOrPointer + (uint32_t)(uintptr_t)assetBase;
      error = ArmyAssetRecord_RelocateModelTree
                        (record,assetBase,Thandor_U32ToPointer<ArmyModelTreeNode>(record->rootNodeOffsetOrPointer),0);
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
    if (candidateRecord != nullptr && candidateRecord->registryId == registryId) {
      return candidateRecord;
    }
  }
  return nullptr;
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
  if (candidateRecord != nullptr) {
    *outRecord = candidateRecord;
    return 0;
  }
  g_WideNumberFormatUtf16(WIDE_FORMAT_WRITE_TERMINATOR,0,10,1,registryId,g_PackageLastErrorPath);
  *outRecord = Thandor_U32ToPointer<ArmyAssetRecordPrefix>(FATAL_ERROR_ARMY_ID_NOT_FOUND);
  return FATAL_ERROR_ARMY_ID_NOT_FOUND;
}

/* Returns 0 when some registered army record with this id is a unit, i.e. has flag 0x0200 of its flags clear;
   1 otherwise. Several records may share an id, so the scan goes on past a match
   with the wrong flags. Predicate of the editor's unit-placement id searches
   (FindNext/FindPrevious/Step*Flag0100Without0200).
*/
bool ArmyAssetRegistry_HasNoUnitWithId(ArmyAssetId recordId)

{
  int slotIndex;
  ArmyAssetRecord *candidateAsset;

  for (slotIndex = 0; slotIndex < ARMY_ASSET_REGISTRY_SLOT_COUNT; slotIndex++) {
    candidateAsset = ArmyAssetRecord_FromPrefix(g_ArmyAssetRecordRegistry[slotIndex]);
    if (candidateAsset != nullptr && recordId == candidateAsset->registryId &&
        (candidateAsset->flags & ARMY_ASSET_FLAG_EDITOR_OBJECT) == 0) {
      return false; /* found a unit */
    }
  }
  return true;
}

/* Returns 0 when some registered army record with this id is an object, i.e. has flag 0x0200 of its flags set;
   1 otherwise. Predicate of the editor's object-placement id searches
   (FindNext/FindPrevious/Step*Flags0100And0200).
*/
bool ArmyAssetRegistry_HasNoObjectWithId(ArmyAssetId recordId)

{
  int slotIndex;
  ArmyAssetRecord *candidateAsset;

  for (slotIndex = 0; slotIndex < ARMY_ASSET_REGISTRY_SLOT_COUNT; slotIndex++) {
    candidateAsset = ArmyAssetRecord_FromPrefix(g_ArmyAssetRecordRegistry[slotIndex]);
    if (candidateAsset != nullptr && recordId == candidateAsset->registryId &&
        (candidateAsset->flags & ARMY_ASSET_FLAG_EDITOR_OBJECT) != 0) {
      return false; /* found an object */
    }
  }
  return true;
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
    candidateAsset = ArmyAssetRecord_FromPrefix(g_ArmyAssetRecordRegistry[slotIndex]);
    if (candidateAsset != nullptr && recordId == candidateAsset->registryId &&
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
    candidateAsset = ArmyAssetRecord_FromPrefix(g_ArmyAssetRecordRegistry[slotIndex]);
    if (candidateAsset != nullptr && recordId == candidateAsset->registryId &&
        (candidateAsset->flags & ARMY_ASSET_FLAG_EDITOR_PLACEABLE) != 0 &&
        (candidateAsset->flags & ARMY_ASSET_FLAG_EDITOR_OBJECT) != 0) {
      return 0; /* found a placeable object */
    }
  }
  return 1;
}

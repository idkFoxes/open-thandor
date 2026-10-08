/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/src/assets/rom/runtime.cpp
 * Reverse engineering by idkFoxes 2026
 */

#include <thandor/assets/rom/runtime.h>
#include <thandor/thandor.h>
#include <thandor/assets/record_bytes.h>
#include <thandor/platform/bootstrap/image.h>

/* Module data. */

RomRegistrySlot *g_RomRegistrySlots = nullptr;

uint16_t g_EngineZentraleRomPathUtf16[20] = {'e', 'n', 'g', 'i', 'n', 'e', '\\', 'z', 'e', 'n', 't', 'r', 'a', 'l', 'e', '.', 'r', 'o', 'm', 0}; /* L"engine\\zentrale.rom" */

/* Checks that the asset (assetByteCount bytes) is a 'rom' of converter version 0x10005 and registers each of
   its variable-size records (from +0x200, each advanced by its leading byteSize) with
   RomAssetRecord_RegisterAndRelocate. An invalid header, or a record that is shorter than its prefix or does
   not fit into the asset, leaves "engine\zentrale.rom" in g_PackageLastErrorPath and fails with
   FATAL_ERROR_ROM_REGISTRY_FULL. Returns 0 on success, otherwise the error code (the original's success return
   value was never used by its caller).
*/
uint32_t RomAsset_PrepareRecords(RomAssetHeader *asset,uint32_t assetByteCount)

{
  uint32_t registrationError;
  AssetRecordCount recordsRemaining;
  RomAssetRecordPrefix *record;
  uint32_t bytesLeft;

  /* The original trusts the asset size and every record's byteSize; bounded here because the walk follows file
     data (a byteSize of 0 loops on one record, a large one walks past the asset). The stock records are 0x1080
     bytes. */
  if (assetByteCount >= sizeof(RomAssetHeader) &&
      asset->recordCountHeader.common.magic == ASSET_MAGIC_ROM &&
      asset->recordCountHeader.common.converterVersion == PCK_CONVERTER_ROM_00010005) {
    record = Asset_RecordAfter<RomAssetRecordPrefix>(asset);
    bytesLeft = assetByteCount - (uint32_t)sizeof(RomAssetHeader);
    for (recordsRemaining = asset->recordCountHeader.recordCount; recordsRemaining != 0; recordsRemaining--) {
      if (bytesLeft < sizeof(RomAssetRecordPrefix) || record->byteSize < sizeof(RomAssetRecordPrefix) ||
          record->byteSize > bytesLeft) {
        Thandor_Log("RomAsset_PrepareRecords: record at offset 0x%X (byteSize 0x%X) does not fit the asset of "
                    "0x%X bytes, rejected",(uint32_t)Asset_ByteDistance(record,asset),
                    bytesLeft < sizeof(RomRecordByteSize) ? 0u : record->byteSize,assetByteCount);
        Package_SetLastErrorPath(g_EngineZentraleRomPathUtf16);
        return FATAL_ERROR_ROM_REGISTRY_FULL;
      }
      registrationError = RomAssetRecord_RegisterAndRelocate(record,asset,assetByteCount);
      if (registrationError != 0) {
        return registrationError;
      }
      bytesLeft = bytesLeft - record->byteSize;
      /* advance by the record's leading byte size */
      record = Asset_RecordAt<RomAssetRecordPrefix>(record,record->byteSize);
    }
    return 0;
  }
  Package_SetLastErrorPath(g_EngineZentraleRomPathUtf16);
  /* an invalid header also fails with the registry-full code */
  return FATAL_ERROR_ROM_REGISTRY_FULL;
}

/* Depth of the explicit walk stacks below; RomSerializedNodeTree_LoadSpritesAndRelocate rejects deeper trees. */
static constexpr int ROM_NODE_TREE_MAX_DEPTH = 64;
#define ROM_NODE_MAX_CHILDREN (sizeof(((RomSerializedNodeHeader *)0)->childReferences) / \
                               sizeof(((RomSerializedNodeHeader *)0)->childReferences[0]))

/* Depth-first walk over a relocated sprite-node tree (RomSerializedNodeHeader), releasing every node's sprite
   asset, parents before children, with an explicit stack of {remaining, nextChild, node} frames. The same
   tree is walked by
   RomSerializedNodeTree_LoadSpritesAndRelocate, which rejects trees with more than ROM_NODE_MAX_CHILDREN
   children per node or deeper than ROM_NODE_TREE_MAX_DEPTH; the limits are applied here as well so that a
   rejected tree is never walked out of bounds.
   Original quirk: every node's sprite is released, also one a node took over from the sprite registry
   (RomSerializedNode_LoadSprite: existingSprite, ownedNestedResourcePresent stays 0), which would be released
   twice. Kept because the stock engine\zentrale.rom (18 records with one node each, 18 distinct sprite ids
   raum00..raum18) never shares a sprite, so every node owns its sprite. */
static void RomSerializedNodeTree_ReleaseSprites(RomSerializedNodeHeader *node)
{
  struct { RomSerializedNodeHeader *node; uint32_t nextChild; uint32_t remaining; } frames[ROM_NODE_TREE_MAX_DEPTH];
  int depth = 0;

  do {
    frames[depth].remaining = node->childCount;
    if (frames[depth].remaining > ROM_NODE_MAX_CHILDREN || depth + 1 >= ROM_NODE_TREE_MAX_DEPTH) {
      frames[depth].remaining = 0; /* rejected while loading; its children were never relocated */
    }
    frames[depth].nextChild = 0;
    Resource_Release(node->spriteAssetReference.spriteAsset);
    frames[depth].node = node;
    depth++;
    /* pop the finished frames; stop at the first one with children left */
    while (depth != 0 && frames[depth - 1].remaining == 0) {
      depth--;
    }
    if (depth != 0) {
      node = frames[depth - 1].node->childReferences[frames[depth - 1].nextChild].node;
      frames[depth - 1].nextChild++;
      frames[depth - 1].remaining--;
    }
  } while (depth != 0);
}

/* Releases the sprite asset of every node of every registered ROM record and empties all 256 registry slots.
*/
void FrontendRomRegistry_ClearAndReleaseNestedResources()

{
  int slotsRemaining;
  RomRegistrySlot *slotCursor;
  RomSerializedNodeHeader *rootNode;

  slotCursor = g_RomRegistrySlots;
  for (slotsRemaining = ROM_REGISTRY_SLOT_COUNT; slotsRemaining != 0; slotsRemaining--) {
    if (slotCursor->record != nullptr) {
      rootNode = Thandor_U32ToPointer<RomSerializedNodeHeader>(slotCursor->record->rootNodeOffsetOrPointer); /* 5f-format: RomAssetRecordPrefix.rootNodeOffsetOrPointer */
      if (rootNode != nullptr) {
        RomSerializedNodeTree_ReleaseSprites(rootNode);
      }
    }
    slotCursor->record = nullptr;
    slotCursor->runtimeRootNode = nullptr;
    slotCursor = slotCursor + 1;
  }
}

/* Reverse lookup in the ROM registry: returns the ROM record whose slot holds the given runtime root node, or
   NULL when no slot does.
*/
RomAssetRecordPrefix * RomRegistry_FindRecordBySlotValue(RomRegistrySlotValue slotValue)

{
  int slotsRemaining;
  RomRegistrySlot *slotCursor;

  slotCursor = g_RomRegistrySlots;
  for (slotsRemaining = ROM_REGISTRY_SLOT_COUNT; slotsRemaining != 0; slotsRemaining--) {
    if (reinterpret_cast<WorldRuntimeNode *>(slotValue) == slotCursor->runtimeRootNode) { /* the slot value is a node address */
      return slotCursor->record;
    }
    slotCursor++;
  }
  return nullptr;
}

/* Returns the entry of a ROM record table (0x200-byte header with the entry count, then 0x200-byte entries)
   whose record id matches, or NULL. Used to find the target record of a frontend camera flight.
*/
void * RomRecordTable_FindRecordById(RomRecordId recordId,void *recordTable)

{
  int recordsRemaining;
  RomRecord *cursor;

  cursor = static_cast<RomRecord *>(recordTable);
  recordsRemaining = cursor->entryCount;
  /* the cursor starts at the header, so the entry it tests lies one header size further on */
  while (recordsRemaining != 0) {
    if (recordId == Asset_RecordAfter<FrontendRomActionEntry>(cursor)->linkedRecordId) {
      return cursor + 1;
    }
    recordsRemaining--;
    cursor = Asset_RecordAt<RomRecord>(cursor,FRONTEND_ROM_ACTION_ENTRY_SIZE);
  }
  return nullptr;
}

/* Same scan as RomRecordTable_FindRecordById, but returns the zero-based entry index, or -1 when no entry of
   the table has the record id.
*/
RomRecordTableIndex RomRecordTable_FindIndexById(RomRecordId recordId,void *table)

{
  int recordIndex;
  int recordsRemaining;
  RomRecord *cursor;

  cursor = static_cast<RomRecord *>(table);
  recordsRemaining = cursor->entryCount;
  recordIndex = 0;
  while (recordsRemaining != 0) {
    if (recordId == Asset_RecordAfter<FrontendRomActionEntry>(cursor)->linkedRecordId) {
      return recordIndex;
    }
    recordIndex++;
    recordsRemaining--;
    cursor = Asset_RecordAt<RomRecord>(cursor,FRONTEND_ROM_ACTION_ENTRY_SIZE);
  }
  return UINT32_MAX;
}

/* True when a node header at the asset-relative offset lies inside the asset of assetByteCount bytes (behind the
   0x200-byte asset header). */
static bool RomSerializedNode_FitsAsset(uint32_t nodeOffset,uint32_t assetByteCount)
{
  return nodeOffset >= sizeof(RomAssetHeader) && assetByteCount >= sizeof(RomSerializedNodeHeader) &&
         nodeOffset <= assetByteCount - sizeof(RomSerializedNodeHeader);
}

/* True when the UTF-16 sprite path that follows a node header is terminated inside the asset of assetByteCount
   bytes and within WIDE_PATH_MAX_CODE_UNITS units. *outCapacity gets the code units the path may use (up to the
   asset end, at most WIDE_PATH_MAX_CODE_UNITS). The node header itself must fit into the asset. */
static bool RomSerializedNode_SpritePathFitsAsset(const uint16_t *path,const RomAssetHeader *assetBase,
                                                  uint32_t assetByteCount,size_t *outCapacity)
{
  size_t capacity = ((size_t)assetByteCount - (size_t)Asset_ByteDistance(path,assetBase)) / sizeof(uint16_t);
  if (capacity > (size_t)WIDE_PATH_MAX_CODE_UNITS) {
    capacity = (size_t)WIDE_PATH_MAX_CODE_UNITS;
  }
  *outCapacity = capacity;
  for (size_t unitIndex = 0; unitIndex < capacity; unitIndex++) {
    if (path[unitIndex] == 0) {
      return true;
    }
  }
  return false;
}

/* Loads the ".spr" sprite named after a serialized node header (UTF-16 file name) into the node, or reuses an
   already registered sprite with the same registry id. Returns true with the error in *outError when loading
   or registering fails, or when the file name is not terminated inside the asset of assetByteCount bytes
   (FATAL_ERROR_ROM_REGISTRY_FULL, the code of an invalid ROM header). */
static bool RomSerializedNode_LoadSprite(RomSerializedNodeHeader *node,const RomAssetHeader *assetBase,
                                         uint32_t assetByteCount,uint32_t *outError)
{
  SpriteAssetHeader *asset;
  SpriteAssetHeader *existingSprite;
  uint16_t *spritePath;
  size_t spritePathCapacity;
  uint32_t loadErrorCode;
  uint32_t spriteRegisterError;

  /* the sprite file name (UTF-16) follows the node header */
  spritePath = Asset_RecordAfter<uint16_t>(node);
  /* The original scans and extends the path without a bound; bounded here because a path without a terminator
     would be read and written past the asset. The stock paths have at most 20 units including the terminator
     and leave at least 17 units to the asset end. */
  if (!RomSerializedNode_SpritePathFitsAsset(spritePath,assetBase,assetByteCount,&spritePathCapacity)) {
    Thandor_Log("RomAssetRecord_RegisterAndRelocate: sprite path at offset 0x%X not terminated within %u units "
                "inside the asset of 0x%X bytes, rejected",(uint32_t)Asset_ByteDistance(spritePath,assetBase),
                (uint32_t)spritePathCapacity,assetByteCount);
    Package_SetLastErrorPath(g_EngineZentraleRomPathUtf16);
    *outError = FATAL_ERROR_ROM_REGISTRY_FULL;
    return true;
  }
  /* cannot fail on a terminated path whose ".spr" fits in spritePathCapacity units (all stock paths); otherwise
     the path is left unchanged */
  WidePath_SetExtensionCode(ASSET_MAGIC_SPR,spritePath,spritePathCapacity);
  asset = static_cast<SpriteAssetHeader *>(Package_LoadEntry(spritePath,&loadErrorCode));
  if (asset == nullptr) {
    *outError = loadErrorCode;
    return true;
  }
  existingSprite = SpriteAssetRegistry_FindById(asset->registryHeader.registryId);
  if (existingSprite != nullptr) {
    node->spriteAssetReference.spriteAsset = existingSprite;
    Resource_Release(asset);
    return false;
  }
  /* set only for sprites this node loaded itself */
  node->ownedNestedResourcePresent++;
  node->spriteAssetReference.spriteAsset = asset;
  spriteRegisterError = SpriteAsset_RegisterAndRelocatePointers(asset);
  if (spriteRegisterError != 0) {
    *outError = spriteRegisterError;
    return true;
  }
  return false;
}

/* Depth-first walk over a serialized sprite-node tree, parents before children: loads every node's sprite
   (RomSerializedNode_LoadSprite) and relocates the child offsets (relative to assetBase) to pointers in place
   while walking, with an explicit stack of {node, nextChild, remaining} frames. Returns 0, or the first loader
   error (a node with more than ROM_NODE_MAX_CHILDREN children or a tree deeper than ROM_NODE_TREE_MAX_DEPTH fails
   with FATAL_ERROR_ROM_REGISTRY_FULL, the code of an invalid ROM header; so does a node whose child offsets do
   not all point at a node header inside the asset of assetByteCount bytes, checked before any of them is
   relocated). The same tree is walked by RomSerializedNodeTree_ReleaseSprites. */
static uint32_t RomSerializedNodeTree_LoadSpritesAndRelocate
          (RomSerializedNodeHeader *node,RomAssetHeader *assetBase,uint32_t assetByteCount)
{
  struct { RomSerializedNodeHeader *node; uint32_t nextChild; uint32_t remaining; } frames[ROM_NODE_TREE_MAX_DEPTH];
  int depth = 0;
  uint32_t loadError;
  uint32_t *child;

  do {
    /* The original trusts the file: childCount indexes the six child references and the frame stack has no
       depth check; bounded here because a malformed ROM would write past both. */
    if (depth >= ROM_NODE_TREE_MAX_DEPTH) {
      Thandor_Log("RomAssetRecord_RegisterAndRelocate: node tree deeper than %d, rejected",ROM_NODE_TREE_MAX_DEPTH);
      Package_SetLastErrorPath(g_EngineZentraleRomPathUtf16);
      return FATAL_ERROR_ROM_REGISTRY_FULL;
    }
    if (RomSerializedNode_LoadSprite(node,assetBase,assetByteCount,&loadError)) {
      return loadError;
    }
    if (node->childCount > ROM_NODE_MAX_CHILDREN) {
      Thandor_Log("RomAssetRecord_RegisterAndRelocate: node with %u children, rejected",node->childCount);
      Package_SetLastErrorPath(g_EngineZentraleRomPathUtf16);
      return FATAL_ERROR_ROM_REGISTRY_FULL;
    }
    /* The original follows every child offset; bounded here because a malformed ROM would read past the asset. */
    for (uint32_t childIndex = 0; childIndex < node->childCount; childIndex++) {
      if (!RomSerializedNode_FitsAsset(node->childReferences[childIndex].savedOffset,assetByteCount)) {
        Thandor_Log("RomAssetRecord_RegisterAndRelocate: child node offset 0x%X outside the asset of 0x%X bytes, "
                    "rejected",node->childReferences[childIndex].savedOffset,assetByteCount);
        Package_SetLastErrorPath(g_EngineZentraleRomPathUtf16);
        return FATAL_ERROR_ROM_REGISTRY_FULL;
      }
    }
    frames[depth].node = node;
    frames[depth].nextChild = 0;
    frames[depth].remaining = node->childCount;
    depth++;
    /* pop the finished frames; stop at the first one with children left */
    while (depth != 0 && frames[depth - 1].remaining == 0) {
      depth--;
    }
    if (depth != 0) {
      child = &frames[depth - 1].node->childReferences[frames[depth - 1].nextChild].savedOffset;
      *child = *child + Thandor_PointerToU32(assetBase); /* 5f-format: RomSerializedNodeHeader.childReferences (relocated in place) */
      frames[depth - 1].nextChild++;
      frames[depth - 1].remaining--;
      node = Thandor_U32ToPointer<RomSerializedNodeHeader>(*child); /* 5f-format: RomSerializedNodeHeader.childReferences (relocated in place) */
    }
  } while (depth != 0);
  return 0;
}

/* Registers a ROM record in the first free slot of g_RomRegistrySlots and relocates its serialized node tree:
   child offsets become pointers, and every node's ".spr" sprite is loaded, or an already registered sprite with
   the same registry id is reused. Returns 0 on success, otherwise FATAL_ERROR_ROM_REGISTRY_FULL or the
   loader's error (the original's success return value, assetBase, was never used by its caller). A record
   without a node tree (root offset 0) is registered as it is. A root offset outside the asset (assetByteCount
   bytes) fails with FATAL_ERROR_ROM_REGISTRY_FULL before the record is registered.
*/
uint32_t RomAssetRecord_RegisterAndRelocate(RomAssetRecordPrefix *record,RomAssetHeader *assetBase,
                                            uint32_t assetByteCount)

{
  uint32_t rootNodeOffset;
  int slotsRemaining;
  RomRegistrySlot *slotCursor;

  /* The original follows the root offset; bounded here because a malformed ROM would read past the asset. */
  if (record->rootNodeOffsetOrPointer != 0 &&
      !RomSerializedNode_FitsAsset(record->rootNodeOffsetOrPointer,assetByteCount)) {
    Thandor_Log("RomAssetRecord_RegisterAndRelocate: record %u has root node offset 0x%X outside the asset of 0x%X "
                "bytes, rejected",record->recordId,record->rootNodeOffsetOrPointer,assetByteCount);
    Package_SetLastErrorPath(g_EngineZentraleRomPathUtf16);
    return FATAL_ERROR_ROM_REGISTRY_FULL;
  }
  slotCursor = g_RomRegistrySlots;
  for (slotsRemaining = ROM_REGISTRY_SLOT_COUNT; slotsRemaining != 0; slotsRemaining--) {
    if (slotCursor->record == nullptr) {
      rootNodeOffset = record->rootNodeOffsetOrPointer;
      slotCursor->record = record;
      if (rootNodeOffset == 0) {
        return 0;
      }
      /* asset start + serialized offset */
      record->rootNodeOffsetOrPointer = Thandor_PointerToU32(Asset_RecordAt(assetBase,record->rootNodeOffsetOrPointer)); /* 5f-format: RomAssetRecordPrefix.rootNodeOffsetOrPointer */
      return RomSerializedNodeTree_LoadSpritesAndRelocate
                       (Asset_RecordAt<RomSerializedNodeHeader>(assetBase,rootNodeOffset),assetBase,assetByteCount);
    }
    slotCursor++;
  }
  Package_SetLastErrorPath(g_EngineZentraleRomPathUtf16);
  return FATAL_ERROR_ROM_REGISTRY_FULL;
}

/* Looks up the runtime root node registered for the ROM record with the given id: returns true and stores it
   (NULL while the record's tree is not built) in *outRootNode, or returns false when no registry slot holds
   such a record (the original's error code FATAL_ERROR_ROM_RECORD_NOT_REGISTERED was read by no caller).
*/
bool RomRegistry_FindSlotValueByRecordId(RomRecordId recordId,WorldRuntimeNode **outRootNode)

{
  int slotsRemaining;
  RomRegistrySlot *slotCursor;

  slotsRemaining = ROM_REGISTRY_SLOT_COUNT;
  slotCursor = g_RomRegistrySlots;
  while (slotCursor->record == nullptr || recordId != slotCursor->record->recordId) {
    slotCursor++;
    slotsRemaining--;
    if (slotsRemaining == 0) {
      return false;
    }
  }
  *outRootNode = slotCursor->runtimeRootNode;
  return true;
}

/* Returns the registered ROM record with this record id, or NULL when no registry slot holds one (the
   original's error code FATAL_ERROR_ROM_RECORD_NOT_REGISTERED; FrontendRomTransition_ActivateRecordById
   reports it).
*/
RomAssetRecordPrefix * RomRegistry_FindRecordById(RomRecordId recordId)

{
  RomAssetRecordPrefix *slotRecord;
  int slotsRemaining;
  RomRegistrySlot *slotCursor;

  slotCursor = g_RomRegistrySlots;
  for (slotsRemaining = ROM_REGISTRY_SLOT_COUNT; slotsRemaining != 0; slotsRemaining--) {
    slotRecord = slotCursor->record;
    if (slotRecord != nullptr && recordId == slotRecord->recordId) {
      return slotRecord;
    }
    slotCursor = slotCursor + 1;
  }
  return nullptr;
}

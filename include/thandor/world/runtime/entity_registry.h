/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/include/thandor/world/runtime/entity_registry.h
 * Reverse engineering by idkFoxes 2026
 */

#ifndef THANDOR_WORLD_RUNTIME_ENTITY_REGISTRY_H
#define THANDOR_WORLD_RUNTIME_ENTITY_REGISTRY_H

#include <thandor/gameplay/session/types.h>
#include <thandor/ui/ingame/types.h>
#include <thandor/world/runtime/types.h>
#include <thandor/core/contracts.h>

/* WorldObjectRecord.common.allocationFlags value of a record in use (WorldObjectArray_AllocateFreeRecord). */
inline constexpr int WORLD_OBJECT_RECORD_ALLOCATED = 0x40000000;
/* WorldOwnerListNode.runtimeFlags bit: the node is linked into its world's owner list. */
inline constexpr uint32_t WORLD_OWNER_NODE_LINKED = 0x80000000;

void WorldRuntime_AttachObjectArray
          (WorldObjectRecordCount count,WorldObjectRecord *objectArray,WorldRuntimeContext *world);

WorldObjectRecord *WorldObjectArray_AllocateFreeRecord(WorldRuntimeContext *worldRuntime);

void WorldRuntime_LinkOwnerListNode(WorldOwnerListNode *node);

void WorldRuntime_UnlinkOwnerListNode(WorldOwnerListNode *node);

void WorldRuntime_ForEachOwnerListNode(void *callbackContext,WorldRuntimeNodeTraversalCallback *callback,
          WorldRuntimeContext *world);

void WorldRuntimeNode_ClearOwnedModelReferencesCallback(void *releasedObject,WorldOwnerListNode *node);

void WorldRuntimeNode_ClearDetachedEntityReferencesCallback(void *detachedObject,WorldOwnerListNode *node);

void WorldRuntimeNode_ReleaseShutdownBindingsCallback(WorldRuntimeContext *shutdownContext,WorldOwnerListNode *node);

#endif /* THANDOR_WORLD_RUNTIME_ENTITY_REGISTRY_H */

/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/include/thandor/assets/rom/runtime.h
 * Reverse engineering by idkFoxes 2026
 */

#ifndef THANDOR_ASSETS_ROM_RUNTIME_H
#define THANDOR_ASSETS_ROM_RUNTIME_H

#include <thandor/generated/types.h>
#include <thandor/core/contracts.h>

/* Submodule: assets/rom/runtime. */
/* Functions are grouped by semantic ownership; address comments are executable virtual addresses. */

/* Frontend ROM action table (g_FrontendActiveRomRecordTable): a 0x200-byte header whose dword at +0x3C is the
   entry count, followed by 0x200-byte entries; each entry holds camera-flight keyframes of 0x20 bytes at +0x40. */
#define FRONTEND_ROM_ACTION_TABLE_HEADER_SIZE 0x200
#define FRONTEND_ROM_ACTION_TABLE_COUNT_OFFSET 0x3c
#define FRONTEND_ROM_ACTION_ENTRY_SIZE 0x200
#define FRONTEND_ROM_ACTION_KEYFRAME_SIZE 0x20
/* Elapsed-tick value beyond every flight's last keyframe time: ends the flight on the next frame. */
#define FRONTEND_ROM_TRANSITION_SKIP_TICKS 0x10000000
/* Record id (dword) inside each ROM record table entry (RomRecordTable_FindRecordById/FindIndexById). */
#define ROM_RECORD_TABLE_ENTRY_ID_OFFSET 0x1c
/* g_RomRegistrySlots: fixed array of 256 {record, runtime root node} slots (RomAssetRecord_RegisterAndRelocate). */
#define ROM_REGISTRY_SLOT_COUNT 256
/* Model-node descriptor list scanned by RomRuntime_ApplyIndexedDescriptor: 0x10-byte entries whose first dword
   holds the kind in its low 4 bits and the entry index above them; kind 4 is a point light. */
#define ROM_NODE_DESCRIPTOR_KIND_MASK 0xf
#define ROM_NODE_DESCRIPTOR_KIND_LIGHT 4

/* 0x005452A0 */
void FrontendRomActionTable_ExecuteRecord
          (uint32_t reservedZero0,uint32_t reservedZero1,FrontendBooleanState32 suppressActivationSound,
          RomRecordTableIndex recordIndex);

/* 0x00546450 */
StatusResult RomAsset_PrepareRecords(RomAssetHeader *asset);

/* 0x005466A0 */
bool RomRuntime_BuildAllRegistryNodeTrees(WorldRuntimeContext *worldRuntime);

/* 0x00547FC0 */
void FrontendRomTransition_ProcessPendingRecord(void);

/* 0x00547400 */
void FrontendRomRegistry_ClearAndReleaseNestedResources(void);

/* 0x00548720 */
void FrontendRomTransition_RequestStop(void);

/* 0x005487F0 */
RomAssetRecordPrefix * RomRegistry_FindRecordBySlotValue(RomRegistrySlotValue slotValue);

/* 0x00548840 */
uint32_t RomRegistry_FindSlotValueByRecord(RomAssetRecordPrefix *record);

/* 0x00548890 */
void * RomRecordTable_FindRecordById(RomRecordId recordId,void *recordTable);

/* 0x005488D0 */
RomRecordTableIndex RomRecordTable_FindIndexById(RomRecordId recordId,void *table);

/* 0x005484D0 */
StatusResult FrontendRomTransition_ActivateRecordById(RomRecordId recordId,WorldRuntimeContext *worldRuntime);

/* 0x00548600 */
bool RomRuntime_UpdateRecordVisibilityAndDescriptors(RomVisibilityFrontendValue frontendValue,RomRecordId recordId);

/* 0x00546330 */
StatusResult RomAssetRecord_RegisterAndRelocate(RomAssetRecordPrefix *record,RomAssetHeader *assetBase);

/* 0x005464C0 */
ModelNodeCreateResult RomRuntime_BuildNodeTreeRecursive
          (PackedArgb32 stateTintArgb,RomSerializedNodeHeader34 *romNodeRecord,
          WorldRuntimeContext *worldObjectArray);

/* 0x005483C0 */
void FrontendRomTransition_InitializeFromRecord(FrontendBooleanState32 transitionEnabled,RomAssetRecordPrefix *record);

/* 0x005487A0 */
StatusResult RomRegistry_FindSlotValueByRecordId(RomRecordId recordId);

/* 0x00548410 */
void RomRuntime_ApplyIndexedDescriptor(RomRecordTableIndex entryIndex,RomAssetRecordPrefix *record);

/* 0x00548740 */
RomRecordResult RomRegistry_FindRecordById(RomRecordId recordId);

#endif /* THANDOR_ASSETS_ROM_RUNTIME_H */

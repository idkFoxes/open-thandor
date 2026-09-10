#ifndef THANDOR_ASSETS_ROM_RUNTIME_H
#define THANDOR_ASSETS_ROM_RUNTIME_H

#include <thandor/generated/v523_types.h>
#include <thandor/core/contracts.h>

/* Submodule: assets/rom/runtime. */
/* Functions are grouped by semantic ownership; address comments are executable virtual addresses. */

/* 0x005452A0 */
void FrontendRomActionTable_ExecuteRecord (undefined4 param_1,undefined4 param_2,FrontendBooleanState32 suppressActivationSound ,RomRecordTableIndex recordIndex);

/* 0x00546450 */
dword RomAsset_PrepareRecords(RomAssetHeader *asset);

/* 0x005466A0 */
void __fastcall RomRuntime_BuildAllRegistryNodeTrees (undefined4 param_1,undefined4 param_2,WorldRuntimeContext *worldRuntime);

/* 0x00547FC0 */
undefined4 __fastcall FrontendRomTransition_ProcessPendingRecord(undefined4 param_1);

/* 0x00547400 */
void __cdecl FrontendRomRegistry_ClearAndReleaseNestedResources(void);

/* 0x00548720 */
void __cdecl FrontendRomTransition_RequestStop(void);

/* 0x005487F0 */
RomAssetRecordPrefix * RomRegistry_FindRecordBySlotValue(RomRegistrySlotValue slotValue);

/* 0x00548840 */
dword RomRegistry_FindSlotValueByRecord(RomAssetRecordPrefix *record);

/* 0x00548890 */
void * RomRecordTable_FindRecordById(RomRecordId recordId,void *recordTable);

/* 0x005488D0 */
undefined8 RomRecordTable_FindIndexById(RomRecordId recordId,void *table);

/* 0x005484D0 */
void FrontendRomTransition_ActivateRecordByIdCf (RomRecordId recordId,WorldRuntimeContext *worldRuntime);

/* 0x00548600 */
void RomRuntime_UpdateRecordVisibilityAndDescriptorsCf (RomVisibilityFrontendValue frontendValue,RomRecordId recordId);

/* 0x00546330 */
undefined8 RomAssetRecord_RegisterAndRelocate(RomAssetRecordPrefix *record,RomAssetHeader *assetBase);

/* 0x005464C0 */
undefined8 __fastcall RomRuntime_BuildNodeTreeRecursive (undefined4 param_1,undefined4 param_2,PackedArgb32 stateTintArgb, RomRuntimeNodeRecordAddress32 romNodeRecord,WorldRuntimeContext *worldObjectArray);

/* 0x005483C0 */
void FrontendRomTransition_InitializeFromRecord (FrontendBooleanState32 transitionEnabled,RomAssetRecordPrefix *record);

/* 0x005487A0 */
dword RomRegistry_FindSlotValueByRecordIdCf(RomRecordId recordId);

/* 0x00548410 */
void RomRuntime_ApplyIndexedDescriptor(RomRecordTableIndex entryIndex,RomAssetRecordPrefix *record);

/* 0x00548740 */
RomAssetRecordPrefix * RomRegistry_FindRecordByIdCf(RomRecordId recordId);

#endif /* THANDOR_ASSETS_ROM_RUNTIME_H */

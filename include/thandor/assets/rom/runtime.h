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

/* 0x005452A0 */
void __thandor_void_preserve_eax_ecx_edx
FrontendRomActionTable_ExecuteRecord
          (dword reservedZero0,dword reservedZero1,FrontendBooleanState32 suppressActivationSound,
          RomRecordTableIndex recordIndex);

/* 0x00546450 */
StatusValueEaxCf5 __thandor_eax_cf_preserve_ecx_edx RomAsset_PrepareRecords(RomAssetHeader *asset);

/* 0x005466A0 */
bool RomRuntime_BuildAllRegistryNodeTrees(WorldRuntimeContext *worldRuntime);

/* 0x00547FC0 */
void __thandor_void_preserve_eax_ecx FrontendRomTransition_ProcessPendingRecord(void);

/* 0x00547400 */
void __thandor_void_preserve_eax_ecx_edx FrontendRomRegistry_ClearAndReleaseNestedResources(void);

/* 0x00548720 */
void __thandor_void_preserve_eax_ecx_edx FrontendRomTransition_RequestStop(void);

/* 0x005487F0 */
RomAssetRecordPrefix * __thandor_eax_preserve_ecx_edx
RomRegistry_FindRecordBySlotValue(RomRegistrySlotValue slotValue);

/* 0x00548840 */
dword RomRegistry_FindSlotValueByRecord(RomAssetRecordPrefix *record);

/* 0x00548890 */
void * __thandor_eax_preserve_ecx_edx
RomRecordTable_FindRecordById(RomRecordId recordId,void *recordTable);

/* 0x005488D0 */
RomRecordTableIndex __thandor_eax_preserve_ecx_edx
RomRecordTable_FindIndexById(RomRecordId recordId,void *table);

/* 0x005484D0 */
StatusValueEaxCf5 __thandor_eax_cf_preserve_ecx_edx
FrontendRomTransition_ActivateRecordByIdCf(RomRecordId recordId,WorldRuntimeContext *worldRuntime);

/* 0x00548600 */
bool __thandor_cf_preserve_eax_ecx_edx
RomRuntime_UpdateRecordVisibilityAndDescriptorsCf
          (RomVisibilityFrontendValue frontendValue,RomRecordId recordId);

/* 0x00546330 */
StatusValueEaxCf5 __thandor_eax_cf_preserve_ecx_edx
RomAssetRecord_RegisterAndRelocate(RomAssetRecordPrefix *record,RomAssetHeader *assetBase);

/* 0x005464C0 */
ModelNodeCreateEaxCf5 __thandor_eax_cf_preserve_ecx_edx
RomRuntime_BuildNodeTreeRecursive
          (PackedArgb32 stateTintArgb,RomSerializedNodeHeader34 *romNodeRecord,
          WorldRuntimeContext *worldObjectArray);

/* 0x005483C0 */
void __thandor_void_preserve_eax_ecx_edx
FrontendRomTransition_InitializeFromRecord
          (FrontendBooleanState32 transitionEnabled,RomAssetRecordPrefix *record);

/* 0x005487A0 */
StatusValueEaxCf5 __thandor_eax_cf_preserve_ecx_edx
RomRegistry_FindSlotValueByRecordIdCf(RomRecordId recordId);

/* 0x00548410 */
void __thandor_void_preserve_eax_ecx_edx
RomRuntime_ApplyIndexedDescriptor(RomRecordTableIndex entryIndex,RomAssetRecordPrefix *record);

/* 0x00548740 */
RomRecordLookupEaxCf5 __thandor_eax_cf_preserve_ecx_edx
RomRegistry_FindRecordByIdCf(RomRecordId recordId);

#endif /* THANDOR_ASSETS_ROM_RUNTIME_H */

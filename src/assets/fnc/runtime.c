/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/src/assets/fnc/runtime.c
 * Reverse engineering by idkFoxes 2026
 */

#include <thandor/assets/fnc/runtime.h>

/* Implementation ownership: assets/fnc/runtime. */

/* Address: 0x0041A610.
   Ownership: assets/fnc/runtime.
   Purpose: Checks the exact little-endian fnc signature. CF clear returns header->bindingMode in EAX; CF set
   returns error code 0x62 when the signature is invalid.
*/
dword FncModule_GetBindingModeCf(FncModuleHeader *module)

{
  if (module->magic == ASSET_MAGIC_FNC) {
    return (module->exportBinding).bindingMode;
  }
  return 0x62;
}

/* Address: 0x0041A640.
   Ownership: assets/fnc/runtime.
   Purpose: Validates the fnc signature, copies the complete image into linear memory, relocates export-table dword
   offsets, and binds seven host services when bindingMode is zero. CF reports failure.
*/
FncModuleLoadEaxCf5 FncModule_LoadAndRelocateCf(FncModuleHeader *serializedModule)

{
  ArenaFreeProc *pAVar1;
  ArenaShrinkProc *pAVar2;
  LocaleGetPackedCurrentDateProc *pLVar3;
  AssetMagic *pAVar4;
  uint uVar5;
  AssetMagic AVar6;
  AssetMagic *pAVar7;
  int *piVar8;
  void **runtimeCallbackTableCursor;
  ArenaLinearReserveEaxCf5 AVar9;
  FncModuleLoadEaxCf5 FVar10;
  
  pAVar4 = (AssetMagic *)0x62;
  if (serializedModule->magic == ASSET_MAGIC_FNC) {
    pAVar4 = (AssetMagic *)0x63;
    uVar5 = serializedModule->allocationSizeBytes;
    if ((serializedModule->exportBinding).bindingMode == 0) {
      AVar9 = (*g_MemoryApi.reserveLinear)(uVar5);
      pAVar4 = (AssetMagic *)AVar9.baseOrError;
      if (!AVar9.carry) {
        pAVar7 = pAVar4;
        for (uVar5 = uVar5 >> 2; uVar5 != 0; uVar5 = uVar5 - 1) {
          *pAVar7 = serializedModule->magic;
          serializedModule = (FncModuleHeader *)&serializedModule->allocationSizeBytes;
          pAVar7 = pAVar7 + 1;
        }
        piVar8 = (int *)((int)pAVar4 + pAVar4[0x2e]);
        pAVar1 = g_MemoryApi.free;
        for (AVar6 = pAVar4[0x2c]; g_MemoryApi.free = pAVar1, AVar6 != 0; AVar6 = AVar6 - 1) {
          *piVar8 = *piVar8 + (int)pAVar4;
          piVar8 = piVar8 + 1;
          pAVar1 = g_MemoryApi.free;
        }
        runtimeCallbackTableCursor = (void **)((int)pAVar4 + pAVar4[0x2f]);
        if (pAVar4[0x2d] == 0) {
          *runtimeCallbackTableCursor = g_MemoryApi.alloc;
          runtimeCallbackTableCursor[1] = pAVar1;
          pAVar2 = g_MemoryApi.shrinkInPlace;
          runtimeCallbackTableCursor[2] = g_MemoryApi.allocLargestFreeBlock;
          runtimeCallbackTableCursor[3] = pAVar2;
          pLVar3 = g_LocaleGetPackedCurrentDate;
          runtimeCallbackTableCursor[4] = g_LocaleGetPackedCurrentTime;
          runtimeCallbackTableCursor[5] = pLVar3;
          runtimeCallbackTableCursor[6] = g_LocaleCopyDefaultComputerLabelUtf16;
        }
        return (FncModuleLoadEaxCf5)((uint5)AVar9 & 0xffffffff);
      }
    }
  }
  FVar10.carry = true;
  FVar10.moduleBase = (int *)pAVar4;
  return FVar10;
}


/* Address: 0x0041A710.
   Ownership: assets/fnc/runtime.
   Purpose: Returns one relocated export pointer by index. CF is set for an index outside exportCount; EAX carries
   pointer or error 0x63. Typed parameters: p0 exportIndex→FncExportIndex_V342. Calling convention, exact
   VariableStorage serialization, function body bytes, control flow, globals, locals, and executable data remain
   unchanged.
*/
StatusValueEaxCf5 __thandor_eax_cf_preserve_ecx_edx
FncModule_GetExportByIndexCf(FncExportIndex exportIndex,FncModuleHeader *module)

{
  StatusValueEaxCf5 SVar1;
  StatusValueEaxCf5 SVar2;
  
  if (exportIndex < (module->exportBinding).exportCount) {
    SVar1.carry = false;
    SVar1.valueOrError =
         *(uint *)(module->reserved10_AF +
                  exportIndex * 4 + (module->exportBinding).exportTableOffset + -0x10);
    return SVar1;
  }
  SVar2.carry = true;
  SVar2.valueOrError = 99;
  return SVar2;
}


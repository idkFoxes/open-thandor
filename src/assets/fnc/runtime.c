/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/src/assets/fnc/runtime.c
 * Reverse engineering by idkFoxes 2026
 */

#include <thandor/assets/fnc/runtime.h>
#include <thandor/thandor.h>

/* Implementation ownership: assets/fnc/runtime. */

/* Address: 0x0041A610.
   Ownership: assets/fnc/runtime.
   Purpose: Checks the exact little-endian fnc signature. CF clear returns header->bindingMode in EAX; CF set
   returns error code 0x62 when the signature is invalid.
*/
uint32_t FncModule_GetBindingModeCf(FncModuleHeader *module)

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
  ArenaFreeProc *arenaFreeProc;
  ArenaShrinkProc *arenaShrinkProc;
  LocaleGetPackedCurrentDateProc *packedDateProc;
  AssetMagic *moduleBaseOrError;
  uint32_t sizeOrDwordCount;
  AssetMagic relocationsRemaining;
  AssetMagic *copyCursor;
  int *relocationCursor;
  void **runtimeCallbackTableCursor;
  ArenaLinearReserveEaxCf5 reserveResult;
  FncModuleLoadEaxCf5 failureResult;
  
  moduleBaseOrError = (AssetMagic *)0x62;
  if (serializedModule->magic == ASSET_MAGIC_FNC) {
    moduleBaseOrError = (AssetMagic *)0x63;
    sizeOrDwordCount = serializedModule->allocationSizeBytes;
    if ((serializedModule->exportBinding).bindingMode == 0) {
      reserveResult = (*g_MemoryApi.reserveLinear)(sizeOrDwordCount);
      moduleBaseOrError = (AssetMagic *)reserveResult.baseOrError;
      if (!reserveResult.carry) {
        copyCursor = moduleBaseOrError;
        for (sizeOrDwordCount = sizeOrDwordCount >> 2; sizeOrDwordCount != 0; sizeOrDwordCount = sizeOrDwordCount - 1) {
          *copyCursor = serializedModule->magic;
          serializedModule = (FncModuleHeader *)&serializedModule->allocationSizeBytes;
          copyCursor = copyCursor + 1;
        }
        relocationCursor = (int *)((int)moduleBaseOrError + moduleBaseOrError[0x2e]);
        arenaFreeProc = g_MemoryApi.free;
        for (relocationsRemaining = moduleBaseOrError[0x2c]; g_MemoryApi.free = arenaFreeProc, relocationsRemaining != 0; relocationsRemaining = relocationsRemaining - 1) {
          *relocationCursor = *relocationCursor + (int)moduleBaseOrError;
          relocationCursor = relocationCursor + 1;
          arenaFreeProc = g_MemoryApi.free;
        }
        runtimeCallbackTableCursor = (void **)((int)moduleBaseOrError + moduleBaseOrError[0x2f]);
        if (moduleBaseOrError[0x2d] == 0) {
          *runtimeCallbackTableCursor = g_MemoryApi.alloc;
          runtimeCallbackTableCursor[1] = arenaFreeProc;
          arenaShrinkProc = g_MemoryApi.shrinkInPlace;
          runtimeCallbackTableCursor[2] = g_MemoryApi.allocLargestFreeBlock;
          runtimeCallbackTableCursor[3] = arenaShrinkProc;
          packedDateProc = g_LocaleGetPackedCurrentDate;
          runtimeCallbackTableCursor[4] = g_LocaleGetPackedCurrentTime;
          runtimeCallbackTableCursor[5] = packedDateProc;
          runtimeCallbackTableCursor[6] = g_LocaleCopyDefaultComputerLabelUtf16;
        }
        return THANDOR_BITCAST(uint64_t, FncModuleLoadEaxCf5, ((THANDOR_BITCAST(ArenaLinearReserveEaxCf5, uint64_t, reserveResult) & 0xFFFFFFFFFFull) & 0xffffffff));
      }
    }
  }
  failureResult.carry = true;
  failureResult.moduleBase = (int *)moduleBaseOrError;
  return failureResult;
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
  StatusValueEaxCf5 successResult;
  StatusValueEaxCf5 failureResult;
  
  if (exportIndex < (module->exportBinding).exportCount) {
    successResult.carry = false;
    successResult.valueOrError =
         *(uint32_t *)(module->reserved10_AF +
                  exportIndex * 4 + (module->exportBinding).exportTableOffset + -0x10);
    return successResult;
  }
  failureResult.carry = true;
  failureResult.valueOrError = 99;
  return failureResult;
}


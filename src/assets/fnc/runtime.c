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
   Returns the binding mode (dword +0xB4) of an 'fnc' code module image with CF clear, or
   FATAL_ERROR_FNC_MODULE_INVALID with CF set when the image lacks the 'fnc' signature. Lets a caller check the
   mode before FncModule_LoadAndRelocate, which only supports mode 0. No caller or table slot referencing it was
   found in src/ or src/generated/image_data.c.
*/
uint32_t FncModule_GetBindingMode(FncModuleHeader *module)

{
  if (module->magic == ASSET_MAGIC_FNC) {
    return (module->exportBinding).bindingMode;
  }
  return FATAL_ERROR_FNC_MODULE_INVALID;
}

/* Address: 0x0041A640.
   Loads an 'fnc' code module: checks the signature, copies the whole image into freshly reserved linear
   memory, adds the load address to every export-table entry (stored as image offsets) and fills the
   module's host API table with seven engine services (arena heap, packed date/time, computer label).
   Only bindingMode 0 is supported. CF clear returns the module base, CF set an error code.
*/
FncModuleLoadResult FncModule_LoadAndRelocate(FncModuleHeader *serializedModule)

{
  ArenaFreeProc *arenaFreeProc;
  ArenaShrinkProc *arenaShrinkProc;
  LocaleGetPackedCurrentDateProc *packedDateProc;
  AssetMagic *moduleBaseOrError;
  uint32_t sizeOrDwordCount;
  AssetMagic relocationsRemaining;
  AssetMagic *copyCursor;
  int *relocationCursor;
  void **hostApiTable;
  ArenaReserveResult reserveResult;
  FncModuleLoadResult failureResult;

  moduleBaseOrError = (AssetMagic *)FATAL_ERROR_FNC_MODULE_INVALID;
  if (serializedModule->magic == ASSET_MAGIC_FNC) {
    moduleBaseOrError = (AssetMagic *)FATAL_ERROR_FNC_MODULE_BINDING;
    sizeOrDwordCount = serializedModule->allocationSizeBytes;
    if ((serializedModule->exportBinding).bindingMode == 0) {
      reserveResult = g_MemoryApi.reserveLinear(sizeOrDwordCount);
      moduleBaseOrError = (AssetMagic *)reserveResult.baseOrError;
      if (!reserveResult.failed) {
        /* dword copy of the whole image (REP MOVSD in the original) */
        copyCursor = moduleBaseOrError;
        for (sizeOrDwordCount = sizeOrDwordCount >> 2; sizeOrDwordCount != 0; sizeOrDwordCount--) {
          *copyCursor = serializedModule->magic;
          serializedModule = (FncModuleHeader *)&serializedModule->allocationSizeBytes;
          copyCursor = copyCursor + 1;
        }
        /* dword 0x2E = exportBinding.exportTableOffset, 0x2C = exportBinding.exportCount of the copy */
        relocationCursor = (int *)((int)moduleBaseOrError + moduleBaseOrError[0x2e]);
        arenaFreeProc = g_MemoryApi.free;
        for (relocationsRemaining = moduleBaseOrError[0x2c]; g_MemoryApi.free = arenaFreeProc, relocationsRemaining != 0; relocationsRemaining--) {
          *relocationCursor = *relocationCursor + (int)moduleBaseOrError;
          relocationCursor = relocationCursor + 1;
          arenaFreeProc = g_MemoryApi.free;
        }
        /* dword 0x2F = exportBinding.hostApiTableOffset, 0x2D = exportBinding.bindingMode */
        hostApiTable = (void **)((int)moduleBaseOrError + moduleBaseOrError[0x2f]);
        if (moduleBaseOrError[0x2d] == 0) {
          *hostApiTable = g_MemoryApi.alloc;
          hostApiTable[1] = arenaFreeProc;
          arenaShrinkProc = g_MemoryApi.shrinkInPlace;
          hostApiTable[2] = g_MemoryApi.allocLargestFreeBlock;
          hostApiTable[3] = arenaShrinkProc;
          packedDateProc = g_LocaleGetPackedCurrentDate;
          hostApiTable[4] = g_LocaleGetPackedCurrentTime;
          hostApiTable[5] = packedDateProc;
          hostApiTable[6] = g_LocaleCopyDefaultComputerLabelUtf16;
        }
        /* success: the module base with CF clear */
        return THANDOR_BITCAST(uint64_t, FncModuleLoadResult, ((THANDOR_BITCAST(ArenaReserveResult, uint64_t, reserveResult) & 0xFFFFFFFFFFull) & 0xffffffff));
      }
    }
  }
  failureResult.failed = true;
  failureResult.moduleBase = (int *)moduleBaseOrError;
  return failureResult;
}


/* Address: 0x0041A710.
   Returns export number exportIndex of a module loaded by FncModule_LoadAndRelocate, i.e. the already
   relocated entry of its export table. CF set with FATAL_ERROR_FNC_MODULE_BINDING for an index outside
   exportCount.
*/
StatusResult FncModule_GetExportByIndex(FncExportIndex exportIndex,FncModuleHeader *module)

{
  StatusResult successResult;
  StatusResult failureResult;

  if (exportIndex < (module->exportBinding).exportCount) {
    successResult.failed = false;
    /* reserved10_AF starts at byte 0x10, so this reads module + exportTableOffset + exportIndex * 4 */
    successResult.valueOrError =
         *(uint32_t *)(module->reserved10_AF +
                  exportIndex * 4 + (module->exportBinding).exportTableOffset - 0x10);
    return successResult;
  }
  failureResult.failed = true;
  failureResult.valueOrError = FATAL_ERROR_FNC_MODULE_BINDING;
  return failureResult;
}


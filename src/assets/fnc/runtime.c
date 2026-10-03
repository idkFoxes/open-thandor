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
   found in src/.
*/
uint32_t FncModule_GetBindingMode(FncModuleHeader *module)

{
  if (module->magic == ASSET_MAGIC_FNC) {
    return module->exportBinding.bindingMode;
  }
  return FATAL_ERROR_FNC_MODULE_INVALID;
}

/* Address: 0x0041A640.
   Loads an 'fnc' code module: checks the signature, copies the whole image into freshly reserved linear
   memory, adds the load address to every export-table entry (stored as image offsets) and fills the
   module's host API table with seven engine services (arena heap, packed date/time, computer label).
   Only bindingMode 0 is supported. Returns true with the loaded module in *outModule, or false with the error
   code in *outError (FATAL_ERROR_FNC_MODULE_INVALID, FATAL_ERROR_FNC_MODULE_BINDING or the reserve failure);
   only one of the two out-parameters is written.
*/
bool FncModule_LoadAndRelocate(FncModuleHeader *serializedModule,FncModuleHeader **outModule,uint32_t *outError)

{
  void *moduleBase;
  FncModuleHeader *module;
  const uint32_t *source;
  uint32_t *destination;
  uint32_t dwordCount;
  uint32_t exportsRemaining;
  int *exportEntry;
  void **hostApiTable;
  uint32_t reserveError;

  if (serializedModule->magic != ASSET_MAGIC_FNC) {
    *outError = FATAL_ERROR_FNC_MODULE_INVALID;
    return false;
  }
  if (serializedModule->exportBinding.bindingMode != 0) {
    *outError = FATAL_ERROR_FNC_MODULE_BINDING;
    return false;
  }
  reserveError = g_MemoryApi.reserveLinear(serializedModule->allocationSizeBytes,&moduleBase);
  if (reserveError != 0) {
    *outError = reserveError;
    return false;
  }

  /* dword copy of the whole image (REP MOVSD in the original; a size remainder of 1-3 bytes is not copied) */
  source = (const uint32_t *)serializedModule;
  destination = (uint32_t *)moduleBase;
  for (dwordCount = serializedModule->allocationSizeBytes >> 2; dwordCount != 0; dwordCount--) {
    *destination = *source;
    source++;
    destination++;
  }
  module = (FncModuleHeader *)moduleBase;

  /* relocate the export table of the copy: image offsets become absolute addresses */
  exportEntry = (int *)((uint8_t *)moduleBase + module->exportBinding.exportTableOffset);
  for (exportsRemaining = module->exportBinding.exportCount; exportsRemaining != 0; exportsRemaining--) {
    *exportEntry = *exportEntry + (int)moduleBase;
    exportEntry++;
  }

  hostApiTable = (void **)((uint8_t *)moduleBase + module->exportBinding.hostApiTableOffset);
  if (module->exportBinding.bindingMode == 0) {
    hostApiTable[0] = g_MemoryApi.alloc;
    hostApiTable[1] = g_MemoryApi.free;
    hostApiTable[2] = g_MemoryApi.allocLargestFreeBlock;
    hostApiTable[3] = g_MemoryApi.shrinkInPlace;
    hostApiTable[4] = g_LocaleGetPackedCurrentTime;
    hostApiTable[5] = g_LocaleGetPackedCurrentDate;
    hostApiTable[6] = g_LocaleCopyDefaultComputerLabelUtf16;
  }
  *outModule = module;
  return true;
}


/* Address: 0x0041A710.
   Returns export number exportIndex of a module loaded by FncModule_LoadAndRelocate, i.e. the already
   relocated entry of its export table. Returns 0 with the export address in *outExport, or
   FATAL_ERROR_FNC_MODULE_BINDING (and *outExport unchanged) for an index outside exportCount.
*/
uint32_t FncModule_GetExportByIndex(FncExportIndex exportIndex,FncModuleHeader *module,void **outExport)

{
  if (exportIndex < module->exportBinding.exportCount) {
    /* the relocated entry at module + exportTableOffset + exportIndex * 4 */
    *outExport = (void *)(uintptr_t)
         *(uint32_t *)((uint8_t *)module + exportIndex * 4 + module->exportBinding.exportTableOffset);
    return 0;
  }
  return FATAL_ERROR_FNC_MODULE_BINDING;
}


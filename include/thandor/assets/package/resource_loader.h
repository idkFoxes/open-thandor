/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/include/thandor/assets/package/resource_loader.h
 * Reverse engineering by idkFoxes 2026
 */

#ifndef THANDOR_ASSETS_PACKAGE_RESOURCE_LOADER_H
#define THANDOR_ASSETS_PACKAGE_RESOURCE_LOADER_H

#include <thandor/core/types.h>
#include <thandor/core/contracts.h>

Bool8 Resource_Load(uint16_t *path,void **outBuffer,uint32_t *outByteCount,uint32_t *outErrorCode);

void Resource_Release(void *resourceBuffer);

#endif /* THANDOR_ASSETS_PACKAGE_RESOURCE_LOADER_H */

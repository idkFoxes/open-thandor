/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/include/thandor/core/settings/persistent.h
 * Reverse engineering by idkFoxes 2026
 */

#ifndef THANDOR_CORE_SETTINGS_PERSISTENT_H
#define THANDOR_CORE_SETTINGS_PERSISTENT_H

#include <thandor/generated/types.h>
#include <thandor/core/contracts.h>

/* Submodule: core/settings/persistent. */
/* Functions are grouped by semantic ownership; address comments are executable virtual addresses. */

/* 0x00402C00 */
void __thandor_preserve_eax PersistentSettings_Flush(void);

/* 0x00402B00 */
void __thandor_void_preserve_eax_ecx PersistentSettings_Load(void);

/* 0x00402C50 */
dword __thandor_eax_preserve_ecx_edx
PersistentSettings_ReadDword
          (PersistentSettingsDwordValue defaultValue,
          PersistentSettingsByteOffset settingsOffsetBytes);

/* 0x00402CC0 */
void * __thandor_eax_preserve_ecx_edx
PersistentSettings_GetRegionOrFallback
          (PersistentSettingsByteCount regionByteCount,void *fallback,
          PersistentSettingsByteOffset settingsOffsetBytes);

/* 0x00402CF0 */
void __thandor_void_preserve_eax_ecx_edx
PersistentSettings_WriteDwords
          (PersistentSettingsByteCount regionByteCount,dword *source,
          PersistentSettingsByteOffset settingsOffsetBytes);

/* 0x00402C80 */
void __thandor_void_preserve_eax_ecx_edx
PersistentSettings_WriteDword
          (PersistentSettingsDwordValue value,PersistentSettingsByteOffset settingsOffsetBytes);

#endif /* THANDOR_CORE_SETTINGS_PERSISTENT_H */

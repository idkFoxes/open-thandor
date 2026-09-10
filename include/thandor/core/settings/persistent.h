#ifndef THANDOR_CORE_SETTINGS_PERSISTENT_H
#define THANDOR_CORE_SETTINGS_PERSISTENT_H

#include <thandor/generated/v523_types.h>
#include <thandor/core/contracts.h>

/* Submodule: core/settings/persistent. */
/* Functions are grouped by semantic ownership; address comments are executable virtual addresses. */

/* 0x00402C00 */
void __cdecl PersistentSettings_Flush(void);

/* 0x00402B00 */
void __cdecl PersistentSettings_Load(void);

/* 0x00402C50 */
dword PersistentSettings_ReadDword (PersistentSettingsDwordValue defaultValue, PersistentSettingsByteOffset settingsOffsetBytes);

/* 0x00402CC0 */
void * PersistentSettings_GetRegionOrFallback (PersistentSettingsByteCount regionByteCount,void *fallback, PersistentSettingsByteOffset settingsOffsetBytes);

/* 0x00402CF0 */
void PersistentSettings_WriteDwords (PersistentSettingsByteCount regionByteCount,void *source, PersistentSettingsByteOffset settingsOffsetBytes);

/* 0x00402C80 */
void PersistentSettings_WriteDword (PersistentSettingsDwordValue value,PersistentSettingsByteOffset settingsOffsetBytes);

#endif /* THANDOR_CORE_SETTINGS_PERSISTENT_H */

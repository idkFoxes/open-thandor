/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/src/core/settings/data.c
 */

/* Data of the original image that this module uses (moved here from the generated image data in
   step 4c); declared in <thandor/core/settings/data.h>. */

#include <thandor/thandor.h>

#pragma warning(disable : 4152) /* function pointer fields initialized through (void *) */

__declspec(align(16)) uint32_t g_LocaleCountryCodeOverride = 0;

__declspec(align(16)) PersistentSettingsRuntime g_PersistentSettings = {.path = L"thandor.dat"};

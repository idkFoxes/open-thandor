/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/src/assets/model/data.c
 */

/* Data of the original image that this module uses (moved here from the generated image data in
   step 4c); declared in <thandor/assets/model/data.h>. Original addresses in the comments. */

#include <thandor/thandor.h>

#pragma warning(disable : 4152) /* function pointer fields initialized through (void *) */

/* 0050AE70 g_ModelRaycastLocalOrigin: Q12 ray origin in the tested node's frame */
__declspec(align(16)) GraphicsFixedVec3 g_ModelRaycastLocalOrigin = {0};

/* 0050AE88 g_ModelRaycastLocalDirectionQ28: Q28 ray direction in the tested node's frame */
__declspec(align(8)) GraphicsFixedVec3 g_ModelRaycastLocalDirectionQ28 = {0};

/* 0051EEF0 g_ModelDefinitionRegistry */
__declspec(align(16)) ModelDefinitionRecordPrefix *g_ModelDefinitionRegistry[768] = {0};

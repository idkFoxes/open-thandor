/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/include/thandor/assets/model/data.h
 */

#ifndef THANDOR_ASSETS_MODEL_DATA_H
#define THANDOR_ASSETS_MODEL_DATA_H

#include <thandor/generated/types.h>
#include <thandor/generated/ui_templates.h>

extern GraphicsFixedVec3 g_ModelRaycastLocalOrigin; /* 0050AE70 g_ModelRaycastLocalOrigin: Q12 ray origin in the tested node's frame */

extern GraphicsFixedVec3 g_ModelRaycastLocalDirectionQ28; /* 0050AE88 g_ModelRaycastLocalDirectionQ28: Q28 ray direction in the tested node's frame */

extern ModelDefinitionRecordPrefix *g_ModelDefinitionRegistry[768]; /* 0051EEF0 g_ModelDefinitionRegistry */

#endif

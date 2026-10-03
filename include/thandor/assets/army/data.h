/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/include/thandor/assets/army/data.h
 */

#ifndef THANDOR_ASSETS_ARMY_DATA_H
#define THANDOR_ASSETS_ARMY_DATA_H

#include <thandor/generated/types.h>
#include <thandor/generated/ui_templates.h>

extern ArmyAssetRecordPrefix *g_ArmyAssetRecordRegistry[768];

extern UiCommandDispatchRecord g_InGameKeyboardDispatchRecords[37]; /* 36 records + the terminator record [36] (key code 0, which ends the dispatch scan; its other two dwords are 0x90 fill); followed by 4 bytes 0x90 fill (dropped) */

#endif

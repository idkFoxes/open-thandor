/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/include/thandor/gameplay/faction/data.h
 */

#ifndef THANDOR_GAMEPLAY_FACTION_DATA_H
#define THANDOR_GAMEPLAY_FACTION_DATA_H

#include <thandor/generated/types.h>
#include <thandor/generated/ui_templates.h>

extern int32_t g_InGamePlacementSurfaceHeightQ12OrSentinel; /* 00563720 g_InGamePlacementSurfaceHeightQ12OrSentinel */

extern uint32_t *g_OldUnitSecondaryTable; /* 00563BBC g_OldUnitSecondaryTable */

extern uint32_t *g_OldUnitPrimaryTable; /* 00563BC0 g_OldUnitPrimaryTable */

extern OldUnitRecordCount g_OldUnitRecordCount; /* 00563BC4 g_OldUnitRecordCount: followed by 8 bytes 0x90 fill (dropped) */

#endif

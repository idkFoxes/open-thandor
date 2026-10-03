/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/include/thandor/ui/support/data.h
 */

#ifndef THANDOR_UI_SUPPORT_DATA_H
#define THANDOR_UI_SUPPORT_DATA_H

#include <thandor/generated/types.h>
#include <thandor/generated/ui_templates.h>

extern RecentTextSerialCounter g_RecentTextSerialCounter; /* 0050F0D0 g_RecentTextSerialCounter */

extern RecentTextHistorySlot *g_RecentTextSlotStorage; /* 0050F0E0 g_RecentTextSlotStorage */

extern uint32_t g_RecentTextEntrySerials[8]; /* 0050F0E4 g_RecentTextEntrySerials */

extern uint16_t g_CreditsTexturePathUtf16[22]; /* 00545C22 g_CreditsTexturePathUtf16 */

#endif

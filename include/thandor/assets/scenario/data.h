/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/include/thandor/assets/scenario/data.h
 */

#ifndef THANDOR_ASSETS_SCENARIO_DATA_H
#define THANDOR_ASSETS_SCENARIO_DATA_H

#include <thandor/generated/types.h>
#include <thandor/generated/ui_templates.h>

extern uint16_t g_ExecutableDirectoryUtf16[256];

extern ScenarioCatalogHeader *g_ScenarioCatalog;

extern uint32_t g_ScenarioCatalogUsedBytes;

extern uint16_t u_save___sve_0050d9c8[11];

extern uint16_t u_level_level_dat_0050da0e[16];

extern ScenarioLevelDataPathTemplate24 g_ScenarioLevelDataPathTemplateUtf16;

extern uint16_t u_level_campagne_dat_0050da52[19];

extern ScenarioCampaignDataPathTemplate2A g_ScenarioCampaignDataPathTemplateUtf16;

extern uint16_t u_level_0050daac[6];

extern uint16_t u_level_0050dab8[6];

extern uint16_t g_LevelResourcePathScratchUtf16[256];

extern FrontendLoadedLevelAsset *g_FrontendLoadedLevelAsset;

extern uint32_t g_FrontendScenarioTransferState;

extern ScenarioCatalogRefreshSelectedRecordCallback *g_FrontendScenarioMapOptionHandlerTable[3];

extern uint16_t g_FrontendScenarioPathScratchUtf16[256]; /* level/campaign/save path (level\<name>.lev etc.) built for Package_LoadEntry */

#endif

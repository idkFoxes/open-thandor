/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/include/thandor/assets/scenario/data.h
 */

#ifndef THANDOR_ASSETS_SCENARIO_DATA_H
#define THANDOR_ASSETS_SCENARIO_DATA_H

#include <thandor/generated/types.h>
#include <thandor/generated/ui_templates.h>

extern uint16_t g_ExecutableDirectoryUtf16[256]; /* 0040AFC0 g_ExecutableDirectoryUtf16 */

extern ScenarioCatalogHeader *g_ScenarioCatalog; /* 0050D9C0 g_ScenarioCatalog */

extern uint32_t g_ScenarioCatalogUsedBytes; /* 0050D9C4 g_ScenarioCatalogUsedBytes */

extern uint16_t u_save___sve_0050d9c8[11]; /* 0050D9C8 u_save___sve_0050d9c8 */

extern uint16_t u_level_level_dat_0050da0e[16]; /* 0050DA0E u_level_level_dat_0050da0e */

extern ScenarioLevelDataPathTemplate24 g_ScenarioLevelDataPathTemplateUtf16; /* 0050DA2E g_ScenarioLevelDataPathTemplateUtf16 */

extern uint16_t u_level_campagne_dat_0050da52[19]; /* 0050DA52 u_level_campagne_dat_0050da52 */

extern ScenarioCampaignDataPathTemplate2A g_ScenarioCampaignDataPathTemplateUtf16; /* 0050DA78 g_ScenarioCampaignDataPathTemplateUtf16 */

extern uint16_t u_level_0050daac[6]; /* 0050DAAC u_level_0050daac */

extern uint16_t u_level_0050dab8[6]; /* 0050DAB8 u_level_0050dab8 */

extern uint16_t g_LevelResourcePathScratchUtf16[256]; /* 00530C28 g_LevelResourcePathScratchUtf16 */

extern FrontendLoadedLevelAsset *g_FrontendLoadedLevelAsset; /* 00545780 g_FrontendLoadedLevelAsset */

extern uint32_t g_FrontendScenarioTransferState; /* 00545918 g_FrontendScenarioTransferState */

extern ScenarioCatalogRefreshSelectedRecordCallback *g_FrontendScenarioMapOptionHandlerTable[3]; /* 00545A98 g_FrontendScenarioMapOptionHandlerTable */

extern uint16_t g_FrontendScenarioPathScratchUtf16[256]; /* 00545C72 g_FrontendScenarioPathScratchUtf16: level/campaign/save path (level\<name>.lev etc.) built for Package_LoadEntry */

#endif

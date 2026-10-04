/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/include/thandor/assets/scenario/session_load.h
 * Reverse engineering by idkFoxes 2026
 */

#ifndef THANDOR_ASSETS_SCENARIO_SESSION_LOAD_H
#define THANDOR_ASSETS_SCENARIO_SESSION_LOAD_H

#include <thandor/generated/types.h>
#include <thandor/core/contracts.h>

/* Submodule: assets/scenario/session_load. */

/* Functions are grouped by semantic ownership. */

void FrontendScenarioAction_StartFieldGridLoad(void *source);

void FrontendScenarioSession_LoadOrRequestFieldGrid(uint32_t playerRuntimeId);

void FrontendScenarioSession_LoadOrRequestCampaignBundle
          (uint32_t playerRuntimeId,uint32_t unusedArg1,uint32_t unusedArg2,uint32_t selectedRecordIndex);

void FrontendScenarioSession_LoadOrRequestLevelAsset
          (uint32_t playerRuntimeId,uint32_t unusedArg1,uint32_t unusedArg2,uint32_t selectedRowIndex);

extern uint16_t g_ScenarioLevelDirectoryUtf16[6];
extern uint16_t g_LevelResourcePathScratchUtf16[256];
extern FrontendLoadedLevelAsset *g_FrontendLoadedLevelAsset;

extern uint16_t g_FrontendScenarioPathScratchUtf16[256]; /* level/campaign/save path (level\<name>.lev etc.) built for Package_LoadEntry */

extern uintptr_t g_FrontendLoadedCampaignAsset;

#endif /* THANDOR_ASSETS_SCENARIO_SESSION_LOAD_H */

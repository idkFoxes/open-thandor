/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/include/thandor/gameplay/session/scenario_load.h
 * Reverse engineering by idkFoxes 2026
 */

#ifndef THANDOR_GAMEPLAY_SESSION_SCENARIO_LOAD_H
#define THANDOR_GAMEPLAY_SESSION_SCENARIO_LOAD_H

#include <thandor/ui/frontend/types.h>
#include <thandor/assets/scenario/catalog.h>
#include <thandor/core/contracts.h>
#include <thandor/core/text/path.h>

/* True when a level of loadedByteCount bytes holds its header, is not shorter than its header's allocationSizeBytes
   and its own path (header pathState.levelPathOffsetOrLoadedFieldGrid, still an offset) starts inside those bytes
   with its terminator at least 5 code units before their end (room for the ".fld" rewrite). Otherwise logs one
   line (context, the level path in g_FrontendScenarioPathScratchUtf16) and returns false. */
bool FrontendLevelAsset_LoadedImageFits(const FrontendLoadedLevelAsset *level,uint32_t loadedByteCount,
                                        const char *context);

void FrontendScenarioAction_StartFieldGridLoad(void *source);

void FrontendScenarioSession_LoadOrRequestFieldGrid(uint32_t playerRuntimeId);

void FrontendScenarioSession_LoadOrRequestCampaignBundle
          (uint32_t playerRuntimeId,uint32_t unusedArg1,uint32_t unusedArg2,uint32_t selectedRecordIndex);

void FrontendScenarioSession_LoadOrRequestLevelAsset
          (uint32_t playerRuntimeId,uint32_t unusedArg1,uint32_t unusedArg2,uint32_t selectedRowIndex);

extern uint16_t g_ScenarioLevelDirectoryUtf16[6];
extern uint16_t g_LevelResourcePathScratchUtf16[THANDOR_PATH_CAPACITY];
extern FrontendLoadedLevelAsset *g_FrontendLoadedLevelAsset;

extern uint16_t g_FrontendScenarioPathScratchUtf16[256]; /* level/campaign/save path (level\<name>.lev etc.) built for Package_LoadEntry */

extern CampaignAsset *g_FrontendLoadedCampaignAsset;

#endif /* THANDOR_GAMEPLAY_SESSION_SCENARIO_LOAD_H */

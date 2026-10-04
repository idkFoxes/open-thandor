/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/include/thandor/ui/frontend/main_loop.h
 * Reverse engineering by idkFoxes 2026
 */

#ifndef THANDOR_UI_FRONTEND_MAIN_LOOP_H
#define THANDOR_UI_FRONTEND_MAIN_LOOP_H

#include <thandor/generated/types.h>
#include <thandor/core/contracts.h>

/* Submodule: ui/frontend/main_loop. */

/* Bits of FrontendPlayerRuntimeRecord.factionAssignment.roleStateFlags: per-player progress through the
   network menu handshake, set locally or from the peer's packets (ui/frontend/player, assets/scenario/catalog). */
#define FRONTEND_PLAYER_STATE_SCENARIO_CATALOG 0x01 /* scenario catalogue exchanged */
#define FRONTEND_PLAYER_STATE_TASK_ASSIGNMENT 0x02 /* ready for the task-assignment page */
#define FRONTEND_PLAYER_STATE_LEVEL_LOADED 0x04 /* level package loaded locally */
#define FRONTEND_PLAYER_STATE_LEVEL_RECEIVED 0x08 /* level package received from / confirmed to the host */
#define FRONTEND_PLAYER_STATE_LEVEL_READY_MASK 0x0C
/* set by FrontendScenarioSession_LoadOrRequestLevelAsset for players whose catalog level mask has the selected
   level, i.e. who can load it from their own disk instead of receiving it */
#define FRONTEND_PLAYER_STATE_HAS_LEVEL_LOCALLY 0x10

/* Functions are grouped by semantic ownership. */

Bool8 Frontend_MainLoop(RomRecordId frontendEntryRecordId,uint32_t *outError);

extern uint32_t g_FrontendPendingPageAction;

extern uint32_t g_FrontendScenarioInitializationCount;

extern uint32_t g_EndMovieSelectionIndex;
extern uint32_t g_FrontendPendingPageActionDepth;

#endif /* THANDOR_UI_FRONTEND_MAIN_LOOP_H */

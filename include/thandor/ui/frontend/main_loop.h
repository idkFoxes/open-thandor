/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/include/thandor/ui/frontend/main_loop.h
 * Reverse engineering by idkFoxes 2026
 */

#ifndef THANDOR_UI_FRONTEND_MAIN_LOOP_H
#define THANDOR_UI_FRONTEND_MAIN_LOOP_H

#include <thandor/assets/rom/types.h>
#include <thandor/core/types.h>
#include <thandor/core/contracts.h>
#include <thandor/ui/frontend/types.h> /* FrontendPageAction */

/* The FRONTEND_PLAYER_STATE_* bits of FrontendPlayerRuntimeRecord.factionAssignment.roleStateFlags are the flag
   enum FrontendRoleStateFlags (ui/frontend/types.h). */

bool Frontend_MainLoop(RomRecordId frontendEntryRecordId,uint32_t *outError);

extern FrontendPageAction g_FrontendPendingPageAction;

extern uint32_t g_FrontendScenarioInitializationCount;

extern uint32_t g_EndMovieSelectionIndex;
extern uint32_t g_FrontendPendingPageActionDepth;

#endif /* THANDOR_UI_FRONTEND_MAIN_LOOP_H */

/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/include/thandor/ui/frontend/scenario.h
 * Reverse engineering by idkFoxes 2026
 */

#ifndef THANDOR_UI_FRONTEND_SCENARIO_H
#define THANDOR_UI_FRONTEND_SCENARIO_H

#include <thandor/generated/types.h>
#include <thandor/core/contracts.h>

/* Submodule: ui/frontend/scenario. */
/* Functions are grouped by semantic ownership; address comments are executable virtual addresses. */

/* First code unit of a level title: rich-text style code, normal or highlighted (a level that some other player
   of the session does not have). */
#define FRONTEND_TEXT_STYLE_NORMAL 0x8000
#define FRONTEND_TEXT_STYLE_HIGHLIGHTED 0x8001
/* Mission briefing text of faction f (1..7) of a level: this + f + level title id * 0x10 (level text page). */
#define TEXT_ID_LEVEL_BRIEFING_BASE 0x230017

/* 0x00547D60 */
void FrontendRoot_TickNetworkPagesMovieCursorAndScenarioState(UiRootNode *rootCallbackContext);

/* 0x0054C9F0 */
void FrontendMissionBriefingPage_Initialize(UiRootNode *frontendRoot);

#endif /* THANDOR_UI_FRONTEND_SCENARIO_H */

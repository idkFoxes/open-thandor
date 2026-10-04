/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/include/thandor/ui/frontend/mission_briefing.h
 * Reverse engineering by idkFoxes 2026
 */

#ifndef THANDOR_UI_FRONTEND_MISSION_BRIEFING_H
#define THANDOR_UI_FRONTEND_MISSION_BRIEFING_H

#include <thandor/ui/controls/types.h>
#include <thandor/core/contracts.h>

/* Submodule: ui/frontend/mission_briefing. */

/* Mission briefing text of faction f (1..7) of a level: this + f + level title id * 0x10 (level text page). */
#define TEXT_ID_LEVEL_BRIEFING_BASE 0x230017
/* Extension code ".flm" (movie) for WidePath_SetExtensionCode, as the WIDE_PATH_EXTENSION_* in core/text/path.h */
#ifndef WIDE_PATH_EXTENSION_FLM
#define WIDE_PATH_EXTENSION_FLM 0x6D6C66
#endif

/* Functions are grouped by semantic ownership. */

void FrontendMissionBriefingPage_Initialize(UiRootNode *frontendRoot);

#endif /* THANDOR_UI_FRONTEND_MISSION_BRIEFING_H */

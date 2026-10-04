/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/include/thandor/ui/frontend/display_settings.h
 * Reverse engineering by idkFoxes 2026
 */

#ifndef THANDOR_UI_FRONTEND_DISPLAY_SETTINGS_H
#define THANDOR_UI_FRONTEND_DISPLAY_SETTINGS_H

#include <thandor/ui/controls/types.h>
#include <thandor/ui/frontend/types.h>
#include <thandor/core/contracts.h>

void FrontendDisplaySettingsAction_OpenPageAndListModes(FrontendDisplaySettingsPageOptionState *source);

void FrontendDisplaySettingsAction_SelectAdapter(UiNodeBase *sourceNode);
/* Not in the original: handler of the display mode kind choices (FRONTEND_ACTION_DISPLAY_MODE_KIND_*). */
void FrontendDisplaySettingsAction_SelectDisplayModeKind(UiNodeBase *sourceNode);

void FrontendDisplaySettingsAction_ApplyPendingResolution(UiNodeBase *optionButton);

void FrontendDisplaySettings_ApplyMode(void *control);

void FrontendDisplaySettingsPage_UpdateModeActionAvailability(UiNodeBase *frontendRoot);

/* Not in the original: the advanced settings page ("Erweitert", displayPageStack page 1). OpenPage is the options
   page's advancedSettingsButton (FRONTEND_ACTION_OPEN_ADVANCED_SETTINGS); the others are its choices
   (FRONTEND_ACTION_ADVANCED_*): each applies and saves its value at once and refreshes the page. */
void FrontendAdvancedSettingsAction_OpenPage(UiNodeBase *sourceNode);
void FrontendAdvancedSettingsAction_SelectEdges(UiNodeBase *sourceNode);
void FrontendAdvancedSettingsAction_SelectUiScale(UiNodeBase *sourceNode);
void FrontendAdvancedSettingsAction_SelectFrameLimit(UiNodeBase *sourceNode);
void FrontendAdvancedSettingsAction_SetVsync(UiNodeBase *sourceNode);

extern FrontendUiScratch g_FrontendUiDisplayModeAndTaskAssignmentScratch; /* followed by 4 bytes 0x90 fill (dropped) */

#endif /* THANDOR_UI_FRONTEND_DISPLAY_SETTINGS_H */

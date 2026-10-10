/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/include/thandor/ui/frontend/campaign_tab.h
 * Project code (not in the original game)
 */

#ifndef THANDOR_UI_FRONTEND_CAMPAIGN_TAB_H
#define THANDOR_UI_FRONTEND_CAMPAIGN_TAB_H

/* The campaign tab of the "Choose game" page with the campaign map (docs/plans/campaign_map.md 6, 7.1-7.4): a row
   of campaign buttons with the progress and the mission map of the selected campaign, in place of the original
   campaigns list. The list stays filled as before (it is only taken out of the page while the map is shown) and
   stays the source of the row index of the Start button and of the network select command. With more than
   FRONTEND_CAMPAIGN_BUTTON_COUNT campaigns, or when a catalog campaign cannot be read natively, the tab shows the
   original list. Everything here uses native memory only (no arena); the graph, layout and node states the map
   control draws live in this module's static storage. */

#include <stdint.h>
#include <thandor/ui/controls/types.h>
#include <thandor/ui/frontend/types.h>

/* Puts the original campaigns list back into the campaign page (the original page layout), with the campaign map
   nodes suppressed. Called before the list is rebuilt, so the original rebuild acts on the list as it always did. */
void FrontendCampaignTab_ShowList();

/* After the campaigns list was rebuilt (ScenarioCatalog_RebuildCampaignRecordListPage): reads the progress, groups
   the list rows into campaigns, builds every campaign's graph, layout and node states, and shows the button row
   and the map of the campaign of the list's selected row (plan 7.1); falls back to the list (FrontendCampaignTab_
   ShowList) when the map cannot be shown. */
void FrontendCampaignTab_Refresh();

/* After the campaigns list selected row selectionIndex (ScenarioCatalog_SelectCampaignAndShowDescription, also
   as the mirrored network command): while the map is shown, selects the campaign of that row and the mission
   levelId (the select command's first argument, sent by a host of open-thandor; 0 or a level not on the map:
   the default mission, plan 7.1). */
void FrontendCampaignTab_SelectRow(UiListRowIndex selectionIndex,uint32_t levelId);

/* The list row the campaign tab opens with: in a local game while the map is shown, the base row of the
   campaign last selected in this process (plan 7.1); otherwise row 0, as the original. */
UiListRowIndex FrontendCampaignTab_InitialRow();

/* For the Start button on the campaign tab (FrontendScenarioSelection_ActivateSelectedRecord): false when the
   map is shown and its selected mission cannot be started (locked, or a carry target without a snapshot or in
   a network game); otherwise true with the start level argument of FRONTEND_COMMAND_LOAD_CAMPAIGN in
   *outStartLevelId (0: the campaign's first level, the original start; the list is shown, or the selected
   mission is the first one). */
bool FrontendCampaignTab_StartLevel(uint32_t *outStartLevelId);

/* Handler of FRONTEND_ACTION_SELECT_CAMPAIGN_BUTTON (handlers60 of g_FrontendUiActionHandlersPage20): selects that
   campaign's base row like a click on the list (FrontendCommand_Issue of ScenarioCatalog_SelectCampaignAndShow
   Description, mirrored on network peers). Network clients only watch: their click is undone. */
void FrontendCampaignTab_SelectCampaignButton(UiNodeBase *sourceNode);

/* Binds what the campaign tab's nodes need once the frontend root exists (Frontend_Init): the click sound of the
   campaign buttons, the selected style of the mission heading and the map's node callback. */
void FrontendCampaignTab_BindNodes(FrontendUiImage *ui);

#endif /* THANDOR_UI_FRONTEND_CAMPAIGN_TAB_H */

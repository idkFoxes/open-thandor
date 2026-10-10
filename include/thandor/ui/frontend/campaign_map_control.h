/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/include/thandor/ui/frontend/campaign_map_control.h
 * Project code (not in the original game)
 */

#ifndef THANDOR_UI_FRONTEND_CAMPAIGN_MAP_CONTROL_H
#define THANDOR_UI_FRONTEND_CAMPAIGN_MAP_CONTROL_H

/* CM13 placeholder of the map control's interface (docs/plans/campaign_map.md 6.2, 6.3): exactly the contract the
   map control (CM12) implements. CM12's real header replaces this file at the merge. */

#include <stdint.h>
#include <span>
#include <thandor/ui/controls/types.h>
#include <thandor/ui/frontend/types.h>
#include <thandor/ui/frontend/campaign_map.h>

enum CampaignMapNodeState : uint8_t { CAMPAIGN_MAP_NODE_LOCKED=0, CAMPAIGN_MAP_NODE_UNLOCKED=1, CAMPAIGN_MAP_NODE_DONE=2 };

void FrontendCampaignMapControl_SetGraph(FrontendCampaignMapControl *control, const CampaignGraph *graph,
                                         const CampaignMapLayout *layout, std::span<const CampaignMapNodeState> nodeStates);
void FrontendCampaignMapControl_SetSelectedNode(FrontendCampaignMapControl *control, int nodeIndex);
int FrontendCampaignMapControl_GetSelectedNode(const FrontendCampaignMapControl *control);
using FrontendCampaignMapCallback = void (*)(int nodeIndex, bool startRequested);
void FrontendCampaignMapControl_SetCallback(FrontendCampaignMapControl *control, FrontendCampaignMapCallback callback);
void UiFramedTextButton_SetCheckMark(UiFramedTextButtonControl *button, bool on);

#endif /* THANDOR_UI_FRONTEND_CAMPAIGN_MAP_CONTROL_H */

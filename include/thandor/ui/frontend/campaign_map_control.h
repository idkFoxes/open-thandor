/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/include/thandor/ui/frontend/campaign_map_control.h
 * Project code (not in the original game)
 */

#ifndef THANDOR_UI_FRONTEND_CAMPAIGN_MAP_CONTROL_H
#define THANDOR_UI_FRONTEND_CAMPAIGN_MAP_CONTROL_H

/* The mission map control of the campaign tab (FrontendUiImage campaignMap, g_FrontendCampaignMapControlVtable;
   docs/plans/campaign_map.md 6.2-6.4) and the check mark of a finished campaign on a campaign button (5.5, Q11).

   The map draws, inside the sunken frame of the old campaign list (win.gfx frame style A), the edges of the
   campaign graph as 1-pixel axis-aligned polylines (a carry edge as a double line at -1/+1), the nodes as win.gfx
   button frames with the mission number (done: green fill, unlocked: raised, locked: disabled frame and text,
   selected: pressed frame) and, when the graph has a carry edge, the legend item Einheitenuebernahme at the
   frame's bottom right. Everything is drawn in logical pixels through the framebuffer slots, so the software and
   the GPU renderers and every UI scale draw it alike. Mouse only (Q9): a press on a node selects it (locked nodes
   too; the tab logic disables Start for them, 7.2), a double click on the selected node that is not locked
   requests its start. The graph, the layout and the node states live in native memory (a module table indexed by
   the control's mapIndex); nothing here touches the arena. */

#include <stdint.h>
#include <span>
#include <thandor/ui/controls/types.h>
#include <thandor/ui/frontend/campaign_map.h>
#include <thandor/ui/frontend/types.h>

enum CampaignMapNodeState : uint8_t {
    CAMPAIGN_MAP_NODE_LOCKED = 0,
    CAMPAIGN_MAP_NODE_UNLOCKED = 1,
    CAMPAIGN_MAP_NODE_DONE = 2
};

/* Called on a press on a node: nodeIndex (into graph.nodes) and startRequested false when a node was newly
   selected (a press on the already selected node does nothing), true for a double click on the selected node
   when it is not locked (the tab logic still decides whether the mission can start, e.g. a carry target without a
   snapshot). Never called for a press beside the nodes. */
using FrontendCampaignMapCallback = void (*)(int nodeIndex, bool startRequested);

/* Shows a graph: graph and layout stay owned by the caller, who keeps them alive while the page is shown; the
   layout must come from CampaignMap_Layout(*graph, control layoutWidth, control layoutHeight) (frame-relative
   positions). nodeStates is copied (one per graph node; missing entries count as locked). nullptr graph (or a
   layout that does not match it) shows the empty frame. A selection outside the new graph is cleared. */
void FrontendCampaignMapControl_SetGraph(FrontendCampaignMapControl *control, const CampaignGraph *graph,
                                         const CampaignMapLayout *layout,
                                         std::span<const CampaignMapNodeState> nodeStates);

/* -1 (or any index outside the graph): no selection. Does not call the callback. */
void FrontendCampaignMapControl_SetSelectedNode(FrontendCampaignMapControl *control, int nodeIndex);

/* -1 when no node is selected. */
int FrontendCampaignMapControl_GetSelectedNode(const FrontendCampaignMapControl *control);

/* nullptr: no callback. */
void FrontendCampaignMapControl_SetCallback(FrontendCampaignMapControl *control, FrontendCampaignMapCallback callback);

/* Check mark of a finished campaign (5.5, Q11) on a framed text button (the campaign buttons): on switches the
   button to g_FrontendCampaignCheckButtonVtable, which draws it like g_UiFramedTextButtonControlVtable plus a
   small two-stroke check mark right of the label in the label colour (the selected colour while the button is
   selected), the label cut so the mark always fits; off switches back to g_UiFramedTextButtonControlVtable. Every
   other slot is the framed text button's. */
void UiFramedTextButton_SetCheckMark(UiFramedTextButtonControl *button, bool on);

/* drawClipped of g_FrontendCampaignCheckButtonVtable. */
void UiFramedTextButtonControl_DrawClippedWithCheckMark
          (UiPixelCoordinate clipBottom,UiPixelCoordinate clipRight,UiPixelCoordinate clipTop,
          UiPixelCoordinate clipLeft,UiFramedTextButtonControl *control);

void FrontendCampaignMapControl_DrawClipped
          (UiPixelCoordinate clipBottom,UiPixelCoordinate clipRight,UiPixelCoordinate clipTop,
          UiPixelCoordinate clipLeft,FrontendCampaignMapControl *control);

void FrontendCampaignMapControl_NonRightPress
          (UiPointerWheelDelta wheelDelta,UiPixelCoordinate pointerY,UiPixelCoordinate pointerX,
          FrontendCampaignMapControl *control);

UiNodeBase *FrontendCampaignMapControl_HitTest
          (UiPixelCoordinate pointerY,UiPixelCoordinate pointerX,FrontendCampaignMapControl *control);

extern UiNodeVtable g_FrontendCampaignMapControlVtable;
extern UiNodeVtable g_FrontendCampaignCheckButtonVtable;

#endif /* THANDOR_UI_FRONTEND_CAMPAIGN_MAP_CONTROL_H */

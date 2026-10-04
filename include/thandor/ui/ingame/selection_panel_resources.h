/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/include/thandor/ui/ingame/selection_panel_resources.h
 * Reverse engineering by idkFoxes 2026
 */

#ifndef THANDOR_UI_INGAME_SELECTION_PANEL_RESOURCES_H
#define THANDOR_UI_INGAME_SELECTION_PANEL_RESOURCES_H

#include <thandor/core/types.h>
#include <thandor/gameplay/selection/types.h>
#include <thandor/graphics/resources/types.h>
#include <thandor/core/contracts.h>

Bool8 SelectionInfoPanel_InitResources(SelectionInfoEntitySlots *entitySlots,uint32_t *outError);

void SelectionInfoPanel_ShutdownResources();

extern GraphicsTextureSourceAsset *g_SelectionPanelTextureSource;
extern GraphicsTextureSourceAsset *g_InfoPanelTextureSource;
extern void *g_SelectionPanelData;
extern void *g_InfoPanelData;

#endif /* THANDOR_UI_INGAME_SELECTION_PANEL_RESOURCES_H */

/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/include/thandor/ui/ingame/selection_detail.h
 * Reverse engineering by idkFoxes 2026
 */

#ifndef THANDOR_UI_INGAME_SELECTION_DETAIL_H
#define THANDOR_UI_INGAME_SELECTION_DETAIL_H

#include <thandor/ui/ingame/types.h>
#include <thandor/core/contracts.h>

/* Selection detail panel text templates (patched by InGameUiRuntime_InitializeControlTreeResources, chosen by
   InGameSelectionDetailPanel_Rebuild): 0x18002C.. single selection + the asset's template variant, 0x18003C..
   the same while researching, 0x180045.. hover/placement stats; 0x18004E fills an unused weapon slot. Model
   names are TEXT_ID_MODEL_NAME_BASE (ui/ingame/technology.h) + name index. */
#define TEXT_ID_SELECTION_DETAIL_TEMPLATE_BASE 0x18002C
#define TEXT_ID_SELECTION_DETAIL_RESEARCH_TEMPLATE_BASE 0x18003C
#define TEXT_ID_SELECTION_DETAIL_HOVER_TEMPLATE_BASE 0x180045
#define TEXT_ID_SELECTION_DETAIL_NO_WEAPON 0x18004E

void InGameSelectionDetailPanel_Rebuild();

extern UiSelectionDetailTextBuffer64Utf16 g_InGameSelectionDetailNameTextUtf16;
extern UiSelectionDetailTextBuffer64Utf16 g_InGameSelectionDetailArmourTextUtf16;
extern UiSelectionDetailTextBuffer64Utf16 g_InGameSelectionDetailWeaponName0TextUtf16;
extern UiSelectionDetailTextBuffer64Utf16 g_InGameSelectionDetailWeaponName1TextUtf16;
extern UiSelectionDetailTextBuffer64Utf16 g_InGameSelectionDetailWeaponName2TextUtf16;
extern UiSelectionDetailTextBuffer64Utf16 g_InGameSelectionDetailTextSlot05Utf16;
extern UiSelectionDetailTextBuffer64Utf16 g_InGameSelectionDetailBuildXeniteCostTextUtf16;
extern UiSelectionDetailTextBuffer64Utf16 g_InGameSelectionDetailBuildTimeTextUtf16;
extern UiSelectionDetailTextBuffer64Utf16 g_InGameSelectionDetailEnergyTextUtf16;
extern UiSelectionDetailTextBuffer64Utf16 g_InGameSelectionDetailTextSlot09Utf16;
extern int g_InGameSelectionDetailGridCellOffsets[12];

#endif /* THANDOR_UI_INGAME_SELECTION_DETAIL_H */

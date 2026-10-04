/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/include/thandor/ui/ingame/editor_tool_selection.h
 * Reverse engineering by idkFoxes 2026
 */

#ifndef THANDOR_UI_INGAME_EDITOR_TOOL_SELECTION_H
#define THANDOR_UI_INGAME_EDITOR_TOOL_SELECTION_H

#include <thandor/ui/controls/types.h>
#include <thandor/ui/ingame/types.h>
#include <thandor/core/contracts.h>

/* Action id of resultsSecondaryExitButton (suppressed in local games) */
#define INGAME_ACTION_RESULTS_SECONDARY_EXIT 0x1025
#define UI_COMMAND_RUNTIME_FLAG_COMMAND_POINTER_CAPTURED 0x80 /* a command-mode click captured the pointer
                                                                 (InGameWorldInput_BeginPointerCapture); the
                                                                 release then issues the mode command */
/* g_UiCommandModeG: active tab of the map editor (InGameCommandModeG_Select0..5, InGameUiImage.editorModeTab*);
   the tools of each tab are g_UiCommandModeC (height), D (material), E (smoothing), A (unit placement) and
   B (object placement). */
#define EDITOR_MODE_TERRAIN_HEIGHT 0
#define EDITOR_MODE_TERRAIN_MATERIAL 1
#define EDITOR_MODE_TERRAIN_SMOOTHING 2
#define EDITOR_MODE_UNIT_PLACEMENT 3
#define EDITOR_MODE_OBJECT_PLACEMENT 4
#define EDITOR_MODE_REGION 5

/* Terrain material swatches of the material tool (UiCommandMatrix_SelectIndex): twelve per page, the page
   scrolls in rows of three */
#define MATERIAL_SWATCH_COUNT 12
#define MATERIAL_SWATCH_ROW_LENGTH 3
/* Relaxation passes of the smoothing page buttons (InGameCommandRange_DispatchState0/1) */
#define TERRAIN_RELAXATION_BUTTON_PASSES 128
/* g_UiCommandModeGColorVariantFlags bit and g_UiCommandModeGColorVariantLimit values of the two terrain colour
   variants (UiCommandModeG_ApplyMaskedColorVariant / _ApplyRawColorVariant) */
#define UI_COMMAND_MODE_G_COLOR_VARIANT_MASKED 0x1000
#define UI_COMMAND_MODE_G_COLOR_LIMIT_MASKED 0x7FFFFFFF
#define UI_COMMAND_MODE_G_COLOR_LIMIT_RAW 0x00FFFFFF

void InGameCommandModeG_Select0(UiSelectableControl *source);

void InGameCommandModeG_Select1(UiSelectableControl *source);

void InGameCommandModeG_Select2(UiSelectableControl *source);

void InGameCommandModeG_Select3(UiSelectableControl *source);

void InGameCommandModeG_Select4(UiSelectableControl *source);

void InGameCommandModeG_Select5(UiSelectableControl *source);

void InGameCommandMatrix_SelectMappedControl(UiNodeBase *source);

void UiCommandModeG_HideGridVertexMarkers(WorldRuntimeContext *context);

void InGameCommandModeC_Select0(UiSpriteButtonControl *source);

void InGameCommandModeC_Select1(UiSpriteButtonControl *source);

void InGameCommandModeC_Select2(UiSpriteButtonControl *source);

void InGameCommandModeC_Select3(UiSpriteButtonControl *source);

void InGameCommandModeD_Select0(UiSpriteButtonControl *source);

void InGameCommandModeD_Select1(UiSpriteButtonControl *source);

void InGameCommandModeD_Select2(UiSpriteButtonControl *source);

void InGameCommandModeD_Select3(UiSpriteButtonControl *source);

void InGameCommandModeA_Select0(UiSpriteButtonControl *source);

void InGameCommandModeA_Select1(UiSpriteButtonControl *source);

void InGameCommandModeA_Select2(UiSpriteButtonControl *source);

void InGameCommandModeB_Select0(UiSpriteButtonControl *source);

void InGameCommandModeB_Select1(UiSpriteButtonControl *source);

void InGameCommandModeB_Select2(UiSpriteButtonControl *source);

void InGameCommandModeE_Select0(UiSpriteButtonControl *source);

void InGameCommandModeE_Select1(UiSpriteButtonControl *source);

void InGameCommandModeE_Select2(UiSpriteButtonControl *source);

void InGameCommandRange_DispatchState0(UiNodeBase *source);

void InGameCommandRange_DispatchState1(UiNodeBase *source);

void InGameCommandModeF_Select0(UiSpriteButtonControl *source);

void InGameCommandModeF_Select1(UiSpriteButtonControl *source);

void UiCommandModeG_ApplyMaskedColorVariant(void *worldRuntime);

void UiCommandModeG_ShowRegionMarkers(WorldRuntimeContext *context);

void UiCommandMatrix_SelectIndex(UiCommandModeIndex absoluteIndex,UiNodeBase *root);

void UiCommandModeG_HideSurfacePointMarker(WorldRuntimeContext *context);

void UiCommandModeG_ShowTerrainPointMarkers(WorldRuntimeContext *context);

void UiCommandModeG_SetSecondarySurfaceOnly(WorldRuntimeContext *context);

void UiCommandModeG_ShowArmyMetrics(WorldRuntimeContext *context);

void UiCommandModeG_HideArmyMetricsAndEndDragSelect(WorldRuntimeContext *context);

void UiCommandModeG_ShowSurfacePointMarker(WorldRuntimeContext *context);

void UiCommandModeG_HideTerrainPointMarkers(WorldRuntimeContext *context);

void UiCommandModeG_ClearSecondarySurfaceOnly(WorldRuntimeContext *context);

void UiCommandModeG_ApplyRawColorVariant(void *worldRuntime);

void UiCommandModeG_HideRegionMarkers(WorldRuntimeContext *context);

void UiCommandModeG_ShowGridVertexMarkers(WorldRuntimeContext *context);

InGameRuntimeRoot * UiCommandModeG_SelectAndSyncPages(UiCommandModeIndex modeIndex,UiSelectableControl *source);

extern uint32_t g_UiCommandModeGColorVariantFlags; /* uint32_t render-state flag word copied into terrain packets (graphics/terrain/terrain_render.cpp); the mode-G handlers in ui/ingame/editor_tool_selection.cpp set/clear the masked G-colour variant bit */
extern uint32_t g_UiCommandModeE;
extern uint32_t g_UiCommandModeA;
extern uint32_t g_UiCommandModeB; /* followed in the original by an all-zero dword no code reaches (dropped) */
extern uint32_t g_UiCommandModeF;

extern void (*g_UiCommandModeGHandlers[6])(UiSelectableControl *);

extern uint32_t g_UiCommandModeGColorVariantLimit; /* uint32_t ARGB mask applied to terrain vertex diffuse colours (0x00FFFFFF raw, other value in masked command mode); its alpha byte also switches overlay/projection paths */

#endif /* THANDOR_UI_INGAME_EDITOR_TOOL_SELECTION_H */

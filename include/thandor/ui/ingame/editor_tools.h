/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/include/thandor/ui/ingame/editor_tools.h
 * Reverse engineering by idkFoxes 2026
 */

#ifndef THANDOR_UI_INGAME_EDITOR_TOOLS_H
#define THANDOR_UI_INGAME_EDITOR_TOOLS_H

#include <thandor/core/types.h>
#include <thandor/graphics/render/types.h>
#include <thandor/ui/ingame/types.h>
#include <thandor/core/contracts.h>

/* Editor tools of the in-game map callbacks: a Q12 grid coordinate (world point through
   FIELD_GRID_WORLD_*_Q20) is rounded to a whole cell by adding the bias and dropping the fraction
   (INGAME_SNAP_GRID_Q12, or + bias >> Q12_SHIFT for the cell index). */
#define INGAME_GRID_SNAP_BIAS_Q12 0x3ffU
#define INGAME_SNAP_GRID_Q12(value) ((value) + INGAME_GRID_SNAP_BIAS_Q12 & ~(uint32_t)Q12_FRACTION_MASK)
/* Cursor frames the editor tools show (InGameUiCommand_ResolveCursorCodeByMode); placement and moving use
   WORLD_CURSOR_MOVE / _NO_TARGET / _OWN_ARMY / _FOREIGN_ARMY */
#define EDITOR_CURSOR_DELETE_TARGET 0x19   /* delete tool over a model */
#define EDITOR_CURSOR_DELETE_NONE 0x1A     /* delete tool, nothing under the pointer */
#define EDITOR_CURSOR_HEIGHT_RAISE 0x1C
#define EDITOR_CURSOR_SMOOTH 0x1D
#define EDITOR_CURSOR_HEIGHT_LOWER 0x1E
#define EDITOR_CURSOR_RECEIVER_MASK 0x1F   /* smoothing tab, fluid receiver exclusion */
#define EDITOR_CURSOR_REBUILD_INFLUENCE 0x20
#define EDITOR_CURSOR_REGION 0x21
#define EDITOR_CURSOR_PAINT 0x22           /* material paint, and the fluid source exclusion */
#define EDITOR_CURSOR_MATERIAL_MODE2 0x23
#define EDITOR_CURSOR_MATERIAL_MODE1 0x24
/* Region tool: bit 31 of the region argument makes the drag remove the region flag again */
#define INGAME_REGION_MASK_REMOVE 0x80000000u
/* Editor drag deltas: screen dx in the low word (masked unless Shift/Ctrl), dy times this in the high word */
#define INGAME_DRAG_DELTA_X_MASK 0xffff
#define INGAME_DRAG_DELTA_Y_SCALE 0x10000
/* WorldRuntimeContext.runtimeFlags bit set while the map editor is active
   (InGameUiCommandRuntime_ApplyInteractionSubsystemActiveState) */
#define INGAME_WORLD_FLAG_EDITOR 0x400000u

#define INGAME_PANEL_SUBRESOURCE_NOTIFICATION_IDLE 0x25

uint32_t InGameUiCommand_ResolveCursorCodeByMode
                (UiPointerRegionCode pointerRegionCode,Q12 pointerWorldXQ12,Q12 pointerWorldYQ12,
                uint32_t reservedArg3,WorldOwnerListNode *ownerNodeUnderPointer,
                WorldRuntimeContext *worldRuntime);

void InGameUiCommand_BeginInteractionByMode
          (UiPointerRegionCode pointerRegionCode,Q12 pointerX,Q12 pointerY,uint32_t reservedArg3,
          WorldOwnerListNode *ownerNodeUnderPointer,WorldRuntimeExtendedMapControlView *mapControl
          );

void InGameUiCommand_UpdateInteractionByMode(UiPointerRegionCode pointerRegionCode,GraphicsScreenCoordinate pointerX,
          GraphicsScreenCoordinate pointerY,uint32_t reservedArg3,int optionalContext,
          WorldRuntimeExtendedMapControlView *mapControl);

void InGameUiCommand_EndInteractionByMode
          (uint32_t callbackArg0,uint32_t callbackArg1,uint32_t callbackArg2,uint32_t callbackArg3,
          WorldOwnerListNode *worldNode,WorldRuntimeContext *worldRuntime);

void InGameUiCommand_ResetInteractionByMode(WorldRuntimeContext *worldRuntime);

void InGameUiCommandRuntime_ApplyInteractionSubsystemActiveState
          (uint32_t playerRuntimeId,uint32_t unusedPayload1,uint32_t unusedPayload2,uint32_t activeStateFlags);

void InGameUiCommand_SaveFieldAndLevelAssetImages
          (uint32_t playerRuntimeId,uint32_t unusedPayload1,uint32_t unusedPayload2,uint32_t unusedPayload3);

extern uint32_t g_LocalPlayerRuntimeId;

extern int32_t g_InGameSelectionInsertTripletDwordCount;
extern int32_t g_InGameSelectionRemoveTripletDwordCount; /* followed by 4 bytes 0x90 fill (dropped) */

extern uint32_t g_UiCommandModeG;
extern uint32_t g_UiCommandModeC;
extern uint32_t g_UiCommandModeD;
extern uint32_t g_UiCommandAbsoluteSelectionIndex;
extern uint32_t g_UiCommandTerrainMaskToggleValue;
extern FactionRuntimeIndex g_UiCommandModeGOwnerFactionIndex;

extern uint32_t g_UiCommandCallerMaskHighBit;
extern const uint32_t g_UiCommandModeGPrimaryPageIndices[6]; /* uint32_t[6]: active page of the mode preview page stack per command mode G; ui/ingame commands/runtime */
extern const uint32_t g_UiCommandModeGSecondaryPageIndices[6]; /* uint32_t[6]: active page of the mode detail page stack per command mode G; ui/ingame commands/runtime */
extern const uint32_t g_UiCommandModeGTertiaryPageIndices[6]; /* uint32_t[6]: active page of the mode command page stack per command mode G; ui/ingame commands/runtime */

#endif /* THANDOR_UI_INGAME_EDITOR_TOOLS_H */

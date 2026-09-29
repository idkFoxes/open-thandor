/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/include/thandor/ui/ingame/commands.h
 * Reverse engineering by idkFoxes 2026
 */

#ifndef THANDOR_UI_INGAME_COMMANDS_H
#define THANDOR_UI_INGAME_COMMANDS_H

#include <thandor/generated/types.h>
#include <thandor/core/contracts.h>

/* Submodule: ui/ingame/commands. */

/* g_UiCommandRuntimeFlags bits that end the in-game session loop (InGameRuntime_RunSessionUntilExit) */
#define UI_COMMAND_RUNTIME_FLAG_END_MOVIE_PENDING 0x800 /* an end trigger fired and chose the end movie
                                                           (set in gameplay/session/runtime.c) */
#define UI_COMMAND_RUNTIME_FLAG_SESSION_CLOSED 0x10000 /* command 150 with flag bit 1: the session is closed
                                                          (InGameCommand_HandlePlayerDeparture) */
#define UI_COMMAND_RUNTIME_FLAG_LOCAL_PLAYER_LEFT 0x20000 /* command 150 reported the local player's departure */
/* g_UiCommandRuntimeFlags bits that gate the simulation step (InGameRuntime_UpdateSimulationAndNetworkTick) */
#define UI_COMMAND_RUNTIME_FLAG_PAUSED 0x01 /* toggled once every player agrees
                                               (InGameCommand_TogglePauseRequest); set at session start */
#define UI_COMMAND_RUNTIME_FLAG_LOCAL_FACTION_ENDED 0x08 /* an end trigger ended the local faction
                                                            (InGameConditionRuntime_UpdateScheduledRecords);
                                                            the step then sets occupancy bit 0 on every cell */
#define UI_COMMAND_RUNTIME_FLAG_WAITING_FOR_PLAYERS 0x10 /* set with PAUSED at session start, cleared with it when
                                                            every player is ready
                                                            (FrontendPlayerRuntime_IncrementReadyCountAndResolveConsensus) */
#define UI_COMMAND_RUNTIME_FLAG_WORLD_INPUT_DISABLED 0x100 /* set with LOCAL_FACTION_ENDED; the world input
                                                              handlers (gameplay/input/world.c) then ignore the map */
/* g_UiCommandRuntimeFlags bit that ends the results screen after the end movie (Frontend_PlaySelectedEndMovie) */
#define UI_COMMAND_RUNTIME_FLAG_RESULTS_CLOSED 0x1000 /* set by the results buttons (actions 0x101B and 0x1025,
                                                         ui/ingame/commands.c) */
/* further g_UiCommandRuntimeFlags bits (gameplay/session/runtime.c, gameplay/ai/planning.c) */
#define UI_COMMAND_RUNTIME_FLAG_AI_PLANNING_OFF 0x02 /* skips the AI planning phase in local games; no writer
                                                        with a constant mask in the original, so it can only come
                                                        from UiCommandRuntimeFlags_ApplyClearSetToggleMasks */
#define UI_COMMAND_RUNTIME_FLAG_INTERACTION_SUBSYSTEM_ACTIVE 0x04 /* set with PAUSED by
                                                                     InGameUiCommandRuntime_ApplyInteractionSubsystemActiveState;
                                                                     world sounds, camera keys and the full
                                                                     simulation step are skipped meanwhile */
#define UI_COMMAND_RUNTIME_FLAG_PLACEMENT_PENDING 0x20 /* an army asset waits for placement on the map
                                                          (InGameCommand_ExecuteLocalPlacementFromSelection) */
#define UI_COMMAND_RUNTIME_FLAG_PLACEMENT_OVERLAY_SHOWN 0x2000 /* the placement overlay was drawn onto the field
                                                                  grid (EndGameResultsUiRuntime_UpdateAndHandleInput) */
#define UI_COMMAND_RUNTIME_FLAG_CHEATS_ENABLED 0x40000 /* toggled by typing the cheat code into the chat line
                                                          (InGameUiAction1024_Handler) */
#define UI_COMMAND_RUNTIME_FLAG_CHEAT_FAST_BUILD 0x100000 /* cheat hotkey: build and research times / 16 */
/* SelectionPlayerRuntimeBlock.sessionFlags bit: the player asks for a pause (shown as "P" in the player roster;
   toggled by InGameCommand_TogglePauseRequest) */
#define PLAYER_SESSION_FLAG_PAUSE_REQUESTED 0x01
/* sessionFlags bit: the player's machine renders too few frames (set/cleared through command 0x340 by
   InGameHud_UpdateStatusCountersAndSessionPrompts; shown as a highlighted "W" in the player roster) */
#define PLAYER_SESSION_FLAG_SLOW_RENDERING 0x02
/* g_UiCommandRuntimeFlags bits of windows that pause a local game while open (mission help:
   InGameUiAction101F_Handler, settings: InGameSettingsPage_ToggleAndSynchronizeControls) */
#define UI_COMMAND_RUNTIME_FLAG_WINDOW_PAUSE 0x4000 /* an open window paused the game */
#define UI_COMMAND_RUNTIME_FLAG_PAUSED_BEFORE_WINDOW 0x400 /* the game was already paused when it opened */
/* UI action toggling the technology window (InGameSelectionPage_ToggleAndRefreshPage2); suppressed by
   InGameSelectionDetailPanel_Rebuild when the single selected army has no available technology */
#define INGAME_ACTION_TECHNOLOGY_WINDOW 0x1010
/* Buttons of the quit game window (InGameUiImage.quitMenuSurrenderButton / quitMenuRestartMissionButton) */
#define INGAME_ACTION_QUIT_SURRENDER 0x101E /* command 150 mode 1: destroys the local faction's armies */
#define INGAME_ACTION_QUIT_RESTART_MISSION 0x1027 /* command 150 mode 2 (label unverified) */
/* flags of InGameCommand_HandlePlayerDeparture; neither bit: the player left the session */
#define INGAME_PLAYER_DEPARTURE_FLAG_SURRENDER 0x01 /* destroy every army of the player's faction */
#define INGAME_PLAYER_DEPARTURE_FLAG_CLOSE_SESSION 0x02 /* sets UI_COMMAND_RUNTIME_FLAG_SESSION_CLOSED */

/* Action id of the results screen's resultsContinueButton (InGameCommandAction_SetFlag1000OrMarkReady);
   a network host only shows it once every client has pressed its own
   (FrontendPlayerRuntime_MarkResultsReadyAndUpdateContinueButton). */
#define INGAME_ACTION_RESULTS_CONTINUE 0x101B
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
/* Bit of the last argument of InGameUiCommandRuntime_ApplyInteractionSubsystemActiveState (command
   INGAME_COMMAND_EDITOR_ACTIVE_STATE): set leaves the editor, clear enters it. */
#define EDITOR_ACTIVE_STATE_LEAVE 0x04
/* g_UiCommandRuntimeFlags bit hiding the world view status texts (UiCommandVisibility*Text_DrawWhenAllowed); no
   writer with a constant mask, so it can only come from command 0x310 */
#define UI_COMMAND_RUNTIME_FLAG_HIDE_WORLD_TEXTS 0x200
/* labelFlags bits of those world view status texts */
#define UI_WORLD_TEXT_PAUSED_ONLY 0x800 /* drawn only while the game is paused */
#define UI_WORLD_TEXT_SHIFT_BY_STEP_TICKS 0x1000 /* needs g_InGameSimulationStepTicks > 1; text shifted by ticks - 2
                                                    bytes */
/* Army stock panel (UiCommandSpriteVariantA_*, g_UiCommandSpriteVariantARecords) */
#define ARMY_STOCK_ENTRY_COUNT 24
#define ARMY_STOCK_MAX_COLUMNS 4
#define INGAME_CURSOR_FRAME_ARMY_STOCK 10 /* pointer over an army stock slot */
#define INGAME_CURSOR_FRAME_ARMY_STOCK_SELL 12 /* the same with Ctrl held: a click sells the army */
/* Message history text of another player's departure (rich text: selector 0 = player name) */
#define TEXT_ID_PLAYER_DEPARTED 0xFF08
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

/* Functions are grouped by semantic ownership; address comments are executable virtual addresses. */

/* 0x0056DB40 */
void InGameCommandModeG_Select0(UiSelectableControl *source);

/* 0x0056DB90 */
void InGameCommandModeG_Select1(UiSelectableControl *source);

/* 0x0056DBE0 */
void InGameCommandModeG_Select2(UiSelectableControl *source);

/* 0x0056DC30 */
void InGameCommandModeG_Select3(UiSelectableControl *source);

/* 0x0056DCA0 */
void InGameCommandModeG_Select4(UiSelectableControl *source);

/* 0x0056DCF0 */
void InGameCommandModeG_Select5(UiSelectableControl *source);

/* 0x0056AC50 */
void InGameCommandAction_SetFlag1000OrMarkReady(void *source);

/* 0x0056ACC0 */
void InGameCommandAction_ToggleRuntimeFlag0800(void *source);

/* 0x0056D6C0 */
void InGameCommandState_CloseSettingsAndDispatchOperation150(UiNodeBase *source);

/* 0x0056DFD0 */
void InGameCommandMatrix_SelectMappedControl(UiNodeBase *source);

/* 0x00516360 */
void UiCommandSpriteButtonControl_BeginPress
          (UiPointerWheelDelta wheelDelta,UiPixelCoordinate pointerY,UiPixelCoordinate pointerX,
          UiCommandSpriteButtonControl *control);

/* 0x005163A0 */
void UiCommandSpriteButtonControl_NonRightRelease
          (UiPointerWheelDelta wheelDelta,UiPixelCoordinate pointerY,UiPixelCoordinate pointerX,
          UiCommandSpriteButtonControl *control);

/* 0x00516410 */
void UiCommandSpriteButtonControl_RightRelease
          (UiPointerWheelDelta wheelDelta,UiPixelCoordinate pointerY,UiPixelCoordinate pointerX,
          UiCommandSpriteButtonControl *control);

/* 0x00516490 */
GraphicsCursorFrameIndex UiCommandSpriteVariantA_PointerMove(UiPixelCoordinate pointerY,UiPixelCoordinate pointerX,
          UiCommandSpriteButtonControl *control);

/* 0x00517F60 */
void UiCommandVisibilityWrappedText_DrawWhenAllowed
          (UiPixelCoordinate clipTop,UiPixelCoordinate clipLeft,UiPixelCoordinate clipBottom,
          UiPixelCoordinate clipRight,UiNodeBase *control);

/* 0x00518010 */
void UiCommandVisibilitySingleLineText_DrawWhenAllowed
          (UiPixelCoordinate clipTop,UiPixelCoordinate clipLeft,UiPixelCoordinate clipBottom,
          UiPixelCoordinate clipRight,UiNodeBase *control);

/* 0x0055F4A0 */
void InGameCommand_TogglePauseRequest
          (PlayerRuntimeId playerRuntimeId,uint32_t callbackArg1,uint32_t callbackArg2,uint32_t callbackArg3);

/* 0x005604D0 */
void InGameCommand_ExecuteLocalPlacementFromSelection(PlayerRuntimeId playerId,CommandPayloadDword04 headingAngle,
          CommandPayloadDword08 worldXQ12,CommandPayloadDword0C worldYQ12);

/* 0x0056A2A0 */
void UiCommandSpriteVariantA_RebuildGrid(UiNodeBase *node);

/* 0x0056AFD0 */
void InGameCommandAction_ClearSelectedArmyTokenAndClosePage(UiNodeBase *control);

/* 0x0056C660 */
void InGameCommandPanel_OpenPage4AndRefreshAvailability(InGameCommandPanelSourceAddress32 source);

/* 0x0056CFA0 */
void InGameCommandCatalog_SubmitGroup48Entry(UiCatalogEntryControl *source);

/* 0x0056D090 */
void InGameCommandCatalog_SubmitGroup42Entry(UiCatalogEntryControl *source);

/* 0x0056D180 */
void InGameCommandSprite_DispatchVariantAControl24(UiCommandSpriteButtonControl *control);

/* 0x0056D540 */
void InGameCommandSprite_DispatchFixedControl8(UiCommandSpriteButtonControl *control);

/* 0x0056D620 */
void InGameCommandState_SetRuntimeFlag1000(UiNodeBase *source);

/* 0x0056D640 */
void InGameCommandState_SelectAndPropagateBinaryMode(UiSelectableControl *source);

/* 0x0056D920 */
void UiCommandModeG_HideGridVertexMarkers(WorldRuntimeContext *context);

/* 0x0056DD50 */
void InGameCommandModeC_Select0(UiSpriteButtonControl *source);

/* 0x0056DDA0 */
void InGameCommandModeC_Select1(UiSpriteButtonControl *source);

/* 0x0056DDF0 */
void InGameCommandModeC_Select2(UiSpriteButtonControl *source);

/* 0x0056DE40 */
void InGameCommandModeC_Select3(UiSpriteButtonControl *source);

/* 0x0056DE90 */
void InGameCommandModeD_Select0(UiSpriteButtonControl *source);

/* 0x0056DEE0 */
void InGameCommandModeD_Select1(UiSpriteButtonControl *source);

/* 0x0056DF30 */
void InGameCommandModeD_Select2(UiSpriteButtonControl *source);

/* 0x0056DF80 */
void InGameCommandModeD_Select3(UiSpriteButtonControl *source);

/* 0x0056E050 */
void InGameCommandModeA_Select0(UiSpriteButtonControl *source);

/* 0x0056E090 */
void InGameCommandModeA_Select1(UiSpriteButtonControl *source);

/* 0x0056E0D0 */
void InGameCommandModeA_Select2(UiSpriteButtonControl *source);

/* 0x0056E110 */
void InGameCommandModeB_Select0(UiSpriteButtonControl *source);

/* 0x0056E150 */
void InGameCommandModeB_Select1(UiSpriteButtonControl *source);

/* 0x0056E190 */
void InGameCommandModeB_Select2(UiSpriteButtonControl *source);

/* 0x0056E1D0 */
void InGameCommandModeE_Select0(UiSpriteButtonControl *source);

/* 0x0056E220 */
void InGameCommandModeE_Select1(UiSpriteButtonControl *source);

/* 0x0056E260 */
void InGameCommandModeE_Select2(UiSpriteButtonControl *source);

/* 0x0056E2A0 */
void InGameCommandRange_DispatchState0(UiNodeBase *source);

/* 0x0056E2E0 */
void InGameCommandRange_DispatchState1(UiNodeBase *source);

/* 0x0056E320 */
void InGameCommandModeF_Select0(UiSpriteButtonControl *source);

/* 0x0056E370 */
void InGameCommandModeF_Select1(UiSpriteButtonControl *source);

/* 0x00570F20 */
void UiCommandRuntime_CallbackNoOp(void);

/* 0x0055F280 */
void InGameCommand_HandlePlayerDeparture
          (PlayerOrFactionRuntimeId32 playerOrFactionId,uint32_t value1,uint32_t value2,
          GameEntityCommandFlags flags);

/* 0x0056D980 */
void UiCommandModeG_ApplyMaskedColorVariant(void *worldRuntime);

/* 0x0056DA50 */
void UiCommandModeG_ShowRegionMarkers(WorldRuntimeContext *context);

/* 0x00571440 */
void UiCommandMatrix_SelectIndex(UiCommandModeIndex absoluteIndex,UiNodeBase *root);

/* 0x0055F440 */
void UiCommandRuntimeFlags_ApplyClearSetToggleMasks(PlayerRuntimeId playerRuntimeId,UiCommandRuntimeFlagMask toggleMask,
          UiCommandRuntimeFlagMask setMask,UiCommandRuntimeFlagMask clearMask);

/* 0x0056D860 */
void UiCommandModeG_HideSurfacePointMarker(WorldRuntimeContext *context);

/* 0x0056D880 */
void UiCommandModeG_ShowTerrainPointMarkers(WorldRuntimeContext *context);

/* 0x0056D940 */
void UiCommandModeG_SetSecondarySurfaceOnly(WorldRuntimeContext *context);

/* 0x0056D8C0 */
void UiCommandModeG_ShowArmyMetrics(WorldRuntimeContext *context);

/* 0x0056D8E0 */
void UiCommandModeG_HideArmyMetricsAndEndDragSelect(WorldRuntimeContext *context);

/* 0x0056D840 */
void UiCommandModeG_ShowSurfacePointMarker(WorldRuntimeContext *context);

/* 0x0056D8A0 */
void UiCommandModeG_HideTerrainPointMarkers(WorldRuntimeContext *context);

/* 0x0056D960 */
void UiCommandModeG_ClearSecondarySurfaceOnly(WorldRuntimeContext *context);

/* 0x0056D9F0 */
void UiCommandModeG_ApplyRawColorVariant(void *worldRuntime);

/* 0x0056DA70 */
void UiCommandModeG_HideRegionMarkers(WorldRuntimeContext *context);

/* 0x0056D900 */
void UiCommandModeG_ShowGridVertexMarkers(WorldRuntimeContext *context);

/* 0x0056DA90 */
InGameRuntimeRootImageC3E4 * UiCommandModeG_SelectAndSyncPages(UiCommandModeIndex modeIndex,UiSelectableControl *source);

#endif /* THANDOR_UI_INGAME_COMMANDS_H */

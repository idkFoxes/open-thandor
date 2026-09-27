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
                                                          (InGameCommand150_HandlePlayerDepartureAndOwnership) */
#define UI_COMMAND_RUNTIME_FLAG_LOCAL_PLAYER_LEFT 0x20000 /* command 150 reported the local player's departure */
/* g_UiCommandRuntimeFlags bits that gate the simulation step (InGameRuntime_UpdateSimulationAndNetworkTick) */
#define UI_COMMAND_RUNTIME_FLAG_PAUSED 0x01 /* toggled once every player agrees (InGameCommandMode_
                                               TogglePlayerFlagBit0AndReconcileGlobal); set at session start */
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

/* Action id of the results screen's resultsContinueButton (InGameCommandAction_SetFlag1000OrMarkReady);
   a network host only shows it once every client has pressed its own
   (FrontendPlayerRuntime_MarkReadyByIdAndUpdateAction101B). */
#define INGAME_ACTION_RESULTS_CONTINUE 0x101B
#define UI_COMMAND_RUNTIME_FLAG_COMMAND_POINTER_CAPTURED 0x80 /* a command-mode click captured the pointer
                                                                 (InGameWorldInput_BeginPointerCapture); the
                                                                 release then issues the mode command */

/* Functions are grouped by semantic ownership; address comments are executable virtual addresses. */

/* 0x0056DB40 */
void __thandor_void_preserve_eax_ecx_edx InGameCommandModeG_Select0(UiSelectableControl *source);

/* 0x0056DB90 */
void __thandor_void_preserve_eax_ecx_edx InGameCommandModeG_Select1(UiSelectableControl *source);

/* 0x0056DBE0 */
void __thandor_void_preserve_eax_ecx_edx InGameCommandModeG_Select2(UiSelectableControl *source);

/* 0x0056DC30 */
void __thandor_void_preserve_eax_ecx_edx InGameCommandModeG_Select3(UiSelectableControl *source);

/* 0x0056DCA0 */
void __thandor_void_preserve_eax_ecx_edx InGameCommandModeG_Select4(UiSelectableControl *source);

/* 0x0056DCF0 */
void __thandor_void_preserve_eax_ecx_edx InGameCommandModeG_Select5(UiSelectableControl *source);

/* 0x0056AC50 */
void __thandor_preserve_eax InGameCommandAction_SetFlag1000OrMarkReady(void *source);

/* 0x0056ACC0 */
void __thandor_preserve_eax InGameCommandAction_ToggleRuntimeFlag0800(void *source);

/* 0x0056D6C0 */
void __thandor_preserve_eax
InGameCommandState_CloseSettingsAndDispatchOperation150(UiNodeBase *source);

/* 0x0056DFD0 */
void __thandor_void_preserve_eax_ecx_edx InGameCommandMatrix_SelectMappedControl(UiNodeBase *source);

/* 0x00516360 */
void __thandor_void_preserve_eax_ecx_edx
UiCommandSpriteButtonControl_BeginPress
          (UiPointerWheelDelta wheelDelta,UiPixelCoordinate pointerY,UiPixelCoordinate pointerX,
          UiCommandSpriteButtonControl *control);

/* 0x005163A0 */
void __thandor_void_preserve_eax_ecx_edx
UiCommandSpriteButtonControl_NonRightRelease
          (UiPointerWheelDelta wheelDelta,UiPixelCoordinate pointerY,UiPixelCoordinate pointerX,
          UiCommandSpriteButtonControl *control);

/* 0x00516410 */
void __thandor_void_preserve_eax_ecx_edx
UiCommandSpriteButtonControl_RightRelease
          (UiPointerWheelDelta wheelDelta,UiPixelCoordinate pointerY,UiPixelCoordinate pointerX,
          UiCommandSpriteButtonControl *control);

/* 0x00516490 */
GraphicsCursorFrameIndex __thandor_eax_preserve_ecx_edx
UiCommandSpriteVariantA_PointerMove
          (UiPixelCoordinate pointerY,UiPixelCoordinate pointerX,
          UiCommandSpriteButtonControl *control);

/* 0x00517F60 */
void __thandor_void_preserve_eax_ecx_edx
UiCommandVisibilityWrappedText_DrawWhenAllowed
          (UiPixelCoordinate clipTop,UiPixelCoordinate clipLeft,UiPixelCoordinate clipBottom,
          UiPixelCoordinate clipRight,UiNodeBase *control);

/* 0x00518010 */
void __thandor_void_preserve_eax_ecx_edx
UiCommandVisibilitySingleLineText_DrawWhenAllowed
          (UiPixelCoordinate clipTop,UiPixelCoordinate clipLeft,UiPixelCoordinate clipBottom,
          UiPixelCoordinate clipRight,UiNodeBase *control);

/* 0x0055F4A0 */
void __thandor_void_preserve_eax_ecx_edx
InGameCommandMode_TogglePlayerFlagBit0AndReconcileGlobal
          (PlayerRuntimeId playerRuntimeId,uint32_t callbackArg1,uint32_t callbackArg2,uint32_t callbackArg3);

/* 0x005604D0 */
void __thandor_void_preserve_eax_ecx_edx
InGameCommand_ExecuteLocalPlacementFromSelection
          (PlayerRuntimeId playerId,CommandPayloadDword04 payloadDword04,
          CommandPayloadDword08 payloadDword08,CommandPayloadDword0C payloadDword0C);

/* 0x0056A2A0 */
void __thandor_void_preserve_eax_ecx_edx UiCommandSpriteVariantA_RebuildGrid(UiNodeBase *node);

/* 0x0056AFD0 */
void __thandor_void_preserve_eax_ecx_edx
InGameCommandAction_ClearSelectedArmyTokenAndClosePage(UiNodeBase *control);

/* 0x0056C660 */
void __thandor_preserve_eax
InGameCommandPanel_OpenPage4AndRefreshAvailability(InGameCommandPanelSourceAddress32 source);

/* 0x0056CFA0 */
void __thandor_void_preserve_eax_ecx_edx
InGameCommandCatalog_SubmitGroup48Entry(UiCatalogEntryControl *source);

/* 0x0056D090 */
void __thandor_void_preserve_eax_ecx_edx
InGameCommandCatalog_SubmitGroup42Entry(UiCatalogEntryControl *source);

/* 0x0056D180 */
void __thandor_void_preserve_eax_ecx_edx
InGameCommandSprite_DispatchVariantAControl24(UiCommandSpriteButtonControl *control);

/* 0x0056D540 */
void __thandor_void_preserve_eax_ecx_edx
InGameCommandSprite_DispatchFixedControl8(UiCommandSpriteButtonControl *control);

/* 0x0056D620 */
void InGameCommandState_SetRuntimeFlag1000(UiNodeBase *source);

/* 0x0056D640 */
void __thandor_void_preserve_eax_ecx_edx
InGameCommandState_SelectAndPropagateBinaryMode(UiSelectableControl *source);

/* 0x0056D920 */
void __thandor_void_preserve_eax_ecx_edx UiCommandModeG_ClearNodeFlag00800000(WorldRuntimeContext *context);

/* 0x0056DD50 */
void __thandor_void_preserve_eax_ecx_edx InGameCommandModeC_Select0(UiSpriteButtonControl *source);

/* 0x0056DDA0 */
void __thandor_void_preserve_eax_ecx_edx InGameCommandModeC_Select1(UiSpriteButtonControl *source);

/* 0x0056DDF0 */
void __thandor_void_preserve_eax_ecx_edx InGameCommandModeC_Select2(UiSpriteButtonControl *source);

/* 0x0056DE40 */
void __thandor_void_preserve_eax_ecx_edx InGameCommandModeC_Select3(UiSpriteButtonControl *source);

/* 0x0056DE90 */
void __thandor_void_preserve_eax_ecx_edx InGameCommandModeD_Select0(UiSpriteButtonControl *source);

/* 0x0056DEE0 */
void __thandor_void_preserve_eax_ecx_edx InGameCommandModeD_Select1(UiSpriteButtonControl *source);

/* 0x0056DF30 */
void __thandor_void_preserve_eax_ecx_edx InGameCommandModeD_Select2(UiSpriteButtonControl *source);

/* 0x0056DF80 */
void __thandor_void_preserve_eax_ecx_edx InGameCommandModeD_Select3(UiSpriteButtonControl *source);

/* 0x0056E050 */
void __thandor_void_preserve_eax_ecx_edx InGameCommandModeA_Select0(UiSpriteButtonControl *source);

/* 0x0056E090 */
void __thandor_void_preserve_eax_ecx_edx InGameCommandModeA_Select1(UiSpriteButtonControl *source);

/* 0x0056E0D0 */
void __thandor_void_preserve_eax_ecx_edx InGameCommandModeA_Select2(UiSpriteButtonControl *source);

/* 0x0056E110 */
void __thandor_void_preserve_eax_ecx_edx InGameCommandModeB_Select0(UiSpriteButtonControl *source);

/* 0x0056E150 */
void __thandor_void_preserve_eax_ecx_edx InGameCommandModeB_Select1(UiSpriteButtonControl *source);

/* 0x0056E190 */
void __thandor_void_preserve_eax_ecx_edx InGameCommandModeB_Select2(UiSpriteButtonControl *source);

/* 0x0056E1D0 */
void __thandor_void_preserve_eax_ecx_edx InGameCommandModeE_Select0(UiSpriteButtonControl *source);

/* 0x0056E220 */
void __thandor_void_preserve_eax_ecx_edx InGameCommandModeE_Select1(UiSpriteButtonControl *source);

/* 0x0056E260 */
void __thandor_void_preserve_eax_ecx_edx InGameCommandModeE_Select2(UiSpriteButtonControl *source);

/* 0x0056E2A0 */
void __thandor_void_preserve_eax_ecx_edx InGameCommandRange_DispatchState0(UiNodeBase *source);

/* 0x0056E2E0 */
void __thandor_void_preserve_eax_ecx_edx InGameCommandRange_DispatchState1(UiNodeBase *source);

/* 0x0056E320 */
void __thandor_void_preserve_eax_ecx_edx InGameCommandModeF_Select0(UiSpriteButtonControl *source);

/* 0x0056E370 */
void __thandor_void_preserve_eax_ecx_edx InGameCommandModeF_Select1(UiSpriteButtonControl *source);

/* 0x00570F20 */
void UiCommandRuntime_CallbackNoOp(void);

/* 0x0055F280 */
void __thandor_void_preserve_eax_ecx
InGameCommand150_HandlePlayerDepartureAndOwnership
          (PlayerOrFactionRuntimeId32 playerOrFactionId,uint32_t value1,uint32_t value2,
          GameEntityCommandFlags flags);

/* 0x0056D980 */
void __thandor_void_preserve_eax_ecx_edx UiCommandModeG_ApplyMaskedColorVariant(void *visualState);

/* 0x0056DA50 */
void __thandor_void_preserve_eax_ecx_edx UiCommandModeG_SetNodeFlag02000000(WorldRuntimeContext *context);

/* 0x00571440 */
void __thandor_void_preserve_eax_ecx_edx
UiCommandMatrix_SelectIndex(UiCommandModeIndex absoluteIndex,UiNodeBase *root);

/* 0x0055F440 */
void __thandor_void_preserve_eax_ecx_edx
UiCommandRuntimeFlags_ApplyClearSetToggleMasks
          (PlayerRuntimeId playerRuntimeId,UiCommandRuntimeFlagMask toggleMask,
          UiCommandRuntimeFlagMask setMask,UiCommandRuntimeFlagMask clearMask);

/* 0x0056D860 */
void __thandor_void_preserve_eax_ecx_edx UiCommandModeG_ClearNodeFlag00100000(WorldRuntimeContext *context);

/* 0x0056D880 */
void __thandor_void_preserve_eax_ecx_edx UiCommandModeG_SetNodeFlag00200000(WorldRuntimeContext *context);

/* 0x0056D940 */
void __thandor_void_preserve_eax_ecx_edx UiCommandModeG_SetNodeFlag01000000(WorldRuntimeContext *context);

/* 0x0056D8C0 */
void __thandor_void_preserve_eax_ecx_edx UiCommandModeG_SetNodeFlag00000400(WorldRuntimeContext *context);

/* 0x0056D8E0 */
void __thandor_void_preserve_eax_ecx_edx UiCommandModeG_ClearNodeFlags00000480(WorldRuntimeContext *context);

/* 0x0056D840 */
void __thandor_void_preserve_eax_ecx_edx UiCommandModeG_SetNodeFlag00100000(WorldRuntimeContext *context);

/* 0x0056D8A0 */
void __thandor_void_preserve_eax_ecx_edx UiCommandModeG_ClearNodeFlag00200000(WorldRuntimeContext *context);

/* 0x0056D960 */
void __thandor_void_preserve_eax_ecx_edx UiCommandModeG_ClearNodeFlag01000000(WorldRuntimeContext *context);

/* 0x0056D9F0 */
void __thandor_void_preserve_eax_ecx_edx UiCommandModeG_ApplyRawColorVariant(void *visualState);

/* 0x0056DA70 */
void __thandor_void_preserve_eax_ecx_edx UiCommandModeG_ClearNodeFlag02000000(WorldRuntimeContext *context);

/* 0x0056D900 */
void __thandor_void_preserve_eax_ecx_edx UiCommandModeG_SetNodeFlag00800000(WorldRuntimeContext *context);

/* 0x0056DA90 */
InGameRuntimeRootImageC3E4 * __thandor_eax_edx_cf_preserve_ecx
UiCommandModeG_SelectAndSyncPages(UiCommandModeIndex modeIndex,UiSelectableControl *source);

#endif /* THANDOR_UI_INGAME_COMMANDS_H */

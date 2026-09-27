/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/include/thandor/ui/frontend/runtime.h
 * Reverse engineering by idkFoxes 2026
 */

#ifndef THANDOR_UI_FRONTEND_RUNTIME_H
#define THANDOR_UI_FRONTEND_RUNTIME_H

#include <thandor/generated/types.h>
#include <thandor/core/contracts.h>

/* Submodule: ui/frontend/runtime. */
/* Functions are grouped by semantic ownership; address comments are executable virtual addresses. */

/* Frontend menu state machine (Frontend_MainLoop).
   g_FrontendPendingPageAction holds the next step of the menu. A ROM action-table record writes it when the
   player activates a menu entry (FrontendRomActionTable_ExecuteRecord; 3, 4 and 9 are refused in a network
   session, 2 needs a network backend), Frontend_MainLoop writes it after re-initialising the frontend. Every
   frame Frontend_MainLoop performs the pending action and resets it to FRONTEND_PAGE_ACTION_NONE; the
   "wait" actions stay pending until every player block has reached the matching FRONTEND_PLAYER_STATE_* bit. */
#define FRONTEND_PAGE_ACTION_NONE 0
#define FRONTEND_PAGE_ACTION_START_SESSION 1 /* leave the frontend, run a new session of the loaded level */
#define FRONTEND_PAGE_ACTION_NETWORK_SETUP_PAGE 2
#define FRONTEND_PAGE_ACTION_GAMEPLAY_SETTINGS_PAGE 3
#define FRONTEND_PAGE_ACTION_QUIT_CONFIRM_PAGE 4 /* quit confirmation, page-stack page 9 (FrontendSession_ShowQuitConfirmPage) */
#define FRONTEND_PAGE_ACTION_SCENARIO_SELECTION_PAGE 5 /* waits for the scenario catalogue exchange */
#define FRONTEND_PAGE_ACTION_RESUME_SAVED_SESSION 6 /* leave the frontend, run the loaded save (load flag 1) */
#define FRONTEND_PAGE_ACTION_TASK_ASSIGNMENT_PAGE 7 /* waits until every player is ready for it */
#define FRONTEND_PAGE_ACTION_MISSION_BRIEFING_PAGE 8 /* waits until every player has the level */
#define FRONTEND_PAGE_ACTION_CREDITS 9
/* Any other value tears the frontend down and rebuilds it at the entry record. */

/* Record ids in engine\zentrale.rom that Frontend_Init activates (the camera/room the menu starts in).
   1 is the entry record ProcessEntry passes to Frontend_MainLoop. */
#define FRONTEND_ROM_RECORD_MAIN_MENU 1
#define FRONTEND_ROM_RECORD_MISSION_BRIEFING 10 /* followed by FRONTEND_PAGE_ACTION_MISSION_BRIEFING_PAGE */
#define FRONTEND_ROM_RECORD_SCENARIO_SELECTION 12 /* followed by FRONTEND_PAGE_ACTION_SCENARIO_SELECTION_PAGE */

/* Bits of FrontendPlayerRuntimeRecord.factionAssignment.roleStateFlags: per-player progress through the
   network menu handshake, set locally or from the peer's packets (ui/frontend/player, assets/scenario/catalog). */
#define FRONTEND_PLAYER_STATE_SCENARIO_CATALOG 0x01 /* scenario catalogue exchanged */
#define FRONTEND_PLAYER_STATE_TASK_ASSIGNMENT 0x02 /* ready for the task-assignment page */
#define FRONTEND_PLAYER_STATE_LEVEL_LOADED 0x04 /* level package loaded locally */
#define FRONTEND_PLAYER_STATE_LEVEL_RECEIVED 0x08 /* level package received from / confirmed to the host */
#define FRONTEND_PLAYER_STATE_LEVEL_READY_MASK 0x0C
/* set by FrontendScenarioSession_LoadOrRequestLevelAsset for players whose catalog level mask has the selected
   level, i.e. who can load it from their own disk instead of receiving it */
#define FRONTEND_PLAYER_STATE_HAS_LEVEL_LOCALLY 0x10

/* Pages of FrontendUiImage.frontendPageStack (see generated/ui_templates.h). */
#define FRONTEND_PAGE_MAIN 0 /* no dialog page: only the menu room */
#define FRONTEND_PAGE_NETWORK_GAME 1 /* protocol, player name, host address, session list */
#define FRONTEND_PAGE_HOST_GAME_SETUP 2
#define FRONTEND_PAGE_HOST_LOBBY 3
#define FRONTEND_PAGE_OPTIONS 5
#define FRONTEND_PAGE_QUIT_CONFIRM 9
#define FRONTEND_PAGE_FACTION_SETUP 11 /* FrontendTaskAssignmentPage_Initialize */
#define FRONTEND_PAGE_MISSION_BRIEFING 12
/* Up to 640 pixels wide the dialog pages cover the menu room, so opening one also stops the room's 3D
   rendering: FrontendModelPointerContext_RenderWorldViewQueuesClipped returns at once while
   FRONTEND_MENU_ROOM_RENDER_SUPPRESSED is set in menuRoomModelView's contextFlags (+0x4C). */
#define FRONTEND_COMPACT_LAYOUT_MAX_WIDTH 640
#define FRONTEND_MENU_ROOM_RENDER_SUPPRESSED 0x2000
/* UiSelectableControl.stateFlags bit set together with UI_NODE_SUPPRESSED to switch a frontend button off:
   sprite buttons skip it in the hit test and draw it only while selected (ui/controls/buttons). */
#define FRONTEND_CONTROL_INACTIVE 0x400
/* Action ids of frontend controls (UiSelectableControl.actionId; handler g_UiActionPage20InitializedHandlers
   [id - 0x2000]). UiNodeList_SuppressActionId/UnsuppressActionId hide and show the controls carrying one. */
#define FRONTEND_ACTION_HOST_GAME 0x2001 /* networkGameHostButton */
#define FRONTEND_ACTION_JOIN_GAME 0x2002 /* networkGameJoinButton */
#define FRONTEND_ACTION_CREATE_HOSTED_GAME 0x2004 /* hostGameCreateButton (FrontendNetworkSetupPage_InitializeSingleLocalPlayer) */
#define FRONTEND_ACTION_KICK_PLAYER 0x200B /* hostLobbyKickPlayerButton */
#define FRONTEND_ACTION_LINK_ROTATION_ZOOM 0x203E /* linkRotationZoomCheckbox */
#define FRONTEND_ACTION_LINK_ROTATION_TILT 0x203F /* linkRotationTiltCheckbox */
#define FRONTEND_ACTION_GAME_SPEED 0x204A /* gameSpeedSlider on the mission briefing page */
#define FRONTEND_ACTION_START_NETWORK_GAME 0x2006 /* hostLobbyStartButton: seeds the random streams and starts */
/* g_FrontendNetworkState, dispatched by Frontend_StateTick (values 3..5 are set by network/protocol/transfer). */
#define FRONTEND_NETWORK_STATE_IDLE 0
#define FRONTEND_NETWORK_STATE_BROWSING 1 /* network game page: polls for sessions, handles join acks */
#define FRONTEND_NETWORK_STATE_HOSTING 2 /* host lobby: publishes the session, handles joining players */
#define FRONTEND_NETWORK_STATE_JOINED 3 /* client in the host lobby after the join ack
                                           (FrontendTransfer_HandleSessionListAndJoinAckPackets) */
#define FRONTEND_NETWORK_STATE_HOST_STARTING 4 /* host sends commands and player snapshots to the clients
                                                  (FrontendTransfer_PublishHostSessionAndDispatchQueuedCommands) */
#define FRONTEND_NETWORK_STATE_CLIENT_STARTING 5 /* client receives the session start
                                                    (FrontendTransfer_HandleHostSessionAndCommandBatchPackets) */
/* Reload value of g_FrontendTimerCountdownTicks: the 80 Hz FrontendRuntime_TimerCountdownTick counts it down,
   so Frontend_StateTick runs its network work at most 20 times per second. */
#define FRONTEND_TIMER_TICKS_PER_NETWORK_TICK 4
/* Text resource ids of the faction setup and mission briefing pages. */
#define TEXT_ID_FACTION_NAME_BASE 0x2173
#define TEXT_ID_FACTION_SETUP_TASK_TEMPLATE 0x218C /* "Task description (%s):", combined with the level title */
#define TEXT_ID_FACTION_MODE_PLAYER 0x2198 /* mode button captions */
#define TEXT_ID_FACTION_MODE_NOBODY 0x2199
#define TEXT_ID_FACTION_MODE_COMPUTER 0x219A
#define TEXT_ID_MISSION_BRIEFING_TEMPLATE 0x219B /* combined with the level title */
/* Host game setup page: networkSpeedLabel's caption id; the text for network speed n (1..7) is this + n. */
#define TEXT_ID_NETWORK_SPEED_BASE 0x210D

/* g_FrontendRuntimeFlags bit set by Frontend_Init; cleared once every player has reported ready
   (FrontendPlayerRuntime_RecordReadyAndUpdateWaitState), which ends Frontend_Init's wait loop. */
#define FRONTEND_RUNTIME_FLAG_WAITING_FOR_PLAYERS 0x10

/* 0x00546BD0 */
FrontendMainLoopResult __thandor_eax_cf_preserve_ecx_edx
Frontend_MainLoop(RomRecordId frontendEntryRecordId);

/* 0x0050C380 */
GraphicsCursorFrameIndex FrontendModelPointerContext_SelectBestModelHitTargetAndResolveAction (int pointerY,int pointerX,FrontendModelPointerContextRuntimeState118 *context);

/* 0x0050C5A0 */
void __thandor_preserve_eax_edx
FrontendModelPointerContext_NonRightPress
          (UiPointerWheelDelta wheelDelta,UiPixelCoordinate pointerY,UiPixelCoordinate pointerX,
          FrontendModelPointerContextRuntimeState17C *callbackContext);

/* 0x0050C610 */
void __thandor_preserve_eax_edx
FrontendModelPointerContext_NonRightRelease
          (UiPointerWheelDelta wheelDelta,UiPixelCoordinate pointerY,UiPixelCoordinate pointerX,
          FrontendModelPointerContextRuntimeState118 *callbackContext);

/* 0x0050C670 */
void __thandor_preserve_eax_edx
FrontendModelPointerContext_NonRightDrag
          (UiPointerWheelDelta wheelDelta,UiPixelCoordinate pointerY,UiPixelCoordinate pointerX,
          FrontendModelPointerContextRuntimeState17C *callbackContext);

/* 0x00549B40 */
void __thandor_void_preserve_eax_ecx FrontendUiAction2044_Handler(UiNodeBase *factionControl);

/* 0x00549BC0 */
void __thandor_void_preserve_eax_ecx FrontendUiAction2045_Handler(UiNodeBase *playerControl);

/* 0x00549C40 */
void __thandor_void_preserve_eax_ecx FrontendUiAction2046_Handler(UiNodeBase *selectionRowControl);

/* 0x0050BB80 */
void FrontendModelPointerContext_Relocate
               (UiSerializedRelocationDelta relocationDelta,
               FrontendModelPointerContextRuntimeState17C *control);

/* 0x0050BC30 */
void __thandor_void_preserve_eax_ecx_edx
FrontendModelPointerContext_Layout(WorldRuntimeContext *callbackContext);

/* 0x0050BC60 */
void __thandor_void_preserve_eax_ecx_edx
FrontendModelPointerContext_RenderWorldViewQueuesClipped
          (UiPixelCoordinate clipTop,UiPixelCoordinate clipLeft,UiPixelCoordinate clipBottom,
          UiPixelCoordinate clipRight,FrontendModelPointerContextRuntimeState17C *control);

/* 0x0050C6E0 */
void __thandor_void_preserve_eax_ecx_edx
FrontendModelPointerContext_RightPress
          (UiPointerWheelDelta wheelDelta,UiPixelCoordinate pointerY,UiPixelCoordinate pointerX,
          FrontendModelPointerContextRuntimeState17C *callbackContext);

/* 0x0050C730 */
void __thandor_preserve_eax_edx
FrontendModelPointerContext_RightRelease
          (UiPointerWheelDelta wheelDelta,UiPixelCoordinate pointerY,UiPixelCoordinate pointerX,
          FrontendModelPointerContextRuntimeState17C *callbackContext);

/* 0x0050CC80 */
void __thandor_void_preserve_eax_ecx_edx
FrontendModelPointerContext_DispatchWorldCameraPointerInput
          (UiPointerWheelDelta wheelDelta,UiPixelCoordinate pointerY,UiPixelCoordinate pointerX,
          WorldRuntimeContext *callbackContext);

/* 0x0050CED0 */
void __thandor_void_preserve_eax_ecx_edx
FrontendModelPointerContext_PointerWheel
          (UiPointerWheelDelta wheelDelta,UiPixelCoordinate pointerY,UiPixelCoordinate pointerX,
          WorldRuntimeContext *callbackContext);

/* 0x0050CF50 */
bool __thandor_cf_preserve_eax_ecx_edx
FrontendModelPointerContext_KeyboardEvent
          (UiKeyboardStateMask keyboardStateMask,UiKeyboardEventCode keyCode,
          FrontendModelPointerContextRuntimeState118 *control);

/* 0x0050CF90 */
void __thandor_void_preserve_eax_ecx_edx
FrontendModelPointerContext_Tick(WorldRuntimeContext *callbackContext);

/* 0x00514E40 */
void __thandor_void_preserve_eax_ecx_edx FrontendRuntime_UpdateCurrentFactionMetricCache(void);

/* 0x00547620 */
void __cdecl FrontendRuntime_TimerCountdownTick(void);

/* 0x00547FB0 */
void __cdecl FrontendRomTransition_AdvanceElapsedTicks(void);

/* 0x00548030 */
bool __thandor_cf_preserve_eax_ecx_edx
FrontendRuntime_DispatchCommandByCodeAndModifierFlags
          (UiKeyboardStateMask modifierFlags,UiActionId commandCode,void *frontendRuntime);

/* 0x00548700 */
void __thandor_void_preserve_eax_ecx_edx FrontendState_DispatchCode(FrontendStatusCode romRecordIndex);

/* 0x00548910 */
uint32_t __thandor_eax_preserve_ecx_edx
FrontendRuntime_UpdatePointerContextAndSceneView
          (uint32_t callbackArgument1,uint32_t callbackArgument2,uint32_t callbackArgument3,uint32_t hitMetric,
          void *pointedModelNode,FrontendPointerSceneRuntimeView43E8 *frontendRuntime);

/* 0x00548BE0 */
void FrontendRuntimeCallback5C_NoOp
               (uint32_t callbackArgument1,uint32_t callbackArgument2,uint32_t callbackArgument3,
               uint32_t hitMetric,uint32_t pointedModelNode,uint32_t pointerContext);

/* 0x00548BF0 */
void FrontendRuntimeCallback60_NoOp
               (uint32_t callbackArgument1,uint32_t callbackArgument2,uint32_t callbackArgument3,
               uint32_t hitMetric,uint32_t pointedModelNode,uint32_t pointerContext);

/* 0x00548C00 */
void __thandor_void_preserve_eax_ecx_edx
FrontendRuntimeCallback64_DispatchRecord1350
          (uint32_t callbackArgument1,uint32_t callbackArgument2,uint32_t callbackArgument3,uint32_t hitMetric,
          FrontendCallbackArgument5 pointedModelNode,uint32_t pointerContext);

/* 0x00548C70 */
void FrontendRuntimeCallback68_DispatchRefresh1340(uint32_t pointerContext);

/* 0x00548CB0 */
void __thandor_void_preserve_eax_ecx_edx FrontendRecentTextHistory_InsertAndRebuild5(uint16_t *text);

/* 0x00549100 */
void FrontendCallback_ApplyGameSpeedOrDispatch02C0(uint32_t callbackArgument);

/* 0x00549140 */
void FrontendCallback_ReleaseSelectedResourceOrDispatch0320(uint32_t callbackArgument);

/* 0x00549180 */
void FrontendCallback_NoOpArg1(void *source);

/* 0x00549AB0 */
void __thandor_preserve_eax FrontendCallback_ReturnToMainPageOrDispatch0DC0(uint32_t callbackArgument);

/* 0x0054A5A0 */
void FrontendCallback_ReturnToMainPageOrDispatchState4(uint32_t callbackArgument);

/* 0x0054A7D0 */
void __thandor_preserve_eax
FrontendCallback_ReturnToMainPageOrDispatch0DC0_Secondary(uint32_t callbackArgument);

/* 0x0054AAD0 */
void __thandor_preserve_eax_edx FrontendUiAction2010_Handler(UiNodeBase *sourceNode);

/* 0x0054AB70 */
void __thandor_void_preserve_eax_ecx_edx
FrontendUiAction2011_Handler(FrontendDisplaySettingsPageOptionState1010 *source);

/* 0x0054BA30 */
void __thandor_void_preserve_eax_ecx
FrontendUiAction202CTo2030_SharedHandler(UiNodeBase *sourceNode);

/* 0x0054D3F0 */
void __thandor_void_preserve_eax_ecx_edx
FrontendUiAction200C_Handler(UiPointerListControl *sessionListControl);

/* 0x0054D460 */
void __thandor_void_preserve_eax_ecx FrontendRecentText_TrimAndSortTopFive(UiNodeBase *source);

/* 0x0054D4A0 */
void __thandor_void_preserve_eax_ecx_edx
FrontendUiAction200F_Handler(FrontendNetworkSetupPageBackendListPtr backendList);

/* 0x00565A30 */
void __thandor_void_preserve_eax_ecx_edx Frontend_PlaySelectedEndMovie(void);

/* 0x00546700 */
FrontendInitResult __thandor_eax_cf_preserve_ecx_edx Frontend_Init(RomRecordId initialRomRecordId);

/* 0x00547630 */
void __thandor_void_preserve_eax_ecx_edx Frontend_StateTick(void);

/* 0x00543B70 */
void __thandor_void_preserve_ecx_edx
FrontendMenu_BindSharedResources(FrontendRootResourceSlots5954 *frontendUiState);

/* 0x005445A0 */
void __thandor_void_preserve_eax_ecx_edx
FrontendUiAction2044_IndexedSelectionHelper
          (FrontendIndexedSelectionArgument argument1,uint32_t argument2,uint32_t argument3,
          FrontendFactionAssignmentIndex selectionIndex);

/* 0x00544640 */
void __thandor_void_preserve_eax_ecx_edx
FrontendUiAction2045_IndexedSelectionHelper
          (uint32_t argument1,uint32_t argument2,uint32_t argument3,
          FrontendFactionAssignmentIndex selectionIndex);

/* 0x005446A0 */
void __thandor_void_preserve_eax_ecx_edx
FrontendUiAction2046_IndexedSelectionHelper
          (FrontendIndexedSelectionArgument argument1,uint32_t argument2,uint32_t argument3,
          FrontendFactionAssignmentIndex selectionIndex);

/* 0x00546190 */
void __thandor_void_preserve_eax_ecx_edx
FrontendDebugOverlay_RefreshCountersAndWorldCoordinates(void);

/* 0x005474E0 */
void __thandor_void_preserve_eax_ecx_edx FrontendRuntime_ShutdownAndReleaseResourcesRegs(void);

/* 0x0050AD90 */
uint64_t FrontendModelPointerContext_FindBestEligibleModelHitTarget (int pointerY,int pointerX,FrontendModelPointerContextRuntimeState118 *context);

#endif /* THANDOR_UI_FRONTEND_RUNTIME_H */

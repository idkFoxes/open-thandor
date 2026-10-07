/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/include/thandor/ui/frontend/common.h
 * Reverse engineering by idkFoxes 2026
 */

#ifndef THANDOR_UI_FRONTEND_COMMON_H
#define THANDOR_UI_FRONTEND_COMMON_H

#include <thandor/core/contracts.h>

/* Frontend pages, page actions, UI actions, player and network states and text ids shared by the frontend files. */

/* Frontend menu state machine (Frontend_MainLoop).
   g_FrontendPendingPageAction holds the next step of the menu. A ROM action-table record writes it when the
   player activates a menu entry (FrontendRomActionTable_ExecuteRecord; 3, 4 and 9 are refused in a network
   session, 2 needs a network backend), Frontend_MainLoop writes it after re-initialising the frontend. Every
   frame Frontend_MainLoop performs the pending action and resets it to FRONTEND_PAGE_ACTION_NONE; the
   "wait" actions stay pending until every player block has reached the matching FRONTEND_PLAYER_STATE_* bit. */
inline constexpr int32_t FRONTEND_PAGE_ACTION_NONE = 0;
inline constexpr int32_t FRONTEND_PAGE_ACTION_START_SESSION = 1; /* leave the frontend, run a new session of the loaded level */
inline constexpr int32_t FRONTEND_PAGE_ACTION_NETWORK_SETUP_PAGE = 2;
inline constexpr int32_t FRONTEND_PAGE_ACTION_GAMEPLAY_SETTINGS_PAGE = 3;
inline constexpr int32_t FRONTEND_PAGE_ACTION_QUIT_CONFIRM_PAGE = 4; /* quit confirmation, page-stack page 9 (FrontendSession_ShowQuitConfirmPage) */
inline constexpr int32_t FRONTEND_PAGE_ACTION_SCENARIO_SELECTION_PAGE = 5; /* waits for the scenario catalogue exchange */
inline constexpr int32_t FRONTEND_PAGE_ACTION_RESUME_SAVED_SESSION = 6; /* leave the frontend, run the loaded save (load flag 1) */
inline constexpr int32_t FRONTEND_PAGE_ACTION_TASK_ASSIGNMENT_PAGE = 7; /* waits until every player is ready for it */
inline constexpr int32_t FRONTEND_PAGE_ACTION_MISSION_BRIEFING_PAGE = 8; /* waits until every player has the level */
inline constexpr int32_t FRONTEND_PAGE_ACTION_CREDITS = 9;
/* Any other value tears the frontend down and rebuilds it at the entry record. */

/* Record ids in engine\zentrale.rom that Frontend_Init activates (the camera/room the menu starts in).
   1 is the entry record ProcessEntry passes to Frontend_MainLoop. */
inline constexpr int32_t FRONTEND_ROM_RECORD_MAIN_MENU = 1;
inline constexpr int32_t FRONTEND_ROM_RECORD_MISSION_BRIEFING = 10; /* followed by FRONTEND_PAGE_ACTION_MISSION_BRIEFING_PAGE */
inline constexpr int32_t FRONTEND_ROM_RECORD_SCENARIO_SELECTION = 12; /* followed by FRONTEND_PAGE_ACTION_SCENARIO_SELECTION_PAGE */

/* Pages of FrontendUiImage.frontendPageStack (see ui/frontend/types.h). */
inline constexpr int32_t FRONTEND_PAGE_MAIN = 0; /* no dialog page: only the menu room */
inline constexpr int32_t FRONTEND_PAGE_NETWORK_GAME = 1; /* protocol, player name, host address, session list */
inline constexpr int32_t FRONTEND_PAGE_HOST_GAME_SETUP = 2;
inline constexpr int32_t FRONTEND_PAGE_HOST_LOBBY = 3;
inline constexpr int32_t FRONTEND_PAGE_CLIENT_LOBBY = 4; /* clientLobbyPlayerList, after the join ack */
inline constexpr int32_t FRONTEND_PAGE_OPTIONS = 5;
inline constexpr int32_t FRONTEND_PAGE_DISPLAY_SETTINGS = 6; /* adapter, resolution, colour depth (graphicsSettingsButton, 0x2011); in
                                            open-thandor displayPageStack: also the advanced settings page */
inline constexpr int32_t FRONTEND_PAGE_GRAPHICS_SETTINGS = 7; /* "3D": shading, polygon detail, texture quality (0x2012) */
inline constexpr int32_t FRONTEND_PAGE_AUDIO_SETTINGS = 8; /* sound toggles and volume sliders (0x2013) */
inline constexpr int32_t FRONTEND_PAGE_QUIT_CONFIRM = 9;
inline constexpr int32_t FRONTEND_PAGE_FACTION_SETUP = 11; /* FrontendTaskAssignmentPage_Initialize */
inline constexpr int32_t FRONTEND_PAGE_MISSION_BRIEFING = 12;
/* Up to 640 pixels wide the dialog pages cover the menu room, so opening one also stops the room's 3D
   rendering: FrontendModelPointerContext_RenderWorldViewQueuesClipped returns at once while
   FRONTEND_MENU_ROOM_RENDER_SUPPRESSED is set in menuRoomModelView's contextFlags. */
inline constexpr int32_t FRONTEND_COMPACT_LAYOUT_MAX_WIDTH = 640;
inline constexpr int32_t FRONTEND_MENU_ROOM_RENDER_SUPPRESSED = 0x2000;
/* UiSelectableControl.stateFlags bit set together with UI_NODE_SUPPRESSED to switch a frontend button off:
   sprite buttons skip it in the hit test and draw it only while selected (ui/controls/buttons). */
inline constexpr int32_t FRONTEND_CONTROL_INACTIVE = 0x400;
/* Action ids of frontend controls (UiSelectableControl.actionId; handler g_UiActionPage20InitializedHandlers
   [id - 0x2000]). UiNodeList_SuppressActionId/UnsuppressActionId hide and show the controls carrying one. */
inline constexpr int32_t FRONTEND_ACTION_HOST_GAME = 0x2001; /* networkGameHostButton */
inline constexpr int32_t FRONTEND_ACTION_JOIN_GAME = 0x2002; /* networkGameJoinButton */
inline constexpr int32_t FRONTEND_ACTION_CREATE_HOSTED_GAME = 0x2004; /* hostGameCreateButton (FrontendNetworkSetupPage_InitializeSingleLocalPlayer) */
inline constexpr int32_t FRONTEND_ACTION_KICK_PLAYER = 0x200B; /* hostLobbyKickPlayerButton */
inline constexpr int32_t FRONTEND_ACTION_LINK_ROTATION_ZOOM = 0x203E; /* linkRotationZoomCheckbox */
inline constexpr int32_t FRONTEND_ACTION_LINK_ROTATION_TILT = 0x203F; /* linkRotationTiltCheckbox */
inline constexpr int32_t FRONTEND_ACTION_GAME_SPEED = 0x204A; /* gameSpeedSlider on the mission briefing page */
inline constexpr int32_t FRONTEND_ACTION_START_NETWORK_GAME = 0x2006; /* hostLobbyStartButton: seeds the random streams and starts */
inline constexpr int32_t FRONTEND_ACTION_START_SELECTED_GAME = 0x2038; /* gameSelectStartButton (FrontendScenarioSelection_ActivateSelectedRecord) */
inline constexpr int32_t FRONTEND_ACTION_SELECT_SINGLE_GAME = 0x203A; /* missionsList */
inline constexpr int32_t FRONTEND_ACTION_SELECT_CAMPAIGN = 0x203B; /* campaignsList */
/* Settings pages (ui/frontend/settings) */
inline constexpr int32_t FRONTEND_ACTION_SHADING_LEVEL = 0x2015; /* the six shadingLevelGrid*Depth* choices */
inline constexpr int32_t FRONTEND_ACTION_REVERSE_STEREO = 0x201A; /* reverseStereoCheckbox */
inline constexpr int32_t FRONTEND_ACTION_EFFECTS_GAIN = 0x201B; /* effectsVolumeSlider */
inline constexpr int32_t FRONTEND_ACTION_MOVIE_GAIN = 0x201C; /* movieVolumeSlider */
inline constexpr int32_t FRONTEND_ACTION_MUSIC_GAIN = 0x201D; /* musicVolumeSlider */
inline constexpr int32_t FRONTEND_ACTION_MOVIE_EVENT_GAIN = 0x204E; /* movieEventVolumeSlider */
inline constexpr int32_t FRONTEND_ACTION_RESOLUTION_OPTION1 = 0x2022; /* displayResolutionOption1..10: 0x2022..0x202B (the extra
                                                  rows of open-thandor take 0x2022 as well) */
/* Not in the original: the display mode kind choices (handlers58_5A of g_FrontendUiActionHandlersPage20) */
inline constexpr int32_t FRONTEND_ACTION_DISPLAY_MODE_KIND_WINDOW = 0x2058;
inline constexpr int32_t FRONTEND_ACTION_DISPLAY_MODE_KIND_BORDERLESS = 0x2059;
inline constexpr int32_t FRONTEND_ACTION_DISPLAY_MODE_KIND_FULLSCREEN = 0x205A;
/* Not in the original: the advanced settings page (handlers5B_5F of g_FrontendUiActionHandlersPage20) */
inline constexpr int32_t FRONTEND_ACTION_ADVANCED_EDGES = 0x205B; /* advancedEdgesSmooth / advancedEdgesExact */
inline constexpr int32_t FRONTEND_ACTION_ADVANCED_UI_SCALE = 0x205C; /* advancedUiScaleAuto, advancedUiScale1..3 */
inline constexpr int32_t FRONTEND_ACTION_ADVANCED_FRAME_LIMIT = 0x205D; /* advancedFrameLimitOff, 60, 120, 144 */
inline constexpr int32_t FRONTEND_ACTION_ADVANCED_VSYNC = 0x205E; /* advancedVsyncCheckbox */
inline constexpr int32_t FRONTEND_ACTION_OPEN_ADVANCED_SETTINGS = 0x205F; /* advancedSettingsButton of the options page */
/* the pages of displayPageStack (frontendPageStack page FRONTEND_PAGE_DISPLAY_SETTINGS) */
inline constexpr int32_t FRONTEND_DISPLAY_SUBPAGE_DISPLAY = 0; /* displaySettingsPage ("Anzeige") */
inline constexpr int32_t FRONTEND_DISPLAY_SUBPAGE_ADVANCED = 1; /* advancedSettingsPage ("Erweitert") */
inline constexpr int32_t FRONTEND_ACTION_ADAPTER_OPTION1 = 0x202C; /* displayAdapterOption1..5: 0x202C..0x2030 */
/* byte offset of an adapter choice (displayAdapterOption1..5) from its parent displayAdapterGroup, as
   FrontendDisplaySettingsAction_SelectAdapter identifies the pressed button */
#define FRONTEND_ADAPTER_OPTION_OFFSET_IN_GROUP(option) \
          ((int)(offsetof(FrontendUiImage,option) - offsetof(FrontendUiImage,displayAdapterGroup)))
inline constexpr int32_t FRONTEND_ACTION_APPLY_DISPLAY_MODE = 0x2031; /* FrontendDisplaySettings_ApplyMode */
/* g_FrontendNetworkState, dispatched by Frontend_StateTick (values 3..5 are set by network/protocol/transfer). */
inline constexpr int32_t FRONTEND_NETWORK_STATE_IDLE = 0;
inline constexpr int32_t FRONTEND_NETWORK_STATE_BROWSING = 1; /* network game page: polls for sessions, handles join acks */
inline constexpr int32_t FRONTEND_NETWORK_STATE_HOSTING = 2; /* host lobby: publishes the session, handles joining players */
inline constexpr int32_t FRONTEND_NETWORK_STATE_JOINED = 3; /* client in the host lobby after the join ack
                                           (FrontendTransfer_HandleSessionListAndJoinAckPackets) */
inline constexpr int32_t FRONTEND_NETWORK_STATE_HOST_STARTING = 4; /* host sends commands and player snapshots to the clients
                                                  (FrontendTransfer_PublishHostSessionAndDispatchQueuedCommands) */
inline constexpr int32_t FRONTEND_NETWORK_STATE_CLIENT_STARTING = 5; /* client receives the session start
                                                    (FrontendTransfer_HandleHostSessionAndCommandBatchPackets) */
/* Reload value of g_FrontendTimerCountdownTicks: the 80 Hz FrontendRuntime_TimerCountdownTick counts it down,
   so Frontend_StateTick runs its network work at most 20 times per second. */
inline constexpr int32_t FRONTEND_TIMER_TICKS_PER_NETWORK_TICK = 4;
/* Text resource ids of the faction setup and mission briefing pages. */
inline constexpr int32_t TEXT_ID_FACTION_NAME_BASE = 0x2173;
inline constexpr int32_t TEXT_ID_FACTION_SETUP_TASK_TEMPLATE = 0x218C; /* "Task description (%s):", combined with the level title */
inline constexpr int32_t TEXT_ID_FACTION_MODE_PLAYER = 0x2198; /* mode button captions */
inline constexpr int32_t TEXT_ID_FACTION_MODE_NOBODY = 0x2199;
inline constexpr int32_t TEXT_ID_FACTION_MODE_COMPUTER = 0x219A;
inline constexpr int32_t TEXT_ID_MISSION_BRIEFING_TEMPLATE = 0x219B; /* combined with the level title */
/* Host game setup page: networkSpeedLabel's caption id; the text for network speed n (1..7) is this + n. */
inline constexpr int32_t TEXT_ID_NETWORK_SPEED_BASE = 0x210D;
/* Display settings page: device name shown for every adapter (the software rasterizer);
   FrontendDisplaySettingsAction_OpenPageAndListModes. */
inline constexpr int32_t TEXT_ID_DISPLAY_SOFTWARE_DEVICE_NAME = 0x212D;
/* Menu room hover hints: hint n of a ROM action record is text id this + n (1 while a page action runs). */
inline constexpr int32_t TEXT_ID_MENU_HINT_BASE = 0x2000;
/* Network game page: the local address line, selector 0 = g_FrontendNetworkEndpointTextUtf16 (Frontend_Init). */
inline constexpr int32_t TEXT_ID_NETWORK_ADDRESS_TEMPLATE = 0x2104;
/* Chat-history notices when the player leaves a network session with Alt+Q (selector 0 = name of player block 0,
   the host): as a client, and as the host. */
inline constexpr int32_t TEXT_ID_NETWORK_SESSION_LEFT = 0xFF02;
inline constexpr int32_t TEXT_ID_NETWORK_SESSION_CLOSED = 0xFF04;

/* g_FrontendRuntimeFlags bit set by Frontend_Init; cleared once every player has reported ready
   (FrontendPlayerRuntime_RecordReadyAndUpdateWaitState), which ends Frontend_Init's wait loop. */
inline constexpr int32_t FRONTEND_RUNTIME_FLAG_WAITING_FOR_PLAYERS = 0x10;

/* Frontend_Init: rate of FrontendRuntime_TimerCountdownTick and of FrontendRomTransition_AdvanceElapsedTicks */
inline constexpr int32_t FRONTEND_PERIODIC_TIMER_HZ = 80;
inline constexpr int32_t FRONTEND_ROM_TRANSITION_TIMER_HZ = 256;
/* World object records of the 3D menu room (g_FrontendWorldObjectRecords, WorldRuntime_AttachObjectArray) */
inline constexpr int32_t FRONTEND_WORLD_OBJECT_RECORD_COUNT = 256;
/* Positions of the two number digits in "sound\menue01.sam" (menu sounds 01..99) */
inline constexpr int32_t FRONTEND_MENU_SOUND_PATH_TENS_DIGIT = 11;
inline constexpr int32_t FRONTEND_MENU_SOUND_PATH_ONES_DIGIT = 12;
/* Record id looked up when the pointer is over no ROM record (matches none), and the transition target id
   "no record to activate when the flight ends" */
inline constexpr uint32_t FRONTEND_ROM_RECORD_ID_NONE = 0xf0000000u;
inline constexpr uint32_t FRONTEND_ROM_TRANSITION_NO_TARGET = 0xffffffffu;
/* Low four bits of WorldRuntimeContext.runtimeFlags: the camera motion a right drag or the wheel is doing
   (FrontendModelPointerContext_DispatchWorldCameraPointerInput / _PointerWheel) */
inline constexpr int32_t FRONTEND_CAMERA_MOTION_MOVE = 1;
inline constexpr int32_t FRONTEND_CAMERA_MOTION_HEADING = 2;
inline constexpr int32_t FRONTEND_CAMERA_MOTION_DISTANCE = 4;
inline constexpr int32_t FRONTEND_CAMERA_MOTION_PITCH = 8;
inline constexpr int32_t FRONTEND_CAMERA_MOTION_MASK = 15;

#endif /* THANDOR_UI_FRONTEND_COMMON_H */

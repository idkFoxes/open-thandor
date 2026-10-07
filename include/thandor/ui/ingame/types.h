/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/include/thandor/ui/ingame/types.h
 * Reverse engineering by idkFoxes 2026
 */

#ifndef THANDOR_UI_INGAME_TYPES_H
#define THANDOR_UI_INGAME_TYPES_H

#include <stddef.h> /* offsetof */
#include <stdint.h>
#include <type_traits>
#include <utility>
#include <thandor/core/flags.h> /* THANDOR_FLAG_ENUM: UiCommandActivationStateFlags */
#include <thandor/core/ptr32.h> /* Ptr32: the pointer fields of these 32-bit layouts */
#include <thandor/assets/army/types.h>
#include <thandor/core/types.h>
#include <thandor/gameplay/army/types.h>
#include <thandor/graphics/resources/types.h>
#include <thandor/ui/controls/types.h>
#include <thandor/ui/dialogs/types.h> /* UiTextButtonTemplateFields: the fields of the truncated text button nodes */
#include <thandor/ui/text/types.h>
#include <thandor/world/terrain/types.h>

struct WorldRuntimeContext;
struct WorldRuntimeInteractionState;
struct WorldMotionState;
struct WorldFieldRegionState;
struct WorldRuntimeSelectionState;
struct WorldLightingState;
struct WorldMotionSnapshot;
struct InGamePersistentSettingsPage3508;
struct InGameCommandTextEntryPage2320;
struct InGameCommandTextEditControlCC;
struct InGameNotificationPayload;
struct RecentTextHistoryView;
struct RecentTextHistoryPointerList;
struct InGameNotificationQueueRecord;
struct InGameCameraCommandDispatchTable;
struct InGameCameraCommandDispatchRecord;
struct InGameRuntimeRoot;
struct TerrainCompositeTextureRuntime;
struct UiCommandRuntimeRecordPrefix;
struct UiCatalogEntryControl;
struct UiCommandSpriteButtonControl;
struct InGamePlayerStatusTextSlot;
struct InGameUiActionHandlerPage12Prefix28;
struct InGameUiActionHandlerPage10Prefix40;
struct InGameUiCommandModeActionHandlerPage11;
struct RuntimeModelFactionPrefix;
struct InGameRuntimeRootUiGridView;
struct InGameRuntimeRootFrameView;
struct ArmyModelTreeNodeAddressView;
struct WorldRuntimeExtendedMapControlView;
struct SelectionPanelCellAdvance;
struct InGameMissionHelpTextPanel;
struct InGameMissionHelpRootView;
struct WorldOwnerListNode;
struct InGameTargetingRootTraversalView;
struct ModelRuntimeNode;
struct MovieRuntime;
struct RecentTextHistorySlot;
struct SelectionPlayerPairRecord;
struct WorldObjectRecord;
struct WorldRuntimeNode;

using ModelDepthBinMask = uint32_t;

using WorldRuntimeFlags = uint32_t;

using WorldObjectRecordCount = uint32_t;

using WorldWorkspaceElementCount = uint32_t;

using WorldRuntimeControlFlags = uint32_t;

using WorldInteractionFlags = UiNodeFlags; /* the world view's UiNodeBase.nodeFlags */

using WorldFieldDimension = uint32_t;

struct WorldRuntimeSelectionState {
    PlayerRuntimeId activePlayerRuntimeId; // Selection-player runtime used to index the local selection block table.
    uint8_t reserved04_0B[8]; // Unresolved selection and overlay state.
    int32_t pointerSurfaceHitWorldX; /* terrain point under the pointer (FrontendModelPointerContext.surfaceHitWorldX); debug overlay slot 10 */
    int32_t pointerSurfaceHitWorldY; /* debug overlay slot 11 */
    int32_t pointerSurfaceHitDepth; /* view depth of the terrain hit; 0x7FFFFFFF (WORLD_POINTER_NO_HIT): none */
    uint8_t reserved18_1F[8];
    Ptr32<struct GameEntityRuntime> selectedEntity; // Current selected entity cleared during destruction and replaced by context-action resolution.
    Ptr32<Bool8 (UiKeyboardStateMask, UiActionId, struct WorldRuntimeContext *)> dispatchCommandCallback; // key commands of the world view: the keyboardFallback slot of FrontendModelPointerHitContext, so it returns true when the key is not taken and the pointer context passes it on
    Ptr32<uint32_t (uint32_t, uint32_t, uint32_t, uint32_t, struct WorldOwnerListNode *, struct WorldRuntimeContext *)> resolveContextActionPrimaryCallback;
    Ptr32<uint32_t (uint32_t, uint32_t, uint32_t, uint32_t, struct WorldOwnerListNode *, struct WorldRuntimeContext *)> resolveContextActionSecondaryCallback;
    Ptr32<void (uint32_t, uint32_t, uint32_t, uint32_t, struct WorldOwnerListNode *, struct WorldRuntimeContext *)> beginPointerCaptureCallback;
    Ptr32<void (uint32_t, uint32_t, uint32_t, uint32_t, struct WorldOwnerListNode *, struct WorldRuntimeContext *)> updateDragSelectionCallback;
    Ptr32<void (uint32_t, uint32_t, uint32_t, uint32_t, struct WorldOwnerListNode *, struct WorldRuntimeContext *)> commitPointerActionCallback;
    Ptr32<void (struct WorldRuntimeContext *)> dispatchWorldContextActionCallback;
    uint32_t rightButtonHoldTicks; // Counted up by the camera motion update while the right button is held (runtime flag 0x40); rightButtonState11C of FrontendModelPointerContext.
};

struct WorldMotionState {
    Q12 positionXQ12; 
    Q12 positionYQ12; 
    Q12 positionZQ12; 
    UQ12 positionMagnitudeQ12; 
    AngleTurn32 headingAngle; 
    AngleTurn32 pitchAngle;
    uint32_t projectionShift; // FrontendModelPointerContext.projectionShift (graphics projection state); kept by WorldRuntime_SetCameraAnglesAndMagnitudeClamped.
    UQ12 committedDistanceQ12;
    Q12 targetPositionXQ12; 
    Q12 targetPositionYQ12; 
    Q12 targetPositionZQ12; 
    UQ12 targetDistanceQ12; 
    AngleTurn32 minimumPitchAngle; 
    AngleTurn32 maximumPitchAngle; 
};

struct WorldMotionSnapshot {
    Q12 positionXQ12; 
    Q12 positionYQ12; 
    Q12 positionZQ12; 
    UQ12 magnitudeQ12; 
    AngleTurn32 headingAngle; 
    AngleTurn32 pitchAngle; 
    UQ12 distanceQ12; 
};

struct WorldLightingState {
    PackedArgb32 rampStepColorArgb; // +0x120: third argument of TerrainLighting_BuildColorRampAndSetBaseColor, scaled by (256 - i) / 256 into the ramp (level terrainRampStepColorArgb).
    PackedArgb32 baseColorArgb; // +0x124: second argument of TerrainLighting_BuildColorRampAndSetBaseColor: ramp offset and alpha, fills the directional-light LUT (level terrainBaseColorArgb).
    PackedArgb32 color128Argb; 
    PackedArgb32 secondaryColorArgb; // Passed as the secondary colour to TerrainLighting_BuildColorRampAndSetBaseColor.
    PackedArgb32 color130Argb;
    PackedArgb32 color134Argb; 
    PackedArgb32 color138Argb; 
    PackedArgb32 color13CArgb; 
};

struct WorldRuntimeInteractionState {
    uint8_t reserved00_47[72]; // UiNodeBase of the world view control up to its nodeFlags.
    WorldInteractionFlags nodeFlags; // UiNodeBase.nodeFlags of the world view: UI_NODE_SUPPRESSED (8) while a window blocks the world input.
};

struct WorldFieldRegionState {
    Ptr32<void (struct WorldRuntimeContext *)> clearTransientStateCallback;
    uint32_t regionToolMode; // Copy of g_UiCommandModeF (region tool option 0/1).
    WorldFieldDimension auxiliaryAzimuthAngle; // Auxiliary angle pair stored by WorldRuntime_RecomputeFieldRegionNormalsAndLighting (saved with the level); 16-bit angle.
    WorldFieldDimension auxiliaryElevationAngle; // Clamped to -0x4000..-0x1000 like the light elevation.
};

/* Key codes of the camera key table (InGameCameraCommandDispatchRecord.keyCode, compared with the dispatched
   UiActionId; 0 ends the table). */
enum class InGameCameraCommandKeyCode : int32_t {
    EncodedDigit1=196657,
    EncodedDigit2=196658,
    EncodedDigit3=196659,
    EncodedDigit4=196660,
    EncodedDigit5=196661,
    EncodedDigit6=196662,
    EncodedDigit7=196663,
    EncodedLowercaseC=196707,
    EncodedLowercaseS=196723
};
using enum InGameCameraCommandKeyCode;

/* The notification target button's cursorFrame as the targeting handler reads it (InGameTargetingRootTraversalView). */
enum class InGameTargetingObservedActionState : int32_t {
    INGAME_TARGETING_OBSERVED_IDLE=0,
    INGAME_TARGETING_OBSERVED_ADVANCE_OR_RESOLVE=7,
    INGAME_TARGETING_OBSERVED_CANCEL_AND_RESTORE=27
};
using enum InGameTargetingObservedActionState;

/* The same cursorFrame as InGameRuntimeRoot.notificationButtonCursorFrame: 0 idle, 7 while a notification target
   can be jumped to, 0x1B after the jump (the next click cancels). */
enum class InGameNotificationInteractionState : int32_t {
    NOTIFICATION_INTERACTION_NONE=0,
    PAYLOAD_ACTIVE=7,
    NOTIFICATION_INTERACTION_JUMPED=27 /* INGAME_TARGETING_OBSERVED_CANCEL_AND_RESTORE */
};
using enum InGameNotificationInteractionState;

/* UiCommandSpriteButtonControl.activationInputState: the keyboard state mask (g_KeyboardStateMask, low nibble the
   modifier keys) at the button release plus the two marker bits. Only bit operations use it. */
enum class UiCommandActivationStateFlags : uint32_t {
    UI_COMMAND_ACTIVATION_RELATION_RESET_REQUEST_MASK=12,
    UI_COMMAND_ACTIVATION_LOW_INPUT_NIBBLE_MASK=15,
    UI_COMMAND_ACTIVATION_REPEAT_OR_DOUBLE_CLICK=262144,
    UI_COMMAND_ACTIVATION_ALTERNATE_BUTTON=2147483648
};
THANDOR_FLAG_ENUM(UiCommandActivationStateFlags);
using enum UiCommandActivationStateFlags;

/* InGameNotificationPayload.payloadKind: what the notification target button jumps to. */
enum class InGameNotificationPayloadKind : int32_t {
    NOTIFICATION_PAYLOAD_NONE=0,
    ARMY_CREATED=1,
    TECHNOLOGY_UNLOCK_POSITION=2,
    FACTION_IMPACT_ANCHOR=3
};
using enum InGameNotificationPayloadKind;

using ArmyBuildXeniteCostQ4 = uint32_t;

using SelectionPanelCellIndex = int;

using UiTechnologyValueTextBuffer16Utf16 = uint16_t[16];

using InGameNotificationPriority = uint32_t;

using UiCommandModeIndex = uint32_t;

using InGameNotificationMovieId = uint32_t;

using InGamePersistentSettingsPageSourceNodePtr = struct UiNodeBase *; /* interior pointer: points at InGamePersistentSettingsPage3508.sourceNode; the containing InGamePersistentSettingsPage3508 is found by subtracting the field offset */

struct WorldRuntimeContext {
    struct WorldRuntimeInteractionState interaction; // In-game interaction state.
    WorldRuntimeFlags runtimeFlags; // World runtime mode and dirty flags.
    FactionRuntimeIndex activeFactionRuntimeIndex; // Faction runtime index used to select the local faction record and compare model/runtime ownership throughout the in-game world.
    Ptr32<struct FieldGridAsset> fieldGrid; // Attached field grid.
    Ptr32<struct WorldObjectRecord> objectArray; // Attached world-object array.
    RuntimeToken pendingToken; // Pending world token.
    struct WorldMotionState motion; // Live and target motion state.
    UQ12 minimumCameraDistanceQ12; // Lower Q12 camera/world-motion distance clamp. Initialized to 0x8000 by both session initializers and used as the lower bound by motion zoom/clamp paths.
    UQ12 maximumCameraDistanceQ12; // Upper Q12 camera/world-motion distance clamp. Initialized to 0x13000 by both session initializers and used as the upper clamp and terrain/secondary ray-distance limit.
    UiPixelCoordinate pointerCaptureX; // Pointer-capture X anchor. World-camera pointer input subtracts this from pointerX and warps the pointer back here after each handled delta.
    UiPixelCoordinate pointerCaptureY; // Pointer-capture Y anchor. World-camera pointer input subtracts this from pointerY and warps the pointer back here after each handled delta.
    uint32_t reservedA8; // Unresolved trailing dword of the former A0..AB runtime span; kept deliberately generic.
    WorldObjectRecordCount objectCount; // Attached world-object count.
    struct WorldFieldRegionState fieldRegion; // Field-region dimensions and retained prefix.
    Ptr32<uintptr_t> dwordArray; // Attached workspace: SpatialSoundSlot pointers by sound index (pointer-sized, runtime only).
    WorldWorkspaceElementCount dwordArrayCount; // Attached workspace element count.
    uint32_t reservedC8; // Never accessed.
    WorldRuntimeControlFlags runtimeControlFlags; // Secondary world control/state flags.
    Ptr32<RuntimeSpinLockValue> tickSpinLock; // Pointer to g_InGameStateTickSpinLock installed by both session initializers.
    Ptr32<void ()> simulationAndNetworkTickCallback; // In-game simulation/network tick callback installed by both session initializers.
    Ptr32<struct WorldOwnerListNode> ownerListHead; // World-runtime owner-list head.
    struct WorldRuntimeSelectionState selection; // In-game selection and overlay state.
    struct WorldLightingState lighting; // Terrain-lighting configuration.
    struct WorldMotionSnapshot snapshot; // Captured motion snapshot.
};

struct InGamePersistentSettingsPage3508 {
    struct UiPageStackControl settingsPageStack; 
    uint8_t reserved0054_1A9F[6732]; 
    struct UiNodeBase sourceNode; 
    uint8_t reserved1AEC_2DCB[4832]; 
    struct UiSelectableControl musicEnabledControl; 
    uint8_t reserved2E20_2E2B[12]; 
    struct UiSelectableControl soundEffectsEnabledControl; 
    uint8_t reserved2E80_2E8B[12]; 
    struct UiSelectableControl reverseStereoControl; 
    uint8_t reserved2EE0_2FFF[288]; 
    struct UiNumericTextControl soundEffectsGainControl; 
    uint8_t reserved3094_317B[232]; 
    struct UiNumericTextControl movieDefaultAudioGainControl; 
    uint8_t reserved3210_32F7[232]; 
    struct UiNumericTextControl musicGainControl; 
    uint8_t reserved338C_3473[232]; 
    struct UiNumericTextControl movieAlternateAudioGainControl; 
};

using SelectionPanelSegmentCount = int;

using PlayerOrFactionRuntimeId32 = int;

using FactionArmyContributionValue = uint32_t;

using InGameSaveGamePageControlAddress32 = intptr_t; /* address of the save page's delete button node, pointer-sized (5f) */

using InGamePointerCallbackValue2 = uint32_t;

using InGamePointerCallbackValue3 = uint32_t;

using InGamePointerCallbackValue0 = uint32_t;

using InGamePointerCallbackValue1 = uint32_t;

using UiPointerRegionCode = int;

using UiSelectionDetailTextBuffer64Utf16 = uint16_t[64];

using ArmyBuildDurationQ5 = uint32_t;

/* g_UiCommandRuntimeFlags: in-game session and UI state bits. Command 0x310 (INGAME_COMMAND_APPLY_UI_FLAG_MASKS,
   UiCommandRuntimeFlags_ApplyClearSetToggleMasks) takes three raw masks, so any bit can be set; values without an
   enumerator stay valid (fixed underlying type). */
enum class UiCommandRuntimeFlagMask : uint32_t {
    /* bits that gate the simulation step (InGameRuntime_UpdateSimulationAndNetworkTick) */
    UI_COMMAND_RUNTIME_FLAG_PAUSED = 0x01, /* toggled once every player agrees (InGameCommand_TogglePauseRequest);
                                              set at session start */
    UI_COMMAND_RUNTIME_FLAG_AI_PLANNING_OFF = 0x02, /* skips the AI planning phase in local games
                                                       (gameplay/ai/planning.cpp); no writer with a constant mask in
                                                       the original, so it can only come from command 0x310 */
    UI_COMMAND_RUNTIME_FLAG_INTERACTION_SUBSYSTEM_ACTIVE = 0x04, /* set with PAUSED by
                                                                    InGameUiCommandRuntime_ApplyInteractionSubsystemActiveState;
                                                                    world sounds, camera keys and the full simulation
                                                                    step are skipped meanwhile */
    UI_COMMAND_RUNTIME_FLAG_LOCAL_FACTION_ENDED = 0x08, /* an end trigger ended the local faction
                                                           (InGameConditionRuntime_UpdateScheduledRecords); the step
                                                           then sets occupancy bit 0 on every cell */
    UI_COMMAND_RUNTIME_FLAG_WAITING_FOR_PLAYERS = 0x10, /* set with PAUSED at session start, cleared with it when every
                                                           player is ready
                                                           (FrontendPlayerRuntime_IncrementReadyCountAndResolveConsensus) */
    UI_COMMAND_RUNTIME_FLAG_PLACEMENT_PENDING = 0x20, /* an army asset waits for placement on the map
                                                         (InGameCommand_ExecuteLocalPlacementFromSelection) */
    UI_COMMAND_RUNTIME_FLAG_DRAW_DEBUG_CELL_MARKERS = 0x40, /* world view debug overlay
                                                               SelectionOverlay_DrawDebugMarkedCellMarkers; no writer
                                                               with a constant mask, so only from command 0x310 */
    UI_COMMAND_RUNTIME_FLAG_COMMAND_POINTER_CAPTURED = 0x80, /* a command-mode click captured the pointer
                                                                (InGameWorldInput_BeginPointerCapture); the release
                                                                then issues the mode command */
    UI_COMMAND_RUNTIME_FLAG_WORLD_INPUT_DISABLED = 0x100, /* set with LOCAL_FACTION_ENDED; the world input handlers
                                                             (ui/ingame/world_input.cpp) then ignore the map */
    UI_COMMAND_RUNTIME_FLAG_HIDE_WORLD_TEXTS = 0x200, /* hides the world view status texts
                                                         (UiCommandVisibility*Text_DrawWhenAllowed); no writer with a
                                                         constant mask, so only from command 0x310 */
    UI_COMMAND_RUNTIME_FLAG_PAUSED_BEFORE_WINDOW = 0x400, /* the game was already paused when a pausing window opened */
    UI_COMMAND_RUNTIME_FLAG_END_MOVIE_PENDING = 0x800, /* ends the session loop (InGameRuntime_RunSessionUntilExit): an
                                                          end trigger fired and chose the end movie
                                                          (gameplay/session/level_script.cpp) */
    UI_COMMAND_RUNTIME_FLAG_RESULTS_CLOSED = 0x1000, /* ends the results screen after the end movie
                                                        (Frontend_PlaySelectedEndMovie); set by the results buttons
                                                        (actions 0x101B and 0x1025, ui/ingame/pages.cpp) */
    UI_COMMAND_RUNTIME_FLAG_PLACEMENT_OVERLAY_SHOWN = 0x2000, /* the placement overlay was drawn onto the field grid
                                                                 (InGameUiRoot_UpdateFrame) */
    UI_COMMAND_RUNTIME_FLAG_WINDOW_PAUSE = 0x4000, /* a window paused the local game while open (mission help:
                                                      InGameMissionHelpPage_Toggle, settings:
                                                      InGameSettingsPage_ToggleAndSynchronizeControls) */
    UI_COMMAND_RUNTIME_FLAG_HIDE_WORLD_OVERLAYS = 0x8000, /* skips every selection overlay of the world view
                                                             (FrontendModelPointerContext_RenderWorldViewQueuesClipped);
                                                             only from command 0x310 */
    UI_COMMAND_RUNTIME_FLAG_SESSION_CLOSED = 0x10000, /* ends the session loop: command 150 with flag bit 1 closed the
                                                         session (InGameCommand_HandlePlayerDeparture) */
    UI_COMMAND_RUNTIME_FLAG_LOCAL_PLAYER_LEFT = 0x20000, /* ends the session loop: command 150 reported the local
                                                            player's departure */
    UI_COMMAND_RUNTIME_FLAG_CHEATS_ENABLED = 0x40000, /* toggled by typing the cheat code into the chat line
                                                         (InGameChatInput_SendLineOrCheckCheatPhrase) */
    UI_COMMAND_RUNTIME_FLAG_CHEAT_PHRASE_ENTERED = 0x80000, /* set with every cheat toggle by the chat phrase; no
                                                               reader found */
    UI_COMMAND_RUNTIME_FLAG_CHEAT_FAST_BUILD = 0x100000, /* cheat hotkey: build and research times / 16 */
};
THANDOR_FLAG_ENUM(UiCommandRuntimeFlagMask);
using enum UiCommandRuntimeFlagMask;

using ModelLinkedDefinitionListAddress32 = int;

using SelectionPanelNumericValue32 = int;

using MusicTrackClassId = uint32_t;

using InGameCommandPanelSourceAddress32 = intptr_t; /* address of the command panel source node, pointer-sized (5f) */

using InGameCommandPayloadTripletValue32 = uint32_t;

using InGameCommandTextEntryPageTextEditPtr = struct InGameCommandTextEditControlCC *; /* interior pointer: points at InGameCommandTextEntryPage2320.commandTextEdit; the containing InGameCommandTextEntryPage2320 is found by subtracting the field offset */

struct InGameCommandTextEditControlCC {
    struct UiNodeBase base; 
    UiTextEditStateFlags editStateFlags; 
    UiActionId actionId; 
    UiPixelOffset horizontalScrollPixels; 
    uint32_t bufferCapacityCodeUnits; 
    UiTextCodeUnitIndex cursorIndex; 
    UiTextCodeUnitIndex selectionStart; 
    UiTextCodeUnitIndex selectionEnd; 
    Ptr32<struct SoundVoiceSet> activationSound; 
    uint16_t textBuffer[48]; 
};

struct InGameCommandTextEntryPage2320 {
    struct UiPageStackControl commandPageStack; 
    uint8_t reserved0054_0057[4]; 
    struct InGameCommandTextEditControlCC commandTextEdit; 
    uint8_t reserved0124_1E2B[7432]; 
    struct UiSelectableControl packedStateModeLowControl; 
    uint8_t reserved1E80_1E8B[12]; 
    struct UiSelectableControl packedStateModeHighControl; 
    uint8_t reserved1EE0_1EEB[12]; 
    struct UiSelectableControl packedStateModeFallbackControl; 
    uint8_t reserved1F40_208B[332]; 
    struct UiSelectableControl selectionSlot0Control; 
    uint8_t reservedSelectionSlotGap0[12]; 
    struct UiSelectableControl selectionSlot1Control; 
    uint8_t reservedSelectionSlotGap1[12]; 
    struct UiSelectableControl selectionSlot2Control; 
    uint8_t reservedSelectionSlotGap2[12]; 
    struct UiSelectableControl selectionSlot3Control; 
    uint8_t reservedSelectionSlotGap3[12]; 
    struct UiSelectableControl selectionSlot4Control; 
    uint8_t reservedSelectionSlotGap4[12]; 
    struct UiSelectableControl selectionSlot5Control; 
    uint8_t reservedSelectionSlotGap5[12]; 
    struct UiSelectableControl selectionSlot6Control; 
};

struct InGameNotificationPayload {
    Q12 worldXQ12; // Target position (compared with WorldOwnerListNode.worldXQ12, camera origin X).
    Q12 worldYQ12;
    AngleTurn32 headingAngle; // Camera heading when jumping to the target.
    uint32_t orientationOrPresentationValue;
    uint32_t reserved10;
    InGameNotificationPayloadKind payloadKind;
};

struct RecentTextHistoryPointerList {
    uint32_t count; 
    Ptr32<struct RecentTextHistorySlot> entries[8]; 
};

struct RecentTextHistoryView {
    uint8_t reserved00_57[88]; 
    struct RecentTextHistoryPointerList recentTextPointerList; 
    uint8_t reserved7C_FF[132]; 
};

struct InGameNotificationQueueRecord {
    InGameNotificationMovieId movieId;
    InGameNotificationPriority priority;
    struct InGameNotificationPayload payload;
};

/* Actions of the camera key table (g_InGameCameraCommandDispatchRecords16). */
enum class InGameCameraKeyAction : uint32_t {
    RecallBookmark1 = 1,        /* 1 */
    RecallBookmark2 = 2,        /* 2 */
    RecallBookmark3 = 3,        /* 3 */
    RecallBookmark4 = 4,        /* 4 */
    RecallBookmark5 = 5,        /* 5 */
    RecallBookmark6 = 6,        /* 6 */
    RecallBookmark7 = 7,        /* 7 */
    ToggleShading = 8,          /* Alt+S */
    StoreBookmark1 = 9,         /* Alt+1 */
    StoreBookmark2 = 10,        /* Alt+2 */
    StoreBookmark3 = 11,        /* Alt+3 */
    StoreBookmark4 = 12,        /* Alt+4 */
    StoreBookmark5 = 13,        /* Alt+5 */
    StoreBookmark6 = 14,        /* Alt+6 */
    StoreBookmark7 = 15,        /* Alt+7 */
    ToggleUnlimitedCamera = 16, /* Ctrl+C */
};

struct InGameCameraCommandDispatchRecord {
    InGameCameraCommandKeyCode keyCode;
    UiKeyboardStateMask requiredModifierMask;
    InGameCameraKeyAction action;
};

struct InGameCameraCommandDispatchTable {
    struct InGameCameraCommandDispatchRecord records[16];
    uint32_t terminatorKeyCode;
    uint8_t alignmentPadding[12];
};
#pragma pack(push, 1) /* packed layout: no alignment padding */
/* The in-game UI root (g_InGameRuntimeRoot, 0xC3E4 bytes): a copy of the InGameUiImage template
   (below) whose node fields are named here where the code reaches them through the root. The
   reserved ranges hold the other template nodes. */
struct InGameRuntimeRoot {
    struct UiRootNode rootUi; // Exact UiRootNode prefix used by root-stack and shutdown paths.
    uint8_t reserved0058_017B[292];
    struct UiPageStackControl primaryPageStack;
    uint8_t reserved01D0_022B[92];
    Ptr32<struct MovieRuntime> activeEndMovieRuntime;
    uint32_t endMoviePlaybackState; // Cleared when selected end-movie playback begins; exact wider meaning remains deferred.
    uint8_t reserved0234_02F7[196];
    struct UiPageStackControl endMoviePageStack;
    uint8_t reserved034C_08D3[1416];
    Ptr32<struct MovieRuntime> levelMovieRuntime;
    uint8_t reserved08D8_08E3[12];
    struct UiNodeBase playerStatusNode;
    uint8_t reserved0930_093B[12];
    uint32_t playerStatusLineCount; // UiConditionalActionControl.lineCount of the player status box (+0x8E4); written by InGamePanel_RebuildPlayerStatusRows and cleared by both session initializers.
    uint8_t reserved0940_09B7[120];
    struct RecentTextHistoryPointerList recentTextHistory;
    uint8_t reserved09DC_0A03[40];
    uint32_t worldViewAreaRightOffset; // UiNodeBase.rightOffset of the world view area (+0x9DC); cleared when the side panel is switched off, before the root layout.
    uint8_t reserved0A08_0A2F[40];
    struct WorldRuntimeContext worldRuntime;
    Ptr32<void (GraphicsBooleanState, struct WorldRuntimeContext *)> worldOverlayCallback; // Overlay rebuild/release callback installed identically for new and loaded sessions.
    int32_t pointerPressX; /* pointer position at the button press */
    int32_t pointerPressY;
    int32_t pointerX; /* current pointer position */
    int32_t pointerY;
    Ptr32<struct SelectionPlayerPairRecord> localPlayerMarkedCells; // The local player's SelectionPlayerRuntimeBlock.markedCells, drawn as terrain point markers.
    uint32_t localPlayerMarkedCellCount; // Copy of the local player's markedCellCount.
    int32_t lightAzimuthAngle; // Terrain light direction (16-bit angle), set by WorldRuntime_RecomputeFieldRegionNormalsAndLighting; wraps.
    int32_t lightElevationAngle; // Terrain light elevation (16-bit angle), -0x4000 (straight down) .. -0x1000.
    uint8_t reserved0BB0_0BCF[32];
    struct UiPageStackControl gameWindowPageStack;
    uint8_t reserved0C24_24DF[6332];
    uint32_t worldViewWrappedTextNodeFlags; // UiNodeBase.nodeFlags of the wrapped world view text (+0x2498); UI_NODE_SUPPRESSED in a local game.
    uint8_t reserved24E4_40AB[7112];
    struct UiPageStackControl sidePanelPageStack; // Page 1 (no side panel) when persistent-settings bit 0x4 selects the editor layout.
    uint8_t reserved4100_452F[1072];
    struct UiPageStackControl resourceBarModePageStack; // Switched by the game/editor layout toggle.
    uint8_t reserved4584_4643[192];
    struct UiPageStackControl gamePanelsModePageStack; // Switched by the game/editor layout toggle.
    uint8_t reserved4698_4937[672];
    uint32_t minimapResourceButtonStateFlags; // UiSelectableControl.stateFlags of the resource panel sprite button (+0x48EC); UI_SELECTABLE_SELECTED_OR_CHECKED shows the resource plane on the minimap.
    uint8_t reserved493C_49B3[120];
    int primaryResourceDisplayCurrent;
    int primaryResourceDisplayLimit;
    uint8_t reserved49BC_4A4B[144];
    int secondaryResourceDisplayCurrent;
    int secondaryResourceDisplayLimit;
    uint8_t reserved4A54_4AE3[144];
    int energyDemandDisplay; // Supplied plus unpowered energy demand of the active faction.
    int energyCapacityDisplay; // Energy generation capacity of the active faction.
    uint8_t reserved4AEC_4B27[60];
    int baselineEnergySupplyDisplay; // Baseline energy supply plus the (unshifted) tritium extraction rate.
    uint8_t reserved4B2C_4D53[552];
    uint32_t diplomacyPanelNodeFlags; // UiNodeBase.nodeFlags of the diplomacy panel (+0x4D0C).
    uint8_t reserved4D58_9A6B[19732];
    FieldGridCoordinates minimapOriginGridPosition; // Minimap (UiSelectionGeometryControl at +0x9A1C) source origin: the camera target in grid coordinates.
    Q12 minimapSampleScaleQ12; // Minimap sampleScaleQ12, follows the camera distance unless automatic zoom is off.
    AngleTurn32 minimapRotationAngle; // Minimap rotationAngle, follows the camera heading unless automatic rotation is off.
    Ptr32<struct TerrainCompositeTextureRuntime> minimapTextureSource; // Minimap textureSource: the terrain composite texture.
    uint8_t reserved9A80_9B4B[204];
    InGameNotificationInteractionState notificationButtonCursorFrame; // UiImageActionControl.cursorFrame of the notification target button (+0x9AFC): 7 while a notification target can be jumped to, 0x1B after the jump (next click cancels), 0 idle.
    UPtr32 notificationButtonTextureSource; // Its textureSource: the playing notification movie, or the panel texture when none plays.
    uint32_t notificationButtonSubresource; // Its subresource: 0 for a movie frame, 0x25 (idle panel image) after playback closes.
    uint8_t reserved9B58_9E3F[744];
    struct InGameNotificationPayload activeNotificationPayload; // Payload promoted from the head queue record when its movie opens.
    Q12 targetingWorldXQ12; // Targeting scratch coordinate copied from activeNotificationPayload.worldXQ12 by the impact-anchor targeting path.
    Q12 targetingWorldYQ12; // Targeting scratch coordinate copied from activeNotificationPayload.worldYQ12 by the impact-anchor targeting path.
    struct InGameNotificationQueueRecord notificationQueue[4]; // Four exact 0x20-byte records maintained in descending priority order.
    uint8_t reserved9EE0_9FAB[204];
    struct UiPageStackControl selectionDetailPageStack;
    uint8_t reservedA000_A05F[96];
    uint32_t selectionDetailArmyAssetValue;
    uint8_t reservedA064_A067[4];
    Ptr32<struct GameEntityRuntime> selectionDetailEntity;
    uint8_t reservedA06C_C3E3[9080];
};
#pragma pack(pop)

struct TerrainCompositeTextureRuntime {
    struct GraphicsTextureSourceAsset textureSource; 
    struct GraphicsTextureSourceEntry sourceEntries[3]; 
    uint32_t argbPixels[1]; 
};

struct UiCommandRuntimeRecordPrefix {
    AssetRecordByteCount byteSize; /* ArmyAssetRecord.byteSize */
    ArmySelectionDetailTemplateVariantIndex selectionDetailTemplateVariantIndex; /* +0x04 added to the hover text id base */
    PckArmyAssetIdCatalog armyAssetId;
    uint32_t rootNodeOffsetOrPointer; /* +0x0C ArmyAssetRecord.rootNodeOffsetOrPointer (ArmyModelTreeNode * after registration) */
    Ptr32<void> linkedRuntimeOrRecord10;
    uint32_t assetFlags14; /* +0x14 army asset flags (ArmyAssetRecord.flags): 1 buildable, 0x10 special catalog, rest capability bits */
    uint8_t reserved18_1B[4];
    Ptr32<struct GraphicsTextureSourceAsset> textureSource;
    uint32_t reserved20;
    ArmyBuildDurationQ5 buildDurationQ5; 
    ArmyBuildXeniteCostQ4 buildXeniteCostQ4; 
};

struct UiCommandSpriteButtonControl {
    struct UiSpriteButtonControl sprite; 
    UiCommandActivationStateFlags activationInputState; 
};

/* Control types of the in-game template vtables that share a layout with an existing control (step 13 U1; see
   UiFocusProxyControl in ui/controls/types.h).
   - g_UiCommandSpriteButtonWithDetailsVtable: a command sprite button (UiCommandSpriteButtonControl_BeginPress,
     _NonRightRelease, _RightRelease) whose pointerMove shows the hovered army stock slot in the selection detail
     panel (InGameArmyStock_PointerMoveShowSlotDetails); the army stock slots and diplomacy relation buttons.
     0x7C bytes, 12 dwords after UiNodeBase.
   - g_UiCommandVisibilitySingleLineTextVtable: a single-line label drawn only while the world texts are shown
     (UiCommandVisibilitySingleLineText_DrawWhenAllowed reads labelFlags 0x800/0x1000); 0x5C bytes.
   - g_UiCommandVisibilityWrappedTextVtable: the wrapped counterpart (UiCommandVisibilityWrappedText_DrawWhenAllowed
     reads labelFlags 0x800); 0x5C bytes. */
using UiCommandSpriteButtonWithDetails = UiCommandSpriteButtonControl;
using UiCommandVisibilitySingleLineText = UiSingleLineTextControl;
using UiCommandVisibilityWrappedText = UiWrappedTextControl;

struct UiCatalogEntryControl {
    struct UiCommandSpriteButtonControl command; 
    uint32_t runtimeDisplayValueQ4; 
};

struct InGamePlayerStatusTextSlot {
    uint16_t text[64]; 
};

struct InGameUiActionHandlerPage12Prefix28 {
    Ptr32<void (void *)> handlers[28]; 
};

struct InGameUiActionHandlerPage10Prefix40 {
    Ptr32<void (void *)> handlers[40]; 
};

struct InGameUiCommandModeActionHandlerPage11 {
    Ptr32<void (void *)> handlers[30]; 
};

struct RuntimeModelFactionPrefix {
    Ptr32<struct ModelRuntimeSlot> modelRuntime; // Root ModelRuntimeSlot consumed by hierarchy metric wrappers.
    Ptr32<struct ModelRuntimeNode> modelNode; // Model node pointer shared by the observed ArmyRuntimeSlot/GameEntityRuntime headers.
    uint32_t runtimeLinkOrKind08; // Owner-specific runtime link or small kind/state value; semantics deliberately not unified.
    FactionRuntimeIndex factionIndex; // Faction/owner index consumed by faction-runtime lookup.
};
#pragma pack(push, 1) /* packed layout: no alignment padding */
/* InGameRuntimeRoot as seen by the build catalog / army stock grid rebuilds (ui/ingame/build_catalog.cpp, army_stock.cpp): the same
   layout, with the grid panels between +0x4D0C and +0x9A6C named (InGameUiImage template names). */
struct InGameRuntimeRootUiGridView {
    struct UiRootNode rootUi;
    uint8_t reserved0058_017B[292];
    struct UiPageStackControl primaryPageStack;
    uint8_t reserved01D0_022B[92];
    Ptr32<struct MovieRuntime> activeEndMovieRuntime;
    uint32_t endMoviePlaybackState;
    uint8_t reserved0234_02F7[196];
    struct UiPageStackControl endMoviePageStack;
    uint8_t reserved034C_08D3[1416];
    Ptr32<struct MovieRuntime> levelMovieRuntime;
    uint8_t reserved08D8_08E3[12];
    struct UiNodeBase playerStatusNode;
    uint8_t reserved0930_093B[12];
    uint32_t playerStatusLineCount;
    uint8_t reserved0940_09B7[120];
    struct RecentTextHistoryPointerList recentTextHistory;
    uint8_t reserved09DC_0A03[40];
    uint32_t worldViewAreaRightOffset;
    uint8_t reserved0A08_0A2F[40];
    struct WorldRuntimeContext worldRuntime;
    Ptr32<void (GraphicsBooleanState, struct WorldRuntimeContext *)> worldOverlayCallback;
    uint8_t reserved0B90_0B9F[16];
    Ptr32<struct SelectionPlayerPairRecord> localPlayerMarkedCells;
    uint32_t localPlayerMarkedCellCount;
    int32_t lightAzimuthAngle;
    int32_t lightElevationAngle;
    uint8_t reserved0BB0_0BCF[32];
    struct UiPageStackControl gameWindowPageStack;
    uint8_t reserved0C24_24DF[6332];
    uint32_t worldViewWrappedTextNodeFlags;
    uint8_t reserved24E4_40AB[7112];
    struct UiPageStackControl sidePanelPageStack;
    uint8_t reserved4100_452F[1072];
    struct UiPageStackControl resourceBarModePageStack;
    uint8_t reserved4584_4643[192];
    struct UiPageStackControl gamePanelsModePageStack;
    uint8_t reserved4698_4937[672];
    uint32_t minimapResourceButtonStateFlags;
    uint8_t reserved493C_49B3[120];
    int primaryResourceDisplayCurrent;
    int primaryResourceDisplayLimit;
    uint8_t reserved49BC_4A4B[144];
    int secondaryResourceDisplayCurrent;
    int secondaryResourceDisplayLimit;
    uint8_t reserved4A54_4AE3[144];
    int energyDemandDisplay;
    int energyCapacityDisplay;
    uint8_t reserved4AEC_4B27[60];
    int baselineEnergySupplyDisplay;
    uint8_t reserved4B2C_4D0B[480];
    struct UiNodeBase diplomacyPanel; // layout container node
    uint8_t reserved4D58_4D73[28];
    Ptr32<struct SoundVoiceSet> diplomacyPanelSoundVoiceSet; // initialized from g_UiButtonSoundVoiceSets7[0]
    struct UiNodeBase diplomacyFrame; // embedded UI node prefix; rebuild updates layout offsets
    uint8_t reserved4DC4_5DB3[4080];
    struct UiNodeBase buildCatalogPanel; // layout container node of the 48-entry build catalog
    uint8_t reserved5E00_5E1B[28];
    Ptr32<struct SoundVoiceSet> buildCatalogSoundVoiceSet; // initialized from g_UiButtonSoundVoiceSets7[0]
    struct UiNodeBase buildCatalogFrame; // embedded UI node prefix; rebuild updates layout offsets
    uint8_t reserved5E6C_767F[6164];
    struct UiNodeBase specialBuildCatalogPanel; // layout container node of the 42-entry special build catalog
    uint8_t reserved76CC_76E7[28];
    Ptr32<struct SoundVoiceSet> specialBuildCatalogSoundVoiceSet; // initialized from g_UiButtonSoundVoiceSets7[0]
    struct UiNodeBase specialBuildCatalogFrame; // embedded UI node prefix; rebuild updates layout offsets
    uint8_t reserved7738_8C4B[5396];
    struct UiNodeBase armyStockPanel; // layout container node of the 24-slot army stock grid
    uint8_t reserved8C98_8CB3[28];
    Ptr32<struct SoundVoiceSet> armyStockSoundVoiceSet; // initialized from g_UiButtonSoundVoiceSets7[0]
    struct UiNodeBase armyStockFrame; // embedded UI node prefix; rebuild updates layout offsets and node flags
    uint8_t reserved8D04_9A6B[3432];
    FieldGridCoordinates minimapOriginGridPosition;
    uint8_t reserved9A74_9B4B[216];
    InGameNotificationInteractionState notificationButtonCursorFrame;
    uint32_t notificationButtonTextureSource;
    uint32_t notificationButtonSubresource;
    uint8_t reserved9B58_9E3F[744];
    struct InGameNotificationPayload activeNotificationPayload;
    Q12 targetingWorldXQ12;
    Q12 targetingWorldYQ12;
    struct InGameNotificationQueueRecord notificationQueue[4];
    uint8_t reserved9EE0_9FAB[204];
    struct UiPageStackControl selectionDetailPageStack;
    uint8_t reservedA000_A05F[96];
    uint32_t selectionDetailArmyAssetValue;
    uint8_t reservedA064_A067[4];
    Ptr32<struct GameEntityRuntime> selectionDetailEntity;
    uint8_t reservedA06C_C3E3[9080];
};
#pragma pack(pop)

/* InGameRuntimeRoot as seen by its frame update (InGameUiRoot_UpdateFrame). */
struct InGameRuntimeRootFrameView {
    struct UiRootNode rootUi;
    uint8_t reserved0058_09B7[2400];
    struct RecentTextHistoryPointerList recentTextHistory;
    struct UiPageStackControl worldViewAreaPageStack; // The world view area (+0x9DC); page 0 lets the world take edge scrolling.
    struct WorldRuntimeContext worldRuntime;
    uint8_t reserved0B8C_0BCF[68];
    struct UiPageStackControl gameWindowPageStack;
    uint8_t reserved0C24_44BF[14492];
    UiNodeFlags countdownPanelNodeFlags; // UiNodeBase.nodeFlags of the countdown text panel (+0x4478).
};

/* ArmyModelTreeNode (assets/army/catalog.h) with its children read as ModelLinkedDefinitionListAddress32. */
struct ArmyModelTreeNodeAddressView {
    uint8_t unresolved00_07[8];
    uint32_t childListCount;
    ModelLinkedDefinitionListAddress32 childList0Address; /* children[ARMY_WEAPON_SLOT_PRIMARY] */
    ModelLinkedDefinitionListAddress32 childList1Address; /* children[ARMY_WEAPON_SLOT_SECONDARY] */
    ModelLinkedDefinitionListAddress32 childList2Address; /* children[ARMY_WEAPON_SLOT_TERTIARY] */
};

/* The world view of InGameRuntimeRoot (WorldRuntimeContext at root+0xA30) with the root fields that follow it. */
struct WorldRuntimeExtendedMapControlView {
    struct WorldRuntimeInteractionState interaction;
    WorldRuntimeFlags runtimeFlags;
    FactionRuntimeIndex activeFactionRuntimeIndex;
    Ptr32<struct FieldGridAsset> fieldGrid;
    Ptr32<struct WorldObjectRecord> objectArray;
    RuntimeToken pendingToken;
    struct WorldMotionState motion;
    UQ12 minimumCameraDistanceQ12; // Lower Q12 camera/world-motion distance clamp. Initialized to 0x8000 by both session initializers and used as the lower bound by motion zoom/clamp paths.
    UQ12 maximumCameraDistanceQ12; // Upper Q12 camera/world-motion distance clamp. Initialized to 0x13000 by both session initializers and used as the upper clamp and terrain/secondary ray-distance limit.
    UiPixelCoordinate pointerCaptureX; // Pointer-capture X anchor. World-camera pointer input subtracts this from pointerX and warps the pointer back here after each handled delta.
    UiPixelCoordinate pointerCaptureY; // Pointer-capture Y anchor. World-camera pointer input subtracts this from pointerY and warps the pointer back here after each handled delta.
    uint32_t reservedA8; // Unresolved trailing dword of the former A0..AB runtime span; kept deliberately generic.
    WorldObjectRecordCount objectCount;
    struct WorldFieldRegionState fieldRegion;
    Ptr32<uintptr_t> dwordArray;
    WorldWorkspaceElementCount dwordArrayCount;
    uint32_t reservedC8;
    WorldRuntimeControlFlags runtimeControlFlags;
    Ptr32<RuntimeSpinLockValue> tickSpinLock;
    Ptr32<void ()> simulationAndNetworkTickCallback;
    Ptr32<struct WorldOwnerListNode> ownerListHead;
    struct WorldRuntimeSelectionState selection;
    struct WorldLightingState lighting;
    struct WorldMotionSnapshot snapshot;
    uint32_t worldOverlayCallback; // InGameRuntimeRoot.worldOverlayCallback (+0xB8C of the root).
    int pointerPressX; // InGameRuntimeRoot.pointerPressX: pointer position at the button press.
    int pointerPressY;
    int pointerX; // InGameRuntimeRoot.pointerX: current pointer position.
    int pointerY;
};

struct SelectionPanelCellAdvance {
    UiPixelCoordinate nextX; // horizontal coordinate after the cell (origin + offset + width unless suppressed)
    UiPixelCoordinate nextY; // vertical coordinate after the cell (origin + offset + height unless suppressed)
};

/* Scrollable text panel of the mission help window. */
struct InGameMissionHelpTextPanel {
    struct UiScrollableControl scrollable;
    uint8_t reserved0090_00B7[40];
    uint32_t measuredWidth;
    uint32_t measuredHeight;
    uint8_t reserved00C0_00DF[32];
    UiPixelExtent wrapWidth;
    UiTextResourceId textResourceId;
};
#pragma pack(push, 1) /* packed layout: no alignment padding */
/* InGameRuntimeRoot as seen by the mission help toggle (UI action 0x101F, InGameMissionHelpPage_Toggle). */
struct InGameMissionHelpRootView {
    union { struct UiNodeBase base; } rootUi; /* UiRootNode truncated to its 0x4C-byte UiNodeBase */
    uint8_t reserved004C_0A2F[2532];
    struct WorldRuntimeContext worldRuntime;
    uint8_t reserved0B8C_0BCF[68];
    struct UiPageStackControl gameWindowPageStack;
    uint8_t reserved0C24_10FF[1244];
    struct InGameMissionHelpTextPanel missionBriefingPanel; // +0x1100: mission help text of the active faction for this level.
    uint8_t reserved11E8_11EB[4];
    struct InGameMissionHelpTextPanel keyboardHelpPanel; // +0x11EC
    uint8_t reserved12D4_1333[96];
    struct InGameMissionHelpTextPanel mouseHelpPanel; // +0x1334: help text 0x2402.
    uint8_t reserved141C_4387[12140];
    struct UiSelectableControl inGameMenuButton; // +0x4388: the in-game menu toggle, deselected when the help opens.
};
#pragma pack(pop)

struct WorldOwnerListNode {
    Ptr32<struct WorldOwnerListNode> previousNode;
    Ptr32<struct WorldOwnerListNode> nextNode;
    Ptr32<struct WorldRuntimeContext> ownerWorld;
    uint8_t opaque0C_13[8]; // Opaque owner-list bytes; semantics remain class-dependent.
    AngleTurn32 modelLocalRotationAngle2; // ModelRuntimeNode local/world rotation angle 2; valid only when ownerClassId == WORLD_OWNER_RUNTIME_MODEL.
    uint8_t opaque18_47[48]; // Opaque owner-list bytes; semantics remain class-dependent.
    Ptr32<void> runtimePayload; // Class-dependent payload: MODEL=>ModelRuntimeSlot*, SHOT=>ShotRuntimeSlot*, EFFECT=>EffectRuntimeSlot*. Kept void here deliberately so the neutral owner-list view cannot select a false union arm.
    ModelRuntimeFlags runtimeFlags; // the node flags word of every owner-list node kind (+0x4C)
    uint8_t opaque50_57[8]; // Opaque owner-list bytes; semantics remain class-dependent.
    PackedArgb32 modelTintArgb; // ModelRuntimeNode tint ARGB; valid only for MODEL owner nodes.
    uint8_t opaque5C_93[56]; // Opaque owner-list bytes; semantics remain class-dependent.
    Q12 worldXQ12;
    Q12 worldYQ12;
    Q12 worldZQ12;
    uint32_t runtimeStateA0;
    WorldOwnerRuntimeClassId ownerClassId; // Binary constructors prove MODEL=0, SHOT=1, EFFECT=2.
    uint8_t opaqueA8_B3[12]; // Opaque owner-list bytes; semantics remain class-dependent.
    ModelDepthBinMask modelDepthBinMaskNear; // ModelRuntimeNode near depth-bin mask; valid only for MODEL owner nodes.
    ModelDepthBinMask modelDepthBinMaskFar; // ModelRuntimeNode far depth-bin mask; valid only for MODEL owner nodes.
    uint8_t opaqueBC_FF[68]; // Opaque owner-list bytes; semantics remain class-dependent.
};

/* The notification target button (+0x9AFC of InGameRuntimeRoot, a UiImageActionControl) and, after walking up
   its parent chain, InGameRuntimeRoot itself. */
struct InGameTargetingRootTraversalView {
    struct UiNodeBase base; // UI-node prefix valid both for the initiating targeting control and while walking its parent chain.
    uint8_t reserved4C_4F[4];
    InGameTargetingObservedActionState actionState; // The button's cursorFrame (InGameRuntimeRoot.notificationButtonCursorFrame): idle, advance/resolve (7), or cancel/restore (0x1B).
    uint8_t reserved54_A2F[2524];
    struct WorldRuntimeContext worldRuntime; // Root image WorldRuntimeContext reached after parent traversal.
    uint8_t reservedB8C_9B4B[36800];
    InGameNotificationInteractionState notificationButtonCursorFrame; // See InGameRuntimeRoot.
    uint32_t notificationButtonTextureSource;
    uint32_t notificationButtonSubresource;
    uint8_t reserved9B58_9E3F[744];
    struct InGameNotificationPayload activeNotificationPayload; // Active root notification/targeting payload consumed by state-7 targeting resolution.
    Q12 targetingWorldXQ12; // Targeting scratch coordinate written from activeNotificationPayload.worldXQ12 before the impact-anchor fallthrough path.
    Q12 targetingWorldYQ12; // Targeting scratch coordinate written from activeNotificationPayload.worldYQ12 before the impact-anchor fallthrough path.
};

/* UI template node links: offsets from the template start, made into pointers when the
   template is copied and linked. */
#define UI_TEMPLATE_LINK(offset) (reinterpret_cast<UiNodeBase *>(offset))
#define UI_TEMPLATE_NO_LINK (reinterpret_cast<UiNodeBase *>(-1))
/* The same as initialisers of a Ptr32 field of a template image, constant at compile time
   (tools/dev/ui_image_retype.py writes these). */
#define UI_TEMPLATE_LINK_BITS(offset) THANDOR_PTR32_BITS(offset)
#define UI_TEMPLATE_NO_LINK_BITS THANDOR_PTR32_BITS(0xFFFFFFFFu)
/* The dwords in front of a technology area tab (InGameUiImage technologyAreaTabN_prefix): the name text
   id of the tab's technology and the tab's tooltip text (the expanded label), both set at runtime. */
struct UiTechnologyAreaTabPrefix {
    int32_t nameTextResourceId; /* -8: TECHNOLOGY_TEXT_ID_BASE + 2 * technology id */
    Ptr32<uint16_t> tooltipText; /* -4 */
};
#pragma pack(push, 1)

/* g_InGameRuntimeDefaultImageTemplate: 452 UI nodes. InGameUi_Image(root)->node is the node in a copy of it (or a
   node's <node>_prefix), typed as its control. */
struct InGameUiImage {
    UiPanelControl inGameRootPanel; /* +0000 g_UiPanelControlVtable: Root panel of the in-game UI; its only child is the primary page stack. */
    UiLayoutContainerControl<2> chatInputPageStack; /* +0058 g_UiLayoutContainerControlVtable: Two-page stack (empty or chat input) inside the world view area that shows or hides the chat text entry line. */
    UiRequiredTextEditControl chatInputTextEdit; /* +00B0 g_UiRequiredTextEditControlVtable: Chat/command text entry (action 0x1024): sends the typed text to the selected players; in single player it checks the developer cheat phrase. */
    uint32_t chatInputTextEdit_trailing[19]; /* +0130: template dwords behind the control */
    UiLayoutContainerControl<3> primaryPageStack; /* +017C g_UiLayoutContainerControlVtable: Top-level page stack (InGameRuntimeRoot.primaryPageStack); page 1 is the end-movie/results view, it also lists the level movie page and the side panel stack 0x40AC. */
    UiImageActionControl endMovieView; /* +01D8 g_UiImageActionControlVtable: Image/action surface that shows the end movie (activeEndMovieRuntime); action 0x1009 clears the playback flag to skip the movie. */
    UiFillPanelControl endMovieLetterboxTop; /* +0240 g_UiFillPanelControlVtable: Black fill bar over the top eighth of the screen while the end movie plays. */
    UiFillPanelControl endMovieLetterboxBottom; /* +029C g_UiFillPanelControlVtable: Black fill bar over the bottom eighth of the screen while the end movie plays. */
    UiLayoutContainerControl<2> endMoviePageStack; /* +02F8 g_UiLayoutContainerControlVtable: Two-page stack (movie only or results screen); set to page 1 after the end movie to show the results screen. */
    UiImagePanelControl resultsScreenPanel; /* +0350 g_UiImagePanelControlVtable: End-of-game results screen background panel holding the chart tabs, charts and summary text; its image subresource is set to the chart mode index (action 0x1026). */
    UiLayoutContainerControl<3> resultsChartPageStack; /* +03AC g_UiLayoutContainerControlVtable: Page stack switching between the three results charts; page chosen by the chart tab buttons (action 0x101C). */
    FrontendResultsTable<6> resultsChart1; /* +0408 g_FrontendResultsTableVtable: First results statistics chart (graph control); which category (points/economy/military) is not verified. Its modeFlags (+0x4C, table vs graph) is set from the chart mode buttons (action 0x1026). */
    FrontendResultsTable<6> resultsChart2; /* +0484 g_FrontendResultsTableVtable: Second results statistics chart; its modeFlags (+0x4C, table vs graph) is set from the chart mode buttons (action 0x1026). */
    FrontendResultsTable<8> resultsChart3; /* +0500 g_FrontendResultsTableVtable: Third results statistics chart (8 series); its modeFlags (+0x4C, table vs graph) is set from the chart mode buttons (action 0x1026). */
    UiFramedTextButtonControl resultsTabMilitary; /* +0584 g_UiFramedTextButtonControlVtable: Results chart tab button labelled Military (text 0x21B1); action 0x101C selects the chart page. */
    UiFramedTextButtonControl resultsTabEconomy; /* +05E4 g_UiFramedTextButtonControlVtable: Results chart tab button labelled Economy (text 0x21B0); action 0x101C selects the chart page. */
    UiFramedTextButtonControl resultsTabThird; /* +0644 g_UiFramedTextButtonControlVtable: Third results chart tab button (text 0x21AF, probably total/points); action 0x101C selects the chart page. */
    UiFramedTextButtonControl resultsContinueButton; /* +06A4 g_UiFramedTextButtonControlVtable: Results screen continue/OK button (action 0x101B, text 0x21AE) that marks the results as done. */
    UiFramedTextButtonControl resultsSecondaryExitButton; /* +0704 g_UiFramedTextButtonControlVtable: Results button (action 0x1025, text 0x21C5) setting runtime flag 0x1000; suppressed in local games, likely a multiplayer leave/next option. */
    UiFramedTextButtonControl resultsChartModeButtonA; /* +0764 g_UiFramedTextButtonControlVtable: First of two exclusive chart mode buttons (action 0x1026, text 0x21C6); the mode index is mirrored into the charts. */
    UiFramedTextButtonControl resultsChartModeButtonB; /* +07C4 g_UiFramedTextButtonControlVtable: Second of two exclusive chart mode buttons (action 0x1026, text 0x21C7). */
    UiListOffsetControl resultsSummaryText; /* +0824 g_UiListOffsetControlVtable: Results summary text 0x21C0 patched with the level title and elapsed game time. */
    UiNodeBase levelMovieView; /* +0880 g_UiImageActionControlVtable: Image/action surface playing the level movie (levelMovieRuntime lives in this node). */
    uint32_t levelMovieView_fields[6];
    UiConditionalActionTextBox<8> playerStatusBox; /* +08E4 g_UiConditionalActionControlVtable: Multiplayer player status box (types.h: playerStatusNode): UiConditionalActionControl whose eight text lines are g_InGamePlayerStatusTextSlots; InGamePanel_RebuildPlayerStatusRows sets lineCount to the player count and sizes it. Invalidated during movie playback. */
    UiConditionalActionTextBox<8> messageHistoryPanel; /* +0960 g_UiConditionalActionControlVtable: Recent message history display (recentTextHistory lives inside it); action 0x100F trims the history to three lines. */
    UiLayoutContainerControl<1> worldViewArea; /* +09DC g_UiLayoutContainerControlVtable: Container for the main play area left of the side panel (right offset set from the panel width); holds world view, windows, messages and chat input. */
    FrontendModelPointerContext worldView; /* +0A30 g_FrontendModelPointerContextVtable: The 3D world view control (InGameRuntimeRoot.worldRuntime) that receives map pointer input and hosts the on-screen status texts. */
    uint32_t worldView_trailing[9]; /* +0BAC: template dwords behind the control */
    UiLayoutContainerControl<9> gameWindowPageStack; /* +0BD0 g_UiLayoutContainerControlVtable: Page stack of the in-game windows (types.h technologyPageStack0BD0): 0 none, 1 message, 2 technology, 3 game menu, 4 quit, 5 save, 6 graphics, 7 audio, 8 mission help. */
    UiImagePanelControl gameMenuWindow; /* +0C44 g_UiImagePanelControlVtable: Game menu / gameplay options window (page 3, action 0x1003) with save, quit, graphics, audio buttons and camera options. */
    UiImagePanelControl quitGameWindow; /* +0CA0 g_UiImagePanelControlVtable: Leave/quit game window (page 4, opened by action 0x1200); its buttons (0x101D/0x101E/0x1027) are in another part. */
    UiImagePanelControl saveGameWindow; /* +0CFC g_UiImagePanelControlVtable: Save game window (page 5) with save list, save, delete and back buttons. */
    UiImagePanelControl graphicsSettingsWindow; /* +0D58 g_UiImagePanelControlVtable: Graphics settings window (page 6, opened by action 0x1202). */
    UiImagePanelControl audioSettingsWindow; /* +0DB4 g_UiImagePanelControlVtable: Audio settings window (page 7, opened by action 0x1203). */
    UiImagePanelControl missionHelpWindow; /* +0E10 g_UiImagePanelControlVtable: Mission info/help window (page 8, action 0x101F) with three tabs: mission briefing, keyboard help and mouse/cursor help. */
    UiImagePanelControl technologyWindow; /* +0E6C g_UiImagePanelControlVtable: Technology research window (page 2, action 0x1010), sized from the tech.gfx texture. */
    UiFocusProxyControl missionHelpTitle; /* +0EC8 g_UiFocusProxyControlVtable: Title label of the mission help window (text 0x21CC). */
    UiFramedTextButtonControl missionHelpCloseButton; /* +0F24 g_UiFramedTextButtonControlVtable: Close button of the mission help window (action 0x1020, text 0x21CD). */
    UiFramedTextButtonControl missionHelpBriefingTab; /* +0F84 g_UiFramedTextButtonControlVtable: Tab 0 button (action 0x1021, text 0x21CE) showing the mission briefing text. */
    UiFramedTextButtonControl missionHelpKeyboardTab; /* +0FE4 g_UiFramedTextButtonControlVtable: Tab 1 button (action 0x1022, text 0x21CF) showing the keyboard help (text page texte_tastatur). */
    UiFramedTextButtonControl missionHelpMouseTab; /* +1044 g_UiFramedTextButtonControlVtable: Tab 2 button (action 0x1023, text 0x21D0) showing help text 0x2402 with inline cursor icons (likely mouse controls). */
    UiLayoutContainerControl<3> missionHelpTabPageStack; /* +10A4 g_UiLayoutContainerControlVtable: Page stack holding the three scrollable help text pages. */
    UiScrollableControl missionBriefingScroll; /* +1100 g_UiScrollableControlVtable: Scrollable view of the mission briefing text (InGameMissionHelpRootView.missionBriefingPanel). */
    UiListOffsetControl missionBriefingText; /* +1190 g_UiListOffsetControlVtable: Briefing text; its resource id is set from the level title and active faction when the window opens. */
    UiScrollableControl keyboardHelpScroll; /* +11EC g_UiScrollableControlVtable: Scrollable view of the keyboard help (keyboardHelpPanel). */
    UiListOffsetControl keyboardHelpKeyColumn; /* +127C g_UiListOffsetControlVtable: Keyboard help text 0x2400 (likely the key column). */
    UiListOffsetControl keyboardHelpDescriptionColumn; /* +12D8 g_UiListOffsetControlVtable: Keyboard help text 0x2401 next to the key column (likely the descriptions). */
    UiScrollableControl mouseHelpScroll; /* +1334 g_UiScrollableControlVtable: Scrollable view of help text 0x2402 (mouseHelpPanel). */
    UiListOffsetControl mouseHelpText; /* +13C4 g_UiListOffsetControlVtable: Help text 0x2402 bound to the cursor texture (mouse/cursor explanations). */
    UiFocusProxyControl technologyTitle; /* +1420 g_UiFocusProxyControlVtable: Title label of the technology window (text 0x217C). */
    UiFramedTextButtonControl technologyCloseButton; /* +147C g_UiFramedTextButtonControlVtable: Close button (action 0x1011): clears the selected army token and closes the window. */
    UiFramedTextButtonControl technologyResearchButton; /* +14DC g_UiFramedTextButtonControlVtable: Starts research of the selected technology (action 0x1013). */
    UiTechnologyAreaTabPrefix technologyAreaTab1_prefix; /* +153C */
    UiFramedTextButtonControl technologyAreaTab1; /* +1544 g_UiFramedTextButtonControlVtable: Technology area tab 1 (action 0x1014) with an icon from tech.gfx. */
    UiTechnologyAreaTabPrefix technologyAreaTab2_prefix; /* +15A4 */
    UiFramedTextButtonControl technologyAreaTab2; /* +15AC g_UiFramedTextButtonControlVtable: Technology area tab 2 (action 0x1015). */
    UiTechnologyAreaTabPrefix technologyAreaTab3_prefix; /* +160C */
    UiFramedTextButtonControl technologyAreaTab3; /* +1614 g_UiFramedTextButtonControlVtable: Technology area tab 3 (action 0x1016). */
    UiTechnologyAreaTabPrefix technologyAreaTab4_prefix; /* +1674 */
    UiFramedTextButtonControl technologyAreaTab4; /* +167C g_UiFramedTextButtonControlVtable: Technology area tab 4 (action 0x1017). */
    UiTechnologyAreaTabPrefix technologyAreaTab5_prefix; /* +16DC */
    UiFramedTextButtonControl technologyAreaTab5; /* +16E4 g_UiFramedTextButtonControlVtable: Technology area tab 5 (action 0x1018). */
    UiTechnologyAreaTabPrefix technologyAreaTab6_prefix; /* +1744 */
    UiFramedTextButtonControl technologyAreaTab6; /* +174C g_UiFramedTextButtonControlVtable: Technology area tab 6 (action 0x1019). */
    UiTechnologyAreaTabPrefix technologyAreaTab7_prefix; /* +17AC */
    UiFramedTextButtonControl technologyAreaTab7; /* +17B4 g_UiFramedTextButtonControlVtable: Technology area tab 7 (action 0x101A). */
    UiImagePanelControl technologyAreaTab1Icon; /* +1814 g_UiImagePanelControlVtable: Icon image (tech.gfx) of technology area tab 1. */
    UiImagePanelControl technologyAreaTab2Icon; /* +1870 g_UiImagePanelControlVtable: Icon image (tech.gfx) of technology area tab 2. */
    UiImagePanelControl technologyAreaTab3Icon; /* +18CC g_UiImagePanelControlVtable: Icon image (tech.gfx) of technology area tab 3. */
    UiImagePanelControl technologyAreaTab4Icon; /* +1928 g_UiImagePanelControlVtable: Icon image (tech.gfx) of technology area tab 4. */
    UiImagePanelControl technologyAreaTab5Icon; /* +1984 g_UiImagePanelControlVtable: Icon image (tech.gfx) of technology area tab 5. */
    UiImagePanelControl technologyAreaTab6Icon; /* +19E0 g_UiImagePanelControlVtable: Icon image (tech.gfx) of technology area tab 6. */
    UiImagePanelControl technologyAreaTab7Icon; /* +1A3C g_UiImagePanelControlVtable: Icon image (tech.gfx) of technology area tab 7. */
    UiImagePanelControl technologyDescriptionFrame; /* +1A98 g_UiImagePanelControlVtable: Image panel beside the description area, widened by the panel texture subresource width; exact role unverified. */
    UiScrollableControl technologyDescriptionScroll; /* +1AF4 g_UiScrollableControlVtable: Scrollable view of the selected technology description. */
    UiListOffsetControl technologyDescriptionText; /* +1B84 g_UiListOffsetControlVtable: Description text of the selected technology (default 0x217F); wrap width derived from the window width. */
    UiImagePanelControl messageWindow; /* +1BE0 g_UiImagePanelControlVtable: Send-message window (page 1): text entry, recipient mode buttons, player checkboxes and send/cancel buttons. */
    UiFocusProxyControl messageWindowTitle; /* +1C3C g_UiFocusProxyControlVtable: Title label of the message window (text 0x2166). */
    UiRequiredTextEditControl messageTextEdit; /* +1C98 g_UiRequiredTextEditControlVtable: Message text entry whose text is sent as a 48-byte payload. */
    uint32_t messageTextEdit_trailing[19]; /* +1D18: template dwords behind the control */
    UiFramedTextButtonControl messageCancelButton; /* +1D64 g_UiFramedTextButtonControlVtable: Closes the message window (action 0x1002, text 0x2168). */
    UiFramedTextButtonControl messageSendAndCloseButton; /* +1DC4 g_UiFramedTextButtonControlVtable: Sends the message and closes the window (action 0x1005, text 0x2169). */
    UiFramedTextButtonControl messageSendButton; /* +1E24 g_UiFramedTextButtonControlVtable: Sends the message with the current recipient mask (action 0x1004, text 0x2167). */
    UiTextButtonControl messageRecipientPlayersTab; /* +1E84 g_UiTextButtonControlVtable: Recipient mode: fills the seven checkboxes from the active players (action 0x1006, text 0x216B). */
    UiTextButtonControl messageRecipientGroupsTab; /* +1EE4 g_UiTextButtonControlVtable: Recipient mode: fills the seven checkboxes from the runtime record catalog, probably teams (action 0x1007, text 0x216C). */
    UiTextButtonControl messageRecipientAllTab; /* +1F44 g_UiTextButtonControlVtable: Recipient mode that hides the checkbox list, likely send to all (action 0x1008, text 0x216A). */
    UiLayoutContainerControl<2> messageRecipientPageStack; /* +1FA4 g_UiLayoutContainerControlVtable: Two-page stack: recipient checkbox list or nothing. */
    UiScrollableControl messageRecipientScroll; /* +1FFC g_UiScrollableControlVtable: Scrollable view of the recipient checkbox list. */
    UiPanelControl messageRecipientList; /* +208C g_UiPanelControlVtable: Panel holding the seven recipient checkboxes. */
    UiTextButtonControl messageRecipientCheckbox1; /* +20E4 g_UiTextButtonControlVtable: Recipient slot 1 checkbox (text 0x216D, preselected). */
    UiTextButtonControl messageRecipientCheckbox2; /* +2144 g_UiTextButtonControlVtable: Recipient slot 2 checkbox (text 0x216E). */
    UiTextButtonControl messageRecipientCheckbox3; /* +21A4 g_UiTextButtonControlVtable: Recipient slot 3 checkbox (text 0x216F). */
    UiTextButtonControl messageRecipientCheckbox4; /* +2204 g_UiTextButtonControlVtable: Recipient slot 4 checkbox (text 0x2170). */
    UiTextButtonControl messageRecipientCheckbox5; /* +2264 g_UiTextButtonControlVtable: Recipient slot 5 checkbox (text 0x2171). */
    UiTextButtonControl messageRecipientCheckbox6; /* +22C4 g_UiTextButtonControlVtable: Recipient slot 6 checkbox (text 0x2172). */
    UiTextButtonControl messageRecipientCheckbox7; /* +2324 g_UiTextButtonControlVtable: Recipient slot 7 checkbox (text 0x2173). */
    UiCommandVisibilitySingleLineText worldViewCyclingInfoText; /* +2384 g_UiCommandVisibilitySingleLineTextVtable: Single-line world view text whose resource id a hotkey cycles through 0x112..0x117; the value is also published as the world-state mirror. */
    UiCommandVisibilitySingleLineText worldViewStatusTextA; /* +23E0 g_UiCommandVisibilitySingleLineTextVtable: Single-line world view text 0x21D1 shown for command flags 0x801 (a status message, unverified). */
    UiCommandVisibilitySingleLineText worldViewStatusTextB; /* +243C g_UiCommandVisibilitySingleLineTextVtable: Single-line world view text 0x21D5 shown for command flags 0x1001 (a status message, unverified). */
    UiCommandVisibilityWrappedText worldViewWrappedStatusText; /* +2498 g_UiCommandVisibilityWrappedTextVtable: Wrapped world view text shown for command flag 0x10; exact message unverified. */
    UiFocusProxyControl gameMenuTitle; /* +24F4 g_UiFocusProxyControlVtable: Title label of the game menu window (text 0x2123). */
    UiFramedTextButtonControl gameMenuSaveButton; /* +2550 g_UiFramedTextButtonControlVtable: Opens the save game window and rebuilds the save list (action 0x120E, text 0x214D). */
    UiFramedTextButtonControl gameMenuQuitButton; /* +25B0 g_UiFramedTextButtonControlVtable: Opens the quit game window, page 4 (action 0x1200, text 0x2148). */
    UiFramedTextButtonControl gameMenuGraphicsButton; /* +2610 g_UiFramedTextButtonControlVtable: Opens the graphics settings window (action 0x1202, text 0x2121). */
    UiFramedTextButtonControl gameMenuAudioButton; /* +2670 g_UiFramedTextButtonControlVtable: Opens the audio settings window (action 0x1203, text 0x2122). */
    UiTextButtonControl rightButtonNoScrollCheckbox; /* +26D0 g_UiTextButtonControlVtable: Option: right mouse button does not scroll (action 0x1216, text 0x21C8). */
    UiFocusProxyControl scrollSpeedGroup; /* +2730 g_UiFocusProxyControlVtable: Labelled group for the camera scroll speed slider (text 0x21C9). */
    UiFocusProxyControl scrollSpeedMinLabel; /* +278C g_UiFocusProxyControlVtable: Scroll speed slider end label (text 0x21CA), probably slow. */
    UiFocusProxyControl scrollSpeedMaxLabel; /* +27E8 g_UiFocusProxyControlVtable: Scroll speed slider end label (text 0x21CB), probably fast. */
    UiRangeSliderControl scrollSpeedSlider; /* +2844 g_UiRangeSliderControlVtable: Camera scroll step slider (action 0x1217). */
    UiTitledWindowControl autoCameraGroup; /* +28AC g_UiTitledWindowControlVtable: Titled group (text 0x215F) with the automatic zoom/rotation options. */
    UiTextButtonControl autoZoomOffCheckbox; /* +2900 g_UiTextButtonControlVtable: Option: automatic zoom off (action 0x1212, text 0x2160). */
    UiTextButtonControl autoRotationOffCheckbox; /* +2960 g_UiTextButtonControlVtable: Option: automatic rotation off (action 0x1213, text 0x2161). */
    UiTitledWindowControl cameraLinkGroup; /* +29C0 g_UiTitledWindowControlVtable: Titled group (text 0x2162) with the rotation link and hide panel options. */
    UiTextButtonControl linkRotationZoomCheckbox; /* +2A14 g_UiTextButtonControlVtable: Option: link rotation with zoom (action 0x1214, excludes 0x1215, text 0x2163). */
    UiTextButtonControl linkRotationTiltCheckbox; /* +2A74 g_UiTextButtonControlVtable: Option: link rotation with tilt (action 0x1215, excludes 0x1214, text 0x2164). */
    UiTextButtonControl hidePanelCheckbox; /* +2AD4 g_UiTextButtonControlVtable: Option: hide panel (action 0x121B, text 0x2165). */
    UiFramedTextButtonControl gameMenuCloseButton; /* +2B34 g_UiFramedTextButtonControlVtable: Closes the game menu (action 0x1201, text 0x211F). */
    UiFramedTextButtonControl saveGameBackButton; /* +2B94 g_UiFramedTextButtonControlVtable: Returns from the save window to the game menu (action 0x1218, text 0x214F). */
    UiFocusProxyControl saveGameTitle; /* +2BF4 g_UiFocusProxyControlVtable: Title label of the save game window (text 0x214E). */
    UiFramedTextButtonControl saveGameSaveButton; /* +2C50 g_UiFramedTextButtonControlVtable: Saves to the selected or typed save name (action 0x1210, text 0x214D). */
    UiFramedTextButtonControl saveGameDeleteButton; /* +2CB0 g_UiFramedTextButtonControlVtable: Deletes the selected save file and rebuilds the list (action 0x1219, text 0x2153). */
    UiScrollableControl saveGameListScroll; /* +2D10 g_UiScrollableControlVtable: Scrollable area of the save game list (children in part 2). */
    UiListControl saveGameList; /* +2DA0 g_UiListControlVtable: Row list of save/*.sve catalog entries on the save-game page; selecting a row fires action 0x120F (InGameSaveGameList_SelectAndRefreshDetail). */
    uint32_t saveGameList_trailing[2]; /* +2E14: template dwords behind the control */
    UiFocusProxyControl saveGameListHeaderLabel; /* +2E1C g_UiFocusProxyControlVtable: Static text 0x2150 above the save list on the save-game page (header caption). */
    UiListOffsetControl saveGameDescriptionText; /* +2E78 g_UiListOffsetControlVtable: Text box below the list showing the selected save's description (text 0x215D, or 0x215E patched with save details). */
    UiLayoutContainerControl<2> saveNameEntryStack; /* +2ED4 g_UiLayoutContainerControlVtable: Detail page stack of the save page: page 0 empty, page 1 shows the save-name editor when the trailing new-save row is selected. */
    UiRequiredTextEditControl saveNameEdit; /* +2F2C g_UiRequiredTextEditControlVtable: Required text edit for the new save name; action 0x1211 validates the name and enables the Save button (0x1210). */
    uint32_t saveNameEdit_trailing[11]; /* +2FAC: template dwords behind the control */
    UiFocusProxyControl saveNameLabel; /* +2FD8 g_UiFocusProxyControlVtable: Caption text 0x2152 above the save-name edit field. */
    UiFramedTextButtonControl quitMenuBackButton; /* +3034 g_UiFramedTextButtonControlVtable: Quit page button (action 0x1218, text 0x2149) that returns to the in-game menu. */
    UiFocusProxyControl quitMenuTitleLabel; /* +3094 g_UiFocusProxyControlVtable: Title text 0x2144 of the in-game quit page (menu page stack index 4). */
    UiFramedTextButtonControl quitMenuAbortMissionButton; /* +30F0 g_UiFramedTextButtonControlVtable: Action 0x101D: closes the menu and issues operation 0x150 mode 0 (local player departs, runtime flag 0x20000 aborts back to the frontend); text 0x214A. */
    UiFramedTextButtonControl quitMenuSurrenderButton; /* +3150 g_UiFramedTextButtonControlVtable: Action 0x101E: operation 0x150 mode 1, which destroys all armies of the local faction (give up); text 0x214B; label inferred from behaviour. */
    UiFramedTextButtonControl quitMenuRestartMissionButton; /* +31B0 g_UiFramedTextButtonControlVtable: Action 0x1027: operation 0x150 mode 2, sets runtime flag 0x10000 (session ends but keeps the scenario path, likely restart); text 0x214C; label unverified. */
    UiFramedTextButtonControl graphicsOptionsBackButton; /* +3210 g_UiFramedTextButtonControlVtable: Graphics options page button (action 0x1218, text 0x211F) returning to the in-game menu. */
    UiFocusProxyControl graphicsOptionsTitleLabel; /* +3270 g_UiFocusProxyControlVtable: Title text 0x212E of the in-game graphics options page (menu page index 6). */
    UiTextButtonControl shadingEnabledCheckbox; /* +32CC g_UiTextButtonControlVtable: Toggle (action 0x1204, text 0x212F) enabling shading (InGameShadingSettings_SetEnabled). */
    UiTitledWindowControl shadingLevelGroup; /* +332C g_UiTitledWindowControlVtable: Titled frame (text 0x2130) holding the six shading-level choice buttons. */
    UiNumericPairTextButton shadingLevel32x32Button; /* +3380 g_UiNumericPairTextButtonVtable: Shading level choice (action 0x1205) with value pair 0x20/0x20. */
    UiNumericPairTextButton shadingLevel32x64Button; /* +33E8 g_UiNumericPairTextButtonVtable: Shading level choice (action 0x1205) with value pair 0x20/0x40. */
    UiNumericPairTextButton shadingLevel32x128Button; /* +3450 g_UiNumericPairTextButtonVtable: Shading level choice (action 0x1205) with value pair 0x20/0x80. */
    UiNumericPairTextButton shadingLevel64x64Button; /* +34B8 g_UiNumericPairTextButtonVtable: Shading level choice (action 0x1205) with value pair 0x40/0x40. */
    UiNumericPairTextButton shadingLevel64x128Button; /* +3520 g_UiNumericPairTextButtonVtable: Shading level choice (action 0x1205) with value pair 0x40/0x80. */
    UiNumericPairTextButton shadingLevel128x128Button; /* +3588 g_UiNumericPairTextButtonVtable: Shading level choice (action 0x1205) with value pair 0x80/0x80. */
    UiFocusProxyControl modelDetailGroup; /* +35F0 g_UiFocusProxyControlVtable: Captioned group (text 0x2131) around the model-LOD distance slider. */
    UiFocusProxyControl modelDetailMinLabel; /* +364C g_UiFocusProxyControlVtable: Left-aligned end caption (text 0x2134) of the model-LOD slider. */
    UiFocusProxyControl modelDetailMaxLabel; /* +36A8 g_UiFocusProxyControlVtable: Right-aligned end caption (text 0x2135) of the model-LOD slider. */
    UiRangeSliderControl modelDetailSlider; /* +3704 g_UiRangeSliderControlVtable: Range slider (action 0x1206) setting the model LOD depth threshold Q8 (range 0x4000..0x40000). */
    UiTitledWindowControl textureQualityGroup; /* +376C g_UiTitledWindowControlVtable: Titled frame (text 0x2132) holding the three texture-quality radio buttons. */
    UiTextButtonControl textureQualityLowButton; /* +37C0 g_UiTextButtonControlVtable: Texture quality choice Low (action 0x1207, text 0x2136): stores TEXTURE_QUALITY_LOW (downsample shift 2). */
    UiTextButtonControl textureQualityMediumButton; /* +3820 g_UiTextButtonControlVtable: Texture quality choice Medium (action 0x1207, text 0x2137). */
    UiTextButtonControl textureQualityHighButton; /* +3880 g_UiTextButtonControlVtable: Texture quality choice High (action 0x1207, text 0x2138): stores TEXTURE_QUALITY_HIGH (no downsampling). */
    UiFramedTextButtonControl soundOptionsBackButton; /* +38E0 g_UiFramedTextButtonControlVtable: Sound options page button (action 0x1218, text 0x211F) returning to the in-game menu. */
    UiFocusProxyControl soundOptionsTitleLabel; /* +3940 g_UiFocusProxyControlVtable: Title text 0x213A of the in-game sound options page (menu page index 7). */
    UiTextButtonControl musicEnabledCheckbox; /* +399C g_UiTextButtonControlVtable: Toggle (action 0x1209, text 0x213B) enabling music (InGameAudioSettings_SetMusicEnabled). */
    UiTextButtonControl effectsEnabledCheckbox; /* +39FC g_UiTextButtonControlVtable: Toggle (action 0x1208, text 0x213C) enabling sound effects. */
    UiTextButtonControl reverseStereoCheckbox; /* +3A5C g_UiTextButtonControlVtable: Toggle (action 0x120A, text 0x213D) swapping the stereo channels. */
    UiFocusProxyControl effectsVolumeGroup; /* +3ABC g_UiFocusProxyControlVtable: Captioned group (text 0x213E) around the effects volume slider. */
    UiFocusProxyControl effectsVolumeMinLabel; /* +3B18 g_UiFocusProxyControlVtable: Left end caption (text 0x2141) of the effects volume slider. */
    UiFocusProxyControl effectsVolumeMaxLabel; /* +3B74 g_UiFocusProxyControlVtable: Right end caption (text 0x2142) of the effects volume slider. */
    UiRangeSliderControl effectsVolumeSlider; /* +3BD0 g_UiRangeSliderControlVtable: Slider (action 0x120B) writing soundEffectsGainQ15. */
    UiFocusProxyControl movieVolumeGroup; /* +3C38 g_UiFocusProxyControlVtable: Captioned group (text 0x213F) around the default movie audio volume slider. */
    UiFocusProxyControl movieVolumeMinLabel; /* +3C94 g_UiFocusProxyControlVtable: Left end caption (text 0x2141) of the movie volume slider. */
    UiFocusProxyControl movieVolumeMaxLabel; /* +3CF0 g_UiFocusProxyControlVtable: Right end caption (text 0x2142) of the movie volume slider. */
    UiRangeSliderControl movieVolumeSlider; /* +3D4C g_UiRangeSliderControlVtable: Slider (action 0x120C) writing movieDefaultAudioGainQ15. */
    UiFocusProxyControl musicVolumeGroup; /* +3DB4 g_UiFocusProxyControlVtable: Captioned group (text 0x2140) around the music volume slider. */
    UiFocusProxyControl musicVolumeMinLabel; /* +3E10 g_UiFocusProxyControlVtable: Left end caption (text 0x2141) of the music volume slider. */
    UiFocusProxyControl musicVolumeMaxLabel; /* +3E6C g_UiFocusProxyControlVtable: Right end caption (text 0x2142) of the music volume slider. */
    UiRangeSliderControl musicVolumeSlider; /* +3EC8 g_UiRangeSliderControlVtable: Slider (action 0x120D) writing musicGainQ15 to the looping music voice. */
    UiFocusProxyControl messageMovieVolumeGroup; /* +3F30 g_UiFocusProxyControlVtable: Captioned group (text 0x2143) around the alternate movie volume slider used by timed in-mission movie events. */
    UiFocusProxyControl messageMovieVolumeMinLabel; /* +3F8C g_UiFocusProxyControlVtable: Left end caption (text 0x2141) of the alternate movie volume slider. */
    UiFocusProxyControl messageMovieVolumeMaxLabel; /* +3FE8 g_UiFocusProxyControlVtable: Right end caption (text 0x2142) of the alternate movie volume slider; sits in 0x3F30's sibling chain although its parent link says 0x3DB4. */
    UiRangeSliderControl messageMovieVolumeSlider; /* +4044 g_UiRangeSliderControlVtable: Slider (action 0x121A) writing movieAlternateAudioGainQ15. */
    UiLayoutContainerControl<2> sidePanelStack; /* +40AC g_UiLayoutContainerControlVtable: Page stack for the right side panel frame: page 0 shows the side panel (game layout), page 1 is empty (editor layout, world view goes full width); toggled by a hotkey and settings bit 0x4. */
    UiImagePanelControl sidePanelFrameLeftEdge; /* +4104 g_UiImagePanelControlVtable: Side panel frame image (panel subresource 0), full-height left border strip; its left offset defines the side panel width. */
    UiImagePanelControl sidePanelFrameRightEdge; /* +4160 g_UiImagePanelControlVtable: Side panel frame image (panel subresource 1), full-height right border strip. */
    UiImagePanelControl sidePanelFrameTopCap; /* +41BC g_UiImagePanelControlVtable: Side panel frame image (panel subresource 2), top piece between the edge strips. */
    UiImagePanelControl sidePanelFrameMenuBar; /* +4218 g_UiImagePanelControlVtable: Side panel frame image (panel subresource 3) below the top cap; the menu/objectives buttons and countdown are laid out over it. */
    UiImagePanelControl sidePanelFrameInfoSection; /* +4274 g_UiImagePanelControlVtable: Side panel frame image (panel subresource 4), middle section whose rect positions the 0xB210..0xB590 controls. */
    UiImagePanelControl sidePanelFrameBottomCap; /* +42D0 g_UiImagePanelControlVtable: Side panel frame image (panel subresource 5), bottom piece of the side panel frame. */
    UiLayoutContainerControl<3> sidePanelMenuButtonStack; /* +432C g_UiLayoutContainerControlVtable: Two-page container (page 0 = menu button row, page 1 empty) holding the menu and objectives buttons and the countdown display. */
    UiSpriteButtonControl inGameMenuButton; /* +4388 g_UiSpriteButtonControlVtable: Sprite toggle (action 0x1003) that opens/closes the in-game menu page (InGameMissionHelpRootView.inGameMenuButton). */
    UiSpriteButtonControl missionObjectivesButton; /* +4400 g_UiSpriteButtonControlVtable: Sprite toggle (action 0x101F) that opens menu page 8 (mission briefing/objectives text panels) and pauses local play. */
    UiImagePanelControl countdownDisplayPanel; /* +4478 g_UiImagePanelControlVtable: Image panel (subresource 9) next to the menu buttons that hosts the countdown text. */
    UiFocusProxyControl countdownText; /* +44D4 g_UiFocusProxyControlVtable: Text control bound to g_InGameCountdownTextUtf16 (mission countdown). */
    UiLayoutContainerControl<3> resourceBarModeStack; /* +4530 g_UiLayoutContainerControlVtable: Page stack switched by the game/editor layout toggle: page 1 = resource panel (game), page 2 = editor tab strip A (editor). */
    UiImagePanelControl resourcePanel; /* +458C g_UiImagePanelControlVtable: Image panel (subresource 6) in the game layout with the Xenite/Tritium/Energy gauges and their icons. */
    UiImagePanelControl editorTabStripA; /* +45E8 g_UiImagePanelControlVtable: Image panel (subresource 0x26) shown in the editor layout; holds editor mode tabs G0..G2. */
    UiLayoutContainerControl<3> gamePanelsModeStack; /* +4644 g_UiLayoutContainerControlVtable: Page stack switched by the game/editor layout toggle: page 1 = game panels area, page 2 = editor tab strip B. */
    UiImagePanelControl gamePanelsArea; /* +46A0 g_UiImagePanelControlVtable: Image panel (subresource 7) in the game layout hosting the pop-up panels (diplomacy panel 0x4D0C, build catalog 0x5DB4, ...). */
    UiImagePanelControl editorTabStripB; /* +46FC g_UiImagePanelControlVtable: Image panel (subresource 0x27) shown in the editor layout; holds editor mode tabs G3..G5. */
    UiImageControl resourcePanelImageToggle8; /* +4758 g_UiImageControlVtable: Hover/selectable image control (image 8) on the resource panel with a nine-slice popup; exact role unknown, named from class and parent. */
    UiNineSlicePanelControl resourcePanelImageToggle8Popup; /* +47C4 g_UiNineSlicePanelControlVtable: Nine-slice panel parented to image control 0x4758 (popup background); role inferred from structure only. */
    UiImageControl resourcePanelImageToggle9; /* +4820 g_UiImageControlVtable: Hover/selectable image control (image 9) on the resource panel with a nine-slice popup; exact role unknown, named from class and parent. */
    UiNineSlicePanelControl resourcePanelImageToggle9Popup; /* +488C g_UiNineSlicePanelControlVtable: Nine-slice panel parented to image control 0x4820 (tooltip id 0x180005); role inferred from structure only. */
    uint32_t resourcePanelImageToggle9Popup_trailing[1]; /* +48E8: template dwords behind the control */
    UiSpriteButtonControl resourcePanelIconButton; /* +48EC g_UiSpriteButtonControlVtable: Sprite button (sprite 0xA, no action, tooltip 0x180011) on the resource panel; exact role unknown, named from class and parent. */
    UiFormattedContainer xeniteGauge; /* +4964 g_UiFormattedContainerVtable: Formatted value display for the primary resource Xenite (current/limit at root+0x49B4/0x49B8). */
    uint32_t xeniteGauge_trailing[1]; /* +49F8: template dwords behind the control */
    UiFormattedContainer tritiumGauge; /* +49FC g_UiFormattedContainerVtable: Formatted value display for the secondary resource Tritium (current/limit at root+0x4A4C/0x4A50). */
    uint32_t tritiumGauge_trailing[1]; /* +4A90: template dwords behind the control */
    UiFormattedContainer energyGauge; /* +4A94 g_UiFormattedContainerVtable: Two-value display for energy demand vs. generation capacity (root+0x4AE4/0x4AE8/0x4B28). */
    uint32_t energyGauge_trailing[7]; /* +4B28: template dwords behind the control */
    UiFocusProxyControl xeniteAmountText; /* +4B44 g_UiFocusProxyControlVtable: Text control bound to g_FrontendCurrentFactionPrimaryResourceTextUtf16 (formatted Xenite amount). */
    uint32_t xeniteAmountText_trailing[1]; /* +4BA0: template dwords behind the control */
    UiSpriteButtonControl editorModeTabTerrainHeight; /* +4BA4 g_UiSpriteButtonControlVtable: Editor mode tab G0 (action 0x1100, InGameCommandModeG_Select0): terrain height tool; syncs the three mode-G page stacks. */
    UiSpriteButtonControl editorModeTabTerrainMaterial; /* +4C1C g_UiSpriteButtonControlVtable: Editor mode tab G1 (action 0x1101, InGameCommandModeG_Select1): terrain material/texture palette tool. */
    UiSpriteButtonControl editorModeTabTerrainSmoothing; /* +4C94 g_UiSpriteButtonControlVtable: Editor mode tab G2 (action 0x1102, InGameCommandModeG_Select2): terrain relaxation/smoothing tool (role partly resolved). */
    UiImageControl diplomacyPanel; /* +4D0C g_UiImageControlVtable: Image-control window (image 0xC, InGameRuntimeRootUiGridView.diplomacyPanel) listing the other active players and relations. */
    UiNineSlicePanelControl diplomacyFrame; /* +4D78 g_UiNineSlicePanelControlVtable: Nine-slice frame of the diplomacy panel holding the seven player row stacks (rebuilt by InGameOtherPlayerCommand_RebuildTargetEntries). */
    UiLayoutContainerControl<2> diplomacyRow1; /* +4DD4 g_UiLayoutContainerControlVtable: Slot page stack for diplomacy row 1 (page 0 visible, page 1 hidden when fewer players). */
    UiLayoutContainerControl<2> diplomacyRow2; /* +4E2C g_UiLayoutContainerControlVtable: Slot page stack for diplomacy row 2 (page 0 visible, page 1 hidden when fewer players). */
    UiLayoutContainerControl<2> diplomacyRow3; /* +4E84 g_UiLayoutContainerControlVtable: Slot page stack for diplomacy row 3 (page 0 visible, page 1 hidden when fewer players). */
    UiLayoutContainerControl<2> diplomacyRow4; /* +4EDC g_UiLayoutContainerControlVtable: Slot page stack for diplomacy row 4 (page 0 visible, page 1 hidden when fewer players). */
    UiLayoutContainerControl<2> diplomacyRow5; /* +4F34 g_UiLayoutContainerControlVtable: Slot page stack for diplomacy row 5 (page 0 visible, page 1 hidden when fewer players). */
    UiLayoutContainerControl<2> diplomacyRow6; /* +4F8C g_UiLayoutContainerControlVtable: Slot page stack for diplomacy row 6 (page 0 visible, page 1 hidden when fewer players). */
    UiLayoutContainerControl<2> diplomacyRow7; /* +4FE4 g_UiLayoutContainerControlVtable: Slot page stack for diplomacy row 7 (page 0 visible, page 1 hidden when fewer players). */
    UiFocusProxyControl diplomacyRow1PlayerNumberLabel; /* +503C g_UiFocusProxyControlVtable: Player number text (0x2190 + faction index) of diplomacy row 1. */
    UiFocusProxyControl diplomacyRow2PlayerNumberLabel; /* +5098 g_UiFocusProxyControlVtable: Player number text of diplomacy row 2. */
    UiFocusProxyControl diplomacyRow3PlayerNumberLabel; /* +50F4 g_UiFocusProxyControlVtable: Player number text of diplomacy row 3. */
    UiFocusProxyControl diplomacyRow4PlayerNumberLabel; /* +5150 g_UiFocusProxyControlVtable: Player number text of diplomacy row 4. */
    UiFocusProxyControl diplomacyRow5PlayerNumberLabel; /* +51AC g_UiFocusProxyControlVtable: Player number text of diplomacy row 5. */
    UiFocusProxyControl diplomacyRow6PlayerNumberLabel; /* +5208 g_UiFocusProxyControlVtable: Player number text of diplomacy row 6. */
    UiFocusProxyControl diplomacyRow7PlayerNumberLabel; /* +5264 g_UiFocusProxyControlVtable: Player number text of diplomacy row 7. */
    UiFocusProxyControl diplomacyRow1FactionLabel; /* +52C0 g_UiFocusProxyControlVtable: Faction/race label text (0x2173 + faction record type) of diplomacy row 1. */
    UiFocusProxyControl diplomacyRow2FactionLabel; /* +531C g_UiFocusProxyControlVtable: Faction/race label text of diplomacy row 2. */
    UiFocusProxyControl diplomacyRow3FactionLabel; /* +5378 g_UiFocusProxyControlVtable: Faction/race label text of diplomacy row 3. */
    UiFocusProxyControl diplomacyRow4FactionLabel; /* +53D4 g_UiFocusProxyControlVtable: Faction/race label text of diplomacy row 4. */
    UiFocusProxyControl diplomacyRow5FactionLabel; /* +5430 g_UiFocusProxyControlVtable: Faction/race label text of diplomacy row 5. */
    UiFocusProxyControl diplomacyRow6FactionLabel; /* +548C g_UiFocusProxyControlVtable: Faction/race label text of diplomacy row 6. */
    UiFocusProxyControl diplomacyRow7FactionLabel; /* +54E8 g_UiFocusProxyControlVtable: Faction/race label text of diplomacy row 7. */
    UiFocusProxyControl diplomacyRow1RelationLabel; /* +5544 g_UiFocusProxyControlVtable: Diplomatic relation state text (0x21A3 + relation state) of diplomacy row 1. */
    UiFocusProxyControl diplomacyRow2RelationLabel; /* +55A0 g_UiFocusProxyControlVtable: Diplomatic relation state text of diplomacy row 2. */
    UiFocusProxyControl diplomacyRow3RelationLabel; /* +55FC g_UiFocusProxyControlVtable: Diplomatic relation state text of diplomacy row 3. */
    UiFocusProxyControl diplomacyRow4RelationLabel; /* +5658 g_UiFocusProxyControlVtable: Diplomatic relation state text of diplomacy row 4. */
    UiFocusProxyControl diplomacyRow5RelationLabel; /* +56B4 g_UiFocusProxyControlVtable: Diplomatic relation state text of diplomacy row 5. */
    UiFocusProxyControl diplomacyRow6RelationLabel; /* +5710 g_UiFocusProxyControlVtable: Diplomatic relation state text of diplomacy row 6. */
    UiFocusProxyControl diplomacyRow7RelationLabel; /* +576C g_UiFocusProxyControlVtable: Diplomatic relation state text of diplomacy row 7. */
    UiFocusProxyControl diplomacyRow1PlayerNameLabel; /* +57C8 g_UiFocusProxyControlVtable: Player name text (network player name, else empty) of diplomacy row 1. */
    UiFocusProxyControl diplomacyRow2PlayerNameLabel; /* +5824 g_UiFocusProxyControlVtable: Player name text of diplomacy row 2. */
    UiFocusProxyControl diplomacyRow3PlayerNameLabel; /* +5880 g_UiFocusProxyControlVtable: Player name text of diplomacy row 3. */
    UiFocusProxyControl diplomacyRow4PlayerNameLabel; /* +58DC g_UiFocusProxyControlVtable: Player-name text of diplomacy row 4 (g_UiAction1012IconImageOffsets[3]); set to the other player's network name or empty by InGameOtherPlayerCommand_RebuildTargetEntries. */
    UiFocusProxyControl diplomacyRow5PlayerNameLabel; /* +5938 g_UiFocusProxyControlVtable: Player-name text of diplomacy row 5 (g_UiAction1012IconImageOffsets[4]); set to the other player's network name or empty by InGameOtherPlayerCommand_RebuildTargetEntries. */
    UiFocusProxyControl diplomacyRow6PlayerNameLabel; /* +5994 g_UiFocusProxyControlVtable: Player-name text of diplomacy row 6 (g_UiAction1012IconImageOffsets[5]); set to the other player's network name or empty by InGameOtherPlayerCommand_RebuildTargetEntries. */
    UiFocusProxyControl diplomacyRow7PlayerNameLabel; /* +59F0 g_UiFocusProxyControlVtable: Player-name text of diplomacy row 7 (g_UiAction1012IconImageOffsets[6]); set to the other player's network name or empty by InGameOtherPlayerCommand_RebuildTargetEntries. */
    UiCommandSpriteButtonWithDetails diplomacyRow1RelationButton; /* +5A4C g_UiCommandSpriteButtonWithDetailsVtable: Action-0x1012 command sprite button of diplomacy row 1; shows the relation-state sprite and advances/resets the relation to that player (InGameOtherPlayerCommand_DispatchSelectedTarget). */
    UiCommandSpriteButtonWithDetails diplomacyRow2RelationButton; /* +5AC8 g_UiCommandSpriteButtonWithDetailsVtable: Action-0x1012 command sprite button of diplomacy row 2; shows the relation-state sprite and advances/resets the relation to that player (InGameOtherPlayerCommand_DispatchSelectedTarget). */
    UiCommandSpriteButtonWithDetails diplomacyRow3RelationButton; /* +5B44 g_UiCommandSpriteButtonWithDetailsVtable: Action-0x1012 command sprite button of diplomacy row 3; shows the relation-state sprite and advances/resets the relation to that player (InGameOtherPlayerCommand_DispatchSelectedTarget). */
    UiCommandSpriteButtonWithDetails diplomacyRow4RelationButton; /* +5BC0 g_UiCommandSpriteButtonWithDetailsVtable: Action-0x1012 command sprite button of diplomacy row 4; shows the relation-state sprite and advances/resets the relation to that player (InGameOtherPlayerCommand_DispatchSelectedTarget). */
    UiCommandSpriteButtonWithDetails diplomacyRow5RelationButton; /* +5C3C g_UiCommandSpriteButtonWithDetailsVtable: Action-0x1012 command sprite button of diplomacy row 5; shows the relation-state sprite and advances/resets the relation to that player (InGameOtherPlayerCommand_DispatchSelectedTarget). */
    UiCommandSpriteButtonWithDetails diplomacyRow6RelationButton; /* +5CB8 g_UiCommandSpriteButtonWithDetailsVtable: Action-0x1012 command sprite button of diplomacy row 6; shows the relation-state sprite and advances/resets the relation to that player (InGameOtherPlayerCommand_DispatchSelectedTarget). */
    UiCommandSpriteButtonWithDetails diplomacyRow7RelationButton; /* +5D34 g_UiCommandSpriteButtonWithDetailsVtable: Action-0x1012 command sprite button of diplomacy row 7; shows the relation-state sprite and advances/resets the relation to that player (InGameOtherPlayerCommand_DispatchSelectedTarget). */
    uint32_t diplomacyRow7RelationButton_trailing[1]; /* +5DB0: template dwords behind the control */
    UiImageControl buildCatalogPanel; /* +5DB4 g_UiImageControlVtable: Image-control window (id 0xD, InGameRuntimeRootUiGridView.buildCatalogPanel) holding the 48-entry build catalog of items the selected/owned production buildings can make; suppressed when empty (UiCatalogGroup48_RebuildGrid). */
    UiNineSlicePanelControl buildCatalogFrame; /* +5E20 g_UiNineSlicePanelControlVtable: Nine-slice frame of the 48-entry build catalog (InGameRuntimeRootUiGridView.buildCatalogFrame); resized to the computed grid in UiCatalogGroup48_RebuildGrid. */
    UiCatalogEntryControl buildCatalogEntry00; /* +5E7C g_UiCatalogEntryControlVtable: Catalog entry 0 of the 48-entry build catalog (action 0x100B: queue or, with modifier, cancel/refund the army asset g_UiCatalogGroup48Records[0] under column count 0). */
    UiCatalogEntryControl buildCatalogEntry01; /* +5EFC g_UiCatalogEntryControlVtable: Catalog entry 1 of the 48-entry build catalog (action 0x100B: queue or, with modifier, cancel/refund the army asset g_UiCatalogGroup48Records[1] under column count 0). */
    UiCatalogEntryControl buildCatalogEntry02; /* +5F7C g_UiCatalogEntryControlVtable: Catalog entry 2 of the 48-entry build catalog (action 0x100B: queue or, with modifier, cancel/refund the army asset g_UiCatalogGroup48Records[2] under column count 0). */
    UiCatalogEntryControl buildCatalogEntry03; /* +5FFC g_UiCatalogEntryControlVtable: Catalog entry 3 of the 48-entry build catalog (action 0x100B: queue or, with modifier, cancel/refund the army asset g_UiCatalogGroup48Records[3] under column count 0). */
    UiCatalogEntryControl buildCatalogEntry04; /* +607C g_UiCatalogEntryControlVtable: Catalog entry 4 of the 48-entry build catalog (action 0x100B: queue or, with modifier, cancel/refund the army asset g_UiCatalogGroup48Records[4] under column count 0). */
    UiCatalogEntryControl buildCatalogEntry05; /* +60FC g_UiCatalogEntryControlVtable: Catalog entry 5 of the 48-entry build catalog (action 0x100B: queue or, with modifier, cancel/refund the army asset g_UiCatalogGroup48Records[5] under column count 0). */
    UiCatalogEntryControl buildCatalogEntry06; /* +617C g_UiCatalogEntryControlVtable: Catalog entry 6 of the 48-entry build catalog (action 0x100B: queue or, with modifier, cancel/refund the army asset g_UiCatalogGroup48Records[6] under column count 0). */
    UiCatalogEntryControl buildCatalogEntry07; /* +61FC g_UiCatalogEntryControlVtable: Catalog entry 7 of the 48-entry build catalog (action 0x100B: queue or, with modifier, cancel/refund the army asset g_UiCatalogGroup48Records[7] under column count 0). */
    UiCatalogEntryControl buildCatalogEntry08; /* +627C g_UiCatalogEntryControlVtable: Catalog entry 8 of the 48-entry build catalog (action 0x100B: queue or, with modifier, cancel/refund the army asset g_UiCatalogGroup48Records[8] under column count 0). */
    UiCatalogEntryControl buildCatalogEntry09; /* +62FC g_UiCatalogEntryControlVtable: Catalog entry 9 of the 48-entry build catalog (action 0x100B: queue or, with modifier, cancel/refund the army asset g_UiCatalogGroup48Records[9] under column count 0). */
    UiCatalogEntryControl buildCatalogEntry10; /* +637C g_UiCatalogEntryControlVtable: Catalog entry 10 of the 48-entry build catalog (action 0x100B: queue or, with modifier, cancel/refund the army asset g_UiCatalogGroup48Records[10] under column count 0). */
    UiCatalogEntryControl buildCatalogEntry11; /* +63FC g_UiCatalogEntryControlVtable: Catalog entry 11 of the 48-entry build catalog (action 0x100B: queue or, with modifier, cancel/refund the army asset g_UiCatalogGroup48Records[11] under column count 0). */
    UiCatalogEntryControl buildCatalogEntry12; /* +647C g_UiCatalogEntryControlVtable: Catalog entry 12 of the 48-entry build catalog (action 0x100B: queue or, with modifier, cancel/refund the army asset g_UiCatalogGroup48Records[12] under column count 0). */
    UiCatalogEntryControl buildCatalogEntry13; /* +64FC g_UiCatalogEntryControlVtable: Catalog entry 13 of the 48-entry build catalog (action 0x100B: queue or, with modifier, cancel/refund the army asset g_UiCatalogGroup48Records[13] under column count 0). */
    UiCatalogEntryControl buildCatalogEntry14; /* +657C g_UiCatalogEntryControlVtable: Catalog entry 14 of the 48-entry build catalog (action 0x100B: queue or, with modifier, cancel/refund the army asset g_UiCatalogGroup48Records[14] under column count 0). */
    UiCatalogEntryControl buildCatalogEntry15; /* +65FC g_UiCatalogEntryControlVtable: Catalog entry 15 of the 48-entry build catalog (action 0x100B: queue or, with modifier, cancel/refund the army asset g_UiCatalogGroup48Records[15] under column count 0). */
    UiCatalogEntryControl buildCatalogEntry16; /* +667C g_UiCatalogEntryControlVtable: Catalog entry 16 of the 48-entry build catalog (action 0x100B: queue or, with modifier, cancel/refund the army asset g_UiCatalogGroup48Records[16] under column count 0). */
    UiCatalogEntryControl buildCatalogEntry17; /* +66FC g_UiCatalogEntryControlVtable: Catalog entry 17 of the 48-entry build catalog (action 0x100B: queue or, with modifier, cancel/refund the army asset g_UiCatalogGroup48Records[17] under column count 0). */
    UiCatalogEntryControl buildCatalogEntry18; /* +677C g_UiCatalogEntryControlVtable: Catalog entry 18 of the 48-entry build catalog (action 0x100B: queue or, with modifier, cancel/refund the army asset g_UiCatalogGroup48Records[18] under column count 0). */
    UiCatalogEntryControl buildCatalogEntry19; /* +67FC g_UiCatalogEntryControlVtable: Catalog entry 19 of the 48-entry build catalog (action 0x100B: queue or, with modifier, cancel/refund the army asset g_UiCatalogGroup48Records[19] under column count 0). */
    UiCatalogEntryControl buildCatalogEntry20; /* +687C g_UiCatalogEntryControlVtable: Catalog entry 20 of the 48-entry build catalog (action 0x100B: queue or, with modifier, cancel/refund the army asset g_UiCatalogGroup48Records[20] under column count 0). */
    UiCatalogEntryControl buildCatalogEntry21; /* +68FC g_UiCatalogEntryControlVtable: Catalog entry 21 of the 48-entry build catalog (action 0x100B: queue or, with modifier, cancel/refund the army asset g_UiCatalogGroup48Records[21] under column count 0). */
    UiCatalogEntryControl buildCatalogEntry22; /* +697C g_UiCatalogEntryControlVtable: Catalog entry 22 of the 48-entry build catalog (action 0x100B: queue or, with modifier, cancel/refund the army asset g_UiCatalogGroup48Records[22] under column count 0). */
    UiCatalogEntryControl buildCatalogEntry23; /* +69FC g_UiCatalogEntryControlVtable: Catalog entry 23 of the 48-entry build catalog (action 0x100B: queue or, with modifier, cancel/refund the army asset g_UiCatalogGroup48Records[23] under column count 0). */
    UiCatalogEntryControl buildCatalogEntry24; /* +6A7C g_UiCatalogEntryControlVtable: Catalog entry 24 of the 48-entry build catalog (action 0x100B: queue or, with modifier, cancel/refund the army asset g_UiCatalogGroup48Records[24] under column count 0). */
    UiCatalogEntryControl buildCatalogEntry25; /* +6AFC g_UiCatalogEntryControlVtable: Catalog entry 25 of the 48-entry build catalog (action 0x100B: queue or, with modifier, cancel/refund the army asset g_UiCatalogGroup48Records[25] under column count 0). */
    UiCatalogEntryControl buildCatalogEntry26; /* +6B7C g_UiCatalogEntryControlVtable: Catalog entry 26 of the 48-entry build catalog (action 0x100B: queue or, with modifier, cancel/refund the army asset g_UiCatalogGroup48Records[26] under column count 0). */
    UiCatalogEntryControl buildCatalogEntry27; /* +6BFC g_UiCatalogEntryControlVtable: Catalog entry 27 of the 48-entry build catalog (action 0x100B: queue or, with modifier, cancel/refund the army asset g_UiCatalogGroup48Records[27] under column count 0). */
    UiCatalogEntryControl buildCatalogEntry28; /* +6C7C g_UiCatalogEntryControlVtable: Catalog entry 28 of the 48-entry build catalog (action 0x100B: queue or, with modifier, cancel/refund the army asset g_UiCatalogGroup48Records[28] under column count 0). */
    UiCatalogEntryControl buildCatalogEntry29; /* +6CFC g_UiCatalogEntryControlVtable: Catalog entry 29 of the 48-entry build catalog (action 0x100B: queue or, with modifier, cancel/refund the army asset g_UiCatalogGroup48Records[29] under column count 0). */
    UiCatalogEntryControl buildCatalogEntry30; /* +6D7C g_UiCatalogEntryControlVtable: Catalog entry 30 of the 48-entry build catalog (action 0x100B: queue or, with modifier, cancel/refund the army asset g_UiCatalogGroup48Records[30] under column count 0). */
    UiCatalogEntryControl buildCatalogEntry31; /* +6DFC g_UiCatalogEntryControlVtable: Catalog entry 31 of the 48-entry build catalog (action 0x100B: queue or, with modifier, cancel/refund the army asset g_UiCatalogGroup48Records[31] under column count 0). */
    UiCatalogEntryControl buildCatalogEntry32; /* +6E7C g_UiCatalogEntryControlVtable: Catalog entry 32 of the 48-entry build catalog (action 0x100B: queue or, with modifier, cancel/refund the army asset g_UiCatalogGroup48Records[32] under column count 0). */
    UiCatalogEntryControl buildCatalogEntry33; /* +6EFC g_UiCatalogEntryControlVtable: Catalog entry 33 of the 48-entry build catalog (action 0x100B: queue or, with modifier, cancel/refund the army asset g_UiCatalogGroup48Records[33] under column count 0). */
    UiCatalogEntryControl buildCatalogEntry34; /* +6F7C g_UiCatalogEntryControlVtable: Catalog entry 34 of the 48-entry build catalog (action 0x100B: queue or, with modifier, cancel/refund the army asset g_UiCatalogGroup48Records[34] under column count 0). */
    UiCatalogEntryControl buildCatalogEntry35; /* +6FFC g_UiCatalogEntryControlVtable: Catalog entry 35 of the 48-entry build catalog (action 0x100B: queue or, with modifier, cancel/refund the army asset g_UiCatalogGroup48Records[35] under column count 0). */
    UiCatalogEntryControl buildCatalogEntry36; /* +707C g_UiCatalogEntryControlVtable: Catalog entry 36 of the 48-entry build catalog (action 0x100B: queue or, with modifier, cancel/refund the army asset g_UiCatalogGroup48Records[36] under column count 0). */
    UiCatalogEntryControl buildCatalogEntry37; /* +70FC g_UiCatalogEntryControlVtable: Catalog entry 37 of the 48-entry build catalog (action 0x100B: queue or, with modifier, cancel/refund the army asset g_UiCatalogGroup48Records[37] under column count 0). */
    UiCatalogEntryControl buildCatalogEntry38; /* +717C g_UiCatalogEntryControlVtable: Catalog entry 38 of the 48-entry build catalog (action 0x100B: queue or, with modifier, cancel/refund the army asset g_UiCatalogGroup48Records[38] under column count 0). */
    UiCatalogEntryControl buildCatalogEntry39; /* +71FC g_UiCatalogEntryControlVtable: Catalog entry 39 of the 48-entry build catalog (action 0x100B: queue or, with modifier, cancel/refund the army asset g_UiCatalogGroup48Records[39] under column count 0). */
    UiCatalogEntryControl buildCatalogEntry40; /* +727C g_UiCatalogEntryControlVtable: Catalog entry 40 of the 48-entry build catalog (action 0x100B: queue or, with modifier, cancel/refund the army asset g_UiCatalogGroup48Records[40] under column count 0). */
    UiCatalogEntryControl buildCatalogEntry41; /* +72FC g_UiCatalogEntryControlVtable: Catalog entry 41 of the 48-entry build catalog (action 0x100B: queue or, with modifier, cancel/refund the army asset g_UiCatalogGroup48Records[41] under column count 0). */
    UiCatalogEntryControl buildCatalogEntry42; /* +737C g_UiCatalogEntryControlVtable: Catalog entry 42 of the 48-entry build catalog (action 0x100B: queue or, with modifier, cancel/refund the army asset g_UiCatalogGroup48Records[42] under column count 0). */
    UiCatalogEntryControl buildCatalogEntry43; /* +73FC g_UiCatalogEntryControlVtable: Catalog entry 43 of the 48-entry build catalog (action 0x100B: queue or, with modifier, cancel/refund the army asset g_UiCatalogGroup48Records[43] under column count 0). */
    UiCatalogEntryControl buildCatalogEntry44; /* +747C g_UiCatalogEntryControlVtable: Catalog entry 44 of the 48-entry build catalog (action 0x100B: queue or, with modifier, cancel/refund the army asset g_UiCatalogGroup48Records[44] under column count 0). */
    UiCatalogEntryControl buildCatalogEntry45; /* +74FC g_UiCatalogEntryControlVtable: Catalog entry 45 of the 48-entry build catalog (action 0x100B: queue or, with modifier, cancel/refund the army asset g_UiCatalogGroup48Records[45] under column count 0). */
    UiCatalogEntryControl buildCatalogEntry46; /* +757C g_UiCatalogEntryControlVtable: Catalog entry 46 of the 48-entry build catalog (action 0x100B: queue or, with modifier, cancel/refund the army asset g_UiCatalogGroup48Records[46] under column count 0). */
    UiCatalogEntryControl buildCatalogEntry47; /* +75FC g_UiCatalogEntryControlVtable: Catalog entry 47 of the 48-entry build catalog (action 0x100B: queue or, with modifier, cancel/refund the army asset g_UiCatalogGroup48Records[47] under column count 0). */
    uint32_t buildCatalogEntry47_trailing[1]; /* +767C: template dwords behind the control */
    UiImageControl specialBuildCatalogPanel; /* +7680 g_UiImageControlVtable: Image-control window (id 0xE, InGameRuntimeRootUiGridView.specialBuildCatalogPanel) holding the 42-entry catalog of flag-0x10 army assets, only offered while the faction owns a model-class-0x0B structure (UiCatalogGroup42_RebuildGrid). */
    UiNineSlicePanelControl specialBuildCatalogFrame; /* +76EC g_UiNineSlicePanelControlVtable: Nine-slice frame of the 42-entry special build catalog (InGameRuntimeRootUiGridView.specialBuildCatalogFrame); resized to the computed grid. */
    UiCatalogEntryControl specialBuildCatalogEntry00; /* +7748 g_UiCatalogEntryControlVtable: Catalog entry 0 of the 42-entry special build catalog (action 0x100C: queue or cancel/refund g_UiCatalogGroup42Records[0]). */
    UiCatalogEntryControl specialBuildCatalogEntry01; /* +77C8 g_UiCatalogEntryControlVtable: Catalog entry 1 of the 42-entry special build catalog (action 0x100C: queue or cancel/refund g_UiCatalogGroup42Records[1]). */
    UiCatalogEntryControl specialBuildCatalogEntry02; /* +7848 g_UiCatalogEntryControlVtable: Catalog entry 2 of the 42-entry special build catalog (action 0x100C: queue or cancel/refund g_UiCatalogGroup42Records[2]). */
    UiCatalogEntryControl specialBuildCatalogEntry03; /* +78C8 g_UiCatalogEntryControlVtable: Catalog entry 3 of the 42-entry special build catalog (action 0x100C: queue or cancel/refund g_UiCatalogGroup42Records[3]). */
    UiCatalogEntryControl specialBuildCatalogEntry04; /* +7948 g_UiCatalogEntryControlVtable: Catalog entry 4 of the 42-entry special build catalog (action 0x100C: queue or cancel/refund g_UiCatalogGroup42Records[4]). */
    UiCatalogEntryControl specialBuildCatalogEntry05; /* +79C8 g_UiCatalogEntryControlVtable: Catalog entry 5 of the 42-entry special build catalog (action 0x100C: queue or cancel/refund g_UiCatalogGroup42Records[5]). */
    UiCatalogEntryControl specialBuildCatalogEntry06; /* +7A48 g_UiCatalogEntryControlVtable: Catalog entry 6 of the 42-entry special build catalog (action 0x100C: queue or cancel/refund g_UiCatalogGroup42Records[6]). */
    UiCatalogEntryControl specialBuildCatalogEntry07; /* +7AC8 g_UiCatalogEntryControlVtable: Catalog entry 7 of the 42-entry special build catalog (action 0x100C: queue or cancel/refund g_UiCatalogGroup42Records[7]). */
    UiCatalogEntryControl specialBuildCatalogEntry08; /* +7B48 g_UiCatalogEntryControlVtable: Catalog entry 8 of the 42-entry special build catalog (action 0x100C: queue or cancel/refund g_UiCatalogGroup42Records[8]). */
    UiCatalogEntryControl specialBuildCatalogEntry09; /* +7BC8 g_UiCatalogEntryControlVtable: Catalog entry 9 of the 42-entry special build catalog (action 0x100C: queue or cancel/refund g_UiCatalogGroup42Records[9]). */
    UiCatalogEntryControl specialBuildCatalogEntry10; /* +7C48 g_UiCatalogEntryControlVtable: Catalog entry 10 of the 42-entry special build catalog (action 0x100C: queue or cancel/refund g_UiCatalogGroup42Records[10]). */
    UiCatalogEntryControl specialBuildCatalogEntry11; /* +7CC8 g_UiCatalogEntryControlVtable: Catalog entry 11 of the 42-entry special build catalog (action 0x100C: queue or cancel/refund g_UiCatalogGroup42Records[11]). */
    UiCatalogEntryControl specialBuildCatalogEntry12; /* +7D48 g_UiCatalogEntryControlVtable: Catalog entry 12 of the 42-entry special build catalog (action 0x100C: queue or cancel/refund g_UiCatalogGroup42Records[12]). */
    UiCatalogEntryControl specialBuildCatalogEntry13; /* +7DC8 g_UiCatalogEntryControlVtable: Catalog entry 13 of the 42-entry special build catalog (action 0x100C: queue or cancel/refund g_UiCatalogGroup42Records[13]). */
    UiCatalogEntryControl specialBuildCatalogEntry14; /* +7E48 g_UiCatalogEntryControlVtable: Catalog entry 14 of the 42-entry special build catalog (action 0x100C: queue or cancel/refund g_UiCatalogGroup42Records[14]). */
    UiCatalogEntryControl specialBuildCatalogEntry15; /* +7EC8 g_UiCatalogEntryControlVtable: Catalog entry 15 of the 42-entry special build catalog (action 0x100C: queue or cancel/refund g_UiCatalogGroup42Records[15]). */
    UiCatalogEntryControl specialBuildCatalogEntry16; /* +7F48 g_UiCatalogEntryControlVtable: Catalog entry 16 of the 42-entry special build catalog (action 0x100C: queue or cancel/refund g_UiCatalogGroup42Records[16]). */
    UiCatalogEntryControl specialBuildCatalogEntry17; /* +7FC8 g_UiCatalogEntryControlVtable: Catalog entry 17 of the 42-entry special build catalog (action 0x100C: queue or cancel/refund g_UiCatalogGroup42Records[17]). */
    UiCatalogEntryControl specialBuildCatalogEntry18; /* +8048 g_UiCatalogEntryControlVtable: Catalog entry 18 of the 42-entry special build catalog (action 0x100C: queue or cancel/refund g_UiCatalogGroup42Records[18]). */
    UiCatalogEntryControl specialBuildCatalogEntry19; /* +80C8 g_UiCatalogEntryControlVtable: Catalog entry 19 of the 42-entry special build catalog (action 0x100C: queue or cancel/refund g_UiCatalogGroup42Records[19]). */
    UiCatalogEntryControl specialBuildCatalogEntry20; /* +8148 g_UiCatalogEntryControlVtable: Catalog entry 20 of the 42-entry special build catalog (action 0x100C: queue or cancel/refund g_UiCatalogGroup42Records[20]). */
    UiCatalogEntryControl specialBuildCatalogEntry21; /* +81C8 g_UiCatalogEntryControlVtable: Catalog entry 21 of the 42-entry special build catalog (action 0x100C: queue or cancel/refund g_UiCatalogGroup42Records[21]). */
    UiCatalogEntryControl specialBuildCatalogEntry22; /* +8248 g_UiCatalogEntryControlVtable: Catalog entry 22 of the 42-entry special build catalog (action 0x100C: queue or cancel/refund g_UiCatalogGroup42Records[22]). */
    UiCatalogEntryControl specialBuildCatalogEntry23; /* +82C8 g_UiCatalogEntryControlVtable: Catalog entry 23 of the 42-entry special build catalog (action 0x100C: queue or cancel/refund g_UiCatalogGroup42Records[23]). */
    UiCatalogEntryControl specialBuildCatalogEntry24; /* +8348 g_UiCatalogEntryControlVtable: Catalog entry 24 of the 42-entry special build catalog (action 0x100C: queue or cancel/refund g_UiCatalogGroup42Records[24]). */
    UiCatalogEntryControl specialBuildCatalogEntry25; /* +83C8 g_UiCatalogEntryControlVtable: Catalog entry 25 of the 42-entry special build catalog (action 0x100C: queue or cancel/refund g_UiCatalogGroup42Records[25]). */
    UiCatalogEntryControl specialBuildCatalogEntry26; /* +8448 g_UiCatalogEntryControlVtable: Catalog entry 26 of the 42-entry special build catalog (action 0x100C: queue or cancel/refund g_UiCatalogGroup42Records[26]). */
    UiCatalogEntryControl specialBuildCatalogEntry27; /* +84C8 g_UiCatalogEntryControlVtable: Catalog entry 27 of the 42-entry special build catalog (action 0x100C: queue or cancel/refund g_UiCatalogGroup42Records[27]). */
    UiCatalogEntryControl specialBuildCatalogEntry28; /* +8548 g_UiCatalogEntryControlVtable: Catalog entry 28 of the 42-entry special build catalog (action 0x100C: queue or cancel/refund g_UiCatalogGroup42Records[28]). */
    UiCatalogEntryControl specialBuildCatalogEntry29; /* +85C8 g_UiCatalogEntryControlVtable: Catalog entry 29 of the 42-entry special build catalog (action 0x100C: queue or cancel/refund g_UiCatalogGroup42Records[29]). */
    UiCatalogEntryControl specialBuildCatalogEntry30; /* +8648 g_UiCatalogEntryControlVtable: Catalog entry 30 of the 42-entry special build catalog (action 0x100C: queue or cancel/refund g_UiCatalogGroup42Records[30]). */
    UiCatalogEntryControl specialBuildCatalogEntry31; /* +86C8 g_UiCatalogEntryControlVtable: Catalog entry 31 of the 42-entry special build catalog (action 0x100C: queue or cancel/refund g_UiCatalogGroup42Records[31]). */
    UiCatalogEntryControl specialBuildCatalogEntry32; /* +8748 g_UiCatalogEntryControlVtable: Catalog entry 32 of the 42-entry special build catalog (action 0x100C: queue or cancel/refund g_UiCatalogGroup42Records[32]). */
    UiCatalogEntryControl specialBuildCatalogEntry33; /* +87C8 g_UiCatalogEntryControlVtable: Catalog entry 33 of the 42-entry special build catalog (action 0x100C: queue or cancel/refund g_UiCatalogGroup42Records[33]). */
    UiCatalogEntryControl specialBuildCatalogEntry34; /* +8848 g_UiCatalogEntryControlVtable: Catalog entry 34 of the 42-entry special build catalog (action 0x100C: queue or cancel/refund g_UiCatalogGroup42Records[34]). */
    UiCatalogEntryControl specialBuildCatalogEntry35; /* +88C8 g_UiCatalogEntryControlVtable: Catalog entry 35 of the 42-entry special build catalog (action 0x100C: queue or cancel/refund g_UiCatalogGroup42Records[35]). */
    UiCatalogEntryControl specialBuildCatalogEntry36; /* +8948 g_UiCatalogEntryControlVtable: Catalog entry 36 of the 42-entry special build catalog (action 0x100C: queue or cancel/refund g_UiCatalogGroup42Records[36]). */
    UiCatalogEntryControl specialBuildCatalogEntry37; /* +89C8 g_UiCatalogEntryControlVtable: Catalog entry 37 of the 42-entry special build catalog (action 0x100C: queue or cancel/refund g_UiCatalogGroup42Records[37]). */
    UiCatalogEntryControl specialBuildCatalogEntry38; /* +8A48 g_UiCatalogEntryControlVtable: Catalog entry 38 of the 42-entry special build catalog (action 0x100C: queue or cancel/refund g_UiCatalogGroup42Records[38]). */
    UiCatalogEntryControl specialBuildCatalogEntry39; /* +8AC8 g_UiCatalogEntryControlVtable: Catalog entry 39 of the 42-entry special build catalog (action 0x100C: queue or cancel/refund g_UiCatalogGroup42Records[39]). */
    UiCatalogEntryControl specialBuildCatalogEntry40; /* +8B48 g_UiCatalogEntryControlVtable: Catalog entry 40 of the 42-entry special build catalog (action 0x100C: queue or cancel/refund g_UiCatalogGroup42Records[40]). */
    UiCatalogEntryControl specialBuildCatalogEntry41; /* +8BC8 g_UiCatalogEntryControlVtable: Catalog entry 41 of the 42-entry special build catalog (action 0x100C: queue or cancel/refund g_UiCatalogGroup42Records[41]). */
    uint32_t specialBuildCatalogEntry41_trailing[1]; /* +8C48: template dwords behind the control */
    UiImageControl armyStockPanel; /* +8C4C g_UiImageControlVtable: Image-control window (id 0xF, InGameRuntimeRootUiGridView.armyStockPanel) showing up to 24 army assets held in the faction's primary army-asset pool (UiCommandSpriteVariantA_RebuildGrid). */
    UiNineSlicePanelControl armyStockFrame; /* +8CB8 g_UiNineSlicePanelControlVtable: Nine-slice frame of the 24-slot army stock grid (InGameRuntimeRootUiGridView.armyStockFrame); resized/suppressed by the rebuild. */
    UiCommandSpriteButtonWithDetails armyStockSlot00; /* +8D14 g_UiCommandSpriteButtonWithDetailsVtable: Army stock slot 0 (action 0x1001): click stages the pooled asset for deployment/transfer, modifier-click sells it for a 7/8 refund. */
    UiCommandSpriteButtonWithDetails armyStockSlot01; /* +8D90 g_UiCommandSpriteButtonWithDetailsVtable: Army stock slot 1 (action 0x1001): click stages the pooled asset for deployment/transfer, modifier-click sells it for a 7/8 refund. */
    UiCommandSpriteButtonWithDetails armyStockSlot02; /* +8E0C g_UiCommandSpriteButtonWithDetailsVtable: Army stock slot 2 (action 0x1001): click stages the pooled asset for deployment/transfer, modifier-click sells it for a 7/8 refund. */
    UiCommandSpriteButtonWithDetails armyStockSlot03; /* +8E88 g_UiCommandSpriteButtonWithDetailsVtable: Army stock slot 3 (action 0x1001): click stages the pooled asset for deployment/transfer, modifier-click sells it for a 7/8 refund. */
    UiCommandSpriteButtonWithDetails armyStockSlot04; /* +8F04 g_UiCommandSpriteButtonWithDetailsVtable: Army stock slot 4 (action 0x1001): click stages the pooled asset for deployment/transfer, modifier-click sells it for a 7/8 refund. */
    UiCommandSpriteButtonWithDetails armyStockSlot05; /* +8F80 g_UiCommandSpriteButtonWithDetailsVtable: Army stock slot 5 (action 0x1001): click stages the pooled asset for deployment/transfer, modifier-click sells it for a 7/8 refund. */
    UiCommandSpriteButtonWithDetails armyStockSlot06; /* +8FFC g_UiCommandSpriteButtonWithDetailsVtable: Slot 6 of the 24-slot faction asset depot grid (UiCommandSpriteVariantA, action 0x1001): shows a purchased army asset; click takes it for placement, other button sells it for 7/8 refund. */
    UiCommandSpriteButtonWithDetails armyStockSlot07; /* +9078 g_UiCommandSpriteButtonWithDetailsVtable: Slot 7 of the 24-slot faction asset depot grid (UiCommandSpriteVariantA, action 0x1001): shows a purchased army asset; click takes it for placement, other button sells it for 7/8 refund. */
    UiCommandSpriteButtonWithDetails armyStockSlot08; /* +90F4 g_UiCommandSpriteButtonWithDetailsVtable: Slot 8 of the 24-slot faction asset depot grid (UiCommandSpriteVariantA, action 0x1001): shows a purchased army asset; click takes it for placement, other button sells it for 7/8 refund. */
    UiCommandSpriteButtonWithDetails armyStockSlot09; /* +9170 g_UiCommandSpriteButtonWithDetailsVtable: Slot 9 of the 24-slot faction asset depot grid (UiCommandSpriteVariantA, action 0x1001): shows a purchased army asset; click takes it for placement, other button sells it for 7/8 refund. */
    UiCommandSpriteButtonWithDetails armyStockSlot10; /* +91EC g_UiCommandSpriteButtonWithDetailsVtable: Slot 10 of the 24-slot faction asset depot grid (UiCommandSpriteVariantA, action 0x1001): shows a purchased army asset; click takes it for placement, other button sells it for 7/8 refund. */
    UiCommandSpriteButtonWithDetails armyStockSlot11; /* +9268 g_UiCommandSpriteButtonWithDetailsVtable: Slot 11 of the 24-slot faction asset depot grid (UiCommandSpriteVariantA, action 0x1001): shows a purchased army asset; click takes it for placement, other button sells it for 7/8 refund. */
    UiCommandSpriteButtonWithDetails armyStockSlot12; /* +92E4 g_UiCommandSpriteButtonWithDetailsVtable: Slot 12 of the 24-slot faction asset depot grid (UiCommandSpriteVariantA, action 0x1001): shows a purchased army asset; click takes it for placement, other button sells it for 7/8 refund. */
    UiCommandSpriteButtonWithDetails armyStockSlot13; /* +9360 g_UiCommandSpriteButtonWithDetailsVtable: Slot 13 of the 24-slot faction asset depot grid (UiCommandSpriteVariantA, action 0x1001): shows a purchased army asset; click takes it for placement, other button sells it for 7/8 refund. */
    UiCommandSpriteButtonWithDetails armyStockSlot14; /* +93DC g_UiCommandSpriteButtonWithDetailsVtable: Slot 14 of the 24-slot faction asset depot grid (UiCommandSpriteVariantA, action 0x1001): shows a purchased army asset; click takes it for placement, other button sells it for 7/8 refund. */
    UiCommandSpriteButtonWithDetails armyStockSlot15; /* +9458 g_UiCommandSpriteButtonWithDetailsVtable: Slot 15 of the 24-slot faction asset depot grid (UiCommandSpriteVariantA, action 0x1001): shows a purchased army asset; click takes it for placement, other button sells it for 7/8 refund. */
    UiCommandSpriteButtonWithDetails armyStockSlot16; /* +94D4 g_UiCommandSpriteButtonWithDetailsVtable: Slot 16 of the 24-slot faction asset depot grid (UiCommandSpriteVariantA, action 0x1001): shows a purchased army asset; click takes it for placement, other button sells it for 7/8 refund. */
    UiCommandSpriteButtonWithDetails armyStockSlot17; /* +9550 g_UiCommandSpriteButtonWithDetailsVtable: Slot 17 of the 24-slot faction asset depot grid (UiCommandSpriteVariantA, action 0x1001): shows a purchased army asset; click takes it for placement, other button sells it for 7/8 refund. */
    UiCommandSpriteButtonWithDetails armyStockSlot18; /* +95CC g_UiCommandSpriteButtonWithDetailsVtable: Slot 18 of the 24-slot faction asset depot grid (UiCommandSpriteVariantA, action 0x1001): shows a purchased army asset; click takes it for placement, other button sells it for 7/8 refund. */
    UiCommandSpriteButtonWithDetails armyStockSlot19; /* +9648 g_UiCommandSpriteButtonWithDetailsVtable: Slot 19 of the 24-slot faction asset depot grid (UiCommandSpriteVariantA, action 0x1001): shows a purchased army asset; click takes it for placement, other button sells it for 7/8 refund. */
    UiCommandSpriteButtonWithDetails armyStockSlot20; /* +96C4 g_UiCommandSpriteButtonWithDetailsVtable: Slot 20 of the 24-slot faction asset depot grid (UiCommandSpriteVariantA, action 0x1001): shows a purchased army asset; click takes it for placement, other button sells it for 7/8 refund. */
    UiCommandSpriteButtonWithDetails armyStockSlot21; /* +9740 g_UiCommandSpriteButtonWithDetailsVtable: Slot 21 of the 24-slot faction asset depot grid (UiCommandSpriteVariantA, action 0x1001): shows a purchased army asset; click takes it for placement, other button sells it for 7/8 refund. */
    UiCommandSpriteButtonWithDetails armyStockSlot22; /* +97BC g_UiCommandSpriteButtonWithDetailsVtable: Slot 22 of the 24-slot faction asset depot grid (UiCommandSpriteVariantA, action 0x1001): shows a purchased army asset; click takes it for placement, other button sells it for 7/8 refund. */
    UiCatalogEntryControl armyStockSlot23; /* +9838 g_UiCatalogEntryControlVtable: Slot 23 of the 24-slot faction asset depot grid (UiCommandSpriteVariantA, action 0x1001): shows a purchased army asset; click takes it for placement, other button sells it for 7/8 refund. Last slot; distinct class variant with extra text id 0x180019. */
    UiSpriteButtonControl editorModeTabRegion; /* +98B8 g_UiSpriteButtonControlVtable: Editor mode tab G5 (action 0x1104, InGameCommandModeG_Select5): tool with two F variants that insert/remove player-pair ranges on the field grid; exact role unresolved. */
    UiSpriteButtonControl editorModeTabUnitPlacement; /* +9930 g_UiSpriteButtonControlVtable: Editor mode tab G3 (action 0x1105): place faction-owned army assets (flag 0x100 without 0x200, owner faction cyclable). */
    UiNodeBase editorModeTabObjectPlacement; /* +99A8 g_UiSpriteButtonControlVtable: Editor mode tab G4 (action 0x1106): place ownerless army assets with flags 0x100|0x200 (objects). */
    uint32_t editorModeTabObjectPlacement_fields[10];
    UiSelectionGeometryControl minimapView; /* +9A1C g_UiSelectionGeometryControlVtable: Rotating minimap (UiSelectionGeometryControl draws a rotated/scaled texture); +0x50 holds the cursor grid position (InGameRuntimeRoot.minimapOriginGridPosition; +0x58..+0x60 are minimapSampleScaleQ12, minimapRotationAngle, minimapTextureSource). */
    UiLayoutContainerControl<8> modePreviewPageStack; /* +9A8C g_UiLayoutContainerControlVtable: 8-page stack synced by mode G (primary index table): page 0 normal game, pages 1-5/7 editor tool previews. */
    UiNodeBase notificationTargetButton; /* +9AFC g_UiImageActionControlVtable: Page 0 of ModePreviewPageStack: image button; left click (0x100D) jumps to/resolves the active notification target, right click (0x100E) cancels and restores the view. */
    uint32_t notificationTargetButton_fields[6];
    UiImagePanelControl heightToolPreview; /* +9B60 g_UiImagePanelControlVtable: Page 1 (mode G0, terrain height tool) of ModePreviewPageStack; static image. */
    UiImagePanelControl materialToolSelectedSwatch; /* +9BBC g_UiImagePanelControlVtable: Page 2 (mode G1) of ModePreviewPageStack: shows the texture of the currently selected terrain material (set by UiCommandMatrix_SelectIndex). */
    UiImagePanelControl smoothingToolPreview; /* +9C18 g_UiImagePanelControlVtable: Page 3 (mode G2) of ModePreviewPageStack; static image; G2 role (relaxation/smoothing tool) only partly resolved. */
    UiFillPanelControl unitPlacementPreviewFrame; /* +9C74 g_UiFillPanelControlVtable: Page 4 (mode G3) of ModePreviewPageStack: black fill frame around the unit preview. */
    UiImagePanelControl unitPlacementPreviewImage; /* +9CD0 g_UiImagePanelControlVtable: Preview texture of the army asset currently chosen for unit placement (g_UiCommandModeGArmyAssetId). */
    UiFillPanelControl objectPlacementPreviewFrame; /* +9D2C g_UiFillPanelControlVtable: Page 5 (mode G4) of ModePreviewPageStack: black fill frame around the object preview. */
    UiImagePanelControl objectPlacementPreviewImage; /* +9D88 g_UiImagePanelControlVtable: Preview texture of the object asset currently chosen for placement (g_UiCommandMode4ArmyAssetId). */
    UiImagePanelControl regionToolPreview; /* +9DE4 g_UiImagePanelControlVtable: Page 7 (mode G5) of ModePreviewPageStack; image panel. */
    uint32_t regionToolPreview_trailing[40]; /* +9E40: template dwords behind the control */
    UiLayoutContainerControl<8> modeDetailPageStack; /* +9EE0 g_UiLayoutContainerControlVtable: 8-page stack synced by mode G (secondary index table): page 0 selection detail in normal game, other pages editor tool panels. */
    UiImagePanelControl selectionDetailPanel; /* +9F50 g_UiImagePanelControlVtable: Page 0 of ModeDetailPageStack: background image hosting the selection detail page stack. */
    UiLayoutContainerControl<4> selectionDetailPageStack; /* +9FAC g_UiLayoutContainerControlVtable: Page stack switched by InGameSelectionDetailPanel_Rebuild: 1 single unit, 2 multi-selection grid, 3 hovered build item (root field selectionDetailPageStack). */
    UiArmyMetricsPanel singleSelectionMetrics; /* +A00C g_UiArmyMetricsPanelVtable: Single-selection page: army metrics panel (portrait/health/value) of the one selected own entity. */
    UiListOffsetControl singleSelectionStatsText; /* +A06C g_UiListOffsetControlVtable: Single-selection page: text list with name/armour/energy/weapon lines of the selected entity. */
    uint32_t singleSelectionStatsText_trailing[1]; /* +A0C8: template dwords behind the control */
    UiNodeBase singleSelectionUpgradeButton; /* +A0CC g_UiSpriteButtonControlVtable: Single-selection page: action 0x1010 button toggling the technology page stack to page 2 for the unit; suppressed unless technologies are available. */
    uint32_t singleSelectionUpgradeButton_fields[10];
    UiImagePanelControl hoverItemIcon; /* +A140 g_UiImagePanelControlVtable: Hover page: icon texture of the hovered build/catalog record. */
    UiListOffsetControl hoverItemStatsText; /* +A19C g_UiListOffsetControlVtable: Hover page: text list with armour, Xenite cost, build time and energy of the hovered record. */
    UiArmyMetricsPanel multiSelectionCell00; /* +A1F8 g_UiArmyMetricsPanelVtable: Multi-selection page: grid cell 0 of 12 (g_InGameSelectionDetailGridCellOffsets) showing one selected entity's metrics. */
    UiArmyMetricsPanel multiSelectionCell01; /* +A258 g_UiArmyMetricsPanelVtable: Multi-selection page: grid cell 1 of 12 (g_InGameSelectionDetailGridCellOffsets) showing one selected entity's metrics. */
    UiArmyMetricsPanel multiSelectionCell02; /* +A2B8 g_UiArmyMetricsPanelVtable: Multi-selection page: grid cell 2 of 12 (g_InGameSelectionDetailGridCellOffsets) showing one selected entity's metrics. */
    UiArmyMetricsPanel multiSelectionCell03; /* +A318 g_UiArmyMetricsPanelVtable: Multi-selection page: grid cell 3 of 12 (g_InGameSelectionDetailGridCellOffsets) showing one selected entity's metrics. */
    UiArmyMetricsPanel multiSelectionCell04; /* +A378 g_UiArmyMetricsPanelVtable: Multi-selection page: grid cell 4 of 12 (g_InGameSelectionDetailGridCellOffsets) showing one selected entity's metrics. */
    UiArmyMetricsPanel multiSelectionCell05; /* +A3D8 g_UiArmyMetricsPanelVtable: Multi-selection page: grid cell 5 of 12 (g_InGameSelectionDetailGridCellOffsets) showing one selected entity's metrics. */
    UiArmyMetricsPanel multiSelectionCell06; /* +A438 g_UiArmyMetricsPanelVtable: Multi-selection page: grid cell 6 of 12 (g_InGameSelectionDetailGridCellOffsets) showing one selected entity's metrics. */
    UiArmyMetricsPanel multiSelectionCell07; /* +A498 g_UiArmyMetricsPanelVtable: Multi-selection page: grid cell 7 of 12 (g_InGameSelectionDetailGridCellOffsets) showing one selected entity's metrics. */
    UiArmyMetricsPanel multiSelectionCell08; /* +A4F8 g_UiArmyMetricsPanelVtable: Multi-selection page: grid cell 8 of 12 (g_InGameSelectionDetailGridCellOffsets) showing one selected entity's metrics. */
    UiArmyMetricsPanel multiSelectionCell09; /* +A558 g_UiArmyMetricsPanelVtable: Multi-selection page: grid cell 9 of 12 (g_InGameSelectionDetailGridCellOffsets) showing one selected entity's metrics. */
    UiArmyMetricsPanel multiSelectionCell10; /* +A5B8 g_UiArmyMetricsPanelVtable: Multi-selection page: grid cell 10 of 12 (g_InGameSelectionDetailGridCellOffsets) showing one selected entity's metrics. */
    UiArmyMetricsPanel multiSelectionCell11; /* +A618 g_UiArmyMetricsPanelVtable: Multi-selection page: grid cell 11 of 12 (g_InGameSelectionDetailGridCellOffsets) showing one selected entity's metrics. */
    UiImagePanelControl heightToolPanel; /* +A678 g_UiImagePanelControlVtable: Page 1 (mode G0) of ModeDetailPageStack; background image. */
    UiImagePanelControl materialPalettePanel; /* +A6D4 g_UiImagePanelControlVtable: Page 2 (mode G1) of ModeDetailPageStack: background of the 12-swatch terrain material palette (swatches follow as siblings). */
    UiImagePanelControl materialSwatch00; /* +A730 g_UiImagePanelControlVtable: Terrain material palette swatch 0 of 12 (3 columns); shows a material texture from the current scroll page. */
    UiNodeBase materialSwatch00Selector; /* +A78C g_UiFramedTextButtonControlVtable: Selectable frame on swatch 0 (action 0x1110, g_UiMappedCommandControlOffsets[0]); click selects that terrain material. */
    UiTextButtonTemplateFields materialSwatch00Selector_fields;
    UiImagePanelControl materialSwatch01; /* +A7E8 g_UiImagePanelControlVtable: Terrain material palette swatch 1 of 12 (3 columns); shows a material texture from the current scroll page. */
    UiNodeBase materialSwatch01Selector; /* +A844 g_UiFramedTextButtonControlVtable: Selectable frame on swatch 1 (action 0x1110, g_UiMappedCommandControlOffsets[1]); click selects that terrain material. */
    UiTextButtonTemplateFields materialSwatch01Selector_fields;
    UiImagePanelControl materialSwatch02; /* +A8A0 g_UiImagePanelControlVtable: Terrain material palette swatch 2 of 12 (3 columns); shows a material texture from the current scroll page. */
    UiNodeBase materialSwatch02Selector; /* +A8FC g_UiFramedTextButtonControlVtable: Selectable frame on swatch 2 (action 0x1110, g_UiMappedCommandControlOffsets[2]); click selects that terrain material. */
    UiTextButtonTemplateFields materialSwatch02Selector_fields;
    UiImagePanelControl materialSwatch03; /* +A958 g_UiImagePanelControlVtable: Terrain material palette swatch 3 of 12 (3 columns); shows a material texture from the current scroll page. */
    UiNodeBase materialSwatch03Selector; /* +A9B4 g_UiFramedTextButtonControlVtable: Selectable frame on swatch 3 (action 0x1110, g_UiMappedCommandControlOffsets[3]); click selects that terrain material. */
    UiTextButtonTemplateFields materialSwatch03Selector_fields;
    UiImagePanelControl materialSwatch04; /* +AA10 g_UiImagePanelControlVtable: Terrain material palette swatch 4 of 12 (3 columns); shows a material texture from the current scroll page. */
    UiNodeBase materialSwatch04Selector; /* +AA6C g_UiFramedTextButtonControlVtable: Selectable frame on swatch 4 (action 0x1110, g_UiMappedCommandControlOffsets[4]); click selects that terrain material. */
    UiTextButtonTemplateFields materialSwatch04Selector_fields;
    UiImagePanelControl materialSwatch05; /* +AAC8 g_UiImagePanelControlVtable: Terrain material palette swatch 5 of 12 (3 columns); shows a material texture from the current scroll page. */
    UiNodeBase materialSwatch05Selector; /* +AB24 g_UiFramedTextButtonControlVtable: Selectable frame on swatch 5 (action 0x1110, g_UiMappedCommandControlOffsets[5]); click selects that terrain material. */
    UiTextButtonTemplateFields materialSwatch05Selector_fields;
    UiImagePanelControl materialSwatch06; /* +AB80 g_UiImagePanelControlVtable: Terrain material palette swatch 6 of 12 (3 columns); shows a material texture from the current scroll page. */
    UiNodeBase materialSwatch06Selector; /* +ABDC g_UiFramedTextButtonControlVtable: Selectable frame on swatch 6 (action 0x1110, g_UiMappedCommandControlOffsets[6]); click selects that terrain material. */
    UiTextButtonTemplateFields materialSwatch06Selector_fields;
    UiImagePanelControl materialSwatch07; /* +AC38 g_UiImagePanelControlVtable: Terrain material palette swatch 7 of 12 (3 columns); shows a material texture from the current scroll page. */
    UiNodeBase materialSwatch07Selector; /* +AC94 g_UiFramedTextButtonControlVtable: Selectable frame on swatch 7 (action 0x1110, g_UiMappedCommandControlOffsets[7]); click selects that terrain material. */
    UiTextButtonTemplateFields materialSwatch07Selector_fields;
    UiImagePanelControl materialSwatch08; /* +ACF0 g_UiImagePanelControlVtable: Terrain material palette swatch 8 of 12 (3 columns); shows a material texture from the current scroll page. */
    UiNodeBase materialSwatch08Selector; /* +AD4C g_UiFramedTextButtonControlVtable: Selectable frame on swatch 8 (action 0x1110, g_UiMappedCommandControlOffsets[8]); click selects that terrain material. */
    UiTextButtonTemplateFields materialSwatch08Selector_fields;
    UiImagePanelControl materialSwatch09; /* +ADA8 g_UiImagePanelControlVtable: Terrain material palette swatch 9 of 12 (3 columns); shows a material texture from the current scroll page. */
    UiNodeBase materialSwatch09Selector; /* +AE04 g_UiFramedTextButtonControlVtable: Selectable frame on swatch 9 (action 0x1110, g_UiMappedCommandControlOffsets[9]); click selects that terrain material. */
    UiTextButtonTemplateFields materialSwatch09Selector_fields;
    UiImagePanelControl materialSwatch10; /* +AE60 g_UiImagePanelControlVtable: Terrain material palette swatch 10 of 12 (3 columns); shows a material texture from the current scroll page. */
    UiNodeBase materialSwatch10Selector; /* +AEBC g_UiFramedTextButtonControlVtable: Selectable frame on swatch 10 (action 0x1110, g_UiMappedCommandControlOffsets[10]); click selects that terrain material. */
    UiTextButtonTemplateFields materialSwatch10Selector_fields;
    UiImagePanelControl materialSwatch11; /* +AF18 g_UiImagePanelControlVtable: Terrain material palette swatch 11 of 12 (3 columns); shows a material texture from the current scroll page. */
    UiNodeBase materialSwatch11Selector; /* +AF74 g_UiFramedTextButtonControlVtable: Selectable frame on swatch 11 (action 0x1110, g_UiMappedCommandControlOffsets[11]); click selects that terrain material. */
    UiTextButtonTemplateFields materialSwatch11Selector_fields;
    UiImagePanelControl smoothingToolPanel; /* +AFD0 g_UiImagePanelControlVtable: Page 3 (mode G2) of ModeDetailPageStack; background image. */
    UiImagePanelControl unitPlacementPanel; /* +B02C g_UiImagePanelControlVtable: Page 4 (mode G3) of ModeDetailPageStack; background of the unit-placement info. */
    UiListOffsetControl unitPlacementStatsText; /* +B088 g_UiListOffsetControlVtable: Text list on UnitPlacementPanel with the stats of the unit chosen for placement. */
    UiImagePanelControl objectPlacementPanel; /* +B0E4 g_UiImagePanelControlVtable: Page 5 (mode G4) of ModeDetailPageStack; background image. */
    UiImagePanelControl regionToolPanel; /* +B140 g_UiImagePanelControlVtable: Page 7 (mode G5) of ModeDetailPageStack; background image. */
    UiLayoutContainerControl<9> modeCommandPageStack; /* +B19C g_UiLayoutContainerControlVtable: 8-page stack synced by mode G (tertiary index table): page 0 selection group buttons, other pages editor tool option buttons. */
    UiCommandSpriteButtonControl selectionGroupButton0; /* +B210 g_UiCommandSpriteButtonControlVtable: Selection group button 0 (action 0x100A, g_UiAction100AControlOffsets[0]): store/recall/jump to faction group 0 depending on modifiers/double-click. */
    uint32_t selectionGroupButton0_trailing[1]; /* +B28C: template dwords behind the control */
    UiCommandSpriteButtonControl selectionGroupButton1; /* +B290 g_UiCommandSpriteButtonControlVtable: Selection group button 1 (action 0x100A, g_UiAction100AControlOffsets[1]): store/recall/jump to faction group 1 depending on modifiers/double-click. */
    uint32_t selectionGroupButton1_trailing[1]; /* +B30C: template dwords behind the control */
    UiCommandSpriteButtonControl selectionGroupButton2; /* +B310 g_UiCommandSpriteButtonControlVtable: Selection group button 2 (action 0x100A, g_UiAction100AControlOffsets[2]): store/recall/jump to faction group 2 depending on modifiers/double-click. */
    uint32_t selectionGroupButton2_trailing[1]; /* +B38C: template dwords behind the control */
    UiCommandSpriteButtonControl selectionGroupButton3; /* +B390 g_UiCommandSpriteButtonControlVtable: Selection group button 3 (action 0x100A, g_UiAction100AControlOffsets[3]): store/recall/jump to faction group 3 depending on modifiers/double-click. */
    uint32_t selectionGroupButton3_trailing[1]; /* +B40C: template dwords behind the control */
    UiCommandSpriteButtonControl selectionGroupButton4; /* +B410 g_UiCommandSpriteButtonControlVtable: Selection group button 4 (action 0x100A, g_UiAction100AControlOffsets[4]): store/recall/jump to faction group 4 depending on modifiers/double-click. */
    uint32_t selectionGroupButton4_trailing[1]; /* +B48C: template dwords behind the control */
    UiCommandSpriteButtonControl selectionGroupButton5; /* +B490 g_UiCommandSpriteButtonControlVtable: Selection group button 5 (action 0x100A, g_UiAction100AControlOffsets[5]): store/recall/jump to faction group 5 depending on modifiers/double-click. */
    uint32_t selectionGroupButton5_trailing[1]; /* +B50C: template dwords behind the control */
    UiCommandSpriteButtonControl selectionGroupButton6; /* +B510 g_UiCommandSpriteButtonControlVtable: Selection group button 6 (action 0x100A, g_UiAction100AControlOffsets[6]): store/recall/jump to faction group 6 depending on modifiers/double-click. */
    uint32_t selectionGroupButton6_trailing[1]; /* +B58C: template dwords behind the control */
    UiCommandSpriteButtonControl selectionGroupButton7; /* +B590 g_UiCommandSpriteButtonControlVtable: Selection group button 7 (action 0x100A, g_UiAction100AControlOffsets[7]): store/recall/jump to faction group 7 depending on modifiers/double-click. */
    uint32_t selectionGroupButton7_trailing[1]; /* +B60C: template dwords behind the control */
    UiSpriteButtonControl heightToolOption0; /* +B610 g_UiSpriteButtonControlVtable: Exclusive tool sub-mode button (action 0x1108) selecting mode C=0; exact option label unresolved. */
    UiSpriteButtonControl heightToolOption1; /* +B688 g_UiSpriteButtonControlVtable: Exclusive tool sub-mode button (action 0x1109) selecting mode C=1; exact option label unresolved. */
    UiSpriteButtonControl heightToolOption2; /* +B700 g_UiSpriteButtonControlVtable: Exclusive tool sub-mode button (action 0x110A) selecting mode C=2; exact option label unresolved. */
    UiSpriteButtonControl heightToolOption3; /* +B778 g_UiSpriteButtonControlVtable: Exclusive tool sub-mode button (action 0x110B) selecting mode C=3; exact option label unresolved. */
    UiSpriteButtonControl materialToolOption0; /* +B7F0 g_UiSpriteButtonControlVtable: Exclusive tool sub-mode button (action 0x110C) selecting mode D=0; exact option label unresolved. */
    UiSpriteButtonControl materialToolOption1; /* +B868 g_UiSpriteButtonControlVtable: Exclusive tool sub-mode button (action 0x110D) selecting mode D=1; exact option label unresolved. */
    UiSpriteButtonControl materialToolOption2; /* +B8E0 g_UiSpriteButtonControlVtable: Exclusive tool sub-mode button (action 0x110E) selecting mode D=2; exact option label unresolved. */
    UiSpriteButtonControl materialToolOption3; /* +B958 g_UiSpriteButtonControlVtable: Exclusive tool sub-mode button (action 0x110F) selecting mode D=3; exact option label unresolved. */
    UiSpriteButtonControl smoothingToolOption0; /* +B9D0 g_UiSpriteButtonControlVtable: Exclusive tool sub-mode button (action 0x1117) selecting mode E=0; exact option label unresolved. */
    UiSpriteButtonControl smoothingToolOption1; /* +BA48 g_UiSpriteButtonControlVtable: Exclusive tool sub-mode button (action 0x1118) selecting mode E=1; exact option label unresolved. */
    UiSpriteButtonControl smoothingToolOption2; /* +BAC0 g_UiSpriteButtonControlVtable: Exclusive tool sub-mode button (action 0x1119) selecting mode E=2; exact option label unresolved. */
    UiSpriteButtonControl smoothingRelaxGatedButton; /* +BB38 g_UiSpriteButtonControlVtable: Mode G2 page: action 0x111A runs terrain relaxation passes (length 0x80, sign-gated) over the field. */
    UiSpriteButtonControl smoothingRelaxLandButton; /* +BBB0 g_UiSpriteButtonControlVtable: Mode G2 page: action 0x111B runs terrain relaxation passes (length 0x80, ungated land tool). */
    UiSpriteButtonControl unitPlacementOption0; /* +BC28 g_UiSpriteButtonControlVtable: Exclusive tool sub-mode button (action 0x1111) selecting mode A=0; exact option label unresolved. */
    UiSpriteButtonControl unitPlacementOption2; /* +BCA0 g_UiSpriteButtonControlVtable: Exclusive tool sub-mode button (action 0x1113) selecting mode A=2; exact option label unresolved. */
    UiSpriteButtonControl unitPlacementOption1; /* +BD18 g_UiSpriteButtonControlVtable: Exclusive tool sub-mode button (action 0x1112) selecting mode A=1; exact option label unresolved. */
    UiSpriteButtonControl objectPlacementOption0; /* +BD90 g_UiSpriteButtonControlVtable: Exclusive tool sub-mode button (action 0x1114) selecting mode B=0; exact option label unresolved. */
    UiSpriteButtonControl objectPlacementOption2; /* +BE08 g_UiSpriteButtonControlVtable: Exclusive tool sub-mode button (action 0x1116) selecting mode B=2; exact option label unresolved. */
    UiSpriteButtonControl objectPlacementOption1; /* +BE80 g_UiSpriteButtonControlVtable: Exclusive tool sub-mode button (action 0x1115) selecting mode B=1; exact option label unresolved. */
    UiSpriteButtonControl regionToolOption0; /* +BEF8 g_UiSpriteButtonControlVtable: Exclusive tool sub-mode button (action 0x111C) selecting mode F=0; exact option label unresolved. */
    UiSpriteButtonControl regionToolOption1; /* +BF70 g_UiSpriteButtonControlVtable: Exclusive tool sub-mode button (action 0x111D) selecting mode F=1; exact option label unresolved. */
    uint32_t regionToolOption1_trailing[255]; /* +BFE8: template dwords behind the control */
};
/* The in-game UI image behind a root pointer (any pointer to the image start): the typed access to its nodes,
   e.g. &InGameUi_Image(rt)->chatInputTextEdit. One reinterpretation for all users. */
template <class T> inline InGameUiImage *InGameUi_Image(T *root)
{
  return reinterpret_cast<InGameUiImage *>(root);
}
/* The world view node of the in-game UI image behind root as the session's WorldRuntimeContext: the node is that
   context (its interaction state starts with the node's UiNodeBase). */
template <class T> inline WorldRuntimeContext *InGameUi_WorldRuntime(T *root)
{
  return FrontendModelPointerContext_AsWorldRuntime(&InGameUi_Image(root)->worldView);
}
/* The twelve metric cells of the multi-selection page in grid order (the members behind the byte offsets of
   g_InGameSelectionDetailGridCellOffsets). */
inline constexpr UiArmyMetricsPanel InGameUiImage::*const g_InGameSelectionDetailGridCells[12] = {
    &InGameUiImage::multiSelectionCell00, &InGameUiImage::multiSelectionCell01, &InGameUiImage::multiSelectionCell02,
    &InGameUiImage::multiSelectionCell03, &InGameUiImage::multiSelectionCell04, &InGameUiImage::multiSelectionCell05,
    &InGameUiImage::multiSelectionCell06, &InGameUiImage::multiSelectionCell07, &InGameUiImage::multiSelectionCell08,
    &InGameUiImage::multiSelectionCell09, &InGameUiImage::multiSelectionCell10, &InGameUiImage::multiSelectionCell11};
#pragma pack(pop)

#endif /* THANDOR_UI_INGAME_TYPES_H */

/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/include/thandor/ui/ingame/runtime.h
 * Reverse engineering by idkFoxes 2026
 */

#ifndef THANDOR_UI_INGAME_RUNTIME_H
#define THANDOR_UI_INGAME_RUNTIME_H

#include <thandor/generated/types.h>
#include <thandor/core/contracts.h>

/* Submodule: ui/ingame/runtime. */

/* Diplomacy panel texts (InGameOtherPlayerCommand_RebuildTargetEntries, see the diplomacyRow* labels in
   ui_templates.h): the player number is 0x2190 + faction index, the relation label 0x21A3 + the 4-bit
   relation state of the faction record's packedRelationStates. */
#define TEXT_ID_PLAYER_NUMBER_BASE 0x2190
#define TEXT_ID_DIPLOMATIC_RELATION_BASE 0x21A3

/* Pages of the in-game window page stack (InGameUiImage.gameWindowPageStack) */
#define INGAME_WINDOW_PAGE_NONE 0 /* no window open, the world view is shown */
#define INGAME_WINDOW_PAGE_TECHNOLOGY 2
#define INGAME_WINDOW_PAGE_GAME_MENU 3
#define INGAME_WINDOW_PAGE_QUIT_MENU 4
#define INGAME_WINDOW_PAGE_SAVE_GAME 5
#define INGAME_WINDOW_PAGE_GRAPHICS_SETTINGS 6
#define INGAME_WINDOW_PAGE_SOUND_SETTINGS 7
#define INGAME_WINDOW_PAGE_MISSION_HELP 8

/* Info texts the world view cycles through with Ctrl+I (worldViewCyclingInfoText holds the text resource id) */
#define TEXT_ID_WORLD_VIEW_INFO_FIRST 0x112
#define TEXT_ID_WORLD_VIEW_INFO_LAST 0x117
/* Step of the editor's light direction and field origin hotkeys (Ctrl/Shift + arrow keys) */
#define EDITOR_ADJUST_STEP 0x400
/* Selection detail panel text templates (patched by InGameUiRuntime_InitializeControlTreeResources, chosen by
   InGameSelectionDetailPanel_Rebuild): 0x18002C.. single selection + the asset's template variant, 0x18003C..
   the same while researching, 0x180045.. hover/placement stats; 0x18004E fills an unused weapon slot. Model
   names are TEXT_ID_MODEL_NAME_BASE (ui/ingame/technology.h) + name index. */
#define TEXT_ID_SELECTION_DETAIL_TEMPLATE_BASE 0x18002C
#define TEXT_ID_SELECTION_DETAIL_RESEARCH_TEMPLATE_BASE 0x18003C
#define TEXT_ID_SELECTION_DETAIL_HOVER_TEMPLATE_BASE 0x180045
#define TEXT_ID_SELECTION_DETAIL_NO_WEAPON 0x18004E
/* Faction status lines of the HUD (InGameHud_UpdateStatusCountersAndSessionPrompts): template with the faction
   name (selector 0), the roster (1) and the score (2); the roster text with the player list (selector 0), or
   the text used without players */
#define TEXT_ID_FACTION_STATUS_TEMPLATE 0x21D2
#define TEXT_ID_FACTION_ROSTER_TEMPLATE 0x21D3
#define TEXT_ID_FACTION_NO_ROSTER 0x21D4
/* Labels of the message window's recipient check boxes, one per active faction (selector 0 = faction name) */
#define TEXT_ID_MESSAGE_RECIPIENT_LABEL_BASE 0x216D
/* Player status lines of a network game (InGamePanel_RebuildPlayerStatusRows), by readyOrWaitState zero or not;
   selector 0 = player name */
#define TEXT_ID_PLAYER_STATUS_STATE_ZERO 0xFF05
#define TEXT_ID_PLAYER_STATUS_STATE_SET 0xFF06
/* Mission help text: TEXT_ID_LEVEL_DESCRIPTION_BASE + 7 + TEXT_ID_LEVEL_DESCRIPTION_STRIDE * level title index +
   active faction (InGameMissionHelpPage_Toggle) */
#define TEXT_ID_MISSION_HELP_BASE 0x230017
/* Slots of the in-game notification queue (notificationQueue, InGameNotificationQueue_InsertPriorityRecord) */
#define INGAME_NOTIFICATION_QUEUE_SLOTS 4

/* The 0x200-byte header at the start of a save-game package, patched by InGameSaveGame_WritePackage
   after the entries are written (the save path's directory is split off behind the header, at +0x200). */
typedef struct InGameSavePackageHeader {
    uint8_t reserved000_0FF[0x100];
    uint16_t saveNameUtf16[0x38]; /* +0x100 file name of the save path */
    uint32_t levelTitleTextId; /* +0x170 */
    uint8_t reserved174_18F[0x1C];
    uint32_t campaignIndex; /* +0x190 -1 without campaign */
    uint8_t reserved194_1BF[0x2C];
    uint16_t dateTimeTextUtf16[0x18]; /* +0x1C0 "date, time" */
    uint32_t packedDate; /* +0x1F0 */
    uint32_t packedTime; /* +0x1F4 */
    uint8_t reserved1F8_1FF[8];
} InGameSavePackageHeader;

/* Functions are grouped by semantic ownership; address comments are executable virtual addresses. */

/* 0x0056E3C0 */
void InGameUiRootKeyboardFallback_DispatchCommandByCodeAndModifierFlags
          (uint32_t keyboardStateMask,uint32_t keyboardEventCode,UiRootNode *uiRoot);

/* 0x0056BAD0 */
void InGameSevenSlotCommand_SubmitAndClosePage(UiNodeBase *source);

/* 0x0056A610 */
void InGameChatInput_SendLineOrCheckCheatPhrase(InGameCommandTextEntryPageTextEditPtr commandTextEdit);

/* 0x0050ECE0 */
bool InGameSaveGame_WritePackage(void *worldView,void *savePath); /* CF: true = failed */

/* 0x0053D9F0 */
void InGameMapAction_RecenterViewFromGridCoordinates(InGameMapViewControlAddress32 mapControl);

/* 0x0055C990 */
StatusResult InGameUiRuntime_InitializeControlTreeResources(UiRootNode *inGameRoot);

/* 0x00563BD0 */
void InGameHud_UpdateStatusCountersAndSessionPrompts(void);

/* 0x005640E0 */
void InGamePanel_RebuildPlayerStatusRows(void *inGameRoot);

/* 0x005678C0 */
void InGameUiRuntime_DispatchCommandByCodeAndModifierFlags(UiKeyboardStateMask modifierFlags,UiActionId commandCode,
          WorldRuntimeContext *world);

/* 0x00569750 */
void InGameUiRuntime_ResetNotificationButtonCursor(void *worldView);

/* 0x00569780 */
void InGameUiRuntime_DispatchWorldContextActionCallback(WorldRuntimeContext *world);

/* 0x00569890 */
void InGameNotificationQueue_InsertPriorityRecord(InGameNotificationPayloadKind payloadKind,uint32_t payloadReserved,
          uint32_t orientationValue,AngleTurn32 orientationAngle,
          Q12 secondaryWorldCoordinateQ12,Q12 primaryWorldCoordinateQ12,
          InGameNotificationPriority priority,InGameNotificationMovieId notificationMovieId);

/* 0x00569B00 */
void InGameOtherPlayerCommand_RebuildTargetEntries(UiNodeBase *node);

/* 0x0056A460 */
uint32_t InGameMusic_ComputeTrackSuitabilityScore(MusicTrackClassId trackClassId,WorldRuntimeContext *worldRuntime);

/* 0x0056A8A0 */
void InGameMissionHelpPage_Toggle(UiNodeBase *source);

/* 0x0056AD00 */
void InGameResultsScreen_SelectChartTab(UiSelectableControl *selectableControl);

/* 0x0056AD80 */
void InGameOtherPlayerCommand_DispatchSelectedTarget(UiCommandSpriteButtonControl *control);

/* 0x0056B520 */
void InGameTechnologyPanel_ToggleForSelection(UiNodeBase *source);

/* 0x0056B5D0 */
void InGameSelectionPage_RebuildActivePlayerEntries(UiNodeBase *source);

/* 0x0056B6E0 */
void InGameSelectionPage_RebuildRuntimeRecordEntries(UiNodeBase *source);

/* 0x0056B7E0 */
void InGameSelectionPage_ShowSubpage1(UiNodeBase *source);

/* 0x0056D500 */
void InGameRecentText_TrimHistoryToThree(RecentTextHistoryView *historyView);

/* 0x0056F7F0 */
uint32_t InGameUiCommand_ResolveCursorCodeByMode
                (UiPointerRegionCode pointerRegionCode,Q12 pointerWorldXQ12,Q12 pointerWorldYQ12,
                uint32_t reservedArg3,ArmyRuntimeSlot *armyRuntimeUnderPointer,
                WorldRuntimeContext *worldRuntime);

/* 0x0056FA70 */
void InGameUiCommand_BeginInteractionByMode
          (UiPointerRegionCode pointerRegionCode,Q12 pointerX,Q12 pointerY,uint32_t reservedArg3,
          ArmyRuntimeSlot *armyRuntimeUnderPointer,WorldRuntimeExtendedMapControlView *mapControl
          );

/* 0x005703D0 */
void InGameUiCommand_UpdateInteractionByMode(UiPointerRegionCode pointerRegionCode,GraphicsScreenCoordinate pointerX,
          GraphicsScreenCoordinate pointerY,uint32_t reservedArg3,int optionalContext,
          WorldRuntimeExtendedMapControlView *mapControl);

/* 0x00570D60 */
void InGameUiCommand_EndInteractionByMode
          (uint32_t callbackArg0,uint32_t callbackArg1,uint32_t callbackArg2,uint32_t callbackArg3,
          WorldOwnerListNode *worldNode,WorldRuntimeContext *worldRuntime);

/* 0x00570F30 */
void InGameUiCommand_ResetInteractionByMode(WorldRuntimeContext *worldRuntime);

/* 0x005609F0 */
void InGameUiCommandRuntime_ApplyInteractionSubsystemActiveState
          (uint32_t playerRuntimeId,uint32_t unusedPayload1,uint32_t unusedPayload2,uint32_t activeStateFlags);

/* 0x005622F0 */
void InGameUiCommand_SaveFieldAndLevelAssetImages
          (uint32_t playerRuntimeId,uint32_t unusedPayload1,uint32_t unusedPayload2,uint32_t unusedPayload3);

/* 0x00567040 */
void InGameRecentTextHistory_InsertAndRebuild8(uint16_t *text);

/* 0x0056B850 */
void InGameSevenSlotCommand_ClosePage(UiNodeBase *source);

/* 0x0056B890 */
void InGameSevenSlotCommand_SubmitTextAndSelectionMask(UiNodeBase *source);

/* 0x005669B0 */
void InGameSelectionDetailPanel_Rebuild(void);

#endif /* THANDOR_UI_INGAME_RUNTIME_H */

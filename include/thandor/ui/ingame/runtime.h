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

/* Functions are grouped by semantic ownership; address comments are executable virtual addresses. */

/* 0x0056E3C0 */
void InGameUiRootKeyboardFallback_DispatchCommandByCodeAndModifierFlags
          (uint32_t keyboardStateMask,uint32_t keyboardEventCode,UiRootNode *uiRoot);

/* 0x0056BAD0 */
void InGameSevenSlotCommand_SubmitAndClosePage(UiNodeBase *source);

/* 0x0056A610 */
void InGameUiAction1024_Handler(InGameCommandTextEntryPageTextEditPtr commandTextEdit);

/* 0x0050ECE0 */
bool InGameUiAction1210_ResourceRegistrationHelper(void *worldView,void *savePath); /* CF: true = failed */

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
void InGameUiRuntime_ClearTransientState1BCallback(void *worldView);

/* 0x00569780 */
void InGameUiRuntime_DispatchWorldContextActionCallback(WorldRuntimeContext *world);

/* 0x00569890 */
void InGameNotificationQueue_InsertPriorityRecord(InGameNotificationPayloadKind payloadKind,uint32_t payloadReserved10,
          uint32_t orientationOrPresentationValue0C,AngleTurn32 primaryOrientationAngle08,
          Q12 secondaryWorldCoordinateQ12_04,Q12 primaryWorldCoordinateQ12_00,
          InGameNotificationPriority priority,InGameNotificationMovieId notificationMovieId);

/* 0x00569B00 */
void InGameOtherPlayerCommand_RebuildTargetEntries(UiNodeBase *node);

/* 0x0056A460 */
uint32_t InGameMusic_ComputeTrackSuitabilityScore(MusicTrackClassId trackClassId,WorldRuntimeContext *worldRuntime);

/* 0x0056A8A0 */
void InGameUiAction101F_Handler(UiNodeBase *source);

/* 0x0056AD00 */
void InGameUiAction101C_Handler(UiSelectableControl *selectableControl);

/* 0x0056AD80 */
void InGameOtherPlayerCommand_DispatchSelectedTarget(UiCommandSpriteButtonControl *control);

/* 0x0056B520 */
void InGameSelectionPage_ToggleAndRefreshPage2(UiNodeBase *source);

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
          ArmyRuntimeSlot *armyRuntimeUnderPointer,WorldRuntimeExtendedMapControlView170 *mapControl
          );

/* 0x005703D0 */
void InGameUiCommand_UpdateInteractionByMode(UiPointerRegionCode pointerRegionCode,GraphicsScreenCoordinate pointerX,
          GraphicsScreenCoordinate pointerY,uint32_t reservedArg3,int optionalContext,
          WorldRuntimeExtendedMapControlView170 *mapControl);

/* 0x00570D60 */
void InGameUiCommand_EndInteractionByMode
          (uint32_t callbackArg0,uint32_t callbackArg1,uint32_t callbackArg2,uint32_t callbackArg3,
          WorldOwnerListNode100 *worldNode,WorldRuntimeContext *worldRuntime);

/* 0x00570F30 */
void InGameUiCommand_ResetInteractionByMode(WorldRuntimeContext *worldRuntime);

/* 0x005609F0 */
void InGameUiCommandRuntime_ApplyInteractionSubsystemActiveState
          (uint32_t playerRuntimeId,uint32_t payloadDword0C,uint32_t payloadDword08,uint32_t activeStateFlags);

/* 0x005622F0 */
void InGameUiCommand_SaveFieldAndLevelAssetImages
          (uint32_t playerRuntimeId,uint32_t payloadDword0C,uint32_t payloadDword08,uint32_t payloadDword04);

/* 0x00567040 */
void InGameRecentTextHistory_InsertAndRebuild8(uint16_t *text);

/* 0x0056B850 */
void InGameSevenSlotCommand_ClosePage(UiNodeBase *source);

/* 0x0056B890 */
void InGameSevenSlotCommand_SubmitTextAndSelectionMask(UiNodeBase *source);

/* 0x005669B0 */
void InGameSelectionDetailPanel_Rebuild(void);

#endif /* THANDOR_UI_INGAME_RUNTIME_H */

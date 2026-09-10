#ifndef THANDOR_UI_INGAME_RUNTIME_H
#define THANDOR_UI_INGAME_RUNTIME_H

#include <thandor/generated/v523_types.h>
#include <thandor/core/contracts.h>

/* Submodule: ui/ingame/runtime. */
/* Functions are grouped by semantic ownership; address comments are executable virtual addresses. */

/* 0x0056E3C0 */
void InGameUiRootKeyboardFallback_DispatchCommandByCodeAndModifierFlagsCf (dword keyboardStateMask,dword keyboardEventCode,UiRootNode *uiRoot);

/* 0x0056BAD0 */
void InGameSevenSlotCommand_SubmitAndClosePage(UiNodeBase *source);

/* 0x0056A610 */
void InGameUiAction1024_Handler(InGameCommandTextEntryPageTextEditPtr commandTextEdit);

/* 0x0050ECE0 */
void InGameUiAction1210_ResourceRegistrationHelper(void *runtimeBase,void *resourcePath);

/* 0x0053D9F0 */
undefined8 InGameMapAction_RecenterViewFromGridCoordinates(InGameMapViewControlAddress32 mapControl);

/* 0x0055C990 */
void InGameUiRuntime_InitializeControlTreeResourcesCf(UiRootNode *inGameRoot);

/* 0x00563BD0 */
void InGameHud_UpdateStatusCountersAndSessionPrompts(void);

/* 0x005640E0 */
void InGamePanel_RebuildPlayerStatusRows(void *uiState);

/* 0x005678C0 */
void InGameUiRuntime_DispatchCommandByCodeAndModifierFlagsCf (UiKeyboardStateMask modifierFlags,UiActionId commandCode, WorldRuntimeContext *inGameRuntime);

/* 0x00569750 */
void InGameUiRuntime_ClearTransientState1BCallback(void *context);

/* 0x00569780 */
void InGameUiRuntime_DispatchWorldContextActionCallback(WorldRuntimeContext *context);

/* 0x00569890 */
void InGameNotificationQueue_InsertPriorityRecord (InGameNotificationPayloadKind payloadKind,dword payloadReserved10, dword orientationOrPresentationValue0C,AngleTurn32 primaryOrientationAngle08, Q12 secondaryWorldCoordinateQ12_04,Q12 primaryWorldCoordinateQ12_00, InGameNotificationPriority priority,InGameNotificationMovieId notificationMovieId);

/* 0x00569B00 */
void InGameOtherPlayerCommand_RebuildTargetEntries(UiNodeBase *node);

/* 0x0056A460 */
dword InGameMusic_ComputeTrackSuitabilityScore (MusicTrackClassId trackClassId,WorldRuntimeContext *worldRuntime);

/* 0x0056A8A0 */
void InGameUiAction101F_Handler(void *source);

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
dword InGameUiCommand_ResolveCursorCodeByMode (UiPointerRegionCode pointerRegionCode,Q12 pointerWorldXQ12,Q12 pointerWorldYQ12, undefined4 param_4,ArmyRuntimeSlot *armyRuntimeUnderPointer, WorldRuntimeContext *worldRuntime);

/* 0x0056FA70 */
void InGameUiCommand_BeginInteractionByMode (UiPointerRegionCode pointerRegionCode,Q12 pointerX,Q12 pointerY,undefined4 param_4, ArmyRuntimeSlot *armyRuntimeUnderPointer, WorldRuntimeExtendedMapControlAddress32 mapControl);

/* 0x005703D0 */
undefined8 __fastcall InGameUiCommand_UpdateInteractionByMode (undefined4 param_1,undefined4 param_2,UiPointerRegionCode pointerRegionCode, GraphicsScreenCoordinate pointerX,GraphicsScreenCoordinate pointerY,undefined4 param_6, int param_7,WorldRuntimeExtendedMapControlAddress32 mapControl);

/* 0x00570D60 */
void InGameUiCommand_EndInteractionByMode (dword callbackArg0,dword callbackArg1,dword callbackArg2,dword callbackArg3, WorldRuntimeNode *worldNode,WorldRuntimeContext *worldRuntime);

/* 0x00570F30 */
void InGameUiCommand_ResetInteractionByMode(WorldRuntimeContext *worldRuntime);

/* 0x005609F0 */
void InGameUiCommandRuntime_ApplyInteractionSubsystemActiveState (dword commandArg0,dword commandArg1,dword commandArg2,dword commandArg3);

/* 0x005622F0 */
void InGameUiCommand_SaveFieldAndLevelAssetImages (dword commandArg0,dword commandArg1,dword commandArg2,dword commandArg3);

/* 0x00567040 */
word * __fastcall InGameRecentTextHistory_InsertAndRebuild8(void);

/* 0x0056B850 */
void InGameSevenSlotCommand_ClosePage(UiNodeBase *source);

/* 0x0056B890 */
void InGameSevenSlotCommand_SubmitTextAndSelectionMask(UiNodeBase *source);

/* 0x005669B0 */
void __cdecl InGameSelectionDetailPanel_Rebuild(void);

#endif /* THANDOR_UI_INGAME_RUNTIME_H */

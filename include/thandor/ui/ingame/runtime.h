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
/* Functions are grouped by semantic ownership; address comments are executable virtual addresses. */

/* 0x0056E3C0 */
void __thandor_void_preserve_eax_ecx_edx
InGameUiRootKeyboardFallback_DispatchCommandByCodeAndModifierFlagsCf
          (dword keyboardStateMask,dword keyboardEventCode,UiRootNode *uiRoot);

/* 0x0056BAD0 */
void InGameSevenSlotCommand_SubmitAndClosePage(UiNodeBase *source);

/* 0x0056A610 */
void __thandor_void_preserve_eax_ecx_edx
InGameUiAction1024_Handler(InGameCommandTextEntryPageTextEditPtr commandTextEdit);

/* 0x0050ECE0 */
void __thandor_void_preserve_eax_ecx_edx
InGameUiAction1210_ResourceRegistrationHelper(void *runtimeBase,void *resourcePath);

/* 0x0053D9F0 */
void __thandor_void_preserve_eax_ecx_edx
InGameMapAction_RecenterViewFromGridCoordinates(InGameMapViewControlAddress32 mapControl);

/* 0x0055C990 */
StatusValueEaxCf5 __thandor_eax_cf_preserve_ecx_edx
InGameUiRuntime_InitializeControlTreeResourcesCf(UiRootNode *inGameRoot);

/* 0x00563BD0 */
void __thandor_void_preserve_eax_ecx_edx InGameHud_UpdateStatusCountersAndSessionPrompts(void);

/* 0x005640E0 */
void __thandor_void_preserve_eax_ecx_edx InGamePanel_RebuildPlayerStatusRows(void *uiState);

/* 0x005678C0 */
void __thandor_void_preserve_eax_ecx_edx
InGameUiRuntime_DispatchCommandByCodeAndModifierFlagsCf
          (UiKeyboardStateMask modifierFlags,UiActionId commandCode,
          WorldRuntimeContext *inGameRuntime);

/* 0x00569750 */
void InGameUiRuntime_ClearTransientState1BCallback(void *context);

/* 0x00569780 */
void InGameUiRuntime_DispatchWorldContextActionCallback(WorldRuntimeContext *context);

/* 0x00569890 */
void __thandor_void_preserve_eax_ecx_edx
InGameNotificationQueue_InsertPriorityRecord
          (InGameNotificationPayloadKind payloadKind,dword payloadReserved10,
          dword orientationOrPresentationValue0C,AngleTurn32 primaryOrientationAngle08,
          Q12 secondaryWorldCoordinateQ12_04,Q12 primaryWorldCoordinateQ12_00,
          InGameNotificationPriority priority,InGameNotificationMovieId notificationMovieId);

/* 0x00569B00 */
void __thandor_void_preserve_eax_ecx_edx
InGameOtherPlayerCommand_RebuildTargetEntries(UiNodeBase *node);

/* 0x0056A460 */
dword __thandor_eax_preserve_ecx_edx
InGameMusic_ComputeTrackSuitabilityScore
          (MusicTrackClassId trackClassId,WorldRuntimeContext *worldRuntime);

/* 0x0056A8A0 */
void __thandor_void_preserve_eax_ecx_edx InGameUiAction101F_Handler(UiNodeBase *source);

/* 0x0056AD00 */
void __thandor_void_preserve_eax_ecx_edx
InGameUiAction101C_Handler(UiSelectableControl *selectableControl);

/* 0x0056AD80 */
void __thandor_void_preserve_eax_ecx_edx
InGameOtherPlayerCommand_DispatchSelectedTarget(UiCommandSpriteButtonControl *control);

/* 0x0056B520 */
void __thandor_void_preserve_eax_ecx_edx
InGameSelectionPage_ToggleAndRefreshPage2(UiNodeBase *source);

/* 0x0056B5D0 */
void __thandor_void_preserve_eax_ecx_edx
InGameSelectionPage_RebuildActivePlayerEntries(UiNodeBase *source);

/* 0x0056B6E0 */
void __thandor_void_preserve_eax_ecx_edx
InGameSelectionPage_RebuildRuntimeRecordEntries(UiNodeBase *source);

/* 0x0056B7E0 */
void __thandor_void_preserve_eax_ecx_edx InGameSelectionPage_ShowSubpage1(UiNodeBase *source);

/* 0x0056D500 */
void __thandor_void_preserve_eax_ecx
InGameRecentText_TrimHistoryToThree(RecentTextHistoryView *historyView);

/* 0x0056F7F0 */
dword InGameUiCommand_ResolveCursorCodeByMode
                (UiPointerRegionCode pointerRegionCode,Q12 pointerWorldXQ12,Q12 pointerWorldYQ12,
                dword reservedArg3,ArmyRuntimeSlot *armyRuntimeUnderPointer,
                WorldRuntimeContext *worldRuntime);

/* 0x0056FA70 */
void __thandor_void_preserve_eax_ecx_edx
InGameUiCommand_BeginInteractionByMode
          (UiPointerRegionCode pointerRegionCode,Q12 pointerX,Q12 pointerY,dword reservedArg3,
          ArmyRuntimeSlot *armyRuntimeUnderPointer,WorldRuntimeExtendedMapControlView170 *mapControl
          );

/* 0x005703D0 */
void __thandor_void_preserve_eax_ecx_edx
InGameUiCommand_UpdateInteractionByMode
          (UiPointerRegionCode pointerRegionCode,GraphicsScreenCoordinate pointerX,
          GraphicsScreenCoordinate pointerY,dword reservedArg3,int optionalContext,
          WorldRuntimeExtendedMapControlView170 *mapControl);

/* 0x00570D60 */
void __thandor_void_preserve_eax_ecx_edx
InGameUiCommand_EndInteractionByMode
          (dword callbackArg0,dword callbackArg1,dword callbackArg2,dword callbackArg3,
          WorldOwnerListNode100 *worldNode,WorldRuntimeContext *worldRuntime);

/* 0x00570F30 */
void __thandor_void_preserve_eax_ecx_edx
InGameUiCommand_ResetInteractionByMode(WorldRuntimeContext *worldRuntime);

/* 0x005609F0 */
void __thandor_void_preserve_eax_ecx_edx
InGameUiCommandRuntime_ApplyInteractionSubsystemActiveState
          (dword commandArg0,dword commandArg1,dword commandArg2,dword commandArg3);

/* 0x005622F0 */
void __thandor_preserve_eax
InGameUiCommand_SaveFieldAndLevelAssetImages
          (dword commandArg0,dword commandArg1,dword commandArg2,dword commandArg3);

/* 0x00567040 */
void __thandor_void_preserve_eax_ecx_edx InGameRecentTextHistory_InsertAndRebuild8(word *text);

/* 0x0056B850 */
void __thandor_preserve_eax InGameSevenSlotCommand_ClosePage(UiNodeBase *source);

/* 0x0056B890 */
void __thandor_void_preserve_eax_ecx_edx
InGameSevenSlotCommand_SubmitTextAndSelectionMask(UiNodeBase *source);

/* 0x005669B0 */
void __thandor_void_preserve_eax_ecx_edx InGameSelectionDetailPanel_Rebuild(void);

#endif /* THANDOR_UI_INGAME_RUNTIME_H */

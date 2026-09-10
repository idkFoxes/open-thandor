#ifndef THANDOR_UI_FRONTEND_RUNTIME_H
#define THANDOR_UI_FRONTEND_RUNTIME_H

#include <thandor/generated/v523_types.h>
#include <thandor/core/contracts.h>

/* Submodule: ui/frontend/runtime. */
/* Functions are grouped by semantic ownership; address comments are executable virtual addresses. */

/* 0x00546BD0 */
undefined8 __fastcall Frontend_MainLoop(undefined4 param_1,undefined4 param_2,RomRecordId param_3);

/* 0x0050C380 */
GraphicsCursorFrameIndex FrontendModelPointerContext_SelectBestModelHitTargetAndResolveAction (int pointerY,int pointerX,FrontendModelPointerContextRuntimeState118 *context);

/* 0x0050C5A0 */
void FrontendModelPointerContext_NonRightPress (UiPointerWheelDelta wheelDelta,UiPixelCoordinate pointerY,UiPixelCoordinate pointerX ,FrontendModelPointerContextRuntimeState118 *callbackContext);

/* 0x0050C610 */
void FrontendModelPointerContext_NonRightRelease (UiPointerWheelDelta wheelDelta,UiPixelCoordinate pointerY,UiPixelCoordinate pointerX ,FrontendModelPointerContextRuntimeState118 *callbackContext);

/* 0x0050C670 */
void FrontendModelPointerContext_NonRightDrag (UiPointerWheelDelta wheelDelta,UiPixelCoordinate pointerY,UiPixelCoordinate pointerX ,FrontendModelPointerContextRuntimeState118 *callbackContext);

/* 0x00549B40 */
void FrontendUiAction2044_Handler(UiNodeBase *factionControl);

/* 0x00549BC0 */
void FrontendUiAction2045_Handler(UiNodeBase *playerControl);

/* 0x00549C40 */
void FrontendUiAction2046_Handler(UiNodeBase *selectionRowControl);

/* 0x0050BB80 */
void FrontendModelPointerContext_Relocate (UiSerializedRelocationDelta relocationDelta,UiNodeBase *control);

/* 0x0050BC30 */
void FrontendModelPointerContext_Layout(WorldRuntimeContext *callbackContext);

/* 0x0050BC60 */
void FrontendModelPointerContext_DrawClipped (UiPixelCoordinate clipTop,UiPixelCoordinate clipLeft,UiPixelCoordinate clipBottom, UiPixelCoordinate clipRight,UiNodeBase *control);

/* 0x0050C6E0 */
void FrontendModelPointerContext_RightPress (UiPointerWheelDelta wheelDelta,UiPixelCoordinate pointerY,UiPixelCoordinate pointerX ,FrontendModelPointerContextRuntimeState118 *callbackContext);

/* 0x0050C730 */
void FrontendModelPointerContext_RightRelease (UiPointerWheelDelta wheelDelta,UiPixelCoordinate pointerY,UiPixelCoordinate pointerX ,FrontendModelPointerContextRuntimeState118 *callbackContext);

/* 0x0050CC80 */
void FrontendModelPointerContext_RightDrag (UiPointerWheelDelta wheelDelta,UiPixelCoordinate pointerY,UiPixelCoordinate pointerX ,WorldRuntimeContext *callbackContext);

/* 0x0050CED0 */
void FrontendModelPointerContext_PointerWheel (UiPointerWheelDelta wheelDelta,UiPixelCoordinate pointerY,UiPixelCoordinate pointerX ,WorldRuntimeContext *callbackContext);

/* 0x0050CF50 */
void FrontendModelPointerContext_KeyboardEventCf (UiKeyboardStateMask keyboardStateMask,UiKeyboardEventCode keyCode, UiNodeBase *control);

/* 0x0050CF90 */
void FrontendModelPointerContext_Tick(WorldRuntimeContext *callbackContext);

/* 0x00514E40 */
void FrontendRuntime_UpdateCurrentFactionMetricCache(void);

/* 0x00547620 */
void __cdecl FrontendRuntime_TimerCountdownTick(void);

/* 0x00547FB0 */
void __cdecl FrontendRuntime_IncrementActiveTickCounter(void);

/* 0x00548030 */
void FrontendRuntime_DispatchCommandByCodeAndModifierFlagsCf (UiKeyboardStateMask modifierFlags,UiActionId commandCode,void *frontendRuntime);

/* 0x00548700 */
undefined4 FrontendState_DispatchCode(FrontendStatusCode stateCode);

/* 0x00548910 */
dword FrontendRuntime_UpdatePointerContextAndSceneViewCf (dword pointerValue0,dword pointerValue1,dword pointerValue2,dword pointerValue3, void *pointedRecord,void *frontendRuntime);

/* 0x00548BE0 */
void FrontendRuntimeCallback5C_NoOp (dword argument1,dword argument2,dword argument3,dword argument4,dword argument5, dword argument6);

/* 0x00548BF0 */
void FrontendRuntimeCallback60_NoOp (dword argument1,dword argument2,dword argument3,dword argument4,dword argument5, dword argument6);

/* 0x00548C00 */
undefined4 FrontendRuntimeCallback64_DispatchRecord1350 (dword argument1,dword argument2,dword argument3,dword argument4, FrontendCallbackArgument5 argument5,dword argument6);

/* 0x00548C70 */
void FrontendRuntimeCallback68_DispatchRefresh1340(dword callbackArgument);

/* 0x00548CB0 */
word * __fastcall FrontendRecentTextHistory_InsertAndRebuild5(void);

/* 0x00549100 */
undefined4 FrontendCallback_ApplyGameSpeedOrDispatch02C0(dword callbackArgument);

/* 0x00549140 */
undefined4 FrontendCallback_ReleaseSelectedResourceOrDispatch0320(dword callbackArgument);

/* 0x00549180 */
undefined1 FrontendCallback_NoOpArg1(void *source);

/* 0x00549AB0 */
undefined4 FrontendCallback_ReturnToMainPageOrDispatch0DC0(dword callbackArgument);

/* 0x0054A5A0 */
undefined4 FrontendCallback_ReturnToMainPageOrDispatchState4(dword callbackArgument);

/* 0x0054A7D0 */
undefined4 FrontendCallback_ReturnToMainPageOrDispatch0DC0_Secondary(dword callbackArgument);

/* 0x0054AAD0 */
void FrontendUiAction2010_Handler(UiNodeBase *sourceNode);

/* 0x0054AB70 */
void FrontendUiAction2011_Handler(FrontendDisplaySettingsPageOptionState1010 *source);

/* 0x0054BA30 */
void FrontendUiAction202CTo2030_SharedHandler(UiNodeBase *sourceNode);

/* 0x0054D3F0 */
void FrontendUiAction200C_Handler(UiPointerListControl *sessionListControl);

/* 0x0054D460 */
undefined4 FrontendRecentText_TrimAndSortTopFive(UiNodeBase *source);

/* 0x0054D4A0 */
void FrontendUiAction200F_Handler(FrontendNetworkSetupPageBackendListPtr backendList);

/* 0x00565A30 */
void Frontend_PlaySelectedEndMovie(void);

/* 0x00546700 */
undefined8 __fastcall Frontend_Init(undefined4 param_1,undefined4 param_2,RomRecordId param_3);

/* 0x00547630 */
undefined8 Frontend_StateTick(void);

/* 0x00543B70 */
void FrontendMenu_BindSharedResources(void *frontendUiState);

/* 0x005445A0 */
void FrontendUiAction2044_IndexedSelectionHelper (FrontendIndexedSelectionArgument argument1,dword argument2,dword argument3, FrontendFactionAssignmentIndex selectionIndex);

/* 0x00544640 */
void FrontendUiAction2045_IndexedSelectionHelper (dword argument1,dword argument2,dword argument3, FrontendFactionAssignmentIndex selectionIndex);

/* 0x005446A0 */
void FrontendUiAction2046_IndexedSelectionHelper (FrontendIndexedSelectionArgument argument1,dword argument2,dword argument3, FrontendFactionAssignmentIndex selectionIndex);

/* 0x00546190 */
void __fastcall FrontendDebugOverlay_RefreshCountersAndWorldCoordinates(undefined4 param_1,undefined4 param_2);

/* 0x005474E0 */
undefined8 __cdecl FrontendRuntime_ShutdownAndReleaseResourcesRegs(void);

/* 0x0050AD90 */
qword FrontendModelPointerContext_FindBestEligibleModelHitTarget (int pointerY,int pointerX,FrontendModelPointerContextRuntimeState118 *context);

#endif /* THANDOR_UI_FRONTEND_RUNTIME_H */

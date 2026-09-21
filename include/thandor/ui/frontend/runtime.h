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

/* 0x00546BD0 */
FrontendMainLoopEaxCf5 __thandor_eax_cf_preserve_ecx_edx
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
FrontendModelPointerContext_KeyboardEventCf
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
void __cdecl FrontendRuntime_IncrementActiveTickCounter(void);

/* 0x00548030 */
bool __thandor_cf_preserve_eax_ecx_edx
FrontendRuntime_DispatchCommandByCodeAndModifierFlagsCf
          (UiKeyboardStateMask modifierFlags,UiActionId commandCode,void *frontendRuntime);

/* 0x00548700 */
void __thandor_void_preserve_eax_ecx_edx FrontendState_DispatchCode(FrontendStatusCode stateCode);

/* 0x00548910 */
dword __thandor_eax_preserve_ecx_edx
FrontendRuntime_UpdatePointerContextAndSceneViewCf
          (dword pointerValue0,dword pointerValue1,dword pointerValue2,dword pointerValue3,
          void *pointedRecord,FrontendPointerSceneRuntimeView43E8 *frontendRuntime);

/* 0x00548BE0 */
void FrontendRuntimeCallback5C_NoOp (dword argument1,dword argument2,dword argument3,dword argument4,dword argument5, dword argument6);

/* 0x00548BF0 */
void FrontendRuntimeCallback60_NoOp (dword argument1,dword argument2,dword argument3,dword argument4,dword argument5, dword argument6);

/* 0x00548C00 */
void __thandor_void_preserve_eax_ecx_edx
FrontendRuntimeCallback64_DispatchRecord1350
          (dword argument1,dword argument2,dword argument3,dword argument4,
          FrontendCallbackArgument5 argument5,dword argument6);

/* 0x00548C70 */
void FrontendRuntimeCallback68_DispatchRefresh1340(dword callbackArgument);

/* 0x00548CB0 */
void __thandor_void_preserve_eax_ecx_edx FrontendRecentTextHistory_InsertAndRebuild5(word *text);

/* 0x00549100 */
void FrontendCallback_ApplyGameSpeedOrDispatch02C0(dword callbackArgument);

/* 0x00549140 */
void FrontendCallback_ReleaseSelectedResourceOrDispatch0320(dword callbackArgument);

/* 0x00549180 */
void FrontendCallback_NoOpArg1(void *source);

/* 0x00549AB0 */
void __thandor_preserve_eax FrontendCallback_ReturnToMainPageOrDispatch0DC0(dword callbackArgument);

/* 0x0054A5A0 */
void FrontendCallback_ReturnToMainPageOrDispatchState4(dword callbackArgument);

/* 0x0054A7D0 */
void __thandor_preserve_eax
FrontendCallback_ReturnToMainPageOrDispatch0DC0_Secondary(dword callbackArgument);

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
FrontendInitEaxCf5 __thandor_eax_cf_preserve_ecx_edx Frontend_Init(RomRecordId initialRomRecordId);

/* 0x00547630 */
void __thandor_void_preserve_eax_ecx_edx Frontend_StateTick(void);

/* 0x00543B70 */
void __thandor_void_preserve_ecx_edx
FrontendMenu_BindSharedResources(FrontendRootResourceSlots5954 *frontendUiState);

/* 0x005445A0 */
void __thandor_void_preserve_eax_ecx_edx
FrontendUiAction2044_IndexedSelectionHelper
          (FrontendIndexedSelectionArgument argument1,dword argument2,dword argument3,
          FrontendFactionAssignmentIndex selectionIndex);

/* 0x00544640 */
void __thandor_void_preserve_eax_ecx_edx
FrontendUiAction2045_IndexedSelectionHelper
          (dword argument1,dword argument2,dword argument3,
          FrontendFactionAssignmentIndex selectionIndex);

/* 0x005446A0 */
void __thandor_void_preserve_eax_ecx_edx
FrontendUiAction2046_IndexedSelectionHelper
          (FrontendIndexedSelectionArgument argument1,dword argument2,dword argument3,
          FrontendFactionAssignmentIndex selectionIndex);

/* 0x00546190 */
void __thandor_void_preserve_eax_ecx_edx
FrontendDebugOverlay_RefreshCountersAndWorldCoordinates(void);

/* 0x005474E0 */
void __thandor_void_preserve_eax_ecx_edx FrontendRuntime_ShutdownAndReleaseResourcesRegs(void);

/* 0x0050AD90 */
qword FrontendModelPointerContext_FindBestEligibleModelHitTarget (int pointerY,int pointerX,FrontendModelPointerContextRuntimeState118 *context);

#endif /* THANDOR_UI_FRONTEND_RUNTIME_H */

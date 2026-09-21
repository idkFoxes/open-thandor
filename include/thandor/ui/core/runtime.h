/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/include/thandor/ui/core/runtime.h
 * Reverse engineering by idkFoxes 2026
 */

#ifndef THANDOR_UI_CORE_RUNTIME_H
#define THANDOR_UI_CORE_RUNTIME_H

#include <thandor/generated/types.h>
#include <thandor/core/contracts.h>

/* Submodule: ui/core/runtime. */
/* Functions are grouped by semantic ownership; address comments are executable virtual addresses. */

/* 0x004228F0 */
bool __thandor_cf_preserve_eax_ecx_edx UiRootCallbacks_FreeCf(UiRootNode *root);

/* 0x00422980 */
bool __thandor_cf_preserve_eax_ecx_edx UiRootCallbacks_NoOpMethod08(UiRootNode *root);

/* 0x00424270 */
void __thandor_void_preserve_eax_ecx UiRuntime_FormatSignedValues140And144(void *runtime);

/* 0x004244E0 */
void __thandor_void_preserve_eax_ecx_edx
UiRuntime_OpenFourValueDialogCf
          (UiPixelCoordinate value0,UiPixelCoordinate value1,UiPixelCoordinate value2,
          UiPixelCoordinate value3);

/* 0x004AEF00 */
UiRuntimeRecordRingDiscardEaxEdxCf9 __thandor_eax_edx_cf_preserve_ecx
UiRuntimeRecordRing_DiscardOldestCf(void);

/* 0x004AF020 */
void __thandor_preserve_eax UiRuntimeRecordRing_Clear(void);

/* 0x004AF030 */
bool __thandor_cf_preserve_eax_ecx_edx UiRuntimeRecordRing_HasPendingCf(void);

/* 0x004AF050 */
bool __thandor_cf_preserve_eax_ecx_edx
UiRuntimeRecordRing_ContainsIdCf(UiTransferSequenceToken sequenceToken);

/* 0x004AF0F0 */
void __thandor_preserve_eax_edx
UiRuntime_SetSynchronizationHooks
          (UiRuntimePostUnlockCallbackProc *postUnlockCallback,RuntimeSpinLockValue *frameLock);

/* 0x004AF210 */
void __thandor_preserve_eax UiRuntime_Initialize(void);

/* 0x004AF2F0 */
void UiRuntime_Shutdown(void);

/* 0x004AF3A0 */
void __cdecl UiRuntime_IncrementPeriodicTickCounter(void);

/* 0x004AF760 */
void __cdecl UiActionQueue_DispatchPending(void);

/* 0x004B05A0 */
void __thandor_void_preserve_eax_ecx_edx UiNode_DefaultMethod04_NoOp(void *node);

/* 0x004B0750 */
void __thandor_void_preserve_eax_ecx_edx
UiNode_DefaultNonRightPress
          (UiPointerWheelDelta wheelDelta,UiPixelCoordinate pointerY,UiPixelCoordinate pointerX,
          UiNodeBase *control);

/* 0x004B0760 */
void __thandor_void_preserve_eax_ecx_edx
UiNode_DefaultNonRightRelease
          (UiPointerWheelDelta wheelDelta,UiPixelCoordinate pointerY,UiPixelCoordinate pointerX,
          UiNodeBase *control);

/* 0x004B0770 */
void __thandor_preserve_eax_edx
UiNode_ForwardRightPressToParent
          (UiPointerWheelDelta wheelDelta,UiPixelCoordinate pointerY,UiPixelCoordinate pointerX,
          UiNodeBase *control);

/* 0x004B07C0 */
void __thandor_void_preserve_eax_ecx_edx
UiNode_DefaultRightRelease
          (UiPointerWheelDelta wheelDelta,UiPixelCoordinate pointerY,UiPixelCoordinate pointerX,
          UiNodeBase *control);

/* 0x004B07D0 */
void UiNode_DefaultNonRightDrag (UiPointerWheelDelta wheelDelta,UiPixelCoordinate pointerY,UiPixelCoordinate pointerX ,UiNodeBase *control);

/* 0x004B07E0 */
void __thandor_preserve_eax_edx
UiNode_DefaultRightDrag
          (UiPointerWheelDelta wheelDelta,UiPixelCoordinate pointerY,UiPixelCoordinate pointerX,
          UiNodeBase *control);

/* 0x004B08E0 */
void __thandor_void_preserve_eax_ecx_edx
UiNode_ApplyFlagsRecursive(UiNodeFlagMask setMask,UiNodeFlagMask retainMask,UiNodeBase *control);

/* 0x004B09E0 */
void UiNode_DefaultTick(UiNodeBase *control);

/* 0x004B0FD0 */
void __thandor_preserve_eax
UiActionHandlers_SetPageCf(UiActionHandlerPageIndex pageIndex,UiActionHandlerPage *page);

/* 0x004B14B0 */
UiNodeBase * UiNode_GetRoot(UiNodeBase *node);

/* 0x004B1510 */
void __thandor_void_preserve_eax_ecx_edx UiNode_InvalidateRoot(UiNodeBase *node);

/* 0x004B1590 */
void __thandor_void_preserve_eax_ecx_edx UiActionQueue_Enqueue(UiActionId actionId,void *source);

/* 0x004BD160 */
PackedArgb32 UiNode_GetStateTintArgb(UiNodeBase *node);


/* 0x00422990 */
int __thandor_eax_preserve_ecx_edx UiRootPointerMissPolicy_ReturnCode8(UiRootNode *root);

#endif /* THANDOR_UI_CORE_RUNTIME_H */

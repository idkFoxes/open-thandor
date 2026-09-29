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

/* Receive ring of network packets (g_UiRuntimeRecordRing, 0x100-byte UiRuntimeRecord slots) with a parallel
   array of 0x80-byte sender-endpoint slots (g_UiRuntimeRecordEndpointSlots); indices wrap after 256. */
#define UI_RUNTIME_RECORD_RING_LAST_INDEX 0xff
#define UI_RUNTIME_RECORD_RING_CAPACITY 0x100
#define UI_RUNTIME_RECORD_ENDPOINT_SLOT_SIZE 0x80

/* ARGB8888 opaque black, the clear colour of full-screen fills */
#define UI_ARGB_OPAQUE_BLACK 0xff000000u
/* Buffers UiRuntime_Initialize allocates for the mailbox transfer (g_UiTransferEndpointBuffer,
   g_UiTransferDataBuffer) */
#define UI_TRANSFER_ENDPOINT_BUFFER_BYTES 0x1000
#define UI_TRANSFER_DATA_BUFFER_BYTES 0x2000
/* ModelRuntimeNode_GetStateTintArgb results */
#define UI_MODEL_TINT_OPAQUE_WHITE 0xffffffffu /* colours unchanged */
#define UI_MODEL_TINT_TRANSPARENT_WHITE 0x00ffffff
#define UI_MODEL_TINT_OPAQUE_GREY 0xff878787u

/* Action queue (g_UiActionQueueEntries, allocated by UiRuntime_Initialize): 16 entries of 8 bytes
   (actionId, source), filled by UiActionQueue_Enqueue and drained once per frame by
   UiActionQueue_DispatchPending. actionId -1 means "no action" and is never queued. */
#define UI_ACTION_QUEUE_BYTES 0x80
#define UI_ACTION_NONE (-1)
/* Dirty rectangles collected by UiNode_InvalidateRoot per frame (0x18-byte UiDirtyRectEntry each). */
#define UI_DIRTY_RECT_CAPACITY 0x40
/* Action handler table g_UiActionHandlerPages: 256 pages of 256 handlers; action id bits 8..15 pick the page,
   bits 0..7 the handler (UiActionQueue_DispatchPending). */
#define UI_ACTION_HANDLER_PAGE_COUNT 256
/* Handler pages Game_LoadCoreAssets installs (UiActionHandlers_SetPage); page = action id >> 8 */
#define UI_ACTION_PAGE_INGAME 0x10              /* g_InGameUiActionHandlersPage10: INGAME_ACTION_* 0x10xx */
#define UI_ACTION_PAGE_INGAME_COMMAND_MODE 0x11 /* g_InGameUiActionHandlersPage11 */
#define UI_ACTION_PAGE_INGAME_MENU 0x12         /* g_InGameUiActionHandlersPage12: settings and save pages, 0x12xx */
#define UI_ACTION_PAGE_FRONTEND 0x20            /* g_FrontendUiActionHandlersPage20: frontend menus, 0x20xx */

/* 0x004228F0 */
bool UiRootCallbacks_Free(UiRootNode *root);

/* 0x00422980 */
bool UiModalDialogRoot_BlockMissedPointerPress(UiRootNode *root);

/* 0x00424270 */
void UiDisplaySettingsRoot_FormatColorReadouts(void *root);

/* 0x004244E0 */
void UiRuntime_OpenFourValueDialog(UiPixelCoordinate previousAdapterIndex,UiPixelCoordinate previousBitsPerPixel,
          UiPixelCoordinate previousHeight,UiPixelCoordinate previousWidth);

/* 0x004AEF00 */
RecordRingDiscardResult UiRuntimeRecordRing_DiscardOldest(void);

/* 0x004AF020 */
void UiRuntimeRecordRing_Clear(void);

/* 0x004AF030 */
bool UiRuntimeRecordRing_HasPending(void);

/* 0x004AF050 */
bool UiRuntimeRecordRing_ContainsId(UiTransferSequenceToken sessionToken);

/* 0x004AF0F0 */
void UiRuntime_SetSynchronizationHooks
          (UiRuntimePostUnlockCallbackProc *postUnlockCallback,RuntimeSpinLockValue *frameLock);

/* 0x004AF210 */
void UiRuntime_Initialize(void);

/* 0x004AF2F0 */
void UiRuntime_Shutdown(void);

/* 0x004AF3A0 */
void __cdecl UiRuntime_IncrementPeriodicTickCounter(void);

/* 0x004AF760 */
void __cdecl UiActionQueue_DispatchPending(void);

/* 0x004B05A0 */
void UiNode_DefaultMethod04_NoOp(void *node);

/* 0x004B0750 */
void UiNode_DefaultNonRightPress(UiPointerWheelDelta wheelDelta,UiPixelCoordinate pointerY,UiPixelCoordinate pointerX,
          UiNodeBase *control);

/* 0x004B0760 */
void UiNode_DefaultNonRightRelease(UiPointerWheelDelta wheelDelta,UiPixelCoordinate pointerY,UiPixelCoordinate pointerX,
          UiNodeBase *control);

/* 0x004B0770 */
void UiNode_ForwardRightPressToParent
          (UiPointerWheelDelta wheelDelta,UiPixelCoordinate pointerY,UiPixelCoordinate pointerX,
          UiNodeBase *control);

/* 0x004B07C0 */
void UiNode_DefaultRightRelease(UiPointerWheelDelta wheelDelta,UiPixelCoordinate pointerY,UiPixelCoordinate pointerX,
          UiNodeBase *control);

/* 0x004B07D0 */
void UiNode_DefaultNonRightDrag (UiPointerWheelDelta wheelDelta,UiPixelCoordinate pointerY,UiPixelCoordinate pointerX ,UiNodeBase *control);

/* 0x004B07E0 */
void UiNode_DefaultRightDrag(UiPointerWheelDelta wheelDelta,UiPixelCoordinate pointerY,UiPixelCoordinate pointerX,
          UiNodeBase *control);

/* 0x004B08E0 */
void UiNode_ApplyFlagsRecursive(UiNodeFlagMask setMask,UiNodeFlagMask retainMask,UiNodeBase *control);

/* 0x004B09E0 */
void UiNode_DefaultTick(UiNodeBase *control);

/* 0x004B0FD0 */
void UiActionHandlers_SetPage(UiActionHandlerPageIndex pageIndex,UiActionHandlerPage *page);

/* 0x004B14B0 */
UiNodeBase * UiNode_GetRoot(UiNodeBase *node);

/* 0x004B1510 */
void UiNode_InvalidateRoot(UiNodeBase *node);

/* 0x004B1590 */
void UiActionQueue_Enqueue(UiActionId actionId,void *source);

/* 0x004BD160 */
PackedArgb32 ModelRuntimeNode_GetStateTintArgb(ModelRuntimeNode *node);


/* 0x00422990 */
int UiModalDialogRoot_BlockMissedPointerMotion(UiRootNode *root);

#endif /* THANDOR_UI_CORE_RUNTIME_H */

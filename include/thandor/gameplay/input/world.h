/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/include/thandor/gameplay/input/world.h
 * Reverse engineering by idkFoxes 2026
 */

#ifndef THANDOR_GAMEPLAY_INPUT_WORLD_H
#define THANDOR_GAMEPLAY_INPUT_WORLD_H

#include <thandor/generated/types.h>
#include <thandor/core/contracts.h>

/* Submodule: gameplay/input/world. */

/* pickedHeightQ12 of the world pointer callbacks when the pointer is not over the field. */
#define WORLD_POINTER_NO_HIT 0x7FFFFFFF
/* Cursor frames returned by InGameWorldInput_ResolveContextActionAndCursor (besides GRAPHICS_CURSOR_FRAME_*). */
#define WORLD_CURSOR_OWN_ARMY 0x15 /* candidate belongs to the local faction; a click selects it */
#define WORLD_CURSOR_FOREIGN_ARMY 0x16 /* candidate belongs to another faction */
#define WORLD_CURSOR_MOVE 0x17 /* ground under the pointer is a valid position command target */
#define WORLD_CURSOR_NO_TARGET 0x18 /* position command not possible (off the field or rejected) */
/* the selection can act on the pointed army or point (same frame as EDITOR_CURSOR_DELETE_TARGET) */
#define WORLD_CURSOR_TARGET 0x19
/* the pointed army is not a target of the selection (same frame as EDITOR_CURSOR_DELETE_NONE) */
#define WORLD_CURSOR_TARGET_REJECTED 0x1A
#define WORLD_CURSOR_PLACEMENT_BLOCKED 0x2D /* the pending army asset cannot be placed here */
#define WORLD_CURSOR_PLACEMENT_VALID 0x2E /* the pending army asset can be placed here */
/* WorldRuntimeContext.runtimeFlags bit set once a captured pointer moved far enough to start a drag selection. */
#define WORLD_RUNTIME_FLAG_DRAG_SELECTING 0x80
/* Set while the camera shows the target of a notification "go to" (InGameTargetingContext_AdvanceOrResolveTarget,
   cleared by InGameTargetingContext_CancelAndRestoreState); the world input then only shows the busy cursor. */
#define WORLD_RUNTIME_FLAG_NOTIFICATION_GOTO 0x10
/* Set on a pointer press while interaction flag 0x80 is held: the release replaces the selection instead of
   selecting a single army (InGameWorldInput_BeginPointerCapture / _CommitPointerAction). */
#define WORLD_RUNTIME_FLAG_REPLACE_SELECTION 0x8000000
/* Pointer travel (pixels on either axis) after which a capture turns into a drag selection. */
#define WORLD_DRAG_SELECTION_THRESHOLD 23
/* g_InGamePointerInteractionStateFlags bits. */
#define WORLD_POINTER_STATE_OVER_OWN_ARMY 0x1 /* command mode: the pointer rests on an own army (click selects it) */
#define WORLD_POINTER_STATE_SELECTION_CAPTURE 0x2 /* selection mode: the pointer was captured on press */
/* Functions are grouped by semantic ownership. */

void InGameTargetingContext_AdvanceOrResolveTarget(InGameTargetingRootTraversalView *targetingContext);

uint32_t InGameWorldInput_ResolveContextActionAndCursor
                (InGamePointerCallbackValue0 pickedHeightQ12,InGamePointerCallbackValue1 pointerWorldXQ12
                ,InGamePointerCallbackValue2 pointerWorldYQ12,InGamePointerCallbackValue3 candidateHeightQ12
                ,WorldOwnerListNode *candidateNode,WorldRuntimeContext *inGameRuntime);

void InGameWorldInput_BeginPointerCapture
          (InGamePointerCallbackValue0 pickedHeightQ12,uint32_t pointerWorldXQ12,uint32_t pointerWorldYQ12,
          InGamePointerCallbackValue3 candidateHeightQ12,WorldOwnerListNode *candidateNode,
          WorldRuntimeContext *inGameRuntime);

void InGameWorldInput_UpdateDragSelectionAndCamera
          (InGamePointerCallbackValue0 pickedHeightQ12,uint32_t pointerWorldXQ12,uint32_t pointerWorldYQ12,
          uint32_t candidateHeightQ12,WorldOwnerListNode *candidateNode,
          WorldRuntimeContext *inGameRuntime);

void InGameWorldInput_CommitPointerAction
          (InGamePointerCallbackValue0 pickedHeightQ12,InGamePointerCallbackValue1 pointerWorldXQ12,
          InGamePointerCallbackValue2 pointerWorldYQ12,InGamePointerCallbackValue3 candidateHeightQ12,
          WorldOwnerListNode *candidateNode,WorldRuntimeContext *inGameRuntime);

bool InGameCameraCommand_DispatchByCodeAndModifierFlags
          (UiKeyboardStateMask modifierFlags,UiActionId commandCode,WorldRuntimeContext *worldRuntime);

void InGameTargetingContext_CancelAndRestoreState(InGameTargetingRootTraversalView *targetingContext);

#endif /* THANDOR_GAMEPLAY_INPUT_WORLD_H */

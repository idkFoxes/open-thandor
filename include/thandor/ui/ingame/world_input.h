/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/include/thandor/ui/ingame/world_input.h
 * Reverse engineering by idkFoxes 2026
 */

#ifndef THANDOR_UI_INGAME_WORLD_INPUT_H
#define THANDOR_UI_INGAME_WORLD_INPUT_H

#include <thandor/core/types.h>
#include <thandor/ui/ingame/types.h>
#include <thandor/core/contracts.h>

/* pickedHeightQ12 of the world pointer callbacks when the pointer is not over the field. */
inline constexpr int32_t WORLD_POINTER_NO_HIT = 0x7FFFFFFF;
/* Cursor frames returned by InGameWorldInput_ResolveContextActionAndCursor (besides GRAPHICS_CURSOR_FRAME_*). */
inline constexpr int32_t WORLD_CURSOR_OWN_ARMY = 0x15; /* candidate belongs to the local faction; a click selects it */
inline constexpr int32_t WORLD_CURSOR_FOREIGN_ARMY = 0x16; /* candidate belongs to another faction */
inline constexpr int32_t WORLD_CURSOR_MOVE = 0x17; /* ground under the pointer is a valid position command target */
inline constexpr int32_t WORLD_CURSOR_NO_TARGET = 0x18; /* position command not possible (off the field or rejected) */
/* the selection can act on the pointed army or point (same frame as EDITOR_CURSOR_DELETE_TARGET) */
inline constexpr int32_t WORLD_CURSOR_TARGET = 0x19;
/* the pointed army is not a target of the selection (same frame as EDITOR_CURSOR_DELETE_NONE) */
inline constexpr int32_t WORLD_CURSOR_TARGET_REJECTED = 0x1A;
inline constexpr int32_t WORLD_CURSOR_PLACEMENT_BLOCKED = 0x2D; /* the pending army asset cannot be placed here */
inline constexpr int32_t WORLD_CURSOR_PLACEMENT_VALID = 0x2E; /* the pending army asset can be placed here */
/* WorldRuntimeContext.runtimeFlags: the flag enum WorldRuntimeFlags (world/runtime/flags.h). */
/* Pointer travel (pixels on either axis) after which a capture turns into a drag selection. */
inline constexpr int32_t WORLD_DRAG_SELECTION_THRESHOLD = 23;
/* g_InGamePointerInteractionStateFlags bits. */
inline constexpr int32_t WORLD_POINTER_STATE_OVER_OWN_ARMY = 0x1; /* command mode: the pointer rests on an own army (click selects it) */
inline constexpr int32_t WORLD_POINTER_STATE_SELECTION_CAPTURE = 0x2; /* selection mode: the pointer was captured on press */

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

extern int32_t g_InGamePlacementSurfaceHeightQ12OrSentinel;

extern InGameCommandPayloadTripletValue32 g_InGameSelectionInsertTripletDwords[12]; /* drag-selection insert batch, four triplets */
extern InGameCommandPayloadTripletValue32 g_InGameSelectionRemoveTripletDwords[12]; /* drag-selection remove batch, four triplets */
extern uint32_t g_InGamePlacementHeading16;
extern uint32_t g_InGamePlacementWorldYQ12;
extern uint32_t g_InGamePlacementWorldXQ12;
extern uint32_t g_InGameCommandPreviewHeading16;
extern uint32_t g_InGameCommandPreviewWorldYQ12;
extern uint32_t g_InGameCommandPreviewWorldXQ12;
extern uint32_t g_InGameCommandPreviewSurfaceHeightQ12OrSentinel;
extern uint32_t g_InGamePointerInteractionStateFlags;

bool WorldRuntimeNode_IsPositionInsideBounds
          (WorldOwnerListNode *runtimeNode,WorldRuntimeExtendedMapControlView *boundsControl);

/* Scroll-arrow cursor frames returned by WorldRuntime_ApplyEdgeScrollAndGetCursorFrame (clockwise from up). */
inline constexpr int32_t WORLD_CURSOR_SCROLL_UP = 0x2F;
inline constexpr int32_t WORLD_CURSOR_SCROLL_UP_RIGHT = 0x30;
inline constexpr int32_t WORLD_CURSOR_SCROLL_RIGHT = 0x31;
inline constexpr int32_t WORLD_CURSOR_SCROLL_DOWN_RIGHT = 0x32;
inline constexpr int32_t WORLD_CURSOR_SCROLL_DOWN = 0x33;
inline constexpr int32_t WORLD_CURSOR_SCROLL_DOWN_LEFT = 0x34;
inline constexpr int32_t WORLD_CURSOR_SCROLL_LEFT = 0x35;
inline constexpr int32_t WORLD_CURSOR_SCROLL_UP_LEFT = 0x36;

uint32_t WorldRuntime_ApplyEdgeScrollAndGetCursorFrame(WorldRuntimeContext *worldRuntime);

#endif /* THANDOR_UI_INGAME_WORLD_INPUT_H */

#ifndef THANDOR_GAMEPLAY_INPUT_WORLD_H
#define THANDOR_GAMEPLAY_INPUT_WORLD_H

#include <thandor/generated/v523_types.h>
#include <thandor/core/contracts.h>

/* Submodule: gameplay/input/world. */
/* Functions are grouped by semantic ownership; address comments are executable virtual addresses. */

/* 0x0056D2D0 */
void InGameTargetingContext_AdvanceOrResolveTarget(InGameTargetingContext *targetingContext);

/* 0x005688A0 */
dword InGameWorldInput_ResolveContextActionAndCursorCf (InGamePointerCallbackValue0 pointerValue0,InGamePointerCallbackValue1 pointerValue1 ,InGamePointerCallbackValue2 pointerValue2,InGamePointerCallbackValue3 pointerValue3 ,WorldRuntimeNode *candidateNode,WorldRuntimeContext *inGameRuntime);

/* 0x00568CB0 */
void InGameWorldInput_BeginPointerCaptureCf (InGamePointerCallbackValue0 pointerValue0,dword pointerValue1,dword pointerValue2, InGamePointerCallbackValue3 pointerValue3,WorldRuntimeNode *candidateNode, WorldRuntimeContext *inGameRuntime);

/* 0x00568E10 */
void InGameWorldInput_UpdateDragSelectionAndCameraCf (InGamePointerCallbackValue0 pointerValue0,dword pointerValue1,dword pointerValue2, dword pointerValue3,WorldRuntimeNode *candidateNode, WorldRuntimeContext *inGameRuntime);

/* 0x005691B0 */
void InGameWorldInput_CommitPointerActionCf (InGamePointerCallbackValue0 pointerValue0,InGamePointerCallbackValue1 pointerValue1, InGamePointerCallbackValue2 pointerValue2,InGamePointerCallbackValue3 pointerValue3, WorldRuntimeNode *candidateNode,WorldRuntimeContext *inGameRuntime);

/* 0x0056F230 */
void InGameCameraCommand_DispatchByCodeAndModifierFlagsCf (dword modifierFlags,dword commandCode,WorldRuntimeContext *worldRuntime);

/* 0x0056F770 */
undefined8 InGameCameraCommand_CaptureBookmarkSlot7EntryAndSharedSuccessTailCf(void);

/* 0x0056D4B0 */
void InGameTargetingContext_CancelAndRestoreState(InGameTargetingContext *targetingContext);

#endif /* THANDOR_GAMEPLAY_INPUT_WORLD_H */

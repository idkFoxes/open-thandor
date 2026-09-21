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
/* Functions are grouped by semantic ownership; address comments are executable virtual addresses. */

/* 0x0056D2D0 */
void __thandor_void_preserve_eax_ecx_edx
InGameTargetingContext_AdvanceOrResolveTarget
          (InGameTargetingRootTraversalView9E60 *targetingContext);

/* 0x005688A0 */
dword InGameWorldInput_ResolveContextActionAndCursorCf
                (InGamePointerCallbackValue0 pointerValue0,InGamePointerCallbackValue1 pointerValue1
                ,InGamePointerCallbackValue2 pointerValue2,InGamePointerCallbackValue3 pointerValue3
                ,WorldOwnerListNode100 *candidateNode,WorldRuntimeContext *inGameRuntime);

/* 0x00568CB0 */
void __thandor_void_preserve_eax_ecx_edx
InGameWorldInput_BeginPointerCaptureCf
          (InGamePointerCallbackValue0 pointerValue0,dword pointerValue1,dword pointerValue2,
          InGamePointerCallbackValue3 pointerValue3,WorldOwnerListNode100 *candidateNode,
          WorldRuntimeContext *inGameRuntime);

/* 0x00568E10 */
void __thandor_void_preserve_eax_ecx_edx
InGameWorldInput_UpdateDragSelectionAndCameraCf
          (InGamePointerCallbackValue0 pointerValue0,dword pointerValue1,dword pointerValue2,
          dword pointerValue3,WorldOwnerListNode100 *candidateNode,
          WorldRuntimeContext *inGameRuntime);

/* 0x005691B0 */
void __thandor_void_preserve_eax_ecx_edx
InGameWorldInput_CommitPointerActionCf
          (InGamePointerCallbackValue0 pointerValue0,InGamePointerCallbackValue1 pointerValue1,
          InGamePointerCallbackValue2 pointerValue2,InGamePointerCallbackValue3 pointerValue3,
          WorldOwnerListNode100 *candidateNode,WorldRuntimeContext *inGameRuntime);

/* 0x0056F230 */
void __thandor_void_preserve_eax_ecx_edx
InGameCameraCommand_DispatchByCodeAndModifierFlagsCf
          (dword modifierFlags,dword commandCode,WorldRuntimeContext *worldRuntime);

/* 0x0056D4B0 */
void __thandor_preserve_eax
InGameTargetingContext_CancelAndRestoreState(InGameTargetingRootTraversalView9E60 *targetingContext);

#endif /* THANDOR_GAMEPLAY_INPUT_WORLD_H */

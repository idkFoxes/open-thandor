/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/src/ui/core/runtime.c
 * Reverse engineering by idkFoxes 2026
 */

#include <thandor/ui/core/runtime.h>

/* Implementation ownership: ui/core/runtime. */

/* Address: 0x004228F0.
   Ownership: ui/core/runtime.
   Purpose: Binary entry is anchored by g_CodePointerTable_004229A0[0]@004229A0;
   g_CodePointerTable_00424324[0]@00424324. UiRootCallbacks root callback with one stack argument.
*/
bool __thandor_cf_preserve_eax_ecx_edx UiRootCallbacks_FreeCf(UiRootNode *root)

{
  ArenaFreeEaxCf5 AVar1;
  
  AVar1 = (*g_MemoryApi.free)(root);
  return AVar1.carry;
}


/* Address: 0x00422980.
   Ownership: ui/core/runtime.
   Purpose: Binary entry is anchored by g_CodePointerTable_004229A0[2]@004229A0;
   g_CodePointerTable_00424324[2]@00424324. UiRootCallbacks method08; caller-cleanup one-argument convention is
   intentional.
*/
bool __thandor_cf_preserve_eax_ecx_edx UiRootCallbacks_NoOpMethod08(UiRootNode *root)

{
  return true;
}


/* Address: 0x00422990.
   Ownership: ui/core/runtime.
   Purpose: Recovered UI root pointer miss-policy helper that returns code 8.
*/
int __thandor_eax_preserve_ecx_edx UiRootPointerMissPolicy_ReturnCode8(UiRootNode *root)

{
  return 8;
}

/* Address: 0x00424270.
   Ownership: ui/core/runtime.
   Purpose: Formats signed dwords at runtime offsets +0x140 and +0x144 into the UTF-16 buffers at +0xBB4 and
   +0xB94. Both calls use width 3, base 10, terminator and signed flags, with exact scale values 0x400000 and
   0x10000. EAX is preserved.
*/
void __thandor_void_preserve_eax_ecx UiRuntime_FormatSignedValues140And144(void *runtime)

{
  (*g_WideNumberFormatUtf16)
            (WIDE_FORMAT_WRITE_TERMINATOR|WIDE_FORMAT_SIGNED_VALUE,3,10,0x400000,
             *(sdword *)((int)runtime + 0x140),(word *)((int)runtime + 0xbb4));
  (*g_WideNumberFormatUtf16)
            (WIDE_FORMAT_WRITE_TERMINATOR|WIDE_FORMAT_SIGNED_VALUE,3,10,0x10000,
             *(sdword *)((int)runtime + 0x144),(word *)((int)runtime + 0xb94));
  return;
}


/* Address: 0x004244E0.
   Ownership: ui/core/runtime.
   Purpose: Typed parameters: p0 value0→UiPixelCoordinate_V297, p1 value1→UiPixelCoordinate_V297, p2
   value2→UiPixelCoordinate_V297, p3 value3→UiPixelCoordinate_V297. Calling convention, complete VariableStorage
   serialization, function bytes, control flow, globals, locals, and executable data remain unchanged.
   Cross-module calls: TextResource_Resolve [assets/text/resources], RichTextCommandStream_PatchPayloadBySelector
   [assets/text/richtext], UiRootStack_Push [ui/controls/layout], UiRootStack_InvalidateAll [ui/controls/layout].
*/
void __thandor_void_preserve_eax_ecx_edx
UiRuntime_OpenFourValueDialogCf
          (UiPixelCoordinate value0,UiPixelCoordinate value1,UiPixelCoordinate value2,
          UiPixelCoordinate value3)

{
  sdword *arg5;
  UiRootNode *root;
  int iVar1;
  undefined4 *puVar2;
  UiRootNode *pUVar3;
  ArenaAllocEaxCf5 AVar4;
  TextResourceResolveEaxCf5 TVar5;
  
  AVar4 = (*g_MemoryApi.alloc)(0x1a4);
  root = (UiRootNode *)AVar4.eax;
  if (!AVar4.carry) {
    puVar2 = &g_UiFourValueDialogTemplateImage;
    pUVar3 = root;
    for (iVar1 = 0x69; iVar1 != 0; iVar1 = iVar1 + -1) {
      (pUVar3->base).nextSibling = (UiNodeBase *)*puVar2;
      puVar2 = puVar2 + 1;
      pUVar3 = (UiRootNode *)&(pUVar3->base).firstChild;
    }
    arg5 = &root[4].base.topOffset;
    TVar5 = TextResource_Resolve(0x109);
    RichTextCommandStream_PatchPayloadBySelector(0,arg5,TVar5.eax);
    (*g_WideNumberFormatUtf16)
              (WIDE_FORMAT_WRITE_TERMINATOR,0,10,1,(sdword)root[4].base.vtable,(word *)arg5);
    root[4].base.top = value3;
    root[4].base.right = value2;
    root[4].base.bottom = value1;
    root[4].base.leftOffset = value0;
    UiRootStack_Push(&g_UiFourValueDialogRootCallbacks,root);
    UiRootStack_InvalidateAll();
    return;
  }
  return;
}


/* Address: 0x004AEF00.
   Ownership: ui/core/runtime.
   Purpose: Acquires the UI runtime-ring lock and advances the 256-entry read index. CF clear means one record was
   discarded; CF set means the ring was empty.
*/
UiRuntimeRecordRingDiscardEaxEdxCf9 __thandor_eax_edx_cf_preserve_ecx
UiRuntimeRecordRing_DiscardOldestCf(void)

{
  uint uVar1;
  dword dVar2;
  UiRuntimeRecordRingDiscardEaxEdxCf9 UVar3;
  UiRuntimeRecordRingDiscardEaxEdxCf9 UVar4;
  
  (*g_SpinLockAcquire)(&g_UiRuntimeRecordRingLock);
  dVar2 = g_UiRuntimeRecordReadIndex;
  if (g_UiRuntimeRecordWriteIndex != g_UiRuntimeRecordReadIndex) {
    uVar1 = g_UiRuntimeRecordReadIndex + 1;
    UVar3.eaxPayloadOrReadIndex = g_UiRuntimeRecordRing + g_UiRuntimeRecordReadIndex;
    UVar3.edxEndpointOrReadIndex =
         g_UiRuntimeRecordReadIndex * 0x80 + g_UiRuntimeAuxiliaryBuffer8000;
    g_UiRuntimeRecordReadIndex = uVar1;
    if (0xff < uVar1) {
      g_UiRuntimeRecordReadIndex = 0;
    }
    (*g_SpinLockRelease)(&g_UiRuntimeRecordRingLock);
    UVar3.carryEmpty = false;
    return UVar3;
  }
  (*g_SpinLockRelease)(&g_UiRuntimeRecordRingLock);
  UVar4.edxEndpointOrReadIndex = dVar2;
  UVar4.eaxPayloadOrReadIndex = dVar2;
  UVar4.carryEmpty = true;
  return UVar4;
}


/* Address: 0x004AF020.
   Ownership: ui/core/runtime.
   Purpose: Drops all pending records by copying the write index to the read index.
*/
void __thandor_preserve_eax UiRuntimeRecordRing_Clear(void)

{
  g_UiRuntimeRecordReadIndex = g_UiRuntimeRecordWriteIndex;
  return;
}


/* Address: 0x004AF030.
   Ownership: ui/core/runtime.
   Purpose: Compares the 256-entry write and read indices. CF set means at least one record is pending; CF clear
   means empty.
*/
bool __thandor_cf_preserve_eax_ecx_edx UiRuntimeRecordRing_HasPendingCf(void)

{
  if (g_UiRuntimeRecordWriteIndex != g_UiRuntimeRecordReadIndex) {
    return true;
  }
  return false;
}


/* Address: 0x004AF050.
   Ownership: ui/core/runtime.
   Purpose: Try-locks the ring and scans pending 0x100-byte records for recordId. CF set means a match; CF clear
   means absent or lock unavailable.
*/
bool __thandor_cf_preserve_eax_ecx_edx
UiRuntimeRecordRing_ContainsIdCf(UiTransferSequenceToken sequenceToken)

{
  dword ringIndex;
  UiRuntimeRecord *recordCursor;
  bool bVar1;
  
  bVar1 = (*g_SpinLockTryAcquire)(&g_UiRuntimeRecordRingLock);
  if (!bVar1) {
    if (g_UiRuntimeRecordReadIndex != g_UiRuntimeRecordWriteIndex) {
      recordCursor = g_UiRuntimeRecordRing + g_UiRuntimeRecordReadIndex;
      ringIndex = g_UiRuntimeRecordReadIndex;
      do {
        do {
          if (sequenceToken == (recordCursor->packetHeader).sequenceToken) {
            (*g_SpinLockRelease)(&g_UiRuntimeRecordRingLock);
            return true;
          }
          ringIndex = ringIndex + 1;
          recordCursor = recordCursor + 1;
          if (ringIndex == g_UiRuntimeRecordWriteIndex)
          goto UiRuntimeRecordRing_ContainsIdCf_ReleaseLockAndReturnNotFoundWithCarryClear;
        } while (ringIndex < 0x100);
        ringIndex = 0;
        recordCursor = g_UiRuntimeRecordRing;
      } while (g_UiRuntimeRecordWriteIndex != 0);
    }
UiRuntimeRecordRing_ContainsIdCf_ReleaseLockAndReturnNotFoundWithCarryClear:
    (*g_SpinLockRelease)(&g_UiRuntimeRecordRingLock);
  }
  return false;
}


/* Address: 0x004AF0F0.
   Ownership: ui/core/runtime.
   Purpose: Installs the runtime frame lock pointer and the optional callback passed to SpinLockReleaseAndInvoke.
   Passing two null pointers disables external synchronization.
*/
void __thandor_preserve_eax_edx
UiRuntime_SetSynchronizationHooks
          (UiRuntimePostUnlockCallbackProc *postUnlockCallback,RuntimeSpinLockValue *frameLock)

{
  g_UiRuntimeFrameLock = frameLock;
  g_UiRuntimePostUnlockCallback = postUnlockCallback;
  return;
}


/* Address: 0x004AF210.
   Ownership: ui/core/runtime.
   Purpose: Registers UI timers, loads core UI assets, allocates the fixed 64-entry dirty-rectangle queue and
   16-entry action queue, and allocates the remaining input/runtime buffers.
   Cross-module calls: FontRuntime_Init [assets/text/resources], UiWindowResources_Init [ui/controls/layout],
   ErrorRuntime_InstallUiHandlerAndAllocateState [core/error/runtime].
*/
void __thandor_preserve_eax UiRuntime_Initialize(void)

{
  ArenaAllocEaxCf5 AVar1;
  FatalErrorEaxCf5 FVar2;
  
  (*g_TimerRegisterPeriodic)(0x14,UiRuntime_IncrementPeriodicTickCounter);
  g_UiRuntimeInitializationCount = g_UiRuntimeInitializationCount + 1;
  FontRuntime_Init();
  UiWindowResources_Init();
  AVar1 = (*g_MemoryApi.alloc)(0x600);
  FVar2 = (*g_FatalErrorPrimaryDispatchCf)(AVar1.eax,AVar1.carry);
  g_UiDirtyRectEntries = (UiDirtyRectEntry *)FVar2.eax;
  AVar1 = (*g_MemoryApi.alloc)(0x80);
  FVar2 = (*g_FatalErrorPrimaryDispatchCf)(AVar1.eax,AVar1.carry);
  g_UiActionQueueEntries = (UiActionQueueEntry *)FVar2.eax;
  ErrorRuntime_InstallUiHandlerAndAllocateState();
  (*g_TimerRegisterPeriodic)(0x7d,UiTransferMailbox_ServiceAndRetransmitTimer);
  AVar1 = (*g_MemoryApi.alloc)(0x8000);
  FVar2 = (*g_FatalErrorPrimaryDispatchCf)(AVar1.eax,AVar1.carry);
  g_UiRuntimeAuxiliaryBuffer8000 = FVar2.eax;
  AVar1 = (*g_MemoryApi.alloc)(0x10000);
  FVar2 = (*g_FatalErrorPrimaryDispatchCf)(AVar1.eax,AVar1.carry);
  g_UiRuntimeRecordRing = (UiRuntimeRecord *)FVar2.eax;
  AVar1 = (*g_MemoryApi.alloc)(0x1000);
  FVar2 = (*g_FatalErrorPrimaryDispatchCf)(AVar1.eax,AVar1.carry);
  g_UiTransferEndpointBuffer = (UiTransferEndpointDescriptor *)FVar2.eax;
  AVar1 = (*g_MemoryApi.alloc)(0x2000);
  FVar2 = (*g_FatalErrorPrimaryDispatchCf)(AVar1.eax,AVar1.carry);
  g_UiTransferDataBuffer = (byte *)FVar2.eax;
  g_UiRuntimeRecordWriteIndex = 0;
  g_UiRuntimeRecordReadIndex = 0;
  g_UiTransferUnitCursor = 0;
  return;
}


/* Address: 0x004AF2F0.
   Ownership: ui/core/runtime.
   Purpose: Unregisters UI timers and frees the dirty-rectangle, action, input, and runtime buffers when the UI
   runtime is initialized.
*/
void UiRuntime_Shutdown(void)

{
  if (g_UiRuntimeInitializationCount != 0) {
    (*g_TimerUnregisterPeriodic)(UiTransferMailbox_ServiceAndRetransmitTimer);
    (*g_MemoryApi.free)(g_UiRuntimeRecordRing);
    (*g_MemoryApi.free)(g_UiRuntimeAuxiliaryBuffer8000);
    (*g_MemoryApi.free)(g_UiTransferDataBuffer);
    (*g_MemoryApi.free)(g_UiTransferEndpointBuffer);
    g_UiRuntimeRecordRing = (UiRuntimeRecord *)0x0;
    g_UiRuntimeAuxiliaryBuffer8000 = (void *)0x0;
    g_UiTransferDataBuffer = (byte *)0x0;
    g_UiTransferEndpointBuffer = (UiTransferEndpointDescriptor *)0x0;
    (*g_MemoryApi.free)(g_UiDirtyRectEntries);
    g_UiDirtyRectEntries = (UiDirtyRectEntry *)0x0;
    (*g_MemoryApi.free)(g_UiActionQueueEntries);
    g_UiActionQueueEntries = (UiActionQueueEntry *)0x0;
    (*g_TimerUnregisterPeriodic)(UiRuntime_IncrementPeriodicTickCounter);
    g_UiRuntimeInitializationCount = g_UiRuntimeInitializationCount + -1;
  }
  return;
}

/* Address: 0x004AF3A0.
   Ownership: ui/core/runtime.
   Purpose: Twenty-millisecond UI timer callback that increments the shared periodic UI tick counter.
*/
void __cdecl UiRuntime_IncrementPeriodicTickCounter(void)

{
  g_UiPendingFrameTicks = g_UiPendingFrameTicks + 1;
  return;
}

/* Address: 0x004AF760.
   Ownership: ui/core/runtime.
   Purpose: Drains the 16-entry action queue under the UI spin lock. Resolves each action ID through
   g_UiActionHandlerPages[highByte]->handlers[lowByte], shifts the remaining queue entries down, and calls the
   handler with the queued source/context pointer.
*/
void __cdecl UiActionQueue_DispatchPending(void)

{
  void *pvVar1;
  code *pcVar2;
  UiActionQueueEntry *pUVar3;
  int iVar4;
  UiActionQueueEntry *pUVar5;
  UiActionQueueEntry *pUVar6;
  
  (*g_SpinLockAcquire)(g_UiRuntimeFrameLock);
  pUVar3 = g_UiActionQueueEntries;
  while (g_UiActionQueueUsedBytes != 0) {
    pvVar1 = pUVar3->source;
    g_UiActionQueueUsedBytes = g_UiActionQueueUsedBytes - 8;
    pcVar2 = *(code **)((int)g_UiActionHandlerPages[(uint)pUVar3->actionId >> 8]->handlers +
                       (pUVar3->actionId * 4 & 0x3fcU));
    pUVar5 = pUVar3 + 1;
    pUVar6 = pUVar3;
    for (iVar4 = 0x1e; pUVar3 = g_UiActionQueueEntries, iVar4 != 0; iVar4 = iVar4 + -1) {
      pUVar6->actionId = pUVar5->actionId;
      pUVar5 = (UiActionQueueEntry *)&pUVar5->source;
      pUVar6 = (UiActionQueueEntry *)&pUVar6->source;
    }
    (*pcVar2)(pvVar1);
  }
  (*g_SpinLockReleaseAndInvoke)
            ((SpinLockReleaseCallbackProc *)g_UiRuntimePostUnlockCallback,g_UiRuntimeFrameLock);
  return;
}

/* Address: 0x004B05A0.
   Ownership: ui/core/runtime.
   Purpose: Shared one-argument no-op installed in common UI-node vtable slot +0x04. It preserves the incoming
   register and flag state except for the ordinary stack-frame instructions.
   [VERSIONLESS_CANONICAL_DATATYPE_CLOSURE] Retired detached enum dictionary UiNodeKnownCallbackSlot after
   transferring its complete value vocabulary to code annotation. It is not a safe whole-value storage type.
   Values: 0=RELOCATE, 2=DRAW_CLIPPED, 3=LAYOUT, 4=NON_RIGHT_PRESS, 5=NON_RIGHT_RELEASE, 6=RIGHT_PRESS,
   7=RIGHT_RELEASE, 8=NON_RIGHT_DRAG, 9=RIGHT_DRAG, 10=POINTER_MOVE, 11=HIT_TEST, 12=KEYBOARD_EVENT_CF,
   13=APPLY_FLAGS, 14=SUPPRESS_ACTION_ID, 15=UNSUPPRESS_ACTION_ID, 16=TICK, 17=POINTER_WHEEL
*/
void __thandor_void_preserve_eax_ecx_edx UiNode_DefaultMethod04_NoOp(void *node)

{
  return;
}


/* Address: 0x004B0750.
   Ownership: ui/core/runtime.
   Purpose: Shared four-argument no-op installed in common UI-node vtable slot +0x10, the non-right pointer-press
   slot. Existing return-register and flag behavior is preserved.
*/
void __thandor_void_preserve_eax_ecx_edx
UiNode_DefaultNonRightPress
          (UiPointerWheelDelta wheelDelta,UiPixelCoordinate pointerY,UiPixelCoordinate pointerX,
          UiNodeBase *control)

{
  return;
}


/* Address: 0x004B0760.
   Ownership: ui/core/runtime.
   Purpose: Default no-op left/middle release handler.
*/
void __thandor_void_preserve_eax_ecx_edx
UiNode_DefaultNonRightRelease
          (UiPointerWheelDelta wheelDelta,UiPixelCoordinate pointerY,UiPixelCoordinate pointerX,
          UiNodeBase *control)

{
  return;
}


/* Address: 0x004B0770.
   Ownership: ui/core/runtime.
   Purpose: Forwards rightPress to node->parent while updating g_UiPointerCaptureTarget. Reaching the root clears
   the capture target and button.
*/
void __thandor_preserve_eax_edx
UiNode_ForwardRightPressToParent
          (UiPointerWheelDelta wheelDelta,UiPixelCoordinate pointerY,UiPixelCoordinate pointerX,
          UiNodeBase *control)

{
  g_UiPointerCaptureTarget = control->parent;
  if (g_UiPointerCaptureTarget == (UiNodeBase *)0xffffffff) {
    g_UiPointerCaptureTarget = (UiNodeBase *)0xffffffff;
    g_UiPointerCaptureButton = UI_POINTER_CAPTURE_NONE;
  }
  else {
    (*g_UiPointerCaptureTarget->vtable->rightPress)
              (wheelDelta,pointerY,pointerX,g_UiPointerCaptureTarget);
  }
  return;
}


/* Address: 0x004B07C0.
   Ownership: ui/core/runtime.
   Purpose: Default no-op right-button release handler.
*/
void __thandor_void_preserve_eax_ecx_edx
UiNode_DefaultRightRelease
          (UiPointerWheelDelta wheelDelta,UiPixelCoordinate pointerY,UiPixelCoordinate pointerX,
          UiNodeBase *control)

{
  return;
}


/* Address: 0x004B07D0.
   Ownership: ui/core/runtime.
   Purpose: Default no-op left/middle capture-drag handler.
*/
void UiNode_DefaultNonRightDrag
               (UiPointerWheelDelta wheelDelta,UiPixelCoordinate pointerY,UiPixelCoordinate pointerX
               ,UiNodeBase *control)

{
  return;
}

/* Address: 0x004B07E0.
   Ownership: ui/core/runtime.
   Purpose: Default no-op right-button capture-drag handler.
*/
void __thandor_preserve_eax_edx
UiNode_DefaultRightDrag
          (UiPointerWheelDelta wheelDelta,UiPixelCoordinate pointerY,UiPixelCoordinate pointerX,
          UiNodeBase *control)

{
  return;
}


/* Address: 0x004B08E0.
   Ownership: ui/core/runtime.
   Purpose: Updates nodeFlags as (nodeFlags & retainMask) | setMask and forwards the same masks to direct children
   through vtable slot +0x34.
*/
void __thandor_void_preserve_eax_ecx_edx
UiNode_ApplyFlagsRecursive(UiNodeFlagMask setMask,UiNodeFlagMask retainMask,UiNodeBase *control)

{
  UiNodeBase *control_00;
  
  control->nodeFlags = control->nodeFlags & retainMask;
  control->nodeFlags = control->nodeFlags | setMask;
  for (control_00 = control->firstChild; control_00 != (UiNodeBase *)0xffffffff;
      control_00 = control_00->nextSibling) {
    (*control_00->vtable->applyFlags)(setMask,retainMask,control_00);
  }
  return;
}


/* Address: 0x004B09E0.
   Ownership: ui/core/runtime.
   Purpose: Default no-op per-frame node tick.
*/
void UiNode_DefaultTick(UiNodeBase *control)

{
  return;
}

/* Address: 0x004B0FD0.
   Ownership: ui/core/runtime.
   Purpose: Registers one 256-entry action-handler page when pageIndex is below 256.
*/
void __thandor_preserve_eax
UiActionHandlers_SetPageCf(UiActionHandlerPageIndex pageIndex,UiActionHandlerPage *page)

{
  if (pageIndex < 0x100) {
    g_UiActionHandlerPages[pageIndex] = page;
    return;
  }
  return;
}


/* Address: 0x004B14B0.
   Ownership: ui/core/runtime.
   Purpose: Walks parent pointers at offset 0x08 until the 0xFFFFFFFF root sentinel is reached. Returns the root
   node in EAX.
*/
UiNodeBase * UiNode_GetRoot(UiNodeBase *node)

{
  UiNodeBase *parentNode;
  
  parentNode = node->parent;
  while (parentNode != (UiNodeBase *)0xffffffff) {
    node = node->parent;
    parentNode = node->parent;
  }
  return node;
}

/* Address: 0x004B1510.
   Ownership: ui/core/runtime.
   Purpose: Finds a node's control-tree root and appends a 0x18-byte UiDirtyRectEntry when invalidation is enabled
   and fewer than 64 entries are queued.
*/
void __thandor_void_preserve_eax_ecx_edx UiNode_InvalidateRoot(UiNodeBase *node)

{
  UiDirtyRectEntry *dirtyRectEntry;
  GraphicsScreenCoordinate bottomEdgeCoordinate;
  UiNodeBase *parentNode;
  GraphicsScreenCoordinate edgeCoordinate;
  
  if (g_UiInvalidationSuppressed == 0) {
    parentNode = node->parent;
    while (parentNode != (UiNodeBase *)0xffffffff) {
      node = (((UiRootNode *)node)->base).parent;
      parentNode = (((UiRootNode *)node)->base).parent;
    }
    if (g_UiDirtyRectCount < 0x40) {
      dirtyRectEntry = g_UiDirtyRectEntries + g_UiDirtyRectCount;
      edgeCoordinate = (((UiRootNode *)node)->base).right;
      dirtyRectEntry->left = (((UiRootNode *)node)->base).left;
      dirtyRectEntry->right = edgeCoordinate;
      bottomEdgeCoordinate = (((UiRootNode *)node)->base).bottom;
      dirtyRectEntry->top = (((UiRootNode *)node)->base).top;
      dirtyRectEntry->bottom = bottomEdgeCoordinate;
      dirtyRectEntry->rootNode = (UiRootNode *)node;
      dirtyRectEntry->rootNodeCopy = (UiRootNode *)node;
      g_UiDirtyRectCount = g_UiDirtyRectCount + 1;
    }
  }
  return;
}


/* Address: 0x004B1590.
   Ownership: ui/core/runtime.
   Purpose: Appends an actionId/source pair to the fixed 0x80-byte queue when space remains and actionId is not -1.
   The queue holds sixteen 8-byte entries.
*/
void __thandor_void_preserve_eax_ecx_edx UiActionQueue_Enqueue(UiActionId actionId,void *source)

{
  UiActionId *destinationEntry;
  
  if (g_UiActionQueueUsedBytes < 0x80) {
    destinationEntry =
         (UiActionId *)((int)&g_UiActionQueueEntries->actionId + g_UiActionQueueUsedBytes);
    if (actionId != -1) {
      *destinationEntry = actionId;
      destinationEntry[1] = (UiActionId)source;
      g_UiActionQueueUsedBytes = g_UiActionQueueUsedBytes + 8;
    }
  }
  return;
}


/* Address: 0x004BD160.
   Ownership: ui/core/runtime.
   Purpose: Returns the ARGB state tint selected from UiNodeBase.nodeFlags bits 0x04, 0x08, and 0x10.
*/
PackedArgb32 UiNode_GetStateTintArgb(UiNodeBase *node)

{
  PackedArgb32 tintArgb;
  UiNodeBase *stateFlags;
  
  tintArgb = 0xffffffff;
  stateFlags = node[1].nextSibling;
  if (((((uint)stateFlags & 4) == 0) && (tintArgb = 0, ((uint)stateFlags & 8) != 0)) &&
     (tintArgb = 0xffffff, ((uint)stateFlags & 0x10) == 0)) {
    tintArgb = 0xff878787;
  }
  return tintArgb;
}

/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/src/ui/core/runtime.cpp
 * Reverse engineering by idkFoxes 2026
 */

#include <thandor/ui/core/runtime.h>
#include <thandor/core/bytes.h>
#include <thandor/thandor.h>

/* Module data. */

UiRuntimeRecord *g_UiRuntimeRecordRing = nullptr;

uintptr_t g_UiRuntimeRecordEndpointSlots = 0;

uint8_t *g_UiTransferDataBuffer = nullptr;

UiTransferEndpointDescriptor *g_UiTransferEndpointBuffer = nullptr;

RuntimeSpinLockValue g_UiRuntimeRecordRingLock = 0;

UiDirtyRectCount g_UiDirtyRectCount = 0;

static uint32_t g_UiRuntimeRecordReadIndex = 0;

static UiDirtyRectEntry *g_UiDirtyRectEntries = nullptr;

static UiActionQueueUsedBytes g_UiActionQueueUsedBytes = 0;

static UiActionQueueEntry *g_UiActionQueueEntries = nullptr;

static uint32_t g_UiRuntimeInitializationCount = 0;

static UiActionHandlerPage *g_UiActionHandlerPages[256] = {};

RuntimeSpinLockValue *g_UiRuntimeFrameLock = nullptr;

UiRuntimePostUnlockCallbackProc *g_UiRuntimePostUnlockCallback = nullptr;

/* vetoClose callback of g_UiDisplaySettingsRootCallbacks and g_UiFourValueDialogRootCallbacks: frees the
   heap copy of the dialog root when UiRootStack_Pop closes it. The close is vetoed (returns true) only when
   the free fails.
*/
Bool8 UiRootCallbacks_Free(UiRootNode *root)

{
  return g_MemoryApi.free(root) != 0;
}

/* method08 of g_UiDisplaySettingsRootCallbacks and g_UiFourValueDialogRootCallbacks: always returns true,
   so a pointer press that misses the dialog ends the root-stack hit test there (the dialogs are modal).
*/
Bool8 UiModalDialogRoot_BlockMissedPointerPress(UiRootNode *root)

{
  return true;
}

/* pointerMissPolicy of g_UiDisplaySettingsRootCallbacks and g_UiFourValueDialogRootCallbacks: the
   non-negative result stops pointer motion that misses the dialog from reaching the roots below it. The value
   8 carries no meaning beyond being non-negative (same as FatalErrorDialog_BlockMissedPointerMotion).
*/
int UiModalDialogRoot_BlockMissedPointerMotion(UiRootNode *root)

{
  return 8;
}

/* Takes the oldest received network packet out of the receive ring (under the ring lock) and returns
   pointers to it and to its sender endpoint. The slot is released, not copied, so the data stays valid only
   until the receiver wraps around to it again. Returns true with *outPacket (the packet slot) and
   *outEndpoint (its sender-endpoint slot) set; returns false and leaves both untouched when nothing was
   pending.
*/
Bool8 UiRuntimeRecordRing_TakeOldest(void **outPacket,void **outEndpoint)

{
  uint32_t nextReadIndex;

  g_SpinLockAcquire(&g_UiRuntimeRecordRingLock);
  if (g_UiRuntimeRecordWriteIndex != g_UiRuntimeRecordReadIndex) {
    nextReadIndex = g_UiRuntimeRecordReadIndex + 1;
    *outPacket = g_UiRuntimeRecordRing + g_UiRuntimeRecordReadIndex;
    *outEndpoint = reinterpret_cast<void *>
         (g_UiRuntimeRecordReadIndex * UI_RUNTIME_RECORD_ENDPOINT_SLOT_SIZE + g_UiRuntimeRecordEndpointSlots);
    g_UiRuntimeRecordReadIndex = nextReadIndex;
    if (UI_RUNTIME_RECORD_RING_LAST_INDEX < nextReadIndex) {
      g_UiRuntimeRecordReadIndex = 0;
    }
    g_SpinLockRelease(&g_UiRuntimeRecordRingLock);
    return true;
  }
  g_SpinLockRelease(&g_UiRuntimeRecordRingLock);
  return false;
}

/* Discards every received network packet still waiting in the record ring (read index = write index),
   e.g. before a new session is opened.
*/
void UiRuntimeRecordRing_Clear()

{
  g_UiRuntimeRecordReadIndex = g_UiRuntimeRecordWriteIndex;
}

/* Returns true when a pending received packet carries sessionToken in its header, i.e. when the
   host of this session has sent something. The in-game client tick uses it to skip processing until the
   host's packets are there. Returns false when nothing matches or the ring lock is busy (it only try-locks).
*/
Bool8 UiRuntimeRecordRing_ContainsId(UiTransferSequenceToken sessionToken)

{
  uint32_t ringIndex;
  UiRuntimeRecord *recordCursor;
  Bool8 lockUnavailable;

  lockUnavailable = g_SpinLockTryAcquire(&g_UiRuntimeRecordRingLock);
  if (lockUnavailable) {
    return false;
  }
  /* walk the pending records from the read index up to the write index (both stay in 0..LAST_INDEX) */
  ringIndex = g_UiRuntimeRecordReadIndex;
  recordCursor = g_UiRuntimeRecordRing + ringIndex;
  while (ringIndex != g_UiRuntimeRecordWriteIndex) {
    if (sessionToken == (recordCursor->packetHeader).sequenceToken) {
      g_SpinLockRelease(&g_UiRuntimeRecordRingLock);
      return true;
    }
    ringIndex++;
    recordCursor++;
    if (UI_RUNTIME_RECORD_RING_LAST_INDEX < ringIndex) {
      /* wrap around the 256-entry ring */
      ringIndex = 0;
      recordCursor = g_UiRuntimeRecordRing;
    }
  }
  g_SpinLockRelease(&g_UiRuntimeRecordRingLock);
  return false;
}

/* Installs the spin lock the UI input and layout code takes around each frame, and the callback run after
   that lock is released (SpinLockReleaseAndInvoke), so UI work is serialised with the active state tick
   (frontend or in-game). Passing two null pointers disables the synchronisation.
*/
void UiRuntime_SetSynchronizationHooks
          (UiRuntimePostUnlockCallbackProc *postUnlockCallback,RuntimeSpinLockValue *frameLock)

{
  g_UiRuntimeFrameLock = frameLock;
  g_UiRuntimePostUnlockCallback = postUnlockCallback;
}

/* Sets up the UI runtime at startup: registers the 20 Hz frame-tick timer and the 125 Hz transfer-mailbox
   timer (g_TimerRegisterPeriodic takes a frequency), loads fonts and window resources, installs the
   in-game error handler and allocates the UI queues and the network transfer buffers. Every allocation
   failure is fatal.
*/
void UiRuntime_Initialize()

{
  uint32_t allocError;
  void *allocPayload;
  uintptr_t checkedValue;

  g_TimerRegisterPeriodic(20,UiRuntime_IncrementPeriodicTickCounter);
  g_UiRuntimeInitializationCount++;
  FontRuntime_Init();
  UiWindowResources_Init();
  allocError = g_MemoryApi.alloc(UI_DIRTY_RECT_CAPACITY * sizeof(UiDirtyRectEntry),&allocPayload);
  checkedValue = FatalError_ExitIfFailed(allocError != 0 ? allocError : (uintptr_t)allocPayload,allocError != 0);
  /* FatalError_ExitIfFailed passes each block through as an address */
  g_UiDirtyRectEntries = reinterpret_cast<UiDirtyRectEntry *>(checkedValue);
  allocError = g_MemoryApi.alloc(UI_ACTION_QUEUE_BYTES,&allocPayload); /* 16 queued actions */
  checkedValue = FatalError_ExitIfFailed(allocError != 0 ? allocError : (uintptr_t)allocPayload,allocError != 0);
  g_UiActionQueueEntries = reinterpret_cast<UiActionQueueEntry *>(checkedValue);
  /* from here on FatalError_ReportIfFailed shows errors in an in-game dialog */
  ErrorRuntime_InstallUiHandlerAndAllocateState();
  g_TimerRegisterPeriodic(125,UiTransferMailbox_ServiceAndRetransmitTimer);
  /* the sender-endpoint slots, parallel to the ring */
  allocError = g_MemoryApi.alloc(UI_RUNTIME_RECORD_RING_CAPACITY * UI_RUNTIME_RECORD_ENDPOINT_SLOT_SIZE,&allocPayload);
  checkedValue = FatalError_ExitIfFailed(allocError != 0 ? allocError : (uintptr_t)allocPayload,allocError != 0);
  g_UiRuntimeRecordEndpointSlots = checkedValue;
  allocError = g_MemoryApi.alloc(UI_RUNTIME_RECORD_RING_CAPACITY * sizeof(UiRuntimeRecord),&allocPayload);
  checkedValue = FatalError_ExitIfFailed(allocError != 0 ? allocError : (uintptr_t)allocPayload,allocError != 0);
  g_UiRuntimeRecordRing = reinterpret_cast<UiRuntimeRecord *>(checkedValue);
  allocError = g_MemoryApi.alloc(UI_TRANSFER_ENDPOINT_BUFFER_BYTES,&allocPayload);
  checkedValue = FatalError_ExitIfFailed(allocError != 0 ? allocError : (uintptr_t)allocPayload,allocError != 0);
  g_UiTransferEndpointBuffer = reinterpret_cast<UiTransferEndpointDescriptor *>(checkedValue);
  allocError = g_MemoryApi.alloc(UI_TRANSFER_DATA_BUFFER_BYTES,&allocPayload);
  checkedValue = FatalError_ExitIfFailed(allocError != 0 ? allocError : (uintptr_t)allocPayload,allocError != 0);
  g_UiTransferDataBuffer = reinterpret_cast<uint8_t *>(checkedValue);
  g_UiRuntimeRecordWriteIndex = 0;
  g_UiRuntimeRecordReadIndex = 0;
  g_UiTransferUnitCursor = 0;
}

/* Counterpart of UiRuntime_Initialize at program end: stops the network receive timer and the frame-tick
   timer and frees the network rings/buffers, the dirty-rectangle list and the action queue. Does nothing if
   the UI runtime was never initialized.
*/
void UiRuntime_Shutdown()

{
  if (g_UiRuntimeInitializationCount != 0) {
    g_TimerUnregisterPeriodic(UiTransferMailbox_ServiceAndRetransmitTimer);
    g_MemoryApi.free(g_UiRuntimeRecordRing);
    g_MemoryApi.free(reinterpret_cast<void *>(g_UiRuntimeRecordEndpointSlots));
    g_MemoryApi.free(g_UiTransferDataBuffer);
    g_MemoryApi.free(g_UiTransferEndpointBuffer);
    g_UiRuntimeRecordRing = nullptr;
    g_UiRuntimeRecordEndpointSlots = 0;
    g_UiTransferDataBuffer = nullptr;
    g_UiTransferEndpointBuffer = nullptr;
    g_MemoryApi.free(g_UiDirtyRectEntries);
    g_UiDirtyRectEntries = nullptr;
    g_MemoryApi.free(g_UiActionQueueEntries);
    g_UiActionQueueEntries = nullptr;
    g_TimerUnregisterPeriodic(UiRuntime_IncrementPeriodicTickCounter);
    g_UiRuntimeInitializationCount--;
  }
}

/* Frame-tick timer (20 Hz, registered by UiRuntime_Initialize): counts the pending frame ticks that the
   frame loop waits for and consumes.
*/
void UiRuntime_IncrementPeriodicTickCounter()

{
  g_UiPendingFrameTicks.fetch_add(1);
}

/* Runs the queued UI actions (button clicks, list selections, ...) in order, under the frame lock. An
   action id selects the handler page by its high byte (g_UiActionHandlerPages) and the handler by its low
   byte; the handler gets the control that queued it. Each entry is removed (the rest moved down) before its
   handler runs, so handlers may queue further actions.
*/
void UiActionQueue_DispatchPending()

{
  void *actionSource;
  void (*actionHandler)(void *);
  UiActionQueueEntry *queueHead;
  int remainingCount;
  UiActionQueueEntry *sourceEntry;
  UiActionQueueEntry *destinationEntry;
  
  g_SpinLockAcquire(g_UiRuntimeFrameLock);
  queueHead = g_UiActionQueueEntries;
  while (g_UiActionQueueUsedBytes != 0) {
    actionSource = queueHead->source;
    g_UiActionQueueUsedBytes = g_UiActionQueueUsedBytes - sizeof(UiActionQueueEntry);
    actionHandler = g_UiActionHandlerPages[(int32_t)((uint32_t)queueHead->actionId >> 8)]->handlers
                    [(int32_t)((uint32_t)queueHead->actionId & (UI_ACTION_HANDLER_PAGE_COUNT - 1))];
    sourceEntry = queueHead + 1;
    destinationEntry = queueHead;
    /* move entries 1..15 (30 dwords) down by one */
    for (remainingCount = 30; queueHead = g_UiActionQueueEntries, remainingCount != 0; remainingCount--) {
      destinationEntry->actionId = sourceEntry->actionId;
      /* one dword further: the entries are moved dword by dword */
      sourceEntry = reinterpret_cast<UiActionQueueEntry *>(&sourceEntry->source);
      destinationEntry = reinterpret_cast<UiActionQueueEntry *>(&destinationEntry->source);
    }
    actionHandler(actionSource);
  }
  g_SpinLockReleaseAndInvoke(g_UiRuntimePostUnlockCallback,g_UiRuntimeFrameLock);
}

/* Default method04 vtable slot of the UI node classes: does nothing. Installed statically in 40
   UiNodeVtable tables; no caller of the slot is known yet.
*/
void UiNode_DefaultMethod04_NoOp(UiNodeBase *node)

{
}

/* Default nonRightPress (left/middle button press) of the UI node classes: ignores the
   press. Installed statically in 14 UiNodeVtable tables.
*/
void UiNode_DefaultNonRightPress(UiPointerWheelDelta wheelDelta,UiPixelCoordinate pointerY,UiPixelCoordinate pointerX,
          UiNodeBase *control)

{
}

/* Default nonRightRelease (left/middle button release) of the UI node classes: ignores
   the release. Installed statically in 25 UiNodeVtable tables.
*/
void UiNode_DefaultNonRightRelease(UiPointerWheelDelta wheelDelta,UiPixelCoordinate pointerY,UiPixelCoordinate pointerX,
          UiNodeBase *control)

{
}

/* Default rightPress of the UI node classes: passes the right-button press up to the
   parent, which also takes over the pointer capture; at the root nobody takes it and the capture is
   cleared. Installed statically in 32 UiNodeVtable tables.
*/
void UiNode_ForwardRightPressToParent
          (UiPointerWheelDelta wheelDelta,UiPixelCoordinate pointerY,UiPixelCoordinate pointerX,
          UiNodeBase *control)

{
  g_UiPointerCaptureTarget = control->parent;
  if (g_UiPointerCaptureTarget == UI_NODE_NONE) {
    g_UiPointerCaptureTarget = UI_NODE_NONE;
    g_UiPointerCaptureButton = UI_POINTER_CAPTURE_NONE;
  }
  else {
    g_UiPointerCaptureTarget->vtable->rightPress
              (wheelDelta,pointerY,pointerX,g_UiPointerCaptureTarget);
  }
}

/* Default rightRelease of the UI node classes: ignores the right-button release.
   Installed statically in 34 UiNodeVtable tables.
*/
void UiNode_DefaultRightRelease(UiPointerWheelDelta wheelDelta,UiPixelCoordinate pointerY,UiPixelCoordinate pointerX,
          UiNodeBase *control)

{
}

/* Default nonRightDrag (pointer motion while a left/middle press holds the capture) of the
   UI node classes: ignores it. Installed statically in 25 UiNodeVtable tables.
*/
void UiNode_DefaultNonRightDrag
               (UiPointerWheelDelta wheelDelta,UiPixelCoordinate pointerY,UiPixelCoordinate pointerX,
               UiNodeBase *control)

{
}

/* Default rightDrag (pointer motion while a right press holds the capture) of the UI
   node classes: ignores it. Installed statically in 34 UiNodeVtable tables.
*/
void UiNode_DefaultRightDrag(UiPointerWheelDelta wheelDelta,UiPixelCoordinate pointerY,UiPixelCoordinate pointerX,
          UiNodeBase *control)

{
}

/* Default applyFlags of the UI node classes: sets nodeFlags to
   (nodeFlags & retainMask) | setMask and passes the same masks to each direct child's applyFlags, so the change
   reaches the whole subtree. Installed statically in 39 UiNodeVtable tables; also called
   directly by UiLayoutContainerControl_ApplyFlagsRecursive once per page.
*/
void UiNode_ApplyFlagsRecursive(UiNodeFlags setMask,UiNodeFlags retainMask,UiNodeBase *control)

{
  UiNodeBase *childControl;
  
  control->nodeFlags = control->nodeFlags & retainMask;
  control->nodeFlags = control->nodeFlags | setMask;
  for (childControl = control->firstChild; childControl != UI_NODE_NONE;
      childControl = childControl->nextSibling) {
    childControl->vtable->applyFlags(setMask,retainMask,childControl);
  }
}

/* Default tick (per-frame update) of the UI node classes: does nothing. Installed
   statically in 30 UiNodeVtable tables.
*/
void UiNode_DefaultTick(UiNodeBase *control)

{
}

/* Installs the handler page for action ids pageIndex * 256 .. pageIndex * 256 + 255, so a UI module can register
   its actions; out-of-range page indices are ignored.
*/
void UiActionHandlers_SetPage(UiActionHandlerPageIndex pageIndex,UiActionHandlerPage *page)

{
  if (pageIndex < UI_ACTION_HANDLER_PAGE_COUNT) {
    g_UiActionHandlerPages[pageIndex] = page;
    return;
  }
}

/* Returns the root (window) node of the UI tree containing node: the ancestor whose parent is UI_NODE_NONE.
*/
UiNodeBase * UiNode_GetRoot(UiNodeBase *node)

{
  UiNodeBase *parentNode;

  parentNode = node->parent;
  while (parentNode != UI_NODE_NONE) {
    node = node->parent;
    parentNode = node->parent;
  }
  return node;
}

/* Requests a redraw of the window (root) that contains node: its rectangle is added to this frame's dirty
   list. Ignored while invalidation is suppressed or when UI_DIRTY_RECT_CAPACITY rectangles are already
   collected.
*/
void UiNode_InvalidateRoot(UiNodeBase *node)

{
  UiDirtyRectEntry *dirtyRectEntry;
  GraphicsScreenCoordinate bottomEdgeCoordinate;
  UiNodeBase *parentNode;
  GraphicsScreenCoordinate edgeCoordinate;
  
  if (g_UiInvalidationSuppressed == 0) {
    parentNode = node->parent;
    while (parentNode != UI_NODE_NONE) {
      node = (UiNode_As<UiRootNode>(node)->base).parent;
      parentNode = (UiNode_As<UiRootNode>(node)->base).parent;
    }
    if (g_UiDirtyRectCount < UI_DIRTY_RECT_CAPACITY) {
      dirtyRectEntry = g_UiDirtyRectEntries + g_UiDirtyRectCount;
      edgeCoordinate = (UiNode_As<UiRootNode>(node)->base).right;
      dirtyRectEntry->left = (UiNode_As<UiRootNode>(node)->base).left;
      dirtyRectEntry->right = edgeCoordinate;
      bottomEdgeCoordinate = (UiNode_As<UiRootNode>(node)->base).bottom;
      dirtyRectEntry->top = (UiNode_As<UiRootNode>(node)->base).top;
      dirtyRectEntry->bottom = bottomEdgeCoordinate;
      dirtyRectEntry->rootNode = UiNode_As<UiRootNode>(node);
      dirtyRectEntry->rootNodeCopy = UiNode_As<UiRootNode>(node);
      g_UiDirtyRectCount++;
    }
  }
}

/* Queues action actionId of the control source for UiActionQueue_DispatchPending (at the end of the
   frame). UI_ACTION_NONE and actions beyond the 16 queue entries are dropped.
*/
void UiActionQueue_Enqueue(UiActionId actionId,void *source)

{
  UiActionQueueEntry *destinationEntry;

  if (g_UiActionQueueUsedBytes < UI_ACTION_QUEUE_BYTES) {
    destinationEntry = Thandor_At<UiActionQueueEntry>(g_UiActionQueueEntries, g_UiActionQueueUsedBytes);
    if (actionId != UI_ACTION_NONE) {
      destinationEntry->actionId = actionId;
      destinationEntry->source = source;
      g_UiActionQueueUsedBytes = g_UiActionQueueUsedBytes + sizeof(UiActionQueueEntry);
    }
  }
}

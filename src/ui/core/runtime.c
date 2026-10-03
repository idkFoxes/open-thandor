/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/src/ui/core/runtime.c
 * Reverse engineering by idkFoxes 2026
 */

#include <thandor/ui/core/runtime.h>
#include <thandor/thandor.h>

/* Module data. */

UiRuntimeRecord *g_UiRuntimeRecordRing = 0;

uint32_t g_UiRuntimeRecordEndpointSlots = 0;

uint8_t *g_UiTransferDataBuffer = 0;

UiTransferEndpointDescriptor *g_UiTransferEndpointBuffer = 0;

RuntimeSpinLockValue g_UiRuntimeRecordRingLock = 0;

UiDirtyRectCount g_UiDirtyRectCount = 0;

static UiRootCallbacks g_UiFourValueDialogRootCallbacks = {
    .vetoClose = THANDOR_FN(UiRootCallbacks_Free),
    .frameUpdate = THANDOR_FN(UiFourValueDialog_TickCountdownAndRequestClose),
    .method08 = THANDOR_FN(UiModalDialogRoot_BlockMissedPointerPress),
    .pointerMissPolicy = THANDOR_FN(UiModalDialogRoot_BlockMissedPointerMotion)};

static FourValueDialogUiImage g_UiFourValueDialogTemplateImage = {
        { /* +0000 confirmModeDialogPanel g_UiPanelControlVtable */
            .nextSibling = UI_TEMPLATE_NO_LINK, .firstChild = UI_TEMPLATE_LINK(0x58), .parent = UI_TEMPLATE_NO_LINK,
            .vtable = THANDOR_PTR(&g_UiPanelControlVtable),
            .leftOffset = -128, .topOffset = -48, .rightOffset = 128, .bottomOffset = 48,
            .leftAnchorQ31 = 0x40000000, .topAnchorQ31 = 0x40000000, .rightAnchorQ31 = 0x40000000, .bottomAnchorQ31 = 0x40000000,
            .layoutWidth = -1, .layoutHeight = -1, .nodeFlags = 0x21},
        {
            0x00000003},
        { /* +0058 revertButton g_UiFramedTextButtonControlVtable */
            .nextSibling = UI_TEMPLATE_LINK(0xB4), .firstChild = UI_TEMPLATE_NO_LINK, .parent = UI_TEMPLATE_LINK(0x0),
            .vtable = THANDOR_PTR(&g_UiFramedTextButtonControlVtable),
            .leftOffset = 16, .topOffset = -32, .rightOffset = 112, .bottomOffset = -8,
            .topAnchorQ31 = 0x80000000, .bottomAnchorQ31 = 0x80000000,
            .layoutWidth = -1, .layoutHeight = -1, .nodeFlags = 0x2},
        {
            0x00000008, 0x0000020D, 0x00000101},
        { /* +00B4 keepModeButton g_UiFramedTextButtonControlVtable */
            .nextSibling = UI_TEMPLATE_LINK(0x110), .firstChild = UI_TEMPLATE_NO_LINK, .parent = UI_TEMPLATE_LINK(0x0),
            .vtable = THANDOR_PTR(&g_UiFramedTextButtonControlVtable),
            .leftOffset = 128, .topOffset = -32, .rightOffset = 240, .bottomOffset = -8,
            .topAnchorQ31 = 0x80000000, .bottomAnchorQ31 = 0x80000000,
            .layoutWidth = -1, .layoutHeight = -1, .nodeFlags = 0x20},
        {
            0x00000004, 0x00000000, 0x00000100},
        { /* +0110 countdownMessageText g_UiListOffsetControlVtable */
            .nextSibling = UI_TEMPLATE_NO_LINK, .firstChild = UI_TEMPLATE_NO_LINK, .parent = UI_TEMPLATE_LINK(0x0),
            .vtable = THANDOR_PTR(&g_UiListOffsetControlVtable),
            .leftOffset = 8, .topOffset = 8, .rightOffset = -8, .bottomOffset = -40,
            .rightAnchorQ31 = 0x80000000, .bottomAnchorQ31 = 0x80000000,
            .layoutWidth = -1, .layoutHeight = -1},
        {
            0x00000004, 0x00000000, 0x00000109, 0x00000000, 0x0000000F, 0x00000014},
};

static uint32_t g_UiRuntimeRecordReadIndex = 0;

static UiDirtyRectEntry *g_UiDirtyRectEntries = 0;

static UiActionQueueUsedBytes g_UiActionQueueUsedBytes = 0;

static UiActionQueueEntry *g_UiActionQueueEntries = 0;

static uint32_t g_UiRuntimeInitializationCount = 0;

static UiActionHandlerPage *g_UiActionHandlerPages[256] = {0};

RuntimeSpinLockValue *g_UiRuntimeFrameLock = 0;

UiRuntimePostUnlockCallbackProc *g_UiRuntimePostUnlockCallback = 0;

/* Implementation ownership: ui/core/runtime. */

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

/* Writes the two number readouts of the display settings dialog (root is a copy of
   g_UiDisplaySettingsRootTemplate): the selected colour bias (applyButton selectedColorBiasQ16, Q16, -64..+64)
   divided by 64.0, i.e. -1.000..+1.000, and the colour scale (applyButton selectedColorScaleQ16, Q16,
   0.5..2.0) as a plain value, both signed with up to 3 fraction digits into the number buffers in the tail of
   colorBiasValueText (colorBiasTextUtf16 for the bias, colorScaleTextUtf16 for the scale). Called when the dialog opens and by
   UiDisplaySettingsRoot_RefreshModeSelection.
*/
void UiDisplaySettingsRoot_FormatColorReadouts(void *root)

{
  UiDisplaySettingsApplyButton *applyButton = (UiDisplaySettingsApplyButton *)DISPLAY_SETTINGS_UI(root,applyButton);
  UiDisplaySettingsValueReadout *readout =
       (UiDisplaySettingsValueReadout *)DISPLAY_SETTINGS_UI(root,colorBiasValueText);

  /* fractionalDigits 3, integerDigitLimit 10; the denominators are 64.0 and 1.0 in Q16 */
  g_WideNumberFormatUtf16
            (WIDE_FORMAT_WRITE_TERMINATOR|WIDE_FORMAT_SIGNED_VALUE,3,10,64 << 16,applyButton->selectedColorBiasQ16,
             readout->colorBiasTextUtf16);
  g_WideNumberFormatUtf16
            (WIDE_FORMAT_WRITE_TERMINATOR|WIDE_FORMAT_SIGNED_VALUE,3,10,1 << 16,applyButton->selectedColorScaleQ16,
             readout->colorScaleTextUtf16);
  return;
}


/* Opens the "keep the new display mode?" dialog after UiDisplayModeAction_ApplyPendingMode switched modes:
   copies g_UiFourValueDialogTemplateImage to the heap, points the countdown text (text 0x109) at its number
   buffer, prints the starting seconds there and stores the previous mode tuple, which the revert action
   0x20D (button or countdown expiry) restores. The original also reports a failed allocation to the caller;
   no caller looks at it.
*/
void UiRuntime_OpenFourValueDialog(UiPixelCoordinate previousAdapterIndex,UiPixelCoordinate previousBitsPerPixel,
          UiPixelCoordinate previousHeight,UiPixelCoordinate previousWidth)

{
  uint16_t *countdownNumberBuffer;
  UiRootNode *root;
  int remainingDwords;
  uint32_t *templateCursor;
  uint32_t *copyCursor;
  uint32_t allocError;
  uint16_t *resolvedText;
  UiFourValueDialogCountdownText *countdownText;

  allocError = g_MemoryApi.alloc(sizeof(g_UiFourValueDialogTemplateImage),(void **)&root);
  if (allocError != 0) {
    root = (UiRootNode *)allocError;
  }
  else {
    /* copy the 0x1A4-byte template, one dword per step */
    templateCursor = (uint32_t *)&g_UiFourValueDialogTemplateImage;
    copyCursor = (uint32_t *)root;
    countdownText = (UiFourValueDialogCountdownText *)FOUR_VALUE_DIALOG_UI(root,countdownMessageText);
    for (remainingDwords = sizeof(g_UiFourValueDialogTemplateImage) / 4; remainingDwords != 0; remainingDwords--) {
      *copyCursor = *templateCursor;
      templateCursor++;
      copyCursor++;
    }
    countdownNumberBuffer = countdownText->countdownTextUtf16;
    resolvedText = TextResource_Resolve(TEXT_ID_DISPLAY_MODE_KEEP_COUNTDOWN);
    RichTextCommandStream_PatchPayloadBySelector(0,countdownNumberBuffer,resolvedText);
    g_WideNumberFormatUtf16(WIDE_FORMAT_WRITE_TERMINATOR,0,10,1,countdownText->countdown,countdownNumberBuffer);
    countdownText->previousWidth = previousWidth;
    countdownText->previousHeight = previousHeight;
    countdownText->previousBitsPerPixel = previousBitsPerPixel;
    countdownText->previousAdapterIndex = previousAdapterIndex;
    UiRootStack_Push(&g_UiFourValueDialogRootCallbacks,root);
    UiRootStack_InvalidateAll();
    return;
  }
  return;
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
    *outEndpoint = (void *)(uintptr_t)
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
void UiRuntimeRecordRing_Clear(void)

{
  g_UiRuntimeRecordReadIndex = g_UiRuntimeRecordWriteIndex;
  return;
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
  return;
}


/* Sets up the UI runtime at startup: registers the 20 Hz frame-tick timer and the 125 Hz transfer-mailbox
   timer (TimerSystem_RegisterPeriodic takes a frequency), loads fonts and window resources, installs the
   in-game error handler and allocates the UI queues and the network transfer buffers. Every allocation
   failure is fatal.
*/
void UiRuntime_Initialize(void)

{
  uint32_t allocError;
  void *allocPayload;
  uint32_t checkedValue;

  g_TimerRegisterPeriodic(20,UiRuntime_IncrementPeriodicTickCounter);
  g_UiRuntimeInitializationCount++;
  FontRuntime_Init();
  UiWindowResources_Init();
  allocError = g_MemoryApi.alloc(UI_DIRTY_RECT_CAPACITY * sizeof(UiDirtyRectEntry),&allocPayload);
  checkedValue = FatalError_ExitIfFailed(allocError != 0 ? allocError : (uint32_t)allocPayload,allocError != 0);
  g_UiDirtyRectEntries = (UiDirtyRectEntry *)checkedValue;
  allocError = g_MemoryApi.alloc(UI_ACTION_QUEUE_BYTES,&allocPayload); /* 16 queued actions of 8 bytes */
  checkedValue = FatalError_ExitIfFailed(allocError != 0 ? allocError : (uint32_t)allocPayload,allocError != 0);
  g_UiActionQueueEntries = (UiActionQueueEntry *)checkedValue;
  /* from here on FatalError_ReportIfFailed shows errors in an in-game dialog */
  ErrorRuntime_InstallUiHandlerAndAllocateState();
  g_TimerRegisterPeriodic(125,UiTransferMailbox_ServiceAndRetransmitTimer);
  /* the sender-endpoint slots, parallel to the ring */
  allocError = g_MemoryApi.alloc(UI_RUNTIME_RECORD_RING_CAPACITY * UI_RUNTIME_RECORD_ENDPOINT_SLOT_SIZE,&allocPayload);
  checkedValue = FatalError_ExitIfFailed(allocError != 0 ? allocError : (uint32_t)allocPayload,allocError != 0);
  g_UiRuntimeRecordEndpointSlots = checkedValue;
  allocError = g_MemoryApi.alloc(UI_RUNTIME_RECORD_RING_CAPACITY * sizeof(UiRuntimeRecord),&allocPayload);
  checkedValue = FatalError_ExitIfFailed(allocError != 0 ? allocError : (uint32_t)allocPayload,allocError != 0);
  g_UiRuntimeRecordRing = (UiRuntimeRecord *)checkedValue;
  allocError = g_MemoryApi.alloc(UI_TRANSFER_ENDPOINT_BUFFER_BYTES,&allocPayload);
  checkedValue = FatalError_ExitIfFailed(allocError != 0 ? allocError : (uint32_t)allocPayload,allocError != 0);
  g_UiTransferEndpointBuffer = (UiTransferEndpointDescriptor *)checkedValue;
  allocError = g_MemoryApi.alloc(UI_TRANSFER_DATA_BUFFER_BYTES,&allocPayload);
  checkedValue = FatalError_ExitIfFailed(allocError != 0 ? allocError : (uint32_t)allocPayload,allocError != 0);
  g_UiTransferDataBuffer = (uint8_t *)checkedValue;
  g_UiRuntimeRecordWriteIndex = 0;
  g_UiRuntimeRecordReadIndex = 0;
  g_UiTransferUnitCursor = 0;
  return;
}


/* Counterpart of UiRuntime_Initialize at program end: stops the network receive timer and the frame-tick
   timer and frees the network rings/buffers, the dirty-rectangle list and the action queue. Does nothing if
   the UI runtime was never initialized.
*/
void UiRuntime_Shutdown(void)

{
  if (g_UiRuntimeInitializationCount != 0) {
    g_TimerUnregisterPeriodic(UiTransferMailbox_ServiceAndRetransmitTimer);
    g_MemoryApi.free(g_UiRuntimeRecordRing);
    g_MemoryApi.free(g_UiRuntimeRecordEndpointSlots);
    g_MemoryApi.free(g_UiTransferDataBuffer);
    g_MemoryApi.free(g_UiTransferEndpointBuffer);
    g_UiRuntimeRecordRing = NULL;
    g_UiRuntimeRecordEndpointSlots = NULL;
    g_UiTransferDataBuffer = NULL;
    g_UiTransferEndpointBuffer = NULL;
    g_MemoryApi.free(g_UiDirtyRectEntries);
    g_UiDirtyRectEntries = NULL;
    g_MemoryApi.free(g_UiActionQueueEntries);
    g_UiActionQueueEntries = NULL;
    g_TimerUnregisterPeriodic(UiRuntime_IncrementPeriodicTickCounter);
    g_UiRuntimeInitializationCount--;
  }
  return;
}

/* Frame-tick timer (20 Hz, registered by UiRuntime_Initialize): counts the pending frame ticks that the
   frame loop waits for and consumes.
*/
void __cdecl UiRuntime_IncrementPeriodicTickCounter(void)

{
  g_UiPendingFrameTicks++;
  return;
}

/* Runs the queued UI actions (button clicks, list selections, ...) in order, under the frame lock. An
   action id selects the handler page by its high byte (g_UiActionHandlerPages) and the handler by its low
   byte; the handler gets the control that queued it. Each entry is removed (the rest moved down) before its
   handler runs, so handlers may queue further actions.
*/
void __cdecl UiActionQueue_DispatchPending(void)

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
    actionHandler = (void (*)(void *))
                    g_UiActionHandlerPages[(uint32_t)queueHead->actionId >> 8]->handlers
                    [(uint32_t)queueHead->actionId & (UI_ACTION_HANDLER_PAGE_COUNT - 1)];
    sourceEntry = queueHead + 1;
    destinationEntry = queueHead;
    /* move entries 1..15 (30 dwords) down by one */
    for (remainingCount = 30; queueHead = g_UiActionQueueEntries, remainingCount != 0; remainingCount--) {
      destinationEntry->actionId = sourceEntry->actionId;
      sourceEntry = (UiActionQueueEntry *)&sourceEntry->source;
      destinationEntry = (UiActionQueueEntry *)&destinationEntry->source;
    }
    actionHandler(actionSource);
  }
  g_SpinLockReleaseAndInvoke
            ((SpinLockReleaseCallbackProc *)g_UiRuntimePostUnlockCallback,g_UiRuntimeFrameLock);
  return;
}

/* Default method04 vtable slot of the UI node classes: does nothing. Installed statically in 40
   UiNodeVtable tables; no caller of the slot is known yet.
*/
void UiNode_DefaultMethod04_NoOp(void *node)

{
  return;
}


/* Default nonRightPress (left/middle button press) of the UI node classes: ignores the
   press. Installed statically in 14 UiNodeVtable tables.
*/
void UiNode_DefaultNonRightPress(UiPointerWheelDelta wheelDelta,UiPixelCoordinate pointerY,UiPixelCoordinate pointerX,
          UiNodeBase *control)

{
  return;
}


/* Default nonRightRelease (left/middle button release) of the UI node classes: ignores
   the release. Installed statically in 25 UiNodeVtable tables.
*/
void UiNode_DefaultNonRightRelease(UiPointerWheelDelta wheelDelta,UiPixelCoordinate pointerY,UiPixelCoordinate pointerX,
          UiNodeBase *control)

{
  return;
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
  return;
}


/* Default rightRelease of the UI node classes: ignores the right-button release.
   Installed statically in 34 UiNodeVtable tables.
*/
void UiNode_DefaultRightRelease(UiPointerWheelDelta wheelDelta,UiPixelCoordinate pointerY,UiPixelCoordinate pointerX,
          UiNodeBase *control)

{
  return;
}


/* Default nonRightDrag (pointer motion while a left/middle press holds the capture) of the
   UI node classes: ignores it. Installed statically in 25 UiNodeVtable tables.
*/
void UiNode_DefaultNonRightDrag
               (UiPointerWheelDelta wheelDelta,UiPixelCoordinate pointerY,UiPixelCoordinate pointerX,
               UiNodeBase *control)

{
  return;
}

/* Default rightDrag (pointer motion while a right press holds the capture) of the UI
   node classes: ignores it. Installed statically in 34 UiNodeVtable tables.
*/
void UiNode_DefaultRightDrag(UiPointerWheelDelta wheelDelta,UiPixelCoordinate pointerY,UiPixelCoordinate pointerX,
          UiNodeBase *control)

{
  return;
}


/* Default applyFlags of the UI node classes: sets nodeFlags to
   (nodeFlags & retainMask) | setMask and passes the same masks to each direct child's applyFlags, so the change
   reaches the whole subtree. Installed statically in 39 UiNodeVtable tables; also called
   directly by UiLayoutContainerControl_ApplyFlagsRecursive once per page.
*/
void UiNode_ApplyFlagsRecursive(UiNodeFlagMask setMask,UiNodeFlagMask retainMask,UiNodeBase *control)

{
  UiNodeBase *childControl;
  
  control->nodeFlags = control->nodeFlags & retainMask;
  control->nodeFlags = control->nodeFlags | setMask;
  for (childControl = control->firstChild; childControl != UI_NODE_NONE;
      childControl = childControl->nextSibling) {
    childControl->vtable->applyFlags(setMask,retainMask,childControl);
  }
  return;
}


/* Default tick (per-frame update) of the UI node classes: does nothing. Installed
   statically in 30 UiNodeVtable tables.
*/
void UiNode_DefaultTick(UiNodeBase *control)

{
  return;
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
  return;
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
      node = (((UiRootNode *)node)->base).parent;
      parentNode = (((UiRootNode *)node)->base).parent;
    }
    if (g_UiDirtyRectCount < UI_DIRTY_RECT_CAPACITY) {
      dirtyRectEntry = g_UiDirtyRectEntries + g_UiDirtyRectCount;
      edgeCoordinate = (((UiRootNode *)node)->base).right;
      dirtyRectEntry->left = (((UiRootNode *)node)->base).left;
      dirtyRectEntry->right = edgeCoordinate;
      bottomEdgeCoordinate = (((UiRootNode *)node)->base).bottom;
      dirtyRectEntry->top = (((UiRootNode *)node)->base).top;
      dirtyRectEntry->bottom = bottomEdgeCoordinate;
      dirtyRectEntry->rootNode = (UiRootNode *)node;
      dirtyRectEntry->rootNodeCopy = (UiRootNode *)node;
      g_UiDirtyRectCount++;
    }
  }
  return;
}


/* Queues action actionId of the control source for UiActionQueue_DispatchPending (at the end of the
   frame). UI_ACTION_NONE and actions beyond the 16 queue entries are dropped.
*/
void UiActionQueue_Enqueue(UiActionId actionId,void *source)

{
  UiActionQueueEntry *destinationEntry;

  if (g_UiActionQueueUsedBytes < UI_ACTION_QUEUE_BYTES) {
    destinationEntry = (UiActionQueueEntry *)((uint8_t *)g_UiActionQueueEntries + g_UiActionQueueUsedBytes);
    if (actionId != UI_ACTION_NONE) {
      destinationEntry->actionId = actionId;
      destinationEntry->source = source;
      g_UiActionQueueUsedBytes = g_UiActionQueueUsedBytes + sizeof(UiActionQueueEntry);
    }
  }
  return;
}


/* Tint of a world model (both callers pass a ModelRuntimeNode) from its terrain-derived runtimeFlags: 0x04 ->
   opaque white (unchanged colours), else without 0x08 -> 0 (black), with 0x08 and 0x10 -> 0x00FFFFFF, with
   0x08 only -> opaque grey 0x878787.
*/
PackedArgb32 ModelRuntimeNode_GetStateTintArgb(ModelRuntimeNode *node)

{
  ModelRuntimeFlags stateFlags;

  stateFlags = node->runtimeFlags;
  if ((stateFlags & TERRAIN_OCCUPANCY_FLAG_PRESENT) != 0) {
    return UI_MODEL_TINT_OPAQUE_WHITE;
  }
  if ((stateFlags & TERRAIN_OCCUPANCY_FLAG_SEEN_BEFORE) == 0) {
    return 0;
  }
  if ((stateFlags & TERRAIN_OCCUPANCY_FLAG_NOT_REMEMBERED) != 0) {
    return UI_MODEL_TINT_TRANSPARENT_WHITE;
  }
  return UI_MODEL_TINT_OPAQUE_GREY;
}

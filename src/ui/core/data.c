/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/src/ui/core/data.c
 */

/* Data of the original image that this module uses (moved here from the generated image data in
   step 4c); declared in <thandor/ui/core/data.h>. */

#include <thandor/thandor.h>

#pragma warning(disable : 4152) /* function pointer fields initialized through (void *) */

__declspec(align(16)) UiRootCallbacks g_UiDisplaySettingsRootCallbacks = {
    .vetoClose = (void *)UiRootCallbacks_Free,
    .frameUpdate = (void *)UiDisplaySettingsRoot_RefreshModeSelection,
    .method08 = (void *)UiModalDialogRoot_BlockMissedPointerPress,
    .pointerMissPolicy = (void *)UiModalDialogRoot_BlockMissedPointerMotion};

__declspec(align(4)) UiRootCallbacks g_UiFourValueDialogRootCallbacks = {
    .vetoClose = (void *)UiRootCallbacks_Free,
    .frameUpdate = (void *)UiFourValueDialog_TickCountdownAndRequestClose,
    .method08 = (void *)UiModalDialogRoot_BlockMissedPointerPress,
    .pointerMissPolicy = (void *)UiModalDialogRoot_BlockMissedPointerMotion};

__declspec(align(8)) FourValueDialogUiImage g_UiFourValueDialogTemplateImage = {
        { /* +0000 confirmModeDialogPanel g_UiPanelControlVtable */
            .nextSibling = UI_TEMPLATE_NO_LINK, .firstChild = UI_TEMPLATE_LINK(0x58), .parent = UI_TEMPLATE_NO_LINK,
            .vtable = (void *)&g_UiPanelControlVtable,
            .leftOffset = -128, .topOffset = -48, .rightOffset = 128, .bottomOffset = 48,
            .leftAnchorQ31 = 0x40000000, .topAnchorQ31 = 0x40000000, .rightAnchorQ31 = 0x40000000, .bottomAnchorQ31 = 0x40000000,
            .layoutWidth = -1, .layoutHeight = -1, .nodeFlags = 0x21},
        {
            0x00000003},
        { /* +0058 revertButton g_UiFramedTextButtonControlVtable */
            .nextSibling = UI_TEMPLATE_LINK(0xB4), .firstChild = UI_TEMPLATE_NO_LINK, .parent = UI_TEMPLATE_LINK(0x0),
            .vtable = (void *)&g_UiFramedTextButtonControlVtable,
            .leftOffset = 16, .topOffset = -32, .rightOffset = 112, .bottomOffset = -8,
            .topAnchorQ31 = 0x80000000, .bottomAnchorQ31 = 0x80000000,
            .layoutWidth = -1, .layoutHeight = -1, .nodeFlags = 0x2},
        {
            0x00000008, 0x0000020D, 0x00000101},
        { /* +00B4 keepModeButton g_UiFramedTextButtonControlVtable */
            .nextSibling = UI_TEMPLATE_LINK(0x110), .firstChild = UI_TEMPLATE_NO_LINK, .parent = UI_TEMPLATE_LINK(0x0),
            .vtable = (void *)&g_UiFramedTextButtonControlVtable,
            .leftOffset = 128, .topOffset = -32, .rightOffset = 240, .bottomOffset = -8,
            .topAnchorQ31 = 0x80000000, .bottomAnchorQ31 = 0x80000000,
            .layoutWidth = -1, .layoutHeight = -1, .nodeFlags = 0x20},
        {
            0x00000004, 0x00000000, 0x00000100},
        { /* +0110 countdownMessageText g_UiListOffsetControlVtable */
            .nextSibling = UI_TEMPLATE_NO_LINK, .firstChild = UI_TEMPLATE_NO_LINK, .parent = UI_TEMPLATE_LINK(0x0),
            .vtable = (void *)&g_UiListOffsetControlVtable,
            .leftOffset = 8, .topOffset = 8, .rightOffset = -8, .bottomOffset = -40,
            .rightAnchorQ31 = 0x80000000, .bottomAnchorQ31 = 0x80000000,
            .layoutWidth = -1, .layoutHeight = -1},
        {
            0x00000004, 0x00000000, 0x00000109, 0x00000000, 0x0000000F, 0x00000014},
};

__declspec(align(16)) UiRuntimeRecord *g_UiRuntimeRecordRing = 0;

__declspec(align(4)) uint32_t g_UiRuntimeRecordEndpointSlots = 0;

__declspec(align(8)) uint8_t *g_UiTransferDataBuffer = 0;

__declspec(align(4)) UiTransferEndpointDescriptor *g_UiTransferEndpointBuffer = 0;

__declspec(align(16)) uint32_t g_UiRuntimeRecordReadIndex = 0;

__declspec(align(4)) RuntimeSpinLockValue g_UiRuntimeRecordRingLock = 0;

__declspec(align(4)) UiDirtyRectCount g_UiDirtyRectCount = 0;

__declspec(align(8)) UiDirtyRectEntry *g_UiDirtyRectEntries = 0;

__declspec(align(4)) UiActionQueueUsedBytes g_UiActionQueueUsedBytes = 0;

__declspec(align(16)) UiActionQueueEntry *g_UiActionQueueEntries = 0;

__declspec(align(4)) uint32_t g_UiRuntimeInitializationCount = 0;

__declspec(align(16)) UiActionHandlerPage *g_UiActionHandlerPages[256] = {0};

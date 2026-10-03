/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/include/thandor/ui/core/data.h
 */

#ifndef THANDOR_UI_CORE_DATA_H
#define THANDOR_UI_CORE_DATA_H

#include <thandor/generated/types.h>
#include <thandor/generated/ui_templates.h>

extern UiRootCallbacks g_UiDisplaySettingsRootCallbacks;

extern UiRootCallbacks g_UiFourValueDialogRootCallbacks;

extern FourValueDialogUiImage g_UiFourValueDialogTemplateImage;

extern UiRuntimeRecord *g_UiRuntimeRecordRing;

extern uint32_t g_UiRuntimeRecordEndpointSlots;

extern uint8_t *g_UiTransferDataBuffer;

extern UiTransferEndpointDescriptor *g_UiTransferEndpointBuffer;

extern uint32_t g_UiRuntimeRecordReadIndex;

extern RuntimeSpinLockValue g_UiRuntimeRecordRingLock;

extern UiDirtyRectCount g_UiDirtyRectCount;

extern UiDirtyRectEntry *g_UiDirtyRectEntries;

extern UiActionQueueUsedBytes g_UiActionQueueUsedBytes;

extern UiActionQueueEntry *g_UiActionQueueEntries;

extern uint32_t g_UiRuntimeInitializationCount;

extern UiActionHandlerPage *g_UiActionHandlerPages[256];

#endif

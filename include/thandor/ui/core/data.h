/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/include/thandor/ui/core/data.h
 */

#ifndef THANDOR_UI_CORE_DATA_H
#define THANDOR_UI_CORE_DATA_H

#include <thandor/generated/types.h>
#include <thandor/generated/ui_templates.h>

extern UiRootCallbacks g_UiDisplaySettingsRootCallbacks; /* 004229A0 g_UiDisplaySettingsRootCallbacks */

extern UiRootCallbacks g_UiFourValueDialogRootCallbacks; /* 00424324 g_UiFourValueDialogRootCallbacks */

extern FourValueDialogUiImage g_UiFourValueDialogTemplateImage; /* 00424338 g_UiFourValueDialogTemplateImage */

extern UiRuntimeRecord *g_UiRuntimeRecordRing; /* 004AE960 g_UiRuntimeRecordRing */

extern uint32_t g_UiRuntimeRecordEndpointSlots; /* 004AE964 g_UiRuntimeRecordEndpointSlots */

extern uint8_t *g_UiTransferDataBuffer; /* 004AE968 g_UiTransferDataBuffer */

extern UiTransferEndpointDescriptor *g_UiTransferEndpointBuffer; /* 004AE96C g_UiTransferEndpointBuffer */

extern uint32_t g_UiRuntimeRecordReadIndex; /* 004AE970 g_UiRuntimeRecordReadIndex */

extern RuntimeSpinLockValue g_UiRuntimeRecordRingLock; /* 004AE98C g_UiRuntimeRecordRingLock */

extern UiDirtyRectCount g_UiDirtyRectCount; /* 004AF1F4 g_UiDirtyRectCount */

extern UiDirtyRectEntry *g_UiDirtyRectEntries; /* 004AF1F8 g_UiDirtyRectEntries */

extern UiActionQueueUsedBytes g_UiActionQueueUsedBytes; /* 004AF1FC g_UiActionQueueUsedBytes */

extern UiActionQueueEntry *g_UiActionQueueEntries; /* 004AF200 g_UiActionQueueEntries */

extern uint32_t g_UiRuntimeInitializationCount; /* 004AF204 g_UiRuntimeInitializationCount */

extern UiActionHandlerPage *g_UiActionHandlerPages[256]; /* 004B0A30 g_UiActionHandlerPages */

#endif

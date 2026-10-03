/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/include/thandor/core/error/data.h
 */

#ifndef THANDOR_CORE_ERROR_DATA_H
#define THANDOR_CORE_ERROR_DATA_H

#include <thandor/generated/types.h>
#include <thandor/generated/ui_templates.h>

extern uint16_t g_FatalErrorDetail2Utf16[256];

extern uint16_t g_FatalErrorDetail3Utf16[256];

extern FatalErrorPassThroughProc *g_FatalErrorExitHandler;

extern FatalErrorPassThroughProc *g_FatalErrorReportHandler;

extern FatalErrorPassThroughProc *g_FatalErrorFallbackHandler;

extern uint16_t u_texte_error_str_00407d20[16];

extern uint16_t g_ErrorTextIoInitializationFailed[34];

extern UiRootNode *g_FatalErrorUiRootTemplate;

extern uint32_t g_FatalErrorDialogDismissed;

extern UiRootCallbacks g_UiRootCallbacks_00407E28;

extern FatalErrorUiImage g_FatalErrorUiRootTemplateImage;

extern UiRootStackActionHandlerPage2 g_UiRootStackActionHandlerPage;

extern uint8_t g_FatalErrorNarrowBuffer[1024];

#endif

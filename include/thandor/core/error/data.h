/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/include/thandor/core/error/data.h
 */

#ifndef THANDOR_CORE_ERROR_DATA_H
#define THANDOR_CORE_ERROR_DATA_H

#include <thandor/generated/types.h>
#include <thandor/generated/ui_templates.h>

extern uint16_t g_FatalErrorDetail2Utf16[256]; /* 00407914 g_FatalErrorDetail2Utf16 */

extern uint16_t g_FatalErrorDetail3Utf16[256]; /* 00407B14 g_FatalErrorDetail3Utf16 */

extern FatalErrorPassThroughProc *g_FatalErrorExitHandler; /* 00407D14 g_FatalErrorExitHandler */

extern FatalErrorPassThroughProc *g_FatalErrorReportHandler; /* 00407D18 g_FatalErrorReportHandler */

extern FatalErrorPassThroughProc *g_FatalErrorFallbackHandler; /* 00407D1C g_FatalErrorFallbackHandler */

extern uint16_t u_texte_error_str_00407d20[16]; /* 00407D20 u_texte_error_str_00407d20 */

extern uint16_t g_ErrorTextIoInitializationFailed[34]; /* 00407D40 g_ErrorTextIoInitializationFailed */

extern UiRootNode *g_FatalErrorUiRootTemplate; /* 00407E20 g_FatalErrorUiRootTemplate */

extern uint32_t g_FatalErrorDialogDismissed; /* 00407E24 g_FatalErrorDialogDismissed */

extern UiRootCallbacks g_UiRootCallbacks_00407E28; /* 00407E28 g_UiRootCallbacks_00407E28 */

extern FatalErrorUiImage g_FatalErrorUiRootTemplateImage; /* 00407E3C g_FatalErrorUiRootTemplateImage */

extern UiRootStackActionHandlerPage2 g_UiRootStackActionHandlerPage; /* 004B0EB0 g_UiRootStackActionHandlerPage */

extern uint8_t g_FatalErrorNarrowBuffer[1024]; /* 00575480 g_FatalErrorNarrowBuffer */

#endif

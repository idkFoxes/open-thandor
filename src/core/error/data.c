/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/src/core/error/data.c
 */

/* Data of the original image that this module uses (moved here from the generated image data in
   step 4c); declared in <thandor/core/error/data.h>. Original addresses in the comments. */

#include <thandor/thandor.h>

#pragma warning(disable : 4152) /* function pointer fields initialized through (void *) */

/* 00407914 g_FatalErrorDetail2Utf16 */
__declspec(align(4)) uint16_t g_FatalErrorDetail2Utf16[256] = {0};

/* 00407B14 g_FatalErrorDetail3Utf16 */
__declspec(align(4)) uint16_t g_FatalErrorDetail3Utf16[256] = {0};

/* 00407D14 g_FatalErrorExitHandler */
__declspec(align(4)) FatalErrorPassThroughProc *g_FatalErrorExitHandler = 0;

/* 00407D18 g_FatalErrorReportHandler */
__declspec(align(8)) FatalErrorPassThroughProc *g_FatalErrorReportHandler = 0;

/* 00407D1C g_FatalErrorFallbackHandler */
__declspec(align(4)) FatalErrorPassThroughProc *g_FatalErrorFallbackHandler = 0;

/* 00407D20 u_texte_error_str_00407d20 */
__declspec(align(16)) uint16_t u_texte_error_str_00407d20[16] = L"texte\\error.str";

/* 00407D40 g_ErrorTextIoInitializationFailed */
__declspec(align(16)) uint16_t g_ErrorTextIoInitializationFailed[34] = L"error: IO: initialization failed!";

/* 00407E20 g_FatalErrorUiRootTemplate */
__declspec(align(16)) UiRootNode *g_FatalErrorUiRootTemplate = 0;

/* 00407E24 g_FatalErrorDialogDismissed */
__declspec(align(4)) uint32_t g_FatalErrorDialogDismissed = 0;

/* 00407E28 g_UiRootCallbacks_00407E28 */
__declspec(align(8)) UiRootCallbacks g_UiRootCallbacks_00407E28 = {
    .method08 = (void *)FatalErrorDialog_BlockMissedPointerPress,
    .pointerMissPolicy = (void *)FatalErrorDialog_BlockMissedPointerMotion};

/* 00407E3C g_FatalErrorUiRootTemplateImage */
__declspec(align(4)) FatalErrorUiImage g_FatalErrorUiRootTemplateImage = {
        { /* +0000 fatalErrorPanel g_UiPanelControlVtable */
            .nextSibling = UI_TEMPLATE_NO_LINK, .firstChild = UI_TEMPLATE_LINK(0x58), .parent = UI_TEMPLATE_NO_LINK,
            .vtable = (void *)&g_UiPanelControlVtable,
            .left = -1, .top = -1, .right = -1, .bottom = -1,
            .leftOffset = -160, .rightOffset = 160, .bottomOffset = 44,
            .leftAnchorQ31 = 0x50000000, .topAnchorQ31 = 0x50000000, .rightAnchorQ31 = 0x50000000, .bottomAnchorQ31 = 0x50000000,
            .layoutWidth = -1, .layoutHeight = -1},
        {
            0x00000003, 0xFFFFFFFF, 0xFFFFFFFF},
        { /* +0058 errorMessageText g_UiListOffsetControlVtable */
            .nextSibling = UI_TEMPLATE_LINK(0xB4), .firstChild = UI_TEMPLATE_NO_LINK, .parent = UI_TEMPLATE_LINK(0x0),
            .vtable = (void *)&g_UiListOffsetControlVtable,
            .leftOffset = 6, .topOffset = 6, .rightOffset = -6, .bottomOffset = -38,
            .rightAnchorQ31 = 0x80000000, .bottomAnchorQ31 = 0x80000000,
            .layoutWidth = -1, .layoutHeight = -1},
        {
            0x00000015},
        { /* +00B4 okButton g_UiNodeVtable_004B1D80 */
            .nextSibling = UI_TEMPLATE_NO_LINK, .firstChild = UI_TEMPLATE_NO_LINK, .parent = UI_TEMPLATE_LINK(0x0),
            .vtable = (void *)&g_UiNodeVtable_004B1D80,
            .leftOffset = -108, .topOffset = -32, .rightOffset = -12, .bottomOffset = -6,
            .leftAnchorQ31 = 0x80000000, .topAnchorQ31 = 0x80000000, .rightAnchorQ31 = 0x80000000, .bottomAnchorQ31 = 0x80000000,
            .layoutWidth = -1, .layoutHeight = -1, .nodeFlags = 0x2},
        {
            0x0000000C, 0x00000001, 0x00000100},
};

/* 004B0EB0 g_UiRootStackActionHandlerPage */
__declspec(align(16)) UiRootStackActionHandlerPage2 g_UiRootStackActionHandlerPage = {.handlers = {(void *)UiRootStack_Pop, (void *)FatalErrorDialog_DismissAndPopRoot}};

/* 00575480 g_FatalErrorNarrowBuffer */
__declspec(align(16)) uint8_t g_FatalErrorNarrowBuffer[1024] = {0};

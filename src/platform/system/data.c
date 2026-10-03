/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/src/platform/system/data.c
 */

/* Data of the original image that this module uses (moved here from the generated image data in
   step 4c); declared in <thandor/platform/system/data.h>. Original addresses in the comments. */

#include <thandor/thandor.h>

#pragma warning(disable : 4152) /* function pointer fields initialized through (void *) */

/* 00402020 g_Win32PumpMessages */
__declspec(align(16)) Win32PumpMessagesProc *g_Win32PumpMessages = 0;

/* 00402628 g_WideNumberFormatUtf16 */
__declspec(align(8)) WideNumberFormatUtf16Proc *g_WideNumberFormatUtf16 = (void *)WideNumber_FormatUtf16;

/* 00402794 g_LocaleFormatDateFieldsUtf16 */
__declspec(align(4)) LocaleFormatDateFieldsUtf16Proc *g_LocaleFormatDateFieldsUtf16 = 0;

/* 00402798 g_LocaleFormatCurrentDateUtf16 */
__declspec(align(8)) LocaleFormatCurrentDateUtf16Proc *g_LocaleFormatCurrentDateUtf16 = 0;

/* 004027A0 g_LocaleFormatTimeFieldsUtf16 */
__declspec(align(16)) LocaleFormatTimeFieldsUtf16Proc *g_LocaleFormatTimeFieldsUtf16 = 0;

/* 004027A4 g_LocaleFormatCurrentTimeUtf16 */
__declspec(align(4)) LocaleFormatCurrentTimeUtf16Proc *g_LocaleFormatCurrentTimeUtf16 = 0;

/* 004027AC g_LocaleGetDefaultTelephoneCountryCode */
__declspec(align(4)) LocaleGetTelephoneCountryCodeProc *g_LocaleGetDefaultTelephoneCountryCode = 0;

/* 004027B8 g_CPUDetectFeatures */
__declspec(align(8)) CpuDetectFeaturesProc *g_CPUDetectFeatures = 0;

/* 005856B8 g_WindowDestroyDepth */
__declspec(align(8)) uint32_t g_WindowDestroyDepth = 0;

/* 0058571C g_TimerSystemState */
__declspec(align(4)) TimerSystemState g_TimerSystemState = {0};

/* 00586950 g_LocaleInfoScratch */
__declspec(align(16)) uint8_t g_LocaleInfoScratch[16] = {0};

/* 00586A70 g_LocaleSystemState */
__declspec(align(16)) LocaleSystemState g_LocaleSystemState = {0};

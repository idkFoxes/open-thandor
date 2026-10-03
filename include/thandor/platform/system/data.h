/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/include/thandor/platform/system/data.h
 */

#ifndef THANDOR_PLATFORM_SYSTEM_DATA_H
#define THANDOR_PLATFORM_SYSTEM_DATA_H

#include <thandor/generated/types.h>
#include <thandor/generated/ui_templates.h>

extern Win32PumpMessagesProc *g_Win32PumpMessages; /* 00402020 g_Win32PumpMessages */

extern WideNumberFormatUtf16Proc *g_WideNumberFormatUtf16; /* 00402628 g_WideNumberFormatUtf16 */

extern LocaleFormatDateFieldsUtf16Proc *g_LocaleFormatDateFieldsUtf16; /* 00402794 g_LocaleFormatDateFieldsUtf16 */

extern LocaleFormatCurrentDateUtf16Proc *g_LocaleFormatCurrentDateUtf16; /* 00402798 g_LocaleFormatCurrentDateUtf16 */

extern LocaleFormatTimeFieldsUtf16Proc *g_LocaleFormatTimeFieldsUtf16; /* 004027A0 g_LocaleFormatTimeFieldsUtf16 */

extern LocaleFormatCurrentTimeUtf16Proc *g_LocaleFormatCurrentTimeUtf16; /* 004027A4 g_LocaleFormatCurrentTimeUtf16 */

extern LocaleGetTelephoneCountryCodeProc *g_LocaleGetDefaultTelephoneCountryCode; /* 004027AC g_LocaleGetDefaultTelephoneCountryCode */

extern CpuDetectFeaturesProc *g_CPUDetectFeatures; /* 004027B8 g_CPUDetectFeatures */

extern uint32_t g_WindowDestroyDepth; /* 005856B8 g_WindowDestroyDepth */

extern TimerSystemState g_TimerSystemState; /* 0058571C g_TimerSystemState */

extern uint8_t g_LocaleInfoScratch[16]; /* 00586950 g_LocaleInfoScratch */

extern LocaleSystemState g_LocaleSystemState; /* 00586A70 g_LocaleSystemState */

#endif

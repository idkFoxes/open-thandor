/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/include/thandor/platform/system/data.h
 */

#ifndef THANDOR_PLATFORM_SYSTEM_DATA_H
#define THANDOR_PLATFORM_SYSTEM_DATA_H

#include <thandor/generated/types.h>
#include <thandor/generated/ui_templates.h>

extern Win32PumpMessagesProc *g_Win32PumpMessages;

extern WideNumberFormatUtf16Proc *g_WideNumberFormatUtf16;

extern LocaleFormatDateFieldsUtf16Proc *g_LocaleFormatDateFieldsUtf16;

extern LocaleFormatCurrentDateUtf16Proc *g_LocaleFormatCurrentDateUtf16;

extern LocaleFormatTimeFieldsUtf16Proc *g_LocaleFormatTimeFieldsUtf16;

extern LocaleFormatCurrentTimeUtf16Proc *g_LocaleFormatCurrentTimeUtf16;

extern LocaleGetTelephoneCountryCodeProc *g_LocaleGetDefaultTelephoneCountryCode;

extern CpuDetectFeaturesProc *g_CPUDetectFeatures;

extern uint32_t g_WindowDestroyDepth;

extern TimerSystemState g_TimerSystemState;

extern uint8_t g_LocaleInfoScratch[16];

extern LocaleSystemState g_LocaleSystemState;

#endif

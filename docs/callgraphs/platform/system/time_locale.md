# Call graph: `platform/system/time_locale`

[Source](../../../../src/platform/system/time_locale.c) · [Header](../../../../include/thandor/platform/system/time_locale.h) · [Graphviz](time_locale.dot) · [Module tree](../../../MODULE_TREE.md) · [Source guide](../../../SOURCE_FILE_GUIDE.md#module-platform-system-time-locale) · [Developer notes](../../../../CHANGELOG.md#module-platform-system-time-locale) · [Full changelog](../../../../CHANGELOG_FULL.md#module-platform-system-time-locale)

16 functions. Addresses are current V523 executable VAs.

## Functions

- `0x005867B0` **[`TimerSystem_Shutdown`](../../../../src/platform/system/time_locale.c#L5)** — local: [`TimerSystem_UnregisterPeriodic`](../../../../src/platform/system/time_locale.c#L543)
- `0x00586BA0` **[`Locale_Init`](../../../../src/platform/system/time_locale.c#L28)** — local: [`Locale_ParseUnsignedDecimalAscii`](../../../../src/platform/system/time_locale.c#L568); cross: [`CPU_DetectFeatures`](../../../../src/platform/bootstrap/runtime.c#L491) → [`platform/bootstrap/runtime`](../bootstrap/runtime.md), [`Text_CopyNarrowToUtf16Cf`](../../../../src/core/text/string.c#L185) → [`core/text/string`](../../core/text/string.md)
- `0x00402F70` **[`Locale_MapTelephoneCountryCodeToRegionTagPacked`](../../../../src/platform/system/time_locale.c#L75)**
- `0x00586790` **[`TimerSystem_Init`](../../../../src/platform/system/time_locale.c#L125)**
- `0x005867E0` **[`WinMM_TimerDispatchCallback`](../../../../src/platform/system/time_locale.c#L139)**
- `0x00586820` **[`TimerSystem_RegisterPeriodic`](../../../../src/platform/system/time_locale.c#L158)**
- `0x00586DD0` **[`Locale_FormatDateFieldsUtf16`](../../../../src/platform/system/time_locale.c#L190)** — cross: [`Utf16_CopyAndReturnByteLength`](../../../../src/core/text/string.c#L215) → [`core/text/string`](../../core/text/string.md)
- `0x00586F10` **[`Locale_FormatCurrentDateUtf16`](../../../../src/platform/system/time_locale.c#L246)** — cross: [`Utf16_CopyAndReturnByteLength`](../../../../src/core/text/string.c#L215) → [`core/text/string`](../../core/text/string.md)
- `0x00587080` **[`Locale_GetPackedCurrentDate`](../../../../src/platform/system/time_locale.c#L332)**
- `0x005870C0` **[`Locale_FormatTimeFieldsUtf16`](../../../../src/platform/system/time_locale.c#L344)** — cross: [`Utf16_CopyAndReturnByteLength`](../../../../src/core/text/string.c#L215) → [`core/text/string`](../../core/text/string.md)
- `0x005871B0` **[`Locale_FormatCurrentTimeUtf16`](../../../../src/platform/system/time_locale.c#L402)** — cross: [`Utf16_CopyAndReturnByteLength`](../../../../src/core/text/string.c#L215) → [`core/text/string`](../../core/text/string.md)
- `0x005872B0` **[`Locale_GetPackedCurrentTime`](../../../../src/platform/system/time_locale.c#L471)**
- `0x005872F0` **[`Locale_GetDefaultTelephoneCountryCode`](../../../../src/platform/system/time_locale.c#L484)**
- `0x00587350` **[`Locale_CopyDefaultComputerLabelUtf16`](../../../../src/platform/system/time_locale.c#L522)**
- `0x00586880` **[`TimerSystem_UnregisterPeriodic`](../../../../src/platform/system/time_locale.c#L543)**
- `0x00586B70` **[`Locale_ParseUnsignedDecimalAscii`](../../../../src/platform/system/time_locale.c#L568)**

## Called by

- [`core/memory/synchronization`](../../core/memory/synchronization.md): [`Runtime_Shutdown`](../../../../src/core/memory/synchronization.c#L79) → [`TimerSystem_Shutdown`](../../../../src/platform/system/time_locale.c#L5)
- [`platform/bootstrap/runtime`](../bootstrap/runtime.md): [`ProcessEntry`](../../../../src/platform/bootstrap/runtime.c#L5) → [`Locale_Init`](../../../../src/platform/system/time_locale.c#L28); [`ProcessEntry`](../../../../src/platform/bootstrap/runtime.c#L5) → [`TimerSystem_Init`](../../../../src/platform/system/time_locale.c#L125)
- [`platform/input/devices`](../input/devices.md): [`DirectInputMouse_Init`](../../../../src/platform/input/devices.c#L81) → [`TimerSystem_RegisterPeriodic`](../../../../src/platform/system/time_locale.c#L158); [`DirectInputMouse_Shutdown`](../../../../src/platform/input/devices.c#L246) → [`TimerSystem_UnregisterPeriodic`](../../../../src/platform/system/time_locale.c#L543)

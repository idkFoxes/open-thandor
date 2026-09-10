# Call graph: `platform/system/time_locale`

16 functions. Addresses are current V523 executable VAs.

## Functions

- `0x005867B0` **TimerSystem_Shutdown** — local: `TimerSystem_UnregisterPeriodic`
- `0x00586BA0` **Locale_Init** — local: `Locale_ParseUnsignedDecimalAscii`; cross: `CPU_DetectFeatures` → `platform/bootstrap/runtime`, `Text_CopyNarrowToUtf16Cf` → `core/text/string`
- `0x00402F70` **Locale_MapTelephoneCountryCodeToRegionTagPacked**
- `0x00586790` **TimerSystem_Init**
- `0x005867E0` **WinMM_TimerDispatchCallback**
- `0x00586820` **TimerSystem_RegisterPeriodic**
- `0x00586DD0` **Locale_FormatDateFieldsUtf16** — cross: `Utf16_CopyAndReturnByteLength` → `core/text/string`
- `0x00586F10` **Locale_FormatCurrentDateUtf16** — cross: `Utf16_CopyAndReturnByteLength` → `core/text/string`
- `0x00587080` **Locale_GetPackedCurrentDate**
- `0x005870C0` **Locale_FormatTimeFieldsUtf16** — cross: `Utf16_CopyAndReturnByteLength` → `core/text/string`
- `0x005871B0` **Locale_FormatCurrentTimeUtf16** — cross: `Utf16_CopyAndReturnByteLength` → `core/text/string`
- `0x005872B0` **Locale_GetPackedCurrentTime**
- `0x005872F0` **Locale_GetDefaultTelephoneCountryCode**
- `0x00587350` **Locale_CopyDefaultComputerLabelUtf16**
- `0x00586880` **TimerSystem_UnregisterPeriodic**
- `0x00586B70` **Locale_ParseUnsignedDecimalAscii**

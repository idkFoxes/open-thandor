# Call graph: `platform/input/devices`

[Source](../../../../src/platform/input/devices.c) · [Header](../../../../include/thandor/platform/input/devices.h) · [Graphviz](devices.dot) · [Module tree](../../../MODULE_TREE.md) · [Source guide](../../../SOURCE_FILE_GUIDE.md#module-platform-input-devices) · [Full changelog](../../../../CHANGELOG_FULL.md#module-platform-input-devices)

15 functions. Addresses are current V523 executable VAs.

## Functions

- `0x00417280` **[`Keyboard_CompareAsciiCaseInsensitiveFlags`](../../../../src/platform/input/devices.c#L5)** — local: [`Keyboard_ToUpperAscii`](../../../../src/platform/input/devices.c#L718)
- `0x00417230` **[`Keyboard_FlushEvents`](../../../../src/platform/input/devices.c#L26)**
- `0x00417240` **[`Keyboard_ReadNextEventRegs`](../../../../src/platform/input/devices.c#L37)**
- `0x004172D0` **[`Keyboard_ToLowerAscii`](../../../../src/platform/input/devices.c#L65)**
- `0x00576CF0` **[`DirectInputMouse_Init`](../../../../src/platform/input/devices.c#L81)** — cross: [`DynDLL_Load`](../../../../src/platform/bootstrap/runtime.c#L294) → [`platform/bootstrap/runtime`](../bootstrap/runtime.md), [`DynAPI_Resolve`](../../../../src/platform/bootstrap/runtime.c#L259) → [`platform/bootstrap/runtime`](../bootstrap/runtime.md), [`TimerSystem_RegisterPeriodic`](../../../../src/platform/system/time_locale.c#L158) → [`platform/system/time_locale`](../system/time_locale.md), [`Package_LoadEntry`](../../../../src/assets/package/runtime.c#L402) → [`assets/package/runtime`](../../assets/package/runtime.md), [`Resource_Load`](../../../../src/assets/resource/runtime.c#L66) → [`assets/resource/runtime`](../../assets/resource/runtime.md)
- `0x00576F20` **[`DirectInputMouse_RefreshDeviceIfIdle`](../../../../src/platform/input/devices.c#L202)**
- `0x00577000` **[`DirectInputMouse_Shutdown`](../../../../src/platform/input/devices.c#L246)** — cross: [`TimerSystem_UnregisterPeriodic`](../../../../src/platform/system/time_locale.c#L543) → [`platform/system/time_locale`](../system/time_locale.md)
- `0x00577080` **[`DirectInputMouse_PollBufferedEvents`](../../../../src/platform/input/devices.c#L271)**
- `0x005772F0` **[`DirectInputMouse_DisplayModeHookCf`](../../../../src/platform/input/devices.c#L412)**
- `0x00577420` **[`DirectInputMouse_SetPosition`](../../../../src/platform/input/devices.c#L470)**
- `0x00577460` **[`DirectInputMouse_FlushBufferedEvents`](../../../../src/platform/input/devices.c#L491)**
- `0x005774A0` **[`Keyboard_OnKeyDown`](../../../../src/platform/input/devices.c#L506)**
- `0x00577880` **[`Keyboard_OnKeyUp`](../../../../src/platform/input/devices.c#L611)**
- `0x00577B30` **[`Keyboard_OnChar`](../../../../src/platform/input/devices.c#L689)**
- `0x004172B0` **[`Keyboard_ToUpperAscii`](../../../../src/platform/input/devices.c#L718)**

## Called by

- [`core/memory/synchronization`](../../core/memory/synchronization.md): [`Runtime_Shutdown`](../../../../src/core/memory/synchronization.c#L79) → [`DirectInputMouse_Shutdown`](../../../../src/platform/input/devices.c#L246)
- [`platform/bootstrap/runtime`](../bootstrap/runtime.md): [`ProcessEntry`](../../../../src/platform/bootstrap/runtime.c#L5) → [`DirectInputMouse_Init`](../../../../src/platform/input/devices.c#L81); [`MainWindowProc`](../../../../src/platform/bootstrap/runtime.c#L410) → [`Keyboard_OnKeyDown`](../../../../src/platform/input/devices.c#L506); [`MainWindowProc`](../../../../src/platform/bootstrap/runtime.c#L410) → [`Keyboard_OnKeyUp`](../../../../src/platform/input/devices.c#L611); [`MainWindowProc`](../../../../src/platform/bootstrap/runtime.c#L410) → [`Keyboard_OnChar`](../../../../src/platform/input/devices.c#L689)
- [`ui/controls/input`](../../ui/controls/input.md): [`UiPointer_DispatchPendingEvents`](../../../../src/ui/controls/input.c#L5) → [`DirectInputMouse_PollBufferedEvents`](../../../../src/platform/input/devices.c#L271)
- [`ui/controls/layout`](../../ui/controls/layout.md): [`UiFrame_Update`](../../../../src/ui/controls/layout.c#L1592) → [`DirectInputMouse_RefreshDeviceIfIdle`](../../../../src/platform/input/devices.c#L202)

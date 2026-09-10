# Call graph: `platform/input/devices`

15 functions. Addresses are current V523 executable VAs.

## Functions

- `0x00417280` **Keyboard_CompareAsciiCaseInsensitiveFlags** — local: `Keyboard_ToUpperAscii`
- `0x00417230` **Keyboard_FlushEvents**
- `0x00417240` **Keyboard_ReadNextEventRegs**
- `0x004172D0` **Keyboard_ToLowerAscii**
- `0x00576CF0` **DirectInputMouse_Init** — cross: `DynDLL_Load` → `platform/bootstrap/runtime`, `DynAPI_Resolve` → `platform/bootstrap/runtime`, `TimerSystem_RegisterPeriodic` → `platform/system/time_locale`, `Package_LoadEntry` → `assets/package/runtime`, `Resource_Load` → `assets/resource/runtime`
- `0x00576F20` **DirectInputMouse_RefreshDeviceIfIdle**
- `0x00577000` **DirectInputMouse_Shutdown** — cross: `TimerSystem_UnregisterPeriodic` → `platform/system/time_locale`
- `0x00577080` **DirectInputMouse_PollBufferedEvents**
- `0x005772F0` **DirectInputMouse_DisplayModeHookCf**
- `0x00577420` **DirectInputMouse_SetPosition**
- `0x00577460` **DirectInputMouse_FlushBufferedEvents**
- `0x005774A0` **Keyboard_OnKeyDown**
- `0x00577880` **Keyboard_OnKeyUp**
- `0x00577B30` **Keyboard_OnChar**
- `0x004172B0` **Keyboard_ToUpperAscii**

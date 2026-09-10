# Call graph: `core/memory/synchronization`

5 functions. Addresses are current V523 executable VAs.

## Functions

- `0x00407470` **SpinLock_Acquire**
- `0x004074A0` **SpinLock_TryAcquireFlags**
- `0x004074D0` **SpinLock_Release**
- `0x004074F0` **SpinLock_ReleaseAndInvoke**
- `0x00585F00` **Runtime_Shutdown** — cross: `PersistentSettings_Flush` → `core/settings/persistent`, `UiRuntime_Shutdown` → `ui/core/runtime`, `DirectInputMouse_Shutdown` → `platform/input/devices`, `Graphics_Shutdown` → `graphics/core/runtime`, `Network_Shutdown` → `network/backend/runtime`, `DirectSound_Shutdown` → `audio/backend/runtime`, `TimerSystem_Shutdown` → `platform/system/time_locale`, `DynDLL_UnloadAll` → `platform/bootstrap/runtime`, `Win32FileSystem_RestoreInitialDirectory` → `platform/filesystem/win32`, `ArenaHeap_Shutdown` → `core/memory/allocator`

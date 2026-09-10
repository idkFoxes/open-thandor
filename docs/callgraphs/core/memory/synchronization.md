# Call graph: `core/memory/synchronization`

[Source](../../../../src/core/memory/synchronization.c) · [Header](../../../../include/thandor/core/memory/synchronization.h) · [Graphviz](synchronization.dot) · [Module tree](../../../MODULE_TREE.md) · [Source guide](../../../SOURCE_FILE_GUIDE.md#module-core-memory-synchronization)

5 functions. Addresses are current V523 executable VAs.

## Functions

- `0x00407470` **[`SpinLock_Acquire`](../../../../src/core/memory/synchronization.c#L5)**
- `0x004074A0` **[`SpinLock_TryAcquireFlags`](../../../../src/core/memory/synchronization.c#L26)**
- `0x004074D0` **[`SpinLock_Release`](../../../../src/core/memory/synchronization.c#L49)**
- `0x004074F0` **[`SpinLock_ReleaseAndInvoke`](../../../../src/core/memory/synchronization.c#L63)**
- `0x00585F00` **[`Runtime_Shutdown`](../../../../src/core/memory/synchronization.c#L79)** — cross: [`PersistentSettings_Flush`](../../../../src/core/settings/persistent.c#L5) → [`core/settings/persistent`](../settings/persistent.md), [`UiRuntime_Shutdown`](../../../../src/ui/core/runtime.c#L228) → [`ui/core/runtime`](../../ui/core/runtime.md), [`DirectInputMouse_Shutdown`](../../../../src/platform/input/devices.c#L246) → [`platform/input/devices`](../../platform/input/devices.md), [`Graphics_Shutdown`](../../../../src/graphics/core/runtime.c#L674) → [`graphics/core/runtime`](../../graphics/core/runtime.md), [`Network_Shutdown`](../../../../src/network/backend/runtime.c#L757) → [`network/backend/runtime`](../../network/backend/runtime.md), [`DirectSound_Shutdown`](../../../../src/audio/backend/runtime.c#L5) → [`audio/backend/runtime`](../../audio/backend/runtime.md), [`TimerSystem_Shutdown`](../../../../src/platform/system/time_locale.c#L5) → [`platform/system/time_locale`](../../platform/system/time_locale.md), [`DynDLL_UnloadAll`](../../../../src/platform/bootstrap/runtime.c#L386) → [`platform/bootstrap/runtime`](../../platform/bootstrap/runtime.md), [`Win32FileSystem_RestoreInitialDirectory`](../../../../src/platform/filesystem/win32.c#L253) → [`platform/filesystem/win32`](../../platform/filesystem/win32.md), [`ArenaHeap_Shutdown`](../../../../src/core/memory/allocator.c#L171) → [`core/memory/allocator`](allocator.md)

## Called by

- [`core/error/runtime`](../error/runtime.md): [`FatalError_Exit`](../../../../src/core/error/runtime.c#L168) → [`Runtime_Shutdown`](../../../../src/core/memory/synchronization.c#L79)
- [`platform/bootstrap/runtime`](../../platform/bootstrap/runtime.md): [`ProcessEntry`](../../../../src/platform/bootstrap/runtime.c#L5) → [`Runtime_Shutdown`](../../../../src/core/memory/synchronization.c#L79)
- [`platform/system/win32`](../../platform/system/win32.md): [`Win32_PumpMessages`](../../../../src/platform/system/win32.c#L5) → [`Runtime_Shutdown`](../../../../src/core/memory/synchronization.c#L79)

#include <thandor/core/memory/synchronization.h>

/* Implementation ownership: core/memory/synchronization. */

/* Address: 0x00407470.
   Ownership: core/memory/synchronization.
   Purpose: Treats null as an immediate success. For a non-null lock, repeatedly performs atomic xchg(lock,-1)
   until the previous value was zero. The loop has no pause, yield, timeout, ownership tracking, or recursion
   support. CF is clear on return.
*/
void SpinLock_Acquire(RuntimeSpinLockValue *lockValue)

{
  RuntimeSpinLockValue *previousLockValue;
  
  previousLockValue = lockValue;
  while (previousLockValue != (RuntimeSpinLockValue *)0x0) {
    LOCK();
    previousLockValue = (RuntimeSpinLockValue *)*lockValue;
    *lockValue = SPIN_LOCK_LOCKED;
    UNLOCK();
  }
  return;
}

/* Address: 0x004074A0.
   Ownership: core/memory/synchronization.
   Purpose: Treats null as success. For a non-null lock, performs one atomic xchg(lock,-1). CF clear means the
   previous value was zero and the lock was acquired. CF set means it was already nonzero. EAX is preserved and is
   not a scalar result.
*/
void SpinLock_TryAcquireFlags(RuntimeSpinLockValue *lockValue)

{
  RuntimeSpinLockValue previousLockValue;
  
  if (lockValue != (RuntimeSpinLockValue *)0x0) {
    LOCK();
    previousLockValue = *lockValue;
    *lockValue = SPIN_LOCK_LOCKED;
    UNLOCK();
    if (previousLockValue != SPIN_LOCK_UNLOCKED) {
      return;
    }
  }
  return;
}

/* Address: 0x004074D0.
   Ownership: core/memory/synchronization.
   Purpose: Treats null as a no-op. For a non-null lock, writes zero with a plain non-atomic store. CF is clear on
   return.
*/
void SpinLock_Release(RuntimeSpinLockValue *lockValue)

{
  if (lockValue != (RuntimeSpinLockValue *)0x0) {
    *lockValue = SPIN_LOCK_UNLOCKED;
  }
  return;
}

/* Address: 0x004074F0.
   Ownership: core/memory/synchronization.
   Purpose: When lockValue is non-null, writes zero first and then invokes callback when callback is non-null. When
   lockValue is null, callback is not invoked. The callback receives no arguments. CF is clear on return.
*/
void SpinLock_ReleaseAndInvoke
               (SpinLockReleaseCallbackProc *callback,RuntimeSpinLockValue *lockValue)

{
  if ((lockValue != (RuntimeSpinLockValue *)0x0) &&
     (*lockValue = SPIN_LOCK_UNLOCKED, callback != (SpinLockReleaseCallbackProc *)0x0)) {
    (*callback)();
  }
  return;
}

/* Address: 0x00585F00.
   Ownership: core/memory/synchronization.
   Purpose: Central reverse-order subsystem shutdown.
   Cross-module calls: PersistentSettings_Flush [core/settings/persistent], UiRuntime_Shutdown [ui/core/runtime],
   DirectInputMouse_Shutdown [platform/input/devices], Graphics_Shutdown [graphics/core/runtime], Network_Shutdown
   [network/backend/runtime], DirectSound_Shutdown [audio/backend/runtime].
*/
void __cdecl Runtime_Shutdown(void)

{
  HANDLE hProcess;
  
  PersistentSettings_Flush();
  g_GraphicsBackendAccessState = -1;
  UiRuntime_Shutdown();
  DirectInputMouse_Shutdown();
  Graphics_Shutdown();
  Network_Shutdown();
  DirectSound_Shutdown();
  TimerSystem_Shutdown();
  DynDLL_UnloadAll();
  Win32FileSystem_RestoreInitialDirectory();
  ArenaHeap_Shutdown();
  hProcess = GetCurrentProcess();
  SetPriorityClass(hProcess,0x20);
  return;
}

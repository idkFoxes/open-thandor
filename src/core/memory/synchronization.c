/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/src/core/memory/synchronization.c
 * Reverse engineering by idkFoxes 2026
 */

#include <thandor/core/memory/synchronization.h>
#include <thandor/thandor.h>

/* Implementation ownership: core/memory/synchronization. */

/* Address: 0x00407470.
   Busy-waits until the lock is taken: swaps -1 into it (XCHG) until the previous value was zero. A null
   lock succeeds at once; there is no pause, yield, timeout or recursion. Guards the per-tick state of the
   frontend and in-game loops against the timer callbacks.
   Reached through the function-pointer slot g_SpinLockAcquire (0x00402784).
*/
void SpinLock_Acquire(RuntimeSpinLockValue *lockValue)

{
  RuntimeSpinLockValue *previousLockValue; /* first the lock pointer (null test), then the swapped-out value */

  previousLockValue = lockValue;
  while (previousLockValue != NULL) {
    previousLockValue =
         (RuntimeSpinLockValue *)(uintptr_t)THANDOR_ATOMIC_EXCHANGE(lockValue,SPIN_LOCK_LOCKED);
  }
  return;
}


/* Address: 0x004074A0.
   Tries once to take the lock by swapping -1 into it (XCHG). Returns the original CF: false when the lock
   was free and is now held (or the lock pointer is null), true when it was already busy, so the caller can
   skip its work instead of waiting.
   Reached through the function-pointer slot g_SpinLockTryAcquire (0x00402788).
*/
bool SpinLock_TryAcquireFlags(RuntimeSpinLockValue *lockValue)

{
  RuntimeSpinLockValue previousLockValue;

  if (lockValue != NULL) {
    previousLockValue = (RuntimeSpinLockValue)THANDOR_ATOMIC_EXCHANGE(lockValue,SPIN_LOCK_LOCKED);
    if (previousLockValue != SPIN_LOCK_UNLOCKED) {
      return true;
    }
  }
  return false;
}


/* Address: 0x004074D0.
   Releases the lock with a plain (non-atomic) store of zero; a null lock is ignored.
   Reached through the function-pointer slot g_SpinLockRelease (0x0040278C).
*/
void SpinLock_Release(RuntimeSpinLockValue *lockValue)

{
  if (lockValue != NULL) {
    *lockValue = SPIN_LOCK_UNLOCKED;
  }
  return;
}


/* Address: 0x004074F0.
   Releases the lock (plain store of zero) and then calls the argument-less callback, if any, so deferred
   work can run once the lock is free. With a null lock nothing happens, not even the callback.
   Reached through the function-pointer slot g_SpinLockReleaseAndInvoke (0x00402790); the UI pointer and
   keyboard dispatchers use it to drop g_UiRuntimeFrameLock and run g_UiRuntimePostUnlockCallback.
*/
void SpinLock_ReleaseAndInvoke(SpinLockReleaseCallbackProc *callback,RuntimeSpinLockValue *lockValue)

{
  if (lockValue == NULL) {
    return;
  }
  *lockValue = SPIN_LOCK_UNLOCKED;
  if (callback != NULL) {
    callback();
  }
}


/* Address: 0x00585F00.
   Shuts the game down at the end of ProcessEntry: saves the settings, then closes the subsystems in
   roughly the reverse order of their initialisation, frees the memory arena last and drops the process
   back from real-time to normal priority.
*/
void Runtime_Shutdown(void)

{
  HANDLE process;

  PersistentSettings_Flush();
  /* any nonzero state makes the present and the cursor timer skip their work from now on */
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
  process = GetCurrentProcess();
  SetPriorityClass(process,NORMAL_PRIORITY_CLASS);
}


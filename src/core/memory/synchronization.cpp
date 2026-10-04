/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/src/core/memory/synchronization.cpp
 * Reverse engineering by idkFoxes 2026
 */

#include <thandor/core/memory/synchronization.h>
#include <thandor/thandor.h>

/* Module data. */

__declspec(align(4)) SpinLockAcquireProc *g_SpinLockAcquire = THANDOR_FN(SpinLock_Acquire);

__declspec(align(8)) SpinLockTryAcquireFlagsProc *g_SpinLockTryAcquire = THANDOR_FN(SpinLock_TryAcquireFlags);

__declspec(align(4)) SpinLockReleaseProc *g_SpinLockRelease = THANDOR_FN(SpinLock_Release);

__declspec(align(16)) SpinLockReleaseAndInvokeProc *g_SpinLockReleaseAndInvoke = THANDOR_FN(SpinLock_ReleaseAndInvoke);

/* Implementation ownership: core/memory/synchronization. */

/* Busy-waits until the lock is taken: atomically swaps -1 into it until the previous value was zero. A
   null lock succeeds at once; there is no pause, yield, timeout or recursion. Guards the per-tick state of
   the frontend and in-game loops against the timer callbacks.
   Reached through the function-pointer slot g_SpinLockAcquire.
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


/* Tries once to take the lock by atomically swapping -1 into it. Returns false when the lock was free and
   is now held (or the lock pointer is null), true when it was already busy, so the caller can skip its
   work instead of waiting.
   Reached through the function-pointer slot g_SpinLockTryAcquire.
*/
Bool8 SpinLock_TryAcquireFlags(RuntimeSpinLockValue *lockValue)

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


/* Releases the lock with a plain (non-atomic) store of zero; a null lock is ignored.
   Reached through the function-pointer slot g_SpinLockRelease.
*/
void SpinLock_Release(RuntimeSpinLockValue *lockValue)

{
  if (lockValue != NULL) {
    *lockValue = SPIN_LOCK_UNLOCKED;
  }
  return;
}


/* Releases the lock (plain store of zero) and then calls the argument-less callback, if any, so deferred
   work can run once the lock is free. With a null lock nothing happens, not even the callback.
   Reached through the function-pointer slot g_SpinLockReleaseAndInvoke; the UI pointer and
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


/* Shuts the game down at the end of ProcessEntry: saves the settings, then closes the subsystems in
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
  SdlInput_Shutdown();
  Graphics_Shutdown();
  SdlVideo_Shutdown();
  Network_Shutdown();
  SdlAudio_Shutdown();
  SdlTimer_Shutdown();
  TimerSystem_Shutdown();
  DynDLL_UnloadAll();
  Win32FileSystem_RestoreInitialDirectory();
  ArenaHeap_Shutdown();
  process = GetCurrentProcess();
  SetPriorityClass(process,NORMAL_PRIORITY_CLASS);
}


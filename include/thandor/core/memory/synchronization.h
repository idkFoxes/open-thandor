/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/include/thandor/core/memory/synchronization.h
 * Reverse engineering by idkFoxes 2026
 */

#ifndef THANDOR_CORE_MEMORY_SYNCHRONIZATION_H
#define THANDOR_CORE_MEMORY_SYNCHRONIZATION_H

#include <thandor/core/memory/types.h>
#include <thandor/core/types.h>
#include <thandor/core/contracts.h>

/* Submodule: core/memory/synchronization. */
/* Functions are grouped by semantic ownership. */

void SpinLock_Acquire(RuntimeSpinLockValue *lockValue);

Bool8 SpinLock_TryAcquireFlags(RuntimeSpinLockValue *lockValue);

void SpinLock_Release(RuntimeSpinLockValue *lockValue);

void SpinLock_ReleaseAndInvoke(SpinLockReleaseCallbackProc *callback,RuntimeSpinLockValue *lockValue);

void Runtime_Shutdown(void);

extern SpinLockAcquireProc *g_SpinLockAcquire;
extern SpinLockTryAcquireFlagsProc *g_SpinLockTryAcquire;
extern SpinLockReleaseProc *g_SpinLockRelease;
extern SpinLockReleaseAndInvokeProc *g_SpinLockReleaseAndInvoke;

/* Scoped spin lock: takes the lock through g_SpinLockAcquire on construction and drops it through
   g_SpinLockRelease when the scope ends (a null lock does nothing, as with the slots themselves). Only for
   sites whose acquire and release already pair up in one scope. */
class SpinLockGuard {
public:
  explicit SpinLockGuard(RuntimeSpinLockValue *lockValue) : m_lockValue(lockValue)
  {
    g_SpinLockAcquire(m_lockValue);
  }
  ~SpinLockGuard()
  {
    g_SpinLockRelease(m_lockValue);
  }
  SpinLockGuard(const SpinLockGuard &) = delete;
  SpinLockGuard &operator=(const SpinLockGuard &) = delete;

private:
  RuntimeSpinLockValue *m_lockValue;
};

#endif /* THANDOR_CORE_MEMORY_SYNCHRONIZATION_H */

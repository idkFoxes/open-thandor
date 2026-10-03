/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/include/thandor/core/memory/synchronization.h
 * Reverse engineering by idkFoxes 2026
 */

#ifndef THANDOR_CORE_MEMORY_SYNCHRONIZATION_H
#define THANDOR_CORE_MEMORY_SYNCHRONIZATION_H

#include <thandor/generated/types.h>
#include <thandor/core/contracts.h>

/* Submodule: core/memory/synchronization. */
/* Functions are grouped by semantic ownership. */

void SpinLock_Acquire(RuntimeSpinLockValue *lockValue);

bool SpinLock_TryAcquireFlags(RuntimeSpinLockValue *lockValue);

void SpinLock_Release(RuntimeSpinLockValue *lockValue);

void SpinLock_ReleaseAndInvoke(SpinLockReleaseCallbackProc *callback,RuntimeSpinLockValue *lockValue);

void Runtime_Shutdown(void);

#endif /* THANDOR_CORE_MEMORY_SYNCHRONIZATION_H */

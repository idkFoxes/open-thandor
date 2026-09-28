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
/* Functions are grouped by semantic ownership; address comments are executable virtual addresses. */

/* 0x00407470 */
void SpinLock_Acquire(RuntimeSpinLockValue *lockValue);

/* 0x004074A0 */
bool SpinLock_TryAcquireFlags(RuntimeSpinLockValue *lockValue);

/* 0x004074D0 */
void SpinLock_Release(RuntimeSpinLockValue *lockValue);

/* 0x004074F0 */
void SpinLock_ReleaseAndInvoke(SpinLockReleaseCallbackProc *callback,RuntimeSpinLockValue *lockValue);

/* 0x00585F00 */
void Runtime_Shutdown(void);

#endif /* THANDOR_CORE_MEMORY_SYNCHRONIZATION_H */

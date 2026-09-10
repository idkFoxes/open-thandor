#ifndef THANDOR_CORE_MEMORY_SYNCHRONIZATION_H
#define THANDOR_CORE_MEMORY_SYNCHRONIZATION_H

#include <thandor/generated/v523_types.h>
#include <thandor/core/contracts.h>

/* Submodule: core/memory/synchronization. */
/* Functions are grouped by semantic ownership; address comments are executable virtual addresses. */

/* 0x00407470 */
void SpinLock_Acquire(RuntimeSpinLockValue *lockValue);

/* 0x004074A0 */
void SpinLock_TryAcquireFlags(RuntimeSpinLockValue *lockValue);

/* 0x004074D0 */
void SpinLock_Release(RuntimeSpinLockValue *lockValue);

/* 0x004074F0 */
void SpinLock_ReleaseAndInvoke (SpinLockReleaseCallbackProc *callback,RuntimeSpinLockValue *lockValue);

/* 0x00585F00 */
void __cdecl Runtime_Shutdown(void);

#endif /* THANDOR_CORE_MEMORY_SYNCHRONIZATION_H */

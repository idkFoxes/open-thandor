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
void __thandor_void_preserve_eax_ecx_edx SpinLock_Acquire(RuntimeSpinLockValue *lockValue);

/* 0x004074A0 */
bool __thandor_cf_preserve_eax_ecx_edx SpinLock_TryAcquireFlags(RuntimeSpinLockValue *lockValue);

/* 0x004074D0 */
void __thandor_void_preserve_eax_ecx_edx SpinLock_Release(RuntimeSpinLockValue *lockValue);

/* 0x004074F0 */
void __thandor_void_preserve_eax_ecx_edx
SpinLock_ReleaseAndInvoke(SpinLockReleaseCallbackProc *callback,RuntimeSpinLockValue *lockValue);

/* 0x00585F00 */
void __thandor_preserve_eax Runtime_Shutdown(void);

#endif /* THANDOR_CORE_MEMORY_SYNCHRONIZATION_H */

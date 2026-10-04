/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/include/thandor/core/error/types.h
 * Reverse engineering by idkFoxes 2026
 */

#ifndef THANDOR_CORE_ERROR_TYPES_H
#define THANDOR_CORE_ERROR_TYPES_H

#include <stdint.h>
#include <thandor/core/ptr32.h> /* Ptr32: the pointer fields of these 32-bit layouts */
#include <thandor/core/types.h>

/* Types (split out by tools/dev/split_types.py). */

using FatalErrorPassThroughProc = uintptr_t (uintptr_t valueOrError, Bool8 failed); /* value or pointer (5f) */

#endif /* THANDOR_CORE_ERROR_TYPES_H */

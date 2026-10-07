/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/include/thandor/world/runtime/types.h
 * Reverse engineering by idkFoxes 2026
 */

#ifndef THANDOR_WORLD_RUNTIME_TYPES_H
#define THANDOR_WORLD_RUNTIME_TYPES_H

#include <stdint.h>
#include <thandor/core/ptr32.h> /* Ptr32: the pointer fields of these 32-bit layouts */

struct WorldOwnerListNode;

using WorldRuntimeNodeTraversalCallback = void (void * callbackContext, WorldOwnerListNode * node);

#endif /* THANDOR_WORLD_RUNTIME_TYPES_H */

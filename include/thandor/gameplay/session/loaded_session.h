/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/include/thandor/gameplay/session/loaded_session.h
 * Reverse engineering by idkFoxes 2026
 */

#ifndef THANDOR_GAMEPLAY_SESSION_LOADED_SESSION_H
#define THANDOR_GAMEPLAY_SESSION_LOADED_SESSION_H

#include <thandor/core/types.h>
#include <thandor/gameplay/selection/types.h>
#include <thandor/core/contracts.h>

/* Submodule: gameplay/session/loaded_session. */

/* Functions are grouped by semantic ownership. */

Bool8 InGameRuntime_InitializeLoadedSession(uint16_t *savePackagePath,uint32_t *outError);

extern SelectionPlayerRuntimeBlock *g_SelectionPlayerBlocks;

#endif /* THANDOR_GAMEPLAY_SESSION_LOADED_SESSION_H */

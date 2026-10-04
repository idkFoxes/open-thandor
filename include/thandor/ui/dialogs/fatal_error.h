/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/include/thandor/ui/dialogs/fatal_error.h
 * Reverse engineering by idkFoxes 2026
 */

#ifndef THANDOR_UI_DIALOGS_FATAL_ERROR_H
#define THANDOR_UI_DIALOGS_FATAL_ERROR_H

#include <thandor/generated/types.h>
#include <thandor/core/contracts.h>

/* Submodule: ui/dialogs/fatal_error. */

/* Functions are grouped by semantic ownership. */

Bool8 FatalErrorDialog_BlockMissedPointerPress(UiRootNode *root);

int FatalErrorDialog_BlockMissedPointerMotion(UiRootNode *root);

void FatalErrorDialog_DismissAndPopRoot(UiRootNode *rootNode);

uintptr_t FatalErrorRuntime_DispatchPendingError(uintptr_t valueOrError,Bool8 failed);

void ErrorRuntime_InstallUiHandlerAndAllocateState(void);

#endif /* THANDOR_UI_DIALOGS_FATAL_ERROR_H */

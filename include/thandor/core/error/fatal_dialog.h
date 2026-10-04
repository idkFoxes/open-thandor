/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/include/thandor/core/error/fatal_dialog.h
 * Reverse engineering by idkFoxes 2026
 */

#ifndef THANDOR_CORE_ERROR_FATAL_DIALOG_H
#define THANDOR_CORE_ERROR_FATAL_DIALOG_H

#include <thandor/generated/types.h>
#include <thandor/core/contracts.h>

/* Submodule: core/error/fatal_dialog. */

/* Functions are grouped by semantic ownership. */

Bool8 FatalErrorDialog_BlockMissedPointerPress(UiRootNode *root);

int FatalErrorDialog_BlockMissedPointerMotion(UiRootNode *root);

void FatalErrorDialog_DismissAndPopRoot(UiRootNode *rootNode);

uintptr_t FatalErrorRuntime_DispatchPendingError(uintptr_t valueOrError,Bool8 failed);

void ErrorRuntime_InstallUiHandlerAndAllocateState(void);

#endif /* THANDOR_CORE_ERROR_FATAL_DIALOG_H */

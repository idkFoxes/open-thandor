/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/include/thandor/ui/frontend/page_actions.h
 * Reverse engineering by idkFoxes 2026
 */

#ifndef THANDOR_UI_FRONTEND_PAGE_ACTIONS_H
#define THANDOR_UI_FRONTEND_PAGE_ACTIONS_H

#include <thandor/generated/types.h>
#include <thandor/core/contracts.h>

/* Submodule: ui/frontend/page_actions. */

/* Functions are grouped by semantic ownership. */

void FrontendCallback_NoOpArg1(void *source);

void FrontendCallback_ReturnToMainPageOrDispatchState4(uint32_t callbackArgument);

void FrontendQuitDialogAction_ReturnToMainPage(uint32_t callbackArgument);

void FrontendOptionsAction_ReturnToMainOrOptionsPage(UiNodeBase *sourceNode);

#endif /* THANDOR_UI_FRONTEND_PAGE_ACTIONS_H */

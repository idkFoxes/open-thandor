/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/include/thandor/ui/frontend/chat.h
 * Reverse engineering by idkFoxes 2026
 */

#ifndef THANDOR_UI_FRONTEND_CHAT_H
#define THANDOR_UI_FRONTEND_CHAT_H

#include <thandor/generated/types.h>
#include <thandor/core/contracts.h>

/* Submodule: ui/frontend/chat. */

/* Functions are grouped by semantic ownership. */

void FrontendRecentTextHistory_InsertAndRebuild5(uint16_t *text);

void FrontendRecentText_TrimAndSortTopFive(UiNodeBase *source);

#endif /* THANDOR_UI_FRONTEND_CHAT_H */

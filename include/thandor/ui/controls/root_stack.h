/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/include/thandor/ui/controls/root_stack.h
 * Reverse engineering by idkFoxes 2026
 */

#ifndef THANDOR_UI_CONTROLS_ROOT_STACK_H
#define THANDOR_UI_CONTROLS_ROOT_STACK_H

#include <thandor/core/types.h>
#include <thandor/ui/controls/types.h>
#include <thandor/ui/core/types.h>
#include <thandor/core/contracts.h>

bool UiRootStack_PopUntilWindowTextureBoundary();

/* End marker of the UI root stack: g_UiRootNode holds it when no root is open, and the bottom root's
   previousRoot link holds it. */
#define UI_ROOT_STACK_END (reinterpret_cast<UiRootNode *>(intptr_t{-1}))

void UiRootStack_Push(UiRootCallbacks *callbacks,UiRootNode *root);

bool UiRootStack_Pop(UiRootNode *root);

bool UiRootStack_BringToFront(UiRootNode *root);

void UiRootStack_Relayout();

void UiRootStack_InvalidateAll();

extern uint32_t g_UiInvalidationSuppressed;
extern UiRootNode *g_UiRootNode;
extern UiRootStackActionHandlerPage2 g_UiRootStackActionHandlerPage;

#endif /* THANDOR_UI_CONTROLS_ROOT_STACK_H */

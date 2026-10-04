/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/src/ui/frontend/page_actions.cpp
 * Reverse engineering by idkFoxes 2026
 */

#include <thandor/ui/frontend/page_actions.h>
#include <thandor/thandor.h>
#include <thandor/platform/bootstrap/image.h>

/* Implementation ownership: ui/frontend/page_actions. */

/* Handler of action 0x2050 (slot 80 of g_FrontendUiActionHandlersPage20.handlers00_54), the "Save" button of
   the in-game variant of the mission briefing: does nothing.
*/
void FrontendCallback_NoOpArg1(void *source)

{
  return;
}

/* Handler of action 0x2034 (slot 52 of g_FrontendUiActionHandlersPage20.handlers00_54), the "Choose game"
   page's "Cancel" button: returns to the main page with ROM action record 0 in a local game and with record 4
   (as FRONTEND_COMMAND_RETURN_TO_MAIN_PAGE) in a network game.
*/
void FrontendCallback_ReturnToMainPageOrDispatchState4(uint32_t callbackArgument)

{
  /* the inner tests repeat the outer one, so only the local-direct and network-queued paths are reachable */
  if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_NETWORKED_MASK) ==
      SESSION_NETWORK_ROLE_LOCAL) {
    if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_NETWORKED_MASK) ==
        SESSION_NETWORK_ROLE_LOCAL) {
      FrontendSession_ReturnToMainPage(g_LocalPlayerRuntimeId,0,0,0);
    }
    else {
      FrontendCommandQueue_EnqueueLocalPlayerCommand(FRONTEND_COMMAND_RETURN_TO_MAIN_PAGE,0,0,0);
    }
  }
  else if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_NETWORKED_MASK) ==
           SESSION_NETWORK_ROLE_LOCAL) {
    FrontendSession_ReturnToMainPage(g_LocalPlayerRuntimeId,0,0,4);
  }
  else {
    FrontendCommandQueue_EnqueueLocalPlayerCommand(FRONTEND_COMMAND_RETURN_TO_MAIN_PAGE,0,0,4);
  }
  return;
}

/* Handler of action 0x2033 (slot 51 of g_FrontendUiActionHandlersPage20.handlers00_54), the quit dialog's "no"
   button: returns to the main page with ROM action record 0 (FRONTEND_COMMAND_RETURN_TO_MAIN_PAGE in a
   network game).
*/
void FrontendQuitDialogAction_ReturnToMainPage(uint32_t callbackArgument)

{
  if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_NETWORKED_MASK) ==
      SESSION_NETWORK_ROLE_LOCAL) {
    FrontendSession_ReturnToMainPage(g_LocalPlayerRuntimeId,0,0,0);
  }
  else {
    FrontendCommandQueue_EnqueueLocalPlayerCommand(FRONTEND_COMMAND_RETURN_TO_MAIN_PAGE,0,0,0);
  }
  return;
}

/* Handler of action 0x2010 (slot 16 of g_FrontendUiActionHandlersPage20.handlers00_54), shared by the options
   page's "Ok" button and the display settings page's "Back" button: "Ok" returns to the main page (ROM action
   record 0, FRONTEND_COMMAND_RETURN_TO_MAIN_PAGE in a network game), "Back" reopens the options page.
*/
void FrontendOptionsAction_ReturnToMainOrOptionsPage(UiNodeBase *sourceNode)

{
  FrontendModelPointerContextFlags *compactLayoutFlags;
  uintptr_t parentNodeAddress;
  FrontendRootPageState *frontendRootPage;

  parentNodeAddress = (uintptr_t)sourceNode->parent;
  frontendRootPage = (FrontendRootPageState *)sourceNode;
  while ((UiNodeBase *)parentNodeAddress != UI_NODE_NONE) {
    frontendRootPage = (FrontendRootPageState *)(frontendRootPage->rootNode).parent;
    parentNodeAddress = (uintptr_t)(frontendRootPage->rootNode).parent;
  }
  if (sourceNode == &frontendRootPage->returnToMainActionControl) {
    if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_NETWORKED_MASK) ==
        SESSION_NETWORK_ROLE_LOCAL) {
      FrontendSession_ReturnToMainPage(g_LocalPlayerRuntimeId,0,0,0);
    }
    else {
      FrontendCommandQueue_EnqueueLocalPlayerCommand(FRONTEND_COMMAND_RETURN_TO_MAIN_PAGE,0,0,0);
    }
    return;
  }
  if ((int)g_FramebufferWidth < FRONTEND_COMPACT_LAYOUT_MAX_WIDTH + 1) {
    compactLayoutFlags =
         &((FrontendModelPointerContext *)FRONTEND_UI(frontendRootPage,menuRoomModelView))->contextFlags;
    *compactLayoutFlags = *compactLayoutFlags | FRONTEND_MENU_ROOM_RENDER_SUPPRESSED;
  }
  UiPageStack_SetActiveIndex(FRONTEND_PAGE_OPTIONS,&frontendRootPage->primaryPageStack);
  return;
}

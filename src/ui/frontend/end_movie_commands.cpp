/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/src/ui/frontend/end_movie_commands.cpp
 * Reverse engineering by idkFoxes 2026
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <thandor/ui/frontend/end_movie_commands.h>
#include <thandor/ui/core/key_dispatch.h>
#include <thandor/thandor.h>
#include <thandor/platform/bootstrap/image.h>
#include <thandor/platform/debug/hooks.h>

/* Module data. */

/* 2 command records and the terminator record (commandCode 0) that ends the dispatcher's scan. The scan
   also reads the terminator's modifierClassFlags (0x90909090, NOP fill). Original quirk: the original's
   terminator was only 8 bytes long and code followed it, so the terminator's continuationEntryAddress is not
   original data (never read). */
static const UiCommandDispatchRecord g_EndMovieCommandDispatchRecords[3] = {
    /* 0 */ {.commandCode = 0x71, .modifierClassFlags = 0x30, .continuationEntryAddress = 0x565990},
    /* 1 */ {.commandCode = 0x70, .modifierClassFlags = 0xC, .continuationEntryAddress = 0x5658F0},
    /* 2 */ {.commandCode = 0x0, .modifierClassFlags = 0x90909090}};

/* Implementation ownership: ui/frontend/end_movie_commands. */

/* Update callback of the end-movie UI in a network game: keeps the frontend session alive while the end
   movie plays by running the session tick of the local role. Does nothing in a local game. Note that the
   SESSION_NETWORK_ROLE_CLIENT bit selects the host tick and the HOST bit the client tick: either the enum or
   the two tick functions are named the wrong way round.
*/
void EndMovieUiRuntime_HandleModeTransition(void *endMovieRuntime)

{
  if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_CLIENT) == SESSION_NETWORK_ROLE_LOCAL) {
    if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_HOST) != SESSION_NETWORK_ROLE_LOCAL) {
      FrontendHostSession_TickPeerTimeoutsAndDropPlayers();
    }
  }
  else {
    FrontendClientSession_TickHostTimeout();
  }
  return;
}

/* Keyboard handler of the end-movie UI: looks the key up in the end-movie command table, whose records also
   say which Ctrl/Alt combination they need, and runs the matching action: save a numbered PCX screenshot, or
   skip the end movie (marks the local player done with the results; in a network game as a queued command).
   endMovieRuntime is the active UI root, i.e. the in-game runtime root (g_InGameRuntimeRoot) whose callbacks
   Frontend_PlaySelectedEndMovie replaced. Skipping is ignored while its resultsContinueButton is suppressed
   (a network host still waiting for its clients, or the local player already marked ready; see
   FrontendPlayerRuntime_MarkResultsReadyAndUpdateContinueButton).
*/
void EndMovieUiRuntime_DispatchCommandByFlags
          (UiKeyboardStateMask modifierFlags,UiActionId commandCode,void *endMovieRuntime)

{
  /* The matching record's continuationEntryAddress selects the action below. endMovieRuntime is the in-game
     root. */
  /* the record's modifier class demands exactly none, Ctrl, Alt or Ctrl+Alt (Shift is ignored) */
  const UiCommandDispatchRecord *record = UiCommandDispatch_Find(g_EndMovieCommandDispatchRecords,
      (uint32_t)commandCode, (uint32_t)modifierFlags, UiKeyModifierRule::ExactShiftIgnored);
  if (record == nullptr) {
    return;
  }
  uint32_t target = (uint32_t)record->continuationEntryAddress;
  switch (target) {
  case 0x5658f0: /* screenshot */
    Screenshot_SaveFramebufferAsPcx();
    break;
  case 0x565990: /* skip the end movie */
    /* UI_NODE_SUPPRESSED in the nodeFlags of the results continue button */
    if (((INGAME_UI(endMovieRuntime,resultsContinueButton)->nodeFlags & UI_NODE_SUPPRESSED) != 0) ||
        ((g_UiCommandRuntimeFlags & UI_COMMAND_RUNTIME_FLAG_END_MOVIE_PENDING) != 0)) {
      break;
    }
    if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_CLIENT) != 0) {
      /* always true here: the direct call below is unreachable in the original as well */
      if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_NETWORKED_MASK) != 0) {
        InGameCommandQueue_AppendLocalPlayerCommand(INGAME_COMMAND_RESULTS_READY,0,0,0);
      }
      else {
        FrontendPlayerRuntime_MarkResultsReadyAndUpdateContinueButton(g_LocalPlayerRuntimeId);
      }
    }
    else if ((g_SessionNetworkRoleFlags & SESSION_NETWORK_ROLE_NETWORKED_MASK) != 0) {
      InGameCommandQueue_AppendLocalPlayerCommand(INGAME_COMMAND_APPLY_UI_FLAG_MASKS,0,
                                                  UI_COMMAND_RUNTIME_FLAG_RESULTS_CLOSED,0);
    }
    else {
      UiCommandRuntimeFlags_ApplyClearSetToggleMasks(g_LocalPlayerRuntimeId,0,
                                                     UI_COMMAND_RUNTIME_FLAG_RESULTS_CLOSED,0);
    }
    break;
  default:
    Thandor_Log("EndMovie dispatch: unhandled continuation %08x",target);
    break;
  }
  return;
}

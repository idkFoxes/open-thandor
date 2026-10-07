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

/* Actions of the end-movie command table. */
enum class EndMovieCommandAction : uint32_t {
    SkipEndMovie = 1,  /* Alt+Q */
    Screenshot = 2,    /* Ctrl+P */
};
static_assert(sizeof(UiKeyCommandRecord<EndMovieCommandAction>) == 0xC, "a key command record keeps the original 12 bytes");

/* 2 command records and the terminator record (commandCode 0) that ends the dispatcher's scan. The scan
   also reads the terminator's modifierClassFlags (0x90909090, NOP fill). Original quirk: the original's
   terminator was only 8 bytes long and code followed it, so the terminator's action is not original data
   (never read). */
static const UiKeyCommandRecord<EndMovieCommandAction> g_EndMovieCommandDispatchRecords[3] = {
    /* 0 */ {.commandCode = 0x71, .modifierClassFlags = 0x30, .action = EndMovieCommandAction::SkipEndMovie},
    /* 1 */ {.commandCode = 0x70, .modifierClassFlags = 0xC, .action = EndMovieCommandAction::Screenshot},
    /* 2 */ {.commandCode = 0x0, .modifierClassFlags = 0x90909090}};

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
  const UiKeyCommandRecord<EndMovieCommandAction> *record = UiCommandDispatch_Find(g_EndMovieCommandDispatchRecords,
      (uint32_t)commandCode, (uint32_t)modifierFlags, UiKeyModifierRule::ExactShiftIgnored);
  if (record == nullptr) {
    return;
  }
  EndMovieCommandAction target = record->action;
  switch (target) {
  case EndMovieCommandAction::Screenshot: /* screenshot */
    Screenshot_SaveFramebufferAsPcx();
    break;
  case EndMovieCommandAction::SkipEndMovie: /* skip the end movie */
    /* UI_NODE_SUPPRESSED in the nodeFlags of the results continue button */
    if (((InGameUi_Image(endMovieRuntime)->resultsContinueButton.selectable.base.nodeFlags &
          UI_NODE_SUPPRESSED) != 0) ||
        (Any(g_UiCommandRuntimeFlags & UI_COMMAND_RUNTIME_FLAG_END_MOVIE_PENDING))) {
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
                                                  ToBits(UI_COMMAND_RUNTIME_FLAG_RESULTS_CLOSED),0);
    }
    else {
      UiCommandRuntimeFlags_ApplyClearSetToggleMasks(g_LocalPlayerRuntimeId,0,
                                                     ToBits(UI_COMMAND_RUNTIME_FLAG_RESULTS_CLOSED),0);
    }
    break;
  default:
    Thandor_Log("EndMovie dispatch: unhandled action %u",static_cast<uint32_t>(target));
    break;
  }
}

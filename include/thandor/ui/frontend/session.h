/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/include/thandor/ui/frontend/session.h
 * Reverse engineering by idkFoxes 2026
 */

#ifndef THANDOR_UI_FRONTEND_SESSION_H
#define THANDOR_UI_FRONTEND_SESSION_H

#include <thandor/generated/types.h>
#include <thandor/core/contracts.h>

/* Submodule: ui/frontend/session. */
/* Functions are grouped by semantic ownership. */

/* Game speed slider percent to gameSpeedQ8: percent * FRONTEND_GAME_SPEED_PERCENT_TO_Q8_Q16 >> 16 =
   percent * 256 / 100 (0x28F5C = 2.56 in Q16, truncated). */
#define FRONTEND_GAME_SPEED_PERCENT_TO_Q8_Q16 0x28F5C

void FrontendSession_ReleaseSelectedResourceAndReturnToMainPage
          (FrontendReturnCallbackContext32 playerRuntimeId,uint32_t unusedArgument1,uint32_t unusedArgument2,
          uint32_t unusedArgument3);

void FrontendSessionAction_CloseMovieAndReturnToMainPage(UiNodeBase *source);

void FrontendSessionAction_ApplySpeedOrToggleReady(void *source);

void FrontendSessionAction_ResetNetworkAndReturnToMainPage(void *source);

void FrontendSessionAction_RandomizeSeedsAndReturnWithStartFlag(UiNodeBase *source);

void FrontendSession_SetGameSpeedPercent(uint32_t playerRuntimeId,uint32_t unusedArgument1,uint32_t unusedArgument2,
          GameSpeedPercent gameSpeedPercent);

void FrontendSession_ShowQuitConfirmPage(FrontendUiImage *frontendUi);

void FrontendTransferPage_ResetSessionOpenAndRequestMailbox(UiNodeBase *source);

void FrontendSessionList_DecrementExpiryAndCompactRows(FrontendNetworkListsRuntimeView *frontendRuntime);

void FrontendSession_PeriodicTick(void);

void FrontendHostSession_TickPeerTimeoutsAndDropPlayers(void);

void FrontendClientSession_TickHostTimeout(void);

void FrontendSession_ApplyGameSpeedAndReturnToMainPage
          (FrontendReturnCallbackContext32 playerRuntimeId,uint32_t unusedArgument1,uint32_t unusedArgument2,
          FrontendStatusCode romActionIndex);

void FrontendSession_ReturnToMainPage(uint32_t playerRuntimeId,uint32_t unusedArgument1,uint32_t unusedArgument2,
          FrontendStatusCode romActionIndex);

extern FrontendSessionDiscoveryRecord *g_FrontendSessionDiscoveryRecords;
extern FrontendPlayerRemovalPacket10007 g_FrontendClientPlayerRemovalPacket10007;

#endif /* THANDOR_UI_FRONTEND_SESSION_H */

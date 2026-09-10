#ifndef THANDOR_UI_FRONTEND_SESSION_H
#define THANDOR_UI_FRONTEND_SESSION_H

#include <thandor/generated/v523_types.h>
#include <thandor/core/contracts.h>

/* Submodule: ui/frontend/session. */
/* Functions are grouped by semantic ownership; address comments are executable virtual addresses. */

/* 0x00544270 */
undefined4 FrontendSession_ReleaseSelectedResourceAndReturnToMainPage (FrontendReturnCallbackContext32 callbackContext,dword argument2,dword argument3, dword argument4);

/* 0x00548FE0 */
undefined4 FrontendSessionAction_CloseMovieAndReturnToMainPage(UiNodeBase *source);

/* 0x00549090 */
undefined4 FrontendSessionAction_ApplySpeedOrToggleReady(void *source);

/* 0x0054C770 */
undefined4 FrontendSessionAction_ResetNetworkAndReturnToMainPage(void *source);

/* 0x0054D0B0 */
undefined4 FrontendSessionAction_RandomizeSeedsAndReturnWithStartFlag(UiNodeBase *source);

/* 0x00544250 */
void FrontendSession_SetGameSpeedPercent (dword argument1,dword argument2,dword argument3,GameSpeedPercent gameSpeedPercent);

/* 0x0054A790 */
void FrontendSession_ShowPage9WithCompactLayout(void *frontendRuntime);

/* 0x0054D2E0 */
undefined4 FrontendTransferPage_ResetSessionOpenAndRequestMailbox(UiNodeBase *source);

/* 0x0054E3A0 */
void FrontendSessionList_DecrementExpiryAndCompactRows(void *frontendRuntime);

/* 0x00565670 */
void FrontendSession_PeriodicTick(void);

/* 0x005725D0 */
void FrontendClientSession_DecrementTimeoutsAndCompactPlayers(void);

/* 0x00572960 */
void FrontendHostSession_TickShutdownOrReadyConsensus(void);

/* 0x00544210 */
undefined4 FrontendSession_ApplyGameSpeedAndReturnToMainPage (FrontendReturnCallbackContext32 callbackContext,dword argument2,dword argument3, FrontendStatusCode stateCode);

/* 0x00544D10 */
undefined4 FrontendSession_ReturnToMainPage (dword callbackContext,dword argument2,dword argument3,FrontendStatusCode stateCode);

#endif /* THANDOR_UI_FRONTEND_SESSION_H */

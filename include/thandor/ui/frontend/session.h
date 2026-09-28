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
/* Functions are grouped by semantic ownership; address comments are executable virtual addresses. */

/* 0x00544270 */
void __thandor_void_preserve_eax_ecx_edx
FrontendSession_ReleaseSelectedResourceAndReturnToMainPage
          (FrontendReturnCallbackContext32 playerRuntimeId,uint32_t unusedArgument1,uint32_t unusedArgument2,
          uint32_t unusedArgument3);

/* 0x00548FE0 */
void __thandor_preserve_eax FrontendSessionAction_CloseMovieAndReturnToMainPage(UiNodeBase *source);

/* 0x00549090 */
void __thandor_preserve_eax FrontendSessionAction_ApplySpeedOrToggleReady(void *source);

/* 0x0054C770 */
void __thandor_preserve_eax FrontendSessionAction_ResetNetworkAndReturnToMainPage(void *source);

/* 0x0054D0B0 */
void __thandor_void_preserve_eax_ecx
FrontendSessionAction_RandomizeSeedsAndReturnWithStartFlag(UiNodeBase *source);

/* 0x00544250 */
void __thandor_void_preserve_eax_ecx_edx
FrontendSession_SetGameSpeedPercent
          (uint32_t playerRuntimeId,uint32_t unusedArgument1,uint32_t unusedArgument2,
          GameSpeedPercent gameSpeedPercent);

/* 0x0054A790 */
void __thandor_preserve_eax FrontendSession_ShowQuitConfirmPage(FrontendUiImage *frontendUi);

/* 0x0054D2E0 */
void __thandor_preserve_eax
FrontendTransferPage_ResetSessionOpenAndRequestMailbox(UiNodeBase *source);

/* 0x0054E3A0 */
void __thandor_void_preserve_eax_ecx_edx
FrontendSessionList_DecrementExpiryAndCompactRows
          (FrontendNetworkListsRuntimeView5650 *frontendRuntime);

/* 0x00565670 */
void __thandor_void_preserve_eax_ecx_edx FrontendSession_PeriodicTick(void);

/* 0x005725D0 */
void __thandor_void_preserve_eax_ecx_edx
FrontendClientSession_DecrementTimeoutsAndCompactPlayers(void);

/* 0x00572960 */
void __thandor_void_preserve_eax_ecx FrontendHostSession_TickShutdownOrReadyConsensus(void);

/* 0x00544210 */
void __thandor_void_preserve_eax_ecx_edx
FrontendSession_ApplyGameSpeedAndReturnToMainPage
          (FrontendReturnCallbackContext32 playerRuntimeId,uint32_t unusedArgument1,uint32_t unusedArgument2,
          FrontendStatusCode romActionIndex);

/* 0x00544D10 */
void __thandor_void_preserve_eax_ecx_edx
FrontendSession_ReturnToMainPage
          (uint32_t playerRuntimeId,uint32_t unusedArgument1,uint32_t unusedArgument2,
          FrontendStatusCode romActionIndex);

#endif /* THANDOR_UI_FRONTEND_SESSION_H */

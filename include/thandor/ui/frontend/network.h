/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/include/thandor/ui/frontend/network.h
 * Reverse engineering by idkFoxes 2026
 */

#ifndef THANDOR_UI_FRONTEND_NETWORK_H
#define THANDOR_UI_FRONTEND_NETWORK_H

#include <thandor/generated/types.h>
#include <thandor/core/contracts.h>

/* Submodule: ui/frontend/network. */
/* Functions are grouped by semantic ownership. */

/* -CLIENT="host address": characters scanned for the closing quote (effectively unbounded; the scan stops at
   the terminator or a control character first). */
#define FRONTEND_CLIENT_OPTION_SCAN_LIMIT 0x7FFFFF

void FrontendNetworkSetupPage_InitializeBackendMode(FrontendUiImage *frontendUi);

void FrontendTeardown_SaveStatusTextAndHostAddress(UiRootNode *root);

void FrontendNetworkGamePage_Show(FrontendUiImage *frontendUi);

void FrontendNetworkGamePage_ClearSessionList(FrontendUiImage *frontendUi);

void FrontendTransferPage_ValidateInputAndRequestMailbox(UiTextEditControl *hostAddressEdit);

void FrontendTransferPage_OpenAndRequestMailbox(UiNodeBase *source);

void FrontendNetworkSetupPage_InitializeFromCommandLine(UiNodeBase *hostButton);

void FrontendNetworkSetupPage_InitializeSingleLocalPlayer(UiNodeBase *createButton);

extern uint16_t g_FrontendLocalPlayerNameUtf16[20];
extern Ptr32<FrontendSessionDiscoveryRecord> *g_FrontendSessionListRows;
extern Ptr32<FrontendPlayerRuntimeRecord> g_FrontendPlayerRuntimeRecordPointers32[32];
extern uint32_t g_FrontendNetworkState;
extern char g_SpielerSpielNetzwerkHostKeywordsAscii[31];
extern char g_NameClientKarteKeywordsAscii[21]; /* the option names NAME=" CLIENT=" KARTE=" (used with explicit lengths); Original quirk: its terminating NUL is the first byte of g_LevelPackageFoundEntry */
extern UiTransferEndpointDescriptor g_FrontendNetworkEndpointScratch;
extern uint16_t g_FrontendNetworkRuntimeCountTextUtf16[4]; /* decimal number of lobby players (payload 0 of the session player count text) */
extern uint16_t g_FrontendNetworkPlayerCountTextUtf16[4]; /* decimal maximum player count (payload 1 of the session player count text, bound to a template text control) */
extern uint16_t g_FrontendNetworkEndpointTextUtf16[512]; /* local address text from g_NetworkBackendSlot7 */

void FrontendNetworkSetup_OpenSelectedBackend(FrontendNetworkSetupPageBackendListPtr backendList);

void FrontendNetworkSettings_SetPlayerName(UiTextEditControl *control);

void FrontendNetworkSettings_SetPlayerCount(UiSettingsValueControl *control);

void FrontendNetworkSettings_SetGameName(UiTextEditControl *control);

void FrontendNetworkSettings_UpdateJoinButtonAndJoinOnDoubleClick
          (FrontendNetworkSettingsControlView *networkSettings);

Bool8 FrontendNetworkSettings_PublishSelectedPlayerDescriptor(FrontendNetworkSettingsControlView *networkSettings);

#endif /* THANDOR_UI_FRONTEND_NETWORK_H */

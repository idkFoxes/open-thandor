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
/* Functions are grouped by semantic ownership; address comments are executable virtual addresses. */

/* -CLIENT="host address": characters scanned for the closing quote (effectively unbounded; the scan stops at
   the terminator or a control character first). */
#define FRONTEND_CLIENT_OPTION_SCAN_LIMIT 0x7FFFFF

void FrontendNetworkSetupPage_InitializeBackendMode(FrontendUiImage *frontendUi);

void FrontendTeardown_SaveStatusTextAndHostAddress(UiRootNode *root);

void FrontendTransferPage_ValidateInputAndRequestMailbox(UiTextEditControl *hostAddressEdit);

void FrontendTransferPage_OpenAndRequestMailbox(UiNodeBase *source);

void FrontendNetworkSetupPage_InitializeFromCommandLine(UiNodeBase *hostButton);

void FrontendNetworkSetupPage_InitializeSingleLocalPlayer(UiNodeBase *createButton);

#endif /* THANDOR_UI_FRONTEND_NETWORK_H */

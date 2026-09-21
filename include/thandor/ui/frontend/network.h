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

/* 0x0054C260 */
void __thandor_void_preserve_eax_ecx
FrontendNetworkSetupPage_InitializeBackendMode(FrontendNetworkSettingsControlView250 *networkPage);

/* 0x005474A0 */
void __thandor_void_preserve_eax_ecx FrontendTeardown_SaveRootStateSnapshot80(UiRootNode *root);

/* 0x0054C7D0 */
void __thandor_preserve_eax_edx
FrontendTransferPage_ValidateInputAndRequestMailbox(UiTextEditControl *transferPageControl);

/* 0x0054CE10 */
void __thandor_preserve_eax FrontendTransferPage_OpenAndRequestMailbox(UiNodeBase *source);

/* 0x0054C830 */
void __thandor_void_preserve_eax_ecx_edx
FrontendNetworkSetupPage_InitializeFromCommandLine
          (FrontendNetworkSettingsControlView250 *networkPage);

/* 0x0054CE80 */
void __thandor_void_preserve_eax_ecx
FrontendNetworkSetupPage_InitializeSingleLocalPlayer
          (FrontendNetworkSettingsControlView250 *networkPage);

#endif /* THANDOR_UI_FRONTEND_NETWORK_H */

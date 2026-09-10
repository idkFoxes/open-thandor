#ifndef THANDOR_UI_FRONTEND_NETWORK_H
#define THANDOR_UI_FRONTEND_NETWORK_H

#include <thandor/generated/v523_types.h>
#include <thandor/core/contracts.h>

/* Submodule: ui/frontend/network. */
/* Functions are grouped by semantic ownership; address comments are executable virtual addresses. */

/* 0x0054C260 */
void FrontendNetworkSetupPage_InitializeBackendMode (FrontendNetworkSettingsControlView250 *networkPage);

/* 0x005474A0 */
void FrontendTeardown_SaveRootStateSnapshot80(UiRootNode *root);

/* 0x0054C7D0 */
undefined8 FrontendTransferPage_ValidateInputAndRequestMailbox(UiTextEditControl *transferPageControl);

/* 0x0054CE10 */
undefined4 FrontendTransferPage_OpenAndRequestMailbox(UiNodeBase *source);

/* 0x0054C830 */
undefined8 FrontendNetworkSetupPage_InitializeFromCommandLine (FrontendNetworkSettingsControlView250 *networkPage);

/* 0x0054CE80 */
undefined4 FrontendNetworkSetupPage_InitializeSingleLocalPlayer (FrontendNetworkSettingsControlView250 *networkPage);

#endif /* THANDOR_UI_FRONTEND_NETWORK_H */

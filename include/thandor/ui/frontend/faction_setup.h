/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/include/thandor/ui/frontend/faction_setup.h
 * Reverse engineering by idkFoxes 2026
 */

#ifndef THANDOR_UI_FRONTEND_FACTION_SETUP_H
#define THANDOR_UI_FRONTEND_FACTION_SETUP_H

#include <thandor/network/protocol/types.h>
#include <thandor/ui/controls/types.h>
#include <thandor/ui/frontend/types.h>
#include <thandor/core/contracts.h>

void FrontendFactionSetupAction_CycleFactionColour(UiNodeBase *factionControl);

void FrontendFactionSetupAction_ToggleFactionActive(UiNodeBase *playerControl);

void FrontendFactionSetupAction_ChooseFaction(UiNodeBase *selectionRowControl);

void FrontendFactionSetupAction_ReturnToMainPage(uint32_t callbackArgument);

void FrontendFactionSetup_CycleFactionColour
          (FrontendIndexedSelectionArgument playerRuntimeId,uint32_t unusedArgument1,uint32_t unusedArgument2,
          FrontendFactionAssignmentIndex rowIndex);

void FrontendFactionSetup_ToggleFactionActive
          (uint32_t playerRuntimeId,uint32_t unusedArgument1,uint32_t unusedArgument2,
          FrontendFactionAssignmentIndex rowIndex);

void FrontendFactionSetup_ChooseFaction
          (FrontendIndexedSelectionArgument playerRuntimeId,uint32_t unusedArgument1,uint32_t unusedArgument2,
          FrontendFactionAssignmentIndex rowIndex);

extern uintptr_t g_FrontendRootNode; /* the FrontendUiImage copy (address) */

#endif /* THANDOR_UI_FRONTEND_FACTION_SETUP_H */

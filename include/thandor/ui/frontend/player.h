/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/include/thandor/ui/frontend/player.h
 * Reverse engineering by idkFoxes 2026
 */

#ifndef THANDOR_UI_FRONTEND_PLAYER_H
#define THANDOR_UI_FRONTEND_PLAYER_H

#include <thandor/generated/types.h>
#include <thandor/core/contracts.h>

/* Submodule: ui/frontend/player. */
/* Functions are grouped by semantic ownership; address comments are executable virtual addresses. */

/* 0x00548CD0 */
void __thandor_void_preserve_eax_ecx_edx
FrontendPlayerMessage_SubmitSevenSlotText(UiTextEditControl *textEditControl);

/* 0x00560750 */
void __thandor_preserve_eax
FrontendPlayerRuntime_AssignModelAndArmyTokensAndRefreshLocalPanel
          (FrontendPlayerIndex playerIndex,uint32_t reservedZero,RuntimeToken armyToken,
          RuntimeToken modelToken);

/* 0x00549AF0 */
void __thandor_preserve_eax
FrontendPlayerConsensus_SubmitSelectedValue(FrontendConsensusSourceAddress32 source);

/* 0x0054D3A0 */
void __thandor_void_preserve_eax_ecx_edx
FrontendPlayerSetup_ExpireSelectedRuntimeBlock(UiRootNode *rootNode);

/* 0x0054F540 */
void __thandor_void_preserve_eax_ecx_edx
FrontendPlayerRuntime_DecrementTimeoutsAndRemoveExpiredPeers(void);

/* 0x00514EF0 */
bool __thandor_cf_preserve_eax_ecx_edx
FrontendPlayerRuntime_HasOtherPlayerWithAssignmentToken
          (RuntimeToken assignmentToken,PlayerRuntimeId excludedPlayerId);

/* 0x00514F60 */
void __thandor_void_preserve_eax_ecx_edx
FrontendPlayerRuntime_ClearAssignmentTokenFromAll(RuntimeToken assignmentToken);

/* 0x00544130 */
void __thandor_void_preserve_eax_ecx_edx
FrontendPlayerRuntime_MarkReadyAndUpdateActionFlag08
          (PlayerRuntimeId playerId,uint32_t argument2,uint32_t argument3,uint32_t argument4);

/* 0x005442B0 */
void __thandor_void_preserve_eax_ecx_edx
FrontendPlayerRuntime_MarkFlag08ById
          (PlayerRuntimeId playerId,uint32_t unusedArgument1,uint32_t unusedArgument2,uint32_t unusedArgument3);

/* 0x00544300 */
void __thandor_void_preserve_eax_ecx_edx
FrontendPlayerRuntime_XorStateMaskByPlayerId
          (PlayerRuntimeId playerId,uint32_t unusedArg1,uint32_t unusedArg2,uint32_t stateMask);

/* 0x00544360 */
void __thandor_void_preserve_eax_ecx_edx
FrontendPlayerRuntime_MarkFlag04ById
          (PlayerRuntimeId playerId,uint32_t unusedArgument1,uint32_t unusedArgument2,uint32_t unusedArgument3);

/* 0x00544820 */
void __thandor_void_preserve_eax_ecx_edx
FrontendPlayerRuntime_MarkFlag02ById
          (PlayerRuntimeId playerId,uint32_t unusedArgument1,uint32_t unusedArgument2,uint32_t unusedArgument3);

/* 0x00544D50 */
void __thandor_void_preserve_eax_ecx_edx
FrontendPlayerRuntime_MarkFlag01AndStoreValuesById
          (PlayerRuntimeId playerId,FrontendPlayerValue8C scenarioAvailabilityMask2,
          FrontendPlayerValue88 scenarioAvailabilityMask1,
          FrontendPlayerValue84 scenarioAvailabilityMask0);

/* 0x00549190 */
void __thandor_void_preserve_eax_ecx_edx FrontendPlayerRuntime_InitializeFactionAssignments(void);

/* 0x0054D000 */
void __thandor_preserve_eax FrontendPlayerSetup_OpenLocalPageAndResetRoster(UiNodeBase *source);

/* 0x0054D1B0 */
void __thandor_preserve_eax FrontendPlayerSetup_SelectCountAndBuildLabel(UiNodeBase *source);

/* 0x0054D720 */
void __thandor_void_preserve_eax_ecx_edx
FrontendPlayerRuntime_UpdateAction2006ByFlag100Fraction(void);

/* 0x0055F470 */
void __thandor_void_preserve_eax_ecx_edx
FrontendPlayerRuntime_SetReadyFlagById
          (PlayerRuntimeId playerRuntimeId,uint32_t reservedArg04,uint32_t reservedArg08,
          FrontendReadyFlagMask readyFlagMask);

/* 0x0055F5A0 */
void __thandor_void_preserve_eax_ecx_edx
FrontendPlayerRuntime_MarkReadyByIdAndUpdateAction101B(PlayerRuntimeId playerRuntimeId);

/* 0x0055F680 */
void __thandor_void_preserve_eax_ecx_edx
FrontendPlayerRuntime_IncrementReadyCountAndResolveConsensus
          (PlayerRuntimeId playerRuntimeId,uint32_t unusedArgument1,uint32_t unusedArgument2,
          uint32_t unusedArgument3);

/* 0x0055FB90 */
void __thandor_preserve_eax
FrontendPlayerSelection_InsertThreeEntriesAndRefresh
          (PlayerRuntimeId playerRuntimeId,ArmyRuntimeSavedOffset armyRuntimeOffset2,
          ArmyRuntimeSavedOffset armyRuntimeOffset1,ArmyRuntimeSavedOffset armyRuntimeOffset0);

/* 0x0055FC30 */
void __thandor_preserve_eax
FrontendPlayerSelection_RemoveThreeEntriesAndRefresh
          (FrontendPlayerIndex playerIndex,RuntimeToken selectionEntryToken2,
          RuntimeToken selectionEntryToken1,RuntimeToken selectionEntryToken0);

/* 0x0055FCD0 */
void __thandor_preserve_eax
FrontendPlayerSelection_ClearAndRefreshLocalPanels
          (FrontendPlayerIndex playerIndex,uint32_t callbackArg1,uint32_t callbackArg2,uint32_t callbackArg3);

/* 0x0055FD10 */
void __thandor_void_preserve_eax_ecx_edx
FrontendPlayerSelection_TransferFactionGroupWithModeAndRefresh
          (PlayerRuntimeId playerRuntimeId,FactionRuntimeIndex factionIndex,
          FrontendSelectionTransferModeFlags transferModeFlags,
          FrontendFactionAssignmentIndex selectionGroupIndex);

/* 0x00560830 */
void __thandor_void_preserve_eax_ecx_edx
FrontendPlayerRuntime_ClearArmyTokenAndRestoreOrApplyTechnology
          (FrontendPlayerIndex playerIndex,uint32_t arg1,
          TechnologyIndexOrRestoreCode technologyIndexOrRestore,ArmyRuntimeSavedOffset modelOffset);

/* 0x005608A0 */
void __thandor_preserve_eax_edx
FrontendPlayerTextCommand_SetPackedState
          (FrontendPlayerIndex playerIndex,uint32_t arg1,uint32_t arg2,
          FrontendPackedTextCommandState packedState);

/* 0x005608D0 */
void __thandor_void_preserve_eax_ecx_edx
FrontendPlayerTextCommand_AppendTripleClamped
          (FrontendPlayerIndex playerIndex,FrontendTextCommandValue2 value2,
          FrontendTextCommandValue1 value1,FrontendTextCommandValue0 value0);

/* 0x00560940 */
void __thandor_void_preserve_eax_ecx_edx
FrontendPlayerTextCommand_PublishConditionalRichText
          (FrontendPlayerIndex playerIndex,uint32_t arg1,uint32_t arg2,uint32_t arg3);

/* 0x00561F10 */
void __thandor_void_preserve_eax_ecx
FrontendPlayerSelection_ApplyEntryOrAll
          (FrontendPlayerIndex playerIndex,uint32_t reservedZero0,uint32_t reservedZero1,
          RuntimeToken selectionEntryToken);

/* 0x00544020 */
void __thandor_void_preserve_eax_ecx
FrontendPlayerRuntime_RecordReadyAndUpdateWaitState
          (PlayerRuntimeId playerId,uint32_t unusedArgument1,uint32_t unusedArgument2,uint32_t unusedArgument3);

/* 0x00544770 */
void __thandor_void_preserve_eax_ecx_edx
FrontendPlayerRuntime_SetConsensusValueAndRefresh
          (PlayerRuntimeId playerId,uint32_t argument2,uint32_t argument3,
          FrontendConsensusValue consensusValue);

/* 0x00545490 */
void __thandor_void_preserve_eax_ecx
FrontendPlayerMessageBuffer_ResetWriteOffsetTo4ById
          (PlayerRuntimeId playerId,uint32_t arg1,uint32_t arg2,uint32_t arg3);

/* 0x00545500 */
void __thandor_void_preserve_eax_ecx_edx
FrontendPlayerMessageBuffer_AppendTripleById
          (PlayerRuntimeId playerId,FrontendMessageValueA valueA,FrontendMessageValueB valueB,
          FrontendMessageValueC valueC);

/* 0x00545590 */
void __thandor_void_preserve_eax_ecx
FrontendPlayerMessageBuffer_PublishTextById
          (PlayerRuntimeId playerId,uint32_t arg1,uint32_t arg2,uint32_t arg3);

/* 0x0054EBD0 */
void __thandor_void_preserve_eax_ecx_edx
FrontendPlayerRuntime_DecrementExpiryAndCompactBlocks
          (FrontendNetworkListsRuntimeView5650 *frontendRoot);

/* 0x0055FAD0 */
void __thandor_preserve_eax
FrontendPlayerRuntime_AssignModelTokenAndRefreshSelection
          (FactionRuntimeIndex playerIndex,uint32_t reservedZero0,uint32_t reservedZero1,
          RuntimeToken modelToken);

/* 0x005607E0 */
void __thandor_void_preserve_eax_ecx
FrontendPlayerRuntime_AssignArmyTokenAndCaptureFlag80
          (FrontendPlayerIndex playerIndex,uint32_t arg1,uint32_t arg2,ArmyRuntimeSavedOffset modelOffset);

#endif /* THANDOR_UI_FRONTEND_PLAYER_H */

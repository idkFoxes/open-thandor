#ifndef THANDOR_UI_FRONTEND_PLAYER_H
#define THANDOR_UI_FRONTEND_PLAYER_H

#include <thandor/generated/v523_types.h>
#include <thandor/core/contracts.h>

/* Submodule: ui/frontend/player. */
/* Functions are grouped by semantic ownership; address comments are executable virtual addresses. */

/* 0x00548CD0 */
undefined8 FrontendPlayerMessage_SubmitSevenSlotText(UiTextEditControl *textEditControl);

/* 0x00560750 */
void FrontendPlayerRuntime_AssignModelAndArmyTokensAndRefreshLocalPanel (FrontendPlayerIndex playerIndex,undefined4 param_2,RuntimeToken armyToken, RuntimeToken modelToken);

/* 0x00549AF0 */
undefined4 FrontendPlayerConsensus_SubmitSelectedValue(FrontendConsensusSourceAddress32 source);

/* 0x0054D3A0 */
undefined8 FrontendPlayerSetup_ExpireSelectedRuntimeBlock(UiRootNode *rootNode);

/* 0x0054F540 */
void __cdecl FrontendPlayerRuntime_DecrementTimeoutsAndRemoveExpiredPeers(void);

/* 0x00514EF0 */
undefined8 FrontendPlayerRuntime_HasOtherPlayerWithAssignmentTokenCf (RuntimeToken assignmentToken,PlayerRuntimeId excludedPlayerId);

/* 0x00514F60 */
undefined8 FrontendPlayerRuntime_ClearAssignmentTokenFromAll(RuntimeToken assignmentToken);

/* 0x00544130 */
void FrontendPlayerRuntime_MarkReadyAndUpdateActionFlag08 (PlayerRuntimeId playerId,dword argument2,dword argument3,dword argument4);

/* 0x005442B0 */
void FrontendPlayerRuntime_MarkFlag08ById (PlayerRuntimeId playerId,dword argument1,dword argument2,dword argument3);

/* 0x00544300 */
undefined4 FrontendPlayerRuntime_XorStateMaskByPlayerId (int param_1,undefined4 param_2,undefined4 param_3,uint param_4);

/* 0x00544360 */
undefined4 FrontendPlayerRuntime_MarkFlag04ById (PlayerRuntimeId playerId,dword argument1,dword argument2,dword argument3);

/* 0x00544820 */
void FrontendPlayerRuntime_MarkFlag02ById (PlayerRuntimeId playerId,dword argument1,dword argument2,dword argument3);

/* 0x00544D50 */
undefined4 FrontendPlayerRuntime_MarkFlag01AndStoreValuesById (PlayerRuntimeId playerId,FrontendPlayerValue8C value8C,FrontendPlayerValue88 value88, FrontendPlayerValue84 value84);

/* 0x00549190 */
void __fastcall FrontendPlayerRuntime_InitializeFactionAssignments(undefined4 param_1,undefined4 param_2);

/* 0x0054D000 */
undefined4 FrontendPlayerSetup_OpenLocalPageAndResetRoster(UiNodeBase *source);

/* 0x0054D1B0 */
undefined4 FrontendPlayerSetup_SelectCountAndBuildLabel(UiNodeBase *source);

/* 0x0054D720 */
void __cdecl FrontendPlayerRuntime_UpdateAction2006ByFlag100Fraction(void);

/* 0x0055F470 */
void FrontendPlayerRuntime_SetReadyFlagById (PlayerRuntimeId playerRuntimeId,dword reservedArg04,dword reservedArg08, FrontendReadyFlagMask readyFlagMask);

/* 0x0055F5A0 */
void __fastcall FrontendPlayerRuntime_MarkReadyByIdAndUpdateAction101B (undefined4 param_1,undefined4 param_2,PlayerRuntimeId playerRuntimeId);

/* 0x0055F680 */
void __fastcall FrontendPlayerRuntime_IncrementReadyCountAndResolveConsensus (undefined4 param_1,undefined4 param_2,PlayerRuntimeId playerRuntimeId);

/* 0x0055FB90 */
undefined4 __fastcall FrontendPlayerSelection_InsertThreeEntriesAndRefresh (undefined4 param_1,undefined4 param_2,PlayerRuntimeId playerRuntimeId, ArmyRuntimeSavedOffset armyRuntimeOffset2,ArmyRuntimeSavedOffset armyRuntimeOffset1, ArmyRuntimeSavedOffset armyRuntimeOffset0);

/* 0x0055FC30 */
undefined4 FrontendPlayerSelection_RemoveThreeEntriesAndRefresh (FrontendPlayerIndex playerIndex,RuntimeToken selectionEntryToken2, RuntimeToken selectionEntryToken1,RuntimeToken selectionEntryToken0);

/* 0x0055FCD0 */
void FrontendPlayerSelection_ClearAndRefreshLocalPanels (FrontendPlayerIndex playerIndex,dword callbackArg1,dword callbackArg2, dword callbackArg3);

/* 0x0055FD10 */
void FrontendPlayerSelection_TransferFactionGroupWithModeAndRefresh (PlayerRuntimeId playerRuntimeId,FactionRuntimeIndex factionIndex, FrontendSelectionTransferModeFlags transferModeFlags, FrontendFactionAssignmentIndex selectionGroupIndex);

/* 0x00560830 */
void FrontendPlayerRuntime_ClearArmyTokenAndRestoreOrApplyTechnology (FrontendPlayerIndex playerIndex,dword arg1, TechnologyIndexOrRestoreCode technologyIndexOrRestore, ArmyRuntimeSavedOffset modelOffset);

/* 0x005608A0 */
void FrontendPlayerTextCommand_SetPackedState (FrontendPlayerIndex playerIndex,dword arg1,dword arg2, FrontendPackedTextCommandState packedState);

/* 0x005608D0 */
void FrontendPlayerTextCommand_AppendTripleClamped (FrontendPlayerIndex playerIndex,FrontendTextCommandValue2 value2, FrontendTextCommandValue1 value1,FrontendTextCommandValue0 value0);

/* 0x00560940 */
void FrontendPlayerTextCommand_PublishConditionalRichText (FrontendPlayerIndex playerIndex,dword arg1,dword arg2,dword arg3);

/* 0x00561F10 */
void FrontendPlayerSelection_ApplyEntryOrAll (FrontendPlayerIndex playerIndex,undefined4 param_2,undefined4 param_3, RuntimeToken selectionEntryToken);

/* 0x00544020 */
void FrontendPlayerRuntime_RecordReadyAndUpdateWaitState (PlayerRuntimeId playerId,dword argument2,dword argument3,dword argument4);

/* 0x00544770 */
void FrontendPlayerRuntime_SetConsensusValueAndRefresh (PlayerRuntimeId playerId,dword argument2,dword argument3, FrontendConsensusValue consensusValue);

/* 0x00545490 */
void FrontendPlayerMessageBuffer_ResetWriteOffsetTo4ById (PlayerRuntimeId playerId,dword arg1,dword arg2,dword arg3);

/* 0x00545500 */
void FrontendPlayerMessageBuffer_AppendTripleById (PlayerRuntimeId playerId,FrontendMessageValueA valueA,FrontendMessageValueB valueB, FrontendMessageValueC valueC);

/* 0x00545590 */
void FrontendPlayerMessageBuffer_PublishTextById (PlayerRuntimeId playerId,dword arg1,dword arg2,dword arg3);

/* 0x0054EBD0 */
void FrontendPlayerRuntime_DecrementExpiryAndCompactBlocks(UiRootNode *frontendRoot);

/* 0x0055FAD0 */
void FrontendPlayerRuntime_AssignModelTokenAndRefreshSelection (FactionRuntimeIndex playerIndex,undefined4 param_2,undefined4 param_3, RuntimeToken modelToken);

/* 0x005607E0 */
void FrontendPlayerRuntime_AssignArmyTokenAndCaptureFlag80 (FrontendPlayerIndex playerIndex,dword arg1,dword arg2, ArmyRuntimeSavedOffset modelOffset);

#endif /* THANDOR_UI_FRONTEND_PLAYER_H */

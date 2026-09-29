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

/* Selection groups (keys 1..8): each faction record holds 8 groups of 32 army pointers (runtimeGroupMembers8x32),
   a player's selection holds 32 entries. transferModeFlags of
   FrontendPlayerSelection_TransferFactionGroupWithModeAndRefresh: */
#define SELECTION_GROUP_COUNT 8
#define SELECTION_GROUP_ENTRY_COUNT 32
#define SELECTION_TRANSFER_TO_GROUP 0x1 /* selection -> group (else group -> selection); its entries are first
                                           removed from every group of the faction */
#define SELECTION_TRANSFER_MERGE 0x2 /* add missing entries instead of replacing the destination */
#define SELECTION_TRANSFER_CENTER_VIEW 0x4 /* local player: move the camera to the selection's average position */
/* Chat. A line is sent as a begin command, four append commands of 12 narrow bytes (3 dwords) each and a
   publish command. Lobby: one 100-byte record per player block in g_FrontendPlayerMessageBuffers, dword 0 the
   byte offset of the next write (starting after itself), then the 0x30-byte text. In game the text is staged
   at +0x80C0 of the player's SelectionPlayerRuntimeBlock, the write offset in the low byte of +0x809C. */
#define FRONTEND_PLAYER_MESSAGE_RECORD_BYTES 100
#define FRONTEND_PLAYER_MESSAGE_TEXT_OFFSET 4
#define PLAYER_CHAT_TEXT_BYTES 0x30
#define TEXT_ID_CHAT_MESSAGE 0xFF07 /* rich text: selector 0 = sender name, selector 1 = message */
/* Recipient mask of a chat line to everybody (bits 8+faction and 16+player); the lobby ignores the mask. */
#define PLAYER_CHAT_RECIPIENTS_ALL 0xFFFFFF00
/* In-game chat staging: the write offset (low byte of packedSelectionState809C) advances 12 bytes per piece
   and stops at the last piece of the 0x30-byte line. */
#define PLAYER_CHAT_PIECE_BYTES 0xC
#define PLAYER_CHAT_LAST_PIECE_OFFSET 0x24
/* The player record whose playerName field name points at (loops that walk the records by their names). */
#define FRONTEND_PLAYER_RECORD_OF_NAME(name) \
  ((FrontendPlayerRuntimeRecord *)((uint8_t *)(name) - offsetof(FrontendPlayerRuntimeRecord,playerName)))
/* Functions are grouped by semantic ownership; address comments are executable virtual addresses. */

/* 0x00548CD0 */
void FrontendPlayerMessage_SubmitSevenSlotText(UiTextEditControl *textEditControl);

/* 0x00560750 */
void FrontendPlayerRuntime_AssignModelAndArmyTokensAndRefreshLocalPanel
          (FrontendPlayerIndex playerIndex,uint32_t reservedZero,RuntimeToken armyToken,
          RuntimeToken modelToken);

/* 0x00549AF0 */
void FrontendPlayerConsensus_SubmitSelectedValue(FrontendConsensusSourceAddress32 source);

/* 0x0054D3A0 */
void FrontendPlayerSetup_ExpireSelectedRuntimeBlock(UiRootNode *rootNode);

/* 0x0054F540 */
void FrontendPlayerRuntime_DecrementTimeoutsAndRemoveExpiredPeers(void);

/* 0x00514EF0 */
bool FrontendPlayerRuntime_HasOtherPlayerWithAssignmentToken
          (RuntimeToken assignmentToken,PlayerRuntimeId excludedPlayerId);

/* 0x00514F60 */
void FrontendPlayerRuntime_ClearAssignmentTokenFromAll(RuntimeToken assignmentToken);

/* 0x00544130 */
void FrontendPlayerRuntime_MarkBriefingReadyAndUpdateBeginButton
          (PlayerRuntimeId playerId,uint32_t unusedArgument1,uint32_t unusedArgument2,uint32_t unusedArgument3);

/* 0x005442B0 */
void FrontendPlayerRuntime_MarkLevelReceivedById
          (PlayerRuntimeId playerId,uint32_t unusedArgument1,uint32_t unusedArgument2,uint32_t unusedArgument3);

/* 0x00544300 */
void FrontendPlayerRuntime_XorStateMaskByPlayerId
          (PlayerRuntimeId playerId,uint32_t unusedArg1,uint32_t unusedArg2,uint32_t stateMask);

/* 0x00544360 */
void FrontendPlayerRuntime_MarkLevelLoadedById
          (PlayerRuntimeId playerId,uint32_t unusedArgument1,uint32_t unusedArgument2,uint32_t unusedArgument3);

/* 0x00544820 */
void FrontendPlayerRuntime_MarkTaskAssignmentReadyById
          (PlayerRuntimeId playerId,uint32_t unusedArgument1,uint32_t unusedArgument2,uint32_t unusedArgument3);

/* 0x00544D50 */
void FrontendPlayerRuntime_MarkScenarioCatalogReceivedById
          (PlayerRuntimeId playerId,FrontendPlayerValue8C scenarioAvailabilityMask2,
          FrontendPlayerValue88 scenarioAvailabilityMask1,
          FrontendPlayerValue84 scenarioAvailabilityMask0);

/* 0x00549190 */
void FrontendPlayerRuntime_InitializeFactionAssignments(void);

/* 0x0054D000 */
void FrontendPlayerSetup_OpenLocalPageAndResetRoster(UiNodeBase *source);

/* 0x0054D1B0 */
void FrontendPlayerSetup_SelectCountAndBuildLabel(UiNodeBase *source);

/* 0x0054D720 */
void FrontendPlayerRuntime_UpdateStartButtonByCdShare(void);

/* 0x0055F470 */
void FrontendPlayerRuntime_SetSlowRenderingFlagById
          (PlayerRuntimeId playerRuntimeId,uint32_t reservedArg04,uint32_t reservedArg08,
          FrontendReadyFlagMask slowRenderingFlag);

/* 0x0055F5A0 */
void FrontendPlayerRuntime_MarkResultsReadyAndUpdateContinueButton(PlayerRuntimeId playerRuntimeId);

/* 0x0055F680 */
void FrontendPlayerRuntime_IncrementReadyCountAndResolveConsensus
          (PlayerRuntimeId playerRuntimeId,uint32_t unusedArgument1,uint32_t unusedArgument2,
          uint32_t unusedArgument3);

/* 0x0055FB90 */
void FrontendPlayerSelection_InsertThreeEntriesAndRefresh
          (PlayerRuntimeId playerRuntimeId,ArmyRuntimeSavedOffset armyRuntimeOffset2,
          ArmyRuntimeSavedOffset armyRuntimeOffset1,ArmyRuntimeSavedOffset armyRuntimeOffset0);

/* 0x0055FC30 */
void FrontendPlayerSelection_RemoveThreeEntriesAndRefresh
          (FrontendPlayerIndex playerRuntimeId,RuntimeToken armyRuntimeOffset2,
          RuntimeToken armyRuntimeOffset1,RuntimeToken armyRuntimeOffset0);

/* 0x0055FCD0 */
void FrontendPlayerSelection_ClearAndRefreshLocalPanels
          (FrontendPlayerIndex playerRuntimeId,uint32_t unusedArg1,uint32_t unusedArg2,uint32_t unusedArg3);

/* 0x0055FD10 */
void FrontendPlayerSelection_TransferFactionGroupWithModeAndRefresh
          (PlayerRuntimeId playerRuntimeId,FactionRuntimeIndex factionIndex,
          FrontendSelectionTransferModeFlags transferModeFlags,
          FrontendFactionAssignmentIndex selectionGroupIndex);

/* 0x00560830 */
void FrontendPlayerRuntime_ClearArmyTokenAndRestoreOrApplyTechnology(FrontendPlayerIndex playerIndex,uint32_t arg1,
          TechnologyIndexOrRestoreCode technologyIndexOrRestore,ArmyRuntimeSavedOffset modelOffset);

/* 0x005608A0 */
void FrontendPlayerTextCommand_SetPackedState(FrontendPlayerIndex playerIndex,uint32_t arg1,uint32_t arg2,
          FrontendPackedTextCommandState packedState);

/* 0x005608D0 */
void FrontendPlayerTextCommand_AppendTripleClamped(FrontendPlayerIndex playerIndex,FrontendTextCommandValue2 value2,
          FrontendTextCommandValue1 value1,FrontendTextCommandValue0 value0);

/* 0x00560940 */
void FrontendPlayerTextCommand_PublishConditionalRichText
          (FrontendPlayerIndex playerIndex,uint32_t arg1,uint32_t arg2,uint32_t arg3);

/* 0x00561F10 */
void FrontendPlayerSelection_ApplyEntryOrAll
          (FrontendPlayerIndex playerIndex,uint32_t reservedZero0,uint32_t reservedZero1,
          RuntimeToken armyRuntimeOffset);

/* 0x00544020 */
void FrontendPlayerRuntime_RecordReadyAndUpdateWaitState
          (PlayerRuntimeId playerId,uint32_t unusedArgument1,uint32_t unusedArgument2,uint32_t unusedArgument3);

/* 0x00544770 */
void FrontendPlayerRuntime_SetConsensusValueAndRefresh
          (PlayerRuntimeId playerId,uint32_t unusedArgument1,uint32_t unusedArgument2,
          FrontendConsensusValue consensusValue);

/* 0x00545490 */
void FrontendPlayerMessageBuffer_ResetWriteOffsetTo4ById
          (PlayerRuntimeId playerId,uint32_t arg1,uint32_t arg2,uint32_t arg3);

/* 0x00545500 */
void FrontendPlayerMessageBuffer_AppendTripleById
          (PlayerRuntimeId playerId,FrontendMessageValueA valueA,FrontendMessageValueB valueB,
          FrontendMessageValueC valueC);

/* 0x00545590 */
void FrontendPlayerMessageBuffer_PublishTextById(PlayerRuntimeId playerId,uint32_t arg1,uint32_t arg2,uint32_t arg3);

/* 0x0054EBD0 */
void FrontendPlayerRuntime_DecrementExpiryAndCompactBlocks(FrontendNetworkListsRuntimeView *frontendRoot);

/* 0x0055FAD0 */
void FrontendPlayerRuntime_AssignModelTokenAndRefreshSelection
          (FactionRuntimeIndex playerRuntimeId,uint32_t unusedArg1,uint32_t unusedArg2,
          RuntimeToken armyRuntimeOffset);

/* 0x005607E0 */
void FrontendPlayerRuntime_AssignArmyTokenAndCaptureFlag80
          (FrontendPlayerIndex playerIndex,uint32_t arg1,uint32_t arg2,ArmyRuntimeSavedOffset modelOffset);

#endif /* THANDOR_UI_FRONTEND_PLAYER_H */

/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/include/thandor/ui/frontend/player.h
 * Reverse engineering by idkFoxes 2026
 */

#ifndef THANDOR_UI_FRONTEND_PLAYER_H
#define THANDOR_UI_FRONTEND_PLAYER_H

#include <thandor/core/types.h>
#include <thandor/gameplay/army/types.h>
#include <thandor/network/protocol/types.h>
#include <thandor/ui/controls/types.h>
#include <thandor/ui/frontend/types.h>
#include <thandor/world/terrain/types.h>
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
   in chatStagingText of the player's SelectionPlayerRuntimeBlock, the write offset in the low byte of
   chatRecipientMaskAndWriteOffset. */
#define FRONTEND_PLAYER_MESSAGE_RECORD_BYTES 100
#define FRONTEND_PLAYER_MESSAGE_TEXT_OFFSET 4
#define PLAYER_CHAT_TEXT_BYTES 0x30
#define TEXT_ID_CHAT_MESSAGE 0xFF07 /* rich text: selector 0 = sender name, selector 1 = message */
/* Recipient mask of a chat line to everybody (bits 8+faction and 16+player); the lobby ignores the mask. */
#define PLAYER_CHAT_RECIPIENTS_ALL 0xFFFFFF00
/* In-game chat staging: the write offset (low byte of chatRecipientMaskAndWriteOffset) advances 12 bytes per piece
   and stops at the last piece of the 0x30-byte line. */
#define PLAYER_CHAT_PIECE_BYTES 0xC
#define PLAYER_CHAT_LAST_PIECE_OFFSET 0x24
/* The player record whose playerName field name points at (loops that walk the records by their names). */
#define FRONTEND_PLAYER_RECORD_OF_NAME(name) \
  ((FrontendPlayerRuntimeRecord *)((uint8_t *)(name) - offsetof(FrontendPlayerRuntimeRecord,playerName)))
/* Functions are grouped by semantic ownership. */

void FrontendPlayerMessage_SubmitSevenSlotText(UiTextEditControl *textEditControl);

void FrontendPlayerRuntime_AssignModelAndArmyTokensAndRefreshLocalPanel
          (FrontendPlayerIndex playerIndex,uint32_t reservedZero,RuntimeToken armyToken,
          RuntimeToken modelToken);

void FrontendPlayerConsensus_SubmitSelectedValue(UiNodeBase *source);

void FrontendPlayerSetup_ExpireSelectedRuntimeBlock(UiRootNode *rootNode);

void FrontendPlayerRuntime_DecrementTimeoutsAndRemoveExpiredPeers(void);

Bool8 FrontendPlayerRuntime_HasOtherPlayerWithAssignmentToken
          (uintptr_t assignmentToken,PlayerRuntimeId excludedPlayerId); /* the building's address */

void FrontendPlayerRuntime_ClearAssignmentTokenFromAll(uintptr_t assignmentToken); /* the building's address */

void FrontendPlayerRuntime_MarkBriefingReadyAndUpdateBeginButton
          (PlayerRuntimeId playerId,uint32_t unusedArgument1,uint32_t unusedArgument2,uint32_t unusedArgument3);

void FrontendPlayerRuntime_MarkLevelReceivedById
          (PlayerRuntimeId playerId,uint32_t unusedArgument1,uint32_t unusedArgument2,uint32_t unusedArgument3);

void FrontendPlayerRuntime_XorStateMaskByPlayerId
          (PlayerRuntimeId playerId,uint32_t unusedArg1,uint32_t unusedArg2,uint32_t stateMask);

void FrontendPlayerRuntime_MarkLevelLoadedById
          (PlayerRuntimeId playerId,uint32_t unusedArgument1,uint32_t unusedArgument2,uint32_t unusedArgument3);

void FrontendPlayerRuntime_MarkTaskAssignmentReadyById
          (PlayerRuntimeId playerId,uint32_t unusedArgument1,uint32_t unusedArgument2,uint32_t unusedArgument3);

void FrontendPlayerRuntime_MarkScenarioCatalogReceivedById
          (PlayerRuntimeId playerId,FrontendScenarioAvailabilityMask2 scenarioAvailabilityMask2,
          FrontendScenarioAvailabilityMask1 scenarioAvailabilityMask1,
          FrontendScenarioAvailabilityMask0 scenarioAvailabilityMask0);

void FrontendPlayerRuntime_InitializeFactionAssignments(void);

void FrontendPlayerSetup_OpenLocalPageAndResetRoster(UiNodeBase *source);

void FrontendNetworkSettings_SetNetworkSpeed(UiNodeBase *source);

void FrontendPlayerRuntime_UpdateStartButtonByCdShare(void);

void FrontendPlayerRuntime_SetSlowRenderingFlagById
          (PlayerRuntimeId playerRuntimeId,uint32_t reservedArg04,uint32_t reservedArg08,
          FrontendReadyFlagMask slowRenderingFlag);

void FrontendPlayerRuntime_MarkResultsReadyAndUpdateContinueButton(PlayerRuntimeId playerRuntimeId);

void FrontendPlayerRuntime_IncrementReadyCountAndResolveConsensus
          (PlayerRuntimeId playerRuntimeId,uint32_t unusedArgument1,uint32_t unusedArgument2,
          uint32_t unusedArgument3);

void FrontendPlayerSelection_InsertThreeEntriesAndRefresh
          (PlayerRuntimeId playerRuntimeId,ArmyRuntimeSavedOffset armyRuntimeOffset2,
          ArmyRuntimeSavedOffset armyRuntimeOffset1,ArmyRuntimeSavedOffset armyRuntimeOffset0);

void FrontendPlayerSelection_RemoveThreeEntriesAndRefresh
          (FrontendPlayerIndex playerRuntimeId,RuntimeToken armyRuntimeOffset2,
          RuntimeToken armyRuntimeOffset1,RuntimeToken armyRuntimeOffset0);

void FrontendPlayerSelection_ClearAndRefreshLocalPanels
          (FrontendPlayerIndex playerRuntimeId,uint32_t unusedArg1,uint32_t unusedArg2,uint32_t unusedArg3);

void FrontendPlayerSelection_TransferFactionGroupWithModeAndRefresh
          (PlayerRuntimeId playerRuntimeId,FactionRuntimeIndex factionIndex,
          FrontendSelectionTransferModeFlags transferModeFlags,
          FrontendFactionAssignmentIndex selectionGroupIndex);

void FrontendPlayerRuntime_ClearArmyTokenAndRestoreOrApplyTechnology(FrontendPlayerIndex playerIndex,uint32_t unusedArg1,
          TechnologyIndexOrRestoreCode technologyIndexOrRestore,ArmyRuntimeSavedOffset modelOffset);

void FrontendPlayerTextCommand_SetPackedState(FrontendPlayerIndex playerIndex,uint32_t unusedArg1,uint32_t unusedArg2,
          FrontendPackedTextCommandState packedState);

void FrontendPlayerTextCommand_AppendTripleClamped(FrontendPlayerIndex playerIndex,FrontendTextCommandValue2 value2,
          FrontendTextCommandValue1 value1,FrontendTextCommandValue0 value0);

void FrontendPlayerTextCommand_PublishConditionalRichText
          (FrontendPlayerIndex playerIndex,uint32_t unusedArg1,uint32_t unusedArg2,uint32_t unusedArg3);

void FrontendPlayerSelection_ApplyEntryOrAll
          (FrontendPlayerIndex playerIndex,uint32_t reservedZero0,uint32_t reservedZero1,
          RuntimeToken armyRuntimeOffset);

void FrontendPlayerRuntime_RecordReadyAndUpdateWaitState
          (PlayerRuntimeId playerId,uint32_t unusedArgument1,uint32_t unusedArgument2,uint32_t unusedArgument3);

void FrontendPlayerRuntime_SetConsensusValueAndRefresh
          (PlayerRuntimeId playerId,uint32_t unusedArgument1,uint32_t unusedArgument2,
          FrontendConsensusValue consensusValue);

void FrontendPlayerMessageBuffer_ResetWriteOffsetTo4ById
          (PlayerRuntimeId playerId,uint32_t unusedArg1,uint32_t unusedArg2,uint32_t unusedArg3);

void FrontendPlayerMessageBuffer_AppendTripleById
          (PlayerRuntimeId playerId,FrontendMessageValueA valueA,FrontendMessageValueB valueB,
          FrontendMessageValueC valueC);

void FrontendPlayerMessageBuffer_PublishTextById(PlayerRuntimeId playerId,uint32_t unusedArg1,uint32_t unusedArg2,
          uint32_t unusedArg3);

void FrontendPlayerRuntime_DecrementExpiryAndCompactBlocks(FrontendNetworkListsRuntimeView *frontendRoot);

void FrontendPlayerRuntime_AssignModelTokenAndRefreshSelection
          (FactionRuntimeIndex playerRuntimeId,uint32_t unusedArg1,uint32_t unusedArg2,
          RuntimeToken armyRuntimeOffset);

void FrontendPlayerRuntime_AssignTechnologyBuildingAndHoldUnpaidResearch
          (FrontendPlayerIndex playerIndex,uint32_t unusedArg1,uint32_t unusedArg2,ArmyRuntimeSavedOffset modelOffset);

extern FrontendPlayerRuntimeRecord *g_FrontendPlayerRuntimeBlocks;
extern FrontendPlayerRuntimeBlockCount g_FrontendPlayerRuntimeBlockCount;
extern UiCommandPayloadTextBatch48 g_UiSevenSlotCommandPayloadText;
extern uint16_t g_FrontendNetworkSpeedLabelUtf16[32]; /* network speed caption, written with a capacity of 64 bytes */

extern uint32_t g_FrontendPlayerMessageBuffers;

#endif /* THANDOR_UI_FRONTEND_PLAYER_H */

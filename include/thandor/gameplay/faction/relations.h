/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/include/thandor/gameplay/faction/relations.h
 * Reverse engineering by idkFoxes 2026
 */

#ifndef THANDOR_GAMEPLAY_FACTION_RELATIONS_H
#define THANDOR_GAMEPLAY_FACTION_RELATIONS_H

#include <thandor/generated/types.h>
#include <thandor/core/contracts.h>

/* Submodule: gameplay/faction/relations. */
/* Functions are grouped by semantic ownership. */

/* Records in a player's marked-cell list (SelectionPlayerRuntimeBlock.markedCells[4096]); the
   PlayerPairList_* functions ignore a list whose count has reached this value. */
#define PLAYER_PAIR_LIST_CAPACITY 4096

/* Relation state nibbles (GameFactionRuntime_GetPackedStateNibble, 0..11): three tiers 0-3, 4-7 and 8-10 with
   the pending states 2, 5 and 9 and the top states 3, 6 and 10; 11 is FACTION_RELATION_MERGE. */
#define FACTION_RELATION_STATE_FRIENDLY 4 /* states 4 and up: same bloc (GameFactionRelations_BuildEligibleFactionMask) */
#define FACTION_RELATION_STATE_ALLIED 8   /* states 8 and up: allied; the game ends when no two active factions are below */
/* GameFactionRuntimeImageTail.relationUiFlags (from the level's LevelWorldSettings.relationUiFlags): relation
   drift freezes */
#define FACTION_RELATION_FREEZE_ALLIED 0x1   /* states 8 and up stay */
#define FACTION_RELATION_FREEZE_FRIENDLY 0x2 /* states 4 and up stay */
#define FACTION_RELATION_FREEZE_ALL 0x4
/* GameFactionRuntimeRecord.packedRelationStates: one relation-state nibble per faction; the nibble bits of
   state (e.g. FACTION_RELATION_STATE_ALLIED) at the given faction */
#define FACTION_RELATION_PACKED(state,factionIndex) ((uint32_t)(state) << ((factionIndex) * 4))
#define FACTION_RELATION_STATE_MASK 0xf /* one relation-state nibble, after shifting it down */
/* GameData_ResetDefaults: packedRelationStates of faction 0 (own nibble 0xF, state 1 toward the seven others);
   each following record gets it rotated left by one nibble, so the 0xF sits at the record's own faction. */
#define FACTION_RELATION_DEFAULT_PATTERN 0x1111111f
/* FactionActiveMask bit of a faction (bit n = faction n) */
#define FACTION_MASK_BIT(factionIndex) (1 << (factionIndex))
/* Random relation drift (GameFactionRelations_MaybeAdvancePairState*): the pair advances when Random & mask
   equals FACTION_RELATION_DRIFT_MATCH; the mask depends on the state tier and whether the pair has pressure */
#define FACTION_RELATION_DRIFT_MATCH 85
#define FACTION_RELATION_DRIFT_RARE_IDLE_MASK 0x17F       /* states outside 3/6/10, no pressure: 1 in 256 */
#define FACTION_RELATION_DRIFT_RARE_PRESSURE_MASK 0x3FF   /* states outside 3/6/10, pressure: 1 in 1024 */
#define FACTION_RELATION_DRIFT_COMMON_IDLE_MASK 0x7F      /* states 3/6/10, no pressure: 1 in 128 */
#define FACTION_RELATION_DRIFT_COMMON_PRESSURE_MASK 0x1FF /* states 3/6/10, pressure: 1 in 512 */
/* GameFactionRelations_MaybeResetPairState: resets when (Random & mask) == match, 1 in 4 */
#define FACTION_RELATION_RESET_RANDOM_MASK 0x180
#define FACTION_RELATION_RESET_RANDOM_MATCH 0x80

void GameFactionRelations_UpdateAllPairsForFaction
          (FactionRuntimeIndex sourceFactionIndex,WorldRuntimeContext *worldRuntime);

void PlayerPairList_InsertRange(PlayerRuntimeId playerRuntimeId,SelectionPlayerPairValue lastWorldXQ12,
          SelectionPlayerPairValue worldYQ12,SelectionPlayerPairValue firstWorldXQ12);

void PlayerPairList_RemoveRange(PlayerRuntimeId playerRuntimeId,SelectionPlayerPairValue lastWorldXQ12,
          SelectionPlayerPairValue worldYQ12,SelectionPlayerPairValue firstWorldXQ12);

Bool8 GameFactionRelations_TestPairTransitionAllowed
          (FactionRuntimeIndex sourceFactionIndex,FactionRuntimeIndex targetFactionIndex);

FactionActiveMask GameFactionRelations_BuildEligibleFactionMask(FactionRuntimeIndex sourceFactionIndex);

Bool8 GameFactionRelations_EvaluateTransitionRules
          (FactionRuntimeIndex focalFactionIndex,FactionActiveMask activeFactionMask);

Bool8 GameFactionRelations_IsNotResetEligibleState
          (FactionRuntimeIndex sourceFactionIndex,FactionRuntimeIndex targetFactionIndex);

void GameFactionRelations_MaybeAdvancePairStateRare
          (FactionRuntimeIndex sourceFactionIndex,FactionRuntimeIndex targetFactionIndex);

void GameFactionRelations_MaybeAdvancePairStateCommon
          (FactionRuntimeIndex sourceFactionIndex,FactionRuntimeIndex targetFactionIndex);

void GameFactionRelations_MaybeResetPairState
          (FactionRuntimeIndex sourceFactionIndex,FactionRuntimeIndex targetFactionIndex);

void PlayerPairList_InsertUnique
          (PlayerRuntimeId playerRuntimeId,uint32_t reservedZero,SelectionPlayerPairValue worldYQ12,
          SelectionPlayerPairValue worldXQ12);

void PlayerPairList_RemoveFirstMatch
          (PlayerRuntimeId playerRuntimeId,uint32_t reservedZero,SelectionPlayerPairValue worldYQ12,
          SelectionPlayerPairValue worldXQ12);

/* GameFactionRuntime_IsRecentTimedRelationState: a pending relation state (2, 5, 9) does not advance again
   within this many simulation ticks of the pair's last change. */
#define FACTION_RELATION_CHANGE_COOLDOWN_TICKS 600

/* Faction merge (relation state 11) without a clear survivor: this random bit clear = the second faction survives */
#define FACTION_MERGE_RANDOM_DIRECTION_BIT 0x2000

void GameFactionRuntime_AdvancePairwiseRelationState(uint32_t unusedRelationArgument0,uint32_t unusedRelationArgument1,
          FactionRuntimeIndex sourceFactionIndex,FactionRuntimeIndex targetFactionIndex);

void GameFactionRuntime_ResetPairwiseRelationState(uint32_t unusedRelationArgument0,uint32_t unusedRelationArgument1,
          FactionRuntimeIndex sourceFactionIndex,FactionRuntimeIndex targetFactionIndex);

void GameFactionRuntime_SynchronizeTechnologiesForRelationStates8To10(void);

Bool8 GameFactionRuntime_IsRecentTimedRelationState
          (FactionRuntimeIndex otherFactionIndex,FactionRuntimeIndex factionIndex);

void GameFactionRuntime_ApplyPairwiseRelationTransition(FactionNotificationCodeBase activeFactionCodeForFirst,
          FactionNotificationCodeBase activeFactionCodeForSecond,
          FactionRelationStateNibble stateFirstTowardSecond,
          FactionRelationStateNibble stateSecondTowardFirst,FactionRuntimeIndex firstFactionIndex,
          FactionRuntimeIndex secondFactionIndex);

#endif /* THANDOR_GAMEPLAY_FACTION_RELATIONS_H */

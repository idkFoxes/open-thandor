/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/src/gameplay/faction/relations.c
 * Reverse engineering by idkFoxes 2026
 */

#include <thandor/gameplay/faction/relations.h>
#include <thandor/thandor.h>

/* Implementation ownership: gameplay/faction/relations. */

/* Address: 0x0053C010.
   Random drift of the diplomatic relations between sourceFactionIndex and every other active faction 7..1
   (faction 0 is never visited). When the pair may change state and sits in state 3, 6 or 10, the relation may
   be reset (GameFactionRelations_IsNotResetEligibleState returns false for exactly those states); otherwise it may
   advance: rarely from the other states, more often from 3, 6 and 10.
*/
void GameFactionRelations_UpdateAllPairsForFaction
          (FactionRuntimeIndex sourceFactionIndex,WorldRuntimeContext *worldRuntime)

{
  int opposingFactionIndex;
  bool pairTestResult;

  opposingFactionIndex = 7;
  do {
    if ((g_GameFactionRuntimeImage.tail.factionLifecycleStates[opposingFactionIndex] ==
         FACTION_RUNTIME_LIFECYCLE_ACTIVE) && (sourceFactionIndex != opposingFactionIndex)) {
      pairTestResult = GameFactionRelations_TestPairTransitionAllowed
                        (opposingFactionIndex,sourceFactionIndex);
      if (pairTestResult) {
        pairTestResult = GameFactionRelations_IsNotResetEligibleState
                          (opposingFactionIndex,sourceFactionIndex);
        if (!pairTestResult) {
          GameFactionRelations_MaybeResetPairState(opposingFactionIndex,sourceFactionIndex);
        }
      }
      else {
        pairTestResult = GameFactionRelations_IsNotResetEligibleState
                          (opposingFactionIndex,sourceFactionIndex);
        if (pairTestResult) {
          GameFactionRelations_MaybeAdvancePairStateRare(opposingFactionIndex,sourceFactionIndex);
        }
        else {
          GameFactionRelations_MaybeAdvancePairStateCommon(opposingFactionIndex,sourceFactionIndex);
        }
      }
    }
    opposingFactionIndex--;
  } while (opposingFactionIndex != 0);
  return;
}


/* Address: 0x00560E30.
   Adds one row of field cells (world X from firstWorldXQ12 to lastWorldXQ12 inclusive, one cell apart, at
   worldYQ12) to the player's marked-cell list. Called per row by the in-game command UI when an area is dragged
   out in a local session (ui/ingame/runtime.c; networked sessions queue command 0x1D00 instead).
*/
void PlayerPairList_InsertRange(PlayerRuntimeId playerRuntimeId,SelectionPlayerPairValue lastWorldXQ12,
          SelectionPlayerPairValue worldYQ12,SelectionPlayerPairValue firstWorldXQ12)

{
  for (; (int)firstWorldXQ12 <= (int)lastWorldXQ12; firstWorldXQ12 = firstWorldXQ12 + FIELD_GRID_CELL_Q12) {
    PlayerPairList_InsertUnique(playerRuntimeId,0,worldYQ12,firstWorldXQ12);
  }
  return;
}


/* Address: 0x00560E70.
   Counterpart of PlayerPairList_InsertRange: removes one row of field cells (world X from firstWorldXQ12 to
   lastWorldXQ12 inclusive, at worldYQ12) from the player's marked-cell list. Called per row by the in-game
   command UI in a local session (ui/ingame/runtime.c; networked sessions queue command 0x1D40 instead).
*/
void PlayerPairList_RemoveRange(PlayerRuntimeId playerRuntimeId,SelectionPlayerPairValue lastWorldXQ12,
          SelectionPlayerPairValue worldYQ12,SelectionPlayerPairValue firstWorldXQ12)

{
  for (; (int)firstWorldXQ12 <= (int)lastWorldXQ12; firstWorldXQ12 = firstWorldXQ12 + FIELD_GRID_CELL_Q12) {
    PlayerPairList_RemoveFirstMatch(playerRuntimeId,0,worldYQ12,firstWorldXQ12);
  }
  return;
}


/* Address: 0x0053C3D0.
   Decides which random drift GameFactionRelations_UpdateAllPairsForFaction applies to a pair. CF set (true)
   selects the reset path: the pending states 2, 5 and 9, relations frozen by relationUiFlags, or a state
   below 4 for which GameFactionRelations_EvaluateTransitionRules holds for either faction. CF clear lets the pair advance.
*/
bool GameFactionRelations_TestPairTransitionAllowed
          (FactionRuntimeIndex sourceFactionIndex,FactionRuntimeIndex targetFactionIndex)

{
  FactionRelationState relationState;
  FactionActiveMask targetEligibleMask;
  FactionActiveMask sourceEligibleMask;
  bool rulesSatisfied;

  relationState = GameFactionRuntime_GetPackedStateNibble(sourceFactionIndex,targetFactionIndex);
  /* relationUiFlags: bit 4 freezes every relation, bit 2 states 4 and up, bit 1 states 8 and up */
  if (relationState != 2 && relationState != 5 && relationState != 9 &&
      (g_GameFactionRuntimeImage.tail.relationUiFlags & FACTION_RELATION_FREEZE_ALL) == 0 &&
      (relationState < FACTION_RELATION_STATE_FRIENDLY ||
       ((g_GameFactionRuntimeImage.tail.relationUiFlags & FACTION_RELATION_FREEZE_FRIENDLY) == 0 &&
        (relationState < FACTION_RELATION_STATE_ALLIED ||
         (g_GameFactionRuntimeImage.tail.relationUiFlags & FACTION_RELATION_FREEZE_ALLIED) == 0)))) {
    if (FACTION_RELATION_STATE_FRIENDLY - 1 < relationState) {
      return false;
    }
    targetEligibleMask = GameFactionRelations_BuildEligibleFactionMask(targetFactionIndex);
    sourceEligibleMask = GameFactionRelations_BuildEligibleFactionMask(sourceFactionIndex);
    rulesSatisfied =
         GameFactionRelations_EvaluateTransitionRules(targetFactionIndex,sourceEligibleMask | targetEligibleMask);
    if ((!rulesSatisfied) &&
       (rulesSatisfied =
             GameFactionRelations_EvaluateTransitionRules(sourceFactionIndex,sourceEligibleMask | targetEligibleMask),
       !rulesSatisfied)) {
      return false;
    }
  }
  return true;
}


/* Address: 0x0053C090.
   Returns the bloc of sourceFactionIndex as a faction bit mask (bit n = faction n, factions 1..7): every active
   faction whose relation state towards it is 4 or higher (friendly), plus the faction itself when active.
*/
FactionActiveMask GameFactionRelations_BuildEligibleFactionMask(FactionRuntimeIndex sourceFactionIndex)

{
  uint32_t relationStateNibble;
  int factionIndex;
  uint32_t blocFactionMask;
  uint32_t currentFactionBit;

  blocFactionMask = 0;
  currentFactionBit = FACTION_MASK_BIT(7);
  factionIndex = 7;
  do {
    if (g_GameFactionRuntimeImage.tail.factionLifecycleStates[factionIndex] ==
        FACTION_RUNTIME_LIFECYCLE_ACTIVE) {
      if ((factionIndex == sourceFactionIndex) ||
          (relationStateNibble = GameFactionRuntime_GetPackedStateNibble(sourceFactionIndex,factionIndex),
           FACTION_RELATION_STATE_FRIENDLY - 1 < relationStateNibble)) {
        blocFactionMask = blocFactionMask | currentFactionBit;
      }
    }
    currentFactionBit = currentFactionBit >> 1;
    factionIndex--;
  } while (factionIndex != 0);
  return blocFactionMask;
}


/* Address: 0x0053C0F0.
   Predicts the level's end conditions for the case that only the factions in activeFactionMask were left
   (used to judge whether two blocs may draw closer): unless the mask equals the currently active factions, the
   64 scheduled conditions are re-evaluated with faction presence taken from the mask, and the first active end
   trigger that then fires for an active faction decides: its movie variant, flipped when that faction is
   neither focalFactionIndex nor in the mask. Returns true (CF) when that variant is 0 or nothing fires.
   Leaves the recomputed satisfied bits in the real condition records.
*/
bool GameFactionRelations_EvaluateTransitionRules
          (FactionRuntimeIndex focalFactionIndex,FactionActiveMask activeFactionMask)

{
  uint8_t tokenOrFactionIndex;
  InGameLevelConditionStorage *levelConditionStorage;
  uint32_t actualActiveMask;
  int remainingCount;
  uint32_t currentFactionBit;
  InGameScheduledConditionKind kindOrStackValue;
  uint8_t *expressionCursor;
  uint8_t movieVariant;
  InGameConditionSchedule *conditionCursor;
  InGameEndConditionTriggerRecord8ReferenceView *triggerCursor;

  levelConditionStorage = g_InGameLevelRuntimeGlobalBlock.conditionStorage;
  actualActiveMask = 0;
  currentFactionBit = FACTION_MASK_BIT(7);
  remainingCount = 7; /* doubles as the faction index 7..1 */
  do {
    if (g_GameFactionRuntimeImage.tail.factionLifecycleStates[remainingCount] ==
        FACTION_RUNTIME_LIFECYCLE_ACTIVE) {
      actualActiveMask = actualActiveMask | currentFactionBit;
    }
    currentFactionBit = currentFactionBit >> 1;
    remainingCount--;
  } while (remainingCount != 0);
  if (actualActiveMask != activeFactionMask) {
    remainingCount = INGAME_SCHEDULED_CONDITION_COUNT;
    conditionCursor = &(g_InGameLevelRuntimeGlobalBlock.conditionStorage)->schedule;
    do {
      /* bit 0 of kind is the satisfied flag: clear it, set it again when the condition would hold */
      kindOrStackValue = conditionCursor->conditions[0].statusAndKind.kind;
      conditionCursor->conditions[0].statusAndKind.kind =
           conditionCursor->conditions[0].statusAndKind.kind & ~INGAME_SCHEDULED_CONDITION_SATISFIED;
      switch(kindOrStackValue & INGAME_SCHEDULED_CONDITION_KIND_MASK) {
      case INGAME_SCHEDULED_CONDITION_FACTION_HAS_NO_ARMY:
        if ((activeFactionMask & 1 << ((uint8_t)conditionCursor->conditions[0].payload.operands[0] & 31)) == 0) {
          conditionCursor->conditions[0].statusAndKind.kind =
               conditionCursor->conditions[0].statusAndKind.kind | INGAME_SCHEDULED_CONDITION_SATISFIED;
        }
        break;
      case INGAME_SCHEDULED_CONDITION_FACTION_HAS_NO_COMMAND_GROUP_A_ARMY:
        if ((activeFactionMask & 1 << ((uint8_t)conditionCursor->conditions[0].payload.operands[0] & 31)) == 0) {
          conditionCursor->conditions[0].statusAndKind.kind =
               conditionCursor->conditions[0].statusAndKind.kind | INGAME_SCHEDULED_CONDITION_SATISFIED;
        }
        break;
      case INGAME_SCHEDULED_CONDITION_FACTION_HAS_NO_ARMY_OF_ASSET:
        if ((activeFactionMask & 1 << ((uint8_t)conditionCursor->conditions[0].payload.operands[0] & 31)) == 0) {
          conditionCursor->conditions[0].statusAndKind.kind =
               conditionCursor->conditions[0].statusAndKind.kind | INGAME_SCHEDULED_CONDITION_SATISFIED;
        }
        break;
      case INGAME_SCHEDULED_CONDITION_FACTION_INACTIVE_OR_RELATION_AT_LEAST_8:
        conditionCursor->conditions[0].statusAndKind.kind =
               conditionCursor->conditions[0].statusAndKind.kind | INGAME_SCHEDULED_CONDITION_SATISFIED;
        break;
      case INGAME_SCHEDULED_CONDITION_ARMY_OF_ASSET_COUNT_AT_LEAST:
        if ((activeFactionMask & 1 << ((uint8_t)conditionCursor->conditions[0].payload.operands[0] & 31)) != 0) {
          conditionCursor->conditions[0].statusAndKind.kind =
               conditionCursor->conditions[0].statusAndKind.kind | INGAME_SCHEDULED_CONDITION_SATISFIED;
        }
        break;
      case INGAME_SCHEDULED_CONDITION_FACTION_TERRAIN_OCCUPANCY_MASK_F9_PERCENT_AT_LEAST:
        conditionCursor->conditions[0].statusAndKind.kind =
               conditionCursor->conditions[0].statusAndKind.kind | INGAME_SCHEDULED_CONDITION_SATISFIED;
        break;
      case INGAME_SCHEDULED_CONDITION_COUNTDOWN_ELAPSED:
        if (conditionCursor->conditions[0].payload.operands[1] == 0) {
          conditionCursor->conditions[0].statusAndKind.kind =
               conditionCursor->conditions[0].statusAndKind.kind | INGAME_SCHEDULED_CONDITION_SATISFIED;
        }
        break;
      case INGAME_SCHEDULED_CONDITION_XENITE_STORAGE_LIMIT_AT_MOST_0FA0:
        conditionCursor->conditions[0].statusAndKind.kind =
               conditionCursor->conditions[0].statusAndKind.kind | INGAME_SCHEDULED_CONDITION_SATISFIED;
        break;
      case INGAME_SCHEDULED_CONDITION_NO_ARMY_OF_CLASS_OUTSIDE_COMMAND_GROUP_A:
        if ((activeFactionMask & 1 << ((uint8_t)conditionCursor->conditions[0].payload.operands[0] & 31)) == 0) {
          conditionCursor->conditions[0].statusAndKind.kind =
               conditionCursor->conditions[0].statusAndKind.kind | INGAME_SCHEDULED_CONDITION_SATISFIED;
        }
        break;
      case INGAME_SCHEDULED_CONDITION_BOOLEAN_POSTFIX_EXPRESSION:
        /* postfix bytes after the kind byte, a bit stack in kindOrStackValue: 0xFF OR, 0xFE AND, 0xFD NOT,
           0xFC end, anything else pushes the satisfied bit of that condition index */
        expressionCursor = &conditionCursor->conditions[0].statusAndKind.kindAndExpression[1];
        kindOrStackValue = INGAME_SCHEDULED_CONDITION_NONE_OR_UNUSED;
        while( true ) {
          while( true ) {
            while( true ) {
              while( true ) {
                tokenOrFactionIndex = *expressionCursor;
                expressionCursor = expressionCursor + 1;
                if (tokenOrFactionIndex != INGAME_CONDITION_TOKEN_OR) break;
                kindOrStackValue = kindOrStackValue >> 1 | kindOrStackValue & 1;
              }
              if (tokenOrFactionIndex != INGAME_CONDITION_TOKEN_AND) break;
              kindOrStackValue = kindOrStackValue >> 1 & (kindOrStackValue | ~1u);
            }
            if (tokenOrFactionIndex != INGAME_CONDITION_TOKEN_NOT) break;
            kindOrStackValue = kindOrStackValue ^ 1;
          }
          if (tokenOrFactionIndex == INGAME_CONDITION_TOKEN_END) break;
          kindOrStackValue =
               ((levelConditionStorage->schedule).conditions[tokenOrFactionIndex].statusAndKind.kind &
                INGAME_SCHEDULED_CONDITION_SATISFIED) + kindOrStackValue * 2;
        }
        conditionCursor->conditions[0].statusAndKind.kind =
             conditionCursor->conditions[0].statusAndKind.kind | kindOrStackValue & 1;
      }
      conditionCursor = (InGameConditionSchedule *)(conditionCursor->conditions + 1);
      remainingCount--;
    } while (remainingCount != 0);
    triggerCursor = (levelConditionStorage->schedule).triggers;
    remainingCount = INGAME_END_CONDITION_TRIGGER_COUNT;
    do {
      if ((triggerCursor->stateFlags == INGAME_END_CONDITION_TRIGGER_ACTIVE) &&
         (((levelConditionStorage->schedule).conditions[triggerCursor->conditionIndex].statusAndKind.kind &
           INGAME_SCHEDULED_CONDITION_SATISFIED) != 0)) {
        tokenOrFactionIndex = triggerCursor->factionRuntimeIndex;
        if (g_GameFactionRuntimeImage.tail.factionLifecycleStates[tokenOrFactionIndex] ==
            FACTION_RUNTIME_LIFECYCLE_ACTIVE) {
          movieVariant = triggerCursor->movieVariantSelector;
          if ((focalFactionIndex != (uint32_t)tokenOrFactionIndex) &&
              ((activeFactionMask & 1 << (tokenOrFactionIndex & 31)) == 0)) {
            movieVariant = movieVariant ^ 1;
          }
          if (movieVariant == 0) {
            return true;
          }
          return false;
        }
      }
      triggerCursor++;
      remainingCount--;
    } while (remainingCount != 0);
  }
  return true;
}


/* Address: 0x0053C490.
   Returns true (CF set) when the pair's relation state is none of 3, 6 and 10, the top state of each tier
   below the merge; only from those states does the random drift reset the relation
   (GameFactionRuntime_ResetPairwiseRelationState).
*/
bool GameFactionRelations_IsNotResetEligibleState
          (FactionRuntimeIndex sourceFactionIndex,FactionRuntimeIndex targetFactionIndex)

{
  uint32_t relationStateNibble;

  relationStateNibble =
       GameFactionRuntime_GetPackedStateNibble(sourceFactionIndex,targetFactionIndex);
  if (((relationStateNibble != 3) && (relationStateNibble != 6)) && (relationStateNibble != 10)) {
    return true;
  }
  return false;
}


/* Address: 0x0053C4D0.
   Random drift for a pair outside the states 3, 6 and 10: advances the relation with a chance of 1 in 256
   while the pair pressure is 0, otherwise 1 in 1024 and only while the pressure (below 32) is smaller than
   the random value's top four bits.
*/
void GameFactionRelations_MaybeAdvancePairStateRare
          (FactionRuntimeIndex sourceFactionIndex,FactionRuntimeIndex targetFactionIndex)

{
  uint32_t pairPressure;
  uint32_t randomValue;
  uint32_t maskedRandom;

  randomValue = g_RandomGeneratorState.next();
  pairPressure = g_GameDataAuxState.pairPressureMatrix8x8[targetFactionIndex * 8 + sourceFactionIndex];
  if (pairPressure == 0) {
    maskedRandom = randomValue & FACTION_RELATION_DRIFT_RARE_IDLE_MASK;
  }
  else {
    if (31 < pairPressure) {
      return;
    }
    maskedRandom = randomValue & FACTION_RELATION_DRIFT_RARE_PRESSURE_MASK;
    if (randomValue >> 28 <= pairPressure) {
      return;
    }
  }
  if (maskedRandom == FACTION_RELATION_DRIFT_MATCH) {
    GameFactionRuntime_AdvancePairwiseRelationState
              (UINT32_MAX,0,sourceFactionIndex,targetFactionIndex);
  }
  return;
}


/* Address: 0x0053C540.
   Random drift for a pair in state 3, 6 or 10: advances the relation to the next tier with a chance of 1 in
   128 while the pair pressure is 0, otherwise 1 in 512 and only while the pressure (below 32) is smaller than
   the random value's top four bits.
*/
void GameFactionRelations_MaybeAdvancePairStateCommon
          (FactionRuntimeIndex sourceFactionIndex,FactionRuntimeIndex targetFactionIndex)

{
  uint32_t pairPressure;
  uint32_t randomValue;
  uint32_t maskedRandom;

  randomValue = g_RandomGeneratorState.next();
  pairPressure = g_GameDataAuxState.pairPressureMatrix8x8[targetFactionIndex * 8 + sourceFactionIndex];
  if (pairPressure == 0) {
    maskedRandom = randomValue & FACTION_RELATION_DRIFT_COMMON_IDLE_MASK;
  }
  else {
    if (31 < pairPressure) {
      return;
    }
    maskedRandom = randomValue & FACTION_RELATION_DRIFT_COMMON_PRESSURE_MASK;
    if (randomValue >> 28 <= pairPressure) {
      return;
    }
  }
  if (maskedRandom == FACTION_RELATION_DRIFT_MATCH) {
    GameFactionRuntime_AdvancePairwiseRelationState
              (UINT32_MAX,0,sourceFactionIndex,targetFactionIndex);
  }
  return;
}


/* Address: 0x0053C5B0.
   Random drift for a pair whose change is blocked: resets the relation state with a chance of 1 in 4.
*/
void GameFactionRelations_MaybeResetPairState
          (FactionRuntimeIndex sourceFactionIndex,FactionRuntimeIndex targetFactionIndex)

{
  uint32_t randomValue;

  randomValue = g_RandomGeneratorState.next();
  if ((randomValue & FACTION_RELATION_RESET_RANDOM_MASK) == FACTION_RELATION_RESET_RANDOM_MATCH) {
    GameFactionRuntime_ResetPairwiseRelationState
              (UINT32_MAX,0,sourceFactionIndex,targetFactionIndex);
  }
  return;
}


/* Address: 0x00560EB0.
   Appends the field cell (worldXQ12, worldYQ12) to the player's marked-cell list unless it is already listed or
   the list is full (PLAYER_PAIR_LIST_CAPACITY). For the local player the in-game root's count at +0xBA4 (its
   records pointer at +0xBA0 aliases this list) is raised too. Called by PlayerPairList_InsertRange.
*/
void PlayerPairList_InsertUnique
          (PlayerRuntimeId playerRuntimeId,uint32_t reservedZero,SelectionPlayerPairValue worldYQ12,
          SelectionPlayerPairValue worldXQ12)

{
  SelectionPlayerPairRecord *pairRecordCursor;
  uint32_t recordsRemaining;
  SelectionPlayerRuntimeBlock *playerRuntimeBlock;
  uint32_t appendRecordIndex;
  InGameRuntimeRoot *inGameRuntimeRoot;
  
  inGameRuntimeRoot = g_InGameRuntimeRoot;
  playerRuntimeBlock = g_SelectionPlayerRuntimeBlockPointers[playerRuntimeId];
  recordsRemaining = playerRuntimeBlock->markedCellCount;
  pairRecordCursor = playerRuntimeBlock->markedCells;
  if (recordsRemaining < PLAYER_PAIR_LIST_CAPACITY) {
    for (; recordsRemaining != 0; recordsRemaining--) {
      if ((worldXQ12 == pairRecordCursor->pairKey) && (worldYQ12 == pairRecordCursor->pairValue)) {
        return;
      }
      pairRecordCursor++;
    }
    appendRecordIndex = playerRuntimeBlock->markedCellCount;
    playerRuntimeBlock->markedCellCount++;
    playerRuntimeBlock->markedCells[appendRecordIndex].pairKey = worldXQ12;
    playerRuntimeBlock->markedCells[appendRecordIndex].pairValue = worldYQ12;
    if (playerRuntimeId == g_LocalPlayerRuntimeId) {
      inGameRuntimeRoot->localPlayerMarkedCellCount++;
    }
  }
  return;
}


/* Address: 0x00560F50.
   Removes the field cell (worldXQ12, worldYQ12) from the player's marked-cell list, moving the later records
   down so the order is kept, and lowers the in-game root's count at +0xBA4 for the local player. A list at
   PLAYER_PAIR_LIST_CAPACITY or above is left untouched. Called by PlayerPairList_RemoveRange.
*/
void PlayerPairList_RemoveFirstMatch
          (PlayerRuntimeId playerRuntimeId,uint32_t reservedZero,SelectionPlayerPairValue worldYQ12,
          SelectionPlayerPairValue worldXQ12)

{
  int trailingDwordsToMove;
  uint32_t *copySourceDword;
  uint32_t *copyTargetDword;
  uint32_t recordsRemaining;
  SelectionPlayerPairRecord *pairRecordCursor;
  SelectionPlayerRuntimeBlock *playerRuntimeBlock;
  InGameRuntimeRoot *inGameRuntimeRoot;
  
  inGameRuntimeRoot = g_InGameRuntimeRoot;
  playerRuntimeBlock = g_SelectionPlayerRuntimeBlockPointers[playerRuntimeId];
  recordsRemaining = playerRuntimeBlock->markedCellCount;
  pairRecordCursor = playerRuntimeBlock->markedCells;
  if (recordsRemaining < PLAYER_PAIR_LIST_CAPACITY) {
    for (; recordsRemaining != 0; recordsRemaining--) {
      if ((worldXQ12 == pairRecordCursor->pairKey) && (worldYQ12 == pairRecordCursor->pairValue)) {
        copySourceDword = (uint32_t *)(pairRecordCursor + 1);
        copyTargetDword = (uint32_t *)pairRecordCursor;
        /* the records behind the match move down one dword at a time (REP MOVSD in the original) */
        trailingDwordsToMove = recordsRemaining * 2 - 2;
        playerRuntimeBlock->markedCellCount--;
        for (; trailingDwordsToMove != 0; trailingDwordsToMove--) {
          *copyTargetDword = *copySourceDword;
          copySourceDword++;
          copyTargetDword++;
        }
        if (playerRuntimeId == g_LocalPlayerRuntimeId) {
          inGameRuntimeRoot->localPlayerMarkedCellCount--;
        }
        return;
      }
      pairRecordCursor++;
    }
  }
  return;
}


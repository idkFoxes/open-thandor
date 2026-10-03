/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/src/gameplay/faction/relations.cpp
 * Reverse engineering by idkFoxes 2026
 */

#include <thandor/gameplay/faction/relations.h>
#include <thandor/thandor.h>

/* Implementation ownership: gameplay/faction/relations. */

/* Random drift of the diplomatic relations between sourceFactionIndex and every other active faction 7..1
   (faction 0 is never visited). When the pair may change state and sits in state 3, 6 or 10, the relation may
   be reset (GameFactionRelations_IsNotResetEligibleState returns false for exactly those states); otherwise it may
   advance: rarely from the other states, more often from 3, 6 and 10.
*/
void GameFactionRelations_UpdateAllPairsForFaction
          (FactionRuntimeIndex sourceFactionIndex,WorldRuntimeContext *worldRuntime)

{
  int opposingFactionIndex;
  Bool8 pairTestResult;

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


/* Adds one row of field cells (world X from firstWorldXQ12 to lastWorldXQ12 inclusive, one cell apart, at
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


/* Counterpart of PlayerPairList_InsertRange: removes one row of field cells (world X from firstWorldXQ12 to
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


/* Decides which random drift GameFactionRelations_UpdateAllPairsForFaction applies to a pair. True
   selects the reset path: the pending states 2, 5 and 9, relations frozen by relationUiFlags, or a state
   below 4 for which GameFactionRelations_EvaluateTransitionRules holds for either faction. False lets the pair
   advance.
*/
Bool8 GameFactionRelations_TestPairTransitionAllowed
          (FactionRuntimeIndex sourceFactionIndex,FactionRuntimeIndex targetFactionIndex)

{
  FactionRelationState relationState;
  FactionActiveMask targetEligibleMask;
  FactionActiveMask sourceEligibleMask;
  FactionActiveMask combinedMask;
  uint32_t relationUiFlags;

  relationState = GameFactionRuntime_GetPackedStateNibble(sourceFactionIndex,targetFactionIndex);
  if (relationState == 2 || relationState == 5 || relationState == 9) {
    return true; /* pending state */
  }
  /* relationUiFlags: bit 4 freezes every relation, bit 2 states 4 and up, bit 1 states 8 and up */
  relationUiFlags = g_GameFactionRuntimeImage.tail.relationUiFlags;
  if ((relationUiFlags & FACTION_RELATION_FREEZE_ALL) != 0) {
    return true;
  }
  if (relationState >= FACTION_RELATION_STATE_FRIENDLY &&
      (relationUiFlags & FACTION_RELATION_FREEZE_FRIENDLY) != 0) {
    return true;
  }
  if (relationState >= FACTION_RELATION_STATE_ALLIED &&
      (relationUiFlags & FACTION_RELATION_FREEZE_ALLIED) != 0) {
    return true;
  }
  if (relationState >= FACTION_RELATION_STATE_FRIENDLY) {
    return false;
  }
  targetEligibleMask = GameFactionRelations_BuildEligibleFactionMask(targetFactionIndex);
  sourceEligibleMask = GameFactionRelations_BuildEligibleFactionMask(sourceFactionIndex);
  combinedMask = sourceEligibleMask | targetEligibleMask;
  if (GameFactionRelations_EvaluateTransitionRules(targetFactionIndex,combinedMask)) {
    return true;
  }
  return GameFactionRelations_EvaluateTransitionRules(sourceFactionIndex,combinedMask);
}


/* Returns the bloc of sourceFactionIndex as a faction bit mask (bit n = faction n, factions 1..7): every active
   faction whose relation state towards it is 4 or higher (friendly), plus the faction itself when active.
*/
FactionActiveMask GameFactionRelations_BuildEligibleFactionMask(FactionRuntimeIndex sourceFactionIndex)

{
  uint32_t relationStateNibble;
  int factionIndex;
  uint32_t blocFactionMask;

  blocFactionMask = 0;
  for (factionIndex = 7; factionIndex != 0; factionIndex--) {
    if (g_GameFactionRuntimeImage.tail.factionLifecycleStates[factionIndex] !=
        FACTION_RUNTIME_LIFECYCLE_ACTIVE) {
      continue;
    }
    if (factionIndex == sourceFactionIndex) {
      blocFactionMask = blocFactionMask | FACTION_MASK_BIT(factionIndex);
      continue;
    }
    relationStateNibble = GameFactionRuntime_GetPackedStateNibble(sourceFactionIndex,factionIndex);
    if (relationStateNibble >= FACTION_RELATION_STATE_FRIENDLY) {
      blocFactionMask = blocFactionMask | FACTION_MASK_BIT(factionIndex);
    }
  }
  return blocFactionMask;
}


/* True when the faction named by a condition's operand 0 is in factionMask. */
static Bool8 GameFactionRelations_IsOperandFactionInMask(FactionActiveMask factionMask,uint32_t factionOperand)
{
  return (factionMask & 1 << ((uint8_t)factionOperand & 31)) != 0;
}


/* Evaluates a BOOLEAN_POSTFIX_EXPRESSION condition on a bit stack: 0xFF OR, 0xFE AND, 0xFD NOT, 0xFC end,
   anything else pushes the satisfied bit of the condition with that index. Returns the bit stack; bit 0 is the
   result. The stack keeps the signed int width of the original (a condition-kind enum value). */
static int GameFactionRelations_EvaluatePostfixExpression
          (InGameLevelConditionStorage *levelConditionStorage,const uint8_t *expression)
{
  int bitStack;
  uint8_t token;

  bitStack = INGAME_SCHEDULED_CONDITION_NONE_OR_UNUSED;
  for (token = *expression++; token != INGAME_CONDITION_TOKEN_END; token = *expression++) {
    if (token == INGAME_CONDITION_TOKEN_OR) {
      bitStack = bitStack >> 1 | bitStack & 1;
    }
    else if (token == INGAME_CONDITION_TOKEN_AND) {
      bitStack = bitStack >> 1 & (bitStack | ~1u);
    }
    else if (token == INGAME_CONDITION_TOKEN_NOT) {
      bitStack = bitStack ^ 1;
    }
    else {
      bitStack = ((levelConditionStorage->schedule).conditions[token].statusAndKind.raw &
                  INGAME_SCHEDULED_CONDITION_SATISFIED) + bitStack * 2;
    }
  }
  return bitStack;
}


/* Whether a scheduled condition of the given kind would hold if only the factions in activeFactionMask were
   left (GameFactionRelations_EvaluateTransitionRules). Unknown kinds never hold. */
static Bool8 GameFactionRelations_PredictConditionHolds
          (InGameLevelConditionStorage *levelConditionStorage,InGameScheduledConditionRecord10 *condition,
           uint32_t kind,FactionActiveMask activeFactionMask)
{
  switch(kind) {
  case INGAME_SCHEDULED_CONDITION_FACTION_HAS_NO_ARMY:
  case INGAME_SCHEDULED_CONDITION_FACTION_HAS_NO_COMMAND_GROUP_A_ARMY:
  case INGAME_SCHEDULED_CONDITION_FACTION_HAS_NO_ARMY_OF_ASSET:
  case INGAME_SCHEDULED_CONDITION_NO_ARMY_OF_CLASS_OUTSIDE_COMMAND_GROUP_A:
    return !GameFactionRelations_IsOperandFactionInMask(activeFactionMask,condition->payload.operands[0]);
  case INGAME_SCHEDULED_CONDITION_ARMY_OF_ASSET_COUNT_AT_LEAST:
    return GameFactionRelations_IsOperandFactionInMask(activeFactionMask,condition->payload.operands[0]);
  case INGAME_SCHEDULED_CONDITION_FACTION_INACTIVE_OR_RELATION_AT_LEAST_8:
  case INGAME_SCHEDULED_CONDITION_FACTION_TERRAIN_OCCUPANCY_MASK_F9_PERCENT_AT_LEAST:
  case INGAME_SCHEDULED_CONDITION_XENITE_STORAGE_LIMIT_AT_MOST_0FA0:
    return true;
  case INGAME_SCHEDULED_CONDITION_COUNTDOWN_ELAPSED:
    return condition->payload.operands[1] == 0;
  case INGAME_SCHEDULED_CONDITION_BOOLEAN_POSTFIX_EXPRESSION:
    return (GameFactionRelations_EvaluatePostfixExpression
              (levelConditionStorage,&condition->statusAndKind.kindAndExpression[1]) & 1) != 0;
  default:
    return false;
  }
}


/* Predicts the level's end conditions for the case that only the factions in activeFactionMask were left
   (used to judge whether two blocs may draw closer): unless the mask equals the currently active factions, the
   64 scheduled conditions are re-evaluated with faction presence taken from the mask, and the first active end
   trigger that then fires for an active faction decides: its movie variant, flipped when that faction is
   neither focalFactionIndex nor in the mask. Returns true when that variant is 0 or nothing fires.
   Leaves the recomputed satisfied bits in the real condition records.
*/
Bool8 GameFactionRelations_EvaluateTransitionRules
          (FactionRuntimeIndex focalFactionIndex,FactionActiveMask activeFactionMask)

{
  InGameLevelConditionStorage *levelConditionStorage;
  uint32_t actualActiveMask;
  int factionIndex;
  int conditionIndex;
  int triggerIndex;
  uint32_t kindAndStatus;
  uint8_t triggerFactionIndex;
  uint8_t movieVariant;
  InGameScheduledConditionRecord10 *condition;
  InGameEndConditionTriggerRecord8ReferenceView *trigger;

  levelConditionStorage = g_InGameLevelRuntimeGlobalBlock.conditionStorage;
  actualActiveMask = 0;
  for (factionIndex = 7; factionIndex != 0; factionIndex--) {
    if (g_GameFactionRuntimeImage.tail.factionLifecycleStates[factionIndex] ==
        FACTION_RUNTIME_LIFECYCLE_ACTIVE) {
      actualActiveMask = actualActiveMask | FACTION_MASK_BIT(factionIndex);
    }
  }
  if (actualActiveMask == activeFactionMask) {
    return true;
  }
  condition = levelConditionStorage->schedule.conditions;
  for (conditionIndex = 0; conditionIndex < INGAME_SCHEDULED_CONDITION_COUNT; conditionIndex++, condition++) {
    /* bit 0 of kind is the satisfied flag: clear it, set it again when the condition would hold */
    kindAndStatus = condition->statusAndKind.raw;
    condition->statusAndKind.raw = kindAndStatus & ~(uint32_t)INGAME_SCHEDULED_CONDITION_SATISFIED;
    if (GameFactionRelations_PredictConditionHolds(levelConditionStorage,condition,
                                                   kindAndStatus & INGAME_SCHEDULED_CONDITION_KIND_MASK,
                                                   activeFactionMask)) {
      condition->statusAndKind.raw = condition->statusAndKind.raw | INGAME_SCHEDULED_CONDITION_SATISFIED;
    }
  }
  trigger = levelConditionStorage->schedule.triggers;
  for (triggerIndex = 0; triggerIndex < INGAME_END_CONDITION_TRIGGER_COUNT; triggerIndex++, trigger++) {
    if (trigger->stateFlags != INGAME_END_CONDITION_TRIGGER_ACTIVE ||
        (levelConditionStorage->schedule.conditions[trigger->conditionIndex].statusAndKind.raw &
         INGAME_SCHEDULED_CONDITION_SATISFIED) == 0) {
      continue;
    }
    triggerFactionIndex = trigger->factionRuntimeIndex;
    if (g_GameFactionRuntimeImage.tail.factionLifecycleStates[triggerFactionIndex] !=
        FACTION_RUNTIME_LIFECYCLE_ACTIVE) {
      continue;
    }
    movieVariant = trigger->movieVariantSelector;
    if ((focalFactionIndex != (uint32_t)triggerFactionIndex) &&
        ((activeFactionMask & 1 << (triggerFactionIndex & 31)) == 0)) {
      movieVariant = movieVariant ^ 1;
    }
    return movieVariant == 0;
  }
  return true;
}


/* Returns true when the pair's relation state is none of 3, 6 and 10, the top state of each tier
   below the merge; only from those states does the random drift reset the relation
   (GameFactionRuntime_ResetPairwiseRelationState).
*/
Bool8 GameFactionRelations_IsNotResetEligibleState
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


/* Random drift for a pair outside the states 3, 6 and 10: advances the relation with a chance of 1 in 256
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


/* Random drift for a pair in state 3, 6 or 10: advances the relation to the next tier with a chance of 1 in
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


/* Random drift for a pair whose change is blocked: resets the relation state with a chance of 1 in 4.
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


/* Appends the field cell (worldXQ12, worldYQ12) to the player's marked-cell list unless it is already listed or
   the list is full (PLAYER_PAIR_LIST_CAPACITY). For the local player the in-game root's
   localPlayerMarkedCellCount (its localPlayerMarkedCells pointer aliases this list) is raised too. Called by
   PlayerPairList_InsertRange.
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


/* Removes the field cell (worldXQ12, worldYQ12) from the player's marked-cell list, moving the later records
   down so the order is kept, and lowers the in-game root's localPlayerMarkedCellCount for the local player. A list at
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
        /* the records behind the match move down one dword at a time, as in the original */
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


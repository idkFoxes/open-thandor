/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/src/gameplay/faction/relations.cpp
 * Reverse engineering by idkFoxes 2026
 */

#include <thandor/gameplay/faction/relations.h>
#include <thandor/thandor.h>

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
}


/* Adds one row of field cells (world X from firstWorldXQ12 to lastWorldXQ12 inclusive, one cell apart, at
   worldYQ12) to the player's marked-cell list. Called per row by the in-game command UI when an area is dragged
   out in a local session (ui/ingame/editor_tools.cpp; networked sessions queue command 0x1D00 instead).
*/
void PlayerPairList_InsertRange(PlayerRuntimeId playerRuntimeId,SelectionPlayerPairValue lastWorldXQ12,
          SelectionPlayerPairValue worldYQ12,SelectionPlayerPairValue firstWorldXQ12)

{
  for (; (int)firstWorldXQ12 <= (int)lastWorldXQ12; firstWorldXQ12 = firstWorldXQ12 + FIELD_GRID_CELL_Q12) {
    PlayerPairList_InsertUnique(playerRuntimeId,0,worldYQ12,firstWorldXQ12);
  }
}


/* Counterpart of PlayerPairList_InsertRange: removes one row of field cells (world X from firstWorldXQ12 to
   lastWorldXQ12 inclusive, at worldYQ12) from the player's marked-cell list. Called per row by the in-game
   command UI in a local session (ui/ingame/editor_tools.cpp; networked sessions queue command 0x1D40 instead).
*/
void PlayerPairList_RemoveRange(PlayerRuntimeId playerRuntimeId,SelectionPlayerPairValue lastWorldXQ12,
          SelectionPlayerPairValue worldYQ12,SelectionPlayerPairValue firstWorldXQ12)

{
  for (; (int)firstWorldXQ12 <= (int)lastWorldXQ12; firstWorldXQ12 = firstWorldXQ12 + FIELD_GRID_CELL_Q12) {
    PlayerPairList_RemoveFirstMatch(playerRuntimeId,0,worldYQ12,firstWorldXQ12);
  }
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
    /* the stack keeps the signed int width of the original (a condition-kind enum value) */
    return (InGameScheduledCondition_EvaluatePostfixExpression<int>
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
}


/* Appends the field cell (worldXQ12, worldYQ12) to the player's marked-cell list unless it is already listed or
   the list is full (PLAYER_PAIR_LIST_CAPACITY). For the local player the in-game root's
   localPlayerMarkedCellCount (its localPlayerMarkedCells pointer aliases this list) is raised too. Called by
   PlayerPairList_InsertRange.
   Original quirk: the cap check is `count < PLAYER_PAIR_LIST_CAPACITY` before the duplicate scan, so a list
   that reached 4096 records accepts no more cells, and PlayerPairList_RemoveFirstMatch skips it as well: a
   full list stays blocked for the rest of the session.
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
        copySourceDword = reinterpret_cast<uint32_t *>(pairRecordCursor + 1); /* dword copy, see below */
        copyTargetDword = reinterpret_cast<uint32_t *>(pairRecordCursor);
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
}

/* Moves the diplomatic relation of a faction pair one step closer, chosen by the state of targetFactionIndex
   towards sourceFactionIndex: 0..2 -> 3/2, 3 -> 4, 4..5 -> 6/5, 6 -> 8, 8..9 -> 10/9, 10 -> 11 (merge),
   first value for the source's state towards the target. State 2 does nothing while the last change is at
   most 600 ticks old; the same check in states 4 and 8 never holds (it only accepts 2, 5 and 9). The first
   two arguments are not used.
*/
void GameFactionRuntime_AdvancePairwiseRelationState(uint32_t unusedRelationArgument0,uint32_t unusedRelationArgument1,
          FactionRuntimeIndex sourceFactionIndex,FactionRuntimeIndex targetFactionIndex)

{
  Bool8 isRecentTimedState;
  
  switch(g_GameFactionRuntimeImage.records[targetFactionIndex].packedRelationStates >>
         ((uint8_t)(sourceFactionIndex << 2) & 31) & 0xf) {
  case 2:
    isRecentTimedState = GameFactionRuntime_IsRecentTimedRelationState(sourceFactionIndex,targetFactionIndex);
    if (isRecentTimedState) {
      return;
    }
  case 0:
  case 1:
    GameFactionRuntime_ApplyPairwiseRelationTransition
              (1,0,3,2,sourceFactionIndex,targetFactionIndex);
    break;
  case 3:
    GameFactionRuntime_ApplyPairwiseRelationTransition
              (2,2,4,4,sourceFactionIndex,targetFactionIndex);
    break;
  case 4:
    /* Original quirk: IsRecentTimedRelationState only accepts states 2, 5 and 9, so this check never holds
       and state 4 always falls through to the state-5 transition. */
    isRecentTimedState = GameFactionRuntime_IsRecentTimedRelationState(sourceFactionIndex,targetFactionIndex);
    if (isRecentTimedState) {
      return;
    }
  case 5:
    GameFactionRuntime_ApplyPairwiseRelationTransition
              (4,3,6,5,sourceFactionIndex,targetFactionIndex);
    break;
  case 6:
    GameFactionRuntime_ApplyPairwiseRelationTransition
              (5,5,8,8,sourceFactionIndex,targetFactionIndex);
    break;
  case 8:
    /* Original quirk: never holds either (see case 4); state 8 always falls through to the state-9
       transition. */
    isRecentTimedState = GameFactionRuntime_IsRecentTimedRelationState(sourceFactionIndex,targetFactionIndex);
    if (isRecentTimedState) {
      return;
    }
  case 9:
    GameFactionRuntime_ApplyPairwiseRelationTransition
              (7,6,10,9,sourceFactionIndex,targetFactionIndex);
    break;
  case 10:
    GameFactionRuntime_ApplyPairwiseRelationTransition
              (8,8,FACTION_RELATION_MERGE,FACTION_RELATION_MERGE,sourceFactionIndex,targetFactionIndex);
  }
}

/* Moves the diplomatic relation of a faction pair back, chosen by the state of targetFactionIndex towards
   sourceFactionIndex: 1..3 -> 0, 4 and 6 -> 0, 5 -> 4, 8 and 10 -> 4, 9 -> 8 (both directions get the same
   state), with the matching notification text. Other states stay. The first two arguments are not used.
*/
void GameFactionRuntime_ResetPairwiseRelationState(uint32_t unusedRelationArgument0,uint32_t unusedRelationArgument1,
          FactionRuntimeIndex sourceFactionIndex,FactionRuntimeIndex targetFactionIndex)

{
  switch(g_GameFactionRuntimeImage.records[targetFactionIndex].packedRelationStates >>
         ((uint8_t)(sourceFactionIndex << 2) & 31) & 0xf) {
  case 1:
  case 2:
  case 3:
    GameFactionRuntime_ApplyPairwiseRelationTransition
              (19,19,0,0,sourceFactionIndex,targetFactionIndex);
    break;
  case 4:
  case 6:
    GameFactionRuntime_ApplyPairwiseRelationTransition
              (9,9,0,0,sourceFactionIndex,targetFactionIndex);
    break;
  case 5:
    GameFactionRuntime_ApplyPairwiseRelationTransition
              (19,19,4,4,sourceFactionIndex,targetFactionIndex);
    break;
  case 8:
  case 10:
    GameFactionRuntime_ApplyPairwiseRelationTransition
              (10,10,4,4,sourceFactionIndex,targetFactionIndex);
    break;
  case 9:
    GameFactionRuntime_ApplyPairwiseRelationTransition
              (19,19,8,8,sourceFactionIndex,targetFactionIndex);
  }
}

#define FACTION_TECHNOLOGY_MASK_BITS 256u

/* Lowest technology whose bit is set in haveMasks and clear in lackMasks (both 256-bit technology masks), or
   FACTION_TECHNOLOGY_MASK_BITS when there is none. */
static TechnologyId GameFactionRuntime_FindFirstTechnologyOnlyIn(const uint32_t *haveMasks,
          const uint32_t *lackMasks)
{
  TechnologyId technologyIndex;
  uint32_t bitMask;

  for (technologyIndex = 0; technologyIndex < FACTION_TECHNOLOGY_MASK_BITS; technologyIndex++) {
    bitMask = 1u << (technologyIndex & 31);
    if (((haveMasks[technologyIndex >> 5] & bitMask) != 0) && ((lackMasks[technologyIndex >> 5] & bitMask) == 0)) {
      return technologyIndex;
    }
  }
  return FACTION_TECHNOLOGY_MASK_BITS;
}

/* Technology exchange between related factions: for every unordered pair of factions 1..7 whose relation
   state is 8, 9 or 10, the lowest technology only the source faction has and the lowest technology only the
   other faction has are swapped (each side unlocks the other's). A pair where either side has nothing the
   other lacks exchanges nothing. Afterwards the other-player command entries are rebuilt.
*/
void GameFactionRuntime_SynchronizeTechnologiesForRelationStates8To10()

{
  uint32_t sourceFactionIndex;
  uint32_t otherFactionIndex;
  uint32_t relationState;
  TechnologyId sourceTechnologyIndex;
  TechnologyId otherTechnologyIndex;
  GameFactionRuntimeRecord *sourceRecord;
  GameFactionRuntimeRecord *otherRecord;

  for (sourceFactionIndex = 1; sourceFactionIndex < 7; sourceFactionIndex++) {
    sourceRecord = &g_GameFactionRuntimeImage.records[sourceFactionIndex];
    for (otherFactionIndex = sourceFactionIndex + 1; otherFactionIndex < 8; otherFactionIndex++) {
      otherRecord = &g_GameFactionRuntimeImage.records[otherFactionIndex];
      /* the other faction's relation-state nibble towards the source faction */
      relationState = otherRecord->packedRelationStates >> ((char)sourceFactionIndex * 4 & 31U) & 0xf;
      if ((relationState < 8) || (10 < relationState)) {
        continue;
      }
      sourceTechnologyIndex = GameFactionRuntime_FindFirstTechnologyOnlyIn(sourceRecord->technologyMasks256Bits,
                                                                          otherRecord->technologyMasks256Bits);
      if (sourceTechnologyIndex == FACTION_TECHNOLOGY_MASK_BITS) {
        continue;
      }
      otherTechnologyIndex = GameFactionRuntime_FindFirstTechnologyOnlyIn(otherRecord->technologyMasks256Bits,
                                                                         sourceRecord->technologyMasks256Bits);
      if (otherTechnologyIndex == FACTION_TECHNOLOGY_MASK_BITS) {
        continue;
      }
      /* swap the pair */
      Technology_UnlockForFaction(0,0,otherTechnologyIndex,sourceFactionIndex);
      Technology_UnlockForFaction(0,0,sourceTechnologyIndex,otherFactionIndex);
    }
  }
  InGameOtherPlayerCommand_RebuildTargetEntries(&g_InGameRuntimeRoot->rootUi.base);
}

/* Returns true when the relation of factionIndex towards otherFactionIndex is in one of the pending states
   2, 5 or 9 and the pair's last relation change is at most 600 ticks old, so the relation does not advance
   again too soon.
*/
Bool8 GameFactionRuntime_IsRecentTimedRelationState
          (FactionRuntimeIndex otherFactionIndex,FactionRuntimeIndex factionIndex)

{
  uint32_t relationStateNibble;

  relationStateNibble =
       g_GameFactionRuntimeImage.records[factionIndex].packedRelationStates >>
       ((char)otherFactionIndex * 4 & 31U) & 0xf;
  /* relationStateTicks[other faction]: tick of the pair's last relation change */
  if ((((relationStateNibble == 2) || (relationStateNibble == 5)) || (relationStateNibble == 9)) &&
     ((int)(g_GameFactionRuntimeImage.tail.simulationTick -
           (int)g_GameFactionRuntimeImage.records[factionIndex].relationStateTicks[otherFactionIndex]) <
      FACTION_RELATION_CHANGE_COOLDOWN_TICKS + 1)) {
    return true;
  }
  return false;
}

/* Appends an absorbed faction's army-asset list to the survivor's list of the same kind, stopping when the
   survivor's 64 entries are full. */
static void GameFactionRuntime_AppendArmyAssetList(FactionArmyAssetCount *survivorCount,uint32_t *survivorList,
          FactionArmyAssetCount absorbedCount,const uint32_t *absorbedList)
{
  FactionArmyAssetCount assetsRemaining;
  uint32_t armyAssetReferenceDword;
  uint32_t slotIndex;
  int sourceIndex;

  assetsRemaining = absorbedCount;
  sourceIndex = 0;
  for (slotIndex = *survivorCount;
      (assetsRemaining != 0) && (slotIndex < FACTION_ARMY_ASSET_LIST_CAPACITY); slotIndex++) {
    armyAssetReferenceDword = absorbedList[sourceIndex];
    *survivorCount = *survivorCount + 1;
    survivorList[slotIndex] = armyAssetReferenceDword;
    sourceIndex++;
    assetsRemaining--;
  }
}

/* Merge (relation state 11) of GameFactionRuntime_ApplyPairwiseRelationTransition: survivingFactionIndex takes
   over absorbedFactionIndex's models (re-owned and repainted), players, per-faction cell bytes, resources,
   technology, statistics and army-asset lists; the absorbed faction becomes inactive and the in-game catalogs
   are rebuilt. */
static void GameFactionRuntime_MergeAbsorbedFaction(FactionRuntimeIndex survivingFactionIndex,
          FactionRuntimeIndex absorbedFactionIndex)
{
  TritiumAmountQ4 *tritiumField;
  XeniteAmountQ4 *xeniteField;
  EnergyAmountQ4 *energyField;
  FactionRelationCounter *relationCounter;
  TritiumAmountQ4 tritiumAmount;
  XeniteAmountQ4 xeniteLimit;
  TritiumAmountQ4 tritiumLimit;
  EnergyAmountQ4 energyCapacity;
  TritiumAmountQ4 tritiumExtracted;
  FactionRelationCounter counterA;
  FactionRelationCounter counterB;
  FactionRelationCounter counterD;
  FactionRelationCounter counterE;
  FactionRelationCounter counterF;
  int maskWordIndex;
  int playerRuntimeId;
  int cellsRemaining;
  FrontendPlayerRuntimeRecord *playerBlockCursor;
  FrontendPlayerRuntimeBlockCount playerBlocksRemaining;
  FieldGridCell *gridCell;
  GraphicsTextureSet *survivingFactionTextureSet;
  GraphicsPaletteAsset *survivingFactionPaletteAsset;
  FieldGridAsset *terrainGrid;
  InGameRuntimeRoot *runtimeRoot;
  WorldOwnerListNode *ownerNode;
  ArmyRuntimeSlot *armyRuntime;
  GameFactionRuntimeRecord *survivor;
  GameFactionRuntimeRecord *absorbed;

  runtimeRoot = g_InGameRuntimeRoot;
  if (absorbedFactionIndex == g_InGameRuntimeRoot->worldRuntime.activeFactionRuntimeIndex) {
    g_InGameRuntimeRoot->worldRuntime.activeFactionRuntimeIndex = survivingFactionIndex;
  }
  survivingFactionTextureSet = g_ArmyGraphicsBindings[survivingFactionIndex].textureSet;
  survivingFactionPaletteAsset = g_ArmyGraphicsBindings[survivingFactionIndex].paletteAsset;
  for (ownerNode = (runtimeRoot->worldRuntime).ownerListHead; ownerNode != nullptr; ownerNode = ownerNode->nextNode) {
    /* re-own the absorbed faction's models (their army's factionIndex) and repaint them in the survivor's colours */
    if (ownerNode->ownerClassId == WORLD_OWNER_RUNTIME_MODEL) {
      armyRuntime = WorldOwnerNode_ModelRuntime(ownerNode)->ownerArmyRuntimeOrSavedOffset.armyRuntime;
      if (armyRuntime->factionIndex == absorbedFactionIndex) {
        armyRuntime->factionIndex = survivingFactionIndex;
        ModelRuntimeHierarchy_SetPaletteAndTextureSetNonNullRecursive
                  (survivingFactionPaletteAsset,survivingFactionTextureSet,armyRuntime->modelNodeRuntime);
      }
    }
  }
  /* the player blocks are read after the repaint walk */
  playerBlockCursor = g_FrontendPlayerRuntimeBlocks;
  playerBlocksRemaining = g_FrontendPlayerRuntimeBlockCount;
  do {
    if (absorbedFactionIndex == (playerBlockCursor->factionAssignment).factionAssignmentIndex) {
      playerRuntimeId = playerBlockCursor->playerRuntimeId;
      (playerBlockCursor->factionAssignment).factionAssignmentIndex = survivingFactionIndex;
      g_SelectionPlayerRuntimeBlockPointers[playerRuntimeId]->factionIndex = survivingFactionIndex;
    }
    playerBlockCursor++;
    playerBlocksRemaining--;
  } while (playerBlocksRemaining != 0);
  /* the survivor also gets the absorbed faction's per-faction cell byte (its byte of the cell's occupancyMask) */
  terrainGrid = runtimeRoot->worldRuntime.fieldGrid;
  cellsRemaining = terrainGrid->gridWidth * terrainGrid->gridHeight;
  gridCell = terrainGrid->cells;
  do {
    /* byte view of the 64-bit mask: one byte per faction */
    reinterpret_cast<uint8_t *>(&gridCell->occupancyMask)[survivingFactionIndex] =
         reinterpret_cast<uint8_t *>(&gridCell->occupancyMask)[survivingFactionIndex] |
         reinterpret_cast<uint8_t *>(&gridCell->occupancyMask)[absorbedFactionIndex];
    gridCell++;
    cellsRemaining--;
  } while (cellsRemaining != 0);
  g_GameFactionRuntimeImage.tail.factionLifecycleStates[absorbedFactionIndex] = FACTION_RUNTIME_LIFECYCLE_INACTIVE;
  survivor = &g_GameFactionRuntimeImage.records[survivingFactionIndex];
  absorbed = &g_GameFactionRuntimeImage.records[absorbedFactionIndex];
  tritiumAmount = absorbed->tritiumCurrentQ4;
  xeniteLimit = absorbed->xeniteStorageLimitQ4;
  tritiumLimit = absorbed->tritiumStorageLimitQ4;
  survivor->xeniteCurrentQ4 = survivor->xeniteCurrentQ4 + absorbed->xeniteCurrentQ4;
  tritiumField = &survivor->tritiumCurrentQ4;
  *tritiumField = *tritiumField + tritiumAmount;
  xeniteField = &survivor->xeniteStorageLimitQ4;
  *xeniteField = *xeniteField + xeniteLimit;
  tritiumField = &survivor->tritiumStorageLimitQ4;
  *tritiumField = *tritiumField + tritiumLimit;
  energyCapacity = absorbed->energyGenerationCapacityQ4;
  energyField = &survivor->baselineEnergySupplyQ4;
  *energyField = *energyField + absorbed->baselineEnergySupplyQ4;
  energyField = &survivor->energyGenerationCapacityQ4;
  *energyField = *energyField + energyCapacity;
  for (maskWordIndex = 0; maskWordIndex < 8; maskWordIndex++) {
    survivor->technologyMasks256Bits[maskWordIndex] =
         survivor->technologyMasks256Bits[maskWordIndex] | absorbed->technologyMasks256Bits[maskWordIndex];
  }
  tritiumExtracted = absorbed->tritiumExtractedTotalQ4;
  counterA = absorbed->relationCounterA;
  counterB = absorbed->relationCounterB;
  xeniteField = &survivor->xeniteExtractedTotalQ4;
  *xeniteField = *xeniteField + absorbed->xeniteExtractedTotalQ4;
  tritiumField = &survivor->tritiumExtractedTotalQ4;
  *tritiumField = *tritiumField + tritiumExtracted;
  relationCounter = &survivor->relationCounterA;
  *relationCounter = *relationCounter + counterA;
  relationCounter = &survivor->relationCounterB;
  *relationCounter = *relationCounter + counterB;
  counterD = absorbed->relationCounterD;
  counterE = absorbed->relationCounterE;
  counterF = absorbed->relationCounterF;
  relationCounter = &survivor->relationCounterC;
  *relationCounter = *relationCounter + absorbed->relationCounterC;
  relationCounter = &survivor->relationCounterD;
  *relationCounter = *relationCounter + counterD;
  relationCounter = &survivor->relationCounterE;
  *relationCounter = *relationCounter + counterE;
  relationCounter = &survivor->relationCounterF;
  *relationCounter = *relationCounter + counterF;
  /* append both army-asset lists (primaryArmyAssetPointersOrIds, secondaryArmyAssetPointersOrIds; 64 entries
     each) */
  GameFactionRuntime_AppendArmyAssetList(&survivor->primaryArmyAssetCount,survivor->primaryArmyAssetPointersOrIds,
                                         absorbed->primaryArmyAssetCount,absorbed->primaryArmyAssetPointersOrIds);
  GameFactionRuntime_AppendArmyAssetList(&survivor->secondaryArmyAssetCount,
                                         survivor->secondaryArmyAssetPointersOrIds,
                                         absorbed->secondaryArmyAssetCount,absorbed->secondaryArmyAssetPointersOrIds);
  InGameArmyStock_RebuildGrid(&g_InGameRuntimeRoot->rootUi.base);
  InGameSpecialBuildCatalog_RebuildGrid(&g_InGameRuntimeRoot->rootUi.base);
  InGameBuildCatalog_RebuildGrid(&g_InGameRuntimeRoot->rootUi.base);
}

/* Changes the diplomatic relation between two factions: notifies the shown faction (text code + 500), stores
   both directed 4-bit relation states, sets or clears the factions' friendly bits in each other's
   capabilityFlags (both by stateSecondTowardFirst: friendly from state 4 on) and stamps the change tick.
   State 11 merges the factions: one of them (see below) gives its units, cells, players, resources,
   technology, statistics and army-asset lists to the other and becomes inactive. Finally the other-player
   command entries are rebuilt.
*/
void GameFactionRuntime_ApplyPairwiseRelationTransition(FactionNotificationCodeBase activeFactionCodeForFirst,
          FactionNotificationCodeBase activeFactionCodeForSecond,
          FactionRelationStateNibble stateFirstTowardSecond,
          FactionRelationStateNibble stateSecondTowardFirst,FactionRuntimeIndex firstFactionIndex,
          FactionRuntimeIndex secondFactionIndex)

{
  FactionCapabilityFlags *capabilityFlagsField;
  InGameSimulationTick currentTick;
  FactionRuntimeIndex originalFirstFactionIndex;
  uint32_t secondFactionBit;
  uint32_t firstFactionBit;
  uint32_t secondFactionPlayerCount;
  uint32_t firstFactionPlayerCount;
  uint32_t randomValue;
  Bool8 swapMergeDirection;
  uint8_t secondShift;
  uint8_t firstShift;
  FrontendPlayerRuntimeRecord *playerBlockCursor;
  FrontendPlayerRuntimeBlockCount playerBlocksRemaining;

  originalFirstFactionIndex = firstFactionIndex;
  if (g_InGameRuntimeRoot->worldRuntime.activeFactionRuntimeIndex == secondFactionIndex) {
    InGameNotificationQueue_InsertPriorityRecord(NOTIFICATION_PAYLOAD_NONE,0,0,0,0,0,2,activeFactionCodeForSecond +
                                                 500);
  }
  else if (g_InGameRuntimeRoot->worldRuntime.activeFactionRuntimeIndex == firstFactionIndex) {
    InGameNotificationQueue_InsertPriorityRecord(NOTIFICATION_PAYLOAD_NONE,0,0,0,0,0,2,activeFactionCodeForFirst + 500);
  }
  /* each record's packedRelationStates holds a nibble per other faction */
  secondShift = (uint8_t)secondFactionIndex * 4;
  firstShift = (uint8_t)firstFactionIndex * 4;
  g_GameFactionRuntimeImage.records[firstFactionIndex].packedRelationStates =
       stateFirstTowardSecond << (secondShift & 31) |
       ~(0xf << (secondShift & 31)) &
       g_GameFactionRuntimeImage.records[firstFactionIndex].packedRelationStates;
  g_GameFactionRuntimeImage.records[secondFactionIndex].packedRelationStates =
       ~(0xf << (firstShift & 31)) &
       g_GameFactionRuntimeImage.records[secondFactionIndex].packedRelationStates |
       stateSecondTowardFirst << (firstShift & 31);
  secondFactionBit = 1 << ((uint8_t)secondFactionIndex & 31);
  if (stateSecondTowardFirst < FACTION_RELATION_STATE_FRIENDLY) {
    capabilityFlagsField = &g_GameFactionRuntimeImage.records[firstFactionIndex].capabilityFlags;
    *capabilityFlagsField = *capabilityFlagsField & ~secondFactionBit;
  }
  else {
    capabilityFlagsField = &g_GameFactionRuntimeImage.records[firstFactionIndex].capabilityFlags;
    *capabilityFlagsField = *capabilityFlagsField | secondFactionBit;
  }
  firstFactionBit = 1 << ((uint8_t)firstFactionIndex & 31);
  if (stateSecondTowardFirst < FACTION_RELATION_STATE_FRIENDLY) {
    capabilityFlagsField = &g_GameFactionRuntimeImage.records[secondFactionIndex].capabilityFlags;
    *capabilityFlagsField = *capabilityFlagsField & ~firstFactionBit;
  }
  else {
    capabilityFlagsField = &g_GameFactionRuntimeImage.records[secondFactionIndex].capabilityFlags;
    *capabilityFlagsField = *capabilityFlagsField | firstFactionBit;
  }
  /* tick of the last relation change per pair (relationStateTicks[other faction]) */
  currentTick = g_GameFactionRuntimeImage.tail.simulationTick;
  g_GameFactionRuntimeImage.records[firstFactionIndex].relationStateTicks[secondFactionIndex] =
       g_GameFactionRuntimeImage.tail.simulationTick;
  g_GameFactionRuntimeImage.records[secondFactionIndex].relationStateTicks[firstFactionIndex] = currentTick;
  if (stateSecondTowardFirst == FACTION_RELATION_MERGE) {
    secondFactionPlayerCount = 0;
    firstFactionPlayerCount = 0;
    playerBlocksRemaining = g_FrontendPlayerRuntimeBlockCount;
    playerBlockCursor = g_FrontendPlayerRuntimeBlocks;
    do {
      if (secondFactionIndex == (playerBlockCursor->factionAssignment).factionAssignmentIndex) {
        secondFactionPlayerCount++;
      }
      if (firstFactionIndex == (playerBlockCursor->factionAssignment).factionAssignmentIndex) {
        firstFactionPlayerCount++;
      }
      playerBlockCursor++;
      playerBlocksRemaining--;
    } while (playerBlocksRemaining != 0);
    /* State 11 merges the two factions. The second faction survives when it has players and the (bitwise) counts
       do not overlap; with no players on either side, or overlapping counts, a random bit decides. */
    if (((secondFactionPlayerCount & firstFactionPlayerCount) == 0) &&
        ((secondFactionPlayerCount != 0) || (firstFactionPlayerCount != 0))) {
      swapMergeDirection = secondFactionPlayerCount != 0;
    }
    else {
      randomValue = g_RandomGeneratorState.next();
      swapMergeDirection = (randomValue & FACTION_MERGE_RANDOM_DIRECTION_BIT) == 0;
    }
    /* from here on firstFactionIndex survives and secondFactionIndex is absorbed */
    if (swapMergeDirection) {
      firstFactionIndex = secondFactionIndex;
      secondFactionIndex = originalFirstFactionIndex;
    }
    GameFactionRuntime_MergeAbsorbedFaction(firstFactionIndex,secondFactionIndex);
  }
  InGameOtherPlayerCommand_RebuildTargetEntries(&g_InGameRuntimeRoot->rootUi.base);
}

/* Diplomatic side effects of a shot hitting an army (called by the projectile maintenance in
   world/shots/flight.cpp). A repair shot (negative impact damage) that finds its target fully repaired
   ends the shooter's command on it. Any other hit adds to the pair pressure of target and shooter faction;
   if the target's faction already treats the shooter as hostile, the pair's relation tick is renewed and
   the "under attack" alert runs, otherwise friendly fire declares hostility (relation state 0 both ways),
   unless the shooter's active command targets another faction or the pair's last relation change is too
   recent (the ticks elapsed in both directions add up to less than 100).
*/
void ShotRuntime_ApplyArmyHitRelationAndNotifications(ModelRuntimeSlot *targetModelRuntime,ShotRuntimeSlot *shotRuntime)

{
  ArmyRuntimeSlot *shooterArmy;
  InGameSimulationTick currentTick;
  InGameRuntimeRoot *inGameRoot;
  FactionRelationState relationState;
  Bool8 alreadyHostile;
  Q12 conditionRatio;
  FactionNotificationCodeBase activeFactionCodeForFirst;
  FactionNotificationCodeBase activeFactionCodeForSecond;
  FactionRelationStateNibble stateFirstTowardSecond;
  FactionRelationStateNibble stateSecondTowardFirst;
  uint32_t targetFactionIndex;
  uint32_t shooterFactionIndex;
  GameEntityRuntime *targetEntity;
  
  shooterArmy = shotRuntime->ownerAndTrajectory.ownerArmyRuntime;
  targetEntity = targetModelRuntime->ownerArmyRuntimeOrSavedOffset.entityRuntime; /* the hit model's army */
  if (shooterArmy != nullptr) {
    if (shotRuntime->definitionOrSavedId.definition->targetClassImpactDamageQ12[0] < 0) {
      /* a condition ratio of 1.0 means the target is fully repaired */
      conditionRatio = ModelRuntime_QueryHierarchyConditionRatioQ12
                        (reinterpret_cast<RuntimeModelFactionPrefix *>(targetEntity)); /* the army's prefix view */
      if (conditionRatio == Q12_ONE &&
          (shooterArmy->commandModeFlags & ARMY_COMMAND_MODE_TARGET_ARMY) != 0 &&
          targetEntity == static_cast<GameEntityRuntime *>(shooterArmy->commandTargetArmyRuntime)) {
        ArmyRuntimeCommand_InterruptActiveTargetAndStampGeneration(shooterArmy);
        shooterArmy->commandGeneration = 1;
      }
    }
    else {
      shooterFactionIndex = shooterArmy->factionIndex;
      /* the target's owning faction */
      targetFactionIndex = targetEntity->common.ownership.ownerIndex;
      g_GameDataAuxState.pairPressureMatrix8x8[targetFactionIndex * 8 + shooterFactionIndex] += 256;
      if (shooterFactionIndex != 0 && targetFactionIndex != 0 && shooterFactionIndex != targetFactionIndex) {
        alreadyHostile = GameFactionRuntime_TestCapabilityBitClear(targetFactionIndex,shooterFactionIndex);
        inGameRoot = g_InGameRuntimeRoot;
        currentTick = g_GameFactionRuntimeImage.tail.simulationTick;
        /* relationStateTicks: tick of the pair's last relation change */
        if (alreadyHostile) {
          g_GameFactionRuntimeImage.records[shooterFactionIndex].relationStateTicks[targetFactionIndex] =
               g_GameFactionRuntimeImage.tail.simulationTick;
          g_GameFactionRuntimeImage.records[targetFactionIndex].relationStateTicks[shooterFactionIndex] =
               currentTick;
          GameFactionRuntime_UpdateImpactAlertAnchorAndNotify
                    (targetModelRuntime,&inGameRoot->worldRuntime);
        }
        else if (((shooterArmy->commandModeFlags & ARMY_COMMAND_MODE_TARGET_ARMY) == 0 ||
                  (shooterArmy->commandTargetArmyRuntime != nullptr &&
                   targetFactionIndex == shooterArmy->commandTargetArmyRuntime->factionIndex)) &&
                 99 < (int)((g_GameFactionRuntimeImage.tail.simulationTick * 2 -
                             g_GameFactionRuntimeImage.records[shooterFactionIndex].relationStateTicks
                             [targetFactionIndex]) -
                            g_GameFactionRuntimeImage.records[targetFactionIndex].relationStateTicks
                            [shooterFactionIndex])) {
          stateSecondTowardFirst = 0;
          stateFirstTowardSecond = 0;
          /* notification text code 11, or 12 when the relation was at state 8 or above */
          activeFactionCodeForSecond = 11;
          activeFactionCodeForFirst = 11;
          relationState = GameFactionRuntime_GetPackedStateNibble(targetFactionIndex,shooterFactionIndex);
          if (7 < relationState) {
            activeFactionCodeForFirst = 12;
            activeFactionCodeForSecond = 12;
          }
          GameFactionRuntime_ApplyPairwiseRelationTransition
                    (activeFactionCodeForFirst,activeFactionCodeForSecond,stateFirstTowardSecond,
                     stateSecondTowardFirst,targetFactionIndex,shooterFactionIndex);
        }
      }
    }
  }
}

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
   Ownership: gameplay/faction/relations.
   Purpose: Walks every active opposing faction, tests pair-transition rules, and dispatches state-specific random
   advance, faster advance, or reset behavior for the selected faction.
   Local calls: GameFactionRelations_TestPairTransitionAllowed, GameFactionRelations_IsResetEligibleState,
   GameFactionRelations_MaybeResetPairState, GameFactionRelations_MaybeAdvancePairStateRare,
   GameFactionRelations_MaybeAdvancePairStateCommon.
*/
void __thandor_void_preserve_eax_ecx_edx
GameFactionRelations_UpdateAllPairsForFaction
          (FactionRuntimeIndex sourceFactionIndex,WorldRuntimeContext *worldRuntime)

{
  int opposingFactionIndex;
  FactionRuntimeIndex unusedFactionIndex;
  bool pairTestResult;
  
  opposingFactionIndex = 7;
  do {
    if ((g_GameFactionRuntimeImage.tail.factionLifecycleStates[opposingFactionIndex] ==
         FACTION_RUNTIME_LIFECYCLE_ACTIVE) && (sourceFactionIndex != opposingFactionIndex)) {
      pairTestResult = GameFactionRelations_TestPairTransitionAllowed
                        (opposingFactionIndex,sourceFactionIndex);
      if (pairTestResult) {
        pairTestResult = GameFactionRelations_IsResetEligibleState
                          (opposingFactionIndex,sourceFactionIndex);
        if (!pairTestResult) {
          GameFactionRelations_MaybeResetPairState(opposingFactionIndex,sourceFactionIndex);
        }
      }
      else {
        pairTestResult = GameFactionRelations_IsResetEligibleState
                          (opposingFactionIndex,sourceFactionIndex);
        if (pairTestResult) {
          GameFactionRelations_MaybeAdvancePairStateRare(opposingFactionIndex,sourceFactionIndex);
        }
        else {
          GameFactionRelations_MaybeAdvancePairStateCommon(opposingFactionIndex,sourceFactionIndex)
          ;
        }
      }
    }
    opposingFactionIndex = opposingFactionIndex + -1;
  } while (opposingFactionIndex != 0);
  return;
}


/* Address: 0x00560E30.
   Ownership: gameplay/faction/relations.
   Purpose: Iterates a 0x1000-spaced key range and inserts each key/value pair through PlayerPairList_InsertUnique.
   Kept distinct from frontend slot indices, faction runtime indices, network endpoint identity, and PCK asset
   identifiers. Typed parameters: p2 playerRuntimeId→PlayerRuntimeId. Calling convention, storage, body bytes,
   control flow, and executable data remain unchanged. Typed parameters: p4 pairValue→SelectionPlayerPairValue.
   Local calls: PlayerPairList_InsertUnique.
*/
void __thandor_void_preserve_eax_ecx_edx
PlayerPairList_InsertRange
          (PlayerRuntimeId playerRuntimeId,SelectionPlayerPairValue endKey,
          SelectionPlayerPairValue pairValue,SelectionPlayerPairValue startKey)

{
  for (; (int)startKey <= (int)endKey; startKey = startKey + 0x1000) {
    PlayerPairList_InsertUnique(playerRuntimeId,0,pairValue,startKey);
  }
  return;
}


/* Address: 0x00560E70.
   Ownership: gameplay/faction/relations.
   Purpose: Iterates a 0x1000-spaced key range and removes each key/value pair through
   PlayerPairList_RemoveFirstMatch. Kept distinct from frontend slot indices, faction runtime indices, network
   endpoint identity, and PCK asset identifiers. Typed parameters: p2 playerRuntimeId→PlayerRuntimeId. Calling
   convention, storage, body bytes, control flow, and executable data remain unchanged. Typed parameters: p4
   pairValue→SelectionPlayerPairValue.
   Local calls: PlayerPairList_RemoveFirstMatch.
*/
void __thandor_void_preserve_eax_ecx_edx
PlayerPairList_RemoveRange
          (PlayerRuntimeId playerRuntimeId,SelectionPlayerPairValue endKey,
          SelectionPlayerPairValue pairValue,SelectionPlayerPairValue startKey)

{
  for (; (int)startKey <= (int)endKey; startKey = startKey + 0x1000) {
    PlayerPairList_RemoveFirstMatch(playerRuntimeId,0,pairValue,startKey);
  }
  return;
}


/* Address: 0x0053C3D0.
   Ownership: gameplay/faction/relations.
   Purpose: Rejects terminal or globally disabled relation states, combines each faction eligibility mask for low
   states, and evaluates transition rules for both directions, returning permission through carry.
   Local calls: GameFactionRelations_BuildEligibleFactionMask, GameFactionRelations_EvaluateTransitionRules.
   Cross-module calls: GameFactionRuntime_GetPackedStateNibble [gameplay/faction/runtime].
*/
bool __thandor_cf_preserve_eax_ecx_edx
GameFactionRelations_TestPairTransitionAllowed
          (FactionRuntimeIndex sourceFactionIndex,FactionRuntimeIndex targetFactionIndex)

{
  FactionRelationState relationState;
  FactionActiveMask targetEligibleMask;
  FactionActiveMask sourceEligibleMask;
  bool rulesSatisfied;
  
  relationState = GameFactionRuntime_GetPackedStateNibble(sourceFactionIndex,targetFactionIndex);
  if (((((relationState != 2) && (relationState != 5)) && (relationState != 9)) &&
      ((g_GameFactionRuntimeImage.tail.relationUiFlags & 4) == 0)) &&
     ((relationState < 4 ||
      (((g_GameFactionRuntimeImage.tail.relationUiFlags & 2) == 0 &&
       ((relationState < 8 || ((g_GameFactionRuntimeImage.tail.relationUiFlags & 1) == 0)))))))) {
    if (3 < relationState) {
      return false;
    }
    targetEligibleMask = GameFactionRelations_BuildEligibleFactionMask(targetFactionIndex);
    sourceEligibleMask = GameFactionRelations_BuildEligibleFactionMask(sourceFactionIndex);
    rulesSatisfied = GameFactionRelations_EvaluateTransitionRules(targetFactionIndex,sourceEligibleMask | targetEligibleMask);
    if ((!rulesSatisfied) &&
       (rulesSatisfied = GameFactionRelations_EvaluateTransitionRules(sourceFactionIndex,sourceEligibleMask | targetEligibleMask),
       !rulesSatisfied)) {
      return false;
    }
  }
  return true;
}


/* Address: 0x0053C090.
   Ownership: gameplay/faction/relations.
   Purpose: Builds an eight-bit mask containing active factions that are either the queried faction itself or have
   a packed pairwise relation state below four.
   Cross-module calls: GameFactionRuntime_GetPackedStateNibble [gameplay/faction/runtime].
*/
FactionActiveMask __thandor_eax_preserve_ecx_edx
GameFactionRelations_BuildEligibleFactionMask(FactionRuntimeIndex sourceFactionIndex)

{
  uint32_t relationStateNibble;
  int factionIndex;
  uint32_t eligibleFactionMask;
  uint32_t currentFactionBit;
  
  eligibleFactionMask = 0;
  currentFactionBit = 0x80;
  factionIndex = 7;
  do {
    if (g_GameFactionRuntimeImage.tail.factionLifecycleStates[factionIndex] ==
        FACTION_RUNTIME_LIFECYCLE_ACTIVE) {
      if ((factionIndex == sourceFactionIndex) ||
          (relationStateNibble = GameFactionRuntime_GetPackedStateNibble(sourceFactionIndex,factionIndex),
           3 < relationStateNibble)) {
        eligibleFactionMask = eligibleFactionMask | currentFactionBit;
      }
    }
    currentFactionBit = currentFactionBit >> 1;
    factionIndex = factionIndex + -1;
    if (factionIndex == 0) {
      return eligibleFactionMask;
    }
  } while( true );
}


/* Address: 0x0053C0F0.
   Ownership: gameplay/faction/relations.
   Purpose: Handles game faction relations evaluate transition rules carry-flag result.
*/

bool __thandor_cf_preserve_eax_ecx_edx
GameFactionRelations_EvaluateTransitionRules
          (FactionRuntimeIndex focalFactionIndex,FactionActiveMask activeFactionMask)

{
  uint8_t tokenOrFactionIndex;
  InGameLevelConditionStorageView800 *levelConditionStorage;
  uint32_t currentActiveMask;
  int remainingCount;
  uint32_t currentFactionBit;
  InGameScheduledConditionKind kindOrStackValue;
  uint8_t *expressionCursor;
  uint8_t movieVariant;
  InGameConditionScheduleImageView480 *conditionCursor;
  InGameEndConditionTriggerRecord8ReferenceView *triggerCursor;
  
  levelConditionStorage = g_InGameLevelRuntimeGlobalBlock.conditionStorage;
  currentActiveMask = 0;
  currentFactionBit = 0x80;
  remainingCount = 7;
  do {
    if (g_GameFactionRuntimeImage.tail.factionLifecycleStates[remainingCount] ==
        FACTION_RUNTIME_LIFECYCLE_ACTIVE) {
      currentActiveMask = currentActiveMask | currentFactionBit;
    }
    currentFactionBit = currentFactionBit >> 1;
    remainingCount = remainingCount + -1;
  } while (remainingCount != 0);
  if (currentActiveMask != activeFactionMask) {
    remainingCount = 0x40;
    conditionCursor = &(g_InGameLevelRuntimeGlobalBlock.conditionStorage)->schedule;
    do {
      kindOrStackValue = conditionCursor->conditions[0].statusAndKind.kind;
      conditionCursor->conditions[0].statusAndKind.kind =
           conditionCursor->conditions[0].statusAndKind.kind & 0xfffffffe;
                    // WARNING: Switch is manually overridden
      switch(kindOrStackValue & 0xfe) {
      case INGAME_SCHEDULED_CONDITION_NO_ACTIVE_ENTITY_WITH_DEFINITION:
        if ((activeFactionMask & 1 << ((uint8_t)conditionCursor->conditions[0].payload.operands[0] & 0x1f)) == 0
           ) {
          conditionCursor->conditions[0].statusAndKind.kind = conditionCursor->conditions[0].statusAndKind.kind | 1;
        }
        break;
      case INGAME_SCHEDULED_CONDITION_NO_ACTIVE_ENTITY_WITH_DEFINITION_AND_CLASS_COMMAND_GROUP_A:
        if ((activeFactionMask & 1 << ((uint8_t)conditionCursor->conditions[0].payload.operands[0] & 0x1f)) == 0
           ) {
          conditionCursor->conditions[0].statusAndKind.kind = conditionCursor->conditions[0].statusAndKind.kind | 1;
        }
        break;
      case INGAME_SCHEDULED_CONDITION_NO_ACTIVE_ENTITY_WITH_DEFINITION_AND_RUNTIME_ID:
        if ((activeFactionMask & 1 << ((uint8_t)conditionCursor->conditions[0].payload.operands[0] & 0x1f)) == 0
           ) {
          conditionCursor->conditions[0].statusAndKind.kind = conditionCursor->conditions[0].statusAndKind.kind | 1;
        }
        break;
      case INGAME_SCHEDULED_CONDITION_FACTION_INACTIVE_OR_RELATION_AT_LEAST_8:
        conditionCursor->conditions[0].statusAndKind.kind = conditionCursor->conditions[0].statusAndKind.kind | 1;
        break;
      case 
      INGAME_SCHEDULED_CONDITION_MATCHING_DEFINITION_AND_RUNTIME_ID_ACTIVE_ENTITY_COUNT_AT_LEAST:
        if ((activeFactionMask & 1 << ((uint8_t)conditionCursor->conditions[0].payload.operands[0] & 0x1f)) != 0
           ) {
          conditionCursor->conditions[0].statusAndKind.kind = conditionCursor->conditions[0].statusAndKind.kind | 1;
        }
        break;
      case INGAME_SCHEDULED_CONDITION_FACTION_TERRAIN_OCCUPANCY_MASK_F9_PERCENT_AT_LEAST:
        conditionCursor->conditions[0].statusAndKind.kind = conditionCursor->conditions[0].statusAndKind.kind | 1;
        break;
      case INGAME_SCHEDULED_CONDITION_COUNTDOWN_ELAPSED:
        if (conditionCursor->conditions[0].payload.operands[1] == 0) {
          conditionCursor->conditions[0].statusAndKind.kind = conditionCursor->conditions[0].statusAndKind.kind | 1;
        }
        break;
      case INGAME_SCHEDULED_CONDITION_PRIMARY_RESOURCE_LIMIT_AT_MOST_0FA0:
        conditionCursor->conditions[0].statusAndKind.kind = conditionCursor->conditions[0].statusAndKind.kind | 1;
        break;
      case INGAME_SCHEDULED_CONDITION_NO_ACTIVE_ENTITY_WITH_CLASS_ID_OUTSIDE_CLASS_COMMAND_GROUP_A:
        if ((activeFactionMask & 1 << ((uint8_t)conditionCursor->conditions[0].payload.operands[0] & 0x1f)) == 0
           ) {
          conditionCursor->conditions[0].statusAndKind.kind = conditionCursor->conditions[0].statusAndKind.kind | 1;
        }
        break;
      case INGAME_SCHEDULED_CONDITION_BOOLEAN_POSTFIX_EXPRESSION:
        expressionCursor = (uint8_t *)((int)&conditionCursor->conditions[0].statusAndKind.kind + 1);
        kindOrStackValue = INGAME_SCHEDULED_CONDITION_NONE_OR_UNUSED;
        while( true ) {
          while( true ) {
            while( true ) {
              while( true ) {
                tokenOrFactionIndex = *expressionCursor;
                expressionCursor = expressionCursor + 1;
                if (tokenOrFactionIndex != 0xff) break;
                kindOrStackValue = kindOrStackValue >> 1 | kindOrStackValue & 1;
              }
              if (tokenOrFactionIndex != 0xfe) break;
              kindOrStackValue = kindOrStackValue >> 1 & (kindOrStackValue | 0xfffffffe);
            }
            if (tokenOrFactionIndex != 0xfd) break;
            kindOrStackValue = kindOrStackValue ^ 1;
          }
          if (tokenOrFactionIndex == 0xfc) break;
          kindOrStackValue = ((levelConditionStorage->schedule).conditions[tokenOrFactionIndex].statusAndKind.kind & 1) + kindOrStackValue * 2;
        }
        conditionCursor->conditions[0].statusAndKind.kind =
             conditionCursor->conditions[0].statusAndKind.kind | kindOrStackValue & 1;
      }
      conditionCursor = (InGameConditionScheduleImageView480 *)(conditionCursor->conditions + 1);
      remainingCount = remainingCount + -1;
    } while (remainingCount != 0);
    triggerCursor = (levelConditionStorage->schedule).triggers;
    remainingCount = 0x10;
    do {
      if ((triggerCursor->stateFlags == INGAME_END_CONDITION_TRIGGER_ACTIVE) &&
         (((levelConditionStorage->schedule).conditions[triggerCursor->conditionIndex].statusAndKind.kind & 1) !=
          INGAME_SCHEDULED_CONDITION_NONE_OR_UNUSED)) {
        tokenOrFactionIndex = triggerCursor->factionRuntimeIndex;
        if (g_GameFactionRuntimeImage.tail.factionLifecycleStates[tokenOrFactionIndex] ==
            FACTION_RUNTIME_LIFECYCLE_ACTIVE) {
          movieVariant = triggerCursor->movieVariantSelector;
          if ((focalFactionIndex != (uint32_t)tokenOrFactionIndex) && ((activeFactionMask & 1 << (tokenOrFactionIndex & 0x1f)) == 0)
             ) {
            movieVariant = movieVariant ^ 1;
          }
          if (movieVariant == 0) {
            return true;
          }
          return false;
        }
      }
      triggerCursor = triggerCursor + 1;
      remainingCount = remainingCount + -1;
    } while (remainingCount != 0);
  }
  return true;
}


/* Address: 0x0053C490.
   Ownership: gameplay/faction/relations.
   Purpose: Tests whether a faction pair is in one of the verified relation states 3, 6, or 10 and returns the
   result through carry.
   Cross-module calls: GameFactionRuntime_GetPackedStateNibble [gameplay/faction/runtime].
*/
bool __thandor_cf_preserve_eax_ecx_edx
GameFactionRelations_IsResetEligibleState
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
   Ownership: gameplay/faction/relations.
   Purpose: Uses the current pair pressure and a random threshold to invoke pairwise relation advancement through
   the lower-probability 0x180/0x400 path. It is distinct from FrontendPlayerIndex_V306, PlayerRuntimeId, active-
   faction masks or codes, and PCK-backed ArmyAssetId, ModelDefinitionId, and TechnologyId domains.
   Cross-module calls: GameFactionRuntime_AdvancePairwiseRelationState [gameplay/faction/runtime].
*/
void __thandor_void_preserve_eax_ecx_edx
GameFactionRelations_MaybeAdvancePairStateRare
          (FactionRuntimeIndex sourceFactionIndex,FactionRuntimeIndex targetFactionIndex)

{
  uint32_t pairPressure;
  uint32_t randomValue;
  uint32_t maskedRandom;
  
  randomValue = g_RandomGeneratorState.next();
  pairPressure = g_GameDataAuxState.pairPressureMatrix8x8[targetFactionIndex * 8 + sourceFactionIndex];
  if (pairPressure == 0) {
    maskedRandom = randomValue & 0x17f;
  }
  else {
    if (0x1f < pairPressure) {
      return;
    }
    maskedRandom = randomValue & 0x3ff;
    if (randomValue >> 0x1c <= pairPressure) {
      return;
    }
  }
  if (maskedRandom == 0x55) {
    GameFactionRuntime_AdvancePairwiseRelationState
              (0xffffffff,0,sourceFactionIndex,targetFactionIndex);
  }
  return;
}


/* Address: 0x0053C540.
   Ownership: gameplay/faction/relations.
   Purpose: Uses the current pair pressure and a random threshold to invoke pairwise relation advancement through
   the higher-probability 0x80/0x200 path. It is distinct from FrontendPlayerIndex_V306, PlayerRuntimeId, active-
   faction masks or codes, and PCK-backed ArmyAssetId, ModelDefinitionId, and TechnologyId domains.
   Cross-module calls: GameFactionRuntime_AdvancePairwiseRelationState [gameplay/faction/runtime].
*/
void __thandor_void_preserve_eax_ecx_edx
GameFactionRelations_MaybeAdvancePairStateCommon
          (FactionRuntimeIndex sourceFactionIndex,FactionRuntimeIndex targetFactionIndex)

{
  uint32_t pairPressure;
  uint32_t randomValue;
  uint32_t maskedRandom;
  
  randomValue = g_RandomGeneratorState.next();
  pairPressure = g_GameDataAuxState.pairPressureMatrix8x8[targetFactionIndex * 8 + sourceFactionIndex];
  if (pairPressure == 0) {
    maskedRandom = randomValue & 0x7f;
  }
  else {
    if (0x1f < pairPressure) {
      return;
    }
    maskedRandom = randomValue & 0x1ff;
    if (randomValue >> 0x1c <= pairPressure) {
      return;
    }
  }
  if (maskedRandom == 0x55) {
    GameFactionRuntime_AdvancePairwiseRelationState
              (0xffffffff,0,sourceFactionIndex,targetFactionIndex);
  }
  return;
}


/* Address: 0x0053C5B0.
   Ownership: gameplay/faction/relations.
   Purpose: Randomly invokes the shared pairwise relation reset helper when the verified random mask equals 0x80.
   It is distinct from FrontendPlayerIndex_V306, PlayerRuntimeId, active-faction masks or codes, and PCK-backed
   ArmyAssetId, ModelDefinitionId, and TechnologyId domains.
   Cross-module calls: GameFactionRuntime_ResetPairwiseRelationState [gameplay/faction/runtime].
*/
void __thandor_void_preserve_eax_ecx_edx
GameFactionRelations_MaybeResetPairState
          (FactionRuntimeIndex sourceFactionIndex,FactionRuntimeIndex targetFactionIndex)

{
  uint32_t randomValue;
  
  randomValue = g_RandomGeneratorState.next();
  if ((randomValue & 0x180) == 0x80) {
    GameFactionRuntime_ResetPairwiseRelationState
              (0xffffffff,0,sourceFactionIndex,targetFactionIndex);
  }
  return;
}


/* Address: 0x00560EB0.
   Ownership: gameplay/faction/relations.
   Purpose: Inserts one unique two-dword pair into the per-player pair list and increments the local-player
   visible-count field. Kept distinct from frontend slot indices, faction runtime indices, network endpoint
   identity, and PCK asset identifiers. Typed parameters: p2 playerRuntimeId→PlayerRuntimeId. Calling convention,
   storage, body bytes, control flow, and executable data remain unchanged. Typed parameters: p4
   pairValue→SelectionPlayerPairValue.
*/
void __thandor_void_preserve_eax_ecx_edx
PlayerPairList_InsertUnique
          (PlayerRuntimeId playerRuntimeId,uint32_t reservedZero,SelectionPlayerPairValue pairValue,
          SelectionPlayerPairValue pairKey)

{
  SelectionPlayerPairRecord *pairRecordCursor;
  uint32_t recordsRemaining;
  SelectionPlayerRuntimeBlock *playerRuntimeBlock;
  uint32_t appendRecordIndex;
  InGameRuntimeRootImageC3E4 *inGameRuntimeRoot;
  
  inGameRuntimeRoot = g_InGameRuntimeRoot;
  playerRuntimeBlock = g_SelectionPlayerRuntimeBlockPointers[playerRuntimeId];
  recordsRemaining = playerRuntimeBlock->activePairCount8084;
  pairRecordCursor = playerRuntimeBlock->pairRecords80_807F;
  if (recordsRemaining < 0x1000) {
    for (; recordsRemaining != 0; recordsRemaining = recordsRemaining - 1) {
      if ((pairKey == pairRecordCursor->pairKey) && (pairValue == pairRecordCursor->pairValue)) {
        return;
      }
      pairRecordCursor = pairRecordCursor + 1;
    }
    appendRecordIndex = playerRuntimeBlock->activePairCount8084;
    playerRuntimeBlock->activePairCount8084 = playerRuntimeBlock->activePairCount8084 + 1;
    playerRuntimeBlock->pairRecords80_807F[appendRecordIndex].pairKey = pairKey;
    playerRuntimeBlock->pairRecords80_807F[appendRecordIndex].pairValue = pairValue;
    if (playerRuntimeId == g_LocalPlayerRuntimeId) {
      inGameRuntimeRoot->localPlayerPairCount0BA4 = inGameRuntimeRoot->localPlayerPairCount0BA4 + 1;
    }
  }
  return;
}


/* Address: 0x00560F50.
   Ownership: gameplay/faction/relations.
   Purpose: Removes the first matching two-dword pair from the per-player pair list and decrements the local-player
   visible-count field. Kept distinct from frontend slot indices, faction runtime indices, network endpoint
   identity, and PCK asset identifiers. Typed parameters: p2 playerRuntimeId→PlayerRuntimeId. Calling convention,
   storage, body bytes, control flow, and executable data remain unchanged. Typed parameters: p4
   pairValue→SelectionPlayerPairValue.
*/
void __thandor_void_preserve_eax_ecx_edx
PlayerPairList_RemoveFirstMatch
          (PlayerRuntimeId playerRuntimeId,uint32_t reservedZero,SelectionPlayerPairValue pairValue,
          SelectionPlayerPairValue pairKey)

{
  int trailingDwordsToMove;
  SelectionPlayerPairRecord *copySourceCursor;
  uint32_t recordsRemaining;
  SelectionPlayerPairRecord *pairRecordCursor;
  SelectionPlayerRuntimeBlock *playerRuntimeBlock;
  InGameRuntimeRootImageC3E4 *inGameRuntimeRoot;
  
  inGameRuntimeRoot = g_InGameRuntimeRoot;
  playerRuntimeBlock = g_SelectionPlayerRuntimeBlockPointers[playerRuntimeId];
  recordsRemaining = playerRuntimeBlock->activePairCount8084;
  pairRecordCursor = playerRuntimeBlock->pairRecords80_807F;
  if (recordsRemaining < 0x1000) {
    for (; recordsRemaining != 0; recordsRemaining = recordsRemaining - 1) {
      if ((pairKey == pairRecordCursor->pairKey) && (pairValue == pairRecordCursor->pairValue)) {
        copySourceCursor = pairRecordCursor + 1;
        trailingDwordsToMove = recordsRemaining * 2 + -2;
        playerRuntimeBlock->activePairCount8084 = playerRuntimeBlock->activePairCount8084 - 1;
        if (trailingDwordsToMove != 0) {
          for (; trailingDwordsToMove != 0; trailingDwordsToMove = trailingDwordsToMove + -1) {
            pairRecordCursor->pairKey = copySourceCursor->pairKey;
            copySourceCursor = (SelectionPlayerPairRecord *)&copySourceCursor->pairValue;
            pairRecordCursor = (SelectionPlayerPairRecord *)&pairRecordCursor->pairValue;
          }
        }
        if (playerRuntimeId == g_LocalPlayerRuntimeId) {
          inGameRuntimeRoot->localPlayerPairCount0BA4 =
               inGameRuntimeRoot->localPlayerPairCount0BA4 - 1;
        }
        return;
      }
      pairRecordCursor = pairRecordCursor + 1;
    }
  }
  return;
}


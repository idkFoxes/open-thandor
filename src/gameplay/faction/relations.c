/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/src/gameplay/faction/relations.c
 * Reverse engineering by idkFoxes 2026
 */

#include <thandor/gameplay/faction/relations.h>

/* Implementation ownership: gameplay/faction/relations. */

/* Address: 0x0053C010.
   Ownership: gameplay/faction/relations.
   Purpose: Walks every active opposing faction, tests pair-transition rules, and dispatches state-specific random
   advance, faster advance, or reset behavior for the selected faction.
   Local calls: GameFactionRelations_TestPairTransitionAllowedCf, GameFactionRelations_IsResetEligibleStateCf,
   GameFactionRelations_MaybeResetPairState, GameFactionRelations_MaybeAdvancePairStateRare,
   GameFactionRelations_MaybeAdvancePairStateCommon.
*/
void __thandor_void_preserve_eax_ecx_edx
GameFactionRelations_UpdateAllPairsForFaction
          (FactionRuntimeIndex sourceFactionIndex,WorldRuntimeContext *worldRuntime)

{
  int sourceFactionIndex_04;
  FactionRuntimeIndex sourceFactionIndex_03;
  bool bVar1;
  
  sourceFactionIndex_04 = 7;
  do {
    if ((g_GameFactionRuntimeImage.tail.factionLifecycleStates[sourceFactionIndex_04] ==
         FACTION_RUNTIME_LIFECYCLE_ACTIVE) && (sourceFactionIndex != sourceFactionIndex_04)) {
      bVar1 = GameFactionRelations_TestPairTransitionAllowedCf
                        (sourceFactionIndex_04,sourceFactionIndex);
      if (bVar1) {
        bVar1 = GameFactionRelations_IsResetEligibleStateCf
                          (sourceFactionIndex_04,sourceFactionIndex);
        if (!bVar1) {
          GameFactionRelations_MaybeResetPairState(sourceFactionIndex_04,sourceFactionIndex);
        }
      }
      else {
        bVar1 = GameFactionRelations_IsResetEligibleStateCf
                          (sourceFactionIndex_04,sourceFactionIndex);
        if (bVar1) {
          GameFactionRelations_MaybeAdvancePairStateRare(sourceFactionIndex_04,sourceFactionIndex);
        }
        else {
          GameFactionRelations_MaybeAdvancePairStateCommon(sourceFactionIndex_04,sourceFactionIndex)
          ;
        }
      }
    }
    sourceFactionIndex_04 = sourceFactionIndex_04 + -1;
  } while (sourceFactionIndex_04 != 0);
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
   Local calls: GameFactionRelations_BuildEligibleFactionMask, GameFactionRelations_EvaluateTransitionRulesCf.
   Cross-module calls: GameFactionRuntime_GetPackedStateNibble [gameplay/faction/runtime].
*/
bool __thandor_cf_preserve_eax_ecx_edx
GameFactionRelations_TestPairTransitionAllowedCf
          (FactionRuntimeIndex sourceFactionIndex,FactionRuntimeIndex targetFactionIndex)

{
  FactionRelationState FVar1;
  FactionActiveMask FVar2;
  FactionActiveMask FVar3;
  bool bVar4;
  
  FVar1 = GameFactionRuntime_GetPackedStateNibble(sourceFactionIndex,targetFactionIndex);
  if (((((FVar1 != 2) && (FVar1 != 5)) && (FVar1 != 9)) &&
      ((g_GameFactionRuntimeImage.tail.relationUiFlags & 4) == 0)) &&
     ((FVar1 < 4 ||
      (((g_GameFactionRuntimeImage.tail.relationUiFlags & 2) == 0 &&
       ((FVar1 < 8 || ((g_GameFactionRuntimeImage.tail.relationUiFlags & 1) == 0)))))))) {
    if (3 < FVar1) {
      return false;
    }
    FVar2 = GameFactionRelations_BuildEligibleFactionMask(targetFactionIndex);
    FVar3 = GameFactionRelations_BuildEligibleFactionMask(sourceFactionIndex);
    bVar4 = GameFactionRelations_EvaluateTransitionRulesCf(targetFactionIndex,FVar3 | FVar2);
    if ((!bVar4) &&
       (bVar4 = GameFactionRelations_EvaluateTransitionRulesCf(sourceFactionIndex,FVar3 | FVar2),
       !bVar4)) {
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
  dword relationStateNibble;
  int factionIndex;
  uint eligibleFactionMask;
  uint currentFactionBit;
  
  eligibleFactionMask = 0;
  currentFactionBit = 0x80;
  factionIndex = 7;
  do {
    if (g_GameFactionRuntimeImage.tail.factionLifecycleStates[factionIndex] ==
        FACTION_RUNTIME_LIFECYCLE_ACTIVE) {
      if (factionIndex != sourceFactionIndex) {
        relationStateNibble =
             GameFactionRuntime_GetPackedStateNibble(sourceFactionIndex,factionIndex);
        if (relationStateNibble < 4)
        goto GameFactionRelations_BuildEligibleFactionMask_AdvanceAfterEligibilityDecision;
      }
      eligibleFactionMask = eligibleFactionMask | currentFactionBit;
    }
GameFactionRelations_BuildEligibleFactionMask_AdvanceAfterEligibilityDecision:
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
GameFactionRelations_EvaluateTransitionRulesCf
          (FactionRuntimeIndex focalFactionIndex,FactionActiveMask activeFactionMask)

{
  byte bVar1;
  InGameLevelConditionStorageView800 *pIVar2;
  uint uVar3;
  int iVar4;
  uint uVar5;
  InGameScheduledConditionKind IVar6;
  byte *pbVar7;
  byte bVar8;
  InGameConditionScheduleImageView480 *pIVar9;
  InGameEndConditionTriggerRecord8ReferenceView *pIVar10;
  
  pIVar2 = g_InGameLevelRuntimeGlobalBlock.conditionStorage;
  uVar3 = 0;
  uVar5 = 0x80;
  iVar4 = 7;
  do {
    if (g_GameFactionRuntimeImage.tail.factionLifecycleStates[iVar4] ==
        FACTION_RUNTIME_LIFECYCLE_ACTIVE) {
      uVar3 = uVar3 | uVar5;
    }
    uVar5 = uVar5 >> 1;
    iVar4 = iVar4 + -1;
  } while (iVar4 != 0);
  if (uVar3 != activeFactionMask) {
    iVar4 = 0x40;
    pIVar9 = &(g_InGameLevelRuntimeGlobalBlock.conditionStorage)->schedule;
    do {
      IVar6 = pIVar9->conditions[0].statusAndKind.kind;
      pIVar9->conditions[0].statusAndKind.kind =
           pIVar9->conditions[0].statusAndKind.kind & 0xfffffffe;
                    // WARNING: Switch is manually overridden
      switch(IVar6 & 0xfe) {
      case INGAME_SCHEDULED_CONDITION_NO_ACTIVE_ENTITY_WITH_DEFINITION:
        if ((activeFactionMask & 1 << ((byte)pIVar9->conditions[0].payload.operands[0] & 0x1f)) == 0
           ) {
          pIVar9->conditions[0].statusAndKind.kind = pIVar9->conditions[0].statusAndKind.kind | 1;
        }
        break;
      case INGAME_SCHEDULED_CONDITION_NO_ACTIVE_ENTITY_WITH_DEFINITION_AND_CLASS_COMMAND_GROUP_A:
        if ((activeFactionMask & 1 << ((byte)pIVar9->conditions[0].payload.operands[0] & 0x1f)) == 0
           ) {
          pIVar9->conditions[0].statusAndKind.kind = pIVar9->conditions[0].statusAndKind.kind | 1;
        }
        break;
      case INGAME_SCHEDULED_CONDITION_NO_ACTIVE_ENTITY_WITH_DEFINITION_AND_RUNTIME_ID:
        if ((activeFactionMask & 1 << ((byte)pIVar9->conditions[0].payload.operands[0] & 0x1f)) == 0
           ) {
          pIVar9->conditions[0].statusAndKind.kind = pIVar9->conditions[0].statusAndKind.kind | 1;
        }
        break;
      case INGAME_SCHEDULED_CONDITION_FACTION_INACTIVE_OR_RELATION_AT_LEAST_8:
        pIVar9->conditions[0].statusAndKind.kind = pIVar9->conditions[0].statusAndKind.kind | 1;
        break;
      case 
      INGAME_SCHEDULED_CONDITION_MATCHING_DEFINITION_AND_RUNTIME_ID_ACTIVE_ENTITY_COUNT_AT_LEAST:
        if ((activeFactionMask & 1 << ((byte)pIVar9->conditions[0].payload.operands[0] & 0x1f)) != 0
           ) {
          pIVar9->conditions[0].statusAndKind.kind = pIVar9->conditions[0].statusAndKind.kind | 1;
        }
        break;
      case INGAME_SCHEDULED_CONDITION_FACTION_TERRAIN_OCCUPANCY_MASK_F9_PERCENT_AT_LEAST:
        pIVar9->conditions[0].statusAndKind.kind = pIVar9->conditions[0].statusAndKind.kind | 1;
        break;
      case INGAME_SCHEDULED_CONDITION_COUNTDOWN_ELAPSED:
        if (pIVar9->conditions[0].payload.operands[1] == 0) {
          pIVar9->conditions[0].statusAndKind.kind = pIVar9->conditions[0].statusAndKind.kind | 1;
        }
        break;
      case INGAME_SCHEDULED_CONDITION_PRIMARY_RESOURCE_LIMIT_AT_MOST_0FA0:
        pIVar9->conditions[0].statusAndKind.kind = pIVar9->conditions[0].statusAndKind.kind | 1;
        break;
      case INGAME_SCHEDULED_CONDITION_NO_ACTIVE_ENTITY_WITH_CLASS_ID_OUTSIDE_CLASS_COMMAND_GROUP_A:
        if ((activeFactionMask & 1 << ((byte)pIVar9->conditions[0].payload.operands[0] & 0x1f)) == 0
           ) {
          pIVar9->conditions[0].statusAndKind.kind = pIVar9->conditions[0].statusAndKind.kind | 1;
        }
        break;
      case INGAME_SCHEDULED_CONDITION_BOOLEAN_POSTFIX_EXPRESSION:
        pbVar7 = (byte *)((int)&pIVar9->conditions[0].statusAndKind.kind + 1);
        IVar6 = INGAME_SCHEDULED_CONDITION_NONE_OR_UNUSED;
        while( true ) {
          while( true ) {
            while( true ) {
              while( true ) {
                bVar1 = *pbVar7;
                pbVar7 = pbVar7 + 1;
                if (bVar1 != 0xff) break;
                IVar6 = IVar6 >> 1 | IVar6 & 1;
              }
              if (bVar1 != 0xfe) break;
              IVar6 = IVar6 >> 1 & (IVar6 | 0xfffffffe);
            }
            if (bVar1 != 0xfd) break;
            IVar6 = IVar6 ^ 1;
          }
          if (bVar1 == 0xfc) break;
          IVar6 = ((pIVar2->schedule).conditions[bVar1].statusAndKind.kind & 1) + IVar6 * 2;
        }
        pIVar9->conditions[0].statusAndKind.kind =
             pIVar9->conditions[0].statusAndKind.kind | IVar6 & 1;
      }
      pIVar9 = (InGameConditionScheduleImageView480 *)(pIVar9->conditions + 1);
      iVar4 = iVar4 + -1;
    } while (iVar4 != 0);
    pIVar10 = (pIVar2->schedule).triggers;
    iVar4 = 0x10;
    do {
      if ((pIVar10->stateFlags == INGAME_END_CONDITION_TRIGGER_ACTIVE) &&
         (((pIVar2->schedule).conditions[pIVar10->conditionIndex].statusAndKind.kind & 1) !=
          INGAME_SCHEDULED_CONDITION_NONE_OR_UNUSED)) {
        bVar1 = pIVar10->factionRuntimeIndex;
        if (g_GameFactionRuntimeImage.tail.factionLifecycleStates[bVar1] ==
            FACTION_RUNTIME_LIFECYCLE_ACTIVE) {
          bVar8 = pIVar10->movieVariantSelector;
          if ((focalFactionIndex != (uint)bVar1) && ((activeFactionMask & 1 << (bVar1 & 0x1f)) == 0)
             ) {
            bVar8 = bVar8 ^ 1;
          }
          if (bVar8 == 0) {
            return true;
          }
          return false;
        }
      }
      pIVar10 = pIVar10 + 1;
      iVar4 = iVar4 + -1;
    } while (iVar4 != 0);
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
GameFactionRelations_IsResetEligibleStateCf
          (FactionRuntimeIndex sourceFactionIndex,FactionRuntimeIndex targetFactionIndex)

{
  dword relationStateNibble;
  
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
  uint uVar1;
  dword dVar2;
  uint uVar3;
  
  dVar2 = (*g_RandomGeneratorState.next)();
  uVar1 = g_GameDataAuxState.pairPressureMatrix8x8[targetFactionIndex * 8 + sourceFactionIndex];
  if (uVar1 == 0) {
    uVar3 = dVar2 & 0x17f;
  }
  else {
    if (0x1f < uVar1) {
      return;
    }
    uVar3 = dVar2 & 0x3ff;
    if (dVar2 >> 0x1c <= uVar1) {
      return;
    }
  }
  if (uVar3 == 0x55) {
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
  uint uVar1;
  dword dVar2;
  uint uVar3;
  
  dVar2 = (*g_RandomGeneratorState.next)();
  uVar1 = g_GameDataAuxState.pairPressureMatrix8x8[targetFactionIndex * 8 + sourceFactionIndex];
  if (uVar1 == 0) {
    uVar3 = dVar2 & 0x7f;
  }
  else {
    if (0x1f < uVar1) {
      return;
    }
    uVar3 = dVar2 & 0x1ff;
    if (dVar2 >> 0x1c <= uVar1) {
      return;
    }
  }
  if (uVar3 == 0x55) {
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
  dword dVar1;
  
  dVar1 = (*g_RandomGeneratorState.next)();
  if ((dVar1 & 0x180) == 0x80) {
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
          (PlayerRuntimeId playerRuntimeId,dword reservedZero,SelectionPlayerPairValue pairValue,
          SelectionPlayerPairValue pairKey)

{
  SelectionPlayerPairRecord *pairRecordCursor;
  uint recordsRemaining;
  SelectionPlayerRuntimeBlock *playerRuntimeBlock;
  dword appendRecordIndex;
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
          (PlayerRuntimeId playerRuntimeId,dword reservedZero,SelectionPlayerPairValue pairValue,
          SelectionPlayerPairValue pairKey)

{
  int trailingDwordsToMove;
  SelectionPlayerPairRecord *copySourceCursor;
  uint recordsRemaining;
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


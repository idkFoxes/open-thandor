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
AiPreservedFactionIndexEdxResult
GameFactionRelations_UpdateAllPairsForFaction
          (FactionRuntimeIndex sourceFactionIndex,WorldRuntimeContext *worldRuntime)

{
  FactionRuntimeIndex targetFactionIndex;
  FactionRuntimeIndex targetFactionIndex_00;
  uint extraout_EAX;
  uint extraout_EAX_00;
  uint sourceFactionIndex_04;
  FactionRuntimeIndex sourceFactionIndex_05;
  uint sourceFactionIndex_06;
  uint extraout_ECX;
  FactionRuntimeIndex sourceFactionIndex_03;
  uint extraout_ECX_00;
  uint extraout_ECX_01;
  FactionRuntimeIndex in_EDX;
  bool bVar1;
  
  sourceFactionIndex_04 = 7;
  do {
    if ((g_GameFactionRuntimeImage.tail.factionLifecycleStates[sourceFactionIndex_04] ==
         FACTION_RUNTIME_LIFECYCLE_ACTIVE) &&
       (bVar1 = (uint)sourceFactionIndex < sourceFactionIndex_04,
       sourceFactionIndex != sourceFactionIndex_04)) {
      GameFactionRelations_TestPairTransitionAllowedCf(sourceFactionIndex_04,sourceFactionIndex);
      if (bVar1) {
        sourceFactionIndex =
             GameFactionRelations_IsResetEligibleStateCf(sourceFactionIndex_05,targetFactionIndex);
        sourceFactionIndex_04 = sourceFactionIndex_06;
        if (!bVar1) {
          sourceFactionIndex =
               GameFactionRelations_MaybeResetPairState(sourceFactionIndex_06,sourceFactionIndex);
          sourceFactionIndex_04 = extraout_ECX;
        }
      }
      else {
        targetFactionIndex_00 =
             GameFactionRelations_IsResetEligibleStateCf(sourceFactionIndex_05,targetFactionIndex);
        if (bVar1) {
          GameFactionRelations_MaybeAdvancePairStateRare
                    (sourceFactionIndex_03,targetFactionIndex_00);
          sourceFactionIndex = extraout_EAX;
          sourceFactionIndex_04 = extraout_ECX_00;
        }
        else {
          GameFactionRelations_MaybeAdvancePairStateCommon
                    (sourceFactionIndex_03,targetFactionIndex_00);
          sourceFactionIndex = extraout_EAX_00;
          sourceFactionIndex_04 = extraout_ECX_01;
        }
      }
    }
    sourceFactionIndex_04 = sourceFactionIndex_04 - 1;
  } while (sourceFactionIndex_04 != 0);
  return (AiPreservedFactionIndexEdxResult)in_EDX;
}

/* Address: 0x00560E30.
   Ownership: gameplay/faction/relations.
   Purpose: Iterates a 0x1000-spaced key range and inserts each key/value pair through PlayerPairList_InsertUnique.
   Kept distinct from frontend slot indices, faction runtime indices, network endpoint identity, and PCK asset
   identifiers. Typed parameters: p2 playerRuntimeId→PlayerRuntimeId. Calling convention, storage, body bytes,
   control flow, and executable data remain unchanged. Typed parameters: p4 pairValue→SelectionPlayerPairValue.
   Local calls: PlayerPairList_InsertUnique.
*/
undefined8
PlayerPairList_InsertRange
          (PlayerRuntimeId playerRuntimeId,SelectionPlayerPairValue endKey,
          SelectionPlayerPairValue pairValue,SelectionPlayerPairValue startKey)

{
  undefined4 in_EAX;
  SelectionPlayerPairValue extraout_ECX;
  undefined4 in_EDX;
  undefined8 operationResultPair;
  
  while ((int)startKey <= (int)endKey) {
    operationResultPair = PlayerPairList_InsertUnique(playerRuntimeId,0,pairValue,startKey);
    playerRuntimeId = (PlayerRuntimeId)((ulonglong)operationResultPair >> 0x20);
    endKey = extraout_ECX;
    startKey = (int)operationResultPair + 0x1000;
  }
  return CONCAT44(in_EDX,in_EAX);
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
undefined8
PlayerPairList_RemoveRange
          (PlayerRuntimeId playerRuntimeId,SelectionPlayerPairValue endKey,
          SelectionPlayerPairValue pairValue,SelectionPlayerPairValue startKey)

{
  undefined4 in_EAX;
  SelectionPlayerPairValue extraout_ECX;
  undefined4 in_EDX;
  undefined8 operationResultPair;
  
  while ((int)startKey <= (int)endKey) {
    operationResultPair = PlayerPairList_RemoveFirstMatch(playerRuntimeId,0,pairValue,startKey);
    playerRuntimeId = (PlayerRuntimeId)((ulonglong)operationResultPair >> 0x20);
    endKey = extraout_ECX;
    startKey = (int)operationResultPair + 0x1000;
  }
  return CONCAT44(in_EDX,in_EAX);
}

/* Address: 0x0053C3D0.
   Ownership: gameplay/faction/relations.
   Purpose: Rejects terminal or globally disabled relation states, combines each faction eligibility mask for low
   states, and evaluates transition rules for both directions, returning permission through carry.
   Local calls: GameFactionRelations_BuildEligibleFactionMask, GameFactionRelations_EvaluateTransitionRulesCf.
   Cross-module calls: GameFactionRuntime_GetPackedStateNibble [gameplay/faction/runtime].
*/
void GameFactionRelations_TestPairTransitionAllowedCf
               (FactionRuntimeIndex sourceFactionIndex,FactionRuntimeIndex targetFactionIndex)

{
  FactionRelationState FVar1;
  FactionActiveMask FVar2;
  FactionActiveMask activeFactionMask;
  uint extraout_EDX;
  bool bVar3;
  
  FVar1 = GameFactionRuntime_GetPackedStateNibble(sourceFactionIndex,targetFactionIndex);
  if (((((FVar1 != 2) && (FVar1 != 5)) && (FVar1 != 9)) &&
      ((g_GameFactionRuntimeImage.tail.relationUiFlags & 4) == 0)) &&
     ((FVar1 < 4 ||
      (((g_GameFactionRuntimeImage.tail.relationUiFlags & 2) == 0 &&
       ((FVar1 < 8 || ((g_GameFactionRuntimeImage.tail.relationUiFlags & 1) == 0)))))))) {
    if (3 < FVar1) {
      return;
    }
    GameFactionRelations_BuildEligibleFactionMask(targetFactionIndex);
    FVar2 = GameFactionRelations_BuildEligibleFactionMask(sourceFactionIndex);
    bVar3 = false;
    GameFactionRelations_EvaluateTransitionRulesCf(targetFactionIndex,FVar2 | extraout_EDX);
    if ((!bVar3) &&
       (GameFactionRelations_EvaluateTransitionRulesCf(sourceFactionIndex,activeFactionMask), !bVar3
       )) {
      return;
    }
  }
  return;
}

/* Address: 0x0053C090.
   Ownership: gameplay/faction/relations.
   Purpose: Builds an eight-bit mask containing active factions that are either the queried faction itself or have
   a packed pairwise relation state below four.
   Cross-module calls: GameFactionRuntime_GetPackedStateNibble [gameplay/faction/runtime].
*/
FactionActiveMask
GameFactionRelations_BuildEligibleFactionMask(FactionRuntimeIndex sourceFactionIndex)

{
  dword relationStateNibble;
  int factionIndex;
  int extraout_ECX;
  int extraout_EDX;
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
        factionIndex = extraout_ECX;
        sourceFactionIndex = extraout_EDX;
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
void GameFactionRelations_EvaluateTransitionRulesCf
               (FactionRuntimeIndex focalFactionIndex,FactionActiveMask activeFactionMask)

{
  byte bVar1;
  InGameConditionRuntime *pIVar2;
  uint uVar3;
  int iVar4;
  uint uVar5;
  byte *pbVar6;
  byte bVar7;
  byte *pbVar8;
  
  pIVar2 = g_InGameConditionRuntime;
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
    pbVar8 = g_InGameConditionRuntime[4].reserved00_4F;
    do {
      pbVar8 = pbVar8 + 0x10;
      uVar3 = *(uint *)pbVar8;
      *(uint *)pbVar8 = *(uint *)pbVar8 & 0xfffffffe;
                    
      switch(uVar3 & 0xfe) {
      case 2:
        if ((activeFactionMask & 1 << ((byte)*(uint *)(pbVar8 + 4) & 0x1f)) == 0) {
          *(uint *)pbVar8 = *(uint *)pbVar8 | 1;
        }
        break;
      case 4:
        if ((activeFactionMask & 1 << ((byte)*(uint *)(pbVar8 + 4) & 0x1f)) == 0) {
          *(uint *)pbVar8 = *(uint *)pbVar8 | 1;
        }
        break;
      case 6:
        if ((activeFactionMask & 1 << ((byte)*(uint *)(pbVar8 + 4) & 0x1f)) == 0) {
          *(uint *)pbVar8 = *(uint *)pbVar8 | 1;
        }
        break;
      case 8:
        *(uint *)pbVar8 = *(uint *)pbVar8 | 1;
        break;
      case 0x10:
        if ((activeFactionMask & 1 << ((byte)*(uint *)(pbVar8 + 4) & 0x1f)) != 0) {
          *(uint *)pbVar8 = *(uint *)pbVar8 | 1;
        }
        break;
      case 0x12:
        *(uint *)pbVar8 = *(uint *)pbVar8 | 1;
        break;
      case 0x14:
        if (*(uint *)(pbVar8 + 8) == 0) {
          *(uint *)pbVar8 = *(uint *)pbVar8 | 1;
        }
        break;
      case 0x16:
        *(uint *)pbVar8 = *(uint *)pbVar8 | 1;
        break;
      case 0x18:
        if ((activeFactionMask & 1 << ((byte)*(uint *)(pbVar8 + 4) & 0x1f)) == 0) {
          *(uint *)pbVar8 = *(uint *)pbVar8 | 1;
        }
        break;
      case 0x1a:
        pbVar6 = pbVar8 + 1;
        uVar3 = 0;
        while( true ) {
          while( true ) {
            while( true ) {
              while( true ) {
                bVar1 = *pbVar6;
                pbVar6 = pbVar6 + 1;
                if (bVar1 != 0xff) break;
                uVar3 = uVar3 >> 1 | uVar3 & 1;
              }
              if (bVar1 != 0xfe) break;
              uVar3 = uVar3 >> 1 & (uVar3 | 0xfffffffe);
            }
            if (bVar1 != 0xfd) break;
            uVar3 = uVar3 ^ 1;
          }
          if (bVar1 == 0xfc) break;
          uVar3 = (*(uint *)(pIVar2[4].reserved00_4F + (uint)bVar1 * 0x10 + 0x10) & 1) + uVar3 * 2;
        }
        *(uint *)pbVar8 = *(uint *)pbVar8 | uVar3 & 1;
      }
      iVar4 = iVar4 + -1;
    } while (iVar4 != 0);
    pbVar8 = pIVar2[8].reserved5C_AB + 0x44;
    iVar4 = 0x10;
    do {
      if ((*pbVar8 == 1) &&
         ((*(uint *)(pIVar2[4].reserved00_4F + (uint)pbVar8[6] * 0x10 + 0x10) & 1) != 0)) {
        bVar1 = pbVar8[4];
        if (g_GameFactionRuntimeImage.tail.factionLifecycleStates[bVar1] ==
            FACTION_RUNTIME_LIFECYCLE_ACTIVE) {
          bVar7 = pbVar8[1];
          if ((focalFactionIndex != (uint)bVar1) && ((activeFactionMask & 1 << (bVar1 & 0x1f)) == 0)
             ) {
            bVar7 = bVar7 ^ 1;
          }
          if (bVar7 == 0) {
            return;
          }
          return;
        }
      }
      pbVar8 = pbVar8 + 8;
      iVar4 = iVar4 + -1;
    } while (iVar4 != 0);
  }
  return;
}

/* Address: 0x0053C490.
   Ownership: gameplay/faction/relations.
   Purpose: Tests whether a faction pair is in one of the verified relation states 3, 6, or 10 and returns the
   result through carry.
   Cross-module calls: GameFactionRuntime_GetPackedStateNibble [gameplay/faction/runtime].
*/
undefined4
GameFactionRelations_IsResetEligibleStateCf
          (FactionRuntimeIndex sourceFactionIndex,FactionRuntimeIndex targetFactionIndex)

{
  undefined4 in_EAX;
  dword relationStateNibble;
  
  relationStateNibble =
       GameFactionRuntime_GetPackedStateNibble(sourceFactionIndex,targetFactionIndex);
  if (((relationStateNibble != 3) && (relationStateNibble != 6)) && (relationStateNibble != 10)) {
    return in_EAX;
  }
  return in_EAX;
}

/* Address: 0x0053C4D0.
   Ownership: gameplay/faction/relations.
   Purpose: Uses the current pair pressure and a random threshold to invoke pairwise relation advancement through
   the lower-probability 0x180/0x400 path. It is distinct from FrontendPlayerIndex_V306, PlayerRuntimeId, active-
   faction masks or codes, and PCK-backed ArmyAssetId, ModelDefinitionId, and TechnologyId domains.
   Cross-module calls: GameFactionRuntime_AdvancePairwiseRelationState [gameplay/faction/runtime].
*/
void GameFactionRelations_MaybeAdvancePairStateRare
               (FactionRuntimeIndex sourceFactionIndex,FactionRuntimeIndex targetFactionIndex)

{
  uint uVar1;
  dword dVar2;
  uint uVar3;
  int extraout_ECX;
  int extraout_EDX;
  
  dVar2 = (*g_RandomGeneratorState.next)();
  uVar1 = *(uint *)((int)g_GameDataAuxState.pairPressureMatrix8x8 + extraout_ECX * 4 + extraout_EDX)
  ;
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
void GameFactionRelations_MaybeAdvancePairStateCommon
               (FactionRuntimeIndex sourceFactionIndex,FactionRuntimeIndex targetFactionIndex)

{
  uint uVar1;
  dword dVar2;
  uint uVar3;
  int extraout_ECX;
  int extraout_EDX;
  
  dVar2 = (*g_RandomGeneratorState.next)();
  uVar1 = *(uint *)((int)g_GameDataAuxState.pairPressureMatrix8x8 + extraout_ECX * 4 + extraout_EDX)
  ;
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
undefined4
GameFactionRelations_MaybeResetPairState
          (FactionRuntimeIndex sourceFactionIndex,FactionRuntimeIndex targetFactionIndex)

{
  undefined4 in_EAX;
  dword dVar1;
  
  dVar1 = (*g_RandomGeneratorState.next)();
  if ((dVar1 & 0x180) == 0x80) {
    GameFactionRuntime_ResetPairwiseRelationState
              (0xffffffff,0,sourceFactionIndex,targetFactionIndex);
  }
  return in_EAX;
}

/* Address: 0x00560EB0.
   Ownership: gameplay/faction/relations.
   Purpose: Inserts one unique two-dword pair into the per-player pair list and increments the local-player
   visible-count field. Kept distinct from frontend slot indices, faction runtime indices, network endpoint
   identity, and PCK asset identifiers. Typed parameters: p2 playerRuntimeId→PlayerRuntimeId. Calling convention,
   storage, body bytes, control flow, and executable data remain unchanged. Typed parameters: p4
   pairValue→SelectionPlayerPairValue.
*/
undefined8
PlayerPairList_InsertUnique
          (PlayerRuntimeId playerRuntimeId,undefined4 param_2,SelectionPlayerPairValue pairValue,
          SelectionPlayerPairValue pairKey)

{
  undefined4 in_EAX;
  undefined4 in_EDX;
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
      if ((pairKey == pairRecordCursor->pairKey) && (pairValue == pairRecordCursor->pairValue))
      goto PlayerPairList_InsertUnique_ReturnAfterDuplicateCapacityOrAppendDecision;
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
PlayerPairList_InsertUnique_ReturnAfterDuplicateCapacityOrAppendDecision:
  return CONCAT44(in_EDX,in_EAX);
}

/* Address: 0x00560F50.
   Ownership: gameplay/faction/relations.
   Purpose: Removes the first matching two-dword pair from the per-player pair list and decrements the local-player
   visible-count field. Kept distinct from frontend slot indices, faction runtime indices, network endpoint
   identity, and PCK asset identifiers. Typed parameters: p2 playerRuntimeId→PlayerRuntimeId. Calling convention,
   storage, body bytes, control flow, and executable data remain unchanged. Typed parameters: p4
   pairValue→SelectionPlayerPairValue.
*/
undefined8
PlayerPairList_RemoveFirstMatch
          (PlayerRuntimeId playerRuntimeId,undefined4 param_2,SelectionPlayerPairValue pairValue,
          SelectionPlayerPairValue pairKey)

{
  undefined4 in_EAX;
  int trailingDwordsToMove;
  undefined4 in_EDX;
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
        return CONCAT44(in_EDX,in_EAX);
      }
      pairRecordCursor = pairRecordCursor + 1;
    }
  }
  return CONCAT44(in_EDX,in_EAX);
}

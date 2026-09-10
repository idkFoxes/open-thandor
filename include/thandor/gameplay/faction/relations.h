#ifndef THANDOR_GAMEPLAY_FACTION_RELATIONS_H
#define THANDOR_GAMEPLAY_FACTION_RELATIONS_H

#include <thandor/generated/v523_types.h>
#include <thandor/core/contracts.h>

/* Submodule: gameplay/faction/relations. */
/* Functions are grouped by semantic ownership; address comments are executable virtual addresses. */

/* 0x0053C010 */
AiPreservedFactionIndexEdxResult GameFactionRelations_UpdateAllPairsForFaction (FactionRuntimeIndex sourceFactionIndex,WorldRuntimeContext *worldRuntime);

/* 0x00560E30 */
undefined8 PlayerPairList_InsertRange (PlayerRuntimeId playerRuntimeId,SelectionPlayerPairValue endKey, SelectionPlayerPairValue pairValue,SelectionPlayerPairValue startKey);

/* 0x00560E70 */
undefined8 PlayerPairList_RemoveRange (PlayerRuntimeId playerRuntimeId,SelectionPlayerPairValue endKey, SelectionPlayerPairValue pairValue,SelectionPlayerPairValue startKey);

/* 0x0053C3D0 */
void GameFactionRelations_TestPairTransitionAllowedCf (FactionRuntimeIndex sourceFactionIndex,FactionRuntimeIndex targetFactionIndex);

/* 0x0053C090 */
FactionActiveMask GameFactionRelations_BuildEligibleFactionMask(FactionRuntimeIndex sourceFactionIndex);

/* 0x0053C0F0 */
void GameFactionRelations_EvaluateTransitionRulesCf (FactionRuntimeIndex focalFactionIndex,FactionActiveMask activeFactionMask);

/* 0x0053C490 */
undefined4 GameFactionRelations_IsResetEligibleStateCf (FactionRuntimeIndex sourceFactionIndex,FactionRuntimeIndex targetFactionIndex);

/* 0x0053C4D0 */
void GameFactionRelations_MaybeAdvancePairStateRare (FactionRuntimeIndex sourceFactionIndex,FactionRuntimeIndex targetFactionIndex);

/* 0x0053C540 */
void GameFactionRelations_MaybeAdvancePairStateCommon (FactionRuntimeIndex sourceFactionIndex,FactionRuntimeIndex targetFactionIndex);

/* 0x0053C5B0 */
undefined4 GameFactionRelations_MaybeResetPairState (FactionRuntimeIndex sourceFactionIndex,FactionRuntimeIndex targetFactionIndex);

/* 0x00560EB0 */
undefined8 PlayerPairList_InsertUnique (PlayerRuntimeId playerRuntimeId,undefined4 param_2,SelectionPlayerPairValue pairValue, SelectionPlayerPairValue pairKey);

/* 0x00560F50 */
undefined8 PlayerPairList_RemoveFirstMatch (PlayerRuntimeId playerRuntimeId,undefined4 param_2,SelectionPlayerPairValue pairValue, SelectionPlayerPairValue pairKey);

#endif /* THANDOR_GAMEPLAY_FACTION_RELATIONS_H */

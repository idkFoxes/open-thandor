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
/* Functions are grouped by semantic ownership; address comments are executable virtual addresses. */

/* 0x0053C010 */
void __thandor_void_preserve_eax_ecx_edx
GameFactionRelations_UpdateAllPairsForFaction
          (FactionRuntimeIndex sourceFactionIndex,WorldRuntimeContext *worldRuntime);

/* 0x00560E30 */
void __thandor_void_preserve_eax_ecx_edx
PlayerPairList_InsertRange
          (PlayerRuntimeId playerRuntimeId,SelectionPlayerPairValue endKey,
          SelectionPlayerPairValue pairValue,SelectionPlayerPairValue startKey);

/* 0x00560E70 */
void __thandor_void_preserve_eax_ecx_edx
PlayerPairList_RemoveRange
          (PlayerRuntimeId playerRuntimeId,SelectionPlayerPairValue endKey,
          SelectionPlayerPairValue pairValue,SelectionPlayerPairValue startKey);

/* 0x0053C3D0 */
bool __thandor_cf_preserve_eax_ecx_edx
GameFactionRelations_TestPairTransitionAllowedCf
          (FactionRuntimeIndex sourceFactionIndex,FactionRuntimeIndex targetFactionIndex);

/* 0x0053C090 */
FactionActiveMask __thandor_eax_preserve_ecx_edx
GameFactionRelations_BuildEligibleFactionMask(FactionRuntimeIndex sourceFactionIndex);

/* 0x0053C0F0 */
bool __thandor_cf_preserve_eax_ecx_edx
GameFactionRelations_EvaluateTransitionRulesCf
          (FactionRuntimeIndex focalFactionIndex,FactionActiveMask activeFactionMask);

/* 0x0053C490 */
bool __thandor_cf_preserve_eax_ecx_edx
GameFactionRelations_IsResetEligibleStateCf
          (FactionRuntimeIndex sourceFactionIndex,FactionRuntimeIndex targetFactionIndex);

/* 0x0053C4D0 */
void __thandor_void_preserve_eax_ecx_edx
GameFactionRelations_MaybeAdvancePairStateRare
          (FactionRuntimeIndex sourceFactionIndex,FactionRuntimeIndex targetFactionIndex);

/* 0x0053C540 */
void __thandor_void_preserve_eax_ecx_edx
GameFactionRelations_MaybeAdvancePairStateCommon
          (FactionRuntimeIndex sourceFactionIndex,FactionRuntimeIndex targetFactionIndex);

/* 0x0053C5B0 */
void __thandor_void_preserve_eax_ecx_edx
GameFactionRelations_MaybeResetPairState
          (FactionRuntimeIndex sourceFactionIndex,FactionRuntimeIndex targetFactionIndex);

/* 0x00560EB0 */
void __thandor_void_preserve_eax_ecx_edx
PlayerPairList_InsertUnique
          (PlayerRuntimeId playerRuntimeId,dword reservedZero,SelectionPlayerPairValue pairValue,
          SelectionPlayerPairValue pairKey);

/* 0x00560F50 */
void __thandor_void_preserve_eax_ecx_edx
PlayerPairList_RemoveFirstMatch
          (PlayerRuntimeId playerRuntimeId,dword reservedZero,SelectionPlayerPairValue pairValue,
          SelectionPlayerPairValue pairKey);

#endif /* THANDOR_GAMEPLAY_FACTION_RELATIONS_H */

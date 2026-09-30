/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/include/thandor/gameplay/ai/combat.h
 * Reverse engineering by idkFoxes 2026
 */

#ifndef THANDOR_GAMEPLAY_AI_COMBAT_H
#define THANDOR_GAMEPLAY_AI_COMBAT_H

#include <thandor/generated/types.h>
#include <thandor/core/contracts.h>

/* Submodule: gameplay/ai/combat. */

/* AiUnitGroup_AssignCollectedEntitiesToBestTarget: the collected armies attack once the sum of their hierarchy
   scale ratios (256 = one intact army) reaches two full armies. */
#define AI_UNIT_GROUP_ATTACK_STRENGTH 0x200
/* Functions are grouped by semantic ownership; address comments are executable virtual addresses. */

/* 0x00536FC0 */
void AiCombatDecision_UpdateTargetAssignment(WorldRuntimeContext *worldRuntime,ArmyRuntimeSlot *armyRuntime);

/* 0x0053B8B0 */
void __fastcall AiUnitGroup_AssignCollectedEntitiesToBestTarget(void);

/* 0x005372C0 */
ArmyRuntimeSlot *AiCombatTarget_SelectBestCandidate
          (WorldRuntimeContext *worldRuntime,ArmyRuntimeSlot *sourceArmyRuntime,
          AiSourceClassCount *outSourceClassCount);

/* 0x00537060 */
AiCandidateScore32 AiCombatTarget_EvaluateCandidateScore
          (AiCandidateScore32 currentBestScore,AiSourceClassCount sourceClassCount,
          DepthBinMask32 sourceDepthMask0,DepthBinMask32 sourceDepthMask1,
          ArmyRuntimeSlot *candidateArmyRuntime,ArmyRuntimeSlot *sourceArmyRuntime);

#endif /* THANDOR_GAMEPLAY_AI_COMBAT_H */

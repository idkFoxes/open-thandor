/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/include/thandor/gameplay/ai/combat.h
 * Reverse engineering by idkFoxes 2026
 */

#ifndef THANDOR_GAMEPLAY_AI_COMBAT_H
#define THANDOR_GAMEPLAY_AI_COMBAT_H

#include <thandor/gameplay/ai/types.h>
#include <thandor/gameplay/army/types.h>
#include <thandor/graphics/render/types.h>
#include <thandor/ui/ingame/types.h>
#include <thandor/core/contracts.h>

/* AiUnitGroup_AssignCollectedEntitiesToBestTarget: the collected armies attack once the sum of their hierarchy
   scale ratios (256 = one intact army) reaches two full armies. */
#define AI_UNIT_GROUP_ATTACK_STRENGTH 0x200

void AiCombatDecision_UpdateTargetAssignment(WorldRuntimeContext *worldRuntime,ArmyRuntimeSlot *armyRuntime);

void AiUnitGroup_AssignCollectedEntitiesToBestTarget();

ArmyRuntimeSlot *AiCombatTarget_SelectBestCandidate
          (WorldRuntimeContext *worldRuntime,ArmyRuntimeSlot *sourceArmyRuntime,
          AiSourceClassCount *outSourceClassCount);

AiCandidateScore32 AiCombatTarget_EvaluateCandidateScore
          (AiCandidateScore32 currentBestScore,AiSourceClassCount sourceClassCount,
          DepthBinMask32 sourceDepthMask0,DepthBinMask32 sourceDepthMask1,
          ArmyRuntimeSlot *candidateArmyRuntime,ArmyRuntimeSlot *sourceArmyRuntime);

#endif /* THANDOR_GAMEPLAY_AI_COMBAT_H */

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
/* Functions are grouped by semantic ownership; address comments are executable virtual addresses. */

/* 0x00536FC0 */
void __thandor_void_preserve_eax_ecx_edx
AiCombatDecision_UpdateTargetAssignment
          (WorldRuntimeContext *worldRuntime,ArmyRuntimeSlot *armyRuntime);

/* 0x0053B8B0 */
void __fastcall AiUnitGroup_AssignCollectedEntitiesToBestTarget(void);

/* 0x005372C0 */
AiCombatTargetSelectionResult
AiCombatTarget_SelectBestCandidate
          (WorldRuntimeContext *worldRuntime,ArmyRuntimeSlot *sourceArmyRuntime);

/* 0x00537060 */
AiCandidateScore32 __thandor_eax_preserve_ecx_edx
AiCombatTarget_EvaluateCandidateScore
          (AiCandidateScore32 currentBestScore,AiSourceClassCount sourceClassCount,
          DepthBinMask32 sourceDepthMask0,DepthBinMask32 sourceDepthMask1,
          ArmyRuntimeSlot *candidateArmyRuntime,ArmyRuntimeSlot *sourceArmyRuntime);

#endif /* THANDOR_GAMEPLAY_AI_COMBAT_H */

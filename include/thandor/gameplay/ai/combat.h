#ifndef THANDOR_GAMEPLAY_AI_COMBAT_H
#define THANDOR_GAMEPLAY_AI_COMBAT_H

#include <thandor/generated/v523_types.h>
#include <thandor/core/contracts.h>

/* Submodule: gameplay/ai/combat. */
/* Functions are grouped by semantic ownership; address comments are executable virtual addresses. */

/* 0x00536FC0 */
void AiCombatDecision_UpdateTargetAssignment (WorldRuntimeContext *worldRuntime,ArmyRuntimeSlot *armyRuntime);

/* 0x0053B8B0 */
void __fastcall AiUnitGroup_AssignCollectedEntitiesToBestTarget(void);

/* 0x005372C0 */
AiCombatTargetSelectionResult64 AiCombatTarget_SelectBestCandidate (WorldRuntimeContext *worldRuntime,ArmyRuntimeSlot *sourceArmyRuntime);

/* 0x00537060 */
AiCandidateScore32 AiCombatTarget_EvaluateCandidateScore (AiCandidateScore32 currentBestScore,AiSourceClassCount sourceClassCount, DepthBinMask32 sourceDepthMask0,DepthBinMask32 sourceDepthMask1, ArmyRuntimeSlot *candidateArmyRuntime,ArmyRuntimeSlot *sourceArmyRuntime);

#endif /* THANDOR_GAMEPLAY_AI_COMBAT_H */

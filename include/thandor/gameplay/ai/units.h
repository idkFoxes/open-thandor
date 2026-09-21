/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/include/thandor/gameplay/ai/units.h
 * Reverse engineering by idkFoxes 2026
 */

#ifndef THANDOR_GAMEPLAY_AI_UNITS_H
#define THANDOR_GAMEPLAY_AI_UNITS_H

#include <thandor/generated/types.h>
#include <thandor/core/contracts.h>

/* Submodule: gameplay/ai/units. */
/* Functions are grouped by semantic ownership; address comments are executable virtual addresses. */

/* 0x0053B0E0 */
void __thandor_void_preserve_eax_ecx_edx
AiUnitBehavior_UpdateWorkspace01Entities
          (FactionRuntimeIndex factionIndex,WorldRuntimeContext *worldRuntime);

/* 0x0053B4C0 */
void __thandor_void_preserve_eax_ecx_edx
AiUnitBehavior_SelectBestAnchorAction
          (MdlDefinitionSemanticPrefix80 *modelDefinition,ArmyRuntimeSlot *armyRuntimeSlot,
          FactionRuntimeIndex factionIndex,WorldRuntimeContext *worldRuntime);

/* 0x0053B1D0 */
AiWorkspace05DistanceSelectionRegs8 __thandor_eax_ebx_cf_preserve_ecx_edx
AiUnitBehavior_ComputeWorkspace05DistanceScore
          (AiCandidateScore32 currentBestScore,MdlDefinitionSemanticPrefix80 *modelDefinition,
          ArmyRuntimeSlot *armyRuntimeSlot);

/* 0x0053B260 */
AiCandidateScore32 __thandor_eax_preserve_ecx_edx
AiUnitBehavior_ComputeFactionAnchorDistanceScore
          (FactionRuntimeIndex factionIndex,AiCandidateScore32 currentBestScore,
          MdlDefinitionSemanticPrefix80 *modelDefinition,ArmyRuntimeSlot *armyRuntimeSlot);

/* 0x0053B330 */
AiSecondaryWorkspaceDistanceSelectionRegs8 __thandor_eax_ebx_cf_preserve_ecx_edx
AiUnitBehavior_ComputeSecondaryWorkspaceDistanceScore
          (AiCandidateScore32 currentBestScore,MdlDefinitionSemanticPrefix80 *modelDefinition,
          ArmyRuntimeSlot *armyRuntimeSlot);

/* 0x0053B3E0 */
void __thandor_preserve_eax
AiUnitCommand_AssignWorkspacePoint
          (dword *workspacePoint,ArmyRuntimeSlot *armyRuntime,
          WorldRuntimeContext *worldRuntimeContext);

/* 0x0053B420 */
void __thandor_preserve_eax_edx
AiUnitCommand_AssignFactionAnchorPoint
          (FactionRuntimeIndex factionIndex,ArmyRuntimeSlot *armyRuntime,
          WorldRuntimeContext *worldRuntimeContext);

/* 0x0053B480 */
void __thandor_void_preserve_eax_ecx_edx
AiUnitBehavior_CollectUnassignedEntity
          (ArmyRuntimeSlot *armyRuntimeSlot,WorldRuntimeContext *worldRuntimeContext);

/* 0x0053B620 */
void __thandor_void_preserve_eax_ecx_edx
AiUnitBehavior_UpdateSpecialClass12Entity
          (MdlDefinitionSemanticPrefix80 *modelDefinition,ArmyRuntimeSlot *armyRuntime,
          FactionRuntimeIndex factionIndex,WorldRuntimeContext *worldRuntime);

#endif /* THANDOR_GAMEPLAY_AI_UNITS_H */

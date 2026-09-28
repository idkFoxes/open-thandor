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

/* ArmyRuntimeSlot.runtimeState8C value the AI stores whenever it gives a unit a command (units.c, combat.c) */
#define AI_UNIT_COMMANDED_STATE 8
/* ArmyRuntimeSlot.runtimeState94 bit 0: set when AiUnitGroup_AssignCollectedEntitiesToBestTarget sends the unit
   to a group target, cleared by the direct AI move commands; AiUnitBehavior_CollectUnassignedEntity skips it */
#define AI_UNIT_STATE94_GROUP_ASSIGNED 0x1
/* Functions are grouped by semantic ownership; address comments are executable virtual addresses. */

/* 0x0053B0E0 */
void AiUnitBehavior_UpdateWorkspace01Entities(FactionRuntimeIndex factionIndex,WorldRuntimeContext *worldRuntime);

/* 0x0053B4C0 */
void AiUnitBehavior_SelectBestAnchorAction
          (MdlDefinitionSemanticPrefix80 *modelDefinition,ArmyRuntimeSlot *armyRuntimeSlot,
          FactionRuntimeIndex factionIndex,WorldRuntimeContext *worldRuntime);

/* 0x0053B1D0 */
AiWorkspace05DistanceSelectionRegs8 AiUnitBehavior_ComputeWorkspace05DistanceScore
          (AiCandidateScore32 currentBestScore,MdlDefinitionSemanticPrefix80 *modelDefinition,
          ArmyRuntimeSlot *armyRuntimeSlot);

/* 0x0053B260 */
AiCandidateScore32 AiUnitBehavior_ComputeFactionAnchorDistanceScore
          (FactionRuntimeIndex factionIndex,AiCandidateScore32 currentBestScore,
          MdlDefinitionSemanticPrefix80 *modelDefinition,ArmyRuntimeSlot *armyRuntimeSlot);

/* 0x0053B330 */
AiSecondaryWorkspaceDistanceSelectionRegs8 AiUnitBehavior_ComputeSecondaryWorkspaceDistanceScore
          (AiCandidateScore32 currentBestScore,MdlDefinitionSemanticPrefix80 *modelDefinition,
          ArmyRuntimeSlot *armyRuntimeSlot);

/* 0x0053B3E0 */
void AiUnitCommand_AssignWorkspacePoint(uint32_t *workspacePoint,ArmyRuntimeSlot *armyRuntime,
          WorldRuntimeContext *worldRuntimeContext);

/* 0x0053B420 */
void AiUnitCommand_AssignFactionAnchorPoint(FactionRuntimeIndex factionIndex,ArmyRuntimeSlot *armyRuntime,
          WorldRuntimeContext *worldRuntimeContext);

/* 0x0053B480 */
void AiUnitBehavior_CollectUnassignedEntity(ArmyRuntimeSlot *armyRuntimeSlot,WorldRuntimeContext *worldRuntimeContext);

/* 0x0053B620 */
void AiUnitBehavior_UpdateSpecialClass12Entity
          (MdlDefinitionSemanticPrefix80 *modelDefinition,ArmyRuntimeSlot *armyRuntime,
          FactionRuntimeIndex factionIndex,WorldRuntimeContext *worldRuntime);

#endif /* THANDOR_GAMEPLAY_AI_UNITS_H */

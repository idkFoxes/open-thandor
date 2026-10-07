/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/include/thandor/gameplay/ai/units.h
 * Reverse engineering by idkFoxes 2026
 */

#ifndef THANDOR_GAMEPLAY_AI_UNITS_H
#define THANDOR_GAMEPLAY_AI_UNITS_H

#include <thandor/core/types.h>
#include <thandor/gameplay/ai/types.h>
#include <thandor/gameplay/army/types.h>
#include <thandor/ui/ingame/types.h>
#include <thandor/core/contracts.h>

/* ArmyRuntimeSlot.aiUnitState value the AI stores whenever it gives a unit a command (units.cpp, combat.cpp) */
inline constexpr int AI_UNIT_COMMANDED_STATE = 8;
/* ArmyRuntimeSlot.aiUnitFlags bit 0: set when AiUnitGroup_AssignCollectedEntitiesToBestTarget sends the unit
   to a group target, cleared by the direct AI move commands; AiUnitBehavior_CollectUnassignedEntity skips it */
inline constexpr int AI_UNIT_STATE94_GROUP_ASSIGNED = 0x1;

void AiUnitBehavior_UpdateOwnUnits(FactionRuntimeIndex factionIndex,WorldRuntimeContext *worldRuntime);

void AiUnitBehavior_SelectBestAnchorAction
          (MdlDefinitionSemanticPrefix *modelDefinition,ArmyRuntimeSlot *armyRuntimeSlot,
          FactionRuntimeIndex factionIndex,WorldRuntimeContext *worldRuntime);

AiGeneralSiteDistanceSelection AiUnitBehavior_ComputeGeneralSiteDistanceScore
          (AiCandidateScore32 currentBestScore,MdlDefinitionSemanticPrefix *modelDefinition,
          ArmyRuntimeSlot *armyRuntimeSlot);

AiCandidateScore32 AiUnitBehavior_ComputeFactionAnchorDistanceScore
          (FactionRuntimeIndex factionIndex,AiCandidateScore32 currentBestScore,
          MdlDefinitionSemanticPrefix *modelDefinition,ArmyRuntimeSlot *armyRuntimeSlot);

AiSecondaryWorkspaceDistanceSelection AiUnitBehavior_ComputeSecondaryWorkspaceDistanceScore
          (AiCandidateScore32 currentBestScore,MdlDefinitionSemanticPrefix *modelDefinition,
          ArmyRuntimeSlot *armyRuntimeSlot);

void AiUnitCommand_AssignWorkspacePoint(AiScoredSiteWorkspaceEntry *workspacePoint,ArmyRuntimeSlot *armyRuntime,
          WorldRuntimeContext *worldRuntimeContext);

void AiUnitCommand_AssignFactionAnchorPoint(FactionRuntimeIndex factionIndex,ArmyRuntimeSlot *armyRuntime,
          WorldRuntimeContext *worldRuntimeContext);

void AiUnitBehavior_CollectUnassignedEntity(ArmyRuntimeSlot *armyRuntimeSlot,WorldRuntimeContext *worldRuntimeContext);

void AiUnitBehavior_UpdatePioneerVehicle
          (MdlDefinitionSemanticPrefix *modelDefinition,ArmyRuntimeSlot *armyRuntime,
          FactionRuntimeIndex factionIndex,WorldRuntimeContext *worldRuntime);

extern uint32_t g_AiCollectedEntityCount;

#endif /* THANDOR_GAMEPLAY_AI_UNITS_H */

/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/include/thandor/gameplay/ai/planning.h
 * Reverse engineering by idkFoxes 2026
 */

#ifndef THANDOR_GAMEPLAY_AI_PLANNING_H
#define THANDOR_GAMEPLAY_AI_PLANNING_H

#include <thandor/core/types.h>
#include <thandor/gameplay/ai/types.h>
#include <thandor/ui/ingame/types.h>
#include <thandor/core/contracts.h>

/* g_AiActiveGridMaskClasses[0..3] (AiPlanning_CollectActiveGridMaskClasses): an unused slot */
#define AI_GRID_MASK_CLASS_FREE 0xffffffff
/* Random terms of the strategic building choice: 14 bits of g_RandomGeneratorState.next added to each
   terrain score as a tie breaker (AiStrategicClass_SelectTerrainSuitedBuilding), and 7 bits added to each
   pressure score as jitter (AiStrategicClass_SelectPressureWeightedBuilding) */
#define AI_STRATEGIC_TIE_BREAK_MASK 0x3fff
#define AI_STRATEGIC_SCORE_JITTER_MASK 0x7f

void AiFactionRuntime_RebuildPlanningCapacityState();

void AiPlanning_CollectActiveGridMaskClasses();

void AiRuntime_DispatchFactionPlanningPhase(FactionRuntimeIndex factionIndex,InGameRuntimeRoot *inGameRoot);

void AiStrategicClass_AddArmsFactoriesStageBuilding(FactionRuntimeIndex factionIndex,WorldRuntimeContext *worldRuntime);

void AiStrategicClass_AddWeightedClassCandidate(FactionRuntimeIndex factionIndex,WorldRuntimeContext *worldRuntime);

void AiFactionPlanning_UpdateActiveEntityPressureFlag(FactionRuntimeIndex factionIndex);

AiStrategicClassSelection AiStrategicClass_SelectTerrainSuitedBuilding
          (FactionRuntimeIndex factionIndex,WorldRuntimeContext *worldRuntime);

AiStrategicClassSelection AiStrategicClass_SelectPressureWeightedBuilding
          (FactionRuntimeIndex factionIndex,WorldRuntimeContext *worldRuntime);

Bool8 AiFactionRuntime_TestPlanningCapacityExceeded(uint32_t additionalEnergyDemand,FactionRuntimeIndex factionIndex);

extern uint32_t g_AiActiveGridMaskClasses[4];

#endif /* THANDOR_GAMEPLAY_AI_PLANNING_H */

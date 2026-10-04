/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/include/thandor/world/model/gameplay_rules.h
 * Reverse engineering by idkFoxes 2026
 */

#ifndef THANDOR_WORLD_MODEL_GAMEPLAY_RULES_H
#define THANDOR_WORLD_MODEL_GAMEPLAY_RULES_H

#include <thandor/generated/types.h>
#include <thandor/core/contracts.h>

/* Aim tolerance of ModelNodeRuntime_SmoothYaw/PitchTowardTarget: outsideTolerance beyond +-0x3FF */
#define MODEL_AIM_TOLERANCE_ANGLE16 0x3ff
/* ModelDefinition.variantModelDefinitionIds[] entries (ModelRuntimeHierarchy_ApplyFactionTechnologyVariantsRecursive) */
#define MODEL_TECHNOLOGY_VARIANT_COUNT 6

void ModelRuntimeHierarchy_ApplyFactionTechnologyVariants
          (FactionRuntimeIndex factionIndex,ArmyRuntimeSlot *armyRuntime);

void ModelRuntimeHierarchy_MarkDestroyedRecursive(WorldRuntimeContext *contextArg,ArmyRuntimeSlot *armyRuntime);

int ModelRuntimeHierarchy_SumArmour(int *modelRuntimeRoot);

void ModelRuntimeHierarchy_AccumulateDerivedSelectionMetrics(int *modelRuntime);

Q12 ModelRuntimeHierarchy_ComputeConditionRatioQ12(ModelRuntimeSlot *modelRuntime);

ModelHierarchyEnergyDemand ModelRuntimeHierarchy_ComputeEnergyDemand(ModelRuntimeSlot *modelRuntime);

Bool8 ModelNodeRuntime_SmoothYawTowardTarget (ModelRuntimeNode *modelNodeRuntime,ModelRuntimeWeaponAimStateView *smoothingState, AngleTurn32 targetYawAngle16);

uint32_t ModelNodeRuntime_SmoothPitchTowardTarget (ModelRuntimeNode *modelNodeRuntime,ModelRuntimeWeaponAimStateView *smoothingState, AngleTurn32 targetPitchAngle16);

void ModelRuntimeHierarchy_ApplyFactionTechnologyVariantsRecursive(FactionRuntimeIndex factionIndex,int *modelRuntime);

Q12 ModelRuntime_QueryHierarchyConditionRatioQ12(RuntimeModelFactionPrefix *runtimeEntry);

int ModelRuntime_QueryActiveHierarchyMetric(ArmyRuntimeSlot *armyRuntime);

ModelHierarchyEnergyDemand
ModelRuntime_QueryHierarchyEnergyDemand(RuntimeModelFactionPrefix *runtimeEntry);

#endif /* THANDOR_WORLD_MODEL_GAMEPLAY_RULES_H */

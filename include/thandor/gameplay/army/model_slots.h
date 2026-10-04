/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/include/thandor/gameplay/army/model_slots.h
 * Reverse engineering by idkFoxes 2026
 */

#ifndef THANDOR_GAMEPLAY_ARMY_MODEL_SLOTS_H
#define THANDOR_GAMEPLAY_ARMY_MODEL_SLOTS_H

#include <thandor/generated/types.h>
#include <thandor/core/contracts.h>

/* Submodule: gameplay/army/model_slots. */

/* ModelRuntimeSlotClassInit_InitializeSentinelBoundsAndTiming: value of the classLinkState words classState68..classState74 that
   hold no coordinate yet (INT32_MIN bit pattern, never a real world coordinate) */
#define MODEL_CLASS_STATE_UNSET_COORDINATE 0x80000000
/* Functions are grouped by semantic ownership. */

void ModelRuntimeSlotClassInit_ApplyDefinitionTextureAnimationIndices
          (ModelDefinition *modelDefinition,ModelRuntimeSlot *modelRuntimeSlot
          );

void ModelRuntimeSlotClassInit_InitializeSentinelBoundsAndTiming
          (ModelDefinitionRecordPrefix *modelDefinition,ModelRuntimeSlot *modelRuntimeSlot);

void ModelRuntimeSlotClassInit_SeedFieldsFromRootTransform
          (ModelDefinitionRecordPrefix *modelDefinition,ModelRuntimeSlot *modelRuntimeSlot);

void ModelRuntimeSlotClassInit_BuildModelKeyPresenceCounters
          (ModelDefinitionRecordPrefix *modelDefinition,ModelRuntimeSlot *modelRuntimeSlot);

void ModelRuntimeSlotClassInit_ResetStructureFactoryBuild (ModelDefinitionRecordPrefix *modelDefinition,ModelRuntimeSlot *modelRuntimeSlot);

void ModelRuntimeSlot_UnrebaseClassArmyLinkOffset6C(ModelRuntimeSlot *modelRuntime);

void ModelRuntimeSlot_RebaseClassArmyLinkOffset6C(ModelRuntimeSlot *modelRuntimeSlot);

void ModelRuntimeSlotClassInit_EnableRootAnimationAndCopyDefinitionC0
          (ModelDefinitionRecordPrefix *modelDefinition,ModelRuntimeSlot *modelRuntimeSlot);

void ModelRuntimeSlotClassInit_AccumulateFactionMetricAndDetachRootChild3
          (ModelDefinitionRecordPrefix *modelDefinition,ModelRuntimeSlot *modelRuntimeSlot);

void ModelRuntimeSlotClassInit_AccumulateFactionMetricAndDetachRootChild1
          (ModelDefinitionRecordPrefix *modelDefinition,ModelRuntimeSlot *modelRuntimeSlot);

void ModelRuntimeSlotClassInit_AddFactionEnergyGenerationCapacity
          (ModelDefinitionRecordPrefix *modelDefinition,ModelRuntimeSlot *modelRuntimeSlot);

void ModelRuntimeSlotClassRelease_SubtractFactionEnergyGenerationCapacity
          (ModelDefinitionRecordPrefix *modelDefinition,ModelRuntimeSlot *modelRuntime);

void ModelRuntimeSlot_UnrebaseClassModelLinkOffset60(ModelRuntimeSlot *modelRuntime);

void ModelRuntimeSlot_RebaseClassModelLinkOffset60(ModelRuntimeSlot *modelRuntimeSlot);

void ModelRuntimeSlotClassInit_ClearStateAndSetRootChild0Offset (ModelDefinitionRecordPrefix *modelDefinition,ModelRuntimeSlot *modelRuntimeSlot);

void ModelRuntimeSlotClassInit_ClearExtendedStateAndEnableRootAnimation
          (ModelDefinitionRecordPrefix *modelDefinition,ModelRuntimeSlot *modelRuntimeSlot);

void ModelRuntimeSlotPointerRebase_NoOp(ModelRuntimeSlot *modelRuntimeSlot);

void ModelRuntimeSlotClassInit_NoOp (ModelDefinitionRecordPrefix *modelDefinition,ModelRuntimeSlot *modelRuntimeSlot);

void ModelRuntimeSlotClassInit_ClearField60 (ModelDefinitionRecordPrefix *modelDefinition,ModelRuntimeSlot *modelRuntimeSlot);

void ModelRuntimeSlotClassInit_ClearFields60AndB8 (ModelDefinitionRecordPrefix *modelDefinition,ModelRuntimeSlot *modelRuntimeSlot);

#endif /* THANDOR_GAMEPLAY_ARMY_MODEL_SLOTS_H */

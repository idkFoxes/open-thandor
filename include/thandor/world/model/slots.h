#ifndef THANDOR_WORLD_MODEL_SLOTS_H
#define THANDOR_WORLD_MODEL_SLOTS_H

#include <thandor/generated/v523_types.h>
#include <thandor/core/contracts.h>

/* Submodule: world/model/slots. */
/* Functions are grouped by semantic ownership; address comments are executable virtual addresses. */

/* 0x005200C0 */
void ModelRuntimeSlotClassInit_ApplyDefinitionTextureAnimationIndices (ModelDefinitionRuntimeSemanticView280 *modelDefinition, ModelRuntimeSlot *modelRuntimeSlot);

/* 0x00522A90 */
void ModelRuntimeSlotClassInit_InitializeSentinelBoundsAndTiming (ModelDefinitionRecordPrefix *modelDefinition,ModelRuntimeSlot *modelRuntimeSlot);

/* 0x00522B90 */
void ModelRuntimeSlotClassInit_SeedFieldsFromRootTransform (ModelDefinitionRecordPrefix *modelDefinition,ModelRuntimeSlot *modelRuntimeSlot);

/* 0x00523CA0 */
void ModelRuntimeSlotClassInit_BuildModelKeyPresenceCounters (ModelDefinitionRecordPrefix *modelDefinition,ModelRuntimeSlot *modelRuntimeSlot);

/* 0x005243D0 */
void ModelRuntimeSlotClassInit_ClearFields64_68_74_B8 (ModelDefinitionRecordPrefix *modelDefinition,ModelRuntimeSlot *modelRuntimeSlot);

/* 0x00524CB0 */
void ModelRuntimeSlot_UnrebaseClassArmyLinkOffset6C(ModelRuntimeSlot *modelRuntime);

/* 0x00524CE0 */
void ModelRuntimeSlot_RebaseClassArmyLinkOffset6C(ModelRuntimeSlot *modelRuntimeSlot);

/* 0x00524D10 */
void ModelRuntimeSlotClassInit_EnableRootAnimationAndCopyDefinitionC0 (ModelDefinitionRecordPrefix *modelDefinition,ModelRuntimeSlot *modelRuntimeSlot);

/* 0x005251C0 */
void ModelRuntimeSlotClassInit_AccumulateFactionMetricAndDetachRootChild3 (ModelDefinitionRecordPrefix *modelDefinition,ModelRuntimeSlot *modelRuntimeSlot);

/* 0x00525250 */
void ModelRuntimeSlotClassInit_AccumulateFactionMetricAndDetachRootChild1 (ModelDefinitionRecordPrefix *modelDefinition,ModelRuntimeSlot *modelRuntimeSlot);

/* 0x005252E0 */
void ModelRuntimeSlotClassInit_AddFactionEnergyGenerationCapacity (ModelDefinitionRecordPrefix *modelDefinition,ModelRuntimeSlot *modelRuntimeSlot);

/* 0x005254B0 */
void ModelRuntimeSlotClassRelease_SubtractFactionEnergyGenerationCapacity (ModelDefinitionRecordPrefix *modelDefinition,ModelRuntimeSlot *modelRuntime);

/* 0x00526340 */
void ModelRuntimeSlot_UnrebaseClassModelLinkOffset60(ModelRuntimeSlot *modelRuntime);

/* 0x00526370 */
void ModelRuntimeSlot_RebaseClassModelLinkOffset60(ModelRuntimeSlot *modelRuntimeSlot);

/* 0x005263A0 */
void ModelRuntimeSlotClassInit_ClearStateAndSetRootChild0Offset (ModelDefinitionRecordPrefix *modelDefinition,ModelRuntimeSlot *modelRuntimeSlot);

/* 0x00526E00 */
void ModelRuntimeSlotClassInit_ClearExtendedStateAndEnableRootAnimation (ModelDefinitionRecordPrefix *modelDefinition,ModelRuntimeSlot *modelRuntimeSlot);

/* 0x00527B80 */
void ModelRuntimeSlotPointerRebase_NoOp(ModelRuntimeSlot *modelRuntimeSlot);

/* 0x00527B90 */
void ModelRuntimeSlotClassInit_NoOp (ModelDefinitionRecordPrefix *modelDefinition,ModelRuntimeSlot *modelRuntimeSlot);

/* 0x005283B0 */
void ModelRuntimeSlotClassInit_ClearField60 (ModelDefinitionRecordPrefix *modelDefinition,ModelRuntimeSlot *modelRuntimeSlot);

/* 0x005285D0 */
void ModelRuntimeSlotClassInit_ClearFields60AndB8 (ModelDefinitionRecordPrefix *modelDefinition,ModelRuntimeSlot *modelRuntimeSlot);

#endif /* THANDOR_WORLD_MODEL_SLOTS_H */

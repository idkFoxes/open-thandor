#ifndef THANDOR_WORLD_MODEL_RUNTIME_H
#define THANDOR_WORLD_MODEL_RUNTIME_H

#include <thandor/generated/v523_types.h>
#include <thandor/core/contracts.h>

/* Submodule: world/model/runtime. */
/* Functions are grouped by semantic ownership; address comments are executable virtual addresses. */

/* 0x00529360 */
void __fastcall ModelRuntimePool_RepairDeferredChild (undefined4 param_1,undefined4 param_2,GraphicsPaletteAsset *paletteAsset, GraphicsTextureSet *textureSet,ModelRuntimeAttachmentIndex attachmentIndex, PckModelDefinitionIdCatalog childDefinitionId,ModelRuntimeSlot *modelRuntime, WorldRuntimeContext *worldRuntime);

/* 0x004BDDB0 */
undefined8 ModelRuntime_CullAndRenderHierarchyRecursive(ModelRuntimeNode *modelNodeRuntime);

/* 0x004BE270 */
undefined8 ModelRuntime_RenderHierarchyRecursiveAlternatePath(ModelRuntimeNode *param_1);

/* 0x0050B440 */
ModelRaycastNearestHitCfRegisterResult ModelRuntime_RaycastCandidateListNearestCf (AngleTurn32 elevationAngle,AngleTurn32 azimuthAngle,Q12 maximumDistanceQ12,Q12 originZQ12 ,Q12 originYQ12,Q12 originXQ12,ModelRuntimeClassId requiredOwnerId, ModelRuntimeNode *excludedNode,WorldRuntimeContext *worldRuntime);

/* 0x0051C240 */
undefined8 __fastcall ModelRuntime_QueryHierarchyScaleRatioQ12(undefined4 param_1,undefined4 param_2,undefined4 *param_3);

/* 0x0051C260 */
ModelRuntimeScaleRatioRegisterPairQ12 ModelRuntime_QueryHierarchyScaleRatioQ12Regs(ArmyRuntimeSlot *modelRuntimeHolder);

/* 0x0051C280 */
int ModelRuntime_QueryActiveHierarchyMetric(ArmyRuntimeSlot *modelRuntimeHolder);

/* 0x0051C2A0 */
ModelRuntimeActiveTotalMetricRegisterPair ModelRuntime_QueryActiveAndTotalHierarchyMetricsRegs(ArmyRuntimeSlot *modelRuntimeHolder);

/* 0x00528A40 */
void __cdecl ModelRuntimePool_Init(void);

/* 0x00528A70 */
void __cdecl ModelRuntimePool_ShutdownAndReleaseDefinitions(void);

/* 0x00528B30 */
void __cdecl ModelRuntimePool_UnrebaseBeforeSave(void);

/* 0x00528CF0 */
void __cdecl ModelRuntimePool_RebaseAfterLoad(void);

/* 0x00529560 */
void ModelRuntimePool_DestroyHierarchyAndDetach (WorldRuntimeContext *worldRuntime,ModelRuntimeSlot *modelRuntime);

/* 0x00529690 */
void ModelRuntime_EmitProjectilesFromAttachmentPoints (ShotRuntimeState14 shotRuntimeState14,Q12 targetWorldZQ12,Q12 targetWorldYQ12, Q12 targetWorldXQ12,ShotDefinition *shotDefinition,ModelRuntimeNode *modelNodeRuntime ,MdlSerializedNodeHeader38 *definitionNode,WorldRuntimeContext *worldRuntime);

/* 0x00529140 */
undefined8 ModelRuntimePool_CreateInstanceByDefinitionIdCf (GraphicsPaletteAsset *paletteAsset,GraphicsTextureSet *textureSet, ArmyRuntimeSlot *armyRuntime,PckModelDefinitionIdCatalog modelDefinitionId, WorldRuntimeContext *worldRuntime);

#endif /* THANDOR_WORLD_MODEL_RUNTIME_H */
